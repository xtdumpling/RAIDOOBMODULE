//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Use AMIBCP to disable boot procedure messages displaying.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jun/21/2016
//
//**************************************************************************//
//****************************************************************************

#ifndef __SMCPOSTPRINTLIB_H__
#define __SMCPOSTPRINTLIB_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Hob.h>

#define EFI_SMCPOSTPRINT_HOB_GUID \
  { \
     0x45a0de77, 0x6e11, 0x48cd, 0xaa, 0x85, 0x2a, 0xd0, 0xa7, 0x1c, 0xb7, 0xd2 \
  }

typedef struct _SMCPOSTPRINT_HOB_INFO {
    EFI_HOB_GUID_TYPE EfiHobGuidType;
    UINT8 SmcBootProcedureMsg;
} SMCPOSTPRINT_HOB_INFO;



/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif // __SMCPOSTPRINTLIB_H__

//****************************************************************************
