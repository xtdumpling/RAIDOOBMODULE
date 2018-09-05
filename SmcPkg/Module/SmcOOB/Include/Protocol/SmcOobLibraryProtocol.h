//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
//
//  Rev. 1.06
//    Bug Fix:  
//    Reason:   Add a function hook before Update Bios Cfg to determine should update BIOS cfg.
//    Auditor:  Durant Lin
//    Date:     Jun/25/2018
//
//  Rev. 1.05
//    Bug Fix:  Add a function hook before LoadDefault.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Jun/12/2018
//
//  Rev. 1.04
//    Bug Fix:  Add a SMM Oob Library interface. 
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Feb/12/2018
//
//  Rev. 1.03
//    Bug Fix:  Add a interface to hook updatevariable before.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Jan/17/2018
//
//  Rev. 1.02
//    Bug Fix : N/A
//    Reason  : Isolate PciLib from OOB to OobLibraryProtocol, since different
//              platform will cause hang up issue.
//    Auditor : Durant Lin
//    Date    : Jun/02/2018
//
//  Rev. 1.01
//    Bug Fix : N/A
//    Reason  : Implement a SmcOobProtocol Interface for InBand and OutBand to
//              access Build time OobLibrary not OBJ OobLobrary. 
//    Auditor : Durant Lin
//    Date    : Dec/27/2017
//
//  Rev. 1.00
//    Bug fixed:
//    Reason   :
//    Auditor  :
//    Date     :
//
//****************************************************************************

#ifndef _SMC_OOB_LIBRARY_PROTOCOL_H_
#define _SMC_OOB_LIBRARY_PROTOCOL_H_
      
#include <Guid/SmcOobPlatformPolicy.h>


#define SMC_OOB_LIBRARY_PROTOCOL_GUID \
  { \
    0x1A48C1C9, 0xAF1B, 0xC3A2, {0xA4, 0x29, 0x8E, 0xA3, 0xC5, 0x3D, 0x6A, 0x9D} \
  }

//
// Forward Declaration
//
typedef struct _SMC_OOB_LIBRARY_PROTOCOL SMC_OOB_LIBRARY_PROTOCOL;
typedef struct _SMC_OOB_LIBRARY_PROTOCOL SMC_SMM_OOB_LIBRARY_PROTOCOL;

//
// Revision 1: First Version
//
#define SMC_OOB_LIBRARY_PROTOCOL_REVISION_1   1
//
// SMC ROM HOLE PROTOCOL
//

typedef
VOID (*SmcGenerateFeatureSupportFlags) (
  IN     UINT8                          *Buffer,
  IN OUT UINT32 						*TotalLength,
  IN     SMC_OOB_PLATFORM_POLICY 		*SmcOobPlatformPolicy
);
typedef 
UINT32 (*SmcPciRead32Func) (
  IN	UINT8	Bus,
  IN	UINT8	Device,
  IN	UINT8	Function,
  IN	UINT16	Register
  );

typedef
EFI_STATUS (*SmcUpdatePlatformVariableFunc)(
    IN CHAR16 	*VariableName,
    IN EFI_GUID	*VariableGuid,
    IN UINTN 	VariableSize,
    IN UINT8 	*VariableBuffer
);

typedef 
EFI_STATUS (*SmcCalculateMd5)(
    IN		VOID	*HashData,
    IN		UINTN	DataSize,
    IN OUT	UINT8	*Digest
);

typedef
UINT8 (*SmcOobGetCmosTokensValue)(
    IN OUT	UINT32	tokenID
);

typedef
VOID (*SmcOobSetCmosTokensValue)(
    IN	UINT32	tokenID,
    IN	UINT8	value
);

typedef
VOID (*SmcOobLoadDefaultPreSetting)(
);

typedef
BOOLEAN (*SmcOobDetermineUpdBiosCfg)(
);

typedef struct _SMC_OOB_LIBRARY_PROTOCOL {
	SmcGenerateFeatureSupportFlags	Smc_GenerateFeatureSupportFlags;
	SmcPciRead32Func				Smc_PciRead32;
	SmcUpdatePlatformVariableFunc	Smc_UpdatePlatformVariable;
	SmcCalculateMd5					Smc_CalculateMd5;
	SmcOobGetCmosTokensValue		Smc_OobGetCmosTokensValue;
	SmcOobSetCmosTokensValue		Smc_OobSetCmosTokensValue;
	SmcOobLoadDefaultPreSetting		Smc_OobLoadDefaultPreSetting;
	SmcOobDetermineUpdBiosCfg		Smc_OobDetermineUpdBiosCfg;
};

extern EFI_GUID gSmcOobLibraryProtocolGuid;
extern EFI_GUID gSmcSmmOobLibraryProtocolGuid;
#endif

