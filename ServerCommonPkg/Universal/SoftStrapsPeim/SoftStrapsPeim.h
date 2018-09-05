/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

Copyright (c)  1999 - 2011 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file SoftStrapsPeim.h

  Header file for the SoftStrapsPeim module.

**/

#ifndef _SOFTSTRAPS_PEIM_H_
#define _SOFTSTRAPS_PEIM_H_

#include <Ppi/SpiSoftStraps.h>

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
//
// CP_SYNC_REF_BEGIN
//
#include <Library/UsraAccessApi.h>
//
// CP_SYNC_REF_END
//

//-----------------------------------------------------------------------
// SPI control regs.
//-----------------------------------------------------------------------
#define R_SB_SPI_HSFSTS 0x04
#define   B_SB_SPI_LOCK_DOWN 0x8000;

#define R_SB_SPI_HSFCTL 0x06
#define   B_SB_SPI_READ_CYCLE  BIT0;
#define   B_SB_SPI_WRITE_CYCLE BIT2|BIT0;
#define   B_SB_SPI_BLOCK_ERASE BIT2|BIT1|BIT0;
#define   B_SB_SPI_GO_CYCLE    BIT0;

#define R_SB_SPI_FADDR  0x08
#define DESCRIPT_SIZE   0x1000

#define R_SB_SPI_FDAT0 0x10
#define R_SB_SPI_FDAT1 0x14
#define R_SB_SPI_FDAT2 0x18
#define R_SB_SPI_FDAT3 0x1C
#define R_SB_SPI_FDAT4 0x20
#define R_SB_SPI_FDAT5 0x24
#define R_SB_SPI_FDAT6 0x28
#define R_SB_SPI_FDAT7 0x2C
#define R_SB_SPI_FDAT8 0x30
#define R_SB_SPI_FDAT9 0x34
#define R_SB_SPI_FDATA 0x38
#define R_SB_SPI_FDATB 0x3C
#define R_SB_SPI_FDATC 0x40
#define R_SB_SPI_FDATD 0x44
#define R_SB_SPI_FDATE 0x48
#define R_SB_SPI_FDATF 0x4C

#define R_SB_SPI_FRACC 0x50
#define   B_SB_SPI_BIOS_WRITE  BIT31|BIT30|BIT29|BIT28|BIT27|BIT26|BIT25|BIT24;
#define   B_SB_SPI_BIOS_READ   BIT23|BIT22|BIT21|BIT20|BIT19|BIT18|BIT17|BIT16;
#define   B_SB_SPI_REG_WRITE   BIT15|BIT14|BIT13|BIT12|BIT11|BIT10| BIT9| BIT8;
#define   B_SB_SPI_REG_READ     BIT7| BIT6| BIT5| BIT4| BIT3| BIT2| BIT1| BIT0;

#define R_SB_SPI_PR0 0x74
#define   B_SB_SPI_PROT_ENABLE BIT31;

#define R_SB_SPI_LVSCC 0xC4
#define R_SB_SPI_UVSCC 0xC8

#define DESCRIP_ADDR        0x10;
#define DESCRIP_VSCC        0xEF0;
#define DESCRIP_FLVALSIG    0x10;
#define DESCRIP_FLMAP0      0x14;
#define DESCRIP_FLMAP1      0x18;
#define DESCRIP_FLMAP2      0x1C;

#define VALID_SPI_SIGNATURE 0x0FF0A55A;

#define PCHSTRP0   (0x0*4)
#define PCHSTRP1   (0x1*4)
#define PCHSTRP2   (0x2*4)
#define PCHSTRP3   (0x3*4)
#define PCHSTRP4   (0x4*4)
#define PCHSTRP5   (0x5*4)
#define PCHSTRP6   (0x6*4)
#define PCHSTRP7   (0x7*4)
#define PCHSTRP8   (0x8*4)
#define PCHSTRP9   (0x9*4)
#define PCHSTRP10  (0x10*4)
#define PCHSTRP11  (0x11*4)
#define PCHSTRP12  (0x12*4)
#define PCHSTRP13  (0x13*4)
#define PCHSTRP14  (0x14*4)
#define PCHSTRP15  (0x15*4)
#define PCHSTRP16  (0x16*4)
#define PCHSTRP17  (0x17*4)

#define BLOCK_SIZE_256B  0x100
#define BLOCK_SIZE_4KB   0x1000
#define BLOCK_SIZE_8KB   0x2000
#define BLOCK_SIZE_64KB  0x10000


#define SPI_VSCC_MINUM_COMMON_DENOMINATOR  0xD817


#define RESET_PORT                0x0CF9
#define CLEAR_RESET_BITS          0x0F1
#define COLD_RESET                0x02  // Set bit 1 for cold reset
#define RST_CPU                   0x04  // Setting this bit triggers a reset of the CPU
#define FULL_RESET                0x08  // Set bit 4 with bit 1 for full reset

#define SPI_VSCC_MINUM_COMMUN_DENOMINATOR  0xD817

#define SB_RCBA           0x0FED1C000
#define SPI_BASE_ADDRESS  (SB_RCBA+0x3800)
#endif
