//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file NvmeDef.h
    Header file for Smm and Non Smm interface

**/

#ifndef _AMI_NVME_DEF_H_
#define _AMI_NVME_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define NVME_DATA_EBDA_OFFSET 0x104

#define NVME_API_MASS_DEVICE_REQUEST    0x27

//----------------------------------------------------------------------------
//      NVME Mass Storage Related Data Structures and Equates
//----------------------------------------------------------------------------
#define NVME_EMU_NONE            0
#define NVME_EMU_FLOPPY_ONLY     1
#define NVME_EMU_HDD_ONLY        2
#define NVME_EMU_FORCED_FDD      3

// Error returned from API handler
#define     NVME_SUCCESS                0x000
#define     NVME_PARAMETER_ERROR        0x010
#define     NVME_NOT_SUPPORTED          0x020
#define     NVME_INVALID_FUNCTION       0x0F0
#define     NVME_ERROR                  0x0FF

#pragma pack(1)

/**
    This is a URP (NVME Request Packet) structure for the BIOS
    API call Controller Info
    
    @param  TransferBufferAddress
    @param  NVMEBaseAddress
    @param  PciBus
    @param  PciDevice
    @param  PciFunc
    @param  Port
    @param  DeviceDetected
    @param  DeviceAddress
    @param  NumHeads
    @param  LBANumHeads
    @param  NumCylinders
    @param  LBANumCyls
    @param  NumSectors
    @param  LBANumSectors
    @param  MaxLBA
    @param  BlockSize
    @param  StorageType
    @param  PNM[27]
    @param  NVMEDevice
    @param  NVMEManufactureId[NVME_MANUFACTUREID_LENGTH]

**/

typedef struct {
    UINT32      TransferBufferAddress;
    UINT64      NVMEBaseAddress;
    UINT8       PciBus;
    UINT8       PciDevice;
    UINT8       PciFunc;
    UINT8       Port;
    BOOLEAN     DeviceDetected;
    UINT8       DeviceAddress;
    UINT8       NumHeads;
    UINT8       LBANumHeads;
    UINT16      NumCylinders;
    UINT16      LBANumCyls;
    UINT8       NumSectors;
    UINT8       LBANumSectors;
    UINT64      MaxLBA;
    UINT16      BlockSize;
    UINT8       StorageType;
    UINT8       PNM[27];
    BOOLEAN     NVMEDevice;
    UINT8       NVMEManufactureId[NVME_MANUFACTUREID_LENGTH];
} NVME_CONTROLLER_INFO;

/**
    This is a URP (NVME Request Packet) structure for the BIOS
    API call Reset NVME

    @param  DeviceAddress
**/

typedef struct {
    UINT8       DeviceAddress;
} RESET_NVME;


/**
    This is a URP (NVME Request Packet) structure for the BIOS
    API call Read 

    @param  DeviceAddress
    @param  Port
    @param  LBA
    @param  NumBlks
    @param  BufferAddress

**/

typedef struct {
    UINT8       DeviceAddress;
    UINT8       Port;
    UINT64      LBA;            // Starting LBA address
    UINT16      NumBlks;        // Number of blocks to read
    UINT32      BufferAddress;  // Far buffer pointer
} NVME_READ_DATA;


/**
    This is a URP (NVME Request Packet) structure for the BIOS
    API call Device Geometry

    @param  DeviceAddress
    @param  NumHeads
    @param  NumCylinders
    @param  NumSectors
    @param  LBANumHeads
    @param  LBANumCyls
    @param  LBANumSectors
    @param  BlockSize
    @param  MediaType
    @param  MaxLBA
    @param  Int13FunctionNo
    @param  BpbMediaDesc

**/
typedef struct {
    UINT8       DeviceAddress;
    UINT8       NumHeads;
    UINT16      NumCylinders;
    UINT8       NumSectors;
    UINT8       LBANumHeads;
    UINT16      LBANumCyls;
    UINT8       LBANumSectors;
    UINT16      BlockSize;
    UINT8       MediaType;
    UINT64      MaxLBA;
    UINT8       Int13FunctionNo;
    UINT8       BpbMediaDesc;
} NVME_DEVICE_GEO;


/**
    This is a union data type of all the API related data

    @param  Reset
    @param  ControllerInfo
    @param  Read
    @param  DeviceGeo
**/

typedef union {
    RESET_NVME              Reset;
    NVME_CONTROLLER_INFO         ControllerInfo;
    NVME_READ_DATA               Read;
    NVME_DEVICE_GEO              DeviceGeo;
} NVME_API_DATA;


/**
    This structure is the URP structure

    @param  bFuncNumber
    @param  bSubFunc
    @param  bRetValue
    @param  ApiData

  @note Fields:Name       Type        Description
              --------------------------------------------------
              bFuncNumber UINT8       Function number of the URP
              bSubFunc    UINT8       Sub-func number of the URP
              bRetValue   UINT8       Return value
              ApiData     API_DATA    Refer structure definition
**/

typedef struct {
    UINT8           bFuncNumber;
    UINT8           bSubFunc;
    UINT8           bRetValue;
    NVME_API_DATA   ApiData;
} NVME_STRUC;

#pragma pack()

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
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
