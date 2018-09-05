/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


    @file SioInitPeim.c
   
    Functions for LpcSio initilization
    
**/
// APTIOV_SERVER_OVERRIDE_RC_START : Use AMI SIO module for SIO initialization
#if 0
#include "PlatformEarlyInit.h"


STATIC SIO_INDEX_DATA  mW83527Table [] = {  
  //
  // Enter Configuration Mode
  //
  { W83527_SIO_INDEX_PORT, 0x87 },
  { W83527_SIO_INDEX_PORT, 0x87 },
  //
  // GPIO Config
  //
  { W83527_SIO_INDEX_PORT, W83527_LOGICAL_DEVICE },       // 0x7
  { W83527_SIO_DATA_PORT,  W83527_SIO_GPIO },             // 0x9
  //
  // Disable  GPIO 2 to make Keyboard work
  //
  { W83527_SIO_INDEX_PORT, W83527_ACTIVATE_REG }, //CR 0x60
  { W83527_SIO_DATA_PORT,  0x0 },                 // 0x00
  //
  // Exit Configuration Mode
  //
  { W83527_SIO_INDEX_PORT, 0xAA }
};


/**

  Initialize the GPIO IO selection, GPIO USE selection, and GPIO signal inversion registers.

  @param PeiServices  -  PeiService point.
  @param CpuIo        -  CpuIo PPI to read/write IO ports.

  @retval EFI_SUCCESS  -  Init succeed.

**/
VOID
LpcSioEarlyInit (
  IN     SYSTEM_BOARD_PPI               *SystemBoard
  )
{
  UINT8 Index;
  UINT32 SioExist;
  EFI_STATUS Status;

#ifdef ASPEED_ENABLE
    PchLpcGenIoRangeSet ((0x600  & 0xFF0), 0x10, LPC_ESPI_FIRST_SLAVE);

    IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);  
    IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);  

    //
    //mailbox
    //
    IoWrite8 (ASPEED2500_SIO_INDEX_PORT, REG_LOGICAL_DEVICE);
    IoWrite8 (ASPEED2500_SIO_DATA_PORT, ASPEED2500_SIO_MAILBOX);

    IoWrite8 (ASPEED2500_SIO_INDEX_PORT, BASE_ADDRESS_HIGH0);
    IoWrite8 (ASPEED2500_SIO_DATA_PORT, (UINT8)(0x600 >> 8));

    IoWrite8 (ASPEED2500_SIO_INDEX_PORT, BASE_ADDRESS_LOW0);
    IoWrite8 (ASPEED2500_SIO_DATA_PORT, (UINT8)(0x600 & 0xFF));
    //
    //active mailbox
    //
    IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ACTIVATE);
    IoWrite8 (ASPEED2500_SIO_DATA_PORT, 1);

    IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_LOCK); 
#endif
  //
  //due to following code only int pilotiv, skit it if pilotiv is not exist.
  //
  SioExist = IsSioExist();
  if((SioExist & PILOTIV_EXIST) == 0){
    return;
  }
    
  
  //
  // Do early SIO GPIO init
  //
  for (Index = 0; Index < SystemBoard->SystemBoardInfo->NumSioItems; Index++) { // Use SystemBoard PPI
    IoWrite8 (
      SystemBoard->SystemBoardInfo->SioIndexPort,                 // Use SystemBoard PPI to get the index port (ie: PILOTIV_SIO_INDEX_PORT)
      SystemBoard->SystemBoardInfo->mSioInitTable[Index].Index    // Use SysetmBoard PPI to get the index value
      );

    IoWrite8 (
      SystemBoard->SystemBoardInfo->SioDataPort,                  // Use SystemBoard PPI to get the data port (ie: PILOTIV_SIO_DATA_PORT)
      SystemBoard->SystemBoardInfo->mSioInitTable[Index].Value    // Use SysetmBoard PPI to get the data value
      );
  }
  Status = PchLpcGenIoRangeSet(PILOTIV_KCS3_DATA_BASE_ADDRESS & ~0xF, 0x10, LPC_ESPI_FIRST_SLAVE);
  ASSERT_EFI_ERROR(Status);
  DEBUG((DEBUG_INFO, "[SIO] PilotIV KCS base at 0x%X\n", PILOTIV_KCS3_DATA_BASE_ADDRESS & ~0xF));
  
  Status = PchLpcGenIoRangeSet(PILOTIV_MAILBOX_BASE_ADDRESS, 0x10, LPC_ESPI_FIRST_SLAVE);
  ASSERT_EFI_ERROR(Status);
  DEBUG((DEBUG_INFO, "[SIO] PilotIV mailbox at  0x%X\n", PILOTIV_MAILBOX_BASE_ADDRESS));
}
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Use AMI SIO module for SIO initialization

