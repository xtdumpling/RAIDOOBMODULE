//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file NGNRecordsData.h

  GUID used for Memory Map Data entries in the HOB list.

**/

#ifndef _NGN_RECORDS_DATA_H_
#define _NGN_RECORDS_DATA_H_

#include "SysHost.h" 
#include "UncoreCommonIncludes.h"

// {A0E4DE1F-ED01-419a-957D-237903108BA8}
extern EFI_GUID gEfiNGNCfgCurGuid;
// {415928AD-31D9-4cbf-9E92-6D1F67EC5711}
extern EFI_GUID gEfiNGNCfgOutGuid;

//
//  NG NVM DIMM Current Configuration HOB Information
//
#pragma pack(1)

struct cfgCurEntry {
  UINT8 socket;                       // Socket number for NG NVM DIMM
  UINT8 channel;                      // Channel number for NG NVM DIMM
  UINT8 dimm;                         // DIMM slot number for NG NVM DIMM
  UINT8 manufacturer[NGN_MAX_MANUFACTURER_STRLEN];  // Manufacturer
  UINT8 serialNumber[NGN_MAX_SERIALNUMBER_STRLEN];  // Serial Number
  UINT8 modelNumber[NGN_MAX_MODELNUMBER_STRLEN];    // Model Number
  NGN_DIMM_CFG_CUR_HOST cfgCurData;   // Current configuration table for NG NVM DIMM populated in socket, channel, dimm
};

struct cfgOutEntry {
  UINT8 socket;                       // Socket number for NG NVM DIMM
  UINT8 channel;                      // Channel number for NG NVM DIMM
  UINT8 dimm;                         // DIMM slot number for NG NVM DIMM
  UINT8 manufacturer[NGN_MAX_MANUFACTURER_STRLEN];  // Manufacturer
  UINT8 serialNumber[NGN_MAX_SERIALNUMBER_STRLEN];  // Serial Number
  UINT8 modelNumber[NGN_MAX_MODELNUMBER_STRLEN];    // Model Number
  NGN_DIMM_CFG_OUT_HOST cfgOutData;   // Output configuration table for NG NVM DIMM populated in socket, channel, dimm
};

struct NGNCfgCurRecordHob {  
  UINT8   numCfgCurRecords ;                                  // Number of current configuration records in this HOB record
  struct  cfgCurEntry cfgCurRecord[MAX_SYS_CH * MAX_AEP_CH] ; // Current configuration records                      
};

struct NGNCfgOutRecordHob {
  UINT8   numCfgOutRecords ;                                  // Number of output configuration records in this HOB record
  struct  cfgOutEntry cfgOutRecord[MAX_SYS_CH * MAX_AEP_CH] ; // Output configuration records
};

#pragma pack()

#endif
