/**
@copyright
  Copyright (c) 2014-2015 Intel Corporation. All rights reserved.
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
  SsaMemoryConfig.h

@brief
  This file contains the definition of the BIOS-SSA Memory Configuration API.

  NOTE: This file is only intended to be included from MrcSsaServices.h.  It
  cannot be included separately.

**/
#ifndef _SsaMemoryConfig_h_
#define _SsaMemoryConfig_h_

#ifdef SSA_FLAG

// BIOS-SSA Memory Configuration API revision
#define SSA_REVISION_MEMORY ((0x01 << 24) | (0x00 << 16) | (0x00 << 8) | 0x00)

#pragma pack (push, 1)

///
/// System information definitions.
///
typedef struct {
  UINT8 MaxNumberSockets;                 ///< The maximum number of CPU sockets in a system.
  UINT8 MaxNumberControllers;             ///< The maximum number of memory controllers in a CPU socket.
  UINT8 MaxNumberChannels;                ///< The maximum number of channels in a memory controller.
  UINT8 MaxNumberDimms;                   ///< The maximum number of DIMMs in a memory channel.
  UINT8 MaxNumberRanks;                   ///< The maximum number of ranks in a DIMM.
  UINT32 SocketBitMask;                   ///< The bit mask of available CPU sockets.
  UINT8 BusWidth;                         ///< The width (number of bits) of the memory data bus on the system.
  UINT32 BusFreq;                         ///< The frequency of the memory bus in MHz.
  BOOLEAN IsEccEnabled;                   ///< TRUE if ECC is enabled in the system, FALSE if not.
  MrcBootMode BootMode;                   ///< System boot mode.
} MRC_SYSTEM_INFO;

///
/// DIMM memory technology
///
typedef enum {
  SsaMemoryDdr3   = 0xB,                  ///< DDR3 technology
  SsaMemoryDdr4   = 0xC,                  ///< DDR4 technology
  SsaMemoryDdrT   = 0xD,                  ///< DDRT technology
  SsaMemoryLpDdr3 = 0xF1,                 ///< LPDDR3 technology
  SsaMemoryLpDdr4,                        ///< LPDDR4 technology
  MemoryTechnologyMax,                    ///< MEMORY_TECHNOLOGY enumeration maximum value.
  MemoryTechnologyDelim = INT32_MAX       ///< This value ensures the enum size is consistent on both sides of the PPI.
} MEMORY_TECHNOLOGY;

///
/// DIMM information definitions.
///
typedef struct {
  BOOLEAN           EccSupport;           ///< TRUE if the DIMM supports ECC, otherwise FALSE.
  UINT32            DimmCapacity;         ///< DIMM capacity in MBytes.
  UINT8             BankCount;            ///< Number of banks per rank in DIMM.
  UINT32            RowSize;              ///< DIMM row address size.
  UINT16            ColumnSize;           ///< DIMM column address size.
  UINT8             DeviceWidth;          ///< DRAM device width.
  MEMORY_TECHNOLOGY MemoryTech;           ///< DIMM memory signaling technology.
  MEMORY_PACKAGE    MemoryPackage;        ///< DIMM memory package.
} MRC_DIMM_INFO;

#ifndef _MrcSpdData_h_

///
/// SPD manufacturing date code byte definitions.
///
typedef struct {
  UINT8 Year;                             ///< Year represented in BCD (00h = 2000)
  UINT8 Week;                             ///< Year represented in BCD (47h = week 47)
} SPD_MANUFACTURING_DATE;

///
/// SPD manufacturing serial number byte definitions.
///
typedef union {
  UINT32 Data;
  UINT16 SerialNumber16[2];
  UINT8  SerialNumber8[4];
} SPD_MANUFACTURER_SERIAL_NUMBER;

///
/// SPD manufacturing information definitions.
///
typedef struct {
  UINT16                         IdCode;       ///< Module Manufacturer ID Code
  UINT8                          Location;     ///< Module Manufacturing Location
  SPD_MANUFACTURING_DATE         Date;         ///< Module Manufacturing Year, in BCD (range: 2000-2255)
  SPD_MANUFACTURER_SERIAL_NUMBER SerialNumber; ///< Module Serial Number
} SPD_UNIQUE_MODULE_ID;

#endif //_MrcSpdData_h_

///
/// Margin parameter applies to specific level of the platform.
///
typedef enum {
  // these are bit mask values
  LaneSpecific       = 0x01,              ///< Margin parameter applies to lane.
  StrobeSpecific     = 0x02,              ///< Margin parameter applies to strobe.
  RankSpecific       = 0x04,              ///< Margin parameter applies to rank.
  ChannelSpecific    = 0x08,              ///< Margin parameter applies to channel.
  ControllerSpecific = 0x10,              ///< Margin parameter applies to controller.
  SocketSpecific     = 0x20,              ///< Margin parameter applies to socket.
  PlatformSpecific   = 0x40,              ///< Margin parameter applies to platform.
  // don't modify above values; add new entries below in bit mask format
  MarginParamSpecificityDelim = INT32_MAX ///< This value ensures the enum size is consistent on both sides of the PPI.
} MARGIN_PARAM_SPECIFICITY, *PMARGIN_PARAM_SPECIFICITY;

///
/// Setup or cleanup action specifier.
///
typedef enum {
  Setup,                                  ///< Set up.
  Cleanup,                                ///< Clean up.
  SetupCleanupMax,                        ///< SETUP_CLEANUP enumeration maximum value.
  SetupCleanupDelim = INT32_MAX           ///< This value ensures the enum size is consistent on both sides of the PPI.
} SETUP_CLEANUP;

/**
  Function used to get information about the system.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     SystemInfo - Pointer to buffer to be filled with system information.

  @retval None
**/
typedef
VOID
(EFIAPI * GET_SYSTEM_INFO) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT MRC_SYSTEM_INFO          *SystemInfo
  );

/**
  Function used to get the platform memory voltage (VDD).

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     Voltage - Pointer to were the platform's memory voltage (in mV) will be written.

  @retval None
**/
typedef
VOID
(EFIAPI * GET_MEM_VOLTAGE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT UINT32                   *Voltage
  );

/**
  Function used to set the platform memory voltage.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Voltage - The memory voltage (in mV) to be set on the platform.

  @retval None
**/
typedef
VOID
(EFIAPI * SET_MEM_VOLTAGE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT32                    Voltage
  );

/**
  Function used to get the DIMM temperature.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[out]     Temperature - Pointer to where the DIMM's temperature will be written.  Units: Celsius with 1 degree precision.

  @retval SUCCESS
  @retval NotAvailable if the DIMM does not support a temperature sensor.
**/
typedef
SSA_STATUS
(EFIAPI * GET_MEM_TEMP) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Dimm,
  OUT INT32                    *Temperature
  );

/**
  Function used to get the bitmask of populated memory controllers on a given CPU socket.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket  - Zero based CPU socket number.
  @param[out]     BitMask     - Pointer to where the memory controller bitmask will be stored.  Bit value 1 = populated; bit value 0 = absent. Bit position 0 = memory controller 0; bit position 1 = memory controller 1, etc.

  @retval None
**/
typedef
VOID
(EFIAPI * GET_CONTROLLER_BIT_MASK) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  OUT UINT8                    *BitMask
  );

/**
  Function used to get the bitmask of populated memory channels on a given memory controller.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[out]     BitMask     - Pointer to where the memory channel bit mask will be stored.  Bit value 1 = populated; bit value 0 = absent. Bit position 0 = memory channel 0; bit position 1 = memory channel 1, etc.

  @retval None
**/
typedef
VOID
(EFIAPI * GET_CHANNEL_BIT_MASK) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  OUT UINT8                    *BitMask
  );

/**
  Function used to get the bitmask of populated DIMMs on a given memory channel.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[out]     BitMask    - Pointer to where the DIMM bit mask will be stored.  Bit value 1 = populated; bit value 0 = absent. Bit position 0 = DIMM 0; bit position 1 = DIMM 1, etc.

  @retval None
**/
typedef
VOID
(EFIAPI * GET_DIMM_BIT_MASK) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  OUT UINT8                    *BitMask
  );

/**
  Function used to get the number of ranks in a given DIMM.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[out]     *RankCount  - Pointer to where the rank count will be stored.

  @retval None
**/
typedef
VOID
(EFIAPI * GET_RANK_IN_DIMM) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Dimm,
  OUT UINT8                    *RankCount
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
typedef
VOID
(EFIAPI * GET_LOGICAL_RANK) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Dimm,
  IN UINT8                     Rank,
  OUT UINT8                    *LogicalRank
  );

/**
  Function used to get DIMM information.

  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket         - Zero based CPU socket number.
  @param[in]      Controller     - Zero based controller number.
  @param[in]      Channel        - Zero based channel number.
  @param[in]      Dimm           - Zero based DIMM number.
  @param[out]     DimmInfoBuffer - Pointer to buffer to be filled with DIMM information.

  @retval None
**/
typedef
VOID
(EFIAPI * GET_DIMM_INFO) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Dimm,
  OUT MRC_DIMM_INFO            *DimmInfoBuffer
  );

/**
  Function used to get DIMM unique module ID.

  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket         - Zero based CPU socket number.
  @param[in]      Controller     - Zero based controller number.
  @param[in]      Channel        - Zero based channel number.
  @param[in]      Dimm           - Zero based DIMM number.
  @param[out]     UniqueModuleId - Pointer to buffer to be filled with DIMM unique module ID.

  @retval None
**/
typedef
VOID
(EFIAPI * GET_DIMM_UNIQUE_MODULE_ID) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Dimm,
  OUT SPD_UNIQUE_MODULE_ID     *UniqueModuleId
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

  @retval SUCCESS
  @retval UnsupportedValue if (ByteOffset + ByteCnt) is larger than the SPD size.
**/
typedef
SSA_STATUS
(EFIAPI * GET_SPD_DATA) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Dimm,
  IN UINT16                    ByteOffset,
  IN UINT16                    ByteCount,
  IN BOOLEAN                   ReadFromBus,
  OUT UINT8                    *Data
  );

/**
  Function used to perform a JEDEC reset for all the DIMMs on all channels of a given memory controller.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.

  @retval None
**/
typedef
VOID
(EFIAPI * JEDEC_RESET) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller
  );

/**
  Function used to reset the I/O for a given memory controller.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.

  @retval None
**/
typedef
VOID
(EFIAPI * IO_RESET) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller
  );

/**
  Function used to get the specificity of a given margin parameter.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      IoLevel     - I/O level.
  @param[in]      MarginGroup - Margin group.
  @param[out]     Specificity - Pointer to where the margin parameter specificity mask will be stored.

  @retval SUCCESS
  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.
**/
typedef
SSA_STATUS
(EFIAPI * GET_MARGIN_PARAM_SPECIFICITY) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN GSM_LT                    IoLevel,
  IN GSM_GT                    MarginGroup,
  OUT MARGIN_PARAM_SPECIFICITY *Specificity
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

  @retval SUCCESS
  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.
**/
typedef
SSA_STATUS
(EFIAPI * INIT_MARGIN_PARAM) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN GSM_LT                    IoLevel,
  IN GSM_GT                    MarginGroup,
  IN SETUP_CLEANUP             SetupCleanup
  );

/**
  Function used to get the minimum and maximum offsets that can be applied to a
  given margin parameter and the time delay in micro seconds for the new value
  to take effect.

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

  @retval SUCCESS
  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.
**/
typedef
SSA_STATUS
(EFIAPI * GET_MARGIN_PARAM_LIMITS) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Dimm,
  IN UINT8                     Rank,
  IN CONST UINT8               *LaneMasks,
  IN GSM_LT                    IoLevel,
  IN GSM_GT                    MarginGroup,
  OUT INT16                    *MinOffset,
  OUT INT16                    *MaxOffset,
  OUT UINT16                   *Delay,
  OUT UINT16                   *StepUnit
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
  @param[in]      CurrentOffset - Signed value of the current offset setting.
  @param[in]      NewOffset     - Signed value of the new offset setting.

  @retval SUCCESS
  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.
**/
typedef
SSA_STATUS
(EFIAPI * SET_MARGIN_PARAM_OFFSET) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Dimm,
  IN UINT8                     Rank,
  IN CONST UINT8               *LaneMasks,
  IN GSM_LT                    IoLevel,
  IN GSM_GT                    MarginGroup,
  IN INT16                     CurrentOffset,
  IN INT16                     NewOffset
  );

/**
  Function used to set the ZQCal configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Enable     - Specifies whether ZQCal is enabled.  TRUE enables ZQCal; FALSE disables it.
  @param[out]     PrevEnable - Pointer to where the previous ZQCal enable setting will be stored.  This value may be NULL.

  @retval None
**/
typedef
VOID
(EFIAPI * SET_ZQCAL_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN BOOLEAN                   Enable,
  OUT BOOLEAN                  *PrevEnable
  );

/**
  Function used to set the RComp update configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Enable     - Specifies whether RComp updates are enabled.  TRUE enables RComp updates; FALSE disables them.
  @param[out]     PrevEnable - Pointer to where the previous RComp update enable setting will be stored.  This value may be NULL.

  @retval None
**/
typedef
VOID
(EFIAPI * SET_RCOMP_UPDATE_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN BOOLEAN                   Enable,
  OUT BOOLEAN                  *PrevEnable
  );

/**
  Function used to set the page-open configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Enable     - Specifies whether page-open is enabled.  TRUE enables page-open; FALSE disables it.
  @param[out]     PrevEnable - Pointer to where the previous page-open enable setting will be stored.  This value may be NULL.

  @retval None
**/
typedef
VOID
(EFIAPI * SET_PAGE_OPEN_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN BOOLEAN                   Enable,
  OUT BOOLEAN                  *PrevEnable
  );

/**
  Function used to clear all memory.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.

  @retval SUCCESS
  @retval NotAvailable if function is not supported by the BIOS.
**/
typedef
SSA_STATUS
(EFIAPI * SCRUB_MEMORY) (
  IN OUT SSA_BIOS_SERVICES_PPI *This
  );

/**
  Function used to initialize CPGC engine(s) for all channels of a given memory
  controller.  Prior to configuring/using the CPGC engine, this function should
  be called with the Context input parameter set to SetupContext.  When
  finished with the CPGC engine, this function should be called with the
  Context input parameter set to CleanupContext.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      SetupCleanup - Specifies setup or cleanup action.

  @retval None
**/
typedef
VOID
(EFIAPI * INIT_CPGC) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN SETUP_CLEANUP             SetupCleanup
  );

/**
  BIOS-SSA Memory Configuration
**/
typedef struct _SSA_MEMORY_CONFIG {
  UINT32                       Revision;                  ///< Incremented when a backwards compatible binary change is made to the PPI.
  VOID                         *MrcData;                  ///< Pointer to the BIOS data buffer.
  GET_SYSTEM_INFO              GetSystemInfo;             ///< Function used to get information about the system.
  GET_MEM_VOLTAGE              GetMemVoltage;             ///< Function used to get the platform memory voltage.
  SET_MEM_VOLTAGE              SetMemVoltage;             ///< Function used to set the platform memory voltage.
  GET_MEM_TEMP                 GetMemTemp;                ///< Function used to get the DIMM temperature.
  GET_CONTROLLER_BIT_MASK      GetControllerBitMask;      ///< Function used to get the bitmask of populated memory controllers on a given CPU socket.
  GET_CHANNEL_BIT_MASK         GetChannelBitMask;         ///< Function used to get the bitmask of populated memory channels on a given memory controller.
  GET_DIMM_BIT_MASK            GetDimmBitMask;            ///< Function used to get the bitmask of populated DIMMs on a given memory channel.
  GET_RANK_IN_DIMM             GetRankInDimm;             ///< Function used to get the number of ranks in a given DIMM.
  GET_LOGICAL_RANK             GetLogicalRank;            ///< Function used to get the MC logical rank associated with a given DIMM and rank.
  GET_DIMM_INFO                GetDimmInfo;               ///< Function used to get DIMM information.
  GET_DIMM_UNIQUE_MODULE_ID    GetDimmUniqueModuleId;     ///< Function used to get DIMM unique module ID.
  GET_SPD_DATA                 GetSpdData;                ///< Function used to get DIMM SPD data.
  JEDEC_RESET                  JedecReset;                ///< Function used to perform a JEDEC reset for all the DIMMs on all channels of a given memory controller.
  IO_RESET                     IoReset;                   ///< Function used to reset the I/O for a given memory controller.
  GET_MARGIN_PARAM_SPECIFICITY GetMarginParamSpecificity; ///< Function used to get the specificity of a given margin parameter.
  INIT_MARGIN_PARAM            InitMarginParam;           ///< Function used to initialize system before/after a margin parameter's use.
  GET_MARGIN_PARAM_LIMITS      GetMarginParamLimits;      ///< Function used to get the minimum and maximum offsets that can be applied to a given margin parameter and the time delay in micro seconds for the new value to take effect.
  SET_MARGIN_PARAM_OFFSET      SetMarginParamOffset;      ///< Function used to set the offset of a margin parameter.
  SET_ZQCAL_CONFIG             SetZQCalConfig;            ///< Function used to set the ZQCal configuration.
  SET_RCOMP_UPDATE_CONFIG      SetRCompUpdateConfig;      ///< Function used to set the RComp update configuration.
  SET_PAGE_OPEN_CONFIG         SetPageOpenConfig;         ///< Function used to set the page-open configuration.
  SCRUB_MEMORY                 ScrubMemory;               ///< Function used to clear all memory.
  INIT_CPGC                    InitCpgc;                  ///< Function used to initialize CPGC engine(s).
} SSA_MEMORY_CONFIG, *PSSA_MEMORY_CONFIG;

#pragma pack (pop)
#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG

#endif // _SsaMemoryConfig_h_
