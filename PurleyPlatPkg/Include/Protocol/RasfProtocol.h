//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/*++

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
  
    RasfProtocol.h
    
Abstract:

   Guids used for RASF protocol

--*/

#ifndef _RASF_SUPPORT_PROTOCOL_H_
#define _RASF_SUPPORT_PROTOCOL_H_


#include <Acpi/Rasf.h>

typedef struct _EFI_RASF_SUPPORT_PROTOCOL {
  EFI_ACPI_PCC_SHARED_MEMORY_REGION_RASF     *RasfSharedMemoryAddress;
  } EFI_RASF_SUPPORT_PROTOCOL;



extern EFI_GUID gEfiRasfSupportProtocolGuid;


#endif
//End of File
