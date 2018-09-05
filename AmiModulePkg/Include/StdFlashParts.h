//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file StdFlashParts.h
    This file contains the structure and function prototypes needed
    for the Flash driver

**/

#ifndef __STD_FLASH_PARTS_H_
#define __STD_FLASH_PARTS_H_
#ifdef __cplusplus
extern "C" {
#endif

BOOLEAN
mStdSpansionIdentify    (
    IN  volatile UINT8      *pBlockAddress,
    OUT FLASH_PART          **FlashStruct
);

BOOLEAN
IntelSTDIdentify    (
    IN  volatile UINT8      *pBlockAddress,
    OUT FLASH_PART          **FlashStruct
);

BOOLEAN
mStdMxicIdentify    (
    IN  volatile UINT8      *pBlockAddress,
    OUT FLASH_PART          **FlashStruct
);


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
