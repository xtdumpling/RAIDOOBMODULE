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
;  ProcessorStartupUncore.asm
;
; Abstract:
;
;
;------------------------------------------------------------------------------
        .586P
        .XMM
IFDEF MINIBIOS_BUILD
        .MODEL  SMALL
ELSE
        .MODEL  SMALL, c
ENDIF



        include ProcessorStartupPlatform.inc
        include ProcessorStartupChipset.inc
        include ProcessorStartupUncore.inc
        include ProcessorStartup.inc

        EXTERN  ReadPciLegacy:NEAR32
        EXTERN  WritePciLegacy:NEAR32
        EXTERN  _OEMCpuSecInit:NEAR32

;----------------------------------------------------------------------------
;       STARTUP_SEG  S E G M E N T  STARTS
;----------------------------------------------------------------------------
STARTUP_SEG SEGMENT PARA PUBLIC 'CODE' USE32

;----------------------------------------------------------------------------
; Executed by all socket BSPs present in the system early after reset
; - Enable MMCFG_RULE on local socket
; - Set IO, MMIO target lists  to point to legacy socket
; - Init MMIO_RULE11 to cover the 64MB below 4GB for NEM DATA region
; -
;  Input:  All general puropse registers undefined
;          Stack may not be available
;          MMX4 = routine return address
;          MMCFG_Rule is enabled on local socket
;
;  Output: All general puropse registers undefined.
;          MMCFG and some related CSRs are initialized to allow CSR accesses on local socket via MMCFG method
;          Following xmm registers hold MMCFG and bus number config scheme info (later used by otehr routines in this file)
;                xmm0 = [3:0]  local socket id
;                             [7:4]  legact socket id
;                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
;                             [29:28]  rsvd
;                             [30]    Legacy socket flag
;                             [31] = Warm Reset Path flag
;                xmm1 = MCFG_TARGET_LIST CSR current data
;                xmm3 = MMCFG_RULE CSR current data
;                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
;                xmm5 = CPUBUSNO [Bus4,Bus5]
;----------------------------------------------------------------------------
EarlyUncoreInit    PROC    NEAR    PUBLIC

      ;
      ; All sockets come here after reset
      ;
      ; In CR4, set OSFXSR bit 9 and OSxmmEXCPT bit 10 to enable access to xmm registers used by this routine
      ;
      mov   eax, cr4
      or    eax, (BIT10+BIT9)
      mov   cr4, eax

      ; Use pre-defined scheme for temp config
      mov   eax, DEFAULT_COLDBOOT_CPUBUSNO
      movd  xmm4, eax
      mov   eax, DEFAULT_COLDBOOT_CPUBUSNO1
      movd  xmm5, eax

      ; xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
      ; xmm5 = CPUBUSNO [Bus4,Bus5]

      ;
      ; At this point, only CF8/CFC IO port can be used for PCI config access
      ;

      ;
      ; Disable LOCK lockcontrol.lockdisable  at 0:8:0:0x50, bit0
      ; TODO:    bit0 is set by default already?
      ;
      ;mov   eax, CSR_LEGACY_LOCKCONTROL
      ;READ_PCI_DWORD_LEGACY                             ; ebx = reg data
      ;or    ebx, BIT0                                   ; set bit0
      ;WRITE_PCI_DWORD_LEGACY

      ;
      ; Read local socket ID and legacy socket ID, and store them in xmm0
      ;
      mov   eax, CSR_LEGACY_SAD_CONTROL_CBO             ; get local  socket ID
      READ_PCI_DWORD_LEGACY                             ; ebx = reg data
      mov   ecx, ebx
      and   ecx, (BIT2+BIT1+BIT0)                       ; keep bit[2:0] local socket ID, ignore bit3

      mov   eax, CSR_LEGACY_SAD_TARGET_CBO              ; get legacy socket ID
      READ_PCI_DWORD_LEGACY                             ; ebx = reg data
      and   ebx, (BIT7+BIT6+BIT5+BIT4)                  ; keep bit[7:4] legacy_pch_target
      btr   ebx, 7                                      ; test & clear bit7  (local/remote?)
      jnc   eui_02                                      ; remote socket ID
      ;
      ; For legacy socket,  use local socket ID as legacy socket ID, and set legacy socket flag bit
      ;
      mov   ebx, ecx
      shl   ebx, 4                                      ; bit[7:4] = Legacy socket id
      or    ebx, BIT30                                  ; bit[30] = Legacy Socket flag
eui_02:
      or    ecx, ebx
      movd  xmm0, ecx                                   ; xmm0[7:4] = legacy socket id, [3:0]= local socket id

      ;
      ; Get CPU Faimly/stepping info and store it in xmm0[27:8]
      ;
      mov  eax, 1
      cpuid
      and  eax, 0fffffh                                 ; keep Falily/Stepping info
      shl  eax, 8                                       ; shift to bit[27:8]
      movd ebx, xmm0
      or   ebx, eax
      movd xmm0, ebx

      ;
      ; Read warm reset flag from sticky scratch pad reg and save it in xmm0
      ;
      mov   eax, SSR_LEGACY_BOOT_CONFIG_INFO_CSR
      READ_PCI_DWORD_LEGACY                             ; ebx = reg data
      test  ebx, BIT5                                   ; Warm Reset Path (Cold Reset Flow Completed = 1)?
      jnz   eui_warmboot                                ; Yes
      test  ebx, BIT4                                   ; Cold Reset Flow Entered flag = 1?
      jz    eui_coldboot                                ; No (normal cold boot path)

      ;
      ; Warm reset occured before completion of MRC on the cold boot path.
      ; In this case SBSP triggers a power-good reset while other socket BSPs waits
      ;

      CALL_MMX6 AmILegacySocketNoStack                  ; am I legacy socket?
      jnz   eui_06                                      ; no ,skip
      ; Legacy BSP comes here - Triggers a power-good reset.
      mov   al, 0eh
      mov   dx, 0cf9h
      out   dx, al
eui_06:
;TODO:
;  comment this out until PEI code has proper handling of BIT4/BIT5 flags
;      jmp   $                                           ; wait for reset to occur


eui_coldboot:
      or    ebx, BIT4                                   ; set Cold Boot Flow Entered flag = 1
      WRITE_PCI_DWORD_LEGACY                            ; write data to CSR
      jmp   eui_20

eui_warmboot:                                           ; Warm reset path
      ;and   ebx, NOT (BIT4)                             ; clear bit4 of scratch pad register
      btr   ebx, 4                                      ; clear bit4 of scratch pad register
      WRITE_PCI_DWORD_LEGACY                            ; write data to CSR

      movd  ebx, xmm0
      or    ebx, BIT31                                  ; Set warm reset path flag in xmm reg
      movd  xmm0, ebx

eui_20:
      ;
      ; Initialize  MMCFG_TARGET_LIST (at 1:29:1:ec)  to target local socket
      ;

      mov   eax, CSR_LEGACY_MMCFG_TARGET_LIST_CBO
      READ_PCI_DWORD_LEGACY                             ; ebx = default value of  MMCFG_TARGET_LIST CSR
      or    ebx, 88888888h                              ; make sure that all entries are marked as "local"
      WRITE_PCI_DWORD_LEGACY                            ; write it back
      movd  xmm1, ebx                                   ; save it in xmm1
      ;
      ; Initialize  CSR_LEGACY_MMCFG_LOCAL_RULE_CBO (at 1:29:1:e4)
      ;    ModeX=0 means  "send devices 16-31 at the root bus to the stack rather than to ubox"
      ;    ModeX=1 means  "send everything to the ubox"
      ;    For legacy socket, set Mode=0, and set all the others to ModeX=1
      ;
      mov  eax, CSR_LEGACY_MMCFG_LOCAL_RULE_CBO
      mov  ebx, DEFAULT_MMCFG_LOCAL_RULE
      movd ecx, xmm0
      test ecx, BIT30                                   ; Am I Legacy Socket?
      jz   eui_22                                       ; No, skip
      btr  ebx, 24                                      ; For legacy socket, set Mode0 = 0
eui_22:
      WRITE_PCI_DWORD_LEGACY

      ;Initialize MMCFG_LOCAL_RULE_ADDRESS_CFG (at 1:29:1:c8, cc)
      ;program local stack bus numbers in Cha
      mov   eax, CSR_LEGACY_MMCFG_LOCAL_RULE_ADDRESS0_CBO
      mov   ebx, DEFAULT_MMCFG_LOCAL_RULE_ADDRESS0      ; ebx = CSR data
      WRITE_PCI_DWORD_LEGACY

      mov   eax, CSR_LEGACY_MMCFG_LOCAL_RULE_ADDRESS1_CBO
      mov   ebx, DEFAULT_MMCFG_LOCAL_RULE_ADDRESS1      ; ebx = CSR data
      WRITE_PCI_DWORD_LEGACY

      ;
      ; Initialize and enable MMCFG_Rule register
      ; a. init Cbo MMCFG_RULE  (at 1:29:1:C0)
      ;
      mov   eax, CSR_LEGACY_MMCFG_RULE_CBO + 4          ; clear MMCFG high address
      xor   ebx, ebx
      WRITE_PCI_DWORD_LEGACY
      mov   eax, CSR_LEGACY_MMCFG_RULE_CBO
      READ_PCI_DWORD_LEGACY                             ; ebx = reg data
      mov   ecx, MMCFG_RULE_ADDR_MASK
      not   ecx
      and   ebx, ecx                                    ; mask off base address
      and   bl, 0f0h                                    ; mask off Length, keep Valid_Targets
      or    ebx, DEFAULT_MMCFG_RULE                     ; update base addr and length
      ;;or    ebx, 0ffh SHL 6                                                        ; set valid bit for all local targets (default)
      or    ebx, BIT0                                   ; set enable bit
      WRITE_PCI_DWORD_LEGACY
      movd  xmm3, ebx                                   ; save current reg value in xmm3

; ----------------------------------------------------------------------------------------------
; From this point on, local CSRs can be accessed at default bus numbers via MMCFG method
; Since MMCFG_TARGET_LIST points to local socket only, all CSR access are local
; ----------------------------------------------------------------------------------------------

      ;
      ; b. init Ubox MMCFG_RULE (at 0:8:2:dc)
      ; Need to convert the mmcfg length field between the two registers
      ; CBO_CR_MMCFG_RULE_CFG.Length
      ;    3'b110 : MaxBusNumber = 63 (ie. 64MB MMCFG range)
      ;    3'b101 : MaxBusNumber = 127 (ie 128MB MMCFG range)
      ;    3'b100 : MaxBusNumber = 256 (ie 256MB MMCFG range)
      ;    3'b010 : MaxBusNumber = 512 (ie 512MB MMCFG range)
      ;    3'b001 : MaxBusNumber = 1048 (ie 1G MMCFG range)
      ;    3'b000 : MaxBusNumber = 2048 (ie 2G MMCFG range)
      ;
      ; UBOX NCDECS_CR_MMCFG_RULE_CFG_REG.length
      ;     2'b10 : MaxBusNumber = 63 (ie 64MB MMCFG range)
      ;     2'b01 : MaxBusNumber = 127 (ie 128MB MMCFG range)
      ;     2'b00 : MaxBusNunber >= 256 (ie 256MB MMCFG range)

      movd  edx, xmm3                                   ; edx = current MMCFG_RULE_CFG csr data
      and   edx, (BIT3+BIT2+BIT1+BIT0)                  ; keep mmcfgRule bus length and enable bits [3:0]
      btr   edx, 3                                      ; test and clear bit3
      jc    eui_30                                      ; length <= 256 buses
      and   edx, BIT0                                   ; clear length to 2b'00 if > 256 buses
eui_30:
      movd  edi, xmm3
      and   edi, MMCFG_RULE_ADDR_MASK                   ; edi = MMCFG base addr
      mov   ebx, CSR_MMCFG_RULE_UBOX
      mov   eax, (BIT2+BIT1+BIT0)                       ; clear bit[2:0]
      not   eax
      and   eax, dword ptr ds:[edi][ebx]                ; eax = reg data
      or    eax, edx                                    ; update bit[3:0]
      mov   dword ptr ds:[edi][ebx], eax                ; write data to csr

      ;
      ; Program IOPORT_TARGET_LIST, MMIO_TARGET_LIST  before any io transaction. Legacy socket is the target
      ;
      mov   eax, 88888888h                              ; target list for legacy socket itself
      CALL_MMX6 AmILegacySocketNoStack                  ; am I legacy socket?
      je    eac_40                                      ; yes

      movd  eax, xmm0
      and   eax, (BIT7+BIT6+BIT5+BIT4)                  ; eax [7:4] = legacy socket id
      shr   eax, 4
      mov   ebx, eax                                    ; ebx[3:0] = eax[3:0] = legacy socket id
      mov   cl, 4                                       ; number of bits for socket id
      mov   ch, 7
eac_30:
      shl   ebx, cl
      or    eax, ebx
      dec   ch
      jnz   eac_30

eac_40:
      ;
      ; edi = MMCFG base addr
      ; eax = target list with all entries targeting legacy socket
      ;
      mov   ebx, CSR_IOPORT_TARGET_LIST_0_CBO
      mov   dword ptr ds:[edi][ebx], eax                ; write data to csr

      mov   ebx, CSR_MMIO_TARGET_LIST_0_CBO
      mov   dword ptr ds:[edi][ebx], eax                ; write data to csr

      mov   ebx, CSR_MMIO_TARGET_LIST_1_CBO
      mov   dword ptr ds:[edi][ebx], eax                ; write data to csr

      ;
      ; Enable the MMIO_RULE11 to cover the 0xFC000000 to 0xFFFFFFFF so that the NEM DATA region can be mapped to it.
      ; The target is set to local IIO.
      ;

      mov   ebx, (CSR_MMIO_RULE11_CBO + 4)
      mov   eax, 30000h                                 ; limitaddress_vlsb_bits = 3
      mov   dword ptr ds:[edi][ebx], eax                ; write data to csr

      mov   ebx, CSR_MMIO_RULE11_CBO
      mov   eax, 0FF00007Fh                             ; Base = 0xFC000000; Limit = 0xFFFFFFFF; Set the enable bit
      mov   dword ptr ds:[edi][ebx], eax                ; write data to csr


      RET_ESI_MMX3

EarlyUncoreInit    ENDP



;----------------------------------------------------------------------------
; Executed by all socket BSPs present in the system early after reset
;
;  Input:  All general puropse registers undefined
;          Stack may not be available
;          MMX3 = routine return address
;          MMCFG_Rule is enabled on local socket
;
;  Output: All general puropse registers undefined.
;----------------------------------------------------------------------------
MidUncoreInit    PROC    NEAR    PUBLIC

;
;  Check if running on Simics emulation
;

        CALL_MMX6 AmILegacySocketNoStack                  ; am I legacy socket?
        jnz   mui_exit                                    ; no ,skip
        ;
        ; Read EMULATION_FLAG CSR  and set the Emulation_flag in xmm0
        ;
        mov   eax, CSR_LEGACY_EMULATION_FLAG_DMI
        READ_PCI_DWORD_LEGACY                             ; ebx = reg data
        test  ebx, (BIT2+BIT1+BIT0)                       ; 0 = real hardware
        jz    mui_exit
;
; Check register header version against Simics
;
        mov   ecx, MSR_SIMICS_CSR_VERSION
        rdmsr                                             ; eax = Simics CSR header version
        cmp   eax, BIOS_CSR_HEADER_VERSION
        ;jae   mui_exit                                     ; version checking pass
        je   mui_exit                                     ; version checking pass

csr_version_check_failure:
        mov   eax, BIOS_CSR_HEADER_VERSION
        wrmsr                                             ; trigger a Simics exception alert
        ;jmp $

mui_exit:
        RET_ESI_MMX3

MidUncoreInit    ENDP


;----------------------------------------------------------------------------
; Executed by all socket BSPs present in the system early after reset
;
;  Input:  All general puropse registers undefined
;          Stack may not be available
;          MMX3 = routine return address
;          MMCFG_Rule is enabled on local socket
;
;  Output: All general puropse registers undefined.
;----------------------------------------------------------------------------

PreNemUncoreInit    PROC    NEAR    PUBLIC
        ;
        ; Prior to NEM init, use VCU API to notify VCU of "BIOS is about to enable cache"
        ;
        movd  eax, xmm0
        shr   eax, 12
        and   eax, 0ffffh                               ; [15:0] = Cpu Family
        cmp   eax, CPU_FAMILY_SKX                       ; SKX CPU?
        jne   pnui_exit                                 ; No, skip

        ; A this point, MMCFG_RULE has been enabled to target local CSRs
        CALL_MMX6 GetMmcfgNoStack                       ; edi = MMCFG.base

        ;
        ; check if running on real hardware or simulation tool
        ;

        mov   ebx, CSR_EMULATION_FLAG_DMI               ; ebx = Dev:Func:Offset of EMULATION_FLAG CSR
        mov   eax, dword ptr ds:[edi][ebx]              ; read CSR
        or    al, al                                    ; read hardware?
        jnz   pnui_exit                                 ; no, skip

        mov   ecx, VCODE_API_DELAY_COUNT                ; software timeout loop-count
        mov   ebx, CSR_VCU_MAILBOX_INTERFACE            ; ebx = Bus:Dev:Func:Offset of VCU_MAILBOX_INTERFACE CSR
        mov   edx, CSR_VCU_MAILBOX_DATA                 ; edx = Bus:Dev:Func:Offset of VCU_MAILBOX_DATA CSR

poll_vcu_busy1:
        mov   eax, dword ptr ds:[edi][ebx]              ; read CSR VCU_MAILBOX_INTERFACE
        cmp   eax, 0ffffffffh
        je    pnui_vcu_failure                          ; debug: - skip if the CSR is not implemented
        test  eax, BIT31
        loopnz poll_vcu_busy1
        or    ecx, ecx
        jz    pnui_vcu_failure

        ;
        ;  Call VCU API Open Sequnece (IOT_LLC_SETUP)
        ;
        mov   eax, VCODE_API_SEQ_ID_IOT_LLC_SETUP
        mov   dword ptr ds:[edi][edx], eax              ; write CSR VCU_MAILBOX_DATA

        mov   eax, (BIT31 OR VCODE_API_OPCODE_OPEN_SEQUENCE)
        mov   dword ptr ds:[edi][ebx], eax              ; write CSR VCU_MAILBOX_INTERFACE

        mov   ecx, VCODE_API_DELAY_COUNT                ; software timeout loop-count
poll_vcu_busy2:
        mov   eax, dword ptr ds:[edi][ebx]              ; read CSR VCU_MAILBOX_INTERFACE
        test  eax, BIT31
        loopnz poll_vcu_busy2
        or    ecx, ecx
        jz    pnui_vcu_failure

        ;
        ;  Call VCU API SET_PARAMETER with Index = 0
        ;  This step is required although the parameters  is dont care
        ;

        mov   eax, 0                                    ; set data = 0
        mov   dword ptr ds:[edi][edx], eax              ; write CSR VCU_MAILBOX_DATA
        mov   eax, (BIT31 OR VCODE_API_OPCODE_SET_PARAMETER)
        mov   dword ptr ds:[edi][ebx], eax              ; write CSR VCU_MAILBOX_INTERFACE

        mov   ecx, VCODE_API_DELAY_COUNT                ; software timeout loop-count
poll_vcu_busy3:
        mov   eax, dword ptr ds:[edi][ebx]              ; read CSR VCU_MAILBOX_INTERFACE
        test  eax, BIT31
        loopnz poll_vcu_busy3
        or    ecx, ecx
        jz    pnui_vcu_failure

        ;
        ;  Call VCU API  IOT_LLC_SETUP with Index=1
        ;
        mov   eax, (BIT31 OR VCODE_API_OPCODE_IOT_LLC_SETUP)
        or    eax, (1 SHL 16)                               ; bit[23:16] = Index = 1
        mov   dword ptr ds:[edi][ebx], eax                  ; write CSR VCU_MAILBOX_INTERFACE

        mov   ecx, VCODE_API_DELAY_COUNT                    ; software timeout loop-count
poll_vcu_busy4:
        mov   eax, dword ptr ds:[edi][ebx]                  ; read CSR VCU_MAILBOX_INTERFACE
        test  eax, BIT31
        loopnz poll_vcu_busy4
        or    ecx, ecx
        jz    pnui_vcu_failure

        ;
        ; Call VCU API Close Sequnece (IOT_LLC_SETUP)
        ;
        mov   eax, VCODE_API_SEQ_ID_IOT_LLC_SETUP
        mov   dword ptr ds:[edi][edx], eax                  ; write CSR VCU_MAILBOX_DATA

        mov   eax, (BIT31 OR VCODE_API_OPCODE_CLOSE_SEQUENCE)
        mov   dword ptr ds:[edi][ebx], eax                  ; write CSR VCU_MAILBOX_INTERFACE

        mov   ecx, VCODE_API_DELAY_COUNT                    ; software timeout loop-count
poll_vcu_busy5:
        mov   eax, dword ptr ds:[edi][ebx]                  ; read CSR VCU_MAILBOX_INTERFACE
        test  eax, BIT31
        loopnz poll_vcu_busy5
        or    ecx, ecx
        jz    pnui_vcu_failure

pnui_exit:
        RET_ESI_MMX3


pnui_vcu_failure:
        PORT80(VCU_COMMAND_TIMEOUT_ERROR_CODE)              ; error code to port 80h
        jmp  pnui_exit

PreNemUncoreInit    ENDP



;----------------------------------------------------------------------------
; Executed by all socket BSPs present in the system early after reset
;
;  Input:  All general puropse registers undefined
;          Stack may not be available
;          MMX3 = routine return address
;          MMCFG_Rule is enabled on local socket
;
;  Output: All general puropse registers undefined.
;----------------------------------------------------------------------------
PostNemUncoreInit    PROC    NEAR    PUBLIC

        call ProcessorWorkaround                ; Early CPU WAs
; CPL3 change - Enable Code after Mailbox Command is available.
        call  GetMmcfgBase                      ; esi = MMCFG.base
        mov   eax, 1
        cpuid
        and   eax, 0ffff0h
        cmp   eax, (CPU_FAMILY_SKX shl 4)  ;Check for SKX family
        jne   EndCpl3

        ; Only set P1 on warm reset
        mov   ebx, CSR_BIOSSCRATCHPAD7
        mov   eax, dword ptr ds:[esi][ebx]
        and   eax, BIT5
        jz    EndCpl3

        ; PCU Mailbox Interface
        mov   ebx, CSR_BIOS_MAILBOX_INTERFACE_PCU_FUN1_REG
        mov   cl, 30
RunBusyLoop1:
        dec   cl
        jz    EndCpl3
        test  dword ptr ds:[esi][ebx], 080000000h  ;Check for RUN_BUSY bit
        jnz   RunBusyLoop1

        ; Set mailbox data register
        mov   ebx, CSR_BIOS_MAILBOX_DATA_PCU_FUN1_REG
        mov   dword ptr ds:[esi][ebx], BIOS_MAILBOX_DATA_SWITCHTO_P1

        ; Write mailbox cmd register
        mov   ebx, CSR_BIOS_MAILBOX_INTERFACE_PCU_FUN1_REG
        mov   dword ptr ds:[esi][ebx], BIOS_MAILBOX_COMMAND_SWITCHTO_P1  ; Request for P1 speed

        mov   cl, 30
RunBusyLoop2:
        dec   cl
        jz    EndCpl3
        test  dword ptr ds:[esi][ebx], 080000000h  ;Check for RUN_BUSY bit
        jnz   RunBusyLoop2
EndCpl3:
; CPL3 change

        ret

PostNemUncoreInit    ENDP



;----------------------------------------------------------------------------
; EnableAccessGlobalCSR_NoStack
; Executed by SBSP Legacy socket only
;     - Update MMCFT_TARGET_LIST to enable cross-socket CSR access,
;     - Enable CPUBUSNO/CPUBUSNO1
;-----------------------------------------------------------------------
;
;  Input:  All general purpose registers undefined
;          Stack may not be available
;          MMX3 = routine return address
;          MMCFG_Rule is enabled on local socket
;
;          Following xmm registers have been initialized to hold such config info:
;                xmm0 = [3:0]  local socket id
;                             [7:4]  legact socket id
;                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
;                             [29:28]  rsvd
;                             [30]    Legacy socket flag
;                             [31] = Warm Reset Path flag
;                xmm1 = MCFG_TARGET_LIST CSR current data
;                xmm3 = MMCFG_RULE CSR current data
;                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
;                xmm5 = CPUBUSNO [Bus4,Bus5]
;
;  Output: All general purpose registers undefined.
;          MMCFG_TARGET_LIST, CPUBUSNO related CSRs are modified/initialized.
;----------------------------------------------------------------------------

EnableAccessGlobalCSR_NoStack    PROC    NEAR  PUBLIC

      movd  edi, xmm3
      and   edi, MMCFG_RULE_ADDR_MASK           ; edi = MMCFG_BASE_ADDR

      ; init MMCFG CSR in IIO (at 0:5:0:90)
      mov   ebx, CSR_MMCFG_IIO
      movd  eax, xmm3                           ; eax = copy of MMCFG_RULE
      and   eax, MMCFG_RULE_ADDR_MASK           ; mask off bit[13:0]
      mov   dword ptr ds:[edi][ebx], eax        ; write to CSR
      ; clear the higher address dword
      xor   eax, eax
      mov   dword ptr ds:[edi][ebx+4], eax      ; write to CSR

      ; calculate mmcfg limit = mmcfg_base + Max_Bus * 1MB)
      movd  ecx, xmm3                           ; ecx = copy of MMCFG_RULE
      and   ecx, 1110b
      shr   cl, 1                               ; cl = max bus number encoding.
                                                ;      0: 2048, 1: 1024, 2:512, 3: rsvd, 4: 256, 5: 128, 6: 64
      cmp   cl, 4
      jb    gcs20
      dec   cl                                  ;  adjuct shift count in cl
gcs20:
      mov   eax, 0800h                          ; eax = 2048 (buses)
      shr   eax, cl                             ; eax = 256 / 128 / 64 buses
      shl   eax, 20                             ; eax = eax * 100000h (i.e.Max_Bus * 1MB)

      movd  ecx, xmm3                           ; ecx = copy of MMCFG_RULE
      and   ecx, MMCFG_RULE_ADDR_MASK           ; ecx = MMCFG_BASE_ADDR
      add   eax, ecx                            ; eax = MMCFG_BASE_ADDR + MMCFG_SIZE
      sub   eax, 1                              ; last decode byte is last byte in previous 64MB block
      and   eax, 0fc000000h                     ; 64MB block address of last block of MMCFG
      add   ebx, CSR_MMCFG_IIO_BASE_LIMIT_GAP   ; CSR_MMCFG_IIO + gap   (MMCFG.Limit) (HSXB0 Update: Change CSR_MMCFG_IIO_BASE_LIMIT_GAP_A0 to CSR_MMCFG_IIO_BASE_LIMIT_GAP)
      mov   dword ptr ds:[edi][ebx], eax        ; write back CSR

;-----------------------------------------------------------------------
;     - Reprogram MMCFG_TARGET_LIST to enable cross-socket CSR access from legacy socket
;     - Enable CPUBUSNO/CPUBUSNO1
;-----------------------------------------------------------------------

; Choose MMCFG target list based on SBSP (legacy) socketID, so that
;   - Each of the 8 sockets is mapped to a range of 32 (20h) buses
;   - Legacy socket is always mapped to the bus range 00h - 1Fh
;
; SBSP    MMCFG Target_list
; -----  -------------------------
;  S0        76543210   (Default)
;  S1        76543201
;  S2        76543102
;  S3        76542103
;  S4        76532104
;  S5        76432105
;  S6        75432106
;  S7        65432107

; Get legacy socketID
      movd  edx, xmm0                               ; DL[7:4] = legacy socketID
      shr   edx, 4
      and   edx, (BIT3+BIT2+BIT1+BIT0)              ; DL = legacy socketID

      mov   eax, S0_MMCFG_TARGET_LIST_GLOBAL
      cmp   dl, 0                                   ; Legacy socketID = 0?
      je    gcs50                                   ; Yes
      mov   eax, S1_MMCFG_TARGET_LIST_GLOBAL
      cmp   dl, 1                                   ; Legacy socketID = 1?
      je    gcs50                                   ; Yes
      mov   eax, S2_MMCFG_TARGET_LIST_GLOBAL
      cmp   dl, 2                                   ; Legacy socketID = 2?
      je    gcs50                                   ; Yes
      mov   eax, S3_MMCFG_TARGET_LIST_GLOBAL
      cmp   dl, 3                                   ; Legacy socketID = 3?
      je    gcs50                                   ; Yes
      mov   eax, S4_MMCFG_TARGET_LIST_GLOBAL
      cmp   dl, 4                                   ; Legacy socketID = 4?
      je    gcs50                                   ; Yes
      mov   eax, S5_MMCFG_TARGET_LIST_GLOBAL
      cmp   dl, 5                                   ; Legacy socketID = 5?
      je    gcs50                                   ; Yes
      mov   eax, S6_MMCFG_TARGET_LIST_GLOBAL
      cmp   dl, 6                                   ; Legacy socketID = 6?
      je    gcs50                                   ; Yes
      mov   eax, S7_MMCFG_TARGET_LIST_GLOBAL
;      cmp   dl, 7                                   ; Legacy socketID = 7?
;      je    gcs50                                   ; Yes

gcs50:
; Map the first 32 buses (00h to 1Fh) to legacy socket
      or    eax, BIT3                           ; set Local bit
      mov   ebx, CSR_MMCFG_TARGET_LIST_CBO


      mov   dword ptr ds:[edi][ebx], eax        ; write CSR
      movd  xmm1, eax                           ; save it in xmm1

;------------------------------------------------------------------------------
; Setup the non-default BUS NO in Legacy Socket
;------------------------------------------------------------------------------
      ;
      ; Disable current MMCFG_RULE (at default bus#)
      ;
      mov   eax, CSR_LEGACY_MMCFG_RULE_CBO
      READ_PCI_DWORD_LEGACY                             ; ebx = reg data
      and   ebx, MMCFG_RULE_CLR_ENABLE                  ; set enable bit
      WRITE_PCI_DWORD_LEGACY


      ;Initialize MMCFG_LOCAL_RULE_ADDRESS_CFG (at 1:29:1:c8, cc)
      ;program local stack bus numbers in Cha
      mov   eax, CSR_LEGACY_MMCFG_LOCAL_RULE_ADDRESS0_CBO
      mov   ebx, LEGACY_MMCFG_LOCAL_RULE_ADDR0          ; ebx = CSR data
      WRITE_PCI_DWORD_LEGACY

      mov   eax, CSR_LEGACY_MMCFG_LOCAL_RULE_ADDRESS1_CBO
      mov   ebx, LEGACY_MMCFG_LOCAL_RULE_ADDR1          ; ebx = CSR data
      WRITE_PCI_DWORD_LEGACY

      ;
      ; Initialize and enable MMCFG_Rule register
      ; a. init Cbo MMCFG_RULE  (at 1:29:1:C0)
      ;
      mov   eax, CSR_LEGACY_MMCFG_RULE_CBO + 4          ; clear MMCFG high address
      xor   ebx, ebx
      WRITE_PCI_DWORD_LEGACY
      mov   eax, CSR_LEGACY_MMCFG_RULE_CBO
      READ_PCI_DWORD_LEGACY                             ; ebx = reg data
      mov   ecx, MMCFG_RULE_ADDR_MASK
      not   ecx
      and   ebx, ecx                                    ; mask off base address
      and   bl, 0f0h                                    ; mask off Length, keep Valid_Targets
      or    ebx, DEFAULT_MMCFG_RULE                     ; update base addr and length
      or    ebx, BIT0                                   ; set enable bit
      WRITE_PCI_DWORD_LEGACY

      mov   eax, LEGACY_COLDBOOT_CPUBUSNO               ; BUS NO 0, 11h, 12h, 13h for PEI to Pre-KTI
      movd  xmm4, eax
      mov   eax, LEGACY_COLDBOOT_CPUBUSNO1              ; BUS NO 14h, 15h  for PEI to Pre-KTI
      movd  xmm5, eax

;
; Program  UBOX CPUBUSNO/CPUBUSNO1 CSRs
;
      mov   ebx, CSR_CPUBUSNO_UBOX
      movd  eax, xmm4
      mov   dword ptr ds:[edi][ebx], eax        ; write CSR

      mov   ebx, CSR_CPUBUSNO1_UBOX
      mov   eax, dword ptr ds:[edi][ebx]        ; eax = CSR data
      and   eax, 0ffff0000h                     ; mask fields for MCP stacks
      movd  esi, xmm5
      or    eax, esi
      mov   dword ptr ds:[edi][ebx], eax        ; write CSR

;
; Set CPUBUSNO_VALID.busno_programmed bit to 1
;
      mov   ebx, CSR_CPUBUSNO_VALID_UBOX
      mov   eax, dword ptr ds:[edi][ebx]        ; eax = CSR data
      or    eax, BIT31                          ; set busno_programmed = 1
      mov   dword ptr ds:[edi][ebx], eax        ; write CSR

;
; Program default IOAPIC_TARGET_LIST
;
      mov   ebx, CSR_IOAPIC_TARGET_LIST_0
      mov   eax, DEFAULT_IOAPIC_TARGET_LIST
      BUSNO1_EBX
      mov   dword ptr ds:[edi][ebx], eax        ; write CSR
      mov   ebx, CSR_IOAPIC_TARGET_LIST_1
      BUSNO1_EBX
      mov   dword ptr ds:[edi][ebx], eax        ; write CSR
      mov   ebx, CSR_IOAPIC_TARGET_LIST_2
      BUSNO1_EBX
      mov   dword ptr ds:[edi][ebx], eax        ; write CSR
      mov   ebx, CSR_IOAPIC_TARGET_LIST_3
      BUSNO1_EBX
      mov   dword ptr ds:[edi][ebx], eax        ; write CSR

;-------------------------------------------------------------------------------
; From this point on, inter-socket CSR access is enabled
; Valid matching bus# must be used to access all CSRs.
;-------------------------------------------------------------------------------

eac_exit:
     RET_ESI_MMX3                              ; return

EnableAccessGlobalCSR_NoStack    ENDP



;-------------------------------------------------------------------------
; CPU Early Workaround
;Executed by all package BSPs
; Input:
;    Stack available
; Output:
;    All general purpose registers destroyed
;-------------------------------------------------------------------------
ProcessorWorkaround PROC    NEAR    PUBLIC

  call  _OEMCpuSecInit
  ret
ProcessorWorkaround  ENDP


;-------------------------------------------------------------------------
; Read a Dword from PCI register 00h thru FFh using IO port CF8h/CFCh
; Input:
;   Stack is avaialble
;   xmm registers initialized.
;   xmm0 register bit [7:4]  contains Legacy Socket ID
; Output: AL = SBSP Socket ID
; Registers changed: None except eax
;-------------------------------------------------------------------------
GetSbspSktId      PROC    NEAR
        movd eax, xmm0                  ; AL[7:4] = SBSP socket id
        shr  eax, 4
        and  eax, (BIT3+BIT2+BIT1+BIT0)
        ret
GetSbspSktId      ENDP

;-------------------------------------------------------------------------
; Returns Socket/Packge ID of the current processor
; Input:
;   Stack is avaialble
;   xmm registers initialized
;   xmm0 register bit[3:0] contains local Socket ID
; Output: CL = Socket/Package ID of the current processor
; Registers changed: None except ecx
;-------------------------------------------------------------------------
GetLocalSktId      PROC    NEAR    PUBLIC

     movd  ecx, xmm0                                      ; xmm0[3:0] stores the socketID
     and   ecx, (BIT3+BIT2+BIT1+BIT0)                     ; cl = local socket ID
     ret

GetLocalSktId      ENDP


;----------------------------------------------------------------------------
; SetupProcessorCheckinData
;  Executed by all socket BSPs.  Store data in its own local scratchpad register:
;       bit[0]        - check in flag
;       bit[23:8]   - ApicId  (only 16-bit is used)
;       bit[30:27] - stepping
;  Input:  Stack available
;       Local MMCFG config CSR  access is enabled
;  Output: All general puropse registers preserved.
;----------------------------------------------------------------------------

SetupProcessorCheckinData    PROC    NEAR    PUBLIC

        pushad

        mov    eax, 1
        cpuid                                               ; ebx[31:24] = initial APIC ID, eax[3:0] = stepping
        and    ebx, 0ff000000h
        shr    ebx, 16t                                     ; APIC ID in ebx[23:8]
        and    eax, 0fh
        shl    eax, 27t                                     ; eax[30:27] = stepping
        mov    esi, eax                                     ; esi [30:27] = setpping
        push   ebx                                          ; save ebx

        ; check to see if processor is x2Apic capable
        mov    eax, 0
        cpuid
        pop    ebx                                          ; restore ebx
        cmp    eax, 0bh                                     ; check to see if processor is x2Apic capable
        jb     SaveDataAndSetCheckIn                        ; processor is not x2APIC capable

        ; processor is x2Apic capable, get ApicId from cpuid leaf B
        mov    eax, 0bh
        xor    ecx, ecx
        cpuid
        movzx  ebx, dx                                      ; though X2Apid is 32bit wide,
        shl    ebx, 8                                       ; HW support up to 12 bits (bit31:12=0)

SaveDataAndsetCheckIn:
        ; ebx [23:8] = Apic ID,  esi [30:27] = stepping
        ; call   GetLocalSktId                                 ; CL = own local socket ID
        mov    eax, SR_PBSP_CHECKIN_CSR
        call   ReadCpuCsr                                    ; EAX = current CSR data
        and    eax, 870000ffh                                ; clear bit[30:27], [23:8]
        or     ebx, eax                                      ;  ebx <- apci id
        or     ebx, esi                                      ;  ebx <- stepping
        or     ebx, 1                                        ; set check in bit
        mov    eax, SR_PBSP_CHECKIN_CSR
        call   WriteCpuCsr

        popad
        ret

SetupProcessorCheckinData       ENDP



;-------------------------------------------------------------------------
; UncoreBusCfgSyncNbsp
;   A PBSP waits for and hand-shakes with SBSP to receive re-programming of
;   MMCFG/CPUBUSNO related registers
;
; Input:
;     Stack is available
;     Following xmm registers hold MMCFG and bus number config info
;                xmm0 = [3:0]  local socket id
;                             [7:4]  legact socket id
;                             [31] = Warm Reset Path flag
;                xmm1 = MCFG_TARGET_LIST CSR current data
;                xmm3 = MMCFG_RULE CSR current data
;                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
;                xmm5 = CPUBUSNO [Bus4,Bus5]
;
; Output:
;    PBSP has switched to the new MMCFG/CPUBUSNO configuration with global view of the MMCFG config space
;
; Registers changed: None
;-------------------------------------------------------------------------


UncoreBusCfgSyncNbsp           PROC    NEAR    PUBLIC

        pushad

;
;  At this point, all CSR accesses are local to the socket, and default bus# 0...5 are used for local targets
;

;
;  Disable CHA MMCFG_RULE
;
      mov   eax, CSR_LEGACY_MMCFG_RULE_CBO                 ; eax = reg address
      call  ReadPciDwordLegacy                             ; ebx = reg dword data
      btr   ebx, 0                                         ; clear Enable bit
      call  WritePciDwordLegacy                            ; write to reg
      movd  xmm3, ebx                                      ; update copy in xmm3
;
;  From this point, only CF8/CFC IO can be used for CSR access
;
;
;  Set flag in local scratchpad CSR to indicate to SBSP "I'm ready for MMCFG/BUSNO re-programming"
;
      mov   eax, SR_LEGACY_BUSCFG_SYNC_CSR                  ; eax = reg address
      mov   ebx, BusCfgSync_PBSP_Ready                      ; ebx = reg dword data
      call  WritePciDwordLegacy                             ; write to reg

PbspWait10:
;
;  Loop here on local scratchpad CSR
;  waiting for local MMCFG/CPUBUSNO etc.  CSRs be updated by SBSP,
;  until notified by SBSP to enable new configuration
;
      mov   eax, SR_LEGACY_BUSCFG_SYNC_CSR                 ; eax = reg address
      call  ReadPciDwordLegacy                             ; ebx = reg dword data
      cmp   ebx, BusCfgSync_PBSP_Go
      jne   PbspWait10
;
;  At this point, some local CSRs have been updated by SBSP with new settings
;  Update local copy of the new CHA MMCFG_RULE, UBOX CPUBUSNO (in xmm registers)
;
      mov   eax, CSR_LEGACY_MMCFG_TARGET_LIST_CBO          ; eax = reg address
      call  ReadPciDwordLegacy                             ; ebx = reg dword data
      movd  xmm1, ebx                                      ; update xmm reg with new data
;
; Get local copy of mmcfg base to support multisegments
;
      mov   eax, SR_LEGACY_BUSCFG_DATA_CSR
      call  ReadPciDwordLegacy                             ; ebx = reg dword data
      or    ebx, BIT0
      movd  xmm3, ebx                                      ; update xmm reg with new data
      mov   eax, CSR_LEGACY_CPUBUSNO_UBOX
      call  ReadPciDwordLegacy                             ; ebx = reg dword data
      movd  xmm4, ebx                                      ; update xmm reg with new data
      mov   eax, CSR_LEGACY_CPUBUSNO1_UBOX
      call  ReadPciDwordLegacy                             ; ebx = reg dword data
      movd  xmm5, ebx                                      ; update xmm reg with new data

;
;  Enable UBOX CPUBUSNO_VALID
;
      mov   eax, CSR_LEGACY_CPUBUSNO_VALID_UBOX            ; eax = reg address
      call  ReadPciDwordLegacy                             ; ebx = reg dword data
      or    ebx, BIT31                                     ; set Valid bit = 1
      call  WritePciDwordLegacy                            ; write to reg
;
;  From this point on, new CPUBUSNO must be used
;  Get new bus0/bus1 value
;
      movd  ecx, xmm4
      mov   edx, ecx

      and   ecx, 000000ffh                                 ; bit[7:0] = bus0 field
      shl   ecx, 16                                        ; bit[23:16] = bus0

      and   edx, 0000ff00h                                 ; bit[15:8] = bus1 field
      shl   edx, 8                                         ; bit[23:16] = bus1
;
;  ecx[23:16] = new bus# for local target 0 (Bus0, UBOX)
;  edx[23:16] = new bus# for local target 1 (Bus1, CHA)
;

;
;  Enable  MMCFG_RULE in CHA, on Bus1
;
      mov   eax, CSR_LEGACY_MMCFG_RULE_CBO                  ; eax = reg address
      and   eax, 0ff00ffffh                                 ; clear bus# field
      or    eax, edx                                        ; update bus# field with new Bus1
      call  ReadPciDwordLegacy                              ; ebx = reg dword data
      or    ebx, BIT0                                       ; set Enable bit = 1
      call  WritePciDwordLegacy                             ; write to reg
;
; At this point, new CPUBUSNO and MMCFG_RULE are all enabled on the PBSP socket
;

;
;  Enable UBOX copy of MMCFG_RULE on Bus0
;
      mov   eax, CSR_MMCFG_RULE_UBOX                        ; eax = reg address
      ;and   eax, 0ff00ffffh                                 ; clear bus# field
      ;or    eax, ecx                                        ; update bus# field with new Bus0
      call  ReadCpuCsr                                      ; eax = reg data
      mov   ebx, eax
      or    ebx, BIT0                                       ; set  Enable bit = 1
      mov   eax, CSR_MMCFG_RULE_UBOX                        ; eax = reg address
      call  WriteCpuCsr                                     ; write to reg

;
;  Send message back to SBSP to indicate "I'm done with enabling the new MMCFG/CPUBUS config"
;  via local scratchpad CSR in UBOX on Bus0
;
      mov   eax, SR_BUSCFG_SYNC_CSR                         ; eax = reg address
      ;and   eax, 0ff00ffffh                                 ; clear bus# field
      ;or    eax, ecx                                        ; update bus# field with new Bus0
      mov   ebx, BusCfgSync_PBSP_Done                       ; ebx = reg dword data
      call  WriteCpuCsr                                     ; write to reg
;
;  Wait for SBSP to receive/process the message and clear the scartchpad CSR as acknoledgement
;
PbspWait50:
      mov   eax, SR_BUSCFG_SYNC_CSR                         ; eax = reg address
      call  ReadCpuCsr                                      ; eax = reg data
      cmp   eax, BusCfgSync_PBSP_Done
      je    PbspWait50                                      ; loop until not equal

      popad
      ret


UncoreBusCfgSyncNbsp          ENDP




;----------------------------------------------------------------------------
; EnableAccessGlobalCSR
;  Executed by SBSP (Legacy socket) only
;  Input:  Stack available
;             Local MMCFG config CSR  access is enabled
;  Output:
;             MMCFG_TARGET_LIST updated
;             CPUBUSNO enabled
;             Inter-socket CSR accesses enabled
;
;  All general puropse registers preserved.
;----------------------------------------------------------------------------
EnableAccessGlobalCSR        PROC     NEAR   PUBLIC

       pushad

       CALL_MMX3 EnableAccessGlobalCSR_NoStack

       popad
       ret

EnableAccessGlobalCSR        ENDP




;-------------------------------------------------------------------------
; Non-SBSPs execute this routine to clear BSP flag in MSR and
; loop on a local scratch pad register for command from SBSP
; Input: None
;
; Output: None
;-------------------------------------------------------------------------

ParkNbsp      PROC    NEAR

     ; clear bit8 BSP flag in MSR
     mov     ecx, MSR_APIC_BASE
     rdmsr
     and     eax, NOT BIT8                                 ; clear bit8 BSP flag in MSR
     wrmsr

     call  GetLocalSktId                                   ; CL = Local Socketr ID

   ; When SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR becomes non-zero,
   ;       SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR will be the jmp location

SitAndSpin:
     mov   eax, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR   ; Pipe Dispatch register in local socket
     call  ReadCpuCsr

     cmp   eax, 0
     jz    SHORT SitAndSpin

     mov   eax, SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR   ; Pipe Command register in local socket
     call  ReadCpuCsr

     cmp   eax, PIPE_DISPATCH_SYNCH_PSYSHOST
     jz    GoToAction

     ; clear out dispatch register because command is invalid
     ;mov   eax, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR   ; Pipe Dispatch register in local socket
     ;xor   ebx, ebx                                        ; Clear the dispactch CSR
     ;call  WriteCpuCsr
     jmp   SitAndSpin

GoToAction:

     mov   eax, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR       ; Pipe Data register in local socket
     call  ReadCpuCsr
     mov   esp, eax                                        ; sync up stack pointer with SBSP

     mov   eax, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR   ; Pipe Dispatch register in local socket
     xor   ebx, ebx                                        ; Clear the dispactch CSR
     call  WriteCpuCsr

WaitForJmpPointer:
     mov   eax, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR   ; Pipe Dispatch register in local socket
     call  ReadCpuCsr

     cmp   eax, 0
     jz    SHORT WaitForJmpPointer

     mov  eax, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR
     call GetBusNum                                        ; update EAX[27:20] with the correct bus #
     call GetMmcfgBase                                     ; esi = MMCFG Base address
     add  esi, eax                                         ; form cfg register address in esi
     and  esi, 0fffff000h                                  ; keep only Bus:Dev:Func portion of it

     mov   eax, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR       ; Pipe Data register in local socket
     call  ReadCpuCsr

   ; EAX = Function pointer to PipeSlaveInit( UINT32 NodeId, UINT32 CfgAddrBDF)
   ; Emulate a C-style function call
     push   esi                                             ; push input arg (PCIE Config Address w/o register offset)
     push   ecx                                             ; push input arg (local socketId)
     call   eax                                             ; call PipeSlaveInit( UINT32 NodeId, UINT32 CfgAddrBDF)
                                                            ; Note: Control will not return to here
ParkNbsp      ENDP

;-------------------------------------------------------------------------
; Check if warm reset path.  Can be called by each socket BSP
; Input:
;   Stack is NOT available
;   MMX6 = return address via RET_ESI_MMX6 macro
;   Register xmm0[31] has been initialized to indicate warm reset
;          xmm0  [31] = 0   Power Good reset path
;                               1   Warm reset path
;
; Output:
;              ZF = 1      Cold reset path
;              ZF = 0      Warm reset path
;
; Registers changed:  ESI
;-------------------------------------------------------------------------

IsWarmResetNoStack    PROC  PUBLIC

      movd  esi, xmm0
      test  esi, BIT31
     ;  ZF = 1      Cold reset path
     ;  ZF = 0      Warm reset path

      RET_ESI_MMX6

IsWarmResetNoStack ENDP



;-------------------------------------------------------------------------
; Check if I am legacy socket. Can be called by each socket BSP
; Input:
;   Stack is NOT available
;   MMX6 = return address via RET_ESI_MMX6 macro
;   xmm0 registers initialized and contains socket id info:
;          xmm0  [3:0]= Local socket id,
;                     [7:4]= Legacy socket id
;                     [30] = Legacy Socket flag
;
; Output:  ZF = 1       Yes, I am legacy socket
;              ZF = 0      No
;
; Registers changed: esi
;-------------------------------------------------------------------------

AmILegacySocketNoStack    PROC  PUBLIC

      movd  esi, xmm0
      and   esi, BIT30              ;  [30] = Legacy Socket flag
      xor   esi, BIT30              ;  am I legacy socket?    ZF = 1/0  -> Yes/No

      RET_ESI_MMX6

AmILegacySocketNoStack ENDP



;-------------------------------------------------------------------------
; Callable version of  AmILegacySocketNoStack()
; Check if I am legacy socket. Can be called by all sockets
; Input:
;              Stack is available
; Output:
;              ZF = 1       Yes, I am legacy socket
;              ZF = 0       No
;
; Registers changed: none
;-------------------------------------------------------------------------

AmILegacySocket             PROC PUBLIC

      push  esi
      CALL_MMX6  AmILegacySocketNoStack
      pop   esi
      ret

AmILegacySocket     ENDP



;-------------------------------------------------------------------------
; Get MMCFG.BASE
; Input:
;     Stack is NOT available
;     MMX6 = return address via RET_ESI_MMX6 macro
;     CL = Local target BusID (0,1,...5)
;     Following xmm registers hold MMCFG and bus number config info
;                xmm0 = [3:0]  local socket id
;                             [7:4]  legact socket id
;                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
;                             [31] = Warm Reset Path flag
;                xmm1 = MCFG_TARGET_LIST CSR current data
;                xmm3 = MMCFG_RULE CSR current data
;                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
;                xmm5 = CPUBUSNO [Bus4,Bus5]
;
; Output: EDI = MMCFG.base
;         The caller can then use
;            mov ebx, CSR_DEV_FUNC_OFFSET
;            mov eax, dword ptr [edi] [ebx]       where ebx = bus/dev/func/offset in PCIe format
;
; Registers changed: edi, esi
;-------------------------------------------------------------------------

GetMmcfgNoStack    PROC  PUBLIC

      movd  edi, xmm3
      test  edi, BIT0                    ; MMCFG.enable already set?
      jnz   gmc_10                       ; yes

gmc_error:
      PORT80(MMCFG_NOT_YET_ENABLED_ERROR_CODE) ; error code to port 80h
      jmp  gmc_error                     ; dead loop

gmc_10:
      and   edi, MMCFG_RULE_ADDR_MASK    ; edi = MMCFG.base

      RET_ESI_MMX6

GetMmcfgNoStack  ENDP



;-------------------------------------------------------------------------
; Get MMCFG.BASE + Bus# field
; Input:
;   Stack is NOT available
;   MMX6 = return address via RET_ESI_MMX6 macro
;   CL = Local target BusID (0,1,...5)
;     Following xmm registers hold MMCFG and bus number config info
;                xmm0 = [3:0]  local socket id
;                             [7:4]  legact socket id
;                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
;                             [31] = Warm Reset Path flag
;                xmm1 = MCFG_TARGET_LIST CSR current data
;                xmm3 = MMCFG_RULE CSR current data
;                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
;                xmm5 = CPUBUSNO [Bus4,Bus5]
;
; Output: EDI = MMCFG.base + bus offset
;         The caller can then use
;            mov ebx, CSR_DEV_FUNC_OFFSET
;            mov eax, dword ptr [edi] [ebx]
;
; Registers changed: cl, edi, esi
;-------------------------------------------------------------------------

GetMmcfgBus_NoStack    PROC  PUBLIC

      movd  edi, xmm3
      test  edi, BIT0                   ; MMCFG.enable already set?
      jnz   gmb_10                      ; yes

gmb_error:
      PORT80(MMCFG_NOT_YET_ENABLED_ERROR_CODE)  ; error code to port 80h
      jmp  gmb_error                    ; dead loop

gmb_10:
      movd  esi, xmm4                   ; esi = CPUBUSNO[3,2,1,0]
      cmp   cl, 4
      jb    gmb_20
      movd  esi, xmm5                   ; esi = CPUBUSNO[_,_, 5,4]
      sub   cl, 4                       ; adjust BusID

gmb_20:
      shl   cl, 2                       ; cl = cl * 4
      shr   esi, cl                     ; esi[7:0] = target bus#
      and   esi, 0ffh
      shl   esi, 20                     ; esi[27:20] = target bus#

      and   edi, MMCFG_RULE_ADDR_MASK   ; edi = MMCFG.base
      add   edi, esi                    ; edi = MMCFG.base + bus offset

      RET_ESI_MMX6

GetMmcfgBus_NoStack  ENDP



;-------------------------------------------------------------------------
; Read a Dword from PCIe config register using mmcfg method
; Limitation:  Currently does not support config space size > 256 MB ( 256 buses)
; Input:
;   EAX = PCIe config address format with format defined as follows
;     EAX[31:28] = 0h
;     EAX[27:20] = Bus# of local target (Bus0,Bus1,...Bus5)
;     EAX[19:15] = Dev #
;     EAX[14:12] = Func #
;     EAX[11:0]  = Reg offset (dword aligned)
;     Following xmm registers hold MMCFG and bus number config info
;                xmm0 = [3:0]  local socket id
;                             [7:4]  legact socket id
;                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
;                             [31] = Warm Reset Path flag
;                xmm1 = MCFG_TARGET_LIST CSR current data
;                xmm3 = MMCFG_RULE CSR current data
;                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
;                xmm5 = CPUBUSNO [Bus4,Bus5]
;
; Output: EAX = regiser dword data
; Registers changed: None except EAX
;-------------------------------------------------------------------------

ReadCpuCsr      PROC    NEAR    PUBLIC

      push  esi
      call  GetBusNum                                    ; update EAX[27:20] with the correct bus# for the socket ID
      call  GetMmcfgBase                                 ; esi = MMCFG Base Address
      add   esi, eax                                     ; form cfg register full address in esi
      mov   eax, dword ptr ds:[esi]                      ; read dword
      pop   esi
      ret

ReadCpuCsr ENDP




;-------------------------------------------------------------------------
; Write a Dword from PCIe config register using mmcfg method
; Limitation:  Currently does not support config space size > 256 MB ( 256 buses)
;
; Input:
;   Stack is available
;   EAX = PCIe config address format with format defined as follows
;     EAX[31:28] = 0h
;     EAX[27:20] = Bus# of local target (Bus0,Bus1,...Bus5)
;     EAX[19:15] = Dev #
;     EAX[14:12] = Func #
;     EAX[11:0]  = Reg offset (dword aligned)
;     EBX  = Dword data to be written to reg
;     Following xmm registers hold MMCFG and bus number config info
;                xmm0 = [3:0]  local socket id
;                             [7:4]  legact socket id
;                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
;                             [31] = Warm Reset Path flag
;                xmm1 = MCFG_TARGET_LIST CSR current data
;                xmm3 = MMCFG_RULE CSR current data
;                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
;                xmm5 = CPUBUSNO [Bus4,Bus5]
;
; Output: None
; Registers changed: None
;-------------------------------------------------------------------------

WriteCpuCsr PROC

      push  eax
      push  esi
      call  GetBusNum                                    ; update EAX[27:20] with the correct bus# for the socket ID
      call  GetMmcfgBase                                 ; esi = MMCFG Base Address
      add   esi, eax                                     ; form cfg register full address in esi
      mov   dword ptr ds:[esi], ebx                      ; write dword
      pop   esi
      pop   eax
      ret

WriteCpuCsr ENDP




;-------------------------------------------------------------------------
;  Update EAX[27:20] with the target Bus number from MSR 300h
; Input:
;   EAX = PCIe config address format with format defined as follows
;     EAX[31:28] = 0h
;     EAX[27:20] = Bus# of local target (Bus0,Bus1,...Bus5)
;     EAX[19:15] = Dev #
;     EAX[14:12] = Func #
;     EAX[11:0]  = Reg offset (dword aligned)
;     Following xmm registers hold MMCFG and bus number config info
;                xmm0 = [3:0]  local socket id
;                             [7:4]  legact socket id
;                             [27:8] Cpu Family/Stepping info (from cpuid eax, bit[19:0])
;                             [31] = Warm Reset Path flag
;                xmm1 = MCFG_TARGET_LIST CSR current data
;                xmm3 = MMCFG_RULE CSR current data
;                xmm4 = CPUSUBNO [Bus0,Bus1,Bus2,Bus3]
;                xmm5 = CPUBUSNO [Bus4,Bus5]
;
; Output: EAX[27:20] updated with the actual bus# intended
; Registers changed: None except EAX[27:20]
;-------------------------------------------------------------------------
GetBusNum      PROC

      push  ebx
      push  ecx
      push  edx
      push  esi
      push  edi

      push  eax                       ; save input eax
      push  ecx                       ; save input ecx
      mov   ecx, 300h
      rdmsr                           ; edx:eax  = current CPUBUSNO/CPUBUSNO1
      test  edx, BIT31                ; Bus number valid?
      jnz   gbn10
      ; Bus Number is not valid, force default bus# that is decoded by UBOX
      ; If legacy Socket, BusNo: 0, 0x11, 0x12, 0x13, 0x14, 0x15
      ; If remote socket, BusNo: 0, 1,2,3,4,5

      call  AmILegacySocket
      jz    gbn5
      mov   eax, LEGACY_COLDBOOT_CPUBUSNO
      mov   edx, LEGACY_COLDBOOT_CPUBUSNO1
      jmp   gbn6
gbn5:
      mov   eax, DEFAULT_COLDBOOT_CPUBUSNO
      mov   edx, DEFAULT_COLDBOOT_CPUBUSNO1
gbn6:



 gbn10:
      mov   esi, eax
      mov   edi, edx
      pop   ecx                       ; restore input ecx
      pop   eax                       ; restore input eax


; Bus# byte-array
;              edi                                            esi
; ---------------------    --------------------------
; [Rsvd, Rsvd, Bus5, Bus4]       [Bus3, Bus2, Bus1, Bus0]

; get Bus# field of PCIe address in EAX
      mov   ecx, eax
      shr   ecx, 20
      and   ecx, 0ffh                    ;  [7:0] = bus#

      cmp   ecx, 4                       ; BusID >=4?
      jb    gbn20
      sub   ecx, 4                       ; adjust BusID
      mov   esi, edi                     ; adjust Bus array

; esi =  proper Bus# byte-array
; ecx = index into Bus# byte-array (value = 0,1,2 or 3)

; shift proper bus#  into esi[27:20]
 gbn20:
      shl   ecx, 3                       ; ecx = ecx * 8,  bit position of bus# field
      shr   esi, cl                      ; esi[7:0] = target bus#
      and   esi, 0ffh                    ; esi[7:0] = bus#
      shl   esi, 20                      ; esi[27:20] = bus#

      and   eax, 000fffffh               ; keep only Dev:Func:Offset
      or    eax, esi                     ; eax = Bus:Dev:Func:Offset

      pop   edi
      pop   esi
      pop   edx
      pop   ecx
      pop   ebx
      ret

GetBusNum      ENDP




;-------------------------------------------------------------------------
; Get MMCFG base address
;
;  Input:
;   Stack is avaialble
;   xmm3 register has been initialized and contains MMCFG_RULE
;
;  Output: ESI = 32-bit MMCFG base address
;  Registers changed:  none except ESI
;-------------------------------------------------------------------------
GetMmcfgBase   PROC     NEAR    PUBLIC

      ; PCIe config base address actually written into the MMCFG_RULE CSR
      movd  esi, xmm3                                       ; xmm3 stores MMCFG_RULE
      and   esi, MMCFG_RULE_ADDR_MASK                       ; make sure the lower bits are 0s
      ret

GetMmcfgBase   ENDP



;------------------------------------------------------------------------------
; Read a PCI config dword via lagacy CF8/CFC method
; Stack is available
; input: EAX = legacy PCI address format with Bus:Dev:Func:Offset
; output: EBX = dword register data
;
; Registers changed:  EBX
;------------------------------------------------------------------------------
ReadPciDwordLegacy     PROC     NEAR    PUBLIC

   push  eax
   push  edx
   or    eax, BIT31                  ; make sure bit31 is set
   READ_PCI_DWORD_LEGACY
   pop   edx
   pop   eax
   ret

ReadPciDwordLegacy     ENDP


;------------------------------------------------------------------------------
; Write a DWord data to PCI config reg via lagacy CF8/CFC method
; Stack is available
; input: EAX = legacy PCI address format with Bus:Dev:Func:Offset
;          EBX = dword data to write
;
; Registers changed: None
;------------------------------------------------------------------------------
WritePciDwordLegacy     PROC     NEAR    PUBLIC

   push  eax
   push  edx
   or    eax, BIT31                  ; make sure bit31 is set
   WRITE_PCI_DWORD_LEGACY
   pop   edx
   pop   eax
   ret

WritePciDwordLegacy      ENDP


STARTUP_SEG ENDS

END

