//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Trigger a software SMI to flash ROM part after BIOS lock.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/30/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcSwSmiFlash.h
//
// Description: 
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef	_SMCSWSMIFLASH_H_
#define	_SMCSWSMIFLASH_H_


#define	SWSMI_E9_SUBCMD_FLASH_WRITE 1
#define	SWSMI_E9_SUBCMD_UNREGISTER 2

#pragma pack(1)

typedef struct {
    VOID* FlashAddress;
    UINTN Size;
    VOID* DataBuffer;
} SMC_SW_SMI_FLASH_HEADER;

#pragma pack()

#endif // _SMCSWSMIFLASH_H_

