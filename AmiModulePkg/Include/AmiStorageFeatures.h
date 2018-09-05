//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
  @brief AMI Storage Features Header File.

  The file contains definitions of the AMI Specific storage related structures and constants.
**/

#ifndef __AMI_STORAGE_FEATURES__H__
#define __AMI_STORAGE_FEATURES__H__

/// Defines type of a Storage Bus Interface.
typedef enum {
    AmiStorageInterfaceIde,
    AmiStorageInterfaceAhci,
    AmiStorageInterfaceNvme,
    AmiStorageInterfaceMax
} AMI_STORAGE_BUS_INTERFACE;

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
