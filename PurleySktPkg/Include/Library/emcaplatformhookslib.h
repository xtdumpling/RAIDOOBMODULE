/** @file
  This is eMCA platform hooks library interface 

  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

  Copyright (c) 2009-2015 Intel Corporation.  All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license.  Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _EMCA_PLATFORM_HOOKS_LIB_H_
#define _EMCA_PLATFORM_HOOKS_LIB_H_

//
// Includes
//
#include <mca_msr.h>
#include <Uefi.h>
#include <emca.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/MpService.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Cpu/CpuRegs.h>
#include <Library/SynchronizationLib.h> 




//
// Defines
//
#define MC_SCOPE_TD    0x0
#define MC_SCOPE_CORE  0x1
#define MC_SCOPE_PKG   0x2
#define MCSCOPE_MASK  (BIT0 | BIT1)

#define EMCA_LOG_SUPPORT (BIT2)

#define EMCA_SECTYPE_NONE       0x0
#define EMCA_SECTYPE_PROCGEN    0x1
#define EMCA_SECTYPE_PROCIA32   0x2
#define EMCA_SECTYPE_PROCIPF    0x3
#define EMCA_SECTYPE_PLATMEM    0x4
#define EMCA_SECTYPE_PCIE       0x5
#define EMCA_SECTYPE_FWERR      0x6
#define EMCA_SECTYPE_PCIBUS     0x7
#define EMCA_SECTYPE_PCIDEV     0x8
#define EMCA_SECTYPE_DMAR       0x9
#define EMCA_SECTYPE_VTIO       0xA
#define EMCA_SECTYPE_IOMMU      0xB
#define EMCA_SECTYPE_RAW        0xC
#define EMCA_SECTYPE_IIO        0xD

#define EMCA_ENABLE_SWSMI       0x8E
#define EMCA_DISABLE_SWSMI      0x8D

#define MCA_UNIT_TYPE_IFU   (BIT0)
#define MCA_UNIT_TYPE_DCU   (BIT1)
#define MCA_UNIT_TYPE_DTLB  (BIT2)
#define MCA_UNIT_TYPE_MLC   (BIT3)
#define MCA_UNIT_TYPE_PCU   (BIT4)
#define MCA_UNIT_TYPE_KTI   (BIT5)
#define MCA_UNIT_TYPE_IIO   (BIT6)
#define MCA_UNIT_TYPE_CHA   (BIT7)
#define MCA_UNIT_TYPE_M2MEM (BIT8)
#define MCA_UNIT_TYPE_IMC   (BIT9)

#define MCA_UNIT_IFU0         (MCA_UNIT_TYPE_IFU)
#define MCA_UNIT_DCU0         (MCA_UNIT_TYPE_DCU)
#define MCA_UNIT_DTLB0        (MCA_UNIT_TYPE_DTLB)
#define MCA_UNIT_MLC0         (MCA_UNIT_TYPE_MLC)

#define MCA_UNIT_PCU0         (MCA_UNIT_TYPE_PCU)
#define MCA_UNIT_KTI0         (MCA_UNIT_TYPE_KTI)
#define MCA_UNIT_KTI1         (MCA_UNIT_TYPE_KTI)
#define MCA_UNIT_KTI2         (MCA_UNIT_TYPE_KTI)
#define MCA_UNIT_CHAM0        (MCA_UNIT_TYPE_CHA)
#define MCA_UNIT_CHAM1        (MCA_UNIT_TYPE_CHA)
#define MCA_UNIT_CHAM2        (MCA_UNIT_TYPE_CHA)

#define MCA_UNIT_IIO0         (MCA_UNIT_TYPE_IIO)
#define MCA_UNIT_M2M0         (MCA_UNIT_TYPE_M2MEM)
#define MCA_UNIT_M2M1         (MCA_UNIT_TYPE_M2MEM)
#define MCA_UNIT_IMC00        (MCA_UNIT_TYPE_IMC)
#define MCA_UNIT_IMC01        (MCA_UNIT_TYPE_IMC)
#define MCA_UNIT_IMC10        (MCA_UNIT_TYPE_IMC)
#define MCA_UNIT_IMC11        (MCA_UNIT_TYPE_IMC)
#define MCA_UNIT_IMC02        (MCA_UNIT_TYPE_IMC)
#define MCA_UNIT_IMC12        (MCA_UNIT_TYPE_IMC)

///
/// Forward declaration.
///
typedef struct _MCA_BANK_HOOKS MCA_BANK_HOOKS;

typedef
EFI_STATUS
(EFIAPI *MCA_AGREGATE_CSR_INFO_HOOK)
(
  IN MCA_BANK_HOOKS *This,
  IN UINT32 McBankIdx,
  IN UINT8 *Skt,
  IN EMCA_MC_SIGNATURE_ERR_RECORD *McSig,
  IN OUT VOID *Section 
);

typedef
EFI_STATUS
(EFIAPI *MCA_READ_MCBANK_SIG_HOOK) 
(
  IN UINT32 McBankIdx,
  IN UINT8 skt,    
  OUT EMCA_MC_SIGNATURE_ERR_RECORD * McSig
);


struct _MCA_BANK_HOOKS {
  MCA_AGREGATE_CSR_INFO_HOOK  CsrInfoHook;
};

typedef
BOOLEAN
(EFIAPI *MCA_RECOVERY_HANDLER_HOOK) 
(
  IN UINT32 McBankIdx,    
  OUT EMCA_MC_SIGNATURE_ERR_RECORD * McSig
);


//
// Type Definitions
//
//
typedef struct {
  UINT32 CtlMsr;
  UINT32 Ctl2Msr;
  UINT16 Attrib;
  UINT16 SecType;
  UINT32 UnitType;
  MCA_READ_MCBANK_SIG_HOOK ReadMcBankSig;
  MCA_BANK_HOOKS Hooks;
} EMCA_MCBANK_DESC; 

typedef struct {
  UINT16 McaCod;
  UINT16 McaCodMsk;
  UINT16 MsCod;
  UINT16 MsCodMsk;
  MCA_RECOVERY_HANDLER_HOOK RecoveryHdr;
} EMCA_MCRECOVERY_DESC;

typedef struct {
  UINT32 McBankNum;
  UINT8 MemoryControllerId;
} IMC_NODE_LOOKUP;

typedef struct {
  UINT16 McaCod;
  UINT16 McaCodMsk;
  UINT16 MsCod;
  UINT16 MsCodMsk;
  UINT8 ErrorType;
} EMCA_CSR_INFO_DESC;

typedef struct {
  SPIN_LOCK Busy;
} CMCI_GEN2_PARAMS;

typedef struct {
  UINTN  LogicalCpu;
  BOOLEAN Sync;
} MCEMSR_ARGS_STRUCT;

typedef struct {
  SPIN_LOCK             ApBusy;
  UINT32                BankIndex;
  UINT32                ApicId;
} CMCI_ARGS_STRUCT;

typedef union {
  struct {
    UINT32  Vector:8;          ///< The vector number of the interrupt being sent.
    UINT32  DeliveryMode:3;
    UINT32  Reserved0:1;       ///< Reserved.
    UINT32  DeliveryStatus:1;  ///< 0: Idle, 1: send pending.
    UINT32  Reserved1:3;       ///< Reserved.
    UINT32  Mask:1;            ///< 0: Not masked, 1: Masked.
    UINT32  Reserved2:14;      ///< Reserved.
  } Bits;
  UINT32    Uint32;
} LOCAL_APIC_LVT_CMCI;



/**

  Populate Silicon Capability Structure of Platform RAS Protocol

  @param *CapabilityPtr  - Pointer to the Silcon RAS Capability Structure

  @retval VOID

**/
VOID
PopulateSiliconCapabilityStructure (
      OUT   SILICON_RAS_CAPABILITY   *CapabilityPtr
);
  
/**
  
      Call Bank Specific CSRs Hook to aggregate Memory Section Information  
  
      @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
      @param Skt       - Socket Id 
      @param McSig     - A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS. 
      @param Section   - A pointer to UEFI error section. 
  
      @retval EFI_INVALID_PARAMETER - At least one of the pointers is invalid or McBank number points to an unavailable bank.  
      @retval EFI_NOT_FOUND - Hook is not available for the bank 
      @retval EFI_SUCCESS           - Function executed Correctly
  
  **/
EFI_STATUS
MCBankCsrInfoHook(
  IN UINT32 McBankIdx,
  IN UINT8 *Skt,
  IN EMCA_MC_SIGNATURE_ERR_RECORD *McSig,
  IN OUT VOID *Section 
);

/**

    Call Bank specific Signature hook. 

    @param McBankIdx -  The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 
    @param skt       - Socket Id
    @param McSig     -  A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS.
    
    @retval EFI_INVALID_PARAMETER - At least one of the pointers is invalid or McBank number points to an unavailable bank.   
    @retval EFI_SUCCESS - McSig contains valid information  

**/
EFI_STATUS
EFIAPI 
ReadMcBankSigHook(
  IN UINT32 McBankIdx,
  IN UINT8 skt,    
  OUT EMCA_MC_SIGNATURE_ERR_RECORD * McSig
);

/**

    Iterate within Silicon Specific Hooks to identify and perform recovery of errors.  

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param Skt       - Socket Id
    @param McSig     -  A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS. 
                                This function should update the values in the McSig if the Recovery is successful.     

    @retval FALSE - Recovery was not successful 
    @retval TRUE - Recovery was successful 

**/
BOOLEAN
AttemptRecovery(
IN UINT32 McBankIdx,
IN UINT8 Skt,
IN OUT EMCA_MC_SIGNATURE_ERR_RECORD *McSig
);

/**

    Write updated values for corrected error in the McBank. The Firmware updated bit must be set. 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param McSig     - A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS. 
                               The values in the signature will be written to the banks. 

    @retval None

**/
VOID
WriteMcBank
(
  IN UINT32 McBankIdx,
  IN OUT EMCA_MC_SIGNATURE_ERR_RECORD *McSig
);




//
// Type Definitions
//
//


//
// Functions prototype declarations 
//

/**
  Get the number of MCBANKS_PER_LP.
  
  MCBANKS_PER_LP = MCBANK_CORE + MCBANKS_UNCORE = MCG_CAP.Count
  
  @return MCBANKS_PER_LP

**/
UINT64 
GetNumMcBanksPerLp(
  VOID
  );

/**
  Get the number of L1 Dir Entries required based on the MC Banks available and number of sockets.

  Number of L1 Dir Entries = SOCKET_COUNT * ( 2 ** APICID_BITS_PER_SOCKET) * MCBANKS_PER_LP where: 
  
  SOCKET_COUNT is  the maximum number  of  processor  sockets in  the platform,
  APICID_BITS_PER_SOCKET is the number of APIC ID bits used up by each socket  
  MCBANKS_PER_LP = MCBANK_CORE + MCBANKS_UNCORE = MCG_CAP.Count
  
  @return number of L1 Dir Entries  

**/
UINT64 
GetNumL1DirEntries(
  VOID
  );

/**
  Get the number of Elog Dir(L2) Entries required based on the MC Banks in the system.

 NumElogentries = number of physical MC BANKS in the system
                      = NumSKTSupported * NumMCBanksPerSKT
                      = NumSKTSupported * (NumCoreMcBanks *  CORES_PER_SOCKET + NumUncoreMcBanks ) 
  
  @return Number of Elog Entries 

**/
UINT64 
GetNumElogEntries(
  VOID
  );

/**

    Get if emca logging support has been implemented in BIOS for particular Mc Bank. 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval  TRUE - logging is implemented in bios for McBank. 
    @retval FALSE - logging is not implemented in bios for McBank.

**/
BOOLEAN
isEMCALogSupported(
  UINT32 McBankIdx
  );

/**

    Return whether the bank unit is Memory

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval TRUE  - Bank Unit is memory
    @retval FALSE - Bank Unit is not memory

**/
BOOLEAN
isEmcaMemoryBank(
  UINT32 McBankIdx
  );

/**

    Return whether the bank unit is Memory

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval UNIT TYPE  - Silicon Specific Bank Unit Type

**/
UINT32
GetMcBankUnitType(
  UINT32 McBankIdx
  );


/**

    Get Scope for the Bank. The scope can be thread level, core level and package level.

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval  0x0  - Thread Scope 
    @retval  0x1  - Core Scope 
    @retval  0x2  - Package Scope 

**/
UINT16
GetMcBankScope(
  UINT32 McBankIdx
  );

/**

    Get Bank Index for Mci_Ctl for McBank 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval Mci_Ctl Index

**/
UINT32
  GetAddrMcBankCtl
(
  UINT32 McBankIdx
);

/**

    Get Bank Index for Mci_status for McBank

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval Mci_status Index

**/
UINT32
GetAddrMcBankSts
(
  UINT32 McBankIdx
);

/**

    Get Bank Index for Mci_addr for McBank 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 
    

    @retval Mci_addr Index

**/
UINT32
GetAddrMcBankAddr
(
  UINT32 McBankIdx
);

/**

    Get Bank Index for Mci_Misc for McBank 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval Mci_Misc Index

**/
UINT32
GetAddrMcBankMisc
(
  UINT32 McBankIdx
);

/**

    Get Bank Index for Mci_Misc2 for McBank 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 

    @retval Mci_ctl2 Index

**/
UINT32
GetAddrMcBankCtl2
(
  UINT32 McBankIdx
);

/**

    Get Section Type for McBank 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.

    @retval SecType - Return the section type related to the McBank Unit

**/
UINT16
GetMcBankSecType
(
  UINT32 McBankIdx
);




/**

    Enable emca gen 1 CSMI morphing 

    @param node - NodeId System Wide

    @retval None

**/
void
EnableCSMIGen1(
UINT8 *node
);

/**

     Enable CMCI->SMI morphing for all the banks 

    @param Params - Used to pass parameters 

    @retval None

**/
VOID
EnableCSMIGen2(
CMCI_GEN2_PARAMS *Params
);


/**

    Enable MCE->SMI morphing for McBankIdx 

    @param McBankIdx - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.

    @retval None

**/
VOID
EmcaEnableMSMI(
  UINT32 *McBankIdx
);

VOID
EnableMsmiBanks(
  IN        VOID  *Buffer
);

/**
  This function enables the SMI generation of uncorrected errors in eMCA mode

  @param[in] Socket   The socket number

  @retval None.

**/
VOID
EnableEmcaSmiForUncorrectedErrors (
  IN        UINT8                           Socket
  );

/**
  This function enables the SMI generation for Corrected errors in eMCA mode

  @param[in] Socket   The socket number

  @retval None.

**/
VOID
EnableEmcaSmiForCorrectedErrors (
    UINT8 Socket,
    UINT8 Mc
    );

/**
  This function enables the SMI generation of uncorrected errors using gen1

  @param[in] Buffer   unused

  @retval None.

**/
VOID
EnableEmcaSmiGen1ForUncorrectedErrors (
  VOID  *Buffer
);

/**
  This function return if MCERR and IERR has been logged.

  @param[in] Socket   The socket number
  @param[in] EmcaMsmiEn   MSMI is enabled
  @param[in] EmcaCsmiEn  CSMI is enabled

  @retval BOOLEAN.

**/
BOOLEAN
CheckErrLogReg (
  IN UINT8 Socket,
  IN UINT8 EmcaMsmiEn,
  IN UINT8 EmcaCsmiEn
);


/**
  This function clears MCERRLOG and IERRLOG.

  @param[in] Socket   The socket number
  @param[in] EmcaMsmiEn   MCE morphing is enabled

  @retval VOID

**/
VOID
ClearErrLogReg(
IN UINT8 Socket,
IN UINT8 EmcaMsmiEn,
IN UINT8 EmcaCsmiEn
) ;

/**
  This function clears Shadow Registers.

  @param[in] Socket   The socket number
  @param[in] Mc   Mc Index

  @retval VOID

**/
VOID
ClearMciShadows (
  IN UINT8 Socket, 
  IN UINT8 Mc
  );



/**
  This function return UMC status.

  @param[in] Socket   The socket number
  @param[in] UmcSmiEn   UMC is enabled

  @retval BOOLEAN.

**/
VOID
ClearUmcStatus (
  IN UINT8 Socket
  );


/**

    Allocates protocols and initialize internal structure required by the library.  

    @param ImageHandle - refers to the image handle of the driver.
    @param SystemTable - points to the EFI System Table.

    @retval EFI_SUCCESS - Library constructor executed correctly 

**/
EFI_STATUS
EFIAPI
EmcaPlatformHooksLibConstructor (
IN EFI_HANDLE ImageHandle,
IN EFI_SYSTEM_TABLE *SystemTable
);



/**

    If successful Write Emca Logging Capability to the input buffer 

    @param EMcaLiteCap - Buffer to write the Emca Logging Capability 

    @retval EFI_INVALID_PARAMETER - EmcaLiteCap pointer is NULL
    @retval EFI_NOT_READY         - Function is executed outside SMM 
    @retval EFI_SUCCESS           - EMcaLiteCap contains valid data

**/
EFI_STATUS
GetEMCALogProcCap(
IN OUT BOOLEAN *EMcaLiteCap
);

/**

    Return TRUE if silicon Supports EMCA Gen 2 and False otherwise 

    @param None

    @retval TRUE - Emca Gen 2 is supported
    @retval FALSE - Emca Gen 2 is not supported

**/
BOOLEAN
isEMCAGen2Cap(
VOID
);

/**

    Return bimap of MC Banks that support EMCA gen 2 morphing 

    @param None

    @retval BitMap - Bitmap of McBanks which support morphing 

**/
UINT64
GetEMcaBankSupp(
VOID
);


/**

    Initialize L1 Directory pointers according to eMCA spec. This assumes the McBank Scope is Core Level so each Thread within the Core will share elog pointer. 

    @param L1DirHdr - L1 directory base pointer 
    @param McBank   - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param NxtElog  - Pointer to Next Free Elog memory 

    @retval EFI_INVALID_PARAMETER - At least of the input pointers is NULL
    @retval EFI_OUT_OF_RESOURCES  - Elog directory memory is insufficient  
    @retval EFI_SUCCESS           - The initialization was successful

**/
EFI_STATUS
InitL1PtrForCores(
  IN EMCA_L1_HEADER *L1DirHdr,
  IN UINT32 McBank,
  IN OUT VOID **NxtElog
);

/**

    Initialize L1 Directory pointers according to eMCA spec. This assumes the McBank Scope is Thread Level so each thread will have its own elog pointer. 

    @param L1DirHdr - L1 directory base pointer 
    @param McBank   - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param NxtElog  - Pointer to Next Free Elog memory 

    @retval EFI_INVALID_PARAMETER - At least of the input pointers is NULL
    @retval EFI_OUT_OF_RESOURCES  - Elog directory memory is insufficient  
    @retval EFI_SUCCESS           - The initialization was successful

**/
EFI_STATUS
InitL1PtrForThreads(
  IN EMCA_L1_HEADER *L1DirHdr,
  IN UINT32 McBank, 
  IN OUT VOID **NxtElog
);

/**

    Initialize L1 Directory pointers according to eMCA spec. This assumes the McBank Scope is Package Level so all the threads within the socket will share the elog pointer. 

    @param L1DirHdr - L1 directory base pointer 
    @param McBank   - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count.
    @param NxtElog  - Pointer to Next Free Elog memory 

    @retval EFI_INVALID_PARAMETER - At least of the input pointers is NULL
    @retval EFI_OUT_OF_RESOURCES  - Elog directory memory is insufficient  
    @retval EFI_SUCCESS           - The initialization was successful

**/
EFI_STATUS
InitL1PtrForSockets(
  IN EMCA_L1_HEADER *L1DirHdr,
  IN UINT32 McBank, 
  IN OUT VOID **NxtElog
);

/**

    Read Core SMI Source MSR which contains bitmap of McBanks containing errors for CORE units. 

    @param None

    @retval retval - Value of CORE_SMI_SRC

**/
UINT64
ReadCoreSmiSrc(
VOID
);

/**

    Write Value to SMI SRC to Clear it. 

    @param Buffer - Value to Write to SMI_SRC 

    @retval None

**/
VOID
ClearCoreSmiSrc(
IN VOID *Buffer
);

/**

    Read Uncore SMI Source MSR which contains bitmap of McBanks containing errors for UNCORE units. 

    @param None

    @retval retval - Value of UNCORE_SMI_SRC

**/
UINT64
ReadUncoreSmiSrc(
VOID
);

/**

    Write Value to UNCORE_SMI SRC to Clear it. 

    @param Buffer - Value to Write to UNCORE_SMI SRC 

    @retval None

**/
VOID
ClearUncoreSmiSrc(
IN VOID *Buffer
);

/**
    

    Disables CSMI at MC bank

    @param McBank - McBank number

    @retval NONE

**/
VOID
DisableCSMIAtMcBank(
UINT8 McBank
);

/**
  Signal an MCE via MMIO

  This function signals an MCE using the MMIO mechanism

  @param[in] LogicalCpu     The CPU number of the CPU that generated the error

  @retval Status.

**/
EFI_STATUS
SignalMceMmio ( 
  IN      UINTN LogicalCpu
);

/**
  Signal an MCE via MSR

  This function signals an MCE using the MSR mechanism

  @param[in] MceArgs     Structure for passing variables in smmstartthisapp across thread

**/
VOID
SignalMceMsr ( 
  IN      MCEMSR_ARGS_STRUCT *MceArgs
);

/**
  Determine MCE Mechanism

  This function returns the mechanism to use to generate the MCE

  @retval UINT32    0 MMIO
                    1 MSR

**/
UINT32
GetSaveStateType ( 
  UINT16 SocketBitMap
);

/**
  Signal a CMCI

  This function will signal a CMCI

  @param[in] CmciVarPtr  Structure for passing variables in smmstartthisapp across thread

**/
VOID SignalCmci ( 
  CMCI_ARGS_STRUCT *CmciVarPtr
);

/**
  Is Lmce supported by the firmware
  
  @param[in] LmceFwrEn True if Lmce is supported by the firmware
  @retval NONE  

**/
VOID
IsLmceFwrEnabled(
  BOOLEAN *LmceFwrEn
  );


/**
  Is Lmce supported by the processor
  
  @param[in] Supported True if Lmce is supported by the processor 
  @retval NONE  

**/
VOID
IsLmceSupported (
  BOOLEAN * Supported
  );


/**
  Has OS enabled Lmce in the mcg_ext_ctl
  
  @param[in] LmceEn Return whether OS has enabled lmce in the mcg_ext_ctl 
  @retval NONE  

**/
VOID
IsLmceOsEnabled (
  OUT BOOLEAN * LmceEn
  );



/**
  Return if Local machine check exception signaled. Indicates (when set) that a local machine check
exception was generated. This indicates that the current machine-check event was delivered to only the
logical processor.

  @retval TRUE  LMCE was signaled.
  @retval FALSE LMCE was not signaled.

**/
VOID
IsLmceSignaled (
  IN BOOLEAN *Signaled
  );


/**

    Enable LMCE

    @param Buffer 

    @retval None

**/
VOID
EnableLMCE(
  IN BOOLEAN  *Enabled
);

/**
  return whether BMCE happen. 

  @retval TRUE  BMCE was signaled.
  @retval FALSE BMCE was not signaled.

**/
BOOLEAN
isBMCE(
  UINT16 SocketBitMap
  );

/**
  Clear Lmce if simultaneous BMCE and LMCE happen. 
  
**/
VOID
ClearLmceStatus (
  IN VOID *Buffer
  );






#endif  // _EMCA_PLATFORM_HOOKS_LIB_H_

