//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
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
//#include <Efi.h>
//#include <Pei.h>
#include <token.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
//#include <Hob.h>
//#include <AmiPeiLib.h>
//#include <AmiCspLib.h>
//#include <ppi\ReadOnlyVariable2.h>
#include "X11DPTSGIPeiDriver.h"
#include "SmcCspLibBmc.h"
#include <GpioConfig.h>
#include <Library\GpioLib.h>
#include <GpioPinsSklH.h>
#include <Guid/SocketIioVariable.h>
#include <Ppi\ReadOnlyVariable2.h>
#if SmcRiserCard_SUPPORT
#include "../Module/SmcRiserCard/SmcRiserCardPei.h"
EFI_GUID gSmcRiserCardPeiProtocolGuid = EFI_SMCRISERCARD_PEI_DRIVER_PROTOCOL_GUID;
#endif

EFI_GUID gProjectOwnPeiProtocolGuid = EFI_PROJECT_OWN_PEI_DRIVER_PROTOCOL_GUID;

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

#define EFI_DEADLOOP()    { volatile int __iii; __iii = 1; while (__iii); } 

#define PEI_ASSERT(PeiServices,Condition) if(!(Condition)) { \
	DEBUG((EFI_D_INFO, "ASSERT in %s on %i: %s\n",__FILE__, __LINE__));\
	EFI_DEADLOOP() \
	}
#define ASSERT_PEI_ERROR(PeiServices,Status)  PEI_ASSERT(PeiServices,!EFI_ERROR(Status))




//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  IsA1B1Node
//
// Description :  determine A1 or B1 node.
//
// Parameters:  VOID
//
//
// Returns     :  BOOLEAN   True :  A1 or B1 side
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
BOOLEAN  IsA1B1Node(
VOID
)
{

   BOOLEAN             A1B1Node = FALSE;
   UINT32             Value = 0;
//    DEBUG ((DEBUG_INFO, "PeiProjectSystemConfiguration.\n"));
   DEBUG((EFI_D_INFO, "Check IsA1B1Node ent .\n"));

   GpioGetInputValue(SGI_NODE_1_PIN, &Value);
   //Check GPP_C_12     1 : A1/B1 side  
    if (Value) {
        //Disable NMI2SMI_EN for prevent NMI trigger again before service.
        DEBUG((EFI_D_INFO, "IsA1B1Node .\n"));
        A1B1Node = TRUE;
    }  

    return A1B1Node;
}

/*-----------------12/23/2014 3:06PM----------------
 * Coding for every platform
 * --------------------------------------------------*/
//HSX_PER_LANE_EPARAM_LINK_INFO  X11DPTSGIPerLanesEparamTable[] = {
//};
//UINT32 X11DPTSGIPerLanesEparamTableSize = sizeof (X11DPTSGIPerLanesEparamTable);

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

    *LanMacInfoData = 0x0;

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
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
     SOCKET_IIO_CONFIGURATION      SocketIioConfiguration;
    UINTN		VariableSize = sizeof(SOCKET_IIO_CONFIGURATION);    
#if SmcRiserCard_SUPPORT
    PEI_SMCRISERCARD_PROTOCOL	*mSmcRiserCardPeiProtocol = NULL;
#endif
    DEBUG((EFI_D_INFO, "X11DPT SGI PEI GetProjectPciEBifurcate Start.\n"));


    Status = (*PeiServices)->LocatePpi(
		PeiServices,
		&gEfiPeiReadOnlyVariable2PpiGuid,
		0,
		NULL,
		&ReadOnlyVariable);

    if(!EFI_ERROR(Status)){
	Status = ReadOnlyVariable->GetVariable(
			ReadOnlyVariable,
		    SOCKET_IIO_CONFIGURATION_NAME,
		    &gEfiSocketIioVariableGuid,
			NULL,
			&VariableSize,
			&SocketIioConfiguration);

    }
    //
    // Onboard NVMe
    //
//	SetupData->ConfigIOU0[0] = IIO_BIFURCATE_x4x4xxx8;
//	SetupData->ConfigIOU0[1] = IIO_BIFURCATE_x4x4xxx8;

#if SmcRiserCard_SUPPORT
    Status = (*PeiServices)->LocatePpi(
		    PeiServices,
    		    &gSmcRiserCardPeiProtocolGuid,
    		    0,
    		    NULL,
    		    &mSmcRiserCardPeiProtocol);

    mSmcRiserCardPeiProtocol->GetSmcRiserCardPcieBifurcation(SetupData);
    DEBUG((-1, "GetSuperMPcieBifurcation End.\n"));
#endif
   //Set PcdSGINodeIsA1B1 
    if ( IsA1B1Node() ){
      PcdSetBool(PcdSGINodeIsA1B1,TRUE);
      PcdSet32(PcdSmcSLOT1BDF,0);
      DEBUG ((DEBUG_INFO, "A1B1 node set PCD TRUE dis A0 riser\n"));  
    } else {
      PcdSetBool(PcdSGINodeIsA1B1,FALSE);
      PcdSet32(PcdSmcSLOT2BDF,0);
      DEBUG ((DEBUG_INFO, "A0B0 node set PCD as FALSE dis A1 riser\n"));   
    }
   if ( IsA1B1Node() ) {
      DEBUG((-1, "A1 Node\n")); 
   // A1 side 
       if(SetupData->ConfigIOU0[0] == 0xff){
       	 SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxxxxx16;
       }
       if(SetupData->ConfigIOU1[0] == 0xff){
         SetupData->ConfigIOU1[0] = IIO_BIFURCATE_x4x4x4x4;

       }
       if(SetupData->ConfigIOU2[0] == 0xff){
         SetupData->ConfigIOU2[0] = IIO_BIFURCATE_xxxxxx16;
       }
       //Second CPU 
       if(SetupData->ConfigIOU0[1] == 0xff){
         SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxxxxx16;
       	 SetupData->HidePEXPMenu[1+NUMBER_PORTS_PER_SOCKET] = 1;//hide unused port
       }
       if(SetupData->ConfigIOU1[1] == 0xff){
         SetupData->ConfigIOU1[1] = IIO_BIFURCATE_x4x4x4x4;
       }
       if(SetupData->ConfigIOU2[1] == 0xff){
         SetupData->ConfigIOU2[1] = IIO_BIFURCATE_xxxxxx16;     	
       }
       if(SetupData->ConfigMCP0[0] == 0xff){
       	  SetupData->ConfigMCP0[0] = IIO_BIFURCATE_xxxxxx16;
          SetupData->HidePEXPMenu[13] = 1;
       }
       if(SetupData->ConfigMCP1[0] == 0xff){
           SetupData->ConfigMCP1[0] = IIO_BIFURCATE_xxxxxx16;
           SetupData->HidePEXPMenu[17] = 1;
       }
       if(SetupData->ConfigMCP0[1] == 0xff){
           SetupData->ConfigMCP0[1] = IIO_BIFURCATE_xxxxxx16;
           SetupData->HidePEXPMenu[34] = 1;
       }
       if(SetupData->ConfigMCP1[1] == 0xff){
           SetupData->ConfigMCP1[1] = IIO_BIFURCATE_xxxxxx16;
           SetupData->HidePEXPMenu[38] = 1;
       }
      
       // VPP for On board NVMe setting  
       // A1 side
       //   Cpu0 port2A 
       SetupData->VppEnable[5]    = 1;
       SetupData->VppPort[5]      = 0;
       SetupData->VppAddress[5]   = 0;   
//       SetupData->VMDEnabled[1]        = 1; // Pstack 1 IOU1
//       SetupData->VMDPortEnable[4]     = 1; // port 2A 
//       SetupData->VMDHotPlugEnable[1]  = 1; // Pstack 1 IOU1 
       SetupData->SLOTIMP[5]           = 1;
       SetupData->SLOTPSP[5]           = 2;  // slot number
       SetupData->SLOTHPCAP[5]         = 1;
       SetupData->SLOTHPSUP[5]         = 1;
       SetupData->SLOTPIP[5]           = 1;
       SetupData->SLOTAIP[5]           = 1;
//       SetupData->SLOTPCP[5]           = 1;
//       SetupData->SLOTABP[5]           = 1;

       //  CPU1 Port2A
	   SetupData->VppEnable[5+NUMBER_PORTS_PER_SOCKET*1]   = 1;
       SetupData->VppPort[5+NUMBER_PORTS_PER_SOCKET*1]     = 0;
       SetupData->VppAddress[5+NUMBER_PORTS_PER_SOCKET*1]  = 0; 
//       SetupData->VMDEnabled[1+VMD_STACK_PER_SOCKET]        = 1; // Pstack 1 IOU1
//       SetupData->VMDPortEnable[4+VMD_PORTS_PER_SOCKET]     = 1; // port 2A 
//       SetupData->VMDHotPlugEnable[1+VMD_STACK_PER_SOCKET]  = 1; // Pstack 1 IOU1 
       SetupData->SLOTIMP[5+NUMBER_PORTS_PER_SOCKET]        = 1;
       SetupData->SLOTPSP[5+NUMBER_PORTS_PER_SOCKET]        = 1;  // slot number
       SetupData->SLOTHPCAP[5+NUMBER_PORTS_PER_SOCKET]      = 1;
       SetupData->SLOTHPSUP[5+NUMBER_PORTS_PER_SOCKET]      = 1;
       SetupData->SLOTPIP[5+NUMBER_PORTS_PER_SOCKET]        = 1;
       SetupData->SLOTAIP[5+NUMBER_PORTS_PER_SOCKET]        = 1;
//       SetupData->SLOTPCP[5+NUMBER_PORTS_PER_SOCKET]        = 1;
//       SetupData->SLOTABP[5+NUMBER_PORTS_PER_SOCKET]        = 1;  
       
       PcdSet32(PcdSmcOBNVMe1BDF,0x00220000);
       PcdSet32(PcdSmcOBNVMe2BDF,0x00120000);
   } else {
    //A0 /B0 side
        DEBUG((-1, "A0 Node\n")); 
       if(SetupData->ConfigIOU0[0] == 0xff){
       	 SetupData->ConfigIOU0[0] = IIO_BIFURCATE_x4x4x4x4;
 	
       }
       if(SetupData->ConfigIOU1[0] == 0xff){
         SetupData->ConfigIOU1[0] = IIO_BIFURCATE_xxxxxx16;
         SetupData->HidePEXPMenu[5] = 1; //hide unused port 
       }
       if(SetupData->ConfigIOU2[0] == 0xff){
         SetupData->ConfigIOU2[0] = IIO_BIFURCATE_xxxxxx16;
   
       }
       //Second CPU 
       if(SetupData->ConfigIOU0[1] == 0xff){
         SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxxxxx16;
       }
       if(SetupData->ConfigIOU1[1] == 0xff){
         SetupData->ConfigIOU1[1] = IIO_BIFURCATE_x4x4x4x4;
       }
       if(SetupData->ConfigIOU2[1] == 0xff){
         SetupData->ConfigIOU2[1] = IIO_BIFURCATE_xxxxxx16;     	
       }
       
       if(SetupData->ConfigMCP0[0] == 0xff){
       	SetupData->ConfigMCP0[0] = IIO_BIFURCATE_xxxxxx16;
        SetupData->HidePEXPMenu[13] = 1;
       }
       if(SetupData->ConfigMCP1[0] == 0xff){
           SetupData->ConfigMCP1[0] = IIO_BIFURCATE_xxxxxx16;
           SetupData->HidePEXPMenu[17] = 1;
       }
       if(SetupData->ConfigMCP0[1] == 0xff){
           SetupData->ConfigMCP0[1] = IIO_BIFURCATE_xxxxxx16;
          SetupData->HidePEXPMenu[34] = 1;
       }
       if(SetupData->ConfigMCP1[1] == 0xff){
           SetupData->ConfigMCP1[1] = IIO_BIFURCATE_xxxxxx16;
          SetupData->HidePEXPMenu[38] = 1;
       }  
        // VPP for On board NVMe setting  
       // A0 side   
       //  Cpu0 port1A
       SetupData->VppEnable[5]    = 1;
       SetupData->VppPort[5]      = 0;
       SetupData->VppAddress[5]   = 0; 
//       SetupData->VMDEnabled[0]        = 1; // Pstack 0 IOU0
//       SetupData->VMDPortEnable[0]     = 1; // port 1A 
//       SetupData->VMDHotPlugEnable[0]  = 1; // Pstack 0 IOU0   
       SetupData->SLOTIMP[1]           = 1;
       SetupData->SLOTPSP[1]           = 2;  // slot number       
       SetupData->SLOTHPCAP[1]         = 1;
       SetupData->SLOTHPSUP[1]         = 1;
       SetupData->SLOTPIP[1]           = 1;
       SetupData->SLOTAIP[1]           = 1;
//       SetupData->SLOTPCP[1]           = 1;
//       SetupData->SLOTABP[1]           = 1; 


       
       //  CPU1 Port2A
       SetupData->VppEnable[5+NUMBER_PORTS_PER_SOCKET*1]   = 1;
       SetupData->VppPort[5+NUMBER_PORTS_PER_SOCKET*1]     = 0;
       SetupData->VppAddress[5+NUMBER_PORTS_PER_SOCKET*1]  = 0;  
//       SetupData->VMDEnabled[1+VMD_STACK_PER_SOCKET]           = 1; // Pstack 1 IOU1
//       SetupData->VMDPortEnable[4+VMD_PORTS_PER_SOCKET]        = 1; // port 2A 
//       SetupData->VMDHotPlugEnable[1+VMD_STACK_PER_SOCKET]     = 1; // Pstack 1 IOU1    
       SetupData->SLOTIMP[5+NUMBER_PORTS_PER_SOCKET]           = 1;
       SetupData->SLOTPSP[5+NUMBER_PORTS_PER_SOCKET]           = 1;  // slot number
       SetupData->SLOTHPCAP[5+NUMBER_PORTS_PER_SOCKET]         = 1;
       SetupData->SLOTHPSUP[5+NUMBER_PORTS_PER_SOCKET]         = 1;
       SetupData->SLOTPIP[5+NUMBER_PORTS_PER_SOCKET]           = 1;
       SetupData->SLOTAIP[5+NUMBER_PORTS_PER_SOCKET]           = 1;
//       SetupData->SLOTPCP[5+NUMBER_PORTS_PER_SOCKET]           = 1;
//       SetupData->SLOTABP[5+NUMBER_PORTS_PER_SOCKET]           = 1;   

            
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
EFI_STATUS EFIAPI X11DPTSGIPeiDriverInit(
  IN EFI_FFS_FILE_HEADER *FfsHeader,
  IN EFI_PEI_SERVICES **PeiServices
  )
{
	EFI_PEI_PPI_DESCRIPTOR *mPpiList = NULL;
	EFI_STATUS Status;

	DEBUG ((DEBUG_INFO, "X11DPTSGI ProjectPeiDriverInit.\n"));
 
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
       
    DEBUG ((DEBUG_INFO, "Exit X11DPTSGI ProjectPeiDriverInit.\n"));

    return EFI_SUCCESS;
}
