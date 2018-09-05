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
  SsaMemoryDdrtConfig.h

@brief
  This file contains the definition of the BIOS-SSA DDR-T Memory Configuration API.

  NOTE: This file is only intended to be included from MrcSsaServices.h.  It
  cannot be included separately.

**/
#ifndef _SsaMemoryDdrtConfig_h_
#define _SsaMemoryDdrtConfig_h_

#ifdef SSA_FLAG

// BIOS-SSA DDR-T Memory Configuration API revision
#define SSA_REVISION_MEMORY_DDRT ((0x01 << 24) | (0x01 << 16) | (0x00 << 8) | 0x00)

#pragma pack (push, 1)

///
/// Falcon Valley training mode.
///
typedef enum {
  FnvTrainModeEnableTraining,
  FnvTrainModeDisableTraining,
  FnvTrainModeEarlyCmdCk,
  FnvTrainModeLateCmdCk,
  FnvTrainModeEarlyRidFine,
  FnvTrainModeEarlyRidCoarse,
  FnvTrainModeCmdVrefCentering,
  FnvTrainModeRxBacksidePhase,
  FnvTrainModeDisableTrainingStep,
  FnvTrainingModeMax,                     ///< FNV_TRAINING_MODE enumeration maximum value.
  FnvTrainingModeDelim = INT32_MAX        ///< This value ensures the enum size is consistent on both sides of the PPI.
} FNV_TRAINING_MODE;

///
/// ERID signal name.
///
typedef enum {
  EridSignal0,
  EridSignal1,
  EridSignalMax,                          ///< ERID_SIGNAL enumeration maximum value.
  EridSignalDelim = INT32_MAX             ///< This value ensures the enum size is consistent on both sides of the PPI.
} ERID_SIGNAL;

///
/// ERID pattern mode.
///
typedef enum {
  EridLfsrMode,
  EridBufferMode,
  EridPatternModeMax,                     ///< ERID_PATTERN_MODE enumeration maximum value.
  EridPatternDelim = INT32_MAX            ///< This value ensures the enum size is consistent on both sides of the PPI.
} ERID_PATTERN_MODE;

/**
  Function used to set the bit validation mask of Early Read ID.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      EridMask   - Early Read ID bitmask.  Bit value 1 = perform validation; bit value 0 = do not perform validation.  Bit position 3: CKP Early Read ID (even).  Bit position 2: CKP Early Read ID (odd).  Bit position 1: CKN Early Read ID (even).  Bit position 0: CKN Early Read ID (odd).

  @retval None
**/
typedef
VOID
(EFIAPI * SET_ERID_VALIDATION_MASK) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     EridMask
  );

/**
  Function used to get the Early Read ID bit error status.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[out]     Status     - Pointer to where the Early Read ID bit error status will be stored.  Bit value 1 = at least one error occurred; bit value 0 = no errors occurred.  Bit position 3: CKP Early Read ID (even).  Bit position 2: CKP Early Read ID (odd).  Bit position 1: CKN Early Read ID (even).  Bit position 0: CKN Early Read ID (odd).
  @param[out]     Count      - Pointer to where the error counter value will be stored.

  @retval None
**/
typedef
VOID
(EFIAPI * GET_ERID_ERROR_STATUS) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  OUT UINT8                    *Status,
  OUT UINT16                   *Count
  );

/**
  Function used to clear the Early Read ID bit error status.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.

  @retval None
**/
typedef
VOID
(EFIAPI * CLEAR_ERID_ERROR_STATUS) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel
  );

/**
  Function used to set up the Early Read ID pattern generator.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      Signal        - ERID signal name.
  @param[in]      PatternMode   - ERID pattern mode.
  @param[in]      EnableReload  - Enable or disable seed reload for the Early Read ID.
  @param[in]      EnableReset   - Enable or disable seed reset for the Early Read ID.
  @param[in]      Seed          - Seed value for Early Read ID.

  @retval None
**/
typedef
VOID
(EFIAPI * SET_ERID_PATTERN) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN ERID_SIGNAL               Signal,
  IN ERID_PATTERN_MODE         PatternMode,
  IN BOOLEAN                   EnableReload,
  IN BOOLEAN                   EnableReset,
  IN UINT32                    Seed
  );

/**
  Function used to set the Early Read ID coarse training configuration.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      EnableTraining  - Specifies whether Early Read ID coarse training mode is enabled.

  @retval None
**/
typedef
VOID
(EFIAPI * SET_ERID_TRAINING_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN BOOLEAN                   EnableTraining
  );

/**
  Function used to set the training mode of the AEP DIMMs for a given socket.

  @param[in, out] This             - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket           - Zero based CPU socket number.
  @param[in]      Mode             - FalconValley (controller on AEP DIMM) training mode.

  @retval None
**/
typedef
VOID
(EFIAPI * SET_AEP_DIMM_TRAINING_MODE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN FNV_TRAINING_MODE         Mode
  );

/**
  Function used to reset the I/O for a given AEP memory DIMM.

  @param[in, out] This             - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket           - Zero based CPU socket number.
  @param[in]      Controller       - Zero based controller number.
  @param[in]      Channel          - Zero based channel number.
  @param[in]      Dimm             - Zero based DIMM number.

  @retval None
**/
typedef
VOID
(EFIAPI * AEP_IO_RESET) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Dimm
  );

/**
  Function used to drain the RPQ for a given AEP memory DIMM.

  @param[in, out] This             - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket           - Zero based CPU socket number.
  @param[in]      Controller       - Zero based controller number.
  @param[in]      Channel          - Zero based channel number.
  @param[in]      Dimm             - Zero based DIMM number.

  @retval None
**/
typedef
VOID
(EFIAPI * DRAIN_AEP_RPQ) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Dimm
  );

/**
  Function used to clear prefetch cache miss of AEP DIMMs for a given socket.

  @param[in, out] This             - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket           - Zero based CPU socket number.

  @retval None
**/
typedef
VOID
(EFIAPI * CLEAR_AEP_PREFETCH_CACHE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket
  );

/**
  Function used to perform FIFO reset for all the AEP DIMMs for a given socket.

  @param[in, out] This             - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket           - Zero based CPU socket number.

  @retval None
**/
typedef
VOID
(EFIAPI * RESET_AEP_FIFO_TRAIN) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket
  );

/**
  BIOS-SSA DDR-T Memory Configuration
**/
typedef struct _SSA_MEMORY_DDRT_CONFIG {
  UINT32                     Revision;               ///< Incremented when a backwards compatible binary change is made to the PPI.
  SET_ERID_VALIDATION_MASK   SetEridValidationMask;  ///< Function used to set the bit validation mask of Early Read ID.
  GET_ERID_ERROR_STATUS      GetEridErrorStatus;     ///< Function used to get the Early Read ID bit error status.
  CLEAR_ERID_ERROR_STATUS    ClearEridErrorStatus;   ///< Function used to clear the Early Read ID bit error status.
  SET_ERID_PATTERN           SetEridPattern;         ///< Function used to set up the Early Read ID pattern generator.
  SET_ERID_TRAINING_CONFIG   SetEridTrainingConfig;  ///< Function used to set the Early Read ID coarse training configuration.
  SET_AEP_DIMM_TRAINING_MODE SetAepDimmTrainingMode; ///< Function used to set the training mode of the controller chip of the AEP DIMM (FalconValley).
  AEP_IO_RESET               AepIoReset;             ///< Function used to reset the I/O for a given AEP memory DIMM.
  DRAIN_AEP_RPQ              DrainAepRpq;            ///< Function used to drain the RPQ for a given AEP memory DIMM.
  CLEAR_AEP_PREFETCH_CACHE   ClearAepPrefetchCache;  ///< Function used to clear prefetch cache miss of AEP DIMMs for a given socket.
  RESET_AEP_FIFO_TRAIN       ResetAepFifoTrain;      ///< Function used to perform FIFO reset for all the AEP DIMMs for a given socket.
} SSA_MEMORY_DDRT_CONFIG, *PSSA_MEMORY_DDRT_CONFIG;

#pragma pack (pop)
#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG

#endif // _SsaMemoryDdrtConfig_h_
