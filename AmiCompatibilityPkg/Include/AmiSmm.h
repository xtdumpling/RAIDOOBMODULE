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
/** @file
  Ami specific Smm definitions
*/
#ifndef __AMI_SMM__H__
#define __AMI_SMM__H__

#include <Efi.h>

#ifdef __cplusplus
extern "C" {
#endif


//The following is for restoring SMM bases on S3 resume.
#define	SMM_BASES_VARIABLE L"SmmBasesVariable"

#pragma pack(1)

typedef struct _SMM_BASE_CHANGE_INFO SMM_BASE_CHANGE_INFO;

struct _SMM_BASE_CHANGE_INFO {
	VOID			(*ChgSmmBases)(SMM_BASE_CHANGE_INFO *This);
};
#pragma pack()

//This structure is added to SMM Configuration Table to determine
// which CPU generated a software smi.

#define SW_SMI_CPU_TRIGGER_GUID \
    {0xe4d535bb,0x5dea,0x42f8,0xb5,0xf8,0xd6,0xb8,0x13,0xff,0x6b,0x46}

typedef struct {
	UINTN 	Cpu;
} SW_SMI_CPU_TRIGGER;




/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

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
