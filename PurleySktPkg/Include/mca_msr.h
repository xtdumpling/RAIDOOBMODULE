/** @file
  This is eMCA platform hooks library private definitions

  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

  Copyright (c) 2009-2015 Intel Corporation.  All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license.  Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _MCA_MSR_H_
#define _MCA_MSR_H_

//
// Defines
//
#define MCI_REG_BASE    0x400

#define IFU_CTL      (MCI_REG_BASE + 0x00)
#define DCU_CTL      (MCI_REG_BASE + 0x04)
#define DTLB_CTL     (MCI_REG_BASE + 0x08)
#define MLC_CTL      (MCI_REG_BASE + 0x0C)
#define PCU_CTL      (MCI_REG_BASE + 0x10)
#define QPI0_CTL     (MCI_REG_BASE + 0x14)
#define IIO_CTL      (MCI_REG_BASE + 0x18)
#define HA0_CTL      (MCI_REG_BASE + 0x1C)
#define HA1_CTL      (MCI_REG_BASE + 0x20)
#define IMC0_CTL     (MCI_REG_BASE + 0x24)
#define IMC1_CTL     (MCI_REG_BASE + 0x28)
#define IMC2_CTL     (MCI_REG_BASE + 0x2C)
#define IMC3_CTL     (MCI_REG_BASE + 0x30)
#define IMC4_CTL     (MCI_REG_BASE + 0x34)
#define IMC5_CTL     (MCI_REG_BASE + 0x38)
#define IMC6_CTL     (MCI_REG_BASE + 0x3C)
#define IMC7_CTL     (MCI_REG_BASE + 0x40)
#define CBO00_CTL    (MCI_REG_BASE + 0x44)
#define CBO01_CTL    (MCI_REG_BASE + 0x48)
#define CBO02_CTL    (MCI_REG_BASE + 0x4C)
#define CBO03_CTL    (MCI_REG_BASE + 0x50)
#define CBO04_CTL    (MCI_REG_BASE + 0x54)
#define CBO05_CTL    (MCI_REG_BASE + 0x58)
#define CBO06_CTL    (MCI_REG_BASE + 0x5C)
#define CBO07_CTL    (MCI_REG_BASE + 0x60)
#define CBO08_CTL    (MCI_REG_BASE + 0x64)
#define CBO09_CTL    (MCI_REG_BASE + 0x68)
#define CBO10_CTL    (MCI_REG_BASE + 0x6C)
#define CBO11_CTL    (MCI_REG_BASE + 0x70)
#define CBO12_CTL    (MCI_REG_BASE + 0x74)
#define CBO13_CTL    (MCI_REG_BASE + 0x78)
#define CBO14_CTL    (MCI_REG_BASE + 0x7C)

#define MSR_MC00_CTL IFU_CTL
#define MSR_MC01_CTL DCU_CTL
#define MSR_MC02_CTL DTLB_CTL
#define MSR_MC03_CTL MLC_CTL      
#define MSR_MC04_CTL PCU_CTL      
#define MSR_MC05_CTL QPI0_CTL     
#define MSR_MC06_CTL IIO_CTL      
#define MSR_MC07_CTL HA0_CTL
#define MSR_MC08_CTL HA1_CTL
#define MSR_MC09_CTL IMC0_CTL
#define MSR_MC10_CTL IMC1_CTL
#define MSR_MC11_CTL IMC2_CTL
#define MSR_MC12_CTL IMC3_CTL
#define MSR_MC13_CTL IMC4_CTL
#define MSR_MC14_CTL IMC5_CTL
#define MSR_MC15_CTL IMC6_CTL
#define MSR_MC16_CTL IMC7_CTL
#define MSR_MC17_CTL CBO00_CTL
#define MSR_MC18_CTL CBO01_CTL
#define MSR_MC19_CTL CBO02_CTL
#define MSR_MC20_CTL CBO03_CTL
#define MSR_MC21_CTL CBO04_CTL
#define MSR_MC22_CTL CBO05_CTL
#define MSR_MC23_CTL CBO06_CTL
#define MSR_MC24_CTL CBO07_CTL
#define MSR_MC25_CTL CBO08_CTL
#define MSR_MC26_CTL CBO09_CTL
#define MSR_MC27_CTL CBO10_CTL
#define MSR_MC28_CTL CBO11_CTL
#define MSR_MC29_CTL CBO12_CTL
#define MSR_MC30_CTL CBO13_CTL
#define MSR_MC31_CTL CBO14_CTL

#define MSR_MC00_STATUS (MSR_MC00_CTL + 0x1)
#define MSR_MC00_ADDR   (MSR_MC00_CTL + 0x2)
#define MSR_MC00_MISC   (MSR_MC00_CTL + 0x3)
#define MSR_MC01_STATUS (MSR_MC01_CTL + 0x1)
#define MSR_MC01_ADDR   (MSR_MC01_CTL + 0x2)
#define MSR_MC01_MISC   (MSR_MC01_CTL + 0x3)
#define MSR_MC02_STATUS (MSR_MC02_CTL + 0x1)
#define MSR_MC02_ADDR   (MSR_MC02_CTL + 0x2)
#define MSR_MC02_MISC   (MSR_MC02_CTL + 0x3)
#define MSR_MC03_STATUS (MSR_MC03_CTL + 0x1)
#define MSR_MC03_ADDR   (MSR_MC03_CTL + 0x2)
#define MSR_MC03_MISC   (MSR_MC03_CTL + 0x3)
#define MSR_MC04_STATUS (MSR_MC04_CTL + 0x1)
#define MSR_MC04_ADDR   (MSR_MC04_CTL + 0x2)
#define MSR_MC04_MISC   (MSR_MC04_CTL + 0x3)
#define MSR_MC05_STATUS (MSR_MC05_CTL + 0x1)
#define MSR_MC05_ADDR   (MSR_MC05_CTL + 0x2)
#define MSR_MC05_MISC   (MSR_MC05_CTL + 0x3)
#define MSR_MC06_STATUS (MSR_MC06_CTL + 0x1)
#define MSR_MC06_ADDR   (MSR_MC06_CTL + 0x2)
#define MSR_MC06_MISC   (MSR_MC06_CTL + 0x3)
#define MSR_MC07_STATUS (MSR_MC07_CTL + 0x1)
#define MSR_MC07_ADDR   (MSR_MC07_CTL + 0x2)
#define MSR_MC07_MISC   (MSR_MC07_CTL + 0x3)
#define MSR_MC08_STATUS (MSR_MC08_CTL + 0x1)
#define MSR_MC08_ADDR   (MSR_MC08_CTL + 0x2)
#define MSR_MC08_MISC   (MSR_MC08_CTL + 0x3)
#define MSR_MC09_STATUS (MSR_MC09_CTL + 0x1)
#define MSR_MC09_ADDR   (MSR_MC09_CTL + 0x2)
#define MSR_MC09_MISC   (MSR_MC09_CTL + 0x3)
#define MSR_MC10_STATUS (MSR_MC10_CTL + 0x1)
#define MSR_MC10_ADDR   (MSR_MC10_CTL + 0x2)
#define MSR_MC10_MISC   (MSR_MC10_CTL + 0x3)
#define MSR_MC11_STATUS (MSR_MC11_CTL + 0x1)
#define MSR_MC11_ADDR   (MSR_MC11_CTL + 0x2)
#define MSR_MC11_MISC   (MSR_MC11_CTL + 0x3)
#define MSR_MC12_STATUS (MSR_MC12_CTL + 0x1)
#define MSR_MC12_ADDR   (MSR_MC12_CTL + 0x2)
#define MSR_MC12_MISC   (MSR_MC12_CTL + 0x3)
#define MSR_MC13_STATUS (MSR_MC13_CTL + 0x1)
#define MSR_MC13_ADDR   (MSR_MC13_CTL + 0x2)
#define MSR_MC13_MISC   (MSR_MC13_CTL + 0x3)
#define MSR_MC14_STATUS (MSR_MC14_CTL + 0x1)
#define MSR_MC14_ADDR   (MSR_MC14_CTL + 0x2)
#define MSR_MC14_MISC   (MSR_MC14_CTL + 0x3)
#define MSR_MC15_STATUS (MSR_MC15_CTL + 0x1)
#define MSR_MC15_ADDR   (MSR_MC15_CTL + 0x2)
#define MSR_MC15_MISC   (MSR_MC15_CTL + 0x3)
#define MSR_MC16_STATUS (MSR_MC16_CTL + 0x1)
#define MSR_MC16_ADDR   (MSR_MC16_CTL + 0x2)
#define MSR_MC16_MISC   (MSR_MC16_CTL + 0x3)
#define MSR_MC17_STATUS (MSR_MC17_CTL + 0x1)
#define MSR_MC17_ADDR   (MSR_MC17_CTL + 0x2)
#define MSR_MC17_MISC   (MSR_MC17_CTL + 0x3)
#define MSR_MC18_STATUS (MSR_MC18_CTL + 0x1)
#define MSR_MC18_ADDR   (MSR_MC18_CTL + 0x2)
#define MSR_MC18_MISC   (MSR_MC18_CTL + 0x3)
#define MSR_MC19_STATUS (MSR_MC19_CTL + 0x1)
#define MSR_MC19_ADDR   (MSR_MC19_CTL + 0x2)
#define MSR_MC19_MISC   (MSR_MC19_CTL + 0x3)
#define MSR_MC20_STATUS (MSR_MC20_CTL + 0x1)
#define MSR_MC20_ADDR   (MSR_MC20_CTL + 0x2)
#define MSR_MC20_MISC   (MSR_MC20_CTL + 0x3)
#define MSR_MC21_STATUS (MSR_MC21_CTL + 0x1)
#define MSR_MC21_ADDR   (MSR_MC21_CTL + 0x2)
#define MSR_MC21_MISC   (MSR_MC21_CTL + 0x3)
#define MSR_MC22_STATUS (MSR_MC22_CTL + 0x1)
#define MSR_MC22_ADDR   (MSR_MC22_CTL + 0x2)
#define MSR_MC22_MISC   (MSR_MC22_CTL + 0x3)
#define MSR_MC23_STATUS (MSR_MC23_CTL + 0x1)
#define MSR_MC23_ADDR   (MSR_MC23_CTL + 0x2)
#define MSR_MC23_MISC   (MSR_MC23_CTL + 0x3)
#define MSR_MC24_STATUS (MSR_MC24_CTL + 0x1)
#define MSR_MC24_ADDR   (MSR_MC24_CTL + 0x2)
#define MSR_MC24_MISC   (MSR_MC24_CTL + 0x3)
#define MSR_MC25_STATUS (MSR_MC25_CTL + 0x1)
#define MSR_MC25_ADDR   (MSR_MC25_CTL + 0x2)
#define MSR_MC25_MISC   (MSR_MC25_CTL + 0x3)
#define MSR_MC26_STATUS (MSR_MC26_CTL + 0x1)
#define MSR_MC26_ADDR   (MSR_MC26_CTL + 0x2)
#define MSR_MC26_MISC   (MSR_MC26_CTL + 0x3)
#define MSR_MC27_STATUS (MSR_MC27_CTL + 0x1)
#define MSR_MC27_ADDR   (MSR_MC27_CTL + 0x2)
#define MSR_MC27_MISC   (MSR_MC27_CTL + 0x3)
#define MSR_MC28_STATUS (MSR_MC28_CTL + 0x1)
#define MSR_MC28_ADDR   (MSR_MC28_CTL + 0x2)
#define MSR_MC28_MISC   (MSR_MC28_CTL + 0x3)
#define MSR_MC29_STATUS (MSR_MC29_CTL + 0x1)
#define MSR_MC29_ADDR   (MSR_MC29_CTL + 0x2)
#define MSR_MC29_MISC   (MSR_MC29_CTL + 0x3)
#define MSR_MC30_STATUS (MSR_MC30_CTL + 0x1)
#define MSR_MC30_ADDR   (MSR_MC30_CTL + 0x2)
#define MSR_MC30_MISC   (MSR_MC30_CTL + 0x3)
#define MSR_MC31_STATUS (MSR_MC31_CTL + 0x1)
#define MSR_MC31_ADDR   (MSR_MC31_CTL + 0x2)
#define MSR_MC31_MISC   (MSR_MC31_CTL + 0x3)

#define MSR_MC00_CTL2   (0x280)
#define MSR_MC01_CTL2   (MSR_MC00_CTL2 + 0x1)
#define MSR_MC02_CTL2   (MSR_MC01_CTL2 + 0x1)
#define MSR_MC03_CTL2   (MSR_MC02_CTL2 + 0x1)
#define MSR_MC04_CTL2   (MSR_MC03_CTL2 + 0x1)
#define MSR_MC05_CTL2   (MSR_MC04_CTL2 + 0x1)
#define MSR_MC06_CTL2   (MSR_MC05_CTL2 + 0x1)
#define MSR_MC07_CTL2   (MSR_MC06_CTL2 + 0x1)
#define MSR_MC08_CTL2   (MSR_MC07_CTL2 + 0x1)
#define MSR_MC09_CTL2   (MSR_MC08_CTL2 + 0x1)
#define MSR_MC10_CTL2   (MSR_MC09_CTL2 + 0x1)
#define MSR_MC11_CTL2   (MSR_MC10_CTL2 + 0x1)
#define MSR_MC12_CTL2   (MSR_MC11_CTL2 + 0x1)
#define MSR_MC13_CTL2   (MSR_MC12_CTL2 + 0x1)
#define MSR_MC14_CTL2   (MSR_MC13_CTL2 + 0x1)
#define MSR_MC15_CTL2   (MSR_MC14_CTL2 + 0x1)
#define MSR_MC16_CTL2   (MSR_MC15_CTL2 + 0x1)
#define MSR_MC17_CTL2   (MSR_MC16_CTL2 + 0x1)
#define MSR_MC18_CTL2   (MSR_MC17_CTL2 + 0x1)
#define MSR_MC19_CTL2   (MSR_MC18_CTL2 + 0x1)
#define MSR_MC20_CTL2   (MSR_MC19_CTL2 + 0x1)
#define MSR_MC21_CTL2   (MSR_MC20_CTL2 + 0x1)
#define MSR_MC22_CTL2   (MSR_MC21_CTL2 + 0x1)
#define MSR_MC23_CTL2   (MSR_MC22_CTL2 + 0x1)
#define MSR_MC24_CTL2   (MSR_MC23_CTL2 + 0x1)
#define MSR_MC25_CTL2   (MSR_MC24_CTL2 + 0x1)
#define MSR_MC26_CTL2   (MSR_MC25_CTL2 + 0x1)
#define MSR_MC27_CTL2   (MSR_MC26_CTL2 + 0x1)
#define MSR_MC28_CTL2   (MSR_MC27_CTL2 + 0x1)
#define MSR_MC29_CTL2   (MSR_MC28_CTL2 + 0x1)
#define MSR_MC30_CTL2   (MSR_MC29_CTL2 + 0x1)
#define MSR_MC31_CTL2   (MSR_MC30_CTL2 + 0x1)

#define MCBANK_MAX                0x20

#define MSR_SMM_MCA_CAP           0x17D
#define MSR_CORE_SMI_ERR_SRC      0x17c
#define MSR_UNCORE_SMI_ERR_SRC    0x17e
#define MCA_ERROR_CONTROL                     0x52

#define MCA_ADDR_BIT_MASK (0x3fffffffffff) //46 bits
#define MCA_MISC_LSB_MASK (0x3f)

#define N_MSR_MCG_COUNT_START                       0
#define N_MSR_MCG_COUNT_STOP                        7

#define IA32_MCG_STATUS                             0x17A
#define MSR_IA32_MCG_CAP                            0x179
#define IA32_MCG_EXT_CTL                            0x4D0
#define N_MSR_IA32_LMCE_EN                          BIT0
#define N_MSR_MCG_LMCE_P_BIT                        BIT27
#define N_MSR_MCG_MCG_SER_P                         24

#define N_MSR_LMCE_ON_BIT                           20
#define N_MSR_FEATURE_CONTROL_LOCK                  0

#define MCA_FIRMWARE_UPDATED_BIT                    BIT37

#define IMC_PATROL_SCRUB_ERR_MCACOD                 0xC0
#define IMC_PATROL_SCRUB_ERR_MCACOD_MSK             0xFFF0
#define IMC_UNC_PATROL_SCRUB_ERR_MSCOD              0x10
#define IMC_COR_PATROL_SCRUB_ERR_MSCOD              0x08
#define IMC_PATROL_SCRUB_ERR_MSCOD_MSK              0xFFFF

#define IMC_SPARING_ERR_MCACOD                      0xC0
#define IMC_SPARING_ERR_MCACOD_MSK                  0xFFF0
#define IMC_UNC_SPARING_ERR_MSCOD                   0x40
#define IMC_COR_SPARING_ERR_MSCOD                   0x20
#define IMC_SPARING_ERR_MSCOD_MSK                   0xFFFF

// DDR4 C/A Parity Error.

#define IMC_DDR4_CAP_ERR_MCACOD             0x0B0
#define IMC_DDR4_CAP_ERR_MCACOD_MSK         0x0F0
#define IMC_DDR4_CAP_ERR_MSCOD              0x0200
#define IMC_DDR4_CAP_ERR_MSCOD_MSK          0x0FFF
#define IMC_DDRT_CAP_ERR_MCACOD             0x80
#define IMC_DDRT_CAP_ERR_MCACOD_MSK         0xF0
#define IMC_DDRT_CAP_ERR_MSCOD              0x80d
#define IMC_DDRT_CAP_ERR_MSCOD_MSK          0x0FFF

#define IMC_MCACOD_CHANNEL_MSK             0x000F
#define M2M0_BANK  7
#define M2M1_BANK  8


typedef union {
   struct{
      UINT64 mca_code:16;
                        /* MCA architected error code */
      UINT64 ms_code:16;
                        /* Model specific error code */
      UINT64 other_info:6;
      UINT64 corrected_err_count:15;
      UINT64 threshold_err_status:2;
      UINT64 ar:1;
                        /* Recovery action required for UCR error */
      UINT64 s:1;
                        /* Signaling an uncorrected recoverable (UCR) error*/
      UINT64 pcc:1;     
                        /* processor context corrupted */
      UINT64 addrv:1;  
                        /* MCi_ADDR valid */
      UINT64 miscv:1;
                        /* MCi_MISC valid */
      UINT64 en:1;
                        /* error reporting enabled */
      UINT64 uc:1;
                        /* uncorrected error */
      UINT64 over:1;
                        /* error overflow */
      UINT64 val:1;
                        /* MCi_STATUS valid */
   } Bits;
   UINT64 Data;
} IA32_MCI_STATUS_MSR_STRUCT;

typedef union {
  struct {
    UINT32 ripv:1;
    UINT32 eipv:1;
    UINT32 mcip:1;
    UINT32 lmce:1;
    UINT32 rsvd:28;
  } Bits;
  UINT32 Data;
} IA32_MCG_STATUS_MSR_STRUCT;

typedef union {
   struct{
      UINT64 error_threshold:15;
                        /* Corrected error count threshold */
      UINT64 reserved0:15;
                        /* reserved */
      UINT64 CMCI_EN:1;
                        /*Enable/Disable CMCI*/
      UINT64 reserved1:33;
                        /* reserved */
   } Bits;
   UINT64 Data;
} IA32_MCI_CTL2_MSR_STRUCT;

//
// Functions prototype declarations 
//

#endif  // _MCA_MSR_H_

