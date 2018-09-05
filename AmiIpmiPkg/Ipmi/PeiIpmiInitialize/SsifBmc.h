//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SsifBmc.h
    SIF BMC Controller Macros.

**/

#ifndef _EFI_SSIF_BMC_H
#define _EFI_SSIF_BMC_H

//
// Smbus Write Commands
//
#define SMBUS_SINGLE_WRITE_CMD 0x2
#define SMBUS_MULTI_WRITE_START_CMD 0x6
#define SMBUS_MULTI_WRITE_MIDDLE_CMD 0x7
#define SMBUS_MULTI_WRITE_END_CMD 0x8

//
// Smbus Read Commands
//
#define SMBUS_SINGLE_READ_CMD 0x3
#define SMBUS_MULTI_READ_START_CMD SMBUS_SINGLE_READ_CMD
#define SMBUS_MULTI_READ_MIDDLE_CMD 0x9
#define SMBUS_MULTI_READ_END_CMD 0x9
#define SMBUS_MULTI_READ_RETRY_CMD 0xA

//
// Block length
//
#define SMBUS_BLOCK_LENGTH 0x20

#define MULTI_READ_ZEROTH_STRT_BIT 0x0
#define MULTI_READ_FIRST_STRT_BIT 0x1

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
