#
# This file contains an 'Intel Pre-EFI Module' and is licensed
# for Intel CPUs and Chipsets under the terms of your license
# agreement with Intel or your vendor.  This file may be
# modified by the user, subject to additional terms of the
# license agreement
#
#------------------------------------------------------------------------------
#
# Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>
# This software and associated documentation (if any) is furnished
# under a license and may only be used or copied in accordance
# with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be
# reproduced, stored in a retrieval system, or transmitted in any
# form or by any means without the express written consent of
# Intel Corporation.
#
# Module Name:
#
#  ProcessorStartupUncore.inc
#
# Abstract:
#
#
#------------------------------------------------------------------------------
#
# This is an assembly include file and is
# licensed for Intel CPUs and chipsets under the terms of your
# license agreement with Intel or your vendor.  This file may
# be modified by the user, subject to additional terms of the
# license agreement
#
#------------------------------------------------------------------------------

.equ                                  CPU_FAMILY_SKX, 0x5065          # bit[19:4] of CPUID_EAX

.equ                                  MSR_SIMICS_CSR_VERSION, 0x0deadbeef             # Simics fake MSR for CSR register header verion
.equ                                  BIOS_CSR_HEADER_VERSION, 0x01522a             # Reg Header Date Stamp: 15ww22a

#-------------------------------------------------------------------------------------------------------------
# CSR register address in legacy PCI format Bus:Dev:Func:Offset (used via IO port CF8/CFC access method)
#-------------------------------------------------------------------------------------------------------------
#Reg Header Date Stamp: 15ww22a
#----------------------------------------------------------------------------------------------------------------------------
#Bios Name                                    Address    SKX Name                                Bus    Device  Fun     Offset
#----------------------------------------------------------------------------------------------------------------------------
.equ                                      CSR_LEGACY_MMIO_TARGET_LIST_1_CBO, 0x8001e9f8  #CBO_CR_MMIO_TARGET_LIST_CFG_1           1      29      1       0x00f8
.equ                                      CSR_LEGACY_SAD_CONTROL_CBO, 0x8001e9f4  #CBO_CR_SAD_CONTROL_CFG                  1      29      1       0x00f4
.equ                                      CSR_LEGACY_SAD_TARGET_CBO, 0x8001e9f0  #CBO_CR_SAD_TARGET_CFG                   1      29      1       0x00f0
.equ                                      CSR_LEGACY_MMCFG_TARGET_LIST_CBO, 0x8001e9ec  #CBO_CR_MMCFG_TARGET_LIST_CFG            1      29      1       0x00ec
.equ                                      CSR_LEGACY_MMIO_TARGET_LIST_0_CBO, 0x8001e9e8  #CBO_CR_MMIO_TARGET_LIST_CFG_0           1      29      1       0x00e8
.equ                                      CSR_LEGACY_MMCFG_LOCAL_RULE_CBO, 0x8001e9e4  #CBO_CR_MMCFG_LOCAL_RULE_CFG             1      29      1       0x00e4
.equ                                      CSR_LEGACY_MMCFG_LOCAL_RULE_ADDRESS1_CBO, 0x8001e9cc  #CBO_CR_MMCFG_LOCAL_RULE_ADDRESS_CFG_1   1      29      1       0x00cc
.equ                                      CSR_LEGACY_MMCFG_LOCAL_RULE_ADDRESS0_CBO, 0x8001e9c8  #CBO_CR_MMCFG_LOCAL_RULE_ADDRESS_CFG_0   1      29      1       0x00c8
.equ                                      CSR_LEGACY_MMCFG_RULE_HI_CBO, 0x8001e9c4  #CBO_CR_MMCFG_RULE_CFG_N1                1      29      1       0x00c4
.equ                                      CSR_LEGACY_MMCFG_RULE_CBO, 0x8001e9c0  #CBO_CR_MMCFG_RULE_CFG_N0                1      29      1       0x00c0
.equ                                      CSR_LEGACY_MMIO_RULE11_CBO, 0x8001e998  #CBO_CR_MMIO_RULE_CFG_11_N0              1      29      1       0x0098
.equ                                      CSR_LEGACY_MMCFG_RULE_UBOX, 0x800042dc  #NCDECS_CR_MMCFG_RULE_CFG                0      8       2       0x00dc
.equ                                      CSR_LEGACY_CPUBUSNO_VALID_UBOX, 0x800042d4  #NCDECS_CR_CPUBUSNO_VALID_CFG            0      8       2       0x00d4
.equ                                      CSR_LEGACY_CPUBUSNO1_UBOX, 0x800042d0  #NCDECS_CR_CPUBUSNO1_CFG                 0      8       2       0x00d0
.equ                                      CSR_LEGACY_CPUBUSNO_UBOX, 0x800042cc  #NCDECS_CR_CPUBUSNO_CFG                  0      8       2       0x00cc
.equ                                      LEGACY_BIOSSNONSTICKYSCRATCHPAD0, 0x800042c0  #NCDECS_CR_BIOSNONSTICKYSCRATCHPAD0_CFG  0      8       2       0x00c0
.equ                                      LEGACY_BIOSSNONSTICKYSCRATCHPAD13, 0x80004294  #NCDECS_CR_BIOSNONSTICKYSCRATCHPAD13_CFG 0      8       2       0x0094
.equ                                      LEGACY_BIOSSNONSTICKYSCRATCHPAD15, 0x8000429c  #NCDECS_CR_BIOSNONSTICKYSCRATCHPAD15_CFG 0      8       2       0x009c
.equ                                      LEGACY_BIOSSCRATCHPAD0, 0x800042a0  #NCDECS_CR_BIOSSCRATCHPAD0_CFG           0      8       2       0x00a0
.equ                                      LEGACY_BIOSSCRATCHPAD2, 0x800042a8  #NCDECS_CR_BIOSSCRATCHPAD2_CFG           0      8       2       0x00a8
.equ                                      LEGACY_BIOSSCRATCHPAD3, 0x800042ac  #NCDECS_CR_BIOSSCRATCHPAD3_CFG           0      8       2       0x00ac
.equ                                      LEGACY_BIOSSCRATCHPAD7, 0x800042bc  #NCDECS_CR_BIOSSCRATCHPAD7_CFG           0      8       2       0x00bc
.equ                                      CSR_LEGACY_LOCKCONTROL, 0x800040d0  #NCEVENTS_CR_LOCKCONTROL_CFG             0      8       0       0x00d0
.equ                                      CSR_LEGACY_MMCFG_IIO, 0x80002890  #IIO_CSTACK_CR_MMCFG_BASE_0_5_0_CFG_N0   0      5       0       0x0090
.equ                                      CSR_LEGACY_EMULATION_FLAG_DMI, 0x800000fc  #Virtual CSR on DMI for SW Emulation     0      0       0       0x00fc
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------

.equ                              SSR_LEGACY_BUS_CONFIG_INFO_CSR, LEGACY_BIOSSCRATCHPAD3
.equ                              SSR_LEGACY_BOOT_CONFIG_INFO_CSR, LEGACY_BIOSSCRATCHPAD7

#-------------------------------------------------------------------------------------------------------------
# CSR register address used by ReadCpuCsr() and WriteCpuCsr()
#  Address format defined as follows
#     [31:28] = 0h
#     [27:20] = Bus # of local target (Bus0,Bus1,...Bus5)
#     [19:15] = Dev #
#     [14:12] = Func #
#     [11:0]  = Reg offset (dword aligned)
#-------------------------------------------------------------------------------------------------------------
# CSR register address in legacy PCIe format Bus:Dev:Func:Offset (used via MMCFG access method)
#----------------------------------------------------------------------------------------------------------------------------
#Bios Name                                    Address    SKX Name                                Bus    Device  Fun     Offset
#----------------------------------------------------------------------------------------------------------------------------
.equ                                      CSR_VCU_MAILBOX_DATA, 0x001f8084  #VCU_CR_CSR_MAILBOX_DATA_CFG             1      31      0       0x0084
.equ                                      CSR_VCU_MAILBOX_INTERFACE, 0x001f8080  #VCU_CR_CSR_MAILBOX_INTERFACE_CFG        1      31      0       0x0080
.equ                                      CSR_CPUBUSNO_45_PCU, 0x001f4098  #PCU_CR_CPUBUSNO1_CFG                    1      30      4       0x0098
.equ                                      CSR_CPUBUSNO_03_CPU, 0x001f4094  #PCU_CR_CPUBUSNO_CFG                     1      30      4       0x0094
.equ                                      CSR_GLOBAL_PKG_C_S_CONTROL_PCU, 0x001f20a0  #PCU_CR_GLOBAL_PKG_C_S_CONTROL_REGISTER  1      30      2       0x00a0
.equ                                      CSR_BIOS_RESET_CPL, 0x001f1094  #PCU_CR_BIOS_RESET_CPL_CFG               1      30      1       0x0094
.equ                                      CSR_MMIO_TARGET_LIST_1_CBO, 0x001e90f8  #CBO_CR_MMIO_TARGET_LIST_CFG_1           1      29      1       0x00f8
.equ                                      CSR_SAD_CONTROL_CBO, 0x001e90f4  #CBO_CR_SAD_CONTROL_CFG                  1      29      1       0x00f4
.equ                                      CSR_SAD_TARGET_CBO, 0x001e90f0  #CBO_CR_SAD_TARGET_CFG                   1      29      1       0x00f0
.equ                                      CSR_MMCFG_TARGET_LIST_CBO, 0x001e90ec  #CBO_CR_MMCFG_TARGET_LIST_CFG            1      29      1       0x00ec
.equ                                      CSR_MMIO_TARGET_LIST_0_CBO, 0x001e90e8  #CBO_CR_MMIO_TARGET_LIST_CFG_0           1      29      1       0x00e8
.equ                                      CSR_MMCFG_LOCAL_RULE_CBO, 0x001e90e4  #CBO_CR_MMCFG_LOCAL_RULE_CFG             1      29      1       0x00e4
.equ                                      CSR_IOAPIC_TARGET_LIST_3, 0x001e90e0  #CBO_CR_IOAPIC_TARGET_LIST_CFG_3         1      29      1       0x00e0
.equ                                      CSR_IOAPIC_TARGET_LIST_2, 0x001e90dc  #CBO_CR_IOAPIC_TARGET_LIST_CFG_2         1      29      1       0x00dc
.equ                                      CSR_IOAPIC_TARGET_LIST_1, 0x001e90d8  #CBO_CR_IOAPIC_TARGET_LIST_CFG_1         1      29      1       0x00d8
.equ                                      CSR_IOAPIC_TARGET_LIST_0, 0x001e90d4  #CBO_CR_IOAPIC_TARGET_LIST_CFG_0         1      29      1       0x00d4
.equ                                      CSR_MMCFG_LOCAL_RULE_ADDRESS1_CBO, 0x001e90cc  #CBO_CR_MMCFG_LOCAL_RULE_ADDRESS_CFG_1   1      29      1       0x00cc
.equ                                      CSR_MMCFG_LOCAL_RULE_ADDRESS0_CBO, 0x001e90c8  #CBO_CR_MMCFG_LOCAL_RULE_ADDRESS_CFG_0   1      29      1       0x00c8
.equ                                      CSR_MMCFG_RULE_HI_CBO, 0x001e90c4  #CBO_CR_MMCFG_RULE_CFG_N1                1      29      1       0x00c4
.equ                                      CSR_MMCFG_RULE_CBO, 0x001e90c0  #CBO_CR_MMCFG_RULE_CFG_N0                1      29      1       0x00c0
.equ                                      CSR_MMIO_RULE11_CBO, 0x001e9098  #CBO_CR_MMIO_RULE_CFG_11_N0              1      29      1       0x0098
.equ                                      CSR_IOPORT_TARGET_LIST_0_CBO, 0x001e82b0  #CBO_CR_IOPORT_TARGET_LIST_CFG_0         1      29      0       0x02b0
.equ                                      CSR_MMCFG_RULE_UBOX, 0x000420dc  #NCDECS_CR_MMCFG_RULE_CFG                0      8       2       0x00dc
.equ                                      CSR_CPUBUSNO_VALID_UBOX, 0x000420d4  #NCDECS_CR_CPUBUSNO_VALID_CFG            0      8       2       0x00d4
.equ                                      CSR_CPUBUSNO1_UBOX, 0x000420d0  #NCDECS_CR_CPUBUSNO1_CFG                 0      8       2       0x00d0
.equ                                      CSR_CPUBUSNO_UBOX, 0x000420cc  #NCDECS_CR_CPUBUSNO_CFG                  0      8       2       0x00cc
.equ                                      CSR_SYSTEMSEMAPHORE0, 0x00042210  #NCDECS_CR_SYSTEMSEMAPHORE0_CFG          0      8       2       0x0210
.equ                                      SR_PBSP_CHECKIN_CSR, 0x000420c8  #NCDECS_CR_BIOSNONSTICKYSCRATCHPAD2_CFG  0      8       2       0x00c8
.equ                                      CSR_BIOSNONSTICKYSCRATCHPAD0, 0x000420c0  #NCDECS_CR_BIOSNONSTICKYSCRATCHPAD0_CFG  0      8       2       0x00c0
.equ                                      CSR_BIOSNONSTICKYSCRATCHPAD11, 0x0004208c  #NCDECS_CR_BIOSNONSTICKYSCRATCHPAD11_CFG 0      8       2       0x008c
.equ                                      CSR_BIOSNONSTICKYSCRATCHPAD13, 0x00042094  #NCDECS_CR_BIOSNONSTICKYSCRATCHPAD13_CFG 0      8       2       0x0094
.equ                                      CSR_BIOSNONSTICKYSCRATCHPAD14, 0x00042098  #NCDECS_CR_BIOSNONSTICKYSCRATCHPAD14_CFG 0      8       2       0x0098
.equ                                      CSR_BIOSNONSTICKYSCRATCHPAD15, 0x0004209c  #NCDECS_CR_BIOSNONSTICKYSCRATCHPAD15_CFG 0      8       2       0x009c
.equ                                      CSR_BIOSSCRATCHPAD7, 0x000420bc  #NCDECS_CR_BIOSSCRATCHPAD7_CFG           0      8       2       0x00bc
.equ                                      CSR_BIOSSCRATCHPAD0, 0x000420a0  #NCDECS_CR_BIOSSCRATCHPAD0_CFG           0      8       2       0x00a0
.equ                                      CSR_LOCKCONTROL, 0x000400d0  #NCEVENTS_CR_LOCKCONTROL_CFG             0      8       0       0x00d0
.equ                                      CSR_CPUNODEID_UBOX, 0x000400c0  #NCEVENTS_CR_CPUNODEID_CFG               0      8       0       0x00c0
.equ                                      CSR_CPUBUSNO_IIO, 0x00028108  #IIO_CSTACK_CR_CPUBUSNO_0_5_0_CFG        0      5       0       0x0108
.equ                                      CSR_MMCFG_IIO, 0x00028090  #IIO_CSTACK_CR_MMCFG_BASE_0_5_0_CFG_N0   0      5       0       0x0090
.equ                                      CSR_LT_CONTROL_CHABC, 0x001E90D0  #LT_CONTROL_CHABC_SAD1_REG               1     29       1       0x00d0
.equ                                      CSR_EMULATION_FLAG_DMI, 0x000000fc  #Virtual CSR on DMI for SW Emulation     0      0       0       0x00fc
#----------------------------------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------------------------------
.equ                                      CSR_MMCFG_IIO_BASE_LIMIT_GAP, 8  #MMCFG_IIO register is QWORD size


.equ                                        SR_LEGACY_BUSCFG_SYNC_CSR, LEGACY_BIOSSNONSTICKYSCRATCHPAD13
.equ                                        SR_LEGACY_BUSCFG_DATA_CSR, LEGACY_BIOSSNONSTICKYSCRATCHPAD15
.equ                                        SR_BUSCFG_SYNC_CSR, CSR_BIOSNONSTICKYSCRATCHPAD13
.equ                                        SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, CSR_BIOSNONSTICKYSCRATCHPAD13
.equ                                        SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR, CSR_BIOSNONSTICKYSCRATCHPAD14
.equ                                        SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, CSR_BIOSNONSTICKYSCRATCHPAD15
#ifdef SV_HOOKS
.equ                                        SR_PATCH_MICROCODE_ADDRESS, CSR_BIOSNONSTICKYSCRATCHPAD11
#endif
#----------------------------------------------------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Pre-defined constants
#------------------------------------------------------------------------------

.equ                                  DEFAULT_MMCFG_RULE, (PCIEX_BASE_ADDRESS + 8)   # mmcfg.base, with max 256 buses
.equ                                  DEFAULT_MMCFG_LOCAL_RULE, 0x3f543210                  # Local Targets = [5,4,3,2,1,0], Mode[5:0] = 111111b
.equ                                  DEFAULT_MMCFG_TARGET_LIST_LOCAL, 0x88888888                  # all cfg requests are confined to the local socket
.equ                                  S0_MMCFG_TARGET_LIST_GLOBAL, 0x76543210                  # when legacy socketId = 0    [7,6,5,4,3,2,1,0], 8-socket config
.equ                                  S1_MMCFG_TARGET_LIST_GLOBAL, 0x76543201                  # when legacy socketId = 1
.equ                                  S2_MMCFG_TARGET_LIST_GLOBAL, 0x76543102                  # when legacy socketId = 2 
.equ                                  S3_MMCFG_TARGET_LIST_GLOBAL, 0x76542103                # ......
.equ                                  S4_MMCFG_TARGET_LIST_GLOBAL, 0x76532104
.equ                                  S5_MMCFG_TARGET_LIST_GLOBAL, 0x76432105
.equ                                  S6_MMCFG_TARGET_LIST_GLOBAL, 0x75432106
.equ                                  S7_MMCFG_TARGET_LIST_GLOBAL, 0x65432107

.equ                                  DEFAULT_IOAPIC_TARGET_LIST, 0x88888888                  # all cfg requests are confined to the local socket

.equ                                  MMCFG_RULE_ADDR_MASK, 0x0ffffc000                 # mask off bit[13:0]

.equ                                  MMCFG_RULE_CLR_ENABLE, 0x0fffffffe                 # mask bit[0]

.equ                                  LEGACY_BUS_NO0, 0x00
.equ                                  LEGACY_BUS_NO1, 0x11
.equ                                  LEGACY_BUS_NO2, 0x12
.equ                                  LEGACY_BUS_NO3, 0x13
.equ                                  LEGACY_BUS_NO4, 0x14
.equ                                  LEGACY_BUS_NO5, 0x15

.equ                                  DEFAULT_BUS_NO0, 0x00
.equ                                  DEFAULT_BUS_NO1, 0x01
.equ                                  DEFAULT_BUS_NO2, 0x02
.equ                                  DEFAULT_BUS_NO3, 0x03
.equ                                  DEFAULT_BUS_NO4, 0x04
.equ                                  DEFAULT_BUS_NO5, 0x05

.equ                                  DEFAULT_COLDBOOT_CPUBUSNO, (DEFAULT_BUS_NO3 << 24) + (DEFAULT_BUS_NO2 << 16) + (DEFAULT_BUS_NO1 << 8) + DEFAULT_BUS_NO0
.equ                                  DEFAULT_COLDBOOT_CPUBUSNO1, (DEFAULT_BUS_NO5 << 8) + DEFAULT_BUS_NO4
.equ                                  DEFAULT_MMCFG_LOCAL_RULE_ADDRESS0, (DEFAULT_BUS_NO2 << 24) + (DEFAULT_BUS_NO1 << 16) + ((DEFAULT_BUS_NO1 - 1) << 8) + DEFAULT_BUS_NO0
.equ                                  DEFAULT_MMCFG_LOCAL_RULE_ADDRESS1, (DEFAULT_BUS_NO5 << 16) + (DEFAULT_BUS_NO4 << 8) + DEFAULT_BUS_NO3

.equ                                  LEGACY_COLDBOOT_CPUBUSNO, (LEGACY_BUS_NO3 << 24) + (LEGACY_BUS_NO2 << 16) + (LEGACY_BUS_NO1 << 8) + LEGACY_BUS_NO0
.equ                                  LEGACY_COLDBOOT_CPUBUSNO1, (LEGACY_BUS_NO5 << 8) + LEGACY_BUS_NO4
.equ                                  LEGACY_MMCFG_LOCAL_RULE_ADDR0, (LEGACY_BUS_NO2 << 24) + (LEGACY_BUS_NO1 << 16) + ((LEGACY_BUS_NO1 - 1) << 8) + LEGACY_BUS_NO0
.equ                                  LEGACY_MMCFG_LOCAL_RULE_ADDR1, (LEGACY_BUS_NO5 << 16) + (LEGACY_BUS_NO4 << 8) + LEGACY_BUS_NO3


.equ                                  BusCfgSync_PBSP_Ready, 0x0ABCDEF01
.equ                                  BusCfgSync_PBSP_Go, 0x0ABCDEF02
.equ                                  BusCfgSync_PBSP_Done, 0x0ABCDEF03

.equ                                  CSR_BIOS_MAILBOX_DATA_PCU_FUN1_REG, ((1 << 20) + (30 << 15) + (1 << 12) + 0x8C) # 1:30:1:0x8C  BIOS_MAILBOX_DATA_PCU_FUN1_REG
.equ                                  CSR_BIOS_MAILBOX_INTERFACE_PCU_FUN1_REG, ((1 << 20) + (30 << 15) + (1 << 12) + 0x90) # 1:30:1:0x90  BIOS_MAILBOX_INTERFACE_PCU_FUN1_REG
.equ                                  BIOS_MAILBOX_COMMAND_SWITCHTO_P1, 0x0800000af
.equ                                  BIOS_MAILBOX_DATA_SWITCHTO_P1, 0x000003f3f

.equ                                  PIPE_DISPATCH_SYNCH_PSYSHOST, 4
.equ                                  CPU_OPTIONS_BIT_DCU_MODE_SEL, BIT0
.equ                                  CPU_OPTIONS_BIT_DEBUG_INTERFACE_EN, BIT1

# VCU Mailbox API related equates
.equ                                  VCODE_API_OPCODE_SET_PARAMETER, 0x01
.equ                                  VCODE_API_SEQ_ID_IOT_LLC_SETUP, 0x1000E
.equ                                  VCODE_API_OPCODE_OPEN_SEQUENCE, 0x03
.equ                                  VCODE_API_OPCODE_CLOSE_SEQUENCE, 0x04
.equ                                  VCODE_API_OPCODE_IOT_LLC_SETUP, 0x1007
.equ                                  VCODE_API_DELAY_COUNT, 0x1000



#------------------------------------------------------------------------------
#        BIOS Usage of Scratchpad LEGACY_BIOSSCRATCHPAD2
#------------------------------------------------------------------------------
#LEGACY_BIOSSCRATCHPAD2             EQU  LEGACY_BIOSSCRATCHPAD0 + 4*2    ; SSR2,  0:8:2:0xA8
    # [7:0]       - CPU package present bitmap (KTIRC updates, in all sockets)
    # Other bits  - UPI Link Valid flags (Used by KTIRC)


#------------------------------------------------------------------------------
#        BIOS Usage of Scratchpad LEGACY_BIOSSCRATCHPAD3 (SSR_LEGACY_BUS_CONFIG_INFO_CSR)
#------------------------------------------------------------------------------
#SSR_LEGACY_BUS_CONFIG_INFO_CSR     EQU  LEGACY_BIOSSCRATCHPAD0 + 4*3    ;SSR03,  0:8:2:0xAC

#------------------------------------------------------------------------------0.

#        BIOS Usage of Scratchpad LEGACY_BIOSSCRATCHPAD7 (SSR_LEGACY_BOOT_CONFIG_INFO_CSR)
#------------------------------------------------------------------------------
#SSR_LEGACY_BOOT_CONFIG_INFO_CSR    EQU  LEGACY_BIOSSCRATCHPAD0 + 4*7    ;SSR07, 0:8:2:0xBC
#         Scratch Pad Register 7 Format
#         [31] - AepDimmPresent
#         [30] - Snc recovery
#         [29] - Remote socket released in LT enabled system
#         [28] - FRB error
#         [27] - Fail mem check in fast cold boot
#         [26] - In DWR
#         [25] - Bios stall if enter dwr
#         [24] - DCU mode select
#         [23:6] - unused
#         [5] - Completed Cold Reset Flow
#         [4] - Entered Cold Reset Flow
#         [3:0]  - Unused


#------------------------------------------------------------------------------
#        BIOS Usage of Scratchpad SR_PBSP_CHECKIN_CSR
#------------------------------------------------------------------------------

#SR_PBSP_CHECKIN_CSR  BIOS usage definition
            # [30:27] = Stepping of Package BSP
            # [23:8] = APIC ID of Package BSP
            # [0]    = Socket BSP check-in Flag
            # All other bits reserved



.macro BUSNO1_EBX
  andl  $0xf00fffff, %ebx
  orl   LEGACY_BUS_NO1 << 20, %ebx
.endm


