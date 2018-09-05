/**@file

@copyright
 Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#include "MeSmbios.h"
#include <IndustryStandard/Pci30.h>
#include <Library/MmPciBaseLib.h>
#include <Library/PchInfoLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Library/SetupLib.h>
#include <Guid/SocketIioVariable.h>
#include <Guid/SocketProcessorCoreVariable.h>
#include "CpuDataStruct.h"
#include <Library/MbpDataLib.h>

UINT16
Swap16 (
  IN  UINT16    Value
  )
/*++

Routine Description:

  16 bits are changed backward for string transfer to value used

Arguments:

  Value - The value to be transferred

Returns:

  UINT16 Value - The value after transferring

--*/
{
  UINT16  OutValue;
  UINT8   *TempIn;
  UINT8   *TempOut;

  TempIn      = (UINT8 *) &Value;
  TempOut     = (UINT8 *) &OutValue;

  TempOut[0]  = TempIn[1];
  TempOut[1]  = TempIn[0];

  return OutValue;
}

MISC_SUBCLASS_TABLE_FUNCTION(MiscOemType0x83)
/*++
Description:

  This function makes boot time changes to the contents of the
  MiscOemType0x83Data.

Parameters:

  RecordType
    Type of record to be processed from the Data Table.
    mMiscSubclassDataTable[].RecordType

  RecordLen
    pointer to the size of static RecordData from the Data Table.
    mMiscSubclassDataTable[].RecordLen

  RecordData
    Pointer to copy of RecordData from the Data Table.  Changes made
    to this copy will be written to the Data Hub but will not alter
    the contents of the static Data Table.

  LogRecordData
    Set *LogRecordData to TRUE to log RecordData to Data Hub.
    Set *LogRecordData to FALSE when there is no more data to log.

Returns:

  EFI_SUCCESS
    All parameters were valid and *RecordData and *LogRecordData have
    been set.

  EFI_UNSUPPORTED
    Unexpected RecordType value.

  EFI_INVALID_PARAMETER
    One of the following parameter conditions was true:
      RecordLen was NULL.
      RecordData was NULL.
      LogRecordData was NULL.
--*/
{
  EFI_STATUS                                Status;
  UINT64                                    Ia32FeatureControl;
  EFI_CPUID_REGISTER                        CpuidRegs;
  UINT32                                    MeEnabled;
  UINT8                                     VTdSupportOption;
  UINT8                                     ProcessorVmxEnableOption;
  UINT8                                     ProcessorSmxEnableOption;
  MEBX_PROTOCOL                             *MebxProtocol;
  HECI_PROTOCOL                             *Heci;
  EFI_SMBIOS_PROTOCOL                       *Smbios;
  ProcessorSmxEnableOption = 0;

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Could not locate SMBIOS protocol.  %r\n", Status));
    return Status;
  }

  //
  // First check for invalid parameters.
  //
  if ((RecordLen == NULL) || (RecordData == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Then check for unsupported RecordType.
  //
  if (RecordType != EFI_MISC_OEM_TYPE_0x83_RECORD_NUMBER) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((DEBUG_INFO, "MiscOemType0x83 entry \n"));
  //
  // Locate HECI protocol to use it for ME info, HECI messaging
  // If HECI not found register callback for it to update data
  //
  Status = gBS->LocateProtocol (
                &gHeciProtocolGuid,
                NULL,
                &Heci
                );
  if (!(EFI_ERROR(Status))) {
    ZeroMem (&((EFI_MISC_OEM_TYPE_0x83*)RecordData)->MeCapabilities, sizeof (ME_CAP));
    MeEnabled = *((VOLATILE UINT32 *)(UINTN)((PciMeRegBase) + FW_STATUS_REGISTER));
    //
    // Make sure Me is in normal mode & hasn't any error
    //
    if ((MeEnabled & 0xFF000) == 0) {
      Status = MbpGetMeFwInfo(&((EFI_MISC_OEM_TYPE_0x83 *)RecordData)->MeCapabilities);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "MiscOemType0x83 exit (no MeFwInfo)\n"));
        return Status;
      }
    }
  }

  ((EFI_MISC_OEM_TYPE_0x83 *)RecordData)->Header.Length = 0x40;
  CopyMem (
    ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->StructureIdentifier,
    vPro_Signature,
    sizeof (UINT32)
  );


  Ia32FeatureControl = AsmReadMsr64(EFI_MSR_IA32_FEATURE_CONTROL);
  AsmCpuid (
          1,
          &CpuidRegs.RegEax,
          &CpuidRegs.RegEbx,
          &CpuidRegs.RegEcx,
          &CpuidRegs.RegEdx
          );

  //
  // CPU
  //
  ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->CpuCapabilities.VMXState = (UINT32)(RShiftU64(Ia32FeatureControl, 2));
  ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->CpuCapabilities.SMXState = (UINT32)(RShiftU64(Ia32FeatureControl, 1));

  if (CpuidRegs.RegEcx & BIT6) {
    ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->CpuCapabilities.LtTxtCap = 1;
  } else {
    ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->CpuCapabilities.LtTxtCap = 0;
  }

  if ((Ia32FeatureControl & TXT_OPT_IN_VMX_AND_SMX_MSR_VALUE) == TXT_OPT_IN_VMX_AND_SMX_MSR_VALUE) {
    ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->CpuCapabilities.LtTxtEnabled = 1;
  } else {
    ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->CpuCapabilities.LtTxtEnabled = 0;
  }

  if (CpuidRegs.RegEcx & BIT5) {
    ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->CpuCapabilities.VTxCap = 1;
  } else {
    ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->CpuCapabilities.VTxCap = 0;
  }
  ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->CpuCapabilities.VTxEnabled = (UINT32)(RShiftU64(Ia32FeatureControl, 2));

  //
  // MEBx Version
  //
  // Allocate RAM to put MEBx 16 bit real mode binary data at
  // based on size returned above.
  //
 //Locate MEBx protocol to get MEBx version information.
 Status = gBS->LocateProtocol(&gMebxProtocolGuid, NULL, &MebxProtocol);
 if(!EFI_ERROR(Status)){
  ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->MebxVersion.Major = MebxProtocol->MebxVersion.Major;
  ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->MebxVersion.Minor = MebxProtocol->MebxVersion.Minor;
  ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->MebxVersion.Hotfix = MebxProtocol->MebxVersion.Hotfix;
  ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->MebxVersion.Build = MebxProtocol->MebxVersion.Build;
 }

 //
 // Pch
 ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->PchCapabilities.FunctionNumber = PCI_FUNCTION_NUMBER_PCH_LPC;
 ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->PchCapabilities.DeviceNumber = PCI_DEVICE_NUMBER_PCH_LPC;
 ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->PchCapabilities.BusNumber = DEFAULT_PCI_BUS_NUMBER_PCH;
 ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->PchCapabilities.DeviceID = MmioRead16 (
                                                                     MmPciBase (
                                                                     DEFAULT_PCI_BUS_NUMBER_PCH,
                                                                     PCI_DEVICE_NUMBER_PCH_LPC,
                                                                     PCI_FUNCTION_NUMBER_PCH_LPC
                                                                     ) + PCI_DEVICE_ID_OFFSET
                                                                     );

 //
 // NetworkDevice
 //
 ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->NetworkDevice.FunctionNumber = PCI_FUNCTION_NUMBER_PCH_LAN; //[2:0] PCI Device Function Number of Wired LAN
 ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->NetworkDevice.DeviceNumber = PCI_DEVICE_NUMBER_PCH_LAN;     //[7:3] PCI Device Device Number of Wired LAN
 ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->NetworkDevice.BusNumber = DEFAULT_PCI_BUS_NUMBER_PCH;       //[15:8] PCI Device Bus Number of Wired LAN
 ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->NetworkDevice.DeviceID = MmioRead16(
                                                                   MmPciBase (
                                                                   DEFAULT_PCI_BUS_NUMBER_PCH,
                                                                   PCI_DEVICE_NUMBER_PCH_LAN,
                                                                   PCI_FUNCTION_NUMBER_PCH_LAN
                                                                   ) + PCI_DEVICE_ID_OFFSET
                                                                   );

 //
 // BIOS
 //
  ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->BiosCapabilities.VTxSupport = 0;
  ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->BiosCapabilities.TxtSupport = 0;
  Status = GetOptionData (&gEfiSocketProcessorCoreVarGuid, OFFSET_OF(SOCKET_PROCESSORCORE_CONFIGURATION, ProcessorVmxEnable), &ProcessorVmxEnableOption, sizeof(ProcessorVmxEnableOption));
  if (!EFI_ERROR(Status)) {
    Status = GetOptionData (&gEfiSocketProcessorCoreVarGuid, OFFSET_OF(SOCKET_PROCESSORCORE_CONFIGURATION, ProcessorSmxEnable), &ProcessorSmxEnableOption, sizeof(ProcessorSmxEnableOption));
  }
  if (!EFI_ERROR(Status)) {
    if (ProcessorVmxEnableOption) {
      ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->BiosCapabilities.VTxSupport = 1;
    }
    if (ProcessorSmxEnableOption) {
      ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->BiosCapabilities.TxtSupport = 1;
    }
  }

  ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->BiosCapabilities.Reserved1   = 0;
  ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->BiosCapabilities.Reserved2   = 0;

  Status = GetOptionData (&gEfiSocketIioVariableGuid, OFFSET_OF(SOCKET_IIO_CONFIGURATION, VTdSupport), &VTdSupportOption, sizeof(VTdSupportOption));
  ASSERT_EFI_ERROR (Status);

  if ((!EFI_ERROR(Status)) && (VTdSupportOption)) {
    ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->BiosCapabilities.VTdSupport  = 1;  //Set to BIOS setup option display, not to feature enabled/disabled value
  } else {
    ((EFI_MISC_OEM_TYPE_0x83*)RecordData)->BiosCapabilities.VTdSupport  = 0;  //Set to BIOS setup option display, not to feature enabled/disabled value
  }

  DEBUG ((DEBUG_INFO, "MiscOemType0x83 exit \n"));

  return EFI_SUCCESS;
}

/* eof - MiscOemType0x83Function.c */
