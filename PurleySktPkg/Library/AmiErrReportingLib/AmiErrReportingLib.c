//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//
//*****************************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		AmiErrReportingLib.c
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "AmiErrReportingLib.h"


EFI_PLATFORM_RAS_POLICY_PROTOCOL  *gRasPolicy = NULL;
STATIC EMCA_PHLIB_PRIV mEmcaPh = { 0 };


/**
  FillMemRecordBeforeHandling - Fill Memory Record before calling handlers.

  This function provides method to fill memory error records before handlers' code so that OemReadyToTakeAction()
  can use it.
  @param[in] NodeId             Current Node ID
  @param[in] MemoryErrorRecord  Memory Error Record
  @param[in] ErrorEvent         ErrorEvent (SDDC, ADDDC, Rank Sparing)
  
  @retval Status.

**/
VOID FillMemRecordBeforeHandling (
	IN  UINT8 NodeId,
	OUT UEFI_MEM_ERROR_RECORD  *MemoryErrorRecord,
	IN  UINT8  ErrorEvent
    )
{
    EFI_STATUS    Status;
    UINT8         Socket=0, Ch=0, Rank=0, Dimm=0, RankRec =0;

    DEBUG((DEBUG_INFO,"In FillMemRecordBeforeHandling()\n"));

    Socket = NODE_TO_SKT(NodeId); // use Socket & Mc temporarily in XXXXMcCpuCsr() calls

    Status = gSmst->SmmLocateProtocol (&gEfiPlatformRasPolicyProtocolGuid, NULL, &gRasPolicy);
    ASSERT_EFI_ERROR (Status);

    for( Ch = 0; Ch < MAX_MC_CH; Ch++ ){
	    //
	    // Check if there is any rank in this channel
	    // that has overflow bit set
	    //
	    for( Rank = 0; Rank < MAX_RANK_CH ; Rank++ ){
	      if( IsErrorExcdForThisRankBeforeHandling( NodeId, Ch, Rank) ){
		      // Fill Memory Error Record
		      // FIll Header
		      MemoryErrorRecord->Header.SignatureStart = SIGNATURE_32('C','P','E','R');
		      MemoryErrorRecord->Header.Revision = 0x0204; // UEFI Spec version 2.4
		      MemoryErrorRecord->Header.SignatureEnd = 0xffffffff;
		      MemoryErrorRecord->Header.SectionCount = 1; // Only supporting one section per error record
		      MemoryErrorRecord->Header.Severity = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;
		      MemoryErrorRecord->Header.ValidBits = 0;
		      MemoryErrorRecord->Header.RecordLength = sizeof (UEFI_MEM_ERROR_RECORD);
		      CopyMem (&MemoryErrorRecord->Header.NotificationType, &gErrRecordNotifyCmcGuid, sizeof (EFI_GUID));
		      MemoryErrorRecord->Header.Flags = HW_ERROR_FLAGS_RECOVERED;

		      // Fill error descriptor
		      MemoryErrorRecord->Descriptor.Offset = sizeof (UEFI_MEM_ERROR_RECORD) + sizeof (UEFI_ERROR_SECT_DESC);
		      MemoryErrorRecord->Descriptor.Length = sizeof (MEMORY_ERROR_SECTION);
		      MemoryErrorRecord->Descriptor.Revision = 0x0204; // UEFI Spec version 2.4
		      MemoryErrorRecord->Descriptor.ValidBits = 0;
		      MemoryErrorRecord->Descriptor.Flags = 1; // Primary
		      CopyMem (&MemoryErrorRecord->Descriptor.SectionType, &gErrRecordPlatformMemErrGuid, sizeof (EFI_GUID));
		      MemoryErrorRecord->Descriptor.Severity = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;

		      // Fill Node , Rank details
		      //
		      // Convert logical rank to DIMM/Physical rank
		      //
		      Dimm = Rank / MAX_RANK_DIMM;
		      RankRec = Rank % MAX_RANK_DIMM;
		      DEBUG((DEBUG_INFO,"FillMemRecordBeforeHandling() -Error Event :%d NodeId:%d.Channel%d. Dimm %d Rank%d exceeded threshold!\n",ErrorEvent, NodeId, Ch, Dimm, RankRec));
		      MemoryErrorRecord->Section.Node = NodeId;
		      MemoryErrorRecord->Section.Card = Ch;
		      MemoryErrorRecord->Section.RankNumber = RankRec;
		      MemoryErrorRecord->Section.Module = Dimm;
		      MemoryErrorRecord->Section.ErrorEvent = ErrorEvent;
		      
		      MemoryErrorRecord->Section.CardHandle = gRasPolicy->EfiRasSystemTopology->SystemInfo.SmbiosType16Handle; // Type 16 handle
		      MemoryErrorRecord->Section.ModuleHandle = gRasPolicy->EfiRasSystemTopology->SystemInfo.SocketInfo[Socket].ChannelInfo[NODECH_TO_SKTCH(NodeId, Ch)].DimmInfo[Dimm].SmbiosType17Handle; // Type 17 handle
		      MemoryErrorRecord->Section.ValidBits |= PLATFORM_MEM_MODULE_VALID | PLATFORM_RANK_NUMBER_VALID | PLATFORM_CARD_HANDLE_VALID | PLATFORM_Module_HANDLE_VALID;
	      }
	    }
	}
}

/**

   IsErrorExcdForThisRankBeforeHandling - Checks if the specified rank has error overflow

  @param NodeId    - Memory controller ID
  @param NodeCh    - Channel ID
  @param Rank      - Rank ID

  @retval TRUE      - if the specified rank has error overflow
  @retval FALSE     - specified rank has no error overflow

**/
BOOLEAN
IsErrorExcdForThisRankBeforeHandling (
    IN UINT8 Node,
    IN UINT8 NodeCh,
    IN UINT8 Rank
 ) {
	EFI_STATUS						Status;
	BOOLEAN                         cpuErrOverflow = FALSE;
	CORRERRORSTATUS_MCDDC_DP_STRUCT corrErrStsReg;
	EFI_CPU_CSR_ACCESS_PROTOCOL       *CpuCsrAccess;
	UINT8                           Socket = NODE_TO_SKT(Node);
	UINT8                           SktCh = NODECH_TO_SKTCH(Node, NodeCh);
	DEBUG((DEBUG_INFO,"IsErrorExcdForThisRankBeforeHandling\n"));

	Status = gSmst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &CpuCsrAccess);
	ASSERT_EFI_ERROR (Status);
	
	//
	// Check error status in CPU for this rank
	//
	if (!EFI_ERROR(Status)) {
	    corrErrStsReg.Data = CpuCsrAccess->ReadCpuCsr( Socket, SktCh, CORRERRORSTATUS_MCDDC_DP_REG );
	}

	if( corrErrStsReg.Bits.err_overflow_stat & (1 << Rank)){
	    cpuErrOverflow = TRUE;
	}

	return cpuErrOverflow;
}

/**

   GetErrorValidMsrCount - Checks if MSR is valid

   @retval NONE

**/
UINT8 
GetErrorValidMsrCount (
	) {
	UINT32  Ia32McgCap = (UINT32) AsmReadMsr64(EFI_IA32_MCG_CAP);
	UINT8   MsrCount = (UINT8) Ia32McgCap, ValidMsrCount = 0;
        UINT8   Count = 0;	
	for( Count = 0; Count < MsrCount; ++Count ) {
	     if ( ( AsmReadMsr64 (EFI_IA32_MC0_STATUS + (4 * Count)) & 0x8000000000000000 ) != 0 ) {
		    ValidMsrCount++;
	     }
	}
	return ValidMsrCount;
}

/**

   FillOtherMcBankInfo - Fill McBank Data

  @param Skt                - Socket
  @param OtherMcBankInfo    - Buffer to fill McBank Data
  @param OtherMcBankCount   - Count of error valid banks

  @retval VOID

**/
VOID
FillOtherMcBankInfo (
	IN   UINT8         Skt,	
	OUT  MCA_BANK_INFO *OtherMcBankInfo,
	OUT  UINT8         *OtherMcBankCount
) {
	UINT8   McBank = 0;
	UINT32  Ia32McgCap = (UINT32) AsmReadMsr64(EFI_IA32_MCG_CAP);
	UINT8   MsrCount = (UINT8) Ia32McgCap;
	EMCA_MC_SIGNATURE_ERR_RECORD McSigErrRecord;
	MCA_BANK_INFO  *OtherMcBankInfoRecord = NULL;
	*OtherMcBankCount = 0;
	OtherMcBankInfoRecord = OtherMcBankInfo;
	for( McBank = 0; McBank < MsrCount; ++McBank ) {
	     if ( ( AsmReadMsr64 (EFI_IA32_MC0_STATUS + (4 * McBank)) & 0x8000000000000000 ) != 0 ) {
		    // MSR valid bit is TRUE. Store MSR data and increment MSR count
     		ReadMcBankSigHook(McBank, Skt, &McSigErrRecord);
     		OtherMcBankInfoRecord->ApicId = McSigErrRecord.ApicId;
     		OtherMcBankInfoRecord->BankNum = McBank;
     		OtherMcBankInfoRecord->BankType = GetMcBankUnitType(McBank);
     		OtherMcBankInfoRecord->BankScope = GetMcBankScope (McBank);
     		OtherMcBankInfoRecord->McaStatus = McSigErrRecord.Signature.McSts;
     		OtherMcBankInfoRecord->McaAddress = McSigErrRecord.Signature.McAddr;
     		OtherMcBankInfoRecord->McaMisc = McSigErrRecord.Signature.McMisc;
     		OtherMcBankInfoRecord->Valid = TRUE;
		    *OtherMcBankCount += 1;
		    OtherMcBankInfoRecord++;
	     }
	}
}

/**

   EnableMode1 - Sets the MSR for Enabling Mode1 Memory Error Reporting

   @retval NONE

**/
VOID
EnableMode1 (
  VOID *Buffer
  )
{
  UINT64 Data;

  Data = AsmReadMsr64(MSR_ERROR_CONTROL);
  Data |= (BIT1);
  AsmWriteMsr64 (MSR_ERROR_CONTROL, Data);
  return;
}

/**

   EnableMode1MemErrorReporting

   @retval NONE

**/
VOID
EnableMode1MemErrorReporting()
{
  UINTN       Cpu;
  
  for(Cpu=0; Cpu < gSmst->NumberOfCpus; Cpu++) {
    if( Cpu == gSmst->CurrentlyExecutingCpu ) {
      EnableMode1(NULL);
    } else {
      gSmst->SmmStartupThisAp(EnableMode1,
                              Cpu,
                              NULL);
    }
  }
}

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//