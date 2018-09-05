//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
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
  Global Pci Hooks Protocol Definitions
*/

#ifndef __AMI_PCI_DXE_INIT_PRIVATE__H__
#define __AMI_PCI_DXE_INIT_PRIVATE__H__

#include <Uefi.h>
#include <Protocol/AmiBoardInitPolicy.h>  


extern AMI_BOARD_INIT_PROTOCOL *gPciInitProtocolPtr;


//Data structure of CallBack delivered trough eLink
typedef struct {
    UINT16                              VendorId;
    UINT16                              DeviceId;
    VOID                                *Callback;
} AMI_PCI_DEVICE_CALLBACK;

typedef EFI_STATUS (AMI_BOARD_INIT_FUNCTION_PROTOTYPE) (
    IN AMI_BOARD_INIT_PROTOCOL              *This,
    IN UINTN                                *Function,
    IN OUT AMI_BOARD_INIT_PARAMETER_BLOCK   *ParameterBlock
);


#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
