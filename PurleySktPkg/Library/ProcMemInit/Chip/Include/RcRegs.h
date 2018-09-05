//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
#ifndef _rcregs_h
#define _rcregs_h

#ifdef interface
#undef interface
#endif

// APTIOV_SERVER_OVERRIDE_RC_START : Should check whether need to do the same for all includes here or a better method is available.
#include "RcRegs\FNV_A_UNIT_0.h"
#include "RcRegs\FNV_D_UNIT_0.h"
#include "RcRegs\FNV_DA_UNIT_0.h"
#include "RcRegs\FNV_DA_UNIT_1.h"
#include "RcRegs\FNV_DFX_MISC_0.h"
#include "RcRegs\FNV_DFX_MISC_1.h"
#include "RcRegs\FNV_DPA_MAPPED_0.h"
#include "RcRegs\FNV_DPA_MAPPED_1.h"
#include "RcRegs\FNV_P_UNIT_0.h"
#include "RcRegs\FNV_P_UNIT_1.h"
#include "RcRegs\FNV_SEC_0.h"
#include "RcRegs\FNV_SEC_1.h"
#include "RcRegs\FNV_DDRIO_COMP.h"
#include "RcRegs\FNV_DDRIO_DAT.h"
#include "RcRegs\FNV_DDRIO_DAT7_CH.h"

#include "RcRegs\CHA_MISC.h"
#include "RcRegs\CHA_CMS.h"
#include "RcRegs\CHA_PMA.h"
#include "RcRegs\CHABC_SAD.h"
#include "RcRegs\CHABC_SAD1.h"
#include "RcRegs\CHABC_UTIL.h"
#include "RcRegs\CHABC_PMA.h"
#include "RcRegs\IIOCB_FUNC0.h"
#include "RcRegs\IIOCB_FUNC1.h"
#include "RcRegs\IIOCB_FUNC2.h"
#include "RcRegs\IIOCB_FUNC3.h"
#include "RcRegs\IIOCB_FUNC4.h"
#include "RcRegs\IIOCB_FUNC5.h"
#include "RcRegs\IIOCB_FUNC6.h"
#include "RcRegs\IIOCB_FUNC7.h"
#include "RcRegs\IIO_DFX.h"
#include "RcRegs\IIO_DFX_DMI.h"
#include "RcRegs\IIO_DFX_VTD.h"
#include "RcRegs\IIO_DFX_VTD_DMI.h"
#include "RcRegs\IIO_IOAPIC.h"
#include "RcRegs\IIO_PCIE.h"
#include "RcRegs\IIO_PCIEDMI.h"
#include "RcRegs\IIO_PCIENTB.h"
#include "RcRegs\IIO_PERFMON.h"
#include "RcRegs\IIO_RAS.h"
#include "RcRegs\IIO_VTD.h"
#include "RcRegs\IIO_VMD.h"
#include "RcRegs\IIO_DFX_GLOBAL.h"
#include "RcRegs\IIO_DFX_GLOBAL_DMI.h"
#include "RcRegs\KTI_CIOPHYDFX.h"
#include "RcRegs\KTI_LLDFX.h"
#include "RcRegs\KTI_LLPMON.h"
#include "RcRegs\KTI_REUT.h"
#include "RcRegs\KTI_PHY_IOVB.h"
#include "RcRegs\KTI_PHY_PQD.h"
#include "RcRegs\M3KTI_CMS.h"
#include "RcRegs\M3KTI_MAIN.h"
#include "RcRegs\M3KTI_PMON.h"
#include "RcRegs\M3KTI_PMON1.h"
#include "RcRegs\M2UPCIE_MAIN.h"
#include "RcRegs\M2UPCIE_CMS.h"
#include "RcRegs\M2UPCIE_PMON.h"
#include "RcRegs\M2MEM_CMS.h"
#include "RcRegs\M2MEM_MAIN.h"
#include "RcRegs\MC_2LM.h"
#include "RcRegs\MC_DDRIOMC.h"
#include "RcRegs\VCU_FUN0.h"
#include "RcRegs\VCU_FUN1.h"
#include "RcRegs\VCU_FUN2.h"
#include "RcRegs\VCU_FUN3.h"
#include "RcRegs\VCU_FUN4.h"
#include "RcRegs\VCU_FUN5.h"
#include "RcRegs\VCU_FUN6.h"
#include "RcRegs\VCU_FUN7.h"
// APTIOV_SERVER_OVERRIDE_RC_END : Should check whether need to do the same for all includes here or a better method is available.
//#include "MC_1LMDP0.h"
// APTIOV_SERVER_OVERRIDE_RC_START : Should check whether need to do the same for all includes here or a better method is available.
#include "RcRegs\MCDDC_CTL.h"
#include "RcRegs\MCIO_DDRIO.h"
#include "RcRegs\MCIO_DDRIOEXT.h"
#include "RcRegs\MC_DDRIOBC.h"
#include "RcRegs\MC_MAIN.h"
#include "RcRegs\MC_MAINEXT.h"
#include "RcRegs\MCDDC_DP.h"
#include "RcRegs\PCU_FUN0.h"
#include "RcRegs\PCU_FUN1.h"
#include "RcRegs\PCU_FUN2.h"
#include "RcRegs\PCU_FUN3.h"
#include "RcRegs\PCU_FUN4.h"
#include "RcRegs\PCU_FUN5.h"
#include "RcRegs\PCU_FUN6.h"
#include "RcRegs\PCU_FUN7.h"
#include "RcRegs\UBOX_CFG.h"
#include "RcRegs\UBOX_DFX.h"
#include "RcRegs\UBOX_MISC.h"
#include "RcRegs\FPGA_CCI.h"
#include "RcRegs\FPGA_FME.h"
// APTIOV_SERVER_OVERRIDE_RC_END : Should check whether need to do the same for all includes here or a better method is available.
// Registers not defined in ConfigDB -- START
// Scratch register usage
//

#ifndef ASM_INC
// register at B:D:F: 0:0:0:FC with SoftSim flag
// Scratch register usage
//
#define CSR_EMULATION_FLAG_UBOX_CFG_REG ((BOX_IIO_PCIE_DMI << 24) | 0x40FC)
#define   EMULATION_INFO_UBOX_CFG_REG   ((BOX_IIO_PCIE_DMI << 24) | 0x40FC)
#define   EMULATION_FLAG_CSR_BOXINSTANCE  0

// Registers not defined in ConfigDB -- END


//
// ConfigDB has as SV ONLY, but shouldnt be -- START
//

#define   UNKNOWN1_0_6_7_CFG_REG   0x40037640
#endif // ASM_INC

#define DDRIOEXT2_CH(mcId)  ((mcId)*host->var.mem.numChPerMC)

#define CPGC_PATCADBPROG0_MCDDC_CTL_HSX_STRUCT              CPGC_PATCADBPROG0_MCDDC_CTL_STRUCT
#define CPGC_PATCADBPROG1_MCDDC_CTL_HSX_STRUCT              CPGC_PATCADBPROG1_MCDDC_CTL_STRUCT


#endif //_rcregs_h

