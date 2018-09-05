/** @file
  Contains definitions relevant to the PCI Express Error Handler.

  Copyright (c) 2009-2016 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/

#ifndef _PCIE_ERROR_HANDLER_H
#define _PCIE_ERROR_HANDLER_H


#include <Iio/IioRegs.h>
#include <Protocol/CpuCsrAccess.h>
#include <PciExpress.h>
#include <IIO_RAS.h>
#include <Library/PciLib.h>
#include <PchAccess.h>
#include "IIoErrorHandler.h"
#include "PchCommonErrorHandler.h"
#include <Setup/IioUniversalData.h>
#include <Library/ChipsetErrReporting.h>
#include <Library/MmPciBaseLib.h>

#define PCI_MAX_DEVICE                              31
#define PCI_MAX_FUNC                                7
#define PCI_ERROR_COUNT_MAX_THRESHOLD               80

#define GET_DEVSTS_OFFSET(Socket, Bus, Dev, Func) (((Socket) == 0 && (Bus) == DMI_BUS_NUM && (Dev) == DMI_DEV_NUM && (Func) == DMI_FUNC_NUM) ? DMI_DEV_STS_OFFSET : PCIE_DEVICE_STATUS_OFFSET)
#define GET_DEVCTL_OFFSET(Socket, Bus, Dev, Func) (((Socket) == 0 && (Bus) == DMI_BUS_NUM && (Dev) == DMI_DEV_NUM && (Func) == DMI_FUNC_NUM) ? DMI_DEV_CTL_OFFSET : PCIE_DEVICE_CONTROL_OFFSET)

#define AER_CAPABILITY_SIZE                         0x38
//
// defined in PCI-to-PCI Bridge Architecture Specification
//
#define PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET      0x18   
#define PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET    0x19   
#define PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET  0x1a
#ifndef PCI_BRIDGE_STATUS_REGISTER_OFFSET
#define PCI_BRIDGE_STATUS_REGISTER_OFFSET           0x1e
#endif
#ifndef PCI_BRIDGE_CONTROL_REGISTER_OFFSET
#define PCI_BRIDGE_CONTROL_REGISTER_OFFSET          0x3e
#endif
#define EFI_PCI_CAPABILITY_PTR                      0x34
#define EFI_PCI_CAPABILITY_ID_PCIEXP                0x10
#define EFI_PCI_BRIDGE_SUBTRACTIVE_DEV_CLASS_CODE   0x06040100
#define EFI_PCI_BRIDGE_DEV_CLASS_CODE               0x06040000
#define PCI_REVISION_ID_OFFSET                      0x08
// APTIOV_SERVER_OVERRIDE_RC_START : Dmi error logging broken
#define DMI_DEV_CTL_OFFSET                          0x60
#define DMI_DEV_STS_OFFSET                          0x62
// APTIOV_SERVER_OVERRIDE_RC_END : Dmi error logging broken

#define MSR_MCG_CONTAIN                             0x178

#define PCIE_CORRECTABLE_ERROR_COUNT_LIMIT          10

#define PCIE_EXT_CAP_HEADER_AERC                    0x0001
#define PCIE_ENHANCED_CAPABILITY_PTR                0x0100
#define PCI_EXP_CAP_SLCAP                           0x14                 // Slot capabilities
#define PCIE_EXT_CAP_HEADER_RPPIOERRC               0x0007
#define PCIE_EXT_CAP_HEADER_RPPIOERRC_REVISION      0x0000

#define PCIEAER_CORRERR_MSK        0x000031c1
#define PCIEAER_UNCORRERR_MSK      0x003ff030

#define PCIE_DEVSTS_OFFSET         0x9a
#define PCIE_AER_UNCERRSTS_OFFSET  0x04
#define PCIE_AER_UNCERRMSK_OFFSET  0x08
#define PCIE_AER_UNCERRSEV_OFFSET  0x0c
#define PCIE_AER_CORERRSTS_OFFSET  0x10
#define PCIE_AER_CORERRMSK_OFFSET  0x14
#define PCIE_AER_ROOTERRSTS_OFFSET 0x30

#define HEADER_LAYOUT_CODE                          0x7f

// 2.5.2. Advanced Error Reporting Enhanced Capability Registers...
//
#define R_PCIE_EXT_CAP_HEADER                       0x148
//

// Invalid header, used for terminating list...
//
#define PCIE_EXT_CAP_HEADER_INVALID                 0xFFFF

// capabilities list parsing support
#define  DWORD_MSK          3 // BIT1 | BIT0
#define  PCIE_EXT_CAP_HEADER_VSEC 0x00B  // Vendor-Specific Extended Capability ID
#define  PCIE_EXT_VENDOR_SPECIFIC_HEADER_OFFSET 0x4

// PCIe r3.0 section 7.9
#define  EX_CAP_OFFSET_BASE  0x100
#define  EX_CAP_OFFSET_LIMIT 0x1000
#define  EX_CAP_MIN_SIZE     4
#define  MAX_EX_CAPABILITIES (EX_CAP_OFFSET_LIMIT - EX_CAP_OFFSET_BASE) / EX_CAP_MIN_SIZE
#define  EXCAPABILITIES_MSK  0xFFF

// PCIe r3.0 section 7.9
#define  CAP_OFFSET_BASE   0x40
#define  CAP_OFFSET_LIMIT  0x100
// PCI r2.2 figure 6-10
#define  CAP_MIN_SIZE      4
#define  MAX_CAPABILITIES (CAP_OFFSET_LIMIT - CAP_OFFSET_BASE) / CAP_MIN_SIZE
#define  CAPABILITIES_MSK  0xFF

//
// ESS_OVERRIDE_START
//
#define EFI_PCI_STATUS_DETECTED_PARITY_ERROR        BIT15 // 0x8000
#define EFI_PCI_STATUS_SIGNALED_SYSTEM_ERROR        BIT14 // 0x4000
#define EFI_PCI_STATUS_RECEIVED_MASTER_ABORT        BIT13 // 0x2000
#define EFI_PCI_STATUS_RECEIVED_TARGET_ABORT        BIT12 // 0x1000
#define EFI_PCI_STATUS_DATA_PARITY_ERROR_DETECTED   BIT8  // 0x0100

#define SPARESWWEST_BOXINST                             7

#define CALC_EFI_PCIEX_ADDRESS(Bus, Dev, Func, ExReg) ( \
      (UINT64) ((((UINTN) Bus) << 24) + (((UINTN) Dev) << 16) + (((UINTN) Func) << 8) + (LShiftU64 ((UINT64) ExReg, 32))) \
   )
#define EFI_PCI_WHEA_ADDRESS(bus, dev, func, reg) \
    ((UINTN) ((((UINTN) bus) << 20) + (((UINTN) dev) << 15) + (((UINTN) func) << 12) + ((UINTN) reg)))

typedef struct {
  UINT32                ExtCapId:16;
  UINT32                ChanelVersion:4;
  UINT32                CapabilityOffset:12;
} PCIE_EXT_CAP;

typedef struct {
  UINT32                ExtCapId:16;
  UINT32                CapabilityVersion:4;
  UINT32                CapabilityOffset:12;
} PCIE_EXTENDED_CAPABILITY;


  extern IOH_INFO                        mIohInfo[MAX_SOCKET];
  extern EFI_CPU_CSR_ACCESS_PROTOCOL     *mCpuCsrAccess;
  extern IIO_UDS                         *mIioUds;
  extern UINT8                           mPciXPUnsupportedRequest;
  extern BOOLEAN                         mAllowClearingOfUEStatus;
  extern EFI_GUID                        gErrRecordNotifyPcieGuid;
  extern EFI_GUID                        gErrRecordPcieErrGuid;
  extern BOOLEAN                         mPcieFatalErrDetected;
  extern BOOLEAN                         mPcieNonFatalErrDetected;
  extern SYSTEM_RAS_SETUP                *mRasSetup;
  extern EFI_RAS_SYSTEM_TOPOLOGY         *mRasTopology;
  extern BOOLEAN                         mAllowClearingOfPcieDeviceStatus;
  extern BOOLEAN                         mNumCorrectablePcieErrs;
  extern BOOLEAN                         mNmiOnSerr;
  extern BOOLEAN                         mNmiOnPerr;
  extern EFI_SMM_GPI_DISPATCH2_PROTOCOL *mGpiDispatch;
  extern SYSTEM_RAS_CAPABILITY                 *mRasCapability;


////////////////////////////////////// COMMON ROUTINES /////////////////////////////////////////

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
  This function clears the specified device's primary and secondary (if appropriate) 
  legacy PCI status registers.
  
  @param[in] Socket      Device's socket number.
  @param[in] Bus         Device's bus number.
  @param[in] Device      Device's device number.
  @param[in] Function    Device's function number.

  @retval  None.
**/
VOID
PciErrLibClearLegacyPciDeviceStatus (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  This function clears the legacy and standard PCI Express status registers for the
  specified PCI Express device.
  
  @param[in] Socket         Device's socket number
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
   This function obtains the PCI configuration space register offset of the specified 
   Capabilities register set.
  
  @param[in] Socket      Device's socket number.
  @param[in] Bus         Device's bus number.
  @param[in] Device      Device's device number.
  @param[in] Function    Device's function number.
  @param[in] Cap         Desired Capability structure

  @retval  CapabilityOffset.
**/
UINT8 
PciErrLibGetCapabilitiesOffset (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function,
  IN      UINT8   Cap 
  );

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
////////////////////////////////////// ERROR HANDLING ROUTINES /////////////////////////////////////////

/**
  This function determines if the specified device is reporting an error and handles the
  error if one is detected.

  @param[in] Socket         PCIe deivice's socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number. 
    
  @retval    BOOLEAN        TRUE   - An error was detected.
                            FALSE  - An error was not detected.
--*/ 
BOOLEAN
PciErrLibIsPcieDevice (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

  /**
  This function determines if the specified device is reporting an error and handles the
  error if one is detected.

  @param[in] Socket         PCIe device's socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number. 
    
  @retval    BOOLEAN        TRUE   - An error was detected.
                            FALSE  - An error was not detected.
--*/
BOOLEAN
PciErrLibPcieDeviceErrorHandler (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  ); 

/**
  This function checks the SERR and PCIe device status to see if a PCIe Uncorrectable Fatal
  error has occurred.

  @param[in] Socket         PCIe device's socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number. 
    
  @retval    BOOLEAN        TRUE   - A PCIe Uncorrectable Fatal error was detected.
                            FALSE  - A PCIe Uncorrectable Fatal error was not detected.
--*/
BOOLEAN
PciErrLibIsFatalPcieDeviceError (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  This function checks the SERR and PCIe device status to see if a Uncorrectable Non-Fatal
  PCIe error has occurred.

  @param[in] Socket         PCIe device's socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number. 
    
  @retval    BOOLEAN        TRUE   - A PCIe Uncorrectable Non-Fatal error was detected.
                            FALSE  - A PCIe Uncorrectable Non-Fatal error was not detected.
--*/
BOOLEAN
PciErrLibIsNonfatalPcieDeviceError (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  This function determines if there is a device reporting an error on the specified 
  P2P bridge's secondary bus or on a subordinate bus.

  @param[in] Socket         P2P bridge's socket number.
  @param[in] Bus            P2P bridge's bus number.
  @param[in] Device         P2P bridge's device number.
  @param[in] Function       P2P bridge's function number. 
    
  @retval    BOOLEAN        TRUE   - An error was detected.
                            FALSE  - An error was not detected.
--*/
BOOLEAN
PciErrLibIsSubordinateDeviceError (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  ); 

/**
  This function handles PCIe Uncorrectable (Fatal and Non-Fatal) errors.
  An error is reported and the PCIe error detection status is updated.

  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/  
VOID
PciErrLibHandleUncorrectablePcieDeviceError (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  This function handles PCIe Correctable errors.
  An error is reported if the correctable error threshold has not been reached.
  If the threshold has been reached, correctable errors are masked in the device.

  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/
VOID
PciErrLibHandleCorrectablePcieDeviceError (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  This function checks the PCIe device status and the correctable error registers to see 
  if a PCIe Correctable error has occurred.

  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    BOOLEAN        TRUE  - A PCIe Correctable error was detected.
                            FALSE - A PCIe Correctable error was not detected.
--*/
BOOLEAN
PciErrLibIsCorrectablePcieDeviceError (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  This function checks for errors on standard PCI devices.
    
  @param[in] Socket         Device's sokcet number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    BOOLEAN        TRUE     - An error was detected.
                            FALSE    - An error was not detected.
--*/  
BOOLEAN
PciErrLibIsPciDeviceError (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  This function checks for, and handles, errors on standard PCI devices.

  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    BOOLEAN        TRUE     - An error was detected.
                            FALSE    - An error was not detected.
--*/
BOOLEAN
PciErrLibPciDeviceErrorHandler (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  This function fills the PCIE error record fields logs the 
  appropriate type of error by calling mPlatformErrorHandlingProtocol.
  
  @oaram[in[ Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/
VOID
PcieLogReportError (
  IN UINT8                    Socket,
  IN UINT8                    Bus,
  IN UINT8                    Device,
  IN UINT8                    Function,
  IN UINT8                    PciErrorType
  );
  
/**
  This function checks to see if PCIe error reporting is enabled in the specified 
  PCIe root port device.

  @param[in] Socket         PCIe root port device's socket number.
  @param[in] Bus            PCIe root port device's bus number.
  @param[in] Device         PCIe root port device's device number.
  @param[in] Function       PCIe root port device's function number. 
    
  @retval    BOOLEAN        TRUE   - Error reporting is enabled.
                            FALSE  - Error reporting is not enabled or it is under OS control.
--*/  
BOOLEAN
PciErrLibIsRootPortErrReportingEnabled (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );

/**
  PCI Express error handler for ESB2 and BNB PCI Express Root Ports.

  @param[in] RootPortSocket         PCIe root port device's socket number.
  @param[in] RootPortBus            PCIe root port device's bus number.
  @param[in] RootPortDevice         PCIe root port device's device number.
  @param[in] RootPortFunction       PCIe root port device's function number. 
    
  @retval    BOOLEAN        TRUE   - An error was detected.
                            FALSE  - An error was not detected.
--*/
BOOLEAN
PciErrLibPcieRootPortErrorHandler (
  IN UINT8   RootPortSocket,
  IN UINT8   RootPortBus,
  IN UINT8   RootPortDevice,
  IN UINT8   RootPortFunction
  );

/**
  This function checks to see if PCIe error reporting is enabled in the specified 
  PCIe root port device.

  @param[in] Socket         PCIe root port deivce's socket number.
  @param[in] Bus            PCIe root port device's bus number.
  @param[in] Device         PCIe root port device's device number.
  @param[in] Function       PCIe root port device's function number. 
    
  @retval    BOOLEAN        TRUE   - Error reporting is enabled.
                            FALSE  - Error reporting is not enabled or it is under OS control.
--*/
BOOLEAN
PciErrLibIsRootPortErrReportingEnabled (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  );
 
////////////////////////////////////// INITIALIZATION ROUTINES /////////////////////////////////////////

/**
  Enable error log for PCH.
    
  @retval EFI_SUCCESS.
**/
EFI_STATUS
EnableElogPCH (
  VOID
  );

/**
  This function enables SERR and Parity error reporting in the specified device's 
  legacy PCI command register.
  
  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/
VOID
PciErrLibEnablePciSerrPerrGeneration (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
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
  This function is used to register the function that enable the IIO error reporting.
  
  @retval    Status.
**/  
EFI_STATUS
EnableElogIoHub (
  VOID
  );

/**
  Enable AsyncSMI Logic.
    
  @retval EFI_SUCCESS.
  
**/  
 EFI_STATUS
EnableAsyncSmiLogic (
  VOID
  );

/*++

Routine Description:
    This function initializes and enables error reporting in the specified PCI or PCIe
    device and all subordinate PCI or PCIe devices.

Arguments:
    Socket   - Device's socket number.
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number

Returns:
    Nothing

--*/

UINT16 
PciErrLibGetExtendedCapabilitiesOffset (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function,
  IN UINT16  ExtCapabilityId,
  IN UINT16  VendorSpecificId
  );

#endif
