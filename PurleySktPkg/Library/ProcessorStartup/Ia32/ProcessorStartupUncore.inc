;
; This file contains an 'Intel Pre-EFI Module' and is licensed
; for Intel CPUs and Chipsets under the terms of your license
; agreement with Intel or your vendor.  This file may be
; modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
; Module Name:
;
;  ProcessorStartupUncore.inc
;
; Abstract:
;
;
;------------------------------------------------------------------------------
;
; This is an assembly include file and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------

CPU_FAMILY_SKX  EQU     5065h         ; bit[19:4] of CPUID_EAX

MSR_SIMICS_CSR_VERSION                EQU   0deadbeefh            ; Simics fake MSR for CSR register header verion
BIOS_CSR_HEADER_VERSION               EQU      01522ah            ; Reg Header Date Stamp: 15ww22a

;-------------------------------------------------------------------------------------------------------------
; CSR register address in legacy PCI format Bus:Dev:Func:Offset (used via IO port CF8/CFC access method)
;-------------------------------------------------------------------------------------------------------------
;Reg Header Date Stamp: 15ww22a
;----------------------------------------------------------------------------------------------------------------------------
;Bios Name                                    Address    SKX Name                                Bus    Device  Fun     Offset
;----------------------------------------------------------------------------------------------------------------------------
CSR_LEGACY_MMIO_TARGET_LIST_1_CBO         EQU 8001e9f8h ;CBO_CR_MMIO_TARGET_LIST_CFG_1           1      29      1       0x00f8
CSR_LEGACY_SAD_CONTROL_CBO                EQU 8001e9f4h ;CBO_CR_SAD_CONTROL_CFG                  1      29      1       0x00f4
CSR_LEGACY_SAD_TARGET_CBO                 EQU 8001e9f0h ;CBO_CR_SAD_TARGET_CFG                   1      29      1       0x00f0
CSR_LEGACY_MMCFG_TARGET_LIST_CBO          EQU 8001e9ech ;CBO_CR_MMCFG_TARGET_LIST_CFG            1      29      1       0x00ec
CSR_LEGACY_MMIO_TARGET_LIST_0_CBO         EQU 8001e9e8h ;CBO_CR_MMIO_TARGET_LIST_CFG_0           1      29      1       0x00e8
CSR_LEGACY_MMCFG_LOCAL_RULE_CBO           EQU 8001e9e4h ;CBO_CR_MMCFG_LOCAL_RULE_CFG             1      29      1       0x00e4
CSR_LEGACY_MMCFG_LOCAL_RULE_ADDRESS1_CBO  EQU 8001e9cch ;CBO_CR_MMCFG_LOCAL_RULE_ADDRESS_CFG_1   1      29      1       0x00cc
CSR_LEGACY_MMCFG_LOCAL_RULE_ADDRESS0_CBO  EQU 8001e9c8h ;CBO_CR_MMCFG_LOCAL_RULE_ADDRESS_CFG_0   1      29      1       0x00c8
CSR_LEGACY_MMCFG_RULE_HI_CBO              EQU 8001e9c4h ;CBO_CR_MMCFG_RULE_CFG_N1                1      29      1       0x00c4
CSR_LEGACY_MMCFG_RULE_CBO                 EQU 8001e9c0h ;CBO_CR_MMCFG_RULE_CFG_N0                1      29      1       0x00c0
CSR_LEGACY_MMIO_RULE11_CBO                EQU 8001e998h ;CBO_CR_MMIO_RULE_CFG_11_N0              1      29      1       0x0098
CSR_LEGACY_MMCFG_RULE_UBOX                EQU 800042dch ;NCDECS_CR_MMCFG_RULE_CFG                0      8       2       0x00dc
CSR_LEGACY_CPUBUSNO_VALID_UBOX            EQU 800042d4h ;NCDECS_CR_CPUBUSNO_VALID_CFG            0      8       2       0x00d4
CSR_LEGACY_CPUBUSNO1_UBOX                 EQU 800042d0h ;NCDECS_CR_CPUBUSNO1_CFG                 0      8       2       0x00d0
CSR_LEGACY_CPUBUSNO_UBOX                  EQU 800042cch ;NCDECS_CR_CPUBUSNO_CFG                  0      8       2       0x00cc
LEGACY_BIOSSNONSTICKYSCRATCHPAD0          EQU 800042c0h ;NCDECS_CR_BIOSNONSTICKYSCRATCHPAD0_CFG  0      8       2       0x00c0
LEGACY_BIOSSNONSTICKYSCRATCHPAD13         EQU 80004294h ;NCDECS_CR_BIOSNONSTICKYSCRATCHPAD13_CFG 0      8       2       0x0094
LEGACY_BIOSSNONSTICKYSCRATCHPAD15         EQU 8000429ch ;NCDECS_CR_BIOSNONSTICKYSCRATCHPAD15_CFG 0      8       2       0x009c
LEGACY_BIOSSCRATCHPAD0                    EQU 800042a0h ;NCDECS_CR_BIOSSCRATCHPAD0_CFG           0      8       2       0x00a0
LEGACY_BIOSSCRATCHPAD2                    EQU 800042a8h ;NCDECS_CR_BIOSSCRATCHPAD2_CFG           0      8       2       0x00a8
LEGACY_BIOSSCRATCHPAD3                    EQU 800042ach ;NCDECS_CR_BIOSSCRATCHPAD3_CFG           0      8       2       0x00ac
LEGACY_BIOSSCRATCHPAD7                    EQU 800042bch ;NCDECS_CR_BIOSSCRATCHPAD7_CFG           0      8       2       0x00bc
CSR_LEGACY_LOCKCONTROL                    EQU 800040d0h ;NCEVENTS_CR_LOCKCONTROL_CFG             0      8       0       0x00d0
CSR_LEGACY_MMCFG_IIO                      EQU 80002890h ;IIO_CSTACK_CR_MMCFG_BASE_0_5_0_CFG_N0   0      5       0       0x0090
CSR_LEGACY_EMULATION_FLAG_DMI             EQU 800000fch ;Virtual CSR on DMI for SW Emulation     0      0       0       0x00fc
;----------------------------------------------------------------------------------------------------------------------------
;----------------------------------------------------------------------------------------------------------------------------

SSR_LEGACY_BUS_CONFIG_INFO_CSR    EQU  LEGACY_BIOSSCRATCHPAD3
SSR_LEGACY_BOOT_CONFIG_INFO_CSR   EQU  LEGACY_BIOSSCRATCHPAD7

;-------------------------------------------------------------------------------------------------------------
; CSR register address used by ReadCpuCsr() and WriteCpuCsr()
;  Address format defined as follows
;     [31:28] = 0h
;     [27:20] = Bus # of local target (Bus0,Bus1,...Bus5)
;     [19:15] = Dev #
;     [14:12] = Func #
;     [11:0]  = Reg offset (dword aligned)
;-------------------------------------------------------------------------------------------------------------
; CSR register address in legacy PCIe format Bus:Dev:Func:Offset (used via MMCFG access method)
;----------------------------------------------------------------------------------------------------------------------------
;Bios Name                                    Address    SKX Name                                Bus    Device  Fun     Offset
;----------------------------------------------------------------------------------------------------------------------------
CSR_VCU_MAILBOX_DATA                      EQU 001f8084h ;VCU_CR_CSR_MAILBOX_DATA_CFG             1      31      0       0x0084
CSR_VCU_MAILBOX_INTERFACE                 EQU 001f8080h ;VCU_CR_CSR_MAILBOX_INTERFACE_CFG        1      31      0       0x0080
CSR_CPUBUSNO_45_PCU                       EQU 001f4098h ;PCU_CR_CPUBUSNO1_CFG                    1      30      4       0x0098
CSR_CPUBUSNO_03_CPU                       EQU 001f4094h ;PCU_CR_CPUBUSNO_CFG                     1      30      4       0x0094
CSR_GLOBAL_PKG_C_S_CONTROL_PCU            EQU 001f20a0h ;PCU_CR_GLOBAL_PKG_C_S_CONTROL_REGISTER  1      30      2       0x00a0
CSR_BIOS_RESET_CPL                        EQU 001f1094h ;PCU_CR_BIOS_RESET_CPL_CFG               1      30      1       0x0094
CSR_MMIO_TARGET_LIST_1_CBO                EQU 001e90f8h ;CBO_CR_MMIO_TARGET_LIST_CFG_1           1      29      1       0x00f8
CSR_SAD_CONTROL_CBO                       EQU 001e90f4h ;CBO_CR_SAD_CONTROL_CFG                  1      29      1       0x00f4
CSR_SAD_TARGET_CBO                        EQU 001e90f0h ;CBO_CR_SAD_TARGET_CFG                   1      29      1       0x00f0
CSR_MMCFG_TARGET_LIST_CBO                 EQU 001e90ech ;CBO_CR_MMCFG_TARGET_LIST_CFG            1      29      1       0x00ec
CSR_MMIO_TARGET_LIST_0_CBO                EQU 001e90e8h ;CBO_CR_MMIO_TARGET_LIST_CFG_0           1      29      1       0x00e8
CSR_MMCFG_LOCAL_RULE_CBO                  EQU 001e90e4h ;CBO_CR_MMCFG_LOCAL_RULE_CFG             1      29      1       0x00e4
CSR_IOAPIC_TARGET_LIST_3                  EQU 001e90e0h ;CBO_CR_IOAPIC_TARGET_LIST_CFG_3         1      29      1       0x00e0
CSR_IOAPIC_TARGET_LIST_2                  EQU 001e90dch ;CBO_CR_IOAPIC_TARGET_LIST_CFG_2         1      29      1       0x00dc
CSR_IOAPIC_TARGET_LIST_1                  EQU 001e90d8h ;CBO_CR_IOAPIC_TARGET_LIST_CFG_1         1      29      1       0x00d8
CSR_IOAPIC_TARGET_LIST_0                  EQU 001e90d4h ;CBO_CR_IOAPIC_TARGET_LIST_CFG_0         1      29      1       0x00d4
CSR_MMCFG_LOCAL_RULE_ADDRESS1_CBO         EQU 001e90cch ;CBO_CR_MMCFG_LOCAL_RULE_ADDRESS_CFG_1   1      29      1       0x00cc
CSR_MMCFG_LOCAL_RULE_ADDRESS0_CBO         EQU 001e90c8h ;CBO_CR_MMCFG_LOCAL_RULE_ADDRESS_CFG_0   1      29      1       0x00c8
CSR_MMCFG_RULE_HI_CBO                     EQU 001e90c4h ;CBO_CR_MMCFG_RULE_CFG_N1                1      29      1       0x00c4
CSR_MMCFG_RULE_CBO                        EQU 001e90c0h ;CBO_CR_MMCFG_RULE_CFG_N0                1      29      1       0x00c0
CSR_MMIO_RULE11_CBO                       EQU 001e9098h ;CBO_CR_MMIO_RULE_CFG_11_N0              1      29      1       0x0098
CSR_IOPORT_TARGET_LIST_0_CBO              EQU 001e82b0h ;CBO_CR_IOPORT_TARGET_LIST_CFG_0         1      29      0       0x02b0
CSR_MMCFG_RULE_UBOX                       EQU 000420dch ;NCDECS_CR_MMCFG_RULE_CFG                0      8       2       0x00dc
CSR_CPUBUSNO_VALID_UBOX                   EQU 000420d4h ;NCDECS_CR_CPUBUSNO_VALID_CFG            0      8       2       0x00d4
CSR_CPUBUSNO1_UBOX                        EQU 000420d0h ;NCDECS_CR_CPUBUSNO1_CFG                 0      8       2       0x00d0
CSR_CPUBUSNO_UBOX                         EQU 000420cch ;NCDECS_CR_CPUBUSNO_CFG                  0      8       2       0x00cc
CSR_SYSTEMSEMAPHORE0                      EQU 00042210h ;NCDECS_CR_SYSTEMSEMAPHORE0_CFG          0      8       2       0x0210
SR_PBSP_CHECKIN_CSR                       EQU 000420c8h ;NCDECS_CR_BIOSNONSTICKYSCRATCHPAD2_CFG  0      8       2       0x00c8
CSR_BIOSNONSTICKYSCRATCHPAD0              EQU 000420c0h ;NCDECS_CR_BIOSNONSTICKYSCRATCHPAD0_CFG  0      8       2       0x00c0
CSR_BIOSNONSTICKYSCRATCHPAD11             EQU 0004208ch ;NCDECS_CR_BIOSNONSTICKYSCRATCHPAD11_CFG 0      8       2       0x008c
CSR_BIOSNONSTICKYSCRATCHPAD13             EQU 00042094h ;NCDECS_CR_BIOSNONSTICKYSCRATCHPAD13_CFG 0      8       2       0x0094
CSR_BIOSNONSTICKYSCRATCHPAD14             EQU 00042098h ;NCDECS_CR_BIOSNONSTICKYSCRATCHPAD14_CFG 0      8       2       0x0098
CSR_BIOSNONSTICKYSCRATCHPAD15             EQU 0004209ch ;NCDECS_CR_BIOSNONSTICKYSCRATCHPAD15_CFG 0      8       2       0x009c
CSR_BIOSSCRATCHPAD7                       EQU 000420bch ;NCDECS_CR_BIOSSCRATCHPAD7_CFG           0      8       2       0x00bc
CSR_BIOSSCRATCHPAD0                       EQU 000420a0h ;NCDECS_CR_BIOSSCRATCHPAD0_CFG           0      8       2       0x00a0
CSR_LOCKCONTROL                           EQU 000400d0h ;NCEVENTS_CR_LOCKCONTROL_CFG             0      8       0       0x00d0
CSR_CPUNODEID_UBOX                        EQU 000400c0h ;NCEVENTS_CR_CPUNODEID_CFG               0      8       0       0x00c0
CSR_CPUBUSNO_IIO                          EQU 00028108h ;IIO_CSTACK_CR_CPUBUSNO_0_5_0_CFG        0      5       0       0x0108
CSR_MMCFG_IIO                             EQU 00028090h ;IIO_CSTACK_CR_MMCFG_BASE_0_5_0_CFG_N0   0      5       0       0x0090
CSR_LT_CONTROL_CHABC                      EQU 001E90D0h ;LT_CONTROL_CHABC_SAD1_REG               1     29       1       0x00d0
CSR_EMULATION_FLAG_DMI                    EQU 000000fch ;Virtual CSR on DMI for SW Emulation     0      0       0       0x00fc
;----------------------------------------------------------------------------------------------------------------------------
;----------------------------------------------------------------------------------------------------------------------------
CSR_MMCFG_IIO_BASE_LIMIT_GAP      EQU         8 ;MMCFG_IIO register is QWORD size


SR_LEGACY_BUSCFG_SYNC_CSR                   EQU LEGACY_BIOSSNONSTICKYSCRATCHPAD13
SR_LEGACY_BUSCFG_DATA_CSR                   EQU LEGACY_BIOSSNONSTICKYSCRATCHPAD15
SR_BUSCFG_SYNC_CSR                          EQU CSR_BIOSNONSTICKYSCRATCHPAD13
SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR    EQU CSR_BIOSNONSTICKYSCRATCHPAD13
SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR     EQU CSR_BIOSNONSTICKYSCRATCHPAD14
SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR        EQU CSR_BIOSNONSTICKYSCRATCHPAD15
;----------------------------------------------------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Pre-defined constants
;------------------------------------------------------------------------------

DEFAULT_MMCFG_RULE                    EQU  (PCIEX_BASE_ADDRESS + 8)  ; mmcfg.base, with max 256 buses
DEFAULT_MMCFG_LOCAL_RULE              EQU  3f543210h                 ; Local Targets = [5,4,3,2,1,0], Mode[5:0] = 111111b
DEFAULT_MMCFG_TARGET_LIST_LOCAL       EQU  88888888h                 ; all cfg requests are confined to the local socket
S0_MMCFG_TARGET_LIST_GLOBAL           EQU  76543210h                 ; when legacy socketId = 0    [7,6,5,4,3,2,1,0], 8-socket config
S1_MMCFG_TARGET_LIST_GLOBAL           EQU  76543201h                 ; when legacy socketId = 1
S2_MMCFG_TARGET_LIST_GLOBAL           EQU  76543102h                 ; when legacy socketId = 2 
S3_MMCFG_TARGET_LIST_GLOBAL           EQU  76542103h               ; ......
S4_MMCFG_TARGET_LIST_GLOBAL           EQU  76532104h  
S5_MMCFG_TARGET_LIST_GLOBAL           EQU  76432105h  
S6_MMCFG_TARGET_LIST_GLOBAL           EQU  75432106h  
S7_MMCFG_TARGET_LIST_GLOBAL           EQU  65432107h  

DEFAULT_IOAPIC_TARGET_LIST            EQU  88888888h                 ; all cfg requests are confined to the local socket

MMCFG_RULE_ADDR_MASK                  EQU  0ffffc000h                ; mask off bit[13:0]

MMCFG_RULE_CLR_ENABLE                 EQU  0fffffffeh                ; mask bit[0]

LEGACY_BUS_NO0                        EQU  00h
LEGACY_BUS_NO1                        EQU  11h
LEGACY_BUS_NO2                        EQU  12h
LEGACY_BUS_NO3                        EQU  13h
LEGACY_BUS_NO4                        EQU  14h
LEGACY_BUS_NO5                        EQU  15h

DEFAULT_BUS_NO0                       EQU  00h
DEFAULT_BUS_NO1                       EQU  01h
DEFAULT_BUS_NO2                       EQU  02h
DEFAULT_BUS_NO3                       EQU  03h
DEFAULT_BUS_NO4                       EQU  04h
DEFAULT_BUS_NO5                       EQU  05h

DEFAULT_COLDBOOT_CPUBUSNO             EQU  (DEFAULT_BUS_NO3 SHL 24) + (DEFAULT_BUS_NO2 SHL 16) + (DEFAULT_BUS_NO1 SHL 8) + DEFAULT_BUS_NO0
DEFAULT_COLDBOOT_CPUBUSNO1            EQU  (DEFAULT_BUS_NO5 SHL 8) + DEFAULT_BUS_NO4
DEFAULT_MMCFG_LOCAL_RULE_ADDRESS0     EQU  (DEFAULT_BUS_NO2 SHL 24) + (DEFAULT_BUS_NO1 SHL 16) + ((DEFAULT_BUS_NO1-1) SHL 8)+ DEFAULT_BUS_NO0
DEFAULT_MMCFG_LOCAL_RULE_ADDRESS1     EQU  (DEFAULT_BUS_NO5 SHL 16) + (DEFAULT_BUS_NO4 SHL 8) + DEFAULT_BUS_NO3

LEGACY_COLDBOOT_CPUBUSNO              EQU  (LEGACY_BUS_NO3 SHL 24) + (LEGACY_BUS_NO2 SHL 16) + (LEGACY_BUS_NO1 SHL 8) + LEGACY_BUS_NO0
LEGACY_COLDBOOT_CPUBUSNO1             EQU  (LEGACY_BUS_NO5 SHL 8) + LEGACY_BUS_NO4
LEGACY_MMCFG_LOCAL_RULE_ADDR0         EQU  (LEGACY_BUS_NO2 SHL 24) + (LEGACY_BUS_NO1 SHL 16) + ((LEGACY_BUS_NO1-1) SHL 8)+ LEGACY_BUS_NO0
LEGACY_MMCFG_LOCAL_RULE_ADDR1         EQU  (LEGACY_BUS_NO5 SHL 16) + (LEGACY_BUS_NO4 SHL 8) + LEGACY_BUS_NO3


BusCfgSync_PBSP_Ready                 EQU  0ABCDEF01h
BusCfgSync_PBSP_Go                    EQU  0ABCDEF02h
BusCfgSync_PBSP_Done                  EQU  0ABCDEF03h

CSR_BIOS_MAILBOX_DATA_PCU_FUN1_REG      EQU  (1 SHL 20) OR (30 SHL 15) OR (1 SHL 12) OR 8Ch ; 1:30:1:0x8C  BIOS_MAILBOX_DATA_PCU_FUN1_REG
CSR_BIOS_MAILBOX_INTERFACE_PCU_FUN1_REG EQU  (1 SHL 20) OR (30 SHL 15) OR (1 SHL 12) OR 90h ; 1:30:1:0x90  BIOS_MAILBOX_INTERFACE_PCU_FUN1_REG
BIOS_MAILBOX_COMMAND_SWITCHTO_P1        EQU 0800000afh
BIOS_MAILBOX_DATA_SWITCHTO_P1           EQU 000003f3fh

PIPE_DISPATCH_SYNCH_PSYSHOST          EQU     4
CPU_OPTIONS_BIT_DCU_MODE_SEL          EQU  BIT0
CPU_OPTIONS_BIT_DEBUG_INTERFACE_EN    EQU  BIT1

; VCU Mailbox API related equates
VCODE_API_OPCODE_SET_PARAMETER        EQU 01h
VCODE_API_SEQ_ID_IOT_LLC_SETUP        EQU 1000Eh
VCODE_API_OPCODE_OPEN_SEQUENCE        EQU 03h
VCODE_API_OPCODE_CLOSE_SEQUENCE       EQU 04h
VCODE_API_OPCODE_IOT_LLC_SETUP        EQU 1007h
VCODE_API_DELAY_COUNT                 EQU 1000h



;------------------------------------------------------------------------------
;        BIOS Usage of Scratchpad LEGACY_BIOSSCRATCHPAD2
;------------------------------------------------------------------------------
;LEGACY_BIOSSCRATCHPAD2             EQU  LEGACY_BIOSSCRATCHPAD0 + 4*2    ; SSR2,  0:8:2:0xA8
    ; [7:0]       - CPU package present bitmap (KTIRC updates, in all sockets)
    ; Other bits  - UPI Link Valid flags (Used by KTIRC)


;------------------------------------------------------------------------------
;        BIOS Usage of Scratchpad LEGACY_BIOSSCRATCHPAD3 (SSR_LEGACY_BUS_CONFIG_INFO_CSR)
;------------------------------------------------------------------------------
;SSR_LEGACY_BUS_CONFIG_INFO_CSR     EQU  LEGACY_BIOSSCRATCHPAD0 + 4*3    ;SSR03,  0:8:2:0xAC

;------------------------------------------------------------------------------0.

;        BIOS Usage of Scratchpad LEGACY_BIOSSCRATCHPAD7 (SSR_LEGACY_BOOT_CONFIG_INFO_CSR)
;------------------------------------------------------------------------------
;SSR_LEGACY_BOOT_CONFIG_INFO_CSR    EQU  LEGACY_BIOSSCRATCHPAD0 + 4*7    ;SSR07, 0:8:2:0xBC
;         Scratch Pad Register 7 Format
;         [31] - AepDimmPresent
;         [30] - Snc recovery
;         [29] - Remote socket released in LT enabled system
;         [28] - FRB error
;         [27] - Fail mem check in fast cold boot
;         [26] - In DWR
;         [25] - Bios stall if enter dwr
;         [24] - DCU mode select
;         [23:6] - unused
;         [5] - Completed Cold Reset Flow
;         [4] - Entered Cold Reset Flow
;         [3:0]  - Unused


;------------------------------------------------------------------------------
;        BIOS Usage of Scratchpad SR_PBSP_CHECKIN_CSR
;------------------------------------------------------------------------------

;SR_PBSP_CHECKIN_CSR  BIOS usage definition
            ; [30:27] = Stepping of Package BSP
            ; [23:8] = APIC ID of Package BSP
            ; [0]    = Socket BSP check-in Flag
            ; All other bits reserved



BUSNO1_EBX   macro
  and   ebx, 0f00fffffh
  or    ebx, LEGACY_BUS_NO1 SHL 20
endm

