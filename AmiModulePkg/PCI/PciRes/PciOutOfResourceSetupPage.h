//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//*************************************************************************
/** @file PciOutOfResourceSetupPage.h
        Out Of resources Setup Page Header File. 

**/
//*************************************************************************
#ifndef __PCI_OUT_OF_RESOURCES__H__
#define __PCI_OUT_OF_RESOURCES__H__
#include <PciOutOfResourceSetupPageStrDefs.h>
#include <Token.h>
#ifndef VFRCOMPILE
#include <AmiDxeLib.h>
#endif

#define PCI_OUT_OF_RESOURCE_FORM_SET_CLASS             0x79

//932d37b0-0d4a-11e0-81e0-0800200c9a66

#define PCI_OUT_OF_RESOURCE_FORM_SET_GUID { 0x932d37b0, 0x0d4a, 0x11e0, 0x81, 0xe0, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 }


#define PCI_OUT_OF_RESOURCE_FORM_MAIN 1

#include <Setup.h>
#ifndef VFRCOMPILE

extern EFI_HII_HANDLE PciOutOfResourceSetupPageHiiHandle;

EFI_STATUS GetHiiString(
    IN EFI_HII_HANDLE HiiHandle,
    IN STRING_REF Token,
    IN OUT UINTN *pDataSize, OUT EFI_STRING *ppData
);
EFI_STATUS UpdateProgressString(
    IN STRING_REF Token, IN UINTN Progress, IN BOOLEAN Failed
);
#endif //VFRCOMPILE

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
