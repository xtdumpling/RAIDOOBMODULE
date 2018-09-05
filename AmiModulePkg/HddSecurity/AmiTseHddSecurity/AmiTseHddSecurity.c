//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file AmiTseHddSecurity.c
    Provides the Hdd password Screen support in the setup.

**/

//---------------------------------------------------------------------------

#include "AmiDxeLib.h"
#include "Token.h"
#include "Protocol/HiiDatabase.h"
#include "Protocol/HiiString.h"
#include "Protocol/PciIo.h"
#include "Protocol/BlockIo.h"
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/AMIPostMgr.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DiskInfo.h>
#include "HddSecurityCommon.h"
#include "AmiTseHddSecurity.h"
#include "AMIVfr.h"
#if SETUP_SAME_SYS_HDD_PW
#include "Core/em/AMITSE/Inc/PwdLib.h"
#include "Core/em/AMITSE/Inc/variable.h"
#endif

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
  #include "Nvme/NvmeController.h"
  #include "Protocol/AmiNvmeController.h"
  #include "Nvme/NvmePassthru.h"
#endif

#if defined(SETUP_IDE_SECURITY_SUPPORT) && SETUP_IDE_SECURITY_SUPPORT
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
#include "minisetup.h"
#endif

EFI_GUID gIDESecGuid                 = IDE_SECURITY_CONFIG_GUID;
#endif

#if defined(AmiChipsetPkg_SUPPORT) && AmiChipsetPkg_SUPPORT
#include "SbSataProtocol.h"
SB_HDD_POLICY_PROTOCOL      *gSbHddPolicyPtr=NULL;
EFI_GUID                    gSbHddPolicyProtocolGuid    = SB_HDD_POLICY_PROTOCOL_GUID;
#endif

BOOLEAN CheckAllController=FALSE;
//---------------------------------------------------------------------------

EFI_GUID gHddPasswordPromptEnterGuid = HDD_PASSWORD_PROMPT_ENTER_GUID;
EFI_GUID gHddPasswordPromptExitGuid  = HDD_PASSWORD_PROMPT_EXIT_GUID;

static EFI_HII_STRING_PROTOCOL *HiiString = NULL;
static CHAR8                   *SupportedLanguages=NULL;
HDD_SECURITY_INTERNAL_DATA     *HddSecurityInternalData = NULL;
EFI_HANDLE                     gHddSecEndHandle = NULL;
EFI_HANDLE                     HddNotifyHandle;
static  EFI_HANDLE             *gHandleBuffer = NULL;
EFI_EVENT                      HddNotifyEvent;
VOID                           *HddNotifyRegistration;
BOOLEAN                        HddFreeze      = FALSE;
UINT16                         gHddSecurityCount = 0;
BOOLEAN                        gIsSbHddPolicyPresent= FALSE;
#if defined(SETUP_IDE_SECURITY_SUPPORT) && SETUP_IDE_SECURITY_SUPPORT
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
EFI_STRING_ID  ConfigPromptToken = 0;
extern UINTN gInvalidPasswordFailMsgBox;
#else
UINTN               gCurrIDESecPage;
#endif
#endif

/**
    Internal function that installs/uninstall protocol
    with a specified GUID and NULL interface.
    Such protocols can be used as event signaling mechanism.

    @param ProtocolGuid Pointer to the protocol GUID

    @retval VOID

**/
VOID
HddSecuritySignalProtocolEvent (
    IN  EFI_GUID    *ProtocolGuid
)
{

    EFI_HANDLE  Handle = NULL;

    pBS->InstallProtocolInterface (
                                &Handle,
                                ProtocolGuid,
                                EFI_NATIVE_INTERFACE,
                                NULL );

    pBS->UninstallProtocolInterface (
                                Handle,
                                ProtocolGuid,
                                NULL);
    return;
}


/**
    Register the Protocol call back event

    @param VOID

    @retval VOID

**/
BOOLEAN
RegisterHddNotification (
    VOID
)
{
    EFI_STATUS Status ;

    Status = gBS->CreateEvent(  EFI_EVENT_NOTIFY_SIGNAL,
                                TPL_CALLBACK,
                                HddNotificationFunction,
                                &HddNotifyRegistration,
                                &HddNotifyEvent);

    ASSERT_EFI_ERROR(Status);
    Status = gBS->RegisterProtocolNotify(   &gAmiHddSecurityEndProtocolGuid,
                                            HddNotifyEvent,
                                            &HddNotifyRegistration);
    ASSERT_EFI_ERROR(Status);

    //
    // get any of these events that have occurred in the past
    // When no con in devices are present this function is called later in BDS after InitConVars,
    // by that time if more than one controller present, Collect all controller and install gAmiHddPasswordVerifiedGuid
    CheckAllController = TRUE;
    gBS->SignalEvent( HddNotifyEvent );

    return FALSE;
}

/**
    HDD notification function gets called when HddSecurityEnd Protocol get installed.

    @param EFI_EVENT Event - Event to signal
    @param void HddRegContext - Event specific context (pointer to NotifyRegisteration

    @retval VOID

**/
VOID
EFIAPI
HddNotificationFunction (
    EFI_EVENT   Event,
    VOID        *HddRegContext
)
{
    EFI_STATUS                  Status;
    EFI_HANDLE                  *HandleBuffer = NULL;
#if defined(SETUP_IDE_SECURITY_SUPPORT) && SETUP_IDE_SECURITY_SUPPORT
    IDE_SECURITY_CONFIG         *pHddSecurityConfig = NULL;
    IDE_SECURITY_CONFIG          HddSecurityConfig;
    UINTN                       HddSecurityConfigSize = 0;
#endif
    HDD_SECURITY_INTERNAL_DATA  *DataPtr=NULL;
    UINTN                       NumHandles;
    UINTN                       Index=0;
    EFI_HANDLE                  Handle = NULL;
    EFI_GUID                    AmiTseHddNotifyGuid = AMI_TSE_HDD_NOTIFY_GUID;
    UINTN BufferSize = sizeof(UINTN);

    if ( HddRegContext == NULL ) {
        return;
    }

#if defined(SETUP_IDE_SECURITY_SUPPORT) && SETUP_IDE_SECURITY_SUPPORT
    // Initialize IdeSecConfig information if this variable is not set already.
    pHddSecurityConfig = VarGetNvramName( L"IDESecDev", &gIDESecGuid, NULL, &HddSecurityConfigSize );

    if ( !pHddSecurityConfig ) {

        pHddSecurityConfig = EfiLibAllocateZeroPool( sizeof(IDE_SECURITY_CONFIG));

        if ( pHddSecurityConfig == NULL ) {
            return;
        }
        MemSet( pHddSecurityConfig, sizeof(IDE_SECURITY_CONFIG), 0);
        VarSetNvramName( L"IDESecDev",
                         &gIDESecGuid,
                         EFI_VARIABLE_BOOTSERVICE_ACCESS,
                         pHddSecurityConfig,
                         sizeof(IDE_SECURITY_CONFIG));
    } else {
        MemFreePointer( (VOID **)&pHddSecurityConfig );
    }
#endif

    //Locate the handle
    if(CheckAllController) {
        // Locate all Handles that were installed previously.
        CheckAllController = FALSE;
        Status = gBS->LocateHandleBuffer(   ByProtocol,
                                            &gAmiHddSecurityEndProtocolGuid,
                                            NULL,
                                            &NumHandles,
                                            &HandleBuffer);
    } else {
        Status = gBS->LocateHandleBuffer(   ByRegisterNotify,
                                            NULL,
                                            *(VOID**)HddRegContext,
                                            &NumHandles,
                                            &HandleBuffer);
    }

    // If protocol not installed return
    if ( EFI_ERROR( Status ))
        return;

    //Locate the Security Protocols
    gHddSecurityCount = InitHddSecurityInternalDataPtr();

    for(Index=0; Index<gHddSecurityCount; Index++) {
        //
        //Initialize the DataPtr
        //
        DataPtr = (HDD_SECURITY_INTERNAL_DATA *) IDEPasswordGetDataPtr(Index);

        //
        // Search for locked Hard disc and not password verification done
        //
        if(DataPtr->Locked && !DataPtr->Validated){
            break;
        }
    }

    // Validate the password only if HDD is locked
    if( (gHddSecurityCount != 0 ) && (NULL != DataPtr ) &&
            (DataPtr->Locked ) && (!DataPtr->Validated ) ){

        TSEIDEPasswordCheck();
    }

#if defined(SETUP_IDE_SECURITY_SUPPORT) && SETUP_IDE_SECURITY_SUPPORT
    else {
          //
          // Update the IdeSecConfig information .
          //
          MemSet( &HddSecurityConfig, sizeof(HddSecurityConfig), 0 );
          HddSecurityConfig.Count = gHddSecurityCount;
          VarSetNvramName( L"IDESecDev",
                     &gIDESecGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS,
                     &HddSecurityConfig,
                     sizeof(HddSecurityConfig));
    }
#endif

    // ConnectController once HDD is unlocked when no CONOUT present.
    for (Index=0; Index<gHddSecurityCount; Index++) {
        //Initialize the DataPtr
        DataPtr = (HDD_SECURITY_INTERNAL_DATA *) IDEPasswordGetDataPtr(Index);

        Status = pBS->ConnectController(DataPtr->DeviceHandle,
                                        NULL,
                                        NULL,
                                        TRUE);
    }
    
    // Install gAmiHddPasswordVerifiedGuid on specific Controller Handle.
    for(Index = 0; Index < NumHandles; Index++){

        gHddSecEndHandle = HandleBuffer[Index];

        // Install the Unlocked Protocol to notify HDD has been unlocked
        // In case gAmiHddPasswordVerifiedGuid is already installed
        // on gHddSecEndHandle, Error shall be ignored to proceed.
        if ( gHddSecEndHandle != NULL ) {
            gBS->InstallProtocolInterface( &gHddSecEndHandle,
                                           &gAmiHddPasswordVerifiedGuid,
                                           EFI_NATIVE_INTERFACE,
                                           NULL);
        }
    }

    gBS->FreePool(HandleBuffer);
    
    Status = pBS->LocateHandle ( ByProtocol,
                                  &AmiTseHddNotifyGuid,
                                  NULL,
                                  &BufferSize,
                                  &Handle );
    if (Status == EFI_SUCCESS) {
        Status = pBS->ReinstallProtocolInterface(
                        Handle,
                        &AmiTseHddNotifyGuid,
                        NULL,
                        NULL);
    } else {
     
        // Notify setup modules that AMITSE initialized HDD data.
        Status = gBS->InstallProtocolInterface( &Handle, 
                                                &AmiTseHddNotifyGuid, 
                                                EFI_NATIVE_INTERFACE,
                                                NULL );
    }
     
    return;
}

/**
    Add the String to HII Database using HiiString Protocol

    @param HiiHandle
    @param String

    @retval VOID

**/
EFI_STRING_ID
PrivateHiiAddString (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  CHAR16 *        String
)
{
    EFI_STATUS      Status;
    CHAR8*          Languages = NULL;
    UINTN           LangSize = 0;
    CHAR8*          CurrentLanguage;
    BOOLEAN         LastLanguage = FALSE;
    EFI_STRING_ID   StringId = 0;

    if(HiiString == NULL) {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) &HiiString);
        if(EFI_ERROR(Status)) {
            return 0;
        }
    }

    if(SupportedLanguages == NULL) {
        Status = HiiString->GetLanguages(HiiString, HiiHandle, Languages, &LangSize);
        if(Status == EFI_BUFFER_TOO_SMALL) {
            Status = pBS->AllocatePool(EfiBootServicesData, LangSize, (VOID**)&Languages);
            if(EFI_ERROR(Status)) {
                //
                //not enough resources to allocate string
                //
                return 0;
            }
            Status = HiiString->GetLanguages(HiiString, HiiHandle, Languages, &LangSize);
        }
        SupportedLanguages=Languages;
    } else {
        Languages=SupportedLanguages;
    }

    while(!LastLanguage) {
        //
        //point CurrentLanguage to start of new language
        //
        CurrentLanguage = Languages;
        while(*Languages != ';' && *Languages != 0)
            Languages++;

        //
        //Last language in language list
        //
        if(*Languages == 0) {
            LastLanguage = TRUE;
            if(StringId == 0) {
                Status = HiiString->NewString(HiiString, HiiHandle, &StringId, CurrentLanguage, NULL, String, NULL);
            } else {
                Status = HiiString->SetString(HiiString, HiiHandle, StringId, CurrentLanguage, String, NULL);
            }
            if(EFI_ERROR(Status)) {
                return 0;
            }
        } else {
            //
            //put null-terminator
            //
            *Languages = 0;
            if(StringId == 0) {
                Status = HiiString->NewString(HiiString, HiiHandle, &StringId, CurrentLanguage, NULL, String, NULL);
            } else {
                Status = HiiString->SetString(HiiString, HiiHandle, StringId, CurrentLanguage, String, NULL);
            }
            *Languages = ';';       //restore original character
            Languages++;
            if(EFI_ERROR(Status)) {
                return 0;
            }
        }
    }
    return StringId;
}

/**
    Add the String to HiiDatabase

    @param HiiHandle
    @param String

    @retval VOID

**/
EFI_STRING_ID
PasswordHiiAddString (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  CHAR16          *String
)
{
    EFI_STRING_ID   StringId = 0;

    StringId=PrivateHiiAddString( HiiHandle, String );

    //
    // Error in Adding String. Try with Default function that AMITSE has.
    //
    if(StringId == 0) {
        StringId= HiiAddString( HiiHandle, String );
    }

    return StringId;
}

/**
    Function checks the Locally stored AmiHddSecurityProtocol handles are valid or not

    @param HandleBuffer
    @param Count

    @retval BOOLEAN

**/

BOOLEAN IsStoredPasswordDevHandleValid(
    EFI_HANDLE    *HandleBuffer,
    UINTN          Count
)
{
    UINTN    i, j;
    BOOLEAN  AllPwddDevHandleAreValid=TRUE;
    
    for( i=0; i<gHddSecurityCount; i++ ) {
        for( j=0; j<Count; j++ ) {
            if( gHandleBuffer[i] == HandleBuffer[j]) {
                break;
            }
        }
        if(j==Count){
            gHandleBuffer[i] = 0;
            AllPwddDevHandleAreValid=FALSE;
        }
    }
    
    if (gHddSecurityCount!=Count) {
        return FALSE;
    }
    
    return AllPwddDevHandleAreValid;
}

/**
    Locate the Security Protocols and return the information

    @param VOID

    @retval VOID

**/
UINT16
InitHddSecurityInternalDataPtr (
)
{
    EFI_STATUS                      Status;
    EFI_HANDLE                      *HandleBuffer = NULL;
    UINT16                          i, j, HDDCount = 0;
    UINTN                           Count;
    CHAR16                          *Name, *Temp1;
    CHAR16                          Temp[60];
    AMI_HDD_SECURITY_PROTOCOL       *HddSecurityProtocol = NULL;
    HDD_SECURITY_INTERNAL_DATA      *DataPtr             = NULL;
    EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
    EFI_DEVICE_PATH_PROTOCOL        *DevicePathNode;
    PCI_DEVICE_PATH                 *PciDevicePath;
    AMI_HDD_SECURITY_INTERFACE      *Security = NULL;
    UINT32                          HddPortNumber;
    CHAR16                          *Buff=L"P";
    EFI_DISK_INFO_PROTOCOL          *DiskInfoPtr=NULL;
    UINT32                          PortNumber;
    UINT32                          PMPortNumber;
    HDD_SECURITY_INTERNAL_DATA      *TempIDEPasswordSecurityData = NULL;
#if defined(AmiChipsetPkg_SUPPORT) && AmiChipsetPkg_SUPPORT
    UINT32                          IdeChannel;
    UINT32                          IdeDevice;
#endif

#if defined(SETUP_IDE_SECURITY_SUPPORT) && SETUP_IDE_SECURITY_SUPPORT
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
    if(ConfigPromptToken == 0)
    ConfigPromptToken = PasswordHiiAddString( gHiiHandle, L"HDD Security Configuration" );
#endif
#endif
    Status = gBS->LocateHandleBuffer(   ByProtocol,
                                        &gAmiHddSecurityProtocolGuid,
                                        NULL,
                                        &Count,
                                        &HandleBuffer);

    // If already data has been found return with that information
    if ( gHddSecurityCount != 0 && HddSecurityInternalData != NULL ) {
        // Check the locally stored AmiHddSecurityProtocol handles are valid or not
        if(IsStoredPasswordDevHandleValid(HandleBuffer,Count)) {
            return gHddSecurityCount;     // HddSecurityInternalData is valid
        }
    }

     //
     // New HDD device found. Need to validate the password for the new HDD
     // and skip the HDD that has been already validated.
     //
     TempIDEPasswordSecurityData = HddSecurityInternalData;
     
     HddSecurityInternalData = EfiLibAllocateZeroPool( Count * sizeof(HDD_SECURITY_INTERNAL_DATA));
     if(HddSecurityInternalData) {
         MemSet(HddSecurityInternalData,sizeof(HDD_SECURITY_INTERNAL_DATA) * Count,0 );
     }

     DataPtr = HddSecurityInternalData;
    if(DataPtr == NULL) {
        return 0;
    }

    for ( i = 0; i < Count; i++ ) {

        // Check if already Validate or not. If already validate don't verify the password again.
        if ( gHandleBuffer != NULL && gHddSecurityCount != 0 ) {
            j = gHddSecurityCount;

            do {
                if ( HandleBuffer[i] == gHandleBuffer[j-1] ) {
                    break;
                }
                j--;
            } while ( j != 0 );

            if ( j != 0 ) {
                MemCopy((HddSecurityInternalData+i),(TempIDEPasswordSecurityData+(j-1)),sizeof(HDD_SECURITY_INTERNAL_DATA));
                HDDCount++;
                continue;
            }
        }

        DataPtr = HddSecurityInternalData+i;

        //
        // Get the PasswordSecurity Protocol
        //
        Status = gBS->OpenProtocol( HandleBuffer[i],
                                    &gAmiHddSecurityProtocolGuid,
                                    (VOID**) &HddSecurityProtocol,
                                    NULL,
                                    HandleBuffer[i],
                                    EFI_OPEN_PROTOCOL_GET_PROTOCOL);

        if ( EFI_ERROR( Status ))
            continue;
        
        Security = (AMI_HDD_SECURITY_INTERFACE *)HddSecurityProtocol;
        Security->PostHddCount = (VOID*)&gHddSecurityCount;
        Security->PostHddData = (VOID**)&HddSecurityInternalData;
        
        if( Security->BusInterfaceType == AmiStorageInterfaceAhci ) {

            // Handle the DiskInfo Protocol
            Status = gBS->OpenProtocol( HandleBuffer[i],
                                        &gEfiDiskInfoProtocolGuid,
                                        (VOID**) &DiskInfoPtr,
                                        NULL,
                                        HandleBuffer[i],
                                        EFI_OPEN_PROTOCOL_GET_PROTOCOL);
            if ( EFI_ERROR( Status )) {
                continue;
            }
        }

        // Locate the device path Protocol
        Status = gBS->OpenProtocol( HandleBuffer[i],
                                    &gEfiDevicePathProtocolGuid,
                                    (VOID**)&DevicePath,
                                    NULL,
                                    HandleBuffer[i],
                                    EFI_OPEN_PROTOCOL_GET_PROTOCOL);

        if ( EFI_ERROR( Status )) {
            continue;
        }

        DevicePathNode = DevicePath;
        //
        // Traverse the Device Path structure till we reach HARDWARE_DEVICE_PATH
        //
        while (!isEndNode (DevicePathNode)) {

            if ((DevicePathNode->Type == HARDWARE_DEVICE_PATH) &&
                (DevicePathNode->SubType == HW_PCI_DP)){

                PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
                break;
            }

            DevicePathNode = NEXT_NODE (DevicePathNode);
        }

        if (PciDevicePath == NULL) continue;

        Security = (AMI_HDD_SECURITY_INTERFACE *)HddSecurityProtocol;

        if( Security->BusInterfaceType == AmiStorageInterfaceAhci ) {
            //
            //  Controller is in AHCI Mode, Call WhichIde function to find out Port Number
            //
            DiskInfoPtr->WhichIde(DiskInfoPtr,&PortNumber,&PMPortNumber);
            //
            //  Assign the PortNumber to HddPortNumber.This Port Number is displayed in Setup.
            //
            HddPortNumber=PortNumber;

            gIsSbHddPolicyPresent=TRUE;

        } else if( Security->BusInterfaceType == AmiStorageInterfaceIde ) {


#if defined(AmiChipsetPkg_SUPPORT) && AmiChipsetPkg_SUPPORT

            if(gSbHddPolicyPtr==NULL){
                Status=gBS->LocateProtocol(&gSbHddPolicyProtocolGuid,
                                            NULL, \
                                            (VOID**)&gSbHddPolicyPtr);
            }

            if(gSbHddPolicyPtr!=NULL){

                //
                //  Find out the Primary/Secondary,Master/Slave Info from WhichIde function
                //
                DiskInfoPtr->WhichIde(DiskInfoPtr,&IdeChannel,&IdeDevice);
                //
                //  Get the Port Number to which the HDD is connected
                //
                gSbHddPolicyPtr->GeneratePortNumber(PciDevicePath->Device,PciDevicePath->Function,
                                                    IdeChannel,IdeDevice,&PortNumber);

                HddPortNumber=PortNumber;

                gIsSbHddPolicyPresent=TRUE;

            } else{

                //
                // SB HDD Policy Protocol is not Present.
                //
                gIsSbHddPolicyPresent=FALSE;
            }
#endif
        }

        if ( CheckSecurityStatus( HddSecurityProtocol, &(DataPtr->Locked), SecurityLockedMask )) {
            DataPtr->HddSecurityProtocol = HddSecurityProtocol;

            if ( HddPasswordGetDeviceName( HandleBuffer[i], &Name )) {

                DataPtr->NameToken = PasswordHiiAddString( gHiiHandle, Name );

                if ( Security->BusInterfaceType == AmiStorageInterfaceNvme ) {
                    //  Display the the Port Number in Setup
                    SPrint( Temp, 60, L"%s",Name );
                    DataPtr->PromptToken = PasswordHiiAddString( gHiiHandle, Temp );
                } else {
                    if(gIsSbHddPolicyPresent) {

                        //  Display the the Port Number in Setup
                        SPrint( Temp, 60, L"%s%d:%s", Buff, HddPortNumber, Name );
                        DataPtr->PromptToken = PasswordHiiAddString( gHiiHandle, Temp );
                    } else{

                        //  If SB HDD Policy Protocol is not Installed Use STR_IDE_SECURITY_PROMPT
                        //  token to display the String Information.
                        Temp1 = HiiGetString( gHiiHandle, STRING_TOKEN( STR_IDE_SECURITY_PROMPT ));
                        SPrint( Temp, 60, L"%s%d:%s", Temp1,HDDCount,Name);
                        DataPtr->PromptToken = PasswordHiiAddString( gHiiHandle, Temp );
                        MemFreePointer( (VOID **)&Temp1);
                    }
                }
            } 
            DataPtr->Validated = FALSE;
            DataPtr->PostHiiHandle = gHiiHandle;
            DataPtr->DeviceHandle = HandleBuffer[i];
            HDDCount++;
         }// end if

    }// end of for

    if ( gHandleBuffer != NULL ) {
        MemFreePointer( (VOID **)&gHandleBuffer );
    }

    gHandleBuffer = EfiLibAllocateZeroPool( sizeof(EFI_HANDLE) * Count );
    MemCopy( gHandleBuffer, HandleBuffer, sizeof(EFI_HANDLE) * Count );

    MemFreePointer((VOID**)&HandleBuffer );
    MemFreePointer((VOID**)&TempIDEPasswordSecurityData);

    //
    //if no HDD is supported
    //
    if ( HDDCount == 0 ) {
        MemFreePointer((VOID**)&HddSecurityInternalData );
        HddSecurityInternalData=NULL;
    }

    return HDDCount;
}

/**
    Return HDD Locked Information

    @param UINTN       Index

    @retval VOID

**/
BOOLEAN
IDEPasswordGetLocked (
    UINTN   Index
)
{
    HDD_SECURITY_INTERNAL_DATA   *DataPtr = (HDD_SECURITY_INTERNAL_DATA*)IDEPasswordGetDataPtr( Index );

    if(DataPtr == NULL) {
        return 0;
    }
    return DataPtr->Locked;
}

/**
    return the Security Status Information

    @param VOID

    @retval VOID

**/
BOOLEAN
CheckSecurityStatus (
    AMI_HDD_SECURITY_PROTOCOL   *HddSecurityProtocol,
    BOOLEAN                     *ReqStatus,
    UINT16                      Mask
)
{
    UINT16     SecurityStatus = 0;
    EFI_STATUS Status;

    //
    //get the security status of the device
    //
    Status = HddSecurityProtocol->ReturnSecurityStatus( HddSecurityProtocol, &SecurityStatus );

    if ( EFI_ERROR( Status ))
        return FALSE;

    *ReqStatus = (BOOLEAN)((SecurityStatus & Mask) ? TRUE : FALSE );
    return TRUE;
}

/**
    return the Device path Length

    @param VOID

    @retval VOID

**/
UINTN
HddPasswordDPLength (
    EFI_DEVICE_PATH_PROTOCOL    *pDp
)
{
    UINTN Size = 0;

    for (; !(IsDevicePathEnd( pDp )); pDp = NextDevicePathNode( pDp ))
        Size += DevicePathNodeLength( pDp );

    //
    // add size of END_DEVICE_PATH node
    //
    return Size + END_DEVICE_PATH_LENGTH;
}

/**
    Returns pointer on very last DP node before END_OF_DEVICE_PATH node

    @param VOID

    @retval VOID

**/
VOID*
HddPasswordDPGetLastNode (
    EFI_DEVICE_PATH_PROTOCOL    *pDp
)
{
    EFI_DEVICE_PATH_PROTOCOL *dp = NULL;

    for (; !IsDevicePathEnd( pDp ); pDp = NextDevicePathNode( pDp ))
        dp = pDp;

    return dp;
}

/**
    Copy the Device path to another Memory buffer

    @param EFI_DEVICE_PATH_PROTOCOL *pDp

    @retval VOID

**/
VOID*
HddPasswordDPCopy (
    EFI_DEVICE_PATH_PROTOCOL    *pDp
)
{
    UINTN l  = HddPasswordDPLength( pDp );
    UINT8 *p = EfiLibAllocateZeroPool( l );

    MemCopy( p, pDp, l );
    return p;
}

/**
    Return the Drive String Name

    @param EFI_HANDLE Controller - the handle of the drive
    @param CHAR16 **wsName - returned pointer to the drive string

    @retval BOOLEAN TRUE - drive string has been found and is in wsName
        - FALSE - drive string has not been found

    @note  it is the caller's responsibility to deallocate the space used for
      wsName

**/
BOOLEAN
HddPasswordGetDeviceName (
    EFI_HANDLE  Controller,
    CHAR16      **wsName
)
{
    EFI_STATUS                  Status;
    AMI_HDD_SECURITY_INTERFACE  *Security = NULL;

    CHAR16 *DeviceName;
    BOOLEAN ReturnStatus = FALSE;
    UINT8 Index = 0;

    // Get the AMI_HDD_SECURITY_INTERFACE (actually getting the AMI_HDD_SECURITY_PROTOCOL, but
    //  the AMI_HDD_SECURITY_INTERFACE is an extended version with more information)
    Status = gBS->HandleProtocol(Controller, &gAmiHddSecurityProtocolGuid, (VOID**)&Security);
    if ( !EFI_ERROR(Status) ) {
        // Check the SATA controller operating mode, and based on the mode, get the UnicodeString
        //  name of the device
        if ( Security->BusInterfaceType == AmiStorageInterfaceAhci ) {
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
            DeviceName = ((SATA_DEVICE_INTERFACE*)Security->BusInterface)->UDeviceName->UnicodeString;
#endif
        } else if ( Security->BusInterfaceType == AmiStorageInterfaceIde ) {
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
            DeviceName = ((AMI_IDE_BUS_PROTOCOL*)Security->BusInterface)->IdeDevice.UDeviceName->UnicodeString;
#endif
        } else if ( Security->BusInterfaceType == AmiStorageInterfaceNvme ) {
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
         DeviceName = \
           ((NVM_EXPRESS_PASS_THRU_INSTANCE*)Security->BusInterface)->NvmeControllerProtocol->UDeviceName->UnicodeString;
#endif
        }

        // Allocate space to copy the UNICODE device name string
        *wsName = EfiLibAllocateZeroPool( sizeof(CHAR16)*(EfiStrLen(DeviceName)+1));

        if ( *wsName!=NULL ) {
            // To remove the spaces from the HardDisk Model Number
            for (Index = HDD_MODEL_NUMBER_LENGTH; Index >= 0; Index-- ) {

                if (DeviceName[Index] != SPACE_ASCII_VALUE) { 
                    break;
                }
                DeviceName[Index] = 0;
            }
            EfiStrCpy( *wsName, DeviceName);
            ReturnStatus = TRUE;
        }
    }

    // Return true to signify that a device name was discovered
    return ReturnStatus;
}

/**
    Get the password and Validate the HDD password

    @param UINT16 PromptToken,
    @param VOID *DataPtr

    @retval VOID

**/
VOID
AMI_CheckIDEPassword (
    UINT16  PromptToken,
    VOID    *DataPtr
)
{
    UINTN       CurrXPos, CurrYPos, i;
    CHAR16      *PasswordEntered;
    EFI_STATUS  Status   =   EFI_ACCESS_DENIED;

    #if SETUP_ASK_MASTER_PASSWORD
    UINT32     IdePasswordFlags = 0;
    #endif

    UINTN      BoxLength  = IDE_PASSWORD_LENGTH;
    CHAR16     *DescToken = NULL;
    CHAR16     *HelpMsgToken = NULL;

    UINT16     SecurityStatus=0;
    AMI_HDD_SECURITY_PROTOCOL *HddSecurityProtocol = NULL;
    CHAR16      *UnlckHddCBToken = NULL;
    UINTN       CB_BoxLength=0;
    UINTN       Help_MsgLength=0;

    CheckForKeyHook((EFI_EVENT)NULL, NULL );
    gST->ConIn->Reset( gST->ConIn, FALSE );

    
    DescToken = HiiGetString( gHiiHandle, STRING_TOKEN( STR_IDE_ENTER_USER ));

    if ( DescToken ) {
        if ( (TestPrintLength( DescToken ) / NG_SIZE) > BoxLength ) {
            BoxLength = TestPrintLength( DescToken ) / NG_SIZE;
        }
    }
    MemFreePointer((VOID**) &DescToken );

    UnlckHddCBToken = HiiGetString( gHiiHandle, STRING_TOKEN(STR_IDE_UNLCK_COLD));
    if ( UnlckHddCBToken ) {
        if ( (TestPrintLength( UnlckHddCBToken ) / NG_SIZE) > CB_BoxLength ) {
            CB_BoxLength = TestPrintLength( UnlckHddCBToken ) / NG_SIZE;
        }
    }
    MemFreePointer((VOID**) &UnlckHddCBToken );
    
    HelpMsgToken = HiiGetString( gHiiHandle, STRING_TOKEN( STR_IDE_HELP_MESSAGE ));
    if ( HelpMsgToken ) {
           if ( (TestPrintLength( HelpMsgToken ) / NG_SIZE) > Help_MsgLength ) {
               Help_MsgLength = TestPrintLength( HelpMsgToken ) / NG_SIZE;
           }
    }
    MemFreePointer((VOID**) &HelpMsgToken );
       
    HddSecuritySignalProtocolEvent(&gHddPasswordPromptEnterGuid);
    //
    //Draw password window
    //
#if ALL_HDD_SAME_PW
    PromptToken = STRING_TOKEN( STR_IDE_SECURITY_PROMPT );
#endif
    _DrawPasswordWindow( PromptToken, Help_MsgLength, &CurrXPos, &CurrYPos );

    PasswordEntered = EfiLibAllocateZeroPool((IDE_PASSWORD_LENGTH + 1) * sizeof(CHAR16));
    HddSecurityProtocol=((HDD_SECURITY_INTERNAL_DATA* )DataPtr)->HddSecurityProtocol;
    //
    //Give four chances to enter user password
    //
    for ( i = 0; i < USER_PASSWORD_RETRY_ATTEMPTS; i++ ) {
        Status = HddSecurityProtocol->ReturnSecurityStatus(HddSecurityProtocol, &SecurityStatus );
        if(Status == EFI_SUCCESS && (SecurityStatus>>4)& 0x1){
            _DrawPasswordWindow( PromptToken, CB_BoxLength, &CurrXPos, &CurrYPos );
            _ReportInBox(CB_BoxLength,STRING_TOKEN(STR_IDE_UNLCK_COLD),CurrXPos,CurrYPos,TRUE);
            HddSecuritySignalProtocolEvent(&gHddPasswordPromptExitGuid);
            if (gST->ConOut != NULL) {
                //Disable cursor, set desired attributes and clear screen
                gST->ConOut->EnableCursor( gST->ConOut, FALSE );
                gST->ConOut->SetAttribute( gST->ConOut, (EFI_BACKGROUND_BLACK | EFI_WHITE));
                gST->ConOut->ClearScreen( gST->ConOut);
            }
            return ;
        }
        _ReportInBox( BoxLength, STRING_TOKEN(STR_IDE_ENTER_USER), CurrXPos, CurrYPos - 1 , FALSE );
#if MASTER_PASSWORD_ENABLE && SETUP_ASK_MASTER_PASSWORD
        _ReportInBox( Help_MsgLength, STRING_TOKEN(STR_IDE_HELP_MESSAGE), CurrXPos, CurrYPos + 1 , FALSE );
#endif
        
        Status = _GetPassword( PasswordEntered,
                             IDE_PASSWORD_LENGTH,
                             CurrXPos,
                             CurrYPos,
                             NULL );
        if ( EFI_SUCCESS != Status ) {
            break;
        } // end if

        //
        // Validate the Password
        //
        Status = IDEPasswordAuthenticate( PasswordEntered, DataPtr, TRUE );

        if ( EFI_SUCCESS == Status ) {
            break;
        } else if ((i + 1) != USER_PASSWORD_RETRY_ATTEMPTS ) {
            _ReportInBox( IDE_PASSWORD_LENGTH, STRING_TOKEN(STR_ERROR_PSWD), CurrXPos, CurrYPos, TRUE );
        }
    }// end of for

    #if SETUP_ASK_MASTER_PASSWORD

    if ( EFI_SUCCESS != Status ) {
        
        if(EFI_ABORTED != Status && MAXIMUM_HDD_UNLOCK_ATTEMPTS != USER_PASSWORD_RETRY_ATTEMPTS) {
            _ReportInBox( IDE_PASSWORD_LENGTH, STRING_TOKEN(STR_ERROR_PSWD), CurrXPos, CurrYPos, TRUE );
        }
        //
        // Checking if the master password is installed
        //
        Status=((HDD_SECURITY_INTERNAL_DATA*)DataPtr)->HddSecurityProtocol->ReturnIdePasswordFlags(
            ((HDD_SECURITY_INTERNAL_DATA*)DataPtr)->HddSecurityProtocol,
            &IdePasswordFlags );

        if((Status == EFI_SUCCESS)&&((IdePasswordFlags>>16)&1)) {
            if ( i < MAXIMUM_HDD_UNLOCK_ATTEMPTS ) {
                BoxLength = IDE_PASSWORD_LENGTH;
                DescToken = HiiGetString( gHiiHandle, STRING_TOKEN( STR_IDE_ENTER_MASTER ));

                if ( DescToken ) {
                    if (( TestPrintLength( DescToken ) / NG_SIZE) > BoxLength ) {
                        BoxLength = TestPrintLength( DescToken ) / NG_SIZE;
                    }
                }
                MemFreePointer((VOID**) &DescToken );

                ClearScreen( EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY );

                //
                //Draw password window
                //
                _DrawPasswordWindow( PromptToken, BoxLength, &CurrXPos, &CurrYPos );
            }

            //
            //Give remaining chances to enter Master password
            //
            for (; i < MAXIMUM_HDD_UNLOCK_ATTEMPTS; i++ ) {
                Status = HddSecurityProtocol->ReturnSecurityStatus(HddSecurityProtocol, &SecurityStatus );
                if(Status == EFI_SUCCESS && (SecurityStatus>>4)& 0x1){
                    _DrawPasswordWindow( PromptToken, CB_BoxLength, &CurrXPos, &CurrYPos );
                    _ReportInBox(CB_BoxLength,STRING_TOKEN(STR_IDE_UNLCK_COLD),CurrXPos,CurrYPos,TRUE);
                    HddSecuritySignalProtocolEvent(&gHddPasswordPromptExitGuid);
                    if (gST->ConOut != NULL) {
                        //Disable cursor, set desired attributes and clear screen
                        gST->ConOut->EnableCursor( gST->ConOut, FALSE );
                        gST->ConOut->SetAttribute( gST->ConOut, (EFI_BACKGROUND_BLACK | EFI_WHITE));
                        gST->ConOut->ClearScreen( gST->ConOut);
                    }
                    return ;
                }
                _ReportInBox( BoxLength, STRING_TOKEN(
                              STR_IDE_ENTER_MASTER ), CurrXPos, CurrYPos - 1,
                          FALSE );

                Status = _GetPassword( PasswordEntered, 
                                   IDE_PASSWORD_LENGTH, 
                                   CurrXPos,
                                   CurrYPos, 
                                   NULL );
                if ( EFI_SUCCESS != Status ) {
                    break;
                }

                //
                // validate the Master password
                //
                Status = IDEPasswordAuthenticate( PasswordEntered, DataPtr, FALSE );

                if ( EFI_SUCCESS == Status ) {
                    break;
                } else {
                    if ( (i + 1) != MAXIMUM_HDD_UNLOCK_ATTEMPTS ) {
                        _ReportInBox( IDE_PASSWORD_LENGTH,
                                  STRING_TOKEN(
                                          STR_ERROR_PSWD ), CurrXPos, CurrYPos,
                                      TRUE );
                    }
                }
            }// end of for
        }// end if
    }// end if
    #endif

    MemSet( PasswordEntered, (IDE_PASSWORD_LENGTH + 1) * sizeof(CHAR16), 0);

    MemFreePointer((VOID**)&PasswordEntered );

    if ( EFI_SUCCESS != Status || i>=MAXIMUM_HDD_UNLOCK_ATTEMPTS) {
        //Report Invalid password
        _ReportInBox( IDE_PASSWORD_LENGTH, STRING_TOKEN(
                          STR_IDE_ERROR_PSWD ), CurrXPos, CurrYPos, TRUE );
        // Unlock failed.
        EfiLibReportStatusCode( EFI_ERROR_CODE | EFI_ERROR_MAJOR,
                                DXE_INVALID_IDE_PASSWORD,
                                0,
                                NULL,
                                NULL );
    }
    HddSecuritySignalProtocolEvent(&gHddPasswordPromptExitGuid);
    if (gST->ConOut != NULL) {
        //Disable cursor, set desired attributes and clear screen
        gST->ConOut->EnableCursor( gST->ConOut, FALSE );
        gST->ConOut->SetAttribute( gST->ConOut, (EFI_BACKGROUND_BLACK | EFI_WHITE));
        gST->ConOut->ClearScreen( gST->ConOut);
    }

    return;
}

/**
    Send Freeze command all the HDD

    @param VOID

    @retval VOID

**/
VOID
IDEPasswordFreezeDevices (
)
{
    EFI_STATUS            Status;
    EFI_HANDLE            *HandleBuffer;
    UINT16                i;
    UINTN                 Count;
    AMI_HDD_SECURITY_PROTOCOL *HddSecurityProtocol = NULL;

    // Return if Hdd FreezeLock command is already sent
    if( HddFreeze ) {
        return;
    }

    Status = gBS->LocateHandleBuffer(
                                ByProtocol,
                                &gAmiHddSecurityProtocolGuid,
                                NULL,
                                &Count,
                                &HandleBuffer
                                );

    if ( EFI_ERROR( Status )) {
        return;
    }

    for ( i = 0; i < Count; i++ ) {
        //
        // get password security protocol
        //
        Status = gBS->OpenProtocol(
                                HandleBuffer[i],
                                &gAmiHddSecurityProtocolGuid,
                                (VOID**) &HddSecurityProtocol,
                                NULL,
                                HandleBuffer[i],
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                                );

        if ( EFI_ERROR( Status )) {
            continue;
        }

        //
        //Send Freeze lock command
        //
        Status= HddSecurityProtocol->SecurityFreezeLock( HddSecurityProtocol );
        if(Status == EFI_SUCCESS) {
            HddFreeze = TRUE;
        }
        
    }// end of for
    MemFreePointer((VOID**)&HandleBuffer );

    return;
}

/**
    Updates the HDD password for the current HDD alone.

    @param UINT32 Index,
    @param CHAR16 *Password,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
BOOLEAN
IDEPasswordUpdateHdd (
    UINT32      Index,
    CHAR16      *Password,
    BOOLEAN     bCheckUser
)
{
    AMI_HDD_SECURITY_PROTOCOL *HddSecurityProtocol = NULL;
    UINT16                Control              = bCheckUser ? 0 : 1;
    UINT8                 RevisionCode         = 0;
    BOOLEAN               Locked, Enabled;
    EFI_STATUS            Status = EFI_UNSUPPORTED;
    UINT8                 Buffer[IDE_PASSWORD_LENGTH + 1];
    UINT8                 Selection;
    CHAR16                *MsgToken = NULL;
    #if !SETUP_SAME_SYS_HDD_PW
    UINTN                 ii;
    #endif
    HDD_SECURITY_INTERNAL_DATA     *DataPtr;

//
// While password is set via hook in TSE to perform some OEM feature
// and SETUP_PASSWORD_NON_CASE_SENSITIVE is set, even then the
// password will be updated as if it is case sensitive but Authenticates
// as non case sensitive so in order to avoid such situation making
// IDEPasswordUpdateHdd() symmetric with IDEPasswordAuthenticateHdd()
// to update case sensitivity {EIP99649}
//
#if defined(HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE) && HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE
    UpdatePasswordToNonCaseSensitive(Password, Wcslen(Password));
#endif

    DataPtr = (HDD_SECURITY_INTERNAL_DATA*)IDEPasswordGetDataPtr( Index );
    
    if ( DataPtr == NULL ) {
        MsgToken = HiiGetString(gHiiHandle,
                                STRING_TOKEN(STR_IDE_SECURITY_UNSUPPORTED)
                                );
        ShowPostMsgBox(
                NULL,
                MsgToken,
                MSGBOX_TYPE_OK,
                &Selection
               ); // ShowPostMsgBox
        MemFreePointer((VOID**)&MsgToken);
        return FALSE;
    }
    HddSecurityProtocol = DataPtr->HddSecurityProtocol;

    //
    //get the status of the device
    //
    if ( !(
                CheckSecurityStatus(
                                     HddSecurityProtocol, &Locked,
                                     SecurityLockedMask )
             && CheckSecurityStatus( HddSecurityProtocol, &Enabled,
                                     SecurityEnabledMask ))) {
        return FALSE;
    }

    if ( !Locked ) {
        if ( Password[0] == 0 ) {
            //
            //empty string is entered -> disable password
            //
            Status = HddSecurityProtocol->SecurityDisablePassword(
                HddSecurityProtocol,
                Control,
                HddSecurityInternalData[Index].PWD );
        } else {
            //
            //set new password
            //
            MemSet( &Buffer, IDE_PASSWORD_LENGTH + 1, 0 );
            #if !SETUP_SAME_SYS_HDD_PW

            for ( ii = 0; ii < IDE_PASSWORD_LENGTH + 1; ii++ ) {
                Buffer[ii] = (UINT8)Password[ii];

                if ( Password[ii] == L'\0' ) {
                    break;
                }
            }// end of for
            #else
            MemCopy( Buffer, Password, IDE_PASSWORD_LENGTH + 1 );
            #endif

            Status = HddSecurityProtocol->SecuritySetPassword(
                HddSecurityProtocol,
                Control,
                Buffer,
                RevisionCode );
        }
    }// end if(!Locked)

    if ( EFI_ERROR( Status )) {
        return FALSE;
    }

    return TRUE;
}

/**
    Updates the HDD password for all the HDDs present.

    @param UINT32 Index,
    @param CHAR16 *Password,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
BOOLEAN
IDEPasswordUpdateAllHdd (
    UINT32      Index,
    CHAR16      *Password,
    BOOLEAN     bCheckUser
)
{
    UINTN       i;
    BOOLEAN     Status = FALSE;
        for ( i = 0; i < gHddSecurityCount; i++ ) {
            Status = IDEPasswordUpdateHdd( (UINT32)i, Password, bCheckUser);
        }
    return Status;
}

/**
    Hook function to update the password for the HDDs based
    on the token ALL_HDD_SAME_PW.

    @param  UINT32 Index,
    @param  CHAR16 *Password,
    @param  BOOLEAN bCheckUser

    @retval VOID

**/
BOOLEAN
IDEPasswordUpdate (
    UINT32      Index,
    CHAR16      *Password,
    BOOLEAN     bCheckUser
)
{
    #if ALL_HDD_SAME_PW
     return IDEPasswordUpdateAllHdd( Index, Password, bCheckUser);
    #else
     return IDEPasswordUpdateHdd( Index, Password, bCheckUser);
    #endif

}
/**
    Unlock the HDD

    @param VOID

    @retval VOID

**/
VOID
UnlockHDD (
    VOID
)
{
    #if SETUP_SAME_SYS_HDD_PW
    UINTN             Size=0, i;
    AMITSESETUP       *TSESetup;
    HDD_SECURITY_INTERNAL_DATA *DataPtr;
    EFI_STATUS        Status;
    EFI_GUID AmiTseSetupGuid = AMITSESETUP_GUID;

    //
    //Do not proceed if ADMIN password is not set
    //
    if ( !(AMI_PASSWORD_USER & PasswordCheckInstalled()) ) {
        return;
    }

    //
    //Get password from NVRAM
    //
    Size = 0;
    TSESetup = VarGetNvramName (L"AMITSESetup", &AmiTseSetupGuid, NULL, &Size);

    if ( Size ) {
        //
        //For all drives
        //
        DataPtr = HddSecurityInternalData;

        for ( i = 0; i < gHddSecurityCount; i++ ) {
            if ( DataPtr->Locked ) {
                //
                //ask for the password if locked
                //
                Status = IDEPasswordAuthenticateHdd( TSESetup->UserPassword,
                                                  DataPtr,
                                                  TRUE );

                if ( EFI_SUCCESS != Status ) {
                    //
                    // Unlock failed.
                    //
                    EfiLibReportStatusCode( EFI_ERROR_CODE | EFI_ERROR_MAJOR,
                                            DXE_INVALID_IDE_PASSWORD,
                                            0,
                                            NULL,
                                            NULL );
                }
            }
            DataPtr++;
        } // end of for
    } // end if

    MemFreePointer((VOID**) &TSESetup );
    return;
    #endif //#if SETUP_SAME_SYS_HDD_PW
}

/**
    Set the HDD password

    @param VOID

    @retval VOID

**/
VOID
SetHDDPassword (
    VOID
)
{
    #if SETUP_SAME_SYS_HDD_PW
    UINTN       Size=0, i;
    AMITSESETUP *TSESetup;
    BOOLEAN     Status;
    EFI_GUID AmiTseSetupGuid = AMITSESETUP_GUID;

    //
    //Get password from NVRAM
    //
    Size = 0;
    TSESetup = VarGetNvramName (L"AMITSESetup", &AmiTseSetupGuid, NULL, &Size);

    if ( Size ) {
        //
        //For all drives
        //
        for ( i = 0; i < gHddSecurityCount; i++ ) {
            Status = IDEPasswordUpdateHdd( (UINT32)i, TSESetup->UserPassword, TRUE);
            ASSERT_EFI_ERROR (Status);
        }
    }

    MemFreePointer((VOID**) &TSESetup );
    #endif //#if SETUP_SAME_SYS_HDD_PW
}

/**
    Validate the HDD password

    @param VOID

    @retval VOID

**/
VOID
IDEPasswordCheck (
)
{
#if ALL_HDD_SAME_PW
    EFI_STATUS  Status = EFI_SUCCESS;
#endif
#if !SETUP_SAME_SYS_HDD_PW
    HDD_SECURITY_INTERNAL_DATA   *DataPtr;
    UINT16              i;
#if defined(SETUP_IDE_SECURITY_SUPPORT) && SETUP_IDE_SECURITY_SUPPORT
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
    UINTN           HddIndex = 0;
#endif
#endif
#endif
#if defined(SETUP_IDE_SECURITY_SUPPORT) && SETUP_IDE_SECURITY_SUPPORT
    IDE_SECURITY_CONFIG HddSecurityConfig;
#endif

    // build IDESecurity data
    gHddSecurityCount = InitHddSecurityInternalDataPtr( );

    if ( HddSecurityInternalData == NULL || gHddSecurityCount == 0
         || HddFreeze ) {
        return;
    }

#if SETUP_SAME_SYS_HDD_PW
        UnlockHDD();
#else
    DataPtr   = HddSecurityInternalData;
#if !ALL_HDD_SAME_PW
    for ( i = 0; i < gHddSecurityCount; i++ ) {
        if ( DataPtr->Locked && (!DataPtr->Validated)) {
            //ask for the password if locked
            DataPtr->Validated = TRUE;
            AMI_CheckIDEPassword( DataPtr->PromptToken, (VOID*)DataPtr );
        }
        DataPtr++;
    }// end of for
#else

    for ( i = 0; i < gHddSecurityCount; i++ ) {
        // Check if Password is validated for a Device, if yes, Use the same Password buffer
        if( DataPtr->Validated && !DataPtr->Locked ) {

            Status = IDEPasswordUpdateAllHddWithValidatedPsw (
                              DataPtr->PWD,
                              DataPtr,
                              DataPtr->LoggedInAsMaster? 0:1 );
            if(!EFI_ERROR(Status)) {
                break;
            }
        }
        DataPtr++;
    }// end of for
    // Ask Password from User
    if( i == gHddSecurityCount ) {
        DataPtr = HddSecurityInternalData;
        if ( DataPtr->Locked && (!DataPtr->Validated)) {
            //ask for the password if locked
            DataPtr->Validated = TRUE;
            AMI_CheckIDEPassword( DataPtr->PromptToken, (VOID*)DataPtr );
        }
    }
#endif
#endif

#if defined(SETUP_IDE_SECURITY_SUPPORT) && SETUP_IDE_SECURITY_SUPPORT
    MemSet( &HddSecurityConfig, sizeof(HddSecurityConfig), 0 );
    HddSecurityConfig.Count = gHddSecurityCount;
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
    for( HddIndex = 0 ; HddIndex < gHddSecurityCount ; HddIndex++ )
    {
        IDEUpdateConfig( &HddSecurityConfig, HddIndex );
    }
#endif
    VarSetNvramName( L"IDESecDev",
                     &gIDESecGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS,
                     &HddSecurityConfig,
                     sizeof(HddSecurityConfig));
#endif
    return;
}


#if defined(SETUP_IDE_SECURITY_SUPPORT) && SETUP_IDE_SECURITY_SUPPORT
/**
    Initializes the structure IDE_SECURITY_CONFIG for the current
    HDD if the data pointer to the structure HDD_SECURITY_INTERNAL_DATA is
    initialized already.

    @param IDE_SECURITY_CONFIG *ptrHddSecurityConfig
    @param  UINTN value

    @retval VOID

**/
VOID
UpdateSetupDataByHddIndex(
    VOID    *ptrHddSecurityConfig,
    UINTN   value
)
{
    HDD_SECURITY_INTERNAL_DATA    *DataPtr             = NULL;
    AMI_HDD_SECURITY_PROTOCOL     *HddSecurityProtocol = NULL;
    BOOLEAN                       Status;
    UINT32                        IdePasswordFlags = 0;
    EFI_STATUS                    ReturnStatus;
    IDE_SECURITY_CONFIG           *HddSecurityConfig
        = (IDE_SECURITY_CONFIG*)ptrHddSecurityConfig;

    //Set current HDD security page
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE

    if ( gHddSecurityCount == 0 || HddSecurityInternalData == NULL ) {
        //try to initialize, if not initialized
        gHddSecurityCount = InitHddSecurityInternalDataPtr( );
    }

    for( value=0; value<gHddSecurityCount; value++ ) {
        DataPtr = (HDD_SECURITY_INTERNAL_DATA*)IDEPasswordGetDataPtr( value );

        if ( DataPtr ) {
            HddSecurityProtocol = DataPtr->HddSecurityProtocol;

            CheckSecurityStatus(
                    HddSecurityProtocol,
                    &Status,
                    SecuritySupportedMask );
            HddSecurityConfig->Supported[value] = Status ? 1 : 0;
            CheckSecurityStatus(
                    HddSecurityProtocol,
                    &Status,
                    SecurityEnabledMask );
            HddSecurityConfig->Enabled[value] = Status ? 1 : 0;
            CheckSecurityStatus(
                    HddSecurityProtocol,
                    &Status,
                    SecurityLockedMask );
            HddSecurityConfig->Locked[value] = Status ? 1 : 0;
            CheckSecurityStatus(
                    HddSecurityProtocol,
                    &Status,
                    SecurityFrozenMask );
            HddSecurityConfig->Frozen[value] = Status ? 1 : 0;
            ReturnStatus = HddSecurityProtocol->ReturnIdePasswordFlags(
                               HddSecurityProtocol,
                               &IdePasswordFlags );

            if ( EFI_ERROR( ReturnStatus )) {
                return;
            }

            HddSecurityConfig->UserPasswordStatus[value]
                = (IdePasswordFlags & 0x00020000) ? 1 : 0;
            HddSecurityConfig->MasterPasswordStatus[value]
                = (IdePasswordFlags & 0x00010000) ? 1 : 0;

            HddSecurityConfig->ShowMaster[value] = 0x0000;

            if ( HddSecurityConfig->Locked[value] ) {
                HddSecurityConfig->ShowMaster[value] = 0x0001;
            } else if ( (DataPtr->LoggedInAsMaster)) {
                HddSecurityConfig->ShowMaster[value] = 0x0001;
            } else if ( !(HddSecurityConfig->UserPasswordStatus[value])) {
                HddSecurityConfig->ShowMaster[value] = 0x0001;
            }
        }// end if
    }
#else
    gCurrIDESecPage = value;

    DataPtr = (HDD_SECURITY_INTERNAL_DATA*)IDEPasswordGetDataPtr( value );

    if ( DataPtr ) {
        HddSecurityProtocol = DataPtr->HddSecurityProtocol;

        CheckSecurityStatus(
            HddSecurityProtocol,
            &Status,
            SecuritySupportedMask );
        HddSecurityConfig->Supported = Status ? 1 : 0;
        CheckSecurityStatus(
            HddSecurityProtocol,
            &Status,
            SecurityEnabledMask );
        HddSecurityConfig->Enabled = Status ? 1 : 0;
        CheckSecurityStatus(
            HddSecurityProtocol,
            &Status,
            SecurityLockedMask );
        HddSecurityConfig->Locked = Status ? 1 : 0;
        CheckSecurityStatus(
            HddSecurityProtocol,
            &Status,
            SecurityFrozenMask );
        HddSecurityConfig->Frozen = Status ? 1 : 0;
        ReturnStatus         = HddSecurityProtocol->ReturnIdePasswordFlags(
            HddSecurityProtocol,
            &IdePasswordFlags );

        if ( EFI_ERROR( ReturnStatus )) {
            return;
        }

        HddSecurityConfig->UserPasswordStatus
            = (IdePasswordFlags & 0x00020000) ? 1 : 0;
        HddSecurityConfig->MasterPasswordStatus
            = (IdePasswordFlags & 0x00010000) ? 1 : 0;

        HddSecurityConfig->ShowMaster = 0x0000;

        if ( HddSecurityConfig->Locked ) {
            HddSecurityConfig->ShowMaster = 0x0001;
        } else if ( (DataPtr->LoggedInAsMaster)) {
            HddSecurityConfig->ShowMaster = 0x0001;
        } else if ( !(HddSecurityConfig->UserPasswordStatus)) {
            HddSecurityConfig->ShowMaster = 0x0001;
        }
    }// end if
#endif
    return;
}

/**
    Initializes the structure IDE_SECURITY_CONFIG for all the
    HDDs present if the data pointer to the structure
    HDD_SECURITY_INTERNAL_DATA is initialized already.

    @param IDE_SECURITY_CONFIG *ptrHddSecurityConfig
    @param  UINTN value

    @retval VOID

**/
VOID
UpdateSetupDataForAllHdd(
    VOID    *ptrHddSecurityConfig,
    UINTN   value
)
{
    value = 0;

    //Set current HDD security page
    #if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
        UpdateSetupDataByHddIndex( ptrHddSecurityConfig, value );
    #else
        for ( ; value < gHddSecurityCount; value++ ) {
            UpdateSetupDataByHddIndex( ptrHddSecurityConfig, value);
        }
    #endif

    return;
}

/**
    Hook function to Initialize the structure IDE_SECURITY_CONFIG
    for the HDDs based on the token ALL_HDD_SAME_PW.

    @param IDE_SECURITY_CONFIG *ptrHddSecurityConfig
    @param  UINTN value

    @retval VOID

**/
VOID
IDEUpdateConfig(
    VOID    *ptrHddSecurityConfig,
    UINTN   value
)
{
    #if ALL_HDD_SAME_PW
     UpdateSetupDataForAllHdd( ptrHddSecurityConfig, value);
    #else
     UpdateSetupDataByHddIndex( ptrHddSecurityConfig, value);
    #endif

}
/**
    Get the Hdd name

    @param VOID

    @retval VOID

**/
UINT16
IDEPasswordGetName (
    UINT16  Index
)
{
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
    // workaround for code in special.c which fills "goto string" token with
    // HDD name string token. In this case we do not need that.
    return ConfigPromptToken;
#else
    UINTN                         size = 0;
    IDE_SECURITY_CONFIG           *HddSecurityConfig;
    HDD_SECURITY_INTERNAL_DATA    *DataPtr = NULL;

    HddSecurityConfig = VarGetNvramName (L"IDESecDev", &gIDESecGuid, NULL, &size);
    UpdateSetupDataByHddIndex (HddSecurityConfig, Index);
    VarSetNvramName (L"IDESecDev",
                     &gIDESecGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS,
                     HddSecurityConfig,
                     size);

    MemFreePointer((VOID **)&HddSecurityConfig);

    DataPtr = (HDD_SECURITY_INTERNAL_DATA*)IDEPasswordGetDataPtr( Index );

    if(DataPtr == NULL) {
        return 0;
    }

    return DataPtr->PromptToken;
#endif
}
#endif

/**
    Function Unlock the Hdd with Valid Password

    @param VOID

    @retval VOID

**/
EFI_STATUS
IDEPasswordUpdateAllHddWithValidatedPsw (
    UINT8   *Password,
    VOID    *Ptr,
    BOOLEAN bCheckUser
)
{
#if ALL_HDD_SAME_PW
    HDD_SECURITY_INTERNAL_DATA *DataPtr;
    UINTN         i;
    EFI_STATUS    Status=EFI_NOT_FOUND;
    CHAR16        PassWordBuffer[IDE_PASSWORD_LENGTH + 1];

    //For all drives
    DataPtr = HddSecurityInternalData;

    if(DataPtr == NULL) {
        return EFI_NOT_FOUND;
    }

    for ( i = 0; i < IDE_PASSWORD_LENGTH + 1; i++ ) {
        PassWordBuffer[i] = Password[i];

        if ( Password[i] == '\0' ) {
            break;
        }
    }

    for ( i = 0; i < gHddSecurityCount; i++ ) {

        if( DataPtr->Locked && !DataPtr->Validated ) {

            DataPtr->Validated = TRUE;
            Status = IDEPasswordAuthenticateHdd( PassWordBuffer,
                                                 DataPtr,
                                                 bCheckUser );
            if ( EFI_SUCCESS != Status ) {
                // Unlock failed.
                EfiLibReportStatusCode( EFI_ERROR_CODE | EFI_ERROR_MAJOR,
                                        DXE_INVALID_IDE_PASSWORD,
                                        0,
                                        NULL,
                                        NULL );
            }            
        }
        DataPtr++;
    }
#endif
    return EFI_SUCCESS;
}

/**
    Validates the password for the current HDD alone.

    @param CHAR16 *Password,
    @param VOID* Ptr,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
EFI_STATUS
IDEPasswordAuthenticateHdd (
    CHAR16  *Password,
    VOID    * Ptr,
    BOOLEAN bCheckUser
)
{
    UINT16            Control = 0;
    EFI_STATUS        Status;
    UINT8             Buffer[IDE_PASSWORD_LENGTH + 1];

    #if !SETUP_SAME_SYS_HDD_PW
    UINTN             i;
    #endif

    HDD_SECURITY_INTERNAL_DATA * DataPtr = (HDD_SECURITY_INTERNAL_DATA*)Ptr;

    MemSet( &Buffer, IDE_PASSWORD_LENGTH + 1, 0 );

#if defined(HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE) && HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE
    UpdatePasswordToNonCaseSensitive(Password, Wcslen(Password));
#endif

    #if !SETUP_SAME_SYS_HDD_PW

    for ( i = 0; i < IDE_PASSWORD_LENGTH + 1; i++ ) {
        Buffer[i] = (UINT8)Password[i];

        if ( Password[i] == L'\0' ) {
            break;
        }
    }
    #else
    MemCopy( Buffer, Password, IDE_PASSWORD_LENGTH + 1 );
    #endif

    Control = bCheckUser ? 0 : 1;

    Status = (DataPtr->HddSecurityProtocol)->SecurityUnlockPassword(
        DataPtr->HddSecurityProtocol,
        Control,
        Buffer );

    if ( EFI_ERROR( Status )) {
        return EFI_ACCESS_DENIED;
    }

    //
    //save password in case we need to disable it during the setup
    //
    MemCopy( &(DataPtr->PWD), &Buffer, IDE_PASSWORD_LENGTH + 1 );
    DataPtr->Locked = FALSE;

    if ( !bCheckUser ) {
        DataPtr->LoggedInAsMaster = TRUE;
    }

    return EFI_SUCCESS;
}

/**
    Validates the password for all the HDDs Present.

    @param CHAR16 *Password,
    @param VOID* Ptr,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
EFI_STATUS
IDEPasswordAuthenticateAllHdd (
    CHAR16  *Password,
    VOID    * Ptr,
    BOOLEAN bCheckUser
)
{
    HDD_SECURITY_INTERNAL_DATA *DataPtr;
    UINTN i;
    EFI_STATUS        Status=EFI_NOT_FOUND;

        //
        //For all drives
        //
        DataPtr = HddSecurityInternalData;

        if(DataPtr == NULL) {
            return EFI_NOT_FOUND;
        }
        

        for ( i = 0; i < gHddSecurityCount; i++ ) {

            Status = IDEPasswordAuthenticateHdd( Password,
                                                 DataPtr,
                                                 bCheckUser );
            if ( EFI_SUCCESS != Status ) {
                //
                // Unlock failed.
                //
                EfiLibReportStatusCode( EFI_ERROR_CODE | EFI_ERROR_MAJOR,
                                        DXE_INVALID_IDE_PASSWORD,
                                        0,
                                        NULL,
                                        NULL );
            }
            DataPtr++;
        }
    return Status;
}

/**
    Hook function to validate Password for the HDDs based on
    the token ALL_HDD_SAME_PW
    @param
        CHAR16 *Password,
        VOID* Ptr,
        BOOLEAN bCheckUser

    @retval VOID

**/
EFI_STATUS
IDEPasswordAuthenticate (
    CHAR16  *Password,
    VOID    * Ptr,
    BOOLEAN bCheckUser
)
{
    #if ALL_HDD_SAME_PW
     return IDEPasswordAuthenticateAllHdd( Password, Ptr, bCheckUser);
    #else
     return IDEPasswordAuthenticateHdd( Password, Ptr, bCheckUser);
    #endif

}

/**
    Get the internal Data pointer

    @param Index - Index of HDD

    @retval VOID

**/
VOID*
IDEPasswordGetDataPtr (
    UINTN   Index
)
{
    HDD_SECURITY_INTERNAL_DATA *DataPtr;

    if ( gHddSecurityCount == 0 || HddSecurityInternalData == NULL ) {
        //
        //try to initialize, if not initialized
        //
        gHddSecurityCount = InitHddSecurityInternalDataPtr( );
    }

    if ( gHddSecurityCount == 0 || HddSecurityInternalData == NULL || Index >=
         gHddSecurityCount ) {
        return NULL;
    }

    DataPtr = (HDD_SECURITY_INTERNAL_DATA*)HddSecurityInternalData;

    if(DataPtr == NULL) {
        return 0;
    }

    return (VOID*)&DataPtr[Index];
}

#if defined(SETUP_IDE_SECURITY_SUPPORT) && SETUP_IDE_SECURITY_SUPPORT
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
/**
    function to update the HDD password

    @param CONTROL_DATA *control : Selected password control data,
    @param VOID *saveData : New password

    @retval EFI_STATUS

**/
EFI_STATUS
FramePwdCallbackIdePasswordUpdate (
    CONTROL_DATA    *control,
    CHAR16          *saveData
)
{
    BOOLEAN bCheckUser = FALSE;
    VOID * data =control->ControlData.ControlPtr;
    UINT8 HardDiskNumber = 0xFF;

    // Check whether selected password control is a HDD Password control
    if (control->ControlData.ControlVariable == VARIABLE_ID_IDE_SECURITY )
    {
        // find index of currently selected HDD and type of password(user/master) to update
        SearchTseHardDiskField( &bCheckUser, NULL, &HardDiskNumber, data );

        if( HardDiskNumber != 0xFF ) // If HDD index is valid
        {
            IDEPasswordUpdate( (UINT32)HardDiskNumber, (CHAR16*) saveData, bCheckUser ); //update it
        }
        return EFI_SUCCESS;
    }
    else
        return EFI_UNSUPPORTED;
}

/**
    Function to authenticate the HDD password

    @param POPUP_PASSWORD_DATA *popuppassword,
    @param BOOLEAN *AbortUpdate,
    @param VOID *data

    @retval EFI_STATUS

**/
EFI_STATUS
PopupPwdAuthenticateIDEPwd (
    POPUP_PASSWORD_DATA *popuppassword,
    BOOLEAN             *AbortUpdate,
    VOID                *data
)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    CHAR16 *Text=NULL;
    UINT8 HardDiskNumber = 0xFF;

    // Check whether selected password control is a HDD Password control
    if(popuppassword->ControlData.ControlVariable == VARIABLE_ID_IDE_SECURITY )
    {
        BOOLEAN bCheckUser = FALSE;
        BOOLEAN EnabledBit = FALSE;
        UINTN size = 0;
        IDE_SECURITY_CONFIG *ideSecConfig;

        ideSecConfig = VarGetVariable( VARIABLE_ID_IDE_SECURITY, &size ); // Get the data from setup page
        if (NULL == ideSecConfig) {
            return EFI_NOT_FOUND;
        }
    // find index of currently selected HDD and type of password(user/master) to authenticate
        SearchTseHardDiskField( &bCheckUser, &EnabledBit, &HardDiskNumber, data );
        // Check if password has been set for selected HDD
        if(  ( HardDiskNumber != 0xFF )  && ideSecConfig->Enabled[HardDiskNumber] )
        {
                EnabledBit = TRUE;
        }

        // If password has been set then proceed
        if(EnabledBit)
        {
            if( bCheckUser || ideSecConfig->MasterPasswordStatus[HardDiskNumber] )
            {
                // Ask for the password
                Status = _DoPopupEdit( popuppassword, STRING_TOKEN(STR_OLD_PSWD), &Text);
                if(EFI_SUCCESS != Status )
                {
                    *AbortUpdate = TRUE; // Status: Password not updated
                }
                else
                {
                    // Get AMI_HDD_SECURITY_PROTOCOL instance for current HDD
                    void* DataPtr = TSEIDEPasswordGetDataPtr( HardDiskNumber );
                    Status = TSEIDEPasswordAuthenticate( Text, DataPtr, bCheckUser ); //Authenticate it
                    if(EFI_ERROR( Status ))
                    {
                        // Show error message if password is wrong
                        CallbackShowMessageBox( (UINTN)gInvalidPasswordFailMsgBox, MSGBOX_TYPE_OK );
                        *AbortUpdate = TRUE; // Status: Password not updated
                    }
                    StringZeroFreeMemory ((VOID **)&Text); // Erase string and free allocated memory
                }
            }
        }

        MemFreePointer((VOID **) &ideSecConfig); // Free setup data memory
        return EFI_SUCCESS;
    }
    return EFI_UNSUPPORTED;
}

/**
    Function to report status of HDD password

    @param POPUP_PASSWORD_DATA *popuppassword,

    @retval EFI_STATUS

**/
EFI_STATUS
CheckForIDEPasswordInstalled (
    POPUP_PASSWORD_DATA *popuppassword
)
{
    UINT8 HardDiskNumber = 0xFF;
    VOID *data = popuppassword->ControlData.ControlPtr;
    
    // Check whether selected password control is a HDD Password control
    if(popuppassword->ControlData.ControlVariable == VARIABLE_ID_IDE_SECURITY )
    {
        BOOLEAN bCheckUser = FALSE;
        BOOLEAN EnabledBit = FALSE;
        UINTN size = 0;
        IDE_SECURITY_CONFIG *ideSecConfig;

        ideSecConfig = VarGetVariable( VARIABLE_ID_IDE_SECURITY, &size ); // Get the data from setup page
        if (NULL == ideSecConfig) {
            return EFI_NOT_FOUND;
        }
    // find index of currently selected HDD and type of password(user/master) to authenticate
        SearchTseHardDiskField( &bCheckUser, &EnabledBit, &HardDiskNumber, data );
        // Check if password has been set for selected HDD
        if(  ( HardDiskNumber != 0xFF )  && ideSecConfig->Enabled[HardDiskNumber] )
        {
                EnabledBit = TRUE;
        }

        // If password has been set then proceed
        if(EnabledBit)
        {
            if( bCheckUser || ideSecConfig->MasterPasswordStatus[HardDiskNumber] )
            {
        	    MemFreePointer((VOID **) &ideSecConfig); // Free setup data memory
        	    return EFI_SUCCESS;
            }
        }

        MemFreePointer((VOID **) &ideSecConfig); // Free setup data memory
       
    }
    
    return EFI_UNSUPPORTED;
}

/**
    function to update the setup configure page after HDD password update

    @param VOID

    @retval VOID

**/

VOID
PopupPwdUpdateIDEPwd (
    VOID
)
{
    UINTN               size = 0;
    IDE_SECURITY_CONFIG *HddSecurityConfig;
    UINT8               HardDiskNumber = 0xFF;

    // Get the old setup configure data
    HddSecurityConfig = VarGetVariable( VARIABLE_ID_IDE_SECURITY, &size );
    if (NULL == HddSecurityConfig) {
        return;
    }
    // Update setup data for all HDDs
    for( HardDiskNumber = 0 ; HardDiskNumber < HddSecurityConfig->Count ; HardDiskNumber++ )
    {
        IDEUpdateConfig( (VOID*)HddSecurityConfig, HardDiskNumber );
    }
    // Set the new setup configure data
    VarSetValue (VARIABLE_ID_IDE_SECURITY, 0, size, HddSecurityConfig);
    if (gApp != NULL)
    gApp->CompleteRedraw = TRUE; // redraw setup configure page to reflect updated configuration
    MemFreePointer((VOID **)&HddSecurityConfig);
}

/**
    function to search TSE hard disk field.

    @param pbCheckUser : Password type - User/Master,
    @param pEnabledBit : Password is set / not,
    @param pHardDiskNumber : HDD index,
    @param data

    @retval VOID

**/
VOID
SearchTseHardDiskField (
    IN  OUT BOOLEAN *pbCheckUser,
    IN  OUT BOOLEAN *pEnabledBit,
    IN  OUT UINT8   *pHardDiskNumber,
    IN  VOID        *data
)
{
    UINTN   size = 0;
    IDE_SECURITY_CONFIG *HddSecurityConfig;

    HddSecurityConfig = VarGetVariable( VARIABLE_ID_IDE_SECURITY, &size );

    // Check if User password field is selected, if yes then set HDD index
    if( UefiGetQuestionOffset(data) /*data->QuestionId*/
            == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEUserPassword) )
    {
        *pHardDiskNumber = 0;
        *pbCheckUser = TRUE;
    }
    else if( UefiGetQuestionOffset(data) /*data->QuestionId*/
            == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEUserPassword_HDD2) )
    {
        *pHardDiskNumber = 1;
        *pbCheckUser = TRUE;
    }
    else if( UefiGetQuestionOffset(data) /*data->QuestionId*/
    == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEUserPassword_HDD3) )
    {
        *pHardDiskNumber = 2;
        *pbCheckUser = TRUE;
    }
    else if( UefiGetQuestionOffset(data) /*data->QuestionId*/
    == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEUserPassword_HDD4) )
    {
        *pHardDiskNumber = 3;
        *pbCheckUser = TRUE;
    }
    else if( UefiGetQuestionOffset(data) /*data->QuestionId*/
    == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEUserPassword_HDD5) )
    {
    *pHardDiskNumber = 4;
    *pbCheckUser = TRUE;
    }
    else if( UefiGetQuestionOffset(data) /*data->QuestionId*/
    == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEUserPassword_HDD6) )
    {
    *pHardDiskNumber = 5;
    *pbCheckUser = TRUE;
    }
    //
    // Check if Master password field is selected, if yes then set HDD index
    //
    else if( UefiGetQuestionOffset(data) /*data->QuestionId*/
            == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEMasterPassword) )
    {
        *pHardDiskNumber = 0;
        *pbCheckUser = FALSE;
    }
    else if( UefiGetQuestionOffset(data) /*data->QuestionId*/
            == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEMasterPassword_HDD2) )
    {
        *pHardDiskNumber = 1;
        *pbCheckUser = FALSE;
    }
    else if( UefiGetQuestionOffset(data) /*data->QuestionId*/
            == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEMasterPassword_HDD3) )
    {
        *pHardDiskNumber = 2;
        *pbCheckUser = FALSE;
    }
    else if( UefiGetQuestionOffset(data) /*data->QuestionId*/
            == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEMasterPassword_HDD4) )
    {
        *pHardDiskNumber = 3;
        *pbCheckUser = FALSE;
    }
    else if( UefiGetQuestionOffset(data) /*data->QuestionId*/
    == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEMasterPassword_HDD5) )
    {
        *pHardDiskNumber = 4;
        *pbCheckUser = FALSE;
    }
    else if( UefiGetQuestionOffset(data) /*data->QuestionId*/
            == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEMasterPassword_HDD6) )
    {
    *pHardDiskNumber = 5;
    *pbCheckUser = FALSE;
    }
    else // Question offset is not from any of the password fields
    {
        *pHardDiskNumber = 0xFF; // No HDD selected
        if( pEnabledBit != NULL )
        {
                *pEnabledBit = FALSE; // No HDD ie no password is set
        }
    }
    // if HDD index is invalid, set it to 0xFF
    if( *pHardDiskNumber >= HddSecurityConfig->Count )
    {
        *pHardDiskNumber = 0xFF;
    }

    MemFreePointer( (VOID **) &HddSecurityConfig );
    return;
}
#endif
#endif
/**
    Returns EFI_SUCCESS and sets *bHddStatus = TRUE if number of
    attempt to unlock HDD has reached MAXIMUM_HDD_UNLOCK_ATTEMPTS.

    @param VOID *DataPtr

    @retval BOOLEAN *bHddStatus

**/

EFI_STATUS
HDDStatus (
    VOID    *DataPtr,
    BOOLEAN *bHddStatus
)
{
    EFI_STATUS                   Status = 0;
    UINT16                       SecurityStatus = 0;
    HDD_SECURITY_INTERNAL_DATA   *Ptr = (HDD_SECURITY_INTERNAL_DATA *)DataPtr;

    *bHddStatus = FALSE;

    TRACE((-1,"\n HDDStatus ..."));
    
    Status = Ptr->HddSecurityProtocol->ReturnSecurityStatus( Ptr->HddSecurityProtocol, &SecurityStatus );
    if ( EFI_ERROR( Status ))
        return Status;

    if (SecurityStatus & 0x10) {
        *bHddStatus = TRUE;
    }

    return EFI_SUCCESS;
}

#if !defined(SETUP_IDE_SECURITY_SUPPORT) || SETUP_IDE_SECURITY_SUPPORT == 0
extern BOOLEAN gBrowserCallbackEnabled;
EFI_GUID       gHddSecurityLoadSetupDefaultsGuid = AMI_HDD_SECURITY_LOAD_SETUP_DEFAULTS_GUID;
EFI_EVENT      gHddSecurityLoadSetupDefaultsEvent = NULL;

VOID
EFIAPI
HddSecurityLoadSetupDefaultsFunc (
        IN EFI_EVENT Event,
        IN VOID *Context
)
{
    //pBS->CloseEvent(Event);
}

VOID
HddSecurityLoadSetupDefaults (
        VOID *defaults,
        UINTN data 
)
{
    EFI_STATUS          Status;
    if(gHddSecurityLoadSetupDefaultsEvent == NULL) {
        Status = pBS->CreateEventEx(
                     EVT_NOTIFY_SIGNAL,
                     TPL_CALLBACK,
                     HddSecurityLoadSetupDefaultsFunc,
                     NULL,
                     &gHddSecurityLoadSetupDefaultsGuid,
                     &gHddSecurityLoadSetupDefaultsEvent);
        ASSERT_EFI_ERROR(Status);
    }
    gBrowserCallbackEnabled = TRUE;
    pBS->SignalEvent(gHddSecurityLoadSetupDefaultsEvent);
    gBrowserCallbackEnabled = FALSE;
}

#endif

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
