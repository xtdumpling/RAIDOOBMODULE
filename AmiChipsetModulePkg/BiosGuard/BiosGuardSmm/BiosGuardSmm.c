//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file BiosGuardSmm.c
    This file contains code for Bios Guard SMM

**/

#include "BiosGuardSmm.h"

/*#define SB_WARM_RESET_GUID {0xb8cafa84, 0x4593, 0x4aa9, 0xae, 0xf7, 0x8e, 0x68, 0x6e, 0xb0, 0x73, 0x20}

#define SB_WARM_RESET_VARIABLE L"SbWarmResetVar"

#define SB_WARM_RESET_TAG 'IsWR'*/

UINT8 ReadRtcIndexedRegister(IN UINT8 Index);
VOID WriteRtcIndexedRegister(IN UINT8 Index, IN UINT8 Value);

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// FUNCTION:  ReadRtcIndexedRegister
//
// DESCRIPTION: Used to read RTC register indexed by the argument
//
// Input:
//    IN UINT8    Index        Index of the register to read
//                                                         
//
// Output:
//    UINT8                    Current value of the register
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 ReadRtcIndexedRegister(IN UINT8 Index){

    UINT8 Byte = IoRead8(0x70) & 0x80;   // preserve bit 7
    IoWrite8(0x70, Index | Byte);
    Byte = IoRead8(0x71);
    return Byte;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// FUNCTION:  ReadRtcIndexedRegister
//
// DESCRIPTION: Used to write to RTC register indexed by the argument
//
// Input:
//    IN UINT8    Index        Index of the register to write to 
//                                                         
//      IN UINT8  Value        Value to write to the RTC register
//
// Output:
//    VOID
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID WriteRtcIndexedRegister(IN UINT8 Index, IN UINT8 Value){
 
    IoWrite8(0x70,Index | (IoRead8(0x70) & 0x80));
    IoWrite8(0x71,Value);   
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   S3RTCresume
//
// Description: This function puts system into ACPI S3 State.
//              if token ENABLE_RTC_ONE_SECOND_WAKEUP = 1, then it setups RTC
//              1 second alarm as well.
//
// Input:       None
//
// Output:      None, system will enter ACPI S3 State.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID S3RTCresume (VOID)
{
    UINT32 IoData;
    UINT8 Hour, Minute, Second;
    BOOLEAN     inBCD = TRUE;

    // determine if RTC is in BCD mode
    if( ReadRtcIndexedRegister(0xB) & 0x4 ) // bit 2
        inBCD = FALSE;
    // wait for time update to complete before reading the values
    while( ReadRtcIndexedRegister(0xA) & 0x80 ); // while bit 7 is set the 
                                                // time update is in progress
    //read current hour, minute, second
    Hour = ReadRtcIndexedRegister(0x4);
    Minute = ReadRtcIndexedRegister(0x2);
    Second = ReadRtcIndexedRegister(0x0);

    //convert second to decimal from BCD and increment by 1
    if(inBCD)
        Second = (Second >> 4) * 10 + (Second & 0x0F);
    Second += 2;
    
    if(Second > 59){
        Second -= 60;
        if(inBCD)
            Minute = (Minute >> 4) * 10 + (Minute & 0x0F);
        Minute++;
        if(Minute > 59){
            Minute = 0;
            if(inBCD)
                Hour = (Hour >> 4) * 10 + (Hour & 0x0F);
            Hour++;
            // check 24 hour mode/12 hour mode
            if( ReadRtcIndexedRegister(0xB) & 0x2 ) {// bit 1 1=24hour else 12 hour
                if(Hour > 23)
                    Hour = 0;
            } else {
                if(Hour > 11)
                    Hour = 0;
            }

            if(inBCD)
                Hour = Hour % 10 + ( (Hour / 10) << 4 ) ;
        }
        if(inBCD)
            Minute = Minute % 10 + ( (Minute / 10) << 4 ) ;
    }

    //convert from decimal to BCD
    if(inBCD)
        Second = Second % 10 + ( (Second / 10) << 4 ) ;
    
    //set the alarm
    WriteRtcIndexedRegister(0x5, Hour);
    WriteRtcIndexedRegister(0x3, Minute);
    WriteRtcIndexedRegister(0x1, Second);
    // Clear date alarm.
    WriteRtcIndexedRegister(0xD, ( ReadRtcIndexedRegister(0xD) & (1 << 7) ));
    //enable the alarm
    WriteRtcIndexedRegister(0xB, ( ReadRtcIndexedRegister(0xB) | ((UINT8)( 1 << 5 )) ));

// ========== PORTING REQUIRED ===========================================================
//  Current implementation to simulate the Warm Reboot may not be sufficient on some platforms. 
//  S3 transition may require additional Chipset/Platform coding.
//  If needed add any necessary OEM hooks to be able to put the system into S3 at the end of this handler
//========================================================================================

    //set RTC_EN bit in PM1_EN to wake up from the alarm
    IoWrite16(PM_BASE_ADDRESS + 0x02, ( IoRead16(PM_BASE_ADDRESS + 0x02) | (1 << 10) ));
    
    //Disable Sleep SMI to avoid SMI re-entrance.
    IoWrite16(PM_BASE_ADDRESS + 0x30, ( IoRead16(PM_BASE_ADDRESS + 0x30) & (~BIT4) ));
    
    //modify power management control register to reflect S3
    IoData = IoRead32(PM_BASE_ADDRESS + 0x04);
    //following code is applicable to Intel PCH only. 
    IoData &= ~(0x1C00);
    IoData |= 0x1400; //Suspend to RAM

    IoWrite32(PM_BASE_ADDRESS + 0x04, IoData );
}

EFI_STATUS
BiosGuardRecoveryS5SmiHandler (
    IN EFI_HANDLE                DispatchHandle,
    IN CONST VOID                *DispatchContext OPTIONAL,
    IN OUT VOID                  *CommBuffer OPTIONAL,
    IN OUT UINTN                 *CommBufferSize OPTIONAL
)
{
    EFI_STATUS                   Status = EFI_SUCCESS;
    EFI_GUID                     BiosGuardRecoveryGuid = BIOS_GUARD_RECOVERY_GUID;
    EFI_GUID                     BiosGuardCapsuleVariableGuid = BIOS_GUARD_CAPSULE_VARIABLE_GUID;
    EFI_GUID                     BiosGuardCapsuleGuid = BIOS_GUARD_CAPSULE_GUID;
    EFI_GUID                     BiosGuardRecoveryAddressGuid = BIOS_GUARD_RECOVERY_ADDRESS_GUID;
    UINTN                        BiosGuardRecoveryAddressVarSize = sizeof(UINT32);
    UINTN                        BiosGuardCapsuleAddressVarSize = sizeof(AMI_BIOS_GUARD_RECOVERY);
    AMI_BIOS_GUARD_RECOVERY      *ApRecoveryAddress;
    EFI_CAPSULE_BLOCK_DESCRIPTOR *BiosGuardCapsuleBlockDesc;
    EFI_CAPSULE_HEADER           *BiosGuardCapsuleHeader;
    AMI_FLASH_UPDATE_BLOCK       FlashUpdDesc;
    UINTN                        FlashUpdDescSize = sizeof(AMI_FLASH_UPDATE_BLOCK);
    EFI_GUID                     FlashUpdGuid  = FLASH_UPDATE_GUID;

#if BIOS_GUARD_DEBUG_MODE
    DEBUG ((EFI_D_INFO, "BiosGuardRecoveryS5SmiHandler Start\n"));
    DEBUG ((EFI_D_INFO, "Guid: %x\n", BiosGuardRecoveryAddressGuid));
    DEBUG ((EFI_D_INFO, "Size: %x\n", BiosGuardRecoveryAddressVarSize));
#endif

    Status = mSmmVariable->SmmGetVariable( L"BiosGuardRecoveryAddressVariable",
                                           &BiosGuardRecoveryAddressGuid,
                                           NULL,
                                           &BiosGuardRecoveryAddressVarSize,
                                           &ApRecoveryAddress);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR (Status);
        return EFI_SUCCESS;
    }

#if BIOS_GUARD_DEBUG_MODE
    DEBUG ((EFI_D_INFO, "ApRecoveryAddress : %x\n", ApRecoveryAddress));
    DEBUG ((EFI_D_INFO, "ApRecoveryAddress->ReoveryAddress : %x\n", ApRecoveryAddress->ReoveryAddress));
#endif
    if ( (ApRecoveryAddress->ReoveryAddress == 0) || (ApRecoveryAddress->ReoveryAddress == 0xFFFFFFF) ) return EFI_SUCCESS;

    BiosGuardCapsuleBlockDesc = (EFI_CAPSULE_BLOCK_DESCRIPTOR*)ApRecoveryAddress->ReoveryAddress;
#if BIOS_GUARD_DEBUG_MODE
    DEBUG ((EFI_D_INFO, "Length[0] : %x\n", BiosGuardCapsuleBlockDesc->Length));
    DEBUG ((EFI_D_INFO, "Union[0].DataBlock : %x\n", BiosGuardCapsuleBlockDesc->Union.DataBlock));
    DEBUG ((EFI_D_INFO, "Union[0].ContinuationPointer : %x\n", BiosGuardCapsuleBlockDesc->Union.ContinuationPointer));
#endif

    BiosGuardCapsuleHeader = (EFI_CAPSULE_HEADER*)((UINTN)BiosGuardCapsuleBlockDesc->Union.DataBlock);

#if BIOS_GUARD_DEBUG_MODE
    DEBUG ((EFI_D_INFO, "BiosGuardCapsuleHeader->CapsuleGuid : %g\n", BiosGuardCapsuleHeader->CapsuleGuid));
    DEBUG ((EFI_D_INFO, "BiosGuardCapsuleHeader->HeaderSize : %X\n", BiosGuardCapsuleHeader->HeaderSize));
    DEBUG ((EFI_D_INFO, "BiosGuardCapsuleHeader->Flags : %X\n", BiosGuardCapsuleHeader->Flags));
    DEBUG ((EFI_D_INFO, "BiosGuardCapsuleHeader->CapsuleImageSize : %X\n", BiosGuardCapsuleHeader->CapsuleImageSize));
#endif

    if ( CompareGuid (&BiosGuardRecoveryGuid, &BiosGuardCapsuleHeader->CapsuleGuid)  ) {	    
        DEBUG ((EFI_D_INFO, "Start to do BIOS Guard Recovery\n"));
        ZeroMem(&FlashUpdDesc, sizeof(FlashUpdDesc));
        FlashUpdDesc.FlashOpType = FlRecovery;
        
        BiosGuardCapsuleBlockDesc++;
        if ( BiosGuardCapsuleBlockDesc->Union.DataBlock == 0 ) return EFI_SUCCESS;
        
        if ( BiosGuardCapsuleBlockDesc->Length > sizeof(FlashUpdDesc.FwImage.AmiRomFileName) ) {
        	DEBUG ((EFI_D_INFO, "Your input file name is too long over %d byte.\n", sizeof(FlashUpdDesc.FwImage.AmiRomFileName)));
        	return EFI_SUCCESS;
        }

        CopyMem(&FlashUpdDesc.FwImage.AmiRomFileName, (VOID*)BiosGuardCapsuleBlockDesc->Union.DataBlock, BiosGuardCapsuleBlockDesc->Length);
        FlashUpdDesc.ImageSize = (UINT32)BIOSGUARD_RECOVERY_IMAGE_SIZE;
        FlashUpdDesc.ROMSection = BIOS_GUARD_RECOVERY_ROM_SECTION;

        Status = mSmmVariable->SmmSetVariable ( FLASH_UPDATE_VAR,
                                                &FlashUpdGuid,
                                                (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                                                FlashUpdDescSize,
                                                &FlashUpdDesc);

        if ( EFI_ERROR(Status) ) {
            ASSERT_EFI_ERROR (Status);
            return EFI_SUCCESS;
        }
    }

    if ( CompareGuid (&BiosGuardRecoveryGuid, &BiosGuardCapsuleHeader->CapsuleGuid) ||
         CompareGuid (&BiosGuardCapsuleGuid, &BiosGuardCapsuleHeader->CapsuleGuid) ) {
        DEBUG ((EFI_D_INFO, "Start to do BIOS Guard Capsule\n"));

        Status = mSmmVariable->SmmSetVariable ( L"BiosGuardCapsuleVariable",
                                                &BiosGuardCapsuleVariableGuid,
                                                (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                                                BiosGuardCapsuleAddressVarSize,
                                                ApRecoveryAddress);
        if ( EFI_ERROR(Status) ) {
            ASSERT_EFI_ERROR (Status);
            return EFI_SUCCESS;
        }

        S3RTCresume();
    }

#if BIOS_GUARD_DEBUG_MODE
    DEBUG ((EFI_D_INFO, "BiosGuardRecoveryS5SmiHandler End\n"));
#endif
    return EFI_SUCCESS;
}

EFI_STATUS
BiosGuardSmmEntry (
    IN EFI_HANDLE                ImageHandle,
    IN EFI_SYSTEM_TABLE          *SystemTable
)
{
    EFI_STATUS                              Status;
    EFI_SMM_SX_DISPATCH2_PROTOCOL           *SxDispatch;
    EFI_SMM_SX_REGISTER_CONTEXT             SxContext = {SxS5, SxEntry};
    EFI_HANDLE                              Handle;

    if ( !((AsmReadMsr64 (EFI_PLATFORM_INFORMATION) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
            (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) ) {
        DEBUG ((DEBUG_INFO, "BIOS Guard is disabled or not supported \n"));
        return EFI_SUCCESS;
    }

    Status = gSmst->SmmLocateProtocol(
                 &gEfiSmmVariableProtocolGuid,
                 NULL,
                 (VOID**)&mSmmVariable
             );
    if ( EFI_ERROR(Status) ) {
        ASSERT_EFI_ERROR (Status);
        return Status;
    }

    Status = gSmst->SmmLocateProtocol (&gEfiSmmSxDispatch2ProtocolGuid, NULL, &SxDispatch);
    ASSERT_EFI_ERROR (Status);
    if (!EFI_ERROR(Status)) {
        Status  = SxDispatch->Register (SxDispatch, \
                                        BiosGuardRecoveryS5SmiHandler, \
                                        &SxContext, \
                                        &Handle);
        ASSERT_EFI_ERROR (Status);
    }

    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
