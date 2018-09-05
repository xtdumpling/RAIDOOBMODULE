/**
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
**/
/**

Copyright (c)  2015 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PlatformOpromPolicyLib.h
    
Revision History:

**/

#ifndef _PLAT_OPROM_
#define _PLAT_OPROM_
#include <Protocol/PciIo.h>

typedef enum _OPROM_LOAD_POLICY
{
  DONT_LOAD,
  EXCLUSIVE_LOAD,
  INCLUSIVE_LOAD
} OPROM_LOAD_POLICY;

/**
  Decide if BIOS embdded option roms should be loaded for a certain PCI device.

  @param  PciIo      PCI device to return the ROM image for.

  @retval TRUE       BIOS embedded option roms should not be loaded for the PCI device.
  @retval FALSE      BIOS embedded option roms could be loaded for the PCI device.
**/

BOOLEAN
PlatformOpromLoadDevicePolicy (
  IN EFI_PCI_IO_PROTOCOL *PciIo
  );

/**
  For devices that support multiple option roms like FCoE, PXE, iSCSI etc., this function decides if one of these BIOS embdded option roms should be loaded for a certain PCI device based on platform choices.

  @param  PciHandle      PCI device to return the ROM image for.
  @param  TableIndex     The index pointing to the option rom in the platform option rom table for the PCI device.

  @retval FALSE          The specific BIOS embedded option rom should not be loaded for the PCI device.
  @retval TRUE           The specific BIOS embedded option rom could be loaded for a certain PCI device.
**/

OPROM_LOAD_POLICY
PlatformOpromLoadTypePolicy (
  IN EFI_HANDLE PciHandle,
  IN UINTN      TableIndex
  );

/**
  Decide if a PCIe device option rom should be dispacthed.

  @param  PciHandle      PCI device handle.

  @retval FALSE          The specific PCIe option rom should not be dispatched for the PCI device. 
  @retval TRUE           The specific PCIe option rom could be dispatched for a certain PCI device.

**/

BOOLEAN
PlatformOpromDispatchPolicy (
  IN  EFI_HANDLE                        DeviceHandle
);

/**
  Enable the legacy console redirection before dispatch the legacy ORPOM or disable the legacy console redirection after dispatch 
  the legacy ORPOM based on setup option and SOL status.

  @param  Mode             Subfunction.
  @param  CheckIsAhciRom   If the device is legacy Ahci device.
  
  @retval 

**/

VOID
PlatformOpromLegacyCRPolicy (
  IN     UINTN                           Mode,
  IN     BOOLEAN                         CheckIsAhciRom
);


#endif
