/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2014 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file McBankErrorInjectionHandler.c

    This is file is called in runtime for Mcbank error inejction.

---------------------------------------------------------------------------**/


#include "McBankErrorInjection.h"

extern EFI_SMM_SYSTEM_TABLE2          *mSmst;

extern MCA_ERR_INJ_PARAM_BUFFER           *mMcejParam;
extern EFI_ACPI_MCEJ_TRIGGER_ACTION_TABLE   *mMcaInjTriggerAction;
//extern EFI_ACPI_MCEJ_TABLE mMcejTable;
extern EFI_ACPI_MCEJ_TRIGGER_ACTION_TABLE mTriggerActionTable;

typedef struct {
  UINT32  MsrNumber;
  UINT64  MsrValue;
} MSR_UPADTE_STRUCT;

MSR_UPADTE_STRUCT mMsrUpdateStruc;

/**

  This function updates the DEBUG_ERR_INJ_CTL MSr 1e3 with the input requested.
  the request could be Enable/Disable Mcbanks write request or Enable /Disable MCA or CMCI sigannling request.
   @param i/p : MSR_UPADTE_STRUCT *MsrUpdateStruc :  MSR address and MSR value.   

  @retval Success if MCEJ ACPI table is published else Failure
  
**/
void
McbankUpdate(
  MSR_UPADTE_STRUCT *MsrUpdateStruc
)
{

  UINT64      Data64;

  Data64 = AsmReadMsr64 (MsrUpdateStruc->MsrNumber);
  Data64 &= ~(3);
  Data64 |= MsrUpdateStruc->MsrValue;
  AsmWriteMsr64 (MsrUpdateStruc->MsrNumber, Data64);

}


EFI_STATUS
McejSwSmiCallBack (   
  IN  EFI_HANDLE                          DispatchHandle,
  IN  CONST EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext, OPTIONAL
  IN OUT VOID                             *CommBuffer,     OPTIONAL
  IN OUT UINTN                            *CommBufferSize  OPTIONAL
  )
/**

  This function invoked by the OS with input copmmand request in SET_CMD action entry

   @param i/p : i/p from MCEJ table:  SET_CMD action entry value = 
   @param CMD_GENERATE_CMCI or CMD_GENERATE_MCA or 
   @param MCBANK_OVERWRITE_EN or MCA_CMCI_SIGNAL_EN 
   @param function I/P: 
   @param IN  EFI_HANDLE                          DispatchHandle,
   @param IN  CONST EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext, OPTIONAL
   @param IN OUT VOID                             *CommBuffer,     OPTIONAL
   @param IN OUT UINTN                            *CommBufferSize  OPTIONAL


   @retval MCA_ERR_INJ_GET_CMD_STATUS action entry in MCEJ ACPI table is updated with status correponding to the input request.
   @retval 0x0 Success, 0x1 Unknown Failure, 0x2 Invalid Access

  Function returns - Success;
  
**/
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      SetCmd;
  UINT32      InjCtrlMsrCmd;
  UINT8       i =0;
  UINTN       Thread = 0;
  BOOLEAN     McaCmciSignalCmdValid = FALSE;
  UINT64    Data64;


  mMcejParam->BusyStatus = 0;
  mMcejParam->CmdStatus = 0;

  // Invalidate the trigger action entries
  CopyMem(mMcaInjTriggerAction, &mTriggerActionTable, sizeof(EFI_ACPI_MCEJ_TRIGGER_ACTION_TABLE));

  // Read the command passed in the set command action structure
  SetCmd   = (UINT32)mMcejParam->CmdValue;

  // Check MCA/CMCI signalling command passed in SET_COMMAND is valid or not?
  if( SetCmd & (CMD_GENERATE_CMCI | CMD_GENERATE_MCA) )  {
    if ((SetCmd & CMD_MCA_CMCI_SIGNAL_EN) != CMD_MCA_CMCI_SIGNAL_EN)  {
      mMcejParam->CmdStatus = MCA_ERR_INJ_CMD_INVALID_ACCESS;
      McaCmciSignalCmdValid = FALSE;
      return EFI_SUCCESS;
    } else {
    McaCmciSignalCmdValid = TRUE;
    }
  }

  // Bit0(MCBW_En) and Bit1(MCA_CMCI_SE) are valid 
  mMsrUpdateStruc.MsrNumber  = DEBUG_ERR_INJ_CTL;
  InjCtrlMsrCmd = SetCmd & 03;
  mMsrUpdateStruc.MsrValue   = InjCtrlMsrCmd;

  for (Thread = 0; Thread < mSmst->NumberOfCpus; Thread++) {
    //while (!ApDone) { }
    //ApDone = 00;
    Status = mSmst->SmmStartupThisAp (McbankUpdate,
                             Thread, 
                             &mMsrUpdateStruc);

  //if (EFI_ERROR(Status))  {
    //mMcejParam->CmdStatus = MCA_ERR_INJ_CMD_UNKNOWN_FAILURE;
    //return EFI_SUCCESS;
  //}
  }

  McbankUpdate(&mMsrUpdateStruc);

  // Read back the DEBUG_ERR_INJ_CTL MSR value to chekc if the MC bank write capability or MCA/CMCI signaling capability bits are set.
  Data64 = AsmReadMsr64 (DEBUG_ERR_INJ_CTL);
  // check Bit0(MCBW_En) or Bit1(MCA_CMCI_SE) is set? If not, could be feature is not supported.
  if( !(Data64 & (MCBANK_OVERWRITE_EN | MCA_CMCI_SIGNAL_EN )) ) { 
      mMcejParam->CmdStatus = MCA_ERR_INJ_CMD_INVALID_ACCESS;
      return EFI_SUCCESS;
  }
   
  i =0;
  // Bit0(MCASignal) and Bit1(MCA_CMCI_SE) are valid 
  if(SetCmd & CMD_GENERATE_CMCI ) {

    mMcaInjTriggerAction->Trigger[i].Operation                  = MCA_ERR_INJ_TRIGGER_ERROR_ACTION;
    mMcaInjTriggerAction->Trigger[i].Instruction                = INSTRUCTION_WRITE_REGISTER_VALUE;
    mMcaInjTriggerAction->Trigger[i].Flags                      = FLAG_NOTHING;
    mMcaInjTriggerAction->Trigger[i].Reserved8                  = 00;
    mMcaInjTriggerAction->Trigger[i].Register.AddressSpaceId    = EFI_ACPI_FUNCTIONAL_FIXED_HARDWARE;
    mMcaInjTriggerAction->Trigger[i].Register.RegisterBitWidth  = 0x40;  
    mMcaInjTriggerAction->Trigger[i].Register.RegisterBitOffset = 0x00;
    mMcaInjTriggerAction->Trigger[i].Register.AccessSize        = EFI_ACPI_QWORD;
    // Bit0(MCA_G ) and BIT1(CMCI_G)
    mMcaInjTriggerAction->Trigger[i].Register.Address           = DEBUG_ERR_INJ_CTL2;
    mMcaInjTriggerAction->Trigger[i].Value                      = GENERATE_CMCI;
    mMcaInjTriggerAction->Trigger[i].Mask                       = GENERATE_CMCI;

    i++;
  }

 if( SetCmd & CMD_GENERATE_MCA ) {

    mMcaInjTriggerAction->Trigger[i].Operation                  = MCA_ERR_INJ_TRIGGER_ERROR_ACTION;
    mMcaInjTriggerAction->Trigger[i].Instruction                = INSTRUCTION_WRITE_REGISTER_VALUE;
    mMcaInjTriggerAction->Trigger[i].Flags                      = FLAG_NOTHING;
    mMcaInjTriggerAction->Trigger[i].Reserved8                  = 00;
    mMcaInjTriggerAction->Trigger[i].Register.AddressSpaceId    = EFI_ACPI_FUNCTIONAL_FIXED_HARDWARE;
    mMcaInjTriggerAction->Trigger[i].Register.RegisterBitWidth  = 0x40;  
    mMcaInjTriggerAction->Trigger[i].Register.RegisterBitOffset = 0x00;
    mMcaInjTriggerAction->Trigger[i].Register.AccessSize        = EFI_ACPI_QWORD;
    // Bit0(MCA_G ) and BIT1(CMCI_G)
    mMcaInjTriggerAction->Trigger[i].Register.Address           = DEBUG_ERR_INJ_CTL2;
    mMcaInjTriggerAction->Trigger[i].Value                      = GENERATE_MCA;
    mMcaInjTriggerAction->Trigger[i].Mask                       = GENERATE_MCA;

  }

  return EFI_SUCCESS;
}


