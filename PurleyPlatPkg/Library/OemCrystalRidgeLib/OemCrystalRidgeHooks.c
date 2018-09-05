/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++
Copyright (c) 1996 - 2016, Intel Corporation.

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

  OemCrystalRidgeHooks.c

Abstract:

  This file contains OEM hooks for Intel AEP DIMM BIOS support.  In most cases no or very few OEM-specific changes are expected.
  Those who do need customization of AEP DIMM support features should try using these hooks to achieve the goals.

--*/

//
// Include files
//
#include "CrystalRidge.h"
#include "Nfit.h"
#include "Pcat.h"
#include <Protocol/PcatProtocol.h>

/*
//
// Externals
//
extern struct SystemMemoryMapHob       *mSystemMemoryMap;
extern CR_INFO                         CrInfo;
extern BLK_CTRL_WND_INFO               BlkAndCtrlWinInfo;
extern SAD_CONTROL_REGION              SadControlRegion;
extern EFI_SMM_BASE2_PROTOCOL          *mSmmBase;
extern EFI_SMM_SYSTEM_TABLE2           *mSmst;
extern EFI_MP_SERVICES_PROTOCOL        *mMpService;
extern EFI_CPU_CSR_ACCESS_PROTOCOL     *mCpuCsrAccess;
extern NGN_ACPI_SMM_INTERFACE          *mNgnAcpiSmmInterface;
extern EFI_NGN_ACPI_SMM_INTERFACE_PROTOCOL *mNgnAcpiSmmInterfaceArea;
extern EFI_NGN_ACPI_SMM_INTERFACE_PROTOCOL  NgnAcpiSmmInterfaceArea;
*/

#pragma warning(disable : 4100)
#pragma optimize("", off)


/**

Routine Description:   OEM hook to modify/rebuild PCD data in buffer, before BIOS updates BIOS partition of PCD on the given AEP DIMM

  @param socket     - Socket Count
  @param ch         - Channel Count
  @param dimm       - DIMM Count

  @retval void          - None

**/
VOID
OemGetMemTopologyInfo(
    UINT8 *skt,
    UINT8 *ch,
    UINT8 *dimm)
{
  //
  // Need to grab those variables from PCD to be the same as in MemorySubClassEntryPoint()
  *skt  = (UINT8) PcdGet32(PcdOemSkuBoardSocketCount);
  *ch   = (UINT8) PcdGet32(PcdOemSkuMaxChannel);
  *dimm = (UINT8) PcdGet32(PcdOemSkuMaxDimmPerChannel);
  return;
}
/**

Routine Description:   OEM hook to modify/rebuild PCD data in buffer, before BIOS updates BIOS partition of PCD on the given AEP DIMM

  @param socket      - Socket ID
  @param ch          - Channel ID
  @param dimm        - DIMM number
  @param *cfgCurRecBuffer   - (Input/Output) Pointer to pointer to buffer containing Current Config record data
  @param cfgCurRecLength    - (Input/Output) Length of the Current Config record data

  @retval void          - None

**/
VOID
OemBeforeUpdatePCDBIOSPartition(
    UINT8 skt,
    UINT8 ch,
    UINT8 dimm,
    UINT8 **cfgCurRecBuffer,
    UINT32 *cfgCurRecLength )
{
    NGN_DIMM_PLATFORM_CFG_HEADER        *headerCfgCur;
    NGN_DIMM_PLATFORM_CFG_CURRENT_BODY  *bodyCfgCur;

    //
    // Create pointers to Current Config record in buffer
    //
    headerCfgCur = ( NGN_DIMM_PLATFORM_CFG_HEADER * ) (*cfgCurRecBuffer) ;
    bodyCfgCur = ( NGN_DIMM_PLATFORM_CFG_CURRENT_BODY * ) ( headerCfgCur + sizeof( NGN_DIMM_PLATFORM_CFG_HEADER ) ) ;

    //
    // Place OEM specific code here
    // If you need to modify the buffer address and/or length, make sure you return the new values via the input/output pointers
    // If you make any change to the data in buffer, make sure to update the checksum field in the header
    //

    //
    //  Example below for modifying OEM ID fields
    //

    //CopyMem (headerCfgCur->OemId, "OEM_ID", 6);
    //CopyMem (&(headerCfgCur->OemTableId), "OEMTBLID", 8);
    //headerCfgCur->OemRevision = EFI_ACPI_OEM_REVISION;
    //headerCfgCur->CreatorId = EFI_ACPI_CREATOR_ID;
    //headerCfgCur->CreatorRevision = EFI_ACPI_CREATOR_REVISION;


    // Update checksum if data record in buffer has been changed
    //ComputeChecksum( (*cfgCurRecBuffer), headerCfgCur->Length, &(headerCfgCur->Checksum) ) ;

    // Update return values only if they have been changed
    //*cfgCurRecBuffer = (UINT8*) headerCfgCur;
    //*cfgCurRecLength = headerCfgCur->Length;

    return;             //  Default implementation.  No OEM-specific code

}



/**

Routine Description:   OEM hook to modify/rebuild PCD data in buffer, before BIOS updates OS partition of PCD on the given AEP DIMM

    @param socket       - Socket ID
    @param ch             - Channel ID
    @param dimm        - DIMM number
    @param *cfgOutRecBuffer   - (Input/Output) Pointer to pointer to buffer containing Config  Output record data
    @param cfgOutRecLength    - (Input/Output) Length of the Config Output record data

    @retval void          - None

**/
VOID
OemBeforeUpdatePCDOSPartition(
    UINT8 skt,
    UINT8 ch,
    UINT8 dimm,
    UINT8 **cfgOutRecBuffer,
    UINT32 *cfgOutRecLength )
{
    NGN_DIMM_PLATFORM_CFG_HEADER       *headerCfgOut;
    NGN_DIMM_PLATFORM_CFG_OUTPUT_BODY  *bodyCfgOut;

    //
    // Create pointers to Config Output record in buffer
    //
    headerCfgOut = ( NGN_DIMM_PLATFORM_CFG_HEADER * ) (*cfgOutRecBuffer) ;
    bodyCfgOut = ( NGN_DIMM_PLATFORM_CFG_OUTPUT_BODY * ) ( headerCfgOut + sizeof( NGN_DIMM_PLATFORM_CFG_HEADER ) ) ;

    //
    // Place OEM specific code here
    // If you need to modify the buffer address and/or length, make sure you return the new values via the input/output pointers
    // If you make any change to the data in buffer, make sure to update the checksum field in the header
    //

    //
    //  Example below for modifying OEM ID fields
    //

    //CopyMem (headerCfgOut->OemId, "OEM_ID", 6);
    //CopyMem (&(headerCfgOut->OemTableId), "OEMTBID", 8);
    //headerCfgOut->OemRevision = EFI_ACPI_OEM_REVISION;
    //headerCfgOut->CreatorId = EFI_ACPI_CREATOR_ID;
    //headerCfgOut->CreatorRevision = EFI_ACPI_CREATOR_REVISION;

    // Update checksum if data record in buffer has been changed
    //ComputeChecksum( (*cfgOutRecBuffer), headerCfgOut->Length, &(headerCfgOut->Checksum) ) ;

    // Update return values only if they have been changed
    //*cfgOutRecBuffer = (UINT8*) headerCfgOut;
    //*cfgOutRecLength = headerCfgOut->Length;

    return;             //  Default implementation.  No OEM-specific code

}



/**

Routine Description:   OEM Hook for creation of NFIT table in the given buffer. Called before NFIT table is created by Intel code.

  @param *Table                  - Pointer to empty buffer for NFIT table
  @param  TableBufferSize    - Size of the buffer

  @retval EFI_STATUS             EFI_UNSUPPORTED - Nothing was done by OEM hook.
                                             EFI_SUCCESS  - Buffer contains the NFIT table ready for use
**/
EFI_STATUS
OemCreateNfitTable (
   UINT64  *Table,
   UINT32   TableBufferSize
   )
{
  PNVDIMM_FW_INTERFACE_TABLE NfitPtr;

  //
  // Create pointer to NFIT table in buffer
  //
  NfitPtr = (PNVDIMM_FW_INTERFACE_TABLE) Table;

  //
  //  Place OEM specific code here to create NFIT table in buffer,  and return EFI_SUCCESS
  //
   return  EFI_UNSUPPORTED;       // default – No OEM specific code
}


/**

Routine Description:   OEM Hook for modification of NFIT table in the given buffer. Called after NFIT table is created by Intel code.

  @param *Table   - Pointer to buffer containing NFIT table

  @retval void        - None

**/
VOID
OemUpdateNfitTable (
   UINT64  *Table
   )
{
  PNVDIMM_FW_INTERFACE_TABLE NfitPtr;
  UINT32   TableLength;

  //
  // Create pointer to NFIT table in buffer
  //
  NfitPtr = (PNVDIMM_FW_INTERFACE_TABLE) Table;
  TableLength = NfitPtr->Length;

  //
  //  Place OEM specific code here to modify NFIT table in buffer
  //  If table length is changed, make sure to update the table length field
  //  Caller will update the checksum field of the table after this function returns
  //

  //
  //  Example below for modifying OEM ID fields in the table
  //
  //CopyMem(NfitPtr->OemID, "OEM_ID", 6);
  //CopyMem(NfitPtr->OemTblID, "OEMTBLID", 8);
  //NfitPtr->OemRevision = EFI_ACPI_OEM_REVISION;
  //NfitPtr->CreatorID   = EFI_ACPI_CREATOR_ID;
  //NfitPtr->CreatorRev  = EFI_ACPI_CREATOR_REVISION;

  return;        // default – No OEM specific code
}



/**

Routine Description:   OEM Hook for creation of PCAT table in the given buffer. Called before PCAT table is created by Intel code.

  @param *Table                  - Pointer to empty buffer for PCAT table
  @param  TableBufferSize    - Size of the buffer

  @retval EFI_STATUS             EFI_UNSUPPORTED - Nothing was done by OEM hook.
                                             EFI_SUCCESS  - Buffer contains the PCAT table ready for use
**/
EFI_STATUS
OemCreatePcatTable (
   UINT64  *Table,
   UINT64   TableBufferSize
   )
{
  PNVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE  PcatPtr;

  //
  // Create pointer to PCAT  table in buffer
  //
  PcatPtr = (PNVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE) Table;

  //
  //  Place OEM specific code here to create PCAT table in buffer,  and return EFI_SUCCESS
  //
  return  EFI_UNSUPPORTED;       // default – No OEM specific code
}


/**

Routine Description:   OEM Hook for modification of PCAT table in the given buffer. Called after PCAT table is created by Intel code.

  @param *Table   - Pointer to buffer containing PCAT table

  @retval void        - None

**/
VOID
OemUpdatePcatTable (
   UINT64  *Table
   )
{
    PNVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE  PcatPtr;
    UINT32   TableLength;

    //
    // Create pointer to PCAT table in buffer
    //
    PcatPtr = (PNVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE) Table;
    TableLength = PcatPtr->Length;

    //
    //  Place OEM specific code here to modify PCAT table in buffer
    //  If table length is changed, make sure to update the table length field
    //  Caller will update the checksum field of the table after this function returns
    //

    //
    //  Example below for modifying OEM ID fields in the table
    //
    // CopyMem (PcatPtr->OemID, "OEM_ID", 6);
    // CopyMem (PcatPtr->OemTblID, "OEMTBLID", 8);
    // PcatPtr->OemRevision = EFI_ACPI_OEM_REVISION;
    // PcatPtr->CreatorID   = EFI_ACPI_CREATOR_ID;
    // PcatPtr->CreatorRev  = EFI_ACPI_CREATOR_REVISION;

    return;        // default – No OEM specific code
}





