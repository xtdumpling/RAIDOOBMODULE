//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IoApic.h

  IO APIC register definitions. These are de-facto industry standard.


Revision History:

**/

#ifndef _IOAPIC_H_
#define _IOAPIC_H_

#define EFI_IO_APIC_INDEX_OFFSET          0x00
#define EFI_IO_APIC_DATA_OFFSET           0x10
#define EFI_IO_APIC_IRQ_ASSERTION_OFFSET  0x20
#define EFI_IO_APIC_EOI_OFFSET            0x40

#define EFI_IO_APIC_ID_REGISTER           0x0
#define EFI_IO_APIC_ID_BITSHIFT           24
#define EFI_IO_APIC_VER_REGISTER          0x1
#define EFI_IO_APIC_BOOT_CONFIG_REGISTER  0x3
#define EFI_IO_APIC_FSB_INT_DELIVERY      0x1

#endif
