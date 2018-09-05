/**

INTEL CONFIDENTIAL
Copyright 2013 - 2016 Intel Corporation All Rights Reserved.

The source code contained or described herein and all documents
related to the source code ("Material") are owned by Intel Corporation
or its suppliers or licensors. Title to the Material remains with
Intel Corporation or its suppliers and licensors. The Material
contains trade secrets and proprietary and confidential information of
Intel or its suppliers and licensors. The Material is protected by
worldwide copyright and trade secret laws and treaty provisions. No
part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed, or disclosed in
any way without Intel's prior express written permission.

No license under any patent, copyright, trade secret or other
intellectual property right is granted to or conferred upon you by
disclosure or delivery of the Materials, either expressly, by
implication, inducement, estoppel or otherwise. Any license under such
intellectual property rights must be express and approved by Intel in
writing.

**/
/**

   Any software source code reprinted in this document is furnished under 
   a software license and may only be used or copied in accordance with 
   the terms of that license.
 

   Copyright (c) 2013, Intel Corporation. All rights reserved.

**/

#ifndef _ICC_DATA_PROTOCOL_H_
#define _ICC_DATA_PROTOCOL_H_

extern EFI_GUID gIccGuid;
#ifndef ICC_DATA_GUID
#define ICC_DATA_GUID {0x64192dca, 0xd034, 0x49d2, 0xa6, 0xde, 0x65, 0xa8, 0x29, 0xeb, 0x4c, 0x74}
#endif

extern EFI_GUID gIccDataProtocolGuid;

#pragma pack(1)
typedef struct _ICC_MBP_DATA {
  UINT8 NumProfiles;
  UINT8 Profile;
  UINT8 ProfileSelectionAllowed;
  UINT8 RegLock;
} ICC_MBP_DATA;

typedef struct _PLATFORM_FVI_SETUP{
  UINT8 LanPhyVersion;
  UINT16 IccMajorVersion;
  UINT16 IccMinorVersion;
  UINT16 IccHotfixVersion;
  UINT16 IccBuildVersion;
} PLATFORM_FVI_SETUP;
#pragma pack()

typedef
EFI_STATUS
(EFIAPI *ICC_GET_DATA) (
  IN OUT  ICC_MBP_DATA *  IccMbpData
  );

typedef struct _ICC_DATA_PROTOCOL {
  ICC_GET_DATA GetMbpData;
} ICC_DATA_PROTOCOL;

#endif /* ICC_DATA_PROTOCOL */
