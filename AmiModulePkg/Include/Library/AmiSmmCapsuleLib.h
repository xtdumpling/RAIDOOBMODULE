//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************l
/** @file
  @brief Defines AmiSmmCapsule library class.

  Capsule Scheduling Service.
**/
#ifndef __AMI_SMM_CAPSULE_LIB__H__
#define __AMI_SMM_CAPSULE_LIB__H__

/**
  Schedules capsule for processing after system reset.

  @param[in]  CapsuleHeaderArray Pointer to a capsule to schedule.

  @return  EFI_STATUS
*/
EFI_STATUS AmiScheduleCapsule(IN EFI_CAPSULE_HEADER *CapsuleHeaderArray);

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
