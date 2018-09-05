//***************************************************************************
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//***************************************************************************
//
//  File History
//
//  Rev. 1.04
//    Bug Fix:  Support SmcRomHole preserve in IPMI.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jan/06/2017
//
//  Rev. 1.03
//    Bug Fix:  Trigger a software SMI to flash ROM part after BIOS lock.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/30/2016
//
//  Rev. 1.02
//    Bug Fix:  Change Flash to SmcFlash for redefine issue.
//    Auditor:  Kasber Chen
//    Date:     Aug/18/2016
//
//  Rev. 1.01
//    Bug Fix:  Fix system hangs at Sum InBand command.
//    Reason:   Because of using Boot Service in SMM. Separate SmcRomHole into two libraries - DXE and SMM.
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/12/2016
//
//  Rev. 1.00
//    Bug Fix:  Renew the SMC ROM Hole.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     May/04/2016
//
//****************************************************************************
#include <token.h>
#include <SmcRomHole.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/FlashProtocol.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>


extern FLASH_PROTOCOL *SmcFlash;
#if DEBUG_MODE == 1
CHAR16* SmcRegionStr[] = {
    L"FULL_REGION",
    L"OA3_REGION",
    L"OA2_REGION",
    L"SMBIOS_FOR_BMC_REGION",
    L"AFU_CMD_REGION",
    L"INBAND_REGION",
    L"CURRENT_SMBIOS_REGION",
    L"PREVIOUS_SMBIOS_REGION"
};
#endif


//   OA3                  Save 0A3 context and restore it after BMC update BIOS
//   SmBiosForBmc         Save DMI edit infromation from NVRAM and restore it after BMC update BIOS
//   AfuCommand           Save afu update BIOS parameter
//   SmcInBand            Save BIOS cfg data(From inband and OOB) and update it into system in next reboot
//   SmcCurrentSmBios     Save full SMBIOS
//   OA2                  Save 0A2 context and restore it after BMC update BIOS
//   PreviousSmbios       Save full SMBIOS data for next time boot to compare last boot smbios data

UINT32
SmcRomHoleSize(
    IN	UINT8	Operation
)
{
    UINT32	SmcRomHoleSizeMap[16];
    SmcRomHoleSizeMap[OA3_REGION] = sizeof(((SMC_ROM_HOLE_MAP*)0)->OA3);
    SmcRomHoleSizeMap[SMBIOS_FOR_BMC_REGION] = sizeof(((SMC_ROM_HOLE_MAP*)0)->SmBiosForBmc);
    SmcRomHoleSizeMap[AFU_CMD_REGION] = sizeof(((SMC_ROM_HOLE_MAP*)0)->AfuCMD);
    SmcRomHoleSizeMap[INBAND_REGION] = sizeof(((SMC_ROM_HOLE_MAP*)0)->SmcInBand);
    SmcRomHoleSizeMap[CURRENT_SMBIOS_REGION] = sizeof(((SMC_ROM_HOLE_MAP*)0)->CurrentSmBios);
    SmcRomHoleSizeMap[OA2_REGION] = sizeof(((SMC_ROM_HOLE_MAP*)0)->OA2);
    SmcRomHoleSizeMap[PREVIOUS_SMBIOS_REGION] = sizeof(((SMC_ROM_HOLE_MAP*)0)->PreviousSmbios);
#if DEBUG_MODE == 1
    DEBUG((-1, "SmcRomHoleSize - %s (Size=0x%x)\n", SmcRegionStr[Operation], SmcRomHoleSizeMap[Operation]));
#endif
    return SmcRomHoleSizeMap[Operation];
}

UINT32
SmcRomHoleOffset(
    IN	UINT8	Operation
)
{
    UINT32	SmcRomHoleOffsetMap[16];
    SmcRomHoleOffsetMap[OA3_REGION] = offsetof(SMC_ROM_HOLE_MAP, OA3);
    SmcRomHoleOffsetMap[SMBIOS_FOR_BMC_REGION] = offsetof(SMC_ROM_HOLE_MAP, SmBiosForBmc);
    SmcRomHoleOffsetMap[AFU_CMD_REGION] = offsetof(SMC_ROM_HOLE_MAP, AfuCMD);
    SmcRomHoleOffsetMap[INBAND_REGION] = offsetof(SMC_ROM_HOLE_MAP, SmcInBand);
    SmcRomHoleOffsetMap[CURRENT_SMBIOS_REGION] = offsetof(SMC_ROM_HOLE_MAP, CurrentSmBios);
    SmcRomHoleOffsetMap[OA2_REGION] = offsetof(SMC_ROM_HOLE_MAP, OA2);
    SmcRomHoleOffsetMap[PREVIOUS_SMBIOS_REGION] = offsetof(SMC_ROM_HOLE_MAP, PreviousSmbios);
#if DEBUG_MODE == 1
    DEBUG((-1, "SmcRomHoleOffset - %s (Offset=0x%x)\n", SmcRegionStr[Operation], SmcRomHoleOffsetMap[Operation]));
#endif
    return SmcRomHoleOffsetMap[Operation];
}

EFI_STATUS
SmcRomHoleReadRegion(
    IN	UINT8	Operation,
    OUT	UINT8	*pBuffer
)
{
    EFI_STATUS	Status;
    UINT32	TempSize, Offset;

    DEBUG((-1, "SmcRomHoleReadRegion Start.\n"));
#if DEBUG_MODE == 1
    DEBUG((-1, "Operation = %s\n", SmcRegionStr[Operation]));
#endif

    if(SmcFlash == NULL) return EFI_NOT_READY;

    if(Operation == FULL_REGION) {
        TempSize = SMC_ROM_HOLE_SIZE;
        Offset = 0;
    } else {
        TempSize = SmcRomHoleSize(Operation);
        Offset = SmcRomHoleOffset(Operation);
    }

    Status = SmcFlash->Read((UINT8*)(SMC_ROM_HOLE_LOCATION + Offset), TempSize, pBuffer);
    DEBUG((-1, "SmcRomHoleReadRegion End.\n"));
    return Status;
}


