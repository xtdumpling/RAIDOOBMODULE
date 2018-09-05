//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/05/2014
//
//****************************************************************************
//****************************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  PeiSimpleTextOutPpi.h
//
// Description: Header file for Pei SimpleTextOut Ppi OEM function.
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef __PEI_SIMPLETEXTOUT_PPI_H_
#define __PEI_SIMPLETEXTOUT_PPI_H_
#ifdef __cplusplus
extern "C" {
#endif
#ifndef __USE_EDK_LIB
#include <EFI.h>
#endif

// {33b79657-8bc9-4920-a9b5-35205889b917}
#define EFI_PEI_SIMPLETEXTOUT_PPI_GUID \
  { 0x33b79657, 0x8bc9, 0x4920, 0xa9, 0xb5, 0x35, 0x20, 0x58, 0x89, 0xb9, 0x17 }

//static EFI_GUID gEfiPeiSimpleTextOutPPIGuid = EFI_PEI_SIMPLETEXTOUT_PPI_GUID;
GUID_VARIABLE_DECLARATION(gEfiPeiSimpleTextOutPPIGuid, EFI_PEI_SIMPLETEXTOUT_PPI_GUID);

/////////////////////////////////////////
// EFI PEI SimpleTextOut PPI Structure //
/////////////////////////////////////////

typedef struct _EFI_PEI_SIMPLETEXTOUT_PPI EFI_PEI_SIMPLETEXTOUT_PPI;

typedef EFI_STATUS
(EFIAPI *EFI_PEI_TEXT_CLEAR_SCREEN) (VOID);

typedef EFI_STATUS
(EFIAPI *EFI_PEI_TEXT_ENABLE_CURSOR) (
  IN  BOOLEAN                         Cursor
  );

typedef VOID
(EFIAPI *EFI_PEI_TEXT_CLEAR_LINE) (
  IN  UINT8                         Yaxis
  );

typedef VOID
(EFIAPI *EFI_PEI_TEXT_WRITE_CHAR) (
  IN  UINT8                        Xaxis,
  IN  UINT8                        Yaxis,
  IN  UINT8                        Attrib,
  IN  UINT8                        Char
  );

typedef UINT8
(EFIAPI *EFI_PEI_TEXT_OUTPUT_STRING) (
  IN  UINT8                            Xaxis,
  IN  UINT8                            Yaxis,
  IN  UINT8                            Attrib,
  IN  CHAR8                           *OutString
  );

typedef VOID
(EFIAPI *EFI_PEI_TEXT_POSTPROGRESS_MESSAGE) (
  IN  UINT8                           Yaxis,
  IN  CHAR8                          *OutString
  );

typedef struct _EFI_PEI_SIMPLETEXTOUT_PPI
{
  EFI_PEI_TEXT_CLEAR_SCREEN     ClearScreen;
  EFI_PEI_TEXT_ENABLE_CURSOR    EnableCursor;
  EFI_PEI_TEXT_WRITE_CHAR             WriteChar;
  EFI_PEI_TEXT_CLEAR_LINE             ClearLine;
  EFI_PEI_TEXT_OUTPUT_STRING    OutputString;
  EFI_PEI_TEXT_POSTPROGRESS_MESSAGE   PostProgressMessage;
} EFI_PEI_SIMPLETEXTOUT_PPI;


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
