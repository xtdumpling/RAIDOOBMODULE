/** @file
  PCH GPIO Native function group and pad numbers

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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
#ifndef _PCH_GPIO_NATIVE_PADS_H_
#define _PCH_GPIO_NATIVE_PADS_H_

//
// SATA
//
//
// GPIO SATA0GP is the Sata port 0 reset pin.
//
//#define PCH_GPIO_SATA_PORT0_RESET          21  // Old LPT implementation
#define PCH_H_GPIO_SATA_PORT0_RESET_GROUP    PchHGpio_GPP_E
#define PCH_H_GPIO_SATA_PORT0_RESET_PAD      0

//#define PCH_LP_GPIO_SATA_PORT0_RESET       (R_PCH_GP_N_CONFIG0 + (34 * 0x08))  // Old LPT implementation
#define PCH_LP_GPIO_SATA_PORT0_RESET_GROUP   PchLpGpio_GPP_E
#define PCH_LP_GPIO_SATA_PORT0_RESET_PAD     0
//
// GPIO SATA1GP is the Sata port 1 reset pin.
//
//#define PCH_GPIO_SATA_PORT1_RESET          19  // Old LPT implementation
#define PCH_H_GPIO_SATA_PORT1_RESET_GROUP    PchHGpio_GPP_E
#define PCH_H_GPIO_SATA_PORT1_RESET_PAD      1
//#define PCH_LP_GPIO_SATA_PORT1_RESET       (R_PCH_GP_N_CONFIG0 + (35 * 0x08))  // Old LPT implementation
#define PCH_LP_GPIO_SATA_PORT1_RESET_GROUP   PchLpGpio_GPP_E
#define PCH_LP_GPIO_SATA_PORT1_RESET_PAD     1

//
// GPIO SATA2GP is the Sata port 2 reset pin.
//
//#define PCH_GPIO_SATA_PORT2_RESET          36  // Old LPT implementation
#define PCH_H_GPIO_SATA_PORT2_RESET_GROUP    PchHGpio_GPP_E
#define PCH_H_GPIO_SATA_PORT2_RESET_PAD      2
//#define PCH_LP_GPIO_SATA_PORT2_RESET       (R_PCH_GP_N_CONFIG0 + (36 * 0x08))  // Old LPT implementation
#define PCH_LP_GPIO_SATA_PORT2_RESET_GROUP   PchLpGpio_GPP_E
#define PCH_LP_GPIO_SATA_PORT2_RESET_PAD     2

//
// GPIO SATA3GP is the Sata port 3 reset pin.
//
//#define PCH_GPIO_SATA_PORT3_RESET          37  // Old LPT implementation
#define PCH_H_GPIO_SATA_PORT3_RESET_GROUP    0
#define PCH_H_GPIO_SATA_PORT3_RESET_PAD      0
//#define PCH_LP_GPIO_SATA_PORT3_RESET       (R_PCH_GP_N_CONFIG0 + (37 * 0x08))  // Old LPT implementation
#define PCH_LP_GPIO_SATA_PORT3_RESET_GROUP   0
#define PCH_LP_GPIO_SATA_PORT3_RESET_PAD     0

//
// GPIO SATA4GP is the Sata port 4 reset pin.
//
//#define PCH_GPIO_SATA_PORT4_RESET          16  // Old LPT implementation
#define PCH_H_GPIO_SATA_PORT4_RESET_GROUP    0
#define PCH_H_GPIO_SATA_PORT4_RESET_PAD      0
//
// GPIO SATA5GP is the Sata port 5 reset pin.
//
//#define PCH_GPIO_SATA_PORT5_RESET          49  // Old LPT implementation
#define PCH_H_GPIO_SATA_PORT5_RESET_GROUP    0
#define PCH_H_GPIO_SATA_PORT5_RESET_PAD      0
//
// GPIO SATA5GP is the Sata port 5 reset pin.
//
//#define PCH_GPIO_SATA_PORT6_RESET          49  // Old LPT implementation
#define PCH_H_GPIO_SATA_PORT6_RESET_GROUP    0
#define PCH_H_GPIO_SATA_PORT6_RESET_PAD      0
//
// GPIO SATA5GP is the Sata port 5 reset pin.
//
//#define PCH_GPIO_SATA_PORT7_RESET          49  // Old LPT implementation
#define PCH_H_GPIO_SATA_PORT7_RESET_GROUP    0
#define PCH_H_GPIO_SATA_PORT7_RESET_PAD      0


//
// Serial GPIO feature
//
#define PCH_H_GPIO_SERIAL_BLINK_GROUP        PchHGpio_GPP_D
#define PCH_LP_GPIO_SERIAL_BLINK_GROUP       PchLpGpio_GPP_D
#define PCH_GPIO_SERIAL_BLINK_MAX_PIN        4 // GPP_D_0-4 support Serial Blink feature

//
// PCIe
//
#define PCH_LP_GPIO_PCIE_SRCCLKREQB_GROUP    PchLpGpio_GPP_B
#define PCH_GPIO_PCIE_SRCCLKREQB_0_PAD       5
#define PCH_GPIO_PCIE_SRCCLKREQB_1_PAD       6
#define PCH_GPIO_PCIE_SRCCLKREQB_2_PAD       7
#define PCH_GPIO_PCIE_SRCCLKREQB_3_PAD       8
#define PCH_GPIO_PCIE_SRCCLKREQB_4_PAD       9
#define PCH_GPIO_PCIE_SRCCLKREQB_5_PAD       10

//
// Thermal
//
#define PCH_H_GPIO_PCHHOT_GROUP              PchHGpio_GPP_B
#define PCH_LP_GPIO_PCHHOT_GROUP             PchLpGpio_GPP_B
#define PCH_GPIO_PCHHOT_PAD                  23
#define PCH_GPIO_PAD_MODE_PCHHOT             V_PCH_GPIO_PAD_MODE_NAT_2

//
// gSPI chip select
//
#define PCH_H_GPIO_GSPIX_CSB_GROUP           PchHGpio_GPP_B
#define PCH_LP_GPIO_GSPIX_CSB_GROUP          PchLpGpio_GPP_B
#define PCH_GPIO_GSPI0_CSB_PAD               15
#define PCH_GPIO_GSPI1_CSB_PAD               19

#endif
