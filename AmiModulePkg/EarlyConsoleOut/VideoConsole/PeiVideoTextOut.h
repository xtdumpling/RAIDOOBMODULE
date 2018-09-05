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

/** @file PeiVideoTextOut.h
    Definitions used by PeiVideoTextOut.

**/

#ifndef _PEI_VIDEO_TEXT_OUT_H__
#define _PEI_VIDEO_TEXT_OUT_H__

#ifdef __cplusplus
extern "C" {
#endif


//---------------------------------------------------------------------------

#include <Token.h>
#include <AmiPeiLib.h>
#include <Library/HobLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Ppi/AmiPeiTextOut.h>
#include <AmiVideoTextOutLib.h>
#include <AmiTextOutHob.h>
//---------------------------------------------------------------------------

EFI_STATUS
EFIAPI
VideoConQueryMode (
    IN AMI_PEI_TEXT_OUT    *This,
    IN UINT8               Mode,
    IN OUT UINT8           *Col,
    IN OUT UINT8           *Row
);

EFI_STATUS
EFIAPI
VideoConWriteString (
    IN AMI_PEI_TEXT_OUT    *This,
    IN UINT8               Side,
    IN UINT8               *Text
);

EFI_STATUS
EFIAPI
VideoConSetCursorPosition (
    IN AMI_PEI_TEXT_OUT    *This, 
    IN UINT8                Col,
    IN UINT8                Row
);

EFI_STATUS
EFIAPI
VideoConSetAttribute (
    IN AMI_PEI_TEXT_OUT    *This, 
    UINT8                   Foreground,
    UINT8                   Background,
    IN BOOLEAN              Blink
);

EFI_STATUS
EFIAPI
VideoConEnableCursor (
    IN AMI_PEI_TEXT_OUT    *This,
    IN BOOLEAN              Enable
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
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
