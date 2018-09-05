//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c) 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IohUds.h
    
  This protocol provides access to the Ioh Universal Data Structure 

  This protocol is EFI compatible.
  

**/

#ifndef _EFI_IIO_UDS_PROTOCOL_H_
#define _EFI_IIO_UDS_PROTOCOL_H_

#include <Setup/IioUniversalData.h>

#define EFI_IIO_UDS_PROTOCOL_GUID  \
  { 0xa7ced760, 0xc71c, 0x4e1a, 0xac, 0xb1, 0x89, 0x60, 0x4d, 0x52, 0x16, 0xcb }

typedef struct _EFI_IIO_UDS_PROTOCOL EFI_IIO_UDS_PROTOCOL;

// APTIOV_SERVER_OVERRIDE_RC_START : Changes done for the CRB defined OutOfResource (MMIO/IO) Support
#define AUTO_ADJUST_MMMIO_IO_VARIABLE_DATA_GUID \
  { 0x3992e100, 0x8860, 0x11e0, 0x9d,0x78, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66}

typedef struct _AUTO_ADJUST_MMMIO_IO_VARIABLE_DATA AUTO_ADJUST_MMMIO_IO_VARIABLE_DATA;

struct _AUTO_ADJUST_MMMIO_IO_VARIABLE_DATA {

	// ########################### SOCKET LEVEL #############################	
    UINT16  IoRatio[MAX_SOCKET];
    UINT16  MmiolRatio[MAX_SOCKET];
    
  // ########################### STACK LEVEL #############################  
    UINT16  IoRatio_Stack_Level[MAX_SOCKET] [MAX_IIO_STACK];
    UINT16  MmiolRatio_Stack_Level[MAX_SOCKET] [MAX_IIO_STACK];
};
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done for the CRB defined OutOfResource (MMIO/IO) Support

typedef
EFI_STATUS
(EFIAPI *IIH_ENABLE_VC) (
  IN EFI_IIO_UDS_PROTOCOL     *This,
  IN UINT32                    VcCtrlData
  );
/**

  Enables the requested VC in IIO
    
  @param This                    Pointer to the EFI_IOH_UDS_PROTOCOL instance.
  @param VcCtrlData              Data read from VC resourse control reg.
                          
**/


typedef struct _EFI_IIO_UDS_PROTOCOL {
  IIO_UDS          *IioUdsPtr;
  IIH_ENABLE_VC    EnableVc;
} EFI_IIO_UDS_PROTOCOL;

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiIioUdsProtocolGuid;

#endif
