//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093 **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/xTcgDxe.c 101   5/09/12 3:52p Fredericko $
//
// $Revision: 101 $
//
// $Date: 5/09/12 3:52p $
//**********************************************************************
//--------------------------------------------------------------------------
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  xTcgDxe.c
//
// Description:
//  Most Tcg DXE initialization and measurements are done here
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Efi.h>
#include <AmiTcg/TcgCommon12.h>
#include <Token.h>
#include <AmiTcg/TCGMisc.h>
#include<AmiTcg/sha1.h>
//#include <AmiDxeLib.h>
#include <IndustryStandard/PeImage.h>
#include <Protocol/DiskIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/TcgService.h>
#include <Protocol/TpmDevice.h>
#include "Protocol/CpuIo.h"
#include "Protocol/FirmwareVolume.h"
#include "Protocol/DevicePath.h"
#include "Protocol/PciIo.h"
#include <Protocol/TcgPlatformSetupPolicy.h>
#include <AmiProtocol.h>
#include <Protocol/AmiTpmSupportTypeProtocol.h>
#include<Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
//#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include<Library/BaseLib.h>
#include<Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <AmiTcg/AmiTpmStatusCodes.h>
#include <Library/UefiLib.h>
#include <Guid/AmiTcgGuidIncludes.h>
#include <Guid/Gpt.h>
#if (defined(TCGMeasureSecureBootVariables) && (TCGMeasureSecureBootVariables != 0))
#include <ImageAuthentication.h>
#endif
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>

//------------------------------------------------------------------------
//Internal Structures
//------------------------------------------------------------------------
typedef struct _TCG_DXE_PRIVATE_DATA
{
    EFI_TCG_PROTOCOL        TcgServiceProtocol;
    EFI_TPM_DEVICE_PROTOCOL *TpmDevice;
} TCG_DXE_PRIVATE_DATA;

EFI_STATUS
EFIAPI TcgDxeCommonExtend(
    IN VOID         *CallbackContext,
    IN TPM_PCRINDEX PCRIndex,
    IN TCG_DIGEST   *Digest,
    OUT TCG_DIGEST  *NewPCRValue );

EFI_STATUS
EFIAPI SHA1_init (
    IN VOID     *CallbackContext,
    IN SHA1_CTX *Sha1Ctx );

EFI_STATUS
EFIAPI SHA1_update (
    IN VOID     *CallbackContext,
    IN SHA1_CTX *Sha1Ctx,
    IN VOID     *Data,
    IN UINTN    DataLen );

EFI_STATUS
EFIAPI SHA1_final (
    IN VOID        *CallbackContext,
    IN SHA1_CTX    *Sha1Ctx,
    OUT TCG_DIGEST **Digest
);


#define TCG_DXE_PRIVATE_DATA_FROM_THIS( This )  \
    _CR( This, TCG_DXE_PRIVATE_DATA, TcgServiceProtocol )
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


static BOOLEAN            BootLaunchDone = FALSE;
static BOOLEAN  		  ReadyToBootSignaled=FALSE;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include <Protocol/AcpiSupport.h>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
EFI_STATUS EFIAPI TcgDxeHashLogExtendEvent (
    IN EFI_TCG_PROTOCOL     *This,
    IN EFI_PHYSICAL_ADDRESS HashData,
    IN UINT64               HashDataLen,
    IN TCG_ALGORITHM_ID     AlgorithmId,
    IN OUT TCG_PCR_EVENT    *TCGLogData,
    IN OUT UINT32           *evNum );

EFI_STATUS
EFIAPI
TcgMeasureGptTable (
    IN      EFI_DEVICE_PATH_PROTOCOL  *DevicePath
);


//
//
// Data Table definition
//
typedef struct _AMI_VALID_CERT_IN_SIG_DB
{
    UINT32          SigOffset;
    UINT32          SigLength;
} AMI_VALID_CERT_IN_SIG_DB;



#if (defined(TCGMeasureSecureBootVariables) && (TCGMeasureSecureBootVariables != 0))
EFI_STATUS
MeasureCertificate(UINTN sizeOfCertificate,
                   UINT8 *pterCertificate)
{
    EFI_STATUS   Status;
    TCG_PCR_EVENT             *TcgEvent = NULL;
    EFI_TCG_PROTOCOL          *tcgSvc=NULL;
    TCG_EFI_VARIABLE_DATA     *VarLog=NULL;
    EFI_PHYSICAL_ADDRESS      Last;
    UINT32                    evNum;
    BOOLEAN                   AlreadyMeasuredCert = FALSE;
    UINTN                     i=0;
    UINTN                     VarNameLength;
    static BOOLEAN            initialized = 0;
    static TPM_DIGEST         digestTrackingArray[6];
    static TPM_DIGEST         zeroDigest;
    UINT8                     *tempDigest = NULL;
    UINT64                    HashedDataLen = 20;

    if(!initialized)
    {
        for(i=0; i<5; i++)
        {
            SetMem(digestTrackingArray[i].digest,20,0);
        }
        SetMem(zeroDigest.digest,20,0);
        initialized = TRUE;
    }

    Status = gBS->LocateProtocol(&gEfiTcgProtocolGuid,
                                 NULL, &tcgSvc );

    if(EFI_ERROR(Status))return Status;

    VarNameLength = StrLen(L"db");
    Status = gBS->AllocatePool( EfiBootServicesData,
                                _TPM_STRUCT_PARTIAL_SIZE( TCG_PCR_EVENT,Event )
                                + (UINT32)(sizeof(*VarLog) + VarNameLength
                                           * sizeof(CHAR16) + sizeOfCertificate - 3),
                                &TcgEvent);

    if(EFI_ERROR(Status))return Status;

    TcgEvent->PCRIndex  = 7;
    TcgEvent->EventType = 0x800000E0;

    TcgEvent->EventSize = (UINT32)( sizeof (*VarLog) + VarNameLength
                                    * sizeof (CHAR16) + sizeOfCertificate) - 3;

    Status = gBS->AllocatePool( EfiBootServicesData, TcgEvent->EventSize, &VarLog );

    if ( EFI_ERROR(Status) )
    {
        if(TcgEvent != NULL)
        {
            gBS->FreePool(TcgEvent);
        }
        return EFI_OUT_OF_RESOURCES;
    }

    VarLog->VariableName       = gEfiImageSecurityDatabaseGuid;
    VarLog->UnicodeNameLength  = VarNameLength;
    VarLog->VariableDataLength = sizeOfCertificate;

    gBS->CopyMem((CHAR16*)(VarLog->UnicodeName),
                 L"db",
                 VarNameLength * sizeof (CHAR16));

    gBS->CopyMem((CHAR16*)(VarLog->UnicodeName) + VarNameLength,
                 pterCertificate,
                 sizeOfCertificate);

    gBS->CopyMem( TcgEvent->Event,
                  VarLog,
                  TcgEvent->EventSize );

    //before extending verify if we have already measured it.
    tcgSvc->HashAll(tcgSvc,
                    (UINT8 *)VarLog,
                    TcgEvent->EventSize,
                    4,
                    &HashedDataLen,
                    &tempDigest);

    for(i=0; i<5; i++)
    {
        //tempDigest
        if(!CompareMem(digestTrackingArray[i].digest, tempDigest, 20)){
            gBS->FreePool(TcgEvent);
            gBS->FreePool(VarLog);
            return EFI_SUCCESS; //already measured
        }

        if(!CompareMem(digestTrackingArray[i].digest, zeroDigest.digest, 20))
            break; //we need to measure
    }

    gBS->CopyMem(digestTrackingArray[i].digest, tempDigest, 20);

    Status = tcgSvc->HashLogExtendEvent(tcgSvc,
                                        (EFI_PHYSICAL_ADDRESS)VarLog,
                                        TcgEvent->EventSize,
                                        4,
                                        TcgEvent,
                                        &evNum,
                                        &Last );
    
    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_SECURE_BOOT_CERTIFICATE_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER);

    if(TcgEvent!=NULL)
    {
        gBS->FreePool(TcgEvent);
    }

    if(VarLog!=NULL)
    {
        gBS->FreePool(VarLog);
    }

    return Status;
}



VOID * TcgGetConfigurationTable(
    IN EFI_SYSTEM_TABLE *SystemTable,
    IN EFI_GUID         *Guid
)
{
    EFI_CONFIGURATION_TABLE *Table = SystemTable->ConfigurationTable;
    UINTN i = SystemTable->NumberOfTableEntries;

    for (; i; --i,++Table)
    {
        if (CompareMem(&Table->VendorGuid,Guid, sizeof(EFI_GUID))==0) return Table->VendorTable;
    }
    return 0;
}



EFI_STATUS FindandMeasureSecureBootCertificate()
{
    EFI_STATUS      Status;
    UINTN           VarSize  = 0;
    UINT8           *SecureDBBuffer = NULL;
    UINT8           *CertificateBuffer = NULL;
    UINTN           SizeofCerificate = 0;
    AMI_VALID_CERT_IN_SIG_DB    *CertInfo;
    UINT8           *CertOffsetPtr = NULL;

    Status   = gRT->GetVariable(L"db",
                                &gEfiImageSecurityDatabaseGuid,
                                NULL,
                                &VarSize,
                                NULL);

    if(EFI_ERROR(Status))
    {
        if ( Status != EFI_BUFFER_TOO_SMALL )
        {
            return EFI_NOT_FOUND;
        }
    }


    Status = gBS->AllocatePool( EfiBootServicesData, VarSize, &SecureDBBuffer );

    if (!EFI_ERROR(Status))
    {
        Status = gRT->GetVariable(L"db",
                                  &gEfiImageSecurityDatabaseGuid,
                                  NULL,
                                  &VarSize,
                                  SecureDBBuffer);

        if ( EFI_ERROR( Status ))
        {
            gBS->FreePool( SecureDBBuffer  );
            return EFI_NOT_FOUND;
        }
    }
    else
    {
        return EFI_OUT_OF_RESOURCES;
    }

    //we need to find the pointer in the EFI system table and work from
    //there
    CertInfo = NULL;
    CertInfo = TcgGetConfigurationTable(gST, &AmiValidBootImageCertTblGuid);
    if(CertInfo == NULL)
    {
        DEBUG(( DEBUG_ERROR,"db variable found SecCertificate Information not found in EFI System Table \n"));
        gBS->FreePool( SecureDBBuffer  );
        return EFI_NOT_FOUND;
    }
    if(CertInfo->SigLength == 0)
    {
        DEBUG(( DEBUG_ERROR,"SecCertificate Information found in EST but Information might be invalid \n"));
        gBS->FreePool( SecureDBBuffer  );
        return EFI_NOT_READY;
    }

    CertOffsetPtr = NULL;
    CertOffsetPtr = (SecureDBBuffer + CertInfo->SigOffset);
    MeasureCertificate((UINTN)CertInfo->SigLength,CertOffsetPtr);

    if(SecureDBBuffer!=NULL)
    {
        gBS->FreePool( SecureDBBuffer  );
    }

    return Status;
}


#endif



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   EfiLibGetSystemConfigurationTable
//
// Description: Get table from configuration table by name
//
// Input:       IN EFI_GUID *TableGuid,
//              IN OUT VOID **Table
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS EfiLibGetSystemConfigurationTable(
    IN EFI_GUID *TableGuid,
    IN OUT VOID **Table )
{
    UINTN Index;

    *Table = NULL;

    for ( Index = 0; Index < gST->NumberOfTableEntries; Index++ )
    {
        if ( !CompareMem( TableGuid, &(gST->ConfigurationTable[Index].VendorGuid),
                      sizeof(EFI_GUID)))
        {
            *Table = gST->ConfigurationTable[Index].VendorTable;
            return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureHandoffTables
//
// Description: Measures, Logs and Extends EFI Handoff Tables. eg: SMBIOS
//
// Input:       VOID
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************

EFI_STATUS MeasureHandoffTables()
{
    EFI_STATUS                     Status = EFI_SUCCESS;
#if (defined(Measure_Smbios_Tables) && (Measure_Smbios_Tables!= 0))    
    EFI_TCG_PROTOCOL               *tcgSvc;
    TCG_PCR_EVENT                  *ev = NULL;
    TCG_EFI_HANDOFF_TABLE_POINTERS HandoffTables;
    SMBIOS_TABLE_ENTRY_POINT       *SmbiosTable;
    EFI_PHYSICAL_ADDRESS           Last;
    UINT32                         evNum;

    Status = gBS->LocateProtocol( &gEfiTcgProtocolGuid,
                                  NULL,
                                  &tcgSvc );
    ASSERT( !EFI_ERROR( Status ));

    Status = EfiLibGetSystemConfigurationTable(
                 &gEfiSmbiosTableGuid,
                 &SmbiosTable
             );

    if ( !EFI_ERROR( Status ))
    {
        Status = gBS->AllocatePool( EfiBootServicesData,
                                    _TPM_STRUCT_PARTIAL_SIZE( TCG_PCR_EVENT, Event )
                                    + sizeof (HandoffTables), &ev );

        ASSERT( !EFI_ERROR( Status ));

        ev->PCRIndex  = 1;
        ev->EventType = EV_EFI_HANDOFF_TABLES;
        ev->EventSize = sizeof (HandoffTables);

        HandoffTables.NumberOfTables            = 1;
        HandoffTables.TableEntry[0].VendorGuid  = gEfiSmbiosTableGuid;
        HandoffTables.TableEntry[0].VendorTable = SmbiosTable;

        gBS->CopyMem( ev->Event,
                      &HandoffTables,
                      sizeof (HandoffTables));

        Status = tcgSvc->HashLogExtendEvent(
                     tcgSvc,
                     (EFI_PHYSICAL_ADDRESS)SmbiosTable->TableAddress,
                     SmbiosTable->TableLength,
                     TCG_ALG_SHA,
                     ev,
                     &evNum,
                     &Last );
    }

    if ( ev != NULL )
    {
        gBS->FreePool( ev );
    }
#endif
    return Status;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureTeImage
//
// Description: Measure a TE Image
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
EFI_STATUS
MeasureTeImage (
    IN  EFI_IMAGE_LOAD_EVENT     *ImageLoad,
    IN  UINT8                    *Digest)
{
    SHA1_CTX                          Sha1Ctx;
    EFI_TE_IMAGE_HEADER               *ptrToTEHdr;
    EFI_IMAGE_SECTION_HEADER          *Section=NULL;
    UINT8                             *HashBase=NULL;
    UINTN                             HashSize;
    UINTN                             SumOfBytesHashed;
    UINTN                             SectionHeaderOffset;
    UINTN                             numOfSectionHeaders;
    UINTN                             Index;

    // 2. Initialize a SHA hash context.
    SHA1Init(&Sha1Ctx);

    DEBUG ((DEBUG_INFO, "Printing TE Image Buffer in Memory Image Location = %x, Image size = %x\n",
            (UINTN)ImageLoad->ImageLocationInMemory, ImageLoad->ImageLengthInMemory));

    //printbuffer((UINT8 *)(UINTN)ImageLoad->ImageLocationInMemory, 1024);

    ptrToTEHdr = (EFI_TE_IMAGE_HEADER *)((UINT8 *)(UINTN)ImageLoad->ImageLocationInMemory);

    //Hash TE Image header and section headers

    HashBase = (UINT8 *)(UINTN)ptrToTEHdr;
    HashSize = sizeof(EFI_TE_IMAGE_HEADER) + ( EFI_IMAGE_SIZEOF_SECTION_HEADER * ptrToTEHdr->NumberOfSections);

    DEBUG ((DEBUG_INFO, "Printing Hashed TE Hdr and Section Header\n"));

    DEBUG ((DEBUG_INFO, "Base = %x Len = %x \n", HashBase, HashSize));

    //printbuffer(HashBase, HashSize);


    SHA1Update(&Sha1Ctx,
               HashBase,
               (u32)HashSize);

    SectionHeaderOffset = sizeof(EFI_TE_IMAGE_HEADER) + (UINTN)ptrToTEHdr;

    numOfSectionHeaders = ptrToTEHdr->NumberOfSections;

    SumOfBytesHashed = HashSize;

    //check for alignment
    //hash Alignment buffer
    HashSize = (((EFI_IMAGE_SECTION_HEADER *)((UINT8 *)SectionHeaderOffset))->PointerToRawData
                - ptrToTEHdr->StrippedSize + sizeof(EFI_TE_IMAGE_HEADER)+ (UINTN)ImageLoad->ImageLocationInMemory - ((UINTN)ImageLoad->ImageLocationInMemory + HashSize));

    if(HashSize !=0)
    {
        HashBase += sizeof(EFI_TE_IMAGE_HEADER) + ( EFI_IMAGE_SIZEOF_SECTION_HEADER * ptrToTEHdr->NumberOfSections);

        DEBUG ((DEBUG_INFO, "Printing Hashed TE Alignment Buffer\n"));

        DEBUG ((DEBUG_INFO, "Base = %x Len = %x \n", HashBase, HashSize));
        //printbuffer(HashBase, HashSize);

        SHA1Update(&Sha1Ctx,
                   HashBase,
                   (u32)HashSize);

        SumOfBytesHashed+=HashSize;
    }

    //
    // TE Images do not have a CertDirector
    //

    //process hash section by section
    //hash in order sections. We do not sort TE sections
    for (Index = 0; Index < numOfSectionHeaders; Index++)
    {

        Section = (EFI_IMAGE_SECTION_HEADER *)((UINT8 *)SectionHeaderOffset);

        if (Section->SizeOfRawData == 0)
        {
            continue;
        }

        //Hash raw data

        HashBase = (UINT8 *)(((UINTN)ImageLoad->ImageLocationInMemory)
                             + (UINTN)Section->PointerToRawData - ptrToTEHdr->StrippedSize + sizeof(EFI_TE_IMAGE_HEADER));
        HashSize = (UINTN) Section->SizeOfRawData;

        DEBUG ((DEBUG_INFO, "Section Base = %x Section Len = %x \n", HashBase, HashSize));

//        printbuffer(HashBase, 1024);

        SHA1Update(&Sha1Ctx,
                   HashBase,
                   (u32)HashSize);

        SumOfBytesHashed += HashSize;
        SectionHeaderOffset += EFI_IMAGE_SIZEOF_SECTION_HEADER;
    }

    DEBUG ((DEBUG_INFO, "SumOfBytesHashed = %x \n", SumOfBytesHashed));

    //verify size
    if ( ImageLoad->ImageLengthInMemory > SumOfBytesHashed)
    {

        DEBUG ((DEBUG_INFO, "Hash rest of Data if true \n"));

        HashBase = (UINT8 *)(UINTN)ImageLoad->ImageLocationInMemory + SumOfBytesHashed;
        HashSize = (UINTN)(ImageLoad->ImageLengthInMemory - SumOfBytesHashed);

        DEBUG ((DEBUG_INFO, "Base = %x Len = %x \n", HashBase, HashSize));
//      printbuffer(HashBase, 106);


        SHA1Update(&Sha1Ctx,
                   HashBase,
                   (u32)HashSize);

    }


    //
    // Finalize the SHA hash.
    //
    SHA1Final(Digest, &Sha1Ctx);

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
MeasurePeImage (
    IN  EFI_PHYSICAL_ADDRESS      ImageAddress,
    IN  UINTN                     ImageSize,
    OUT UINT8                     *Digest1
)
{
    EFI_STATUS                           Status;
    EFI_IMAGE_DOS_HEADER                 *DosHdr;
    UINT32                               PeCoffHeaderOffset;
    EFI_IMAGE_SECTION_HEADER             *Section=NULL;
    UINT8                                *HashBase=NULL;
    UINTN                                HashSize;
    UINTN                                SumOfBytesHashed;
    EFI_IMAGE_SECTION_HEADER             *SectionHeader=NULL;
    UINTN                                Index;
    UINTN                                Pos;
    UINT16                               Magic;
    EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION  Hdr;
    UINT32                               NumberOfRvaAndSizes;
    UINT32                               CertSize;
    SHA1_CTX                              Sha1Ctx;

    Status        = EFI_SUCCESS;
    SectionHeader = NULL;

    //
    // Check PE/COFF image
    //
    DosHdr = (EFI_IMAGE_DOS_HEADER *) (UINTN) ImageAddress;
    PeCoffHeaderOffset = 0;
    if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE)
    {
        PeCoffHeaderOffset = DosHdr->e_lfanew;
    }

    Hdr.Pe32 = (EFI_IMAGE_NT_HEADERS32 *)((UINT8 *) (UINTN) ImageAddress + PeCoffHeaderOffset);
    if (Hdr.Pe32->Signature != EFI_IMAGE_NT_SIGNATURE)
    {
        goto Finish;
    }

    //
    // PE/COFF Image Measurement
    //
    //    NOTE: The following codes/steps are based upon the authenticode image hashing in
    //      PE/COFF Specification 8.0 Appendix A.
    //
    //

    // 1.  Load the image header into memory.

    // 2.  Initialize a SHA hash context.
    SHA1Init(&Sha1Ctx);


    //
    // Measuring PE/COFF Image Header;
    // But CheckSum field and SECURITY data directory (certificate) are excluded
    //
    Magic = Hdr.Pe32->OptionalHeader.Magic;

    //
    // 3.  Calculate the distance from the base of the image header to the image checksum address.
    // 4.  Hash the image header from its base to beginning of the image checksum.
    //
    HashBase = (UINT8 *) (UINTN) ImageAddress;
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
        //
        // Use PE32 offset
        //
        NumberOfRvaAndSizes = Hdr.Pe32->OptionalHeader.NumberOfRvaAndSizes;
        HashSize = (UINTN) ((UINT8 *)(&Hdr.Pe32->OptionalHeader.CheckSum) - HashBase);
    }
    else
    {
        //
        // Use PE32+ offset
        //
        NumberOfRvaAndSizes = Hdr.Pe32Plus->OptionalHeader.NumberOfRvaAndSizes;
        HashSize = (UINTN) ((UINT8 *)(&Hdr.Pe32Plus->OptionalHeader.CheckSum) - HashBase);
    }

    SHA1Update(&Sha1Ctx,
               HashBase,
               (u32)HashSize);

    //
    // 5.  Skip over the image checksum (it occupies a single ULONG).
    //
    if (NumberOfRvaAndSizes <= EFI_IMAGE_DIRECTORY_ENTRY_SECURITY)
    {
        //
        // 6.  Since there is no Cert Directory in optional header, hash everything
        //     from the end of the checksum to the end of image header.
        //
        if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC)
        {
            //
            // Use PE32 offset.
            //
            HashBase = (UINT8 *) &Hdr.Pe32->OptionalHeader.CheckSum + sizeof (UINT32);
            HashSize = Hdr.Pe32->OptionalHeader.SizeOfHeaders - (UINTN) (HashBase - ImageAddress);
        }
        else
        {
            //
            // Use PE32+ offset.
            //
            HashBase = (UINT8 *) &Hdr.Pe32Plus->OptionalHeader.CheckSum + sizeof (UINT32);
            HashSize = Hdr.Pe32Plus->OptionalHeader.SizeOfHeaders - (UINTN) (HashBase - ImageAddress);
        }

        if (HashSize != 0)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }
    }
    else
    {
        //
        // 7.  Hash everything from the end of the checksum to the start of the Cert Directory.
        //
        if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC)
        {
            //
            // Use PE32 offset
            //
            HashBase = (UINT8 *) &Hdr.Pe32->OptionalHeader.CheckSum + sizeof (UINT32);
            HashSize = (UINTN) ((UINT8 *)(&Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - HashBase);
        }
        else
        {
            //
            // Use PE32+ offset
            //
            HashBase = (UINT8 *) &Hdr.Pe32Plus->OptionalHeader.CheckSum + sizeof (UINT32);
            HashSize = (UINTN) ((UINT8 *)(&Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - HashBase);
        }

        if (HashSize != 0)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        //
        // 8.  Skip over the Cert Directory. (It is sizeof(IMAGE_DATA_DIRECTORY) bytes.)
        // 9.  Hash everything from the end of the Cert Directory to the end of image header.
        //
        if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC)
        {
            //
            // Use PE32 offset
            //
            HashBase = (UINT8 *) &Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
            HashSize = Hdr.Pe32->OptionalHeader.SizeOfHeaders - (UINTN) (HashBase - ImageAddress);
        }
        else
        {
            //
            // Use PE32+ offset
            //
            HashBase = (UINT8 *) &Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
            HashSize = Hdr.Pe32Plus->OptionalHeader.SizeOfHeaders - (UINTN) (HashBase - ImageAddress);
        }

        if (HashSize != 0)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }
    }

    //
    // 10. Set the SUM_OF_BYTES_HASHED to the size of the header
    //
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
        //
        // Use PE32 offset
        //
        SumOfBytesHashed = Hdr.Pe32->OptionalHeader.SizeOfHeaders;
    }
    else
    {
        //
        // Use PE32+ offset
        //
        SumOfBytesHashed = Hdr.Pe32Plus->OptionalHeader.SizeOfHeaders;
    }

    //
    // 11. Build a temporary table of pointers to all the IMAGE_SECTION_HEADER
    //     structures in the image. The 'NumberOfSections' field of the image
    //     header indicates how big the table should be. Do not include any
    //     IMAGE_SECTION_HEADERs in the table whose 'SizeOfRawData' field is zero.
    //
    SectionHeader = (EFI_IMAGE_SECTION_HEADER *) AllocateZeroPool (sizeof (EFI_IMAGE_SECTION_HEADER) * Hdr.Pe32->FileHeader.NumberOfSections);
    if (SectionHeader == NULL)
    {
        Status = EFI_OUT_OF_RESOURCES;
        goto Finish;
    }

    //
    // 12.  Using the 'PointerToRawData' in the referenced section headers as
    //      a key, arrange the elements in the table in ascending order. In other
    //      words, sort the section headers according to the disk-file offset of
    //      the section.
    //
    Section = (EFI_IMAGE_SECTION_HEADER *) (
                  (UINT8 *) (UINTN) ImageAddress +
                  PeCoffHeaderOffset +
                  sizeof(UINT32) +
                  sizeof(EFI_IMAGE_FILE_HEADER) +
                  Hdr.Pe32->FileHeader.SizeOfOptionalHeader
              );
    for (Index = 0; Index < Hdr.Pe32->FileHeader.NumberOfSections; Index++)
    {
        Pos = Index;
        while ((Pos > 0) && (Section->PointerToRawData < SectionHeader[Pos - 1].PointerToRawData))
        {
            CopyMem (&SectionHeader[Pos], &SectionHeader[Pos - 1], sizeof(EFI_IMAGE_SECTION_HEADER));
            Pos--;
        }
        CopyMem (&SectionHeader[Pos], Section, sizeof(EFI_IMAGE_SECTION_HEADER));
        Section += 1;
    }

    //
    // 13.  Walk through the sorted table, bring the corresponding section
    //      into memory, and hash the entire section (using the 'SizeOfRawData'
    //      field in the section header to determine the amount of data to hash).
    // 14.  Add the section's 'SizeOfRawData' to SUM_OF_BYTES_HASHED .
    // 15.  Repeat steps 13 and 14 for all the sections in the sorted table.
    //
    for (Index = 0; Index < Hdr.Pe32->FileHeader.NumberOfSections; Index++)
    {
        Section  = (EFI_IMAGE_SECTION_HEADER *) &SectionHeader[Index];
        if (Section->SizeOfRawData == 0)
        {
            continue;
        }
        HashBase = (UINT8 *) (UINTN) ImageAddress + Section->PointerToRawData;
        HashSize = (UINTN) Section->SizeOfRawData;

        SHA1Update(&Sha1Ctx,
                   HashBase,
                   (u32)HashSize);

        SumOfBytesHashed += HashSize;
    }

    //
    // 16.  If the file size is greater than SUM_OF_BYTES_HASHED, there is extra
    //      data in the file that needs to be added to the hash. This data begins
    //      at file offset SUM_OF_BYTES_HASHED and its length is:
    //             FileSize  -  (CertDirectory->Size)
    //
    if (ImageSize > SumOfBytesHashed)
    {
        HashBase = (UINT8 *) (UINTN) ImageAddress + SumOfBytesHashed;

        if (NumberOfRvaAndSizes <= EFI_IMAGE_DIRECTORY_ENTRY_SECURITY)
        {
            CertSize = 0;
        }
        else
        {
            if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC)
            {
                //
                // Use PE32 offset.
                //
                CertSize = Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
            }
            else
            {
                //
                // Use PE32+ offset.
                //
                CertSize = Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
            }
        }

        if (ImageSize > CertSize + SumOfBytesHashed)
        {
            HashSize = (UINTN) (ImageSize - CertSize - SumOfBytesHashed);

            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);

        }
        else if (ImageSize < CertSize + SumOfBytesHashed)
        {
            goto Finish;
        }
    }

    //
    // 17.  Finalize the SHA hash.
    //
    SHA1Final(Digest1, &Sha1Ctx);

Finish:
    if (SectionHeader != NULL)
    {
        FreePool (SectionHeader);
    }

    return Status;
}


//--------------------------------------------------------------------------------------------
//Description:  Measure a PE/COFF image into PCR 2 or 4 depending on Boot policy of 0 or 1
//Arguments:
//  BootPolicy  - Boolean value of 0 or 1 for PCR index 2 or 4.
//  ImageContext - Contains details about the image.
//  LinkTimeBase - Linking time Image Address
//  ImageType    - EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION, BOOT_SERVICE_DRIVER, EFI_RUNTIME_DRIVER
//  DeviceHandle - Device identification handle
//  FilePath     - Device File path
//Output:  EFI_SUCCESS - Image Measured successfully.
//---------------------------------------------------------------------------------------------

EFI_STATUS
EFIAPI
TcgMeasurePeImage (
    IN      BOOLEAN                   BootPolicy,
    IN      EFI_PHYSICAL_ADDRESS      ImageAddress,
    IN      UINTN                     ImageSize,
    IN      UINTN                     LinkTimeBase,
    IN      UINT16                    ImageType,
    IN      EFI_HANDLE                DeviceHandle,
    IN      EFI_DEVICE_PATH_PROTOCOL  *FilePath
)
{
    EFI_STATUS                        Status;
    TCG_PCR_EVENT                     *TcgEvent=NULL;
    TCG_PCR_EVENT                     *TcgEventlog = NULL;
    EFI_IMAGE_LOAD_EVENT              *ImageLoad=NULL;
    UINT32                            FilePathSize;

    EFI_IMAGE_DOS_HEADER              *DosHdr;
    UINT32                            PeCoffHeaderOffset;

    UINT32                            EventSize;
    UINT32                            EventNumber;
    EFI_TCG_PROTOCOL			      *TcgProtocol=NULL;
    EFI_TPM_DEVICE_PROTOCOL	          *TpmProtocol=NULL;
    TCG_DIGEST                        Sha1Digest;

    Status        = EFI_SUCCESS;
    ImageLoad     = NULL;
    FilePathSize  = (UINT32) GetDevicePathSize (FilePath);

    if(IsTcmSupportType())
    {
        return EFI_SUCCESS;
    }

    Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid,  NULL, &TcgProtocol);
    if (EFI_ERROR (Status))
    {
        return Status;
    }

    Status = gBS->LocateProtocol (&gEfiTpmDeviceProtocolGuid,  NULL, &TpmProtocol );
    if (EFI_ERROR (Status))
    {
        return Status;
    }

    TpmProtocol->Init(TpmProtocol);

    EventSize = sizeof (*ImageLoad) - sizeof (ImageLoad->DevicePath) + FilePathSize;
    TcgEvent = AllocateZeroPool (EventSize + sizeof (TCG_PCR_EVENT));
    if (TcgEvent == NULL)
    {
        return EFI_OUT_OF_RESOURCES;
    }

    TcgEvent->EventSize = EventSize;
    ImageLoad           = (EFI_IMAGE_LOAD_EVENT *) TcgEvent->Event;


    switch (ImageType)
    {
        case EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION:
            TcgEvent->EventType = EV_EFI_BOOT_SERVICES_APPLICATION;
            TcgEvent->PCRIndex  = 4;
            break;
        case EFI_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER:
            TcgEvent->EventType = EV_EFI_BOOT_SERVICES_DRIVER;
            TcgEvent->PCRIndex  = 2;
            break;
        case EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER:
            TcgEvent->EventType = EV_EFI_RUNTIME_SERVICES_DRIVER;
            TcgEvent->PCRIndex  = 2;
#if defined(HashSmmDrivers) && (HashSmmDrivers == 0)
            Status = EFI_UNSUPPORTED;
            goto Finish;
#endif
            break;
        default:
            TcgEvent->EventType = ImageType;
            Status = EFI_UNSUPPORTED;
            goto Finish;
    }

    ImageLoad->ImageLocationInMemory = ImageAddress;
    ImageLoad->ImageLengthInMemory   = ImageSize;
    ImageLoad->ImageLinkTimeAddress  = LinkTimeBase;
    ImageLoad->LengthOfDevicePath    = FilePathSize;
    gBS->CopyMem (ImageLoad->DevicePath, FilePath, FilePathSize);

    //
    // Check PE/COFF image
    //
    DosHdr = (EFI_IMAGE_DOS_HEADER *) (UINTN) ImageAddress;
    PeCoffHeaderOffset = 0;
    if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE)
    {
        PeCoffHeaderOffset = DosHdr->e_lfanew;
    }
    if (((EFI_TE_IMAGE_HEADER *)((UINT8 *) (UINTN) ImageAddress + PeCoffHeaderOffset))->Signature
            == EFI_TE_IMAGE_HEADER_SIGNATURE)
    {

        MeasureTeImage(ImageLoad, Sha1Digest.digest);
        gBS->CopyMem(&TcgEvent->Digest.digest, Sha1Digest.digest, SHA1_DIGEST_SIZE);
        goto MeasurePeTeImageDone;
    }
    else
    {
        MeasurePeImage( ImageAddress, ImageSize, Sha1Digest.digest);
        gBS->CopyMem(&TcgEvent->Digest.digest, Sha1Digest.digest, SHA1_DIGEST_SIZE);
        goto MeasurePeTeImageDone;
    }

MeasurePeTeImageDone:
    //
    // HashLogExtendEvent
    //
    if(ReadyToBootSignaled){
        MeasureHandoffTables();
    }

    // We might need to Log the PCR7 BootCert Event First.
#if (defined(TCGMeasureSecureBootVariables) && (TCGMeasureSecureBootVariables != 0))  
    if(TcgEvent->EventType == EV_EFI_BOOT_SERVICES_APPLICATION){
        FindandMeasureSecureBootCertificate();
    }

#if defined (Measure_CA_For_External_EFI_DRIVERS) && (Measure_CA_For_External_EFI_DRIVERS == 1)
    if(TcgEvent->EventType == EV_EFI_BOOT_SERVICES_DRIVER){
        FindandMeasureSecureBootCertificate();
    }
#endif
#endif

    //hash has been generated so extend it
    TpmProtocol->Init(TpmProtocol);

    Status = TcgDxeCommonExtend (
                 (void *)TcgProtocol,
                 TcgEvent->PCRIndex,
                 &TcgEvent->Digest,
                 &Sha1Digest
             );

    TpmProtocol->Close(TpmProtocol);

    if (!EFI_ERROR (Status))
    {

        TcgEventlog  = AllocateZeroPool (EventSize + sizeof (TCG_PCR_EVENT));

//Now log the event
        TcgEventlog->PCRIndex  = TcgEvent->PCRIndex;
        TcgEventlog->EventType = TcgEvent->EventType;
        TcgEventlog->EventSize = TcgEvent->EventSize;
        gBS->CopyMem(&TcgEventlog->Digest, &TcgEvent->Digest, sizeof(TCG_DIGEST));
        gBS->CopyMem(&TcgEventlog->Event, ImageLoad, TcgEvent->EventSize);
        Status = TcgProtocol->LogEvent(TcgProtocol, TcgEventlog, &EventNumber,0x01);
    }
    TpmDxeReportStatusCodeEx(EFI_PROGRESS_CODE, AMI_SPECIFIC_UEFI_IMAGE_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER, 0,
                           NULL, &ExtendedDataGuid, FilePath, FilePathSize);

    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_UEFI_IMAGE_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER);

    TpmProtocol->Close(TpmProtocol);

Finish:
    if (TcgEventlog != NULL )
    {
        gBS->FreePool( TcgEventlog );
    }

    if (TcgEvent!= NULL )
    {
        gBS->FreePool( TcgEvent );
    }

    return Status;
}


EFI_STATUS
EFIAPI
TcgMeasureGptTable (
    IN      EFI_DEVICE_PATH_PROTOCOL  *DevicePath
)
{

    EFI_STATUS                        Status;
    EFI_BLOCK_IO_PROTOCOL             *BlockIo=NULL;
    EFI_DISK_IO_PROTOCOL              *DiskIo=NULL;
    EFI_PARTITION_TABLE_HEADER        *PrimaryHeader=NULL;
    EFI_PARTITION_ENTRY               *PartitionEntry=NULL;
    UINT8                             *EntryPtr=NULL;
    UINTN                             NumberOfPartition;
    UINT32                            Index;
    TCG_PCR_EVENT                     *TcgEvent=NULL;
    EFI_GPT_DATA                  *GptData;
    EFI_HANDLE                        Handle;
    EFI_TCG_PROTOCOL                  *TcgProtocol=NULL;
    EFI_TPM_DEVICE_PROTOCOL           *TpmProtocol=NULL;
    UINT32                            evNum;
    EFI_PHYSICAL_ADDRESS              Last;
    UINTN                             GptIndex;

    if(IsTcmSupportType())
    {
        return EFI_SUCCESS;
    }

    Status = gBS->LocateDevicePath (
                 &gEfiDiskIoProtocolGuid,
                 &DevicePath,
                 &Handle
             );

    if (EFI_ERROR (Status))
    {
        return EFI_UNSUPPORTED;
    }
    Status = gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, &BlockIo);
    if (EFI_ERROR (Status))
    {
        return EFI_UNSUPPORTED;
    }
    Status = gBS->HandleProtocol (Handle, &gEfiDiskIoProtocolGuid, &DiskIo);
    if (EFI_ERROR (Status))
    {
        return EFI_UNSUPPORTED;
    }


    Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid,  NULL, &TcgProtocol);
    if (EFI_ERROR (Status))
    {
        return Status;
    }

    Status = gBS->LocateProtocol (&gEfiTpmDeviceProtocolGuid,  NULL, &TpmProtocol );
    if (EFI_ERROR (Status))
    {
        return Status;
    }

    TpmProtocol ->Init( TpmProtocol );

    //
    // Read the EFI Partition Table Header
    //

    Status = gBS->AllocatePool( EfiBootServicesData,
                                BlockIo->Media->BlockSize,
                                &PrimaryHeader );

    if (EFI_ERROR(Status) ||PrimaryHeader == NULL)
    {
        return EFI_OUT_OF_RESOURCES;
    }

    Status = DiskIo->ReadDisk (
                 DiskIo,
                 BlockIo->Media->MediaId,
                 1 * BlockIo->Media->BlockSize,
                 BlockIo->Media->BlockSize,
                 (UINT8 *)PrimaryHeader
             );

    if (EFI_ERROR (Status))
    {
        DEBUG ((DEBUG_ERROR, "Failed to Read Partition Table Header!\n"));
        gBS->FreePool (PrimaryHeader);
        return EFI_DEVICE_ERROR;
    }
    //
    // Read the partition entry.
    //
    Status = gBS->AllocatePool( EfiBootServicesData,
                                PrimaryHeader->NumberOfPartitionEntries * PrimaryHeader->SizeOfPartitionEntry,
                                &EntryPtr );

    if (EFI_ERROR(Status) ||EntryPtr == NULL)
    {
        gBS->FreePool (PrimaryHeader);
        return EFI_OUT_OF_RESOURCES;
    }

    Status = DiskIo->ReadDisk (
                 DiskIo,
                 BlockIo->Media->MediaId,
                 MultU64x32(PrimaryHeader->PartitionEntryLBA, BlockIo->Media->BlockSize),
                 PrimaryHeader->NumberOfPartitionEntries * PrimaryHeader->SizeOfPartitionEntry,
                 EntryPtr);

    if (EFI_ERROR (Status))
    {
        gBS->FreePool (PrimaryHeader);
        gBS->FreePool (EntryPtr);
        return EFI_DEVICE_ERROR;
    }


    //
    // Count the valid partition
    //
    PartitionEntry    = (EFI_PARTITION_ENTRY *)EntryPtr;
    NumberOfPartition = 0;
    for (Index = 0; Index < PrimaryHeader->NumberOfPartitionEntries; Index++)
    {
        if (CompareMem(&PartitionEntry->PartitionTypeGUID, &gEfiPartTypeUnusedGuid, sizeof(EFI_GUID)))
        {
            NumberOfPartition++;
        }
        PartitionEntry++;
    }
    //
    // Parepare Data for Measurement
    //

    //allocate memory for TCG event
    Status = gBS->AllocatePool( EfiBootServicesData,
                                sizeof(TCG_PCR_EVENT_HDR) + \
                                (UINT32)(sizeof (EFI_PARTITION_TABLE_HEADER) + sizeof(UINTN)\
                                         + (NumberOfPartition * PrimaryHeader->SizeOfPartitionEntry)),
                                &TcgEvent );

    if(EFI_ERROR(Status) || TcgEvent == NULL){
        gBS->FreePool (PrimaryHeader);
        gBS->FreePool (EntryPtr);
        return Status;
    }

    TcgEvent->PCRIndex   = 5;
    TcgEvent->EventType  = EV_EFI_GPT_EVENT;
    TcgEvent->EventSize  =  (UINT32)(sizeof (EFI_PARTITION_TABLE_HEADER) + sizeof(UINTN)\
                                     + (NumberOfPartition * PrimaryHeader->SizeOfPartitionEntry));


    Status = gBS->AllocatePool( EfiBootServicesData,
                                TcgEvent->EventSize,
                                &GptData );
    if (EFI_ERROR(Status) || GptData == NULL)
    {
        gBS->FreePool (TcgEvent);
        gBS->FreePool (PrimaryHeader);
        gBS->FreePool (EntryPtr);
        return EFI_OUT_OF_RESOURCES;
    }

    SetMem(GptData, TcgEvent->EventSize, 0);
    //
    // Copy the EFI_PARTITION_TABLE_HEADER and NumberOfPartition
    //
    gBS->CopyMem ((UINT8 *)GptData, (UINT8*)PrimaryHeader, sizeof (EFI_PARTITION_TABLE_HEADER));
    GptData->NumberOfPartitions = NumberOfPartition;
    //
    // Copy the valid partition entry
    //

    PartitionEntry = (EFI_PARTITION_ENTRY*)EntryPtr;
    GptIndex = 0;
    for (Index = 0; Index < PrimaryHeader->NumberOfPartitionEntries; Index++)
    {
        if (CompareMem (&PartitionEntry->PartitionTypeGUID, &gEfiPartTypeUnusedGuid, sizeof(EFI_GUID)))
        {
            gBS->CopyMem (
                (UINT8 *)&GptData->Partitions + (GptIndex * sizeof (EFI_PARTITION_ENTRY)),
                (UINT8 *)PartitionEntry,
                sizeof (EFI_PARTITION_ENTRY)
            );
            GptIndex+=1;
        }
        PartitionEntry++;
    }
    //
    // Measure the GPT data
    //

    gBS->CopyMem (TcgEvent->Event,
                  GptData,
                  TcgEvent->EventSize);

    TpmProtocol ->Init( TpmProtocol );

    Status = TcgProtocol->HashLogExtendEvent(
                 TcgProtocol,
                 (EFI_PHYSICAL_ADDRESS)GptData,
                 TcgEvent->EventSize,
                 TCG_ALG_SHA,
                 TcgEvent,
                 &evNum,
                 &Last);

    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_UEFI_GPT_PARTITION_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER);
    TpmProtocol ->Close( TpmProtocol );

    gBS->FreePool (PrimaryHeader);
    gBS->FreePool (EntryPtr);
    gBS->FreePool (TcgEvent);
    gBS->FreePool (GptData);

    DEBUG(( DEBUG_INFO,"GPT_EXIT"));
    return Status;
}



EFI_STATUS
TcgMeasureAction(
    IN      CHAR8             *String
)
{

    TCG_PCR_EVENT                     *TcgEvent = NULL;
    EFI_PHYSICAL_ADDRESS              Last;
    EFI_TCG_PROTOCOL                  *tcgSvc = NULL;
    UINT32                            evNum;
    UINT32                            Len;
    EFI_STATUS                        Status;


    Status = gBS->LocateProtocol (
                 &gEfiTcgProtocolGuid,
                 NULL,
                 &tcgSvc);

    ASSERT(!EFI_ERROR(Status));

    Len = (UINT32)AsciiStrLen(String);
    Status = gBS->AllocatePool (EfiBootServicesData,
                                _TPM_STRUCT_PARTIAL_SIZE (TCG_PCR_EVENT, Event) +
                                Len,
                                &TcgEvent);

    ASSERT(!EFI_ERROR(Status));
    if(EFI_ERROR(Status) || TcgEvent == NULL){
        return EFI_OUT_OF_RESOURCES;
    }

    TcgEvent->PCRIndex    = 5;
    TcgEvent->EventType   = EV_EFI_ACTION;
    TcgEvent->EventSize   = Len;

    gBS->CopyMem (TcgEvent->Event,
                  String,
                  Len);

    Status = tcgSvc->HashLogExtendEvent (
                 tcgSvc,
                 (EFI_PHYSICAL_ADDRESS)String,
                 TcgEvent->EventSize,
                 TCG_ALG_SHA,
                 TcgEvent,
                 &evNum,
                 &Last);

    if(TcgEvent!=NULL)
    {
        gBS->FreePool (TcgEvent);
    }

    return Status;
}

UINT8 GetPlatformSupportType()
{
    return (IsTcmSupportType());
}


VOID
EFIAPI
TcgOnReadyToBoot (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
    ReadyToBootSignaled = TRUE;
}


static EFI_TCG_PLATFORM_PROTOCOL  mTcgPlatformProtocol =
{
    TcgMeasurePeImage,
    TcgMeasureAction,
    TcgMeasureGptTable
};


static AMI_TPM_SUPPORT_TYPE_PROTOCOL  mAmiTcgPlatformProtocol =
{
    GetPlatformSupportType
};


EFI_STATUS EFIAPI TcmDxeEntry (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable);

TpmDxeEntry(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE          * SystemTable);



EFI_STATUS
EFIAPI TcgDxeEntry (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable );


//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   CommonTcgDxEntryPoint
//
// Description: Common entry point for Tcgdxe
//
// Input:       IN EFI_HANDLE        ImageHandle
//              IN EFI_SYSTEM_TABLE *SystemTable
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
EFI_STATUS
EFIAPI CommonTcgDxEntryPoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS                      Status;
    TCG_PLATFORM_SETUP_PROTOCOL     *ProtocolInstance;
    BOOLEAN                         TpmInitError = FALSE;
#if TCG_LEGACY == 1
    BOOLEAN                         TpmLegBin = TRUE;
#else
    BOOLEAN                         TpmLegBin = FALSE;
#endif
    TCG_CONFIGURATION               Config;
    EFI_TCG_PROTOCOL                *TcgProtocol;
    void                            ** DummyPtr;
    BOOLEAN                         *ResetAllTcgVar = NULL;
    UINT8                           *TcgDeviceTypeHob = NULL;
    EFI_EVENT           			ReadyToBootEvent;

    Status = gBS->LocateProtocol (&gTcgPlatformSetupPolicyGuid,  NULL, &ProtocolInstance);
    if (EFI_ERROR (Status))
    {
        return Status;
    }

    CopyMem(&Config, &ProtocolInstance->ConfigFlags, sizeof(TCG_CONFIGURATION));

    if(Config.DeviceType == 1)
    {
        return EFI_SUCCESS;
    }


    if((IsTcmSupportType()== TRUE) || (TpmLegBin == TRUE))
    {
        if( Config.TpmSupport != 0x00)
        {
#if TCG_LEGACY == 0
            Status = TcmDxeEntry( ImageHandle, SystemTable );
#else

            Status = TpmDxeEntry( ImageHandle, SystemTable);
#endif
            if(Status)
            {
                TpmInitError = TRUE;
            }
        }
    }
    else
    {
        Status = TpmDxeEntry( ImageHandle, SystemTable );
        if(Status)
        {
            TpmInitError = TRUE;
        }
        else
        {
        }
    }

    ProtocolInstance->UpdateStatusFlags(&Config, FALSE);
    if(TpmInitError)
    {
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MINOR, AMI_SPECIFIC_TPM_ERR_1_2_NOT_DISCOVERED | EFI_SOFTWARE_DXE_BS_DRIVER);
        return Status;
    }

    Status = TcgDxeEntry( ImageHandle, SystemTable );
    if(EFI_ERROR(Status))
    {

        Config.PpiSetupSyncFlag = TRUE;
        ProtocolInstance->UpdateStatusFlags(&Config, FALSE);
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MINOR, AMI_SPECIFIC_TPM_ERR_1_2_NOT_DISCOVERED | EFI_SOFTWARE_DXE_BS_DRIVER);
        return Status;
    }
    else
    {

        Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid,  NULL, &TcgProtocol);
        if (EFI_ERROR (Status))
        {
            ResetAllTcgVar = (UINT8*)LocateATcgHob(
                                 gST->NumberOfTableEntries,
                                 gST->ConfigurationTable,
                                 &AmiTcgResetVarHobGuid);

            DummyPtr = &ResetAllTcgVar;
            if ( *DummyPtr != NULL )
            {
                if ( *ResetAllTcgVar == TRUE )
                {
                    Config.PpiSetupSyncFlag = TRUE;
                }
            }
            ProtocolInstance->UpdateStatusFlags(&Config, FALSE);
            return Status;
        }

        Config.Tpm20Device = 0;
        Config.TpmHardware = 0; // Hardware Present
        TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_DISCOVERED | EFI_SOFTWARE_DXE_BS_DRIVER);
        if(IsTcmSupportType())
        {
            Config.TcgSupportEnabled = 0;
            Config.TcmSupportEnabled = 1;
        }
        else
        {
            Config.TcgSupportEnabled = TRUE;
            Config.TcmSupportEnabled = 0;
        }
        ProtocolInstance->UpdateStatusFlags(&Config, FALSE);
    }

    Status = gBS->InstallProtocolInterface(
                 &ImageHandle,
                 &gEfiTcgPlatformProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &mTcgPlatformProtocol);

    Status = gBS->InstallProtocolInterface(
                 &ImageHandle,
                 &gAmiTpmSupportTypeProtocolguid,
                 EFI_NATIVE_INTERFACE,
                 &gAmiTpmSupportTypeProtocolguid);
   
   
    Status = EfiCreateEventReadyToBootEx(TPL_CALLBACK,
    									TcgOnReadyToBoot,
                                        NULL,
                                        &ReadyToBootEvent);

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
