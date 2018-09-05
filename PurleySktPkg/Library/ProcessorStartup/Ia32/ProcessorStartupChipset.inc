;
; This file contains an 'Intel Pre-EFI Module' and is licensed
; for Intel CPUs and Chipsets under the terms of your license 
; agreement with Intel or your vendor.  This file may be      
; modified by the user, subject to additional terms of the    
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 1999 - 2014, Intel Corporation. All rights reserved.<BR>
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
;  ProcessorStartupChipset.inc
;
; Abstract:
;
;   Chipset constants and macros
;
;------------------------------------------------------------------------------
; APTIOV_SERVER_OVERRIDE_RC_START : Use SDL token for PM base address.
include token.equ
; APTIOV_SERVER_OVERRIDE_RC_END : Use SDL token for PM base address.

;
; APIC register
;
APICID                        EQU 0FEE00020h

PCH_SPI_BAR                   EQU 0FE010000h
PCH_PWRM_BAR                  EQU 0FE000000h
PCH_TCO_BASE_ADDRESS          EQU 400h

; APTIOV_SERVER_OVERRIDE_RC_START : Added to halt TCO timer
ICH_IOREG_TCO1_STS            EQU 04h
ICH_IOREG_TCO1_CNT            EQU 08h
BIT_TCO_TMR_HLT               EQU 0800h
; APTIOV_SERVER_OVERRIDE_RC_END : Added to halt TCO timer

IFDEF PCH_LBG_ASM
;
; PCH PCR and ACPI base addresses
;
PCH_PCR_BASE                  EQU 0FD000000h
PCH_PCR_BASE_REG              EQU (000F9010h + PCIEXPRESS_BASE_ADDRESS)     ; PCH Register B0:D1F:F1:R10
PCH_P2SB_CMD_REG              EQU (000F9004h + PCIEXPRESS_BASE_ADDRESS)     ; PCH Register B0:D1F:F1:R04
; APTIOV_SERVER_OVERRIDE_RC_START : Use SDL token for PM base address.
PCH_ACPI_BASE_ADDRESS         EQU MKF_PM_BASE_ADDRESS
; APTIOV_SERVER_OVERRIDE_RC_END : Use SDL token for PM base address.
SB_REG_BAR		      EQU 0FD000000h

;
; PCH RTC configuration registers
;
PCH_PCR_RTC_CONF             EQU (0C30000h + 03400h)
PCH_PCR_RTC_CONF_UCMOS_EN    EQU 04h
PCH_RTC_CONF_UE_PCI_ADDR      EQU 04h

;
; PCH sideband PCR DMI GCS configuration
;
PCH_PCR_DMI_GCS_CONF         EQU (0EF0000h + 0274Ch)
ELSE
;
; RCBA related registers
;
PCH_ACPI_BASE_ADDRESS         EQU 0400h
PCH_RCRB_BASE                 EQU 0FED1C000h
PCH_RCRB_BASE_REG             EQU 8000F8F0h       ; PCH Register B0:D31:RF0
PCH_RCRB_GCS                  EQU 03410h
PCH_RCRB_RTC_CONF             EQU 03400h
PCH_RCRB_RTC_CONF_UCMOS_EN    EQU 04h
PCH_RCRB_HPET                 EQU 03404h
;
; GPIO registers
;
GPIO_BASE_ADDRESS	EQU	0500h
R_GPIO_USE_SEL2		EQU     030h
R_GPIO_IO_SEL2		EQU	034h
ENDIF
ACPI_PM1_STS                  EQU 000h
ACPI_PM1_CNT                  EQU 004h

;
; HPET general registers
;
PCH_HPET_REG                  EQU (000F9060h + PCIEXPRESS_BASE_ADDRESS)     ; PCH Register B0:D1F:F1:R60
PCH_HPET_AE                   EQU 80h             ; [7] Address Enable
HPET_COMP_1                   EQU 0FED00108h
HPET_COMP_2                   EQU 0FED0010Ch
HPET_COMP_3                   EQU 0FED00128h
HPET_COMP_4                   EQU 0FED00148h
PCH_HPET_DECODE               EQU 080h
PCH_HPET_CFG_PCI_ADDR	      EQU ((1Fh * 8 + 01h) * 1000h + 0040h)

;
; PCI registers
;
PCIEXPRESS_BASE_ADDRESS       EQU 080000000h
SYRE_CPURST                   EQU 14

;
; PCIEXBAR constants for enable in bit [0]
;
ENABLE                        EQU 1

;
; PCIEXBAR constants for size in bit [2:1]
;
PCIEXBAR_64MB                 EQU 010b
PCIEXBAR_128MB                EQU 001b
PCIEXBAR_256MB                EQU 000b


PCHBAR_REG                    EQU (048h + PCIEXPRESS_BASE_ADDRESS)
PCH_BASE_ADDRESS              EQU 0FED10000h

PCI_LPC_BASE                  EQU 08000F800h
PCI_PMC_BASE                  EQU 08000FA00h     ; PCH Register B0:D31:RF2
PCI_SPI_BASE                  EQU 08000FD00h     ; PCH Register B0:D31:RF5


;
; PCH SMBUS base address
;
PCH_SMBUS_BASE_ADDRESS        EQU 0EFA0h
PCH_SMBUS_CMD_REG             EQU (000FC004h + PCIEXPRESS_BASE_ADDRESS)     ; PCH Register B0:D1F:F4:R04
PCH_SMBUS_BASE_REG            EQU (000FC020h + PCIEXPRESS_BASE_ADDRESS)     ; PCH Register B0:D1F:F4:R20
PCH_SMBUS_HCFG_REG            EQU (000FC040h + PCIEXPRESS_BASE_ADDRESS)     ; PCH Register B0:D1F:F4:R40
PCH_SMBUS_TCOBASE_REG         EQU (000FC050h + PCIEXPRESS_BASE_ADDRESS)     ; PCH Register B0:D1F:F4:R50
PCH_SMBUS_TCOCTL_REG          EQU (000FC054h + PCIEXPRESS_BASE_ADDRESS)     ; PCH Register B0:D1F:F4:R54

;
; PCI registers
;
PCH_HSX_LPC_PMBASE_PCI_ADDR   EQU ((1Fh * 8 + 00h) * 1000h + 0040h)
PCH_HSX_LPC_ACPICNTL_PCI_ADDR EQU ((1Fh * 8 + 00h) * 1000h + 0044h)
PCH_LPC_PMBASE_PCI_ADDR       EQU ((1Fh * 8 + 00h) * 1000h + 0040h + PCIEXPRESS_BASE_ADDRESS)
PCH_LPC_ACPICNTL_PCI_ADDR     EQU ((1Fh * 8 + 00h) * 1000h + 0044h + PCIEXPRESS_BASE_ADDRESS)
PCH_PMC_PMBASE_PCI_ADDR       EQU ((1Fh * 8 + 02h) * 1000h + 0040h + PCIEXPRESS_BASE_ADDRESS)
PCH_PMC_ACPICNTL_PCI_ADDR     EQU ((1Fh * 8 + 02h) * 1000h + 0044h + PCIEXPRESS_BASE_ADDRESS)
PCH_LPC_RCRB_PCI_ADDR         EQU ((1Fh * 8 + 00h) * 1000h + 00F0h + PCIEXPRESS_BASE_ADDRESS)

;
; Use PCI write via CF8/CFC access mechanism to write MMCFG_BASE
; as (4GB -512MB) using GQ1_CR_PCIEXBAR
;
GQ1_CR_PCIEXBAR               EQU 80000150h       ; D0:F1:R50h

BIT1    EQU     002h
BIT2    EQU     004h
BIT3    EQU     008h
BIT4    EQU     010h
BIT7    EQU     080h
BIT9    EQU     0200h
;
;Defines for PCH DEVICE 31
;
SB_BUS                  EQU     00h
SB_PCI2ISA_DEVICE       EQU     31
SB_PCI2ISA_FUNC         EQU     0
SB_SMBUS_FUNC           EQU     3
;Word Equate for Bus, Device, Function number for I/O Controller Hub
SB_PCI2ISA_BUS_DEV_FUNC EQU     (SB_BUS SHL 8) + ((SB_PCI2ISA_DEVICE SHL 3)+ SB_PCI2ISA_FUNC)
SB_SMBUS_BUS_DEV_FUNC   EQU     (SB_BUS SHL 8) + ((SB_PCI2ISA_DEVICE SHL 3) + SB_SMBUS_FUNC)

; Define the equates here
PCI_LPC_BASE            EQU     BIT31 + (SB_PCI2ISA_BUS_DEV_FUNC SHL 8)
P2I_GEN_PMCON_2_OFFSET  EQU     0A2h            ; General PM Configuration 2
  CPUPWR_FLR    EQU     BIT1
P2I_GEN_PMCON_3_OFFSET  EQU     0A4h            ; General PM Configuration 3
  PWR_FLR       EQU     BIT1
  RTC_PWR_STS   EQU     BIT2
  GEN_RST_STS   EQU     BIT9
P2I_PMIR_OFFSET         EQU     0ACh            ; Power Management Intitialization Register.

PCI_SMBUS_BASE          EQU     BIT31 + (SB_SMBUS_BUS_DEV_FUNC SHL 8)
LPC_IO_KBC_ENB          EQU     083h


;
; IIO registers
;
IIO_MISCSS          EQU (20 SHL 15) + (0 SHL 12) + 09Ch ; B0:D20:F0:R9Ch
IIO_D20F0_DID       EQU (20 SHL 15) + (0 SHL 12) + 02h  ; B0:D20:F0:R02h
;
; Clarksfield IIO Scratch pad register equates
;
IIO_SPAD_SR00       EQU (20 SHL 15) + (1 SHL 12) + 07Ch ; B0:D20:F1:R7Ch
IIO_SPAD_SR01       EQU (20 SHL 15) + (1 SHL 12) + 080h ; B0:D20:F1:R80h
IIO_SPAD_SR02       EQU (20 SHL 15) + (1 SHL 12) + 084h ; B0:D20:F1:R84h
IIO_SPAD_SR03       EQU (20 SHL 15) + (1 SHL 12) + 088h ; B0:D20:F1:R88h
IIO_SPAD_SR04       EQU (20 SHL 15) + (1 SHL 12) + 08Ch ; B0:D20:F1:R8Ch
IIO_SPAD_SR05       EQU (20 SHL 15) + (1 SHL 12) + 090h ; B0:D20:F1:R90h
IIO_SPAD_SR06       EQU (20 SHL 15) + (1 SHL 12) + 094h ; B0:D20:F1:R94h

        
