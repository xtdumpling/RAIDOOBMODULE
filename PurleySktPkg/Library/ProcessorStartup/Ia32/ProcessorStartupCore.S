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
#  ProcessorStartupCore.asm
#
# Abstract:
#
#
#------------------------------------------------------------------------------        

        .include "ProcessorStartupPlatform.i"
        .include "ProcessorStartupChipset.i"
        .include "ProcessorStartupUncore.i"
        .include "ProcessorStartup.i"

        .extern OemFindMicrocode
        .extern GetPlatformCpuSettings
        .extern GetLocalSktId
        .extern GetBusNum
        .extern ReadCpuCsr


#ifndef MINIBIOS_BUILD
        .extern PcdGet32 (PcdFlashSecCacheRegionBase)
        .extern PcdGet32 (PcdFlashSecCacheRegionSize)
#endif

#----------------------------------------------------------------------------
#       STARTUP_SEG  S E G M E N T  STARTS
#----------------------------------------------------------------------------

.code32

ASM_GLOBAL ASM_PFX(EarlyCoreInit)
ASM_PFX(EarlyCoreInit):
        #---If Limit CPU ID enabled because of soft reset, disable.
        movl    $0x1a0, %ecx
        rdmsr
        btrl    $22, %eax                               # Reset bit
        jnc     1f                                      # If already reset, do not write to 1a0.

        wrmsr
1:
        #---Change APIC Address---
        movl    $MSR_XAPIC_BASE, %ecx                      # Enable local APIC
        rdmsr
        andl    $(~0x0f), %edx
        andl    $(~0x0fffff000), %eax
        orl     $(LOCAL_APIC_BASE), %eax
        wrmsr

        RET_ESI_MMX3


ASM_GLOBAL ASM_PFX(LoadProcessorMicrocode)
ASM_PFX(LoadProcessorMicrocode):
  #
  # Load microcode on all pacakge BSPs.
  # Input: Stack NOT available
  #        MMX3 = routine return address

        CALL_MMX4  OemFindMicroCode

        orl   %eax, %eax                                # Microcode found?
        jz    MicrocodeLoadDone                         # JIf not.

        movl  %eax, %esi                                # ESI -> Microcode Update header.

        cmpl  $0, UpdateHeaderStruc_dUpdateRevision(%esi) # Microcode version is negative?
        jl    load_ucode                                  # yes, always load

   # Read MSR(8Bh) to verify microcode version
        movl  $MSR_IA32_BIOS_SIGN_ID, %ecx
        xorl  %eax, %eax                                # Clear EAX
        xorl  %edx, %edx                                # Clear EDX
        wrmsr                                           # Write 0 to MSR 8Bh
        movl  $1, %eax
        cpuid                                           # this will update MSR 8Bh
        movl  $MSR_IA32_BIOS_SIGN_ID, %ecx
        rdmsr                                           # edx = current microcode signature/revision

        cmpl  UpdateHeaderStruc_dUpdateRevision(%esi), %edx
        jge   MicrocodeLoadDone                         # ucode patch already loaded. Skip.

load_ucode: 
        movl  %esi, %eax                                # eax => ucode buffer
        movl  $MSR_IA32_BIOS_UPDT_TRIG, %ecx             # MSR 79h
        xorl  %edx, %edx
        addl  $48, %eax                                 # EAX -> Microcode Update data.
        wrmsr                                           # load the microcode

   # Read MSR(8Bh) to verify that the microcode loading was successful
        movl  $MSR_IA32_BIOS_SIGN_ID, %ecx
        xorl  %eax, %eax                                # Clear EAX
        xorl  %edx, %edx                                # Clear EDX
        wrmsr                                           # Write 0 to MSR 8Bh
        movl  $1, %eax
        cpuid                                           # this will update MSR 8Bh
        movl  $MSR_IA32_BIOS_SIGN_ID, %ecx
        rdmsr                                           # edx = current microcode signature/revision

        cmpl  UpdateHeaderStruc_dUpdateRevision(%esi), %edx
        je    MicrocodeLoadDone                         # successful

MicrocodeLoadFailure: 
        PORT80 (MCU_LOAD_ERROR_CODE)
        jmp     load_ucode


MicrocodeLoadDone: 

        RET_ESI_MMX3


ASM_GLOBAL ASM_PFX(CheckGenuineIntelCpu)
ASM_PFX(CheckGenuineIntelCpu):
  # 
  # Executes the CPUID instruction with EAX = 0, then reads 
  # the EBX, ECX, and EDX registers to determine if the CPU is "GenuineIntel"
  # 
        xorl  %eax, %eax
        cpuid
        cmpl  $0x756e6547, %ebx # "Genu"
        jne   cgic_error
        cmpl  $0x6c65746e, %ecx # "ntel"
        jne   cgic_error
        cmpl  $0x49656e69, %edx # "ineI"
        jne   cgic_error

        RET_ESI_MMX3

cgic_error: 
        # Error: non-Intel processor found
        PORT80  (NON_INTEL_CPU_ERROR_CODE)
        jmp  .                         # dead loop
##        hlt
##        jmp short cgic_error 



ASM_GLOBAL ASM_PFX(ConfigDfx)
ASM_PFX(ConfigDfx):
  #
  # Config package BSP CPU  DEBUG_INTERFACE_MSR.Enable bit.
  #        Note:  BIOS is only allowed to write 1 to this bit (never 0).
  #
  # Input: Stack NOT available
  #        MMX3 = routine return address
  # 

        movl  $1, %eax
        cpuid
        testl $BIT11, %ecx                               # Is  DEBUG_INTERFACE_MSR supported?
        jz    ConfigDfxExit                             # No

        CALL_MMX5  GetPlatformCpuSettings
        testl $CPU_OPTIONS_BIT_DEBUG_INTERFACE_EN, %ecx  # 1 -> Enable DEBUG_INTERFACE 
        jz    ConfigDfxExit                             # 0 -> Leave this MSR alone (as is)

        movl  $MSR_IA32_DEBUG_INTERFACE, %ecx             # Read the MSR
        rdmsr
        testl $BIT30, %eax                               # Lock bit set?
        jnz  ConfigDfxExit                              # yes 
        testl $BIT0, %eax                                # Enable bit set already?
        jnz  ConfigDfxExit                              # yes

        orl  $BIT0, %eax                                 # set ENABLE bit = 1
        wrmsr                                           # write back to MSR

ConfigDfxExit: 

        RET_ESI_MMX3



ASM_GLOBAL ASM_PFX(ConfigDcuMode)
ASM_PFX(ConfigDcuMode):
  #
  # Config package BSP CPU DCU_MODE (32KB 8-way v.s. 16KB 4-way ECC)
  # This must be done before CR0.CD is changed from 1 to 0 for the first time after reset, i.e., before NEM init.
  # Refer to DCU_MODE MSR in CPU BWG
  #
  # Input: Stack NOT available
  #        MMX3 = routine return address
  # 
        movl  $MSR_PLATFORM_INFO, %ecx        # Check if CPU supports DCU Mode select
        rdmsr
        test  $(1 << 26), %eax
        jz    ConfigDcuModeExit              # Skip if not

        CALL_MMX5  GetPlatformCpuSettings
        testl $CPU_OPTIONS_BIT_DCU_MODE_SEL, %ecx # 0/1 -> 32KB 8-way(HW default) / 16KB 4-way ECC
        jz    ConfigDcuModeExit              # JIf zero

        movl  $MSR_DCU_MODE, %ecx
        rdmsr                                # read MSR_DCU_MODE
        orb   $BIT0, %al                                         # set DCU_MODE bit
        wrmsr

ConfigDcuModeExit: 

        RET_ESI_MMX3

#ConfigDcuMode ENDP



ASM_PFX(FindFreeMtrr):
#************************************************************
# Input: Stack NOT available
#        MMX4 = routine return address
# Output:
#        ECX = MSR address of the next free MTRR   PHYS_BASE
#
# Regsters changed:
#        EAX, EBX, ECX, EDX, ESI
#************************************************************

#
#  Find total number of MTRRs supported
#
  movl   $MTRR_CAP, %ecx
  rdmsr
  movzbl %al, %esi                  # ESI  = number of variable MTRR pairs
  shll   %esi                       # ESI x 2
  addl   $MTRR_PHYS_BASE_0, %esi     # ESI  = (Addr of last MTRR) +1

  movl   $MTRR_PHYS_MASK_0, %ecx     # Start with MTRR 0
1: 
  rdmsr
  testl  $BIT11, %eax                # Valid = 1?
  jz     FoundFreeMtrr
  addl   $2, %ecx                   # next MTRR
  cmpl   %esi, %ecx                 # all available MTRRs checked?
  jb     1b                         # No, keep searching
  #
  # No available MTRR, halt system
  #
  PORT80 (NEM_NO_FREE_MTRR_ERROR_CODE)
  jmp    .

FoundFreeMtrr: 
  decl   %ecx                       # return ECX = MTRR PHYS_BASE address

  RET_ESI_MMX4




ASM_GLOBAL ASM_PFX(NemInit)
ASM_PFX(NemInit):
#
#************************************************************
#
# Input: Stack NOT available
#        MMX3 = routine return address
#
# Description:
#
#   This function initializes the Cache for Data, Stack, and Code
#   as specified in the  BIOS Writers Guide.
#************************************************************
#
# Check CPUID if support Boot Guard feature
#

  movl    $BIT0, %eax                  # To get CPU signature.
  cpuid                               # EAX = CPU signature.
  andl    $0xFFFF0, %eax
  shrl    $4, %eax
  cmpl    $CPU_FAMILY_SKX, %eax        # Check SKX cpu
  jne     SkipDetectBootGuard

#
# Detect Boot Guard Boot
#
DetectBootGuard:  
  movl    $MSR_BOOT_GUARD_SACM_INFO, %ecx # 
  rdmsr
  testl   $BIT0, %eax                    # BootGuard status = 1?
  jnz     BootGuardNemSetup             # Yes

SkipDetectBootGuard: 
#
#  Normal flow without BootGuard handling
#
  movl $MTRR_CAP, %ecx
  rdmsr
  movzbl %al, %eax                      # EAX = number of variable MTRR pairs
  lea  FixedMtrrSize(,%eax,4), %ebx
                                        # to program: 4 * VariableMtrrPairs + FixedMtrrSize
  xorl %eax, %eax                       # EAX = 0
  xorl %edx, %edx                       # EDX = 0

  GET_ABS_ADDR MtrrInitTable

InitMtrrLoop: 
  subl $2, %ebx
  movzwl %cs:(%esi,%ebx,), %ecx            # ECX <- address of MTRR to zero
  wrmsr
  jnz  InitMtrrLoop                     # loop through the whole table

  #
  # Configure the default memory type to un-cacheable (UC) in the 
  # IA32_MTRR_DEF_TYPE MSR.
  #
  movl    $MTRR_DEF_TYPE, %ecx           # Load the MTRR default type index
  rdmsr
  andl    $(~0x00000CFF), %eax
  wrmsr

  # Configure MTRR_PHYS_MASK_HIGH for proper addressing above 4GB
  # based on the physical address size supported for this processor
  # This is based on read from CPUID EAX = 080000008h, EAX bits [7:0]
  #
  # Examples:
  #  MTRR_PHYS_MASK_HIGH = 00000000Fh  For 36 bit addressing
  #  MTRR_PHYS_MASK_HIGH = 0000000FFh  For 40 bit addressing
  #
  movl    $0x80000008, %eax               # Address sizes leaf
  cpuid
  subb    $32, %al
  movzbl  %al, %eax
  xorl    %esi, %esi
  btsl    %eax, %esi
  decl    %esi                          # esi <- MTRR_PHYS_MASK_HIGH

  #
  # Configure the DataStack region as write-back (WB) cacheable memory type
  # using the variable range MTRRs.
  #

  #
  # Set the base address of the DataStack cache range
  #
  mov     $(DATA_STACK_BASE_ADDRESS | MTRR_MEMORY_TYPE_WB), %eax
                                        # Load the write-back cache value
  xorl    %edx, %edx                    # clear upper dword
  movl    $MTRR_PHYS_BASE_0, %ecx        # Load the MTRR index
  wrmsr                                 # the value in MTRR_PHYS_BASE_0

  #
  # Set the mask for the DataStack cache range
  #
  movl    $(DATA_STACK_SIZE_MASK | MTRR_PHYS_MASK_VALID), %eax
                                        # turn on the Valid flag
  movl    %esi, %edx                    # edx <- MTRR_PHYS_MASK_HIGH
  movl    $MTRR_PHYS_MASK_0, %ecx        # For proper addressing above 4GB
  wrmsr                                 # the value in MTRR_PHYS_BASE_0

  #
  # Configure the BIOS code region as write-protected (WP) cacheable 
  # memory type using a single variable range MTRR.
  #
  # Platform Specific - ensure region to cache meets MTRR requirements for 
  # size and alignment.
  #

  #
  # Set the base address of the CodeRegion cache range
  #
#ifdef MINIBIOS_BUILD
  movl    $CODE_REGION_SIZE, %eax
  movl    $CODE_REGION_BASE_ADDRESS, %edi
#else
  movl    PcdGet32 (PcdFlashSecCacheRegionSize), %eax
  movl    PcdGet32 (PcdFlashSecCacheRegionBase), %edi
#endif

  #
  # Round up to page size
  #
  movl    %eax, %ecx                    # Save
  andl    $0xFFFF0000, %ecx             # Number of pages in 64K
  andl    $0xFFFF, %eax                 # Number of "less-than-page" bytes
  jz      rounded
  movl    $0x10000, %eax                # Add the whole page size

rounded: 
  addl    %ecx, %eax                    # eax - rounded up code cache size

  #
  # Define "local" vars for this routine
  #
.equ      CODE_SIZE_TO_CACHE, %mm4
.equ      CODE_BASE_TO_CACHE, %mm5
.equ      NEXT_MTRR_INDEX,    %mm6
.equ      NEXT_MTRR_SIZE,     %mm7
.equ      CODE_SIZE_TO_CACHE, %mm4
.equ      CODE_BASE_TO_CACHE, %mm5
.equ      NEXT_MTRR_INDEX,    %mm6
.equ      NEXT_MTRR_SIZE,     %mm7
  #
  # Initialize "locals"
  #
  subl    %ecx, %ecx
  movd    %ecx, NEXT_MTRR_INDEX         # Count from 0 but start from MTRR_PHYS_BASE_1

  #
  # Save remaining size to cache
  #
  movd    %eax, CODE_SIZE_TO_CACHE      # Size of code cache region that must be cached
  movd    %edi, CODE_BASE_TO_CACHE      # Base code cache address

nextMtrr: 
  #
  # Get remaining size to cache
  #
  movd    CODE_SIZE_TO_CACHE, %eax
  andl    %eax, %eax
  jz      done                          # If no left size - we are done
  #
  # Determine next size to cache.
  # We start from bottom up. Use the following algorythm:
  # 1. Get our own alignment. Max size we can cache equals to our alignment
  # 2. Determine what is bigger - alignment or remaining size to cache.
  #    If aligment is bigger - cache it.
  #      Adjust remaing size to cache and base address
  #      Loop to 1.
  #    If remaining size to cache is bigger
  #      Determine the biggest 2^N part of it and cache it.
  #      Adjust remaing size to cache and base address
  #      Loop to 1.
  # 3. End when there is no left size to cache or no left MTRRs
  #
  movd    CODE_BASE_TO_CACHE, %edi
  bsfl    %edi, %ecx                    # Get index of lowest bit set in base address
  #
  # Convert index into size to be cached by next MTRR
  #
  movl    $0x1, %edx
  shl     %cl, %edx                     # Alignment is in edx
  cmpl    %eax, %edx                    # What is bigger, alignment or remaining size?
  jbe     gotSize                       # JIf aligment is less
  #
  # Remaining size is bigger. Get the biggest part of it, 2^N in size
  #
  bsrl    %eax, %ecx                    # Get index of highest set bit
  #
  # Convert index into size to be cached by next MTRR
  #
  movl    $1, %edx
  shl     %cl, %edx                     # Size to cache

gotSize: 
  movl    %edx, %eax
  movd    %eax, NEXT_MTRR_SIZE          # Save

  #
  # Compute MTRR mask value:  Mask = NOT (Size - 1)
  #
  decl    %eax                          # eax - size to cache less one byte
  notl    %eax                          # eax contains low 32 bits of mask
  orl     $MTRR_PHYS_MASK_VALID, %eax    # Set valid bit

  #
  # Program mask register
  #
  movl    $MTRR_PHYS_MASK_1, %ecx        # setup variable mtrr
  movd    NEXT_MTRR_INDEX, %ebx
  addl    %ebx, %ecx

  movl    %esi, %edx                    # edx <- MTRR_PHYS_MASK_HIGH
  wrmsr
  #
  # Program base register
  #
  subl    %edx, %edx
  movl    $MTRR_PHYS_BASE_1, %ecx        # setup variable mtrr
  addl    %ebx, %ecx                    # ebx is still NEXT_MTRR_INDEX

  movd    CODE_BASE_TO_CACHE, %eax
  orl     $MTRR_MEMORY_TYPE_WP, %eax     # set type to write protect
  wrmsr
  #
  # Advance and loop
  # Reduce remaining size to cache
  #
  movd    CODE_SIZE_TO_CACHE, %ebx
  movd    NEXT_MTRR_SIZE, %eax
  subl    %eax, %ebx
  movd    %ebx, CODE_SIZE_TO_CACHE

  #
  # Increment MTRR index
  #
  movd    NEXT_MTRR_INDEX, %ebx
  addl    $2, %ebx
  movd    %ebx, NEXT_MTRR_INDEX
  #
  # Increment base address to cache
  #
  movd    CODE_BASE_TO_CACHE, %ebx
  movd    NEXT_MTRR_SIZE, %eax
  addl    %eax, %ebx
  movd    %ebx, CODE_BASE_TO_CACHE

  jmp     nextMtrr
done: 
  #
  # Enable the MTRRs by setting the IA32_MTRR_DEF_TYPE MSR E flag.
  #
  movl    $MTRR_DEF_TYPE, %ecx           # Load the MTRR default type index
  rdmsr
  orl     $MTRR_DEF_TYPE_E, %eax         # Enable variable range MTRRs
  wrmsr

  #
  # Need to disable fast strings to prevent ItoM transition which leads to illegal WbtoMmio in NEM mode
  #
  movl    $IA32_MISC_ENABLE, %ecx
  rdmsr
  andl    $(~FAST_STRING_ENABLE_BIT), %eax
  wrmsr

  #
  # Enable the logical processors (BSP) cache: execute INVD and set 
  # CR0.CD = 0, CR0.NW = 0.
  #
  movl    %cr0, %eax
  andl    $(~(CR0_CACHE_DISABLE + CR0_NO_WRITE)), %eax
  invd
  movl    %eax, %cr0
  #
  # Enable No-Eviction Mode Setup State by setting
  # NO_EVICT_MODE  MSR 2E0h bit [0] = '1'.
  #
  movl    $NO_EVICT_MODE, %ecx
  rdmsr
  orl     $1, %eax
  wrmsr

  #
  # One location in each 64-byte cache line of the DataStack region
  # must be written to set all cache values to the modified state.
  #
  movl    $DATA_STACK_BASE_ADDRESS, %edi
  movl    $(DATA_STACK_SIZE / 64), %ecx
  movl    $CACHE_INIT_VALUE, %eax
1: 
  movl    %eax, (%edi)
  sfence
  addl    $64, %edi
  loop    1b

  #
  # Enable No-Eviction Mode Run State by setting
  # NO_EVICT_MODE MSR 2E0h bit [1] = '1'.
  #
  movl    $NO_EVICT_MODE, %ecx
  rdmsr
  orl     $2, %eax
  wrmsr

  #
  # Finished with cache configuration
  #

  jmp     FinishedCacheConfig

BootGuardNemSetup: 
  #
  #Ccontrol comes here when Boot Guard boot and NEM is initialized by Boot Guard ACM
  #
  # Configure MTRR_PHYS_MASK_HIGH for proper addressing above 4GB
  # based on the physical address size supported for this processor
  # This is based on read from CPUID EAX = 080000008h, EAX bits [7:0]
  #
  # Examples: 
  #  MTRR_PHYS_MASK_HIGH = 00000000Fh  For 36 bit addressing
  #  MTRR_PHYS_MASK_HIGH = 0000000FFh  For 40 bit addressing
  #
  movl  $0x80000008, %eax               # Address sizes leaf
  cpuid
  subb  $32, %al
  movzbl %al, %eax
  xorl  %esi, %esi
  btsl  %eax, %esi
  decl  %esi                            # esi <- MTRR_PHYS_MASK_HIGH
  movl  %esi, %edi                      # edi <- MTRR_PHYS_MASK_HIGH

  #
  # Configure the DataStack region as write-back (WB) cacheable memory type
  # using the variable range MTRRs.
  #
  #
  # Find available MTRR
  #
  CALL_MMX4    FindFreeMtrr

  #
  # Set the base address of the DataStack cache range
  #
  movl    $(DATA_STACK_BASE_ADDRESS | MTRR_MEMORY_TYPE_WB), %eax
                                        # Load the write-back cache value
  xorl    %edx, %edx                    # clear upper dword
  wrmsr                                 # the value in MTRR_PHYS_BASE_0  

  #
  # Set the mask for the DataStack cache range
  # Compute MTRR mask value:  Mask = NOT (Size - 1)
  #
  incl    %ecx                          # ecx = PHYS_MASK msr
  movl    $(DATA_STACK_SIZE_MASK | MTRR_PHYS_MASK_VALID), %eax
                                        # turn on the Valid flag
  movl    %edi, %edx                    # edx <- MTRR_PHYS_MASK_HIGH
  wrmsr

  #
  # Need to disable fast strings to prevent ItoM transition which leads to illegal WbtoMmio in NEM mode
  #
  movl    $IA32_MISC_ENABLE, %ecx
  rdmsr
  andl    $(~FAST_STRING_ENABLE_BIT), %eax
  wrmsr

  #
  # One location in each 64-byte cache line of the DataStack region
  # must be written to set all cache values to the modified state.
  #
  movl    $DATA_STACK_BASE_ADDRESS, %edi
  movl    $(DATA_STACK_SIZE / 64), %ecx
  movl    $CACHE_INIT_VALUE, %eax
1: 
  movl    %eax, (%edi)
  sfence
  addl    $64, %edi
  loop    1b

  #
  # Finished with cache configuration
  #

FinishedCacheConfig: 

  #
  # Stop PBE timer if system is in Boot Guard boot
  #
  movl    $MSR_BOOT_GUARD_SACM_INFO, %ecx # 
  rdmsr
  testl   $BIT0, %edx                    # BootGuard Fuse = 1?
  jz      SkipPbet

  movl    $MSR_BC_PBEC, %ecx
  movl    $BIT0, %eax                    # Set B_STOP_PBET BIT0
  xorl    %edx, %edx
  wrmsr

SkipPbet: 
  #
  # Optionally test the Data Region by writing and reading
  #
  cld
  movl    $DATA_STACK_BASE_ADDRESS, %edi
  movl    $(DATA_STACK_SIZE / 4), %ecx
  movl    $CACHE_TEST_VALUE, %eax

  rep
  stosl
  movl    $DATA_STACK_BASE_ADDRESS, %edi
  movl    $(DATA_STACK_SIZE / 4), %ecx
  repe scasl
  jnz     DataStackTestFail
  jmp     DataStackTestPass

  #
  # Data Cache test failed
  #
DataStackTestFail: 
  PORT80 (NEM_DATA_RW_TEST_ERROR_CODE)
  jmp     .

DataStackTestPass: 

  #
  # At this point you may continue normal execution.  Typically this would include 
  # reserving stack, initializing the stack pointer, etc.
  #

  #
  # After memory initialization is complete, please follow the algorithm in the BIOS
  # Writers Guide to properly transition to a normal system configuration.
  # The algorithm covers the required sequence to properly exit this mode.
  #


   RET_ESI_MMX3


ASM_GLOBAL ASM_PFX(EstablishNemStack)
ASM_PFX(EstablishNemStack):
        # Establish stack
        #
        # Fill the stack with a recognizable pattern.
        #
        movl  $DATA_STACK_BASE_ADDRESS, %esp
        addl  $DATA_STACK_SIZE, %esp
        subl  $0x40, %esp
        movl  $DATA_STACK_BASE_ADDRESS, %edi
        cld
        movl  $DATA_STACK_SIZE, %ecx
        shrl  $2, %ecx
        movl  $0x5AA55AA5, %eax
        rep
        stosl

        RET_ESI_MMX3


MtrrInitTable:
.word MTRR_DEF_TYPE
.word MTRR_FIX_64K_00000
.word MTRR_FIX_16K_80000
.word MTRR_FIX_16K_A0000
.word MTRR_FIX_4K_C0000
.word MTRR_FIX_4K_C8000
.word MTRR_FIX_4K_D0000
.word MTRR_FIX_4K_D8000
.word MTRR_FIX_4K_E0000
.word MTRR_FIX_4K_E8000
.word MTRR_FIX_4K_F0000
.word MTRR_FIX_4K_F8000

.equ           FixedMtrrSize, (. - MtrrInitTable)

.word MTRR_PHYS_BASE_0
.word MTRR_PHYS_MASK_0
.word MTRR_PHYS_BASE_1
.word MTRR_PHYS_MASK_1
.word MTRR_PHYS_BASE_2
.word MTRR_PHYS_MASK_2
.word MTRR_PHYS_BASE_3
.word MTRR_PHYS_MASK_3
.word MTRR_PHYS_BASE_4
.word MTRR_PHYS_MASK_4
.word MTRR_PHYS_BASE_5
.word MTRR_PHYS_MASK_5
.word MTRR_PHYS_BASE_6
.word MTRR_PHYS_MASK_6
.word MTRR_PHYS_BASE_7
.word MTRR_PHYS_MASK_7
.word MTRR_PHYS_BASE_8
.word MTRR_PHYS_MASK_8
.word MTRR_PHYS_BASE_9
.word MTRR_PHYS_MASK_9



#-------------------------------------------------------------------------
# TxtInit
#   LTsx-related CPU early init that must be executed by all package BSPs
#
# Input: 
#   AccessCSR proc has been executed
#   No Eviction Mode (NEM) is enabled, stack is available
# Output: 
#-------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(TxtInit)
ASM_PFX(TxtInit):

     .equ                   TXT_PUBLIC_BASE, 0x0FED30000
     .equ                   TPM_STATUS_REG_ADDRESS, 0x0FED40000

    #
    # Check if CPU is fused for LTSX support
    #
    movl    $1, %eax
    cpuid
    testl   $BIT6, %ecx
    jz      TxtInitExit               # no, skip

    #
    # Check local LT_CONTROL.lt_target_enable (bit 4)
    #
    movl    $CSR_LT_CONTROL_CHABC, %eax
    call    ReadCpuCsr                # EAX = current CSR data
    btl     $4, %eax
    jnc     TxtInitExit

    #
    # Check EXISTS registe TXT_PUBLIC_BASE + 10h  to verify 
    # If the thread is registered as LT capable (each socket has a corresponding bit) 
    #
    call   GetLocalSktId              # CL = own socket ID (0, 1, 2, 3)
    movl   $TXT_PUBLIC_BASE, %esi
    movl   %ds:0x10(%esi), %eax
    btl    %ecx, %eax
    jnc    TxtInitExit                # no, skip

    #
    #  Do a read to TPM Status (and drop the data).
    #  If 1, then hardware will clear WAKE-ERROR.STS bits in TXT public reg space offset 08h[6]
    # 
    movl   $TPM_STATUS_REG_ADDRESS, %esi
    movl   %ds:(%esi), %eax

    #
    #  Unlock memory by writing 0 to MSR 2E6h
    #

    movl    $0x2E6, %ecx
    xorl    %eax, %eax
    xorl    %edx, %edx
    wrmsr

TxtInitExit: 
    ret


#-------------------------------------------------------------------------
# Read a Dword from PCI register 00h thru FFh using IO port CF8h/CFCh
# Assumptions: 
#   No Eviction Mode (NEM) is enabled, stack is available
# Input: 
#   EAX = legacy PCI address format
#   EAX[31:24] = 80h
#   EAX[23:16] = Bus #
#   EAX[15:11] = Dev #
#   EAX[10:8]  = Func #
#   EAX[7:0]   = Reg offset (dword aligned)
#   
# Output: EAX = regiser dword data
# Registers changed: None except EAX
#-------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(ReadPciLegacy)
ASM_PFX(ReadPciLegacy):

   push %edx
   movw  $0xcf8, %dx
   outl  %eax, %dx
   movw  $0xcfc, %dx
   in    %dx, %eax
   popl  %edx
   ret

#-------------------------------------------------------------------------
# Write a Dword to a PCI register 00h thru FFh using IO port CF8h/CFCh
# Assumptions: 
#   No Eviction Mode (NEM) is enabled, stack is available
# Input: 
#   EAX = legacy PCI address format
#   EAX[31:24] = 80h
#   EAX[23:16] = Bus #
#   EAX[15:11] = Dev #
#   EAX[10:8]  = Func #
#   EAX[7:0]   = Reg offset (dword aligned)
#   EBX = dword data to be written
# Output: None
# Registers changed: None
#-------------------------------------------------------------------------

ASM_GLOBAL ASM_PFX(WritePciLegacy)
ASM_PFX(WritePciLegacy):

   pushl %edx
   movw  $0xcf8, %dx
   outl  %eax, %dx
   movw  $0xcfc, %dx
   xchgl %ebx, %eax
   outl  %eax, %dx
   xchgl %ebx, %eax
   popl  %edx

   ret
