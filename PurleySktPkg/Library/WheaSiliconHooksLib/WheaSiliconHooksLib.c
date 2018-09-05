/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

Copyright (c) 2009-2016, Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file WheaSiliconHooks.c
   
    Whea Silicon hook functions and Silicon data, which needs to be 
    ported.

**/
#include <Register/PchRegsPcie.h>
#include "WheaSiliconHooksLib.h"
#include "DimmIsolationFlow.h"
#include <UncoreCommonIncludes.h>
#include "CpuPciAccess.h"
#include <IndustryStandard/WheaDefs.h>
#include <Protocol/CrystalRidge.h>

#pragma optimize( "", off)

//
// Data definitions & structures
//
// 
// WHEA error injection 
//
// IOH_DATA structure is used in runtime when IOH UDS protocol is not available. Always have this array for MAX 8 sockets.
IOH_INFO  IohInfo[8] = {
            {(UINT8) -1, (UINT8) -1, (UINT8) -1, (UINT8) -1  },
            {(UINT8) -1, (UINT8) -1, (UINT8) -1, (UINT8) -1  },
            {(UINT8) -1, (UINT8) -1, (UINT8) -1, (UINT8) -1  },
            {(UINT8) -1, (UINT8) -1, (UINT8) -1, (UINT8) -1  },
            {(UINT8) -1, (UINT8) -1, (UINT8) -1, (UINT8) -1  },
            {(UINT8) -1, (UINT8) -1, (UINT8) -1, (UINT8) -1  },
            {(UINT8) -1, (UINT8) -1, (UINT8) -1, (UINT8) -1  },
            {(UINT8) -1, (UINT8) -1, (UINT8) -1, (UINT8) -1  }
};


EFI_WHEA_BOOT_PROTOCOL		*mWheaBootProtocolLib = NULL;
static EFI_CPU_CSR_ACCESS_PROTOCOL *mCpuCsrAccess = NULL;
IIO_UDS						*mIioUdsLib = NULL;
EFI_MEM_RAS_PROTOCOL		*mMemRasLib = NULL;
EFI_CRYSTAL_RIDGE_PROTOCOL  *m2CrystalRidgeProtocol;

static PREV_BOOT_ERR_GLOBAL_DATA	mPrevBootErrData;
PREV_BOOT_ERR_SRC_HOB		*mPrevBootErrSrcHob = NULL;

// Trigger action 1 is added to lock iio_err_inj lck after pcie error injection
UINT64  EinjIioErrInjLockSwSmi;

EFI_STATUS
EFIAPI
WheaSiliconHooksLib (VOID)
{
	EFI_STATUS	Status = 0;
	EFI_HOB_GUID_TYPE	*GuidHob;
	
	RASDEBUG ((DEBUG_ERROR, "Initialize WheaSiliconHooksLib\n"));

	//
	// Memory RAS protocolER
	//
	Status = gSmst->SmmLocateProtocol (&gEfiMemRasProtocolGuid, NULL, &mMemRasLib);
	ASSERT_EFI_ERROR (Status);

	Status = gSmst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
    ASSERT_EFI_ERROR (Status);
	 
	// Locate the WheaBootprotocol.
	Status = gBS->LocateProtocol (&gEfiWheaBootProtocolGuid,NULL,&mWheaBootProtocolLib);
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to Remove WHEA Depex from RAS Drivers
//	ASSERT_EFI_ERROR (Status);
	if(EFI_ERROR(Status)){
	    DEBUG ((DEBUG_ERROR, "gEfiWheaBootProtocolGuid Not Located\n"));
	}
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to Remove WHEA Depex from RAS Drivers

	// Locate the WhereaaBootprotocol.
  Status = gSmst->SmmLocateProtocol (&gEfiCrystalRidgeSmmGuid, NULL, &m2CrystalRidgeProtocol);
	ASSERT_EFI_ERROR (Status);

	// Initialize with null pointer
	mPrevBootErrSrcHob = NULL;

	// Get the HOB Pointer to update the new error source
	GuidHob = GetFirstGuidHob (&gPrevBootErrSrcHobGuid);
	ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
	mPrevBootErrSrcHob = (PREV_BOOT_ERR_SRC_HOB *)GET_GUID_HOB_DATA(GuidHob);
	InitializeIsolationFlow();
  
	return EFI_SUCCESS;
}


/*
  Checks and updates the previous boot fatal CBO errors. 

@param I/P : McBankErrInfo contains the Prev boot fatal QPI/PCU error type.
  
@retval Status  : Success or  failure

*/
EFI_STATUS
UpdateChaError(
  MCBANK_ERR_INFO     *McBankErrInfo
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CPU_DEV_INFO    CpuInfo; 
  EFI_CPUID_REGISTER Register;
  
  AsmCpuid (EFI_CPUID_VERSION_INFO, &Register.RegEax, &Register.RegEbx, &Register.RegEcx, &Register.RegEdx);
  // Need to port other parameters of CPU_DEV_INFO if required
  ZeroMem(&CpuInfo, sizeof(CPU_DEV_INFO));
  CpuInfo.ProcessorType = GEN_ERR_PROC_TYPE_IA32_X64;
  CpuInfo.ProcessorISA = GEN_ERR_PROC_ISA_X64;
  CpuInfo.ProcessorErrorType = UEFI_PROC_CACHE_ERR_TYPE;  // Cache error.
  CpuInfo.Operation = 0;    // Generic Error
  CpuInfo.ProcessorApicId = (UINT8)McBankErrInfo->ApicId;    // Processor ID
  CpuInfo.VersionInfo = Register.RegEax;
  // if Mcbanks status is cleared and whea is enabled . Report the error using whea.
  if((mPrevBootErrData.SetupMcBankStsClear == 1) && (mWheaBootProtocolLib != NULL))
    Status = mWheaBootProtocolLib->WheaBootElogProcessor(mWheaBootProtocolLib, GEN_ERR_SEV_CPU_GENERIC_FATAL, &CpuInfo); 

  return Status;
}

/*
  Checks and updates the previous boot fatal QPI/PCU errors. 

@param I/P : McBankErrInfo contains the Prev boot fatal QPI/PCU error type.
  
@retval Status  : Success or  failure

*/
EFI_STATUS
UpdateQpiPcuError(
  MCBANK_ERR_INFO     *McBankErrInfo
  )
{
  EFI_STATUS         Status = EFI_SUCCESS;
  CPU_DEV_INFO       CpuInfo; 
  EFI_CPUID_REGISTER Register;

  AsmCpuid (EFI_CPUID_VERSION_INFO, &Register.RegEax, &Register.RegEbx, &Register.RegEcx, &Register.RegEdx);
  // Need to port other parameters of CPU_DEV_INFO if required
  ZeroMem(&CpuInfo, sizeof(CPU_DEV_INFO));
  CpuInfo.ProcessorType = GEN_ERR_PROC_TYPE_IA32_X64;
  CpuInfo.ProcessorISA = GEN_ERR_PROC_ISA_X64;
  CpuInfo.ProcessorErrorType = 0; // Unknown error.
  CpuInfo.Operation = 0;    // Generic Error
  CpuInfo.ProcessorApicId = (UINT8)McBankErrInfo->ApicId;    // Processor ID
  CpuInfo.VersionInfo = Register.RegEax;
  // if Mcbanks status is cleared and whea is enabled . Report the error using whea.
  if((mPrevBootErrData.SetupMcBankStsClear == 1) && (mWheaBootProtocolLib != NULL))
    Status = mWheaBootProtocolLib->WheaBootElogProcessor(mWheaBootProtocolLib, GEN_ERR_SEV_CPU_GENERIC_FATAL, &CpuInfo); 

  return Status;
}

/*
  Checks and updates the previous boot fatal Core errors. 

@param I/P : McBankErrInfo contains the Prev boot fatal Core error type.
  
@retval Status  : Success or  failure

*/
EFI_STATUS
UpdateCoreError(
  MCBANK_ERR_INFO     *McBankErrInfo
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CPU_DEV_INFO    CpuInfo; 
  EFI_CPUID_REGISTER Register;
  
  AsmCpuid (EFI_CPUID_VERSION_INFO, &Register.RegEax, &Register.RegEbx, &Register.RegEcx, &Register.RegEdx);
  // Need to port other parameters of CPU_DEV_INFO if required
  ZeroMem(&CpuInfo, sizeof(CPU_DEV_INFO));
  CpuInfo.ProcessorType = GEN_ERR_PROC_TYPE_IA32_X64;
  CpuInfo.ProcessorISA = GEN_ERR_PROC_ISA_X64;
  CpuInfo.ProcessorErrorType = UEFI_PROC_MICRO_ARCH_ERR_TYPE; // MicroArchitecture error.
  CpuInfo.Operation = 0;    // Generic Error
  CpuInfo.ProcessorApicId = (UINT8)McBankErrInfo->ApicId;    // Processor ID
  CpuInfo.VersionInfo = Register.RegEax;
  // if Mcbanks status is cleared and whea is enabled . Report the error using whea.
  if((mPrevBootErrData.SetupMcBankStsClear == 1) && (mWheaBootProtocolLib != NULL))
    Status = mWheaBootProtocolLib->WheaBootElogProcessor(mWheaBootProtocolLib, GEN_ERR_SEV_CPU_GENERIC_FATAL, &CpuInfo); 

  return Status;
}

EFI_STATUS
UpdateMemoryError(
  MCBANK_ERR_INFO     *McBankErrInfo
  )
{
	EFI_STATUS        Status;
	MEMORY_DEV_INFO   MemInfo;


    Status = IsolateFaultyDimm(McBankErrInfo, &MemInfo);

    if (EFI_ERROR(Status))
      return Status;
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to Remove WHEA Depex from RAS Drivers
    if(mWheaBootProtocolLib != NULL) {
        Status = mWheaBootProtocolLib->WheaBootElogMemory(mWheaBootProtocolLib, GEN_ERR_SEV_PLATFORM_MEMORY_FATAL, &MemInfo);
    }
    return Status;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to Remove WHEA Depex from RAS Drivers
}


/*
  Checks and updates the previous boot fatal pci-ex errors. 
  This function is currently supported for Root ports only.

@param I/P : McBankErrInfo contains the Prev boot fatal memory error type.
  
@retval Status  : Success or  failure

*/
EFI_STATUS
UpdatePciExError(
  PCI_EX_ERR_INFO   *PcieDev
  )
{
  EFI_STATUS        Status = EFI_SUCCESS;
  PCIE_PCI_DEV_INFO ErrPcieDev;
  UINT16            Data8 = 0;
  
  ErrPcieDev.Segment  = PcieDev->Segment;
  ErrPcieDev.Bus      = PcieDev->Bus;
  ErrPcieDev.Device   = PcieDev->Device;  
  ErrPcieDev.Function = PcieDev->Function;

  Data8 = MmioRead8(MmPciAddress(ErrPcieDev.Segment, ErrPcieDev.Bus, ErrPcieDev.Device, ErrPcieDev.Function, PCI_HEADER_TYPE_OFFSET));


  // Check for type 0 header.
  if( (Data8 & 0x03f) == 0)   {
    if( mWheaBootProtocolLib != NULL )  {
      Status = mWheaBootProtocolLib->WheaBootElogPcieRootDevBridge(mWheaBootProtocolLib, GEN_ERR_SEV_PCIE_FATAL, &ErrPcieDev);       
    }
  }
  return  Status;

}

EFI_STATUS
UpdateMemoryErrorFromMc5Shadow(
  //UINT64      PhyAddr,
  UINT8       SocketId
  )
/*
  Checks and updates the previous boot fatal memory errors This is added for WHQL testing since the Previous boot error record hob is not created in MRC. 

@param I/P : Phyisical adrress and socket number.

  
@retval Status  : Success or  failure

*/
{

  EFI_STATUS  Status = EFI_SUCCESS;
  MEMORY_DEV_INFO     MemInfo;
  //UINT8       MemoryControllerId = 00;
  //UINT8       ChannelId = 00;
  //UINT8       DimmSlot = 00;
  //UINT8       DimmRank = 00;
  //UINT32      Row = 00;
  //UINT32      Col = 00;
  //UINT8       Bank = 00;
  //UINT64      McBankAddValue;

  MemInfo.Node = SocketId;
  MemInfo.ValidBits = PLATFORM_MEM_NODE_VALID;
/*
  if( (PhyAddr != 0) && (mMemRasLib != NULL) ) {
    mMemRasLib->SystemAddressToDimmAddress (PhyAddr,&SocketId,&MemoryControllerId,&ChannelId,&DimmSlot,&DimmRank,&Row,&Col,&Bank);
    MemInfo.ValidBits |= PLATFORM_MEM_PHY_ADDRESS_VALID;  
    MemInfo.Dimm      = DimmSlot;
    MemInfo.Column    = Col;
    MemInfo.Bank      = Bank;
    MemInfo.Row       = Row;
    MemInfo.ValidBits |=  PLATFORM_MEM_CARD_VALID | PLATFORM_MEM_MODULE_VALID | PLATFORM_MEM_ROW_VALID | PLATFORM_MEM_COLUMN_VALID;
  }
*/
  // if Mcbanks status is cleared and whea is enabled . Report the error using whea.
  if( mWheaBootProtocolLib != NULL )
    Status = mWheaBootProtocolLib->WheaBootElogMemory(mWheaBootProtocolLib, GEN_ERR_SEV_PLATFORM_MEMORY_FATAL, &MemInfo);

  return Status;
}


VOID
CheckAndUpdatePciExError()
{
  UINT8 RootBridgeLoop;
  UINT8 Bus = 0;
  UINT8 Device = 0;
  UINT8 Function = 0;
  UINT8 Port = 0;
  UINT16 Data16 = 0;
  UINT32 Data32 = 0;
  PCIE_PCI_DEV_INFO ErrPcieDev;
  EFI_STATUS  Status = EFI_SUCCESS;

  for ( RootBridgeLoop =0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {

    if (IohInfo[RootBridgeLoop].Valid == 1) {
   
      for(Port = 0; Port < NUMBER_PORTS_PER_SOCKET; Port++) {

        Bus = IohInfo[RootBridgeLoop].Port[Port].Bus; 
        Device = IohInfo[RootBridgeLoop].Port[Port].Device; 
        Function = IohInfo[RootBridgeLoop].Port[Port].Function; 

        if((Bus == 0) && (Device == 0) && (Function == 0)) continue;
        //
        // Check if device is present
        //
        Data16 = MmioRead16 (MmPciAddress(RootBridgeLoop, Bus, Device, Function, PCI_VENDOR_ID_OFFSET));

        if (Data16 == 0xFFFF) continue;
          Data32 = MmioRead32 (MmPciAddress(RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET(UNCERRSTS_IIO_PCIE_REG)));

        if(Data32 & 0x3fffff)  {  // Bits[22:31] reserved
          //if( (mPrevBootErrData.SetupMcBankStsClear == 1) && (mWheaBootProtocollib != NULL) )  {
          if(mWheaBootProtocolLib != NULL )  {
            ErrPcieDev.Segment  = 00;
            ErrPcieDev.Bus      = Bus;
            ErrPcieDev.Device   = Device;  
            ErrPcieDev.Function = Function;
            Status = mWheaBootProtocolLib->WheaBootElogPcieRootDevBridge(mWheaBootProtocolLib, GEN_ERR_SEV_PCIE_FATAL, &ErrPcieDev);       
          }
        }
      }
    }
  }

  // PCH root ports
  Device  = (UINT8)PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS;
  for(Function = 0; Function <= PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8; Function++) {
      // Check if device is present
	  Data16 = MmioRead16 (MmPciAddress(IohInfo[0].SocketId, IohInfo[0].BusNum, Device, Function, PCI_VENDOR_ID_OFFSET));

	  if (Data16 == 0xFFFF)
		continue;

	  Data32 = MmioRead32 (MmPciAddress(IohInfo[0].SocketId, IohInfo[0].BusNum, Device, Function, PCH_PCIE_RROT_PORT_AER_UES));

      if(Data32 & 0x3fffff)  {  // Bits[22:31] reserved
        if( (mPrevBootErrData.SetupMcBankStsClear == 1) && (mWheaBootProtocolLib != NULL) )  {
          ErrPcieDev.Segment  = 00;
          ErrPcieDev.Bus      = IohInfo[0].BusNum;
          ErrPcieDev.Device   = Device;  
          ErrPcieDev.Function = Function;
          Status = mWheaBootProtocolLib->WheaBootElogPcieRootDevBridge(mWheaBootProtocolLib, GEN_ERR_SEV_PCIE_FATAL, &ErrPcieDev);       
        }
      }
    }
}

/*
  Checks and updates the previous boot errors. This function is currentlyimplemented for McBank errors.

   @param mPrevBootErrSrcHob contains the Prev boot fatal error sources.
  
@retval Status  : Success or  failure

*/
EFI_STATUS
ChecAndUpdatePrevBootErrors(
  VOID
  )
{
  EFI_STATUS                              Status = EFI_SUCCESS;
  UINT8                                   Skt = 0;
  UINT8                                   Length;
  UINT8                                   node = 0;
  MCBANK_ERR_INFO                         *McBankErrInfo;
  PCI_EX_ERR_INFO                         *PcieDev;
  IA32_MCI_STATUS_MSR_STRUCT              MsrIa32MCiStatusReg; 
  MCI_STATUS_SHADOW_N1_M2MEM_MAIN_STRUCT  Mc5ShadowStatus;

  if( mPrevBootErrSrcHob == NULL)  {
    // For WHQl testing - Previous boot error collection hob is not yet inplemented, So directly reading from the sticky Shadow regisetrs to get the error information for error type 32 of EINJ table.
    if(mIioUdsLib == NULL) {
      return EFI_NOT_FOUND;
    }
    for (node = 0; node < MC_MAX_NODE; node++) {
      Skt =  NODE_TO_SKT(node);
      if ( mMemRasLib->SystemMemInfo->Socket[Skt].imcEnabled[NODE_TO_MC(node)] == 00 ) 
        continue;
                  
      Mc5ShadowStatus.Data = mCpuCsrAccess->ReadCpuCsr(  Skt, NODE_TO_MC(node),MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG);
      if( Mc5ShadowStatus.Bits.valid && Mc5ShadowStatus.Bits.uc && Mc5ShadowStatus.Bits.pcc) {
        UpdateMemoryErrorFromMc5Shadow( Skt);
      }
    }
    return EFI_SUCCESS;
  }
  //No error record come out of the function.
  if(mPrevBootErrSrcHob->Length <= 2) return EFI_SUCCESS;

  Length = 2;  // Offset of the Error source from HOB.

  if( Length <= mPrevBootErrSrcHob->Length) {

    McBankErrInfo = (MCBANK_ERR_INFO *)( (UINTN)mPrevBootErrSrcHob + Length);
    switch (McBankErrInfo->Type)  {
    
    case McBankType :
      
      // Check for memory Mcbank errors.
      if(
        ((McBankErrInfo->McBankNum <= MEM_IMC_MCBANK_NUM_END) && (McBankErrInfo->McBankNum >= MEM_IMC_MCBANK_NUM_START)) ||
        ((McBankErrInfo->McBankNum <= MEM_M2M_MCBANK_NUM_END) && (McBankErrInfo->McBankNum >= MEM_M2M_MCBANK_NUM_START))
        ) {
          
        MsrIa32MCiStatusReg.Data = McBankErrInfo->McBankStatus;
        //check for fatal error.
        if (MsrIa32MCiStatusReg.Bits.uc == 1 && MsrIa32MCiStatusReg.Bits.pcc == 1){
          Status = UpdateMemoryError(McBankErrInfo);
          if(EFI_ERROR(Status)) return Status;
        }
      }  // check for Processor core errors
      else if( (McBankErrInfo->McBankNum <= CORE_MCBANK_NUM_END) && (McBankErrInfo->McBankNum >= CORE_MCBANK_NUM_START) ) {
        MsrIa32MCiStatusReg.Data = McBankErrInfo->McBankStatus;
        //check for fatal error.
        if (MsrIa32MCiStatusReg.Bits.uc == 1 && MsrIa32MCiStatusReg.Bits.pcc == 1){
          Status = UpdateCoreError(McBankErrInfo);
          if(EFI_ERROR(Status)) return Status;
        }           
      }  // check for CBO errors
      else if ( 
        (McBankErrInfo->McBankNum <= CHA_MCBANK_NUM_END) && (McBankErrInfo->McBankNum >= CHA_MCBANK_NUM_START)
        ) {
        MsrIa32MCiStatusReg.Data = McBankErrInfo->McBankStatus;
        //check for fatal error.
        if (MsrIa32MCiStatusReg.Bits.uc == 1 && MsrIa32MCiStatusReg.Bits.pcc == 1){
          Status = UpdateChaError(McBankErrInfo);
          if(EFI_ERROR(Status)) return Status;
        }
      } // check for QPI errors.
      else if ( 
        ((McBankErrInfo->McBankNum <= KTI0_MCBANK_NUM_END) && (McBankErrInfo->McBankNum >= KTI0_MCBANK_NUM_START)) ||
        (McBankErrInfo->McBankNum == KTI1_MCBANK_NUM) || 
        (McBankErrInfo->McBankNum == KTI2_MCBANK_NUM)
        ) {
        MsrIa32MCiStatusReg.Data = McBankErrInfo->McBankStatus;
        //check for fatal error.
        if (MsrIa32MCiStatusReg.Bits.uc == 1 && MsrIa32MCiStatusReg.Bits.pcc == 1){
          Status = UpdateQpiPcuError(McBankErrInfo);
          if(EFI_ERROR(Status)) return Status;
        }
      }
       
      Length += sizeof (MCBANK_ERR_INFO);

      break;

    case PciExType :

      PcieDev  = (PCI_EX_ERR_INFO *) (UINTN)(McBankErrInfo);
      // Update PCI-ex errors
      UpdatePciExError(PcieDev);
      mPrevBootErrData.PciexErrFound = TRUE;
      Length += sizeof (PCI_EX_ERR_INFO);
      
      break;

    //case CsrOtherType :

      //Length += sizeof (CSR_ERR_INFO);

      //break;

    default:
      return EFI_NOT_FOUND;

    }
  }

  return Status;
}



/**

    This function will write to Internal MSR (ERR_INJ_LCK_UNLOCK_CTL_MSR) to lock error injection mechanism. 
    See Machine Check Architecture HAS for details. 

    @param node - Node Id 

    @retval None

**/
void
LockInjLogic(
  UINT8 *node
)
{
  UINT64      Data64;

  Data64 = AsmReadMsr64 (MC_ERR_INJ_LCK);
  Data64 |= 1;
  AsmWriteMsr64 (MC_ERR_INJ_LCK, Data64);

}
/**

    By default error injection mechanism comes up disabled. 
    This function will write to Internal MSR (ERR_INJ_LCK_UNLOCK_CTL_MSR) to unlock the mechanism. 
    See Machine Check Architecture HAS for details. 

    @param node - Node Id 

    @retval None

**/
void
UnLockInjLogic(
  UINT8 *node
)
{
  UINT64      Data64;

  Data64 = AsmReadMsr64 (MC_ERR_INJ_LCK);
  Data64 &= ~(1);
  AsmWriteMsr64 (MC_ERR_INJ_LCK, Data64);

}
/**

    Disable data poison in the processor by writing to MCG_CONTAIN MSR. 

    @param node - Node Id

    @retval None

**/
void
DisablePoison(
  UINT8 *node
)
{
  UINT64      Data;
  Data = AsmReadMsr64(MSR_MCG_CONTAIN);
  Data &=  ~(BIT0 | BIT1);  //Cannot just disable Poison,  so disabling Viral as well.
  AsmWriteMsr64 (MSR_MCG_CONTAIN, Data);

}
/**

    Enable data poison in the processor by writing to MCG_CONTAIN MSR. 

    @param node - Node Id

    @retval None

**/
void
EnablePoison(
  UINT8 *node
)
{
  UINT64      Data;
  Data = AsmReadMsr64(MSR_MCG_CONTAIN);
  Data |=  BIT0;                        // just enable poison
  AsmWriteMsr64 (MSR_MCG_CONTAIN, Data);

}

VOID
SetupLlcInjRegister(
  WHEA_EINJ_PARAM_BUFFER *EinjParam)
{
  UINT8   Cha;
  UINT8   SocketId = 0;
  UINT32  Data32;
  UINT32  InjAddrValue;
  UINT64  PhyAddr;
  
  SADDBGMM0_CHA_PMA_STRUCT      SadDebugMm0;
  SADDBGMM1_CHA_PMA_STRUCT      SadDebugMm1;
  SADDBGMM2_CHA_PMA_STRUCT      SadDebugMm2;
  LLCERRINJ_CHA_MISC_STRUCT     LLCErrInj;

  PhyAddr = EinjParam->EinjAddr;
  SocketId = EinjParam->Skt;
  InjAddrValue = INJECTION_ADDRESS_VALUE;
  
  // Bring the cache line to M state
  Data32  = *(volatile  UINT32 *)(UINTN)(PhyAddr);
  *(volatile  UINT32 *)(UINTN)(PhyAddr) = 0x12345678;
  AsmFlushCacheLine ((VOID*)PhyAddr);
  AsmMfence();
  TurnOnQuiesce();
 
  Data32  = *(volatile  UINT32 *)(UINTN)(PhyAddr);
  *(volatile  UINT32 *)(UINTN)(PhyAddr) = InjAddrValue;
  TurnOffQuiesce();
  
  for (Cha = 0; Cha < mIioUdsLib->PlatformData.CpuQpiInfo[SocketId].TotCha ; Cha++) {
    if((mIioUdsLib->PlatformData.CpuQpiInfo[SocketId].ChaList & (1 << Cha)) == 0) {
      continue;
    }

    // Set ErrInjMode bit to inject once only and set inj3bdataerr bit for triple bit data error in LLC when Address match happens
    LLCErrInj.Data = mCpuCsrAccess->ReadCpuCsr( SocketId, Cha, LLCERRINJ_CHA_MISC_REG );
    LLCErrInj.Bits.errinjmode = 01;
    LLCErrInj.Bits.inj3bdataerr = 01;
    RASDEBUG((DEBUG_INFO,"LLC Debug data %x\n",LLCErrInj.Data));
    mCpuCsrAccess->WriteCpuCsr( SocketId, Cha, LLCERRINJ_CHA_MISC_REG, LLCErrInj.Data);

    // Set address Match and Mask bits
    // SADDBGMM0[31:0]   - mask field for address[37:6]
    SadDebugMm0.Data = mCpuCsrAccess->ReadCpuCsr( SocketId, Cha, SADDBGMM0_CHA_PMA_REG );
    SadDebugMm0.Bits.mask_37_6  = 00;
    mCpuCsrAccess->WriteCpuCsr( SocketId, Cha, SADDBGMM0_CHA_PMA_REG, SadDebugMm0.Data);

    SadDebugMm1.Data = mCpuCsrAccess->ReadCpuCsr( SocketId, Cha, SADDBGMM1_CHA_PMA_REG );
    SadDebugMm1.Bits.mask_45_38 = 00;
    SadDebugMm1.Bits.match_29_6 = (UINT32)(PhyAddr >> 06 );
    mCpuCsrAccess->WriteCpuCsr( SocketId, Cha, SADDBGMM1_CHA_PMA_REG, SadDebugMm1.Data);

    // SADDBDMM2[15:0] - match field for address[45:30]
    SadDebugMm2.Data = mCpuCsrAccess->ReadCpuCsr( SocketId, Cha, SADDBGMM2_CHA_PMA_REG );
    SadDebugMm2.Bits.match_45_30      = (UINT32)(PhyAddr >> 30 );
    SadDebugMm2.Bits.errinjaddrvalid    = 1;
    SadDebugMm2.Bits.errinjopcodevalid  = 1;
    mCpuCsrAccess->WriteCpuCsr( SocketId, Cha, SADDBGMM2_CHA_PMA_REG, SadDebugMm2.Data);

  }
}
/**

    enables reporting of Uncorrected patrol scrub data errors downgraded to corrected data errors.

    @param node - Node Id mapping to every memory controller in the system.  
    
    @retval None

**/
void
IntDownGrade(
  UINT8 *node
)
{
  UINT64      Data64;

  // bit 2 of MSR 0x17f
  Data64 = AsmReadMsr64 (MC_PS_INT_DOWNGRADE);
  Data64 |= V_PS_INT_DOWNGRADE;
  AsmWriteMsr64 (MC_PS_INT_DOWNGRADE, Data64);

}

/**

    Inject Pcie related Errors. 
    
    PcieSBDF must be a device capable of support error injection

    @param  PcieSBDF -  BYTE 3 - PCIe Segment
                                 BYTE 2 - Bus Number
                                 BYTE 1 - Device Number[BITs 7:3], Function Number BITs[2:0]
                                 BYTE 0 - RESERVED
    
    @param ErrorToInject   - BIT 8 set - Pci-ex fatal error (Receiver error with fatal error severity)
                                       BIT 7 set - Pci-ex Un correctable error (Receiver error with uncorrected non fatal error severity)
                                       BIT 6 - Pci-ex correctable error ( Completion timeout error)

    @retval EFI_SUCCESS - Injection Success
    
**/
EFI_STATUS
InjectPciExError (
 IN   UINT32      PcieSBDF,
 IN   UINT32      ErrorToInject,
 IN   WHEA_EINJ_TRIGGER_ACTION_TABLE  *EinjAction,
 IN   WHEA_EINJ_PARAM_BUFFER          *EinjParam
 )
{

  UINT8   Seg;
  UINT8   Bus;
  UINT8   Dev;
  UINT8   Func;
  UINT8   node = 0;  
  UINTN   thread = 0;
  UINT32  PciInjdevAddr = 00;
  UINT32  PciInjdevAddr1 = 00;
  ERRINJCON_IIO_PCIE_STRUCT ErrInjCon;
  UNCERRSEV_IIO_PCIE_STRUCT UncErrSev;

  Seg = (PcieSBDF >> 24) & (0xFF);
  Bus = (PcieSBDF >> 16) & (0xFF);
  Dev = (PcieSBDF >> 11) & (0x1F);
  Func= (PcieSBDF >> 8) & (0x7);

  //PciInjdevAddr =  (UINT32)mIioUdsLib->PlatformData.PciExpressBase | PCI_PCIE_ADDR(Bus, Dev, Func, ONLY_REGISTER_OFFSET(ERRINJCON_IIO_PCIE_REG));   // ERRINJCAP
  PciInjdevAddr =  (UINT32)MmPciAddress(Seg, Bus, Dev, Func, ONLY_REGISTER_OFFSET(ERRINJCON_IIO_PCIE_REG));   // ERRINJCAP

  ErrInjCon.Data = *((volatile UINT16 *) ((UINTN) PciInjdevAddr));

  for (thread = 0; thread < gSmst->NumberOfCpus; thread++) {
    gSmst->SmmStartupThisAp (UnLockInjLogic,
                 thread,
                 &node);
  }
  UnLockInjLogic(&node);
  ErrInjCon.Data = *((volatile UINT16 *) ((UINTN) PciInjdevAddr));
  ErrInjCon.Bits.errinjdis  = 0;
  ErrInjCon.Bits.cause_rcverr = 0;
  ErrInjCon.Bits.cause_ctoerr = 0;
  *((volatile UINT16 *) ((UINTN) PciInjdevAddr)) = ErrInjCon.Data;
  ErrInjCon.Data = *((volatile UINT16 *) ((UINTN) PciInjdevAddr));
  PciInjdevAddr1 = (UINT32)mIioUdsLib->PlatformData.PciExpressBase | PCI_PCIE_ADDR(Bus, Dev, Func, ONLY_REGISTER_OFFSET(UNCERRSEV_IIO_PCIE_REG));   // UNCERRSEV_IIO_PCIE_REG
  UncErrSev.Data = *((volatile UINT32 *) ((UINTN) PciInjdevAddr1));

  if (ErrorToInject == INJECT_ERROR_PCIE_UE_FATAL ) {
    UncErrSev.Bits.completion_time_out_severity  = 1;
    ErrInjCon.Bits.errinjdis  = 0;
    ErrInjCon.Bits.cause_rcverr = 0;
    ErrInjCon.Bits.cause_ctoerr = 1;
  } else if (ErrorToInject == INJECT_ERROR_PCIE_UE_NON_FATAL ) {
    UncErrSev.Bits.completion_time_out_severity  = 0;
    ErrInjCon.Bits.errinjdis  = 0;
    ErrInjCon.Bits.cause_rcverr = 0;
    ErrInjCon.Bits.cause_ctoerr = 1;
   } else if (ErrorToInject == INJECT_ERROR_PCIE_CE ){
    ErrInjCon.Bits.errinjdis  = 0;
    ErrInjCon.Bits.cause_rcverr = 1;
    ErrInjCon.Bits.cause_ctoerr = 0;
   } else {
    ErrInjCon.Bits.cause_rcverr = 00;
    ErrInjCon.Bits.cause_ctoerr = 0;
  }
    
  *((volatile UINT32 *) ((UINTN) PciInjdevAddr1))  = UncErrSev.Data;  

  if(EinjParam->PcieErrInjActionTable == TRUE) {
    EinjAction->Trigger0.Operation                  = WHEA_EINJ_TRIGGER_ERROR_ACTION;
    EinjAction->Trigger0.Instruction                = INSTRUCTION_WRITE_REGISTER_VALUE;
    EinjAction->Trigger0.Flags                      = FLAG_NOTHING;
    EinjAction->Trigger0.Reserved8                  = 00;
    EinjAction->Trigger0.Register.AddressSpaceId    = 00;
    EinjAction->Trigger0.Register.RegisterBitWidth  = 0x20;
    EinjAction->Trigger0.Register.RegisterBitOffset = 0x00;
    EinjAction->Trigger0.Register.AccessSize        = 0x03;
    EinjAction->Trigger0.Register.Address           = PciInjdevAddr;
    EinjAction->Trigger0.Value                      = ErrInjCon.Data;
    EinjAction->Trigger0.Mask                       = 0xffffffff;
    
    // Trigger action 1 is added to lock iio_err_inj lck after pcie error injection
    EinjAction->Trigger1.Operation                  = WHEA_EINJ_TRIGGER_ERROR_ACTION;
    EinjAction->Trigger1.Instruction                = INSTRUCTION_WRITE_REGISTER_VALUE;
    EinjAction->Trigger1.Flags                      = FLAG_NOTHING;
    EinjAction->Trigger1.Reserved8                  = 00;
    EinjAction->Trigger1.Register.AddressSpaceId    = 01;
    EinjAction->Trigger1.Register.RegisterBitWidth  = 0x08;
    EinjAction->Trigger1.Register.RegisterBitOffset = 0x00;
    EinjAction->Trigger1.Register.AccessSize        = 0x01;
    EinjAction->Trigger1.Register.Address           = 0xB2;
    EinjAction->Trigger1.Value                      = EinjIioErrInjLockSwSmi;
    EinjAction->Trigger1.Mask                       = 0xffffffff;
  } else {
    *((volatile UINT16 *) ((UINTN) PciInjdevAddr)) = ErrInjCon.Data;

    for (thread = 0; thread < gSmst->NumberOfCpus; thread++) {
      gSmst->SmmStartupThisAp (LockInjLogic,
                   thread,
                   &node);
    }

    LockInjLogic(&node);
  }

  return EFI_SUCCESS;
}


/**

    Set HA Parity Check Enable.
    This function set parity check enable based on BIOS knob.T his is a workaround for Purley sighting 4930281.

    @param  enable - HA parity check enablement.

**/
VOID SetHAParityCheckEnable(
  BOOLEAN enable
  )
{
  UINT8                                   Socket;
  UINT8                                   Ch;
  MC0_DP_CHKN_BIT_MCDDC_DP_STRUCT         MC0DpChknBit;
#ifdef SV_HOOK
  DDRT_WDB_PAR_ERR_CTL_MC_2LM_STRUCT      WdbParErrCtl;
#endif /* SV_HOOKS */

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {

	//Check if socket is populated.
	if (IohInfo[Socket].Valid != 1)
      continue;

	for (Ch = 0; Ch < MAX_CHANNELS; Ch++) {
	  MC0DpChknBit.Data = mCpuCsrAccess->ReadCpuCsr(Socket, Ch, MC0_DP_CHKN_BIT_MCDDC_DP_REG);
	  if (enable) {
	    MC0DpChknBit.Bits.dis_ha_par_chk = 0;
	  } else {
	    MC0DpChknBit.Bits.dis_ha_par_chk = 1;
	  }
	  mCpuCsrAccess->WriteCpuCsr(Socket, Ch, MC0_DP_CHKN_BIT_MCDDC_DP_REG, MC0DpChknBit.Data);
	  MC0DpChknBit.Data = mCpuCsrAccess->ReadCpuCsr(Socket, Ch, MC0_DP_CHKN_BIT_MCDDC_DP_REG);

#ifdef SV_HOOK
	  WdbParErrCtl.Data = mCpuCsrAccess->ReadCpuCsr(Socket, Ch, DDRT_WDB_PAR_ERR_CTL_MC_2LM_REG);
	  if (enable) {
		WdbParErrCtl.Bits.dis_ha_par_chk = 0;
	  } else {
		WdbParErrCtl.Bits.dis_ha_par_chk = 1;
	  }
	  mCpuCsrAccess->WriteCpuCsr(Socket, Ch, DDRT_WDB_PAR_ERR_CTL_MC_2LM_REG, WdbParErrCtl.Data);
	  WdbParErrCtl.Data = mCpuCsrAccess->ReadCpuCsr(Socket, Ch, DDRT_WDB_PAR_ERR_CTL_MC_2LM_REG);

#endif /* SV_HOOKS */
	}
  }
}
