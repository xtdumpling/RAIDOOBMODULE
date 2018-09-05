//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
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
// $Header: /Alaska/SOURCE/Modules/TCG/TCGSmm/TCGSmm.h 5     8/09/11 6:28p Fredericko $
//
// $Revision: 5 $
//
// $Date: 8/09/11 6:28p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: TCGSmm.h
//
// Description:
// Header file for TCGSMM subcomponent
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef _TCGSMM_H_
#define _TCGSMM_H_

//#include <AmiDxeLib.h>
//#include <Protocol\SmmThunk.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SmmSwDispatch.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Token.h>
//#include <AmiDxeLib.h>
#include <Hob.h>
#include <AmiTcg/TCGMisc.h>
#include <AmiTcg/tcg.h>
#include <Guid/AmiTcgGuidIncludes.h>



extern EFI_GUID SmmRsTableGuid;

//
// The return code for Sumbit TPM Request to Pre-OS Environment
// and Sumbit TPM Request to Pre-OS Environment 2
//
#define TREE_PP_SUBMIT_REQUEST_TO_PREOS_SUCCESS                                  0
#define TREE_PP_SUBMIT_REQUEST_TO_PREOS_NOT_IMPLEMENTED                          1
#define TREE_PP_SUBMIT_REQUEST_TO_PREOS_GENERAL_FAILURE                          2
#define TREE_PP_SUBMIT_REQUEST_TO_PREOS_BLOCKED_BY_BIOS_SETTINGS                 3


//
// The return code for Get User Confirmation Status for Operation
//
#define TREE_PP_GET_USER_CONFIRMATION_NOT_IMPLEMENTED                                 0
#define TREE_PP_GET_USER_CONFIRMATION_BIOS_ONLY                                       1
#define TREE_PP_GET_USER_CONFIRMATION_BLOCKED_BY_BIOS_CONFIGURATION                   2
#define TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_REQUIRED                     3
#define TREE_PP_GET_USER_CONFIRMATION_ALLOWED_AND_PPUSER_NOT_REQUIRED                 4


//
// The return code for Return TPM Operation Response to OS Environment
//
#define PP_RETURN_TPM_OPERATION_RESPONSE_SUCCESS                   0
#define PP_RETURN_TPM_OPERATION_RESPONSE_FAILURE                   1

//
// The return code for Memory Clear Interface Functions
//
#define MOR_REQUEST_SUCCESS                                        0
#define MOR_REQUEST_GENERAL_FAILURE                                1

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2005, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
