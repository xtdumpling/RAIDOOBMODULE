//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
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
/** @file MEFwUpdatePlatformLib.c
**/
//**********************************************************************
//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <AmiDxeLib.h>
#include <Protocol/MeFwUpdLclProtocol.h>
#include <CoreBiosMsg.h>
#include <Token.h>
#include <SeCState.h>
#include <Protocol/Heci.h>
#include <Protocol/PchReset.h>
#include <Library/HeciMsgLib.h>
#include "FWUpdateLib.h"
#include "me_status.h"

//----------------------------------------------------------------------------
// Function Externs

//----------------------------------------------------------------------------
// Local prototypes
typedef struct _PCH_PLATFORM_POLICY  PRIVATE_PCH_PLATFORM_POLICY_PROTOCOL;

// Last Reset Types
#define NORESET 0
#define HOSTRESET  1
#define MERESET  2
#define GLOBALRESET 3

// Get Interface
#define FW_UPDATE_DISABLED 0
#define FW_UPDATE_ENABLED 1
#define FW_UPDATE_PASSWORD_PROTECTED 2

//----------------------------------------------------------------------------
// Local Variables
PRIVATE_PCH_PLATFORM_POLICY_PROTOCOL    *gPchPlatformPolicy = NULL;
_UUID   mOemId = ME_FW_OEM_ID;
EFI_BOOT_SERVICES   *BS;

//----------------------------------------------------------------------------
// Function Definitions
/**
 * This hook updates the BiosLock function of the PchPlatformPolicy to disabled.
 * @param This A pointer to the ME_FW_UPDATE_LOCAL_PROTOCOL instance.
 * @param UnlockType Type to be set.
 * @retval EFI_STATUS Return the EFI Stauts
**/
EFI_STATUS
MeUpdPlatformUnlock (
  IN ME_FW_UPDATE_LOCAL_PROTOCOL *This,
  IN UINT32                      UnlockType
)
{
    // ==== PORTING REQUEST ==== 
    // if (gPchPlatformPolicy != NULL) gPchPlatformPolicy->LockDownConfig.BiosLock = 0;
    // Patch disabling IDE_R if ME is Disabled for system assert if DEBUG_MODE is ON.
    // The IDE-R device will be disabled if ME is in Normal state (HeciInit.c), 
    // Here ME is in ME_MODE_SECOVER, the IDE-R is active and could cause assert error
    // in IdeBus.Start proceduce.
    // MeDeviceControl (IDER, Disabled);
    // ==== PORTING REQUEST ==== 
    return EFI_SUCCESS;
}

/**
 * This hook is callback fucntion for PchPolicyProtocol.
 * @param Event The EFI event
 * @param Context Context for the callback
 * @retval EFI_STATUS Return the EFI Stauts
**/
EFI_STATUS
MeFwUpdatePchPolicyEvent  (
    IN EFI_EVENT        Event,
    IN EFI_HANDLE       ImageHandle
    )
{
    // EFI_STATUS          Status;
    // Status = pBS->LocateProtocol(&gPchPlatformPolicyProtocolGuid, \
    //                                             NULL, &gPchPlatformPolicy);
    // if (EFI_ERROR(Status)) return EFI_SUCCESS;
    // pBS->CloseEvent(Event);
    return EFI_SUCCESS;
}

/**
 * This hook initial for the ME_FW_UPDATE_LOCAL_PROTOCOL.
**/
VOID MeFwUpdateLibInit (VOID)
{
    // static EFI_EVENT    gEvtPchPolicy;
    // static VOID         *gRgnPchPolicy;
    // Create PchPlatformPolicy Callback for unlocking BIOS_LOCK. 
    // RegisterProtocolCallback ( &gPchPlatformPolicyProtocolGuid, \
    //                            MeFwUpdatePchPolicyEvent, \
    //                            NULL, \
    //                            &gEvtPchPolicy, \
    //                            &gRgnPchPolicy  );
    // MeFwUpdatePchPolicyEvent(gEvtPchPolicy, NULL);                                    
    return ;
}

/**
 * This hook resets system via PchResetProtocol.
 * @param This A pointer to the ME_FW_UPDATE_LOCAL_PROTOCOL instance.
 * @param ResetType Type to be reset. 
 * @retval EFI_SUCCESS Command succeeded
**/
EFI_STATUS
MeUpdPlatformReset (
  IN ME_FW_UPDATE_LOCAL_PROTOCOL    *This,
  IN EFI_RESET_TYPE                 ResetType
)
{
    EFI_STATUS          Status;
    PCH_RESET_PROTOCOL  *PchReset;
    // Reset system to re-start ME FW..
    Status = pBS->LocateProtocol (&gPchResetProtocolGuid, NULL, (VOID **) &PchReset);
    if (!EFI_ERROR(Status)) PchReset->Reset(PchReset, GlobalResetWithEc);
    pRS->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
    return EFI_SUCCESS;
}

/**
 * This hook checks the ME FW state.
 * @param This A pointer to the ME_FW_UPDATE_LOCAL_PROTOCOL instance.
 * @param MeFwState ME FW state to be checked. 
 * @retval EFI_SUCCESS ME FW state match.
 * @retval EFI_UNSUPPORTED ME FW state not match.
**/
EFI_STATUS
MeCheckFwState(
  IN ME_FW_UPDATE_LOCAL_PROTOCOL *This,
  IN ME_FW_STATE                 MeFwState
)
{
    EFI_STATUS          Status;
    UINT32              MeMode;
    EFI_HECI_PROTOCOL   *Heci = NULL;

    Status = pBS->LocateProtocol (&gEfiHeciProtocolGuid, NULL, (VOID**)&Heci);
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
    Status = Heci->GetSeCMode (&MeMode);
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
    if (((MeFwState == MeModeNormal) && (MeMode == SEC_MODE_NORMAL)) || \
        ((MeFwState == MeModeDebug) && (MeMode == SEC_DEBUG_MODE_ALT_DIS)) || \
        ((MeFwState == MeModeTempDisabled) && (MeMode == SEC_MODE_TEMP_DISABLED)) || \
        ((MeFwState == MeModeSecOver) && (MeMode == SEC_MODE_SECOVER)) || \
        ((MeFwState == MeModeFailed) && (MeMode == SEC_MODE_FAILED))) return EFI_SUCCESS;
    return EFI_UNSUPPORTED;
}

/**
 * This hook gets the ME FW version from MeBiosPayloadData Protocol..
 * @param This A pointer to the ME_FW_UPDATE_LOCAL_PROTOCOL instance.
 * @param MeFwVersion ME FW Version returned. 
 * @retval EFI_SUCCESS Get ME FW Version succeeded
**/
EFI_STATUS
MeGetFwVersion (
  IN ME_FW_UPDATE_LOCAL_PROTOCOL *This,
  IN ME_FW_UPD_VERSION           *MeFwVersion
)
{
    EFI_STATUS          Status = EFI_SUCCESS;
    UINT32              Length;
    GEN_GET_FW_VER      MsgGenGetFwVersion;
    GEN_GET_FW_VER_ACK  MsgGenGetFwVersionAck;
    EFI_HECI_PROTOCOL   *Heci;

    if (EFI_ERROR(MeCheckFwState(This, MeModeNormal))) return EFI_UNSUPPORTED;
    Status = pBS->LocateProtocol (&gEfiHeciProtocolGuid, NULL, (VOID**)&Heci);
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
    // Allocate MsgGenGetFwVersion data structure
    MsgGenGetFwVersion.MKHIHeader.Data = 0;
    MsgGenGetFwVersion.MKHIHeader.Fields.GroupId = MKHI_GEN_GROUP_ID;
    MsgGenGetFwVersion.MKHIHeader.Fields.Command = GEN_GET_FW_VERSION_CMD;
    MsgGenGetFwVersion.MKHIHeader.Fields.IsResponse = 0;
    Length = sizeof (GEN_GET_FW_VER);
    // Send Get Firmware Version Request to SEC
    Status = Heci->SendMsg ((UINT32*)&MsgGenGetFwVersion, \
                        Length, BIOS_FIXED_HOST_ADDR, HECI_CORE_MESSAGE_ADDR);
    if (EFI_ERROR (Status)) return Status;
    Length = sizeof (GEN_GET_FW_VER_ACK);
    Status = Heci->ReadMsg (BLOCKING, (UINT32*)&MsgGenGetFwVersionAck, &Length);
    if (EFI_ERROR (Status)) return Status;
    MeFwVersion->MajorVersion   = MsgGenGetFwVersionAck.Data.CodeMajor;
    MeFwVersion->MinorVersion   = MsgGenGetFwVersionAck.Data.CodeMinor;
    MeFwVersion->HotfixVersion  = MsgGenGetFwVersionAck.Data.CodeHotFix;
    MeFwVersion->BuildVersion   = MsgGenGetFwVersionAck.Data.CodeBuildNo;
    return Status;}

/**
 * This hook sends HMRFPO Enable MEI to disable ME/TXE FW for Firmware Update.
 * @param This A pointer to the ME_FW_UPDATE_LOCAL_PROTOCOL instance.
 * @retval EFI_SUCCESS Command succeeded
 * @retval EFI_DEVICE_ERROR HECI Device error, command aborts abnormally
 * @retval EFI_TIMEOUT HECI does not return the buffer before timeout
**/
EFI_STATUS
MeUpdHmrfpoEnable (
    ME_FW_UPDATE_LOCAL_PROTOCOL *This
)
{
    EFI_STATUS          Status = EFI_UNSUPPORTED;
    EFI_HECI_PROTOCOL   *Heci = NULL;
    MKHI_HMRFPO_ENABLE          HmrfpoEnableRequest;
    MKHI_HMRFPO_ENABLE_RESPONSE HmrfpoEnableResponse;
    CBM_RESET_REQ               CbmResetRequest;
    UINT32                      HeciLength;
    
    if (EFI_ERROR(MeCheckFwState(This, MeModeNormal))) return EFI_UNSUPPORTED;
    Status = pBS->LocateProtocol (&gEfiHeciProtocolGuid, NULL, (VOID**)&Heci);
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
    HmrfpoEnableRequest.MkhiHeader.Data               = 0;
    HmrfpoEnableRequest.MkhiHeader.Fields.GroupId     = MKHI_SPI_GROUP_ID;
    HmrfpoEnableRequest.MkhiHeader.Fields.Command     = HMRFPO_ENABLE_CMD_ID;
    HmrfpoEnableRequest.MkhiHeader.Fields.IsResponse  = 0;
    HmrfpoEnableRequest.Nonce                         = 0; //Nonce;
    HeciLength = sizeof (MKHI_HMRFPO_ENABLE);
    Status = Heci->SendMsg ((UINT32*)&HmrfpoEnableRequest, \
                    HeciLength, BIOS_FIXED_HOST_ADDR, HECI_CORE_MESSAGE_ADDR);
    if (EFI_ERROR (Status)) return Status;
    HeciLength = sizeof (MKHI_HMRFPO_ENABLE_RESPONSE);
    Status = Heci->ReadMsg (BLOCKING, (UINT32*)&HmrfpoEnableResponse, &HeciLength);
    if (EFI_ERROR (Status) || \
        (HmrfpoEnableResponse.Status != HMRFPO_ENABLE_SUCCESS)) return Status;
    // (A6) The BIOS sends the GLOBAL RESET MEI message
    CbmResetRequest.MKHIHeader.Data               = 0;
    CbmResetRequest.MKHIHeader.Fields.Command     = CBM_RESET_REQ_CMD;
    CbmResetRequest.MKHIHeader.Fields.IsResponse  = 0;
    CbmResetRequest.MKHIHeader.Fields.GroupId     = MKHI_CBM_GROUP_ID;
    CbmResetRequest.MKHIHeader.Fields.Reserved    = 0;
    CbmResetRequest.MKHIHeader.Fields.Result      = 0;
    CbmResetRequest.Data.RequestOrigin            = CBM_RR_REQ_ORIGIN_BIOS_POST;
    CbmResetRequest.Data.ResetType                = CBM_HRR_GLOBAL_RESET;
    HeciLength = sizeof (CBM_RESET_REQ);
    Status = Heci->SendMsg ((UINT32*)&CbmResetRequest, \
                    HeciLength, BIOS_FIXED_HOST_ADDR, HECI_CORE_MESSAGE_ADDR);
    MeUpdPlatformReset(This, PowerCycleReset);
    CpuDeadLoop  ();
    return Status;
}

/**
 * This dummy hook for FWUPdateLib used only.
**/
void _fltused()
{
}

/**
 * This dummy hook for FWUPdateLib used only.
 * @param BytesSent 
 * @param BytestobeSent 
**/

void
DisplaySendStatus (
  float     BytesSent,
  float     BytestobeSent
  )
{
//    float   value = (BytesSent / BytestobeSent) * 100;
//    UINT32  pValue = (UINT32)value;
//
//    if (pValue != 100)
//    {
//        DEBUG ((D_ERROR, "Sending the update image to FW for verification:  [ %d%% ]\r", pValue));
//    }
//    else
//    {
//        DEBUG ((D_ERROR, "Sending the update image to FW for verification:  [ COMPLETE ] \n"));
//    }
}

/**
 * This hook update the ME/TXE Firmware via FWUpdateLib.
 * @param FileBuffer buffer to be updated.
 * @param FileLength length to be updated.
 * @retval EFI_SUCCESS Command succeeded
**/
EFI_STATUS
FwUpdLclFunc (
    ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  UINT8                   *FileBuffer,
    IN  UINTN                   FileLength,
    IN  UINT8                   FirmwareType
)
{
    EFI_STATUS          Status;
    UINTN               ImageLength = 0;
    UINT8               *ImageBuffer = NULL;
    UPDATE_FLAGS_LIB    update_flags;
    CHAR8               Password[9];
    UINT32              FWUpdateStatus;
    UINT32              QueryStatus;
    UINT32              UpdateStatus = 0;
    UINT32              TotalStages = 0;
    UINT32              PercentWritten = 0;
    UINT32              LastStatus;
    UINT32              LastResetType;
    UINT16              Interfaces;
    // UINT8               Symbol;
    // UINT32              Index;
    UINT32              CheckPolicyStatus = 0;
    BOOLEAN             AllowSV = FALSE;
    UPDATE_TYPE         UpdType;
    VersionLib          Ver;
    VOID                *DisplayHandle;
    AMI_POST_MGR_KEY    OutKey;

    ImageBuffer = FileBuffer;
    ImageLength = FileLength;

    BS = pBS;
    ZeroMem( &update_flags, sizeof(UPDATE_FLAGS_LIB) );
    
//-    if(DummyBs != NULL) {
//-        BackupBs = pBS;
//-        BS = DummyBs;
//-    }

    Status = GetLastStatus( &LastStatus );
    if( EFI_ERROR(Status) )
    {
        return Status;
    }
    
    if( LastStatus == STATUS_UPDATE_HOST_RESET_REQUIRED )
    {
        //DEBUG ((D_ERROR, "PLEASE REBOOT YOUR SYSTEM. "));
        //DEBUG ((D_ERROR, "Firmware update cannot be initiated without a reboot.\n"));
        return EFI_SUCCESS;
    }

    if( IsUpdateStatusPending(LastStatus) )
    {
        //DEBUG ((D_ERROR, "Firmware Update operation not initiated "));
        //DEBUG ((D_ERROR, "because a firmware update is already in progress\n"));
        return EFI_SUCCESS;
    }

    Status = GetLastUpdateResetType( &LastResetType );
    if( EFI_ERROR(Status) ) return Status;
    switch( LastResetType )
    {
        case HOSTRESET:
        case GLOBALRESET:
            //DEBUG ((D_ERROR, "PLEASE REBOOT YOUR SYSTEM. "));
            //DEBUG ((D_ERROR, "Firmware update cannot be initiated without a reboot.\n"));
            return EFI_SUCCESS;
            break;
        default:
            break;
	}

    Status = GetInterfaces( &Interfaces );
    if( EFI_ERROR(Status) ) return Status;
    
    switch( Interfaces )
    {
        case FW_UPDATE_DISABLED:
            //DEBUG ((D_ERROR, "Local FWUpdate is Disabled\n"));
            return EFI_SUCCESS;
        case FW_UPDATE_PASSWORD_PROTECTED:
            //DEBUG ((D_ERROR, "Local FWUpdate is Password Protected\n"));
            break;
        case FW_UPDATE_ENABLED:
            break;
        default:
            break;
    }

    //DEBUG ((D_ERROR, "Checking firmware parameters...\n"));

    CheckPolicyStatus = CheckPolicyBuffer( (char *)ImageBuffer,
                                           (INT32)ImageLength,
                                           (INT32)AllowSV,
                                           &UpdType,
                                           &Ver );
    if (EFI_ERROR(CheckPolicyStatus)) return EFI_NOT_READY;
    switch( UpdType )
    {
        case DOWNGRADE_SUCCESS:
        case SAMEVERSION_SUCCESS:
        case UPGRADE_SUCCESS:
            break;

        case DOWNGRADE_FAILURE:
            //DEBUG ((D_ERROR, "FW Update downgrade not allowed\n"));
            return EFI_SUCCESS;
            break;

        case SAMEVERSION_FAILURE:
            //DEBUG ((D_ERROR, "FW Update same version not allowed, specify /s on command line\n"));
            return EFI_SUCCESS;

        default:
            break;
    }

    ZeroMem( &Password, sizeof(Password) );
    
    if (This->AmiPostMgr != NULL) {
        This->AmiPostMgr->DisplayProgress (AMI_PROGRESS_BOX_INIT,
                                     L"ME FW update", \
                                     L"Flash New ME Firmware", \
                                     NULL, 0, &DisplayHandle, &OutKey);
    }
        
    FWUpdateStatus = FwUpdateFullBuffer(
                        (char*)ImageBuffer,
                        (unsigned int)ImageLength,
                        Password,
                        0,
                        FWU_ENV_MANUFACTURING,
                        mOemId,
                        update_flags,
                        &DisplaySendStatus );
    if( FWUpdateStatus != FWU_ERROR_SUCCESS )
    {
        //DEBUG ((D_ERROR, "FWUpdateStatus: %x\n", FWUpdateStatus));
        //if( ImageBuffer )
        //    FreePool( ImageBuffer );
        return EFI_SUCCESS;
    }

    //if( ImageBuffer )
    //{
    //    Status = FreePool( ImageBuffer );
    //}

    // Index = 0;
    do
    {
        // Symbol = (++Index % 2 == 0) ? '|':'-';
    
        QueryStatus = FWUpdate_QueryStatus_Get_Response(
                            &UpdateStatus,
                            &TotalStages,
                            &PercentWritten,
                            &LastStatus,
                            &LastResetType );
        if( QueryStatus == FWU_ERROR_SUCCESS )
        {
            //DEBUG ((D_ERROR, "FW Update:  [ %d%% (Stage: %d of %d) (%c)]\r", PercentWritten, UpdateStatus, TotalStages, Symbol));
            if (This->AmiPostMgr != NULL)
                This->AmiPostMgr->DisplayProgress (AMI_PROGRESS_BOX_UPDATE,
                                     L"ME FW update", \
                                     L"Flash New ME Firmware", \
                                     NULL, PercentWritten, &DisplayHandle, &OutKey);
        }
        else if ( LastStatus != STATUS_UPDATE_NOT_READY )
        {
            //DEBUG ((D_ERROR, "\nLastStatus: %x\n", LastStatus));
            break;
        }
        BS->Stall(100000);
    } while( (PercentWritten < 100) && (QueryStatus == FWU_ERROR_SUCCESS) );

//-    BS = BackupBs;
    if (This->AmiPostMgr != NULL)
        This->AmiPostMgr->DisplayProgress (AMI_PROGRESS_BOX_CLOSE,
                                     L"ME FW update", \
                                     L"Flash New ME Firmware", \
                                     NULL, 0, &DisplayHandle, &OutKey);

    return EFI_SUCCESS;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
