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
#  ProcessorStartupPlatform.inc
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


.equ                            PCIEX_BASE_ADDRESS, 0x080000000           # MMCFG / PCIe Config Base Address, used as cold boot default only.
                                                            # Use MMCFG_BASE_ADDRESS in PlatformHost.h file to
                                                            # define platform choice of PCIe config base address

.equ                            SB_RCBA, 0x0fed1c000           # South Bridge PCH RCBA Address

# IO Port 80h  POST CODE used in SEC phase
.equ                            POWER_ON_POST_CODE, 0x01
.equ                            MICROCODE_POST_CODE, 0x02
.equ                            NEM_CACHE_INIT_POST_CODE, 0x03
.equ                            NEM_CACHE_ENABLED_POST_CODE, 0x04
.equ                            SBSP_PBSP_BRANCH_POST_CODE, 0x05
.equ                            SEC_CPU_INIT_END_POST_CODE, 0x06

# IO Port 80h  Fatal Error CODE used in SEC phase
.equ                               NON_INTEL_CPU_ERROR_CODE, 0x0CA
.equ                               VCU_COMMAND_TIMEOUT_ERROR_CODE, 0x0CB
.equ                               ILLEGAL_SOCKET_ID_ERROR_CODE, 0x0CC
.equ                               MCU_LOAD_ERROR_CODE, 0x0CD
.equ                               MMCFG_NOT_YET_ENABLED_ERROR_CODE, 0x0CE
.equ                               NEM_NO_FREE_MTRR_ERROR_CODE, 0x0D0
.equ                               NEM_DATA_RW_TEST_ERROR_CODE, 0x0D1

#
#MTRR Range Size and Alignment Requirement
#A range that is to be mapped to a variable-range MTRR must meet the following “power of 2” size and alignment rules:
#1. The minimum range size is 4 KBytes and the base address of the range must be
#   on at least a 4-KByte boundary.
#2. For ranges greater than 4 KBytes, each range must be of length 2n and its base
#   address must be aligned on a 2n boundary, where n is a value equal to or greater
#   than 12. The base-address alignment value cannot be less than its length. For
#   example, an 8-KByte range cannot be aligned on a 4-KByte boundary. It must be
#   aligned on at least an 8-KByte boundary.

#
# Define the No-Eviction Mode Code Region below 4GB
#
#ifdef MINIBIOS_BUILD
.equ                          CODE_REGION_BASE_ADDRESS, 0x0ffd00000                      # Base = 4GB-3MB
.equ                          CODE_REGION_SIZE, 0x0300000                      # Size = 3MB
.equ                          CODE_REGION_SIZE_MASK, (~(CODE_REGION_SIZE - 1))
#endif

#
# Define the No-Eviction Mode DataStack region
# This must be in an address range the chipset decodes
#
# FE000000 thru FE3FFFFF must not be used for NEM (now reserved for SPI controller config space)
.equ                          DATA_STACK_BASE_ADDRESS, 0x0fe800000
.equ                          DATA_STACK_SIZE, 0x200000                         # Size = 2MB
.equ                          DATA_STACK_SIZE_MASK, (~(DATA_STACK_SIZE - 1))

.equ                          LOCAL_APIC_BASE, 0x0fee00000
#
# Cache init and test values
# These are inverted to flip each bit at least once
#
.equ                          CACHE_INIT_VALUE, 0x0A5A5A5A5
.equ                          CACHE_TEST_VALUE, (~CACHE_INIT_VALUE)

.equ                          BIT0, 0x01
.equ                          BIT1, 0x02
.equ                          BIT2, 0x04
.equ                          BIT3, 0x08
.equ                          BIT4, 0x10
.equ                          BIT5, 0x20
.equ                          BIT6, 0x40
.equ                          BIT7, 0x80
.equ                          BIT8, 0x100
.equ                          BIT9, 0x200
.equ                          BIT10, 0x400
.equ                          BIT11, 0x800
.equ                          BIT12, 0x1000
.equ                          BIT13, 0x2000
.equ                          BIT14, 0x4000
.equ                          BIT15, 0x8000
.equ                          BIT16, 0x10000
.equ                          BIT17, 0x20000
.equ                          BIT18, 0x40000
.equ                          BIT19, 0x80000
.equ                          BIT20, 0x100000
.equ                          BIT21, 0x200000
.equ                          BIT22, 0x400000
.equ                          BIT23, 0x800000
.equ                          BIT24, 0x1000000
.equ                          BIT25, 0x2000000
.equ                          BIT26, 0x4000000
.equ                          BIT27, 0x8000000
.equ                          BIT28, 0x10000000
.equ                          BIT29, 0x20000000
.equ                          BIT30, 0x40000000
.equ                          BIT31, 0x80000000

#ifdef MINIBIOS_BUILD
#define cpu_rom_setup_options_sucode_region_addr   0      # uCode region base addr
#define cpu_rom_setup_options_ucode_region_size    4      # uCode region total size
#define cpu_rom_setup_options_nem_code_addr        8      # NEM code region base addr
#define cpu_rom_setup_options_nem_code_size       12      # NEM code region size
#define cpu_rom_setup_options_nem_data_addr       16      # NEM data stack region base addr
#define cpu_rom_setup_options_nem_data_size       20      # NEM data stack region size
#define cpu_rom_setup_options_END                 24
#endif

