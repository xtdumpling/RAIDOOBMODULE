//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file IpmiUsbPei.h
    IPMI Usb Pei definitions.

**/

#ifndef _IPMI_USB_PEI_H_
#define _IPMI_USB_PEI_H_

//
// Max temp data should always be in multiple of 512 bytes as shown below.
//#define BLOCK_SIZE                  512
//#define MAX_TEMP_DATA               (BLOCK_SIZE * MAX_BLOCK_PER_TRANSACTION)
/*
// If you want to change the value of it, change MAX_BLOCK_PER_TRANSACTION value.
// Maximum limit to read and write the data using IPMI USB BMC is set by 32768 (0x8000) bytes.
// So MAX_BLOCK_PER_TRANSACTION value should be (0x8000/512 =)0x40 
//
#define MAX_BLOCK_PER_TRANSACTION   0x40
*/
// Keeping MAX_BLOCK_PER_TRANSACTION as 1 for now.
//
#define MAX_BLOCK_PER_TRANSACTION   0x01

#include <IpmiUsbCommon.h>

#pragma pack()

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
