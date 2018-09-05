/** @file
  PCH SERIAL GPIO Driver implements the SERIAL GPIO Interface.
  Usage model for this protocol is:
  1. locate this protocol by guid  variable gPchSerialGpioProtocolGuid
  2. Use SerialGpioRegister to register for one serial GPIO pin.
  3. Send data using SerialGpioSendData.
  4. If another GPIO need to send serial data,
     the former one need to be unregistered using SerialGpioUnRegister since PCH have only one set of registers for serial GPIO data sending.
     And register the new GPIO pin for Serial Gpio data sending.

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
#include "PchSerialGpio.h"

///
/// Global variables
///
GLOBAL_REMOVE_IF_UNREFERENCED UINT32  mPchGpioBase;

/**
  Register for one GPIO Pin that will be used as serial GPIO.
  For SKL PCH only GPP_D_0-4 will have the capability to be used as serial GPIO.
  The caller of this procedure need to be very clear of whPch GPIO should be used as serial GPIO,
  it should not be input, native, conflict with other GPIO, or Index > 4 on the caller's platform.

  @param[in] This                 Pointer to the PCH_SERIAL_GPIO_PROTOCOL instance.
  @param[in] SerialGpioPad        The GPIO pad number that will be used as serial GPIO for data sending.

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
  )
{
  UINT32                   GpioSerBlinkValue;
  UINT32                   GpioPwmValue;
  GPIO_CONFIG              GpioConfiguration;
  GPIO_GROUP               Group;
  SERIAL_GPIO_INSTANCE     *SerialGpioInstance;

  if (SerialGpioPad >= PCH_GPIO_SERIAL_BLINK_MAX_PIN) {
    return EFI_INVALID_PARAMETER;
  }

  SerialGpioInstance = SERIAL_GPIO_INSTANCE_FROM_SERIAL_GPIO_PROTOCOL (This);
  if (SerialGpioInstance->CurrentActiveSerialGpio != SERIAL_GPIO_PIN_CLEARED) {
    DEBUG (
      (DEBUG_ERROR,
      "You have to unregister the former serial GPIO %d registered, then try to register this GPIO pin %d\n",
      SerialGpioInstance->CurrentActiveSerialGpio,
      SerialGpioPad)
      );
  }

  GpioSerBlinkValue = 0;
  GpioPwmValue = 0;

  ///
  /// Read out values original in serial GPIO registers.
  ///
  if (GetPchSeries () == PchH) {
    Group = GPIO_SKL_H_GROUP_GPP_D;
  } else {
    Group = GPIO_SKL_LP_GROUP_GPP_D;
  }

  GpioGetReg (Group, R_PCH_PCR_GPIO_GP_SER_BLINK, &GpioSerBlinkValue);
  GpioGetReg (Group, R_PCH_PCR_GPIO_PWMC, &GpioPwmValue);

  GpioGetPadConfig (GpioGetGpioPadFromGroupAndPadNumber (Group, SerialGpioPad), &GpioConfiguration);

  SerialGpioInstance->RegistersToRecover.SavedGpioSerBlinkValue   = GpioSerBlinkValue;
  SerialGpioInstance->RegistersToRecover.SavedGpioPwmValue        = GpioPwmValue;
  SerialGpioInstance->RegistersToRecover.SavedGpioConfiguration   = GpioConfiguration;

  ///
  /// Modify settings in serial GPIO registers.
  ///
  ///
  /// Serial GPIO will have to be selected as GPIO, not native
  ///
  GpioConfiguration.PadMode = GpioPadModeGpio;

  ///
  /// Serial GPIO will have no PWM setting
  ///
  GpioPwmValue &= (~(1 << SerialGpioPad));

  ///
  /// Serial GPIO will have to enable Serial Blink setting
  ///
  GpioSerBlinkValue |= (1 << SerialGpioPad);

  ///
  /// Serial GPIO will have to be output
  ///
  GpioConfiguration.Direction = GpioDirOut;

  if (WaitForSerialGpioNotBusy () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  GpioSetReg (Group, R_PCH_PCR_GPIO_GP_SER_BLINK, GpioSerBlinkValue);
  GpioSetReg (Group, R_PCH_PCR_GPIO_PWMC, GpioPwmValue);

  GpioSetPadConfig (GpioGetGpioPadFromGroupAndPadNumber (Group, SerialGpioPad), &GpioConfiguration);

  ///
  /// Record this GPIO index to private data structure
  ///
  SerialGpioInstance->CurrentActiveSerialGpio = SerialGpioPad;

  return EFI_SUCCESS;
}

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
  )
{
  UINT32                   GpioSerBlinkValue;
  UINT32                   GpioPwmValue;
  GPIO_CONFIG              GpioConfiguration;
  GPIO_GROUP               Group;
  SERIAL_GPIO_INSTANCE     *SerialGpioInstance;

  SerialGpioInstance = SERIAL_GPIO_INSTANCE_FROM_SERIAL_GPIO_PROTOCOL (This);
  if (SerialGpioInstance == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((SerialGpioPad != SerialGpioInstance->CurrentActiveSerialGpio) ||
      (SerialGpioPad >= PCH_GPIO_SERIAL_BLINK_MAX_PIN)
      ) {
    return EFI_INVALID_PARAMETER;
  }

  GpioSerBlinkValue     = SerialGpioInstance->RegistersToRecover.SavedGpioSerBlinkValue;
  GpioPwmValue          = SerialGpioInstance->RegistersToRecover.SavedGpioPwmValue;
  GpioConfiguration     = SerialGpioInstance->RegistersToRecover.SavedGpioConfiguration;

  ///
  /// At least to clear the serial Blink property
  ///
  GpioSerBlinkValue &= (~(1 << SerialGpioInstance->CurrentActiveSerialGpio));

  if (WaitForSerialGpioNotBusy () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  if (GetPchSeries () == PchH) {
    Group = GPIO_SKL_H_GROUP_GPP_D;
  } else {
    Group = GPIO_SKL_LP_GROUP_GPP_D;
  }

  ///
  /// Write values with original values in serial GPIO registers.
  ///
  GpioSetReg (Group, R_PCH_PCR_GPIO_GP_SER_BLINK, GpioSerBlinkValue);
  GpioSetReg (Group, R_PCH_PCR_GPIO_PWMC, GpioPwmValue);

  GpioSetPadConfig (GpioGetGpioPadFromGroupAndPadNumber (Group, SerialGpioPad), &GpioConfiguration);

  ///
  /// Clear the GPIO index in private data structure
  ///
  SerialGpioInstance->CurrentActiveSerialGpio = SERIAL_GPIO_PIN_CLEARED;

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS            Status;
  UINTN                 DataCountInDword;
  UINTN                 DataCountLeftInByte;
  UINTN                 Index;
  UINT32                GpioSbCmdStsValue;
  UINT32                Group;
  SERIAL_GPIO_INSTANCE  *SerialGpioInstance;

  SerialGpioInstance = SERIAL_GPIO_INSTANCE_FROM_SERIAL_GPIO_PROTOCOL (This);
  if (SerialGpioInstance == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Check if the parameters are valid.
  ///
  if ((Buffer == NULL) ||
      (DataRate > SERIAL_GPIO_MAX_DATA_RATE) ||
      (GpioPad != SerialGpioInstance->CurrentActiveSerialGpio)
      ) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Make sure it's safe to program the serial GPIO.
  ///
  if (WaitForSerialGpioNotBusy () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  if (GetPchSeries () == PchH) {
    Group = GPIO_SKL_H_GROUP_GPP_D;
  } else {
    Group = GPIO_SKL_LP_GROUP_GPP_D;
  }

  ///
  /// set data rate
  ///
  GpioGetReg (Group, R_PCH_PCR_GPIO_GP_SER_CMDSTS, &GpioSbCmdStsValue);
  GpioSbCmdStsValue &= (~ B_PCH_PCR_GPIO_GP_SER_CMDSTS_DRS);
  GpioSbCmdStsValue |= (DataRate << N_PCH_PCR_GPIO_GP_SER_CMDSTS_DRS);
  GpioSetReg (Group, R_PCH_PCR_GPIO_GP_SER_CMDSTS, GpioSbCmdStsValue);

  if (WaitForSerialGpioNotBusy () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  DataCountInDword    = DataCountInByte / 4;
  DataCountLeftInByte = DataCountInByte % 4;
  for (Index = 0; Index < DataCountInDword; Index++) {
    Status = SendSerialGpioSend (
              This,
              EnumSerialGpioDataDword,
              (Buffer + Index * 4)
              );
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
  }

  for (Index = 0; Index < DataCountLeftInByte; Index++) {
    Status = SendSerialGpioSend (
              This,
              EnumSerialGpioDataByte,
              (Buffer + DataCountInDword * 4 + Index)
              );
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
  }

  return EFI_SUCCESS;
}

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
  )
{
  UINT32  DataInDword;
  UINT32  GpioSbCmdStsValue;
  UINT32  Group;

  ///
  /// Wait the SERIAL GPIO BUSY to be cleared.
  ///
  if (WaitForSerialGpioNotBusy () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  if (GetPchSeries () == PchH) {
    Group = GPIO_SKL_H_GROUP_GPP_D;
  } else {
    Group = GPIO_SKL_LP_GROUP_GPP_D;
  }

  ///
  /// set data length
  ///
  GpioGetReg (Group, R_PCH_PCR_GPIO_GP_SER_CMDSTS, &GpioSbCmdStsValue);
  GpioSbCmdStsValue &= (~B_PCH_PCR_GPIO_GP_SER_CMDSTS_DLS);
  GpioSbCmdStsValue |= (DataWidth << N_PCH_PCR_GPIO_GP_SER_CMDSTS_DLS);
  GpioSetReg (Group, R_PCH_PCR_GPIO_GP_SER_CMDSTS, GpioSbCmdStsValue);

  ///
  /// Set Data
  ///
  DataInDword = *(UINT32 *) Data;
  GpioSetReg (Group, R_PCH_PCR_GPIO_GP_SER_DATA, DataInDword);

  ///
  /// Set GO to start transmit
  ///
  GpioSbCmdStsValue |= B_PCH_PCR_GPIO_GP_SER_CMDSTS_GO;
  GpioSetReg (Group, R_PCH_PCR_GPIO_GP_SER_CMDSTS, GpioSbCmdStsValue);
  if (WaitForSerialGpioNotBusy () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Wait PCH serial GPIO Busy bit being cleared by PCH chipset.

  @retval EFI_SUCCESS             SERIAL GPIO BUSY bit is cleared.
  @retval EFI_DEVICE_ERROR        Time out while waiting the SERIAL GPIO BUSY bit to be cleared.
                                  It's not safe to send next data block on the SERIAL GPIO interface.
**/
EFI_STATUS
WaitForSerialGpioNotBusy (
  VOID
  )
{
  UINTN   WaitTicks;
  UINTN   WaitCount;
  UINT32  GpioSbCmdStsValue;
  UINT32  Group;

  if (GetPchSeries () == PchH) {
    Group = GPIO_SKL_H_GROUP_GPP_D;
  } else {
    Group = GPIO_SKL_LP_GROUP_GPP_D;
  }

  ///
  /// Convert the wait period allowed into to tick count
  ///
  WaitCount = WAIT_TIME / WAIT_PERIOD;
  ///
  /// Wait for the SERIAL_GPIO cycle to complete.
  ///
  for (WaitTicks = 0; WaitTicks < WaitCount; WaitTicks++) {
    GpioGetReg (Group, R_PCH_PCR_GPIO_GP_SER_CMDSTS, &GpioSbCmdStsValue);
    if ((GpioSbCmdStsValue & B_PCH_PCR_GPIO_GP_SER_CMDSTS_BUSY) == 0) {
      return EFI_SUCCESS;
    }

    MicroSecondDelay (WAIT_PERIOD);
  }

  return EFI_DEVICE_ERROR;
}

/**
  <b>SerialGpio DXE Module Entry Point</b>\n
  - <b>Introduction</b>\n
    The SerialGpio module is a DXE driver which provides a standard way for other
    drivers to use the serial GPIO Interface.

  - @pre
    - PCH PCR base address configured
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
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE      *SystemTable
  )
{
  EFI_STATUS               Status;
  SERIAL_GPIO_INSTANCE     *SerialGpioInstance;
  UINT32                   GpioSerBlinkValue;
  UINT32                   GpioPwmValue;
  UINT32                   Group;

  DEBUG ((DEBUG_INFO, "InstallPchSerialGpio() Start\n"));

  ///
  /// Allocate Runtime memory for the SERIAL_GPIO protocol instance.
  ///
  SerialGpioInstance = AllocateRuntimePool (sizeof (SERIAL_GPIO_INSTANCE));
  if (SerialGpioInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (GetPchSeries () == PchH) {
    Group = GPIO_SKL_H_GROUP_GPP_D;
  } else {
    Group = GPIO_SKL_LP_GROUP_GPP_D;
  }

  GpioGetReg (Group, R_PCH_PCR_GPIO_GP_SER_BLINK, &GpioSerBlinkValue);
  GpioGetReg (Group, R_PCH_PCR_GPIO_PWMC, &GpioPwmValue);

  SerialGpioInstance->Signature = PCH_SERIAL_GPIO_PRIVATE_DATA_SIGNATURE;
  SerialGpioInstance->Handle = NULL;
  SerialGpioInstance->CurrentActiveSerialGpio = SERIAL_GPIO_PIN_CLEARED;
  SerialGpioInstance->RegistersToRecover.SavedGpioSerBlinkValue = GpioSerBlinkValue;
  SerialGpioInstance->RegistersToRecover.SavedGpioPwmValue      = GpioPwmValue;
  SerialGpioInstance->SerialGpioProtocol.SerialGpioRegister     = PchSerialGpioRegister;
  SerialGpioInstance->SerialGpioProtocol.SerialGpioSendData     = PchSerialGpioSendData;
  SerialGpioInstance->SerialGpioProtocol.SerialGpioUnRegister   = PchSerialGpioUnRegister;
  ///
  /// Install the PCH_SERIAL_GPIO_PROTOCOL interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &(SerialGpioInstance->Handle),
                  &gPchSerialGpioProtocolGuid,
                  &(SerialGpioInstance->SerialGpioProtocol),
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    FreePool (SerialGpioInstance);
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((DEBUG_INFO, "InstallPchSerialGpio() End\n"));

  return EFI_SUCCESS;
}
