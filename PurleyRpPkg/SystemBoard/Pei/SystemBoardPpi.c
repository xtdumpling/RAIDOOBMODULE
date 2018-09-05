//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Add PCIE bifurcation setting in PEI.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/06/2016
//
//*****************************************************************************
/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2017, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SystemBoardPpi.c

  Setup system board information

**/

#include "SystemBoardPpi.h"

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PchHsioLib.h>
#include <Library/PchPcieRpLib.h>

#include <Ppi/PchHsioPtssTable.h>
#include "PchHHsioPtssTablesBx.h"
#include <Library/HeciMsgLib.h>
#include <Library/SpsPeiLib.h>
#include "PchLbgHsioPtssTablesAx.h"
#include "PchLbgHsioPtssTablesBx.h"
#include "PchLbgHsioPtssTablesBx_Ext.h"
#include "PchLbgHsioPtssTablesSx.h"
#include "PchLbgHsioPtssTablesSx_Ext.h"
#ifdef SPS_SUPPORT
#include <Library/MeTypeLib.h>
#endif // SPS_SUPPORT
#include <PiPei.h>
#include <Guid/SetupVariable.h>

//
// HSSI Card Slot GPIO information
//
FPGA_HSSI_DATA mNeoCityFpgaHssiCardData[] = {
  {TypeNeonCityFPGA, 0,  GPIO_SKL_H_GPP_E2,  GPIO_SKL_H_GPP_E6,  GPIO_SKL_H_GPP_D23},
};

//
// FPGA BBS map table 
//
FPGA_HSSI_BBS_Map mNeoCityFpgaHssiBBSMap[] = {
{TypeNeonCityFPGA, 0, BBS_None },
{TypeNeonCityFPGA, 1, BBS_Gbe},  //2x25Gbe
{TypeNeonCityFPGA, 2, BBS_None},
{TypeNeonCityFPGA, 3, BBS_Gbe},  //4x25Gbe
{TypeNeonCityFPGA, 4, BBS_Gbe},  //2x40Gbe
{TypeNeonCityFPGA, 5, BBS_Gbe},  //2x50Gbe
{TypeNeonCityFPGA, 6, BBS_Gbe},  //4x10Gbe
{TypeNeonCityFPGA, 7, BBS_None},
};


//
// GPIO table for FPGA socket power off
//
FPGA_SOCKET_POWEROFF_MAP mFpgaPowerOffGpioTable[] = {
  {TypeNeonCityFPGA, 0, GPIO_SKL_H_GPP_E0},
  {TypeNeonCityFPGA, 1, GPIO_SKL_H_GPP_E1},

  {TypeOpalCityFPGA, 0, GPIO_SKL_H_GPP_E0},
};

//
// Default FPGA Platform BBS map table 
//
FPGA_PLATFORM_BBS_MAP mDefaultPlatformBbbMap[] = {
  {TypeNeonCityFPGA, 0, BBS_Gbe},
  {TypeNeonCityFPGA, 1, BBS_Gbe},

  {TypeOpalCityFPGA, 0, BBS_Gbe},
};



STATIC SOFTSTRAP_FIXUP_ENTRY SoftstrapFixupTable[] = {
  /// BoardID, strap_num, low_bit, bit_length, value

//
// LBG stepping A0/B0 single SPI image WA section - Start
//
    {TypeNeonCityEPRP, 124, 0, 32, 0x80808080},
    {TypeNeonCityEPRP, 125, 0, 32, 0x0961F400},
    {TypeNeonCityEPRP, 126, 0, 32, 0x5555D000},
    {TypeNeonCityEPRP, 127, 0, 32, 0x1F051C55},
    {TypeNeonCityEPRP, 128, 0, 32, 0x01080400},
    {TypeNeonCityEPRP, 129, 0, 32, 0x0C020508},
    {TypeNeonCityEPRP, 130, 0, 32, 0x0F000E00},
    {TypeNeonCityEPRP, 131, 0, 32, 0x0000000A},

    {TypeNeonCityEPECB, 124, 0, 32, 0x80808080},
    {TypeNeonCityEPECB, 125, 0, 32, 0x0961F400},
    {TypeNeonCityEPECB, 126, 0, 32, 0x5555D000},
    {TypeNeonCityEPECB, 127, 0, 32, 0x1F051C55},
    {TypeNeonCityEPECB, 128, 0, 32, 0x01080400},
    {TypeNeonCityEPECB, 129, 0, 32, 0x0C020508},
    {TypeNeonCityEPECB, 130, 0, 32, 0x0F000E00},
    {TypeNeonCityEPECB, 131, 0, 32, 0x0000000A},

    {TypePurleyLBGEPDVP, 124, 0, 32, 0x80808080},
    {TypePurleyLBGEPDVP, 125, 0, 32, 0x0961F400},
    {TypePurleyLBGEPDVP, 126, 0, 32, 0x5555D000},
    {TypePurleyLBGEPDVP, 127, 0, 32, 0x1F051C55},
    {TypePurleyLBGEPDVP, 128, 0, 32, 0x01080400},
    {TypePurleyLBGEPDVP, 129, 0, 32, 0x0C020508},
    {TypePurleyLBGEPDVP, 130, 0, 32, 0x0F000E00},
    {TypePurleyLBGEPDVP, 131, 0, 32, 0x0000000A},

    {TypeWolfPass, 124, 0, 32, 0x80808080},
    {TypeWolfPass, 125, 0, 32, 0x0961F400},
    {TypeWolfPass, 126, 0, 32, 0x5555D000},
    {TypeWolfPass, 127, 0, 32, 0x1F051C55},
    {TypeWolfPass, 128, 0, 32, 0x01080400},
    {TypeWolfPass, 129, 0, 32, 0x0C020508},
    {TypeWolfPass, 130, 0, 32, 0x0F000E00},
    {TypeWolfPass, 131, 0, 32, 0x0000000A},

    {TypeOpalCitySTHI, 124, 0, 32, 0x80808080},
    {TypeOpalCitySTHI, 125, 0, 32, 0x0961F400},
    {TypeOpalCitySTHI, 126, 0, 32, 0x5555D000},
    {TypeOpalCitySTHI, 127, 0, 32, 0x1F051C55},
    {TypeOpalCitySTHI, 128, 0, 32, 0x01080400},
    {TypeOpalCitySTHI, 129, 0, 32, 0x0C020508},
    {TypeOpalCitySTHI, 130, 0, 32, 0x0F000E00},
    {TypeOpalCitySTHI, 131, 0, 32, 0x0000000A},

    {TypeLightningRidgeEXRP, 124, 0, 32, 0x80808080},
    {TypeLightningRidgeEXRP, 125, 0, 32, 0x0961F400},
    {TypeLightningRidgeEXRP, 126, 0, 32, 0x5555D000},
    {TypeLightningRidgeEXRP, 127, 0, 32, 0x1F051C55},
    {TypeLightningRidgeEXRP, 128, 0, 32, 0x01080400},
    {TypeLightningRidgeEXRP, 129, 0, 32, 0x0C020508},
    {TypeLightningRidgeEXRP, 130, 0, 32, 0x0F000E00},
    {TypeLightningRidgeEXRP, 131, 0, 32, 0x0000000A},

    {TypeLightningRidgeEXECB1, 124, 0, 32, 0x80808080},
    {TypeLightningRidgeEXECB1, 125, 0, 32, 0x0961F400},
    {TypeLightningRidgeEXECB1, 126, 0, 32, 0x5555D000},
    {TypeLightningRidgeEXECB1, 127, 0, 32, 0x1F051C55},
    {TypeLightningRidgeEXECB1, 128, 0, 32, 0x01080400},
    {TypeLightningRidgeEXECB1, 129, 0, 32, 0x0C020508},
    {TypeLightningRidgeEXECB1, 130, 0, 32, 0x0F000E00},
    {TypeLightningRidgeEXECB1, 131, 0, 32, 0x0000000A},

    {TypeNeonCityFPGA, 124, 0, 32, 0x80808080},
    {TypeNeonCityFPGA, 125, 0, 32, 0x0961F400},
    {TypeNeonCityFPGA, 126, 0, 32, 0x5555D000},
    {TypeNeonCityFPGA, 127, 0, 32, 0x1F051C55},
    {TypeNeonCityFPGA, 128, 0, 32, 0x01080400},
    {TypeNeonCityFPGA, 129, 0, 32, 0x0C020508},
    {TypeNeonCityFPGA, 130, 0, 32, 0x0F000E00},
    {TypeNeonCityFPGA, 131, 0, 32, 0x0000000A},

    {TypeOpalCityFPGA, 124, 0, 32, 0x80808080},
    {TypeOpalCityFPGA, 125, 0, 32, 0x0961F400},
    {TypeOpalCityFPGA, 126, 0, 32, 0x5555D000},
    {TypeOpalCityFPGA, 127, 0, 32, 0x1F051C55},
    {TypeOpalCityFPGA, 128, 0, 32, 0x01080400},
    {TypeOpalCityFPGA, 129, 0, 32, 0x0C020508},
    {TypeOpalCityFPGA, 130, 0, 32, 0x0F000E00},
    {TypeOpalCityFPGA, 131, 0, 32, 0x0000000A},
//
// LBG stepping A0/B0 single SPI image WA section - End
//

    {TypeNeonCityEPECB, 15, 0, 2, 0x03}, // sSATA / PCIe Select for Port 0 (SATA_PCIE_SP0)
                            // help:
                            // Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6.
                            // Alternately, this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
    {TypeNeonCityEPECB, 15, 2, 2, 0x03}, // sSATA / PCIe Select for Port 1 (SATA_PCIE_SP1)
                            // help:
                            // Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7.
                            // Alternately, this selection can be made by the value of GPP_H19 (Record 15 bit [17] and Record 19 bit [1]).
    {TypeNeonCityEPECB, 15, 8, 2, 0x0}, // "SATA_PCIE_Select_for_Port_4";
                            //help:
                            // Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 4. Alternately,
                            // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                            // Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit
    {TypeNeonCityEPECB, 15, 10, 2, 0x0}, // "SATA_PCIE_Select_for_Port_5";
                            //help:
                            // Choose to statically assign Flex IO Port 18 to sSATA Port 5 or PCIe Port 5. Alternately,
                            // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                            // Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit
    {TypeNeonCityEPECB, 15, 16, 1, 0x00}, // Polarity of GPP_G23 (GPIO polarity of Select between sSATA Port 0 and PCIe Port 6)
                            // help:
                            // When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH.
                            // A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [0].
                            // Not doing so may cause boot or functional issues on the platform.
    {TypeNeonCityEPECB, 15, 17, 1, 0x00}, // Polarity of GPP_H19 (GPIO polarity of Select between sSATA Port 1 and PCIe Port 7)
                            // help:
                            // When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH.
                            // A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [0].
                            // Not doing so may cause boot or functional issues on the platform.
    {TypeNeonCityEPECB, 15, 20, 1, 0x00}, // "Polarity of GPP_H22 (GPIO polarity of Select between sSATA Port 4 and PCIe Port 10)" set to: "When GPIO pin is 0, PCIe mode is selected, when GPIO pin is , sSATA mode is selected. ";
                            //help:
                            //This strap must also be configured if Record 15 bit [9:8] and Record 18 bit [3:2] are configured to 0x3 and Record 16 bit [9:8] is configured to '0x2'.
                            //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [4]. Not doing so may cause boot or functional issues on the platform.
    {TypeNeonCityEPECB, 15, 21, 1, 0x00}, // "Polarity of GPP_H23 (GPIO polarity of Select between sSATA Port 4 and PCIe Port 10)" set to: "When GPIO pin is 0, PCIe mode is selected, when GPIO pin is 1, sSATA mode is selected. ";
                            //help:
                            //This strap must also be configured if Record 15 bit [11:10] and Record 18 bit [5:4] are configured to 0x3 and Record 16 bit [11:10] is configured to '0x2'.
                            //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [5]. Not doing so may cause boot or functional issues on the platform.
    {TypeNeonCityEPECB, 16, 8, 2, 0x0}, // "PCIE_SATA_COMBO_PORT_4_Strap";
                            //help:
                            // Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 10. Alternately,
                            // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                            // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 17 bit
    {TypeNeonCityEPECB, 16, 10, 2, 0x0}, // "PCIE_SATA_COMBO_PORT_5_Strap";
                            //help:
                            // Choose to statically assign Flex IO Port 18 to sSATA Port 5 or PCIe Port 11. Alternately,
                            // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                            // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 17 bit
    {TypeNeonCityEPECB, 17, 3, 3, 0x00}, // "GbE Legacy PCIe Port Select" set to: "Port 3";
                            // help:
                            // Used to determine which PCIe port to be used for GbE Legacy MAC/PHY over PCI Express communication.
    {TypeNeonCityEPECB, 17, 6, 1, 0x00}, // "Intel (R) GbE Legacy PHY over PCIe Enabled"
                            // help:
                            // Enable or disable GbE Legacy MAC/external PHY communication over PCI Express. Required to be '1' when using the GbE Legacy Controller.
    {TypeNeonCityEPECB, 17, 8, 2, 0x03}, // sSATA / PCIe Combo Port 0
                            // help:
                            // Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6.
                            // Alternately, this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                            // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 16 bit
    {TypeNeonCityEPECB, 17, 10, 2, 0x03}, // sSATA / PCIe Combo Port 1
                            // help:
                            // Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7.
                            // Alternately, this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                            // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 16 bit
    {TypeNeonCityEPECB, 18, 2, 2, 0x00}, // sSATA / PCIe Combo Port 4
                            // help:
                            // Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 10.
                            // Alternately, this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                            // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 16 bit
    {TypeNeonCityEPECB, 18, 4, 2, 0x00}, // sSATA / PCIe Combo Port 5
                            // help:
                            // Choose to statically assign Flex IO Port 18 to sSATA Port 5 or PCIe Port 11.
                            // Alternately, this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                            // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 16 bit
    {TypeNeonCityEPECB, 18, 14, 2, 0x01}, // SATA / PCIe Combo Port 4
                            // help:
                            // Choose to statically assign Flex IO Port 19 to SATA Port 4 or PCIe Port 16.
                            // Alternately, this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                            // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 16 bit
   {TypeNeonCityEPECB, 19, 4, 1, 0x00}, // "Polarity Select sSATA / PCIe Combo Port 4" set to: "When GPIO pin is 0, sSATA mode is selected, when GPIO pin is 1, PCIe mode is selected";
                            //help:
                            //This strap must also be configured if Record 15 bit [9:8] and Record 17 bit [9:8] are configured to 0x3 and Record 16 bit [9:8] is configured to '0x2'.
                            //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 15 bit [20]. Not doing so may cause boot or functional issues on the platform.
    {TypeNeonCityEPECB, 19, 16, 2, 0x01}, // USB3 / PCIe Combo Port 0
                            // help:
                            // Choose to statically assign Flex IO Port 7 to USB3 Port 7 or PCIe Port 0.
                            // Ensure that the value for this strap is the same as Record 95  [17:16]. Not doing so may cause boot or functional issues on the platform.
    {TypeNeonCityEPECB, 19, 18, 2, 0x01}, // USB3 / PCIe Combo Port 1
                            // help:
                            // Choose to statically assign Flex IO Port 8 to USB3 Port 8 or PCIe Port 1.
                            // Ensure that the value for this strap is the same as Record 95  [17:16]. Not doing so may cause boot or functional issues on the platform.
    {TypeNeonCityEPECB, 19, 20, 2, 0x01}, // USB3 / PCIe Combo Port 2
                            // help:
                            // Choose to statically assign Flex IO Port 9 to USB3 Port 9 or PCIe Port 12.
                            // Ensure that the value for this strap is the same as Record 95  [17:16]. Not doing so may cause boot or functional issues on the platform.
    {TypeNeonCityEPECB, 94, 2, 2, 0x01}, // Flex IO Port 23 AUXILLARY Mux Select between SATA Port 4 and PCIe Port 16
                            // help:
                            // Choose to statically assign Flex IO Port 23 to SATA Port 4 or PCIe Port 16.
                            // Alternately, this selection can be made by the value of GPP_F1 (Record 19 bit [10] and Record 102 bit [20]).
                            // Ensure that the value for this strap is the same as Record 18  bit [15:14] and Record 102
    {TypeNeonCityEPECB, 94, 4, 2, 0x01}, // Flex IO Port 24 AUXILLARY Mux Select between SATA Port 5 and PCIe Port 17
                            // help:
                            // Choose to statically assign Flex IO Port 24 to SATA Port 5 or PCIe Port 17.
                            // Alternately, this selection can be made by the value of GPP_F1 (Record 19 bit [10] and Record 102 bit [20]).
                            // Ensure that the value for this strap is the same as Record 18  bit [15:14] and Record 102
    {TypeNeonCityEPECB, 94, 6, 2, 0x01}, // Flex IO Port 25 AUXILLARY Mux Select between SATA Port 6 and PCIe Port 18
                            // help:
                            // Choose to statically assign Flex IO Port 25 to SATA Port 6 or PCIe Port 18.
                            // Alternately, this selection can be made by the value of GPP_F1 (Record 19 bit [10] and Record 102 bit [20]).
                            // Ensure that the value for this strap is the same as Record 18  bit [15:14] and Record 102
    {TypeNeonCityEPECB, 94, 8, 2, 0x01}, // Flex IO Port 26 AUXILLARY Mux Select between SATA Port 7 and PCIe Port 19
                            // help:
                            // Choose to statically assign Flex IO Port 26 to SATA Port 7 or PCIe Port 19.
                            // Alternately, this selection can be made by the value of GPP_F1 (Record 19 bit [10] and Record 102 bit [20]).
                            // Ensure that the value for this strap is the same as Record 18  bit [15:14] and Record 102
    {TypeNeonCityEPECB, 102, 8, 2, 0x01}, // "Flex IO Port 23 Mux Select between SATA Port 4 and PCIe Port 16
                            // help:
                            // Choose to statically assign Flex IO Port 23 to SATA Port 4 or PCIe Port 16.
                            // Alternately, this selection can be made by the value of GPP_F1 (Record 19 bit [10] and Record 102 bit [20]).
                            // Ensure that the value for this strap is the same as Record 18  bit [15:14] and Record 94
    {TypeNeonCityEPECB, 102, 10, 2, 0x01}, // Flex IO Port 24 Mux Select between SATA Port 5 and PCIe Port 17
                            // help:
                            // Choose to statically assign Flex IO Port 24 to SATA Port 5 or PCIe Port 17.
                            // Alternately, this selection can be made by the value of GPP_F1 (Record 19 bit [10] and Record 102 bit [20]).
                            // Ensure that the value for this strap is the same as Record 18  bit [15:14] and Record 94
    {TypeNeonCityEPECB, 102, 12, 2, 0x01}, // Flex IO Port 25 Mux Select between SATA Port 6 and PCIe Port 18
                            // help:
                            // Choose to statically assign Flex IO Port 25 to SATA Port 6 or PCIe Port 16.
                            // Alternately, this selection can be made by the value of GPP_F1 (Record 19 bit [10] and Record 102 bit [20]).
                            // Ensure that the value for this strap is the same as Record 18  bit [15:14] and Record 94
    {TypeNeonCityEPECB, 102, 14, 2, 0x01}, // Flex IO Port 26 Mux Select between SATA Port 7 and PCIe Port 19
                            // help:
                            // Choose to statically assign Flex IO Port 26 to SATA Port 7 or PCIe Port 19.
                            // Alternately, this selection can be made by the value of GPP_F1 (Record 19 bit [10] and Record 102 bit [20]).
                            // Ensure that the value for this strap is the same as Record 18  bit [15:14] and Record 94
    {TypeNeonCityEPECB, 108, 11, 2, 0x02}, // PCIe Controller 2 (Port 4-7)
                            // help:
                            // Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
    {TypeNeonCityEPECB, 110, 11, 2, 0x00}, // PCIe Controller 3 (Port 8-11)
                            // help:
                            // Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
    {TypeNeonCityEPECB, 114, 11, 2, 0x03}, // PCIe Controller 4 (Port 12-15)
                            // help:
                            // Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
    {TypeNeonCityEPECB, 117, 22, 1, 0x01}, // Port 7 Ownership Selection
    {TypeNeonCityEPECB, 117, 23, 1, 0x01}, // Port 8 Ownership Selection
    {TypeNeonCityEPECB, 117, 24, 1, 0x01}, // Port 9 Ownership Selection
    {TypeNeonCityEPECB, 117, 25, 1, 0x01}, // Port 10 Ownership Selection

  ///
  /// Wolf Pass platform (Board ID=0x01 - TypeWolfPass)
  ///
  {TypeWolfPass, 2, 7, 1, 0x01}, // "MROM1 Enabled" set to: "MROM1 Disabled for not support IE";
                    //help:
                    //MROM contains BIOS code that will setup and enable many of the platform management features.
                    //Disable if not use IE.
  {TypeWolfPass, 15, 0, 2, 0x01}, // "sSATA / PCIe Select for Port 0 (SATA_PCIE_SP0)" set to: "Statically assigned to PCIe Port 6";
                    //help:
                    //Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6. Alternately, this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]). Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit [9:8]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 15, 2, 2, 0x03}, // "sSATA / PCIe Select for Port 1 (SATA_PCIE_SP1)" set to: "Assigned based on sSATA Port 1 GPIO pin(GPP_H19) and polarity softstrap";
                    //help:
                    //Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7. Alternately, this selection can be made by the value of GPP_H19 (Record 15 bit [17] and Record 19 bit [1]). Ensure that the value for this strap is the same as Record 16 bit [3:2] and Record 17 bit [11:10]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 15, 6, 2, 0x01}, // "sSATA / PCIe Select for Port 3 (SATA_PCIE_SP3)" set to: "Statically assigned to PCIe Port 9";
                    //help:
                    //Choose to statically assign Flex IO Port 16 to sSATA Port 3 or PCIe Port 9. Alternately, this selection can be made by the value of GPP_H21 (Record 15 bit [19] and Record 19 bit [3]).Ensure that the value for this strap is the same as Record 16 bit [7:6] and Record 18 bit [1:0]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 15, 8, 2, 0x00}, // "sSATA / PCIe Select for Port 4 (SATA_PCIE_SP4)" set to: "Statically assigned to SSATA Port 4";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 10. Alternately, this selection can be made by the value of GPP_H22 (Record 15 bit [20] and Record 19 bit [4]).Ensure that the value for this strap is the same as Record 16 bit [9:8] and Record 18 bit [3:2]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 15, 10, 2, 0x00}, // "sSATA / PCIe Select for Port 5 (SATA_PCIE_SP5)" set to: "Statically assigned to SSATA Port 5";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 10. Alternately, this selection can be made by the value of GPP_H22 (Record 15 bit [20] and Record 19 bit [4]).Ensure that the value for this strap is the same as Record 16 bit [9:8] and Record 18 bit [3:2]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 15, 17, 1, 0x01}, // "Polarity of GPP_H19 (GPIO polarity of Select between sSATA Port 1 and PCIe Port 7)" set to: "When GPIO pin is ‘0’, sSATA mode is selected, when GPIO pin is ‘1’, PCIe mode is selected";
                    //help:
                    //This strap must also be configured if Record 15 bit [3:2] and Record 17 bit [11:10] are configured to ‘0x3’ and Record 16 bit [3:2] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [1]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 15, 18, 1, 0x01}, // "Polarity of GPP_H20 (GPIO polarity of Select between sSATA Port 2 and PCIe Port 8)" set to: "When GPIO pin is ‘0’, sSATA mode is selected, when GPIO pin is ‘1’, PCIe mode is selected";
                    //help:
                    //This strap must also be configured if Record 15 bit [5:4] and Record 17 bit [13:12] are configured to ‘0x3’ and Record 16 bit [5:4] is configured as '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [2]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 15, 19, 1, 0x00}, // "Polarity of GPP_H21 (GPIO polarity of Select between sSATA Port 3 and PCIe Port 9)" set to: "When GPIO pin is ‘0’, PCIe mode is selected, when GPIO pin is ‘1’, sSATA mode is selected. ";
                    //help:
                    //This strap must also be configured if Record 15 bit [7:6] and Record 18 bit [1:0] are configured to ‘0x3’ and Record 16 bit [7:6] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [3]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 15, 21, 1, 0x00}, // "Polarity of GPP_H23 (GPIO polarity of Select between sSATA Port 5 and PCIe Port 11)" set to: "When GPIO pin is ‘0’, PCIe mode is selected, when GPIO pin is ‘1’, sSATA mode is selected. ";
                    //help:
                    //This strap must also be configured if Record 15 bit [11:10] and Record 18 bit [5:4] are configured to ‘0x3’ and Record 16 bit [11:10] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [5]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 17, 3, 3, 0x00}, // "GbE Legacy PCIe Port Select" set to: "Port 3";
                    //help:
                    //Used to determine which PCIe port to be used for GbE Legacy MAC/PHY over PCI Express communication.
  {TypeWolfPass, 17, 6, 1, 0x00}, // "Intel (R) GbE Legacy PHY over PCIe Enabled" set to: "GbE MAC/PHY port communication is not Enabled over PCI Express";
                    //help:
                    //Enable or disable GbE Legacy MAC/external PHY communication over PCI Express. Required to be '1' when using the GbE Legacy Controller.
  {TypeWolfPass, 17, 8, 2, 0x01}, // "sSATA / PCIe Combo Port 0" set to: "Statically assigned to PCIe Port 6";
                    //help:
                    //Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6. Alternately, this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]). Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 16 bit [1:0]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 17, 10, 2, 0x03}, // "sSATA / PCIe Combo Port 1" set to: "Assigned based on GPIO pin (GPP_H19) and polarity softstrap";
                    //help:
                    //Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7. Alternately, this selection can be made by the value of GPP_H19 (Record 15 bit [17] and Record 19 bit [1]). Ensure that the value for this strap is the same as Record 15 bit [3:2] and Record 16 bit [3:2]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 18, 2, 2, 0x00}, // "sSATA / PCIe Combo Port 4" set to: "Statically assigned to sSATA Port 4";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 10. Alternately, this selection can be made by the value of GPP_H22 (Record 15 bit [20] and Record 19 bit [4]). Ensure that the value for this strap is the same as Record 15 bit [9:8] and Record 16 bit [9:8]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 18, 4, 2, 0x00}, // "sSATA / PCIe Combo Port 5" set to: "Statically assigned to sSATA Port 5";
                    //help:
                    //Choose to statically assign Flex IO Port 18 to sSATA Port 5 or PCIe Port 11 (or GbE). Alternately, this selection can be made by the value of GPP_H23 (Record 15 bit [21] and Record 19 bit [5]). Ensure that the value for this strap is the same as Record 15 bit [11:10] and Record 16 bit [11:10]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 18, 16, 2, 0x00}, // "USB3 / PCIe Combo Port 0" set to: "Statically assigned to USB3 P7 (default prior to strap sample)";
                    //help:
                    //Choose to statically assign Flex IO Port 7 to USB3 Port 7 or PCIe Port 0. Ensure that the value for this strap is the same as Record 95  [17:16]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 18, 18, 2, 0x00}, // "USB3 / PCIe Combo Port 1" set to: "Statically assigned to USB3 P8 (default prior to strap sample)";
                    //help:
                    //Choose to statically assign Flex IO Port 8 to USB3 Port 8 or PCIe Port 1. Ensure that the value for this strap is the same as Record 95 bit [19:18]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 18, 20, 2, 0x00}, // "USB3 / PCIe Combo Port 2" set to: "Statically assigned to USB3 P9 (default prior to strap sample)";
                    //help:
                    //Choose to statically assign Flex IO Port 9 to USB3 Port 9 or PCIe Port 2.Ensure that the value for this strap is the same as Record 95 bit [21:20]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 19, 1, 1, 0x01}, // "Polarity Select sSATA / PCIe Combo Port 1" set to: "When GPIO pin is ‘0’, sSATA mode is selected, when GPIO pin is ‘1’, PCIe mode is selected";
                    //help:
                    //This strap must also be configured if Record 15 bit [3:2] and Record 17 bit [11:10] are configured to ‘0x3’ and Record 16 bit [3:2] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 15 bit [17]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 19, 2, 1, 0x01}, // "Polarity Select sSATA / PCIe Combo Port 2" set to: "When GPIO pin is ‘0’, sSATA mode is selected, when GPIO pin is ‘1’, PCIe mode is selected";
                    //help:
                    //This strap must also be configured if Record 15 bit [5:4] and Record 17 bit [13:12] are configured to ‘0x3’ and Record 16 bit [5:4] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 15 bit [18]. Not doing so may cause boot or functional issues on the platform.
  {TypeWolfPass, 23, 8, 1, 0x00}, // "SMLink0B Enable" set to: "SMT Disabled";
                    //help:
                    //SMLink0B enable
  {TypeWolfPass, 24, 16, 1, 0x00}, // "SMLink0B I2C Target Address Enable" set to: "I2C Target Address Disabled";
                    //help:
                    //If providing an I2C target address (for I2C writes), then set this to 1, otherwise 0.
  {TypeWolfPass, 28, 8, 1, 0x00}, // "SMLink0 Enable" set to: "SMT Disabled";
                    //help:
                    //SMLink0 enable
  {TypeWolfPass, 33, 8, 1, 0x00}, // "SMLink1 Enable" set to: "SMT Disabled";
                    //help:
                    //Enable or disable this SMBus network
  {TypeWolfPass, 84, 24, 1, 0x00}, // "SMS1 Gbe Legacy MAC SMBus Address Enable" set to: "GbE MAC Address Disabled";
                    //help:
                    //
  {TypeWolfPass, 85, 8, 3, 0x00}, // "SMS1 PMC SMBus Connect" set to: "SMS Not Connected to any bus";
                    //help:
                    //
  {TypeWolfPass, 86, 24, 1, 0x00}, // "SMS2 GbE MAC SMBus Address Enable" set to: "GbE MAC Address Disabled";
                    //help:
                    //
  {TypeWolfPass, 92, 2, 1, 0x01}, // "SLP_A# / GPD6 Signal Configuration" set to: "Use as GPD_6";
                    //help:
                    //GPD6_SLP_A_N pin, select between SLP_A# native function and GPIO GPD_6
  {TypeWolfPass, 92, 6, 2, 0x00}, // "LAN PHY Power Control GPD11 Signal Configuration" set to: "Use as GPD_11";
                    //help:
                    //GPD11_GBEPHY pin, select between LANPHYPC native function and GPIO GPD_11
  {TypeWolfPass, 92, 8, 1, 0x01}, // "SLP_S5# / GPD10 Signal Configuration" set to: "Use as GPD_10";
                    //help:
                    //GPD10_SLP_S5_N pin, select between SLP_S5# native function and GPIO GPD_10
  {TypeWolfPass, 103, 16, 3, 0x00}, // "GbE Legacy PHY Smbus Connection" set to: "No PHY connected";
                    //help:
                    //Select which SMBus network is used to connect GbE Legacy PHY to MAC/PCH.
  {TypeWolfPass, 103, 26, 1, 0x00}, // "GbE Legacy LCD SMBus PHY Address Enabled" set to: "Disabled";
                    //help:
                    //Set to '1' if GbE Legacy MAC/PHY is enabled, otherwise set to '0'.
  {TypeWolfPass, 103, 27, 1, 0x00}, // "GbE Legacy LC SMBus Address Enabled" set to: "Disabled";
                    //help:
                    //Set to '1' if GbE Legacy MAC/PHY is enabled, otherwise set to '0'.
  {TypeWolfPass, 106, 11, 2, 0x00}, // "PCIe Controller 1 (Port 0-3)" set to: "4x1, Ports 1-4 (x1)";
                    //help:
                    //Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
  {TypeWolfPass, 108, 10, 1, 0x01}, // "PCIe Controller 2 Lane Reversal Enabled" set to: "PCIe Lanes are reversed";
                    //help:
                    //PCIe Lane Reversal is only supported if all 4 ports are configured in x4 mode.
  {TypeWolfPass, 108, 11, 2, 0x03}, // "PCIe Controller 2 (Port 4-7)" set to: "Port 4 (x4), Ports 5-7 (disabled)";
                    //help:
                    //Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
  {TypeWolfPass, 110, 11, 2, 0x01}, // "PCIe Controller 3 (Port 8-11)" set to: "2x1 Port 8 (x2), Port 9 (disabled), Ports 10, 11 (x1)";
                    //help:
                    //Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
  {TypeWolfPass, 110, 28, 1, 0x00}, // "PCIe Controller 3 Port 1 SRIS Enabled" set to: "Disabled";
                    //help:
                    //Enable or disable SRIS on this PCIe Port.
  {TypeWolfPass, 110, 29, 1, 0x00}, // "PCIe Controller 3 Port 2 SRIS Enabled" set to: "Disabled";
                    //help:
                    //Enable or disable SRIS on this PCIe Port.
  {TypeWolfPass, 119, 6, 1, 0x00}, // "Aux Well Power Policy" set to: "Power down the AUX well prior to MBB_RST# deassertion";
                    //help:
                    //Set to '1' if GbE Legacy MAC/PHY is enabled, otherwise set to '0'.
  {TypeWolfPass, 119, 12, 1, 0x00}, // "EVA Signalling Enable (EVA_SIG_EN)" set to: "Disabled";
                    //help:
                    //This bit indicates whether EVA signalling is enabled
  {TypeWolfPass, 119, 20, 1, 0x00}, // "DeepSx Platform Configuration (DEEPSX_PLT_CFG_SS)" set to: "The platform does not support DeepSx";
                    //help:
                    //Indicates if the platform supports Deep Sx
  {TypeWolfPass, 120, 0, 1, 0x01}, // "Intel(R) Integrated wired LAN Enable" set to: "GbE is Disabled";
                    //help:
                    //Disable or enable GbE Legacy MAC operation.
  {TypeWolfPass, 120, 13, 1, 0x01}, // "LPC Disabled " set to: "LPC Disabled";
                    //help:
                    //Disable or enable LPC controller. Note: this strap is not an eSPI/LPC switch.
  {TypeWolfPass, 133, 23, 1, 0x00}, // "SPI Voltage Select" set to: "The VCCSPI supply is 3.3 Volt";
                    //help:
                    //Note:
                    //The strap defaults to 1.8V mode before the softstraps are loaded, i.e. before the actual supply voltage is known. This is because the pad performance is slightly better when assuming 1.8V when the actual is 3.3 than vice-versa.
                    //

  ///
  /// Opal City  platform (Board ID=0x06 - TypeOpalCitySTHI)
  ///
  {TypeOpalCitySTHI, 15, 10, 2, 0x1}, // "SATA_PCIE_Select_for_Port_5";
                    //help:
                    // Choose to statically assign Flex IO Port 18 to sSATA Port 5 or PCIe Port 5. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit
  {TypeOpalCitySTHI, 15, 8, 2, 0x1}, // "SATA_PCIE_Select_for_Port_4";
                    //help:
                    // Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 4. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit
  {TypeOpalCitySTHI, 15, 2, 2, 0x1}, // "SATA_PCIE_Select_for_Port_1";
                    //help:
                    // Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit
  {TypeOpalCitySTHI, 15, 0, 2, 0x1}, // "SATA_PCIE_Select_for_Port_0";
                    //help:
                    // Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit
  {TypeOpalCitySTHI, 16, 10, 2, 0x1}, // "PCIE_SATA_COMBO_PORT_5_Strap";
                    //help:
                    // Choose to statically assign Flex IO Port 18 to sSATA Port 5 or PCIe Port 11. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 17 bit
  {TypeOpalCitySTHI, 16, 8, 2, 0x1}, // "PCIE_SATA_COMBO_PORT_4_Strap";
                    //help:
                    // Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 10. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 17 bit
  {TypeOpalCitySTHI, 16, 2, 2, 0x1}, // "PCIE_SATA_COMBO_PORT_1_Strap";
                    //help:
                    // Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 17 bit
  {TypeOpalCitySTHI, 16, 0, 2, 0x1}, // "PCIE_SATA_COMBO_PORT_0_Strap";
                    //help:
                    // Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 17 bit
  {TypeOpalCitySTHI, 17, 10, 2, 0x1}, // "sSATA / PCIe Combo Port 1";
                    //help:
                    // Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7. Alternately,
                    // this selection can be made by the value of GPP_H19 (Record 15 bit [17] and Record 19 bit [1]).
                    // Ensure that the value for this strap is the same as Record 15 bit [3:2] and Record 16 bit
  {TypeOpalCitySTHI, 17, 8, 2, 0x1}, // "sSATA / PCIe Combo Port 0";
                    //help:
                    // Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6. Alternately,
                    // this selection can be made by the value of GPP_H19 (Record 15 bit [17] and Record 19 bit [1]).
                    // Ensure that the value for this strap is the same as Record 15 bit [3:2] and Record 16 bit
  {TypeOpalCitySTHI, 17, 3, 3, 0x4}, // "GbE Legacy PCIe Port Select";
                    //help:
                    // Used to determine which PCIe port to be used for GbE Legacy MAC/PHY over PCI Express communication.
  {TypeOpalCitySTHI, 18, 4, 2, 0x1}, // "sSATA / PCIe Combo Port 5";
                    //help:
                    // Choose to statically assign Flex IO Port 18 to sSATA Port 5 or PCIe Port 11 (or GbE). Alternately,
                    // this selection can be made by the value of GPP_H23 (Record 15 bit [21] and Record 19 bit [5]).
                    // Ensure that the value for this strap is the same as Record 15 bit [11:10] and
  {TypeOpalCitySTHI, 18, 2, 2, 0x1}, // "sSATA / PCIe Combo Port 4";
                    //help:
                    // Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 10. Alternately,
                    // this selection can be made by the value of GPP_H22 (Record 15 bit [20] and Record 19 bit [4]).
                    // Ensure that the value for this strap is the same as Record 15 bit [9:8] and Record 16 bit
  {TypeOpalCitySTHI, 85, 8, 2, 0x2}, // "SMS1 PMC SMBus Connect";
                    //help:
  {TypeOpalCitySTHI, 92, 8, 1, 0x1}, // "SLP_S5# / GPD10 Signal Configuration";
                    //help:
                    //GPD10_SLP_S5_N pin, select between SLP_S5# native function and GPIO GPD_10
  {TypeOpalCitySTHI, 92, 2, 1, 0x1}, // "SLP_A# / GPD6 Signal Configuration";
                    //help:
                    //GPD6_SLP_A_N pin, select between SLP_A# native function and GPIO GPD_6
  {TypeOpalCitySTHI, 96, 3, 1, 0x1}, // "SMT3 connected to the pins enable";
                    //help:
  {TypeOpalCitySTHI, 96, 2, 1, 0x1}, // "SMT2 connected to the pins enable";
                    //help:
  {TypeOpalCitySTHI, 96, 1, 1, 0x1}, // "SMT1 connected to the pins enable";
                    //help:
  {TypeOpalCitySTHI, 97, 0, 1, 0x0}, // "TPM Over SPI Bus Enabled";
                    //help: TPM on SPI provided SPI TPM disable fuse = 0 and ME FTPM enable = 0
  {TypeOpalCitySTHI, 104, 0, 2, 0x2}, // "GbE SMLINK3 SMBus Frequency";
                    //help:
                    //The value of these bits determine the physical bus speed supported by the SMLink3 HW
  {TypeOpalCitySTHI, 106, 10, 1, 0x1}, // "PCIe Controller 1 Lane Reversal Enabled";
                    //help:
                    //PCIe Lane Reversal is only supported if all 4 ports are configured in x4 mode.
  {TypeOpalCitySTHI, 108, 11, 2, 0x3}, // "PCIe Controller 2 (Port 4-7)";
                    //help:
                    //Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
  {TypeOpalCitySTHI, 110, 11, 2, 0x0}, // "PCIe Controller 3 (Port 8-11)";
                    //help:
                    //Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
  {TypeOpalCitySTHI, 119, 20, 1, 0x0}, // "SMS2CON - Shared with SMT";
                    //help:
                    //0x3;SMT3 Bus
  {TypeOpalCitySTHI, 123, 0, 2, 0x3}, // "SMS2CON - Shared with SMT";
                    //help:
                    //0x3;SMT3 Bus

  ///
  /// Opal City platform (Board ID=0x0E - TypeOpalCityFPGA)
  ///
  {TypeOpalCityFPGA, 15, 10, 2, 0x1}, // "SATA_PCIE_Select_for_Port_5";
                    //help:
                    // Choose to statically assign Flex IO Port 18 to sSATA Port 5 or PCIe Port 5. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit
  {TypeOpalCityFPGA, 15, 8, 2, 0x1}, // "SATA_PCIE_Select_for_Port_4";
                    //help:
                    // Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 4. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit
  {TypeOpalCityFPGA, 15, 2, 2, 0x1}, // "SATA_PCIE_Select_for_Port_1";
                    //help:
                    // Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit
  {TypeOpalCityFPGA, 15, 0, 2, 0x1}, // "SATA_PCIE_Select_for_Port_0";
                    //help:
                    // Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit
  {TypeOpalCityFPGA, 16, 10, 2, 0x1}, // "PCIE_SATA_COMBO_PORT_5_Strap";
                    //help:
                    // Choose to statically assign Flex IO Port 18 to sSATA Port 5 or PCIe Port 11. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 17 bit
  {TypeOpalCityFPGA, 16, 8, 2, 0x1}, // "PCIE_SATA_COMBO_PORT_4_Strap";
                    //help:
                    // Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 10. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 17 bit
  {TypeOpalCityFPGA, 16, 2, 2, 0x1}, // "PCIE_SATA_COMBO_PORT_1_Strap";
                    //help:
                    // Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 17 bit
  {TypeOpalCityFPGA, 16, 0, 2, 0x1}, // "PCIE_SATA_COMBO_PORT_0_Strap";
                    //help:
                    // Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6. Alternately,
                    // this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]).
                    // Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 17 bit
  {TypeOpalCityFPGA, 17, 10, 2, 0x1}, // "sSATA / PCIe Combo Port 1";
                    //help:
                    // Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7. Alternately,
                    // this selection can be made by the value of GPP_H19 (Record 15 bit [17] and Record 19 bit [1]).
                    // Ensure that the value for this strap is the same as Record 15 bit [3:2] and Record 16 bit
  {TypeOpalCityFPGA, 17, 8, 2, 0x1}, // "sSATA / PCIe Combo Port 0";
                    //help:
                    // Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6. Alternately,
                    // this selection can be made by the value of GPP_H19 (Record 15 bit [17] and Record 19 bit [1]).
                    // Ensure that the value for this strap is the same as Record 15 bit [3:2] and Record 16 bit
  {TypeOpalCityFPGA, 17, 3, 3, 0x4}, // "GbE Legacy PCIe Port Select";
                    //help:
                    // Used to determine which PCIe port to be used for GbE Legacy MAC/PHY over PCI Express communication.
  {TypeOpalCityFPGA, 18, 4, 2, 0x1}, // "sSATA / PCIe Combo Port 5";
                    //help:
                    // Choose to statically assign Flex IO Port 18 to sSATA Port 5 or PCIe Port 11 (or GbE). Alternately,
                    // this selection can be made by the value of GPP_H23 (Record 15 bit [21] and Record 19 bit [5]).
                    // Ensure that the value for this strap is the same as Record 15 bit [11:10] and
  {TypeOpalCityFPGA, 18, 2, 2, 0x1}, // "sSATA / PCIe Combo Port 4";
                    //help:
                    // Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 10. Alternately,
                    // this selection can be made by the value of GPP_H22 (Record 15 bit [20] and Record 19 bit [4]).
                    // Ensure that the value for this strap is the same as Record 15 bit [9:8] and Record 16 bit
  {TypeOpalCityFPGA, 85, 8, 2, 0x2}, // "SMS1 PMC SMBus Connect";
                    //help:
  {TypeOpalCityFPGA, 92, 8, 1, 0x1}, // "SLP_S5# / GPD10 Signal Configuration";
                    //help:
                    //GPD10_SLP_S5_N pin, select between SLP_S5# native function and GPIO GPD_10
  {TypeOpalCityFPGA, 92, 2, 1, 0x1}, // "SLP_A# / GPD6 Signal Configuration";
                    //help:
                    //GPD6_SLP_A_N pin, select between SLP_A# native function and GPIO GPD_6
  {TypeOpalCityFPGA, 96, 3, 1, 0x1}, // "SMT3 connected to the pins enable";
                    //help:
  {TypeOpalCityFPGA, 96, 2, 1, 0x1}, // "SMT2 connected to the pins enable";
                    //help:
  {TypeOpalCityFPGA, 96, 1, 1, 0x1}, // "SMT1 connected to the pins enable";
                    //help:
  {TypeOpalCityFPGA, 97, 0, 1, 0x0}, // "TPM Over SPI Bus Enabled";
                    //help: TPM on SPI provided SPI TPM disable fuse = 0 and ME FTPM enable = 0
  {TypeOpalCityFPGA, 104, 0, 2, 0x2}, // "GbE SMLINK3 SMBus Frequency";
                    //help:
                    //The value of these bits determine the physical bus speed supported by the SMLink3 HW
  {TypeOpalCityFPGA, 106, 10, 1, 0x1}, // "PCIe Controller 1 Lane Reversal Enabled";
                    //help:
                    //PCIe Lane Reversal is only supported if all 4 ports are configured in x4 mode.
  {TypeOpalCityFPGA, 108, 11, 2, 0x3}, // "PCIe Controller 2 (Port 4-7)";
                    //help:
                    //Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
  {TypeOpalCityFPGA, 110, 11, 2, 0x0}, // "PCIe Controller 3 (Port 8-11)";
                    //help:
                    //Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
  {TypeOpalCityFPGA, 119, 20, 1, 0x0}, // "SMS2CON - Shared with SMT";
                    //help:
                    //0x3;SMT3 Bus
  {TypeOpalCityFPGA, 123, 0, 2, 0x3}, // "SMS2CON - Shared with SMT";
                    //help:
                    //0x3;SMT3 Bus

  ///
  /// DVP  platform (Board ID=0x03 - TypePurleyLBGEPDVP)
  ///
  {TypePurleyLBGEPDVP, 15, 8, 2, 0x00}, // "sSATA / PCIe Select for Port 4 (SATA_PCIE_SP4)" set to: "Statically assigned to SSATA Port 4";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 10. Alternately, this selection can be made by the value of GPP_H22 (Record 15 bit [20] and Record 19 bit [4]).Ensure that the value for this strap is the same as Record 16 bit [9:8] and Record 18 bit [3:2]. Not doing so may cause boot or functional issues on the platform.
  {TypePurleyLBGEPDVP, 15, 20, 1, 0x00}, // "Polarity of GPP_H23 (GPIO polarity of Select between sSATA Port 5 and PCIe Port 11)" set to: "When GPIO pin is '0', sSATA mode is selected, when GPIO pin is '1', PCIe mode is selected ";
                    //help:
                    //This strap must also be configured if Record 15 bit [11:10] and Record 18 bit [5:4] are configured to '0x3' and Record 16 bit [11:10] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [5]. Not doing so may cause boot or functional issues on the platform.
  {TypePurleyLBGEPDVP, 16, 8, 2, 0x00}, // "sSATA / PCIe GP Select for Port 4 (SATA_PCIE_GP4)" set to: "Statically assigned to SSATA Port 4";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 5 or PCIe Port 11 (or GbE). Alternately, this selection can be made by the value of GPP_H23 (Record 15 bit [21] and Record 19 bit [5]). Ensure that the values for this strap is the same as Record 15 bit [11:10] and Record 18 bit [5:4]. Not doing so may cause boot or funticonal issues on the platform.
  {TypePurleyLBGEPDVP, 18, 2, 2, 0x00}, // "sSATA / PCIe Combo Port 5" set to: "Statically assigned to SATA";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 5 or PCIe Port 11 (or GbE). Alternately, this selection can be made by the value of GPP_H23 (Record 15 bit [21] and Record 19 bit [5]). Ensure that the value for this strap is the same as Record 15 bit [11:10] and Record 16 bit [11:10]. Not doing so may cause boot or functional issues on the platform.
  {TypePurleyLBGEPDVP, 19, 4, 1, 0x00}, // "COMBO0PCIE_COMBO1SATA";
                    //help:
                    //This strap must also be configured if Record 15 bit [3:2] and Record 17 bit [11:10] are configured to '0x3' and Record 16 bit [3:2] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 15 bit [17]. Not doing so may cause boot or functional issues on the platform.
  {TypePurleyLBGEPDVP, 28, 24, 7, 0x2C}, // "SMLink0 I2C Target Address" set to: "0x2C";
                    //help:
                    //Specify 7-bit target address for I2C writes from device to PCH, if this capability is present. This address must not conflict with another device on the bus.

  ///
  /// Lignhtning Ridge 4S RP platform (Board ID=0x10 - TypeLightningRidgeEXRP)
  ///
  {TypeLightningRidgeEXRP, 2, 7, 1, 0x01}, // "MROM1 Enabled" set to: "MROM1 Disabled for not support IE";
                    //help:
                    //MROM contains BIOS code that will setup and enable many of the platform management features.
                    //Disable if not use IE.
  {TypeLightningRidgeEXRP, 3, 30, 1, 0x00}, // "Intel QuickAssist Endpoint 2 (EP[2]) Primary Mux Select" set to: "PCI hierarchy configuration 2, QAT EP[2]is connected to Uplink x16, uplink x8 is disabled";
                    //help:
                    //This strap selects between configuration 1 and configuration 2.
                    //LBG -1G/-2/-4/-E apply to config 2, uplink x8 is disabled
                    //LBG -M/T/LP apply to config 1, uplink x8 is not disabled
                    //
                    //
  {TypeLightningRidgeEXRP, 15, 0, 2, 0x01}, // "sSATA / PCIe Select for Port 0 (SATA_PCIE_SP0)" set to: "Statically assigned to PCIe Port 6";
                    //help:
                    //Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6. Alternately, this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]). Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit [9:8]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 15, 2, 2, 0x03}, // "sSATA / PCIe Select for Port 1 (SATA_PCIE_SP1)" set to: "Assigned based on sSATA Port 1 GPIO pin(GPP_H19) and polarity softstrap";
                    //help:
                    //Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7. Alternately, this selection can be made by the value of GPP_H19 (Record 15 bit [17] and Record 19 bit [1]). Ensure that the value for this strap is the same as Record 16 bit [3:2] and Record 17 bit [11:10]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 15, 4, 2, 0x00}, // "sSATA / PCIe Select for Port 2 (SATA_PCIE_SP2)" set to: "Statically assigned to SSATA Port 2";
                    //help:
                    //Choose to statically assign Flex IO Port 15 to sSATA Port 2 or PCIe Port 8 (or GbE). Alternately, this selection can be made by the value of GPP_H20 (Record 15 bit [18] and Record 19 bit [2]).Ensure that the value for this strap is the same as Record 16 bit [5:4] and Record 17 bit [13:12]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 15, 6, 2, 0x00}, // "sSATA / PCIe Select for Port 3 (SATA_PCIE_SP3)" set to: "Statically assigned to SSATA Port 3";
                    //help:
                    //Choose to statically assign Flex IO Port 16 to sSATA Port 3 or PCIe Port 9. Alternately, this selection can be made by the value of GPP_H21 (Record 15 bit [19] and Record 19 bit [3]).Ensure that the value for this strap is the same as Record 16 bit [7:6] and Record 18 bit [1:0]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 15, 8, 2, 0x00}, // "sSATA / PCIe Select for Port 4 (SATA_PCIE_SP4)" set to: "Statically assigned to SSATA Port 4";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 10. Alternately, this selection can be made by the value of GPP_H22 (Record 15 bit [20] and Record 19 bit [4]).Ensure that the value for this strap is the same as Record 16 bit [9:8] and Record 18 bit [3:2]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 15, 10, 2, 0x01}, // "sSATA / PCIe Select for Port 5 (SATA_PCIE_SP5)" set to: "Statically assigned to PCIe Port 11";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 5 or PCIe Port 11. Alternately, this selection can be made by the value of GPP_H23 (Record 15 bit [21] and Record 19 bit [5]).Ensure that the value for this strap is the same as Record 16 bit [11:10] and Record 18 bit [5:4]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 15, 17, 1, 0x01}, // "Polarity of GPP_H19 (GPIO polarity of Select between sSATA Port 1 and PCIe Port 7)" set to: "When GPIO pin is ‘0’, sSATA mode is selected, when GPIO pin is ‘1’, PCIe mode is selected";
                    //help:
                    //This strap must also be configured if Record 15 bit [3:2] and Record 17 bit [11:10] are configured to 0x3 and Record 16 bit [3:2] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [1]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 15, 19, 1, 0x00}, // "Polarity of GPP_H21 (GPIO polarity of Select between sSATA Port 3 and PCIe Port 9)" set to: "When GPIO pin is ‘0’, PCIe mode is selected, when GPIO pin is ‘1’, sSATA mode is selected. ";
                    //help:
                    //This strap must also be configured if Record 15 bit [7:6] and Record 18 bit [1:0] are configured to ‘0x3’ and Record 16 bit [7:6] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [3]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 15, 20, 1, 0x00}, // "Polarity of GPP_H23 (GPIO polarity of Select between sSATA Port 5 and PCIe Port 11)" set to: "When GPIO pin is '0', sSATA mode is selected, when GPIO pin is '1', PCIe mode is selected ";
                    //help:
                    //This strap must also be configured if Record 15 bit [11:10] and Record 18 bit [5:4] are configured to 0x3 and Record 16 bit [11:10] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [5]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 16, 0, 2, 0x01},  // sSATA / PCIe GP Select for Port 0 (SATA_PCIE_GP0)
                    // Choose to statically assign Flex IO Port 12 to sSATA Port 0 or PCIe Port 6. Alternately, this selection can be made by the value of GPP_G23 (Soft strap 15 bit [16] and Soft strap 19 bit [0]). Ensure that the value for this strap is the same as Soft strap 15 bit [1:0] and Soft strap 17 bit [9:8]. Not doing so may cause boot or functional issues on the platform.
                    // 0x0;Statically assigned to SSATA Port 0
                    // 0x1;Statically assigned to PCIe Port 6
                    // 0x3;Assigned based on GPIO pin (GPP_G23) and polarity softstrap
  {TypeLightningRidgeEXRP, 16, 2, 2, 0x03},  // "sSATA / PCIe GP Select for Port 1 (SATA_PCIE_GP1)" set to: " 0x3;Assigned based on GPIO pin(GPP_H19) and polarity softstrap"
                    //help:
                    //Choose to statically assign Flex IO Port 13 to sSATA Port 1 or PCIe Port 7. Alternately, this selection can be made by the value of GPP_H19 (Soft strap 15 bit [17] and Soft strap 19 bit [1]). Ensure that the value for this strap is the same as Soft strap 15 bit [3:2] and Soft strap 17 bit [11:10]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 16, 4, 2, 0x00},  // sSATA / PCIe GP Select for Port 2 (SATA_PCIE_GP2)
                    // Choose to statically assign Flex IO Port 14 to sSATA Port 2 or PCIe Port 8 (or GbE). Alternately, this selection can be made by the value of GPP_H20 (Soft strap 15 bit [18] and Soft strap 19 bit [2]). Ensure that the values for this strap is the same as Soft strap 15 bit [5:4] and Soft strap 17 bit [13:12]. Not doing so may cause boot or funticonal issues on the platform.
                    // 0x0;Statically assigned to SSATA Port 2
                    // 0x1;Statically assigned to PCIe Port 8 (GbE)
                    // 0x3;Assigned based on GPIO pin(GPP_H20) and polarity softstrap
  {TypeLightningRidgeEXRP, 16, 6, 2, 0x00}, // sSATA / PCIe GP Select for Port 3 (SATA_PCIE_GP3)
                    // Choose to statically assign Flex IO Port 15 to sSATA Port 3 or PCIe Port 9. Alternately, this selection can be made by the value of GPP_H21 (Soft strap 15 bit [19] and Soft strap 19 bit [3]). Ensure that the values for this strap is the same as Soft strap 15 bit [7:6] and Soft strap 18 bit [1:0]. Not doing so may cause boot or funticonal issues on the platform
                    // 0x0;Statically assigned to SSATA Port 3
                    // 0x1;Statically assigned to PCIe Port 9
                    // 0x3;Assigned based on sSATA Port 3 GPIO pin(GPP_H21) and polarity softstrap.
  {TypeLightningRidgeEXRP, 16, 8, 2, 0x00}, // "sSATA / PCIe GP Select for Port 4 (SATA_PCIE_GP4)" set to: "Statically assigned to SSATA Port 4";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 5 or PCIe Port 11 (or GbE). Alternately, this selection can be made by the value of GPP_H23 (Record 15 bit [21] and Record 19 bit [5]). Ensure that the values for this strap is the same as Record 15 bit [11:10] and Record 18 bit [5:4]. Not doing so may cause boot or funticonal issues on the platform.
  {TypeLightningRidgeEXRP, 16, 10, 2, 0x01},  // "sSATA / PCIe GP Select for Port 5 (SATA_PCIE_GP5)" set to "0x1;Statically assigned to PCIe Port 11 (or GbE)"
					          //Choose to statically assign Flex IO Port 17 to sSATA Port 5 or PCIe Port 11 (or GbE). Alternately, this selection can be made by the value of GPP_H23 (Soft strap 15 bit [21] and Soft strap 19 bit [5]). Ensure that the values for this strap is the same as Soft strap 15 bit [11:10] and Soft strap 18 bit [5:4]. Not doing so may cause boot or funticonal issues on the platform
  {TypeLightningRidgeEXRP, 17, 6, 1, 0x00}, // "Intel (R) GbE Legacy PHY over PCIe Enabled" set to: "GbE MAC/PHY port communication is Enabled over PCI Express";
                    //help:
                    //Enable or disable GbE Legacy MAC/external PHY communication over PCI Express. Required to be '1' when using the GbE Legacy Controller.
  {TypeLightningRidgeEXRP, 17, 8, 2, 0x01}, // "sSATA / PCIe Combo Port 0" set to: "Statically assigned to PCIe Port 6";
                    //help:
                    //Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6. Alternately, this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]). Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 16 bit [1:0]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 17, 10, 2, 0x03}, // "sSATA / PCIe Combo Port 1" set to: "Assigned based on GPIO pin (GPP_H19) and polarity softstrap";
                    //help:
                    //Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7. Alternately, this selection can be made by the value of GPP_H19 (Record 15 bit [17] and Record 19 bit [1]). Ensure that the value for this strap is the same as Record 15 bit [3:2] and Record 16 bit [3:2]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 17, 12, 2, 0x00}, // "sSATA / PCIe Combo Port 2" set to: "Statically assigned to SATA";
                    //help:
                    //Choose to statically assign Flex IO Port 15 to sSATA Port 2 or PCIe Port 8 (or GbE). Alternately, this selection can be made by the value of GPP_H20 (Record 15 bit [18] and Record 19 bit [2]).Ensure that the value for this strap is the same as Record 15 bit [7:6] and Record 16 bit [5:4]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 18, 0, 2, 0x00}, // "sSATA / PCIe Combo Port 3" set to: "Statically assigned to sSATA Port 3";
                    //help:
                    //Choose to statically assign Flex IO Port 16 to sSATA Port 3 or PCIe Port 9. Alternately, this selection can be made by the value of GPP_H21 (Record 15 bit [19] and Record 19 bit [3]). Ensure that the value for this strap is the same as Record 15 bit [7:6] and Record 16 bit [7:6]. Not doing so may cause boot or functional issues on the platform.
                    //
  {TypeLightningRidgeEXRP, 18, 2, 2, 0x00}, // sSATA / PCIe Combo Port 4
                    // Choose to statically assign Flex IO Port 16 to sSATA Port 4 or PCIe Port 10. Alternately, this selection can be made by the value of GPP_H22 (Soft strap 15 bit [20] and Soft strap 19 bit [4]). Ensure that the value of soft strap 18 bit [3:2] is the same as Soft strap 15 bit [9:8] and Soft strap 16 bit [9:8]. Not doing so may cause boot or functional issues on the platform.
                    // 0x0;Statically assigned to sSATA Port 4
                    // 0x1;Statically assigned to PCIe Port 10
                    // 0x3;Assigned based on GPIO pin (GPP_H22) and polarity softstrap                    
  {TypeLightningRidgeEXRP, 18, 4, 2, 0x01}, // "sSATA / PCIe Combo Port 5" set to: "Statically assigned to PCIe Port 11 (or GbE)";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 5 or PCIe Port 11 (or GbE). Alternately, this selection can be made by the value of GPP_H23 (Record 15 bit [21] and Record 19 bit [5]). Ensure that the value for this strap is the same as Record 15 bit [11:10] and Record 16 bit [11:10]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 19, 1, 1, 0x01}, //  Polarity Select sSATA / PCIe Combo Port 1
                    // This strap must also be configured if Soft strap 15 bit [3:2] and Soft strap 17 bit [11:10] and Soft strap 16 bit [3:2] are configured to '0x3'.
                    // When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value of soft strap 19 bit [1] is the same as Soft strap 15 bit [17]. Not doing so may cause boot or functional issues on the platform.
                    // 0x0;When GPIO pin is '0', PCIe mode is selected, when GPIO pin is '1', sSATA mode is selected. 
                    // 0x1;When GPIO pin is '0', sSATA mode is selected, when GPIO pin is '1', PCIe mode is selected
  {TypeLightningRidgeEXRP, 19, 4, 1, 0x00}, // "COMBO0PCIE_COMBO1SATA";
                    //help:
                    //This strap must also be configured if Record 15 bit [3:2] and Record 17 bit [11:10] are configured to 0x3 and Record 16 bit [3:2] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 15 bit [17]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXRP, 85, 8, 3, 0x00}, // "SMS Connect (SMSCON) Configuration; Indicates what SMT associated physical bus SMS is connected to."
                    // 000 : SMS not connected
  {TypeLightningRidgeEXRP, 92, 2, 1, 0x01}, // "SLP_A# / GPD6 Signal Configuration" set to: "Use as GPD_6";
                    //help:
                    //GPD6_SLP_A_N pin, select between SLP_A# native function and GPIO GPD_6
  {TypeLightningRidgeEXRP, 103, 16, 3, 0x00}, // "GbE Legacy PHY Smbus Connection" set to: "No PHY connected";
                    //help:
                    //Select which SMBus network is used to connect GbE Legacy PHY to MAC/PCH.
  {TypeLightningRidgeEXRP, 108, 10, 1, 0x01}, // "PCIe Controller 2 Lane Reversal Enabled" set to: "PCIe Lanes are reversed";
                    //help:
                    //PCIe Lane Reversal is only supported if all 4 ports are configured in x4 mode.
  {TypeLightningRidgeEXRP, 108, 11, 2, 0x03}, // "PCIe Controller 2 (Port 4-7)" set to: "Port 4 (x4), Ports 5-7 (disabled)";
                    //help:
                    //Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
  {TypeLightningRidgeEXRP, 110, 10, 1, 0x00}, // "PCIe Controller 3 Lane Reversal Enabled" set to: "PCIe Lanes are not reversed";
                    //help:
                    //PCIe Lane Reversal is only supported if all 4 ports are configured in x4 mode.
  {TypeLightningRidgeEXRP, 110, 11, 2, 0x00}, // "PCIe Controller 3 (Port 8-11)" set to: "4x1, Ports 8-11 (x1)";
                    //help:
                    //Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
  {TypeLightningRidgeEXRP, 110, 28, 1, 0x00}, // "PCIe Controller 3 Port 1 SRIS Enabled" set to: "Disabled";
                    //help:
                    //Enable or disable SRIS on this PCIe Port.
  {TypeLightningRidgeEXRP, 110, 29, 1, 0x00}, // "PCIe Controller 3 Port 2 SRIS Enabled" set to: "Disabled";
                    //help:
                    //Enable or disable SRIS on this PCIe Port.
  {TypeLightningRidgeEXRP, 119, 6, 1, 0x00}, // "Aux Well Power Policy" set to: "Power down the AUX well prior to MBB_RST# deassertion";
                    //help:
                    //Set to '1' if GbE Legacy MAC/PHY is enabled, otherwise set to '0'.
  {TypeLightningRidgeEXRP, 119, 14, 1, 0x01} , // GbE enable/disable set to: GbE disabled
                    // help:
                    // Indicates that internal GbE controller is disabled. If set to '1' the controller is disabled.
  {TypeLightningRidgeEXRP, 119, 20, 1, 0x00}, // "DeepSx Platform Configuration (DEEPSX_PLT_CFG_SS)" set to: "The platform does not support DeepSx";
                    //help:
                    //Indicates if the platform supports Deep Sx
  {TypeLightningRidgeEXRP, 133, 23, 1, 0x00}, // "SPI Voltage Select" set to: "The VCCSPI supply is 3.3 Volt";
                    //help:
                    //Note:
                    //The strap defaults to 1.8V mode before the softstraps are loaded, i.e. before the actual supply voltage is known. This is because the pad performance is slightly better when assuming 1.8V when the actual is 3.3 than vice-versa.
                    //
  { TypeLightningRidgeEXRP, 136, 3, 3, 0x00 }, //For Slave 0 (EC/BMC): Indicates the maximum frequency of the eSPI bus that is supported by the eSPI Master and platform configuration (trace length, number of Slaves, etc.). The actual frequency of the eSPI bus will be the minimum of this field and the Slave's maximum frequency advertised in its General Capabilities register
                    //PchStrap136eSPIECBusFrequency = 0 // 20MHz
  { TypeLightningRidgeEXRP, 137, 22, 1, 0x00 }, //Not for production use; only intended for use in Engineering Samples (ES). Should be marked as Reserved with '0' values. eSPI bus low frequency (div-by-8) mode is set as follows based on fuse:espi_freq_divby8_en, soft-strap:espi_freq_divby8_ovrd:
                    //PchStrap137eSPIbuslowfrequencydivIderby8modeoverride = 0 //eSPI Low Freq Debug Mode Enabled

  ///
  /// Lightning Ridge 4S ECB platform (Board ID=0x12 - TypeLightningRidgeEXECB1)
  ///
  {TypeLightningRidgeEXECB1, 2, 7, 1, 0x01}, // "MROM1 Enabled" set to: "MROM1 Disabled for not support IE";
                    //help:
                    //MROM contains BIOS code that will setup and enable many of the platform management features.
                    //Disable if not use IE.
  {TypeLightningRidgeEXECB1, 3, 30, 1, 0x00}, // "Intel QuickAssist Endpoint 2 (EP[2]) Primary Mux Select" set to: "PCI hierarchy configuration 2, QAT EP[2]is connected to Uplink x16, uplink x8 is disabled";
                    //help:
                    //This strap selects between configuration 1 and configuration 2.
                    //LBG -1G/-2/-4/-E apply to config 2, uplink x8 is disabled
                    //LBG -M/T/LP apply to config 1, uplink x8 is not disabled
                    //
                    //
  {TypeLightningRidgeEXECB1, 15, 0, 2, 0x01}, // "sSATA / PCIe Select for Port 0 (SATA_PCIE_SP0)" set to: "Statically assigned to PCIe Port 6";
                    //help:
                    //Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6. Alternately, this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]). Ensure that the value for this strap is the same as Record 16 bit [1:0] and Record 17 bit [9:8]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 15, 2, 2, 0x03}, // "sSATA / PCIe Select for Port 1 (SATA_PCIE_SP1)" set to: "Assigned based on sSATA Port 1 GPIO pin(GPP_H19) and polarity softstrap";
                    //help:
                    //Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7. Alternately, this selection can be made by the value of GPP_H19 (Record 15 bit [17] and Record 19 bit [1]). Ensure that the value for this strap is the same as Record 16 bit [3:2] and Record 17 bit [11:10]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 15, 4, 2, 0x00}, // "sSATA / PCIe Select for Port 2 (SATA_PCIE_SP2)" set to: "Statically assigned to SSATA Port 2";
                    //help:
                    //Choose to statically assign Flex IO Port 15 to sSATA Port 2 or PCIe Port 8 (or GbE). Alternately, this selection can be made by the value of GPP_H20 (Record 15 bit [18] and Record 19 bit [2]).Ensure that the value for this strap is the same as Record 16 bit [5:4] and Record 17 bit [13:12]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 15, 6, 2, 0x00}, // "sSATA / PCIe Select for Port 3 (SATA_PCIE_SP3)" set to: "Statically assigned to SSATA Port 3";
                    //help:
                    //Choose to statically assign Flex IO Port 16 to sSATA Port 3 or PCIe Port 9. Alternately, this selection can be made by the value of GPP_H21 (Record 15 bit [19] and Record 19 bit [3]).Ensure that the value for this strap is the same as Record 16 bit [7:6] and Record 18 bit [1:0]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 15, 8, 2, 0x00}, // "sSATA / PCIe Select for Port 4 (SATA_PCIE_SP4)" set to: "Statically assigned to SSATA Port 4";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 4 or PCIe Port 10. Alternately, this selection can be made by the value of GPP_H22 (Record 15 bit [20] and Record 19 bit [4]).Ensure that the value for this strap is the same as Record 16 bit [9:8] and Record 18 bit [3:2]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 15, 10, 2, 0x01}, // "sSATA / PCIe Select for Port 5 (SATA_PCIE_SP5)" set to: "Statically assigned to PCIe Port 11";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 5 or PCIe Port 11. Alternately, this selection can be made by the value of GPP_H23 (Record 15 bit [21] and Record 19 bit [5]).Ensure that the value for this strap is the same as Record 16 bit [11:10] and Record 18 bit [5:4]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 15, 17, 1, 0x01}, // "Polarity of GPP_H19 (GPIO polarity of Select between sSATA Port 1 and PCIe Port 7)" set to: "When GPIO pin is ‘0’, sSATA mode is selected, when GPIO pin is ‘1’, PCIe mode is selected";
                    //help:
                    //This strap must also be configured if Record 15 bit [3:2] and Record 17 bit [11:10] are configured to ‘0x3’ and Record 16 bit [3:2] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [1]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 15, 19, 1, 0x00}, // "Polarity of GPP_H21 (GPIO polarity of Select between sSATA Port 3 and PCIe Port 9)" set to: "When GPIO pin is ‘0’, PCIe mode is selected, when GPIO pin is ‘1’, sSATA mode is selected. ";
                    //help:
                    //This strap must also be configured if Record 15 bit [7:6] and Record 18 bit [1:0] are configured to ‘0x3’ and Record 16 bit [7:6] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [3]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 15, 20, 1, 0x00}, // "Polarity of GPP_H23 (GPIO polarity of Select between sSATA Port 5 and PCIe Port 11)" set to: "When GPIO pin is '0', sSATA mode is selected, when GPIO pin is '1', PCIe mode is selected ";
                    //help:
                    //This strap must also be configured if Record 15 bit [11:10] and Record 18 bit [5:4] are configured to ‘0x3’ and Record 16 bit [11:10] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 19 bit [5]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 16, 0, 2, 0x01},  // sSATA / PCIe GP Select for Port 0 (SATA_PCIE_GP0)
                    // Choose to statically assign Flex IO Port 12 to sSATA Port 0 or PCIe Port 6. Alternately, this selection can be made by the value of GPP_G23 (Soft strap 15 bit [16] and Soft strap 19 bit [0]). Ensure that the value for this strap is the same as Soft strap 15 bit [1:0] and Soft strap 17 bit [9:8]. Not doing so may cause boot or functional issues on the platform.
                    // 0x0;Statically assigned to SSATA Port 0
                    // 0x1;Statically assigned to PCIe Port 6
                    // 0x3;Assigned based on GPIO pin (GPP_G23) and polarity softstrap
  {TypeLightningRidgeEXECB1, 16, 2, 2, 0x03},  // "sSATA / PCIe GP Select for Port 1 (SATA_PCIE_GP1)" set to: " 0x3;Assigned based on GPIO pin(GPP_H19) and polarity softstrap"
                    //help:
                    //Choose to statically assign Flex IO Port 13 to sSATA Port 1 or PCIe Port 7. Alternately, this selection can be made by the value of GPP_H19 (Soft strap 15 bit [17] and Soft strap 19 bit [1]). Ensure that the value for this strap is the same as Soft strap 15 bit [3:2] and Soft strap 17 bit [11:10]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 16, 4, 2, 0x00},  // sSATA / PCIe GP Select for Port 2 (SATA_PCIE_GP2)
                    // Choose to statically assign Flex IO Port 14 to sSATA Port 2 or PCIe Port 8 (or GbE). Alternately, this selection can be made by the value of GPP_H20 (Soft strap 15 bit [18] and Soft strap 19 bit [2]). Ensure that the values for this strap is the same as Soft strap 15 bit [5:4] and Soft strap 17 bit [13:12]. Not doing so may cause boot or funticonal issues on the platform.
                    // 0x0;Statically assigned to SSATA Port 2
                    // 0x1;Statically assigned to PCIe Port 8 (GbE)
                    // 0x3;Assigned based on GPIO pin(GPP_H20) and polarity softstrap
  {TypeLightningRidgeEXECB1, 16, 6, 2, 0x00}, // sSATA / PCIe GP Select for Port 3 (SATA_PCIE_GP3)
                    // Choose to statically assign Flex IO Port 15 to sSATA Port 3 or PCIe Port 9. Alternately, this selection can be made by the value of GPP_H21 (Soft strap 15 bit [19] and Soft strap 19 bit [3]). Ensure that the values for this strap is the same as Soft strap 15 bit [7:6] and Soft strap 18 bit [1:0]. Not doing so may cause boot or funticonal issues on the platform
                    // 0x0;Statically assigned to SSATA Port 3
                    // 0x1;Statically assigned to PCIe Port 9
                    // 0x3;Assigned based on sSATA Port 3 GPIO pin(GPP_H21) and polarity softstrap.
  {TypeLightningRidgeEXECB1, 16, 8, 2, 0x00}, // "sSATA / PCIe GP Select for Port 4 (SATA_PCIE_GP4)" set to: "Statically assigned to SSATA Port 4";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 5 or PCIe Port 11 (or GbE). Alternately, this selection can be made by the value of GPP_H23 (Record 15 bit [21] and Record 19 bit [5]). Ensure that the values for this strap is the same as Record 15 bit [11:10] and Record 18 bit [5:4]. Not doing so may cause boot or funticonal issues on the platform.
  {TypeLightningRidgeEXECB1, 16, 10, 2, 0x01},  // "sSATA / PCIe GP Select for Port 5 (SATA_PCIE_GP5)" set to "0x1;Statically assigned to PCIe Port 11 (or GbE)"
					          //Choose to statically assign Flex IO Port 17 to sSATA Port 5 or PCIe Port 11 (or GbE). Alternately, this selection can be made by the value of GPP_H23 (Soft strap 15 bit [21] and Soft strap 19 bit [5]). Ensure that the values for this strap is the same as Soft strap 15 bit [11:10] and Soft strap 18 bit [5:4]. Not doing so may cause boot or funticonal issues on the platform
  {TypeLightningRidgeEXECB1, 17, 6, 1, 0x00}, // "Intel (R) GbE Legacy PHY over PCIe Enabled" set to: "GbE MAC/PHY port communication is Enabled over PCI Express";
                    //help:
                    //Enable or disable GbE Legacy MAC/external PHY communication over PCI Express. Required to be '1' when using the GbE Legacy Controller.
  {TypeLightningRidgeEXECB1, 17, 8, 2, 0x01}, // "sSATA / PCIe Combo Port 0" set to: "Statically assigned to PCIe Port 6";
                    //help:
                    //Choose to statically assign Flex IO Port 13 to sSATA Port 0 or PCIe Port 6. Alternately, this selection can be made by the value of GPP_G23 (Record 15 bit [16] and Record 19 bit [0]). Ensure that the value for this strap is the same as Record 15 bit [1:0] and Record 16 bit [1:0]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 17, 10, 2, 0x03}, // "sSATA / PCIe Combo Port 1" set to: "Assigned based on GPIO pin (GPP_H19) and polarity softstrap";
                    //help:
                    //Choose to statically assign Flex IO Port 14 to sSATA Port 1 or PCIe Port 7. Alternately, this selection can be made by the value of GPP_H19 (Record 15 bit [17] and Record 19 bit [1]). Ensure that the value for this strap is the same as Record 15 bit [3:2] and Record 16 bit [3:2]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 17, 12, 2, 0x00}, // "sSATA / PCIe Combo Port 2" set to: "Statically assigned to SATA";
                    //help:
                    //Choose to statically assign Flex IO Port 15 to sSATA Port 2 or PCIe Port 8 (or GbE). Alternately, this selection can be made by the value of GPP_H20 (Record 15 bit [18] and Record 19 bit [2]).Ensure that the value for this strap is the same as Record 15 bit [7:6] and Record 16 bit [5:4]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 18, 0, 2, 0x00}, // "sSATA / PCIe Combo Port 3" set to: "Statically assigned to sSATA Port 3";
                    //help:
                    //Choose to statically assign Flex IO Port 16 to sSATA Port 3 or PCIe Port 9. Alternately, this selection can be made by the value of GPP_H21 (Record 15 bit [19] and Record 19 bit [3]). Ensure that the value for this strap is the same as Record 15 bit [7:6] and Record 16 bit [7:6]. Not doing so may cause boot or functional issues on the platform.
                    //
  {TypeLightningRidgeEXECB1, 18, 2, 2, 0x00}, // sSATA / PCIe Combo Port 4
                    // Choose to statically assign Flex IO Port 16 to sSATA Port 4 or PCIe Port 10. Alternately, this selection can be made by the value of GPP_H22 (Soft strap 15 bit [20] and Soft strap 19 bit [4]). Ensure that the value of soft strap 18 bit [3:2] is the same as Soft strap 15 bit [9:8] and Soft strap 16 bit [9:8]. Not doing so may cause boot or functional issues on the platform.
                    // 0x0;Statically assigned to sSATA Port 4
                    // 0x1;Statically assigned to PCIe Port 10
                    // 0x3;Assigned based on GPIO pin (GPP_H22) and polarity softstrap                    
  {TypeLightningRidgeEXECB1, 18, 4, 2, 0x01}, // "sSATA / PCIe Combo Port 5" set to: "Statically assigned to PCIe Port 11 (or GbE)";
                    //help:
                    //Choose to statically assign Flex IO Port 17 to sSATA Port 5 or PCIe Port 11 (or GbE). Alternately, this selection can be made by the value of GPP_H23 (Record 15 bit [21] and Record 19 bit [5]). Ensure that the value for this strap is the same as Record 15 bit [11:10] and Record 16 bit [11:10]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 19, 1, 1, 0x01}, //  Polarity Select sSATA / PCIe Combo Port 1
                    // This strap must also be configured if Soft strap 15 bit [3:2] and Soft strap 17 bit [11:10] and Soft strap 16 bit [3:2] are configured to '0x3'.
                    // When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value of soft strap 19 bit [1] is the same as Soft strap 15 bit [17]. Not doing so may cause boot or functional issues on the platform.
                    // 0x0;When GPIO pin is '0', PCIe mode is selected, when GPIO pin is '1', sSATA mode is selected. 
                    // 0x1;When GPIO pin is '0', sSATA mode is selected, when GPIO pin is '1', PCIe mode is selected
  {TypeLightningRidgeEXECB1, 19, 4, 1, 0x00}, // "COMBO0PCIE_COMBO1SATA";
                    //help:
                    //This strap must also be configured if Record 15 bit [3:2] and Record 17 bit [11:10] are configured to ذx3 and Record 16 bit [3:2] is configured to '0x2'.
                    //When this parameter is '0', then PCIe when GPIO is LOW and sSATA when HIGH. A setting of '1' reverses the polarity.  Ensure that the value for this strap is the same as Record 15 bit [17]. Not doing so may cause boot or functional issues on the platform.
  {TypeLightningRidgeEXECB1, 85, 8, 3, 0x00}, // "SMS Connect (SMSCON) Configuration; Indicates what SMT associated physical bus SMS is connected to."
                    // 000 : SMS not connected
  {TypeLightningRidgeEXECB1, 92, 2, 1, 0x01}, // "SLP_A# / GPD6 Signal Configuration" set to: "Use as GPD_6";
                    //help:
                    //GPD6_SLP_A_N pin, select between SLP_A# native function and GPIO GPD_6
  {TypeLightningRidgeEXECB1, 103, 16, 3, 0x00}, // "GbE Legacy PHY Smbus Connection" set to: "No PHY connected";
                    //help:
                    //Select which SMBus network is used to connect GbE Legacy PHY to MAC/PCH.
  {TypeLightningRidgeEXECB1, 108, 10, 1, 0x01}, // "PCIe Controller 2 Lane Reversal Enabled" set to: "PCIe Lanes are reversed";
                    //help:
                    //PCIe Lane Reversal is only supported if all 4 ports are configured in x4 mode.
  {TypeLightningRidgeEXECB1, 108, 11, 2, 0x03}, // "PCIe Controller 2 (Port 4-7)" set to: "Port 4 (x4), Ports 5-7 (disabled)";
                    //help:
                    //Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
  {TypeLightningRidgeEXECB1, 110, 10, 1, 0x00}, // "PCIe Controller 3 Lane Reversal Enabled" set to: "PCIe Lanes are not reversed";
                    //help:
                    //PCIe Lane Reversal is only supported if all 4 ports are configured in x4 mode.
  {TypeLightningRidgeEXECB1, 110, 11, 2, 0x00}, // "PCIe Controller 3 (Port 8-11)" set to: "4x1, Ports 8-11 (x1)";
                    //help:
                    //Choose between 1x4, 1x2/2x1, 2x2, or 4x1 configurations.
  {TypeLightningRidgeEXECB1, 110, 28, 1, 0x00}, // "PCIe Controller 3 Port 1 SRIS Enabled" set to: "Disabled";
                    //help:
                    //Enable or disable SRIS on this PCIe Port.
  {TypeLightningRidgeEXECB1, 110, 29, 1, 0x00}, // "PCIe Controller 3 Port 2 SRIS Enabled" set to: "Disabled";
                    //help:
                    //Enable or disable SRIS on this PCIe Port.
  {TypeLightningRidgeEXECB1, 119, 6, 1, 0x00}, // "Aux Well Power Policy" set to: "Power down the AUX well prior to MBB_RST# deassertion";
                    //help:
                    //Set to '1' if GbE Legacy MAC/PHY is enabled, otherwise set to '0'.
  {TypeLightningRidgeEXECB1, 119, 14, 1, 0x01} , // GbE enable/disable set to: GbE disabled
                    // help:
                    // Indicates that internal GbE controller is disabled. If set to '1' the controller is disabled.
  {TypeLightningRidgeEXECB1, 119, 20, 1, 0x00}, // "DeepSx Platform Configuration (DEEPSX_PLT_CFG_SS)" set to: "The platform does not support DeepSx";
                    //help:
                    //Indicates if the platform supports Deep Sx
  {TypeLightningRidgeEXECB1, 133, 23, 1, 0x00}, // "SPI Voltage Select" set to: "The VCCSPI supply is 3.3 Volt";
                    //help:
                    //Note:
                    //The strap defaults to 1.8V mode before the softstraps are loaded, i.e. before the actual supply voltage is known. This is because the pad performance is slightly better when assuming 1.8V when the actual is 3.3 than vice-versa.
                    //
  { TypeLightningRidgeEXECB1, 136, 3, 3, 0x00 }, //For Slave 0 (EC/BMC): Indicates the maximum frequency of the eSPI bus that is supported by the eSPI Master and platform configuration (trace length, number of Slaves, etc.). The actual frequency of the eSPI bus will be the minimum of this field and the Slave's maximum frequency advertised in its General Capabilities register
                    //PchStrap136eSPIECBusFrequency = 0 // 20MHz
  { TypeLightningRidgeEXECB1, 137, 22, 1, 0x00 }, //Not for production use; only intended for use in Engineering Samples (ES). Should be marked as Reserved with '0' values. eSPI bus low frequency (div-by-8) mode is set as follows based on fuse:espi_freq_divby8_en, soft-strap:espi_freq_divby8_ovrd:
                    //PchStrap137eSPIbuslowfrequencydivIderby8modeoverride = 0 //eSPI Low Freq Debug Mode Enabled
                   
  ///
  /// Signal the end of softstrap fixup table
  ///
  {0xFF, 0, 0, 0}
};

STATIC SIO_INDEX_DATA mSioInitTable[] = {
  //
  // Init GPIO
  //
  {
    PILOTIV_LOGICAL_DEVICE,
    PILOTIV_SIO_GPIO
  },
  {
    PILOTIV_ACTIVATE,
    0x01
  },
  {
    PILOTIV_BASE_ADDRESS_HIGH0,
    (UINT8) ((UINT16) SIO_GPIO_BASE_ADDRESS >> 8)
  },
  {
    PILOTIV_BASE_ADDRESS_LOW0,
    (UINT8) (SIO_GPIO_BASE_ADDRESS & 0x00ff)
  }
};


//
// Platform system board information structure
//
static PEI_SYSTEM_BOARD_INFO SystemBoardInfo = {
  //
  // GPIO table
  //
  //mPchGpioInitData_Platform,               // mSystemGpioInitData - Gpio initialization data for Platform

  //
  // Pch softstrap fixup table.
  //
  SoftstrapFixupTable,                      // SoftstrapFixupTable - contains information used for PCH softstrap fix up.

  //
  // SIO Initialization table
  //
  PILOTIV_SIO_INDEX_PORT,                  // SIO Index port
  PILOTIV_SIO_DATA_PORT,                   // SIO Data port
  mSioInitTable,                            // mSioInitTable - contains the settings for initializing the SIO.
  sizeof(mSioInitTable)/sizeof(SIO_INDEX_DATA) // NumSioItems - Number of items in the SIO init table.
};

//
// System board PPI structure
//
static SYSTEM_BOARD_PPI                mSystemBoardPpi = {
  &SystemBoardInfo,           // System board information
  PchPlatformPolicyInit,
  GetUsbConfig,               // Get the platforms USB configuration
  SystemIioPortBifurcationInit,   // Set IIO Bifurcation ports configuration
  GetUplinkPortInformation,
  OutputDataToPlatformLcd,    // Output data to the platforms LCD.
  LanEarlyInit,               // Initialize LAN
  HandleBootMode,             // Handle any special boot mode changes for the system.
  ConfigurePlatformClocks,
  HandleAcPowerRecovery,
  SystemBoardIdValue,
  SystemBoardIdSkuValue,
  SystemPchUsbInitRecovery,
  FeaturesBasedOnPlatform,
  PchGpioInit,
  InstallPlatformHsioPtssTable
  , SystemBoardRevIdValue
  , GetBmcPciePort
  , PcieSataPortSelectionWA
  , GetFpgaHssiCardBbsInfo
  , SetFpgaGpioOff
  , GetDefaultFpgaBbsIndex
};

static EFI_PEI_PPI_DESCRIPTOR       mSystemBoardPpiDesc = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gEfiPeiSystemBoardPpiGuid,
  &mSystemBoardPpi
};

//
// Table providing details on clocks supported by this library
//
// It is critical that this table be properly constructed.
// The table entries must line up with the clock generatory types
//
CLOCK_GENERATOR_DETAILS mSupportedClockGeneratorTable[] = {
{
    ClockGeneratorCk410,
    CK410_GENERATOR_ID,
    CK410_GENERATOR_SPREAD_SPECTRUM_BYTE,
    CK410_GENERATOR_SPREAD_SPECTRUM_BIT
  },
{
    ClockGeneratorCk420,
    CK420_GENERATOR_ID,
    CK420_GENERATOR_SPREAD_SPECTRUM_BYTE,
    CK420_GENERATOR_SPREAD_SPECTRUM_BIT
  },
{
    ClockGeneratorCk505,
    CK505_GENERATOR_ID,
    CK505_GENERATOR_SPREAD_SPECTRUM_BYTE,
    CK505_GENERATOR_SPREAD_SPECTRUM_BIT
  }
};

/**

    GC_TODO: Return BoardID

    @param VOID

    @retval BoardId

**/
UINT8
SystemBoardIdValue (VOID)
{
  EFI_HOB_GUID_TYPE       *GuidHob;
  EFI_PLATFORM_INFO       *mPlatformInfo;

  GuidHob       = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT(GuidHob != NULL);
  if (GuidHob == NULL) {
    return TypePlatformUnknown;
  }
  mPlatformInfo  = GET_GUID_HOB_DATA(GuidHob);
  return mPlatformInfo->BoardId;
}

/**

    Return BoardID Subtype

    @param VOID

    @retval BoardId Subtype

**/
UINT16
SystemBoardIdSkuValue (VOID)
{
  return PcdGet16(PcdOemSkuSubBoardID);
}

UINT32
SystemBoardRevIdValue (VOID)
{
  EFI_HOB_GUID_TYPE       *GuidHob;
  EFI_PLATFORM_INFO       *mPlatformInfo;

  GuidHob       = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT(GuidHob != NULL);
  if (GuidHob == NULL) {
    return 0xFF; //TODO: Create an Unknown type to return here
  }
  mPlatformInfo  = GET_GUID_HOB_DATA(GuidHob);
  return mPlatformInfo->TypeRevisionId;
}


/**

  Configure the clock generator to enable free-running operation.  This keeps
  the clocks from being stopped when the system enters C3 or C4.

  @param None

  @retval EFI_SUCCESS    The function completed successfully.

**/
EFI_STATUS
ConfigurePlatformClocks (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN VOID                               *Ppi
  )
{
  EFI_STATUS                            Status;
  CLOCKING_MODES                        ClockingMode;
  UINT8                                 *ConfigurationTable;
  UINT8                                 ConfigurationTablePlatformSRP[] = CLOCK_GENERATOR_SETTINGS_PLATFORMSRP;
  UINT8                                 ConfigurationTableCK505[] = CLOCK_GENERATOR_SETTINGS_CK505;
  UINTN                                 Length;
  CLOCK_GENERATOR_TYPE                  ClockType;
  BOOLEAN                               SecondarySmbus = FALSE;
  BOOLEAN                               EnableSpreadSpectrum;
  PCH_POLICY_PPI                        *PchPolicyPpi;

#if ME_SUPPORT_FLAG
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#if SPS_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
  Status = PeiGetCurrenClockingMode(&ClockingMode);
#else
  if (PchStepping () == LbgA0) {
    ClockingMode = InternalStandard;
  } else {
    ClockingMode = InternalAlternate;
  }
  Status = EFI_SUCCESS;
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
#endif // ME_SUPPORT_FLAG
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ConfigurePlatformClocks. Can't read clock mode! EFI_STATUS= 0x%x\n", Status));
    return Status;
  }

  if (External == ClockingMode)
  {
    DEBUG ((DEBUG_INFO, "ConfigurePlatformClocks. Clock Mode: External\n"));

          if (IsSimicsPlatform()) {
            //
            // Simics implements CK505 model
            //
            ConfigurationTable = ConfigurationTableCK505;
            Length = sizeof (ConfigurationTableCK505);
            ClockType = ClockGeneratorCk505;
          }
          else {
            //
            // SRP/DVP configuration
            //
            ConfigurationTable = ConfigurationTablePlatformSRP;
            Length = sizeof (ConfigurationTablePlatformSRP);
            ClockType = ClockGeneratorCk420;
      }
      Status = (*PeiServices)->LocatePpi (PeiServices,
                                          &gPchPlatformPolicyPpiGuid,
                                          0,
                                          NULL,
                                          (VOID **)&PchPolicyPpi
                                         );
      ASSERT_EFI_ERROR (Status);
      EnableSpreadSpectrum = (BOOLEAN) PchPolicyPpi->PchConfig.EnableClockSpreadSpec;
      if (1 == EnableSpreadSpectrum) {
        ConfigurationTable[mSupportedClockGeneratorTable[ClockType].SpreadSpectrumByteOffset] |= mSupportedClockGeneratorTable[ClockType].SpreadSpectrumBitOffset;
      } else {
        ConfigurationTable[mSupportedClockGeneratorTable[ClockType].SpreadSpectrumByteOffset] &= ~(mSupportedClockGeneratorTable[ClockType].SpreadSpectrumBitOffset);
      }
      Status = ConfigureClockGenerator (PeiServices,
                                        ClockType,
                                        (UINT8) CLOCK_GENERATOR_ADDRESS,
                                        Length,
                                        ConfigurationTable,
                                        EnableSpreadSpectrum,
                                        &mSupportedClockGeneratorTable[ClockType],
                                        SecondarySmbus
                                       );
      ASSERT_EFI_ERROR (Status);
  }





  return EFI_SUCCESS;
}

//
// Functions defined by the system board PPI.
//
/**

    Gets USB Overcurrent mapping configuration based on board identification

    @param Usb20OverCurrentMappings - USB 2.0 ports overcurrent mapping configuration
    @param Usb30OverCurrentMappings - USB 3.0 ports overcurrent mapping configuration

    @retval None

**/
VOID
GetUsbConfig (
  OUT VOID **Usb20OverCurrentMappings,
  OUT VOID **Usb30OverCurrentMappings,
  OUT VOID **Usb20AfeParams
  )
{
  PlatformGetUsbOcMappings ( (PCH_USB_OVERCURRENT_PIN **)Usb20OverCurrentMappings,
                             (PCH_USB_OVERCURRENT_PIN **)Usb30OverCurrentMappings,
                             (PCH_USB20_AFE           **)Usb20AfeParams
                           );

}
/**
  DVP/Neon City platform support BW5 bifurcation card in socket0 and socket1
  Lightning Ridge platform support BW5 bifurcation card in socket1, socket2 and socket3
  The bifurcation info stored at I/O Expander (PCA9555) which BIOS
  can get through Smbus read.

  PCA9555 SMBus slave Address: 0x4C

----------------------------------
  Neon
----------------------------------
    data0 = BW5 socket0 iio2
    data1 = BW5 socket1 iio0
    data2 = BW5 socket1 iio3
----------------------------------
  Lighting Ridge
----------------------------------
    data1 = BW5 socket1 iio2
    data2 = BW5 socket2 iio1
    data3 = BW5 socket3 iio3

  The bifurcation encoding is [2:0]:
  BW5_BIFURCATE_x4x4x4x4  x 0 0 0
  BW5_BIFURCATE_xxx8x4x4  x 0 0 1
  BW5_BIFURCATE_x4x4xxx8  x 0 1 0
  BW5_BIFURCATE_xxx8xxx8  x 0 1 1
  No BW5                  x 1 1 1

  @param IioGlobalData - Pointer to IioGlobals
  @param Bw5Id         - Pointer to BW5ID

  @retval None

**/
VOID
GetBw5Id (
  IN IIO_GLOBALS *IioGlobalData,
  IN  OUT BW5_BIFURCATION_DATA_STRUCT        *Bw5Id
  )
{
  CONST EFI_PEI_SERVICES    **PeiServices;
  EFI_STATUS                Status;
  EFI_SMBUS_DEVICE_ADDRESS  SmbusDeviceAddress;
  EFI_SMBUS_DEVICE_COMMAND  SmbusCommand;
  EFI_PEI_SMBUS2_PPI        *Smbus = NULL;
  UINT16                    SmbusData = 0;
  UINT8                     RetryCount;
  UINTN                     SmbusLength = 2;
  UINT8                     Index;

  PeiServices = GetPeiServicesTablePointer ();

  Status = (**PeiServices).LocatePpi (
                              PeiServices,
                              &gEfiPeiSmbus2PpiGuid,
                              0,
                              NULL,
                              &Smbus
                              );

  // Initialize Bw5Id to not present
  for (Index = 0; Index < BW5_CARDS_PRESENT; Index++){
    Bw5Id[Index].Data =  BW5_CARD_NOT_PRESENT;
  }

  if (Status != EFI_SUCCESS || Smbus == NULL) {
    DEBUG ((DEBUG_ERROR, "!!!!Get SMBus protocol error %x\n", Status));
  } else {

    // Read Socket 0 HP Controller
    SmbusDeviceAddress.SmbusDeviceAddress = (BW5_SMBUS_ADDRESS >> 1);

    for (RetryCount = 0; RetryCount < NUM_OF_RETRIES; RetryCount++) {
      //
      // Read the current I/O pins Config for Port0
      //
      SmbusCommand = PCA9555_COMMAND_CONFIG_PORT0_REG;
      Status = Smbus->Execute (
                             Smbus,
                             SmbusDeviceAddress,
                             SmbusCommand,
                             EfiSmbusReadWord,
                             FALSE,
                             &SmbusLength,
                             &SmbusData
                             );
       if (!EFI_ERROR(Status)) {
        //
        // Configure the direction of I/O pins for Port0/Port1 as Input.
        //
        SmbusData = SmbusData | BW5_CONFIG_REG_MASK;
        Status = Smbus->Execute (
                             Smbus,
                             SmbusDeviceAddress,
                             SmbusCommand,
                             EfiSmbusWriteWord,
                             FALSE,
                             &SmbusLength,
                             &SmbusData
                             );
         if (!EFI_ERROR(Status)) {
           //
           // Read Input Port0/Port1 register to identify BW5 Id
           //
           SmbusCommand = PCA9555_COMMAND_INPUT_PORT0_REG;
           Status = Smbus->Execute( Smbus,
                               SmbusDeviceAddress,
                               SmbusCommand,
                               EfiSmbusReadWord,
                               FALSE,
                               &SmbusLength,
                               &SmbusData );
           if (!EFI_ERROR(Status)){
             DEBUG ((DEBUG_ERROR, "SmbusData Port0/1 %x\n", SmbusData));
             //
             // Mask the Input Port0/1 register data [15:0] to get BW5 ID.
             //
             Bw5Id[0].Data = (SmbusData & BW5_0_ID_MASK);
             Bw5Id[1].Data = (SmbusData & BW5_1_ID_MASK) >> 4;
             Bw5Id[2].Data = (SmbusData & BW5_2_ID_MASK) >> 8;
             Bw5Id[3].Data = (SmbusData & BW5_3_ID_MASK) >> 12;
             break; // Break Loop if read was successfully.
          } // Read Port0
        } // Configure Port0
      } // Read Port0 Config
    } //RetryCount
  } // (Status != EFI_SUCCESS || Smbus == NULL)

  return;
}

VOID
OverrideDefaultBifSlots(
    IN IIO_GLOBALS *IioGlobalData,
    IN UINT8       BoardId
)
{
  EFI_STATUS                    Status;
  UINT32                        QATGpio;
  PCIE_RISER_ID                 RightRiser, LeftRiser;
  UINT32                        RiserBit;
  UINT8                         Index;
  BW5_BIFURCATION_DATA_STRUCT Bw5id[4]= {0,0,0,0}; // Default, no BW card.
  IIO_BROADWAY_ADDRESS_ENTRY    *BroadwayTable;
  IIO_BROADWAY_ADDRESS_DATA_ENTRY *BroadwayTableTemp;
  UINT8                           IOU0Setting;
  UINT8                           IOU2Setting;
  UINT8                           FlagValue = 0;
  UINT8                           SkuPersonality = 0;

  BroadwayTable = NULL;
  IOU0Setting = IioGlobalData->SetupData.ConfigIOU0[0];
  IOU2Setting = IioGlobalData->SetupData.ConfigIOU2[0];

  //
  // Specific platform overrides.
  //
  // Changes because GPIO (QAT, Riser Cards, etc).
  // Read QAT and riser card GPIOs.
  //
  // Purley platforms need to read the QAT bit
  //
  Status = GpioGetInputValue (GPIO_SKL_H_GPP_B3,  &QATGpio);
  DEBUG((EFI_D_INFO, "QAT GPIO: %d\n", QATGpio));

  if ((IioGlobalData->IioVar.IioVData.SkuPersonality[0] == TYPE_FPGA) &&\
      (IioGlobalData->IioVar.IioVData.SkuPersonality[1] == TYPE_FPGA)) {
    SkuPersonality = 1;
  } else if ((IioGlobalData->IioVar.IioVData.SkuPersonality[0] == TYPE_FPGA) &&\
      (IioGlobalData->IioVar.IioVData.SkuPersonality[1] != TYPE_FPGA)) {
    SkuPersonality = 2;
  } else if ((IioGlobalData->IioVar.IioVData.SkuPersonality[0] != TYPE_FPGA) &&\
      (IioGlobalData->IioVar.IioVData.SkuPersonality[1] == TYPE_FPGA)) {
    SkuPersonality = 3;
  } else {
    SkuPersonality = 0;
  }
  DEBUG((EFI_D_ERROR, "SKU Personality Type: %d\n", SkuPersonality));

  BroadwayTableTemp = (IIO_BROADWAY_ADDRESS_DATA_ENTRY *)BroadwayTable;
  PlatformGetSlotTableData2 (&BroadwayTableTemp, &IOU0Setting, &FlagValue, &IOU2Setting, SkuPersonality);
  BroadwayTable    = (IIO_BROADWAY_ADDRESS_ENTRY *)BroadwayTableTemp; // if no platform definition, BroadwayTable will be NULL
  IioGlobalData->SetupData.ConfigIOU2[0] = IOU2Setting;
  if (FlagValue == 1) {
      //
      // Right riser
      //
      Status = GpioGetInputValue (GPIO_SKL_H_GPP_B4,   &RiserBit);  // PresentSignal
      RightRiser.Bits.PresentSignal = (UINT8)RiserBit;

      Status = GpioGetInputValue (GPIO_SKL_H_GPP_C15,  &RiserBit);  // HotPlugConf
      RightRiser.Bits.HPConf   = (UINT8)RiserBit;

      Status = GpioGetInputValue (GPIO_SKL_H_GPP_C16,  &RiserBit);  // WingConf
      RightRiser.Bits.WingConf      = (UINT8)RiserBit;

      Status = GpioGetInputValue (GPIO_SKL_H_GPP_C17,  &RiserBit);  // Slot9En
      RightRiser.Bits.Slot9En       = (UINT8)RiserBit;

      DEBUG((EFI_D_INFO, "GPIO Right riser information: PresentSignal=%x, HotPlugConf=%x, WingConf=%x, Slot9En=%x\n",
            RightRiser.Bits.PresentSignal, RightRiser.Bits.HPConf, RightRiser.Bits.WingConf, RightRiser.Bits.Slot9En));

      //
      // Left riser
      //
      Status = GpioGetInputValue (GPIO_SKL_H_GPP_B5,   &RiserBit);  // PresentSignal
      LeftRiser.Bits.PresentSignal = (UINT8)RiserBit;

      Status = GpioGetInputValue (GPIO_SKL_H_GPP_C18,  &RiserBit);  // HotPlugConf
      LeftRiser.Bits.HPConf   = (UINT8)RiserBit;

      Status = GpioGetInputValue (GPIO_SKL_H_GPP_C19,  &RiserBit);  // WingConf
      LeftRiser.Bits.WingConf      = (UINT8)RiserBit;

      Status = GpioGetInputValue (GPIO_SKL_H_GPP_B21,  &RiserBit);  // Slot9En
      LeftRiser.Bits.Slot9En       = (UINT8)RiserBit;

      DEBUG((EFI_D_INFO, "GPIO Left riser information: PresentSignal=%x, HotPlugConf=%x, WingConf=%x, Slot9En=%x\n",
            LeftRiser.Bits.PresentSignal, LeftRiser.Bits.HPConf, LeftRiser.Bits.WingConf, LeftRiser.Bits.Slot9En));
  }

  if (QATGpio == QAT_ENABLED) {
    // So Configuration of IUO0 is:
    //  1A-1B - QAT xxx8
    //  1C    - SSD x4
    //  1D    - SSD x4
    IioGlobalData->SetupData.ConfigIOU0[0] = IOU0Setting;
  }

  if (FlagValue == 1) {
      if (QATGpio != QAT_ENABLED) {
        if ((RightRiser.Bits.Slot9En == RISER_SLOT9_DISABLE) &&
             (LeftRiser.Bits.Slot9En == RISER_SLOT9_DISABLE)) {
          //
          // SLOT 9 is disabled. SSDs are present.
          // Change configuration to x4x4x4x4.
          //
          IioGlobalData->SetupData.ConfigIOU0[0]=IIO_BIFURCATE_x4x4x4x4;
          IioGlobalData->IioVar.IioOutData.PciePortOwnership[SOCKET_0_INDEX + PORT_1A_INDEX] = PCIEAIC_OCL_OWNERSHIP;
          IioGlobalData->IioVar.IioOutData.PciePortOwnership[SOCKET_0_INDEX + PORT_1B_INDEX] = PCIEAIC_OCL_OWNERSHIP;
          IioGlobalData->IioVar.IioOutData.PciePortOwnership[SOCKET_0_INDEX + PORT_1C_INDEX] = PCIEAIC_OCL_OWNERSHIP;
          IioGlobalData->IioVar.IioOutData.PciePortOwnership[SOCKET_0_INDEX + PORT_1D_INDEX] = PCIEAIC_OCL_OWNERSHIP;
        } else if (RightRiser.Bits.PresentSignal == RISER_PRESENT) {
          //
          // Slot 9 is enabled. Keep the xxxxxx16 configuration (default) and
          // enable slot 9. Slot 9 supports HP.
          //
          IioGlobalData->SetupData.SLOTIMP[SOCKET_0_INDEX + PORT_1A_INDEX] = 1;
          IioGlobalData->SetupData.SLOTPSP[SOCKET_0_INDEX + PORT_1A_INDEX] = 9;
        }
      } // End of QAT_ENABLED

      if (RightRiser.Bits.PresentSignal == RISER_PRESENT) {
        IioGlobalData->SetupData.SLOTIMP[SOCKET_0_INDEX + PORT_3A_INDEX] = 1;
        IioGlobalData->SetupData.SLOTIMP[SOCKET_3_INDEX + PORT_2A_INDEX] = 1;
        IioGlobalData->SetupData.SLOTIMP[SOCKET_3_INDEX + PORT_3A_INDEX] = 1;
        if (RightRiser.Bits.WingConf == RISER_WINGED_IN) {
          IioGlobalData->SetupData.SLOTPSP[SOCKET_0_INDEX + PORT_3A_INDEX] = 1;
          IioGlobalData->SetupData.SLOTPSP[SOCKET_3_INDEX + PORT_2A_INDEX] = 4;
          IioGlobalData->SetupData.SLOTPSP[SOCKET_3_INDEX + PORT_3A_INDEX] = 2;
        } else {  // RISER_WINGED_OUT
          IioGlobalData->SetupData.SLOTPSP[SOCKET_0_INDEX + PORT_3A_INDEX] = 2;
          IioGlobalData->SetupData.SLOTPSP[SOCKET_3_INDEX + PORT_2A_INDEX] = 3;
          IioGlobalData->SetupData.SLOTPSP[SOCKET_3_INDEX + PORT_3A_INDEX] = 1;
          if (RightRiser.Bits.HPConf == RISER_HP_EN) {
            //
            // PCIe Hot Plug is supported on Winged-out riser only
            //
            EnableHotPlug(IioGlobalData, SOCKET_0_INDEX + PORT_3A_INDEX, VPP_PORT_0, 0x40, REGULAR_PCIE_OWNERSHIP);
            EnableHotPlug(IioGlobalData, SOCKET_3_INDEX + PORT_2A_INDEX, VPP_PORT_1, 0x40, REGULAR_PCIE_OWNERSHIP);
            EnableHotPlug(IioGlobalData, SOCKET_3_INDEX + PORT_3A_INDEX, VPP_PORT_0, 0x40, REGULAR_PCIE_OWNERSHIP);
          }
        }
      }

      if (LeftRiser.Bits.PresentSignal == RISER_PRESENT) {
        IioGlobalData->SetupData.SLOTIMP[SOCKET_1_INDEX + PORT_1A_INDEX] = 1;
        IioGlobalData->SetupData.SLOTIMP[SOCKET_1_INDEX + PORT_2A_INDEX] = 1;
        IioGlobalData->SetupData.SLOTIMP[SOCKET_2_INDEX + PORT_1A_INDEX] = 1;
        IioGlobalData->SetupData.SLOTIMP[SOCKET_2_INDEX + PORT_3A_INDEX] = 1;
        if (LeftRiser.Bits.WingConf == RISER_WINGED_IN) {
          IioGlobalData->SetupData.SLOTPSP[SOCKET_1_INDEX + PORT_1A_INDEX] = 7;
          IioGlobalData->SetupData.SLOTPSP[SOCKET_1_INDEX + PORT_2A_INDEX] = 5;
          IioGlobalData->SetupData.SLOTPSP[SOCKET_2_INDEX + PORT_1A_INDEX] = 6;
          IioGlobalData->SetupData.SLOTPSP[SOCKET_2_INDEX + PORT_3A_INDEX] = 8;
        } else {  // RISER_WINGED_OUT
          IioGlobalData->SetupData.SLOTPSP[SOCKET_1_INDEX + PORT_1A_INDEX] = 5;
          IioGlobalData->SetupData.SLOTPSP[SOCKET_1_INDEX + PORT_2A_INDEX] = 7;
          IioGlobalData->SetupData.SLOTPSP[SOCKET_2_INDEX + PORT_1A_INDEX] = 8;
          IioGlobalData->SetupData.SLOTPSP[SOCKET_2_INDEX + PORT_3A_INDEX] = 6;
          if (LeftRiser.Bits.HPConf == RISER_HP_EN) {
            //
            // PCIe Hot Plug is supported on Winged-out riser only
            //
            EnableHotPlug(IioGlobalData, SOCKET_1_INDEX + PORT_1A_INDEX, VPP_PORT_0, 0x42, REGULAR_PCIE_OWNERSHIP);
            EnableHotPlug(IioGlobalData, SOCKET_1_INDEX + PORT_2A_INDEX, VPP_PORT_1, 0x42, REGULAR_PCIE_OWNERSHIP);
            EnableHotPlug(IioGlobalData, SOCKET_2_INDEX + PORT_1A_INDEX, VPP_PORT_1, 0x42, REGULAR_PCIE_OWNERSHIP);
            EnableHotPlug(IioGlobalData, SOCKET_2_INDEX + PORT_3A_INDEX, VPP_PORT_0, 0x42, REGULAR_PCIE_OWNERSHIP);
          }
        }
      }

  }


  /// Broadway overrides.
  if (BroadwayTable != NULL) {
    GetBw5Id(IioGlobalData, Bw5id);
    DEBUG((EFI_D_INFO,"Broadway Config: 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",Bw5id[Bw5_Addr_0].Data, Bw5id[Bw5_Addr_1].Data, Bw5id[Bw5_Addr_2].Data,Bw5id[Bw5_Addr_3].Data));
    for (Index = 0; Index < 3; Index ++) {
      //
      // Check if BW5 is present before override IOUx Bifurcation
      //
      if (BroadwayTable->BroadwayAddress == Bw5_Addr_Max) {
        break;
      }
      if (Bw5id[BroadwayTable->BroadwayAddress].Bits.BifBits != BW5_CARD_NOT_PRESENT){
        switch (BroadwayTable->IouNumber) {
         case Iio_Iou0:
           IioGlobalData->SetupData.ConfigIOU0[BroadwayTable->Socket] = Bw5id[BroadwayTable->BroadwayAddress].Bits.BifBits;
           DEBUG((DEBUG_ERROR,"IioGlobalData->SetupData.ConfigIOU0[%x] = %x\n",BroadwayTable->Socket, IioGlobalData->SetupData.ConfigIOU0[BroadwayTable->Socket]));
           break;
         case Iio_Iou1:
           IioGlobalData->SetupData.ConfigIOU1[BroadwayTable->Socket] = Bw5id[BroadwayTable->BroadwayAddress].Bits.BifBits;
           DEBUG((DEBUG_ERROR,"IioGlobalData->SetupData.ConfigIOU1[%x] = %x\n",BroadwayTable->Socket, IioGlobalData->SetupData.ConfigIOU1[BroadwayTable->Socket]));
           break;
         case Iio_Iou2:
           IioGlobalData->SetupData.ConfigIOU2[BroadwayTable->Socket] = Bw5id[BroadwayTable->BroadwayAddress].Bits.BifBits;
           DEBUG((DEBUG_ERROR,"IioGlobalData->SetupData.ConfigIOU2[%x] = %x\n",BroadwayTable->Socket,IioGlobalData->SetupData.ConfigIOU2[BroadwayTable->Socket]));
           break;
         default:
          break;
        } // BroadwayTable->IouNumber
      } // No BW5 present
      BroadwayTable ++;
    } // for Index
  } // BroadwayTable != NULL

}

/**

  GetUplinkPortInformation - Get uplink port information

  @param IioIndex - socket ID.

  @retval PortIndex for uplink port

**/
UINT8
GetUplinkPortInformation (
    IN UINT8 IioIndex
)
{
  UINT8 UplinkPortIndex;
  UINT8 BoardId;
  BoardId = SystemBoardIdValue();
  UplinkPortIndex =  GetUplinkPortInformationCommon(BoardId, IioIndex);
  return UplinkPortIndex;
}

/**

  SystemIioPortBifurcationInit - Program the IIO_GLOBALS data structure with OEM IIO init values
  for SLOTs and Bifurcation.

  @param mSB - pointer to this protocol
  @param IioUds - Pointer to the IIO UDS datastructure.

  @retval EFI_SUCCESS

**/
VOID
SystemIioPortBifurcationInit (
    IN IIO_GLOBALS *IioGlobalData
)
{

  UINT8                         IioIndex;
#if SMCPKG_SUPPORT == 0
  IIO_BIFURCATION_ENTRY         *BifurcationTable = NULL;
  UINT8                         BifurcationEntries;
  IIO_SLOT_CONFIG_ENTRY         *SlotTable = NULL;
  UINT8                         SlotEntries;
  UINT8 BoardId;
  BoardId = SystemBoardIdValue();
  // This function outline:
  // 1. Based on platform apply the default bifurcation and slot configuration.
  // 2. Apply dynamic overrides based on GPIO and other configurations.
  // 3. Hide unused ports due bifurcation.

  SystemIioPortBifurcationInitCommon(BoardId, IioGlobalData, &BifurcationTable, &BifurcationEntries, &SlotTable, &SlotEntries);
  /// Set the default bifurcations for this platform.
  SetBifurcations(IioGlobalData, BifurcationTable, BifurcationEntries);
  ConfigSlots(IioGlobalData, SlotTable, SlotEntries);
  OverrideDefaultBifSlots(IioGlobalData, BoardId);
  OverrideConfigSlots(IioGlobalData, SlotTable, SlotEntries);
#endif
  // All overrides have been applied now.
  // Hide root ports whose lanes are assigned preceding ports.
  for (IioIndex = Iio_Socket0; IioIndex < MaxIIO; IioIndex++) {
    if (IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]) {
      SystemHideIioPortsCommon(IioGlobalData, IioIndex);
    }
  }
}

/**

  Allow users to view the BIOS version and CPU stepping
  from the MCU LCD.  Request to also output if ME FW is SV or non-SV

  @retval EFI_SUCCESS    The function completed successfully.

**/
EFI_STATUS
OutputDataToPlatformLcd (
    IN     CONST EFI_PEI_SERVICES               **PeiServices
  )
{
/*
  EFI_PEI_STALL_PPI           *StallPpi;
  EFI_STATUS                  Status;
  EFI_PEI_SMBUS2_PPI          *SmbusPpi;
  EFI_SMBUS_DEVICE_COMMAND    SmbusCommand;
  EFI_SMBUS_DEVICE_ADDRESS    SlaveAddress;
  CHAR8                       *FwType = "BIOS Ver:";
  UINTN                       SmbusLength = 12;
  UINT8                       SmbusData[32] = {0x00};
  UINT8                       Index;
  UINT8                       BiosId[] = CONVERT_TO_STRING( BIOS_ID );

  if(IsSimicsPlatform())
    return EFI_SUCCESS;

  //
  // Locate SmBus Ppi
  //
  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gEfiPeiSmbus2PpiGuid,
                            0,
                            NULL,
                            &SmbusPpi
                            );
  ASSERT_EFI_ERROR (Status);

  //
  // Locate Stall PPI
  //
  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gEfiPeiStallPpiGuid,
                            0,
                            NULL,
                            &StallPpi
                            );
  ASSERT_EFI_ERROR (Status);


  //
  // MCU address = 0x72, Command to MCU = 0x32
  // MCU will then return SmbusLength bytes at location SmbusData (max of 32 bytes).
  // First line: SV or Non-SV. Length is +2 (leading line number byte, trailing null byte)
  //
  SlaveAddress.SmbusDeviceAddress = (0x72 >> 1);
  SmbusCommand = 0x32;
  SmbusData[ 0 ] = 0;

  for (Index = 0; Index < (SmbusLength - 1); Index++) {
    SmbusData[ Index+1 ] = FwType[ Index ];
  }

  //
  // Use EfiSmbusBWBRProcessCall to communicate with MCU
  //
  Status = SmbusPpi->Execute( SmbusPpi,
                              SlaveAddress,
                              SmbusCommand,
                              EfiSmbusBWBRProcessCall,
                              FALSE,
                              &SmbusLength,
                              &SmbusData );

  StallPpi->Stall (PeiServices, StallPpi, 20000);

  //
  // Ensure that we have back 3 bytes and retry util buffer is free
  //
  if(Status == EFI_SUCCESS)
  {
    while(SmbusData[0] == 1)// Check if BWBR was deferred by MCU
    {
      SmbusLength = 1; // Send Length of 1
      SmbusCommand = 0xb;
      SmbusData[ 0 ] = SmbusData[1]; // Send buffer ID to read
      Status = SmbusPpi->Execute(SmbusPpi,
                                SlaveAddress,
                                SmbusCommand,
                                EfiSmbusBWBRProcessCall,
                                FALSE,
                                &SmbusLength,
                                &SmbusData );
    }
  }

  //
  // Second line: BIOS version including Date/time
  // e.g. "D082.06110935"
  //
  SlaveAddress.SmbusDeviceAddress = (0x72 >> 1);
  SmbusCommand = 0x32;
  SmbusLength  = 16;
  SmbusData[ 0 ] = 1;

  for (Index = 0; Index < 14; Index++) {
     SmbusData[Index+1] = BiosId[Index+18];
  }

  //
  // Use EfiSmbusBWBRProcessCall to communicate with MCU
  //
  Status = SmbusPpi->Execute( SmbusPpi,
                              SlaveAddress,
                              SmbusCommand,
                              EfiSmbusBWBRProcessCall,
                              FALSE,
                              &SmbusLength,
                              &SmbusData );

  StallPpi->Stall (PeiServices, StallPpi, 20000);
  //
  // Ensure that we have back 3 bytes and retry until buffer is free
  //
  if(Status == EFI_SUCCESS)
  {
    while (SmbusData[0] == 1){ // Check if BWBR was deferred by MCU
      SmbusLength = 1; // Set length to 1 to only send buffer ID
      SmbusCommand = 0xb;
      SmbusData[ 0 ] = SmbusData[1]; // Send buffer ID to read
      Status = SmbusPpi->Execute( SmbusPpi,
                                  SlaveAddress,
                                  SmbusCommand,
                                  EfiSmbusBWBRProcessCall,
                                  FALSE,
                                  &SmbusLength,
                                  &SmbusData );
    }
  }
  */
  return EFI_UNSUPPORTED;
}

/**

    GC_TODO: add routine description

    @param PeiServices - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS
HandleAcPowerRecovery (
  IN EFI_PEI_SERVICES  **PeiServices
  )
{
  UINT8 BoardId;
  BoardId = SystemBoardIdValue();
  //
  // Program board specific AC Power recovery code here.
  //
  return EFI_SUCCESS;
}

/*
  HandleBootMode is here to allow the platform to make any system board updates, etc... for a given boot mode.

  HandleClock is used to update any specific clock data bytes before it is written to the
  clock generator.

  @param BootMode - Boot mode variable, which contains the current boot mode.

*/
VOID
HandleBootMode (
  IN EFI_BOOT_MODE          BootMode
  )
{
  UINT8 BoardId;
  BoardId = SystemBoardIdValue();
  // Do any boarding specific programming for the boot mode.
}

/**
  PchPlatformPolicyInit

  @param *PchPlatformPolicyPpi - Pointer to PCH_PLATFORM_POLICY_PPI Ppi

  @retval None

**/
VOID
PchPlatformPolicyInit (
  //  IN PCH_SATA_CONTROL **SataConfig
  IN PCH_POLICY_PPI *PchPlatformPolicyPpi
  )
{
  UINT8 BoardId;

  BoardId = SystemBoardIdValue();


  return;

}

/**

    GC_TODO: add routine description

    @param *OverCurrentMappings - Pointer to OverCurrentMappings
    @param *OverCurrentMappingsSize - Pointer to OverCurrentMappingSize
	@param *PortLength - Pointer to Portlength

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
VOID
SystemPchUsbInitRecovery (
  IN VOID                      *OverCurrentMappings,
  IN UINTN                     *OverCurrentMappingsSize,
  IN VOID                      *PortLength
)
{
}

/**
  Do any LAN initialization for the system board.

  @param **PeiServices - PEI services
  @param *SystemConfiguration - Points to where the setup data will be stored.

  @retval EFI_SUCCESS  -  Init succeed.

**/
EFI_STATUS
LanEarlyInit (
  IN CONST EFI_PEI_SERVICES          **PeiServices,
  IN SYSTEM_CONFIGURATION            *SystemConfiguration
  )
{
  PlatformPchLanConfig(SystemConfiguration);
  return EFI_SUCCESS;
}

/**

    FeaturesBasedOnPlatform get features based on platform

    @param VOID

    @retval Feature flag

**/
UINT32
FeaturesBasedOnPlatform (
  VOID
  )
{
  UINT32 PlatformFeatureFlag = 0;
  PlatformFeatureFlag = PcdGet32(PcdOemSkuPlatformFeatureFlag);
  return PlatformFeatureFlag;
}

//
// PEI entry point - SystemBoardPpi entry point
//
/**

  PEI system board PPI intialization main entry point. This will setup up a PPI that will handle providing system board level
  configuration for the platform.

  @param FileHandle         Pointer to the PEIM FFS file header.
  @param PeiServices       General purpose services available to every PEIM.

  @retval EFI_SUCCESS       Operation completed successfully.
  @retval Otherwise         System board initialization failed.
**/
EFI_STATUS
EFIAPI
SystemBoardPpiEntry (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  //
  // Initialize system board information PPI
  //
  Status = PeiServicesInstallPpi(&mSystemBoardPpiDesc);
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**

    GC_TODO: add routine description

    @param PeiServices         - GC_TODO: add arg description
    @param SystemBoard         - GC_TODO: add arg description
    @param SystemConfiguration - GC_TODO: add arg description

    @retval None

**/
VOID
PchGpioInit (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
  )
{
}

VOID
InstallPlatformHsioPtssTable (
  IN          PCH_RC_CONFIGURATION         *PchSetup,
  IN OUT      PCH_POLICY_PPI               *PchPolicy
  )
{
  HSIO_PTSS_TABLES            *PtssTables;
  UINT8                       PtssTableIndex;
  UINT32                      TableSize;
  UINT32                      Entry;
  UINT8                       LaneNum;
  UINT8                       Index;
  UINT8                       MaxSataPorts;
  UINT8                       MaxsSataPorts;
// APTIOV_SERVER_OVERRIDE_RC_START
#ifdef SPS_SUPPORT //(SERVER_BIOS_FLAG)
// APTIOV_SERVER_OVERRIDE_RC_END
  CLOCKING_MODES              ClockingMode;
// APTIOV_SERVER_OVERRIDE_RC_START
#endif // SPS_SUPPORT (SERVER_BIOS_FLAG)
// APTIOV_SERVER_OVERRIDE_RC_END

  UINT8                       MaxPciePorts;
  UINT8                       PcieTopologyReal[PCH_MAX_PCIE_ROOT_PORTS];
  UINT8                       PciePort;
  UINTN                       RpBase;
  UINTN                       RpDevice;
  UINTN                       RpFunction;
  UINT32                      StrapFuseCfg;
  UINT8                       PcieControllerCfg;
  EFI_STATUS                  Status;
  UINT16                      BoardId;

  switch (PchStepping ()) {
    case LbgA0:
      PtssTables = PchLbgHsioPtss_Ax;
      TableSize = PchLbgHsioPtss_Ax_Size;
      break;
    case LbgB0:
    case LbgB1:
    case LbgB2:
// APTIOV_SERVER_OVERRIDE_RC_START
#ifdef SPS_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END
      ClockingMode = InternalStandard;
      PeiGetCurrenClockingMode(&ClockingMode);
      if (ClockingMode == External) {
        PtssTables = PchLbgHsioPtss_Bx_Ext;
        TableSize = PchLbgHsioPtss_Bx_Size_Ext;
      }
      else
//APTIOV_SERVER_OVERRIDE_RC_START
#endif // SPS_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END
      {
        PtssTables = PchLbgHsioPtss_Bx;
        TableSize = PchLbgHsioPtss_Bx_Size;
      }
      break;
    case LbgS0:
    case LbgS1:
//APTIOV_SERVER_OVERRIDE_RC_START
#ifdef SPS_SUPPORT
//APTIOV_SERVER_OVERRIDE_RC_END
      ClockingMode = InternalStandard;
      PeiGetCurrenClockingMode(&ClockingMode);
      if (ClockingMode == External) {
        PtssTables = PchLbgHsioPtss_Sx_Ext;
        TableSize = PchLbgHsioPtss_Sx_Size_Ext;
      }
      else
//APTIOV_SERVER_OVERRIDE_RC_START
#endif // SPS_SUPPORT
//APTIOV_SERVER_OVERRIDE_RC_END
      {
        PtssTables = PchLbgHsioPtss_Sx;
        TableSize = PchLbgHsioPtss_Sx_Size;
      }
      break;      
    default:
      PtssTables = NULL;
      TableSize = 0;
      DEBUG ((DEBUG_ERROR, "Cannot find PTSS table for this PCH Stepping\n"));
      ASSERT (FALSE);
  }

  BoardId = SystemBoardIdValue();
  PtssTableIndex = 0;
  MaxSataPorts = GetPchMaxSataPortNum ();
  MaxsSataPorts = GetPchMaxsSataPortNum ();
  MaxPciePorts = GetPchMaxPciePortNum ();
  ZeroMem (PcieTopologyReal, sizeof (PcieTopologyReal));
  //Populate PCIe topology based on lane configuration

  CopyMem (
    PcieTopologyReal,
    PchSetup->PcieTopology,
    sizeof (PcieTopologyReal)
    );
  for (PciePort = 0; PciePort < MaxPciePorts; PciePort += 4) {
    Status = GetPchPcieRpDevFun (PciePort, &RpDevice, &RpFunction);
    RpBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32) RpDevice, (UINT32) RpFunction);
    StrapFuseCfg = MmioRead32 (RpBase + R_PCH_PCIE_STRPFUSECFG);
    PcieControllerCfg = (UINT8) ((StrapFuseCfg & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC);
    DEBUG ((DEBUG_INFO, "PCIE Port %d StrapFuseCfg Value = %d\n", PciePort, PcieControllerCfg));
    PcieTopologyReal[PciePort] = PchSetup->PcieTopology[PciePort];
    if (PcieControllerCfg != V_PCH_PCIE_STRPFUSECFG_RPC_1_1_1_1) {
      PcieTopologyReal[PciePort + 1] = PchSetup->PcieTopology[PciePort];
    }
    if (PcieControllerCfg == V_PCH_PCIE_STRPFUSECFG_RPC_4) {
      PcieTopologyReal[PciePort + 2] = PchSetup->PcieTopology[PciePort];
      PcieTopologyReal[PciePort + 3] = PchSetup->PcieTopology[PciePort];
    }
    if (PcieControllerCfg == V_PCH_PCIE_STRPFUSECFG_RPC_2_2) {
      PcieTopologyReal[PciePort + 2] = PchSetup->PcieTopology[PciePort + 2];
      PcieTopologyReal[PciePort + 3] = PchSetup->PcieTopology[PciePort + 2];
    }
    if (PcieControllerCfg == V_PCH_PCIE_STRPFUSECFG_RPC_2_1_1) {
      PcieTopologyReal[PciePort + 2] = PchSetup->PcieTopology[PciePort + 2];
      PcieTopologyReal[PciePort + 3] = PchSetup->PcieTopology[PciePort + 3];
    }
  }
  for (Index = 0; Index < MaxPciePorts; Index++) {
    DEBUG ((DEBUG_INFO, "PCIE PTSS Setup RP %d Topology = %d\n", Index, PchSetup->PcieTopology[Index]));
    DEBUG ((DEBUG_INFO, "PCIE PTSS Assigned RP %d Topology = %d\n", Index, PcieTopologyReal[Index]));
  }
  //Case 1: BoardId is known, Topology is known/unknown
  //Case 1a: SATA
  for (Index = 0; Index < MaxSataPorts; Index++) {
    if (PchGetSataLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
      for (Entry = 0; Entry < TableSize; Entry++) {
        if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
          (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_SATA) &&
          (PchSetup->SataTopology[Index] == PtssTables[Entry].Topology) &&
          (BoardId == PtssTables[Entry].BoardId)) {
          PtssTableIndex++;
          if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD20) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0) == (UINT32) B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0)) {
            PchPolicy->HsioSataConfig.PortLane[Index].HsioRxGen3EqBoostMagEnable = TRUE;
            PchPolicy->HsioSataConfig.PortLane[Index].HsioRxGen3EqBoostMag = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0;
          } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_TX_DWORD8)) {
            if (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) {
              PchPolicy->HsioSataConfig.PortLane[Index].HsioTxGen1DownscaleAmpEnable = TRUE;
              PchPolicy->HsioSataConfig.PortLane[Index].HsioTxGen1DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
            }
            if (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) {
              PchPolicy->HsioSataConfig.PortLane[Index].HsioTxGen2DownscaleAmpEnable = TRUE;
              PchPolicy->HsioSataConfig.PortLane[Index].HsioTxGen2DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
            }
          }
          else {
            DEBUG ((DEBUG_ERROR, "ERROR! PTSS programming: The PTSS table offset and/or mask are not compatible with the BIOS Code.\n"));
          }
        }
      }
    }
  }

  //Case 1a continued: Secondary SATA
  for (Index = 0; Index < MaxsSataPorts; Index++) {
    if (PchGetsSataLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
      for (Entry = 0; Entry < TableSize; Entry++) {
        if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
          (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_SATA) &&
          (PchSetup->sSataTopology[Index] == PtssTables[Entry].Topology) &&
          (BoardId == PtssTables[Entry].BoardId)) {
          PtssTableIndex++;
          if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD20) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0) == (UINT32) B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0)) {
            PchPolicy->HsiosSataConfig.PortLane[Index].HsioRxGen3EqBoostMagEnable = TRUE;
            PchPolicy->HsiosSataConfig.PortLane[Index].HsioRxGen3EqBoostMag = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0;
          } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_TX_DWORD8)) {
            if (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) {
              PchPolicy->HsiosSataConfig.PortLane[Index].HsioTxGen1DownscaleAmpEnable = TRUE;
              PchPolicy->HsiosSataConfig.PortLane[Index].HsioTxGen1DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
            }
            if (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) {
              PchPolicy->HsiosSataConfig.PortLane[Index].HsioTxGen2DownscaleAmpEnable = TRUE;
              PchPolicy->HsiosSataConfig.PortLane[Index].HsioTxGen2DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
            }
          } else {
            DEBUG ((DEBUG_ERROR, "ERROR! PTSS programming: The PTSS table offset and/or mask are not compatible with the BIOS Code.\n"));
          }
        }
      }
    }
  }

  //Case 1b: PCIe
  for (Index = 0; Index < MaxPciePorts; Index++) {
    if (PchGetPcieLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
      for (Entry = 0; Entry < TableSize; Entry++) {
          // Skip matching Lanes when the table record has settings for WM20 FIA
          if ((PtssTables[Entry].PtssTable.SbPortID == PID_MODPHY4) ||
            (PtssTables[Entry].PtssTable.SbPortID == PID_MODPHY5)){
            continue;
          }
        if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
          (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) &&
          (PcieTopologyReal[Index] == PtssTables[Entry].Topology) &&
          (BoardId == PtssTables[Entry].BoardId)) {
          PtssTableIndex++;
          if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD25) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0) == (UINT32) B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0)) {
            PchPolicy->HsioPcieConfig.Lane[Index].HsioRxSetCtleEnable = TRUE;
            PchPolicy->HsioPcieConfig.Lane[Index].HsioRxSetCtle = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0;
          }
          else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD39) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0) == (UINT32) B_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0)) {
            PchPolicy->HsioPcieConfig.Lane[Index].HsioIcfgAdjLimitLoEnable = TRUE;
            PchPolicy->HsioPcieConfig.Lane[Index].HsioIcfgAdjLimitLo = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0;
          } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD40) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP) == (UINT32) B_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP)) {
            PchPolicy->HsioPcieConfig.Lane[Index].HsioSampOffstEvenErrSpEnable = TRUE;
            PchPolicy->HsioPcieConfig.Lane[Index].HsioSampOffstEvenErrSp = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP;
          } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD41) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS) == (UINT32) B_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS)) {
            PchPolicy->HsioPcieConfig.Lane[Index].HsioRemainingSamplerOffEnable = TRUE;
            PchPolicy->HsioPcieConfig.Lane[Index].HsioRemainingSamplerOff = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS;
          } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD7) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL) == (UINT32) B_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL)) {
            PchPolicy->HsioPcieConfig.Lane[Index].HsioVgaGainCalEnable = TRUE;
            PchPolicy->HsioPcieConfig.Lane[Index].HsioVgaGainCal = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL;
          } else {
            DEBUG ((DEBUG_ERROR, "ERROR! PTSS programming: The PTSS table offset and/or mask are not compatible with the BIOS Code.\n"));
          }
        }
      }
    }
  }

  //Case 1b Continued: PCIe for WM20 FIA
  for (Index = 0; Index < PCH_MAX_WM20_LANES_NUMBER; Index++) {
      LaneNum = Index;
      for (Entry = 0; Entry < TableSize; Entry++) {
        // Skip entries which are not for WM20 FIA
        if ((PtssTables[Entry].PtssTable.SbPortID != PID_MODPHY4) &&
          (PtssTables[Entry].PtssTable.SbPortID != PID_MODPHY5)){
          continue;
        }
        if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
          (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) &&
          (BoardId == PtssTables[Entry].BoardId)) {
          PtssTableIndex++;
          if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD25) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0) == (UINT32) B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0)) {
            PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioRxSetCtleEnable = TRUE;
            PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioRxSetCtle = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0;
          } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD39) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0) == (UINT32) B_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0)) {
            PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioIcfgAdjLimitLoEnable = TRUE;
            PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioIcfgAdjLimitLo = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0;
          } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD40) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP) == (UINT32) B_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP)) {
            PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioSampOffstEvenErrSpEnable = TRUE;
            PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioSampOffstEvenErrSp = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP;
          } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD41) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS) == (UINT32) B_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS)) {
            PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioRemainingSamplerOffEnable = TRUE;
            PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioRemainingSamplerOff = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS;
          } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD7) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL) == (UINT32) B_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL)) {
            PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioVgaGainCalEnable = TRUE;
            PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioVgaGainCal = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL;
          } else {
            DEBUG ((DEBUG_ERROR, "ERROR! PTSS programming: The PTSS table offset and/or mask are not compatible with the BIOS Code.\n"));
          }
        }
      }
  }

  //Case 2: BoardId is unknown, Topology is known/unknown
  if (PtssTableIndex == 0) {
    DEBUG ((DEBUG_INFO, "PTSS Settings for unknown board will be applied\n"));
    for (Index = 0; Index < MaxSataPorts; Index++) {
      if (PchGetSataLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
        for (Entry = 0; Entry < TableSize; Entry++) {
          if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
            (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_SATA) &&
            (PchSetup->SataTopology[Index] == PtssTables[Entry].Topology) &&
            (PtssTables[Entry].BoardId == TypePlatformUnknown)) {
            if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD20) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0) == (UINT32) B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0)) {
              PchPolicy->HsioSataConfig.PortLane[Index].HsioRxGen3EqBoostMagEnable = TRUE;
              PchPolicy->HsioSataConfig.PortLane[Index].HsioRxGen3EqBoostMag = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0;
            } else if (PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_TX_DWORD8) {
              if (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) {
                PchPolicy->HsioSataConfig.PortLane[Index].HsioTxGen1DownscaleAmpEnable = TRUE;
                PchPolicy->HsioSataConfig.PortLane[Index].HsioTxGen1DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
              }
              if (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) {
                PchPolicy->HsioSataConfig.PortLane[Index].HsioTxGen2DownscaleAmpEnable = TRUE;
                PchPolicy->HsioSataConfig.PortLane[Index].HsioTxGen2DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
              }
            }
            else {
              DEBUG ((DEBUG_ERROR, "ERROR! PTSS programming: The PTSS table offset and/or mask are not compatible with the BIOS Code.\n"));
            }

          }
        }
      }
    }

    // Case 2 Continued secondary SATA when BoardId is unknown, Topology is known/unknown
    for (Index = 0; Index < MaxsSataPorts; Index++) {
      if (PchGetsSataLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
        for (Entry = 0; Entry < TableSize; Entry++) {
          if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
            (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_SATA) &&
            (PchSetup->sSataTopology[Index] == PtssTables[Entry].Topology) &&
            (PtssTables[Entry].BoardId == TypePlatformUnknown)) {
            if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD20) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0) == (UINT32) B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0)) {
              PchPolicy->HsiosSataConfig.PortLane[Index].HsioRxGen3EqBoostMagEnable = TRUE;
              PchPolicy->HsiosSataConfig.PortLane[Index].HsioRxGen3EqBoostMag = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0;
            } else if (PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_TX_DWORD8) {
              if (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) {
                PchPolicy->HsiosSataConfig.PortLane[Index].HsioTxGen1DownscaleAmpEnable = TRUE;
                PchPolicy->HsiosSataConfig.PortLane[Index].HsioTxGen1DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
              }
              if (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) {
                PchPolicy->HsiosSataConfig.PortLane[Index].HsioTxGen2DownscaleAmpEnable = TRUE;
                PchPolicy->HsiosSataConfig.PortLane[Index].HsioTxGen2DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
              }
            }
            else {
              DEBUG ((DEBUG_ERROR, "ERROR! PTSS programming: The PTSS table offset and/or mask are not compatible with the BIOS Code.\n"));
            }
          }
        }
      }
    }

    for (Index = 0; Index < MaxPciePorts; Index++) {
      if (PchGetPcieLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
        for (Entry = 0; Entry < TableSize; Entry++) {
          // Skip matching Lanes when the table record has settings for WM20 FIA
          if ((PtssTables[Entry].PtssTable.SbPortID == PID_MODPHY4) ||
            (PtssTables[Entry].PtssTable.SbPortID == PID_MODPHY5)){
            continue;
          }
          if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
            (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) &&
            (PcieTopologyReal[Index] == PtssTables[Entry].Topology) &&
            (PtssTables[Entry].BoardId == TypePlatformUnknown)) {
            if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD25) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0) == (UINT32) B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0)) {
              PchPolicy->HsioPcieConfig.Lane[Index].HsioRxSetCtleEnable = TRUE;
              PchPolicy->HsioPcieConfig.Lane[Index].HsioRxSetCtle = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0;
            }
            else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD39) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0) == (UINT32) B_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0)) {
              PchPolicy->HsioPcieConfig.Lane[Index].HsioIcfgAdjLimitLoEnable = TRUE;
              PchPolicy->HsioPcieConfig.Lane[Index].HsioIcfgAdjLimitLo = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0;
            } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD40) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP) == (UINT32) B_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP)) {
              PchPolicy->HsioPcieConfig.Lane[Index].HsioSampOffstEvenErrSpEnable = TRUE;
              PchPolicy->HsioPcieConfig.Lane[Index].HsioSampOffstEvenErrSp = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP;
            } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD41) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS) == (UINT32) B_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS)) {
              PchPolicy->HsioPcieConfig.Lane[Index].HsioRemainingSamplerOffEnable = TRUE;
              PchPolicy->HsioPcieConfig.Lane[Index].HsioRemainingSamplerOff = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS;
            } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD7) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL) == (UINT32) B_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL)) {
              PchPolicy->HsioPcieConfig.Lane[Index].HsioVgaGainCalEnable = TRUE;
              PchPolicy->HsioPcieConfig.Lane[Index].HsioVgaGainCal = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL;
            } else {
              DEBUG ((DEBUG_ERROR, "ERROR! PTSS programming: The PTSS table offset and/or mask are not compatible with the BIOS Code.\n"));
            }
          }
        }
      }
    }

    //Continued for PCIe ports in WM20 FIA
    for (Index = 0; Index < PCH_MAX_WM20_LANES_NUMBER; Index++) {
        LaneNum = Index;
        for (Entry = 0; Entry < TableSize; Entry++) {
          // Skip entries which are not for WM20 FIA
          if ((PtssTables[Entry].PtssTable.SbPortID != PID_MODPHY4) &&
            (PtssTables[Entry].PtssTable.SbPortID != PID_MODPHY5)){
            continue;
          }
          if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
            (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) &&
            (PtssTables[Entry].BoardId == TypePlatformUnknown)) {
            if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD25) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0) == (UINT32) B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0)) {
              PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioRxSetCtleEnable = TRUE;
              PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioRxSetCtle = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0;
            } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD39) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0) == (UINT32) B_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0)) {
              PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioIcfgAdjLimitLoEnable = TRUE;
              PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioIcfgAdjLimitLo = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0;
            } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD40) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP) == (UINT32) B_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP)) {
              PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioSampOffstEvenErrSpEnable = TRUE;
              PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioSampOffstEvenErrSp = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP;
            } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD41) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS) == (UINT32) B_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS)) {
              PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioRemainingSamplerOffEnable = TRUE;
              PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioRemainingSamplerOff = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS;
            } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD7) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL) == (UINT32) B_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL)) {
              PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioVgaGainCalEnable = TRUE;
              PchPolicy->HsioPcieConfigFIAWM20.Lane[Index].HsioVgaGainCal = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL;
            } else {
              DEBUG ((DEBUG_ERROR, "ERROR! PTSS programming: The PTSS table offset and/or mask are not compatible with the BIOS Code.\n"));
            }
          }
        }
    }

  }
}


BOOLEAN
PcieSataPortSelectionWA(
  VOID
  )
{
  if ((SystemBoardIdValue() == TypeNeonCityEPRP || SystemBoardIdValue() == TypeKyanite || SystemBoardIdValue() == TypeNeonCityFPGA)
  && (SystemBoardRevIdValue() == 0 )
  ) {
    return TRUE;
  } else {
    return FALSE;
  }
}

UINT8
GetBmcPciePort(
  VOID
  )
{
  return PcdGet8(PcdOemSkuBmcPciePortNumber);
}


/**
  Gets FPGA BBS index and card ID according to the HSSI card type

  @param SocketNum           Socket Number
  @param HssiCardBbsIndex    HSSI card BBS Index
  @param HssiCardID          HSSI card ID

  @retval EFI_SUCCESS:       The function executed successfully
  @retval Others             Internal error when execute the function
**/
EFI_STATUS
GetFpgaHssiCardBbsInfo (
  IN      UINT8 SocketNum,
  OUT     UINT8 *HssiCardBbsIndex,
  OUT     UINT8 *HssiCardID
  )
{
  UINT8                 BoardId;
  UINT8                 i,j;
  FPGA_HSSI_CARD_ID     HssiId;
  UINT32                Data32 = 0;
  EFI_STATUS            Status;

  if ((HssiCardBbsIndex == NULL) || (HssiCardID == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  BoardId = SystemBoardIdValue(); 

  //
  // Read the RC_CARD_ID[2][1][0] to selecte Bit Stream Guid Index
  //
  HssiId.Data       = 0;
  *HssiCardBbsIndex = BBS_None;
  *HssiCardID       = 0;

  for (i = 0; i < sizeof(mNeoCityFpgaHssiCardData) / sizeof(FPGA_HSSI_DATA); i++) {
    
    if ((mNeoCityFpgaHssiCardData[i].FpgaSktIndex == SocketNum) && (mNeoCityFpgaHssiCardData[i].BoardId == BoardId)) {        

      Status = GpioGetInputValue (mNeoCityFpgaHssiCardData[i].GpioHssiCardID0,   &Data32);  // FM_RC_CARD_ID0
      if (!EFI_ERROR(Status)) {
        DEBUG ((EFI_D_ERROR, "Socket[%X] GpioHssiCardID0 = %X\n", SocketNum, Data32));    
        HssiId.Bits.CardID0 = (UINT8)Data32;
      }
      Status = GpioGetInputValue (mNeoCityFpgaHssiCardData[i].GpioHssiCardID1,   &Data32);  // FM_RC_CARD_ID1
      if (!EFI_ERROR(Status)) {
        DEBUG ((EFI_D_ERROR, "Socket[%X] GpioHssiCardID1 = %X\n", SocketNum, Data32));        
        HssiId.Bits.CardID1 = (UINT8)Data32;
      }
      Status = GpioGetInputValue (mNeoCityFpgaHssiCardData[i].GpioHssiCardID2,   &Data32);  // FM_RC_CARD_ID2
      if (!EFI_ERROR(Status)) {
        DEBUG ((EFI_D_ERROR, "Socket[%X] GpioHssiCardID2 = %X\n", SocketNum, Data32));         
        HssiId.Bits.CardID2 = (UINT8)Data32;
      }

      for (j = 0; j < sizeof(mNeoCityFpgaHssiBBSMap) / sizeof(FPGA_HSSI_BBS_Map); j++) {
        if ((mNeoCityFpgaHssiBBSMap[j].BoardId == BoardId) && (mNeoCityFpgaHssiBBSMap[j].HssiCardID == HssiId.Data)) { 
          *HssiCardBbsIndex = mNeoCityFpgaHssiBBSMap[j].BBStype;
          *HssiCardID = mNeoCityFpgaHssiBBSMap[j].HssiCardID;

          return EFI_SUCCESS;
        }
      }     
    }
  }

  return EFI_NOT_FOUND;
}

/*
  Sets FPGA Power Control GPIO

  @param SocketNum           Socket Number
    
  @retval EFI_SUCCESS        Operation completed successfully.
  @retval Otherwise          Operation failed.

*/
EFI_STATUS
SetFpgaGpioOff (
  IN UINT8 SocketNum
  )
{
  UINT8         BoardId;
  GPIO_PAD      GpioNum = 0xFFFFFFFF;
  EFI_STATUS    Status = EFI_UNSUPPORTED;
  UINT8         i;
  BoardId = SystemBoardIdValue();

  for(i = 0; i < sizeof (mFpgaPowerOffGpioTable)/sizeof (FPGA_SOCKET_POWEROFF_MAP); i++) {
    if ((mFpgaPowerOffGpioTable[i].BoardId == BoardId) && (mFpgaPowerOffGpioTable[i].FpgaSktIndex == SocketNum)) {
      GpioNum = mFpgaPowerOffGpioTable[i].FpgaPowerOffGpio;
      break;        
    }
  }

  if (GpioNum != 0xFFFFFFFF ) {   
    //
    // Set the GPIO to low to turn off power.
    //
    GpioSetOutputValue (GpioNum, 0);
    Status = EFI_SUCCESS;
  }

  return Status;
}

/**
  Gets FPGA default BBS index 

  @param SocketNum           Socket Number

  @retval  Bbs Index

**/
UINT8
GetDefaultFpgaBbsIndex (
  IN UINT8 SocketNum
  )
{
  UINT8 BoardId;
  UINT8 i;
  BoardId = SystemBoardIdValue();

  for(i = 0; i < sizeof (mDefaultPlatformBbbMap)/sizeof (FPGA_PLATFORM_BBS_MAP); i++) {
    if ((mDefaultPlatformBbbMap[i].BoardId == BoardId) && (mDefaultPlatformBbbMap[i].FpgaSktIndex == SocketNum)) {      
      return mDefaultPlatformBbbMap[i].Bbstype;
    }
  }
  
  return BBS_None;
}
