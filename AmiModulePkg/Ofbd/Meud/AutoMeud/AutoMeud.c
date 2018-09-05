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
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log:
// 
//
//**********************************************************************
/** @file AutoMeud.c

**/
//**********************************************************************
//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/AMIPostMgr.h>
#include <Protocol/MeFwUpdLclProtocol.h>
#include <Guid/DxeServices.h>
#include <Flash.h>
#include <Library/IoLib.h>
#include <Uefi/UefiSpec.h>

//----------------------------------------------------------------------------
// Function Externs
static EFI_GUID gMeFwUpdVariableGuid = ME_FW_UPD_VARIABLE_GUID;
extern ME_FW_UPD_VERSION  gMeFwImgVersion[];

//----------------------------------------------------------------------------
// Local prototypes
//-typedef struct _PCH_PLATFORM_POLICY  PRIVATE_PCH_PLATFORM_POLICY_PROTOCOL;
#if !defined(MEUD_NONBIOS_LENGTH)
#define MEUD_NONBIOS_LENGTH 0
#endif
#define ME_UPD_LCL_SIGNATURE SIGNATURE_32 ('_', 'M', 'U', 'L')
EFI_STATUS 
MeFwUpdateViaIntelLib(
    IN  UINT8                       *pBuffer,
    IN  AMI_POST_MANAGER_PROTOCOL   *AmiPostMgr
);
//----------------------------------------------------------------------------
// Local Variables

#define BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID \
 { 0xDBC9FD21, 0xFAD8, 0x45B0, 0x9E, 0x78, 0x27, 0x15, 0x88, 0x67, 0xCC, 0x93 }

static EFI_GUID gBdsAllDriversConnectedProtocolGuid = BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID;

static EFI_GUID gMeRegionFfsGuid = \
{0xDE90FFA8, 0xB985, 0x4575, 0xAB, 0x8D, 0xAD, 0xE5, 0x2C, 0x36, 0x2C, 0xA3};
static EFI_GUID gConOutStartedProtocolGuid = CONSOLE_OUT_DEVICES_STARTED_PROTOCOL_GUID;
static EFI_GUID AmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
static EFI_GUID gMeFwUpdLclProtocolGuid = ME_FW_UPD_LOCAL_PROTOCOL_GUID;
EFI_RESET_SYSTEM gSavedResetSystem = NULL;
ME_FW_UPDATE_LOCAL_PROTOCOL     *gMeFwUpdProtocol = NULL;

#if (AUTO_ME_UPDATE_POLICY != 0)
#if !defined(FLASH_AREA_EX)
#define FLASH_AREA_EX         VOID
#endif
FLASH_AREA_EX *BlocksToUpdate;
#endif
UINT8         *RecoveryBuffer = NULL;
//----------------------------------------------------------------------------
// Function Definitions
#define FLASH_BASE_ADDRESS_X(a) (UINTN)(0xFFFFFFFF - (MEUD_NONBIOS_LENGTH + FLASH_SIZE) + 1 + (UINTN)a)
/**
    Send Dummy Reset

    @param ResetType 
    @param ResetStatus 
    @param DataSize 
    @param ResetData OPTIONAL

    @retval EFI_STATUS Return the EFI Stauts

**/
EFI_STATUS
EFIAPI
DummyResetSystem(
    IN  EFI_RESET_TYPE  ResetType,
    IN  EFI_STATUS      ResetStatus,
    IN  UINTN           DataSize,
    IN  CHAR16          *ResetData OPTIONAL
)
{
    return EFI_SUCCESS;
}
/**

    @param ProgressBoxState 
    @param Title 
    @param Message 
    @param Legend 
    @param Percent 
    @param Handle 
    @param OutKey 

    @retval EFI_STATUS Return the EFI Stauts

**/
EFI_STATUS
EFIAPI
DisplayTextProgress(
    IN  UINT8	            ProgressBoxState, 
    IN  CHAR16	            *Title,
    IN  CHAR16	            *Message,
    IN  CHAR16	            *Legend,
    IN  UINTN 	            Percent,
    IN  OUT VOID	        **Handle,
    OUT AMI_POST_MGR_KEY    *OutKey
)
{
    CHAR16  Messages[80];
    Swprintf( Messages, L"ME Update in Progress ... %d%c    \r", Percent, L'%' );
    pST->ConOut->OutputString( pST->ConOut, Messages );
    return EFI_SUCCESS;
}
/**

    @param Event 
    @param ImageHandle 

    @retval EFI_STATUS Return the EFI Stauts

**/
EFI_STATUS
EFIAPI
MeAutoUpdate2ConOutReadyEvent(
    IN  EFI_EVENT   Event,
    IN  EFI_HANDLE  ImageHandle
)
{
    UINT32                      Dummy = 0;
    AMI_POST_MANAGER_PROTOCOL   AmiPostMgr;
    
    pBS->CloseEvent( Event );
    // Delete "MeAutoUpdateReq" variable then starting Update Processes.
    pRS->SetVariable( L"MeAutoUpdateReq", &gMeFwUpdVariableGuid, \
          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS, 0, &Dummy );

    // Provided the Text Progress Display hook.
    AmiPostMgr.DisplayProgress = DisplayTextProgress;      

    // Start ME FW update process.
    MeFwUpdateViaIntelLib( (UINT8*)FLASH_BASE_ADDRESS_X(0), &AmiPostMgr );

    // ME Update completed, issue a global to take effect.
    gMeFwUpdProtocol->PlatformReset( gMeFwUpdProtocol, EfiResetPlatformSpecific );
    return EFI_SUCCESS;
}
/**

    @param Event 
    @param ImageHandle 

    @retval EFI_STATUS Return the EFI Stauts

**/
EFI_STATUS
EFIAPI
MeAutoUpdateConOutReadyEvent(
    IN  EFI_EVENT   Event,
    IN  EFI_HANDLE  ImageHandle
)
{
    UINT8                           *pMeRegion = NULL;                            
    UINTN                           Number , i, MeRegionLength = 0;   
    UINT32                          Authentication;
    EFI_HANDLE                      *Handle = NULL;
    EFI_STATUS                      Status;
    EFI_FIRMWARE_VOLUME_PROTOCOL    *Fv;
    AMI_POST_MANAGER_PROTOCOL       *AmiPostMgr = NULL;
    VOID                            *DisplayHandle;
    AMI_POST_MGR_KEY                OutKey;

    pBS->CloseEvent( Event );
        // Delete "MeAutoUpdateReq" variable then starting Update Processes.
    pRS->SetVariable( L"MeAutoUpdateReq", &gMeFwUpdVariableGuid, \
          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS, 0, &i );

    Status = pBS->LocateHandleBuffer( ByProtocol, \
                    &gEfiFirmwareVolume2ProtocolGuid, NULL, &Number, &Handle );

    for( i = 0; i < Number; i++ )
    {
        Status = pBS->HandleProtocol( Handle[i], \
                            &gEfiFirmwareVolume2ProtocolGuid, (VOID**) &Fv );
        if( EFI_ERROR(Status) ) continue;

        Status = Fv->ReadSection( Fv, &gMeRegionFfsGuid, EFI_SECTION_RAW, \
                    0, (VOID**)&pMeRegion, &MeRegionLength,  &Authentication );
        if( !EFI_ERROR(Status) ) break;
    }
    if( Handle != NULL ) pBS->FreePool( Handle );
    if( EFI_ERROR(Status) || (MeRegionLength != MEUD_NONBIOS_LENGTH) )
    {
        if( pMeRegion != NULL ) pBS->FreePool( pMeRegion );
        return EFI_SUCCESS;
    }

    Status = pBS->LocateProtocol( &AmiPostManagerProtocolGuid, NULL, &AmiPostMgr );
    if( EFI_ERROR(Status) ) AmiPostMgr = NULL;
    if( AmiPostMgr != NULL )
    {
        pST->ConOut->ClearScreen( pST->ConOut );
        AmiPostMgr->DisplayProgress( AMI_PROGRESS_BOX_INIT, \
                    L"ME Update Progress", NULL, NULL, 0, &DisplayHandle, &OutKey );
    }

    FlashDeviceWriteEnable();
    for( i = 0, Number = 0xFF; i < (MeRegionLength / FLASH_BLOCK_SIZE); i++ )
    {
        volatile UINT8 *pMeRegionAddr = (volatile UINT8*)FLASH_BASE_ADDRESS_X(i * FLASH_BLOCK_SIZE);
        if( AmiPostMgr != NULL )
        {
            AmiPostMgr->DisplayProgress(
                            AMI_PROGRESS_BOX_UPDATE,
                            L"ME Update Progress",
                            L"Flash new data",
                            NULL,
                            (i * 100) / (MeRegionLength / FLASH_BLOCK_SIZE),
                            &DisplayHandle,
                            &OutKey );        
        }
        else
        {
            CHAR16 Messages[80];
            if( (Number != (i * 100) / (MeRegionLength / FLASH_BLOCK_SIZE)) )
            {
                Number = (i * 100) / (MeRegionLength / FLASH_BLOCK_SIZE);
                Swprintf( Messages, L"ME Update in Progress ... %d%c    \r", Number, L'%' );
                pST->ConOut->OutputString( pST->ConOut, Messages );
            }
        }
        FlashEraseBlock( pMeRegionAddr );
        FlashProgram( pMeRegionAddr, \
                        pMeRegion + (i * FLASH_BLOCK_SIZE), FLASH_BLOCK_SIZE );
    }
    FlashDeviceWriteDisable();
 
    if( AmiPostMgr != NULL )
    {
        AmiPostMgr->DisplayProgress( AMI_PROGRESS_BOX_CLOSE, \
                 L"ME Update Progress", NULL, NULL, 0, &DisplayHandle, &OutKey );
    }
    if( pMeRegion != NULL ) pBS->FreePool( pMeRegion );
    // ME Update completed, issue a global to take effect.
    gMeFwUpdProtocol->PlatformReset( gMeFwUpdProtocol, EfiResetPlatformSpecific );
    return EFI_SUCCESS;
}
/**

    @param Event 
    @param ImageHandle 

    @retval EFI_STATUS Return the EFI Stauts

**/
EFI_STATUS
EFIAPI
MeFwUpdateProtocolEvent(
    IN  EFI_EVENT   Event,
    IN  EFI_HANDLE  ImageHandle
)
{
    UINT32              Dummy = 0;                            
    UINTN               VariableSize = sizeof(UINT8);   
    EFI_STATUS          Status;
    static EFI_EVENT    gEvtConOut;
    static VOID         *gRgnConOut;
    ME_FW_UPD_VERSION   MeFwVersion;

    // Check ME state if ME is in ME_MODE_SECOVER mode only.
    Status = pBS->LocateProtocol( &gMeFwUpdLclProtocolGuid, \
                                        NULL, (VOID **) &gMeFwUpdProtocol );
    if( EFI_ERROR(Status) ) return Status;
    pBS->CloseEvent( Event );

    if( gMeFwImgVersion->MajorVersion != 0 )
    {
        Status = gMeFwUpdProtocol->MeFwGetVersion( gMeFwUpdProtocol, &MeFwVersion );
        if( !EFI_ERROR (Status) )
        {
            VariableSize = sizeof(ME_FW_UPD_VERSION);
            // Set MbpMeFwVersion to "NV+BS", because MBP could be not available if
            // Capsule Update mode.
            pRS->SetVariable(
                    L"MbpMeFwVersion",
                    &gMeFwUpdVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof(ME_FW_UPD_VERSION),
                    &MeFwVersion );
        }
    }
    // Check "MeAutoUpdateReq" variable if ME Auto Update is requested. 
    VariableSize = sizeof(UINT32);
    Status = pRS->GetVariable(
                    L"MeAutoUpdateReq",
                    &gMeFwUpdVariableGuid,
                    NULL,
                    &VariableSize,
                    &Dummy );
    if( EFI_ERROR(Status) ) return EFI_SUCCESS;

    if( Dummy == ME_UPD_LCL_SIGNATURE )
    {
        // ******** -------------------------------- ***********
        // ******** ME FW update via Intel Flash API ***********
        // ******** -------------------------------- ***********
        RegisterProtocolCallback( &gConOutStartedProtocolGuid, \
            MeAutoUpdate2ConOutReadyEvent, NULL, &gEvtConOut, &gRgnConOut );
        return EFI_SUCCESS;                                    
    }

    if( !EFI_ERROR(gMeFwUpdProtocol->MeFwCheckMode( gMeFwUpdProtocol, MeModeNormal )) )
    {
        if( !EFI_ERROR(gMeFwUpdProtocol->HmrfpoEnable( gMeFwUpdProtocol )) )
            gMeFwUpdProtocol->PlatformReset( gMeFwUpdProtocol, EfiResetPlatformSpecific );
    }

    // Here we hook the ResetSystem of RuntimeService to avoid the system reset by 
    // re-flashing Nvram causes Hardware Configurations changed. (HbCspAdjustMemoryMmioOverlap)
    gSavedResetSystem = pRS->ResetSystem;
    pRS->ResetSystem = DummyResetSystem;

    // Update Platform Policy for disabling BIOS_LOCK and SMM_BWP.
    gMeFwUpdProtocol->PlatformUnlock( gMeFwUpdProtocol, 0 );

    // Create Con Out Started Protocol event when Devices are ready. 
    RegisterProtocolCallback( &gBdsAllDriversConnectedProtocolGuid, \
                MeAutoUpdateConOutReadyEvent, NULL, &gEvtConOut, &gRgnConOut );
    return EFI_SUCCESS;                                    
}
/**
    DXE Entry Point for PeiRamBoot Driver.

    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS Return the EFI Stauts

**/
EFI_STATUS
EFIAPI
AutoMeudDxeEntry(
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS          Status;
    static EFI_EVENT    gEvtMeUpd;
    static VOID         *gRgnMeUpd;
    UINT32              Dummy = 0;                            
    UINTN               VariableSize = sizeof(UINT32);  

    InitAmiLib( ImageHandle, SystemTable );

    // Create PchPlatformPolicy Callback for unlocking BIOS_LOCK. 
    RegisterProtocolCallback( &gMeFwUpdLclProtocolGuid, \
                               MeFwUpdateProtocolEvent, NULL, &gEvtMeUpd, &gRgnMeUpd );
    MeFwUpdateProtocolEvent( gEvtMeUpd, NULL );                                    

	Status = pRS->GetVariable( L"MeAutoUpdateReq", \
                        &gMeFwUpdVariableGuid, NULL, &VariableSize, &Dummy );
    if( EFI_ERROR(Status) || (Dummy == ME_UPD_LCL_SIGNATURE) ) return EFI_SUCCESS;
                            
#if (AUTO_ME_UPDATE_POLICY == 0)
    {
        DXE_SERVICES    *DxeService = NULL;
        EFI_GUID        DxeServicesTableGuid = DXE_SERVICES_TABLE_GUID;
        EFI_HANDLE      FvHandle = NULL;
        // Process FV_DATA Firmware Volume for producing Firmware Volume2 Protocol for
        // FV_DATA.
  	    DxeService = (DXE_SERVICES*)GetEfiConfigurationTable( pST, &DxeServicesTableGuid );
	    if( DxeService != NULL )
        {
            DxeService->ProcessFirmwareVolume( \
                        (VOID*)FV_DATA_BASE, FV_DATA_SIZE, &FvHandle );
        }
    }
#endif
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
