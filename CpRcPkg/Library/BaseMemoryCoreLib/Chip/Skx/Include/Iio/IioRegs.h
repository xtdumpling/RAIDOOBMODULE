//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  2010 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioRegs.h

  IIO constants.

Revision History:

**/

#ifndef _IIO_REGS_H_
#define _IIO_REGS_H_

/**
==================================================================================================
==================================  General Defintions          ==================================
==================================================================================================
**/

#define NUMBER_PORTS_PER_SOCKET       21
#define IIO_UPLINK_PORT_INDEX         5    //Port 2A is the uplink port in Neon-City ///TODO Check if this is required for SKX/Purley SKX_TTEST
#define MaxIIO                        MAX_SOCKET

#if MAX_SOCKET > 4
#define TOTAL_CB3_DEVICES             64   // Todo Check SKX CB3 devices (IOAT_TOTAL_FUNCS * MAX_SOCKET). Note: this covers up to 8S.
#define MAX_TOTAL_PORTS               168   //NUMBER_PORTS_PER_SOCKET * MaxIIO. As now, treats setup S0-S3 = S4_S7 as optimal
#else
#define TOTAL_CB3_DEVICES             32   // Todo Check SKX CB3 devices.
#define MAX_TOTAL_PORTS               84   //NUMBER_PORTS_PER_SOCKET * MaxIIO
#endif

#if MAX_SOCKET > 4
#define TOTAL_IIO_STACKS              48   // MAX_SOCKET * MAX_IIO_STACK. Not reflect architecture but only sysHost structure!
#define TOTAL_SYSTEM_IIO_STACKS       32   // In term of system architecture support
#else
#define TOTAL_IIO_STACKS              24   // MAX_SOCKET * MAX_IIO_STACK
#define TOTAL_SYSTEM_IIO_STACKS       24   // In term of system architecture support
#endif

#define NUMBER_NTB_PORTS_PER_SOCKET   3
#if MAX_SOCKET > 4
#define MAX_NTB_PORTS                 24   // NUMBER_NTB_PORTS_PER_SOCKET * MAX_SOCKET
#else
#define MAX_NTB_PORTS                 12   // NUMBER_NTB_PORTS_PER_SOCKET * MAX_SOCKET
#endif
#define VMD_STACK_PER_SOCKET          3
#define VMD_PORT_PER_STACK            4
#define VMD_PORTS_PER_SOCKET          12
#if MAX_SOCKET > 4
#define MAX_VMD_PORTS                 96   // VMD_PORTS_PER_SOCKET * MAX_SOCKET
#define MAX_VMD_STACKS                24   // VMD_STACK_PER_SOCKET * MAX_SOCKET
#else
#define MAX_VMD_PORTS                 48   // VMD_PORTS_PER_SOCKET * MAX_SOCKET
#define MAX_VMD_STACKS                12   // VMD_STACK_PER_SOCKET * MAX_SOCKET
#endif


#define VARIABLE_FUNC3_ELEMENTS       4
#if MAX_SOCKET > 4
#define MAX_TOTAL_CORE_HIDE           32   //(MAX_SOCKET * VARIABLE_FUNC3_ELEMENTS)
#else
#define MAX_TOTAL_CORE_HIDE           16   //(MAX_SOCKET * VARIABLE_FUNC3_ELEMENTS)
#endif

#define MAX_IOU_PORT_DEVICES          4


/**
==================================================================================================
==================================  IIO Root Port Defintions              ====================
==================================================================================================
**/
typedef enum {
  IioPortA = 0,
  IioPortB = 1,
  IioPortC = 2,
  IioPortD = 3
}IIOPORTS;

typedef enum {
 IioIou0 = 0,
 IioIou1,
 IioIou2,
 IioMcp0,
 IioMcp1,
 IioIouMax
} IIOIOUS;

// HSD 4756772 added start - Moved from OemIioInit.c in OemIioHooksLib
//
// Bifurcation control register shorthand
//
#define IIO_BIFURCATE_AUTO      0xFF

// Ports 1D-1A, 2D-2A, 3D-3A
//
#define IIO_BIFURCATE_x4x4x4x4  0
#define IIO_BIFURCATE_x4x4xxx8  1
#define IIO_BIFURCATE_xxx8x4x4  2
#define IIO_BIFURCATE_xxx8xxx8  3
#define IIO_BIFURCATE_xxxxxx16  4
#define IIO_BIFURCATE_xxxxxxxx  0xF

#define PORT_0_INDEX             0
#define PCIE_PORT_2_DEV          0x02

// IOU2
#define PORT_1A_INDEX            1
#define PORT_1B_INDEX            2
#define PORT_1C_INDEX            3
#define PORT_1D_INDEX            4
// IOU0
#define PORT_2A_INDEX            5
#define PORT_2B_INDEX            6
#define PORT_2C_INDEX            7
#define PORT_2D_INDEX            8
// IOU1
#define PORT_3A_INDEX            9
#define PORT_3B_INDEX            10
#define PORT_3C_INDEX            11
#define PORT_3D_INDEX            12
//MCP0
#define PORT_4A_INDEX            13
#define PORT_4B_INDEX            14
#define PORT_4C_INDEX            15
#define PORT_4D_INDEX            16
//MCP1
#define PORT_5A_INDEX            17
#define PORT_5B_INDEX            18
#define PORT_5C_INDEX            19
#define PORT_5D_INDEX            20

//
#define SOCKET_0_INDEX           0
#define SOCKET_1_INDEX           21
#define SOCKET_2_INDEX           42
#define SOCKET_3_INDEX           63
#define SOCKET_4_INDEX           84
#define SOCKET_5_INDEX           105
#define SOCKET_6_INDEX           126
#define SOCKET_7_INDEX           147

#define PCIE_PORT_0_DEV     0x00
#define PCIE_PORT_0_FUNC    0x00

#define PCIE_PORT_1A_DEV    0x00
#define PCIE_PORT_1B_DEV    0x01
#define PCIE_PORT_1C_DEV    0x02
#define PCIE_PORT_1D_DEV    0x03
#define PCIE_PORT_1A_FUNC   0x00
#define PCIE_PORT_1B_FUNC   0x00
#define PCIE_PORT_1C_FUNC   0x00
#define PCIE_PORT_1D_FUNC   0x00

#define PCIE_PORT_2A_DEV    0x00
#define PCIE_PORT_2B_DEV    0x01
#define PCIE_PORT_2C_DEV    0x02
#define PCIE_PORT_2D_DEV    0x03
#define PCIE_PORT_2A_FUNC   0x00
#define PCIE_PORT_2B_FUNC   0x00
#define PCIE_PORT_2C_FUNC   0x00
#define PCIE_PORT_2D_FUNC   0x00

#define PCIE_PORT_3A_DEV    0x00
#define PCIE_PORT_3B_DEV    0x01
#define PCIE_PORT_3C_DEV    0x02
#define PCIE_PORT_3D_DEV    0x03
#define PCIE_PORT_3A_FUNC   0x00
#define PCIE_PORT_3B_FUNC   0x00
#define PCIE_PORT_3C_FUNC   0x00
#define PCIE_PORT_3D_FUNC   0x00

#define PCIE_PORT_4A_DEV    0x00
#define PCIE_PORT_4B_DEV    0x01
#define PCIE_PORT_4C_DEV    0x02
#define PCIE_PORT_4D_DEV    0x03
#define PCIE_PORT_4A_FUNC   0x00
#define PCIE_PORT_4B_FUNC   0x00
#define PCIE_PORT_4C_FUNC   0x00
#define PCIE_PORT_4D_FUNC   0x00

#define PCIE_PORT_5A_DEV    0x00
#define PCIE_PORT_5B_DEV    0x01
#define PCIE_PORT_5C_DEV    0x02
#define PCIE_PORT_5D_DEV    0x03
#define PCIE_PORT_5A_FUNC   0x00
#define PCIE_PORT_5B_FUNC   0x00
#define PCIE_PORT_5C_FUNC   0x00
#define PCIE_PORT_5D_FUNC   0x00

#define PCIE_PORT_GLOBAL_DFX_DEV     0x07
#define PCIE_PORT_GLOBAL_DFX_FUNC    0x07

#define PCIE_PORT_0_DFX_DEV     0x07
#define PCIE_PORT_0_DFX_FUNC    0x00

#define PCIE_PORT_1A_DFX_DEV    0x07
#define PCIE_PORT_1A_DFX_FUNC   0x00
#define PCIE_PORT_1B_DFX_DEV    0x07
#define PCIE_PORT_1B_DFX_FUNC   0x01
#define PCIE_PORT_1C_DFX_DEV    0x07
#define PCIE_PORT_1C_DFX_FUNC   0x02
#define PCIE_PORT_1D_DFX_DEV    0x07
#define PCIE_PORT_1D_DFX_FUNC   0x03


#define PCIE_PORT_2A_DFX_DEV    0x07
#define PCIE_PORT_2A_DFX_FUNC   0x00
#define PCIE_PORT_2B_DFX_DEV    0x07
#define PCIE_PORT_2B_DFX_FUNC   0x01
#define PCIE_PORT_2C_DFX_DEV    0x07
#define PCIE_PORT_2C_DFX_FUNC   0x02
#define PCIE_PORT_2D_DFX_DEV    0x07
#define PCIE_PORT_2D_DFX_FUNC   0x03

#define PCIE_PORT_3A_DFX_DEV    0x07
#define PCIE_PORT_3A_DFX_FUNC   0x00
#define PCIE_PORT_3B_DFX_DEV    0x07
#define PCIE_PORT_3B_DFX_FUNC   0x01
#define PCIE_PORT_3C_DFX_DEV    0x07
#define PCIE_PORT_3C_DFX_FUNC   0x02
#define PCIE_PORT_3D_DFX_DEV    0x07
#define PCIE_PORT_3D_DFX_FUNC   0x03

#define PCIE_PORT_4A_DFX_DEV    0x07
#define PCIE_PORT_4A_DFX_FUNC   0x00
#define PCIE_PORT_4B_DFX_DEV    0x07
#define PCIE_PORT_4B_DFX_FUNC   0x01
#define PCIE_PORT_4C_DFX_DEV    0x07
#define PCIE_PORT_4C_DFX_FUNC   0x02
#define PCIE_PORT_4D_DFX_DEV    0x07
#define PCIE_PORT_4D_DFX_FUNC   0x03

#define PCIE_PORT_5A_DFX_DEV    0x07
#define PCIE_PORT_5A_DFX_FUNC   0x00
#define PCIE_PORT_5B_DFX_DEV    0x07
#define PCIE_PORT_5B_DFX_FUNC   0x01
#define PCIE_PORT_5C_DFX_DEV    0x07
#define PCIE_PORT_5C_DFX_FUNC   0x02
#define PCIE_PORT_5D_DFX_DEV    0x07
#define PCIE_PORT_5D_DFX_FUNC   0x03

#define PCIE_PORT_0_LINK_SEL  0x00
#define PCIE_PORT_1A_LINK_SEL  0x00
#define PCIE_PORT_1B_LINK_SEL  0x01
#define PCIE_PORT_1C_LINK_SEL  0x02
#define PCIE_PORT_1D_LINK_SEL  0x03
#define PCIE_PORT_2A_LINK_SEL  0x00
#define PCIE_PORT_2B_LINK_SEL  0x01
#define PCIE_PORT_2C_LINK_SEL  0x02
#define PCIE_PORT_2D_LINK_SEL  0x03
#define PCIE_PORT_3A_LINK_SEL  0x00
#define PCIE_PORT_3B_LINK_SEL  0x01
#define PCIE_PORT_3C_LINK_SEL  0x02
#define PCIE_PORT_3D_LINK_SEL  0x03
#define PCIE_PORT_4A_LINK_SEL  0x00
#define PCIE_PORT_4B_LINK_SEL  0x01
#define PCIE_PORT_4C_LINK_SEL  0x02
#define PCIE_PORT_4D_LINK_SEL  0x03
#define PCIE_PORT_5A_LINK_SEL  0x00
#define PCIE_PORT_5B_LINK_SEL  0x01
#define PCIE_PORT_5C_LINK_SEL  0x02
#define PCIE_PORT_5D_LINK_SEL  0x03

#define PCIE_PORT_0_SUPER_CLUSTER_PORT  0x00
#define PCIE_PORT_1A_SUPER_CLUSTER_PORT  0x01
#define PCIE_PORT_1B_SUPER_CLUSTER_PORT  0x01
#define PCIE_PORT_1C_SUPER_CLUSTER_PORT  0x01
#define PCIE_PORT_1D_SUPER_CLUSTER_PORT  0x01
#define PCIE_PORT_2A_SUPER_CLUSTER_PORT  0x05
#define PCIE_PORT_2B_SUPER_CLUSTER_PORT  0x05
#define PCIE_PORT_2C_SUPER_CLUSTER_PORT  0x05
#define PCIE_PORT_2D_SUPER_CLUSTER_PORT  0x05
#define PCIE_PORT_3A_SUPER_CLUSTER_PORT  0x09
#define PCIE_PORT_3B_SUPER_CLUSTER_PORT  0x09
#define PCIE_PORT_3C_SUPER_CLUSTER_PORT  0x09
#define PCIE_PORT_3D_SUPER_CLUSTER_PORT  0x09
#define PCIE_PORT_4A_SUPER_CLUSTER_PORT  0x0D
#define PCIE_PORT_4B_SUPER_CLUSTER_PORT  0x0D
#define PCIE_PORT_4C_SUPER_CLUSTER_PORT  0x0D
#define PCIE_PORT_4D_SUPER_CLUSTER_PORT  0x0D
#define PCIE_PORT_5A_SUPER_CLUSTER_PORT  0x11
#define PCIE_PORT_5B_SUPER_CLUSTER_PORT  0x11
#define PCIE_PORT_5C_SUPER_CLUSTER_PORT  0x11
#define PCIE_PORT_5D_SUPER_CLUSTER_PORT  0x11

#define PORT_LINK_WIDTH_x16     16
#define PORT_LINK_WIDTH_x8      8
#define PORT_LINK_WIDTH_x4      4
#define PORT_LINK_WIDTH_x2      2
#define PORT_LINK_WIDTH_x1      1

#define R_PCIE_PORT_PCICMD                        0x04
#define   B_PCIE_PORT_PCICMD_PERRE                  BIT6
#define   B_PCIE_PORT_PCICMD_SERRE                  BIT8
#define   B_PCIE_PORT_PCICMD_INTDIS                 BIT10
#define R_PCIE_MISCCTRLSTS                        0x188

//
// Port Config Mode
//
#define REGULAR_PCIE_OWNERSHIP        0
#define PCIE_PORT_REGULAR_MODE        1
#define PCIE_PORT_NTB_MODE            2
#define VMD_OWNERSHIP                 3
#define PCIEAIC_OCL_OWNERSHIP         4

/**
==================================================================================================
==================================  Multi-Cast Defintions      ====================
==================================================================================================
**/
#define MCAST_MEM_AUTO      0x0
#define MCAST_MEM_BELOW_4G  0x1
//#define MCAST_MEM_ABOVE_4G  0x2

#define MC_INDEX_POS_MASK   0x3F

#define MC_ENABLE           BIT15
#define MC_NUM_GROUP_MASK   0x3F

#define MC_RCV_ENABLE_BITS  BIT1

typedef enum {
  MCAST_RET_SUCCESS = 0,
  MCAST_ERR_NOT_SUPPORTED,
  MCAST_ERR_MEM_ALLOC_FAIL,
  MCAST_ERR_INVALID_CONFIG,
} MCAST_RETURN_STATUS;

/**
==================================================================================================
==================================  DMI Registers  Defintions      ====================
==================================================================================================
**/
#define DMI_BUS_NUM                   0
#define DMI_DEV_NUM                   0
#define DMI_FUNC_NUM                  0

#define R_PCIE_PORT_DMIRCBAR                      0x50            // DMI Root Complex Block Base Address
#define   V_DMIRCBA_SIZE                            (4*1024)      // DMI RCBA size is 4KB with Natural Alignment
#define   B_PCIE_PORT_DMIRCBAR_EN_SET              BIT0           // Enable changing for DMI Bar, must unset before LT Lock
#define R_PCIE_PORT_LNKCON2                       0xFC            // PCI Express Link Control 2 Register  

#define R_DMI_PORT_PXPLNKCTRL                    0x1B0
#define   B_DMI_PORT_PXPLNKCTRL_EXT_SYNCH          BIT7
#define   B_DMI_PORT_PXPLNKCTRL_CCCON              BIT6
#define   B_DMI_PORT_PXPLNKCTRL_RETRAIN_LINK       BIT5
#define   B_DMI_PORT_PXPLNKCTRL_LNKDIS             BIT4
#define R_DMI_PORT_PXPLNKSTS                     0x1B2
#define   B_DMI_PORT_PXPLNKSTS_LABS                BIT15
#define   B_DMI_PORT_PXPLNKSTS_LBMS                BIT14
#define   B_DMI_PORT_PXPLNKSTS_DLLA                BIT13
#define   B_DMI_PORT_PXPLNKSTS_SCCON               BIT12
#define   B_DMI_PORT_PXPLNKSTS_LINKTRAIN           BIT11
#define   B_DMI_PORT_PXPLNKSTS_NLNKWD              (BIT4 | BIT5 | BIT6 | BIT7 | BIT8 | BIT9)
#define   B_DMI_PORT_PXPLNKSTS_LNKSPD              (BIT0 | BIT1 | BIT2 | BIT3)

#define R_DMI_PORT_PXPLNKCTRL2                   0x1C0
#define   B_DMI_PORT_PXPLNKCTRL2_DEEMPH            BIT6
#define   B_DMI_PORT_PXPLNKCTRL2_ENCmpl            BIT4
#define   B_DMI_PORT_PXPLNKCTRL2_LNKSPEED          (BIT3 | BIT2 | BIT1 | BIT0)
#define     V_DMI_PORT_PXPLNKCTRL2_LS_GEN3           0x03
#define     V_DMI_PORT_PXPLNKCTRL2_LS_GEN2           0x02
#define     V_DMI_PORT_PXPLNKCTRL2_LS_GEN1           0x01

#define R_XPUNCERRSTS                           0x208
#define RECEIVED_PCIE_COMPELTION_WITH_UR_STATUS BIT6



/**
==================================================================================================
==================================              VC/TC  Mapping            ========================
==================================================================================================
**/
#define R_IIO_RCRB_DMIPVCCAP1                              0x04          // DMI Port Capability Register 1
#define R_IIO_RCRB_DMIVC0RCAP                              0x10          // DMI VC0 Resource Capability
#define R_IIO_RCRB_DMIVC0RCTL                              0x14          // DMI VC0 Resource Control
#define   B_IIO_RCRB_DMIVCxRCTL_VCx_EN                       BIT31       // Virtual Channel x Enable (VCxE)
#define   B_IIO_RCRB_DMIVCxRCTL_VCx_ID_MASK                  (7 << 24) // Bit[26:24]
#define   B_IIO_RCRB_DMIVCxRCTL_VCx_TCVCxM_MASK               0xFE
#define R_IIO_RCRB_DMIVC0RSTS                              0x1A          // DMI VC0 Resource Status
#define   B_IIO_RCRB_DMIVCxRSTS_VCx_NP                       BIT1        // Virtual Channel x Negotiation Pending (VCxNP)
#define R_IIO_RCRB_DMIVC1RCAP                              0x1C          // DMI VC1 Resource Capability
#define R_IIO_RCRB_DMIVC1RCTL                              0x20          // DMI VC1 Resource Control
#define R_IIO_RCRB_DMIVC1RSTS                              0x26          // DMI VC1 Resource Status
#define R_IIO_RCRB_DMIVCMRCAP                              0x34          // DMI VCM Resource Capability
#define R_IIO_RCRB_DMIVCMRCTL                              0x38          // DMI VCP Resource Control
#define R_IIO_RCRB_DMIVCMRSTS                              0x3E          // DMI VCM Resource Status
#define R_IIO_RCRB_DMILCTRL                                0x88          // DMI Link control
#define R_IIO_RCRB_DMILSTS                                 0x8A          // DMI Link status



/**
==================================================================================================
==================================  PCIE Registers Defintions     ====================
==================================================================================================
**/

// NOTE: Although not really applicaple for most UP/DP configuration the DMI port could be configured 
//       as a PCIE port.  In that configuration.

#define PCI_EXPRESS_BASE_ADDRESS  ((VOID *) (UINTN) 0x80000000)

#define ONLY_REGISTER_OFFSET(x)  (x & 0xFFF)

#define PCI_PCIE_ADDR(Bus,Device,Function,Offset) \
  (((Offset) & 0xfff) | (((Function) & 0x07) << 12) | (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

#define PCIESTACKPORTINDEX(port, stack, portIdx) \
  if(stack == 0) portIdx = port; \
  else if (stack == 1) portIdx = (port + stack); \
  else if (stack == 2) portIdx = (port + 5); \
  else if (stack == 3) portIdx = (port + 9); \
  else if (stack == 4) portIdx = (port + 13); \
  else if (stack == 5) portIdx = (port + 17);

#define MAX_PAYLOAD_SIZE_256B                     1
#define MAX_PAYLOAD_SIZE_128B                     0

#define TEMP_BUSBASE_OFFSET                       1

#define SECBUS_TEMP_DEV_NUM                       0
#define SECBUS_TEMP_FUNC_NUM                      0

#define R_PCIE_PORT_STATUS                        0x06
#define R_PCIE_PORT_IOBAS                         0x1C
#define   B_PCIE_PORT_MOREIO                        (BIT2|BIT3)
#define R_PCIE_PORT_CAPPTR                        0x34
#define R_PCIE_PORT_BCTRL                         0x3E
#define   B_PCIE_PORT_BCTRL_SBR                   BIT6
#define R_PCIE_PORT_SCAPID                        0x40
#define R_PCIE_PORT_SNXTPTR                       0x41
#define R_PCIE_PORT_SVID_OTHER                    0x44
#define R_PCIE_PORT_SID_OTHER                     0x46

#define R_PCIE_PORT_MSINXTPTR                     0x61
#define R_PCIE_PORT_MSICTRL                       0x62
#define   B_PCIE_PORT_MSICTRL_MSIEN                 BIT0
#define R_PCIE_PORT_MSIAR                         0x64
#define R_PCIE_PORT_MSGDAT                        0x68
#define R_PCIE_PORT_MSIMSK                        0x6C
#define R_PCIE_PORT_MSIPENDING                    0x70

#define R_PCIE_PORT_PXPNXTPTR                     0x91
#define R_PCIE_PORT_PXPCAP                        0x92
#define   B_PCIE_PORT_PXPCAP_SLOTIMPL               BIT8
#define   B_PCIE_PORT_PXPCAP_DPT                    (BIT4|BIT5|BIT6|BIT7)
#define R_PCIE_PORT_PXPDEVCAP                     0x94
#define R_PCIE_PORT_PXPDEVCTRL                    0x98
#define   B_PCIE_PORT_PXPDEVCTRL_ENNOSNP            BIT11
#define   B_PCIE_PORT_PXPDEVCTRL_MPS                (BIT5|BIT6|BIT7)
#define     V_PCIE_PORT_PXPDEVCTRL_MPS_128            (0<<5)
#define     V_PCIE_PORT_PXPDEVCTRL_MPS_256            (1<<5)
#define   B_PCIE_PORT_PXPDEVCTRL_CERE               BIT0
#define   B_PCIE_PORT_PXPDEVCTRL_NERE               BIT1
#define   B_PCIE_PORT_PXPDEVCTRL_FERE               BIT2
#define   B_PCIE_PORT_PXPDEVCTRL_URRE               BIT3
#define R_PCIE_PORT_PXPDEVSTS                     0x9A
#define   B_PCIE_PORT_PXPDEVCTRL_CERS               BIT0
#define   B_PCIE_PORT_PXPDEVCTRL_NERS               BIT1
#define   B_PCIE_PORT_PXPDEVCTRL_FERS               BIT2
#define   B_PCIE_PORT_PXPDEVSTS_UNSUPREQT           BIT3
#define R_PCIE_PORT_PXPLNKCAP                     0x9C
#define   B_PCIE_PORT_PXPLNKCAP_PORTNUM             (BIT31 | BIT30 | BIT29 | BIT28 |BIT27 | BIT26 | BIT25 | BIT24)
#define   B_PCIE_PORT_PXPLNKCAP_L1EXITL             (BIT17 | BIT16 | BIT15)
#define     V_PCIE_PORT_PXPLNKCAP_L1EXITL_SHIFT       15
#define   B_PCIE_PORT_PXPLNKCAP_L0SEXITL            (BIT14 | BIT13 | BIT12)
#define     V_PCIE_PORT_PXPLNKCAP_L0SEXITL_SHIFT      12
#define   B_PCIE_PORT_PXPLNKCAP_ASPMCTRL            (BIT10 | BIT11)
#define     B_PCIE_PORT_PXPLNKCAP_ASPMCTRL_L0S        (BIT0<<10)
#define     B_PCIE_PORT_PXPLNKCAP_ASPMCTRL_L1         (BIT1<<10)
#define   B_PCIE_PORT_PXPLNKCAP_MAXLNKWH            (BIT9 | BIT8 | BIT7 | BIT6 | BIT5  | BIT4)
#define   B_PCIE_PORT_PXPLNKCAP_LNKSPEED            (BIT3 | BIT2 | BIT1 | BIT0)
#define     V_PCIE_PORT_PXPLNKCAP_LS_GEN3             0x03
#define     V_PCIE_PORT_PXPLNKCAP_LS_GEN2             0x02
#define     V_PCIE_PORT_PXPLNKCAP_LS_GEN1             0x01
#define R_PCIE_PORT_PXPLNKCTRL                    0xA0
#define   B_PCIE_PORT_PXPLNKCTRL_EXT_SYNCH          BIT7
#define   B_PCIE_PORT_PXPLNKCTRL_CCCON              BIT6
#define   B_PCIE_PORT_PXPLNKCTRL_RETRAIN_LINK       BIT5
#define   B_PCIE_PORT_PXPLNKCTRL_LNKDIS             BIT4
#define R_PCIE_PORT_PXPLNKSTS                     0xA2
#define   B_PCIE_PORT_PXPLNKSTS_LABS                BIT15
#define   B_PCIE_PORT_PXPLNKSTS_LBMS                BIT14
#define   B_PCIE_PORT_PXPLNKSTS_DLLA                BIT13
#define   B_PCIE_PORT_PXPLNKSTS_SCCON               BIT12
#define   B_PCIE_PORT_PXPLNKSTS_NLNKWD              (BIT4 | BIT5 | BIT6 | BIT7 | BIT8 | BIT9)
#define   B_PCIE_PORT_PXPLNKSTS_LNKSPD              (BIT0 | BIT1 | BIT2 | BIT3)


#define R_PCIE_PORT_PXPSLOTCAP                    0xA4
#define   B_PCIE_PORT_PXPSLOTCAP_PSN                (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24)
#define   B_PCIE_PORT_PXPSLOTCAP_EIP                BIT17
#define   B_PCIE_PORT_PXPSLOTCAP_SPLS               (BIT16 | BIT15)
#define   B_PCIE_PORT_PXPSLOTCAP_SPLV               (BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7)
#define   B_PCIE_PORT_PXPSLOTCAP_HPC                BIT6
#define   B_PCIE_PORT_PXPSLOTCAP_HPS                BIT5
#define   B_PCIE_PORT_PXPSLOTCAP_PIP                BIT4
#define   B_PCIE_PORT_PXPSLOTCAP_AIP                BIT3
#define   B_PCIE_PORT_PXPSLOTCAP_MRLSP              BIT2
#define   B_PCIE_PORT_PXPSLOTCAP_PCP                BIT1
#define   B_PCIE_PORT_PXPSLOTCAP_ABP                BIT0

#define R_PCIE_PORT_PXPSLOTCTRL                   0xA8
#define   B_PCIE_PORT_PXPSLOTCTRL_PWRLED            (BIT8 | BIT9)
#define     V_PCIE_PORT_PXPSLOTCTRL_PWRLED_OFF        (3<<8)
#define     V_PCIE_PORT_PXPSLOTCTRL_PWRLED_BLINK      (2<<8)
#define     V_PCIE_PORT_PXPSLOTCTRL_PWRLED_ON         (1<<8)
#define   B_PCIE_PORT_PXPSLOTCTRL_PWRCTRL           BIT10
#define     V_PCIE_PORT_PXPSLOTCTRL_PWRCTRL_OFF       (1<<10)
#define     V_PCIE_PORT_PXPSLOTCTRL_PWRCTRL_ON        (0<<10)
#define   B_PCIE_PORT_PXPSLOTCTRL_ATNLED            (BIT6 | BIT7)
#define     V_PCIE_PORT_PXPSLOTCTRL_ATNLED_OFF        (3<<6)
#define     V_PCIE_PORT_PXPSLOTCTRL_ATNLED_BLINK      (2<<6)
#define     V_PCIE_PORT_PXPSLOTCTRL_ATNLED_ON         (1<<6)
#define   B_PCIE_PORT_PXPSLOTCTRL_HPINTEN           BIT5
#define   B_PCIE_PORT_PXPSLOTCTRL_CCIEN             BIT4
#define   B_PCIE_PORT_PXPSLOTCTRL_PRSINTEN          BIT3
#define   B_PCIE_PORT_PXPSLOTCTRL_MRLINTEN          BIT2
#define   B_PCIE_PORT_PXPSLOTCTRL_PWRINTEN          BIT1
#define   B_PCIE_PORT_PXPSLOTCTRL_ATNINTEN          BIT0

#define R_PCIE_PORT_PXPSLOTSTS                    0xAA
#define   B_PCIE_PORT_PXPSLOTSTS_DLLSCS             BIT8
#define   B_PCIE_PORT_PXPSLOTSTS_EMLS               BIT7
#define   B_PCIE_PORT_PXPSLOTSTS_PDS                BIT6
#define   B_PCIE_PORT_PXPSLOTSTS_MRLSS              BIT5
#define   B_PCIE_PORT_PXPSLOTSTS_CMDCOMP            BIT4
#define   B_PCIE_PORT_PXPSLOTSTS_PRSDC              BIT3
#define   B_PCIE_PORT_PXPSLOTSTS_MRLSC              BIT2
#define   B_PCIE_PORT_PXPSLOTSTS_PWRFD              BIT1
#define   B_PCIE_PORT_PXPSLOTSTS_ABP                BIT0

#define R_PCIE_PORT_ROOTCON                       0xAC
#define   B_PCIE_PORT_ROOTCON_CRSSWVISEN            BIT4
#define   B_PCIE_PORT_ROOTCON_SECE                  BIT0
#define   B_PCIE_PORT_ROOTCON_SENE                  BIT1
#define   B_PCIE_PORT_ROOTCON_SEFE                  BIT2

#define R_PCIE_PORT_PXPDEVCAP2                    0xB4
#define   B_PCIE_PORT_ARI_EN                        BIT5


#define R_PCIE_PORT_PXPDEVCTRL2                   0xB8
#define   B_PCIE_PORT_PXPDEVCTRL2_ARI               BIT5
#define   B_PCIE_PORT_PXPDEVCTRL2_CTPE              BIT4
#define     V_PCIE_PORT_PXPDEVCTRL2_CTPE_EN           (1<<4)
#define   B_PCIE_PORT_PXPDEVCTRL2_CTV               (BIT0 | BIT1 | BIT2 | BIT3)

#define R_PCIE_PORT_PXPLINKCAP2                   0xBC
#define   B_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC         (BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7)
#define     V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_3G    BIT3
#define     V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_2G    BIT2
#define     V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_1G    BIT1
// LINK supports G3, G2 and G1 
#define     V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_3G_TOP    ( BIT3 | BIT2 | BIT1)
// LINK supports G2 and G1 
#define     V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_2G_TOP    ( BIT2 | BIT1 )
// LINK only supports G1   
#define     V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_1G_TOP    BIT1

#define R_PCIE_PORT_PXPLNKCTRL2                   0xC0
#define   B_PCIE_PORT_PXPLNKCTRL2_DEEMPH            BIT6
#define   B_PCIE_PORT_PXPLNKCTRL2_ENCmpl            BIT4
#define   B_PCIE_PORT_PXPLNKCTRL2_LNKSPEED          (BIT3 | BIT2 | BIT1 | BIT0)
#define     V_PCIE_PORT_PXPLNKCTRL2_LS_GEN3           0x03
#define     V_PCIE_PORT_PXPLNKCTRL2_LS_GEN2           0x02
#define     V_PCIE_PORT_PXPLNKCTRL2_LS_GEN1           0x01

#define R_PCIE_PORT_PXPPMCAP                      0xE0
#define R_PCIE_PORT_PXPPMCSR                      0xE4
#define   B_PCIE_PORT_PXPPMCSR_PWR_STATE              (BIT1 | BIT0)
#define     V_PCIE_PORT_PXPPMCSR_PWR_STATE_D0         0x00
#define     V_PCIE_PORT_PXPPMCSR_PWR_STATE_D3HOT      0x03

#define R_ACSCTRL                                 0x116
#define R_PCIE_PORT_ACSCTRL                       R_ACSCTRL
#define   B_PCIE_PORT_ACSCTRL_U                     BIT4
#define   B_PCIE_PORT_ACSCTRL_C                     BIT3
#define   B_PCIE_PORT_ACSCTRL_R                     BIT2
#define   B_PCIE_PORT_ACSCTRL_B                     BIT1
#define   B_PCIE_PORT_ACSCTRL_V                     BIT0

#define R_PCIE_PORT_APICBASE                      0x140
#define   B_PCIE_PORT_APICBASE_ENABLE               BIT0
#define R_PCIE_PORT_APICLIMIT                     0x142
#define R_PCIE_PORT_UNCERRSTS                     0x14C
#define   B_PCIE_PORT_UNSUPPORTED_REQUEST           BIT20

#define R_PCIE_PORT_CORERRMSK                     0x15C
#define   B_PCIE_CORERRMSK_ANFE_MASK               BIT13

#define R_PCIE_PORT_RPERRCMD                      0x174
#define   B_PCIE_CORRECTABLE_ERR_RPT_EN           BIT0
#define   B_PCIE_NONFATAL_ERR_RPT_EN              BIT1
#define   B_PCIE_FATAL_ERR_RPT_EN                 BIT2

#define R_PCIE_PORT_PCIEIPERFCTRLSTS              0x180
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_TPHDIS                             BIT41
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_DCA_ReqID_Override                 BIT40
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_max_read_completion_combine_size   BIT35
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_outstanding_requests_gen1          BIT16   //BIT 20:16 Outstanding Requests for Gen1
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_outstanding_requests_gen2          BIT8    //BIT 13:8 Outstanding Requests for Gen2
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_UseAllocatingFlowWr                BIT7
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_vcp_roen_nswr                      BIT6
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_vcp_nsen_rd                        BIT5
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_SIS          BIT4
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_NSOW         BIT3
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_NSOR         BIT2
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_SSP          BIT1
#define   B_PCIE_PORT_PCIEIPERFCTRLSTS_SSCTL        BIT0

#define R_PCIE_PORT_PCIEMISCCTRLSTS               0x188
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_Locked_Rd_TO        BIT49
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_Rcv_PME_TO_ACK      BIT48
#define   B_PROBLEMATIC_PORT_FOR_LOCK_FLOW                BIT38
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_FormFact            BIT36
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_MSIFATEN            BIT35
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_MSINFATEN           BIT34
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_MSICOREN            BIT33
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_ACPI_PMEEN          BIT32
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_DIS_L0sTx           BIT31
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_InboundIOEN         BIT30
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_TO_DIS              BIT28
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_SIOLnkBWSts         BIT27
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_DIS_EOI             BIT26
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_DIS_INB_P2PWrtDis   BIT25
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_DIS_INB_P2PRdDis    BIT24
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_DIS_PHold           BIT23
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_PME_Ack_TOCtl       (BIT8 | BIT7)
#define   V_PCIE_PORT_PCIEMISCCTRLSTS_PME_AckTOCtl        0x7
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_PME_TO_ACK_TIMEOUT  BIT6
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_Send_PME_OFF        BIT5
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_SysERR_only4_AER    BIT4
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_EN_ACPI_HotPlug     BIT3
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_EN_ACPI_PM          BIT2
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_Inbound_CFGEN       BIT1
#define   B_PCIE_PORT_PCIEMISCCTRLSTS_Set_HostBrdg_CC     BIT0

#define R_PCIE_PORT_PCIEIOUxBIFCTRL               0x190     // registe is only applicapble in device 1, 3, & 7
#define   B_PCIE_PORT_PCIEIOUxBIFCTRL_SBIF          BIT3    // This is the bit to initiate the bifurcuatin on the port
#define   B_PCIE_PORT_PCIEIOUxBIFCTRL_LNKCFG       (BIT0 + BIT1 + BIT2)
#define R_PCIE_PORT_XPCORERRSTS                   0x200
#define   B_PCIE_PORT_XPCORERRSTS_LBS               BIT1
#define R_PCIE_PORT_XPCORERRMSK                   0x204
#define   B_PCIE_PORT_XPCORERRMSK_LBM               BIT1
#define R_PCIE_PORT_XPUNCERRSTS                   0x208
#define   V_PCIE_PORT_XPUNCERRSTS                   (BIT1 + BIT3 + BIT4 + BIT5 + BIT6 + BIT8 + BIT9)
#define R_PCIE_PORT_XPUNCERRMSK                   0x20c
#define   V_PCIE_PORT_XPUNCERRMSK                   (BIT1 + BIT3 + BIT4 + BIT5 + BIT6 + BIT8 + BIT9)
#define R_PCIE_PORT_XPUNCERRSEV                   0x210
#define   V_PCIE_PORT_XPUNCERRSEV                   (BIT1 + BIT3 + BIT4 + BIT5 + BIT6 + BIT8 + BIT9)
#define R_PCIE_PORT_XPUNCERRPTR                   0x214
#define R_PCIE_PORT_UNCEDMASK                     0x218
#define R_PCIE_PORT_COREDMASK                     0x21c
#define   V_PCIE_PORT_COREDMASK                     (BIT1 + BIT6 + BIT7 + BIT8 + BIT12 + BIT13)
#define R_PCIE_PORT_RPEDMASK                      0x220
#define R_PCIE_PORT_XPUNCEDMASK                   0x224
#define R_PCIE_PORT_XPCOREDMASK                   0x228
#define R_PCIE_PORT_XPGLBERRSTS                   0x230
#define   B_PCIE_PORT_XPGLBERRSTS_FAER              BIT0
#define   B_PCIE_PORT_XPGLBERRSTS_NAER              BIT1
#define   B_PCIE_PORT_XPGLBERRSTS_CAER              BIT2
#define R_PCIE_PORT_XPGLBERRPTR                   0x232

#define R_PCIE_PORT_LN0LN3EQ                      0x25C
#define LINKTRAINING_DELAY                        2
#define LINKTRAINING_TIME_OUT                     5000

/**
==================================================================================================
================================== NTB Definitions  =========================================
==================================================================================================
**/

#define R_NTB_PPD                               0xD4
#define   B_NTB_PORT_DEF                          (BIT0 | BIT1)
                                             
#define R_NTB_PORT_PXPLNKSTS                    0x1A2
#define   B_NTB_PORT_PXPLNKSTS_DLLA               BIT13
#define   B_NTB_PORT_PXPLNKSTS_SCCON              BIT12
#define   B_NTB_PORT_PXPLNKSTS_NLNKWD             (BIT4 | BIT5 | BIT6 | BIT7 | BIT8 | BIT9)
#define   B_NTB_PORT_PXPLNKSTS_LNKSPD             (BIT0 | BIT1 | BIT2 | BIT3)

#define IIO_NTB_DEVICE_NUM                      0x3
#define IIO_NTB_FUNC_NUM                        0x0
#define NTB_PORT_DEF_TRANSPARENT                0
#define NTB_PORT_DEF_NTB_NTB                    1
#define NTB_PORT_DEF_NTB_RP                     2

#define NO_NTB_PORT_INDEX                       0xFF

#define NTB_PORT_DEF_TRANSPARENT                0
#define NTB_PORT_DEF_NTB_NTB                    1
#define NTB_PORT_DEF_NTB_RP                     2
                                                
#define NTB_PB01_POW2_ALIGNMENT                 16
#define NTB_PB01_SIZE                           0x10000
                                                
#define R_SB23BASE_SB45BASE_OFF                 0x500
#define R_NTBCNTL                               0x58
#define   B_SECLNK_DIS                          BIT1
#define   B_SECCFG_LOCK                         BIT0

#define R_PB01BASE_SB01BASE       0x510
#define   B_SB01BASE_PREFETCHABLE BIT3
#define R_PB01BASE_SB23BASE       0x518
#define R_PB01BASE_SB45BASE       0x520
#define R_NTBCNTL                 0x58
#define   B_SECLNK_DIS            BIT1
#define   B_SECCFG_LOCK           BIT0
#define ONLY_REGISTER_OFFSET(x) (x & 0xFFF)
#define R_PB01BASE_PBAR2XLAT      0x4518
#define R_PB01BASE_PBAR4XLAT      0x4520

#define R_PB01BASE_PBARPCIPCMD    0x504
#define R_PB01BASE_SBARPCIPCMD    0x4504


#define   B_PBAR2XLAT_Type_Mem64  2 << 1
#define PBAR2XLAT_MBA_Default     0x4000000000
#define PBAR4XLAT_MBA_Default     0x8000000000

/**
==================================================================================================
================================== IOAT Definitions     =========================================
==================================================================================================
**/
#define IOAT_DEVICE_NUM                           0x04
#define IOAT_FUNC_START                           0x00
#define IOAT_TOTAL_FUNCS                          0x08
#define R_IIO_CB_DEVCON                           0x98
#define   B_IIO_CB_DEVCON_RELAX_OR_EN             BIT4

#define R_CB_BAR_CSI_DMACAPABILITY                0x10
#define   B_CB_BAR_CSI_DMACAP_DCA_SUP             BIT4
#define R_CB_BAR_CSI_CAP_ENABLE                   0x10C
#define   B_CB_BAR_CSI_CAP_ENABLE_PREFETCH_HINT   BIT0
#define R_CB_BAR_PCIE_CAP_ENABLE                  0x10E
#define   B_CB_BAR_PCIE_CAP_ENABLE_MEM_WR         BIT0
#define R_CB_BAR_APICID_TAG_MAP                   0x110
#define R_CB_BAR_CBVER                            0x08
#define   V_CB32_VERSION                          0x32
#define   V_CB33_VERSION                          0x33

#define IIO_CBDMA_MMIO_SIZE 0x10000 //64kB for one CBDMA function
#define IIO_CBDMA_MMIO_ALLIGNMENT 14 //2^14 - 16kB

/**
==================================================================================================
================================== VTd Definitions      =========================================
==================================================================================================
**/
//
// Dev 5, Func 0 - Intel VT, Address Map, System Management, Miscellaneous Registers
//
// APTIOV_SERVER_OVERRIDE_RC_START : DYNAMIC_MMCFG_BASE_SUPPORT Changes
#define R_IIO_MMCFG_B0                         0x90              // MMCFG Base Address
#define CORE05_BUS_NUM                            0            
// APTIOV_SERVER_OVERRIDE_RC_END : DYNAMIC_MMCFG_BASE_SUPPORT Changes
#define CORE05_DEV_NUM                            5
#define CORE05_FUNC_NUM                           0

#define R_IIO_CIPCTRL                          0x140             // Coherent Interface Protocol Control
#define   B_IIO_CIP_FLU_WRITE                    BIT31           // Flush Currently Pending Writes to dram from Write Cache
#define   B_IIO_CIP_WRI_UPDATE_DIS               BIT28           // Disable WriteUpdate Flow
#define   B_IIO_CIP_EXRTID_MODE                  BIT2            // Extended RTID Mode Enable
#define   B_IIO_CIP_WRI_COMBIN_DIS               BIT1            // Disable write combining
#define   V_IIO_CIP_PCIRDCURRENT                 0               // PCIRdCurrent/DRd.UC mode select
#define   V_IIO_CIP_DRDUC                        1

#define R_IIO_CIPSTS                           0x144             // Coherent Interface Protocol Status
#define R_IIO_CIPDCASAD                        0x148             // Coherent Interface Protocol DCA Source Address
#define   B_IIO_CIP_TPH_DCA_EN                   BIT0            // Enable TPH/DCA
#define R_IIO_CIPINTRC                         0x14C             // Coherent Interface Protocol Interrupt Control
#define   B_IIO_CIP_SMI_MASK                     BIT10           // SMI Mask
#define R_IIO_CIPINTRS                         0x150             // Coherent interface Protocol Interrupt Status


#define R_IIO_VTBAR                            0x180             // Base Address Register for Intel VT-d Registers
#define   B_IIO_VT_BAR_EN                        BIT0            // Intel VT-d Base Address Enable
#define   V_VT_D_IIO_BAR_SIZE                    (8*1024)        // VT-D IIO BAR is 8KB with Natural Alignment
#define R_IIO_VTGENCTRL                        0x184             // Intel VT-d General Control
#define   V_IIO_VTCTRL_LOCK_VTD_SET            BIT15             // VT-D Bar lock bit
#define   V_IIO_VTCTRL_LIMITS_CLEAR              0x0FF00         // Clear bits [7:0]
#define   V_IIO_VTCTRL_VTIGPAL_47BIT             0x07
#define   V_IIO_VTCTRL_VTHPAL_38BIT              0x03
#define   V_IIO_VTCTRL_VTHPAL_39BIT              0x04
#define   V_IIO_VTCTRL_VTHPAL_46BIT              0x0A
#define   V_IIO_VTCTRL_VTHPAL_48BIT              0x0C
#define   V_IIO_VTCTRL_VTHPAL_51BIT              0x0F
#define   V_IIO_VTCTRL_VTNGPAL_48BIT             0x08
#define R_IIO_VTISOCHCTRL                      0x188            // Intel VT-d Isoch Related Control
#define   B_IIO_VTISOCHCTRL_AZALIA_ON_VCP        BIT8           // Azalia on VCP bit
#define R_IIO_VTGENCTRL2                       0x18C            // Intel VT-d General Control 2
#define   B_IIO_VTGENCTRL2_PrefCtrl              (BIT6 | BIT5)  // Prefetch Control
#define R_IIO_IOTLBPARTITION                   0x194            // IOTLB Partitioning Control
#define   B_IIO_IOT_PARTITION_EN                 BIT0           // IOTLB Partitioning Enable
#define R_IIO_VTUNCERRSTS                      0x1A8            // Uncorrectable Error Status
#define R_IIO_VTUNCERRMSK                      0x1AC            // Intel VT Uncorrectable Error Mask
#define   B_IIO_VTUNCERRMSK_MASK                 BIT31          // Mask reporting Intel VT-d defined errors to IIO core logic
#define   B_MISCERRM                              BIT7
#define   B_VTDERR_MASK                           BIT31
#define R_IIO_VTUNCERRSEV                      0x1B0            // Intel VT Uncorrectable Error Severity
#define R_IIO_VTUNCERRPTR                      0x1B4            // Intel VT Uncorrectable Error Pointer

#define R_VTD_VERSION                             0x00
#define R_VTD_CAP                                 0x08
#define R_VTD_CAP_LOW                             0x08
#define R_VTD_CAP_HIGH                            0x0C
#define R_VTD_EXT_CAP_LOW                         0x10
#define R_VTD_EXT_CAP_HIGH                        0x14

#define V_VTD_REMAP_ENGINE_SIZE                   0x1000
#define VT_D_CHIPSET_BASE_ADDRESS                 0xFE710000

#define VTD1_BASE_OFFSET                          0x1000

#define R_VTD1_VERSION                            VTD1_BASE_OFFSET  + 0x00
#define R_VTD1_CAP                                VTD1_BASE_OFFSET + 0x08
#define R_VTD1_CAP_LOW                            VTD1_BASE_OFFSET + 0x08
#define R_VTD1_CAP_HIGH                           VTD1_BASE_OFFSET + 0x0C
#define R_VTD1_EXT_CAP_LOW                        VTD1_BASE_OFFSET + 0x10
#define R_VTD1_EXT_CAP_HIGH                       VTD1_BASE_OFFSET + 0x14
#define R_VTD0_INTR_REMAP_TABLE_BASE              0xB8          // VTD0 Interrupt Remapping Table Base Address
#define R_VTD1_INTR_REMAP_TABLE_BASE              0x10B8        // VTD1 Interrupt Remapping Table Base Address
#define   B_IA32_EXT_INT_ENABLE                    BIT11        // IA32_Extended_Interrupt_Enable


/**
==================================================================================================
================================== IOAPIC Defintions     =========================================
==================================================================================================
**/
#define CORE05_FUNC4_NUM                          4
#define CORE05_MAX_FUNC                           4

#define APIC_DEV_NUM                              CORE05_DEV_NUM
#define APIC_FUNC_NUM                             CORE05_FUNC4_NUM

#define R_APIC_MBAR                                    0x10          // IOxAPIC Base Address  
#define R_APIC_ABAR                                    0x40          // I/OxAPIC Alternate BAR
#define   B_APIC_ABAR_ABAR_EN                            BIT15       // ABAR Enable
#define R_APIC_PMCAP                                   0xE0          // Power Management Capabilities
#define R_APIC_PMCSR                                   0xE4          // Power Management Control and Status
#define R_APIC_RDINDEX                                 0x80          // Alternate Index to read Indirect I/OxAPIC Registers
#define R_APIC_RDWINDOW                                0x90          // Alternate Window to read Indirect I/OxAPIC Registers
#define R_APIC_IOAPICTETPC                             0xA0          // IOxAPIC Table Entry Target Programmable Control
#define R_APIC_IOADSELS0                               0x288         // IOxAPIC DSELS Register 0
#define R_APIC_IOADSELS1                               0x28C         // IOxAPIC DSELS Register 1
#define R_APIC_IOINTSRC0                               0x2A0         // IO Interrupt Source Register 0
#define R_APIC_IOINTSRC1                               0x2A4         // IO Interrupt Source Register 1
#define R_APIC_IOREMINTCNT                             0x2A8         // Remote IO Interrupt Count
#define R_APIC_IOREMGPECNT                             0x2AC         // Remote IO GPE Count
#define R_APIC_IOXAPICPARERRINJCTL                     0x2C0         // IOxAPIC Parity Error Injection Control
#define R_APIC_FAUXGV                                  0x2C4         // FauxGV
#define   B_APIC_FAUX_GV_EN                              BIT0        // Faux GV Enable

/**
==================================================================================================
================================== VMD Defintions =========================================
==================================================================================================
**/
#define VMD_DEV_NUM                              CORE05_DEV_NUM
#define VMD_FUNC_NUM                             5

/**
==================================================================================================
================================== Devide Hide Definitions =======================================
==================================================================================================
**/

#define UBOX_MISC_DEV_NUM                           0x8
#define UBOX_MISC_FUNC_NUM                          0x2

#define PCU22_DEV_NUM                          30
#define PCU22_FUNC_NUM                         06
#define NUM_DEVHIDE_REGS                              8 
// Hide all 8 Devices for every Stack 
#define MAX_DEVHIDE_REGS                             (MAX_IIO_STACK * NUM_DEVHIDE_REGS)
#if MaxIIO > 4
#define MAX_DEVHIDE_REGS_PER_SYSTEM                  384 //(MAX_DEVHIDE_REGS * MaxIIO)
#else
#define MAX_DEVHIDE_REGS_PER_SYSTEM                  192 //(MAX_DEVHIDE_REGS * MaxIIO)
#endif


/**
==================================================================================================
================================== LT Definitions             ====================
==================================================================================================
**/

#ifdef LT_FLAG
#define LT_DPR_SIZE                               0x00300000
#define LT_DPR_SIZE_REG                           0x00030
#define LT_DEV_MEM_SIZE                           0x00200000
#define LT_DEV_MEM_ALIGNMENT                      0x00100000
#define SINIT_MAX_SIZE                            0x00040000
#define LT_HEAP_SIZE                              0x00100000

#define LT_PUB_BASE                               0xFED30000
#define LT_SPAD_HIGH                              0xFED300A4
#define LT_SPAD_EXIST                             0xFED30010
#define LPB_DPR_BAR                               0x0330

#define PLATFORM_TXT_FIT_FAILSAFE_TYPE            9
#define PLATFORM_TXT_FIT_BIOSACM_TYPE             2
#endif



/**
==================================================================================================
================================== PCH Definitions             ===================================
==================================================================================================
**/

//
// ME-HECI defines
//
#define HECI1_DEV_NUM                             0x12
#define HECI1_FUNC_NUM                            0x00
#define HECI2_DEV_NUM                             0x12
#define HECI2_FUNC_NUM                            0x01

#define V_HECI_VID                                V_INTEL_VID
#define V_HECI1_DID_CLIENT                        0x3414    // HECI-1 Client SKU
#define V_HECI2_DID_CLIENT                        0x3415    // HECI-2 Client SKU
#define V_HECI1_DID_SERVER                        0x3434    // HECI-1 Server SKU
#define V_HECI2_DID_SERVER                        0x3435    // HECI-2 Server SKU

#define R_HECIMBAR                                0x10
#define R_FWSTATE                                 0x40
#define   B_CUR_STATE                               (BIT3 | BIT2 | BIT1 | BIT0)
#define     V_CUR_STATE_MEFW_HARDM_DISABLED           0x00
#define     V_CUR_STATE_MEFW_INITIALIZING             0x01
#define     V_CUR_STATE_MEFW_RECOVERY                 0x02
#define     V_CUR_STATE_MEFW_NPTM_ACTIVE              0x03
#define     V_CUR_STATE_MEFW_NPTM_INACTIVE            0x04
#define     V_CUR_STATE_MEFW_SOFTM_DISABLED           0x08
#define     V_CUR_STATE_MEFW_VALIDATION               0x0F

#define   B_ERR_CODE                                (BIT15 | BIT14 | BIT13 | BIT12)
#define     V_ERR_CORE_MEFW_NOERROR                   0x00
#define     V_ERR_CORE_MEFW_UNCATEGORIZED             0x01
#define     V_ERR_CORE_MEFW_DISABLED                  0x02
#define     V_ERR_CORE_MEFW_IMG_FAILURE               0x03

#define   B_EXT_STAT                                ( BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | \
                                                      BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16)

#define R_HIDM                                    0xA0

//
// ME-HECI MBAR registers
//
#define HECI_DEV                                  22
#define HECI_FUN                                  0
#define R_HECI_HOST_CB_WW                         0x00
#define R_HECI_HOST_H_CSR                         0x04
#define R_HECI_ME_CB_RW                           0x08
#define R_HECI_ME_CSR_HA                          0x0C
#define R_HECI_GEN_STS                            0x4C

#define PCH_LBG_MROM0_PORT_DEVICE               17
#define PCH_LBG_MROM0_PORT_FUNCTION             0
#define PCH_LBG_MSUINT_MROM0_PLKCTL_REG         0xe8
#define PCH_LBG_MSUINT_MROM0_MSDEVFUNCHIDE_REG  0xD4

#define R_PCH_DMI_LSTS                         0x21AA
#define R_PCH_DMI_LCTL2                        0x21B0

#define PCH_PMC_ACPI_BASE                      0x40


/**
==================================================================================================
================================== LBC Command Definitions     ===================================
==================================================================================================
**/
#define LBC_CPL_COMMAND                        0
#define LBC_START_COMMAND                      0x1
#define LBC_READ_COMMAND                       0x1
#define LBC_WRITE_COMMAND                      0x2
#define LBC_LN_SEL_OFFSET                      11
#define LBC_PROGRAM_DELAY                      1       //us
#define LBC_TIME_OUT                           5000


/**
==================================================================================================
================================== UniPhy Definitions            ===================================
==================================================================================================
**/
//
// BIT 31 of PortBit field indicates IIO Global Dfx Register
//
#define GLOBAL_DFX_BIT             0x80000000
#define IIO_UNKNOW_REG             0xF
#define IIO_PORT_PCIE_REG          0x0
#define IIO_PORT_DFX_REG           0x1
#define IIO_GLOBAL_DFX_REG         0x7
/**
==================================================================================================
================================== GEN3 Definitions            ===================================
==================================================================================================
**/

#define GEN3_LINK_POLL_GRANULARITY          100     //us
#define UNIPHY_RECIPE_DELAY                 180     //us
#define UNIPHY_DIS_DELAY                    100000  //us  (100ms)
#define VPP_WAIT_DELAY                      500     //us
#define GEN3_STALL_SBR                      2000    //us
#define IIO_LINK_TRAIN_TIME_NO_G3EQ         30000   //us  (30ms)
#define IIO_LINK_TRAIN_TIME_MARGIN_NO_G3EQ  100000  //us  (100ms)
#define WAIT_FOR_DOWNSTREAM_DEV             100000  //us  (100ms)
#define IIO_LINK_TRAIN_TIME_G3EQ            60000   //us  (60ms)
#define IIO_LINK_TRAIN_TIME_MARGIN_G3EQ     200000  //us  (200ms)
#define GEN3_LINK_POLL_TIMEOUT              10000   //us
#define LINK_INIT_POLL_TIMEOUT              10000   //us
#define SLTCAP_DELAY                        200000  //us
#define GEN3_LINK_POLL_TIMEOUT_MID          300000  //us
#define LINK_TRAINING_DELAY                 400000  //us
#define GEN3_LINK_POLL_TIMEOUT_RECOVERY     2000000 //us
#define SLOT_POWER_DELAY                    1000    //us  (1ms)
#define RX_RESET_DELAY                      1       //us

#define NONE_GEN3_PCIE_PORT                 0xFF

#define GEN3_SUCCESS                        0
#define GEN3_FAILURE                        1

#define GEN3_TX_PRESET_AUTO                0xFF
#define GEN3_RX_PRESET_AUTO                0xFF

/**
==================================================================================================
================================== IIO POST code - Major Definitions      ========================
==================================================================================================
**/

#define STS_IIO_EARLY_INIT_ENTRY            0xE0  // IIO early init entry
#define STS_EARLY_PRELINK_TRAINING          0xE1  // Early Pre-link training setting
#define STS_GEN3_EQ_PROGRAMMING             0xE2  // IIO Gen3 EQ programming
#define STS_LINK_TRAINING                   0xE3  // IIO Link training
#define STS_GEN3_OVERRIDE                   0xE4  // IIO Gen3 override
#define STS_IIO_EARLY_INIT_EXIT             0xE5  // IIO early init exit
#define STS_IIO_LATE_INIT_ENTRY             0xE6  // IIO late init entry
#define STS_IIO_PCIE_PORT_INIT              0xE7  // PCIE port init
#define STS_IIO_IOAPIC_INIT                 0xE8  // IOAPIC init
#define STS_IIO_VTD_INIT                    0xE9  // VTD init
#define STS_IIO_IOAT_INIT                   0xEA  // IOAT init
#define STS_IIO_DFX_INIT                    0xEB  // IIO DFX init
#define STS_IIO_NTB_INIT                    0xEC  // NTB init
#define STS_IIO_SECURITY_INIT               0xED  // Security init
#define STS_IIO_LATE_INIT_EXIT              0xEE  // IIO late init exit
#define STS_IIO_ON_READY_TO_BOOT            0xEF  // IIO On ready to boot

/**
==================================================================================================
================================== IIO POST code - Minor Definitions      ========================
==================================================================================================
**/
#define STS_IIO_INIT                0x0  // IIO On ready to boot
#define DISABLE_ALL_PORTS           0xFFFF
#define DISABLE_A_PORT              0x000F
#define DISABLE_B_PORT              0x00F0
#define DISABLE_C_PORT              0x0F00
#define DISABLE_D_PORT              0xF000
#endif //_IIO_REGS_H_
