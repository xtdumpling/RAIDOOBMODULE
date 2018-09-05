/*++

   Copyright (c) 2005 Intel Corporation. All rights reserved
   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.


   Module Name:

   TcgDxe.c

   Abstract:

   DXE Driver that provides TCG services

   --*/
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/TcgDxe.c 33    5/09/12 6:37p Fredericko $
//
// $Revision: 33 $
//
// $Date: 5/09/12 6:37p $
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
#include <Token.h>
#include <AmiTcg/TcgCommon12.h>
#include <AmiTcg/sha.h>
#include <AmiTcg/TCGMisc.h>
#include <Protocol/TcgTcmService.h>
#include <Protocol/TcgPlatformSetupPolicy.h>
#include <Protocol/AcpiSupport.h>
#include "AmiTcg/TcgPc.h"
#include "Protocol/TcgService.h"
#include "Protocol/TpmDevice.h"
#include<Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/LocalApicLib.h>
#include <IndustryStandard/Acpi30.h>
#include <Protocol/Runtime.h>
#include <Library/UefiLib.h>
#include <Guid/MemoryOverwriteControl.h>
#include <AmiTcg/AmiTpmStatusCodes.h>
#include <Library/PcdLib.h>

#include <AmiProtocol.h>
#include <Guid/AmiTcgGuidIncludes.h>

#if PI_SPECIFICATION_VERSION<0x00010000
#include <Protocol/FirmwareVolume.h>
#else
#include <Protocol/FirmwareVolume2.h>
#endif

extern EFI_GUID TcgPlatformSetupPolicyGuid;
extern EFI_GUID gEfiHobListGuid;

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

typedef struct
{
    EFI_TCG_PCR_EVENT_HEADER Header;
    EFI_TCG_EV_POST_CODE     Event;
} PEI_EFI_POST_CODE;
#pragma pack()


typedef struct _TCG_DXE_PRIVATE_DATA
{
    EFI_TCG_PROTOCOL        TcgServiceProtocol;
    EFI_TPM_DEVICE_PROTOCOL *TpmDevice;
} TCG_DXE_PRIVATE_DATA;


typedef struct _TCM_DXE_PRIVATE_DATA
{
    EFI_TCM_PROTOCOL        TcgServiceProtocol;
    EFI_TPM_DEVICE_PROTOCOL *TpmDevice;
} TCM_DXE_PRIVATE_DATA;



EFI_STATUS EFIAPI TcgDxeLogEvent (
    IN EFI_TCG_PROTOCOL *This,
    IN TCG_PCR_EVENT    *TCGLogData,
    IN OUT UINT32       *EventNumber,
    IN UINT32           Flags );

EFI_STATUS EFIAPI TcmDxeLogEvent(
    IN EFI_TCM_PROTOCOL *This,
    IN TCM_PCR_EVENT    *TCGLogData,
    IN OUT UINT32       *EventNumber,
    IN UINT32           Flags );


BOOLEAN IsTpm20Device()
{
   Tpm20DeviceHob  *TpmSupport = NULL;
   
   TpmSupport = LocateATcgHob( gST->NumberOfTableEntries,
                  gST->ConfigurationTable,
                  &gTpm20HobGuid);

   if(TpmSupport != NULL){
      if((*(volatile UINT8 *)((UINTN)(0xfed40000))!=0xff) &&
              !isTpm20CrbPresent()){
         if(TpmSupport->Tpm20DeviceState == 1)
           return TRUE;
      }
   }    
   
   return FALSE;
}

UINTN FindNextLogLocation(TCG_PCR_EVENT_HDR   *TcgLog, UINTN EventNum);
UINTN GetLogEventCount(TCG_PCR_EVENT_HDR   *TcgLog);

static UINTN    TcmBootVar = 0;
//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcmBootDone
//
// Description: SetEfiOSTransitions
//
// Input:       IN  EFI_EVENT       efiev
//              IN  VOID            *ctx
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
void TcmBootDone(
    IN EFI_EVENT efiev,
    IN VOID      *ctx )
{
    TcmBootVar = 1;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   EfiOSReadyToBoot
//
// Description: Sets ready to boot callback on ready to boot
//
// Input:   NONE    
//
// Output:   EFI_STATUS   
//
// Modified:
//
// Referrals:   
//
// Notes:       
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI TcmOSTransition()
{
   EFI_EVENT  ReadToBootEvent;
   EFI_STATUS Status;

   #if defined(EFI_EVENT_SIGNAL_READY_TO_BOOT)\
        && EFI_SPECIFICATION_VERSION < 0x20000
       
         Status = gBS->CreateEvent( EFI_EVENT_SIGNAL_READY_TO_BOOT,
                                   EFI_TPL_CALLBACK,
                                   TcmBootDone, NULL, &ReadToBootEvent );
        
   #else
        Status = EfiCreateEventReadyToBootEx( EFI_TPL_CALLBACK,
                                         TcmBootDone,
                                         NULL,
                                         &ReadToBootEvent );
   #endif

   return EFI_SUCCESS;
}


#define _CR( Record, TYPE,\
       Field )((TYPE*) ((CHAR8*) (Record) - (CHAR8*) &(((TYPE*) 0)->Field)))

#define TCG_DXE_PRIVATE_DATA_FROM_THIS( This )  \
    _CR( This, TCG_DXE_PRIVATE_DATA, TcgServiceProtocol )

#define TCM_DXE_PRIVATE_DATA_FROM_THIS( This )  \
    _CR( This, TCM_DXE_PRIVATE_DATA, TcgServiceProtocol )


TCG_ACPI_TABLE                        mTcgAcpiTableTemplate = {
    {
        EFI_ACPI_3_0_TRUSTED_COMPUTING_PLATFORM_ALLIANCE_CAPABILITIES_TABLE_SIGNATURE,
        sizeof (TCG_ACPI_TABLE)
        //
        // Compiler initializes the remaining bytes to 0
        // These fields should be filled in in production
        //
    },
    0,
#if TCG_PLATFORM_CLASS == 1
    0,
#endif
    TPM_LOG_AREA_MAX_LEN,
    (EFI_PHYSICAL_ADDRESS)( -1 ),
    {0}
};

static TPM_Capabilities_PermanentFlag TcgDxe_Cap;

EFI_STATUS
EFIAPI TcgCommonPassThrough(
    IN VOID                    *Context,
    IN UINT32                  NoInputBuffers,
    IN TPM_TRANSMIT_BUFFER     *InputBuffers,
    IN UINT32                  NoOutputBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutputBuffers )
{
    TCG_DXE_PRIVATE_DATA *Private;
    EFI_STATUS            Status;

    Private = TCG_DXE_PRIVATE_DATA_FROM_THIS( Context );

    Status = Private->TpmDevice->Init( Private->TpmDevice );

    Status=  Private->TpmDevice->Transmit(
               Private->TpmDevice,
               NoInputBuffers,
               InputBuffers,
               NoOutputBuffers,
               OutputBuffers
               );
    
    if(EFI_ERROR(Status)){
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_COMMUNICATION_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);   
    }
    
    Private->TpmDevice->Close( Private->TpmDevice );

    return Status;

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
EFIAPI  TcmCommonPassThrough(
    IN VOID                    *Context,
    IN UINT32                  NoInputBuffers,
    IN TPM_TRANSMIT_BUFFER     *InputBuffers,
    IN UINT32                  NoOutputBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutputBuffers )
{
    TCM_DXE_PRIVATE_DATA *Private;

    Private = TCM_DXE_PRIVATE_DATA_FROM_THIS( Context );
    return Private->TpmDevice->Transmit(
               Private->TpmDevice,
               NoInputBuffers,
               InputBuffers,
               NoOutputBuffers,
               OutputBuffers
               );
}


void printbuffer(UINT8 *Buffer, UINTN BufferSize)
{
    UINTN i=0; UINTN j=0;
    
    DEBUG((DEBUG_INFO,"\n**********PrintBuffer Entry******** \n"));
    
    for(i=0; i<BufferSize; i++){
        
        if(i%16 == 0){
            DEBUG((DEBUG_INFO,"\n"));
            DEBUG((DEBUG_INFO,"%04x :", j));
            j+=1;
        }
        DEBUG((DEBUG_INFO,"%02x ", Buffer[i]));
    }
    DEBUG((DEBUG_INFO,"\n"));
}


EFI_STATUS EFIAPI TcgDxePassThroughToTpm(
    IN EFI_TCG_PROTOCOL *This,
    IN UINT32           TpmInputParamterBlockSize,
    IN UINT8            *TpmInputParamterBlock,
    IN UINT32           TpmOutputParameterBlockSize,
    IN UINT8            *TpmOutputParameterBlock )
{
    TPM_TRANSMIT_BUFFER InBuffer[1], OutBuffer[1];
    EFI_STATUS Status;
    TCG_DXE_PRIVATE_DATA              *Private;

    //some applications might not set init command before making this call.
    //Just set init commands[locality zero for them]
    

    Private = TCG_DXE_PRIVATE_DATA_FROM_THIS( This );    

    Status = Private->TpmDevice->Init( Private->TpmDevice );
 
    InBuffer[0].Buffer  = TpmInputParamterBlock;
    InBuffer[0].Size    = TpmInputParamterBlockSize;
    OutBuffer[0].Buffer = TpmOutputParameterBlock;
    OutBuffer[0].Size   = TpmOutputParameterBlockSize;

    Status=  Private->TpmDevice->Transmit(
               Private->TpmDevice,
                sizeof (InBuffer) / sizeof (*InBuffer),
                InBuffer,
                sizeof (OutBuffer) / sizeof (*OutBuffer),
                OutBuffer);
    
    if(EFI_ERROR(Status)){
        TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_COMM_INTERFACE_TIS| EFI_SOFTWARE_PEI_MODULE);
    }

    Private->TpmDevice->Close( Private->TpmDevice );
    
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI  TcgDxeCommonExtend(
    IN VOID         *CallbackContext,
    IN TPM_PCRINDEX PCRIndex,
    IN TCG_DIGEST   *Digest,
    OUT TCG_DIGEST  *NewPCRValue )
{
    TPM_1_2_CMD_HEADER  cmdHeader;
    TPM_1_2_RET_HEADER  retHeader;
    TPM_TRANSMIT_BUFFER InBuffer[3], OutBuffer[2];  

    DEBUG((DEBUG_INFO,"Tpm1_2 Extend Status \n"));
    InBuffer[0].Buffer = &cmdHeader;
    InBuffer[0].Size   = sizeof (cmdHeader);
    InBuffer[1].Buffer = &PCRIndex;
    InBuffer[1].Size   = sizeof (PCRIndex);
    InBuffer[2].Buffer = Digest->digest;
    InBuffer[2].Size   = sizeof (Digest->digest);

    OutBuffer[0].Buffer = &retHeader;
    OutBuffer[0].Size   = sizeof (retHeader);
    OutBuffer[1].Buffer = NewPCRValue->digest;
    OutBuffer[1].Size   = sizeof (NewPCRValue->digest);

    cmdHeader.Tag       = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdHeader.ParamSize = TPM_H2NL(sizeof (cmdHeader) 
                          + sizeof (PCRIndex) + sizeof (Digest->digest));

    cmdHeader.Ordinal = TPM_H2NL( TPM_ORD_Extend );
    PCRIndex          = TcgCommonH2NL( PCRIndex );

    return TcgCommonPassThrough( CallbackContext,
                                 sizeof (InBuffer) / sizeof (*InBuffer),
                                 InBuffer,
                                 sizeof (OutBuffer) / sizeof (*OutBuffer),
                                 OutBuffer);
}



EFI_STATUS EFIAPI TcgDxeStatusCheck(
    IN EFI_TCG_PROTOCOL                 *This,
    OUT TCG_EFI_BOOT_SERVICE_CAPABILITY *ProtocolCapability,
    OUT UINT32                          *TCGFeatureFlags,
    OUT EFI_PHYSICAL_ADDRESS            *EventLogLocation,
    OUT EFI_PHYSICAL_ADDRESS            *LastEvent )
{
    TCG_LOG_HOB   *TcgLog;
    TCG_PCR_EVENT *EventStart;
    UINTN         Index;

    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_PROTOCOL_GET_CAPABILITY_REQUEST | EFI_SOFTWARE_DXE_BS_DRIVER);
    if ( ProtocolCapability != NULL )
    {
        gBS->SetMem( ProtocolCapability, sizeof (*ProtocolCapability), 0 );
        ProtocolCapability->Size = sizeof (TCG_EFI_BOOT_SERVICE_CAPABILITY);
        ProtocolCapability->StructureVersion.Major = 1;     
        ProtocolCapability->StructureVersion.Minor = 2;     
        ProtocolCapability->StructureVersion.RevMajor = 0;
        ProtocolCapability->StructureVersion.RevMinor = 0;
        ProtocolCapability->ProtocolSpecVersion.Major = 1;  
        ProtocolCapability->ProtocolSpecVersion.Minor = 2;  
        ProtocolCapability->ProtocolSpecVersion.RevMajor = 0;
        ProtocolCapability->ProtocolSpecVersion.RevMinor = 0;
        ProtocolCapability->HashAlgorithmBitmap          = 1;    // SHA-1
        ProtocolCapability->TPMPresentFlag               = 1;   // TPM is present.
        ProtocolCapability->TPMDeactivatedFlag    = TcgDxe_Cap.deactivated;
    }

    if ( TCGFeatureFlags != NULL )
    {
        *TCGFeatureFlags = 0;
    }

    EventStart = (TCG_PCR_EVENT*)(UINTN)mTcgAcpiTableTemplate.LogStart;
    TcgLog     = (TCG_LOG_HOB*)EventStart;
    TcgLog--;

    if ( EventLogLocation != NULL )
    {
        *EventLogLocation
            = (EFI_PHYSICAL_ADDRESS)( UINTN ) mTcgAcpiTableTemplate.LogStart;
    }

    if ( LastEvent != NULL )
    {
        if ( TcgLog->EventNum == 0 )
        {
            *LastEvent = 0;
        }
        else {
            TcgLog->EventNum = (UINT32)GetLogEventCount( (TCG_PCR_EVENT_HDR*)(TcgLog + 1) );
            Index = TcgLog->EventNum;
            *LastEvent = (EFI_PHYSICAL_ADDRESS)( UINTN )FindNextLogLocation((TCG_PCR_EVENT_HDR *)mTcgAcpiTableTemplate.LogStart,
                            (TcgLog->EventNum - 1));
        }
    }

    return EFI_SUCCESS;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcgTcmDxeStatusCheck
//
// Description: Tcm Dxe status check function
//
// Input:       IN EFI_TCM_PROTOCOL                 *This,
//              OUT TCM_EFI_BOOT_SERVICE_CAPABILITY *ProtocolCapability,
//              OUT UINT32                          *TCGFeatureFlags,
//              OUT EFI_PHYSICAL_ADDRESS            *EventLogLocation,
//              OUT EFI_PHYSICAL_ADDRESS            *LastEvent
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
EFI_STATUS EFIAPI TcgTcmDxeStatusCheck(
    IN EFI_TCM_PROTOCOL                 *This,
    OUT TCM_EFI_BOOT_SERVICE_CAPABILITY *ProtocolCapability,
    OUT UINT32                          *TCGFeatureFlags,
    OUT EFI_PHYSICAL_ADDRESS            *EventLogLocation,
    OUT EFI_PHYSICAL_ADDRESS            *LastEvent )
{
    TCG_LOG_HOB   *TcgLog;
    TCM_PCR_EVENT *EventStart;
    UINTN         Index;

    if((IsTcmSupportType()) && (TcmBootVar == 1)){
        gBS->SetMem( ProtocolCapability, sizeof (TCM_EFI_BOOT_SERVICE_CAPABILITY), 0 );
        if ( TCGFeatureFlags != NULL )*TCGFeatureFlags = 0;
        if ( LastEvent != NULL )*LastEvent = 0;
        return EFI_UNSUPPORTED;
    }

    if ( ProtocolCapability != NULL )
    {
        gBS->SetMem( ProtocolCapability, sizeof (*ProtocolCapability), 0 );
        ProtocolCapability->Size = sizeof (TCG_EFI_BOOT_SERVICE_CAPABILITY);
        ProtocolCapability->StructureVersion.Major = 1;     // For TCM, the spec version might be 1.2 or 1.0
        ProtocolCapability->StructureVersion.Minor = 2;
        ProtocolCapability->StructureVersion.RevMajor = 0;
        ProtocolCapability->StructureVersion.RevMinor = 0;
        ProtocolCapability->ProtocolSpecVersion.Major = 1;  // For Protocol version, we still follow the UEFI TPM 1.2 protocol( Latest version is 1.22 )
        ProtocolCapability->ProtocolSpecVersion.Minor = 2;
        ProtocolCapability->ProtocolSpecVersion.RevMajor = 0;
        ProtocolCapability->ProtocolSpecVersion.RevMinor = 0;
        ProtocolCapability->HashAlgorithmBitmap          = 1;    // SHA-1
        ProtocolCapability->TPMPresentFlag               = 1;   // TPM is present.
        ProtocolCapability->TPMDeactivatedFlag    = TcgDxe_Cap.deactivated;
    }

    if ( TCGFeatureFlags != NULL )
    {
        *TCGFeatureFlags = 0;
    }

    EventStart = (TCM_PCR_EVENT*)(UINTN)mTcgAcpiTableTemplate.LogStart;
    TcgLog     = (TCG_LOG_HOB*)EventStart;
    TcgLog--;

    if ( EventLogLocation != NULL )
    {
        *EventLogLocation
            = (EFI_PHYSICAL_ADDRESS)( UINTN ) mTcgAcpiTableTemplate.LogStart;
    }

    if ( LastEvent != NULL )
    {
        TcgLog->EventNum = (UINT32)GetLogEventCount( (TCG_PCR_EVENT_HDR*)(TcgLog + 1) );
        if ( TcgLog->EventNum == 0 )
        {
            *LastEvent = 0;
        }
        else {
            Index = TcgLog->EventNum;

            do
            {
                *LastEvent = (EFI_PHYSICAL_ADDRESS)( UINTN ) EventStart;
                EventStart = (TCM_PCR_EVENT*)(UINTN)(
                    *LastEvent
                    + _TPM_STRUCT_PARTIAL_SIZE( TCM_PCR_EVENT, Event[  0] )
                    + EventStart->EventSize
                    );
            } while ( --Index > 0 );
        }
 
    }

    return EFI_SUCCESS;
}






EFI_STATUS EFIAPI TcgDxeHashAll(
    IN EFI_TCG_PROTOCOL *This,
    IN UINT8            *HashData,
    IN UINT64           HashDataLen,
    IN TCG_ALGORITHM_ID AlgorithmId,
    IN OUT UINT64       *HashedDataLen,
    IN OUT UINT8        **HashedDataResult )
{
    if ( AlgorithmId != TCG_ALG_SHA )
    {
        return EFI_UNSUPPORTED;
    }

    if ( *HashedDataResult == NULL || *HashedDataLen == 0 )
    {
        *HashedDataLen = sizeof (TCG_DIGEST);
        gBS->AllocatePool( EfiBootServicesData,
                           (UINTN)*HashedDataLen,
                           HashedDataResult );

        if ( *HashedDataResult == NULL )
        {
            return EFI_OUT_OF_RESOURCES;
        }
    }

    return SHA1HashAll(
                    This,
                    (VOID*)(UINTN)HashData,
                    (UINTN)HashDataLen,
                    *HashedDataResult);
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  TcmDxeHashAll 
//
// Description: TcmDxeHashAll function [SHA1]
//
// Input:           IN EFI_TCG_PROTOCOL *This,
//                  IN UINT8            *HashData,
//                  IN UINT64           HashDataLen,
//                  IN TCG_ALGORITHM_ID AlgorithmId,
//                  IN OUT UINT64       *HashedDataLen,
//                  IN OUT UINT8        **HashedDataResult
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
EFI_STATUS EFIAPI TcmDxeHashAll(
    IN EFI_TCM_PROTOCOL *This,
    IN UINT8            *HashData,
    IN UINT64           HashDataLen,
    IN TCG_ALGORITHM_ID AlgorithmId,
    IN OUT UINT64       *HashedDataLen,
    IN OUT UINT8        **HashedDataResult )
{

    if((IsTcmSupportType()) && (TcmBootVar == 1)){
         return EFI_UNSUPPORTED;
    }

    if ( AlgorithmId != TCG_ALG_SHA )
    {
        return EFI_UNSUPPORTED;
    }

    if ( *HashedDataResult == NULL || *HashedDataLen == 0 )
    {
        *HashedDataLen = sizeof (TCG_DIGEST);
        gBS->AllocatePool( EfiBootServicesData,
                           (UINTN)*HashedDataLen,
                           HashedDataResult );

        if ( *HashedDataResult == NULL )
        {
            return EFI_OUT_OF_RESOURCES;
        }
    }

    return SHA1HashAll(
                    This,
                    (VOID*)(UINTN)HashData,
                    (UINTN)HashDataLen,
                    *HashedDataResult);
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Name:  TcgDxeHashLogExtendEventTpm
//
// Description: TcgDxe common function to Hash, Log and Extend data using TPM
//
// Input:       *This
//              *HashData
//              HashDataLen
//              AlgorithmId,
//              *TCGLogData,
//              *EventNum,
//              *EventLogLastEntry
//
// Output:     EFI STATUS
//
// Modified:
//
// Referrals:  TcgCommonSha1Start, TcgCommonSha1Start, TcgCommonSha1CompleteExtend
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI TcgDxeHashLogExtendEventTpm(
    IN EFI_TCG_PROTOCOL      *This,
    IN EFI_PHYSICAL_ADDRESS  HashData,
    IN UINT64                HashDataLen,
    IN TCG_ALGORITHM_ID      AlgorithmId,
    IN OUT TCG_PCR_EVENT     *TCGLogData,
    IN OUT UINT32            *EventNum,
    OUT EFI_PHYSICAL_ADDRESS *EventLogLastEntry )
{
    EFI_STATUS           Status;
    UINT32               Sha1MaxBytes;
    TCG_DIGEST           NewPCRValue;
    TCG_DXE_PRIVATE_DATA *Private;

#if defined LOG_EV_EFI_ACTION && LOG_EV_EFI_ACTION == 0
    if(TCGLogData->EventType == EV_EFI_ACTION  && TCGLogData->PCRIndex != 7)
    {
        return EFI_SUCCESS;
    }
#endif

    Private = TCG_DXE_PRIVATE_DATA_FROM_THIS( This );

    Status = Private->TpmDevice->Init( Private->TpmDevice );

    if ( EFI_ERROR( Status ))
    {
         DEBUG((DEBUG_ERROR,"Private->TpmDevice Status = %r \n", Status));
        goto Exit;
    }

    Status = TcgCommonSha1Start( This, TCG_ALG_SHA, &Sha1MaxBytes );

    if ( EFI_ERROR( Status ))
    {
         DEBUG((DEBUG_ERROR,"TcgCommonSha1Start Status = %r \n", Status));
        goto Exit;
    }

    Status = TcgCommonSha1Update(
        This,
        (UINT8 *)HashData,
        (UINT32)HashDataLen,
        Sha1MaxBytes
        );

    if ( EFI_ERROR( Status ))
    {
         DEBUG((DEBUG_ERROR,"TcgCommonSha1Update Status = %r \n", Status));
        goto Exit;
    }

#if defined(TCG_DEBUG_MODE) && (TCG_DEBUG_MODE == 1)
    HashData    += (UINTN)(HashDataLen & ~63);
    HashDataLen &= 63;
#else
    HashData    += (HashDataLen & ~63);
    HashDataLen &= 63;
#endif

    Status = TcgCommonSha1CompleteExtend(
        This,
        (UINT8 *)HashData,
        (UINT32)HashDataLen,
        TCGLogData->PCRIndex,
        &TCGLogData->Digest,
        &NewPCRValue
        );

    if ( EFI_ERROR( Status ))
    {
         DEBUG((DEBUG_ERROR,"TcgCommonSha1CompleteExtend Status = %r \n", Status));
        goto Exit;
    }

    Status = TcgDxeLogEvent( This, TCGLogData, EventNum, 1 );
     DEBUG((DEBUG_INFO,"TcgDxeLogEvent Status = %r \n", Status));    

Exit:
    Private->TpmDevice->Close( Private->TpmDevice );
    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Name:  TcgDxeHashLogExtendEventTcm
//
// Description: TcgDxe common function to Hash, Log and Extend data using TPM
//
// Input:       *This
//              *HashData
//              HashDataLen
//              AlgorithmId,
//              *TCGLogData,
//              *EventNum,
//              *EventLogLastEntry
//
// Output:     EFI STATUS
//
// Modified:
//
// Referrals:  TcgCommonSha1Start, TcgCommonSha1Start, TcgCommonSha1CompleteExtend
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI TcgDxeHashLogExtendEventTcm(
    IN EFI_TCM_PROTOCOL      *This,
    IN EFI_PHYSICAL_ADDRESS  HashData,
    IN UINT64                HashDataLen,
    IN TCG_ALGORITHM_ID      AlgorithmId,
    IN OUT TCM_PCR_EVENT     *TCGLogData,
    IN OUT UINT32            *EventNum,
    OUT EFI_PHYSICAL_ADDRESS *EventLogLastEntry )
{
    EFI_STATUS           Status;
    UINT32               Sha1MaxBytes;
    TCM_DIGEST           NewPCRValue;
    TCM_DXE_PRIVATE_DATA *Private;

    Private = TCM_DXE_PRIVATE_DATA_FROM_THIS( This );

    if((IsTcmSupportType()) && (TcmBootVar == 1)){
        return EFI_UNSUPPORTED;
    }

    Status = TcgCommonSha1Start( This, TCG_ALG_SHA, &Sha1MaxBytes );

    if ( EFI_ERROR( Status ))
    {
        goto Exit;
    }

    Status = TcgCommonSha1Update(
        This,
        (UINT8 *)HashData,
        (UINT32)HashDataLen,
        Sha1MaxBytes
        );

    if ( EFI_ERROR( Status ))
    {
        goto Exit;
    }

    HashData    += (HashDataLen & ~63);
    HashDataLen &= 63;

    Status = TcmCommonSha1CompleteExtend(
        This,
        (UINT8 *)HashData,
        (UINT32)HashDataLen,
        TCGLogData->PCRIndex,
        &TCGLogData->Digest,
        &NewPCRValue
        );

    if ( EFI_ERROR( Status ))
    {
        goto Exit;
    }

    Status = TcmDxeLogEvent( This, TCGLogData, EventNum, 1 );

Exit:
    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  TcgDxeLogEvent
//
// Description: Logs TCG events in DXE
//
// Input:               IN EFI_TCG_PROTOCOL *This,
//                      IN TCG_PCR_EVENT    *TCGLogData,
//                      IN OUT UINT32       *EventNumber,
//                      IN UINT32           Flags
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
EFI_STATUS EFIAPI TcgDxeLogEvent(
    IN EFI_TCG_PROTOCOL *This,
    IN TCG_PCR_EVENT    *TCGLogData,
    IN OUT UINT32       *EventNumber,
    IN UINT32           Flags )
{
    EFI_STATUS           Status;
    TCG_LOG_HOB          *TcgLog;
    TCG_DXE_PRIVATE_DATA *Private;
    TCG_DIGEST           NewPCR;
    UINTN               NextLocation; 

    Private = TCG_DXE_PRIVATE_DATA_FROM_THIS( This );

#if defined LOG_EV_EFI_ACTION && LOG_EV_EFI_ACTION == 0
    if(TCGLogData->EventType == EV_EFI_ACTION  && TCGLogData->PCRIndex != 7)
    {
        return EFI_SUCCESS;
    }
#endif

    Status = EFI_SUCCESS;
    
    if(TCGLogData->EventSize == 0)return Status;

    
    if ( !(Flags & (UINT32)(0x1)))
    {
         DEBUG((DEBUG_INFO, "Flags = %x \n", Flags));
        Status = Private->TpmDevice->Init( Private->TpmDevice );

         DEBUG((DEBUG_INFO, "Private->TpmDevice->Init = %r \n", Status));

        if ( !EFI_ERROR( Status ))
        {          
           Status = TcgDxeCommonExtend(
            This,
            TCGLogData->PCRIndex,
            &TCGLogData->Digest,
            &NewPCR
            );
            
             DEBUG((DEBUG_INFO, "TcgDxeCommonExtend Status = %r \n", Status));
        }
        Private->TpmDevice->Close( Private->TpmDevice );
    }
        
    if ( !TcgDxe_Cap.deactivated )
    {
        TcgLog = (TCG_LOG_HOB*)(UINTN)mTcgAcpiTableTemplate.LogStart;
        TcgLog--;

        if ( !EFI_ERROR( Status ))
        {            
            NextLocation = FindNextLogLocation((TCG_PCR_EVENT_HDR*)(TcgLog + 1), (UINT32)GetLogEventCount( (TCG_PCR_EVENT_HDR*)(TcgLog + 1) ) );
                        
            if((NextLocation + TCGLogData->EventSize + sizeof(TCG_PCR_EVENT_HDR)-1)>=
                    (mTcgAcpiTableTemplate.LogStart +  TcgLog->TableMaxSize))
            {
                return EFI_OUT_OF_RESOURCES;
            }
            
            //printbuffer((UINT8 *)TCGLogData, sizeof(PEI_EFI_POST_CODE));
            
            TcgLog->TableSize = (UINT32)(NextLocation - (UINTN)(TcgLog + 1) );
            Status = TcgCommonLogEvent(
                This,
                (TCG_PCR_EVENT*)(NextLocation),
                &TcgLog->TableSize,
                TcgLog->TableMaxSize,
                TCGLogData, 0);

             DEBUG((DEBUG_INFO, "TcgCommonLogEvent Status = %r \n", Status));
             
             //printbuffer((UINT8 *)TcgLog + 1, 0x100);

            if ( !EFI_ERROR( Status ))
            {
                TcgLog->EventNum = (UINT32)GetLogEventCount( (TCG_PCR_EVENT_HDR*)(TcgLog + 1) );
                *EventNumber = TcgLog->EventNum;
            }
        }
    }
    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  TcmDxeLogEvent
//
// Description: Logs TCM events in DXE
//
// Input:               IN EFI_TCG_PROTOCOL *This,
//                      IN TCG_PCR_EVENT    *TCGLogData,
//                      IN OUT UINT32       *EventNumber,
//                      IN UINT32           Flags
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
EFI_STATUS EFIAPI TcmDxeLogEvent(
    IN EFI_TCM_PROTOCOL *This,
    IN TCM_PCR_EVENT    *TCGLogData,
    IN OUT UINT32       *EventNumber,
    IN UINT32           Flags )
{
    EFI_STATUS           Status;
    TCG_LOG_HOB          *TcgLog;
    TCM_DXE_PRIVATE_DATA *Private;
    TCM_DIGEST           NewPCR;

    Private = TCM_DXE_PRIVATE_DATA_FROM_THIS( This );

    if((IsTcmSupportType()) && (TcmBootVar == 1)){
        return EFI_UNSUPPORTED;
    }

    Status = EFI_SUCCESS;

    if ( !(Flags & 1))
    {
        if ( !EFI_ERROR( Status ))
        {
            Status = TcmCommonExtend(
                This,
                TCGLogData->PCRIndex,
                &TCGLogData->Digest,
                &NewPCR);
        }
    }

    if ( !TcgDxe_Cap.deactivated )
    {
        TcgLog = (TCG_LOG_HOB*)(UINTN)mTcgAcpiTableTemplate.LogStart;
        TcgLog--;

        if ( !EFI_ERROR( Status ))
        {
            Status = TcmCommonLogEvent(
                This,
                (TCM_PCR_EVENT*)(TcgLog + 1),
                &TcgLog->TableSize,
                TcgLog->TableMaxSize,
                TCGLogData
                );

            if ( !EFI_ERROR( Status ))
            {
                TcgLog->EventNum = (UINT32)GetLogEventCount( (TCG_PCR_EVENT_HDR*)(TcgLog + 1) );
                *EventNumber = TcgLog->EventNum;
            }
        }
    }
    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  TcgDxePassThroughToTcm
//
// Description: Helper function for TCM transmit function
//
// Input:               IN EFI_TCM_PROTOCOL *This,
//                      IN UINT32           TpmInputParamterBlockSize,
//                      IN UINT8            *TpmInputParamterBlock,
//                      IN UINT32           TpmOutputParameterBlockSize,
//                      IN UINT8            *TpmOutputParameterBlock
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
EFI_STATUS EFIAPI TcgDxePassThroughToTcm(
    IN EFI_TCM_PROTOCOL *This,
    IN UINT32           TpmInputParamterBlockSize,
    IN UINT8            *TpmInputParamterBlock,
    IN UINT32           TpmOutputParameterBlockSize,
    IN UINT8            *TpmOutputParameterBlock )
{
    TPM_TRANSMIT_BUFFER InBuffer[1], OutBuffer[1];
    EFI_STATUS Status;
    TCM_DXE_PRIVATE_DATA              *Private;

    //some applications might not set init command before making this call.
    //Just set init commands[locality zero for them]
    Private = TCM_DXE_PRIVATE_DATA_FROM_THIS( This );

    if((IsTcmSupportType()) && (TcmBootVar == 1)){
        return EFI_UNSUPPORTED;
    }

    InBuffer[0].Buffer  = TpmInputParamterBlock;
    InBuffer[0].Size    = TpmInputParamterBlockSize;
    OutBuffer[0].Buffer = TpmOutputParameterBlock;
    OutBuffer[0].Size   = TpmOutputParameterBlockSize;

    Status = TcmCommonPassThrough(
        This,
        sizeof (InBuffer) / sizeof (*InBuffer),
        InBuffer,
        sizeof (OutBuffer) / sizeof (*OutBuffer),
        OutBuffer
        );

    return EFI_SUCCESS;
}


EFI_STATUS EFIAPI TcgDxeHashLogExtendEvent(
    IN EFI_TCG_PROTOCOL      *This,
    IN EFI_PHYSICAL_ADDRESS  HashData,
    IN UINT64                HashDataLen,
    IN TCG_ALGORITHM_ID      AlgorithmId,
    IN OUT TCG_PCR_EVENT     *TCGLogData,
    IN OUT UINT32            *EventNumber,
    OUT EFI_PHYSICAL_ADDRESS *EventLogLastEntry )
{
    EFI_STATUS          Status;
    UINT64              DigestSize;
    UINT8               *HashResult;
    TCG_DXE_PRIVATE_DATA *Private;

    Private = TCG_DXE_PRIVATE_DATA_FROM_THIS( This );

    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_PROTOCOL_HASH_LOG_EXTEND_REQUEST | EFI_SOFTWARE_DXE_BS_DRIVER);
#if defined LOG_EV_EFI_ACTION && LOG_EV_EFI_ACTION == 0
    if(TCGLogData->EventType == EV_EFI_ACTION && TCGLogData->PCRIndex != 7)
    {
        return EFI_SUCCESS;
    }
#endif

    DigestSize = sizeof (TCGLogData->Digest);
    HashResult = TCGLogData->Digest.digest;
    Status     = TcgDxeHashAll(
        This,
        (UINT8 *)HashData,
        HashDataLen,
        AlgorithmId,
        &DigestSize,
        &HashResult
        );

     DEBUG((DEBUG_INFO, "TcgDxeHashAll Status = %r \n", Status));


    if ( !EFI_ERROR( Status ))
    {
        Status = TcgDxeLogEvent(
            This,
            TCGLogData,
            EventNumber,
            0
            );

         DEBUG((DEBUG_INFO, "TcgDxeLogEvent Status = %r \n", Status));
    }else{
        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TCG_PROTOCOL_HASH_LOG_EXTEND_EVENT_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
    }

    return Status;
}


static TCG_DXE_PRIVATE_DATA mTcgDxeData = {
    {
        TcgDxeStatusCheck,
        TcgDxeHashAll,
        TcgDxeLogEvent,
        TcgDxePassThroughToTpm,
        TcgDxeHashLogExtendEvent
    },
    NULL
};


static TCM_DXE_PRIVATE_DATA mTcmDxeData = {
    {
        TcgTcmDxeStatusCheck,
        TcmDxeHashAll,
        TcmDxeLogEvent,
        TcgDxePassThroughToTcm,
        TcgDxeHashLogExtendEventTcm       
    },
    NULL
};

//***********************************************************************
//                      MOR RELATED FUNCTIONS
//***********************************************************************

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
  for (Index = 0; Index < (MemoryMapSize / DescriptorSize); Index++) {
      switch (MemoryMap->Type){
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
    EFI_GUID   MorUefiGuid = MEMORY_ONLY_RESET_CONTROL_GUID;
    UINT8      mor         = 0;
    UINTN      size        = sizeof(mor);
    EFI_STATUS Status;

   
    Status = gRT->GetVariable( UefiMor, &MorUefiGuid,
                               NULL, &size, &mor );

    if(EFI_ERROR(Status))return;

    if ( (mor & 1)!=0 )
    {

        //clear memory
        DEBUG((DEBUG_INFO,"MOR: before Clear memory"));
#if !(defined(TCG_SKIP_MOR_FULL) && TCG_SKIP_MOR_FULL == 1)
        Status = OverwriteSystemMemory();
        DEBUG((DEBUG_INFO,"Status: %r", Status));
        TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_MOR_REQUEST_ACK_EXECUTED | EFI_SOFTWARE_DXE_BS_DRIVER);
#endif
        DEBUG((DEBUG_INFO,"MOR: After Clear memory"));
	}	
}




//**********************************************************************
//                      MOR FUNCTIONS END
//**********************************************************************
//**********************************************************************
//<AMI_PHDR_START>
//
// Name: OnAcpiInstalled
//
// Description: Adds Tcg Table to Acpi Tables
//
// Input:       IN      EFI_EVENT ev
//              IN      Callback Context *ctx
//
// Output:      Device path size
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS OnAcpiInstalled(
    IN EFI_EVENT ev,
    IN VOID      *ctx )
{
    EFI_STATUS                Status;
    EFI_ACPI_SUPPORT_PROTOCOL *acpi;
    UINTN                     handle = 0;
#if defined ACPI_MODULE_VER && ACPI_MODULE_VER < 120     
    UINT8                     OemTblId[8] = CONVERT_TO_STRING(T_ACPI_OEM_TBL_ID);
    UINT8                     OemId[6]    = CONVERT_TO_STRING(T_ACPI_OEM_ID);
#endif
    

    DEBUG((DEBUG_INFO, "Adding TCG ACPI table...\n"));
    Status = gBS->LocateProtocol( &gEfiAcpiSupportGuid, NULL, &acpi );

    if ( EFI_ERROR( Status ))
    {
        return EFI_ABORTED;
    }
    
    mTcgAcpiTableTemplate.Header.Revision = TCG_TBL_REV;

#if defined ACPI_MODULE_VER && ACPI_MODULE_VER > 110   
    
    CopyMem (&mTcgAcpiTableTemplate.Header.OemId,
            PcdGetPtr (PcdAcpiDefaultOemId),
            sizeof (mTcgAcpiTableTemplate.Header.OemId));
    
    mTcgAcpiTableTemplate.Header.OemTableId = PcdGet64 (PcdAcpiDefaultOemTableId);
#else
    gBS->CopyMem(&mTcgAcpiTableTemplate.Header.OemTableId, OemTblId, 8);
    gBS->CopyMem(&mTcgAcpiTableTemplate.Header.OemId, OemId, 6);
#endif
    mTcgAcpiTableTemplate.Header.OemRevision     = TCG_TBL_OEM_REV;
    mTcgAcpiTableTemplate.Header.CreatorId       = TCG_CREATOR_ID;
    mTcgAcpiTableTemplate.Header.CreatorRevision = TCG_CREATOR_REVISION;
    mTcgAcpiTableTemplate.PlatformClass          = TCG_PLATFORM_CLASS;
    
    if( TCG_PLATFORM_CLASS == 0 )
    {
        mTcgAcpiTableTemplate.Header.Length = 0x32;
    }else{
        mTcgAcpiTableTemplate.TcgSpecRev = 0x102;
    }
    
    Status = acpi->SetAcpiTable( acpi, &mTcgAcpiTableTemplate, TRUE,
                                 EFI_ACPI_TABLE_VERSION_ALL,
                                 &handle );
	
    gBS->CloseEvent(ev);
    return Status;
}


EFI_EVENT                   ev1;
static VOID                 *reg1;


EFI_STATUS
EFIAPI SetTcgAcpiTable()
{
   EFI_STATUS                Status;
   EFI_ACPI_SUPPORT_PROTOCOL *acpi;
   UINTN                     handle = 0;

    DEBUG((DEBUG_INFO, "SetTcgAcpiTable....\n"));
   Status = gBS->LocateProtocol( &gEfiAcpiSupportGuid, NULL, &acpi );

   if(EFI_ERROR(Status)){
    
      Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                                   EFI_TPL_CALLBACK,
                                   OnAcpiInstalled,
                                   &reg1,
                                   &ev1 );

      ASSERT( !EFI_ERROR( Status ));
      Status = gBS->RegisterProtocolNotify( &gEfiAcpiSupportGuid, ev1, &reg1 );
      return Status;
   }

   mTcgAcpiTableTemplate.Header.Revision = TCG_TBL_REV;
   CopyMem( mTcgAcpiTableTemplate.Header.OemId,TCG_OEMID,
           sizeof(mTcgAcpiTableTemplate.Header.OemId));

    mTcgAcpiTableTemplate.Header.OemTableId      = TCG_TBL_OEM_ID;
    mTcgAcpiTableTemplate.Header.OemRevision     = TCG_TBL_OEM_REV;
    mTcgAcpiTableTemplate.Header.CreatorId       = TCG_CREATOR_ID;
    mTcgAcpiTableTemplate.Header.CreatorRevision = TCG_CREATOR_REVISION;
    mTcgAcpiTableTemplate.PlatformClass          = TCG_PLATFORM_CLASS;
  

    if( TCG_PLATFORM_CLASS == 0 )
    {
        mTcgAcpiTableTemplate.Header.Length = 0x32;
    }else{
        mTcgAcpiTableTemplate.TcgSpecRev = 0x102;
    }

    Status = acpi->SetAcpiTable( acpi, &mTcgAcpiTableTemplate, TRUE,
                                 EFI_ACPI_TABLE_VERSION_ALL,
                                 &handle );

    return Status;
}


static EFI_STATUS CopyLogToAcpiNVS(
    void )
{
    EFI_STATUS Status;
    TCG_LOG_HOB                       *TcgLog = NULL;
    void**                                 DummyPtr;

    TcgLog = (TCG_LOG_HOB*)                   LocateATcgHob(
        gST->NumberOfTableEntries,
        gST->ConfigurationTable,
        &gEfiPeiTcgLogHobGuid );

    DummyPtr = &TcgLog;

    if ( *DummyPtr == NULL )
    {
        return EFI_NOT_FOUND;
    }

    Status = gBS->AllocatePages(
        AllocateMaxAddress,
        EfiACPIMemoryNVS,
        EFI_SIZE_TO_PAGES( mTcgAcpiTableTemplate.LogMaxLength + sizeof (*TcgLog)),
        (UINT64*)(UINTN)&mTcgAcpiTableTemplate.LogStart
        );

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    gBS->SetMem(
        (VOID*)((UINTN)mTcgAcpiTableTemplate.LogStart),
        (UINTN)mTcgAcpiTableTemplate.LogMaxLength,
        0x00 // Clean up this region with this value.
        );

#if TCG_PLATFORM_CLASS == 0
    TcgLog->TableMaxSize = mTcgAcpiTableTemplate.LogMaxLength;
#else
    TcgLog->TableMaxSize = (UINT32)mTcgAcpiTableTemplate.LogMaxLength;
#endif
    
    if(TcgDxe_Cap.deactivated){
            TcgLog->EventNum = 0;
            TcgLog->TableSize = 0;

            gBS->CopyMem(
            (VOID*)(UINTN)mTcgAcpiTableTemplate.LogStart,
            TcgLog,
            sizeof (TCG_LOG_HOB));

        mTcgAcpiTableTemplate.LogStart += sizeof (*TcgLog);
    }else{
       gBS->CopyMem(
        (VOID*)(UINTN)mTcgAcpiTableTemplate.LogStart,
        TcgLog,
        TcgLog->TableSize + sizeof (*TcgLog)
        );
        mTcgAcpiTableTemplate.LogStart += sizeof (*TcgLog);
    }

    Status = SetTcgAcpiTable();

    return Status;
}


/*
VOID InsertTailList(
    EFI_LIST_ENTRY *ListHead,
    EFI_LIST_ENTRY *Entry )
{
    EFI_LIST_ENTRY *_ListHead;
    EFI_LIST_ENTRY *_BackLink;

    _ListHead              = ListHead;
    _BackLink              = _ListHead->BackLink;
    Entry->ForwardLink     = _ListHead;
    Entry->BackLink        = _BackLink;
    _BackLink->ForwardLink = Entry;
    _ListHead->BackLink    = Entry;
}*/

typedef struct _TCG_DXE_FWVOL_LIST
{
    EFI_LIST_ENTRY Link;
    EFI_HANDLE FvHandle;
} TCG_DXE_FWVOL_LIST;

static EFI_LIST_ENTRY mMeasuredFvs = {
    &mMeasuredFvs,
    &mMeasuredFvs
};

static EFI_STATUS AddFvToMeasuredFvList(
    EFI_HANDLE FvHandle )
{
    TCG_DXE_FWVOL_LIST                *NewEntry=NULL;
    EFI_STATUS                        Status;

    Status = gBS->AllocatePool( EfiBootServicesData, sizeof (*NewEntry), &NewEntry );

    if ( NewEntry == NULL || EFI_ERROR(Status))
    {
        return Status;
    }

    NewEntry->FvHandle = FvHandle;
    InsertTailList( &mMeasuredFvs, &NewEntry->Link );
    return EFI_SUCCESS;
}

static EFI_STATUS HashAllFilesInFv(
    IN SHA1_CTX        *Sha1Ctx,
    IN EFI_FIRMWARE_VOLUME2_PROTOCOL  *FwVol,
    IN EFI_FV_FILETYPE FileType )
{
    EFI_STATUS Status;
    VOID                              *KeyBuffer = NULL;
    EFI_GUID FileName;
    EFI_FV_FILE_ATTRIBUTES FileAttr;
    UINTN FileSize;
    VOID                              *FileBuffer;
    UINT32 AuthStat;


    Status = gBS->AllocatePool( EfiBootServicesData, FwVol->KeySize, KeyBuffer );

    if ( KeyBuffer != NULL )
    {
        gBS->SetMem( KeyBuffer, FwVol->KeySize, 0 );
    }

    if ( KeyBuffer == NULL || EFI_ERROR(Status))
    {
        return EFI_OUT_OF_RESOURCES;
    }

    do
    {
        Status = FwVol->GetNextFile(
            FwVol,
            KeyBuffer,
            &FileType,
            &FileName,
            &FileAttr,
            &FileSize
            );

        if ( !EFI_ERROR( Status ))
        {
            FileBuffer = NULL;
            Status     = FwVol->ReadFile(
                FwVol,
                &FileName,
                &FileBuffer,
                &FileSize,
                &FileType,
                &FileAttr,
                &AuthStat
                );
            ASSERT( !EFI_ERROR( Status ));

            SHA1Update(Sha1Ctx, FileBuffer, (u32)FileSize );
            if(FileBuffer!=NULL)
            {
                gBS->FreePool( FileBuffer );
            }
        }
    } while ( !EFI_ERROR( Status ));

    if(KeyBuffer != NULL)
    {
        gBS->FreePool( KeyBuffer );
    }
    return EFI_SUCCESS;
}



static EFI_STATUS MeasureFv(
    IN EFI_TCG_PROTOCOL *This,
    IN EFI_HANDLE       FvHandle )
{
    EFI_STATUS Status;
    EFI_FIRMWARE_VOLUME2_PROTOCOL      *FwVol;
    EFI_LIST_ENTRY                    *Link;
    TCG_DXE_FWVOL_LIST    *FwVolList;
    SHA1_CTX                                Sha1Ctx;
    TCG_DIGEST                          *FvDigest;
    EFI_TCG_PCR_EVENT TcgEvent;
    UINT32 EventNum;

    for ( Link = mMeasuredFvs.ForwardLink;
          Link != &mMeasuredFvs;
          Link = Link->ForwardLink )
    {
        FwVolList = _CR( Link, TCG_DXE_FWVOL_LIST, Link );

        if ( FvHandle == FwVolList->FvHandle )
        {
            return EFI_SUCCESS;
        }
    }

    Status = AddFvToMeasuredFvList( FvHandle );

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    Status = gBS->HandleProtocol(
        FvHandle,
        &gEfiFirmwareVolume2ProtocolGuid,
        &FwVol
        );
    ASSERT( !EFI_ERROR( Status ));

    SHA1Init(&Sha1Ctx );
    Status = HashAllFilesInFv( &Sha1Ctx, FwVol, EFI_FV_FILETYPE_DRIVER );

    if ( EFI_ERROR( Status ))
    {
        goto Exit;
    }
    Status = HashAllFilesInFv( &Sha1Ctx, FwVol, EFI_FV_FILETYPE_APPLICATION );

    if ( EFI_ERROR( Status ))
    {
        goto Exit;
    }
    SHA1Final((unsigned char *)&FvDigest->digest, &Sha1Ctx);

    TcgEvent.Header.PCRIndex      = PCRi_OPROM_CODE;
    TcgEvent.Header.EventType     = EV_EVENT_TAG;
    TcgEvent.Event.Tagged.EventID = EV_ID_OPROM_EXECUTE;
    TcgEvent.Event.Tagged.EventSize
        = sizeof (TcgEvent.Event.Tagged.EventData.OptionRomExecute);
    TcgEvent.Header.EventDataSize
        = _TPM_STRUCT_PARTIAL_SIZE( struct _EFI_TCG_EV_TAG, EventData )
        + TcgEvent.Event.Tagged.EventSize;

    TcgEvent.Event.Tagged.EventData.OptionRomExecute.PFA      = 0;
    TcgEvent.Event.Tagged.EventData.OptionRomExecute.Reserved = 0;
    TcgEvent.Event.Tagged.EventData.OptionRomExecute.Hash     = *FvDigest;

    Status = TcgDxeHashLogExtendEvent(
        This,
        (UINTN)&TcgEvent.Event,
        TcgEvent.Header.EventDataSize,
        TCG_ALG_SHA,
        (TCG_PCR_EVENT*)&TcgEvent,
        &EventNum,
        0
        );

Exit:
    return Status;
}




static
VOID
EFIAPI OnFwVolInstalled(
    IN EFI_EVENT Event,
    IN VOID      *Context )
{
    EFI_STATUS Status;
    EFI_HANDLE                        *Handles;
    UINTN NumHandles;

    Handles    = NULL;
    NumHandles = 0;
    Status     = gBS->LocateHandleBuffer(
        ByRegisterNotify,
        NULL,
        *(VOID**)Context,
        &NumHandles,
        &Handles
        );

    ASSERT(!EFI_ERROR( Status ));

    while (!EFI_ERROR( Status ) && NumHandles > 0 )
    {
        NumHandles--;
        Status = MeasureFv( &mTcgDxeData.TcgServiceProtocol,
                            Handles[NumHandles] );
    }

    if ( Handles != NULL )
    {
        gBS->FreePool( Handles );
    }
}

static EFI_STATUS MonitorFvs(
    void )
{
    EFI_STATUS Status;
    EFI_EVENT Event;
    static VOID                       *RegFwVol;

    Status = gBS->CreateEvent(
        EFI_EVENT_NOTIFY_SIGNAL,
        EFI_TPL_DRIVER,
        OnFwVolInstalled,
        (VOID*)&RegFwVol,
        &Event
        );
    ASSERT( !EFI_ERROR( Status ));

    Status = gBS->RegisterProtocolNotify(
        &gEfiFirmwareVolume2ProtocolGuid,
        Event,
        &RegFwVol
        );
    ASSERT( !EFI_ERROR( Status ));

    return Status;
}


//*******************************************************************************
//<AMI_PHDR_START>
//
// Procedure:   FindAndMeasureDxeFWVol
//
// Description: 
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
//******************************************************************************
EFI_STATUS FindAndMeasureDxeFWVol()
{
    PEI_EFI_POST_CODE       ev;
    EFI_STATUS              Status;

    UINTN                         Size;
    void                          *Buffer = NULL;
    UINTN                         BufferSize=0x10000;
    EFI_TCG_PROTOCOL			  *TcgProtocol;
    VOID                          *HobStart;
    UINTN                          TableEntries;
    EFI_PEI_HOB_POINTERS           FirmwareVolumeHob;
    BOOLEAN                        Found=FALSE;
    UINTN                          i=0;
    SHA1_CTX                       Sha1Ctx;
    TCG_DIGEST                    Sha1Digest;
    TCG_DIGEST                    OutSha1Digest;
    UINTN                         HashSize=0;
    BOOLEAN                       BreakLoop = FALSE;
    TCG_DXE_PRIVATE_DATA          *Private;
    UINT32                         EventNumber;
   
    TableEntries = gST->NumberOfTableEntries;

    while ( TableEntries > 0 )
    {
        TableEntries--;

        if ((!CompareMem(
                 &gST->ConfigurationTable[TableEntries].VendorGuid,
                 &gEfiHobListGuid, sizeof(EFI_GUID))))
        {            
            HobStart = gST->ConfigurationTable[TableEntries].VendorTable;
            FirmwareVolumeHob.Raw = GetHob (EFI_HOB_TYPE_FV, HobStart);
            if (FirmwareVolumeHob.Header->HobType != EFI_HOB_TYPE_FV) {
                 continue;
            }
            break;   
        }
    }        
   
    for (Status = EFI_NOT_FOUND; EFI_ERROR (Status);) {
        if (TCGEND_OF_HOB_LIST (FirmwareVolumeHob)) {
          return EFI_NOT_FOUND;
        }

        if (TCGGET_HOB_TYPE (FirmwareVolumeHob) == EFI_HOB_TYPE_FV) {
        if ((((UINT64)FirmwareVolumeHob.FirmwareVolume->BaseAddress)\
                < (UINT64)NVRAM_ADDRESS ) || 
                ((UINT64)FirmwareVolumeHob.FirmwareVolume->BaseAddress) == FV_MAIN_BASE)
            {
                Found = TRUE;
                break;
            }
        }
        
        FirmwareVolumeHob.Raw = TCGGET_NEXT_HOB (FirmwareVolumeHob);
    }

    if(Found== FALSE)return EFI_NOT_FOUND;

    DEBUG((DEBUG_INFO,"TcgDxe:: Found Volume: Base = %x Length = %x",\
         FirmwareVolumeHob.FirmwareVolume->BaseAddress,\
         FirmwareVolumeHob.FirmwareVolume->Length));

    Status = gBS->AllocatePool(
                    EfiBootServicesData, 
                    BufferSize, 
					&Buffer);

    if(EFI_ERROR(Status) || Buffer == NULL) return EFI_OUT_OF_RESOURCES;
  
     Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid,\
                                                 NULL, &TcgProtocol);
     if(EFI_ERROR(Status)){
         gBS->FreePool(Buffer);
         return Status;
      }

     Size = (UINTN)FirmwareVolumeHob.FirmwareVolume->Length;
     
     if(Size > TCG_SIZE)
     {
        Size = TCG_SIZE;
     }
         
     SHA1Init(&Sha1Ctx );
         
     do{
         if(Size >BufferSize){
            gBS->CopyMem(Buffer, (UINT8 *)(EFI_PHYSICAL_ADDRESS)(FirmwareVolumeHob.FirmwareVolume->BaseAddress + i),\
                    BufferSize);
            HashSize = BufferSize;
         }else{
             gBS->CopyMem(Buffer, (UINT8 *)(EFI_PHYSICAL_ADDRESS)(FirmwareVolumeHob.FirmwareVolume->BaseAddress + i),\
                         Size);
             HashSize=Size;
             BreakLoop = TRUE;
        }
                   
        SHA1Update(&Sha1Ctx, Buffer, (u32)HashSize );
             
        if(BreakLoop == TRUE){
            SHA1Final((unsigned char *)&Sha1Digest.digest, &Sha1Ctx);
            
            Private = TCG_DXE_PRIVATE_DATA_FROM_THIS( TcgProtocol );    
                 
            Status = Private->TpmDevice->Init( Private->TpmDevice );
              
            Status = TcgDxeCommonExtend (
                         (void *)TcgProtocol,
                         PCRi_CRTM_AND_POST_BIOS,
                         &Sha1Digest,
                         &OutSha1Digest
                        );

               
            Status = Private->TpmDevice->Close( Private->TpmDevice );
            
            break;
        }
             
        i+=HashSize;
        Size-=HashSize;
     }while(Size>0);
      
      ev.Header.PCRIndex      = PCRi_CRTM_AND_POST_BIOS;
      ev.Header.EventType     = EV_POST_CODE;
      ev.Header.EventDataSize = sizeof (EFI_TCG_EV_POST_CODE);
      ev.Event.PostCodeAddress = \
                    (EFI_PHYSICAL_ADDRESS)FirmwareVolumeHob.FirmwareVolume->BaseAddress;

  #if defined x64_TCG &&  x64_TCG == 1
      ev.Event.PostCodeLength = FirmwareVolumeHob.FirmwareVolume->Length;
  #else
      ev.Event.PostCodeLength = (UINTN)FirmwareVolumeHob.FirmwareVolume->Length;
  #endif
      gBS->CopyMem(&ev.Header.Digest, &Sha1Digest.digest, sizeof(TCG_DIGEST));
      Status = TcgProtocol->LogEvent(TcgProtocol, (TCG_PCR_EVENT *)&ev, &EventNumber,0x01);
      if(Buffer!=NULL){
          gBS->FreePool(Buffer);
      }
      return Status;
}


EFI_STATUS
EFIAPI TcgDxeEntry(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS Status;
    BOOLEAN Support  = FALSE;
    TPM_GetCapabilities_Input   cmdGetCap;
   
                
    Status = gBS->LocateProtocol(
                &gEfiTpmDeviceProtocolGuid,
                NULL,
                &mTcgDxeData.TpmDevice);    

    Status = gBS->LocateProtocol(
        &gEfiTpmDeviceProtocolGuid,
        NULL,
        &mTcmDxeData.TpmDevice); 
  
    if ( EFI_ERROR( Status )){
        return Status;
    }

    Status = CopyLogToAcpiNVS( );
 
    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    cmdGetCap.Tag         = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdGetCap.ParamSize   = TPM_H2NL( sizeof (cmdGetCap));

    if(IsTcmSupportType()){
        cmdGetCap.CommandCode = TPM_H2NL( TCM_ORD_GetCapability );
        cmdGetCap.CommandCode = TPM_H2NL( TCM_ORD_GetCapability );
        cmdGetCap.caparea     = TPM_H2NL( TPM_CAP_FLAG );   
    }else{
        cmdGetCap.CommandCode = TPM_H2NL( TPM_ORD_GetCapability );
        cmdGetCap.CommandCode = TPM_H2NL( TPM_ORD_GetCapability );
        cmdGetCap.caparea     = TPM_H2NL( TPM_CAP_FLAG );
    }

    cmdGetCap.subCapSize  = TPM_H2NL( 4 ); // subCap is always 32bit long
    cmdGetCap.subCap      = TPM_H2NL( TPM_CAP_FLAG_PERMANENT );

    if(IsTcmSupportType()){
          Status = TcgDxePassThroughToTcm( &mTcmDxeData.TcgServiceProtocol,
                                           sizeof (cmdGetCap),
                                           (UINT8*)&cmdGetCap,
                                           sizeof (TPM_Capabilities_PermanentFlag),
                                           (UINT8*)&TcgDxe_Cap );
     }else{
            Status = TcgDxePassThroughToTpm( &mTcgDxeData.TcgServiceProtocol,
                                         sizeof (cmdGetCap),
                                         (UINT8*)&cmdGetCap,
                                         sizeof (TPM_Capabilities_PermanentFlag),
                                         (UINT8*)&TcgDxe_Cap );
     }

    if ( TPM_H2NL(TcgDxe_Cap.RetCode)!=0)
    {
        return EFI_SUCCESS;
    }

    Support = IsTcmSupportType();

    ReadMORValue();

    if(!Support){
        Status = gBS->InstallMultipleProtocolInterfaces(
                   &ImageHandle,
                   &gEfiTcgProtocolGuid,
                   &mTcgDxeData.TcgServiceProtocol,
                   NULL);
        if(EFI_ERROR(Status)){
            TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MINOR, AMI_SPECIFIC_TPM_ERR_TCG2_PROTOCOL_NOT_INSTALLED_TPM_1_2_DEVICE_FOUND | EFI_SOFTWARE_DXE_BS_DRIVER);
        }
        return Status;
    }else{

        TcmOSTransition();
        return gBS->InstallMultipleProtocolInterfaces(
                   &ImageHandle,
                   &gEfiTcgProtocolGuid,
                   &mTcmDxeData.TcgServiceProtocol,
                   NULL);
    }

}
