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
  BiosSsaMemoryDdrtConfig.h

@brief
  This file contains the definition of the BIOS implementation of the BIOS-SSA DDR-T Memory Configuration API.

**/
#ifndef __BIOS_SSA_MEMORY_DDRT_CONFIG__
#define __BIOS_SSA_MEMORY_DDRT_CONFIG__

#ifdef SSA_FLAG
#pragma pack (push, 1)

/**
  Function used to set the bit validation mask of Early Read ID.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      EridMask   - Early Read ID bitmask.  Bit value 1 = perform validation; bit value 0 = do not perform validation.  Bit position 3: CKP Early Read ID (even).  Bit position 2: CKP Early Read ID (odd).  Bit position 1: CKN Early Read ID (even).  Bit position 0: CKN Early Read ID (odd).

  @retval None.
**/
VOID
(EFIAPI BiosSetEridValidationMask) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 EridMask
  );

/**
  Function used to get the Early Read ID bit error status.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[out]     Status     - Pointer to where the Early Read ID bit error status will be stored.  Bit value 1 = at least one error occurred; bit value 0 = no errors occurred.  Bit position 3: CKP Early Read ID (even).  Bit position 2: CKP Early Read ID (odd).  Bit position 1: CKN Early Read ID (even).  Bit position 0: CKN Early Read ID (odd).
  @param[out]     Count      - Pointer to where the error counter value will be stored.

  @retval None.
**/
VOID
(EFIAPI BiosGetEridErrorStatus) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 *Status,
  UINT16                *Count
  );

/**
  Function used to clear the Early Read ID bit error status.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.

  @retval None.
**/
VOID
(EFIAPI BiosClearEridErrorStatus) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel
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

  @retval None.
**/
VOID
(EFIAPI BiosSetEridPattern) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  ERID_SIGNAL           Signal,
  ERID_PATTERN_MODE     PatternMode,
  BOOLEAN               EnableReload,
  BOOLEAN               EnableReset,
  UINT32                Seed
  );

/**
  Function used to set the Early Read ID coarse training configuration.

  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket         - Zero based CPU socket number.
  @param[in]      Controller     - Zero based controller number.
  @param[in]      Channel        - Zero based channel number.
  @param[in]      EnableTraining - Specifies whether Early Read ID coarse training mode is enabled.

  @retval None.
**/
VOID
(EFIAPI BiosSetEridTrainingConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  BOOLEAN               EnableTraining
  );

/**
  Function used to set the training mode of the controller chip of the AEP DIMM (FalconValley).

  @param[in, out] This             - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket           - Zero based CPU socket number.
  @param[in]      Controller       - Zero based controller number.
  @param[in]      Channel          - Zero based channel number.
  @param[in]      Dimm             - Zero based DIMM number.
  @param[in]      TrainingModeMask -

  @retval None.
**/
VOID
(EFIAPI BiosSetAepDimmTrainingMode) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  FNV_TRAINING_MODE     Mode
  );

/**
  Function used to reset the I/O for a given AEP memory DIMM.

  @param[in, out] This             - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket           - Zero based CPU socket number.
  @param[in]      Controller       - Zero based controller number.
  @param[in]      Channel          - Zero based channel number.
  @param[in]      Dimm             - Zero based DIMM number.

  @retval None.
**/
VOID
(EFIAPI BiosAepIoReset) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm
  );

/**
  Function used to drain the RPQ for a given AEP memory DIMM.

  @param[in, out] This             - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket           - Zero based CPU socket number.
  @param[in]      Controller       - Zero based controller number.
  @param[in]      Channel          - Zero based channel number.
  @param[in]      Dimm             - Zero based DIMM number.

  @retval None.
**/
VOID
(EFIAPI BiosAepRpqDrain) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm
  );

/**
  Function used to clear prefetch cache miss of AEP DIMMs for a given socket.

  @param[in, out] This             - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket           - Zero based CPU socket number.

  @retval None.
**/
VOID
(EFIAPI BiosAepClrPrefetchCache) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket
  );

/**
  Function used to perform FIFO reset for all the AEP DIMMs for a given socket.

  @param[in, out] This             - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket           - Zero based CPU socket number.

  @retval None.
**/
VOID
(EFIAPI BiosAepFifoTrainReset) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket
  );

#pragma pack (pop)
#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG

#endif // __BIOS_SSA_MEMORY_DDRT_CONFIG__

// end file BiosSsaMemoryDdrtConfig.h
