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

/** @file IpmiSdrReader.c
    Server Management IPMI SDR reader Driver. Reads the SDR
    version details from the BMC. Updates the SDR version in Dynamic PCD.

**/

//----------------------------------------------------------------------

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Include/IpmiNetFnStorageDefinitions.h>
#include <Protocol/IPMITransportProtocol.h>

//----------------------------------------------------------------------

EFI_IPMI_TRANSPORT      *gIpmiTransport = NULL;

/**
    Read the SDR version details from the BMC.
    Updates the SDR version in Dynamic PCD.

    @param VOID

    @retval EFI_STATUS Return Status

**/

EFI_STATUS
GetSdrVerNum (
  VOID )
{
    EFI_STATUS                          Status;
    IPMI_GET_SDR_REPOSITORY_INFO        SdrInfo;
    UINT8                               SdrInfoSize = sizeof (SdrInfo);
    UINT16                              *SdrVersionNo = NULL;
    UINTN                               SizeofBuffer;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... \n", __FUNCTION__));

    Status = gBS->LocateProtocol (
                    &gEfiDxeIpmiTransportProtocolGuid,
                    NULL,
                    (VOID **)&gIpmiTransport );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiDxeIpmiTransportProtocolGuid Status: %r \n", Status));
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Get SDR Version using Get SDR Repository Info command
    //
    Status = gIpmiTransport->SendIpmiCommand (
                                gIpmiTransport,
                                IPMI_NETFN_STORAGE,
                                BMC_LUN,
                                IPMI_STORAGE_GET_SDR_REPOSITORY_INFO,
                                NULL,
                                0,
                                (UINT8 *) &SdrInfo,
                                (UINT8 *) &SdrInfoSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_STORAGE_GET_SDR_REPOSITORY_INFO Status: %r\n", Status ));
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Got the required SDR version Number in BCD format
    // Convert it to Unicode format and Update the same in PCD
    //
    SdrVersionNo = AllocateRuntimeZeroPool ( (UINTN)( sizeof (UINT16) * 4 ) );  //Allocate size for two nibbles and a dot as a Unicode string with NULL terminator.
    if (SdrVersionNo == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SdrVersionNo Address : %x\n", SdrVersionNo));

    //
    // Version will be in format LSB.MSB example. 51h as 1.5
    //
    SdrVersionNo[0] = (UINT8)((SdrInfo.Version & 0x0F) + 0x30); // LSB of Version
    SdrVersionNo[1] = '.'; // Dot Separates LSB and MSB
    SdrVersionNo[2] = (UINT8)((SdrInfo.Version >> 0x04) + 0x30); // MSB of Version

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SdrVersionNo: %S \n", SdrVersionNo));

    SizeofBuffer = StrSize (SdrVersionNo);
    PcdSetPtr (PcdSdrVersionNo, &SizeofBuffer, (VOID*)SdrVersionNo);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SdrVersionNo: %S\n", (CHAR16 *) PcdGetPtr (PcdSdrVersionNo)));

    FreePool (SdrVersionNo);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting... \n", __FUNCTION__));
    return EFI_SUCCESS;
}

/**
    Calls routine to Read the SDR version details from the BMC and
    Update the SDR version in Dynamic PCD.

    @param ImageHandle - Handle of this driver image
    @param SystemTable - Table containing standard EFI services

    @retval EFI_SUCCESS Successful driver initialization

**/

EFI_STATUS
InitializeIpmiSdrReader (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable )
{
    //
    // Read the SDR Version. This is BMC SDR records dependent.
    //
    return GetSdrVerNum();
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
