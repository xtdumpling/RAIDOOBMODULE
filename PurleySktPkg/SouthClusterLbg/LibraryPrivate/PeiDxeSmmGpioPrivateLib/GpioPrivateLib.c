/** @file
  This file contains GPIO routines for RC usage

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
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
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <PchAccess.h>
#include <Library/GpioLib.h>
#include <Library/GpioNativeLib.h>
#include <IncludePrivate/Library/GpioPrivateLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PeiDxeSmmGpioLib/GpioLibrary.h>
/**
  This procedure will get value of selected gpio register

  @param[in]  Group               GPIO group number
  @param[in]  Offset              GPIO register offset
  @param[out] RegVal              Value of gpio register

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioGetReg (
  IN  GPIO_GROUP              Group,
  IN  UINT32                  Offset,
  OUT UINT32                  *RegVal
  )
{
  GPIO_GROUP_INFO      *GpioGroupInfo;
  UINTN                GpioGroupInfoLength;
  UINTN                GroupIndex;

  GroupIndex = GpioGetGroupIndexFromGroup (Group);
  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);
  //
  // Check if group argument exceeds GPIO GROUP INFO array
  //
  if ((UINTN)GroupIndex >= GpioGroupInfoLength) {
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }

  *RegVal = MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, Offset));

  return EFI_SUCCESS;
}

/**
  This procedure will set value of selected gpio register

  @param[in] Group               GPIO group number
  @param[in] Offset              GPIO register offset
  @param[in] RegVal              Value of gpio register

  @retval EFI_SUCCESS            The function completed successfully
  @retval EFI_INVALID_PARAMETER  Invalid group or pad number
**/
EFI_STATUS
GpioSetReg (
  IN GPIO_GROUP              Group,
  IN UINT32                  Offset,
  IN UINT32                  RegVal
  )
{
  GPIO_GROUP_INFO      *GpioGroupInfo;
  UINTN                GpioGroupInfoLength;
  UINTN                GroupIndex;

  GroupIndex = GpioGetGroupIndexFromGroup (Group);
  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);
  //
  // Check if group argument exceeds GPIO GROUP INFO array
  //
  if ((UINTN)GroupIndex >= GpioGroupInfoLength) {
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }

  MmioWrite32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, Offset), RegVal);

  return EFI_SUCCESS;
}

/**
  This procedure is used by PchSmiDispatcher and will return information
  needed to register GPI SMI. Relation between Index and GpioPad number is:
  Index = GpioGroup + 24 * GpioPad

  @param[in]  Index               GPI SMI number
  @param[out] GpioPin             GPIO pin
  @param[out] GpiSmiBitOffset     GPI SMI bit position within GpiSmi Registers
  @param[out] GpiSmiEnRegAddress  Address of GPI SMI Enable register
  @param[out] GpiSmiStsRegAddress Address of GPI SMI status register

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioGetPadAndSmiRegs (
  IN UINT32            Index,
  OUT GPIO_PAD         *GpioPin,
  OUT UINT8            *GpiSmiBitOffset,
  OUT UINT32           *GpiSmiEnRegAddress,
  OUT UINT32           *GpiSmiStsRegAddress
  )
{
  UINT32               GroupIndex;
  UINT32               Pad;
  GPIO_GROUP_INFO      *GpioGroupInfo;
  GPIO_GROUP           GpioGroupOffset;
  UINTN                GpioGroupInfoLength;

  GPIO_PAD_OWN       PadOwnVal;
  UINT32             HostOwnVal;


  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  GpioGroupOffset = GpioGetLowestGroup ();

  Pad = 0;
  GroupIndex = 0;
  for (GroupIndex = 0; GroupIndex < GpioGroupInfoLength; GroupIndex++) {
    Pad = Index;
    if (Pad < GpioGroupInfo[GroupIndex].PadPerGroup) {
      //
      // Found group and pad number
      //
      break;
    }
    Index = Index - GpioGroupInfo[GroupIndex].PadPerGroup;
  }

  //
  // Check if legal pad number
  //
  if (Pad >= GpioGroupInfo[GroupIndex].PadPerGroup){
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check if selected group has GPI SMI Enable and Status registers
  //
  if (GpioGroupInfo[GroupIndex].SmiEnOffset == NO_REGISTER_FOR_PROPERTY) {
    return EFI_INVALID_PARAMETER;
  }

DEBUG_CODE_BEGIN();
  //
  // Check if selected GPIO Pad is not owned by CSME/ISH/IE
  //
  GpioGetPadOwnership (GpioGetGpioPadFromGroupAndPadNumber (GroupIndex + GpioGroupOffset, Pad), &PadOwnVal);
  if (PadOwnVal != GpioPadOwnHost) {
    DEBUG ((DEBUG_ERROR, "GPIO ERROR: Accessing pad not owned by host (Group=%d, Pad=%d)!\n",GroupIndex, Pad));
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check if Host Software Pad Ownership is set to ACPI Mode
  //
  GpioGetHostSwOwnershipForPad (GpioGetGpioPadFromGroupAndPadNumber (GroupIndex + GpioGroupOffset, Pad), &HostOwnVal);
  if (HostOwnVal != V_PCH_PCR_GPIO_HOSTSW_OWN_ACPI) {
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }

DEBUG_CODE_END();

  *GpioPin = GpioGetGpioPadFromGroupAndPadNumber (GroupIndex + GpioGroupOffset, Pad);
  *GpiSmiBitOffset = (UINT8)Pad;
  *GpiSmiEnRegAddress = (UINT32)PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].SmiEnOffset);
  *GpiSmiStsRegAddress = (UINT32)PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].SmiEnOffset - (R_PCH_PCR_GPIO_GPP_A_GPI_GPE_EN - R_PCH_PCR_GPIO_GPP_A_GPI_GPE_STS));

  return EFI_SUCCESS;
}

/**
  This procedure will clear GPIO_UNLOCK_SMI_STS

  @param[in]  None

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GpioClearUnlockSmiSts (
  VOID
  )
{
  UINT16  AcpiBaseAddr;

  PchAcpiBaseGet (&AcpiBaseAddr);

  //
  // GPIO_UNLOCK_SMI_STS is cleared by writing 1 to it. Other bits in
  // R_PCH_SMI_STS registers are either RO or RW/1C so writing 0 to them
  // will not change their state.
  //
  IoWrite32 (AcpiBaseAddr + R_PCH_SMI_STS, B_PCH_SMI_STS_GPIO_UNLOCK);

  return EFI_SUCCESS;
}


/**
  This procedure will set GPIO Driver IRQ number

  @param[in]  Irq                 Irq number

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid IRQ number
**/
EFI_STATUS
GpioSetIrq (
  IN  UINT8          Irq
  )
{
  UINT32   Data32And;
  UINT32   Data32Or;

  //
  // Check if Irq is 14 or 15
  //
  if ((Irq < 14) || (Irq > 15)) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  Data32And = (UINT32)~(B_PCH_PCR_GPIO_MISCCFG_IRQ_ROUTE);
  Data32Or  = (UINT32)(Irq - 14) << N_PCH_PCR_GPIO_MISCCFG_IRQ_ROUTE;

  //
  // Program MISCCFG register for Community 0
  //
  MmioAndThenOr32 (
    (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM0, R_PCH_PCR_GPIO_MISCCFG),
    Data32And,
    Data32Or
    );

  //
  // Program MISCCFG register for Community 1
  //
  MmioAndThenOr32 (
    (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM1, R_PCH_PCR_GPIO_MISCCFG),
    Data32And,
    Data32Or
    );

  //
  // Program MISCCFG register for Community 2
  //
  MmioAndThenOr32 (
    (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM2, R_PCH_PCR_GPIO_MISCCFG),
    Data32And,
    Data32Or
    );

  //
  // Program MISCCFG register for Community 3
  //
  MmioAndThenOr32 (
    (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM3, R_PCH_PCR_GPIO_MISCCFG),
    Data32And,
    Data32Or
    );

  return EFI_SUCCESS;
}
