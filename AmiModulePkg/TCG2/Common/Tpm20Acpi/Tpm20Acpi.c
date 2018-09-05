//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
//<AMI_FHDR_START>
//
// Name:
//
// Description:
//
//<AMI_FHDR_END>
//*************************************************************************

#include <Token.h>
#include "Tpm20Acpi.h"
#include <AmiTcg/TCGMisc.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <AmiTcg/TrEEProtocol.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#include <Protocol/FirmwareVolume2.h>
#include <Acpi.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiSupport.h>
#include <Include/AmiProtocol.h>
#include <Library/IoLib.h>
#include <Guid/AmiTcgGuidIncludes.h>
#include <Library/PcdLib.h>
#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6
#include <Protocol/VariableLock.h>
#endif



//#define     BIT00   0x00000001
#define EFI_ACPI_TABLE_VERSION_X        (EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0 | EFI_ACPI_TABLE_VERSION_4_0)
#define EFI_ACPI_TABLE_VERSION_ALL      (EFI_ACPI_TABLE_VERSION_1_0B|EFI_ACPI_TABLE_VERSION_X)


extern EFI_GUID gBdsAllDriversConnectedProtocolGuid;

AMI_ASL_PPI_NV_VAR              *MemoryAddress = NULL;

EFI_TPM2_ACPI_TABLE  mTpm2AcpiTemplate =
{
    {
        EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE,
        sizeof (mTpm2AcpiTemplate),
        EFI_TPM2_ACPI_TABLE_REVISION,
        //
        // initialize to 0
        //
        //
    },
    0, // Flags
    (EFI_PHYSICAL_ADDRESS)(UINTN)0xFFFFFFFF, // Control Area
    EFI_TPM2_ACPI_TABLE_START_METHOD_ACPI,
};


#if FTpmPlatformProfile == 1

EFI_STATUS
iTpmGetInfo (
    IN OUT   UINTN                *iTPMStatus
);

#endif

EFI_STATUS TcgUpdateAslNameObject(ACPI_HDR *PDsdt, UINT8 *ObjName, UINT64 Value);
EFI_STATUS TcgLibGetDsdt(EFI_PHYSICAL_ADDRESS *DsdtAddr, EFI_ACPI_TABLE_VERSION Version);
EFI_STATUS GetPspBar1Addr (IN OUT   EFI_PHYSICAL_ADDRESS *PspMmio );

EFI_STATUS TcgSetVariableWithNewAttributes(
    IN CHAR16 *Name, IN EFI_GUID *Guid, IN UINT32 Attributes,
    IN UINTN DataSize, IN VOID *Data
);

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   GetHob
//
// Description: Find instance of a HOB type in a HOB list
//
// Input:
//      Type          The HOB type to return.
//      HobStart      The first HOB in the HOB list.
//
// Output:
//      Pointer to the Hob matching the type or NULL
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
VOID* GetHob(
    IN UINT16 Type,
    IN VOID   *HobStart  )
{
    EFI_PEI_HOB_POINTERS Hob;

    Hob.Raw = HobStart;

    //
    // Return input if not found
    //
    if ( HobStart == NULL )
    {
        return HobStart;
    }

    //
    // Parse the HOB list, stop if end of list or matching type found.
    //
    while ( !END_OF_HOB_LIST( Hob ))
    {
        if ( Hob.Header->HobType == Type )
        {
            break;
        }

        Hob.Raw = GET_NEXT_HOB( Hob );
    }

    //
    // Return input if not found
    //
    if ( END_OF_HOB_LIST( Hob ))
    {
        return HobStart;
    }

    return (VOID*)(Hob.Raw);
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   GetNextGuidHob
//
// Description: Find GUID HOB
//
// Input:       HobStart    A pointer to the start hob.
//              Guid        A pointer to a guid.
// Output:
//              Buffer          A pointer to the buffer.
//              BufferSize      Buffer size.
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS TcgGetNextGuidHob(
    IN OUT VOID          **HobStart,
    IN EFI_GUID          * Guid,
    OUT VOID             **Buffer,
    OUT UINTN            *BufferSize OPTIONAL )
{
    EFI_STATUS           Status;
    EFI_PEI_HOB_POINTERS GuidHob;

    if ( Buffer == NULL )
    {
        return EFI_INVALID_PARAMETER;
    }

    for ( Status = EFI_NOT_FOUND; EFI_ERROR( Status );)
    {
        GuidHob.Raw = *HobStart;

        if ( END_OF_HOB_LIST( GuidHob ))
        {
            return EFI_NOT_FOUND;
        }

        GuidHob.Raw = GetHob( EFI_HOB_TYPE_GUID_EXTENSION, *HobStart );

        if ( GuidHob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION )
        {
            if ( (CompareMem( Guid, &GuidHob.Guid->Name, sizeof(EFI_GUID) ))==0)
            {
                Status  = EFI_SUCCESS;
                *Buffer = (VOID*)((UINT8*)(&GuidHob.Guid->Name)
                                  + sizeof (EFI_GUID));

                if ( BufferSize != NULL )
                {
                    *BufferSize = GuidHob.Header->HobLength
                                  - sizeof (EFI_HOB_GUID_TYPE);
                }
            }
        }

        *HobStart = GET_NEXT_HOB( GuidHob );
    }

    return Status;
}



VOID* FindHob(
    IN UINTN                   NoTableEntries,
    IN EFI_CONFIGURATION_TABLE *ConfigTable,
    IN EFI_GUID                *HOB_guid )
{
    VOID *HobStart;
    VOID *PtrHob;

    while ( NoTableEntries > 0 )
    {
        NoTableEntries--;

        if ((!CompareMem(
                    &ConfigTable[NoTableEntries].VendorGuid,
                    &gEfiHobListGuid, sizeof(EFI_GUID)
                )))
        {
            HobStart = ConfigTable[NoTableEntries].VendorTable;

            if ( !EFI_ERROR(
                        TcgGetNextGuidHob( &HobStart, HOB_guid, &PtrHob, NULL )
                    ))
            {
                DEBUG ((DEBUG_INFO, "Hob found = %x \n", PtrHob));
                return PtrHob;
            }
        }
    }
    return NULL;
}


#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6
AcpiOnVariableLockProtocolGuid (
    IN      EFI_EVENT                 Event,
    IN      VOID                      *Context
)
{
    EDKII_VARIABLE_LOCK_PROTOCOL    *LockProtocol;
    EFI_STATUS                      Status;


    DEBUG((DEBUG_INFO, "Tpm20OnVariableLockProtocolGuid callback entry\n"));

    Status =  gBS->LocateProtocol(&gEdkiiVariableLockProtocolGuid, NULL, &LockProtocol);
    if(!EFI_ERROR(Status))
    {
        Status = LockProtocol->RequestToLock(LockProtocol, L"TpmServFlags", &FlagsStatusguid);
        ASSERT_EFI_ERROR(Status);
    }

    gBS->CloseEvent(Event);
}
#endif

EFI_STATUS
Tpm20PublishAcpiTable (IN EFI_EVENT ev,
                       IN VOID *ctx)
{
    EFI_STATUS                      Status;
    UINTN                           TableKey = 0;
    EFI_ACPI_TABLE_PROTOCOL         *AcpiTable;
    EFI_TPM2_ACPI_CONTROL_AREA      *CtrlAreaMap = NULL;

    INTEL_PTT_HOB_DATA              *StolenAddress = NULL;
    UINTN                           FtpmBase=0;
    EFI_PHYSICAL_ADDRESS            dsdtAddress=0;
    ACPI_HDR                        *dsdt;
    EFI_ACPI_SUPPORT_PROTOCOL       *mTcgAcpiSupport;
#if defined ACPI_MODULE_VER && ACPI_MODULE_VER < 120     
    UINT8                     OemTblId[8] = CONVERT_TO_STRING(T_ACPI_OEM_TBL_ID);
    UINT8                     OemId[6]    = CONVERT_TO_STRING(T_ACPI_OEM_ID);
#endif
    EFI_TREE_PROTOCOL                *TreeProtocolPtr;
    TREE_BOOT_SERVICE_CAPABILITY     ProtocolCapability;
    EFI_GUID                         gMeDataHobguid  = gTcgMeDataHobGuid;
#if FTpmPlatformProfile == 1
    UINTN  Info;   //TODO
#endif


    mTpm2AcpiTemplate.Header.OemRevision     = TPM20TABLEOEMREVISION;
    mTpm2AcpiTemplate.Header.CreatorId  = CREATOR_ID_AMI;
    
#if defined ACPI_MODULE_VER && ACPI_MODULE_VER > 110   
    
    CopyMem (&mTpm2AcpiTemplate.Header.OemId,
            PcdGetPtr (PcdAcpiDefaultOemId),
            sizeof (mTpm2AcpiTemplate.Header.OemId));
    
    mTpm2AcpiTemplate.Header.OemTableId = PcdGet64 (PcdAcpiDefaultOemTableId);
#else
    gBS->CopyMem(&mTpm2AcpiTemplate.Header.OemTableId, OemTblId, 8);
    gBS->CopyMem(&mTpm2AcpiTemplate.Header.OemId, OemId, 6);
#endif
    mTpm2AcpiTemplate.Flags = TCG_PLATFORM_CLASS;

    //
    // Publish the TPM ACPI table
    //
    Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);
    if(EFI_ERROR(Status))return Status;

    Status = gBS->LocateProtocol( &gEfiAcpiSupportGuid, NULL, &mTcgAcpiSupport );
    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    Status = TcgLibGetDsdt(&dsdtAddress, EFI_ACPI_TABLE_VERSION_ALL);
    if(EFI_ERROR(Status))
    {
        return Status;
    }

    dsdt = (ACPI_HDR*)dsdtAddress;


    Status = gBS->LocateProtocol(&gEfiTrEEProtocolGuid, NULL, &TreeProtocolPtr);
    if(!EFI_ERROR(Status))
    {
        ProtocolCapability.Size = sizeof(TREE_BOOT_SERVICE_CAPABILITY);
        Status = TreeProtocolPtr->GetCapability(TreeProtocolPtr, &ProtocolCapability);
        if ( EFI_ERROR( Status ))
        {
            return Status;
        }
        Status=TcgUpdateAslNameObject(dsdt, "TPMF", 1);
    }
    else
    {
        Status=TcgUpdateAslNameObject(dsdt, "TPMF", 0);
        Status=TcgUpdateAslNameObject(dsdt, "TPMM", (UINT32)0xFED40000);
        Status=TcgUpdateAslNameObject(dsdt, "AMDT", 0x0);
        return Status;
    }

    if(!isTpm20CrbPresent())
    {
        mTpm2AcpiTemplate.AddressOfControlArea = 0;
        mTpm2AcpiTemplate.StartMethod = EFI_TPM2_ACPI_TABLE_START_METHOD_TIS_CANCEL;

        Status=TcgUpdateAslNameObject(dsdt, "AMDT", 0x0);
        if(EFI_ERROR(Status))
        {
            return Status;
        }

        Status=TcgUpdateAslNameObject(dsdt, "DTPT", 0x0);
        if(EFI_ERROR(Status))
        {
            return Status;
        }

        Status=TcgUpdateAslNameObject(dsdt, "TPMM", (UINT64)PORT_TPM_IOMEMBASE);
        if(EFI_ERROR(Status))
        {
            return Status;
        }

    }
    else
    {
#if FTpmPlatformProfile == 1
        if(EFI_ERROR(iTpmGetInfo(&Info)))
        {
#endif
            if((*(UINT32 *)(PORT_TPM_IOMEMBASE + 0x0C))==0xFFFFFFFF)
            {
                StolenAddress = GetFirstGuidHob (&gMeDataHobguid);
                DEBUG ((DEBUG_INFO, "Tpm20Acpi StolenAddress = %x \n", StolenAddress->BufferAddress));
                DEBUG ((DEBUG_INFO, "Tpm20Acpi StolenAddress Loc = %x \n", StolenAddress));

                if(StolenAddress != NULL)
                {
                    if(((UINTN)((UINT64 *)StolenAddress->BufferAddress)) != 0)
                    {
                        mTpm2AcpiTemplate.AddressOfControlArea = StolenAddress->BufferAddress;
                    }
                    else
                    {
                        mTpm2AcpiTemplate.AddressOfControlArea = TPM20_CRBBASE;
                    }
                }
                else
                {
                    mTpm2AcpiTemplate.AddressOfControlArea = TPM20_CRBBASE;
                }

                mTpm2AcpiTemplate.StartMethod = EFI_TPM2_ACPI_TABLE_START_METHOD_ACPI;
                CtrlAreaMap = (EFI_TPM2_ACPI_CONTROL_AREA *)(UINTN) mTpm2AcpiTemplate.AddressOfControlArea;

                if(mTpm2AcpiTemplate.AddressOfControlArea != TPM20_CRBBASE)
                {
                    gBS->SetMem (CtrlAreaMap, sizeof(EFI_TPM2_ACPI_CONTROL_AREA), 0);
                }

                CtrlAreaMap->CommandSize  = ProtocolCapability.MaxCommandSize;
                CtrlAreaMap->ResponseSize = ProtocolCapability.MaxResponseSize;
                CtrlAreaMap->Command      = (UINTN)mTpm2AcpiTemplate.AddressOfControlArea + 0x80;
                CtrlAreaMap->Response     = (UINTN)mTpm2AcpiTemplate.AddressOfControlArea + 0x80;

                Status=TcgUpdateAslNameObject(dsdt, "TPMM", (UINT32)INTELFTPMBASE);
                if(EFI_ERROR(Status))
                {
                    return Status;
                }

                Status=TcgUpdateAslNameObject(dsdt, "FTPM", (UINT32)mTpm2AcpiTemplate.AddressOfControlArea);
                if(EFI_ERROR(Status))
                {
                    return Status;
                }

                Status=TcgUpdateAslNameObject(dsdt, "AMDT", 0x0);
                if(EFI_ERROR(Status))
                {
                    return Status;
                }

                Status=TcgUpdateAslNameObject(dsdt, "DTPT", 0x0);
                if(EFI_ERROR(Status))
                {
                    return Status;
                }
            }
            else
            {
#if defined(TPM20_CRB_WITH_START_METHOD) && TPM20_CRB_WITH_START_METHOD == 1
                mTpm2AcpiTemplate.StartMethod          = EFI_TPM2_ACPI_TABLE_START_METHOD_ACPI;
#else
                mTpm2AcpiTemplate.StartMethod          = EFI_TPM2_ACPI_TABLE_START_METHOD_CRB_INTF;
#endif
                mTpm2AcpiTemplate.AddressOfControlArea = (UINTN) (PORT_TPM_IOMEMBASE + 0x40);
                CtrlAreaMap = (EFI_TPM2_ACPI_CONTROL_AREA *)(UINTN) mTpm2AcpiTemplate.AddressOfControlArea;

                DEBUG ((DEBUG_INFO, "CtrlAreaMap->Error = %x \n", CtrlAreaMap->Error));
                DEBUG ((DEBUG_INFO, "CtrlAreaMap->Cancel = %x \n", CtrlAreaMap->Cancel));
                DEBUG ((DEBUG_INFO, "CtrlAreaMap->Start = %x \n", CtrlAreaMap->Start));
                DEBUG ((DEBUG_INFO, "CtrlAreaMap->Reserved = %x \n", CtrlAreaMap->Reserved));
                DEBUG ((DEBUG_INFO, "CtrlAreaMap->CommandSize = %x \n", CtrlAreaMap->CommandSize));
                DEBUG ((DEBUG_INFO, "CtrlAreaMap->Command = %x \n", CtrlAreaMap->Command));
                DEBUG ((DEBUG_INFO, "CtrlAreaMap->ResponseSize = %x \n", CtrlAreaMap->ResponseSize));
                DEBUG ((DEBUG_INFO, "CtrlAreaMap->Response = %x \n", CtrlAreaMap->Response));

                Status=TcgUpdateAslNameObject(dsdt, "TPMM", ((UINT32)mTpm2AcpiTemplate.AddressOfControlArea-0x40));
                if(EFI_ERROR(Status))
                {
                    return Status;
                }

                Status=TcgUpdateAslNameObject(dsdt, "FTPM", (UINT32)mTpm2AcpiTemplate.AddressOfControlArea);
                if(EFI_ERROR(Status))
                {
                    return Status;
                }

                Status=TcgUpdateAslNameObject(dsdt, "AMDT", 0x0);
                if(EFI_ERROR(Status))
                {
                    return Status;
                }

                Status=TcgUpdateAslNameObject(dsdt, "DTPT", 0x1);
                if(EFI_ERROR(Status))
                {
                    return Status;
                }
            }
#if FTpmPlatformProfile == 1
        }
        else
        {
#else
            // We will not config the AMD fTPM.
        }
        if( 0 )
        {
#endif
            if(EFI_ERROR( GetPspBar1Addr ((EFI_PHYSICAL_ADDRESS *)&FtpmBase))) return EFI_UNSUPPORTED;
            FtpmBase = FtpmBase + 0x10;
            mTpm2AcpiTemplate.AddressOfControlArea = FtpmBase;

            Status=TcgUpdateAslNameObject(dsdt, "TPMB", *(UINT32 *)(FtpmBase + 0x1C));
            if(EFI_ERROR(Status))
            {
                return Status;
            }

            Status=TcgUpdateAslNameObject(dsdt, "TPMC", *(UINT32 *)(FtpmBase + 0x28));
            if(EFI_ERROR(Status))
            {
                return Status;
            }

            Status=TcgUpdateAslNameObject(dsdt, "TPMM", FtpmBase);
            if(EFI_ERROR(Status))
            {
                return Status;
            }

            Status=TcgUpdateAslNameObject(dsdt, "FTPM", FtpmBase);
            if(EFI_ERROR(Status))
            {
                return Status;
            }

            Status=TcgUpdateAslNameObject(dsdt, "AMDT", 0x1);
            if(EFI_ERROR(Status))
            {
                return Status;
            }

            CtrlAreaMap = (EFI_TPM2_ACPI_CONTROL_AREA *)(UINTN) &mTpm2AcpiTemplate.AddressOfControlArea;
            mTpm2AcpiTemplate.StartMethod = 2;
#if FTpmPlatformProfile == 1
        }
#endif
    }

    //if MemoryAddress is NULL, TCGsmm checks for this
    Status=TcgUpdateAslNameObject(dsdt, "PPIM", (UINT32)MemoryAddress);
    ASSERT_EFI_ERROR (Status);
    Status=TcgUpdateAslNameObject(dsdt, "PPIL", (UINT32)(sizeof(AMI_ASL_PPI_NV_VAR)));
    ASSERT_EFI_ERROR (Status);

    Status = AcpiTable->InstallAcpiTable (AcpiTable,
                                          &mTpm2AcpiTemplate,
                                          sizeof(EFI_TPM2_ACPI_TABLE),
                                          &TableKey);

    if(EFI_ERROR(Status))return Status;

    gBS->CloseEvent(ev);

    return Status;
}

EFI_STATUS
EFIAPI
Tpm20AcpiInitEntry (
    IN EFI_HANDLE                  ImageHandle,
    IN EFI_SYSTEM_TABLE            *SystemTable
)
{
    EFI_STATUS Status;
    EFI_EVENT        ev;
    static VOID      *reg;
#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6
    EDKII_VARIABLE_LOCK_PROTOCOL    *LockProtocol;
    EFI_EVENT                       VarLockEvent;
static VOID                         *VarLockreg;
#endif
    EFI_PHYSICAL_ADDRESS             VarLoc;

    Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                               TPL_CALLBACK,
                               Tpm20PublishAcpiTable,
                               0,
                               &ev );

    if(EFI_ERROR(Status))
    {
        return Status;
    }

    Status = gBS->RegisterProtocolNotify(
                 &gBdsAllDriversConnectedProtocolGuid,
                 ev,
                 &reg );

    if(EFI_ERROR(Status))
    {
        return Status;
    }

    Status = gBS->AllocatePool (EfiACPIMemoryNVS, sizeof(AMI_ASL_PPI_NV_VAR), &MemoryAddress);
    if(!EFI_ERROR(Status))
    {
       ZeroMem (MemoryAddress, sizeof(AMI_ASL_PPI_NV_VAR));

       VarLoc = (EFI_PHYSICAL_ADDRESS)MemoryAddress;
#if NVRAM_VERSION > 6
       Status = TcgSetVariableWithNewAttributes(L"TpmServFlags",
                              &FlagsStatusguid,
                              EFI_VARIABLE_BOOTSERVICE_ACCESS |
                              EFI_VARIABLE_NON_VOLATILE |
                              EFI_VARIABLE_RUNTIME_ACCESS,
                              sizeof (EFI_PHYSICAL_ADDRESS),
                              &VarLoc );
        ASSERT_EFI_ERROR (Status);
#else
       Status = TcgSetVariableWithNewAttributes(L"TpmServFlags",
                              &FlagsStatusguid,
                              EFI_VARIABLE_BOOTSERVICE_ACCESS |
                              EFI_VARIABLE_NON_VOLATILE,
                              sizeof (EFI_PHYSICAL_ADDRESS),
                              &VarLoc );

       ASSERT_EFI_ERROR (Status);
#endif

#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6

       //Lock the service flags variable as well
       Status =  gBS->LocateProtocol(&gEdkiiVariableLockProtocolGuid, NULL, &LockProtocol);
       if(!EFI_ERROR(Status)){
           Status = LockProtocol->RequestToLock(LockProtocol, L"TpmServFlags", &FlagsStatusguid);
           DEBUG ((DEBUG_INFO, "TpmServFlags Flags successfully locked\n", Status));
           ASSERT_EFI_ERROR(Status);
       }else{
           //setcallback
           Status = gBS->CreateEvent (EFI_EVENT_NOTIFY_SIGNAL,
                                      TPL_CALLBACK,
                                      AcpiOnVariableLockProtocolGuid,
                                      NULL,
                                      &VarLockEvent);
           if(!EFI_ERROR(Status))
           {
               Status = gBS->RegisterProtocolNotify(
                             &gEdkiiVariableLockProtocolGuid,
                             VarLockEvent,
                             &VarLockreg );
           }
       }

       ASSERT_EFI_ERROR (Status);
#endif
    }else{
        //use defaults form SDL token.
        DEBUG ((DEBUG_ERROR, "Allocate PPI Memory failed Status = %r\n", Status));
    }
    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
