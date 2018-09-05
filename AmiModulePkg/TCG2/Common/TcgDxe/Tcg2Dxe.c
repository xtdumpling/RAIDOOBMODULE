/*++
   Module Name:

   TcgDxe.c

   Abstract:

   DXE Driver that provides TCG services

   --*/
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/TcgDxe/TcgDxe20.c 2     10/09/13 6:30p Fredericko $
//
// $Revision: 2 $
//
// $Date: 10/09/13 6:30p $
//*************************************************************************
// Revision History
// ----------------
// $Log:
#include <Uefi.h>
#include "AmiTcg/TcgCommon20.h"
#include <AmiTcg/TCGMisc.h>
#include <Token.h>
#include <AmiTcg/Tpm20.h>
#include <AmiTcg/TrEEProtocol.h>
#include "Protocol/TpmDevice.h"
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/AcpiSupport.h>
#include "Protocol/TcgPlatformSetupPolicy.h"
#include <IndustryStandard/Acpi30.h>
#include <Acpi.h>
#include "../../CRB_lib/Tpm20CRBLib.h"
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include<Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <ImageAuthentication.h>
#include <Protocol/AcpiSupport.h>
#include <Tcg2Dxe.h>
#include <Library/IoLib.h>
#include <ImageAuthentication.h>
#include <Protocol/FirmwareVolume2.h>
#include <Guid/MemoryOverwriteControl.h>
#include <Library/UefiLib.h>
#include <Protocol/Reset.h>
#include <IndustryStandard/SmBios.h>
#include <AmiTcg/AmiTpmStatusCodes.h>
#include <AmiProtocol.h>
#include <Protocol/AmiTcgProtocols.h>
#include <Guid/AmiTcgGuidIncludes.h>
#ifdef AMI_MODULE_PKG_VERSION
#if AMI_MODULE_PKG_VERSION > 27
#include <Guid/AmiResetSystemEvent.h>
#endif
#endif

#if defined (ALLOCATE_PCR_AFTER_SMM_INIT) && (ALLOCATE_PCR_AFTER_SMM_INIT == 1)
#include <Protocol/SmmSwDispatch2.h>
#endif

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

/*
  locates the TPM20 hob from Pei. If found we are processing TPM 20 devic
  need to install the TreeProtocol and do TPM20 binding measurements
*/
#define EFI_ACPI_TABLE_VERSION_X        (EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0 | EFI_ACPI_TABLE_VERSION_4_0)
#define EFI_ACPI_TABLE_VERSION_ALL      (EFI_ACPI_TABLE_VERSION_1_0B|EFI_ACPI_TABLE_VERSION_X)

#define TCPA_PPIOP_SET_PCR_BANKS                         23


extern EFI_GUID gTcgPlatformSetupPolicyGuid;
extern EFI_GUID gEfiAcpiSupportGuid;
#ifdef AMI_MODULE_PKG_VERSION
#if AMI_MODULE_PKG_VERSION < 28
EFI_GUID gAmiResetSystemEventGuid = AMI_RESET_SYSTEN_EVENT_GUID;
#endif
#endif

EFI_EVENT                   Event;
static VOID                 *reg;

#define     MAX_LOG_AREA_SIZE (64 * 1024) // 64KB

static EFI_PHYSICAL_ADDRESS TreeEventLogLocation;
static EFI_PHYSICAL_ADDRESS LastEntry = 0;
static EFI_PHYSICAL_ADDRESS FinalsLastEntry = 0;
static EFI_PHYSICAL_ADDRESS LastEventPtr = 0;
static EFI_TPM_DEVICE_PROTOCOL *TpmDevice=NULL;
Tpm20DeviceHob  *TpmSupport = NULL;
static BOOLEAN  IEventLogTruncated = FALSE;
static BOOLEAN  ReadyToBootSignaled=FALSE;
static EFI_PHYSICAL_ADDRESS TreeExtraTCPASha1LogLoc;
static EFI_PHYSICAL_ADDRESS   pEfiTcg2FinalEventsTbl;
BOOLEAN IsTpm20Device();
EFI_STATUS TcgLibGetDsdt(EFI_PHYSICAL_ADDRESS *DsdtAddr, EFI_ACPI_TABLE_VERSION Version);
EFI_STATUS TcgUpdateAslNameObject(ACPI_HDR *PDsdt, UINT8 *ObjName, UINT64 Value);
BOOLEAN IsPTP();

EFI_STATUS GetAllDigestValues(UINT32 PcrBitMap,
                              TPML_DIGEST_VALUES *HashValues,
                              VOID            *HashData,
                              UINTN           HashDataLen,
                              UINTN           *TotalHashDigestLen);


EFI_HANDLE PlatformProtocolHandle;

UINT32                  ActiveBankBitMap=0;
UINT32                  TcgSupportedBankBitMap=0;
UINT32                  gNumberOfPcrBanks = 0;

UINT32                  Tpm20FwVersion=0;
UINT32                  Tpm20Manufacturer=0;

//
//
// Data Table definition
//
typedef struct _AMI_VALID_CERT_IN_SIG_DB
{
    UINT32          SigOffset;
    UINT32          SigLength;
} AMI_VALID_CERT_IN_SIG_DB;

EFI_TREE_PROTOCOL                 *TrEEProtocolInstance = NULL;

#define AMI_TREE_BOOT_HASH_ALG_SM3    0x00000010

EFI_STATUS
EFIAPI
Tpm2HashSequenceStart (
    IN EFI_TREE_PROTOCOL   *TreeProtocol,
    IN TPMI_ALG_HASH   HashAlg,
    OUT TPMI_DH_OBJECT *SequenceHandle
);

UINT32
EFIAPI
CopyAuthSessionCommand (
    IN      TPMS_AUTH_COMMAND         *AuthSessionIn, OPTIONAL
    OUT     UINT8                     *AuthSessionOut
);

EFI_STATUS
EFIAPI
TrEEDxeGetDigestNonBIOSAlg(
    IN  UINT8                    *DataToHash,
    IN  UINTN                    DataSize,
    IN  OUT  UINT8              *Digest,
    IN  TPMI_ALG_HASH            AlgoId
);


EFI_STATUS
EFIAPI SHA384HashAll(
    IN  VOID            *HashData,
    IN  UINTN           HashDataLen,
    OUT UINT8           *Digest
);

EFI_STATUS
EFIAPI SHA512HashAll(
    IN  VOID            *HashData,
    IN  UINTN           HashDataLen,
    OUT UINT8           *Digest
);

EFI_STATUS
EFIAPI
Tpm2SequenceUpdate (
    IN EFI_TREE_PROTOCOL   *TreeProtocol,
    IN TPMI_DH_OBJECT   SequenceHandle,
    IN TPM2B_MAX_BUFFER *Buffer
);

EFI_STATUS
EFIAPI
Tpm2SequenceComplete (
    IN EFI_TREE_PROTOCOL   *TreeProtocol,
    IN TPMI_DH_OBJECT      SequenceHandle,
    IN TPM2B_MAX_BUFFER    *Buffer,
    OUT TPM2B_DIGEST       *Result
);



EFI_STATUS
EFIAPI
TpmLibPassThrough (
    IN      TPM_1_2_REGISTERS_PTR     TpmReg,
    IN      UINTN                     NoInputBuffers,
    IN      TPM_TRANSMIT_BUFFER       *InputBuffers,
    IN      UINTN                     NoOutputBuffers,
    IN OUT  TPM_TRANSMIT_BUFFER       *OutputBuffers
);

EFI_STATUS
EFIAPI
TisRequestLocality (
    IN      TPM_1_2_REGISTERS_PTR     TpmReg
);

EFI_STATUS
EFIAPI
TisReleaseLocality (
    IN      TPM_1_2_REGISTERS_PTR     TpmReg
);


EFI_STATUS
EFIAPI
TreeSubmitCommand (
    IN  EFI_TREE_PROTOCOL   *This,
    IN  UINT32              InputParameterBlockSize,
    IN  UINT8               *InputParameterBlock,
    IN  UINT32              OutputParameterBlockSize,
    IN  UINT8               *OutputParameterBlock
);

UINTN TransferTcgEvent2ToTcpaEvent( VOID );
EFI_STATUS GetNextSMLEvent(MiscPCR_EVENT_HDR* pStart, MiscPCR_EVENT_HDR** pNext);

static UINT16   *gDriverName=L"TPM Driver";
static BOOLEAN   Tpm20FwDevice = FALSE;


void printbuffer(UINT8 *Buffer, UINTN BufferSize)
{
    UINTN i=0;
    UINTN j=0;

    DEBUG((DEBUG_INFO,"\n**********PrintBuffer Entry********"));

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



EFI_STATUS
EFIAPI
TrEEDxeGetDigestNonBIOSAlg(
    IN  UINT8                    *DataToHash,
    IN  UINTN                    DataSize,
    IN  OUT  UINT8              *Digest,
    IN  TPMI_ALG_HASH            AlgoId
)
{
    TPMI_DH_OBJECT        HashHandle;
    EFI_STATUS            Status;
    TPM2B_MAX_BUFFER      HashBuffer;
    UINT8                 *Buffer;
    TPM2B_DIGEST          Result;
    EFI_TREE_PROTOCOL                 *pTreeProtocol  =  NULL;

    Status = gBS->LocateProtocol(
                 &gEfiTrEEProtocolGuid,
                 NULL,
                 &pTreeProtocol);

    if(EFI_ERROR(Status))return Status;

    Status = Tpm2HashSequenceStart (pTreeProtocol, AlgoId, &HashHandle);

    Buffer = DataToHash;

    while( DataSize )
    {

        HashBuffer.size = (UINT16)( (DataSize >= MAX_DIGEST_BUFFER) ? MAX_DIGEST_BUFFER : DataSize );

        CopyMem(HashBuffer.buffer, Buffer, HashBuffer.size);

        Buffer += HashBuffer.size;
        DataSize -= HashBuffer.size;

        Status = Tpm2SequenceUpdate(pTreeProtocol, (TPMI_DH_OBJECT)HashHandle, &HashBuffer);
        if (EFI_ERROR(Status))
        {
            return EFI_DEVICE_ERROR;
        }
    }

    HashBuffer.size = 0;
    Status = Tpm2SequenceComplete (
                 pTreeProtocol,
                 HashHandle,
                 &HashBuffer,
                 &Result
             );
    if (EFI_ERROR(Status))
    {
        return EFI_DEVICE_ERROR;
    }

    DEBUG((EFI_D_VERBOSE, "\n Tpm2SequenceComplete Success \n"));

    CopyMem (Digest, Result.buffer, Result.size);
    return EFI_SUCCESS;
}


EFI_STATUS GetAllDigestValues(UINT32 PcrBitMap,
                              TPML_DIGEST_VALUES *HashValues,
                              VOID            *HashData,
                              UINTN           HashDataLen,
                              UINTN           *Digestsize)
{
    UINT32     Count = 0;
    UINTN      TotalSize=0;

    if((PcrBitMap & 1) == 1)
    {
        //sha1
        SHA1HashAll(NULL, HashData, HashDataLen, (UINT8 *)&HashValues->digests[Count].digest.sha1);
        HashValues->digests[Count].hashAlg = TPM2_ALG_SHA1;
        TotalSize += SHA1_DIGEST_SIZE;
        Count+=1;
    }

    if((PcrBitMap & 2) == 2)
    {
        //sha256
        SHA2HashAll(NULL, HashData, HashDataLen, (UINT8 *)&HashValues->digests[Count].digest.sha256);
        HashValues->digests[Count].hashAlg = TPM2_ALG_SHA256;
        TotalSize += SHA256_DIGEST_SIZE;
        Count+=1;
    }

    if((PcrBitMap & 4) == 4)
    {
        //sha384
        SHA384HashAll(HashData, HashDataLen, (UINT8 *)&HashValues->digests[Count].digest.sha384);
        HashValues->digests[Count].hashAlg = TPM2_ALG_SHA384;
        TotalSize += SHA384_DIGEST_SIZE;
        Count+=1;
    }

    if((PcrBitMap & 8) == 8)
    {
        //sha512
        SHA512HashAll(HashData, HashDataLen, (UINT8 *)&HashValues->digests[Count].digest.sha512);
        HashValues->digests[Count].hashAlg = TPM2_ALG_SHA512;
        TotalSize += SHA512_DIGEST_SIZE;
        Count+=1;
    }

    if((PcrBitMap & 0x10) == 0x10)
    {
        //Special case //SM3 use hash sequence
        TrEEDxeGetDigestNonBIOSAlg(HashData,HashDataLen,(UINT8 *)&HashValues->digests[Count].digest.sm3_256, TPM2_ALG_SM3_256);
        HashValues->digests[Count].hashAlg = TPM2_ALG_SM3_256;
        TotalSize += SM3_256_DIGEST_SIZE;
        Count+=1;
    }

    DEBUG(( DEBUG_INFO," TotalSize = %x \n", TotalSize));

    HashValues->count = Count;
    *Digestsize = TotalSize;
    return EFI_SUCCESS;
}


UINT8  GetTcgSpecVersion()
{
    TCG_PLATFORM_SETUP_PROTOCOL     *ProtocolInstance;
    EFI_STATUS                      Status;

    Status = gBS->LocateProtocol (&gTcgPlatformSetupPolicyGuid,  NULL, &ProtocolInstance);
    if (EFI_ERROR (Status))
    {
        return 0;
    }

    return(ProtocolInstance->ConfigFlags.Tcg2SpecVersion);
}

BOOLEAN IsTpm20Device()
{

    TpmSupport = LocateATcgHob( gST->NumberOfTableEntries,
                                gST->ConfigurationTable,
                                &gTpm20HobGuid);

    if(TpmSupport != NULL)
    {
        Tpm20FwVersion = TpmSupport->Tpm2FWersion1;
        Tpm20Manufacturer = TpmSupport->Tpm2manufacturer;
        if(TpmSupport->Tpm20DeviceState == 1)
        {
            return TRUE;
        }
    }

    return FALSE;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcmCommonPassThrough
//
// Description: Helper function for TCM transmit command
//
// Input:       VOID *Context
//              UINT32 NoInputBuffers
//              TPM_TRANSMIT_BUFFER InputBuffers
//              UINT32 NoOutputBuffers
//              TPM_TRANSMIT_BUFFER OutputBuffers
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
EFI_STATUS
EFIAPI TcmCommonPassThrough(
    IN VOID                    *Context,
    IN UINT32                  NoInputBuffers,
    IN TPM_TRANSMIT_BUFFER     *InputBuffers,
    IN UINT32                  NoOutputBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutputBuffers )
{
    return EFI_UNSUPPORTED;
}



UINT8  GetInterfacePolicy()
{
    TCG_PLATFORM_SETUP_PROTOCOL     *ProtocolInstance;
    EFI_STATUS                      Status;

    Status = gBS->LocateProtocol (&gTcgPlatformSetupPolicyGuid,  NULL, &ProtocolInstance);
    if (EFI_ERROR (Status))
    {
        return 0;
    }

    return(ProtocolInstance->ConfigFlags.InterfaceSel);
}

EFI_STATUS
EFIAPI
Tpm2GetCapability (
    IN      TPM_CAP                   Capability,
    IN      UINT32                    Property,
    IN      UINT32                    PropertyCount,
    OUT     TPMI_YES_NO               *MoreData,
    OUT     TPMS_CAPABILITY_DATA      *CapabilityData
)
{
    EFI_STATUS                        Status;
    TPM2_GET_CAPABILITY_COMMAND       SendBuffer;
    TPM2_GET_CAPABILITY_RESPONSE      RecvBuffer;
    UINT32                            SendBufferSize;
    UINT32                            RecvBufferSize;
    EFI_TREE_PROTOCOL                 *pTreeProtocol  =  NULL;

    Status = gBS->LocateProtocol(
                 &gEfiTrEEProtocolGuid,
                 NULL,
                 &pTreeProtocol);

    if(EFI_ERROR(Status))return Status;

    //
    // Construct command
    //
    SendBuffer.Header.tag = SwapBytes16(TPM_ST_NO_SESSIONS);
    SendBuffer.Header.commandCode = SwapBytes32(TPM_CC_GetCapability);

    SendBuffer.Capability = SwapBytes32 (Capability);
    SendBuffer.Property = SwapBytes32 (Property);
    SendBuffer.PropertyCount = SwapBytes32 (PropertyCount);

    SendBufferSize = (UINT32) sizeof (SendBuffer);
    SendBuffer.Header.paramSize = SwapBytes32 (SendBufferSize);

    //
    // send Tpm command
    //
    RecvBufferSize = sizeof (RecvBuffer);
    Status = TreeSubmitCommand (pTreeProtocol, SendBufferSize, (UINT8 *)&SendBuffer, RecvBufferSize, (UINT8 *)&RecvBuffer );
    if (EFI_ERROR (Status))
    {
        return Status;
    }

    if (RecvBufferSize <= sizeof (TPM2_RESPONSE_HEADER) + sizeof (UINT8))
    {
        return EFI_DEVICE_ERROR;
    }

    //
    if (SwapBytes32(RecvBuffer.Header.responseCode) != TPM_RC_SUCCESS)
    {
        DEBUG((DEBUG_ERROR, "Tpm2GetCapability: Response Code error! 0x%08x\r\n", SwapBytes32(RecvBuffer.Header.responseCode)));
        return EFI_DEVICE_ERROR;
    }
    // Return the response
    //
    *MoreData = RecvBuffer.MoreData;
    //
    // Does not unpack all possiable property here, the caller should unpack it and note the byte order.
    //
    gBS->CopyMem (CapabilityData, &RecvBuffer.CapabilityData, RecvBufferSize - sizeof (TPM2_RESPONSE_HEADER) - sizeof (UINT8));

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
Tpm2GetCapabilityManufactureID (
    OUT     UINT32                    *ManufactureId
)
{
    TPMS_CAPABILITY_DATA    TpmCap;
    TPMI_YES_NO             MoreData;
    EFI_STATUS              Status;

    Status = Tpm2GetCapability (
                 TPM_CAP_TPM_PROPERTIES,
                 TPM_PT_MANUFACTURER,
                 1,
                 &MoreData,
                 &TpmCap
             );
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    *ManufactureId = SwapBytes32 (TpmCap.data.tpmProperties.tpmProperty->value);

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
Tpm2GetCapabilityMaxCommandResponseSize (
    OUT UINT32                    *MaxCommandSize,
    OUT UINT32                    *MaxResponseSize
)
{
    TPMS_CAPABILITY_DATA    TpmCap;
    TPMI_YES_NO             MoreData;
    EFI_STATUS              Status;

    Status = Tpm2GetCapability (
                 TPM_CAP_TPM_PROPERTIES,
                 TPM_PT_MAX_COMMAND_SIZE,
                 1,
                 &MoreData,
                 &TpmCap
             );
    if (EFI_ERROR (Status))
    {
        return Status;
    }

    *MaxCommandSize = SwapBytes32 (TpmCap.data.tpmProperties.tpmProperty->value);

    Status = Tpm2GetCapability (
                 TPM_CAP_TPM_PROPERTIES,
                 TPM_PT_MAX_RESPONSE_SIZE,
                 1,
                 &MoreData,
                 &TpmCap
             );
    if (EFI_ERROR (Status))
    {
        return Status;
    }

    *MaxResponseSize = SwapBytes32(TpmCap.data.tpmProperties.tpmProperty->value);
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
Tpm2GetCapabilitySupportedAlg (
    OUT TPML_ALG_PROPERTY      *AlgList
)
{
    TPMS_CAPABILITY_DATA    TpmCap;
    TPMI_YES_NO             MoreData;
    UINTN                   Index;
    EFI_STATUS              Status;

    Status = Tpm2GetCapability (
                 TPM_CAP_ALGS,
                 1,
                 MAX_CAP_ALGS,
                 &MoreData,
                 &TpmCap
             );
    if (EFI_ERROR (Status))
    {
        return Status;
    }

    gBS->CopyMem (AlgList, &TpmCap.data.algorithms, sizeof (TPML_ALG_PROPERTY));

    AlgList->count = SwapBytes32 (AlgList->count);
    for (Index = 0; Index < AlgList->count; Index++)
    {
        AlgList->algProperties[Index].alg = SwapBytes16 (AlgList->algProperties[Index].alg);
        *(UINT32 *)&AlgList->algProperties[Index].algProperties = SwapBytes32 (*(UINT32 *)&AlgList->algProperties[Index].algProperties);
    }
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
Tpm20ComponentNameGetDriverName (
    IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
    IN  CHAR8                        *Language,
    OUT CHAR16                       **DriverName
)
{
    //Supports only English
    if(!Language || !DriverName) return EFI_INVALID_PARAMETER;

    if (AsciiStriCmp( Language, "en-US"))
        return EFI_UNSUPPORTED;
    else
        *DriverName=gDriverName;

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
Tpm20ComponentNameGetControllerName (
    IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
    IN  EFI_HANDLE                                      ControllerHandle,
    IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
    IN  CHAR8                                           *Language,
    OUT CHAR16                                          **ControllerName
)
{
    return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Tpm20ComponentNameGetDriverName2 (
    IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
    IN  CHAR8                        *Language,
    OUT CHAR16                       **DriverName
)
{
    //Supports only English
    if(!Language || !DriverName) return EFI_INVALID_PARAMETER;
//LANGUAGE_CODE_ENGLISH
    if (AsciiStriCmp( Language, "en-US"))
        return EFI_UNSUPPORTED;
    else
        *DriverName=gDriverName;

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
Tpm20ComponentNameGetControllerName2 (
    IN  EFI_COMPONENT_NAME2_PROTOCOL                     *This,
    IN  EFI_HANDLE                                      ControllerHandle,
    IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
    IN  CHAR8                                           *Language,
    OUT CHAR16                                          **ControllerName
)
{
    return EFI_UNSUPPORTED;
}


static EFI_COMPONENT_NAME_PROTOCOL gComponentName =
{
    Tpm20ComponentNameGetDriverName,
    Tpm20ComponentNameGetControllerName,
    "en-US"
};


static EFI_COMPONENT_NAME2_PROTOCOL gComponentName2 =
{
    Tpm20ComponentNameGetDriverName2,
    Tpm20ComponentNameGetControllerName2,
    "en-US"
};


EFI_STATUS
EFIAPI
Tpm20DriverBindingSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath   OPTIONAL
)
{
    EFI_STATUS          Status      =  EFI_SUCCESS;
    static BOOLEAN      Initialized = FALSE;

    if(Initialized == FALSE)
    {
        Initialized = TRUE;
        if(IsTpm20Device())
        {
            return EFI_SUCCESS;
        }
        else if(Tpm20FwDevice)return EFI_SUCCESS;
    }

    return EFI_UNSUPPORTED;
}


EFI_STATUS
EFIAPI
Tpm20DriverBindingStart (
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath   OPTIONAL
)
{
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
Tpm20DriverBindingStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
    IN  EFI_HANDLE                      Controller,
    IN  UINTN                           NumberOfChildren,
    IN  EFI_HANDLE                      *ChildHandleBuffer   OPTIONAL
)
{
    return EFI_UNSUPPORTED;
}

EFI_DRIVER_BINDING_PROTOCOL Tpm20DriverBinding =
{
    Tpm20DriverBindingSupported,
    Tpm20DriverBindingStart,
    Tpm20DriverBindingStop,
    0xa,
    NULL,
    NULL
};


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   OverwriteSystemMemory
//
// Description: Overwrites system memory
//
// Input:
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
EFI_STATUS OverwriteSystemMemory(
)
{

    EFI_STATUS                           Status;
    UINT8                                TmpMemoryMap[1];
    UINTN                                MapKey;
    UINTN                                DescriptorSize;
    UINT32                               DescriptorVersion;
    UINTN                                MemoryMapSize;
    EFI_MEMORY_DESCRIPTOR                *MemoryMap;
    EFI_MEMORY_DESCRIPTOR                *MemoryMapPtr;
    UINTN                                Index;
    UINT64                                Size;

    //
    // Get System MemoryMapSize
    //
    MemoryMapSize = 1;
    Status = gBS->GetMemoryMap (
                 &MemoryMapSize,
                 (EFI_MEMORY_DESCRIPTOR *)TmpMemoryMap,
                 &MapKey,
                 &DescriptorSize,
                 &DescriptorVersion
             );
    ASSERT (Status == EFI_BUFFER_TOO_SMALL);
    //
    // Enlarge space here, because we will allocate pool now.
    //
    MemoryMapSize += EFI_PAGE_SIZE;
    Status = gBS->AllocatePool (
                 EfiBootServicesData,
                 MemoryMapSize,
                 (VOID**)&MemoryMap
             );
    ASSERT_EFI_ERROR (Status);

    //
    // Get System MemoryMap
    //
    Status = gBS->GetMemoryMap (
                 &MemoryMapSize,
                 MemoryMap,
                 &MapKey,
                 &DescriptorSize,
                 &DescriptorVersion
             );
    ASSERT_EFI_ERROR (Status);

    MemoryMapPtr = MemoryMap;
    //
    // Search the request Address
    //
    for (Index = 0; Index < (MemoryMapSize / DescriptorSize); Index++)
    {
        switch (MemoryMap->Type)
        {
            case EfiMemoryMappedIO:
            case EfiReservedMemoryType:
            case EfiRuntimeServicesCode:
            case EfiRuntimeServicesData:
            case EfiUnusableMemory:
            case EfiMemoryMappedIOPortSpace:
            case EfiPalCode:
            case EfiACPIReclaimMemory:
            case EfiACPIMemoryNVS:
            case EfiBootServicesCode:
            case EfiBootServicesData:
            case EfiLoaderCode:
            case EfiLoaderData:
            case EfiMaxMemoryType:
                Size = MemoryMap->NumberOfPages <<  EFI_PAGE_SHIFT;
                DEBUG((DEBUG_INFO,"MOR: Start = %x Len = %x", MemoryMap->PhysicalStart, Size));
                DEBUG((DEBUG_INFO," Left Alone \n"));
                break;
            default:
                Size = MemoryMap->NumberOfPages <<  EFI_PAGE_SHIFT;
                DEBUG((DEBUG_INFO,"MOR: Start = %x Len = %x", MemoryMap->PhysicalStart, Size));
                DEBUG((DEBUG_INFO," Cleaned \n"));
                Size = MemoryMap->NumberOfPages <<  EFI_PAGE_SHIFT;
                SetMem((VOID*)MemoryMap->PhysicalStart, (UINTN)Size, 0);
        }
        MemoryMap = (EFI_MEMORY_DESCRIPTOR *)((UINTN)MemoryMap + DescriptorSize);
    }

    //
    // Done
    //
    gBS->FreePool (MemoryMapPtr);

    return Status;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   ReadMORValue
//
// Description: Reads TCG MOR variable
//
// Input:       IN  EFI_PEI_SERVICES  **PeiServices,
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
VOID ReadMORValue( )
{
    CHAR16     UefiMor[]   = L"MemoryOverwriteRequestControl";
    UINT8      mor         = 0;
    UINTN      size        = sizeof(mor);
    EFI_STATUS Status;


    Status = gRT->GetVariable( UefiMor, &gEfiMemoryOverwriteControlDataGuid,
                               NULL, &size, &mor );

    if(EFI_ERROR(Status))return;

    if ( (mor & 1)!=0 )
    {

        //clear memory
        DEBUG((DEBUG_INFO,"MOR: before Clear memory"));
#if !(defined(TCG_SKIP_MOR_FULL) && TCG_SKIP_MOR_FULL == 1)
        Status = OverwriteSystemMemory();
        if(EFI_ERROR(Status))
        {
            TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_MOR_REQUEST_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
            DEBUG((DEBUG_INFO,"OverwriteSystemMemory Failed"));
        }else{
            TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_MOR_REQUEST_ACK_EXECUTED | EFI_SOFTWARE_DXE_BS_DRIVER);
        }
#endif
        DEBUG((DEBUG_INFO,"MOR: After Clear memory"));
    }
}


EFI_STATUS
EFIAPI
Tpm2GetCapabilityCapPCRS ()
{
    TPMS_CAPABILITY_DATA    TpmCap;
    TPMI_YES_NO             MoreData;
    EFI_STATUS              Status;
    TPMS_PCR_SELECTION      *PcrSelect;
    UINT8                   *Buffer;
    UINTN                   size = 0, i=0, j=0;
    UINT32                  SupportedBankBitMap=0;
    AMITCGSETUPINFOFLAGS    Info;
    UINT16                  hash;

    SupportedBankBitMap = 0;

    Status  = Tpm2GetCapability (
                  TPM_CAP_PCRS,
                  0,
                  MAX_PCR_PROPERTIES,
                  &MoreData,
                  &TpmCap);

    if(EFI_ERROR(Status))
    {
        Info.SupportedPcrBitMap = 1;
        Info.ActivePcrBitMap  = 1;
        Info.Reserved = 0;

        DEBUG(( DEBUG_INFO," SupportedPcrBitMap = %x \n", Info.SupportedPcrBitMap));
        DEBUG(( DEBUG_INFO," ActivePcrBitMap = %x \n", Info.ActivePcrBitMap));

        Status = gRT->SetVariable( L"PCRBitmap", \
                                   &gTcgInternalflagGuid, \
                                   EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, \
                                   sizeof(AMITCGSETUPINFOFLAGS), \
                                   &Info);
        return Status;
    }

    //printbuffer((UINT8 *)&TpmCap, 0x30);

    Buffer = (UINT8 *)&TpmCap;
    PcrSelect = (TPMS_PCR_SELECTION *)(Buffer + (sizeof(UINT32)*2));
    size = SwapBytes32(*(UINT32 *)(Buffer + sizeof(UINT32)));

    DEBUG(( DEBUG_INFO," size = %x \n", size));

    //ActiveBankBitMap
    for(i=0; i<size; i++,PcrSelect++)
    {

        //printbuffer((UINT8 *)PcrSelect, 0x30);
        DEBUG(( DEBUG_INFO," PcrSelect->hash = %x \n", PcrSelect->hash));
        DEBUG(( DEBUG_INFO," PcrSelect->sizeofSelect = %x \n", PcrSelect->sizeofSelect));
        hash = SwapBytes16(PcrSelect->hash);
        switch(hash)
        {
            case 0x4:
                SupportedBankBitMap |= 1;
                for(j=0; j<PcrSelect->sizeofSelect; j++)
                {
                    if(PcrSelect->pcrSelect[j] ==0)continue;
                    else
                    {
                        ActiveBankBitMap |= 1;
                        break;
                    }
                }
                break;
            case 0xB:
                SupportedBankBitMap |= 2;
                for(j=0; j<PcrSelect->sizeofSelect; j++)
                {
                    if(PcrSelect->pcrSelect[j] ==0)continue;
                    else
                    {
                        ActiveBankBitMap |= 2;
                        break;
                    }
                }
                break;
            case 0xC:
                SupportedBankBitMap |= 4;
                for(j=0; j<PcrSelect->sizeofSelect; j++)
                {
                    if(PcrSelect->pcrSelect[j] ==0)continue;
                    else
                    {
                        ActiveBankBitMap |= 4;
                        break;
                    }
                }
                break;
            case 0xD:
                SupportedBankBitMap |= 8;
                for(j=0; j<PcrSelect->sizeofSelect; j++)
                {
                    if(PcrSelect->pcrSelect[j] ==0)continue;
                    else
                    {
                        ActiveBankBitMap |= 8;    // Correct for EFI_TCG2_BOOT_HASH_ALG512
                        break;
                    }
                }
                break;
            case 0x12:
                for(j=0; j<PcrSelect->sizeofSelect; j++)
                {
                    if(PcrSelect->pcrSelect[j] ==0)continue;
                    else
                    {
                        ActiveBankBitMap |= 0x10;
                        break;
                    }
                }
                SupportedBankBitMap |= 0x10;
                break;
            default:
                break;
        }
    }

    for( i=0, gNumberOfPcrBanks=0; i<16; ++i)
    {
        if( (SupportedBankBitMap & (1<<i)) )
            ++gNumberOfPcrBanks;
    }
    TcgSupportedBankBitMap = SupportedBankBitMap;
    Info.SupportedPcrBitMap = SupportedBankBitMap;
    Info.ActivePcrBitMap  = ActiveBankBitMap;
    Info.Reserved = 0;
    Info.TpmFwVersion = Tpm20FwVersion;
    Info.TpmManufacturer = Tpm20Manufacturer;

    DEBUG(( DEBUG_INFO," SupportedPcrBitMap = %x \n", Info.SupportedPcrBitMap));
    DEBUG(( DEBUG_INFO," ActivePcrBitMap = %x \n", Info.ActivePcrBitMap));

    Status = gRT->SetVariable( L"PCRBitmap", \
                               &gTcgInternalflagGuid, \
                               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, \
                               sizeof(AMITCGSETUPINFOFLAGS), \
                               &Info);

    return Status;
}

EFI_STATUS
EFIAPI
Tpm2Shutdown (
  IN      TPM_SU             ShutdownType
  )
{
  EFI_STATUS                        Status;
  TPM2_SHUTDOWN_COMMAND             Cmd;
  TPM2_SHUTDOWN_RESPONSE            Res;
  UINT32                            ResultBufSize;
  EFI_TREE_PROTOCOL                 *pTreeProtocol  =  NULL;
  EFI_GUID                          gEfiTrEEProtocolGuid =  EFI_TREE_PROTOCOL_GUID;

  Status = gBS->LocateProtocol(
                  &gEfiTrEEProtocolGuid,
                  NULL,
                  &pTreeProtocol);

  if(EFI_ERROR(Status))return Status;

  Cmd.Header.tag         = SwapBytes16(TPM_ST_NO_SESSIONS);
  Cmd.Header.paramSize   = SwapBytes32(sizeof(Cmd));
  Cmd.Header.commandCode = SwapBytes32(TPM_CC_Shutdown);
  Cmd.ShutdownType       = SwapBytes16(ShutdownType);

  ResultBufSize = sizeof(Res);
  Status = TreeSubmitCommand (pTreeProtocol, sizeof(Cmd), (UINT8 *)&Cmd, ResultBufSize, (UINT8 *)&Res);

  return Status;
}

EFI_STATUS
EFIAPI
TPM2ResetSystemWorkAround(
    IN EFI_EVENT ev,
    IN VOID      *ctx )
{
    DEBUG((DEBUG_INFO, "\tResetSystem, Tpm2Shutdown(TPM_SU_CLEAR)\n"));
    // Before ResetSystem/ReBoot, we need to execute TPM2_Shutdown(ST_CLEAR) to Reflash the TPM
    // Internal NVRAM, make the PCR changed command work.
    // For NTZ TPM, we need the patch.
    Tpm2Shutdown(TPM_SU_CLEAR);
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
DoResetNow(
    IN EFI_EVENT ev,
    IN VOID      *ctx )
{
    gRT->ResetSystem( EfiResetCold, 0, 0, NULL );
    DEBUG((DEBUG_INFO, "\tError: Reset failed???\n"));
    return EFI_SUCCESS;
}


EFI_STATUS 
EFIAPI
Tpm2AllocatePCR(UINT32 RequestedBank, UINT32 ActivePCRBanks, UINT32 Supported, UINT32 *TpmErrorCode)
{
    EFI_STATUS                 Status;
    TPM2_ALLOCATE_PCR_COMMAND  Tpm2Alloccmd;
    TPM2_ALLOCATE_PCR_RESPONSE Tpm2AllocResponse;
    UINT8                      *Buffer;
    UINT32                     *AuthsizeOffset;
    TPML_PCR_SELECTION         pcrAllocation;
    UINT32                     CmdSize = 0;
    UINT32                     RecvBufferSize=0;
    EFI_TREE_PROTOCOL                 *pTreeProtocol  =  NULL;
    TPMI_YES_NO                *AllocPass;
    UINT8                       i=0, Count=0;
    BOOLEAN                     AllocateFail = FALSE;



    Status = gBS->LocateProtocol(
                 &gEfiTrEEProtocolGuid,
                 NULL,
                 &pTreeProtocol);

    if(EFI_ERROR(Status))return Status;

    gBS->SetMem(&Tpm2Alloccmd, sizeof(TPM2_ALLOCATE_PCR_COMMAND), 0);
    gBS->SetMem(&Tpm2AllocResponse, sizeof(TPM2_ALLOCATE_PCR_RESPONSE), 0);


    Tpm2Alloccmd.Header.tag          = SwapBytes16(TPM_ST_SESSIONS);
    Tpm2Alloccmd.Header.paramSize    = SwapBytes32(sizeof(TPM2_ALLOCATE_PCR_COMMAND));
    Tpm2Alloccmd.Header.commandCode  = SwapBytes32(TPM_CC_PCR_Allocate);
    Tpm2Alloccmd.Authhandle          = SwapBytes32(TPM_RH_PLATFORM);

    Buffer = (UINT8 *)&Tpm2Alloccmd.AuthorizationSize;
    AuthsizeOffset = (UINT32 *)Buffer;

    //set AuthorizationSize to 0
    *(UINT32 *)Buffer = 0;
    Buffer += sizeof(UINT32);

    //init authSessionHandle
    *(UINT32 *)Buffer = SwapBytes32(TPM_RS_PW);
    Buffer += sizeof(UINT32);

    //use nullNonce
    *(UINT16 *)Buffer = 0;
    Buffer += sizeof(UINT16);

    //sessionAttributes should be zero
    *(UINT8 *)Buffer = 0;
    Buffer += sizeof(UINT8);


    //use = nullAuth
    *(UINT16 *)Buffer = 0;
    Buffer += sizeof(UINT16);


    *AuthsizeOffset = SwapBytes32((UINT32)(Buffer - (UINT8 *)AuthsizeOffset - sizeof(UINT32)));

    DEBUG((DEBUG_INFO," Tpm2AllocatePCR::Supported = %x \n", Supported));
    DEBUG((DEBUG_INFO," Tpm2AllocatePCR::RequestedBank = %x \n", RequestedBank));
    DEBUG((DEBUG_INFO," Tpm2AllocatePCR::ActivePCRBanks = %x \n", ActivePCRBanks));

    if(((Supported & TREE_BOOT_HASH_ALG_SHA1)!=0) && \
            (RequestedBank & TREE_BOOT_HASH_ALG_SHA1) == TREE_BOOT_HASH_ALG_SHA1)
    {

        DEBUG((DEBUG_INFO," Tpm2AllocatePCR::Allocate Sha-1 \n", Count));
        pcrAllocation.pcrSelections[i].hash = SwapBytes16(TPM2_ALG_SHA1);
        pcrAllocation.pcrSelections[i].pcrSelect[0] = (0xFF);
        pcrAllocation.pcrSelections[i].pcrSelect[1] = (0xFF);
        pcrAllocation.pcrSelections[i].pcrSelect[2] = (0xFF);
        pcrAllocation.pcrSelections[i].sizeofSelect = 3;
        i+=1;
        Count++;
    }


    if((Supported & TREE_BOOT_HASH_ALG_SHA256) && \
            (RequestedBank & TREE_BOOT_HASH_ALG_SHA256) == TREE_BOOT_HASH_ALG_SHA256 && i < HASH_COUNT)
    {
        DEBUG((DEBUG_INFO," Tpm2AllocatePCR::Allocate Sha-2 \n", Count));
        pcrAllocation.pcrSelections[i].hash = SwapBytes16(TPM2_ALG_SHA256);
        pcrAllocation.pcrSelections[i].pcrSelect[0] = (0xFF);
        pcrAllocation.pcrSelections[i].pcrSelect[1] = (0xFF);
        pcrAllocation.pcrSelections[i].pcrSelect[2] = (0xFF);
        pcrAllocation.pcrSelections[i].sizeofSelect = 3;
        i+=1;
        Count++;
    }


    if(((Supported & TREE_BOOT_HASH_ALG_SHA384)!=0) && \
            (RequestedBank & TREE_BOOT_HASH_ALG_SHA384) == TREE_BOOT_HASH_ALG_SHA384 && i < HASH_COUNT)
    {
        DEBUG((DEBUG_INFO," Tpm2AllocatePCR::Allocate Sha-3 \n", Count));
        pcrAllocation.pcrSelections[i].hash = SwapBytes16(TPM2_ALG_SHA384);
        pcrAllocation.pcrSelections[i].pcrSelect[0] = (0xFF);
        pcrAllocation.pcrSelections[i].pcrSelect[1] = (0xFF);
        pcrAllocation.pcrSelections[i].pcrSelect[2] = (0xFF);
        pcrAllocation.pcrSelections[i].sizeofSelect = 3;
        i+=1;
        Count++;
    }


    if(((Supported & TREE_BOOT_HASH_ALG_SHA512)!=0) && \
            (RequestedBank & TREE_BOOT_HASH_ALG_SHA512) == TREE_BOOT_HASH_ALG_SHA512 && i < HASH_COUNT)
    {
        DEBUG((DEBUG_INFO," Tpm2AllocatePCR::Allocate Sha-512 \n", Count));
        pcrAllocation.pcrSelections[i].hash = SwapBytes16(TPM2_ALG_SHA512);
        pcrAllocation.pcrSelections[i].pcrSelect[0] = (0xFF);
        pcrAllocation.pcrSelections[i].pcrSelect[1] = (0xFF);
        pcrAllocation.pcrSelections[i].pcrSelect[2] = (0xFF);
        pcrAllocation.pcrSelections[i].sizeofSelect = 3;
        i+=1;
        Count++;
    }


    if(((Supported & TREE_BOOT_HASH_ALG_SM3)!=0) && \
            (RequestedBank & TREE_BOOT_HASH_ALG_SM3) == TREE_BOOT_HASH_ALG_SM3 && i < HASH_COUNT)
    {
        DEBUG((DEBUG_INFO," Tpm2AllocatePCR::Allocate SM3 \n", Count));
        pcrAllocation.pcrSelections[i].hash = SwapBytes16(TPM2_ALG_SM3_256);
        pcrAllocation.pcrSelections[i].pcrSelect[0] = (0xFF);
        pcrAllocation.pcrSelections[i].pcrSelect[1] = (0xFF);
        pcrAllocation.pcrSelections[i].pcrSelect[2] = (0xFF);
        pcrAllocation.pcrSelections[i].sizeofSelect = 3;
        i+=1;
        Count++;
    }


    if(((ActivePCRBanks & TREE_BOOT_HASH_ALG_SHA1)!=0) && \
            ((RequestedBank & TREE_BOOT_HASH_ALG_SHA1) == 0) && i < HASH_COUNT)
    {
        DEBUG((DEBUG_INFO," Tpm2AllocatePCR::deAllocate Sha-1 \n", Count));
        pcrAllocation.pcrSelections[i].hash = SwapBytes16(TPM2_ALG_SHA1);
        pcrAllocation.pcrSelections[i].pcrSelect[0] = (0x0);
        pcrAllocation.pcrSelections[i].pcrSelect[1] = (0x0);
        pcrAllocation.pcrSelections[i].pcrSelect[2] = (0x0);
        pcrAllocation.pcrSelections[i].sizeofSelect = 3;
        i+=1;
        Count++;
    }

    if(((ActivePCRBanks & TREE_BOOT_HASH_ALG_SHA256)!=0) && \
            ((RequestedBank & TREE_BOOT_HASH_ALG_SHA256) == 0) && i < HASH_COUNT)
    {
        DEBUG((DEBUG_INFO," Tpm2AllocatePCR::deAllocate Sha-2 \n", Count));
        pcrAllocation.pcrSelections[i].hash = SwapBytes16(TPM2_ALG_SHA256);
        pcrAllocation.pcrSelections[i].pcrSelect[0] = (0x0);
        pcrAllocation.pcrSelections[i].pcrSelect[1] = (0x0);
        pcrAllocation.pcrSelections[i].pcrSelect[2] = (0x0);
        pcrAllocation.pcrSelections[i].sizeofSelect = 3;
        i+=1;
        Count++;
    }


    if(((ActivePCRBanks & TREE_BOOT_HASH_ALG_SHA384)!=0) && \
            ((RequestedBank & TREE_BOOT_HASH_ALG_SHA384) == 0) && i < HASH_COUNT)
    {
        DEBUG((DEBUG_INFO," Tpm2AllocatePCR::deAllocate Sha-3 \n", Count));
        pcrAllocation.pcrSelections[i].hash = SwapBytes16(TPM2_ALG_SHA384);
        pcrAllocation.pcrSelections[i].pcrSelect[0] = (0x0);
        pcrAllocation.pcrSelections[i].pcrSelect[1] = (0x0);
        pcrAllocation.pcrSelections[i].pcrSelect[2] = (0x0);
        pcrAllocation.pcrSelections[i].sizeofSelect = 3;
        i+=1;
        Count++;
    }


    if(((ActivePCRBanks & TREE_BOOT_HASH_ALG_SHA512)!=0) && \
            ((RequestedBank & TREE_BOOT_HASH_ALG_SHA512) == 0) && i < HASH_COUNT)
    {
        DEBUG((DEBUG_INFO," Tpm2AllocatePCR::deAllocate Sha-512 \n", Count));
        pcrAllocation.pcrSelections[i].hash = SwapBytes16(TPM2_ALG_SHA512);
        pcrAllocation.pcrSelections[i].pcrSelect[0] = (0x0);
        pcrAllocation.pcrSelections[i].pcrSelect[1] = (0x0);
        pcrAllocation.pcrSelections[i].pcrSelect[2] = (0x0);
        pcrAllocation.pcrSelections[i].sizeofSelect = 3;
        i+=1;
        Count++;
    }

    if(((ActivePCRBanks & TREE_BOOT_HASH_ALG_SM3)!=0) && \
            ((RequestedBank & TREE_BOOT_HASH_ALG_SM3) == 0) && i < HASH_COUNT)
    {
        DEBUG((DEBUG_INFO," Tpm2AllocatePCR::deAllocate SM3 \n", Count));
        pcrAllocation.pcrSelections[i].hash = SwapBytes16(TPM2_ALG_SM3_256);
        pcrAllocation.pcrSelections[i].pcrSelect[0] = (0x0);
        pcrAllocation.pcrSelections[i].pcrSelect[1] = (0x0);
        pcrAllocation.pcrSelections[i].pcrSelect[2] = (0x0);
        pcrAllocation.pcrSelections[i].sizeofSelect = 3;
        i+=1;
        Count++;
    }

    if(Count > HASH_COUNT)Count=HASH_COUNT; // Using the TPM20.h Structure Default define Count.
    pcrAllocation.count = SwapBytes32(Count);

    CopyMem(Buffer, &pcrAllocation.count, sizeof(UINT32));
    Buffer+= sizeof(UINT32);
    CopyMem(Buffer, &pcrAllocation.pcrSelections[0].hash, sizeof(TPMS_PCR_SELECTION)*i);
    Buffer+=(sizeof(TPMS_PCR_SELECTION)*i);

    CmdSize = (UINT32)(Buffer - (UINT8 *)&Tpm2Alloccmd);
    Tpm2Alloccmd.Header.paramSize = SwapBytes32(CmdSize);

    //printbuffer((UINT8 *)&Tpm2Alloccmd, CmdSize);

    //send command
    Status = TreeSubmitCommand (pTreeProtocol, CmdSize, (UINT8 *)&Tpm2Alloccmd,
                                sizeof(TPM2_ALLOCATE_PCR_RESPONSE), (UINT8 *)&Tpm2AllocResponse );
    if (EFI_ERROR (Status))
    {
        DEBUG((DEBUG_INFO," Tpm2AllocatePCR:: TreeSubmitCommand returned error \n"));
        AllocateFail = TRUE;
        goto FAIL_RETURN;
    }

    RecvBufferSize = SwapBytes32(Tpm2AllocResponse.Header.paramSize);

    DEBUG((DEBUG_INFO," Tpm2AllocatePCR:: RecvBufferSize = %x \n", RecvBufferSize));

    *TpmErrorCode = SwapBytes32(Tpm2AllocResponse.Header.responseCode);
    if(SwapBytes32(Tpm2AllocResponse.Header.responseCode))
    {
        DEBUG((DEBUG_ERROR," Tpm2AllocResponse.Header.responseCode = %x \n", SwapBytes32(Tpm2AllocResponse.Header.responseCode)));
        AllocateFail = TRUE;
        goto FAIL_RETURN;
    }

    if (RecvBufferSize <= sizeof (TPM2_RESPONSE_HEADER) + sizeof (UINT8))
    {
        DEBUG((DEBUG_ERROR," Tpm2AllocatePCR:: RecvBufferSize <= sizeof (TPM2_RESPONSE_HEADER) + sizeof (UINT8) \n"));
        AllocateFail = TRUE;
        goto FAIL_RETURN;
    }

    AllocPass = (TPMI_YES_NO *)&Tpm2AllocResponse;
    AllocPass += (sizeof(TPM2_COMMAND_HEADER) + sizeof(UINT32));

    if(*AllocPass != 1)
    {
        DEBUG((DEBUG_ERROR," Tpm2AllocatePCR:: TPM Alloca failed \n"));
        AllocateFail = TRUE;
        goto FAIL_RETURN;
    }

    DEBUG((DEBUG_INFO," Tpm2AllocatePCR:: Allocation success \n"));
    return EFI_SUCCESS;

FAIL_RETURN:
    if(RequestedBank & TREE_BOOT_HASH_ALG_SHA1){
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_SHA_1_ACTIVE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
    }

    if(RequestedBank & TREE_BOOT_HASH_ALG_SHA256){
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_SHA_256_ACTIVE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
    }

    if(RequestedBank & TREE_BOOT_HASH_ALG_SHA384){
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_SHA_384_ACTIVE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
    }

    if(RequestedBank & TREE_BOOT_HASH_ALG_SHA512){
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_SHA_512_ACTIVE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
    }

    if(RequestedBank & TREE_BOOT_HASH_ALG_SM3){
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_SHA_SM3_ACTIVE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
    }

    return EFI_DEVICE_ERROR;
}


#if defined (ALLOCATE_PCR_AFTER_SMM_INIT) && (ALLOCATE_PCR_AFTER_SMM_INIT == 1)
EFI_STATUS
EFIAPI
Tpm2AllocatePCRCallback (IN EFI_EVENT ev,
                                    IN VOID      *ctx)
{
    EFI_STATUS Status;
    TCG_PLATFORM_SETUP_PROTOCOL     *PolicyInstance;
    static              EFI_EVENT      Resetev;
    static      void    * Resetreg;
    UINT32              TpmError;
    EFI_SMM_BASE2_PROTOCOL  *SmmBase2 = NULL;
    UINT8               ResetVal=0;

    DEBUG((DEBUG_INFO, "Tpm2AllocatePCRCallback Entry\n"));
    Status = gBS->LocateProtocol (&gTcgPlatformSetupPolicyGuid,  NULL, &PolicyInstance);
    if (EFI_ERROR (Status))
    {
        return Status;
    }


    //smm should be ready now
    Status = gBS->LocateProtocol( &gEfiSmmBase2ProtocolGuid, NULL, &SmmBase2 );
    if ( EFI_ERROR(Status) || SmmBase2 == NULL) return EFI_NOT_READY;

    Status = Tpm2AllocatePCR(PolicyInstance->ConfigFlags.PcrBanks , ActiveBankBitMap, TcgSupportedBankBitMap, &TpmError);
    if( !EFI_ERROR(Status))
    {
        ResetVal = 1;
        Status = gRT->SetVariable( L"Tpm20PCRallocateReset", \
                                   &gTcgInternalflagGuid, \
                                   EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, \
                                   sizeof(UINT8), \
                                   &ResetVal);

        if(!EFI_ERROR(Status))
        {
            gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
            DEBUG((DEBUG_ERROR, "\tError: Reset failed???\n"));

            Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                                       EFI_TPL_CALLBACK, DoResetNow, NULL, &Resetev);

            ASSERT( !EFI_ERROR( Status ));
            Status = gBS->RegisterProtocolNotify(&gEfiResetArchProtocolGuid, Resetev, &Resetreg);
            DEBUG((DEBUG_ERROR, "\tRegister DoResetNow after Reset Architecture driver\n"));
        }
    }

    return  Status;
}
#endif


EFI_STATUS
EFIAPI
EfiTreeGetActivePcrs(
    EFI_TREE_PROTOCOL    *This,
    UINT32               *ActivatePcrBanks
)
{
    if (This == NULL || ActivatePcrBanks == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }

    DEBUG(( DEBUG_INFO, " EfiTreeGetActivePcrs ActiveBankBitMap[0x%x]\n", ActiveBankBitMap));
    *ActivatePcrBanks = ActiveBankBitMap;
    return EFI_SUCCESS;
}

EFI_STATUS
EfiTreeGetResultOfSetActivePcrs(
    IN  EFI_TREE_PROTOCOL   *This,
    OUT UINT32              *OperationPresent,
    OUT UINT32              *Response
)
{
    EFI_STATUS  Status = EFI_INVALID_PARAMETER;
    UINTN               Size = sizeof(AMI_PPI_NV_VAR);
    AMI_PPI_NV_VAR      Temp;

    if ((OperationPresent == NULL) || (Response == NULL))
    {
        return EFI_INVALID_PARAMETER;
    }

    DEBUG(( DEBUG_INFO, "[%s] [%d]:EfiTreeGetResultOfSetActivePcrs \n", __FILE__, __LINE__));

    Status = gRT->GetVariable( L"AMITCGPPIVAR", \
                               &AmitcgefiOsVariableGuid, \
                               NULL, \
                               &Size, \
                               &Temp );

    if(EFI_ERROR(Status))
    {
        DEBUG(( DEBUG_ERROR, "[%s] [%d]: Status Error(...)\n", __FILE__, __LINE__));
        return Status;
    }

    if(Temp.RCNT  == TCPA_PPIOP_SET_PCR_BANKS)
    {
        *OperationPresent = 1;
        *Response = Temp.ERROR;
        DEBUG(( DEBUG_ERROR, "Temp.ERROR = %x \n", Temp.ERROR));
        if(*Response != EFI_SUCCESS)
        {
            return EFI_UNSUPPORTED;
        }
    }

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
EfiTreeSetActivePcrs(
    EFI_TREE_PROTOCOL    *This,
    UINT32               ActivatePcrBanks
)
{
    EFI_STATUS  Status = EFI_INVALID_PARAMETER;
    UINTN               VariableSize  =  sizeof(AMI_PPI_NV_VAR);
    AMI_PPI_NV_VAR      Variable;
    UINT32              VariableAttributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE;

    DEBUG(( DEBUG_INFO, "[%d]: Enter EfiTreeSetActivePcrs(...)\n", __LINE__));
    DEBUG(( DEBUG_INFO, "EfiTreeSetActivePcrs ActivatePcrBanks[0x%x]\n", ActivatePcrBanks));
    DEBUG(( DEBUG_INFO, "EfiTreeSetActivePcrs ActiveBankBitMap[0x%x]\n", ActiveBankBitMap));
    DEBUG(( DEBUG_INFO, "EfiTreeSetActivePcrs TcgSupportedBankBitMap[0x%x]\n", TcgSupportedBankBitMap));

    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_PROTOCOL_SET_ACTIVE_PCRS_REQUEST | EFI_SOFTWARE_DXE_BS_DRIVER);

    if(This == NULL){
    	return EFI_INVALID_PARAMETER;
    }

    if(((ActivatePcrBanks & TcgSupportedBankBitMap) != ActivatePcrBanks) || (ActivatePcrBanks == 0))
    {
        Status = EFI_INVALID_PARAMETER;
        return Status;
    }

    if(ActivatePcrBanks == ActiveBankBitMap)
    {
        Status = EFI_SUCCESS;
        return Status;
    }

    // Check the ActivatePcrBanks is under the TcgSupportedBankBitMap Sets
    if( (TcgSupportedBankBitMap & ActivatePcrBanks) == ActivatePcrBanks )
    {
        Variable.RQST = TCPA_PPIOP_SET_PCR_BANKS;
        Variable.RCNT = TCPA_PPIOP_SET_PCR_BANKS;
        Variable.OptionalData = ActivatePcrBanks;

        Status = gRT->SetVariable (L"AMITCGPPIVAR2",
                                   &AmitcgefiOsVariableGuid,
                                   VariableAttributes,
                                   VariableSize,
                                   &Variable);

        DEBUG(( DEBUG_INFO, "EfiTreeSetActivePcrs SetVariable[0x%r]\n", Status));
    }



    return Status;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TreeGetCapability
//
// Description: Get Tree Capability
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
#pragma optimize("",off)
EFI_STATUS
EFIAPI
TreeGetCapability (
    IN EFI_TREE_PROTOCOL                *This,
    IN OUT TREE_BOOT_SERVICE_CAPABILITY *ProtocolCapability
)
{
    EFI_STATUS        Status = EFI_SUCCESS;
    UINT8             Tcg2SpecVersion = GetTcgSpecVersion();
    static UINT32     ManufactureID = 0xFFFFFFFF;
    static UINT32     MaxResponseSize = 0xFFFFFFFF;
    static UINT32     MaxCommandSize  = 0xFFFFFFFF;
    TPML_ALG_PROPERTY  AlgList;
    BOOLEAN           Fillsize=FALSE;
    UINTN             Size;
    TREE_BOOT_SERVICE_CAPABILITY Capability;
    UINTN             i=0;

    TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr = (TPM_CRB_ACCESS_REG_PTR)(( UINTN ) (PORT_TPM_IOMEMBASE));

    DEBUG(( DEBUG_INFO, "TreeGetCapability Entry\n"));
    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_PROTOCOL_GET_CAPABILITY_REQUEST | EFI_SOFTWARE_DXE_BS_DRIVER);
    if((ProtocolCapability == NULL) || (This == NULL))
    {
        TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_ERR_TCG_PROTOCOL_GET_CAPABILITY_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
        Status = EFI_INVALID_PARAMETER;
    }
    else
    {
        Size = ProtocolCapability->Size;
        if(ManufactureID == 0xFFFFFFFF && MaxResponseSize == 0xFFFFFFFF && MaxCommandSize ==0xFFFFFFFF)
        {
            Status = Tpm2GetCapabilityManufactureID (&ManufactureID);
            if(EFI_ERROR(Status))
            {
                ManufactureID=0x0;
                Status = EFI_SUCCESS;
            }

            Status = Tpm2GetCapabilityMaxCommandResponseSize (&MaxCommandSize, &MaxResponseSize);
            if(EFI_ERROR(Status) && !IsPTP())
            {
                MaxCommandSize = 0x800;
                MaxResponseSize = 0x800;
                Status = EFI_SUCCESS;
            }

            if(MaxCommandSize == 0 || MaxResponseSize == 0)
            {
                if(IsPTP())
                {
                    MaxCommandSize = dCrbAccessRegPtr->TpmCrbCtrlCmdSize;
                    MaxResponseSize = dCrbAccessRegPtr->TpmCrbCtrlRespSize;
                    ManufactureID = dCrbAccessRegPtr->TpmCrbIntfId[1];
                }
            }

        }

        if( TCG2_PROTOCOL_SPEC_TCG_1_2 == Tcg2SpecVersion )
        {
            DEBUG(( DEBUG_INFO, "TCG2_PROTOCOL_SPEC_TCG_1_2\n"));
            if(ProtocolCapability->Size < sizeof(TREE_BOOT_SERVICE_CAPABILITY_SHA1) )
            {
                ProtocolCapability->Size = sizeof(TREE_BOOT_SERVICE_CAPABILITY_SHA1);
                Status = EFI_BUFFER_TOO_SMALL;
            }
        }
        else if( TCG2_PROTOCOL_SPEC_TCG_2 == Tcg2SpecVersion )
        {
            DEBUG(( DEBUG_INFO, "TCG2_PROTOCOL_SPEC_TCG_2\n"));
            if(ProtocolCapability->Size < sizeof(TREE_BOOT_SERVICE_CAPABILITY))
            {
                ProtocolCapability->Size = sizeof(TREE_BOOT_SERVICE_CAPABILITY);
                Fillsize = TRUE;
            }
        }
        else
        {
            Status = EFI_UNSUPPORTED;
            ASSERT_EFI_ERROR(Status);
        }


        DEBUG(( DEBUG_INFO, "SpecVersion Status = %r \n",Status ));

        if( !EFI_ERROR(Status) || Fillsize == TRUE)
        {
            Status = Tpm2GetCapabilitySupportedAlg(&AlgList);
            Capability.HashAlgorithmBitmap = TREE_BOOT_HASH_ALG_SHA1;
            if(!EFI_ERROR(Status))
            {
                for(; i<AlgList.count; i++)
                {
                    if(AlgList.algProperties[i].alg == TPM2_ALG_SHA1 )
                    {
                        Capability.HashAlgorithmBitmap |= TREE_BOOT_HASH_ALG_SHA1;
                    }

                    if(AlgList.algProperties[i].alg == TPM2_ALG_SHA256)
                    {
                        Capability.HashAlgorithmBitmap |= TREE_BOOT_HASH_ALG_SHA256;
                    }

                    if(AlgList.algProperties[i].alg == TPM2_ALG_SHA384)
                    {
                        Capability.HashAlgorithmBitmap |= TREE_BOOT_HASH_ALG_SHA384;
                    }

                    if(AlgList.algProperties[i].alg == TPM2_ALG_SHA512)
                    {
                        Capability.HashAlgorithmBitmap |= TREE_BOOT_HASH_ALG_SHA512;
                    }
                    // Add for SM3 Support
                    if(AlgList.algProperties[i].alg == TPM2_ALG_SM3_256)
                    {
                        Capability.HashAlgorithmBitmap |= TREE_BOOT_HASH_ALG_SM3;
                    }
                }
            }
            else
            {
                Status = EFI_SUCCESS;
            }

            if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2 )
            {
                Capability.SupportedEventLogs  = TREE_EVENT_LOG_FORMAT_TCG_1_2;
                Capability.StructureVersion.Major = 1;
                Capability.ProtocolVersion.Major = 1;
                Capability.ProtocolVersion.Minor = 0;  // For BackCompabile Win8.1/Win10
                Capability.StructureVersion.Minor = 0; // For BackCompabile Win8.1/Win10
                Capability.Size = sizeof(TREE_BOOT_SERVICE_CAPABILITY_SHA1);
            }
            else if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
            {

                DEBUG(( DEBUG_INFO, "TreeGetCap ActiveBankBitMap: %x \n", ActiveBankBitMap));

                if((ActiveBankBitMap & TREE_BOOT_HASH_ALG_SHA1) != 0)
                {
                    Capability.SupportedEventLogs  = TREE_EVENT_LOG_FORMAT_TCG_1_2 | TREE_EVENT_LOG_FORMAT_TCG_2;
                }
                else
                {
                    Capability.SupportedEventLogs  =  TREE_EVENT_LOG_FORMAT_TCG_2 ;
                }

                DEBUG(( DEBUG_INFO, "TreeGetCap SupportedEventLogs: %x \n", Capability.SupportedEventLogs));

                Capability.StructureVersion.Major = 1;
                Capability.ProtocolVersion.Major = 1;
                Capability.ProtocolVersion.Minor = 1;
                Capability.StructureVersion.Minor = 1;
                Capability.ActivePcrBanks = ActiveBankBitMap;  // Report for the Actual BitMap.
                Capability.NumberOfPcrBanks = gNumberOfPcrBanks;   // Report for Maximun Number of PCR banks.
                Capability.Size = sizeof(TREE_BOOT_SERVICE_CAPABILITY);
            }
            Capability.TrEEPresentFlag     = TRUE;
            Capability.MaxCommandSize      = MaxCommandSize;
            Capability.MaxResponseSize     = MaxResponseSize;
            Capability.ManufacturerID      = ManufactureID;

            if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
            {
                if(Size >= sizeof(TREE_BOOT_SERVICE_CAPABILITY_SHA1))
                {
                    if(Size<sizeof(TREE_BOOT_SERVICE_CAPABILITY))
                    {
                        CopyMem (ProtocolCapability, &Capability, sizeof(TREE_BOOT_SERVICE_CAPABILITY_SHA1));
                        ProtocolCapability->StructureVersion.Major = 1;
                        ProtocolCapability->ProtocolVersion.Major = 1;
                        ProtocolCapability->ProtocolVersion.Minor = 0;  // For BackCompabile Win8.1/Win10
                        ProtocolCapability->StructureVersion.Minor = 0; // For BackCompabile Win8.1/Win10
                        ProtocolCapability->Size = sizeof(TREE_BOOT_SERVICE_CAPABILITY_SHA1);
                    }
                    else   
                    {
                        CopyMem (ProtocolCapability, &Capability, sizeof(TREE_BOOT_SERVICE_CAPABILITY));
                    }

                    Status = EFI_SUCCESS;
                }
                else
                {
                    Status = EFI_BUFFER_TOO_SMALL;
                }
            }
            else
            {
                CopyMem (ProtocolCapability, &Capability, sizeof(TREE_BOOT_SERVICE_CAPABILITY_SHA1));
            }

            DEBUG(( DEBUG_INFO, "TreeGetCap ProtocolCapability:\n"));
            //printbuffer( (UINT8*)ProtocolCapability,ProtocolCapability->Size );
        }
    }

    if(EFI_ERROR(Status)){
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TCG_PROTOCOL_GET_CAPABILITY_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
    }

    return Status;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TreeGetEventLog
//
// Description: Get TPM 20 Event log
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
EFIAPI
TreeGetEventLog (
    IN  EFI_TREE_PROTOCOL     *This,
    IN  TREE_EVENTLOGTYPE     EventLogFormat,
    OUT EFI_PHYSICAL_ADDRESS  *EventLogLocation,
    OUT EFI_PHYSICAL_ADDRESS  *EventLogLastEntry,
    OUT BOOLEAN               *EventLogTruncated
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8    Tcg2SpecVersion = GetTcgSpecVersion();
    UINTN    _LastEvtPtr;

    DEBUG(( DEBUG_INFO, "Tcg2SpecVersion = %x\n", Tcg2SpecVersion));
    DEBUG(( DEBUG_INFO, "EventLogFormat = %x\n", EventLogFormat));
    if(This == NULL || EventLogLocation == NULL || EventLogTruncated == NULL){
    	return EFI_INVALID_PARAMETER;
    }

    if(Tcg2SpecVersion != TCG2_PROTOCOL_SPEC_TCG_1_2 && EventLogFormat == TREE_EVENT_LOG_FORMAT_TCG_1_2 )
    {

        if((ActiveBankBitMap & TREE_BOOT_HASH_ALG_SHA1) == 0 )
        {
            DEBUG(( DEBUG_INFO, "ActiveBankBitMap & TREE_BOOT_HASH_ALG_SHA1) != 0\n"));
            Status = EFI_INVALID_PARAMETER;
            goto Done;
        }

        _LastEvtPtr = TransferTcgEvent2ToTcpaEvent();
        if( 0 == _LastEvtPtr )
            _LastEvtPtr = (UINTN)TreeExtraTCPASha1LogLoc;

        *EventLogLocation  = TreeExtraTCPASha1LogLoc;
        *EventLogLastEntry = _LastEvtPtr;
        *EventLogTruncated = IEventLogTruncated;

        goto Done;
    }

    if(Tcg2SpecVersion != TCG2_PROTOCOL_SPEC_TCG_2 && EventLogFormat == TREE_EVENT_LOG_FORMAT_TCG_2 )
    {
        Status = EFI_INVALID_PARAMETER;
        goto Done;

    }

    *EventLogLocation  = TreeEventLogLocation;
    *EventLogLastEntry = LastEventPtr;
    *EventLogTruncated = IEventLogTruncated;

    if(FinalsLastEntry==0)
    {
        FinalsLastEntry = (pEfiTcg2FinalEventsTbl + (sizeof(UINT64)*2));
    }

Done:
    DEBUG(( DEBUG_INFO, "EventLogLocation = %x\n", *EventLogLocation));
    DEBUG(( DEBUG_INFO, "EventLogLastEntry = %x\n", *EventLogLastEntry));
    DEBUG(( DEBUG_INFO, "EventLogTruncated = %x\n", *EventLogTruncated));

    DEBUG(( DEBUG_INFO, "TreeEventLogLocation = %x\n", TreeEventLogLocation));
    DEBUG(( DEBUG_INFO, "TreeExtraTCPASha1LogLoc = %x\n", TreeExtraTCPASha1LogLoc));
    DEBUG(( DEBUG_INFO, "LastEventPtr = %x\n", LastEventPtr));
    DEBUG(( DEBUG_INFO, "EventLogTruncated = %x\n", IEventLogTruncated));

    DEBUG(( DEBUG_INFO, "EventLogFormat = %x\n", EventLogFormat));
    return Status;
}

EFI_STATUS
EFIAPI
InternalTcg20CommonExtend(
    IN  EFI_TREE_PROTOCOL  *TrEEProtocol,
    IN  TPM_PCRINDEX PcrIndex,
    IN  TPML_DIGEST_VALUES  *Digest)
{
    TPM2_PCRExtend_cmd_t  Cmd;
    TPM2_PCRExtend_res_t  Res;
    TPM2_PCRExtend_res_t  Tmpres;
    UINT32                CmdSize;
    UINT8                 *Buffer;
    UINT8                 *ResultBuf = NULL;
    UINT32                ResultBufSize = 0;
    UINT32                DigestSize=0;
    UINT32                SessionInfoSize;
    EFI_STATUS            Status;
    UINTN                 i=0;

    Cmd.Tag          = (TPMI_ST_COMMAND_TAG)TPM_H2NS(TPM_ST_SESSIONS);
    Cmd.CommandCode  = TPM_H2NL(TPM_CC_PCR_Extend);
    Cmd.PcrHandle    = TPM_H2NL(PcrIndex);

    Buffer = (UINT8 *)&Cmd.AuthSessionPcr;

    SessionInfoSize = CopyAuthSessionCommand (NULL, Buffer);
    Buffer += SessionInfoSize;
    Cmd.AuthorizationSize = SwapBytes32(SessionInfoSize);

    DEBUG ((DEBUG_INFO, " InternalTcg20CommonExtend Cmd = %x \n", &Cmd));

    //Digest count
    *(UINT32 *)Buffer = TPM_H2NL(Digest->count);
    Buffer += sizeof(UINT32);

    DEBUG ((DEBUG_INFO, " Digest->count = %x \n", Digest->count));

    for(i=0; i<Digest->count; i++)
    {

        *(UINT16 *)Buffer = TPM_H2NS(Digest->digests[i].hashAlg);
        Buffer += sizeof(UINT16);

        DEBUG ((DEBUG_INFO, " Digest->digests[i].hashAlg = %x \n", Digest->digests[i].hashAlg));
        switch(Digest->digests[i].hashAlg)
        {
            case TPM2_ALG_SHA1:
                DigestSize = SHA1_DIGEST_SIZE;
                break;
            case TPM2_ALG_SHA256:
                DigestSize = SHA256_DIGEST_SIZE;
                break;
            case TPM2_ALG_SHA384:
                DigestSize = SHA384_DIGEST_SIZE;
                break;
            case TPM2_ALG_SHA512:
                DigestSize = SHA512_DIGEST_SIZE;
                break;
            case TPM2_ALG_SM3_256:
                DigestSize = SM3_256_DIGEST_SIZE;
                break;
            default:
                break;
        }

        TcgCommonCopyMem(NULL, Buffer, &Digest->digests[i].digest, DigestSize);
        Buffer += DigestSize;
    }

    DEBUG ((DEBUG_INFO, " InternalTcg20CommonExtend DigestSize = %x \n", DigestSize));

    CmdSize = (UINT32)(Buffer - (UINTN)&Cmd);
    Cmd.CommandSize = TPM_H2NL(CmdSize);

    ResultBuf     = (UINT8 *) &Tmpres;
    ResultBufSize = sizeof(Res);

    DEBUG ((DEBUG_INFO, "InternalTcg20CommonExtend CmdSize = %x \n", CmdSize));
    //printbuffer((UINT8 *)&Cmd, CmdSize);

    Status  = TrEEProtocol->SubmitCommand(TrEEProtocol,CmdSize, (UINT8 *)&Cmd, ResultBufSize, ResultBuf);

    //printbuffer(ResultBuf, 0x13);

    return Status;
}


EFI_STATUS
EFIAPI
TpmHashLogExtendEventI(
    IN  EFI_TREE_PROTOCOL         *This,
    IN  UINT8                     *DataToHash,
    IN  UINT64                    Flags,
    IN  UINTN                     DataSize,
    IN  OUT  TCG_PCR_EVENT2_HDR   *NewEventHdr,
    IN      UINT8                 *NewEventData
)
{
    EFI_STATUS                Status;
    static UINT32             Tcg2SpecVersion = 0xFFFFFFFF;
    UINTN                    TempSize=0;
    UINTN                     RequiredSpace=0;
    TCG_PLATFORM_SETUP_PROTOCOL     *PolicyInstance;
    TCG_PCClient_PCR_Event2_Hdr       EventData;
    static UINT32                 PcrBanks = 0;
    UINT32                        Count=0;
    UINT8                         i=0;
    UINTN                         HashSize=0;
    UINTN                         FinalsDataLen=0;
    //Test
    static UINT8                  Events=0;


    DEBUG(( DEBUG_INFO," TpmHashLogExtendEvent Entry \n"));
#if defined LOG_EV_EFI_ACTION && LOG_EV_EFI_ACTION == 0
    if(NewEventHdr->EventType == EV_EFI_ACTION && NewEventHdr->PCRIndex != 7)
    {
        return EFI_SUCCESS;
    }
#endif

    if(Tcg2SpecVersion == 0xFFFFFFFF)
    {
        Status = gBS->LocateProtocol (&gTcgPlatformSetupPolicyGuid,  NULL, &PolicyInstance);
        if (EFI_ERROR (Status))
        {
            Tcg2SpecVersion = TCG2_PROTOCOL_SPEC_TCG_1_2;
        }
        else
        {
            Tcg2SpecVersion = PolicyInstance->ConfigFlags.Tcg2SpecVersion;
            PcrBanks = PolicyInstance->ConfigFlags.PcrBanks;
        }
    }

    if( Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        PcrBanks = 1;
    }

    DEBUG(( DEBUG_INFO," Tcg2SpecVersion = %x \n", Tcg2SpecVersion));
    DEBUG(( DEBUG_INFO," PcrBanks = %x \n", PcrBanks));

    //HashValue
    if(DataToHash != NULL && DataSize != 0)
    {
        GetAllDigestValues(PcrBanks, &NewEventHdr->Digests, DataToHash, DataSize, &HashSize);
    }
    else
    {
        if((PcrBanks & 1) == 1)
        {
            HashSize += SHA1_DIGEST_SIZE;
            Count+=1;
        }

        if((PcrBanks & 2) == 2)
        {
            //sha256
            HashSize += SHA256_DIGEST_SIZE;
            Count+=1;
        }

        if((PcrBanks & 4) == 4)
        {
            //sha384
            HashSize += SHA384_DIGEST_SIZE;
            Count+=1;
        }

        if((PcrBanks & 8) == 8)
        {
            //sha512
            HashSize += SHA512_DIGEST_SIZE;
            Count+=1;
        }

        if((PcrBanks & 0x10) == 0x10)
        {
            //Special case //SM3 use hash sequence
            HashSize += SM3_256_DIGEST_SIZE;
            Count+=1;
        }

        NewEventHdr->Digests.count = Count;
    }

    DEBUG(( DEBUG_INFO," HashSize = %x \n", HashSize));
    DEBUG(( DEBUG_INFO," NewEventHdr->Digests.count = %x \n",NewEventHdr->Digests.count));
    //printbuffer((UINT8 *)&NewEventHdr->Digests, HashSize);
    RequiredSpace = sizeof(TCG_PCClient_PCR_Event2_Hdr) - sizeof(TPML_DIGEST_VALUES)\
                    + sizeof(UINT32) + NewEventHdr->EventSize;

    RequiredSpace += (HashSize + NewEventHdr->Digests.count * sizeof(UINT16));

    if((RequiredSpace + LastEntry) > \
            (TreeEventLogLocation  + MAX_LOG_AREA_SIZE))
    {
        IEventLogTruncated = TRUE;
    }
    
    if(NewEventHdr->EventType == EV_NO_ACTION)
    {
        i=0;
        if((PcrBanks & 1) == 1)
        {
            //sha1
            SetMem((UINT8 *)&NewEventHdr->Digests.digests[i].digest.sha1, SHA1_DIGEST_SIZE, 0);
            i+=1;
        }

        if((PcrBanks & 2) == 2)
        {
            //sha256
            SetMem((UINT8 *)&NewEventHdr->Digests.digests[i].digest.sha256, SHA256_DIGEST_SIZE, 0);
            i+=1;
        }

        if((PcrBanks & 4) == 4)
        {
            //sha384
            SetMem((UINT8 *)&NewEventHdr->Digests.digests[i].digest.sha384, SHA384_DIGEST_SIZE, 0);
            i+=1;
        }

        if((PcrBanks & 8) == 8)
        {
            //sha512
            SetMem((UINT8 *)&NewEventHdr->Digests.digests[i].digest.sha512, SHA512_DIGEST_SIZE, 0);
            i+=1;
        }

        if((PcrBanks & 0x10) == 0x10)
        {
            //SM3
            SetMem((UINT8 *)&NewEventHdr->Digests.digests[i].digest.sm3_256, SM3_256_DIGEST_SIZE, 0);
            i+=1;
        }
    }
    
    DEBUG(( DEBUG_INFO," InternalTcg20CommonExtend entry \n"));
    DEBUG(( DEBUG_INFO," NewEventHdr->Digests.count = %x  \n", NewEventHdr->Digests.count));

    if( NewEventHdr->EventType != EV_NO_ACTION)
    {
        Status = InternalTcg20CommonExtend(This,
                                           NewEventHdr->PCRIndex,
                                           &NewEventHdr->Digests);
        DEBUG(( DEBUG_INFO," InternalTcg20CommonExtend Status=%r \n",Status));
    }

    DEBUG(( DEBUG_INFO," InternalTcg20CommonExtend exit \n"));

    if(IEventLogTruncated)return EFI_VOLUME_FULL;
    if( ((Flags & TREE_EXTEND_ONLY) ==  TREE_EXTEND_ONLY) && NewEventHdr->EventType != EV_NO_ACTION)
    {
        // Return InternalTcg20CommonExtend Status
        DEBUG(( DEBUG_INFO," EFI_TCG2_EXTEND_ONLY on TCG2_HASH_LOG_EXTEND_EVENT(...)  \n"));
        return Status;
    }

    DEBUG(( DEBUG_INFO," LastEntry = %x \n", LastEntry));
    if(LastEntry == 0) return EFI_ABORTED;

    LastEventPtr = LastEntry;

    EventData.pcrIndex = NewEventHdr->PCRIndex;
    EventData.eventType = NewEventHdr->EventType;

    CopyMem((VOID*)(UINTN)LastEntry , NewEventHdr,
            sizeof(TCG_PCRINDEX) +  sizeof(TCG_EVENTTYPE));

    TempSize =  sizeof(TCG_PCRINDEX) +  sizeof(TCG_EVENTTYPE);

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)  //log event record
    {
        gBS->CopyMem(
            (VOID*)(UINTN)(LastEntry + TempSize) ,
            (UINT8 *)&NewEventHdr->Digests.digests[0].digest.sha1,
            SHA1_DIGEST_SIZE);

        TempSize+= (SHA1_DIGEST_SIZE);
        EventData.digests.count = 1;

    }
    else if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {

        CopyMem((VOID*)(UINTN)(LastEntry + TempSize), &NewEventHdr->Digests.count, sizeof(UINT32));
        TempSize+= sizeof(UINT32);
        i = 0;	// i variable initialize

        if( PcrBanks & TREE_BOOT_HASH_ALG_SHA1 )
        {
            //sha1
            CopyMem((VOID*)(UINTN)(LastEntry  + TempSize) ,
                    (UINT8 *)&NewEventHdr->Digests.digests[i],
                    (SHA1_DIGEST_SIZE + sizeof(TPM2_ALG_ID)));

            TempSize+= ((SHA1_DIGEST_SIZE) + sizeof(TPM2_ALG_ID));
            i+=1;
            FinalsDataLen+=((SHA1_DIGEST_SIZE) + sizeof(TPM2_ALG_ID));
        }

        if( PcrBanks & TREE_BOOT_HASH_ALG_SHA256 )
        {
            //sha256
            CopyMem((VOID*)(UINTN)(LastEntry  + TempSize) ,
                    (UINT8 *)&NewEventHdr->Digests.digests[i],
                    (SHA256_DIGEST_SIZE + sizeof(TPM2_ALG_ID)));

            TempSize+= ((SHA256_DIGEST_SIZE) + sizeof(TPM2_ALG_ID));
            FinalsDataLen+= ((SHA256_DIGEST_SIZE) + sizeof(TPM2_ALG_ID));
            i+=1;
        }

        if(PcrBanks & 4)
        {
            //sha256
            CopyMem((VOID*)(UINTN)(LastEntry  + TempSize) ,
                    (UINT8 *)&NewEventHdr->Digests.digests[i],
                    (SHA384_DIGEST_SIZE + sizeof(TPM2_ALG_ID)));

            TempSize+= ((SHA384_DIGEST_SIZE) + sizeof(TPM2_ALG_ID));
            FinalsDataLen+= ((SHA384_DIGEST_SIZE) + sizeof(TPM2_ALG_ID));
            i+=1;
        }

        if(PcrBanks & 8)
        {
            //sha512
            CopyMem((VOID*)(UINTN)(LastEntry + TempSize) ,
                    (UINT8 *)&NewEventHdr->Digests.digests[i],
                    (SHA512_DIGEST_SIZE + sizeof(TPM2_ALG_ID)));

            TempSize+= ((SHA512_DIGEST_SIZE) + sizeof(TPM2_ALG_ID));
            FinalsDataLen+= ((SHA512_DIGEST_SIZE) + sizeof(TPM2_ALG_ID));
            i+=1;
        }


        if(PcrBanks & 0x10)
        {
            CopyMem((VOID*)(UINTN)(LastEntry + TempSize) ,
                    (UINT8 *)&NewEventHdr->Digests.digests[i],
                    (SM3_256_DIGEST_SIZE + sizeof(TPM2_ALG_ID)));

            TempSize+= ((SM3_256_DIGEST_SIZE) + sizeof(TPM2_ALG_ID));
            FinalsDataLen+= ((SM3_256_DIGEST_SIZE) + sizeof(TPM2_ALG_ID));
        }

    }

    gBS->CopyMem(
        (VOID*)(UINTN)(LastEntry + TempSize),
        (UINT8 *)&NewEventHdr->EventSize,
        sizeof(UINT32));

    TempSize+=sizeof(UINT32);

    DEBUG((DEBUG_INFO, "TempSize = %x \n", TempSize));
    CopyMem((VOID*)(UINTN)(LastEntry + TempSize) ,
            NewEventData,
            NewEventHdr->EventSize);

    DEBUG((DEBUG_INFO, "NewEventHdr->EventType = %x \n", NewEventHdr->EventType));

    LastEntry = LastEventPtr + ((EFI_PHYSICAL_ADDRESS)(UINTN)(NewEventHdr->EventSize \
                                + TempSize));

    Events+=1;
    DEBUG((DEBUG_INFO, "Events Count = %x \n", Events));

    if(FinalsLastEntry != 0)
    {
        FinalsDataLen +=(sizeof(TCG_PCRINDEX) +  sizeof(TCG_EVENTTYPE)+ sizeof(UINT64));
        FinalsDataLen += NewEventHdr->EventSize;

        CopyMem((VOID*)(UINTN)FinalsLastEntry , (VOID*)(UINTN)LastEventPtr, FinalsDataLen);
        FinalsLastEntry+=FinalsDataLen;
        ((EFI_TCG2_FINAL_EVENTS_TABLE *)pEfiTcg2FinalEventsTbl)->NumberOfEvents+=1;
    }

    return EFI_SUCCESS;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TreeHashLogExtendEvent
//
// Description: Hash, log and Extend a TPM 20 Event
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
#pragma optimize ("",off)
EFI_STATUS
EFIAPI
TreeHashLogExtendEvent (
    IN  EFI_TREE_PROTOCOL     *This,
    IN  UINT64                Flags,
    IN  EFI_PHYSICAL_ADDRESS  DataToHash,
    IN  UINT64                DataToHashLen,
    IN  TrEE_EVENT            *TreeEvent
)
{
    EFI_STATUS            Status     = EFI_SUCCESS;
    TCG_PCR_EVENT2_HDR     TcgEvent;

    DEBUG(( DEBUG_INFO," TreeHashLogExtendEvent entry \n"));
    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_PROTOCOL_HASH_LOG_EXTEND_REQUEST | EFI_SOFTWARE_DXE_BS_DRIVER);
    if((This == NULL ) || (DataToHash == 0) || (TreeEvent == NULL))
    {
        Status = EFI_INVALID_PARAMETER;
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TCG_PROTOCOL_HASH_LOG_EXTEND_EVENT_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
        goto Exit;
    }
    else if(TreeEvent->Size < (TreeEvent->Header.HeaderSize + sizeof(UINT32))\
            || (TreeEvent->Header.PCRIndex > 23))
    {
        Status = EFI_INVALID_PARAMETER;
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TCG_PROTOCOL_HASH_LOG_EXTEND_EVENT_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
        goto Exit;
    }

    if(TreeEvent->Header.HeaderVersion != 1) {
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TCG_PROTOCOL_HASH_LOG_EXTEND_EVENT_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
        return EFI_INVALID_PARAMETER;
    }

    if( PE_COFF_IMAGE == Flags )
    {
        //Current, we did not support the PE_COFF_IMAGE on TCG EFI Protocol.
        //load image will measure UEFI images.
        return EFI_UNSUPPORTED;
    }

    TcgEvent.PCRIndex  = TreeEvent->Header.PCRIndex;
    TcgEvent.EventType = TreeEvent->Header.EventType;
    TcgEvent.EventSize = TreeEvent->Size - sizeof(TrEE_EVENT_HEADER) \
                         -sizeof(UINT32);

    DEBUG(( DEBUG_INFO," DataToHash = %x \n", DataToHash));
    DEBUG(( DEBUG_INFO," DataToHashLen = %x \n", DataToHashLen));
    DEBUG(( DEBUG_INFO," TreeEvent->Header.EventType = %x \n", TreeEvent->Header.EventType));
    DEBUG(( DEBUG_INFO," TcgEvent.EventType = %x \n",  TcgEvent.EventType));
    DEBUG(( DEBUG_INFO," TcgEvent.PCRIndex = %x \n", TcgEvent.PCRIndex));
    DEBUG(( DEBUG_INFO," TreeEvent->Header.PCRIndex = %x \n",  TreeEvent->Header.PCRIndex));
    DEBUG(( DEBUG_INFO," TcgEvent.EventSize = %x \n",  TcgEvent.EventSize));
    DEBUG(( DEBUG_INFO," TreeEvent->Size = %x \n",  TreeEvent->Size));


    Status = TpmHashLogExtendEventI(This,
                                    (UINT8 *)(UINTN)DataToHash,
                                    Flags,
                                    (UINTN)  DataToHashLen,
                                    &TcgEvent,
                                    (UINT8 *)&TreeEvent->Event
                                   );

Exit:
    DEBUG(( DEBUG_INFO," TreeHashLogExtendEvent exit\n"));
    return Status;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TreeSubmitCommand
//
// Description: Submit TPM 20 Command
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
#pragma optimize("",off)
EFI_STATUS
EFIAPI
TreeSubmitCommand (
    IN  EFI_TREE_PROTOCOL   *This,
    IN  UINT32              InputParameterBlockSize,
    IN  UINT8               *InputParameterBlock,
    IN  UINT32              OutputParameterBlockSize,
    IN  UINT8               *OutputParameterBlock
)
{
    EFI_STATUS            Status     = EFI_SUCCESS;
    UINT32                Size = 0;
    TPM_TRANSMIT_BUFFER   InBuffer[1], OutBuffer[1];
    TPM_1_2_REGISTERS_PTR     TpmReg = (TPM_1_2_REGISTERS_PTR)(UINTN)PORT_TPM_IOMEMBASE;

    if (This == NULL || InputParameterBlock == NULL || OutputParameterBlock == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }

    if(TpmSupport->InterfaceType == 1)
    {

        TisRequestLocality ( TpmReg );

        InBuffer[0].Buffer  = InputParameterBlock;
        InBuffer[0].Size    = InputParameterBlockSize;
        OutBuffer[0].Buffer = OutputParameterBlock;
        OutBuffer[0].Size   = OutputParameterBlockSize;

        Status = TpmLibPassThrough(TpmReg,sizeof (InBuffer) / sizeof (*InBuffer),
                          InBuffer,sizeof (OutBuffer) / sizeof (*OutBuffer),
                          OutBuffer);

        if(EFI_ERROR(Status)){
            TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_COMMUNICATION_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
        }

        TisReleaseLocality ( TpmReg );

    }
    else
    {

        Size = OutputParameterBlockSize;
        Status = CrbSubmitCmd(InputParameterBlock,
                              InputParameterBlockSize,
                              OutputParameterBlock,
                              &Size);

        if(EFI_ERROR(Status)){
            TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_COMMUNICATION_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
        }

    }

    return Status;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   CopyTcgLog
//
// Description: Copy the Tcg log from Pei Ho into Memory
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
static EFI_STATUS 
EFIAPI
CopyTcgLog(
    void )
{
    TCG_LOG_HOB     *TcgLog = NULL;
    void**          DummyPtr;

    DEBUG(( DEBUG_INFO," CopyTcgLog Entry \n"));

    TcgLog = (TCG_LOG_HOB*)                   LocateATcgHob(
                 gST->NumberOfTableEntries,
                 gST->ConfigurationTable,
                 &gEfiTcgTreeLogHobGuid );

    DummyPtr = &TcgLog;

    if ( *DummyPtr == NULL )
    {
        DEBUG(( DEBUG_ERROR," gEfiPeiLogHobGuid Not found \n"));
        return EFI_NOT_FOUND;
    }

    TcgLog->TableMaxSize = MAX_LOG_AREA_SIZE;
    DEBUG(( DEBUG_INFO," TcgLog->TableMaxSize \n", TcgLog->TableMaxSize));

    gBS->CopyMem(
        (UINT8 *)(UINTN)TreeEventLogLocation,
        ((UINT8 *)TcgLog),
        sizeof(TCG_LOG_HOB)
    );

    gBS->CopyMem(
        (UINT8 *)(UINTN)TreeEventLogLocation,
        (((UINT8 *)TcgLog) + sizeof(TCG_LOG_HOB)),
        TcgLog->TableSize
    );

    LastEntry = TreeEventLogLocation  +  TcgLog->TableSize;
    DEBUG(( DEBUG_INFO," CopyTcgLog TreeEventLogLocation = %x \n", TreeEventLogLocation));
    DEBUG(( DEBUG_INFO," CopyTcgLog LastEntry = %x \n", LastEntry));

    //printbuffer((UINT8 *)(UINTN)TreeEventLogLocation, 0xA0);
    return EFI_SUCCESS;
}


EFI_TREE_PROTOCOL mTreeProtocol =
{
    TreeGetCapability,
    TreeGetEventLog,
    TreeHashLogExtendEvent,
    TreeSubmitCommand,
    EfiTreeGetActivePcrs,
    EfiTreeSetActivePcrs,
    EfiTreeGetResultOfSetActivePcrs
};

AMI_INTERNAL_HLXE_PROTOCOL  InternalLogProtocol =
{
    TpmHashLogExtendEventI,
    Tpm2AllocatePCR
};





//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TrEEUpdateTpmDeviceASL
//
// Description: Update TPM Asl tokens
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
VOID 
EFIAPI
TrEEUpdateTpmDeviceASL(
    IN EFI_EVENT ev,
    IN VOID      *ctx)
{
    ACPI_HDR                    *dsdt;
    EFI_PHYSICAL_ADDRESS        dsdtAddress=0;
    EFI_STATUS                  Status;
    UINT64                      Value;
    SETUP_DATA                  SetupDataBuffer;
    UINTN                       SetupVariableSize = sizeof(SETUP_DATA);
    UINT32                      SetupVariableAttributes=0;

    DEBUG(( DEBUG_INFO, "TrEEUpdateTpmDeviceASL Entry \n"));

    //locate AcpiProtocol
    Status = TcgLibGetDsdt(&dsdtAddress, EFI_ACPI_TABLE_VERSION_ALL);
    if (EFI_ERROR(Status))
    {
        DEBUG((DEBUG_INFO, "TrEEUpdateTpmDeviceASL::DSDT not found\n"));
        if(Status == EFI_NOT_AVAILABLE_YET)
        {
            //set callback
            DEBUG((DEBUG_ERROR, "Setting callback for TrEEUpdateTpmDeviceASL\n"));
            Status = gBS->CreateEventEx( EFI_EVENT_NOTIFY_SIGNAL,
                                       EFI_TPL_CALLBACK, TrEEUpdateTpmDeviceASL, NULL, &gEfiAcpiTableGuid, &Event );
            
            if(EFI_ERROR(Status))
            {
                DEBUG((DEBUG_ERROR, "Unable to create Event..Exit(1)\n"));
                return;
            }            
        }
        return;
    }

    DEBUG((DEBUG_INFO, "TrEEUpdateTpmDeviceASL::dsdtAddress %x \n", dsdtAddress));
    dsdt = (ACPI_HDR*)dsdtAddress;

    DEBUG((DEBUG_INFO, "dsdt->Signature =  %x \n", dsdt->Signature));

    // Update for TCM Device
    if( IsTcmSupportType() )
    {
        Value = 1;
        DEBUG(( DEBUG_INFO, "TrEEUpdateTpmDeviceASL::Set TCMF Device ID \n"));
        Status=TcgUpdateAslNameObject(dsdt, "TCMF", (UINT64)Value);
        if (EFI_ERROR(Status))
        {
            DEBUG((DEBUG_ERROR, "TrEEUpdateTpmDeviceASL::Failed set TCMF Device ID  %r \n", Status));
            return;
        }
    }

    if(!IsTpm20Device())
    {
        Value = 0;
    }
    else
    {
        Value = 1;
    }

    DEBUG((DEBUG_INFO, "TrEEUpdateTpmDeviceASL::Setting  TTDP to %x \n", Value));
    Status=TcgUpdateAslNameObject(dsdt, "TTDP", (UINT64)Value);
    if (EFI_ERROR(Status))
    {
        DEBUG((DEBUG_ERROR, "TrEEUpdateTpmDeviceASL::Failure setting ASL TTDP %r \n", Status));
        return;
    }

    if(isTpm20CrbPresent())
    {
        Value = 0;
    }
    else
    {
        Value = 1;
    }

    Status = gRT->GetVariable (L"Setup",
                                 &gSetupVariableGuid,
                                 &SetupVariableAttributes,
                                 &SetupVariableSize,
                                 &SetupDataBuffer);

    Status=TcgUpdateAslNameObject(dsdt, "PPIV", SetupDataBuffer.PpiSpecVersion);

    DEBUG((DEBUG_INFO, "TrEEUpdateTpmDeviceASL::Setting  TTPF to %x \n", Value));

    Status=TcgUpdateAslNameObject(dsdt, "TTPF", (UINT64)Value);
    if (EFI_ERROR(Status))
    {
        DEBUG((DEBUG_ERROR, "TrEEUpdateTpmDeviceASL::Failure setting ASL value %r \n", Status));
    }

    // Check the Event is been register, not the direct calling function. (NULL input)
    if( ev )
    {
        gBS->CloseEvent(ev);
    }
    return;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureCertificate
//
// Description: Measure a Certs
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
MeasureCertificate(UINTN sizeOfCertificate,
                   UINT8 *pterCertificate)
{
    EFI_STATUS                Status;
    TrEE_EVENT               *Tcg20Event = NULL;
    EFI_VARIABLE_DATA        *VarLog = NULL;
    UINTN                     i=0;
    UINTN                     VarNameLength;
    static BOOLEAN            initialized = 0;
    static TPM_DIGEST         digestTrackingArray[5];
    static TPM_DIGEST         zeroDigest;
    SHA1_CTX                  Sha1Ctx;
    TCG_DIGEST                Sha1Digest;
    UINT64                    Flags = 0;
    UINT32                    EventSize = 0;
    UINT8                     *EventDataPtr = NULL;

    if(TrEEProtocolInstance == NULL)
    {
        Status = gBS->LocateProtocol(&gEfiTrEEProtocolGuid, NULL, &TrEEProtocolInstance);
        if(EFI_ERROR(Status)){
            return EFI_NOT_FOUND;
        }
    }

    VarNameLength = StrLen(L"db");

    EventSize = (UINT32)( sizeof (*VarLog) + VarNameLength
                          * sizeof (CHAR16) + sizeOfCertificate) - 3;

    Status = gBS->AllocatePool(EfiBootServicesData, (sizeof(TrEE_EVENT_HEADER) + \
                                            sizeof(UINT32) + EventSize), &Tcg20Event);

    if(EFI_ERROR(Status) || Tcg20Event==NULL){
        return EFI_OUT_OF_RESOURCES;
    }

    if(!initialized)
    {
        for(i=0; i<5; i++)
        {
            gBS->SetMem(digestTrackingArray[i].digest,20, 0);
        }
        gBS->SetMem(zeroDigest.digest,20, 0);
        initialized = TRUE;
    }

    Tcg20Event->Size  = sizeof(TrEE_EVENT_HEADER) + sizeof(UINT32) + EventSize;
    Tcg20Event->Header.HeaderSize = sizeof(TrEE_EVENT_HEADER);
    Tcg20Event->Header.HeaderVersion = 1;
    Tcg20Event->Header.PCRIndex    = 7;
    Tcg20Event->Header.EventType   = 0x800000E0;

    Status = gBS->AllocatePool(EfiBootServicesData, EventSize, &VarLog);

    if ( VarLog == NULL )
    {
        gBS->FreePool(Tcg20Event);
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

    //before extending verify if we have already measured it.
    SHA1Init(&Sha1Ctx);

    SHA1Update(&Sha1Ctx,
               VarLog,
               (u32)EventSize);

    SHA1Final((unsigned char *)&Sha1Digest.digest, &Sha1Ctx);

    for(i=0; i<5; i++)
    {
        //tempDigest
        if(!CompareMem(digestTrackingArray[i].digest, &Sha1Digest, 20)){
            gBS->FreePool(Tcg20Event);
            gBS->FreePool( VarLog );
            return EFI_SUCCESS; //already measured
        }

        if(!CompareMem(digestTrackingArray[i].digest, zeroDigest.digest, 20))
            break; //we need to measure
    }

    if(i<5)
    {
        gBS->CopyMem(digestTrackingArray[i].digest, &Sha1Digest, 20);
    }

    EventDataPtr = (UINT8 *)Tcg20Event;

    EventDataPtr += sizeof(TrEE_EVENT_HEADER) + sizeof(UINT32);

    gBS->CopyMem(EventDataPtr, VarLog, EventSize);

    Status = TrEEProtocolInstance->HashLogExtendEvent(TrEEProtocolInstance,
             Flags, (EFI_PHYSICAL_ADDRESS)(UINT8 *)(UINTN)VarLog, (UINT64)EventSize,
             Tcg20Event);

    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_SECURE_BOOT_CERTIFICATE_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER);

    if( VarLog != NULL )
    {
        gBS->FreePool( VarLog );
    }
    if( Tcg20Event != NULL)
    {
        gBS->FreePool( Tcg20Event );
    }
    return Status;
}





//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   FindandMeasureSecureBootCertificate
//
// Description: Measure Secureboot Certs
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

EFI_STATUS FindandMeasureSecureBootCertificate()
{
    EFI_STATUS      Status;
    UINTN           VarSize  = 0;
    UINT8           *SecureDBBuffer = NULL;
    AMI_VALID_CERT_IN_SIG_DB    *CertInfo;
    UINT8           *CertOffsetPtr = NULL;

    VarSize = 0;

    Status   = gRT->GetVariable(L"db",
                                &gEfiImageSecurityDatabaseGuid,
                                NULL,
                                &VarSize,
                                NULL);

    if ( Status != EFI_BUFFER_TOO_SMALL )
    {
        return EFI_NOT_FOUND;
    }

    Status = gBS->AllocatePool(EfiBootServicesData, VarSize, &SecureDBBuffer);

    if ( SecureDBBuffer != NULL )
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
    EfiGetSystemConfigurationTable(&AmiValidBootImageCertTblGuid, &CertInfo );
    
    if(CertInfo == NULL)
	{
        if(SecureDBBuffer!=NULL)
        {
            gBS->FreePool( SecureDBBuffer  );
        }
        return EFI_NOT_FOUND;
    }

    if(CertInfo->SigLength == 0)
	{
        if(SecureDBBuffer!=NULL)
        {
           gBS->FreePool( SecureDBBuffer  );
        }
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
EFIAPI
MeasureTeImage (
    IN  EFI_IMAGE_LOAD_EVENT     *ImageLoad,
    IN  UINTN                     Tcg2SpecVersion,
    IN  UINT32                    PcrBanks,
    OUT TPM2_HALG                 *TpmDigest,
    OUT UINTN                     *HashCount)
{
    EFI_TE_IMAGE_HEADER               *ptrToTEHdr;
    EFI_IMAGE_SECTION_HEADER          *Section;
    UINT8                             *HashBase;
    UINTN                             HashSize;
    UINTN                             SumOfBytesHashed;
    UINTN                             SectionHeaderOffset;
    UINTN                             numOfSectionHeaders;
    UINTN                             Index;
    SHA1_CTX                          Sha1Ctx;
    SHA2_CTX                          Sha2Ctx;
    SHA384_CTX                        Sha384Ctx;
    SHA512_CTX                        Sha512Ctx;
    TPMI_DH_OBJECT                    HashHandle;
    UINTN                             hashContextSize;

    TPM2B_DIGEST                      Result;
    UINTN                             count=0;
    EFI_STATUS                        Status;
    UINTN                             TempSize=0;
    UINT32                            mPcrBanks=PcrBanks;
    AMI_DXE_HASH_INTERFACE_PROTOCOL     *HashInterface = NULL;

    // 2. Initialize a SHA hash context.
    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Init(&Sha1Ctx);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Init( &Sha1Ctx );
            count+=1;
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_init(&Sha2Ctx);
            count+=1;
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
        {
            sha384_init(&Sha384Ctx);
            count+=1;
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_init(&Sha512Ctx);
            count+=1;
        }

        if( mPcrBanks & TREE_BOOT_HASH_ALG_SM3)
        {
            Status = EFI_SUCCESS;
            if(TrEEProtocolInstance == NULL)
            {

                Status = gBS->LocateProtocol(
                             &gEfiTrEEProtocolGuid,
                             NULL,
                             &TrEEProtocolInstance);
            }

            if( !EFI_ERROR(Status) )
            {
                Status = gBS->LocateProtocol(
                             &gAmiDxeHashInterfaceguid,
                             NULL,
                             &HashInterface);
            }

            if( !EFI_ERROR(Status) )
            {

                hashContextSize = sizeof(TPMI_DH_OBJECT);
                Status = HashInterface->Init( TrEEProtocolInstance, TREE_BOOT_HASH_ALG_SM3, &HashHandle, &hashContextSize);

                count+=1;
            }

            if( EFI_ERROR(Status) )
            {
                mPcrBanks &=(~ TREE_BOOT_HASH_ALG_SM3);
            }
        }
    }

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

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Update(&Sha1Ctx,
                   HashBase,
                   (u32)HashSize);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
        {
            sha512_process( &Sha384Ctx, HashBase, (u32)HashSize );
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_process( &Sha512Ctx, HashBase, (u32)HashSize );
        }

        if( mPcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
        {
            Status = HashInterface->Update(
                                        TrEEProtocolInstance,
                                        HashBase,
                                        HashSize,
                                        TREE_BOOT_HASH_ALG_SM3,
                                        &HashHandle);
        }
    }


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

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
            {
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
                {
                    SHA1Update(&Sha1Ctx,
                               HashBase,
                               (u32)HashSize);
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
                {
                    sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
                {
                    sha512_process( &Sha384Ctx, HashBase, (u32)HashSize );
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
                {
                    sha512_process( &Sha512Ctx, HashBase, (u32)HashSize );
                }

                if( mPcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
                {
                    Status = HashInterface->Update(
                                                TrEEProtocolInstance,
                                                HashBase,
                                                HashSize,
                                                TREE_BOOT_HASH_ALG_SM3,
                                                &HashHandle);
                }
            }
        }

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

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
            {
                SHA1Update(&Sha1Ctx,
                           HashBase,
                           (u32)HashSize);
            }
            if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
            {
                sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
            }
            if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
            {
                sha512_process( &Sha384Ctx, HashBase, (u32)HashSize );
            }
            if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
            {
                sha512_process( &Sha512Ctx, HashBase, (u32)HashSize );
            }

            if( mPcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
            {
                Status = HashInterface->Update(
                                            TrEEProtocolInstance,
                                            HashBase,
                                            HashSize,
                                            TREE_BOOT_HASH_ALG_SM3,
                                            &HashHandle);
            }
        }


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


        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
            {
                SHA1Update(&Sha1Ctx,
                           HashBase,
                           (u32)HashSize);
            }
            if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
            {
                sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
            }
            if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
            {
                sha512_process( &Sha384Ctx, HashBase, (u32)HashSize );
            }
            if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
            {
                sha512_process( &Sha512Ctx, HashBase, (u32)HashSize );
            }

            if( mPcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
            {
                Status = HashInterface->Update(
                                            TrEEProtocolInstance,
                                            HashBase,
                                            HashSize,
                                            TREE_BOOT_HASH_ALG_SM3,
                                            &HashHandle);
            }
        }
    }


    //
    // Finalize the SHA hash.
    //
    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Final(TpmDigest->sha1, &Sha1Ctx);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Final(TpmDigest->sha1, &Sha1Ctx);
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_done( &Sha2Ctx, TpmDigest->sha256 );
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
        {
            sha384_done( &Sha384Ctx, TpmDigest->sha384 );
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_done( &Sha512Ctx, TpmDigest->sha512 );
        }

        if( mPcrBanks & TREE_BOOT_HASH_ALG_SM3)
        {

            UINTN   ResultBufferSize;
            ResultBufferSize = 64;
            Status = HashInterface->GetHashResults(
                                        TrEEProtocolInstance,
                                        &HashHandle,
                                        TREE_BOOT_HASH_ALG_SM3,
                                        Result.buffer,
                                        &ResultBufferSize
                                        );
            Result.size = (UINT16)ResultBufferSize;
            if (EFI_ERROR(Status))
            {
                return EFI_DEVICE_ERROR;
            }

            gBS->CopyMem(TpmDigest->sm3256, Result.buffer, Result.size);

            DEBUG((EFI_D_VERBOSE, "\n Tpm2SequenceComplete Success \n"));
        }
    }

    *HashCount = count;

    return EFI_SUCCESS;
}



EFI_STATUS
EFIAPI
AmiDxeHashInitInterface (
    IN  EFI_TREE_PROTOCOL   *TrEEProtocolInstance,
    IN  UINT32               Alg,
    OUT VOID                *Context,
    IN OUT UINTN            *ContextSize
)
{
    EFI_STATUS  Status = EFI_SUCCESS;

    if(TrEEProtocolInstance == NULL)return EFI_INVALID_PARAMETER;
    if( NULL == ContextSize || NULL == Context ) return EFI_INVALID_PARAMETER;
    if( 0 == *ContextSize ) return EFI_INVALID_PARAMETER;

    //can only do one algorithm at a time
    if(Alg & 1)
    {
        if( sizeof(SHA1_CTX) > *ContextSize )
        {
            *ContextSize = sizeof(SHA1_CTX);
            Status = EFI_BUFFER_TOO_SMALL;
        }
        SHA1Init( (SHA1_CTX*)Context );
    }
    else if(Alg & 2)
    {
        if( sizeof(SHA2_CTX) > *ContextSize )
        {
            *ContextSize = sizeof(SHA2_CTX);
            Status = EFI_BUFFER_TOO_SMALL;
        }
        sha256_init( (SHA2_CTX*)Context );
    }
    else if(Alg & 4)
    {
        if( sizeof(SHA384_CTX) > *ContextSize )
        {
            *ContextSize = sizeof(SHA384_CTX);
            Status = EFI_BUFFER_TOO_SMALL;
        }
        sha384_init( (SHA384_CTX*)Context);
    }
    else if(Alg & 8)
    {
        if( sizeof(SHA512_CTX) > *ContextSize )
        {
            *ContextSize = sizeof(SHA512_CTX);
            Status = EFI_BUFFER_TOO_SMALL;
        }
        sha512_init( (SHA512_CTX*)Context);
    }
    else if(Alg & 0x10)
    {
        if( sizeof(TPMI_DH_OBJECT) > *ContextSize )
        {
            *ContextSize = sizeof(TPMI_DH_OBJECT);
            Status = EFI_BUFFER_TOO_SMALL;
        }
        Status = Tpm2HashSequenceStart (TrEEProtocolInstance, TPM2_ALG_SM3_256, (TPMI_DH_OBJECT *)Context);
    }

    return Status;
}



EFI_STATUS
EFIAPI
AmiDxeHashUpdateInterface (
    IN  EFI_TREE_PROTOCOL  *TrEEProtocolInstance,
    IN  UINT8              *Data,
    IN  UINTN               Datasize,
    IN  UINT32              Alg,
    IN  OUT VOID           *Context
)
{
    TPM2B_MAX_BUFFER      HashBuffer;
    UINT8                 *Buffer;
    EFI_STATUS            Status;

    if(TrEEProtocolInstance == NULL)return EFI_INVALID_PARAMETER;
    if(Data == NULL)return EFI_INVALID_PARAMETER;
    if(Context == NULL) return EFI_INVALID_PARAMETER;

    //can only do one algorithm at a time
    if(Alg & 1)
    {
        SHA1Update( (SHA1_CTX *)Context, Data, (u32) Datasize );
    }
    else if(Alg & 2)
    {
        sha256_process((SHA2_CTX *)Context, Data, (u32)Datasize );
    }
    else if(Alg & 4)
    {
        sha512_process((SHA384_CTX *)Context, Data, (unsigned long)Datasize);
    }
    else if(Alg & 8)
    {
        sha512_process((SHA512_CTX *)Context, Data, (unsigned long)Datasize);
    }
    else if(Alg & 0x10)
    {
        Buffer = Data;
        while( Datasize )
        {
            HashBuffer.size = (UINT16)( ( Datasize >= MAX_DIGEST_BUFFER ) ? MAX_DIGEST_BUFFER : Datasize );
            CopyMem(HashBuffer.buffer, Buffer, HashBuffer.size );

            Buffer += HashBuffer.size;
            Datasize -= HashBuffer.size;

            Status = Tpm2SequenceUpdate(TrEEProtocolInstance,  *(TPMI_DH_OBJECT *)Context, &HashBuffer);
            DEBUG(( DEBUG_INFO," Tpm2SequenceUpdate results = %r \n", Status));
            if (EFI_ERROR(Status))
            {
                return EFI_DEVICE_ERROR;
            }
        }
    }

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
AmiDxeHashCompleteInterface (
    IN  EFI_TREE_PROTOCOL  *TrEEProtocolInstance,
    IN  VOID               *Context,
    IN  UINT32              Alg,
    IN  OUT  UINT8         *resultsbuffer,
    IN  OUT  UINTN         *resultsbuffersize
)
{
    TPM2B_MAX_BUFFER    HashBuffer;
    TPM2B_DIGEST        Result;
    EFI_STATUS          Status;

    if(resultsbuffer == NULL) return EFI_INVALID_PARAMETER;
    if(resultsbuffersize == NULL) return EFI_INVALID_PARAMETER;
    if(*resultsbuffersize == 0) return EFI_INVALID_PARAMETER;
    if(Context == NULL) return EFI_INVALID_PARAMETER;
    if(TrEEProtocolInstance == NULL) return EFI_INVALID_PARAMETER;

    if(Alg & 1)
    {
        if(*resultsbuffersize < 20)
        {
            *resultsbuffersize = 20;
            return EFI_BUFFER_TOO_SMALL;
        }
        SHA1Final( resultsbuffer, (SHA1_CTX *)Context );
        *resultsbuffersize = 20;
    }
    else if(Alg & 2)
    {
        if(*resultsbuffersize < 32)
        {
            *resultsbuffersize = 32;
            return EFI_BUFFER_TOO_SMALL;
        }
        sha256_done( (SHA2_CTX *)Context, (unsigned char *)resultsbuffer );
        *resultsbuffersize = 32;
    }
    else if(Alg & 4)
    {
        if(*resultsbuffersize < 48)
        {
            *resultsbuffersize = 48;
            return EFI_BUFFER_TOO_SMALL;
        }
        sha384_done((SHA384_CTX *)Context, (unsigned char *)resultsbuffer);
        *resultsbuffersize = 48;
    }
    else if(Alg & 8)
    {
        if(*resultsbuffersize < 64)
        {
            *resultsbuffersize = 64;
            return EFI_BUFFER_TOO_SMALL;
        }
        sha512_done((SHA512_CTX *)Context, (unsigned char *)resultsbuffer);
        *resultsbuffersize = 64;
    }
    else if(Alg & 0x10)
    {
        if(*resultsbuffersize < 32)
        {
            *resultsbuffersize = 32;
            return EFI_BUFFER_TOO_SMALL;
        }

        HashBuffer.size = 0;
        Status = Tpm2SequenceComplete (TrEEProtocolInstance,
                                       *(TPMI_DH_OBJECT *)Context,
                                       &HashBuffer,
                                       &Result);
        if (EFI_ERROR(Status))
        {
            return EFI_DEVICE_ERROR;
        }

        CopyMem (resultsbuffer, Result.buffer, Result.size);
        *resultsbuffersize = (UINTN)Result.size;
    }

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
AmiDxeHashAllInterface (
    IN  EFI_TREE_PROTOCOL        *TrEEProtocolInstance,
    IN  UINT8                    *DataToHash,
    IN  UINTN                    DataSize,
    IN  TPML_DIGEST_VALUES       *HashResults,
    IN  UINT32                   *AlgBitMap
)
{
    UINTN   HashSize = 0;
    return (GetAllDigestValues(*AlgBitMap, HashResults, DataToHash, DataSize, &HashSize));
}

AMI_DXE_HASH_INTERFACE_PROTOCOL AmiHashAllInterface =
{
    AmiDxeHashAllInterface,
    AmiDxeHashInitInterface,
    AmiDxeHashUpdateInterface,
    AmiDxeHashCompleteInterface
};




EFI_STATUS
EFIAPI
MeasurePeImage (
    IN  EFI_PHYSICAL_ADDRESS      ImageAddress,
    IN  UINTN                     ImageSize,
    IN  UINTN                     Tcg2SpecVersion,
    IN  UINT32                    PcrBanks,
    OUT TPM2_HALG                 *TpmDigest,
    OUT UINTN                     *HashCount
)
{
    EFI_STATUS                           Status;
    EFI_IMAGE_DOS_HEADER                 *DosHdr;
    UINT32                               PeCoffHeaderOffset;
    EFI_IMAGE_SECTION_HEADER             *Section;
    UINT8                                *HashBase;
    UINTN                                HashSize;
    UINTN                                SumOfBytesHashed;
    EFI_IMAGE_SECTION_HEADER             *SectionHeader;
    UINTN                                Index;
    UINTN                                Pos;
    UINT16                               Magic;
    EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION  Hdr;
    UINT32                               NumberOfRvaAndSizes;
    UINT32                               CertSize;
    SHA1_CTX                             Sha1Ctx;
    SHA2_CTX                             Sha2Ctx;
    SHA384_CTX                           Sha384Ctx;
    SHA512_CTX                           Sha512Ctx;
    TPMI_DH_OBJECT                       HashHandle;
    UINTN                               hashContextSize;

    TPM2B_DIGEST                        Result;
    UINTN                               TempSize=0;
    UINTN                               count=0;
    UINT32                              mPcrBanks=PcrBanks;
    AMI_DXE_HASH_INTERFACE_PROTOCOL     *HashInterface = NULL;


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
    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Init(&Sha1Ctx);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Init( &Sha1Ctx );
            count+=1;
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_init(&Sha2Ctx);
            count+=1;
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
        {
            sha384_init(&Sha384Ctx);
            count+=1;
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_init(&Sha512Ctx);
            count+=1;
        }

        if( mPcrBanks & TREE_BOOT_HASH_ALG_SM3)
        {
            Status = EFI_SUCCESS;
            if(TrEEProtocolInstance == NULL)
            {
                Status = gBS->LocateProtocol(
                             &gEfiTrEEProtocolGuid,
                             NULL,
                             &TrEEProtocolInstance);
            }

            if( !EFI_ERROR(Status) )
            {
                Status = gBS->LocateProtocol(
                             &gAmiDxeHashInterfaceguid,
                             NULL,
                             &HashInterface);
            }

            if( !EFI_ERROR(Status) )
            {
//                Status = Tpm2HashSequenceStart (TrEEProtocolInstance, TPM2_ALG_SM3_256, &HashHandle);
                hashContextSize = sizeof(TPMI_DH_OBJECT);
                Status = HashInterface->Init( TrEEProtocolInstance, TREE_BOOT_HASH_ALG_SM3, &HashHandle, &hashContextSize);
//                HashBuffer.size = 0;
                count+=1;
            }

            if( EFI_ERROR(Status) )
            {
                mPcrBanks &=(~ TREE_BOOT_HASH_ALG_SM3);
            }
        }
    };

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

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Update(&Sha1Ctx,
                   HashBase,
                   (u32)HashSize);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
        {
            sha512_process( &Sha384Ctx, HashBase, (u32)HashSize );
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_process( &Sha512Ctx, HashBase, (u32)HashSize );
        }

        if( mPcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
        {
            Status = HashInterface->Update(
                                        TrEEProtocolInstance,
                                        HashBase,
                                        HashSize,
                                        TREE_BOOT_HASH_ALG_SM3,
                                        &HashHandle);
        }
    }

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
            if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
            {
                SHA1Update(&Sha1Ctx,
                           HashBase,
                           (u32)HashSize);
            }

            if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
            {
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
                {
                    SHA1Update(&Sha1Ctx,
                               HashBase,
                               (u32)HashSize);
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
                {
                    sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
                {
                    sha512_process( &Sha384Ctx, HashBase, (u32)HashSize );
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
                {
                    sha512_process( &Sha512Ctx, HashBase, (u32)HashSize );
                }

                if( mPcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
                {
                    Status = HashInterface->Update(
                                                TrEEProtocolInstance,
                                                HashBase,
                                                HashSize,
                                                TREE_BOOT_HASH_ALG_SM3,
                                                &HashHandle);
                }
            }
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
            if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
            {
                SHA1Update(&Sha1Ctx,
                           HashBase,
                           (u32)HashSize);
            }

            if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
            {
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
                {
                    SHA1Update(&Sha1Ctx,
                               HashBase,
                               (u32)HashSize);
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
                {
                    sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
                {
                    sha512_process( &Sha384Ctx, HashBase, (u32)HashSize );
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
                {
                    sha512_process( &Sha512Ctx, HashBase, (u32)HashSize );
                }


                if( mPcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
                {
                    Status = HashInterface->Update(
                                                    TrEEProtocolInstance,
                                                    HashBase,
                                                    HashSize,
                                                    TREE_BOOT_HASH_ALG_SM3,
                                                    &HashHandle);
                }
            }
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
            if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
            {
                SHA1Update(&Sha1Ctx,
                           HashBase,
                           (u32)HashSize);
            }

            if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
            {
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
                {
                    SHA1Update(&Sha1Ctx,
                               HashBase,
                               (u32)HashSize);
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
                {
                    sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
                {
                    sha512_process( &Sha384Ctx, HashBase, (u32)HashSize );
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
                {
                    sha512_process( &Sha512Ctx, HashBase, (u32)HashSize );
                }

                if( mPcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
                {
                    Status = HashInterface->Update(
                                                    TrEEProtocolInstance,
                                                    HashBase,
                                                    HashSize,
                                                    TREE_BOOT_HASH_ALG_SM3,
                                                    &HashHandle);
                }
            }
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

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
            {
                SHA1Update(&Sha1Ctx,
                           HashBase,
                           (u32)HashSize);
            }
            if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
            {
                sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
            }
            if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
            {
                sha512_process( &Sha384Ctx, HashBase, (u32)HashSize );
            }
            if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
            {
                sha512_process( &Sha512Ctx, HashBase, (u32)HashSize );
            }

            if( mPcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
            {
                Status = HashInterface->Update(
                                                TrEEProtocolInstance,
                                                HashBase,
                                                HashSize,
                                                TREE_BOOT_HASH_ALG_SM3,
                                                &HashHandle);
            }
        }
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

            if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
            {
                SHA1Update(&Sha1Ctx,
                           HashBase,
                           (u32)HashSize);
            }

            if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
            {
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
                {
                    SHA1Update(&Sha1Ctx,
                               HashBase,
                               (u32)HashSize);
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
                {
                    sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
                {
                    sha512_process( &Sha384Ctx, HashBase, (u32)HashSize );
                }
                if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
                {
                    sha512_process( &Sha512Ctx, HashBase, (u32)HashSize );
                }

                if( mPcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
                {
                    Status = HashInterface->Update(
                                                TrEEProtocolInstance,
                                                HashBase,
                                                HashSize,
                                                TREE_BOOT_HASH_ALG_SM3,
                                                &HashHandle);
                }
            }
        }
        else if (ImageSize < CertSize + SumOfBytesHashed)
        {
            goto Finish;
        }
    }

    //
    // 17.  Finalize the SHA hash.
    //
    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Final( TpmDigest->sha1, &Sha1Ctx);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Final(TpmDigest->sha1, &Sha1Ctx);
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_done( &Sha2Ctx, TpmDigest->sha256 );
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
        {
            sha384_done( &Sha384Ctx, TpmDigest->sha384 );
        }
        if( mPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_done( &Sha512Ctx, TpmDigest->sha512 );
        }

        if( mPcrBanks & TREE_BOOT_HASH_ALG_SM3)
        {
            UINTN   ResultBufferSize;
            ResultBufferSize = 64;
            Status = HashInterface->GetHashResults(
                                        TrEEProtocolInstance,
                                        &HashHandle,
                                        TREE_BOOT_HASH_ALG_SM3,
                                        Result.buffer,
                                        &ResultBufferSize
                                        );
            Result.size = (UINT16)ResultBufferSize;
            if (EFI_ERROR(Status))
            {
                return EFI_DEVICE_ERROR;
            }

            gBS->CopyMem(TpmDigest->sm3256, Result.buffer, Result.size);

            DEBUG((EFI_D_VERBOSE, "\n Tpm2SequenceComplete Success \n"));
        }
    }

    *HashCount = count;

Finish:
    if (SectionHeader != NULL)
    {
        FreePool (SectionHeader);
    }

    return Status;
}


EFI_STATUS
EFIAPI
MeasureHandoffTables (
    VOID
)
{
    EFI_STATUS                        Status = EFI_SUCCESS;
#if ( defined(Measure_Smbios_Tables) && (Measure_Smbios_Tables!= 0) )
    SMBIOS_TABLE_ENTRY_POINT          *SmbiosTable=NULL;
    TrEE_EVENT                        *Tpm20Event=NULL;

    if(TrEEProtocolInstance == NULL)
    {
        Status = gBS->LocateProtocol(&gEfiTrEEProtocolGuid, NULL, &TrEEProtocolInstance);
        if(EFI_ERROR(Status)){
            return EFI_NOT_FOUND;
        }
    }

    Status = gBS->AllocatePool(EfiBootServicesData, (sizeof(TrEE_EVENT_HEADER) + \
                               sizeof(UINT32) + sizeof(EFI_HANDOFF_TABLE_POINTERS)), &Tpm20Event);

    if(EFI_ERROR(Status) || (Tpm20Event == NULL))return Status;

    Status = EfiGetSystemConfigurationTable (&gEfiSmbiosTableGuid,
             (VOID **) &SmbiosTable);

    if (!EFI_ERROR (Status))
    {
        ASSERT (SmbiosTable != NULL);
    }

    Tpm20Event->Size  = sizeof(TrEE_EVENT_HEADER) + \
                        sizeof(UINT32) + sizeof(EFI_HANDOFF_TABLE_POINTERS);

    Tpm20Event->Header.HeaderSize = sizeof(TrEE_EVENT_HEADER);
    Tpm20Event->Header.HeaderVersion = 1;
    Tpm20Event->Header.PCRIndex    = 1;
    Tpm20Event->Header.EventType   = EV_EFI_HANDOFF_TABLES;

    ((EFI_HANDOFF_TABLE_POINTERS *)((UINTN)&Tpm20Event->Event[0]))->NumberOfTables = 1;
    ((EFI_HANDOFF_TABLE_POINTERS *)((UINTN)&Tpm20Event->Event[0]))->TableEntry[0].VendorGuid = gEfiSmbiosTableGuid;
    ((EFI_HANDOFF_TABLE_POINTERS *)((UINTN)&Tpm20Event->Event[0]))->TableEntry[0].VendorTable = SmbiosTable;

    Status = TrEEProtocolInstance->HashLogExtendEvent(TrEEProtocolInstance,
             0, (EFI_PHYSICAL_ADDRESS)(UINT8*)(UINTN)SmbiosTable->TableAddress,
             SmbiosTable->TableLength,
             Tpm20Event);

    gBS->FreePool(Tpm20Event);

#endif

    return Status;
}



TCG_PLATFORM_SETUP_PROTOCOL       *AmiProtocolInstance;
AMI_INTERNAL_HLXE_PROTOCOL        *InternalHLXE = NULL;
//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TreeMeasurePeImage
//
// Description: Measure a PE Image
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
EFIAPI
TreeMeasurePeImage (
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
    TCG_PCR_EVENT2_HDR                TcgEvent;
    UINT8                             *EventData = NULL;
    EFI_IMAGE_LOAD_EVENT              *ImageLoad = NULL;
    EFI_DEVICE_PATH_PROTOCOL          *DevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL          *FullPath = NULL;
    UINT32                            FullPathSize;
    EFI_IMAGE_DOS_HEADER              *DosHdr = NULL;
    UINT32                            PeCoffHeaderOffset;
    UINT32                            Tcg2SpecVersion;
    UINT32                            PcrBanks;
    UINTN                             iter=0;
    UINTN                             HashCount=0;
    TPM2_HALG                         TpmDigest;

    DEBUG ((DEBUG_INFO, "TreeMeasurePeImage Entry\n"));

    if(AmiProtocolInstance==NULL)
    {
        Status = gBS->LocateProtocol (&gTcgPlatformSetupPolicyGuid,  NULL, &AmiProtocolInstance);
        if (EFI_ERROR (Status))
        {
            return 0;
        }
    }

    Tcg2SpecVersion = AmiProtocolInstance->ConfigFlags.Tcg2SpecVersion;
    PcrBanks = AmiProtocolInstance->ConfigFlags.PcrBanks;

    ImageLoad     = NULL;
    FullPath      = NULL;

    FullPathSize  = 0;

//  DEBUG ((DEBUG_INFO, "TreeMeasurePeImage Entry\n")); // The repeat debug message.

    if (DeviceHandle != NULL)
    {
        //
        // Get device path for the device handle
        //
        Status = gBS->HandleProtocol (
                     DeviceHandle,
                     &gEfiDevicePathProtocolGuid,
                     &DevicePath
                 );
        if (EFI_ERROR (Status))
        {
            FullPathSize = (UINT32)GetDevicePathSize (FullPath);
        }
        else
        {
            FullPath     = AppendDevicePath (DevicePath, FilePath);
            FullPathSize = (UINT32)GetDevicePathSize (FullPath);
        }
    }
    else if(FilePath!= NULL)
    {
        FullPath     = FilePath;
        FullPathSize  = (UINT32) GetDevicePathSize (FilePath);
    }

    //Allocate Event log memory
    Status = gBS ->AllocatePool(EfiBootServicesData, ((sizeof (*ImageLoad)
                                - sizeof (ImageLoad->DevicePath)) + FullPathSize), &EventData);

    if(EFI_ERROR(Status)){
        goto Done;
    }
    //
    // Determine destination PCR by BootPolicy
    //
    TcgEvent.EventSize  = sizeof (*ImageLoad) - sizeof (ImageLoad->DevicePath);
    TcgEvent.EventSize += FullPathSize;

    switch (ImageType)
    {
        case EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION:
            TcgEvent.PCRIndex   = 4;
            TcgEvent.EventType = EV_EFI_BOOT_SERVICES_APPLICATION;
            break;
        case EFI_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER:
            TcgEvent.PCRIndex   = 2;
            TcgEvent.EventType = EV_EFI_BOOT_SERVICES_DRIVER;
            break;
        case EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER:
            TcgEvent.PCRIndex   = 2;
            TcgEvent.EventType = EV_EFI_RUNTIME_SERVICES_DRIVER;
#if defined(HashSmmDrivers) && (HashSmmDrivers == 0)
            if(ReadyToBootSignaled==FALSE)
            {
                goto Done;
            }
#endif
            break;
        default:
            TcgEvent.EventType = ImageType;
            Status = EFI_UNSUPPORTED;
            goto Done;
    }

    Status = gBS ->AllocatePool(EfiBootServicesData,TcgEvent.EventSize, &ImageLoad);

    if (ImageLoad == NULL)
    {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
    }

    ImageLoad->ImageLocationInMemory = ImageAddress;
    ImageLoad->ImageLengthInMemory   = ImageSize;
    ImageLoad->ImageLinkTimeAddress  = LinkTimeBase;
    ImageLoad->LengthOfDevicePath    = FullPathSize;
    gBS->CopyMem( ImageLoad->DevicePath, FullPath,  FullPathSize );

    //
    // Check PE/COFF image
    //
    DosHdr = (EFI_IMAGE_DOS_HEADER *)(UINTN)ImageAddress;
    PeCoffHeaderOffset = 0;
    if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE)
    {
        PeCoffHeaderOffset = DosHdr->e_lfanew;
    }
    if (((EFI_TE_IMAGE_HEADER *)((UINT8 *)(UINTN)ImageAddress + PeCoffHeaderOffset))->Signature
            == EFI_TE_IMAGE_HEADER_SIGNATURE)
    {

        //Measure TE Image
        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            MeasureTeImage(ImageLoad, Tcg2SpecVersion, PcrBanks, &TpmDigest, &HashCount);
            gBS->CopyMem(&TcgEvent.Digests.digests[0].digest.sha1, TpmDigest.sha1, SHA1_DIGEST_SIZE);
            TcgEvent.Digests.digests[0].hashAlg = TPM2_ALG_SHA1;
            TcgEvent.Digests.count = 1;
        }

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            MeasureTeImage(ImageLoad, Tcg2SpecVersion, PcrBanks, &TpmDigest, &HashCount);
            if( PcrBanks & TREE_BOOT_HASH_ALG_SHA1)
            {
                gBS->CopyMem(&TcgEvent.Digests.digests[iter].digest.sha1, &TpmDigest.sha1, SHA1_DIGEST_SIZE);
                TcgEvent.Digests.digests[iter].hashAlg = TPM2_ALG_SHA1;
                iter+=1;
            }

            if( PcrBanks & TREE_BOOT_HASH_ALG_SHA256)
            {
                gBS->CopyMem(&TcgEvent.Digests.digests[iter].digest.sha256, &TpmDigest.sha256, SHA256_DIGEST_SIZE);
                TcgEvent.Digests.digests[iter].hashAlg = TPM2_ALG_SHA256;
                iter+=1;
            }

            if( PcrBanks & TREE_BOOT_HASH_ALG_SHA384)
            {
                gBS->CopyMem(&TcgEvent.Digests.digests[iter].digest.sha384,  &TpmDigest.sha384, SHA384_DIGEST_SIZE);
                TcgEvent.Digests.digests[iter].hashAlg = TPM2_ALG_SHA384;
                iter+=1;
            }

            if( PcrBanks & TREE_BOOT_HASH_ALG_SHA512)
            {
                gBS->CopyMem(&TcgEvent.Digests.digests[iter].digest.sha512,  &TpmDigest.sha512, SHA512_DIGEST_SIZE);
                TcgEvent.Digests.digests[iter].hashAlg = TPM2_ALG_SHA512;
                iter+=1;
            }

            if( PcrBanks & TREE_BOOT_HASH_ALG_SM3)
            {
                gBS->CopyMem(&TcgEvent.Digests.digests[iter].digest.sm3_256,  &TpmDigest.sm3256, SM3_256_DIGEST_SIZE);
                TcgEvent.Digests.digests[iter].hashAlg = TPM2_ALG_SM3_256;
                iter+=1;
            }

            TcgEvent.Digests.count = (UINT32)HashCount;
        }

        goto MeasurePeTeImageDone;
    }
    // MeasrePeImage
    else
    {
        // The change the Fix for X32_Build Windows OS, will check the Digest of the PeImage.
        // We have match the algorithm on WinOS
        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            MeasurePeImage( ImageAddress, ImageSize, Tcg2SpecVersion, PcrBanks, &TpmDigest, &HashCount);
            gBS->CopyMem(&TcgEvent.Digests.digests[0].digest.sha1, TpmDigest.sha1, SHA1_DIGEST_SIZE);
            TcgEvent.Digests.digests[0].hashAlg = TPM2_ALG_SHA1;
            TcgEvent.Digests.count = 1;
        }

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            MeasurePeImage( ImageAddress, ImageSize, Tcg2SpecVersion, PcrBanks, &TpmDigest, &HashCount);
            if( PcrBanks & TREE_BOOT_HASH_ALG_SHA1)
            {
                gBS->CopyMem(&TcgEvent.Digests.digests[iter].digest.sha1, &TpmDigest.sha1, SHA1_DIGEST_SIZE);
                TcgEvent.Digests.digests[iter].hashAlg = TPM2_ALG_SHA1;
                iter+=1;
            }

            if( PcrBanks & TREE_BOOT_HASH_ALG_SHA256)
            {
                gBS->CopyMem(&TcgEvent.Digests.digests[iter].digest.sha256, &TpmDigest.sha256, SHA256_DIGEST_SIZE);
                TcgEvent.Digests.digests[iter].hashAlg = TPM2_ALG_SHA256;
                iter+=1;
            }

            if( PcrBanks & TREE_BOOT_HASH_ALG_SHA384)
            {
                gBS->CopyMem(&TcgEvent.Digests.digests[iter].digest.sha384,  &TpmDigest.sha384, SHA384_DIGEST_SIZE);
                TcgEvent.Digests.digests[iter].hashAlg = TPM2_ALG_SHA384;
                iter+=1;
            }

            if( PcrBanks & TREE_BOOT_HASH_ALG_SHA512)
            {
                gBS->CopyMem(&TcgEvent.Digests.digests[iter].digest.sha512,  &TpmDigest.sha512, SHA512_DIGEST_SIZE);
                TcgEvent.Digests.digests[iter].hashAlg = TPM2_ALG_SHA512;
                iter+=1;
            }

            if( PcrBanks & TREE_BOOT_HASH_ALG_SM3)
            {
                gBS->CopyMem(&TcgEvent.Digests.digests[iter].digest.sm3_256,  &TpmDigest.sm3256, SM3_256_DIGEST_SIZE);
                TcgEvent.Digests.digests[iter].hashAlg = TPM2_ALG_SM3_256;
                iter+=1;
            }

            TcgEvent.Digests.count = (UINT32)HashCount;
        }

        goto MeasurePeTeImageDone;
    }

MeasurePeTeImageDone:

    //
    // HashLogExtendEvent
    //
    gBS->CopyMem(EventData, ImageLoad, TcgEvent.EventSize);

    Status = gBS->LocateProtocol(&AmiProtocolInternalHlxeGuid, NULL, &InternalHLXE);
    if(EFI_ERROR(Status)){
        goto Done;
    }

    if(TrEEProtocolInstance == NULL)
    {
        Status = gBS->LocateProtocol(&gEfiTrEEProtocolGuid, NULL, &TrEEProtocolInstance);
        if(EFI_ERROR(Status)){
            goto Done;
        }
    }

    if(ReadyToBootSignaled == TRUE)
    {
        MeasureHandoffTables();
    }

    if(TcgEvent.EventType == EV_EFI_BOOT_SERVICES_APPLICATION){
        FindandMeasureSecureBootCertificate();
    }

#if defined (Measure_CA_For_External_EFI_DRIVERS) && (Measure_CA_For_External_EFI_DRIVERS == 1)
    if(TcgEvent.EventType == EV_EFI_BOOT_SERVICES_DRIVER){
        FindandMeasureSecureBootCertificate();
    }
#endif

    InternalHLXE->AmiHashLogExtend2(TrEEProtocolInstance, NULL, 0, 0, &TcgEvent, EventData);
    TpmDxeReportStatusCodeEx(EFI_PROGRESS_CODE, AMI_SPECIFIC_UEFI_IMAGE_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER, 0,
                           NULL, &ExtendedDataGuid, FullPath, FullPathSize);
    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_UEFI_IMAGE_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER);

Done:
    if (ImageLoad != NULL)
    {
        gBS->FreePool (ImageLoad);
    }

    if(EventData != NULL)
    {
        gBS->FreePool (EventData);
    }

    return Status;
}

EFI_STATUS
InternalMeasureAction (
    IN      CHAR8                     *ActionString
)
{
    return EFI_SUCCESS; //not supported
}

EFI_STATUS
InternalMeasureGpt (
    IN      EFI_DEVICE_PATH_PROTOCOL  *DevicePath
)
{

    return EFI_SUCCESS; //not supported;
}


EFI_TCG_PLATFORM_PROTOCOL  mTcgPlatformProtocol =
{
    TreeMeasurePeImage,
    InternalMeasureAction,
    InternalMeasureGpt
};



VOID
EFIAPI
TrEEOnReadyToBoot (
    IN      EFI_EVENT                 Event,
    IN      VOID                      *Context
)
{
    ReadyToBootSignaled = TRUE;
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   InstallTrEEProtocol
//
// Description: Install Tree Protocol
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
InstallTrEEProtocol(
    IN EFI_HANDLE Handle
)
{
    EFI_STATUS Status;


    DEBUG(( DEBUG_INFO," InstallTrEEProtocol \n"));

    Status = gBS->AllocatePages(AllocateAnyPages,
                                EfiACPIMemoryNVS,
                                EFI_SIZE_TO_PAGES(MAX_LOG_AREA_SIZE),
                                (UINT64*)(UINTN)&TreeEventLogLocation);

    if(EFI_ERROR(Status))return Status;

    // When SHA1 + SHA2 enable, we still need to provide the SHA1 format log.
    Status = gBS->AllocatePages(AllocateAnyPages,
                                EfiACPIMemoryNVS,
                                EFI_SIZE_TO_PAGES(MAX_LOG_AREA_SIZE),
                                (UINT64*)(UINTN)&TreeExtraTCPASha1LogLoc);

    if(EFI_ERROR(Status))return Status;

    Status = gBS->AllocatePages(AllocateAnyPages,
                                EfiACPIMemoryNVS,
                                EFI_SIZE_TO_PAGES(MAX_LOG_AREA_SIZE),
                                (UINT64*)(UINTN)&pEfiTcg2FinalEventsTbl);

    if(EFI_ERROR(Status))return Status;

    DEBUG(( DEBUG_INFO," TreeExtraTCPASha1LogLoc = %x \n", TreeEventLogLocation));
    DEBUG(( DEBUG_INFO," TreeEventLogLocation    = %x \n", TreeExtraTCPASha1LogLoc));
    DEBUG(( DEBUG_INFO," pEfiTcg2FinalEventsTbl  = %x \n", pEfiTcg2FinalEventsTbl));

    gBS->SetMem(
        (VOID*)((UINTN)pEfiTcg2FinalEventsTbl),
        (UINTN)MAX_LOG_AREA_SIZE,
        0x00);

    ((EFI_TCG2_FINAL_EVENTS_TABLE *)pEfiTcg2FinalEventsTbl)->Version = EFI_TCG2_FINAL_EVENTS_TABLE_VERSION;
    ((EFI_TCG2_FINAL_EVENTS_TABLE *)pEfiTcg2FinalEventsTbl)->NumberOfEvents   = 0;

    Status = gBS->InstallConfigurationTable(&gTcg2FinalEventsTableGuid, (VOID*)((UINTN)pEfiTcg2FinalEventsTbl));

    if(EFI_ERROR(Status))
    {
        DEBUG(( DEBUG_ERROR," TCG2 Finals Configuration table install failed\n"));
    }

    gBS->SetMem(
        (VOID*)((UINTN)TreeEventLogLocation),
        (UINTN)MAX_LOG_AREA_SIZE,
        0xFF);

    gBS->SetMem(
        (VOID*)((UINTN)TreeExtraTCPASha1LogLoc),
        (UINTN)MAX_LOG_AREA_SIZE,
        0xFF);

    //locate PEI hob and copy to the TreeLogArea
    Status = CopyTcgLog();

    //interface installation is
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Handle,
                    &gEfiTrEEProtocolGuid, &mTreeProtocol,
                    &AmiProtocolInternalHlxeGuid,    &InternalLogProtocol,
                    &gAmiDxeHashInterfaceguid,  &AmiHashAllInterface,
                    &gEfiTcgPlatformProtocolGuid,  &mTcgPlatformProtocol,
                    NULL
                    );

    if(EFI_ERROR(Status))return Status;

    DEBUG(( DEBUG_INFO," InstallTrEEProtocol Exit Status = %r \n", Status));
    return Status;
}

VOID ReadMORValue( );


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TreeDxeEntry
//
// Description: TreeDxeEntry
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
EFIAPI TreeDxeEntry(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS          Status;
    TCG_PLATFORM_SETUP_PROTOCOL     *PolicyInstance;
    EFI_EVENT           ReadyToBootEvent;
    static EFI_EVENT    gAmiSystemResetEvent;
    TCG_CONFIGURATION   Config;
    static              EFI_EVENT      Resetev;
    UINT32              TpmError=0;
    static      void    * Resetreg;
    static              EFI_EVENT      PcrAllocEv;
    static      void    *PcrAllocreg;
    static EFI_RESET_TYPE ptype;
    UINT8               ResetVal=0;
    UINTN               sizeofResetVal=sizeof(ResetVal);

    DEBUG(( DEBUG_INFO," TreeDxeEntry \n"));
    Status = gBS->LocateProtocol (&gTcgPlatformSetupPolicyGuid,  NULL, &PolicyInstance);
    if (EFI_ERROR (Status))
    {
        return Status;
    }


    gBS-> CopyMem(&Config, &PolicyInstance->ConfigFlags, sizeof(TCG_CONFIGURATION));

    if(Config.DeviceType == 0)
    {
        Config.Tpm20Device = 0;
        PolicyInstance->UpdateStatusFlags(&Config, FALSE);
        return EFI_SUCCESS;
    }

    if(!PolicyInstance->ConfigFlags.TpmSupport)
    {
        return EFI_SUCCESS;
    }

    if(!IsTpm20Device())
    {
        Config.Tpm20Device = 0;

        PolicyInstance->UpdateStatusFlags(&Config, FALSE);
        DEBUG(( DEBUG_INFO," isTpm20CrbPresent returned false \n"));
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MINOR, AMI_SPECIFIC_TPM_ERR_2_0_NOT_DISCOVERED | EFI_SOFTWARE_DXE_BS_DRIVER);

        if(*((UINT32 *)(UINTN)0xFED40000)== 0xFFFFFFFF){
            TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MINOR, AMI_SPECIFIC_TPM_ERR_NO_TPM_DEVICE | EFI_SOFTWARE_DXE_BS_DRIVER);
        }
        return EFI_UNSUPPORTED;
    }

    Config.Tpm20Device = 1;
    Config.InterfaceSel = TpmSupport->InterfaceType;

    ReadMORValue();

    Status = InstallTrEEProtocol(ImageHandle);
    if(EFI_ERROR(Status)){
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MINOR, AMI_SPECIFIC_TPM_ERR_TCG_PROTOCOL_NOT_INSTALLED_TPM_2_0_DEVICE_FOUND | EFI_SOFTWARE_DXE_BS_DRIVER);
        return EFI_ABORTED;
    }

    // Here, if customer using the TCG2 protocol version 1.0 ( 0.99 Old version)
    // It only support the SHA1 Event Log, we must make sure the SHA1 PCR Bank is Enable
    // Or there is no PcrBanks, we have to enable at least one,
    // According to the spec define, the Activate PCR Banks at least 1
    if( TCG2_PROTOCOL_SPEC_TCG_1_2 == Config.Tcg2SpecVersion || 0 == Config.PcrBanks)
    {
        if( 0 == (Config.PcrBanks & TREE_BOOT_HASH_ALG_SHA1) )
        {
            Config.PcrBanks |= TREE_BOOT_HASH_ALG_SHA1;

            Status = PolicyInstance->UpdateStatusFlags(&Config, TRUE);
            ASSERT( !EFI_ERROR( Status ));
        }
    }

    Tpm2GetCapabilityCapPCRS();

    DEBUG(( DEBUG_INFO," ActiveBankBitMap = %x\n", ActiveBankBitMap));
    DEBUG(( DEBUG_INFO," Config.PcrBanks = %x\n", Config.PcrBanks));
    DEBUG(( DEBUG_INFO," gNumberOfPcrBanks = %x \n", gNumberOfPcrBanks));

    Status   = gRT->GetVariable(L"Tpm20PCRallocateReset",
                                &gTcgInternalflagGuid,
                                NULL,
                                &sizeofResetVal,
                                &ResetVal);

    if(EFI_ERROR(Status) || ResetVal == 0)
    {
#if defined (ALLOCATE_PCR_AFTER_SMM_INIT) && (ALLOCATE_PCR_AFTER_SMM_INIT == 0)
        if(((TcgSupportedBankBitMap & Config.PcrBanks)& ActiveBankBitMap) != (ActiveBankBitMap | (TcgSupportedBankBitMap & Config.PcrBanks)))
        {
            Status = Tpm2AllocatePCR( Config.PcrBanks, ActiveBankBitMap, TcgSupportedBankBitMap, &TpmError);

            // We still need to check the AllocatePCR is success or not.
            // If the TPM is not support the command, we should not execute the gRS->ResetSystem to make the system dead loop
            if( !EFI_ERROR(Status) )
            {
                ResetVal = 1;
                Status = gRT->SetVariable( L"Tpm20PCRallocateReset", \
                                           &gTcgInternalflagGuid, \
                                           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, \
                                           sizeof(UINT8), \
                                           &ResetVal);

                if(!EFI_ERROR(Status))
                {
                    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
                    DEBUG((DEBUG_ERROR, "\tError: Reset failed???\n"));

                    Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                                               EFI_TPL_CALLBACK, DoResetNow, NULL, &Resetev);

                    ASSERT( !EFI_ERROR( Status ));
                    Status = gBS->RegisterProtocolNotify(&gEfiResetArchProtocolGuid, Resetev, &Resetreg);
                    DEBUG((DEBUG_INFO, "\tRegister DoResetNow after Reset Architecture driver\n"));
                }
            }
        }
#else
        if(((TcgSupportedBankBitMap & Config.PcrBanks)& ActiveBankBitMap) != (ActiveBankBitMap | (TcgSupportedBankBitMap & Config.PcrBanks)))
        {
            Status = gBS->CreateEvent( EVT_NOTIFY_SIGNAL,
                                       EFI_TPL_CALLBACK, Tpm2AllocatePCRCallback, NULL, &PcrAllocEv);

            ASSERT( !EFI_ERROR( Status ));
            Status = gBS->RegisterProtocolNotify(&gBdsAllDriversConnectedProtocolGuid, PcrAllocEv, &PcrAllocreg);

            goto InstallTPM20Protocol;

        }
#endif
    }

    // Because the Default PcrBanks maybe out of the Actually ActiveBankBitMap
    // Here, we need to sync once
    // For example:
    // If BIOS Default PcrBanks is 0x1F ( All Banks is Enable ), But the Real Support ActivateBank may be SHA1+SHA256
    // We may need to sync PcrBanks with ActiveBankBitMap.
    if(ResetVal == 1)
    {
        ResetVal = 0;
        Status = gRT->SetVariable( L"Tpm20PCRallocateReset", \
                                   &gTcgInternalflagGuid, \
                                   EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, \
                                   sizeof(UINT8), \
                                   &ResetVal);

        ASSERT( !EFI_ERROR( Status ));
    }

    Config.PcrBanks = ActiveBankBitMap;

#if defined (ALLOCATE_PCR_AFTER_SMM_INIT) && (ALLOCATE_PCR_AFTER_SMM_INIT == 1)
InstallTPM20Protocol:
#endif
    //install binding protocol TrEE binding protocol
    Tpm20DriverBinding.DriverBindingHandle = ImageHandle;
    Tpm20DriverBinding.ImageHandle = ImageHandle;

    Status = gBS->InstallMultipleProtocolInterfaces (
                 &Tpm20DriverBinding.DriverBindingHandle,
                 &gEfiDriverBindingProtocolGuid, &Tpm20DriverBinding,
                 &gEfiComponentNameProtocolGuid, &gComponentName,
                 &gEfiComponentName2ProtocolGuid, &gComponentName2,
                 NULL);


    TrEEUpdateTpmDeviceASL(Event, reg);
    Config.TcgSupportEnabled = 1;
    Config.TcmSupportEnabled = 0;
    Config.TpmHardware = 0; // Hardware Present

    Status = EfiCreateEventReadyToBootEx(TPL_CALLBACK,
                                         TrEEOnReadyToBoot,
                                         NULL,
                                         &ReadyToBootEvent);

    if(EFI_ERROR(Status))return Status;

    // on every gRT->ResetSystem, TCG need to execute the TPM2_Shutdown(...)
    Status = gBS->CreateEventEx(
                     EVT_NOTIFY_SIGNAL,
                     TPL_CALLBACK,
                     TPM2ResetSystemWorkAround,
                     NULL,
                     &gAmiResetSystemEventGuid,
                     &gAmiSystemResetEvent );
    
    ASSERT_EFI_ERROR(Status);
    Status = PolicyInstance->UpdateStatusFlags(&Config, FALSE);
    return Status;
}

typedef VOID (*tdTpmtHaProc)( UINT8* pTpmtHa, VOID* CallBackContext );

UINTN GetNextSMLEvent(MiscPCR_EVENT_HDR* pStart, MiscPCR_EVENT_HDR** pNext)
{
    UINT8*          _pStart = (UINT8*)pStart;

    *pNext = (MiscPCR_EVENT_HDR*)(_pStart + sizeof( MiscPCR_EVENT_HDR ) + pStart->EventSize );

    if( !(*pNext)->EventType && !(*pNext)->EventSize )
        return -1;

    if( (UINTN)(*pNext) > LastEntry )
        return -1;

    return *pNext - pStart;
}

UINTN GetSize_TpmpHa( UINT8* pStart, UINT8** pNext, tdTpmtHaProc TpmtHaProc, VOID *CallBackContext )
{
    UINT8*      pBuf = pStart;
    UINTN       DigestSize;
    UINT16      AlgorithmId;

    AlgorithmId = *(UINT16*)pBuf;
//    AlgorithmId = TPM_H2NS( AlgorithmId );
    switch( AlgorithmId )
    {
        case TPM2_ALG_SHA1:
            DigestSize = SHA1_DIGEST_SIZE;
            break;
        case  TPM2_ALG_SHA256:
            DigestSize = SHA256_DIGEST_SIZE;
            break;
        case TPM2_ALG_SHA384:
            DigestSize = SHA384_DIGEST_SIZE;
            break;
        case TPM2_ALG_SHA512:
            DigestSize = SHA512_DIGEST_SIZE;
            break;
        case TPM2_ALG_SM3_256:
            DigestSize = SM3_256_DIGEST_SIZE;
            break;
        default:
            DigestSize = -1;
            // The Digest AlgorithmId Err.
            return -1;
            break;
    }

    if( TpmtHaProc )
    {
        TpmtHaProc( pStart, CallBackContext );
    }

    pBuf += sizeof(UINT16);     // AlgorithmId
    pBuf += DigestSize;         // Digest

    *pNext = pBuf;

    return (UINTN)( pBuf - pStart );

}

UINTN   GetSize_TpmlDigestValues( UINT8* pStart, UINT8** pNext, tdTpmtHaProc TpmtHaProc, VOID *CallBackContext )
{
    UINT8*      pBuf = pStart;
    UINTN       unIdx;
    UINTN       Count;

    Count = *(UINT32*)pBuf;
    pBuf += sizeof(UINT32);     // Count

    for( unIdx=0; unIdx<Count; ++unIdx)
    {
        if( -1 == GetSize_TpmpHa( pBuf, &pBuf, TpmtHaProc, CallBackContext ) )
        {
            // The Struct Parsing Err.
            return -1;
        }

    }

    *pNext = pBuf;

    return (UINTN)( pBuf - pStart );
}

VOID ExtraSha1FromTpmHa( UINT8* pTpmtHa, VOID* pCopyStartBuf )
{
    UINT8*      pBuf = pTpmtHa;
    UINTN       DigestSize;
    UINT16      AlgorithmId;
    BOOLEAN     bFound = FALSE;

    AlgorithmId = *(UINT16*)pBuf;

//    AlgorithmId = TPM_H2NS( AlgorithmId );
    switch( AlgorithmId )
    {
        case TPM2_ALG_SHA1:
            bFound = TRUE;
            DigestSize = SHA1_DIGEST_SIZE;
            break;
        case  TPM2_ALG_SHA256:
            DigestSize = SHA256_DIGEST_SIZE;
            break;
        case TPM2_ALG_SHA384:
            DigestSize = SHA384_DIGEST_SIZE;
            break;
        case TPM2_ALG_SHA512:
            DigestSize = SHA512_DIGEST_SIZE;
            break;
        case TPM2_ALG_SM3_256:
            DigestSize = SM3_256_DIGEST_SIZE;
            break;
        default:
            DigestSize = -1;
            // The Digest AlgorithmId Err.
            return;
            break;
    }

    pBuf += sizeof(UINT16);

    if( SHA1_DIGEST_SIZE == DigestSize && TRUE == bFound)
    {
        gBS->CopyMem( pCopyStartBuf, pBuf, SHA1_DIGEST_SIZE );
    }
}

UINTN  ExtractSingleTcpaEventFromTcgEVENT2( UINT8* pStart, UINT8** pNext, UINT8 *pCopyStartBuf  )
{
    UINT8*          pBuf = pStart;
    UINTN           EventSize = 0;
    UINT8*          pCopyBuf = pCopyStartBuf;
    UINTN           unTmpOffset = 0;

    EFI_STATUS  Status;

    if( ((MiscPCR_EVENT_HDR*)pStart)->EventType == 0x03 && pStart == (UINT8*)TreeEventLogLocation ) // EV_NO_ACTION
    {
//        bLogFirstActEvent = TRUE;
        Status = GetNextSMLEvent( (MiscPCR_EVENT_HDR*)pStart, (MiscPCR_EVENT_HDR**)pNext );
        if( EFI_ERROR(Status) )
        {
            DEBUG(( DEBUG_ERROR, "Error[%d]: \n", __LINE__ ));
            return -1;
        }

//        return (UINTN)( (*pNext) - pStart );
        return 0;
    }

    *(UINT32*)(pCopyStartBuf + unTmpOffset)  = *(UINT32*)(pStart + unTmpOffset);   // PCRIndex
    unTmpOffset += sizeof(UINT32);

    *(UINT32*)(pCopyStartBuf + unTmpOffset) = *(UINT32*)(pStart + unTmpOffset);   // EventType
    unTmpOffset += sizeof(UINT32);

    pBuf = pStart + unTmpOffset;
    pCopyBuf = pCopyStartBuf + unTmpOffset;
    if( -1 == GetSize_TpmlDigestValues( pBuf, &pBuf, ExtraSha1FromTpmHa, pCopyBuf ) )    // Digests
    {
        // The Struct Detect Err.
        return -1;
    }
    unTmpOffset += SHA1_DIGEST_SIZE;

    EventSize = *(UINT32*)(pBuf);
    *(UINT32*)(pCopyStartBuf + unTmpOffset) = (UINT32)EventSize;
    pBuf += sizeof(UINT32);
    unTmpOffset += sizeof(UINT32);     // EventSize

    gBS->CopyMem( pCopyStartBuf + unTmpOffset, pBuf, EventSize );
    unTmpOffset += EventSize;          // EventData
    pBuf += EventSize;

    *pNext = pBuf;

    return unTmpOffset;
}

UINTN TransferTcgEvent2ToTcpaEvent( VOID )
{
    UINT8*                              pNext = NULL;
    UINT8*                              pCopyBuf = (UINT8*)TreeExtraTCPASha1LogLoc;
    UINTN                               SingleEventSize = 0;

    pNext = (UINT8*)TreeEventLogLocation;
    for(SingleEventSize = 0; SingleEventSize != -1; )
    {
        pCopyBuf += SingleEventSize;

        if( (UINTN)pNext >= (UINTN)LastEventPtr )
        {
            if(  (UINTN)pNext == (UINTN)LastEventPtr )
            {
                SingleEventSize = ExtractSingleTcpaEventFromTcgEVENT2( pNext, &pNext, pCopyBuf );
            }
            break;
        }

        SingleEventSize = ExtractSingleTcpaEventFromTcgEVENT2( pNext, &pNext, pCopyBuf );
    };

    return (UINTN)pCopyBuf;
}

//
// <+ End of Extract the TCPA Sha1 Log from Crypto Agile Log Event
//
