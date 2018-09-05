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

/** @file thunk.c
    Int86 and FarCall86 functions implementation

**/

#include <Protocol/LegacyBios.h>
#include "Csm.h"
#include <AmiDxeLib.h>
#if defined(AMIUSB_SUPPORT)
#include <Protocol/AmiUsbController.h>
#endif

#if defined(AMIUSB_SUPPORT)
EFI_USB_PROTOCOL *gUsbProtocol = NULL;
#endif
/*
AMI_EFIKEYCODE_PROTOCOL *pKeyCodeProtocol=NULL;

EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *gPS2KBDInterface = NULL;
*/
EFI_GUID gCsmThunkGuid = {
    0xA08276EC,0xA0FE,0x4e06,0x86,0x70,0x38,0x53,0x36,0xC7,0xD0,0x93
};

EFI_LEGACY_8259_PROTOCOL	*gLegacy8259;
extern BIOS_INFO        	*gCoreBiosInfo;

UINTN gThunkAsmAddr;
//extern BOOLEAN gIsBootPrepared;
BOOLEAN gInsideThunk = FALSE;

//EFI_STATUS GetPs2SimpleTextInProtocolInterface(
//    BIOS_INFO               *This,
//    EFI_SIMPLE_TEXT_INPUT_PROTOCOL  **PS2KBDInterface
//);

/**
    Executes BIOS interrupt routine

    @param LegacyBios protocol instance pointer
    @param BiosInt     Interrupt number
    @param Regs        CPU register values to pass to the interrupt handler

    @retval TRUE        Interrupt was not executed
    @retval FALSE       INT completed. Caller will see Regs for status

**/

BOOLEAN EFIAPI Int86(
    IN EFI_LEGACY_BIOS_PROTOCOL     *This,
    IN UINT8                        BiosInt,
    IN OUT EFI_IA32_REGISTER_SET    *Regs
)
{
    EFI_TPL     OldTpl;

    AMI_CSM_THUNK_DATA *ThunkData = (AMI_CSM_THUNK_DATA*)(gThunkAsmAddr+6);
    //
    // To avoid reentrance in Int86
    //
    if(gInsideThunk){
        return FALSE;
    }
    gInsideThunk = TRUE;
    ThunkData->isFarCall = FALSE;
    ThunkData->BiosInt = BiosInt;
    ThunkData->StackData.StackSize = 0; //This is required


    //Copy thunk registers.
    ThunkData->Regs = *Regs;
    
    Csm16CallCompanion(ThunkData, TRUE);
    
#if defined(AMIUSB_SUPPORT)
{
    EFI_STATUS  Status;

    if (gUsbProtocol == NULL) {
        Status = pBS->LocateProtocol(&gEfiUsbProtocolGuid, NULL, &gUsbProtocol);
        if (EFI_ERROR(Status)) {
            gUsbProtocol=NULL;      // To be used later, after returning from INT
        }
    }
    if (gUsbProtocol != NULL) {
        gUsbProtocol->UsbChangeEfiToLegacy(1);
    }
}
#endif

    OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
    gLegacy8259->SetMode(gLegacy8259,Efi8259LegacyMode,NULL,NULL);

    ((void(*)(UINTN))gThunkAsmAddr)(gThunkAsmAddr);

#if defined(AMIUSB_SUPPORT)
    if (gUsbProtocol) {
        gUsbProtocol->UsbChangeEfiToLegacy(0);
    }
#endif

    gLegacy8259->SetMode(gLegacy8259,Efi8259ProtectedMode,NULL,NULL);
    pBS->RestoreTPL(OldTpl);

    Csm16CallCompanion(ThunkData, FALSE);
    
    //Restore thunk registers.
    *Regs = ThunkData->Regs;
    gInsideThunk = FALSE;

    return FALSE;   // INT completed. Caller will see Regs for status.
}


/**
    Executes 16 bit code starting from a given address

    @param This        LegacyBiosProtocol instance pointer
    @param Segment_Offset     Starting address of the 16 bit routine
    @param Regs        CPU register values to pass to the routine
    @param Stack       Stack pointer with the possible data to be passed to the routine
    @param StackSize   Size of the stack passed to the routine

    @retval TRUE        Call was not executed
    @retval FALSE       Call completed. Caller will see Regs for status

**/

BOOLEAN EFIAPI FarCall86(
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  IN  UINT16                          Segment,
  IN  UINT16                          Offset,
  IN  EFI_IA32_REGISTER_SET           *Regs,
  IN  VOID                            *Stack,
  IN  UINTN                           StackSize
)
{
    EFI_TPL     OldTpl;
    UINT16      IrqMask, SaveIrqMask;
    UINT16      ProtectedIrqMask, ProtectedSaveIrqMask;
    AMI_CSM_THUNK_DATA *ThunkData = (AMI_CSM_THUNK_DATA*)(gThunkAsmAddr+6);
    UINT16      FuncNumber;
    BOOLEAN     IsCsm16Call =
                    Segment == gCoreBiosInfo->Csm16EntrySeg &&
                    gCoreBiosInfo->Csm16EntryOfs == Offset;
    BOOLEAN     IsCsm16DispatchOpRom = IsCsm16Call &&
                    Regs->X.AX == Legacy16DispatchOprom;

    BOOLEAN     IsCsm16LegacyBoot = IsCsm16Call &&
                    Regs->X.AX == Legacy16Boot;

    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_KEY_TOGGLE_STATE    KeyToggleState=0;  
    // 
    // To avoid reentrance in FarCall86
    //
    if(gInsideThunk){
        return  FALSE;
    }

    gInsideThunk = TRUE;

    //
    // Change USB operation mode indicator from EFI to Legacy
    //
#if defined(AMIUSB_SUPPORT)
    if (IsCsm16LegacyBoot == FALSE) {
        if (gUsbProtocol == NULL) {
            Status = pBS->LocateProtocol(&gEfiUsbProtocolGuid, NULL, &gUsbProtocol);
            if (EFI_ERROR(Status)) {
                gUsbProtocol=NULL;      // To be used later, after FarCall execution
            }
        }
    }
    if (gUsbProtocol != NULL) {
        gUsbProtocol->UsbChangeEfiToLegacy(1);
    }
#endif

    ThunkData->isFarCall = TRUE;

    //Copy address for thunk.
    ThunkData->FarCall.Segment = Segment;
    ThunkData->FarCall.Offset = Offset;

    //Copy address for stack
    if (Stack)
    {
        ThunkData->StackData.Stack = (UINT32)Stack;
        ThunkData->StackData.StackSize = (UINT32)StackSize;
    } else ThunkData->StackData.StackSize = 0;

    //Copy thunk registers.
    ThunkData->Regs = *Regs;
    
    Csm16CallCompanion(ThunkData, TRUE);
    
/*
    // Reset the PS/2 keyboard before legacy boot
    if (IsCsm16LegacyBoot == TRUE) {
        if (gPS2KBDInterface) gPS2KBDInterface->Reset(gPS2KBDInterface, FALSE);
    }
*/
    FuncNumber = Regs->X.AX;

    if (IsCsm16Call) {
        if (FuncNumber != 05 && FuncNumber != 02 && FuncNumber != 03) {
            gLegacy8259->GetMask(gLegacy8259, &SaveIrqMask, NULL, NULL, NULL);      // Save current Mask

            IrqMask = -1;
            gLegacy8259->SetMask(gLegacy8259, &IrqMask, NULL, NULL, NULL);          // Set new Mask
        }
    }

    //
    // Mask all HW interrupts for real mode for CSM16 function #0 (InitializeYourself).
    // This is needed since IVT is not installed before executing function #0.
    //
    if (IsCsm16Call && FuncNumber == 00 ) {
        gLegacy8259->GetMask(gLegacy8259, NULL, NULL, &ProtectedSaveIrqMask, NULL);      // Save current Mask
        ProtectedIrqMask = -1;
        gLegacy8259->SetMask(gLegacy8259, NULL, NULL, &ProtectedIrqMask, NULL);          // Set new Mask
    }

    OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
    
	//
    // The following call will change the PIC settings to real mode; this includes
    // unmasking all real mode interrupts. Note, that at this point HW interrupts
    // must be disabled. This is done by the previous RaiseTPL call.
    //
    gLegacy8259->SetMode(gLegacy8259,Efi8259LegacyMode,NULL,NULL);

    ((void(*)(UINTN))gThunkAsmAddr)(gThunkAsmAddr);

#if defined(AMIUSB_SUPPORT)
    if (gUsbProtocol) {
        gUsbProtocol->UsbChangeEfiToLegacy(0);
    }
#endif

    if (IsCsm16Call && FuncNumber == 00 ) {
        gLegacy8259->SetMask(gLegacy8259, NULL, NULL, &ProtectedSaveIrqMask, NULL);          // Set new Mask
    }

    gLegacy8259->SetMode(gLegacy8259,Efi8259ProtectedMode,NULL,NULL);
     
    //Restore thunk registers.
    *Regs = ThunkData->Regs;
    pBS->RestoreTPL(OldTpl);

    //
    // Change USB operation mode indicator back to EFI
    //

    if (!IsCsm16Call ||
        (IsCsm16Call &&
            (FuncNumber == 05 || FuncNumber == 02 || FuncNumber == 03))) {
/*
		//      Reset PS2 Mouse
        if (!gPS2KBDInterface) {
            GetPs2SimpleTextInProtocolInterface((BIOS_INFO *)This, &gPS2KBDInterface);
        }

        if (gPS2KBDInterface) gPS2KBDInterface->Reset(gPS2KBDInterface, FALSE);
*/
    }
    else    gLegacy8259->SetMask(gLegacy8259, &SaveIrqMask, NULL, NULL, NULL);  // Restore mask
/*
    if(IsCsm16DispatchOpRom) {
        if((pKeyCodeProtocol == NULL && IsCsm16LegacyBoot == FALSE)) {
            pBS->HandleProtocol(pST->ConsoleInHandle, &gAmiEfiKeycodeProtocolGuid, (void*)&pKeyCodeProtocol);
        }
        if(pKeyCodeProtocol != NULL) {
            UINT8   KeyState;
            KeyState = *(UINT8*)(UINTN)0x417;
            if(KeyState & 0x10) {
                KeyToggleState |= SCROLL_LOCK_ACTIVE;
            }
            if(KeyState & 0x20) {
                KeyToggleState |= NUM_LOCK_ACTIVE;
            }
            if(KeyState & 0x40) {
                KeyToggleState |= CAPS_LOCK_ACTIVE;
            }
            KeyToggleState |= TOGGLE_STATE_VALID;
            pKeyCodeProtocol->SetState((EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL*)pKeyCodeProtocol,&KeyToggleState);
        }
    }
*/
    Csm16CallCompanion(ThunkData, FALSE);
    
    *(UINT8*)(UINTN)0x417 &= 0x70;  // Clear key modifiers status in BDA
   
    gInsideThunk = FALSE;

    return FALSE;   // CALL completed. Caller will see Regs for status.
}


/**
    Code that loads 16-bit thunk code at a particular address

    @param Guid - 16-bit binary file GUID
    @param Address - Address to load the binary to

    @retval 
        Status of the load operation

**/

EFI_STATUS LoadRealModeFile(EFI_GUID *Guid, VOID *Address)
{
    EFI_FIRMWARE_VOLUME2_PROTOCOL    *Fv;
    UINT32      Authentication;
    VOID        *BufferAddr=0;
    UINTN       BufferSize=0;
    EFI_STATUS  Status;
    UINTN       NumHandles;
    EFI_HANDLE  *HandleBuffer;
    UINTN       i;

    Status = pBS->LocateHandleBuffer(ByProtocol,&gEfiFirmwareVolume2ProtocolGuid,NULL,&NumHandles,&HandleBuffer);
    if (EFI_ERROR(Status)) return Status;

    for (i = 0; i< NumHandles; ++i)
    {
        Status = pBS->HandleProtocol(HandleBuffer[i],&guidFV,&Fv);
        if (EFI_ERROR(Status)) continue;

        Status = Fv->ReadSection(Fv,
            Guid,
            EFI_SECTION_RAW,
            0,
            &BufferAddr,
            &BufferSize,
            &Authentication);

        if (Status == EFI_SUCCESS)
        {
            MemCpy(
                Address,
                (UINT8*)BufferAddr,
                BufferSize
            );
            pBS->FreePool(BufferAddr);
            break;
        }
    }

    pBS->FreePool(HandleBuffer);

    return Status;
}


/**
    16-bit thunk initialization routine. It calls 16 bit code to
    do the address fixups within 16-bit code.

    @param CoreBiosInfo The instance of BIOS_INFO

    @retval EFI_STATUS Status of the initialization completion

**/

EFI_STATUS InitializeThunk(
    BIOS_INFO* CoreBiosInfo
)
{
    EFI_STATUS Status;
    EFI_PHYSICAL_ADDRESS    Thunkaddr = (EFI_PHYSICAL_ADDRESS)CoreBiosInfo->Thunk;
    UINT16 ThunkFixupsOffset;

    //Locate Legacy8259 Protocol
    gLegacy8259 = CoreBiosInfo->i8259;
	ASSERT(gLegacy8259);

    //Copy ThunkAsm to memory.
    Status = LoadRealModeFile(&gCsmThunkGuid, (VOID*)Thunkaddr);
    if (EFI_ERROR(Status)) return Status;

    gThunkAsmAddr = (UINT32)Thunkaddr;

    //Call fixups
    ThunkFixupsOffset = *(UINT16*)(gThunkAsmAddr+2);
    ((void(*)(UINTN))(gThunkAsmAddr+ThunkFixupsOffset))(gThunkAsmAddr);

    return Status;
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
