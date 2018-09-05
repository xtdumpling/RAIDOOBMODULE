/**
 This file contains an 'Intel Peripheral Driver' and is
 licensed for Intel CPUs and chipsets under the terms of your
 license agreement with Intel or your vendor.  This file may
 be modified by the user, subject to additional terms of the
 license agreement
 **/
/**

 Copyright (c) 2009-2016, Intel Corporation.  All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license.  Except as permitted by such
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.

 @file WheaPlatformHooks.c

 Whea Platform hook functions and platform data, which needs to be
 ported.

 **/

#include "Library/WheaPlatformHooksLib.h"
#include <Register/PchRegsLpc.h>
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsSmbus.h>
#include <Library/GpioLib.h>
#include <Library/PeiDxeSmmGpioLib/GpioLibrary.h>
#include <IncludePrivate/Library/GpioPrivateLib.h>
#include <Library/PciLib.h>

//
// Data definitions & structures
//

#define EFI_ACPI_CREATOR_ID                 0x4C544E49  // "INTL"
#define EFI_ACPI_CREATOR_REVISION           0x00000001
#define EFI_ACPI_OEM_TABLE_ID               SIGNATURE_64 ('I', 'V', 'T', 'E', 'X', ' ', ' ') // OEM table id 8 bytes long
#define EFI_WHEA_OEM_REVISION               0x0001;

//////////////////////////////////////////////////////////////////////////
//                     PLATFORM HOOK DATA & TABLES                      //
//                     PLATFORM HOOK DATA & TABLES                      //
//                     PLATFORM HOOK DATA & TABLES                      //
//////////////////////////////////////////////////////////////////////////

//
// Error Sources that are supported in the platform & chipset
//

// HW Error notification method used for FW first model
#define     ERROR_STATUS_GAS_DATADEF        {0, 64, 0, 0, 0x0000}
#define     SCI_ERROR_NOTIFY_DATADEF        {ERR_NOTIFY_SCI, sizeof(HW_NOTIFY_METHOD), 0, 0, 0, 0, 0, 0, 0}
#define     NMI_ERROR_NOTIFY_DATADEF        {ERR_NOTIFY_NMI, sizeof(HW_NOTIFY_METHOD), 0, 0, 0, 0, 0, 0, 0}
// APTIOV_SERVER_OVERRIDE_RC_START : Added support for Polling for Corrected error
#define     POLLED_ERROR_NOTIFY_DATADEF     {ERR_NOTIFY_POLLING, sizeof(HW_NOTIFY_METHOD), 0x3E, 60000, 2, 0, 0, 0, 0}

//
// Generic error HW error sources - Tip: list all FW First error sources
// Note: Used 1KB Error Status block for each error source
//
//  RelSrcId,   MaxErrData, ErrStsAddr,                Notify method,           Max ErrSts block Size
GENERIC_HW_ERROR_SOURCE PlatformCeSrc = { 0xffff, 4096,
    ERROR_STATUS_GAS_DATADEF, POLLED_ERROR_NOTIFY_DATADEF, 4096 }; 
// APTIOV_SERVER_OVERRIDE_RC_END : Added support for Polling for Corrected error
GENERIC_HW_ERROR_SOURCE PlatformUeSrc = { 0xffff, 4096,
    ERROR_STATUS_GAS_DATADEF, NMI_ERROR_NOTIFY_DATADEF, 4096 };

SYSTEM_GENERIC_ERROR_SOURCE SysGenErrSources[] = {
//  Type,                     SrcId,  Flags,  NumOfRecords   MaxSecPerRecord     SourceInfo
    { GENERIC_ERROR_CORRECTED, 0, 0, 1, 1, &PlatformCeSrc }, {
        GENERIC_ERROR_FATAL, 0, 0, 1, 1, &PlatformUeSrc }, };
UINTN NumSysGenericErrorSources = (sizeof(SysGenErrSources)
    / sizeof(SYSTEM_GENERIC_ERROR_SOURCE));

//
// Native error sources - Tip: Errors that can be handled by OS directly
// Note: Used 1KB Error Status block for each error source
//
HW_NOTIFY_METHOD NotifyMthd = { 0, sizeof(HW_NOTIFY_METHOD), 0, 0, 0, 0x9, 0,
    0x9, 0 };

SYSTEM_NATIVE_ERROR_SOURCE SysNativeErrSources[] = { 00 };

//UINTN NumNativeErrorSources         = (sizeof(SysNativeErrSources)/sizeof(SYSTEM_NATIVE_ERROR_SOURCE));
UINTN NumNativeErrorSources = 00;

UINT32 mWheaPlatformType = 0xFFFF;
UINT16 mWheaPlatformFlavor = 0xFFFF;

//////////////////////////////////////////////////////////////////////////
//                      PLATFORM HOOK FUNCTIONS                         //
//                      PLATFORM HOOK FUNCTIONS                         //
//                      PLATFORM HOOK FUNCTIONS                         //
//////////////////////////////////////////////////////////////////////////

#define PCH_SCI_WHEA_GPP_GROUP  GPIO_SKL_H_GROUP_GPP_A
#define PCH_SCI_WHEA_GPP_PAD    GPIO_SKL_H_GPP_A12


EFI_STATUS InitWheaPlatformHookLib(void) {
  /* uncomment after moving to Platform package
   EFI_PLATFORM_TYPE_PROTOCOL  *PlatformType;
   EFI_STATUS                  Status;
   Status  = EFI_SUCCESS;

   if (mWheaPlatformType == 0xFFFF) {
   Status = gBS->LocateProtocol (
   &gEfiPlatformTypeProtocolGuid,
   NULL,
   &PlatformType
   );
   if(EFI_ERROR(Status)) {
   mWheaPlatformType = PlatformType->Type;
   }
   }
   return Status;
   */
  return EFI_SUCCESS;
}

/**

 This hook is call at POST time (DXE) by WheaSupport driver while creating WHEA ACPI tables. This hook will allow the platform to update OEM Ids and OEM table Ids of WHEA Acpi tables.

 @param TableHeader - Pointer to WHEA ACPI table.

 typedef struct {
 UINT32  Signature;
 UINT32  Length;
 UINT8   Revision;
 UINT8   Checksum;
 UINT8   OemId[6];
 UINT64  OemTableId;
 UINT32  OemRevision;
 UINT32  CreatorId;
 UINT32  CreatorRevision;
 } EFI_ACPI_DESCRIPTION_HEADER;

 @retval None

 **/
VOID UpdateAcpiTableIds(EFI_ACPI_DESCRIPTION_HEADER *TableHeader) {
  UINT64  TempOemTableId;

  //
  // Update the OEMID, Creator ID, and Creator revision.
  //
  *(UINT32 *) (TableHeader->OemId) = 'I' + ('N' << 8) + ('T' << 16) + ('E' << 24);
  *(UINT16 *) (TableHeader->OemId + 4) = 'L' + (' ' << 8);

  //
  // Update the OEMID and OEM Table ID.
  //
  TempOemTableId = PcdGet64(PcdAcpiDefaultOemTableId);

  CopyMem (TableHeader->OemId, PcdGetPtr(PcdAcpiDefaultOemId), sizeof(TableHeader->OemId));
  CopyMem (&TableHeader->OemTableId, &TempOemTableId, sizeof(TableHeader->OemTableId));

  TableHeader->CreatorId = EFI_ACPI_CREATOR_ID;
  TableHeader->CreatorRevision = EFI_ACPI_CREATOR_REVISION;

  //
  // Update OEM revision and OEM TABLE ID based on the platform/SKU
  //
  TableHeader->OemRevision = EFI_WHEA_OEM_REVISION;
}

/**

 Get Field replaceable unit Information.

 @param MemInfo   - Pointer to memory device information structure.
 @param FruId     - GUID representing the FRU ID, if it exists, for the section reporting the error.
 The default value is zero indicating an invalid FRU ID. System software can use this to uniquely identify a physical device for tracking purposes.
 Association of a GUID to a physical device is done by the platform in an implementation-specific way. See UEFI APPENDIX N for more details.
 @param FruString - ASCII string identifying the FRU hardware. See UEFI APPENDIX N for more details.
 @param Card      - The card number of the memory error location.

 @retval EFI_SUCCESS - Output Parameters contain valid information.

 **/
EFI_STATUS WheaGetMemoryFruInfo( IN MEMORY_DEV_INFO *MemInfo,
    OUT EFI_GUID *FruId, OUT CHAR8 *FruString, OUT UINT16 *Card) {
  CHAR8 SlotStr[] = { 'D', 'I', 'M', 'M', ' ', '?', '?', 0 };
  UINT32 SlotLetter;
  EFI_STATUS Status;
  UINTN      FruStringSize;

  //
  // Data for Platform memory.
  // FruString buffer is 20 bytes.
  //

  //
  // DIMM slot letter.
  //
  if ((MemInfo->ValidBits & PLATFORM_MEM_NODE_VALID)
      && (MemInfo->ValidBits & PLATFORM_MEM_CARD_VALID)) {
    SlotLetter = (((UINT32)(MemInfo->Node)) << 1) + (UINT32)(MemInfo->Channel)
        + 'A';
    if (SlotLetter <= 'Z') {
      SlotStr[5] = (CHAR8) (SlotLetter);
    }
  }

  //
  // DIMM slot number within channel (1-based).
  //
  if (MemInfo->ValidBits & PLATFORM_MEM_MODULE_VALID) {
    SlotStr[6] = (CHAR8) (MemInfo->Dimm + '1');
  }
  FruStringSize = sizeof(((GENERIC_ERROR_DATA_ENTRY*)0)->FruString); 
  Status = AsciiStrCpyS(FruString, FruStringSize, SlotStr);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  //
  // This is the channel information on the memory controller on the CPU socket (0-based).
  //
  if (MemInfo->ValidBits & PLATFORM_MEM_CARD_VALID) {
    *Card = MemInfo->Channel;
  }

  return EFI_SUCCESS;
}

/**

 Trigger a System Common Interrupt(SCI). This hook is invoked at runtime (in SMM) at the end of creating Error data for WHEA OS.
 WheaPlatformSupport driver will call this when correctable or recoverable errors are being reported via WHEA to the OS.

 @param SmmBase - Pointer to EFI_SMM_BASE2_PROTOCOL. This protocol is used to locate the SMST during SMM driver initialization.
 @param Smst    - Pointer to System Management Service Table. This can be used access I/O functions.

 @retval EFI_SUCCESS - If programming HW to trigger SCI is success.
 @retval EFI_DEVICE_ERROR - If failed to program the HW.
 @retval EFI_UNSUPPORTED - If OS is not Whea Enabled

 **/
EFI_STATUS TriggerWheaSci(
  EFI_SMM_BASE2_PROTOCOL    *SmmBase,
  EFI_SMM_SYSTEM_TABLE2     *Smst
  ) 
{
  EFI_STATUS  Status;
  GPIO_PAD    GpioPad;
  GPIO_CONFIG GpioPadConfig;
  UINT32      InvertState;
  UINT32      Data32;

  GpioPad = PCH_SCI_WHEA_GPP_PAD;
  GpioGetPadConfig (GpioPad, &GpioPadConfig);

  //DEBUG ((DEBUG_ERROR,"Generating SCI Now\n"));

  //DEBUG ((DEBUG_ERROR,"TriggerWheaSci PadMode %x \n", GpioPadConfig.PadMode));
  //DEBUG ((DEBUG_ERROR,"TriggerWheaSci HostSoftPadOwn %x \n", GpioPadConfig.HostSoftPadOwn));
  //DEBUG ((DEBUG_ERROR,"TriggerWheaSci Direction %x \n", GpioPadConfig.Direction));
  //DEBUG ((DEBUG_ERROR,"TriggerWheaSci OutputState %x \n", GpioPadConfig.OutputState));
  //DEBUG ((DEBUG_ERROR,"TriggerWheaSci InterruptConfig %x \n", GpioPadConfig.InterruptConfig));
  //DEBUG ((DEBUG_ERROR,"TriggerWheaSci PowerConfig %x \n", GpioPadConfig.PowerConfig));
  //DEBUG ((DEBUG_ERROR,"TriggerWheaSci ElectricalConfig %x \n", GpioPadConfig.ElectricalConfig));
  //DEBUG ((DEBUG_ERROR,"TriggerWheaSci LockConfig %x \n", GpioPadConfig.LockConfig));

  Status = GpioGetInputInversion (GpioPad,&InvertState);
  //DEBUG ((DEBUG_INFO,"TriggerWheaSci InvertState %x \n", InvertState));
  InvertState = InvertState ^ 1;
  Status = GpioSetInputInversion(GpioPad,InvertState);
  Status = GpioGetInputInversion (GpioPad,&InvertState);
  //DEBUG ((DEBUG_INFO,"TriggerWheaSci InvertState1 %x \n", InvertState));

  while(TRUE) {
    //DEBUG ((DEBUG_INFO,"SCI: while TRUE \n"));
    GpioGetGpiGpeSts(GpioPad,&Data32);
    //DEBUG ((DEBUG_INFO,"TriggerWheaSci Data32 %x \n", Data32));
    if(Data32)  {
      //DEBUG ((DEBUG_INFO,"SCI: if(Data32) \n"));
      Data32 = 0xfff;
      while (Data32--);
      
      //
      // Reset the gpio inv register
      //
      Status = GpioGetInputInversion (GpioPad,&InvertState);
      //DEBUG ((DEBUG_INFO,"TriggerWheaSci InvertState2 %x \n", InvertState));
      InvertState = InvertState ^ 1;
      Status = GpioSetInputInversion(GpioPad,InvertState);
      Status = GpioGetInputInversion (GpioPad,&InvertState);
      //DEBUG ((DEBUG_INFO,"TriggerWheaSci InvertState3 %x \n", InvertState));
      //DEBUG ((DEBUG_INFO,"TriggerWheaSci: Sci triggered \n"));
      break;
    }
  }

  return EFI_SUCCESS;

}

/**

 Trigger Non-maskable Interrupt(NMI).
 This hook is invoked at runtime (in SMM) at the end of creating Error data for WHEA OS.
 WheaPlatformSupport driver will call this when uncorrectable or fatal errors(Outside MCA domain) are being reported via WHEA to the OS.

 @param SmmBase - Pointer to EFI_SMM_BASE2_PROTOCOL. This protocol is used to locate the SMST during SMM driver initialization.
 @param Smst    - Pointer to System Management Service Table. This can be used access I/O functions.

 @retval EFI_SUCCESS - If programming HW to trigger NMI is success.
 @retval EFI_DEVICE_ERROR - If failed to program the HW.

 **/
EFI_STATUS TriggerWheaNmi(EFI_SMM_BASE2_PROTOCOL *SmmBase,
    EFI_SMM_SYSTEM_TABLE2 *Smst) {

  UINT8 Data;
  UINT8 Save_Nmi2Smi_En;
  UINT8 Save_Port70;
  UINT16 Data16;
  EFI_STATUS Status;
  UINT32 SmbusBase;
  //
  // TcoBase was moved to SMBUS device in PCH
  //
  SmbusBase = PciRead32 (
      PCI_LIB_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH,
          PCI_DEVICE_NUMBER_PCH_SMBUS,
          PCI_FUNCTION_NUMBER_PCH_SMBUS,
          R_PCH_SMBUS_TCOBASE)
  );

  // Read the NMI2SMI_EN bit, save it for future restore
  Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT8, (SmbusBase + R_PCH_TCO1_CNT + 1), 1, &Save_Nmi2Smi_En);

  // Set the NMI2SMI_EN bit to 0
  // This also clears NMI_NOW if it is already set
  Data = (UINT8)(Save_Nmi2Smi_En & 0xFD);
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, (SmbusBase + R_PCH_TCO1_CNT + 1), 1, &Data);

  // Enable NMI_EN
  Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT8, 0x74, 1, &Save_Port70);
  Data = (UINT8)(Save_Port70 & 0x7F);
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, R_PCH_NMI_EN, 1, &Data);

  // Set NMI_NOW = 1
  Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT8, (SmbusBase + R_PCH_TCO1_CNT + 1), 1, &Data);
  Data = (UINT8) (Data | 0x01);
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, (SmbusBase + R_PCH_TCO1_CNT + 1), 1, &Data);

  // Clear the MCHSERR_STS bit, bit 12
  Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT16, (SmbusBase + R_PCH_TCO1_STS), 1, &Data16);
  Data16 = (Data16 | 0x1000);
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT16, (SmbusBase + R_PCH_TCO1_STS), 1, &Data16);

  // Clear the NMI2SMI_STS bit if set
  Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT16, (SmbusBase + R_PCH_TCO1_STS), 1, &Data16);
  if (Data16 & 0x0001) {
    // check port 0x61
    Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT8, R_PCH_NMI_SC, 1, &Data);
    if (Data & 0x80) {
      Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT8, R_PCH_NMI_SC, 1, &Data);
      Data = (UINT8) (Data | 0x04);
      Data = (UINT8) (Data & 0x0F);
      Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, R_PCH_NMI_SC, 1, &Data);
      Data = (UINT8) (Data & 0x0B);
      Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, R_PCH_NMI_SC, 1, &Data);
    }
  }

  // Restore NMI_EN
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, R_PCH_NMI_EN, 1, &Save_Port70);

  return EFI_SUCCESS;
}

/**

 This hook will allow any platform or chipset specific programming to enable/disable WHEA error signaling.
 Note that this hook will be called at BIOS runtime (in SMM) during OS load. For e.g. setting super I/O or ICH9 GPIO for SCI trigger.

 @param EnableWhea - This Boolean indicates whether to enable (TRUE) or disable (FALSE) WHEA error signaling & etc.
 @param SmmBase    - Pointer to EFI_SMM_BASE2_PROTOCOL. This protocol is used to locate the SMST during SMM driver initialization.
 @param Smst       -  Pointer to System Management Service Table. This can be used access I/O functions.

 @retval EFI_SUCCESS - If programming in HW is success.
 @retval EFI_DEVICE_ERROR - If failed to program the HW.

 **/
EFI_STATUS ProgramWheaSignalsHook(
  BOOLEAN                 EnableWhea,
  EFI_SMM_BASE2_PROTOCOL  *SmmBase, 
  EFI_SMM_SYSTEM_TABLE2  *Smst
 ) 

{
  //
  // The GPIO PAD for Whea is  GPIO_SKL_H_GPP_A12. 
  // It is enabled to generate SCI in PEIPlatformHookLib
  // Keeping this function here as a place hodler if we expect to do anything later
  //
  return EFI_SUCCESS;
}

/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

 Copyright (c) 2009-2010 Intel Corporation.  All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license.  Except as permitted by such
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.

 ---------------------------------------------------------------------------**/
