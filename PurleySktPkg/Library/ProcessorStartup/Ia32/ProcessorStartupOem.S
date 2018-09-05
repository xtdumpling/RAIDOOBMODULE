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
#  ProcessorStartupOem.asm
#
# Abstract:
#
#
#------------------------------------------------------------------------------

#ifdef MINIBIOS_BUILD
        .extern PcdGet32 (PcdFlashNvStorageMicrocodeBase)
        .extern PcdGet32 (PcdFlashNvStorageMicrocodeSize)
#endif
        .extern AmILegacySocketNoStack
        .extern GetMmcfgNoStack
        .include "ProcessorStartupPlatform.i"
        .include "ProcessorStartupUncore.i"
        .include "ProcessorStartup.i"

.code32

#-------------------------------------------------------
# Microcode Update region
#-------------------------------------------------------

# Microcode updates must be aligned on a paragraph boundary. If more
# than microcode update is included in the microcode storage block,
# each microcode update be forced to to align on paragraph boundary.
#ifdef MINIBIOS_BUILD
.align 16
.global microcode_region_start
.global microcode_region_size
# This is the microcode update region that will be searched and loaded
# by CPU reference code.

microcode_region_start:
   # Include the first microcode update here
   #
    #include "ucode/null_patch_bios_skx.i"            # SKX Pre-Si dummy patch

.align 16
   # Include the next microcode update here
   #
    #include "ucode/mef306f1_80000013.i"              # HSX B0 Stepping

.align 16
   # Include the next microcode update here
   #
    #include "ucode/mef306f2_c0000007.i"              # HSX C0 Stepping

.align 16
   # Include the next microcode update here
   #
.equ                  microcode_region_size, (.-microcode_region_start)   # size of entire microcode update region above
#endif
#------------------------------------------------------------------
# ROM-based options. Values in each field can be customized by OEM
#------------------------------------------------------------------
#ifdef MINIBIOS_BUILD

oem_cpu_rom_setup_options:
.long cpu_rom_setup_options
.long microcode_region_start
.long microcode_region_size
.long CODE_REGION_BASE_ADDRESS
.long CODE_REGION_SIZE
.long DATA_STACK_BASE_ADDRESS
.long DATA_STACK_SIZE

#endif
#----------------------------------------------------------------------------
#       STARTUP_SEG  S E G M E N T  STARTS
#----------------------------------------------------------------------------
#ifdef MINIBIOS_BUILD


#-----------------------------------------------------------------
#   Executed by SBSP and NBSP
#   Returns build-time platform options related to CPU early init
# Input:
#   Stack not available
#   mmx6 = routine return address
# Output:
#   ebp = pointer to structure cpu_rom_setup_option
#   All other registers destroyed
#-----------------------------------------------------------------

ASM_GLOBAL ASM_PFX(oem_get_cpu_rom_setup_options)
ASM_PFX(oem_get_cpu_rom_setup_options):

   # OEM can override this default implementation

   movl  oem_cpu_rom_setup_options, %ebp
   retl %ebp                      # return to caller

#endif


#-----------------------------------------------------------------
# This routine returns CPU-specific platform setup options/settings 
# to the caller via ECX register
#-----------------------------------------------------------------
#   Executed by SBSP and NBSP
# Input:
#   Stack not available
#   mmx5 = routine return address
#   MMCFG/CPUBUSNO has been configured so node BSPs can access CSRs on socket 0, and vice-versa
#   xmm registers initialized by EnableAccessCSR routine
#      xmm1 register [31:24] contains local Socket ID
#      xmm3 register contains MMCFG_RULE
#   xmm4,xmm6 registers contain the IIO bus number array initialized by EnableAccessCSR routine
#       xmm4: socket 0 - 3, xmm6: 4 - 7
#   xmm5,xmm7 registers contain the Uncore bus number array initialized by EnableAccessCSR routine
#       xmm5: socket 0 - 3, xmm7: 4 - 7
#  Output: 
#  Output: 
#    ECX[0]  - DCU_MODE select   0/1:   32KB 8-way non-ECC (hardware default) / 16KB 4-way ECC 
#    ECX[1]  - debugInterfaceEn   1:   Enable Debug Interface
#    ECX[31:2] reserved
#   All other general purpose registers destroyed
#-----------------------------------------------------------------

ASM_GLOBAL ASM_PFX(GetPlatformCpuSettings)
ASM_PFX(GetPlatformCpuSettings):

#
# DCU_MODE Select setup data is stored(copied) into sticky scratch pad register SSR07[24] on the warm boot path.
# Read the scratch pad register and returns the DCU_MODE setting to caller
#
   CALL_MMX6  GetMmcfgNoStack
   addl  $CSR_BIOSSCRATCHPAD7, %edi
   xorl  %edx, %edx

   movl  %ds:(%edi), %ebx                          # ebx = CSR data

   testl $BIT5, %ebx                                # "Comlepted Cold Reset Flow"?
   jz    gdms_50                                   # no
   testl $BIT27, %ebx                               # ebx[27] = debugInterfaceEn info saved from cold boot path
   jz    1f
   orl   $CPU_OPTIONS_BIT_DEBUG_INTERFACE_EN, %edx  # return debugInterfaceEn = 1
1: 
   testl $BIT24, %ebx                               # ebx[24] = DCU Mode Select info saved from cold boot path
   jz    gdms_50
   orl   $CPU_OPTIONS_BIT_DCU_MODE_SEL, %edx        # return DCU_MODE select = 1

gdms_50: 
   movl  %edx, %ecx                                # return data in ecx
   RET_ESI_MMX5


# Stub dummy hook for MiniBIOS implementation
ASM_GLOBAL ASM_PFX(_OEMCpuSecInit)
ASM_PFX(_OEMCpuSecInit):
       ret

#-----------------------------------------------------------------
# This routine returns the address of CPU microcode update
#-----------------------------------------------------------------
#   Executed by SBSP and NBSP
# Input:
#   Stack not available
#   mmx4 = routine return address
# Output:
#   EAX = Zero       No matching microcode found
#   EAX = Non-Zero   Linear address of CPU microcode update header that matches the current CPU
#   All other registers destroyed
#-----------------------------------------------------------------
ASM_GLOBAL ASM_PFX(OemFindMicrocode)
ASM_PFX(OemFindMicrocode):

#ifdef MINIBIOS_BUILD
   CALL_MMX6   oem_get_cpu_rom_setup_options

   movl  (%ebp).cpu_rom_setup_options.ucode_region_addr, %esi # esi = Start address of ucode region
#else
   movl  PcdGet32 (PcdFlashNvStorageMicrocodeBase), %esi
   addl  $0x60, %esi #bypass fv header
#endif
   # Get processor signature and platform ID from the installed processor
   # and save into registers for later use
   # ebx = processor signature
   # edx = platform ID
   movl  $1, %eax
   cpuid
   movl  %eax, %ebx
   movl  $MSR_IA32_PLATFORM_ID, %ecx
   rdmsr
   movl  %edx, %ecx
   shrl  $(50-32), %ecx
   andl  $0x7, %ecx
   movl  $1, %edx
   shl   %cl, %edx

   # Current register usage
   # esi -> microcode update to check
   # ebx = processor signature
   # edx = platform ID

check_main_header: 
   # Check for valid microcode header
   # Minimal test checking for header version and loader version as 1
   movl  $1, %eax
   cmpl  %eax, UpdateHeaderStruc_dHeaderVersion(%esi)
   jne   advance_fixed_size
   cmpl  %eax, UpdateHeaderStruc_dLoaderRevision(%esi)
   jne   advance_fixed_size

   # Check if signature and plaform ID match
   cmpl  %ebx, UpdateHeaderStruc_dProcessorSignature(%esi) # reversed operands
   jne   1f
   testl %edx, UpdateHeaderStruc_dProcessorFlags(%esi) # reversed operands
   jnz   load_check  # Jif signature and platform ID match

1: 
   # Check if extended header exists
   # First check if total_size and data_size are valid
   xorl  %eax, %eax
   cmpl  %eax, UpdateHeaderStruc_dTotalSize(%esi)
   je    next_microcode
   cmpl  %eax, UpdateHeaderStruc_dDataSize(%esi)
   je    next_microcode

   # Then verify total size - sizeof header > data size
   movl UpdateHeaderStruc_dTotalSize(%esi), %ecx
#   movl  4(%esi), %ecx
   subl  $UpdateHeaderStruc_END, %ecx
   cmpl  UpdateHeaderStruc_dDataSize(%esi), %ecx
   jbe   next_microcode    # Jif extended header does not exist

   # Check if total size fits in microcode region
   movl  %esi, %edi
   addl  UpdateHeaderStruc_dTotalSize(%esi), %edi
#ifdef MINIBIOS_BUILD
   movl  cpu_rom_setup_options_ucode_region_addr(%ebp), %ecx
   addl  cpu_rom_setup_options_ucode_region_size(%ebp), %ecx
#else
   mov   PcdGet32 (PcdFlashNvStorageMicrocodeBase), %ecx
   add   PcdGet32 (PcdFlashNvStorageMicrocodeSize), %ecx
#endif
   cmpl  %ecx, %edi
   ja    exit_not_found              # Jif address is outside of ucode region

   # Set edi -> extended header
   movl  %esi, %edi
   addl  $UpdateHeaderStruc_END, %edi
   addl  UpdateHeaderStruc_dDataSize(%esi), %edi

   # Get count of extended structures
   movl  ext_sig_hdr_count(%edi), %ecx

   # Move pointer to first signature structure
   addl  $ext_sig_hdr_END, %edi

check_ext_sig: 
   # Check if extended signature and platform ID match
   cmpl  %ebx, ext_sig_processor(%edi)
   jne   1f
   testl %edx, ext_sig_flags(%edi)
   jnz   load_check     # Jif signature and platform ID match
1: 
   # Check if any more extended signatures exist
   addl  $ext_sig_END, %edi
   loop  check_ext_sig

next_microcode: 
   # Advance just after end of this microcode
   xorl  %eax, %eax
   cmpl  %eax, UpdateHeaderStruc_dTotalSize(%esi)
   je    2f
   addl  UpdateHeaderStruc_dTotalSize(%esi), %esi
   jmp   check_address
2: 
   addl  $2048, %esi
   jmp   check_address

advance_fixed_size: 
   # Advance by 4X dwords
   addl  $1024, %esi

check_address: 
   # Address >= microcode region address + microcode region size?
#ifdef MINIBIOS_BUILD
   movl  cpu_rom_setup_options_ucode_region_addr(%ebp), %eax
   addl  cpu_rom_setup_options_ucode_region_size(%ebp), %eax
#else
   movl  PcdGet32 (PcdFlashNvStorageMicrocodeBase), %eax
   addl  PcdGet32 (PcdFlashNvStorageMicrocodeSize), %eax
#endif
   cmpl  %eax, %esi
   jae   exit_not_found        #Jif address is outside of ucode region
   jmp   check_main_header


load_check: 
   # Get the revision of the current microcode update loaded
    movl $MSR_IA32_BIOS_SIGN_ID, %ecx
    xorl  %eax, %eax              # Clear EAX
    xorl  %edx, %edx              # Clear EDX
    wrmsr                         # Load 0 to MSR at 8Bh
    movl  $1, %eax
    cpuid
    movl  $MSR_IA32_BIOS_SIGN_ID, %ecx
    rdmsr                         # Get current microcode signature

   # Verify this microcode update is not already loaded
   cmpl  %edx, UpdateHeaderStruc_dUpdateRevision(%esi)
   je    exit_not_found

found_microcode: 
   movl  %esi, %eax               # EAX contains the linear address of the start of the Microcode Update Header
   jmp   exit

exit_not_found: 
   xorl  %eax, %eax               # ucode not found

exit: 
   RET_ESI_MMX4                   # return to caller
