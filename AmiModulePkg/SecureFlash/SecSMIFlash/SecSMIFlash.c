//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiHobs.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SmmSxDispatch.h>
#include <Protocol/SecSmiFlash.h>
#include <Protocol/FlashProtocol.h>
#include <Library/AmiBufferValidationLib.h>
#include <Library/DebugLib.h>
#include "AmiCertificate.h"

#if FWCAPSULE_RECOVERY_SUPPORT == 1
#include <Capsule.h>
#include <Guid/CapsuleVendor.h>
#endif

#if INSTALL_SECURE_FLASH_SW_SMI_HNDL == 1
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmSwDispatch2.h>

static EFI_SMM_CPU_PROTOCOL  *gSmmCpu;
#define RETURN(status) {return status;}
#endif
//----------------------------------------------------------------------
// Module defined global variables
//----------------------------------------------------------------------

static EFI_GUID FlashUpdGuid    = FLASH_UPDATE_GUID;
EFI_GUID gFWCapsuleGuid         = APTIO_FW_CAPSULE_GUID;
EFI_GUID gPRKeyGuid             = PR_KEY_GUID;
EFI_GUID gFwCapFfsGuid          = AMI_FW_CAPSULE_FFS_GUID;

static FLASH_UPD_POLICY FlUpdatePolicy = {FlashUpdatePolicy, BBUpdatePolicy};

EFI_SHA256_HASH  *gHashTbl = NULL;
UINT8             gHashDB[SHA256_DIGEST_SIZE];
CRYPT_HANDLE      gpPubKeyHndl;

AMI_DIGITAL_SIGNATURE_PROTOCOL *gAmiSig;

#if FWCAPSULE_RECOVERY_SUPPORT == 1
static EFI_PHYSICAL_ADDRESS  gpFwCapsuleMailbox     = 0;
static UINTN                 gpFwCapsuleMailboxSize = 0;
static CHAR16                gCapsuleNameBuffer[30];
#endif

// Allocate the space in OS Reserved mem for new FW image to be uploaded by a Flash tool
// Alternatively the buffer may be reserved within the SMM TSEG based on NEW_BIOS_MEM_ALLOC Token setting
// AFU would have to execute a sequence of SW SMI calls to load new BIOS image to a reserved mem
static EFI_PHYSICAL_ADDRESS gpFwCapsuleBuffer = 0;
UINTN gFwCapMaxSize = FWCAPSULE_IMAGE_SIZE;

//----------------------------------------------------------------------
// Flash Upd Protocol defines
//----------------------------------------------------------------------
typedef EFI_STATUS (EFIAPI *FLASH_READ_WRITE)(
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
);
typedef EFI_STATUS (EFIAPI *FLASH_ERASE)(
    VOID* FlashAddress, UINTN Size
);

FLASH_PROTOCOL      *Flash;
FLASH_READ_WRITE    pFlashWrite; // Original Ptr inside FlashAPI
FLASH_READ_WRITE    pFlashUpdate;
FLASH_ERASE         pFlashErase;

// Global variable used to store the address of the start of the BIOS region in the flash part
UINTN Flash4GBMapStart = (0xFFFFFFFF - FLASH_SIZE + 1);
ROM_AREA *RomLayout = NULL;
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// UpFront Function definitions
BOOLEAN SupportUpdateCapsuleReset (
    VOID
);
EFI_STATUS CapsuleValidate (
    IN OUT UINT8     **pFwCapsule,
    IN OUT APTIO_FW_CAPSULE_HEADER     **pFwCapsuleHdr
);
EFI_STATUS LoadFwImage(
    IN OUT FUNC_BLOCK  *pFuncBlock
);
EFI_STATUS GetFlUpdPolicy(
    IN OUT FLASH_POLICY_INFO_BLOCK  *InfoBlock
);
EFI_STATUS SetFlUpdMethod(
    IN OUT FUNC_FLASH_SESSION_BLOCK  *pSessionBlock
);
EFI_STATUS EFIAPI SecureFlashWrite (
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
);
EFI_STATUS EFIAPI SecureFlashUpdate (
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
);
EFI_STATUS EFIAPI SecureFlashErase (
    VOID* FlashAddress, UINTN Size
);
EFI_STATUS FindCapHdrFFS(
    IN  VOID    *pCapsule,
    OUT UINT8  **pFfsData
);

//----------------------------------------------------------------------
//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   SecSmiFlash
//
// Description: 
//
// Input:   
//
// Output: 
//
// Returns: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_SEC_SMI_FLASH_PROTOCOL    SecSmiFlash = {
    LoadFwImage,
    GetFlUpdPolicy,
    SetFlUpdMethod,
    (void*)CapsuleValidate,
    0,
    0,
    0
};

#if FWCAPSULE_RECOVERY_SUPPORT == 1

#if defined(CSLIB_WARM_RESET_SUPPORTED) 

#if CSLIB_WARM_RESET_SUPPORTED == 1
extern SBLib_ResetSystem( IN EFI_RESET_TYPE ResetType );
#else

void AsmWbinvd ();

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

    //
    // Flush cache into memory before we go to sleep. It is necessary for S3 sleep
    // because the changes in gpFwCapsuleMailbox are only in cache now
    //
    AsmWbinvd ();

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
/*
    // AMD w/a to enter S3 state
    IoData |= 0x2C00; //Suspend to RAM
    {
        UINT8 Temp8;
        IoWrite8(0xCD6, 0x004);
        Temp8 = IoRead8(0xCD7);
        Temp8 &= ~(BIT7);
        IoWrite8(0xCD6, 0x004);
        IoWrite8(0xCD7, Temp8);
        IoWrite8(0xCD6, 0x007);
        IoWrite8(0xCD7, BIT7);
    }
} 
*/
    IoWrite32(PM_BASE_ADDRESS + 0x04, IoData );
}
#endif // #if CSLIB_WARM_RESET_SUPPORTED
#endif //#if defined(CSLIB_WARM_RESET_SUPPORTED) 

/**
 * Capsule NVRAM variables are owned by either the runtime Capsule Update service 
 * or by this driver. Each service must not override previously created instances 
 * of the variable and create a new one with n+1 index
 */
CHAR16* GetLastFwCapsuleVariable()
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN Index = 0;
    EFI_PHYSICAL_ADDRESS    IoData=0;
    UINTN                   Size;

    Swprintf_s(gCapsuleNameBuffer, sizeof(gCapsuleNameBuffer)/sizeof(CHAR16), L"%s", EFI_CAPSULE_VARIABLE_NAME);
    // get any NVRAM variable of the format L"CapsuleUpdateDataN" where N is an integer
    while(!EFI_ERROR(Status)) {
        if(Index > 0 )
            Swprintf_s(gCapsuleNameBuffer, sizeof(gCapsuleNameBuffer)/sizeof(CHAR16), L"%s%d", EFI_CAPSULE_VARIABLE_NAME, Index);
        Size = sizeof(EFI_PHYSICAL_ADDRESS);
        Status = pRS->GetVariable(  gCapsuleNameBuffer, &gEfiCapsuleVendorGuid,
                                    NULL,
                                    &Size, 
                                    &IoData);
        DEBUG ((SecureMod_DEBUG_LEVEL,"Get '%S' %r, %lX\n", gCapsuleNameBuffer, Status, IoData));
        if(!EFI_ERROR(Status) && 
           (IoData == gpFwCapsuleMailbox && 
            !EFI_ERROR(AmiValidateSmramBuffer((VOID*)IoData, gpFwCapsuleMailboxSize)))
        ) {
            break;
        }
        Index++;
    }

    return gCapsuleNameBuffer;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// FUNCTION:  SmiS5CapsuleCallback
//
// DESCRIPTION: SMI handler to perform capsule reset (bounce from S5 to S3)
// ========== PORTING REQUIRED ===========================================================
//  Current implementation to simulate the Warm Reboot may not be sufficient on some platforms. 
//  S3 transition may require additional Chipset/Platform coding.
//  If needed add any necessary OEM hooks to be able to put the system into S3 at the end of this handler
//========================================================================================
//
// Input:
//    IN  EFI_HANDLE    DispatchHandle                   Handle of SMI dispatch
//                                                       protocol
//    IN  EFI_SMM_SX_DISPATCH_CONTEXT* DispatchContext   Pointer to SMI dispatch
//                                                       context structure
//
// Output:
//    VOID
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID SmiS5CapsuleCallback ( IN  EFI_HANDLE                    DispatchHandle,
                            IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
){
    EFI_PHYSICAL_ADDRESS    IoData;
    UINTN                   Size;
    EFI_CAPSULE_HEADER     *CapsuleHeader;
    EFI_CAPSULE_BLOCK_DESCRIPTOR *pCapsuleMailboxPtr;
    AMI_FLASH_UPDATE_BLOCK  FlUpdateBlock;

    DEBUG ((SecureMod_DEBUG_LEVEL,"SecSMI. S5 Trap\n"));
    
    //
    //Check if the Capsule update is supported by platform policy
    //
    if (!SupportUpdateCapsuleReset()) 
        return;

    Size = sizeof(AMI_FLASH_UPDATE_BLOCK);
    if(EFI_ERROR(pRS->GetVariable(FLASH_UPDATE_VAR, &FlashUpdGuid, NULL, &Size, &FlUpdateBlock)) ||
       FlUpdateBlock.FlashOpType != FlCapsule)
        return;

    // verify the FW capsule mailbox is in SMRAM
    Size = sizeof(EFI_PHYSICAL_ADDRESS);
    if(EFI_ERROR(pRS->GetVariable(GetLastFwCapsuleVariable(), &gEfiCapsuleVendorGuid, NULL, &Size, &IoData)))
        return;

    if(IoData != gpFwCapsuleMailbox ||
       EFI_ERROR(AmiValidateSmramBuffer((VOID*)IoData, gpFwCapsuleMailboxSize)))
        return;
    
    pCapsuleMailboxPtr = (EFI_CAPSULE_BLOCK_DESCRIPTOR*)(UINTN)IoData;
    CapsuleHeader = (EFI_CAPSULE_HEADER*)pCapsuleMailboxPtr[0].Union.DataBlock;
    //
    // Compare GUID with APTIO_FW_CAPSULE_GUID 
    //
    if (guidcmp (&CapsuleHeader->CapsuleGuid, &gFWCapsuleGuid))
        return;

    DEBUG ((SecureMod_DEBUG_LEVEL,"Enter warm reset...\n"));

#if defined(CSLIB_WARM_RESET_SUPPORTED)
    #if CSLIB_WARM_RESET_SUPPORTED == 1
    SBLib_ResetSystem(EfiResetWarm);
    #else
    S3RTCresume();
    #endif
#endif
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    SupportUpdateCapsuleReset
//
// Description:  This function returns platform policy capability for capsule update via a system reset.
//
// Input:        None
//
// Output:      TRUE  - memory can be preserved across reset
//              FALSE - memory integrity across reset is not guaranteed
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN SupportUpdateCapsuleReset (
    VOID
){
  //
  //If the platform has a way to guarantee the memory integrity across a system reset, return 
  //TRUE, else FALSE. 
  //
    if( (FlUpdatePolicy.FlashUpdate & FlCapsule) || 
        (FlUpdatePolicy.BBUpdate & FlCapsule))
        return TRUE;

    return FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    UpdateCapsule
//
// Description:    This code prepares Capsule Update EFI Variable
//
// Input:        
//  IN EFI_CAPSULE_HEADER **CapsuleHeaderArray - array of pointers to capsule headers passed in
//
// Output:      EFI_SUCCESS - capsule processed successfully
//              EFI_INVALID_PARAMETER - CapsuleCount is less than 1,CapsuleGuid is not supported
//              EFI_DEVICE_ERROR - capsule processing failed
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS UpdateCapsule (
    IN FUNC_FLASH_SESSION_BLOCK *pSessionBlock,
    IN APTIO_FW_CAPSULE_HEADER  *pFwCapsuleHdr
){
    EFI_CAPSULE_BLOCK_DESCRIPTOR *pCapsuleMailboxPtr;
    UINT32              Attributes;
    CHAR16             *EfiFwCapsuleVarName = NULL;

    //
    //Check if the platform supports update capsule across a system reset
    //
    if (!SupportUpdateCapsuleReset()) {
        return EFI_UNSUPPORTED;
    }
    //
    //Compare GUID with APTIO_FW_CAPSULE_GUID 
    //
    if (!pFwCapsuleHdr || guidcmp (&pFwCapsuleHdr->CapHdr.CapsuleGuid, &gFWCapsuleGuid))
        return EFI_DEVICE_ERROR; 

    pCapsuleMailboxPtr = (EFI_CAPSULE_BLOCK_DESCRIPTOR*)(UINTN)gpFwCapsuleMailbox;
    pCapsuleMailboxPtr[0].Length = pFwCapsuleHdr->CapHdr.HeaderSize;
    pCapsuleMailboxPtr[0].Union.DataBlock = (EFI_PHYSICAL_ADDRESS)(UINTN)pFwCapsuleHdr;

    pCapsuleMailboxPtr[1].Length = pFwCapsuleHdr->CapHdr.CapsuleImageSize-pFwCapsuleHdr->CapHdr.HeaderSize;
    pCapsuleMailboxPtr[1].Union.DataBlock = (EFI_PHYSICAL_ADDRESS)(UINTN)SecSmiFlash.pFwCapsule;

    pCapsuleMailboxPtr[2].Length = 0;
    pCapsuleMailboxPtr[2].Union.DataBlock = 0;

    EfiFwCapsuleVarName = GetLastFwCapsuleVariable();
    Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS;
    // Erase prev Var copy
    pRS->SetVariable ( EfiFwCapsuleVarName, &gEfiCapsuleVendorGuid, 0, 0, NULL);
    if(!EFI_ERROR(pRS->SetVariable ( EfiFwCapsuleVarName, &gEfiCapsuleVendorGuid, Attributes, sizeof(EFI_PHYSICAL_ADDRESS),(VOID*)&gpFwCapsuleMailbox)))
        return EFI_SUCCESS;

    return EFI_DEVICE_ERROR;
}

#endif //#if FWCAPSULE_RECOVERY_SUPPORT == 1

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    SetFlashUpdateVar
//
// Description:    This code finds if the capsule needs reset to update, if no, update immediately.
//
// Input:
//  IN EFI_CAPSULE_HEADER **CapsuleHeaderArray - array of pointers to capsule headers passed in
//  IN UINTN CapsuleCount - number of capsule
//  IN EFI_PHYSICAL_ADDRESS ScatterGatherList - physical address of datablock list points to capsule
//
// Output:      EFI_SUCCESS - capsule processed successfully
//              EFI_INVALID_PARAMETER - CapsuleCount is less than 1,CapsuleGuid is not supported
//              EFI_DEVICE_ERROR - capsule processing failed
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SetFlashUpdateVar (
    IN FUNC_FLASH_SESSION_BLOCK    *pSessionBlock
){
    UINTN               Size;
    UINT32              CounterHi;

    if(pSessionBlock->FlUpdBlock.FlashOpType == FlRecovery &&
        pSessionBlock->FlUpdBlock.FwImage.AmiRomFileName[0] == 0
    )
        return EFI_DEVICE_ERROR;

    CounterHi = 0;
    Size = sizeof(UINT32);
// MonotonicCounter is a boot time service, hence the variable may have restricted access in runtime
    if(EFI_ERROR(pRS->GetVariable(L"MonotonicCounter", &gAmiGlobalVariableGuid,
                  NULL, &Size, &CounterHi))
    )
//        return Status;//EFI_DEVICE_ERROR;
//SetMode should set FlashUpd even if no MC var detected.
        CounterHi=0xffffffff;

    pSessionBlock->FlUpdBlock.MonotonicCounter = CounterHi;
    CounterHi = (EFI_VARIABLE_NON_VOLATILE |
        EFI_VARIABLE_RUNTIME_ACCESS |
        EFI_VARIABLE_BOOTSERVICE_ACCESS);
    // Erase prev copy
    pRS->SetVariable ( FLASH_UPDATE_VAR, &FlashUpdGuid,0,0,NULL); 
    if(!EFI_ERROR(pRS->SetVariable ( FLASH_UPDATE_VAR, &FlashUpdGuid, CounterHi,
                  sizeof(AMI_FLASH_UPDATE_BLOCK), (VOID*) &pSessionBlock->FlUpdBlock )))
        return EFI_SUCCESS;

    return EFI_DEVICE_ERROR;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   GetFlUpdPolicy
//
// Description: 
//
// Input:   
//
// Output: 
//
// Returns: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS GetFlUpdPolicy(
    IN OUT FLASH_POLICY_INFO_BLOCK  *InfoBlock
)
{
    UINTN   KeySize = DEFAULT_RSA_KEY_MODULUS_LEN;

DEBUG ((SecureMod_DEBUG_LEVEL,"SecSMI. GetPolicy. %X_%X\n",FlUpdatePolicy.FlashUpdate, FlUpdatePolicy.BBUpdate));

//APTIOV_SERVER_OVERRIDE_START: Security Vulnerability
    if(EFI_ERROR(AmiValidateMemoryBuffer(InfoBlock, sizeof(FLASH_POLICY_INFO_BLOCK))) ||
       EFI_ERROR(AmiValidateMemoryBuffer(&(InfoBlock->FlUpdPolicy), sizeof(FLASH_UPD_POLICY))) ||
       EFI_ERROR(AmiValidateMemoryBuffer(&(InfoBlock->PKpub), KeySize))
       )
//APTIOV_SERVER_OVERRIDE_END: Security Vulnerability
        return EFI_DEVICE_ERROR;

    MemCpy(&InfoBlock->FlUpdPolicy, &FlUpdatePolicy, sizeof(FLASH_UPD_POLICY));
    MemSet(&InfoBlock->PKpub, KeySize, 0xFF);
    if(gpPubKeyHndl.BlobSize < KeySize)
        KeySize = gpPubKeyHndl.BlobSize;
    MemCpy(&InfoBlock->PKpub, gpPubKeyHndl.Blob, KeySize);

    InfoBlock->ErrorCode = 0;

    return EFI_SUCCESS;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   SetFlUpdMethod
//
// Description: 
//
// Input:   
//
// Output: 
//
// Returns: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SetFlUpdMethod(
    IN OUT FUNC_FLASH_SESSION_BLOCK    *pSessionBlock
)
{
    EFI_STATUS          Status;
    UINT32              HashBlock;
    UINT32              BlockSize;
    UINT8              *BlockAddr;
    UINT32             *FSHandl;
    APTIO_FW_CAPSULE_HEADER    *pFwCapsuleHdr;

    Status = EFI_DEVICE_ERROR;

    if(EFI_ERROR(AmiValidateMemoryBuffer(pSessionBlock, sizeof(FUNC_FLASH_SESSION_BLOCK))))
        return Status;

DEBUG ((SecureMod_DEBUG_LEVEL,"SecSMI. SetFlash\npSessionBlock %X\nSize     : %X\n",pSessionBlock, pSessionBlock->FlUpdBlock.ImageSize));
#ifdef EFI_DEBUG
if(pSessionBlock->FlUpdBlock.FlashOpType == FlRecovery)
DEBUG ((SecureMod_DEBUG_LEVEL,"File Name: %a\n",pSessionBlock->FlUpdBlock.FwImage.AmiRomFileName));
else
DEBUG ((SecureMod_DEBUG_LEVEL,"Image Adr: %X\n",pSessionBlock->FlUpdBlock.FwImage.CapsuleMailboxPtr[0]));
#endif
DEBUG ((SecureMod_DEBUG_LEVEL,"ROMmap   : %X\n",pSessionBlock->FlUpdBlock.ROMSection));
DEBUG ((SecureMod_DEBUG_LEVEL,"FlOpType : %X\n",pSessionBlock->FlUpdBlock.FlashOpType));

// Verify if chosen Flash method is compatible with FlUpd Policy
    if(((pSessionBlock->FlUpdBlock.ROMSection & (1<<BOOT_BLOCK)) && (pSessionBlock->FlUpdBlock.FlashOpType & FlUpdatePolicy.BBUpdate)) || 
      (!(pSessionBlock->FlUpdBlock.ROMSection & (1<<BOOT_BLOCK)) && (pSessionBlock->FlUpdBlock.FlashOpType & FlUpdatePolicy.FlashUpdate))
    ){

DEBUG ((SecureMod_DEBUG_LEVEL,"Buff Adr : %lX\nBuff Size: %X\n",gpFwCapsuleBuffer, gFwCapMaxSize));
//!!! make sure Flash blocks BOOT_BLOCK, MAIN_, NV_ and EC_ are matching enum types in FlashUpd.h
        // Get Flash Update mode   
        switch(pSessionBlock->FlUpdBlock.FlashOpType)
        {
#if FWCAPSULE_RECOVERY_SUPPORT == 1
            case FlCapsule:
#endif
            case FlRuntime:
               //  common for FlRuntime or Capsule

                SecSmiFlash.pFwCapsule = (UINT32*)gpFwCapsuleBuffer;
                // AFU updates the address in CapsuleMailboxPtr if 
                // it's capable of allocating large buffer to load entire FW Capsule image
                if(pSessionBlock->FlUpdBlock.FwImage.CapsuleMailboxPtr[0] != 0 )
                {
                    if(EFI_ERROR(AmiValidateMemoryBuffer((VOID*)(pSessionBlock->FlUpdBlock.FwImage.CapsuleMailboxPtr[0]),
                                                          pSessionBlock->FlUpdBlock.ImageSize)))
                        break;

                    if(SecSmiFlash.pFwCapsule != NULL) {

                        if(pSessionBlock->FlUpdBlock.ImageSize > gFwCapMaxSize)
                           break; // suspecting buffer overrun

                        MemCpy((VOID*)SecSmiFlash.pFwCapsule, 
                               (VOID*)pSessionBlock->FlUpdBlock.FwImage.CapsuleMailboxPtr[0],
                                pSessionBlock->FlUpdBlock.ImageSize);

                    } else {
                        SecSmiFlash.pFwCapsule = (UINT32*)pSessionBlock->FlUpdBlock.FwImage.CapsuleMailboxPtr[0];
                    }
                } 
                // else AFU must've uploaded the image to designated SMM space using LoadFw command
DEBUG ((SecureMod_DEBUG_LEVEL,"SecSmiFlash.pFwCapsule : %X\n",SecSmiFlash.pFwCapsule));
                if(SecSmiFlash.pFwCapsule == NULL)
                    break;
                // verify we got a capsule at SecSmiFlash.pFwCapsule, update pointers to FwCapHdr and to start of a Payload
                Status = CapsuleValidate((UINT8**)&(SecSmiFlash.pFwCapsule), &pFwCapsuleHdr);
                if(EFI_ERROR(Status)) { 
                    // Ignore roll-back protection failure for Capsule upd. Err will be processed in PEI recovery
                    if (Status == EFI_INCOMPATIBLE_VERSION && pSessionBlock->FlUpdBlock.FlashOpType == FlCapsule)
                        Status = EFI_SUCCESS;
                    else 
                        break;
                }
                if(pFwCapsuleHdr == NULL)
                    break;
                // capture RomLayout from new Secure Image if it's loaded in memory and validated
                SecSmiFlash.RomLayout = (ROM_AREA *)(UINTN)((UINTN)pFwCapsuleHdr+pFwCapsuleHdr->RomLayoutOffset);
                if(pSessionBlock->FlUpdBlock.FlashOpType == FlRuntime)
                {
                    // Fill in gShaHashTbl Hash Table
                    BlockSize = FLASH_BLOCK_SIZE;
                    BlockAddr = (UINT8*)SecSmiFlash.pFwCapsule;
                    for(HashBlock = 0; HashBlock < SEC_FLASH_HASH_TBL_BLOCK_COUNT; HashBlock++) 
                    {
                        Status = gAmiSig->Hash(gAmiSig, &gEfiHashAlgorithmSha256Guid, 
                            1, &BlockAddr, (const UINTN*)&BlockSize, gHashTbl[HashBlock]); 
                        if (EFI_ERROR(Status)) break;
                        BlockAddr+= (UINTN)(BlockSize);
                    }
                    // done for Runtime Upd
                    break;
                }
            // Set Capsule EFI Var if Capsule(Verify Capsule Mailbox points to FW_CAPSULE) 
                pSessionBlock->FlUpdBlock.ImageSize = pFwCapsuleHdr->CapHdr.CapsuleImageSize;
                pSessionBlock->FlUpdBlock.FwImage.CapsuleMailboxPtr[0] = gpFwCapsuleMailbox;
#if FWCAPSULE_RECOVERY_SUPPORT == 1
                Status = UpdateCapsule (pSessionBlock, pFwCapsuleHdr);
DEBUG ((SecureMod_DEBUG_LEVEL,"SetFlash.UpdateCapsuleVar %r\n",Status));
#endif
                if(EFI_ERROR(Status)) break;
            //  common for Recovery or Capsule
            case FlRecovery:
                //  Set FlUpd EFI Var (Get MC, verify RecFileName)
                Status = SetFlashUpdateVar (pSessionBlock);
DEBUG ((SecureMod_DEBUG_LEVEL,"SetFlash.SetFlashUpdVar %r\n",Status));
                break;

            default:
                Status = EFI_DEVICE_ERROR;
        }
    }

    // Set Error Status
    if (Status != EFI_SUCCESS) { 
        SecSmiFlash.FSHandle = 0;
        SecSmiFlash.pFwCapsule = NULL;
        SecSmiFlash.RomLayout = RomLayout; // back to default RomLayout
        pSessionBlock->FSHandle  = 0;
        pSessionBlock->ErrorCode = 1;

        return EFI_DEVICE_ERROR;
    }
    // FSHandle is updated if Capsule validation passed.
    // Create FSHandle as 1st 4 bytes of gHashTbl. It must be different each time 
    //  SetMethod is called with new Image
    FSHandl = (UINT32*)gHashTbl;
    SecSmiFlash.FSHandle = *FSHandl; // should be unique per Capsule;
    pSessionBlock->FSHandle  = SecSmiFlash.FSHandle;
    pSessionBlock->ErrorCode = 0;

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   LoadFwImage
//
// Description: Routine is called in a loop by the Flash tool. 
//              Depending on the OS environment, Flash tool passes either an entire 
//              Flash Image into SMM buffer or block by block. 
//              E.g AFUDOS could allocate a contiguous buffer for the entire ROM buffer,
//              while certain OSes (Linux) may only allocate limited buffer sizes
//
// Input:       FUNC_BLOCK -> Address, size
//
// Output:      FUNC_BLOCK -> Status
//
// Returns:
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS LoadFwImage(
    IN OUT FUNC_BLOCK   *pFuncBlock
)
{
    if(gpFwCapsuleBuffer == 0 || pFuncBlock == NULL)
        return EFI_DEVICE_ERROR;

//APTIOV_SERVER_OVERRIDE_START: Security Vulnerability
   // pFuncBlock->ErrorCode = 1;
//APTIOV_SERVER_OVERRIDE_END: Security Vulnerability
    
//DEBUG ((SecureMod_DEBUG_LEVEL,"SecSMI. LoadImage at %lX\n",(UINTN)gpFwCapsuleBuffer + pFuncBlock->BlockAddr));
    if(EFI_ERROR(AmiValidateMemoryBuffer(pFuncBlock, sizeof(FUNC_BLOCK))))
        return EFI_DEVICE_ERROR;

    // prevent leaking of the SMM code to the external buffer
    if(EFI_ERROR(AmiValidateMemoryBuffer((VOID*)(pFuncBlock->BufAddr), pFuncBlock->BlockSize)))
        return EFI_DEVICE_ERROR;

// assuming the address in 0 based offset in new ROM image
    if(((UINT64)pFuncBlock->BlockAddr + pFuncBlock->BlockSize) > (UINT64)gFwCapMaxSize)
        return EFI_DEVICE_ERROR;

    SecSmiFlash.FSHandle = 0; // clear out Hndl. Will be set to valid number in SetFlashMethod
    SecSmiFlash.pFwCapsule = NULL;
    SecSmiFlash.RomLayout = RomLayout; // back to default RomLayout

    MemCpy((VOID*)((UINTN)gpFwCapsuleBuffer+pFuncBlock->BlockAddr), 
            (VOID*)pFuncBlock->BufAddr, pFuncBlock->BlockSize);

    pFuncBlock->ErrorCode = (UINT8)MemCmp(
        (VOID*)((UINTN)gpFwCapsuleBuffer+pFuncBlock->BlockAddr), 
        (VOID*)pFuncBlock->BufAddr, pFuncBlock->BlockSize)==0?0:1;

    return (pFuncBlock->ErrorCode==0)?EFI_SUCCESS:EFI_DEVICE_ERROR;
}
// End Secured Flash Update API

// <AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: BeforeSecureUpdate
//
// Description: Verifies if the Update range is protected by Signature
//              1. return Success if flash region is inside unSigned RomArea
//              2. if region is signed - compare its hash with pre-calculated Hash in smm
//                  and return pointer to internal DataBuffer
//
// Input:
//  VOID*       FlashAddress    Pointer to address of a flash 
//
// Output:      Status
//
//--------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS BeforeSecureUpdate (
    VOID* FlashAddress, UINTN Size, UINT8 **DataBuffer
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    ROM_AREA    *Area;
    UINT8       *BuffAddr;
    UINT8       *PageAddr;
    UINT8       HashCounter;
    UINTN       PageSize;
    UINTN       PageCount;
    UINT32      *FSHandl;

    // enforce write protection if RomArea undefined
    // Always use embedded RomLayout to enforce static Rom Hole locations in new ROM image
    if ( RomLayout == NULL )
        Status = EFI_WRITE_PROTECTED;

    for (Area = RomLayout; Area && Area->Size!=0; Area++)
    {
        if(Area->Address == 0) // construct an Address field if not initialized
            Area->Address = (UINT64)Flash4GBMapStart + Area->Offset;
//DEBUG ((SecureMod_DEBUG_LEVEL, "RomArea %8X(%8X) + Size %8X = %8X, Attr %X\n",Area->Address, Area->Offset, Area->Size, Area->Address+Area->Size, Area->Attributes));
        if( ((UINTN)FlashAddress >= Area->Address && 
             (UINTN)FlashAddress  < (Area->Address+Area->Size))
             ||
             (Area->Address >= (UINTN)FlashAddress && 
              Area->Address  < ((UINT64)(UINTN)FlashAddress + Size)) )
        {
//DEBUG ((SecureMod_DEBUG_LEVEL, "\nSignAttr %x(%x)\nRomArea %lX, Size %8X, (%lX)\nFlsAddr %lX, Size %8X, (%lX)\n", Area->Attributes, (Area->Attributes & ROM_AREA_FV_SIGNED),
//        Area->Address, Area->Size, Area->Address+Area->Size, 
//        (UINTN)FlashAddress, Size, (UINT64)(UINTN)FlashAddress+Size));
            if (Area->Attributes & ROM_AREA_FV_SIGNED)
            {
                Status = EFI_WRITE_PROTECTED;
                break;
            }
        }
    }
//    DEBUG ((SecureMod_DEBUG_LEVEL, "%r FlashAdddr %8lx(%8X), sz %X\n",Status, FlashAddress, (UINTN)FlashAddress-Flash4GBMapStart, Size));
//if(Status != EFI_WRITE_PROTECTED) {
//    DEBUG ((SecureMod_DEBUG_LEVEL, "Spi range %08lX : %08lX - %08lX\n", FlashAddress, (UINTN)FlashAddress-Flash4GBMapStart, (UINTN)FlashAddress-Flash4GBMapStart+Size));
//}
    if(Status == EFI_WRITE_PROTECTED &&
        (FlUpdatePolicy.FlashUpdate & FlRuntime)
    ){
        // check Verify status by comparing FSHandl with gHashTbl[0]
        // should be unique per Capsule;
        FSHandl = (UINT32*)gHashTbl;
        if(SecSmiFlash.pFwCapsule == NULL ||
           SecSmiFlash.FSHandle == 0 || 
           SecSmiFlash.FSHandle != *FSHandl)
            return Status; // EFI_WRITE_PROTECTED

        PageSize = FLASH_BLOCK_SIZE;
        PageCount=( (UINTN)FlashAddress - Flash4GBMapStart) / PageSize;
        // Flash Write -> Update ptr to internal Acpi NVS or SMM Buffer
        BuffAddr = (UINT8*)SecSmiFlash.pFwCapsule;
        PageAddr = (UINT8*)((UINTN)BuffAddr + (PageSize * PageCount));
        BuffAddr = (UINT8*)((UINTN)BuffAddr + ((UINTN)FlashAddress - Flash4GBMapStart));
//DEBUG ((SecureMod_DEBUG_LEVEL, "Page %2d, PageAddr=%x, WriteBuff=%8X(%8X), Size=%x\n", PageCount, PageAddr, BuffAddr, *((UINT32*)BuffAddr), Size));
        Status = EFI_SUCCESS;
        HashCounter = 2; // addr may rollover to next flash page
        while(HashCounter-- && PageCount < SEC_FLASH_HASH_TBL_BLOCK_COUNT)
        { 
            // compare calculated block hash with corresponding hash from the Hw Hash Table
            // if no match -> make Size=0 to skip Flash Write Op
            Status = gAmiSig->Hash(gAmiSig, &gEfiHashAlgorithmSha256Guid, 
                1, (const UINT8**)&PageAddr, (const UINTN*)&PageSize, gHashDB); 
            if(EFI_ERROR(Status) || 
                MemCmp(gHashDB, SecSmiFlash.HashTbl[PageCount], SHA256_DIGEST_SIZE)
            ){   
//DEBUG ((EFI_D_ERROR, "Hash Err!\nPage %2d, PageAddr=%x, WriteBuff=%8X(%8X), Size=%x\n", PageCount, PageAddr, BuffAddr, *((UINT32*)BuffAddr), Size));
                return EFI_WRITE_PROTECTED;
            }
            // repeat Hash check on next Flash Block if Write Block overlaps the Flash Block boundary
            PageCount++;
            PageAddr = (UINT8*)((UINTN)PageAddr + PageSize);
//DEBUG ((SecureMod_DEBUG_LEVEL, "Page %2d, PageAddr=%x\n", PageCount, PageAddr));
            if(PageAddr >= (BuffAddr+Size))
                break;
        }
        // not Erase ?
        if(DataBuffer != NULL)
            *DataBuffer = BuffAddr;
    }

    return Status;
}

// <AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: SecureFlashWrite
//
// Description: Allows to write to flash device if Secure Capsule is loaded into memory
//              Function replacing Flash->Write API call
//
// Input:       VOID* FlashAddress, UINTN Size, VOID* DataBuffer
// 
//
// Output:      EFI_SUCCESS
//
//--------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS EFIAPI SecureFlashWrite (
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
)
{
    EFI_STATUS  Status;
    UINT8       *CurrBuff = (UINT8*)DataBuffer;

    Status = BeforeSecureUpdate(FlashAddress, Size, &CurrBuff);
//    DEBUG ((SecureMod_DEBUG_LEVEL, "SecSMIFlash: Write %r, FlshAddr=%X(%X), BuffAddr=%X, OrgBuff=%X\n", Status, FlashAddress, Size, CurrBuff, DataBuffer));
    if(!EFI_ERROR(Status))
        return pFlashWrite(FlashAddress, Size, CurrBuff);

    return Status;
}

// <AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: SecureFlashupdate
//
// Description: Allows to update flash device if Secure Capsule is loaded into memory
//              Function replacing Flash->Write API call
//
// Input:       VOID* FlashAddress, UINTN Size, VOID* DataBuffer
// 
//
// Output:      EFI_SUCCESS
//
//--------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS EFIAPI SecureFlashUpdate (
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
)
{
    EFI_STATUS  Status;
    UINT8       *CurrBuff = (UINT8*)DataBuffer;

    Status = BeforeSecureUpdate(FlashAddress, Size, &CurrBuff);
//DEBUG ((SecureMod_DEBUG_LEVEL, "SecSMIFlash: Update %r, FlshAddr=%X(%X), BuffAddr=%X, OrgBuff=%X\n", Status, FlashAddress, Size, CurrBuff, DataBuffer));
    if(!EFI_ERROR(Status))
        return pFlashUpdate(FlashAddress, Size, CurrBuff);

    return Status;
}

// <AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: SecureFlashErase
//
// Description: Allows erase of flash device is Secure Capsule is loaded into memory
//              Function replacing Flash->Erase API call
//
// Input:       none
// 
//
// Output:      EFI_SUCCESS
//
//--------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS EFIAPI SecureFlashErase (
    VOID* FlashAddress, UINTN Size
)
{
    EFI_STATUS  Status;

    Status = BeforeSecureUpdate(FlashAddress, Size, NULL);
//DEBUG ((SecureMod_DEBUG_LEVEL, "SecSMIFlash Erase %r, FlshAddr=%X(%X)\n", Status, FlashAddress, Size));
    if(!EFI_ERROR(Status)) 
        return pFlashErase(FlashAddress, Size);

    return Status;
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  GetFwCapFfs
//
// Description:    Loads binary from RAW section of X firmware volume
//
//  Input:
//               NameGuid  - The guid of binary file
//               Buffer    - Returns a pointer to allocated memory. Caller must free it when done.
//               Size      - Returns the size of the binary loaded into the buffer.
//
// Output:       Buffer - returns a pointer to allocated memory. Caller must free it when done.
//               Size   - returns the size of the binary loaded into the buffer.
//               EFI_NOT_FOUND  - Can't find the binary.
//               EFI_LOAD_ERROR - Load fail.
//               EFI_SUCCESS    - Load success.
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
GetFwCapFfs (
  IN      EFI_GUID       *NameGuid,
  IN OUT  VOID           **Buffer,
  IN OUT  UINTN          *Size
  )
{
  EFI_STATUS                    Status;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
  EFI_HANDLE                    *HandleBuff;
  UINT32                        AuthenticationStatus;

 *Buffer=0;
 *Size=0;
  Status = pBS->LocateHandleBuffer (ByProtocol,&gEfiFirmwareVolume2ProtocolGuid,NULL,&HandleCount,&HandleBuff);
  if (EFI_ERROR (Status) || HandleCount == 0) {
    return EFI_NOT_FOUND;
  }
  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = pBS->HandleProtocol (HandleBuff[Index],&gEfiFirmwareVolume2ProtocolGuid,&Fv);

    if (EFI_ERROR (Status)) {
       continue;//return EFI_LOAD_ERROR;
    }
    //
    // Try a raw file
    //
    Status = Fv->ReadSection (
                  Fv,
                  NameGuid, //&gFwCapFfsGuid,
                  EFI_SECTION_FREEFORM_SUBTYPE_GUID,//EFI_SECTION_RAW
                  0,    //Instance
                  Buffer,
                  Size,
                  &AuthenticationStatus
                  );

    if (Status == EFI_SUCCESS) break;
  }

  pBS->FreePool(HandleBuff);

  if (Index >= HandleCount) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

// <AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: GetRomLayout
//
// Description: 
//
// Input:
//  IN EFI_HANDLE               ImageHandle     Image Handle
//  IN EFI_SYSTEM_TABLE         *SystemTable    Pointer to System Table
//
// Output:      EFI_STATUS
//
//--------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS GetRomLayout(
    IN EFI_SYSTEM_TABLE    *SystemTable,
    OUT ROM_AREA **RomLayout
)
{
    EFI_STATUS Status;
    static EFI_GUID HobListGuid = HOB_LIST_GUID;
    static EFI_GUID AmiRomLayoutHobGuid = AMI_ROM_LAYOUT_HOB_GUID;
    ROM_LAYOUT_HOB *RomLayoutHob;
    UINTN RomLayoutSize=0, Size;
    ROM_AREA *Area;
    APTIO_FW_CAPSULE_HEADER *FwCapHdr;
    UINT8*  pFwCapHdr=NULL;

// 1. Try to locate RomLayout from embedded CapHdr Ffs 
    Status = GetFwCapFfs(&gFwCapFfsGuid, &pFwCapHdr, &Size);
    if(!EFI_ERROR(Status)) 
    {
        // Skip over Section GUID
        FwCapHdr = (APTIO_FW_CAPSULE_HEADER*)((UINTN)pFwCapHdr + sizeof (EFI_GUID));
        Size -= sizeof (EFI_GUID);
        *RomLayout = (ROM_AREA *)((UINTN)FwCapHdr+FwCapHdr->RomLayoutOffset);
        RomLayoutSize = sizeof(ROM_AREA);
        for (Area=*RomLayout; 
             Area->Size!=0 && RomLayoutSize<=(Size - FwCapHdr->RomLayoutOffset); 
             Area++)
        {
            DEBUG ((SecureMod_DEBUG_LEVEL, "%08X...%08lX, offs %08X, size 0x%08X, attr %X\n",Area->Address, (Area->Address+Area->Size-1), Area->Offset, Area->Size, Area->Attributes));
            RomLayoutSize += sizeof(ROM_AREA);
        }
        Area=*RomLayout; 
        DEBUG ((SecureMod_DEBUG_LEVEL, "Use Rom Map from FwCapHdr FFS at %X(size 0x%X), map offs %X(size 0x%X)\n", FwCapHdr, Size, FwCapHdr->RomLayoutOffset, RomLayoutSize));
    }
    else
    {
// 2. Backup: Use primary RomLayout from Rom Layout HOB. 
//  This one does not yet report the Rom Hole regions
    //----- Get HobList -------------------------------------
        RomLayoutHob = GetEfiConfigurationTable(SystemTable, &HobListGuid);
        if (RomLayoutHob!=NULL)
        {
    // -------- Get RomLayoutHob ----------------------
        	Status = FindNextHobByGuid((EFI_GUID *)&AmiRomLayoutHobGuid, (VOID **)&RomLayoutHob);
            if (!EFI_ERROR(Status))
            {
                RomLayoutSize =   RomLayoutHob->Header.Header.HobLength
                                  - sizeof(ROM_LAYOUT_HOB);

                Area=(ROM_AREA*)(RomLayoutHob+1);
                DEBUG ((SecureMod_DEBUG_LEVEL, "Use Default Rom Map from the Hob at %X(size 0x%X)\n", Area, RomLayoutSize));
            }
        }
    }
    if(RomLayoutSize)
    {
        //---Allocate memory in SMRAM for RomLayout---
        *RomLayout = NULL;
        Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, RomLayoutSize,(void **)RomLayout);
        if (EFI_ERROR(Status) || *RomLayout == NULL)
            return EFI_NOT_FOUND;

        pBS->CopyMem( *RomLayout, Area, RomLayoutSize);
    
        if(pFwCapHdr)
            pBS->FreePool(pFwCapHdr);

        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}

// !!! do not install if OFBD SecFlash is installed 
#if INSTALL_SECURE_FLASH_SW_SMI_HNDL == 1
//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   SecSMIFlashSMIHandler
//
// Description:
//
// Input:
//
// Output:
//
// Returns:
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
SecSMIFlashSMIHandler (
    IN  EFI_HANDLE                  DispatchHandle,
        IN CONST VOID               *Context OPTIONAL,
        IN OUT VOID                 *CommBuffer OPTIONAL,
        IN OUT UINTN                *CommBufferSize OPTIONAL
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8        Data;
    UINT64       pCommBuff;
    UINT32       HighBufferAddress = 0;
    UINT32       LowBufferAddress = 0;
    UINTN       Cpu;

    Cpu = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->SwSmiCpuIndex;
    //
    // Found Invalid CPU number, return
    //
    if(Cpu == (UINTN)-1) RETURN(Status);

    gSmmCpu->ReadSaveState ( gSmmCpu, \
                                      4, \
                                      EFI_SMM_SAVE_STATE_REGISTER_RBX, \
                                      Cpu, \
                                      &LowBufferAddress );
    gSmmCpu->ReadSaveState ( gSmmCpu, \
                                      4, \

                                      EFI_SMM_SAVE_STATE_REGISTER_RCX, \
                                      Cpu, \
                                      &HighBufferAddress );

    Data = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->CommandPort;

    pCommBuff            = HighBufferAddress;
    pCommBuff            = Shl64(pCommBuff, 32);
    pCommBuff            += LowBufferAddress;

//DEBUG ((SecureMod_DEBUG_LEVEL, "Sec SW SMI Flash Hook == 0x%x\n", Data));

    switch(Data)
    {
        case SecSMIflash_Load:             // 0x1d Send Flash Block to memory
            Status = LoadFwImage((FUNC_BLOCK *)pCommBuff);
            break;

        case SecSMIflash_GetPolicy:        // 0x1e Get Fl Upd Policy 
            Status = GetFlUpdPolicy((FLASH_POLICY_INFO_BLOCK *)pCommBuff);
            break;
        
        case SecSMIflash_SetFlash:        // 0x1f Set Flash method
            Status = SetFlUpdMethod((FUNC_FLASH_SESSION_BLOCK *)pCommBuff);
            break;
    }
//DEBUG ((SecureMod_DEBUG_LEVEL,"Exit SW SMI with %r\n", Status));
    RETURN(Status);
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   InSmmFunction
//
// Description: 
//
// Input:   
//
// Output: 
//
// Returns: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS InSmmFunction(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
#if INSTALL_SECURE_FLASH_SW_SMI_HNDL == 1
    EFI_SMM_SW_DISPATCH2_PROTOCOL    *pSwDispatch = NULL;
    EFI_SMM_SW_REGISTER_CONTEXT      SwContext;
    UINTN                            Index;
    UINT8   MinSMIPort = SecSMIflash_Load;    //0x1d
    //UINT8   MinSMIPort = SecSMIflash_GetPolicy; //0x1e;
    UINT8   MaxSMIPort = SecSMIflash_SetFlash; //0x1f;
#endif
#if FWCAPSULE_RECOVERY_SUPPORT == 1
const EFI_SMM_SX_REGISTER_CONTEXT      SxRegisterContext = {SxS5, SxEntry};
      EFI_SMM_SX_DISPATCH2_PROTOCOL    *SxDispatchProtocol;
#if CSLIB_WARM_RESET_SUPPORTED == 0
      static EFI_GUID guidHob = HOB_LIST_GUID;
      EFI_HOB_HANDOFF_INFO_TABLE *pHit;
#endif
#endif

    EFI_STATUS              Status;
    EFI_HANDLE              Handle = NULL;
    EFI_HANDLE              DummyHandle = NULL;
    UINTN                   Size;
    VOID                   *Memory = NULL;

    InitAmiSmmLib( ImageHandle, SystemTable );
    
#if defined(CSLIB_WARM_RESET_SUPPORTED) 
    #if FWCAPSULE_RECOVERY_SUPPORT == 1 && CSLIB_WARM_RESET_SUPPORTED == 0
    //Get Boot Mode
    pHit = GetEfiConfigurationTable(pST, &guidHob);
    if (pHit && pHit->BootMode == BOOT_ON_FLASH_UPDATE) {
        if( (ReadRtcIndexedRegister(0xB) & ( 1 << 5 )) == ( 1 << 5 )) {
            // ========== INTEL CHIPSET PORTING ========================
            //clear RTC_STS bit in PM1_STS if resuming from RTC alarm on flash update
            IoWrite16(PM_BASE_ADDRESS, ( IoRead16(PM_BASE_ADDRESS) | (1 << 10) ));
            //disable the RTC alarm
            WriteRtcIndexedRegister(0xB, ( ReadRtcIndexedRegister(0xB) & ~( 1 << 5 ) ));
        }
    }
#endif
#endif
    
    Status = pSmst->SmmLocateProtocol(&gAmiSmmDigitalSignatureProtocolGuid, NULL, &gAmiSig);
    if (EFI_ERROR(Status)) return Status;

// Test if Root Platform Key is available,else - don't install Flash Upd security measures.
    gpPubKeyHndl.Blob = NULL;
    gpPubKeyHndl.BlobSize = 0;
    Status = gAmiSig->GetKey(gAmiSig, &gpPubKeyHndl, &gPRKeyGuid, gpPubKeyHndl.BlobSize, 0);
DEBUG ((SecureMod_DEBUG_LEVEL,"GetKey %r (%X, 0x%lx bytes)\n",Status, gpPubKeyHndl.Blob, gpPubKeyHndl.BlobSize));
    if (EFI_ERROR(Status) || gpPubKeyHndl.Blob == NULL) {
        if(Status == EFI_BUFFER_TOO_SMALL) 
            return EFI_SUCCESS;
        return Status;
    }
    //
    // Allocate scratch buffer to hold entire Signed BIOS image for Secure Capsule and Runtime Flash Updates
    // AFU would have to execute a sequence of SW SMI calls to push entire BIOS image to SMM
    //
    //NEW_BIOS_MEM_ALLOC == 2 AFU will allocate a buffer and provide pointer via SET_FLASH_METHOD API call. 
    //
#if NEW_BIOS_MEM_ALLOC < 2
#if NEW_BIOS_MEM_ALLOC == 0
    //
    // Alternatively the buffer may be reserved within the SMM TSEG memory 
    //
    Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, gFwCapMaxSize, &Memory);
    gpFwCapsuleBuffer = (EFI_PHYSICAL_ADDRESS)(UINTN)Memory;
#endif
#if NEW_BIOS_MEM_ALLOC == 1
    //
    // The buffer is allocated anywhere within reserved system memory
    //
//    Status = pST->BootServices->AllocatePool(EfiReservedMemoryType, gFwCapMaxSize, &gpFwCapsuleBuffer);
    Status = pST->BootServices->AllocatePages(AllocateAnyPages, EfiReservedMemoryType, EFI_SIZE_TO_PAGES(gFwCapMaxSize), &gpFwCapsuleBuffer);
#endif
    DEBUG ((SecureMod_DEBUG_LEVEL,"Allocate FwCapsuleBuffer %lX,0x%X %r\n",gpFwCapsuleBuffer, gFwCapMaxSize, Status));
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    MemSet((void*)gpFwCapsuleBuffer, gFwCapMaxSize, 0 );
#endif
    //
    // Allocate space to hold a Hash table for all Flash blocks
    //
    Size = SEC_FLASH_HASH_TBL_SIZE;
    Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, Size, (void**)&gHashTbl);
    DEBUG ((SecureMod_DEBUG_LEVEL,"AllocateHashTbl HashTbl Pool %lx(0x%x) %r\n",gHashTbl, Size, Status));
    if (EFI_ERROR(Status)) return Status;
    MemSet((void*)gHashTbl, Size, 0xdb );

#if FWCAPSULE_RECOVERY_SUPPORT == 0
    FlUpdatePolicy.FlashUpdate &=~FlCapsule;
    FlUpdatePolicy.BBUpdate &=~FlCapsule;
#else    
    //
    // Reserve pool in smm runtime memory for capsule's mailbox list
    //
    gpFwCapsuleMailboxSize = 4*sizeof(EFI_CAPSULE_BLOCK_DESCRIPTOR) + sizeof(EFI_CAPSULE_HEADER); // (4*16)+28
    Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, gpFwCapsuleMailboxSize, &Memory);
    gpFwCapsuleMailbox = (EFI_PHYSICAL_ADDRESS)(UINTN)Memory;
    DEBUG ((SecureMod_DEBUG_LEVEL,"Allocate FwCapsuleMailbox %lx(0x%x) %r\n",gpFwCapsuleMailbox,Size, Status));
    if (EFI_ERROR(Status)) return Status;
    MemSet((void*)gpFwCapsuleMailbox, gpFwCapsuleMailboxSize, 0 );

    //
    // Install callback on S5 Sleep Type SMI. Needed to transition to S3 if Capsule's mailbox is pending
    // Locate the Sx Dispatch Protocol
    //
    // gEfiSmmSxDispatch2ProtocolGuid
    Status = pSmst->SmmLocateProtocol( &gEfiSmmSxDispatch2ProtocolGuid, NULL, &SxDispatchProtocol);
    ASSERT_EFI_ERROR (Status);  
    if (EFI_ERROR(Status)) return Status;
    //
    // Register the callback for S5 entry
    //
    if (SxDispatchProtocol && SupportUpdateCapsuleReset()) {
      Status = SxDispatchProtocol->Register (
                    SxDispatchProtocol,
                    (EFI_SMM_HANDLER_ENTRY_POINT2)SmiS5CapsuleCallback,
                    &SxRegisterContext,
                    &Handle
                    );
      ASSERT_EFI_ERROR (Status);  
    }
    if (EFI_ERROR(Status)) goto Done;
#endif
/*
AFU For Rom Holes in Runtime/Capsule upd
    1. Read full ROM image to ROM buffer
    2. Merge Rom Hole from input file to ROM buffer
    3. call "LoadImage" for full BIOS
    3. call "SetFlash" with Runtime update (NVRAM block should be unsigned!!!)
    4. calls to upd  Rom hole -erase,write should pass
*/
    Status = GetRomLayout(SystemTable, &RomLayout);
DEBUG ((SecureMod_DEBUG_LEVEL,"Smm Rom Layout at %X, %r\n",RomLayout, Status));
    // Rom Layout HOB may not be found in Recovery mode and if FW does not include built in FwCapsule Hdr file
//    if (EFI_ERROR(Status)) goto Done;
    //
    // Trap the original Flash Driver API calls to enforce 
    // Flash Write protection in SMM at the driver API level
    //
	Status = pSmst->SmmLocateProtocol( &gFlashSmmProtocolGuid, NULL, &Flash);
    if (EFI_ERROR(Status)) 
        Status = pBS->LocateProtocol(&gFlashSmmProtocolGuid, NULL, &Flash);
DEBUG ((SecureMod_DEBUG_LEVEL,"Flash->Write API fixup %X->%X(->%X)\n", Flash, Flash->Write,SecureFlashWrite));
    if (EFI_ERROR(Status)) goto Done;

    // preserve org Flash API
    pFlashWrite = Flash->Write; 
    pFlashUpdate = Flash->Update;
    pFlashErase = Flash->Erase;
    // replace with local functions 
    Flash->Write = SecureFlashWrite;
    Flash->Update= SecureFlashUpdate;
    Flash->Erase = SecureFlashErase;
    //
    // Install Secure SMI Flash Protocol 
    //
    SecSmiFlash.pFwCapsule = (UINT32*)gpFwCapsuleBuffer;
    SecSmiFlash.HashTbl = gHashTbl;
    SecSmiFlash.RomLayout = RomLayout;
    SecSmiFlash.FSHandle = 0;
    Status = pSmst->SmmInstallProtocolInterface(
                 &DummyHandle,
                 &gSecureSMIFlashProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &SecSmiFlash
             );
    ASSERT_EFI_ERROR(Status);
//    if (EFI_ERROR(Status)) return Status;
    if (EFI_ERROR(Status)) goto Done;

    //    
    // Install SW SMI callbacks for 3 SecSMI Flash functions
    // !!! do not install if OFBD SecFlash is installed 
    //
#if INSTALL_SECURE_FLASH_SW_SMI_HNDL == 1

    Status = pSmst->SmmLocateProtocol( \
                        &gEfiSmmSwDispatch2ProtocolGuid, NULL, &pSwDispatch);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;


    Status = pSmst->SmmLocateProtocol(&gEfiSmmCpuProtocolGuid, NULL, &gSmmCpu);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    for(Index=MinSMIPort;Index<=MaxSMIPort;Index++)
    {
        SwContext.SwSmiInputValue    = Index;
        Status    = pSwDispatch->Register(pSwDispatch, SecSMIFlashSMIHandler, &SwContext, &Handle);
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR(Status)) break;
        //If any errors,unregister any registered SwSMI by this driver.
        //If error, and driver is unloaded, then a serious problem would exist.
    }
#endif
Done:
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   SecSMIFlashDriverEntryPoint
//
// Description: Secure SMI Flash driver init 
//
// Input:   
//
// Output: 
//
// Returns: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI SecSMIFlashDriverEntryPoint(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE    *SystemTable
)
{
    InitAmiLib(ImageHandle, SystemTable);
    return InitSmmHandlerEx(ImageHandle, SystemTable, InSmmFunction, NULL);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
