//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
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
#include <Token.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
#include <Ppi/Smbus2.h>
#include "X11DPTBPeiDriver.h"
#include "SmcCspLibBmc.h"
#include <Library\GpioLib.h>
#include <GpioPinsSklH.h>
#include <Library\IoLib.h>
#include <Ppi/IPMITransportPpi.h>

EFI_GUID gProjectOwnPeiProtocolGuid = EFI_PROJECT_OWN_PEI_DRIVER_PROTOCOL_GUID;

static GPIO_CONFIG INIT_SIOM_ID_PIN[] =
{
    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone, }
};	// Default setting for GPIO input

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

/*-----------------12/23/2014 3:06PM----------------
 * Coding for every platform
 * --------------------------------------------------*/
//HSX_PER_LANE_EPARAM_LINK_INFO  X11DPTBPerLanesEparamTable[] = {
//};
//UINT32 X11DPTBPerLanesEparamTableSize = sizeof (X11DPTBPerLanesEparamTable);
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure   :  Check_BPN_GPI
// Description :  Collect GPI info from SXB1/SXB2 slots
// Parameters  :  NONE
// Returns     :  GPI values from SXB1 AND SXB2
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
//UINT8 Check_BPN_GPI(VOID)
//{
//BigTwin adapter detection                                      SXB2                                                    SXB1
//                                               GPP_G8          GPP_G9          GPP_G10         |       GPP_G11         GPP_G12         GPP_G13
//Not Present                                    1               1               1               |       1               1               1
//BPN-ADP-6NVMe3-1UB (Rannie Chen)               0               1               1               |       x               x               x
//BPN-ADP-6SATA3-1UB (Dican Zhu)                 1               1               1               |       1               1               0
//BPN-ADP-6S3008N4-1UB (Chris Chang)             1               0               1               |       x               x               x
//BPN-ADP-2M2-1UB (Grant Liu)                    x               x               x               |       0               1               1
//BPN-ADP-12NVMe-2UB (Rannie Chen)               0               0               1               |       1               1               1
//BPN-ADP-12S3216N4-2UB (Jerry Mou)              1               1               0               |       1               1               1
//BPN-ADP-4TNS6-1UB (Hung Pham)                  1               0               0               |       1               1               1
//}

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
// Description :  Create Project own Hardware Information
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
	UINT32	ReturnVal0 = 0, ReturnVal1 = 0, ReturnVal2 = 0, ReturnVal3 = 0, SIOM_ID = 0;

	DEBUG((EFI_D_INFO, "X11DPTB PeiProjectHardwareInfo.\n"));

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
//  AOC-MHFI-i1C : 1000b
	GpioSetPadConfig(SIOM_ID0, INIT_SIOM_ID_PIN);	// Initial GPIO before read
	GpioSetPadConfig(SIOM_ID1, INIT_SIOM_ID_PIN);	// Initial GPIO before read
	GpioSetPadConfig(SIOM_ID2, INIT_SIOM_ID_PIN);	// Initial GPIO before read
	GpioSetPadConfig(SIOM_ID3, INIT_SIOM_ID_PIN);	// Initial GPIO before read

	GpioGetInputValue(SIOM_ID0, &ReturnVal0);
	GpioGetInputValue(SIOM_ID1, &ReturnVal1);
	GpioGetInputValue(SIOM_ID2, &ReturnVal2);
	GpioGetInputValue(SIOM_ID3, &ReturnVal3);   
	SIOM_ID = (ReturnVal3 << 3) | (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;

	DEBUG ((EFI_D_INFO, "GetProjectPciEBifurcate PEI Return0 = %x, Return1 = %x, Return2 = %x, Return3 = %x, SIOM_ID = %x \n", ReturnVal0,ReturnVal1,ReturnVal2,ReturnVal3, SIOM_ID));

    if(SIOM_ID == 0x08)
      *(HardWareInfoData + 3) = 0x00;
    else
      *(HardWareInfoData + 3) = 0x01;

    // LanMacInfoData parameter data is used by 0x9E command
    // IPMI 0x9E command data:
    // Byte 0    : LAN number N  (exclude NumLan of 0x30 0x20 command)
    // Byte 1-6  : LAN port M MAC address
    // Byte 7-12 : LAN port M+1 MAC address

    *LanMacInfoData = 0x01;

	return EFI_SUCCESS;
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
//  UINT8   GbePciePortNum;    // Indicates the PCIe port qhen PCH LAN on board is connected.
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
    EFI_PEI_SMBUS2_PPI  *SmBus;
    PEI_IPMI_TRANSPORT_PPI      *IpmiTransportPpi;    
    EFI_SMBUS_DEVICE_ADDRESS    Address;
    UINT8   NVMeModeBitMap = 0, Data = 0, Retry, i;
    UINTN   Len = 1;
    UINT32  ReturnVal0 = 0, ReturnVal1 = 0, ReturnVal2 = 0, ReturnVal3 = 0, SIOM_ID = 0;
    UINT8   IpmiNvmeData[5] = {0x00, BPN_CPLD1_Address, 0x00, 0x08, 0x00};
    UINT8   ResponseDataSize = 10, ResponseData[10];
    
    DEBUG((EFI_D_INFO, "X11DPTB GetProjectPciEBifurcate PEI Start.\n"));
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
    AOC-MHIBE-m2C     : 0101b
*/
    if((SIOM_ID == 0x0C) || (SIOM_ID == 0x0A) || (SIOM_ID == 0x09) || (SIOM_ID == 0x07) || (SIOM_ID == 0x06))
    {
	    if(SetupData->ConfigIOU0[0] == 0xff) SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxx8xxx8;
	    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_1C_INDEX] = 1;	//SIOM
	    SetupData->SLOTPSP[SOCKET_0_INDEX + PORT_1C_INDEX] = 10;
		PcdSet8(PcdSmcMultiFuncLanChip1, 0x02);
		if((SIOM_ID == 0x0A) || (SIOM_ID == 0x0C)){
		    PcdSet32(PcdSmcOBLan1BDF, 0x00110010);
		} else {
		    PcdSet32(PcdSmcOBLan3BDF, 0x00110010);
		}
    } else {
	    if(SetupData->ConfigIOU0[0] == 0xff) SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxxxxx16;
    }
    if (SIOM_ID == 0x05) {
	    PcdSet32(PcdSmcOBLan1BDF, 0x000000E4);
    }

    if(SetupData->ConfigIOU1[0] == 0xff) SetupData->ConfigIOU1[0] = IIO_BIFURCATE_xxxxxx16;

    if(SIOM_ID == 0x01)
    {
        SetupData->PciePortDisable[9] = 1;  //10Gbe LAN map to CPU PCIe port 3A
        PcdSet32(PcdSmcOBLan1BDF, 0x00130000);
        PcdSet32(PcdSmcSLOT5BDF, 0x00130000);
    }
    else{
        SetupData->PciePortDisable[9] = 0;  //Other SIOM, disabled 3A
    }
    
    //BigTwin adapter detection                                      SXB2                                                    SXB1
    //                                               GPP_G8          GPP_G9          GPP_G10         |       GPP_G11         GPP_G12         GPP_G13
    //Not Present                                    1               1               1               |       1               1               1
    //BPN-ADP-6NVMe3-1UB (Rannie Chen)               0               1               1               |       x               x               x
    //BPN-ADP-6SATA3-1UB (Dican Zhu)                 1               1               1               |       1               1               0
    //BPN-ADP-6S3008N4-1UB (Chris Chang)             1               0               1               |       x               x               x
    //BPN-ADP-2M2-1UB (Grant Liu)                    x               x               x               |       0               1               1
    //BPN-ADP-12NVMe-2UB (Rannie Chen)               0               0               1               |       1               1               1
    //BPN-ADP-12S3216N4-2UB (Jerry Mou)              1               1               0               |       1               1               1
    //BPN-ADP-4TNS6-1UB (Hung Pham)                  1               0               0               |       1               1               1    
/*
    GpioGetInputValue(SXB1_ID0, &ReturnVal0);
    GpioGetInputValue(SXB1_ID1, &ReturnVal1);
    GpioGetInputValue(SXB1_ID2, &ReturnVal2);
    SIOM_ID = (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;

    if (SIOM_ID == 0x06)
    {
    	  if(SetupData->ConfigIOU2[0] == 0xff) SetupData->ConfigIOU2[0] = IIO_BIFURCATE_x4x4xxx8;
    	  SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_3D_INDEX] = 1;	//M.2
    	  SetupData->SLOTPSP[SOCKET_0_INDEX + PORT_3D_INDEX] = 4;
    } else {
    	  if(SetupData->ConfigIOU2[0] == 0xff) SetupData->ConfigIOU2[0] = IIO_BIFURCATE_xxx8xxx8;
    }
*/
    if(SetupData->ConfigIOU2[0] == 0xff) SetupData->ConfigIOU2[0] = IIO_BIFURCATE_x4x4xxx8;
    
    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_1A_INDEX] = 1;	//SIOM1
    SetupData->SLOTPSP[SOCKET_0_INDEX + PORT_1A_INDEX] = 5;
    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_2A_INDEX] = 1;	//SXB3
    SetupData->SLOTPSP[SOCKET_0_INDEX + PORT_2A_INDEX] = 1;
    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_3A_INDEX] = 1;	//Uplink
    SetupData->SLOTPSP[SOCKET_0_INDEX + PORT_3A_INDEX] = 11;
    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_3C_INDEX] = 1;	//M.2
    SetupData->SLOTPSP[SOCKET_0_INDEX + PORT_3C_INDEX] = 3;
    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_3D_INDEX] = 1;	//M.2
    SetupData->SLOTPSP[SOCKET_0_INDEX + PORT_3D_INDEX] = 4;
    
    GpioGetInputValue(SXB2_ID0, &ReturnVal0);
    GpioGetInputValue(SXB2_ID1, &ReturnVal1);
    GpioGetInputValue(SXB2_ID2, &ReturnVal2);
    SIOM_ID = (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;

    switch (SIOM_ID)
    {
        case 0x04:	//BPN-ADP-12NVMe-2UB
          SetupData->PcieHotPlugEnable = 1;
/*          
          if(SetupData->ConfigIOU2[0] == 0xff) SetupData->ConfigIOU2[0] = IIO_BIFURCATE_x4x4xxx8;
          SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_3D_INDEX] = 1;   //M.2
          SetupData->SLOTPSP[SOCKET_0_INDEX + PORT_3D_INDEX] = 4;
*/          
          if(SetupData->ConfigIOU0[1] == 0xff) SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxxxxx16;
          if(SetupData->ConfigIOU2[1] == 0xff) SetupData->ConfigIOU2[1] = IIO_BIFURCATE_xxxxxx16;
          PcdSet64(PcdSmcAOCLocation, 0x0000000000000000);
          IpmiNvmeData[0] = 0x05;
          break;
        case 0x06:	//BPN-ADP-6NVMe3-1UB
          SetupData->PcieHotPlugEnable = 1;
          if(SetupData->ConfigIOU0[1] == 0xff) SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxx8x4x4;
          if(SetupData->ConfigIOU2[1] == 0xff) SetupData->ConfigIOU2[1] = IIO_BIFURCATE_x4x4x4x4;
          //1A
          SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_1A_INDEX]    = 1;    //NVMe
          SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_1A_INDEX]    = 12;
          if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_1A_INDEX - 1] == 0) {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_1A_INDEX, VPP_PORT_0, 0x00, PCIEAIC_OCL_OWNERSHIP);
            NVMeModeBitMap &= ~BIT0;
          } else {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_1A_INDEX, VPP_PORT_0, 0x00, VMD_OWNERSHIP);
            NVMeModeBitMap |= BIT0;
		  }
          //1B
          SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_1B_INDEX]    = 1;    //NVMe
          SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_1B_INDEX]    = 13;
          if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_1B_INDEX - 1] == 0) {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_1B_INDEX, VPP_PORT_1, 0x00, PCIEAIC_OCL_OWNERSHIP);
            NVMeModeBitMap &= ~BIT1;
          } else {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_1B_INDEX, VPP_PORT_1, 0x00, VMD_OWNERSHIP);
            NVMeModeBitMap |= BIT1;
          }
          //3A
          SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_3A_INDEX]    = 1;    //NVMe
          SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_3A_INDEX]    = 14;
          if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_3A_INDEX - 1] == 0) {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3A_INDEX, VPP_PORT_0, 0x02, PCIEAIC_OCL_OWNERSHIP);
            NVMeModeBitMap &= ~BIT2;
          } else {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3A_INDEX, VPP_PORT_0, 0x02, VMD_OWNERSHIP);
            NVMeModeBitMap |= BIT2;
          }
          //3B
          SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_3B_INDEX]    = 1;    //NVMe
          SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_3B_INDEX]    = 15;
          if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_3B_INDEX - 1] == 0) {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3B_INDEX, VPP_PORT_1, 0x02, PCIEAIC_OCL_OWNERSHIP);
            NVMeModeBitMap &= ~BIT3;
          } else {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3B_INDEX, VPP_PORT_1, 0x02, VMD_OWNERSHIP);
            NVMeModeBitMap |= BIT3;
          }
          //3C
          SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_3C_INDEX]    = 1;    //NVMe
          SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_3C_INDEX]    = 16;
          if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_3C_INDEX - 1] == 0) {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3C_INDEX, VPP_PORT_0, 0x04, PCIEAIC_OCL_OWNERSHIP);
            NVMeModeBitMap &= ~BIT4;
          } else {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3C_INDEX, VPP_PORT_0, 0x04, VMD_OWNERSHIP);              
            NVMeModeBitMap |= BIT4;
		  }
          //3D
          SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_3D_INDEX]    = 1;    //NVMe
          SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_3D_INDEX]    = 17;
          if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_3D_INDEX - 1] == 0) {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3D_INDEX, VPP_PORT_1, 0x04, PCIEAIC_OCL_OWNERSHIP);
            NVMeModeBitMap &= ~BIT5;
          } else {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3D_INDEX, VPP_PORT_1, 0x04, VMD_OWNERSHIP);
            NVMeModeBitMap |= BIT5;
          }
          PcdSet32(PcdSmcSLOT7BDF, 0x00000000);
          PcdSet32(PcdSmcSLOT8BDF, 0x00000000);
          PcdSet32(PcdSmcOBSASBDF, 0x00000000);
          PcdSet64(PcdSmcAOCLocation, 0x0055000500000000);
          IpmiNvmeData[0] = 0x05;
          break;
        case 0x03:	//BPN-ADP-12S3216N4-2UB
/*
          if(SetupData->ConfigIOU2[0] == 0xff) SetupData->ConfigIOU2[0] = IIO_BIFURCATE_x4x4xxx8;
          SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_3D_INDEX] = 1;   //M.2
          SetupData->SLOTPSP[SOCKET_0_INDEX + PORT_3D_INDEX] = 4;
*/
        case 0x01:	//BPN-ADP-4TNS6-1UB
        case 0x05:	//BPN-ADP-6S3008N4-1UB
          SetupData->PcieHotPlugEnable = 1;
          if(SetupData->ConfigIOU0[1] == 0xff) SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxx8xxx8;
          if(SetupData->ConfigIOU2[1] == 0xff) SetupData->ConfigIOU2[1] = IIO_BIFURCATE_x4x4x4x4;
          //3A
          SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_3A_INDEX]    = 1;    //NVMe
          SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_3A_INDEX]    = 14;
          if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_3A_INDEX - 1] == 0) {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3A_INDEX, VPP_PORT_0, 0x00, PCIEAIC_OCL_OWNERSHIP);
            NVMeModeBitMap &= ~BIT0;
          } else {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3A_INDEX, VPP_PORT_0, 0x00, VMD_OWNERSHIP);
            NVMeModeBitMap |= BIT0;
          }
          //3B
          SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_3B_INDEX]    = 1;    //NVMe
          SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_3B_INDEX]    = 15;
          if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_3B_INDEX - 1] == 0) {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3B_INDEX, VPP_PORT_1, 0x00, PCIEAIC_OCL_OWNERSHIP);
            NVMeModeBitMap &= ~BIT1;
          } else {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3B_INDEX, VPP_PORT_1, 0x00, VMD_OWNERSHIP);
            NVMeModeBitMap |= BIT1;
          }
          //3C
          SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_3C_INDEX]    = 1;    //NVMe
          SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_3C_INDEX]    = 16;
          if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_3C_INDEX - 1] == 0) {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3C_INDEX, VPP_PORT_0, 0x02, PCIEAIC_OCL_OWNERSHIP);
            NVMeModeBitMap &= ~BIT2;
          } else {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3C_INDEX, VPP_PORT_0, 0x02, VMD_OWNERSHIP);
            NVMeModeBitMap |= BIT2;
          }
          //3D
          SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_3D_INDEX]    = 1;    //NVMe
          SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_3D_INDEX]    = 17;
          if (SetupData->VMDPortEnable[VMD_PORTS_PER_SOCKET + PORT_3D_INDEX - 1] == 0) {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3D_INDEX, VPP_PORT_1, 0x02, PCIEAIC_OCL_OWNERSHIP);
            NVMeModeBitMap &= ~BIT3;
          } else {
            OemEnableHotPlug(SetupData, SOCKET_1_INDEX + PORT_3D_INDEX, VPP_PORT_1, 0x02, VMD_OWNERSHIP);
            NVMeModeBitMap |= BIT3;
          }
          PcdSet32(PcdSmcSLOT7BDF, 0x00000000);
          PcdSet32(PcdSmcSLOT8BDF, 0x00000000);
          PcdSet32(PcdSmcOBNVMe1BDF, 0x00230000);
          PcdSet32(PcdSmcOBNVMe2BDF, 0x00230008);
          PcdSet32(PcdSmcOBNVMe3BDF, 0x00230010);
          PcdSet32(PcdSmcOBNVMe4BDF, 0x00230018);
          PcdSet32(PcdSmcOBNVMe5BDF, 0x88888888);
          PcdSet32(PcdSmcOBNVMe6BDF, 0x88888888);
          PcdSet64(PcdSmcAOCLocation, 0x0055000000000000);
          IpmiNvmeData[0] = 0x07;
          break;
	default:
          if(SetupData->ConfigIOU0[1] == 0xff) SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxx8xxx8;
          if(SetupData->ConfigIOU2[1] == 0xff) SetupData->ConfigIOU2[1] = IIO_BIFURCATE_xxxxxx16;
          PcdSet32(PcdSmcOBNVMe1BDF, 0x88888888);
          PcdSet32(PcdSmcOBNVMe2BDF, 0x88888888);
          PcdSet32(PcdSmcOBNVMe3BDF, 0x88888888);
          PcdSet32(PcdSmcOBNVMe4BDF, 0x88888888);
          PcdSet32(PcdSmcOBNVMe5BDF, 0x88888888);
          PcdSet32(PcdSmcOBNVMe6BDF, 0x88888888);
          PcdSet32(PcdSmcOBSASBDF, 0x00000000);
          break;
    }
    if(SetupData->ConfigIOU1[1] == 0xff) SetupData->ConfigIOU1[1] = IIO_BIFURCATE_xxxxxx16;
    SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_2A_INDEX]    = 1;	//SXB4
    SetupData->SLOTPSP[SOCKET_1_INDEX + PORT_2A_INDEX]    = 2;

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

    DEBUG((DEBUG_INFO, "NVMeModeBitMap = %x.\n", NVMeModeBitMap));

    Status = (*PeiServices)->LocatePpi(PeiServices, &gEfiPeiSmbus2PpiGuid, 0, NULL, &SmBus );
    DEBUG((-1, "LocatePpi Smbus2Ppi %r\n", Status));
    if(Status)  return Status;

    Status = (*PeiServices)->LocatePpi(PeiServices, &gEfiPeiIpmiTransportPpiGuid, 0, NULL, &IpmiTransportPpi);
    DEBUG((-1, "LocatePpi IpmiTransportPpi %r\n", Status));
    if(Status)  return Status;
/*
    Address.SmbusDeviceAddress = 0xE2>>1;
    Status = SmBus->Execute(SmBus, Address, 0, EfiSmbusWriteByte, 0, &Len, &Data);  // 0x00 -> offset 0
    DEBUG((DEBUG_INFO, "Stop PCIe MUX Status = %x.\n", Status));

    for ( i = 0; i < 25; i++ ) {
      IoWrite8( 0xEB, 0x55 );
      IoWrite8( 0xEB, 0xAA );
    }
    
    Data = 0x04;
    Status = SmBus->Execute(SmBus, Address, 0, EfiSmbusWriteByte, 0, &Len, &Data);  // 0x04 -> offset 0
    DEBUG((DEBUG_INFO, "Change PCIe MUX Status = %x.\n", Status));

    for ( i = 0; i < 250; i++ ) {
      IoWrite8( 0xEB, 0x55 );
      IoWrite8( 0xEB, 0xAA );
    }
*/
    Address.SmbusDeviceAddress = BPN_CPLD1_Address >> 1;
    for(Retry = 0; Retry < 3; Retry++) {
      Status = SmBus->Execute(SmBus, Address, 0x08, EfiSmbusWriteByte, 0, &Len, &NVMeModeBitMap);       // bitmap 7~0
      DEBUG((DEBUG_INFO, "Set NVMe Mode Status = %x.\n", Status));
      for ( i = 0; i < 250; i++ ) {
        IoWrite8( 0xEB, 0x55 );
        IoWrite8( 0xEB, 0xAA );
      }
      if(!Status) break;
    }

    if(Status) {
      IpmiNvmeData[4] = NVMeModeBitMap;
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
          DEBUG((DEBUG_INFO, "Set NVMe Mode via IPMI Status = %x.\n", Status));
          for ( i = 0; i < 250; i++ ) {
            IoWrite8( 0xEB, 0x55 );
            IoWrite8( 0xEB, 0xAA );
          }
          if(!Status) break;
      }
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
EFI_STATUS EFIAPI X11DPTBPeiDriverInit(
  IN EFI_FFS_FILE_HEADER *FfsHeader,
  IN EFI_PEI_SERVICES **PeiServices
  )
{
    EFI_PEI_PPI_DESCRIPTOR *mPpiList = NULL;
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_PEI_PCI_CFG2_PPI *PciCfg;
    EFI_PEI_CPU_IO_PPI          *CpuIo;

    DEBUG ((DEBUG_INFO, "X11DPTB ProjectPeiDriverInit.\n"));

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
	UINT8	NVMe_Redriver[6][25][2] ={
			{{0x06, 0x18}, 	//B0
			 {0x0F, 0x00}, {0x10, 0xAB}, {0x11, 0x02},
			 {0x16, 0x00}, {0x17, 0xAB}, {0x18, 0x02},
			 {0x1D, 0x00}, {0x1E, 0xAB}, {0x1F, 0x02},
			 {0x24, 0x00}, {0x25, 0xAB}, {0x26, 0x02},
			 {0x2C, 0x03}, {0x2D, 0xAE}, {0x2E, 0xE1},
			 {0x33, 0x03}, {0x34, 0xAE}, {0x35, 0xE1},
			 {0x3A, 0x03}, {0x3B, 0xAE}, {0x3C, 0xE1},
			 {0x41, 0x03}, {0x42, 0xAE}, {0x43, 0xE1}},
			{{0x06, 0x18}, 	//B2
			 {0x0F, 0x00}, {0x10, 0xAB}, {0x11, 0x02},
			 {0x16, 0x00}, {0x17, 0xAB}, {0x18, 0x02},
			 {0x1D, 0x00}, {0x1E, 0xAB}, {0x1F, 0x02},
			 {0x24, 0x00}, {0x25, 0xAB}, {0x26, 0x02},
			 {0x2C, 0x03}, {0x2D, 0xAE}, {0x2E, 0xE1},
			 {0x33, 0x03}, {0x34, 0xAE}, {0x35, 0xE1},
			 {0x3A, 0x03}, {0x3B, 0xAE}, {0x3C, 0xE1},
			 {0x41, 0x03}, {0x42, 0xAE}, {0x43, 0xE1}},
			{{0x06, 0x18}, 	//B4
			 {0x0F, 0x00}, {0x10, 0xAB}, {0x11, 0x02},
			 {0x16, 0x00}, {0x17, 0xAB}, {0x18, 0x02},
			 {0x1D, 0x00}, {0x1E, 0xAB}, {0x1F, 0x02},
			 {0x24, 0x00}, {0x25, 0xAB}, {0x26, 0x02},
			 {0x2C, 0x03}, {0x2D, 0xAE}, {0x2E, 0xE1},
			 {0x33, 0x03}, {0x34, 0xAE}, {0x35, 0xE1},
			 {0x3A, 0x03}, {0x3B, 0xAE}, {0x3C, 0xE1},
			 {0x41, 0x03}, {0x42, 0xAE}, {0x43, 0xE1}},
			{{0x06, 0x18}, 	//B6
			 {0x0F, 0x00}, {0x10, 0xAB}, {0x11, 0x02},
			 {0x16, 0x00}, {0x17, 0xAB}, {0x18, 0x02},
			 {0x1D, 0x00}, {0x1E, 0xAB}, {0x1F, 0x02},
			 {0x24, 0x00}, {0x25, 0xAB}, {0x26, 0x02},
			 {0x2C, 0x03}, {0x2D, 0xAE}, {0x2E, 0xE1},
			 {0x33, 0x03}, {0x34, 0xAE}, {0x35, 0xE1},
			 {0x3A, 0x03}, {0x3B, 0xAE}, {0x3C, 0xE1},
			 {0x41, 0x03}, {0x42, 0xAE}, {0x43, 0xE1}},
			{{0x06, 0x18}, 	//B8
			 {0x0F, 0x00}, {0x10, 0xAB}, {0x11, 0x02},
			 {0x16, 0x00}, {0x17, 0xAB}, {0x18, 0x02},
			 {0x1D, 0x00}, {0x1E, 0xAB}, {0x1F, 0x02},
			 {0x24, 0x00}, {0x25, 0xAB}, {0x26, 0x02},
			 {0x2C, 0x03}, {0x2D, 0xAE}, {0x2E, 0xE1},
			 {0x33, 0x03}, {0x34, 0xAE}, {0x35, 0xE1},
			 {0x3A, 0x03}, {0x3B, 0xAE}, {0x3C, 0xE1},
			 {0x41, 0x03}, {0x42, 0xAE}, {0x43, 0xE1}},
			{{0x06, 0x18}, 	//BA
			 {0x0F, 0x00}, {0x10, 0xAB}, {0x11, 0x02},
			 {0x16, 0x00}, {0x17, 0xAB}, {0x18, 0x02},
			 {0x1D, 0x00}, {0x1E, 0xAB}, {0x1F, 0x02},
			 {0x24, 0x00}, {0x25, 0xAB}, {0x26, 0x02},
			 {0x2C, 0x03}, {0x2D, 0xAE}, {0x2E, 0xE1},
			 {0x33, 0x03}, {0x34, 0xAE}, {0x35, 0xE1},
			 {0x3A, 0x03}, {0x3B, 0xAE}, {0x3C, 0xE1},
			 {0x41, 0x03}, {0x42, 0xAE}, {0x43, 0xE1}},
			};
	UINT8	Hybrid_Redriver[4][25][2] ={
			{{0x06, 0x18}, 	//B4
			 {0x0F, 0x00}, {0x10, 0xAB}, {0x11, 0x02},
			 {0x16, 0x00}, {0x17, 0xAB}, {0x18, 0x02},
			 {0x1D, 0x00}, {0x1E, 0xAB}, {0x1F, 0x02},
			 {0x24, 0x00}, {0x25, 0xAB}, {0x26, 0x02},
			 {0x2C, 0x03}, {0x2D, 0xAE}, {0x2E, 0xE1},
			 {0x33, 0x03}, {0x34, 0xAE}, {0x35, 0xE1},
			 {0x3A, 0x03}, {0x3B, 0xAE}, {0x3C, 0xE1},
			 {0x41, 0x03}, {0x42, 0xAE}, {0x43, 0xE1}},
			{{0x06, 0x18}, 	//B6
			 {0x0F, 0x00}, {0x10, 0xAB}, {0x11, 0x02},
			 {0x16, 0x00}, {0x17, 0xAB}, {0x18, 0x02},
			 {0x1D, 0x00}, {0x1E, 0xAB}, {0x1F, 0x02},
			 {0x24, 0x00}, {0x25, 0xAB}, {0x26, 0x02},
			 {0x2C, 0x03}, {0x2D, 0xAE}, {0x2E, 0xE1},
			 {0x33, 0x03}, {0x34, 0xAE}, {0x35, 0xE1},
			 {0x3A, 0x03}, {0x3B, 0xAE}, {0x3C, 0xE1},
			 {0x41, 0x03}, {0x42, 0xAE}, {0x43, 0xE1}},
	 	 	{{0x06, 0x18}, 	//B8
			 {0x0F, 0x00}, {0x10, 0xAB}, {0x11, 0x02},
			 {0x16, 0x00}, {0x17, 0xAB}, {0x18, 0x02},
			 {0x1D, 0x00}, {0x1E, 0xAB}, {0x1F, 0x02},
			 {0x24, 0x00}, {0x25, 0xAB}, {0x26, 0x02},
			 {0x2C, 0x03}, {0x2D, 0xAE}, {0x2E, 0xE1},
			 {0x33, 0x03}, {0x34, 0xAE}, {0x35, 0xE1},
			 {0x3A, 0x03}, {0x3B, 0xAE}, {0x3C, 0xE1},
			 {0x41, 0x03}, {0x42, 0xAE}, {0x43, 0xE1}},
 			{{0x06, 0x18}, 	//BA
			 {0x0F, 0x00}, {0x10, 0xAB}, {0x11, 0x02},
			 {0x16, 0x00}, {0x17, 0xAB}, {0x18, 0x02},
			 {0x1D, 0x00}, {0x1E, 0xAB}, {0x1F, 0x02},
			 {0x24, 0x00}, {0x25, 0xAB}, {0x26, 0x02},
			 {0x2C, 0x03}, {0x2D, 0xAE}, {0x2E, 0xE1},
			 {0x33, 0x03}, {0x34, 0xAE}, {0x35, 0xE1},
			 {0x3A, 0x03}, {0x3B, 0xAE}, {0x3C, 0xE1},
			 {0x41, 0x03}, {0x42, 0xAE}, {0x43, 0xE1}},
			};
	UINT8	SATA_Redriver[2][27][2] ={
			{{0x01, 0xC0}, {0x02, 0x01}, {0x06, 0x18}, //C0
			 {0x0F, 0x02}, {0x10, 0xAE}, {0x11, 0x80},
			 {0x16, 0x02}, {0x17, 0xAE}, {0x18, 0x80},			 
			 {0x1D, 0x02}, {0x1E, 0xAE}, {0x1F, 0x80},
			 {0x24, 0x02}, {0x25, 0xAE}, {0x26, 0x80},
			 {0x2C, 0x02}, {0x2D, 0xAE}, {0x2E, 0x80},
			 {0x33, 0x02}, {0x34, 0xAE}, {0x35, 0x80},
			 {0x3A, 0x02}, {0x3B, 0xAE}, {0x3C, 0x80},
			 {0x41, 0x02}, {0x42, 0xAE}, {0x43, 0x80}},
			{{0x01, 0xC0}, {0x02, 0x01}, {0x06, 0x18}, //C6
			 {0x0F, 0x02}, {0x10, 0xAE}, {0x11, 0x80},
			 {0x16, 0x02}, {0x17, 0xAE}, {0x18, 0x80},			 
			 {0x1D, 0x02}, {0x1E, 0xAE}, {0x1F, 0x80},
			 {0x24, 0x02}, {0x25, 0xAE}, {0x26, 0x80},
			 {0x2C, 0x02}, {0x2D, 0xAE}, {0x2E, 0x80},
			 {0x33, 0x02}, {0x34, 0xAE}, {0x35, 0x80},
			 {0x3A, 0x02}, {0x3B, 0xAE}, {0x3C, 0x80},
			 {0x41, 0x02}, {0x42, 0xAE}, {0x43, 0x80}},
			};
	UINTN	i, j, Len;
	UINT8	Data; 
	UINT32	ReturnVal0 = 0, ReturnVal1 = 0, ReturnVal2 = 0, ReturnVal3 = 0, SIOM_ID = 0;

	Status = (*PeiServices)->LocatePpi(PeiServices,	&gEfiPeiSmbus2PpiGuid, 0, NULL,	&SmBus );
//	ASSERT_PEI_ERROR( PeiServices, Status);

//BigTwin adapter detection                                      SXB2                                                    SXB1
//                                               GPP_G8          GPP_G9          GPP_G10         |       GPP_G11         GPP_G12         GPP_G13
//Not Present                                    1               1               1               |       1               1               1
//BPN-ADP-6NVMe3-1UB (Rannie Chen)               0               1               1               |       x               x               x
//BPN-ADP-6SATA3-1UB (Dican Zhu)                 1               1               1               |       1               1               0
//BPN-ADP-6S3008N4-1UB (Chris Chang)             1               0               1               |       x               x               x
//BPN-ADP-2M2-1UB (Grant Liu)                    x               x               x               |       0               1               1
//BPN-ADP-12NVMe-2UB (Rannie Chen)               0               0               1               |       1               1               1
//BPN-ADP-12S3216N4-2UB (Jerry Mou)              1               1               0               |       1               1               1
//BPN-ADP-4TNS6-1UB (Hung Pham)                  1               0               0               |       1               1               1	

	GpioGetInputValue(SXB2_ID0, &ReturnVal0);
	GpioGetInputValue(SXB2_ID1, &ReturnVal1);
	GpioGetInputValue(SXB2_ID2, &ReturnVal2);
	SIOM_ID = (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;

	switch (SIOM_ID)
	{
		case 0x06:	//BPN-ADP-6NVMe3-1UB
			DEBUG((DEBUG_INFO, "BPN-ADP-6NVMe3-1UB detected.\n"));
			Len = 1;

			Address.SmbusDeviceAddress = 0xE2>>1;
			Data = 0x00;
			Status = SmBus->Execute(SmBus, Address, 0, EfiSmbusWriteByte, 0, &Len, &Data);  // 0x00 -> offset 0
			DEBUG((DEBUG_INFO, "Stop PCIe MUX Status = %x.\n",Status));

            for ( i = 0; i < 25; i++ ) {
              IoWrite8( 0xEB, 0x55 );
              IoWrite8( 0xEB, 0xAA );
            }
			
            Data = 0x04;			
            Status = SmBus->Execute(SmBus, Address, 0, EfiSmbusWriteByte, 0, &Len, &Data);  // 0x04 -> offset 0
            DEBUG((DEBUG_INFO, "Change PCIe MUX Status = %x.\n",Status));

			for ( i = 0; i < 250; i++ ) {
			  IoWrite8( 0xEB, 0x55 );
			  IoWrite8( 0xEB, 0xAA );
			}

			Address.SmbusDeviceAddress = 0xB0>>1;
			for (i=0; i<6; i++) {
				for(j=0; j<25; j++) {
					Status = SmBus->Execute(SmBus, Address, NVMe_Redriver[i][j][0], EfiSmbusWriteByte, 0, &Len, &NVMe_Redriver[i][j][1]);
					DEBUG((DEBUG_INFO, "Redriver %x offset %x Status = %x.\n", Address.SmbusDeviceAddress, NVMe_Redriver[i][j][0], Status));
				}
				Address.SmbusDeviceAddress++;
			}
			break;
		case 0x01:	//BPN-ADP-4TNS6-1UB
		case 0x03:	//BPN-ADP-12S3216N4-2UB
		case 0x05:	//BPN-ADP-6S3008N4-1UB
			DEBUG((DEBUG_INFO, "BPN-ADP-6S3008N4-1UB/BPN-ADP-12S3216N4-2UB/BPN-ADP-4TNS6-1UB detected.\n"));
			Len = 1;

			Address.SmbusDeviceAddress = 0xE2>>1;
			Data = 0x00;
			Status = SmBus->Execute(SmBus, Address, 0, EfiSmbusWriteByte, 0, &Len, &Data);  // 0x00 -> offset 0
            DEBUG((DEBUG_INFO, "Stop PCIe MUX Status = %x.\n",Status));

            for ( i = 0; i < 25; i++ ) {
              IoWrite8( 0xEB, 0x55 );
              IoWrite8( 0xEB, 0xAA );
            }
            
            Data = 0x04;            
            Status = SmBus->Execute(SmBus, Address, 0, EfiSmbusWriteByte, 0, &Len, &Data);  // 0x04 -> offset 0
            DEBUG((DEBUG_INFO, "Change PCIe MUX Status = %x.\n",Status));

			for ( i = 0; i < 250; i++ ) {
			  IoWrite8( 0xEB, 0x55 );
			  IoWrite8( 0xEB, 0xAA );
			}

			Address.SmbusDeviceAddress = 0xB4>>1;
			for (i=0; i<4; i++) {
				for(j=0; j<25; j++) {
					Status = SmBus->Execute(SmBus, Address, Hybrid_Redriver[i][j][0], EfiSmbusWriteByte, 0, &Len, &Hybrid_Redriver[i][j][1]);
					DEBUG((DEBUG_INFO, "Redriver %x offset %x Status = %x.\n", Address.SmbusDeviceAddress, Hybrid_Redriver[i][j][0], Status));
				}
				Address.SmbusDeviceAddress++;
			}
			break;
		default:
			break;
	}

	GpioGetInputValue(SXB1_ID0, &ReturnVal0);
	GpioGetInputValue(SXB1_ID1, &ReturnVal1);
	GpioGetInputValue(SXB1_ID2, &ReturnVal2);
	SIOM_ID = (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;
	
	switch (SIOM_ID)
	{
		case 0x03:
			DEBUG((DEBUG_INFO, "BPN-ADP-6SATA3-1UB detected.\n"));
			Len = 1;
			Address.SmbusDeviceAddress = 0xC0>>1;
			for (i=0; i<2; i++) {
				for(j=0; j<27; j++) {
					Status = SmBus->Execute(SmBus, Address, SATA_Redriver[i][j][0], EfiSmbusWriteByte, 0, &Len, &SATA_Redriver[i][j][1]);
					DEBUG((DEBUG_INFO, "Redriver %x offset %x Status = %x.\n", Address.SmbusDeviceAddress, SATA_Redriver[i][j][0], Status));
				}
				Address.SmbusDeviceAddress+=3;
			}
			break;
		case 0x06:
			DEBUG((DEBUG_INFO, "BPN-ADP-2M2-1UB detected.\n"));
            break;
		default:
			break;
	}
	return Status;
}
