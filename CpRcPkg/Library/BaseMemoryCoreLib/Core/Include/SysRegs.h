//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

  Reference Code

  ESS - Enterprise Silicon Software

  INTEL CONFIDENTIAL

@copyright
  Copyright 2006 - 2015 Intel Corporation All Rights Reserved.

  The source code contained or described herein and all documents
  related to the source code ("Material") are owned by Intel
  Corporation or its suppliers or licensors. Title to the Material
  remains with Intel Corporation or its suppliers and licensors.
  The Material contains trade secrets and proprietary and confidential
  information of Intel or its suppliers and licensors. The Material
  is protected by worldwide copyright and trade secret laws and treaty
  provisions.  No part of the Material may be used, copied, reproduced,
  modified, published, uploaded, posted, transmitted, distributed, or
  disclosed in any way without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other
  intellectual property right is granted to or conferred upon you
  by disclosure or delivery of the Materials, either expressly,
  by implication, inducement, estoppel or otherwise. Any license
  under such intellectual property rights must be express and
  approved by Intel in writing.


@file
  SysRegs.h

@brief
  This file contains memory detection and initialization for
  IMC and DDR3 modules compliant with JEDEC specification.

**/

#ifndef _sysregs_h
#define _sysregs_h

#include "DataTypes.h"

#ifndef BIT0
#define BIT0  1
#define BIT1  (1 << 1)
#define BIT2  (1 << 2)
#define BIT3  (1 << 3)
#define BIT4  (1 << 4)
#define BIT5  (1 << 5)
#define BIT6  (1 << 6)
#define BIT7  (1 << 7)
#define BIT8  (1 << 8)
#define BIT9  (1 << 9)
#endif
#ifndef BIT10
#define BIT10 (1 << 10)
#define BIT11 (1 << 11)
#define BIT12 (1 << 12)
#define BIT13 (1 << 13)
#define BIT14 (1 << 14)
#define BIT15 (1 << 15)
#define BIT16 (1 << 16)
#define BIT17 (1 << 17)
#define BIT18 (1 << 18)
#define BIT19 (1 << 19)
#define BIT20 (1 << 20)
#define BIT21 (1 << 21)
#define BIT22 (1 << 22)
#define BIT23 (1 << 23)
#define BIT24 (1 << 24)
#define BIT25 (1 << 25)
#define BIT26 (1 << 26)
#define BIT27 (1 << 27)
#define BIT28 (1 << 28)
#define BIT29 (1 << 29)
#define BIT30 (1 << 30)
#define BIT31 (UINT32) (1 << 31)
#endif

#ifndef TRUE
#define TRUE  ((BOOLEAN) 1 == 1)
#endif

#ifndef FALSE
#define FALSE ((BOOLEAN) 0 == 1)
#endif

#ifndef ABS
#define ABS(x)  (((x) < 0) ? (-x) : (x))
#endif
//
// disable compiler warning to use bit fields on unsigned short/long types
//
#ifdef _MSC_VER
#pragma warning(disable : 4214)
#endif

#ifndef ASM_INC
//
// Base structures
//
struct smbAddress {
  UINT16 rsvd:1;              ///< reserved bit
  UINT16 strapAddress:3;      ///< SMBus slave address (0...7)
  UINT16 deviceType:4;        ///< '0011' specifies TSOD, '1010' specifies EEPROM's, '0101' specifies other
  UINT16 busSegment:4;        ///< SMBus Segment
  UINT16 controller:4;        ///< PROCESSOR, ICH, etc...
};

struct smbDevice {
  UINT16            compId;   ///< SPD, MTS, DCP_ISL9072X, DCP_AD5247, DCP_UP6262, DCP_AD5337
  UINT8             mcId;     ///< Memory Controller ID
  struct smbAddress address;  ///< SMBus Segment
};

/*struct gpioDevice {
  UINT16  compId;             ///< ICH, SIO3, PCA9555, PCA9554, NO_DEV
  UINT16  address;            ///< I/O port address or struct smbAddress
}; */


//
// MSR definitions
//
#define PCIEXBAR_MSR  0x300

//#include "RcRegs.h"

//
// macros that converts PROCESSOR reg defines into legacy PCI CF8 address format
//
#define PCI_LEGACY_ADDR_BUS0_1(x) 0x80000000 + (((x & 0x0ffff000) >> 4) + (x & 0x000000ff))
#define PCI_LEGACY_ADDR_BUS0(x)   0x80000000 + (((x & 0x000ff000) >> 4) + (x & 0x000000ff))


#ifndef  _SB_H   //To Avoid this header get compiled twice

#define PCH_DEV_MIN   16
#define LPC_BUS       0
#define LPC_DEVICE    31
#define P2P_DEVICE    30
#define EHCI1_DEVICE  29
#define HDA_DEVICE    27
#define EHCI2_DEVICE  26
#define EHCI1_FUNC    7
#define EHCI2_FUNC    7
#define LPC_FUNC      0
#define PMC_BUS       0
#define PMC_DEVICE    31
#define PMC_FUNC      2

//
//-----------------------------------------------------------------------;
// PCH: SMBus I/O Space Equates
//-----------------------------------------------------------------------;
//
#define BIT_SLAVE_ADDR        BIT0
#define BIT_COMMAND           BIT1
#define BIT_DATA              BIT2
#define BIT_COUNT             BIT3
#define BIT_WORD              BIT4
#define BIT_CONTROL           BIT5
#define BIT_PEC               BIT6
#define BIT_READ              BIT7
#define SMBUS_IO_READ_BIT     BIT0

#define SMB_CMD_QUICK         0x00
#define SMB_CMD_BYTE          0x04
#define SMB_CMD_BYTE_DATA     0x08
#define SMB_CMD_WORD_DATA     0x0C
#define SMB_CMD_PROCESS_CALL  0x10
#define SMB_CMD_BLOCK         0x14
#define SMB_CMD_I2C_READ      0x18
#define SMB_CMD_RESERVED      0x1c

#define PCH_SMBUS_FUNC  3

#define HOST_STATUS_REGISTER          0x00  // Host Status Register R/W
#define   HST_STS_HOST_BUSY             BIT0  // RO
#define   HST_STS_INTR                  BIT1  // R/WC
#define   HST_STS_DEV_ERR               BIT2  // R/WC
#define   HST_STS_BUS_ERR               BIT3  // R/WC
#define   HST_STS_FAILED                BIT4  // R/WC
//SKX_TODO: These cause a redef error, as the originals are defined elsewhere
//SKX_TODO: so I renamed them to conform to the rest of the names here.
#define   HST_SMBUS_B_SMBALERT          BIT5  // R/WC
#define   HST_STS_INUSE                 BIT6  // R/WC
#define   HST_SMBUS_B_BYTE_DONE         BIT7  // R/WC
#define   SMBUS_B_HSTS_ALL              0xFF  // R/WC
#define HOST_CONTROL_REGISTER         0x02  // Host Control Register R/W
#define   HST_CNT_INTREN                BIT0  // RW
#define   HST_CNT_KILL                  BIT1  // RW
#define   SMBUS_B_SMB_CMD               0x1C  // RW
#define     SMBUS_V_SMB_CMD_QUICK         0x00
#define     SMBUS_V_SMB_CMD_BYTE          0x04
#define     SMBUS_V_SMB_CMD_BYTE_DATA     0x08
#define     SMBUS_V_SMB_CMD_WORD_DATA     0x0C
#define     SMBUS_V_SMB_CMD_PROCESS_CALL  0x10
#define     SMBUS_V_SMB_CMD_BLOCK         0x14
#define     SMBUS_V_SMB_CMD_IIC_READ      0x18
#define   HST_SMBUS_B_LAST_BYTE         BIT5  // WO
#define   HST_CNT_START                 BIT6  // WO
#define   HST_CNT_PEC_EN                BIT7  // RW
#define  HOST_COMMAND_REGISTER        0x03  // Host Command Register R/W
#define  XMIT_SLAVE_ADDRESS_REGISTER  0x04  // Transmit Slave Address Register R/W
#define   SMBUS_B_RW_SEL                0x01  // RW
#define   SMBUS_B_ADDRESS               0xFE  // RW
#define  HOST_DATA_0_REGISTER         0x05  // Data 0 Register R/W
#define  HOST_DATA_1_REGISTER         0x06  // Data 1 Register R/W
#define  HOST_BLOCK_DATA_BYTE_REGISTER 0x07  // Host Block Data Register R/W
#define SMBUS_R_PEC                   0x08  // Packet Error Check Data Register R/W
#define SMBUS_R_RSA                   0x09  // Receive Slave Address Register R/W
#define   SMBUS_B_SLAVE_ADDR            0x7F  // RW
#define SMBUS_R_SD                    0x0A  // Receive Slave Data Register R/W
#define SMBUS_R_AUXS                  0x0C  // Auxiliary Status Register R/WC
#define   SMBUS_B_CRCE                  0x01  //R/WC
#define AUXILIARY_CONTROL_REGISTER    0x0D  // Auxiliary Control Register R/W
#define   SMBUS_B_AAC                   0x01  //R/W
#define   SMBUS_B_E32B                  0x02  //R/W

// ADR related PCH defines
#define PM_SYNC_MISC_CFG         0xC8
#define PM_SYNC_MODE             0xD4
#define ADR_EN                   0xF0
#define SSB_PM_STS               0x1C
#define GPIO_GP_0_SEL            (1 << 9)
#define PWRM_BASE_REG            ((4 << 28) | (0x2 << 20) | (0x1F << 15) | (0x2 << 12) | 0x48)
#define PWRM_BASE_ADDRESS        0xFE000000
#define ADR_RST_STS              (1 << 16)
#define GPIO_B_PM_SYNC_EN        (1 << 29)
#define GPIO_B_C0_TX_EN          (1 << 13)
#define REG_10_C0_TX_EN          (1 << 9)
#define ADR_GPIO_SEL_B           (1 << 30)
#define ADR_GPIO_RST_EN          (1 << 29)
#define ADR_FEAT_EN              (1 << 0)
#define GPIO_BASE_REG            ((4 << 28) | (0x2 << 20) | (0x1F << 15) | 0x48)
#define GPIO_INV         0x2C
#endif //_SB_H

#define BYTE_DATA (HST_CNT_START | SMBUS_V_SMB_CMD_BYTE_DATA) //0x48 = [4:2] = 010
#define WORD_DATA (HST_CNT_START | SMBUS_V_SMB_CMD_WORD_DATA) //0x4C = [4:2] = 011
#define SR_DATA   (HST_CNT_START | SMBUS_V_SMB_CMD_BYTE)      //0x44 = [4:2] = 001
#define BLOCK_DATA (HST_CNT_START | SMBUS_V_SMB_CMD_BLOCK)    //0x54 = [4:2] = 101

//----------------------------------------------------------------------
//Define Dallas DS12C887 Real Time Clock INDEX and DATA registers
//----------------------------------------------------------------------
#define RTC_INDEX_REG       0x70
#define RTC_DATA_REG        0x71

//----------------------------------------------------------------------
//Define Internal Registers for Dallas DS12C887 Real Time Clock
//----------------------------------------------------------------------
#define RTC_SECONDS_REG         0x00    // R/W  Range 0..59
#define RTC_SECONDS_ALARM_REG   0x01    // R/W  Range 0..59
#define RTC_MINUTES_REG         0x02    // R/W  Range 0..59
#define RTC_MINUTES_ALARM_REG   0x03    // R/W  Range 0..59
#define RTC_HOURS_REG           0x04    // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define RTC_HOURS_ALARM_REG     0x05    // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define RTC_DAY_OF_WEEK_REG     0x06    // R/W  Range 1..7
#define RTC_DAY_OF_MONTH_REG    0x07    // R/W  Range 1..31
#define RTC_MONTH_REG           0x08    // R/W  Range 1..12
#define RTC_YEAR_REG            0x09    // R/W  Range 0..99
#define RTC_REG_A_INDEX         0x0a    // R/W[0..6]  R0[7]
#define RTC_REG_B_INDEX         0x0b    // R/W
#define RTC_REG_C_INDEX         0x0c    // RO
#define RTC_REG_D_INDEX         0x0d    // RO

#define RTC_NMI_MASK            0x80    // NMI mask bit.

#endif // ASM_INC


#endif // _sysregs_h
