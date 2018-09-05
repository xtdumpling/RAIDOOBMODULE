//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.03
//    Bug Fix:  Fixed memory frequency doesn't show on POST screen when Quiet Boot disabled.
//    Reason:   
//    Auditor:  Jimmy Chiu
//    Date:     Aug/15/2016
//
//  Rev. 1.02
//    Bug Fix:  Fix CPU name show incorrectly.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jun/22/2016
//
//  Rev. 1.01
//    Bug Fix:  1. Sync up BIOS revision of setup and Post message with 
//                 SMBIOS Type 0.
//              2. fix BIOS post sign on message issue (Type 2).
//    Reason:   
//    Auditor:  Sunny Yang
//    Date:     Jun/20/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/12/2016
//
//***************************************************************************
//***************************************************************************
#include "Pei.h"
#include "token.h"
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <SmcCspLib.h>

extern EFI_BOOT_SERVICES	*gBS;

EFI_SMBIOS_PROTOCOL	*SmbiosProtocol = NULL;
EFI_SMBIOS_HANDLE	SmbiosHandle;
EFI_SMBIOS_TYPE		Type;
EFI_SMBIOS_TABLE_HEADER	*Record;

EFI_STATUS
SmcLibLocateSmbiosProtocol(VOID)
{
    EFI_STATUS	Status = EFI_SUCCESS;

    if(SmbiosProtocol == NULL){
	Status = gBS->LocateProtocol(
			&gEfiSmbiosProtocolGuid,
			NULL,
			&SmbiosProtocol);
    }

    return Status;
}

EFI_STATUS
SmcLibGetBiosInforInSmbios(	// Smbios Type 0
    IN OUT	CHAR8	*Vendor,
    IN OUT	CHAR8	*BiosVersion,
    IN OUT	CHAR8	*BiosReleaseDate
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	StrLength, SmbiosLength, i, j;
    SMBIOS_TABLE_TYPE0	*RecordPtr;

    DEBUG((-1, "SmcLibGetBiosInforInSmbios Start.\n"));

    if(SmcLibLocateSmbiosProtocol() != EFI_SUCCESS)	return Status;

    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Type = EFI_SMBIOS_TYPE_BIOS_INFORMATION;

    while(!Status){
    	Status = SmbiosProtocol->GetNext(SmbiosProtocol, &SmbiosHandle, &Type, &Record, NULL);
    	RecordPtr = (SMBIOS_TABLE_TYPE0*)Record;
    	SmbiosLength = RecordPtr->Hdr.Length;

    	if(!Status){
    	    for(i = 1, j = 0; i <= RecordPtr->BiosReleaseDate; i++){
    		StrLength = (UINT8)AsciiStrSize((CHAR8*)((UINT8*)RecordPtr + SmbiosLength + j));
    		if((i == RecordPtr->Vendor) && Vendor)
    		    gBS->CopyMem(Vendor, (UINT8*)RecordPtr + SmbiosLength + j, StrLength);
    		else if((i == RecordPtr->BiosVersion) && BiosVersion)
    		    gBS->CopyMem(BiosVersion, (UINT8*)RecordPtr + SmbiosLength + j, StrLength);
    		else if((i == RecordPtr->BiosReleaseDate) && BiosReleaseDate)
    		    gBS->CopyMem(BiosReleaseDate, (UINT8*)RecordPtr + SmbiosLength + j, StrLength);

    		j += StrLength;	//j is index in TempBuffer(type 2)
    	    }
    	}
    }
	        
    DEBUG((-1, "SmcLibGetBiosInforInSmbios End.\n"));
    return Status;
}

EFI_STATUS
SmcLibGetBoardInforInSmbios(	// Smbios Type 2
    IN OUT	CHAR8	*Manufacturer,
    IN OUT	CHAR8	*ProductName
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	StrLength, SmbiosLength, i, j;
    SMBIOS_TABLE_TYPE2	*RecordPtr;

    DEBUG((-1, "SmcLibGetBoardInforInSmbios Start.\n"));

    if(SmcLibLocateSmbiosProtocol() != EFI_SUCCESS)	return Status;

    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Type = EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION;

    while(!Status){
    	Status = SmbiosProtocol->GetNext(SmbiosProtocol, &SmbiosHandle, &Type, &Record, NULL);
    	RecordPtr = (SMBIOS_TABLE_TYPE2*)Record;
    	SmbiosLength = RecordPtr->Hdr.Length;

    	if(!Status){
    	    for(i = 1, j = 0; i <= RecordPtr->LocationInChassis; i++){
    		StrLength = (UINT8)AsciiStrSize((CHAR8*)((UINT8*)RecordPtr + SmbiosLength + j));
    		if((i == RecordPtr->Manufacturer) && Manufacturer)
    		    gBS->CopyMem(Manufacturer, (UINT8*)RecordPtr + SmbiosLength + j, StrLength);
    		else if((i == RecordPtr->ProductName) && ProductName)
    		    gBS->CopyMem(ProductName, (UINT8*)RecordPtr + SmbiosLength + j, StrLength);

    		j += StrLength;	//j is index in TempBuffer(type 2)
    	    }
    	}
    }
        
    DEBUG((-1, "SmcLibGetBoardInforInSmbios End.\n"));
    return Status;
}

EFI_STATUS
SmcLibGetCPUInforBySmbios(	// Smbios Type 4
    IN OUT	CHAR8	*ProcessorVersion,
    IN OUT	UINT16	*MaxSpeed,
    IN OUT	UINT16	*CurrentSpeed,
    IN OUT	UINT8	*CoreCount,
    IN OUT	UINT8	*ThreadCount
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	StrLength, SmbiosLength, i, j;
    SMBIOS_TABLE_TYPE4	*RecordPtr;

    DEBUG((-1, "SmcLibGetCPUInforBySmbios Start.\n"));

    if(SmcLibLocateSmbiosProtocol() != EFI_SUCCESS)	return Status;

    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Type = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;

    while(!Status){
	Status = SmbiosProtocol->GetNext(SmbiosProtocol, &SmbiosHandle, &Type, &Record, NULL);
	RecordPtr = (SMBIOS_TABLE_TYPE4*)Record;
	SmbiosLength = RecordPtr->Hdr.Length;

	if((!Status) && (RecordPtr->Status & 0x40)){
	    for(i = 1, j = 0; i <= RecordPtr->AssetTag; i++){
		StrLength = (UINT8)AsciiStrSize((CHAR8*)((UINT8*)RecordPtr + SmbiosLength + j));
		if((i == RecordPtr->ProcessorVersion) && ProcessorVersion && StrLength < 100)
		    gBS->CopyMem(ProcessorVersion, (UINT8*)RecordPtr + SmbiosLength + j, StrLength);

		j += StrLength;	//j is index in TempBuffer(type 4)
	    }
	    if(MaxSpeed)	*MaxSpeed = RecordPtr->MaxSpeed;
	    if(CurrentSpeed)	*CurrentSpeed = RecordPtr->CurrentSpeed;
	    if(CoreCount)	*CoreCount = RecordPtr->CoreCount;
	    if(ThreadCount)	*ThreadCount = RecordPtr->ThreadCount;
	}
    }
    
    DEBUG((-1, "SmcLibGetCPUInforBySmbios End.\n"));
    return Status;
}

EFI_STATUS
SmcLibGetCacheInforBySmbios(	// Smbios Type 7
    IN OUT	UINT16	*L1CacheSize,
    IN OUT	UINT16	*L2CacheSize,
    IN OUT	UINT16	*L3CacheSize
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    SMBIOS_TABLE_TYPE7	*RecordPtr;

    DEBUG((-1, "SmcLibGetCacheInforBySmbios Start.\n"));

    if(SmcLibLocateSmbiosProtocol() != EFI_SUCCESS)	return Status;
    
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Type = EFI_SMBIOS_TYPE_CACHE_INFORMATION;

    while(!Status){
    	Status = SmbiosProtocol->GetNext(SmbiosProtocol, &SmbiosHandle,	&Type, &Record, NULL);
    	RecordPtr = (SMBIOS_TABLE_TYPE7*)Record;
	if(!Status){
	    if(((RecordPtr->CacheConfiguration & 0x07) == 0x00) && L1CacheSize){
		if(RecordPtr->InstalledSize & BIT15)
		    *L1CacheSize = (RecordPtr->InstalledSize & 0x7FFF) * 64;
		else
		    *L1CacheSize = RecordPtr->InstalledSize;
	    }
	    else if(((RecordPtr->CacheConfiguration & 0x07) == 0x01) && L2CacheSize){
		if(RecordPtr->InstalledSize & BIT15)
		    *L2CacheSize = (RecordPtr->InstalledSize & 0x7FFF) * 64;
		else
		    *L2CacheSize = RecordPtr->InstalledSize;
	    }
	    else if(((RecordPtr->CacheConfiguration & 0x07) == 0x02) && L3CacheSize){
		if(RecordPtr->InstalledSize & BIT15)
		    *L3CacheSize = (RecordPtr->InstalledSize & 0x7FFF) * 64;
		else
		    *L3CacheSize = RecordPtr->InstalledSize;
	    }
	}
    }

    DEBUG((-1, "SmcLibGetCacheInforBySmbios End.\n"));
    return Status;
}

EFI_STATUS
SmcLibGetPhysicalMemoryBySmbios(	// Smbios Type 16
    IN OUT	BOOLEAN	*EccModeEnabled
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    SMBIOS_TABLE_TYPE16	*RecordPtr;

    DEBUG((-1, "SmcLibGetPhysicalMemoryBySmbios Start.\n"));

    if(SmcLibLocateSmbiosProtocol() != EFI_SUCCESS)	return Status;

    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Type = EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY;
    *EccModeEnabled = 0;

    while(!Status){
	Status = SmbiosProtocol->GetNext(SmbiosProtocol, &SmbiosHandle,	&Type, &Record, NULL);
	RecordPtr = (SMBIOS_TABLE_TYPE16*)Record;
	if(!Status){
	    if((RecordPtr->MemoryErrorCorrection == MemoryErrorCorrectionSingleBitEcc) ||
		    (RecordPtr->MemoryErrorCorrection == MemoryErrorCorrectionMultiBitEcc)){
		if(EccModeEnabled)	*EccModeEnabled = 1;
	    }
	}
    }

    DEBUG((-1, "SmcLibGetPhysicalMemoryBySmbios End.\n"));
    return Status;
}

EFI_STATUS
SmcLibGetMemoryInforBySmbios(	// Smbios Type 17
    IN OUT	UINT8	*MemoryType,
    IN OUT	UINT16	*TypeDetail,
    IN OUT	UINT16	*MemFreq,
    IN OUT	UINTN	*Size
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    SMBIOS_TABLE_TYPE17	*RecordPtr;

    DEBUG((-1, "SmcLibGetMemoryInforBySmbios Start.\n"));

    if(SmcLibLocateSmbiosProtocol() != EFI_SUCCESS)	return Status;

    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Type = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
    *Size = 0;
    while(!Status){
    	Status = SmbiosProtocol->GetNext(SmbiosProtocol, &SmbiosHandle,	&Type, &Record, NULL);
    	RecordPtr = (SMBIOS_TABLE_TYPE17*)Record;
    	if((!Status) && (RecordPtr->Size)){
    	    if(MemoryType)	*MemoryType = RecordPtr->MemoryType;
    	    if(TypeDetail)	*TypeDetail = 0x0C;
    	    if(MemFreq)		*MemFreq = SmcCspGetMemFreq();
    	    if(Size){
    		if(RecordPtr->Size & BIT15)	// The value is specified in kilobyte units.
    		    *Size += RecordPtr->Size;
    		else // The value is specified in megabyte units.
    		    *Size += RecordPtr->Size * 1024;
    	    }
    	    
    	}
    }

    DEBUG((-1, "SmcLibGetMemoryInforBySmbios End.\n"));
    return Status;
}
//****************************************************************************
