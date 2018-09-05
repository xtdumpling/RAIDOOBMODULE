//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
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

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
//  Name:           SecureFlash.h
//
//  Description:    Common Secure Flash Setup definitions
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#ifndef _SECURE_FLASH_MODE_H_
#define _SECURE_FLASH_MODE_H_

typedef struct{
    UINT8 Key;
    UINT8 Type;
    UINT8 Mode;
    UINT8 Lock;
    UINT8 Rollback;
} SECURE_FLASH_SETUP_VAR;

#define AMI_SECURE_FLASH_SETUP_VAR  L"SecureFlashSetup"

// {35C936AF-E1E1-441a-BAD1-E1544E9D97A6}
#define AMI_SECURE_FLASH_SETUP_VAR_GUID \
    {0x35c936af, 0xe1e1, 0x441a, 0xba, 0xd1, 0xe1, 0x54, 0x4e, 0x9d, 0x97, 0xa6}

#endif //_SECURE_FLASH_MODE_H_
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
