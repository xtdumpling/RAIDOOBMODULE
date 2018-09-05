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

   TpmDxe.h

   Abstract:

   DXE driver for TPM devices

   --*/

//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/TpmDxe.c 13    11/14/11 1:27p Fredericko $
//
// $Revision: 13 $
//
// $Date: 11/14/11 1:27p $
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  TpmDxe.c
//
// Description:
//  Abstracted functions for Tpm protocol are defined here
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Uefi.h>
#include <AmiTcg/TcgTpm12.h>
#include <AmiTcg/TpmLib.h>
#include <Protocol/TpmDevice.h>
#include <Library/DebugLib.h>
#include <Token.h>
#include <Library/IoLib.h>
#include<Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/TcgPlatformSetupPolicy.h>

extern EFI_GUID TcgPlatformSetupPolicyGuid;

#define _CR( Record, TYPE,\
        Field )((TYPE*) ((CHAR8*) (Record) - (CHAR8*) &(((TYPE*) 0)->Field)))

VOID* LocateATcgHob (
    UINTN                   NoTableEntries,
    EFI_CONFIGURATION_TABLE *ConfigTable,
    EFI_GUID                *HOB_guid );

#define TPM_DXE_PRIVATE_DATA_FROM_THIS( this )  \
    _CR( this, TPM_DXE_PRIVATE_DATA, TpmInterface )

typedef struct _TPM_DXE_PRIVATE_DATA
{
    EFI_TPM_DEVICE_PROTOCOL TpmInterface;
    EFI_PHYSICAL_ADDRESS    BaseAddr;
} TPM_DXE_PRIVATE_DATA;

FixedDelay(UINT32 dCount);

static UINTN                  TpmFuncID;
static TPMTransmitEntryStruct TpmEmptyBuf;
static UINT32                 TpmRet;
static UINTN                  ReadytoBootVar = 0;


#if TCG_LEGACY == 0
EFI_STATUS
EFIAPI TpmDxeInit(
    IN EFI_TPM_DEVICE_PROTOCOL *This )
{
    EFI_STATUS Status;
    volatile TPM_1_2_REGISTERS_PTR TpmReg;
    TpmReg  = (TPM_1_2_REGISTERS_PTR)(( UINTN ) (PORT_TPM_IOMEMBASE));
    Status = TisRequestLocality(TpmReg);
    return Status;
}

EFI_STATUS
EFIAPI TpmDxeClose(
    IN EFI_TPM_DEVICE_PROTOCOL *This )
{
    EFI_STATUS Status;
    volatile TPM_1_2_REGISTERS_PTR TpmReg;
    TpmReg  = (TPM_1_2_REGISTERS_PTR)(( UINTN ) (PORT_TPM_IOMEMBASE));
    Status = TisReleaseLocality(TpmReg);
    return Status;
}

EFI_STATUS
EFIAPI TpmDxeGetStatusInfo(
    IN EFI_TPM_DEVICE_PROTOCOL *This )
{
    return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI TpmDxeTransmit(
    IN EFI_TPM_DEVICE_PROTOCOL *This,
    IN UINTN                   NoInBuffers,
    IN TPM_TRANSMIT_BUFFER     *InBuffers,
    IN UINTN                   NoOutBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutBuffers )
{
    volatile TPM_1_2_REGISTERS_PTR TpmReg;
    TpmReg  = (TPM_1_2_REGISTERS_PTR)(( UINTN ) (PORT_TPM_IOMEMBASE));

    return TpmLibPassThrough(
               TpmReg,
               NoInBuffers,
               InBuffers,
               NoOutBuffers,
               OutBuffers
           );
}

#else


//**********************************************************************
//<AMI_PHDR_START>
//
// Name: TpmDxeInit
//
// Description: Call to OEM driver to initialize TPM.
//
// Input:       IN  EFI_TPM_DEVICE_PROTOCOL   *This
//
// Returns:     EFI_STATUS
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
EFIAPI TpmDxeInit(
    IN EFI_TPM_DEVICE_PROTOCOL *This )
{
    return EFI_SUCCESS;  //init command is only sent once and done in PEI
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Name:  TpmDxeClose
//
// Description: Close TPM connection for locality 0
//
// Input:       IN  PEI_TPM_PPI               *This,
//              IN  EFI_PEI_SERVICES          **PeiServices
//
// Output:      EFI STATUS
///
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
EFIAPI TpmDxeClose(
    IN EFI_TPM_DEVICE_PROTOCOL *This )
{
    return EFI_SUCCESS;
}




//**********************************************************************
//<AMI_PHDR_START>
//
//
// Name:  TpmDxeGetStatusInfo
//
// Description: GetStatus Info from MP driver
//
// Input:       IN  PEI_TPM_PPI               *This,
//              IN  EFI_PEI_SERVICES          **PeiServices
//
// Output:      EFI STATUS
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
EFIAPI  TpmDxeGetStatusInfo(
    IN EFI_TPM_DEVICE_PROTOCOL   * This
)
{
    return EFI_UNSUPPORTED;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   SetEfiOSTransitions
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
void SetEfiOSTransitions(
    IN EFI_EVENT efiev,
    IN VOID      *ctx )
{
    ReadytoBootVar = 1;
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
EFIAPI EfiOSReadyToBoot()
{
    EFI_EVENT  ReadToBootEvent;
    EFI_STATUS Status;

#if defined(EFI_EVENT_SIGNAL_READY_TO_BOOT)\
        && EFI_SPECIFICATION_VERSION < 0x20000

    Status = pBS->CreateEvent( EFI_EVENT_SIGNAL_READY_TO_BOOT,
                               EFI_TPL_CALLBACK,
                               SetEfiOSTransitions, NULL, &ReadToBootEvent );

#else
    Status = CreateReadyToBootEvent( EFI_TPL_CALLBACK-1,
                                     SetEfiOSTransitions,
                                     NULL,
                                     &ReadToBootEvent );
#endif

    return Status;
}


//**********************************************************************
//<AMI_PHDR_START>
//
//
// Name:  AllocateMemoryBelow4G
//
// Description: Allocate Memory Pages below 4G
//
// Input:       IN  EFI_MEMORY_TYPE
//              IN  Size
//
// Output:      VOID* allocated memory address
//
//<AMI_PHDR_END>
//**********************************************************************

VOID*
AllocateMemoryBelow4G (
  IN EFI_MEMORY_TYPE    MemoryType,
  IN UINTN              Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID*                 Buffer;
  
  Pages = EFI_SIZE_TO_PAGES (Size);
  
  Address = 0xffffffff;		//limit to 4G

  Status  = gBS->AllocatePages (
                   AllocateMaxAddress,
                   MemoryType,
                   Pages,
                   &Address
                   );
  
  ASSERT_EFI_ERROR (Status);

  Buffer = (UINT8 *) (UINTN) Address;
  
  if(Status == EFI_SUCCESS) ZeroMem (Buffer, Size);

  return Buffer;
}
//**********************************************************************
//<AMI_PHDR_START>
//
//
// Name: TpmDxeTransmit
//
// Description: Dxe Transmit Tcg Data
//
// Input:    IN      EFI_TPM_DEVICE_PROTOCOL   *This,
//           IN      UINTN                     NoInBuffers,
//           IN      TPM_TRANSMIT_BUFFER       *InBuffers,
//           IN      UINTN                     NoOutBuffers,
//           IN OUT  TPM_TRANSMIT_BUFFER       *OutBuffers
//
// Output:     EFI STATUS
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
EFIAPI TpmDxeTransmit(
    IN EFI_TPM_DEVICE_PROTOCOL *This,
    IN UINTN                   NoInBuffers,
    IN TPM_TRANSMIT_BUFFER     *InBuffers,
    IN UINTN                   NoOutBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutBuffers )
{
    TPM_DXE_PRIVATE_DATA   *Private;
    TPMTransmitEntryStruct FillESI;
    UINTN                  FuncID = (UINTN)MP_FUNCTION_TRANSMIT;
    UINT32                 Ret;
    BOOLEAN                FillBuff   = FALSE;
    BOOLEAN                Other      = FALSE;
    UINTN                  i = 0, loc = 0;
    EFI_PHYSICAL_ADDRESS   Tpm_SHA_ARRAY = 0;
    EFI_PHYSICAL_ADDRESS   SHA_ARRAY_OUT = 0;


    if(NoInBuffers == 0 || InBuffers == NULL || NoOutBuffers == 0 || OutBuffers == NULL)
        return EFI_INVALID_PARAMETER;

    if(ReadytoBootVar == 1)
    {
        return EfiPreOSTransition(This, NoInBuffers, InBuffers, NoOutBuffers, OutBuffers);
    }


    FillESI.dwInLen = 0;
    FillESI.dwOutLen = 0;

    for (; i < NoInBuffers; i++ )
    {
        FillESI.dwInLen += (UINT32)InBuffers[i].Size;
    }

    Tpm_SHA_ARRAY = (EFI_PHYSICAL_ADDRESS)(UINTN)AllocateMemoryBelow4G(EfiBootServicesData,FillESI.dwInLen);
    if(Tpm_SHA_ARRAY == 0xffffffff) return EFI_OUT_OF_RESOURCES;
    
    for (i = 0; i < NoOutBuffers; i++ )
    {
        FillESI.dwOutLen += (UINT32)OutBuffers[i].Size;
    }

    SHA_ARRAY_OUT = (EFI_PHYSICAL_ADDRESS)(UINTN)AllocateMemoryBelow4G(EfiBootServicesData,FillESI.dwOutLen);
    
    if(SHA_ARRAY_OUT == 0xffffffff) {
      gBS->FreePages ((EFI_PHYSICAL_ADDRESS) (UINTN) Tpm_SHA_ARRAY,EFI_SIZE_TO_PAGES (FillESI.dwInLen));
      return EFI_OUT_OF_RESOURCES;
    }

    for (i = 0; i < NoInBuffers; i++ )
    {
        pBS->CopyMem((UINT8 *)(Tpm_SHA_ARRAY + loc),
                     InBuffers[i].Buffer,
                     InBuffers[i].Size );

        loc += InBuffers[i].Size;
    }

    FillESI.pbInBuf  = (UINT32)Tpm_SHA_ARRAY;
    FillESI.pbOutBuf = (UINT32)SHA_ARRAY_OUT;

    Private = TPM_DXE_PRIVATE_DATA_FROM_THIS( This );
    TpmDxeCallMPDriver( FuncID, &FillESI, &Ret );

    loc = 0;

    for (i=0; i < NoOutBuffers; i++ )
    {
        pBS->CopyMem( OutBuffers[i].Buffer,
                      (UINT8 *)(SHA_ARRAY_OUT + loc),
                      OutBuffers[i].Size );
        loc += (UINTN)OutBuffers[i].Size;
    }

    gBS->FreePages ((EFI_PHYSICAL_ADDRESS) (UINTN) Tpm_SHA_ARRAY,EFI_SIZE_TO_PAGES (FillESI.dwInLen));
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS) (UINTN) SHA_ARRAY_OUT,EFI_SIZE_TO_PAGES (FillESI.dwOutLen));

    return EFI_SUCCESS;
}

#endif

static TPM_DXE_PRIVATE_DATA mTpmPrivate =
{
    {
        TpmDxeInit,
        TpmDxeClose,
        TpmDxeGetStatusInfo,
        TpmDxeTransmit
    },
    TPM_BASE_ADDRESS
};




EFI_STATUS
EFIAPI TpmDxeEntry(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS Status;
#if TCG_LEGACY == 1
    FAR32LOCALS          *TempLoc   = NULL;
#endif
    TCG_PLATFORM_SETUP_PROTOCOL     *ProtocolInstance;

    Status = gBS->LocateProtocol (&gTcgPlatformSetupPolicyGuid,  NULL, &ProtocolInstance);
    if (EFI_ERROR (Status))
    {
        return Status;
    }

#if TCG_LEGACY == 0
    if(ProtocolInstance->ConfigFlags.DeviceType != 0)
    {
        if(isTpm20CrbPresent())return EFI_NOT_FOUND;
    }

    Status = IsTpmPresent((TPM_1_2_REGISTERS_PTR)( UINTN ) mTpmPrivate.BaseAddr );

    if ( EFI_ERROR( Status ))
    {
        return EFI_UNLOAD_IMAGE;
    }
#else
    TempLoc = (FAR32LOCALS*)LocateATcgHob( pST->NumberOfTableEntries,
                                           pST->ConfigurationTable,
                                           &gEfiTcgMpDriverHobGuid );


    if(TempLoc == NULL || TempLoc->Offset == NULL  ) return EFI_NOT_FOUND;
    Prepare2Thunkproc( TempLoc->Offset, TempLoc->Selector );
    Status = EfiOSReadyToBoot();
#endif

    Status = gBS->InstallMultipleProtocolInterfaces(
                 &ImageHandle,
                 &gEfiTpmDeviceProtocolGuid,
                 &mTpmPrivate.TpmInterface,
                 NULL
             );

    return Status;
}
