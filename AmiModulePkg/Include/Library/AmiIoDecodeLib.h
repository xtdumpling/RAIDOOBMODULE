//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file 
  @brief
    Chipset Specific LPC IO Decode library.

**/

#ifndef __AMI_IO_DECODE_LIB_H__
#define __AMI_IO_DECODE_LIB_H__

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Uefi.h>
#include <Protocol/AmiSio.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//Variable, Prototype, and External Declarations
//-------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   AmiSioLibSetLpcDeviceDecoding
//
// Description: This function sets LPC Bridge Device Decoding
//
// Input:       *LpcPciIo - Pointer to LPC PCI IO Protocol(NULL in PEI)
//              Base      - I/O base address, if Base is 0 means disabled the
//                          decode of the device
//              DevUid    - The device Unique ID
//              Type      - Device Type, please refer to AMISIO.h
//
// Output:      EFI_STATUS
//                  EFI_SUCCESS - Set successfully.
//                  EFI_UNSUPPORTED - There is not proper Device Decoding
//                                    register for the device UID.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI AmiSioLibSetLpcDeviceDecoding (
    IN VOID      				*LpcPciIo,
    IN UINT16                   Base,
    IN UINT8                    DevUid,
    IN UINT32                   Type );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   AmiSioLibSetLpcGenericDecoding
//
// Description: This function set LPC Bridge Generic Decoding
//
// Input:       *LpcPciIo - Pointer to LPC PCI IO Protocol
//              Base      - I/O base address
//              Length    - I/O Length
//              Enabled   - Enable/Disable the generic decode range register
//
// Output:      EFI_STATUS
//                  EFI_SUCCESS - Set successfully.
//                  EFI_UNSUPPORTED - This function is not implemented or the
//                                    Length more than the maximum supported
//                                    size of generic range decoding.
//                  EFI_INVALID_PARAMETER - the Input parameter is invalid.
//                  EFI_OUT_OF_RESOURCES - There is not available Generic
//                                         Decoding Register.
//                  EFI_NOT_FOUND - the generic decode range will be disabled
//                                  is not found.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI AmiSioLibSetLpcGenericDecoding (
    IN VOID      				*LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable );





#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

