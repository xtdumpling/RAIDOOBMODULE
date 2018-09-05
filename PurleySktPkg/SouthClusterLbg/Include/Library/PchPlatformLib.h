/** @file
  Header file for PchPlatform Lib.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

@copyright
 Copyright (c) 2008 - 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PCH_PLATFORM_LIB_H_
#define _PCH_PLATFORM_LIB_H_

#include <PchAccess.h>
#include <Uefi/UefiBaseType.h>

typedef struct {
  UINT8 DevNum;
  UINT8 Pid;
  UINT8 RpNumBase;
} PCH_PCIE_CONTROLLER_INFO;

/**
  Return Pch stepping type

  @retval PCH_STEPPING            Pch stepping type
**/
PCH_STEPPING
EFIAPI
PchStepping (
  VOID
  );

/**
  Determine if PCH is supported

  @retval TRUE                    PCH is supported
  @retval FALSE                   PCH is not supported
**/
BOOLEAN
IsPchSupported (
  VOID
  );

/**
  This function can be called to enable/disable Alternate Access Mode

  @param[in] AmeCtrl              If TRUE, enable Alternate Access Mode.
                                  If FALSE, disable Alternate Access Mode.
**/
VOID
EFIAPI
PchAlternateAccessMode (
  IN  BOOLEAN       AmeCtrl
  );

/**
  Check whether GbE region is valid
  Check SPI region directly since GBE might be disabled in SW.

  @retval TRUE                    Gbe Region is valid
  @retval FALSE                   Gbe Region is invalid
**/
BOOLEAN
EFIAPI
PchIsGbeRegionValid (
  VOID
  );

/**
  Returns GbE over PCIe port number based on a soft strap.

  @return                         Root port number (1-based)
  @retval 0                       GbE over PCIe disabled
**/
UINT32
EFIAPI
PchGetGbePortNumber (
  VOID
  );

/**
  Check whether LAN controller is present in this chipset SKU.

  @retval TRUE                    GbE is supported in this chipset
  @retval FALSE                   GbE is not supported
**/
BOOLEAN
EFIAPI
PchIsGbePresent (
  VOID
  );

/**
  Check whether GbE is available in the platform, i.e.
   - GbE is present in the chipset
   - GbE is enbaled with soft straps
   - GbE NVM region is valid

  @retval TRUE                    Gbe Region is valid
  @retval FALSE                   Gbe Region is invalid
**/
BOOLEAN
EFIAPI
PchIsGbeAvailable (
  VOID
  );


typedef enum {
  PchH          = 1,
  PchLp,
  PchUnknownSeries
} PCH_SERIES;

typedef enum {
  SklPch        = 1,
  PchUnknownGeneration
} PCH_GENERATION;

/**
  Return Pch Series

  @retval PCH_SERIES                Pch Series
**/
PCH_SERIES
EFIAPI
GetPchSeries (
  VOID
  );

/**
  Return TRUE if Server Sata is present 
  
  @retval BOOLEAN           TRUE if sSata device is present
**/  
BOOLEAN
EFIAPI
GetIsPchsSataPresent (
  VOID
  );

/**
  Get Pch Maximum sSata Port Number

  @param[in] None

  @retval Pch Maximum sSata Port Number
**/
UINT8
EFIAPI
GetPchMaxsSataPortNum (
  VOID
  );

/**
  Get Pch Maximum sSata Controller Number

  @param[in] None

  @retval Pch Maximum sSata Controller Number
**/
UINT8 
EFIAPI
GetPchMaxsSataControllerNum (
  VOID
  );

/**
  Return Pch Generation

  @retval PCH_GENERATION            Pch Generation
**/
PCH_GENERATION
EFIAPI
GetPchGeneration (
  VOID
  );

/**
  Return Pch Lpc Device Id

  @retval UINT16            Pch DeviceId
**/
UINT16
EFIAPI
GetPchLpcDeviceId (
  VOID
  );

/**
  Get Pch Maximum Pcie Root Port Number

  @retval PcieMaxRootPort         Pch Maximum Pcie Root Port Number
**/
UINT8
EFIAPI
GetPchMaxPciePortNum (
  VOID
  );

/**
  Get Pch Maximum Sata Port Number

  @retval Pch Maximum Sata Port Number
**/
UINT8
EFIAPI
GetPchMaxSataPortNum (
  VOID
  );

/**
  Get Pch Usb Maximum Physical Port Number

  @retval Pch Usb Maximum Physical Port Number
**/
UINT8
EFIAPI
GetPchUsbMaxPhysicalPortNum (
  VOID
  );

/**
  Get Pch Maximum Usb2 Port Number of XHCI Controller

  @retval Pch Maximum Usb2 Port Number of XHCI Controller
**/
UINT8
EFIAPI
GetPchXhciMaxUsb2PortNum (
  VOID
  );

/**
  Get Pch Maximum Usb3 Port Number of XHCI Controller

  @retval Pch Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
EFIAPI
GetPchXhciMaxUsb3PortNum (
  VOID
  );

typedef enum {
  PchWarmBoot          = 1,
  PchColdBoot,
  PwrFlr,
  PwrFlrSys,
  PwrFlrPch,
  PchPmStatusMax
} PCH_PM_STATUS;

/**
  Query PCH to determine the Pm Status

  @param[in] PmStatus - The Pch Pm Status to be probed

  @retval Status TRUE if Status querried is Valid or FALSE if otherwise
**/
BOOLEAN
GetPchPmStatus (
  PCH_PM_STATUS PmStatus
  );

/**
  Get Pch Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber            Root port physical number. (0-based)
  @param[out] RpDev               Return corresponding root port device number.
  @param[out] RpFun               Return corresponding root port function number.

  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
GetPchPcieRpDevFun (
  IN  UINTN   RpNumber,
  OUT UINTN   *RpDev,
  OUT UINTN   *RpFun
  );

/**
  Get Root Port physical Number by Pch Pcie Root Port Device and Function Number

  @param[in]  RpDev                 Root port device number.
  @param[in]  RpFun                 Root port function number.
  @param[out] RpNumber              Return corresponding physical Root Port index (0-based)

  @retval     EFI_SUCCESS           Physical root port is retrieved
  @retval     EFI_INVALID_PARAMETER RpDev and/or RpFun are invalid
  @retval     EFI_UNSUPPORTED       Root port device and function is not assigned to any physical root port
**/
EFI_STATUS
EFIAPI
GetPchPcieRpNumber (
  IN  UINTN   RpDev,
  IN  UINTN   RpFun,
  OUT UINTN   *RpNumber
  );

/**
  Get P2SB pci configuration register.
  It returns register at Offset of P2SB controller and size in 4bytes.
  The Offset should not exceed 255 and must be aligned with size.
  This function will be unavailable after P2SB is hidden by PSF.

  @param[in]  Offset                    Register offset of P2SB controller.
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchP2sbCfgGet32 (
  IN  UINTN                             Offset,
  OUT UINT32                            *OutData
  );

/**
  Get P2SB pci configuration register.
  It returns register at Offset of P2SB controller and size in 2bytes.
  The Offset should not exceed 255 and must be aligned with size.
  This function will be unavailable after P2SB is hidden by PSF.

  @param[in]  Offset                    Register offset of P2SB controller.
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchP2sbCfgGet16 (
  IN  UINTN                             Offset,
  OUT UINT16                            *OutData
  );

/**
  Get P2SB pci configuration register.
  It returns register at Offset of P2SB controller and size in 1byte.
  The Offset should not exceed 255 and must be aligned with size.
  This function will be unavailable after P2SB is hidden by PSF.

  @param[in]  Offset                    Register offset of P2SB controller.
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchP2sbCfgGet8 (
  IN  UINTN                             Offset,
  OUT UINT8                             *OutData
  );

/**
  Set P2SB pci configuration register.
  It programs register at Offset of P2SB controller and size in 4bytes.
  The Offset should not exceed 255 and must be aligned with size.
  This function will be unavailable after P2SB is hidden by PSF.

  @param[in]  Offset                    Register offset of P2SB controller.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchP2sbCfgSet32 (
  IN  UINTN                             Offset,
  IN  UINT32                            AndData,
  IN  UINT32                            OrData
  );

/**
  Set P2SB pci configuration register.
  It programs register at Offset of P2SB controller and size in 2bytes.
  The Offset should not exceed 255 and must be aligned with size.
  This function will be unavailable after P2SB is hidden by PSF.

  @param[in]  Offset                    Register offset of P2SB controller.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchP2sbCfgSet16 (
  IN  UINTN                             Offset,
  IN  UINT16                            AndData,
  IN  UINT16                            OrData
  );

/**
  Set P2SB pci configuration register.
  It programs register at Offset of P2SB controller and size in 1bytes.
  The Offset should not exceed 255 and must be aligned with size.
  This function will be unavailable after P2SB is hidden by PSF.

  @param[in]  Offset                    Register offset of P2SB controller.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchP2sbCfgSet8 (
  IN  UINTN                             Offset,
  IN  UINT8                             AndData,
  IN  UINT8                             OrData
  );

/**
  Get IO APIC regsiters base address.
  It returns IO APIC INDEX, DATA, and EOI regsiter address once the parameter is not NULL.
  This function will be unavailable after P2SB is hidden by PSF.

  @param[out] IoApicIndex               Buffer of IO APIC INDEX regsiter address
  @param[out] IoApicData                Buffer of IO APIC DATA regsiter address

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
PchIoApicBaseGet (
  OPTIONAL OUT UINT32                   *IoApicIndex,
  OPTIONAL OUT UINT32                   *IoApicData
  );

/**
  Get HPET base address.
  This function will be unavailable after P2SB is hidden by PSF.

  @param[out] HpetBase                  Buffer of HPET base address

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchHpetBaseGet (
  OUT UINT32                            *HpetBase
  );

/**
  Read PCR register.
  It returns PCR register and size in 4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchPcrRead32 (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            Offset,
  OUT UINT32                            *OutData
  );

/**
  Read PCR register.
  It returns PCR register and size in 2bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchPcrRead16 (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            Offset,
  OUT UINT16                            *OutData
  );

/**
  Read PCR register.
  It returns PCR register and size in 1bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchPcrRead8 (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            Offset,
  OUT UINT8                             *OutData
  );

/**
  Write PCR register.
  It programs PCR register and size in 4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchPcrWrite32 (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            Offset,
  IN  UINT32                            InData
  );

/**
  Write PCR register.
  It programs PCR register and size in 2bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchPcrWrite16 (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            Offset,
  IN  UINT16                            InData
  );

/**
  Write PCR register.
  It programs PCR register and size in 1bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchPcrWrite8 (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            Offset,
  IN  UINT8                             InData
  );

/**
  Write PCR register.
  It programs PCR register and size in 4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchPcrAndThenOr32 (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            Offset,
  IN  UINT32                            AndData,
  IN  UINT32                            OrData
  );

/**
  Write PCR register.
  It programs PCR register and size in 2bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchPcrAndThenOr16 (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            Offset,
  IN  UINT16                            AndData,
  IN  UINT16                            OrData
  );

/**
  Write PCR register.
  It programs PCR register and size in 1bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchPcrAndThenOr8 (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            Offset,
  IN  UINT8                             AndData,
  IN  UINT8                             OrData
  );

/**
  Enable VTd support in PSF.
  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
PchPsfEnableVtd (
  VOID
  );

/**
  Hide P2SB device.

  @param[in]  P2sbBase                  Pci base address of P2SB controller.

  @retval EFI_SUCCESS                   Always return success.
**/
EFI_STATUS
PchHideP2sb (
  UINTN                                 P2sbBase
  );

/**
  Reveal P2SB device.
  Also return the original P2SB status which is for Hidding P2SB or not after.
  If OrgStatus is not NULL, then TRUE means P2SB is unhidden,
  and FALSE means P2SB is hidden originally.

  @param[in]  P2sbBase                  Pci base address of P2SB controller.
  @param[out] OrgStatus                 Original P2SB hidding/unhidden status

  @retval EFI_SUCCESS                   Always return success.
**/
EFI_STATUS
PchRevealP2sb (
  UINTN                                 P2sbBase,
  BOOLEAN                               *OrgStatus
  );

typedef enum  {
  PchPcieP1,
  PchPcieP2,
  PchPcieP3,
  PchPcieP4,
  PchPcieP5,
  PchPcieP6,
  PchPcieP7,
  PchPcieP8,
  PchPcieP9,
  PchPcieP10,
  PchPcieP11,
  PchPcieP12,
  PchPcieP13,
  PchPcieP14,
  PchPcieP15,
  PchPcieP16,
  PchPcieP17,
  PchPcieP18,
  PchPcieP19,
  PchPcieP20,
  PchSataP0,
  PchSataP1,
  PchSataP2,
  PchSataP3,
  PchSataP4,
  PchSataP5,
  PchSataP6,
  PchSataP7,
  PchsSataP0,
  PchsSataP1,
  PchsSataP2,
  PchsSataP3,
  PchsSataP4,
  PchsSataP5,  
  PchUpx8P0,
  PchUpx8P1,
  PchUpx8P2,
  PchUpx8P3,
  PchUpx8P4,
  PchUpx8P5,
  PchUpx8P6, 
  PchUpx8P7,   
  PchUsbP0,
  PchUsbP1,
  PchUsbP2,
  PchUsbP3,
  PchUsbP4,
  PchUsbP5,
  PchUsbP6,
  PchUsbP7,
  PchUsbP8,
  PchUsbP9,
  PchGbeP0,
  PchUfsP0
} PCH_DEVICE_PORT;

/**
  Check if the device port is available on any lane

  @param[in]  DevicePort     Device Port Number

  @retval EFI_SUCCESS
**/
EFI_STATUS
IsPortAvailable (
  PCH_DEVICE_PORT DevicePort
  );

/**
  The function returns the Port Id and lane owner for the specified lane

  @param[in]  PhyMode             Phymode that needs to be checked
  @param[out] Pid                 Common Lane End Point ID
  @param[out] LaneOwner           Lane Owner

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid lane number
**/
EFI_STATUS
EFIAPI
PchGetLaneInfo (
  IN  UINT32                            LaneNum,
  OUT UINT8                             *PortId,
  OUT UINT8                             *LaneOwner
  );

//
// implemented in PchSbiAccess.c
//

/**
  PCH SBI Register structure
**/
typedef struct {
  UINT32            SbiAddr;
  UINT32            SbiExtAddr;
  UINT32            SbiData;
  UINT16            SbiStat;
  UINT16            SbiRid;
} PCH_SBI_REGISTER_STRUCT;

/**
  PCH SBI opcode definitions
**/
typedef enum {
  MemoryRead             = 0x0,
  MemoryWrite            = 0x1,
  PciConfigRead          = 0x4,
  PciConfigWrite         = 0x5,
  PrivateControlRead     = 0x6,
  PrivateControlWrite    = 0x7,
  GpioLockUnlock         = 0x13
} PCH_SBI_OPCODE;

/**
  PCH SBI response status definitions
**/
typedef enum {
  SBI_SUCCESSFUL          = 0,
  SBI_UNSUCCESSFUL        = 1,
  SBI_POWERDOWN           = 2,
  SBI_MIXED               = 3,
  SBI_INVALID_RESPONSE
} PCH_SBI_RESPONSE;

/**
  Execute PCH SBI message
  Take care of that there is no lock protection when using SBI programming in both POST time and SMI.
  It will clash with POST time SBI programming when SMI happen.
  Programmer MUST do the save and restore opration while using the PchSbiExecution inside SMI
  to prevent from racing condition.
  This function will reveal P2SB and hide P2SB if it's originally hidden. If more than one SBI access
  needed, it's better to unhide the P2SB before calling and hide it back after done.

  When the return value is "EFI_SUCCESS", the "Response" do not need to be checked as it would have been
  SBI_SUCCESS. If the return value is "EFI_DEVICE_ERROR", then this would provide additional information
  when needed.

  @param[in] Pid                        Port ID of the SBI message
  @param[in] Offset                     Offset of the SBI message
  @param[in] Opcode                     Opcode
  @param[in] Posted                     Posted message
  @param[in, out] Data32                Read/Write data
  @param[out] Response                  Response

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_DEVICE_ERROR              Transaction fail
  @retval EFI_INVALID_PARAMETER         Invalid parameter
**/
EFI_STATUS
EFIAPI
PchSbiExecution (
  IN     PCH_SBI_PID                    Pid,
  IN     UINT64                         Offset,
  IN     PCH_SBI_OPCODE                 Opcode,
  IN     BOOLEAN                        Posted,
  IN OUT UINT32                         *Data32,
  OUT    UINT8                          *Response
  );

/**
  Full function for executing PCH SBI message
  Take care of that there is no lock protection when using SBI programming in both POST time and SMI.
  It will clash with POST time SBI programming when SMI happen.
  Programmer MUST do the save and restore opration while using the PchSbiExecution inside SMI
  to prevent from racing condition.
  This function will reveal P2SB and hide P2SB if it's originally hidden. If more than one SBI access
  needed, it's better to unhide the P2SB before calling and hide it back after done.

  When the return value is "EFI_SUCCESS", the "Response" do not need to be checked as it would have been
  SBI_SUCCESS. If the return value is "EFI_DEVICE_ERROR", then this would provide additional information
  when needed.

  @param[in] Pid                        Port ID of the SBI message
  @param[in] Offset                     Offset of the SBI message
  @param[in] Opcode                     Opcode
  @param[in] Posted                     Posted message
  @param[in] Fbe                        First byte enable
  @param[in] Bar                        Bar
  @param[in] Fid                        Function ID
  @param[in, out] Data32                Read/Write data
  @param[out] Response                  Response

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_DEVICE_ERROR              Transaction fail
  @retval EFI_INVALID_PARAMETER         Invalid parameter
**/
EFI_STATUS
EFIAPI
PchSbiExecutionEx (
  IN     PCH_SBI_PID                    Pid,
  IN     UINT64                         Offset,
  IN     PCH_SBI_OPCODE                 Opcode,
  IN     BOOLEAN                        Posted,
  IN     UINT16                         Fbe,
  IN     UINT16                         Bar,
  IN     UINT16                         Fid,
  IN OUT UINT32                         *Data32,
  OUT    UINT8                          *Response
  );

/**
  This function saves all PCH SBI registers.
  The save and restore operations must be done while using the PchSbiExecution inside SMM.
  It prevents the racing condition of PchSbiExecution re-entry between POST and SMI.
  Before using this function, make sure the P2SB is not hidden.

  @param[in, out] PchSbiRegister        Structure for saving the registers

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_DEVICE_ERROR              Device is hidden.
**/
EFI_STATUS
EFIAPI
PchSbiRegisterSave (
  IN OUT PCH_SBI_REGISTER_STRUCT        *PchSbiRegister
  );

/**
  This function restores all PCH SBI registers
  The save and restore operations must be done while using the PchSbiExecution inside SMM.
  It prevents the racing condition of PchSbiExecution re-entry between POST and SMI.
  Before using this function, make sure the P2SB is not hidden.

  @param[in] PchSbiRegister             Structure for restoring the registers

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_DEVICE_ERROR              Device is hidden.
**/
EFI_STATUS
EFIAPI
PchSbiRegisterRestore (
  IN PCH_SBI_REGISTER_STRUCT            *PchSbiRegister
  );

//
// implemented in PchCycleDecoding.c
//

/**
  Set PCH ACPI base address.
  The Address should not be 0 and should be 256 bytes alignment, and it is IO space, so must not exceed 0xFFFF.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. clear PMC PCI offset 44h [7] to diable ACPI base address first before changing base address.
  2. program PMC PCI offset 40h [15:2] to ACPI base address.
  3. set PMC PCI offset 44h [7] to enable ACPI base address.
  4. program "ACPI Base Address" PCR[DMI] + 27B4h[23:18, 15:2, 0] to [0x3F, PMC PCI Offset 40h bit[15:2],  1].
  5. Program "ACPI Base Destination ID" PCR[DMI] + 27B8h[31:0] to [0x23A0].

  @param[in] Address                    Address for ACPI base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address passed.
**/
EFI_STATUS
EFIAPI
PchAcpiBaseSet (
  UINT16                                Address
  );

/**
  Get PCH ACPI base address.

  @param[in] Address                    Address of ACPI base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid pointer passed.
**/
EFI_STATUS
EFIAPI
PchAcpiBaseGet (
  UINT16                                *Address
  );

/**
  Set PCH PWRM base address.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. clear PMC PCI offset 44h [8] to diable PWRM base address first before changing PWRM base address.
  2. program PMC PCI offset 48h [31:16] to PM base address.
  3. set PMC PCI offset 44h [8] to enable PWRM base address.
  4. program "PM Base Address Memory Range Base" PCR[DMI] + 27ACh[15:0] to the same value programmed in PMC PCI Offset 48h bit[31:16], this has an implication of making sure the PWRMBASE to be 64KB aligned.
     program "PM Base Address Memory Range Limit" PCR[DMI] + 27ACh[31:16] to the value programmed in PMC PCI Offset 48h bit[31:16], this has an implication of making sure the memory allocated to PWRMBASE to be 64KB in size.
  5. program "PM Base Control" PCR[DMI] + 27B0h[31, 30:0] to [1, 0x23A0].

  @param[in] Address                    Address for PWRM base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address passed.
**/
EFI_STATUS
EFIAPI
PchPwrmBaseSet (
  UINT32                                Address
  );

/**
  Get PCH PWRM base address.

  @param[in] Address                    Address of PWRM base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid pointer passed.
**/
EFI_STATUS
EFIAPI
PchPwrmBaseGet (
  UINT32                                *Address
  );

/**
  Set PCH TCO base address.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. set Smbus PCI offset 54h [8] to enable TCO base address.
  2. program Smbus PCI offset 50h [15:5] to TCO base address.
  3. set Smbus PCI offset 54h [8] to enable TCO base address.
  4. program "TCO Base Address" PCR[DMI] + 2778h[15:5, 1] to [Smbus PCI offset 50h[15:5], 1].

  @param[in] Address                    Address for TCO base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address passed.
**/
EFI_STATUS
EFIAPI
PchTcoBaseSet (
  UINT16                                Address
  );

/**
  Get PCH TCO base address.

  @param[in] Address                    Address of TCO base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid pointer passed.
**/
EFI_STATUS
EFIAPI
PchTcoBaseGet (
  UINT16                                *Address
  );

///
/// structure of LPC general IO range register
/// It contains base address, address mask, and enable status.
///
typedef struct {
  UINT32                                BaseAddr :16;
  UINT32                                Length   :15;
  UINT32                                Enable   : 1;
} PCH_LPC_GEN_IO_RANGE;

#define PCH_LPC_GEN_IO_RANGE_MAX        4
///
/// structure of LPC general IO range register list
/// It lists all LPC general IO ran registers supported by PCH.
///
typedef struct {
  PCH_LPC_GEN_IO_RANGE                  Range[PCH_LPC_GEN_IO_RANGE_MAX];
} PCH_LPC_GEN_IO_RANGE_LIST;

/**
  Set PCH LPC generic IO range.
  For generic IO range, the base address must align to 4 and less than 0xFFFF, and the length must be power of 2
  and less than or equal to 256. Moreover, the address must be length aligned.
  This function basically checks the address and length, which should not overlap with all other generic ranges.
  If no more generic range register available, it returns out of resource error.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  The IO ranges below 0x100 have fixed target. The target might be ITSS,RTC,LPC,PMC or terminated inside P2SB
  but all predefined and can't be changed. IO range below 0x100 will be skipped except 0x80-0x8F.
  Steps of programming generic IO range:
  1. Program LPC/eSPI PCI Offset 84h ~ 93h of Mask, Address, and Enable.
  2. Program LPC/eSPI Generic IO Range #, PCR[DMI] + 2730h ~ 273Fh to the same value programmed in LPC/eSPI PCI Offset 84h~93h.

  @param[in] Address                    Address for generic IO range base address.
  @param[in] Length                     Length of generic IO range.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address or length passed.
  @retval EFI_OUT_OF_RESOURCES          No more generic range available.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
PchLpcGenIoRangeSet (
  UINT16                                Address,
  UINTN                                 Length
  , IN  UINT8                           SlaveDevice
  );

/**
  Get PCH LPC generic IO range list.
  This function returns a list of base address, length, and enable for all LPC generic IO range regsiters.

  @param[in] LpcGenIoRangeList          Return all LPC generic IO range register status.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address passed.
**/
EFI_STATUS
EFIAPI
PchLpcGenIoRangeGet (
  PCH_LPC_GEN_IO_RANGE_LIST             *LpcGenIoRangeList
    , IN  UINT8                         SlaveDevice
  );

/**
  Set PCH LPC memory range decoding.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. Program LPC/eSPI PCI 98h [0] to [0] to disable memory decoding first before changing base address.
  2. Program LPC/eSPI PCI 98h [31:16, 0] to [Address, 1].
  3. Program LPC/eSPI Memory Range, PCR[DMI] + 2740h to the same value programmed in LPC/eSPI PCI Offset 98h.

  @param[in] Address                    Address for memory base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address or length passed.
  @retval EFI_OUT_OF_RESOURCES          No more generic range available.
**/
EFI_STATUS
EFIAPI
PchLpcMemRangeSet (
  UINT32                                Address
    , IN  UINT8                         SlaveDevice
  );

/**
  Get PCH LPC memory range decoding address.

  @param[in] Address                    Address of LPC memory decoding base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address passed.
**/
EFI_STATUS
EFIAPI
PchLpcMemRangeGet (
  UINT32                                *Address
    , IN  UINT8                         SlaveDevice
  );

/**
  Set PCH BIOS range deocding.
  This will check General Control and Status bit 10 (GCS.BBS) to identify SPI or LPC/eSPI and program BDE register accordingly.
  Please check EDS for detail of BiosDecodeEnable bit definition.
    bit 15: F8-FF Enable
    bit 14: F0-F8 Enable
    bit 13: E8-EF Enable
    bit 12: E0-E8 Enable
    bit 11: D8-DF Enable
    bit 10: D0-D7 Enable
    bit  9: C8-CF Enable
    bit  8: C0-C7 Enable
    bit  7: Legacy F Segment Enable
    bit  6: Legacy E Segment Enable
    bit  5: Reserved
    bit  4: Reserved
    bit  3: 70-7F Enable
    bit  2: 60-6F Enable
    bit  1: 50-5F Enable
    bit  0: 40-4F Enable
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. if GCS.BBS is 0 (SPI), program SPI PCI offset D8h to BiosDecodeEnable.
     if GCS.BBS is 1 (LPC/eSPi), program LPC/eSPI PCI offset D8h to BiosDecodeEnable.
  2. program LPC/eSPI/SPI BIOS Decode Enable, PCR[DMI] + 2744h to the same value programmed in LPC/eSPI or SPI PCI Offset D8h.

  @param[in] BiosDecodeEnable           Bios decode enable setting.

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
EFIAPI
PchBiosDecodeEnableSet (
  UINT16                                BiosDecodeEnable
  );

/**
  Set PCH LPC IO decode ranges.
  Program LPC I/O Decode Ranges, PCR[DMI] + 2770h[15:0] to the same value programmed in LPC offset 80h.
  Please check EDS for detail of Lpc IO decode ranges bit definition.
  Bit  12: FDD range
  Bit 9:8: LPT range
  Bit 6:4: ComB range
  Bit 2:0: ComA range

  @param[in] LpcIoDecodeRanges          Lpc IO decode ranges bit settings.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
PchLpcIoDecodeRangesSet (
  UINT16                                LpcIoDecodeRanges
  );

/**
  Set PCH LPC IO enable decoding.
  Setup LPC I/O Enables, PCR[DMI] + 2774h[15:0] to the same value program in LPC offset 82h.
  Note: Bit[15:10] of the source decode register is Read-Only. The IO range indicated by the Enables field
  in LPC 82h[13:10] is always forwarded by DMI to subtractive agent for handling.
  Please check EDS for detail of Lpc IO decode ranges bit definition.

  @param[in] LpcIoEnableDecoding        Lpc IO enable decoding bit settings.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
PchLpcIoEnableDecodingSet (
  UINT16                                LpcIoEnableDecoding
  , IN  UINT8                           SlaveDevice
  );

/**
  Set PCH IO port 80h cycle decoding to PCIE root port.
  System BIOS is likely to do this very soon after reset before PCI bus enumeration, it must ensure that
  the IO Base Address field (PCIe:1Ch[7:4]) contains a value greater than the IO Limit field (PCIe:1Ch[15:12])
  before setting the IOSE bit. Otherwise the bridge will positively decode IO range 000h - FFFh by its default
  IO range values.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. Program "RPR Destination ID", PCR[DMI] + 274Ch[31:16] to the Dest ID of RP.
  2. Program "Reserved Page Route", PCR[DMI] + 274Ch[11] to '1'. Use byte write on GCS+1 and leave the BILD bit which is RWO.
  3. Program IOSE bit of PCIE:Reg04h[0] to '1'  for PCH to send such IO cycles to PCIe bus for subtractive decoding.

  @param[in] RpPhyNumber                PCIE root port physical number.

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
EFIAPI
PchIoPort80DecodeSet (
  UINTN                                 RpPhyNumber
  );

///
/// This enumeration defines the available types to query using
/// PchGetSpiRegionBase routine
///
typedef enum {
  DescriptorType,
  BiosType,
  MeType,
  GbeType,
  PlatformDataType,
  DeviceExpansionType,
  SecondaryBiosType,
  BfpregType,
  PchSpiRegionTypeMax
} PCH_SPI_REGION_TYPE;

/**
  Get the SPI region base, based on the enum type

  @param[in]  RegionType          Region type to query for the base address
  @param[out] LimitAddress        This address is left shifted by 12 bits to represent bits 26:12 for the Region 'n' Limit
  @param[out] BaseAddress         This address is left shifted by 12 bits to represent bits 26:12 for the Region 'n' Base

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid region type given
**/
EFI_STATUS
EFIAPI
PchGetSpiRegionAddresses (
  IN   PCH_SPI_REGION_TYPE   RegionType,
  OUT  UINT16                *LimitAddress,
  OUT  UINT16                *BaseAddress
  );

/**
  Enumeration of each PCH XHCI LEGACY SMI type.
  PchXhciLegacySmiEn is the major switch for XHCI legacy SMI.
  And each can be ORed with others as parameter. ex..
  PchXhciLegacySmiEnSet (PchXhciLegacySmiEn | PchXhciLegacySmiOsOwnershipEn);
**/
enum PCH_XHCI_LEGACY_SMI {
  PchXhciLegacySmiEn                 = B_PCH_XHCI_USBLEGCTLSTS_USBSMIE,
  PchXhciLegacySmiHostSysErrorEn     = B_PCH_XHCI_USBLEGCTLSTS_SMIHSEE,
  PchXhciLegacySmiOsOwnershipEn      = B_PCH_XHCI_USBLEGCTLSTS_SMIOSOE,
  PchXhciLegacySmiPciCmdEn           = B_PCH_XHCI_USBLEGCTLSTS_SMIPCICE,
  PchXhciLegacySmiBarEn              = B_PCH_XHCI_USBLEGCTLSTS_SMIBARE
};

/**
  Get XHCI USB legacy SMI control setting.

  @retval     return XHCI USB legacy SMI setting.
**/
UINT32
EFIAPI
PchXhciLegacySmiEnGet (
  VOID
  );

/**
  Set XHCI USB legacy SMI control setting.

  @param[in]  UsbLegacySmiEn            Set XHCI USB legacy SMI

**/
VOID
EFIAPI
PchXhciLegacySmiEnSet (
  UINT32                                UsbLegacySmiEn
  );

/**
  Funtion to check if Battery lost or CMOS cleared.

  @reval TRUE  Battery is always present.
  @reval FALSE CMOS is cleared.
**/
BOOLEAN
EFIAPI
PchIsRtcBatteryGood (
  VOID
  );

#endif // _PEI_DXE_SMM_PCH_PLATFORM_LIB_H_
