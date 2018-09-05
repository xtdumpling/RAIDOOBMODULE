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
#  ProcessorStartupUncore.asm
#
# Abstract:
#
#
#------------------------------------------------------------------------------

        .include "ProcessorStartupPlatform.i"
        .include "ProcessorStartupChipset.i"
        .include "ProcessorStartupUncore.i"
        .include "ProcessorStartup.i"

        .extern ReadPciLegacy
        .extern WritePciLegacy
        .extern _OEMCpuSecInit

#----------------------------------------------------------------------------
#       STARTUP_SEG  S E G M E N T  STARTS
#----------------------------------------------------------------------------
.code32

#----------------------------------------------------------------------------
# Executed by all socket BSPs present in the system early after reset
# - Enable MMCFG_RULE on local socket
# - Set IO, MMIO target lists  to point to legacy socket
# - Init MMIO_RULE11 to cover the 64MB below 4GB for NEM DATA region
# -
#  Input:  All general puropse registers undefined
#          Stack may not be available
#          MMX4 = routine return address
#          MMCFG_Rule is enabled on local socket
#
#  Output: All general puropse registers undefined.
#          MMCFG and some related CSRs are initialized to allow CSR accesses on local socket via MMCFG method
#          Following xmm registers hold MMCFG and bus number config scheme info (later used by otehr routines in this file)
#                xmm0 = [3:0]  local socket id
#                             [7:4]  legact socket id
#                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
#                             [29:28]  rsvd
#                             [30]    Legacy socket flag
#                             [31] = Warm Reset Path flag
#                xmm1 = MCFG_TARGET_LIST CSR current data
#                xmm3 = MMCFG_RULE CSR current data
#                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
#                xmm5 = CPUBUSNO [Bus4,Bus5]
#----------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(EarlyUncoreInit)
ASM_PFX(EarlyUncoreInit):

      #
      # All sockets come here after reset
      #
      # In CR4, set OSFXSR bit 9 and OSxmmEXCPT bit 10 to enable access to xmm registers used by this routine
      #
      movl  %cr4, %eax
      orl   $(BIT10+BIT9), %eax
      movl  %eax, %cr4

      # Use pre-defined scheme for temp config
      movl  $DEFAULT_COLDBOOT_CPUBUSNO, %eax
      movd  %eax, %xmm4
      movl  $DEFAULT_COLDBOOT_CPUBUSNO1, %eax
      movd  %eax, %xmm5

      # xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
      # xmm5 = CPUBUSNO [Bus4,Bus5]

      #
      # At this point, only CF8/CFC IO port can be used for PCI config access
      #

      #
      # Disable LOCK lockcontrol.lockdisable  at 0:8:0:0x50, bit0
      # TODO:    bit0 is set by default already?
      #
      #mov   eax, $CSR_LEGACY_LOCKCONTROL
      #READ_PCI_DWORD_LEGACY                             ; ebx = reg data
      #or    ebx, $BIT0                                   ; set bit0
      #WRITE_PCI_DWORD_LEGACY

      #
      # Read local socket ID and legacy socket ID, and store them in xmm0
      #
      movl  $CSR_LEGACY_SAD_CONTROL_CBO, %eax            # get local  socket ID
      READ_PCI_DWORD_LEGACY
      movl  %ebx, %ecx
      andl  $(BIT2+BIT1+BIT0), %ecx                      # keep bit[2:0] local socket ID, ignore bit3

      movl  $CSR_LEGACY_SAD_TARGET_CBO, %eax             # get legacy socket ID
      READ_PCI_DWORD_LEGACY
      andl  $(BIT7+BIT6+BIT5+BIT4), %ebx                 # keep bit[7:4] legacy_pch_target
      btrl  $7, %ebx                                    # test & clear bit7  (local/remote?)
      jnc   eui_02                                      # remote socket ID
      #
      # For legacy socket,  use local socket ID as legacy socket ID, and set legacy socket flag bit
      #
      movl  %ecx, %ebx
      shll  $4, %ebx                                    # bit[7:4] = Legacy socket id
      orl   $BIT30, %ebx                                 # bit[30] = Legacy Socket flag
eui_02: 
      orl   %ebx, %ecx
      movd  %ecx, %xmm0                                  # xmm0[7:4] = legacy socket id, [3:0]= local socket id

      #
      # Get CPU Faimly/stepping info and store it in xmm0[27:8]
      #
      movl $1, %eax
      cpuid
      andl $0xfffff, %eax                               # keep Falily/Stepping info
      shll $8, %eax                                     # shift to bit[27:8]
      movd %xmm0, %ebx
      orl  %eax, %ebx
      movd %ebx, %xmm0

      #
      # Read warm reset flag from sticky scratch pad reg and save it in xmm0
      #
      movl  $SSR_LEGACY_BOOT_CONFIG_INFO_CSR, %eax
      READ_PCI_DWORD_LEGACY
      testl $BIT5, %ebx                                  # Warm Reset Path (Cold Reset Flow Completed = 1)?
      jnz   eui_warmboot                                # Yes
      testl $BIT4, %ebx                                  # Cold Reset Flow Entered flag = 1?
      jz    eui_coldboot                                # No (normal cold boot path)

      #
      # Warm reset occured before completion of MRC on the cold boot path.
      # In this case SBSP triggers a power-good reset while other socket BSPs waits
      #

      CALL_MMX6 AmILegacySocketNoStack
      jnz   eui_06                                      # no ,skip
      # Legacy BSP comes here - Triggers a power-good reset.
      movb  $0xe, %al
      movw  $0xcf9, %dx
      outb  %al, %dx
eui_06: 
#TODO:
#  comment this out until PEI code has proper handling of BIT4/BIT5 flags
#      jmp   $                                           ; wait for reset to occur


eui_coldboot: 
      orl   $BIT4, %ebx                                  # set Cold Boot Flow Entered flag = 1
      WRITE_PCI_DWORD_LEGACY
      jmp   eui_20

eui_warmboot:                                            # Warm reset path
      #and   ebx, NOT (BIT4)                             ; clear bit4 of scratch pad register
      btrl  $4, %ebx                                    # clear bit4 of scratch pad register
      WRITE_PCI_DWORD_LEGACY

      movd  %xmm0, %ebx
      orl   $BIT31, %ebx                                 # Set warm reset path flag in xmm reg
      movd  %ebx, %xmm0

eui_20: 
      #
      # Initialize  MMCFG_TARGET_LIST (at 1:29:1:ec)  to target local socket
      #

      movl  $CSR_LEGACY_MMCFG_TARGET_LIST_CBO, %eax
      READ_PCI_DWORD_LEGACY
      orl   $0x88888888, %ebx                           # make sure that all entries are marked as "local"
      WRITE_PCI_DWORD_LEGACY
      movd  %ebx, %xmm1                                  # save it in xmm1
      #
      # Initialize  CSR_LEGACY_MMCFG_LOCAL_RULE_CBO (at 1:29:1:e4)
      #    ModeX=0 means  "send devices 16-31 at the root bus to the stack rather than to ubox"
      #    ModeX=1 means  "send everything to the ubox"
      #    For legacy socket, set Mode=0, and set all the others to ModeX=1
      #
      movl $CSR_LEGACY_MMCFG_LOCAL_RULE_CBO, %eax
      movl $DEFAULT_MMCFG_LOCAL_RULE, %ebx
      movd  %xmm0, %ecx
      testl $BIT30, %ecx                                 # Am I Legacy Socket?
      jz   eui_22                                       # No, skip
      btrl $24, %ebx                                    # For legacy socket, set Mode0 = 0
eui_22: 
      WRITE_PCI_DWORD_LEGACY

      #Initialize MMCFG_LOCAL_RULE_ADDRESS_CFG (at 1:29:1:c8, cc)
      #program local stack bus numbers in Cha
      movl  $CSR_LEGACY_MMCFG_LOCAL_RULE_ADDRESS0_CBO, %eax
      movl  $DEFAULT_MMCFG_LOCAL_RULE_ADDRESS0, %ebx     # ebx = CSR data
      WRITE_PCI_DWORD_LEGACY

      movl  $CSR_LEGACY_MMCFG_LOCAL_RULE_ADDRESS1_CBO, %eax
      movl  $DEFAULT_MMCFG_LOCAL_RULE_ADDRESS1, %ebx     # ebx = CSR data
      WRITE_PCI_DWORD_LEGACY

      #
      # Initialize and enable MMCFG_Rule register
      # a. init Cbo MMCFG_RULE  (at 1:29:1:C0)
      #
      movl  $(CSR_LEGACY_MMCFG_RULE_CBO + 4), %eax
      xorl  %ebx, %ebx
      WRITE_PCI_DWORD_LEGACY
      movl  $CSR_LEGACY_MMCFG_RULE_CBO, %eax
      READ_PCI_DWORD_LEGACY
      movl  $MMCFG_RULE_ADDR_MASK, %ecx
      notl  %ecx
      andl  %ecx, %ebx                                  # mask off base address
      andb  $0xf0, %bl                                  # mask off Length, keep Valid_Targets
      orl   $DEFAULT_MMCFG_RULE, %ebx                    # update base addr and length
      ##or    ebx, 0ffh SHL 6                                                        ; set valid bit for all local targets (default)
      orl   $BIT0, %ebx                                  # set enable bit
      WRITE_PCI_DWORD_LEGACY
      movd  %ebx, %xmm3                                  # save current reg value in xmm3

# ----------------------------------------------------------------------------------------------
# From this point on, local CSRs can be accessed at default bus numbers via MMCFG method
# Since MMCFG_TARGET_LIST points to local socket only, all CSR access are local
# ----------------------------------------------------------------------------------------------

      #
      # b. init Ubox MMCFG_RULE (at 0:8:2:dc)
      # Need to convert the mmcfg length field between the two registers
      # CBO_CR_MMCFG_RULE_CFG.Length
      #    3'b110 : MaxBusNumber = 63 (ie. 64MB MMCFG range)
      #    3'b101 : MaxBusNumber = 127 (ie 128MB MMCFG range)
      #    3'b100 : MaxBusNumber = 256 (ie 256MB MMCFG range)
      #    3'b010 : MaxBusNumber = 512 (ie 512MB MMCFG range)
      #    3'b001 : MaxBusNumber = 1048 (ie 1G MMCFG range)
      #    3'b000 : MaxBusNumber = 2048 (ie 2G MMCFG range)
      #
      # UBOX NCDECS_CR_MMCFG_RULE_CFG_REG.length
      #     2'b10 : MaxBusNumber = 63 (ie 64MB MMCFG range)
      #     2'b01 : MaxBusNumber = 127 (ie 128MB MMCFG range)
      #     2'b00 : MaxBusNunber >= 256 (ie 256MB MMCFG range)

      movd  %xmm3, %edx                                  # edx = current MMCFG_RULE_CFG csr data
      andl  $(BIT3+BIT2+BIT1+BIT0), %edx                 # keep mmcfgRule bus length and enable bits [3:0]
      btrl  $3, %edx                                    # test and clear bit3
      jc    eui_30                                      # length <= 256 buses
      andl  $BIT0, %edx                                  # clear length to 2b'00 if > 256 buses
eui_30: 
      movd  %xmm3, %edi
      andl  $MMCFG_RULE_ADDR_MASK, %edi                  # edi = MMCFG base addr
      movl  $CSR_MMCFG_RULE_UBOX, %ebx
      movl  $(BIT2+BIT1+BIT0), %eax                      # clear bit[2:0]
      notl  %eax
      andl  %ds:(%edi,%ebx), %eax
      orl   %edx, %eax                                  # update bit[3:0]
      movl  %eax, %ds:(%edi,%ebx)

      #
      # Program IOPORT_TARGET_LIST, MMIO_TARGET_LIST  before any io transaction. Legacy socket is the target
      #
      movl  $0x88888888, %eax                           # target list for legacy socket itself
      CALL_MMX6 AmILegacySocketNoStack
      je    eac_40                                      # yes

      movd  %xmm0, %eax
      andl  $(BIT7+BIT6+BIT5+BIT4), %eax                 # eax [7:4] = legacy socket id
      shrl  $4, %eax
      movl  %eax, %ebx                                  # ebx[3:0] = eax[3:0] = legacy socket id
      movb  $4, %cl                                     # number of bits for socket id
      movb  $7, %ch
eac_30: 
      shl   %cl, %ebx
      orl   %ebx, %eax
      dec   %ch
      jnz   eac_30

eac_40: 
      #
      # edi = MMCFG base addr
      # eax = target list with all entries targeting legacy socket
      #
      movl  $CSR_IOPORT_TARGET_LIST_0_CBO, %ebx
      movl  %eax, %ds:(%edi,%ebx)

      movl  $CSR_MMIO_TARGET_LIST_0_CBO, %ebx
      movl  %eax, %ds:(%edi,%ebx)

      movl  $CSR_MMIO_TARGET_LIST_1_CBO, %ebx
      movl  %eax, %ds:(%edi,%ebx)

      #
      # Enable the MMIO_RULE11 to cover the 0xFC000000 to 0xFFFFFFFF so that the NEM DATA region can be mapped to it.
      # The target is set to local IIO.
      #

      movl  $(CSR_MMIO_RULE11_CBO + 4), %ebx
      movl  $0x30000, %eax                              # limitaddress_vlsb_bits = 3
      movl  %eax, %ds:(%edi,%ebx)

      movl  $CSR_MMIO_RULE11_CBO, %ebx
      movl  $0xFF00007F, %eax                           # Base = 0xFC000000; Limit = 0xFFFFFFFF; Set the enable bit
      movl  %eax, %ds:(%edi,%ebx)

      RET_ESI_MMX3


#----------------------------------------------------------------------------
# Executed by all socket BSPs present in the system early after reset
#
#  Input:  All general puropse registers undefined
#          Stack may not be available
#          MMX3 = routine return address
#          MMCFG_Rule is enabled on local socket
#
#  Output: All general puropse registers undefined.
#----------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(MidUncoreInit)
ASM_PFX(MidUncoreInit):

#
#  Check if running on Simics emulation
#

        CALL_MMX6 AmILegacySocketNoStack
        jnz   mui_exit                                    # no ,skip
        #
        # Read EMULATION_FLAG CSR  and set the Emulation_flag in xmm0
        #
        movl  $CSR_LEGACY_EMULATION_FLAG_DMI, %eax
        READ_PCI_DWORD_LEGACY
        testl $(BIT2+BIT1+BIT0), %ebx                      # 0 = real hardware
        jz    mui_exit
#
# Check register header version against Simics
#
        movl  $MSR_SIMICS_CSR_VERSION, %ecx
        rdmsr                                             # eax = Simics CSR header version
        cmpl  $BIOS_CSR_HEADER_VERSION, %eax
        #jae   mui_exit                                     ; version checking pass
        je   mui_exit                                     # version checking pass

csr_version_check_failure: 
        movl  $BIOS_CSR_HEADER_VERSION, %eax
        wrmsr                                             # trigger a Simics exception alert
        #jmp $

mui_exit: 
        RET_ESI_MMX3


#----------------------------------------------------------------------------
# Executed by all socket BSPs present in the system early after reset
#
#  Input:  All general puropse registers undefined
#          Stack may not be available
#          MMX3 = routine return address
#          MMCFG_Rule is enabled on local socket
#
#  Output: All general puropse registers undefined.
#----------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(PreNemUncoreInit)
ASM_PFX(PreNemUncoreInit):
        #
        # Prior to NEM init, use VCU API to notify VCU of "BIOS is about to enable cache"
        #
        movd  %xmm0, %eax
        shrl  $12, %eax
        andl  $0xffff, %eax                             # [15:0] = Cpu Family
        cmpl  $CPU_FAMILY_SKX, %eax                      # SKX CPU?
        jne   pnui_exit                                 # No, skip

        # A this point, MMCFG_RULE has been enabled to target local CSRs
        CALL_MMX6 GetMmcfgNoStack

        #
        # check if running on real hardware or simulation tool
        #

        movl  $CSR_EMULATION_FLAG_DMI, %ebx              # ebx = Dev:Func:Offset of EMULATION_FLAG CSR
        movl  %ds:(%edi,%ebx), %eax
        orb   %al, %al                                  # read hardware?
        jnz   pnui_exit                                 # no, skip

        movl  $VCODE_API_DELAY_COUNT, %ecx               # software timeout loop-count
        movl  $CSR_VCU_MAILBOX_INTERFACE, %ebx           # ebx = Bus:Dev:Func:Offset of VCU_MAILBOX_INTERFACE CSR
        movl  $CSR_VCU_MAILBOX_DATA, %edx                # edx = Bus:Dev:Func:Offset of VCU_MAILBOX_DATA CSR

poll_vcu_busy1: 
        mov   %ds:(%edi,%ebx), %eax
        cmpl  $0xffffffff, %eax
        je    pnui_vcu_failure                          # debug: - skip if the CSR is not implemented
        testl $BIT31, %eax
        loopnz poll_vcu_busy1
        orl   %ecx, %ecx
        jz    pnui_vcu_failure

        #
        #  Call VCU API Open Sequnece (IOT_LLC_SETUP)
        #
        movl  $VCODE_API_SEQ_ID_IOT_LLC_SETUP, %eax
        movl  %eax, %ds:(%edi,%edx)

        movl  $(BIT31 | VCODE_API_OPCODE_OPEN_SEQUENCE), %eax
        movl  %eax, %ds:(%edi,%ebx)

        movl  $VCODE_API_DELAY_COUNT, %ecx               # software timeout loop-count
poll_vcu_busy2:
        movl  %ds:(%edi,%ebx), %eax
        testl $BIT31, %eax
        loopnz poll_vcu_busy2
        orl   %ecx, %ecx
        jz    pnui_vcu_failure

        #
        #  Call VCU API SET_PARAMETER with Index = 0
        #  This step is required although the parameters  is dont care
        #

        movl  $0, %eax                                  # set data = 0
        movl  %eax, %ds:(%edi,%edx)
        movl  $(BIT31 | VCODE_API_OPCODE_SET_PARAMETER), %eax
        movl  %eax, %ds:(%edi,%ebx)

        movl  $VCODE_API_DELAY_COUNT, %ecx               # software timeout loop-count
poll_vcu_busy3:
        movl  %ds:(%edi,%ebx), %eax
        testl $BIT31, %eax
        loopnz poll_vcu_busy3
        orl   %ecx, %ecx
        jz    pnui_vcu_failure

        #
        #  Call VCU API  IOT_LLC_SETUP with Index=1
        #
        movl  $(BIT31 | VCODE_API_OPCODE_IOT_LLC_SETUP), %eax
        orl   $(1 << 16), %eax
        movl  %eax, %ds:(%edi,%ebx)

        movl  $VCODE_API_DELAY_COUNT, %ecx                   # software timeout loop-count
poll_vcu_busy4:
        movl  %ds:(%edi,%ebx), %eax
        testl $BIT31, %eax
        loopnz poll_vcu_busy4
        orl   %ecx, %ecx
        jz    pnui_vcu_failure

        #
        # Call VCU API Close Sequnece (IOT_LLC_SETUP)
        #
        movl  $VCODE_API_SEQ_ID_IOT_LLC_SETUP, %eax
        movl  %eax, %ds:(%edi,%edx)

        movl  $(BIT31 | VCODE_API_OPCODE_CLOSE_SEQUENCE), %eax
        movl  %eax, %ds:(%edi,%ebx)

        movl  $VCODE_API_DELAY_COUNT, %ecx                   # software timeout loop-count
poll_vcu_busy5:
        movl  %ds:(%edi,%ebx), %eax
        testl $BIT31, %eax
        loopnz poll_vcu_busy5
        orl   %ecx, %ecx
        jz    pnui_vcu_failure

pnui_exit: 
        RET_ESI_MMX3


pnui_vcu_failure: 
        PORT80(VCU_COMMAND_TIMEOUT_ERROR_CODE)
        jmp  pnui_exit


#----------------------------------------------------------------------------
# Executed by all socket BSPs present in the system early after reset
#
#  Input:  All general puropse registers undefined
#          Stack may not be available
#          MMX3 = routine return address
#          MMCFG_Rule is enabled on local socket
#
#  Output: All general puropse registers undefined.
#----------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(PostNemUncoreInit)
ASM_PFX(PostNemUncoreInit):

        call ProcessorWorkaround                # Early CPU WAs
# CPL3 change - Enable Code after Mailbox Command is available.
        call  GetMmcfgBase                      # esi = MMCFG.base
        movl  $1, %eax
        cpuid
        andl  $0x0ffff0, %eax
        cmpl  $(CPU_FAMILY_SKX << 4), %eax  #Check for SKX family
        jne   EndCpl3

        # Only set P1 on warm reset
        movl  $CSR_BIOSSCRATCHPAD7, %ebx
        movl  %ds:(%esi,%ebx), %eax
        andl  $BIT5, %eax
        jz    EndCpl3

        # PCU Mailbox Interface
        movl  $CSR_BIOS_MAILBOX_INTERFACE_PCU_FUN1_REG, %ebx
        movb  $30, %cl
RunBusyLoop1:
        decb  %cl
        jz    EndCpl3
        testl $0x80000000, %ds:(%esi,%ebx) #Check for RUN_BUSY bit
        jnz   RunBusyLoop1

        # Set mailbox data register
        movl  $CSR_BIOS_MAILBOX_DATA_PCU_FUN1_REG, %ebx
        movl  $BIOS_MAILBOX_DATA_SWITCHTO_P1, %ds:(%esi,%ebx)

        # Write mailbox cmd register
        movl  $CSR_BIOS_MAILBOX_INTERFACE_PCU_FUN1_REG, %ebx
        movl  $BIOS_MAILBOX_COMMAND_SWITCHTO_P1, %ds:(%esi,%ebx)  # Request for P1 speed

        movb  $30 , %cl
RunBusyLoop2:
        decb  %cl
        jz    EndCpl3
        testl $0x80000000, %ds:(%esi,%ebx) # Check for RUN_BUSY bit
        jnz   RunBusyLoop2
EndCpl3:
# CPL3 change

        ret


#----------------------------------------------------------------------------
# EnableAccessGlobalCSR_NoStack
# Executed by SBSP Legacy socket only
#     - Update MMCFT_TARGET_LIST to enable cross-socket CSR access,
#     - Enable CPUBUSNO/CPUBUSNO1
#-----------------------------------------------------------------------
#
#  Input:  All general purpose registers undefined
#          Stack may not be available
#          MMX3 = routine return address
#          MMCFG_Rule is enabled on local socket
#
#          Following xmm registers have been initialized to hold such config info:
#                xmm0 = [3:0]  local socket id
#                             [7:4]  legact socket id
#                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
#                             [29:28]  rsvd
#                             [30]    Legacy socket flag
#                             [31] = Warm Reset Path flag
#                xmm1 = MCFG_TARGET_LIST CSR current data
#                xmm3 = MMCFG_RULE CSR current data
#                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
#                xmm5 = CPUBUSNO [Bus4,Bus5]
#
#  Output: All general purpose registers undefined.
#          MMCFG_TARGET_LIST, CPUBUSNO related CSRs are modified/initialized.
#----------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(EnableAccessGlobalCSR_NoStack)
ASM_PFX(EnableAccessGlobalCSR_NoStack):

      movd  %xmm3, %edi
      andl  $MMCFG_RULE_ADDR_MASK, %edi          # edi = MMCFG_BASE_ADDR

      # init MMCFG CSR in IIO (at 0:5:0:90)
      movl  $CSR_MMCFG_IIO, %ebx
      movd  %xmm3, %eax                          # eax = copy of MMCFG_RULE
      andl  $MMCFG_RULE_ADDR_MASK, %eax          # mask off bit[13:0]
      movl  %eax, %ds:(%edi,%ebx)
      # clear the higher address dword
      xorl  %eax, %eax
      movl  %eax, %ds:4(%edi,%ebx)

      # calculate mmcfg limit = mmcfg_base + Max_Bus * 1MB)
      movd  %xmm3, %ecx                          # ecx = copy of MMCFG_RULE
      andl  $14, %ecx
      shrb  %cl                                 # cl = max bus number encoding.
                                                #      0: 2048, 1: 1024, 2:512, 3: rsvd, 4: 256, 5: 128, 6: 64
      cmpb  $4, %cl
      jb    gcs20
      decb  %cl                                 #  adjuct shift count in cl
gcs20: 
      movl  $0x800, %eax                        # eax = 2048 (buses)
      shr   %cl, %eax                           # eax = 256 / 128 / 64 buses
      shll  $20, %eax                           # eax = eax * 100000h (i.e.Max_Bus * 1MB)

      movd  %xmm3, %ecx                          # ecx = copy of MMCFG_RULE
      andl  $MMCFG_RULE_ADDR_MASK, %ecx          # ecx = MMCFG_BASE_ADDR
      addl  %ecx, %eax                          # eax = MMCFG_BASE_ADDR + MMCFG_SIZE
      subl  $1, %eax                            # last decode byte is last byte in previous 64MB block
      andl  $0xfc000000, %eax                   # 64MB block address of last block of MMCFG
      addl  $CSR_MMCFG_IIO_BASE_LIMIT_GAP, %ebx  # CSR_MMCFG_IIO + gap   (MMCFG.Limit) (HSXB0 Update: Change CSR_MMCFG_IIO_BASE_LIMIT_GAP_A0 to CSR_MMCFG_IIO_BASE_LIMIT_GAP)
      movl  %eax, %ds:(%edi,%ebx)

#-----------------------------------------------------------------------
#     - Reprogram MMCFG_TARGET_LIST to enable cross-socket CSR access from legacy socket
#     - Enable CPUBUSNO/CPUBUSNO1
#-----------------------------------------------------------------------

# Choose MMCFG target list based on SBSP (legacy) socketID, so that
#   - Each of the 8 sockets is mapped to a range of 32 (20h) buses
#   - Legacy socket is always mapped to the bus range 00h - 1Fh
#
# SBSP    MMCFG Target_list
# -----  -------------------------
#  S0        76543210   (Default)
#  S1        76543201
#  S2        76543102
#  S3        76542103
#  S4        76532104
#  S5        76432105
#  S6        75432106
#  S7        65432107

# Get legacy socketID
      movd  %xmm0, %edx                              # DL[7:4] = legacy socketID
      shrl  $4, %edx
      andl  $(BIT3+BIT2+BIT1+BIT0), %edx             # DL = legacy socketID

      movl  $S0_MMCFG_TARGET_LIST_GLOBAL, %eax
      cmpb  $0, %dl                                 # Legacy socketID = 0?
      je    gcs50                                   # Yes
      movl  $S1_MMCFG_TARGET_LIST_GLOBAL, %eax
      cmpb  $1, %dl                                 # Legacy socketID = 1?
      je    gcs50                                   # Yes
      movl  $S2_MMCFG_TARGET_LIST_GLOBAL, %eax
      cmpb  $2, %dl                                 # Legacy socketID = 2?
      je    gcs50                                   # Yes
      movl  $S3_MMCFG_TARGET_LIST_GLOBAL, %eax
      cmpb  $3, %dl                                 # Legacy socketID = 3?
      je    gcs50                                   # Yes
      movl  $S4_MMCFG_TARGET_LIST_GLOBAL, %eax
      cmpb  $4, %dl                                 # Legacy socketID = 4?
      je    gcs50                                   # Yes
      movl  $S5_MMCFG_TARGET_LIST_GLOBAL, %eax
      cmpb  $5, %dl                                 # Legacy socketID = 5?
      je    gcs50                                   # Yes
      movl  $S6_MMCFG_TARGET_LIST_GLOBAL, %eax
      cmpb  $6, %dl                                 # Legacy socketID = 6?
      je    gcs50                                   # Yes
      movl  $S7_MMCFG_TARGET_LIST_GLOBAL, %eax
#      cmp   dl, 7                                   ; Legacy socketID = 7?
#      je    gcs50                                   ; Yes

gcs50: 
# Map the first 32 buses (00h to 1Fh) to legacy socket
      orl   $BIT3, %eax                          # set Local bit
      movl  $CSR_MMCFG_TARGET_LIST_CBO, %ebx


      movl  %eax, %ds:(%edi,%ebx)
      movd  %eax, %xmm1                          # save it in xmm1

#------------------------------------------------------------------------------
# Setup the non-default BUS NO in Legacy Socket
#------------------------------------------------------------------------------
      #
      # Disable current MMCFG_RULE (at default bus#)
      #
      movl  $CSR_LEGACY_MMCFG_RULE_CBO, %eax
      READ_PCI_DWORD_LEGACY
      andl  $MMCFG_RULE_CLR_ENABLE, %ebx                 # set enable bit
      WRITE_PCI_DWORD_LEGACY


      #Initialize MMCFG_LOCAL_RULE_ADDRESS_CFG (at 1:29:1:c8, cc)
      #program local stack bus numbers in Cha
      movl  $CSR_LEGACY_MMCFG_LOCAL_RULE_ADDRESS0_CBO, %eax
      movl  $LEGACY_MMCFG_LOCAL_RULE_ADDR0, %ebx         # ebx = CSR data
      WRITE_PCI_DWORD_LEGACY

      movl  $CSR_LEGACY_MMCFG_LOCAL_RULE_ADDRESS1_CBO, %eax
      movl  $LEGACY_MMCFG_LOCAL_RULE_ADDR1, %ebx         # ebx = CSR data
      WRITE_PCI_DWORD_LEGACY

      #
      # Initialize and enable MMCFG_Rule register
      # a. init Cbo MMCFG_RULE  (at 1:29:1:C0)
      #
      movl  $(CSR_LEGACY_MMCFG_RULE_CBO + 4), %eax
      xorl  %ebx, %ebx
      WRITE_PCI_DWORD_LEGACY
      movl  $CSR_LEGACY_MMCFG_RULE_CBO, %eax
      READ_PCI_DWORD_LEGACY
      movl  $MMCFG_RULE_ADDR_MASK, %ecx
      notl  %ecx
      andl  %ecx, %ebx                                  # mask off base address
      andb  $0xf0, %bl                                  # mask off Length, keep Valid_Targets
      orl   $DEFAULT_MMCFG_RULE, %ebx                    # update base addr and length
      orl   $BIT0, %ebx                                  # set enable bit
      WRITE_PCI_DWORD_LEGACY

      movl  $LEGACY_COLDBOOT_CPUBUSNO, %eax              # BUS NO 0, 11h, 12h, 13h for PEI to Pre-KTI
      movd  %eax, %xmm4
      movl  $LEGACY_COLDBOOT_CPUBUSNO1, %eax             # BUS NO 14h, 15h  for PEI to Pre-KTI
      movd  %eax, %xmm5

#
# Program  UBOX CPUBUSNO/CPUBUSNO1 CSRs
#
      movl  $CSR_CPUBUSNO_UBOX, %ebx
      movd  %xmm4, %eax
      movl  %eax, %ds:(%edi,%ebx)

      movl  $CSR_CPUBUSNO1_UBOX, %ebx
      movl  %ds:(%edi,%ebx), %eax
      andl  $0xffff0000, %eax                   # mask fields for MCP stacks
      movd  %xmm5, %esi
      orl   %esi, %eax
      movl  %eax, %ds:(%edi,%ebx)

#
# Set CPUBUSNO_VALID.busno_programmed bit to 1
#
      movl  $CSR_CPUBUSNO_VALID_UBOX, %ebx
      movl  %ds:(%edi,%ebx), %eax
      orl   $BIT31, %eax                         # set busno_programmed = 1
      movl  %eax, %ds:(%edi,%ebx)

#
# Program default IOAPIC_TARGET_LIST
#
      movl  $CSR_IOAPIC_TARGET_LIST_0, %ebx
      movl  $DEFAULT_IOAPIC_TARGET_LIST, %eax
      BUSNO1_EBX
      movl  %eax, %ds:(%edi,%ebx)
      movl  $CSR_IOAPIC_TARGET_LIST_1, %ebx
      BUSNO1_EBX
      movl  %eax, %ds:(%edi,%ebx)
      movl  $CSR_IOAPIC_TARGET_LIST_2, %ebx
      BUSNO1_EBX
      movl  %eax, %ds:(%edi,%ebx)
      movl  $CSR_IOAPIC_TARGET_LIST_3, %ebx
      BUSNO1_EBX
      movl  %eax, %ds:(%edi,%ebx)

#-------------------------------------------------------------------------------
# From this point on, inter-socket CSR access is enabled
# Valid matching bus# must be used to access all CSRs.
#-------------------------------------------------------------------------------

eac_exit: 
     RET_ESI_MMX3                                 # return


#-------------------------------------------------------------------------
# CPU Early Workaround
#Executed by all package BSPs
# Input:
#    Stack available
# Output:
#    All general purpose registers destroyed
#-------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(ProcessorWorkaround)
ASM_PFX(ProcessorWorkaround):

  call  _OEMCpuSecInit
  ret


#-------------------------------------------------------------------------
# Read a Dword from PCI register 00h thru FFh using IO port CF8h/CFCh
# Input:
#   Stack is avaialble
#   xmm registers initialized.
#   xmm0 register bit [7:4]  contains Legacy Socket ID
# Output: AL = SBSP Socket ID
# Registers changed: None except eax
#-------------------------------------------------------------------------
GetSbspSktId:
        movd %xmm0, %eax                # AL[7:4] = SBSP socket id
        shrl $4, %eax
        andl $(BIT3+BIT2+BIT1+BIT0), %eax
        ret

#-------------------------------------------------------------------------
# Returns Socket/Packge ID of the current processor
# Input:
#   Stack is avaialble
#   xmm registers initialized
#   xmm0 register bit[3:0] contains local Socket ID
# Output: CL = Socket/Package ID of the current processor
# Registers changed: None except ecx
#-------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(GetLocalSktId)
ASM_PFX(GetLocalSktId):

     movd  %xmm0, %ecx                                     # xmm0[3:0] stores the socketID
     andl  $(BIT3+BIT2+BIT1+BIT0), %ecx                    # cl = local socket ID
     ret


#----------------------------------------------------------------------------
# SetupProcessorCheckinData
#  Executed by all socket BSPs.  Store data in its own local scratchpad register:
#       bit[0]        - check in flag
#       bit[23:8]   - ApicId  (only 16-bit is used)
#       bit[30:27] - stepping
#  Input:  Stack available
#       Local MMCFG config CSR  access is enabled
#  Output: All general puropse registers preserved.
#----------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(SetupProcessorCheckinData)
ASM_PFX(SetupProcessorCheckinData):

        pushal

        movl   $1, %eax
        cpuid                                               # ebx[31:24] = initial APIC ID, eax[3:0] = stepping
        andl   $0xff000000, %ebx
        shrl   $16, %ebx                                    # APIC ID in ebx[23:8]
        andl   $0xf, %eax
        shll   $27, %eax                                    # eax[30:27] = stepping
        movl   %eax, %esi                                   # esi [30:27] = setpping
        pushl  %ebx                                         # save ebx

        # check to see if processor is x2Apic capable
        movl   $0, %eax
        cpuid
        popl   %ebx                                         # restore ebx
        cmpl   $0xb, %eax                                   # check to see if processor is x2Apic capable
        jb     SaveDataAndSetCheckIn                        # processor is not x2APIC capable

        # processor is x2Apic capable, get ApicId from cpuid leaf B
        movl   $0xb, %eax
        xorl   %ecx, %ecx
        cpuid
        movzwl %dx, %ebx                                    # though X2Apid is 32bit wide,
        shll   $8, %ebx                                     # HW support up to 12 bits (bit31:12=0)

SaveDataAndsetCheckIn: 
        # ebx [23:8] = Apic ID,  esi [30:27] = stepping
        # call   GetLocalSktId                                 ; CL = own local socket ID
        movl   $SR_PBSP_CHECKIN_CSR, %eax
        call   ReadCpuCsr                                    # EAX = current CSR data
        andl   $0x870000ff, %eax                             # clear bit[30:27], [23:8]
        orl    %eax, %ebx                                    #  ebx <- apci id
        orl    %esi, %ebx                                    #  ebx <- stepping
        orl    $1, %ebx                                      # set check in bit
        movl   $SR_PBSP_CHECKIN_CSR, %eax
        call   WriteCpuCsr

        popal
        ret



#-------------------------------------------------------------------------
# UncoreBusCfgSyncNbsp
#   A PBSP waits for and hand-shakes with SBSP to receive re-programming of
#   MMCFG/CPUBUSNO related registers
#
# Input:
#     Stack is available
#     Following xmm registers hold MMCFG and bus number config info
#                xmm0 = [3:0]  local socket id
#                             [7:4]  legact socket id
#                             [31] = Warm Reset Path flag
#                xmm1 = MCFG_TARGET_LIST CSR current data
#                xmm3 = MMCFG_RULE CSR current data
#                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
#                xmm5 = CPUBUSNO [Bus4,Bus5]
#
# Output:
#    PBSP has switched to the new MMCFG/CPUBUSNO configuration with global view of the MMCFG config space
#
# Registers changed: None
#-------------------------------------------------------------------------


ASM_GLOBAL ASM_PFX(UncoreBusCfgSyncNbsp)
ASM_PFX(UncoreBusCfgSyncNbsp):

        pushal

#
#  At this point, all CSR accesses are local to the socket, and default bus# 0...5 are used for local targets
#

#
#  Disable CHA MMCFG_RULE
#
      movl  $CSR_LEGACY_MMCFG_RULE_CBO, %eax                # eax = reg address
      call  ReadPciDwordLegacy                             # ebx = reg dword data
      btrl  $0, %ebx                                       # clear Enable bit
      call  WritePciDwordLegacy                            # write to reg
      movd  %ebx, %xmm3                                     # update copy in xmm3
#
#  From this point, only CF8/CFC IO can be used for CSR access
#
#
#  Set flag in local scratchpad CSR to indicate to SBSP "I'm ready for MMCFG/BUSNO re-programming"
#
      movl  $SR_LEGACY_BUSCFG_SYNC_CSR, %eax                 # eax = reg address
      movl  $BusCfgSync_PBSP_Ready, %ebx                     # ebx = reg dword data
      call  WritePciDwordLegacy                             # write to reg

PbspWait10: 
#
#  Loop here on local scratchpad CSR
#  waiting for local MMCFG/CPUBUSNO etc.  CSRs be updated by SBSP,
#  until notified by SBSP to enable new configuration
#
      movl  $SR_LEGACY_BUSCFG_SYNC_CSR, %eax                # eax = reg address
      call  ReadPciDwordLegacy                             # ebx = reg dword data
      cmpl  $BusCfgSync_PBSP_Go, %ebx
      jne   PbspWait10
#
#  At this point, some local CSRs have been updated by SBSP with new settings
#  Update local copy of the new CHA MMCFG_RULE, UBOX CPUBUSNO (in xmm registers)
#
      movl  $CSR_LEGACY_MMCFG_TARGET_LIST_CBO, %eax         # eax = reg address
      call  ReadPciDwordLegacy                             # ebx = reg dword data
      movd  %ebx, %xmm1                                     # update xmm reg with new data
#
# Get local copy of mmcfg base to support multisegments
#
      movl  $SR_LEGACY_BUSCFG_DATA_CSR, %eax
      call  ReadPciDwordLegacy                             # ebx = reg dword data
      orl   $BIT0, %ebx
      movd  %ebx, %xmm3                                     # update xmm reg with new data
      movl  $CSR_LEGACY_CPUBUSNO_UBOX, %eax
      call  ReadPciDwordLegacy                             # ebx = reg dword data
      movd  %ebx, %xmm4                                     # update xmm reg with new data
      movl  $CSR_LEGACY_CPUBUSNO1_UBOX, %eax
      call  ReadPciDwordLegacy                             # ebx = reg dword data
      movd  %ebx, %xmm5                                     # update xmm reg with new data

#
#  Enable UBOX CPUBUSNO_VALID
#
      movl  $CSR_LEGACY_CPUBUSNO_VALID_UBOX, %eax           # eax = reg address
      call  ReadPciDwordLegacy                             # ebx = reg dword data
      orl   $BIT31, %ebx                                    # set Valid bit = 1
      call  WritePciDwordLegacy                            # write to reg
#
#  From this point on, new CPUBUSNO must be used
#  Get new bus0/bus1 value
#
      movd  %xmm4, %ecx
      movl  %ecx, %edx

      andl  $0x00000ff, %ecx                               # bit[7:0] = bus0 field
      shll  $16, %ecx                                      # bit[23:16] = bus0

      andl  $0x000ff00, %edx                               # bit[15:8] = bus1 field
      shll  $8, %edx                                       # bit[23:16] = bus1
#
#  ecx[23:16] = new bus# for local target 0 (Bus0, UBOX)
#  edx[23:16] = new bus# for local target 1 (Bus1, CHA)
#

#
#  Enable  MMCFG_RULE in CHA, on Bus1
#
      movl  $CSR_LEGACY_MMCFG_RULE_CBO, %eax                 # eax = reg address
      andl  $0xff00ffff, %eax                               # clear bus# field
      orl   %edx, %eax                                      # update bus# field with new Bus1
      call  ReadPciDwordLegacy                              # ebx = reg dword data
      orl   $BIT0, %ebx                                      # set Enable bit = 1
      call  WritePciDwordLegacy                             # write to reg
#
# At this point, new CPUBUSNO and MMCFG_RULE are all enabled on the PBSP socket
#

#
#  Enable UBOX copy of MMCFG_RULE on Bus0
#
      movl  $CSR_MMCFG_RULE_UBOX, %eax                       # eax = reg address
      #and   eax, 0ff00ffffh                                 ; clear bus# field
      #or    eax, ecx                                        ; update bus# field with new Bus0
      call  ReadCpuCsr                                      # eax = reg data
      movl  %eax, %ebx
      orl   $BIT0, %ebx                                      # set  Enable bit = 1
      movl  $CSR_MMCFG_RULE_UBOX, %eax                       # eax = reg address
      call  WriteCpuCsr                                     # write to reg

#
#  Send message back to SBSP to indicate "I'm done with enabling the new MMCFG/CPUBUS config"
#  via local scratchpad CSR in UBOX on Bus0
#
      movl  $SR_BUSCFG_SYNC_CSR, %eax                        # eax = reg address
      #and   eax, 0ff00ffffh                                 ; clear bus# field
      #or    eax, ecx                                        ; update bus# field with new Bus0
      movl  $BusCfgSync_PBSP_Done, %ebx                      # ebx = reg dword data
      call  WriteCpuCsr                                     # write to reg
#
#  Wait for SBSP to receive/process the message and clear the scartchpad CSR as acknoledgement
#
PbspWait50: 
      movl  $SR_BUSCFG_SYNC_CSR, %eax                        # eax = reg address
      call  ReadCpuCsr                                      # eax = reg data
      cmpl  $BusCfgSync_PBSP_Done, %eax
      je    PbspWait50                                      # loop until not equal

      popal
      ret


#----------------------------------------------------------------------------
# EnableAccessGlobalCSR
#  Executed by SBSP (Legacy socket) only
#  Input:  Stack available
#             Local MMCFG config CSR  access is enabled
#  Output:
#             MMCFG_TARGET_LIST updated
#             CPUBUSNO enabled
#             Inter-socket CSR accesses enabled
#
#  All general puropse registers preserved.
#----------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(EnableAccessGlobalCSR)
ASM_PFX(EnableAccessGlobalCSR):

       pushal

       CALL_MMX3 EnableAccessGlobalCSR_NoStack

       popal
       ret


#-------------------------------------------------------------------------
# Non-SBSPs execute this routine to clear BSP flag in MSR and
# loop on a local scratch pad register for command from SBSP
# Input: None
#
# Output: None
#-------------------------------------------------------------------------

ParkNbsp:

     # clear bit8 BSP flag in MSR
     movl    $MSR_APIC_BASE, %ecx
     rdmsr
     andl    $(~BIT8), %eax
     wrmsr

     call  GetLocalSktId                                   # CL = Local Socketr ID

   # When SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR becomes non-zero,
   #       SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR will be the jmp location

SitAndSpin: 
     movl  $SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, %eax  # Pipe Dispatch register in local socket
     call  ReadCpuCsr

     cmpl  $0, %eax
     jz    SitAndSpin

     movl  $SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR, %eax  # Pipe Command register in local socket
     call  ReadCpuCsr

     cmpl  $PIPE_DISPATCH_SYNCH_PSYSHOST, %eax
     jz    GoToAction

     # clear out dispatch register because command is invalid
     #mov   eax, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR   ; Pipe Dispatch register in local socket
     #xor   ebx, ebx                                        ; Clear the dispactch CSR
     #call  WriteCpuCsr
     jmp   SitAndSpin

GoToAction: 

     movl  $SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, %eax      # Pipe Data register in local socket
     call  ReadCpuCsr
     movl  %eax, %esp                                      # sync up stack pointer with SBSP

     movl  $SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, %eax  # Pipe Dispatch register in local socket
     xorl  %ebx, %ebx                                      # Clear the dispactch CSR
     call  WriteCpuCsr

WaitForJmpPointer: 
     movl  $SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, %eax  # Pipe Dispatch register in local socket
     call  ReadCpuCsr

     cmpl  $0, %eax
     jz    WaitForJmpPointer

     movl $SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, %eax
     call GetBusNum                                        # update EAX[27:20] with the correct bus #
     call GetMmcfgBase                                     # esi = MMCFG Base address
     addl %eax, %esi                                       # form cfg register address in esi
     andl $0xfffff000, %esi                                # keep only Bus:Dev:Func portion of it

     movl  $SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, %eax      # Pipe Data register in local socket
     call  ReadCpuCsr

   # EAX = Function pointer to PipeSlaveInit( UINT32 NodeId, UINT32 CfgAddrBDF)
   # Emulate a C-style function call
     pushl  %esi                                            # push input arg (PCIE Config Address w/o register offset)
     pushl  %ecx                                            # push input arg (local socketId)
     call   *%eax                                             # call PipeSlaveInit( UINT32 NodeId, UINT32 CfgAddrBDF)
                                                            # Note: Control will not return to here
#-------------------------------------------------------------------------
# Check if warm reset path.  Can be called by each socket BSP
# Input:
#   Stack is NOT available
#   MMX6 = return address via RET_ESI_MMX6 macro
#   Register xmm0[31] has been initialized to indicate warm reset
#          xmm0  [31] = 0   Power Good reset path
#                               1   Warm reset path
#
# Output:
#              ZF = 1      Cold reset path
#              ZF = 0      Warm reset path
#
# Registers changed:  ESI
#-------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(IsWarmResetNoStack)
ASM_PFX(IsWarmResetNoStack):

      movd  %xmm0, %esi
      testl $BIT31, %esi
     #  ZF = 1      Cold reset path
     #  ZF = 0      Warm reset path

      RET_ESI_MMX6


#-------------------------------------------------------------------------
# Check if I am legacy socket. Can be called by each socket BSP
# Input:
#   Stack is NOT available
#   MMX6 = return address via RET_ESI_MMX6 macro
#   xmm0 registers initialized and contains socket id info:
#          xmm0  [3:0]= Local socket id,
#                     [7:4]= Legacy socket id
#                     [30] = Legacy Socket flag
#
# Output:  ZF = 1       Yes, I am legacy socket
#              ZF = 0      No
#
# Registers changed: esi
#-------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(AmILegacySocketNoStack)
ASM_PFX(AmILegacySocketNoStack):

      movd  %xmm0, %esi
      andl  $BIT30, %esi             #  [30] = Legacy Socket flag
      xorl  $BIT30, %esi             #  am I legacy socket?    ZF = 1/0  -> Yes/No

      RET_ESI_MMX6


#-------------------------------------------------------------------------
# Callable version of  AmILegacySocketNoStack()
# Check if I am legacy socket. Can be called by all sockets
# Input:
#              Stack is available
# Output:
#              ZF = 1       Yes, I am legacy socket
#              ZF = 0       No
#
# Registers changed: none
#-------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(AmILegacySocket)
ASM_PFX(AmILegacySocket):

      pushl %esi
      CALL_MMX6  AmILegacySocketNoStack
      popl  %esi
      ret


#-------------------------------------------------------------------------
# Get MMCFG.BASE
# Input:
#     Stack is NOT available
#     MMX6 = return address via RET_ESI_MMX6 macro
#     CL = Local target BusID (0,1,...5)
#     Following xmm registers hold MMCFG and bus number config info
#                xmm0 = [3:0]  local socket id
#                             [7:4]  legact socket id
#                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
#                             [31] = Warm Reset Path flag
#                xmm1 = MCFG_TARGET_LIST CSR current data
#                xmm3 = MMCFG_RULE CSR current data
#                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
#                xmm5 = CPUBUSNO [Bus4,Bus5]
#
# Output: EDI = MMCFG.base
#         The caller can then use
#            mov ebx, CSR_DEV_FUNC_OFFSET
#            mov eax, dword ptr [edi] [ebx]       where ebx = bus/dev/func/offset in PCIe format
#
# Registers changed: edi, esi
#-------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(GetMmcfgNoStack)
ASM_PFX(GetMmcfgNoStack):

      movd  %xmm3, %edi
      testl $BIT0, %edi                   # MMCFG.enable already set?
      jnz   gmc_10                       # yes

gmc_error: 
      PORT80(MMCFG_NOT_YET_ENABLED_ERROR_CODE)                                # error code to port 80h
      jmp  gmc_error                     # dead loop

gmc_10: 
      andl  $MMCFG_RULE_ADDR_MASK, %edi   # edi = MMCFG.base

      RET_ESI_MMX6


#-------------------------------------------------------------------------
# Get MMCFG.BASE + Bus# field
# Input:
#   Stack is NOT available
#   MMX6 = return address via RET_ESI_MMX6 macro
#   CL = Local target BusID (0,1,...5)
#     Following xmm registers hold MMCFG and bus number config info
#                xmm0 = [3:0]  local socket id
#                             [7:4]  legact socket id
#                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
#                             [31] = Warm Reset Path flag
#                xmm1 = MCFG_TARGET_LIST CSR current data
#                xmm3 = MMCFG_RULE CSR current data
#                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
#                xmm5 = CPUBUSNO [Bus4,Bus5]
#
# Output: EDI = MMCFG.base + bus offset
#         The caller can then use
#            mov ebx, CSR_DEV_FUNC_OFFSET
#            mov eax, dword ptr [edi] [ebx]
#
# Registers changed: cl, edi, esi
#-------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(GetMmcfgBus_NoStack)
ASM_PFX(GetMmcfgBus_NoStack):

      movd  %xmm3, %edi
      testl $BIT0, %edi                  # MMCFG.enable already set?
      jnz   gmb_10                      # yes

gmb_error: 
      PORT80(MMCFG_NOT_YET_ENABLED_ERROR_CODE)                                 # error code to port 80h
      jmp  gmb_error                    # dead loop

gmb_10: 
      movd  %xmm4, %esi                  # esi = CPUBUSNO[3,2,1,0]
      cmpb  $4, %cl
      jb    gmb_20
      movd  %xmm5, %esi                  # esi = CPUBUSNO[_,_, 5,4]
      subb  $4, %cl                     # adjust BusID

gmb_20: 
      shlb  $2, %cl                     # cl = cl * 4
      shr   %cl, %esi                   # esi[7:0] = target bus#
      andl  $0xff, %esi
      shll  $20, %esi                   # esi[27:20] = target bus#

      andl  $MMCFG_RULE_ADDR_MASK, %edi  # edi = MMCFG.base
      addl  %esi, %edi                  # edi = MMCFG.base + bus offset

      RET_ESI_MMX6



#-------------------------------------------------------------------------
# Read a Dword from PCIe config register using mmcfg method
# Limitation:  Currently does not support config space size > 256 MB ( 256 buses)
# Input:
#   EAX = PCIe config address format with format defined as follows
#     EAX[31:28] = 0h
#     EAX[27:20] = Bus# of local target (Bus0,Bus1,...Bus5)
#     EAX[19:15] = Dev #
#     EAX[14:12] = Func #
#     EAX[11:0]  = Reg offset (dword aligned)
#     Following xmm registers hold MMCFG and bus number config info
#                xmm0 = [3:0]  local socket id
#                             [7:4]  legact socket id
#                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
#                             [31] = Warm Reset Path flag
#                xmm1 = MCFG_TARGET_LIST CSR current data
#                xmm3 = MMCFG_RULE CSR current data
#                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
#                xmm5 = CPUBUSNO [Bus4,Bus5]
#
# Output: EAX = regiser dword data
# Registers changed: None except EAX
#-------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(ReadCpuCsr)
ASM_PFX(ReadCpuCsr):

      pushl %esi
      call  GetBusNum                                    # update EAX[27:20] with the correct bus# for the socket ID
      call  GetMmcfgBase                                 # esi = MMCFG Base Address
      addl  %eax, %esi                                   # form cfg register full address in esi
      movl  %ds:(%esi), %eax                             # read dword
      popl  %esi
      ret



#-------------------------------------------------------------------------
# Write a Dword from PCIe config register using mmcfg method
# Limitation:  Currently does not support config space size > 256 MB ( 256 buses)
#
# Input:
#   Stack is available
#   EAX = PCIe config address format with format defined as follows
#     EAX[31:28] = 0h
#     EAX[27:20] = Bus# of local target (Bus0,Bus1,...Bus5)
#     EAX[19:15] = Dev #
#     EAX[14:12] = Func #
#     EAX[11:0]  = Reg offset (dword aligned)
#     EBX  = Dword data to be written to reg
#     Following xmm registers hold MMCFG and bus number config info
#                xmm0 = [3:0]  local socket id
#                             [7:4]  legact socket id
#                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
#                             [31] = Warm Reset Path flag
#                xmm1 = MCFG_TARGET_LIST CSR current data
#                xmm3 = MMCFG_RULE CSR current data
#                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
#                xmm5 = CPUBUSNO [Bus4,Bus5]
#
# Output: None
# Registers changed: None
#-------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(WriteCpuCsr)
ASM_PFX(WriteCpuCsr):

      pushl %eax
      pushl %esi
      call  GetBusNum                                    # update EAX[27:20] with the correct bus# for the socket ID
      call  GetMmcfgBase                                 # esi = MMCFG Base Address
      addl  %eax, %esi                                   # form cfg register full address in esi
      movl  %ebx, %ds:(%esi)                             # write dword
      popl  %esi
      popl  %eax
      ret


#-------------------------------------------------------------------------
#  Update EAX[27:20] with the target Bus number from MSR 300h
# Input:
#   EAX = PCIe config address format with format defined as follows
#     EAX[31:28] = 0h
#     EAX[27:20] = Bus# of local target (Bus0,Bus1,...Bus5)
#     EAX[19:15] = Dev #
#     EAX[14:12] = Func #
#     EAX[11:0]  = Reg offset (dword aligned)
#     Following xmm registers hold MMCFG and bus number config info
#                xmm0 = [3:0]  local socket id
#                             [7:4]  legact socket id
#                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
#                             [31] = Warm Reset Path flag
#                xmm1 = MCFG_TARGET_LIST CSR current data
#                xmm3 = MMCFG_RULE CSR current data
#                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
#                xmm5 = CPUBUSNO [Bus4,Bus5]
#
# Output: EAX[27:20] updated with the actual bus# intended
# Registers changed: None except EAX[27:20]
#-------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(GetBusNum)
ASM_PFX(GetBusNum):

      pushl %ebx
      pushl %ecx
      pushl %edx
      pushl %esi
      pushl %edi

      pushl %eax                      # save input eax
      pushl %ecx                      # save input ecx
      movl  $0x300, %ecx
      rdmsr                           # edx:eax  = current CPUBUSNO/CPUBUSNO1
      testl $BIT31, %edx               # Bus number valid?
      jnz   gbn10
      # Bus Number is not valid, force default bus# that is decoded by UBOX
      # If legacy Socket, BusNo: 0, 0x11, 0x12, 0x13, 0x14, 0x15
      # If remote socket, BusNo: 0, 1,2,3,4,5

      call  AmILegacySocket
      jz    gbn5
      movl  $LEGACY_COLDBOOT_CPUBUSNO, %eax
      movl  $LEGACY_COLDBOOT_CPUBUSNO1, %edx
      jmp   gbn6
gbn5: 
      movl  $DEFAULT_COLDBOOT_CPUBUSNO, %eax
      movl  $DEFAULT_COLDBOOT_CPUBUSNO1, %edx
gbn6: 



 gbn10: 
      movl  %eax, %esi
      movl  %edx, %edi
      popl  %ecx                      # restore input ecx
      popl  %eax                      # restore input eax


# Bus# byte-array
#              edi                                            esi
# ---------------------    --------------------------
# [Rsvd, Rsvd, Bus5, Bus4]       [Bus3, Bus2, Bus1, Bus0]

# get Bus# field of PCIe address in EAX
      movl  %eax, %ecx
      shrl  $20, %ecx
      andl  $0xff, %ecx                  #  [7:0] = bus#

      cmpl  $4, %ecx                     # BusID >=4?
      jb    gbn20
      subl  $4, %ecx                     # adjust BusID
      movl  %edi, %esi                   # adjust Bus array

# esi =  proper Bus# byte-array
# ecx = index into Bus# byte-array (value = 0,1,2 or 3)

# shift proper bus#  into esi[27:20]
 gbn20: 
      shll  $3, %ecx                     # ecx = ecx * 8,  bit position of bus# field
      shr   %cl, %esi                    # esi[7:0] = target bus#
      andl  $0xff, %esi                  # esi[7:0] = bus#
      shll  $20, %esi                    # esi[27:20] = bus#

      andl  $0x00fffff, %eax             # keep only Dev:Func:Offset
      orl   %esi, %eax                   # eax = Bus:Dev:Func:Offset

      popl  %edi
      popl  %esi
      popl  %edx
      popl  %ecx
      popl  %ebx
      ret


#-------------------------------------------------------------------------
# Get MMCFG base address
#
#  Input:
#   Stack is avaialble
#   xmm3 register has been initialized and contains MMCFG_RULE
#
#  Output: ESI = 32-bit MMCFG base address
#  Registers changed:  none except ESI
#-------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(GetMmcfgBase)
ASM_PFX(GetMmcfgBase):

      # PCIe config base address actually written into the MMCFG_RULE CSR
      movd  %xmm3, %esi                                      # xmm3 stores MMCFG_RULE
      andl  $MMCFG_RULE_ADDR_MASK, %esi                      # make sure the lower bits are 0s
      ret


#------------------------------------------------------------------------------
# Read a PCI config dword via lagacy CF8/CFC method
# Stack is available
# input: EAX = legacy PCI address format with Bus:Dev:Func:Offset
# output: EBX = dword register data
#
# Registers changed:  EBX
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(ReadPciDwordLegacy)
ASM_PFX(ReadPciDwordLegacy):

   pushl %eax
   pushl %edx
   orl   $BIT31, %eax                 # make sure bit31 is set
   READ_PCI_DWORD_LEGACY
   popl  %edx
   popl  %eax
   ret


#------------------------------------------------------------------------------
# Write a DWord data to PCI config reg via lagacy CF8/CFC method
# Stack is available
# input: EAX = legacy PCI address format with Bus:Dev:Func:Offset
#          EBX = dword data to write
#
# Registers changed: None
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(WritePciDwordLegacy)
ASM_PFX(WritePciDwordLegacy):

   pushl %eax
   pushl %edx
   orl   $BIT31, %eax                 # make sure bit31 is set
   WRITE_PCI_DWORD_LEGACY
   popl  %edx
   popl  %eax
   ret
