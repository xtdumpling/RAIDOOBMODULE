/** @file
  Header file for the PCH SERIAL GPIO Driver.

@copyright
 Copyright (c) 2004 - 2014 Intel Corporation. All rights reserved
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
#ifndef _PCH_SERIAL_GPIO_H_
#define _PCH_SERIAL_GPIO_H_

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SerialGpio.h>
#include <PchAccess.h>
#include <Library/PchInfoLib.h>

#include <Library/MmPciBaseLib.h>

#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>
#include <Library/GpioNativeLib.h>
#include <IncludePrivate/Library/GpioPrivateLib.h>

#define PCH_GPIO_SERIAL_BLINK_MAX_PIN        4
///
/// Private data structure definitions for the driver
///
#define PCH_SERIAL_GPIO_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('S', 'G', 'P', 'O')

#define SERIAL_GPIO_INSTANCE_FROM_SERIAL_GPIO_PROTOCOL(a) \
  CR ( \
  a, \
  SERIAL_GPIO_INSTANCE, \
  SerialGpioProtocol, \
  PCH_SERIAL_GPIO_PRIVATE_DATA_SIGNATURE \
  )

///
/// Only when CurrentActiveSerialGpio == SERIAL_GPIO_PIN_CLEARED,
/// can the next GPIO be register as serial GPIO using SerialGpioProtocol
///
#define SERIAL_GPIO_PIN_CLEARED 0xFF
///
/// This is the old values in GPIO related registers for recovery when unregister this serial GPIO pin.
///
typedef struct {
  UINT32           SavedGpioSerBlinkValue;
  UINT32           SavedGpioPwmValue;
  GPIO_CONFIG      SavedGpioConfiguration;
} SERIAL_GPIO_REGISTERS_TO_RECOVER;

typedef struct {
  UINTN                             Signature;
  EFI_HANDLE                        Handle;
  UINT8                             CurrentActiveSerialGpio;  ///< can only have one pin at one time
  SERIAL_GPIO_REGISTERS_TO_RECOVER  RegistersToRecover;
  PCH_SERIAL_GPIO_PROTOCOL          SerialGpioProtocol;
} SERIAL_GPIO_INSTANCE;

//
// Function prototypes used by the SERIAL_GPIO protocol.
//

/**
  Register for one GPIO Pin that will be used as serial GPIO.
  For SKL PCH only GPP_D_0-4 will have the capability to be used as serial GPIO.
  The caller of this procedure need to be very clear of whPch GPIO should be used as serial GPIO,
  it should not be input, native, conflict with other GPIO, or Index > 4 on the caller's platform.

  @param[in] This                Pointer to the PCH_SERIAL_GPIO_PROTOCOL instance.
  @param[in] SerialGpioPad       The GPIO pad number that will be used as serial GPIO for data sending.

  @retval EFI_SUCCESS             Opcode initialization on the SERIAL_GPIO host controller completed.
  @retval EFI_ACCESS_DENIED       The SERIAL_GPIO configuration interface is locked.
  @retval EFI_OUT_OF_RESOURCES    Not enough resource available to initialize the device.
  @retval EFI_DEVICE_ERROR        Device error, operation failed.
  @retval EFI_INVALID_PARAMETER   SerialGpioPinIndex is out of range
**/
EFI_STATUS
EFIAPI
PchSerialGpioRegister (
  IN PCH_SERIAL_GPIO_PROTOCOL       *This,
  IN UINT8                          SerialGpioPad
  );

/**
  Unregister for one GPIO Pin that has been used as serial GPIO, and recover the registers before
  registering.

  @param[in] This                 Pointer to the PCH_SERIAL_GPIO_PROTOCOL instance.
  @param[in] SerialGpioPad        The GPIO pad number that will be used as serial GPIO for data sending.

  @retval EFI_SUCCESS             Opcode initialization on the SERIAL_GPIO host controller completed.
  @retval EFI_ACCESS_DENIED       The SERIAL_GPIO configuration interface is locked.
  @retval EFI_OUT_OF_RESOURCES    Not enough resource available to initialize the device.
  @retval EFI_DEVICE_ERROR        Device error, operation failed.
  @retval EFI_INVALID_PARAMETER   Invalid function parameters
**/
EFI_STATUS
EFIAPI
PchSerialGpioUnRegister (
  IN PCH_SERIAL_GPIO_PROTOCOL       *This,
  IN UINT8                          SerialGpioPad
  );

/**
  Execute SERIAL_GPIO commands from the host controller.

  @param[in] This                 Pointer to the PCH_SERIAL_GPIO_PROTOCOL instance.
  @param[in] GpioPad              GPIO pad number.
  @param[in] DataRate             The data rate for serial data transferring. 1 ~ SERIAL_GPIO_MAX_DATA_RATE; 1: 128ns intervals; ...; 8: 8*128 = 1024ns intervals, default value;...
  @param[in] DataCountInByte      Number of bytes of the data will be transmitted through the GPIO pin.
  @param[in, out] Buffer          Pointer to caller-allocated buffer containing the data sent through the GPIO pin.

  @retval EFI_SUCCESS             Execute succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, GPIO serial data sent failed.
**/
EFI_STATUS
EFIAPI
PchSerialGpioSendData (
  IN     PCH_SERIAL_GPIO_PROTOCOL   *This,
  IN     UINT8                      GpioPad,
  IN     UINT8                      DataRate,
  IN     UINTN                      DataCountInByte,
  IN OUT UINT8                      *Buffer
  );

/**
  This function sends the dword/word/byte through the serial GPIO pin.

  @param[in] This                 Pointer to the PCH_SERIAL_GPIO_PROTOCOL instance.
  @param[in] DataWidth            The data width. 0: byte; 1: word; 2: reserved; 3: dword.
  @param[in] Data                 Data buffer that contains the data (<= UINT32)

  @retval EFI_SUCCESS             SERIAL_GPIO command completes successfully.
  @retval EFI_DEVICE_ERROR        Device error, the command aborts abnormally.
**/
EFI_STATUS
SendSerialGpioSend (
  IN     PCH_SERIAL_GPIO_PROTOCOL   *This,
  IN     SERIAL_GPIO_DATA_WIDTH     DataWidth,
  IN     UINT8                      *Data
  );

/**
  Wait PCH serial GPIO Busy bit being cleared by PCH chipset.

  @retval EFI_SUCCESS             SERIAL GPIO BUSY bit is cleared.
  @retval EFI_DEVICE_ERROR        Time out while waiting the SERIAL GPIO BUSY bit to be cleared.
                                  It's not safe to send next data block on the SERIAL GPIO interface.
**/
EFI_STATUS
WaitForSerialGpioNotBusy (
  VOID
  );

/**
  <b>SerialGpio DXE Module Entry Point</b>\n
  - <b>Introduction</b>\n
    The SerialGpio module is a DXE driver which provides a standard way for other
    drivers to use the serial GPIO Interface.

  - @pre
    - @link GpioLib.h PeiDxeSmmGpioLib @endlink library

  - @result
    The SerialGpio driver produces @link _PCH_SERIAL_GPIO_PROTOCOL PCH_SERIAL_GPIO_PROTOCOL @endlink.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
  @retval EFI_SUCCESS             Initialization complete.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
InstallPchSerialGpio (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  );

#endif
