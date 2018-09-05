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

   TcgPei.c

   Abstract:

   PEIM that provides TCG services

   --*/
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/TcgPei/TcgTcmPeiAfterMem.c 1     10/08/13 12:03p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:03p $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/TcgPei/TcgTcmPeiAfterMem.c $
//
// 1     10/08/13 12:03p Fredericko
// Initial Check-In for Tpm-Next module
//
// 2     10/03/13 1:57p Fredericko
//
// 1     7/10/13 5:51p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
//
// 2     1/12/12 12:12p Fredericko
// Remove unused function declaration.
//
// 1     12/12/11 12:40p Fredericko
// [TAG]        EIP76865
// [Category]   Improvement
// [Description]    Dual Support for TCM and TPM. System could hang in TXT
// if txt is enabled in setup
// [Files]          AmiTcgPlatfompeilib.c, AmiTcgPlatformPpi.cif,
// AmiTcgPlatformPpi.h, AmiTcgPlatformProtocol.cif,
// AmiTcgPlatformProtocol.h,
// EMpTcmPei.c, TcgDxe.cif, TcgPei.cif, TcgPeiAfterMem.cif,
// TcgPeiAfterMem.mak, TcgTcmPeiAfterMem.c, xTcgDxe.c, xTcgPei.c,
// xTcgPeiAfterMem.c
//
//*************************************************************************
#include <Efi.h>
#include <AmiTcg\TcgCommon12.h>
#include <AmiTcg\Sha.h>
#include <AmiTcg\TcgMisc.h>
#include <token.h>
#include <AmiTcg\TcgPc.h>
#include "PPI\TcgTcmService.h"
#include "PPI\TcgService.h"
#include "PPI\TpmDevice.h"
#include "PPI\CpuIo.h"
#include "PPI\LoadFile.h"
#include <FFS.h>
#include <Library\DebugLib.h>


//*********************************************************************
//                      GLOBAL DEFINITIONS
//*********************************************************************


#pragma pack(1)
typedef struct _TCG_PEI_CALLBACK_CONTEXT
{
    PEI_TPM_PPI      *TpmDevice;
    EFI_PEI_SERVICES **PeiServices;
} TCG_PEI_CALLBACK_CONTEXT;
#pragma pack()

static
EFI_STATUS
__stdcall FillCallbackContext(
    IN EFI_PEI_SERVICES          **PeiService,
    OUT TCG_PEI_CALLBACK_CONTEXT *CallbackContext )
{
    CallbackContext->PeiServices = PeiService;
    return (*PeiService)->LocatePpi(
               PeiService,
               &gPeiTpmPpiGuid,
               0,
               NULL,
               &CallbackContext->TpmDevice
           );
}

EFI_STATUS
EFIAPI TcmPeiLogEvent(
    IN PEI_TCM_PPI      *This,
    IN EFI_PEI_SERVICES **PeiServices,
    IN TCM_PCR_EVENT    *Event,
    OUT UINT32          *EventNum );

EFI_STATUS
EFIAPI TcgPeiHashLogExtendEventTcm(
    IN PEI_TCM_PPI       *This,
    IN EFI_PEI_SERVICES  **PeiServices,
    IN UINT8             *HashData,
    IN UINT32            HashDataLen,
    IN OUT TCM_PCR_EVENT *NewEvent,
    OUT UINT32           *EventNum );


EFI_STATUS
EFIAPI TcgPeiPassThroughToTcm(
    IN PEI_TCM_PPI      *This,
    IN EFI_PEI_SERVICES **PeiServices,
    IN UINT32           TpmInputParameterBlockSize,
    IN UINT8            *TpmInputParameterBlock,
    IN UINT32           TpmOutputParameterBlockSize,
    IN UINT8            *TpmOutputParameterBlock );



static PEI_TCM_PPI            mTcmPpi =
{
    TcgPeiHashLogExtendEventTcm,
    TcmPeiLogEvent,
    TcgPeiPassThroughToTcm
};


static EFI_PEI_PPI_DESCRIPTOR mTcgTcgPpiList[] =
{
    {
        EFI_PEI_PPI_DESCRIPTOR_PPI
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gPeiTcgPpiGuid,
        &mTcmPpi
    }
};


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcmPeiGetEventLog
//
// Description: Helper function for logging TCM events
//
// Input:        IN EFI_PEI_SERVICES **PeiServices,
//               OUT TCG_LOG_HOB     **EventLog
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
EFIAPI TcmPeiGetEventLog(
    IN EFI_PEI_SERVICES **PeiServices,
    OUT TCG_LOG_HOB     **EventLog )
{
    EFI_STATUS Status;
    VOID       *HobStart;
    EFI_GUID gEfiPeiAmiTcgTcmLogHobGuid        = EFI_TCG_LOG_HOB_GUID;

    Status = (*PeiServices)->GetHobList( PeiServices, &HobStart );

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    return TcgGetNextGuidHob( &HobStart, &gEfiPeiAmiTcgTcmLogHobGuid, EventLog, NULL );
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  IntTcmTcgPeiGetCapabilities
//
// Description: Executes TCM operation to read capabilities
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      TCM_Capabilities_PermanentFlag
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
TCM_Capabilities_PermanentFlag IntTcmTcgPeiGetCapabilities(
    IN EFI_PEI_SERVICES **PeiServices )
{
    TCM_Capabilities_PermanentFlag * cap = NULL;
    EFI_STATUS                     Status;
    TPM_GetCapabilities_Input      cmdGetCap;
    UINT8                          result[0x100];

    TCG_PEI_CALLBACK_CONTEXT       Context;

    cmdGetCap.Tag         = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdGetCap.ParamSize   = TPM_H2NL( sizeof (cmdGetCap));
    cmdGetCap.CommandCode = TPM_H2NL( TCM_ORD_GetCapability );
    cmdGetCap.caparea     = TPM_H2NL( TPM_CAP_FLAG );
    cmdGetCap.subCapSize  = TPM_H2NL( 4 ); // subCap is always 32bit long
    cmdGetCap.subCap      = TPM_H2NL( TPM_CAP_FLAG_PERMANENT );

    Status =                FillCallbackContext( PeiServices, &Context );

    ASSERT_EFI_ERROR( Status );

    Status = Context.TpmDevice->Init( Context.TpmDevice, PeiServices );
    ASSERT_EFI_ERROR( Status );

    Status = TcgPeiPassThroughToTcm(
                 &mTcmPpi,
                 PeiServices,
                 sizeof(cmdGetCap),
                 (UINT8*)&cmdGetCap,
                 0x100,
                 result );

    cap = (TCM_Capabilities_PermanentFlag*)result;

    DEBUG((DEBUG_INFO,
           "GetCapability: %r; size: %x; retCode:%x; tag:%x; bytes %08x\n",
           Status,TPM_H2NL( cap->ParamSize ), TPM_H2NL(cap->RetCode ),
           (UINT32)TPM_H2NS(cap->tag ), TPM_H2NL( *(UINT32*)&cap->disabled )));

    Status = Context.TpmDevice->Close( Context.TpmDevice, PeiServices );
    ASSERT_EFI_ERROR( Status );

    return *cap;
}




//**********************************************************************
//<AMI_PHDR_START>
// Name:  TcmPeiLogEvent
//
// Description: TCM common function to log PEI events
//
// Input:       IN   *This
//              IN   **PeiServices
//              IN   *Event,
//              IN   *EventNum
//
// Output:     EFI STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//*********************************************************************
EFI_STATUS
EFIAPI TcmPeiLogEvent(
    IN PEI_TCM_PPI      *This,
    IN EFI_PEI_SERVICES **PeiServices,
    IN TCM_PCR_EVENT    *Event,
    OUT UINT32          *EventNum )
{
    EFI_STATUS                     Status = EFI_SUCCESS;
    TCG_LOG_HOB                    *TcgLog;
    TCG_PEI_CALLBACK_CONTEXT       Context;
    TCM_Capabilities_PermanentFlag Cap;

    //only log events if TPM is not deactivated
    Cap = IntTcmTcgPeiGetCapabilities( PeiServices );

    if ( !Cap.deactivated )
    {
        Status = FillCallbackContext( PeiServices, &Context );
        ASSERT_EFI_ERROR( Status );

        Status = TcmPeiGetEventLog(PeiServices, &TcgLog );

        if ( EFI_ERROR( Status ))
        {
            goto Exit;
        }

        Status = TcmCommonLogEvent(
                     &Context,
                     (TCM_PCR_EVENT*)(TcgLog + 1),
                     &TcgLog->TableSize,
                     TcgLog->TableMaxSize,
                     (TCM_PCR_EVENT *)Event
                 );

        if ( EFI_ERROR( Status ))
        {
            goto Exit;
        }

        *EventNum = TcgLog->EventNum;
        TcgLog->EventNum++;
    }
Exit:
    return Status;
}










//**********************************************************************
//<AMI_PHDR_START>
// Name:  TcgPeiHashLogExtendEventTcm
//
// Description: Tcm common function to Hash, Log and Extend data
//
// Input:       IN  *This
//              IN  **PeiServices
//              IN  *HashData
//              IN  HashDataLen
//              IN  *NewEvent,
//              IN  *EventNum
//
// Output:     EFI STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//*********************************************************************
EFI_STATUS
EFIAPI TcgPeiHashLogExtendEventTcm(
    IN PEI_TCM_PPI       *This,
    IN EFI_PEI_SERVICES  **PeiServices,
    IN UINT8             *HashData,
    IN UINT32            HashDataLen,
    IN OUT TCM_PCR_EVENT *NewEvent,
    OUT UINT32           *EventNum )
{
    EFI_STATUS               Status;
    UINT32                   Sha1MaxBytes;
    TCM_DIGEST               NewPCRValue;
    TCG_PEI_CALLBACK_CONTEXT Context;

    Status = FillCallbackContext( PeiServices, &Context );
    ASSERT_EFI_ERROR( Status );

    if ( EFI_ERROR( Status ))
    {
        goto Exit;
    }

    Status = TcgCommonSha1Start( &Context, TCG_ALG_SHA, &Sha1MaxBytes );

    if ( EFI_ERROR( Status ))
    {
        goto Exit;
    }

    Status = TcgCommonSha1Update(
                 &Context,
                 HashData,
                 HashDataLen,
                 Sha1MaxBytes
             );

    if ( EFI_ERROR( Status ))
    {
        goto Exit;
    }

    HashData    += (HashDataLen & ~63);
    HashDataLen &= 63;

    Status = TcmCommonSha1CompleteExtend(
                 &Context,
                 HashData,
                 HashDataLen,
                 NewEvent->PCRIndex,
                 &NewEvent->Digest,
                 &NewPCRValue
             );

    if ( EFI_ERROR( Status ))
    {
        goto Exit;
    }

    Status = TcmPeiLogEvent( This, PeiServices, NewEvent, EventNum );

Exit:
    return Status;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Name:  TcgPeiPassThroughToTcm
//
// Description: TCGPEI common function to abstract passing commands to the TPM
//                FIFO
//
// Input:       IN   *This
//              IN   **PeiServices
//              IN   TpmInputParameterBlockSize
//              IN   *TpmInputParameterBlock
//              IN   TpmOutputParameterBlockSize
//              IN   *TpmOutputParameterBlock
//
// Output:      EFI STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI TcgPeiPassThroughToTcm(
    IN PEI_TCM_PPI      *This,
    IN EFI_PEI_SERVICES **PeiServices,
    IN UINT32           TpmInputParameterBlockSize,
    IN UINT8            *TpmInputParameterBlock,
    IN UINT32           TpmOutputParameterBlockSize,
    IN UINT8            *TpmOutputParameterBlock )
{
    EFI_STATUS               Status;
    TPM_TRANSMIT_BUFFER      InBuffer[1], OutBuffer[1];
    TCG_PEI_CALLBACK_CONTEXT Context;

    Status = FillCallbackContext( PeiServices, &Context );
    ASSERT_EFI_ERROR( Status );

    InBuffer[0].Buffer  = TpmInputParameterBlock;
    InBuffer[0].Size    = TpmInputParameterBlockSize;
    OutBuffer[0].Buffer = TpmOutputParameterBlock;
    OutBuffer[0].Size   = TpmOutputParameterBlockSize;

    return TcgCommonPassThrough(
               &Context,
               sizeof (InBuffer) / sizeof (*InBuffer),
               InBuffer,
               sizeof (OutBuffer) / sizeof (*OutBuffer),
               OutBuffer
           );
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcgPeiEntry
//
// Description: Entry point to Memory Absent and Memory Present Initialization
//              for TPM
//
//
// Input:       IN      EFI_FFS_FILE_HEADER       *FfsHeader
//              IN      EFI_PEI_SERVICES          **PeiServices,
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
EFIAPI TcgTcmPeiMemoryCallbackEntry(
    IN EFI_PEI_SERVICES    **PeiServices
)
{
    EFI_PEI_PPI_DESCRIPTOR         *PpiDesc;
    PEI_TCG_PPI                    *TcgPpi;
    EFI_STATUS                     Status;

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gPeiTcgPpiGuid,
                 0,
                 &PpiDesc,
                 &TcgPpi);


    Status = (*PeiServices)->ReInstallPpi(
                 PeiServices,
                 PpiDesc,
                 &mTcgTcgPpiList[0] );

    ASSERT_EFI_ERROR( Status );

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gPeiTcgPpiGuid,
                 0,
                 &PpiDesc,
                 &TcgPpi );

    ASSERT_EFI_ERROR( Status );

    return Status;
}


