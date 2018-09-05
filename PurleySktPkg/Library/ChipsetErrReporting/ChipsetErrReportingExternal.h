/** @file 
  This is Chipset Error Reporting library Private Interface 
  
Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/
#ifndef _CHIPSET_ERR_REPORTING_EXTERNAL_H_
#define _CHIPSET_ERR_REPORTING_EXTERNAL_H_

//Functions declared here are not meant to be in this library. 
//This functions need an external definition(protocol/library) or refactor. 

/**
  This function checks if a PCI or PCIe device exists at the specified PCI address.
  
  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number.

  @retval    BOOLEAN        True   Device present at the specified address.
                            False  Device not present at the specified address.
**/  
BOOLEAN
PciErrLibIsDevicePresent (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  This function will verify whether a port is PCIe root port
  
  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number.

  @retval    BOOLEAN        True   if PCIe is root port.
                            False  if PCIe is not root port.
**/  
BOOLEAN
PCIeIsRootPort(
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );


/**
  This function obtains the extended PCI configuration space register offset for a
  specified Extended Capability for the specified PCI device.
  
  @param[in] Socket               Device's socket number.
  @param[in] Bus                  Device's bus number.
  @param[in] Device               Device's device number.
  @param[in] Function             Device's function number.
  @param[in] ExtCapabilityId      ID of the desired Extended Capability.
  @param[in] VendoreSpecificId    vendor ID that is at offset ExtCapabilityoffset+4.

  @retval    Value                0         PCIe extended capability ID not found
                                  non-zero  Extended PCI configuration space offset of the specified Ext Capability block
**/  
UINT16 
PciErrLibGetExtendedCapabilitiesOffset (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function,
  IN UINT16  ExtCapabilityId,
  IN UINT16  VendorSpecificId
  );

/**
  This function initializes and enables error reporting in the specified PCI or PCIe device.
  
  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/
VOID
PciErrLibEnableElogDevice (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  This function checks if a PCI device is present on the specified bus.

  @param[in] Bus        Device's bus number

  @retval TRUE        A PCI device was found on the bus

**/
 BOOLEAN
PciErrLibDeviceOnBus (
  IN UINT8   Bus
  );

/**
  This function initializes and enables error reporting in the specified PCI or PCIe
  device and all subordinate PCI or PCIe devices.
  
  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/
VOID
PciErrLibEnableElogDeviceAndSubDevices (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  Clears Extended IIO Errors.

  This function clears Extended IIO status registers.

  @param[in] Socket      The Socket number.
  @param[in] Bus         The Bus number.
  @param[in] Device      The Device number.
  @param[in] Function    The Function number.

  @retval  None.
**/
VOID
ClearIohExtPcieDeviceStatus(
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
);

/**
  This function clears the legacy and standard PCI Express status registers for the
  specified PCI Express device.
  
  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number.

  @retval    None
**/
VOID
PciErrLibClearPcieDeviceStatus (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  This function masks PCI exp errors reporting.

  @retval    Status.
**/ 
EFI_STATUS
IohPcieMaskErrors(
  VOID
  );

/**
  This function handles IIO PCI exp errors.

  @param[in] Ioh            The IOH number.

  @retval    Status.
**/
EFI_STATUS
IohPcieErrorHandler (
  IN      UINT8   Ioh,
  IN      UINT8   IioStack
  );

/**
  This function checks if a PCI-to-PCI bridge exists at the specified PCI address.
  
  @param[in] Socket      Device's socket number.
  @param[in] Bus         Device's bus number.
  @param[in] Device      Device's device number.
  @param[in] Function    Device's function number.

  @retval  BOOLEAN       TRUE  - P2P present at the specified address.
                         FALSE - P2P not present at the specified address.
**/  
BOOLEAN
PciErrLibIsPciBridgeDevice (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
EFI_STATUS
IioDmiErrorEnable (
  VOID);



#endif //_CHIPSET_ERR_REPORTING_EXTERNAL_H_
