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

/** @file AmiUsbSmmGlobalDataValidationLib.h
  Defines the AmiUsbSmmGlobalDataValidationLib library functions.

  Buffer Validation Functions.
**/
#ifndef __AMI_USB_SMM_GLOBAL_DATA_VALIDATION_LIB__H
#define __AMI_USB_SMM_GLOBAL_DATA_VALIDATION_LIB__H

EFI_STATUS EFIAPI UpdateAmiUsbSmmGlobalDataCrc32(USB_GLOBAL_DATA*);
EFI_STATUS EFIAPI AmiUsbSmmGlobalDataValidation(USB_GLOBAL_DATA*);

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
