/** @file 
  This is Chipset Error Reporting library Interface 
  
Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/
#ifndef _CHIPSET_ERR_REPORTING_H_
#define _CHIPSET_ERR_REPORTING_H_

//
// Includes
//
#include <Uefi.h>
#include <Cpu/CpuRegs.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Library/MmPciBaseLib.h>
#include <Library/IoLib.h>




//
// Defines
//
#define PCIE_EXT_CAP_HEADER_LERC                    0x000b
#define PCIE_EXT_CAP_HEADER_LERC_REVISION           0x0002
#define PCIE_EXT_CAP_HEADER_LER_VSECID              0x0005

//
// Type Definitions
//
//

typedef struct {
  UINT8 Enabled;
  UINT8 Bus;
  UINT8 Device;
  UINT8 Function;
  UINT8 StackPerPort;
} PCIE_PORT_INFO;

typedef	struct {
  UINT8 StackValid;
  UINT8 StackBus;
} STACK_INFO;

typedef struct {
  UINT8 BusNum;
  UINT8 SocketId;
  UINT8 Core20BusNum;
  UINT8 FpgaStack;
  UINT8 Valid;
  PCIE_PORT_INFO Port[NUMBER_PORTS_PER_SOCKET];
  STACK_INFO     IioStackInfo[MAX_IIO_STACK];
} IOH_INFO;


//
// Functions prototype declarations 
//

/**
  Mask IIO Errors.

  This function masks IIO errors reporting.

    @param None

  @retval Status.
**/
EFI_STATUS
MaskIIOErrorReporting(
  VOID
);

/**
  Clears local and global IIO Errors.

  This function clears local and global IIO status registers.

  @param[in] SktId   The socket ID.

  @retval  None.

**/
VOID
ClearIioErrorRegisterStatus (
  UINT8 SktId
  );

/**
  Clear all IIO Errors.

  This function masks IIO errors reporting.

    @param None

  @retval None.
**/
VOID
ClearIioErrors(
  VOID
);

/**
  Check For Iio Errors.

  This function check if IioErrors is enabled then check
  if any errors were reported in the GSYSST status register

  @retval  BOOLEAN  return TRUE if an IIO error is detected
**/
BOOLEAN
CheckForIioErrors (
  VOID
  );

/**
  This function enable Disable IoMca.

  @param[in] Buffer        The pointer to private data buffer.

  @retval    Status.
**/ 
EFI_STATUS
IIODisableIoMca (
  IN      VOID    *Buffer
  );

/**
  This function enable Enable IoMca.

  @param[in] Buffer        The pointer to private data buffer.

  @retval    Status.
**/
EFI_STATUS
IIOEnableIoMca (
  IN      VOID  *Buffer
);

/**
  This function will enable IIO error reporting.

  @param[in] RootBridge        The socket number.

  @retval    Status.
**/
EFI_STATUS
IIOErrorEnable(
  IN      UINT8   RootBridge
  );

/**
  This function puts the machine in a CpuDeadLoop if the socket 
  is in viral state.

  @retval Status.
**/
EFI_STATUS
ProcessViralError (
   VOID
   );

/**
  IVT-EX sighting 4031306 : CLONE from ivytown: IVT B0 - RAS - SMI not getting delivered
  to UBox when in VIRAL & SMI is generated on fatal error.

  @retval    None.
**/
VOID
SetupIioCipIntrCtrl (
  IN      UINT8   RootBridgeLoop
  );

/**
  IIO error main handler routine.
    
  This function performs the IOH error checking and processing functions

  @retval Status.
**/
EFI_STATUS
ProcessIioErrors(
  VOID
);

/**
  This function initialize the IOH info structure.

  @retval    None.
**/
VOID
InitIOHInfo (
   VOID
   );


// IOH_DATA structure is used in runtime when IOH UDS protocol is not available.
extern IOH_INFO  mIohInfo[MAX_SOCKET];

/**
  Initialize PCIE_IIO specific RAS capabilities. 

  @param[in, out] Capability  Pointer to the Silicon Ras capability structure.

  @retval Status.

**/
EFI_STATUS
InitPcieSiliconCap (
  IN       OUT   SILICON_RAS_CAPABILITY  *Capability
  );

/**
  We need to clear any pending GPI SMI status to avoid a EOS.
  
  @retval    None
--*/
VOID
ClearGpiSmiStatus (
  VOID
  );




#endif //_CHIPSET_ERR_REPORTING_H_

