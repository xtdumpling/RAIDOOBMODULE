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
#  ProcessorStartup.asm
#
# Abstract:
#
#
#------------------------------------------------------------------------------        
        .include "ProcessorStartupPlatform.i"
        .include "ProcessorStartupChipset.i"
        .include "ProcessorStartup.i"

        .extern CheckGenuineIntelCpu
        .extern EarlyCoreInit
        .extern EarlyUncoreInit
        .extern EarlyChipsetInit
        .extern LoadProcessorMicrocode
        .extern MidUncoreInit
        .extern MidChipsetInit
        .extern PreNemUncoreInit
        .extern ConfigDfx
        .extern ConfigDcuMode
        .extern NemInit
        .extern EstablishNemStack
        .extern TxtInit
        .extern PostNemUncoreInit
        .extern SetupProcessorCheckinData
        .extern AmILegacySocket
        .extern UncoreBusCfgSyncNbsp
        .extern ParkNbsp
        .extern LateChipsetInit
        .extern EnableAccessGlobalCSR

#ifdef MINIBIOS_BUILD
        .extern _CpuStart
#endif
        .extern EarlyChpsetInit_OemHook
        .extern LateChipsetInit_OemHook


#----------------------------------------------------------------------------
#       STARTUP_SEG  S E G M E N T  STARTS
#----------------------------------------------------------------------------
.code32

ASM_GLOBAL ASM_PFX(_ProcessorStartup)
ASM_PFX(_ProcessorStartup):

        movd    %esi, %mm1
        movd    %edi, %mm2

#       mm0 : Assumed to be BIST state (not used by this module, but cant be destroyed)
#       mm1 : _ProcessorStartup return address
#       mm2 : LAST_ADDRESS of decode region
#       mm3 : Used for stack by CALL_MMX3/RET_ESI_MMX3 macro
#       mm4 : Used for stack by CALL_MMX4/RET_ESI_MMX4 macro
#       mm5 : Used for stack by CALL_MMX5/RET_ESI_MMX5 macro
#       mm6 : Used for stack by CALL_MMX6/RET_ESI_MMX6 macro

        PORT80 (POWER_ON_POST_CODE)
        CALL_MMX3   CheckGenuineIntelCpu
        CALL_MMX3   EarlyCoreInit
        CALL_MMX3   EarlyUncoreInit
#
# At this point,  MMCFG_RULE in CHA is enabled. MMCFG_TARGET_LIST points to local socket only,
# threfore inter-socket CSR accesses are not enabled yet.
# Local socket CSRs can be accessed via either CF8/CFC IO or mmcfg methods.
# CPUBUSNO in UBOX is not enabled, so defalut bus# for local targets (stacks) must be used.
#
        CALL_MMX3   ConfigDfx
        CALL_MMX3   EarlyChipsetInit

        PORT80 (MICROCODE_POST_CODE)
        #
        #OEM_HOOK after EarlyChipsetInit
        #
        CALL_MMX3 EarlyChpsetInit_OemHook
        #
        #
        CALL_MMX3   LoadProcessorMicrocode

        CALL_MMX3   MidUncoreInit
        CALL_MMX3   MidChipsetInit

        PORT80 (NEM_CACHE_INIT_POST_CODE)

        CALL_MMX3   PreNemUncoreInit
        CALL_MMX3   ConfigDcuMode
        CALL_MMX3   NemInit

        CALL_MMX3   EstablishNemStack

        # After this point functions must be called using stack
        #  Functions after here may NOT destroy ESP!

        PORT80 (NEM_CACHE_ENABLED_POST_CODE)
        call    TxtInit
        call    PostNemUncoreInit
        call    SetupProcessorCheckinData

        PORT80 (SBSP_PBSP_BRANCH_POST_CODE)
        #
        # Early SBSP Selection
        #
        call    AmILegacySocket                    # Am I legacy-socket?
        jz      Sbsp                               # Yes
Pbsp:
        #
        # PBSPs come here...
        #
        call    UncoreBusCfgSyncNbsp               # Wait for SBSP to globally program MMCFG/BUSNUM  on all PBSP sockets
        call    ParkNbsp                           # Loop for SBSP command to proceed. PBSPs will not return after the call

Sbsp:
        #
        # Only SBSP comes here to continue. All other package BSPs are held in a loop
        #
        call    LateChipsetInit
        #
        #OEM_HOOK after LateChipsetInit
        #
        call    LateChipsetInit_OemHook
        #
        # SBSP enables inter-socket global CSR access
        #
        call    EnableAccessGlobalCSR
        PORT80 (SEC_CPU_INIT_END_POST_CODE)

#ifdef MINIBIOS_BUILD
        # OEM hook to call reference code
        call    _CpuStart
#endif

        movd    %mm1, %esi
        jmp     *%esi

