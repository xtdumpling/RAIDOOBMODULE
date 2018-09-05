//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.08
//    Bug Fix:  Add memory error LED function.
//    Reason:   
//    Auditor:  Jimmy Chiu
//    Date:     Jul/03/2017
//
//  Rev. 1.07
//    Bug Fix:  10NVMe/24NVMe VMD LED wrong
//    Reason:   Those 2 systems use 3rd I2C bus
//    Auditor:  Donald Han
//    Date:     Jun/27/2017
//
//  Rev. 1.06
//    Bug Fix:  Change 24NVMe riser card model due to previous config cannot pass SI
//    Reason:   All oculink now are from riser card, we need to disable onboard NVMe headers
//    Auditor:  Donald Han
//    Date:     Jun/08/2017
//
//  Rev. 1.05
//    Bug Fix:  Programm RSC redriver chip as well
//    Reason:   Some system config has redriver on both UIO and WIO-L
//    Auditor:  Donald Han
//    Date:     May/05/2017
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

#include <Token.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/IoLib.h>
#include <Guid/SocketMemoryVariable.h>
#include <IndustryStandard/SmBus.h>
#include <Ppi/Smbus2.h>
#include <Ppi/ReadOnlyVariable2.h>
#include "X11DPUPeiDriver.h"
#include "SmcCspLibBmc.h"

#if SmcRiserCard_SUPPORT
#include "../Module/SmcRiserCard/SmcRiserCardPei.h"
EFI_GUID gSmcRiserCardPeiProtocolGuid = EFI_SMCRISERCARD_PEI_DRIVER_PROTOCOL_GUID;
#endif
#if SmcAOC_SUPPORT
#include "../Module/SmcAOC/SmcAOCPei.h"
EFI_GUID gSmcAOCPeiProtocolGuid = EFI_SMC_AOC_PEI_PROTOCOL_GUID;
#endif
EFI_GUID gEfiPeiReadOnlyVariablePpiGuid = EFI_PEI_READ_ONLY_VARIABLE2_PPI_GUID;

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

// Function Definition(s)
EFI_STATUS PeiMemErrLedCtrl(
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
        PeiMemErrLedCtrl
    }
};

VOID WriteCMOS (
  IN UINT8  Offset,
  IN OUT UINT8 Value )
{
    IoWrite8(0x70, Offset);
    IoWrite8(0xEB, Offset);
    IoWrite8(0x71, Value);
}

UINT8 ReadCMOS (
  IN UINT8  Offset)
{
    UINT8 Value;
    IoWrite8(0x70, Offset);
    IoWrite8(0xEB, Offset);
    Value = IoRead8(0x71);
    return Value;
}

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

ALL_LANES_EPARAM_LINK_INFO  X11DPGAllLanesEparamTable[] = {

    //
    // SocketID, Freq, Link, TXEQL, CTLEPEAK
    //

    //
    // Socket 0
    //
    {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2E39343F, ADAPTIVE_CTLE},
    {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2F39353F, ADAPTIVE_CTLE},

    //
    // Socket 1
    //
    {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2D37353F, ADAPTIVE_CTLE},
    {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2F3A343F, ADAPTIVE_CTLE}
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
    IN EFI_PEI_SERVICES **PeiServices,
    IN PROJECT_PEI_DRIVER_PROTOCOL *This,
    OUT UINT32 *SizeOfTable,
    OUT ALL_LANES_EPARAM_LINK_INFO **Ptr)
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
#endif
    SMC_SLOT_INFO       SYS_2029U_TN24R4T = {0xe0, 6, 0, 0};

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
    if(!Status){
        mSmcAOCPeiProtocol->SmcAOCPeiCardDetection(PeiServices, 0x05, 0xe2, 0x04,
            SXB1_SLOT_INFO_Tbl, sizeof(SXB1_SLOT_INFO_Tbl)/sizeof(SMC_SLOT_INFO));//mux ch0
        mSmcAOCPeiProtocol->SmcAOCPeiCardDetection(PeiServices, 0x05, 0xe2, 0x05,
            SXB2_SLOT_INFO_Tbl, sizeof(SXB2_SLOT_INFO_Tbl)/sizeof(SMC_SLOT_INFO));//mux ch1
        mSmcAOCPeiProtocol->SmcAOCPeiCardDetection(PeiServices, 0x05, 0xe2, 0x06,
            SXB3_SLOT_INFO_Tbl, sizeof(SXB3_SLOT_INFO_Tbl)/sizeof(SMC_SLOT_INFO));//mux ch2

        OnboardNVMe = PcdGet64(PcdSmcAOCLocation);
        if(OnboardNVMe == 0x0000005200550550){  // SYS-1029U-TN10RT
            DEBUG((-1, "SYS-1029U-TN10RT\n"));
            OnboardNVMe &= 0x0000005200550500;  // mask CPU1 onboard NVNe(1C, 1D)
            PcdSet64(PcdSmcAOCLocation, OnboardNVMe);
            mSmcAOCPeiProtocol->SmcAOCPeiBifurcation(SetupData, DefaultIOU0, DefaultIOU1, DefaultIOU2);
            mSmcAOCPeiProtocol->SmcAOCPeiSetNVMeMode(PeiServices, 0x05, 0xe2, 0x06, NULL, 0);
            SMB_Select = 1;
        }
//KKK        if(OnboardNVMe == 0x0000555000550050){  // SYS-2029U-TN24R4T
        if(OnboardNVMe == 0x0000015000010050){  // SYS-2029U-TN24R4T
            DEBUG((-1, "SYS-2029U-TN24R4T\n"));
            // override pcie bifurcation for 24 bay NVMe system.
            // we must specially program CPU1 3A and CPU2 2A to x16.
            SetupData->ConfigIOU2[0] = IIO_BIFURCATE_xxxxxx16;
            SetupData->ConfigIOU1[1] = IIO_BIFURCATE_xxxxxx16;
            // mask all NVMe port
            OnboardNVMe &= 0;
            PcdSet64(PcdSmcAOCLocation, OnboardNVMe);
            // set CPU1 3A and CPU2 2A are x16 PLX
            OnboardNVMe = PcdGet64(PcdSmcAOCExtendFlag) | 0x0000020000020000;
            PcdSet64(PcdSmcAOCExtendFlag, OnboardNVMe);
            // BPN CPLD1 for CPU1 and control 12 NVMe, CPU1 3A is PLX and VMD enable
            OnboardNVMe = PcdGet64(PcdSmcAOCNVMeMode);
            if(SetupData->VMDEnabled[2] && SetupData->VMDPortEnable[8])
                OnboardNVMe |= (UINT64)0x0fff;
            // BPN CPLD2 for CPU2 and control 12 NVMe, CPU2 2A is PLX and VMD enable
            if(SetupData->VMDEnabled[4] && SetupData->VMDPortEnable[16])
                OnboardNVMe |= LShiftU64((UINT64)0x0fff, 32);
            PcdSet64(PcdSmcAOCNVMeMode, OnboardNVMe);
            mSmcAOCPeiProtocol->SmcAOCPeiBifurcation(SetupData, DefaultIOU0, DefaultIOU1, DefaultIOU2);
            mSmcAOCPeiProtocol->SmcAOCPeiSetNVMeMode(PeiServices, 0x05, 0xe2, 0x06,
                            &SYS_2029U_TN24R4T, 1);
            SMB_Select = 1;     // set bitmap by PCH smbus to BPN CPLD
        }

        if(!SMB_Select){
            mSmcAOCPeiProtocol->SmcAOCPeiBifurcation(SetupData, DefaultIOU0, DefaultIOU1, DefaultIOU2);
            mSmcAOCPeiProtocol->SmcAOCPeiSetNVMeMode(PeiServices, 0x0d, 0, 0, NULL, 0);
        }
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
EFI_STATUS EFIAPI X11DPUPeiDriverInit(
    IN EFI_FFS_FILE_HEADER *FfsHeader,
    IN EFI_PEI_SERVICES **PeiServices
)
{
    EFI_PEI_PPI_DESCRIPTOR *mPpiList = NULL;
    EFI_STATUS Status;
    UINT8 Data8;

    DEBUG ((EFI_D_INFO, "ProjectPeiDriverInit.\n"));

    IoWrite32(0xcf8, 0x8000FAA4); //D31:F2:Reg[A4]
    Data8 = IoRead8(0xcfc);

    DEBUG ((EFI_D_INFO, "PM3 Configuration = %x \n", Data8));
    if((Data8 & 0x02) == 0x02) //Power Failure (PWR_FLR)
    {
        DEBUG ((EFI_D_INFO, "AC power lose, clear memory error CMOS flag \n"));
        WriteCMOS(CMOS_MEM_ERR_FLAG_0, 0x55); //Reset memory error flag
        WriteCMOS(CMOS_MEM_ERR_FLAG_1, 0xAA); //Reset memory error flag
    }

    // Set the Smbus Notify PPI
    Status = (*PeiServices)->NotifyPpi(PeiServices, mNotifyList);
    //ASSERT_PEI_ERROR (PeiServices, Status);
 
    Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (EFI_PEI_PPI_DESCRIPTOR), &mPpiList);
    //ASSERT_PEI_ERROR (PeiServices, Status);

    mPpiList->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    mPpiList->Guid = &gProjectOwnPeiProtocolGuid;
    mPpiList->Ppi = &PeiProjectInterface;

    Status = (**PeiServices).InstallPpi(PeiServices, mPpiList);
    if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_INFO, "ProjectPeiDriverInit fail.\n"));
        return Status;
    }

    DEBUG ((DEBUG_INFO, "Exit ProjectPeiDriverInit.\n"));

    return EFI_SUCCESS;
}

EFI_STATUS PeiMemErrLedCtrl(
    IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *NullPpi
)
{
    EFI_STATUS Status;
    EFI_PEI_SMBUS2_PPI *SmBus;
    EFI_SMBUS_DEVICE_ADDRESS SlaveAddress;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVariable;
    SOCKET_MEMORY_CONFIGURATION gSocketMemoryConfiguration;

    UINTN Len = 1;
    UINT8 tempData[4];
    UINT8 PCA9539_ADDR1 = 0x75;
    UINT8 PCA9539_ADDR2 = 0x76;
    UINTN VariableSize;
	
    DEBUG((EFI_D_INFO, "Set Memory error LED start\n"));
    
    if(!(((ReadCMOS(CMOS_MEM_ERR_FLAG_0) == 0xAA) && (ReadCMOS(CMOS_MEM_ERR_FLAG_1) == 0x55)) || ((ReadCMOS(CMOS_MEM_ERR_FLAG_0) == 0x55) && (ReadCMOS(CMOS_MEM_ERR_FLAG_1) == 0xAA)))){
        WriteCMOS(CMOS_MEM_ERR_FLAG_0, 0x55); //Reset memory error flag
        WriteCMOS(CMOS_MEM_ERR_FLAG_1, 0xAA); //Reset memory error flag
        WriteCMOS(CMOS_ERR_DIMM_LOC0, 0x00); //Clear memory error DIMM location 0
        WriteCMOS(CMOS_ERR_DIMM_LOC1, 0x00); //Clear memory error DIMM location 1
        WriteCMOS(CMOS_ERR_DIMM_LOC2, 0x00); //Clear memory error DIMM location 2
        WriteCMOS(CMOS_ERR_DIMM_LOC3, 0x00); //Clear memory error DIMM location 3
        //return EFI_SUCCESS;
    }
    
    Status = (*PeiServices)->LocatePpi( PeiServices,
					&gEfiPeiSmbus2PpiGuid,
					0,
					NULL,
					&SmBus );
    //ASSERT_PEI_ERROR( PeiServices, Status);
    if (EFI_ERROR (Status)) {
        DEBUG((EFI_D_INFO, "Locating Smbus PPI failed!, Status = %r\n", Status));
    } else {
        DEBUG((EFI_D_INFO, "Locating Smbus PPI Success!, Status = %r\n", Status));
    }
    
    Status = (**PeiServices).LocatePpi ( PeiServices,
                                       &gEfiPeiReadOnlyVariablePpiGuid,
                                       0,
                                       NULL,
                                       &PeiVariable );
    //ASSERT_PEI_ERROR( PeiServices, Status);
    if (EFI_ERROR (Status)) {
        DEBUG((EFI_D_INFO, "Locating ReadOnlyVariable PPI failed!, Status = %r\n", Status));
    } else {
       DEBUG((EFI_D_INFO, "Locating ReadOnlyVariable PPI Success!, Status = %r\n", Status));
    }
    
    VariableSize = sizeof (SOCKET_MEMORY_CONFIGURATION);
    Status = PeiVariable->GetVariable (PeiVariable, SOCKET_MEMORY_CONFIGURATION_NAME, &gSmcEfiSocketMemoryVariableGuid, NULL, &VariableSize, &gSocketMemoryConfiguration);
    //ASSERT_PEI_ERROR( PeiServices, Status);
    if (EFI_ERROR (Status)) {
        DEBUG((EFI_D_INFO, "GetVariable SocketMemoryConfig failed!, Status = %r\n", Status));
    } else {
        DEBUG((EFI_D_INFO, "GetVariable SocketMemoryConfig Success!, Status = %r\n", Status));
    }
    
    //Check clear memory LED by setup item
    if(gSocketMemoryConfiguration.MemErrorLedOff == 1){
        WriteCMOS(CMOS_MEM_ERR_FLAG_0, 0x55);
        WriteCMOS(CMOS_MEM_ERR_FLAG_1, 0xAA);
    }
    
    //Set GPIO high/low for turn on/off
    if((ReadCMOS(CMOS_MEM_ERR_FLAG_0) == 0x55) && (ReadCMOS(CMOS_MEM_ERR_FLAG_1) == 0xAA)){
        DEBUG((EFI_D_INFO, "Turn off LED\n"));
        WriteCMOS(CMOS_ERR_DIMM_LOC0, 0x00); //Clear memory error DIMM location 0
        WriteCMOS(CMOS_ERR_DIMM_LOC1, 0x00); //Clear memory error DIMM location 1
        WriteCMOS(CMOS_ERR_DIMM_LOC2, 0x00); //Clear memory error DIMM location 2
        WriteCMOS(CMOS_ERR_DIMM_LOC3, 0x00); //Clear memory error DIMM location 3
    } else if((ReadCMOS(CMOS_MEM_ERR_FLAG_0) == 0xAA) && ((ReadCMOS(CMOS_MEM_ERR_FLAG_1) == 0x55))){
        DEBUG((EFI_D_INFO, "Turn on LED\n"));
    } else {
        DEBUG((EFI_D_INFO, "Flag not match, do nothing\n"));
        return EFI_SUCCESS;
    }

    tempData[0] = ReadCMOS(CMOS_ERR_DIMM_LOC0); //Get memory error DIMM location 0
    tempData[1] = ReadCMOS(CMOS_ERR_DIMM_LOC1); //Get memory error DIMM location 1
    tempData[2] = ReadCMOS(CMOS_ERR_DIMM_LOC2); //Get memory error DIMM location 2
    tempData[3] = ReadCMOS(CMOS_ERR_DIMM_LOC3); //Get memory error DIMM location 3
		
    SlaveAddress.SmbusDeviceAddress = PCA9539_ADDR1;
    Status = SmBus->Execute(SmBus, SlaveAddress, 2, EfiSmbusWriteByte, 0, &Len, &tempData[0]);
    Status = SmBus->Execute(SmBus, SlaveAddress, 3, EfiSmbusWriteByte, 0, &Len, &tempData[1]);
    		
    SlaveAddress.SmbusDeviceAddress = PCA9539_ADDR2;
    Status = SmBus->Execute(SmBus, SlaveAddress, 2, EfiSmbusWriteByte, 0, &Len, &tempData[2]);
    Status = SmBus->Execute(SmBus, SlaveAddress, 3, EfiSmbusWriteByte, 0, &Len, &tempData[3]);
            
    //Set GPIO as output
    DEBUG((EFI_D_INFO, "Set GPIO as output\n"));
    
    tempData[0] = 0x00;
    SlaveAddress.SmbusDeviceAddress = PCA9539_ADDR1;
    Status = SmBus->Execute(SmBus, SlaveAddress, 6, EfiSmbusWriteByte, 0, &Len, &tempData[0]);
    Status = SmBus->Execute(SmBus, SlaveAddress, 7, EfiSmbusWriteByte, 0, &Len, &tempData[0]);

    SlaveAddress.SmbusDeviceAddress = PCA9539_ADDR2;
    Status = SmBus->Execute(SmBus, SlaveAddress, 6, EfiSmbusWriteByte, 0, &Len, &tempData[0]);
    Status = SmBus->Execute(SmBus, SlaveAddress, 7, EfiSmbusWriteByte, 0, &Len, &tempData[0]);
    
    DEBUG((EFI_D_INFO, "Set Memory error LED end\n"));

    return Status;
}
