/*************************************************************************
 *
 * Memory Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2013-15 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 ************************************************************************
 *
 *  PURPOSE:
 *
 *  This file contains code to support MSVC simulation environment for
 *  testing memory decoder algorithms.
 *
 ************************************************************************/

#ifndef _MemSpdSimInit_h
#define _MemSpdSimInit_h

#include "RcSim.h"

#if defined(_MSC_VER)
#define strtoll   _strtoi64
#define strtoull  _strtoui64
#endif


#define MMS_SKT0        0
#define MMS_SKT1        1
#define MMS_SKT2        2
#define MMS_SKT3        3

#define MMS_IMC0        0
#define MMS_IMC1        1

#define MMS_CH0         0
#define MMS_CH1         1
#define MMS_CH2         2

#define MMS_SLOT0       0
#define MMS_SLOT1       1

#define MMS_QUAD_RNK    4

#define MMS_DDR4_TYPE      0
#define MMS_AEP_TYPE       1
#define MMS_GENERIC_TYPE   2

#define MMS_DDRT_RAW_PARMS       0
#define MMS_DDRT_FILE_PARMS      1
#define MMS_GENERIC_DIMM_PARMS   2

#define MAX_LINE        256

#define RCSIMCAPID0               "capid0"
#define RCSIMCAPID1               "capid1"
#define RCSIMCAPID2               "capid2"
#define RCSIMCAPID3               "capid3"
#define RCSIMCAPID4               "capid4"
#define RCSIMCAPID5               "capid5"
#define RCSIMCAPID6               "capid6"
#define RCSIMMEMFLOWS             "memFlows"
#define RCSIMMEMFLOWSEXT          "memFlowsExt"
#define RCSIMCPUSTEPPING          "cpuStepping"
#define RCSIMCPUFAMILY            "cpuFamily"


// DDR4 DIMM Structure
typedef struct DDR4DIMM {
  UINT8 numberOfRanks ; // Number of ranks
  UINT16 rankSize ;      // Rank size
} DDR4DIMM ;

// NVM DIMM Parameters Structure
typedef struct DDRTDIMMParameters {
  //UINT8           manufacturer[MAX_MANUFACTURER_STRLEN];  // Manufacturer
  //UINT8           serialNumber[MAX_SERIALNUMBER_STRLEN];  // Serial Number
  UINT32          rawCap;             // Raw Capacity
  UINT32          volCap;             // Volatile Capacity (2LM)
  UINT32          perCap;             // Persistent Capacity (PM)
  UINT32          blkCap;             // Block Capacity (BLK)
  UINT32          volRegionDPA;       // DPA start address of 2LM Region
  UINT32          perRegionDPA;       // DPA start address of PM Region
} DDRTDIMMParameters ;

// Generic DIMM Structure
typedef struct GENERICDIMM {
  UINT8 dimmDescriptionType;
  char  *filename;
} GENERICDIMM ;

// NVM DIMM Structure
typedef struct DDRTDIMM {
  UINT8 dimmDescriptionType;
  union  {
    char   *filename;
    struct DDRTDIMMParameters ddrtDIMMParameters;
  } dimmDescription ;
} DDRTDIMM ;

// DIMM Definition Structure
typedef struct DIMMDefinition {
  struct DIMMDefinition *next ;
  char   *dimmName ;
  UINT8 dimmType ;
  union {
    struct GENERICDIMM genericDIMM ;
    struct DDR4DIMM ddr4DIMM ;
    struct DDRTDIMM ddrtDIMM ;
  } dimm ;
} DIMMDefinition ;

// Setup Variables Structure
typedef struct SetupVarDefinition {
  char    *varName;
  UINT8   useOverrideValue;
  UINT8   hostStruct;
  UINT32  overrideValue;
  size_t  variableSize ;
  void    *variable;
} SetupVarDefinition ;


// Internal support functions
UINT32 RcSimSetSetupParamOverride (char *paramName, void *valuePtr);
void InitSetupOptions(PSYSHOST host) ;
void InitPlatformTopology( PSYSHOST host ) ;
void InitMemoryConfiguration( PSYSHOST host ) ;
int ProcessIniFile( PSYSHOST host, FILE *iniFile, char **nvramFileName ) ;
int ProcessIniFileDDR4DIMMDefinition( FILE *iniFile, int number ) ;
int ProcessIniFileAEPDIMMDefinition( FILE *iniFile, int number ) ;
int ProcessIniFileDIMMPopulation( PSYSHOST host, FILE *iniFile, int number ) ;
int ProcessIniFileSetup( FILE *iniFile, int number  ) ;
int GetKeyValue( FILE *iniFile, char *section, char *key, char *value, unsigned int valueSize ) ;
int FindSection( FILE *iniFile, char *section ) ;
int FindKeyValue( FILE *iniFile, char *key, char *value, unsigned int valueSize ) ;
int CreateDDR4DIMMDefinitionEntry( char *dimmName, UINT8 numberOfRanks, UINT16 rankSize ) ;
int CreateDDRTDIMMDefinitionEntry( char *dimmName, UINT8 dimmDescriptionType, char *manufacturer, char *serialNumber, UINT32 rawCap, UINT32 volCap, UINT32 perCap, UINT32 blkCap, UINT32 volRegionDPA, UINT32 perRegionDPA, char *filename ) ;
BOOLEAN AddDIMM( PSYSHOST host, UINT8 socket, UINT8 imc, UINT8 ch, UINT8 dimm, DIMMDefinition *dimmDefinition ) ;



// Eye Definition Structure
typedef struct EyeDefinition {
  struct EyeDefinition *next ;
  char   *eyeName ;
  char   *rxfilename;
  char   *txfilename;
} EyeDefinition ;


UINT32 EyeFeedbackIniFileProcessing (PSYSHOST host, FILE *iniFile);
int ProcessEyeIniFile (PSYSHOST host, FILE *iniFile);
int ProcessIniFileEyeDefinition(FILE *iniFile, int index);
int ProcessIniFileEyePopulation(PSYSHOST host, FILE *iniFile, int index);
int CreateEyeDefinitionEntry ( char *eyeName,  char *txfilename, char *rxfilename);
BOOLEAN AddEye( PSYSHOST host, UINT8 socket, UINT8 imc, UINT8 ch, UINT8 dimm, UINT8 rank, EyeDefinition *eyeDefinition);


#endif // _MemSpdSimInit_h

