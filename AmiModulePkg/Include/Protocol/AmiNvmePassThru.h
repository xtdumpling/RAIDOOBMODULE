//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AmiNvmePassThru.h
    Protocol Header file for the Nvme PassThru Protocol 

**/

#ifndef _NVME_PASSTHRU_PROTOCOL_H_
#define _NVME_PASSTHRU_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
    Execute Admin and Nvme cmds

    @param NvmeController 
    @param NvmeCmdWrapper 
    @param CmdCompletionData 

    @retval EFI_STATUS

    @note  If the cmd needs to be retried due to a failure, caller can 
    initialize the RetryCount.Can be called recursively.
**/
typedef EFI_STATUS
(EFIAPI *AMI_NVME_PASS_THRU_EXECUTECMD)(
    IN  AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController,
    IN  NVME_COMMAND_WRAPPER            *NvmeCmdWrapper,
    OUT COMPLETION_QUEUE_ENTRY          *CmdCompletionData
);

typedef struct {
    AMI_NVME_PASS_THRU_EXECUTECMD       ExecuteNvmeCmd;
} AMI_NVME_PASS_THRU_PROTOCOL;

extern EFI_GUID gAmiNvmePassThruProtocolGuid;
extern EFI_GUID gAmiSmmNvmePassThruProtocolGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
