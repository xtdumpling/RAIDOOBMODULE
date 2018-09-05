//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/AmiTcgNvflagSample/AmiTcgNvflagSample.c 1     10/08/13 12:03p Fredericko $Revision:
//
// $Date:
//*************************************************************************
// Revision History
// ----------------
//
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: AmiTcgNvflagSample
//
// Description: This is a sample file for support TCG Ppi. It creates and installed
//      a protocal to access the persistent bios tpm flags in Tpm NV 0x50010000.
//      See Ppi Spec 1.2
//
//<AMI_FHDR_END>
//************************************************************************
#include "AmiTcgNvflagSample.h"
#include <Efi.h>
#include "AmiTcg/TcgEFI12.h"
#include "AmiTcg/TcgPc.h"
#include <AmiTcg/TcgCommon12.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include<Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/AmiTcgGuidIncludes.h>
#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6
#include <Protocol/VariableLock.h>
#endif

UINT8       Internal_flag = 0;

EFI_STATUS TcgSetVariableWithNewAttributes(
    IN CHAR16 *Name, IN EFI_GUID *Guid, IN UINT32 Attributes,
    IN UINTN DataSize, IN VOID *Data
);

UINT32
NvSendTpmCommand    (
    IN EFI_TCG_PROTOCOL     *tcg,
    IN UINT32               ord,
    IN int                  dataSize,
    IN VOID                 *data
);



TPM_RESULT SendSelfTest()
{
    EFI_STATUS              Status = EFI_SUCCESS;
    EFI_TCG_PROTOCOL        *tcgSvc;
    TPM_RESULT              tpmResult = 0;
    EFI_TPM_DEVICE_PROTOCOL *TpmDevice;

    Status = gBS->LocateProtocol( &gEfiTpmDeviceProtocolGuid,NULL, &TpmDevice);
    if ( EFI_ERROR( Status ))
    {
        return 0;
    }

    Status = gBS->LocateProtocol( &gEfiTcgProtocolGuid, NULL, &tcgSvc );
    if ( EFI_ERROR( Status ))
    {
        DEBUG((DEBUG_ERROR, "Error: failed to locate TCG protocol: %r\n"));
        return 0;
    }

#if defined DONT_SEND_SELFTEST_TILL_READY_TO_BOOT && DONT_SEND_SELFTEST_TILL_READY_TO_BOOT == 1
    TpmDevice->Init( TpmDevice );

    if(*(UINT16 *)(UINTN)(PORT_TPM_IOMEMBASE + 0xF00) == SELF_TEST_VID)
    {
        tpmResult = NvSendTpmCommand( tcgSvc,  TPM_ORD_ContinueSelfTest,0, 0);
    }

    TpmDevice->Close( TpmDevice );
#endif
    return tpmResult;
}
//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   Set_Persistent_Bios_TPM_Flags
//
// Description: Sets persistent bios flags in TPM NV
//
//
// Input:       IN PERSISTENT_BIOS_TPM_FLAGS *
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
EFI_STATUS Set_Persistent_Bios_TPM_Flags(PERSISTENT_BIOS_TPM_FLAGS *NvBIOSflags)
{
    EFI_STATUS              Status = EFI_SUCCESS;
#if USE_BIOSNVRAM_ONLY == 0
    TPM_NV_WRITE_CMD        WriteCmd;
    TPM_NV_WRITE_RET        WriteRet;
    TPM_RESULT              RetCode;
#endif
    EFI_TCG_PROTOCOL        *tcgSvc;
    EFI_TPM_DEVICE_PROTOCOL *TpmDevice;
    UINTN                   Count = 10;

    UINTN                   Size = sizeof(PERSISTENT_BIOS_TPM_FLAGS);
    UINTN                   BkupSize = sizeof(INTERNAL_BIOS_TPM_PPI_INTERFACE);
    PERSISTENT_BIOS_TPM_FLAGS  TpmNvflags;
    INTERNAL_BIOS_TPM_PPI_INTERFACE Interface;


    if(NvBIOSflags == NULL)return EFI_INVALID_PARAMETER;

    Status = gBS->LocateProtocol( &gEfiTpmDeviceProtocolGuid,NULL, &TpmDevice);
    if ( EFI_ERROR( Status ))
    {
        return EFI_NOT_FOUND;
    }

    Status = gBS->LocateProtocol( &gEfiTcgProtocolGuid, NULL, &tcgSvc );
    if ( EFI_ERROR( Status ))
    {
        DEBUG((DEBUG_ERROR, "Error: failed to locate TCG protocol: %r\n"));
        return EFI_NOT_FOUND;
    }

    Status = gRT->GetVariable(L"TcgInterfaceVar", &mAmiTcgInterfaceVarGuid, NULL, &BkupSize, &Interface);
    if(!EFI_ERROR(Status) && (Interface.Interface == 1)) //use NVRAM
    {
        gBS->CopyMem (&TpmNvflags, NvBIOSflags, sizeof(PERSISTENT_BIOS_TPM_FLAGS));

        Status =  TcgSetVariableWithNewAttributes(L"INTERNALPERBIOSFLAGS", &mAmiPpiInternalVarGuid, \
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                  Size, &TpmNvflags  );
    }

#if USE_BIOSNVRAM_ONLY == 1
    SetMem(&Interface, sizeof(Interface), 0);
    Interface.Interface = 1;

    Status =  TcgSetVariableWithNewAttributes(L"TcgInterfaceVar", &mAmiTcgInterfaceVarGuid, \
              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
              BkupSize, &Interface );

    if(!EFI_ERROR(Status))
    {
        gBS->CopyMem (&TpmNvflags, NvBIOSflags, sizeof(PERSISTENT_BIOS_TPM_FLAGS));

        Status =  TcgSetVariableWithNewAttributes(L"INTERNALPERBIOSFLAGS", &mAmiPpiInternalVarGuid, \
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                  Size, &TpmNvflags  );
    }

    if(!EFI_ERROR(Status))
    {
#if NVRAM_VERSION > 6
        Status =  TcgSetVariableWithNewAttributes(L"TPMPERBIOSFLAGS", &FlagsStatusguid, \
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,\
                  sizeof (PERSISTENT_BIOS_TPM_FLAGS), &TpmNvflags);

#else
        Status =  TcgSetVariableWithNewAttributes(L"TPMPERBIOSFLAGS", &FlagsStatusguid, \
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                  sizeof (PERSISTENT_BIOS_TPM_FLAGS), &TpmNvflags  );
#endif
    }

    return Status;
#else
    SendSelfTest();

    TpmDevice->Init( TpmDevice );

    //physical presence is required so tcgpei should have already taken care of
    //physical presence requirement
    WriteCmd.writeHeader.dataSize    = TPM_H2NL( NV_DATA_SIZE );
    WriteCmd.writeHeader.tag         = TPM_H2NS(TPM_TAG_RQU_COMMAND);
    WriteCmd.writeHeader.paramSize = TPM_H2NL( sizeof(TPM_NV_WRITE_CMD_HDR) + NV_DATA_SIZE);
    WriteCmd.writeHeader.ordinal = TPM_H2NL(TPM_ORD_NV_WriteValue);
    WriteCmd.writeHeader.nvIndex = TPM_H2NL(0x50010000);
    WriteCmd.writeHeader.offset  = TPM_H2NL(0x00000000);

    gBS->SetMem(WriteCmd.data, NV_DATA_SIZE, 0);
    gBS->CopyMem(WriteCmd.data, NvBIOSflags, sizeof(PERSISTENT_BIOS_TPM_FLAGS));

    Status = tcgSvc->PassThroughToTpm ( tcgSvc, \
                                        sizeof(TPM_NV_WRITE_CMD_HDR) +  NV_DATA_SIZE, \
                                        (UINT8*)&WriteCmd, \
                                        sizeof(TPM_NV_WRITE_RET), \
                                        (UINT8*)&WriteRet );

    RetCode = WriteRet.returnCode;

    if ( EFI_ERROR(Status) || (WriteRet.returnCode != 0))
    {

        DEBUG((DEBUG_ERROR, "(TPM Error) Status: %r; RetCode: %x.\n", \
               Status, \
               TPM_H2NL(WriteRet.returnCode)));
//wait for proper return codes
        while(TPM_H2NL(RetCode)==INTERNAL_TPM_DOING_SELFTEST)
        {

            gBS->Stall (500000); //stall 500ms for Selftest to complete

            Status = tcgSvc->PassThroughToTpm ( tcgSvc, \
                                                sizeof(TPM_NV_WRITE_CMD_HDR) +  NV_DATA_SIZE, \
                                                (UINT8*)&WriteCmd, \
                                                sizeof(TPM_NV_WRITE_RET), \
                                                (UINT8*)&WriteRet );

            RetCode = WriteRet.returnCode;
            if(Count == 0)break;
            Count-=1;  //total timeout is 5minutes for define the space
        }

    }


    TpmDevice->Close( TpmDevice );


    if(RetCode != 0)
    {
        //use NVRAM
        Status = gRT->GetVariable( L"TPMPERBIOSFLAGS", \
                                   &FlagsStatusguid, \
                                   NULL, \
                                   &Size, \
                                   &TpmNvflags );

        if(EFI_ERROR(Status))
        {
            SetMem(&Interface, sizeof(Interface), 0);
            Interface.Interface = 1;
            Status =  TcgSetVariableWithNewAttributes(L"TcgInterfaceVar", &mAmiTcgInterfaceVarGuid, \
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                      BkupSize, &Interface);
        }

        if(!EFI_ERROR(Status))
        {
            TpmNvflags.NoPpiProvision = TRUE;
            TpmNvflags.NoPpiClear = FALSE;
            TpmNvflags.NoPpiMaintenance = FALSE;

            Status =  TcgSetVariableWithNewAttributes(L"INTERNALPERBIOSFLAGS", &mAmiPpiInternalVarGuid, \
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                      Size, &TpmNvflags);
        }

        return Status;
    }
    else
    {

#if NVRAM_VERSION > 6
        Status =  TcgSetVariableWithNewAttributes(L"TPMPERBIOSFLAGS", &FlagsStatusguid, \
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,\
                  sizeof (PERSISTENT_BIOS_TPM_FLAGS), &NvBIOSflags);
#else
        Status =  TcgSetVariableWithNewAttributes(L"TPMPERBIOSFLAGS", &FlagsStatusguid, \
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                  sizeof (PERSISTENT_BIOS_TPM_FLAGS), &NvBIOSflags);
        return Status;
    }
#endif
#endif

    return EFI_SUCCESS;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcmSet_Persistent_Bios_TPM_Flags
//
// Description: Sets persistent bios flags in TPM NV
//
//
// Input:       IN PERSISTENT_BIOS_TPM_FLAGS *
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
EFI_STATUS TcmSet_Persistent_Bios_TPM_Flags(PERSISTENT_BIOS_TPM_FLAGS *NvBIOSflags)
{
    return EFI_UNSUPPORTED;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcmRead_Persistent_Bios_TPM_Flags
//
// Description: Sets persistent bios flags in TPM NV
//
//
// Input:       IN PERSISTENT_BIOS_TPM_FLAGS *
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
EFI_STATUS TcmRead_Persistent_Bios_TPM_Flags(PERSISTENT_BIOS_TPM_FLAGS *NvBIOSflags)
{
    return EFI_UNSUPPORTED;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   Read_Persistent_Bios_TPM_Flags
//
// Description: Read persistent bios flags in TPM NV
//
//
// Input:       IN PERSISTENT_BIOS_TPM_FLAGS *
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
EFI_STATUS Read_Persistent_Bios_TPM_Flags(  PERSISTENT_BIOS_TPM_FLAGS *NvBIOSflags)
{
    EFI_STATUS              Status = EFI_SUCCESS;
#if USE_BIOSNVRAM_ONLY == 0
    TPM_NV_READ_CMD         ReadCmd;
    TPM_NV_READ_RET         ReadRet;
#endif
    EFI_TCG_PROTOCOL        *tcgSvc;
    EFI_TPM_DEVICE_PROTOCOL *TpmDevice;

    PERSISTENT_BIOS_TPM_FLAGS  TpmNvflags;
    INTERNAL_BIOS_TPM_PPI_INTERFACE Interface;
    UINTN                       Size = sizeof(PERSISTENT_BIOS_TPM_FLAGS);
    UINTN                   BkupSize = sizeof(INTERNAL_BIOS_TPM_PPI_INTERFACE);


    if(NvBIOSflags == NULL)return EFI_INVALID_PARAMETER;

    Status = gBS->LocateProtocol( &gEfiTpmDeviceProtocolGuid,NULL, &TpmDevice);
    if ( EFI_ERROR( Status ))
    {
        return EFI_NOT_FOUND;
    }

    Status = gBS->LocateProtocol( &gEfiTcgProtocolGuid, NULL, &tcgSvc );
    if ( EFI_ERROR( Status ))
    {
        DEBUG((DEBUG_ERROR, "Error: failed to locate TCG protocol: %r\n"));
        return EFI_NOT_FOUND;
    }


    Status = gRT->GetVariable(L"TcgInterfaceVar", &mAmiTcgInterfaceVarGuid, NULL, &BkupSize, &Interface);
    if(!EFI_ERROR(Status) && (Interface.Interface == 1)) //use NVRAM
    {
        //get variable from BIOS NVRAM
        Status = gRT->GetVariable( L"INTERNALPERBIOSFLAGS", \
                                   &mAmiPpiInternalVarGuid, \
                                   NULL, \
                                   &Size, \
                                   &TpmNvflags );
        if(!EFI_ERROR(Status))
        {

            gBS->CopyMem (NvBIOSflags, &TpmNvflags, sizeof(PERSISTENT_BIOS_TPM_FLAGS));
            return EFI_SUCCESS;
        }
        else
        {

            NvBIOSflags->NoPpiProvision = TRUE;
            NvBIOSflags->NoPpiClear = FALSE;
            NvBIOSflags->NoPpiMaintenance = FALSE;
            NvBIOSflags->Ppi1_3_Flags.PpRequiredForChangePCRS = TRUE;
            return EFI_SUCCESS;
        }
    }

#if USE_BIOSNVRAM_ONLY == 1
    return Status;
#else

    TpmDevice->Init( TpmDevice );

    //read space
    ReadCmd.dataSize    = TPM_H2NL( NV_DATA_SIZE );
    ReadCmd.tag         = TPM_H2NS(TPM_TAG_RQU_COMMAND);
    ReadCmd.paramSize = TPM_H2NL( sizeof(TPM_NV_READ_CMD));
    ReadCmd.ordinal = TPM_H2NL(TPM_ORD_NV_ReadValue);
    ReadCmd.nvIndex = TPM_H2NL(0x50010000);
    ReadCmd.offset  = TPM_H2NL(0x00000000);

    Status = tcgSvc->PassThroughToTpm ( tcgSvc, \
                                        sizeof(TPM_NV_READ_CMD) , \
                                        (UINT8*)&ReadCmd, \
                                        (sizeof(TPM_NV_READ_CMD) - 4 + NV_DATA_SIZE), \
                                        (UINT8*)&ReadRet );

    if ( EFI_ERROR(Status) || (ReadRet.returnCode != 0))
        DEBUG((DEBUG_ERROR, "(TPM Error) Status: %r; RetCode: %x.\n", \
               Status, \
               TPM_H2NL(ReadRet.returnCode)));



    TpmDevice->Close( TpmDevice );

    if( TPM_H2NL(ReadRet.returnCode) == INTERNAL_TPM_BADINDEX )
    {

        Status = gRT->GetVariable( L"TPMPERBIOSFLAGS", \
                                   &FlagsStatusguid, \
                                   NULL, \
                                   &Size, \
                                   &TpmNvflags );

        if(!EFI_ERROR(Status))
        {
            //error condition change to use NVRAM
            SetMem(&Interface, sizeof(Interface), 0);
            Interface.Interface = 1;

            Status =  TcgSetVariableWithNewAttributes(L"TcgInterfaceVar", &mAmiTcgInterfaceVarGuid, \
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                      BkupSize, &Interface);

            if(!EFI_ERROR(Status))
            {
                TpmNvflags.NoPpiProvision = TRUE;
                TpmNvflags.NoPpiClear = FALSE;
                TpmNvflags.NoPpiMaintenance = FALSE;
                TpmNvflags.Ppi1_3_Flags.PpRequiredForChangePCRS = TRUE;

                Status =  TcgSetVariableWithNewAttributes(L"INTERNALPERBIOSFLAGS", &mAmiPpiInternalVarGuid, \
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                          Size, &TpmNvflags);
            }

            NvBIOSflags->NoPpiProvision = TRUE;
            NvBIOSflags->NoPpiClear = FALSE;
            NvBIOSflags->NoPpiMaintenance = FALSE;
            NvBIOSflags->Ppi1_3_Flags.PpRequiredForChangePCRS = TRUE;

            return EFI_SUCCESS;
        }
        else
        {
            if(Internal_flag == 1)
            {
                return EFI_NOT_AVAILABLE_YET;
            }
        }
    }

    if( ReadRet.returnCode != 0)
    {
        //error condition change to use NVRAM
        SetMem(&Interface, sizeof(Interface), 0);
        Interface.Interface = 1;

        Status =  TcgSetVariableWithNewAttributes(L"TcgInterfaceVar", &mAmiTcgInterfaceVarGuid, \
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                  BkupSize, &Interface);

        if(!EFI_ERROR(Status))
        {
            TpmNvflags.NoPpiProvision = TRUE;
            TpmNvflags.NoPpiClear = FALSE;
            TpmNvflags.NoPpiMaintenance = FALSE;

            TcgSetVariableWithNewAttributes(L"INTERNALPERBIOSFLAGS", &mAmiPpiInternalVarGuid, \
                                            EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                                            Size, &TpmNvflags);
        }

        NvBIOSflags->NoPpiProvision = TRUE;
        NvBIOSflags->NoPpiClear = FALSE;
        NvBIOSflags->NoPpiMaintenance = FALSE;
        NvBIOSflags->Ppi1_3_Flags.PpRequiredForChangePCRS = TRUE;
        return EFI_SUCCESS;
    }

    gBS->CopyMem (NvBIOSflags, ReadRet.data, sizeof(PERSISTENT_BIOS_TPM_FLAGS));

#endif
    return EFI_SUCCESS;
}





//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   NvSendTpmCommand
//
// Description: Sends a command to the TPM
//
// Input:   *EFI_TCG_PROTOCOL   - Tcg Protocol type.
//          UINT32              - Tcg Command ordinal.
//          int                 - cmd datasize.
//          VOID*               - cmd data.
//
// Output:  UINT32              - TPM_RESULT
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT32
NvSendTpmCommand    (
    IN EFI_TCG_PROTOCOL     *tcg,
    IN UINT32               ord,
    IN int                  dataSize,
    IN VOID                 *data
)
{
    EFI_STATUS              Status;
    static UINT8            result[0x400];
    TPM_1_2_RET_HEADER*     tpmResult;
    struct
    {
        TPM_1_2_CMD_HEADER  hdr;
        UINT8               data[0x100];
    } cmd;

    cmd.hdr.Tag = TPM_H2NS (TPM_TAG_RQU_COMMAND);
    cmd.hdr.ParamSize = TPM_H2NL (sizeof(TPM_1_2_RET_HEADER) + dataSize);
    cmd.hdr.Ordinal = TPM_H2NL (ord);
    gBS->CopyMem(cmd.data, data, dataSize);
    Status = tcg->PassThroughToTpm (    tcg, \
                                        sizeof(TPM_1_2_CMD_HEADER) + dataSize, \
                                        (UINT8*)&cmd, \
                                        sizeof (result), \
                                        (UINT8*)&result );
    tpmResult = (TPM_1_2_RET_HEADER*)result;
    if ( EFI_ERROR(Status) || (tpmResult->RetCode != 0))
        DEBUG((DEBUG_ERROR, "TXT (TPM Error) Status: %r; RetCode: %x.\n", \
               Status, \
               TPM_H2NL(tpmResult->RetCode)    ));
    return tpmResult->RetCode;
}



PERSISTENT_BIOS_TPM_MANAGEMENT_FLAGS_PROTOCOL   PersistentBiosflagsManagementProtocol =
{
    Read_Persistent_Bios_TPM_Flags,
    Set_Persistent_Bios_TPM_Flags
};

PERSISTENT_BIOS_TPM_MANAGEMENT_FLAGS_PROTOCOL   TcmPersistentBiosflagsManagementProtocol =
{
    TcmRead_Persistent_Bios_TPM_Flags,
    TcmSet_Persistent_Bios_TPM_Flags
};



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   AmiTcgPpiNvflagEntry
//
// Description: Entry point
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
EFIAPI AmiTcgPpiNvflagEntry (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS                  Status;
    EFI_TCG_PROTOCOL            *tcgSvc;
    EFI_TPM_DEVICE_PROTOCOL     *TpmDevice;
    PERSISTENT_BIOS_TPM_FLAGS   NvBIOSflags;
    UINTN                       Count = 10;
#if USE_BIOSNVRAM_ONLY == 0
    TPM_RESULT                  RetCode;
#endif

    UINTN     Size = sizeof(PERSISTENT_BIOS_TPM_FLAGS);
    TPM_DEF_NV_DATA  CmdDefineNvram [] = { \
                                           // TPM_NV_DATA_PUBLIC1 & TPM_NV_DATA_PUBLIC1.TPM_PCR_INFO_SHORT
                                           TPM_H2NS(0x18), TPM_H2NL(0x50010000), TPM_H2NS(0x03), 0, 0, 0 , 0x1f, \
                                           // TPM_NV_DATA_PUBLIC1.TPM_PCR_INFO_SHORT.TCPA_DIGEST
                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                                           // TPM_NV_DATA_PUBLIC2.TPM_PCR_INFO_SHORT
                                           TPM_H2NS(0x3), 0, 0, 0 , 0x1f, \
                                           // TPM_NV_DATA_PUBLIC2.TPM_PCR_INFO_SHORT.TCPA_DIGEST
                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                                           // TPM_NV_DATA_PUBLIC2
                                           TPM_H2NS(0x17), TPM_H2NL(0x00000001), 0, 0, 0, TPM_H2NL(NV_DATA_SIZE), \
                                           // TCPA_DIGEST
                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                         };

//    InitAmiLib( ImageHandle, SystemTable );

    Status = gBS->LocateProtocol( &gEfiTpmDeviceProtocolGuid,NULL, &TpmDevice);
    if ( EFI_ERROR( Status ))
    {
        return EFI_NOT_FOUND;
    }

    Status = gBS->LocateProtocol( &gEfiTcgProtocolGuid, NULL, &tcgSvc );
    if ( EFI_ERROR( Status ))
    {
        DEBUG((DEBUG_ERROR, "Error: failed to locate TCG protocol: %r\n"));
        return EFI_NOT_FOUND;
    }

    if(IsTcmSupportType())
    {

        Status = gBS->InstallProtocolInterface(
                     &ImageHandle,
                     &AmiBiosPpiFlagsManagementGuid,
                     EFI_NATIVE_INTERFACE,
                     &TcmPersistentBiosflagsManagementProtocol);

        return Status;
    }

    Internal_flag = 1;

    Status = Read_Persistent_Bios_TPM_Flags(&NvBIOSflags);

#if USE_BIOSNVRAM_ONLY == 1
    if(EFI_ERROR(Status))
    {
        //error, set default policy
        NvBIOSflags.NoPpiProvision        = NO_PPI_PROVISION_DEFAULT;
        NvBIOSflags.NoPpiClear            = NO_PPI_CLEAR_DEFAULT;
        NvBIOSflags.NoPpiMaintenance      = NO_PPI_MAINTENANCE_DEFAULT;
        Set_Persistent_Bios_TPM_Flags(&NvBIOSflags);
    }
#else

    TpmDevice->Init( TpmDevice );


    //analyze return code
    if(Status == EFI_NOT_AVAILABLE_YET)
    {
        //it might not be defined so define it

        SendSelfTest();   //make sure we send selftest before defining space

        RetCode = NvSendTpmCommand  ( tcgSvc, \
                                      TPM_ORD_NV_DefineSpace, \
                                      sizeof(TPM_DEF_NV_DATA), \
                                      &CmdDefineNvram );

        if(RetCode != 0)
        {
            while(TPM_H2NL(RetCode) == INTERNAL_TPM_DOING_SELFTEST)
            {

                gBS->Stall (500000); //stall 500ms for Selftest to complete

                RetCode = NvSendTpmCommand  ( tcgSvc, \
                                              TPM_ORD_NV_DefineSpace, \
                                              sizeof(TPM_DEF_NV_DATA), \
                                              &CmdDefineNvram );

                if(Count == 0)break;
                Count-=1;
            }
        }

        Internal_flag = 0;

        //read again to make sure we can read TPM Indices
        Status = Read_Persistent_Bios_TPM_Flags(&NvBIOSflags);
        if(Status)return EFI_ABORTED;

        //if read is successful set the default values only once
        //we only do this here because of the protections on TPM Nvram
        //writes without owner. Do this only after defining the index
        NvBIOSflags.NoPpiProvision      = NO_PPI_PROVISION_DEFAULT;
        NvBIOSflags.NoPpiClear          = NO_PPI_CLEAR_DEFAULT;
        NvBIOSflags.NoPpiMaintenance    = NO_PPI_MAINTENANCE_DEFAULT;

        Set_Persistent_Bios_TPM_Flags(&NvBIOSflags);
    }
    else if(Status)
    {
        return EFI_ABORTED;
    }
    else
    {
        //already allocated but check for first boot.
        Status = gRT->GetVariable( L"TPMPERBIOSFLAGS", \
                                   &FlagsStatusguid, \
                                   NULL, \
                                   &Size, \
                                   &NvBIOSflags );

        if(Status == EFI_NOT_FOUND)
        {
            NvBIOSflags.NoPpiProvision      = NO_PPI_PROVISION_DEFAULT;
            NvBIOSflags.NoPpiClear          = NO_PPI_CLEAR_DEFAULT;
            NvBIOSflags.NoPpiMaintenance    = NO_PPI_MAINTENANCE_DEFAULT;
            Set_Persistent_Bios_TPM_Flags(&NvBIOSflags);
        }
    }

    TpmDevice->Close( TpmDevice );
#endif
    Status = gBS->InstallProtocolInterface(
                 &ImageHandle,
                 &AmiBiosPpiFlagsManagementGuid,
                 EFI_NATIVE_INTERFACE,
                 &PersistentBiosflagsManagementProtocol);

    return Status;
}



//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
