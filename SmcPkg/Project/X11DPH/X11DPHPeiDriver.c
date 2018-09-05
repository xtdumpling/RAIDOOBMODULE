//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Detect Board ID via IPMI FRU
//    Reason:   PCIe RP #0 bifurcate for X11DPH-TQ.
//    Auditor:  Max Mu
//    Date:     Jun/05/2017
//
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Max Mu
//    Date:     Mar/02/2016
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
#include <Library/GpioLib.h>
#include "X11DPHPeiDriver.h"
#include "SmcCspLibBmc.h"
#include "SspTokens.h"
#include "SmcLibCommon.h"
#include <Library/BaseMemoryLib.h>

#if ONBOARD_LAN_DISABLE_BY_SETUP_ITEM
#include <Ppi/ReadOnlyVariable2.h>
#include <PEI.h>
#include "Setup.h"
#endif

#if SmcAOC_SUPPORT
#include "../Module/SmcAOC/SmcAOCPei.h"
EFI_GUID gSmcAOCPeiProtocolGuid = EFI_SMC_AOC_PEI_PROTOCOL_GUID;
#endif

#if IPMI_SUPPORT == 1
#include <Include/IpmiNetFnAppDefinitions.h>
#include <Include/IpmiNetFnStorageDefinitions.h>
#include <Ppi/IPMITransportPpi.h>

typedef struct {
  UINT8       FormatVersionNumber:4;
  UINT8       Reserved:4;
  UINT8       InternalUseStartingOffset;
  UINT8       ChassisInfoStartingOffset;
  UINT8       BoardAreaStartingOffset;
  UINT8       ProductInfoStartingOffset;
  UINT8       MultiRecInfoStartingOffset;
  UINT8       Pad;
  UINT8       Checksum;
} IPMI_FRU_COMMON_HEADER;

#define FRU_BOARD_INFO_MAX_SIZE   64

#define  BOARD_MFG_OFFSET               6 

#define  FRU_DATA_LENGTH_MASK           0x3F
#define  FRU_DATA_TYPE_MASK             0xC0

#endif // #if IPMI_SUPPORT == 1

#define SPEED_REC_96GT     0
#define SPEED_REC_104GT    1
#define MAX_KTI_LINK_SPEED 2
#define FREQ_PER_LINK      3
#define ADAPTIVE_CTLE 0x3f
#define PER_LANE_ADAPTIVE_CTLE 0X3f3f3f3f
#define KTI_LINK0 0
#define KTI_LINK1 1
#define KTI_LINK2 2

enum{
    BOARD_X11DPH_T = 0x0,
    BOARD_X11DPH_TQ,
    BOARD_UNKNOWN
}BOARD_ID;

ALL_LANES_EPARAM_LINK_INFO  X11DPH_KtiAllLanesEparamTable[] = {
  //
  // SocketID, Freq, Link, TXEQL, CTLEPEAK
  //
  //
  // Socket 0
  //
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2E39343F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2F39353F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2D3A323F, ADAPTIVE_CTLE},
  //
  // Socket 1
  //
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2D37353F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2F3A343F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2D3A323F, ADAPTIVE_CTLE}
};


#if 0
{
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
#endif

//
// From Leon
//
#if 0
{
  //
  // SocketID, Freq, Link, TXEQL, CTLEPEAK
  //
  //
  // Socket 0
  //
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2E39343F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2F39353F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2D3A323F, ADAPTIVE_CTLE},
  //
  // Socket 1
  //
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2D37353F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2F3A343F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2D3A323F, ADAPTIVE_CTLE}
};
#endif

#if (IPMI_SUPPORT == 1) && (X11DPH_DETECT_BOARD_VIA_FRU == 1)

EFI_STATUS IpmiReadFruData (
  IN PEI_IPMI_TRANSPORT_PPI               *IpmiTransportPpi,
  IN UINTN                                FruDataOffset,
  IN UINTN                                FruDataSize,
  IN UINT8                                *FruData )
{
    EFI_STATUS                    Status;
    IPMI_FRU_READ_COMMAND         FruReadCommandData = {0};
    UINT8                         ResponseData[20];
    UINT8                         ResponseDataSize;
    UINT16                        PointerOffset = 0;
    UINT8                         Counter = 3;//retry counter

    
    //
    // Create the FRU Read Command for the logical FRU Device.
    //
    FruReadCommandData.Data.FruDeviceId = 0x01;//always FRU1, the FRU0 is used for customer and not safe.
    FruReadCommandData.Data.FruOffset = (UINT16) FruDataOffset;

    //
    // Collect the data till it is completely retrieved.
    //
    while (FruDataSize > 0) {
        //
        // BMC allows to get only 16 bytes with in a single command.
        // Data more than 16 bytes will be received as part by part of 16 bytes
        //
        if (FruDataSize > 0x10) {
            FruReadCommandData.Count = 0x10;
        } else {
            FruReadCommandData.Count = (UINT8)FruDataSize;
        }

        //
        // Read FRU data
        //
        while (TRUE) {
            ResponseDataSize = sizeof (ResponseData);
            Status = IpmiTransportPpi->SendIpmiCommand (
                        IpmiTransportPpi,
                        IPMI_NETFN_STORAGE,
                        BMC_LUN,
                        IPMI_STORAGE_READ_FRU_DATA,
                        (UINT8 *) &FruReadCommandData,
                        sizeof (FruReadCommandData),
                        (UINT8 *) &ResponseData,
                        (UINT8 *) &ResponseDataSize );
            if (EFI_ERROR(Status)) {
                return Status;
            }

            if (IpmiTransportPpi->CommandCompletionCode == 0x81) {
                if (Counter-- == 0) {
                  return EFI_NOT_READY;
                }
                //MicroSecondDelay (IPMI_FRU_COMMAND_RETRY_DELAY);
            } else {
                break;
            }
        }

        //
        // In case of partial retrieval; Data[0] contains the retrieved data size;
        //
        ResponseData[0] = ResponseData[0] <= FruReadCommandData.Count \
                        ? ResponseData[0] \
                        : FruReadCommandData.Count ;
        FruDataSize = (UINT16) (FruDataSize - ResponseData[0]);    // Remaining Count
        FruReadCommandData.Data.FruOffset = (UINT16) (FruReadCommandData.Data.FruOffset + ResponseData[0]);  // Next Offset to retrieve
        CopyMem (&FruData[PointerOffset], &ResponseData[1], ResponseData[0]);  // Copy the partial data
        PointerOffset = (UINT16) (PointerOffset + ResponseData[0]);  // Next offset to the input pointer.
    } //while (FruDataSize > 0)

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  DetectBoardIdFromIpmiFru
//
// Description :  Detect the board ID by reading FRU data, if FRU is not programed correct, using defatul ID
//
// Parameters:  **PeiServices - Pointer to the PEI services table
//
// Returns     :  
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS DetectBoardIdFromIpmiFru(
    IN EFI_PEI_SERVICES **PeiServices,
    IN OUT	UINT32	*BoardId
)
{
    EFI_STATUS                    Status;
    PEI_IPMI_TRANSPORT_PPI        *IpmiTransportPpi;
    IPMI_FRU_COMMON_HEADER        FruCommonHeader = {0};
    UINT8                         FruBoardInfo[FRU_BOARD_INFO_MAX_SIZE];
    UINT32                        Offset;
    UINT32                        Length;
    UINT32                        i;
    UINT8                         *BPN = NULL;//Board Product Name 

    
    *BoardId = BOARD_X11DPH_T;//default for X11DPH_T
        
    //
    //Locate IPMI Transport protocol to send commands to BMC.
    //
    Status = (*PeiServices)->LocatePpi (
        (CONST EFI_PEI_SERVICES**)PeiServices,
        &gEfiPeiIpmiTransportPpiGuid,
        0,
        NULL,
        (VOID **)&IpmiTransportPpi );
    if ( EFI_ERROR(Status) ) {
        return Status;
    }
    
    //
    // Read the Common header of FRU records
    //
    Status = IpmiReadFruData (IpmiTransportPpi, 0, sizeof (IPMI_FRU_COMMON_HEADER), (UINT8*) &FruCommonHeader);
    if (EFI_ERROR (Status)) {
        return Status;
    }
    
    DEBUG((-1, "FruCommonHeader.FormatVersionNumber = 0x%x\n",FruCommonHeader.FormatVersionNumber));
    DEBUG((-1, "FruCommonHeader.BoardAreaStartingOffset = 0x%x\n",FruCommonHeader.BoardAreaStartingOffset));
    DEBUG((-1, "FruCommonHeader.Checksum = 0x%x\n",FruCommonHeader.Checksum));
    
    //
    // Do a validity check on the FRU header, since it may be all 0xFF(s) in
    // case there of no FRU programmed on the system.
    //
    if  ( CalculateCheckSum8 ((UINT8 *)&FruCommonHeader, sizeof (FruCommonHeader)) != 0 ){
        DEBUG((-1, "FRU header invalid.\n"));
        //
        //  The FRU Common header is bad
        //
        return EFI_CRC_ERROR;
    }

    //
    // Read Base Board data 
    //
    Offset = FruCommonHeader.BoardAreaStartingOffset * 8;
    if(Offset == 0){
        return EFI_UNSUPPORTED;
    }
    Length = 0;
    IpmiReadFruData (IpmiTransportPpi, (Offset + 1), 1, (UINT8*) &Length);
    if (Length > 0) {
        Length = Length * 8;
    }
    Status = IpmiReadFruData (IpmiTransportPpi, Offset, Length, FruBoardInfo);
    if (!EFI_ERROR (Status)) {
        //
        //  Validating Board Info Area Checksum before reading the Board Info
        //  Area Data
        //
        if ( CalculateCheckSum8 ( FruBoardInfo, Length ) != 0 ) {    
            DEBUG((-1, "FRU header invalid.\n"));
            //
            //  The FRU Board information is bad
            //
            return EFI_CRC_ERROR;
        }
        
        //
        // Board Manufacturer
        //
        Offset = BOARD_MFG_OFFSET;
        Length = FRU_DATA_LENGTH_MASK & FruBoardInfo[Offset];
        
        //
        // Board Product Name
        //
        Offset = Offset + Length + 1;
        Length = FRU_DATA_LENGTH_MASK & FruBoardInfo[Offset];
        BPN    = &FruBoardInfo[Offset + 1];
        BPN[Length] = '\0';

        DEBUG((-1, "FruBoardInfo Data :\n"));
        i = 0;
        while ( i <= Length ) {
            DEBUG((-1,"%02x ",FruBoardInfo[Offset + i]));
            i++;
        }
        DEBUG((-1, "\n"));

        if(AsciiStriCmp(BPN, "x11dph-t") == 0){
            *BoardId = BOARD_X11DPH_T;
        }
        else if(AsciiStriCmp(BPN, "x11dph-tq") == 0){
            *BoardId = BOARD_X11DPH_TQ;
        }
        else{
            *BoardId = BOARD_X11DPH_T;
        }
    }
    return EFI_SUCCESS;
}
#endif//(IPMI_SUPPORT == 1) && (X11DPH_DETECT_BOARD_VIA_FRU == 1)

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
	*Ptr = X11DPH_KtiAllLanesEparamTable;
	*SizeOfTable =  (UINT32)(sizeof(X11DPH_KtiAllLanesEparamTable)/sizeof(ALL_LANES_EPARAM_LINK_INFO));
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
#if (IPMI_SUPPORT == 1) && (X11DPH_DETECT_BOARD_VIA_FRU == 1)
    UINT32  board_id = BOARD_X11DPH_T;
#endif
#if SmcAOC_SUPPORT
    PEI_SMC_AOC_PROTOCOL    *mSmcAOCPeiProtocol = NULL;
    SMC_SLOT_INFO           SLOT_Tbl[] = SLOT_INFO_Tbl;
#endif
#if ONBOARD_LAN_DISABLE_BY_SETUP_ITEM
    UINTN       VarSize;
    SETUP_DATA	Setup10GbE;
    EFI_GUID	SetupGuid = SETUP_GUID;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
#endif


    DefaultIOU0[0] = SetupData->ConfigIOU0[0];
    DefaultIOU1[0] = SetupData->ConfigIOU1[0];
    DefaultIOU2[0] = SetupData->ConfigIOU2[0];
    DefaultIOU0[1] = SetupData->ConfigIOU0[1];
    DefaultIOU1[1] = SetupData->ConfigIOU1[1];
    DefaultIOU2[1] = SetupData->ConfigIOU2[1];

    DEBUG((EFI_D_INFO, "PEI GetProjectPciEBifurcate Start.\n"));
    DEBUG((-1, "DefaultIOU = %x, %x, %x\n", DefaultIOU0[0], DefaultIOU1[0], DefaultIOU2[0]));
    DEBUG((-1, "DefaultIOU = %x, %x, %x\n", DefaultIOU0[1], DefaultIOU1[1], DefaultIOU2[1]));

#if (IPMI_SUPPORT == 1) && (X11DPH_DETECT_BOARD_VIA_FRU == 1)
    DetectBoardIdFromIpmiFru(PeiServices,&board_id);
    if(board_id == BOARD_X11DPH_TQ){//QAT support board
        DEBUG((-1, "RP#0 PciEBifurcate for X11DPH-TQ : PCH x16 uplink\n"));
        SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxxxxx16;
        SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_1A_INDEX] = 1;//PCH x16 uplink
    }
    else{
        // Slot 1
        DEBUG((-1, "RP#0 PciEBifurcate for X11DPH-T : Slot 1 + PCH x8 uplink\n"));
        SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxx8xxx8;
        SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_1C_INDEX] = 1; // Slot 1
        SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_1A_INDEX] = 1;//PCH x8 uplink
    }
#else
    // Slot 1
    SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxx8xxx8;
    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_1C_INDEX] = 1; // Slot 1
    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_1A_INDEX] = 1;//PCH x8 uplink
#endif
    // Slot 3 + M.2
    SetupData->ConfigIOU1[0] = IIO_BIFURCATE_xxx8x4x4;
    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_2A_INDEX] = 1; // Slot 3
    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_2B_INDEX] = 1; // M.2 - C1
    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_2C_INDEX] = 1; // M.2 - C2

    // Slot 6 + Slot 7
    SetupData->ConfigIOU2[0] = IIO_BIFURCATE_xxx8xxx8;
    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_3A_INDEX] = 1; // Slot 6
    SetupData->SLOTIMP[SOCKET_0_INDEX + PORT_3C_INDEX] = 1; // Slot 7

    // Slot 2
    SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxxxxx16;
    SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_1A_INDEX] = 1; // Slot 2

    // Slot 5
    SetupData->ConfigIOU1[1] = IIO_BIFURCATE_xxxxxx16;
    SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_2A_INDEX] = 1; // Slot 5
    
    // Slot 4
    SetupData->ConfigIOU2[1] = IIO_BIFURCATE_xxxxxx16;
    SetupData->SLOTIMP[SOCKET_1_INDEX + PORT_3A_INDEX] = 1; // Slot 4
    

#if SmcAOC_SUPPORT
    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gSmcAOCPeiProtocolGuid,
                 0,
                 NULL,
                 &mSmcAOCPeiProtocol);
    DEBUG((-1, "LocatePpi mSmcAOCPeiProtocol = %r.\n", Status));
    if(!Status) {
        mSmcAOCPeiProtocol->SmcAOCPeiCardDetection(PeiServices, 0, 0, 0, SLOT_Tbl, sizeof(SLOT_Tbl)/sizeof(SMC_SLOT_INFO));
        mSmcAOCPeiProtocol->SmcAOCPeiBifurcation(SetupData, DefaultIOU0, DefaultIOU1, DefaultIOU2);
        mSmcAOCPeiProtocol->SmcAOCPeiSetNVMeMode(PeiServices, 0, 0, 0, SLOT_Tbl, sizeof(SLOT_Tbl)/sizeof(SMC_SLOT_INFO));
    }
#endif
    
    if(SetupData->ConfigIOU0[0] == 0xff){
    	SetupData->ConfigIOU0[0] = IIO_BIFURCATE_xxxxxx16;
    	SetupData->HidePEXPMenu[1] = 1;
    }
    if(SetupData->ConfigIOU1[0] == 0xff){
       	SetupData->ConfigIOU1[0] = IIO_BIFURCATE_xxxxxx16;
       	SetupData->HidePEXPMenu[5] = 1;
    }
    if(SetupData->ConfigIOU2[0] == 0xff){
       	SetupData->ConfigIOU2[0] = IIO_BIFURCATE_xxxxxx16;
       	SetupData->HidePEXPMenu[9] = 1;
    }
    if(SetupData->ConfigIOU0[1] == 0xff){
       	SetupData->ConfigIOU0[1] = IIO_BIFURCATE_xxxxxx16;
       	SetupData->HidePEXPMenu[22] = 1;
    }
    if(SetupData->ConfigIOU1[1] == 0xff){
       	SetupData->ConfigIOU1[1] = IIO_BIFURCATE_xxxxxx16;
       	SetupData->HidePEXPMenu[26] = 1;
    }
    if(SetupData->ConfigIOU2[1] == 0xff){
       	SetupData->ConfigIOU2[1] = IIO_BIFURCATE_xxxxxx16;
       	SetupData->HidePEXPMenu[30] = 1;
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
            SetupData->PciePortDisable[PORT_1A_INDEX] = 0;  //10Gbe LAN map to CPU PCIe port 1A
        }
    }
#endif

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
EFI_STATUS EFIAPI X11DPHPeiDriverInit(
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
