//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.06
//    Bug Fix:  Only enable PCH LAN port when detect correct SIOM card.
//    Reason:   
//    Auditor:  Jacker Yeh
//    Date:     Jun/19/2017
//
//  Rev. 1.05
//    Bug Fix:  a. Update Re-driver settings.
//              b. Fixed MTG-AOC-i4S and i2T cannot get MAC issue.
//    Reason:   
//    Auditor:  Jacker Yeh
//    Date:     Jan/17/2017
//
//  Rev. 1.04
//    Bug Fix:  Update TXEQ value from SI LAB.
//    Reason:   
//    Auditor:  Jacker Yeh
//    Date:     Jan/17/2017
//
//  Rev. 1.03
//    Bug Fix:  To avoid system hang 0xEE when install SKX-F CPU on system problem.
//    Reason:   Temp solution.
//    Auditor:  Jacker Yeh
//    Date:     Sep/27/2016
//
//  Rev. 1.02
//    Bug Fix:  Update default bifurcation settings in PEI stage to avoid system hang at PEI stage.
//    Reason:   Our SMC code skip default IIO settings in SystemIioPortBifurcationInit cause the problem.
//    Auditor:  Jacker Yeh
//    Date:     Jun/18/2016
//
//  Rev. 1.01
//    Bug Fix:  Add re-driver related code.
//    Reason:   
//    Auditor:  Jacker Yeh
//    Date:     Jun/14/2016
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
//#include <Efi.h>
//#include <Pei.h>
#include <token.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
//#include <Hob.h>
//#include <AmiPeiLib.h>
//#include <AmiCspLib.h>
//#include <ppi\ReadOnlyVariable2.h>
#include <Ppi/Smbus2.h>
#include "X11DPTPSPeiDriver.h"
#include "SmcCspLibBmc.h"
#include <GpioConfig.h>
#include <Library\GpioLib.h>
#include <GpioPinsSklH.h>

EFI_GUID gProjectOwnPeiProtocolGuid = EFI_PROJECT_OWN_PEI_DRIVER_PROTOCOL_GUID;
EFI_GUID gEfiPeiSmbus2PpiGuid = EFI_PEI_SMBUS2_PPI_GUID;

static GPIO_CONFIG INIT_BMC_PRESENT_PIN[] =
{
    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntDis, GpioResetPwrGood, GpioTermNone}
};	// Default setting for GPIO input

static GPIO_INIT_CONFIG INIT_LPC_PIN[] =
{
    {GPIO_SKL_H_GPP_A14, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone}},//GPP_A_14_ESPI_RESET_N
    {GPIO_SKL_H_GPP_A9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone}},//GPP_A_9_CLK_24M_66M_LPC0_ESPI
    {GPIO_SKL_H_GPP_A10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone}},//GPP_A_10_TP_PCH_GPP_A_10
    {GPIO_SKL_H_GPP_A16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone}},//GPP_A_16_TP_PCH_GPP_A_16
    {GPIO_SKL_H_GPP_G9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone}},//GPP_G_9_FAN_PWM_1_FAN_PWM1IE
    {GPIO_SKL_H_GPP_G10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone}},//GPP_G_10_FAN_PWM_2_FAN_PWM2IE
    {GPIO_SKL_H_GPP_G16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone}},//GPP_G_16_FM_BOARD_SKU_ID4 //PCH_VR_UPDATE
};
// PPI Definition(s)

// Function Definition(s)
EFI_STATUS PeiPericomInit(
	IN EFI_PEI_SERVICES             **PeiServices,
	IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
	IN VOID                         *NullPpi
);
// PPI that are installed

// PPI that are notified
static EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList[] = 
{
    {
        EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gEfiPeiSmbus2PpiGuid,
		PeiPericomInit
	}
};

#define EFI_DEADLOOP()    { volatile int __iii; __iii = 1; while (__iii); } 

#define PEI_ASSERT(PeiServices,Condition) if(!(Condition)) { \
	DEBUG((EFI_D_INFO, "ASSERT in %s on %i: %s\n",__FILE__, __LINE__));\
	EFI_DEADLOOP() \
	}
#define ASSERT_PEI_ERROR(PeiServices,Status)  PEI_ASSERT(PeiServices,!EFI_ERROR(Status))

//
#define SPEED_REC_96GT     0
#define SPEED_REC_104GT    1
#define MAX_KTI_LINK_SPEED 2
#define FREQ_PER_LINK      3
#define ADAPTIVE_CTLE 0x3f
#define PER_LANE_ADAPTIVE_CTLE 0X3f3f3f3f
#define KTI_LINK0 0
#define KTI_LINK1 1

//
// Link Speed Modes
//
#define KTI_LINK_SLOW_SPEED_MODE 0
#define KTI_LINK_FULL_SPEED_MODE 1


/*-----------------12/23/2014 3:06PM----------------
 * Coding for every platform
 * --------------------------------------------------*/
//HSX_PER_LANE_EPARAM_LINK_INFO  X11DPTPSPerLanesEparamTable[] = {
//};
//UINT32 X11DPTPSPerLanesEparamTableSize = sizeof (X11DPTPSPerLanesEparamTable);
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure   :  Check_BPN_GPI
// Description :  Collect GPI info from SXB1/SXB2 slots
// Parameters  :  NONE
// Returns     :  GPI values from SXB1 AND SXB2
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT8 Check_BPN_GPI(VOID)
{
//	GpioSetPadConfig(BMC_Present_Pin, INIT_BMC_PRESENT_PIN);	// Initial GPIO before read
//	GpioGetInputValue(BMC_Present_Pin, &ReturnVal);
	
//	GpioSetPadConfig(BMC_Present_Pin, INIT_BMC_PRESENT_PIN);	// Initial GPIO before read
//	GpioGetInputValue(BMC_Present_Pin, &ReturnVal);
	
//	GpioSetPadConfig(BMC_Present_Pin, INIT_BMC_PRESENT_PIN);	// Initial GPIO before read
//	GpioGetInputValue(BMC_Present_Pin, &ReturnVal);
	
	
	return 0xd1;//JKTEST//IoRead8(GPIO_BASE_ADDRESS + 0x48) & 0xf1;	//only GPIO64/68/69/70/71 work as RISER_ID
}

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

ALL_LANES_EPARAM_LINK_INFO  X11DPTPSAllLanesEparamTable[] = {
  //
  // SocketID, Freq, Link, TXEQL, CTLEPEAK
  //

  //
  // Socket 0
  //
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2A30393F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x303A353F, ADAPTIVE_CTLE},

  //
  // Socket 1
  //
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2D33393F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2D35373F, ADAPTIVE_CTLE}
};

UINT32 X11DPTPSAllLanesEparamTableSize = sizeof (X11DPTPSAllLanesEparamTable);

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
#if 0
VOID
GetDefaultKtiLaneEparam (
  IN OUT ALL_LANES_EPARAM_LINK_INFO           **PtrAllLanes,
  IN OUT UINT32                               *SizeOfAllLanesTable,
  )
{

  *SizeOfAllLanesTable = Kti8SAllLanesEparamTableSize;
  *PtrAllLanes = (ALL_LANES_EPARAM_LINK_INFO *) &Kti8SAllLanesEparamTable;
  *SizeOfPerLaneTable = 0;
  *PtrPerLane  = NULL;
#ifndef MINIBIOS_BUILD
  DEBUG((DEBUG_ERROR,"[UBA KTI] Platform kti lane eparam default config data is loaded! \n"));
#endif
}
#endif
UINT32 EFIAPI GetProjectEParam(
	IN EFI_PEI_SERVICES **PeiServices,
	IN PROJECT_PEI_DRIVER_PROTOCOL *This,
	OUT UINT32 *SizeOfTable,
	OUT ALL_LANES_EPARAM_LINK_INFO **Ptr)
{

  *SizeOfTable = X11DPTPSAllLanesEparamTableSize;
  *Ptr = (ALL_LANES_EPARAM_LINK_INFO *) &X11DPTPSAllLanesEparamTable;
    
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
	UINT32	ReturnVal0 = 0, ReturnVal1 = 0, ReturnVal2 = 0, ReturnVal3 = 0, SIOM_ID = 0, SXB3_ID = 0, SXB4_ID = 0, SXB1_ID = 0, SXB2_ID = 0;
	DEBUG((EFI_D_INFO, "GetProjectPciEBifurcate Start.\n"));
//Check SIOM GPIO for different bifurcation.
	GpioGetInputValue(SIOM_ID0, &ReturnVal0);
	GpioGetInputValue(SIOM_ID1, &ReturnVal1);
	GpioGetInputValue(SIOM_ID2, &ReturnVal2);
	GpioGetInputValue(SIOM_ID3, &ReturnVal3);   
	SIOM_ID = (ReturnVal3 << 3) | (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;
	
	DEBUG ((EFI_D_INFO, "GetProjectPciEBifurcate PEI Return0 = %x, Return1 = %x, Return2 = %x, Return3 = %x, SIOM_ID = %x \n", ReturnVal0,ReturnVal1,ReturnVal2,ReturnVal3, SIOM_ID));

/*
	    AOC-MTG-I4T/I2T   : 1100b
	    AOC-MHIBF-2QG/1QG : 1010b
	    AOC-MHIBF-m2Q2G
	    AOC-MHIBF-m1Q2G
	    AOC-MH25G-b2S2G   : 1001b
	    AOC-MH25G-m2S2T   : 0111b
	    AOC-ML-4S         : 0001b
	    AOC-M25G-m4S      : 0110b
*/
	    if((SIOM_ID == 0x0C) || (SIOM_ID == 0x0A) || (SIOM_ID == 0x09) || (SIOM_ID == 0x07) || (SIOM_ID == 0x06))
	    {
		    if(SetupData->ConfigIOU0[0] == 0xff) SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxx8xxx8;
		if((SIOM_ID == 0x0A) || (SIOM_ID == 0x0C)){
		    PcdSet32(PcdSmcOBLan1BDF, 0x00110010);
		}
		
	    } else {
		    if(SetupData->ConfigIOU0[0] == 0xff) SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxxxxx16;
	    }

//Check Riser card GPIO for different bifurcation.
	GpioGetInputValue(JPCIE1_ID0, &ReturnVal0);
	GpioGetInputValue(JPCIE1_ID1, &ReturnVal1);
	GpioGetInputValue(JPCIE2_ID0, &ReturnVal2);
	GpioGetInputValue(JPCIE2_ID1, &ReturnVal3);   

	SXB3_ID = (ReturnVal1 << 1) | ReturnVal0;
	SXB4_ID = (ReturnVal3 << 1) | ReturnVal2;
	DEBUG ((EFI_D_INFO, "GetProjectPciEBifurcate PEI Return0 = %x, Return1 = %x, Return2 = %x, Return3 = %x, SXB3_ID = %x, SXB4_ID = %x \n", ReturnVal0,ReturnVal1,ReturnVal2,ReturnVal3, SXB3_ID, SXB4_ID));

    GpioGetInputValue(SXB1_ID0, &ReturnVal0);
    GpioGetInputValue(SXB1_ID1, &ReturnVal1);
    GpioGetInputValue(SXB1_ID2, &ReturnVal2);

    SXB1_ID = (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;
	
	DEBUG ((EFI_D_INFO, "GetProjectPciEBifurcate PEI Return0 = %x, Return1 = %x, Return2 = %x, SXB1_ID = %x \n", ReturnVal0,ReturnVal1,ReturnVal2, SXB1_ID));

    GpioGetInputValue(SXB2_ID0, &ReturnVal0);
    GpioGetInputValue(SXB2_ID1, &ReturnVal1);
    GpioGetInputValue(SXB2_ID2, &ReturnVal2);

    SXB2_ID = (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;
	
    DEBUG ((EFI_D_INFO, "GetProjectPciEBifurcate PEI Return0 = %x, Return1 = %x, Return2 = %x, SXB2_ID = %x \n", ReturnVal0,ReturnVal1,ReturnVal2, SXB2_ID));

	
//RSC-R2UT-E8E16R = 0x02
//RSC-R1U-E16R = 0x03 = no card installed
	    if(SXB3_ID == 0x01)//RSC-R2UT-3E8R
	    {
	        if(SetupData->ConfigIOU1[0] == 0xff) SetupData->ConfigIOU1[0] = IIO_BIFURCATE_xxx8xxx8;
	    }
	    else{
	        if(SetupData->ConfigIOU1[0] == 0xff) SetupData->ConfigIOU1[0] = IIO_BIFURCATE_xxxxxx16;
	    }
		
	    if(SetupData->ConfigIOU2[0] == 0xff) SetupData->ConfigIOU2[0] = IIO_BIFURCATE_xxx8xxx8;//3A for GBE, 3C for SXB1 RAID ADP
	    if(SIOM_ID == 0x01){
	        SetupData->PciePortDisable[9] = 1;  //10Gbe LAN map to CPU PCIe port 3A
	        PcdSet32(PcdSmcOBLan1BDF, 0x00130000);
	        PcdSet32(PcdSmcSLOT5BDF, 0x00130000);
	    }
	    else{
	        SetupData->PciePortDisable[9] = 0;  //10Gbe LAN map to CPU PCIe port 3A
	    }

    if(SetupData->ConfigIOU0[1] == 0xff) SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxx8xxx8;    //
    if(SetupData->ConfigIOU1[1] == 0xff) SetupData->ConfigIOU1[1] = IIO_BIFURCATE_xxxxxx16;    //
    
    if(SXB2_ID == 0x02){//AOC-SMG3-2H8M2	    
      if(SetupData->ConfigIOU2[1] == 0xff) SetupData->ConfigIOU2[1] = IIO_BIFURCATE_x4x4x4x4;
	    SetupData->DfxDnTxPreset[30] = 0x0004;
	    SetupData->DfxDnTxPreset[31] = 0x0004;
	    SetupData->TXEQ[30] = 1;
	    SetupData->TXEQ[31] = 1;
	    }
	    else{
      if(SetupData->ConfigIOU2[1] == 0xff) SetupData->ConfigIOU2[1] = IIO_BIFURCATE_xxxxxx16;    //  
      }	    
	    
    if(SetupData->ConfigMCP0[0] == 0xff){
    	SetupData->ConfigMCP0[0] = IIO_BIFURCATE_xxxxxx16;
    }
    if(SetupData->ConfigMCP1[0] == 0xff){
        SetupData->ConfigMCP1[0] = IIO_BIFURCATE_xxxxxx16;
    }
    if(SetupData->ConfigMCP0[1] == 0xff){
        SetupData->ConfigMCP0[1] = IIO_BIFURCATE_xxxxxx16;
    }
    if(SetupData->ConfigMCP1[1] == 0xff){
        SetupData->ConfigMCP1[1] = IIO_BIFURCATE_xxxxxx16;
    }

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
EFI_STATUS EFIAPI X11DPTPSPeiDriverInit(
  IN EFI_FFS_FILE_HEADER *FfsHeader,
  IN EFI_PEI_SERVICES **PeiServices
  )
{
	EFI_PEI_PPI_DESCRIPTOR *mPpiList = NULL;
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_PEI_PCI_CFG2_PPI *PciCfg;
    EFI_PEI_CPU_IO_PPI          *CpuIo;

	DEBUG ((DEBUG_INFO, "ProjectPeiDriverInit.\n"));

    // Get pointer to the PCI config PPI
    PciCfg = (*PeiServices)->PciCfg;
    CpuIo = (*PeiServices)->CpuIo;

    // Set the CRB Notify PPI
    Status = (*PeiServices)->NotifyPpi(PeiServices, mNotifyList);
    ASSERT_PEI_ERROR (PeiServices, Status);

    Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (EFI_PEI_PPI_DESCRIPTOR), &mPpiList);
    ASSERT_PEI_ERROR (PeiServices, Status);

    mPpiList->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    mPpiList->Guid = &gProjectOwnPeiProtocolGuid;
    mPpiList->Ppi = &PeiProjectInterface;

    Status = (**PeiServices).InstallPpi(PeiServices, mPpiList);
    if (EFI_ERROR(Status)){
        DEBUG ((DEBUG_INFO, "ProjectPeiDriverInit fail.\n"));
        return Status;
    }

    DEBUG ((DEBUG_INFO, "Exit ProjectPeiDriverInit.\n"));

    return EFI_SUCCESS;
}
EFI_STATUS PeiPericomInit(
    IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *NullPpi
)
{
//	UINT8	CardID;
	EFI_STATUS		Status = EFI_SUCCESS;
	EFI_PEI_SMBUS2_PPI	*SmBus;
	EFI_SMBUS_DEVICE_ADDRESS	Address;
	UINT8		PericomReadback_Byte=1;
#if SGI_OEM_SUPPORT	//for Toshiba SSD which needs special redrive value instead of normal HDD settings
	UINT8		PericomTBL0[] = {0x00, 0xFE, 0x00, 0x00, 0x57, 0x47, 0x57, 0x47, 0x57, 0x47, 0x57, 0x47, 0xEF, 0xC0};
	UINT8		PericomTBL1[] = {0x00, 0xFE, 0x00, 0x00, 0x57, 0x47, 0x57, 0x47, 0x57, 0x47, 0x57, 0x47, 0xEF, 0xC0};
	UINT8		PericomTBL2[] = {0x00, 0xFE, 0x00, 0x00, 0x57, 0x47, 0x57, 0x47, 0x57, 0x47, 0x57, 0x47, 0xEF, 0xC0};
#else 
	UINT8		PericomTBL0[] = {0x00, 0xF8, 0x00, 0x00, 0x97, 0x87, 0x97, 0x87, 0x97, 0x87, 0x97, 0x87, 0xF7, 0x00};
	UINT8		PericomTBL1[] = {0x00, 0xF8, 0x00, 0x00, 0x97, 0x87, 0x97, 0x87, 0x97, 0x87, 0x97, 0x87, 0xF7, 0x00};
	UINT8		PericomTBL2[] = {0x00, 0xF8, 0x00, 0x00, 0x97, 0x87, 0x97, 0x87, 0x97, 0x87, 0x97, 0x87, 0xF7, 0x00};
#endif // SGI_OEM_SUPPORT
	UINT8		NMVe_Redrv[4][25][2] ={
									{{0x06, 0x18},
									 {0x0f, 0x02}, {0x10, 0xaa}, {0x11, 0xe1},
									 {0x16, 0x00}, {0x17, 0xaa}, {0x18, 0xe1},
									 {0x1d, 0x00}, {0x1e, 0xaa}, {0x1f, 0x01},
									 {0x24, 0x00}, {0x25, 0xaa}, {0x26, 0x01},
									 {0x2c, 0x02}, {0x2d, 0xaa}, {0x2e, 0xe0},
									 {0x33, 0x00}, {0x34, 0xaa}, {0x35, 0xe0},
									 {0x3a, 0x00}, {0x3b, 0xaa}, {0x3c, 0xe0},
									 {0x41, 0x00}, {0x42, 0xaa}, {0x43, 0xe0}},
									{{0x06, 0x18},
									 {0x0f, 0x02}, {0x10, 0xad}, {0x11, 0xe0},
									 {0x16, 0x00}, {0x17, 0xad}, {0x18, 0xe0},
									 {0x1d, 0x00}, {0x1e, 0xad}, {0x1f, 0xe0},
									 {0x24, 0x00}, {0x25, 0xad}, {0x26, 0xe0},
									 {0x2c, 0x00}, {0x2d, 0xad}, {0x2e, 0xe0},
									 {0x33, 0x00}, {0x34, 0xad}, {0x35, 0xe0},
									 {0x3a, 0x00}, {0x3b, 0xad}, {0x3c, 0xe0},
									 {0x41, 0x00}, {0x42, 0xad}, {0x43, 0xe0}},
									{{0x06, 0x18},
									 {0x0f, 0x00}, {0x10, 0xad}, {0x11, 0xe0},
									 {0x16, 0x00}, {0x17, 0xad}, {0x18, 0x00},
									 {0x1d, 0x00}, {0x1e, 0xad}, {0x1f, 0x00},
									 {0x24, 0x00}, {0x25, 0xad}, {0x26, 0x00},
									 {0x2c, 0x00}, {0x2d, 0xad}, {0x2e, 0x00},
									 {0x33, 0x00}, {0x34, 0xad}, {0x35, 0x00},
									 {0x3a, 0x00}, {0x3b, 0xad}, {0x3c, 0x00},
									 {0x41, 0x00}, {0x42, 0xad}, {0x43, 0x00}},
									{{0x06, 0x18},
									 {0x0f, 0x00}, {0x10, 0xad}, {0x11, 0xe0},
									 {0x16, 0x00}, {0x17, 0xad}, {0x18, 0xe0},
									 {0x1d, 0x01}, {0x1e, 0xad}, {0x1f, 0xe0},
									 {0x24, 0x00}, {0x25, 0xad}, {0x26, 0xe0},
									 {0x2c, 0x00}, {0x2d, 0xad}, {0x2e, 0xe0},
									 {0x33, 0x00}, {0x34, 0xad}, {0x35, 0xe0},
									 {0x3a, 0x00}, {0x3b, 0xad}, {0x3c, 0xe0},
									 {0x41, 0x00}, {0x42, 0xad}, {0x43, 0xe0}}
									};
	UINT8		NMVe_Redrv_401A[4][25][2] ={
									{{0x06, 0x18},
									 {0x0f, 0x00}, {0x10, 0xac}, {0x11, 0xe0},
									 {0x16, 0x00}, {0x17, 0xac}, {0x18, 0xe0},
									 {0x1d, 0x00}, {0x1e, 0xac}, {0x1f, 0xe0},
									 {0x24, 0x00}, {0x25, 0xac}, {0x26, 0xe0},
									 {0x2c, 0x03}, {0x2d, 0xae}, {0x2e, 0x01},
									 {0x33, 0x03}, {0x34, 0xae}, {0x35, 0x01},
									 {0x3a, 0x03}, {0x3b, 0xae}, {0x3c, 0x01},
									 {0x41, 0x03}, {0x42, 0xae}, {0x43, 0x01}},
									{{0x06, 0x18},
									 {0x0f, 0x00}, {0x10, 0xac}, {0x11, 0xe0},
									 {0x16, 0x00}, {0x17, 0xac}, {0x18, 0xe0},
									 {0x1d, 0x00}, {0x1e, 0xac}, {0x1f, 0xe0},
									 {0x24, 0x00}, {0x25, 0xac}, {0x26, 0xe0},
									 {0x2c, 0x03}, {0x2d, 0xae}, {0x2e, 0x01},
									 {0x33, 0x03}, {0x34, 0xae}, {0x35, 0x01},
									 {0x3a, 0x03}, {0x3b, 0xae}, {0x3c, 0x01},
									 {0x41, 0x03}, {0x42, 0xae}, {0x43, 0x01}},
									{{0x06, 0x18},
									 {0x0f, 0x00}, {0x10, 0xac}, {0x11, 0xe0},
									 {0x16, 0x00}, {0x17, 0xac}, {0x18, 0xe0},
									 {0x1d, 0x00}, {0x1e, 0xac}, {0x1f, 0xe0},
									 {0x24, 0x00}, {0x25, 0xac}, {0x26, 0xe0},
									 {0x2c, 0x03}, {0x2d, 0xae}, {0x2e, 0x01},
									 {0x33, 0x03}, {0x34, 0xae}, {0x35, 0x01},
									 {0x3a, 0x03}, {0x3b, 0xae}, {0x3c, 0x01},
									 {0x41, 0x03}, {0x42, 0xae}, {0x43, 0x01}},
									{{0x06, 0x18},
									 {0x0f, 0x00}, {0x10, 0xac}, {0x11, 0xe0},
									 {0x16, 0x00}, {0x17, 0xac}, {0x18, 0xe0},
									 {0x1d, 0x00}, {0x1e, 0xac}, {0x1f, 0xe0},
									 {0x24, 0x00}, {0x25, 0xac}, {0x26, 0xe0},
									 {0x2c, 0x03}, {0x2d, 0xae}, {0x2e, 0x01},
									 {0x33, 0x03}, {0x34, 0xae}, {0x35, 0x01},
									 {0x3a, 0x03}, {0x3b, 0xae}, {0x3c, 0x01},
									 {0x41, 0x03}, {0x42, 0xae}, {0x43, 0x01}}
									};

	UINT8		NMVe_Redrv_401A_BDW[4][25][2] ={
									{{0x06, 0x18},
									 {0x0f, 0x00}, {0x10, 0xae}, {0x11, 0xe2},
									 {0x16, 0x00}, {0x17, 0xae}, {0x18, 0xe2},
									 {0x1d, 0x00}, {0x1e, 0xae}, {0x1f, 0xe2},
									 {0x24, 0x00}, {0x25, 0xae}, {0x26, 0xe2},
									 {0x2c, 0x03}, {0x2d, 0xae}, {0x2e, 0x00},
									 {0x33, 0x03}, {0x34, 0xae}, {0x35, 0x00},
									 {0x3a, 0x03}, {0x3b, 0xae}, {0x3c, 0x00},
									 {0x41, 0x03}, {0x42, 0xae}, {0x43, 0x00}},
									{{0x06, 0x18},
									 {0x0f, 0x00}, {0x10, 0xae}, {0x11, 0xe2},
									 {0x16, 0x00}, {0x17, 0xae}, {0x18, 0xe2},
									 {0x1d, 0x00}, {0x1e, 0xae}, {0x1f, 0xe2},
									 {0x24, 0x00}, {0x25, 0xae}, {0x26, 0xe2},
									 {0x2c, 0x03}, {0x2d, 0xae}, {0x2e, 0x00},
									 {0x33, 0x03}, {0x34, 0xae}, {0x35, 0x00},
									 {0x3a, 0x03}, {0x3b, 0xae}, {0x3c, 0x00},
									 {0x41, 0x03}, {0x42, 0xae}, {0x43, 0x00}},
									{{0x06, 0x18},
									 {0x0f, 0x00}, {0x10, 0xae}, {0x11, 0xe2},
									 {0x16, 0x00}, {0x17, 0xae}, {0x18, 0xe2},
									 {0x1d, 0x00}, {0x1e, 0xae}, {0x1f, 0xe2},
									 {0x24, 0x00}, {0x25, 0xae}, {0x26, 0xe2},
									 {0x2c, 0x03}, {0x2d, 0xae}, {0x2e, 0x00},
									 {0x33, 0x03}, {0x34, 0xae}, {0x35, 0x00},
									 {0x3a, 0x03}, {0x3b, 0xae}, {0x3c, 0x00},
									 {0x41, 0x03}, {0x42, 0xae}, {0x43, 0x00}},
									{{0x06, 0x18},
									 {0x0f, 0x00}, {0x10, 0xae}, {0x11, 0xe2},
									 {0x16, 0x00}, {0x17, 0xae}, {0x18, 0xe2},
									 {0x1d, 0x00}, {0x1e, 0xae}, {0x1f, 0xe2},
									 {0x24, 0x00}, {0x25, 0xae}, {0x26, 0xe2},
									 {0x2c, 0x03}, {0x2d, 0xae}, {0x2e, 0x00},
									 {0x33, 0x03}, {0x34, 0xae}, {0x35, 0x00},
									 {0x3a, 0x03}, {0x3b, 0xae}, {0x3c, 0x00},
									 {0x41, 0x03}, {0x42, 0xae}, {0x43, 0x00}}
									};
	
	UINTN		i, j, Len;
	UINT8		value8 = Check_BPN_GPI();

//	while(1);
	Status = (*PeiServices)->LocatePpi(PeiServices,	&gEfiPeiSmbus2PpiGuid, 0, NULL,	&SmBus );
//	ASSERT_PEI_ERROR( PeiServices, Status);
	
//TwinPro adapter detection					SXB1					SXB2
//									GPIO64	GPIO65	GPIO68	|	GPIO69	GPIO70	GPIO71
//Not Present						1		1		1	   	|	1		1		1
//BPN-ADP-S3108-H8iRP (John Chen)	1		0		1	   	|	1		1		1
//AOC-PTG-i1S (Jerry Deng)			0		1		1	   	|	No Care	No Care	No Care
//BPN-ADP-8SATA3P (Yuri Tsai)		0  		0		1	   	|	1		1		1
//BPN-ADP-6SATA3N2-2UP				1		?		0	   	|	1		1		1
//BPN-ADP-8S3008N4-2UP				1		0		0	   	|	1		1		1--actually it's 0
//BPN-ADP-6SATA3P (Wai Jor)			No Care	No Care	No Care	|	1		1		1
//BPN-ADP-S3008H-L8IP (John Chen)	No Care	No Care	No Care	|	1		1		0
//BPN-ADP-S3108L-H6iR (Darren Liang)1		1		1	   	|	1		0		1
//BPN-ADP-S3008L-L6IP (Peter Lee)	No Care	No Care	No Care	|	1		0		0
//BPN-ADP-6SATA3-1UPK (Lei Xie) 	No Care	No Care	No Care	|	0		1		1
	switch (value8)
	{
		case 0x61 :       	// BPN-ADP-8S3008N4-2UP (John Chen)
		case 0x60 :
			DEBUG((EFI_D_INFO, "Start program Ti.\n"));
			Len = 1;
			Address.SmbusDeviceAddress = 0xB0>>1;
			for (i=0; i<4; i++)
			{
				if(value8==0x61)
					for(j=0; j<25; j++)
						Status = SmBus->Execute(SmBus, Address, NMVe_Redrv[i][j][0], EfiSmbusWriteByte, 0, &Len, &NMVe_Redrv[i][j][1]);
				else // 0x60
					for(j=0; j<25; j++)
					{
							Status = SmBus->Execute(SmBus, Address, NMVe_Redrv_401A[i][j][0], EfiSmbusWriteByte, 0, &Len, &NMVe_Redrv_401A[i][j][1]);
					}
				Address.SmbusDeviceAddress++;
			}
			break;

		case 0xf1 :			// BPN-ADP-S3108-H8iRP (John Chen) or BPN-ADP-6SATA3P (Wai Jor)
		case 0x71 :			// BPN-ADP-S3008H-L8IP (John Chen)
		case 0xf0 :			// AOC-PTG-i1S (Jerry Deng) or BPN-ADP-8SATA3P (Yuri Tsai)
//		case 0xb1 :			// BPN-ADP-S3108L-H6iR (Darren Liang), no redrive chip
		case 0x31 :			// BPN-ADP-S3008L-L6IP (Peter Lee)
		case 0xd1 :			// BPN-ADP-6SATA3-1UPK (Lei Xie)
			DEBUG((EFI_D_INFO, "Program PeiPericom start.\n"));
			Address.SmbusDeviceAddress = 0xC4 >> 1;
			Len = 1;					// read one byte
			Status = SmBus->Execute( SmBus,	Address, 0,	EfiSmbusReadBlock, 0, &Len, &PericomReadback_Byte);
			if((Status&2)==0)		// if C4 has data, that means BPN-ADP-8SATA3P (Yuri Tsai)
			{                       // EfiSmbusReadBlock has bug, we have to use alternative way to judge success
				/*
				PericomTBL0[2] = 0;
				PericomTBL0[3] = 0;
				PericomTBL0[10] = 0xB1;
				PericomTBL0[11] = 0x27;
				PericomTBL1[2] = 0;
				PericomTBL1[3] = 0;
				PericomTBL1[10] = 0xB1;
				PericomTBL1[11] = 0x27;
				PericomTBL2[2] = 0;
				PericomTBL2[3] = 0;
				PericomTBL2[10] = 0xB1;
				PericomTBL2[11] = 0x27;
				*/
				Len = sizeof(PericomTBL2);
				Status = SmBus->Execute(SmBus, Address, 0, EfiSmbusWriteBlock, 0, &Len,	PericomTBL2);
			}
			Address.SmbusDeviceAddress = 0xC6 >> 1;
			Len = 1;					// read one byte
			Status = SmBus->Execute( SmBus,	Address, 0,	EfiSmbusReadBlock, 0, &Len, &PericomReadback_Byte);
			if(Status&2)			// if C6 has no data, that means old SATA ADP
				Address.SmbusDeviceAddress = 0xC2 >> 1;		// C2 for old SATA ADP
			Len = sizeof(PericomTBL1);
			Status = SmBus->Execute(SmBus, Address, 0, EfiSmbusWriteBlock, 0, &Len,	PericomTBL1);
			Address.SmbusDeviceAddress = 0xC0 >> 1;
			Status = SmBus->Execute(SmBus, Address, 0, EfiSmbusWriteBlock, 0, &Len,	PericomTBL0);
			break;
//		default:
	}
	return Status;
}
