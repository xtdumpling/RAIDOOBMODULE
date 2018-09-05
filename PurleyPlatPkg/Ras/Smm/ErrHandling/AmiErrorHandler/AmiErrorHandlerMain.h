//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

	
#ifndef _AMI_ERRORHANDLER_MAIN_H
#define _AMI_ERRORHANDLER_MAIN_H

#include <OemRasLib/OemRasLib.h>

#define MC_ERR_INJ_LCK                             0x790

// Function prototype declarations

/**
  Entry point for the AMI Error Handler initialization. 

  This function initializes AMI Error handling.
  

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval Status.

**/

EFI_STATUS
AmiLogErrorWrapper (
  IN      UEFI_ERROR_RECORD_HEADER   *ErrorRecordHeader
  );



#endif
//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
