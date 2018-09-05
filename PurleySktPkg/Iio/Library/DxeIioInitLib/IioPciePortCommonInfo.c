//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioPciePortCommonInfo.c

  This file provides required platform data structure for PCIE.

**/

#include "IioRegs.h"
#include <Uefi/UefiBaseType.h>
#include <Library/PcieCommonInitLib.h>
//************************************************************************************************
//
//  IIO 0
//
//************************************************************************************************

PCIE_LIB_ROOT_PORT_INF Iio0RootPortCStack[] = {
// Valid,Type,       Name  Seg,Bus,       Dev,         Fun,          Attr,    PortNum,   FeatureList
{1, PCIE_LIB_PCIE, "CSP0", 0, 0, PCIE_PORT_0_DEV, PCIE_PORT_0_FUNC,  0,    PORT_0_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio0RootPortPStack0[] = {
// Valid,Type,       Name   Seg,Bus,     Dev,           Fun,           Attr,  PortNum,    FeatureList
{1, PCIE_LIB_PCIE, "PS0P0", 0,   0, PCIE_PORT_1A_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1A_INDEX,   NULL},
{1, PCIE_LIB_PCIE, "PS0P1", 0,   0, PCIE_PORT_1B_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1B_INDEX,   NULL},
{1, PCIE_LIB_PCIE, "PS0P2", 0,   0, PCIE_PORT_1C_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1C_INDEX,   NULL},
{1, PCIE_LIB_PCIE, "PS0P3", 0,   0, PCIE_PORT_1D_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1D_INDEX,   NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio0RootPortPStack1[] = {
// Valid,Type,       Name  Seg,Bus,    Dev,             Fun,          Attr,  PortNum,        FeatureList
{1, PCIE_LIB_PCIE, "PS1P0", 0,   0, PCIE_PORT_2A_DEV, PCIE_PORT_2A_FUNC, 0,   PORT_2A_INDEX,      NULL},
{1, PCIE_LIB_PCIE, "PS1P1", 0,   0, PCIE_PORT_2B_DEV, PCIE_PORT_2B_FUNC, 0,   PORT_2B_INDEX,      NULL},
{1, PCIE_LIB_PCIE, "PS1P2", 0,   0, PCIE_PORT_2C_DEV, PCIE_PORT_2C_FUNC, 0,   PORT_2C_INDEX,      NULL},
{1, PCIE_LIB_PCIE, "PS1P3", 0,   0, PCIE_PORT_2D_DEV, PCIE_PORT_2D_FUNC, 0,   PORT_2D_INDEX,      NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio0RootPortPStack2[] = {
// Valid,Type,       Name  Seg,Bus,     Dev,             Fun,          Attr,    PortNum,     FeatureList
{1, PCIE_LIB_PCIE, "PS2P0", 0,   0, PCIE_PORT_3A_DEV, PCIE_PORT_3A_FUNC, 0,  PORT_3A_INDEX,     NULL},
{1, PCIE_LIB_PCIE, "PS2P1", 0,   0, PCIE_PORT_3B_DEV, PCIE_PORT_3B_FUNC, 0,  PORT_3B_INDEX,     NULL},
{1, PCIE_LIB_PCIE, "PS2P2", 0,   0, PCIE_PORT_3C_DEV, PCIE_PORT_3C_FUNC, 0,  PORT_3C_INDEX,     NULL},
{1, PCIE_LIB_PCIE, "PS2P3", 0,   0, PCIE_PORT_3D_DEV, PCIE_PORT_3D_FUNC, 0,  PORT_3D_INDEX,     NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio0RootPortPStack3[] = {
// Valid,Type,        Name  Seg,Bus,     Dev,               Fun,            Attr,   PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS3P0", 0,   0, PCIE_PORT_4A_DEV, PCIE_PORT_4A_FUNC, 0,   PORT_4A_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS3P1", 0,   0, PCIE_PORT_4B_DEV, PCIE_PORT_4B_FUNC, 0,   PORT_4B_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS3P2", 0,   0, PCIE_PORT_4C_DEV, PCIE_PORT_4C_FUNC, 0,   PORT_4C_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS3P3", 0,   0, PCIE_PORT_4D_DEV, PCIE_PORT_4D_FUNC, 0,   PORT_4D_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio0RootPortPStack4[] = {
// Valid,Type,       Name  Seg,Bus,            Dev,         Fun,          Attr,    PortNum,        FeatureList
{1, PCIE_LIB_PCIE, "PS4P0", 0, 0, PCIE_PORT_5A_DEV, PCIE_PORT_5A_FUNC, 0,    PORT_5A_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P1", 0, 0, PCIE_PORT_5B_DEV, PCIE_PORT_5B_FUNC, 0,    PORT_5B_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P2", 0, 0, PCIE_PORT_5C_DEV, PCIE_PORT_5C_FUNC, 0,    PORT_5C_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P3", 0, 0, PCIE_PORT_5D_DEV, PCIE_PORT_5D_FUNC, 0,    PORT_5D_INDEX,  NULL},
{0},//End of the table
};

//************************************************************************************************
//
//  IIO 1
//
//************************************************************************************************

PCIE_LIB_ROOT_PORT_INF Iio1RootPortCStack[] = {
// Valid,Type,       Name  Seg,Bus,       Dev,         Fun,          Attr,    PortNum,   FeatureList
{1, PCIE_LIB_PCIE, "CSP0", 0, 0, PCIE_PORT_0_DEV, PCIE_PORT_0_FUNC,  0,    PORT_0_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio1RootPortPStack0[] = {
// Valid,Type,       Name   Seg,Bus,     Dev,           Fun,           Attr,  PortNum,    FeatureList
{1, PCIE_LIB_PCIE, "PS0P0", 0,   0, PCIE_PORT_1A_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1A_INDEX,   NULL},
{1, PCIE_LIB_PCIE, "PS0P1", 0,   0, PCIE_PORT_1B_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1B_INDEX,   NULL},
{1, PCIE_LIB_PCIE, "PS0P2", 0,   0, PCIE_PORT_1C_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1C_INDEX,   NULL},
{1, PCIE_LIB_PCIE, "PS0P3", 0,   0, PCIE_PORT_1D_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1D_INDEX,   NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio1RootPortPStack1[] = {
// Valid,Type,       Name  Seg,Bus,    Dev,             Fun,          Attr,  PortNum,        FeatureList
{1, PCIE_LIB_PCIE, "PS1P0", 0,   0, PCIE_PORT_2A_DEV, PCIE_PORT_2A_FUNC, 0,   PORT_2A_INDEX,      NULL},
{1, PCIE_LIB_PCIE, "PS1P1", 0,   0, PCIE_PORT_2B_DEV, PCIE_PORT_2B_FUNC, 0,   PORT_2B_INDEX,      NULL},
{1, PCIE_LIB_PCIE, "PS1P2", 0,   0, PCIE_PORT_2C_DEV, PCIE_PORT_2C_FUNC, 0,   PORT_2C_INDEX,      NULL},
{1, PCIE_LIB_PCIE, "PS1P3", 0,   0, PCIE_PORT_2D_DEV, PCIE_PORT_2D_FUNC, 0,   PORT_2D_INDEX,      NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio1RootPortPStack2[] = {
// Valid,Type,       Name  Seg,Bus,     Dev,             Fun,          Attr,    PortNum,     FeatureList
{1, PCIE_LIB_PCIE, "PS2P0", 0,   0, PCIE_PORT_3A_DEV, PCIE_PORT_3A_FUNC, 0,  PORT_3A_INDEX,     NULL},
{1, PCIE_LIB_PCIE, "PS2P1", 0,   0, PCIE_PORT_3B_DEV, PCIE_PORT_3B_FUNC, 0,  PORT_3B_INDEX,     NULL},
{1, PCIE_LIB_PCIE, "PS2P2", 0,   0, PCIE_PORT_3C_DEV, PCIE_PORT_3C_FUNC, 0,  PORT_3C_INDEX,     NULL},
{1, PCIE_LIB_PCIE, "PS2P3", 0,   0, PCIE_PORT_3D_DEV, PCIE_PORT_3D_FUNC, 0,  PORT_3D_INDEX,     NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio1RootPortPStack3[] = {
// Valid,Type,        Name  Seg,Bus,     Dev,               Fun,            Attr,   PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS3P0", 0,   0, PCIE_PORT_4A_DEV, PCIE_PORT_4A_FUNC, 0,   PORT_4A_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS3P1", 0,   0, PCIE_PORT_4B_DEV, PCIE_PORT_4B_FUNC, 0,   PORT_4B_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS3P2", 0,   0, PCIE_PORT_4C_DEV, PCIE_PORT_4C_FUNC, 0,   PORT_4C_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS3P3", 0,   0, PCIE_PORT_4D_DEV, PCIE_PORT_4D_FUNC, 0,   PORT_4D_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio1RootPortPStack4[] = {
// Valid,Type,       Name  Seg,Bus,            Dev,         Fun,          Attr,    PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS4P0", 0, 0, PCIE_PORT_5A_DEV, PCIE_PORT_5A_FUNC, 0,    PORT_5A_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P1", 0, 0, PCIE_PORT_5B_DEV, PCIE_PORT_5B_FUNC, 0,    PORT_5B_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P2", 0, 0, PCIE_PORT_5C_DEV, PCIE_PORT_5C_FUNC, 0,    PORT_5C_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P3", 0, 0, PCIE_PORT_5D_DEV, PCIE_PORT_5D_FUNC, 0,    PORT_5D_INDEX,  NULL},
{0},//End of the table
};

//************************************************************************************************
//
//  IIO 2
//
//************************************************************************************************
PCIE_LIB_ROOT_PORT_INF Iio2RootPortCStack[] = {
// Valid,Type,       Name  Seg,Bus,       Dev,         Fun,          Attr,    PortNum,   FeatureList
{1, PCIE_LIB_PCIE, "CSP0", 0, 0, PCIE_PORT_0_DEV, PCIE_PORT_0_FUNC,  0,    PORT_0_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio2RootPortPStack0[] = {
// Valid,Type,       Name   Seg,Bus,     Dev,           Fun,           Attr,  PortNum,    FeatureList
{1, PCIE_LIB_PCIE, "PS0P0", 0,   0, PCIE_PORT_1A_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1A_INDEX,   NULL},
{1, PCIE_LIB_PCIE, "PS0P1", 0,   0, PCIE_PORT_1B_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1B_INDEX,   NULL},
{1, PCIE_LIB_PCIE, "PS0P2", 0,   0, PCIE_PORT_1C_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1C_INDEX,   NULL},
{1, PCIE_LIB_PCIE, "PS0P3", 0,   0, PCIE_PORT_1D_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1D_INDEX,   NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio2RootPortPStack1[] = {
// Valid,Type,       Name  Seg,Bus,    Dev,             Fun,          Attr,  PortNum,        FeatureList
{1, PCIE_LIB_PCIE, "PS1P0", 0,   0, PCIE_PORT_2A_DEV, PCIE_PORT_2A_FUNC, 0,   PORT_2A_INDEX,      NULL},
{1, PCIE_LIB_PCIE, "PS1P1", 0,   0, PCIE_PORT_2B_DEV, PCIE_PORT_2B_FUNC, 0,   PORT_2B_INDEX,      NULL},
{1, PCIE_LIB_PCIE, "PS1P2", 0,   0, PCIE_PORT_2C_DEV, PCIE_PORT_2C_FUNC, 0,   PORT_2C_INDEX,      NULL},
{1, PCIE_LIB_PCIE, "PS1P3", 0,   0, PCIE_PORT_2D_DEV, PCIE_PORT_2D_FUNC, 0,   PORT_2D_INDEX,      NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio2RootPortPStack2[] = {
// Valid,Type,       Name  Seg,Bus,     Dev,             Fun,          Attr,    PortNum,     FeatureList
{1, PCIE_LIB_PCIE, "PS2P0", 0,   0, PCIE_PORT_3A_DEV, PCIE_PORT_3A_FUNC, 0,  PORT_3A_INDEX,     NULL},
{1, PCIE_LIB_PCIE, "PS2P1", 0,   0, PCIE_PORT_3B_DEV, PCIE_PORT_3B_FUNC, 0,  PORT_3B_INDEX,     NULL},
{1, PCIE_LIB_PCIE, "PS2P2", 0,   0, PCIE_PORT_3C_DEV, PCIE_PORT_3C_FUNC, 0,  PORT_3C_INDEX,     NULL},
{1, PCIE_LIB_PCIE, "PS2P3", 0,   0, PCIE_PORT_3D_DEV, PCIE_PORT_3D_FUNC, 0,  PORT_3D_INDEX,     NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio2RootPortPStack3[] = {
// Valid,Type,        Name  Seg,Bus,     Dev,               Fun,            Attr,   PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS3P0", 0,   0, PCIE_PORT_4A_DEV, PCIE_PORT_4A_FUNC, 0,   PORT_4A_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS3P1", 0,   0, PCIE_PORT_4B_DEV, PCIE_PORT_4B_FUNC, 0,   PORT_4B_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS3P2", 0,   0, PCIE_PORT_4C_DEV, PCIE_PORT_4C_FUNC, 0,   PORT_4C_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS3P3", 0,   0, PCIE_PORT_4D_DEV, PCIE_PORT_4D_FUNC, 0,   PORT_4D_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio2RootPortPStack4[] = {
// Valid,Type,       Name  Seg,Bus,            Dev,         Fun,          Attr,    PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS4P0", 0, 0, PCIE_PORT_5A_DEV, PCIE_PORT_5A_FUNC, 0,    PORT_5A_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P1", 0, 0, PCIE_PORT_5B_DEV, PCIE_PORT_5B_FUNC, 0,    PORT_5B_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P2", 0, 0, PCIE_PORT_5C_DEV, PCIE_PORT_5C_FUNC, 0,    PORT_5C_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P3", 0, 0, PCIE_PORT_5D_DEV, PCIE_PORT_5D_FUNC, 0,    PORT_5D_INDEX,  NULL},
{0},//End of the table
};

//************************************************************************************************
//
//  IIO 3
//
//************************************************************************************************
PCIE_LIB_ROOT_PORT_INF Iio3RootPortCStack[] = {
// Valid,Type,       Name  Seg,Bus,       Dev,         Fun,         Attr,    PortNum,   FeatureList
{1, PCIE_LIB_PCIE, "CSP0", 0, 0, PCIE_PORT_0_DEV, PCIE_PORT_0_FUNC,  0,    PORT_0_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio3RootPortPStack0[] = {
// Valid,Type,       Name   Seg,Bus,     Dev,           Fun,           Attr,  PortNum,    FeatureList
{1, PCIE_LIB_PCIE, "PS0P0", 0,   0, PCIE_PORT_1A_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1A_INDEX,   NULL},
{1, PCIE_LIB_PCIE, "PS0P1", 0,   0, PCIE_PORT_1B_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1B_INDEX,   NULL},
{1, PCIE_LIB_PCIE, "PS0P2", 0,   0, PCIE_PORT_1C_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1C_INDEX,   NULL},
{1, PCIE_LIB_PCIE, "PS0P3", 0,   0, PCIE_PORT_1D_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1D_INDEX,   NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio3RootPortPStack1[] = {
// Valid,Type,       Name  Seg,Bus,    Dev,             Fun,          Attr,  PortNum,        FeatureList
{1, PCIE_LIB_PCIE, "PS1P0", 0,   0, PCIE_PORT_2A_DEV, PCIE_PORT_2A_FUNC, 0,   PORT_2A_INDEX,      NULL},
{1, PCIE_LIB_PCIE, "PS1P1", 0,   0, PCIE_PORT_2B_DEV, PCIE_PORT_2B_FUNC, 0,   PORT_2B_INDEX,      NULL},
{1, PCIE_LIB_PCIE, "PS1P2", 0,   0, PCIE_PORT_2C_DEV, PCIE_PORT_2C_FUNC, 0,   PORT_2C_INDEX,      NULL},
{1, PCIE_LIB_PCIE, "PS1P3", 0,   0, PCIE_PORT_2D_DEV, PCIE_PORT_2D_FUNC, 0,   PORT_2D_INDEX,      NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio3RootPortPStack2[] = {
// Valid,Type,       Name  Seg,Bus,     Dev,             Fun,          Attr,    PortNum,     FeatureList
{1, PCIE_LIB_PCIE, "PS2P0", 0,   0, PCIE_PORT_3A_DEV, PCIE_PORT_3A_FUNC, 0,  PORT_3A_INDEX,     NULL},
{1, PCIE_LIB_PCIE, "PS2P1", 0,   0, PCIE_PORT_3B_DEV, PCIE_PORT_3B_FUNC, 0,  PORT_3B_INDEX,     NULL},
{1, PCIE_LIB_PCIE, "PS2P2", 0,   0, PCIE_PORT_3C_DEV, PCIE_PORT_3C_FUNC, 0,  PORT_3C_INDEX,     NULL},
{1, PCIE_LIB_PCIE, "PS2P3", 0,   0, PCIE_PORT_3D_DEV, PCIE_PORT_3D_FUNC, 0,  PORT_3D_INDEX,     NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio3RootPortPStack3[] = {
// Valid,Type,        Name  Seg,Bus,     Dev,               Fun,            Attr,   PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS3P0", 0,   0, PCIE_PORT_4A_DEV, PCIE_PORT_4A_FUNC, 0,   PORT_4A_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS3P1", 0,   0, PCIE_PORT_4B_DEV, PCIE_PORT_4B_FUNC, 0,   PORT_4B_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS3P2", 0,   0, PCIE_PORT_4C_DEV, PCIE_PORT_4C_FUNC, 0,   PORT_4C_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS3P3", 0,   0, PCIE_PORT_4D_DEV, PCIE_PORT_4D_FUNC, 0,   PORT_4D_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio3RootPortPStack4[] = {
// Valid,Type,       Name  Seg,Bus,            Dev,         Fun,          Attr,    PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS4P0", 0, 0, PCIE_PORT_5A_DEV, PCIE_PORT_5A_FUNC, 0,    PORT_5A_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P1", 0, 0, PCIE_PORT_5B_DEV, PCIE_PORT_5B_FUNC, 0,    PORT_5B_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P2", 0, 0, PCIE_PORT_5C_DEV, PCIE_PORT_5C_FUNC, 0,    PORT_5C_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P3", 0, 0, PCIE_PORT_5D_DEV, PCIE_PORT_5D_FUNC, 0,    PORT_5D_INDEX,  NULL},
{0},//End of the table
};


PCI_LIB_OP_STRUCT OpcodeForIio0Bif[]={0};  //TODO:Add Bifurcation OP here
PCI_LIB_OP_STRUCT OpcodeForIio1Bif[]={0};  //TODO:Add Bifurcation OP here
PCI_LIB_OP_STRUCT OpcodeForIio2Bif[]={0};  //TODO:Add Bifurcation OP here
PCI_LIB_OP_STRUCT OpcodeForIio3Bif[]={0};  //TODO:Add Bifurcation OP here

// Add RootportList for Socket 4-7
//************************************************************************************************
//
//  IIO 4
//
//************************************************************************************************

PCIE_LIB_ROOT_PORT_INF Iio4RootPortCStack[] = {
// Valid,Type,       Name  Seg,Bus,       Dev,         Fun,          Attr,    PortNum,   FeatureList
{1, PCIE_LIB_PCIE, "CSP0", 0, 0, PCIE_PORT_0_DEV, PCIE_PORT_0_FUNC,  0,    PORT_0_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio4RootPortPStack0[] = {
// Valid,Type,       Name   Seg,Bus,     Dev,           Fun,           Attr,  PortNum,    FeatureList
{1, PCIE_LIB_PCIE, "PS0P0", 0,   0, PCIE_PORT_1A_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS0P1", 0,   0, PCIE_PORT_1B_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS0P2", 0,   0, PCIE_PORT_1C_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS0P3", 0,   0, PCIE_PORT_1D_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio4RootPortPStack1[] = {
// Valid,Type,       Name  Seg,Bus,    Dev,             Fun,          Attr,  PortNum,        FeatureList
{1, PCIE_LIB_PCIE, "PS1P0", 0,   0, PCIE_PORT_2A_DEV, PCIE_PORT_2A_FUNC, 0,   PORT_2A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS1P1", 0,   0, PCIE_PORT_2B_DEV, PCIE_PORT_2B_FUNC, 0,   PORT_2B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS1P2", 0,   0, PCIE_PORT_2C_DEV, PCIE_PORT_2C_FUNC, 0,   PORT_2C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS1P3", 0,   0, PCIE_PORT_2D_DEV, PCIE_PORT_2D_FUNC, 0,   PORT_2D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio4RootPortPStack2[] = {
// Valid,Type,       Name  Seg,Bus,     Dev,             Fun,          Attr,    PortNum,     FeatureList
{1, PCIE_LIB_PCIE, "PS2P0", 0,   0, PCIE_PORT_3A_DEV, PCIE_PORT_3A_FUNC, 0,  PORT_3A_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS2P1", 0,   0, PCIE_PORT_3B_DEV, PCIE_PORT_3B_FUNC, 0,  PORT_3B_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS2P2", 0,   0, PCIE_PORT_3C_DEV, PCIE_PORT_3C_FUNC, 0,  PORT_3C_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS2P3", 0,   0, PCIE_PORT_3D_DEV, PCIE_PORT_3D_FUNC, 0,  PORT_3D_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio4RootPortPStack3[] = {
// Valid,Type,        Name  Seg,Bus,     Dev,               Fun,            Attr,   PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS3P0", 0,   0, PCIE_PORT_4A_DEV, PCIE_PORT_4A_FUNC, 0,   PORT_4A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS3P1", 0,   0, PCIE_PORT_4B_DEV, PCIE_PORT_4B_FUNC, 0,   PORT_4B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS3P2", 0,   0, PCIE_PORT_4C_DEV, PCIE_PORT_4C_FUNC, 0,   PORT_4C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS3P3", 0,   0, PCIE_PORT_4D_DEV, PCIE_PORT_4D_FUNC, 0,   PORT_4D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio4RootPortPStack4[] = {
// Valid,Type,       Name  Seg,Bus,            Dev,         Fun,          Attr,    PortNum,        FeatureList
{1, PCIE_LIB_PCIE, "PS4P0", 0, 0, PCIE_PORT_5A_DEV, PCIE_PORT_5A_FUNC, 0,    PORT_5A_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P1", 0, 0, PCIE_PORT_5B_DEV, PCIE_PORT_5B_FUNC, 0,    PORT_5B_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P2", 0, 0, PCIE_PORT_5C_DEV, PCIE_PORT_5C_FUNC, 0,    PORT_5C_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P3", 0, 0, PCIE_PORT_5D_DEV, PCIE_PORT_5D_FUNC, 0,    PORT_5D_INDEX,  NULL},
{0},//End of the table
};

//************************************************************************************************
//
//  IIO 5
//
//************************************************************************************************

PCIE_LIB_ROOT_PORT_INF Iio5RootPortCStack[] = {
// Valid,Type,       Name  Seg,Bus,       Dev,         Fun,          Attr,    PortNum,   FeatureList
{1, PCIE_LIB_PCIE, "CSP0", 0, 0, PCIE_PORT_0_DEV, PCIE_PORT_0_FUNC,  0,    PORT_0_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio5RootPortPStack0[] = {
// Valid,Type,       Name   Seg,Bus,     Dev,           Fun,           Attr,  PortNum,    FeatureList
{1, PCIE_LIB_PCIE, "PS0P0", 0,   0, PCIE_PORT_1A_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS0P1", 0,   0, PCIE_PORT_1B_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS0P2", 0,   0, PCIE_PORT_1C_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS0P3", 0,   0, PCIE_PORT_1D_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio5RootPortPStack1[] = {
// Valid,Type,       Name  Seg,Bus,    Dev,             Fun,          Attr,  PortNum,        FeatureList
{1, PCIE_LIB_PCIE, "PS1P0", 0,   0, PCIE_PORT_2A_DEV, PCIE_PORT_2A_FUNC, 0,   PORT_2A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS1P1", 0,   0, PCIE_PORT_2B_DEV, PCIE_PORT_2B_FUNC, 0,   PORT_2B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS1P2", 0,   0, PCIE_PORT_2C_DEV, PCIE_PORT_2C_FUNC, 0,   PORT_2C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS1P3", 0,   0, PCIE_PORT_2D_DEV, PCIE_PORT_2D_FUNC, 0,   PORT_2D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio5RootPortPStack2[] = {
// Valid,Type,       Name  Seg,Bus,     Dev,             Fun,          Attr,    PortNum,     FeatureList
{1, PCIE_LIB_PCIE, "PS2P0", 0,   0, PCIE_PORT_3A_DEV, PCIE_PORT_3A_FUNC, 0,  PORT_3A_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS2P1", 0,   0, PCIE_PORT_3B_DEV, PCIE_PORT_3B_FUNC, 0,  PORT_3B_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS2P2", 0,   0, PCIE_PORT_3C_DEV, PCIE_PORT_3C_FUNC, 0,  PORT_3C_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS2P3", 0,   0, PCIE_PORT_3D_DEV, PCIE_PORT_3D_FUNC, 0,  PORT_3D_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio5RootPortPStack3[] = {
// Valid,Type,        Name  Seg,Bus,     Dev,               Fun,            Attr,   PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS3P0", 0,   0, PCIE_PORT_4A_DEV, PCIE_PORT_4A_FUNC, 0,   PORT_4A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS3P1", 0,   0, PCIE_PORT_4B_DEV, PCIE_PORT_4B_FUNC, 0,   PORT_4B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS3P2", 0,   0, PCIE_PORT_4C_DEV, PCIE_PORT_4C_FUNC, 0,   PORT_4C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS3P3", 0,   0, PCIE_PORT_4D_DEV, PCIE_PORT_4D_FUNC, 0,   PORT_4D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio5RootPortPStack4[] = {
// Valid,Type,       Name  Seg,Bus,            Dev,         Fun,          Attr,    PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS4P0", 0, 0, PCIE_PORT_5A_DEV, PCIE_PORT_5A_FUNC, 0,    PORT_5A_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P1", 0, 0, PCIE_PORT_5B_DEV, PCIE_PORT_5B_FUNC, 0,    PORT_5B_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P2", 0, 0, PCIE_PORT_5C_DEV, PCIE_PORT_5C_FUNC, 0,    PORT_5C_INDEX,  NULL},
{1, PCIE_LIB_PCIE, "PS4P3", 0, 0, PCIE_PORT_5D_DEV, PCIE_PORT_5D_FUNC, 0,    PORT_5D_INDEX,  NULL},
{0},//End of the table
};

//************************************************************************************************
//
//  IIO 6
//
//************************************************************************************************
PCIE_LIB_ROOT_PORT_INF Iio6RootPortCStack[] = {
// Valid,Type,       Name  Seg,Bus,       Dev,         Fun,          Attr,    PortNum,   FeatureList
{1, PCIE_LIB_PCIE, "CSP0", 0, 0, PCIE_PORT_0_DEV, PCIE_PORT_0_FUNC,  0,    PORT_0_INDEX,  NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio6RootPortPStack0[] = {
// Valid,Type,       Name   Seg,Bus,     Dev,           Fun,           Attr,  PortNum,    FeatureList
{1, PCIE_LIB_PCIE, "PS0P0", 0,   0, PCIE_PORT_1A_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS0P1", 0,   0, PCIE_PORT_1B_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS0P2", 0,   0, PCIE_PORT_1C_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS0P3", 0,   0, PCIE_PORT_1D_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio6RootPortPStack1[] = {
// Valid,Type,       Name  Seg,Bus,    Dev,             Fun,          Attr,  PortNum,        FeatureList
{1, PCIE_LIB_PCIE, "PS1P0", 0,   0, PCIE_PORT_2A_DEV, PCIE_PORT_2A_FUNC, 0,   PORT_2A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS1P1", 0,   0, PCIE_PORT_2B_DEV, PCIE_PORT_2B_FUNC, 0,   PORT_2B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS1P2", 0,   0, PCIE_PORT_2C_DEV, PCIE_PORT_2C_FUNC, 0,   PORT_2C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS1P3", 0,   0, PCIE_PORT_2D_DEV, PCIE_PORT_2D_FUNC, 0,   PORT_2D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio6RootPortPStack2[] = {
// Valid,Type,       Name  Seg,Bus,     Dev,             Fun,          Attr,    PortNum,     FeatureList
{1, PCIE_LIB_PCIE, "PS2P0", 0,   0, PCIE_PORT_3A_DEV, PCIE_PORT_3A_FUNC, 0,  PORT_3A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS2P1", 0,   0, PCIE_PORT_3B_DEV, PCIE_PORT_3B_FUNC, 0,  PORT_3B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS2P2", 0,   0, PCIE_PORT_3C_DEV, PCIE_PORT_3C_FUNC, 0,  PORT_3C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS2P3", 0,   0, PCIE_PORT_3D_DEV, PCIE_PORT_3D_FUNC, 0,  PORT_3D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio6RootPortPStack3[] = {
// Valid,Type,        Name  Seg,Bus,     Dev,               Fun,            Attr,   PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS3P0", 0,   0, PCIE_PORT_4A_DEV, PCIE_PORT_4A_FUNC, 0,   PORT_4A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS3P1", 0,   0, PCIE_PORT_4B_DEV, PCIE_PORT_4B_FUNC, 0,   PORT_4B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS3P2", 0,   0, PCIE_PORT_4C_DEV, PCIE_PORT_4C_FUNC, 0,   PORT_4C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS3P3", 0,   0, PCIE_PORT_4D_DEV, PCIE_PORT_4D_FUNC, 0,   PORT_4D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio6RootPortPStack4[] = {
// Valid,Type,       Name  Seg,Bus,            Dev,         Fun,          Attr,    PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS4P0", 0, 0, PCIE_PORT_5A_DEV, PCIE_PORT_5A_FUNC, 0,    PORT_5A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS4P1", 0, 0, PCIE_PORT_5B_DEV, PCIE_PORT_5B_FUNC, 0,    PORT_5B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS4P2", 0, 0, PCIE_PORT_5C_DEV, PCIE_PORT_5C_FUNC, 0,    PORT_5C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS4P3", 0, 0, PCIE_PORT_5D_DEV, PCIE_PORT_5D_FUNC, 0,    PORT_5D_INDEX, NULL},
{0},//End of the table
};

//************************************************************************************************
//
//  IIO 7
//
//************************************************************************************************
PCIE_LIB_ROOT_PORT_INF Iio7RootPortCStack[] = {
// Valid,Type,       Name  Seg,Bus,       Dev,         Fun,         Attr,    PortNum,   FeatureList
{1, PCIE_LIB_PCIE, "CSP0", 0, 0, PCIE_PORT_0_DEV, PCIE_PORT_0_FUNC,  0,    PORT_0_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio7RootPortPStack0[] = {
// Valid,Type,       Name   Seg,Bus,     Dev,           Fun,           Attr,  PortNum,    FeatureList
{1, PCIE_LIB_PCIE, "PS0P0", 0,   0, PCIE_PORT_1A_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS0P1", 0,   0, PCIE_PORT_1B_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS0P2", 0,   0, PCIE_PORT_1C_DEV, PCIE_PORT_1A_FUNC, 0, PORT_1C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS0P3", 0,   0, PCIE_PORT_1D_DEV, PCIE_PORT_1B_FUNC, 0, PORT_1D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio7RootPortPStack1[] = {
// Valid,Type,       Name  Seg,Bus,    Dev,             Fun,          Attr,  PortNum,        FeatureList
{1, PCIE_LIB_PCIE, "PS1P0", 0,   0, PCIE_PORT_2A_DEV, PCIE_PORT_2A_FUNC, 0,   PORT_2A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS1P1", 0,   0, PCIE_PORT_2B_DEV, PCIE_PORT_2B_FUNC, 0,   PORT_2B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS1P2", 0,   0, PCIE_PORT_2C_DEV, PCIE_PORT_2C_FUNC, 0,   PORT_2C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS1P3", 0,   0, PCIE_PORT_2D_DEV, PCIE_PORT_2D_FUNC, 0,   PORT_2D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio7RootPortPStack2[] = {
// Valid,Type,       Name  Seg,Bus,     Dev,             Fun,          Attr,    PortNum,     FeatureList
{1, PCIE_LIB_PCIE, "PS2P0", 0,   0, PCIE_PORT_3A_DEV, PCIE_PORT_3A_FUNC, 0,  PORT_3A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS2P1", 0,   0, PCIE_PORT_3B_DEV, PCIE_PORT_3B_FUNC, 0,  PORT_3B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS2P2", 0,   0, PCIE_PORT_3C_DEV, PCIE_PORT_3C_FUNC, 0,  PORT_3C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS2P3", 0,   0, PCIE_PORT_3D_DEV, PCIE_PORT_3D_FUNC, 0,  PORT_3D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio7RootPortPStack3[] = {
// Valid,Type,        Name  Seg,Bus,     Dev,               Fun,            Attr,   PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS3P0", 0,   0, PCIE_PORT_4A_DEV, PCIE_PORT_4A_FUNC, 0,   PORT_4A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS3P1", 0,   0, PCIE_PORT_4B_DEV, PCIE_PORT_4B_FUNC, 0,   PORT_4B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS3P2", 0,   0, PCIE_PORT_4C_DEV, PCIE_PORT_4C_FUNC, 0,   PORT_4C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS3P3", 0,   0, PCIE_PORT_4D_DEV, PCIE_PORT_4D_FUNC, 0,   PORT_4D_INDEX, NULL},
{0},//End of the table
};

PCIE_LIB_ROOT_PORT_INF Iio7RootPortPStack4[] = {
// Valid,Type,       Name  Seg,Bus,            Dev,         Fun,          Attr,    PortNum,       FeatureList
{1, PCIE_LIB_PCIE, "PS4P0", 0, 0, PCIE_PORT_5A_DEV, PCIE_PORT_5A_FUNC, 0,    PORT_5A_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS4P1", 0, 0, PCIE_PORT_5B_DEV, PCIE_PORT_5B_FUNC, 0,    PORT_5B_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS4P2", 0, 0, PCIE_PORT_5C_DEV, PCIE_PORT_5C_FUNC, 0,    PORT_5C_INDEX, NULL},
{1, PCIE_LIB_PCIE, "PS4P3", 0, 0, PCIE_PORT_5D_DEV, PCIE_PORT_5D_FUNC, 0,    PORT_5D_INDEX, NULL},
{0},//End of the table
};

PCI_LIB_OP_STRUCT OpcodeForIio4Bif[]={0};  //TODO:Add Bifurcation OP here
PCI_LIB_OP_STRUCT OpcodeForIio5Bif[]={0};  //TODO:Add Bifurcation OP here
PCI_LIB_OP_STRUCT OpcodeForIio6Bif[]={0};  //TODO:Add Bifurcation OP here
PCI_LIB_OP_STRUCT OpcodeForIio7Bif[]={0};  //TODO:Add Bifurcation OP here

PCIE_LIB_HOST_BRIDGE_INF HostBridgeList[]={
// Revision,                    Seg, MinBus,  MaxBus,   Name, Attr, BifInf,                                          RootportList
{0xFF, 0,    1,      0x7F,   "IIO0", 0,  {1,OpcodeForIio0Bif,sizeof(OpcodeForIio0Bif),80}, Iio0RootPortCStack},
{0xFF, 0,    1,      0x7F,   "IIO0", 0,  {1,OpcodeForIio0Bif,sizeof(OpcodeForIio0Bif),80}, Iio0RootPortPStack0},
{0xFF, 0,    1,      0x7F,   "IIO0", 0,  {1,OpcodeForIio0Bif,sizeof(OpcodeForIio0Bif),80}, Iio0RootPortPStack1},
{0xFF, 0,    1,      0x7F,   "IIO0", 0,  {1,OpcodeForIio0Bif,sizeof(OpcodeForIio0Bif),80}, Iio0RootPortPStack2},
{0xFF, 0,    1,      0x7F,   "IIO0", 0,  {1,OpcodeForIio0Bif,sizeof(OpcodeForIio0Bif),80}, Iio0RootPortPStack3},
{0xFF, 0,    1,      0x7F,   "IIO0", 0,  {1,OpcodeForIio0Bif,sizeof(OpcodeForIio0Bif),80}, Iio0RootPortPStack4},
{0xFF, 0,    0x80,   0xFF,   "IIO1", 0,  {1,OpcodeForIio1Bif,sizeof(OpcodeForIio1Bif),80}, Iio1RootPortCStack},
{0xFF, 0,    0x80,   0xFF,   "IIO1", 0,  {1,OpcodeForIio1Bif,sizeof(OpcodeForIio1Bif),80}, Iio1RootPortPStack0},
{0xFF, 0,    0x80,   0xFF,   "IIO1", 0,  {1,OpcodeForIio1Bif,sizeof(OpcodeForIio1Bif),80}, Iio1RootPortPStack1},
{0xFF, 0,    0x80,   0xFF,   "IIO1", 0,  {1,OpcodeForIio1Bif,sizeof(OpcodeForIio1Bif),80}, Iio1RootPortPStack2},
{0xFF, 0,    0x80,   0xFF,   "IIO1", 0,  {1,OpcodeForIio1Bif,sizeof(OpcodeForIio1Bif),80}, Iio1RootPortPStack3},
{0xFF, 0,    0x80,   0xFF,   "IIO1", 0,  {1,OpcodeForIio1Bif,sizeof(OpcodeForIio1Bif),80}, Iio1RootPortPStack4},
{0xFF, 0,    0x80,   0xFF,   "IIO2", 0,  {1,OpcodeForIio2Bif,sizeof(OpcodeForIio2Bif),80}, Iio2RootPortCStack},
{0xFF, 0,    0x80,   0xFF,   "IIO2", 0,  {1,OpcodeForIio2Bif,sizeof(OpcodeForIio2Bif),80}, Iio2RootPortPStack0},
{0xFF, 0,    0x80,   0xFF,   "IIO2", 0,  {1,OpcodeForIio2Bif,sizeof(OpcodeForIio2Bif),80}, Iio2RootPortPStack1},
{0xFF, 0,    0x80,   0xFF,   "IIO2", 0,  {1,OpcodeForIio2Bif,sizeof(OpcodeForIio2Bif),80}, Iio2RootPortPStack2},
{0xFF, 0,    0x80,   0xFF,   "IIO2", 0,  {1,OpcodeForIio2Bif,sizeof(OpcodeForIio2Bif),80}, Iio2RootPortPStack3},
{0xFF, 0,    0x80,   0xFF,   "IIO2", 0,  {1,OpcodeForIio2Bif,sizeof(OpcodeForIio2Bif),80}, Iio2RootPortPStack4},
{0xFF, 0,    0x80,   0xFF,   "IIO3", 0,  {1,OpcodeForIio3Bif,sizeof(OpcodeForIio3Bif),80}, Iio3RootPortCStack},
{0xFF, 0,    0x80,   0xFF,   "IIO3", 0,  {1,OpcodeForIio3Bif,sizeof(OpcodeForIio3Bif),80}, Iio3RootPortPStack0},
{0xFF, 0,    0x80,   0xFF,   "IIO3", 0,  {1,OpcodeForIio3Bif,sizeof(OpcodeForIio3Bif),80}, Iio3RootPortPStack1},
{0xFF, 0,    0x80,   0xFF,   "IIO3", 0,  {1,OpcodeForIio3Bif,sizeof(OpcodeForIio3Bif),80}, Iio3RootPortPStack2},
{0xFF, 0,    0x80,   0xFF,   "IIO3", 0,  {1,OpcodeForIio3Bif,sizeof(OpcodeForIio3Bif),80}, Iio3RootPortPStack3},
{0xFF, 0,    0x80,   0xFF,   "IIO3", 0,  {1,OpcodeForIio3Bif,sizeof(OpcodeForIio3Bif),80}, Iio3RootPortPStack4},
{0xFF, 0,    0x80,   0xFF,   "IIO4", 0,  {1,OpcodeForIio4Bif,sizeof(OpcodeForIio4Bif),80}, Iio4RootPortCStack},
{0xFF, 0,    0x80,   0xFF,   "IIO4", 0,  {1,OpcodeForIio4Bif,sizeof(OpcodeForIio4Bif),80}, Iio4RootPortPStack0},
{0xFF, 0,    0x80,   0xFF,   "IIO4", 0,  {1,OpcodeForIio4Bif,sizeof(OpcodeForIio4Bif),80}, Iio4RootPortPStack1},
{0xFF, 0,    0x80,   0xFF,   "IIO4", 0,  {1,OpcodeForIio4Bif,sizeof(OpcodeForIio4Bif),80}, Iio4RootPortPStack2},
{0xFF, 0,    0x80,   0xFF,   "IIO4", 0,  {1,OpcodeForIio4Bif,sizeof(OpcodeForIio4Bif),80}, Iio4RootPortPStack3},
{0xFF, 0,    0x80,   0xFF,   "IIO4", 0,  {1,OpcodeForIio4Bif,sizeof(OpcodeForIio4Bif),80}, Iio4RootPortPStack4},
{0xFF, 0,    0x80,   0xFF,   "IIO5", 0,  {1,OpcodeForIio5Bif,sizeof(OpcodeForIio5Bif),80}, Iio5RootPortCStack},
{0xFF, 0,    0x80,   0xFF,   "IIO5", 0,  {1,OpcodeForIio5Bif,sizeof(OpcodeForIio5Bif),80}, Iio5RootPortPStack0},
{0xFF, 0,    0x80,   0xFF,   "IIO5", 0,  {1,OpcodeForIio5Bif,sizeof(OpcodeForIio5Bif),80}, Iio5RootPortPStack1},
{0xFF, 0,    0x80,   0xFF,   "IIO5", 0,  {1,OpcodeForIio5Bif,sizeof(OpcodeForIio5Bif),80}, Iio5RootPortPStack2},
{0xFF, 0,    0x80,   0xFF,   "IIO5", 0,  {1,OpcodeForIio5Bif,sizeof(OpcodeForIio5Bif),80}, Iio5RootPortPStack3},
{0xFF, 0,    0x80,   0xFF,   "IIO5", 0,  {1,OpcodeForIio5Bif,sizeof(OpcodeForIio5Bif),80}, Iio5RootPortPStack4},
{0xFF, 0,    0x80,   0xFF,   "IIO6", 0,  {1,OpcodeForIio6Bif,sizeof(OpcodeForIio6Bif),80}, Iio6RootPortCStack},
{0xFF, 0,    0x80,   0xFF,   "IIO6", 0,  {1,OpcodeForIio6Bif,sizeof(OpcodeForIio6Bif),80}, Iio6RootPortPStack0},
{0xFF, 0,    0x80,   0xFF,   "IIO6", 0,  {1,OpcodeForIio6Bif,sizeof(OpcodeForIio6Bif),80}, Iio6RootPortPStack1},
{0xFF, 0,    0x80,   0xFF,   "IIO6", 0,  {1,OpcodeForIio6Bif,sizeof(OpcodeForIio6Bif),80}, Iio6RootPortPStack2},
{0xFF, 0,    0x80,   0xFF,   "IIO6", 0,  {1,OpcodeForIio6Bif,sizeof(OpcodeForIio6Bif),80}, Iio6RootPortPStack3},
{0xFF, 0,    0x80,   0xFF,   "IIO6", 0,  {1,OpcodeForIio6Bif,sizeof(OpcodeForIio6Bif),80}, Iio6RootPortPStack4},
{0xFF, 0,    0x80,   0xFF,   "IIO7", 0,  {1,OpcodeForIio7Bif,sizeof(OpcodeForIio7Bif),80}, Iio7RootPortCStack},
{0xFF, 0,    0x80,   0xFF,   "IIO7", 0,  {1,OpcodeForIio7Bif,sizeof(OpcodeForIio7Bif),80}, Iio7RootPortPStack0},
{0xFF, 0,    0x80,   0xFF,   "IIO7", 0,  {1,OpcodeForIio7Bif,sizeof(OpcodeForIio7Bif),80}, Iio7RootPortPStack1},
{0xFF, 0,    0x80,   0xFF,   "IIO7", 0,  {1,OpcodeForIio7Bif,sizeof(OpcodeForIio7Bif),80}, Iio7RootPortPStack2},
{0xFF, 0,    0x80,   0xFF,   "IIO7", 0,  {1,OpcodeForIio7Bif,sizeof(OpcodeForIio7Bif),80}, Iio7RootPortPStack3},
{0xFF, 0,    0x80,   0xFF,   "IIO7", 0,  {1,OpcodeForIio7Bif,sizeof(OpcodeForIio7Bif),80}, Iio7RootPortPStack4},
{PCIE_LIB_HOSTBRIDGE_REVISION_END},//End of the table
};

PCIE_LIB_TOPOLOGY_INF PcieTopology = {
  PCIE_LIB_TOPOLOGY_REVISION_1,
  HostBridgeList,
};
