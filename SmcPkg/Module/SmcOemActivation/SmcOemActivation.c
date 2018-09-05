//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Update OA key to SmcRomHole when key updated.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jul/01/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     May/09/2016
//
//****************************************************************************
//****************************************************************************

//****************************************************************************
//<AMI_FHDR_START>
//
// Name:  SmcOemActivation.c
//
// Description:
//  Publish SLIC OEM Activation ACPI Table.
// 
//<AMI_FHDR_END>
//****************************************************************************
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/AcpiTable.h>
#include <AmiLib.h>
#include "OemActivation/OA2/OA2.h"
#include "SmcOemActivation.h"
#include "SmcRomHole.h"

EFI_ACPI_SLP	SlpTable = {
                {SLIC_SIG, sizeof(EFI_ACPI_SLP), 0x01, 0x00, {0}, {0}, ACPI_OEM_REV,
                CREATOR_ID_AMI, CREATOR_REV_MS}, {0xff}, {0xff}};

EFI_GUID AmiPubKeyGuid = OEM_SLP_PUBLIC_KEY_GUID;
EFI_GUID AmiMarkerGuid = OEM_SLP_MARKER_GUID;
EFI_GUID SmcPubKeyGuid = SMC_ROMHOLE_PUBLIC_KEY_GUID;
EFI_GUID SmcMarkerGuid = SMC_ROMHOLE_MARKER_GUID;

EFI_STATUS 
LocateAndLoadRawData(
    IN	EFI_GUID        *FfsGuid,
    IN OUT      VOID    *TempBuff,
    IN	UINTN           Size
)
{
    EFI_STATUS	Status;
    EFI_HANDLE	*HandleBuffer = NULL;
    UINTN	NumberOfHandles, Index;
    UINT32	Authentication;
    EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol = NULL;
    EFI_FV_FILE_ATTRIBUTES      Attributes;
    EFI_FV_FILETYPE             FileType;

    DEBUG((-1, "LocateAndLoadRawData entry.\n"));

    Status = gBS->LocateHandleBuffer(
		    ByProtocol,
		    &gEfiFirmwareVolume2ProtocolGuid,
		    NULL,
		    &NumberOfHandles,
		    &HandleBuffer);
    DEBUG((-1, "LocateAndLoadRawData LocateHandleBuffer Status = %r.\n", Status));
    if(Status)	return EFI_NOT_FOUND;
    
    // Find and read raw data
    for(Index = 0; Index < NumberOfHandles; Index++){
        Status = gBS->HandleProtocol(
        		HandleBuffer[Index],
			&gEfiFirmwareVolume2ProtocolGuid,
			&FwVol);
        DEBUG((-1, "LocateAndLoadRawData HandleProtocol Status = %r.\n", Status));
        if(Status)	continue;

	if(!guidcmp(FfsGuid, &SmcPubKeyGuid) || !guidcmp(FfsGuid, &SmcMarkerGuid)){
	    Status = FwVol->ReadFile(
			    FwVol,
			    FfsGuid,
			    &TempBuff,
			    &Size,
			    &FileType,
			    &Attributes,
			    &Authentication);
	    DEBUG((-1, "LocateAndLoadRawData ReadFile Status = %r.\n", Status));
	    DEBUG((-1, "LocateAndLoadRawData Size = %x.\n", Size));
	    if((Status == EFI_WARN_BUFFER_TOO_SMALL) && (*(UINT8*)TempBuff != 0xff))
		Status = EFI_SUCCESS;
	}
	else {
            Status = FwVol->ReadSection(
        		    FwVol,
        		    FfsGuid,
        		    EFI_SECTION_RAW,
        		    0x0,
        		    &TempBuff,
        		    &Size,
        		    &Authentication);
            DEBUG((-1, "LocateAndLoadRawData ReadSection Status = %r.\n", Status));
            DEBUG((-1, "LocateAndLoadRawData Size = %x.\n", Size));
	}
        if(Status == EFI_SUCCESS) 
	    break;
    }
    
    gBS->FreePool(HandleBuffer);
    DEBUG((-1, "LocateAndLoadRawData end.\n"));
    return	Status;
}

EFI_STATUS
SMCSaveOA2key(void)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    BOOLEAN	get_pub = FALSE, get_mark = FALSE;
    OEM_PUBLIC_KEY_STRUCTURE	OEM_pubkey;
    WINDOWS_MARKER_STRUCTURE	OEM_marker;
    UINT8	*TempBuffer, *Old_key;
    UINT32	TempSize;

    DEBUG((-1, "SMCSaveOAkey entry.\n"));

    TempSize = SmcRomHoleSize(OA2_REGION);
    Status = gBS->AllocatePool(EfiBootServicesData, TempSize, &TempBuffer);
    if(Status)	return Status;
    Status = gBS->AllocatePool(EfiBootServicesData, TempSize, &Old_key);
    if(Status)	return Status;

    gBS->SetMem(TempBuffer, TempSize, 0xff);
    SmcRomHoleReadRegion(OA2_REGION, Old_key);

// search AMISLP pubkey exist, if not, search ROMHOLE pubkey
    Status = LocateAndLoadRawData(&AmiPubKeyGuid, &OEM_pubkey, sizeof(OEM_pubkey));
    if(Status)	Status = LocateAndLoadRawData(&SmcPubKeyGuid, &OEM_pubkey, sizeof(OEM_pubkey));

    if(!Status){	// write AMISLP pubkey to ROMHOLE pubkey and temp space
	DEBUG((-1, "AMISLP pubkey searched.\n"));
	gBS->CopyMem(TempBuffer, &OEM_pubkey, sizeof(OEM_pubkey));
	if(CompareMem(Old_key, TempBuffer, sizeof(OEM_pubkey)))
	    get_pub = TRUE;
    }

// search AMISLP marker exist, if not, search ROMHOLE marker
    Status = LocateAndLoadRawData(&AmiMarkerGuid, &OEM_marker, sizeof(OEM_marker));
    if(Status)	Status = LocateAndLoadRawData(&SmcMarkerGuid, &OEM_marker, sizeof(OEM_marker));

    if(!Status){	// write AMISLP marker to ROMHOLE marker and temp space
	DEBUG((-1, "AMISLP marker copy searched.\n"));
	gBS->CopyMem(TempBuffer + 0x500, &OEM_marker, sizeof(OEM_marker));
	if(CompareMem(Old_key + 0x500, TempBuffer + 0x500, sizeof(OEM_marker)))
	    get_mark = TRUE;
    }

    if(get_pub || get_mark)
	SmcRomHoleWriteRegion(OA2_REGION, TempBuffer);

    DEBUG((-1, "SMCSaveOAkey end.\n"));
    return	Status;
}

VOID
SmcOA2Callback(
    IN EFI_EVENT	Event,
    IN VOID		*Context
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    EFI_ACPI_TABLE_PROTOCOL	*AcpiTableProtocol;
    UINTN	TableKey = 0;
	    
    DEBUG((-1, "SmcOA2Callback Entry.\n"));
// Locate the ACPI table protocol
    Status = gBS->LocateProtocol(
		    &gEfiAcpiTableProtocolGuid,
    		    NULL, 
    		    &AcpiTableProtocol);

    if(Status)	return;

// Publish SLIC to ACPI table
    Status = AcpiTableProtocol->InstallAcpiTable(
    		    AcpiTableProtocol,
    		    &SlpTable,
    		    sizeof(EFI_ACPI_SLP), 
    		    &TableKey);

    DEBUG((-1, "SmcOA2Callback End.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SmcOemActivationEntry
//
// Description : 
//
// Parameters  : None
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
SmcOemActivationEntry(
    IN	EFI_HANDLE		ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status;
    UINT8	*TempBuffer;
    UINT32	TempSize;
    VOID	*SmcOA2Reg;
    EFI_EVENT	Event;
    
    DEBUG((-1, "SmcOemActivationEntry Entry.\n"));

    SMCSaveOA2key();

    TempSize = SmcRomHoleSize(OA2_REGION);
    Status = gBS->AllocatePool(EfiBootServicesData, TempSize, &TempBuffer);
    if(Status)	return Status;
    
    Status = SmcRomHoleReadRegion(OA2_REGION, TempBuffer);
    if(Status){
    	gBS->FreePool(TempBuffer);
    	return Status;
    }
    
    gBS->CopyMem(&SlpTable.PubKey, TempBuffer, sizeof(SlpTable.PubKey));
    gBS->CopyMem(&SlpTable.WinMarker, TempBuffer + 0x500, sizeof(SlpTable.WinMarker));
    gBS->FreePool(TempBuffer);

    if((SlpTable.PubKey.StructType == 0xffffffff) || (SlpTable.WinMarker.StructType == 0xffffffff))
	return EFI_NOT_FOUND;
    
    gBS->CopyMem(SlpTable.Header.OemId, SlpTable.WinMarker.sOEMID, sizeof(SlpTable.WinMarker.sOEMID));
    gBS->CopyMem(SlpTable.Header.OemTblId, SlpTable.WinMarker.sOEMTABLEID, sizeof(SlpTable.WinMarker.sOEMTABLEID));
        
    Status = gBS->CreateEvent(
    			EFI_EVENT_NOTIFY_SIGNAL,
    			TPL_CALLBACK,
    			SmcOA2Callback,
    			NULL,
    			&Event);

    if(!Status){
    	Status = gBS->RegisterProtocolNotify(
    			&gEfiAcpiTableProtocolGuid,
    			Event,
    			&SmcOA2Reg);
    }

    DEBUG((-1, "SmcOemActivationEntry End.\n"));
    return Status;
}
