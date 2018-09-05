//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/AMITSEBoard/TseBoard.c $
//
// $Author: Madhans $
//
// $Revision: 5 $
//
// $Date: 2/19/10 12:58p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

#include "Token.h" 
#include <Efi.h>
#include <Protocol/AMIPostMgr.h>
#include "PwdLib.h"

// This Module is to implement the OME Functions, which can be override the existing TSE functions.
// For Example, UEFI Callback 
// If Board module handle the Passwordwith Formcallback and return sccuess then 
// TSE will not takecare of Password handling.



//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
