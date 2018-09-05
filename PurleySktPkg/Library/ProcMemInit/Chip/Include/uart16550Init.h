//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/*************************************************************************
 *
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
 * disclosed in any way without Intel’s prior express written permission.
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
 *      This file contains Serial port initialization headers
 *
 ************************************************************************/

#ifndef _UART16550INIT_H
#define _UART16550INIT_H
//
// void Init16550Uart (U16 ComPort);
//
#define MRC_COM1_BASE             0x3f8
#define MRC_COM2_BASE             0x2f8
#define MRC_COM3_BASE             0x3e8
#define MRC_COM4_BASE             0x2e8

#define BMC_INDEX_PORT0           0x04E
#define BMC_DATA_PORT0            BMC_INDEX_PORT0 + 1
#define BMC_LDN_UART0           0x2
#define BMC_LDN_UART1           0x1
#define BMC_ACTIVATE_CFG_MODE     0x5A

#define WPC_INDEX_PORT0           0x2E
#define WPC_DATA_PORT0            WPC_INDEX_PORT0 + 1
#define WPC_LDN_UART0           0x3
#define WPC_LDN_UART1           0x2
//
// Global register in NSPC87427
//
#define REG_LOGICAL_DEVICE          0x07
#define BASE_ADDRESS_HIGH           0x60
#define BASE_ADDRESS_LOW            0x61
#define DEV_ID_REGISTER             0x20
#define   V_BMC_DEV_ID  0x3
#define   V_WPC_DEV_ID  0xf1

#define ACTIVATE                    0x30

#define ICH_LPC_EN                  0x400F8080  // D31:F0:R80h
#define ICH_LPC_ENABLES_ME2         0x2000
#define ICH_LPC_ENABLES_SE          0x1000
#define ICH_LPC_ENABLES_ME1         0x0800
#define ICH_LPC_ENABLES_KE          0x0400
#define ICH_LPC_ENABLES_HGE         0x0200
#define ICH_LPC_ENABLES_LGE         0x0100
#define ICH_LPC_ENABLES_FDE         0x0008
#define ICH_LPC_ENABLES_PPE         0x0004
#define ICH_LPC_ENABLES_CBE         0x0002
#define ICH_LPC_ENABLES_CAE         0x0001
#define ICH_IOPORT_PCI_INDEX        0xCF8
#define ICH_IOPORT_PCI_DATA         0xCFC
#define PCI_FUNCTION_NUMBER_ICH_LPC 0
#define PCI_DEVICE_NUMBER_ICH_LPC   31
#define PCI_CF8_ADDR(Bus, Dev, Func, Off) \
    (((Off) & 0xFF) | (((Func) & 0x07) << 8) | (((Dev) & 0x1F) << 11) | (((Bus) & 0xFF) << 16) | (1 << 31))

#define ICH_LPC_CF8_ADDR(Offset)  PCI_CF8_ADDR (0, PCI_DEVICE_NUMBER_ICH_LPC, PCI_FUNCTION_NUMBER_ICH_LPC, Offset)
#define ICH_LPC_IO_DEC            0x80

#ifdef SERIAL_DBG_MSG
#define ConsoleComPort MRC_COM1_BASE // 0x3F8-0x3FF
#define DebugComPort  MRC_COM2_BASE // 0x2F8-0x2FF
#endif

#endif // _UART16550INIT_H
