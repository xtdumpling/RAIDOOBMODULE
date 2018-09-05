/**

Copyright (c) 2004, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

    @file FvbExtension.h

  FVB Extension protocol that extends the FVB Class in a component fashion.

**/

#ifndef _FVB_EXTENSION_H_
#define _FVB_EXTENSION_H_

 typedef struct _EFI_FVB_EXTENSION_PROTOCOL EFI_FVB_EXTENSION_PROTOCOL;

//  FVB Extension Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI * EFI_FV_ERASE_CUSTOM_BLOCK) (
  IN EFI_FVB_EXTENSION_PROTOCOL   *This,
  IN EFI_LBA                              StartLba,
  IN UINTN                                OffsetStartLba,
  IN EFI_LBA                              LastLba,
  IN UINTN                                OffsetLastLba
);

//
// IPMI TRANSPORT PROTOCOL
//
 struct _EFI_FVB_EXTENSION_PROTOCOL {
  EFI_FV_ERASE_CUSTOM_BLOCK               EraseFvbCustomBlock;
 };

//

extern EFI_GUID                           gEfiFvbExtensionProtocolGuid;

#endif

