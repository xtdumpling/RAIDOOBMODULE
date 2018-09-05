//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//    Rev. 1.10
//      Bug Fixed: Enhance the CMOS string 'SMCI'. According C Logical Operators,
//                 If the first operand of a logical-AND operation is equal to 0,
//                 the second operand is not evaluated.
//      Reason   : 
//      Auditor  : Yulin Yang
//      Date     : Jun/08/2017
//
//    Rev. 1.09
//      Bug Fixed: Pressing Ctrl+Home to enter BIOS recovery mode.
//      Reason   : 
//      Auditor  : Yenhsin Cho
//      Date     : Apr/07/2017
//
//    Rev. 1.08
//      Bug Fix:  Add code to log CMOS Battery Low event into SMBIOS(POST Error Type 08h).
//      Reason:
//      Auditor:  Jimmy Chiu
//      Date:     Feb/23/2017
//
//    Rev. 1.07
//      Bug Fix:  Fix that press F12 key can't boot from PXE.
//      Reason:
//      Auditor:  William Hsu
//      Date:     Jan/11/2017
//
//    Rev. 1.06
//      Bug Fixed:  Support 1024x768 logo.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Oct/28/2016
//
//    Rev. 1.05
//      Bug Fixed:  Fix Bus dynamic change issue.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Sep/06/2016
//
//     Rev. 1.04
//       Bug Fix:   add CPLD version support
//       Reason:	Get version from BMC
//       Auditor:   Sunny Yang
//       Date:      Sep/01/2016
//     Rev. 1.03
//       Bug Fix:   Fix clear CMOS issue.
//       Reason:	The system could not show "Setup default has been loaded" and "Press <DEL> to Run SETUP" and "Press <F1> to Continue Booting" message By enabling Dual boot mode and setting USB Key the first priority.
//       Auditor:   Hartmann Hsieh
//       Date:      Jul/14/2016
//
//     Rev. 1.02
//       Reason:	Fix the system date is not match build date after clear CMOS.
//       Auditor:   Hartmann Hsieh
//       Date:      Jul/06/2016
//
//     Rev. 1.01
//       Reason:	Clear CMOS feature doesn't show MSG 
//       Auditor:   Sunny Yang 
//       Date:      Jun/04/2016
//       
//     Rev. 1.00
//       Reason:	Rewrite SmcPostMsgHotKey
//       Auditor:       Leon Xu 
//       Date:          Dec/19/2014
//
//*****************************************************************//
#include "token.h"
#include "EFI.h"
#include "Protocol/AMIPostMgr.h"
#include "Protocol/LegacyBios.h"
#include "AMITSEStrTokens.h"
#include "timestamp.h"
#include "bootflow.h"
#include "HiiLib.h"
#include "LogoLib.h"
#include "mem.h"
#include "AmiHobs.h"
#include "Setup.h"
#include <PciBus.h>
#include <AmiCspLib.h>
#include "SmcLib.h"
#include "NvRam/NVRAM.h"
#include <variable.h>
#include <Library/PcdLib.h>
#include "Library/PciExpressLib.h"
#include "AmiDxeLib.h"
#include <Library/HobLib.h>
#include <Library/MmPciBaseLib.h>

//------------------Define -----------------------
#define gBS pBS
#define gST pST
#define gRT pRS

//------------------Extern -----------------------
extern BOOLEAN	gQuietBoot;
extern BOOLEAN	gEnterSetup;
extern UINT32	gBootFlow;
extern VOID GetAMITSEVariable(VOID **mSysConf, UINT8 **setup, UINTN *VarSize);

//------------------Declare -----------------------
EFI_STATUS GetScreenResolution(UINTN *ResX, UINTN *ResY);
static UINT32 HorPosition = 0, VerPosition = 0;

static CHAR16 PostMessage[][100] = {
		{L"Press <TAB> to display BIOS POST message. Press <DEL> to run Setup."},
		{L"Press <F11> to invoke Boot Menu.  Press <F12> to boot from PXE/LAN."},
};

static CHAR16 DisplayPost_Message[] = 
		L"Press <TAB> to display BIOS POST message. Press <DEL> to run Setup.";
static CHAR16 RunSetup_Message[] = 
		L"Press <DEL> to run Setup";	
static CHAR16 InvokeBootMenu_Message[] = 
		L"Press <F11> to invoke Boot Menu.  Press <F12> to boot from PXE/LAN.";
static CHAR16 Press_RunSetup_Message[] = 
		L"  Entering Setup...";
static CHAR16 Press_InvokeBootMenu_Message[] = 
		L"  Invoking Boot Menu";
static CHAR16 Press_BootFromLan_Message[] = 
		L"  Booting from PXE/LAN";
static CHAR16 Empty_Message[] = 
		L"                                                                 ";
BOOLEAN Press_RunSetup_Message_Print_One = 0;
BOOLEAN Press_InvokeBootMenu_Message_Print_One = 0;
BOOLEAN Press_BootFromLan_Message_Print_One = 0;
BOOLEAN KeyPress = FALSE;

typedef struct{
    UINT8 flag;
}SMC_HIDDEN_TPM_FLAG;

BOOLEAN		PxeBootFlag = FALSE;
EFI_GUID	mSetupGuid = SETUP_GUID;
SETUP_DATA	*mSetupData = NULL;
UINTN		mSetupSize = 0;

struct{
    CHAR16	*MemTypeStr;
    UINT8	MemType;
    UINT8	MemModuleType;
    CHAR16	*MemModuleTypeStr; // [6];
    UINT16	MemOpFreq;
    BOOLEAN	XmpEnabled;
    BOOLEAN	EccEnabled;
}MemoryInfTemplate[] = {
    {L"DDR4", 0x1A, 0x00, NULL, 0x0000, FALSE, FALSE},
    {L"LPDDR3", 0x1D, 0x00, NULL, 0x0000, FALSE, FALSE},  
    {L"LPDDR4", 0x1E, 0x00, NULL, 0x0000, FALSE, FALSE},
    {NULL,    0xFF, 0xFF, NULL, 0xFFFF, FALSE, FALSE}
};


struct{
    CHAR16	*PostErrMsgStr;
    BOOLEAN	PostErrFound;
}PostErrMsgStrTbl[] = {
    {L"No POST Error", FALSE},
    {NULL, FALSE}		// Flag
};

const EFI_GRAPHICS_OUTPUT_BLT_PIXEL ColorArray[] = {			
    {GC_COLOR_BLACK       , 0}, // case EFI_BLACK: 	// case EFI_BACKGROUND_BLACK >> 4
    {GC_COLOR_BLUE        , 0}, // case EFI_BLUE : 	// case EFI_BACKGROUND_BLUE >> 4
    {GC_COLOR_GREEN       , 0}, // case EFI_GREEN : 	// case EFI_BACKGROUND_GREEN >> 4
    {GC_COLOR_CYAN        , 0}, // case EFI_CYAN : 	// case EFI_BACKGROUND_CYAN >> 4
    {GC_COLOR_RED         , 0}, // case EFI_RED : 	// case EFI_BACKGROUND_RED >> 4
    {GC_COLOR_MAGENTA     , 0}, // case EFI_MAGENTA : 	// case EFI_BACKGROUND_MAGENTA >> 4
    {GC_COLOR_BROWN       , 0}, // case EFI_BROWN : 	// case EFI_BACKGROUND_BROWN >> 4
    {GC_COLOR_LIGHTGRAY   , 0}, // case EFI_LIGHTGRAY : // case EFI_BACKGROUND_LIGHTGRAY >> 4
    {GC_COLOR_DARKGRAY    , 0}, // case EFI_DARKGRAY : 
    {GC_COLOR_LIGHTBLUE   , 0}, // case EFI_LIGHTBLUE : 
    {GC_COLOR_LIGHTGREEN  , 0}, // case EFI_LIGHTGREEN : 
    {GC_COLOR_LIGHTCYAN   , 0}, // case EFI_LIGHTCYAN : 
    {GC_COLOR_LIGHTRED    , 0}, // case EFI_LIGHTRED : 
    {GC_COLOR_LIGHTMAGENTA, 0}, // case EFI_LIGHTMAGENTA : 
    {GC_COLOR_YELLOW      , 0}, // case EFI_YELLOW : 
    {GC_COLOR_WHITE       , 0}  // case EFI_WHITE : 
};

VOID
DisplayQuietBootHotKeyMessages()
{
    EFI_STATUS Status;
    EFI_STATUS PostManagerStatus;
    AMI_POST_MANAGER_PROTOCOL *PostManager = NULL;
    EFI_GUID mAmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
    UINT32 HorResolution = 1024, VerResolution = 768;
    BOOLEAN HotKeySupport = TRUE;
    UINT32 LogoWidth = 640, LogoHeight = 480;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Foreground = {0, 0, 0, 0};
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Background = {0xFF, 0xFF, 0xFF, 0};	
    UINTN  Index, MaxLength;

    EFI_GUID SetupGuid = SETUP_GUID;
    SETUP_DATA SetupData;
    UINTN VariableSize = sizeof(SETUP_DATA);

    Status = pRS->GetVariable(L"Setup", &mSetupGuid, NULL, &VariableSize, &SetupData);
    if(!EFI_ERROR(Status)){
	Foreground = ColorArray[SetupData.HotKey_Msg_Foreground_Color];	
	Background = ColorArray[SetupData.HotKey_Msg_Background_Color];
	HotKeySupport = SetupData.HotKey_Support;
    }

    if(!HotKeySupport)	return;

    PostManagerStatus = gBS->LocateProtocol(&mAmiPostManagerProtocolGuid, NULL, &PostManager);

    Status = GetScreenResolution ((UINTN*)&HorResolution, (UINTN*)&VerResolution);

    DEBUG((-1, "HorResolution = %08x VerResolution = %08x\n", HorResolution, VerResolution));
    if(!EFI_ERROR(PostManagerStatus)){
	for (Index = 0, MaxLength = 0; Index < sizeof(PostMessage)/(sizeof(CHAR16) * 100); Index ++){
	    if (MaxLength < StrLen (&PostMessage [Index][0])){
		MaxLength = StrLen (&PostMessage [Index][0]);
	    }
	}
	HorPosition = (HorResolution - ((UINT32)MaxLength * EFI_GLYPH_WIDTH)) / 2;
	VerPosition = (VerResolution - LogoHeight) / 2;
	PostManager->DisplayQuietBootMessage(DisplayPost_Message, HorPosition, VerPosition - 20,
			CA_AttributeLeftBottom, Foreground, Background);
	PostManager->DisplayQuietBootMessage(InvokeBootMenu_Message, HorPosition, VerPosition - 40,
			CA_AttributeLeftBottom, Foreground, Background);
    }
}

VOID
SmcCheckForKey(
    EFI_EVENT Event,
    VOID *Context
)
{
    //EFI_STATUS Status;
    EFI_STATUS PostManagerStatus;
    AMI_POST_MANAGER_PROTOCOL *PostManager = NULL;
    EFI_GUID mAmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Foreground = {0, 0, 0, 0};
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Background = {0xFF, 0xFF, 0xFF, 0};	

    EFI_STATUS Status;
    EFI_GUID SetupGuid = SETUP_GUID;
    SETUP_DATA SetupData;
    UINTN VariableSize = sizeof(SETUP_DATA);

    if(KeyPress)	return;

    Status = pRS->GetVariable(L"Setup", &SetupGuid, NULL, &VariableSize, &SetupData);
    if(!EFI_ERROR(Status)){
	Foreground = ColorArray[SetupData.HotKey_Msg_Foreground_Color];	
	Background = ColorArray[SetupData.HotKey_Msg_Background_Color];	
    }    

    // check if enter setup menu
    if(gEnterSetup){
	if(gQuietBoot){
	    PostManagerStatus = gBS->LocateProtocol(&mAmiPostManagerProtocolGuid, NULL, &PostManager);
	    if(!EFI_ERROR(PostManagerStatus)){
		PostManager->DisplayQuietBootMessage(Press_RunSetup_Message, 0, 0,
				CA_AttributeLeftBottom, Foreground, Background);
	    }
	}
	else{
	    if(Press_RunSetup_Message_Print_One == 0){
		PostManagerDisplayPostMessage(Press_RunSetup_Message);
		Press_RunSetup_Message_Print_One = 1;
	    }
	}
	KeyPress = TRUE;
    }
	
    // check if boot to boot menu
    if(gBootFlow == BOOT_FLOW_CONDITION_BBS_POPUP){
	if(gQuietBoot){
	    PostManagerStatus = gBS->LocateProtocol(&mAmiPostManagerProtocolGuid, NULL, &PostManager);
	    if(!EFI_ERROR(PostManagerStatus)){
		PostManager->DisplayQuietBootMessage (Press_InvokeBootMenu_Message, 0, 0,
				CA_AttributeLeftBottom, Foreground, Background);
	    }
	}
	else{
	    if(Press_InvokeBootMenu_Message_Print_One == 0){
		PostManagerDisplayPostMessage(Press_InvokeBootMenu_Message);
		Press_InvokeBootMenu_Message_Print_One = 1;
	    }
	}
	KeyPress = TRUE;
    }

    // check if boot from PXE/LAN
    if((gBootFlow == BOOT_FLOW_CONDITION_OEM_KEY2) && (mSetupData != NULL) && (mSetupData->PxeOpRom >= 2)){
	if(gQuietBoot){
	    PostManagerStatus = gBS->LocateProtocol(&mAmiPostManagerProtocolGuid, NULL, &PostManager);
	    if(!EFI_ERROR(PostManagerStatus)){
		PostManager->DisplayQuietBootMessage (Press_BootFromLan_Message, 0, 0,
				CA_AttributeLeftBottom, Foreground, Background);
	    }
	}
	else{
	    if(Press_BootFromLan_Message_Print_One == 0){
		PostManagerDisplayPostMessage (Press_BootFromLan_Message);
		Press_BootFromLan_Message_Print_One = 1;
	    }
	}
	KeyPress = TRUE;
    }

    return;
}

VOID
PauseAndDisplayEnterSetupContinue()
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_INPUT_KEY Key;
    UINTN Index = 0;

#ifndef PCH_SERVER_BIOS_FLAG
    Status = GetEfiVariable(
                L"Setup",
                &mSetupGuid,
                NULL,
                &mSetupSize,
                &mSetupData);

    if (!EFI_ERROR (Status)) {
        if(mSetupData->SmcF1KeySkipErrorMsg == 0)
            return;
    }
#endif // #ifndef PCH_SERVER_BIOS_FLAG

    Key.ScanCode = 0x0000;
    Key.UnicodeChar = 0x0000;

    if(SETUP_ENTRY_SCAN == EFI_SCAN_F1)
        PostManagerDisplayPostMessage (L"Press <F1> to Run SETUP");
    else if(SETUP_ENTRY_SCAN == EFI_SCAN_F2)
        PostManagerDisplayPostMessage (L"Press <F2> to Run SETUP");
    else
        PostManagerDisplayPostMessage (RunSetup_Message);

    PostManagerDisplayPostMessage (L"Press <F1> to Continue Booting");

    if(Key.ScanCode == SETUP_ENTRY_SCAN){
        gEnterSetup = TRUE;
    }
}

BOOLEAN
SmcProcessConInAvailability(
    IN	VOID
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    CHAR16	*TmpStrBuffer = NULL;
    CHAR8	TempAStr[100];
    CHAR8	BIOS_Ver[20], BIOS_Date[10], Manufacturer[30], Product[20];
    CHAR16	TempStrBuff[100];
    UINT8	TempIndex;
    UINT32	TempData32 = 0;
    UINT8	MemType;
    UINT16	MemFreq;
    UINTN	MemSize;
    BOOLEAN	EccModeEnabled;
    BOOLEAN	XmpMode = FALSE, EccMode = FALSE, TempFound = FALSE;
    UINTN	Index = 0;
    UINT16	CpuCurrentSpeed = 0;
    UINTN	ms = 0;
    CHAR16	*text = NULL;
    EFI_INPUT_KEY	Key;

    DEBUG((-1, "Smc post message code entry\n"));
    if(mSetupData == NULL){
	GetEfiVariable(
			L"Setup",
			&mSetupGuid,
			NULL,
			&mSetupSize,
			&mSetupData);
    }
	
    gBS->SetMem(
		    TempAStr,
		    sizeof (TempAStr),
		    0x00);

    MemSet(TempAStr, sizeof(TempAStr), 0);
    MemSet(BIOS_Ver, sizeof(BIOS_Ver), 0);
    MemSet(BIOS_Date, sizeof(BIOS_Date), 0);
    MemSet(Manufacturer, sizeof(Manufacturer), 0);
    MemSet(Product, sizeof(Product), 0);
    MemSet(TempStrBuff, 100, 0);

    SmcLibGetBiosInforInSmbios(TempAStr, BIOS_Ver, BIOS_Date);
    SmcLibGetBoardInforInSmbios(Manufacturer, Product);

    PostManagerSetCursorPosition(0, 2);

    Swprintf(TempStrBuff, L"%a %a BIOS Date:%a Rev:%a",
		    Manufacturer, Product, BIOS_Date, BIOS_Ver);

    PostManagerDisplayPostMessage(TempStrBuff);
    MemSet(TempAStr, sizeof(TempAStr), 0);

    SmcLibGetCPUInforBySmbios(TempAStr, NULL, &CpuCurrentSpeed, NULL, NULL);
	
    PostManagerSetCursorPosition(0, 4);
    MemSet(TempStrBuff, 100, 0);
    Swprintf(TempStrBuff, L"CPU : %a", TempAStr);
    PostManagerDisplayPostMessage(TempStrBuff);

    MemSet(TempStrBuff, 100, 0);
    Swprintf(TempStrBuff, L" Speed : %d.%02d GHz", CpuCurrentSpeed / 1000, (CpuCurrentSpeed % 1000) / 10);
    PostManagerDisplayPostMessage(TempStrBuff);

    PostManagerSetCursorPosition(0, 20);
    PostManagerDisplayPostMessage(L"Press DEL to run Setup\nPress F11 to invoke Boot Menu\nPress F12 to boot from PXE/LAN\n");
    PostManagerSetCursorPosition(0, 6);

    MemSize = 0;

    Status = SmcLibGetPhysicalMemoryBySmbios(&EccModeEnabled);
    Status = SmcLibGetMemoryInforBySmbios(&MemType, NULL, &MemFreq, &MemSize);

    if(MemType != 0x00){
	for(TempIndex = 0; MemoryInfTemplate[TempIndex].MemTypeStr != NULL; TempIndex++){
	    if(MemType == MemoryInfTemplate[TempIndex].MemType){
		    MemoryInfTemplate[TempIndex].MemModuleType = 0;
		    MemoryInfTemplate[TempIndex].MemOpFreq     = MemFreq;
		    MemoryInfTemplate[TempIndex].EccEnabled    = EccModeEnabled;
		    MemSet(TempStrBuff, 100, 0);
		    Swprintf(
				    TempStrBuff,
				    L"The IMC is operating with %s %d MHz",
				    MemoryInfTemplate[TempIndex].MemTypeStr,
				    MemoryInfTemplate[TempIndex].MemOpFreq);

		    PostManagerDisplayPostMessage (TempStrBuff);
		    break;
	    }
	}
    }
#if IPMI_SUPPORT
    Status = SendIPMIGetCPLDVersion ();
#endif //IPMI_SUPPORT
  
    // Wait for key stroke.
    do{
	Status = gST->ConIn->ReadKeyStroke( gST->ConIn, &Key );
	if(!EFI_ERROR(Status)){						
	    //check for enter setup hot key.
	    if(Key.ScanCode == SETUP_ENTRY_SCAN){
		gEnterSetup = TRUE;
		break;
	    }
	    //check for bbs popup menu hot key
	    if(Key.ScanCode == POPUP_MENU_ENTRY_SCAN){
		gBootFlow = BOOT_FLOW_CONDITION_BBS_POPUP;
		break;
	    }
	    //add more hot key check if need.
	}
	//wait for 1 seconds to make user can press hot key easily.
	pBS->Stall(1000);		// stall 1 ms
	ms++;
	if(ms == 500)	break;	// 0.5 sec
    }while(1);

    if((mSetupData != NULL) && (mSetupData->SmcF1KeySkipErrorMsg == 1)){
	//
	// To get POST error
	//

	//
	// To display POST error message
	//
	for(TempIndex = 0; PostErrMsgStrTbl[TempIndex].PostErrMsgStr != NULL; TempIndex++){
	    if(PostErrMsgStrTbl[TempIndex].PostErrFound){
		PostManagerDisplayPostMessage(L"\n");
		PostManagerDisplayPostMessage(PostErrMsgStrTbl[TempIndex].PostErrMsgStr);
		TempFound = TRUE;
	    }
	}

	if(TempFound){
	    PauseAndDisplayEnterSetupContinue();
	}
    }
    return FALSE;
}

EFI_STATUS
SmcKeyTabHook(
    IN	BOOT_FLOW	*BootFlowPtr
)
{
    EFI_STATUS	Status = EFI_SUCCESS;

    if(gQuietBoot){
	gQuietBoot = FALSE;
	CleanUpLogo();
	InitPostScreen();
    }

    return Status;
}

EFI_STATUS
SmcKeyF12Hook(
    IN	BOOT_FLOW	*BootFlowPtr
)
{
    EFI_STATUS	Status = EFI_SUCCESS;

    if((mSetupData != NULL) && (mSetupData->PxeOpRom >= 2)){
	PxeBootFlag = TRUE;
    }

    return Status;
}

VOID
SmcPxeBoot(
    IN	BOOLEAN	LegacyBoot
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    EFI_LEGACY_BIOS_PROTOCOL	*LegacyBios = NULL;
    UINT16	HddCount = 0, BbsCount = 0;
    HDD_INFO	*HddInfo = NULL;
    BBS_TABLE	*BbsTable = NULL;
    UINT16	i, j, NewBootPriority;
    UINT8	OnboardLAN;
    UINT16	LAN_BDF[4] = {0, 0, 0, 0};
    UINT32	TempBus, TempDev, TempFun;
    BOOLEAN	PxeBootRomFound = FALSE;

    DEBUG((-1, "Smc post message PXE boot\n"));
    Status = gBS->LocateProtocol(
		    &gEfiLegacyBiosProtocolGuid,
		    NULL,
		    &LegacyBios);

    if(Status)	return;

    Status = LegacyBios->GetBbsInfo(
		    LegacyBios,
		    &HddCount,
		    &HddInfo,
		    &BbsCount,
		    &BbsTable);

    if(Status)	return;
    
    LAN_BDF[0] = SmcBDFTransfer(PcdGet32(PcdSmcOBLan1BDF));
    LAN_BDF[1] = SmcBDFTransfer(PcdGet32(PcdSmcOBLan2BDF));
    LAN_BDF[2] = SmcBDFTransfer(PcdGet32(PcdSmcOBLan3BDF));
    LAN_BDF[3] = SmcBDFTransfer(PcdGet32(PcdSmcOBLan4BDF));
    NewBootPriority = sizeof(LAN_BDF) / sizeof(UINT16);
    
    for(i = 0; i < BbsCount; i++){
	if((BbsTable[i].DeviceType == BBS_EMBED_NETWORK) || (BbsTable[i].DeviceType == BBS_BEV_DEVICE)){
	    OnboardLAN = 0;
	    for(j = 0; j < (sizeof(LAN_BDF)/sizeof(UINT16)); j++){
		if(!LAN_BDF[j])	continue;

		if(LAN_BDF[j] != 0xffff){
			TempBus = GetOnBoardLanBus(LAN_BDF[j]); 
			TempDev = 0;
		    TempFun = 0;
		}
		else{
		    TempBus = 0;
		    TempDev = 31;
		    TempFun = 6;
		}

		if(TempBus == 0xff)	continue;

		if((BbsTable[i].Bus == TempBus) && (BbsTable[i].Device == TempDev) &&
			(BbsTable[i].Function == TempFun)){
		    BbsTable[i].BootPriority = j;
		    OnboardLAN = 1;
		    break;
		}
	    }
	    if(!OnboardLAN)
		BbsTable[i].BootPriority = NewBootPriority++;

	    PxeBootRomFound = TRUE;
	}
    }

    //move other boot device into bbs
    for(i = 0; i < BbsCount; i++){
	if((BbsTable[i].BootPriority != BBS_DO_NOT_BOOT_FROM) && 
	    (BbsTable[i].BootPriority != BBS_LOWEST_PRIORITY) &&
	    (BbsTable[i].BootPriority != BBS_UNPRIORITIZED_ENTRY) &&
	    (BbsTable[i].BootPriority != BBS_IGNORE_ENTRY) &&
	    (BbsTable[i].DeviceType != BBS_EMBED_NETWORK) &&
	    (BbsTable[i].DeviceType != BBS_BEV_DEVICE))
	    BbsTable[i].BootPriority = NewBootPriority++;
    }

    if(PxeBootRomFound){
	if(!LegacyBoot){
	    LegacyBios->LegacyBoot(
			    LegacyBios,
			    NULL,
			    0,
			    0);
	}
    }
    return;
}

VOID
SmcBeforeLegacyBootLaunch(
    IN 	VOID
)
{
    if(PxeBootFlag)
	SmcPxeBoot(TRUE);

    return;
}


VOID
SmcBeforeEfiBootLaunch(
    IN	VOID
)
{
    if(PxeBootFlag)
	SmcPxeBoot(FALSE);

    return;
}


// -----------------------------
// -------- SmcPwdChk.c --------
// -----------------------------
#define Str2No(A)	(A - '0')
#define TwoDigitStr2BCD(String) (Str2No(String[0])*0x10+Str2No(String[1]))
UINT8
BCDToDec(
    IN	UINT8	BCD
)
{
    UINT8	FirstDigit = BCD & 0xf;
    UINT8	SecondDigit = BCD >> 4;;

    return	SecondDigit * 10  + FirstDigit;
}

UINT8
DecToBCD(
    UINT8 Dec
)
{
    UINT8	FirstDigit = Dec % 10;
    UINT8	SecondDigit = Dec / 10;

    return (SecondDigit << 4) + FirstDigit;
}

void
WriteRtcIndex(
    IN	UINT8	Index,
    IN	UINT8	Value
)
/*++

Routine Description:

  Writes to an RTC Index register

Arguments:

  Index - Index to be written
  Value - Value to be written to Index register

Returns:

  VOID

--*/
{
    IoWrite8(0x70, Index | 0x80);
    if(Index <= 9){
	Value = DecimalToBcd8(Value);
    }
    //
    // Write Register.
    //
    IoWrite8(0x71, Value);
}

VOID
SetSysTimeToBuildTime()
{
    EFI_TIME	Time;
    UINT8	*strYear = FOUR_DIGIT_YEAR;
    UINT8	*strMonth = TWO_DIGIT_MONTH;
    UINT8	*strDay = TWO_DIGIT_DAY;
    UINT8	*strHour = TWO_DIGIT_HOUR;
    UINT8	*strMin = TWO_DIGIT_MINUTE;
    UINT8	*strSec = TWO_DIGIT_SECOND;
  
    Time.Hour = BCDToDec(TwoDigitStr2BCD(strHour));
    Time.Minute = BCDToDec(TwoDigitStr2BCD(strMin));
    Time.Second = BCDToDec(TwoDigitStr2BCD(strSec));
    Time.Month = BCDToDec(TwoDigitStr2BCD(strMonth));
    Time.Day = BCDToDec(TwoDigitStr2BCD(strDay));
    Time.Year = (Str2No(strYear[0])*1000+Str2No(strYear[1])*100+Str2No(strYear[2])*10+Str2No(strYear[3]));

    WriteRtcIndex(ACPI_CENTURY_CMOS, 0x20);
    WriteRtcIndex(9, Time.Year % 100);
    WriteRtcIndex(8,Time.Month);
    WriteRtcIndex(7,Time.Day);
    WriteRtcIndex(4,Time.Hour);
    WriteRtcIndex(2,Time.Minute);
    WriteRtcIndex(0,Time.Second);
}

BOOLEAN
IsPasswordCmosClear()
{
    EFI_STATUS	Status;
    SETUP_DATA	*SetupData = NULL;
    UINTN	VariableSize = 0;
    UINT8	CMOS_SecurityFlag[4];

    CMOS_SecurityFlag[0] = GetCmosTokensValue(Q_SMCSecurityJumer_S);
    CMOS_SecurityFlag[1] = GetCmosTokensValue(Q_SMCSecurityJumer_M);
    CMOS_SecurityFlag[2] = GetCmosTokensValue(Q_SMCSecurityJumer_C);
    CMOS_SecurityFlag[3] = GetCmosTokensValue(Q_SMCSecurityJumer_I);

    if(*((UINT32 *)CMOS_SecurityFlag) != (UINT32)'ICMS'){
        SetCmosTokensValue(Q_SMCSecurityJumer_S, 'S');
        SetCmosTokensValue(Q_SMCSecurityJumer_M, 'M');
        SetCmosTokensValue(Q_SMCSecurityJumer_C, 'C');
        SetCmosTokensValue(Q_SMCSecurityJumer_I, 'I');

        Status = GetEfiVariable(
                    L"Setup",
                    &mSetupGuid,
                    NULL,
                    &VariableSize,
                    &SetupData);

        if(!EFI_ERROR(Status)){
            if(SetupData->SmcNotFirstBoot){
                return	TRUE; //clear
            }else{
                SetupData->SmcNotFirstBoot = 1;
                Status = pRS->SetVariable (
                            L"Setup",
                            &mSetupGuid,
                            EFI_VARIABLE_NON_VOLATILE |
                            EFI_VARIABLE_BOOTSERVICE_ACCESS,
                            sizeof(SETUP_DATA),
                            SetupData);
                return	FALSE;
            }
        }
        return TRUE; //clear
    }

    Status = GetEfiVariable(
                L"Setup",
                &mSetupGuid,
                NULL,
                &VariableSize,
                &SetupData);

    if(!EFI_ERROR(Status)){
        if (SetupData->SmcNotFirstBoot == 0) { // If the value is 1, don't save variable again.
            SetupData->SmcNotFirstBoot = 1;
            Status = pRS->SetVariable (
                        L"Setup",
                        &mSetupGuid,
                        EFI_VARIABLE_NON_VOLATILE |
                        EFI_VARIABLE_BOOTSERVICE_ACCESS,
                        sizeof(SETUP_DATA),
                        SetupData);
        }
    }
    return FALSE;
}

BOOLEAN
IsPasswordJumperClear()
{
    EFI_BOOT_MODE BootMode;

    BootMode = GetBootModeHob ();

    if (BootMode == BOOT_WITH_DEFAULT_SETTINGS)
        return TRUE;
    else
        return FALSE;
}


VOID
CheckPasswordJumperStatus()
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	*setup = NULL;
    AMITSESETUP	*mSysConf = NULL;
    UINTN	VarSize = 0;

    if(!IsPasswordCmosClear() && !IsPasswordJumperClear()){
	return;
    }
    GetAMITSEVariable(&mSysConf,&setup,&VarSize);
    pBS->SetMem(mSysConf->UserPassword, SETUP_PASSWORD_LENGTH * sizeof(CHAR16), 0);
    pBS->SetMem(mSysConf->AdminPassword, SETUP_PASSWORD_LENGTH * sizeof(CHAR16), 0);
    VarSetNvram(VARIABLE_ID_AMITSESETUP, mSysConf, VarSize);

    // Clean screen for show message.
    if(gQuietBoot){
	gQuietBoot = FALSE;
	CleanUpLogo();
	InitPostScreen();
    }

    PostManagerDisplayPostMessage (L"Setup default has been loaded.");
    ERROR_CODE(DXE_SB_BAD_BATTERY, EFI_ERROR_MAJOR);
    SetSysTimeToBuildTime();
    PauseAndDisplayEnterSetupContinue();
}
