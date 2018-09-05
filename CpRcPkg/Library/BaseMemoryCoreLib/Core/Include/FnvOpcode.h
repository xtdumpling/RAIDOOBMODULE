/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file FnvOpcode.h
**/


#ifndef _fnvopcode_h
#define _fnvopcode_h

/*
*Updated 8/19/2014*
*NVMCTLR CIS 0.82*

*/
#define FNV_OPCODE_ARRAY_SIZE    62
#define NULL_COMMAND             0x0000
#define IDENTIFY_DIMM            0x0001
#define DEVICE_CHARACTERISTICS   0x0101
#define GET_SECURITY_STATE       0x0002
#define SET_SECURITY_NONCE       0x0003
#define OVERWRITE_DIMM           0x0103
#define SET_PASSPHRASE           0xF103
#define DISABLE_PASSPHRASE       0xF203
#define UNLOCK_UNIT              0xF303
#define SECURE_ERASE_UNIT        0xF503
#define FREEZE_LOCK              0xF603
#define GET_ALARM_THRESHOLDS     0x0104
#define GET_POWER_MANAGEMENT     0x0204
#define GET_DIE_SPARING          0x0304
#define GET_ADDRESS_RANGE_SCRUB  0x0404
#define GET_DDRT_ALERTS          0x0504
#define GET_CONFIG_DATA_POLICY   0x0604
#define GET_PMON_REGISTERS       0x0704
#define RESET_ALL_THRESHOLDS     0x0005
#define SET_ALARM_THRESHOLDS     0x0105
#define SET_POWER_MANAGEMENT     0x0205
#define SET_DIE_SPARING          0x0305
#define SET_ADDRESS_RANGE_SCRUB  0x0405
#define SET_DDRT_ALERTS          0x0505
#define SET_CONFIG_DATA_POLICY   0x0605
#define SET_PMON_REGISTERS       0x0705
#define GET_SYSTEM_TIME          0x0006
#define GET_PLATFORM_CONFIG      0x0106
#define GET_DIMM_PARTITION       0x0206
#define GET_FNV_FW_DEBUG_LOG     0x0306
#define GET_CONFIG_LOCKDOWN      0x0506
#define Get_DDRT_INIT_INFO       0x0606
#define GET_SUPPORTED_SKU        0x0706
#define GET_ENABLE_DIMM          0x0806
#define SET_SYSTEM_TIME          0x0007
#define SET_PLATFORM_CONFIG      0x0107
#define SET_DIMM_PARTITION       0x0207
#define SET_FNV_FW_DEBUG_LOG     0x0307
#define SET_CONFIG_LOCKDOWN      0x0507
#define DDRT_IO_INIT             0x0607
#define SET_CREATE_SKU           0x0707
#define SET_ENABLE_DIMM          0x0807
#define GET_SMART_HEALTH         0x0008
#define GET_FIRMWARE_IMAGE       0x0108
#define GET_FIRMWARE_DEBUG       0x0208
#define GET_MEMORY_INFO          0x0308
#define GET_LONG_OPERATION       0x0408
#define GET_ERROR_LOG            0x0508
#define GET_COMMAND_EFFECT_LOG   0xFF08
#define UPDATE_FNV_FW            0x0009
#define EXECUTE_FNV_FW           0x0109
#define ENABLE_ERR_INJECTION     0x000A
#define INJECT_POISON_ERROR      0x010A
#define INJECT_TEMP_ERROR        0x020A
#define INJECT_SW_TRIGGER        0x030A

#endif // _fnvopcode_h
