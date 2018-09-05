/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++
Copyright (c) 2015 - 2016, Intel Corporation.

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

  PlatformCfgData.c

Abstract:

  Implementation of DXE support to update the Platform Configuration Data areas supported by NGN DIMMs

--*/

#include "CrystalRidge.h"
#include "XlateFunctions/XlateFunctions.h"
#include "PlatformCfgData.h"
#include "NGNRecordsData.h"
#include <SysHostChip.h>

#pragma optimize("", off)

extern
EFI_STATUS
SendFnvCommand(
  PAEP_DIMM pDimm,
  UINT16    Opcode
  );

extern
VOID
IssueWpqFlush(
  UINT16  SocketId,
  UINT16  ImcId,
  BOOLEAN SecondSfence
  );

extern
VOID
DurableCacheLineWrite (
  UINTN   CsrAddress,
  UINT64  Data,
  UINT8   DataSize
  );

UINT8                             mNGNCfgCurRecordIndices[MAX_SOCKET][MAX_CH][MAX_DIMM] ;
UINT8                             mNGNCfgOutRecordIndices[MAX_SOCKET][MAX_CH][MAX_DIMM] ;
struct NGNCfgCurRecordHob         *mNGNCfgCurRecord;
struct NGNCfgOutRecordHob         *mNGNCfgOutRecord;
// Global boolean to tell whether to use BIOS Mailbox or OS Mailbox (used for _DSM commands only)
extern BOOLEAN                    mUseOSMailbox;
extern UINT8                      *mLargePayloadBuffer;
extern CR_INFO                    mCrInfo;
extern struct SystemMemoryMapHob  *mSystemMemoryMap;


/*++

  Update the platform configuration data for each NGN DIMM

  @retval TBD

++*/
EFI_STATUS UpdatePCD( VOID )
{
  EFI_STATUS  status ;
  UINT8       skt, ch, dimm ;
  UINT8       *cfgCurRecBuffer = NULL ;
  UINT32      cfgCurRecLength = 0 ;
  UINT8       *cfgOutRecBuffer = NULL ;
  UINT32      cfgOutRecLength = 0 ;
  BOOLEAN     CurrentMailbox;

  // DEBUG(( EFI_D_ERROR, "UpdatePCD()\n" )) ;

  // Force BIOS Mailbox, needed for updating BIOS partition.
  CurrentMailbox = mUseOSMailbox;
  mUseOSMailbox = FALSE;

  // Load HOBs with the CfgCur and CfgOut records and create indicies
  status = LoadHOBsForPCDUpdates( ) ;
  if( status != EFI_SUCCESS )  {
    return( status ) ;
  }

  // Update platform configuration data for each DIMM
  for( skt = 0 ; skt < MAX_SOCKET ; skt++ )  {
    for( ch = 0 ; ch < MAX_CH ; ch++ )  {
      for( dimm = 0 ; dimm < MAX_DIMM ; dimm++ )  {

        // If DIMM has an update to the CfgCur record then create the record
        if( mNGNCfgCurRecordIndices[skt][ch][dimm] != NGN_NOT_POPULATED )  {

          // Create the CfgCur record in the local buffer from the HOB data
          CreateCfgCurRecordFromHOBData( mNGNCfgCurRecordIndices[skt][ch][dimm], &cfgCurRecBuffer, &cfgCurRecLength ) ;

          // Call OEM-hook to update/modify Current Config record in buffer
          OemBeforeUpdatePCDBIOSPartition(skt, ch, dimm, &cfgCurRecBuffer, &cfgCurRecLength ) ;

          // Display record
          DEBUG(( EFI_D_ERROR, "CurCfg Record for Skt[%d] Ch[%d] Dimm[%d]\n", skt, ch, dimm )) ;
          DisplayBuffer( cfgCurRecBuffer, cfgCurRecLength ) ;
        }

        // Update PCD BIOS partition
        if( cfgCurRecBuffer != NULL )  {
          DEBUG(( EFI_D_ERROR, "Updating PCD BIOS Partition for Skt[%d] Ch[%d] Dimm[%d]\n", skt, ch, dimm )) ;
          UpdatePCDBIOSPartition( skt, ch, dimm, cfgCurRecBuffer, cfgCurRecLength ) ;
        }

        // Read the platform configuration data to aid in updating the PCD OS partition
        if( ( mNGNCfgCurRecordIndices[skt][ch][dimm] != NGN_NOT_POPULATED ) || ( mNGNCfgOutRecordIndices[skt][ch][dimm] != NGN_NOT_POPULATED ) ) {
          status = GetPCD( skt, ch, dimm, OS_PARTITION ) ;
          DEBUG(( EFI_D_ERROR, "Read the platform configuration data to aid in creating the updated CfgOut record and updating the PCD OS partition Status = %X\n", status )) ;
          ASSERT(status == EFI_SUCCESS);
        }

        // If DIMM has an update to the CfgOut record then create the record
        if( mNGNCfgOutRecordIndices[skt][ch][dimm] != NGN_NOT_POPULATED )  {

          // Create the CfgOut record in the local buffer from the HOB data
          CreateCfgOutRecordFromHOBData( mNGNCfgOutRecordIndices[skt][ch][dimm], &cfgOutRecBuffer, &cfgOutRecLength ) ;

          // Call OEM-hook to update/modify Config Output record in buffer
          OemBeforeUpdatePCDOSPartition(skt, ch, dimm, &cfgOutRecBuffer, &cfgOutRecLength ) ;

          // Display record
          DEBUG(( EFI_D_ERROR, "CfgOut Record for Skt[%d] Ch[%d] Dimm[%d]\n", skt, ch, dimm )) ;
          DisplayBuffer( cfgOutRecBuffer, cfgOutRecLength ) ;
        }

        // Update PCD OS partition
        if( ( cfgCurRecBuffer != NULL ) || ( cfgOutRecBuffer != NULL ) )  {
          DEBUG(( EFI_D_ERROR, "Updating PCD OS Partition for Skt[%d] Ch[%d] Dimm[%d]\n", skt, ch, dimm )) ;
          UpdatePCDOSPartition( skt, ch, dimm, cfgCurRecBuffer, cfgCurRecLength, cfgOutRecBuffer, cfgOutRecLength ) ;
        }

        // Free local buffer with CfgCur record
        if( cfgCurRecBuffer != NULL )  {
          gBS->FreePool( cfgCurRecBuffer ) ;
          cfgCurRecBuffer = NULL ;
          cfgCurRecLength = 0 ;
        }

        // Free local buffer with CfgOut record
        if( cfgOutRecBuffer != NULL )  {
          gBS->FreePool( cfgOutRecBuffer ) ;
          cfgOutRecBuffer = NULL ;
          cfgOutRecLength = 0 ;
        }
      }
    }
  }
  mUseOSMailbox = CurrentMailbox;
  return( EFI_SUCCESS ) ;
}

/*++

  Update the platform configuration data stored in the BIOS partiton for each NGN DIMM

  @param skt             - Socket number
  @param ch              - Channel number
  @param dimm            - DIMM number
  @param cfgCurBuffer    - CfgCur record buffer
  @param cfgCurRecLength - CfgCur record length

  @retval TBD

++*/
EFI_STATUS UpdatePCDBIOSPartition( UINT8 skt, UINT8 ch, UINT8 dimm, UINT8 *cfgCurRecBuffer, UINT32 cfgCurRecLength )
{
  EFI_STATUS status ;
  UINT8 *buffer ;
  UINT32 lengthPlatformCfgHeaderBody ;
  UINT32 largePayloadOutputOffset ;
  NGN_DIMM_PLATFORM_CFG_HEADER *headerDIMMCfg ;
  NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY *headerDIMMBody ;
  UINT64 TempOemTableId ;

  //DEBUG(( EFI_D_ERROR, "UpdatePCDBIOSPartiton()\n" )) ;
  // Create a buffer for the record
  lengthPlatformCfgHeaderBody = sizeof( NGN_DIMM_PLATFORM_CFG_HEADER ) + sizeof( NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY ) ;
  status = gBS->AllocatePool( EfiBootServicesData, lengthPlatformCfgHeaderBody, &buffer ) ;
  ASSERT_EFI_ERROR( status ) ;

  // Zero buffer
  ZeroMem( buffer, lengthPlatformCfgHeaderBody ) ;

  // zero out the large payload input before using it.
  status = ZeroLargePayloadInput( skt, ch, dimm );

  TempOemTableId  = PcdGet64(PcdAcpiDefaultOemTableId);

  // Set pointers to header and body
  headerDIMMCfg = ( NGN_DIMM_PLATFORM_CFG_HEADER * ) buffer ;
  headerDIMMBody = ( NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY * ) ( buffer + sizeof( NGN_DIMM_PLATFORM_CFG_HEADER ) ) ;


  // Create DIMM configuration header each time PCD is written as it may be corrupted
  headerDIMMCfg->Signature = NGN_DIMM_CONFIGURATION_HEADER_SIG ;
  headerDIMMCfg->Length = lengthPlatformCfgHeaderBody ;
  headerDIMMCfg->Revision = 1 ;
  headerDIMMCfg->Checksum = 0 ;
  CopyMem(headerDIMMCfg->OemId,          PcdGetPtr (PcdAcpiDefaultOemId), sizeof(headerDIMMCfg->OemId));
  CopyMem(&(headerDIMMCfg->OemTableId),  &TempOemTableId, sizeof(headerDIMMCfg->OemTableId));
  headerDIMMCfg->OemRevision = EFI_ACPI_OEM_REVISION;
  headerDIMMCfg->CreatorId = EFI_ACPI_CREATOR_ID;
  headerDIMMCfg->CreatorRevision = EFI_ACPI_CREATOR_REVISION;

  // Update offset into the large payload region input buffer
  largePayloadOutputOffset = lengthPlatformCfgHeaderBody ;

  if (mSystemMemoryMap->cpuStepping < H0_REV_SKX) {
#ifdef __GNUC__
    asm("sfence");
#else
    _mm_sfence();
#endif
  } else {
    IssueWpqFlush(skt, ch > 2 ? 1 : 0, TRUE);
  }

  // Copy local buffer to large payload region
  status = CopyFromBufferToLargePayload(cfgCurRecBuffer, 0, skt, ch, dimm, largePayloadOutputOffset, cfgCurRecLength, LARGE_PAYLOAD_INPUT);
  if( status != EFI_SUCCESS )  {
    return( status ) ;
  }

  // Update CfgCur length parameter in DIMM configuration header since this is a new record
  headerDIMMBody->CurrentConfDataSize = cfgCurRecLength ;
  // Update CfgCur parameters in DIMM configuration header
  headerDIMMBody->CurrentConfStartOffset = largePayloadOutputOffset ;


  // No CfgIn in DIMM configuration header in BIOS partiton
  headerDIMMBody->ConfRequestDataOffset = 0 ;
  headerDIMMBody->ConfRequestDataSize = 0;

  //No CfgOut in DIMM configuration header in BIOS partition
  headerDIMMBody->ConfResponseDataOffset = 0;
  headerDIMMBody->ConfResponseDataSize = 0;

  // Compute checksum of header
  ComputeChecksum( buffer, lengthPlatformCfgHeaderBody, &(headerDIMMCfg->Checksum) ) ;

  // Copy header record to large payload input region
  status = CopyFromBufferToLargePayload(buffer, 0, skt, ch, dimm, 0, lengthPlatformCfgHeaderBody, LARGE_PAYLOAD_INPUT);
  ASSERT(status == EFI_SUCCESS);

  if (mSystemMemoryMap->cpuStepping < H0_REV_SKX) {
#ifdef __GNUC__
    asm("sfence");
#else
    _mm_sfence();
#endif
    status = CopyFromLargePayloadToBuffer(skt, ch, dimm, 0, mLargePayloadBuffer, 0, lengthPlatformCfgHeaderBody + cfgCurRecLength, LARGE_PAYLOAD_INPUT);
  }

  // Write the platform configuration data
  status = SetPCD( skt, ch, dimm, BIOS_PARTITION ) ;
  ASSERT(status == EFI_SUCCESS);

  return( status ) ;
}


/*++

  Update the platform configuration data stored in the OS partiton for each NGN DIMM

  @param skt             - Socket number
  @param ch              - Channel number
  @param dimm            - DIMM number
  @param cfgCurRecBuffer - CfgCur record buffer
  @param cfgCurRecLength - CfgCur record length
  @param cfgOutRecBuffer - CfgOut record buffer
  @param cfgOutRecLength - CfgOut record length

  @retval TBD

++*/
EFI_STATUS UpdatePCDOSPartition( UINT8 skt, UINT8 ch, UINT8 dimm, UINT8 *cfgCurRecBuffer, UINT32 cfgCurRecLength, UINT8 *cfgOutRecBuffer, UINT32 cfgOutRecLength )
{
  EFI_STATUS status ;
  UINT8 *buffer ;
  UINT32 lengthPlatformCfgHeaderBody ;
  UINT32 largePayloadOutputOffset ;
  NGN_DIMM_PLATFORM_CFG_HEADER *headerDIMMCfg ;
  NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY *headerDIMMBody ;
  UINT64 TempOemTableId ;

  //DEBUG(( EFI_D_ERROR, "UpdatePCDOSPartiton()\n" )) ;

  // Create a buffer for the record
  lengthPlatformCfgHeaderBody = sizeof( NGN_DIMM_PLATFORM_CFG_HEADER ) + sizeof( NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY ) ;
  status = gBS->AllocatePool( EfiBootServicesData, lengthPlatformCfgHeaderBody, &buffer ) ;
  //DEBUG(( EFI_D_ERROR, "Allocate Pool Buffer @ 0x%X of Length %d\n", buffer, lengthPlatformCfgHeaderBody )) ;
  ASSERT_EFI_ERROR( status ) ;

  // Zero buffer
  ZeroMem( buffer, lengthPlatformCfgHeaderBody ) ;

  if (mSystemMemoryMap->cpuStepping >= H0_REV_SKX) {
    IssueWpqFlush(skt, ch > 2 ? 1 : 0, TRUE);
  } else {
#ifdef __GNUC__
    asm("sfence");
#else
    _mm_sfence();
#endif
  }

  // zero out the large payload input before using it.
  status = ZeroLargePayloadInput( skt, ch, dimm );

  //DEBUG(( EFI_D_ERROR, "Copy DIMM configuration header from large payload region to local buffer\n" )) ;


  // Copy DIMM configuration header from large payload region to local buffer
  status = CopyFromLargePayloadToBuffer(skt, ch, dimm, 0, buffer, 0, lengthPlatformCfgHeaderBody, LARGE_PAYLOAD_OUTPUT);
  ASSERT(status == EFI_SUCCESS);

  TempOemTableId  = PcdGet64(PcdAcpiDefaultOemTableId);

  // Set pointers to header and body
  headerDIMMCfg = ( NGN_DIMM_PLATFORM_CFG_HEADER * ) buffer ;
  headerDIMMBody = ( NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY * ) ( buffer + sizeof( NGN_DIMM_PLATFORM_CFG_HEADER ) ) ;

  // Create DIMM configuration header each time PCD is written as it may be corrupted
  headerDIMMCfg->Signature = NGN_DIMM_CONFIGURATION_HEADER_SIG ;
  headerDIMMCfg->Length = lengthPlatformCfgHeaderBody ;
  headerDIMMCfg->Revision = 1 ;
  headerDIMMCfg->Checksum = 0 ;
  CopyMem(headerDIMMCfg->OemId,          PcdGetPtr (PcdAcpiDefaultOemId), sizeof(headerDIMMCfg->OemId));
  CopyMem(&(headerDIMMCfg->OemTableId),  &TempOemTableId, sizeof(headerDIMMCfg->OemTableId));
  headerDIMMCfg->OemRevision = EFI_ACPI_OEM_REVISION;
  headerDIMMCfg->CreatorId = EFI_ACPI_CREATOR_ID;
  headerDIMMCfg->CreatorRevision = EFI_ACPI_CREATOR_REVISION;

  // Update offset into the large payload region input buffer
  largePayloadOutputOffset = lengthPlatformCfgHeaderBody ;

  // Update the CfgCur record if there is a new record
  if( ( cfgCurRecBuffer != NULL ) && ( cfgCurRecLength != 0 ) ) {

    //DEBUG(( EFI_D_ERROR, "Copy CfgCur record to large payload input region\n" )) ;
    // Copy CfgCur record to large payload input region
    status = CopyFromBufferToLargePayload(cfgCurRecBuffer, 0, skt, ch, dimm, largePayloadOutputOffset, cfgCurRecLength, LARGE_PAYLOAD_INPUT);
    ASSERT(status == EFI_SUCCESS);

    // Update CfgCur length parameter in DIMM configuration header since this is a new record
    headerDIMMBody->CurrentConfDataSize = cfgCurRecLength ;

    // Update CfgCur parameters in DIMM configuration header
    headerDIMMBody->CurrentConfStartOffset = largePayloadOutputOffset ;
    largePayloadOutputOffset += headerDIMMBody->CurrentConfDataSize ;
  }

  // Otherwise, then keep the existing CfgCur record if it exists
  else if( headerDIMMBody->CurrentConfDataSize ) {

    //DEBUG(( EFI_D_ERROR, "Copy existing CfgCur record to large payload input region\n" )) ;

    // Copy existing CfgCur record to large payload input region
    status = CopyLPOutputToLPInput( skt, ch, dimm, headerDIMMBody->CurrentConfStartOffset, skt, ch, dimm, largePayloadOutputOffset, headerDIMMBody->CurrentConfDataSize ) ;
    ASSERT(status == EFI_SUCCESS);

    // Update CfgCur parameters in DIMM configuration header
    headerDIMMBody->CurrentConfStartOffset = largePayloadOutputOffset ;
    largePayloadOutputOffset += headerDIMMBody->CurrentConfDataSize ;
  }

  // Keep the existing CfgIn record if it exists
  if( headerDIMMBody->ConfRequestDataSize )  {

    //DEBUG(( EFI_D_ERROR, "Copy existing CfgIn record to large payload input region\n" )) ;

    // Copy existing CfgIn record to large payload input region
    status = CopyLPOutputToLPInput( skt, ch, dimm, headerDIMMBody->ConfRequestDataOffset, skt, ch, dimm, largePayloadOutputOffset, headerDIMMBody->ConfRequestDataSize ) ;
    ASSERT(status == EFI_SUCCESS);

    // Update CfgIn parameters in DIMM configuration header
    headerDIMMBody->ConfRequestDataOffset = largePayloadOutputOffset ;
    largePayloadOutputOffset += headerDIMMBody->ConfRequestDataSize ;
  }

  // Update the Cfgout record if there is a new record
  if( ( cfgOutRecBuffer != NULL ) && ( cfgOutRecLength != 0 ) ) {

    //DEBUG(( EFI_D_ERROR, "Copy CfgOut record to large payload input region\n" )) ;

    // Copy CfgOut record to large payload input region
    status = CopyFromBufferToLargePayload(cfgOutRecBuffer, 0, skt, ch, dimm, largePayloadOutputOffset, cfgOutRecLength, LARGE_PAYLOAD_INPUT);
    ASSERT(status == EFI_SUCCESS);

    // Update CfgOut length parameter in DIMM configuration header since this is a new record
    headerDIMMBody->ConfResponseDataSize = cfgOutRecLength ;

    // Update CfgOut parameters in DIMM configuration header
    headerDIMMBody->ConfResponseDataOffset = largePayloadOutputOffset ;
    largePayloadOutputOffset += headerDIMMBody->ConfResponseDataSize ;
  }

  // Otherwise, then keep the existing CfgOut record if it exists
  else if( headerDIMMBody->ConfResponseDataSize ) {

    //DEBUG(( EFI_D_ERROR, "Copy existing CfgCur record to large payload input region\n" )) ;

    // Copy existing CfgOut record to large payload input region
    status = CopyLPOutputToLPInput( skt, ch, dimm, headerDIMMBody->ConfResponseDataOffset, skt, ch, dimm, largePayloadOutputOffset, headerDIMMBody->ConfResponseDataSize ) ;
    ASSERT(status == EFI_SUCCESS);

    // Update CfgCur parameters in DIMM configuration header
    headerDIMMBody->ConfResponseDataOffset = largePayloadOutputOffset ;
    largePayloadOutputOffset += headerDIMMBody->ConfResponseDataSize ;
  }

  // Compute checksum of header
  ComputeChecksum( buffer, lengthPlatformCfgHeaderBody, &(headerDIMMCfg->Checksum) ) ;

  //DEBUG(( EFI_D_ERROR, "Copy header record to large payload input region\n" )) ;

  // Copy header record to large payload input region
  status = CopyFromBufferToLargePayload(buffer, 0, skt, ch, dimm, 0, lengthPlatformCfgHeaderBody, LARGE_PAYLOAD_INPUT);
  ASSERT(status == EFI_SUCCESS);

  if (mSystemMemoryMap->cpuStepping < H0_REV_SKX) {
#ifdef __GNUC__
      asm("sfence");
#else
    _mm_sfence();
#endif
    status = CopyFromLargePayloadToBuffer(skt, ch, dimm, 0, mLargePayloadBuffer, 0, CR_BIOS_LARGE_PAYLOAD_SIZE, LARGE_PAYLOAD_INPUT);
  }

  // Write the platform configuration data
  status = SetPCD( skt, ch, dimm, OS_PARTITION ) ;
  ASSERT(status == EFI_SUCCESS);

  return( status ) ;
}

/*++

  Writes the data in the large payload region to the specified partiton on the NGN DIMM

  @param skt         - Socket number
  @param ch          - Channel number
  @param dimm        - DIMM number
  @param partitionId - Specified partition

  @retval TBD

++*/
EFI_STATUS SetPCD( UINT8 skt, UINT8 ch, UINT8 dimm, UINT8 partitionId )
{
  EFI_STATUS    status ;
  PAEP_DIMM     pDimm = NULL ;
  CR_MAILBOX    *pMBox ;
  //UINT8         *pByte ;

  //DEBUG(( EFI_D_ERROR, "SetPCD()\n" )) ;

  // Get the pointer to the NGN DIMM structure
  pDimm = GetDimm( skt, ch, dimm );

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  if(mUseOSMailbox) {
    pMBox = &pDimm->OSMailboxSPA;
  } else {
    // Use BIOS Mailbox
    pMBox = &pDimm->MailboxSPA;
  }

  if (mSystemMemoryMap->cpuStepping >= H0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);

    *(UINT64 *)(UINTN)pMBox->pInPayload[0] = partitionId ;
  } else {
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[0], partitionId, sizeof (UINT32));
  }


  //Send the command
  if ( GetMediaStatus(pDimm) == MEDIA_READY) {
    status = SendFnvCommand( pDimm, SET_PLATFORM_CONFIG ) ;
  } else {
    status = EFI_DEVICE_ERROR;
  }

  return( status ) ;

}

/*++

  Reads the data from the specified partiton on the NGN DIMM into the large payload region

  @param skt         - Socket number
  @param ch          - Channel number
  @param dimm        - DIMM number
  @param partitionId - Specified partition

  @retval TBD

++*/
EFI_STATUS GetPCD( UINT8 skt, UINT8 ch, UINT8 dimm, UINT8 partitionId )
{
  EFI_STATUS    status ;
  PAEP_DIMM     pDimm = NULL ;
  CR_MAILBOX    *pMBox ;

  //DEBUG(( EFI_D_ERROR, "GetPCD()\n" )) ;

  // Get the pointer to the NGN DIMM structure
  pDimm = GetDimm( skt, ch, dimm );

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  if(mUseOSMailbox) {
    pMBox = &pDimm->OSMailboxSPA;
  } else {
    // Use BIOS Mailbox
    pMBox = &pDimm->MailboxSPA;
  }

  if (mSystemMemoryMap->cpuStepping >= H0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);

    *(UINT64 *)(UINTN)pMBox->pInPayload[0] = partitionId ;
  } else {
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[0], partitionId, sizeof (UINT32));
  }

  // Send the command
  if ( GetMediaStatus(pDimm) == MEDIA_READY) {
    status = SendFnvCommand( pDimm, GET_PLATFORM_CONFIG ) ;
  } else {
    status = EFI_DEVICE_ERROR;
  }

  return( status ) ;
}


/*++

  Creates the CfgCur record from the HOB data.

  @param index           - Index to HOB record containing the CfgCur data
  @param cfgRecord       - Buffer allocated by function to store the CfgCur record
  @param cfgRecordLength - Length of allocated buffer

++*/
VOID CreateCfgCurRecordFromHOBData( UINT8 index, UINT8 **cfgRecord, UINT32 *cfgRecordLength )
{
  UINT8 i, j, k, indexInterleaveDIMM, *buffer ;
  UINT16 lengthCfgCur ;
  UINT16 bufferOffset ;
  NGN_DIMM_CFG_CUR_HOST *recordCfgCur ;
  NGN_DIMM_PLATFORM_CFG_HEADER *headerCfgCur ;
  NGN_DIMM_PLATFORM_CFG_CURRENT_BODY *bodyCfgCur ;
  NGN_PCAT_HEADER *headerPCAT ;
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT *interleaveInfoPCAT ;
  NGN_DIMM_INTERLEAVE_ID *interleaveInfoId ;
  EFI_STATUS status ;
  UINT64 TempOemTableId ;

  //DEBUG(( EFI_D_ERROR, "CreateCfgCurRecordFromHOBData()\n" )) ;

  // Create a pointer to the data for this record that was stored in the HOB
  recordCfgCur = &(mNGNCfgCurRecord->cfgCurRecord[index].cfgCurData) ;

  // Compute the size of the CfgCur record.  Size includes the header and body plus size for any associated interleaves
  lengthCfgCur = sizeof( NGN_DIMM_PLATFORM_CFG_HEADER ) + sizeof( NGN_DIMM_PLATFORM_CFG_CURRENT_BODY ) ;
  for( i = 0 ; i < MAX_UNIQUE_NGN_DIMM_INTERLEAVE ; i++ )  {

    if( ( recordCfgCur->interleave[i].RecPresent ) && ( recordCfgCur->interleave[i].Valid ) ) {

      lengthCfgCur += sizeof( NGN_PCAT_HEADER ) + sizeof( NGN_DIMM_INTERLEAVE_INFORMATION_PCAT ) + ( recordCfgCur->interleave[i].NumOfDimmsInInterleaveSet * sizeof( NGN_DIMM_INTERLEAVE_ID ) ) ;

    }
  }

  // Create a buffer for the record
  status = gBS->AllocatePool( EfiBootServicesData, lengthCfgCur * sizeof( UINT8 ), &buffer ) ;
  //DEBUG(( EFI_D_ERROR, "Allocate Pool Buffer @ 0x%X of Length %d\n", buffer, lengthCfgCur )) ;
  ASSERT_EFI_ERROR( status ) ;

  // Zero buffer
  ZeroMem( buffer, lengthCfgCur ) ;

  TempOemTableId  = PcdGet64(PcdAcpiDefaultOemTableId);

  // Create header in buffer
  headerCfgCur = ( NGN_DIMM_PLATFORM_CFG_HEADER * ) buffer ;
  headerCfgCur->Signature = NGN_DIMM_CURRENT_CONFIG_SIG ;
  headerCfgCur->Length = lengthCfgCur ;
  headerCfgCur->Revision = 1 ;
  CopyMem(headerCfgCur->OemId,          PcdGetPtr (PcdAcpiDefaultOemId), sizeof(headerCfgCur->OemId));
  CopyMem(&(headerCfgCur->OemTableId),  &TempOemTableId, sizeof(headerCfgCur->OemTableId));
  headerCfgCur->OemRevision = EFI_ACPI_OEM_REVISION;
  headerCfgCur->CreatorId = EFI_ACPI_CREATOR_ID;
  headerCfgCur->CreatorRevision = EFI_ACPI_CREATOR_REVISION;
  bufferOffset = sizeof( NGN_DIMM_PLATFORM_CFG_HEADER ) ;

  // Create body in buffer
  bodyCfgCur = ( NGN_DIMM_PLATFORM_CFG_CURRENT_BODY * ) ( buffer + bufferOffset ) ;
  bodyCfgCur->ConfigurationStatus = recordCfgCur->body.ConfigurationStatus ;
  bodyCfgCur->VolatileMemSizeIntoSpa = ( UINT64 ) recordCfgCur->body.VolatileMemSizeIntoSpa << 26 ;
  bodyCfgCur->PersistentMemSizeIntoSpa = ( UINT64 ) recordCfgCur->body.PersistentMemSizeIntoSpa << 26 ;
  bufferOffset += sizeof( NGN_DIMM_PLATFORM_CFG_CURRENT_BODY ) ;

  // Create interleave information tables
  for( i = 0 ; i < MAX_UNIQUE_NGN_DIMM_INTERLEAVE ; i++ )  {

    if( ( recordCfgCur->interleave[i].RecPresent ) && ( recordCfgCur->interleave[i].Valid ) ) {

      headerPCAT = ( NGN_PCAT_HEADER * ) ( buffer + bufferOffset ) ;
      headerPCAT->Type = NGN_PCAT_TYPE_DIMM_INTERLEAVE_INFORMATION ;
      headerPCAT->Length = sizeof( NGN_PCAT_HEADER ) + sizeof( NGN_DIMM_INTERLEAVE_INFORMATION_PCAT ) + ( recordCfgCur->interleave[i].NumOfDimmsInInterleaveSet * sizeof( NGN_DIMM_INTERLEAVE_ID ) ) ;
      bufferOffset += sizeof( NGN_PCAT_HEADER ) ;

      interleaveInfoPCAT = ( NGN_DIMM_INTERLEAVE_INFORMATION_PCAT * ) ( buffer + bufferOffset ) ;
      interleaveInfoPCAT->InterleaveSetIndex = recordCfgCur->interleave[i].InterleaveSetIndex ;
      interleaveInfoPCAT->NumOfDimmsInInterleaveSet = recordCfgCur->interleave[i].NumOfDimmsInInterleaveSet ;
      interleaveInfoPCAT->InterleaveMemoryType = recordCfgCur->interleave[i].InterleaveMemoryType ;
      interleaveInfoPCAT->InterleaveFormat = recordCfgCur->interleave[i].InterleaveFormat ;
      interleaveInfoPCAT->MirrorEnable = recordCfgCur->interleave[i].MirrorEnable ;
      interleaveInfoPCAT->InterleaveChangeResponseStatus = recordCfgCur->interleave[i].InterleaveChangeResponseStatus ;
      bufferOffset += sizeof( NGN_DIMM_INTERLEAVE_INFORMATION_PCAT ) ;

      for( j = 0 ; j < recordCfgCur->interleave[i].NumOfDimmsInInterleaveSet ; j++ )  {

        interleaveInfoId = ( NGN_DIMM_INTERLEAVE_ID * ) ( buffer + bufferOffset ) ;

        indexInterleaveDIMM = mNGNCfgCurRecordIndices[recordCfgCur->interleaveId[i][j].socket][recordCfgCur->interleaveId[i][j].channel][recordCfgCur->interleaveId[i][j].dimm] ;

        for( k = 0 ; k < NGN_MAX_MANUFACTURER_STRLEN ; k++ )  {
          interleaveInfoId->DimmManufacturerId[k] = mNGNCfgCurRecord->cfgCurRecord[indexInterleaveDIMM].manufacturer[k] ;
        }

        for( k = 0 ; k < NGN_MAX_SERIALNUMBER_STRLEN ; k++ )  {
          interleaveInfoId->DimmSerialNumber[k] = mNGNCfgCurRecord->cfgCurRecord[indexInterleaveDIMM].serialNumber[k] ;
        }

        for( k = 0 ; k < NGN_MAX_MODELNUMBER_STRLEN ; k++ ) {
           interleaveInfoId->DimmModelNumber[k] =  mNGNCfgCurRecord->cfgCurRecord[indexInterleaveDIMM].modelNumber[k] ;
        }

        interleaveInfoId->PartitionOffset = (UINT64) recordCfgCur->interleaveId[i][j].PartitionOffset << 26;
        interleaveInfoId->PartitionSize = (UINT64) recordCfgCur->interleaveId[i][j].PartitionSize << 26 ;
        bufferOffset += sizeof( NGN_DIMM_INTERLEAVE_ID ) ;
      }
    }
  }

  // Update checksum
  ComputeChecksum( buffer, lengthCfgCur, &(headerCfgCur->Checksum) ) ;

  // Update return values
  *cfgRecord = buffer ;
  *cfgRecordLength = lengthCfgCur ;
}

/*++

  Creates the CfgOut record from the HOB data.

  @param index           - Index to HOB record containing the CfgOut data
  @param cfgRecord       - Buffer allocated by function to store the CfgOut record
  @param cfgRecordLength - Length of allocated buffer

++*/
VOID CreateCfgOutRecordFromHOBData (UINT8 index, UINT8 **cfgRecord, UINT32 *cfgRecordLength)
{
  UINT8 i, *buffer;
  UINT8 skt, ch, dimm;
  UINT16 lengthCfgOut;
  UINT16 bufferOffset, tempBufferOffset;
  UINT16 lengthPlatformCfgHeaderBody;
  UINT32 confRequestDataOffset;
  UINT32 confRequestDataSize;
  UINT32 confRequestOffsetVariableBody;
  NGN_DIMM_CFG_OUT_HOST *recordCfgOut;
  NGN_DIMM_PLATFORM_CFG_HEADER *headerCfgOut;
  NGN_DIMM_PLATFORM_CFG_OUTPUT_BODY *bodyCfgOut;
  NGN_PCAT_HEADER *headerPCAT;
  NGN_DIMM_PARTITION_SIZE_CHANGE_PCAT *partitionSizePCAT;
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT *interleaveInfoPCAT;
  NGN_DIMM_PLATFORM_CFG_HEADER *headerDIMMCfg;
  NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY *headerDIMMBody;
  EFI_STATUS status;
  UINT64 TempOemTableId ;

  //DEBUG ((EFI_D_ERROR, "CreateCfgOutRecordFromHOBData()\n"));

  // Create local variables to reference DIMM
  skt = mNGNCfgOutRecord->cfgOutRecord[index].socket;
  ch = mNGNCfgOutRecord->cfgOutRecord[index].channel;
  dimm = mNGNCfgOutRecord->cfgOutRecord[index].dimm;

  // Create a buffer for the record
  lengthPlatformCfgHeaderBody = sizeof (NGN_DIMM_PLATFORM_CFG_HEADER) + sizeof (NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY);
  status = gBS->AllocatePool (EfiBootServicesData, lengthPlatformCfgHeaderBody, &buffer);
  //DEBUG ((EFI_D_ERROR, "Allocate Pool Buffer @ 0x%X of Length %d\n", buffer, lengthPlatformCfgHeaderBody));
  ASSERT_EFI_ERROR (status);

  // Zero buffer
  ZeroMem (buffer, lengthPlatformCfgHeaderBody);

  // Copy DIMM configuration header from large payload region to local buffer
  status = CopyFromLargePayloadToBuffer (skt, ch, dimm, 0, buffer, 0, lengthPlatformCfgHeaderBody, LARGE_PAYLOAD_OUTPUT);
  ASSERT (status == EFI_SUCCESS);

  // Set pointers to header and body
  headerDIMMCfg = (NGN_DIMM_PLATFORM_CFG_HEADER *) buffer;
  headerDIMMBody = (NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY *) (buffer + sizeof (NGN_DIMM_PLATFORM_CFG_HEADER));

  // Determine offset and size of configuration input record
  confRequestDataOffset = headerDIMMBody->ConfRequestDataOffset;
  confRequestDataSize = headerDIMMBody->ConfRequestDataSize;

  // Free memory used for header
  gBS->FreePool (headerDIMMCfg);

  // Create a pointer to the data for this record that was stored in the HOB
  recordCfgOut = &(mNGNCfgOutRecord->cfgOutRecord[index].cfgOutData);

  // Compute the size of the CfgOut record.  Size includes the header and body plus size for any associated partition request and interleaves that need to be copied form the configuration input table
  lengthCfgOut = (UINT16) confRequestDataSize + sizeof (NGN_DIMM_PLATFORM_CFG_OUTPUT_BODY) - sizeof (NGN_DIMM_PLATFORM_CFG_INPUT_BODY);

  // Create a buffer for the record
  status = gBS->AllocatePool (EfiBootServicesData, lengthCfgOut * sizeof (UINT8), &buffer);
  //DEBUG ((EFI_D_ERROR, "Allocate Pool Buffer @ 0x%X of Length %d\n", buffer, lengthCfgOut));
  ASSERT_EFI_ERROR (status);

  // Zero buffer
  ZeroMem (buffer, lengthCfgOut);

  TempOemTableId  = PcdGet64(PcdAcpiDefaultOemTableId);

  // Create header in buffer
  headerCfgOut = (NGN_DIMM_PLATFORM_CFG_HEADER *) buffer;
  headerCfgOut->Signature = NGN_DIMM_OUTPUT_CONFIG_SIG;
  headerCfgOut->Length = lengthCfgOut;
  headerCfgOut->Revision = 1;
  CopyMem(headerCfgOut->OemId,          PcdGetPtr (PcdAcpiDefaultOemId), sizeof(headerCfgOut->OemId));
  CopyMem(&(headerCfgOut->OemTableId),  &TempOemTableId, sizeof(headerCfgOut->OemTableId));
  headerCfgOut->OemRevision = EFI_ACPI_OEM_REVISION;
  headerCfgOut->CreatorId = EFI_ACPI_CREATOR_ID;
  headerCfgOut->CreatorRevision = EFI_ACPI_CREATOR_REVISION;
  bufferOffset = sizeof (NGN_DIMM_PLATFORM_CFG_HEADER);

  // Create body in buffer
  bodyCfgOut = (NGN_DIMM_PLATFORM_CFG_OUTPUT_BODY *) (buffer + bufferOffset);
  bodyCfgOut->SequenceNumber = recordCfgOut->body.SequenceNumber;
  bodyCfgOut->ValidationStatus = recordCfgOut->body.ValidationStatus;
  bufferOffset += sizeof (NGN_DIMM_PLATFORM_CFG_OUTPUT_BODY);


  // Copy DIMM partition size change and DIMM interleave information tables from configuration input record to configuration output record
  confRequestOffsetVariableBody = sizeof (NGN_DIMM_PLATFORM_CFG_HEADER) + sizeof (NGN_DIMM_PLATFORM_CFG_INPUT_BODY);
  if (confRequestDataSize > confRequestOffsetVariableBody) {
    status = CopyFromLargePayloadToBuffer (skt, ch, dimm, confRequestDataOffset + confRequestOffsetVariableBody, buffer + bufferOffset, 0, confRequestDataSize - confRequestOffsetVariableBody, LARGE_PAYLOAD_OUTPUT) ;
    ASSERT (status == EFI_SUCCESS);


    // Update partition table
    if (recordCfgOut->partitionSize.RecPresent) {

      // Set temporary pointer to start of PCAT structures
      tempBufferOffset = bufferOffset;

      // Find DIMM partition size change table
      headerPCAT = (NGN_PCAT_HEADER *) (buffer + tempBufferOffset);
      while (headerPCAT->Type != NGN_PCAT_TYPE_DIMM_PARTITION_SIZE_CHANGE) {
        tempBufferOffset += headerPCAT->Length;
        headerPCAT = (NGN_PCAT_HEADER *) (buffer + tempBufferOffset);
      }

      // Update the partition size response status
      partitionSizePCAT = (NGN_DIMM_PARTITION_SIZE_CHANGE_PCAT *) (buffer + tempBufferOffset + sizeof (NGN_PCAT_HEADER));
      partitionSizePCAT->ResponseStatus = recordCfgOut->partitionSize.ResponseStatus;
    }


    // Update interleave information tables
    for (i = 0, tempBufferOffset = bufferOffset; i < MAX_UNIQUE_NGN_DIMM_INTERLEAVE; i++)  {

      if (recordCfgOut->interleave[i].RecPresent) {

        // Find DIMM intereleave information table
        headerPCAT = (NGN_PCAT_HEADER *) (buffer + tempBufferOffset);

        while (headerPCAT->Type != NGN_PCAT_TYPE_DIMM_INTERLEAVE_INFORMATION) {
          tempBufferOffset += headerPCAT->Length ;
          headerPCAT = (NGN_PCAT_HEADER *) (buffer + tempBufferOffset);
        }

        // Update the interleave change response status
        interleaveInfoPCAT = (NGN_DIMM_INTERLEAVE_INFORMATION_PCAT *) (buffer + tempBufferOffset + sizeof (NGN_PCAT_HEADER)) ;
        interleaveInfoPCAT->InterleaveChangeResponseStatus = recordCfgOut->interleave[i].InterleaveChangeResponseStatus;

        // Advance to next PCAT header
        tempBufferOffset += headerPCAT->Length;
      }
    }

    // Update checksum
    ComputeChecksum (buffer, lengthCfgOut, &(headerCfgOut->Checksum));

    // Update return values
    *cfgRecord = buffer;
    *cfgRecordLength = lengthCfgOut;
  } else {
    *cfgRecord = NULL;
    *cfgRecordLength = 0;
  }
}

/*++

  Load HOB data for updating the platform configuration data for each NGN DIMM and populate the record indicies.

  @retval EFI_SUCCESS           - Success

++*/
EFI_STATUS LoadHOBsForPCDUpdates( VOID )
{
  EFI_HOB_GUID_TYPE    *GuidHobNGNCurRecords ;
  EFI_HOB_GUID_TYPE    *GuidHobNGNOutRecords ;
  UINT8                i, j, k ;

  //DEBUG(( EFI_D_ERROR, "LoadHOBsForPCDUpdates()\n" )) ;

  // Initialize the indicies arrays to not populated
  for( i = 0 ; i < MAX_SOCKET ; i++ )  {
    for( j = 0 ; j < MAX_CH ; j++ )  {
      for( k = 0 ; k < MAX_DIMM ; k++ )  {

        mNGNCfgCurRecordIndices[i][j][k] = NGN_NOT_POPULATED ;
        mNGNCfgOutRecordIndices[i][j][k] = NGN_NOT_POPULATED ;

      }
    }
  }

  // Load the HOB containing the CfgCur records
  GuidHobNGNCurRecords = GetFirstGuidHob(&gEfiNGNCfgCurGuid);
  if(GuidHobNGNCurRecords != NULL) {
    mNGNCfgCurRecord = GET_GUID_HOB_DATA(GuidHobNGNCurRecords);

    // Update the array indicies for the CfgCur records
    for( i = 0 ; i < mNGNCfgCurRecord->numCfgCurRecords ; i++ )  {

      //DEBUG((EFI_D_ERROR, "Found Entry Sckt = %d, Ch = %d, Dimm = %d\n",  mNGNCfgCurRecord->cfgCurRecord[i].socket, mNGNCfgCurRecord->cfgCurRecord[i].channel, mNGNCfgCurRecord->cfgCurRecord[i].dimm ));
      mNGNCfgCurRecordIndices[mNGNCfgCurRecord->cfgCurRecord[i].socket][mNGNCfgCurRecord->cfgCurRecord[i].channel][mNGNCfgCurRecord->cfgCurRecord[i].dimm] = i ;

    }
    //DEBUG((EFI_D_ERROR, "numCfgCurRecords = %d\n", mNGNCfgCurRecord->numCfgCurRecords));
  }

  // Load the HOB containing the CfgOut records
  GuidHobNGNOutRecords = GetFirstGuidHob(&gEfiNGNCfgOutGuid);
  if(GuidHobNGNCurRecords != NULL) {
    mNGNCfgOutRecord = GET_GUID_HOB_DATA(GuidHobNGNOutRecords);

    // Update the array indicies for the CfgOut records
    for( i = 0 ; i < mNGNCfgOutRecord->numCfgOutRecords ; i++ )  {

      //DEBUG((EFI_D_ERROR, "Found Entry Sckt = %d, Ch = %d, Dimm = %d\n",  mNGNCfgOutRecord->cfgOutRecord[i].socket, mNGNCfgOutRecord->cfgOutRecord[i].channel, mNGNCfgOutRecord->cfgOutRecord[i].dimm ));
      mNGNCfgOutRecordIndices[mNGNCfgOutRecord->cfgOutRecord[i].socket][mNGNCfgOutRecord->cfgOutRecord[i].channel][mNGNCfgOutRecord->cfgOutRecord[i].dimm] = i ;

    }
    //DEBUG((EFI_D_ERROR, "numCfgOutRecords = %d\n", mNGNCfgOutRecord->numCfgOutRecords));
  }

  return( EFI_SUCCESS ) ;
}

/*++

  Copy data from the SMM large payload output region to a local buffer

  @param srcSkt         - Source socket
  @param srcCh          - Source channel
  @param srcDimm        - Source dimm
  @param srcOffset      - Source offset in the large payload region
  @param dstBuffer      - Destination pointer to local buffer
  @param dstOffset      - Destination offset in the local buffer
  @param length         - Number of bytes to copy
  @param largePayloadRegion - If is = LARGE_PAYLOAD_INPUT, copies from Input Payload Region, if is = LARGE_PAYLOAD_OUTPUT copies from Output Payload Region

  @retval EFI_NOT_FOUND         - DIMM not found
  @retval EFI_SUCCESS           - Success

++*/
EFI_STATUS CopyFromLargePayloadToBuffer(UINT8 srcSkt, UINT8 srcCh, UINT8 srcDimm, UINT32 srcOffset, UINT8 *dstBuffer, UINT32 dstOffset, UINT32 length, BOOLEAN largePayloadRegion)
{
  UINT32  chIntSize ;
  UINT32  numBytesFirstPartialLine ;
  UINT32  numBytesToCopyFirstPartialLine ;
  UINT32  numFullLinesToCopy ;
  UINT32  numBytesToCopyLastPartialLine ;
  UINT32  dstBufferOffset ;
  UINT32  i ;
  PAEP_DIMM pDimm = NULL;
  UINT64  srcDpaBase ;
  UINT64  spa ;
  EFI_STATUS  status;

  //DEBUG(( DEBUG_INFO, "CopyFromLargePayloadToBuffer()\n" )) ;

  // Get the PAEP_DIMM struct for the DIMM
  pDimm = GetDimm( srcSkt, srcCh, srcDimm );

  // Unable to find DIMM
  if( pDimm == NULL ) {
    return( EFI_NOT_FOUND ) ;
  }

  // Verify length
  if( length == 0 )  {
    return( EFI_SUCCESS ) ;
  }

  // Determine interleave parameters for the large payload region
  chIntSize = mCrInfo.ChGran ;

  // Determine number of bytes on first line beginning at offset
  numBytesFirstPartialLine = chIntSize - ( srcOffset % chIntSize ) ;

  // Compute transfer sizes
  if( length <= numBytesFirstPartialLine )  {
    numBytesToCopyFirstPartialLine = length ;
  }
  else  {
    numBytesToCopyFirstPartialLine = numBytesFirstPartialLine ;
  }
  numFullLinesToCopy = ( length - numBytesToCopyFirstPartialLine ) / chIntSize ;
  numBytesToCopyLastPartialLine = ( length - numBytesToCopyFirstPartialLine - ( numFullLinesToCopy * chIntSize ) ) % chIntSize ;

  // Set the DPA of the specified payload region
  if( largePayloadRegion == LARGE_PAYLOAD_INPUT ) {
    if( mUseOSMailbox )
      srcDpaBase = CR_OS_MB_LARGE_INPAYLOAD;
    else
      srcDpaBase = CR_BIOS_MB_LARGE_INPAYLOAD ;
  }
  else {
    if( mUseOSMailbox )
      srcDpaBase = CR_OS_MB_LARGE_OUTPAYLOAD ;
    else
      srcDpaBase = CR_BIOS_MB_LARGE_OUTPAYLOAD;
  }

  // Initialize the buffer offset
  dstBufferOffset = dstOffset ;

  // Copy first partial line
  if( numBytesToCopyFirstPartialLine )  {

    // Compute SPA of lineNumber
    status = XlateDpaOffsetToSpa( pDimm, pDimm->CntrlBase, (  srcDpaBase + srcOffset ), &spa ) ;
    ASSERT(status == EFI_SUCCESS);

    //DEBUG(( EFI_D_ERROR, "Translated Address is SPA = 0x%lX\n", spa )) ;
    //DEBUG(( EFI_D_ERROR, "Calling CopyMem ( 0x%lX, 0x%lX, 0x%x )\n", ( dstBuffer + dstBufferOffset ), spa, numBytesToCopyFirstPartialLine )) ;

    // Perform copy operation
    CopyMem ((UINT8 *) (dstBuffer + dstBufferOffset ), (UINT8 * ) spa, numBytesToCopyFirstPartialLine) ;

    // Update srcOffset and dstBufferOffset
    srcOffset += numBytesToCopyFirstPartialLine ;
    dstBufferOffset += numBytesToCopyFirstPartialLine ;
  }

  // Copy full lines
  if( numFullLinesToCopy )  {

    for( i = 0 ; i < numFullLinesToCopy ; i++ )  {

      // Compute SPA of lineNumber
      status = XlateDpaOffsetToSpa( pDimm, pDimm->CntrlBase, (  srcDpaBase + srcOffset ), &spa ) ;
      ASSERT(status == EFI_SUCCESS);

      //DEBUG(( EFI_D_ERROR, "Translated Address is SPA = 0x%lX\n", spa )) ;
      //DEBUG(( EFI_D_ERROR, "Calling CopyMem ( 0x%lX, 0x%lX, 0x%x )\n", ( dstBuffer + dstBufferOffset ), spa, chIntSize )) ;

      // Perform copy operation
      CopyMem ((UINT8 *) (dstBuffer + dstBufferOffset), (UINT8 *) spa, chIntSize) ;

      // Update srcOffset and dstBufferOffset
      srcOffset += chIntSize ;
      dstBufferOffset += chIntSize ;
    }
  }

  // Copy last partial line
  if( numBytesToCopyLastPartialLine )  {

    // Compute SPA of lineNumber
    status = XlateDpaOffsetToSpa( pDimm, pDimm->CntrlBase, (  srcDpaBase + srcOffset ), &spa ) ;
    ASSERT(status == EFI_SUCCESS);

    //DEBUG(( EFI_D_ERROR, "Translated Address is SPA = 0x%lX\n", spa )) ;
    //DEBUG(( EFI_D_ERROR, "Calling CopyMem (0x%lX, 0x%lX, 0x%x)\n", ( dstBuffer + dstBufferOffset ), spa, numBytesToCopyLastPartialLine )) ;

    // Perform copy operation
    CopyMem ((UINT8 *) (dstBuffer + dstBufferOffset), (UINT8 *) spa, numBytesToCopyLastPartialLine) ;
  }

  return( EFI_SUCCESS ) ;
}

/*++

  Copy data from a local buffer to SMM large payload region

  @param srcBuffer      - Source pointer to local buffer
  @param srcOffset      - Source offset in the local buffer
  @param dstSkt         - Destination socket
  @param dstCh          - Destination channel
  @param dstDimm        - Destination dimm
  @param dstOffset      - Destination offset in the large payload region
  @param length         - Number of bytes to copy
  @param largePayloadRegion  - If it is = LARGE_PAYLOAD_INPUT, copies to the Large Input Payload, else if = LARGE_PAYLOAD_OUTPUT, copies buffer
                          to the Large Output Payload

  @retval EFI_NOT_FOUND         - DIMM not found
  @retval EFI_SUCCESS           - Success

++*/
EFI_STATUS CopyFromBufferToLargePayload(UINT8 *srcBuffer, UINT32 srcOffset, UINT8 dstSkt, UINT8 dstCh, UINT8 dstDimm, UINT32 dstOffset, UINT32 length, BOOLEAN largePayloadRegion)
{
  UINT32  chIntSize ;
  UINT32  numBytesFirstPartialLine ;
  UINT32  numBytesToCopyFirstPartialLine ;
  UINT32  numFullLinesToCopy ;
  UINT32  numBytesToCopyLastPartialLine ;
  UINT32  srcBufferOffset ;
  UINT32  i ;
  PAEP_DIMM pDimm = NULL;
  UINT64  dstDpaBase ;
  UINT64  spa ;
  EFI_STATUS  status ;

  //DEBUG(( EFI_D_ERROR, "CopyBufferToLPInput()\n" )) ;

  // Get the PAEP_DIMM struct for the DIMM
  //DEBUG(( EFI_D_ERROR, "Calling GetDimm( %d, %d, %d)\n", dstSkt, dstCh, dstDimm )) ;
  pDimm = GetDimm( dstSkt, dstCh, dstDimm );

  // Unable to find DIMM
  if( pDimm == NULL ) {
    //DEBUG(( EFI_D_ERROR, "Unable to find DIMM\n" )) ;
    return( EFI_NOT_FOUND ) ;
  }

  // Verify length
  if( length == 0 )  {
    //DEBUG(( EFI_D_ERROR, "Length is zero\n" )) ;
    return( EFI_SUCCESS ) ;
  }

  // Determine interleave parameters for the large payload region
  chIntSize = mCrInfo.ChGran ;

  // Determine number of bytes on first line beginning at offset
  numBytesFirstPartialLine = chIntSize - ( dstOffset % chIntSize ) ;

  // Compute transfer sizes
  if( length <= numBytesFirstPartialLine )  {
    numBytesToCopyFirstPartialLine = length ;
  }
  else  {
    numBytesToCopyFirstPartialLine = numBytesFirstPartialLine ;
  }
  numFullLinesToCopy = ( length - numBytesToCopyFirstPartialLine ) / chIntSize ;
  numBytesToCopyLastPartialLine = ( length - numBytesToCopyFirstPartialLine - ( numFullLinesToCopy * chIntSize ) ) % chIntSize ;

  // Set the DPA of the specified payload region
  if( largePayloadRegion == LARGE_PAYLOAD_INPUT ) {
    if( mUseOSMailbox )
      dstDpaBase = CR_OS_MB_LARGE_INPAYLOAD;
    else
      dstDpaBase = CR_BIOS_MB_LARGE_INPAYLOAD;
  }
  else {
    // We believe we cannot copy anything to the Large Payload Output Region,
    // return error status
    return (EFI_UNSUPPORTED);
  }

  // Initialize the buffer offset
  srcBufferOffset = srcOffset ;

  // Copy first partial line
  if( numBytesToCopyFirstPartialLine )  {

    // Compute SPA
    status = XlateDpaOffsetToSpa( pDimm, pDimm->CntrlBase, ( dstDpaBase + dstOffset ), &spa ) ;
    ASSERT(status == EFI_SUCCESS);

    //DEBUG(( EFI_D_ERROR, "Translated Address is SPA = 0x%lX\n", spa )) ;
    //DEBUG(( EFI_D_ERROR, "Calling CopyMem (0x%lX, 0x%lX, 0x%x )\n", spa, ( srcBuffer + srcBufferOffset ), numBytesToCopyFirstPartialLine )) ;

    // Perform copy operation
    CopyMem ((UINT8 *) spa, (UINT8 *) srcBuffer + srcBufferOffset, numBytesToCopyFirstPartialLine);

    // Update dstOffset and srcBufferOffset
    dstOffset += numBytesToCopyFirstPartialLine ;
    srcBufferOffset += numBytesToCopyFirstPartialLine ;
  }

  // Copy full lines
  if( numFullLinesToCopy )  {

    for( i = 0 ; i < numFullLinesToCopy ; i++ )  {

      // Compute SPA of lineNumber
      status = XlateDpaOffsetToSpa( pDimm, pDimm->CntrlBase, ( dstDpaBase + dstOffset ), &spa ) ;
      ASSERT(status == EFI_SUCCESS);

      //DEBUG(( EFI_D_ERROR, "Translated Address is SPA = 0x%lX\n", spa )) ;
      //DEBUG(( EFI_D_ERROR, "Calling CopyMem (0x%lX, 0x%lX, 0x%x )\n", spa, ( srcBuffer + srcBufferOffset ), chIntSize )) ;

      // Perform copy operation
      CopyMem ((UINT8 *)spa, (UINT8 *) (srcBuffer + srcBufferOffset), chIntSize);

      // Update dstOffset and srcBufferOffset
      dstOffset += chIntSize ;
      srcBufferOffset += chIntSize ;
    }
  }

  // Copy last partial line
  if( numBytesToCopyLastPartialLine )  {

    // Compute SPA of lineNumber
    status = XlateDpaOffsetToSpa( pDimm, pDimm->CntrlBase, ( dstDpaBase + dstOffset ), &spa ) ;
    ASSERT(status == EFI_SUCCESS);

    //DEBUG(( EFI_D_ERROR, "Translated Address is SPA = 0x%lX\n", spa )) ;
    //DEBUG(( EFI_D_ERROR, "Calling CopyMem (0x%lX, 0x%lX, 0x%x )\n", spa, ( srcBuffer + srcBufferOffset ), numBytesToCopyLastPartialLine )) ;

    // Perform copy operation
    CopyMem ((UINT8 *)spa, (UINT8 *) (srcBuffer + srcBufferOffset), numBytesToCopyLastPartialLine) ;

  }
  return( EFI_SUCCESS ) ;
}

/*++

  Copy data from SMM large payload region to SMM large payload region

  @param srcSkt       - Source socket
  @param srcCh        - Source channel
  @param srcDimm      - Source dimm
  @param srcOffset    - Source offset in the large payload region
  @param dstSkt       - Destination socket
  @param dstCh        - Destination channel
  @param dstDimm      - Destination dimm
  @param dstOffset    - Destination offset in the large payload region
  @param length       - Number of bytes to copy

  @retval EFI_NOT_FOUND         - DIMM not found
  @retval EFI_SUCCESS           - Success

++*/
EFI_STATUS CopyLPOutputToLPInput( UINT8 srcSkt, UINT8 srcCh, UINT8 srcDimm, UINT32 srcOffset, UINT8 dstSkt, UINT8 dstCh, UINT8 dstDimm, UINT32 dstOffset, UINT32 length )
{
  UINT8 buffer[NGN_INT_BUFFER_SIZE];
  UINT32 remBytesToCopy ;
  UINT32 numBytesToCopy ;
  EFI_STATUS status ;

  //DEBUG(( EFI_D_ERROR, "CopyLPOutputToLPInput()\n" )) ;

  remBytesToCopy = length ;

  while( remBytesToCopy )  {

    if( remBytesToCopy > NGN_INT_BUFFER_SIZE )  {
      numBytesToCopy = NGN_INT_BUFFER_SIZE ;
    }
    else  {
      numBytesToCopy = length ;
    }

    // Copy source to internal buffer
    status = CopyFromLargePayloadToBuffer(srcSkt, srcCh, srcDimm, srcOffset, buffer, 0, numBytesToCopy, LARGE_PAYLOAD_OUTPUT);
    if( status != EFI_SUCCESS )  {
      return( status ) ;
    }

    // Copy internal buffer to destination
    status = CopyFromBufferToLargePayload(buffer, 0, dstSkt, dstCh, dstDimm, dstOffset, numBytesToCopy, LARGE_PAYLOAD_INPUT);
    if( status != EFI_SUCCESS )  {
      return( status ) ;
    }

    remBytesToCopy = remBytesToCopy - numBytesToCopy ;
  }

  return( EFI_SUCCESS ) ;
}

/*++

  Compute byte checksum of a buffer

  @param srcBuffer    - Pointer to local buffer
  @param length       - Number of bytes to checksum
  @param chksum       - Computed checksum

  @retval EFI_INVALID_PARAMETER - Invalid parameter
  @retval EFI_SUCCESS           - Success

++*/
EFI_STATUS ComputeChecksum( UINT8 *srcBuffer, UINT32 length, UINT8 *chksum )
{
  UINT8 computedChksum = 0 ;

  //DEBUG(( EFI_D_ERROR, "ComputeChecksum()\n" )) ;

  if( ( srcBuffer == NULL ) || ( length == 0 ) ) {
    return( EFI_INVALID_PARAMETER ) ;
  }

  while( length-- ) {
    computedChksum += (UINT8) *srcBuffer++ ;
  }

  computedChksum = ~(computedChksum) + 1 ;

  *chksum = computedChksum ;

  return( EFI_SUCCESS ) ;
}

/*++

  Displays a buffer of a specified length

  @param buffer       - Pointer to local buffer
  @param length       - Number of bytes to display

  @retval EFI_SUCCESS           - Success

++*/
EFI_STATUS DisplayBuffer( UINT8 *buffer, UINT32 length )
{
  UINT32 i ;

  for( i = 0 ; i < length ; i++ )  {

    DEBUG(( EFI_D_ERROR, "%02X ", *buffer++ )) ;
    if( ( ( ( i + 1 ) % 16 ) == 0 ) && ( i != 0 ) )  {
      DEBUG(( EFI_D_ERROR, "\n" )) ;
    }
  }
  DEBUG(( EFI_D_ERROR, "\n" )) ;

  return( EFI_SUCCESS ) ;
}

/*++
  clear the LP input buffer on when using the LP input buffer for BIOS calls to update the PCD

  @param skt             - Socket number
  @param ch              - Channel number
  @param dimm            - DIMM number

  @retval EFI_NOT_FOUND         - DIMM not found
  @retval EFI_SUCCESS           - Success

++*/

EFI_STATUS ZeroLargePayloadInput(UINT8 skt, UINT8 ch, UINT8 dimm ) {

  UINT32  i ;
  PAEP_DIMM pDimm = NULL;
  UINT64  dstDpaBase;
  UINT64  spa ;
  EFI_STATUS  status ;
  UINT32 chIntSize;
  UINT32 IncGrn = 0;

  pDimm = GetDimm( skt, ch, dimm );

  // Unable to find DIMM
  if( pDimm == NULL ) {
    //DEBUG(( EFI_D_ERROR, "Unable to find DIMM\n" )) ;
    return( EFI_NOT_FOUND ) ;
  }

  // Determine interleave parameters for the large payload region
  chIntSize = mCrInfo.ChGran ;
  dstDpaBase = (mUseOSMailbox) ? CR_OS_MB_LARGE_INPAYLOAD : CR_BIOS_MB_LARGE_INPAYLOAD ;

  // zero out the large payload input before using it.
  for ( i = 0; i < CR_BIOS_LARGE_PAYLOAD_SIZE/chIntSize; i++) {
    status = XlateDpaOffsetToSpa( pDimm, pDimm->CntrlBase, (dstDpaBase + IncGrn), &spa ) ;
    ASSERT(status == EFI_SUCCESS);
    IncGrn += chIntSize;
    ZeroMem( ( UINT8 * ) spa, chIntSize) ;
  }
  return (EFI_SUCCESS);
}

