/*++
Copyright (c) 2014, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


Module Name:

  PlatformCfgData.h

--*/
#ifndef _PLATFORMCFGDATA_H_
#define _PLATFORMCFGDATA_H_

#include <Guid/NGNRecordsData.h>

#define NGN_LARGE_PAYLOAD_IN    0
#define NGN_LARGE_PAYLOAD_OUT   1
#define NGN_INT_BUFFER_SIZE     1024

#define NGN_NOT_POPULATED       0xFF

// Functions in PlatformCfgData.c
EFI_STATUS UpdatePCD( VOID ) ;
EFI_STATUS UpdatePCDBIOSPartition( UINT8 skt, UINT8 ch, UINT8 dimm, UINT8 *cfgCurRecBuffer, UINT32 cfgCurRecLength ) ;
EFI_STATUS UpdatePCDOSPartition( UINT8 skt, UINT8 ch, UINT8 dimm, UINT8 *cfgCurRecBuffer, UINT32 cfgCurRecLength, UINT8 *cfgOutRecBuffer, UINT32 cfgOutRecLength ) ;
EFI_STATUS SetPCD( UINT8 skt, UINT8 ch, UINT8 dimm, UINT8 partitionId ) ;
EFI_STATUS GetPCD( UINT8 skt, UINT8 ch, UINT8 dimm, UINT8 partitionId ) ;
VOID CreateCfgCurRecordFromHOBData( UINT8 index, UINT8 **cfgRecord, UINT32 *cfgRecordLength ) ;
VOID CreateCfgOutRecordFromHOBData( UINT8 index, UINT8 **cfgRecord, UINT32 *cfgRecordLength ) ;
EFI_STATUS LoadHOBsForPCDUpdates( VOID ) ;
EFI_STATUS CopyFromLargePayloadToBuffer(UINT8 srcSkt, UINT8 srcCh, UINT8 srcDimm, UINT32 srcOffset, UINT8 *dstBuffer, UINT32 dstOffset, UINT32 length, BOOLEAN largePayloadRegion); 
EFI_STATUS CopyFromBufferToLargePayload(UINT8 *srcBuffer, UINT32 srcOffset, UINT8 dstSkt, UINT8 dstCh, UINT8 dstDimm, UINT32 dstOffset, UINT32 length, BOOLEAN largePayloadRegion); 
EFI_STATUS CopyLPOutputToLPInput( UINT8 srcSkt, UINT8 srcCh, UINT8 srcDimm, UINT32 srcOffset, UINT8 dstSkt, UINT8 dstCh, UINT8 dstDimm, UINT32 dstOffset, UINT32 length ) ;
EFI_STATUS ComputeChecksum( UINT8 *srcBuffer, UINT32 length, UINT8 *chksum ) ;
EFI_STATUS DisplayBuffer( UINT8 *buffer, UINT32 length ) ;
EFI_STATUS ZeroLargePayloadInput(UINT8 skt, UINT8 ch, UINT8 dimm );

#endif // _PLATFORMCFGDATA_H_