//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Jimmy Chiu
//    Date:     Nov/22/2016
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
#include <Library/GpioLib.h>
#include "X11DDWPeiDriver.h"
#include "SmcCspLibBmc.h"

#if ONBOARD_LAN_DISABLE_BY_SETUP_ITEM
#include <Ppi/ReadOnlyVariable2.h>
#include <PEI.h>
#include "Setup.h"
#endif

#if SmcRiserCard_SUPPORT
#include "../Module/SmcRiserCard/SmcRiserCardPei.h"
EFI_GUID gSmcRiserCardPeiProtocolGuid = EFI_SMCRISERCARD_PEI_DRIVER_PROTOCOL_GUID;
#endif

#if SmcAOC_SUPPORT
#include "../Module/SmcAOC/SmcAOCPei.h"
EFI_GUID gSmcAOCPeiProtocolGuid = EFI_SMC_AOC_PEI_PROTOCOL_GUID;
#endif

#include <Library/IoLib.h>  // JPME2_Jumpless_SUPPORT
#include <Sps.h>  // JPME2_Jumpless_SUPPORT
#include "MeAccess.h"  // JPME2_Jumpless_SUPPORT


#include <PchAccess.h>  // JPME2_Jumpless_SUPPORT
#include <SspTokens.h>
#include <SmcLibCommon.h>
#include <Guid/SetupVariable.h> //Supermicro

#include <Ppi/IPMITransportPpi.h>

/*-----------------12/23/2014 3:06PM----------------
 * Coding for every platform
 * --------------------------------------------------*/
//HSX_PER_LANE_EPARAM_LINK_INFO  X11DDWPerLanesEparamTable[] = {
//};
//UINT32 X11DDWPerLanesEparamTableSize = sizeof (X11DDWPerLanesEparamTable);

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
    IN	EFI_PEI_SERVICES	**PeiServices,
    IN	PROJECT_PEI_DRIVER_PROTOCOL	*This
)
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
    IN	EFI_PEI_SERVICES	**PeiServices,
    IN	PROJECT_PEI_DRIVER_PROTOCOL	*This
)
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
    IN	EFI_PEI_SERVICES	**PeiServices,
    IN	PROJECT_PEI_DRIVER_PROTOCOL	*This
)
{
    UINT32	ReturnVal = 1;
	
    //ReturnVal = CheckBMCPresent();

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
    IN	EFI_PEI_SERVICES	**PeiServices,
    IN	PROJECT_PEI_DRIVER_PROTOCOL	*This,
    OUT	UINT8	*SystemConfigurationData
)
{
    UINT8	i;

    DEBUG((EFI_D_INFO, "PeiProjectSystemConfiguration.\n"));

    for(i = 0; i < 10; i++)
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
    IN	EFI_PEI_SERVICES	**PeiServices,
    IN	PROJECT_PEI_DRIVER_PROTOCOL	*This,
    OUT	UINT8	*HardWareInfoData,
    OUT	UINT8	*LanMacInfoData
)
{
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
    IN	EFI_PEI_SERVICES	**PeiServices,
    IN	PROJECT_PEI_DRIVER_PROTOCOL	*This,
    OUT	UINT32	*SizeOfTable,
    OUT	ALL_LANES_EPARAM_LINK_INFO	**Ptr
)
{
    return TRUE;
}

BOOLEAN
MeIsInManufacturingMode (
  VOID
  )
{
    SPS_MEFS1  MeFs1;
    SPS_MEFS2  MeFs2;
    
    //
    // Note: We are checking if ME is in Recovery Mode actually.
    //       This follows the current status of Purley CRB.
    //       This may change in the future.
    //
    
    MeFs1.UInt32 = HeciPciRead32(0x40);
    if(MeFs1.UInt32 == 0xFFFFFFFF) MeFs1.UInt32 = 0;
    if (MeFs1.Bits.CurrentState != MEFS1_CURSTATE_RECOVERY) return FALSE;
    MeFs2.UInt32 = HeciPciRead32(0x48);
    if(MeFs2.UInt32 == 0xFFFFFFFF) MeFs2.UInt32 = 0;
    if (MeFs2.Bits.RecoveryCause != MEFS2_RCAUSE_MFGJMPR) return FALSE;
    
    return TRUE;
}

VOID Issue_Global_Reboot(
  IN EFI_PEI_SERVICES          **PeiServices,
  IN UINT32	tokenID,
  IN UINT8	CmosValue
)
{
    UINT32 i;
    
    SetCmosTokensValue(tokenID, CmosValue);
    
    i = *(UINT32*)MmPciAddress(0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_ETR3);
    i = i | B_PCH_PMC_ETR3_CF9GR;
    *(UINT32*)MmPciAddress(0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_ETR3) = i;
    IoWrite8(R_PCH_RST_CNT, V_PCH_RST_CNT_FULLRESET);
}

VOID Check_ME_Reboot(
  IN EFI_PEI_SERVICES          **PeiServices
)
{
    BOOLEAN  MeInManuMode = FALSE;
    UINT32   ReadGPIO32;
    UINT8    CMOS50, CMOS51, CMOS52;
    
    //use ipmi to set GPION6 start
    EFI_STATUS	Status = EFI_SUCCESS;
    PEI_IPMI_TRANSPORT_PPI  *IpmiTransportPpi;
	UINT8       Index;
	UINT8		CmdData[5]={0xc2, 0xd6, 0x0, 0x1, 0x0};
    UINT8   ResponseDataSize = 10, ResponseData[10];
    //use ipmi to set GPION6 end

    Status = (*PeiServices)->LocatePpi(PeiServices, &gEfiPeiIpmiTransportPpiGuid, 0, NULL, &IpmiTransportPpi);

    DEBUG((-1, "Project PEI Locating IPMI Transport protocol Status = %r\n", Status));

    ReadGPIO32 = MmioRead32(JPME2_GPIO_CFG_MMIO_Address);
    // MEI Host Firmware Status
    MeInManuMode = MeIsInManufacturingMode();
    CMOS50 = GetCmosTokensValue (Q_MEUD);
    CMOS51 = GetCmosTokensValue (Q_MEUD_Reboot_CHK);
    CMOS52 = GetCmosTokensValue (Q_MEUD_HW_WDT);

    if ( (CMOS50 == 0x5A) && ((CMOS51 & 0x0A) != 0x0A))
    {
        if (!MeInManuMode)
        {
#if JPME2_GPIO_CFG_MMIO_Address == 0xFFFFFFFF
        	
        	for(Index = 0; Index < 3; Index++){
        	    
                Status = IpmiTransportPpi->SendIpmiCommand( //Set GPION6 to high 30 70 C2 D6 0 1 0
                                IpmiTransportPpi,
                                0x30,       // 0x30
                                BMC_LUN,
                                0x70,
                                CmdData,
                                sizeof(CmdData),
                                ResponseData,
                                &ResponseDataSize);

        	    DEBUG ((EFI_D_LOAD, "IPMI set JPME2 high Status =%r\n", Status));
        	    if(Status == EFI_SUCCESS)
        		        break;
        	}
        	
#else        	
            MmioWrite32(JPME2_GPIO_CFG_MMIO_Address, 0x04000201);
            ReadGPIO32 = MmioRead32(JPME2_GPIO_CFG_MMIO_Address);  // Take register effect.
#endif
            Issue_Global_Reboot(PeiServices, Q_MEUD_Reboot_CHK, 0x0A);  // set Q_MEUD_Reboot_CHK
        }
    }
    
    if ( (CMOS50 == 0x5A) && ((CMOS51 & 0x0A) == 0x0A))
    {
        // if Manufacturing mode is Enabled, fill 08h into CMOS 51h. 
        if (MeInManuMode) SetCmosTokensValue(Q_MEUD_Reboot_CHK, 0x08);
    }
    
    CMOS51 = GetCmosTokensValue (Q_MEUD_Reboot_CHK);
    if (((CMOS50 == 00) && (CMOS51 == 00)) || ((CMOS50 == 0x5A) && ((CMOS51 & 0x10) == 0x10)))
    {
        if (MeInManuMode)
        {
#if JPME2_GPIO_CFG_MMIO_Address == 0xFFFFFFFF
        	
        	CmdData[3]=0x0; //Set GPION6 to low 30 70 C2 D6 0 0 0
        	
        	for(Index = 0; Index < 3; Index++){
        	    
                Status = IpmiTransportPpi->SendIpmiCommand(
                                IpmiTransportPpi,
                                0x30,       // 0x30
                                BMC_LUN,
                                0x70,       // master write-read
                                CmdData,
                                sizeof(CmdData),
                                ResponseData,
                                &ResponseDataSize);

        	    DEBUG ((EFI_D_LOAD, "IPMI set JPME2 low Status =%r\n", Status));
        	    if(Status == EFI_SUCCESS)
        		        break;
        	}
        	
#else
            MmioWrite32(JPME2_GPIO_CFG_MMIO_Address, 0x04000200);
            ReadGPIO32 = MmioRead32(JPME2_GPIO_CFG_MMIO_Address);  // Take register effect.
#endif
            SetCmosTokensValue(Q_MEUD, 0x00);
            if(CMOS52 <= 5){  // WD Flag for the ME enter by JPME2 HW jumper.
                SetCmosTokensValue (Q_MEUD_HW_WDT, (CMOS52 + 1)); // Increase time out flag.
                Issue_Global_Reboot(PeiServices, Q_MEUD_Reboot_CHK, 00); // set Q_MEUD_Reboot_CHK
            }
        }
    }
    if(CMOS52 > 5){
        SetCmosTokensValue(Q_MEUD, 0xFF);  // Clear the CMOS50 flag 
        SetCmosTokensValue(Q_MEUD_Reboot_CHK, 0xFF);  // Clear the CMOS51 flag 
        SetCmosTokensValue(Q_MEUD_HW_WDT, 0);  // Clear the WD flag
    }
    DEBUG((-1, "Check_ME_Reboot end\n"));
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
    IN	EFI_PEI_SERVICES	**PeiServices,
    IN	PROJECT_PEI_DRIVER_PROTOCOL	*This,
    IN OUT	UINT8	PLATFORM_PCH_DATA[10]
)
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
    UINT8	DefaultIOU0[2], DefaultIOU1[2], DefaultIOU2[2];
    UINT64      OnboardNVMe;
#if SmcRiserCard_SUPPORT
    PEI_SMCRISERCARD_PROTOCOL	*mSmcRiserCardPeiProtocol = NULL;
    SMC_SLOT_INFO               SXB1_SLOT_INFO_Tbl[MAX_SLOT];
    SMC_SLOT_INFO               SXB2_SLOT_INFO_Tbl[MAX_SLOT];
#endif
#if ONBOARD_LAN_DISABLE_BY_SETUP_ITEM
    UINTN       VarSize;
    SETUP_DATA	Setup10GbE;
    EFI_GUID	SetupGuid = SETUP_GUID;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
#endif
#if SmcAOC_SUPPORT
    PEI_SMC_AOC_PROTOCOL        *mSmcAOCPeiProtocol = NULL;
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

    // Onboard NVMe
    SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxx8x4x4;
    SetupData->SLOTIMP[NUMBER_PORTS_PER_SOCKET*0 + 1] = 1;
    SetupData->SLOTIMP[NUMBER_PORTS_PER_SOCKET*0 + 2] = 1;
    
    SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxx8x4x4;
    SetupData->SLOTIMP[NUMBER_PORTS_PER_SOCKET*1 + 1] = 1;
    SetupData->SLOTIMP[NUMBER_PORTS_PER_SOCKET*1 + 2] = 1;

    OnboardNVMe = PcdGet64(PcdSmcAOCLocation);
    OnboardNVMe |= 0x0000000500000005;
    PcdSet64(PcdSmcAOCLocation, OnboardNVMe);

    //AOM-S3108M-H8
    SetupData->ConfigIOU1[0] = IIO_BIFURCATE_xxxxxx16;    
    SetupData->SLOTIMP[NUMBER_PORTS_PER_SOCKET*0 + 5] = 1;
    
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
        SXB1_SLOT_INFO_Tbl, SXB2_SLOT_INFO_Tbl, NULL);
    DEBUG((-1, "PEI - GetSuperMPcieBifurcation End.\n"));
#endif
#if SmcAOC_SUPPORT
    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gSmcAOCPeiProtocolGuid,
                 0,
                 NULL,
                 &mSmcAOCPeiProtocol);
    DEBUG((-1, "LocatePpi mSmcAOCPeiProtocol = %r.\n", Status));
    if(!Status){
        mSmcAOCPeiProtocol->SmcAOCPeiCardDetection(PeiServices, 0, 0xe2, 0x01, SXB1_SLOT_INFO_Tbl,
                        sizeof(SXB1_SLOT_INFO_Tbl)/sizeof(SMC_SLOT_INFO));      //mux ch0
        mSmcAOCPeiProtocol->SmcAOCPeiCardDetection(PeiServices, 0, 0xe2, 0x02, SXB2_SLOT_INFO_Tbl,
                        sizeof(SXB2_SLOT_INFO_Tbl)/sizeof(SMC_SLOT_INFO));      //mux ch1
        mSmcAOCPeiProtocol->SmcAOCPeiBifurcation(SetupData, DefaultIOU0,
                        DefaultIOU1, DefaultIOU2);
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
    
#if ONBOARD_LAN_DISABLE_BY_SETUP_ITEM
    Status = (*PeiServices)->LocatePpi(
		PeiServices,
		&gEfiPeiReadOnlyVariable2PpiGuid,
		0,
		NULL,
		&ReadOnlyVariable);

    if(!EFI_ERROR(Status)){
        VarSize = sizeof(SETUP_DATA);
	Status = ReadOnlyVariable->GetVariable(
			ReadOnlyVariable,
			L"Setup",
			&SetupGuid,
			NULL,
			&VarSize,
			&Setup10GbE);
    
    if(!EFI_ERROR(Status)){
        if(Setup10GbE.SmcOnboardLan == 0)
            SetupData->PciePortDisable[3] = 0;  //10Gbe LAN map to CPU PCIe port 1C
        }
    }
#endif
    
    Check_ME_Reboot(PeiServices);

    DEBUG((EFI_D_INFO, "PEI - GetProjectPciEBifurcate End.\n"));
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
EFI_STATUS EFIAPI X11DDWPeiDriverInit(
    IN	EFI_FFS_FILE_HEADER	*FfsHeader,
    IN	EFI_PEI_SERVICES	**PeiServices
)
{
    EFI_PEI_PPI_DESCRIPTOR	*mPpiList = NULL;
    EFI_STATUS	Status;

    DEBUG((DEBUG_INFO, "ProjectPeiDriverInit.\n"));

    Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (EFI_PEI_PPI_DESCRIPTOR), &mPpiList);

    mPpiList->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    mPpiList->Guid = &gProjectOwnPeiProtocolGuid;
    mPpiList->Ppi = &PeiProjectInterface;

    Status = (**PeiServices).InstallPpi(PeiServices, mPpiList);
    if (EFI_ERROR(Status)){
        DEBUG((DEBUG_INFO, "ProjectPeiDriverInit fail.\n"));
        return Status;
    }

    DEBUG((DEBUG_INFO, "Exit ProjectPeiDriverInit.\n"));

    return EFI_SUCCESS;
}
