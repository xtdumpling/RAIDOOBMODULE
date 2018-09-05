//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
/** @file AmiNetworkPostManagerLib.h
    AmiNetworkPostManagerLib Definitions

**/
//*************************************************************************
#ifndef _AMI_NETWORK_POSTMANAGER_LIB_H_
#define _AMI_NETWORK_POSTMANAGER_LIB_H_

VOID 
AMICreatePopUp(
    IN  CHAR16          *String
);

VOID 
AMIPrintText(
    IN  CHAR16          *String
);

VOID 
AMISwitchToPostScreen (
);

VOID 
ClearGraphicsScreen (
 VOID
);

EFI_STATUS
CheckInvalidCharinIpAddress (
  IN CONST CHAR16       *IpSource,
  IN UINT8              IpMode
  );

EFI_STRING
EFIAPI
HiiGetString (
  IN EFI_HII_HANDLE  HiiHandle,
  IN EFI_STRING_ID   StringId,
  IN CONST CHAR8     *Language  OPTIONAL
  );


CHAR16* NetLibHiiGetString( 
  EFI_HII_HANDLE HiiHandle, 
  EFI_STRING_ID StringId,
  CHAR8 *Language
  );

#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

