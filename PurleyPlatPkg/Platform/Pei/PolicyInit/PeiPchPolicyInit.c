/** @file
  This file is SampleCode for Intel PCH PEI Policy initialzation.

@copyright
 Copyright (c) 2009 - 2014 Intel Corporation. All rights reserved
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
#include "PeiPchPolicyInit.h"

#define PCI_CLASS_NETWORK             0x02
#define PCI_CLASS_NETWORK_ETHERNET    0x00
#define PCI_CLASS_NETWORK_OTHER       0x80


GLOBAL_REMOVE_IF_UNREFERENCED PCH_PCIE_DEVICE_OVERRIDE mPcieDeviceTable[] = {
  //
  // Intel PRO/Wireless
  //
  { 0x8086, 0x422b, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x422c, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x4238, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x4239, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  //
  // Intel WiMAX/WiFi Link
  //
  { 0x8086, 0x0082, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0085, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0083, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0084, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0086, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0087, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0088, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0089, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x008F, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0090, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  //
  // Intel Crane Peak WLAN NIC
  //
  { 0x8086, 0x08AE, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x08AF, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  //
  // Intel Crane Peak w/BT WLAN NIC
  //
  { 0x8086, 0x0896, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0897, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  //
  // Intel Kelsey Peak WiFi, WiMax
  //
  { 0x8086, 0x0885, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0886, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  //
  // Intel Centrino Wireless-N 105
  //
  { 0x8086, 0x0894, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0895, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  //
  // Intel Centrino Wireless-N 135
  //
  { 0x8086, 0x0892, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0893, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  //
  // Intel Centrino Wireless-N 2200
  //
  { 0x8086, 0x0890, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0891, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  //
  // Intel Centrino Wireless-N 2230
  //
  { 0x8086, 0x0887, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x0888, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  //
  // Intel Centrino Wireless-N 6235
  //
  { 0x8086, 0x088E, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x088F, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  //
  // Intel CampPeak 2 Wifi
  //
  { 0x8086, 0x08B5, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  { 0x8086, 0x08B6, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },
  //
  // Intel WilkinsPeak 1 Wifi
  //
  { 0x8086, 0x08B3, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2AndL1SubstatesOverride, 0x0154, 0x00000003 },
  { 0x8086, 0x08B3, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1SubstatesOverride,        0x0158, 0x00000003 },
  { 0x8086, 0x08B4, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2AndL1SubstatesOverride, 0x0154, 0x00000003 },
  { 0x8086, 0x08B4, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1SubstatesOverride,        0x0158, 0x00000003 },
  //
  // Intel Wilkins Peak 2 Wifi
  //
  { 0x8086, 0x08B1, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2AndL1SubstatesOverride, 0x0154, 0x00000003 },
  { 0x8086, 0x08B1, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1SubstatesOverride,        0x0158, 0x00000003 },
  { 0x8086, 0x08B2, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2AndL1SubstatesOverride, 0x0154, 0x00000003 },
  { 0x8086, 0x08B2, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1SubstatesOverride,        0x0158, 0x00000003 },
  //
  // Intel Wilkins Peak PF Wifi
  //
  { 0x8086, 0x08B0, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0 },

  //
  // End of Table
  //
  { 0 }
};

STATIC
EFI_STATUS
InstallPcieDeviceTable (
  IN    PCH_PCIE_DEVICE_OVERRIDE         *DeviceTable
  )
{
  EFI_PEI_PPI_DESCRIPTOR  *DeviceTablePpiDesc;
  EFI_STATUS               Status;

  DeviceTablePpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (DeviceTablePpiDesc != NULL);

  if (DeviceTablePpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  DeviceTablePpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  DeviceTablePpiDesc->Guid  = &gPchPcieDeviceTablePpiGuid;
  DeviceTablePpiDesc->Ppi   = DeviceTable;

  Status = PeiServicesInstallPpi (DeviceTablePpiDesc);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  This PEIM performs PCH PEI Policy initialzation.

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.

  @retval     EFI_SUCCESS            The PPI is installed and initialized.
  @retval     EFI ERRORS             The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES   Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
PeiPchPolicyInit (
  IN UINT8                     FirmwareConfiguration,
  IN SYSTEM_CONFIGURATION      *SystemConfiguration,
  IN PCH_RC_CONFIGURATION      *PchRcConfiguration
  )
{
  EFI_STATUS                    Status;
  PCH_POLICY_PPI               *PchPolicyPpi;

  //
  // Call PchCreatePolicyDefaults to initialize platform policy structure
  // and get all intel default policy settings.
  //
  Status = PchCreatePolicyDefaults (&PchPolicyPpi);
  ASSERT_EFI_ERROR (Status);
  //
  // Update and override all platform related and customized settings below.
  //
  UpdatePeiPchPolicy (PchPolicyPpi, SystemConfiguration, PchRcConfiguration);
  if ((FirmwareConfiguration >= FwConfigTest) && (FirmwareConfiguration < FwConfigMax)) {
    // Update Debug Policies
  }

  //
  // Install PchPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = PchInstallPolicyPpi (PchPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Install PCIe device override table
  //
  InstallPcieDeviceTable (mPcieDeviceTable);

  return Status;
}

