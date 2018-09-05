//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.16
//    Bug Fix: Fix POST hang up when set MMCFG Base to 3G.
//    Reason : 
//    Auditor: Isaac Hsu
//    Date   : May/17/2017
//
//  Rev. 1.15
//    Bug Fix: Fix compiler error when turn off the 'EarlyVideo_SUPPORT' token.
//    Reason : 
//    Auditor: Max Mu
//    Date   : Apr/24/2017
//
//  Rev. 1.14
//    Bug Fix: For EarlyGraphic, Change Font Style from 8x16 to 8x19.
//    Reason : 
//    Auditor: Isaac Hsu
//    Date   : Apr/07/2017
//
//  Rev. 1.13
//    Bug Fix: Fix Intel P2P has yellow mark when booting with Offboard VGA
//             and EarlyVideo enabled.
//    Reason : 
//    Auditor: Isaac Hsu
//    Date   : Apr/05/2017
//
//  Rev. 1.12
//    Bug Fix:  Fix System cannot enter UEFI OS for some projects.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Mar/20/2017
//
//  Rev. 1.11
//    Bug Fix:  Modify location of graphic mode progress-code.
//    Reason:   
//    Auditor:  YuLin Yang
//    Date:     Mar/17/2017
//
//  Rev. 1.10
//    Bug Fix:  Set Foreground color to white and Background color to black
//              when Changed Logo by AMI utility ChangeLogo.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Mar/16/2017
//
//  Rev. 1.09
//    Bug Fix:  Fix System cannot enter UEFI OS.
//    Reason:   It cannot get PCD at runtime stage.
//    Auditor:  Isaac Hsu
//    Date:     Mar/16/2017
//
//  Rev. 1.08
//    Bug Fix:  Fix Early Graphic Logo malfunction for DP platform.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Mar/15/2017
//
//  Rev. 1.07
//    Bug Fix:  Early Graphic Logo Support.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Mar/14/2017
//
//  Rev. 1.06
//    Bug Fix:  Fix label 52 hang 0x99 when early video enable.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Oct/27/2016
//
//  Rev. 1.05
//    Bug Fix:  Use AMIBCP to disable boot procedure messages displaying.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jun/21/2016
//
//  Rev. 1.04
//      Bug Fix: Turn on Early-Video in earlier PEI.
//      Reason : To sync with Grantley
//      Auditor: Yenhsin Cho
//      Date   : May/27/2016
//
//  Rev. 1.03
//      Bug Fix: Implement SMC_SIMPLETEXTOUT_PROTOCOL.
//      Reason : Use the protocol instead of the previous lib.
//      Auditor: Yenhsin Cho
//      Date   : May/27/2016
//
//  Rev. 1.02
//      Bug Fixed:  Fix message in wrong place when Quiet Boot disabled
//      Reason:     Save current position then restore it.
//      Auditor:    Mark Chen
//      Date:       May/03/2015
//
//  Rev. 1.01
//      Bug Fixed:  Change Foreground and Background color
//      Reason:     For new supermicro green logo.
//      Auditor:    Mark Chen
//      Date:       Apr/29/2015
//
//     Rev. 1.00
//       Reason:	Rewrite SmcPostMsgHotKey
//       Auditor:       Leon Xu 
//       Date:          Dec/19/2014
//
//****************************************************************************
//****************************************************************************
#include <UEFI.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Dxe.h>
#include <Protocol/ReportStatusCodeHandler.h>
#include <Protocol/AMIPostMgr.h>
#include <AmiTsePkg/Include/AMIVfr.h>
#include <AmiModulePkg/AmiStatusCode/StatusCodeMap.c>
#include "SmcSimpleTextOutProtocol.h"
#include <Library/PciLib.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include "SmcPostPrintLib.h"
#include <Protocol/GraphicsOutput.h>
#if EarlyVideo_Mode
#include "font8x19.c"
UINT8   Attr = 0x0F; // Color Attribute
UINT8   AttrBds = 0x0F; // Color Attribute for BDS Stage
#endif


#define TextAttrib(FORE, BACK)  ((FORE<<4)|(BACK&0x0F))
#define GetHiToAscii(NUM)       ConvNumToAsciiCode((NUM>>4)&0x0F)
#define GetLoToAscii(NUM)       ConvNumToAsciiCode((NUM)&0x0F)

typedef struct {
    EFI_STATUS_CODE_VALUE	Value;
    CHAR16*	str16;
} SMCPOSTMSGSTRINGTABLE;

#if EarlyVideo_Mode
typedef struct {
    UINT8 *FontTable;
    UINT32 FontWidth;
    UINT32 FontHeight;
    UINT32 XScale;
    UINT32 YScale;
    UINT32 FG;
    UINT32 BG;
} _FontInfo;

#define WriteDD(baseaddr,offset,data)   *(UINT32 *)((UINT32)(baseaddr)+(UINT32)(offset))=(UINT32)(data)
#define WriteDW(baseaddr,offset,data)   *(UINT16 *)((UINT32)(baseaddr)+(UINT32)(offset))=(UINT16)(data)

const EFI_GRAPHICS_OUTPUT_BLT_PIXEL ColorArray[] = {            
    {GC_COLOR_BLACK       , 0}, // case EFI_BLACK:  // case EFI_BACKGROUND_BLACK >> 4
    {GC_COLOR_BLUE        , 0}, // case EFI_BLUE :  // case EFI_BACKGROUND_BLUE >> 4
    {GC_COLOR_GREEN       , 0}, // case EFI_GREEN :     // case EFI_BACKGROUND_GREEN >> 4
    {GC_COLOR_CYAN        , 0}, // case EFI_CYAN :  // case EFI_BACKGROUND_CYAN >> 4
    {GC_COLOR_RED         , 0}, // case EFI_RED :   // case EFI_BACKGROUND_RED >> 4
    {GC_COLOR_MAGENTA     , 0}, // case EFI_MAGENTA :   // case EFI_BACKGROUND_MAGENTA >> 4
    {GC_COLOR_BROWN       , 0}, // case EFI_BROWN :     // case EFI_BACKGROUND_BROWN >> 4
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

#endif //EarlyVideo_Mode


STATIC SMCPOSTMSGSTRINGTABLE SmcPostMsgDXEStringTable[] = {
	{DXE_CORE_STARTED, L"  DXE--DXE Phase Start.."},
	{DXE_SB_INIT, L"  DXE--SB Initialization.."},
	{DXE_ACPI_INIT, L"  DXE--ACPI Initialization.."},
	{DXE_CSM_INIT, L"  DXE--CSM Initialization.."},
	{DXE_PCI_BUS_BEGIN, L"  DXE--BIOS PCI Bus Enumeration.."},
	{DXE_PCI_BUS_ENUM, L"  DXE--BIOS PCI Bus Enumeration.."},
	{DXE_PCI_BUS_HPC_INIT, L"  DXE--PCI Bus HPC Initialization.."},
	{DXE_PCI_BUS_REQUEST_RESOURCES, L"  DXE--PCI Bus Request Resources.."},
	{DXE_PCI_BUS_ASSIGN_RESOURCES, L"  DXE--PCI Bus Assign Resources.."},
	{DXE_IDE_BEGIN, L"  DXE--AHCI Initialization.."},
	{DXE_BDS_CONNECT_DRIVERS, L"  DXE--BIOS PCI Bus Enumeration.."},
	{DXE_CON_OUT_CONNECT, L"  DXE--Console In Device Connect.."},
	{DXE_CON_IN_CONNECT, L"  DXE--Console Out Device Connect.."},
	{DXE_SIO_INIT, L"  DXE--SuperIO Initialization.."},
	{DXE_USB_BEGIN, L"  DXE--USB Initialization.."},
	{DXE_USB_RESET, L"  DXE--USB Bus Reset.."},
	{DXE_USB_DETECT, L"  DXE--USB Device Detect.."},
	{DXE_USB_ENABLE, L"  DXE--USB Device Enable.."},
	{DXE_SETUP_START, L"  DXE--Go to BIOS Setup.."},
	{DXE_READY_TO_BOOT, L"  DXE--Ready to Boot.."},
	{DXE_LEGACY_BOOT, L"  DXE--Legacy Boot Start.."},
	{DXE_SB_SMM_INIT, L"  DXE--SMM Initialization.."},
	{DXE_LEGACY_OPROM_INIT, L"  DXE--Legacy OPROM Initialization.."},
	{0, 0}
};

SMC_SIMPLETEXTOUT_PROTOCOL  mSmcSimpleTextOutProtocol;
EFI_RSC_HANDLER_PROTOCOL    *mRscHandlerProtocol = NULL;

BOOLEAN  mDriversConnected = FALSE;
#if EarlyVideo_SUPPORT
BOOLEAN VgaDecodeEnable = FALSE;
#endif

VOID
SmcFnOpenVgaDecode(VOID)
{
#if EarlyVideo_SUPPORT
    UINT8 Data8;
    UINT32 VgaBar = PcdGet32(PcdSmcVgaBar);
//    DEBUG((-1, "SmcFnOpenVgaDecode Start.\n"));

    // Determine if PCI Resources assigned by PciBus Driver
    Data8 = PciRead8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x19));
    if(Data8!=Aspeed_P2P_BUS)
        return;

    VgaDecodeEnable = TRUE;

    //
    // Enable I/O & Mem space on PCI Command register on both VGA Controller and PCI Bridge
    //
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x19), Aspeed_P2P_BUS);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x1a), Aspeed_VGA_BUS);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x18), Aspeed_P2P_BUS);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x19), Aspeed_VGA_BUS);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x1a), Aspeed_VGA_BUS);

    //Config Intel P2P
    PciWrite16(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x1c), 0xD0D0);
#if EarlyVideo_Mode
    PciWrite32(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x20), (UINT32)((VgaBar+0x1000000)+(VgaBar>>16)));
#else
    PciWrite32(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x20), 0xFA00F800);
#endif
    PciWrite32(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x24), 0x0001FFF1);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x0c), 0x10);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x3c), 0x05);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x3e), 0x18);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x04), 0x07);

    //Config Aspeed P2P
    PciWrite16(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x1c), 0xD1D1);
#if EarlyVideo_Mode
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x20), (UINT32)((VgaBar+0x1000000)+(VgaBar>>16)));
#else
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x20), 0xFA00F800);
#endif
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x24), 0x0001FFF1);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x0c), 0x10);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x3c), 0x07);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x3e), 0x18);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x04), 0x07);

    //Config Aspeed VGA
#if EarlyVideo_Mode
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x10), VgaBar);
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x14), (VgaBar+0x1000000));
#else
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x10), 0xF8000000);
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x14), 0xFA000000);
#endif
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x18), 0x0000D001);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x3C), 0x07);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x04), 0x07);

//    DEBUG((-1, "SmcFnOpenVgaDecode End.\n"));
#endif
}

VOID
SmcFnCloseVgaDecode(VOID)
{
#if EarlyVideo_SUPPORT
//    DEBUG((-1, "SmcFnCloseVgaDecode Start.\n"));

    if(!VgaDecodeEnable)
        return;

    VgaDecodeEnable = FALSE;

    //
    // Disable I/O & Mem space on PCI Command register on both VGA Controller and PCI Bridge
    //
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x10), 0);
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x14), 0);
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x18), 0);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x3C), 0);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x04), 0);

    // Config Aspeed P2P
    PciWrite16(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x1c), 0);
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x20), 0);
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x24), 0);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x0c), 0);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x3c), 0);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x3e), 0);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x04), 0);

    // Config Intel P2P
    PciWrite16(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x1c), 0);
    PciWrite32(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x20), 0);
    PciWrite32(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x24), 0);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x0c), 0);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x3c), 0);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x3e), 0);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x04), 0);

//    DEBUG((-1, "SmcFnCloseVgaDecode End.\n"));
#endif
}

VOID
SmcFnEnableCursor(
    IN	BOOLEAN	Enabled
)
{
#if EarlyVideo_SUPPORT
    UINT8	Value = 0;

    DEBUG((-1, "SmcFnEnableCursor Start.\n"));
    if(!mDriversConnected){
	IoWrite8(0x3d4, 0x0a);
	Value = IoRead8(0x3d5);
	if(Enabled)
	    Value &= (~0x20);
	else
	    Value |= 0x20;
	IoWrite8(0x3d4, 0x0a);
	IoWrite8(0x3d5, Value);
    }
    DEBUG((-1, "SmcFnEnableCursor End.\n"));
#endif
}

VOID
SmcFnWriteChar(
    IN	UINT8	Xaxis,
    IN	UINT8	Yaxis,
    IN	UINT8	Attrib,
    IN	UINT8	Char
)
{
#if EarlyVideo_SUPPORT
#if EarlyVideo_Mode
    UINT8 *FontEntry, HeightOffset;
    UINT32 PixelPerByte, ScreenPicth, VgaBar;
    UINT32 WriteData;
    UINT32 NextLineAddr, StartAddr, DestAddr;
    UINT8 i, j, k, l, Temp;
    _FontInfo FontInfo;


    if(mDriversConnected)
        return;

    PixelPerByte = (SMC_bpp+1)/8;
    ScreenPicth  = SMC_xres*PixelPerByte;

    //Init Font Table
    FontInfo.FontTable  = &FONT8X19[0][0];
    FontInfo.FontWidth  = 8;
    FontInfo.FontHeight = 19;
    FontInfo.XScale     = 1;
    FontInfo.YScale     = 1;
    FontInfo.FG         = (ColorArray[(Attrib>>4)].Reserved<<24) + (ColorArray[(Attrib>>4)].Red<<16) + (ColorArray[(Attrib>>4)].Green<<8) + ColorArray[(Attrib>>4)].Blue;
    FontInfo.BG         = (ColorArray[(Attrib&0x0F)].Reserved<<24) + (ColorArray[(Attrib&0x0F)].Red<<16) + (ColorArray[(Attrib&0x0F)].Green<<8) + ColorArray[(Attrib&0x0F)].Blue;
    FontEntry = FontInfo.FontTable + ((FontInfo.FontWidth+7)/8)*FontInfo.FontHeight*Char;

    VgaBar = PcdGet32(PcdSmcVgaBar);
    HeightOffset = SMC_yres-(19*SMC_MAX_ROW);
    StartAddr = Xaxis*FontInfo.FontWidth*PixelPerByte+Yaxis*ScreenPicth*FontInfo.FontHeight+HeightOffset*ScreenPicth;
    for (i=0; i<FontInfo.FontHeight; i++) {
        NextLineAddr = StartAddr + ScreenPicth*FontInfo.YScale*i;
        for (j=0; j<FontInfo.YScale; j++) {
            DestAddr = NextLineAddr + ScreenPicth*j;
            Temp = *(UINT8 *)(FontEntry + i);
            for (k=0; k<FontInfo.FontWidth; k++) {
                if (Temp & 0x80)
                    WriteData = FontInfo.FG;
                else
                    WriteData = FontInfo.BG;
                Temp <<= 1;
                for (l=0; l<FontInfo.XScale; l++) {
                    if      (PixelPerByte == 1) *(UINT8 *)(VgaBar+DestAddr)  = (UINT8) WriteData & 0x0F;
                    else if (PixelPerByte == 2) *(UINT16 *)(VgaBar+DestAddr) = (UINT16) WriteData;
                    else                        *(UINT32 *)(VgaBar+DestAddr)  = WriteData;
                    DestAddr += PixelPerByte;
                }
            }
        }
    }
#else
    UINT16	*buffer = (UINT16*)0xB8000 + (Yaxis * SMC_MAX_COL) + Xaxis;

    //DEBUG((-1, "SmcFnWriteChar Start.\n")); // Reduce boot time
    if(!mDriversConnected)
	*buffer = ((Attrib << 8) | Char);
    //DEBUG((-1, "SmcFnWriteChar End.\n")); // Reduce boot time
#endif
#endif
}

VOID
SmcFnClearLine(
    IN	UINT8	Yaxis
)
{
#if EarlyVideo_SUPPORT
    UINT8	i = 0;

    DEBUG((-1, "SmcFnClearLine Start.\n"));
    if(!mDriversConnected){
	for( ; i < (SMC_MAX_COL - 20); i++)
	    SmcFnWriteChar(i, Yaxis, 0x07, ' ');
    }
    DEBUG((-1, "SmcFnClearLine End.\n"));
#endif
}

VOID
SmcFnClearScreen(VOID)
{
#if EarlyVideo_SUPPORT
#if EarlyVideo_Mode
    UINT32 i, ulScreenOffset, ulPitch, ulSizeX, ulSizeY, VgaBar;
    INT32 j;
    union {
        UINT32   ul;
        UINT16  us;
        UINT8   b[4];
    } ulData32;
    UINT8 R, G, B;


    DEBUG((-1, "SmcFnClearScreen Start.\n"));
    if(mDriversConnected)
        return;

    if((SMC_bpp != 16) && (SMC_bpp != 32))
        return;

    ulScreenOffset = 0;
    ulPitch = SMC_xres * ((SMC_bpp+1)/8);
    ulSizeX = SMC_xres;
    ulSizeY = SMC_yres;
    R = G = B = 0x00;

    VgaBar = PcdGet32(PcdSmcVgaBar);
    for(j=(INT32)(ulSizeY-1); j>=0; j--) {
        for(i=0; i<ulSizeX; i++) {
            if(SMC_bpp==16) {
                ulData32.us = (((UINT16) (R >> 3) << 11) | ((UINT16) (G >> 2) << 5) | ((UINT16) (B >> 3)) );
                WriteDW(VgaBar, ulScreenOffset+i*2, ulData32.us);
            }
            else {
                ulData32.b[0] = B;
                ulData32.b[1] = G;
                ulData32.b[2] = R;
                ulData32.b[3] = 0;
                WriteDD(VgaBar, ulScreenOffset+i*4, ulData32.ul);
            }
        }
        ulScreenOffset += ulPitch;
    }
    DEBUG((-1, "SmcFnClearScreen End.\n"));
#else
    UINT16	CharAttr = 0x0720, i = 0;
    UINT16	*VideoBuffer = (UINT16*)0xB8000;

    DEBUG((-1, "SmcFnClearScreen Start.\n"));
    if(!mDriversConnected){
	for(i = 0 ; i < 2000 ; i++)
	    *(VideoBuffer + i) = CharAttr;
    }
    DEBUG((-1, "SmcFnClearScreen End.\n"));
#endif
#endif
}

UINT8
SmcFnOutputString(
    IN	UINT8	Xaxis,
    IN	UINT8	Yaxis,
    IN	UINT8	Attrib,
    IN	CHAR8	*OutString
)
{
    UINT8	i = 0;
#if EarlyVideo_SUPPORT
    //DEBUG((-1, "SmcFnOutputString Start.\n")); // Reduce boot time
    if(!mDriversConnected){
	for( ; OutString[i] != '\0'; i++)
	    SmcFnWriteChar(Xaxis+i, Yaxis, Attrib, OutString[i]);
    }
#endif
    //DEBUG((-1, "SmcFnOutputString End.\n")); // Reduce boot time
    return i;
}

VOID
SmcFnPostProgressMessage(
    IN	UINT8	Yaxis,
    IN	CHAR8	*OutString
)
{
#if EarlyVideo_SUPPORT
    UINT8	i = 0;

    DEBUG((-1, "SmcFnPostProgressMessage Start.\n"));
    if(!mDriversConnected){
	if(*((UINT16*)(UINTN)0xB8000) == 0xFFFF)
	    SmcFnOpenVgaDecode();  // Init once and go
	SmcFnClearLine(Yaxis);
#if EarlyVideo_Mode
	i = SmcFnOutputString(0, Yaxis, Attr, OutString);
#else
	i = SmcFnOutputString(0, Yaxis, 0x0F, OutString);
#endif
	SmcFnWriteChar(i, Yaxis, 0x8F, '.');
    }
    DEBUG((-1, "SmcFnPostProgressMessage End.\n"));
#endif
}

VOID
InstallSmcSimpleTextOutProtocol(VOID)
{
    EFI_STATUS	Status;
    EFI_HANDLE	Handle = NULL;
  
    mSmcSimpleTextOutProtocol.OpenVgaDecode       = SmcFnOpenVgaDecode;
    mSmcSimpleTextOutProtocol.CloseVgaDecode      = SmcFnCloseVgaDecode;
    mSmcSimpleTextOutProtocol.EnableCursor        = SmcFnEnableCursor;
    mSmcSimpleTextOutProtocol.ClearLine           = SmcFnClearLine;
    mSmcSimpleTextOutProtocol.ClearScreen         = SmcFnClearScreen;
    mSmcSimpleTextOutProtocol.WriteChar           = SmcFnWriteChar;
    mSmcSimpleTextOutProtocol.OutputString        = SmcFnOutputString;
    mSmcSimpleTextOutProtocol.PostProgressMessage = SmcFnPostProgressMessage;
  
    Status = pBS->InstallProtocolInterface(
                  &Handle,
                  &gSmcSimpleTextOutProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSmcSimpleTextOutProtocol);
    
    ASSERT_EFI_ERROR(Status);
}

STATIC UINT8
SmcPostMsgHotKeyDXEFindString(
    IN	EFI_STATUS_CODE_VALUE	Value, 
    IN OUT	CHAR16	**str16
)
{
    UINT32	i = 0;
    UINT8	iSize = 0;
    CHAR16	*pStr;
	
    while(SmcPostMsgDXEStringTable[i].Value != 0){
	if(SmcPostMsgDXEStringTable[i].Value == Value){
	    pStr = *str16 = SmcPostMsgDXEStringTable[i].str16;
	    while(pStr[iSize] != 0)
		iSize++;
	    return iSize;
	}
	i++;
    }
    return 0;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name:        CheckQuietBoot
//
// Description: This function is Check the "QuietBoot" mode is enable or
//              disable.
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
STATIC UINT8
CheckQuietBoot(VOID)
{
    EFI_STATUS  Status;
    EFI_GUID    AmiTseSetupGuid = AMITSESETUP_GUID;
    AMITSESETUP AmiTseData;
    UINTN       VariableSize = sizeof(AMITSESETUP);
    UINT8       QuietBoot = 0;

    Status = pRS->GetVariable(
		    L"AMITSESetup",
		    &AmiTseSetupGuid,
	            NULL,
	            &VariableSize,
	            &AmiTseData);

    if(!EFI_ERROR(Status))
	QuietBoot = AmiTseData.AMISilentBoot;

    return (QuietBoot);
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name:        ClearBottomLine
//
// Description: This function is Clear a Bottom Line, avoid previous screen
//              text residual, and return Y coordinates.
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
STATIC VOID
ClearBottomLine (UINTN *Yres)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    EFI_GUID	gEfiGraphicsOutputProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL	*pGOP = NULL;
    EFI_UGA_PIXEL	UgaBlt={0xFF, 0xFF, 0xFF, 0};
    UINTN	y, w;

    if(pGOP == NULL){
	Status = pBS->LocateProtocol(
			&gEfiGraphicsOutputProtocolGuid,
			NULL,
			(void**)&pGOP);

	if(EFI_ERROR(Status)){
	    pGOP = NULL;
	    return;
	}
    }
#if EarlyVideo_Mode
    // Use background color to clear bottom line
    UgaBlt.Red = ColorArray[(AttrBds&0x0F)].Red;
    UgaBlt.Green = ColorArray[(AttrBds&0x0F)].Green;
    UgaBlt.Blue = ColorArray[(AttrBds&0x0F)].Blue;
    UgaBlt.Reserved = ColorArray[(AttrBds&0x0F)].Reserved;
#endif
    y = (pGOP->Mode->Info->VerticalResolution) - (EFI_GLYPH_HEIGHT* 2);
    w = (pGOP->Mode->Info->HorizontalResolution) - (EFI_GLYPH_WIDTH * 5);
    pGOP->Blt (pGOP, &UgaBlt, EfiBltVideoFill, 0, 0, 0, y, w, EFI_GLYPH_HEIGHT, 0);
    *Yres = y;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name:        _PrintPostBootMessage
//
// Description: This function is print a message on the screen bottom.
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
STATIC VOID
_PrintPostBootMessage(
    IN	CHAR16	*PostMessage,
    IN	BOOLEAN	bShowString
)
{
    EFI_STATUS	PostManagerStatus;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL	Foreground = {0, 0, 0, 0};
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL	Background = {0xFF, 0xFF, 0xFF, 0};
    AMI_POST_MANAGER_PROTOCOL	*PostManager = NULL;
    CHAR16	SpaceStrings[]= L"                                           ";
    EFI_GUID	mAmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
    UINTN	Yres =0;


    PostManagerStatus = pBS->LocateProtocol(
		    &mAmiPostManagerProtocolGuid,
		    NULL,
		    &PostManager);
#if EarlyVideo_Mode
    // Get Color for Foreground and Background
    Foreground = ColorArray[(AttrBds>>4)];
    Background = ColorArray[(AttrBds&0x0F)];
#endif

    if(!EFI_ERROR(PostManagerStatus)){
	if(CheckQuietBoot() == 1){
	    ClearBottomLine(&Yres);
	    PostManager->DisplayQuietBootMessage(PostMessage, 0, Yres,
				CA_AttributeCustomised, Foreground, Background);
	}
	else{
	    PostManager->SetCurPos (0, 23);
	    PostManager->DisplayPostMessage (SpaceStrings);
	    PostManager->SetCurPos (0, 24);
	    PostManager->DisplayPostMessage (SpaceStrings);			
	    if(bShowString){
		PostManager->SetCurPos (0, 23);
		PostManager->DisplayPostMessage (PostMessage);
	    }
	}
    }
    return;
}

EFI_STATUS
SmcPostMsgHotKeyDXEReportWorker(
    IN	EFI_STATUS_CODE_TYPE	CodeType,
    IN	EFI_STATUS_CODE_VALUE	Value,
    IN	UINT32			Instance,
    IN	EFI_GUID		*CallerId,
    IN	EFI_STATUS_CODE_DATA	*Data OPTIONAL
)
{
    EFI_STATUS	Status=EFI_SUCCESS;
    UINT8	dot;
#if EarlyVideo_SUPPORT
    UINT8	n;
    CHAR8	ASCIIString[80];
#endif
    CHAR16*	str16;
    CHAR8*	strBlank = " ";
    UINTN	Yres = 0;	// Add ClearBottomLine for always show "go to SETUP" string
    EFI_GUID    mAmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
    AMI_POST_MANAGER_PROTOCOL   *PostManager = NULL;
    UINT8   Data8 = 0;
    EFI_GUID    gEfiGraphicsOutputProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL    *pGOP = NULL;


    // Determine if Progress Code
    if(CodeType != EFI_PROGRESS_CODE)
        return Status;

    // Determine if Interesting Progress Code
    dot = SmcPostMsgHotKeyDXEFindString(Value, &str16);
    if(dot==0)
        return Status;

    // Determine if GraphicOutputProtocol installed
    if(!mDriversConnected) {
        Status = pBS->LocateProtocol(&mAmiPostManagerProtocolGuid, NULL, &PostManager);
        if(!EFI_ERROR(Status)) {
            Status = pBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (void**)&pGOP);
            if(!EFI_ERROR(Status))
                mDriversConnected = TRUE;
        }
    }

    if(!mDriversConnected) {
#if EarlyVideo_SUPPORT
        // Open VGA Decode
        SmcFnOpenVgaDecode();

        if(!VgaDecodeEnable)
            return Status;
        
        // Print String
#if EarlyVideo_Mode
        for(n = 0; n < 50; n++)
            SmcFnOutputString(n, SMC_MAX_ROW-2, Attr, strBlank);
        UnicodeStrToAsciiStr(str16, ASCIIString);
        SmcFnOutputString(00, SMC_MAX_ROW-2, Attr, ASCIIString);
        SmcFnOutputString(dot, SMC_MAX_ROW-2, Attr, ".");
#else
		for(n = 0; n < 50; n++)
			SmcFnOutputString(n, 24, 0x07, strBlank);
		UnicodeStrToAsciiStr(str16, ASCIIString);
		SmcFnOutputString(00, 24, 0x0F, ASCIIString);
		SmcFnOutputString(dot, 24, 0x8F, ".");
#endif

        // Close VGA Decode
        SmcFnCloseVgaDecode();
#endif
    }
    else {
        _PrintPostBootMessage(str16, TRUE);
        // if enable serial console redirection and the screen resolution is 80*24 and go into SETUP MENU
        // we need clear the string
        if(DXE_SETUP_START == Value) {
            _PrintPostBootMessage(str16, FALSE);
            ClearBottomLine(&Yres); // Add ClearBottomLine for always show "go to SETUP" string
        }
    }

    return Status;
}

VOID
UnregisterReportHandlers (
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    if(mRscHandlerProtocol)
        mRscHandlerProtocol->Unregister(SmcPostMsgHotKeyDXEReportWorker);
}

EFI_STATUS
SmcPostMsgHotKey_DXE_Init(
    IN	EFI_HANDLE	ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    EFI_GUID	mSmcPostPrintHobGuid = EFI_SMCPOSTPRINT_HOB_GUID;
    SMCPOSTPRINT_HOB_INFO	*pSmcPostPrintHob;
    UINT8	DisplayPostMessages = 1;
    EFI_EVENT ExitBootServicesEvent = NULL;

    InitAmiLib(ImageHandle, SystemTable);

    DEBUG((-1, "SmcPostMsgHotKey_Dxe_Init Start\n"));

    mDriversConnected = FALSE;

#if EarlyVideo_Mode
    // Get Color Attribute
    Attr = PcdGet8(PcdSmcColor);
    // Get Color Attribute for BDS Stage
    AttrBds = PcdGet8(PcdSmcBdsColor);
#endif
  
    InstallSmcSimpleTextOutProtocol();
	
    pSmcPostPrintHob = (SMCPOSTPRINT_HOB_INFO*)GetFirstGuidHob(&mSmcPostPrintHobGuid);
    if(pSmcPostPrintHob != NULL){
	DEBUG((-1, "pSmcPostPrintHob->SmcBootProcedureMsg = %02x\n", pSmcPostPrintHob->SmcBootProcedureMsg));
	if(!pSmcPostPrintHob->SmcBootProcedureMsg)	DisplayPostMessages = 0;
    }

    if(DisplayPostMessages){
        Status = pBS->LocateProtocol(
                &gEfiRscHandlerProtocolGuid, 
                NULL, 
                (VOID**)&mRscHandlerProtocol);
        if(!EFI_ERROR(Status)){
            Status = mRscHandlerProtocol->Register(SmcPostMsgHotKeyDXEReportWorker, TPL_HIGH_LEVEL);
            ASSERT_EFI_ERROR(Status);
        }

        Status = pBS->CreateEventEx(
                EVT_NOTIFY_SIGNAL,
                TPL_NOTIFY,
                UnregisterReportHandlers,
                NULL,
                &gEfiEventExitBootServicesGuid,
                &ExitBootServicesEvent);
    }

    DEBUG((-1, "SmcPostMsgHotKey_Dxe_Init End\n"));

    return Status;
}
