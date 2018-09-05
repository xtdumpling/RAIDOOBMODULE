//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.11
//    Bug Fix:  Fixed build error. 
//    Reason:   
//    Auditor:  Joshua Fan
//    Date:     Aug/23/2017
//
//  Rev. 1.11
//    Bug Fix:  Fixed incorrect LED when switch VMD mode 
//    Reason:   
//    Auditor:  Joshua Fan
//    Date:     Jun/22/2017
//
//  Rev. 1.10
//    Bug Fix:   
//    Reason:   Added token SLOT_INFO_Tbl for NVMe addon card detecting.
//    Auditor:  Joshua Fan
//    Date:     Jun/20/2017
//
//  Rev. 1.09
//    Bug Fix:   
//    Reason:   Clear up non used code.
//    Auditor:  Jacker Yeh
//    Date:     May/26/2017
//
//  Rev. 1.08
//    Bug Fix:   
//    Reason:   Updated Bifurcation for SIOM slot.
//    Auditor:  Joshua Fan
//    Date:     May/24/2017
//
//  Rev. 1.07
//    Bug Fix:   
//    Reason:   Updated Bifurcation and switch OPROM BDF for AOC-MTG-i2T/i4T (X550).
//    Auditor:  Joshua Fan
//    Date:     May/10/2017
//
//  Rev. 1.06
//    Bug Fix:   
//    Reason:   Detecting the AOM-4M23-2UF card and switch bifurcation.
//    Auditor:  Joshua Fan
//    Date:     May/04/2017
//
//  Rev. 1.05
//    Bug Fix:  Disable unuse code. 
//    Reason:   
//    Auditor:  Joshua Fan
//    Date:     Apr/05/2017
//
//  Rev. 1.04
//    Bug Fix:  Doesn't show MAC address for AOC-ML-2S/4S
//    Reason:   Modified onboard LAN and slot BDF
//    Auditor:  Joshua Fan
//    Date:     Mar/03/2017
//
//  Rev. 1.03
//    Bug Fix:  
//    Reason:   Updated VMD mode setting
//    Auditor:  Joshua Fan
//    Date:     Feb/17/2017
//
//  Rev. 1.02
//    Bug Fix:  Updated hotplug function setting
//    Reason:   
//    Auditor:  Joshua Fan
//    Date:     Feb/06/2017
//
//  Rev. 1.01
//    Bug Fix:  Corrected GBE and SIOM slot
//    Reason:   
//    Auditor:  Joshua Fan
//    Date:     Jan/27/2017
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Joshua Fan
//    Date:     Jan/18/2017
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
#include <Efi.h>
#include <Pei.h>
#include <token.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
//#include <Hob.h>
//#include <AmiPeiLib.h>
//#include <AmiCspLib.h>
//#include <ppi\ReadOnlyVariable2.h>
#include "X11DPFRPeiDriver.h"
#include "SmcCspLibBmc.h"
#include <GpioConfig.h>
#include <Library\GpioLib.h>
#include <GpioPinsSklH.h>
#include <Library\IoLib.h>
#include <Ppi/IPMITransportPpi.h>

#if SmcAOC_SUPPORT
#include "../Module/SmcAOC/SmcAOCPei.h"
EFI_GUID gSmcAOCPeiProtocolGuid = EFI_SMC_AOC_PEI_PROTOCOL_GUID;
#endif
EFI_GUID gProjectOwnPeiProtocolGuid = EFI_PROJECT_OWN_PEI_DRIVER_PROTOCOL_GUID;
/*-----------------12/23/2014 3:06PM----------------
 * Coding for every platform
 * --------------------------------------------------*/

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
    PEI_IPMI_TRANSPORT_PPI      *IpmiTransportPpi;    
    UINT8   NVMeModeBitMap = 0, Data = 0, Retry, i;
    UINTN   Len = 1;
    UINT8   IpmiNvmeData[] = {0x0d, BPN_CPLD1_Address, 0x00, 0x08, 0x00};
    UINT8   ResponseDataSize = 10, ResponseData[10];
    UINT32	ReturnVal0 = 0, ReturnVal1 = 0, ReturnVal2 = 0, ReturnVal3 = 0, ReturnVal4 = 0, ReturnVal5 = 0, SIOM_ID = 0, SXB3_ID = 0, SXB4_ID = 0, SXB5_ID = 0;
    UINT8			DefaultIOU0[2], DefaultIOU1[2], DefaultIOU2[2];
#if SmcAOC_SUPPORT
    PEI_SMC_AOC_PROTOCOL	*mSmcAOCPeiProtocol = NULL;
    SMC_SLOT_INFO               SLOT_Tbl[] = SLOT_INFO_Tbl;
#endif
    
    DEBUG((EFI_D_INFO, "GetProjectPciEBifurcate Start.\n"));
    
    DefaultIOU0[0] = SetupData->ConfigIOU0[0];
    DefaultIOU1[0] = SetupData->ConfigIOU1[0];
    DefaultIOU2[0] = SetupData->ConfigIOU2[0];
    DefaultIOU0[1] = SetupData->ConfigIOU0[1];
    DefaultIOU1[1] = SetupData->ConfigIOU1[1];
    DefaultIOU2[1] = SetupData->ConfigIOU2[1];

    DEBUG((-1, "DefaultIOU = %x, %x, %x\n", DefaultIOU0[0], DefaultIOU1[0], DefaultIOU2[0]));
    DEBUG((-1, "DefaultIOU = %x, %x, %x\n", DefaultIOU0[1], DefaultIOU1[1], DefaultIOU2[1]));
    
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
	    if((SIOM_ID == 0x0A) || (SIOM_ID == 0x09) || (SIOM_ID == 0x07) || (SIOM_ID == 0x06) || (SIOM_ID == 0x0C))
	    {
		    if(SetupData->ConfigIOU2[0] == 0xff) SetupData->ConfigIOU2[0] = IIO_BIFURCATE_xxx8xxx8;
		    PcdSet32(PcdSmcSLOT6BDF, 0x00130010);
			PcdSet8(PcdSmcMultiFuncLanChip1, 0x02);
	        if((SIOM_ID == 0x0A) || (SIOM_ID == 0x0C)) {
	    	    if(SetupData->ConfigIOU2[0] == 0xff) SetupData->ConfigIOU2[0] = IIO_BIFURCATE_xxx8xxx8;
	    	    PcdSet32(PcdSmcSLOT5BDF, 0x00130010);
	    	    PcdSet32(PcdSmcSLOT6BDF, 0x00130000);
	    	    PcdSet32(PcdSmcOBLan1BDF, 0x00130010);
		    }
	    } else {
		    if(SetupData->ConfigIOU2[0] == 0xff) SetupData->ConfigIOU2[0] = IIO_BIFURCATE_xxxxxx16;
	    }

//Check Riser card GPIO for different bifurcation.
	GpioGetInputValue(JPCIE1_ID0, &ReturnVal0);
	GpioGetInputValue(JPCIE1_ID1, &ReturnVal1);
	GpioGetInputValue(JPCIE2_ID0, &ReturnVal2);
	GpioGetInputValue(JPCIE2_ID1, &ReturnVal3); 
	GpioGetInputValue(SASSLOT_ID1, &ReturnVal4);
	GpioGetInputValue(SASSLOT_ID2, &ReturnVal5);

	SXB3_ID = (ReturnVal1 << 1) | ReturnVal0; // JPCIE1_1 (SXB1)
	SXB4_ID = (ReturnVal3 << 1) | ReturnVal2; // JPCIE2   (SXB3)
	SXB5_ID = (ReturnVal5 << 1) | ReturnVal4; // SAS SLOT (SXB2)
	DEBUG ((EFI_D_INFO, "GetProjectPciEBifurcate PEI Return0 = %x, Return1 = %x, Return2 = %x, Return3 = %x, SXB3_ID = %x, SXB4_ID = %x \n", ReturnVal0,ReturnVal1,ReturnVal2,ReturnVal3, SXB3_ID, SXB4_ID));
	DEBUG ((EFI_D_INFO, "GetProjectPciEBifurcate PEI Return4 = %x, Return5 = %x, SXB5_ID = %x \n", ReturnVal4,ReturnVal5,SXB5_ID));

//RSC-R2UT-E8E16R = 0x02
//RSC-R1U-E16R = 0x03 = no card installed
	    if(SXB3_ID == 0x01)//RSC-R2UT-3E8R
	    {
	        if(SetupData->ConfigIOU0[0] == 0xff) SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxx8xxx8;
	    }
	    else{
	        if(SetupData->ConfigIOU0[0] == 0xff) SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxxxxx16;
	    }
		
	    if(SetupData->ConfigIOU1[0] == 0xff) SetupData->ConfigIOU1[0] = IIO_BIFURCATE_xxx8xxx8;//2A for GBE, 2C for SXB2
    if(SIOM_ID == 0x01) {
        SetupData->PciePortDisable[7] = 1;  //10Gbe LAN map to CPU PCIe port 3A
        PcdSet32(PcdSmcOBLan1BDF, 0x00120010);
        PcdSet32(PcdSmcSLOT5BDF, 0x00120010);
    } else {
        SetupData->PciePortDisable[7] = 0;  //10Gbe LAN map to CPU PCIe port 3A
    }

    if(SetupData->ConfigIOU0[1] == 0xff) SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxx8xxx8;    // 1C for SAS SLOT
    if(SetupData->ConfigIOU1[1] == 0xff) SetupData->ConfigIOU1[1] = IIO_BIFURCATE_x4x4x4x4;    // For four NVMe
    if(SetupData->ConfigIOU2[1] == 0xff) SetupData->ConfigIOU2[1] = IIO_BIFURCATE_xxxxxx16;    // JPCIE2 (SXB3)
    
    if(SXB5_ID == 0x02) {
    	DEBUG ((EFI_D_INFO, "AOM-4M23-2UF\n"));
    	SetupData->ConfigIOU1[0] = IIO_BIFURCATE_xxx8x4x4; // CPU0 2A
    	SetupData->ConfigIOU0[1] = IIO_BIFURCATE_x4x4xxx8; // CPU1 1C
    }
    
    // 
    // NVMe setting
    //
    
    SetupData->PcieHotPlugEnable = 1;
    // 2A
    SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_2A_INDEX]    = 1;    //NVMe
    SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_2A_INDEX]    = 26;
    if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_2A_INDEX - 1] == 0) {
        OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_2A_INDEX, VPP_PORT_0, 0x00, PCIEAIC_OCL_OWNERSHIP);
        NVMeModeBitMap &= ~BIT0;
    } else {
        OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_2A_INDEX, VPP_PORT_0, 0x00, VMD_OWNERSHIP);
        NVMeModeBitMap |= BIT0;
    }
    // 2B
    SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_2B_INDEX]    = 1;    //NVMe
    SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_2B_INDEX]    = 27;
    if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_2B_INDEX - 1] == 0) {
        OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_2B_INDEX, VPP_PORT_1, 0x00, PCIEAIC_OCL_OWNERSHIP);
        NVMeModeBitMap &= ~BIT1;
    } else {
        OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_2B_INDEX, VPP_PORT_1, 0x00, VMD_OWNERSHIP);
        NVMeModeBitMap |= BIT1;
    }
    // 2C
    SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_2C_INDEX]    = 1;    //NVMe
    SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_2C_INDEX]    = 28;
    if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_2C_INDEX - 1] == 0) {
        OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_2C_INDEX, VPP_PORT_0, 0x02, PCIEAIC_OCL_OWNERSHIP);
        NVMeModeBitMap &= ~BIT2;
    } else {
        OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_2C_INDEX, VPP_PORT_0, 0x02, VMD_OWNERSHIP);              
        NVMeModeBitMap |= BIT2;
    }
    // 2D
    SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_2D_INDEX]    = 1;    //NVMe
    SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_2D_INDEX]    = 29;
    if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_2D_INDEX - 1] == 0) {
        OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_2D_INDEX, VPP_PORT_1, 0x02, PCIEAIC_OCL_OWNERSHIP);
        NVMeModeBitMap &= ~BIT3;
    } else {
        OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_2D_INDEX, VPP_PORT_1, 0x02, VMD_OWNERSHIP);
        NVMeModeBitMap |= BIT3;
    }
    
#if SmcAOC_SUPPORT

    Status = (*PeiServices)->LocatePpi(
		    PeiServices,
    		    &gSmcAOCPeiProtocolGuid,
    		    0,
    		    NULL,
    		    &mSmcAOCPeiProtocol);
    DEBUG((-1, "LocatePpi mSmcAOCPeiProtocol = %r.\n", Status));
    if(!Status){
        mSmcAOCPeiProtocol->SmcAOCPeiCardDetection(PeiServices, 0, 0, 0, SLOT_Tbl,
                        sizeof(SLOT_Tbl)/sizeof(SMC_SLOT_INFO));
	mSmcAOCPeiProtocol->SmcAOCPeiBifurcation(SetupData, DefaultIOU0,
	                DefaultIOU1, DefaultIOU2);
	mSmcAOCPeiProtocol->SmcAOCPeiSetNVMeMode(PeiServices, 0, 0, 0, SLOT_Tbl,
	                sizeof(SLOT_Tbl)/sizeof(SMC_SLOT_INFO));
    }
#endif
    
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


    Status = (*PeiServices)->LocatePpi(PeiServices, &gEfiPeiIpmiTransportPpiGuid, 0, NULL, &IpmiTransportPpi);
    DEBUG((-1, "LocatePpi IpmiTransportPpi %r\n", Status));
    if(Status)  return Status;

    IpmiNvmeData[4] = NVMeModeBitMap;
    DEBUG((DEBUG_INFO, "NVMeModeBitMap = %x.\n", NVMeModeBitMap));

    for(Retry = 0; Retry < 3; Retry++) {
        Status = IpmiTransportPpi->SendIpmiCommand(
                        IpmiTransportPpi,
                        0x06,       // APP
                        BMC_LUN,
                        0x52,       // master write-read
                        IpmiNvmeData,
                        sizeof(IpmiNvmeData),
                        ResponseData,
                        &ResponseDataSize);
        DEBUG((DEBUG_INFO, "Set NVMe Mode via IPMI Status = %r.\n", Status));
        for ( i = 0; i < 250; i++ ) {
          IoWrite8( 0xEB, 0x55 );
          IoWrite8( 0xEB, 0xAA );
        }
        if(!Status) break;
    }

    DEBUG((EFI_D_INFO, "GetProjectPciEBifurcate End.\n"));
    return EFI_SUCCESS;
}

VOID
OemEnableHotPlug (
    IN OUT  IIO_CONFIG  *SetupData,
    IN UINT8 Port,
    IN UINT8 VppPort,
    IN UINT8 VppAddress,
    IN UINT8 PortOwnership
  )
{
  SetupData->SLOTHPCAP[Port]= ENABLE;      // Slot Hot Plug capable
  SetupData->SLOTAIP[Port]  = ENABLE;      // Attention Indicator Present
  SetupData->SLOTPIP[Port]  = ENABLE;      // Power Indicator Present

  if (PortOwnership ==  PCIEAIC_OCL_OWNERSHIP){
    SetupData->SLOTABP[Port]  = ENABLE;    // Attention Button Present
    SetupData->SLOTPCP[Port]  = ENABLE;    // Power Controlled Present
    SetupData->SLOTHPSUP[Port]= DISABLE;   // HotPlug Surprise
  }
  if (PortOwnership ==  VMD_OWNERSHIP){
    SetupData->SLOTABP[Port]  = DISABLE;   // Attention Button Present
//  SetupData->SLOTPCP[Port]  = DISABLE;   // Power Controlled Present
    SetupData->SLOTHPSUP[Port]= ENABLE;    // HotPlug Surprise
  }

  if (VppPort!= VPP_PORT_MAX) {
    SetupData->VppEnable[Port]= ENABLE;
    SetupData->VppPort[Port]= VppPort;
    SetupData->VppAddress[Port] = VppAddress;
  } else {
      DEBUG((EFI_D_ERROR, "PCIE HOT Plug. Missing VPP values on slot table\n"));
  }
}

PROJECT_PEI_DRIVER_PROTOCOL PeiProjectInterface =
{
	PeiProjectCheckAdjustVID,
	NULL,
	NULL,
	PeiProjectHardwareInfo,
	PeiProjectCheckBMCPresent,
	NULL,
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
EFI_STATUS EFIAPI X11DPFRPeiDriverInit(
  IN EFI_FFS_FILE_HEADER *FfsHeader,
  IN EFI_PEI_SERVICES **PeiServices
  )
{
	EFI_PEI_PPI_DESCRIPTOR *mPpiList = NULL;
    EFI_STATUS Status = EFI_SUCCESS;

	DEBUG ((DEBUG_INFO, "ProjectPeiDriverInit.\n"));

    Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (EFI_PEI_PPI_DESCRIPTOR), &mPpiList);
    //ASSERT_PEI_ERROR (PeiServices, Status);

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
