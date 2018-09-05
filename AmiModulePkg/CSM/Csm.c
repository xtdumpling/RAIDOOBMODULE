//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Add setup item to enable/disable LEGACYT to EFI support when enable LEGACY_TO_EFI token.
//    Reason:   Function improvement.
//    Auditor:  Jimmy Chiu (Refer Grantley - Jacker Yeh)
//    Date:     Aug/24/16
//
//  Rev. 1.00
//    Bug Fix:  Add legacy re-try boot feature (Refer from Grantley)
//    Reason:   
//    Auditor:  Jacker Yeh
//    Date:     Mar/18/16
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
    Compatibility Support Module
*/

#include "Token.h"

#include <AmiDxeLib.h>
#include <Csm.h>
#include <Pci.h>
#include <Protocol/Cpu.h>
#include <Protocol/Timer.h>
#include <Protocol/MpService.h>
#include <TimeStamp.h>
#include <ImageAuthentication.h>
#include <Setup.h>
#include <Protocol/AmiBoardInfo2.h>
#include <Library/AmiSdlLib.h>
#include "Protocol/AmiCsmVideoPolicy.h"

// move later to a different file
#include <AcpiRes.h>
#include <Protocol/PciRootBridgeIo.h>

#define MAX_IDE_PCI_CONTROLLER (MAX_IDE_CONTROLLER << 1)

BIOS_INFO       *gCoreBiosInfo = NULL;
SAVED_PCI_ROM   *gSavedOprom;
BOOLEAN         gServiceRomsExecuted = FALSE;

UINT8   *gNextRomAddress;       // PCI ROM address pointer, updated as ROMs are installed
UINTN   gMaxOpRomAddress;       // Highest available address for PCI ROM

AMI_CSM_EXECUTED_PCI_ROM    *gExecutedRomsPci;   // List of PciIo's with installed OpROMs

BOOLEAN     gIsBootPrepared = FALSE;
OEMINT_FUNC *gCsm16IntStart = 0;
OEM16_FUNC  *gCsm16FuncAddress = 0;
UINTN       gCsm32DataAddress = 0;

UINT32  gHwIrqRedirSav[8];   // hw interrupts redirected vectors - IRQ0..IRQ7
UINT8   gIrq0;

#define MAX_NUM_IRQS_SAVERESTORE 16
IVT_SAVE_RESTORE gIrqSav[MAX_NUM_IRQS_SAVERESTORE];

//
// gSetTxtMode
// ff - initial value
// 0 - switching to text mode is needed (init value)
// 1 - switching is needed, restoration is not (set in ShadowOptionRoms)
// 2 - neither switching nor restoration is needed
//
UINT8  gSetTxtMode;

extern EFI_HANDLE gVgaHandle;
extern AMI_CSM_VIDEO_POLICY_PROTOCOL *gAmiCsmVideoPolicy;

VOID	*gProtocolNotifyRegistration;

EFI_GUID    gSetupGuid = SETUP_GUID;
SETUP_DATA  gSetup;
VOID ConnectSerialIO();
#ifndef DEFAULT_TICK_DURATION
#define DEFAULT_TICK_DURATION ((65535 * 838 + 50) / 100)
#endif

#define LTEB_GUID  \
    {0xC8BCA618, 0xBFC6, 0x46B7, 0x8D, 0x19, 0x83, 0x14, 0xE2, 0xE5, 0x6E, 0xC1}

EFI_GUID gLTEBGuid = LTEB_GUID;

VOID DisconnectSerialIO();
VOID DummyFunction(EFI_EVENT Event, VOID *Context);
EFI_TPL     gOldTplValue = 0;
UINT16      gProtectedIrqMask = 0xffff;
BBS_TABLE   *gBbsTableBackup = NULL;

VOID DumpBbsTable(UINT32);
#define BBS_LOWEST_ACTIVE_PRIORITY 0xfffb

/*---------------------------------------------------*/
/*---                                             ---*/
/*---    AMI CSM Extensions Protocol functions    ---*/
/*---                                             ---*/
/*---------------------------------------------------*/

EFI_LEGACY_BIOS_EXT_PROTOCOL gAmiCsmExtensionsProtocol = {
  GetEmbeddedRom,
  AddBbsEntry,
  InsertBbsEntryAt,
  RemoveBbsEntryAt,
  AllocateEbda,
  UnlockShadow,
  LockShadow,
  Get16BitFuncAddress,
  CopyLegacyTable,
  InstallIsaRom,
  GetShadowRamAddress,
  GetBbsTable,
  GetPlatformInformationEx,
  Csm16Configuration
};

EFI_STATUS AllocateHiMemPmmBlock(UINTN, UINTN*);
VOID  FreePmmBeforeBoot (EFI_EVENT, VOID*);


/**
    CSM driver entry point

**/

EFI_STATUS EFIAPI CsmEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS  Status;
    UINT16      SaveIrqMask;
    UINT16      IrqMask;
    UINTN       Size=sizeof(SETUP_DATA);
    EFI_EVENT   Event;
    EFI_HANDLE  Handle = NULL;

    InitAmiLib(ImageHandle,SystemTable);

    PROGRESS_CODE(DXE_CSM_INIT);

    // Indicate that CSM has been launched. This protocol can be used as dependecy for other driver instead of 
    // LegacyBIOS protocol which may or may not be installed.
    pBS->InstallProtocolInterface (
        &Handle, &gAmiCsmStartedProtocolGuid, EFI_NATIVE_INTERFACE, NULL
        );

	//
	// Check with the CSM Policy if CSM can be loaded
	//

	if (CsmIsTobeLoaded(ImageHandle) == FALSE) {
		return EFI_ACCESS_DENIED;
	}

    //
    // Initialize LegacyBiosPlatform
    //
    Status = InitCsmBoard (ImageHandle, SystemTable);
    if (EFI_ERROR(Status)) return Status;

    //
    // Initialize Csm Simple Key In
    //
    Status = InitCsmSimpleIn (ImageHandle, SystemTable);
    if (EFI_ERROR(Status)) return Status;

    //Locate AmiBoardInfo2 Protocol
	if(gAmiBoardInfo2Protocol == NULL) {
		Status = AmiSdlInitBoardInfo();
		TRACE((TRACE_ALWAYS,"CSM: Locating AMI_BOARD_INFO2_PROTOCOL...Status = %r\n", Status));
		ASSERT_EFI_ERROR(Status);
	    if (EFI_ERROR(Status)) return Status;
	}

    //
    // Instantiate LEGACY_BIOS
    //
    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(BIOS_INFO), &gCoreBiosInfo);
    if (EFI_ERROR(Status)) return Status;

    pBS->SetMem(gCoreBiosInfo, sizeof(BIOS_INFO), 0);
    gCoreBiosInfo->hImage = ImageHandle;

    Status = pBS->LocateProtocol (&gEfiLegacyRegion2ProtocolGuid, NULL, &gCoreBiosInfo->iRegion);
    if (EFI_ERROR(Status)) return Status;

    Status = pBS->LocateProtocol (&gEfiLegacyBiosPlatformProtocolGuid, NULL, &gCoreBiosInfo->iBiosPlatform);
    if (EFI_ERROR(Status)) return Status;

    Status = pBS->LocateProtocol (&gEfiLegacy8259ProtocolGuid, NULL, &gCoreBiosInfo->i8259);
    if (EFI_ERROR(Status)) return Status;

    Status = pBS->LocateProtocol (&gEfiLegacyInterruptProtocolGuid, NULL, &gCoreBiosInfo->iInterrupt);
    if (EFI_ERROR(Status)) return Status;

    gCoreBiosInfo->iBios.Int86            = Int86;
    gCoreBiosInfo->iBios.FarCall86        = FarCall86;
    gCoreBiosInfo->iBios.CheckPciRom      = CheckPciRom;
    gCoreBiosInfo->iBios.InstallPciRom    = InstallPciRom;
    gCoreBiosInfo->iBios.PrepareToBootEfi = PrepareToBoot;
    gCoreBiosInfo->iBios.LegacyBoot       = LegacyBoot;
    gCoreBiosInfo->iBios.GetBbsInfo       = GetBbsInfo;
    gCoreBiosInfo->iBios.ShadowAllLegacyOproms   = ShadowAllLegacyOproms;
    gCoreBiosInfo->iBios.GetLegacyRegion         = GetLegacyRegion;
    gCoreBiosInfo->iBios.CopyLegacyRegion        = CopyLegacyRegion;
    gCoreBiosInfo->iBios.UpdateKeyboardLedStatus = UpdateKeyboardLedStatus;
    //
    // Call LegacyBiosPlatform to get the end of OpROM
    //
    Status = gCoreBiosInfo->iBiosPlatform->GetPlatformInfo(
                    gCoreBiosInfo->iBiosPlatform,
                    EfiGetPlatformEndOpromShadowAddr,
                    NULL, NULL,
                    &gMaxOpRomAddress,
                    NULL, 0, 0);
    if (EFI_ERROR(Status)) {
        gMaxOpRomAddress = 0xDFFFF;
    }

    ASSERT(gMaxOpRomAddress > 0xC0000 && gMaxOpRomAddress < 0xF0000);
    //
    // Initialize memory
    //
    Status = InitializeLegacyMemory(gCoreBiosInfo);
    if (EFI_ERROR(Status)) return Status;

    // Get the current IRQ mask
    Status = gCoreBiosInfo->i8259->GetMask(gCoreBiosInfo->i8259, &SaveIrqMask, NULL, NULL, NULL);
    ASSERT_EFI_ERROR(Status);

    // Mask all interrupts
    IrqMask = -1;
    Status = gCoreBiosInfo->i8259->SetMask(gCoreBiosInfo->i8259, &IrqMask, NULL, NULL, NULL);
    ASSERT_EFI_ERROR(Status);

    Status = pRS->GetVariable(L"Setup", &gSetupGuid, NULL, &Size, &gSetup);
    if (EFI_ERROR(Status)) {
        pBS->SetMem(&gSetup, sizeof(SETUP_DATA), 0);
    }

    // The following code implements the video mode switching policy during
    // Option ROMs installation. There are two ingredients in this policy
    // implementation - one SDL token and one setup question:
    //
    //  - SDL token CSM_DEFAULT_VMODE_SWITCHING
    //  - Setup question "Option ROM Messages" (use BIOS/keep current)
    //
    // Meaning of the "Option ROM Messages" setup options:
    //  - "Keep Current" installs dummy INT10, output from Option ROMs is
    //    suppressed, value of CSM_DEFAULT_VMODE_SWITCHING is ignored.
    //  - "Use BIOS" will have INT10 available for the Option ROMs to output
    //    the messages.
    // Meaning of the CSM_DEFAULT_VMODE_SWITCHING:
    //  - 0 - switch video mode from graphics to text before executing the
    //      Option ROM, switch back to graphics afterwards
    //  - 1 - use text mode during all Option ROMs execution time, no mode
    //      switching during this time frame.

    gSetTxtMode = 0xff;    // CSM_DEFAULT_VMODE_SWITCHING will be applied after 1st ROM is executed
	if (Status == EFI_SUCCESS && gSetup.ExpansionCardText == 0) {
        gSetTxtMode = 2;
    }

    //
    // Copy Compatibility16 binary into shadow and
    // call its Func0 (InitializeYourself)
    //
    Status = InitCompatibility16(gCoreBiosInfo, SystemTable);
    if (EFI_ERROR(Status)) return Status;

    // Re-enable IRQ mask
    Status = gCoreBiosInfo->i8259->SetMask(gCoreBiosInfo->i8259, &SaveIrqMask, NULL, NULL, NULL);
    ASSERT_EFI_ERROR(Status);

    //
    // Create entries for legacy SW SMIs
    //
    Status = gCoreBiosInfo->iBiosPlatform->SmmInit(
                gCoreBiosInfo->iBiosPlatform,
                &gCoreBiosInfo->Thunk->Csm16BootTable);
    ASSERT_EFI_ERROR(Status);

    gCoreBiosInfo->hVga = NULL;

    gNextRomAddress = (UINT8*)0xC0000;   // Beginning of shadow region
    gIsBootPrepared = FALSE;

    //
    // Allocate memory for saved OpROM structures
    //
    Status = pBS->AllocatePool(EfiBootServicesData,
        MAX_BCV_OPROMS * sizeof(SAVED_PCI_ROM),
        &gSavedOprom);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    pBS->SetMem(gSavedOprom, MAX_BCV_OPROMS * sizeof(SAVED_PCI_ROM), 0);
    gSavedOprom++;   // 1st entry is zero, this will let us find the beginning
                    // afterwards.
    //
    // Allocate memory for PciIo's with executed OpROMs
    //
    Status = pBS->AllocatePool(EfiBootServicesData,
        MAX_EXECUTED_OPROMS * sizeof(AMI_CSM_EXECUTED_PCI_ROM),
        &gExecutedRomsPci);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    pBS->SetMem(gExecutedRomsPci, MAX_EXECUTED_OPROMS * sizeof(AMI_CSM_EXECUTED_PCI_ROM), 0);
    gExecutedRomsPci++;  // 1st entry is zero, this will let us find the
                        // beginning afterwards.


    //
    // Make a new handle and install the protocol
    //
    gCoreBiosInfo->hBios = NULL;
    Status = pBS->InstallMultipleProtocolInterfaces(
                &gCoreBiosInfo->hBios,
                &gEfiLegacyBiosProtocolGuid, &gCoreBiosInfo->iBios,
                &gEfiLegacyBiosExtProtocolGuid, &gAmiCsmExtensionsProtocol,
                NULL
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    //
    // Register callback notification on AllDriverConnected. 
    // This is set to TPL_NOTIFY so that this callback gets executed first
    // and GOP will be ready for the other callback function, who wants to 
    // display based on the AllDriverconnected event
    //
    Status = pBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                                TPL_NOTIFY,
                                AllConnectedCallback,
                                NULL,
                                &Event );
    ASSERT_EFI_ERROR(Status);

    Status = pBS->RegisterProtocolNotify(
                                &gBdsAllDriversConnectedProtocolGuid,
                                Event,
                                &gProtocolNotifyRegistration);

    ASSERT_EFI_ERROR(Status);

    UpdatePciLastBus(); // Update Pci Last Bus number in Csm16 Header

    Status = CreateReadyToBootEvent(TPL_CALLBACK, FreePmmBeforeBoot, NULL, &Event);
    ASSERT_EFI_ERROR(Status);

    return EFI_SUCCESS; // Force success: every unsuccessful status is branched
                        // before, GetVariable status will not be successful on 
                        // a "fresh" boot.
}

/**
    This function calls FindEmbeddedRom to search for onboard Option ROM.


**/

EFI_STATUS
EFIAPI
GetEmbeddedRom (
    IN UINT16 ModuleId,
    IN UINT16 VendorId,
    IN UINT16 DeviceId,
    OUT VOID **ImageStart,
    OUT UINTN *ImageSize
)
{
    return FindEmbeddedRom(ModuleId, VendorId, DeviceId, ImageStart, ImageSize);
}


/**
    This function initialize AMI specific CSM16 data: POST/RT variables in
    CSM16 based on Setup settings, OEM porting overrides. It will be skipped
    in case of non-AMI CSM16 binary.

 
    @note  
  Shadow regions are unlocked.

**/

VOID
InitializeCsm16MiscInfo(
    LEGACY16_TO_EFI_DATA_TABLE_STRUC    *Csm16Data
)
{
    UINT8   KbcSupport;
    UINTN   Data;
    EFI_STATUS  Status;

    //
    // Initialize misc_info
    //
#ifdef KBC_SUPPORT
    KbcSupport = KBC_SUPPORT;
#else
    KbcSupport = ((IoRead8(0x64)== 0xff))? 0 : 1; 
#endif
    
    if (KbcSupport == 0) {
        Csm16Configuration(Csm16FeatureSet, Csm16LegacyFree, NULL);
    }
    else {
        Csm16Configuration(Csm16FeatureReset, Csm16LegacyFree, NULL);
    }

    Csm16Configuration(Csm16FeatureSet, Csm16I19Trap, NULL);
    Csm16Configuration(Csm16FeatureSet, Csm16Flexboot, NULL);

    // Set GA20 related information in misc_info and rt_cmos_byte
	Status = GetGateA20Information(&Data);

    if (!EFI_ERROR(Status)) {
        if (Data & 1) {
            Csm16Configuration(Csm16FeatureSet, Csm16GA20, NULL);
        } else {
            Csm16Configuration(Csm16FeatureReset, Csm16GA20, NULL);
        }
        if (Data & 2) {
            Csm16Configuration(Csm16FeatureSet, Csm16SkipGA20Deactivation, NULL);
        } else {
            Csm16Configuration(Csm16FeatureReset, Csm16SkipGA20Deactivation, NULL);
        }
    }
    //SMC R1.00 - Start
#if SMCPKG_SUPPORT
    	if (gSetup.SmcReTryBoot == 1)
    	{
            Csm16Configuration(Csm16FeatureSet, Csm16HeadlessI19Retry, NULL);
    	}
#endif
    //SMC R1.00 - End

    // Set NMI related information in misc_info and rt_cmos_byte; if porting function is not
    // implemented, enable NMI
	Status = GetNmiInformation(&Data);
	
    if (EFI_ERROR(Status) || (Data == 1)) {
        Csm16Configuration(Csm16FeatureSet, Csm16Nmi, NULL);
     } else {
         Csm16Configuration(Csm16FeatureReset, Csm16Nmi, NULL);
    }
/*
    // Allocate stack for reverse thunk
    {
        EFI_PHYSICAL_ADDRESS RtStackPtr;
        UINTN CsmRtStackPtrLocation;

        Status = pBS->AllocatePages(AllocateAnyPages, EfiReservedMemoryType, 16, &RtStackPtr);
        ASSERT_EFI_ERROR(Status);
        pBS->SetMem( (UINT8*)(UINTN)RtStackPtr, 0x10000, 0 );
        CsmRtStackPtrLocation = 0xf0000 + Csm16Data->ReverseThunkStackPtrOfs;
        *(UINTN*)CsmRtStackPtrLocation = (UINTN)RtStackPtr + 0x10000;
    } */
    {
        // Update the E000 memory bounds
        UINTN StartOfFreeE000Ptr = 0xf0000 + Csm16Data->StartOfFreeE000Ofs;
        UINT16 StartOfFreeE000;
        UINT16 SizeOfFreeE000;

        ASSERT(gMaxOpRomAddress >= 0xdffff);
        ASSERT(gMaxOpRomAddress < 0xeffff);

        StartOfFreeE000 = (UINT16)((UINT32)gMaxOpRomAddress + 1);   // Cut off upper word
        SizeOfFreeE000 = ~StartOfFreeE000;

        *(UINT16*)StartOfFreeE000Ptr = StartOfFreeE000;
        *(UINT16*)(StartOfFreeE000Ptr + 2) = SizeOfFreeE000;
    }
    {
        // Allocate legacy boot context save buffer and pass the pointer and the size to CSM16
        UINTN NumberOfPages = EFI_SIZE_TO_PAGES(LEGACY_TO_EFI_BOOT_BUFFER_SIZE);
        EFI_PHYSICAL_ADDRESS Address = 0xffffffff;
        UINTN Csm16BufferPointer = 0xf0000 + Csm16Data->BootBufferPtrOfs;

        Status = pBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiACPIMemoryNVS,
                    NumberOfPages,
                    &Address
        );
        ASSERT_EFI_ERROR(Status);

        TRACE((-1, "CSM LegacyBoot context save buffer: %x, size %x; pointer is updated at %x\n",
                Address, LEGACY_TO_EFI_BOOT_BUFFER_SIZE, Csm16BufferPointer));

        *(UINT32*)Csm16BufferPointer = (UINT32)Address;
        *(UINT32*)(Csm16BufferPointer+4) = LEGACY_TO_EFI_BOOT_BUFFER_SIZE;

#if LEGACY_TO_EFI_DEFAULT
#if SMCPKG_SUPPORT
        if(gSetup.LegacyToEFISupport == 1){
#endif
        Csm16Configuration(Csm16FeatureSet, Csm16HeadlessI19Retry, NULL);
        Csm16Configuration(Csm16FeatureSet, Csm16LegacyToEfi, NULL);
#if SMCPKG_SUPPORT
        }
#endif
#else
        Csm16Configuration(Csm16FeatureReset, Csm16HeadlessI19Retry, NULL);
        Csm16Configuration(Csm16FeatureReset, Csm16LegacyToEfi, NULL);
#endif
    }
}


/**
    This function enables all PCI devices before legacy boot

**/

EFI_STATUS EnablePciDevices(EFI_LEGACY_BIOS_PROTOCOL *Bios)
{
    EFI_PCI_IO_PROTOCOL *PciIo;
    UINTN       Count;
    EFI_HANDLE  *Buffer;
    UINTN       i;
    EFI_STATUS  Status;
    UINT8       PciCfg[4];
    UINT64      Capabilities;

    Status = pBS->LocateHandleBuffer(ByProtocol,
                    &gEfiPciIoProtocolGuid,
                    NULL,
                    &Count,
                    &Buffer);
    ASSERT_EFI_ERROR (Status);

    for (i = 0; i < Count; i++) {
        Status = pBS->HandleProtocol (Buffer[i], &gEfiPciIoProtocolGuid, &PciIo);
        ASSERT_EFI_ERROR (Status);

        PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 8, 1, &PciCfg);    // Reg 8,9,A,B

        //
        // Skip enabling device for:
        // - VGA
        // - Old VGA
        // - IDE
        // - LPC Bridge
        //
        if ((PciCfg[3]==PCI_CL_DISPLAY && PciCfg[2]==PCI_CL_DISPLAY_SCL_VGA)
            || (PciCfg[3]==PCI_CL_OLD && PciCfg[2]==PCI_CL_OLD_SCL_VGA)
            || (PciCfg[3]==PCI_CL_MASS_STOR && PciCfg[2]==PCI_CL_MASS_STOR_SCL_IDE)) {
            continue;
        }
        Status = PciIo->Attributes (PciIo,
                EfiPciIoAttributeOperationSupported, 0,
                &Capabilities);     // Get device capabilities
        ASSERT_EFI_ERROR(Status);

        Status = PciIo->Attributes (PciIo,
                EfiPciIoAttributeOperationEnable,
                Capabilities & EFI_PCI_DEVICE_ENABLE,
                NULL);              // Enable device
        ASSERT_EFI_ERROR(Status);
    }

    pBS->FreePool(Buffer);

    return EFI_SUCCESS;
}


/**
    In a multiple root-complex platform, the CSM16 routine "rth_pci_get_last_bus"
    needs to return the correct bus number. For example, in a 2 IOH system, the
    base bus #s for IOH0 can be 0 and IOH1 can be 80h. It should not be assumned
    there is only one root-complex and return the correct bus # (max bus of first IOH),
    for the card is plugged into the 2nd IOH.

    This function creates a callback notification event for EfiPciRootBridgeProtocol GUID
    installation. The callback function then will process the list of root bridges and
    will update the LastPciBus field of EFI_COMPATIBILITY16_TABLE accordingly.

**/

EFI_STATUS UpdatePciLastBus()
{
    EFI_STATUS  Status;
    EFI_EVENT   UpdatePciLastBusEvent;
    VOID        *UpdatePciLastBusReg = NULL;

    //
    // Create the notification and register callback function on the AmiExtPciBusProtocolGuid,
    // callback function will update Pci Last Bus in Csm16 header.
    //
    Status = pBS->CreateEvent (
        EVT_NOTIFY_SIGNAL,
        TPL_CALLBACK,
        UpdatePciLastBusCallback,
        NULL,
        &UpdatePciLastBusEvent);
    ASSERT_EFI_ERROR(Status);

    if (EFI_ERROR(Status)) return Status;

    Status = pBS->RegisterProtocolNotify (
        &gAmiExtPciBusProtocolGuid,
        UpdatePciLastBusEvent,
        &UpdatePciLastBusReg);
    ASSERT_EFI_ERROR(Status);

    return Status;
}


/**
    This function is executed after all drivers have been connected.

    @note  
  Specific to Core version, first time this event is signaled in 4.6.3.1.

**/

VOID
AllConnectedCallback (
    EFI_EVENT Event,
    VOID      *Context
)
{
    BBS_TABLE *BbsEntry;
    UINT8 count=0;
    EFI_STATUS  Status;
    EFI_IA32_REGISTER_SET RegSet;
    UINT32 Granularity;
	static BOOLEAN IsBbsInitialized = FALSE;

	TRACE((-1, "CSM.AllConnectedCallback: SetMode: %x VgaHandle %x\n", gSetTxtMode, gVgaHandle));

    if (gSetTxtMode == 1)
    {
        if (gVgaHandle != NULL)
        {
            TRACE((-1, "Reconnecting video and serial after all OPROMs are done.\n"));
            pBS->DisconnectController(gVgaHandle, NULL, NULL);
            pBS->ConnectController(gVgaHandle, NULL, NULL, TRUE);
            ConnectSerialIO();
        }
        UnlockConsole();
    }

	// Install legacy mass storage devices
    ASSERT(!IsBbsInitialized);	// reentrance guard
	IsBbsInitialized = TRUE;

    //
	// Insert floppy drive entry
	//
	BbsEntry = gCoreBiosInfo->BbsTable;
	BbsEntry->Bus        = 0xFF;
	BbsEntry->Device     = 0xFF;
	BbsEntry->Function   = 0xFF;
	BbsEntry->DeviceType = BBS_FLOPPY;
	BbsEntry->Class      = 1;
	BbsEntry->SubClass   = 2;

	switch (GetFDDStatus()) {
		case NO_FLOPPY_DRIVE: BbsEntry->BootPriority = BBS_IGNORE_ENTRY; break;
		case NO_FLOPPY_DISK:
		case FLOPPY_INSERTED: BbsEntry->BootPriority = BBS_UNPRIORITIZED_ENTRY; break;
	}

	InstallLegacyMassStorageDevices();

	Status = UpdateCsm16Configuration(&gCoreBiosInfo->Thunk->Csm16BootTable);
	ASSERT_EFI_ERROR(Status);

	//
	// Make UpdateBBS call to CSM16 to adjust the table if needed.
	//
	pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
	RegSet.X.AX = Legacy16UpdateBbs;
	RegSet.X.ES = EFI_SEGMENT (&gCoreBiosInfo->Thunk->Csm16BootTable);
	RegSet.X.BX = EFI_OFFSET (&gCoreBiosInfo->Thunk->Csm16BootTable);

	Status = gCoreBiosInfo->iRegion->UnLock (
		gCoreBiosInfo->iRegion,
		(UINT32)gMaxOpRomAddress+1,
		0xFFFFF-(UINT32)gMaxOpRomAddress,
		&Granularity
	);
	ASSERT_EFI_ERROR(Status);

	FarCall86 (&gCoreBiosInfo->iBios,
				gCoreBiosInfo->Csm16EntrySeg,
				gCoreBiosInfo->Csm16EntryOfs,
				&RegSet,
				NULL,
				0);
	Status = (RegSet.X.AX)? ((RegSet.X.AX & 0x7FFF) | EFI_ERROR_BIT) : EFI_SUCCESS;
	ASSERT_EFI_ERROR(Status);
	gCoreBiosInfo->iRegion->Lock (
		gCoreBiosInfo->iRegion,
		(UINT32)gMaxOpRomAddress+1,
		0xFFFFF-(UINT32)gMaxOpRomAddress,
		&Granularity
	);

	gCoreBiosInfo->BbsEntriesNo = gCoreBiosInfo->Thunk->Csm16BootTable.NumberBbsEntries;
	IsBbsInitialized = TRUE;
}


/**
    This function identifies the validity of CSM16 binary by searching "$EFI"
    and verifying table checksum and returs the location of $EFI table

    @param RomImageStart    Binary location address
    @param RomImageSize     Size in bytes

    @retval EFI_COMPATIBILITY16_TABLE*
	    Address of the beginning of $EFI table or NULL if this table is not found

    @note  
  Compatibility16 binary is linked with the current section

**/

EFI_COMPATIBILITY16_TABLE*
FindCompatibility16Table(
    VOID *RomImageStart,
    UINTN RomImageSize
)
{
    UINT8 *ImageStart = RomImageStart;
    UINT8 *ImageEnd = ImageStart + RomImageSize;

    for (;ImageStart < ImageEnd; ImageStart +=16) {
        if (*(UINT32*)ImageStart == '$EFI') {
            UINT8 TableLength = ((EFI_COMPATIBILITY16_TABLE*)ImageStart)->TableLength;
            UINT8 Checksum = 0;
            UINT8 i;
            for(i=0; i < TableLength; ++i) Checksum += ImageStart[i];
            return (Checksum == 0) ? (EFI_COMPATIBILITY16_TABLE*)ImageStart : 0;
        }
    }

    return NULL;
}


/**
    This function goes through system memory map and clears memory that is not
    allocated within the given range.

    @retval 
        EFI_STATUS the status of memory clearing operation

**/

EFI_STATUS ClearFreeMemory(
    EFI_PHYSICAL_ADDRESS AddrFrom,
    EFI_PHYSICAL_ADDRESS AddrTo
)
{
    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    EFI_MEMORY_DESCRIPTOR *MemDesc = NULL;
    UINTN DescriptorSize = 0;
    UINTN NumberOfDescriptors = 0;
    EFI_STATUS Status;
    UINTN i;
    UINTN Size;

    Status = GetSystemMemoryMap(&MemoryMap, &DescriptorSize, &NumberOfDescriptors);
    ASSERT_EFI_ERROR(Status);

    if (EFI_ERROR(Status)) return Status;

    for (i = 0; i < NumberOfDescriptors; i++)
    {
        MemDesc = (EFI_MEMORY_DESCRIPTOR*)((UINT8*)MemoryMap + i * DescriptorSize);

        if (MemDesc->Type == EfiConventionalMemory &&
            MemDesc->PhysicalStart < AddrTo &&
            MemDesc->PhysicalStart >= AddrFrom
        )
        {
            Size = (UINTN)MemDesc->NumberOfPages * EFI_PAGE_SIZE;
            if (((UINTN)MemDesc->PhysicalStart + Size) > (UINTN)AddrTo)
            {
                Size = (UINTN)AddrTo - (UINTN)MemDesc->PhysicalStart;
            }

            pBS->SetMem((VOID*)(UINTN)MemDesc->PhysicalStart, Size, 0);
        }
    }

    pBS->FreePool(MemoryMap);

    return EFI_SUCCESS;
}


/**
    This function initializes CSM related memory regions

    @param 
        BIOS_INFO *CoreBiosInfo

    @retval 
        EFI_STATUS  Legacy memory initialization status

**/

EFI_STATUS  InitializeLegacyMemory(BIOS_INFO *CoreBiosInfo)
{
    EFI_PHYSICAL_ADDRESS    Address;
    EFI_CPU_ARCH_PROTOCOL   Cpu;
    EFI_CPU_ARCH_PROTOCOL   *pCpu = &Cpu;
    EFI_GUID                gCpu = EFI_CPU_ARCH_PROTOCOL_GUID;
    EFI_STATUS              Status;
    BDA_DATA                *bda;
    UINT8                   *ebda;
    BBS_TABLE               *BbsEntry;
    UINT16                  i;
    UINT32                  TopOfBaseMemory;

    //
    // Set memory attributes for legacy regions
    //
    Status = pBS->LocateProtocol(&gCpu, NULL, &pCpu);
    if (EFI_ERROR(Status)) return Status;
    pCpu->SetMemoryAttributes(pCpu, 0xC0000, 0x40000, EFI_MEMORY_WP);   // WriteProtected

    Status = ClearFreeMemory(0, 0xa0000);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    //
    // Allocate PMM and EBDA areas at the top of 640K; if some memory was already allocated,
    // then adjust base memory information accordingly in 40:13 and EBDA
    //
    Address = 0xa0000;
    Status = pBS->AllocatePages (
                AllocateMaxAddress,
                EfiBootServicesCode,
                PMM_EBDA_LOMEM_SIZE >> 12,  // Bytes->4KiloBytes conversion
                &Address);

    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    TopOfBaseMemory = (UINT32)Address+PMM_EBDA_LOMEM_SIZE;
    ASSERT(TopOfBaseMemory>=PMM_EBDA_LOMEM_SIZE+0x8000);	// PMM, EBDA, and extra 32KB must fit in
    if (TopOfBaseMemory<(PMM_EBDA_LOMEM_SIZE+0x8000))
        return EFI_OUT_OF_RESOURCES;

    //
    // Allocate base memory for IVT/BDA(0..2K), PMM data(2K..6K), Thunk (6K..32K),
    // low memory PMM and EBDA (640K-PMM_EBDA_LOMEM_SIZE .. 640K).
    //
    Address = 0;    // Allocate 0..32K
    Status = pBS->AllocatePages (AllocateAddress,
                    EfiBootServicesCode, 8, &Address);
    ASSERT_EFI_ERROR(Status);

    CoreBiosInfo->Thunk = (THUNK_MEMORY*)0x1800;    // at 6K

    //
    // Initialize BDA and EBDA
    //
    bda = (BDA_DATA*)((UINTN) 0x400);

    bda->system_memory = (TopOfBaseMemory>>10)-5;   // 5K is for BIOS EBDA
    bda->kb_buf_head_ptr = bda->kb_buf_tail_ptr = 0x1E;
    bda->motor_time_count = 0xFF;
    bda->kb_buff_begin = 0x1E;
    bda->kb_buff_end = 0x3E;

    bda->current_floppy_media[0] = 7;
    bda->winch_number = 0;
//  bda->machine_config is updated later when SIO information becomes available

    bda->ext_bios_data_seg = (TopOfBaseMemory>>4)-0x140;

    ebda = (UINT8*)((UINTN)(TopOfBaseMemory-0x1400));
    *ebda = 5;  // 5K

    //
    // Create BBS table and initialize it with BBS_IGNORE_ENTRY values
    //
    Status = pBS->AllocatePool(
                EfiBootServicesData,
                sizeof(BBS_TABLE)*MAX_BBS_ENTRIES_NO,
                &CoreBiosInfo->BbsTable);
    pBS->SetMem(CoreBiosInfo->BbsTable, sizeof(BBS_TABLE)*MAX_BBS_ENTRIES_NO, 0);

    for (BbsEntry = CoreBiosInfo->BbsTable, i = 0; i < MAX_BBS_ENTRIES_NO; i++, BbsEntry++) {
        BbsEntry->BootPriority = BBS_IGNORE_ENTRY;
    }

    CoreBiosInfo->BbsEntriesNo = MAX_IDE_CONTROLLER*2 + 1;  //  1 floppy + up to MAX_IDE_CONTROLLER*2 HDD

    return Status;
}


/**
    This function reads Compatibility16 binary into the pre-defined shadow
    memory region, executes InitializeYourself function and initializes Thunk
    data structures.

    @param        BIOS_INFO           *CoreBiosInfo
    @param        EFI_SYSTEM_TABLE    *SystemTable

    @retval 
        EFI_SUCCESS = Success

    @note  
  InitializeLegacyMemory has to be executed prior to this function

**/

EFI_STATUS
InitCompatibility16(
    IN BIOS_INFO        *CoreBiosInfo,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS          Status;
    VOID                *RomImageStart = NULL;
    UINTN               RomImageSize = 0;
    EFI_IA32_REGISTER_SET   RegSet;
    EFI_TO_COMPATIBILITY16_INIT_TABLE   *Csm16InitTable;
    UINT32              *ivt = (UINT32*)0;
    UINT16              RedirSeg, RedirOfs;
    UINT8               i;
    UINTN               LowPmmMemorySizeInBytes;
    UINTN               HiPmmMemorySizeInBytes;
    UINTN               HiPmmMemory;
    UINTN               PciExpressBase;
    UINT32              FirmwareRevision = (pST->FirmwareRevision) ? pST->FirmwareRevision : CORE_COMBINED_VERSION;

    struct IRQ_REDIRECTION_CODE {
        UINT8 intOpCode;
        UINT8 intnumOpCode;
        UINT8 iretOpCode;
    } RedirTable[8] = {
        {0xcd, 0x08, 0xcf}, // int 8, iret
        {0xcd, 0x09, 0xcf}, // int 9, iret
        {0xcd, 0x0a, 0xcf}, // int a, iret
        {0xcd, 0x0b, 0xcf}, // int b, iret
        {0xcd, 0x0c, 0xcf}, // int c, iret
        {0xcd, 0x0d, 0xcf}, // int d, iret
        {0xcd, 0x0e, 0xcf}, // int e, iret
        {0xcd, 0x0f, 0xcf}  // int f, iret
    };

    UINT8 *InterruptsToPreserve;
    UINTN NumberOfInterruptsToPreserve;
    UINT32 Granularity;
    BOOLEAN On;

    //
    // Enable decoding of shadow regions
    //
    On = TRUE;
    Status = CoreBiosInfo->iRegion->Decode (CoreBiosInfo->iRegion,
        0xC0000, 0x40000,&Granularity, &On);

    //
    // Open all shadow regions for writing and initalize them
    //
    Status = CoreBiosInfo->iRegion->UnLock (CoreBiosInfo->iRegion,
        0xC0000, 0x40000,&Granularity);

    pBS->SetMem((UINT8*)(UINTN)0xC0000, gMaxOpRomAddress-0xBFFFF, 0xFF);
    pBS->SetMem((UINT8*)(gMaxOpRomAddress+1), 0xFFFFF-gMaxOpRomAddress, 0);

    CoreBiosInfo->HddCount = 0;

    //
    // Get Eficompatibility16 binary
    //
    Status = CoreBiosInfo->iBiosPlatform->GetPlatformInfo(
                    CoreBiosInfo->iBiosPlatform,
                    EfiGetPlatformBinarySystemRom,
                    &RomImageStart, &RomImageSize,
                    NULL, NULL, 0, 0);
    if (EFI_ERROR(Status)) return Status;

    pBS->CopyMem((VOID*)((UINTN)(0x100000-RomImageSize)), RomImageStart, RomImageSize);
    RomImageStart = (VOID*)((UINTN)(0x100000-RomImageSize));

    CoreBiosInfo->Csm16Header = FindCompatibility16Table(RomImageStart, RomImageSize);
    if (CoreBiosInfo->Csm16Header == NULL) return EFI_NOT_FOUND;

    //
    // Save CSM16 entry point
    //
    CoreBiosInfo->Csm16EntrySeg = CoreBiosInfo->Csm16Header->Compatibility16CallSegment;
    CoreBiosInfo->Csm16EntryOfs = CoreBiosInfo->Csm16Header->Compatibility16CallOffset;

    //
    // Get PCIe base address
    //
    Status = CoreBiosInfo->iBiosPlatform->GetPlatformInfo(
                    CoreBiosInfo->iBiosPlatform,
                    EfiGetPlatformPciExpressBase,
                    NULL, NULL,
                    &PciExpressBase,
                    NULL, 0, 0);

    CoreBiosInfo->Csm16Header->PciExpressBase = (EFI_ERROR(Status))? 0 : (UINT32)PciExpressBase;

    CoreBiosInfo->Csm16Header->EfiSystemTable = (UINT32)SystemTable;

    //
    // Initialize thunk code
    //
    Status = InitializeThunk(CoreBiosInfo);
    if (EFI_ERROR(Status)) return Status;

    //
    // Fill in EFI_TO_COMPATIBILITY16_INIT_TABLE for InitializeYourself input
    //
    Csm16InitTable = &CoreBiosInfo->Thunk->Csm16InitTable;  // Located below 1 MB
    Csm16InitTable->ThunkStart = (UINT32)CoreBiosInfo->Thunk;
    Csm16InitTable->ThunkSizeInBytes = sizeof (THUNK_MEMORY);
    Csm16InitTable->LowPmmMemory = 0xa0000-PMM_EBDA_LOMEM_SIZE;
    Csm16InitTable->ReverseThunkCallSegment = Csm16InitTable->ThunkStart >> 4;
    Csm16InitTable->ReverseThunkCallOffset = *(UINT16*)(UINTN)(Csm16InitTable->ThunkStart + 4);

    GetExtendedMemSize(&Csm16InitTable->OsMemoryAbove1Mb);

    //
    // E820 table will be valid before PrepareToBoot
    //
    Csm16InitTable->NumberE820Entries = 0;

    //
    // Call LegacyBiosPlatform to get the PMM boundaries
    //
    Status = CoreBiosInfo->iBiosPlatform->GetPlatformInfo(
                    CoreBiosInfo->iBiosPlatform,
                    EfiGetPlatformPmmSize,
                    NULL,
                    &LowPmmMemorySizeInBytes,
                    NULL,
                    &HiPmmMemorySizeInBytes,
                    0, 0);
    ASSERT_EFI_ERROR(Status);

    Csm16InitTable->LowPmmMemorySizeInBytes = (UINT32)LowPmmMemorySizeInBytes;

    Status = AllocateHiMemPmmBlock((HiPmmMemorySizeInBytes >> 12) + 1, &HiPmmMemory);
    if (EFI_ERROR(Status))
    {
        Csm16InitTable->HiPmmMemorySizeInBytes = 0;
        Csm16InitTable->HiPmmMemory = 0;
        TRACE((-1, "HI PMM memory block is NOT allocated.\n"));
        ERROR_CODE(DXE_CSM_INIT,EFI_ERROR_MINOR);
    }
    else
    {
        Csm16InitTable->HiPmmMemorySizeInBytes = (UINT32)HiPmmMemorySizeInBytes;
        Csm16InitTable->HiPmmMemory = (UINT32)HiPmmMemory;
        TRACE((-1, "HI PMM memory allocated at 0x%x, size 0x%x\n", HiPmmMemory, HiPmmMemorySizeInBytes));
    }

    //
    // Make a call to 16 bit code to initialize itself
    //
    pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.X.AX = Legacy16InitializeYourself;
    RegSet.X.ES = EFI_SEGMENT (Csm16InitTable);
    RegSet.X.BX = EFI_OFFSET (Csm16InitTable);
    #if ACPI_TIMER_IN_LEGACY_SUPPORT
    // ACPI timer address is passed to AmiLegacy16 binary in the following structure
    // Width (0/1) | Signature 'TM'| base address
    // 31          30              15           0   
    RegSet.E.ECX = (FACP_FLAG_TMR_VAL_EXT << ACPI_TMR_WIDTH_SHIFT) 
                    | ( ACPI_TMR_SIGNATURE << ACPI_TMR_SIGNATURE_SHIFT) 
                    |  PM_TMR_BLK_ADDRESS;
    #else
    RegSet.E.ECX = 0;
    #endif
    
    FarCall86 (&CoreBiosInfo->iBios,
                CoreBiosInfo->Csm16EntrySeg,
                CoreBiosInfo->Csm16EntryOfs,
                &RegSet,
                NULL,
                0);
    Status = (RegSet.X.AX)? ((RegSet.X.AX & 0x7FFF) | EFI_ERROR_BIT) : EFI_SUCCESS;
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    // InitializeYourself (func #0) that has just executed, returns a pointer to
    // LEGACY16_TO_EFI_DATA_TABLE_STRUC to pass the AMIBIOS specific data to CSM16
    // This structure is AMI BIOS specific, so that the following call that updates
    // CSM16 data structures must be avoided for non-AMI CSM16 binaries.

    if (IsAMICSM16(CoreBiosInfo->Csm16Header)) {
        // Program POST/RT variables in CSM16 based on Setup settings, OEM porting overrides
        InitializeCsm16MiscInfo(
            (LEGACY16_TO_EFI_DATA_TABLE_STRUC*)(UINTN)Csm16InitTable->BiosLessThan1MB
        );
    }

    Status = InitializePortingHooks(CoreBiosInfo);
    if (EFI_ERROR(Status)) {
        TRACE((-1,"No OEM/Chipset CSM16 porting hooks found.\n"));
    }

    // Update project specific fields in CSM16 bin
    if (IsAMICSM16(CoreBiosInfo->Csm16Header)) {
        // BIOS date, time, and version information
        UINT8   BiosDate[] = TODAY;
        UINT8   BiosTime[] = NOW;
        UINT8   *p = (UINT8*)0xff55b;

        pBS->CopyMem(p, BiosDate, 6);    // MM/DD/
        // Place two-digit year since TODAY is in a different format
        p[6] = BiosDate[8];
        p[7] = BiosDate[9];

        pBS->CopyMem(&p[9], BiosTime, 8);   // BIOS time

        
        Sprintf(&p[23], "%02X.%05X", ((UINT16*)&FirmwareRevision)[1], *(UINT16*)&FirmwareRevision);

        // Update BIOS project tag
		p = (UINT8*)0xff4f0;
/* PROJECT specific data is not found in AptioV, uncomment when it is available
        // The following conversion assumes the following:
        // 1. PROJECT_TAG to be 5 Bytes long, e.g. '0ABCD'; this assumption
        //    follows AMI project naming convention.
        // 2. PROJECT_MAJOR_VERSION is limited to one decimal digit, i.e. in
        //    the range from 0 to 9.
        //    If it is greater than 9, then the upper digit(s) will be truncated. This is
        //    needed to keep the length of the project tag as 8.
        Sprintf(&p[0], "%s%1d%02d",
            CONVERT_TO_STRING(PROJECT_TAG), PROJECT_MAJOR_VERSION, PROJECT_MINOR_VERSION);*/
    }

    ChecksumCSM16Header(CoreBiosInfo->Csm16Header);

    //
    // Lock legacy region
    //
    CoreBiosInfo->iRegion->Lock (
        CoreBiosInfo->iRegion,
        0xC0000,
        0x40000,
        &Granularity
    );

    //
    // Initialize interrupt save tables with 0
    //
    pBS->SetMem(gIrqSav, sizeof(gIrqSav), 0);

    //
    // Prepare hardware interrupt (IRQ0...IRQF) redirection table
    //
    pBS->CopyMem(&CoreBiosInfo->Thunk->InterruptRedirectionCode, RedirTable, 8*3);
    RedirSeg = EFI_SEGMENT(CoreBiosInfo->Thunk->InterruptRedirectionCode);
    RedirOfs = EFI_OFFSET(CoreBiosInfo->Thunk->InterruptRedirectionCode);

    CoreBiosInfo->i8259->GetVector(CoreBiosInfo->i8259, 0, &gIrq0);

    for(i = 0; i < 8; i++) {
        UINT32 vector;

        gHwIrqRedirSav[i] = ivt[gIrq0 + i];
        vector = RedirSeg << 4 | RedirOfs;
        ivt[gIrq0 + i] = vector;
        RedirOfs += 3;
    }

     Status = GetPlatformInformationEx(&gAmiCsmExtensionsProtocol,
                     EfiGetPlatformIntSaveRestoreTable,
                     &InterruptsToPreserve,
                     &NumberOfInterruptsToPreserve,
                     NULL, NULL, 0, 0);

     if (!EFI_ERROR(Status)) {
        if (NumberOfInterruptsToPreserve > MAX_NUM_IRQS_SAVERESTORE) {
            NumberOfInterruptsToPreserve = MAX_NUM_IRQS_SAVERESTORE;
        }
        for (i = 0; i < NumberOfInterruptsToPreserve; i++) {
            gIrqSav[i].irq_num = InterruptsToPreserve[i];
            gIrqSav[i].irq_adr = ivt[InterruptsToPreserve[i]];
        }
    }

    return EFI_SUCCESS;
}



/**
    Presents BBS information to external agents.

    @param        This        Indicates the EFI_LEGACY_BIOS_PROTOCOL instance.
    @param        HddCount    Number of HDD_INFO structures.
    @param        HddInfo     Onboard IDE controller information.
    @param        BbsCount    Number of BBS_TABLE structures.
    @param        BbsTable    BBS entry.

**/

EFI_STATUS EFIAPI
GetBbsInfo (
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  OUT  UINT16                         *HddCount,
  OUT  HDD_INFO               **HddInfo,
  OUT  UINT16                         *BbsCount,
  IN OUT  BBS_TABLE           **BbsTable
)
{
    *HddCount = gCoreBiosInfo->HddCount;
    *HddInfo  = (HDD_INFO*)gCoreBiosInfo->Thunk->Csm16BootTable.HddInfo;
    *BbsCount = gCoreBiosInfo->BbsEntriesNo;
    *BbsTable = (BBS_TABLE*)gCoreBiosInfo->BbsTable;

    return EFI_SUCCESS;
}


/**
    Updates the BDA to reflect status of the Scroll Lock, Num Lock, and Caps
    Lock keys and LEDs.

    @param       This    Indicates the EFI_LEGACY_BIOS_PROTOCOL instance.
    @param        Leds    Current LED status, as follows:
        Bit 0 - Scroll Lock         
        Bit 1 - Num Lock
        Bit 2 - Caps Lock
    @retval 
        EFI_SUCCESS The BDA was updated successfully.

**/

EFI_STATUS EFIAPI
UpdateKeyboardLedStatus(
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  IN  UINT8                           Leds
)
{
    *(UINT8*)0x417 &= 0x8F;         // Clear BIT4, BIT5 and BIT6
    *(UINT8*)0x417 |= (Leds << 4);  // Set the state of xLock keys
    return EFI_SUCCESS;
}


/**
    This fuction allocates CSM16 memory within E0000..FFFFF and then copies the
    passed data into allocated region.

    @param Table Data to be copied
	@param TableSize data size
	@param Csm16LocationAttribute CSM16 and memory location attribute
        (F0000_BIT or E0000_BIT or both)

    @note  
  CSM16 memory must be open for writing.

**/

UINTN
EFIAPI
CopyLegacyTable(
    VOID *Table,
    UINT16 TableSize,
    UINT16 Alignment,
    UINT16 Csm16LocationAttribute
	)
{
    EFI_IA32_REGISTER_SET   RegSet;
    EFI_STATUS Status;
    UINTN Dest = 0;
    UINT32 Granularity;

    Status = gCoreBiosInfo->iRegion->UnLock (
        gCoreBiosInfo->iRegion,
        (UINT32)gMaxOpRomAddress+1,
        0xFFFFF-(UINT32)gMaxOpRomAddress,
        &Granularity
    );
    ASSERT_EFI_ERROR(Status);

    pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.X.AX = Legacy16GetTableAddress;
    RegSet.X.BX = Csm16LocationAttribute;
    RegSet.X.CX = TableSize;
    RegSet.X.DX = Alignment;

    FarCall86 (&gCoreBiosInfo->iBios,
                gCoreBiosInfo->Csm16EntrySeg,
                gCoreBiosInfo->Csm16EntryOfs,
                &RegSet,
                0, 0);

    Status = (RegSet.X.AX)? EFI_OUT_OF_RESOURCES : EFI_SUCCESS;
    ASSERT_EFI_ERROR(Status);

    if (!(EFI_ERROR(Status))) {
        Dest = ((UINTN)RegSet.X.DS << 4) + RegSet.X.BX;
        pBS->CopyMem((VOID*)Dest, Table, TableSize);
    }

    Status = gCoreBiosInfo->iRegion->Lock (
        gCoreBiosInfo->iRegion,
        (UINT32)gMaxOpRomAddress+1,
        0xFFFFF-(UINT32)gMaxOpRomAddress,
        &Granularity
    );
    ASSERT_EFI_ERROR(Status);

    return Dest;
}


/**
    This function is called when EFI needs to reserve an area in the 0xE0000
    or 0xF0000 64 KB blocks.
 

    @param        This                Indicates the EFI_LEGACY_BIOS_PROTOCOL instance.
    @param        LegacyMemorySize    Requested size in bytes of the region.
    @param        Region              Requested region.
    @param        00 = Either 0xE0000 or 0xF0000 blocks.
        Bit0 = 1 Specify 0xF0000 block
        Bit1 = 1 Specify 0xE0000 block
    @param        Alignment           Bit-mapped value specifying the address alignment 
        of the requested region. The first nonzero value 
        from the right is alignment.
    @param        LegacyMemoryAddress Address assigned.

    @retval       EFI_SUCCESS The requested region was assigned.
    @retval       EFI_ACCESS_DENIED   The function was previously invoked.
    @retval       Other   The requested region was not assigned.

**/

EFI_STATUS EFIAPI
GetLegacyRegion (
  IN EFI_LEGACY_BIOS_PROTOCOL          *This,
  IN UINTN                             LegacyMemorySize,
  IN UINTN                             Region,
  IN UINTN                             Alignment,
  OUT VOID                             **LegacyMemoryAddress
 )
{
    EFI_IA32_REGISTER_SET   Regs;
    EFI_STATUS              Status;
    UINT32 Granularity;

    gCoreBiosInfo->iRegion->UnLock (
        gCoreBiosInfo->iRegion,
        (UINT32)gMaxOpRomAddress+1,
        0xFFFFF-(UINT32)gMaxOpRomAddress,
        &Granularity
    );

    pBS->SetMem(&Regs, sizeof (EFI_IA32_REGISTER_SET), 0);
    Regs.X.AX = Legacy16GetTableAddress;
    Regs.X.BX = (UINT16) Region;
    Regs.X.CX = (UINT16) LegacyMemorySize;
    Regs.X.DX = (UINT16) Alignment;
    FarCall86 (
            This,
            gCoreBiosInfo->Csm16EntrySeg,
            gCoreBiosInfo->Csm16EntryOfs,
            &Regs,
            NULL,
            0
            );

    Status = (Regs.X.AX)? ((Regs.X.AX & 0x7FFF) | EFI_ERROR_BIT) : EFI_SUCCESS;

    *LegacyMemoryAddress  = (VOID *) (UINTN) ((Regs.X.DS << 4) + Regs.X.BX);

    gCoreBiosInfo->iRegion->Lock (
        gCoreBiosInfo->iRegion,
        (UINT32)gMaxOpRomAddress+1,
        0xFFFFF-(UINT32)gMaxOpRomAddress,
        &Granularity
    );

    return Status;
}


/**
    This function is called when copying data to the region assigned by
    EFI_LEGACY_BIOS_PROTOCOL.GetLegacyRegion().

    @param        This                Indicates the EFI_LEGACY_BIOS_PROTOCOL instance.
    @param        LegacyMemorySize    Size in bytes of the memory to copy.
    @param        LegacyMemoryAddress The location within the region returned by
        EFI_LEGACY_BIOS_PROTOCOL.GetLegacyRegion().
    @param        LegacyMemorySourceAddress       Source of the data to copy.

 
    @retval        EFI_SUCCESS         The data was copied successfully.
    @retval        EFI_ACCESS_DENIED   Either the starting or ending address is out of bounds.

**/

EFI_STATUS EFIAPI
CopyLegacyRegion (
  IN EFI_LEGACY_BIOS_PROTOCOL          *This,
  IN UINTN                             LegacyMemorySize,
  IN VOID                              *LegacyMemoryAddress,
  IN VOID                              *LegacyMemorySourceAddress
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32 Granularity;

    if (((UINTN)LegacyMemoryAddress <= gMaxOpRomAddress) ||
        ((UINTN) ((UINTN) (LegacyMemoryAddress) + LegacyMemorySize) > 0xFFFFF)) {
        return EFI_ACCESS_DENIED;
    }

    Status = gCoreBiosInfo->iRegion->UnLock (
        gCoreBiosInfo->iRegion,
        (UINT32)gMaxOpRomAddress+1,
        0xFFFFF-(UINT32)gMaxOpRomAddress,
        &Granularity
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    pBS->CopyMem(LegacyMemoryAddress, LegacyMemorySourceAddress, LegacyMemorySize);

    gCoreBiosInfo->iRegion->Lock (
        gCoreBiosInfo->iRegion,
        (UINT32)gMaxOpRomAddress+1,
        0xFFFFF-(UINT32)gMaxOpRomAddress,
        &Granularity
    );

    return Status;
}


/**
    This function is called when booting an EFI-aware OS with legacy hard disks.
    The legacy hard disks may or may not be the boot device but will be accessed
    by the EFI-aware OS.

    @param         This        Indicates the EFI_LEGACY_BIOS_PROTOCOL instance.
    @param         BbsCount    Number of BBS_TABLE structures.
    @param         BbsTable    BBS entry.

    @retval EFI_STATUS   Status of the boot preparation.

**/

EFI_STATUS EFIAPI
PrepareToBoot(
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  OUT  UINT16                         *BbsCount,
  OUT  BBS_TABLE              **BbsTable
)
{
    EFI_STATUS  Status;
    EFI_IA32_REGISTER_SET   RegSet;
    UINTN E820MapAddress;
    UINT32 *ivt = (UINT32*)0;
    SAVED_PCI_ROM   *rom;
    UINT32 LockUnlockAddr, LockUnlockSize;

    gCoreBiosInfo->NumberE820Entries = UpdateE820Map(gCoreBiosInfo->E820Map);

    Status = UpdateCsm16Configuration(&gCoreBiosInfo->Thunk->Csm16BootTable);
    ASSERT_EFI_ERROR(Status);

    //
    // Copy prepared ACPI, E820 tables to F000 segment and update EFI header.
    // Note that after executing CopyLegacyTable function E/F000 regions are read-only.
    //
    E820MapAddress = CopyLegacyTable(
                gCoreBiosInfo->E820Map,
                gCoreBiosInfo->NumberE820Entries * sizeof(E820_ENTRY),
                0,  // alignment
                0);	// either E000 or F000 segment
    ASSERT(E820MapAddress);

    //
    // Unlock the shadow regions necessary for OpROM execution
    //
    Status = UnlockShadow(
        (UINT8*)(UINTN)0xC0000,
        (UINTN)gNextRomAddress-0xC0000,
        &LockUnlockAddr,
        &LockUnlockSize
    );
    ASSERT_EFI_ERROR(Status);

    gCoreBiosInfo->Csm16Header->E820Pointer = (UINT32)E820MapAddress;
    gCoreBiosInfo->Csm16Header->E820Length = gCoreBiosInfo->NumberE820Entries * sizeof(E820_ENTRY);

    //
    // Restore saved OpROMs and EBDAs associated with them
    //
    for (rom=gSavedOprom-1; rom->Address; rom--) {
        pBS->CopyMem (
            rom->Address,
            rom->Data,
            ((LEGACY_OPT_ROM_HEADER*)rom->Data)->Size512 * 0x200
        );

        if (rom->rtDataAddr) {
            // Adjust rt data address according to the current EBDA location
            if (rom->isEbda) {
                rom->rtDataAddr = (UINT8*)(UINTN)((((UINT32)*(UINT16*)0x40e) << 4) + rom->ebdaOffset);
            }

            pBS->CopyMem (
                rom->rtDataAddr,
                rom->rtData,
                rom->rtDataSize
            );
        }
    }

    //
    // Reset number of ATA drives to let CSM16 PrepareToBoot function connect the drives
    //
    *(UINT8*)0x475 = 0;
    //
    // Make PrepareToBoot 16 bit call
    //
    pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.X.AX = Legacy16PrepareToBoot;
    RegSet.X.ES = EFI_SEGMENT (&gCoreBiosInfo->Thunk->Csm16BootTable);
    RegSet.X.BX = EFI_OFFSET (&gCoreBiosInfo->Thunk->Csm16BootTable);

    FarCall86 (&gCoreBiosInfo->iBios,
                gCoreBiosInfo->Csm16EntrySeg,
                gCoreBiosInfo->Csm16EntryOfs,
                &RegSet,
                NULL,
                0);

    Status = LockShadow(
            LockUnlockAddr,
            LockUnlockSize
    );
    ASSERT_EFI_ERROR(Status);

    if (RegSet.X.AX) Status = (RegSet.X.AX & 0x7FFF) | EFI_ERROR_BIT;

    if (EFI_ERROR(Status)) return Status;

    gIsBootPrepared = TRUE;

    *BbsCount = gCoreBiosInfo->BbsEntriesNo;
    *BbsTable = (BBS_TABLE*)gCoreBiosInfo->BbsTable;

    //
    // Reinitialize keyboard buffer tail and head
    //
	*(UINT16*)0x41A = 0x1E;
	*(UINT16*)0x41C = 0x1E;

    return EFI_SUCCESS;
}


/**
    Boots a traditional OS

    @param        This            Indicates the EFI_LEGACY_BIOS_PROTOCOL instance.
    @param        BootOption      The EFI device path from BootXXXX variable.
    @param        LoadOptionSize  Size of LoadOption.
    @param        LoadOption      The load option from BootXXXX variable.

    @retval       EFI_DEVICE_ERROR    Failed to boot from any legacy boot device

    @note  
  This function normally never returns. It will either boot the OS or
  reset the system if memory has been "corrupted" by loading a boot sector
  and passing control to it.

**/

EFI_STATUS EFIAPI
LegacyBoot (
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  IN  BBS_BBS_DEVICE_PATH             *BootOption,
  IN  UINT32                          LoadOptionsSize,
  IN  VOID                            *LoadOptions
)
{
    UINT16      BbsCount;
    BBS_TABLE   *BbsTable;
    EFI_EVENT   eLegacyBoot;
    EFI_STATUS  Status;
    EFI_IA32_REGISTER_SET   RegSet;
    UINT32      *ivt = (UINT32*)0;
    UINT16      i;
    EFI_TIME    boottime;
    UINT32      localtime;
    BDA_DATA    *bda;
    UINTN       Count;
    EFI_HANDLE  *Buffer;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *TxtOut;
	UINTN       Size;
    MEDIA_FW_VOL_FILEPATH_DEVICE_PATH   *MediaDevicePath=(MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *)BootOption;
    EFI_TIMER_ARCH_PROTOCOL *Timer;
    UINT8       Data;
    static EFI_GUID EfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE;
    EFI_EVENT   Event;

    PROGRESS_CODE(DXE_LEGACY_BOOT);

	//
	// Check if Secure Boot is on; if so, return EFI_SECURITY_VIOLATION
	//
    Size = sizeof(UINT8);
    Status = pRS->GetVariable(EFI_SECURE_BOOT_NAME, &EfiGlobalVariableGuid, NULL, &Size, &Data);
    if(!EFI_ERROR(Status) && (Data != 0)) {
        return EFI_SECURITY_VIOLATION;     // no CSM in secure boot mode
    }

    //
    // Clear all screens
    //
    Status = pBS->LocateHandleBuffer(ByProtocol,
                    &gEfiSimpleTextOutProtocolGuid,
                    NULL,
                    &Count,
                    &Buffer);

    if (!EFI_ERROR (Status)) {
        for (i = 0; i < Count; i++) {
            Status = pBS->HandleProtocol (Buffer[i], &gEfiSimpleTextOutProtocolGuid, &TxtOut);
            ASSERT_EFI_ERROR (Status);
            TxtOut->ClearScreen(TxtOut);
        }
    }

    bda = (BDA_DATA*)((UINTN) 0x400);
    if (bda->mode_of_vdo != 3) {
        RegSet.X.AX = 3;
        Status = gCoreBiosInfo->iBios.Int86 (&gCoreBiosInfo->iBios, 0x10, &RegSet);
        ASSERT_EFI_ERROR(Status);
    }

    //
    // Update local time data in BDA (dword at 40:6C and byte at 40:70)
    //
    pRS->GetTime (&boottime, NULL);
    localtime = (boottime.Hour * 3600 + boottime.Minute * 60 + boottime.Second) + 1;

    localtime = (localtime * 182)/10;   // Count number of timer ticks.

    bda->timer_count = localtime;
    bda->timer_count_rolled = 0;
    for (i=0; i<4; i++) bda->print_time_max[i] = 0x14;  // Maximum time for printing
    for (i=0; i<4; i++) bda->rs_232_time_max[i] = 0x1;  // Maximum transmission time for COM ports

    UpdateCmos();       // This function unconditionally enables NMI
    EnableDisableNmi();

    //
    // Restore saved interrupts
    //
    for(i = 0; gIrqSav[i].irq_adr; i++) {
        ivt[gIrqSav[i].irq_num] = gIrqSav[i].irq_adr;
    }

// APTIOV_SERVER_OVERRIDE_START : Enable/Disable legacy NIC per port bases
    Status = This->ShadowAllLegacyOproms(This);   // Launch remaining OpROMs
// APTIOV_SERVER_OVERRIDE_END : Enable/Disable legacy NIC per port bases
    ASSERT_EFI_ERROR(Status);

    // For the 1st boot allocate BBS table backup
    if (gBbsTableBackup == NULL){
        Status = pBS->AllocatePool(EfiBootServicesData, (sizeof(BBS_TABLE))*gCoreBiosInfo->BbsEntriesNo,
                               &gBbsTableBackup);
        ASSERT_EFI_ERROR(Status);
        
        // If BootNext is set, TSE sets the BBS priorities to BBS_UNPRIORITIZED_ENTRY.
        // Give unprioritized devices a priority to allow it to show up in INT13h.
        {
            #define BBS_LOWEST_ACTIVE_PRIORITY  0xfffb
            UINT16  LowestPriority = 0;

            // Locate the lowest priority
            for (i = 0; i < gCoreBiosInfo->BbsEntriesNo; i++) {
                if (gCoreBiosInfo->BbsTable[i].BootPriority > LowestPriority
                     && gCoreBiosInfo->BbsTable[i].BootPriority <= BBS_LOWEST_ACTIVE_PRIORITY) {
                    LowestPriority = gCoreBiosInfo->BbsTable[i].BootPriority;
                }
            }
    
            for (i = 0; i < gCoreBiosInfo->BbsEntriesNo; i++) {
                if (gCoreBiosInfo->BbsTable[i].BootPriority == BBS_UNPRIORITIZED_ENTRY) {
                    TRACE((-1, "Assigning unprioritized device %d priority %x\n", i, (LowestPriority+1)));
                    gCoreBiosInfo->BbsTable[i].BootPriority = ++LowestPriority;
                }
            }
        }
        pBS->CopyMem(gBbsTableBackup, gCoreBiosInfo->BbsTable, (sizeof(BBS_TABLE))*gCoreBiosInfo->BbsEntriesNo);
    }
    else
    {
        UINT32 HighestPriority = 0xFFFF;
        UINT16  AttemptedBootDevIndex = 0;
        UINT8  *Ebda = (UINT8*)(bda->ext_bios_data_seg << 4);

        // find the attempted boot drive which has the highest priority in the BBS table (set by TSE)
        for (i = 0; i < gCoreBiosInfo->BbsEntriesNo; i++)
        {
            if (gCoreBiosInfo->BbsTable[i].BootPriority < HighestPriority)
            {
                HighestPriority = gCoreBiosInfo->BbsTable[i].BootPriority;
                AttemptedBootDevIndex = i;
            }
        }
        
        // Adjust the AttemptedBootDrive and AttemptedIplBitmap to the proper value
        *((UINT16*)&Ebda[0x90]) = gBbsTableBackup[AttemptedBootDevIndex].BootPriority; // AttemptedBootDrive
        *((UINT32*)&Ebda[0xf8]) = 0; // Clear AttemptedIplBitmap lower 32bit
        *((UINT32*)&Ebda[0xfc]) = 0; // Clear AttemptedIplBitmap upper 32bit

        pBS->CopyMem(gCoreBiosInfo->BbsTable, gBbsTableBackup, (sizeof(BBS_TABLE))*gCoreBiosInfo->BbsEntriesNo);
    }

    DumpBbsTable(0);

    Status = EnablePciDevices(This);
    ASSERT_EFI_ERROR(Status);

    Status = pBS->LocateProtocol(&gEfiTimerArchProtocolGuid, NULL, &Timer);
    ASSERT_EFI_ERROR(Status);

    DisconnectSerialIO();
    
    //
    // Signal EFI_EVENT_SIGNAL_LEGACY_BOOT event
    //
    Status = CreateLegacyBootEvent (
        TPL_CALLBACK, NULL, NULL, &eLegacyBoot
    );

    if (Status == EFI_SUCCESS) {
        Status = pBS->SignalEvent(eLegacyBoot);
        ASSERT_EFI_ERROR(Status);
        pBS->CloseEvent(eLegacyBoot);
    }

    Timer->SetTimerPeriod (Timer, 0);   // Kill timer, mask its interrupt

    gCoreBiosInfo->iBiosPlatform->PrepareToBoot(
        gCoreBiosInfo->iBiosPlatform,
        BootOption,
        gCoreBiosInfo->BbsTable,
        LoadOptionsSize,
        LoadOptions,
        &gCoreBiosInfo->Thunk->Csm16BootTable
    );

    PrepareToBoot(This, &BbsCount, (BBS_TABLE**)&BbsTable);

    //
    // Lock the shadow permanently
    //
    gCoreBiosInfo->iRegion->BootLock (gCoreBiosInfo->iRegion,
        0xC0000, 0x40000, NULL);

    // APs need to be in HLT state on legacy boot. We used to have a call below to enforce APs to be in HLT.
    // Current CPU module already places APs in HLT. Also, executing this call would unnecessary generate CPU
    // events that will be reported as errors (this comes from CPU RC). We kept the call commented out; if it
    // is found that APs are in a different than HLT state, then the call below can be uncommented.
    // HaltApsBeforeBoot();

    SignalAmiLegacyBootEvent();

    //
    // Set NumLock state according to Setup question
    //
    i = (gSetup.Numlock)? 2 : 0;    // On:Off

    UpdateKeyboardLedStatus(This, (UINT8)i);
    						//(EIP95347+)>
    gCoreBiosInfo->i8259->GetMask(gCoreBiosInfo->i8259, NULL, NULL, &gProtectedIrqMask, NULL);      // Save current Mask
    						//<(EIP95347+)
    //
    // Restore redirected HW IRQ0..7
    //
    gOldTplValue = pBS->RaiseTPL(TPL_HIGH_LEVEL);  // disable interrupts

    //
    // Note: after this point TPL stays HIGH
    //
    for(i = 0; i < 8; i++) {
        ivt[gIrq0 + i] = gHwIrqRedirSav[i];
    }

    //
    // Program legacy vector base in 8259
    //
    gCoreBiosInfo->i8259->SetVectorBase (gCoreBiosInfo->i8259, 8, 0x70);

    if(guidcmp(&MediaDevicePath->FvFileName, &gWinCeGuid) == 0) {
        return EFI_NOT_FOUND;   // WinCE boot request, called from WinCE module
    }  
    //
    // Pass control to CSM16 to execute Int19
    //
    pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.X.AX = Legacy16Boot;

    FarCall86 (&gCoreBiosInfo->iBios,
                gCoreBiosInfo->Csm16EntrySeg,
                gCoreBiosInfo->Csm16EntryOfs,
                &RegSet,
                NULL,
                0);
                				//(EIP95347+)>
    pBS->RaiseTPL(TPL_HIGH_LEVEL);              // disable interrupts
    gCoreBiosInfo->i8259->SetVectorBase(gCoreBiosInfo->i8259, MASTER_INTERRUPT_BASE, SLAVE_INTERRUPT_BASE);
    gCoreBiosInfo->i8259->SetMask(gCoreBiosInfo->i8259, NULL , NULL, &gProtectedIrqMask, NULL);

    pBS->RestoreTPL(gOldTplValue);
    Timer->SetTimerPeriod (Timer, DEFAULT_TICK_DURATION);
 
    Status = pBS->CreateEventEx(
                 EVT_NOTIFY_SIGNAL,
                 TPL_CALLBACK,
                 DummyFunction,
                 NULL,
                 &gLTEBGuid,
                 &Event);

    pBS->SignalEvent(Event);
    pBS->CloseEvent(Event);
    
    
    if (gVgaHandle != NULL){
        pBS->DisconnectController(gVgaHandle, NULL, NULL);
        pBS->ConnectController(gVgaHandle, NULL, NULL, TRUE);
    }
    ConnectSerialIO();

    pBS->CopyMem(gCoreBiosInfo->BbsTable, gBbsTableBackup, (sizeof(BBS_TABLE))*gCoreBiosInfo->BbsEntriesNo);

    return EFI_SUCCESS;

}


/**
    Initialize the data areas for Chipset/OEM 16 bit and 32 bit routines and
    interrupt handlers.

    @retval EFI_STATUS Success of failure of the operation.

    @note  
  At the point of calling CSM16 is loaded, no CSM16 function is executed.
  E0000..FFFFF area is unlocked.

**/

EFI_STATUS
InitializePortingHooks (
    BIOS_INFO *CoreBiosInfo
)
{
    EFI_COMPATIBILITY16_TABLE   *Csm16;

    EFI_IA32_REGISTER_SET   Registers;
    EFI_STATUS              Status;
    VOID                    *OemData;
    UINTN                   OemDataSize;

    Csm16 = CoreBiosInfo->Csm16Header;

    //
    // Read OEM code/data image from the FFS. The following steps are to be taken:
    // 1) Copy it into E000/F000 area
    // 2) Initialize the appropriate pointers in CSM16: Csm16->OemIntSegment/Offset,
    //    Csm16->Oem32Segment/Offset, Csm16->Oem16Segment/Offset.
    //
	OemData = NULL;	// make ReadSection allocate the buffer
	Status = LoadFileSection(&gAmiOemCsm16BinaryGuid, &OemData, &OemDataSize);
    if (EFI_ERROR(Status)) return Status;

    pBS->SetMem(&Registers, sizeof (EFI_IA32_REGISTER_SET), 0);
    Registers.X.AX = Legacy16GetTableAddress;
    Registers.X.BX = E0000_BIT; // Allocate from 0xE0000 64 KB block
    Registers.X.CX = (UINT32)OemDataSize;
    Registers.X.DX = 1;         // Alignment

    FarCall86 (&CoreBiosInfo->iBios,
            CoreBiosInfo->Csm16EntrySeg, CoreBiosInfo->Csm16EntryOfs,
            &Registers, 0, 0);
    Status = (Registers.X.AX)? ((Registers.X.AX & 0x7FFF) | EFI_ERROR_BIT) : EFI_SUCCESS;
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    gCsm16IntStart = (OEMINT_FUNC*)(UINTN)(((UINTN)Registers.X.DS << 4) + Registers.X.BX);

    //
    // Copy the data to E000/F000
    //
    pBS->CopyMem((VOID*)gCsm16IntStart, OemData, OemDataSize);

    //
    // Initialize OEM data and update CSM16 data pointers
    //
    gCsm16FuncAddress = InitializeOemInt(&Csm16->OemIntSegment);
    gCsm32DataAddress = InitializeOem16(&Csm16->Oem16Segment);
//  InitializeOemData(&Csm16->Oem32Segment);

    return EFI_SUCCESS;
}


/**
    Initialize OEM interrupts and update CSM16 OEM int pointer

    @param        Csm16TableStart     The address of the beginning of OEM table
    @param        Csm16Address        The address of the beginning of OEM interrupt table handler(s)
    @param        Csm16InitSegPtr     The address of the field with OEM interrupt pointer that is
        passed to CSM16.

    @retval        Pointer to the next type of OEM code/data

**/

OEM16_FUNC*
InitializeOemInt(
    IN OUT UINT16   *Csm16InitSegPtr
)
{
    OEMINT_FUNC *IntFuncTable;

    UINT32  *ivt = (UINT32*)0;
    UINT32  *IntSavAddress;

    UINT16 Seg16 = (UINT16)((UINTN)gCsm16IntStart >> 4);

    //
    // Update Csm16 data pointer
    //
    *Csm16InitSegPtr = Seg16;
    *(Csm16InitSegPtr+1) = 0;

    //
    // Initialize interrupt handlers with the interrupt original locations
    //
    for (IntFuncTable=gCsm16IntStart;IntFuncTable->interrupt!=0xFFFF;IntFuncTable++) {
        IntSavAddress = (UINT32*)((UINTN)gCsm16IntStart+IntFuncTable->funcoffset-sizeof(UINT32));
        // interrupt vector handshake
        *IntSavAddress = ivt[IntFuncTable->interrupt];
        ivt[IntFuncTable->interrupt] = ((UINT32)Seg16 << 16) + IntFuncTable->funcoffset;
    }

    //
    // Return the pointer to the OEM16 function table that immediately follows OEMINT table
    //
    return (OEM16_FUNC*)(++IntFuncTable);
}


/**
    Initialize OEM interrupts and update CSM16 OEM int pointer

    @param         Csm16TableStart     The address of the beginning of OEM table
    @param         Csm16Address        The address of the beginning of OEM16 data/code
    @param         Csm16InitSegPtr     The address of the field with OEM16 pointer
        that is passed to CSM16.

    @retval         Pointer to the next type of OEM code/data

**/

UINTN
InitializeOem16(
    IN OUT UINT16   *Csm16InitSegPtr
)
{
    OEM16_FUNC  *Oem16FuncTable;

    UINT16 Seg16 = (UINT16)(((UINTN)gCsm16FuncAddress >> 4) & 0xf000);
    UINT16 Ofs16 = (UINT16)((UINTN)gCsm16FuncAddress & 0xffff);

    //
    // Update Csm16 data pointer
    //
    *Csm16InitSegPtr = Seg16;
    *(Csm16InitSegPtr+1) = Ofs16;

 	// Find the end of Oem16 function table
    for (Oem16FuncTable = gCsm16FuncAddress;
            Oem16FuncTable->funcID != 0xffff; Oem16FuncTable++) {}

    //
    // Update the OEM16 segment to SEG so that the 1st routine has an
    // entry point at SEG:0. Store this SEG value at gCsm16FuncAddress-2
    // This will make all offsets in OEM16_CSEG valid.
    //
    *(UINT16*)((UINTN)gCsm16FuncAddress-2) = (UINT16)((UINTN)gCsm16IntStart >> 4);

    //
    // Return the pointer to the OEM16 function table that immediately follows OEMINT table
    //
    return (UINTN)(++Oem16FuncTable);
}


/**
    LegacyBiosExt API, returns the entry point of 16-bit function which was
    added using "CsmOem16Functions" eLink

    @param   id          function ID

    @retval  EFI_SUCCESS if found, Addr is updated with 32-bit pointer to the function
    @retval  EFI_NOT_FOUND if function with the requested ID is not found

**/

EFI_STATUS
EFIAPI
Get16BitFuncAddress (
    IN UINT16 id,
    OUT UINT32 *Addr
)
{
    OEM16_FUNC  *Oem16FuncTable;
    UINT16      Seg16;

    if (gCsm16FuncAddress == NULL) return EFI_NOT_FOUND;

    Oem16FuncTable = gCsm16FuncAddress;
    Seg16 = *(UINT16*)((UINTN)gCsm16FuncAddress-2);

    for (; Oem16FuncTable->funcID != 0xffff; Oem16FuncTable++) {
        if (Oem16FuncTable->funcID == id) {
            *Addr = ((UINT32)Seg16<<4)+(UINT32)Oem16FuncTable->funcOffset;
            return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}


/**
    This call is halting APs before giving control to Legacy OS.

**/

VOID
HaltApsBeforeBoot()
{
    UINTN NumberOfCPUs;
    EFI_STATUS Status;
    EFI_MP_SERVICES_PROTOCOL *MpServices;

    // Execute on running APs
    Status = pBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return;

// Use the #else path for the CPU modules compliant with PI1.1
#if PI_SPECIFICATION_VERSION < 0x0001000A || BACKWARD_COMPATIBLE_MODE && defined(NO_PI_MP_SERVICES_SUPPORT)
    MpServices->GetGeneralMPInfo(MpServices,
        &NumberOfCPUs, NULL, NULL, NULL, NULL); 
#else
{
    UINTN NumberOfEnabledProcessors;

    MpServices->GetNumberOfProcessors(MpServices,
        &NumberOfCPUs, &NumberOfEnabledProcessors);
}
#endif

    NumberOfCPUs--;  //Exclude BSP
    while(NumberOfCPUs) {
        MpServices->EnableDisableAP(
            MpServices,
            NumberOfCPUs,
            FALSE,
            NULL);
        NumberOfCPUs--;
    }	
}


/**
    Policy to load CSM

    @param  ImageHandle		CSM image handle

         
    @retval TRUE OK to load CSM
    @retval FALSE - CSM must not be loaded

**/

BOOLEAN CsmIsTobeLoaded(
	IN EFI_HANDLE ImageHandle
)
{
    EFI_STATUS Status;
    EFI_HANDLE Handle = NULL;
    SETUP_DATA SetupData;
    UINTN Size = sizeof(SETUP_DATA);
    UINT8 Data;
	static EFI_GUID EfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE_GUID;
	static EFI_GUID EfiSetupGuid = SETUP_GUID;

	// First check if we're in secure boot mode
    Size = sizeof(UINT8);
    Status = pRS->GetVariable(EFI_SECURE_BOOT_NAME, &EfiGlobalVariableGuid, NULL, &Size, &Data);
    if(!EFI_ERROR(Status) && Data)
    {
        UINT32 Attributes;
        
        Size = sizeof(SETUP_DATA);
        Status = pRS->GetVariable(L"Setup", &EfiSetupGuid, &Attributes, &Size, &SetupData);
        if (!EFI_ERROR(Status))
        {
            SetupData.CsmSupport = 0;   // Disable CSM in setup
            pRS->SetVariable(L"Setup", &EfiSetupGuid, Attributes, Size, &SetupData);
        }
        return FALSE;     // no CSM in secure boot mode
    }

    Size = sizeof(SETUP_DATA);
    Status = pRS->GetVariable(L"Setup", &EfiSetupGuid, NULL, &Size, &SetupData);

    Data = (EFI_ERROR(Status)) ? DEFAULT_CSM_LAUNCH_POLICY : SetupData.CsmSupport;

    switch(Data) {
        case 0:
            return FALSE;		//do not load CSM

        case 1:
            return TRUE;    	//load CSM
            
        default:
            return TRUE;		//load CSM
    }
}

VOID DumpBbsTable(UINT32 CaseNumber)
{
#ifdef EFI_DEBUG
    BBS_TABLE   *BbsEntry;
    EFI_HANDLE  Handle;
    EFI_GUID    DevicePathProtocolGuid = EFI_DEVICE_PATH_PROTOCOL_GUID;
    EFI_DEVICE_PATH_PROTOCOL    *dpp;
    UINT32      i;
    EFI_STATUS  Status;

    TRACE((-1,"_____BBS_TABLE(%x)_____\n", CaseNumber));
    for (BbsEntry = gCoreBiosInfo->BbsTable, i = 0; i < MAX_BBS_ENTRIES_NO; i++, BbsEntry++) {
        if (BbsEntry->BootPriority == BBS_IGNORE_ENTRY) continue;
        TRACE((-1, "#%02d: %d %02X/%02X/%02X %x %x %x %04X handle: %x %x, %s\n", i, BbsEntry->BootPriority,
                    (BbsEntry->Bus==0xffffffff)? 0xff : BbsEntry->Bus,
                    (BbsEntry->Device==0xffffffff)? 0xff : BbsEntry->Device,
                    (BbsEntry->Function==0xffffffff)? 0xff : BbsEntry->Function,
                    BbsEntry->Class, BbsEntry->SubClass,
                    BbsEntry->DeviceType, BbsEntry->StatusFlags,
                    BbsEntry->IBV1, BbsEntry->IBV2,
                    (UINTN)((UINTN)BbsEntry->DescStringSegment<<4) + BbsEntry->DescStringOffset
        ));
        Handle = *(VOID**)(&BbsEntry->IBV1);
        if (Handle == 0) continue;
        Status = pBS->HandleProtocol(Handle, &DevicePathProtocolGuid, &dpp);
        TRACE((-1, "Handle=%x, HandleProtocol status: %r, DevicePath: %x\n", Handle, Status, dpp));
    }
#endif
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
