/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  Pcat.c

@brief:

  Access to ACPI PCAT (Platform Configuration Attributes Table)

**/

#include "Pcat.h"
#include "CrystalRidge.h"
#include "Nfit.h"
#include <Protocol/PcatProtocol.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <UncoreCommonIncludes.h>
#include <Library/UefiBootServicesTableLib.h>


extern struct SystemMemoryMapHob       *mSystemMemoryMap;
extern EFI_CPU_CSR_ACCESS_PROTOCOL     *mCpuCsrAccess;


/// @brief Calculate a new address, basing on some address and a byte offset
#define PTR_OFFSET(PTR, OFFSET) ((VOID *) ((UINT8 *) PTR + OFFSET))

/// @brief GUID for SSKU Attribute Extension Table
static const EFI_GUID SSKU_ATTRIBUTE_EXTENSION_TABLE  = {
  0xf93032e5,
  0xb045,
  0x40ef,
  { 0x91, 0xc8, 0xf0, 0x2b, 0x6, 0xad, 0x94, 0x8d }
};

/// @brief Address of memory allocated for PCAT table
PNVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE mPcatTablePtr = NULL;
/// @brief Max size of PCAT table (size of allocated memory)
UINT64 mPcatTableSize = 0;

/// @brief List of protocol functions
EFI_ACPI_PCAT_PROTOCOL mAcpiPcat;


/**
  @brief Check if PCAT is already initialized

  PCAT is initialized when:
  -memory buffer for PCAT is allocated
  -allocated memory buffer size is non-zero
  -table header contains valid PCAT signature

  @param        None

  @retval TRUE  PCAT already initialized
  @retval FALSE PCAT not initialized
*/
BOOLEAN
IsInitialized (
  VOID
  )
{
  if (NULL == mPcatTablePtr) {
    return FALSE;
  }

  if (0 == mPcatTableSize) {
    return FALSE;
  }

  if (mPcatTablePtr->Signature != NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE_SIGNATURE) {
    return FALSE;
  }

  return TRUE;
}

/**

This routine adds Platform Capabilities Information Table to
PCAT.

  @param void

  @retval VOID        - Nothing

**/
VOID
AddPlatCapInfoTableToPCAT(
   VOID
   )
{
  UINT64                     PcatAppendPtr;
  PPLAT_CAPABILITIES_INFO    pPlatCapInfo;
  CAPID5_PCU_FUN3_STRUCT     Capid5;
  UINT8                      SktIndex;

  // Get the current end of PCAT so we can append the Platform Capabilities Info Table
  PcatAppendPtr = (UINT64)mPcatTablePtr + mPcatTablePtr->Length;
  pPlatCapInfo = (PPLAT_CAPABILITIES_INFO)PcatAppendPtr;
  pPlatCapInfo->Type = PLAT_CAP_INFO_TABLE_TYPE;
  pPlatCapInfo->Length = sizeof(PLAT_CAPABILITIES_INFO);

  // BIOS doesn't support changing config thru Mgmt S/W and doesn't support RT Mgmt change request
  (mSystemMemoryMap->MemSetup.dfxMemSetup.dfxDimmManagement == DIMM_MGMT_CR_MGMT_DRIVER) ? (pPlatCapInfo->CRMgmtSWConfigInput = 1) : (pPlatCapInfo->CRMgmtSWConfigInput = 0);

  // BIOS supports the follwoing modes including SNC
  pPlatCapInfo->MemModeCap = PLAT_CAP_MEM_MODE_CAP_1LM | PLAT_CAP_MEM_MODE_CAP_2LM | PLAT_CAP_MEM_MODE_CAP_PMEM |PLAT_CAP_MEM_MODE_CAP_BLK | PLAT_CAP_MEM_MODE_CAP_SNC;

  switch( mSystemMemoryMap->MemSetup.volMemMode )  {
    case VOL_MEM_MODE_1LM:
      pPlatCapInfo->CurrentMemMode = PLAT_CAP_CURRENT_VOL_MEM_MODE_1LM;
      break;

    case VOL_MEM_MODE_2LM:
      pPlatCapInfo->CurrentMemMode = PLAT_CAP_CURRENT_VOL_MEM_MODE_2LM;
      break;

    case VOL_MEM_MODE_AUTO:
      pPlatCapInfo->CurrentMemMode = PLAT_CAP_CURRENT_VOL_MEM_MODE_AUTO;
      break;

  }

  if ((mSystemMemoryMap->MemSetup.dfxMemSetup.dfxPerMemMode == PER_MEM_MODE) || (mSystemMemoryMap->MemSetup.dfxMemSetup.dfxDimmManagement == DIMM_MGMT_CR_MGMT_DRIVER) )
    pPlatCapInfo->CurrentMemMode |= (PLAT_CAP_CURRENT_PER_MEM_MODE_PMEM << 2);
  else
    pPlatCapInfo->CurrentMemMode |= PLAT_CAP_CURRENT_PER_MEM_MODE_DISABLED;

  if( mSystemMemoryMap->sncEnabled == 1 ) {
    pPlatCapInfo->CurrentMemMode |= PLAT_CAP_CURRENT_MEM_MODE_SNC ;
  }


  // SetPMEMMirrorCap based on mmcapability
  pPlatCapInfo->PMEMMirrorCap = TRUE;
  for (SktIndex = 0; SktIndex < MAX_SOCKET; SktIndex++) {
    if (mSystemMemoryMap->Socket[SktIndex].SAD[0].Enable) {
      Capid5.Data = mCpuCsrAccess->ReadCpuCsr(SktIndex, 0, CAPID5_PCU_FUN3_REG);
      pPlatCapInfo->PMEMMirrorCap &= Capid5.Bits.cap_mirror_ddrt_en;
    }
  }

  //
  // Update the size of the PCAT to include this table
  mPcatTablePtr->Length += sizeof(PLAT_CAPABILITIES_INFO);

  DEBUG(( EFI_D_ERROR, "PCAT Table Dump:\n" )) ;
  DisplayBuffer( (UINT8 *) PcatAppendPtr, sizeof( PLAT_CAPABILITIES_INFO ) ) ;
}

UINT8
GetNumSockets(
   VOID
   )
{
  UINT8            NumSocket = 0;
  UINT8            SktIndex;

  for (SktIndex = 0; SktIndex < MAX_SOCKET; SktIndex++) {
    if (mSystemMemoryMap->Socket[SktIndex].SAD[0].Enable)
      NumSocket++;
  }

  return (NumSocket);
}

VOID
AddInterleaveCapabilitiesInfoTable(
   VOID
   )
{
  UINT64                     PcatAppendPtr;
  PINTERLEAVE_CAPABILITIES   pIntCap;
  UINT8                      PlatformMaxSocket, PlatformMaxChannelsPerSocket, PlatformMaxDimm;

  // Get the current end of PCAT so we can append the Interleave Capabilities Info Table
  PcatAppendPtr = (UINT64)mPcatTablePtr + mPcatTablePtr->Length;
  pIntCap = (PINTERLEAVE_CAPABILITIES)PcatAppendPtr;

  // Interleave Capabilities for PMEM as it is the only mode that BIOS supports configuration requests
  pIntCap->Type = TYPE_INTERLEAVE_CAPABILITY_DESCRIPTOR;
  pIntCap->MemMode = 3; // App Direct PM Mode
  pIntCap->IntAlignSize = INTERLEAVE_ALIGNMENT_SIZE;
  pIntCap->NumIntFormats = 1;  // 1 interleave format supported

  // get max channel per socket info via PCD
  OemGetMemTopologyInfo(&PlatformMaxSocket, &PlatformMaxChannelsPerSocket, &PlatformMaxDimm); 

  // SKX-D is max 4 channels per socket
  if (PlatformMaxChannelsPerSocket == 0x4) {
    pIntCap->IntFormatList[0] = CH_INT_SIZE_4KB | IMC_INT_SIZE_4KB | SOC_INT_4_WAY | RECOMMENDED_INTERLEAVE_FORMAT;
  } else {
    pIntCap->IntFormatList[0] = CH_INT_SIZE_4KB | IMC_INT_SIZE_4KB | SOC_INT_6_WAY | RECOMMENDED_INTERLEAVE_FORMAT;
  }

  // Update the Length field of Memory Interleave Capability Info Table to include the format lists of variable length (4 x m)
  pIntCap->Length = (sizeof(UINT32) * (pIntCap->NumIntFormats)) + INTERLEAVE_CAP_HEADER_SIZE;
  // Update the Length field of PCAT table accordingly
  mPcatTablePtr->Length += pIntCap->Length;

  ASSERT(mPcatTablePtr->Length <= MAX_PCAT_SIZE);
  DEBUG(( EFI_D_ERROR, "PCAT Interleave Capability Info Table Dump:\n" )) ;
  DisplayBuffer( (UINT8 *) PcatAppendPtr, pIntCap->Length  ) ;
}

/**
  @brief Initialize ACPI Platform Configuration Attribute Table

  Prepare header of ACPI Platform Configuration Attribute Table
  Memory for the table must be already allocated by the caller. This function
  only configures PCAT header fields.
  Whole unused memory buffer (outside of PCAT header) is cleared.
  Address and size of the buffer are stored in global variables (mPcatTablePtr + mPcatTableSize)

  @param[out]     PcatTablePtr    Address of allocated memory buffer, where PCAT header will be prepared
  @param[in]      PcatTableSize   Size of allocated memory buffer (bytes)

  @retval EFI_SUCCESS             PCAT header initialized
  @retval EFI_ALREADY_STARTED     PCAT already initialized, nothing done this time
  @retval EFI_INVALID_PARAMETER   NULL pointer passed as a parameter
  @retval EFI_BUFFER_TOO_SMALL    Memory buffer too small to fit PCAT header

**/
EFI_STATUS
InitializeAcpiPcat (
  OUT UINT64 *PcatTablePtr,
  IN  UINT64 PcatTableSize
  )
{
  UINT64 TempOemTableId;

  DEBUG ((EFI_D_INFO, "[InitializeAcpiPcat] start\n"));

  if ((NULL == PcatTablePtr)) {
    DEBUG ((DEBUG_ERROR,  "[InitializeAcpiPcat] ERROR - NULL pointer as a parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  // Don't initialize the table twice
  if (IsInitialized ()) {
    DEBUG ((DEBUG_ERROR,  "[InitializeAcpiPcat] ERROR - PCAT table already initialized\n"));
    return EFI_ALREADY_STARTED;
  }

  if (PcatTableSize < sizeof (NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE)) {
    DEBUG ((DEBUG_ERROR,  "[InitializeAcpiPcat] ERROR - Buffer too small to fit PCAT header\n"));
    return EFI_BUFFER_TOO_SMALL;
  }

  // Call OEM-hook to create PCAT table in buffer. If success, then skip code below and return
  if (OemCreatePcatTable(PcatTablePtr, PcatTableSize) == EFI_SUCCESS) {
	  DEBUG ((DEBUG_ERROR,	"[InitializeAcpiPcat] PCAT created by OEM-hook\n"));
	  return EFI_SUCCESS;
  }
  // Save address and size of PCAT for future purposes
  mPcatTablePtr = (PNVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE) PcatTablePtr;
  mPcatTableSize = PcatTableSize;

  // Clear whole buffer
  ZeroMem (mPcatTablePtr, mPcatTableSize);

  TempOemTableId  = PcdGet64(PcdAcpiDefaultOemTableId);

  mPcatTablePtr->Signature   = NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE_SIGNATURE;
  // Length = only header right now
  mPcatTablePtr->Length      = sizeof (NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE);
  mPcatTablePtr->Revision    = NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE_REVISION;
  CopyMem(mPcatTablePtr->OemID,     PcdGetPtr (PcdAcpiDefaultOemId), sizeof(mPcatTablePtr->OemID));
  CopyMem(mPcatTablePtr->OemTblID,  &TempOemTableId, sizeof(mPcatTablePtr->OemTblID));
  mPcatTablePtr->OemRevision = EFI_ACPI_OEM_REVISION;
  mPcatTablePtr->CreatorID   = EFI_ACPI_CREATOR_ID;
  mPcatTablePtr->CreatorRev  = EFI_ACPI_CREATOR_REVISION;
  //
  // Add both the Platform Capability Table and Interleave Capabilities to the PCAT
  // These two table when they added, they update PCAT Length.
  AddPlatCapInfoTableToPCAT();
  AddInterleaveCapabilitiesInfoTable();
  DEBUG ((DEBUG_ERROR,  "[InitializeAcpiPcat] length: %d\n",mPcatTablePtr->Length ));
  // Call OEM-hook to update PCAT table in buffer
  OemUpdatePcatTable(PcatTablePtr);

  //Add code to calculate the checksum for the table
  ComputeChecksum( (UINT8 *)PcatTablePtr, mPcatTablePtr->Length, &(mPcatTablePtr->Checksum) ) ;
  DEBUG ((DEBUG_ERROR,  "[InitializeAcpiPcat] checksum: %d\n",mPcatTablePtr->Checksum ));
  return EFI_SUCCESS;

}


/**
  @brief Create SSKU Attribute Extension Table in ACPI PCAT

  Create a new SSKU Attribute Extension Table at the end of PCAT.
  PCAT must be already initialized and have a free space in the
  allocated memory buffer to fit SAET. PCAT may already contain other
  extension tables.
  Newly created SAET will be empty (only headers are configured).
  Actual size of PCAT is updated after SAET creation.

  @param                          None

  @retval EFI_SUCCESS             Empty SAET added to PCAT
  @retval EFI_NOT_STARTED         PCAT not initialized yet
  @retval EFI_INVALID_PARAMETER   NULL pointer passed as a parameter
  @retval EFI_BUFFER_TOO_SMALL    Left space in PCAT is to small to fit new empty SAET

**/
EFI_STATUS
CreateAcpiPcatSaet (
  VOID
  )
{
  PNVDIMM_SSKU_ATTR_EXT_TABLE AttrExtTable;

  DEBUG ((EFI_D_INFO, "[CreateAcpiPcatSaet] start\n"));

  if (!IsInitialized ()) {
    DEBUG ((DEBUG_ERROR,  "[CreateAcpiPcatSaet] ERROR - PCAT table not initialized yet\n"));
    return EFI_NOT_STARTED;
  }

  // Check if buffer is large enough to fit SSKU Attribute Extension Table header
  if (mPcatTableSize - mPcatTablePtr->Length < sizeof (NVDIMM_SSKU_ATTR_EXT_TABLE)) {
    DEBUG ((DEBUG_ERROR,  "[CreateAcpiPcatSaet] ERROR - ACPI Table size not big enough to fit SSKU Attribute Extension Table header\n"));
    return EFI_BUFFER_TOO_SMALL;
  }

  // Append SSKU Attribute Extension Table at the actual end of PCAT
  AttrExtTable = (PNVDIMM_SSKU_ATTR_EXT_TABLE) PTR_OFFSET (mPcatTablePtr, mPcatTablePtr->Length);

  // Fill SSKU Attribute Extension Table header
  AttrExtTable->Type = SSKU_ATTRIBUTE_EXTENSION_TABLE_TYPE;
  AttrExtTable->VendorID = SSKU_ATTRIBUTE_EXTENSION_TABLE_VENDOR_ID;
  CopyMem (&AttrExtTable->Guid, &SSKU_ATTRIBUTE_EXTENSION_TABLE, sizeof (AttrExtTable->Guid));
  // Length = only header right now
  AttrExtTable->Length = sizeof (NVDIMM_SSKU_ATTR_EXT_TABLE);

  // Update length of whole PCAT table
  mPcatTablePtr->Length += AttrExtTable->Length;

  return EFI_SUCCESS;
}

/**
  @brief Find SSKU Attribute Extension Table in ACPI PCAT

  Look for a SSKU Attribute Extension Table, where new VLR could be appended.
  To append new VLR, SAET must be at the end of PCAT (complicated resize of
  existing tables inside of PCAT).
  If there is a SAET inside of PCAT but not at the end, a failue will be
  returned.

  @param[in]      PcatTablePtr      PCAT buffer (already initialized)
  @param[out]     PcatSeatTablePtr  Found SAET

  @retval EFI_SUCCESS             SAET found at the end of PCAT
  @retval EFI_NOT_STARTED         PCAT not initialized yet
  @retval EFI_NOT_FOUND           SAET not found at the end of PCAT
  @retval EFI_INVALID_PARAMETER   NULL pointer passed as a parameter

**/
EFI_STATUS
FindAcpiPcatSaet (
  OUT PNVDIMM_SSKU_ATTR_EXT_TABLE *PcatSeatTablePtr
  )
{
  PNVDIMM_SSKU_ATTR_EXT_TABLE CurrentPtr;

  if ((NULL == PcatSeatTablePtr)) {
    DEBUG ((DEBUG_ERROR,  "[FindAcpiPcatSaet] ERROR - NULL pointer as a parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (!IsInitialized ()) {
    DEBUG ((DEBUG_ERROR,  "[FindAcpiPcatSaet] ERROR - PCAT table not initialized yet\n"));
    return EFI_NOT_STARTED;
  }

  // Start with end of PCAT header
  CurrentPtr = (PNVDIMM_SSKU_ATTR_EXT_TABLE) PTR_OFFSET (mPcatTablePtr, sizeof (*mPcatTablePtr));

  // Don't go outside of current PCAT length
  while ((VOID*)CurrentPtr < PTR_OFFSET (mPcatTablePtr, mPcatTablePtr->Length)) {
    if ((CurrentPtr->VendorID == SSKU_ATTRIBUTE_EXTENSION_TABLE_VENDOR_ID) &&
        (CurrentPtr->Type == SSKU_ATTRIBUTE_EXTENSION_TABLE_TYPE) &&
        (!CompareMem (&CurrentPtr->Guid, &SSKU_ATTRIBUTE_EXTENSION_TABLE, sizeof (CurrentPtr->Guid)))) {
      // Found SSKU Attribute Extension Table, check if it is the last table in PCAT
      if (PTR_OFFSET (CurrentPtr, CurrentPtr->Length) == PTR_OFFSET (mPcatTablePtr, mPcatTablePtr->Length)) {
        // There is nothing after SAET in PCAT
        *PcatSeatTablePtr = CurrentPtr;
        return EFI_SUCCESS;
      }
      // Something is after the SAET, won't be able to append to it, so need to look for another one
    }
    // Check the next table in PCAT
    CurrentPtr = (PNVDIMM_SSKU_ATTR_EXT_TABLE) PTR_OFFSET (CurrentPtr, CurrentPtr->Length);
  }

  return EFI_NOT_FOUND;
}

/**
  @brief Add new VLR to SSKU Attribute Extension Table in ACPI PCAT

  Add new VLR (Variable Length Record) to SAET in PCAT. If there is no SAET at
  the actual end of PCAT, a new SAET is created.
  PCAT must be already initialized and have a free space in the allocated
  memory buffer to fit VLR (and eventual SAET).
  Actual sizes of PCAT and SAET are updated after appending VLR.


  @param[out]   PcatTablePtr      PCAT buffer (already initialized)
  @param[in]    PcatTableSize     Allowed total PCAT size- size of allocated memory buffer (bytes)
  @param[in]    Vlr               VLR to be added to SAET
  @param[in]    VlrSize           Size of the VLR to be added to SAET

  @retval EFI_SUCCESS             VLR added to SAET
  @retval EFI_NOT_STARTED         PCAT not initialized yet
  @retval EFI_INVALID_PARAMETER   NULL pointer passed as a parameter
  @retval EFI_NOT_FOUND           Cannot find a SAET and a new one cannot be created
  @retval EFI_BUFFER_TOO_SMALL    Left space in PCAT is to small to fit new VLR

**/
EFI_STATUS
AddVlr2PcatSaet (
  IN  UINT8   *Vlr,
  IN  UINTN   VlrSize
  )
{
  PNVDIMM_SSKU_ATTR_EXT_TABLE AttrExtTable;
  UINTN SpaceLeft;
  EFI_STATUS Status;

  if ((NULL == Vlr) ||
      (0 == VlrSize)) {
    DEBUG ((DEBUG_ERROR,  "[AddVlr2PcatSaet] ERROR - NULL pointer as a parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (!IsInitialized ()) {
    DEBUG ((DEBUG_ERROR,  "[AddVlr2PcatSaet] ERROR - PCAT table not initialized yet\n"));
    return EFI_NOT_STARTED;
  }

  ASSERT (VlrSize < MAX_UINT16);

  // Find SSKU Attribute Extension Table to append to
  Status = FindAcpiPcatSaet (&AttrExtTable);
  if (EFI_NOT_FOUND == Status) {
    // Need to create SSKU Attribute Extension Table first
    Status = CreateAcpiPcatSaet ();
  }

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR,  "[AddVlr2PcatSaet] ERROR - Cannot find/create SSKU Attribute Extension Table in PCAT\n"));
    return EFI_NOT_FOUND;
  }

  // Check if there is enough space left in PCAT buffer to fit another VLR
  SpaceLeft = mPcatTableSize - mPcatTablePtr->Length;
  if (SpaceLeft < VlrSize) {
    DEBUG ((DEBUG_ERROR,  "[AddVlr2PcatSaet] ERROR - ACPI Table size not big enough to fit another VLR; have only %d bytes free, next VLR requires %d\n", SpaceLeft, VlrSize));
    return EFI_BUFFER_TOO_SMALL;
  }

  // Copy VLR to the end of SSKU Attribute Extension Table
  CopyMem ((UINT8*) AttrExtTable + AttrExtTable->Length, Vlr, VlrSize);

  // Update current size of ACPI tables
  AttrExtTable->Length += (UINT16) VlrSize;
  mPcatTablePtr->Length += (UINT16) VlrSize;

  DEBUG ((DEBUG_INFO,  "[AddVlr2PcatSaet] ERROR - VLR added to PCAT->SSKU Attribute Extension Table\n"));

  return EFI_SUCCESS;
}

/**
  @brief Install protocol to access ACPI PCAT supporting functions
*/
EFI_STATUS
InstallAcpiPcatProtocol (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS Status;
  if (NULL == SystemTable) {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  mAcpiPcat.InitializeAcpiPcat  = InitializeAcpiPcat;
  mAcpiPcat.CreateAcpiPcatSaet  = CreateAcpiPcatSaet;
  mAcpiPcat.AddVlr2PcatSaet     = AddVlr2PcatSaet;
  mAcpiPcat.IsInitialized       = IsInitialized;

  Status = gBS->InstallMultipleProtocolInterfaces (
              &ImageHandle,
              &gAcpiPcatProtocolGuid,
              &mAcpiPcat,
              NULL
           );

  return EFI_SUCCESS;
}
