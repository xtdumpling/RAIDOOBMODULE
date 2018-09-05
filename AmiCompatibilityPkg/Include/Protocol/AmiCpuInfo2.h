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
  AMI CPU INFO 2 protocol header.
*/

#ifndef _AMI_CPU_INFO_2_H_
#define _AMI_CPU_INFO_2_H_

#ifdef __cplusplus
extern "C" {
#endif

// {AC9CF0A8-E551-4be2-AD0A-E1B564EEA273}
#define AMI_CPU_INFO_2_PROTOCOL_GUID \
    {0xac9cf0a8, 0xe551, 0x4be2, 0xad, 0xa, 0xe1, 0xb5, 0x64, 0xee, 0xa2, 0x73}

#define AMI_CPU_INFO_2_PROTOCOL_VERSION 3

///////////////
//ACPI Data
//////////////

//Table Type 0//
//The Local APIC in MADT and MP should be filled in order of table entries.
//The length may be increased, so to be compatible with earlier version, get the
// next entry by using length instead of sizeof(ACPI_PROCESSOR_INFO) or array.

#define ACPI_PROCESSOR_INFO_TYPE 0
#define ACPI_PROC_INFO_xAPIC     0
#define ACPI_PROC_INFO_x2APIC    1

//When adding to structure, use only types with the same size for x32 and x64,
//so length can be used to determine supported features.
//This means no VOID* and no UINTN.

typedef struct {
    UINT32 Type;            //0
    UINT32 Length;          //Note: Find next structure by length.
    UINT32 Enable;
    UINT32 Bsp;             //0 or 1
    UINT32 Package;
    UINT32 Core;
    UINT32 Thread;          //Thread of processor 0 or 1.
    UINT32 ApicId;          //APIC ID of processor.
    UINT32 ApicVer;
    UINT32 CpuSignature;    //Used by MPS table.
    UINT32 FeatureFlags;    //Used by MPS table.
    UINT32 ProcId;          //ASL processor object ID.
    EFI_PHYSICAL_ADDRESS    ProcObjPath;    //ASL Processor Object Path.
 //Only if Length >= 60.
    UINT32                  LocalApicType;  //0 - xApic, 1 - x2Apic
//Only if Length >= 72.     //Check for 72 because of alignment.
    UINT32                  ProduceNmi;
    UINT32                  NmiFlags;
    UINT32                  LintnPin;
} ACPI_PROCESSOR_INFO;


typedef struct _AMI_CPU_INFO_2_PROTOCOL AMI_CPU_INFO_2_PROTOCOL;

#define AMI_CPU_INFO_2_FLAGS_NUM_DIS_CORES_VALID        1
#define AMI_CPU_INFO_2_FLAGS_NUM_DIS_THRDS_VALID        2

typedef EFI_STATUS (EFIAPI *GET_NUM_PACKAGES) (
	IN AMI_CPU_INFO_2_PROTOCOL  *This,
    OUT UINT32                  *NumPopulatedPackages,
    OUT UINT32                  *NumBoardSockets OPTIONAL
);


typedef EFI_STATUS (EFIAPI *GET_NUM_CORES_THREADS) (
	IN AMI_CPU_INFO_2_PROTOCOL  *This,
    IN UINT32                   Package,
    OUT UINT32                  *NumEnabledCores,
    OUT UINT32                  *NumEnabledThreads,
    OUT UINT32                  *NumDisabledCores OPTIONAL,
    OUT UINT32                  *NumDisabledThreads OPTIONAL
);

typedef EFI_STATUS (EFIAPI *GET_NUM_THREADS) (
	IN AMI_CPU_INFO_2_PROTOCOL  *This,
    IN UINT32                   Package,
    IN UINT32                   Core,
    OUT UINT32                  *NumEnabledThreads,
    OUT UINT32                  *NumDisabledThreads OPTIONAL
);

typedef EFI_STATUS (EFIAPI *GET_SBSP) (
	IN AMI_CPU_INFO_2_PROTOCOL  *This,
    OUT UINT32                  *Package,
    OUT UINT32                  *Core,
    OUT UINT32                  *Thread
);

typedef EFI_STATUS (EFIAPI *GET_APIC_INFO) (
	IN AMI_CPU_INFO_2_PROTOCOL  *This,
    IN UINT32                   Package,
    IN UINT32                   Core,
    IN UINT32                   Thread,
    OUT UINT32                  *ApicId,
    OUT UINT32                  *ApicVer OPTIONAL
);

typedef EFI_STATUS (EFIAPI *GET_ACPI_INFO) (
	IN AMI_CPU_INFO_2_PROTOCOL  *This,
    OUT VOID                    **AcpiData,
    OUT UINT32                  *NumEntries
);

#define AMI_CPU_INFO_2_CACHE_TYPE_DATA      0
#define AMI_CPU_INFO_2_CACHE_TYPE_CODE      1
#define AMI_CPU_INFO_2_CACHE_TYPE_UNIFIED   2

#define AMI_CPU_INFO_2_CACHE_SHARED_CORE    1
#define AMI_CPU_INFO_2_CACHE_SHARED_PACKAGE 2

typedef struct {
    UINT32  LengthDesc;        //Later updates may increase this.
    UINT32  Level;
    UINT32  Size;
    UINT32  Type;
    UINT32  Associativity;
    UINT32  Shared;
}  AMI_CPU_INFO_2_CACHE_DESCR;

typedef EFI_STATUS (EFIAPI *GET_PACKAGE_CACHE_DESCR) (
    IN AMI_CPU_INFO_2_PROTOCOL      *This,
    IN UINT32                       Package,
    OUT AMI_CPU_INFO_2_CACHE_DESCR  **Description,
    OUT UINT32                      *NumEntries
);

typedef EFI_STATUS (EFIAPI *GET_CORE_CACHE_DESCR) (
    IN AMI_CPU_INFO_2_PROTOCOL      *This,
    IN UINT32                       Package,
    IN UINT32                       Core,
    OUT AMI_CPU_INFO_2_CACHE_DESCR  **Description,
    OUT UINT32                      *NumEntries
);

struct _AMI_CPU_INFO_2_PROTOCOL {
    //Protocol Ver 0
    UINT32                  ProtocolVer;
    UINT64                  Flags;
    GET_NUM_PACKAGES        GetNumPackages;
    GET_NUM_CORES_THREADS   GetNumCoresThreads;
    GET_NUM_THREADS         GetNumThreads;
    GET_SBSP                GetSbsp;
    GET_APIC_INFO           GetApicInfo;
    //Protocol Ver 1
    GET_ACPI_INFO           GetAcpiInfo;
    //Protocol Ver 2
    GET_PACKAGE_CACHE_DESCR GetPackageCacheDesc;
    GET_CORE_CACHE_DESCR    GetCoreCacheDesc;
};

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
