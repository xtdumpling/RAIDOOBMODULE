/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  OpromUpdateDxe.h

Abstract:

--*/
#ifndef _OPTIONROM_UPDATE_DXE_H_
#define _OPTIONROM_UPDATE_DXE_H_

#include <Base.h>
#include <Uefi.h>

#include <Protocol/UbaCfgDb.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <IndustryStandard/Pci.h>
#include <Platform.h>
#include <BoardTypes.h>

//
// Global variables for Option ROMs
//
#define NULL_ROM_FILE_GUID \
  { \
    0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
  }

//
// {7301762E-4BF3-4b06-ADA0-766041BE36AF}
//
#define ONBOARD_VIDEO_OPTION_ROM_FILE_GUID \
  { \
    0x7301762e, 0x4bf3, 0x4b06, 0xad, 0xa0, 0x76, 0x60, 0x41, 0xbe, 0x36, 0xaf \
  }

//
// {A6F63D76-6975-4144-96B3-26D624189B2B}
//
#define PXE_UNDI_OPTION_ROM_FILE_GUID \
  { \
    0xa6f63d76, 0x6975, 0x4144, 0x96, 0xb3, 0x26, 0xd6, 0x24, 0x18, 0x9b, 0x2b \
  }

#define PXE_BASE_OPTION_ROM_FILE_GUID \
  { \
    0x6f2bc426, 0x8d83, 0x4f17, 0xbe, 0x2c, 0x6b, 0x1f, 0xcb, 0xd1, 0x4c, 0x80 \
  }

//
// {7A39517D-DD10-4abb-868F-B859FBBBBA87}
//
#define IDE_RAID_OPTION_ROM_FILE_GUID \
  { \
    0x7a39517d, 0xdd10, 0x4abb, 0x86, 0x8f, 0xb8, 0x59, 0xfb, 0xbb, 0xba, 0x87 \
  }

#define BARTON_HILLS_DEVICE_ID 0x150E

// {BD5D4CA5-674F-4584-8CF9-CE4EA1F54DD1}
#define SATA_RAID_OPTION_ROM_FILE_GUID \
  { \
    0xbd5d4ca5, 0x674f, 0x4584, 0x8c, 0xf9, 0xce, 0x4e, 0xa1, 0xf5, 0x4d, 0xd1 \
  }


    // {2CC25173-BD9F-4C89-89CC-29256A3FD9C3}
#define SSATA_RAID_OPTION_ROM_FILE_GUID \
  { \
    0x2cc25173, 0xbd9f, 0x4c89, 0x89, 0xcc, 0x29, 0x25, 0x6a, 0x3f, 0xd9, 0xc3 \
  }

#define V_PCH_EVA_SCU_DEVICE_ID                       0x1D60      // Patsburg EVA SCU Device ID
// {505DDA79-1A65-43c8-9B0C-ED7183136D56}
#define PCH_EVA_SCU_OPTION_ROM_FILE_GUID \
  { \
    0x505DDA79, 0x1A65, 0x43c8, 0x9B, 0x0C, 0xED, 0x71, 0x83, 0x13, 0x6D, 0x56 \
  }

//
//OPTION_ROM_START
//
#define AHCI_OPTION_ROM_FILE_GUID \
  {0x592bfc62, 0xd817, 0x4d1a, 0x86, 0xf8, 0x33, 0x33, 0x4c, 0x9e, 0x90, 0xd8}


//
// {686CAF4E-5140-4d68-90B0-B76291038B00}
//
#define LAN1_OPTION_ROM_FILE_GUID \
  { \
    0x686CAF4E, 0x5140, 0x4d68, 0x90, 0xB0, 0xB7, 0x62, 0x91, 0x03, 0x8B, 0x00 \
  }

//
// {2628EE3E-1FD2-429b-A273-2EC3EDB8C659}
//
#define LAN2_OPTION_ROM_FILE_GUID \
  { \
    0x2628EE3E, 0x1FD2, 0x429b, 0xA2, 0x73, 0x2E, 0xC3, 0xED, 0xB8, 0xC6, 0x59 \
  }

//
// {73FF33F5-C605-4715-B5B3-37C363EEEB03}
//
#define ALL_1GBE_PXE_OPTION_ROM_FILE_GUID \
  { \
    0x73FF33F5, 0xC605, 0x4715, 0xB5, 0xB3, 0x37, 0xC3, 0x63, 0xEE, 0xEB, 0x03 \
  }
  
#define I350_1GBE_PXE_OPTION_ROM_FILE_GUID \
  { \
    0x73FF33F5, 0xC605, 0x4715, 0xB5, 0xB3, 0x37, 0xC3, 0x63, 0xEE, 0xEB, 0x03 \
  }

//
// {D42409FC-CD8C-48cc-BCEA-31C7D66EC5DE}
//
#define ISCSI_V1_1GBE_10GBE_OPTION_ROM_FILE_GUID \
  { \
    0xd42409fc, 0xcd8c, 0x48cc, 0xbc, 0xea, 0x31, 0xc7, 0xd6, 0x6e, 0xc5, 0xde \
  }

//
//X540_FCoE_OpROM_START
//
//
// {D84472A9-D68F-4de8-BDD8-A574AAF7F25B}
//
#define X540_FCOE_OPTION_ROM_FILE_GUID \
  { \
    0xd84472a9, 0xd68f, 0x4de8, 0xbd, 0xd8, 0xa5, 0x74, 0xaa, 0xf7, 0xf2, 0x5b \
  }
//
//X540_FCoE_OpROM_END
//

//
// {D84472A9-D68F-4de8-BDD8-A574AAF7F22B}
//
#define FCOE_V1_1GBE_10GBE_OPTION_ROM_FILE_GUID \
  { \
    0xd84472a9, 0xd68f, 0x4de8, 0xbd, 0xd8, 0xa5, 0x74, 0xaa, 0xf7, 0xf2, 0x2b \
  }
//
// {81200379-E12D-4dc1-8F3E-A5164386CBCC}
//
#define ALL_10GBE_OPTION_ROM_FILE_GUID \
  { \
    0x81200379, 0xe12d, 0x4dc1, 0x8f, 0x3e, 0xa5, 0x16, 0x43, 0x86, 0xcb, 0xcc \
  }

#define I540_10GBE_OPTION_ROM_FILE_GUID \
  { \
    0x81200379, 0xe12d, 0x4dc1, 0x8f, 0x3e, 0xa5, 0x16, 0x43, 0x86, 0xcb, 0xcc \
  }

//
// {E023D434-7067-46aa-A7AE-E2122EEA32E2}
//
#define I540_1GBE_PXE_OPTION_ROM_FILE_GUID \
  { \
    0xe023d434, 0x7067, 0x46aa, 0xa7, 0xae, 0xe2, 0x12, 0x2e, 0xea, 0x32, 0xe2 \
  }
//
//OPTION_ROM_END
//

// {602040E1-449E-4cf0-8294-16A694A42B5F}
#define ESTR2_OPTION_ROM_FILE_GUID \
  { \
    0x602040e1, 0x449e, 0x4cf0, 0x82, 0x94, 0x16, 0xa6, 0x94, 0xa4, 0x2b, 0x5f \
  }



#endif  //_OPTIONROM_UPDATE_DXE_H_

