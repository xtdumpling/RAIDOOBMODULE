//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2012, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/BIN/Core/Include/Protocol/AmiReflashProtocol.h $
//
// $Author: Artems $
//
// $Revision: 1 $
//
// $Date: 7/20/12 10:09a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		AMIReflashProtocol.h
//
// Description:	Header file for ESRT.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef __REFLASH_PROTOCOL__H__
#define __REFLASH_PROTOCOL__H__

#ifdef __cplusplus
extern "C"
{
#endif

#define EFI_REFLASH_PROTOCOL_GUID \
    {0x8fe545e5, 0xdca9, 0x4d52, 0xbf, 0xcd, 0x8f, 0x13, 0xe7, 0x17, 0x1, 0x5f}

extern EFI_GUID gAmiReflashProtocolGuid;

typedef struct _EFI_REFLASH_PROTOCOL EFI_REFLASH_PROTOCOL;

typedef EFI_STATUS
(EFIAPI *EFI_REFLASH_PROGRESS)(
    IN EFI_REFLASH_PROTOCOL    *This
);

typedef EFI_STATUS
(EFIAPI *EFI_REFLASH_GET_DISPLAY_IMAGE)(
    IN EFI_REFLASH_PROTOCOL *This,
    OUT UINTN               *CoordinateX,
    OUT UINTN               *CoordinateY,
    OUT VOID                **ImageAddress
);

struct _EFI_REFLASH_PROTOCOL {
    EFI_REFLASH_PROGRESS          CapUpdProgress;
    EFI_REFLASH_GET_DISPLAY_IMAGE GetDisplayImage;  	  	
};


    /****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2012, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
