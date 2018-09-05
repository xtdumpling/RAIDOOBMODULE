//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
	Definitions of AMI specific Hobs
*/
#ifndef __AMIHOBS_H__
#define __AMIHOBS_H__

#include <Pei.h>
#include <Hob.h>
//#include <CpuHobs.h>

#pragma pack(push,1)


// {39E8CDA1-6A35-4cdd-A902-D1A795F70380}
#define AMI_RESET_TYPE_HOB_GUID \
    {0x39e8cda1, 0x6a35, 0x4cdd, 0xa9, 0x2, 0xd1, 0xa7, 0x95, 0xf7, 0x3, 0x80}

//Not all reset types are available on all systems.
//Not all reset types may be reported.

#define AMI_HOB_RESET_TYPE_POWER_ON     0
#define AMI_HOB_RESET_TYPE_CPU          1
#define AMI_HOB_RESET_TYPE_WARM         2
#define AMI_HOB_RESET_TYPE_HARD         3
#define AMI_HOB_RESET_TYPE_POWER_GOOD   4

typedef struct {
    EFI_HOB_GUID_TYPE   EfiHobGuidType;
    UINT8               ResetType;
} AMI_RESET_TYPE_HOB;

////////////////////////////////////////////////////////////////////////


//HOB to report Apic Ids
//This is a EFI_HOB_TYPE_GUID_EXTENSION.
#define AMI_CPUINFO_HOB_GUID \
	{0x2e547cb5,0x40f8,0x4f3e,0x8a,0xf4,0x19,0x52,0xaa,0xd5,0x5a,0xc}

typedef struct {
    UINT64 SmrrSupport:1;
    UINT64 Reserved:63;
} ADDITIONAL_FEATURE_FLAGS;

typedef struct {
    UINT32 FeatureEcx;
    UINT32 FeatureEdx;
    UINT32 ExtFeatureEax;
    UINT32 ExtFeatureEbx;
    UINT32 ExtFeatureEcx;
    UINT32 ExtFeatureEdx;
    ADDITIONAL_FEATURE_FLAGS Flags;
} CPU_FEATURES;

typedef struct {
	BOOLEAN     Valid;
	BOOLEAN     Disabled;
	UINT32      CpuSignature;
	UINT64      CpuFeatureInfo;
	UINT32      CpuRevision;		// Specific to the CPU brand
	UINT32      BIST;
	UINT8       ApicId;
	UINT8       ApicEId;
	UINT8       ApicVer;
	BOOLEAN     EistEnable;
	UINT8       BusRatioMax;
	UINT8       BusRatioMin;
	UINT8       VidMax;
	UINT8       VidMin;
	UINT8       BusRatioMinStep;
} CPUINFO;

// SmramCap field values
#define	SMRAM_ASEG_SUPPORTED			0x01			// Bit 0
#define	SMRAM_TSEG_SUPPORTED			0x02			// BIT 1
#define	SMRAM_HSEG_SUPPORTED			0x04			// BIT 2

typedef struct {
	EFI_HOB_GUID_TYPE		EfiHobGuidType;
	UINT8					CpuCount;
	UINT8					BspNo;
	UINT8					NodeCount;				// Physical CPU count
	UINT32					SmramCap;
	EFI_PHYSICAL_ADDRESS	TsegAddress;
	UINT32					TsegSize;
	UINT16					CacheLineSize;			// (Bytes)
    CPU_FEATURES            CpuFeatures;
	CPUINFO					Cpuinfo[1];				//This must be last in structure.
} CPUINFO_HOB;
////////////////////////////////////////////////////////////////////////

//AB1BBFF5-2655-4e78-B442-0D264844B428
#define SMM_HOB_GUID \
    {0xab1bbff5,0x2655,0x4e78,0xb4,0x42,0xd,0x26,0x48,0x44,0xb4,0x28}

typedef struct {
    EFI_HOB_GUID_TYPE   EfiHobGuidType;
    UINT32  Tseg;
    UINT32  TsegLength;
    UINT32  ManagedMemStart;
    UINT32  ManagedMemSize;
    UINT32  IedStart;
    UINT32  IedSize;
    UINT16  NumCpus;
    UINT16  Bsp;
    UINT32  SmmBase[1];
} SMM_HOB;

////////////////////////////////////////////////////////////////////////

// GUIDED NB HOB
// {E7D449D9-1FFC-4eed-8D5D-7A53A4F8FB9A}
#define AMI_NB_INFO_HOB_GUID \
	{0xe7d449d9,0x1ffc,0x4eed,0x8d,0x5d,0x7a,0x53,0xa4,0xf8,0xfb,0x9a}

typedef struct {
	EFI_HOB_GUID_TYPE		EfiHobGuidType;
	UINT32					ReservedIgdFrameBufferSize;
} NB_INFO_HOB;

////////////////////////////////////////////////////////////////////////

//GUIDed HOB to pass the code pointer from PEI to DXE
// {F4491BA4-7672-486f-B4D7-99899D22DA57}
#define AMI_PEIM_HOB_GUID \
	{ 0xf4491ba4, 0x7672, 0x486f, { 0xb4, 0xd7, 0x99, 0x89, 0x9d, 0x22, 0xda, 0x57 } }

typedef struct {
	EFI_HOB_GUID_TYPE Header;
	EFI_IMAGE_ENTRY_POINT DxeEntryPoint;
} PEIM_HOB;

//GUIDed HOB to notify core that PEIM has to be reallocated to RAM once it's available
// {EC9C36FD-1642-4b84-91FA-919C2D066FB4}
#define AMI_PEIM_LOAD_HOB_GUID \
	{ 0xec9c36fd, 0x1642, 0x4b84, { 0x91, 0xfa, 0x91, 0x9c, 0x2d, 0x6, 0x6f, 0xb4 } }

typedef struct {
	EFI_HOB_GUID_TYPE Header;
	VOID *pFfsHeader;
	EFI_PEIM_ENTRY_POINT EntryPoint, InMemEntryPoint;
} PEIM_LOAD_HOB;

//GUIDed Hob for Capsule, that should be populated via System table
// {BF66FDF7-F64C-4b11-8AB7-F843AA2A8BEA}
#define AMI_CAPSULE_HOB_GUID \
    { 0xbf66fdf7, 0xf64c, 0x4b11, { 0x8a, 0xb7, 0xf8, 0x43, 0xaa, 0x2a, 0x8b, 0xea } }

typedef struct {
	EFI_HOB_GUID_TYPE Header;
    EFI_GUID CapsuleGuid;
	EFI_PHYSICAL_ADDRESS CapsuleData;
    UINT64 CapsuleLength;
} AMI_CAPSULE_HOB;

//GUIDed Hob used for PEI performance measurements
#define EFI_PEI_PERFORMANCE_HOB_GUID  \
{0x10f432de, 0xdeec, 0x4631, 0x80, 0xcd, 0x47, 0xf6, 0x5d, 0x8f, 0x80, 0xbb}

#define PEI_PERF_MAX_DESC_STRING 8

typedef struct {
  UINT64          StartTimeCount;
  UINT64          StopTimeCount;
  EFI_GUID        Name;
  UINT16          DescriptionString[PEI_PERF_MAX_DESC_STRING];
} PEI_PERFORMANCE_MEASURE_LOG_ENTRY;

typedef struct {
  UINT32                             NumberOfEntries;
  UINT32                             Reserved;
  PEI_PERFORMANCE_MEASURE_LOG_ENTRY  Log[1];
} EFI_HOB_GUID_DATA_PERFORMANCE_LOG;

//GUIDed Hob to pass SM Bus ArpMap
// {017CB4B7-B80C-4040-B6C8-EA982BBB25B7}
#define AMI_SMBUS_HOB_GUID \
    { 0x17cb4b7, 0xb80c, 0x4040, { 0xb6, 0xc8, 0xea, 0x98, 0x2b, 0xbb, 0x25, 0xb7 } }

typedef struct {
	EFI_HOB_GUID_TYPE Header;
    UINT8 BoardReservedAddressCount;
    UINT32 BoardReservedAddressList;
    UINT8 ArpDeviceCount;
    UINT8 ArpDeviceList[1];
} AMI_SMBUS_HOB;

//ROM Layout GUIDed HOB
// {D7642443-87B7-4832-9607-0E1EA81C1D86}
#define AMI_ROM_LAYOUT_HOB_GUID { 0xd7642443, 0x87b7, 0x4832, { 0x96, 0x7, 0xe, 0x1e, 0xa8, 0x1c, 0x1d, 0x86 } }
typedef struct {
	EFI_HOB_GUID_TYPE Header;
    //ROM_AREA RomLayout[];    
} ROM_LAYOUT_HOB;

//GUIDed HOB used to describe location of the Recovery image
// {DAC3CB98-2295-412e-826D-FDEEA320CF31}
#define AMI_RECOVERY_IMAGE_HOB_GUID \
	{ 0xdac3cb98, 0x2295, 0x412e, { 0x82, 0x6d, 0xfd, 0xee, 0xa3, 0x20, 0xcf, 0x31 } }
    
extern EFI_GUID gAmiRecoveryImageHobGuid;

//Extended security errors definitions
typedef enum {
    InvalidHeader = 1,
    InvalidSignature,
    IvalidPlatformKey,
    InvalidFwVersion,
    MaxError
} EXTENDED_SECURITY_ERRORS;

typedef struct {
	EFI_HOB_GUID_TYPE    Header;
	EFI_PHYSICAL_ADDRESS Address;
    UINT8                Status;            // Generic EFI_STATUS
    UINT32               FailedStage;       // Extended Security Error
    UINT32               ImageSize;
} RECOVERY_IMAGE_HOB;

//GUIDed HOB used to pass SMBIOS memory related data to DXE SMBIOS driver
// {7D6B8734-B754-443f-B588-7743843AD3F1}
#define AMI_SMBIOS_MEMORY_INFO_HOB_GUID \
    { 0x7d6b8734, 0xb754, 0x443f, { 0xb5, 0x88, 0x77, 0x43, 0x84, 0x3a, 0xd3, 0xf1 } }

typedef struct {
	EFI_HOB_GUID_TYPE Header;
//  MEM_SPD_DATA      SpdDataArray[NO_OF_PHYSICAL_MEMORY_ARRAY][MAX_NUMBER_OF_MEM_MODULE];
} AMI_SMBIOS_MEMORY_INFO_HOB;



#pragma pack(pop)
/****** DO NOT WRITE BELOW THIS LINE *******/

#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
