//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file NvmePassthru.c
    Contain Nvme Express passthru protocol APIs definition

**/

//---------------------------------------------------------------------------
#include "NvmeIncludes.h"
#include "NvmeController.h"
#include "NvmeBus.h"
#include "Protocol/AmiNvmeController.h"
#include "NvmePassthru.h"
//---------------------------------------------------------------------------

/**
    Routine to get the Active NameSpace

    @param NvmeController 
    @param NamespaceId 

    @retval Returns ACTIVE_NAMESPACE_DATA pointer for the NamespaceId
**/

ACTIVE_NAMESPACE_DATA*
GetActiveNameSpace (
    AMI_NVME_CONTROLLER_PROTOCOL   *NvmeController,
    UINT32                         NamespaceId
)
{
    LIST_ENTRY            *LinkData;
    ACTIVE_NAMESPACE_DATA *ActiveNameSpace = NULL;
    
    if(!NvmeController) {
        return NULL;
    }

    for (LinkData = NvmeController->ActiveNameSpaceList.ForwardLink; 
            LinkData != &NvmeController->ActiveNameSpaceList; 
            LinkData = LinkData->ForwardLink) {

        ActiveNameSpace = BASE_CR(LinkData, ACTIVE_NAMESPACE_DATA, Link);

        if(ActiveNameSpace->ActiveNameSpaceID == NamespaceId) {
            return ActiveNameSpace;
        }
    }
    return NULL;
}

/**
    Sends an NVM Express Command Packet to an NVM Express controller or namespace

    @param This         A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance
    @param NamespaceId  A 32 bit namespace ID as defined in the NVMe specification 
                        to which the NVM Express Command Packet will be sent.  
    @param Packet       A pointer to the NVM Express Command Packet to send to
                        the NVMe namespace specified by NamespaceId. 
    @param Event        Event to be signaled when the NVM Express Command Packet completes
                        and non-blocking I/O is supported

    @retval EFI_STATUS

**/
EFI_STATUS 
EFIAPI
NvmePassThru ( 
    IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL            *This,
    IN UINT32                                        NamespaceId,
    IN OUT EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET  *Packet,
    IN EFI_EVENT                                     Event
)
{
    EFI_STATUS                    Status;
    ACTIVE_NAMESPACE_DATA         *ActiveNameSpace = NULL;
    NVME_COMMAND_WRAPPER          *NvmeCmdWrapper = NULL;
    NVME_DXE_SMM_DATA_AREA        *NvmeDxeSmmDataArea = NULL;
    AMI_NVME_CONTROLLER_PROTOCOL  *NvmeController;
    UINT32                        AlignmentBoundry;
    VOID                          *BufferUnMap = NULL;
    COMPLETION_QUEUE_ENTRY        *NvmeCompletionData = NULL;
    UINT8                         NvmeOpcode;
    UINT64                        LBA;
    UINTN                         BufferSize = 0;
    UINTN                         Offset = 0;
    UINTN                         RemainingSize = 0;
    UINTN                         PRP1TransferSize;
    UINTN                         PRP2TransferSize;

    if( !This || !Packet || !Packet->NvmeCmd ) {
        return EFI_INVALID_PARAMETER;
    }

#if defined(USE_MDE_PKG_NVME_PASSTHRU_CHANGES)
    NvmeOpcode = (UINT8)Packet->NvmeCmd->Cdw0.Opcode;
#else
    NvmeOpcode = Packet->NvmeCmd->Cdw0.OpCode;
#endif

    // Return if QueueType is invalid
    if (Packet->QueueType != NVME_ADMIN_SUBMISSION_QUEUE && Packet->QueueType != NVME_IO_SUBMISSION_QUEUE) {
        return EFI_INVALID_PARAMETER;
    }

    // Check Transfer buffer is NULL, if not,Check its memory is properly alligned or not
    if( Packet->TransferBuffer && ( This->Mode->IoAlign > 1 ) ) {

        // Should be align in 2's power
        AlignmentBoundry = 1 << This->Mode->IoAlign;
        //
        //Get what is the number in 2 to the power Mode->IoAlign 
        //       
        if((0 !=  ((UINTN)((UINTN)(Packet->TransferBuffer) % AlignmentBoundry) ) ||
           (0 !=  ((UINTN)( Packet->TransferLength % AlignmentBoundry))) )) {
            return EFI_INVALID_PARAMETER;
        }
    }

    NvmeController = NVME_CONTROLLER_PROTOCOL_FROM_THIS(This);

    NvmeDxeSmmDataArea = NvmeController->NvmeDxeSmmDataArea;

    if(!Packet->NvmeCompletion) {
        return EFI_INVALID_PARAMETER;
    }

    NvmeCompletionData = (COMPLETION_QUEUE_ENTRY*)Packet->NvmeCompletion;

    Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof(NVME_COMMAND_WRAPPER),
                  (VOID**)&NvmeCmdWrapper
                  );

    if (EFI_ERROR(Status)) {
        return Status;
    }
    // Clear memory
    SetMem(NvmeCmdWrapper, sizeof(NVME_COMMAND_WRAPPER), 0);
    
    // Fill Common values of command
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = NvmeOpcode;
    NvmeCmdWrapper->NvmCmd.CMD0.FusedOperation = Packet->NvmeCmd->Cdw0.FusedOperation;
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = Packet->QueueType?  NvmeDxeSmmDataArea->CommandIdentifierQueue1 : NvmeDxeSmmDataArea->CommandIdentifierAdmin;
    NvmeCmdWrapper->NvmCmd.NSID = Packet->NvmeCmd->Nsid;
    NvmeCmdWrapper->NvmCmd.MPTR = 0;
    NvmeCmdWrapper->AdminOrNVMCmdSet = Packet->QueueType ? FALSE : TRUE;
    NvmeCmdWrapper->SQIdentifier = Packet->QueueType;           // Queue 0 for Admin cmds,1 for I/O command
    NvmeCmdWrapper->CmdTimeOut = (UINT32)(Packet->CommandTimeout < 10000 ? 1 : DivU64x32 (Packet->CommandTimeout, 10000));

    if(Packet->NvmeCmd->Flags & CDW10_VALID) {
        NvmeCmdWrapper->NvmCmd.CDW10 = Packet->NvmeCmd->Cdw10;
    }
    if(Packet->NvmeCmd->Flags & CDW11_VALID) {
        NvmeCmdWrapper->NvmCmd.CDW11 = Packet->NvmeCmd->Cdw11;
    }
    if(Packet->NvmeCmd->Flags & CDW12_VALID) {
        NvmeCmdWrapper->NvmCmd.CDW12 = Packet->NvmeCmd->Cdw12;
    }
    if(Packet->NvmeCmd->Flags & CDW13_VALID) {
        NvmeCmdWrapper->NvmCmd.CDW13 = Packet->NvmeCmd->Cdw13;
    }
    if(Packet->NvmeCmd->Flags & CDW14_VALID) {
        NvmeCmdWrapper->NvmCmd.CDW14 = Packet->NvmeCmd->Cdw14;
    }
    if(Packet->NvmeCmd->Flags & CDW15_VALID) {
        NvmeCmdWrapper->NvmCmd.CDW15 = Packet->NvmeCmd->Cdw15;
    }

    // if QueueType is 0, Admin Command
    // if QueueType is 1, I/O Command
    if ( Packet->QueueType == NVME_ADMIN_SUBMISSION_QUEUE ) {

        Status = EFI_SUCCESS;
        switch(NvmeOpcode)
        {
            case DELETE_IO_SUBMISSION_QUEUE:
            case CREATE_IO_SUBMISSION_QUEUE:
            case DELETE_IO_COMPLETION_QUEUE:
            case CREATE_IO_COMPLETION_QUEUE:
            case ABORT:
            case ASYNC_EVENT_REQUEST:
                Status = EFI_UNSUPPORTED;
                break;

            case DEVICE_SELF_TEST:
                if( !(Packet->NvmeCmd->Flags & CDW10_VALID) ) {
                    Status = EFI_INVALID_PARAMETER;
                    goto PassthruErrorExit;
                }
                break;

            case IDENTIFY:

                if( !( (Packet->NvmeCmd->Flags & CDW10_VALID) && Packet->TransferBuffer ) ) {
                    Status = EFI_INVALID_PARAMETER;
                    goto PassthruErrorExit;
                }

                if( Packet->TransferLength < sizeof(IDENTIFY_CONTROLLER_DATA) ) {
                    Status = EFI_BAD_BUFFER_SIZE;
                    goto PassthruErrorExit;
                }

                 NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64)Packet->TransferBuffer;

                 // If PRP1 isn't aligned on MemoryPageSize, then PRP2 will also be used.
                 NvmeCmdWrapper->NvmCmd.PRP2 = ((UINT64)Packet->TransferBuffer & ~((UINT64)NvmeController->MemoryPageSize-1)) + 
                                               NvmeController->MemoryPageSize;
                break;

            case GET_FEATURES:
            case SET_FEATURES:
                if( (Packet->NvmeCmd->Flags & (CDW10_VALID | CDW11_VALID)) != (CDW10_VALID | CDW11_VALID) ) {
                    Status = EFI_INVALID_PARAMETER;
                    goto PassthruErrorExit;
                }
                if( Packet->TransferBuffer ) {
                    NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64)Packet->TransferBuffer;
                
                    // If PRP1 isn't aligned on MemoryPageSize, then PRP2 will also be used.
                    NvmeCmdWrapper->NvmCmd.PRP2 = ((UINT64)Packet->TransferBuffer & ~((UINT64)NvmeController->MemoryPageSize-1)) + 
                                                   NvmeController->MemoryPageSize;
                }
                break;

            case GET_LOG_PAGE:
                if( !( (Packet->NvmeCmd->Flags & CDW10_VALID) && Packet->TransferBuffer) ) {
                    Status = EFI_INVALID_PARAMETER;
                    goto PassthruErrorExit;
                }

                NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64)Packet->TransferBuffer;
                           
                // If PRP1 isn't aligned on MemoryPageSize, then PRP2 will also be used.
                NvmeCmdWrapper->NvmCmd.PRP2 = ((UINT64)Packet->TransferBuffer & ~((UINT64)NvmeController->MemoryPageSize-1)) + 
                                              NvmeController->MemoryPageSize;
                break;

            case FORMAT_NVM:
                if( !(Packet->NvmeCmd->Flags & CDW10_VALID) ) {
                    Status = EFI_INVALID_PARAMETER;
                    goto PassthruErrorExit;
                }
                break;

            case SECURITY_SEND:
            case SECURITY_RECEIVE:
                if( ( (Packet->NvmeCmd->Flags & (CDW10_VALID | CDW11_VALID)) != (CDW10_VALID | CDW11_VALID)) ) {
                    Status = EFI_INVALID_PARAMETER;
                    goto PassthruErrorExit;
                }

                NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64)Packet->TransferBuffer;

                // If PRP1 isn't aligned on MemoryPageSize, then PRP2 will also be used.
                NvmeCmdWrapper->NvmCmd.PRP2 = ((UINT64)Packet->TransferBuffer & ~((UINT64)NvmeController->MemoryPageSize-1)) + 
                                              NvmeController->MemoryPageSize;
                break;

            case FIRMWARE_ACTIVATE:
                if( !(Packet->NvmeCmd->Flags & CDW10_VALID) ) {
                    Status = EFI_INVALID_PARAMETER;
                    goto PassthruErrorExit;
                }
                break;
                                
            case FIRMWARE_IMAGE_DOWNLOAD:
                if( ( (Packet->NvmeCmd->Flags & (CDW10_VALID | CDW11_VALID)) != (CDW10_VALID | CDW11_VALID)) ) {
                    Status = EFI_INVALID_PARAMETER;
                    goto PassthruErrorExit;
                }
                
                RemainingSize = (Packet->NvmeCmd->Cdw10+1) << 2;  // Converting size into bytes
                Offset = Packet->NvmeCmd->Cdw11 << 2;

                while(RemainingSize > 0) {
                    NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64)Packet->TransferBuffer + Offset;
                    PRP1TransferSize = NvmeController->MemoryPageSize - 
                                       (NvmeCmdWrapper->NvmCmd.PRP1 & ((UINTN)(NvmeController->MemoryPageSize) - 1));
    
                    // Do we need PRP2
                    if(RemainingSize > PRP1TransferSize) {
                        NvmeCmdWrapper->NvmCmd.PRP2 = NvmeCmdWrapper->NvmCmd.PRP1 + PRP1TransferSize;
                        PRP2TransferSize = NvmeController->MemoryPageSize;
                    } else {
                        NvmeCmdWrapper->NvmCmd.PRP2 = 0;
                        PRP2TransferSize = 0;
                    }
                    
                    BufferSize = PRP1TransferSize + PRP2TransferSize;
                    NvmeCmdWrapper->NvmCmd.CDW10 = (UINT32)(BufferSize>>2)-1;  // BufferSize in Dwords
                    NvmeCmdWrapper->NvmCmd.CDW11 = (UINT32)(Offset>>2);        // Starting Offset
                    
                    RemainingSize -= BufferSize;
                    Offset += BufferSize;
                    
                    Status = ExecuteNvmeCmd (NvmeController, NvmeCmdWrapper, (COMPLETION_QUEUE_ENTRY*)NvmeCompletionData);
                    if (EFI_ERROR(Status)) {
                        break;
                    }
                }

                if((RemainingSize == 0) || EFI_ERROR(Status)) {
                    goto PassthruErrorExit;
                }
                break;
                
            default:
                Status = EFI_UNSUPPORTED;
                break;
        }
        if( Status != EFI_UNSUPPORTED ) {
            // Send Nvme Command
            Status = ExecuteNvmeCmd (NvmeController, NvmeCmdWrapper, (COMPLETION_QUEUE_ENTRY*)NvmeCompletionData);
        }
    } else {  // Send NVM command to Valid Namespace ID

        Status = EFI_INVALID_PARAMETER;
        switch(NvmeOpcode)
        {
            case NVME_READ:
            case NVME_WRITE:
            case NVME_COMPARE:

                // LBA value is passed through Command Dword 10 & 11, return error if either CDW10 or CDW11 is invalid
                if( ((Packet->NvmeCmd->Flags & (CDW10_VALID | CDW11_VALID)) != (CDW10_VALID | CDW11_VALID)) ) {
                    break;
                }
                
                LBA = Packet->NvmeCmd->Cdw11;
                LBA = LShiftU64 (LBA, 32) | ( (UINT64)Packet->NvmeCmd->Cdw10 & 0x0FFFFFFFF );

                ActiveNameSpace = GetActiveNameSpace( NvmeController, Packet->NvmeCmd->Nsid );

                if( !ActiveNameSpace ) {
                    goto PassthruErrorExit;
                }
                Status = NvmeReadWriteBlocks ( ActiveNameSpace,
                                               0,                                // MediaId = 0
                                               LBA,
                                               (UINT64)Packet->TransferLength,   // BufferSize,
                                               (VOID*)(Packet->TransferBuffer),  // Buffer,
                                               (COMPLETION_QUEUE_ENTRY*)NvmeCompletionData,
                                                NvmeOpcode                       // ReadWriteOpCode
                                              );
                 break;

            case NVME_WRITE_ZEROES:
                // LBA value is passed through Command Dword 10,11,12,13,14 and 15, return error if any one of the
                // Command Dword is invalid
                if( (Packet->NvmeCmd->Flags & (CDW14_VALID|CDW15_VALID)) != (CDW14_VALID|CDW15_VALID)) {
                    break;
                }

            case NVME_WRITE_UNCORRECTABLE:
                // LBA value is passed through Command Dword 10,11 and 12, return error if any one of the
                // Command Dword is invalid
                if( (Packet->NvmeCmd->Flags & (CDW10_VALID|CDW11_VALID|CDW12_VALID)) != 
                         (CDW10_VALID|CDW11_VALID|CDW12_VALID) ) {
                    break;
                }

            case NVME_FLUSH:
                // Send Nvme Command
                Status = ExecuteNvmeCmd (NvmeController, NvmeCmdWrapper, (COMPLETION_QUEUE_ENTRY*)NvmeCompletionData);
                break;

            case NVME_DATASET_MANAGEMENT:
                // LBA value is passed through Command Dword 10,11 return error if any one of the
                // Command Dword is invalid
                if( (Packet->NvmeCmd->Flags & (CDW10_VALID |CDW11_VALID)) != (CDW10_VALID |CDW11_VALID)) {
                    break;
                }

                NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64)Packet->TransferBuffer;

                // If PRP1 isn't aligned on MemoryPageSize, then PRP2 will also be used.
                NvmeCmdWrapper->NvmCmd.PRP2 = ((UINT64)Packet->TransferBuffer & ~((UINT64)NvmeController->MemoryPageSize-1)) + 
                                              NvmeController->MemoryPageSize;

                // Send Nvme Command
                Status = ExecuteNvmeCmd (NvmeController, NvmeCmdWrapper, (COMPLETION_QUEUE_ENTRY*)NvmeCompletionData);
                break;

            default:
                Status = EFI_UNSUPPORTED;
                break;
         }
    }
PassthruErrorExit:

    if( NvmeCmdWrapper ) {
        gBS->FreePool ( NvmeCmdWrapper );
    }

    return Status;
}

/**
    API to get next valid NameSpace ID of the Nvme Device

    @param  This         A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance
    @param  NamespaceId  A pointer to a valid Namespace ID on this NVM Express controller 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
GetNextNamespace(
    IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL    *This,
    IN OUT UINT32                            *NamespaceId
)
{
    AMI_NVME_CONTROLLER_PROTOCOL *NvmeController;
    ACTIVE_NAMESPACE_DATA        *ActiveNameSpaceData;
    LIST_ENTRY                   *LinkData;
    BOOLEAN                      ReturnNextNameSpaceId = FALSE;

    if(!NamespaceId || !This) {
        return EFI_INVALID_PARAMETER;
    }
    if( 0xFFFFFFFF == *NamespaceId) {
        ReturnNextNameSpaceId = TRUE;
    }

    NvmeController = NVME_CONTROLLER_PROTOCOL_FROM_THIS(This);

    for (LinkData = NvmeController->ActiveNameSpaceList.ForwardLink; 
            LinkData != &NvmeController->ActiveNameSpaceList; 
            LinkData = LinkData->ForwardLink) {
        
        ActiveNameSpaceData = BASE_CR(LinkData ,ACTIVE_NAMESPACE_DATA, Link);

        if( TRUE == ReturnNextNameSpaceId) {
            *NamespaceId = ActiveNameSpaceData->ActiveNameSpaceID;
            return EFI_SUCCESS;
        }
        
        if(ActiveNameSpaceData->ActiveNameSpaceID == *NamespaceId) {
            ReturnNextNameSpaceId = TRUE;
        }
    }
    if( ReturnNextNameSpaceId ) {
        return EFI_NOT_FOUND;
    }

    return EFI_INVALID_PARAMETER;

}

/**
    Routine Searches device path by specific Type and SubType

    @param DevicePath A pointer to the device path protocol
    @param Type       Device path Node Type
    @param SubType    Device path node subtype

    @retval EFI_DEVICE_PATH_PROTOCOL*

**/

EFI_DEVICE_PATH_PROTOCOL*
SearchDevicePathNode (
  IN    OUT     EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN    UINT8   Type,
  IN    UINT8   SubType
)
{
    if (DevicePath == NULL) {
        return NULL;
    }

    while (!IsDevicePathEnd (DevicePath)) {
        if ((DevicePathType (DevicePath) == Type) && (DevicePathSubType (DevicePath) == SubType)) {
            return DevicePath;
        }
        DevicePath = NextDevicePathNode (DevicePath);
    }

    return NULL;
}

/**
    Used to allocate and build a device path node for an NVM Express namespace 
    on an NVM Express controller

    @param This         A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance
    @param NamespaceId  The NVM Express namespace ID for which a device path node is
                        to be allocated and built
    @param DevicePath   A pointer to a single device path node that describes the 
                        NVM Express namespace specified by NamespaceId. 

    @retval EFI_STATUS
**/
EFI_STATUS 
EFIAPI
BuildDevicePath (
    IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL    *This,
    IN UINT32                                NamespaceId,
    IN OUT EFI_DEVICE_PATH_PROTOCOL          **DevicePath
)
{
    EFI_STATUS                   Status;
    AMI_NVME_CONTROLLER_PROTOCOL *NvmeController;
    LIST_ENTRY                   *LinkData;
    ACTIVE_NAMESPACE_DATA        *ActiveNameSpaceData;
    EFI_DEVICE_PATH_PROTOCOL     *DevicePathProtocol;
    NVME_DEVICE_PATH              *NvmeDevicePathNode = NULL;
    
    if( !DevicePath || !This ) {
        return EFI_INVALID_PARAMETER;
    }

    Status = gBS->AllocatePool( EfiBootServicesData, 
                                sizeof(NVME_DEVICE_PATH),
                                (void**)DevicePath );
    if(EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
    }       
    
    NvmeController = NVME_CONTROLLER_PROTOCOL_FROM_THIS(This);
    
    for (LinkData = NvmeController->ActiveNameSpaceList.ForwardLink; 
            LinkData != &NvmeController->ActiveNameSpaceList; 
            LinkData = LinkData->ForwardLink) {
        
        ActiveNameSpaceData = BASE_CR(LinkData ,ACTIVE_NAMESPACE_DATA, Link);
        
        if(ActiveNameSpaceData->ActiveNameSpaceID == NamespaceId) {

            Status = gBS->HandleProtocol (
                        ActiveNameSpaceData->NvmeDeviceHandle,
                        &gEfiDevicePathProtocolGuid,
                        &DevicePathProtocol );

            if (EFI_ERROR(Status)) {
                return EFI_NOT_FOUND;
            }

             NvmeDevicePathNode = (NVME_DEVICE_PATH*)SearchDevicePathNode ( DevicePathProtocol, MESSAGING_DEVICE_PATH, MSG_NVME_DP);
            gBS->CopyMem( (UINT8*)*DevicePath, (UINT8*)NvmeDevicePathNode, sizeof(NVME_DEVICE_PATH) );
 
            return EFI_SUCCESS;
        }
    }
    
    return EFI_NOT_FOUND;
}

/**
    Used to translate a device path node to a Namespace ID

    @param This         A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance
    @param DevicePath   A pointer to the device path node that describes an NVM 
                        Express namespace on the NVM Express controller. 
    @param NamespaceId  The NVM Express namespace ID contained in the device path node

    @retval EFI_STATUS

**/
EFI_STATUS 
EFIAPI
GetNamespace (
    IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL    *This,
    IN EFI_DEVICE_PATH_PROTOCOL              *DevicePath,
    OUT UINT32                               *NamespaceId
)
{
    UINT16                       DevicePathLength = 0;
    if(!DevicePath || !This || !NamespaceId) {
        return EFI_INVALID_PARAMETER;
    }

    DevicePathLength = (DevicePath)->Length[0] | (DevicePath)->Length[1] << 8;

    if ((DevicePath->Type != MESSAGING_DEVICE_PATH) || 
        (DevicePath->SubType != MSG_NVME_DP) ||
        (DevicePathLength != sizeof(NVME_DEVICE_PATH))) {
        return EFI_UNSUPPORTED;
    }

    *NamespaceId = ((NVME_DEVICE_PATH* )DevicePath)->Nsid;
    
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
