//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2016, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//  
//  History
// 
//  
//  Rev. 1.00
//      Bug Fixed:  Fix cannot log SERR/PERR. 
//      Reason:     
//      Auditor:    Chen Lin
//      Date:       Sep/09/2016
// 
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c) 2009-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PCHErrorHandler.c

Abstract:

 Error handling for PCH
----------------------------------------------------------------------*/


//#include "CommonErrorHandlerDefs.h"

//#include <Library\PciExpressLib.h>
//#include <Library\IoLib.h>
#include <PchAccess.h>
#include <GpioPinsSklH.h>
#include <Library/PeiDxeSmmGpioLib/GpioLibrary.h>
#include <IncludePrivate/Library/GpioPrivateLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/MmPciBaseLib.h>
#include <PchCommonErrorHandler.h>
#include "CommonErrorHandlerDefs.h"
#include "PcieErrorHandler.h"
#include <Library/PciExpressLib.h>
#include <Library/IoLib.h>
#include <Token.h> //SMCPKG_SUPPORT


UINT8      mPchBusNum;

RAS_PCH_PCI_DEVICES  mPchPciDeviceTable[] = {
//
//   Segment        Bus                     Device                            Function                            Bridge Y/N
// -------------------------------------------------------------------------------------------------------------------------
  {00,            DEFAULT_PCI_BUS_NUMBER_PCH,  PCI_DEVICE_NUMBER_PCH_LPC,        PCI_FUNCTION_NUMBER_PCH_LPC,                FALSE },
  {00,            DEFAULT_PCI_BUS_NUMBER_PCH,  PCI_DEVICE_NUMBER_PCH_SATA,       PCI_FUNCTION_NUMBER_PCH_SATA,               FALSE },
  {00,            DEFAULT_PCI_BUS_NUMBER_PCH,  PCI_DEVICE_NUMBER_PCH_XHCI,       PCI_FUNCTION_NUMBER_PCH_XHCI,               FALSE },
  {00,            DEFAULT_PCI_BUS_NUMBER_PCH,  PCI_DEVICE_NUMBER_PCH_HDA,        PCI_FUNCTION_NUMBER_PCH_HDA,                FALSE },
  {00,            DEFAULT_PCI_BUS_NUMBER_PCH,  PCI_DEVICE_NUMBER_PCH_LAN,        PCI_FUNCTION_NUMBER_PCH_LAN,                FALSE },
//  {00,            DEFAULT_PCI_BUS_NUMBER_PCH,  PCI_DEVICE_NUMBER_PCH_PCI_PCI,    PCI_FUNCTION_NUMBER_PCH_PCI_PCI,            TRUE  },
  {INVALID_FLAG,  INVALID_FLAG,        INVALID_FLAG,                     INVALID_FLAG,                               FALSE }
};


//--------------------------------------------------------------------------------------------------------------------------//
//                                                                                                                          //
//                                               PCH Error generation Functions                                            //
//                                                                                                                          //
//--------------------------------------------------------------------------------------------------------------------------//

////////////////////////////////////// COMMON ROUTINES /////////////////////////////////////////

/**
  Function to clear DMI errors

  @param[in] Bus        Device bus number to check

  @retval None.

  **/
VOID
ClearDmiErrors (
  IN      UINT8   Bus
  )
{
  UINT32    Ues;
  UINT32    Ces;
  UINT32    Res;
  EFI_STATUS Status;

  Status = PchPcrRead32(PID_DMI,R_PCH_PCR_DMI_UES,&Ues);
  if(!EFI_ERROR(Status)) {
    PchPcrWrite32(PID_DMI,R_PCH_PCR_DMI_UES,Ues);
  }

  Status = PchPcrRead32(PID_DMI,R_PCH_PCR_DMI_CES,&Ces);
  if(!EFI_ERROR(Status)) {
    PchPcrWrite32(PID_DMI,R_PCH_PCR_DMI_CES,Ces);
  }

  Status = PchPcrRead32(PID_DMI,R_PCH_PCR_DMI_RES,&Res);
  if(!EFI_ERROR(Status)) {
    PchPcrWrite32(PID_DMI,R_PCH_PCR_DMI_RES,Res);
  }
}

/**
  Function to clear PCH errors

  @retval None.
**/
VOID
ClearPchErrors (
  VOID
  )
{
  ClearDmiErrors (DEFAULT_PCI_BUS_NUMBER_PCH);
}

////////////////////////////////////// ERROR HANDLING ROUTINES /////////////////////////////////////////

EFI_STATUS
GpiChildCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *mGpiContext,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{ 
  EFI_SMM_GPI_REGISTER_CONTEXT *Context;
  UINT32                       GpioGpiGpeStsValue;
  UINT32                       GpiNum;

  Context = (EFI_SMM_GPI_REGISTER_CONTEXT *)mGpiContext;

  // Read SMI_STS of PCH_CATERR_GPI_SMI_GPP_H community to see if has generated a SMI
  GpioGetReg (GPIO_SKL_H_GROUP_GPP_C, R_PCH_PCR_GPIO_GPP_C_SMI_STS, &GpioGpiGpeStsValue);
  GpiNum = (GpioGpiGpeStsValue & BIT23)? 1 : 0;
  //
  // caterr# connected GpiNum
  //
  if(GpiNum)   {
    GpioClearGpiSmiSts(GPIO_SKL_H_GPP_C23);
  }
  return EFI_SUCCESS;
}

/**
  Generate an NMI

  This function generates an NMI  to force an entry to the NMI handler..
  Please refer to Platform Controller Hub BIOS Specification: Routing NMI to SMI
  
  @retval None. 

**/
VOID
ElogGenerateNmiNow (
  VOID
  )
{
  UINT16     TcoBaseAddr;
  UINT8   Data;
  UINT8   Save_Nmi2Smi_En;
  UINT8   Save_Port70;
  UINT16  Data16;
  
  RASDEBUG ((DEBUG_INFO,"Generating NMI Now\n"));
  TcoBaseAddr = 0;

  //
  // Get the TCO Base Address
  // TcoBase was moved to SMBUS device in PCH
  //
  if(EFI_ERROR(PchTcoBaseGet(&TcoBaseAddr))) {
    return;  
  }
  //
  // Read the NMI2SMI_EN bit, save it for future restore
  //
  Save_Nmi2Smi_En = IoRead8 (TcoBaseAddr + R_PCH_TCO1_CNT + 1);
  
  //
  // Set the NMI2SMI_EN bit to 0
  //
  Data = Save_Nmi2Smi_En & 0xfd;
  IoWrite8 (TcoBaseAddr + R_PCH_TCO1_CNT + 1, Data);

  //
  // Enable NMI_EN
  //
  Save_Port70 = IoRead8 (0x74);
  Data = Save_Port70 & 0x7f;
  IoWrite8 (R_PCH_NMI_EN, Data);

  //
  // Set NMI_NOW = 1
  //
  Data = IoRead8 (TcoBaseAddr + R_PCH_TCO1_CNT + 1);
  Data |= 0x01;
  IoWrite8 (TcoBaseAddr + R_PCH_TCO1_CNT + 1, Data);

  //
  // Clear NMI_NOW = 0 by writing 1 to NMI_NOW bit
  //
  Data = IoRead8 (TcoBaseAddr + R_PCH_TCO1_CNT + 1);
  Data |= 0x01;
  IoWrite8 (TcoBaseAddr + R_PCH_TCO1_CNT + 1, Data);
  
  //
  // Restore NMI2SMI_EN
  //
  IoWrite8 (TcoBaseAddr + R_PCH_TCO1_CNT + 1, Save_Nmi2Smi_En);
  
  //
  // Clear the CPUSERR_STS/DMISERR_STS bit, bit 12
  // This bit is set to 1 if the CPU complex sends a
  //DMI special cycle message indicating that it wants to cause an SERR#.
  Data16 = IoRead16 (TcoBaseAddr + R_PCH_TCO1_STS);
  Data16 |= B_PCH_TCO1_STS_DMISERR;
  IoWrite16(TcoBaseAddr + R_PCH_TCO1_STS, Data16);
  
  //
  // Clear the NMI2SMI_STS bit if set
  //
  Data16 = IoRead16(TcoBaseAddr + R_PCH_TCO1_STS);
  if (Data16 & 0x0001) {
    //
    // check port 0x61
    //
    Data = IoRead8 (0x61); //NMI_STS_CNT
    if (Data & 0x80) {
      Data = IoRead8 (0x61);
      Data |= 0x04;
      Data &= 0x0f;
      IoWrite8 (0x61, Data);
      Data &= 0x0b;
      IoWrite8 (0x61, Data);
    }
  }
  //
  // Restore NMI_EN
  //
  IoWrite8 (R_PCH_NMI_EN, Save_Port70);
}

/**
  Notify the OS using PCH mechanisms

  This function takes the appropriate action for a particular error based on severity.

  @param[in] ErrorSeverity    The severity of the error to be handled

  @retval Status.

**/
EFI_STATUS
PchNotifyOs (
  IN      UINT8     ErrorSeverity,
     OUT  BOOLEAN   *OsNotified
  )
{
  switch (ErrorSeverity) {
    case EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED:
      break;
    case EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE:
    case EFI_ACPI_5_0_ERROR_SEVERITY_FATAL:
    #if SMCPKG_SUPPORT == 0	
      RASDEBUG ((DEBUG_INFO, "Sending OS notification via NMI\n"));
      ElogGenerateNmiNow ();
    #endif
      *OsNotified = TRUE;
      break;
    case EFI_ACPI_5_0_ERROR_SEVERITY_NONE:
      break;
    default:
      ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
      return EFI_INVALID_PARAMETER;
  }
  
  return EFI_SUCCESS;
}

/**
  Handler for PCH DMI

  @param[in] Bus        Device BUS number to check

  @retval EFI_SUCCESS. 

**/
EFI_STATUS
ElogPchDmiHandler (
  IN UINT8            Bus
  )
{
  UINT32    Cem;
  UINT32    Res;
  EFI_STATUS Status;
  UINT8     Device;
  UINT8     Function;

  RASDEBUG ((DEBUG_INFO,"ElogPchDmiHandler: Inside the function\n"));

  Device = 0;
  Function = 0;  
  
  Status  = PchPcrRead32(PID_DMI,R_PCH_PCR_DMI_RES,&Res);
  if(EFI_ERROR(Status)) {
    return Status;
  }

  if(Res & B_PCH_DMI_RES_CR) {
    //
    // Check if the correctable-error reporting limit has already been reached.
    //
    if (mNumCorrectablePcieErrs < PCIE_CORRECTABLE_ERROR_COUNT_LIMIT) {
      //
      // Report the correctable error.
      //
      PcieLogReportError (0, Bus, Device, Function, COR_ERROR_TYPE);
      mNumCorrectablePcieErrs++;
    } else {
      //
      // Mask all Correctable errors
      //
      Status  = PchPcrRead32(PID_DMI,R_PCH_PCR_DMI_CEM,&Cem);
      if(!EFI_ERROR(Status)) {
        Cem |= (B_PCH_DMI_CE_RE | B_PCH_DMI_CE_BT | B_PCH_DMI_CE_BD | B_PCH_DMI_CE_RNR);
        PchPcrWrite32(PID_DMI,R_PCH_PCR_DMI_CEM,Cem);
      }      
    }
  }

  if(Res & B_PCH_DMI_RES_ENR)  {
    //
    // Report the Uncorrectable error.
    //
    PcieLogReportError (0, Bus, Device, Function, FATAL_ERROR_TYPE);
  }
  
  return EFI_SUCCESS;
}

/**
  Handler for standard PCI to PCI bridge device.

  @param[in] Bus             Device's bus number 
  @param[in] Device          Device's device number
  @param[in] Function        Device's function number

  @retval ErrorDetected      Return BOOLEAN as Error is detected

**/
BOOLEAN
ElogP2PHandler (
  IN UINT8            Bus,
  IN UINT8            Device,
  IN UINT8            Function
  )
{
  UINT8           SecondaryBusNumber;
  UINT8           SubordinateBusNumber;
  UINT16          SecondaryStatus;
  BOOLEAN         ErrorDetected;
  UINT16          ErrorMap;

  //
  // Handle the primary bridge device
  //
  ErrorDetected = PciErrLibPciDeviceErrorHandler (0, Bus, Device, Function);

  //
  // Check the subordinate devices for errors
  //
  if (PciErrLibIsPciBridgeDevice (0, Bus, Device, Function)) {
    ErrorMap = EFI_PCI_STATUS_SIGNALED_SYSTEM_ERROR |
               EFI_PCI_STATUS_DETECTED_PARITY_ERROR |
               EFI_PCI_STATUS_DATA_PARITY_ERROR_DETECTED |
               EFI_PCI_STATUS_RECEIVED_TARGET_ABORT;
    
    SecondaryStatus = MmioRead16 (MmPciBase(Bus, Device, Function) + PCI_BRIDGE_STATUS_REGISTER_OFFSET);
    if (SecondaryStatus & ErrorMap) {
    //
    // Clear the secondary status bits...
    //
    PciErrLibClearLegacyPciDeviceStatus (0, Bus, Device, Function);

    SecondaryBusNumber = MmioRead8 (MmPciBase(Bus, Device, Function) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);

    SubordinateBusNumber = MmioRead8 (MmPciBase(Bus, Device, Function) + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);

      if (SecondaryBusNumber && SubordinateBusNumber) {
    for (Bus = SecondaryBusNumber; Bus <= SubordinateBusNumber; Bus++) {
      for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
        for (Function = 0; Function <= PCI_MAX_FUNC; Function++) {
          if (PciErrLibIsDevicePresent (0, Bus, Device, Function)) {
            if (PciErrLibPciDeviceErrorHandler (0, Bus, Device, Function)) {
              ErrorDetected = TRUE;
            }
              }
            }
          }
        }
      }
    }
  }
  return ErrorDetected;
}

/**
  This function checks the Root Error Status register to see if one of the enabled errors
  has occurred.

  @param[in] Bus            PCIe root port device's bus number.
  @param[in] Device         PCIe root port device's device number.
  @param[in] Function       PCIe root port device's function number. 
    
  @retval    BOOLEAN        TRUE   - An error was detected.
                            FALSE  - An error was not detected, or the device is not a root port.
--*/
BOOLEAN
PchIsErrOnPcieRootPort (
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT16    RootControl;
  UINT8     RootErrorStatus;
  BOOLEAN   IsError;

  IsError = FALSE;

  RootControl = MmioRead16 (MmPciBase (Bus, Device, Function) + R_PCH_PCIE_RCTL);

  RootErrorStatus = MmioRead8 (MmPciBase (Bus, Device, Function) + R_PCH_PCIE_RES);
  if (RootControl & (B_PCIE_ROOT_CONTROL_FAT_ERR_EN | B_PCIE_ROOT_CONTROL_NF_ERR_EN)) {
    if (RootErrorStatus & (B_PCH_PCIE_RES_ENR | B_PCH_PCIE_RES_MENR)) {
      IsError = TRUE;      
    }
  }
  if (RootControl & (B_PCIE_ROOT_CONTROL_COR_ERR_EN)) {
    if (RootErrorStatus & (B_PCH_PCIE_RES_CR | B_PCH_PCIE_RES_MCR)) {
      IsError = TRUE;      
    }
  }

  return IsError;
}

/**
  Check  and handle errors in the PCH Root port    
    
  @param[in] Bus             Device's bus number 
  @param[in] Device          Device's device number
  @param[in] Function        Device's function number

  @retval NONE
  
**/
VOID
ElogPchPciExHandlerCheckRootPort (
  IN UINT8            Bus,
  IN UINT8            Device,
  IN UINT8            Function
  )
{
  UINT8   Data8;
  UINT32  Data32;
  UINT8   CapOffset;
  BOOLEAN    ErrorDetected;

  ErrorDetected = FALSE;

  RASDEBUG ((DEBUG_INFO,"ElogPchPciExHandlerCheckRootPort: Inside the function\n"));
  if (PciErrLibIsRootPortErrReportingEnabled (0, Bus, Device, Function)) {
    if (PchIsErrOnPcieRootPort (Bus, Device, Function)) {
      CapOffset = PciErrLibGetCapabilitiesOffset (0, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);
      // APTIOV_SERVER_OVERRIDE_RC_START : PCH PCI error logging not working
      Data8 = MmioRead8 (MmPciBase (Bus, Device, Function) + CapOffset + GET_DEVSTS_OFFSET(0, Bus, Device, Function));
      RASDEBUG ((DEBUG_INFO,"ElogPchPciExHandlerCheckRootPort: Bus = 0x%X, Device = 0x%X, Function = 0x%X\n", Bus, Device, Function));
      RASDEBUG ((DEBUG_INFO,"ElogPchPciExHandlerCheckRootPort: CapOffset = 0x%X\n", CapOffset));
      RASDEBUG ((DEBUG_INFO,"ElogPchPciExHandlerCheckRootPort: DevStsOffset = 0x%X, DevSts = 0x%X\n", CapOffset + GET_DEVSTS_OFFSET(0, Bus, Device, Function), Data8));
      // APTIOV_SERVER_OVERRIDE_RC_END : PCH PCI error logging not working
      if (Data8 & B_PCIE_DEVICE_STATUS_COR_ERR) {
      //
      // Correctable
      //
      Data32 = MmioRead32 (MmPciBase (Bus, Device, Function) + R_PCIEXP_AER_CES);
      Data32 |= V_PCIEXP_AER_CES;
      MmioWrite32 (MmPciBase (Bus, Device, Function) + R_PCIEXP_AER_CES, Data32);

    } else if (Data8 & B_PCIE_DEVICE_STATUS_NF_ERR) {
      //
      // Non-Fatal
      //
      if(mAllowClearingOfUEStatus)  {
        Data32 = MmioRead32 (MmPciBase (Bus, Device, Function) + R_PCIEXP_AER_UES);
        Data32 |= V_PCIEXP_AER_UES;
        MmioWrite32 (MmPciBase (Bus, Device, Function) + R_PCIEXP_AER_UES, Data32);
      }
    } else if (Data8 & B_PCIE_DEVICE_STATUS_FAT_ERR) {
      //
      // Fatal
      //
    } else if (Data8 & B_PCIE_DEVICE_STATUS_UNSUP_REQ) {  // UR
      Data8 |= BIT3;
      // APTIOV_SERVER_OVERRIDE_RC_START : PCH PCI error logging not working
      MmioWrite8 (MmPciBase (Bus, Device, Function) + CapOffset + GET_DEVSTS_OFFSET(0, Bus, Device, Function), Data8);
      // APTIOV_SERVER_OVERRIDE_RC_END : PCH PCI error logging not working
    }
    //
    // Error flagged on this root port, process and log the error.
    //
    ErrorDetected = PciErrLibPcieRootPortErrorHandler (0, Bus, Device, Function);
  }
  }

}

/**
  PCH PCIe Error Handler

  @param[in] Bus        Device BUS number to check

  @retval EFI_SUCCESS. 

**/
EFI_STATUS
ElogPchPciExHandler (
  IN UINT8            Bus
  )
{
  UINT8   Function;
  UINT8   Device;
  UINT8   DeviceIndex;
  UINT8   PchPcieElogdev[] = {PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1 ,PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3};

  RASDEBUG ((DEBUG_INFO,"ElogPchPciExHandler: Inside the function\n"));
  // Check all root port devices
  for (DeviceIndex = 0; DeviceIndex < 3; DeviceIndex++)  {
    Device = (UINT8)PchPcieElogdev[DeviceIndex] ;
    for (Function = 0; Function <= PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8; Function++) {
      // Check if the root port is enabled.
      if (PciErrLibIsDevicePresent (0, Bus, Device, Function)) {
        // Stop when root port 20 is reached
        if (Device   == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3 &&
            Function > PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_20) {
          break;
        } else {
          ElogPchPciExHandlerCheckRootPort (Bus, Device, Function);
        }
      }
    }
  }
  return EFI_SUCCESS;
}


/**
    Subroutine for clearing the PCI SERR# status (and LPC IOCHK# status) in the
    NMI Status and Control Register.  This keeps the system from executing in an
    infinite SMI loop when the system is configured to SMI on NMI.

  @retval NONE. 

**/
VOID
ElogPchClearSerrStatus (
    VOID
  )
{
  UINT8       Data;
  UINT8       OriginalErrorMask;
  //
  // Clear the PCI SERR# and LPC IOCHK# status in the NMI status and control register if set
  //
  Data = IoRead8 (R_PCH_NMI_SC);
  if (Data & 0x0C) {
  //
  // Clear and configure the NMI status and control register so SERR# and IOCHK# will cause NMI.
  //
  OriginalErrorMask = Data & 0x0C;
  //
  // SERR# and IOCHK# error reporting masks are in bits <3:2>
  //
  Data |= 0x0C;
  //
  // Set <3:2> for clearing SERR# and IOCHK# NMI Enables
  //
  Data &= 0x0F;
  //
  // Not allowed to set bits in the upper nibble
  //
  IoWrite8 (R_PCH_NMI_SC, Data );
  //
  // Clear status
  //
  Data &= 0x03;
  Data |= OriginalErrorMask;
  //
  // Restore original error reporting masks
  //
  IoWrite8 (R_PCH_NMI_SC, Data );
}
}

/**
  Handler for PCH PCI.

  @retval EFI_SUCCESS. 

**/
EFI_STATUS
ElogPchPciHandler (
  VOID
  )
{
  UINT8   Bus;
  UINT8   Device;
  UINT8   Function;
  BOOLEAN DetectedError;
  UINT8   Index;

  DetectedError = FALSE;
  Index         = 0;
  //
  // Call the device-specific handler for each device in the table...
  //
  RASDEBUG ((DEBUG_INFO,"ElogPchPciHandler: Inside the function\n"));
  while ((mPchPciDeviceTable[Index].Segment != INVALID_FLAG) && (mPchPciDeviceTable[Index].Bus != INVALID_FLAG)) {
    Bus = mPchPciDeviceTable[Index].Bus;
    Device = mPchPciDeviceTable[Index].Device;
    Function = mPchPciDeviceTable[Index].Function;

    //
    // Check device status before device error handler initialing
    //
    if (PciErrLibIsDevicePresent (0, Bus, Device, Function)) {
      if (mPchPciDeviceTable[Index].Bridge == TRUE)  {
      DetectedError = ElogP2PHandler (Bus, Device, Function);
      } else {
        DetectedError = PciErrLibPciDeviceErrorHandler (0, Bus, Device, Function);
    }
    }

    Index++;
  }

  if (DetectedError) {
    //
    // Clean up the error status registers in the PCH so we don't get caught in
    // a SMI loop.  The PCH PCI Express root port may have detected a correctable
    // error and the system should continue to run after the error was processed.
    //
    ElogPchClearSerrStatus ();
  }
  return EFI_SUCCESS;
}

/**
  This function performs the PCH error checking and processing functions

  @retval EFI_SUCCESS. 

**/
EFI_STATUS
ProcessPchErrors (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;
  RASDEBUG ((DEBUG_INFO,"ProcessPchErrors: Inside the function\n"));
  if (mRasSetup->Pcie.PcieErrEn == 1)  {
    Status = ElogPchPciHandler ();
    Status = ElogPchPciExHandler (mPchBusNum);
    Status = ElogPchDmiHandler (mPchBusNum);
  }

  return Status;
}

////////////////////////////////////// INITIALIZATION ROUTINES /////////////////////////////////////////

/**
  This function checks if a PCI device is present on the specified bus.

  @param[in] Bus        Device's bus number 

  @retval TRUE        A PCI device was found on the bus

**/
BOOLEAN
PciErrLibDeviceOnBus (
  IN UINT8   Bus
  )
{
  UINT8   Device;
  UINT8   Function;

  for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
    for (Function = 0; Function <= PCI_MAX_FUNC; Function++) {

      if (PciErrLibIsDevicePresent (0, Bus, Device, Function)) {
        return TRUE;
      }
    }
  }
  return FALSE;
}


/**
  Enable error log for the PCH Root Ports

  @param[in] Bus        Device's bus number 

  @retval NONE.

**/
  VOID
ElogPchRootPortEnable (
  IN      UINT8   Bus
  )
{
  UINT8     Function;
  UINT8     Device;
  UINT8     Data8;
  UINT8     DeviceIndex;
  UINT32    Data32;
  UINT8     PchPcieElogdev[] = {PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1 ,PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3};

  for (DeviceIndex = 0; DeviceIndex<3; DeviceIndex++)  {
    Device = (UINT8)PchPcieElogdev[DeviceIndex] ;
    for (Function = 0; Function <= PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8; Function++) {
      // Check if the root port is enabled.
      if (PciErrLibIsDevicePresent (0, Bus, Device, Function)) {
        // Do not enable error logging if there are no devices behind the bridge.
        Data8 = MmioRead8 (MmPciBase (Bus, Device, Function) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
        if ((Data8 != 0) && (PciErrLibDeviceOnBus (Data8))) {
          // Enable error reporting in the root port and all subordinate devices.
          PciErrLibEnableElogDeviceAndSubDevices (0, Bus, Device, Function);

          // Override AER programming for PCH root ports
          Data32 = MmioRead32 (MmPciBase (Bus, Device, Function) + R_PCIEXP_AER_UEM);
          Data32 |= PCH_AER_UEM_OVERRIDE;
          MmioWrite32 (MmPciBase (Bus, Device, Function) + R_PCIEXP_AER_UEM, Data32);

          MmioRead32 (MmPciBase (Bus, Device, Function) + R_PCIEXP_AER_CEM);
          Data32 |= PCH_AER_CEM_OVERRIDE;
          MmioWrite32 (MmPciBase (Bus, Device, Function) + R_PCIEXP_AER_CEM, Data32);
        }
      }
    }
  }
}

/**
  Function to enable PCH DMI

  @param[in] Bus        Device BUS number to check

  @retval NONE. 

**/
VOID
ElogPchDmiEnable (
  IN UINT8              Bus
  )
{

  UINT32    Uem;
  UINT32    Cem;
  EFI_STATUS Status;

  ClearDmiErrors (Bus);

  Status  = PchPcrRead32(PID_DMI,R_PCH_PCR_DMI_UEM,&Uem);
  if(EFI_ERROR(Status)) {
    return;
  }

  Status  = PchPcrRead32(PID_DMI,R_PCH_PCR_DMI_CEM,&Cem);
  if(EFI_ERROR(Status)) {
    return;
  }

  Uem |= 0x1ff011; // Mask off all errors
  Cem |= 0x31c1; // Mask off all errors

  if (mRasSetup->Pcie.PcieErrEn == 1) {
    // UnMask the uncorrectable errors
    if (mRasSetup->Pcie.PcieUncorrErrEn == 1) {
      Uem &= ~(B_PCH_DMI_UE_DLPE | B_PCH_DMI_UE_PT | B_PCH_DMI_UE_CA | B_PCH_DMI_UE_RO | B_PCH_DMI_UE_MT);
    }

    // UnMask the Correctable errors
    if (mRasSetup->Pcie.PcieCorrErrEn == 1) {
      Cem &= ~(B_PCH_DMI_CE_RE | B_PCH_DMI_CE_BT | B_PCH_DMI_CE_BD | B_PCH_DMI_CE_RNR);
    }
  }

  PchPcrWrite32(PID_DMI,R_PCH_PCR_DMI_UEM,Uem);
  PchPcrWrite32(PID_DMI,R_PCH_PCR_DMI_CEM,Cem);
  
}

/**
  Enable error log for standard P2P bridge and its PCI devices.
  Any specific P2P enable stuff like PCH HI PCI bridge 8-bit protocol
  should add to the end of this call in that module.


  @param[in] Bus             Device's bus number 
  @param[in] Device          Device's device number
  @param[in] Function        Device's function number

  @retval EFI_SUCCESS

**/
EFI_STATUS
EnableElogP2P (
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT32  Data;
  UINT16  Data16;
  //
  // Standard Initialization...
  //
  PciErrLibEnableElogDeviceAndSubDevices (0, Bus, Device, Function);

  //
  // Enable Discard Timer SERR# enable ,Secondary Discard timer
  //
  Data16 = MmioRead16 (MmPciBase (Bus, Device, Function) + PCI_BRIDGE_CONTROL_REGISTER_OFFSET);
  Data16 |= (BIT9 | BIT11);
  MmioWrite16 (MmPciBase (Bus, Device, Function) + PCI_BRIDGE_CONTROL_REGISTER_OFFSET, Data16);

  //
  // Force SERR on PERR and SERR on Target Abort
  //
  if (mNmiOnSerr) {
    Data = MmioRead32 (MmPciBase (Bus, Device, Function) + R_PCH_PCI_PCI_BPC);
    Data |= (B_PCH_PCI_PCI_BPC_PSE | B_PCH_PCI_PCI_BPC_RTAE);
    MmioWrite32 (MmPciBase (Bus, Device, Function) + R_PCH_PCI_PCI_BPC, Data);
  }

  return EFI_SUCCESS;
}

/**
  Enable error log for PCI/PCIe devices in the PCH.

  @retval NONE.

**/
VOID
ElogPchPciEnable (
  VOID
  )
{
  UINT8   Bus;
  UINT8   Device;
  UINT8   Function;
  BOOLEAN DetectedError;
  UINT8   Index;

  DetectedError = FALSE;
  Index         = 0;
  //
  // Call the device-specific handler for each device in the table...
  //
  while ((mPchPciDeviceTable[Index].Segment != INVALID_FLAG) && (mPchPciDeviceTable[Index].Bus != INVALID_FLAG)) {
    Bus = mPchPciDeviceTable[Index].Bus;
    Device = mPchPciDeviceTable[Index].Device;
    Function = mPchPciDeviceTable[Index].Function;
    //
    // Check device status before device error handler initialing
    //
    if (PciErrLibIsDevicePresent (0, Bus, Device, Function)) {
      if (mPchPciDeviceTable[Index].Bridge == TRUE)  {
      EnableElogP2P (Bus, Device, Function);
      } else {
        PciErrLibEnableElogDevice (0, Bus, Device, Function);
    }
    }
    Index++;
  }
}

/**
  Enable error log for PCH.
    
  @retval EFI_SUCCESS.

**/
EFI_STATUS
EnableElogPCH (
   VOID
  )
{
  mPchBusNum = DEFAULT_PCI_BUS_NUMBER_PCH;
  RASDEBUG ((DEBUG_INFO,"EnableElogPCH: Inside the function\n"));
  if (mRasSetup->Iio.IioErrorEn == 1) {
    ElogPchDmiEnable (mPchBusNum);
    if (mRasSetup->Pcie.PcieErrEn == 1)  {
      ElogPchPciEnable ();
      ElogPchRootPortEnable (mPchBusNum);
    }
  }
  return EFI_SUCCESS;
}

/**
  Enable AsyncSMI Logic.

  @retval EFI_SUCCESS.

**/
EFI_STATUS
EnableAsyncSmiLogic (
  VOID
  )
{
  UINT16                      Data16;
  UINT32                      Data32;
  //UINT32                      GpiRouteData;
  UINT16					  PmBase;
  UINT8                     SmiEn;
  EFI_SMM_GPI_REGISTER_CONTEXT  mGpiContext;
  EFI_HANDLE                    GpiHandle;
  EFI_STATUS  Status;

  //TODO: are we having a ubox umc/cbo option? 
  // Not currently present 
  if(mRasSetup->Platform.CaterrGpioSmiEn == 1 && 
     mRasSetup->Emca.EmcaMsmiEn == 0
     ) {
    SmiEn = 1;    
  } else {
    SmiEn = 0;
  }

  if(SmiEn) {
    RASDEBUG ((DEBUG_INFO,"CaterrGpioSmiEn enabled:\n"));
    // Setup the GPIO that will be register in the GPIO SMM Handler
    mGpiContext.GpiNum = (PCH_CATERR_GPI_SMI_GPP_COM * V_PCH_GPIO_GPP_C_PAD_MAX) + GpioGetPadNumberFromGpioPad (PCH_CATERR_GPI_SMI_GPP_PAD);

    // Configure PCH_CATERR_GPI_SMI_GPP_H or PCH_CATERR_GPI_SMI_GPP_LP to GPIO by default
    SetGpioPadMode(PCH_CATERR_GPI_SMI_GPP_PAD, GpioPadModeGpio);

    // Register PCH_CATERR_GPI_SMI_GPP_H in the GPIO SMM Handler
    Status = mGpiDispatch->Register (
                            mGpiDispatch,
                            GpiChildCallback,
                            &mGpiContext,
                            &GpiHandle
                            );
    ASSERT_EFI_ERROR (Status);  
  }


  //
  // (LBG PM base is located in PMC device not LPC)
  //
  PmBase = MmioRead16 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_ACPI_BASE);
  PmBase = (UINT16) (PmBase & 0xFF80);

  //
  // Clear all Spurious Sources of the SMI at the SMI_STS register offset PMBASE + 34
  //
  Data32 = IoRead32 ((PmBase + R_PCH_SMI_STS)); 
  IoWrite32 ((PmBase + R_PCH_SMI_STS), Data32);

  //
  // Setup the GBL_SMI_EN=1 to only cause SMI.
  //
  Data16 = IoRead16 ((PmBase + R_PCH_SMI_EN));
  Data16 = (UINT16) (Data16 | (B_PCH_SMI_EN_GBL_SMI & SmiEn));
  IoWrite16 ((PmBase + R_PCH_SMI_EN), Data16);

  return EFI_SUCCESS;
}
