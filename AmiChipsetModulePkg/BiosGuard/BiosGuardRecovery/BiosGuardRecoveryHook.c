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

#include <Pei.h>
#include <AmiPeiLib.h>
#include <token.h>
#include <Library/DebugLib.h>
#include <AmiCspLib.h>
#include <Cpu/CpuRegs.h>
#include <Ppi/BlockIo.h>

typedef struct
{
    CHAR8  FileName[11];
    UINT8  DirAttr;
    UINT8  Unused[2];        //NTRes, CrtTimeTenth;
    UINT16 Unused1[3];       //CrtTime, CrtDate, LstAccDate;
    UINT16 FirstClusterHi;
    UINT16 Unused2[2];       //WrtTime, WrtDate;
    UINT16 FirstClusterLo;
    UINT32 FileSize;
} DIR_ENTRY;

typedef struct _RC_VOL_INFO
{
    UINT32                        BlockSize;
    EFI_PEI_RECOVERY_BLOCK_IO_PPI *BlkIo;
    UINTN                         Device;
    UINT8                         FatType;
    UINT8                         *FatPointer;
    DIR_ENTRY                     *RootPointer;
    UINT64                        PartitionOffset;
    UINT64                        FatOffset;
    UINT64                        RootOffset;
    UINT64                        DataOffset;
    UINT32                        BytesPerCluster;
} RC_VOL_INFO;


/**
    When BiosGuard is enabled, recovery size need to change to BIOSGUARD_RECOVERY_IMAGE_SIZE.
               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FileName - recovery capsule file name
    @param FileSize - pointer to size of provided buffer
    @param Buffer - pointer to buffer to store data


    @retval EFI_STATUS

**/
EFI_STATUS 
BiosGuardRecoveryHook (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN OUT UINTN         *FileSize,
    OUT VOID             *Buffer 
)
{
    if ((AsmReadMsr64 (EFI_PLATFORM_INFORMATION) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
            (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) {
        // Bios Guard is enabled
        DEBUG((EFI_D_INFO, "BiosGuardRecoveryHook.c : Modify recovery file size from 0x%x to 0x%x\n", *FileSize, BIOSGUARD_RECOVERY_IMAGE_SIZE));
        *FileSize = BIOSGUARD_RECOVERY_IMAGE_SIZE;
    }
    return EFI_NOT_FOUND;
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
