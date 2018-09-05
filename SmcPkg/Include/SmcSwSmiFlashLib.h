//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.03
//    Bug Fix:  Trigger a software SMI to flash ROM part after BIOS lock.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/30/2016
//
//**************************************************************************//
//****************************************************************************

#ifndef __SMCSWSMIFLASHLIB_H__
#define __SMCSWSMIFLASHLIB_H__
#ifdef __cplusplus
extern "C" {
#endif

EFI_STATUS SmcSwSmiFlashWrite (
    VOID* FlashAddress,
    UINTN Size,
    VOID* DataBuffer
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif // __SMCSWSMIFLASHLIB_H__

//****************************************************************************
