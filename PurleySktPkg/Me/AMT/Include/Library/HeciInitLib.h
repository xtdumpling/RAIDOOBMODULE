/** @file
  Definitions for HECI driver

@copyright
 Copyright (c) 2006 - 2016 Intel Corporation. All rights reserved
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
#ifndef _HECI_INIT_LIB_H
#define _HECI_INIT_LIB_H


#include <CoreBiosMsg.h>
#include <HeciRegs.h>


//
// HECI bus function version
//
#define HBM_MINOR_VERSION 0
#define HBM_MAJOR_VERSION 1

/**
  Abstract ME BIOS Boot Path definitions.
  BIOS Boot Path vs. MEI1 Host Firmware Status 1, 4, and 5 registers:
  MeNormalBiosPath -
    If (HECI_FWS_REGISTER.r.CurrentState == ME_STATE_NORMAL AND
      HECI_FWS_REGISTER.r.ErrorCode == ME_ERROR_CODE_NO_ERROR)
    - BIOS takes the normal firmware BIOS path.

  MeFwUpdateBiosPath
    If (HECI_FWS_REGISTER.r.CurrentState == ME_STATE_NORMAL AND
      HECI_FWS_REGISTER.r.MeOperationMode == ME_OPERATION_MODE_NORMAL AND
      HECI_FWS_REGISTER.r.FwUpdateInprogress == 0x01 )
    - BIOS starts a 90-seconds loop to wait for the firmware to clear the bit to 0

  MeErrorWithoutDidMsgBiosPath -
    If HECI_FWS_REGISTER.r.ErrorCode == ME_ERROR_CODE_IMAGE_FAILURE
    - The BIOS does not send any Intel MEI messages including DID and EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - MEI 1 device should be enabled by the BIOS so it allows the user to update the new firmware and
      take the firmware out of a recovery or error condition. Hide MEI 2, MEI3, SOL and IDER. In addition,
      Image Failure results in a platform involuntary 30-minute shut down triggered by Intel ME.
      BIOS shall post the warning message as part of the error handling flow.

  MeErrorBiosPath -
    If (HECI_FWS_REGISTER.r.FptBad == 0x01) OR
      (HECI_FWS_REGISTER.r.ErrorCode != ME_ERROR_CODE_NO_ERROR) OR
      (HECI_FWS_REGISTER.r.ErrorCode != ME_ERROR_CODE_IMAGE_FAILURE)
    - The BIOS does not send any Intel MEI messages except for the DRAM Init Done message. Moreover,
      the BIOS doesn't even send EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - MEI 1 device should be enabled by the BIOS so it allows the user to update the new firmware and
      take the firmware out of a recovery or error condition. Hide MEI 2, SOL and IDER.

  MeRecoveryBiosPath -
    If HECI_FWS_REGISTER.r.CurrentState == ME_STATE_RECOVERY
    - The BIOS does not send any Intel MEI messages except for the DRAM Init Done message. Moreover,
      the BIOS doesn't even send EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - MEI 1 device should be enabled by the BIOS so it allows the user to update the new firmware and
      take the firmware out of a recovery or error condition. Hide MEI 2, SOL and IDER.

  MeDebugModeBiosPath -
    If HECI_FWS_REGISTER.r.MeOperationMode == ME_OPERATION_MODE_DEBUG
    - The BIOS does not send any Intel MEI messages including DID and EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - Hide MEI1, MEI2, MEI3, SOL and IDER before OS boot. It means there is no MEI drivers loaded in OS environment.

  MeSwTempDisableBiosPath
    If (HECI_FWS_REGISTER.r.MeOperationMode == ME_OPERATION_MODE_SOFT_TEMP_DISABLE)
    - The BIOS does not send any Intel MEI messages except for the DRAM Init Done message, Set Me Enable message
      and Global Reset Message.
      Moreover, the BIOS doesn't even send EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - Hide MEI2, MEI3, SOL and IDER before OS boot. It means there is no MEI drivers loaded in OS environment.
      Hides MEI1 device after sending the Set Me Enable message or prior to boot.
      It means there is no MEI drivers loaded in OS environment.

  MeSecoverJmprBiosPath
    If (HECI_FWS_REGISTER.r.MeOperationMode == ME_OPERATION_MODE_SECOVR_JMPR)
    - The BIOS does not send any Intel MEI messages except for the DRAM Init Done message. Moreover,
      the BIOS doesn't even send EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - Hide MEI1, MEI2, MEI3, SOL and IDER before OS boot. It means there is no MEI drivers loaded in OS environment.

  MeSecoverMeiMsgBiosPath
    If HECI_FWS_REGISTER.r.MeOperationMode == ME_OPERATION_MODE_SECOVR_HECI_MSG
    - The BIOS does not send any Intel MEI messages except for the DRAM Init Done (DID) message and HMRFPO DISABLE message.
      The HMRFPO DISABLE message is to bring the firmware out of SECOVR_MEI_MSG operation mode back to normal.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - Hide MEI2, MEI3, SOL and IDER before OS boot.
      Hides MEI1 device after sending the HMRFPO DISABLE message.
      It means there is no MEI drivers loaded in OS environment.

  MeEnforcementWithoutDidMsgBiosPath
    If HECI_FW_STS4_REGISTER.r.FwInEnfFlow == 1
    - The BIOS does not send any Intel MEI messages including DID and EOP message.
    - The BIOS does not invoke the Intel MEBX and should hide Intel MEBX hotkey entry.
    - Hide MEI1, MEI2, MEI3, SOL and IDER before OS boot.
**/

/**
  The FW registers may report multiple status to reflect Me Bios boot path, BIOS will follow the
  prioritized Me Bios boot path to continue boot. If the priority will be changed, then
  BOOT_PATH enumeration shall be adjusted as well to reflect real priority.
**/
typedef enum {
  NormalPath = 0,
  FwUpdatePath,
  ErrorPath,
  RecoveryPath,
  ErrorWithoutDidMsgPath,
  SecoverMeiMsgPath,
  SwTempDisablePath,
  SecoverJmprPath,
  DebugModePath,
  EnforcementWithoutDidMsgPath,
  SpsPath,
  MaxPathValue
} BOOT_PATH;

/**
  Abstract ME devices hidden in BIOS Path definition
  Bit16 - MEI1
  Bit17 - MEI2
  Bit18 - MEI3
  Bit19 - SOL
**/
#define HIDE_MEI1                BIT16
#define HIDE_MEI2                BIT17
#define HIDE_MEI3                BIT18
#define HIDE_SOL                 BIT19
#define HIDE_AMT_DEVICE          HIDE_SOL
#define HIDE_SPS_DEVICE          (HIDE_MEI3 | HIDE_AMT_DEVICE)
#define HIDE_ALL_ME_DEVICE       (HIDE_MEI1 | HIDE_MEI2 | HIDE_MEI3 | HIDE_AMT_DEVICE)
#define DEVICE_HIDE_MASK         0x00FF0000

/**
  Abstract MEI message allowance in non MeNormalBiosPath definition
  Bit0 - DID message is allowed to be sent with this BIOS path
  Bit1 - EOP message is allowed to be sent with this BIOS path
  Bit2 - HMRFPO DISABLE message is allowed to be sent with this BIOS path
  Bit3 - SET ME ENABLE message is allowed to be sent with this BIOS path
  Bit4 - GLOBAL RESET message is allowed to be sent with this BIOS path
**/
#define DID_MSG_ALLOWANCE             BIT0
#define EOP_MSG_ALLOWANCE             BIT1
#define HMRFPO_DISABLE_MSG_ALLOWANCE  BIT2
#define SET_ME_ENABLE_MSG_ALLOWANCE   BIT3
#define GLOBAL_RST_MSG_ALLOWANCE      BIT4
#define HMRFPO_GRP_MSG_ALLOWANCE      BIT5  // MKHI_SPI_GROUP_ID messages
#define GEN_GRP_MSG_ALLOWANCE         BIT6  // MKHI_GEN_GROUP_ID messages
#define ALL_MSG_ALLOWANCE             BIT15
#define MSG_ALLOWANCE_MASK            0x0000FFFF

/**
  Bit 15:0 - MEI message allowance definitions
  Bit 23:16 - ME device hidden control
  Bit 31:24 - BIOS Path value definitions
**/
typedef enum {
  MeNormalBiosPath = ((NormalPath << 24) + ALL_MSG_ALLOWANCE),
  MeFwUpdateBiosPath = ((FwUpdatePath << 24) + ALL_MSG_ALLOWANCE),
  MeErrorBiosPath = ((ErrorPath << 24) + HIDE_MEI2 + HIDE_AMT_DEVICE + DID_MSG_ALLOWANCE),
  MeRecoveryBiosPath = ((RecoveryPath << 24) + HIDE_MEI2 + HIDE_AMT_DEVICE + DID_MSG_ALLOWANCE),
  MeErrorWithoutDidMsgBiosPath = ((ErrorWithoutDidMsgPath << 24) + HIDE_MEI2 + HIDE_MEI3 + HIDE_AMT_DEVICE),
  MeSecoverMeiMsgBiosPath = ((SecoverMeiMsgPath << 24) + HIDE_MEI2 + HIDE_MEI3 + HIDE_AMT_DEVICE + DID_MSG_ALLOWANCE + HMRFPO_DISABLE_MSG_ALLOWANCE),
  MeSwTempDisableBiosPath = ((SwTempDisablePath << 24) + HIDE_MEI2 + HIDE_MEI3 + HIDE_AMT_DEVICE + DID_MSG_ALLOWANCE + SET_ME_ENABLE_MSG_ALLOWANCE + GLOBAL_RST_MSG_ALLOWANCE),
  MeSecoverJmprBiosPath = ((SecoverJmprPath << 24) + HIDE_ALL_ME_DEVICE + DID_MSG_ALLOWANCE),
  MeDebugModeBiosPath = ((DebugModePath << 24) + HIDE_ALL_ME_DEVICE),
  MeEnforcementWithoutDidMsgBiosPath = ((EnforcementWithoutDidMsgPath << 24) + HIDE_ALL_ME_DEVICE),
  MeSpsOprBiosPath = ((SpsPath << 24) + ALL_MSG_ALLOWANCE + HIDE_SPS_DEVICE),
  MeSpsRcvBiosPath = ((SpsPath << 24) + DID_MSG_ALLOWANCE + HMRFPO_GRP_MSG_ALLOWANCE + GEN_GRP_MSG_ALLOWANCE + HIDE_SPS_DEVICE)
} ME_BIOS_BOOT_PATH;

//
// Prototypes
//
/**
  Check ME Boot path. The function provides ME BIOS boot path required based on current
  HECI1 FW Status Register (MEI1 HFSTS1 used in BWG, R_FWSTATE used in code definition).
  HECI1 must be enabled before invoke the function.

  @param[out] MeBiosPath          Pointer for ME BIOS boot path report

  @retval EFI_SUCCESS             MeBiosPath copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeBiosPath is invalid
**/
EFI_STATUS
CheckMeBootPath (
  OUT ME_BIOS_BOOT_PATH           *MeBiosPath
  );

#endif // _HECI_INIT_LIB_H

