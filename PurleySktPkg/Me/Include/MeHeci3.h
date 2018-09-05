/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2015 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  MeHeci.h

Abstract:

  Definitions for HECI interface to ME.
  TODO: Currently this file is used by HECI-3 SMM driver only.
  TODO: The plan is to make it common core for all HECI functions.

--*/
#ifndef _ME_HECI3_H_
#define _ME_HECI3_H_

/**
  * ME device location on PCI
  *
  * ME has one PCI device in PCH bus 0 with multiple functions.
  */
#define ME_BUS   0
#define ME_DEV   22

/**
 * HECI functions in ME device
 */
#define ME_FUN_HECI1  0
#define ME_FUN_HECI2  1
#define ME_FUN_HECI3  4


/**
 * HECI PCI config space registers list.
 *
 * Note that HECI Device Id varies between chipset generations and also between
 * client and server chipset SKU.
 * Note that H_GSx registers are host status registers writable for host,
 * read only for ME, and HSF + GS_SHDWx registers are ME status writeable
 * for ME, read only for host side. HFS in HECI-1 is the well known ME Firmware
 * Status 1 register.
 * The definition of the status registers functionality depends on the type of
 * firmwere running in ME.
 *
 * TODO: Exclude these generic register definitions to separate file shared with IE.
 */
#define HECI_R_VID       0x00
#define HECI_R_DID       0x02
#define HECI_R_CMD       0x04
#define HECI_R_REVID     0x08
#define HECI_R_MBAR      0x10
#define HECI_R_IRQ       0x3C
#define HECI_R_HIDM      0xA0
#define HECI_R_HFS       0x40
#define HECI_R_MISC_SHDW 0x44
#define HECI_R_GS_SHDW   0x48
#define HECI_R_H_GS      0x4C
#define HECI_R_GS_SHDW2  0x60
#define HECI_R_GS_SHDW3  0x64
#define HECI_R_GS_SHDW4  0x68
#define HECI_R_GS_SHDW5  0x6C
#define HECI_R_H_GS2     0x70
#define HECI_R_H_GS3     0x74
#define HECI_R_MEFS1     HECI_R_HFS     // HFS in HECI-1 is ME Firmware Status 1
#define HECI_R_MEFS2     HECI_R_GS_SHDW // GS_SHDW in HECI-1 is ME Firmware Status 2


/**
 * HECI command (HECI_R_CMD) register bits.
 */
#define HECI_CMD_BME     0x04  // Bus Master Enable
#define HECI_CMD_MSE     0x02  // Memory Space Enable


/**
 * HECI Interrupt Delivery Mode (HECI_R_HIDM) register values.
 */
#define HECI_HIDM_MSI 0
#define HECI_HIDM_SCI 1
#define HECI_HIDM_SMI 2
#define HECI_HIDM_LAST HECI_HIDM_SMI
#define HECI_HIDM_LOCK 4

#define MeHeciPciReadMefs1()                                                  \
        MmioRead32(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI1) + HECI_R_MEFS1)
#define MeHeciPciReadMefs2()                                                  \
        MmioRead32(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI1) + HECI_R_MEFS2)

#define MeHeci1MmioRead32(Offset)                                              \
        MmioRead32(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI1) + (Offset))
#define MeHeci1MmioRead16(Offset)                                              \
        MmioRead16(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI1) + (Offset))
#define MeHeci1MmioRead8(Offset)                                               \
        MmioRead8(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI1) + (Offset))
#define MeHeci1MmioWrite32(Offset, Value)                                      \
        MmioWrite32(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI1) + (Offset), (Value))
#define MeHeci1MmioWrite16(Offset, Value)                                      \
        MmioWrite16(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI1) + (Offset), (Value))
#define MeHeci1MmioWrite8(Offset, Value)                                       \
        MmioWrite8(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI1) + (Offset), (Value))
#define MeHeci2PciRead32(Offset)                                              \
        MmioRead32(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI2) + (Offset))
#define MeHeci2PciRead16(Offset)                                              \
        MmioRead16(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI2) + (Offset))
#define MeHeci2PciRead8(Offset)                                               \
        MmioRead8(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI2) + (Offset))
#define MeHeci2PciWrite32(Offset, Value)                                      \
        MmioWrite32(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI2) + (Offset), (Value))
#define MeHeci2PciWrite16(Offset, Value)                                      \
        MmioWrite16(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI2) + (Offset), (Value))
#define MeHeci2PciWrite8(Offset, Value)                                       \
        MmioWrite8(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI2) + (Offset), (Value))
#define MeHeci3MmioRead32(Offset)                                              \
        MmioRead32(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI3) + (Offset))
#define MeHeci3MmioRead16(Offset)                                              \
        MmioRead16(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI3) + (Offset))
#define MeHeci3MmioRead8(Offset)                                               \
        MmioRead8(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI3) + (Offset))
#define MeHeci3MmioWrite32(Offset, Value)                                      \
        MmioWrite32(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI3) + (Offset), (Value))
#define MeHeci3MmioWrite16(Offset, Value)                                      \
        MmioWrite16(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI3) + (Offset), (Value))
#define MeHeci3MmioWrite8(Offset, Value)                                       \
        MmioWrite8(MmPciBase(ME_BUS, ME_DEV, ME_FUN_HECI3) + (Offset), (Value))

/**
 * Default values to be used in ME HECI_R_MBAR before PCI enumeration.
 */
#define ME_HECI1_MBAR_DEFAULT 0xFEDB0000
#define ME_HECI2_MBAR_DEFAULT 0xFEDB1000
#define ME_HECI3_MBAR_DEFAULT 0xFEDB2000


/**
 * ME Firmware Status 1 register basics.
 *
 * ME Firmware Status 1 register is in HECI-1 configuration space at offset 40h.
 * Full definition of the register depends on type of the firmware running in ME.
 * The HECI_MEFS1 structure defines only common, basic part.
 */
typedef union
{
  UINT32   DWord;
  struct
  {
    UINT32 CurrentState : 4,  //< 0:3   Current ME firmware state
           Reserved0    : 5,  //  4:8
           InitComplete : 1,  //< 9     ME firmware finished initialization
           Reserved1    : 2,  // 10:11
           ErrorCode    : 4,  //< 12:15 If set means fatal error
           OperatingMode: 4,  //< 16:19 Current ME operating mode
           Reserved2    :12;  //< 20:31
  } Bits;
} HECI_MEFS1;

/**
 * HECI_MEFS1::CurrentState values list.
 *
 * This field describes the current state of the firmware.
 */
#define MEFS1_CURSTATE_RESET        0  //< ME is in reset, will exit this state within 1 ms
#define MEFS1_CURSTATE_INIT         1  //< ME is initializing, will exit this state within 2 s
#define MEFS1_CURSTATE_RECOVERY     2  //< ME is in recovery mode
#define MEFS1_CURSTATE_DISABLED     4  //< ME functionality has been disabled
#define MEFS1_CURSTATE_NORMAL       5  //< ME is in normal operational state
#define MEFS1_CURSTATE_TRANSITION   7  //< ME is transitioning to a new operating State

/**
 * HECI_MEFS1::ErrorCode values list.
 *
 * If HECI_MEFS.::ErrorCode is set to non zero value the ME firmware has
 * encountered a fatal error and stopped normal operation.
 */
#define MEFS1_ERRCODE_NOERROR       0  //< ME firmware does not report errors
#define MEFS1_ERRCODE_UNKNOWN       1  //< Unkategorized error occured
#define MEFS1_ERRCODE_DISABLED      2  //< ME firmware disabled (debug)
#define MEFS1_ERRCODE_IMAGEFAIL     3  //< No valid firmware image in ME region

/**
 * HECI_MEFS1::OperatingMode values list.
 *
 * This field describes the current operating mode of ME.
 */
#define MEFS1_OPMODE_NORMAL         0  // Client firmware is running in ME
#define MEFS1_OPMODE_IGNITION       1  // Ignition firmware is running in ME
#define MEFS1_OPMODE_DEBUG          2  // 
#define MEFS1_OPMODE_TEMP_DISABLE   3  // 
#define MEFS1_OPMODE_SECOVR_JMPR    4  // Security Override activated with jumper
#define MEFS1_OPMODE_SECOVR_MSG     5  // Security Override activated with HMRFPO_ENABLE request
#define MEFS1_OPMODE_SPS           15  // SPS firmware is running in ME


/*
 * MISC_SHDW register in HECI-1 config space at offset 44h
 *
 * If MSVLD bit is not set the register is not implemented.
 * Only in HECI-1 this register is implemented.
 */
typedef union {
  UINT32   DWord;
  struct {
    UINT32 MUSZ     : 6,  // 0:5 - ME UMA Size
           Reserved0:10,  // 6:15 - Reserved
           MUSZV    : 1,  // 16:16 - ME UMA Size Valid
           Reserved1:14,  // 17:30 - Reserved
           MSVLD    : 1;  // 31:31 - Miscellaneous Shadow Valid
  } Bits;
} HECI_MISC_SHDW;

/**
 * HECI message header.
 *
 * HECI massage header is one double word long. It identifies communication
 * entities (subsystems) on ME and host side and provides the exact number
 * of bytes in the message body following this header.
 */
typedef union
{
  UINT32   DWord;
  struct
  {
    UINT32 MeAddress  : 8,  ///< Addressee on ME side
           HostAddress: 8,  ///< Addressee on host siede, zero for BIOS
           Length     : 9,  ///< Number of bytes following the header
           Reserved   : 6,
           MsgComplete: 1;  ///< Whether this is last fragment of a message
  } Bits;
} HECI_MSG_HEADER;

/**
 * Macro for building HECI message header dword using given ingredients.
 */
#define MeHeciMsgHeader(MeAdr, HstAdr, Len, Cpl) \
(UINT32)(((Cpl) << 31) | ((UINT32)(Len) & 0x1FF) << 16 | ((HstAdr) & 0xFF) << 8 | ((MeAdr) & 0xFF))

/**
 * Maximum length of HECI message.
 *
 * Actually it is maximum length of HECI queue that can be configured by ME.
 * Single message must not exceed HECI queue size. Current HECI queue size is
 * configured by ME, so this macro does not define current maximum message
 * length. It defines maximum length in case ME configures maximum queue size.
 */
#define HECI_MSG_MAXLEN (0x80 * sizeof(UINT32))

#endif // _ME_HECI3_H_

