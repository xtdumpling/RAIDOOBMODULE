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
#  ProcessorStartupChipset.inc
#
# Abstract:
#
#   Chipset constants and macros
#
#------------------------------------------------------------------------------

; APTIOV_SERVER_OVERRIDE_RC_START : Use SDL token for PM base address.
        .include "token.equ"
; APTIOV_SERVER_OVERRIDE_RC_END : Use SDL token for PM base address.

#
# APIC register
#
.equ                          APICID, 0x0FEE00020

.equ                          PCH_SPI_BAR, 0x0FE010000
.equ                          PCH_PWRM_BAR, 0x0FE000000
.equ                          PCH_TCO_BASE_ADDRESS, 0x400

; APTIOV_SERVER_OVERRIDE_RC_START : Added to halt TCO timer
.equ                          ICH_IOREG_TCO1_STS, 04h
.equ                          ICH_IOREG_TCO1_CNT, 08h
.equ                          BIT_TCO_TMR_HLT, 0800h
; APTIOV_SERVER_OVERRIDE_RC_END : Added to halt TCO timer

#ifdef PCH_LBG_ASM
#
# PCH PCR and ACPI base addresses
#
.equ                          PCH_PCR_BASE, 0x0FD000000
.equ                          PCH_PCR_BASE_REG, (0x000F9010 + PCIEXPRESS_BASE_ADDRESS)      # PCH Register B0:D1F:F1:R10
.equ                          PCH_P2SB_CMD_REG, (0x000F9004 + PCIEXPRESS_BASE_ADDRESS)      # PCH Register B0:D1F:F1:R04
; APTIOV_SERVER_OVERRIDE_RC_START : Use SDL token for PM base address.
.equ                          PCH_ACPI_BASE_ADDRESS, MKF_PM_BASE_ADDRESS
; APTIOV_SERVER_OVERRIDE_RC_END : Use SDL token for PM base address.
.equ                          PCH_ACPI_BASE_ADDRESS, 0x0500
.equ                          SB_REG_BAR, 0x0FD000000

#
# PCH RTC configuration registers
#
.equ                         PCH_PCR_RTC_CONF, (0x0C30000 + 0x03400)
.equ                         PCH_PCR_RTC_CONF_UCMOS_EN, 0x04
.equ                         PCH_RTC_CONF_UE_PCI_ADDR, 0x04

#
# PCH sideband PCR DMI GCS configuration
#
.equ                         PCH_PCR_DMI_GCS_CONF, (0x0EF0000 + 0x0274C)
#else
#
# RCBA related registers
#
.equ                          PCH_ACPI_BASE_ADDRESS, 0x0400
.equ                          PCH_RCRB_BASE, 0x0FED1C000
.equ                          PCH_RCRB_BASE_REG, 0x8000F8F0        # PCH Register B0:D31:RF0
.equ                          PCH_RCRB_GCS, 0x03410
.equ                          PCH_RCRB_RTC_CONF, 0x03400
.equ                          PCH_RCRB_RTC_CONF_UCMOS_EN, 0x04
.equ                          PCH_RCRB_HPET, 0x03404
#
# GPIO registers
#
.equ                    GPIO_BASE_ADDRESS, 0x0500
.equ                    R_GPIO_USE_SEL2, 0x030
.equ                    R_GPIO_IO_SEL2, 0x034
#endif
.equ                          ACPI_PM1_STS, 0x000
.equ                          ACPI_PM1_CNT, 0x004

#
# HPET general registers
#
.equ                          PCH_HPET_REG, (0x000F9060 + PCIEXPRESS_BASE_ADDRESS)      # PCH Register B0:D1F:F1:R60
.equ                          PCH_HPET_AE, 0x80              # [7] Address Enable
.equ                          HPET_COMP_1, 0x0FED00108
.equ                          HPET_COMP_2, 0x0FED0010C
.equ                          HPET_COMP_3, 0x0FED00128
.equ                          HPET_COMP_4, 0x0FED00148
.equ                          PCH_HPET_DECODE, 0x080
.equ                          PCH_HPET_CFG_PCI_ADDR, ((0x1F * 8 + 0x01) * 0x1000 + 0x0040)

#
# PCI registers
#
.equ                          PCIEXPRESS_BASE_ADDRESS, 0x080000000
.equ                          SYRE_CPURST, 14

#
# PCIEXBAR constants for enable in bit [0]
#
.equ                          ENABLE, 1

#
# PCIEXBAR constants for size in bit [2:1]
#
.equ                          PCIEXBAR_64MB, 0x02
.equ                          PCIEXBAR_128MB, 0x01
.equ                          PCIEXBAR_256MB, 0x00


.equ                          PCHBAR_REG, (0x048 + PCIEXPRESS_BASE_ADDRESS)
.equ                          PCH_BASE_ADDRESS, 0x0FED10000

.equ                          PCI_LPC_BASE, 0x08000F800
.equ                          PCI_PMC_BASE, 0x08000FA00      # PCH Register B0:D31:RF2
.equ                          PCI_SPI_BASE, 0x08000FD00      # PCH Register B0:D31:RF5


#
# PCH SMBUS base address
#
.equ                          PCH_SMBUS_BASE_ADDRESS, 0x0EFA0
.equ                          PCH_SMBUS_CMD_REG, (0x000FC004 + PCIEXPRESS_BASE_ADDRESS)      # PCH Register B0:D1F:F4:R04
.equ                          PCH_SMBUS_BASE_REG, (0x000FC020 + PCIEXPRESS_BASE_ADDRESS)      # PCH Register B0:D1F:F4:R20
.equ                          PCH_SMBUS_HCFG_REG, (0x000FC040 + PCIEXPRESS_BASE_ADDRESS)      # PCH Register B0:D1F:F4:R40
.equ                          PCH_SMBUS_TCOBASE_REG, (0x000FC050 + PCIEXPRESS_BASE_ADDRESS)      # PCH Register B0:D1F:F4:R50
.equ                          PCH_SMBUS_TCOCTL_REG, (0x000FC054 + PCIEXPRESS_BASE_ADDRESS)      # PCH Register B0:D1F:F4:R54

#
# PCI registers
#
.equ                          PCH_HSX_LPC_PMBASE_PCI_ADDR, ((0x1F * 8 + 0x00) * 0x1000 + 0040)
.equ                          PCH_HSX_LPC_ACPICNTL_PCI_ADDR, ((0x1F * 8 + 0x00) * 0x1000 + 0044)
.equ                          PCH_LPC_PMBASE_PCI_ADDR, ((0x1F * 8 + 0x00) * 0x1000 + 0x0040 + PCIEXPRESS_BASE_ADDRESS)
.equ                          PCH_LPC_ACPICNTL_PCI_ADDR, ((0x1F * 8 + 0x00) * 0x1000 + 0x0044 + PCIEXPRESS_BASE_ADDRESS)
.equ                          PCH_PMC_PMBASE_PCI_ADDR, ((0x1F * 8 + 0x02) * 0x1000 + 0x0040 + PCIEXPRESS_BASE_ADDRESS)
.equ                          PCH_PMC_ACPICNTL_PCI_ADDR, ((0x1F * 8 + 0x02) * 0x1000 + 0x0044 + PCIEXPRESS_BASE_ADDRESS)
.equ                          PCH_LPC_RCRB_PCI_ADDR, ((0x1F * 8 + 0x00) * 0x1000 + 0x00F0 + PCIEXPRESS_BASE_ADDRESS)

#
# Use PCI write via CF8/CFC access mechanism to write MMCFG_BASE
# as (4GB -512MB) using GQ1_CR_PCIEXBAR
#
.equ                          GQ1_CR_PCIEXBAR, 0x80000150        # D0:F1:R50h

.equ    BIT1,   0x002
.equ    BIT2,   0x004
.equ    BIT3,   0x008
.equ    BIT4,   0x010
.equ    BIT7,   0x080
.equ    BIT9,   0x0200
#
#Defines for PCH DEVICE 31
#
.equ                    SB_BUS, 0x00
.equ                    SB_PCI2ISA_DEVICE, 31
.equ                    SB_PCI2ISA_FUNC, 0
.equ                    SB_SMBUS_FUNC, 3
#Word Equate for Bus, Device, Function number for I/O Controller Hub
.equ                    SB_PCI2ISA_BUS_DEV_FUNC, (SB_BUS << 8) + ((SB_PCI2ISA_DEVICE << 3) + SB_PCI2ISA_FUNC)
.equ                    SB_SMBUS_BUS_DEV_FUNC, (SB_BUS << 8) + ((SB_PCI2ISA_DEVICE << 3) + SB_SMBUS_FUNC)

# Define the equates here
.equ                    PCI_LPC_BASE, BIT31 + (SB_PCI2ISA_BUS_DEV_FUNC << 8)
.equ                    P2I_GEN_PMCON_2_OFFSET, 0x0A2             # General PM Configuration 2
  .equ          CPUPWR_FLR, BIT1
.equ                    P2I_GEN_PMCON_3_OFFSET, 0x0A4             # General PM Configuration 3
  .equ          PWR_FLR, BIT1
  .equ          RTC_PWR_STS, BIT2
  .equ          GEN_RST_STS, BIT9
.equ                    P2I_PMIR_OFFSET, 0x0AC             # Power Management Intitialization Register.

.equ                    PCI_SMBUS_BASE, BIT31 + (SB_SMBUS_BUS_DEV_FUNC << 8)
.equ                    LPC_IO_KBC_ENB, 0x083


#
# IIO registers
#
.equ                IIO_MISCSS, (20 << 15) + (0 << 12) + 0x09C  # B0:D20:F0:R9Ch
.equ                IIO_D20F0_DID, (20 << 15) + (0 << 12) + 0x02   # B0:D20:F0:R02h
#
# Clarksfield IIO Scratch pad register equates
#
.equ                IIO_SPAD_SR00, (20 << 15) + (1 << 12) + 0x07C  # B0:D20:F1:R7Ch
.equ                IIO_SPAD_SR01, (20 << 15) + (1 << 12) + 0x080  # B0:D20:F1:R80h
.equ                IIO_SPAD_SR02, (20 << 15) + (1 << 12) + 0x084  # B0:D20:F1:R84h
.equ                IIO_SPAD_SR03, (20 << 15) + (1 << 12) + 0x088  # B0:D20:F1:R88h
.equ                IIO_SPAD_SR04, (20 << 15) + (1 << 12) + 0x08C  # B0:D20:F1:R8Ch
.equ                IIO_SPAD_SR05, (20 << 15) + (1 << 12) + 0x090  # B0:D20:F1:R90h
.equ                IIO_SPAD_SR06, (20 << 15) + (1 << 12) + 0x094  # B0:D20:F1:R94h



