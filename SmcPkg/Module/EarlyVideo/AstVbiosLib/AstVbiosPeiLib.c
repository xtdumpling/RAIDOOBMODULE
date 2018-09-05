//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.12
//    Bug Fix:  To let can display earlyVideo message by on board video when set Offboar
//    Reason:   
//    Auditor:  Chen Lin
//    Date:     Jul/21/2017
//
//
//  Rev. 1.11
//    Bug Fix:  Fix EarlyGraphic malfunction when modify MMCFG Base sometimes.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     May/19/2017
//
//  Rev. 1.10
//    Bug Fix:  Fix POST hang up when set MMCFG Base to 3G.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     May/17/2017
//
//  Rev. 1.09
//    Bug Fix:  For EarlyGraphic, Change Font Style from 8x16 to 8x19.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Apr/07/2017
//
//  Rev. 1.08
//    Bug Fix:  Add a token EarlyVideoBootGuard_Support to support BootGuard
//              with EarlyVideo_Mode enabled.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Mar/28/2017
//
//  Rev. 1.07
//    Bug Fix:  Fix POST shutdown due to BootGuard verified failure.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Mar/27/2017
//
//  Rev. 1.06
//    Bug Fix:  Support Early Graphic Logo with TSE_ROMHOLE_SUPPORT enabled.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Mar/22/2017
//
//  Rev. 1.05
//    Bug Fix:  Early Graphic Logo Support.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Mar/14/2017
//
//  Rev. 1.04
//    Bug Fix:  Skip VGA RP DID detection.
//    Reason:   Different DID in QS and ES PCH
//    Auditor:  Kasber Chen
//    Date:     Dec/19/2016
//
//  Rev. 1.03
//    Bug Fix:  Disable onboard VGA output during POST when VGA Priority set to offboard.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Sep/12/2016
//
//  Rev. 1.02
//    Bug Fix:  Fixed the SMC SVID/SDID can't update to Ast2500 VGA devcie
//    Reason:
//    Auditor:  Jimmy Chiu
//    Date:     Sep/08/2016
//
//  Rev. 1.01
//    Bug Fix:  Skip SUC settings to avoid system hang in eSPI mode.
//    Reason:   Per vendor recommend.
//    Auditor:  Jacker Yeh
//    Date:     Mar/10/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/05/2014
//
//****************************************************************************
//****************************************************************************

#include <AstVbiosPeiLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>
#include <SetupVariable.h>
#include "SmcLib.h"
#include <devlib.h>
#include <vgahw.h>
#include <vgamode.h>
#include <Library\PciLib.h>
#if EarlyVideo_Mode
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Pi/PiFirmwareFile.h>
#include "font8x19.c"
#include "../LogoId/Logoid.h"
#include <Protocol/GraphicsOutput.h>
#endif

UINT8 AST_CONFIG_INDEX = AST2500_CONFIG_INDEX;
UINT8 AST_CONFIG_DATA = AST2500_CONFIG_DATA;

#if EarlyVideo_Mode

#define SOCKET_COMMONRC_CONFIGURATION_NAME L"SocketCommonRcConfig"
EFI_GUID gEfiSocketCommonRcVariableGuid = { 0x4402ca38, 0x808f, 0x4279, { 0xbc, 0xec, 0x5b, 0xaf, 0x8d, 0x59, 0x09, 0x2f } };

#pragma pack(1)
typedef struct {
    //
    //  Common Section of RC
    //
    UINT32   MmiohBase;
    UINT16   MmiohSize;
    UINT8    MmcfgBase;
    UINT8    MmcfgSize;
    UINT8    IsocEn;
    UINT8    NumaEn;
    UINT8    MirrorMode;
    UINT8    LockStep;
    UINT8    CpuStepping;
    UINT8    SystemRasType;
} SOCKET_COMMONRC_CONFIGURATION;
#pragma pack()

LOGO_DATA LogArray[BmpNum] = {
    {FV_BB_SMC_LOGO_FFS_FILE_1_GUID, 0, 16*0},
    {FV_BB_SMC_LOGO_FFS_FILE_2_GUID, 0, 16*1},
    {FV_BB_SMC_LOGO_FFS_FILE_3_GUID, 0, 16*2},
    {FV_BB_SMC_LOGO_FFS_FILE_4_GUID, 0, 16*3},
    {FV_BB_SMC_LOGO_FFS_FILE_5_GUID, 0, 16*4},
    {FV_BB_SMC_LOGO_FFS_FILE_6_GUID, 0, 16*5},
    {FV_BB_SMC_LOGO_FFS_FILE_7_GUID, 0, 16*6},
    {FV_BB_SMC_LOGO_FFS_FILE_8_GUID, 0, 16*7},
    {FV_BB_SMC_LOGO_FFS_FILE_9_GUID, 0, 16*8},
    {FV_BB_SMC_LOGO_FFS_FILE_10_GUID, 0, 16*9},
    {FV_BB_SMC_LOGO_FFS_FILE_11_GUID, 0, 16*10},
    {FV_BB_SMC_LOGO_FFS_FILE_12_GUID, 0, 16*11},
    {FV_BB_SMC_LOGO_FFS_FILE_13_GUID, 0, 16*12},
    {FV_BB_SMC_LOGO_FFS_FILE_14_GUID, 0, 16*13},
    {FV_BB_SMC_LOGO_FFS_FILE_15_GUID, 0, 16*14},
    {FV_BB_SMC_LOGO_FFS_FILE_16_GUID, 0, 16*15},
    {FV_BB_SMC_LOGO_FFS_FILE_17_GUID, 0, 16*16},
    {FV_BB_SMC_LOGO_FFS_FILE_18_GUID, 0, 16*17},
    {FV_BB_SMC_LOGO_FFS_FILE_19_GUID, 0, 16*18},
    {FV_BB_SMC_LOGO_FFS_FILE_20_GUID, 0, 16*19},
    {FV_BB_SMC_LOGO_FFS_FILE_21_GUID, 0, 16*20},
    {FV_BB_SMC_LOGO_FFS_FILE_22_GUID, 0, 16*21},
    {FV_BB_SMC_LOGO_FFS_FILE_23_GUID, 0, 16*22},
    {FV_BB_SMC_LOGO_FFS_FILE_24_GUID, 0, 16*23},
    {FV_BB_SMC_LOGO_FFS_FILE_25_GUID, 0, 16*24},
    {FV_BB_SMC_LOGO_FFS_FILE_26_GUID, 0, 16*25},
    {FV_BB_SMC_LOGO_FFS_FILE_27_GUID, 0, 16*26},
    {FV_BB_SMC_LOGO_FFS_FILE_28_GUID, 0, 16*27},
    {FV_BB_SMC_LOGO_FFS_FILE_29_GUID, 0, 16*28},
    {FV_BB_SMC_LOGO_FFS_FILE_30_GUID, 0, 16*29},
    {FV_BB_SMC_LOGO_FFS_FILE_31_GUID, 0, 16*30},
    {FV_BB_SMC_LOGO_FFS_FILE_32_GUID, 0, 16*31},
    {FV_BB_SMC_LOGO_FFS_FILE_33_GUID, 0, 16*32},
    {FV_BB_SMC_LOGO_FFS_FILE_34_GUID, 0, 16*33},
    {FV_BB_SMC_LOGO_FFS_FILE_35_GUID, 0, 16*34},
    {FV_BB_SMC_LOGO_FFS_FILE_36_GUID, 0, 16*35},
    {FV_BB_SMC_LOGO_FFS_FILE_37_GUID, 0, 16*36},
    {FV_BB_SMC_LOGO_FFS_FILE_38_GUID, 0, 16*37},
    {FV_BB_SMC_LOGO_FFS_FILE_39_GUID, 0, 16*38},
    {FV_BB_SMC_LOGO_FFS_FILE_40_GUID, 0, 16*39},
    {FV_BB_SMC_LOGO_FFS_FILE_41_GUID, 0, 16*40},
    {FV_BB_SMC_LOGO_FFS_FILE_42_GUID, 0, 16*41},
    {FV_BB_SMC_LOGO_FFS_FILE_43_GUID, 0, 16*42},
    {FV_BB_SMC_LOGO_FFS_FILE_44_GUID, 0, 16*43},
    {FV_BB_SMC_LOGO_FFS_FILE_45_GUID, 0, 16*44},
    {FV_BB_SMC_LOGO_FFS_FILE_46_GUID, 0, 16*45},
    {FV_BB_SMC_LOGO_FFS_FILE_47_GUID, 0, 16*46},
    {FV_BB_SMC_LOGO_FFS_FILE_48_GUID, 0, 16*47}
};

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

extern LzmaGuidedSectionGetInfo();
extern LzmaGuidedSectionExtraction();
#endif //EarlyVideo_Mode


UINT32 lpc_read8 (
        IN      UINT32 ulAddress)
{

    UINT32     ultemp = 0;
    UINT8      jtemp, uldata;

//Write Address
    IoWrite8 (AST_CONFIG_INDEX, 0xf0);
    IoWrite8 (AST_CONFIG_DATA, ((ulAddress & 0xff000000) >> 24));
    IoWrite8 (AST_CONFIG_INDEX, 0xf1);
    IoWrite8 (AST_CONFIG_DATA, ((ulAddress & 0x00ff0000) >> 16));
    IoWrite8 (AST_CONFIG_INDEX, 0xf2);
    IoWrite8 (AST_CONFIG_DATA, ((ulAddress & 0x0000ff00) >> 8));
    IoWrite8 (AST_CONFIG_INDEX, 0xf3);
    IoWrite8 (AST_CONFIG_DATA, ulAddress & 0xff);
//Write Mode
    IoWrite8 (AST_CONFIG_INDEX, 0xf8);
    jtemp = IoRead8 (AST_CONFIG_DATA);
    IoWrite8 (AST_CONFIG_DATA, ((jtemp & 0xfc) | 0x02));
//Fire
    IoWrite8 (AST_CONFIG_INDEX, 0xfe);
    jtemp = IoRead8 (AST_CONFIG_DATA);
//Get Data
    IoWrite8 (AST_CONFIG_INDEX, 0xf4);
    uldata = IoRead8 (AST_CONFIG_DATA);
    ultemp |= (uldata << 24);
    IoWrite8 (AST_CONFIG_INDEX, 0xf5);
    uldata = IoRead8 (AST_CONFIG_DATA);
    ultemp |= (uldata << 16);
    IoWrite8 (AST_CONFIG_INDEX, 0xf6);
    uldata = IoRead8 (AST_CONFIG_DATA);
    ultemp |= (uldata << 8);
    IoWrite8 (AST_CONFIG_INDEX, 0xf7);
    uldata = IoRead8 (AST_CONFIG_DATA);
    ultemp |= uldata;
    return ultemp;;
}

VOID lpc_write8 (
        IN      UINT32  ulAddress,
        IN      UINT32  uldata)
{
    UINT8     jtemp;

//Write Address
    IoWrite8 (AST_CONFIG_INDEX, 0xf0);
    IoWrite8 (AST_CONFIG_DATA, ((ulAddress & 0xff000000) >> 24));
    IoWrite8 (AST_CONFIG_INDEX, 0xf1);
    IoWrite8 (AST_CONFIG_DATA, ((ulAddress & 0x00ff0000) >> 16));
    IoWrite8 (AST_CONFIG_INDEX, 0xf2);
    IoWrite8 (AST_CONFIG_DATA, ((ulAddress & 0x0000ff00) >> 8));
    IoWrite8 (AST_CONFIG_INDEX, 0xf3);
    IoWrite8 (AST_CONFIG_DATA, ulAddress & 0xff);
//Write Data
    IoWrite8 (AST_CONFIG_INDEX, 0xf4);
    IoWrite8 (AST_CONFIG_DATA, ((uldata & 0xff000000) >> 24));
    IoWrite8 (AST_CONFIG_INDEX, 0xf5);
    IoWrite8 (AST_CONFIG_DATA, ((uldata & 0x00ff0000) >> 16));
    IoWrite8 (AST_CONFIG_INDEX, 0xf6);
    IoWrite8 (AST_CONFIG_DATA, ((uldata & 0x0000ff00) >> 8));
    IoWrite8 (AST_CONFIG_INDEX, 0xf7);
    IoWrite8 (AST_CONFIG_DATA, uldata);
//Write Mode
    IoWrite8 (AST_CONFIG_INDEX, 0xf8);
    jtemp = IoRead8 (AST_CONFIG_DATA);
    IoWrite8 (AST_CONFIG_DATA, (jtemp & 0xfc) | 0x02);
//Fire
    IoWrite8 (AST_CONFIG_INDEX, 0xfe);
    IoWrite8 (AST_CONFIG_DATA, 0xcf);
}

VOID lpc_write32 (
        IN      UINT32  ulAddress,
        IN      UINT32  uldata)
{
    UINT8     jtemp;

//Write Address
    IoWrite8 (AST_CONFIG_INDEX, 0xf0);
    IoWrite8 (AST_CONFIG_DATA, ((ulAddress & 0xff000000) >> 24));
    IoWrite8 (AST_CONFIG_INDEX, 0xf1);
    IoWrite8 (AST_CONFIG_DATA, ((ulAddress & 0x00ff0000) >> 16));
    IoWrite8 (AST_CONFIG_INDEX, 0xf2);
    IoWrite8 (AST_CONFIG_DATA, ((ulAddress & 0x0000ff00) >> 8));
    IoWrite8 (AST_CONFIG_INDEX, 0xf3);
    IoWrite8 (AST_CONFIG_DATA, ulAddress & 0xff);
//Write Data
    IoWrite8 (AST_CONFIG_INDEX, 0xf4);
    IoWrite8 (AST_CONFIG_DATA, ((uldata & 0xff000000) >> 24));
    IoWrite8 (AST_CONFIG_INDEX, 0xf5);
    IoWrite8 (AST_CONFIG_DATA, ((uldata & 0x00ff0000) >> 16));
    IoWrite8 (AST_CONFIG_INDEX, 0xf6);
    IoWrite8 (AST_CONFIG_DATA, ((uldata & 0x0000ff00) >> 8));
    IoWrite8 (AST_CONFIG_INDEX, 0xf7);
    IoWrite8 (AST_CONFIG_DATA, uldata);
//Write Mode
    IoWrite8 (AST_CONFIG_INDEX, 0xf8);
    jtemp = IoRead8 (AST_CONFIG_DATA);
    IoWrite8 (AST_CONFIG_DATA, (jtemp & 0xfc) | 0x02);
//Fire
    IoWrite8 (AST_CONFIG_INDEX, 0xfe);
    IoWrite8 (AST_CONFIG_DATA, 0xcf);
}



VOID
ScanPciDeviceAndInitDevice (
  IN  IPD_t                  *IPDTable
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  UINT16        VenId = 0,  DevId = 0;
  UINT8         TableIndex = 0, i = 0;
  UINT8         Offset = 0, Value = 0, RegValue = 0, BeforeValue = 0, AfterValue = 0;
  UINT8         PciValue[3] = {0};
  UINT32	    RegValue32 = 0;
  UINT32		address = 0;

  while (1)
  {
     if ((IPDTable[TableIndex].PciBus == 0) && (IPDTable[TableIndex].PciDev == 0) &&
         (IPDTable[TableIndex].PciFun == 0) &&
         (IPDTable[TableIndex].VendorId == 0) &&
         (IPDTable[TableIndex].DeviceId == 0) &&
         (IPDTable[TableIndex].InitValueTable == NULL) )
     {
          break;
     }
     else
     {
         VenId = DevId = 0x00;
         VenId = PciRead16( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + 0x00 );
         DevId = PciRead16( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + 0x02 );
         
	     Offset = 0x08;
	     RegValue32 = PciRead32( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset );
	     PciValue[0] = (UINT8)(RegValue32 >> 8);   //0x09
	     PciValue[1] = (UINT8)(RegValue32 >> 16);  //0x0A
	     PciValue[2] = (UINT8)(RegValue32 >> 24);  //0x0B
	     if ((PciValue[0] == 0x00) && (PciValue[1] == 0x04) && (PciValue[2] == 0x06))
	     {
	         Offset = 0x18;
	         RegValue = PciRead8( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset );
	         if (!EFI_ERROR(Status) && RegValue != IPDTable[TableIndex].PciBus)
	         {
	             RegValue = IPDTable[TableIndex].PciBus;
	             PciWrite8( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset,  RegValue);
	             RegValue = PciRead8( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset );
	         }
	         PciValue[0] = RegValue;
					
	         Offset = 0x19;
	         RegValue = PciRead8( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset );
	         if (!EFI_ERROR(Status) && RegValue != (IPDTable[TableIndex+1].PciBus))
	         {
	             RegValue = (IPDTable[TableIndex+1].PciBus);   
	             PciWrite8( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset,  RegValue);
	             RegValue = PciRead8( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset );
	         }
	         PciValue[1] = RegValue;
	         Offset = 0x1A;
	         RegValue = PciRead8( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset );
	         if (!EFI_ERROR(Status) && RegValue != MaxDeepPciBusNumber)
	         {
	             RegValue = MaxDeepPciBusNumber;
	             PciWrite8( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset,  RegValue);
	             RegValue = 0x00;
	             RegValue = PciRead8( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset );
	         }
	         PciValue[2] = RegValue;
	     }

	     i = 0;
	     while (1)
	     {
	         Offset = Value = 0x00;
	         Offset = IPDTable[TableIndex].InitValueTable[i].Offset;
	         Value  = IPDTable[TableIndex].InitValueTable[i].Value;
	         if ((Offset == 0x00) && (Value == 0x00))
	         {
	             //Check device is AST VGA controller		       
	             if ( DevId == 0x2000)
	             {
	                 RegValue = 0x00;
					//pasword unlock for SSID&SVID
	                 RegValue32 = 0xAA1A03AA;
	                 PciWrite32( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + 0x80,  RegValue32);
	                 //RegValue32 = CRB1_SSID;
	                 RegValue32 =SMC_SSID;
	                 PciWrite32( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + 0x2C,  RegValue32);
	                 //pasword lock for SSID&SVID
	                 RegValue32 = 0x00;
	                 PciWrite32( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + 0x80,  RegValue32);			
	             }
	             //Check device is AST P2P bridge
	             if(DevId == 0x1150)
	             {
	                 RegValue32 = PciRead32( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + 0x00 );
	             }
	             break;
	         }
                
	         RegValue = PciRead8( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset );
	         if (!EFI_ERROR(Status) )
	         {
	             BeforeValue = RegValue;
	             RegValue |= Value;
	             PciWrite8( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset,  RegValue);
	             RegValue = 0x00;
	             RegValue = PciRead8( ((UINT32)IPDTable[TableIndex].PciBus << 20) | ((UINT32)IPDTable[TableIndex].PciDev << 15) | ((UINT32)IPDTable[TableIndex].PciFun << 12) + Offset );
	             AfterValue = RegValue;
	         }
	         else {
	             BeforeValue = AfterValue = Value;
	         }
	         i++;
	     }
	     TableIndex++;
     }
  }

  // clear PCH PCI-E root port offse 0x18~0x1A value.
//  Offset = 0x18;  
//  RegValue = 0x00;
//  SMC_PciMemWrite (EfiPeiPciCfgWidthUint8,
//                IPDTable[0].PciBus, IPDTable[0].PciDev,
//                IPDTable[0].PciFun, Offset, (VOID*)&RegValue); 
//  
//  Offset = 0x19;  
//  RegValue = 0x00;
//  SMC_PciMemWrite (EfiPeiPciCfgWidthUint8,
//                IPDTable[0].PciBus, IPDTable[0].PciDev,
//                IPDTable[0].PciFun, Offset, (VOID*)&RegValue); 
//
//  Offset = 0x1A;  
//  RegValue = 0x00;
//  SMC_PciMemWrite (EfiPeiPciCfgWidthUint8,
//                IPDTable[0].PciBus, IPDTable[0].PciDev,
//                IPDTable[0].PciFun, Offset, (VOID*)&RegValue);   
//
}

#if EarlyVideo_Mode
void UpdateDecode()
{
    UINT32 VgaBar;


    switch(PcdGet64(PcdPciExpressBaseAddress)) {
        case 0x40000000: // 1G
            PcdSet32(PcdSmcVgaBar, VgaMmio_1G);
            VgaBar = VgaMmio_1G;
            break;
        case 0x60000000: // 1.5G
            PcdSet32(PcdSmcVgaBar, VgaMmio_15G);
            VgaBar = VgaMmio_15G;
            break;
        case 0x70000000: // 1.75G
            PcdSet32(PcdSmcVgaBar, VgaMmio_175G);
            VgaBar = VgaMmio_175G;
            break;
        case 0x80000000: // 2G
            PcdSet32(PcdSmcVgaBar, VgaMmio_2G);
            VgaBar = VgaMmio_2G;
            return;
            break;
        case 0x90000000: // 2.25G
            PcdSet32(PcdSmcVgaBar, VgaMmio_225G);
            VgaBar = VgaMmio_225G;
            break;
        case 0xC0000000: // 3G
            PcdSet32(PcdSmcVgaBar, VgaMmio_3G);
            VgaBar = VgaMmio_3G;
            break;
    }

    // Disable Decode
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

    // Enable Decode
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x19), Aspeed_P2P_BUS);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x1a), Aspeed_VGA_BUS);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x18), Aspeed_P2P_BUS);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x19), Aspeed_VGA_BUS);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x1a), Aspeed_VGA_BUS);

    // Config Intel P2P
    PciWrite16(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x1c), 0xD0D0);
    PciWrite32(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x20), (UINT32)((VgaBar+0x1000000)+(VgaBar>>16)));
    PciWrite32(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x24), 0x0001FFF1);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x0c), 0x10);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x3c), 0x05);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x3e), 0x18);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x04), 0x07);

    // Config Aspeed P2P
    PciWrite16(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x1c), 0xD1D1);
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x20), (UINT32)((VgaBar+0x1000000)+(VgaBar>>16)));
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x24), 0x0001FFF1);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x0c), 0x10);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x3c), 0x07);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x3e), 0x18);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x04), 0x07);

    // Config Aspeed VGA
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x10), VgaBar);
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x14), (VgaBar+0x1000000));
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x18), 0x0000D001);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x3C), 0x07);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x04), 0x07);
}

void EnableMMIO()
{
    // Set MMIO_RULE_CFG_0, refer to function ProgramCpuMmiolEntries
    PciWrite32(PCI_LIB_ADDRESS(LEGACY_BUS1, 29, 1, 0x40), 0x9D000049);
    PcdSet32(PcdSmcVgaBar, VgaMmio_2G);
}

EFI_STATUS GetIntegrityCheckFromLid(EFI_PEI_SERVICES **PeiServices, UINT16 *Chk, UINT8 *LidStatus)
{
    UINTN i, offset, FFSlength, StartAddr;
    UINT8       *tmp;
    EFI_FIRMWARE_VOLUME_HEADER  *FVHeader;
    EFI_FFS_FILE_HEADER   *FFSHeader;
    EFI_FREEFORM_SUBTYPE_GUID_SECTION *FreeformSection;
    LOGO_ID *Signature;
    EFI_GUID      gEfiFirmwareFileSystemGuid = EFI_FIRMWARE_FILE_SYSTEM2_GUID;
    EFI_GUID      FfsFileGuid = EFI_DEFAULT_LOGO_ID_GUID;
    EFI_GUID      SectionGuid = EFI_DEFAULT_LOGO_SECTION_GUID;

#if Skip_Integrity_Check_From_Lid
	return EFI_SUCCESS; 	    		
#endif

#if LOGO_Data_FV_Support
    StartAddr = FV_LOGO_BASE;
    tmp = (UINT8*)StartAddr;
    for( i=0; i<FV_LOGO_SIZE; i+=16) {
#else
    StartAddr = FV_BB_BASE;
    tmp = (UINT8*)StartAddr;
    for( i=0; i<FV_BB_SIZE; i+=16) {
#endif
        // Find FV
        if ( !MemCmp( (EFI_GUID*)(tmp+i), &gEfiFirmwareFileSystemGuid, sizeof(EFI_GUID)) ) {
            offset = ((UINTN)(& (((EFI_FIRMWARE_VOLUME_HEADER *) 0)->FileSystemGuid)));
            FVHeader = (void*)(tmp+i-offset);

            // Find First FFS
            FFSHeader = (EFI_FFS_FILE_HEADER *)((UINT8 *)FVHeader + FVHeader->HeaderLength);
            do {
                // Find FFS
                if ( !MemCmp( &(FFSHeader->Name), &FfsFileGuid, sizeof(EFI_GUID)) ) {
                    FreeformSection = (EFI_FREEFORM_SUBTYPE_GUID_SECTION *)(FFSHeader + 1);
                    if ( !MemCmp( &(FreeformSection->SubTypeGuid), &SectionGuid, sizeof(EFI_GUID)) ) {
                        Signature = (LOGO_ID*)(FreeformSection+1);
                        *Chk = Signature->IntegrityCheck;
                        *LidStatus = Signature->Status;
                        return EFI_SUCCESS;
                    }
                }

                // Find Next FFS
                FFSlength = (UINTN)(FFSHeader->Size[0])+(UINTN)(FFSHeader->Size[1] << 8)+(UINTN)(FFSHeader->Size[2] << 16);
                FFSlength = (FFSlength+aligment_FFS)&(~aligment_FFS);
                FFSlength &= 0xffffff;
                FFSHeader = (EFI_FFS_FILE_HEADER *)((UINT8 *)FFSHeader + FFSlength);
            } while ( (FFSlength!=0)&&(FFSlength!=0xffffff));
        }
    } // for( i=0; i<FV_MAIN_SIZE; i+=16)

    return EFI_NOT_FOUND;
}

EFI_STATUS GetIntegrityCheckFromBmpFfs(UINT16 *Chk)
{
    UINTN         i, offset, FFSlength, StartAddr;
    UINT8         *tmp;
    EFI_FIRMWARE_VOLUME_HEADER  *FVHeader;
    EFI_FFS_FILE_HEADER   *FFSHeader;
    EFI_GUID      gEfiFirmwareFileSystemGuid = EFI_FIRMWARE_FILE_SYSTEM2_GUID;
    EFI_GUID      FfsFileGuid = EFI_DEFAULT_BMP_LOGO_GUID;
#if TSE_ROMHOLE_SUPPORT
    EFI_GUID      AMIROMHOLEGuid = TSE_ROMHOLE_HEADER_GUID;
    UINTN         j;
#endif


    StartAddr = FV_MAIN_BASE;
    tmp = (UINT8*)StartAddr;
    for( i=0; i<FV_MAIN_SIZE; i+=16) {
        // Find FV
        if ( !MemCmp( (EFI_GUID*)(tmp+i), &gEfiFirmwareFileSystemGuid, sizeof(EFI_GUID)) ) {
            offset = ((UINTN)(& (((EFI_FIRMWARE_VOLUME_HEADER *) 0)->FileSystemGuid)));
            FVHeader = (void*)(tmp+i-offset);

            // Find First FFS
            FFSHeader = (EFI_FFS_FILE_HEADER *)((UINT8 *)FVHeader + FVHeader->HeaderLength);
            do {
#if TSE_ROMHOLE_SUPPORT
                // Find AMI TSE ROMHOLE FFS
                if ( !MemCmp( &(FFSHeader->Name), &AMIROMHOLEGuid, sizeof(EFI_GUID)) ) {
                    j = ((UINTN)(FFSHeader) & 0xFFFFFFF0) - FV_MAIN_BASE;

                    // Find FV inside AMI TSE ROMHOLE FFS
                    for(j; j<FV_MAIN_SIZE; j+=16) {
                        if ( !MemCmp( (EFI_GUID*)(tmp+j), &gEfiFirmwareFileSystemGuid, sizeof(EFI_GUID)) ) {
                            offset = ((UINTN)(& (((EFI_FIRMWARE_VOLUME_HEADER *) 0)->FileSystemGuid)));
                            FVHeader = (void*)(tmp+j-offset);

                            // Find BMP FFS
                            FFSHeader = (EFI_FFS_FILE_HEADER *)((UINT8 *)FVHeader + FVHeader->HeaderLength);
                            do {
                                if ( !MemCmp( &(FFSHeader->Name), &FfsFileGuid, sizeof(EFI_GUID)) ) {
                                    *Chk = FFSHeader->IntegrityCheck.Checksum16;
                                    return EFI_SUCCESS;
                                }
                                
                                // Find Next FFS
                                FFSlength = (UINTN)(FFSHeader->Size[0])+(UINTN)(FFSHeader->Size[1] << 8)+(UINTN)(FFSHeader->Size[2] << 16);
                                FFSlength = (FFSlength+aligment_FFS)&(~aligment_FFS);
                                FFSlength &= 0xffffff;
                                FFSHeader = (EFI_FFS_FILE_HEADER *)((UINT8 *)FFSHeader + FFSlength);
                            } while ( (FFSlength!=0)&&(FFSlength!=0xffffff));
                        }
                    }
                    return EFI_NOT_FOUND;
                }
#else
                // Find BMP FFS
                if ( !MemCmp( &(FFSHeader->Name), &FfsFileGuid, sizeof(EFI_GUID)) ) {
                    *Chk = FFSHeader->IntegrityCheck.Checksum16;
                    return EFI_SUCCESS;
                }
#endif // #if TSE_ROMHOLE_SUPPORT

                // Find Next FFS
                FFSlength = (UINTN)(FFSHeader->Size[0])+(UINTN)(FFSHeader->Size[1] << 8)+(UINTN)(FFSHeader->Size[2] << 16);
                FFSlength = (FFSlength+aligment_FFS)&(~aligment_FFS);
                FFSlength &= 0xffffff;
                FFSHeader = (EFI_FFS_FILE_HEADER *)((UINT8 *)FFSHeader + FFSlength);
            } while ( (FFSlength!=0)&&(FFSlength!=0xffffff));
        }
    } // for( i=0; i<FV_MAIN_SIZE; i+=16)

    return EFI_NOT_FOUND;
}

BOOLEAN CheckIfChangelogo(EFI_PEI_SERVICES **PeiServices)
{
    EFI_STATUS Status;
    UINT16 Chk1, Chk2;
    UINT8 LidStatus;


    Status = GetIntegrityCheckFromLid(PeiServices, &Chk1, &LidStatus);
    DEBUG((EFI_D_INFO, "[EarlyGraphic] GetIntegrityCheckFromLid --- %r Chk1:0x%x LidStatus:0x%x\n", Status, Chk1, LidStatus));
    if(EFI_ERROR(Status))
        return TRUE;

    // Check $LID Status, Show Logo if Status doesn't equals to 0xFF
    if(LidStatus!=0xFF)
        return FALSE;

    Status = GetIntegrityCheckFromBmpFfs(&Chk2);
    DEBUG((EFI_D_INFO, "[EarlyGraphic] GetIntegrityCheckFromBmpFfs --- %r Chk2:0x%x\n", Status, Chk2));
    if(EFI_ERROR(Status))
        return TRUE;

    if(Chk1!=Chk2)
        return TRUE;

    return FALSE;
}

BOOL ShowBitmap(UINT8 *LogoBuffer, UINT16 xPos, UINT16 yPos)
{
    BOOL Status = FALSE;
    UINT32 i, VgaBar;
    LONG  j;
    UINT8 R, G, B;
    union {
        UINT32   ul;
        UINT16  us;
        UINT8   b[4];
    } ulData32;

    UINT8 *pjBMPAddr;
    UINT32 ulBMPSizeX, ulBMPSizeY, ulBMPOffset, ulBMPPitch;
    UINT8 jColorDepth, jColorBytes;
    UINT32 ulScreenOffset = 0;
    UINT32 PixelPerByte = (SMC_bpp+1)/8;
    UINT32 ScreenPicth  = SMC_xres*PixelPerByte;

    if(LogoBuffer==NULL)
        return Status;

    pjBMPAddr = LogoBuffer;

    // Get BMP Info
    ulBMPSizeX = 1024;
    ulBMPSizeY = SMC_yres/BmpNum; // 768/48
    jColorDepth = 0x18;
    jColorBytes = jColorDepth /8;
    ulBMPOffset = 0;
    ulBMPPitch =  m04BytesAlignment(ulBMPSizeX * jColorDepth /8);
    ulScreenOffset = yPos*ScreenPicth + xPos*PixelPerByte;

    // Write BMP to Video Buffer
    if(jColorDepth!=24)
        return Status;

    if((SMC_bpp != 16) && (SMC_bpp != 32))
        return Status;

    VgaBar = PcdGet32(PcdSmcVgaBar);
    for (j=(LONG)(ulBMPSizeY-1); j>=0; j--) {
        for (i=0; i < ulBMPSizeX; i++) {
            B = ReadDB(pjBMPAddr+ulBMPOffset, ((UINT32)j*ulBMPPitch + i*jColorBytes));
            G = ReadDB(pjBMPAddr+ulBMPOffset, ((UINT32)j*ulBMPPitch + i*jColorBytes + 1));
            R = ReadDB(pjBMPAddr+ulBMPOffset, ((UINT32)j*ulBMPPitch + i*jColorBytes + 2));

            switch (SMC_bpp) {
                case 16:
                    ulData32.us = (((UINT16) (R >> 3) << 11) | ((UINT16) (G >> 2) << 5) | ((UINT16) (B >> 3)) );
                    WriteDW(VgaBar, ulScreenOffset+i*2, ulData32.us);
                    break;
                case 32:    
                    ulData32.b[0] = B;
                    ulData32.b[1] = G;
                    ulData32.b[2] = R;
                    ulData32.b[3] = 0;
                    WriteDD(VgaBar, ulScreenOffset+i*4, ulData32.ul);
                    break;                    
            }
        }
        ulScreenOffset += ScreenPicth;
    }

    return TRUE;
}

EFI_STATUS DrawSmciImage(EFI_PEI_SERVICES **PeiServices, void *ScratchBuffer, void *OutputBuffer, EFI_GUID FileGuid, UINT16 xPos, UINT16 yPos)
{
    EFI_STATUS      Status = EFI_NOT_FOUND;
    UINTN i, offset, FFSlength, StartAddr, SectionLength;
    UINT8       *tmp, *Section;
    EFI_FIRMWARE_VOLUME_HEADER  *FVHeader;
    EFI_FFS_FILE_HEADER   *FFSHeader;
    EFI_GUID      gEfiFirmwareFileSystemGuid = EFI_FIRMWARE_FILE_SYSTEM2_GUID;
    EFI_GUID      FfsFileGuid = FileGuid;
    EFI_COMMON_SECTION_HEADER *SectionHeader;
    UINT32                                  Authentication;
    UINT32      OutputBufferSize, ScratchBufferSize;
    UINT16      SectionAttribute;


#if LOGO_Data_FV_Support
    StartAddr = FV_LOGO_BASE;
    tmp = (UINT8*)StartAddr;
    for( i=0; i<FV_LOGO_SIZE; i+=16) {
#else
	StartAddr = FV_MAIN_BASE;
	tmp = (UINT8*)StartAddr;
	for( i=0; i<FV_MAIN_SIZE; i+=16) {
#endif
        // Find FV
        if ( !MemCmp( (EFI_GUID*)(tmp+i), &gEfiFirmwareFileSystemGuid, sizeof(EFI_GUID)) ) {
            offset = ((UINTN)(& (((EFI_FIRMWARE_VOLUME_HEADER *) 0)->FileSystemGuid)));
            FVHeader = (void*)(tmp+i-offset);

            // Find First FFS
            FFSHeader = (EFI_FFS_FILE_HEADER *)((UINT8 *)FVHeader + FVHeader->HeaderLength);
            do {
                // Find FFS
                if ( !MemCmp( &(FFSHeader->Name), &FfsFileGuid, sizeof(EFI_GUID)) ) {
                    // Find first Section
                    SectionHeader = (EFI_COMMON_SECTION_HEADER *)(FFSHeader + 1);
                    do {
                        if( SectionHeader->Type == EFI_SECTION_GUID_DEFINED ) {
                            // Extract Section and show bitmap
                            Status = LzmaGuidedSectionGetInfo(SectionHeader, &OutputBufferSize, &ScratchBufferSize, &SectionAttribute);
                            if(EFI_ERROR(Status))
                                return Status;

                            Authentication = 0;
                            Status = LzmaGuidedSectionExtraction(SectionHeader, &OutputBuffer, ScratchBuffer, &Authentication);
                            if(!EFI_ERROR(Status)) {
                                Section = (UINT8*)OutputBuffer;
                                Section += 4; // Discard Section Header
                                ShowBitmap(Section, xPos, yPos);
                            }
                            return Status;
                        }

                        // Find Next Section
                        SectionLength = (UINTN)(SectionHeader->Size[0])+(UINTN)(SectionHeader->Size[1] << 8)+(UINTN)(SectionHeader->Size[2] << 16);
                        SectionLength = (SectionLength+aligment_section)&(~aligment_section);
                        SectionHeader = (EFI_COMMON_SECTION_HEADER *)((UINT8 *)SectionHeader + SectionLength);
                    } while ( SectionLength!=0 && SectionHeader->Type!=0xFF );
                }

                // Find Next FFS
                FFSlength = (UINTN)(FFSHeader->Size[0])+(UINTN)(FFSHeader->Size[1] << 8)+(UINTN)(FFSHeader->Size[2] << 16);
                FFSlength = (FFSlength+aligment_FFS)&(~aligment_FFS);
                FFSlength &= 0xffffff;
                FFSHeader = (EFI_FFS_FILE_HEADER *)((UINT8 *)FFSHeader + FFSlength);
            } while ( (FFSlength!=0)&&(FFSlength!=0xffffff));
        }
    } // for( i=0; i<FV_MAIN_SIZE; i+=16)

    return Status;
}

VOID DrawInitImage(EFI_PEI_SERVICES **PeiServices)
{
    UINTN i;
    EFI_STATUS Status;
    void  *ScratchBuffer, *OutputBuffer;

    if(CheckIfChangelogo(PeiServices))
    	return;

    Status = (*PeiServices)->AllocatePool(PeiServices, SmcScratchBufferSize, &ScratchBuffer);
    DEBUG((EFI_D_INFO, "[EarlyGraphic] AllocatePool for ScratchBuffer with size:0x%x --- %r\n", SmcScratchBufferSize, Status));
    if(EFI_ERROR(Status))
        return;

    Status = (*PeiServices)->AllocatePool(PeiServices, SmcOutputBufferSize, &OutputBuffer);
    DEBUG((EFI_D_INFO, "[EarlyGraphic] AllocatePool for OutputBuffer with size:0x%x --- %r\n", SmcOutputBufferSize, Status));
    if(EFI_ERROR(Status))
        return;

    for(i=0; i<BmpNum; i++) {
        Status = DrawSmciImage(PeiServices, ScratchBuffer, OutputBuffer, LogArray[i].LogName, LogArray[i].xPos, LogArray[i].yPos);
        if(EFI_ERROR(Status))
            return;
    }
}
#endif //#if EarlyVideo_Mode

//////////////////////
// The PPI Services //
//////////////////////
EFI_STATUS
EFIAPI PPI_ClearScreen (VOID)
{
#if EarlyVideo_Mode
    UINT32 i, ulScreenOffset, ulPitch, ulSizeX, ulSizeY, VgaBar;
    LONG j;
    union {
        UINT32   ul;
        UINT16  us;
        UINT8   b[4];
    } ulData32;
    UINT8 R, G, B;


    if((SMC_bpp != 16) && (SMC_bpp != 32))
        return EFI_UNSUPPORTED;

    ulScreenOffset = 0;
    ulPitch = SMC_xres * ((SMC_bpp+1)/8);
    ulSizeX = SMC_xres;
    ulSizeY = SMC_yres;
    R = G = B = 0;

    VgaBar = PcdGet32(PcdSmcVgaBar);
    for(j=(LONG)(ulSizeY-1); j>=0; j--) {
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
#else
  UINT16   CharAttr = 0x0720, i = 0;
  UINT16  *VideoBuffer = (UINT16*)0xB8000;

  for (i = 0 ; i < 2000 ; i++)
       *(VideoBuffer + i) = CharAttr;
#endif
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI PPI_EnableCursor (
  IN  BOOLEAN            Cursor
  )
{
  UINT8  Value =0;

  IoWrite8 (0x3d4, 0x0a);
  Value = IoRead8 (0x3d5);
  switch (Cursor)
  {
    case TRUE:
         Value &= (~0x20);
         break;
    case FALSE:
         Value |= 0x20;
         break;
  }
  IoWrite8 (0x3d4, 0x0a);
  IoWrite8 (0x3d5, Value);
  return EFI_SUCCESS;
}

VOID PPI_WriteChar (
  IN UINT8      Xaxis,
  IN UINT8      Yaxis,
  IN UINT8      Attrib,
  IN UINT8      Char
  )
{
#if EarlyVideo_Mode
    UINT8 *FontEntry, HeightOffset;
    UINT32 PixelPerByte, ScreenPicth, VgaBar;
    UINT32 WriteData;
    UINT32 NextLineAddr, StartAddr, DestAddr;
    UINT8 i, j, k, l, Temp;
    _FontInfo FontInfo;


    // Re-Config decode after RC program MMCFG Rules related settings
    if( PcdGet64(PcdPciExpressBaseAddress)!=0x80000000 ) // Default 0x80000000 come from token PcdPciExpressBaseAddress, i.e. 2G
        UpdateDecode();

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
  UINT16 *buffer = (UINT16*)0xB8000 + (Yaxis * SMC_MAX_COL) + Xaxis;
 *buffer = ((Attrib<<8) | Char);
#endif
}

VOID PPI_ClearLine (
  IN  UINT8         Yaxis
  )
{
  UINT8 i = 0;
  UINT16 RegValue;

//  SMC_PciMemRead (EfiPeiPciCfgWidthUint16, Aspeed_VGA_BUS, 0, 0, 0, (VOID*)&RegValue);
  RegValue = PciRead16( ((UINT32)Aspeed_VGA_BUS << 20) | ((UINT32)0 << 15) | ((UINT32)0 << 12) + 0x00 );
  if (RegValue == 0xFFFF)
  {
     EnableASpeedDevice();
  }

  for (; i<(SMC_MAX_COL-20); i++)
  {
     PPI_WriteChar (i, Yaxis, 0x07, ' ');
  }
  return;
}

UINT8
EFIAPI PPI_OutputString (
  IN  UINT8               Xaxis,
  IN  UINT8               Yaxis,
  IN  UINT8               Attrib,
  IN  CHAR8              *OutString
  )
{
  UINT8  i = 0;
  UINT16 RegValue;

//  SMC_PciMemRead (EfiPeiPciCfgWidthUint16, Aspeed_VGA_BUS, 0, 0, 0, (VOID*)&RegValue);
  RegValue = PciRead16( ((UINT32)Aspeed_VGA_BUS << 20) | ((UINT32)0 << 15) | ((UINT32)0 << 12) + 0x00 );
  if (RegValue == 0xFFFF)
  {
  	EnableASpeedDevice();
  }

  for (; OutString[i] != '\0'; i++)
  {
     PPI_WriteChar (Xaxis+i, Yaxis, Attrib, OutString[i]);
  }
  return (i);
}

VOID
EFIAPI PPI_PostProgressMessage (
  IN  UINT8                     Yaxis,
  IN  CHAR8                    *OutString
  )
{
  UINT8  i = 0;
//  if ((UINT8)PcdGet8(SmcPostProgressFlag) & 0x01)
//  {
     PPI_ClearLine (Yaxis);
     i = PPI_OutputString (0, Yaxis, 0x0F, OutString);
     PPI_WriteChar (i, Yaxis, 0x8F, '.');
//  }
  return;
}
////////////////////////////
// PPI that are installed //
////////////////////////////
static EFI_PEI_SIMPLETEXTOUT_PPI EfiPeiSimpleTextOutPPI =
{
       PPI_ClearScreen,
       PPI_EnableCursor,
       PPI_WriteChar,
       PPI_ClearLine,
       PPI_OutputString,
       PPI_PostProgressMessage
};

static EFI_PEI_PPI_DESCRIPTOR EfiPeiSimpleTextOutPPiDesc[] =
{
       (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
       &gEfiPeiSimpleTextOutPPIGuid,
       &EfiPeiSimpleTextOutPPI
};

VOID
EnableASpeedDevice (VOID)
{
  ScanPciDeviceAndInitDevice (&AstVbiosPciDeviceInitTable[0]);
}



VOID VbiosInitEntry (
  IN  EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS  Status;
  _DEVInfo DevInfo;
#if EarlyVideo_Mode
  UCHAR *Mode = "1024x768x32";
#else
  UCHAR *Mode = "mode3+";
#endif
  UINT8 TEMP=0;
  UINT16 i=0, j=0;
  BOOL   DeviceStatus;
//  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
//  SYSTEM_CONFIGURATION    SystemConfiguration;
//  UINTN		VariableSize = sizeof(SYSTEM_CONFIGURATION);

#if EarlyVideo_Mode
  EnableMMIO();
#endif
  ScanPciDeviceAndInitDevice (&AstVbiosPciDeviceInitTable[0]);
  DeviceStatus = GetDevInfo(0x1A03, 0x2000, &DevInfo);
  if (!DeviceStatus) return;

  InitVGA_HW(&DevInfo, Init_MEM);

//  Status = (*PeiServices)->LocatePpi(
//  		PeiServices,
//  		&gEfiPeiReadOnlyVariable2PpiGuid,
//  		0,
//  		NULL,
//  		&ReadOnlyVariable);
//  if(!Status){
//    Status = ReadOnlyVariable->GetVariable(
//		    ReadOnlyVariable,
//		    L"IntelSetup",
//		    &gEfiSetupVariableGuid,
//		    NULL,
//		    &VariableSize,
//		    &SystemConfiguration);
////if VGA Priority set to offboard, disable onboard VGA output.
//    if(SystemConfiguration.VideoSelect == 2)
//	return;
//  }
//SMCDebug2();
  SetMode(&DevInfo, Mode, Init_MEM);
//  ASTVBIOSASMENTRY ();
  Status = (*PeiServices)->InstallPpi (PeiServices, EfiPeiSimpleTextOutPPiDesc);
  ASSERT_PEI_ERROR (PeiServices, Status);
}

