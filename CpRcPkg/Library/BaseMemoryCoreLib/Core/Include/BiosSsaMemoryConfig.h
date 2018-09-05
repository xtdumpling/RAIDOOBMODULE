/**
@copyright
  Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@file
  BiosSsaMemoryConfig.h

@brief
  This file contains the definition of the BIOS implementation of the BIOS-SSA Memory Configuration API.

**/
#ifndef __BIOS_SSA_MEMORY_CONFIG__
#define __BIOS_SSA_MEMORY_CONFIG__

#ifdef SSA_FLAG
#pragma pack (push, 1)
/**
  Function used to get information about the system.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     SystemInfo - Pointer to buffer to be filled with system information.

  @retval None.
**/
VOID
(EFIAPI BiosGetSystemInfo) (
  SSA_BIOS_SERVICES_PPI *This,
  MRC_SYSTEM_INFO       *SystemInfo
  );

/**
  Function used to get the platform memory voltage (VDD).

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     Voltage - Pointer to were the platform's memory voltage (in mV) will be written.

  @retval None.
**/
VOID
(EFIAPI BiosGetMemVoltage) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                *Voltage
  );

/**
  Function used to set the platform memory voltage.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Voltage - The memory voltage (in mV) to be set on the platform.

  @retval None.
**/
VOID
(EFIAPI BiosSetMemVoltage) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                Voltage
  );

/**
  Function used to get the DIMM temperature.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[out]     Temperature - Pointer to where the DIMM's temperature will be written.  Units: Celsius with 1 degree precision.

  @retval NotAvailable if the DIMM does not support a temperature sensor.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosGetMemTemp) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  INT32                 *Temperature
  );

/**
  Function used to get the bitmask of populated memory controllers on a given CPU socket.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket  - Zero based CPU socket number.
  @param[out]     BitMask     - Pointer to where the memory controller bitmask will be stored.  Bit value 1 = populated; bit value 0 = absent. Bit position 0 = memory controller 0; bit position 1 = memory controller 1, etc.

  @retval None.
**/
VOID
(EFIAPI BiosGetControllerBitMask) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 *BitMask
  );

/**
  Function used to get the bitmask of populated memory channels on a given memory controller.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[out]     BitMask     - Pointer to where the memory channel bit mask will be stored.  Bit value 1 = populated; bit value 0 = absent. Bit position 0 = memory channel 0; bit position 1 = memory channel 1, etc.

  @retval None.
**/
VOID
(EFIAPI BiosGetChannelBitMask) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 *BitMask
  );

/**
  Function used to get the bitmask of populated DIMMs on a given memory channel.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[out]     BitMask    - Pointer to where the DIMM bit mask will be stored.  Bit value 1 = populated; bit value 0 = absent. Bit position 0 = DIMM 0; bit position 1 = DIMM 1, etc.

  @retval None.
**/
VOID
(EFIAPI BiosGetDimmBitMask) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 *BitMask
  );

/**
  Function used to get the number of ranks in a given DIMM.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[out]     *RankCount  - Pointer to where the rank count will be stored.

  @retval None.
**/
VOID
(EFIAPI BiosGetRankInDimm) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 *RankCount
  );

/**
  Function used to get the MC logical rank associated with a given DIMM and rank.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[in]      Rank        - Zero based rank number.
  @param[out]     LogicalRank - Pointer to where the logical rank will be stored.

  @retval None.
**/
VOID
(EFIAPI BiosGetLogicalRank) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 Rank,
  UINT8                 *LogicalRank
  );

/**
  Function used to get DIMM information.

  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket         - Zero based CPU socket number.
  @param[in]      Controller     - Zero based controller number.
  @param[in]      Channel        - Zero based channel number.
  @param[in]      Dimm           - Zero based DIMM number.
  @param[out]     DimmInfoBuffer - Pointer to buffer to be filled with DIMM information.

  @retval None.
**/
VOID
(EFIAPI BiosGetDimmInfo) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  MRC_DIMM_INFO         *DimmInfoBuffer
  );

/**
  Function used to get DIMM unique module ID.

  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket         - Zero based CPU socket number.
  @param[in]      Controller     - Zero based controller number.
  @param[in]      Channel        - Zero based channel number.
  @param[in]      Dimm           - Zero based DIMM number.
  @param[out]     UniqueModuleId - Pointer to buffer to be filled with DIMM unique module ID.

  @retval None.
**/
VOID
(EFIAPI BiosGetDimmUniqueModuleId) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  SPD_UNIQUE_MODULE_ID  *UniqueModuleId
  );

/**
  Function used to get DIMM SPD data.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      Channel      - Zero based channel number.
  @param[in]      Dimm         - Zero based DIMM number.
  @param[in]      ByteOffset   - The byte offset in the SPD.
  @param[in]      ByteCount    - The number of bytes to read starting from the offset location specified by the "ByteOffset".
  @param[in]      ReadFromBus  - Flag to determine where to retrieve the SPD value.  TRUE = read from bus.  FALSE = return from MRC cache value.
  @param[out]     Data         - Pointer to buffer to be filled with  DIMM SPD data.

  @retval UnsupportedValue if (ByteOffset + ByteCnt) is larger than the SPD size.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosGetSpdData) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT16                ByteOffset,
  UINT16                ByteCount,
  BOOLEAN               ReadFromBus,
  UINT8                 *Data
  );

/**
  Function used to perform a JEDEC reset for all the DIMMs on all channels of a given memory controller.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.

  @retval None.
**/
VOID
(EFIAPI BiosJedecReset) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller
  );

/**
  Function used to reset the I/O for a given memory controller.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.

  @retval None.
**/
VOID
(EFIAPI BiosIoReset) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller
  );

/**
  Function used to get the specificity of a given margin parameter.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      IoLevel     - I/O level.
  @param[in]      MarginGroup - Margin group.
  @param[out]     Specificity - Pointer to where the margin parameter specificity mask will be stored.

  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosGetMarginParamSpecificity) (
  SSA_BIOS_SERVICES_PPI    *This,
  GSM_LT                   IoLevel,
  GSM_GT                   MarginGroup,
  MARGIN_PARAM_SPECIFICITY *Specificity
  );

/**
  Function used to initialize the system before/after a margin parameter's use.
  Prior to calling the GetMarginParamLimits or SetMarginParamOffset functions
  for a margin parameter, this function should be called with the SetupCleanup
  input parameter set to Setup.  When finished with the margin parameter, this
  function should be called with the SetupCleanup input parameter set to
  Cleanup.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      IoLevel      - I/O level.
  @param[in]      MarginGroup  - Margin group.
  @param[in]      SetupCleanup - Specifies setup or cleanup action.

  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosInitMarginParam) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  GSM_LT                IoLevel,
  GSM_GT                MarginGroup,
  SETUP_CLEANUP         SetupCleanup
  );

/**
  Function used to get the minimum and maximum offsets that can be applied to a given margin parameter and the time delay in micro seconds for the new value to take effect.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[in]      Rank        - Zero based physical rank number.
  @param[in]      LaneMasks   - Pointer to array of masks of the lanes/strobes that participate to the margin parameter offset limit calculation.  It is applicable only if margin parameter is per-strobe or per-bit/lane controllable.  The number of array elements is the BusWidth value from the GetSystemInfo() function divided by 8.  If a margin parameter is strobe specific and if any lane associated with that strobe is set in the mask then that strobe is selected.  For example, for a LaneMasks value of [0x00, … 0x00, 0x01], only the min/max offset of the first strobe group is returned.
  @param[in]      IoLevel     - I/O level.
  @param[in]      MarginGroup - Margin group.
  @param[out]     MinOffset   - Pointer to where the minimum offset from the current setting supported by the margin parameter will be stored.  This is a signed value.
  @param[out]     MaxOffset   - Pointer to where the maximum offset from the current setting supported by the margin parameter parameter will be stored.  This is a signed value.
  @param[out]     Delay       - Pointer to where the wait time in micro-seconds that is required for the new setting to take effect will be stored.
  @param[out]     StepUnit    - Pointer to where the margin parameter's step unit will be stored.  For timing parameters, the units are tCK / 2048.  For voltage parameters, the units are Vdd / 100.

  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosGetMarginParamLimits) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 Rank,
  CONST UINT8           *LaneMasks,
  GSM_LT                IoLevel,
  GSM_GT                MarginGroup,
  INT16                 *MinOffset,
  INT16                 *MaxOffset,
  UINT16                *Delay,
  UINT16                *StepUnit
  );

/**
  Function used to set the offset of a margin parameter.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      Dimm          - Zero based DIMM number.
  @param[in]      Rank          - Zero based physical rank number.
  @param[in]      LaneMasks     - Pointer to array of masks of the lanes/strobes that participate to the margin parameter offset limit calculation.  It is applicable only if margin parameter is per-strobe or per-bit/lane controllable.  The number of array elements is the BusWidth value from the GetSystemInfo() function divided by 8.  If a margin parameter is strobe specific and if any lane associated with that strobe is set in the mask then that strobe is selected.  For example, for a LaneMasks value of [0x00, … 0x00, 0x01], only the min/max offset of the first strobe group is returned.
  @param[in]      IoLevel       - Id of the I/O level to access. Can be VmseLevel(0), DdrLevel(1), LrbufLevel(2).
  @param[in]      MarginGroup   - Id of the margin group. Can be RcvEna(0), RdT(1), WrT(2), WrDqsT(3), RdV(4) or WrV(5).
  @param[in]      CurrentOffset - Signed value of the current offset setting.  The range of valid values is available  via the GetMarginParamLimits() function.
  @param[in]      NewOffset     - Signed value of the new offset setting.  The range of valid values is available  via the GetMarginParamLimits() function.

  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosSetMarginParamOffset) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 Rank,
  CONST UINT8           *LaneMasks,
  GSM_LT                IoLevel,
  GSM_GT                MarginGroup,
  INT16                 CurrentOffset,
  INT16                 NewOffset
  );

/**
  Function used to set the ZQCal configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Enable     - Specifies whether ZQCal is enabled.  TRUE enables ZQCal; FALSE disables it.
  @param[out]     PrevEnable - Pointer to where the previous ZQCal enable setting will be stored.  This value may be NULL.

  @retval None.
**/
VOID
(EFIAPI BiosSetZQCalConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               Enable,
  BOOLEAN               *PrevEnable
  );

/**
  Function used to set the RComp update configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Enable     - Specifies whether RComp updates are enabled.  TRUE enables RComp updates; FALSE disables them.
  @param[out]     PrevEnable - Pointer to where the previous RComp update enable setting will be stored.  This value may be NULL.

  @retval None.
**/
VOID
(EFIAPI BiosSetRCompUpdateConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               Enable,
  BOOLEAN               *PrevEnable
  );

/**
  Function used to set the page-open configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Enable     - Specifies whether page-open is enabled.  TRUE enables page-open; FALSE disables it.
  @param[out]     PrevEnable - Pointer to where the previous page-open enable setting will be stored.  This value may be NULL.

  @retval None.
**/
VOID
(EFIAPI BiosSetPageOpenConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               Enable,
  BOOLEAN               *PrevEnable
  );

/**
  Function used to clear all memory.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.

  @retval NotAvailable if function is not supported by the BIOS.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosScrubMemory) (
  SSA_BIOS_SERVICES_PPI *This
  );

/**
  Function used to initialize CPGC engine(s) for all channels of a given memory
  controller.  Prior to configuring/using the CPGC engine, this function should
  be called with the SetupCleanup input parameter set to Setup.  When finished
  with the CPGC engine, this function should be called with the SetupCleanup
  input parameter set to Cleanup.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      SetupCleanup - Specifies setup or cleanup action.

  @retval None.
**/
VOID
(EFIAPI BiosInitCpgc) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  SETUP_CLEANUP         SetupCleanup
  );

#pragma pack (pop)
#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
#endif // __BIOS_SSA_MEMORY_CONFIG__
// end file BiosSsaMemoryConfig.h
