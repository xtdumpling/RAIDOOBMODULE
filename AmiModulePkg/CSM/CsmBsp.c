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

//**********************************************************************
/** @file CsmBsp.c
    Compatibility Support Module - Board Support Package.
    This driver implements LEGACY_BIOS_PLATFORM_PTOTOCOL interface functions.

**/

#include "Efi.h"
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include "Csm.h"
#include <AmiDxeLib.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include "Token.h"
#include "Setup.h"
#include "CsmElinkFunctions.h"
#include "Pci.h"

extern AMI_CSM_GET_OPROM_VIDEO_SWITCHING_MODE CSM_GET_OPROM_VIDEO_SWITCHING_MODE_FUNCTIONS EndOfGetOpRomVideoModeFunctions;
AMI_CSM_GET_OPROM_VIDEO_SWITCHING_MODE *CsmGetOpRomVideoModeSwitchingFunctions[] = { CSM_GET_OPROM_VIDEO_SWITCHING_MODE_FUNCTIONS NULL };

extern AMI_CSM_GET_CUSTOM_PCI_PIRQ_MASK CSM_GET_CUSTOM_PCI_PIRQ_MASK_FUNCTIONS EndOfGetCustomPciPirqMaskFunctions;
AMI_CSM_GET_CUSTOM_PCI_PIRQ_MASK *CsmGetCustomPciPirqMaskFunctions[] = { CSM_GET_CUSTOM_PCI_PIRQ_MASK_FUNCTIONS NULL };

extern AMI_CSM_GET_GATE_A20_INFORMATION CSM_GET_GATE_A20_INFORMATION_FUNCTIONS EndOfGetGateA20InformationFunctions;
AMI_CSM_GET_GATE_A20_INFORMATION *CsmGetGateA20InformationFunctions[] = { CSM_GET_GATE_A20_INFORMATION_FUNCTIONS NULL };

extern AMI_CSM_GET_NMI_INFORMATION CSM_GET_NMI_INFORMATION_FUNCTIONS EndOfGetNmiInformationFunctions;
AMI_CSM_GET_NMI_INFORMATION *CsmGetNmiInformationFunctions[] = { CSM_GET_NMI_INFORMATION_FUNCTIONS NULL };

// Get Platform Handle ELINK function list
extern AMI_CSM_GET_PLATFORM_HANDLE CSM_GET_PLATFORM_HANDLE_FUNCTIONS EndOfPlatformHandleFunctions;
AMI_CSM_GET_PLATFORM_HANDLE *CsmGetPlatformFunctions[] = { CSM_GET_PLATFORM_HANDLE_FUNCTIONS NULL };

extern AMI_CSM_GET_PLATFORM_EMBEDDED_ROM CSM_GET_PLATFORM_EMBEDDED_ROM_FUNCTIONS EndOfGetPlatformEmbeddedRomFunctions;
AMI_CSM_GET_PLATFORM_EMBEDDED_ROM *CsmGetPlatformEmbeddedRomFunctions[] = { CSM_GET_PLATFORM_EMBEDDED_ROM_FUNCTIONS NULL };

extern AMI_CSM_GET_PLATFORM_PCI_EMBEDDED_ROM CSM_GET_PLATFORM_PCI_EMBEDDED_ROM_FUNCTIONS EndOfGetPlatformPciEmbeddedRomFunctions;
AMI_CSM_GET_PLATFORM_PCI_EMBEDDED_ROM *CsmGetPlatformPciEmbeddedRomFunctions[] = { CSM_GET_PLATFORM_PCI_EMBEDDED_ROM_FUNCTIONS NULL };

extern AMI_CSM_CHECK_OEM_PCI_SIBLINGS CSM_CHECK_OEM_PCI_SIBLINGS_FUNCTIONS EndOfCheckOemPciSiblingsFunctions;
AMI_CSM_CHECK_OEM_PCI_SIBLINGS *CsmCheckOemPciSiblingsFunctions[] = { CSM_CHECK_OEM_PCI_SIBLINGS_FUNCTIONS NULL };

extern AMI_CSM_ENABLE_OEM_PCI_SIBLINGS CSM_ENABLE_OEM_PCI_SIBLINGS_FUNCTIONS EndOfEnableOemPciSiblingsFunctions;
AMI_CSM_ENABLE_OEM_PCI_SIBLINGS *CsmEnableOemPciSiblingsFunctions[] = { CSM_ENABLE_OEM_PCI_SIBLINGS_FUNCTIONS NULL };

extern AMI_CSM_16_CALL_COMPANION CSM_16_CALL_COMPANION_FUNCTIONS EndOfCsm16CompanionFunctions;
AMI_CSM_16_CALL_COMPANION *Csm16CallCompanionFunctions[] = { CSM_16_CALL_COMPANION_FUNCTIONS NULL };
 
extern AMI_CSM_GET_ROUTING_TABLE CSM_GET_ROUTING_TABLE_FUNCTIONS EndOfGetRoutingTableFunctions;
AMI_CSM_GET_ROUTING_TABLE *CsmGetRoutingTableFunctions[] = { CSM_GET_ROUTING_TABLE_FUNCTIONS NULL };

extern AMI_CSM_BSP_UPDATE_PRT CSM_BSP_UPDATE_PRT_FUNCTIONS EndOfBspUpdatePrtFunctions;
AMI_CSM_BSP_UPDATE_PRT *CsmBspUpdatePrtFunctions[] = { CSM_BSP_UPDATE_PRT_FUNCTIONS NULL };

extern AMI_CSM_BSP_PREPARE_TO_BOOT CSM_BSP_PREPARE_TO_BOOT_FUNCTIONS EndOfBspPrepareToBootFunctions;
AMI_CSM_BSP_PREPARE_TO_BOOT *CsmBspPrepareToBootFunctions[] = { CSM_BSP_PREPARE_TO_BOOT_FUNCTIONS NULL };


// end of CSM elink functions


PLATFORM_BIOS_INFO  *gBspBiosInfo;


extern EFI_LEGACY_PIRQ_TABLE_HEADER *gMsPrt;

//AMIIRQ_HEADER                 *AmiPrt;
extern EFI_LEGACY_IRQ_ROUTING_ENTRY *gPrt;
BOOLEAN                         gIsValidPrt;

EFI_STATUS InitPrt(EFI_LEGACY_BIOS_PLATFORM_PROTOCOL*);
UINT8   GetRBSecondaryBusNum(EFI_HANDLE);
UINT8   GetP2PSecondaryBusNum(EFI_HANDLE, UINT8, UINT8);
EFI_STATUS Intx2Pirq (EFI_LEGACY_BIOS_PLATFORM_PROTOCOL*,
                UINTN, UINTN, UINTN, UINT8*, UINT8*, UINT16*);

extern VOID *gEmbeddedRom;
extern UINTN gEmbeddedRomSize;
extern UINT16 gCurrentIrqMask;
extern UINTN    gUsedPciEntries;
extern SETUP_DATA   gSetup;
extern EFI_HANDLE gVgaHandle;
//
// The following table has a list of interrupts to be saved after
// CSM Init call and restored before LegacyBoot. The default list
// is defined in CSM.SDL and contains three interrups: int13, int15,
// and int40.
// This list can be expanded according to the project needs. Only
// MAX_NUM_IRQS_SAVERESTORE entries will be processed, others will
// be ignored.
//
UINT8 gInterruptsToPreserve[] = INTERRUPTS_TO_PRESERVE;

//
// The following table is the list of VID/DID (combined into DWORDs)
// that will have problems with BCV executed during early POST right
// after Option ROM is initialized, or problems with BCV executed
// multiple times during POST. The examples of such "problematic"
// BCVs would be trapping BIOS interrupts that are not BCV specific,
// allocating blocks of memory, etc.
// Note that for these devices BCV will be executed during PrepareToBoot;
// drivers that require BCV execution before that (CsmBlockIo) will not
// support these devices.
//
UINT32 gSkipBcvDeviceList[] = SKIP_EARLY_BCV_DEVICES;


/**
    This function is called from LEGACY_BIOS.InstallPciRom()
    before and after OpROM execution. It allows OEM override the
    default video mode switching policy during OpROM execution.

    @param PciIo               PciIo protocol of the device associated with the OpROM;
        This is an optional field, if NULL then it is ignored.

    @param TextSwitchingMode   The current text switching mode. Possible values are:
        0 - switching to text mode is needed;
        1 - switching is needed, restoration is not;
        2 - neither switching nor restoration is needed.
    @retval SetTxtSwitchingMode The mode that will be used for this OpROM. The valid
        values are 0, 1, and 2. The meaning is the same as
        TextSwitchingMode (above).

    @retval EFI_SUCCESS         The value of SetTxtSwithcingMode is updated according
        to the platform policy and will be used for video mode
        switching for the given PciIo's Option ROM.

    @retval EFI_UNSUPPORTED     The value of SetTxtSwitchingMode is not valid.


    @note  This function can change the policy of video switching for selected
          Option ROMs by looking up the PCI VID/DID. It also can change this
          policy for all OpROMs.

          Use SetTxtSwitchingMode value to adjust the platform policy. For
          example, value equal to 1 probably indicates the end of the boot
          process, when system is already in text mode and switching back
          is not desired. In this case platform may choose not to interfere
          with the default policy. This can be done by either returning
          EFI_UNSUPPORTED or returning EFI_SUCCESS with SetTxtSwitchingMode
          equal to TextSwitchingMode.

          Returning EFI_SUCCESS must be carefully considered. If function
          returns EFI_SUCCESS, please make sure SetTxtSwitchingMode is
          assigned the valid values: 0, 1, or 2. If the wrong value is
          assigned, then video switching policy will fail.

          The default implementation simply returns EFI_SUCCESS with the
          SetTxtSwitchingMode set to the value passed in TextSwitchingMode.

**/

EFI_STATUS
GetOpromVideoSwitchingMode(
    IN EFI_PCI_IO_PROTOCOL  *PciIo  OPTIONAL,
    IN UINT16               TextSwitchingMode,
    OUT UINTN               *SetTxtSwitchingMode
)
{
/*
// Here is an example of the platform implementation of this function.
    UINT32      VidDid;
    EFI_STATUS  Status;

    if (TextSwitchingMode == 1) return EFI_UNSUPPORTED; // see the notes

    if (PciIo == NULL) return EFI_UNSUPPORTED;

    //
    // Force mode switching for certaing PCI cards
    //
    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32,
        0,  // offset
        1,  // width
        &VidDid);
    ASSERT_EFI_ERROR(Status);

    if (VidDid == 0x12345678) {
        *SetTxtSwitchingMode = 0;
        return EFI_SUCCESS;
    }
    return EFI_UNSUPPORTED;
*/

    EFI_STATUS Status;
    UINTN i = 0;

    // default value
    *SetTxtSwitchingMode = (UINTN)TextSwitchingMode;

    for(i = 0; CsmGetOpRomVideoModeSwitchingFunctions[i] != NULL; i++)
    {
        Status = CsmGetOpRomVideoModeSwitchingFunctions[i](PciIo, TextSwitchingMode, SetTxtSwitchingMode);
        if(!EFI_ERROR(Status)) break;
    }

    // Enforce headless operation if VGA handle is not available
    //
    if (gVgaHandle == NULL)
    {
        BOOLEAN IsVga = FALSE;

        if (PciIo != NULL)
        {
            UINT8 PciCfgData[4];
            EFI_STATUS Status;
            
            Status = PciIo->Pci.Read(
                PciIo,
                EfiPciIoWidthUint8,
                8,   // offset
                4,   // width
                PciCfgData);
            ASSERT_EFI_ERROR(Status);
    
            if (PciCfgData[3]==PCI_CL_OLD && PciCfgData[2]==PCI_CL_OLD_SCL_VGA) {
                IsVga = TRUE;
            }
            if (PciCfgData[3]==PCI_CL_DISPLAY && PciCfgData[2]==PCI_CL_DISPLAY_SCL_VGA) {
                IsVga = TRUE;
            }
        }
        if (!IsVga)
            *SetTxtSwitchingMode = 2;
    }
    
    // Prevent video mode switching when UEFI video is selected
    //
    {
        if (gVgaHandle != NULL)
        {
            EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *Entries;
            UINTN Count;
            EFI_HANDLE AgentHandle;
            VOID *Interface;
            UINTN i;

            Status = pBS->OpenProtocolInformation(gVgaHandle, &gEfiPciIoProtocolGuid, &Entries, &Count);
            ASSERT_EFI_ERROR(Status);

            for(i = 0; i < Count; i++) {
                if(!(Entries[i].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER))
                    continue;

                Status = pBS->HandleProtocol(Entries[i].ControllerHandle, &gEfiGraphicsOutputProtocolGuid, &Interface);
                if(!EFI_ERROR(Status)) {
                    AgentHandle = Entries[i].AgentHandle;
                    break;
                }
            }
            pBS->FreePool(Entries);

            if(i < Count) {
                // check if AgentHandle is CSM thunk
                Status = pBS->HandleProtocol(AgentHandle, &gAmiCsmThunkDriverGuid, &Interface);
                if (EFI_ERROR(Status)) {
                    TRACE((-1, "UEFI Video driver is connected: suppress video switching for legacy Option ROM.\n"));
                    *SetTxtSwitchingMode = 2;
                }
                
            }
        }
    }
        
    return EFI_SUCCESS;
}


/**
    This function is called from PciInterrupts.ProgramPciIrq()
    before programming PCI register 3C. This function is given
    the PciIo of the device and a list of possible IRQ choices
    for this device.
    The possible IRQs are given in a form of UINT16 IRQ mask,
    where 1 is a masked IRQ level.
    This function can modify this list and return EFI_SUCCESS.
    A caller then will AND the existing mask with the one returned
    by this function. Note that this way the existing list can
    only be shrunk, not extended.

    @param PciIo       PciIo protocol of the device

    @param IrqMask     The current bit mask of IRQ levels available for this device

    @retval ModifiedIrqMask     The value that a caller will use to OR with the existing mask.

    @retval EFI_SUCCESS         Function is returning a valid ModifiedIrqMask
    @retval EFI_UNSUPPORTED     Function is not ported for this PciIo, value returned
        in ModifiedIrqMask will be ignored.

**/

EFI_STATUS
GetCustomPciIrqMask (
    IN EFI_PCI_IO_PROTOCOL  *PciIo,
    IN UINT16               IrqMask,
    OUT UINTN               *ModifiedIrqMask
)
{
/* Implementation example - force IRQ11 for device 0x12345678

    UINT32      VidDid;
    EFI_STATUS  Status;

    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32,
        0,  // offset
        1,  // width
        &VidDid);
    ASSERT_EFI_ERROR(Status);

    if (VidDid == 0x12345678) {
        if (IrqMask & 0xF7FF) {
            *ModifiedIrqMask = 0xF7FF;  // leave only IRQ11 unmaksed
            return EFI_SUCCESS;
        }
    }

    return EFI_UNSUPPORTED;
*/

    EFI_STATUS Status = EFI_UNSUPPORTED;
    UINTN i = 0;

    // Set a default value
    *ModifiedIrqMask = IrqMask;

    for(i = 0; CsmGetCustomPciPirqMaskFunctions[i] != NULL; i++)
    {
        Status = CsmGetCustomPciPirqMaskFunctions[i](PciIo, IrqMask, ModifiedIrqMask);
        if(!EFI_ERROR(Status)) break;
    }

    return Status;
}


/**
    This function is called from LegacyBios entry point and it
    returns the board specific information related to the way of
    controlling GateA20. Two different aspects of GateA20 control
    can be returned by this function:
    1) Line GA20 can be activated/deactivated through KBC's GPIO
    (command D1), or through Port92 (Bit2).
    2) Deactivation of GA20 is be desirable at all, any CSM16
    function related to deactivation will be skipped.

    @retval     GateA20Info:
        BIT0    0 - GA20 controlled through KBC
        BIT0    1 GA20 controlled through Port92
        BIT1    0 - Do not skip GA20 deactivation
        BIT1    1 Skip GA20 deactivation
    @retval       EFI_SUCCESS         GateA20Info has valid bits 0 and 1
    @retval       EFI_UNSUPPORTED     Function is not supported, GateA20Info information
        is invalid; CSM core will use the default GA20 policy.

    @note  Skipping GA20 deactivation is needed when APs are running above
              1MB; if GA20 is inactive, then any access >1MB may be invalid.
              At the same time, keeping GA20 active all the time may fail the
              applications that test GA20 enable/disable functionality. It may
              also affect some applications that rely on GA20 being inactive
              after legacy boot.

**/

EFI_STATUS
GetGateA20Information(
    OUT UINTN   *GateA20Info
)
{
    EFI_STATUS Status;
    UINTN i = 0;

    // Set BIT0 if using Port92, Set BIT1 to skip GA20 deactivation
    if (gSetup.KeepGA20Active == 1) *GateA20Info = 3;
    else *GateA20Info = 1;

    for(i = 0; CsmGetGateA20InformationFunctions[i] != NULL; i++)
    {
        Status = CsmGetGateA20InformationFunctions[i](GateA20Info);
        if(!EFI_ERROR(Status)) break;
    }

    return EFI_SUCCESS;
}


/**
    This function is called from LegacyBios entry point and it returns the board
    specific information related to NMI.

        
    @retval        NMI_information:
        BIT0    0 - NMI should be kept disabled
        BIT0    1 - enable NMI
    @retval        EFI_SUCCESS         GateA20Info has valid bits 0 and 1
    @retval        EFI_UNSUPPORTED     Function is not supported, NMI information is invalid; CSM
        will use the default NMI policy.

**/

EFI_STATUS
GetNmiInformation(
    OUT UINTN   *NmiInfo
)
{
    EFI_STATUS Status;
    UINTN i = 0;

    // prime the return value to enabled
    *NmiInfo = 1;

    for(i = 0; CsmGetNmiInformationFunctions[i] != NULL; i++)
    {
        Status = CsmGetNmiInformationFunctions[i](NmiInfo);
        if(!EFI_ERROR(Status)) break;
    }

    return EFI_SUCCESS;
}



/**
    Finds the binary data or other platform information.
    Refer to the subfunctions for additional information.

    @param         This    Indicates the EFI_LEGACY_BIOS_PLATFORM_PROTOCOL instance.
    @param         Mode    Specifies what data to return:
        EfiGetPlatformBinaryMpTable
        EfiGetPlatformBinaryOemIntData
        EfiGetPlatformBinaryOem16Data
        EfiGetPlatformBinaryOem32Data
        EfiGetPlatformBinaryTpmBinary
        EfiGetPlatformBinarySystemRom
        EfiGetPlatformPciExpressBase
        EfiGetPlatformPmmSize
        EfiGetPlatformEndOpromShadowAddr

    @param         Table       Pointer to OEM legacy 16-bit code or data.
    @param         TableSize   Size of data.
    @param         Location    Location to place table.
        Bit 0 = 1 0xF0000 64 KB block.
        Bit 1 = 1 0xE0000 64 KB block.
    Note that multiple bits can be set.
    @param   Alignment   Bit-mapped address alignment granularity. The first 
                  nonzero bit from the right is the address granularity.
    @param   LegacySegment   Segment where EfiCompatibility code will place the 
                  table or data.
    @param   LegacyOffset    Offset where EfiCompatibility code will place the 
                  table or data.

    @retval         EFI_SUCCESS     The data was returned successfully.
    @retval         EFI_UNSUPPORTED Mode is not supported on this platform.
    @retval         EFI_NOT_FOUND   Binary image not found.

**/

EFI_STATUS
GetPlatformInfo (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           *This,
  IN  EFI_GET_PLATFORM_INFO_MODE                  Mode,
  IN OUT VOID                                     **Table,
  IN OUT UINTN                                    *TableSize,
  IN OUT UINTN                                    *Location,
  OUT UINTN                                       *Alignment,
  IN UINT16                                       LegacySegment,
  IN UINT16                                       LegacyOffset
  )
{
    switch (Mode) {
        case EfiGetPlatformPmmSize:
            //
            // Return PMM area size below 1MB to be allocated in bytes.
            // This size should be a multiple of 32K
            //
            *TableSize = PMM_LOMEM_SIZE;
            //
            // Return PMM area size above 1MB to be allocated in bytes.
            // This size should be a multiple of 64k.
            // Note: This field is not bit-mapped for this function.
            //
            *Alignment = PMM_HIMEM_SIZE;
            return EFI_SUCCESS;

        case EfiGetPlatformEndOpromShadowAddr:
            *Location = OPROM_MAX_ADDRESS;
            return EFI_SUCCESS;

        case EfiGetPlatformBinarySystemRom:
            return FindEmbeddedRom(CSM16_MODULEID, CSM16_VENDORID, CSM16_DEVICEID,
                    Table, TableSize);

        case EfiGetPlatformPciExpressBase:
            *(UINT64*)Location =  PcdGet64 (PcdPciExpressBaseAddress);
            return EFI_SUCCESS;

    }

    return EFI_UNSUPPORTED;
}


/**
    The AMI extension of GetPlatformInfo.

**/

EFI_STATUS
EFIAPI
GetPlatformInformationEx (
  IN  EFI_LEGACY_BIOS_EXT_PROTOCOL      *This,
  IN  EFI_GET_PLATFORM_INFO_EXT_MODE    Mode,
  IN OUT VOID                           **Table,
  IN OUT UINTN                          *TableSize,
  IN OUT UINTN                          *Location,
  OUT UINTN                             *Alignment,
  IN UINT16                             LegacySegment,
  IN UINT16                             LegacyOffset
  )
{
    switch (Mode) {
        case EfiGetPlatformOpromVideoMode:
            return GetOpromVideoSwitchingMode((EFI_PCI_IO_PROTOCOL*)(*Table),
                    LegacySegment, TableSize);

         case EfiGetPlatformIntSaveRestoreTable:
            *Table = gInterruptsToPreserve;
            *TableSize = sizeof(gInterruptsToPreserve);
            return EFI_SUCCESS;

        case EfiGetPlatformPciIrqMask:
            return GetCustomPciIrqMask((EFI_PCI_IO_PROTOCOL*)(*Table),
                LegacySegment, Alignment);

        case EfiGetPlatformGateA20Information:
            return GetGateA20Information(Alignment);

        case EfiGetPlatformNmiInformation:
            return GetNmiInformation(Alignment);

        case EfiGetBcvSkipDeviceList:
            *Table = gSkipBcvDeviceList;
            *TableSize = sizeof(gSkipBcvDeviceList)/4;
            return EFI_SUCCESS;
    }

    return EFI_UNSUPPORTED;
}


/**
    Returns a buffer of handles for the requested subfunction.

    @param        This            Indicates the EFI_LEGACY_BIOS_PLATFORM_PROTOCOL instance.
    @param        Mode            Specifies what handle to return.
        GetVgaHandle
        GetIdeHandle
        GetIsaBusHandle
        GetUsbHandle
    @param        Type            Handle Modifier - Mode specific
    @param        HandleBuffer    Pointer to buffer containing all Handles matching the 
        specified criteria. Handles are sorted in priority order.
        Type EFI_HANDLE is defined in InstallProtocolInterface()
        in the EFI 1.10 Specification.
    @param        HandleCount     Number of handles in HandleBuffer.
    @param        AdditionalData  Pointer to additional data returned - mode specific..

    @retval         EFI_SUCCESS The handle is valid.
    @retval         EFI_UNSUPPORTED Mode is not supported on this platform.
    @retval         EFI_NOT_FOUND   The handle is not known.

**/

EFI_STATUS
GetPlatformHandle (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL     *This,
  IN  EFI_GET_PLATFORM_HANDLE_MODE          Mode,
  IN  UINT16                                Type,
  OUT EFI_HANDLE                            **HandleBuffer,
  OUT UINTN                                 *HandleCount,
  OUT VOID                                  OPTIONAL **AdditionalData
  )
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    UINT16 i = 0;

    for(i = 0; CsmGetPlatformFunctions[i] != NULL; i++)
    {
        // Call all ELINK functions until one returns EFI_SUCCESS
        Status = CsmGetPlatformFunctions[i](This, Mode, Type, HandleBuffer, HandleCount, AdditionalData);
        if(!EFI_ERROR(Status)) break;
    }
    return Status;
}


/**
    This function is called from CheckPciRom and allows OEM to
    have a custom code that looks for a binary. 

    @note
	Input parameters are same as in FindEmbeddedRom function.

    @retval EFI_UNSUPPORTED Function is not implemented
    @retval EFI_NOT_FOUND No ROM found
    @retval EFI_SUCCESS ROM is found, output is the same as
        in FindEmbeddedRom function.
**/

EFI_STATUS
GetPlatformPciEmbeddedRom (
    IN EFI_PCI_IO_PROTOCOL  *PciIo,
	OUT VOID    **PciRom,
    OUT UINTN   *PciRomSize
)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    UINTN i = 0;

    for(i = 0; CsmGetPlatformPciEmbeddedRomFunctions[i] != NULL; i++)
    {
        Status = CsmGetPlatformPciEmbeddedRomFunctions[i](PciIo, PciRom, PciRomSize);
        if(!EFI_ERROR(Status)) break;
    }
    return Status;
}


/**
    This function is called from FindEmbeddedRom and allows OEM to have a custom
    code that looks for a binary. By default all addon ROMs are packed as BINFILE
    section. This function may be needed if binary is packed in a way that is
    different from default (for example it can be FFS file, etc.).

    @note
	Input parameters are same as in FindEmbeddedRom function.

    @retval EFI_NOT_FOUND   No ROM found, or function is not implemented.
    @retval EFI_SUCCESS     ROM is found, output is the same as in FindEmbeddedRom
        function.
**/

EFI_STATUS GetPlatformEmbeddedRom(
    UINT16 ModuleId, UINT16 VendorId, UINT16 DeviceId, 
    VOID **ImageStart, UINTN *ImageSize)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    UINTN i = 0;

    for(i = 0; CsmGetPlatformEmbeddedRomFunctions[i] != NULL; i++)
    {
        Status = CsmGetPlatformEmbeddedRomFunctions[i](ModuleId, VendorId, DeviceId, ImageStart, ImageSize);
        if(!EFI_ERROR(Status)) break;
    }

    return Status;
}


/**
    This function checks whether a given PCI device can be controlled by one of
    the Option ROMs that have been already executed.

    @param PciIo PCI I/O protocol of the device whose Option ROM is about to
        execute
    @param ExecutedRom Pointer to the last element of AMI_CSM_EXECUTED_PCI_ROM structure

    @retval EFI_UNSUPPORTED Function is not implemented, use the default 
        device match criteria
    @retval EFI_SUCCESS     Device can be managed by one of the loaded ROMs

**/

EFI_STATUS
CheckOemPciSiblings(
    EFI_PCI_IO_PROTOCOL *PciIo,
    AMI_CSM_EXECUTED_PCI_ROM *ExecutedRom
)
{
/* By default this function returns EFI_UNSUPPORTED to make every Option ROM execute
    One of the possible implementation is to execute only one Option ROM for all functions
    of the PCI device. The sample below implements this kind of policy. Uncomment if needed.

    EFI_STATUS  Status;
    UINTN       Seg, Bus, Dev, Func;
    AMI_CSM_EXECUTED_PCI_ROM    *er;

    // Go through ExecutedRoms and search for the ROM that has been executed for
    // similar device (PCI Bus/Dev match)
    Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
    ASSERT_EFI_ERROR(Status);

    for (er = ExecutedRom; er->Seg | er->Bus | er->Dev | er->Fun; er--) {
        if (er->Bus == Bus && er->Dev == Dev) {
            TRACE((TRACE_ALWAYS, "CSM OPROM: skip OpROM execution for device B%x/d%x/F%x.\n",
                Bus, Dev, Func));
            return EFI_SUCCESS;
        }
    }
*/
    EFI_STATUS Status = EFI_UNSUPPORTED;
    UINTN i = 0;

    for(i = 0; CsmCheckOemPciSiblingsFunctions[i] != NULL; i++)
    {
        Status = CsmCheckOemPciSiblingsFunctions[i](PciIo, ExecutedRom);
        if(!EFI_ERROR(Status)) break;
    }
    return Status;
}


/**
    This function enables devices on PCI bus that can be controlled by the Option
    ROM associated with the given PCI device.

    @param 
        PciIo   PCI I/O protocol of the device whose Option ROM is about to execute

    @retval         EFI_UNSUPPORTED     Function is not implemented
    @retval         EFI_SUCCESS         Function has enabled all related devices

    @note  
  If OEM code can not decide for ALL related devices, it may enable only devices
  it knows about, and still return EFI_UNSUPPORTED. In this case CSM Core will
  finish the job.

**/

EFI_STATUS
EnableOemPciSiblings(
    EFI_PCI_IO_PROTOCOL *PciIo
)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    UINTN i = 0;

    for(i = 0; CsmEnableOemPciSiblingsFunctions[i] != NULL; i++)
    {
        Status = CsmEnableOemPciSiblingsFunctions[i](PciIo);
        if(!EFI_ERROR(Status)) break;
    }

    return Status;
}


/**
    This function is called from FarCall86() and Int86() before and after executing
    16-bit code. OEM code can be hooked up in SDL to the parent eLink
    "AMI_CSM_16_CALL_COMPANION_FUNCTIONS" and execute before and after 16-bit code.

    @param IsFarCall       BOOLEAN to indicate the 16-bit FAR CALL (TRUE) or INT (FALSE)
    @param TargetLocation  UINT32 address of FAR CALL or INT number
    @param Priority        BOOLEAN function is called before (TRUE) or after (FALSE) 16-bit code

    @retval         EFI_UNSUPPORTED     Function is not implemented
    @retval         EFI_SUCCESS         All OEM functions have been executed

    @note  
      For the function calls the address is passed in segment:offset format; upper 16 bits
      is the segment, lower 16 bits is the offset of the function entry point.
    @note
      After 16-bit code execution (Priority == FALSE) TargetLocation is not relevant; value is ignored.

**/

EFI_STATUS
Csm16CallCompanion(
    AMI_CSM_THUNK_DATA *ThunkData,
    BOOLEAN Priority
)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    UINTN i;
    AMI_CSM_THUNK_DATA  ThunkDataCopy = *ThunkData;

    for(i = 0; Csm16CallCompanionFunctions[i] != NULL; i++)
    {
        Status = EFI_SUCCESS;
        Csm16CallCompanionFunctions[i](&ThunkDataCopy, Priority);
    }
    return Status;
}


/**
    Loads and registers the Compatibility16 handler with the EFI SMM code.
    In also updates EFI_TO_COMPATIBILITY16_BOOT_TABLE.SmmTable.

    @param This     Indicates the EFI_LEGACY_BIOS_PLATFORM_PROTOCOL instance.
    @param EfiToCompatibility16BootTable    The boot table passed to the Compatibility16.

    @retval EFI_SUCCESS         The SMM code loaded.
    @retval EFI_DEVICE_ERROR    The SMM code failed to load.

**/

EFI_STATUS
SmmInit(
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL   *This,
  IN  VOID                                *EfiToCompatibility16BootTable
  )
{
    SMM_TABLE *SmmTable;
    SMM_ENTRY *SmmEntry;
    EFI_STATUS Status;
    UINT8 *p;
    //
    // Currently this function is implemented for two SW SMI handlers: INT15_D042 and USB
    // change NUMBER_OF_CSM_SUPPORTED_SW_SMIS and add the new SMM_ENTRYs if needed
    //
    #define NUMBER_OF_CSM_SUPPORTED_SW_SMIS 2

    Status = pBS->AllocatePool(EfiBootServicesData,
        sizeof (SMM_TABLE) + NUMBER_OF_CSM_SUPPORTED_SW_SMIS * sizeof(SMM_ENTRY),
        &SmmTable);
    ASSERT_EFI_ERROR (Status);
    pBS->SetMem(SmmTable,
        sizeof(SMM_TABLE) + NUMBER_OF_CSM_SUPPORTED_SW_SMIS * sizeof(SMM_ENTRY), 0);

    //
    // Fill in two SMM entries - one for INT15 func D042, another for USB
    //
    SmmTable->NumSmmEntries = NUMBER_OF_CSM_SUPPORTED_SW_SMIS;

    SmmEntry = &SmmTable->SmmEntry;

    SmmEntry->SmmAttributes.Type = STANDARD_IO;
    SmmEntry->SmmAttributes.PortGranularity = PORT_SIZE_16;
    SmmEntry->SmmAttributes.DataGranularity = DATA_SIZE_8;
    SmmEntry->SmmFunction.Function = INT15_D042;
    SmmEntry->SmmFunction.Owner = STANDARD_OWNER;
    SmmEntry->SmmPort = SW_SMI_IO_ADDRESS;
    SmmEntry->SmmData = INT15_D042_SWSMI;
    SmmEntry++;
    //
    // SmmPort and SmmData are UINT16 data fields, in our case SmmEntry data is UINT8.
    // We need to adjust the pointer to the next SmmEntry accofdingly so that CSM16 will
    // find it correctly.
    //
    p = (UINT8*)SmmEntry - 1;
    SmmEntry = (SMM_ENTRY*)p;

    SmmEntry->SmmAttributes.Type = STANDARD_IO;
    SmmEntry->SmmAttributes.PortGranularity = PORT_SIZE_16;
    SmmEntry->SmmAttributes.DataGranularity = DATA_SIZE_8;
    SmmEntry->SmmFunction.Function = GET_USB_BOOT_INFO;
    SmmEntry->SmmFunction.Owner = STANDARD_OWNER;
    SmmEntry->SmmPort = SW_SMI_IO_ADDRESS;
    SmmEntry->SmmData = USB_SWSMI;

    //CSM specification 0.96 defines the pointer as UINT32
    ((EFI_TO_COMPATIBILITY16_BOOT_TABLE*)EfiToCompatibility16BootTable)->SmmTable = (UINT32)SmmTable;

    return EFI_SUCCESS;
}


/**
    Executes Option ROMs that are not associated with PCI device,
    e.g. PXE base code image.

**/

EFI_STATUS
ShadowServiceRoms (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL *This
)
{
    void    *RomImageStart;
    UINTN   RomImageSize;
    EFI_STATUS  Status;
    EFI_LEGACY_BIOS_PROTOCOL *iLegacyBios;
    UINTN   Flags;
    UINTN       NoHandles;
    EFI_HANDLE  *HandleBuffer;

#pragma pack(push,1)
    typedef struct{
        UINT16 ModuleId, VendorId, DeviceId;
        UINT32 Size;
    } OPROM_HEADER;
#pragma pack(pop)
    OPROM_HEADER *Header;

    Status = pBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiLegacyBiosProtocolGuid,
        NULL,
        &NoHandles,
        &HandleBuffer);
    if (EFI_ERROR(Status)) return Status;

    Status = pBS->HandleProtocol(
            HandleBuffer[0],
            &gEfiLegacyBiosProtocolGuid,
            &iLegacyBios
            );
    pBS->FreePool(HandleBuffer);
    if (EFI_ERROR(Status)) return Status;

    Status = FindEmbeddedRom(SERVICEROM_MODULEID, 0, 0, &RomImageStart, &RomImageSize);
    if (EFI_ERROR(Status)) return Status;

    //
    // Execute InstallPciRom for service ROMs: the 1st service ROM is pointed by
    // RomImageStart; install this ROM and all consequent service ROMs
    //
    do {
        TRACE((-1, "CsmBsp: Installing service ROM, %x...", RomImageStart));
        Status = iLegacyBios->InstallPciRom(
                    iLegacyBios,
                    NULL,   // PciHandle;
                    &RomImageStart,
                    &Flags,
                    NULL, NULL, NULL, NULL);
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR(Status)) return Status;

        Header = (OPROM_HEADER*)RomImageStart-1;
        TRACE((-1, "...OK, size %x\n", Header->Size));
        Header = (OPROM_HEADER*)((UINT8*)RomImageStart+Header->Size);
        RomImageStart = Header+1;
    } while(Header->ModuleId == SERVICEROM_MODULEID);

    return EFI_SUCCESS;
}


/**
    Allows platform to perform any required action after a LegacyBios
    operation.
 
    @param        DeviceHandle    List of PCI devices in the system. Type EFI_HANDLE is defined in
        InstallProtocolInterface() in the EFI 1.10 Specification.
    @param        ShadowAddress   First free OpROM area, after other OpROMs have been dispatched..
    @param        Compatibility16Table    Pointer to the Compatibility16 Table.
    @param        AdditionalData          Pointer to additional data returned - mode specific..

    @retval        EFI_SUCCESS     The operation performed successfully.
    @retval        EFI_UNSUPPORTED Mode is not supported on this platform.
    @retval        EFI_SUCCESS     Mode specific.

**/

EFI_STATUS
PlatformHooks (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL *This,
  IN  EFI_GET_PLATFORM_HOOK_MODE        Mode,
  IN  UINT16                            Type,
  IN  EFI_HANDLE                        OPTIONAL DeviceHandle,
  IN OUT UINTN                          OPTIONAL *ShadowAddress,
  IN EFI_COMPATIBILITY16_TABLE          OPTIONAL *Compatibility16Table,
  OUT VOID                              OPTIONAL **AdditionalData
  )
{
    switch (Mode) {
        case EfiPlatformHookPrepareToScanRom:
            return EFI_UNSUPPORTED;
        case EfiPlatformHookShadowServiceRoms:
            return ShadowServiceRoms(This);
        case EfiPlatformHookAfterRomInit:
            return EFI_UNSUPPORTED;
    }
    return EFI_UNSUPPORTED;
}


/**
    Returns information associated with PCI IRQ routing.

    @param         This            Indicates the EFI_LEGACY_BIOS_PLATFORM_PROTOCOL instance.
    @param         RoutingTable    Pointer to the PCI IRQ routing table. This location is
        the $PIR table minus the header. The contents is defined
        by the PCI IRQ Routing Table Specification; it has the 
        number of RoutingTableEntries of EFI_LEGACY_IRQ_ROUTING_ENTRY.
    @param         RoutingTableEntries     Number of entries in the PCI IRQ routing table.
    @param         LocalPirqTable          $PIR table. It contains EFI_LEGACY_PIRQ_TABLE_HEADER,
        immediately followed by RoutingTable.
    @param         PirqTableSize           Size of $PIR table.
    @param         LocalIrqPriorityTable   A priority table of IRQs to assign to PCI. This table 
        has IrqPriorityTableEntries entries of
        EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY type and is used to 
        prioritize the allocation of IRQs to PCI.
    @param         IrqPriorityTableEntries Number of entries in the priority table.

    @retval  EFI_SUCCESS Data was returned successfully.

**/

EFI_STATUS
GetRoutingTable(
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL   *This,
  OUT VOID                                **RoutingTable,
  OUT UINTN                               *RoutingTableEntries,
  OUT VOID                                **LocalPirqTable, OPTIONAL
  OUT UINTN                               *PirqTableSize, OPTIONAL
  OUT VOID                                **LocalIrqPriorityTable, OPTIONAL
  OUT UINTN                               *IrqPriorityTableEntries OPTIONAL
  )
{
    UINTN i = 0;
    EFI_STATUS Status = EFI_NOT_READY;

    if(gIsValidPrt)
    {
        *RoutingTable = gPrt;
    //  *RoutingTableEntries = AmiPrt->bUsedEntries;
        *RoutingTableEntries = gUsedPciEntries;
        if (LocalPirqTable != NULL) *LocalPirqTable = gMsPrt;
        if (PirqTableSize != NULL) *PirqTableSize = gMsPrt->TableSize;
        if (LocalIrqPriorityTable != NULL || IrqPriorityTableEntries != NULL) return EFI_UNSUPPORTED;
        Status = EFI_SUCCESS;

        for(i = 0; CsmGetRoutingTableFunctions[i] != NULL; i++)
        {
            // Do not check the return status incase there are multiple functions that update
            //  each of the tables individually.
            Status = CsmGetRoutingTableFunctions[i](This, 
                                                    RoutingTable, 
                                                    RoutingTableEntries, 
                                                    LocalPirqTable, 
                                                    PirqTableSize, 
                                                    LocalIrqPriorityTable, 
                                                    IrqPriorityTableEntries);
        }
    }
    return Status;
}


/**
    This function is called from PciInterrupts.c when PCI Interrupt
    routing table is ready for copying to shadow memory. Do the final
    updates of the routing information. 

    @param         This            Indicates the EFI_LEGACY_BIOS_PLATFORM_PROTOCOL instance.
    @param         RoutingTable    Pointer to the PCI IRQ routing table. This location is
        the $PIR table. The contents is defined by the PCI IRQ 
        Routing Table Specification; it has the  number of
        RoutingTableEntries of EFI_LEGACY_IRQ_ROUTING_ENTRY.

    @retval EFI_SUCCESS Data was updated successfully.

    @note  Checksum of the table is not required in this routine, it will
                  be done after this call is executed.

**/

EFI_STATUS
BspUpdatePrt(
  IN  EFI_LEGACY_BIOS_PROTOCOL   *This,
  IN  VOID                       *RoutingTable
)
{
    UINTN i = 0;
    EFI_STATUS Status = EFI_UNSUPPORTED;

    for(i = 0; CsmBspUpdatePrtFunctions[i] != NULL; i++)
    {
        Status = CsmBspUpdatePrtFunctions[i](This, RoutingTable);
        if(!EFI_ERROR(Status)) break;
    }

    return Status;
}


/**
    This function translates the given PIRQ back through all
    buses, if required, and returns the true PIRQ and associated IRQ.

    @param         This        Indicates the EFI_LEGACY_BIOS_PLATFORM_PROTOCOL instance.
    @param         PciBus      PCI bus number for this device.
    @param         PciDevice   PCI device number for this device.
    @param         PciFunction PCI function number for this device.
    @param         Pirq        The PIRQ. PIRQ A = 0, PIRQ B = 1, and so on.
    @param         PirqIrq     IRQ assigned to the indicated PIRQ.

    @retval        EFI_SUCCESS The PIRQ was translated.
**/

EFI_STATUS
TranslatePirq (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL  *This,
  IN  UINTN                              PciBus,
  IN  UINTN                              PciDevice,
  IN  UINTN                              PciFunction,
  IN  OUT UINT8                          *Pirq,
  OUT UINT8                              *PciIrq
  )
{
    return  Intx2Pirq(
                This,
                PciBus, PciDevice, PciFunction,
                Pirq, PciIrq, NULL);
}


/**
    Prepares the attempt to boot a traditional OS.

    @param         This            Indicates the EFI_LEGACY_BIOS_PLATFORM_PROTOCOL instance.
    @param         BbsDevicePath   EFI Device Path from BootXXXX variable. Type 
        BBS_BBS_DEVICE_PATH is defined in EFI_LEGACY_BIOS_PROTOCOL.LegacyBoot().
    @param         BbsTable        A list of BBS entries of type BBS_TABLE. Type BBS_TABLE is 
        defined in Compatibility16PrepareToBoot().
    @param         LoadOptionsSize Size of LoadOption in bytes.
    @param         LoadOptions     LoadOption from BootXXXX variable.
    @param         EfiToLegacyBootTable    Pointer to EFI_TO_COMPATIBILITY16_BOOT_TABLE. Type
        EFI_TO_COMPATIBILITY16_BOOT_TABLE is defined
        in Compatibility16PrepareToBoot().
    @retval 
        EFI_SUCCESS Ready to boot.

**/

EFI_STATUS
BspPrepareToBoot (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL     *This,
  IN  BBS_BBS_DEVICE_PATH                   *BbsDevicePath,
  IN  VOID                                  *BbsTable,
  IN  UINT32                                LoadOptionsSize,
  IN  VOID                                  *LoadOptions,
  IN  VOID                                  *EfiToLegacyBootTable
  )
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    UINTN i = 0;

    for(i = 0; CsmBspPrepareToBootFunctions[i] != NULL; i++)
    {
        Status = CsmBspPrepareToBootFunctions[i](This, BbsDevicePath, BbsTable, LoadOptionsSize, LoadOptions, EfiToLegacyBootTable);
        if(!EFI_ERROR(Status)) break;
    }
    return Status;
}


/**
    CSM BSP driver entry point, it will instantinate
    LEGACY_BIOS_PLATFORM_PTOTOCOL interface

    @retval EFI_STATUS The status of CSM board initalization

**/

EFI_STATUS
InitCsmBoard (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
    EFI_STATUS          Status;
    EFI_LOADED_IMAGE_PROTOCOL   *LoadedImage;
//    EFI_GUID            FileGuid;
    static EFI_GUID       FileGuid = {0x365C62BA, 0x05EF, 0x4b2e, { 0xA7, 0xF7, 0x92, 0xC1, 0x78, 0x1A, 0xF4, 0xF9 }};

    Status = pBS->AllocatePool( EfiBootServicesData,
		sizeof(PLATFORM_BIOS_INFO),
        &gBspBiosInfo);
    if (EFI_ERROR(Status)) return Status;

    pBS->SetMem(gBspBiosInfo, sizeof(PLATFORM_BIOS_INFO), 0);

    //
    // Get LOADED_IMAGE protocol in order to get to the file name GUID
    //
    Status = pBS->HandleProtocol(
        ImageHandle, &gEfiLoadedImageProtocolGuid, &LoadedImage);
    if (EFI_ERROR(Status)) return Status;
/* change it when ROMs.bin is a section of CsmDxe.ffs
    //
    // Now we have EFI_DEVICE_PATH_PROTOCOL *LoadedImage->FilePath
    //
    FileGuid = ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)(LoadedImage->FilePath))->FvFileName;
*/

    gEmbeddedRom = NULL;     // make ReadSection allocate memory
    gEmbeddedRomSize = 0;

	Status = LoadFileSection(&FileGuid, &gEmbeddedRom, &gEmbeddedRomSize);

    if (EFI_ERROR(Status)) return Status;

    gBspBiosInfo->hImage = ImageHandle;
    gBspBiosInfo->iBiosPlatform.GetPlatformInfo      = GetPlatformInfo;
    gBspBiosInfo->iBiosPlatform.GetPlatformHandle    = GetPlatformHandle;
    gBspBiosInfo->iBiosPlatform.SmmInit              = SmmInit;
    gBspBiosInfo->iBiosPlatform.PlatformHooks        = PlatformHooks;
    gBspBiosInfo->iBiosPlatform.GetRoutingTable      = GetRoutingTable;
    gBspBiosInfo->iBiosPlatform.TranslatePirq        = TranslatePirq;
    gBspBiosInfo->iBiosPlatform.PrepareToBoot        = BspPrepareToBoot;

    InitPrt(&gBspBiosInfo->iBiosPlatform);

    //
    // Make a new handle and install the protocol
    //
    gBspBiosInfo->hBios = NULL;
    Status = pBS->InstallProtocolInterface (
                &gBspBiosInfo->hBios,
                &gEfiLegacyBiosPlatformProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &gBspBiosInfo->iBiosPlatform
    );

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
