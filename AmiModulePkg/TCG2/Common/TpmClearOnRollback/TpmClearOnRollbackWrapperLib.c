//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/TpmClearOnRollBack/TpmClearOnRollbackWrapperLib.c 1     5/23/13 7:49p Fredericko $
//
// $Revision: 1 $
//
// $Date: 5/23/13 7:49p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TCG/TpmClearOnRollBack/TpmClearOnRollbackWrapperLib.c $
//
// 1     5/23/13 7:49p Fredericko
// [TAG]        EIP121823
// [Category]   New Feature
// [Description]    Clear TPM on BIOS rollback
// [Files]          TpmClearOnRollback.cif
// TpmClearOnRollback.c
// TpmClearOnRollback.h
// TpmClearOnRollback.sdl
// TpmClearOnRollback.mak
// TpmClearOnRollback.dxs
// TpmClearOnRollbackSmiFlash.c
// TpmClearOnRollbackWrapperLib.c
// TpmClearOnRollbackWrapperLib.h
//**********************************************************************
//<AMI_FHDR_START>
//---------------------------------------------------------------------------
// Name:
//
//
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>
#include "TpmClearOnRollback.h"
#include "TpmClearOnRollbackWrapperLib.h"
#include <Protocol/FirmwareVolume2.h>
#include <AmiDxeLib.h>


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: TpmRecoveryGetFidFromFv
//
// Description:
//  Finds FID information in the Flash
//
// Input:
//
//
// Output:
//
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS TpmRecoveryGetFidFromFv(
    OUT VOID *Fid
)
{

    EFI_STATUS Status;
    EFI_HANDLE *FvHandle;
    UINTN FvCount;
    UINTN i;
    UINTN BufferSize;
    UINT8 *Buffer;
    static EFI_GUID FidFileName = FID_FFS_FILE_NAME_GUID;

    Status = pBS->LocateHandleBuffer(ByProtocol, &gEfiFirmwareVolume2ProtocolGuid, NULL, &FvCount, &FvHandle);
    if (EFI_ERROR(Status))
        return Status;

    for(i = 0; i < FvCount; i++)
    {
        EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
        UINT32 AuthStatus;
        Status = pBS->HandleProtocol(FvHandle[i], &gEfiFirmwareVolume2ProtocolGuid, &Fv);
        if (EFI_ERROR(Status))
            continue;
        Buffer = 0;
        BufferSize = 0;
        Status = Fv->ReadSection(Fv, &FidFileName, EFI_SECTION_FREEFORM_SUBTYPE_GUID, 0, &Buffer, &BufferSize, &AuthStatus);
//        DEBUG((DEBUG_INFO, "extracted section with guid %g\n", (EFI_GUID *)Buffer));
        if (!EFI_ERROR(Status))
        {
            Buffer+= sizeof(EFI_GUID);
            MemCpy(Fid, Buffer, sizeof(FW_VERSION));
            Buffer -= sizeof(EFI_GUID);
            pBS->FreePool(FvHandle);
            pBS->FreePool(Buffer);
            return EFI_SUCCESS;
        }
    }
    pBS->FreePool(FvHandle);
    return EFI_NOT_FOUND;
}



//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: TpmGetFidFromBuffer
//
// Description:
//  Finds FID information in provided buffer
//
// Input:
//
//
// Output:
//
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS TpmGetFidFromBuffer(
    OUT VOID *Fid,
    IN VOID *Buffer
)
{

    UINT32 Signature;
    UINT32 *SearchPointer;
    static EFI_GUID FidSectionGuid = FID_FFS_FILE_SECTION_GUID;

    SearchPointer = (UINT32 *)((UINT8 *)Buffer - sizeof(EFI_GUID) + FLASH_SIZE);
    Signature = FidSectionGuid.Data1;

    do
    {
        if(*SearchPointer == Signature)
        {
            if(!guidcmp(&FidSectionGuid, (EFI_GUID *)SearchPointer))
            {
                SearchPointer += sizeof(EFI_GUID);
                MemCpy(Fid, SearchPointer, sizeof(FW_VERSION));
                return EFI_SUCCESS;
            }
        }
    }
    while(SearchPointer-- >= (UINT32 *)Buffer);

    return EFI_NOT_FOUND;
}




//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SendTpmCommandRecovery
//
// Description:
//  Generic send TPM Transaction
//
// Input:
//
//
// Output:
//
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SendTpmCommandRecovery(UINT8 *Cmd, UINTN CmdSize,
                                  UINT8 *RetBuff, UINTN RetBuffSize)
{
    EFI_STATUS Status;
    TPM_TRANSMIT_BUFFER     InBuffer[1], OutBuffer[1];

    InBuffer[0].Buffer = Cmd;
    InBuffer[0].Size   = CmdSize;
    OutBuffer[0].Buffer = RetBuff;
    OutBuffer[0].Size   = RetBuffSize;

    Status = TpmLibPassThrough(((TPM_1_2_REGISTERS_PTR)(UINTN)TPM_BASE_ADDRESS),
                               sizeof (InBuffer)/sizeof (*InBuffer),
                               InBuffer,
                               sizeof (OutBuffer) / sizeof (*OutBuffer),
                               OutBuffer);

    return Status;

}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   RecoverySetPhysicalPresence
//
// Description:
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS RecoverySetPhysicalPresence()
{
    PHYSICAL_PRESENCE_CMD   PpCmd;
    TPM_1_2_RET_HEADER      retHeader;


    PpCmd.Hdr.Tag       = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    PpCmd.Hdr.ParamSize = TPM_H2NL( sizeof (PHYSICAL_PRESENCE_CMD));
    PpCmd.Hdr.Ordinal   = TPM_H2NL( TSC_ORD_PhysicalPresence );
    PpCmd.Data = TPM_H2NS(TPM_PHYSICAL_PRESENCE_CMD_ENABLE );

    MemSet(&retHeader, sizeof(TPM_1_2_RET_HEADER), 0);

    SendTpmCommandRecovery((UINT8 *)&PpCmd, sizeof(PHYSICAL_PRESENCE_CMD),
                           (UINT8 *)&retHeader, sizeof(TPM_1_2_RET_HEADER));

    //don't care about error case on setting physical presence CMD type
    //care about actual physical presence failure
    MemSet(&retHeader, sizeof(TPM_1_2_RET_HEADER), 0);

    PpCmd.Data  = TPM_H2NS(TPM_PHYSICAL_PRESENCE_PRESENT);

    SendTpmCommandRecovery((UINT8 *)&PpCmd, sizeof(PHYSICAL_PRESENCE_CMD),
                           (UINT8 *)&retHeader, sizeof(TPM_1_2_RET_HEADER));

    if(retHeader.RetCode == 0)
    {
        return EFI_SUCCESS;
    }

    return EFI_SECURITY_VIOLATION;

}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   RecoveryEnableActivate
//
// Description:
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS RecoveryEnableActivate()
{
    EFI_STATUS              Status;
    DATA_CMD                Command;
    TPM_1_2_CMD_HEADER      cmdEnable;
    TPM_1_2_RET_HEADER      retHeader;

    cmdEnable.Tag       = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdEnable.ParamSize = TPM_H2NL( sizeof (TPM_1_2_CMD_HEADER));
    cmdEnable.Ordinal   = TPM_H2NL( TPM_ORD_PhysicalEnable );

    MemSet(&retHeader, sizeof(TPM_1_2_RET_HEADER), 0);

    Status = SendTpmCommandRecovery((UINT8 *)&cmdEnable, sizeof(TPM_1_2_CMD_HEADER),
                                    (UINT8 *)&retHeader, sizeof(TPM_1_2_RET_HEADER));

    if(!EFI_ERROR(Status) && retHeader.RetCode == 0)
    {
        Command.Hdr.Tag       = TPM_H2NS( TPM_TAG_RQU_COMMAND );
        Command.Hdr.ParamSize = TPM_H2NL( sizeof (DATA_CMD));
        Command.Hdr.Ordinal   = TPM_H2NL( TPM_ORD_PhysicalSetDeactivated );
        Command.Data          = 0x0;

        MemSet(&retHeader, sizeof(TPM_1_2_RET_HEADER), 0);

        SendTpmCommandRecovery((UINT8 *)&Command, sizeof(DATA_CMD),
                               (UINT8 *)&retHeader, sizeof(TPM_1_2_RET_HEADER));

        if(retHeader.RetCode == 0)
        {
            return EFI_SUCCESS;
        }
    }

    return EFI_SECURITY_VIOLATION;

}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   RecoverySendClearCmd
//
// Description:
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
UINT8 RecoverySendClearCmd()
{
    EFI_STATUS              Status;
    TPM_1_2_CMD_HEADER      cmdClear;
    TPM_1_2_RET_HEADER      retHeader;

    cmdClear.Tag       = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdClear.ParamSize = TPM_H2NL( sizeof (TPM_1_2_CMD_HEADER));
    cmdClear.Ordinal   = TPM_H2NL( TPM_ORD_ForceClear );

    MemSet(&retHeader, sizeof(TPM_1_2_RET_HEADER), 0);

    Status = SendTpmCommandRecovery((UINT8 *)&cmdClear, sizeof(TPM_1_2_CMD_HEADER),
                                    (UINT8 *)&retHeader, sizeof(TPM_1_2_RET_HEADER));

    if(retHeader.RetCode == 0)
    {
        return TPM_CLEAR_SUCCESSFUL;
    }
    else if((retHeader.RetCode == TPM_H2NL(DISABLED))||
            (retHeader.RetCode == TPM_H2NL(DEACTIVATED)))
    {
        //we need to send enable and activate command
        //reset and then clear.
        Status = RecoveryEnableActivate();

        if(!EFI_ERROR(Status))
        {
            return TPM_CLEAR_RESET_REQUIRED;
        }
    }

    return TPM_FATAL_CONTINUE_FLASH;

}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   RecoverySendStartUpCmd
//
// Description:
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS RecoverySendStartUpCmd()
{
    TPM_1_2_CMD_STARTUP      cmdStartup;
    TPM_1_2_RET_HEADER       retHeader;
    TPM_STARTUP_TYPE         TpmSt = TPM_ST_CLEAR;

    cmdStartup.Header.Tag       = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdStartup.Header.ParamSize = TPM_H2NL( sizeof (cmdStartup));
    cmdStartup.Header.Ordinal   = TPM_H2NL( TPM_ORD_Startup );
    cmdStartup.StartupType      = TPM_H2NS( TpmSt );

    MemSet(&retHeader, sizeof(TPM_1_2_RET_HEADER), 0);

    SendTpmCommandRecovery((UINT8 *)&cmdStartup, sizeof(TPM_1_2_CMD_STARTUP),
                           (UINT8 *)&retHeader, sizeof(TPM_1_2_RET_HEADER));

    if(retHeader.RetCode == 0)
    {
        return EFI_SUCCESS;
    }

    return EFI_DEVICE_ERROR;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   GetCapabilityCmd
//
// Description:
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS GetCapabilityCmd(TPM_Capabilities_PermanentFlag *PermFlags)
{
    TPM_GetCapabilities_Input   cmdGetCap;

    cmdGetCap.Tag         = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdGetCap.ParamSize   = TPM_H2NL( sizeof (cmdGetCap));

    if(IsTcmSupportType())
    {
        //not supported
        return EFI_SUCCESS;
    }
    else
    {
        cmdGetCap.CommandCode = TPM_H2NL( TPM_ORD_GetCapability );
        cmdGetCap.caparea     = TPM_H2NL( TPM_CAP_FLAG );
    }

    cmdGetCap.subCapSize  = TPM_H2NL( 4 ); // subCap is always 32bit long
    cmdGetCap.subCap      = TPM_H2NL( TPM_CAP_FLAG_PERMANENT );

    SendTpmCommandRecovery((UINT8 *)&cmdGetCap, sizeof(TPM_GetCapabilities_Input),
                           (UINT8 *)PermFlags, sizeof(TPM_Capabilities_PermanentFlag));

    if(PermFlags->RetCode == TPM_H2NL(INVALID_POSTINIT))
    {
        return EFI_NOT_READY;
    }

    if(PermFlags->RetCode == 0)
    {
        return EFI_SUCCESS;
    }

    return EFI_DEVICE_ERROR;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   GetVolatileFlags
//
// Description:
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS GetVolatileFlags(TPM_VOLATILE_FLAGS *VolatileFlags)
{
    TPM_GetCapabilities_Input   cmdGetCap;

    cmdGetCap.Tag         = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdGetCap.ParamSize   = TPM_H2NL( sizeof (cmdGetCap));

    if(IsTcmSupportType())
    {
        return EFI_SUCCESS;
    }
    else
    {
        cmdGetCap.CommandCode = TPM_H2NL( TPM_ORD_GetCapability   );
        cmdGetCap.caparea     = TPM_H2NL( TPM_CAP_FLAG );
    }

    cmdGetCap.subCapSize  = TPM_H2NL( 4 ); // subCap is always 32bit long
    cmdGetCap.subCap      = TPM_H2NL( TPM_CAP_FLAG_VOLATILE  );

    SendTpmCommandRecovery((UINT8 *)&cmdGetCap, sizeof(TPM_GetCapabilities_Input),
                           (UINT8 *)VolatileFlags, sizeof(TPM_VOLATILE_FLAGS));

    if(VolatileFlags->RetCode == TPM_H2NL(INVALID_POSTINIT))
    {
        return EFI_NOT_READY;
    }

    if(VolatileFlags->RetCode == 0)
    {
        return EFI_SUCCESS;
    }

    return EFI_DEVICE_ERROR;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   ClearTpmBeforeFlash
//
// Description:
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
UINT8 ClearTpmBeforeFlash()
{
    TPM_Capabilities_PermanentFlag PermFlags;
    UINT8                          Result=TPM_FATAL_CONTINUE_FLASH;
    EFI_STATUS                     Status;
    TPM_VOLATILE_FLAGS             TpmVolatileFlags;

    if(IsTcmSupportType())return TPM_CLEAR_SUCCESSFUL;

    Status = IsTpmPresent((TPM_1_2_REGISTERS_PTR)( UINTN ) TPM_BASE_ADDRESS);
    if(EFI_ERROR(Status))
    {
        return TPM_CLEAR_SUCCESSFUL;
    }

    //else we have a TPM need to process to clear
    TisRequestLocality((TPM_1_2_REGISTERS_PTR)( UINTN ) TPM_BASE_ADDRESS);

    Status = GetCapabilityCmd(&PermFlags);
    if(Status == EFI_NOT_READY)
    {
        Status = RecoverySendStartUpCmd();
        if(EFI_ERROR(Status))
        {
            return TPM_FATAL_CONTINUE_FLASH;
        }
    }

    if(EFI_ERROR(Status))return TPM_FATAL_CONTINUE_FLASH;

    Status =  RecoverySetPhysicalPresence();
    if(!EFI_ERROR(Status))
    {
        Status = GetVolatileFlags(&TpmVolatileFlags);
        if(!EFI_ERROR(Status))
        {
            if(TpmVolatileFlags.physicalPresenceLock)
            {
                //we need to go into manufacturing mode\special mode
                //to skip physical presence lock; Return we failed
                //to clear TPM and flashing should be discontinued.
                return TPM_FATAL_DISCONTINUE_FLASH;
            }
        }
        Result = RecoverySendClearCmd();
    }
    else
    {
        if(EFI_ERROR(Status))return TPM_FATAL_CONTINUE_FLASH;
    }

    TisReleaseLocality((TPM_1_2_REGISTERS_PTR)( UINTN ) TPM_BASE_ADDRESS);

    return Result;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
