//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.03
//      Bug Fixed:  Fix NMI button can't work issue.
//      Reason:     BIOS doesn't clear NMI status in SMI and cause enter SMI repeatedly.
//      Auditor:    William Hsu
//      Date:       Feb/23/2017
//
//  Rev. 1.02
//      Bug Fixed:  Add NVDIMM/CPLD function
//      Reason:     
//      Auditor:    Leon Xu
//      Date:       Feb/17/2017
//
//  Rev. 1.01
//    Bug Fix:  Fixed Power Button 4 sec shutdown
//    Reason:   
//    Auditor:  Sunny Yang
//    Date:     Jun/15/2016
//
//  Rev. 1.00
//    Bug Fix:  Fix front panel NMI doesn't working.
//    Reason:   For GPI_NMI and BMC_NMI.
//    Auditor:  Jimmy Chiu (Refer Greenlow)
//    Date:     Jun/13/2016
//
//****************************************************************************
/** @file
  This driver is responsible for the registration of child drivers
  and the abstraction of the PCH SMI sources.

@copyright
 Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#include "PchSmm.h"
#include "PchSmmHelpers.h"
#include "PchSmmEspi.h"
#include "Token.h"
#if SMCPKG_SUPPORT
#include <Library/PeiDxeSmmGpioLib/GpioLibrary.h>
#include <Register/PchRegsPmc.h>
#endif

//
// MODULE / GLOBAL DATA
//
// Module variables used by the both the main dispatcher and the source dispatchers
// Declared in PchSmm.h
//
GLOBAL_REMOVE_IF_UNREFERENCED UINT16                mAcpiBaseAddr;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16                mTcoBaseAddr;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN               mReadyToLock;

GLOBAL_REMOVE_IF_UNREFERENCED PRIVATE_DATA          mPrivateData = {
  {
    NULL,
    NULL
  },                                    ///< CallbackDataBase linked list head
  NULL,                                 ///< EFI handle returned when calling InstallMultipleProtocolInterfaces
  NULL,                                 //
  {                                     ///< protocol arrays
    ///
    /// elements within the array
    ///
    {
      PROTOCOL_SIGNATURE,
      UsbType,
      &gEfiSmmUsbDispatch2ProtocolGuid,
      {{
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister,
      }}
    },
    {
      PROTOCOL_SIGNATURE,
      SxType,
      &gEfiSmmSxDispatch2ProtocolGuid,
      {{
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister
      }}
    },
    {
      PROTOCOL_SIGNATURE,
      SwType,
      &gEfiSmmSwDispatch2ProtocolGuid,
      {{
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister,
        (UINTN) MAXIMUM_SWI_VALUE
      }}
    },
    {
      PROTOCOL_SIGNATURE,
      GpiType,
      &gEfiSmmGpiDispatch2ProtocolGuid,
      {{
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister,
        (UINTN) V_PCH_GPIO_NUM_SUPPORTED_GPIS
      }}
    },
    {
      PROTOCOL_SIGNATURE,
      PowerButtonType,
      &gEfiSmmPowerButtonDispatch2ProtocolGuid,
      {{
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister
      }}
    },
    {
      PROTOCOL_SIGNATURE,
      PeriodicTimerType,
      &gEfiSmmPeriodicTimerDispatch2ProtocolGuid,
      {{
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister,
        (UINTN) PchSmmPeriodicTimerDispatchGetNextShorterInterval
      }}
    },
  }
};

GLOBAL_REMOVE_IF_UNREFERENCED CONTEXT_FUNCTIONS     mContextFunctions[PCH_SMM_PROTOCOL_TYPE_MAX] = {
  {
    NULL,
    NULL,
    NULL
  },
  {
    SxGetContext,
    SxCmpContext,
    NULL
  },
  {
    SwGetContext,
    SwCmpContext,
    SwGetCommBuffer
  },
  {
    NULL,
    NULL,
    NULL
  },
  {
    PowerButtonGetContext,
    PowerButtonCmpContext,
    NULL
  },
  {
    PeriodicTimerGetContext,
    PeriodicTimerCmpContext,
    PeriodicTimerGetCommBuffer
  },
};

//
// PROTOTYPES
//
// Functions use only in this file
//
EFI_STATUS
EFIAPI
PchSmmCoreDispatcher (
  IN EFI_HANDLE             SmmImageHandle,
  IN     CONST VOID               *PchSmmCore,     OPTIONAL
  IN OUT VOID               *CommunicationBuffer,
  IN OUT UINTN              *SourceSize
  );

//
// FUNCTIONS
//
/**
  SMM ready to lock notification event handler.

  @param  Protocol   Points to the protocol's unique identifier
  @param  Interface  Points to the interface instance
  @param  Handle     The handle on which the interface was installed

  @retval EFI_SUCCESS   SmmReadyToLockCallback runs successfully

**/
EFI_STATUS
EFIAPI
SmmReadyToLockCallback (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  mReadyToLock = TRUE;


  return EFI_SUCCESS;
}

/**
  <b>PchSmiDispatcher SMM Module Entry Point</b>\n
  - <b>Introduction</b>\n
    The PchSmiDispatcher module is an SMM driver which  provides SMI handler registration
    services for PCH generated SMIs.

  - <b>Details</b>\n
    This module provides SMI handler registration servicies for PCH SMIs.
    NOTE: All the register/unregister functions will be locked after SMM END OF DXE signal event.
    Please make sure no handler is installed after that.

  - @pre
    - EFI_SMM_BASE2_PROTOCOL
      - Documented in the System Management Mode Core Interface Specification
    - EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL
      - Documented in the UEFI 2.0 Specification and above
    - EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
      - This is to ensure that PCI MMIO and IO resource has been prepared and available for this driver to allocate.
    - EFI_SMM_CPU_PROTOCOL

  - @result
    The PchSmiDispatcher driver produces:
    - EFI_SMM_USB_DISPATCH2_PROTOCOL
    - EFI_SMM_SX_DISPATCH2_PROTOCOL
    - EFI_SMM_SW_DISPATCH2_PROTOCOL
    - EFI_SMM_GPI_DISPATCH2_PROTOCOL
    - EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL
    - EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL
    - EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL
    - @link _PCH_SMM_IO_TRAP_CONTROL_PROTOCOL PCH_SMM_IO_TRAP_CONTROL_PROTOCOL @endlink
    - @link _PCH_PCIE_SMI_DISPATCH_PROTOCOL PCH_PCIE_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_TCO_SMI_DISPATCH_PROTOCOL PCH_TCO_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_ACPI_SMI_DISPATCH_PROTOCOL PCH_ACPI_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_GPIO_UNLOCK_SMI_DISPATCH_PROTOCOL PCH_GPIO_UNLOCK_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_SMI_DISPATCH_PROTOCOL PCH_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_ESPI_SMI_DISPATCH_PROTOCOL PCH_ESPI_SMI_DISPATCH_PROTOCOL @endlink

  @param[in] ImageHandle          Pointer to the loaded image protocol for this driver
  @param[in] SystemTable          Pointer to the EFI System Table

  @retval EFI_SUCCESS             PchSmmDispatcher Initialization completed.
**/
EFI_STATUS
EFIAPI
InitializePchSmmDispatcher (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINTN       LpcBaseAddress;
  EFI_STATUS  Status;
  VOID        *SmmReadyToLockRegistration;

  ///
  /// Access ACPI Base Addresses Register
  ///
  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );

  PchAcpiBaseGet (&mAcpiBaseAddr);
  ASSERT (mAcpiBaseAddr != 0);

  ///
  /// Access TCO Base Addresses Register
  ///
  PchTcoBaseGet (&mTcoBaseAddr);
  ASSERT (mTcoBaseAddr != 0);


  PchSwDispatchInit ();

  ///
  /// Register a callback function to handle subsequent SMIs.  This callback
  /// will be called by SmmCoreDispatcher.
  ///
  Status = gSmst->SmiHandlerRegister (PchSmmCoreDispatcher, NULL, &mPrivateData.SmiHandle);
  ASSERT_EFI_ERROR (Status);
  ///
  /// Initialize Callback DataBase
  ///
  InitializeListHead (&mPrivateData.CallbackDataBase);

  ///
  /// Enable SMIs on the PCH now that we have a callback
  ///
  PchSmmInitHardware ();

  //
  // Install and initialize all the needed protocols
  //
  PchSmmPublishDispatchProtocols ();
  InstallPchSmiDispatchProtocols ();
  InstallIoTrap (ImageHandle);
  InstallEspiSmi (ImageHandle);
  InstallPchSmmPeriodicTimerControlProtocol (mPrivateData.InstallMultProtHandle);

  //
  // Register EFI_SMM_READY_TO_LOCK_PROTOCOL_GUID notify function.
  //
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmReadyToLockProtocolGuid,
                    SmmReadyToLockCallback,
                    &SmmReadyToLockRegistration
                    );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  Check the Fed SwSmiInputValue to see if there is a duplicated one in the database

  @param[in] FedSwSmiInputValue   Fed SwSmiInputValue

  @retval EFI_SUCCESS             There is no duplicated SwSmiInputValue
  @retval EFI_INVALID_PARAMETER   There is a duplicated SwSmiInputValue
**/
EFI_STATUS
SmiInputValueDuplicateCheck (
  UINTN           FedSwSmiInputValue
  )
{

  DATABASE_RECORD *RecordInDb;
  LIST_ENTRY      *LinkInDb;

  LinkInDb = GetFirstNode (&mPrivateData.CallbackDataBase);
  while (!IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
    RecordInDb = DATABASE_RECORD_FROM_LINK (LinkInDb);

    if (RecordInDb->ProtocolType == SwType) {
      if (RecordInDb->ChildContext.Sw.SwSmiInputValue == FedSwSmiInputValue) {
        return EFI_INVALID_PARAMETER;
      }
    }

    LinkInDb = GetNextNode (&mPrivateData.CallbackDataBase, &RecordInDb->Link);
  }

  return EFI_SUCCESS;
}

/**
  Register a child SMI dispatch function with a parent SMM driver.

  @param[in] This                 Pointer to the PCH_SMM_GENERIC_PROTOCOL instance.
  @param[in] DispatchFunction     Pointer to dispatch function to be invoked for this SMI source.
  @param[in] DispatchContext      Pointer to the dispatch function's context.
  @param[out] DispatchHandle      Handle of dispatch function, for when interfacing
                                  with the parent SMM driver, will be the address of linked
                                  list link in the call back record.

  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create database record
  @retval EFI_INVALID_PARAMETER   The input parameter is invalid
  @retval EFI_SUCCESS             The dispatch function has been successfully
                                  registered and the SMI source has been enabled.
**/
EFI_STATUS
PchSmmCoreRegister (
  IN  PCH_SMM_GENERIC_PROTOCOL                          *This,
    IN  EFI_SMM_HANDLER_ENTRY_POINT2                      DispatchFunction,
  IN  PCH_SMM_CONTEXT                                   *DispatchContext,
  OUT EFI_HANDLE                                        *DispatchHandle
  )
{
  EFI_STATUS                  Status;
  DATABASE_RECORD             *Record;
  PCH_SMM_QUALIFIED_PROTOCOL  *Qualified;
  PCH_SMM_SOURCE_DESC         NullSourceDesc = NULL_SOURCE_DESC_INITIALIZER;
  UINTN                       Index;
  GPIO_PAD                    GpioPad;
  UINT8                       GpiSmiBitOffset;
  UINT32                      GpiSmiEnRegAddress;
  UINT32                      GpiSmiStsRegAddress;
  UINT32                      Data32Or;
  UINT32                      Data32And;


  Index     = 0;
  ///
  /// Create database record and add to database
  ///

  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (DATABASE_RECORD), (VOID **) &Record);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (Record, sizeof (DATABASE_RECORD));

  ///
  /// Gather information about the registration request
  ///
  Record->Callback          = DispatchFunction;

  Record->ChildContext      = *DispatchContext;

  Qualified                 = QUALIFIED_PROTOCOL_FROM_GENERIC (This);

  Record->ProtocolType      = Qualified->Type;

  Record->ContextFunctions  = mContextFunctions[Qualified->Type];
  ///
  /// Perform linked list housekeeping
  ///
  Record->Signature = DATABASE_RECORD_SIGNATURE;

  switch (Qualified->Type) {
  ///
  /// By the end of this switch statement, we'll know the
  /// source description the child is registering for
  ///
  case UsbType:
    ///
    /// Check the validity of Context Type
    ///
    if ((Record->ChildContext.Usb.Type < UsbLegacy) || (Record->ChildContext.Usb.Type > UsbWake)) {
      goto Error;
    }

    MapUsbToSrcDesc (DispatchContext, &(Record->SrcDesc));
    Record->ClearSource = NULL;
    ///
    /// use default clear source function
    ///
    break;

  case SxType:
    ///
    /// Check the validity of Context Type and Phase
    ///
    if ((Record->ChildContext.Sx.Type < SxS0) ||
        (Record->ChildContext.Sx.Type >= EfiMaximumSleepType) ||
        (Record->ChildContext.Sx.Phase < SxEntry) ||
        (Record->ChildContext.Sx.Phase >= EfiMaximumPhase)
        ) {
      goto Error;
    }

    CopyMem ((VOID *) &(Record->SrcDesc), (VOID *) (&SX_SOURCE_DESC), sizeof (PCH_SMM_SOURCE_DESC));
    Record->ClearSource = NULL;
    ///
    /// use default clear source function
    ///
    break;

  case SwType:
    ///
    /// Check the validity of Context Value
    ///
  if (Record->ChildContext.Sw.SwSmiInputValue  == (UINTN)-1) {
    for (Index = 1; Index < MAXIMUM_SWI_VALUE; Index++) {
      if (!EFI_ERROR (SmiInputValueDuplicateCheck (Index))) {
        Record->ChildContext.Sw.SwSmiInputValue = Index;
        break;
      }
    }
    if (Record->ChildContext.Sw.SwSmiInputValue == (UINTN)-1) {
      goto Error;
    }
  }
    if (Record->ChildContext.Sw.SwSmiInputValue > MAXIMUM_SWI_VALUE) {
      goto Error;
    }

    if (EFI_ERROR (SmiInputValueDuplicateCheck (Record->ChildContext.Sw.SwSmiInputValue))) {
      goto Error;
    }

    CopyMem ((VOID *) &(Record->SrcDesc), (VOID *) (&SW_SOURCE_DESC), sizeof (PCH_SMM_SOURCE_DESC));
    Record->ClearSource = NULL;
    ///
    /// use default clear source function
    ///
    break;

  case GpiType:
    Index = Record->ChildContext.Gpi.GpiNum;

    Status = GpioGetPadAndSmiRegs (
      (UINT32)Index,
      &GpioPad,
      &GpiSmiBitOffset,
      &GpiSmiEnRegAddress,
      &GpiSmiStsRegAddress
      );

    if (EFI_ERROR (Status)) {
      goto Error;
    }

    CopyMem (
      (VOID *) &(Record->SrcDesc),
      (VOID *) &(PCH_GPI_SOURCE_DESC_TEMPLATE),
      sizeof (PCH_SMM_SOURCE_DESC)
      );
    Record->SrcDesc.En[0].Reg.Data.raw = GpiSmiEnRegAddress;    // GPI SMI Enable register
    Record->SrcDesc.En[0].Bit = GpiSmiBitOffset;                // Bit position for selected pad
    Record->SrcDesc.Sts[0].Reg.Data.raw = GpiSmiStsRegAddress;  // GPI SMI Status register
    Record->SrcDesc.Sts[0].Bit = GpiSmiBitOffset;               // Bit position for selected pad
    Record->ClearSource = NULL;
    ///
    /// use default clear source function
    ///
    ///
    /// Add GPI_SMI_EN programming into S3 boot script
    ///
    Data32Or  = (1u << GpiSmiBitOffset);
    Data32And = 0xFFFFFFFF;
    S3BootScriptSaveMemReadWrite (S3BootScriptWidthUint32, GpiSmiEnRegAddress, &Data32Or, &Data32And);

    break;

  case PowerButtonType:
    ///
    /// Check the validity of Context Phase
    ///
    if ((Record->ChildContext.PowerButton.Phase < EfiPowerButtonEntry) ||
        (Record->ChildContext.PowerButton.Phase > EfiPowerButtonExit)
       )
    {
      goto Error;
    }

    CopyMem ((VOID *) &(Record->SrcDesc), (VOID *) &POWER_BUTTON_SOURCE_DESC, sizeof (PCH_SMM_SOURCE_DESC));
    Record->ClearSource = NULL;
    ///
    /// use default clear source function
    ///
    break;

  case PeriodicTimerType:
    ///
    /// Check the validity of timer value
    ///
    if (DispatchContext->PeriodicTimer.SmiTickInterval <= 0) {
      goto Error;
    }

    MapPeriodicTimerToSrcDesc (DispatchContext, &(Record->SrcDesc));
    Record->ClearSource = PchSmmPeriodicTimerClearSource;
    break;

  default:
    goto Error;
    break;
  }

  if (CompareSources (&Record->SrcDesc, &NullSourceDesc)) {
    goto Error;
  }

  ///
  /// After ensuring the source of event is not null, we will insert the record into the database
  ///
  InsertTailList (&mPrivateData.CallbackDataBase, &Record->Link);

  if (Record->ClearSource == NULL) {
    ///
    /// Clear the SMI associated w/ the source using the default function
    ///
    PchSmmClearSource (&Record->SrcDesc);
  } else {
    ///
    /// This source requires special handling to clear
    ///
    Record->ClearSource (&Record->SrcDesc);
  }

  PchSmmEnableSource (&Record->SrcDesc);

  ///
  /// Child's handle will be the address linked list link in the record
  ///
  *DispatchHandle = (EFI_HANDLE) (&Record->Link);
  *DispatchContext = Record->ChildContext;

  return EFI_SUCCESS;

Error:
  Status = gSmst->SmmFreePool (Record);
  ///
  /// DEBUG((DEBUG_ERROR,"Free pool status %d\n", Status ));
  ///
  return EFI_INVALID_PARAMETER;
}

/**
  Unregister a child SMI source dispatch function with a parent SMM driver.

  @param[in] This                 Pointer to the PCH_SMM_GENERIC_PROTOCOL instance.
  @param[in] DispatchHandle       Handle of dispatch function to deregister.

  @retval EFI_SUCCESS             The dispatch function has been successfully
                                  unregistered and the SMI source has been disabled
                                  if there are no other registered child dispatch
                                  functions for this SMI source.
  @retval EFI_INVALID_PARAMETER   Handle is invalid.
**/
EFI_STATUS
PchSmmCoreUnRegister (
  IN PCH_SMM_GENERIC_PROTOCOL                           *This,
  IN EFI_HANDLE                                         *DispatchHandle
  )
{
  EFI_STATUS      Status;
  BOOLEAN         NeedClearEnable;
  UINTN           DescIndex;
  DATABASE_RECORD *RecordToDelete;
  DATABASE_RECORD *RecordInDb;
  LIST_ENTRY      *LinkInDb;


  if (DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }



  RecordToDelete = DATABASE_RECORD_FROM_LINK (DispatchHandle);

  //
  // Take the entry out of the linked list
  //
  if (RecordToDelete->Link.ForwardLink == (LIST_ENTRY *) EFI_BAD_POINTER) {
    return EFI_INVALID_PARAMETER;
  }

  RemoveEntryList (&RecordToDelete->Link);

  //
  // Loop through all the souces in record linked list to see if any source enable is equal.
  // If any source enable is equal, we do not want to disable it.
  //
  for (DescIndex = 0; DescIndex < NUM_EN_BITS; ++DescIndex) {
    if (IS_BIT_DESC_NULL (RecordToDelete->SrcDesc.En[DescIndex])) {
      continue;
  }
    NeedClearEnable = TRUE;
  LinkInDb = GetFirstNode (&mPrivateData.CallbackDataBase);
  while (!IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
    RecordInDb = DATABASE_RECORD_FROM_LINK (LinkInDb);
      if (IsBitEqualToAnySourceEn (&RecordToDelete->SrcDesc.En[DescIndex], &RecordInDb->SrcDesc)) {
        NeedClearEnable = FALSE;
        break;
      }
      LinkInDb = GetNextNode (&mPrivateData.CallbackDataBase, &RecordInDb->Link);
    }
    if (NeedClearEnable == FALSE) {
      continue;
  }
      WriteBitDesc (&RecordToDelete->SrcDesc.En[DescIndex], 0, FALSE);
    }

  Status = gSmst->SmmFreePool (RecordToDelete);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return Status;
  }
  return EFI_SUCCESS;
}

/**
  The callback function to handle subsequent SMIs.  This callback will be called by SmmCoreDispatcher.

  @param[in] SmmImageHandle             Not used
  @param[in] PchSmmCore                 Not used
  @param[in, out] CommunicationBuffer   Not used
  @param[in, out] SourceSize            Not used

  @retval EFI_SUCCESS                   Function successfully completed
**/
EFI_STATUS
EFIAPI
PchSmmCoreDispatcher (
  IN EFI_HANDLE             SmmImageHandle,
  IN     CONST VOID               *PchSmmCore,
  IN OUT VOID               *CommunicationBuffer,
  IN OUT UINTN              *SourceSize
  )
{
  ///
  /// Used to prevent infinite loops
  ///
  UINTN               EscapeCount;

  BOOLEAN             ContextsMatch;
  BOOLEAN             EosSet;
  BOOLEAN             SxChildWasDispatched;
  // APTIOV_SERVER_OVERRIDE_RC_START : Multiple Power button handler support
  BOOLEAN             PbtnChildWasDispatched = FALSE;
  // APTIOV_SERVER_OVERRIDE_RC_END : Multiple Power button handler support

  DATABASE_RECORD     *RecordInDb;
  LIST_ENTRY          *LinkInDb;
  DATABASE_RECORD     *RecordToExhaust;
  LIST_ENTRY          *LinkToExhaust;

  PCH_SMM_CONTEXT     Context;
  VOID                *CommBuffer;
  UINTN               CommBufferSize;

  EFI_STATUS          Status;
  BOOLEAN             SciEn;
  UINT32              SmiEnValue;
  UINT32              SmiStsValue;
  UINT8               Port74Save;
  UINT8               Port76Save;
#if SMCPKG_SUPPORT
  UINT16              TCO1_CNT;
  GPIO_GROUP_INFO     *GpioGroupInfo;
  UINT32              GroupIndex;
  UINTN               GpioGroupInfoLength;
#endif  

  PCH_SMM_SOURCE_DESC ActiveSource = NULL_SOURCE_DESC_INITIALIZER;

  EscapeCount           = 3;
  ContextsMatch         = FALSE;
  EosSet                = FALSE;
  SxChildWasDispatched  = FALSE;
  Status                = EFI_SUCCESS;

  //
  // Save IO index registers
  // @note: Save/Restore port 70h directly might break NMI_EN# setting,
  //       then save/restore 74h/76h instead.
  // @note: CF8 is not saved. Prefer method is to use MMIO instead of CF8
  //
  Port76Save = IoRead8 (R_PCH_RTC_EXT_INDEX_ALT);
  Port74Save = IoRead8 (R_PCH_RTC_INDEX_ALT);

#if SMCPKG_SUPPORT
#if defined SMC_EXT_NMI_GPIO && SMC_EXT_NMI_GPIO != 0xFF
  //
  // Check if NMI_STS asserted
  //
  if(SMC_EXT_NMI_GPIO){
      GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);
      GroupIndex = GpioGetGroupIndexFromGpioPad (SMC_EXT_NMI_GPIO);

      //Check GPI_NMI_STS 
      if ((MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].NmiStsOffset))) & (1 <<(SMC_EXT_NMI_GPIO&0xFFFF))) {
    
          //Disable NMI2SMI_EN for prevent NMI trigger again before service.
          TCO1_CNT = IoRead16 (TCO_BASE_ADDRESS + R_PCH_TCO1_CNT);
          TCO1_CNT = TCO1_CNT & (~B_PCH_TCO_CNT_NMI2SMI_EN);
          IoWrite16 (TCO_BASE_ADDRESS + R_PCH_TCO1_CNT, TCO1_CNT);  
	  
          // Clear GPI_NMI_STS Status
          MmioWrite32 (
            PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].NmiStsOffset),
            1 << (SMC_EXT_NMI_GPIO&0xFFFF)
          );

          //Enable NMI2SMI_EN for next NMI trigger 
          TCO1_CNT = TCO1_CNT | B_PCH_TCO_CNT_NMI2SMI_EN;
          IoWrite16 (TCO_BASE_ADDRESS + R_PCH_TCO1_CNT, TCO1_CNT);  
      }  
  }
#endif

#if defined SMC_BMC_NMI_GPIO && SMC_BMC_NMI_GPIO != 0xFF
  //
  // Check if NMI_STS asserted
  //
      GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);
      GroupIndex = GpioGetGroupIndexFromGpioPad (SMC_BMC_NMI_GPIO);

      //Check GPI_NMI_STS 
      if ((MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].NmiStsOffset))) & (1 <<(SMC_BMC_NMI_GPIO&0xFFFF))) {
 	      
          //Disable NMI2SMI_EN for prevent NMI trigger again before service.
          TCO1_CNT = IoRead16 (TCO_BASE_ADDRESS + R_PCH_TCO1_CNT);
          TCO1_CNT = TCO1_CNT & (~B_PCH_TCO_CNT_NMI2SMI_EN);
          IoWrite16 (TCO_BASE_ADDRESS + R_PCH_TCO1_CNT, TCO1_CNT);  
	  
          // Clear GPI_NMI_STS Status
          MmioWrite32 (
            PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].NmiStsOffset),
            1 << (SMC_BMC_NMI_GPIO&0xFFFF)
          );

          //Enable NMI2SMI_EN for next NMI trigger 
          TCO1_CNT = TCO1_CNT | B_PCH_TCO_CNT_NMI2SMI_EN;
          IoWrite16 (TCO_BASE_ADDRESS + R_PCH_TCO1_CNT, TCO1_CNT);  
      }    
#endif

#if defined SMC_CPLD_NMI_GPIO && SMC_CPLD_NMI_GPIO != 0xFF
  //
  // Check if NMI_STS asserted
  //
      GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);
      GroupIndex = GpioGetGroupIndexFromGpioPad (SMC_CPLD_NMI_GPIO);

      //Check GPI_NMI_STS 
      if ((MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].NmiStsOffset))) & (1 <<(SMC_CPLD_NMI_GPIO&0xFFFF))) {
          //Disable NMI2SMI_EN for prevent NMI trigger again before service.
          TCO1_CNT = IoRead16 (TCO_BASE_ADDRESS + R_PCH_TCO1_CNT);
          TCO1_CNT = TCO1_CNT & (~B_PCH_TCO_CNT_NMI2SMI_EN);
          IoWrite16 (TCO_BASE_ADDRESS + R_PCH_TCO1_CNT, TCO1_CNT);  
	  
          // Clear GPI_NMI_STS Status
          MmioWrite32 (
            PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].NmiStsOffset),
            1 << (SMC_CPLD_NMI_GPIO&0xFFFF)
          );

          //Enable NMI2SMI_EN for next NMI trigger 
          TCO1_CNT = TCO1_CNT | B_PCH_TCO_CNT_NMI2SMI_EN;
          IoWrite16 (TCO_BASE_ADDRESS + R_PCH_TCO1_CNT, TCO1_CNT);  
      }    
#endif


#endif

  if (!IsListEmpty (&mPrivateData.CallbackDataBase)) {
    ///
    /// We have children registered w/ us -- continue
    ///
    while ((!EosSet) && (EscapeCount > 0)) {
      EscapeCount--;

      LinkInDb = GetFirstNode (&mPrivateData.CallbackDataBase);

      ///
      /// Cache SciEn, SmiEnValue and SmiStsValue to determine if source is active
      ///
      SciEn       = PchSmmGetSciEn ();
      SmiEnValue  = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_SMI_EN));
      SmiStsValue = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_SMI_STS));

      while (!IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
        RecordInDb = DATABASE_RECORD_FROM_LINK (LinkInDb);

        ///
        /// look for the first active source
        ///
        if (!SourceIsActive (&RecordInDb->SrcDesc, SciEn, SmiEnValue, SmiStsValue)) {
          ///
          /// Didn't find the source yet, keep looking
          ///
          LinkInDb = GetNextNode (&mPrivateData.CallbackDataBase, &RecordInDb->Link);

          ///
          /// if it's the last one, try to clear EOS
          ///
          if (IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
            if (SmiStsValue & B_PCH_SMI_STS_TCO) {
              PchTcoClearNewCenturySts (NULL);
            }
            EosSet = PchSmmSetAndCheckEos ();
          }
        } else {
          ///
          /// We found a source. If this is a sleep type, we have to go to
          /// appropriate sleep state anyway.No matter there is sleep child or not
          ///
          if (RecordInDb->ProtocolType == SxType) {
            SxChildWasDispatched = TRUE;
          }
   // APTIOV_SERVER_OVERRIDE_RC_START : Multiple Power button handler support
          if (RecordInDb->ProtocolType == PowerButtonType) {
#if SMCPKG_SUPPORT == 0
            PbtnChildWasDispatched = TRUE;
#endif
          }
   // APTIOV_SERVER_OVERRIDE_RC_END : Multiple Power button handler support
          ///
          /// "cache" the source description and don't query I/O anymore
          ///
          CopyMem ((VOID *) &ActiveSource, (VOID *) &(RecordInDb->SrcDesc), sizeof (PCH_SMM_SOURCE_DESC));
          LinkToExhaust = LinkInDb;

          ///
          /// exhaust the rest of the queue looking for the same source
          ///
          while (!IsNull (&mPrivateData.CallbackDataBase, LinkToExhaust)) {
            RecordToExhaust = DATABASE_RECORD_FROM_LINK (LinkToExhaust);
            ///
            /// RecordToExhaust->Link might be removed (unregistered) by Callback function, and then the
            /// system will hang in ASSERT() while calling GetNextNode().
            /// To prevent the issue, we need to get next record in DB here (before Callback function).
            ///
            LinkToExhaust = GetNextNode (&mPrivateData.CallbackDataBase, &RecordToExhaust->Link);

            if (CompareSources (&RecordToExhaust->SrcDesc, &ActiveSource)) {
              ///
              /// These source descriptions are equal, so this callback should be
              /// dispatched.
              ///
              if (RecordToExhaust->ContextFunctions.GetContext != NULL) {
                ///
                /// This child requires that we get a calling context from
                /// hardware and compare that context to the one supplied
                /// by the child.
                ///
                ASSERT (RecordToExhaust->ContextFunctions.CmpContext != NULL);

                ///
                /// Make sure contexts match before dispatching event to child
                ///
                RecordToExhaust->ContextFunctions.GetContext (RecordToExhaust, &Context);
                ContextsMatch = RecordToExhaust->ContextFunctions.CmpContext (&Context, &RecordToExhaust->ChildContext);

              } else {
                ///
                /// This child doesn't require any more calling context beyond what
                /// it supplied in registration.  Simply pass back what it gave us.
                ///
                Context       = RecordToExhaust->ChildContext;
                ContextsMatch = TRUE;
              }

              if (ContextsMatch) {
                if (RecordToExhaust->ProtocolType == PchSmiDispatchType) {
                  //
                  // For PCH SMI dispatch protocols
                  //
                  PchSmiTypeCallbackDispatcher (RecordToExhaust);
                } else {
                  //
                  // For EFI standard SMI dispatch protocols
                  //
                  if (RecordToExhaust->Callback != NULL) {
                    if (RecordToExhaust->ContextFunctions.GetCommBuffer != NULL) {
                      ///
                      /// This callback function needs CommBuffer and CommBufferSize.
                      /// Get those from child and then pass to callback function.
                      ///
                      RecordToExhaust->ContextFunctions.GetCommBuffer (RecordToExhaust, &CommBuffer, &CommBufferSize);
                    } else {
                      ///
                      /// Child doesn't support the CommBuffer and CommBufferSize.
                      /// Just pass NULL value to callback function.
                      ///
                      CommBuffer     = NULL;
                      CommBufferSize = 0;
                    }

                    PERF_START_EX (NULL, "SmmFunction", NULL, AsmReadTsc(), RecordToExhaust->ProtocolType);
                    RecordToExhaust->Callback ((EFI_HANDLE) & RecordToExhaust->Link, &Context, CommBuffer, &CommBufferSize);
                    PERF_END_EX (NULL, "SmmFunction", NULL, AsmReadTsc(), RecordToExhaust->ProtocolType);
                    if (RecordToExhaust->ProtocolType == SxType) {
                      SxChildWasDispatched = TRUE;
                    }
                } else {
                    ASSERT (FALSE);
                }
                }
              }
            }
          }

          if (RecordInDb->ClearSource == NULL) {
            ///
            /// Clear the SMI associated w/ the source using the default function
            ///
            PchSmmClearSource (&ActiveSource);
          } else {
            ///
            /// This source requires special handling to clear
            ///
            RecordInDb->ClearSource (&ActiveSource);
          }
          ///
          /// Also, try to clear EOS
          ///
          if (SmiStsValue & B_PCH_SMI_STS_TCO) {
            PchTcoClearNewCenturySts (NULL);
          }
          EosSet = PchSmmSetAndCheckEos ();
          ///
          /// Queue is empty, reset the search
          ///
          break;
        }
      }
    }
  }
  ///
  /// If you arrive here, there are two possible reasons:
  /// (1) you've got problems with clearing the SMI status bits in the
  /// ACPI table.  If you don't properly clear the SMI bits, then you won't be able to set the
  /// EOS bit.  If this happens too many times, the loop exits.
  /// (2) there was a SMM communicate for callback messages that was received prior
  /// to this driver.
  /// If there is an asynchronous SMI that occurs while processing the Callback, let
  /// all of the drivers (including this one) have an opportunity to scan for the SMI
  /// and handle it.
  /// If not, we don't want to exit and have the foreground app. clear EOS without letting
  /// these other sources get serviced.
  ///
  /// This assert is not valid with CSM legacy solution because it generates software SMI
  /// to test for legacy USB support presence.
  /// This may not be illegal, so we cannot assert at this time.
  ///
  ///  ASSERT (EscapeCount > 0);
  ///
   // APTIOV_SERVER_OVERRIDE_RC_START : Multiple Power button handler support
  if (PbtnChildWasDispatched) {
    IoOr32(mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT, V_PCH_ACPI_PM1_CNT_S5);
    IoOr32(mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT, B_PCH_ACPI_PM1_CNT_SLP_EN);
  }
  // APTIOV_SERVER_OVERRIDE_RC_END : Multiple Power button handler support
  if (SxChildWasDispatched) {
    ///
    /// A child of the SmmSxDispatch protocol was dispatched during this call;
    /// put the system to sleep.
    ///
    PchSmmSxGoToSleep ();
  }
  //
  // Restore IO index registers
  // @note: Save/Restore port 70h directly might break NMI_EN# setting,
  //       then save/restore 74h/76h instead.
  //
  IoWrite8 (R_PCH_RTC_EXT_INDEX_ALT, Port76Save);
  IoWrite8 (R_PCH_RTC_INDEX_ALT, Port74Save);

  return Status;
}
