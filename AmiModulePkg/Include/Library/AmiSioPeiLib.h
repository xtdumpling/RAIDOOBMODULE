//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
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
    SIO PEI Init Library definitions.

**/

#ifndef _AMI_SIO_PEI_LIB_H_
#define _AMI_SIO_PEI_LIB_H_

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Protocol/AmiSio.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
typedef struct _IO_DECODE_DATA{
    UINT16          BaseAdd;
    //!!!Attention!!!If type is 0xFF, UID is a IO legth
    UINT8           UID;
    SIO_DEV_TYPE    Type;
} IO_DECODE_DATA;

typedef struct _SIO_DEVICE_INIT_DATA{
    UINT16      Reg16;
    UINT8       AndData8;   // 0x00 means register don't need AndMask
                            // only write OrData8 to regisrer.
    UINT8       OrData8;
} SIO_DEVICE_INIT_DATA;

typedef struct _SIO_DATA{
    UINT16           Addr;
    //AND mask value, 0xFF means register don't need AndMask and 
    //only write OrData8 to regisrer.
    UINT8           DataMask; 
    //OR mask value.  
    UINT8           DataValue;
} SIO_DATA;

//-------------------------------------------------------------------------
//Variable, Prototype, and External Declarations
//-------------------------------------------------------------------------

//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure:  AmiSioLibSetLpcDeviceDecoding
//
// Description:
//  This function goes through the elinked list of identify functions
//  giving control when the token "IODECODETYPE == 1".
//
// Input:
//  Base    - I/O base address
//            Base=0 means disable the decode of the device
//  DevUid  - The device Unique ID
//            If type is 0xFF, DevUid contain the IO length
//  Type    - Device type
//            If type is 0xFF, DevUid contain the IO length
//
// Output:
//  EFI_SUCCESS - Set successfully.
//  EFI_INVALID_PARAMETER - the Input parameter is invalid.
//
// Notes:
//  Chipset porting should provide the Io Ranage decode function.
//  If chipset porting provide this function, set IODECODETYPE = 0.
//  If chipset porting doesn't provide this function, you can eLink your
//  function to IoRangeDecodeList or replace CSP_SetLpcDeviceDecoding elink
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI AmiSioLibSetLpcDeviceDecoding(
    IN    VOID           *LpcPciIo,
    IN    UINT16         Base,
    IN    UINT8          DevUid,
    IN    SIO_DEV_TYPE   Type);

//<AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:   AmiSioLibSetLpcGenericDecoding
//
// Description:
//  This function set LPC Bridge Generic Decoding
//
// Input:
//  *LpcPciIo - Pointer to LPC PCI IO Protocol
//  Base      - I/O base address
//  Length    - I/O Length
//  Enabled   - Enable/Disable the generic decode range register
//
// Output:
//  EFI_SUCCESS - Set successfully.
//  EFI_UNSUPPORTED - This function is not implemented or the
//                    Length more than the maximum supported
//                    size of generic range decoding.
//  EFI_INVALID_PARAMETER - the Input parameter is invalid.
//  EFI_OUT_OF_RESOURCES - There is not available Generic
//                         Decoding Register.
//  EFI_NOT_FOUND - the generic decode range will be disabled
//                  is not found.
//
// Notes:
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI AmiSioLibSetLpcGenericDecoding (
    IN VOID				       *LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable );



VOID EFIAPI ProgramRtRegisterTable(
    IN  UINT16  Base,
    IN  SIO_DEVICE_INIT_DATA  *Table,
    IN  UINT8   Count
);

#endif //_AMI_SIO_PEI_LIB_H_
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

