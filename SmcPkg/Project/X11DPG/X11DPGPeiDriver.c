//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.05
//    Bug Fix:  Set IIO MCP port 0 width to x8x8 for FPGA SKU SPU only.
//    Reason:   System would hang when set CPU MCP port 0 to x8x8 with Fabric SKU SPU
//    Auditor:  Jimmy Chiu
//    Date:     Jun/22/2017
//
//  Rev. 1.04
//    Bug Fix:  Update CPU UPI2(C0K2, C1K2) TxEQ value.
//    Reason:   From LAB suggestion.
//    Auditor:  Jimmy Chiu
//    Date:     Apr/21/2017
//
//  Rev. 1.03
//    Bug Fix:  Change MCP0 to x8x8 for FPGA.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Feb/08/2017
//
//  Rev. 1.02
//    Bug Fix:  Change UPI EParams overriding method.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jan/18/2017
//
//  Rev. 1.01
//    Bug Fix:  Correct PCIE Bifurcate.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jan/09/2017
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Dec/27/2016
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
#include <Library/PcdLib.h>
#include "X11DPGPeiDriver.h"
#include "SmcCspLibBmc.h"
#if SmcAOC_SUPPORT
#include "../Module/SmcAOC/SmcAOCPei.h"
EFI_GUID gSmcAOCPeiProtocolGuid = EFI_SMC_AOC_PEI_PROTOCOL_GUID;
#endif

//
// Link speed
//
#define SPEED_REC_96GT     0
#define SPEED_REC_104GT    1
#define MAX_KTI_LINK_SPEED 2
#define FREQ_PER_LINK      3
#define ADAPTIVE_CTLE 0x3f
#define PER_LANE_ADAPTIVE_CTLE 0X3f3f3f3f

typedef enum {
  KTI_LINK0 =  0x0,
  KTI_LINK1,
  KTI_LINK2
} KTI_LOGIC_LINK;

//
// Link Speed Modes
//
#define KTI_LINK_SLOW_SPEED_MODE 0
#define KTI_LINK_FULL_SPEED_MODE 1


/*-----------------12/23/2014 3:06PM----------------
 * Coding for every platform
 * --------------------------------------------------*/
//HSX_PER_LANE_EPARAM_LINK_INFO  X11DPGPerLanesEparamTable[] = {
//};
//UINT32 X11DPGPerLanesEparamTableSize = sizeof (X11DPGPerLanesEparamTable);

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

ALL_LANES_EPARAM_LINK_INFO  X11DPGAllLanesEparamTable[] = {

  //
  // SocketID, Freq, Link, TXEQL, CTLEPEAK
  //

  //
  // Socket 0
  //
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2E39343F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2F39353F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2C34373F, ADAPTIVE_CTLE},

  //
  // Socket 1
  //
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2D37353F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2F3A343F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2E36373F, ADAPTIVE_CTLE}
};

UINT32 X11DPGAllLanesEparamTableSize = sizeof (X11DPGAllLanesEparamTable);

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

    *SizeOfTable = X11DPGAllLanesEparamTableSize;
    *Ptr = (ALL_LANES_EPARAM_LINK_INFO *) &X11DPGAllLanesEparamTable;

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
#if SmcAOC_SUPPORT
    PEI_SMC_AOC_PROTOCOL	*mSmcAOCPeiProtocol = NULL;
    SMC_SLOT_INFO               SLOT_Tbl[] = SLOT_INFO_Tbl;
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
    // First CPU
    //
    if(SetupData->ConfigIOU0[0] == 0xff){
        SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxxxxx16;
        SetupData->SLOTIMP[1] = 1;
        SetupData->SLOTPSP[1] = 1;
    }
            
    if(SetupData->ConfigIOU1[0] == 0xff){
        SetupData->ConfigIOU1[0] = IIO_BIFURCATE_xxxxxx16;
        SetupData->SLOTIMP[5] = 1;
        SetupData->SLOTPSP[5] = 5;
    }
            
    if(SetupData->ConfigIOU2[0] == 0xff) {
        SetupData->ConfigIOU2[0] = IIO_BIFURCATE_xxxxxx16;
        SetupData->SLOTIMP[9] = 1;
        SetupData->SLOTPSP[9] = 1;
    }

    if(SetupData->ConfigMCP0[0] == 0xff) {
        if(PcdGet64(PcdSmcCPU0IIOBus) & BIT56) { //FPGA CPU
            SetupData->ConfigMCP0[0] = IIO_BIFURCATE_xxx8xxx8;
            SetupData->SLOTIMP[13] = 1;
            SetupData->SLOTPSP[13] = 1;
            SetupData->SLOTIMP[15] = 1;
            SetupData->SLOTPSP[15] = 1;
        } else {
            SetupData->ConfigMCP0[0] = IIO_BIFURCATE_xxxxxx16;
            SetupData->SLOTIMP[13] = 1;
            SetupData->SLOTPSP[13] = 1;
        }
    }

    if(SetupData->ConfigMCP1[0] == 0xff) {
        SetupData->ConfigMCP1[0] = IIO_BIFURCATE_xxxxxx16;
        SetupData->SLOTIMP[17] = 1;
        SetupData->SLOTPSP[17] = 1;
    }

    //
    // Second CPU
    //
    if(SetupData->ConfigIOU0[1] == 0xff) {
        SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxxxxx16;
        SetupData->SLOTIMP[22] = 1;
        SetupData->SLOTPSP[22] = 1;
    }

    if(SetupData->ConfigIOU1[1] == 0xff) {
        SetupData->ConfigIOU1[1] = IIO_BIFURCATE_xxxxxx16;
        SetupData->SLOTIMP[26] = 1;
        SetupData->SLOTPSP[26] = 1;
    }

    if(SetupData->ConfigIOU2[1] == 0xff) {
        SetupData->ConfigIOU2[1] = IIO_BIFURCATE_xxxxxx16;
        SetupData->SLOTIMP[30] = 1;
        SetupData->SLOTPSP[30] = 1;
    }

    if(SetupData->ConfigMCP0[1] == 0xff) {
        if(PcdGet64(PcdSmcCPU1IIOBus) & BIT56) { //FPGA CPU
            SetupData->ConfigMCP0[1] = IIO_BIFURCATE_xxx8xxx8;
            SetupData->SLOTIMP[34] = 1;
            SetupData->SLOTPSP[34] = 1;
            SetupData->SLOTIMP[36] = 1;
            SetupData->SLOTPSP[36] = 1;
        } else {
            SetupData->ConfigMCP0[1] = IIO_BIFURCATE_xxxxxx16;
            SetupData->SLOTIMP[34] = 1;
            SetupData->SLOTPSP[34] = 1;
        }
    }

    if(SetupData->ConfigMCP1[1] == 0xff) {
        SetupData->ConfigMCP1[1] = IIO_BIFURCATE_xxxxxx16;
        SetupData->SLOTIMP[38] = 1;
        SetupData->SLOTPSP[38] = 1;
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

    DEBUG((EFI_D_INFO, "PEI GetProjectPciEBifurcate End.\n"));
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
EFI_STATUS EFIAPI X11DPGPeiDriverInit(
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
