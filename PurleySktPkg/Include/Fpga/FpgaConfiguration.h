//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 2016 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FpgaConfiguration.h

Abstract:

  This file provides required Data formats of FPGA setup configuration values.

--*/

#define FPGA_CONFIGURATION_KEY              0xF0E0


#define FPGA_CLASS_ID                       44
#define FPGA_SUBCLASS_ID                    0x00

#define FPGA_SOCKET_CONFIGURATION_VARID     0x9a00


// {1EB3BC41-1492-40B8-AC9E-43A56C14C7A1}
#define FPGA_SETUP_GUID {0x1eb3bc41, 0x1492, 0x40b8, 0xac, 0x9e, 0x43, 0xa5, 0x6c, 0x14, 0xc7, 0xa1}


#define VFR_FORMID_FPGA_SETUP               0x47D0
#define VFR_FORMID_FPGA_THERMAL_MANAGEMENT  0x47D1
#define VFR_FORMID_FPGA_DFX_CONFIG          0x47D2

#define KEY_FPGA_TEST                       0x8a00
#define KEY_FPGA_SOCKET                     0x8b00
#define KEY_FPGA_ENABLE                     0x8c00

#define VFR_FORMLABLE_FPGA_SOCKET_TOP       0x8a10
#define VFR_FORMLABLE_FPGA_SOCKET_BOTTOM    0x8a11

#define FPGA_SETUP_ENABLE_DEFAULT_VALUE     0x3
#define FPGA_GUID_INDEX_AUTO_VALUE          0x00
#define FPGA_GUID_INDEX_NONE_VALUE          0xff


#define FPGA_HII_GUID_STRING_ID_BASE        0x2200
#define FPGA_HII_GUID_STRING_ID(GuidIndex)   ((EFI_STRING_ID )(FPGA_HII_GUID_STRING_ID_BASE + (GuidIndex)))

#define FPGA_HII_SOCKET_STRING_ID_BASE      0x2300
#define FPGA_HII_SOCKET_STRING_ID(SocketIndex)   ((EFI_STRING_ID )(FPGA_HII_SOCKET_STRING_ID_BASE + (SocketIndex)))


// {22819110-7F6F-4852-B4BB-13A770149B0C}
#define FPGA_FORMSET_GUID \
  { \
  0x22819110, 0x7f6f, 0x4852, 0xb4, 0xbb, 0x13, 0xa7, 0x70, 0x14, 0x9b, 0xc \
  }

//  {75839B0B-0A99-4233-8AA4-3866F6CEF4B3}
#define FPGA_CONFIGURATION_GUID \
  { \
  0x75839b0b, 0x0a99, 0x4233, 0x8a, 0xa4, 0x38, 0x66, 0xf6, 0xce, 0xf4, 0xb3 \
  }
