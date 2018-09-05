//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.04
//    Bug Fix:  Update NVMe hotplug settings.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/10/2016
//
//     Rev. 1.03
//       Bug Fix:   Fix sometimes IPMI isn't ready.
//       Reason:	
//       Auditor:   Hartmann Hsieh
//       Date:      Sep/01/2016
//
//     Rev. 1.02
//       Bug Fix:   Fix Onboard NVMe can't be detected.
//       Reason:	
//       Auditor:   Hartmann Hsieh
//       Date:      Jul/14/2016
//
//  Rev. 1.01
//      Bug Fixed:  Add PCIE bifurcation setting in PEI.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/06/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Jacker Yeh
//    Date:     Feb/04/2016
//
//****************************************************************************
//****************************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		ProjectPeiDriver.c
//
// Description:
//  This file implement function of PROJECTDRIVER_PROTOCOL
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include <token.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
#include <Library\GpioLib.h>
#include "X11DPUZPeiDriver.h"
#include "SmcCspLibBmc.h"

#if SmcRiserCard_SUPPORT
#include "../Module/SmcRiserCard/SmcRiserCardPei.h"
EFI_GUID gSmcRiserCardPeiProtocolGuid = EFI_SMCRISERCARD_PEI_DRIVER_PROTOCOL_GUID;
#endif
#if SmcAOC_SUPPORT
#include "../Module/SmcAOC/SmcAOCPei.h"
EFI_GUID gSmcAOCPeiProtocolGuid = EFI_SMC_AOC_PEI_PROTOCOL_GUID;
#endif

#include <Library/IoLib.h>
#include "KtiSetupDefinitions.h"
#include "X11DPUZPeiDriver.h"
#include "SspTokens.h"
#include "SmcLibCommon.h"
#include "SmcLibBmcPrivate.h"

#define KTI_LINK0 	0
#define KTI_LINK1	1
#define KTI_LINK2	2
#define ADAPTIVE_CTLE	0x3f

ALL_LANES_EPARAM_LINK_INFO  X11DPUZ_KtiNeonCityEPRPAllLanesEparamTable[] = {
  //
  // SocketID, Freq, Link, TXEQL, CTLEPEAK
  //
  //
  // Socket 0
  //
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2E39343F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2F39353F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2A34353F, ADAPTIVE_CTLE},
  //
  // Socket 1
  //
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2D37353F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2F3A343F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2A32373F, ADAPTIVE_CTLE}
};



/*-----------------12/23/2014 3:06PM----------------
 * Coding for every platform
 * --------------------------------------------------*/
//HSX_PER_LANE_EPARAM_LINK_INFO  X11DPUPerLanesEparamTable[] = {
//};
//UINT32 X11DPUPerLanesEparamTableSize = sizeof (X11DPUPerLanesEparamTable);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  PeiProjectCheckAdjustVID
//
// Description :  Send out the Project Adjust VID
//
// Parameters:  **PeiServices - Pointer to the PEI services table
//              PROJECT_PEI_DRIVER_PROTOCOL *This - Pointer to an instance of the SUPERM_PEI_DRIVER_PROTOCOL
//
// Returns     :  Project Adjust VID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT8 EFIAPI PeiProjectCheckAdjustVID(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PROJECT_PEI_DRIVER_PROTOCOL *This)
{
//    DEBUG ((DEBUG_INFO, "PeiProjectCheckAdjustVID.\n"));
    DEBUG((EFI_D_INFO, "PeiProjectCheckAdjustVID.\n"));

    return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  PeiProjectGetBID
//
// Description :  Send out the Project BID
//
// Parameters:  **PeiServices - Pointer to the PEI services table
//              PROJECT_PEI_DRIVER_PROTOCOL *This - Pointer to an instance of the SUPERM_PEI_DRIVER_PROTOCOL
//
// Returns     :  Project BID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT8 EFIAPI PeiProjectGetBID(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PROJECT_PEI_DRIVER_PROTOCOL *This)
{
//    DEBUG ((DEBUG_INFO, "PeiProjectGetBID.\n"));
    DEBUG((EFI_D_INFO, "PeiProjectGetBID.\n"));

    return 0x00;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  PeiProjectCheckBMCPresent
//
// Description :  Check BMC Present or not
//
// Parameters:  **PeiServices - Pointer to the PEI services table
//              PROJECT_PEI_DRIVER_PROTOCOL *This - Pointer to an instance of the SUPERM_PEI_DRIVER_PROTOCOL
//
// Returns     :  1: BMC Present  0: MBC not present
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT8 EFIAPI PeiProjectCheckBMCPresent(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PROJECT_PEI_DRIVER_PROTOCOL *This)
{
    UINT32	ReturnVal = 1;

//#if IPMI_SUPPORT
//	GpioSetPadConfig(BMC_Present_Pin, INIT_BMC_PRESENT_PIN);	// Initial GPIO before read
//	GpioGetInputValue(BMC_Present_Pin, &ReturnVal);
//#endif

    DEBUG ((EFI_D_INFO, "PeiProjectCheckBMCPresent Return = %x.\n", ReturnVal));

    return (UINT8)ReturnVal;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  PeiProjectSystemConfiguration
//
// Description :  Create Project own System Configurate Information
//
// Parameters:  **PeiServices - Pointer to the PEI services table
//              PROJECT_PEI_DRIVER_PROTOCOL *This - Pointer to an instance of the SUPERM_PEI_DRIVER_PROTOCOL
//				UINT8 *SystemConfigurationData - Pointer to SystemConfigurationData data structure address
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS PeiProjectSystemConfiguration(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PROJECT_PEI_DRIVER_PROTOCOL *This,
    OUT UINT8 *SystemConfigurationData
)
{
    UINT8 i;

//    DEBUG ((DEBUG_INFO, "PeiProjectSystemConfiguration.\n"));
    DEBUG((EFI_D_INFO, "PeiProjectSystemConfiguration.\n"));

    for( i = 0 ; i < 10 ; i ++ )
        *(SystemConfigurationData + i) = 0x00;

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  PeiProjectHardwareInfo
//
// Description :  Create Project own Hareware Information
//
// Parameters:  **PeiServices - Pointer to the PEI services table
//              PROJECT_PEI_DRIVER_PROTOCOL *This - Pointer to an instance of the SUPERM_PEI_DRIVER_PROTOCOL
//              UINT8 *HardWareInfoData - Pointer to HardWareInfoData data structure address
//              UINT8 *LanMacInfoData - Pointer to LanMacInfoData data structure address
//
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI PeiProjectHardwareInfo(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PROJECT_PEI_DRIVER_PROTOCOL *This,
    OUT UINT8 *HardWareInfoData,
    OUT UINT8 *LanMacInfoData)
{


//	PEI_TRACE((-1, PeiServices, "PeiProjectHardwareInfo.\n"));
    DEBUG((EFI_D_INFO, "PeiProjectHardwareInfo.\n"));


    // HardWareInfoData parameter data is used by 0x20 command
    // IPMI 0x20 command data:
    // Byte 0   : motherboard SSID  (bit[0:8])
    // Byte 1   : motherboard SSID  (bit[16:24])
    // Byte 2   : hardware monitor chip number
    // Byte 3   : NumLan M
    // Byte 4-9 : LAN port M+1 MAC address

    *(HardWareInfoData + 0) = (SMC_SSID >> 16) & 0xFF;
    *(HardWareInfoData + 1) = (SMC_SSID >> 24) & 0xFF;
    *(HardWareInfoData + 2) = 0x01;
    *(HardWareInfoData + 3) = 0x01;


    // LanMacInfoData parameter data is used by 0x9E command
    // IPMI 0x9E command data:
    // Byte 0    : LAN number N  (exclude NumLan of 0x30 0x20 command)
    // Byte 1-6  : LAN port M MAC address
    // Byte 7-12 : LAN port M+1 MAC address

    *LanMacInfoData = 0x01;

    return EFI_SUCCESS;
}

/*-----------------12/23/2014 3:06PM----------------
 * Coding for every platform
 * --------------------------------------------------*/
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  PeiProjectSystemConfiguration
//
// Description :  Create Project own System Configurate Information
//
// Parameters:  **PeiServices - Pointer to the PEI services table
//              PROJECT_PEI_DRIVER_PROTOCOL *This - Pointer to an instance of the SUPERM_PEI_DRIVER_PROTOCOL
//              OUT UINT32 *SizeOfTable - Size of HSX_ALL_LANES_EPARAM_LINK_INFO
//              OUT UINT32 *per_lane_SizeOfTable - Size of HSX_PER_LANE_EPARAM_LINK_INFO
//              OUT HSX_ALL_LANES_EPARAM_LINK_INFO **ptr - Pointer to HSX_ALL_LANES_EPARAM_LINK_INFO data structure address
//              OUT HSX_PER_LANE_EPARAM_LINK_INFO  **per_lane_ptr - Pointer to HSX_PER_LANE_EPARAM_LINK_INFO data structure address
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT32 EFIAPI GetProjectEParam(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PROJECT_PEI_DRIVER_PROTOCOL *This,
    OUT UINT32 *SizeOfTable,
    OUT ALL_LANES_EPARAM_LINK_INFO **Ptr)
{
	*Ptr = X11DPUZ_KtiNeonCityEPRPAllLanesEparamTable;
	*SizeOfTable =  (UINT32)(sizeof(X11DPUZ_KtiNeonCityEPRPAllLanesEparamTable)/sizeof(ALL_LANES_EPARAM_LINK_INFO));

    return TRUE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  GetProjectPchData
//
// Description :  Get PCH data function in board level
//  PLATFORM_PCH_DATA[10] as below
//  UINT8   PchPciePortCfg1;   // PCIE Port Configuration Strap 1
//  UINT8   PchPciePortCfg2;   // PCIE Port Configuration Strap 2
//  UINT8   PchPciePortCfg3;   // PCIE Port Configuration Strap 3
//  UINT8   PchPciePortCfg4;   // PCIE Port Configuration Strap 4
//  UINT8   PchPciePortCfg5;   // PCIE Port Configuration Strap 5
//  UINT8   PchPcieSBDE;
//  UINT8   LomLanSupported;   // Indicates if PCH LAN on board is supported or not
//  UINT8   GbePciePortNum;    // Indicates the PCIe port qhen PCH LAN on board is connnected.
// UINT8   GbeRegionInvalid;
// BOOLEAN GbeEnabled;        // Indicates if the GBE is SS disabled
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI GetProjectPchData(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PROJECT_PEI_DRIVER_PROTOCOL *This,
    IN OUT UINT8 PLATFORM_PCH_DATA[10])
{
    EFI_STATUS	Status = EFI_SUCCESS;
    DEBUG((EFI_D_INFO, "GetProjectPchData Start.\n"));
    PLATFORM_PCH_DATA[0] = 1;
    PLATFORM_PCH_DATA[1] = 1;
    PLATFORM_PCH_DATA[2] = 1;
    PLATFORM_PCH_DATA[3] = 1;
    PLATFORM_PCH_DATA[4] = 1;

    DEBUG((EFI_D_INFO, "GetProjectPchData End.\n"));
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI GetProjectPciEBifurcate(
    IN EFI_PEI_SERVICES **PeiServices,
    IN OUT	IIO_CONFIG	*SetupData
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8       DefaultIOU0[2], DefaultIOU1[2], DefaultIOU2[2];
    UINT8       SMB_Select = 0; //default from IPMI bus 7
    UINT64      OnboardNVMe;
#if SmcRiserCard_SUPPORT
    PEI_SMCRISERCARD_PROTOCOL	*mSmcRiserCardPeiProtocol = NULL;
    SMC_SLOT_INFO               SXB1_SLOT_INFO_Tbl[MAX_SLOT];
    SMC_SLOT_INFO               SXB2_SLOT_INFO_Tbl[MAX_SLOT];
    SMC_SLOT_INFO               SXB3_SLOT_INFO_Tbl[MAX_SLOT];
#endif
#if SmcAOC_SUPPORT
    PEI_SMC_AOC_PROTOCOL	*mSmcAOCPeiProtocol = NULL;
    UINT64      AOCLocationMask = 0;
#endif

    DEBUG((EFI_D_INFO, "PEI GetProjectPciEBifurcate Start.\n"));

    DefaultIOU0[0] = SetupData->ConfigIOU0[0];
    DefaultIOU1[0] = SetupData->ConfigIOU1[0];
    DefaultIOU2[0] = SetupData->ConfigIOU2[0];
    DefaultIOU0[1] = SetupData->ConfigIOU0[1];
    DefaultIOU1[1] = SetupData->ConfigIOU1[1];
    DefaultIOU2[1] = SetupData->ConfigIOU2[1];

    DEBUG((-1, "DefaultIOU = %x, %x, %x\n", DefaultIOU0[0], DefaultIOU1[0], DefaultIOU2[0]));
    DEBUG((-1, "DefaultIOU = %x, %x, %x\n", DefaultIOU0[1], DefaultIOU1[1], DefaultIOU2[1]));

    //
    // Onboard NVMe
    //
    SetupData->ConfigIOU0[0] = IIO_BIFURCATE_x4x4xxx8;
    SetupData->SLOTIMP[3] = 1;
    SetupData->SLOTIMP[4] = 1;

    SetupData->ConfigIOU0[1] = IIO_BIFURCATE_x4x4xxx8;
    SetupData->SLOTIMP[24] = 1;
    SetupData->SLOTIMP[25] = 1;

    OnboardNVMe = PcdGet64(PcdSmcAOCLocation);
    OnboardNVMe |= 0x0000005000000050;
    PcdSet64(PcdSmcAOCLocation, OnboardNVMe);

#if SmcRiserCard_SUPPORT
    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gSmcRiserCardPeiProtocolGuid,
                 0,
                 NULL,
                 &mSmcRiserCardPeiProtocol);

    mSmcRiserCardPeiProtocol->PeiSmcRiserCardPcieBifurcation(
        SetupData, DefaultIOU0, DefaultIOU1, DefaultIOU2);
    mSmcRiserCardPeiProtocol->PeiSmcRiserCardSlotBDFTable(
        SXB1_SLOT_INFO_Tbl, SXB2_SLOT_INFO_Tbl, SXB3_SLOT_INFO_Tbl);
    mSmcRiserCardPeiProtocol->PeiSmcRiserCardRedriver(PeiServices, 0x05, 0xe2, 0x06,
            PcdGet8(PcdSmcSXB3Index));
    mSmcRiserCardPeiProtocol->PeiSmcRiserCardRedriver(PeiServices, 0x05, 0xe2, 0x04,
            PcdGet8(PcdSmcSXB1Index));
    DEBUG((-1, "GetSuperMPcieBifurcation End.\n"));
#endif
#if SmcAOC_SUPPORT
    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gSmcAOCPeiProtocolGuid,
                 0,
                 NULL,
                 &mSmcAOCPeiProtocol);
    DEBUG((-1, "LocatePpi mSmcAOCPeiProtocol = %r.\n", Status));
    if(!Status) {
        mSmcAOCPeiProtocol->SmcAOCPeiCardDetection(PeiServices, 0x05, 0xe2, 0x04,
            SXB1_SLOT_INFO_Tbl, sizeof(SXB1_SLOT_INFO_Tbl)/sizeof(SMC_SLOT_INFO));//mux ch0
        mSmcAOCPeiProtocol->SmcAOCPeiCardDetection(PeiServices, 0x05, 0xe2, 0x05,
            SXB2_SLOT_INFO_Tbl, sizeof(SXB2_SLOT_INFO_Tbl)/sizeof(SMC_SLOT_INFO));//mux ch1
        mSmcAOCPeiProtocol->SmcAOCPeiCardDetection(PeiServices, 0x05, 0xe2, 0x06,
            SXB3_SLOT_INFO_Tbl, sizeof(SXB3_SLOT_INFO_Tbl)/sizeof(SMC_SLOT_INFO));//mux ch2

        OnboardNVMe = PcdGet64(PcdSmcAOCLocation);				// save Pcd to variable to save time
        if(OnboardNVMe == 0x0000005200550550)
        {
        	AOCLocationMask = 0x00000000000000f0;					// SYS-1029U-TN10RT: mask CPU1 onboard NVNe (1C, 1D)
        	SetupData->VMDEnabled[13] =	SetupData->VMDEnabled[12];	// On 10NVMe system, 6th NVMe and 7th NVMe both come from AOC with one menu in setup
        }
        else if(OnboardNVMe == 0x0000015000010050) AOCLocationMask = 0x0000000f0000000f0;// SYS-2029U-TN24R4T: mask CPU1/2 onboard NVNe (1C, 1D)
        else SMB_Select = 0;									// IPMI bus 7 from onboard NVMe header
        mSmcAOCPeiProtocol->SmcAOCPeiBifurcation(SetupData, DefaultIOU0, DefaultIOU1, DefaultIOU2);

	SMB_Select = 0;
        if(SMB_Select)
            mSmcAOCPeiProtocol->SmcAOCPeiSetNVMeMode(PeiServices, 0x05, 0xe2, 0x06, NULL, 0);
        else
            mSmcAOCPeiProtocol->SmcAOCPeiSetNVMeMode(PeiServices, 0x0d, 0, 0, NULL, 0);
    }
#endif
    if(SetupData->ConfigIOU0[0] == 0xff) {
        SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxxxxx16;
        SetupData->HidePEXPMenu[1] = 1;
    }
    if(SetupData->ConfigIOU1[0] == 0xff) {
        SetupData->ConfigIOU1[0] = IIO_BIFURCATE_xxxxxx16;
        SetupData->HidePEXPMenu[5] = 1;
    }
    if(SetupData->ConfigIOU2[0] == 0xff) {
        SetupData->ConfigIOU2[0] = IIO_BIFURCATE_xxxxxx16;
        SetupData->HidePEXPMenu[9] = 1;
    }
    if(SetupData->ConfigIOU0[1] == 0xff) {
        SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxxxxx16;
        SetupData->HidePEXPMenu[22] = 1;
    }
    if(SetupData->ConfigIOU1[1] == 0xff) {
        SetupData->ConfigIOU1[1] = IIO_BIFURCATE_xxxxxx16;
        SetupData->HidePEXPMenu[26] = 1;
    }
    if(SetupData->ConfigIOU2[1] == 0xff) {
        SetupData->ConfigIOU2[1] = IIO_BIFURCATE_xxxxxx16;
        SetupData->HidePEXPMenu[30] = 1;
    }
    if(SetupData->ConfigMCP0[0] == 0xff) {
        SetupData->ConfigMCP0[0] = IIO_BIFURCATE_xxxxxx16;
        SetupData->HidePEXPMenu[13] = 1;
    }
    if(SetupData->ConfigMCP1[0] == 0xff) {
        SetupData->ConfigMCP1[0] = IIO_BIFURCATE_xxxxxx16;
        SetupData->HidePEXPMenu[17] = 1;
    }
    if(SetupData->ConfigMCP0[1] == 0xff) {
        SetupData->ConfigMCP0[1] = IIO_BIFURCATE_xxxxxx16;
        SetupData->HidePEXPMenu[34] = 1;
    }
    if(SetupData->ConfigMCP1[1] == 0xff) {
        SetupData->ConfigMCP1[1] = IIO_BIFURCATE_xxxxxx16;
        SetupData->HidePEXPMenu[38] = 1;
    }

#if X11DPUZ_TEMP_DISABLE_VPP
{
	UINT32 i;
	for(i=0; i<MAX_SOCKET*NUMBER_PORTS_PER_SOCKET; i++) {
		SetupData->VppPort[i] = SetupData->VppAddress[i] = 0xff;
		SetupData->VppEnable[i] = 0;
	}
}
#endif


#ifdef EFI_DEBUG
{
/*
#define IIO_BIFURCATE_x4x4x4x4  0
#define IIO_BIFURCATE_x4x4xxx8  1
#define IIO_BIFURCATE_xxx8x4x4  2
#define IIO_BIFURCATE_xxx8xxx8  3
#define IIO_BIFURCATE_xxxxxx16  4
*/
#define GET_STR(AAA,BBB) (strBif[SetupData->ConfigIOU##BBB[AAA]])
#define GET_STR1(AAA,BBB) SetupData->ConfigIOU##BBB[AAA]

	char* strBif[5] = {"4444", "448 ", "844 ", "88  ", "16  "};
	DEBUG((-1, "CPU0_IOU0=%d, CPU0_IOU1=%d, CPU0_IOU2=%d, CPU1_IOU0=%d, CPU1_IOU1=%d, CPU1_IOU2=%d \n", 
		GET_STR1(0,0), 
		GET_STR1(0,1), 
		GET_STR1(0,2), 
		GET_STR1(1,0), 
		GET_STR1(1,1), 
		GET_STR1(1,2)
		));
	DEBUG((-1, "--------------------------------------\n"));
	DEBUG((-1, "     IOU0 IOU1 IOU2\n"));
	DEBUG((-1,"CPU0 %a %a %a\n", GET_STR(0,0), GET_STR(0,1), GET_STR(0,2)));
	DEBUG((-1, "--------------------------------------\n"));
	DEBUG((-1, "CPU1 %a %a %a\n", GET_STR(1,0), GET_STR(1,1), GET_STR(1,2)));
	DEBUG((-1, "--------------------------------------\n"));
}
#endif

    DEBUG((EFI_D_INFO, "GetProjectPciEBifurcate End.\n"));
    return EFI_SUCCESS;
}

PROJECT_PEI_DRIVER_PROTOCOL PeiProjectInterface =
{
    PeiProjectCheckAdjustVID,
    NULL,
    NULL,
    PeiProjectHardwareInfo,
    PeiProjectCheckBMCPresent,
    GetProjectEParam,
    GetProjectPchData,
    GetProjectPciEBifurcate,
    NULL,
    NULL
};

EFIAPI X11DPUZIPMICallback(
	IN EFI_PEI_SERVICES 		 **PeiServices,
	IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
	IN VOID 					 *Ppi)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	
	UINT8 pause_ipmi[] = {0xdf, 0x00};	// 	Pause BMC SDR Reading
	UINT8 pause_ipmi_return[32];
	UINT8 pause_ipmi_return_size = 32;

	Status = SMC_IPMICmd(0x30, 0x00, 0x70, pause_ipmi, 2, pause_ipmi_return, &pause_ipmi_return_size);
	DEBUG((-1, "X11DPUZIPMICallback Status=%r\n", Status));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : ProjectPeiDriverInit
//
// Description : Init Project at PEI phase
//
// Parameters:  FfsHeader   Pointer to the FFS file header
//              PeiServices Pointer to the PEI services table
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI X11DPUZPeiDriverInit(
    IN EFI_FFS_FILE_HEADER *FfsHeader,
    IN EFI_PEI_SERVICES **PeiServices
)
{
	EFI_PEI_PPI_DESCRIPTOR *mPpiList = NULL;
	EFI_STATUS Status;
	EFI_PEI_NOTIFY_DESCRIPTOR *pIPMICallback;
	UINT8 CMOS_SecurityFlag[4];

	DEBUG ((DEBUG_INFO, "ProjectPeiDriverInit.\n"));

	CMOS_SecurityFlag[0] = GetCmosTokensValue(Q_SMCSecurityJumer_S);
	CMOS_SecurityFlag[1] = GetCmosTokensValue(Q_SMCSecurityJumer_M);
	CMOS_SecurityFlag[2] = GetCmosTokensValue(Q_SMCSecurityJumer_C);
	CMOS_SecurityFlag[3] = GetCmosTokensValue(Q_SMCSecurityJumer_I);

	if(*((UINT32 *)CMOS_SecurityFlag) != (UINT32)'ICMS') {
		DEBUG ((DEBUG_INFO, "CMOS clear\n"));
		IoWrite8(0x70, ACPI_CENTURY_CMOS);
		IoWrite8(0x71, 0x20);
	}

	Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (EFI_PEI_PPI_DESCRIPTOR), &mPpiList);
	if ( !EFI_ERROR(Status) ) {
		mPpiList->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
		mPpiList->Guid = &gProjectOwnPeiProtocolGuid;
		mPpiList->Ppi = &PeiProjectInterface;

		Status = (**PeiServices).InstallPpi(PeiServices, mPpiList);
		if (EFI_ERROR(Status)) {
			DEBUG ((DEBUG_INFO, "ProjectPeiDriverInit fail.\n"));
		}
	}

	Status = (**PeiServices).AllocatePool(
			       PeiServices,
			       sizeof (EFI_PEI_NOTIFY_DESCRIPTOR),
			       &pIPMICallback);
	if ( !EFI_ERROR(Status) ) {
		pIPMICallback->Flags = EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
							| EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
		pIPMICallback->Guid   = &gEfiPeiIpmiTransportPpiGuid;
		pIPMICallback->Notify = X11DPUZIPMICallback;
		(*PeiServices)->NotifyPpi( PeiServices, pIPMICallback);
	}

	DEBUG ((DEBUG_INFO, "Exit ProjectPeiDriverInit.\n"));

	return EFI_SUCCESS;
}
