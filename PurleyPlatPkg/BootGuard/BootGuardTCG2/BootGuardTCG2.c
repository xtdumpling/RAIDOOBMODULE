//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
/** @file BootGuardTCG2.c
    TCG2 Initialization Flow for Boot Guard

**/
//**********************************************************************

#include <Efi.h>
#include <Pei.h>
#include <Library/HobLib.h>
#include <AmiPeiLib.h>
#include <AmiTcg/TcgMisc.h>
#include <Token.h>
#include <AmiCspLibInc.h>
#include "BootGuardTCG2.h"

EFI_GUID gIntEfiPeiTcgLogHobGuid            =   EFI_TCG_LOG_HOB_GUID;         // TCG TPM 1.2 HOB
EFI_GUID gIntEfiTcgTreeLogHobGuid           =   EFI_TCG_TREE_LOG_HOB_GUID;    // TCG TPM 2.0 HOB

EFI_GUID gIntTpm20HobGuid                   =   TPM20_HOB_GUID;
EFI_GUID gIntAmiTreePpiGuid                 =   AMI_TREE_PPI_GUID;


CONST CHAR8 DetailPCRStr[] = "Boot Guard Measured S-CRTM";
CONST CHAR16 AuthoritySCRTMStr[] = L"Boot Guard Measured S-CRTM";

void sha1_vector(UINTN num_elem, const UINT8* addr[], const UINTN* len, UINT8* mac);
void sha256_vector(UINTN num_elem, const UINT8* addr[], const UINTN* len, UINT8* mac);
UINT64  ReadMsr (UINT32 Msr);

/**
  Check if this is MeasuredBoot

  @retval TRUE  This is MeasuredBoot
  @retval FALSE This is NOT MeasuredBoot
**/
BOOLEAN
IsMeasuredBoot (
    VOID
)
{
    if ((AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT) != 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
  Determine if Boot Guard is supported

  @retval TRUE  - Processor is Boot Guard capable.
  @retval FALSE - Processor is not Boot Guard capable.

**/
BOOLEAN
IsBootGuardSupported (
  VOID
  )
{
  UINT64          BootGuardBootStatus;
  UINT32          BootGuardAcmStatus;
  UINT64          BootGuardCapability;

  BootGuardBootStatus = *(UINT64 *)(UINTN)(TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_BOOTSTATUS);
  BootGuardAcmStatus  = *(UINT32 *)(UINTN)(TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_STATUS);
  BootGuardCapability = AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_CAPABILITY;

    if (BootGuardCapability != 0)
    {
    DEBUG ((DEBUG_INFO, "Processor supports Boot Guard.\n"));
    return TRUE;
    }
    else
    {
    DEBUG ((DEBUG_ERROR, "Processor does not support Boot Guard.\n"));
    return FALSE;
  }
}

VOID PrintBuf(
    UINTN   unBufSize,
    UINT8*  _buf
)
{
    UINTN   unIdx;
    for( unIdx = 0; unIdx<unBufSize; ++unIdx )
    {
        if( unIdx % 0x10 == 0 )
            DEBUG(( DEBUG_INFO, "\n" ));

        DEBUG(( DEBUG_INFO, " %02x", _buf[unIdx] ));
    }

    DEBUG(( DEBUG_INFO, "\n" ));
}

ShowStruID( UINT8* Tbl )
{
    UINTN   unIdx;
    
    DEBUG(( DEBUG_INFO, "StructureID:"));
    for( unIdx=0; unIdx<8; ++unIdx)
    {
        DEBUG(( DEBUG_INFO, "%c", Tbl[unIdx]));
    }
    DEBUG(( DEBUG_INFO, "\nStructVersion[%02x]\n", Tbl[8]));
}

EFI_STATUS ACM_TpmStsChk(
    IN EFI_PEI_SERVICES **PeiServices
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT32      AcmStatus = MmioRead32 (MMIO_ACM_STATUS);
    UINT32      AcmErrClass;
    
    AcmErrClass = AcmStatus >> 4;
    AcmErrClass &= 0x3F;
    
    DEBUG((EFI_D_INFO, "[%d]: Check ACM_STATUS[%x]\n", __LINE__, AcmStatus));
    
    switch ( AcmErrClass )
    {
        case 0x06 :
        case 0x0B :
        case 0x0C :
        case 0x15 :
            Status = EFI_NOT_READY;
            break;
    }
    
    return Status;
}

EFI_STATUS ACM_PostSuccess(
)
{
    UINT32  MsrValue;

    MsrValue = (UINT32) ReadMsr (MSR_BOOT_GUARD_SACM_INFO);
    if ( ((MsrValue & B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) == B_BOOT_GUARD_SACM_INFO_NEM_ENABLED)  &&
         ((MsrValue & B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT) == B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT)  &&         
         ((MsrValue & B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS) == B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS) ) {
        DEBUG(( DEBUG_INFO, "[BootGuardTCG2.c] : ACM_PostSuccess : EFI_SUCCESS\n"));
        return EFI_SUCCESS;
    }

    DEBUG(( DEBUG_INFO, "[BootGuardTCG2.c] : "__FUNCTION__" : EFI_UNSUPPORTED\n"));
    return EFI_UNSUPPORTED;
}

UINT8
GetBpRstrLow (
  VOID
  )
{
  BP_RSTR_LOW BpRstr;
    UINT32      BootGuardPolicy;
  UINT64      SacmInfo;

    BootGuardPolicy = MmioRead32 (TXT_PUBLIC_BASE + PURLEY_BOOTGUARD_POLICY_STS);
  SacmInfo  = ReadMsr (MSR_BOOT_GUARD_SACM_INFO);

  BpRstr.Bits.Facb = (UINT8)((SacmInfo  & BIT4)  >> 4);
    BpRstr.Bits.Dcd  = (UINT8)((BootGuardPolicy & BIT5) >> 5);
    BpRstr.Bits.Dbi  = (UINT8)((BootGuardPolicy & BIT6) >> 6);
    BpRstr.Bits.Pbe  = (UINT8)((BootGuardPolicy & BIT7) >> 7);
    BpRstr.Bits.Bbp  = (UINT8)((BootGuardPolicy & BIT8) >> 8);
    BpRstr.Bits.Reserved = 0;
  BpRstr.Bits.BpInvd   = 0;

  return BpRstr.Data;
}

/**
  Get the lower 8 bits of Boot Policy Type

  @return The lower 8 bits of BP.TYPE
**/
UINT8
GetBpTypeLow (
  VOID
  )
{
  BP_TYPE_LOW BpType;
    UINT32      BootGuardPolicy;
  UINT64      SacmInfo;

    BootGuardPolicy = MmioRead32 ( TXT_PUBLIC_BASE + PURLEY_BOOTGUARD_POLICY_STS );
  SacmInfo  = ReadMsr (MSR_BOOT_GUARD_SACM_INFO);

  BpType.Bits.MeasuredBoot = (UINT8)((SacmInfo & BIT5) >> 5);
  BpType.Bits.VerifiedBoot = (UINT8)((SacmInfo & BIT6) >> 6);
    BpType.Bits.Hap      = (UINT8)((BootGuardPolicy & BIT4) >> 4);
  BpType.Bits.Reserved = 0;

  return BpType.Data;
}

//#pragma pack(push, 1)
//typedef struct {
//    UINT32  PCRIndex;
//    UINT32  EventType;
//    UINT8[] Digest;
//    UINT32  EventSize;
//    UINT8[] Event;    
//} EVENT_COMMON_HEADER;
//#pragma pack(pop)

EFI_STATUS CreateBootGuardTPM12Event(
    EFI_PEI_SERVICES **PeiServices,
    UINT32  PcrIndex,
    UINT32  EventType,
    UINT8*  HashData,
    UINTN   HashDataLen,
    UINT8*  EventData,
    UINT32  EventDataLen
)
{
    EFI_STATUS          Status = EFI_SUCCESS;
    UINT8               Digest[64];
    CONST UINT8*        HashDataAddr[1];
    UINTN               HashDataAddrLen[1];
    UINT8               *ptrEvent;
    TCG_LOG_HOB         *TcgLog;
    VOID                *HobStart;
    UINTN               DigestSize = SHA1_DIGEST_SIZE;
    UINTN               Len;
    
    DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d] : Enter "__FUNCTION__"(...)\n", __LINE__));
    
    (*PeiServices)->SetMem( Digest, sizeof (Digest), 0 );

    Status = (*PeiServices)->GetHobList( PeiServices, &HobStart );

    if ( EFI_ERROR( Status ))
    {
        DEBUG(( DEBUG_ERROR, "BootGuardTCG2.c[%d] : Can not locate PEI HOB Logs\n", __LINE__));
        return EFI_NOT_FOUND;
    }

    TcgLog = (TCG_LOG_HOB*)GetNextGuidHob( &gIntEfiPeiTcgLogHobGuid, HobStart );
    if( NULL == TcgLog)
    {
        DEBUG(( DEBUG_ERROR, "BootGuardTCG2.c[%d] : Can not Find TCG HOB Logs\n", __LINE__));
        return EFI_NOT_FOUND;        
    }
    
    (UINTN)TcgLog = (UINTN)TcgLog + sizeof(EFI_HOB_GUID_TYPE);

//    if( TcgLog->EventNum )
//    {
        // The BootGuard CRTM Event, we only log once,
//        return EFI_SUCCESS;
//    }

    HashDataAddr[0] = HashData;
    HashDataAddrLen[0] = HashDataLen;
    
    
    sha1_vector( 1, HashDataAddr, HashDataAddrLen, (UINT8*)&Digest[0] );

    ptrEvent = (UINT8*)( (UINTN)(TcgLog+1) + (UINTN)TcgLog->TableSize );
    
    // PCRIndex
    *(UINT32*)ptrEvent = PcrIndex;
    ptrEvent += sizeof(UINT32);
    // EventSize
    *(UINT32*)ptrEvent = EventType;
    ptrEvent += sizeof(UINT32);
    // Digest
    (*PeiServices)->CopyMem(ptrEvent, &Digest[0], DigestSize);
    ptrEvent += DigestSize;
    // EventSize
    *(UINT32*)ptrEvent = EventDataLen;
    ptrEvent += sizeof(UINT32);
    // EventData
    (*PeiServices)->CopyMem(ptrEvent, EventData, EventDataLen);
    ptrEvent += EventDataLen;
    
    Len = (UINTN)ptrEvent - ((UINTN)(TcgLog+1) + TcgLog->TableSize);
    TcgLog->TableSize += Len;
    ++TcgLog->EventNum;

    return EFI_SUCCESS;
}

EFI_STATUS  CreateBootGuardTPM20Event (
    EFI_PEI_SERVICES **PeiServices,
    UINT32  PcrIndex,
    UINT32  EventType,
    UINT8*  HashData,
    UINTN   HashDataLen,
    UINT8*  EventData,
    UINT32  EventDataLen
)
{
    EFI_STATUS          Status = EFI_SUCCESS;
    UINT8               Sha1Digest[SHA1_DIGEST_SIZE];
    UINT8               Sha256Digest[SHA256_DIGEST_SIZE];
    CONST UINT8*        HashDataAddr[1];
    UINTN               HashDataAddrLen[1];
    UINT8               *ptrEvent;
    TCG_LOG_HOB         *TcgLog;
    VOID                *HobStart;
//    UINTN               DigestSize = SHA1_DIGEST_SIZE;
    UINTN               Len;
    UINT32              u32HashCount = 0;
    
    DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d] : Enter "__FUNCTION__"(...)\n", __LINE__));
    
    
    (*PeiServices)->SetMem( Sha1Digest, sizeof (Sha1Digest), 0 );
    (*PeiServices)->SetMem( Sha256Digest, sizeof (Sha256Digest), 0 );

    Status = (*PeiServices)->GetHobList( PeiServices, &HobStart );

    if ( EFI_ERROR( Status ))
    {
        DEBUG(( DEBUG_ERROR, "BootGuardTCG2.c[%d] : Can not locate PEI HOB Logs\n", __LINE__));
        return EFI_NOT_FOUND;
    }

    TcgLog = (TCG_LOG_HOB*)GetNextGuidHob( &gIntEfiTcgTreeLogHobGuid, HobStart );
    
    if( NULL == TcgLog)
    {
        DEBUG(( DEBUG_ERROR, "BootGuardTCG2.c[%d] : Can not Find TCG HOB Logs\n", __LINE__));
        return EFI_NOT_FOUND;        
    }
    
    (UINTN)TcgLog = (UINTN)TcgLog + sizeof(EFI_HOB_GUID_TYPE);
    DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d] : TcgLog Addr 0x%x\n", __LINE__, TcgLog));

    if( 0 == ( TcgLog->ActPcrBanks & (TREE_BOOT_HASH_ALG_SHA1|TREE_BOOT_HASH_ALG_SHA256) ) )
    {
        DEBUG(( DEBUG_ERROR, "BootGuardTCG2.c[%d] : Err ActPcrBanks, No SHA1/SHA256 banks\n", __LINE__));
        return EFI_INVALID_PARAMETER;
    }

    HashDataAddr[0] = HashData;
    HashDataAddrLen[0] = HashDataLen;
    
    // if ( 0 == TcgLog->Tcg2SpecVersion ) means it is TPM 12 SHA1 Digest Event
    if( TCG2_PROTOCOL_SPEC_VERSION_1_0 == TcgLog->Tcg2SpecVersion || 0 == TcgLog->Tcg2SpecVersion )
    {   
        sha1_vector( 1, HashDataAddr, HashDataAddrLen, (UINT8*)&Sha1Digest[0] );
    }
    else if( TCG2_PROTOCOL_SPEC_VERSION_1_X == TcgLog->Tcg2SpecVersion )
    {
        if( TcgLog->ActPcrBanks & TREE_BOOT_HASH_ALG_SHA1 )
        {
            ++u32HashCount;
            if ( HashDataLen != 0 )
            sha1_vector( 1, HashDataAddr, HashDataAddrLen, (UINT8*)&Sha1Digest[0] );
        }
        if( TcgLog->ActPcrBanks & TREE_BOOT_HASH_ALG_SHA256 )
        {
            ++u32HashCount;
            if ( HashDataLen != 0 )
            sha256_vector( 1, HashDataAddr, HashDataAddrLen, (UINT8*)&Sha256Digest[0] );
        }
    }

    ptrEvent = (UINT8*)(UINTN)TcgLog->LastEntry;
    
    // PCRIndex
    *(UINT32*)ptrEvent = PcrIndex;
    ptrEvent += sizeof(UINT32);
    // EventType
    *(UINT32*)ptrEvent = EventType;
    ptrEvent += sizeof(UINT32);
    // if ( 0 == TcgLog->Tcg2SpecVersion ) means it is TPM 12 SHA1 Digest Event
    if( TCG2_PROTOCOL_SPEC_VERSION_1_0 == TcgLog->Tcg2SpecVersion || 0 == TcgLog->Tcg2SpecVersion )
    {
        // Digest
        (*PeiServices)->CopyMem(ptrEvent, &Sha1Digest[0], SHA1_DIGEST_SIZE);
        ptrEvent += SHA1_DIGEST_SIZE;
    }
    else if( TCG2_PROTOCOL_SPEC_VERSION_1_X == TcgLog->Tcg2SpecVersion )
    {
        *(UINT32*)ptrEvent = u32HashCount;     // Count
        ptrEvent += sizeof(UINT32);
        
        if( TcgLog->ActPcrBanks & TREE_BOOT_HASH_ALG_SHA1 )
        {
            *(UINT16*)ptrEvent = 0x0004;    // AlgorithmId
            ptrEvent += sizeof(UINT16);

            (*PeiServices)->CopyMem(ptrEvent, &Sha1Digest[0], SHA1_DIGEST_SIZE);
            ptrEvent += SHA1_DIGEST_SIZE;
        }

        if( TcgLog->ActPcrBanks & TREE_BOOT_HASH_ALG_SHA256 )
        {
            *(UINT16*)ptrEvent = 0x000B;    // AlgorithmId
            ptrEvent += sizeof(UINT16);

            (*PeiServices)->CopyMem(ptrEvent, &Sha256Digest[0], SHA256_DIGEST_SIZE);
            ptrEvent += SHA256_DIGEST_SIZE;
        }
    }
    // EventSize
    *(UINT32*)ptrEvent = EventDataLen;
    ptrEvent += sizeof(UINT32);
    // EventData
    (*PeiServices)->CopyMem(ptrEvent, EventData, EventDataLen);
    ptrEvent += EventDataLen;
    
    Len = (UINTN)ptrEvent - (UINTN)TcgLog->LastEntry;
    TcgLog->LastEventPtr = TcgLog->LastEntry;
    TcgLog->LastEntry = (UINT64)(UINTN)ptrEvent;
    TcgLog->TableSize += Len;
    ++TcgLog->EventNum;

    return EFI_SUCCESS;
}

EFI_STATUS
Tpm20MeasureCRTM(
    IN CONST EFI_PEI_SERVICES **PeiServices, 
    AMI_TREE_PPI *TrEEPeiPpi)
{
    tdTpm2Event         *TrEEEventData;
    UINT32              EventSize = 0;
    EFI_GUID            CrtmVersion = CRTM_GUID;
    UINT8               *EventDataPtr;
    UINTN               Len;
    UINT8               InBuf[0x100];

    if( NULL == TrEEPeiPpi )
    {
//        ASSERT_EFI_ERROR( Status );
        return EFI_NOT_FOUND;
    }

    EventSize  = sizeof(EFI_GUID);
    
    Len =  sizeof(tdTpm2EventHeader) +  sizeof(UINT32) + EventSize;

    TrEEEventData = (tdTpm2Event*)&InBuf[0];
    
    TrEEEventData->Size = Len;

    TrEEEventData->Header.EventType  = 0x00000008;
    TrEEEventData->Header.HeaderSize = sizeof(tdTpm2EventHeader);
    TrEEEventData->Header.HeaderVersion = 1; 
    TrEEEventData->Header.PCRIndex      = 0;
    
    EventDataPtr = (UINT8 *)TrEEEventData;

    EventDataPtr += sizeof(tdTpm2EventHeader) + sizeof(UINT32);

    (*PeiServices)->CopyMem(
        EventDataPtr,
        &CrtmVersion,
        EventSize
        );
    
    return TrEEPeiPpi->HashLogExtendEvent(TrEEPeiPpi,
            0, (EFI_PHYSICAL_ADDRESS)EventDataPtr, EventSize, TrEEEventData);
}

EFI_STATUS
Tpm12MeasureCRTMVersion(
    IN EFI_PEI_SERVICES **PeiServices )
{
    UINT8                   *pBufTcgEvent;
    UINT8                   InBuf[0x100];
    UINT32                  EventNum;
    EFI_GUID                CrtmVersion   = CRTM_GUID;         
    PEI_TCG_PPI             *TcgPpi       = NULL;
    EFI_STATUS              Status;
    EFI_GUID gTcgPpiguid    =  PEI_TCG_PPI_GUID;
    
    pBufTcgEvent = &InBuf[0];
    
    *(UINT32*)pBufTcgEvent      = 0;                // PCRIndex
    pBufTcgEvent += sizeof(UINT32);
    *(UINT32*)pBufTcgEvent      = 0x00000008;       // EventType
    pBufTcgEvent += sizeof(UINT32);    
    pBufTcgEvent += 20;                             // Digest
    *(UINT32*)pBufTcgEvent      = sizeof(EFI_GUID); // EventDataSize
    pBufTcgEvent += sizeof(UINT32);
    // Event Data
    (*PeiServices)->CopyMem (
            pBufTcgEvent,
            &CrtmVersion,
            sizeof(EFI_GUID)
            );

    Status =  (*PeiServices)->LocatePpi(
                           PeiServices,
                           &gTcgPpiguid,
                           0, NULL,
                           &TcgPpi);

    if(EFI_ERROR(Status)){
        DEBUG(( DEBUG_ERROR, "[%d]: gTcgPpiguid NOT found %r \n", __LINE__, Status)); 
        return Status;
    }

    return TcgPpi->TCGHashLogExtendEvent(
               TcgPpi,
               PeiServices,
               (UINT8*)&CrtmVersion,
               sizeof(EFI_GUID),
               &InBuf[0],
               &EventNum
               );
}

// 0: No TPM
// 1: TPM 1.2
// 2: TPM 2.0
// 3: PTT on HCI
UINTN CheckCpuMsrTpmDevice()
{
    UINT64      u64Msr13A;
    
    u64Msr13A = ReadMsr (MSR_BOOT_GUARD_SACM_INFO);
    
    u64Msr13A >>= 1;
    u64Msr13A &= 3;
    
    return (UINTN)u64Msr13A;
}

// 0: No TPM
// 1: TPM 1.2
// 2: TPM 2.0
// 3: PTT on HCI
UINTN CheckTreeHobTpmDevice(
    CONST EFI_PEI_SERVICES **PeiServices
)
{
    Tpm20DeviceHob      *TrEEDeviceHob;
    VOID                *HobStart;
    EFI_STATUS          Status;

    Status = (*PeiServices)->GetHobList( PeiServices, &HobStart );

    if ( EFI_ERROR( Status ))
    {
        DEBUG(( DEBUG_ERROR, "BootGuardTCG2.c[%d] : Can not locate PEI HOB Logs, It may TPM 1.2 Device\n", __LINE__));
        return 1;
    }

    TrEEDeviceHob = (Tpm20DeviceHob*)GetNextGuidHob( &gIntTpm20HobGuid, HobStart );
    (UINTN)TrEEDeviceHob = (UINTN)TrEEDeviceHob + sizeof(EFI_HOB_GUID_TYPE);
    if( 1 == TrEEDeviceHob->Tpm20DeviceState )
        return 2;

    return 1;
}

EFI_STATUS FindEntryInFit(
    IN UINT8    FitType,
    VOID        **pFitTable
)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    UINT32          FitEntryPointer = 0, FitEntryNumber = 0, i = 0;
    FIT_ENTRY       *FitEntry = NULL;
    BOOLEAN         bFoundTbl = FALSE;

    do
    {
        FitEntryPointer = MmioRead32(IBB_ENTRYPOINT_M);
        FitEntry = (FIT_ENTRY*)(UINTN)FitEntryPointer;
        FitEntryNumber = FitEntry->TblSIZE;

        if ( ( 0xFFFFFFFF == (UINTN)FitEntry ) || ( NULL == FitEntry ) )
        {
            Status = EFI_NOT_FOUND;
            DEBUG(( DEBUG_ERROR, "[BootGuardTCG2.c] : "__FUNCTION__"(%lx) is Invalid\n", FitEntryPointer));
            break;
        }

        FitEntry = (FIT_ENTRY*)FitEntryPointer;

//        DEBUG(( DEBUG_INFO, "[BootGuardTCG2.c] : FitEntryPoint[0x%08x], TableCount[%x]\n", FitEntryPointer, FitEntryNumber ));

        for(i=1; i<FitEntryNumber; i++)
        {
            FitEntry = (FIT_ENTRY*)(FitEntryPointer + i*16);
//            DEBUG(( DEBUG_INFO, "[BootGuardTCG2.c] : FitEntry->TblType = %x\n", FitEntry->TblType));
            if ( FitEntry->TblType == FitType )
            {
                bFoundTbl =1;
                break;
            }
        }

        if( FALSE == bFoundTbl )
        {
            Status = EFI_NOT_FOUND;
            DEBUG(( DEBUG_INFO, "[BootGuardTCG2.c] : Error, Did not find Tbl type = %x\n", FitType));
            break;
        }

        *pFitTable = (VOID*)(UINTN)FitEntry->TblAddress;
    } while( FALSE );

    if( EFI_ERROR(Status) )
    {
        // Do something
    }
    return Status;
}

BOOLEAN NeedLocalityEvent (
  VOID
)
{
// For Purley Platform, we need report the startup event is locality 3
    EFI_STATUS                  Status = EFI_SUCCESS;
    UINT32                      u32IBBElementFlag = 0;
    BpmStruct                   *BpmStructure = NULL;
    
    Status = FindEntryInFit( FIT_BPM_TYPE, &BpmStructure );
    if( EFI_ERROR(Status) )
    {
      DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d]: TRACE ERROR - %r\n", __LINE__, Status ));
      return FALSE;
    }
    
    u32IBBElementFlag = BpmStructure->Ibb_Element.Flags;

    // Check the Startup Locality bit set or not?
    if( 0x02 != (u32IBBElementFlag & 0x02) )
    {
      DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d]: Skip Log Authority Event\n", __LINE__ ));
      return FALSE;
    }

   return TRUE;
}

EFI_STATUS LogLocalityStartupEvent(
    IN EFI_PEI_SERVICES **PeiServices
)
{
    EFI_STATUS                          Status;
    TCG_EFI_STARTUP_LOCALITY_EVENT      LocalityEventData;

    if (NeedLocalityEvent())
    {
        // Make sure the Startup Event must be null terminal. Form TCG PC Client Platform Spec Define.
        (*PeiServices)->SetMem( LocalityEventData.Signature, sizeof(LocalityEventData.Signature), 0 );
        (*PeiServices)->CopyMem (LocalityEventData.Signature, (UINT8 *) "StartupLocality", sizeof ("StartupLocality"));
        LocalityEventData.StartupLocality = 0x03;

        if( 1 == CheckCpuMsrTpmDevice() )
        {
            // For TPM 1.2, did not need the startup event locality event
        }
        else if ( (2 == CheckCpuMsrTpmDevice() ) || ( 3 == CheckCpuMsrTpmDevice() ) )
        {
            Status =  CreateBootGuardTPM20Event(
                                    PeiServices,
                                    0,                      // PcrIndex
                                    0x00000003,             // EventType : EV_NO_ACTION
                                    NULL,                   // HashData
                                    0,                      // HashDataLen
                                    (UINT8*)&LocalityEventData,
                                    sizeof(LocalityEventData)
                                    );

            if( EFI_ERROR(Status) )
            {
               // Log Event Error
            }

            return EFI_SUCCESS;
        }
    }
    return EFI_SUCCESS;
}
EFI_STATUS LogAuthorityPCREvent(
    IN EFI_PEI_SERVICES **PeiServices
)
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    BOOT_POLICY_AUTHORITY       BPAuthorityEntire;
    BOOT_POLICY_AUTHORITY       *BP = NULL;
    KEY_MANIFEST_STRAUCTURE     *KmStructure = NULL;
    BpmStruct                   *BpmStructure = NULL;
    UINT8                       *AcmTable = NULL;
    CONST UINT8*                HashDataAddr[1];
    UINTN                       HashDataAddrLen[1];
    UINT32                      u32IBBElementFlag = 0;

    DEBUG(( DEBUG_INFO,  "[BootGuardTCG2.c] : Enter "__FUNCTION__"(...)\n"));
    BP = &BPAuthorityEntire;

    do
    {
        Status = FindEntryInFit( FIT_BPM_TYPE, &BpmStructure );
        if( EFI_ERROR(Status) )
        {
            DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d]: TRACE ERROR - %r\n", __LINE__, Status ));
            break;
        }
        u32IBBElementFlag = BpmStructure->Ibb_Element.Flags;

        // Check the AuthorityMeasure bit set or not?
        if( 0x04 != (u32IBBElementFlag & 0x04) )
        {
            DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d]: Skip Log Authority Event\n", __LINE__ ));
            Status = EFI_SUCCESS;
            break;
        }

        // 1)
        BP->RSTR = GetBpRstrLow ();
        DEBUG(( DEBUG_INFO, "BP->RSTR:[%x]\n", BP->RSTR ));

        // 2)
        BP->TYPE = GetBpTypeLow ();
        DEBUG(( DEBUG_INFO, "BP->TYPE:[%x]\n", BP->TYPE ));

        // 3) ACM_SVN from 4.6 ACM
        Status = FindEntryInFit( FIT_ACM_TYPE, &AcmTable );
        if( EFI_ERROR(Status) )
        {
            DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d]: TRACE ERROR - %r\n", __LINE__, Status ));
            break;
        }
        BP->ACM_SVN = MmioRead16( (UINTN)AcmTable + 0x1C);
        DEBUG(( DEBUG_INFO, "BP->ACM_SVN = [%x]\n", BP->ACM_SVN ));

        // 4) ACM Key hash from Section 4.5 Boot Guard ACM Public Key Hash
        (*PeiServices)->CopyMem( BP->BootGuardPkHash, (VOID*)M_KEYHASH_ADDR, sizeof(BP->BootGuardPkHash) );

        // 5) Calculate the hash of the key used to verify the key Manifest
        Status = FindEntryInFit( FIT_KM_TYPE, &KmStructure );
        if( EFI_ERROR(Status) )
        {
            DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d]: TRACE ERROR - %r\n", __LINE__, Status ));
            break;
        }
        HashDataAddr[0]     = KmStructure->KeyManifestSignature.Key.Modulus;
        HashDataAddrLen[0]  = sizeof(KmStructure->KeyManifestSignature.Key.Modulus);
        sha256_vector( 1, HashDataAddr, HashDataAddrLen, BP->KMPkHash);

        // 6) BP Key Hash from KM
        (*PeiServices)->CopyMem( BP->BPKeyHash, KmStructure->BPKey.HashBuffer, sizeof(BP->BPKeyHash) );

        DEBUG(( DEBUG_INFO, "BP Hash Structure, Size[%x]", sizeof(BOOT_POLICY_AUTHORITY) ));
        PrintBuf( sizeof(BOOT_POLICY_AUTHORITY), (UINT8*)BP);

        if( 1 == CheckCpuMsrTpmDevice() )
        {
           // For TPM 1.2, did not need the Authority PCR event creation 
        }
        else if ( (2 == CheckCpuMsrTpmDevice() ) || ( 3 == CheckCpuMsrTpmDevice() ) )
        {
            Status =  CreateBootGuardTPM20Event(
                                                    PeiServices,
                                                    7,                      // PcrIndex
                                                    0x80000001,             // EV_EFI_VARIABLE_DRIVER_CONFIG
                                                    (UINT8*)BP,             // HashData
                                                    sizeof(BOOT_POLICY_AUTHORITY),    // HashDataLen
                                                    AuthoritySCRTMStr,
                                                    sizeof(AuthoritySCRTMStr)
                                                    );
            if( EFI_ERROR(Status) )
            {
                // Log Event Error
                break;
            }
        }
    } while( FALSE );

    if( EFI_ERROR(Status) )
    {
        // Do something
    }

    return Status;

}

EFI_STATUS LogDetailPCREvent(
    IN EFI_PEI_SERVICES **PeiServices
)
{
    UINT32                      i = 0;
    EFI_STATUS                  Status = EFI_SUCCESS;
    BOOT_POLICY                 *BP = NULL;
    KEY_MANIFEST_STRAUCTURE     *KmStructure = NULL;
    BpmStruct                   *BpmStructure = NULL;
    UINT8                       *AcmTable = NULL;
    UINTN                       IBBSegmentOffset = 0;

    DEBUG(( DEBUG_INFO,  "[BootGuardTCG2.c] : Enter "__FUNCTION__"(...)\n"));

    // On page 44.
    // The pHashData must be the format .
    // SHA-1 {
    //          1) One byte containing the lower 8 bit of the BP.RSTR
    //          2) One byte contain the lower 8 bits of BP.TYPE
    //          ....
    //          7)  Digest of Hashed IBB Segments(s)

    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof (BOOT_POLICY),&BP);
    if ( EFI_ERROR( Status ) ) {
        return EFI_OUT_OF_RESOURCES;
    }

    do
    {
        DEBUG(( DEBUG_INFO, "MSR[0x%x]:[%08x]\n", MSR_BOOT_GUARD_SACM_INFO, ReadMsr (MSR_BOOT_GUARD_SACM_INFO) ));
        DEBUG(( DEBUG_INFO, "ACM_STATUS:[%08x]\n", ACM_STATUS ));

        BP->RSTR = GetBpRstrLow ();
        DEBUG(( DEBUG_INFO, "BP->RSTR:[%x]\n", BP->RSTR ));

        BP->TYPE = GetBpTypeLow ();
        DEBUG(( DEBUG_INFO, "BP->TYPE:[%x]\n", BP->TYPE ));

        Status = FindEntryInFit( FIT_ACM_TYPE, &AcmTable );
        if( EFI_ERROR(Status) )
        {
            DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d]: TRACE ERROR - %r\n", __LINE__, Status ));
            break;
        }
        BP->ACM_SVN = MmioRead16( (UINTN)AcmTable + 0x1C);
        DEBUG(( DEBUG_INFO, "BP->ACM_SVN = [%x]\n", BP->ACM_SVN ));

        (*PeiServices)->CopyMem( BP->ACM_Signature, (VOID*)((UINTN)AcmTable + 0x184), sizeof(BP->ACM_Signature) );
        PrintBuf( sizeof(BP->ACM_Signature), BP->ACM_Signature );

        Status = FindEntryInFit( FIT_KM_TYPE, &KmStructure );
        if( EFI_ERROR(Status) )
        {
            DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d]: TRACE ERROR - %r\n", __LINE__, Status ));
            break;
        }
        DEBUG(( DEBUG_INFO, "\nKmStructure: Addr[%lx]\n", (UINTN)KmStructure));
        ShowStruID( (UINT8*)KmStructure );

        (*PeiServices)->CopyMem( BP->Key_Manifest_Signature, KmStructure->KeyManifestSignature.Signature.Signature, sizeof(BP->Key_Manifest_Signature) );
        PrintBuf( sizeof(BP->Key_Manifest_Signature), BP->Key_Manifest_Signature );

        Status = FindEntryInFit( FIT_BPM_TYPE, &BpmStructure );
        if( EFI_ERROR(Status) )
        {
            DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d]: TRACE ERROR - %r\n", __LINE__, Status ));
            break;
        }
        DEBUG(( DEBUG_INFO, "\nBpmStructure: Addr[%lx]\n", (UINTN)BpmStructure));
        ShowStruID( (UINT8*)BpmStructure );
        DEBUG(( DEBUG_INFO, "BpmStructure->Ibb_Element.EntryPoint[%x]\n", (UINTN)BpmStructure->Ibb_Element.EntryPoint ));
        DEBUG(( DEBUG_INFO, "BpmStructure->Ibb_Element.SegmentCount[%x]\n", (UINTN)BpmStructure->Ibb_Element.SegmentCount ));
        IBBSegmentOffset = (UINTN)BpmStructure->Ibb_Element.SegmentCount * sizeof(IBB_SEGMENT_ELEMENT);
        DEBUG(( DEBUG_INFO, "\nBpmStructure->Bpm_Signature_Element.KeySignaturee"));
        (*PeiServices)->CopyMem( BP->Boot_Policy_Manifest_Signature, ((BpmStruct*)((UINTN)BpmStructure + IBBSegmentOffset))->Bpm_Signature_Element.KeySignature.Signature.Signature, sizeof(BP->Boot_Policy_Manifest_Signature) );
        PrintBuf( sizeof(BP->Boot_Policy_Manifest_Signature), BP->Boot_Policy_Manifest_Signature );

        DEBUG(( DEBUG_INFO, "\n\nBpmStructure->Digest_of_Hashed_IBB_Segment:"));
        (*PeiServices)->CopyMem( BP->Digest_of_Hashed_IBB_Segment, BpmStructure->Ibb_Element.Digest.HashBuffer, sizeof(BP->Digest_of_Hashed_IBB_Segment) );
        PrintBuf( sizeof(BP->Digest_of_Hashed_IBB_Segment), BP->Digest_of_Hashed_IBB_Segment );
        
        DEBUG(( DEBUG_INFO, "BP Hash Structure, Size[%x]", sizeof(BOOT_POLICY) ));
        PrintBuf( sizeof(BOOT_POLICY), (UINT8*)BP);

        if( 1 == CheckCpuMsrTpmDevice() )
        {
            Status =  CreateBootGuardTPM12Event(
                                                PeiServices, 
                                                0,                      // PcrIndex
                                                0x00000007,             // EventType
                                                (UINT8*)BP,             // HashData   
                                                sizeof(BOOT_POLICY),    // HashDataLen
                                                DetailPCRStr,
                                                sizeof(DetailPCRStr)
                                                );
            if( EFI_ERROR(Status) )
            {
                // Log Event Error
                break;
            }
        }
        else if ( (2 == CheckCpuMsrTpmDevice() ) || ( 3 == CheckCpuMsrTpmDevice() ) )
        {
            Status =  CreateBootGuardTPM20Event(
                                                    PeiServices,
                                                    0,                      // PcrIndex
                                                    0x00000007,             // EventType
                                                    (UINT8*)BP,             // HashData
                                                    sizeof(BOOT_POLICY),    // HashDataLen
                                                    DetailPCRStr,
                                                    sizeof(DetailPCRStr)
                                                    );
            if( EFI_ERROR(Status) )
            {
                // Log Event Error
                break;
            }
        }
    } while( FALSE );

    if( EFI_ERROR(Status) )
    {
        // Do something
    }
    
    return EFI_SUCCESS;
}

EFI_STATUS
BootGuardTcg2MeasureCRTMVersion (
    CONST EFI_PEI_SERVICES **PeiServices, 
    VOID *_TrEEPeiPpi )
{
    EFI_STATUS Status = EFI_NOT_READY;
    VOID        *TrEEPeiPpi = NULL;
    DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d]: Enter "__FUNCTION__"\n", __LINE__));

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gIntAmiTreePpiGuid,
                 0, NULL,
                 &TrEEPeiPpi);
    if( EFI_ERROR(Status) )
    {
        TrEEPeiPpi = NULL;
    }

    if ( IsBootGuardSupported() == FALSE ) {
        goto TCG_TPM_CRTM_L;
    }
    
    Status = ACM_PostSuccess();
    if( !EFI_ERROR(Status) )
    {
        DEBUG((EFI_D_INFO, "[BootGuardTCG2.c] Start the LogDetailPCREvent\n"));
        if(  IsMeasuredBoot( ) ) {
            Status = LogLocalityStartupEvent( PeiServices );
            ASSERT_EFI_ERROR ( Status );
            Status = LogDetailPCREvent( PeiServices );
            ASSERT_EFI_ERROR ( Status );
            Status = LogAuthorityPCREvent( PeiServices );
            ASSERT_EFI_ERROR ( Status );
        } else {
            goto TCG_TPM_CRTM_L;   	    
        }
    }
    else
    {
        DEBUG(( DEBUG_INFO, "[%d]: The BootGuard ACM did not Enable\n", __LINE__));
        Status = EFI_SUCCESS;
    }
    
    DEBUG(( DEBUG_INFO, "BootGuardTCG2.c[%d]: End of BootGuardTcg2MeasureCRTMVersion\n", __LINE__));
    
TCG_TPM_CRTM_L: ;
    if( 1 == CheckCpuMsrTpmDevice() )
    {
        Tpm12MeasureCRTMVersion( PeiServices );
    }
    else if ( (2 == CheckCpuMsrTpmDevice() ) || ( 3 == CheckCpuMsrTpmDevice() ) )
    {
        Tpm20MeasureCRTM( PeiServices, TrEEPeiPpi );
    }
    else
    {
        if( 2 == CheckTreeHobTpmDevice( PeiServices ) )
        {
            Tpm20MeasureCRTM( PeiServices, TrEEPeiPpi );
        }
        else
        {
            Tpm12MeasureCRTMVersion( PeiServices );
        }
    }
    
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
