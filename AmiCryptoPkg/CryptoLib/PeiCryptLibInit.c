//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
#include <Token.h>
#include <AmiPeiLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include "includes.h"
#include <CryptLib.h>

//
// Global variables
//

// Crypto Memory Manager heap address
static EFI_PHYSICAL_ADDRESS gDstAddress = 0;
static UINTN gHeapSize = 0;

/**
  This constructor function allocates memory for PEI Crypto Memory manager,
 
  @param  FileHandle    The firmware allocated handle for the EFI image.
  @param  PeiServices   A pointer to the PEI Services.
  
  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
PeiAmiCryptLibConstructor (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS  Status;
    UINTN       Npages;

    DEBUG((AmiCryptoPkg_DEBUG_LEVEL,"AmiPeiCryptoLib Constructor\n"));
    //  
    // Update Crypto debug traces level
    //
#ifdef EFI_DEBUG
    wpa_set_trace_level(CRYPTO_trace_level);
#endif    
    ///////////////////////////////////////////////////////////////////////////////
    //
    // Init Aux Memory Manager
    // Pre-allocates runtime space for possible cryptographic operations
    //
    ///////////////////////////////////////////////////////////////////////////////
    Status = EFI_SUCCESS;
    if(gDstAddress == 0) {
        gHeapSize = PcdGet32(AmiPcdCpMaxHeapSize);
        Npages = EFI_SIZE_TO_PAGES(gHeapSize);
        Status = (*PeiServices)->AllocatePages(PeiServices, EfiBootServicesData, Npages, &gDstAddress);
    }
    DEBUG ((AmiCryptoPkg_DEBUG_LEVEL, "PEI Heap alloc %r (addr=%X, size=%x)\n", Status, (UINT32)gDstAddress, gHeapSize));
    if(!EFI_ERROR(Status) && gDstAddress != 0 )
        InitCRmm( (void*)gDstAddress, gHeapSize);

    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
