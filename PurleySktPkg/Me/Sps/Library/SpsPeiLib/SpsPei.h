/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file SpsPeiLib.h

  Definitions for Sps Pei Library

**/
#ifndef _SPS_PEI_H
#define _SPS_PEI_H

#include <CoreBiosMsg.h>
#include <Library/SpsPeiLib.h>

// Index of the clock capabilities to "Get" or "Set"
#define REQUESTED_CLOCK_IsCLK_BCLK      1

#pragma pack(1)
typedef struct _ICC_SETGET_CLOCK_SETTINGS_REQ {
  ICC_HEADER      IccHeader;        // ICC Command
  UINT8           RequestedClock;   // Req: Specifies an index of the clock capabilities to “Get” or “Set”.
  UINT8           SettingType;      // Req: Selects the source of to retrieve the clock settings.
  UINT8           Reserved1;
  UINT8           ClockMode;        // Rsp: PCH Clocking Mode
  UINT32          Frequency;        // Rsp: Frequency of the clock in Hz
  UINT32          Reserved2;
  UINT32          MaxFrequency;     // Rsp: Specifies the maximum frequency supported by the clock
  UINT32          MinFrequency;     // Rsp: Specifies the minimum frequency supported by the clock
  UINT8           SSCMode;          // Rsp: Spread Spectrum Clocking Mode (Up, Down, Center, None)
  UINT8           SSCPercentage;    // Rsp: 1/100 percentage of the frequency range to spread
  UINT8           MaxSSCPercentage; // Rsp: 1/100 percentage of the frequency range to spread
  UINT8           SSCsetting;       // Req: Spread Spectrum settings for IsCLK
  UINT16          Reserved3;
  UINT16          Reserved4;
} ICC_SETGET_CLOCK_SETTINGS_REQ;

typedef struct _ICC_SETGET_CLOCK_SETTINGS_RSP {
  ICC_HEADER      IccHeader;        // ICC Command
  UINT8           RequestedClock;   // Req: Specifies an index of the clock capabilities to “Get” or “Set”.
  UINT8           SettingType;      // Req: Selects the source of to retrieve the clock settings.
  UINT8           Reserved1;
  UINT8           ClockMode;        // Rsp: PCH Clocking Mode
  UINT32          Frequency;        // Rsp: Frequency of the clock in Hz
  UINT32          Reserved2;
  UINT32          MaxFrequency;     // Rsp: Specifies the maximum frequency supported by the clock
  UINT32          MinFrequency;     // Rsp: Specifies the minimum frequency supported by the clock
  UINT8           SSCMode;          // Rsp: Spread Spectrum Clocking Mode (Up, Down, Center, None)
  UINT8           SSCPercentage;    // Rsp: 1/100 percentage of the frequency range to spread
  UINT8           MaxSSCPercentage; // Rsp: 1/100 percentage of the frequency range to spread
  UINT8           SSCsetting;       // Req: Spread Spectrum settings for IsCLK
  UINT16          Reserved3;
  UINT16          Reserved4;
} ICC_SETGET_CLOCK_SETTINGS_RSP;

typedef union {
  ICC_SETGET_CLOCK_SETTINGS_REQ  Message;
  ICC_SETGET_CLOCK_SETTINGS_RSP  Response;
} ICC_SETGET_CLOCK_SETTINGS_BUFFER;
#pragma pack()

#endif // _SPS_PEI_H
