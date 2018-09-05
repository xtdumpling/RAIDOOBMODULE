/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2014 - 2016 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file McBankErrorInjection.c

    This is an implementation of the McbankError injection support using MCEJ acpi table.

---------------------------------------------------------------------------**/


#include "McBankErrorInjection.h"


EFI_SMM_BASE2_PROTOCOL         *mSmmBase = NULL;
EFI_SMM_SYSTEM_TABLE2          *mSmst = NULL;

EFI_ACPI_TABLE_PROTOCOL        *mAcpiDrv = NULL;

MCA_ERR_INJ_PARAM_BUFFER           *mMcejParam = NULL;
EFI_ACPI_MCEJ_TRIGGER_ACTION_TABLE   *mMcaInjTriggerAction = NULL;

UINT8 mSetupMcejSupport = MCEJ_SUPPORT_DIS;
EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocol;
EFI_RAS_SYSTEM_TOPOLOGY           *mRasTopology = NULL;
SYSTEM_RAS_SETUP                  *mRasSetup = NULL;
SYSTEM_RAS_CAPABILITY             *mRasCapability = NULL;


/////////////////////////////////////////////////////////////////////////////////////
// MCEJ AcpiTable
/////////////////////////////////////////////////////////////////////////////////////

EFI_ACPI_MCEJ_TABLE mMcejTable = {

  // ACPI Standard Header
  { 
    EFI_ACPI_3_0_MCEJ_SIGNATURE,
    sizeof (EFI_ACPI_MCEJ_TABLE),     // Length
    EFI_ACPI_MCEJ_REVISION,           // Revision
    0x00,                             // CheckSum
    'I', 'N', 'T', 'E', 'L', ' ',     // OemId
    0,                                // OemTableId
    EFI_ACPI_OEM_REVISION,        // OemRevision
    EFI_ACPI_CREATOR_ID,              // CreaterId
    EFI_ACPI_CREATOR_REVISION,        // CreaterRevision
  },

  // MCA Bank Injection Header
  { 
    sizeof(EFI_ACPI_DESCRIPTION_HEADER)   +
    sizeof(EFI_ACPI_MCEJ_MCA_BANK_ERROR_INJECTION_HEADER),  //InjectionHeaderSize;
    0x00,                                 //Resv1;
    EFI_ACPI_MCEJ_ACTION_ENTRY_COUNT,     //InstructionEntryCount;
  },

  // MCA bank Injection Action Table
  {
    {   //Action0
      MCA_ERR_INJ_BEGIN_INJECT_OP,        // BEGIN_INJECTION_OPERATION
      INSTRUCTION_WRITE_REGISTER_VALUE,   // WRITE_REGISTER_VALUE
      FLAG_PRESERVE_REGISTER,             // Flags
      0x00,                               // Reserved
      McejRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
      MCA_ERR_INJ_BEGIN_OPERATION,               // Value for InjectError()
      0xffffffff                          // Mask is only valid for 32-bits 
    },
    {   //Action1
      MCA_ERR_INJ_GET_TRIGGER_ACTION_TABLE, // GET_TRIGGER_ERROR_STRUC
      INSTRUCTION_READ_REGISTER,          // READ_REGISTER
      FLAG_NOTHING,                       // Flags
      0x00,                               // Reserved
      McejRegisterFiller,                 // {0x00, 0x40, 0x00, 0x04, -1},    // GAS (QWORD Memory) Address will be filled during boot
      0,                                  // Value for InjectError()
      0xffffffffffffffff                  // Mask is only valid for 32-bits 
    },
    {   //Action2
      MCA_ERR_INJ_GET_CAP,                   //  GET_ERROR_TYPE
      INSTRUCTION_READ_REGISTER,          // READ_REGISTER
      FLAG_NOTHING,                       // Flags
      0x00,                               // Reserved
      McejRegisterFiller,                 //{0x00, 0x20, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
      0,                                  // Value for InjectError()
      0xffffffff                          // Mask is only valid for 32-bits 
    },
    {   //Action3
      MCA_ERR_INJ_SET_CMD,                   // SET_CMD
      INSTRUCTION_WRITE_REGISTER,         // WRITE_REGISTER
      FLAG_PRESERVE_REGISTER,             // Flags
      0x00,                               // Reserved
      McejRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
      0,                                  // Value for InjectError()
      0xffffffff                          // Mask is only valid for 32-bits 
    },
    {   //Action4
      MCA_ERR_INJ_EXECUTE_INJECT_OP,        // EXECUTE_OPERATION
      INSTRUCTION_WRITE_REGISTER_VALUE,   // WRITE_REGISTER_VALUE
      FLAG_PRESERVE_REGISTER,             // Flags
      0x00,                               // Reserved
      McejActionItemExecute,     // GAS (BYTE IO). Address will be filled in runtime
      MCEJ_SWSMI_VALUE,                   // Value for InjectError()
      0xff                                // Mask is only valid for 16-bits 
    },
    {   //Action5
      MCA_ERR_INJ_END_INJECT_OP,            // END_OPERATION
      INSTRUCTION_WRITE_REGISTER_VALUE,   // WRITE_REGISTER_VALUE
      FLAG_PRESERVE_REGISTER,             // Flags
      0x00,                               // Reserved
      McejRegisterFiller,                 //{0x00, 0x20, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
      MCA_ERR_INJ_END_OPERATION,                 // Value for InjectError()
      0xffffffff                          // Mask is only valid for 32-bits 
    },
    {   //Action6
      MCA_ERR_INJ_CHECK_BUSY_STATUS,        // CHECK_BUSY_STATUS
      INSTRUCTION_READ_REGISTER_VALUE,    // READ_REGISTER
      FLAG_NOTHING,                       // Flags
      0x00,                               // Reserved
      McejRegisterFiller,                 // {0x00, 0x30, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
      0x00000001,                         // Value for InjectError()
      0x00000001                          // Mask is only valid for 32-bits 
    },
    {   //Action7
      MCA_ERR_INJ_GET_CMD_STATUS,           // GET_OPERATION_STATUS
      INSTRUCTION_READ_REGISTER,          // READ_REGISTER
      FLAG_PRESERVE_REGISTER,             // Flags
      0x00,                               // Reserved
      McejRegisterFiller,                 // {0x00, 0x40, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
      0,                                  // Value for InjectError()
      0x000001fe                          // Mask is only valid for 32-bits 
    }
  }
};

// Trigger Action table executed by OS driver.
EFI_ACPI_MCEJ_TRIGGER_ACTION_TABLE mTriggerActionTable = \
{
  {sizeof (EFI_ACPI_MCEJ_TRIGGER_ACTION_HEADER), 0, sizeof (EFI_ACPI_MCEJ_TRIGGER_ACTION_TABLE), 4},
  {
    { //Action0
      MCA_ERR_INJ_TRIGGER_ERROR_ACTION,
      INSTRUCTION_NO_OPERATION,         // Intialized as NOP. To be filled by the runtime injection code
      FLAG_NOTHING,                     // Flags
      0x00,                             // Reserved
      McejRegisterFiller,               // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
      0,                                // Value for InjectError()
      0xffffffffffffffff                // Mask is only valid for 32-bits 
    },
    { //Action1
      MCA_ERR_INJ_TRIGGER_ERROR_ACTION,
      INSTRUCTION_NO_OPERATION,         // Intialized as NOP. To be filled by the runtime injection code
      FLAG_NOTHING,                     // Flags
      0x00,                             // Reserved
      McejRegisterFiller,               // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
      0,                                // Value for InjectError()
      0xffffffff                        // Mask is only valid for 32-bits 
    },
    { //Action2
      MCA_ERR_INJ_TRIGGER_ERROR_ACTION,
      INSTRUCTION_NO_OPERATION,         // Intialized as NOP. To be filled by the runtime injection code
      FLAG_NOTHING,                     // Flags
      0x00,                             // Reserved
      McejRegisterFiller,               // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
      0,                                // Value for InjectError()
      0xffffffff                        // Mask is only valid for 32-bits 
    },
    { //Action3
      MCA_ERR_INJ_TRIGGER_ERROR_ACTION,
      INSTRUCTION_NO_OPERATION,         // Write register
      FLAG_NOTHING,                     // Flags
      0x00,                             // Reserved
      McejRegisterFiller,               // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
      0,                                // Value for InjectError()
      0xffffffff                        // Mask is only valid for 32-bits 
    }
  }
};

VOID
CreateMcejAcpiTable(
  EFI_ACPI_MCEJ_TABLE   *McejTable
  )
  /**

  This function updates the MCEJ ACPI table entires
   @param i/p : pointer to the McejTable    

  @retval Success if MCEJ ACPI table is published else Failure
    
  
**/
{
  EFI_STATUS                        Status;
  UINT8                             *Ests;
  UINT64                            TempOemTableId;

  Status = gBS->AllocatePool (EfiReservedMemoryType, (sizeof(MCA_ERR_INJ_PARAM_BUFFER) + sizeof (EFI_ACPI_MCEJ_TRIGGER_ACTION_TABLE) + 0x80), (VOID **)&Ests);
  ASSERT_EFI_ERROR (Status);
  ZeroMem (Ests, (sizeof(MCA_ERR_INJ_PARAM_BUFFER) + sizeof (EFI_ACPI_MCEJ_TRIGGER_ACTION_TABLE) + 0x80) );

  //
  // Init buffer pointers and data for Einj parameter and Error log address range.
  //
  mMcejParam    = (MCA_ERR_INJ_PARAM_BUFFER *)Ests;
  mMcaInjTriggerAction   = (EFI_ACPI_MCEJ_TRIGGER_ACTION_TABLE *)(Ests + ((sizeof(MCA_ERR_INJ_PARAM_BUFFER)+0x3F)/0x40)*0x40);

  CopyMem(mMcaInjTriggerAction, &mTriggerActionTable, sizeof(EFI_ACPI_MCEJ_TRIGGER_ACTION_TABLE));
  
  // Update Mcej parameters to be used in the MCEJ action structures.
  mMcejParam->TriggerActionTable    = (UINTN)mMcaInjTriggerAction;
  mMcejParam->InjectionCap          = MCA_CAP_SUPPORTED | CMCI_CAP_SUPPORTED;
  mMcejParam->OpState               = 0;
  mMcejParam->BusyStatus            = 0;
  mMcejParam->CmdStatus             = 0;
  mMcejParam->CmdValue              = 0;

  //
  // Update the OEMID and OEM Table ID.
  //
  TempOemTableId = PcdGet64(PcdAcpiDefaultOemTableId);

  CopyMem (McejTable->Header.OemId, PcdGetPtr(PcdAcpiDefaultOemId), sizeof(McejTable->Header.OemId));
  CopyMem (&McejTable->Header.OemTableId, &TempOemTableId, sizeof(McejTable->Header.OemTableId));

  // Fill the MCEJ action entries.
  McejTable->actionEntryList[0].Register.Address = (UINTN)&mMcejParam->OpState;              // Update address for BEGIN_OPERATION
  McejTable->actionEntryList[1].Register.Address = (UINTN)&mMcejParam->TriggerActionTable;   // Update address for GET_TRIGGER_ERROR_ACTION_TABLE
  McejTable->actionEntryList[2].Register.Address = (UINTN)&mMcejParam->InjectionCap;         // Update address for GET_CAPABLILITIES
  McejTable->actionEntryList[3].Register.Address = (UINTN)&mMcejParam->CmdValue;             // Update address for SET_COMMAND
  McejTable->actionEntryList[5].Register.Address = (UINTN)&mMcejParam->OpState;              // Update address for END_OPERATION
  McejTable->actionEntryList[6].Register.Address = (UINTN)&mMcejParam->BusyStatus;           // Update address for CHECK_BUSY_STATUS
  McejTable->actionEntryList[7].Register.Address = (UINTN)&mMcejParam->CmdStatus;            // Update address for GET_CMD_STATUS
  //McejTable->actionEntryList[8].Register.Address = (UINTN)&mMcejParam;                     // Debug entry

}

/**

  This function publishes the MCEJ ACPI table 
   @param i/p : pointer to the McejTable      

  @retval Success if MCEJ ACPI table is published else Failure
    
  
**/
STATIC
EFI_STATUS
SetMcejAcpiTable (
  EFI_ACPI_MCEJ_TABLE   *McejTable
  )
{
  EFI_STATUS                          Status;
  UINTN                               TabHand;

  TabHand = 0;   
  mAcpiDrv = NULL;

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, &mAcpiDrv);
  if(EFI_ERROR(Status)) return Status;
  
  Status = mAcpiDrv->InstallAcpiTable (
                         mAcpiDrv, 
                         McejTable,
                         McejTable->Header.Length,
                         &TabHand
                         );
    return Status;
}

EFI_STATUS
UpdateMcejTable (
  VOID
  )
  /**

  This function creates and publishes the MCEJ ACPI table.

   @param none      

  @retval Success if MCEJ ACPI table is published else Failure
    
  
**/
{
  EFI_STATUS                        Status;

  CreateMcejAcpiTable(&mMcejTable);

  Status  = SetMcejAcpiTable(&mMcejTable);

  return Status;
}


/**

  Entry point of the McBank ErrorInjection support. 

  @param ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  @param SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table        

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded 
  @retval EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
McejSmmEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        SwHandle;
  EFI_SMM_SW_REGISTER_CONTEXT       SwContext;
  EFI_SMM_SW_DISPATCH2_PROTOCOL      *SwDispatch  = 0;
  BOOLEAN             InSmm;


  //
  // Retrieve SMM Base Protocol
  //
  Status = gBS->LocateProtocol (
          &gEfiSmmBase2ProtocolGuid,
          NULL,
          &mSmmBase
          );
  ASSERT_EFI_ERROR (Status);
  //
  // Check to see if we are already in SMM
  //
  mSmmBase->InSmm (mSmmBase, &InSmm);

  if (InSmm) {
    //
    // Step1. setup interface
    // Gets the user selected option from setup
    //
    SetupInterface();

    //
    // Check if MCEJ support is enabled in bios. if it is enabled create the MCEJ ACPI table
    //
    if(mSetupMcejSupport == MCEJ_SUPPORT_DIS) {
      
      return EFI_SUCCESS;
    }
    
    //
    // Step2. Mcej Acpi table Update
    //
    Status = UpdateMcejTable();
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // Step3. Runtime SMi hanlder setup
    //
    mSmmBase->GetSmstLocation (mSmmBase, &mSmst);
    //
    // Register our SMI handlers
    //
    Status = mSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
    ASSERT_EFI_ERROR (Status);;
    //
    // Register SW SMI handler for Mcbank Errorinjection.
    //
    SwContext.SwSmiInputValue = MCEJ_SWSMI_VALUE;
    Status                    = SwDispatch->Register (SwDispatch, McejSwSmiCallBack, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR (Status);    

  }
  return EFI_SUCCESS;
}

/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2012 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

---------------------------------------------------------------------------**/
