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
//**********************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//**********************************************************************
// Revision History
// ----------------
// $Log: $
// 
// 
//
//**********************************************************************

/** @file ASFVerbosity.c
    Shows ASF POST Message

**/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/AmiSmBios.h>
#include <Protocol/SmbiosGetFlashDataProtocol.h>
#include <Protocol/AlertStandardFormat.h>
#include <Amt.h>

#define AMI_EFI_SOL_POST_MESSAGE_GUID \
{ 0xf42f3752, 0x12e, 0x4812, 0x99, 0xe6, 0x49, 0xf9, 0x43, 0x4, 0x84, 0x54 }

//#define AMI_SMBIOS_PROTOCOL_GUID    {0x5e90a50d, 0x6955, 0x4a49, { 0x90, 0x32, 0xda, 0x38, 0x12, 0xf8, 0xe8, 0xe5 }}
//EFI_GUID  gAmiSmbiosProtocolGuid  = AMI_SMBIOS_PROTOCOL_GUID;

#define SOL_NORMAL 	0x00
#define SOL_VERB 	0x40
#define SOL_QUIET 	0x20
#define SOL_BLANK 	0x60
#define SOL_SETUP 	0x00

EFI_GUID guidEfiVar = EFI_GLOBAL_VARIABLE;
//EFI_GUID gSmbiosProtocolGuid  = EFI_SMBIOS_PROTOCOL_GUID;
EFI_GUID gConsoleControlProtocolGuid = EFI_CONSOLE_CONTROL_PROTOCOL_GUID;
EFI_GUID gAmiEfiSolPostMessageGuid = AMI_EFI_SOL_POST_MESSAGE_GUID;


SMBIOS_TABLE_ENTRY_POINT    *SmBiosTableEntryPoint = 0;

static CHAR16  *SolString[] = {
    L"Intel Corporation. Copyright 2004-2016.\r\n",
    L"Intel Active Management Technology - Serial Over LAN operational mode.\r\n\r\n",
    L"BIOS Revision: ",
    L"\r\n\r\nBIOS Settings: <ESC>\r\n",
#if defined SETUP_BBS_POPUP_ENABLE && SETUP_BBS_POPUP_ENABLE
    //L"One Time Boot Menu: <F7>\r\n",
#else
    L"\r\n",
#endif
    L"Intel(R) AMT Purley WS BIOS Setup Entry\r\n",
    L"Intel Remote PC Assist Technology - Serial Over LAN operational mode.\r\n\r\n",
};

/**
    This structure represents DMI variable


**/
typedef struct {
    UINT8     Type;
    UINT8     Length;
    UINT16    Handle;
} DMIHDR;

/**
    Computes Ascii string length including 0-terminator

        
    @param String Pointer to input ASCII string



         
    @retval UINTN Length

**/
UINTN AsciiStrLength(IN CHAR8* String)
{

    UINTN Length = 0;
    if (String == NULL)
        return Length;
    while (String[Length++] != 0);

    return Length;
}
/**
    Transfer Ascii code to Unicode

        
    @param String Pointer to input ASCII string



         
    @retval VOID

**/
VOID AsciiToUnicode (
    IN  CHAR8     *AsciiString,
    OUT CHAR16    *UnicodeString
)
{
    UINT8 Index = 0;

    while (AsciiString[Index] != 0) {
        UnicodeString[Index] = (CHAR16) AsciiString[Index];
        Index++;
    }
}

/**
    Converts Ascii string to Unicode string

        
    @param AsciiString Pointer to input Ascii string
    @param UnicodeString Pointer to output Unicode string


         
    @retval VOID

**/
VOID AsciiToUnicodeSmBiosString
(
    IN CHAR8* AsciiString,
    OUT CHAR16* UnicodeString
)
{


    do {
        *(UnicodeString++) = (CHAR16)*AsciiString ;
    } while (*(AsciiString++) != 0);

}


/**
    Finds the length of SmBios structure

        
    @param SmBiosStructure Pointer to SmBios structure

         
    @retval UINTN Length

**/
UINTN GetSmBiosStructureLength
(
    IN VOID* SmBiosStructure
)
{

    UINT8* SmBiosStringsBlockPtr;
    UINTN Length = 0;
    SmBiosStringsBlockPtr = (UINT8*) SmBiosStructure + \
                            ((DMIHDR*)SmBiosStructure)->Length;
    while ( *(CHAR16*)(SmBiosStringsBlockPtr + Length) != 0) Length++;

    // determine the right value
    return ((DMIHDR*)SmBiosStructure)->Length + Length + 2;
}
/**
    Finds SmBios structure given a handle

        
    @param Handle Handle to SmBios structure

         
    @retval VOID

**/
VOID* GetSmBiosStructureByHandle
(
    IN UINT16 Handle
)
{

    UINT8* SmBiosStructurePtr = (UINT8*)(SmBiosTableEntryPoint->TableAddress);
    UINT8* SmBiosEnd = SmBiosStructurePtr + SmBiosTableEntryPoint->TableLength;

    while (SmBiosStructurePtr < SmBiosEnd) {

        if (((DMIHDR*)SmBiosStructurePtr)->Handle == Handle)
            return SmBiosStructurePtr;
        SmBiosStructurePtr += GetSmBiosStructureLength(SmBiosStructurePtr);
    }

    return NULL;
}

/**
    Finds SmBios data (Ascii string) inside SmBios structure

        
    @param SmBiosStructure Pointer to SmBios structure
    @param StringNumber SmBios String number

         
    @retval CHAR8 String

**/
CHAR8* GetSmBiosAsciiString
(
    IN VOID* SmBiosStructure, 
    IN UINT8 StringOffset
)
{

    CHAR8* SmBiosStringPtr;
    UINT8  StringNumber = *((UINT8*)SmBiosStructure + StringOffset);

    SmBiosStringPtr = (CHAR8*)SmBiosStructure + \
                        ((DMIHDR*)SmBiosStructure)->Length;
    if (StringNumber == 0)
        return NULL;

    if (SmBiosStructure != NULL)
        while ( (StringNumber--) != 1)
            SmBiosStringPtr += AsciiStrLength(SmBiosStringPtr);

    return SmBiosStringPtr;
}
ASF_BOOT_OPTIONS             *mAsfBootOptions = NULL;
/**
    Dispaly Sol Message

    @param Event 
    @param Context 



    @retval VOID


**/
VOID DisplaySolMessageEvent(IN EFI_EVENT Event, IN VOID *Context)
{
    EFI_STATUS          Status;
    UINT8               i;
    UINT8               CastlePeak;
    UINTN               Size = 0;
    CHAR8               *SmBiosData = NULL;
    CHAR16              *SmBiosString;
    AMI_SMBIOS_PROTOCOL *SmbiosProtocol;
    EFI_CONSOLE_CONTROL_SCREEN_MODE ScreenMode;
    EFI_CONSOLE_CONTROL_PROTOCOL *ConsoleControl;
    //EFI_GUID gEfiAlertStandardFormatProtocolGuid = ALERT_STANDARD_FORMAT_PROTOCOL_GUID;
    ALERT_STANDARD_FORMAT_PROTOCOL  *Asf;
    Status = gBS->LocateProtocol (
                  &gAlertStandardFormatProtocolGuid,
                  NULL,
                  &Asf
                  );
    if (EFI_ERROR (Status))
        return;
    Status = Asf->GetBootOptions (Asf, &mAsfBootOptions);
    if((mAsfBootOptions->IanaId != ASF_INTEL_CONVERTED_IANA) && 
       (mAsfBootOptions->IanaId != ASF_INDUSTRY_CONVERTED_IANA))
        return;
    Status = gBS->LocateProtocol (&gAmiSmbiosProtocolGuid, NULL, &SmbiosProtocol);
    if (EFI_ERROR(Status))return;

    Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, \
                                  NULL, (VOID**)&ConsoleControl);
    if (EFI_ERROR(Status))return;


    SmBiosTableEntryPoint = SmbiosProtocol->SmbiosGetTableEntryPoint();
    SmBiosData = GetSmBiosAsciiString( GetSmBiosStructureByHandle(0x00), 0x05);
    Size = AsciiStrLength(SmBiosData) + 1;

    Status = gBS->AllocatePool(EfiBootServicesData, \
             Size*sizeof(CHAR16), &SmBiosString);
    if (EFI_ERROR(Status))return;

    if (SmBiosData[0] != 0x20) {
        gBS->SetMem(SmBiosString, Size*sizeof(CHAR16), 0);
        AsciiToUnicodeSmBiosString(SmBiosData, SmBiosString);
    }
    CastlePeak = FALSE;
    ConsoleControl->GetMode(ConsoleControl, &ScreenMode, NULL, NULL);
    if (ScreenMode == EfiConsoleControlScreenGraphics) {
        ConsoleControl->SetMode( ConsoleControl, EfiConsoleControlScreenText );
    }
    gST->ConOut->EnableCursor( gST->ConOut, FALSE );
    gST->ConOut->ClearScreen( gST->ConOut);
    gST->ConOut->SetCursorPosition( gST->ConOut, 0, 0 );

    DEBUG((-1,"mAsfBootOptions->BootOptions = %08x",mAsfBootOptions->BootOptions));;
    // SOL Normal Mode and SOL Setup Mode
    switch (((mAsfBootOptions->BootOptions)>>8)) {
    case SOL_NORMAL:
    case SOL_VERB:

        for (i = 0; i <= 3; i++) {
            // AMT or Castle Peak ?
            if (i == 1 && CastlePeak) {
                gST->ConOut->OutputString ( gST->ConOut, SolString[5]);
            } else gST->ConOut->OutputString ( gST->ConOut, SolString[i]);

            if (i == 2)gST->ConOut->OutputString ( gST->ConOut, SmBiosString );
        }

        break;

    case SOL_QUIET:
        gST->ConOut->OutputString ( gST->ConOut, SolString[0]);
        gST->ConOut->OutputString ( gST->ConOut, L"\n\r");
        gST->ConOut->OutputString ( gST->ConOut, SolString[2]);
        gST->ConOut->OutputString ( gST->ConOut, SmBiosString);
        gST->ConOut->OutputString ( gST->ConOut, L"\n\r\n\r" );
        break;

    case SOL_BLANK:
        break;
    default:
        break;
    }//switch

    // SOL_SETUP
    if ((mAsfBootOptions->SpecialCommandParam & BIT3))
        gST->ConOut->OutputString ( gST->ConOut, SolString[4]);

    // Kill Event
    gBS->CloseEvent(Event);
}

/**
    Sol Post Message

    @param VOID


    @retval EFI_STATUS Status


**/
EFI_STATUS SolPostMessage(VOID)
{
    EFI_STATUS Status;
    VOID* Registration;
    EFI_EVENT gEvtSolMessage;


    //
    // Register for callback to 'AmiEfiSolPostMessage' protocol installation
    //
    Status = gBS->CreateEvent(
                 EFI_EVENT_NOTIFY_SIGNAL,
                 TPL_CALLBACK,
                 DisplaySolMessageEvent,
                 NULL,
                 &gEvtSolMessage
             );
    ASSERT_EFI_ERROR(Status);


    //
    // Register for protocol notifications on this event
    //
    Status = gBS->RegisterProtocolNotify (&gAmiEfiSolPostMessageGuid,
                                          gEvtSolMessage,
                                          &Registration);
    ASSERT_EFI_ERROR (Status);

    return EFI_SUCCESS;
}

EFI_STATUS ASFVerbosityEntryPoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    SolPostMessage();
    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
