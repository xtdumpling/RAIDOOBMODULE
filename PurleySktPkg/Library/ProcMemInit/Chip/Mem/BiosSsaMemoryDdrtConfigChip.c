/** @file
  This file contains the implementation of the BIOS version of the SSA services PPI.

  This version contains code for both client and server implementations.

  For Client:
    - Delete include of BiosSsaMemoryServerConfig.h and BiosSsaMemoryDdrtConfig.h.
    - Delete definitions of SSA_REVISION_MEMORY_SERVER and SSA_REVISION_MEMORY_DDRT.
    - Delete declarations of SsaMemoryServerConfig and SsaMemoryDdrtConfig.
    - Replace SsaMemoryServerConfig and SsaMemoryDdrtConfig field values in SsaBiosServicesPpi with NULL.

  For Server:
    - Delete include of BiosSsaMemoryClientConfig.h.
    - Delete definition of SSA_REVISION_MEMORY_CLIENT.
    - Delete declaration of SsaMemoryClientConfig.
    - Replace SsaMemoryClientConfig field value in SsaBiosServicesPpi with NULL.

@copyright
  Copyright (c) 2012 - 2016 Intel Corporation. All rights
  reserved. This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include "SysFunc.h"
#include "RcRegs.h"
#include "FnvAccess.h"

#ifdef SSA_FLAG

//
//This BSSA file contains subsets of all BSSA function calls made from the Core files; all CSR accesses are made here. This is DDRT Code and hence not available on Broadwell Server(BDX)
//

/**

  This routine sets the CSRs required by BiosSetEridValidationMask

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param EridMask                          - Early Read ID bitmask.  Bit value 1 = perform validation; bit value 0 = do not perform validation.  Bit position 3: CKP Early Read ID (even).  Bit position 2: CKP Early Read ID (odd).  Bit position 1: CKN Early Read ID (even).  Bit position 0: CKN Early Read ID (odd).

  @retval N/A

**/
VOID
BiosSetEridValidationMaskChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 ChannelInSocket,
  UINT8                                 EridMask
  )
{
  CPGC_ERID_LFSR_ERR_MC_2LM_STRUCT        cpgcEridLfsrErr;

  cpgcEridLfsrErr.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERID_LFSR_ERR_MC_2LM_REG);
  cpgcEridLfsrErr.Bits.mask = EridMask; //0x80;
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_ERID_LFSR_ERR_MC_2LM_REG, cpgcEridLfsrErr.Data);
}

/**

  This routine gets the CSRs required by BiosGetEridErrorStatus

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param Status                            - Pointer to where the Early Read ID bit error status will be stored.  Bit value 1 = at least one error occurred; bit value 0 = no errors occurred.  Bit position 3: CKP Early Read ID (even).  Bit position 2: CKP Early Read ID (odd).  Bit position 1: CKN Early Read ID (even).  Bit position 0: CKN Early Read ID (odd).
  @param Count                             - Pointer to where the error counter value will be stored.

  @retval N/A

**/
VOID
BiosGetEridErrorStatusChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 ChannelInSocket,
  UINT8                                 *Status,
  UINT16                                *Count
  )
{
  CPGC_ERID_LFSR_ERR_MC_2LM_STRUCT  cpgcEridLfsrErr;

  cpgcEridLfsrErr.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERID_LFSR_ERR_MC_2LM_REG);
  *Status = (UINT8)cpgcEridLfsrErr.Bits.status;
  *Count = (UINT16)cpgcEridLfsrErr.Bits.count;
}

/**

  This routine sets the CSRs required by BiosSetEridPattern

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param Signal                            - Zero based ERID signal number.
  @param PatternMode                       - LFSR or pattern buffer mode.
  @param EnableReload                      - Enable or disable seed reload for the Early Read ID.
  @param EnableReset                       - Enable or disable seed reset for the Early Read ID.
  @param Seed                              - Seed value for Early Read ID.

  @retval N/A

**/
VOID
BiosSetEridPatternChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 ChannelInSocket,
  ERID_SIGNAL                           Signal,
  ERID_PATTERN_MODE                     PatternMode,
  BOOLEAN                               EnableReload,
  BOOLEAN                               EnableReset,
  UINT32                                Seed
  )
{
  CPGC_ERID_LFSR_INIT0_MC_2LM_STRUCT        cpgcEridLfsrInit0;
  CPGC_ERID_LFSR_INIT1_MC_2LM_STRUCT        cpgcEridLfsrInit1;
  CPGC_ERID_LFSR_CTL_MC_2LM_STRUCT          cpgcEridLfsrCtl;

  cpgcEridLfsrCtl.Data =  MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERID_LFSR_CTL_MC_2LM_REG);

  if (Signal == EridSignal0) {
    cpgcEridLfsrCtl.Bits.mode0 = (PatternMode == EridLfsrMode)? 0 : 1; // 0 selects LFSR
    cpgcEridLfsrCtl.Bits.reset0 = (EnableReset)? 1 : 0;
    cpgcEridLfsrCtl.Bits.load0 = (EnableReload)? 1 : 0;

    // program the seed
    cpgcEridLfsrInit0.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERID_LFSR_INIT0_MC_2LM_REG);
    cpgcEridLfsrInit0.Bits.seed = Seed;
    MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_ERID_LFSR_INIT0_MC_2LM_REG, cpgcEridLfsrInit0.Data);
  } else {
    cpgcEridLfsrCtl.Bits.mode1 = (PatternMode == EridLfsrMode)? 0 : 1;  // 0 selects LFSR
    cpgcEridLfsrCtl.Bits.reset1 = (EnableReset)? 1 : 0;
    cpgcEridLfsrCtl.Bits.load1 = (EnableReload)? 1 : 0;

    // program the seed
    cpgcEridLfsrInit1.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERID_LFSR_INIT1_MC_2LM_REG);
    cpgcEridLfsrInit1.Bits.seed = Seed;
    MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_ERID_LFSR_INIT1_MC_2LM_REG, cpgcEridLfsrInit1.Data);
  }

  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_ERID_LFSR_CTL_MC_2LM_REG, cpgcEridLfsrCtl.Data);
}

/**

  This routine sets the CSRs required by BiosSetEridTrainingConfig

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param EnableTraining                    - Specifies whether Early Read ID coarse training mode is enabled.

  @retval N/A

**/
VOID
BiosSetEridTrainingConfigChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 ChannelInSocket,
  BOOLEAN                               EnableTraining
  )
{
  CPGC_ERID_LFSR_CTL_MC_2LM_STRUCT          cpgcEridLfsrCtl;
  CPGC_DDRT_MISC_CTL_MC_2LM_STRUCT          cpgcDdrtMiscCtl;

  cpgcEridLfsrCtl.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERID_LFSR_CTL_MC_2LM_REG);
  cpgcEridLfsrCtl.Bits.enable_erid_lfsr_training = (EnableTraining == TRUE)? 1:0;
  cpgcDdrtMiscCtl.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_DDRT_MISC_CTL_MC_2LM_REG);
  cpgcDdrtMiscCtl.Bits.enable_erid_return  = (EnableTraining == TRUE)? 0:1;
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_ERID_LFSR_CTL_MC_2LM_REG, cpgcEridLfsrCtl.Data);
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_DDRT_MISC_CTL_MC_2LM_REG, cpgcDdrtMiscCtl.Data);
}

/**

  This routine sets the CSRs required by BiosAepIoResetChip

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param Dimm                              - Zero based DIMM number.

  @retval N/A

**/
VOID
BiosAepIoResetChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 ChannelInSocket,
  UINT8                                 Dimm
  )
{
  struct dimmNvram                                 (*dimmNvList)[MAX_DIMM];
  MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_STRUCT   fnvioCtl;

  dimmNvList = GetDimmNvList (host, Socket, ChannelInSocket);

  //Set FNV MC Reset IO Bits
  fnvioCtl.Data = (*dimmNvList)[Dimm].fnvioControl;
  fnvioCtl.Bits.mc2ddrttrainreset = 1;
  WriteFnvCfg(host, Socket, ChannelInSocket, Dimm, MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_REG, fnvioCtl.Data);
  fnvioCtl.Bits.mc2ddrttrainreset = 0;
  WriteFnvCfg(host, Socket, ChannelInSocket, Dimm, MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_REG, fnvioCtl.Data);
  (*dimmNvList)[Dimm].fnvioControl = fnvioCtl.Data;
}

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
