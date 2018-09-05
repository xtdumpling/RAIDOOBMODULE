/**++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

    @file eMCA.c

    This is an implementation of the eMCA driver 

---------------------------------------------------------------------------**/

#include <Library/emcalib.h>
#include <Library/emcaplatformhookslib.h>
#include <Library/SynchronizationLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/mpsyncdatalib.h>


VOID   *mL1DirBkp = NULL;
VOID   *mElogDirBkp = NULL;
UINT64      MemBanksMask = 0;


//EFI_SMM_BASE2_PROTOCOL    *mSmmBase;
//EFI_SMM_SYSTEM_TABLE2     *mSmst;

/**
  Get memory size in bytes required to store the L1 Directory.

  L1 Dir Size = Number of L1 Dir entries * 8 + 0x40

  Number of L1 Dir Entries = SOCKET_COUNT * ( 2 ** APICID_BITS_PER_SOCKET) * MCBANKS_PER_LP where: 
  
  SOCKET_COUNT is  the maximum number  of  processor  sockets in  the platform,
  APICID_BITS_PER_SOCKET is the number of APIC ID bits used up by each socket  
  MCBANKS_PER_LP = MCBANK_CORE + MCBANKS_UNCORE = MCG_CAP.Count
  
  @return required sized in bytes 

**/
STATIC
UINT64
GetL1DirSize (
  VOID
  )

{
  return (GetNumL1DirEntries() * L1_DIR_PTR_LEN) + sizeof(EMCA_L1_HEADER); 
}

/**
  Get memory size in bytes required to store the Elog Directory(L2).

  NumElogEntries * 0x400 
  
  @return required sized in bytes 

**/
STATIC
UINT64
GetElogDirSize (
  VOID
  )

{
  return (GetNumElogEntries() * ELOG_ENTRY_LEN);  
}

/**

    Initialize L1 Directory pointers according to eMCA spec. This will take the elog directory memory and the l1 directory memory and initialize the elog directory entry pointers depending on the processor topology and mc bank structure. 

    @param L1Dir - Pointer to the L1 Directory Header

    @retval EFI_SUCCESS - The initialization was successful
    @retval EFI_INVALID_PARAMETER - L1 Pointer is null or invalid 
    @retval EFI_OUT_OF_RESOURCES - Elog directory memory is insufficient    

**/
STATIC
EFI_STATUS
InitL1Dir(
  IN VOID                         *L1Dir
  )
{
  EMCA_L1_HEADER *L1DirHdr;
  UINT32 i;
  UINT16  McScope;
  UINT8 *NxtElog;
  EFI_STATUS Status = EFI_SUCCESS;
  
  L1DirHdr = (EMCA_L1_HEADER *)L1Dir;
  NxtElog = (UINT8 *) L1DirHdr->ElogDirBase;

  for(i = 0; i < L1DirHdr->NumL1EntryLp; i++) {
    if(isEMCALogSupported(i)) {
      McScope = GetMcBankScope(i);
      switch(McScope) {
        
        case MC_SCOPE_TD:
          Status = InitL1PtrForThreads(L1DirHdr, i, &NxtElog);
          if(Status != EFI_SUCCESS)
            return Status;
          break;
        case MC_SCOPE_CORE:
          Status = InitL1PtrForCores(L1DirHdr, i, &NxtElog);
          if(Status != EFI_SUCCESS)
            return Status;
          break;
        case MC_SCOPE_PKG:
          Status = InitL1PtrForSockets(L1DirHdr, i, &NxtElog);
          if(Status != EFI_SUCCESS)
            return Status;
          break;
        default:
          ASSERT(FALSE); //should not enter here
          break;
      }         
    }
  }
  return Status;
}

/**

    This routine takes the L1 directory pointer and initialize the header with correct values

    @param L1DirHdr    - L1 directory base pointer 
    @param ElogDirBase - Elog directory base pointer 
    @param L1DirSize   - L1 directory size in bytes 
    @param ElogSize    - Elog directory size in bytes 

    @retval EFI_INVALID_PARAMETER - At least one of the input pointers was NULL
    @retval EFI_SUCCESS           - The directory was initialized succesfully 

**/
STATIC
EFI_STATUS
CreateL1DirHdr(
  IN OUT EMCA_L1_HEADER *L1DirHdr,
  IN VOID *ElogDirBase,
  IN UINT64 L1DirSize,
  IN UINT64 ElogSize
  )
{  
  if(L1DirHdr == NULL || ElogDirBase == NULL) {
    return EFI_INVALID_PARAMETER;
  }  
  
  L1DirHdr->Version = EMCA_L1_VERSION;
  L1DirHdr->HdrLen = sizeof(EMCA_L1_HEADER);
  L1DirHdr->L1DirLen = L1DirSize;
  L1DirHdr->ElogDirBase = (UINT64)ElogDirBase;
  L1DirHdr->ElogDirLen = ElogSize; 
  L1DirHdr->NumL1EntryLp = (UINT32)GetNumMcBanksPerLp();

  return EFI_SUCCESS;  
}

/**
  Allocates L1 memory and the pointer is stored in the parameter.
  
  @param  Buffer                A pointer to a pointer to the allocated buffer if the call succeeds;
                                undefined otherwise.

  @retval EFI_SUCCESS           The requested number of bytes was allocated.
  @retval EFI_OUT_OF_RESOURCES  The pool requested could not be allocated.
  @retval EFI_INVALID_PARAMETER PoolType was invalid.

**/
EFI_STATUS
AllocateL1Dir(
  OUT VOID                         **Buffer,
  OUT UINT64                        *L1Size
  )
{
  //UseAllocatepages, below 4GB, Memory reserved 
  UINTN Pages = 0;
  EFI_STATUS Status;
  EFI_PHYSICAL_ADDRESS MaxAddress;

  if(Buffer == NULL || L1Size == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  *Buffer = NULL;
  *L1Size = GetL1DirSize(); 
  Pages = EFI_SIZE_TO_PAGES(*L1Size);
  MaxAddress = 0xffffffff; //less or equal address for the space allocated. It must be under 4GB

  Status = gBS->AllocatePages(
                  AllocateMaxAddress,
                  EfiReservedMemoryType,
                  Pages,
                  &MaxAddress          
               );

  if(Status == EFI_SUCCESS) {
    *Buffer = (VOID *)MaxAddress;
    gBS->SetMem(*Buffer,EFI_PAGES_TO_SIZE(Pages), 0);
  } 

  return Status;
}

/**
  Allocates Elog(L2) memory and the pointer is stored in the parameter.
  
  @param  Buffer                A pointer to a pointer to the allocated buffer if the call succeeds;
                                undefined otherwise.

  @retval EFI_SUCCESS           The buffer was allocated.
  @retval EFI_OUT_OF_RESOURCES  The buffer could not be allocated.
  @retval EFI_INVALID_PARAMETER

**/
STATIC
EFI_STATUS 
AllocateElogDir(
  OUT VOID                         **Buffer,
  OUT UINT64                            *ElogSize
  )
{
  UINTN Pages = 0;
  EFI_STATUS Status;
  EFI_PHYSICAL_ADDRESS MaxAddress;

  if(Buffer == NULL || ElogSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  *Buffer = NULL;
  *ElogSize = GetElogDirSize();

  Pages = EFI_SIZE_TO_PAGES(*ElogSize);
  MaxAddress = 0xffffffff;

  Status = gBS->AllocatePages(
                  AllocateMaxAddress,
                  EfiReservedMemoryType,
                  Pages,
                  &MaxAddress
                );

  if(Status == EFI_SUCCESS) {
    *Buffer = (VOID *) MaxAddress;
    gBS->SetMem(*Buffer,EFI_PAGES_TO_SIZE(Pages), 0); 
  }

  return Status;
}  


/**
  Initialize directories

  a.  Allocate and zero out memory
  b.  Setup L1 directory  header  and clear all the L1 directory entries
  c.  Setup valid L1 entries 
  d.  Bring all the threads to SMM and update Enhanced_MCA_LOG(63)
  
  @param  L1Dir                 Pointer to Pointer to save the L1 Directory Memory Location 
  @param  ElogDir               Pointer to Pointer to save Elog(L2) Directory Memory Location ;
  undefined otherwise.

  @retval EFI_SUCCESS          
  @retval EFI_INVALID_PARAMETER 

**/
STATIC
EFI_STATUS
InitLogDirs(
  IN OUT VOID                         **L1Dir,
  IN OUT VOID                         **ElogDir
  )
{
  EFI_STATUS Status = EFI_SUCCESS; 
  UINT64 L1DirSize;
  UINT64 ElogSize;  

  Status = AllocateL1Dir(L1Dir,&L1DirSize);
  if(Status != EFI_SUCCESS) {
    return Status;
  }
  Status = AllocateElogDir(ElogDir,&ElogSize);
  if(Status != EFI_SUCCESS) {
    return Status;
  }
  Status = CreateL1DirHdr((EMCA_L1_HEADER *)*L1Dir,*ElogDir,L1DirSize,ElogSize);
  if(Status != EFI_SUCCESS) {
    return Status;
  }
  Status = InitL1Dir(*L1Dir);
  if(Status != EFI_SUCCESS) {
    return Status;
  }

  mL1DirBkp = *L1Dir;
  mElogDirBkp = *ElogDir;
  
  return Status;
}

/**

    Perform eMCA L1 directory boot time initialization

    @param L1Dir   - A pointer to a pointer to the allocated L1 directory buffer if the call succeeds; undefined otherwise. 
    @param ElogDir - A pointer to a pointer to the allocated Elog directory buffer if the call succeeds; undefined otherwise.

    @retval EFI_SUCCESS - The Allocation and Initialization was successful 
    @retval EFI_UNSUPPORTED - EMCA Logging is not supported
    @retval EFI_OUT_OF_RESOURCES - The directories could not be allocated.
    @retval EFI_INVALID_PARAMETER - At least one of the pointers is NULL or invalid      

**/
EFI_STATUS
InitEMCA( 
IN OUT VOID                         **L1Dir,
IN OUT VOID                         **ElogDir
)
{
  BOOLEAN EmcaLiteCap;
  EFI_STATUS Status = EFI_SUCCESS;
  
  Status = GetEMCALogProcCap(&EmcaLiteCap);
  ASSERT_EFI_ERROR(Status);
  
  if(EmcaLiteCap) {
    Status = InitLogDirs(L1Dir,ElogDir);
    ASSERT_EFI_ERROR(Status);
    //Status = RegisterEmcaOSOptinSmi();
    //ASSERT_EFI_ERROR(Status);
  } else {
    Status = EFI_UNSUPPORTED;
  }

  return Status;  
}

/**

    Get the pointer for the L1 Directory Header 

    @param Hdr - It is a Pointer to a Pointer where to store the address for L1 Directory if successful. Undefined otherwise. 

    @retval EFI_INVALID_PARAMETER - Hdr Pointer is NULL
    @retval EFI_SUCCESS - The Header was located successfully  

**/
EFI_STATUS
GetL1DirHdr(
OUT VOID **Hdr
)
{
  if(Hdr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Hdr = mL1DirBkp;
  return EFI_SUCCESS;   
}

/**

    Return True if OS has set the opt in flag in the L1 Directory header or False if it is not set 

    @param None

    @retval optin - TRUE if optin flag is set, False if cleared 

**/
BOOLEAN
isEmcaOptInFlagSet(
VOID
) 
{
  EFI_STATUS Status = EFI_SUCCESS;
  BOOLEAN    optin;
  EMCA_L1_HEADER *L1Hdr = NULL;

  Status = GetL1DirHdr(&L1Hdr);
  if(Status != EFI_SUCCESS) {
    optin = FALSE;
  } else {
    optin = ((L1Hdr->Flags & OS_VMM_OPT_IN_FLAG) != 0);
  }

  return optin;  
}

/**

    It receives the generic error status record and stores it in the appropriate elog entry. 

    @param McBankNum - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 
    @param McSig     - A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS. 
    @param Log       - A pointer to an ACPI Generic Error Status Block completely filled along with its UEFI error section. This function just stores the log. See ACPI Specification for details of the record format. 

    @retval EFI_INVALID_PARAMETER - At least one of the pointers is invalid or McBank number points to an unavailable bank.   
    @retval EFI_OUT_OF_RESOURCES  - The corresponding Elog Entry pointer for the bank and apicid is invalid. There is no memory to store the log. 
    @retval Status                - The log was successfully stored in the elog directory.  

**/
EFI_STATUS
LogEmcaRecord(
  IN UINT32                                 McBankNum,
  IN CONST EMCA_MC_SIGNATURE_ERR_RECORD     *McSig,
  IN CONST GENERIC_ERROR_STATUS             *Log
  )  
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    GENERIC_ERROR_STATUS       *ErrStsBlk = NULL;
    EMCA_L1_HEADER             *L1Hdr = NULL;
    UINT8                      *L1Base = NULL;
    UINT8                      *RawData = NULL;
    UINT32                     *BlockStatus = NULL;
    UINT64                       L1Offset;
    UINT32                        BlkLen;
    UINT32 CompareValue;
    UINT32 ExchangeValue;

    if(Log == NULL || McSig == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    Status = GetL1DirHdr(&L1Hdr);
    ASSERT_EFI_ERROR(Status);

    if(L1Hdr == NULL) 
      return EFI_INVALID_PARAMETER;

    if(McBankNum >= L1Hdr->NumL1EntryLp) {
      return EFI_INVALID_PARAMETER;
    }

    L1Base = EMCA_GET_L1_BASE(L1Hdr);
    L1Offset = EMCA_GET_L1_OFFSET(L1Hdr, McSig->ApicId, McBankNum);
    ErrStsBlk = EMCA_GET_ELOG_ENTRY(L1Base, L1Offset);

    //Check for Valid bit in pointer and mask 63:52 bits which are not part of the address. 
    if(((UINT64)ErrStsBlk & BIT63) != 0) { 
      ErrStsBlk = (GENERIC_ERROR_STATUS *)((UINT64)ErrStsBlk & 0x000FFFFFFFFFFFFF);
    } else {
      return EFI_OUT_OF_RESOURCES;
    }

    BlkLen = sizeof(GENERIC_ERROR_STATUS) + Log->ErrDataSize;
    BlockStatus = (UINT32*) &(ErrStsBlk->BlockStatus);

    CompareValue = 0;
    ExchangeValue = *((UINT32*) &(Log->BlockStatus));
    
    if(InterlockedCompareExchange32(BlockStatus, CompareValue, ExchangeValue) == 0) {
      //If blockstatus is 0 then the record is either invalid or the OS has consumed it, we can just write the record to memory 
      CopyMem(ErrStsBlk, Log, BlkLen);
      ErrStsBlk->RawDataOffset = BlkLen;
      ErrStsBlk->RawDataSize = sizeof(EMCA_MC_SIGNATURE_ERR_RECORD);
      RawData = ((UINT8 *)ErrStsBlk) + ErrStsBlk->RawDataOffset;
      CopyMem(RawData, McSig, ErrStsBlk->RawDataSize);
        
    } else {
      //There is already a log in memory
      //Use MCA Overwrite Rules to either overwrite the existing record or leave it alone. 
      if( (ErrStsBlk->Severity == GENERIC_ERROR_CORRECTED) || 
          (ErrStsBlk->Severity == GENERIC_ERROR_RECOVERABLE && Log->Severity == GENERIC_ERROR_FATAL) 
         ) {
         //Overwrite elog
        CopyMem(ErrStsBlk, Log, BlkLen);
        ErrStsBlk->RawDataOffset = BlkLen;
        ErrStsBlk->RawDataSize = sizeof(EMCA_MC_SIGNATURE_ERR_RECORD);
        RawData = ((UINT8 *)ErrStsBlk) + ErrStsBlk->RawDataOffset;
        CopyMem(RawData, McSig, ErrStsBlk->RawDataSize);
      } else {
        if(McSig->Signature.McSts & BIT62) {
            //Over bit is set, update MC Signature
            RawData = ((UINT8 *)ErrStsBlk) + ErrStsBlk->RawDataOffset;
            CopyMem(RawData, McSig, ErrStsBlk->RawDataSize);
        }
      }
    }
    
  return Status; 
}

/**
  Get the uncore machine check banks.

  Returns a bitmap indicating uncore machine check banks.

  @retval McBankBitField

**/
UINT64
ReadUncoreMcBankBitField( 
  UINT8  Mode
)
{
  UINT64 McBankBitField;
  UINT8 McBank;
  UINT16 McScope;
  UINT64 MaxBank;

  McBankBitField = 0;
  McBank = 0;
  McScope = 0;
  MaxBank = 0;

  

  if (Mode == MODE_EMCA) {
    McBankBitField = ReadUncoreSmiSrc();

    /* Uncore and core smisrc are bitmaps of Mc banks. Only the bits corresponding to the
     * banks that generate the SMI should be set.
     * This formula will calculate bitmask for the maximum possible bank that can be set
     * For instance, if Max Banks is 20 then the mask will be (0x100000 - 1) =  0xFFFFF.
     * Clearing bit 20 and above while passing lower bits 19-0*/

    MaxBank = GetNumMcBanksPerLp();
    McBankBitField &= ((UINT64)1 << MaxBank)-1;
  } else {
    MaxBank = GetNumMcBanksPerLp();
    for (McBank = 0, McBankBitField = 0; McBank < MaxBank; McBank++) {
      McScope = GetMcBankScope (McBank);
      if (McScope == MC_SCOPE_PKG) {
        McBankBitField |= (1ULL << McBank);
      }
    }
  }
  return McBankBitField;
}

/**
  Return a bitmap of any error sources set in Uncore McBanks.

  @param[out]  McBankBitField   - Bitmap of sources in UNCORE_SMI_ERR_SRC that indicate error conditions

**/
VOID
GetUncoreMcBankErrors ( 
      OUT GET_SMI_SRC_ARGS_STRUCT  *Params
)
{
  Params->McBankBits = ReadUncoreMcBankBitField (Params->Mode);
}

/**
  Return a bitmap of any memory error sources set in McBanks.

  @param[in]  MemBankBits   - Bitmap of memory sources in UNCORE_SMI_ERR_SRC that indicate error conditions

  @retval TRUE if memory bank reported errors, otherwise false
**/
BOOLEAN
GetMcBankMemErrors (
  IN       UINT64   McBankBits
  )
{
  UINT32            McBank;
  UINT32            MaxBank;
    
  if (MemBanksMask == 0) {
    MaxBank = (UINT32)GetNumMcBanksPerLp ();
    for (McBank = 0; McBank < MaxBank; McBank++) {
      if (GetMcBankSecType (McBank) == EMCA_SECTYPE_PLATMEM) {
        MemBanksMask |= (UINT64)(1 << McBank);
      }
    }
  }

  return ((McBankBits & MemBanksMask) != 0) ? TRUE : FALSE;
}


/**
  Get the core machine check banks.

  Returns a bitmap indicating core machine check banks.

  @retval CoreMcBankBitField

**/

UINT64
ReadCoreMcBankBitField ( 
  UINT8  Mode
  )
{
  UINT64  CoreMcBankBitField;
  UINT8   McBank;
  UINT16  McScope;
  UINT64  MaxBank;

  CoreMcBankBitField = 0;
  McBank = 0;
  McScope = 0;
  MaxBank = 0;

  if (Mode == MODE_EMCA) {
    CoreMcBankBitField = ReadCoreSmiSrc ();
    /* Uncore and core smisrc are bitmaps of Mc banks. Only the bits corresponding to the
     * banks that generate the SMI should be set.
     * This formula will calculate bitmask for the maximum possible bank that can be set
     * For instance, if Max Banks is 20 then the mask will be (0x100000 - 1) =  0xFFFFF.
     * Clearing bit 20 and above while passing lower bits 19-0*/
    MaxBank = GetNumMcBanksPerLp ();
    CoreMcBankBitField &= ((UINT64)1 << MaxBank)-1;
  } else {
    MaxBank = GetNumMcBanksPerLp ();
    for (McBank = 0, CoreMcBankBitField = 0; McBank < MaxBank; McBank++) {
      McScope = GetMcBankScope (McBank);
      if (McScope  == MC_SCOPE_CORE) {
        CoreMcBankBitField |= (1ULL << McBank);
      }
    }
  }
  return CoreMcBankBitField;
}

/**
  Clears core error sources.

  @param[in] McBankBitField   Bits to clear

  @retval None

**/
VOID
ClearCoreMcBankBitField ( 
  IN      UINT64    McBankBitField,
  IN      UINT8 Mode
  )
{
  if (Mode == MODE_EMCA) {
    ClearCoreSmiSrc (&McBankBitField);
  }
}

/**
  Clears uncore error sources.

  @param[in] McBankBitField   Bits to clear

  @retval None

**/
VOID
ClearUncoreMcBankBitField ( 
  IN      UINT64    McBankBitField,
  IN      UINT8 Mode
  )
{
  if (Mode == MODE_EMCA) {
    ClearUncoreSmiSrc (&McBankBitField);
  }
}

/**
  Signal a MCE to the OS

  This function will signal an MCE to the OS

  @param[in] LogicalCpu  Logical Cpu Number
  @param[in] SaveStateType  Msr or Mmio smm save state area
  @param[in] Broadcast  Whether MCE should be broadcast
  @param[in] Lmce  if Lmce was signaled by this thread 

  @retval Status.

**/
EFI_STATUS
SignalMceToOs ( 
  IN      UINTN  LogicalCpu,
  IN      UINT32  SaveStateType,
  IN      BOOLEAN Broadcast,
  IN      BOOLEAN Lmce
)
{
  UINTN                 NumberOfCpus;
  MCEMSR_ARGS_STRUCT    MceArgs;

  NumberOfCpus = gSmst->NumberOfCpus;
  
  if(Broadcast == FALSE && Lmce == FALSE) {
      return EFI_SUCCESS;
  }

  if (SaveStateType == 0) {
    //MMIO
    SignalMceMmio (LogicalCpu);
  } else {
    //MSR
    MceArgs.LogicalCpu = LogicalCpu;
    MceArgs.Sync = TRUE;
    if (gSmst->CurrentlyExecutingCpu != LogicalCpu) {
      SmmBlockingStartupThisAp (SignalMceMsr,
                                LogicalCpu,
                                &MceArgs
                                );
    } else {
      SignalMceMsr(&MceArgs);
    }
  }
  return EFI_SUCCESS;
}



/**
  Signal a CMCI to the OS

  This function will signal an CMCI to the OS

  @param[in] BankIndex  Bank Index of the CPU.

  @param[in] ApicId     APIC ID of the CPU.

**/

VOID
SignalCmciToOs (  
  IN      UINT32  BankIndex,
  IN      UINT32  ApicId
)
{
  UINT16              CmciScope;
  UINTN               CoreBits;
  UINTN               ThreadBits;
  UINT8               Socket;
  UINT8               Core;
  UINT8               Thread;
  UINTN               CpuNumber;
  EFI_STATUS          Status;
  CMCI_ARGS_STRUCT    CmciVar;
  UINT32              NumThread;
  UINT32              NumCore;
  EFI_CPU_PHYSICAL_LOCATION Location;

  GetCoreAndSMTBits (&ThreadBits,&CoreBits);
  Socket  = (UINT8)(ApicId >> (CoreBits+ThreadBits));
  Core    = (UINT8)((ApicId >> ThreadBits) & ~((~0) << CoreBits));
  Thread  = (UINT8)(ApicId & ~((~0) << ThreadBits));

  InitializeSpinLock (&CmciVar.ApBusy);

  CmciScope = GetMcBankScope (BankIndex);
  switch (CmciScope) {
    case MC_SCOPE_CORE:
      // Code to calculate the logical processor and assign it to cpuNumber
      NumThread = 1 << ThreadBits;
      for (Thread = 0; Thread < NumThread; Thread++){
        CpuNumber = GetTheCpuNumber (Socket, Core, Thread);
        if (CpuNumber!= GETTHECPUNUMBERFAILED){
          Location.Core = Core;
          Location.Package = Socket;
          Location.Thread = Thread;
          CmciVar.ApicId = GetApicIdByLoc(&Location);
            CmciVar.BankIndex = BankIndex;

          if (CpuNumber == gSmst->CurrentlyExecutingCpu) {
              SignalCmci (&CmciVar);
              // no return here because it will be a for loop sometime in future
          } else {
            // not current CPU use smmstartup to call fn
            AcquireSpinLock (&CmciVar.ApBusy);
            Status = gSmst->SmmStartupThisAp (SignalCmci,
                                      CpuNumber,
                                      &CmciVar);
            if (Status == EFI_SUCCESS) {
              AcquireSpinLock (&CmciVar.ApBusy);
            }
            ReleaseSpinLock (&CmciVar.ApBusy);
          }
        } else {
          // wrong CPU number we should not be here
          continue;
        }
      }
    break;

    case MC_SCOPE_PKG:
      // Code to calculate the logical processor and assign it to cpuNumber
      NumThread = 1 << ThreadBits;
      NumCore = 1 << CoreBits;
      for (Core = 0; Core < NumCore; Core++) {
        for (Thread = 0; Thread < NumThread; Thread++) {
          CpuNumber = GetTheCpuNumber (Socket, Core, Thread);
          if (CpuNumber != GETTHECPUNUMBERFAILED) {
            Location.Core = Core;
            Location.Package = Socket;
            Location.Thread = Thread;
            CmciVar.ApicId = GetApicIdByLoc(&Location);
            CmciVar.BankIndex = BankIndex;
            InitializeSpinLock (&CmciVar.ApBusy);
            if (CpuNumber == gSmst->CurrentlyExecutingCpu) {
              SignalCmci (&CmciVar);
              // no return here because it will be a for loop sometime in future
            } else {
              // not current CPU use smmstartup to call fn
              AcquireSpinLock (&CmciVar.ApBusy);
              Status = gSmst->SmmStartupThisAp (SignalCmci,
                                        CpuNumber,
                                        &CmciVar);
              if (Status == EFI_SUCCESS) {
                AcquireSpinLock (&CmciVar.ApBusy);
              }
              ReleaseSpinLock (&CmciVar.ApBusy);
            }
          } else {
            // wrong CPU number we should not be here
            continue;
          }
        }
      }
      break;
    default:
      return;
  }
  return;
}

/**
  Checks an EMCA Smi source.

  @param[out] ErrorFound  - Highest severity error detected

  @retval ErrorFound      - TRUE if error, FALSE if no error

**/
VOID
EmcaSmiSrc (
      OUT   VOID            *ErrorFound
)
{
  BOOLEAN                   *RetVal;

  RetVal = (BOOLEAN *)ErrorFound;
  *RetVal = FALSE;  

  if (ReadCoreSmiSrc () != 0 || ReadUncoreSmiSrc () != 0) {
    *RetVal = TRUE;
  }
}



