//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.24
//    Bug Fix: Merge AOC and PLX detection rule.
//    Reason:     
//    Auditor: Kasber Chen
//    Date:    Aug/21/2017
//
//  Rev. 1.23
//    Bug Fix: Add PLX detection for VMD mode. It's used to help create SMBIOS type 40.
//    Reason:     
//    Auditor: Jason Hsu
//    Date:    Aug/10/2017
//
//  Rev. 1.22
//    Bug Fix:  Correct SMBIOS Type 9/40 information for CPU1 PCIe slot.
//    Reason:   CPU has the NIC device which present on BUS 0x7F, 0xFF.
//              The BUS number checking would incorrect when the device doesn't present and read as 0xFF
//    Auditor:  Jimmy Chiu
//    Date:     Aug/01/2017
//
//  Rev. 1.21
//    Bug Fix:  Fix no NVMe temperature in IPMI web when VMD mode.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jun/12/2017
//
//  Rev. 1.20
//    Bug Fix:  Add PSU information checksum.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     May/16/2017
//
//  Rev. 1.19
//    Bug Fix:  Correct type 38 KCS base address.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Apr/13/2017
//
//  Rev. 1.18
//    Bug Fix:  Fix 4 LANs on chip type 40 show incorrect BDF.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Mar/13/2017
//
//  Rev. 1.17
//    Bug Fix:  Fix LAN2 type 40 AssetInfo data show incorrect.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Mar/06/2017
//
//  Rev. 1.16
//    Bug Fix:  Fix onboard device bus is incorrect.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jan/12/2017
//
//  Rev. 1.15
//    Bug Fix:  Fix sum can't change SMBIOS Type 1 UUID.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Dec/13/2016
//
//  Rev. 1.14
//      Bug Fixed:  Fix PSU always show present and status OK.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Nov/28/2016
//
//  Rev. 1.13
//      Bug Fixed:  Fix ultra IO riser card LAN always show disable.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Nov/02/2016
//
//  Rev. 1.12
//      Bug Fixed:  Create type 40 for onboard device(type 41).
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Oct/19/2016
//
//  Rev. 1.11
//      Bug Fixed:  Add type 17 memory manufacturer strings override function.
//      Reason:     
//      Auditor:    Wayne Chen
//      Date:       Oct/17/2016
//
//  Rev. 1.10
//      Bug Fixed:  Fix incorrect device number in type 41.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Oct/04/2016
//
//  Rev. 1.09
//      Bug Fixed:  Fix Bus dynamic change issue.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Sep/06/2016
//
//  Rev. 1.08
//    Bug Fix:  Remove slot status and provide SSID and SVID for SW requirement.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Aug/24/2016
//
//  Rev. 1.07
//    Bug Fix:  Correct type 40 entry length.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Aug/22/2016
//
//  Rev. 1.06
//    Bug Fix:  Fill slot status into type 9 for BMC get device info.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Aug/03/2016
//
//  Rev. 1.05
//    Bug Fix:  Fix LAN on bridge can't detect.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Aug/02/2016
//
//  Rev. 1.04
//    Bug Fix:  Get MAC from SuperM protocol.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/29/2016
//
//  Rev. 1.03
//    Bug Fix:  Add VPD data to type 40.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/26/2016
//
//  Rev. 1.02
//    Bug Fix:  Add type 16 and type 17, define type token.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/20/2016
//
//  Rev. 1.01
//    Bug Fix:  Remove SmBiosType00Update routine to avoid version cannot be changed by tool problem.
//    Reason:   The default BIOS version has been created in SmbData.mac, no need to update it every time.
//    Auditor:  Jacker Yeh
//    Date:     Jul/19/2016
//
//  Rev. 1.00
//    Bug Fix:  Renew the SMC ROM Hole.
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/04/2016
//
//****************************************************************************
//****************************************************************************
#include <Token.h>
#include <AmiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MmPciBaseLib.h>
#include <Protocol/SMBios.h>
#include <Protocol/AmiSmbios.h>
#include <Protocol/SmbiosDynamicData.h>
#include <PciBus.h>
#include <TimeStamp.h>
#include <Library/HobLib.h>
#include <Guid/MemoryMapData.h>
#include <Protocol/AmiSmbios.h>
#include "SuperMDxeDriver.h"
#include "SmcLib.h"
#include "PlatformSmBios.h"
#include "SmcSmBios.h"
#include "SmcVPD.h"
#include "SmcSlotStatus.h"
#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>

#if IPMI_SUPPORT
#include "IPMITransportProtocol.h"

typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     InterfaceType;
    UINT8                     IPMISpecRevision;
    UINT8                     I2CSlaveAddr;
    UINT8                     NVStorageDevAddr;
    UINT64                    BaseAddress;
}SMBIOS_IPMI_DEV_INFO;                                       // TYPE 38
#endif //IPMI_SUPPORT

#ifndef offsetof
#define offsetof(st, m)	((UINTN)((char*)&((st*)0)->m - (char*)0))
#endif

extern EFI_RUNTIME_SERVICES *gRT;

extern	AMI_SMBIOS_PROTOCOL	*SmbiosProtocol;
extern	UINT8	SMB_Func, Instance, Temp_Instance;
IMC_DIMM_NUM	IMC_DIMM[] = {
		CPU1_DIMM_NUM, CPU1_DIMM_NUM,
		CPU2_DIMM_NUM, CPU2_DIMM_NUM};

EFI_STATUS
SmBiosType130Create(IN  UINT16  UpdateHandle)
{
    UINT32  ReturnVal0 = 0, ReturnVal1 = 0, ReturnVal2 = 0, ReturnVal3 = 0, SXB1_ID = 0, SXB2_ID = 0;
    EFI_STATUS Status = EFI_SUCCESS;
    AMI_SMBIOS_PROTOCOL *SmbiosProtocol;
    UINT8   *SMBIOS_TYPE130;
    UINT8   SMBIOS_TYPE130_DEFAULT[] = {0x82, 0x20, 0xFF, 0xFF, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
                                        0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    CHAR8*  BpnString;

    DEBUG((EFI_D_INFO, "X11DPTB Create SMBIOS Type 130 start.\n"));

    Status = gBS->LocateProtocol(&gAmiSmbiosProtocolGuid,NULL,&SmbiosProtocol);

    Status = gBS->AllocatePool(EfiBootServicesData, 0x50, &SMBIOS_TYPE130);
    if(!Status)
	{
      //SXB1
      gBS->SetMem(SMBIOS_TYPE130, 0x50, 0);

      GpioGetInputValue(SXB1_ID0, &ReturnVal0);
      GpioGetInputValue(SXB1_ID1, &ReturnVal1);
      GpioGetInputValue(SXB1_ID2, &ReturnVal2);
      SXB1_ID = (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;

      switch(SXB1_ID)
      {
        case 3:
          SMBIOS_TYPE130_DEFAULT[0x0A] = 0x01;
          BpnString = "BPN-ADP-6SATA3-1UB";
          break;
        case 6:
          SMBIOS_TYPE130_DEFAULT[0x0A] = 0x01;
          BpnString = "AOC-SMG3-2H8M2-B";
          break;
        default:
          break;
      }

      if(SMBIOS_TYPE130_DEFAULT[0x0A])
      {
        MemCpy(SMBIOS_TYPE130, SMBIOS_TYPE130_DEFAULT, 0x20);
        AsciiStrCpy(&SMBIOS_TYPE130[0x20], BpnString);
        Status = SmbiosProtocol->SmbiosAddStructure(SMBIOS_TYPE130, ((UINT16)AsciiStrLen(BpnString) + 0x20 + 2));
        SMBIOS_TYPE130_DEFAULT[0x0A] = 0x00;
      }

      //SXB2
      gBS->SetMem(SMBIOS_TYPE130, 0x50, 0);

      GpioGetInputValue(SXB2_ID0, &ReturnVal0);
      GpioGetInputValue(SXB2_ID1, &ReturnVal1);
      GpioGetInputValue(SXB2_ID2, &ReturnVal2);
      SXB2_ID = (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;

      switch (SXB2_ID)
      {
        case 6:
          SMBIOS_TYPE130_DEFAULT[0x0A] = 0x02;
          BpnString = "BPN-ADP-6NVMe3-1UB";
          break;
        case 5:
          SMBIOS_TYPE130_DEFAULT[0x0A] = 0x02;
          BpnString = "BPN-ADP-6S3008N4-1UB";
          break;
        case 4:
          SMBIOS_TYPE130_DEFAULT[0x0A] = 0x02;
          BpnString = "BPN-ADP-12NVMe-2UB";
          break;
        case 3:
          SMBIOS_TYPE130_DEFAULT[0x0A] = 0x02;
          if (SXB1_ID == 5) BpnString = "BPN-ADP-6S3008N4-2UB";
          else              BpnString = "BPN-ADP-12S3216N4-2UB";
        case 1:
          SMBIOS_TYPE130_DEFAULT[0x0A] = 0x02;
          BpnString = "BPN-ADP-4TNS6-1UB";
        default:
          break;
      }

      if(SMBIOS_TYPE130_DEFAULT[0x0A])
      {
        MemCpy(SMBIOS_TYPE130, SMBIOS_TYPE130_DEFAULT, 0x20);
        AsciiStrCpy(&SMBIOS_TYPE130[0x20], BpnString);
        Status = SmbiosProtocol->SmbiosAddStructure(SMBIOS_TYPE130, ((UINT16)AsciiStrLen(BpnString) + 0x20 + 2));
      }
      gBS->FreePool(SMBIOS_TYPE130);
    }
    DEBUG((EFI_D_INFO, "X11DPTB Create SMBIOS Type 130 end.\n"));
    return EFI_SUCCESS;	
}

UINT8   IsAOC;

UINT8
SmcAOCCheck(
    IN  UINT32  TempBDF
)
{
    UINT8       CPU, IOU, IOU_AOC, IOU_PLX, ShiftValue = 0;

    DEBUG((-1, "SmcAOCCheck Enter.\n"));

    CPU = ((UINT8)(TempBDF >> 20) & 0x0f);
    IOU = ((UINT8)(TempBDF >> 16) & 0x0f);
    if((CPU > 4) || (CPU < 1) || (IOU > 5))     return 0;

    ShiftValue = ((CPU - 1) * 32) + ((IOU - 1) * 8);
    IOU_AOC = (UINT8)(PcdGet64(PcdSmcAOCLocation) >> ShiftValue);
    IOU_PLX = (UINT8)(PcdGet64(PcdSmcAOCExtendFlag) >> ShiftValue);

    if((UINT8)TempBDF == 0x00)  return (IOU_AOC & 0x03);        //IOU A port
    if((UINT8)TempBDF == 0x08)  return (IOU_AOC & 0x0C);        //IOU B port
    if((UINT8)TempBDF == 0x10)  return (IOU_AOC & 0x30);        //IOU C port
    if((UINT8)TempBDF == 0x18)  return (IOU_AOC & 0xC0);        //IOU D port
    if((UINT8)TempBDF == 0x00)  return (IOU_PLX & 0x02);        //IOU A port
    if((UINT8)TempBDF == 0x08)  return (IOU_PLX & 0x08);        //IOU B port
    if((UINT8)TempBDF == 0x10)  return (IOU_PLX & 0x20);        //IOU C port
    if((UINT8)TempBDF == 0x18)  return (IOU_PLX & 0x80);        //IOU D port

    return 0;
    DEBUG((-1, "SmcAOCCheck End.\n"));
}

EFI_STATUS
SmBiosType40Create(
    IN	UINT16	UpdateHandle,
    IN	UINT8	*Bus,
    IN	UINT8	*Dev,
    IN	UINT8	*Fun,
    IN	UINT8	*BusE,
    IN	UINT16	SlotID
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    AssetInfo_Header	*AssetInfo_H;
    VPD_HEADER		*VPD_H;
    UINT8	*TempType40, TempType40Size;
    UINT8	ClassCode, SubClassCode, ProgramInterface;
    UINT16	VPD_Len;
    UINT32	VID_DID = 0, SVID_SSID = 0;

    DEBUG((-1, "SmBiosType40Create Entry.\n"));

//Get SSVID and SSID
    if(*(UINT16*)MmPciAddress(0, *Bus, *Dev, 0, 0x0A) == 0x0604){	//first bridge
        if(*(UINT16*)MmPciAddress(0, *Bus, *Dev, 0, 0xA8) == 0x15D9){	//SMC PLX NVMe card
            VID_DID = *(UINT32*)MmPciAddress(0, *Bus, *Dev, 0, 0xA8);
            SVID_SSID = *(UINT32*)MmPciAddress(0, *Bus, *Dev, 0, 0xA8);
            *BusE = 0xff;
        }
        for( ; (*Bus <= *BusE) && (*BusE != 0xff); (*Bus)++){
            for(*Dev = 0; *Dev <= 31; (*Dev)++){
                for(*Fun = 0; *Fun <= 7; (*Fun)++){
                    if(*(UINT16*)MmPciAddress(0, *Bus, *Dev, *Fun, 0) == 0xffff)
                        continue;
                    if(*(UINT16*)MmPciAddress(0, *Bus, *Dev, *Fun, 0x0A) == 0x0604){//second bridge
                        *Dev = 32;
                        *Fun = 8;
                        continue;
                    }
                    VID_DID = *(UINT32*)MmPciAddress(0, *Bus, *Dev, *Fun, 0);
                    SVID_SSID = *(UINT32*)MmPciAddress(0, *Bus, *Dev, *Fun, 0x2C);
                    break;
                }
                if(SVID_SSID)   break;
            }
            if(SVID_SSID)       break;
        }
    } else {
        VID_DID = *(UINT32*)MmPciAddress(0, *Bus, *Dev, *Fun, 0);
        SVID_SSID = *(UINT32*)MmPciAddress(0, *Bus, *Dev, *Fun, 0x2C);
    }
//when VMD enable, root port and device will be hidden but we still need to report
//the slot is NVMe, so we add a virtual VID, DID, classcode, sub-classcode for IPMI
    if(IsAOC && (VID_DID == 0xffffffff)){
        VID_DID = 0x09538086;
        ClassCode = 0x01;
        SubClassCode = 0x08;
        ProgramInterface = 0x01;
    } else if ((*Bus == 0xFF) || (*BusE == 0xFF)){
        VID_DID = 0xFFFFFFFF;
        SVID_SSID = 0xFFFFFFFF;
        ClassCode = 0xFF;
        SubClassCode = 0xFF;
        ProgramInterface = 0xFF;
    } else {
        ClassCode = *(UINT8*)MmPciAddress(0, *Bus, *Dev, *Fun, 0x0B);
        SubClassCode = *(UINT8*)MmPciAddress(0, *Bus, *Dev, *Fun, 0x0A);
        ProgramInterface = *(UINT8*)MmPciAddress(0, *Bus, *Dev, *Fun, 0x09);
    }

    DEBUG((-1, "Bus = %x, Dev = %x, Fun = %x.\n", *Bus, *Dev, *Fun));
    DEBUG((-1, "VID_DID = %x, SVID_SSID = %x.\n", VID_DID, SVID_SSID));
// create type 40
    Status = gBS->AllocatePool(EfiBootServicesData, 0xff, &TempType40);
    if(!Status){
        gBS->SetMem(TempType40, 0xff, 0x00);
//AssetInfo area
        TempType40Size = 10;	// point to AssetInfo area in entry format
        AssetInfo_H = (AssetInfo_Header*)(TempType40 + TempType40Size);
        gBS->CopyMem(AssetInfo_H->Signature, "_AST", 4);
        AssetInfo_H->Version = AssetInfo_VERSION;
        AssetInfo_H->AST_H_Len = sizeof(AssetInfo_Header);
        *(UINT32*)(AssetInfo_H->VID_DID) = VID_DID;
        AssetInfo_H->ClassCode = ClassCode;
        AssetInfo_H->SubClassCode = SubClassCode;
        AssetInfo_H->ProgramInterface = ProgramInterface;
        *(UINT32*)(AssetInfo_H->SVID_SSID) = SVID_SSID;
        TempType40Size += sizeof(AssetInfo_Header);	//point to VPD area in entry format
        ((SMBIOS_ADDITIONAL_INFO*)TempType40)->EntryLength = 6;	//minimum
        ((SMBIOS_ADDITIONAL_INFO*)TempType40)->EntryLength += sizeof(AssetInfo_Header);
//VPD area
        Status = SmcVPDPkgCollect(*Bus, 0, 0, (UINT8)(SlotID >> 8), (UINT8)SlotID,
                        &TempType40[TempType40Size + sizeof(VPD_HEADER)], &VPD_Len);
        if(!Status){
            VPD_H = (VPD_HEADER*)(TempType40 + TempType40Size);
            gBS->CopyMem(VPD_H->Signature, "_VPD", 4);
            VPD_H->Version = VPD_VERSION;
            VPD_H->VPD_H_Length = sizeof(VPD_HEADER);
            TempType40Size += (sizeof(VPD_HEADER) + (UINT8)VPD_Len);
            ((SMBIOS_ADDITIONAL_INFO*)TempType40)->EntryLength += (sizeof(VPD_HEADER) + (UINT8)VPD_Len);
        }
        DEBUG((-1, "TempType40Size %x.\n", TempType40Size));
        ((SMBIOS_ADDITIONAL_INFO*)TempType40)->Type = 40;
        ((SMBIOS_ADDITIONAL_INFO*)TempType40)->Length = TempType40Size;
        ((SMBIOS_ADDITIONAL_INFO*)TempType40)->Handle = 0xffff;
        ((SMBIOS_ADDITIONAL_INFO*)TempType40)->NumAdditionalInfoEntries = 1;
        ((SMBIOS_ADDITIONAL_INFO*)TempType40)->RefHandle = UpdateHandle;
        ((SMBIOS_ADDITIONAL_INFO*)TempType40)->RefOffset = 0;
        ((SMBIOS_ADDITIONAL_INFO*)TempType40)->StringNum = 0;

        TempType40Size += 2;	//two byte for end
        Status = SmbiosProtocol->SmbiosAddStructure(TempType40, TempType40Size);
        gBS->FreePool(TempType40);
        DEBUG((-1, "SmbiosAddStructure type 40 %r.\n", Status));
    }
    DEBUG((-1, "SmBiosType40Create End.\n"));
    return EFI_SUCCESS;
}

typedef struct {
  UINT16  MfgId;
  UINT8  *String;
} MEMORY_MODULE_MANUFACTURER_LIST;

MEMORY_MODULE_MANUFACTURER_LIST MemoryModuleManufacturerList[] =
{
  { 0x0100, "AMD" },
  { 0x0400, "Fujitsu" },
  { 0x0700, "Hitachi" },
  { 0x8900, "Intel" },
  { 0x1000, "NEC" },
  { 0x9700, "Texas Instruments" },
  { 0x9800, "Toshiba" },
  { 0x1c00, "Mitsubishi" },
  { 0x1f00, "Atmel" },
  { 0x2000, "STMicroelectronics" },
  { 0xa400, "IBM" },
  { 0x2c00, "Micron Technology" },
  { 0xad00, "SK Hynix" },
  { 0xb000, "Sharp" },
  { 0xb300, "IDT" },
  { 0x3e00, "Oracle Corporation" },
  { 0xbf00, "Silicon Storage Technology" },
  { 0x4000, "ProMos/Mosel Vitelic" },
  { 0xc100, "Infineon (Siemens)" },
  { 0xc200, "Macronix" },
  { 0x4500, "SanDisk Corporation" },
  { 0xce00, "Samsung" },
  { 0xda00, "Winbond Electronic" },
  { 0xe000, "LG Semi (Goldstar)" },
  { 0x6200, "Sanyo" },
  { 0x3d00, "Tektronix" },
  { 0x9801, "Kingston" },
  { 0x9401, "Smart Modular" },
  { 0xba01, "PNY Technologies, Inc." },
  { 0x4f01, "Transcend Information" },
  { 0x7a01, "Apacer Technology" },
  { 0x9e02, "Corsair" },
  { 0xfe02, "Elpida" },
  { 0xc802, "Agilent Technologies" },
  { 0x0b03, "Nanya Technology" },
  { 0x9403, "Mushkin" },
  { 0x2503, "Kingmax Semiconductor" },
  { 0xb004, "OCZ" },
  { 0xcb04, "A-DATA Technology" },
  { 0xcd04, "G Skill Intl" },
  { 0xef04, "Team Group Inc." },
  { 0xb304, "Inphi Corporation" },
  { 0x0205, "Patriot Memory (PDP Systems)" },
  { 0x9b05, "Crucial Technology" },
  { 0x5105, "Qimonda" },
  { 0x5705, "AENEON" },
  { 0xf705, "Avant Technology" },
  { 0x3406, "Super Talent" },
  { 0xd306, "Silicon Power Computer & Communications" },
  { 0xf106, "InnoDisk Corporation" },
  { 0x3206, "Montage Technology Group" }
};


/**
    Returns the length of the structure pointed by BufferStart
    in bytes

    @param BufferStart

    @return UINT16 Size of the structure

**/
UINT16
GetStructureLength(
    IN UINT8     *BufferStart
)
{
    UINT8       *BufferEnd = BufferStart;

    BufferEnd += ((SMBIOS_STRUCTURE_HEADER*)BufferStart)->Length;

    while (*(UINT16*)BufferEnd != 0) {
        BufferEnd++;
    }

    return (UINT16)(BufferEnd + 2 - BufferStart);   // +2 for double zero terminator
}

/**
    Returns the string offset for StringNumber from input string
    buffer BufferStart

    @param BufferStart
    @param StringNumber (1-based)

    @return UINT16 Offset from BufferStart

**/
UINT16
GetStringOffset(
    IN UINT8    *BufferStart,
    IN UINT8    StringNumber                    // 1-based
)
{
    UINT8       *BufferEnd = BufferStart;

    while (--StringNumber) {
        while(*BufferEnd != 0) {
            BufferEnd++;
        }
        BufferEnd++;
    }

    return (UINT16)(BufferEnd - BufferStart);
}

/**
    Returns pointer to the string number in structure BufferPtr

    @param BufferPtr
    @param StringNumber

    @retval TRUE    *BufferPtr = Pointer to the #StringNumber string

**/
BOOLEAN
FindString(
    IN OUT UINT8    **BufferPtr,
    IN     UINT8    StringNumber          // 1-based
)
{
    UINT8       *StructurePtr;
    UINT8       *StructureEnd;

    StructurePtr = *BufferPtr;
    StructureEnd = StructurePtr + GetStructureLength(StructurePtr);
    StructurePtr += ((SMBIOS_STRUCTURE_HEADER*)StructurePtr)->Length;
    StructurePtr += GetStringOffset(StructurePtr, StringNumber);
    if (StructurePtr >= StructureEnd) {
        return FALSE;
    }
    else {
        *BufferPtr = StructurePtr;
        return TRUE;
    }
}


/**
    Replace the #StringNum in the input buffer *DestStructPtr
    with StringData

    @param DestStructPtr Pointer to structure to be updated
    @param StringNum String number (1 based)
    @param StringData String with NULL terminated character

    @return EFI_STATUS

**/
EFI_STATUS
ReplaceSmbiosString(
    IN UINT8    *DestStructPtr,
    IN OUT  UINT16  *UpdateBufferSize,
    IN UINT8    StringNum,
    IN UINT8    *StringData
)
{
    UINT8       StringSize = 0, OrgStringSize = 0;
    UINT8       *StructEndPtr;
    UINTN       RemainingSize;

    if (FindString(&DestStructPtr, StringNum)) {
        UINT8       *TempPtr;
        UINT8       *NextStrPtr;
        
        if(Strcmp ("NO DIMM", DestStructPtr) == 0)
        {
        	return EFI_NOT_FOUND;
        }

        NextStrPtr = DestStructPtr;
        StructEndPtr = DestStructPtr;

        while(*NextStrPtr != 0) {
            NextStrPtr++;
        }

        // NextStrPtr = Pointer to the next string
        NextStrPtr++;

        while(*(UINT16*)StructEndPtr != 0) {
            StructEndPtr++;
        }

        RemainingSize = StructEndPtr + 2 - NextStrPtr;  // Including double NULL characters

        TempPtr = StringData;
        while (*(TempPtr++) != 0) {
            StringSize++;
        }
        StringSize++;                   // Including NULL character

        OrgStringSize = (UINT8)(NextStrPtr - DestStructPtr);
        DEBUG((-1, "OrgStringSize = %X, StringSize = %X\n", OrgStringSize, StringSize));

        // Copy remaining strings
        MemCpy(DestStructPtr + StringSize, NextStrPtr, RemainingSize);

        // Copy the string
        MemCpy(DestStructPtr, StringData, StringSize);

        // Calculate the structure size
        *UpdateBufferSize = *UpdateBufferSize + StringSize - OrgStringSize;

        return EFI_SUCCESS;
    }
    else {
        // String not found
        return EFI_NOT_FOUND;
    }
}

EFI_STATUS
SmBiosType00Update(
    IN	UINT16	UpdateHandle,
    IN OUT	UINT8	*UpdateBuffer,
    IN OUT	UINT16	*UpdateBufferSize
)
{
#if (OFFICIAL_RELEASE == 1)
    UINT8	reg;

    DEBUG((-1, "SmBiosType00Update Start.\n"));
    reg = (UINT8)Strlen(CONVERT_TO_STRING(BIOS_VERSION));
    gBS->CopyMem(&(((UINT8*)UpdateBuffer)[0x31]), (CONVERT_TO_STRING(BIOS_VERSION)), (UINTN)reg);
#else
    DEBUG((-1, "SmBiosType00Update Start.\n"));
    gBS->CopyMem(&UpdateBuffer[0x31], CONVERT_TO_STRING(T), 1);
    gBS->CopyMem(&UpdateBuffer[0x32], CONVERT_TO_STRING(THIS_YEAR), 4);
    gBS->CopyMem(&UpdateBuffer[0x36], CONVERT_TO_STRING(THIS_DIGIT_MONTH), 2);
    gBS->CopyMem(&UpdateBuffer[0x38], CONVERT_TO_STRING(THIS_DIGIT_DAY), 2);
    gBS->CopyMem(&UpdateBuffer[0x3A], CONVERT_TO_STRING(THIS_DIGIT_HOUR), 2);
    gBS->CopyMem(&UpdateBuffer[0x3C], CONVERT_TO_STRING(THIS_DIGIT_MINUTE), 2);
    gBS->CopyMem(&UpdateBuffer[0x3E], CONVERT_TO_STRING(THIS_DIGIT_SECOND), 2);
#endif
    DEBUG((-1, "SmBiosType00Update End.\n"));
    return EFI_SUCCESS;
}

EFI_STATUS
SmBiosType01Update(
    IN	UINT16	UpdateHandle,
    IN OUT	VOID	*UpdateBuffer,
    IN OUT	UINT16	*UpdateBufferSize
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8 TempUpdate[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    SUPERMDRIVER_PROTOCOL *gSuperMProtocol;
    SUPERMBOARDINFO gSuperMBoardInfo;
    UINTN TempN = 0;
    CHAR16 *S1 = L"                ";
    CHAR8 Buffer[0x400];
    UINTN BufferSize = sizeof(Buffer);

    DEBUG((-1, "SmBiosType01Update Start.\n"));
    
    Status = gBS->LocateProtocol(
		    &gSuperMDriverProtocolGuid, 
		    NULL, 
		    &gSuperMProtocol);

    if(Status)	return EFI_SUCCESS;

    //
    // Sum would save data to DmiVar, check it.
    //
	TempN = Swprintf (S1, L"DmiVar%02x%04x%02x%02x",
            1, // Type 1
            UpdateHandle,
            offsetof (SMBIOS_SYSTEM_INFO, Uuid), // 08h - UUID
            0);
	Status = gRT->GetVariable (
            S1,
            &gAmiSmbiosNvramGuid,
            NULL,
            &BufferSize,
            &Buffer);

    //
    // If UUID is changed by sum, return, don't get UUID from LAN1 MAC.
    //
    if (!EFI_ERROR (Status)) return EFI_SUCCESS;

    Status = gSuperMProtocol->GetSuperMBoardInfo(&gSuperMBoardInfo);
    gBS->CopyMem(&(TempUpdate[10]), &(gSuperMBoardInfo.HardwareInfo[4]), 6);

    gBS->CopyMem(&(((SMBIOS_SYSTEM_INFO*)UpdateBuffer)->Uuid), TempUpdate, sizeof(EFI_GUID));
    DEBUG((-1, "SmBiosType01Update End.\n"));
    return EFI_SUCCESS;
}
/*
EFI_STATUS
SmBiosType02Update(
    IN	UINT16	UpdateHandle,
    IN OUT	VOID	*UpdateBuffer,
    IN OUT	UINT16	*UpdateBufferSize
)
{
    EFI_STATUS	Status = EFI_SUCCESS;

    return Status;
}
*/
///
///EFI_STATUS
///SmBiosType03Update (
///  VOID
///  )
///{
///  return EFI_SUCCESS;
///}
///
///
///EFI_STATUS
///SmBiosType04Update (
///  VOID
///  )
///{
///  return EFI_SUCCESS;
///}
///
///
///EFI_STATUS
///SmBiosType07Update (
///  VOID
///  )
///{
///  return EFI_SUCCESS;
///}
///
///
EFI_STATUS
SmBiosType09Update(
    IN	UINT16	UpdateHandle,
    IN OUT	UINT8	*UpdateBuffer,
    IN OUT	UINT16	*UpdateBufferSize
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	Bus, BusE, Dev, Fun;
    UINT32	TempBDF;

    DEBUG((-1, "SmBiosType09Update Start.\n"));  //SMBIOS_SYSTEM_SLOTS_INFO
//transfer BDF define
    TempBDF = ((UINT32)(((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->SegGroupNumber) << 16) +
	((UINT32)(((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->BusNumber) << 8) +
	(UINT32)(((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->DevFuncNumber);
//check any AOC in slot
    IsAOC = SmcAOCCheck(TempBDF);

    TempBDF = (UINT32)SmcBDFTransfer(TempBDF);
    ((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->SegGroupNumber = (UINT16)(TempBDF >> 16);
    ((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->BusNumber = (UINT8)(TempBDF >> 8);
    ((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->DevFuncNumber = (UINT8)TempBDF;
//
    Bus = ((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->BusNumber;
    Dev = (((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->DevFuncNumber) >>3;
    Fun = (((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->DevFuncNumber) & 0x07;
// Get secondary bus number
    BusE = *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, 0x1A);
    Bus = *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, 0x19);
    for( ; BusE >= Bus; BusE--){
        if(*(UINT8*)MmPciAddress(0, Bus, 0, 0, 0) != 0xff)
            break;
    }
// Update bus number
    ((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->BusNumber = Bus;
    ((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->DevFuncNumber = 0;
    Dev = (((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->DevFuncNumber) >>3;
    Fun = (((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->DevFuncNumber) & 0x07;
// Update slot CurrentUsage
    if((*(UINT32*)MmPciAddress(0, Bus, 0, 0, 0) == 0xFFFFFFFF) || (Bus == 0xFF) || (BusE == 0xFF))
	((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->CurrentUsage = 0x03;	//available
    else {
	((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->CurrentUsage = 0x04;	//In use
//	((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->SlotChar_2 |= SmcSlotStatus(Bus);
    }

    SmBiosType40Create(UpdateHandle, &Bus, &Dev, &Fun, &BusE, ((SMBIOS_SYSTEM_SLOTS_INFO*)UpdateBuffer)->SlotID);

    DEBUG((-1, "SmBiosType09Update End.\n"));
    return EFI_SUCCESS;
}

///
///
///EFI_STATUS
///SmBiosType11Update (
///  VOID
///  )
///{
///  return EFI_SUCCESS;
///}
///
///
///EFI_STATUS
///SmBiosType15Update (
///  VOID
///  )
///{
///  return EFI_SUCCESS;
///}
///
///
EFI_STATUS
SmBiosType16Update (
    IN	UINT16	UpdateHandle,
    IN OUT	UINT8	*UpdateBuffer,
    IN OUT	UINT16	*UpdateBufferSize
)
{
    UINT8	i, j = 0;
    UINT8	Max_Instance = sizeof(IMC_DIMM) / sizeof(IMC_DIMM_NUM);

    DEBUG((-1, "SmBiosType16Update entry.\n"));

    for(i = 0; i < CHANNEL_PER_IMC; i++)
	j += IMC_DIMM[Temp_Instance].DIMM_NUM[i];

    if(j || (Temp_Instance > Max_Instance)){	//you define DIMM number in the IMC
	SMB_Func = SkipStructure;
    }
    else {	//the IMC can delete
	SMB_Func = DeleteStructure;
	Instance--;
    }
    Temp_Instance++;

    DEBUG((-1, "SmBiosType16Update end.\n"));
    return EFI_SUCCESS;
}

/**
    Returns the location of DIMM base on Type 17 Device Locator string.

    @param UpdateBuffer

    @return EFI_STATUS

**/
EFI_STATUS
GetDimmLocate(
    IN UINT8   *UpdateBuffer,
    IN OUT  UINT8  *socket,
    IN OUT  UINT8  *ch,
    IN OUT  UINT8  *dimm
)
{
	SMBIOS_MEMORY_DEVICE_INFO     *Type17 = (SMBIOS_MEMORY_DEVICE_INFO*)UpdateBuffer;

    if (FindString(&UpdateBuffer, Type17->DeviceLocator)) {
        //ex, P1-DIMMC1
        *socket = UpdateBuffer[1] - 0x31; //ASCII(base 1) to UINT8(base 0)
        *ch = UpdateBuffer[7] - 0x41;     //ASCII(base A) to UINT8(base 0)
        *dimm = UpdateBuffer[8] - 0x31;   //ASCII(base 1) to UINT8(base 0)
        
        DEBUG((-1, "GetDimmLocate socket %d, channel %d, dimm %d\n", *socket, *ch, *dimm ));

        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}    

EFI_STATUS
SmBiosType17Update(
    IN	UINT16	UpdateHandle,
    IN OUT	UINT8	*UpdateBuffer,
    IN OUT	UINT16	*UpdateBufferSize
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	i, j, k;
    UINT8	Max_Instance = sizeof(IMC_DIMM) * 2;
    EFI_HOB_GUID_TYPE             *GuidHob = NULL;
    struct SystemMemoryMapHob     *SystemMemoryMap;
    UINT8                         socket, ch, dimm;
    SMBIOS_MEMORY_DEVICE_INFO     *Type17 = (SMBIOS_MEMORY_DEVICE_INFO*)UpdateBuffer;
              
    DEBUG((-1, "SmBiosType17Update entry.\n"));

    i = Temp_Instance / (CHANNEL_PER_IMC * DIMM_PER_CHANNEL);	// max DIMM per IMC
    j = (Temp_Instance - (i * CHANNEL_PER_IMC * DIMM_PER_CHANNEL)) / DIMM_PER_CHANNEL;	// max DIMM per channel
    k = Temp_Instance - (i * CHANNEL_PER_IMC * DIMM_PER_CHANNEL) - (j * DIMM_PER_CHANNEL);
    
    if((k < IMC_DIMM[i].DIMM_NUM[j]) || (Temp_Instance > Max_Instance)){
        SMB_Func = SkipStructure;
    }
    else{
        SMB_Func = DeleteStructure;
        Instance--;	//when we del one, instance need to del one to get next structure
    }
    Temp_Instance++;

    //
    // Override Manufacturer string based on memory map HOB.
    //
    if( SMB_Func == 0 ){
        GuidHob = GetFirstGuidHob( &gEfiMemoryMapGuid );
        if( GuidHob != NULL ) {
            SystemMemoryMap = (struct SystemMemoryMapHob *) GET_GUID_HOB_DATA( GuidHob );

            Status = GetDimmLocate( UpdateBuffer, &socket, &ch, &dimm );
            if(EFI_ERROR (Status)){
                return EFI_SUCCESS;
            }
            DEBUG((-1, "MMfgId = %X\n", SystemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].SPDMMfgId ));
            for( i=0; i<sizeof(MemoryModuleManufacturerList)/sizeof(MEMORY_MODULE_MANUFACTURER_LIST); i++ )
            {
                if(SystemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].SPDMMfgId == MemoryModuleManufacturerList[i].MfgId)
                {
                	DEBUG((-1, "ReplaceSmbiosString Manufacturer %d\n", i ));
                    Status = ReplaceSmbiosString( UpdateBuffer, UpdateBufferSize, Type17->Manufacturer, MemoryModuleManufacturerList[i].String );
                    if(!EFI_ERROR (Status)){
                        SMB_Func = WriteStructure;
                    }
                    Status = EFI_SUCCESS;
                    break;
                } 
            }
        }
    }
    
    DEBUG((-1, "SmBiosType17Update end.\n"));
    return EFI_SUCCESS;
}
///
///
///EFI_STATUS
///SmBiosType19Update (
///  VOID
///  )
///{
///  return EFI_SUCCESS;
///}
///
///
///EFI_STATUS
///SmBiosType20Update (
///  VOID
///  )
///{
///  return EFI_SUCCESS;
///}
///
///
///EFI_STATUS
///SmBiosType32Update (
///  VOID
///  )
///{
///  return EFI_SUCCESS;
///}
///
///
EFI_STATUS
SmBiosType38Update (
    IN	UINT16	UpdateHandle,
    IN OUT	UINT8	*UpdateBuffer,
    IN OUT	UINT16	*UpdateBufferSize
  )
{
    DEBUG((-1, "SmBiosType38Update Start.\n"));
#if IPMI_SUPPORT
    ((SMBIOS_IPMI_DEV_INFO*)UpdateBuffer)->BaseAddress = IPMI_KCS_BASE_ADDRESS + 1;
#endif
    SmBiosType130Create(UpdateHandle);
    DEBUG((-1, "SmBiosType38Update End.\n"));
    return EFI_SUCCESS;
}

EFI_STATUS
SmBiosType39Update(
    IN	UINT16	UpdateHandle,
    IN OUT	UINT8	*UpdateBuffer,
    IN OUT	UINT16	*UpdateBufferSize
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
#if IPMI_SUPPORT
    UINT8       BMCCMDData[2];  // PSU#, FRU Block#
    UINT8       FRUBlockData[128], *pFRUBlockData, FRUBlockChar; // first 96 bytes
    UINT8       FRUBlockDataSize = 32;
    UINT8       i, j = 23, k = 2;       // j: type39 string start address; k: start from string#2
    UINT8       PLength, cksum1, cksum2, cksumRetry = 0;

    DEBUG((-1, "SmBiosType39Update Start.\n"));
//default status = unknown and not present
    ((SMBIOS_SYSTEM_PWR_SUPPY_INFO*)UpdateBuffer)->PwrSupplyChar &= 0xFD7D;

    do{ // if cksum fail and retry no more than 5, then read the FRU again
        BMCCMDData[0] = UpdateBuffer[4];     // get PSU#
        gBS->SetMem(FRUBlockData, 0, sizeof(FRUBlockData));
        pFRUBlockData = FRUBlockData;
        //Get three blocks FRU information for each power supply
        for(i = 1; i <= 4; i++){
            BMCCMDData[1] = i;
            Status = SMC_IPMICmd30_A1(BMCCMDData, pFRUBlockData); 
            if(Status)      return Status;  // return device not ready if PSU is not plugged

            pFRUBlockData += 32;    // next block
        } // for i

        PLength = FRUBlockData[25];
        if(PLength < 6 || PLength > 10)
            PLength = 88;
        else
            PLength = 24 + PLength * 8;

        cksum1 = 0;
        for(i = 24; i < PLength; i++)   cksum1 += FRUBlockData[i];

        cksum2 = FRUBlockData[PLength + 3];
        PLength += 5;
        for(i = PLength; i < (PLength + 24); i++)       cksum2 += FRUBlockData[i];
        DEBUG((-1, "cksum1 %x, cksum2 %x, cksumRetry %x\n", cksum1, cksum2, cksumRetry));
    }while((cksum1 || cksum2) && (cksumRetry++ < 5));
                    
    ((SMBIOS_SYSTEM_PWR_SUPPY_INFO*)UpdateBuffer)->PwrSupplyChar |= 0x0082;
    *(UINT16*)(UpdateBuffer + 6) = 0x0203;      // adjust type 39 string order according to FRU string order
    *(UINT32*)(UpdateBuffer + 8) = 0x05040706;  // adjust type 39 string order according to FRU string order

    while(UpdateBuffer[j++]);   // bypass location string

    for(i = 0x1c; i < 0x5d; i++){       // start from 1c for vendor name
        FRUBlockChar = FRUBlockData[i];
        if(FRUBlockChar < 0xc0)
            UpdateBuffer[j++] = FRUBlockChar;
        else if((FRUBlockChar >= 0xc2) && (FRUBlockChar <= 0xdf)){
            UpdateBuffer[j++] = 0;
            if((++k) >= 7)  break;  // total 7 strings
        }
        else if(FRUBlockChar == 0xc0){
            *(UINT32*)(UpdateBuffer + j) = 0x412f4e00;  //"N/A"
            j += 4;
            if((++k) >= 7)      break;  // total 7 strings
        }
        else    break;
    }   // for i
    if(UpdateBuffer[j] != 0)
        UpdateBuffer[j++] = 0;          // end of string
    UpdateBuffer[j++] = 0;              // end of type39
    *UpdateBufferSize = (UINT16)j;      // write back buffer length
    *(UINT16*)(UpdateBuffer + 0x0c) = *(UINT16*)(FRUBlockData + 0x5d);  // max power
#endif  // IPMI_SUPPORT
    DEBUG((-1, "SmBiosType39Update End.\n"));
    return Status;
}

EFI_STATUS
SmBiosType41Update(
    IN	UINT16	UpdateHandle,
    IN OUT	UINT8	*UpdateBuffer,
    IN OUT	UINT16	*UpdateBufferSize
)
{
    UINT8       Bus, BusE, Dev, Fun, LAN_Instance;
    UINT32      TempBDF;
    UINT32      ReturnVal0 = 0, ReturnVal1 = 0, ReturnVal2 = 0, ReturnVal3 = 0, SIOM_ID = 0;    

    DEBUG((-1, "SmBiosType41Update Start.\n"));
//transfer BDF define
    TempBDF = ((UINT32)(((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->SegGroupNumber) << 16) +
        ((UINT32)(((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->BusNumber) << 8) +
        (UINT32)(((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DevFuncNumber);

    TempBDF = (UINT32)SmcBDFTransfer(TempBDF);
    ((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->SegGroupNumber = (UINT16)(TempBDF >> 16);
    ((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->BusNumber = (UINT8)(TempBDF >> 8);
    ((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DevFuncNumber = (UINT8)TempBDF;

    Bus = (UINT8)((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->BusNumber;
    Dev = (((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DevFuncNumber) >> 3;
    Fun = (((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DevFuncNumber) & 0x07;
    BusE = *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, 0x1A);
    Bus = *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, 0x19);

    for( ; BusE >= Bus; BusE--){
        if(*(UINT8*)MmPciAddress(0, BusE, 0, 0, 0) != 0xff)
            break;
    }

    ((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->BusNumber = BusE;
    ((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DevFuncNumber = 0;

    if((((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DeviceType & 0x7f) == 0x05){      //ethernet
        LAN_Instance = ((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DeviceTypeInstance;
        if(PcdGet8(PcdSmcMultiFuncLanChip1) >= 0x04){
            LAN_Instance = LAN_Instance - 1;
            ((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DevFuncNumber = LAN_Instance;
        } else if(PcdGet8(PcdSmcMultiFuncLanChip1) >= 0x02){
            if(LAN_Instance > 2)
                LAN_Instance = LAN_Instance - 3;
            else
                LAN_Instance = LAN_Instance - 1;
            ((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DevFuncNumber = LAN_Instance;
        }
    }
    
    Dev = (((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DevFuncNumber) >>3;
    Fun = (((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DevFuncNumber) & 0x07;
    if(*(UINT16*)MmPciAddress(0, BusE, Dev, Fun, 0) == 0xffff)
        ((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DeviceType &= 0x7f;       // disable if no present
    else
        ((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DeviceType |= 0x80;

    GpioGetInputValue(SIOM_ID0, &ReturnVal0);
    GpioGetInputValue(SIOM_ID1, &ReturnVal1);
    GpioGetInputValue(SIOM_ID2, &ReturnVal2);
    GpioGetInputValue(SIOM_ID3, &ReturnVal3);    
    SIOM_ID = (ReturnVal3 << 3) | (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;

/*
    AOC-ML-4S         : 0001b 0x01
    AOC-MHIBE-m2C     : 0101b 0x05    
    AOC-M25G-m4S      : 0110b 0x06
    AOC-MH25G-m2S2T   : 0111b 0x07
    AOC-MH25G-b2S2G   : 1001b 0x09
    AOC-MHIBF-2QG/1QG : 1010b 0x0A
    AOC-MHIBF-m2Q2G
    AOC-MHIBF-m1Q2G
    AOC-MTG-I4T/I2T   : 1100b 0x0C    

    if((((SMBIOS_ONBOARD_DEV_EXT_INFO*)UpdateBuffer)->DeviceType & 0x7f) == 0x05)
    {
      switch (SIOM_ID)
      {
        case 6:
          string[0] =
          string[1] =
          string[2] =
          string[3] =
          break;
        case 5:
          break;
        case 4:
          break;
        case 3:
        case 1:
        default:
          break;
      }

      for (i=0; i<4; i++)
        reg = (UINT8)Strlen(CONVERT_TO_STRING(SMC_10G_Designation));
        MemCpy(&UpdateBuffer[0x0b], CONVERT_TO_STRING(SMC_10G_Designation), reg);
	}
*/    
    SmBiosType40Create(UpdateHandle, &Bus, &Dev, &Fun, &BusE, 0x0000);

    DEBUG((-1, "SmBiosType41Update End.\n"));
    return EFI_SUCCESS;
}

SMC_SMBIOS_UPDATE_TABLE                   SmcSmBiosUpdateTable[] = {
//  {0, SmBiosType00Update},
  {1, SmBiosType01Update},
//  {02, SmBiosType02Update},
//  {03, SmBiosType03Update},
//  {04, SmBiosType04Update},
//  {07, SmBiosType07Update},
  {0x09, SmBiosType09Update},
//  {11, SmBiosType11Update},
//  {15, SmBiosType15Update},
  {16, SmBiosType16Update},
  {17, SmBiosType17Update},
//  {19, SmBiosType19Update},
//  {20, SmBiosType20Update},
//  {32, SmBiosType32Update},
  {38, SmBiosType38Update},
  {39, SmBiosType39Update},
  {41, SmBiosType41Update},
  {0xFF, NULL}
};


//****************************************************************************
