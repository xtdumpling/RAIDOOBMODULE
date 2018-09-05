//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license 
// agreement with Intel or your vendor.  This file may be      
// modified by the user, subject to additional terms of the    
// license agreement                                           
//
/**

Copyright (c) 1999 - 2007, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IchRegsAliases.h

  Registers' aliases for ICH9x. These are for compatibility with existing
  platform codes.

**/

#ifndef _ICH_REGS_ALIASES_H_
#define _ICH_REGS_ALIASES_H_

//
// Definitions beginning with "R_" are registers
// Definitions beginning with "B_" are bits within registers
// Definitions beginning with "V_" are meaningful values of bits within the registers
// Definitions beginning with "S_" are register sizes
// Definitions beginning with "N_" are the bit position
//

//
// DMI to PCI Bridge Registers (D30:F0)
//
#define PCI_DEVICE_NUMBER_ICH_HL2P                     PCI_DEVICE_NUMBER_ICH_PCI_PCI
#define PCI_FUNCTION_NUMBER_ICH_HL2P                   PCI_FUNCTION_NUMBER_ICH_PCI_PCI
#define R_ICH_HL2P_VENDOR_ID                           R_ICH_PCI_PCI_VID
#define   V_ICH_HL2P_VENDOR_ID                           V_ICH_PCI_PCI_VENDOR_ID
#define R_ICH_HL2P_DEVICE_ID                           R_ICH_PCI_PCI_DID
#define   V_ICH_HL2P_DEVICE_ID_0                         V_ICH_PCI_PCI_DEVICE_ID_0
#define   V_ICH_HL2P_DEVICE_ID_1                         V_ICH_PCI_PCI_DEVICE_ID_1

#define R_ICH_HL2P_COMMAND                             R_ICH_PCI_PCI_PCICMD
#define   B_ICH_HL2P_COMMAND_FBE                         B_ICH_PCI_PCI_PCICMD_FBE
#define   B_ICH_HL2P_COMMAND_SERR_EN                     B_ICH_PCI_PCI_PCICMD_SERR_EN
#define   B_ICH_HL2P_COMMAND_WCC                         B_ICH_PCI_PCI_PCICMD_WCC
#define   B_ICH_HL2P_COMMAND_PER                         B_ICH_PCI_PCI_PCICMD_PER
#define   B_ICH_HL2P_COMMAND_VPS                         B_ICH_PCI_PCI_PCICMD_VPS
#define   B_ICH_HL2P_COMMAND_MWE                         B_ICH_PCI_PCI_PCICMD_MWE
#define   B_ICH_HL2P_COMMAND_SCE                         B_ICH_PCI_PCI_PCICMD_SCE
#define   B_ICH_HL2P_COMMAND_BME                         B_ICH_PCI_PCI_PCICMD_BME
#define   B_ICH_HL2P_COMMAND_MSE                         B_ICH_PCI_PCI_PCICMD_MSE
#define   B_ICH_HL2P_COMMAND_IOSE                        B_ICH_PCI_PCI_PCICMD_IOSE
#define R_ICH_HL2P_PD_STS                              R_ICH_PCI_PCI_PSTS
#define   B_ICH_HL2P_PD_STS_DPE                          B_ICH_PCI_PCI_PSTS_DPE
#define   B_ICH_HL2P_PD_STS_SSE                          B_ICH_PCI_PCI_PSTS_SSE
#define   B_ICH_HL2P_PD_STS_RMA                          B_ICH_PCI_PCI_PSTS_RMA
#define   B_ICH_HL2P_PD_STS_RTA                          B_ICH_PCI_PCI_PSTS_RTA
#define   B_ICH_HL2P_PD_STS_STA                          B_ICH_PCI_PCI_PSTS_STA
#define   B_ICH_HL2P_PD_STS_DPD                          B_ICH_PCI_PCI_PSTS_DPD
#define R_ICH_HL2P_REV_ID                              R_ICH_PCI_PCI_RID
#define R_ICH_HL2P_PI                                  R_ICH_PCI_PCI_PI
#define R_ICH_HL2P_SCC                                 R_ICH_PCI_PCI_SCC
#define R_ICH_HL2P_BCC                                 R_ICH_PCI_PCI_BCC
#define R_ICH_HL2P_PMLT                                R_ICH_PCI_PCI_PMLT
#define   B_ICH_HL2P_PMLT_MLC                            B_ICH_PCI_PCI_PMLT_MLTC
#define R_ICH_HL2P_HEADTYP                             R_ICH_PCI_PCI_HEADTYP
#define   B_ICH_HL2P_HEADTYP_MFD                         B_ICH_PCI_PCI_HEADTYP_MFD
#define   B_ICH_HL2P_HEADTYP_HT                          B_ICH_PCI_PCI_HEADTYP_HTYPE
#define R_ICH_HL2P_PBUS_NUM                            R_ICH_PCI_PCI_PBN
#define R_ICH_HL2P_SBUS_NUM                            R_ICH_PCI_PCI_SCBN
#define R_ICH_HL2P_SUB_BUS_NUM                         R_ICH_PCI_PCI_SBBN
#define R_ICH_HL2P_SMLT                                R_ICH_PCI_PCI_SMLT
#define   B_ICH_HL2P_SMLT_MLC                            B_ICH_PCI_PCI_SMLT_MLTC
#define R_ICH_HL2P_IO_BASE                             R_ICH_PCI_PCI_IO_BASE
#define   B_ICH_HL2P_IO_BASE_BAR                         B_ICH_PCI_PCI_IO_BASE_IOBA
#define   B_ICH_HL2P_IO_BASE_ADDR_CAP                    B_ICH_PCI_PCI_IO_BASE_IOBC
#define R_ICH_HL2P_IOLIM                               R_ICH_PCI_PCI_IO_LIMIT
#define   B_ICH_HL2P_IOLIM                               B_ICH_PCI_PCI_IO_LIMIT_IOLA
#define   B_ICH_HL2P_IOLIM_ADDR_CAP                      B_ICH_PCI_PCI_IO_LIMIT_IOLC
#define R_ICH_HL2P_SECSTS                              R_ICH_PCI_PCI_SECSTS
#define   B_ICH_HL2P_SECSTS_DPE                          B_ICH_PCI_PCI_SECSTS_DPE
#define   B_ICH_HL2P_SECSTS_SSE                          B_ICH_PCI_PCI_SECSTS_SSE
#define   B_ICH_HL2P_SECSTS_RMA                          B_ICH_PCI_PCI_SECSTS_RMA
#define   B_ICH_HL2P_SECSTS_RTA                          B_ICH_PCI_PCI_SECSTS_RTA
#define   B_ICH_HL2P_SECSTS_STA                          B_ICH_PCI_PCI_SECSTS_STA
#define   B_ICH_HL2P_SECSTS_MDPED                        B_ICH_PCI_PCI_SECSTS_DPD
#define   B_ICH_HL2P_SECSTS_FB2B                         B_ICH_PCI_PCI_SECSTS_FBC
#define   B_ICH_HL2P_SECSTS_66MHZ_CAP                    B_ICH_PCI_PCI_SECSTS_66MHZ_CAP
#define R_ICH_HL2P_MEMBASE                             R_ICH_PCI_PCI_MEM_BASE
#define   B_ICH_HL2P_MEMBASE_BAR                         B_ICH_PCI_PCI_MEM_BASE_MB
#define R_ICH_HL2P_MEMLIM                              R_ICH_PCI_PCI_MEM_LIMIT
#define   B_ICH_HL2P_MEMLIM                              B_ICH_PCI_PCI_MEM_LIMIT_ML
#define R_ICH_HL2P_PREF_MEM_BASE                       R_ICH_PCI_PCI_PREF_MEM_BASE
#define   B_ICH_HL2P_PREF_MEM_BASE_BAR                   B_ICH_PCI_PCI_PREF_MEM_BASE_PMB
#define R_ICH_HL2P_PREF_MEM_MLT                        R_ICH_PCI_PCI_PREF_MEM_LIMIT
#define   B_ICH_HL2P_PREF_MEM_MLT                        B_ICH_PCI_PCI_PREF_MEM_LIMIT_PML
#define R_ICH_HL2P_PMBU32                              R_ICH_PCI_PCI_PMBU32
#define R_ICH_HL2P_PMLU32                              R_ICH_PCI_PCI_PMLU32
#define R_ICH_HL2P_CAPP                                R_ICH_PCI_PCI_CAPP
#define R_ICH_HL2P_INT_LINE                            R_ICH_PCI_PCI_INTR
#define   B_ICH_HL2P_INT_IPIN                            B_ICH_PCI_PCI_INTR_IPIN
#define   B_ICH_HL2P_INT_ILINE                           B_ICH_PCI_PCI_INTR_ILINE
#define R_ICH_HL2P_BRIDGE_CNT                          R_ICH_PCI_PCI_BCTRL
#define   B_ICH_HL2P_BRIDGE_CNT_DTE                      B_ICH_PCI_PCI_BCTRL_DTE
#define   B_ICH_HL2P_BRIDGE_CNT_DTS                      B_ICH_PCI_PCI_BCTRL_DTS
#define   B_ICH_HL2P_BRIDGE_CNT_SDT                      B_ICH_PCI_PCI_BCTRL_SDT
#define   B_ICH_HL2P_BRIDGE_CNT_PDT                      B_ICH_PCI_PCI_BCTRL_PDT
#define   B_ICH_HL2P_BRIDGE_CNT_FB2B                     B_ICH_PCI_PCI_BCTRL_FBE
#define   B_ICH_HL2P_BRIDGE_CNT_SBR                      B_ICH_PCI_PCI_BCTRL_SBR
#define   B_ICH_HL2P_BRIDGE_CNT_MAM                      B_ICH_PCI_PCI_BCTRL_MAM
#define   B_ICH_HL2P_BRIDGE_CNT_VGA16                    B_ICH_PCI_PCI_BCTRL_V16D
#define   B_ICH_HL2P_BRIDGE_CNT_VGA_EN                   B_ICH_PCI_PCI_BCTRL_VGAE
#define   B_ICH_HL2P_BRIDGE_CNT_ISA_EN                   B_ICH_PCI_PCI_BCTRL_IE
#define   B_ICH_HL2P_BRIDGE_CNT_SERR_EN                  B_ICH_PCI_PCI_BCTRL_SEE
#define   B_ICH_HL2P_BRIDGE_CNT_PER_EN                   B_ICH_PCI_PCI_BCTRL_PERE
#define R_ICH_HL2P_SPDH                                R_ICH_PCI_PCI_SPDH
#define   B_ICH_HL2P_SPDH_HD3                            B_ICH_PCI_PCI_SPDH_HD3
#define   B_ICH_HL2P_SPDH_HD2                            B_ICH_PCI_PCI_SPDH_HD2
#define   B_ICH_HL2P_SPDH_HD1                            B_ICH_PCI_PCI_SPDH_HD1
#define   B_ICH_HL2P_SPDH_HD0                            B_ICH_PCI_PCI_SPDH_HD0
#define R_ICH_HL2P_DTC                                 R_ICH_PCI_PCI_DTC
#define   B_ICH_HL2P_DTC_DDT                             B_ICH_PCI_PCI_DTC_DDT
#define   B_ICH_HL2P_DTC_BDT                             B_ICH_PCI_PCI_DTC_BDT
#define   B_ICH_HL2P_DTC_MDT_MASK                        B_ICH_PCI_PCI_DTC_MDT
#define   B_ICH_HL2P_DTC_AFADE                           B_ICH_PCI_PCI_DTC_AFADE
#define   B_ICH_HL2P_DTC_NP                              B_ICH_PCI_PCI_DTC_NP
#define   B_ICH_HL2P_DTC_MRMPD                           B_ICH_PCI_PCI_DTC_MRMPD
#define   B_ICH_HL2P_DTC_MRLPD                           B_ICH_PCI_PCI_DTC_MRLPD
#define   B_ICH_HL2P_DTC_MRPD                            B_ICH_PCI_PCI_DTC_MRPD
#define R_ICH_HL2P_BPS                                 R_ICH_PCI_PCI_BPS
#define   B_ICH_HL2P_BPS_PAD                             B_ICH_PCI_PCI_BPS_PAD
#define   B_ICH_HL2P_BPS_NPT                             B_ICH_PCI_PCI_BPS_NPT
#define   B_ICH_HL2P_BPS_NAT                             B_ICH_PCI_PCI_BPS_NAT
#define R_ICH_HL2P_BPC                                 R_ICH_PCI_PCI_BPC
#define   B_ICH_HL2P_BPC_URLT                            B_ICH_PCI_PCI_BPC_URLT
#define   B_ICH_HL2P_BPC_SDP                             B_ICH_PCI_PCI_BPC_SDP
#define   B_ICH_HL2P_BPC_PSE                             B_ICH_PCI_PCI_BPC_PSE
#define   B_ICH_HL2P_BPC_SDTT                            B_ICH_PCI_PCI_BPC_SDTT
#define   B_ICH_HL2P_BPC_PDE                             B_ICH_PCI_PCI_BPC_PDE
#define   B_ICH_HL2P_BPC_RTAE                            B_ICH_PCI_PCI_BPC_RTAE
#define R_ICH_HL2P_SVCAP                               R_ICH_PCI_PCI_SVCAP
#define   B_ICH_HL2P_SVCAP_NEXT                          B_ICH_PCI_PCI_SVCAP_NEXT
#define   B_ICH_HL2P_SVCAP_CID                           B_ICH_PCI_PCI_SVCAP_CID
#define R_ICH_HL2P_SVID                                R_ICH_PCI_PCI_SVID
#define   B_ICH_HL2P_SVID_SID                            B_ICH_PCI_PCI_SVID_SID
#define   B_ICH_HL2P_SVID_SVID                           B_ICH_PCI_PCI_SVID_SVID


//
// PCI to LPC Bridge Registers (D31:F0)
//
#define   B_ICH_LPC_ENABLES_ME2                          B_ICH_LPC_ENABLES_CNF2_EN
#define   B_ICH_LPC_ENABLES_SE                           B_ICH_LPC_ENABLES_CNF1_EN
#define   B_ICH_LPC_ENABLES_ME1                          B_ICH_LPC_ENABLES_MC_EN
#define   B_ICH_LPC_ENABLES_KE                           B_ICH_LPC_ENABLES_KBC_EN
#define   B_ICH_LPC_ENABLES_HGE                          B_ICH_LPC_ENABLES_GAMEH_EN
#define   B_ICH_LPC_ENABLES_LGE                          B_ICH_LPC_ENABLES_GAMEL_EN
#define   B_ICH_LPC_ENABLES_FDE                          B_ICH_LPC_ENABLES_FDD_EN
#define   B_ICH_LPC_ENABLES_PPE                          B_ICH_LPC_ENABLES_LPT_EN
#define   B_ICH_LPC_ENABLES_CBE                          B_ICH_LPC_ENABLES_COMB_EN
#define   B_ICH_LPC_ENABLES_CAE                          B_ICH_LPC_ENABLES_COMA_EN
#define     V_SRC_PREFETCH_EN_CACHE_EN                     V_ICH_SRC_PREF_EN_CACHE_EN
#define     V_SRC_PREFETCH_DIS_CACHE_DIS                   V_ICH_SRC_PREF_DIS_CACHE_DIS
#define     V_SRC_PREFETCH_DIS_CACHE_EN                    V_ICH_SRC_PREF_DIS_CACHE_EN
#define   B_ICH_LPC_GEN_PMCON_SLP_S4_ASSERT_EN           B_ICH_LPC_GEN_PMCON_SLP_S4_ASE
#define   B_ICH_LPC_GEN_PMCON_SLP_S4_MIN_ASSERT_WIDTH    B_ICH_LPC_GEN_PMCON_SLP_S4_MAW
#define     V_S4_MIN_ASSERT_1_2S                          V_ICH_LPC_GEN_PMCON_SLP_S4_MAW_1_2S
#define     V_S4_MIN_ASSERT_2_3S                          V_ICH_LPC_GEN_PMCON_SLP_S4_MAW_2_3S
#define     V_S4_MIN_ASSERT_3_4S                          V_ICH_LPC_GEN_PMCON_SLP_S4_MAW_3_4S
#define     V_S4_MIN_ASSERT_4_5S                          V_ICH_LPC_GEN_PMCON_SLP_S4_MAW_4_5S
#define   B_ICH_LPC_GEN_PMCON_SWSMI_RATE_SEL             B_ICH_LPC_GEN_PMCON_SWSMI_RTSL
#define     V_SWSMI_RATE_64MS                             V_ICH_LPC_GEN_PMCON_SWSMI_RTSL_64MS
#define     V_SWSMI_RATE_32MS                             V_ICH_LPC_GEN_PMCON_SWSMI_RTSL_32MS
#define     V_SWSMI_RATE_16MS                             V_ICH_LPC_GEN_PMCON_SWSMI_RTSL_16MS
#define     V_SWSMI_RATE_1_5MS                            V_ICH_LPC_GEN_PMCON_SWSMI_RTSL_1_5MS
#define   B_ICH_LPC_GEN_PMCON_ULOCK_C_STATE_TRANS        B_ICH_LPC_GEN_PMCON_LOCK_UCST
#define   B_ICH_LPC_GEN_PMCON_SLP_S4_STRETCH_LOCK_DOWN   B_ICH_LPC_GEN_PMCON_LOCK_S4_STRET_LD
#define   B_ICH_LPC_GEN_PMCON_ACPI_BASE_LOCK             B_ICH_LPC_GEN_PMCON_LOCK_ABASE_LK
#define   B_ICH_LPC_GEN_PMCON_C_STATE_CONFIG_LOCK        B_ICH_LPC_GEN_PMCON_LOCK_CST_CONF_LK
#define     V_PMSYNC_STPCLK_16_17PCICLKS                  V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_16_17PCLK   
#define     V_PMSYNC_STPCLK_80_86US                       V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_80_86US
#define     V_PMSYNC_STPCLK_99_105US                      V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_99_105US
#define     V_PMSYNC_STPCLK_118_124US                     V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_118_124US
#define     V_PMSYNC_STPCLK_18_24US                       V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_18_24US
#define     V_PMSYNC_STPCLK_38_44US                       V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_38_44US
#define     V_PMSYNC_STPCLK_56_62US                       V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_56_62US
#define     V_PMSYNC_STPCLK_199_205US                     V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_199_205US
#define   B_ICH_LPC_C5_EXIT_DPRSTP_TO_STPCPU             B_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU
#define     V_DPRSTP_STPCPU_22_28US                       V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_22_28US
#define     V_DPRSTP_STPCPU_34_40US                       V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_34_40US
#define     V_DPRSTP_STPCPU_56_62US                       V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_56_62US
#define     V_DPRSTP_STPCPU_95_102US                      V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_95_102US
#define     V_DPRSTP_STPCPU_138_144US                     V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_138_144US
#define     V_DPRSTP_STPCPU_72_78US                       V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_72_78US
#define     V_DPRSTP_STPCPU_199_205US                     V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_199_205US

//
// ACPI and legacy I/O register offsets from PMBASE
//
#define   V_ACPI_TIMER_MAX_VALUE                        V_ACPI_PM1_TMR_MAX_VAL
#define   B_ICH_ACPI_HOT_PLUG_STS                        B_HOT_PLUG_STS

//
//  SATA Controller common Registers
//
#define R_ICH_SDMA_CNT                                 0x48
#define   B_ICH_SDMA_CNT_PSDE1                           BIT1
#define   B_ICH_SDMA_CNT_PSDE0                           BIT0
#define R_ICH_SDMA_TIM                                 0x4A
#define   B_ICH_SDMA_TIM_PCT1_MASK                       0x0030
#define   B_ICH_SDMA_TIM_PCT0_MASK                       0x0003
#define   V_ICH_SDMA_TIM_PCT_33MH_CT4_RP6                0x00   //PCB1 = 0
#define   V_ICH_SDMA_TIM_PCT_33MH_CT3_RP5                0x01
#define   V_ICH_SDMA_TIM_PCT_33MH_CT2_RP4                0x02
#define   V_ICH_SDMA_TIM_PCT_66MH_CT3_RP8                0x01  //PCB1 = 1
#define   V_ICH_SDMA_TIM_PCT_66MH_CT2_RP8                0x02
#define   V_ICH_SDMA_TIM_PCT_133MH_CT3_RP16              0x01  //PCB1 = 1
#define R_ICH_IDE_CONFIG                               0x54
#define   V_ICH_IDE_CONFIG_SEC_SIG_EN                   (~(BIT19 | BIT18))
#define   V_ICH_IDE_CONFIG_PRIM_SIG_EN                  (~(BIT17 | BIT16))
#define   B_ICH_IDE_CONFIG_FAST_PCB1                     BIT13
#define   B_ICH_IDE_CONFIG_FAST_PCB0                     BIT12
#define   B_ICH_IDE_CONFIG_SSCCR_80_PIN                  BIT7   // Secondary Slave Channel Cable Reporting
#define   B_ICH_IDE_CONFIG_SMCCR_80_PIN                  BIT6   // Secondary Master Channel Cable Reporting
#define   B_ICH_IDE_CONFIG_PSCCR_80_PIN                  BIT5   // Primary Slave Channel Cable Reporting
#define   B_ICH_IDE_CONFIG_PMCCR_80_PIN                  BIT4   // Primary Master Channel Cable Reporting
#define   B_ICH_IDE_CONFIG_PCB1                          BIT1
#define   B_ICH_IDE_CONFIG_PCB0                          BIT0

//
//  Thermal Device Registers (D31:F6)
//
#define   ICH_TBARB_TS0E                               R_ICH_TBARB_TS0E
#define   ICH_TBARB_TS0TTP                             R_ICH_TBARB_TS0TTP
#define   ICH_TBARB_TS0CO                              R_ICH_TBARB_TS0CO
#define   ICH_TBARB_TS0PC                              R_ICH_TBARB_TS0PC
#define   ICH_TBARB_TS0LOCK                            R_ICH_TBARB_TS0LOCK
#define   ICH_TBARB_TS1E                               R_ICH_TBARB_TS1E
#define   ICH_TBARB_TS1TTP                             R_ICH_TBARB_TS1TTP
#define   ICH_TBARB_TS1CO                              R_ICH_TBARB_TS1CO
#define   ICH_TBARB_TS1PC                              R_ICH_TBARB_TS1PC
#define   ICH_TBARB_TS1LOCK                            R_ICH_TBARB_TS1LOCK

//
// SMBus Controller Registers (D31:F3)
//
#define   R_ICH_SMBUS_COMMAND                          R_ICH_SMBUS_PCICMD
#define     B_ICH_SMBUS_COMMAND_IOSE                     B_ICH_SMBUS_PCICMD_IOSE

//
// LAN Controller Registers (D25:F0)
//
#define R_ICH_LAN_COMMAND                              R_ICH_LAN_CMD
#define   B_ICH_LAN_COMMAND_BME                          B_ICH_LAN_CMD_BME
#define   B_ICH_LAN_COMMAND_MSE                          B_ICH_LAN_CMD_MSE

//
// Pci Express Root Ports (D28:F0~5)
//
#define PCI_DEVICE_NUMBER_ICH_PCIEXP                   28
#define PCI_MAX_PCIEXP_ROOT_PORTS                      6
#define PCI_ICH_MAX_PCIEXP_ROOT_PORTS                  6
#define PCI_FUNCTION_NUMBER_ICH_PCIEXP0                0
#define PCI_FUNCTION_NUMBER_ICH_PCIEXP1                1
#define PCI_FUNCTION_NUMBER_ICH_PCIEXP2                2
#define PCI_FUNCTION_NUMBER_ICH_PCIEXP3                3
#define PCI_FUNCTION_NUMBER_ICH_PCIEXP4                4
#define PCI_FUNCTION_NUMBER_ICH_PCIEXP5                5

#define R_ICH_PCIEXP_CMD                0x04    // Command reg
#define R_ICH_PCIEXP_BCTRL              0x3E    // Bridge control reg
#define   B_ICH_PCIEXP_BCTRL_PERE       BIT0    // Parity Error Response Enable 
#define   B_ICH_PCIEXP_BCTRL_SE         BIT1    // SERR  Enable 
#define R_ICH_PCIEXP_DCTL               0x48    // Device Control
#define R_ICH_PCIEXP_DSTS               0x4A    // Device Status
#define R_ICH_PCIEXP_LCAP               0x4C    // Link Capability Register                                                
#define   B_ICH_PCIEXP_LCAP_MLW         0x01F0  // Max Link Width                                                
#define R_ICH_PCIEXP_RCTL               0x5C    // Root Control
#define R_ICH_PCIEXP_MPC                0xD8    // Misc Port Config Reg
#define R_ICH_PCIEXP_RP_SMSCS           0xDC    // SMI/SCI Status Reg
#define R_ICH_PCIEXP_XCAP               0x42    // PCI Express capabilities Register
#define R_ICH_PCIEXP_SLCAP              0x54    // PCI Express slot capabilities Register
#define R_ICH_PCIEXP_SLSTS              0X5A    // Slot Status
#define R_ICH_PCIEXP_DEVICE_CONTROL     0x48    // Device Control Register
#define R_ICH_PCIEXP_SYSTEM_CONTROL     0x5C    // System Control Register
#define R_ICH_PCIEXP_SVID                 0x94    // Subsystem vendor/device ID 
#define R_ICH_PCIEXP_VC0_CONFIG         0xD0    // Virtual Channel0 config reg
#define R_ICH_PCIEXP_RWC                0xE0    // Resume Well Control
#define   B_ICH_PCIEXP_RWC_L10M           BIT1    // LTSSM 1.0 Mode
#define R_ICH_PCIEXP_RPDCGEN            0xE1    // Root Port Dynamic Clock Gate Enable
#define R_ICH_PCIEXP_VC0CTL             0x114   // Virtual Channel 0 Resouce Control
#define R_ICH_PCIEXP_VC1CTL             0x120   // Virtual Channel 1 Resouce Control
#define R_ICH_PCIEXP_PCIEDRCC0        0x030C    // PCI Express Lane 1 DRC Configuration
#define R_ICH_PCIEXP_PCIECFG2         0x0314    // PCI Express Configuration 2
#define R_ICH_PCIEXP_PCIEDBG          0x0318    // PCI Express Debug

//
// PCI Express link structure offsets    
//
//       PCI Express Capability Structure registers
#define PCI_EXP_CAP_XCAP                0x02              // PCI express capabilities
#define   PCI_EXP_CAP_XCAP_SI           BIT8              // Slot implemented
#define   PCI_EXP_CAP_XCAP_DT           BIT4+BIT5+BIT6+BIT7 // Device/port type
#define   PCI_EXP_CAP_XCAP_DT_UPSWITCH   BIT4+BIT6           // Up stream switch
#define   PCI_EXP_CAP_XCAP_DT_DOWNSWITCH BIT5+BIT6           // Down stream switch
#define PCI_EXP_CAP_DCAP                0x04              // Device capabilities
#define   PCI_EXP_CAP_DCAP_EXTTAG       BIT5              // Extended Tag Capability
#define   PCI_EXP_CAP_DCAP_L0S          (BIT8+BIT7+BIT6)    // L0s maximum acceptable latency
#define   PCI_EXP_CAP_DCAP_L1           (BIT11+BIT10+BIT9)  // L0s maximum acceptable latency
#define PCI_EXP_CAP_DCTL                0x08              // Device control
#define   PCI_EXP_CAP_DCTL_EXTTAG       BIT8              // Extended Tag Enable
#define   PCI_EXP_CAP_DCTL_NOSNOOP      BIT11             // No snoop
#define PCI_EXP_CAP_DSTS                0x0A              // Device status
#define PCI_EXP_CAP_LCAP                0x0C              // Link capabilitis
#define   PCI_EXP_CAP_LCAP_L0s_EL       (BIT14+BIT13+BIT12) // L0s exit latency
#define   PCI_EXP_CAP_LCAP_L1_EL        (BIT17+BIT16+BIT15) // L1 exit latency
#define   PCI_EXP_CAP_LCAP_ASPM         (BIT11+BIT10)       // ASPM support
#define   PCI_EXP_CAP_LCAP_L0s          BIT10             // L0s support only
#define   PCI_EXP_CAP_LCAP_L1           BIT11             // L1 support only
#define   PCI_EXP_CAP_LCAP_L0sL1        (BIT11+BIT10)       // L0s and L1 support
#define   PCI_EXP_CAP_LCAP_MLW            0x01F0            // Max Link Width
#define PCI_EXP_CAP_LCTL                0x10              // Link control
#define   PCI_EXP_CAP_LCTL_L0S          BIT0              // L0s only
#define   PCI_EXP_CAP_LCTL_L1           BIT1              // L0s only
#define   PCI_EXP_CAP_LCTL_L0SL1        (BIT1+BIT0)         // L0s and L1
#define   PCI_EXP_CAP_LCTL_RL           BIT5              // Retrain link
#define   PCI_EXP_CAP_LCTL_CCC          BIT6              // Common clock configuration
#define PCI_EXP_CAP_LSTS                0x12              // Link status
#define   PCI_EXP_CAP_LSTS_NLW          (BIT9+BIT8+BIT7+BIT6+BIT5+BIT4)
#define   PCI_EXP_CAP_LSTS_LT           BIT11             // Link training status
#define   PCI_EXP_CAP_LSTS_SCC          BIT12             // Slot clock configuration
#define PCI_EXP_CAP_SLCAP               0x14              // Slot capabilities
#define PCI_EXP_CAP_SLCTL               0x18              // Slot control
#define PCI_EXP_CAP_SLSTS               0x1A              // Slot status
#define PCI_EXP_CAP_RCTL                0x1C              // Root control
#define PCI_EXP_CAP_RSTS                0x20              // Root status

#define PCIEXP_ROOT_PORT_URE_ENABLE    BIT0   //	unsupported request reporting enable                         
#define PCIEXP_ROOT_PORT_FEE_ENABLE    BIT1   //	Fatal Error Reporting Enable                                 
#define PCIEXP_ROOT_PORT_NFE_ENABLE    BIT2   //	Non-Fatal Error Reporting Enable                             
#define PCIEXP_ROOT_PORT_CEE_ENABLE    BIT3   //	Correctable Error Reporting Enable                           
#define PCIEXP_ROOT_PORT_SFE_ENABLE    BIT4   //	System Error on Fatal Error Enable                           
#define PCIEXP_ROOT_PORT_SNE_ENABLE    BIT5   //	System Error on Non-Fatal Error Enable                       
#define PCIEXP_ROOT_PORT_SCE_ENABLE    BIT6   //	System Error on Correctable Error Enable    

//
// PCI-Ex AER registers
//
#define R_PCIEXP_AER_UES              0x144   // Uncorrectable error status
#define V_PCIEXP_AER_UES              (BIT0+BIT4+BIT12+BIT13+BIT14+BIT15+BIT16+BIT17+BIT18+BIT19+BIT20)   // Uncorrectable error status or value
#define R_PCIEXP_AER_UEM              0x148   // Uncorrectable error mask
#define V_PCIEXP_AER_UEM              (BIT0+BIT4+BIT12+BIT13+BIT14+BIT15+BIT16+BIT17+BIT18+BIT19+BIT20)   // Uncorrectable error mask
#define R_PCIEXP_AER_UEV              0x14C   // Uncorrectable error severity
#define V_PCIEXP_AER_UEV              (BIT0+BIT4+BIT12+BIT13+BIT14+BIT15+BIT16+BIT17+BIT18+BIT19+BIT20)   // Uncorrectable error severity

#define R_PCIEXP_AER_CES              0x150   // Correctable error status
#define V_PCIEXP_AER_CES              (BIT0+BIT6+BIT7+BIT8+BIT12+BIT13) // Correctable error status or value
#define R_PCIEXP_AER_CEM              0x154   // Correctable error mask
#define V_PCIEXP_AER_CEM              (BIT0+BIT6+BIT7+BIT8+BIT12+BIT13)   // Correctable error mask

#define R_PCIEXP_AER_REC              0x16c   // Root error command
#define R_PCIEXP_AER_RES              0x170   // Root error status


#define X16_SLOT_POWER_LIMIT            0x04b
#define ICH_RCRB_CID                    2
#define SB_VC1_MTS                      0x12
#define X4_SLOT_POWER_LIMIT             0x19
#define X2_SLOT_POWER_LIMIT             0x0A
#define X1_SLOT_POWER_LIMIT             0x0A
#define SB_RCRB_PN                      1
#define SB_RCRB_TCID                    1

//
// Chipset configuration registers (Memory space)
// RCBA
//
#define ICH_RCRB_VCH                    R_ICH_RCRB_VCH
#define ICH_RCRB_VCAP1                  R_ICH_RCRB_VCAP1
#define ICH_RCRB_VCAP2                  R_ICH_RCRB_VCAP2
#define ICH_RCRB_PVC                    R_ICH_RCRB_PVC
#define ICH_RCRB_PVS                    R_ICH_RCRB_PVS
#define ICH_RCRB_V0CAP                  R_ICH_RCRB_V0CAP
#define ICH_RCRB_V0CTL                  R_ICH_RCRB_V0CTL
#define ICH_RCRB_V0STS                  R_ICH_RCRB_V0STS
#define ICH_RCRB_V1CAP                  R_ICH_RCRB_V1CAP
#define ICH_RCRB_V1CTL                  R_ICH_RCRB_V1CTL
#define   B_RCRB_VCCTL_EN        BIT31            // Virtual channel enable
#define   B_RCRB_VCCTL_LAT       BIT16            // Load port arbitration table
#define   B_RCRB_VCSTS_NP        BIT1             // VC negotiation pending
#define   B_RCRB_VCSTS_ATS       BIT0             // VC port arbitration status
#define   B_RCRB_VCCTL_TVM       0xFF             // TC/VC mapping field
#define   B_RCRB_VCCTL_ID        (BIT26+BIT25+BIT24)  // VC ID
#define   B_RCRB_VCCTL_PAS       (BIT19+BIT18+BIT17)  // Port address selection
#define   B_RCRB_VCCTL_MTS       (0x7F << 16)     // Maximum time slots
#define   B_RCRB_V1CTL_EN                             BIT31
#define   V_RCRB_V1CTL_ID_MASK                        (3 << 24)
#define   V_RCRB_V1CTL_TVM_MASK                       0xFF
#define ICH_RCRB_V1STS                  R_ICH_RCRB_V1STS
#define   B_RCRB_V1STS_NP                             BIT1
#define   B_RCRB_V1STS_ATS                            BIT0
#define ICH_RCRB_V1PAT                  R_ICH_RCRB_V1PAT
#define ICH_RCRB_REC                    R_ICH_RCRB_REC
#define ICH_RCRB_RCTCL                  R_ICH_RCRB_RCTCL
#define ICH_RCRB_ESD                    R_ICH_RCRB_ESD
#define ICH_RCRB_ULD                    R_ICH_RCRB_ULD
#define ICH_RCRB_ULBA                   R_ICH_RCRB_ULBA
#define ICH_RCRB_RP1D                   R_ICH_RCRB_RP1D
#define ICH_RCRB_RP1BA                  R_ICH_RCRB_RP1BA
#define ICH_RCRB_RP2D                   R_ICH_RCRB_RP2D
#define ICH_RCRB_RP2BA                  R_ICH_RCRB_RP2BA
#define ICH_RCRB_RP3D                   R_ICH_RCRB_RP3D
#define ICH_RCRB_RP3BA                  R_ICH_RCRB_RP3BA
#define ICH_RCRB_RP4D                   R_ICH_RCRB_RP4D
#define ICH_RCRB_RP4BA                  R_ICH_RCRB_RP4BA
#define ICH_RCRB_HDD                    R_ICH_RCRB_HDD
#define ICH_RCRB_HDBA                   R_ICH_RCRB_HDBA
#define ICH_RCRB_RP5D                   R_ICH_RCRB_RP5D
#define ICH_RCRB_RP5BA                  R_ICH_RCRB_RP5BA
#define ICH_RCRB_RP6D                   R_ICH_RCRB_RP6D
#define ICH_RCRB_RP6BA                  R_ICH_RCRB_RP6BA
#define ICH_RCRB_ILCL                   R_ICH_RCRB_ILCL
#define ICH_RCRB_LCAP                   R_ICH_RCRB_LCAP
#define   B_RCRB_LCAP_APMS                B_ICH_RCRB_LCAP_APMS
#define ICH_RCRB_LCTL                   R_ICH_RCRB_LCTL
#define ICH_RCRB_LSTS                   R_ICH_RCRB_LSTS
#define ICH_RCRB_GBC                    R_ICH_RCRB_BCR
#define ICH_RCRB_RPC                    R_ICH_RCRB_RPC
#define ICH_RCRB_DMI_CNTL               R_ICH_RCRB_DMI_CNTL
#define   B_RCRB_DMI_CNTL_DMILCGEN        B_ICH_RCRB_DMI_CNTL_DMILCGEN
#define   B_RCRB_DMI_CNTL_DMIBCGEN        B_ICH_RCRB_DMI_CNTL_DMIBCGEN

#define ICH_RCRB_RPFN                   R_ICH_RCRB_RPFN
#define R_ICH_RCRB_RPFN_AND_RPH         R_ICH_RCRB_RPFN
#define   B_RCRB_RP1CH                    B_ICH_RCRB_RPFN_RP1CH
#define   B_RCRB_RP2CH                    B_ICH_RCRB_RPFN_RP2CH
#define   B_RCRB_RP3CH                    B_ICH_RCRB_RPFN_RP3CH
#define   B_RCRB_RP4CH                    B_ICH_RCRB_RPFN_RP4CH
#define   B_RCRB_RP5CH                    B_ICH_RCRB_RPFN_RP5CH
#define   B_RCRB_RP6CH                    B_ICH_RCRB_RPFN_RP6CH

#define ICH_RCRB_FLRSTAT                R_ICH_RCRB_FLRSTAT
#define ICH_RCRB_CIR13                  R_ICH_RCRB_CIR13
#define ICH_RCRB_CIR5                   R_ICH_RCRB_CIR5

#define ICH_RCRB_TRSR                   R_ICH_RCRB_TRSR
#define   B_TRSR_CTSS_MASK                B_ICH_RCRB_TRSR_CTSS
#define ICH_RCRB_TRCR                   R_ICH_RCRB_TRCR
#define ICH_RCRB_TRWDR                  R_ICH_RCRB_TRWDR
#define ICH_RCRB_IO_TRAP_0              R_ICH_RCRB_IO_TRAP_0
#define ICH_RCRB_IO_TRAP_1              R_ICH_RCRB_IO_TRAP_1
#define ICH_RCRB_IO_TRAP_2              R_ICH_RCRB_IO_TRAP_2
#define ICH_RCRB_IO_TRAP_3              R_ICH_RCRB_IO_TRAP_3
#define   B_TRAP_AND_SMI_ENABLE           B_ICH_RCRB_IO_TRAP_TRSE
#define ICH_RCRB_DMI_MISC_CNTL          R_ICH_RCRB_DMI_MISC_CNTL
#define ICH_RCRB_DMI_DBG                R_ICH_RCRB_CIR6
#define ICH_RCRB_DMI_DBG_FTS            R_ICH_RCRB_DMI_DBG_FTS
#define ICH_RCRB_CIR11                  R_ICH_RCRB_CIR11
#define ICH_RCRB_CIR12                  R_ICH_RCRB_CIR12
#define ICH_RCRB_TCO_CNTL               R_ICH_RCRB_TCO_CNTL
#define ICH_RCRB_D31IP                  R_ICH_RCRB_D31IP
#define ICH_RCRB_D30IP                  R_ICH_RCRB_D30IP
#define ICH_RCRB_D29IP                  R_ICH_RCRB_D29IP
#define ICH_RCRB_D28IP                  R_ICH_RCRB_D28IP
#define ICH_RCRB_D27IP                  R_ICH_RCRB_D27IP
#define ICH_RCRB_D26IP                  R_ICH_RCRB_D26IP
#define ICH_RCRB_D25IP                  R_ICH_RCRB_D25IP

#define ICH_RCRB_D31IR                  R_ICH_RCRB_D31IR
#define ICH_RCRB_D30IR                  R_ICH_RCRB_D30IR
#define ICH_RCRB_D29IR                  R_ICH_RCRB_D29IR
#define ICH_RCRB_D28IR                  R_ICH_RCRB_D28IR
#define ICH_RCRB_D27IR                  R_ICH_RCRB_D27IR
#define ICH_RCRB_D26IR                  R_ICH_RCRB_D26IR
#define ICH_RCRB_D25IR                  R_ICH_RCRB_D25IR
#define   B_DXXIR_IDR_MASK                B_ICH_RCRB_DXXIR_IDR_MASK
#define     V_DXXIR_IDR_PIRQA               V_ICH_RCRB_DXXIR_IDR_PIRQA
#define     V_DXXIR_IDR_PIRQB               V_ICH_RCRB_DXXIR_IDR_PIRQB
#define     V_DXXIR_IDR_PIRQC               V_ICH_RCRB_DXXIR_IDR_PIRQC
#define     V_DXXIR_IDR_PIRQD               V_ICH_RCRB_DXXIR_IDR_PIRQD
#define     V_DXXIR_IDR_PIRQE               V_ICH_RCRB_DXXIR_IDR_PIRQE
#define     V_DXXIR_IDR_PIRQF               V_ICH_RCRB_DXXIR_IDR_PIRQF
#define     V_DXXIR_IDR_PIRQG               V_ICH_RCRB_DXXIR_IDR_PIRQG
#define     V_DXXIR_IDR_PIRQH               V_ICH_RCRB_DXXIR_IDR_PIRQH
#define     V_DXXIR_ICR_PIRQA               V_ICH_RCRB_DXXIR_ICR_PIRQA
#define     V_DXXIR_ICR_PIRQB               V_ICH_RCRB_DXXIR_ICR_PIRQB
#define     V_DXXIR_ICR_PIRQC               V_ICH_RCRB_DXXIR_ICR_PIRQC
#define     V_DXXIR_ICR_PIRQD               V_ICH_RCRB_DXXIR_ICR_PIRQD
#define     V_DXXIR_ICR_PIRQE               V_ICH_RCRB_DXXIR_ICR_PIRQE
#define     V_DXXIR_ICR_PIRQF               V_ICH_RCRB_DXXIR_ICR_PIRQF
#define     V_DXXIR_ICR_PIRQG               V_ICH_RCRB_DXXIR_ICR_PIRQG
#define     V_DXXIR_ICR_PIRQH               V_ICH_RCRB_DXXIR_ICR_PIRQH
#define     V_DXXIR_IBR_PIRQA               V_ICH_RCRB_DXXIR_IBR_PIRQA
#define     V_DXXIR_IBR_PIRQB               V_ICH_RCRB_DXXIR_IBR_PIRQB
#define     V_DXXIR_IBR_PIRQC               V_ICH_RCRB_DXXIR_IBR_PIRQC
#define     V_DXXIR_IBR_PIRQD               V_ICH_RCRB_DXXIR_IBR_PIRQD
#define     V_DXXIR_IBR_PIRQE               V_ICH_RCRB_DXXIR_IBR_PIRQE
#define     V_DXXIR_IBR_PIRQF               V_ICH_RCRB_DXXIR_IBR_PIRQF
#define     V_DXXIR_IBR_PIRQG               V_ICH_RCRB_DXXIR_IBR_PIRQG
#define     V_DXXIR_IBR_PIRQH               V_ICH_RCRB_DXXIR_IBR_PIRQH
#define     V_DXXIR_IAR_PIRQA               V_ICH_RCRB_DXXIR_IAR_PIRQA
#define     V_DXXIR_IAR_PIRQB               V_ICH_RCRB_DXXIR_IAR_PIRQB
#define     V_DXXIR_IAR_PIRQC               V_ICH_RCRB_DXXIR_IAR_PIRQC
#define     V_DXXIR_IAR_PIRQD               V_ICH_RCRB_DXXIR_IAR_PIRQD
#define     V_DXXIR_IAR_PIRQE               V_ICH_RCRB_DXXIR_IAR_PIRQE
#define     V_DXXIR_IAR_PIRQF               V_ICH_RCRB_DXXIR_IAR_PIRQF
#define     V_DXXIR_IAR_PIRQG               V_ICH_RCRB_DXXIR_IAR_PIRQG
#define     V_DXXIR_IAR_PIRQH               V_ICH_RCRB_DXXIR_IAR_PIRQH

#define ICH_RCRB_OIC                    R_ICH_RCRB_OIC
#define   B_RCRB_OIC_CEN                  B_ICH_RCRB_OIC_CEN
#define   B_RCRB_OIC_AEN                  B_ICH_RCRB_OIC_AEN
#define ICH_RCRB_SBEMC3                 R_ICH_RCRB_SBEMC3
#define ICH_RCRB_SBEMC4                 R_ICH_RCRB_SBEMC4
#define ICH_RCRB_RTC_CONF               R_ICH_RCRB_RTC_CONF
#define   ICH_RCRB_RTC_CONF_UCMOS_LOCK    B_ICH_RCRB_RTC_CONF_UCMOS_LOCK
#define   ICH_RCRB_RTC_CONF_LCMOS_LOCK    B_ICH_RCRB_RTC_CONF_LCMOS_LOCK
#define   ICH_RCRB_RTC_CONF_UCMOS_EN      B_ICH_RCRB_RTC_CONF_UCMOS_EN
#define ICH_RCRB_HPTC                   R_ICH_RCRB_HPTC
#define   ICH_RCRB_HPTC_AS                B_ICH_RCRB_HPTC_AS
#define   ICH_RCRB_HPTC_AE                B_ICH_RCRB_HPTC_AE
#define ICH_RCRB_GCS                    R_ICH_RCRB_GCS
#define   B_RCRB_GCS_FLRCSSEL             B_ICH_RCRB_GCS_FLRCSSEL
#define   B_RCRB_GCS_BBS                  B_ICH_RCRB_GCS_BBS
#define     V_RCRB_GCS_BBS_SPI              V_ICH_RCRB_GCS_BBS_SPI
#define     V_RCRB_GCS_BBS_PCI              V_ICH_RCRB_GCS_BBS_PCI
#define     V_RCRB_GCS_BBS_LPC              V_ICH_RCRB_GCS_BBS_LPC
#define   B_RCRB_GCS_SERM                 B_ICH_RCRB_GCS_SERM
#define   B_RCRB_GCS_MICLD                B_ICH_RCRB_GCS_MICLD
#define   B_RCRB_GCS_FME                  B_ICH_RCRB_GCS_FME
#define   B_RCRB_GCS_NR                   B_ICH_RCRB_GCS_NR
#define   B_RCRB_GCS_AME                  B_ICH_RCRB_GCS_AME
#define   B_RCRB_GCS_SPS                  B_ICH_RCRB_GCS_SPS
#define   B_RCRB_GCS_RPR                  B_ICH_RCRB_GCS_RPR
#define   B_RCRB_GCS_BILD                 B_ICH_RCRB_GCS_BILD
#define   N_RCRB_GCS_BBS                  N_ICH_RCRB_GCS_BBS
#define ICH_RCRB_BUC                    R_ICH_RCRB_BUC
#define   B_RCRB_BUC_LAN_DIS              B_ICH_RCRB_BUC_LAN_DIS
#define   B_RCRB_BUC_SDO                  B_ICH_RCRB_BUC_SDO
#define   B_RCRB_BUC_CBE                  B_ICH_RCRB_BUC_CBE
#define   B_RCRB_BUC_PRS                  B_ICH_RCRB_BUC_PRS
#define   B_RCRB_BUC_TS                   B_ICH_RCRB_BUC_TS
#define ICH_RCRB_FUNC_DIS               R_ICH_RCRB_FUNC_DIS
#define   B_RCRB_FUNC_DIS_FUNCTION_0      B_ICH_RCRB_FUNC_DIS_FUNCTION_0
#define   B_RCRB_FUNC_DIS_PATA            B_ICH_RCRB_FUNC_DIS_PATA
#define   B_RCRB_FUNC_DIS_SATA1           B_ICH_RCRB_FUNC_DIS_SATA1
#define   B_RCRB_FUNC_DIS_SMBUS           B_ICH_RCRB_FUNC_DIS_SMBUS
#define   B_RCRB_FUNC_DIS_AZALIA          B_ICH_RCRB_FUNC_DIS_AZALIA
#define   B_RCRB_FUNC_DIS_UHCI1           B_ICH_RCRB_FUNC_DIS_UHCI1
#define   B_RCRB_FUNC_DIS_UHCI2           B_ICH_RCRB_FUNC_DIS_UHCI2
#define   B_RCRB_FUNC_DIS_UHCI3           B_ICH_RCRB_FUNC_DIS_UHCI3
#define   B_RCRB_FUNC_DIS_UHCI4           B_ICH_RCRB_FUNC_DIS_UHCI4
#define   B_RCRB_FUNC_DIS_UHCI5           B_ICH_RCRB_FUNC_DIS_UHCI5
#define   B_RCRB_FUNC_DIS_UHCI6           B_ICH_RCRB_FUNC_DIS_UHCI6
#define   B_RCRB_FUNC_DIS_EHCI2           B_ICH_RCRB_FUNC_DIS_EHCI2
#define   B_RCRB_FUNC_DIS_LPC_BRIDGE      B_ICH_RCRB_FUNC_DIS_LPC_BRIDGE
#define   B_RCRB_FUNC_DIS_EHCI1           B_ICH_RCRB_FUNC_DIS_EHCI1
#define   B_RCRB_FUNC_DIS_PCI_EX_PORT1    B_ICH_RCRB_FUNC_DIS_PCI_EX_PORT1
#define   B_RCRB_FUNC_DIS_PCI_EX_PORT2    B_ICH_RCRB_FUNC_DIS_PCI_EX_PORT2
#define   B_RCRB_FUNC_DIS_PCI_EX_PORT3    B_ICH_RCRB_FUNC_DIS_PCI_EX_PORT3
#define   B_RCRB_FUNC_DIS_PCI_EX_PORT4    B_ICH_RCRB_FUNC_DIS_PCI_EX_PORT4
#define   B_RCRB_FUNC_DIS_PCI_EX_PORT5    B_ICH_RCRB_FUNC_DIS_PCI_EX_PORT5
#define   B_RCRB_FUNC_DIS_PCI_EX_PORT6    B_ICH_RCRB_FUNC_DIS_PCI_EX_PORT6
#define   B_RCRB_FUNC_DIS_THERMAL         B_ICH_RCRB_FUNC_DIS_THERMAL
#define   B_RCRB_FUNC_DIS_SATA2           B_ICH_RCRB_FUNC_DIS_SATA2
#define ICH_RCRB_PRC                    R_ICH_RCRB_PRC
#define   B_RCRB_PRC_EN_DCLK_GATE_LPC       B_ICH_RCRB_PRC_EN_DCLK_GATE_LPC
#define   B_RCRB_PRC_EN_DCLK_GATE_PATA      B_ICH_RCRB_PRC_EN_DCLK_GATE_PATA
#define   B_RCRB_PRC_EN_DCLK_GATE_UHCI      B_ICH_RCRB_PRC_EN_DCLK_GATE_UHCI
#define   B_RCRB_PRC_EN_DCLK_GATE_SATA3     B_ICH_RCRB_PRC_EN_DCLK_GATE_SATA3
#define   B_RCRB_PRC_EN_DCLK_GATE_SATA2     B_ICH_RCRB_PRC_EN_DCLK_GATE_SATA2
#define   B_RCRB_PRC_EN_DCLK_GATE_SATA1     B_ICH_RCRB_PRC_EN_DCLK_GATE_SATA1
#define   B_RCRB_PRC_EN_DCLK_GATE_SATA0     B_ICH_RCRB_PRC_EN_DCLK_GATE_SATA0
#define   B_RCRB_PRC_EN_SCLK_GATE_LAN       B_ICH_RCRB_PRC_EN_SCLK_GATE_LAN
#define   B_RCRB_PRC_EN_DCLK_GATE_HDA       B_ICH_RCRB_PRC_EN_DCLK_GATE_HDA
#define   B_RCRB_PRC_EN_SCLK_GATE_HDA       B_ICH_RCRB_PRC_EN_SCLK_GATE_HDA
#define   B_RCRB_PRC_EN_SCLK_GATE_EHCI      B_ICH_RCRB_PRC_EN_SCLK_GATE_EHCI
#define   B_RCRB_PRC_EN_DCLK_GATE_EHCI      B_ICH_RCRB_PRC_EN_DCLK_GATE_EHCI
#define   B_RCRB_PRC_EN_DCLK_GATE_SATA5     B_ICH_RCRB_PRC_EN_DCLK_GATE_SATA5
#define   B_RCRB_PRC_EN_DCLK_GATE_SATA4     B_ICH_RCRB_PRC_EN_DCLK_GATE_SATA4
#define   B_RCRB_PRC_EN_DCLK_GATE_PCI       B_ICH_RCRB_PRC_EN_DCLK_GATE_PCI
#define   B_RCRB_PRC_DIS_IDE_C3_BIASING     B_ICH_RCRB_PRC_DIS_IDE_C3_BIASING
#define   B_RCRB_PRC_EN_DCLK_GATE_SMBUS     B_ICH_RCRB_PRC_EN_DCLK_GATE_SMBUS
#define   B_RCRB_PRC_EN_PCIE_RX_GATE        B_ICH_RCRB_PRC_EN_PCIE_RX_GATE
#define   B_RCRB_PRC_EN_DCLK_GATE_RX        B_ICH_RCRB_PRC_EN_DCLK_GATE_RX
#define   B_RCRB_PRC_EN_DCLK_GATE_PCIE_TX   B_ICH_RCRB_PRC_EN_DCLK_GATE_PCIE_TX
#define   B_RCRB_PRC_EN_DCLK_GATE_DMI_TX    B_ICH_RCRB_PRC_EN_DCLK_GATE_DMI_TX
#define   B_RCRB_PRC_EN_SCLK_GATE_PCIE_RP   B_ICH_RCRB_PRC_EN_SCLK_GATE_PCIE_RP
#define   B_RCRB_FDSW_FDSWL                 B_ICH_RCRB_FDSW_FDSWL
#define   B_RCRB_FDSW_LAN_DIS               B_ICH_RCRB_FDSW_LAN_DIS
#define   B_ICH_RCBA_CIR8_FIELD_1_MASK      B_ICH_RCRB_CIR8_FIELD_1_MASK
#define   B_ICH_USB_PORT9_INIT_FIELD_MASK   B_ICH_RCRB_USBIR1_PORT9_INIT_FIELD_MASK
#define   B_ICH_USB_PORT8_INIT_FIELD_MASK   B_ICH_RCRB_USBIR1_PORT8_INIT_FIELD_MASK
#define   B_ICH_USB_PORT7_INIT_FIELD_MASK   B_ICH_RCRB_USBIR1_PORT7_INIT_FIELD_MASK
#define   B_ICH_USB_PORT6_INIT_FIELD_MASK   B_ICH_RCRB_USBIR1_PORT6_INIT_FIELD_MASK
#define   B_ICH_USB_PORT5_INIT_FIELD_MASK   B_ICH_RCRB_USBIR1_PORT5_INIT_FIELD_MASK
#define   B_ICH_USB_PORT4_INIT_FIELD_MASK   B_ICH_RCRB_USBIR1_PORT4_INIT_FIELD_MASK
#define   B_ICH_USB_PORT3_INIT_FIELD_MASK   B_ICH_RCRB_USBIR1_PORT3_INIT_FIELD_MASK
#define   B_ICH_USB_PORT2_INIT_FIELD_MASK   B_ICH_RCRB_USBIR1_PORT2_INIT_FIELD_MASK
#define   B_ICH_USB_PORT1_INIT_FIELD_MASK   B_ICH_RCRB_USBIR1_PORT1_INIT_FIELD_MASK
#define   B_ICH_USB_PORT0_INIT_FIELD_MASK   B_ICH_RCRB_USBIR1_PORT0_INIT_FIELD_MASK
#define   B_ICH_USB_PORT11_INIT_FIELD_MASK  B_ICH_RCRB_USBIR2_PORT11_INIT_FIELD_MASK
#define   B_ICH_USB_PORT10_INIT_FIELD_MASK  B_ICH_RCRB_USBIR2_PORT10_INIT_FIELD_MASK
#define   B_SPI_FDATA00_FD                  B_ICH_SPI_FDATA00_FD
#define R_ICH_RCRB_MAP                  R_ICH_RCRB_REMAP_CONTROL
#define   B_ICH_RCRB_MAP_UHCI6_REMAP      B_ICH_RCRB_UHCI6_REMAP    // UHCI Controller 6 remap

//
// SPI Host Interface Registers
//
#define ICH_RCRB_SPI_BASE               R_ICH_RCRB_SPI_BASE             
#define ICH_SPI_BFPR                    R_ICH_SPI_BFPR
#define ICH_SPI_HSFS                    R_ICH_SPI_HSFS
#define   B_SPI_HSFS_FLOCKDN              B_ICH_SPI_HSFS_FLOCKDN
#define   B_SPI_HSFS_FDV                  B_ICH_SPI_HSFS_FDV
#define   B_SPI_HSFS_FDOPSS               B_ICH_SPI_HSFS_FDOPSS
#define   B_SPI_HSFS_SCIP                 B_ICH_SPI_HSFS_SCIP
#define   B_SPI_HSFS_BERASE_MASK          B_ICH_SPI_HSFS_BERASE_MASK
#define     V_SPI_HSFS_BERASE_256B          V_ICH_SPI_HSFS_BERASE_256B
#define     V_SPI_HSFS_BERASE_4K            V_ICH_SPI_HSFS_BERASE_4K
#define     V_SPI_HSFS_BERASE_64K           V_ICH_SPI_HSFS_BERASE_64K
#define   B_SPI_HSFS_AEL                  B_ICH_SPI_HSFS_AEL
#define   B_SPI_HSFS_FCERR                B_ICH_SPI_HSFS_FCERR
#define   B_SPI_HSFS_FDONE                B_ICH_SPI_HSFS_FDONE
#define ICH_SPI_HSFC                    R_ICH_SPI_HSFC
#define   B_SPI_HSFC_FSMIE                B_ICH_SPI_HSFC_FSMIE
#define   B_SPI_HSFC_FDBC_MASK            B_ICH_SPI_HSFC_FDBC_MASK
#define     V_SPI_SPID_MAX                  V_ICH_SPI_SPID_MAX
#define   B_SPI_HSFC_FCYCLE_MASK          B_ICH_SPI_HSFC_FCYCLE_MASK
#define     V_SPI_HSFC_FCYCLE_READ          V_ICH_SPI_HSFC_FCYCLE_READ
#define     V_SPI_HSFC_FCYCLE_WRITE         V_ICH_SPI_HSFC_FCYCLE_WRITE
#define     V_SPI_HSFC_FCYCLE_ERASE         V_ICH_SPI_HSFC_FCYCLE_ERASE
#define   B_SPI_HSFC_FCYCLE_FGO           B_ICH_SPI_HSFC_FCYCLE_FGO
#define ICH_SPI_FADDR                   R_ICH_SPI_FADDR
#define ICH_SPI_FDATA00                 R_ICH_SPI_FDATA00
#define   B_SPI_FDATA00_FD                B_ICH_SPI_FDATA00_FD
#define ICH_SPI_FDATA01                 R_ICH_SPI_FDATA01
#define ICH_SPI_FDATA02                 R_ICH_SPI_FDATA02
#define ICH_SPI_FDATA03                 R_ICH_SPI_FDATA03
#define ICH_SPI_FDATA04                 R_ICH_SPI_FDATA04
#define ICH_SPI_FDATA05                 R_ICH_SPI_FDATA05
#define ICH_SPI_FDATA06                 R_ICH_SPI_FDATA06
#define ICH_SPI_FDATA07                 R_ICH_SPI_FDATA07
#define ICH_SPI_FDATA08                 R_ICH_SPI_FDATA08
#define ICH_SPI_FDATA09                 R_ICH_SPI_FDATA09
#define ICH_SPI_FDATA10                 R_ICH_SPI_FDATA10
#define ICH_SPI_FDATA11                 R_ICH_SPI_FDATA11
#define ICH_SPI_FDATA12                 R_ICH_SPI_FDATA12
#define ICH_SPI_FDATA13                 R_ICH_SPI_FDATA13
#define ICH_SPI_FDATA14                 R_ICH_SPI_FDATA14
#define ICH_SPI_FDATA15                 R_ICH_SPI_FDATA15
#define ICH_SPI_FRAP                    R_ICH_SPI_FRAP
#define   B_SPI_FRAP_BMWAG_MASK           B_ICH_SPI_FRAP_BMWAG_MASK
#define   B_SPI_FRAP_BMWAG_BIOS           B_ICH_SPI_FRAP_BMWAG_BIOS
#define   B_SPI_FRAP_BMWAG_ME             B_ICH_SPI_FRAP_BMWAG_ME
#define   B_SPI_FRAP_BMWAG_GBE            B_ICH_SPI_FRAP_BMWAG_GBE
#define   B_SPI_FRAP_BMRAG_MASK           B_ICH_SPI_FRAP_BMRAG_MASK
#define   B_SPI_FRAP_BMRAG_BIOS           B_ICH_SPI_FRAP_BMRAG_BIOS
#define   B_SPI_FRAP_BMRAG_ME             B_ICH_SPI_FRAP_BMRAG_ME
#define   B_SPI_FRAP_BMRAG_GBE            B_ICH_SPI_FRAP_BMRAG_GBE
#define   B_SPI_FRAP_BRWA_MASK            B_ICH_SPI_FRAP_BRWA_MASK
#define   B_SPI_FRAP_BRWA_FLASHD          B_ICH_SPI_FRAP_BRWA_FLASHD
#define   B_SPI_FRAP_BRWA_BIOS            B_ICH_SPI_FRAP_BRWA_BIOS
#define   B_SPI_FRAP_BRWA_ME              B_ICH_SPI_FRAP_BRWA_ME
#define   B_SPI_FRAP_BRWA_GBE             B_ICH_SPI_FRAP_BRWA_GBE
#define   B_SPI_FRAP_BRRA_MASK            B_ICH_SPI_FRAP_BRRA_MASK
#define   B_SPI_FRAP_BRRA_FLASHD          B_ICH_SPI_FRAP_BRRA_FLASHD
#define   B_SPI_FRAP_BRRA_BIOS            B_ICH_SPI_FRAP_BRRA_BIOS
#define   B_SPI_FRAP_BRRA_ME              B_ICH_SPI_FRAP_BRRA_ME
#define   B_SPI_FRAP_BRRA_GBE             B_ICH_SPI_FRAP_BRRA_GBE
#define ICH_SPI_FREG0_FLASHD            R_ICH_SPI_FREG0_FLASHD
#define   B_SPI_FREG0_LIMIT_MASK          B_ICH_SPI_FREG0_LIMIT_MASK
#define   B_SPI_FREG0_BASE_MASK           B_ICH_SPI_FREG0_BASE_MASK
#define ICH_SPI_FREG1_BIOS              R_ICH_SPI_FREG1_BIOS
#define   B_SPI_FREG1_LIMIT_MASK          B_ICH_SPI_FREG1_LIMIT_MASK
#define   B_SPI_FREG1_BASE_MASK           B_ICH_SPI_FREG1_BASE_MASK
#define ICH_SPI_FREG2_ME                R_ICH_SPI_FREG2_ME
#define   B_SPI_FREG2_LIMIT_MASK          B_ICH_SPI_FREG2_LIMIT_MASK
#define   B_SPI_FREG2_BASE_MASK           B_ICH_SPI_FREG2_BASE_MASK
#define ICH_SPI_FREG3_GBE               R_ICH_SPI_FREG3_GBE
#define   B_SPI_FREG3_LIMIT_MASK          B_ICH_SPI_FREG3_LIMIT_MASK
#define   B_SPI_FREG3_BASE_MASK           B_ICH_SPI_FREG3_BASE_MASK
#define ICH_SPI_FREG4_GBE               R_ICH_SPI_FREG4_GBE
#define   B_SPI_FREG4_LIMIT_MASK          B_ICH_SPI_FREG4_LIMIT_MASK
#define   B_SPI_FREG4_BASE_MASK           B_ICH_SPI_FREG4_BASE_MASK
#define ICH_SPI_PR0                     R_ICH_SPI_PR0
#define   B_SPI_PR0_WPE                   B_ICH_SPI_PR0_WPE
#define   B_SPI_PR0_PRL_MASK              B_ICH_SPI_PR0_PRL_MASK
#define   B_SPI_PR0_RPE                   B_ICH_SPI_PR0_RPE
#define   B_SPI_PR0_PRB_MASK              B_ICH_SPI_PR0_PRB_MASK
#define ICH_SPI_PR1                     R_ICH_SPI_PR1
#define   B_SPI_PR1_WPE                   B_ICH_SPI_PR1_WPE
#define   B_SPI_PR1_PRL_MASK              B_ICH_SPI_PR1_PRL_MASK
#define   B_SPI_PR1_RPE                   B_ICH_SPI_PR1_RPE
#define   B_SPI_PR1_PRB_MASK              B_ICH_SPI_PR1_PRB_MASK
#define ICH_SPI_PR2                     R_ICH_SPI_PR2
#define   B_SPI_PR2_WPE                   B_ICH_SPI_PR2_WPE
#define   B_SPI_PR2_PRL_MASK              B_ICH_SPI_PR2_PRL_MASK
#define   B_SPI_PR2_RPE                   B_ICH_SPI_PR2_RPE
#define   B_SPI_PR2_PRB_MASK              B_ICH_SPI_PR2_PRB_MASK
#define ICH_SPI_PR3                     R_ICH_SPI_PR3
#define   B_SPI_PR3_WPE                   B_ICH_SPI_PR3_WPE
#define   B_SPI_PR3_PRL_MASK              B_ICH_SPI_PR3_PRL_MASK
#define   B_SPI_PR3_RPE                   B_ICH_SPI_PR3_RPE
#define   B_SPI_PR3_PRB_MASK              B_ICH_SPI_PR3_PRB_MASK
#define ICH_SPI_PR4                     R_ICH_SPI_PR4
#define   B_SPI_PR4_WPE                   B_ICH_SPI_PR4_WPE
#define   B_SPI_PR4_PRL_MASK              B_ICH_SPI_PR4_PRL_MASK
#define   B_SPI_PR4_RPE                   B_ICH_SPI_PR4_RPE
#define   B_SPI_PR4_PRB_MASK              B_ICH_SPI_PR4_PRB_MASK
#define ICH_SPI_SSFS                    R_ICH_SPI_SSFS
#define   B_SPI_SSFS_AEL                  B_ICH_SPI_SSFS_AEL
#define   B_SPI_SSFS_FCERR                B_ICH_SPI_SSFS_FCERR
#define   B_SPI_SSFS_CDS                  B_ICH_SPI_SSFS_CDS
#define   B_SPI_SSFS_SCIP                 B_ICH_SPI_SSFS_SCIP
#define ICH_SPI_SSFC                    R_ICH_SPI_SSFC
#define   B_SPI_SSFC_SCF_MASK             B_ICH_SPI_SSFC_SCF_MASK
#define     V_SPI_SSFC_SCF_20MHZ            V_ICH_SPI_SSFC_SCF_20MHZ
#define     V_SPI_SSFC_SCF_33MHZ            V_ICH_SPI_SSFC_SCF_33MHZ
#define     V_SPI_SSFC_SCF_66MHZ            V_ICH_SPI_SSFC_SCF_66MHZ
#define   B_SPI_SSFC_SME                  B_ICH_SPI_SSFC_SME
#define   B_SPI_SSFC_DS                   B_ICH_SPI_SSFC_DS
#define   B_SPI_SSFC_DBC_MASK             B_ICH_SPI_SSFC_DBC_MASK
#define   B_SPI_SSFC_COP                  B_ICH_SPI_SSFC_COP
#define   B_SPI_SSFC_SPOP                 B_ICH_SPI_SSFC_SPOP
#define   B_SPI_SSFC_ACS                  B_ICH_SPI_SSFC_ACS
#define   B_SPI_SSFC_SCGO                 B_ICH_SPI_SSFC_SCGO
#define ICH_SPI_PREOP                   R_ICH_SPI_PREOP
#define   B_SPI_PREOP1_MASK               B_ICH_SPI_PREOP1_MASK
#define   B_SPI_PREOP0_MASK               B_ICH_SPI_PREOP0_MASK
#define ICH_SPI_OPTYPE                  R_ICH_SPI_OPTYPE
#define   B_SPI_OPTYPE0_MASK              B_ICH_SPI_OPTYPE0_MASK
#define   B_SPI_OPTYPE1_MASK              B_ICH_SPI_OPTYPE1_MASK
#define   B_SPI_OPTYPE2_MASK              B_ICH_SPI_OPTYPE2_MASK
#define   B_SPI_OPTYPE3_MASK              B_ICH_SPI_OPTYPE3_MASK
#define   B_SPI_OPTYPE4_MASK              B_ICH_SPI_OPTYPE4_MASK
#define   B_SPI_OPTYPE5_MASK              B_ICH_SPI_OPTYPE5_MASK
#define   B_SPI_OPTYPE6_MASK              B_ICH_SPI_OPTYPE6_MASK
#define   B_SPI_OPTYPE7_MASK              B_ICH_SPI_OPTYPE7_MASK
#define     V_SPI_OPTYPE_RDNOADDR           V_ICH_SPI_OPTYPE_RDNOADDR
#define     V_SPI_OPTYPE_WRNOADDR           V_ICH_SPI_OPTYPE_WRNOADDR
#define     V_SPI_OPTYPE_RDADDR             V_ICH_SPI_OPTYPE_RDADDR
#define     V_SPI_OPTYPE_WRADDR             V_ICH_SPI_OPTYPE_WRADDR
#define ICH_SPI_OPMENU                  R_ICH_SPI_OPMENU
#define ICH_SPI_BBAR                    R_ICH_SPI_BBAR
#define ICH_SPI_FDOC                    R_ICH_SPI_FDOC
#define   B_SPI_FDOC_FDSS_MASK            B_ICH_SPI_FDOC_FDSS_MASK
#define     V_SPI_FDOC_FDSS_FSDM            V_ICH_SPI_FDOC_FDSS_FSDM
#define     V_SPI_FDOC_FDSS_COMP            V_ICH_SPI_FDOC_FDSS_COMP
#define     V_SPI_FDOC_FDSS_REGN            V_ICH_SPI_FDOC_FDSS_REGN
#define     V_SPI_FDOC_FDSS_MSTR            V_ICH_SPI_FDOC_FDSS_MSTR
#define     V_SPI_FDOC_FDSS_ICHS            V_ICH_SPI_FDOC_FDSS_ICHS
#define   B_SPI_FDOC_FDSI_MASK            B_ICH_SPI_FDOC_FDSI_MASK
#define ICH_SPI_FDOD                    R_ICH_SPI_FDOD
#define ICH_RCRB_SPI_CLK_GATE           R_ICH_RCRB_SPI_CLK_GATE
#define ICH_SPI_LVSCC                   R_ICH_SPI_LVSCC
#define   B_SPI_LVSCC_VCL                 B_ICH_SPI_LVSCC_VCL 
#define   B_SPI_LVSCC_EO_MASK             B_ICH_SPI_LVSCC_EO_MASK
#define   B_SPI_LVSCC_WEWS                B_ICH_SPI_LVSCC_WEWS
#define   B_SPI_LVSCC_WSR                 B_ICH_SPI_LVSCC_WSR
#define   B_SPI_LVSCC_WG_64B              B_ICH_SPI_LVSCC_WG_64B
#define   B_SPI_LVSCC_BSES_MASK           B_ICH_SPI_LVSCC_BSES_MASK
#define     V_SPI_LVSCC_BSES_256B           V_ICH_SPI_LVSCC_BSES_256B
#define     V_SPI_LVSCC_BSES_4K             V_ICH_SPI_LVSCC_BSES_4K
#define     V_SPI_LVSCC_BSES_64K            V_ICH_SPI_LVSCC_BSES_64K
#define ICH_SPI_UVSCC                   R_ICH_SPI_UVSCC
#define   B_SPI_UVSCC_VCL                 B_ICH_SPI_UVSCC_VCL
#define   B_SPI_UVSCC_EO_MASK             B_ICH_SPI_UVSCC_EO_MASK
#define   B_SPI_UVSCC_WEWS                B_ICH_SPI_UVSCC_WEWS
#define   B_SPI_UVSCC_WSR                 B_ICH_SPI_UVSCC_WSR
#define   B_SPI_UVSCC_WG_64B              B_ICH_SPI_UVSCC_WG_64B
#define   B_SPI_UVSCC_BSES_MASK           B_ICH_SPI_UVSCC_BSES_MASK
#define     V_SPI_UVSCC_BSES_256B           V_ICH_SPI_UVSCC_BSES_256B
#define     V_SPI_UVSCC_BSES_4K             V_ICH_SPI_UVSCC_BSES_4K
#define     V_SPI_UVSCC_BSES_64K            V_ICH_SPI_UVSCC_BSES_64K
#define ICH_SPI_FPB                     R_ICH_SPI_FPB
#define   B_SPI_FPB_FPBA_MASK             B_ICH_SPI_FPB_FPBA_MASK

//
// Flash Descriptor Base Address Region (FDBAR) from Flash Region 0
//
#define SPI_FDBAR_FLVALSIG              R_SPI_FDBAR_FLVALSIG
#define SPI_FDBAR_FLASH_MAP0            R_SPI_FDBAR_FLASH_MAP0
#define SPI_FDBAR_FLASH_MAP1            R_SPI_FDBAR_FLASH_MAP1
#define SPI_FDBAR_FLASH_MAP2            R_SPI_FDBAR_FLASH_MAP2
#define SPI_FDBAR_FLASH_UPPER_MAP1      R_SPI_FDBAR_FLASH_UPPER_MAP1
#define SPI_FDBAR_VTBA_VSCC0            R_SPI_FDBAR_VTBA_VSCC0
#define SPI_FDBAR_VTBA_VSCC1            R_SPI_FDBAR_VTBA_VSCC1

//
// Miscellaneous 
//

#define PCI_PCIE_ADDR(Bus,Device,Function,Offset) \
  (((Offset) & 0xfff) | (((Function) & 0x07) << 12) | (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

#define PCI_CF8_ADDR(Bus, Dev, Func, Off) \
          (((Off) & 0xFF) | (((Func) & 0x07) << 8) | (((Dev) & 0x1F) << 11) | (((Bus) & 0xFF) << 16) | (1 << 31))

#define PCI_PCIR_ADDR(Bus, Dev, Func, Off) \
          ((UINT64) ((((UINTN) (Func)) << 16) + (((UINTN)(Dev)) << 8) + ((UINTN) ((Off)))))

#define ICH_LPC_CF8_ADDR(Offset)    PCI_CF8_ADDR(0, PCI_DEVICE_NUMBER_ICH_LPC, PCI_FUNCTION_NUMBER_ICH_LPC, Offset)
#define ICH_LPC_PCIE_ADDR(Offset)   PCI_PCIE_ADDR(0, PCI_DEVICE_NUMBER_ICH_LPC, PCI_FUNCTION_NUMBER_ICH_LPC, Offset)
#define ICH_LPC_PCIR_ADDR(Offset)   PCI_PCIR_ADDR(0, PCI_DEVICE_NUMBER_ICH_LPC, PCI_FUNCTION_NUMBER_ICH_LPC, Offset)

#define ICH_SIDE1_PCIR_ADDR(Offset) PCI_PCIR_ADDR(0, PCI_DEVICE_NUMBER_ICH_SIDE, PCI_FUNCTION_NUMBER_ICH_SIDE, Offset)
#define ICH_SIDE1_PCIE_ADDR(Offset) PCI_PCIE_ADDR(0, PCI_DEVICE_NUMBER_ICH_SIDE, PCI_FUNCTION_NUMBER_ICH_SIDE, Offset)

#define ICH_SIDE2_PCIR_ADDR(Offset) PCI_PCIR_ADDR(0, PCI_DEVICE_NUMBER_ICH_SIDE, PCI_FUNCTION_NUMBER_ICH_SIDE2, Offset)
#define ICH_SIDE2_PCIE_ADDR(Offset) PCI_PCIE_ADDR(0, PCI_DEVICE_NUMBER_ICH_SIDE, PCI_FUNCTION_NUMBER_ICH_SIDE2, Offset)

#define ICH_HL2P_PCIR_ADDR(Offset)  PCI_PCIR_ADDR(0, PCI_DEVICE_NUMBER_ICH_HL2P, PCI_FUNCTION_NUMBER_ICH_HL2P, Offset)
#define ICH_HL2P_PCIE_ADDR(Offset)  PCI_PCIE_ADDR(0, PCI_DEVICE_NUMBER_ICH_HL2P, PCI_FUNCTION_NUMBER_ICH_HL2P, Offset)

#define   V_RCBA_BASE_MASK                            0xFFFFC000


//
// ICH I/O Port Defines
//
#define R_ICH_IOPORT_PCI_INDEX                      0xCF8
#define R_ICH_IOPORT_PCI_DATA                       0xCFC

#define R_ICH_IOPORT_A20_INIT                       0x92
#define   B_ICH_IOPORT_A20_INIT_NOW                   BIT0

#define R_ICH_IOPORT_CMOS_STANDARD_INDEX            0x70
#define R_ICH_IOPORT_CMOS_STANDARD_DATA             0x71

#define R_ICH_IOPORT_CMOS_UPPER_INDEX               0x72
#define R_ICH_IOPORT_CMOS_UPPER_DATA                0x73

#define R_ICH_IOPORT_RST_CNT 0xCF9
#define   B_ICH_IOPORT_RST_CNT_SYS_RST                BIT1
#define   B_ICH_IOPORT_RST_CNT_RST_CPU                BIT2
#define   B_ICH_IOPORT_RST_CNT_FULL_RST               BIT3

//
// Reset Generator I/O Port
//
#define RESET_PORT                0x0CF9
#define CLEAR_RESET_BITS          0x0F1
#define COLD_RESET                0x02  // Set bit 1 for cold reset
#define RST_CPU                   0x04  // Setting this bit triggers a reset of the CPU
#define FULL_RESET                0x08  // Set bit 4 with bit 1 for full reset

//
// PCH Softstraps 
//
#define PCHSTRAP_9   9
#define PCHSTRAP_10  10
#define PCHSTRAP_16  16
#define PCHSTRAP_17  17


#endif
