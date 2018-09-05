/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2014 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/

#ifndef _USB_DEBUG_PORT_CONFIG_H
#define _USB_DEBUG_PORT_CONFIG_H

#include "PlatformHost.h"

//
// Platform dependent information for USB Debug
//
// Bit-mask of Information to disable USB Debug
#define	EMULATION_FLAG_TO_DISABLE_USB_DEBUG	SIMICS_FLAG

// PCH BIOS Spec Update Rev 0.71 Sighting #3305753 : High Speed USB Devices May Not Be Detected
// with RMH Enabled.
// System BIOS is required to follow steps below in early chipset initialization before RMH is
// enabled to reduce the failure rate.
// 1. D26/D29:F7:F4[31] = 1b
// 2. D26/D29:F7:F4[19:16] = 1111b
// 3. D26/D29:F7:F4[31] = 0b
#define	USB_DEBUG_WA_BUS			0
#define	USB_DEBUG_WA_DEVICE			26
#define	USB_DEBUG_WA_FUNCTION			7
#define	USB_DEBUG_WA_OFFSET			0xF4

// APTIOV_SERVER_OVERRIDE_RC_START : To avoid redefinition error
#ifndef CAR_BASE_ADDRESS
#define CAR_BASE_ADDRESS			0xfe080000
#endif
#ifndef SB_RCBA
#define	SB_RCBA					0x0FED1C000
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : To avoid redefinition error
#define PWR_MGT_CAP_ID				0x01
#define DBG_PRT_CAP_ID				0x0A
#define EP_IN					0x82
#define EP_OUT					0x01

//
#endif /* _USB_DEBUG_PORT_CONFIG_H */
