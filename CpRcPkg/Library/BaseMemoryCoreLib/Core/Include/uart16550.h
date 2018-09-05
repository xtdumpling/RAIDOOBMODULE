//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
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
 ************************************************************************
 *
 *  PURPOSE:
 *
 *      This file contains serial port usage headers
 *
 ************************************************************************/

#ifndef _UART16550_H
#define _UART16550_H

#define RECEIVER_BUFFER           0x00
#define TRANSMIT_HOLDING          0x00
#define DIVISOR_LATCH_LOW         0x00  // 0/1 - 16-bit
#define DIVISOR_LATCH_HIGH        0x01
#define INTERRUPT_ENABLE          0x01
#define DISABLE_INTERRUPTS        0x00

#define INTERRUPT_IDENTIFICATION  0x02
#define FIFO_CONTROL              0x02
#define FIFO_DISABLE_MASK         0xfe

#define LINE_CONTROL              0x03
#define DIVISOR_LATCH_ACCESS      0x80
#define NO_STOP                   0x00
#define NO_PARITY                 0x00
#define BITS8                     0x03

#define MODEM_CONTROL             0x04
#define LINE_STATUS               0x05
#define TRANS_HOLDING_REG_EMPTY   0x20
#define DATA_READY                0x01

#define MODEM_STATUS              0x06
#define SCRATCH                   0x07

#endif
