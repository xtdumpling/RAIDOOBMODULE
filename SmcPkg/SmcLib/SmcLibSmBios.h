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
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/12/2016
//
//***************************************************************************
//***************************************************************************

#ifndef __SMC_LIB_SMBIOS__H__
#define __SMC_LIB_SMBIOS__H__
#ifdef __cplusplus
extern "C" {
#endif

EFI_STATUS
SmcLibGetBiosInforInSmbios(		// Smbios Type 0
    IN OUT	CHAR8	*Vendor,
    IN OUT	CHAR8	*BiosVersion,
    IN OUT	CHAR8	*BiosReleaseDate
);

EFI_STATUS
SmcLibGetBoardInforInSmbios(	// Smbios Type 2
    IN OUT	CHAR8	*Manufacturer,
    IN OUT	CHAR8	*ProductName
);

EFI_STATUS
SmcLibGetCPUInforBySmbios(		// Smbios Type 4
    IN OUT	CHAR8	*ProcessorVersion,
    IN OUT	UINT16	*MaxSpeed,
    IN OUT	UINT16	*CurrentSpeed,
    IN OUT	UINT8	*CoreCount,
    IN OUT	UINT8	*ThreadCount
);

EFI_STATUS
SmcLibGetCacheInforBySmbios(		// Smbios Type 7
    IN OUT	UINT16	*L1CacheSize,
    IN OUT	UINT16	*L2CacheSize,
    IN OUT	UINT16	*L3CacheSize
);

EFI_STATUS
SmcLibGetPhysicalMemoryBySmbios(	// Smbios Type 16
    IN OUT	BOOLEAN	*EccModeEnabled
);

EFI_STATUS
SmcLibGetMemoryInforBySmbios(		// Smbios Type 17
    IN OUT	UINT8	*MemoryType,
    IN OUT	UINT16	*TypeDetail,
    IN OUT	UINT16	*MemFreq,
    IN OUT	UINTN	*Size
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************
