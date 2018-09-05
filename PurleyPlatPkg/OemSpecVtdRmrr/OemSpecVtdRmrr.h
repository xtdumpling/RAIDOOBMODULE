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

//**********************************************************************
// $Header:  $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  OemSpecVtdRmrr.h
//
// Description: Oem Spec Vtd Rmrr definitions
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef _OEM_SPEC_VTD_RMRR_H_
#define _OEM_SPEC_VTD_RMRR_H_

#define OEM_SPEC_INT_15H                 0x15
#define OEM_SPEC_FUNCTION_CODE           0xDE
#define OEM_SPEC_FUNCTION_SIGNATURE      0xABBADADA

#define SEGMENT0                        0x00
#define SEGMENT1                        0x01

#define OEM_SPEC_VTD_RMMR_DEBUG_SUPPORT  1

#if OEM_SPEC_VTD_RMMR_DEBUG_SUPPORT
#define OEM_SPEC_RMMR_DEBUG(Arguments)  DEBUG(Arguments)
#else
#define OEM_SPEC_RMMR_DEBUG(Arguments)
#endif

typedef enum {
    OemSpecGetControllerCount,
    OemSpecGetMemoryReqBDFInfo,
    OemSpecSetMemory,
    OemSpecMaxFuncNumber
} OEM_SPEC_INT15_FUNC;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
