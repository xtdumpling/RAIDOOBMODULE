/**

@copyright
  Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  NGNDimmPlatformCfgData.h

@brief
  Configuration data

**/

#ifndef _NGN_DIMM_PLATFORM_CONFIG_DATA_H_
#define _NGN_DIMM_PLATFORM_CONFIG_DATA_H_



#define NGN_PLATFORM_CAPABILITY_SIG         SIGNATURE_32('P', 'C', 'A', 'P')
#define NGN_DIMM_CONFIGURATION_HEADER_SIG   SIGNATURE_32('D', 'M', 'H', 'D')
#define NGN_DIMM_CURRENT_CONFIG_SIG         SIGNATURE_32('C', 'C', 'U', 'R')
#define NGN_DIMM_CONFIGURATION_INPUT_SIG    SIGNATURE_32('C', 'I', 'N', '_')
#define NGN_DIMM_OUTPUT_CONFIG_SIG          SIGNATURE_32('C', 'O', 'U', 'T')


#define NGN_DIMM_MEMORY_VOLATILE_TYPE   1
#define NGN_DIMM_MEMORY_PERSISTENT_TYPE 2

#define NGN_DIMM_MANAGEMENT_INTERFACE_REVISION  "0.86"

#define MAX_CUR_CFG_SIZE    sizeof(NGN_DIMM_PLATFORM_CFG_HEADER) + \
                                      sizeof(NGN_DIMM_PLATFORM_CFG_CURRENT_BODY) + \
                                      + (MAX_UNIQUE_NGN_DIMM_INTERLEAVE  * (sizeof(NGN_PCAT_HEADER) + sizeof(NGN_DIMM_INTERLEAVE_INFORMATION_PCAT) + ((MAX_CH * MAX_SOCKET) *sizeof(NGN_DIMM_INTERLEAVE_ID)) ))

#define MAX_CFG_IN_SIZE    sizeof(NGN_DIMM_PLATFORM_CFG_HEADER) + sizeof(NGN_DIMM_PLATFORM_CFG_INPUT_BODY) + \
                                      + (MAX_UNIQUE_NGN_DIMM_INTERLEAVE  * (sizeof(NGN_PCAT_HEADER) + sizeof(NGN_DIMM_INTERLEAVE_INFORMATION_PCAT) + ((MAX_CH * MAX_SOCKET) *sizeof(NGN_DIMM_INTERLEAVE_ID)) )) + \
                                      sizeof(NGN_PCAT_HEADER) + sizeof(NGN_DIMM_PARTITION_SIZE_CHANGE_PCAT)



#define NGN_CURRENT_CONFIGURATION_SIGNATURE  0x52554343
#define NGN_CONFIGURATION_HEADER_SIGNATURE   0x44484D44
#define NGN_CONFIGURATION_INPUT_SIGNATURE    0x5F4E4943
#define NGN_CONFIGURATION_OUTPUT_SIGNATURE   0x54554F43

#define NGN_CR_SW_FW_INTERFACE_REVISION  0x01

#define CFG_CUR  0x00
#define CFG_IN  0x01
#define CFG_OUT  0x02


#define BIOS_PARTITION      0x0
#define OS_PARTITION        0x01
#define FNV_SIZE            0x01
#define FNV_DATA            0x00

#define BYTES_128          0x80

//Error codes
#define NOT_PROCESSED                           0x00
#define PARTITION_TOO_BIG                       0x01
#define INTERLEAVE_PARTICIPANT_UNAVAILABLE      0x02
#define INTERLEAVE_PARAMETER_MISMATCH           0x03
#define FNV_FW_ERROR                            0x04
#define SAD_RULE_UNAVAILABLE                    0x05
#define SPA_UNAVIALABLE                         0x06
#define MISSING_CFG_REQUEST                     0x07
#define PARTITIONING_REQUEST_FAILED             0x08
#define PARTITIONING_SUCCESS                    0x09
#define CHANNEL_INTERLEAVE_MISMATCH             0x0A
#define NM_POPULATION_MISMATCH                  0x0B
#define INTERLEAVE_FORMAT_NOT_VALID             0x0C
#define MIRROR_MAPPING_FAILED                   0x0D
#define INTERLEAVE_NOT_ALIGNED                  0x0E

#define VALIDATION_SUCCESS                      0xFF
#define OPERATION_SUCCESS                       0xFF

#define NGN_CFGCUR_UNDEFINED 0
#define NGN_CFGCUR_RESPONSE_SUCCESS 1
#define NGN_CFG_CUR_RESPONSE_ALL_DIMM_NOT_FOUND 3 
#define NGN_CFG_CUR_RESPONSE_MATCHING_INTERLEAVE_NOT_FOUND 4
#define NGN_CFGCUR_RESPONSE_NEW_DIMM 5
#define NGN_CFGCUR_RESPONSE_CFGIN_ERROR_CCUR_USED 6
#define NGN_CFGCUR_RESPONSE_CFGIN_ERROR_DIMM_NOT_MAPPED 7
#define NGN_CFGCUR_RESPONSE_CFGIN_CHECKSUM_ERROR 8
#define NGN_CFGCUR_RESPONSE_CFGIN_REVISION_ERROR 9
#define NGN_CFGCUR_RESPONSE_CCUR_CHECKSUM_NOT_VALID 10
#define NGN_CFGCUR_RESPONSE_CCUR_REVISION_ERROR 11


#define NGN_VLD_STAT_UNDEFINED 0
#define NGN_VLD_STAT_SUCCESS 1
#define NGN_VLD_STAT_ERROR 2
#define NGN_VLD_STAT_IN_PROGRESS 3
#define NGN_VLD_STAT_COMPLETE_SUCCESS 4
#define NGN_VLD_STAT_COMPLETE_ERROR 5
//Need to add this to the NGNDIMM SW-FW v0.87 spec
#define NGN_VLD_STAT_INTER_REQ_NUM_EXCEED_TWO 6

#define NGN_PART_SIZE_STAT_UNDEFINED 0
#define NGN_PART_SIZE_STAT_SUCCESS 1
#define NGN_PART_SIZE_STAT_INTERLEAVE_PARTICIPANT_MISSING 3
#define NGN_PART_SIZE_STAT_INTERLEAVE_SET_NOT_FOUND 4
#define NGN_PART_SIZE_STAT_PARTITION_SIZE_TOO_BIG 5
#define NGN_PART_SIZE_STAT_AEP_FW_ERROR 6
#define NGN_PART_SIZE_STAT_INSUFFICIENT_DRAM_RULES 7
#define NGN_PART_SIZE_STAT_SIZE_IS_NOT_ALIGNED 8 

#define NGN_INT_CH_STAT_UNDEFINED 0
#define NGN_INT_CH_STAT_SUCCESS 1
#define NGN_INT_CH_STAT_NOT_PROCESSED 2
#define NGN_INT_CH_STAT_MATCHING_DIMM_MISSING 3
#define NGN_INT_CH_STAT_INTERLEAVE_PARAMETERS_MISMATCH 4
#define NGN_INT_CH_STAT_INSUFFICIENT_DRAM_RULES 5
#define NGN_INT_CH_STAT_SYSTEM_ADDRESS_UNAVAILABLE 6
#define NGN_INT_CH_STAT_MIRROR_MAPPING_FAILED 7
#define NGN_INT_CH_STAT_PARTITIONING_FAILED 8
#define NGN_INT_CH_STAT_CFG_REQ_MISSING 9
#define NGN_INT_CH_STAT_CHANNEL_INTERLEAVE_MISMATCH 10
#define NGN_INT_CH_STAT_PARTITION_OFFSET_NOT_ALIGNED 11
//Need to add this to NGNDIMM SW-FW spec V0.87
#define NGN_INT_CH_STAT_NEAR_MEMORY_POPULATION_MISMATCH 12
//Need to add this to NGNDIMM SW-FW spec V0.87
#define NGN_INT_CH_STAT_INTERLEAVE_FORMAT_NOT_VALID 13


#define NGN_PCAT_TYPE_PLATFORM_CAPABILITY_INFORMATION                       0x0
#define NGN_PCAT_TYPE_INTERLEAVE_CAPABILITY_INFORMATION                     0x1
#define NGN_PCAT_TYPE_RUNTIME_CONFIG_CHANGE_VALIDATION                      0x2
#define NGN_PCAT_TYPE_CONFIG_MANAGEMENT_ATTRIBUTES_EXTENSION                0x3
#define NGN_PCAT_TYPE_DIMM_PARTITION_SIZE_CHANGE                            0x4
#define NGN_PCAT_TYPE_DIMM_INTERLEAVE_INFORMATION                           0x5

#define NGN_MAX_MANUFACTURER_STRLEN 2
#define NGN_MAX_SERIALNUMBER_STRLEN 4
#define NGN_MAX_MODELNUMBER_STRLEN  20

#define INTERLEAVE_FORMAT_64B   BIT0
#define INTERLEAVE_FORMAT_128B  BIT1
#define INTERLEAVE_FORMAT_256B  BIT2
#define INTERLEAVE_FORMAT_4KB   BIT6
#define INTERLEAVE_FORMAT_1GB   BIT7

#define  INTERLEAVE_ALIGNMENT_SIZE 30       //Interleave alignment size in 2^n bytes, n = 26 for 64MB, n = 27 for 128MB

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Following structures are used to parse the data defined in the the CR Software-Firmware Management Interface Spec v0.6c
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// NGN DIMM Config Header
//
typedef struct {

  UINT32 Signature;       //!< Signature for this table
  UINT32 Length;          //!< Length in bytes for entire table. It implies the number of Entry fields at the end of the table.
  UINT8 Revision;
  UINT8 Checksum;         //!< Entire table must sum to zero
  UINT8 OemId[6];
  UINT64 OemTableId;      //!< Manufacturer model ID
  UINT32 OemRevision;     //!< Revision of OEM Table ID
  UINT32 CreatorId;       //!< Vendor ID of utility that created the table
  UINT32 CreatorRevision; //!< Revision of utility that created the table

} NGN_DIMM_PLATFORM_CFG_HEADER;

//
// NGN DIMM Config Description
//
typedef struct {

  UINT32 CurrentConfDataSize;    //!< Size of data area allocated to the current configuration information in bytes
  UINT32 CurrentConfStartOffset; //!< Starting location of current configuration. Valid if the size is non-zero
  UINT32 ConfRequestDataSize;    //!< Size of data area allocated to the configuration request area in bytes
  UINT32 ConfRequestDataOffset;  //!< Starting location of configuration request data. Valid if the size is non-zero
  UINT32 ConfResponseDataSize;   //!< Size of data area allocated to the configuration response area in bytes
  UINT32 ConfResponseDataOffset; //!< Starting location of configuration response data. Valid if the size is non-zero

} NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY;

//
// NGN DIMM Current Config
//
typedef struct {

  UINT16 ConfigurationStatus;
  UINT8 Reserved[2] ;
  UINT64 VolatileMemSizeIntoSpa;   //!< Total amount of 2LM size from the NGN DIMM mapped into the SPA
  UINT64 PersistentMemSizeIntoSpa; //!< Total amount of PM size from NGN DIMM mapped into the SPA

} NGN_DIMM_PLATFORM_CFG_CURRENT_BODY;


typedef struct {
  NGN_DIMM_PLATFORM_CFG_HEADER  PlatformCfgHeader;
  NGN_DIMM_PLATFORM_CFG_CURRENT_BODY  CurrentCfgBody;
} NGN_DIMM_PLATFORM_CURRENT_CFG;

//
// NGN DIMM Configuration Request
//
typedef struct {

  /**
    Request sequence number. The BIOS copies this sequence number to the response structure once the BIOS
    completes processing the request input
  **/
  UINT32 SequenceNumber;
  UINT8 Reserved[8];
} NGN_DIMM_PLATFORM_CFG_INPUT_BODY;

//
// NGN DIMM Configuration Output
//
typedef struct {

  UINT32 SequenceNumber;  //!< Copy of the request sequence number to indicate that the BIOS has processed the request
  UINT8 ValidationStatus;
  UINT8 Reserved[7];
} NGN_DIMM_PLATFORM_CFG_OUTPUT_BODY;

//
// NGN DIMM Platform Capabilities
//
typedef struct {
  /**
    Bit0 - Clear - BIOS does not allow config change request from CR management software
    Bit0 - Set - BIOS allows config change request from NGN management software
  **/
  UINT8 NGNManagementConfigRequestSupport;
  /**
    Bit0: Set - 1LM Mode supported
    Bit1: Set - 2LM Mode supported
    Bit2: Set - PM-Direct Mode supported
    Bit3: Set - PM-Cache Mode supported
    Bit4: Set - Block Mode supported
  **/
  UINT8 MemoryModeCapabilities;
  /**
    1 - 1LM Mode selected in BIOS Setup
    2 - 2LM + PM-Direct Mode selected in BIOS Setup
    3 - 2LM + PM-Cache Mode selected in BIOS Setup
    4 - Auto selected in BIOS Setup (2LM if DDR4 + AEP DIMM with volatile mode present, 1LM otherwise)
  **/
  UINT8 CurrentMemoryMode;
  /**
    Bit0: Set - AEP DIMM Persistent Mem Mirror Supported
  **/
  UINT8 PersistentMemoryMirrorCapabilities;
} NGN_PLATFORM_CAPABILITY_BODY;


//
// NGN DIMM PCAT Table Header
typedef struct {

  UINT16 Type;
  UINT16 Length ;

} NGN_PCAT_HEADER;

//
// NGN DIMM Partition Size Change PCAT Table
//
typedef struct {

  /**
    Valid on config output structure

    Byte [1-0]:
    0 - Undefined
    1 - Success
    2 - Reserved
    3 - All the DIMMs in the interleave set not found
    4 - Matching Interleave set not found (Matching DIMMs found)
    5 - Total partition size defined in the interleave set exceeds the partition size input
    6 - NGN DIMM FW returned error
    7 - insufficient number of DRAM Decoders available to map all the DIMMs in the 
        interleave set. Repartition not performed
    8 - Persistent memory partition size is not aligned to the Interleave Alignment 
        size specified in the Interface Capability Information Table    

    Byte [3-2]: Intel NVDIMM FW Error Response Code

  **/
  UINT32 ResponseStatus;
  /**
    Size of persistent memory partition on the request and actually allocated persistent memory size on the response
  **/
  UINT64 PartitionSize;

} NGN_DIMM_PARTITION_SIZE_CHANGE_PCAT;

//
// NGN DIMM Interleave Information PCAT Table
//
typedef struct {


  /**
    Logical index number
  **/
  UINT16 InterleaveSetIndex;
  /**
    Total number of DIMMs participating in this interleave set
  **/
  UINT8 NumOfDimmsInInterleaveSet;
  /**
    1 - Volatile
    2 - Persistent
  **/
  UINT8 InterleaveMemoryType;
  /**
    Interleave format to be used for this interleave set
  **/
  UINT32 InterleaveFormat;
  /**
    Config input:
    0 - Disable mirror for this interleave set
    1 - Enable mirror for this interleave set

    Config output:
    0 - Mirror disabled for this interleave set
    1 - Mirror enabled for this interleave set
  **/
  UINT8 MirrorEnable;
  /**
    Config Output:
    0 - Information not processed
    1 - Successfully interleaved the request
    2 - Unable to find matching DIMMs in the interleave set
    3 - Matching DIMMs found, but interleave information does not match
    4 - Insufficient numnber of DRAM decoders available to map all the DIMMs in the interleave set
    5 - Memory mapping failed due to unavailable system address space
    6 - Mirror mapping failed due to unavailable resources
  **/
  UINT8 InterleaveChangeResponseStatus;
  /**
    Config Input:
    0-This interleave set is not a spare
    1-Reserve this interleave set as spare
  **/
  UINT8 MemorySpare;
  UINT8 Reserved[9];
} NGN_DIMM_INTERLEAVE_INFORMATION_PCAT;

//
// NGN DIMM Interleave Information
//
typedef struct {
  /**
    Manufacturer ID string from NGN DIMM
  **/
  UINT8 DimmManufacturerId[NGN_MAX_MANUFACTURER_STRLEN];
  /**
    Serial number string from NGN DIMM
  **/
  UINT8 DimmSerialNumber[NGN_MAX_SERIALNUMBER_STRLEN];
  /**
    Model number string for NGN DIMM
  **/
  UINT8 DimmModelNumber[NGN_MAX_MODELNUMBER_STRLEN];
  
  UINT8 Reserved[6];
  /**
    Logical offset from the base of the partition type
  **/
  UINT64 PartitionOffset;
  /**
    Size in bytes contributed by this DIMM for this interleave set
  **/
  UINT64 PartitionSize;
} NGN_DIMM_INTERLEAVE_ID;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Following structures are used to represent the data defined in the the CR Software-Firmware Management Interface Spec
// v0.6c in the host structure.  These data structures remove some fields that are not required for the host structure to
// optimize the space used by the host structure.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// HOST NGN DIMM Current Config
//
typedef struct {

  UINT16 ConfigurationStatus;
  UINT16 VolatileMemSizeIntoSpa;   //!< Total amount of 2LM size from the NGN DIMM mapped into the SPA ; 64-bit field in interface specification ; Store in 64MB granularity ; 16-bits can store 2TB DIMM
  UINT16 PersistentMemSizeIntoSpa; //!< Total amount of PM size from NGN DIMM mapped into the SPA ; 64-bit field in interface specification ; Store in 64MB granularity ; 16-bits can store 2TB DIMM

} NGN_DIMM_PLATFORM_CFG_CURRENT_BODY_HOST;

//
// HOST NGN DIMM Configuration Request
//
typedef struct {

  /**
    Request sequence number. The BIOS copies this sequence number to the response structure once the BIOS
    completes processing the request input
  **/
  UINT32 SequenceNumber;

} NGN_DIMM_PLATFORM_CFG_INPUT_BODY_HOST;

//
// HOST NGN DIMM Configuration Output
//
typedef struct {

  UINT32 SequenceNumber;  //!< Copy of the request sequence number to indicate that the BIOS has processed the request
  UINT8 ValidationStatus;

} NGN_DIMM_PLATFORM_CFG_OUTPUT_BODY_HOST;

//
// HOST NGN DIMM Partition Size Change PCAT Table
//
typedef struct {
  BOOLEAN RecPresent;
  BOOLEAN Valid;
  /**
    Size of persistent memory partition on the request and actually allocated persistent memory size on the response
  **/
  UINT16 PartitionSize;     // 64-bit field in interface specification ; Store in 64MB granularity ; 16-bits can store 2TB DIMM
  UINT16 ResponseStatus;

} NGN_DIMM_PARTITION_SIZE_CHANGE_PCAT_HOST;

//
// HOST NGN DIMM Interleave Info
//

//
// NGN DIMM Interleave Information PCAT Table
//
typedef struct {
  BOOLEAN RecPresent;
  BOOLEAN Valid;
  BOOLEAN Processed;
  UINT32  SadLimit;
  /**
    Logical index number
  **/
  UINT16 InterleaveSetIndex;
  /**
    Total number of DIMMs participating in this interleave set
  **/
  UINT8 NumOfDimmsInInterleaveSet;
  /**
    1 - Volatile
    2 - Persistent
  **/
  UINT8 InterleaveMemoryType;
  /**
    Interleave format to be used for this interleave set
  **/
  UINT32 InterleaveFormat;
  /**
    Config input:
    0 - Disable mirror for this interleave set
    1 - Enable mirror for this interleave set

    Config output:
    0 - Mirror disabled for this interleave set
    1 - Mirror enabled for this interleave set
  **/
  UINT8 MirrorEnable;
  /**
    Config Output:
    0 - Information not processed
    1 - Successfully interleaved the request
    2 - Unable to find matching DIMMs in the interleave set
    3 - Matching DIMMs found, but interleave information does not match
    4 - Insufficient numnber of DRAM decoders available to map all the DIMMs in the interleave set
    5 - Memory mapping failed due to unavailable system address space
    6 - Mirror mapping failed due to unavailable resources
  **/
  UINT8 InterleaveChangeResponseStatus;
  /**
    Config Input:
    0-This interleave set is not a spare
    1-Reserve this interleave set as spare
  **/
  UINT8 MemorySpare;
} NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST;

typedef struct {
  UINT8 socket;
  UINT8 channel;
  UINT8 dimm;
  UINT16 PartitionOffset;     // 64-bit field in interface specification ; Store in 64MB granularity ; 16-bits can store 2TB DIMM
  UINT16 PartitionSize;       // 64-bit field in interface specification ; Store in 64MB granularity ; 16-bits can store 2TB DIMM
} NGN_DIMM_INTERLEAVE_ID_HOST;


#pragma pack()

#endif
