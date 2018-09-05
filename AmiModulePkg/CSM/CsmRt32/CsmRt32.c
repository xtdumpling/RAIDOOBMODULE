//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name: rt32.c
//
// Description:	This is the source of PCI 32-bit services driver.
//
//<AMI_FHDR_END>
//**********************************************************************

#include <AmiDxeLib.h>
#include <Protocol\LegacyBios.h>
#include <Protocol\FirmwareVolume.h>
#include "token.h"


EFI_GUID gRt32Section = 
    {0xf303af22, 0x6804, 0x494b, 0xa2, 0x8a, 0xa0, 0x3b, 0xe7, 0xd5, 0xc7, 0x42};

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:	RT32_EntryPoint
//
// Description:	RT32 driver entry point
//
//<AMI_PHDR_END>
//**********************************************************************                
EFI_STATUS
RT32_EntryPoint(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS  Status;
    UINTN       FvProtocolCount;
    EFI_HANDLE  *FvHandles;
    EFI_FIRMWARE_VOLUME2_PROTOCOL   *Fv;
    UINTN       Index;
    UINT32      AuthenticationStatus;
    VOID        *Rt32Dest;
    UINT8       *Rt32Bin;
    UINTN       Rt32BinSize;
    EFI_LEGACY_BIOS_PROTOCOL *LegacyBios;
    UINT8       *p;
    UINT8       chksum, i;

    static EFI_GUID  Rt32ImageGuid  = { 0xF303AF22, 0x6804, 0x494b, 0xA2, 0x8A, 0xA0, 0x3B, 0xE7, 0xD5, 0xC7, 0x42 };

    InitAmiLib(ImageHandle, SystemTable);

    Status = pBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    // locate and load RT32 binary
    Rt32Bin         = NULL;
    FvHandles       = NULL;
    Status = pBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiFirmwareVolume2ProtocolGuid,
        NULL,
        &FvProtocolCount,
        &FvHandles
        );
    if (EFI_ERROR(Status)) return Status;
    
    for (Index = 0; Index < FvProtocolCount; Index++) {
        Status = pBS->HandleProtocol (
                FvHandles[Index],
                &gEfiFirmwareVolume2ProtocolGuid,
                (VOID **) &Fv
                );

        Status = Fv->ReadSection (
                Fv,
                &Rt32ImageGuid,
                EFI_SECTION_RAW,
                0,
                &Rt32Bin,
                &Rt32BinSize,
                &AuthenticationStatus
                );

        if (!EFI_ERROR (Status)) {
            break;
        }
    }
    
    if (FvHandles != NULL) {
        pBS->FreePool (FvHandles);
    }

    if (Index == FvProtocolCount) {
        ASSERT(FALSE);  // binary not found
        return EFI_NOT_FOUND;
    }

    // Copy RT32.BIN to E0000...FFFFF
    Status = LegacyBios->GetLegacyRegion(
                LegacyBios,
                Rt32BinSize,
                RT32_BINARY_LOCATION,
                0x10,
                &Rt32Dest
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    *((UINT32*)Rt32Bin+1) = (UINT32)((UINTN)Rt32Dest+0x20);   // BIOS32_SD_EntryPoint

    for (p=(UINT8*)Rt32Bin, i=0, chksum=0; i<16; p++, i++) {
        chksum += *p;
    }
    *((UINT8*)Rt32Bin+10) = 0xFF-chksum+1;      // BIOS32_SD_CheckSum

    Status = LegacyBios->CopyLegacyRegion(
                LegacyBios,
                Rt32BinSize,
                Rt32Dest,
                Rt32Bin
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    return EFI_SUCCESS;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
