//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  1999 - 2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file Slit.h

  This file describes the contents of the ACPI System Locality Information 
  Table (SLIT).  Some additional ACPI 3.0 values are defined in Acpi3_0.h.
  All changes to the Slit contents should be done in this file.

**/

#ifndef _SLIT_H_
#define _SLIT_H_

//
// SLIT Definitions, see TBD specification for details.
//

#define EFI_ACPI_OEM_SLIT_REVISION  0x00000001
//
// SLIT Revision (defined in spec)
//
#define EFI_ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE_REVISION 0x01
#define EFI_ACPI_NUMBER_OF_SYSTEM_LOCALITIES                0x0000000000000008
#define EFI_ACPI_SYSTEM_LOCALITIES_ENTRY_COUNT              64
#define ZERO_HOP                    10
#define ONE_ONE                     11
#define ONE_HOP                     21
#define TWO_HOP                     31
#define THREE_HOP                   41
#define DISTANT_NODE_4S_EP          2
#define DISTANT_NODE_4S_EP_COD      (DISTANT_NODE_4S_EP * 2) 

typedef struct {
  UINT8   Entry;
} EFI_ACPI_NUMBER_OF_SYSTEM_LOCALITIES_STRUCTURE;

typedef struct {
  EFI_ACPI_3_0_SYSTEM_LOCALITY_DISTANCE_INFORMATION_TABLE_HEADER    Header;

#if EFI_ACPI_SYSTEM_LOCALITIES_ENTRY_COUNT > 0
  EFI_ACPI_NUMBER_OF_SYSTEM_LOCALITIES_STRUCTURE                    NumSlit[EFI_ACPI_SYSTEM_LOCALITIES_ENTRY_COUNT];
#endif
} EFI_ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE;


#endif
