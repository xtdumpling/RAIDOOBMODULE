/** @file
  Register names for PCH private chipset register

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_[generation_name]_" in register/bit names.
  - Registers / bits that are specific to PCH-H denoted by "_H_" in register/bit names.
    Registers / bits that are specific to PCH-LP denoted by "_LP_" in register/bit names.
    e.g., "_PCH_H_", "_PCH_LP_"
    Registers / bits names without _H_ or _LP_ apply for both H and LP.
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without [generation_name] inserted.

@copyright
 Copyright (c) 2013 - 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PCH_REGS_PCR_H_
#define _PCH_REGS_PCR_H_

///
/// Definition for PCR base address (defined in PchReservedResources.h)
///
//#define PCH_PCR_BASE_ADDRESS            0xFD000000
//#define PCH_PCR_MMIO_SIZE               0x01000000
/**
  Definition for PCR address
  The PCR address is used to the PCR MMIO programming
**/
#define PCH_PCR_ADDRESS(Pid, Offset)    (PCH_PCR_BASE_ADDRESS | ((UINT8)(Pid) << 16) | (UINT16)(Offset))

/**
  Definition for SBI PID
  The PCH_SBI_PID defines the PID for PCR MMIO programming and PCH SBI programming as well.
**/
typedef enum {
  PID_BROADCAST1                        = 0xFF,
  PID_BROADCAST2                        = 0xFE,
  //Rsv                                 = 0xFD-0xF0,
  PID_DMI                               = 0xEF,
  PID_ESPISPI                           = 0xEE,
  PID_ICLK                              = 0xED,
  PID_MODPHY4                             = 0xEB,
  PID_MODPHY5                             = 0x10,
  PID_MODPHY1                             = 0xE9,
  PID_PMC                               = 0xE8,
  //Rsv                                 = 0xE7,
  PID_XHCI                              = 0xE6,
  PID_OTG                               = 0xE5,
  PID_SPE                               = 0xE4,        // Reserved in SKL PCH LP
  PID_SPD                               = 0xE3,        // Reserved in SKL PCH LP
  PID_SPC                               = 0xE2,
  PID_SPB                               = 0xE1,
  PID_SPA                               = 0xE0,
  PID_UPSX8                             = 0x06,
  PID_UPSX16                            = 0x07,
  PID_TAP2IOSFSB1                       = 0xDF,
  PID_TRSB                              = 0xDD,
  PID_ICC                               = 0xDC,
  PID_GBE                               = 0xDB,
  //Rsv                                 = 0xDA,
  PID_SATA                              = 0xD9,
  PID_SSATA                             = 0x0F,
  PID_LDO                               = 0x14,
  //Rsv                                 = 0xD8,
  PID_DSP                               = 0xD7,
  //Rsv                                 = 0xD6,
  PID_FUSE                              = 0xD5,
  PID_FSPROX0                           = 0xD4,
  PID_DRNG                              = 0xD2,
  //Rsv                                 = 0xD1,
  PID_FIA                               = 0xCF,
  PID_FIAWM26                           = 0x13,
  //Rsv                                 = 0xCE-0xCC,
  PID_USB2                              = 0xCA,
  //Rsv                                 = 0xC8,
  PID_LPC                               = 0xC7,
  PID_SMB                               = 0xC6,
  PID_P2S                               = 0xC5,
  PID_ITSS                              = 0xC4,
  PID_RTC                               = 0xC3,
  //Rsv                                 = 0xC2-0xC1,
  PID_PSF5                              = 0x8F,
  PID_PSF6                              = 0x70,
  PID_PSF7                              = 0x01,
  PID_PSF8                              = 0x29,
  PID_PSF9                              = 0x21,
  PID_PSF10                             = 0x36,
  PID_PSF4                              = 0xBD,
  PID_PSF3                              = 0xBC,
  PID_PSF2                              = 0xBB,
  PID_PSF1                              = 0xBA,
  PID_HOTHARM                           = 0xB9,
  PID_DCI                               = 0xB8,
  PID_DFXAGG                            = 0xB7,
  PID_NPK                               = 0xB6,
  //Rsv                                 = 0xB5-0xB1,
  PID_MMP0                              = 0xB0,
  PID_GPIOCOM0                          = 0xAF,
  PID_GPIOCOM1                          = 0xAE,
  PID_GPIOCOM2                          = 0xAD,
  PID_GPIOCOM3                          = 0xAC,
  PID_GPIOCOM4                          = 0xAB,
  PID_GPIOCOM5                          = 0x11,
  PID_MODPHY2                             = 0xA9,
  PID_MODPHY3                             = 0xA8,
  //Rsv                                 = 0xA7-0xA6,
  PID_PNCRC                             = 0xA5,
  PID_PNCRB                             = 0xA4,
  PID_PNCRA                             = 0xA3,
  PID_PNCR0                             = 0xA2,
  PID_CSME15                            = 0x9F, // SMS2
  PID_CSME14                            = 0x9E, // SMS1
  PID_CSME13                            = 0x9D, // PMT
  PID_CSME12                            = 0x9C, // PTIO
  PID_CSME11                            = 0x9B, // PECI
  PID_CSME9                             = 0x99, // SMT6
  PID_CSME8                             = 0x98, // SMT5
  PID_CSME7                             = 0x97, // SMT4
  PID_CSME6                             = 0x96, // SMT3
  PID_CSME5                             = 0x95, // SMT2
  PID_CSME4                             = 0x94, // SMT1 (SMBus Message Transport 1)
  PID_CSME3                             = 0x93, // FSC
  PID_CSME2                             = 0x92, // USB-R SAI
  PID_CSME0                             = 0x90, // CSE
  PID_CSME_PSF                          = 0x8F, // ME PSF
  //Rsv                                 = 0x88-0x30,
  //PID_EVA                             = 0x2F-0x00,
  PID_CSMERTC                           = 0x8E,
  PID_IEUART                            = 0x80,
  PID_IEHOTHAM                          = 0x7F,
  PID_IEPMT                             = 0x7E,
  PID_IESSTPECI                         = 0x7D,
  PID_IEFSC                             = 0x7C,
  PID_IESMT5                            = 0x7B,
  PID_IESMT4                            = 0x7A,
  PID_IESMT3                            = 0x79,
  PID_IESMT2                            = 0x78,
  PID_IESMT1                            = 0x77,
  PID_IESMT0                            = 0x76,
  PID_IEUSBR                            = 0x74,
  PID_IEPTIO                            = 0x73,
  PID_IEIOSFGASKET                      = 0x72,
  PID_IEPSF                             = 0x70,
  PID_FPK                               = 0x0A,
  PID_MP0KR                             = 0x3C,
  PID_MP1KR                             = 0x3E,
  PID_RUAUX                             = 0x0B,
  PID_RUMAIN                            = 0x3B,
  PID_EC                                = 0x20,
  PID_CPM2                              = 0x38,
  PID_CPM1                              = 0x37,
  PID_CPM0                              = 0x0C,
  PID_VSPTHERM                          = 0x25,
  PID_VSPP2SB                           = 0x24,
  PID_VSPFPK                            = 0x22,
  PID_VSPCPM2                           = 0x35,
  PID_VSPCPM1                           = 0x34,
  PID_VSPCPM0                           = 0x33,
  PID_MSMROM                            = 0x08,
  PID_PSTH                              = 0x89
} PCH_SBI_PID;

#endif
