//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Support a feature that Update SMBIOS Type 1 and 3 with Fru0.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     May/12/2017
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Jan/19/2015
//
//****************************************************************************
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

/** @file IpmiRedirFru.c
    Server Management IPMI Redir FRU Driver.
    This REDIR FRU driver attaches to the Generic FRU driver.

**/

//----------------------------------------------------------------------

#include "IpmiRedirFru.h"
#include <Guid/EventGroup.h>

//----------------------------------------------------------------------

EFI_IPMI_FRU_GLOBAL     *gIpmiFruGlobal;
EFI_IPMI_TRANSPORT      *gIpmiTransport = NULL;

/**
    Notification function for Virtual address change event.
    Convert all addresses inside IpmiRedirFru Protocol

    @param Event - Event which caused this handler
    @param Context - Context passed during Event Handler registration

    @retval VOID

**/

VOID
EFIAPI
IpmiRedirFruVirtualAddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{
    //
    // Change all addresses inside IpmiRedirFruProtocol stored locally
    //
    gRT->ConvertPointer (
            0,
            (VOID **) &(gIpmiFruGlobal->IpmiRedirFruProtocol.GetFruRedirInfo));
    gRT->ConvertPointer (
            0,
            (VOID **) &(gIpmiFruGlobal->IpmiRedirFruProtocol.GetFruSlotInfo));
    gRT->ConvertPointer (
            0,
            (VOID **) &(gIpmiFruGlobal->IpmiRedirFruProtocol.GetFruRedirData));
    gRT->ConvertPointer (
            0,
            (VOID **) &(gIpmiFruGlobal->IpmiRedirFruProtocol.SetFruRedirData));

    return;
}

/**
    Returns the empty FRU slot to fill the new entry.

    @param VOID

    @return UINT8
    @retval empty FRU slot number
    @retval Returns 0xFF if empty slot is not found

**/

UINT8
GetEmptyFruSlot (
  VOID )
{
    UINT8       i;

    for (i = 1; i < gIpmiFruGlobal->MaxFruSlots; i++) {
        if (!gIpmiFruGlobal->FruDeviceInfo[i].Valid) {
            return i;
        }
    }
    return 0xFF;
}

/**
    This routine Returns overall the size of the FRU Inventory Area in this
    device, in bytes.

     
    @param This - Pointer to the Redir FRU protocol
    @param FruSlotNumber - This is the Slot Number of the  this FRU Type. This
                     instance represents Slot Number in case of Memory, CPU etc
    @param FruFormatGuid - FRU Format GUID. This GUID represents a format of
                         FRU we are dealing with. This GUID can also be OEM specific.
    @param DataAccessGranularity - Data Access granularity  in bytes.
    @param FruInformationString - This is a human readable string that can has
               information that can be useful to a user while
                reading through a setup or terminal.

    @return EFI_STATUS Return Status
    @retval EFI_SUCCESS FRU Data info retrieved  successful.
    @retval EFI_UNSUPPORTED Data is not available yet.
    @retval EFI_INVALID_PARAMETER Invalid Parameter
    @retval EFI_SECURITY_VIOLATION User not allowed to read this information

**/

EFI_STATUS
EFIAPI
EfiGetFruRedirInfo (
  IN  EFI_SM_FRU_REDIR_PROTOCOL           *This,
  IN  UINTN                               FruSlotNumber,
  OUT EFI_GUID                            *FruFormatGuid,
  OUT UINTN                               *DataAccessGranularity,
  OUT CHAR16                              **FruInformationString )
{
    EFI_IPMI_FRU_GLOBAL             *FruPrivate = NULL;
    EFI_GUID                        Guid = EFI_IPMI_FORMAT_FRU_GUID;

    FruPrivate = INSTANCE_FROM_EFI_SM_IPMI_FRU_THIS (This);
    if ((FruSlotNumber + 1) > FruPrivate->NumSlots) {
        return EFI_NO_MAPPING;
    }

    CopyMem (FruFormatGuid, &Guid, sizeof (EFI_GUID));

    *DataAccessGranularity = 0x1;
    *FruInformationString = NULL;

    return EFI_SUCCESS;
}

/**
    This routine Returns the SLOT information in case of FRU. This slot info
    could represent a SLOT NUMBER in case of Memory, CPU or simple instances
    in other cases.

    @param This - Pointer to the Redir FRU protocol
    @param FruTypeGuid - This is the TYPE GUID represented by this protocol.
    @param StartFruSlotNumber - This is the Starting slot number for this
              instance of TYPE GUID.
    @param NumSlots - This is the number of slots for this instance of  TYPE GUID

    @return EFI_STATUS Return Status
    @retval EFI_SUCCESS FRU Data info retrieved successful.
    @retval EFI_UNSUPPORTED Data is not available yet.
    @retval EFI_INVALID_PARAMETER Invalid Parameter
    @retval EFI_SECURITY_VIOLATION User not allowed to read this information

**/

EFI_STATUS
EFIAPI
EfiGetFruSlotInfo (
  IN  EFI_SM_FRU_REDIR_PROTOCOL           *This,
  OUT EFI_GUID                            *FruTypeGuid,
  OUT UINTN                               *StartFruSlotNumber,
  OUT UINTN                               *NumSlots )
{
    EFI_IPMI_FRU_GLOBAL          *FruPrivate = NULL;

    EFI_GUID  Guid = EFI_SYSTEM_TYPE_FRU_GUID;

    FruPrivate = INSTANCE_FROM_EFI_SM_IPMI_FRU_THIS (This);

    CopyMem (FruTypeGuid, &Guid, sizeof (EFI_GUID));
    *StartFruSlotNumber = 0;
    *NumSlots = FruPrivate->NumSlots;
    return EFI_SUCCESS;
}

/**
    The API returns the specified data from the FRU Inventory Info area. This is
    effectively a ‘low level’ direct interface to a non -volatile storage area.

    @param This - Pointer to the Redir FRU protocol
    @param FruSlotNumber - This is the Slot Number of the this FRU Type. This
             instance represents Slot Number in case of Memory, CPU etc
    @param FruDataOffset - This is the Data Offset from where FRU is to be read.
    @param FruDataSize - Size of the FRU Data to be read.
    @param FruData - Pointer to the buffer, where FRU data will be returned.

    @return EFI_STATUS Return Status
    @retval EFI_SUCCESS FRU Data info retrieved successful.
    @retval EFI_UNSUPPORTED Data is not available yet.
    @retval EFI_INVALID_PARAMETER Invalid Parameter
    @retval EFI_SECURITY_VIOLATION User not allowed to read this information

**/

EFI_STATUS
EFIAPI
EfiGetFruRedirData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN UINTN                                FruSlotNumber,
  IN UINTN                                FruDataOffset,
  IN UINTN                                FruDataSize,
  IN UINT8                                *FruData )
{
    EFI_IPMI_FRU_GLOBAL           *FruPrivate = NULL;
    IPMI_FRU_READ_COMMAND         FruReadCommandData = {0};
    UINT8                         ResponseData[20];
    UINT8                         ResponseDataSize;
    UINT16                        PointerOffset = 0;
    EFI_STATUS                    Status;
    UINT8                         Counter = IPMI_FRU_COMMAND_RETRY_COUNTER;

    FruPrivate = INSTANCE_FROM_EFI_SM_IPMI_FRU_THIS (This);

#if SMCPKG_SUPPORT == 0
    if ((FruSlotNumber + 1) > FruPrivate->NumSlots) {
        return EFI_NO_MAPPING;
    }
#endif

#if SMCPKG_SUPPORT
    if(1){
#else
    if (FruPrivate->FruDeviceInfo[FruSlotNumber].FruDevice.LogicalFruDevice) {
#endif
        //
        // Create the FRU Read Command for the logical FRU Device.
        //
#if SMCPKG_SUPPORT
        FruReadCommandData.Data.FruDeviceId = 0x01;//FruPrivate->FruDeviceInfo[FruSlotNumber].FruDevice.FruDeviceId;
#else
        FruReadCommandData.Data.FruDeviceId = FruPrivate->FruDeviceInfo[FruSlotNumber].FruDevice.FruDeviceId;
#endif
        FruReadCommandData.Data.FruOffset = (UINT16) FruDataOffset;

        //
        // Collect the data till it is completely retrieved.
        //
        while (FruDataSize > 0) {
            //
            // BMC allows to get only 16 bytes with in a single command.
            // Data more than 16 bytes will be received as part by part of 16 bytes
            //
            if (FruDataSize > 0x10) {
                FruReadCommandData.Count = 0x10;
            } else {
                FruReadCommandData.Count = (UINT8)FruDataSize;
            }

            //
            // Read FRU data
            //
            while (TRUE) {
                ResponseDataSize = sizeof (ResponseData);
                Status = gIpmiTransport->SendIpmiCommand (
                            gIpmiTransport,
                            IPMI_NETFN_STORAGE,
                            BMC_LUN,
                            IPMI_STORAGE_READ_FRU_DATA,
                            (UINT8 *) &FruReadCommandData,
                            sizeof (FruReadCommandData),
                            (UINT8 *) &ResponseData,
                            (UINT8 *) &ResponseDataSize );
                if (EFI_ERROR(Status)) {
                    return Status;
                }

                if (gIpmiTransport->CommandCompletionCode == 0x81) {
                    if (Counter-- == 0) {
                      return EFI_NOT_READY;
                    }
                    MicroSecondDelay (IPMI_FRU_COMMAND_RETRY_DELAY);
                } else {
                    break;
                }
            }

            //
            // In case of partial retrieval; Data[0] contains the retrieved data size;
            //
            ResponseData[0] = ResponseData[0] <= FruReadCommandData.Count \
                            ? ResponseData[0] \
                            : FruReadCommandData.Count ;
            FruDataSize = (UINT16) (FruDataSize - ResponseData[0]);    // Remaining Count
            FruReadCommandData.Data.FruOffset = (UINT16) (FruReadCommandData.Data.FruOffset + ResponseData[0]);  // Next Offset to retrieve
            CopyMem (&FruData[PointerOffset], &ResponseData[1], ResponseData[0]);  // Copy the partial data
            PointerOffset = (UINT16) (PointerOffset + ResponseData[0]);  // Next offset to the input pointer.
        } //while (FruDataSize > 0)
    } else {
        return  EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
}

#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
EFI_STATUS
EFIAPI
SmcEfiGetFruRedirData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN UINTN                                FruSlotNumber,
  IN UINTN                                FruDataOffset,
  IN UINTN                                FruDataSize,
  IN UINT8                                *FruData )
{
    EFI_IPMI_FRU_GLOBAL           *FruPrivate = NULL;
    IPMI_FRU_READ_COMMAND         FruReadCommandData = {0};
    UINT8                         ResponseData[20];
    UINT8                         ResponseDataSize;
    UINT16                        PointerOffset = 0;
    EFI_STATUS                    Status;
    UINT8                         Counter = IPMI_FRU_COMMAND_RETRY_COUNTER;

    FruPrivate = INSTANCE_FROM_EFI_SM_IPMI_FRU_THIS (This);

//    if ((FruSlotNumber + 1) > FruPrivate->NumSlots) {
//        return EFI_NO_MAPPING;
//    }

//    if (FruPrivate->FruDeviceInfo[FruSlotNumber].FruDevice.LogicalFruDevice) {
    if(1) {
        //
        // Create the FRU Read Command for the logical FRU Device.
        //
//        FruReadCommandData.Data.FruDeviceId = FruPrivate->FruDeviceInfo[FruSlotNumber].FruDevice.FruDeviceId;
        FruReadCommandData.Data.FruDeviceId = 0x00;
        FruReadCommandData.Data.FruOffset = (UINT16) FruDataOffset;

        //
        // Collect the data till it is completely retrieved.
        //
        while (FruDataSize > 0) {
            //
            // BMC allows to get only 16 bytes with in a single command.
            // Data more than 16 bytes will be received as part by part of 16 bytes
            //
            if (FruDataSize > 0x10) {
                FruReadCommandData.Count = 0x10;
            } else {
                FruReadCommandData.Count = (UINT8)FruDataSize;
            }

            //
            // Read FRU data
            //
            while (TRUE) {
                ResponseDataSize = sizeof (ResponseData);
                Status = gIpmiTransport->SendIpmiCommand (
                            gIpmiTransport,
                            IPMI_NETFN_STORAGE,
                            BMC_LUN,
                            IPMI_STORAGE_READ_FRU_DATA,
                            (UINT8 *) &FruReadCommandData,
                            sizeof (FruReadCommandData),
                            (UINT8 *) &ResponseData,
                            (UINT8 *) &ResponseDataSize );
                if (EFI_ERROR(Status)) {
                    return Status;
                }

                if (gIpmiTransport->CommandCompletionCode == 0x81) {
                    if (Counter-- == 0) {
                      return EFI_NOT_READY;
                    }
                    MicroSecondDelay (IPMI_FRU_COMMAND_RETRY_DELAY);
                } else {
                    break;
                }
            }

            //
            // In case of partial retrieval; Data[0] contains the retrieved data size;
            //
            ResponseData[0] = ResponseData[0] <= FruReadCommandData.Count \
                            ? ResponseData[0] \
                            : FruReadCommandData.Count ;
            FruDataSize = (UINT16) (FruDataSize - ResponseData[0]);    // Remaining Count
            FruReadCommandData.Data.FruOffset = (UINT16) (FruReadCommandData.Data.FruOffset + ResponseData[0]);  // Next Offset to retrieve
            CopyMem (&FruData[PointerOffset], &ResponseData[1], ResponseData[0]);  // Copy the partial data
            PointerOffset = (UINT16) (PointerOffset + ResponseData[0]);  // Next offset to the input pointer.
        } //while (FruDataSize > 0)
    } else {
        return  EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
}
#endif //SMC_UPDATE_DMI_WITH_FRU0_SUPPORT

/**
    The command writes the specified bytes to the FRU Inventory Info area.
    This is a low level direct interface to a non -volatile storage area.
    Updating the FRU Inventory Data is presumed to be a system level,
    privileged operation. There is no requirement for devices implementing
    this API to provide mechanisms for rolling back the FRU Inventory Area
    in the case of incomplete or incorrect writes

    @param This - Pointer to the Redir FRU protocol
    @param FruSlotNumber - This is the Slot Number of the  this FRU Type. This
        instance represents Slot Number in case of Memory, CPU etc
    @param FruDataOffset - This is the Data Offset to where FRU is to be written
    @param FruDataSize - Size of the FRU Data to be written.
    @param FruData - Pointer to the buffer, from where FRU data will be read.

    @return EFI_STATUS Return Status
    @retval EFI_SUCCESS FRU Data written successfully.
    @retval EFI_UNSUPPORTED Data Writes are Un-Supported
    @retval EFI_INVALID_PARAMETER Invalid Parameter
    @retval EFI_SECURITY_VIOLATION User not allowed to write to the FRU.

**/

EFI_STATUS
EFIAPI
EfiSetFruRedirData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN UINTN                                FruSlotNumber,
  IN UINTN                                FruDataOffset,
  IN UINTN                                FruDataSize,
  IN UINT8                                *FruData )
{
    EFI_IPMI_FRU_GLOBAL           *FruPrivate = NULL;
    IPMI_FRU_WRITE_COMMAND        FruWriteCommandData;
    UINT8                         Count;
    UINT8                         ResponseData[20];
    UINT8                         ResponseDataSize;
    UINT16                        PointerOffset = 0;
    EFI_STATUS                    Status;
    UINT8                         Counter = IPMI_FRU_COMMAND_RETRY_COUNTER;

    FruPrivate = INSTANCE_FROM_EFI_SM_IPMI_FRU_THIS (This);

    if ((FruSlotNumber + 1) > FruPrivate->NumSlots) {
        return EFI_NO_MAPPING;
    }

    if (FruPrivate->FruDeviceInfo[FruSlotNumber].FruDevice.LogicalFruDevice) {
        //
        // Create the FRU Read Command for the logical FRU Device.
        //
        FruWriteCommandData.Data.FruDeviceId = FruPrivate->FruDeviceInfo[FruSlotNumber].FruDevice.FruDeviceId;
        FruWriteCommandData.Data.FruOffset = (UINT16) FruDataOffset;

        //
        // Collect the data till it is completely retrieved.
        //
        while (FruDataSize > 0) {
            //
            // BMC allows to set only 16 bytes with in a single command.
            // Data more than 16 bytes will be send as part by part of 16 bytes
            //
            if (FruDataSize > 0x10) {
                Count = 0x10;
            } else {
                Count = (UINT8)FruDataSize;
            }
            CopyMem (FruWriteCommandData.FruData, &FruData[PointerOffset], Count);

            //
            // Write FRU data
            //
            while(TRUE) {
                ResponseDataSize = sizeof (ResponseData);
                Status = gIpmiTransport->SendIpmiCommand (
                            gIpmiTransport,
                            IPMI_NETFN_STORAGE,
                            BMC_LUN,
                            IPMI_STORAGE_WRITE_FRU_DATA,
                            (UINT8 *) &FruWriteCommandData,
                            (sizeof (FruWriteCommandData) + Count),
                            (UINT8 *) &ResponseData,
                            (UINT8 *) &ResponseDataSize );
                if (EFI_ERROR(Status)) {
                      return Status;
                }

                if (gIpmiTransport->CommandCompletionCode == 0x80) {
                      return EFI_WRITE_PROTECTED;
                }

                if (gIpmiTransport->CommandCompletionCode == 0x81) {
                    if (Counter-- == 0) {
                        return EFI_NOT_READY;
                    }
                    MicroSecondDelay (IPMI_FRU_COMMAND_RETRY_DELAY);
                } else {
                    break;
                }
            }

            //
            // In case of partial set; Data[0] contains the size of set;
            //
            FruDataSize = (UINT8) (FruDataSize - ResponseData[0]);    // Remaining Count
            FruWriteCommandData.Data.FruOffset = (UINT16) (FruWriteCommandData.Data.FruOffset + ResponseData[0]);  // Next Offset to set
            PointerOffset = (UINT16) (PointerOffset + ResponseData[0]);  // Next offset to the input pointer.
        } //while (FruDataSize > 0)
    } else {
        return  EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
}

/**
    1. Installs Redir FRU protocol.
    2. Read the FRU information from authenticated device and publish FRU stings
    3. If FRU information is not present then publish dummy FRU stings

    @param ImageHandle - Handle of this driver image
    @param SystemTable - Table containing standard EFI services

    @retval EFI_SUCCESS Successful driver initialization

**/

EFI_STATUS
InitializeSmRedirFruLayer (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable )
{
    EFI_HANDLE                          NewHandle;
    EFI_STATUS                          Status;
    BOOLEAN                             DummyFruRecords = FALSE;
    UINT8                               Index = 0;
    UINT16                              ReserveId = 0;
    UINT8                               ReserveIdSize = sizeof (ReserveId);
    IPMI_GET_SDR_REQUEST                GetSdrCommandData = {0};
    UINT8                               GetSdrResponseData[0xFF];
    UINT8                               GetSdrResponseDataSize = sizeof (GetSdrResponseData);
    IPMI_GET_DEVICE_ID_RESPONSE         GetDeviceIdResponse = {0};
    UINT8                               GetDeviceIdResponseSize = sizeof (GetDeviceIdResponse);
    EFI_EVENT                           EfiVirtualAddressChangeEvent = NULL;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered... \n", __FUNCTION__));

    //
    // Allocate memory for gIpmiFruGlobal
    //   
    gIpmiFruGlobal = (EFI_IPMI_FRU_GLOBAL *)AllocateRuntimeZeroPool (sizeof (EFI_IPMI_FRU_GLOBAL));
    if (gIpmiFruGlobal == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for gIpmiFruGlobal! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Initialize Global memory
    //
    gIpmiFruGlobal->NumSlots = 0;
    gIpmiFruGlobal->IpmiRedirFruProtocol.GetFruRedirInfo  = EfiGetFruRedirInfo;
    gIpmiFruGlobal->IpmiRedirFruProtocol.GetFruSlotInfo   = EfiGetFruSlotInfo;
    gIpmiFruGlobal->IpmiRedirFruProtocol.GetFruRedirData  = EfiGetFruRedirData;
    gIpmiFruGlobal->IpmiRedirFruProtocol.SetFruRedirData  = EfiSetFruRedirData;
    gIpmiFruGlobal->Signature = EFI_SM_FRU_REDIR_SIGNATURE;
    gIpmiFruGlobal->MaxFruSlots = MAX_FRU_SLOT;

    Status = gBS->LocateProtocol (
                &gEfiDxeIpmiTransportProtocolGuid,
                NULL,
                (VOID **)&gIpmiTransport );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiDxeIpmiTransportProtocolGuid Status: %r \n", Status));
    if (EFI_ERROR (Status)) {
        //
        // If IPMI transport protocol is not located so publish Dummy records.
        //
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, " Error in locating IPMI transport protocol so publish Dummy records...\n" ));
        DummyFruRecords = TRUE;
        goto GenerateFruRecords;
    }

#if SMCPKG_SUPPORT
    goto GenerateFruRecords;
#endif
    //
    // Get the device ID information for the BMC.
    //
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_DEVICE_ID,
                NULL,
                0,
                (UINT8 *) &GetDeviceIdResponse,
                (UINT8 *) &GetDeviceIdResponseSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get Device ID status %r\n", Status));
    if (EFI_ERROR (Status)) {
        //
        // If Device id command is not successful then publish Dummy records.
        //
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, " Get Device id command is not successful so publish Dummy records...\n" ));
        DummyFruRecords = TRUE;
        goto GenerateFruRecords;
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "GetDeviceIdResponse.FruInventorySupport %X\n", GetDeviceIdResponse.FruInventorySupport));
    if (GetDeviceIdResponse.FruInventorySupport) {
        //
        // Add a mandatory FRU Inventory device attached to the controller.
        //
        gIpmiFruGlobal->FruDeviceInfo[0].ControllerInstance = 0;
        gIpmiFruGlobal->FruDeviceInfo[0].Valid = TRUE;
        ZeroMem (&gIpmiFruGlobal->FruDeviceInfo[0].FruDevice, sizeof (IPMI_FRU_DATA_INFO));
        gIpmiFruGlobal->FruDeviceInfo[0].FruDevice.LogicalFruDevice = TRUE;
        gIpmiFruGlobal->NumSlots++;

        SERVER_IPMI_DEBUG ((EFI_D_INFO, "*( (UINT16*) &GetDeviceIdResponse.ManufacturerId[0] ) %X\n", *( (UINT16*) &GetDeviceIdResponse.ManufacturerId[0] ) ));
        // 
        // Verify the manufacturer ID and proceed....
        //
        if ((*((UINT16*) &GetDeviceIdResponse.ManufacturerId[0]) == IANA_PEN_IPMI_FORUM) || \
           (*((UINT16*) &GetDeviceIdResponse.ManufacturerId[0]) == IANA_PEN_INTEL) || \
           (*((UINT16*) &GetDeviceIdResponse.ManufacturerId[0]) == OEM_MANF_ID)) {
            //
            // Now we found out a controller that supports FRU Inventory & IPMI Compatible,
            // Start searching for Sensor Information.
            //
#if CUSTOM_FRU_ID_SUPPORT
            gIpmiFruGlobal->FruDeviceInfo[0].FruDevice.FruDeviceId = CUSTOM_FRU_ID;
#else
            //
            // Get Reservation ID for Get SDR Info command
            //
            Status = gIpmiTransport->SendIpmiCommand (
                        gIpmiTransport,
                        IPMI_NETFN_STORAGE,
                        BMC_LUN,
                        IPMI_STORAGE_RESERVE_SDR_REPOSITORY,
                        NULL,
                        0,
                        (UINT8 *) &ReserveId,
                        (UINT8 *) &ReserveIdSize );
            SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_STORAGE_RESERVE_SDR_REPOSITORY Status: %r ReserveId: %x\n", Status, ReserveId ));

            //
            //  Fill Get SDR command Data
            //
            GetSdrCommandData.ReservationId = ReserveId; // Reservation ID
            GetSdrCommandData.RecordId = 0; // Record ID

            while (GetSdrCommandData.RecordId != 0xFFFF) {

                GetSdrCommandData.RecordOffset = 0; // Record Offset
                GetSdrCommandData.BytesToRead = sizeof(IPMI_SDR_RECORD_STRUCT_HEADER); // Bytes to read
                GetSdrResponseDataSize = sizeof (GetSdrResponseData);

                //
                // Clear the GetSdrResponseData
                //
                ZeroMem (&GetSdrResponseData[0], GetSdrResponseDataSize);

                SERVER_IPMI_DEBUG ((EFI_D_INFO, " GetSdrCommandData.RecordId: %x\n", GetSdrCommandData.RecordId ));
                //
                // Get SDR Info command to Get FRU DATA
                //
                Status = gIpmiTransport->SendIpmiCommand (
                            gIpmiTransport,
                            IPMI_NETFN_STORAGE,
                            BMC_LUN,
                            IPMI_STORAGE_GET_SDR,
                            (UINT8 *) &GetSdrCommandData,
                            sizeof (GetSdrCommandData),
                            (UINT8 *) &GetSdrResponseData,
                            (UINT8 *) &GetSdrResponseDataSize );
                SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_STORAGE_GET_SDR Status: %r RecordType: %x RecordId: %x\n", Status, ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->RecordHeader.RecordType, ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->RecordHeader.RecordId ));

                if (EFI_ERROR (Status)) {
                    //
                    // FRU data is not available from the BMC. As such we need to publish Dummy records.
                    //
                    SERVER_IPMI_DEBUG ((EFI_D_ERROR, "  FRU data is not available from the BMC so publish Dummy records...\n" ));
                    DummyFruRecords = TRUE;
                    goto GenerateFruRecords;
                }
                //
                // We only care about TYPE 11 Records. Discard other Record types.
                //  
                if (((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->RecordHeader.RecordType == 0x11) {
                    GetSdrCommandData.RecordOffset = 0; // Record Offset
                    GetSdrCommandData.BytesToRead = sizeof(((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->RecordHeader.RecordLength) + sizeof(IPMI_SDR_RECORD_STRUCT_HEADER); // Bytes to read
                    GetSdrResponseDataSize = sizeof (GetSdrResponseData);
                    //
                    // Clear the GetSdrResponseData
                    //
                    ZeroMem (&GetSdrResponseData[0], GetSdrResponseDataSize);
                    //
                    // Get SDR Info command to Get FRU DATA
                    //
                    Status = gIpmiTransport->SendIpmiCommand (
                         gIpmiTransport,
                         IPMI_NETFN_STORAGE,
                         BMC_LUN,
                         IPMI_STORAGE_GET_SDR,
                         (UINT8 *) &GetSdrCommandData,
                         sizeof (GetSdrCommandData),
                         (UINT8 *) &GetSdrResponseData,
                         (UINT8 *) &GetSdrResponseDataSize );
                    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_STORAGE_GET_SDR Status: %r RecordType: %x RecordId: %x\n", Status, ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->RecordHeader.RecordType, ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->RecordHeader.RecordId ));

                    if (EFI_ERROR (Status)) {
                         //
                         // FRU data of record type 11 with particular record ID not found.
                         //
                         SERVER_IPMI_DEBUG ((EFI_D_ERROR, "  Error with reading the FRU data of record type 11 with record ID %d, %r .... \n", GetSdrCommandData.RecordId, Status));
                         continue;
                    }
                    Index = GetEmptyFruSlot ();
                    if (Index >= MAX_FRU_SLOT) {
                        DEBUG ((EFI_D_INFO, "Increase the number of FRU Slots!!!\n"));
                        break;
                    }
                    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Type 0x11 Record Found: FruDeviceId:  %x\n",  gIpmiFruGlobal->FruDeviceInfo[Index].FruDevice.FruDeviceId ));
                    gIpmiFruGlobal->FruDeviceInfo[Index].Valid = TRUE;
                    gIpmiFruGlobal->FruDeviceInfo[Index].ControllerInstance = 0;
                    gIpmiFruGlobal->NumSlots++;
                    //
                    // Copy the FRU Data to FruDeviceInfo
                    //
                    CopyMem (
                        &gIpmiFruGlobal->FruDeviceInfo[Index].FruDevice,
                        (UINT8*)GetSdrResponseData + sizeof (IPMI_SDR_RECORD_STRUCT_HEADER),
                        sizeof (IPMI_FRU_DATA_INFO)
                        );    
                }
                //
                // Update the RecordId for the GET SDR command
                //
                if (GetSdrCommandData.RecordId == ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->NextRecordId) {
                    break;
                } else {
                    GetSdrCommandData.RecordId = ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->NextRecordId; // Record ID
                }

            } //while ( GetSdrCommandData.RecordId != 0xFFFF )
#endif
        } else { //Verify the Manufacturer ID
            //
            // Manufacturer Id is not supported. So generate dummy FRU records.
            //
            SERVER_IPMI_DEBUG ((EFI_D_ERROR, " Manufacturer Id is not supported so publish Dummy records...\n" ));
            Status = EFI_UNSUPPORTED;
            DummyFruRecords = TRUE;
            goto GenerateFruRecords;
        }
    } else { //if (GetDeviceIdResponse.FruInventorySupport)
        //
        // FruInventory is not supported. So generate dummy FRU records.
        //
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, " FruInventory is not supported so publish Dummy records...\n" ));
        DummyFruRecords = TRUE;
        goto GenerateFruRecords;
    }

    //
    // Install the FRU IPMI Redir protocol.
    //
    NewHandle = NULL;
    Status = gBS->InstallProtocolInterface (
                &NewHandle,
                &gEfiRedirFruProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &gIpmiFruGlobal->IpmiRedirFruProtocol );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " InstallProtocolInterface gEfiRedirFruProtocolGuid Status: %r \n", Status));

GenerateFruRecords:
    //
    // Generate FRU SMBIOS Records.
    //
    GenerateFruSmbiosData (&gIpmiFruGlobal->IpmiRedirFruProtocol, DummyFruRecords);
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
    SmcGenerateFruSmbiosData(&gIpmiFruGlobal->IpmiRedirFruProtocol, DummyFruRecords); // For Fru0 to SMBIOS Type 1 and 3
#endif

    if (EFI_ERROR (Status)) {
        //
        // Error in installing the protocol. So free the allocated memory.
        //
        FreePool (gIpmiFruGlobal);
    } else {
        Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    IpmiRedirFruVirtualAddressChangeEvent,
                    NULL,
                    (CONST EFI_GUID *)&gEfiEventVirtualAddressChangeGuid,
                    &EfiVirtualAddressChangeEvent );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Create event for the Virtual Address Change Event  status %r\n", Status));
    }
    //
    // Update SMBIOS tables with FRU data
    //
    UpdateFruSmbiosTables();
    return EFI_SUCCESS;
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
