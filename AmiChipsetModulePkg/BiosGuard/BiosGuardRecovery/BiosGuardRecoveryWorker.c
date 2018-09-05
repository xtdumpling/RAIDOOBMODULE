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

// Includes
#include <AmiDxeLib.h>
#include <Hob.h>
#include <AmiHobs.h>
#include <token.h>
#include <BiosGuardDefinitions.h>
#include <../BiosGuardNvs/BiosGuardNvs.h>
#include <AmiCspLib.h>
#include <Cpu/CpuRegs.h>
#include "BiosGuardRecoveryMapElink.h"

#define FLASH_DEVICE_BASE        (0xFFFFFFFF - (BIOSGUARD_RECOVERY_IMAGE_SIZE & 0xFFF00000) + 1)
#define FLASH_BIOS_BASE        (0xFFFFFFFF - (FLASH_SIZE) + 1)
#define BIOSGUARD_PACKAGE_CERT   524

//----------------------------------------------------------------------------
// Function Externs
typedef enum {
    FvTypeMain,
    FvTypeBootBlock,
    FvTypeNvRam,
    FvTypeBootBlockBackup,
    FvTypeCustom,
    FvTypeMax
} FLASH_FV_TYPE;

typedef struct _FLASH_AREA_EX FLASH_AREA_EX;

typedef EFI_STATUS (REFLASH_FUNCTION_EX)(
    IN FLASH_AREA_EX *Block,
    IN UINTN BlockNumber
);

struct _FLASH_AREA_EX
{
    UINT8 *BlockAddress;                //!< Address of the area in flash
    UINT8 *BackUpAddress;               //!< Backup address of the area in flash
    UINT32 RomFileOffset;               //!< Offset of the area in ROM file
    UINTN Size;                         //!< Size of the area
    UINT32 BlockSize;                   //!< Size of the area block
    FLASH_FV_TYPE Type;                 //!< Type of the area
    BOOLEAN Update;                     //!< Update area flag
    BOOLEAN TopSwapTrigger;             //!< TOP_SWAP trigger flag
    REFLASH_FUNCTION_EX *BackUp;        //!< Area backup function
    REFLASH_FUNCTION_EX *Program;       //!< Area flash programm function
    EFI_STRING_ID BackUpStringId;       //!< Area backup message ID
    EFI_STRING_ID ProgramStringId;      //!< Area flash message ID
};

extern FLASH_AREA_EX *BlocksToUpdate;

typedef struct {
    UINT64    BiosGuardMemAddress;
    UINT32    BiosGuardMemSize;
    UINT16    BiosGuardIoTrapAddress;
    UINT64    BgupCertificate;
} BIOS_GUARD_INFO;

typedef struct {
    CHAR8     *Name;
    UINT8     Type;
    UINT8     NumOfBlocks;
    UINT8     BlockIndex;
    UINT32    BlockAddress;
} BIOS_GUARD_DESC_INFO;

//----------------------------------------------------------------------------
// Local Variables
EFI_GUID                           gAmiBiosGuardNvsAreaProtocolGuid = AMI_BIOS_GUARD_NVS_AREA_PROTOCOL_GUID;
EFI_PHYSICAL_ADDRESS               gBiosGuardRecoveryImage = 0;
AMI_BIOS_GUARD_NVS_AREA_PROTOCOL   *AmiBiosGuardNvsProtocol;
BIOS_GUARD_INFO                    BiosGuardInfo = {0,0,0,0};
EFI_HOB_HANDOFF_INFO_TABLE         *HobList;
BIOS_GUARD_DESC_INFO BiosGuardRecoveryMap[] = {
    BIOS_GUARD_RECOVERY_MAP_LIST
    {NULL,       0xff, 0xff, 0xff,  -1}
};

UINT32  FlashAddressBase = FLASH_DEVICE_BASE;
//----------------------------------------------------------------------------
// Function Definitions

UINT32
StringLength (
    UINT8 *string
)
{
    UINT32 length = 0;

    while(*(UINT16*)string != 0xa0d) {
        string++;
        length++;
    };
    return length + 2;
}

EFI_STATUS
GetBiosGuardBlockAddress (
    IN  UINT8   BlockIndex,
    OUT UINT32  *BlockAddress,
    OUT UINT32  *Source
)
{
    BGUP_HEADER             *BgupHdr;
    UINT8                   *BiosGuardImage;
    UINT32                  i = 0, j =0, Index = 0;

    BiosGuardImage = (UINT8*)(gBiosGuardRecoveryImage + *(UINT32*)gBiosGuardRecoveryImage);
    BgupHdr = (BGUP_HEADER*)BiosGuardImage;

    do {
        if ( BlockIndex == Index++ ) {
            *Source = (UINT32)(BiosGuardImage + i);
            *BlockAddress = (FlashAddressBase + j);
            return EFI_SUCCESS;
        }
        // Skip ScriptSection and BGUP Header
        i += (BgupHdr->ScriptSectionSize + sizeof(BGUP_HEADER));
        // Pointer to next BGUP block.
        i += (BgupHdr->DataSectionSize + BIOSGUARD_PACKAGE_CERT);
        j += BgupHdr->DataSectionSize;
        BgupHdr = (BGUP_HEADER*)(BiosGuardImage + i);
    } while (i < BIOSGUARD_RECOVERY_IMAGE_SIZE);

    return EFI_NOT_FOUND;
}

EFI_STATUS
BiosGuardVariablesInitialize (
    VOID
)
{
    UINT8       AmiBiosGuardSign[] = "_AMIPFAT", *RecoveryRomAddress = (UINT8*)gBiosGuardRecoveryImage;
    UINT32      i, j, AmiBiosGuardHeaderLength, Index, Source;
    EFI_STATUS  Status;

    //
    // Get AMI BiosGuard Global NVA Area from BiosGuard protocol.
    //
    Status = pBS->LocateProtocol (&gAmiBiosGuardNvsAreaProtocolGuid, NULL, &AmiBiosGuardNvsProtocol);
    if( EFI_ERROR(Status) ) {
        ASSERT_EFI_ERROR (Status);
        return Status;
    }

    //
    // Init BiosGuard relating variables.
    //
    BiosGuardInfo.BiosGuardMemAddress = AmiBiosGuardNvsProtocol->Area->AmiBiosGuardMemAddress;
    BiosGuardInfo.BiosGuardMemSize = AmiBiosGuardNvsProtocol->Area->AmiBiosGuardMemSize << 20;
    BiosGuardInfo.BiosGuardIoTrapAddress = AmiBiosGuardNvsProtocol->Area->AmiBiosGuardIoTrapAddress;
    BiosGuardInfo.BgupCertificate = BiosGuardInfo.BiosGuardMemAddress + BiosGuardInfo.BiosGuardMemSize - BGUPC_MEMORY_OFFSET;

    //
    // Verify AMI BIOS GUARD Header.
    //
    if (MemCmp (RecoveryRomAddress + 8, AmiBiosGuardSign, 8)) return EFI_INVALID_PARAMETER;
    AmiBiosGuardHeaderLength = *(UINT32*)RecoveryRomAddress;

    for (i = 0, Index = 0; BiosGuardRecoveryMap[i].Name != NULL; i++) {
        //
        // Search Description Strings in AMI BiosGuard Header
        //
        for (j = StringLength(RecoveryRomAddress + sizeof(UINT32)); j < AmiBiosGuardHeaderLength; j++ ) {
#if BIOS_GUARD_DEBUG_MODE
            DEBUG ((EFI_D_INFO, "Current data = %X \n", *(RecoveryRomAddress + j) ));
            DEBUG ((EFI_D_INFO, "Offset = %X \n", j ));
            DEBUG ((EFI_D_INFO, "Name = %s \n", BiosGuardRecoveryMap[i].Name ));
#endif
            if ( MemCmp (RecoveryRomAddress + j, BiosGuardRecoveryMap[i].Name, (UINT32)Strlen(BiosGuardRecoveryMap[i].Name) ) ||
       	         (*(RecoveryRomAddress + j + Strlen(BiosGuardRecoveryMap[i].Name)) != 0x20) ) continue;
            
            // Skip space char.
            for (j += (UINT32)Strlen(BiosGuardRecoveryMap[i].Name); *(RecoveryRomAddress + j) == 0x20; j++);
#if BIOS_GUARD_DEBUG_MODE
            DEBUG ((EFI_D_INFO, "*Final = %X \n", *(RecoveryRomAddress + j) ));
#endif
            // Record each block information.
            BiosGuardRecoveryMap[i].BlockIndex = Index;
            BiosGuardRecoveryMap[i].NumOfBlocks = (UINT8)Atoi(RecoveryRomAddress + j);
#if BIOS_GUARD_DEBUG_MODE            
            DEBUG ((EFI_D_INFO, "BiosGuardRecoveryMap[%d].NumOfBlocks = %d \n", i , BiosGuardRecoveryMap[i].NumOfBlocks));
#endif
            GetBiosGuardBlockAddress (Index, (UINT32*)&BiosGuardRecoveryMap[i].BlockAddress, (UINT32*)&Source);
#if BIOS_GUARD_DEBUG_MODE    	    
            DEBUG ((EFI_D_INFO, "Index = %d \n", Index));            
            DEBUG ((EFI_D_INFO, "BiosGuardRecoveryMap[%d].BlockAddress = %X \n", i , BiosGuardRecoveryMap[i].BlockAddress));
#endif
            Index += BiosGuardRecoveryMap[i].NumOfBlocks;
            break;
        }
    }
    return EFI_SUCCESS;
}

EFI_STATUS
BiosGuardFlashProgram(
    IN FLASH_AREA_EX *Block,
    IN UINTN BlockNumber
)
{
    UINT32           i, BlockAddress, Source, BiosGuardDataLength;
    BGUP_HEADER      *BgupHdr;

    for (i = 0; BiosGuardRecoveryMap[i].Name != NULL; i++) {
        if ((UINT32)Block->BlockAddress != BiosGuardRecoveryMap[i].BlockAddress) continue;
        GetBiosGuardBlockAddress (BiosGuardRecoveryMap[i].BlockIndex + (UINT8)BlockNumber, (UINT32*)&BlockAddress, (UINT32*)&Source);
        BgupHdr = (BGUP_HEADER*)Source;
        BiosGuardDataLength = BgupHdr->ScriptSectionSize + BgupHdr->DataSectionSize + sizeof(BGUP_HEADER);

        //
        // Moved ScriptSection and DataSection to BiosGuard memory.
        //
        pBS->CopyMem((VOID*)BiosGuardInfo.BiosGuardMemAddress, (VOID*)Source, BiosGuardDataLength);

        //
        // Moved Certificate Section to BiosGuard Certificate memory.
        //
        pBS->CopyMem((VOID*)BiosGuardInfo.BgupCertificate, (VOID*)(Source + BiosGuardDataLength), BIOSGUARD_PACKAGE_CERT);

        //
        // Trigger BiosGuard update.
        //
        IoWrite8 (BiosGuardInfo.BiosGuardIoTrapAddress, 0);

        //
        // Check BiosGuard status.
        //
        if ( HobList->BootMode == BOOT_IN_RECOVERY_MODE ) {
            if (*(UINT32*)BiosGuardInfo.BiosGuardMemAddress != 0) {
                ASSERT_EFI_ERROR (EFI_ACCESS_DENIED);        	    
                return EFI_DEVICE_ERROR;
            }
        } else if ( HobList->BootMode == BOOT_ON_FLASH_UPDATE ) {
            if (*(UINT32*)BiosGuardInfo.BgupCertificate != 0) {
                ASSERT_EFI_ERROR (EFI_ACCESS_DENIED);        	    
                return EFI_DEVICE_ERROR;
            }
        }
    }
    return EFI_SUCCESS;

}

VOID
BiosGuardRecoveryBeforeFlash (
    VIOD
)
{
    UINT8                             i, j;
    EFI_STATUS                        Status;
    EFI_HOB_HANDOFF_INFO_TABLE        *HobList;
    EFI_GUID                          AmiBiosGuardRecoveryImageHobGuid = AMI_BIOSGUARD_RECOVERY_IMAGE_HOB_GUID;

    if ( !((AsmReadMsr64 (EFI_PLATFORM_INFORMATION) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
            (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) ) {
        //
        // BIOS Guard is disabled or not supported
        //
        DEBUG ((EFI_D_INFO, "BiosGuardRecoveryWorker.c : BIOS Guard is disabled or not supported\n"));
        return;
    }

    HobList = GetEfiConfigurationTable(pST, &gEfiHobListGuid);

    //
    // Get BiosGuard Image from BiosGuard HOB
    //
    Status = FindNextHobByGuid(&AmiBiosGuardRecoveryImageHobGuid, &HobList);
    if( EFI_ERROR(Status) ) {
        ASSERT_EFI_ERROR (Status);
        return;
    }

    gBiosGuardRecoveryImage = ((RECOVERY_IMAGE_HOB*)HobList)->Address;

#if defined (BIOS_GUARD_MEUD_SUPPORT) && (BIOS_GUARD_MEUD_SUPPORT == 1)    
    if (((RECOVERY_IMAGE_HOB*)HobList)->ImageSize == (BIOSGUARD_MEUD_BIOS_SIZE + FV_MEFW_CAPSULE_SIZE)) {
        FlashAddressBase = FLASH_BIOS_BASE;
    }
#endif    

    //
    // Initialize BiosGuard Variables
    //
    Status = BiosGuardVariablesInitialize();
    if( EFI_ERROR(Status) ) {
        ASSERT_EFI_ERROR (Status);
        return;
    }

    //
    // Update BiosGuard Block to BlockToUpdate structure.
    //
    for(i = 0; BlocksToUpdate[i].Type != FvTypeMax; i++) {
        for (j = 0; BiosGuardRecoveryMap[j].Name != NULL; j++) {
            // Find BlocksToUpdate by BlockAddress & Type
#if BIOS_GUARD_DEBUG_MODE
            DEBUG ((EFI_D_INFO, "\nBiosGuardRecoveryMap[%d].BlockAddress = %X \n", j,BiosGuardRecoveryMap[j].BlockAddress));
            DEBUG ((EFI_D_INFO, "BiosGuardRecoveryMap[%d].Type = %X \n", j , BiosGuardRecoveryMap[j].Type));
        	
            DEBUG ((EFI_D_INFO, "BlocksToUpdate[%d].BlockAddress = %X \n",i,  BlocksToUpdate[i].BlockAddress));
            DEBUG ((EFI_D_INFO, "BlocksToUpdate[%d].Type = %X \n", i, BlocksToUpdate[i].Type));
#endif
        	
            if ((BiosGuardRecoveryMap[j].BlockAddress == (UINT32)BlocksToUpdate[i].BlockAddress) && \
                (BiosGuardRecoveryMap[j].Type == BlocksToUpdate[i].Type) ) {
#if BIOS_GUARD_DEBUG_MODE
                DEBUG ((EFI_D_INFO, "Found.BlockAddress = %X \n", BiosGuardRecoveryMap[j].BlockAddress));
                DEBUG ((EFI_D_INFO, "Found.Type = %X \n", BiosGuardRecoveryMap[j].Type));        	
#endif
        	
                // Number of BiosGuard Section to be updated.
                BlocksToUpdate[i].Size = BiosGuardRecoveryMap[j].NumOfBlocks;
                // BiosGuard Section unit is 1
                BlocksToUpdate[i].BlockSize = 1;
                // Redirect AmiFlashProgram to BiosGuardFlashProgram.
                BlocksToUpdate[i].Program = (REFLASH_FUNCTION_EX*)BiosGuardFlashProgram;
            }
        }
    }
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
