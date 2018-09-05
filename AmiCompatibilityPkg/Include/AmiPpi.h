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
	This file contains AMI-defined and deprecated PPIs which are not present 
	in the EDKII header files.
*/
#ifndef  _AMI_PPI_H   //To Avoid this header get compiled twice
#define  _AMI_PPI_H

#include <Pei.h>

#ifndef GUID_VARIABLE_DECLARATION
#define GUID_VARIABLE_DECLARATION(Variable, Guid) extern EFI_GUID Variable
#endif


// Ppi/AtaController.h
// The following has been deprecated by new version of PeiAtaControllerPpi
#define PEI_IDE_RECOVERY_NATIVE_MODE_PPI_GUID \
  { 0x7e13637a, 0xc3f8, 0x43d1, 0xb0, 0x51, 0xed, 0x19, 0xd7, 0x8, 0xec, 0x7a }

typedef struct {
	UINT16 PCMDBarAddress;
	UINT16 PCNLBarAddress;
	UINT16 SCMDBarAddress;
	UINT16 SCNLBarAddress;
} PEI_IDE_RECOVERY_NATIVE_MODE_PPI;

// Ppi/ReadOnlyVariable.h
// The following GUID is named differently in MdePkg
#define EFI_PEI_READ_ONLY_VARIABLE_PPI_GUID \
    {0x3cdc90c6, 0x13fb, 0x4a75, 0x9e, 0x79, 0x59, 0xe9, 0xdd, 0x78, 0xb9, 0xfa}


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
