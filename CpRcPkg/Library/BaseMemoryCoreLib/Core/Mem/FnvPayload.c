/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2009-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file FnvMain.c

  FalconValley payload table

**/
//
// Include files
//
#include "SysHost.h"
#include "FnvOpcode.h"

/*
*Updated 5/27/2016*
*NVMCTLR FIS 1.20*
*/

UINT32 FnvPayloadLookupTable [FNV_OPCODE_ARRAY_SIZE][5] = {

   //  Opcode            Input Payload Registers     Output Payload Registers       Implemented Command     R/W                       Description
   {NULL_COMMAND,             0,                            0,                         0,                     0},                      // 0. null command
   {IDENTIFY_DIMM,            0,                           17,                         1,                     0},                      // 1. Identify Dimm
   {DEVICE_CHARACTERISTICS,   0,                            2,                         1,                     0},                      // 2. Device Characteristics                                                                                                                                        
   {GET_SECURITY_STATE,       0,                            1,                         1,                     0},                      // 3. Get Security state
   {SET_SECURITY_NONCE,       2,                            0,                         1,                     0},                      // 4. Set SMM Nonce
   {OVERWRITE_DIMM,           0,                            0,                         1,                     0},                      // 5. Overwrite DIMM                                                                                                                                       
   {SET_PASSPHRASE,          32,                            0,                         0,                     1},                      // 6. Set Passphrase
   {DISABLE_PASSPHRASE,      32,                            0,                         0,                     1},                      // 7. Disable Passphrase
   {UNLOCK_UNIT,             32,                            0,                         0,                     1},                      // 8. Unlock Unit
   {SECURE_ERASE_UNIT,       32,                            0,                         1,                     1},                      // 9. Secure Erase Prepare *Not Listed*
   {FREEZE_LOCK,              1,                            0,                         0,                     1},                      // 10. Freeze Lock *Not Listed*
   {GET_ALARM_THRESHOLDS,     2,                            0,                         0,                     0},                      // 11. Get Alarm Thresholds
   {GET_POWER_MANAGEMENT,     0,                            2,                         1,                     0},                      // 12. Get Power Management Policy *Not Listed*
   {GET_DIE_SPARING,          1,                            0,                         0,                     0},                      // 13. Get Die Sparring Policy *Not Listed*
   {GET_ADDRESS_RANGE_SCRUB,  1,                            0,                         0,                     0},                      // 14. Get Address Range Srcub Policy *Not Listed*
   {GET_DDRT_ALERTS,          0,                           32,                         1,                     0},                      // 15. Get NVMDIMM Alerts
   {GET_CONFIG_DATA_POLICY,   1,                            0,                         0,                     0},                      // 16. Get Configuration Data Policy
   {GET_PMON_REGISTERS,       1,                           32,                         0,                     0},                      // 17. Get PMON registers                                                                                                                                       
   {RESET_ALL_THRESHOLDS,     2,                            0,                         0,                     1},                      // 18. Set Reset to Defaults
   {SET_ALARM_THRESHOLDS,     1,                            0,                         0,                     1},                      // 19. Set Alarm Thresholds
   {SET_POWER_MANAGEMENT,     2,                            0,                         1,                     1},                      // 20. Set Power Management Policy *Not Listed*
   {SET_DIE_SPARING,          1,                            0,                         0,                     1},                      // 21. Set Die Sparring Policy
   {SET_ADDRESS_RANGE_SCRUB,  1,                            0,                         0,                     1},                      // 22. Set Address Range Scrub Policy *Not Listed*
   {SET_DDRT_ALERTS,         32,                            0,                         1,                     1},                      // 23. Set NVMDIMM Alerts
   {SET_CONFIG_DATA_POLICY,   1,                            0,                         0,                     0},                      // 24. Set Configuration Data Policy
   {SET_PMON_REGISTERS,      32,                            0,                         0,                     0},                      // 25. Set PMON registers
   {GET_SYSTEM_TIME,          2,                            0,                         1,                     0},                      // 26. Get Admin System Time
   {GET_PLATFORM_CONFIG,      2,                           32,                         1,                     0},                      // 27. Get Admin Platform Config Data
   {GET_DIMM_PARTITION,       0,                            9,                         1,                     0},                      // 28. Get Admin Dimm Partition Info
   {GET_FNV_FW_DEBUG_LOG,     1,                            0,                         0,                     0},                      // 29. Get Admin NVMCTLR FW Debug Log Level
   {(0x06) | (0x04 << 8),     0,                            0,                         0,                     0},                      // 30. Reserved
   {GET_CONFIG_LOCKDOWN,      1,                            0,                         0,                     0},                      // 31. Get Admin DPA Lockdown
   {Get_DDRT_INIT_INFO,       1,                            0,                         0,                     0},                      // 32. Get Admin NVMDIMM IO Init Info
   {GET_SUPPORTED_SKU,        0,                            1,                         0,                     0},                      // 33. Get Supported SKU Features
   {GET_ENABLE_DIMM,          0,                            1,                         0,                     0},                      // 34. Get Enable DIMM                                                                                                                                        
   {SET_SYSTEM_TIME,          2,                            0,                         1,                     1},                      // 35. Set Admin System Time
   {SET_PLATFORM_CONFIG,     32,                            0,                         1,                     1},                      // 36. Set Admin Platform Config Data
   {SET_DIMM_PARTITION,       2,                            0,                         1,                     1},                      // 37. Set Admin Dimm Partition Info
   {SET_FNV_FW_DEBUG_LOG,     1,                            0,                         0,                     1},                      // 38. Set Admin NVMCTLR FW Debug Log Level
   {(0x07) | (0x04 << 8),     0,                            0,                         0,                     0},                      // 39. Reserved
   {SET_CONFIG_LOCKDOWN,      1,                            0,                         0,                     1},                      // 40. Set Admin DPA Lockdown
   {DDRT_IO_INIT,             1,                            0,                         0,                     1},                      // 41. Set Admin NVMCTLR NVMDIMM IO init
   {SET_CREATE_SKU,           1,                            0,                         0,                     1},                      // 42. Set Create SKU
   {SET_ENABLE_DIMM,          0,                            0,                         0,                     1},                      // 43. Set Enable DIMM                                                                                                                                       // 
   {GET_SMART_HEALTH,         1,                            0,                         0,                     0},                      // 44. Get Log Page Smart & Health Info
   {GET_FIRMWARE_IMAGE,       0,                           32,                         0,                     0},                      // 45. Get Log Page Firmware Image Information
   {GET_FIRMWARE_DEBUG,       1,                            0,                         0,                     0},                      // 46. Get Log Page Firmware Debug Log
   {GET_MEMORY_INFO,          1,                            0,                         0,                     0},                      // 47. Get Log Page Memory Test Log
   {GET_LONG_OPERATION,       1,                            0,                         0,                     0},                      // 48. Get Log Page Long Operation Status
   {GET_ERROR_LOG,            32,                          32,                         1,                     0},                      // 49. Get Log Page Error Log
   {GET_COMMAND_EFFECT_LOG,   0,                            1,                         0,                     0},                      // 50. Get Command Effect Log
   {UPDATE_FNV_FW,            1,                            0,                         0,                     1},                      // 51. Update Firmware Update NVMCTLR FW
   {EXECUTE_FNV_FW,           1,                            0,                         0,                     1},                      // 52. Update Firmware Execute NVMCTLR FW
   {ENABLE_ERR_INJECTION,     1,                            0,                         0,                     1},                      // 53. Inject Error Enable Injection
   {INJECT_POISON_ERROR,      1,                            0,                         0,                     1},                      // 54. Inject Error Poison Error
   {INJECT_TEMP_ERROR,        1,                            0,                         0,                     1},                      // 55. Inject Error Temperature Error
   {INJECT_SW_TRIGGER,        1,                            0,                         0,                     1},                      // 56. Inject Software Triggers   
};

