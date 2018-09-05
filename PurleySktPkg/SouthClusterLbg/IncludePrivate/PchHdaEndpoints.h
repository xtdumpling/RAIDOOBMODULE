/** @file
  Header file for PchHdaLib Endpoint descriptors.

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
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

#ifndef _PCH_HDA_ENDPOINTS_H_
#define _PCH_HDA_ENDPOINTS_H_

#include <IncludePrivate/Library/DxePchHdaNhlt.h>

typedef enum {
  HdaDmicX1       = 0,
  HdaDmicX2,
  HdaDmicX4,
  HdaBtRender,
  HdaBtCapture,
  HdaI2sRender1,
  HdaI2sRender2,
  HdaI2sCapture,
  HdaEndpointMax
} NHLT_ENDPOINT;

typedef struct {
  NHLT_ENDPOINT EndpointType;
  UINT32        EndpointFormatsBitmask;
  BOOLEAN       Enable;
} PCH_HDA_NHLT_ENDPOINTS;

#define PCH_HDA_NHLT_TABLE_SIZE 0x2000

// Format bitmask
#define B_HDA_DMIC_2CH_48KHZ_16BIT_FORMAT             BIT0
#define B_HDA_DMIC_2CH_48KHZ_32BIT_FORMAT             BIT1
#define B_HDA_DMIC_4CH_48KHZ_16BIT_FORMAT             BIT2
#define B_HDA_DMIC_4CH_48KHZ_32BIT_FORMAT             BIT3
#define B_HDA_BT_NARROWBAND_FORMAT                    BIT4
#define B_HDA_BT_WIDEBAND_FORMAT                      BIT5
#define B_HDA_BT_A2DP_FORMAT                          BIT6
#define B_HDA_I2S_RTK_RENDER_4CH_48KHZ_24BIT_FORMAT   BIT7
#define B_HDA_I2S_RTK_CAPTURE_4CH_48KHZ_24BIT_FORMAT  BIT8
#define B_HDA_I2S_ADI_4CH_48KHZ_32BIT_FORMAT          BIT9
#define B_HDA_DMIC_1CH_48KHZ_16BIT_FORMAT             BIT10
#define V_HDA_FORMAT_MAX                              11

// Formats
extern CONST WAVEFORMATEXTENSIBLE Ch1_48kHz16bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch2_48kHz16bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch2_48kHz24bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch2_48kHz32bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch4_48kHz16bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch4_48kHz32bitFormat;
extern CONST WAVEFORMATEXTENSIBLE NarrowbandFormat;
extern CONST WAVEFORMATEXTENSIBLE WidebandFormat;
extern CONST WAVEFORMATEXTENSIBLE A2dpFormat;

// Format Config
extern CONST UINT32 DmicStereo16BitFormatConfig[];
extern CONST UINT32 DmicStereo16BitFormatConfigSize;
extern CONST UINT32 DmicStereo32BitFormatConfig[];
extern CONST UINT32 DmicStereo32BitFormatConfigSize;
extern CONST UINT32 DmicQuad16BitFormatConfig[];
extern CONST UINT32 DmicQuad16BitFormatConfigSize;
extern CONST UINT32 DmicQuad32BitFormatConfig[];
extern CONST UINT32 DmicQuad32BitFormatConfigSize;
extern CONST UINT32 DmicMono16BitFormatConfig[];
extern CONST UINT32 DmicMono16BitFormatConfigSize;
extern CONST UINT32 I2sRtkRender4ch48kHz24bitFormatConfig[];
extern CONST UINT32 I2sRtkRender4ch48kHz24bitFormatConfigSize;
extern CONST UINT32 I2sRtkCapture4ch48kHz24bitFormatConfig[];
extern CONST UINT32 I2sRtkCapture4ch48kHz24bitFormatConfigSize;
extern CONST UINT32 I2sAdi4ch48kHzFormatConfig[];
extern CONST UINT32 I2sAdi4ch48kHzFormatConfigSize;
extern CONST UINT32 BtFormatConfig[];
extern CONST UINT32 BtFormatConfigSize;

// Endpoints
extern ENDPOINT_DESCRIPTOR  HdaEndpointDmicX1;
extern ENDPOINT_DESCRIPTOR  HdaEndpointDmicX2;
extern ENDPOINT_DESCRIPTOR  HdaEndpointDmicX4;
extern ENDPOINT_DESCRIPTOR  HdaEndpointBtRender;
extern ENDPOINT_DESCRIPTOR  HdaEndpointBtCapture;
extern ENDPOINT_DESCRIPTOR  HdaEndpointI2sRender;
extern ENDPOINT_DESCRIPTOR  HdaEndpointI2sCapture;

// Endpoint Config
extern CONST UINT8  DmicX1Config[];
extern CONST UINT32 DmicX1ConfigSize;
extern CONST UINT8  DmicX2Config[];
extern CONST UINT32 DmicX2ConfigSize;
extern CONST UINT8  DmicX4Config[];
extern CONST UINT32 DmicX4ConfigSize;
extern CONST UINT8  BtConfig[];
extern CONST UINT32 BtConfigSize;
extern CONST UINT8  I2sRender1Config[];
extern CONST UINT32 I2sRender1ConfigSize;
extern CONST UINT8  I2sRender2Config[];
extern CONST UINT32 I2sRender2ConfigSize;
extern CONST UINT8  I2sCaptureConfig[];
extern CONST UINT32 I2sCaptureConfigSize;

// Oed Configuration
extern CONST UINT32 NhltConfiguration[];
extern CONST UINT32 NhltConfigurationSize;

#endif // _PCH_HDA_ENDPOINTS_H_
