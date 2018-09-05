/****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//    Rev.  1.05
//       Bug Fixed:   Add more DID for modify Lan string.
//       Reason:      X722, X710, XL710
//       Auditor:     Kasber Chen
//       Issue Date:  Jul/17/2017
//
//    Rev.  1.04
//       Bug Fixed:   Add a workaround to modify lan string from 40G to 40-10G.
//       Reason:      Code from Jacker, PM ask
//       Auditor:     Kasber Chen
//       Issue Date:  Jun/16/2017
//
//    Rev.  1.03
//       Bug Fixed:   Comment out suppress Option ROM execution during INT19 trap
//       Reason:      APTIOV_SERVER_OVERRIDE_START from label75Beta
//       Auditor:     Stephen Chen
//       Issue Date:  May/02/2017
//
//    Rev.  1.02
//       Bug Fixed:   Add a function that limit the primary VGA's resources
//                    under 4G.(Refer to Grantley)
//       Reason:      To prevent form Display have no output when enabled
//                    Above 4G and set VGA Priority to offboard.
//       Auditor:     Isaac Hsu
//       Issue Date:  Dec/14/2016
//
//    Rev.  1.01
//       Bug Fixed:   1. Add workaround to fixed the Onboard LSIRaid device ColdBoot and WarmBoot
//                       will cause device name strings change symptom.
//                    2. Patch XL710 10G NIC PXE show "IBA 40G"
//       Reason:      Function improve.
//       Auditor:     Jacker Yeh (Refer from Grantley)
//       Issue Date:  Oct/27/2016
//
//  Rev. 1.00
//      Bug Fixed: Fixed "AddOn ROM display" setup option not work correctly problem.
//      Reason:
//      Auditor:   Jacker Yeh
//      Date:      Oct/12/2016
//
//*****************************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file CsmOpROM.c
    CSM ROM placement and execution interface routines

**/

#include "Csm.h"
#include "Token.h"
#include <AmiDxeLib.h>
#include "Pci.h"
#include <Protocol/PciIo.h>
#include "Setup.h"
#include <Protocol/AmiCsmOpromPolicy.h>
#include <Guid/SetupVariable.h> //Supermicro

#if SMCPKG_SUPPORT
//#if defined(PATCH_ONBOARD_LSIRAID_WORKAROUND)&&(PATCH_ONBOARD_LSIRAID_WORKAROUND==1)
UINT8  OnBoardLSIRaidDeviceNum = 0;
UINT8 *OnBoardLSIRaidDeviceString = NULL;
/*
UINT16
ReadPci16 (
  IN UINT8        Bus,
  IN UINT8        Dev,
  IN UINT8        Fun,
  IN UINT16       Reg
  );

BOOLEAN
SmcCheckOnBoardLSIRaidDevice (
  IN UINTN        Bus,
  IN UINTN        Dev,
  IN UINTN        Fun,
  IN UINT8        Class,
  IN UINT8        SubClass
  )
{
  BOOLEAN  IsLSIDevice = FALSE;
  UINT16   VenId =0, DevId =0;

  VenId = ReadPci16 ((UINT8)Bus, (UINT8)Dev, (UINT8)Fun, 0x00);
  DevId = ReadPci16 ((UINT8)Bus, (UINT8)Dev, (UINT8)Fun, 0x02);
  if (VenId == 0x1000 && DevId == 0x0097)
  {
     if (Class == 0x01 && SubClass == 0x07)
     {
         IsLSIDevice = TRUE;
     }
  }
  return (IsLSIDevice);
}*/
#endif

extern  UINT8 *gNextRomAddress;
extern  AMI_CSM_EXECUTED_PCI_ROM *gExecutedRomsPci;
extern  SAVED_PCI_ROM *gSavedOprom;
extern  SETUP_DATA  gSetup;

AMI_CSM_OPROM_POLICY_PROTOCOL *gCsmPolicy = NULL;
EFI_HANDLE gVgaHandle = NULL;
BOOLEAN gBbsUpdateInProgress = FALSE;
BOOLEAN gDoNotUpdateBbsTable = FALSE;

//
// gSetTxtMode
// ff - initial value
// 0 - switching to text mode is needed
// 1 - switching is needed, restoration is not
// 2 - neither switching nor restoration is needed
//
extern UINT8  gSetTxtMode;
extern BOOLEAN gServiceRomsExecuted;

#pragma pack(push, 1)

// EFI Load Option needed for call to LegacyBios->LegacyBoot()
static struct {
    EFI_LOAD_OPTION             LoadOption;
    CHAR16                      Description[10];
    BBS_BBS_DEVICE_PATH         BbsDevicePath;
    EFI_DEVICE_PATH_PROTOCOL    DevicePathEnd;
} DummyLoadOption = {
    {   // EFI_LOAD_OPTION LoadOption
        0,                              // Attributes (UINT32)
        sizeof (BBS_BBS_DEVICE_PATH)
            + sizeof (EFI_DEVICE_PATH_PROTOCOL), // FilePathListLength (UINT16)
    },

    L"DummyLoad",                         // Description

    {   // BbsDevicePath   
        {   // Header
            BBS_DEVICE_PATH,            // Type
            BBS_BBS_DP,                 // Subtype
            sizeof(BBS_BBS_DEVICE_PATH) // Length
        },
        
        BBS_HARDDISK,       // DeviceType
        0,                  // StatusFlags
        0,                  // String
    },

    {   // DevicePathEnd
        END_DEVICE_PATH,                    // Type
        END_ENTIRE_SUBTYPE,                 // SubType
        sizeof(EFI_DEVICE_PATH_PROTOCOL)    // Size
    }
};

#pragma pack(pop)

/**
    Tests PCI ROM for Legacy PCI ROM compatibility.

         
    @retval TRUE Image is valid
    @retval FALSE Image is not valid

    @note  
  From PCI Fw Specification 3.0, 5.2.1.21. Backward Compatibility of Option ROMs Page # 86.
    It is also possible to have two separate ROM images for the same PCI device: one for PCI 2.1
    System Firmware and one for PCI 3.0 compliance. In this case, the PCI 2.1 Option ROM image
    must appear first in the sequence of images. PCI 3.0 System Firmware will first search for a
    PCI 3.0 Option ROM image and only use the PCI 2.1 Option ROM image if no PCI 3.0 Option ROM
    image is found.
**/

BOOLEAN
IsValidLegacyPciOpROM (
    IN UINT32 VidDid,       // PCI vendor ID/Device ID
    IN OUT VOID **Image,    // Pointer to the beginning of PCI Option ROM
    IN OUT UINTN *Size      // Input: PciIo->RomSize, Output: OpROM size in bytes
)
{
    PCI_DATA_STRUCTURE *pcir;
    BOOLEAN IsLastImage = FALSE;
    UINT8 *RomStart = *Image;
    UINT32 RomSize = 0;
    BOOLEAN FoundLegacyRom = FALSE;
    UINTN   RomEnd = (UINTN)*Image + *Size;

    for(; !IsLastImage && ((UINTN)RomStart < RomEnd); ) {
        //
        // Check for 55AA in the beginning of the image
        //
        if (((LEGACY_OPT_ROM_HEADER*)RomStart)->Signature != 0xaa55) {
            RomStart += 512;
            continue;
        }

        //
        // Validate "PCIR" data
        //
        pcir = (PCI_DATA_STRUCTURE *)(RomStart + *(UINT16*)(RomStart + 0x18));
        if (pcir->Signature != 0x52494350) return FALSE;    // "PCIR"

        IsLastImage = pcir->Indicator & 0x80;

        //   Code Type Description
        //   0x00 Intel IA-32, PC-AT compatible
        //   0x01 Open Firmware standard for PCI
        //   0x02 Hewlett-Packard PA RISC
        //   0x03 EFI Image
        //   0x04-0xFF Reserved
        //
        if (pcir->CodeType == 0) {  // IA-32, PC-AT compatible

            if (pcir->Revision != 3 && FoundLegacyRom)
            {
                // More than one legacy OpROM is present with revision less
                // than 3.0; return the pointer and the size of the previous one.
                // Image and Size are updated when FoundLegacyRom became TRUE.
                // This implements backward compatibility mentioned in the notes
                // above.
                //
                return TRUE;
            }
            
            // Validate the ROM size
            RomSize = pcir->ImageLength << 9;
            if (RomSize <= 0x1fe00)
            {
                UINT32 HeaderRomSize = ((LEGACY_OPT_ROM_HEADER*)RomStart)->Size512 << 9;
                if (HeaderRomSize > RomSize) RomSize = HeaderRomSize;
            }
            else
            {
                TRACE((-1, "CSM: Found unusually large legacy Option ROM (%d Bytes) - loading ", RomSize));
                if (CSM_ALLOW_LARGE_OPROMS == 0)
                {
                    TRACE((-1, "skipped.\n"));
                    RomSize = 0;
                } else TRACE((-1, "allowed.\n"));
            }

            if (RomSize == 0) return FALSE;
    
            *Image = RomStart;
            *Size = (UINTN)RomSize;

            if (pcir->Revision == 3) return TRUE;

            FoundLegacyRom = TRUE;
            RomStart += RomSize;
            continue;
        }

        // Non-legacy ROM; find the size from "PCIR" structure
        RomSize = pcir->ImageLength << 9;
        RomStart += RomSize;
    }

    return FoundLegacyRom;
}


/**
    This routine is called after every OpROM (BBS or non-BBS) is
    executed. It updates the locations of EBDA in SAVED_PCI_ROM.ebdaAddr
    fields after OpROM expands EBDA.

    @param AddrChange The size of EBDA created by the OpROM


    @note  When this function is called SAVED_PCI_ROM structure will not have
              the current OpROM information inserted; gSavedOprom is pointing to NULL
              located right after the last valid entry (1st entry is also NULL).

**/

VOID
UpdateEbdaMap(UINT32 AddrChange)
{
    SAVED_PCI_ROM *SavedOprom;

    //
    // for every SAVED_PCI_ROM update ebdaAddr
    //
    for (SavedOprom=gSavedOprom-1; SavedOprom->Address; SavedOprom--) {
        if (SavedOprom->isEbda && SavedOprom->rtDataAddr) {
            SavedOprom->rtDataAddr -= AddrChange;
        }
    }
}

/**
    This routine saves Oprom that was just executed to the next
    gSavedOprom data field; gSavedOprom is incremented.

    @param Rom Address and size or runtime data taken during ROM initialization

    @note  Saving Oprom data is only required for the option ROMs that
              produce BCV because of early BCV execution; it should not be
              called for non-BBS compliant OpROMs, for those OpROMs related
              memory context will be the same til the system is booted.

**/

VOID
SaveOprom (
    UINT8   *Rom,
    UINT8   *RtDataAddr,
    UINT32  RtDataSize,
    BOOLEAN IsEbda,
    UINT32  EbdaOffset
)
{
    UINTN  RomSize = ((LEGACY_OPT_ROM_HEADER*)Rom)->Size512 << 9;

    if (RomSize==0) return;
    pBS->AllocatePool(EfiBootServicesData, RomSize, &gSavedOprom->Data);
    pBS->CopyMem(gSavedOprom->Data, Rom, RomSize);
    gSavedOprom->Address = Rom;
    //
    // Save runtime data associated with this ROM
    //
    if (RtDataSize) {
        ASSERT(RtDataAddr); // if size is not zero, address must not be zero
        gSavedOprom->rtDataAddr = RtDataAddr;
        gSavedOprom->rtDataSize = RtDataSize;
        gSavedOprom->isEbda = IsEbda;
        gSavedOprom->ebdaOffset = EbdaOffset;
        pBS->AllocatePool(EfiBootServicesData, RtDataSize, &gSavedOprom->rtData);
        pBS->CopyMem(gSavedOprom->rtData, RtDataAddr, RtDataSize);
    }
    gSavedOprom++;  // Points to zero address/data now.
}


/**
    Checks if the platform allows BCV execution; if so, executes BCV and logs
    the status of HW interrupt changes.

**/

EFI_STATUS
ExecuteBcv (
    BIOS_INFO *BiosInfo,
    UINT8 *PciCfg,
    UINT16 BcvSeg,
    UINT16 BcvOfs,
    UINT8 *Disk
)
{
    EFI_STATUS  Status;
    UINTN       TotalDevices;
    UINT32      *DeviceList;
    UINTN       Counter;
    EFI_IA32_REGISTER_SET RegSet;
    UINT8       Irq;
    UINT32      *Ivt = (UINT32*)0;
    UINT32      IrqHandler = 0;
    static EFI_LEGACY_BIOS_EXT_PROTOCOL *LegacyBiosExt = NULL;

    if (LegacyBiosExt == NULL) {
        Status = pBS->LocateProtocol(&gEfiLegacyBiosExtProtocolGuid, NULL, &LegacyBiosExt);
        ASSERT_EFI_ERROR(Status);
    }

    Status = LegacyBiosExt->GetPlatformInfoEx(LegacyBiosExt,
                    EfiGetBcvSkipDeviceList,
                    &DeviceList,
                    &TotalDevices,
                    NULL, NULL, 0, 0);

    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    if (TotalDevices > 0 && *DeviceList == 0xffffffff) return EFI_UNSUPPORTED;  // Force to skip BCV execution

    for (Counter = 0; Counter < TotalDevices; Counter++)
    {
        if (*(UINT32*)PciCfg == DeviceList[Counter]) return EFI_UNSUPPORTED;
    }

    // Get the hardware interrupt vector and its handler pointer
    Irq =  *(PciCfg+0x3C);
    if (Irq > 0 && Irq < 0xF)
    {
        Status = BiosInfo->i8259->GetVector (BiosInfo->i8259, Irq, &Irq);   // irq has INT number
        ASSERT_EFI_ERROR(Status);
        IrqHandler = Ivt[Irq];
    }

    //
    // Execute BCV
    //
    pBS->SetMem (&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.X.ES = BiosInfo->Csm16Header->PnPInstallationCheckSegment;
    RegSet.X.DI = BiosInfo->Csm16Header->PnPInstallationCheckOffset;

    FarCall86 (&BiosInfo->iBios,
                BcvSeg,
                BcvOfs,
                &RegSet,
                NULL,
                0);
    if (IrqHandler && (Ivt[Irq] != IrqHandler)) {
        *Disk |= 0x40;  // Indicate BCV has hooked HW IRQ
    }

    return EFI_SUCCESS;
}


/**
    Check PCI ROM for PnP structures and inserts BCV/BEV devices
    into BBS table.

**/

VOID
FetchBbsBootDevices(
    BIOS_INFO *BiosInfo,
    UINT8 *Rom,
    UINT8 *PciCfgOfs8,
    UINTN Bus, UINTN Dev, UINTN Fun,
    UINT8 *DiskFrom,
    BOOLEAN NewInt18,
    BOOLEAN NewInt19)
{
    UINT16 BbsDevType, bbsDevType;
    PCI_PNP_EXPANSION_HEADER *PnpHdr;
    UINT16 PnpSeg = (UINT16)((UINTN)Rom >> 4);
    UINT16 PnpOfs;
    BBS_TABLE *BbsTable = BiosInfo->BbsTable;
    UINT8 BbsCount;
    UINT32 *ivt = (UINT32*)0;
    UINT8  i, Checksum;
#if SMCPKG_SUPPORT
    UINT8   TempStr[12] = "IBA 40-10G ", x;
    UINT32  DID_Tbl[] = {0x37D08086, 0x37D18086, 0x37D28086, 0x37D38086,
                         0x15728086, 0x15808086, 0x15838086, 0x15898086};
#endif	
    
    if (gDoNotUpdateBbsTable) return;

    gBbsUpdateInProgress = TRUE;

    BbsCount = BiosInfo->BbsEntriesNo;

    //
    // Get BBS device type
    //
TRACE((-1, "FetchBbsBootDevices: B%x/D%x/F%x, ClassCode %x\n", Bus, Dev, Fun, *(PciCfgOfs8+0xB)));

    if (Bus|Dev|Fun) {
        switch (*(PciCfgOfs8+0xB)) {    // class code
            case PCI_CL_SYSTEM_PERIPHERALS:
                                    if(*(PciCfgOfs8+0xA) == PCI_CL_SYSTEM_PERIPHERALS_SCL_SD) {
                                        BbsDevType = BBS_HARDDISK;
                                    } else {
                                        BbsDevType = BBS_UNKNOWN;
                                    }
                                    break;
            case PCI_CL_MASS_STOR:
            case PCI_CL_I2O:        BbsDevType = BBS_HARDDISK; break;
            case PCI_CL_NETWORK:    BbsDevType = BBS_EMBED_NETWORK; break;
            case PCI_CL_BRIDGE:     BbsDevType = BBS_EMBED_NETWORK;     //for nVIDIA MCP55 LAN device is bridge mode
                break;
            case PCI_CL_SER_BUS:    BbsDevType = BBS_BEV_DEVICE; break;

            default:    BbsDevType = BBS_UNKNOWN;
        }
    } else {
        BbsDevType = BBS_EMBED_NETWORK; // Service ROMs
    }
    //
    // Get PnP information from ROM header and fill BBS structures
    //
    PnpOfs = *((UINT16*)(Rom + 0x1A));// Offset of the 1st PnP header
    for (;;PnpOfs = (UINT16)PnpHdr->NextHeaderOffset) {
        if (gDoNotUpdateBbsTable) break;
        
        PnpHdr = (PCI_PNP_EXPANSION_HEADER*) (Rom + PnpOfs);
        if (*((UINT32*)PnpHdr) != 0x506E5024) break;    // "$PnP"

        //
        // Calculate the CheckSum and check if table is valid
        //
        Checksum = 0;
        for (i = 0; i < sizeof(PCI_PNP_EXPANSION_HEADER); i++){
            Checksum += *(((UINT8*)PnpHdr) + i);
        }
        if (Checksum) continue;

        if (PnpHdr->BCV == 0 && PnpHdr->BEV == 0 &&
            !(NewInt18 || NewInt19)) continue;

        //
        // Change BbsType from BBS_HARDDISK to BBS_CDROM if BCV==0 and BEV!=0
        //
        bbsDevType = BbsDevType;
        if (BbsDevType == BBS_HARDDISK && (!PnpHdr->BCV) && PnpHdr->BEV) {
            bbsDevType = BBS_CDROM;
        }
        if (PnpHdr->BCV != 0) {
            bbsDevType = BBS_HARDDISK;
        }
#if SMCPKG_SUPPORT
//#if defined(PATCH_ONBOARD_LSIRAID_WORKAROUND)&&(PATCH_ONBOARD_LSIRAID_WORKAROUND==1)
        OnBoardLSIRaidDeviceString = (UINT8*)(((UINTN)PnpSeg<<4) + PnpHdr->ProductNamePtr);
//        if ( SmcCheckOnBoardLSIRaidDevice (Bus, Dev, Fun, *(PciCfgOfs8+0xB), *(PciCfgOfs8+0xA)) )
//        {
//           TRACE ((-1, "## Onboard LSI Raid Device:[%s] ##\n", OnBoardLSIRaidDeviceString));
//           *(OnBoardLSIRaidDeviceString + 8) = ('0' + (OnBoardLSIRaidDeviceNum / 10));
//           *(OnBoardLSIRaidDeviceString + 9) = ('0' + (OnBoardLSIRaidDeviceNum % 10));
//           OnBoardLSIRaidDeviceNum++;
//        }
    for(x = 0; x < (sizeof(DID_Tbl)/sizeof(UINT32)); x++){
        if((*(UINT32*)PciCfgOfs8 == DID_Tbl[x]) && (OnBoardLSIRaidDeviceString[4]=='4')){
            pBS->CopyMem(&OnBoardLSIRaidDeviceString[11],&OnBoardLSIRaidDeviceString[8],20);
            pBS->CopyMem(OnBoardLSIRaidDeviceString,TempStr,11);
        }
    }
//#endif
#endif
        BbsTable[BbsCount].DeviceType = bbsDevType;
        BbsTable[BbsCount].Bus = (UINT32)Bus;
        BbsTable[BbsCount].Device = (UINT32)Dev;
        BbsTable[BbsCount].Function = (UINT32)Fun;
        BbsTable[BbsCount].Class = *(PciCfgOfs8+0xB);
        BbsTable[BbsCount].SubClass = *(PciCfgOfs8+0xA);
        BbsTable[BbsCount].DescStringSegment = PnpSeg;
        BbsTable[BbsCount].DescStringOffset = PnpHdr->ProductNamePtr;
        BbsTable[BbsCount].MfgStringSegment = PnpSeg;
        BbsTable[BbsCount].MfgStringOffset = PnpHdr->MfgPtr;
        BbsTable[BbsCount].BootPriority = BBS_UNPRIORITIZED_ENTRY;
        BbsTable[BbsCount].BootHandlerSegment = PnpSeg;
        if (NewInt18) {
            BbsTable[BbsCount].AdditionalIrq18Handler = ivt[0x18];
            BbsTable[BbsCount].BootHandlerOffset = ivt[0x18];
        }
        if (NewInt19) {
            BbsTable[BbsCount].AdditionalIrq19Handler = ivt[0x19];
            BbsTable[BbsCount].BootHandlerOffset = ivt[0x19];
        }

        if (PnpHdr->BCV) {
            BbsTable[BbsCount].BootHandlerOffset = PnpHdr->BCV;
            ExecuteBcv(BiosInfo, PciCfgOfs8, PnpSeg, PnpHdr->BCV, DiskFrom);
        }
        if (PnpHdr->BEV && !(NewInt18 || NewInt19)) {
            BbsTable[BbsCount].BootHandlerOffset = PnpHdr->BEV;
        }

        if (gSetup.I19Trap == 0 && NewInt19) {
            TRACE((-1, "CSM: trapped int19 execution postponed.\n"));
            
            // clear up the BBS table, leave only the one entry that traps INT19
            // block any further BBS table updates
            for (i = 0; i < BbsCount; i++) {
                BbsTable[i].BootPriority = BBS_IGNORE_ENTRY;
            }
            gDoNotUpdateBbsTable = TRUE;
        }
        BbsCount++;
    }

    //
    // Update number of BBS entries
    //
    BiosInfo->BbsEntriesNo = BbsCount;
    gBbsUpdateInProgress = FALSE;
}


/**
    Verifies whether the passed PCI ROM image is PCI 3.0 compatible.
    If so, returns the projected (runtime) size of this ROM.

    @retval TRUE image is PCI 3.0 compliant ROM, size is updated
    @retval FALSE image is not PCI 3.0 compliant ROM, size remains untouched.

**/

BOOLEAN
Check30ROM(
    IN VOID         *RomLocation,
    IN OUT UINTN    *Size,
    IN  EFI_HANDLE  PciHandle
)
{
    PCI_DATA_STRUCTURE *pcir;
    EFI_STATUS Status;

    static UINT32 pci30pretender[] = {
                0x00041103, // Adaptec 1200 (did 0004 vid 1103)
                0x444d8086, // Intel ROBSON Technology card (444D)
                0x444e8086, // Intel ROBSON Technology card (444E)
                0x26818086  // Intel AHCI Option ROM
    };

    //
    // Check for 55AA in the beginning of the image
    //
    if (((LEGACY_OPT_ROM_HEADER*)RomLocation)->Signature != 0xAA55) return FALSE;
    //
    // Validate "PCIR" data
    //
    pcir = (PCI_DATA_STRUCTURE *)((UINT8*)RomLocation + *(UINT16*)((UINT8*)RomLocation + 0x18));
    if (pcir->Signature != 0x52494350) return FALSE;    // "PCIR"

    if (pcir->Revision == 3) {
        UINT8 i;    // check for the OpROMs that are faking PCI3.0 compatibility
        UINT32 VidDid;
        EFI_PCI_IO_PROTOCOL *PciIo;

        if (PciHandle != NULL) {

            Status = pBS->HandleProtocol(PciHandle, &gEfiPciIoProtocolGuid, &PciIo);
            ASSERT(PciIo);
    
            if (EFI_ERROR(Status)) return FALSE;
    
            Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32,
                0,  // offset
                1,  // width
                &VidDid);
            ASSERT_EFI_ERROR(Status);
    
            for (i=0; i<(sizeof(pci30pretender)/sizeof(pci30pretender[0])); i++) {
                if (pci30pretender[i] == VidDid) return FALSE;
            }
        }

        *Size = pcir->Reserved1 << 9;   // Follow PCI.H definitions
        return TRUE;
    }
    return FALSE;
}


/**
    Tests to see if a traditional PCI ROM exists for this device

    @param         This        Indicates the EFI_LEGACY_BIOS_PROTOCOL instance.
    @param         PciHandle   The handle for this device. Type EFI_HANDLE is defined in
    @param         InstallProtocolInterface() in the EFI 1.10 Specification.
    @param         RomImage    Pointer to the ROM image.
    @param         RomSize     The size of the ROM image.
    @param         Flags       The type of ROM discovered. Multiple bits can be set, as follows:
        00 = No ROM
        01 = ROM Found
        02 = ROM is a valid legacy ROM

    @retval EFI_SUCCESS A traditional OpROM is available for this device.
    @retval EFI_UNSUPPORTED A traditional OpROM is not supported.
**/

EFI_STATUS
EFIAPI
CheckPciRom (
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  IN  EFI_HANDLE                      PciHandle,
  OUT VOID                            **RomImage, OPTIONAL
  OUT UINTN                           *RomSize,   OPTIONAL
  OUT UINTN                           *Flags
)
{
    EFI_PCI_IO_PROTOCOL *pPciIo;
    UINT32 VidDid;
    VOID *PciRom;
    UINTN PciRomSize = 0;
    EFI_STATUS Status;
    BOOLEAN ValidRom = FALSE;
    UINTN RomStatus = 0;
    PCI_STD_DEVICE              Pci;
    AMI_CSM_EXECUTED_PCI_ROM *ExecutedRom = gExecutedRomsPci-1;
    AMI_CSM_EXECUTED_PCI_ROM *er;
    UINTN PciSegment, PciBus, PciDeviceNumber, PciFunction;

    Status = pBS->HandleProtocol(PciHandle, &gEfiPciIoProtocolGuid, &pPciIo);
    if (EFI_ERROR(Status) || (&pPciIo == NULL)) return EFI_UNSUPPORTED;

    PciRom = pPciIo->RomImage;

    Status = pPciIo->Pci.Read(pPciIo,
        EfiPciIoWidthUint32,
        0,  // offset
        1,  // width
        &VidDid);
    ASSERT_EFI_ERROR(Status);

    if (PciRom) {
        // the following structure is defined in AmiModulePkg_33 as AMI_PCI_OPROM_VALIDATION_DATA
        // the GUID used for signaling is defined in AmiModulePkg_32 as gAmiPciOpromDataProtocolGuid
        // as AmiModulePkg is adopted by the majority of the projects, these definitions can be used
        // instead of the local ones (below).
        struct _PCI_OPROM_VALIDATION_DATA {
            BOOLEAN     OemPciRomOverride;
            EFI_HANDLE  Handle;
            VOID        *PciRom;
            UINTN       PciRomSize;
            BOOLEAN     RomIsValid;
        } CsmPciOpromData;
        
        static EFI_GUID gPciOpromDataProtocolGuid = {
             0x30965142, 0xfc5a, 0x4e6e, { 0x94, 0xdb, 0xbb, 0xa4, 0x41, 0xb3, 0x68, 0x51 }
        };

        PciRomSize = (UINTN)pPciIo->RomSize;
        ValidRom = IsValidLegacyPciOpROM(VidDid, &PciRom, &PciRomSize);
        
        // See if OEM needs to override ROM data
        CsmPciOpromData.OemPciRomOverride = FALSE;
        CsmPciOpromData.Handle = PciHandle;
        CsmPciOpromData.PciRom = PciRom;
        CsmPciOpromData.PciRomSize = PciRomSize;
        CsmPciOpromData.RomIsValid = ValidRom;
        SignalProtocolEvent(&gPciOpromDataProtocolGuid, &CsmPciOpromData);
        
        if (CsmPciOpromData.OemPciRomOverride)
        {
            PciRom = CsmPciOpromData.PciRom;
            PciRomSize = CsmPciOpromData.PciRomSize;
            ValidRom = CsmPciOpromData.RomIsValid;
        }
        
        RomStatus = ValidRom? 2 : 1;
    }
    if (!ValidRom) {
		Status = GetPlatformPciEmbeddedRom(pPciIo, &PciRom, &PciRomSize);
        
		if (EFI_ERROR(Status)) {
        	Status = FindEmbeddedRom(OPROM_MODULEID, (UINT16)VidDid,
            	        *((UINT16*)&VidDid+1), &PciRom, &PciRomSize);
        }

        //
        // Check whether the identified ROM is a legacy OptionROM,
        // and correspondingly modify the value of "RomStatus".
        // Note: PciRomSize is initialized here
        if (!EFI_ERROR(Status)) {
    		ValidRom = IsValidLegacyPciOpROM(VidDid, &PciRom, &PciRomSize);
    		RomStatus = ValidRom? 2 : 1;
    	}
    }

    if(!ValidRom) {

        Status = pPciIo->GetLocation(pPciIo,
                    &PciSegment, &PciBus, &PciDeviceNumber, &PciFunction);

        //
        // Sd option rom can handle all the function on one time
        // So if any of one of the function on device already launched option
        // no need to launch the option rom for other functions.
        //
        for (er = ExecutedRom; er->Seg | er->Bus | er->Dev | er->Fun; er--) {
            if (er->Seg == PciSegment && er->Bus == PciBus &&
                er->Dev == PciDeviceNumber) {
                break;
            }
        }
        
        //
        //SD option can handle all the function. So don't launch option for other functions.
        //
        if(!(er->Seg | er->Bus | er->Dev | er->Fun)) {

            Status = pPciIo->Pci.Read (pPciIo,
                                EfiPciIoWidthUint32,
                                0,
                                sizeof (Pci) / sizeof (UINT32),
                                &Pci);
            //
            //Check For SD controller. If it's SD controller find Sd option rom and launch it.
            //
		    if ( Pci.Header.ClassCode[1] == PCI_CL_SYSTEM_PERIPHERALS_SCL_SD && \
			    Pci.Header.ClassCode[2] == PCI_CL_SYSTEM_PERIPHERALS ) {
                Status=FindEmbeddedRom( CSM16_MODULEID, CSM16_VENDORID, CSM16_SD_BOOT_DID, &PciRom, &PciRomSize);
                if (!EFI_ERROR(Status)) {
    		        ValidRom = IsValidLegacyPciOpROM(VidDid, &PciRom, &PciRomSize);
    		        RomStatus = ValidRom? 2 : 1;
    	        }
            }
        }
    }

    *Flags = RomStatus;

    if (RomStatus == 2) {
        if (RomImage != NULL) *RomImage = PciRom;
        if (RomSize != NULL) *RomSize = PciRomSize;
        return EFI_SUCCESS;
    }

    return EFI_UNSUPPORTED;
}


/**
    Returns the first VGA controller handle from PciIo device list.

    @retval PCI VGA controller PciIo handle

**/

EFI_STATUS FindAnyVga(
    OUT EFI_HANDLE* hVga
)
{
    EFI_HANDLE  *HandleBuffer;
    UINTN       n, HandleCount;
    EFI_STATUS  Status;
    UINT8       dData[4];
    EFI_PCI_IO_PROTOCOL *PciIo;
    EFI_STATUS  VgaStatus = EFI_NOT_FOUND;

    //
    // Locate all PciIo handles
    //
    Status = pBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiPciIoProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer);

    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR (Status)) {
        return Status;
    }
    for (n = 0 ; n < HandleCount; n++) {

        Status = pBS->HandleProtocol (
            HandleBuffer[n],
            &gEfiPciIoProtocolGuid,
            &PciIo);           // Get PciIo protocol
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status)) break;

        Status = PciIo->Pci.Read(
            PciIo,
            EfiPciIoWidthUint32,
            8,  // offset
            1,  // width
            dData);
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR(Status)) break;

        if (dData[3]==PCI_CL_DISPLAY) {
            *hVga = HandleBuffer[n];
            VgaStatus = EFI_SUCCESS;
            break;
        }
    }
    pBS->FreePool(HandleBuffer);
    return VgaStatus;
}


/**
    Reports whether OpROM for a given PciIo already executed.

**/

AMI_CSM_EXECUTED_PCI_ROM*
PciRomAlreadyExecuted(
    IN EFI_PCI_IO_PROTOCOL *PciIo
)
{
    UINTN               Seg, Bus, Dev, Fun;
    AMI_CSM_EXECUTED_PCI_ROM    *ExecutedRom;
    EFI_STATUS          Status;


    Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fun);
    ASSERT_EFI_ERROR(Status);

    for (ExecutedRom = gExecutedRomsPci-1;
         ExecutedRom->Seg | ExecutedRom->Bus | ExecutedRom->Dev | ExecutedRom->Fun;
         ExecutedRom--)
    {
        if (ExecutedRom->Seg == Seg && ExecutedRom->Bus == Bus &&
            ExecutedRom->Dev == Dev && ExecutedRom->Fun == Fun) {
            return ExecutedRom;
        }
    }
    return NULL;
}


/**
    This function manages the ability of the Option ROM to control several PCI
    devices in the system.
    For example, SCSI Option ROM designed for the multi-channel SCSI adaptor
    usually controls all the channels even though they are different PCI 
    functions of the same device. Likewise, NIC Option ROM may be able to control
    several NICs located on different PCI devices or even on different PCI buses.

    @param         PciIo       - PCI I/O protocol of the PCI device whose Option ROM is about
        to be executed
    @param         PciCfgData  - byte array of device's PCI configuration space (registers 0..3f)

         
    @retval EFI_SUCCESS Option ROM is okay execute
    @retval EFI_ALREADY_STARTED Option ROM must be skipped

    @note  
  Function code flow:
  1) Execute OEM porting hook to see if OEM overrides the default device enable
     policy. If so, return EFI_ALREADY_STARTED.
  2) Enable all function of the given PciIo.
  3) Enable all PCI devices with the same VID/DID as in given PciIo.
  4) Return EFI_SUCCESS indicating "greenlight" for Option ROM execution.

**/

EFI_STATUS
CheckEnablePciSiblings(
    EFI_PCI_IO_PROTOCOL *PciIo,
    UINT8               *PciCfgData
)
{
    EFI_STATUS          Status;
    AMI_CSM_EXECUTED_PCI_ROM    *ExecutedRom = gExecutedRomsPci-1;
    EFI_HANDLE          *HandleBuffer;
    UINTN               Count;
    UINTN               HandleCount;
    UINTN               PciSegment, PciBus, PciDeviceNumber, PciFunction;
    UINTN               Seg, Bus, Dev, Func;
    EFI_PCI_IO_PROTOCOL *CurrentPciIo;
    UINT64              Capabilities;
    UINT32              VidDid;
    UINT8               PciCfgData1[40];

    Status = pBS->LocateHandleBuffer (ByProtocol, &gEfiPciIoProtocolGuid,
        NULL, &HandleCount, &HandleBuffer);
    ASSERT_EFI_ERROR(Status);

    Status = CheckOemPciSiblings(PciIo, ExecutedRom);
    if (Status == EFI_SUCCESS) return EFI_ALREADY_STARTED;

    Status = EnableOemPciSiblings(PciIo);
    if (Status == EFI_SUCCESS) return EFI_SUCCESS;


    Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
    ASSERT_EFI_ERROR(Status);

    for (Count = 0; Count < HandleCount; Count++) {
        Status = pBS->HandleProtocol (HandleBuffer[Count], &gEfiPciIoProtocolGuid, &CurrentPciIo);
        ASSERT_EFI_ERROR(Status);

        Status = CurrentPciIo->GetLocation(CurrentPciIo,
                    &PciSegment, &PciBus, &PciDeviceNumber, &PciFunction);

        // Check if it is the same device
        if (PciBus == Bus && PciDeviceNumber == Dev && PciFunction == Func) continue;

        Status = CurrentPciIo->Pci.Read(CurrentPciIo, EfiPciIoWidthUint32, 0, 1, &VidDid);
        ASSERT_EFI_ERROR(Status);
		
        // Do not do anything else for VGA; if multiple VGA is enabled at the same time,
        // OS might have a problem. EIP60317.
        Status = CurrentPciIo->Pci.Read(CurrentPciIo, EfiPciIoWidthUint8, 0, 40, PciCfgData1);
        ASSERT_EFI_ERROR(Status);
        if (PciCfgData1[0xb] == PCI_CL_DISPLAY) continue;

        // Check if it is a different function of the same device or if VID/DID is the same
        if ((PciBus == Bus && PciDeviceNumber == Dev)
            || (*(UINT32*)PciCfgData == VidDid)) {

            Status = CurrentPciIo->Attributes (CurrentPciIo,
                    EfiPciIoAttributeOperationSupported, 0,
                    &Capabilities);     // Get device capabilities
            ASSERT_EFI_ERROR(Status);
    
            Status = CurrentPciIo->Attributes (CurrentPciIo,
                    EfiPciIoAttributeOperationEnable,
                    Capabilities & EFI_PCI_DEVICE_ENABLE,
                    NULL);              // Enable device
            ASSERT_EFI_ERROR(Status);
            TRACE((TRACE_ALWAYS,
                "CSM OPROM: device B%x/d%x/F%x was enabled for B%x/d%x/F%x OPROM execution.\n",
                PciBus, PciDeviceNumber, PciFunction, Bus, Dev, Func));
        }
    }

    return EFI_SUCCESS;
}


/**
    This is CLP execution protocol function that is called by any driver that
    needs to perform device configuration using Command Line Protocol.

    @param         PciIo               Command Line Protocol instance pointer
    @param         CmdInputLine        Pointer to a null-terminated input string
    @param         CmdResponseBuffer   Pointer to command output buffer
    @param         CmdStatus           CLP command execution status

    @retval        EFI_SUCCESS         Execution succeeded, result is in CmdStatus
    @retval        Any other value     Error status of the execution

**/

EFI_STATUS
ClpExecute (
    IN EFI_CLP_PROTOCOL *This,
    IN OUT UINT8        *CmdInputLine,
    IN OUT UINT8        *CmdResponseBuffer,
    OUT UINT32          *CmdStatus
)
{
    EFI_STATUS  Status;
    BOOLEAN FarCallStatus;
    EFI_IA32_REGISTER_SET   RegSet;
    static EFI_LEGACY_BIOS_PROTOCOL *LegacyBios = NULL;
    EFI_PCI_IO_PROTOCOL *PciIo;
    UINTN Seg, Bus, Dev, Fun;

    // Get LegacyBios protocol for FarCall86 execution
    if (LegacyBios == NULL) {
        Status = pBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR(Status)) {
            return Status;
        }
    }

    // Get PciIo protocol for the PCI bus/dev/func information
    Status = pBS->HandleProtocol(This->Handle, &gEfiPciIoProtocolGuid, &PciIo);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fun);

    // Prepare the registers for CLP execution
    pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.H.AH = (UINT8)Bus;
    RegSet.H.AL = ((UINT8)Dev << 3) | (UINT8)Fun;
    RegSet.E.EDI = (UINT32)(UINTN)CmdInputLine;
    RegSet.E.ESI = (UINT32)(UINTN)CmdResponseBuffer;

    // Execute CLP command
    FarCallStatus = FarCall86 (LegacyBios,
                This->EntrySeg,
                This->EntryOfs,
                &RegSet,
                NULL,
                0);

    if (FarCallStatus == FALSE) {
        Status = EFI_SUCCESS;
        *CmdStatus = RegSet.E.EAX;
    } else {
        Status = EFI_DEVICE_ERROR;
    }

    return Status;
}


/**
    This function is initializing and installing the CLP protocol.

    @param        Handle      PCI device handle
    @param        Csm16DOT    Option ROM related data structure

    @retval       EFI_SUCCESS         CLP protocol successfully installed
    @retval       EFI_UNSUPPORTED     CLP protocol can not be installed on this device

**/

EFI_STATUS
InitClp (
    IN EFI_HANDLE   Handle,
    IN EFI_DISPATCH_OPROM_TABLE *Csm16DOT
)
{
    UINT8 *RomLocation;
    PCI_PCIR30_DATA_STRUCTURE *Pcir;
    EFI_CLP_PROTOCOL *ClpProtocol;
    EFI_STATUS Status;
    static EFI_GUID guidClp = EFI_CLP_PROTOCOL_GUID;

    RomLocation = (UINT8*)(UINTN)((UINT32)Csm16DOT->OpromSegment << 4);

    Pcir = (PCI_PCIR30_DATA_STRUCTURE *)(RomLocation + *(UINT16*)(RomLocation + 0x18));

    if (Pcir->Signature != 0x52494350) {
        TRACE((-1, "Init CLP: PCIR signature is missing."));
        return EFI_UNSUPPORTED;
    }

    if (Pcir->Revision < 3) {
        TRACE((-1, "Init CLP: CLP support requires PCI version 3.0 or above."));
        return EFI_UNSUPPORTED;
    }

    if (Pcir->ClpEntryPoint == 0) {
        TRACE((-1, "Init CLP: CLP entry point is not present."));
        return EFI_UNSUPPORTED;
    }

    if (Handle == NULL) {
        TRACE((-1, "Init CLP: CLP ROM must be associated with PCI device."));
        return EFI_UNSUPPORTED;
    }

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(EFI_CLP_PROTOCOL), &ClpProtocol);
    ASSERT_EFI_ERROR(Status);

    ClpProtocol->Handle = Handle;
    ClpProtocol->EntrySeg = Csm16DOT->OpromSegment;
    ClpProtocol->EntryOfs = Pcir->ClpEntryPoint;
    ClpProtocol->Execute = ClpExecute;

    return pBS->InstallProtocolInterface(
        &Handle, &guidClp, EFI_NATIVE_INTERFACE, ClpProtocol);
}


/**
    Executes a given ROM using parameters pre-defined as input in
    EFI_DISPATCH_OPROM_TABLE data structure.

    @param        CoreBiosInfo    The pointer to the BIOS_INFO variable
    @param        PciIo           PCI IO handle, NULL for non-PCI ROMs
    @param        Csm16DOT        ROM execution input parameters
    @param        NextRomAddress  The location in the shadow that will have the run-time image
        of the ROM
    @param        IsVga           Video ROM indicator

    @retval        NewRomSize          Run-time size of the ROM, in Bytes
    @retval        ProcessBootDevices  Indicator of the new bootable devices found during
        ROM execution
**/

EFI_STATUS
CsmInstallRom (
    IN BIOS_INFO    *CoreBiosInfo,
    IN EFI_HANDLE   Handle,
    IN EFI_DISPATCH_OPROM_TABLE *Csm16DOT,
    IN UINTN        NextRomAddress,
    IN BOOLEAN      IsVga,
    OUT UINT32      *NewRomSize,
    OUT BOOLEAN     *ProcessBootDevices
)
{
    UINT16      ebdaSeg1, ebdaSeg2;  // ebda pointer before and after OpROM execution
    UINT16      baseMem1, baseMem2;  // 40:13 before and after OpROM execution
    UINT32      ebdaSize1, ebdaSize2;  // ebda size before and after OpROM execution
    BOOLEAN     IsEbda;
    UINT8       ebdaSizeKB;
    EFI_IA32_REGISTER_SET   RegSet;
    UINT16      RegBX;
    UINT8       *RtData = NULL;
    UINT32      RtDataSize;
    UINT32      RtRomSize;
    EFI_STATUS  Status;
    UINT32      CurrentInt10 = 0;
    UINT32      EbdaOffset;
    EFI_PCI_IO_PROTOCOL *PciIo = NULL;
    UINT64      Capabilities;
    static BOOLEAN BeenHere = FALSE;
    UINTN       SetTxtMode;
    UINT32      bTimer;//SMC
    UINT32      SmiEn;//SMC

    if (!IsVga)
    {
#if SMCPKG_SUPPORT
        if (gSetTxtMode == 2) {
            CurrentInt10 = *(UINT32*)(UINTN)0x40;
            *(UINT32*)(UINTN)0x40 = 0xf000f065;    // legacy int10
        } else {
#endif
        LockConsole();

        // Call LegacyBiosPlatform to get the VGA switching policy override
        // by default gSetTxtMode will be set to CSM_DEFAULT_VMODE_SWITCHING
        //
        if (Handle != NULL)
        {
            Status = pBS->HandleProtocol (
                Handle,
                &gEfiPciIoProtocolGuid,
                &PciIo);
            ASSERT_EFI_ERROR (Status);
        }

        Status = GetOpromVideoSwitchingMode(PciIo, CSM_DEFAULT_VMODE_SWITCHING, &SetTxtMode);
        if (EFI_ERROR(Status))
        {
            SetTxtMode = CSM_DEFAULT_VMODE_SWITCHING;
        }
        else
        {
            ASSERT((SetTxtMode == 0) || (SetTxtMode == 1) || (SetTxtMode == 2));
        }

        // Turn off the BeenHere flag if:
        //   - this is not the 1st time execution AND
        //   - OEM function returns the same data for this PciIo as for the previous one
        //
        BeenHere = (gSetTxtMode == (UINT8)SetTxtMode);
        gSetTxtMode = (UINT8)SetTxtMode;
       
        if (!BeenHere || (gSetTxtMode == 0))
        {
            DisconnectSerialIO();
        }

        if (!BeenHere && (gSetTxtMode == 1))
        {
            // Keep gVgaHandle connected, quietly change the video mode
            //
            RegSet.X.AX = 3;
            Int86 (&CoreBiosInfo->iBios, 0x10, &RegSet);
        }
        
        if (gSetTxtMode == 0)
        {
            // Disconnect controller and enable legacy VGA MEM/IO
            //
            Status = pBS->DisconnectController(gVgaHandle, NULL, NULL);
            ASSERT_EFI_ERROR(Status);
            
            Status = pBS->HandleProtocol (
                    gVgaHandle,
                    &gEfiPciIoProtocolGuid,
                    &PciIo);
            ASSERT_EFI_ERROR(Status);

            Status = PciIo->Attributes (PciIo, EfiPciIoAttributeOperationSupported, 0,
                    &Capabilities);
            ASSERT_EFI_ERROR(Status);

            // Enable VGA legacy MEM/IO access
            //
            PciIo->Attributes (PciIo, EfiPciIoAttributeOperationEnable,
                    (Capabilities & EFI_PCI_DEVICE_ENABLE)
                     | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO,
                     NULL);
        }
#if SMCPKG_SUPPORT
        }//end of else
#else
        // Set a dummy INT10 handler if gSetTxtMode is set to 2
        //
        if (gSetTxtMode == 2)
        {
            CurrentInt10 = *(UINT32*)(UINTN)0x40;
            *(UINT32*)(UINTN)0x40 = 0xf000f065;    // legacy int10
        }
#endif
    }

    // Initialize CLP (Command Line Protocol) support for this ROM
    //
    Status = InitClp(Handle, Csm16DOT);


    // Save the current EBDA location to check if OpROM modifies it
    //
    ebdaSeg1 = *(UINT16*)(UINTN)0x40e;
    ebdaSizeKB = *(UINT8*)((UINTN)ebdaSeg1<<4);
    ebdaSize1 = (UINTN)ebdaSizeKB << 10;
    ASSERT(ebdaSizeKB);   // should be initialized, can not be 0
    baseMem1 = *(UINT16*)(UINTN)0x413;

    // Execute OpROM
    //
    //SMC - Start
        bTimer = *(UINT32*)(UINTN)(0x46C); 
        SmiEn = IoRead32 (0x1830);//(ABase + R_PCH_SMI_EN);//cc
        IoWrite32 (0x1830, (BIT31+BIT3+BIT0));//(ABase + R_PCH_SMI_EN, SmiEn);//cc
    //SMC - End  
        
    pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.X.AX = Legacy16DispatchOprom;
    RegSet.X.ES = EFI_SEGMENT (Csm16DOT);
    RegSet.X.BX = EFI_OFFSET (Csm16DOT);

    FarCall86 (&CoreBiosInfo->iBios,
                CoreBiosInfo->Csm16EntrySeg,
                CoreBiosInfo->Csm16EntryOfs,
                &RegSet,
                NULL,
                0);

    RegBX = RegSet.X.BX;

    // Get the run-time Option ROM size right away; note that the value of
    // OpROMSeg:0002 can change later, for example after BCV execution (noticed
    // on NetCell PCI SATA RAID card).
    RtRomSize = ((LEGACY_OPT_ROM_HEADER*)NextRomAddress)->Size512 * 0x200;
TRACE((-1, "InstallRom...Run-time ROM Size = %x Bytes\n", RtRomSize));

    //SMC - Start
    if ((bTimer == *(UINT32*)(UINTN)(0x46C)) && IsVga) IoWrite8(0xcf9, 0x0e);
    IoWrite32 (0x1830, SmiEn);//(ABase + R_PCH_SMI_EN, SmiEn);//cc
    //SMC - End    
    // Update EBDA map
    ebdaSeg2 = *(UINT16*)(UINTN)0x40e;
    ebdaSize2 = *(UINT8*)((UINTN)ebdaSeg2<<4) << 10;

    RtDataSize = 0;
    if (ebdaSeg1 > ebdaSeg2) {
        RtDataSize = ebdaSize2 - ebdaSize1; // #of bytes taken by this OpROM
        UpdateEbdaMap((UINT32)(ebdaSeg1 - ebdaSeg2) << 4);
    }

    // Check for a ROM size not being FF
    if (RtRomSize == 0x1fe00) RtRomSize = 0;

    // Prepare the output parameters
    *ProcessBootDevices = (RegBX == 0 && RtRomSize != 0);
    *NewRomSize = RtRomSize;

    if (IsVga) return EFI_SUCCESS;  // Done for VBIOS

    if (gSetTxtMode != 1) {
        UnlockConsole();
    }

    // Restore video mode if needed
    //  gSetTxtMode:
    //      0: reconnect VGA controller
    //      1: do nothing
    //      2: restore fake INT10 vector
    // Note that at first pass gSetTxtMode is either 0 or 2; in case of 0 it is
    // reassigned using CSM_DEFAULT_VMODE_SWITCHING and OEM override.

    if (gSetTxtMode == 2) {
        *(UINT32*)(UINTN)0x40 = CurrentInt10;
    }

    if (gSetTxtMode == 0) {
        pBS->DisconnectController(gVgaHandle, NULL, NULL);
        pBS->ConnectController(gVgaHandle, NULL, NULL, TRUE);
    }
    
    if (gSetTxtMode != 1) {
        ConnectSerialIO();
    }
    
    // Update BBS device count
    if (CoreBiosInfo->BbsEntriesNo != Csm16DOT->NumberBbsEntries) {
        // CSM16 had inserted some BBS entries for non-BBS devices
        CoreBiosInfo->BbsEntriesNo = Csm16DOT->NumberBbsEntries;
    }

    // Process boot devices
    if (RegBX == 0 && RtRomSize != 0) {   // Either BBS OpROM or no bootable devices connected

        // Save the BBS compliant OpROM memory context here. Note that saving Oprom
        // data is only required for the option ROMs that produce BCV because of early
        // BCV execution; it should not be called for non-BBS compliant OpROMs, for
        // those OpROMs memory context will be the same til the system is booted.

        if (RtDataSize) {
            // EBDA was allocated, calculate the address
            IsEbda = TRUE;
            RtData = (UINT8*)(((UINTN)ebdaSeg2<<4) + ((UINTN)ebdaSizeKB<<10));
        } else {
            // EBDA was not allocated; verify data is not requested
            // by a blind update of 40:13. This memory allocation method
            // was observed on OpROM by Adaptec 39160, FW ver V2.55.0.
            IsEbda = FALSE;
            baseMem2 = *(UINT16*)(UINTN)0x413;
            RtDataSize = (UINT32)(baseMem1-baseMem2)<<10;
            if (RtDataSize) {
                RtData = (UINT8*)((UINTN)baseMem2<<10);
            }
        }

        EbdaOffset = IsEbda? (UINT32)ebdaSizeKB<<10 : 0;
        SaveOprom ((UINT8*)NextRomAddress, RtData, RtDataSize, IsEbda, EbdaOffset);
    }

    return EFI_SUCCESS;
}


/**
    Shadows an OpROM

    @param        This        Indicates the EFI_LEGACY_BIOS_PROTOCOL instance.
    @param        PciHandle   The PCI PC-AT* OpROM from this device's ROM BAR will be loaded
    @param        RomImage    A PCI PC-AT ROM image.  This argument is non-NULL if there is
        no hardware associated with the ROM and thus no PciHandle;
        otherwise it must be NULL. An example is the PXE base code.
    @param        Flags       The type of ROM discovered. Multiple bits can be set, as follows:
        00 = No ROM.
        01 = ROM found.
        02 = ROM is a valid legacy ROM.
    @param        DiskStart   Disk number of the first device hooked by the ROM. If DiskStart is
        the same as DiskEnd, no disks were hooked.
    @param        DiskEnd     Disk number of the last device hooked by the ROM.
    @param        RomShadowAddress    Shadow address of PC-AT ROM.
    @param        ShadowedRomSize     Size in bytes of RomShadowAddress.

    @retval EFI_SUCCESS     The OpROM was shadowed
    @retval EFI_UNSUPPORTED The PciHandle was not found
**/

EFI_STATUS
EFIAPI
InstallPciRom (
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  IN  EFI_HANDLE                      PciHandle,
  IN  VOID                            **RomImage,
  OUT UINTN                           *Flags,
  OUT UINT8                           *DiskStart, OPTIONAL
  OUT UINT8                           *DiskEnd, OPTIONAL
  OUT VOID                            **RomShadowAddress, OPTIONAL
  OUT UINT32                          *ShadowedRomSize OPTIONAL
  )
{
    EFI_STATUS Status, Status1 = EFI_SUCCESS;
    VOID* RomLocation;
    EFI_PHYSICAL_ADDRESS Rom30Address = 0;
    UINTN RomSize, Rom30Size = 0;
    EFI_DISPATCH_OPROM_TABLE *Csm16DOT;
    UINTN PciSegment, PciBus, PciDeviceNumber, PciFunction;
    BIOS_INFO *CoreBiosInfo = (BIOS_INFO*)This;
    UINT32 NewRomSize = 0;
    BOOLEAN IsVga = FALSE;
    UINT8 DiskFrom = 0x80;
    UINT8 DiskTo = 0x80;
    UINT8 PciCfgData[0x40];
    EFI_HANDLE *VgaHandlePtr = &gVgaHandle;
    UINTN VgaHandleCount;
    EFI_PCI_IO_PROTOCOL *PciIo = NULL;
    UINT64 Capabilities;
    BOOLEAN is30ROM = FALSE;
    UINT32 LockUnlockAddr, LockUnlockSize;
//    CSM_PLATFORM_PROTOCOL *CsmPlatformProtocol;
    UINTN LowMem4KPages = 0;
    AMI_CSM_EXECUTED_PCI_ROM *ExecutedRom = NULL;
    BOOLEAN ProcessBootDevices;
    volatile UINT32 *ivt = (UINT32*)0;
    UINT32 Int18;
    UINT32 Int19;
// APTIOV_SERVER_OVERRIDE_START : Suppress Option ROM execution during INT19 trap
    BOOLEAN Int19Trapped = FALSE;
//  static BOOLEAN Int19Trapped = FALSE;
// APTIOV_SERVER_OVERRIDE_END : Suppress Option ROM execution during INT19 trap
	UINT32 NumberAlreadyExecutedPciRoms;

// APTIOV_SERVER_OVERRIDE_START : Suppress Option ROM execution during INT19 trap
//    if (Int19Trapped) {
//        return EFI_UNSUPPORTED;
//    }
// APTIOV_SERVER_OVERRIDE_END : Suppress Option ROM execution during INT19 trap

#if SMCPKG_SUPPORT
    SETUP_DATA SetupData;
    EFI_GUID SetupGuid = SETUP_GUID;
    UINTN VariableSize = sizeof(SETUP_DATA);
    UINT32 Attributes = 0;
    UINT32 VariableAttr = 0;
    SYSTEM_CONFIGURATION SystemConfiguration;
#endif

    //
    // Handle separately HW independent OpROMs, e.g. PXE
    //
    if (PciHandle == NULL) {
        IsVga = FALSE;
        PciSegment = 0; PciBus = 0; PciDeviceNumber = 0; PciFunction = 0;
    }
    else {
        Status = pBS->HandleProtocol (
            PciHandle,
            &gEfiPciIoProtocolGuid,
            &PciIo);            // Get PciIo protocol
        ASSERT_EFI_ERROR (Status);

        //
        // NOTE: The following call will check whether the LegacyOpROM
        // has already been executed for PciIo. If so, it returns EFI_SUCCESS.
        //
        ExecutedRom = PciRomAlreadyExecuted(PciIo);
        if (ExecutedRom != NULL) {
            *Flags = ExecutedRom->Flags;
            if (DiskStart) *DiskStart = ExecutedRom->DiskFrom;
            if (DiskEnd) *DiskEnd = ExecutedRom->DiskTo;
            if (RomShadowAddress) *RomShadowAddress = ExecutedRom->RomAddress;
            if (ShadowedRomSize) *ShadowedRomSize = ExecutedRom->RomSize;

            return EFI_SUCCESS;
        }

		// Verify the number of already executed PCI ROMs does not exceed MAX_EXECUTED_OPROMS
		NumberAlreadyExecutedPciRoms = 0;
		for (
			ExecutedRom = gExecutedRomsPci-1;
			ExecutedRom->Seg | ExecutedRom->Bus | ExecutedRom->Dev | ExecutedRom->Fun;
			NumberAlreadyExecutedPciRoms++, ExecutedRom--
		){}

		if (NumberAlreadyExecutedPciRoms >= MAX_EXECUTED_OPROMS)
		{
			return EFI_OUT_OF_RESOURCES;
		}

        Status = PciIo->Pci.Read(
            PciIo,
            EfiPciIoWidthUint8,
            0,      // offset
            0x40,   // width
            PciCfgData);
        ASSERT_EFI_ERROR(Status);

        if (PciCfgData[0xB]==PCI_CL_OLD && PciCfgData[0xA]==PCI_CL_OLD_SCL_VGA) {
            IsVga = TRUE;
        }
        if (PciCfgData[0xB]==PCI_CL_DISPLAY && PciCfgData[0xA]==PCI_CL_DISPLAY_SCL_VGA) {
            IsVga = TRUE;
        }

        Status = CheckEnablePciSiblings(PciIo, PciCfgData);
        if (EFI_ERROR(Status)) return Status;

        Status = PciIo->GetLocation(PciIo,
                    &PciSegment, &PciBus, &PciDeviceNumber, &PciFunction);
        ASSERT_EFI_ERROR(Status);
    }

    // Notify others about ROM execution, see if any driver objects
	Status = PreProcessOpRom(PciHandle, RomImage);
	
    if (EFI_ERROR(Status)) {
        PostProcessOpRom();
        return Status;
    }

    if (IsVga) {
        if (CoreBiosInfo->hVga != NULL) {
            //
            // More than one legacy video is not supported
            // We return EFI_SUCCESS so that Video Thunk driver start function
            // does not fail.
            //
            PostProcessOpRom();
            return EFI_SUCCESS;
        }
        Status = CoreBiosInfo->iBiosPlatform->GetPlatformHandle(
                        CoreBiosInfo->iBiosPlatform,
                        EfiGetPlatformVgaHandle,
                        0,
                        &VgaHandlePtr,
                        &VgaHandleCount,
                        NULL);
        if (!EFI_ERROR(Status)) {       // Platform returned VGA handle
            if (PciHandle != *VgaHandlePtr) {   // Not the one requested by platform
                PostProcessOpRom();
                return EFI_UNSUPPORTED;
            }
        }
    }
    else {  // Not VGA
        if (CoreBiosInfo->hVga == NULL) {
            // NOTE: WITH THE CURRENT BDS IMPLEMENTATION CONTROL SHOULD NEVER COME HERE
            EFI_PCI_IO_PROTOCOL *VgaPciIo = NULL;
            //
            // The control is passed to this routine to install non-VGA OpROM and VGA BIOS is
            // not yet installed. This could happen in the following scenarios:
            // 1) Video is controlled by Efi native video driver
            // 2) BDS is connecting mass storage before consoles
            // 3) The system is headless (no video controller)
            //
            // We will try to find video and launch its oprom; for case #1 we will disconnect
            // the native driver and reconnect it after OpROM is executed.
            //
            Status = CoreBiosInfo->iBiosPlatform->GetPlatformHandle(
                        CoreBiosInfo->iBiosPlatform,
                        EfiGetPlatformVgaHandle,
                        0,
                        &VgaHandlePtr,
                        &VgaHandleCount,
                        NULL);
            if (EFI_ERROR(Status)) {        // Platform did not return VGA handle, try to find one
                Status1 = FindAnyVga(VgaHandlePtr);
            }
            if (!EFI_ERROR(Status) || !EFI_ERROR(Status1)) {    // Found VGA - enable it and launch OpROM
                Status = pBS->HandleProtocol (
                        *VgaHandlePtr,
                        &gEfiPciIoProtocolGuid,
                        &VgaPciIo);            // Get PciIo protocol
                ASSERT_EFI_ERROR(Status);

                Status = VgaPciIo->Attributes (VgaPciIo,
                        EfiPciIoAttributeOperationSupported, 0,
                        &Capabilities);     // Get device capabilities
                ASSERT_EFI_ERROR(Status);

                Status = VgaPciIo->Attributes (VgaPciIo,
                        EfiPciIoAttributeOperationEnable,
                        Capabilities & EFI_PCI_DEVICE_ENABLE,
                        NULL);              // Enable device
                ASSERT_EFI_ERROR(Status);
                
                InstallPciRom(This,         // Recursive call
                        *VgaHandlePtr, NULL, Flags,
                        NULL, NULL, NULL, NULL);
            }
        }
    }

    VgaHandlePtr = &gVgaHandle;

    if (IsVga) {
        ASSERT(PciIo != NULL);
        Status = PciIo->Attributes (PciIo, EfiPciIoAttributeOperationGet, 0,
                &Capabilities);     // Get device capabilities
        ASSERT_EFI_ERROR(Status);

        Status = PciIo->Attributes (PciIo, EfiPciIoAttributeOperationEnable,
                Capabilities | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO,
                NULL);              // Enable VGA legacy MEM/IO access
        ASSERT_EFI_ERROR(Status);

        CoreBiosInfo->hVga = PciHandle;
        *VgaHandlePtr = PciHandle;
    }

    //
    // Get the ROM image location
    //
    if (PciHandle != NULL) {
        Status = CheckPciRom (This, PciHandle, &RomLocation, &RomSize, Flags);
        if (EFI_ERROR(Status) || (RomLocation == NULL)) {
            PostProcessOpRom();
            return EFI_UNSUPPORTED;
        }

        is30ROM = Check30ROM(RomLocation, &Rom30Size, PciHandle);
    }
    else {  // ROM is not associated with PCI device
        RomLocation = *RomImage;
        RomSize = ((LEGACY_OPT_ROM_HEADER*)RomLocation)->Size512 * 0x200;
        if (RomSize == 0) {
            PostProcessOpRom();
            return EFI_UNSUPPORTED;
        }
        is30ROM = Check30ROM(RomLocation, &Rom30Size, NULL);
    }

    //
    // For PCI3.0 compliant ROMs reserve base memory for ROM Init code
    //
    if (is30ROM) {
        Rom30Address = 0xA0000;
        LowMem4KPages = RomSize >> 12;   // Number of 4KB units
        if (RomSize % 0x1000) {
            LowMem4KPages++;
        }
        Status = pBS->AllocatePages(AllocateMaxAddress,
                    EfiBootServicesData,
                    LowMem4KPages,
                    &Rom30Address);
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR(Status)) {
            PostProcessOpRom();
            DEVICE_ERROR_CODE(DXE_LEGACY_OPROM_NO_SPACE, EFI_ERROR_MAJOR, PciHandle);
            return EFI_OUT_OF_RESOURCES;
        }

        if (Rom30Address < 0x8000) {
            PostProcessOpRom();

            Status = EFI_OUT_OF_RESOURCES;
            TRACE(((UINTN)TRACE_ALWAYS,"Can not execute PCI 3.0 OPROM: out of Base Memory.\n"));
            DEVICE_ERROR_CODE(DXE_LEGACY_OPROM_NO_SPACE, EFI_ERROR_MAJOR, PciHandle);
            goto ReturnErrorStatus;
        }
    }

    PROGRESS_CODE(DXE_LEGACY_OPROM_INIT);

    //
    // Check for the room in shadow for this ROM and copy it from RomLocation.
    //
    {
        UINTN SizeInShadow = is30ROM? Rom30Size : RomSize;
        UINTN CopyToAddress = is30ROM? (UINTN)Rom30Address : (UINTN)gNextRomAddress;

        if(((UINTN)(gNextRomAddress) + SizeInShadow) > OPROM_MAX_ADDRESS){
            PostProcessOpRom();

            TRACE(((UINTN)TRACE_ALWAYS,"Can not execute PCI OPROM: out of resources. RomAddr %x RomSize %x\n", gNextRomAddress, SizeInShadow));
            Status = EFI_OUT_OF_RESOURCES;
            DEVICE_ERROR_CODE(DXE_LEGACY_OPROM_NO_SPACE, EFI_ERROR_MAJOR, PciHandle);
            goto ReturnErrorStatus;
        }

        Status = UnlockShadow(gNextRomAddress, SizeInShadow, &LockUnlockAddr, &LockUnlockSize);
        ASSERT_EFI_ERROR(Status);

        //
        // Initilize the size field to 0.
        //
        ((LEGACY_OPT_ROM_HEADER*)gNextRomAddress)->Size512=0;
		pBS->CopyMem((VOID*)CopyToAddress, RomLocation, RomSize);
    }

	DiskFrom = *(UINT8*)(UINTN)0x475 + 0x80;

    Csm16DOT = &CoreBiosInfo->Thunk->DispatchOpromTable;
    Csm16DOT->PnPInstallationCheckSegment = CoreBiosInfo->Csm16Header->PnPInstallationCheckSegment;
    Csm16DOT->PnPInstallationCheckOffset = CoreBiosInfo->Csm16Header->PnPInstallationCheckOffset;
    if (is30ROM) {
        Csm16DOT->OpromSegment = (UINT16)(Shr64(Rom30Address, 4));
        Csm16DOT->RuntimeSegment = (UINT16)((UINTN)gNextRomAddress >> 4);
    }
    else {
        Csm16DOT->OpromSegment = (UINT16)((UINTN)gNextRomAddress >> 4);
        Csm16DOT->RuntimeSegment = 0;
    }

    Csm16DOT->PciBus = (UINT8)PciBus;
    Csm16DOT->PciDeviceFunction = (UINT8)(PciDeviceNumber << 3 | PciFunction);
    Csm16DOT->NumberBbsEntries = CoreBiosInfo->BbsEntriesNo;
	Csm16DOT->BbsTablePointer = (UINT32)(UINTN)(CoreBiosInfo->BbsTable);

    TRACE((TRACE_ALWAYS, "OptionROM for B%x/D%x/F%x is executed from %x:0003\n",
            PciBus, PciDeviceNumber, PciFunction, Csm16DOT->OpromSegment));

#if SMCPKG_SUPPORT
    if(IsVga) {
        VariableSize = sizeof(SETUP_DATA);
        Status = pRS->GetVariable(L"Setup", &SetupGuid, &Attributes, &VariableSize, &SetupData);
        if( !EFI_ERROR(Status) && SetupData.Above4gDecode ) {
            VariableSize = sizeof(SYSTEM_CONFIGURATION);
            Status = pRS->GetVariable(L"IntelSetup", &gEfiSetupVariableGuid, &VariableAttr, &VariableSize, &SystemConfiguration);
            if( !EFI_ERROR(Status) && SystemConfiguration.VideoSelect==1 ) { // Onboard
                SetupData.SmcPriorVgaBus = 0;
                Status = pRS->SetVariable(L"Setup", &SetupGuid, Attributes, sizeof(SETUP_DATA), &SetupData);
            }
            else if( !EFI_ERROR(Status) && SystemConfiguration.VideoSelect==2 ) { // Offboard
                if( SetupData.SmcPriorVgaBus != (UINT8)(PciBus & 0xFF) ) {
                    // Record Primary VGA's Bus for limiting resources usage later
                    SetupData.SmcPriorVgaBus = (UINT8)(PciBus & 0xFF);
                    Status = pRS->SetVariable(L"Setup", &SetupGuid, Attributes, sizeof(SETUP_DATA), &SetupData);
                    IoWrite8 (0xcf9, 6);
                }
            }
        }
    }
#endif //#if SMCPKG_SUPPORT

    // Save INT18 and INT19 to be able to see its trapping after ROM execution
    Int19 = ivt[0x19];
    Int18 = ivt[0x18];

    Status = CsmInstallRom(CoreBiosInfo, PciHandle,
                Csm16DOT, (UINTN)gNextRomAddress, IsVga, &NewRomSize, &ProcessBootDevices);

    Int19Trapped = Int19 != ivt[0x19];

    if (ProcessBootDevices) {
            FetchBbsBootDevices(CoreBiosInfo,
                gNextRomAddress,
                PciCfgData,
                PciBus,
                PciDeviceNumber,
                PciFunction,
                &DiskFrom,
                Int18 != ivt[0x18],
                Int19 != ivt[0x19]);
    }

    // Restore INT18 and INT19
    ivt[0x18] = Int18;
    ivt[0x19] = Int19;

    DiskTo = (DiskFrom & 0xc0) + *(UINT8*)(UINTN)0x475; 

	PostProcessOpRom();

    if (is30ROM) {
    //
    // For PCI3.0 do necessary things with Setup Routine here, then
    // free Rom30Address memory
    //
        pBS->FreePages(Rom30Address, LowMem4KPages);
    }

    if (!is30ROM) {
        if (RomSize > NewRomSize) {
            pBS->SetMem(gNextRomAddress+NewRomSize, RomSize-NewRomSize, 0xFF);
        }
    }
    Status = LockShadow(LockUnlockAddr, LockUnlockSize);
    ASSERT_EFI_ERROR(Status);

    //
    // Make the rom size 2K aligned
    //
    if (NewRomSize % 0x800) {
        NewRomSize += (0x800 - (NewRomSize % 0x800));
    }

    //
    // Return OPTIONAL parameters: updated disks, oprom address and size.
    //
    if (DiskStart) *DiskStart = DiskFrom;
    if (DiskEnd) *DiskEnd = DiskTo;
    if (RomShadowAddress) *RomShadowAddress = gNextRomAddress;
    if (ShadowedRomSize) *ShadowedRomSize = NewRomSize;

    //
    // Update the list of Executed Roms
    //
    if (PciHandle) {
        gExecutedRomsPci->Seg = PciSegment;
        gExecutedRomsPci->Bus = PciBus;
        gExecutedRomsPci->Dev = PciDeviceNumber;
        gExecutedRomsPci->Fun = PciFunction;
        gExecutedRomsPci->Flags = *Flags;
        gExecutedRomsPci->DiskFrom = DiskFrom;
        gExecutedRomsPci->DiskTo = DiskTo;
        gExecutedRomsPci->RomAddress = gNextRomAddress;
        gExecutedRomsPci->RomSize = NewRomSize;
        gExecutedRomsPci++;
    }
TRACE((TRACE_ALWAYS, "PCI OPROM(handle %x, %x/%x/%x): addr %x, size %x\n",
            PciHandle, PciBus, PciDeviceNumber, PciFunction, gNextRomAddress, NewRomSize));
    gNextRomAddress += NewRomSize;

    if (Int19Trapped && !IsVga && (gSetup.I19Trap == 1))
    {
        if (gSetTxtMode == 1)
        {
            if (gVgaHandle != NULL) 
            {
                TRACE((-1, "Reconnecting video and serial before calling INT19 trap.\n"));
                pBS->DisconnectController(gVgaHandle, NULL, NULL);
                pBS->ConnectController(gVgaHandle, NULL, NULL, TRUE);
                ConnectSerialIO();
            }
            UnlockConsole();
        }

        // Signal READY_TO_BOOT event
        {
            EFI_EVENT ReadyToBootEvent;
            Status = CreateReadyToBootEvent(
                TPL_CALLBACK, NULL, NULL, &ReadyToBootEvent
            );
            if (!EFI_ERROR(Status)) {
                pBS->SignalEvent(ReadyToBootEvent);
                pBS->CloseEvent(ReadyToBootEvent);
            }
        }
        
        LegacyBoot (
            &CoreBiosInfo->iBios,
            &(DummyLoadOption.BbsDevicePath),
            sizeof(DummyLoadOption),
            &DummyLoadOption
        );
        ASSERT(FALSE);  // CONTROL MUST NOT BE GIVEN BACK HERE
        // If by any chance we are here, we have to do the needful:
        pRS->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
    }

    return EFI_SUCCESS;

ReturnErrorStatus:
    if (LowMem4KPages != 0) {
        pBS->FreePages(Rom30Address, LowMem4KPages);
    }
    return Status;
}

/**
    Returns the available shadow RAM address out of the given range.

    @param AddrMin      Min address
    @param AddrMax       Max address
    @param Size          Range size, in Bytes
    @param Alignment     Range alignment

    @retval EFI_SUCCESS             AddrMin variable contains the available address
    @retval EFI_OUT_OF_RESOURCES    The requested memory range is not available in
        the shadow RAM
    @note  
      This function does not reserve or allocate memory in the shadow RAM

**/

EFI_STATUS
EFIAPI
GetShadowRamAddress(
    IN OUT UINT32   *AddrMin,
    IN UINT32       AddrMax,
    IN UINT32       Size,
    IN UINT32       Alignment
)
{    
	UINT32  Addr = *AddrMin;
	UINT32  Aln;
	
	if (Addr > AddrMax) return EFI_INVALID_PARAMETER;
	if (Size == 0x00) return EFI_INVALID_PARAMETER;
	if (Alignment > OPROM_MAX_ADDRESS) return EFI_INVALID_PARAMETER;
	if (AddrMax + Size > (OPROM_MAX_ADDRESS + 1)) return EFI_OUT_OF_RESOURCES;
	if (AddrMax < (UINT32)(UINTN)gNextRomAddress) return EFI_OUT_OF_RESOURCES;
	
	if(Alignment != 0) Alignment--;
    Aln = ~Alignment;
	
	if (Addr < (UINT32)(UINTN)gNextRomAddress) {
        Addr = (UINT32)(UINTN)gNextRomAddress;
    }

	if ((Alignment !=0) && (Addr & Aln)) {
		if ( (Addr & Aln) != Addr){
			Addr += (Alignment+1);
			Addr &= Aln;
            
		}
    }
	gNextRomAddress = (UINT8 *)(Addr + Size);
	*AddrMin = Addr;

    return EFI_SUCCESS;
}



/**
    Executes ISA Option ROM

    @param        This        Indicates the EFI_LEGACY_BIOS_PROTOCOL instance.
        RomAddress  Location of the Option ROM

    @retval        EFI_SUCCESS             ROM was successfully executed
    @retval        EFI_OUT_OF_RESOURCES    Shadow RAM full, ROM was not executed
**/

EFI_STATUS
EFIAPI
InstallIsaRom(
    IN EFI_LEGACY_BIOS_EXT_PROTOCOL *This,
    IN UINTN    RomAddress
)
{
    UINTN       RomSize;
    BOOLEAN     ProcessBootDevices;
    EFI_STATUS  Status;
    EFI_DISPATCH_OPROM_TABLE *Csm16DOT;
    EFI_LEGACY_BIOS_PROTOCOL    *LegacyBios;
    BIOS_INFO   *CoreBiosInfo;
    UINT8       PciCfgData[0x40] = {0};
    UINT8       DiskFrom = 0x80;
    volatile UINT32 *ivt = (UINT32*)0;
    UINT32      Int18;
    UINT32      Int19;
    UINT32      LockUnlockAddr, LockUnlockSize;

    // Validate ISA ROM
    if (((LEGACY_OPT_ROM_HEADER*)RomAddress)->Signature != 0xAA55) {
        TRACE((-1, "Can not execute ISA ROM: missing 0xAA55 signature.\n"));
        return EFI_UNSUPPORTED;
    }

    // See if ROM fits in the shadow
    RomSize = ((LEGACY_OPT_ROM_HEADER*)RomAddress)->Size512 << 9;
	if((RomAddress + RomSize) > OPROM_MAX_ADDRESS) {
        TRACE((-1, "Can not execute ISA ROM: won't fit in the shadow memory.\n"));
        return EFI_OUT_OF_RESOURCES;
    }

    // Get BIOS_INFO variable pointer
    Status = pBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
    ASSERT_EFI_ERROR(Status);
    CoreBiosInfo = (BIOS_INFO*)LegacyBios;

    //
    // Execute platform pre-OpROM function
    //
	Status = PreProcessOpRom(NULL, (VOID*)&RomAddress);
	
    if (EFI_ERROR(Status)) {
        PostProcessOpRom();
        return Status;
    }

    PROGRESS_CODE(DXE_LEGACY_OPROM_INIT);

    //
    // Check for the room in shadow for this ROM and copy it from RomLocation.
    //
    {
        UINTN SizeInShadow = RomSize;
        UINTN CopyToAddress = RomAddress;
        UINTN TempRomAddress = 0;

        Status = pBS->AllocatePool(EfiBootServicesData, RomSize, (VOID**)&TempRomAddress);
        if (EFI_ERROR(Status)) {
            TRACE((-1, "Can not copy ISA ROM to memory: insufficient memory space.\n"));
            return EFI_OUT_OF_RESOURCES;
        }
        pBS->CopyMem(&TempRomAddress, &CopyToAddress, SizeInShadow);
        ASSERT_EFI_ERROR(Status);

        Status = UnlockShadow((UINT8*)&CopyToAddress, SizeInShadow, &LockUnlockAddr, &LockUnlockSize);
        ASSERT_EFI_ERROR(Status);

        pBS->CopyMem(&CopyToAddress, &TempRomAddress, SizeInShadow);
        TRACE((-1, "Copy ISA ROM to shadow memory address %x.\n", CopyToAddress));
        pBS->FreePages(TempRomAddress, SizeInShadow);
    }

	DiskFrom = *(UINT8*)(UINTN)0x475 + 0x80;

    Csm16DOT = &CoreBiosInfo->Thunk->DispatchOpromTable;
    Csm16DOT->OpromSegment = (UINT16)(RomAddress >> 4);
    Csm16DOT->NumberBbsEntries = CoreBiosInfo->BbsEntriesNo;
	Csm16DOT->BbsTablePointer = (UINT32)(UINTN)(CoreBiosInfo->BbsTable);

    TRACE((TRACE_ALWAYS, "OptionROM for ISA Device is executed from %x:0003\n", Csm16DOT->OpromSegment));

    // Save INT18 and INT19 to be able to see its trapping after ROM execution
    Int19 = ivt[0x19];
    Int18 = ivt[0x18];

    Status = CsmInstallRom(CoreBiosInfo, NULL,
                Csm16DOT, RomAddress, FALSE, NULL, &ProcessBootDevices);

    if (ProcessBootDevices) {
            FetchBbsBootDevices(CoreBiosInfo,
                (UINT8*)RomAddress,
                PciCfgData,
                0,
                0,
                0,
                &DiskFrom,
                Int18 != ivt[0x18],
                Int19 != ivt[0x19]);
    }

    // Restore INT18 and INT19
    ivt[0x18] = Int18;
    ivt[0x19] = Int19;

    PostProcessOpRom();

    Status = LockShadow(LockUnlockAddr, LockUnlockSize);
    ASSERT_EFI_ERROR(Status);

    return Status;
}


/**
    Allows external agents to force loading of all legacy OpROMs.
    This function can be invoked before GetBbsInfo() to ensure all
    devices are counted.
	
    @param This        Indicates the EFI_LEGACY_BIOS_PROTOCOL instance.

    @retval EFI_SUCCESS OpROMs are shadowed successfully.
**/

EFI_STATUS
EFIAPI
ShadowAllLegacyOproms (
    IN EFI_LEGACY_BIOS_PROTOCOL *This
)
{
    EFI_HANDLE  *HandleBuffer;
    UINTN       n, HandleCount;
    EFI_STATUS  Status;
    EFI_PCI_IO_PROTOCOL *PciIo;
    BIOS_INFO   *CoreBiosInfo = (BIOS_INFO*)This;
    UINT64      Capabilities;
    UINTN       Flags;
    UINT8       dData[4];

    //
    // Locate all PciIo handles
    //
    Status = pBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiPciIoProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer);

    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR (Status)) {
        return Status;
    }

    for (n = 0; n < HandleCount; n++) {
        Status = pBS->HandleProtocol (HandleBuffer[n], &gEfiPciIoProtocolGuid, &PciIo);
        ASSERT_EFI_ERROR(Status);

        if (PciRomAlreadyExecuted(PciIo)) continue;

        //
        // Skip the VGA devices: the active VGA controller might have
        // been already enabled, all other VGA controllers have to be disabled.
        //
        if (CoreBiosInfo->hVga != NULL) {
            Status = PciIo->Pci.Read(
                PciIo,
                EfiPciIoWidthUint32,
                8,  // offset
                1,  // width
                &dData);
            ASSERT_EFI_ERROR(Status);
            if (dData[3]==PCI_CL_DISPLAY) continue;
        }

        //
        // See if device has OpROM, if so - enable device and istlall OpROM; assume
        // CSM has already assinged IRQ and programmed IRQ router and register 3C
        //
        Status = CheckPciRom(This, HandleBuffer[n], NULL, NULL, &Flags);
        if (EFI_ERROR(Status) || (Flags != 2)) continue;    // No OpROM or OpROM is invalid

        Status = PciIo->Attributes (PciIo,
                EfiPciIoAttributeOperationSupported, 0,
                &Capabilities);     // Get device capabilities
        ASSERT_EFI_ERROR(Status);

        Status = PciIo->Attributes (PciIo,
                EfiPciIoAttributeOperationEnable,
                Capabilities & EFI_PCI_DEVICE_ENABLE,
                NULL);              // Enable device

        if (EFI_ERROR(Status)) continue;

        InstallPciRom(This,
                HandleBuffer[n], NULL, &Flags,
                NULL, NULL, NULL, NULL);
    }
    pBS->FreePool(HandleBuffer);	

    if (!gServiceRomsExecuted) {
        //
        // Launch service ROMs
        //
        Status = CoreBiosInfo->iBiosPlatform->PlatformHooks(
                    CoreBiosInfo->iBiosPlatform,
                    EfiPlatformHookShadowServiceRoms,
                    0, NULL, NULL, NULL, NULL);
        gServiceRomsExecuted = TRUE;
    }

    return EFI_SUCCESS;
}

/**
    This function notifies other drivers about Option ROM execution. Drivers
    may reject this ROM execution.

    @param 
        EFI_PCI_IO_PROTOCOL *PciIo - OPTIONAL - PCI device PciIo instance, NULL for
        non-PCI ROMs.

           
    @retval EFI_SUCCESS run the option rom for this device
    @retval EFI_ABORTED code determined to not run Oprom for this device

**/

EFI_STATUS PreProcessOpRom (
    IN EFI_HANDLE PciHandle,
    IN VOID **RomImage
)
{
    AMI_CSM_PLATFORM_POLICY_DATA CsmOpromPolicyData;
    EFI_STATUS Status;
    EFI_PCI_IO_PROTOCOL *PciIo;
    
    PciIo = NULL;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    Status = EFI_SUCCESS;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
    
    if (PciHandle != NULL) {
        Status = pBS->HandleProtocol(PciHandle, &gEfiPciIoProtocolGuid, &PciIo);
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR(Status)) return EFI_ABORTED;
    }

    // See if any callback function wishes to cancel this OpROM execution
    CsmOpromPolicyData.PciIo = PciIo;
	CsmOpromPolicyData.Rom = RomImage;
    CsmOpromPolicyData.ExecuteThisRom = TRUE;
#if AMI_MODULE_PKG_VERSION > 32
    CsmOpromPolicyData.PciHandle = PciHandle;
#endif

    if (gCsmPolicy == NULL) {
        Status = pBS->LocateProtocol(&gAmiCsmOpromPolicyProtocolGuid, NULL, &gCsmPolicy);
    }
    
    if (!EFI_ERROR(Status)) {
        gCsmPolicy->ProcessOpRom(&CsmOpromPolicyData);
    }

    SignalProtocolEvent(&gOpromStartEndProtocolGuid, &CsmOpromPolicyData);

    return ( CsmOpromPolicyData.ExecuteThisRom == FALSE) ? EFI_ABORTED : EFI_SUCCESS;
}


/**
    This function signals the event of the end of Option ROM execution.

    @note  
   This function is using the same GUID as PreprocessOprom function; for the
	 callback function the indication of the END of the Oprom would be NULL context.

**/

EFI_STATUS PostProcessOpRom ()
{
    if (gCsmPolicy == NULL) {
        pBS->LocateProtocol(&gAmiCsmOpromPolicyProtocolGuid, NULL, &gCsmPolicy);
    }
    
    if (gCsmPolicy != NULL) {
        gCsmPolicy->ProcessOpRom(NULL);
    }
    
    // signal the OpromStartEndProtocolGuid event
    SignalProtocolEvent(&gOpromStartEndProtocolGuid, NULL);
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
