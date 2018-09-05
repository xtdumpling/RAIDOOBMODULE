//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  TcgDxe.c
//
// Description:
//  Abstracted functions for Tcg protocol are defined here
//
//<AMI_FHDR_END>
//*************************************************************************
#include <AmiTcg/TcgCommon20.h>
#include <AmiTcg/sha.h>
#include <AmiTcg/TCGMisc.h>
#include <AmiTcg/Tpm20.h>
#include <AmiTcg/TrEEProtocol.h>
#include <Token.h>
#include <Protocol/TcgTcmService.h>
#include <Protocol/TcgPlatformSetupPolicy.h>
#include <Protocol/AcpiSupport.h>
#include "AmiTcg/TcgPc.h"
#include "Protocol/TcgService.h"
#include "Protocol/TpmDevice.h"
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#if defined (RomLayout_SUPPORT) && (RomLayout_SUPPORT!=0)
#include <Library/AmiRomLayoutLib.h>
#endif
#include <Library/TimerLib.h>
#include <Library/LocalApicLib.h>
#include <IndustryStandard/Acpi30.h>
#include <Protocol/Runtime.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Guid/MemoryOverwriteControl.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <AmiTcg/AmiTpmStatusCodes.h>
#include <Guid/AmiTcgGuidIncludes.h>

#include <AmiProtocol.h>

#if PI_SPECIFICATION_VERSION<0x00010000
#include <Protocol/FirmwareVolume.h>
#else
#include <Protocol/FirmwareVolume2.h>
#endif

extern EFI_GUID AmiProtocolInternalHlxeGuid;


#define SHA1_DIGEST_SIZE        20
#define SHA256_DIGEST_SIZE      32
#define SM3_256_DIGEST_SIZE     32
#define SHA384_DIGEST_SIZE      48
#define SHA512_DIGEST_SIZE      64

#if defined (MDE_PKG_VERSION) && (MDE_PKG_VERSION > 9)
#ifndef TPM2_ALG_SHA1
#define TPM2_ALG_SHA1 TPM_ALG_SHA1
#endif
#ifndef TPM2_ALG_SHA256
#define TPM2_ALG_SHA256 TPM_ALG_SHA256
#endif
#ifndef TPM2_ALG_SHA384
#define TPM2_ALG_SHA384 TPM_ALG_SHA384
#endif
#ifndef TPM2_ALG_SHA512
#define TPM2_ALG_SHA512 TPM_ALG_SHA512
#endif
#ifndef TPM2_ALG_SM3_256
#define TPM2_ALG_SM3_256 TPM_ALG_SM3_256
#endif
#ifndef TPM2_ALG_ID
#define TPM2_ALG_ID TPM_ALG_ID
#endif
#endif

UINT8  GetHashPolicy();

#pragma pack (1)
typedef struct
{
    EFI_PHYSICAL_ADDRESS PostCodeAddress;
#if x64_TCG
    UINT64               PostCodeLength;
#else
    UINTN                PostCodeLength;
#endif
} EFI_TCG_EV_POST_CODE;

EFI_TCG_EV_POST_CODE       PhysicalAddressBases;
UINTN   PcrBanks;
UINTN        count=0;

typedef struct
{
    EFI_TCG_PCR_EVENT_HEADER Header;
    EFI_TCG_EV_POST_CODE     Event;
} PEI_EFI_POST_CODE;


typedef struct
{
    BYTE  sha1[SHA1_DIGEST_SIZE];
    BYTE  sha256[SHA256_DIGEST_SIZE];
    BYTE  sm3_256[SM3_256_DIGEST_SIZE];
    BYTE  sha384[SHA384_DIGEST_SIZE];
    BYTE  sha512[SHA512_DIGEST_SIZE];
} TPM2_HALG;


typedef struct
{
    TCG_PCRINDEX         PCRIndex;
    TCG_EVENTTYPE        EventType;
    TPML_DIGEST_VALUES   Digests;
    UINT32               EventSize; // UINT32 aligned
} TCG_PCR_EVENT2_HDR;

typedef struct _AMI_INTERNAL_HLXE_PROTOCOL  AMI_INTERNAL_HLXE_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI * INTERNAL_HASH_LOG_EXTEND_EVENT) (
    IN  EFI_TREE_PROTOCOL        *This,
    IN  UINT8                    *DataToHash,
    IN  UINT64                   Flags,
    IN  UINTN                    DataSize,
    IN  OUT  TCG_PCR_EVENT2_HDR  *NewEventHdr,
    IN  UINT8                    *NewEventData
);

struct _AMI_INTERNAL_HLXE_PROTOCOL
{
    INTERNAL_HASH_LOG_EXTEND_EVENT  AmiHashLogExtend2;
};

#pragma pack()

EFI_EVENT                  Event;
static VOID                *reg;
EFI_EVENT                  Event2;
static VOID                *reg2;
UINT8                      DigestAlgo=0;
UINT8                      Sha1Digest[SHA1_DIGEST_SIZE];
TPM2_HALG                  TpmDigest;

void printbuffer(UINT8 *Buffer, UINTN BufferSize)
{
    UINTN i=0;
    UINTN j=0;

    DEBUG ((DEBUG_INFO, "Buffer  = %x  \n", Buffer));
    DEBUG ((DEBUG_INFO, "BufferSize  = %x  \n", BufferSize));

    for(i=0; i<BufferSize; i++)
    {



        if(i%16 == 0)
        {
            DEBUG((DEBUG_INFO,"\n"));
            DEBUG((DEBUG_INFO,"%04x :", j));

            j+=1;

        }
        DEBUG((DEBUG_INFO,"%02x ", Buffer[i]));

    }
    DEBUG((DEBUG_INFO,"\n"));

}


VOID CallbackOnTcgprotocolInstall(IN EFI_EVENT event,
                                  IN VOID      *ctx)
{
    EFI_STATUS                        Status;
    EFI_TCG_PROTOCOL                  *TcgProtocol;
    PEI_EFI_POST_CODE                 ev;
    UINT32                             EventNumber;
    EFI_GUID                          ExtendedDataGuid = TCG_FWEXTENDED_DATA_GUID;


    DEBUG ((DEBUG_INFO, "CallbackOnTcgprotocolInstall Entry\n"));

    Status = gBS->LocateProtocol(&gEfiTcgProtocolGuid,
                                 NULL,
                                 &TcgProtocol);

    if(EFI_ERROR(Status))return;

    ev.Header.PCRIndex      = PCRi_CRTM_AND_POST_BIOS;
    ev.Header.EventType     = EV_POST_CODE;
    ev.Header.EventDataSize = sizeof (EFI_TCG_EV_POST_CODE);
    ev.Event.PostCodeAddress = \
                               (EFI_PHYSICAL_ADDRESS)PhysicalAddressBases.PostCodeAddress;

    gBS->CopyMem(&ev.Header.Digest, Sha1Digest, SHA1_DIGEST_SIZE);
    Status = TcgProtocol->LogEvent(TcgProtocol, (TCG_PCR_EVENT *)&ev, &EventNumber,0x00);
    if(EFI_ERROR(Status))return;
    
    TpmDxeReportStatusCodeEx(EFI_PROGRESS_CODE, AMI_SPECIFIC_BIOS_FWVOL_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER,
                             0, NULL, &ExtendedDataGuid, &ev.Event, sizeof (EFI_TCG_EV_POST_CODE));
    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_BIOS_FWVOL_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER);

}




VOID CallbackOnTreeInstall(IN EFI_EVENT ev,
                           IN VOID      *ctx)
{
    EFI_STATUS                        Status;
    EFI_TREE_PROTOCOL                 *pTreeProtocol  =  NULL;
    AMI_INTERNAL_HLXE_PROTOCOL        *InternalHLXE = NULL;
    TCG_PCR_EVENT2_HDR                Tcg20Event;
    EFI_TCG_EV_POST_CODE              EventData;
    UINTN                             Count = 0;

    DEBUG ((DEBUG_INFO, "CallbackOnTreeInstall Entry\n"));

    Status = gBS->LocateProtocol(&gEfiTrEEProtocolGuid,
                                 NULL,
                                 &pTreeProtocol);

    Status = gBS->LocateProtocol(&AmiProtocolInternalHlxeGuid, NULL, &InternalHLXE);
    if(EFI_ERROR(Status))return;

    EventData.PostCodeAddress = PhysicalAddressBases.PostCodeAddress;
    EventData.PostCodeLength  = PhysicalAddressBases.PostCodeLength;

    Tcg20Event.EventSize = sizeof(EventData);
    Tcg20Event.EventType = EV_POST_CODE;
    Tcg20Event.PCRIndex  = 0;

    if(DigestAlgo == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        gBS->CopyMem(Tcg20Event.Digests.digests[0].digest.sha1, Sha1Digest, SHA1_DIGEST_SIZE);
        Tcg20Event.Digests.digests[0].hashAlg = TPM2_ALG_SHA1;
        //printbuffer(Sha1Digest,SHA1_DIGEST_SIZE );

    }
    else if(DigestAlgo == TCG2_PROTOCOL_SPEC_TCG_2)
    {

        if( PcrBanks & TREE_BOOT_HASH_ALG_SHA1)
        {
            gBS->CopyMem(&Tcg20Event.Digests.digests[Count].digest.sha1, &TpmDigest.sha1, SHA1_DIGEST_SIZE);
            Tcg20Event.Digests.digests[Count].hashAlg = TPM2_ALG_SHA1;
            Count+=1;
        }
        if( PcrBanks & TREE_BOOT_HASH_ALG_SHA256)
        {
            gBS->CopyMem(&Tcg20Event.Digests.digests[Count].digest.sha256, &TpmDigest.sha256, SHA256_DIGEST_SIZE);
            Tcg20Event.Digests.digests[Count].hashAlg = TPM2_ALG_SHA256;
            Count+=1;
        }
        if( PcrBanks & TREE_BOOT_HASH_ALG_SHA384)
        {
            gBS->CopyMem(&Tcg20Event.Digests.digests[Count].digest.sha384,  &TpmDigest.sha384, SHA384_DIGEST_SIZE);
            Tcg20Event.Digests.digests[Count].hashAlg = TPM2_ALG_SHA384;
            Count+=1;
        }
        if( PcrBanks & TREE_BOOT_HASH_ALG_SHA512)
        {
            gBS->CopyMem(&Tcg20Event.Digests.digests[Count].digest.sha512,  &TpmDigest.sha512, SHA512_DIGEST_SIZE);
            Tcg20Event.Digests.digests[Count].hashAlg = TPM2_ALG_SHA512;
            Count+=1;
        }

        if( PcrBanks & TREE_BOOT_HASH_ALG_SM3)
        {
            gBS->CopyMem(&Tcg20Event.Digests.digests[Count].digest.sm3_256,  &TpmDigest.sm3_256, SM3_256_DIGEST_SIZE);
            Tcg20Event.Digests.digests[Count].hashAlg = TPM2_ALG_SM3_256;
            Count+=1;
        }

        Tcg20Event.Digests.count = (UINT32)Count;
    }

    DEBUG ((DEBUG_INFO, "AmiHashLogExtend2 FwVol\n"));
    //printbuffer((UINT8 *)&Tcg20Event,sizeof(TCG_PCR_EVENT2_HDR));
    InternalHLXE->AmiHashLogExtend2(pTreeProtocol, NULL, 0,0, &Tcg20Event, (UINT8 *)&EventData);
    
    TpmDxeReportStatusCodeEx(EFI_PROGRESS_CODE, AMI_SPECIFIC_BIOS_FWVOL_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER,
                             0, NULL, &ExtendedDataGuid, &EventData, sizeof(EFI_TCG_EV_POST_CODE)); 
    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_BIOS_FWVOL_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER);
}


//#define TREE_BOOT_HASH_ALG_SHA1   0x00000001
//#define TREE_BOOT_HASH_ALG_SHA256 0x00000002
//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcgHookCoreinit
//
// Description: Hashes FVMain Volume
//
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
void TcgHookCoreinit(IN EFI_HANDLE        ImageHandle,
                     IN EFI_SYSTEM_TABLE  *SystemTable)
{
    EFI_STATUS Status;
    SHA1_CTX     Sha1Ctx;
    SHA2_CTX     Sha2Ctx;
    SHA384_CTX   Sha384Ctx;
    SHA512_CTX   Sha512Ctx;
    UINT8        i=0;
    FwVolHob    *TpmFwVolHob;
    UINTN        FwVolHobCount;
    EFI_PEI_HOB_POINTERS          FvHob;
    UINTN        MeasuredCount=0;

    DEBUG ((DEBUG_INFO, "TcgHookCoreinit Entry\n"));

    TpmFwVolHob = LocateATcgHob( gST->NumberOfTableEntries,
                                 gST->ConfigurationTable,
                                 &amiFVhoblistguid);

    if(TpmFwVolHob == NULL)return;

#if defined(SAVE_ENTIRE_FV_IN_MEM) && SAVE_ENTIRE_FV_IN_MEM == 0
    if( TpmFwVolHob->Tcg2SpecVersion != 0 ){
         return;
    }
#endif
      
    DEBUG ((DEBUG_INFO, "SHA1Init done\n"));
    FwVolHobCount = TpmFwVolHob->Count;


    if(FwVolHobCount == 0) return;

    DEBUG ((DEBUG_INFO, "FwVolHobCount = %x \n", FwVolHobCount));

    DigestAlgo = TpmFwVolHob->Tcg2SpecVersion;
    PcrBanks = TpmFwVolHob->PcrBanks;
    // The Tcg2SpecVersion == 0 , => It is a TPM 1.2 device, otherwise, 1: TPM20 TCG2 Old Protocol compatible, 2: TPM 20 TCG2 New Protocol
    if(TpmFwVolHob->Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2 || TpmFwVolHob->Tcg2SpecVersion == 0 )
    {
        SHA1Init( &Sha1Ctx );
    }
    else if(TpmFwVolHob->Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Init( &Sha1Ctx );
            count+=1;
        }
        if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_init(&Sha2Ctx);
            count+=1;
        }
        if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SHA384)
        {
            sha384_init(&Sha384Ctx);
            count+=1;
        }
        if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_init(&Sha512Ctx);
            count+=1;
        }

        if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SM3)
        {
            sha256_init(&Sha2Ctx);
            count+=1;
        }

    }
    
    for(i=0; i< FwVolHobCount; i++)
    {
        DEBUG ((DEBUG_INFO, "TpmFwVolHob[i].Size = %x \n", TpmFwVolHob[i].Size));
        DEBUG ((DEBUG_INFO, "TpmFwVolHob[i].baseAddress = %x \n", TpmFwVolHob[i].baseAddress));


        FvHob.Raw = GetHobList ();
        DEBUG ((DEBUG_INFO, "TpmFwVolHob->Tcg2SpecVersion = %x \n", TpmFwVolHob->Tcg2SpecVersion));
        
        do{
            
            DEBUG ((DEBUG_INFO, "FvHob.FirmwareVolume->BaseAddress = %x  \n", FvHob.FirmwareVolume->BaseAddress));
            
            if(TpmFwVolHob[i].baseAddress == FvHob.FirmwareVolume->BaseAddress ||
               TpmFwVolHob[i].Size == ((UINT32)FvHob.FirmwareVolume->Length) ||
              (TpmFwVolHob[i].Size == ((UINT32)FvHob.FirmwareVolume->Length)) && (TpmFwVolHob[i].Size == FV_MAIN_SIZE)){

                // The Tcg2SpecVersion == 0 , => It is a TPM 1.2 device, otherwise, 1: TPM20 TCG2 Old Protocol compatible, 2: TPM 20 TCG2 New Protocol
                if(TpmFwVolHob->Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2 || 0 == TpmFwVolHob->Tcg2SpecVersion)
                {
                    DEBUG ((DEBUG_INFO, "SHA1Update FwVol hob \n"));
                    DEBUG ((DEBUG_INFO, "(u32)FvHob.FirmwareVolume->Length  = %x  \n", (u32)FvHob.FirmwareVolume->Length ));
                    DEBUG ((DEBUG_INFO, "(u32)FvHob.FirmwareVolume->BaseAddres  = %x  \n", (u32)FvHob.FirmwareVolume->BaseAddress));
                    //printbuffer((UINT8 *)FvHob.FirmwareVolume->BaseAddress,FvHob.FirmwareVolume->Length );
                    SHA1Update( &Sha1Ctx, (unsigned char *)FvHob.FirmwareVolume->BaseAddress, (u32)FvHob.FirmwareVolume->Length );
                    MeasuredCount+=1;
                }
                else if(TpmFwVolHob->Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
                {
                    DEBUG ((DEBUG_INFO, "sha256_process FwVol hob \n"));
                    DEBUG ((DEBUG_INFO, "(u32)FvHob.FirmwareVolume->Length  = %x  \n", (u32)FvHob.FirmwareVolume->Length ));
                    DEBUG ((DEBUG_INFO, "(u32)FvHob.FirmwareVolume->BaseAddres  = %x  \n", (u32)FvHob.FirmwareVolume->BaseAddress));
                    //printbuffer((UINT8 *)FvHob.FirmwareVolume->BaseAddress,FvHob.FirmwareVolume->Length );
                    if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SHA256)
                    {
                        sha256_process( &Sha2Ctx, (unsigned char *)FvHob.FirmwareVolume->BaseAddress, (u32)FvHob.FirmwareVolume->Length );
                    }
                    if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SHA1)
                    {
                        SHA1Update( &Sha1Ctx, (unsigned char *)FvHob.FirmwareVolume->BaseAddress, (u32)FvHob.FirmwareVolume->Length );
                    }
                    if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SHA384)
                    {
                        sha512_process(&Sha384Ctx, (unsigned char *)FvHob.FirmwareVolume->BaseAddress, (u32)FvHob.FirmwareVolume->Length);
                    }
                    if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SHA512)
                    {
                        sha512_process(&Sha512Ctx, (unsigned char *)FvHob.FirmwareVolume->BaseAddress, (u32)FvHob.FirmwareVolume->Length);
                    }
                    if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SM3)
                    {
                        //until sm3 is available in official OpenSSL, Use SM3[SHA-256[FwVol]]
                        sha256_process( &Sha2Ctx, (unsigned char *)FvHob.FirmwareVolume->BaseAddress, (u32)FvHob.FirmwareVolume->Length );
                    }
                    MeasuredCount+=1;
                }
                break;
            }
            FvHob.Raw = GET_NEXT_HOB (FvHob);
            FvHob.Raw = GetNextHob (EFI_HOB_TYPE_FV, FvHob.Raw);
        }while ((FvHob.Raw != NULL )&&(!END_OF_HOB_LIST (FvHob)));
    }

    DEBUG ((DEBUG_INFO, "MeasuredCount = %x \n", MeasuredCount));
    DEBUG ((DEBUG_INFO, "TpmFwVolHob->PcrBanks = %x \n", TpmFwVolHob->PcrBanks));

    if(MeasuredCount != 0)
    {
        // The Tcg2SpecVersion == 0 , => It is a TPM 1.2 device, otherwise, 1: TPM20 TCG2 Old Protocol compatible, 2: TPM 20TCG2 New Protocol
        if(DigestAlgo == TCG2_PROTOCOL_SPEC_TCG_1_2 || 0 == TpmFwVolHob->Tcg2SpecVersion)
        {
            SHA1Final((unsigned char *)&Sha1Digest, &Sha1Ctx);
            //printbuffer(Sha1Digest,SHA1_DIGEST_SIZE );
        }
        else if(DigestAlgo == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SHA1)
            {
                SHA1Final((unsigned char *)&TpmDigest.sha1, &Sha1Ctx);
            }
            if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SHA256)
            {
                sha256_done(&Sha2Ctx, (unsigned char *)&TpmDigest.sha256);
            }
            if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SHA384)
            {
                sha384_done(&Sha384Ctx, (unsigned char *)&TpmDigest.sha384 );
            }
            if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SHA512)
            {
                sha512_done(&Sha512Ctx, (unsigned char *)&TpmDigest.sha512);
            }
            if( TpmFwVolHob->PcrBanks & TREE_BOOT_HASH_ALG_SM3)
            {
                sha256_done(&Sha2Ctx, (unsigned char *)&TpmDigest.sm3_256);
            }
            //printbuffer(Sha2Digest,SHA256_DIGEST_SIZE );
        }

        //hashlogexend digest
        PhysicalAddressBases.PostCodeAddress = TpmFwVolHob[0].baseAddress;
        PhysicalAddressBases.PostCodeLength  = TpmFwVolHob[0].Size;

        Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                                   EFI_TPL_CALLBACK, CallbackOnTreeInstall, &reg, &Event );

        if(EFI_ERROR(Status))
        {
            DEBUG((DEBUG_ERROR, "Unable to create Event..Exit(1)\n"));
            return;
        }

        Status = gBS->RegisterProtocolNotify( &AmiProtocolInternalHlxeGuid, Event, &reg );
        DEBUG ((DEBUG_INFO, "TcgHookCoreinit::RegisterProtocolNotify Status = %r\n", Status));

        Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                                   EFI_TPL_CALLBACK, CallbackOnTcgprotocolInstall, &reg2, &Event2);

        if(EFI_ERROR(Status))
        {
            DEBUG((DEBUG_ERROR, "Unable to create Event..Exit(1)\n"));
            return;
        }

        Status = gBS->RegisterProtocolNotify( &gEfiTcgProtocolGuid, Event2, &reg2);
        DEBUG ((DEBUG_INFO, "TcgHookCoreinit::RegisterProtocolNotify Status = %r\n", Status));
    }

    return;
}
