//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
    Implementation of shuting down a network adapter.
 
Copyright (c) 2004 - 2007, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed 
and made available under the terms and conditions of the BSD License which 
accompanies this distribution. The full text of the license may be found at 
http://opensource.org/licenses/bsd-license.php 

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Snp.h"
#include "Token.h"  // AMI PORTING


/**
  Call UNDI to shut down the interface.

  @param  Snp   Pointer to snp driver structure.

  @retval EFI_SUCCESS        UNDI is shut down successfully.
  @retval EFI_DEVICE_ERROR   UNDI could not be shut down.

**/
EFI_STATUS
PxeShutdown (
  IN SNP_DRIVER *Snp
  )
{
  Snp->Cdb.OpCode     = PXE_OPCODE_SHUTDOWN;
  Snp->Cdb.OpFlags    = PXE_OPFLAGS_NOT_USED;
  Snp->Cdb.CPBsize    = PXE_CPBSIZE_NOT_USED;
  Snp->Cdb.DBsize     = PXE_DBSIZE_NOT_USED;
  Snp->Cdb.CPBaddr    = PXE_CPBADDR_NOT_USED;
  Snp->Cdb.DBaddr     = PXE_DBADDR_NOT_USED;
  Snp->Cdb.StatCode   = PXE_STATCODE_INITIALIZE;
  Snp->Cdb.StatFlags  = PXE_STATFLAGS_INITIALIZE;
  Snp->Cdb.IFnum      = Snp->IfNum;
  Snp->Cdb.Control    = PXE_CONTROL_LAST_CDB_IN_LIST;

  //
  // Issue UNDI command and check result.
  //
  DEBUG ((EFI_D_NET, "\nsnp->undi.shutdown()  "));

  (*Snp->IssueUndi32Command) ((UINT64)(UINTN) &Snp->Cdb);

  if (Snp->Cdb.StatCode != PXE_STATCODE_SUCCESS) {
    //
    // UNDI could not be shutdown. Return UNDI error.
    //
    DEBUG ((EFI_D_WARN, "\nsnp->undi.shutdown()  %xh:%xh\n", Snp->Cdb.StatFlags, Snp->Cdb.StatCode));

    return EFI_DEVICE_ERROR;
  }
  //
  // Free allocated memory.
  //
  if (Snp->TxRxBuffer != NULL) {
    Snp->PciIo->FreeBuffer (
                  Snp->PciIo,
                  SNP_MEM_PAGES (Snp->TxRxBufferSize),
                  (VOID *) Snp->TxRxBuffer
                  );
  }

  Snp->TxRxBuffer      = NULL;
  Snp->TxRxBufferSize  = 0;

  return EFI_SUCCESS;
}

#if (NET_PKG_AMI_PORTING_ENABLE == 1)
// AMI PORTING STARTS - Register an event for CTRL_ALT_DEL Key Combination.
/**
  Nofication call back function for CTRL_ALT_DEL/Soft reset key sequence.

  @param[in] Event      A pointer to the Event that triggered the callback.
  @param[in] Context    A pointer to private data registered with the callback function.

  @retval None

**/
VOID 
EFIAPI
TriggerPxeShutdown(IN EFI_EVENT  Event,IN VOID   *Context)
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    UINTN   SnpHandleCount = 0, HandleIndex = 0;
    EFI_HANDLE  *SnpHandlesBuffer = NULL;
    EFI_SIMPLE_NETWORK_PROTOCOL *SnpProtocol;
    SNP_DRIVER  *SnpDriverInfo = NULL;
    
	// locate and issue shutdown command for all available NIC interfaces
    DEBUG((DEBUG_INFO,"\n PXERESET: Inside TriggerPxeShutdown\n"));
    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiSimpleNetworkProtocolGuid,
            NULL,
            &SnpHandleCount,
            &SnpHandlesBuffer);
    DEBUG((DEBUG_INFO,"\n PXERESET: LocateHandleBuffer Status :: %r\n",Status));
    if(!EFI_ERROR(Status) && SnpHandleCount > 0)
    {
        DEBUG((DEBUG_INFO,"\n PXERESET: gEfiSimpleNetworkProtocolGuid SnpHandles :: 0x%X\n",SnpHandleCount));
        for(HandleIndex = 0; HandleIndex < SnpHandleCount; HandleIndex++)
        {
            Status = gBS->HandleProtocol(
                    SnpHandlesBuffer[HandleIndex],
                    &gEfiSimpleNetworkProtocolGuid,
                    &SnpProtocol);
            DEBUG((DEBUG_INFO,"\n PXERESET: HandleProtocol Status :: %r\n",Status));
            if(!EFI_ERROR(Status) && (SnpProtocol != NULL))
            {
                SnpDriverInfo = EFI_SIMPLE_NETWORK_DEV_FROM_THIS (SnpProtocol);
                if(SnpDriverInfo)
                {
                    Status = PxeShutdown(SnpDriverInfo);
                    DEBUG((DEBUG_INFO,"\n PXERESET: PxeShutdown Status for index#%d is %r\n",HandleIndex,Status));
                }
            }
        }
    }
    if(SnpHandlesBuffer)
        FreePool(SnpHandlesBuffer);
	if(Event)
	{
		gBS->CloseEvent(Event);
		Event = NULL;
	}
    return ;
}
// AMI PORTING ENDS - Register an event for CTRL_ALT_DEL Key Combination.
#endif	// NET_PKG_AMI_PORTING_ENABLE

/**
  Resets a network adapter and leaves it in a state that is safe for another 
  driver to initialize. 
  
  This function releases the memory buffers assigned in the Initialize() call.
  Pending transmits and receives are lost, and interrupts are cleared and disabled.
  After this call, only the Initialize() and Stop() calls may be used. If the 
  network interface was successfully shutdown, then EFI_SUCCESS will be returned.
  If the driver has not been initialized, EFI_DEVICE_ERROR will be returned.

  @param  This  A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.

  @retval EFI_SUCCESS           The network interface was shutdown.
  @retval EFI_NOT_STARTED       The network interface has not been started.
  @retval EFI_INVALID_PARAMETER This parameter was NULL or did not point to a valid 
                                EFI_SIMPLE_NETWORK_PROTOCOL structure.
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.

**/
EFI_STATUS
EFIAPI
SnpUndi32Shutdown (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This
  )
{
  SNP_DRIVER  *Snp;
  EFI_STATUS  Status;
  EFI_TPL     OldTpl;

  //
  // Get pointer to SNP driver instance for *This.
  //
  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Snp = EFI_SIMPLE_NETWORK_DEV_FROM_THIS (This);

  OldTpl = gBS->RaiseTPL (TPL_CALLBACK);

  //
  // Return error if the SNP is not initialized.
  //
  switch (Snp->Mode.State) {
  case EfiSimpleNetworkInitialized:
    break;

  case EfiSimpleNetworkStopped:
    Status = EFI_NOT_STARTED;
    goto ON_EXIT;

  default:
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }
  
  Status                          = PxeShutdown (Snp);

  Snp->Mode.State                 = EfiSimpleNetworkStarted;
  Snp->Mode.ReceiveFilterSetting  = 0;

  Snp->Mode.MCastFilterCount      = 0;
  Snp->Mode.ReceiveFilterSetting  = 0;
  ZeroMem (Snp->Mode.MCastFilter, sizeof Snp->Mode.MCastFilter);
  CopyMem (
    &Snp->Mode.CurrentAddress,
    &Snp->Mode.PermanentAddress,
    sizeof (EFI_MAC_ADDRESS)
    );

  gBS->CloseEvent (Snp->Snp.WaitForPacket);

ON_EXIT:
  gBS->RestoreTPL (OldTpl);

  return Status;
}
