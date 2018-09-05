//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix : Fixed TXT/TPM Support/Enable Status Discovery failed when SPS
//              file system is empty for SW SUM team to develop SMC PPI-x OOB 
//              feature.
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Mar/07/2017
//
//****************************************************************************
/** @file
  Execute pending Out-Of-Band One-Touch Request.

  Caution: This module requires additional review when modified.
  This driver will have external input - variable.
  This external input must be validated carefully to avoid security issue.

  Tpm2ExecutePendingTpmRequest() will receive untrusted input and do validation.

Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials 
are licensed and made available under the terms and conditions of the BSD License 
which accompanies this distribution.  The full text of the license may be found at 
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>

// APTIOV_SERVER_OVERRIDE_START : Removed unnecessary header files and added AMI needs
#if 0
#include <Protocol/Tcg2Protocol.h>
#endif
#include <Protocol/VariableLock.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#if 0
#include <Library/UefiDriverEntryPoint.h>
#endif
#include <Library/UefiBootServicesTableLib.h>
#if 0
#include <Library/UefiLib.h>
#endif
#include <Library/MemoryAllocationLib.h>
#if 0
#include <Library/PrintLib.h>
#include <Library/HiiLib.h>
#endif
#include <Library/HobLib.h>

#if 0
#include "../../Include/Guid/Tcg2PhysicalPresenceData.h"
#include "../../Include/Library/Tcg2PhysicalPresenceLib.h"
#include "../../Include/Library/Tcg2PpVendorLib.h"
#endif

#include <Guid/EventGroup.h>
//#include <Guid/Tcg2PhysicalPresenceData.h>
#if 0
#include <Library/Tpm2CommandLib.h>
#endif
//#include <Library/Tcg2PhysicalPresenceLib.h>
//#include <Library/Tcg2PpVendorLib.h>

#include <PpixOobPlatform.h>
#include <PpixOobTask.h>
// APTIOV_SERVER_OVERRIDE_END : Removed unnecessary header files and added AMI needs
#include <Me/Include/Library/MeStorageLib.h>
#include <Token.h> //SMCPKG_SUPPORT

#define	JMP_$()	{volatile int j; j = 1; while (j);}

//-----------------------------------------------------------------------------------------------------
//
EFI_GUID    mEfiOobTxtTpmDataGuid = EFI_OOB_TXT_TPM_DATA_GUID;	// EFI Variable GUID for OOB Data
//
BOOLEAN	    MeNvStorage = FALSE;				// ME NV Storage Not Supported
//
//-----------------------------------------------------------------------------------------------------
// OOB Tasks that require authentication
//
UINT8  AuthenticateTaskTable[] = {
		OXP_TASK_ENABLE_TXT_DTPM,
		OXP_TASK_DTPM_OWNER_CLEAR_ONLY,
		OXP_TASK_DTPM_CLEAR_DTPM_ACTIVATION,
		OXP_TASK_DTPM_CLEAR_ENABLE_TXT_DTPM,
		OXP_TASK_ENABLE_TXT_FTPM,
		OXP_TASK_FTPM_OWNER_CLEAR_ONLY,
		OXP_TASK_FTPM_CLEAR_FTPM_ACTIVATION,
		OXP_TASK_FTPM_CLEAR_ENABLE_TXT_FTPM,
		OXP_TASK_DISABLE_TXT_ONLY,
		OXP_TASK_DISABLE_DTPM_ONLY,
		OXP_TASK_DISABLE_FTPM_ONLY,
		};
//

//-----------------------------------------------------------------------------------------------------
/**
  AuthenticationOobTaskRequired(): Find if authentication is required for the requested task.

  @param  Ott		INPUT:  Ptr to OOB_TXT_TPM data

  @retval Result	OUTPUT: BOOLEAN, TRUE/FALSE -> Authentication Yes/Not required
**/
BOOLEAN
AuthenticationOobTaskRequired (
  IN  OOB_TXT_TPM	*Ott
  )
{
  // Local data
  UINT8		i, t;
  BOOLEAN	a;

  // Check if authentication required for the task
  t = (Ott->Header.TaskAndResult & OXP_TASK_BIT_MASK)		// Requested Task#
					>> OXP_TASK_BIT_N;
  a = FALSE;							// FALSE: Authentication not required for the given task
  for (i = 0; i < (sizeof AuthenticateTaskTable); i++) {	// Search for the task in the table
    if (AuthenticateTaskTable[i] == t) { a = TRUE; break; }	// If task found in the table, Indicate authentication required, Stop
  }

  return a;							// Result
}

//-----------------------------------------------------------------------------------------------------
/**
  TpmPreparationOobTaskRequired(): Find if TPM preparation is required for the requested task.

  @param  Ott		INPUT:  Ptr to OOB_TXT_TPM data

  @retval Result	OUTPUT: BOOLEAN, TRUE/FALSE -> TPM preparation Yes/Not required
**/
BOOLEAN
TpmPreparationOobTaskRequired (
  IN  OOB_TXT_TPM	*Ott
  )
{
  if (((Ott->Header.TaskAndResult & OXP_TASK_BIT_MASK)		// If Disable TXT Only
		>> OXP_TASK_BIT_N) == OXP_TASK_DISABLE_TXT_ONLY) {
    return FALSE;						// TPM Preparation not required
  }

  return (AuthenticationOobTaskRequired (Ott));
}

//-----------------------------------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------------------------------
//---------------------	OOB Data Read, Write, Integrity, Initialization Functions { -------------------
//-----------------------------------------------------------------------------------------------------
/**
  ReadOobDataFromMeStorage(): Read OOB Data from ME Non-Volatile Storage.

  @param	Buffer	INPUT: VOID*, Ptr to buffer where data to be read
  @param	Size	INPUT/OUTPUT: UINTN*, Ptr to area having Size of buffer in bytes
				INPUT : Ptr to area having max size of supplied buffer in bytes
				OUTPUT: Ptr to area having size of data read in bytes

  @retval	Result	BOOLEAN, TRUE/FALSE: Success/Error
**/
BOOLEAN
ReadOobDataFromMeStorage (
  IN     VOID	*Buffer,
  IN OUT UINTN	*Size
  )
{
  // LOcal data
  EFI_STATUS	Status;
  UINTN		MeStatus = 0;
  BOOLEAN	b;

  DEBUG ((EFI_D_ERROR, "[OOB] ReadOobDataFromMeStorage()  { -> Input : Index 0x00, Buffer Size 0x%08x bytes\n", *Size));

  b = TRUE;							// Assume Success
  Status = MeStorageEntryRead (					// Read from ME Storage
		0,						//   Index
		(VOID*)(UINTN) &MeStatus,			//   Ptr to detail Error
		Buffer,						//   Ptr to buffer
		(UINT32*)(UINTN) Size);				//   Ptr to buffer size

  DEBUG ((EFI_D_ERROR, "[OOB] ReadOobDataFromMeStorage()  } -> Output: EFI Status 0x%08x, ME Protocol Status 0x%08x", Status, MeStatus));
  if (Status == EFI_SUCCESS) {					// If success
    DEBUG ((EFI_D_ERROR, ", OOB Data Size read 0x%08x bytes -> Success\n", *Size));
  } else {
    DEBUG ((EFI_D_ERROR, "-> Error\n"));
    b = FALSE;							// Error
  }

  return b;							// Result
}

//-----------------------------------------------------------------------------------------------------
/**
  WriteOobDataToMeStorage(): Write OOB Data to ME Non-Volatile Storage.

  @param	Buffer	INPUT: VOID*, Ptr to buffer containing data to be written
  @param	Size	INPUT: UINTN*, Ptr to area having Size of buffer in bytes

  @retval	Result	BOOLEAN, TRUE/FALSE: Success/Error
**/
BOOLEAN
WriteOobDataToMeStorage (
  IN     VOID	*Buffer,
  IN OUT UINTN	*Size
  )
{
  // LOcal data
  EFI_STATUS	Status;
  UINTN		MeStatus = 0;
  BOOLEAN	b;

  DEBUG ((EFI_D_ERROR, "[OOB] WriteOobDataToMeStorage()  { -> Input : Index 0x00, Data Size 0x%08x bytes\n", *Size));

  b = TRUE;							// Assume Success
  Status = MeStorageEntryWrite (				// Write to ME Storage
		0,						//   Index
		(VOID*)(UINTN) &MeStatus,			//   Ptr to detail Error
		Buffer,						//   Ptr to buffer
		((UINT32)(UINTN) *Size));			//   Buffer size
  DEBUG ((EFI_D_ERROR, "[OOB] WriteOobDataToMeStorage()  } -> Output: EFI Status 0x%08x, ME Protocol Status 0x%08x, ", Status, MeStatus));
  if (Status == EFI_SUCCESS) {					// If success
    DEBUG ((EFI_D_ERROR, "Success\n"));
  } else {
    DEBUG ((EFI_D_ERROR, "Error\n"));
    b = FALSE;							// Error
  }

  return b;							// Result
}

//-----------------------------------------------------------------------------------------------------
/**
  ReadOobDataFromEfiVariable(): Read OOB Data from EFI Variable.

  @param	Buffer	INPUT: VOID*, Ptr to buffer where data to be read
  @param	Size	INPUT/OUTPUT: UINTN*, Ptr to area having Size of buffer in bytes
				INPUT : Ptr to area having max size of supplied buffer in bytes
				OUTPUT: Ptr to area having size of data read in bytes

  @retval	Result	BOOLEAN, TRUE/FALSE: Success/Error
**/
BOOLEAN
ReadOobDataFromEfiVariable (
  IN     VOID	*Buffer,
  IN OUT UINTN	*Size
  )
{
  // LOcal data
  EFI_STATUS	Status;
  BOOLEAN	b;

  DEBUG ((EFI_D_ERROR, "[OOB] ReadOobDataFromEfiVariable()  { -> Input : Buffer Size 0x%08x bytes\n", *Size));

  b = TRUE;							// Assume Success
  Status = gRT->GetVariable (					// Read OOB data
		EFI_OOB_TXT_TPM_DATA_VARIABLE,
		&mEfiOobTxtTpmDataGuid,
		NULL,
		Size,
		Buffer);
  DEBUG ((EFI_D_ERROR, "[OOB] ReadOobDataFromEfiVariable()  } -> "));
  if (Status == EFI_SUCCESS) {					// If success
    DEBUG ((EFI_D_ERROR, "OOB Data Size read = 0x%08x bytes -> Success\n", *Size));
  } else {
    b = FALSE;							// Error
    DEBUG ((EFI_D_ERROR, "Error\n"));
  }

  return b;							// Result
}

//-----------------------------------------------------------------------------------------------------
/**
  WriteOobDataToEfiVariable(): Write OOB Data to EFI Variable.

  @param	Buffer	INPUT: VOID*, Ptr to buffer where data to be read
  @param	Size	INPUT: UINTN*, Ptr to area having Size of buffer in bytes

  @retval	Result	BOOLEAN, TRUE/FALSE: Success/Error

**/
BOOLEAN
WriteOobDataToEfiVariable (
  IN     VOID	*Buffer,
  IN     UINTN	*Size
  )
{
  // LOcal data
  EFI_STATUS	Status;
  BOOLEAN	b;

  DEBUG ((EFI_D_ERROR, "[OOB] WriteOobDataToEfiVariable()  { -> Input : OOB Data Size to write 0x%08x bytes\n", *Size));

  b = TRUE;							// Assume Success
  Status = gRT->SetVariable (					// Write OOB data
                  EFI_OOB_TXT_TPM_DATA_VARIABLE,
                  &mEfiOobTxtTpmDataGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  *Size,
                  Buffer);
  DEBUG ((EFI_D_ERROR, "[OOB] WriteOobDataToEfiVariable()  } -> "));
  if (Status == EFI_SUCCESS) {					// If success
    DEBUG ((EFI_D_ERROR, "Success\n"));
  } else {
    b = FALSE;							// Error
    DEBUG ((EFI_D_ERROR, "Error\n"));
  }

  return b;							// Result
}

//-----------------------------------------------------------------------------------------------------
/**
  WriteOobDataToNvStorage(): Write OOB One-Touch Data to Non-Volatile Storage.

  @param	Buffer	INPUT: VOID*, Ptr to buffer conatining data to be written
  @param	Size	INPUT: UINTN*, Ptr to area having Size of buffer in bytes

  @retval	Result	BOOLEAN, TRUE/FALSE: Success/Error
**/
BOOLEAN
WriteOobDataToNvStorage (
  IN     VOID	*Buffer,
  IN     UINTN	*Size
  )
{
  // LOcal data
  BOOLEAN	b;

  DEBUG ((EFI_D_ERROR, "[OOB] WriteOobDataToNvStorage()  { -> Write OOB Data to Non-Volatile Storage\n"));

  if (MeNvStorage) {						// If ME Storage supported
    b = WriteOobDataToMeStorage (Buffer, Size);			//   Use ME Storage
  } else {
    b = WriteOobDataToEfiVariable (Buffer, Size);		//   Use EFI Variable
  }

  DEBUG ((EFI_D_ERROR, "[OOB] WriteOobDataToNvStorage()  } -> "));
  if (b) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return b;							// Success/Error
}

//-----------------------------------------------------------------------------------------------------
/**
  ReadOobDataFromNvStorage(): Read OOB Data from Non-Volatile Storage.

  @param	Buffer	INPUT: VOID*, Ptr to buffer where data to be read
  @param	Size	INPUT/OUTPUT: UINTN*, Ptr to area having Size of buffer in bytes
				INPUT : Ptr to area having max size of supplied buffer in bytes
				OUTPUT: Ptr to area having size of data read in bytes

  @retval	Result	BOOLEAN, TRUE/FALSE: Success/Error
**/
BOOLEAN
ReadOobDataFromNvStorage (
  IN     VOID	*Buffer,
  IN OUT UINTN	*Size
  )
{
  // Local data
  BOOLEAN	b;

  DEBUG ((EFI_D_ERROR, "[OOB] ReadOobDataFromNvStorage()  { -> Read OOB Data from Non-Volatile Storage\n"));

  if (MeNvStorage) {						// If ME Storage supported
    b = ReadOobDataFromMeStorage (Buffer, Size);		//   Read from ME Storage
  } else {
    b = ReadOobDataFromEfiVariable (Buffer, Size);		//   Read from EFI Variable
  }

  DEBUG ((EFI_D_ERROR, "[OOB] ReadOobDataFromNvStorage()  } -> "));
  if (b) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return b;							// Success/Error
}

//-----------------------------------------------------------------------------------------------------
/**
  GetOobNvStorageSize(): Return OOB Non-Volatile Storage size.

  @param	None

  @retval	Result	UINTN, Max size of OOB Non-Volatile Storage in bytes
**/
UINTN
GetOobNvStorageSize (
  )
{
  if (MeNvStorage) { return (ME_STORAGE_MAX_BUFFER); }		// ME NV storage Max Size in bytes
  return ((UINTN)						// EFI Variable Max Size in bytes (rounded up to next para)
	  (((UINT16) sizeof (OOB_TXT_TPM) + (UINT16) 0x000F) & (UINT16) 0xFFF0));
}

//-----------------------------------------------------------------------------------------------------
/**
  OobNvStorageSupported(): Check if OOB Non-Volatile Storage is supported.

  @param	None

  @retval	Result	BOOLEAN, TRUE/FALSE: Yes/Not supported

**/
BOOLEAN
OobNvStorageSupported (
  )
{
  return (MeNvStorage = MeStorageIsSupported());		// TRUE/FALSE = ME NV storage yes/not Supported
}

//-----------------------------------------------------------------------------------------------------
/**
  InitOobOneTouchData(): Initialize OOB data with default values.

  @param  Ott	INPUT: Ptr to OOB_TXT_TPM data
  @param  Opd	INPUT: Ptr to OT_PLATFORM_DATA
  @param  Size	INPUT/OUTPUT: UINTN*, Ptr to area containing OOB Data Size in bytes
				INPUT : Ptr to area containing OOB Data Size in bytes as read from NV space
				OUTPUT: Ptr to area containing OOB Data Size in bytes as generated

  @retval None

**/
VOID
InitOobOneTouchData (
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINTN			*Size
  )
{
  DEBUG ((EFI_D_ERROR, "[OOB] InitOobOneTouchData()  { -> Initialize OOB data with default values\n"));

  // Init OOB data with 0
  ZeroMem ((VOID*) Ott, sizeof (OOB_TXT_TPM));				// Init with 0

  // Init OXP_HEADER: Chosen Default values: Outout OOB data of Task 1 (TXT/TPM Support Discovery)
  Ott->Header.Signature = OXP_SIGNATURE_OUTPUT;				// Signature
  Ott->Header.TotalLength = sizeof (OXP_HEADER);			// Total Length in bytes (just OXP_HEADER)
  Ott->Header.HeaderLength = sizeof (OXP_HEADER);			// Header Length in bytes
  Ott->Header.Version = OXP_VERSION;					// Version
  Ott->Header.TaskAndResult = (OXP_TASK_PERFORMED_BIT |			// Task Performed
				OXP_TASK_TXT_TPM_SUPPORT_DISCOVERY);	// Use-Case 1
  Ott->Header.Status = OXP_SUCCESS;					// Status
  Ott->Header.PasswordAttribute = Opd->PasswordAttribute;		// Password Attribute
  Ott->Header.FeatureSupported = Opd->FeatureSupported;			// Features supported
  Ott->Header.FeatureEnabled = Opd->FeatureEnabled;			// Features enabled

/*
  // Activate following data to test a particular Task
  UINT8	i = 0;
  UINT8	n = 0;								// Password length in bytes excluding NULL termination

  Ott->Header.Signature = OXP_SIGNATURE_INPUT;				// Signature
  //Ott->Header.TaskAndResult = OXP_TASK_DTPM_CLEAR_ENABLE_TXT_DTPM;	// Use-Case 6
  //Ott->Header.TaskAndResult = OXP_TASK_FTPM_CLEAR_ENABLE_TXT_FTPM;	// Use-Case 14
  //Ott->Header.TaskAndResult = OXP_TASK_DISABLE_TXT_ONLY;		// Use-Case 15
  //Ott->Header.TaskAndResult = OXP_TASK_DISABLE_DTPM_ONLY;		// Use-Case 16
  //Ott->Header.TaskAndResult = OXP_TASK_DISABLE_FTPM_ONLY;		// Use-Case 17
  //Ott->Header.TaskAndResult = OXP_TASK_DTPM_CLEAR_DTPM_ACTIVATION;	// Use-Case 5
  Ott->Header.TaskAndResult = OXP_TASK_FTPM_CLEAR_FTPM_ACTIVATION;	// Use-Case 13
  Ott->Header.Status = OXP_UNKNOWN_ERROR;				// Status

  if (n != 0) {								// If password to be used
    Ott->Header.TotalLength = sizeof (OXP_HEADER) + (n + 1) + 0x05;	//   Total Length in bytes (OXP_HEADER and OXP_PASSWORD_DATA)
  // Init OXP_PASSWORD_DATA
  Ott->PasswordData.Id = OXP_ID_PASSWORD;				// Identifier
  Ott->PasswordData.Attribute = (OXP_PASSWORD_DATA_VALID_BIT |		// Bit-mapped Attribute
					OXP_PASSWORD_ASCII_BIT);
  Ott->PasswordData.Length = (n + 1) + 0x05;				// Length of OXP_PASSWORD_DATA in bytes
  for (i = 0; i < n; i++) { Ott->PasswordData.Password[i] = (i+0x61); }	// Fill Password ASCII characters ('a', 'b', 'c', ..... so on)
  Ott->PasswordData.Password[n] = 0x00;					// Fill Password NULL termination
  }
*/

  // Update checksum
  //Ott->Header.Checksum = 0;	// Checksum field already initialized with 0
  Ott->Header.Checksum = CalculateCheckSum8 (				// Update Checksum
  			(UINT8*)(UINTN) Ott, (UINTN) Ott->Header.TotalLength);

  // Generated OOB data size
  *Size = (UINTN) Ott->Header.TotalLength;				// Generated OOB data size in bytes

  DEBUG ((EFI_D_ERROR, "[OOB] InitOobOneTouchData()  }\n"));
}

//-----------------------------------------------------------------------------------------------------
/**
  ValidateOobOneTouchInputData(): Validate OOB Input data (supplied by caller).

  @param  Ott	INPUT: Ptr to OOB_TXT_TPM data
  @param  Size	INPUT/OUTPUT: UINTN*, Ptr to area containing OOB Data Size in bytes
				INPUT : Ptr to area containing OOB Data Size in bytes as read from NV space
				OUTPUT: Ptr to area containing actual OOB Data Size in bytes
  @param  State	INPUT/OUTPUT: UINT32*, Ptr to state of task

  @retval None

**/
VOID
ValidateOobOneTouchInputData (
  IN     OOB_TXT_TPM	*Ott,
  IN OUT UINTN		*Size,
  IN OUT UINT32		*State
  )
{
  // Local data
  UINT32	s;


  DEBUG ((EFI_D_ERROR, "[OOB] ValidateOobOneTouchInputData()  { -> Validate OOB Input Data\n"));
  DEBUG ((EFI_D_ERROR, "      Current State 0x%08x, OOB Data Size 0x%08x bytes\n", *State, *Size));

  s = 0;										// State

  // Validate OOB data integrity
  if ((Ott->Header.Signature != OXP_SIGNATURE_INPUT) &&					// If Signature not valid, indicate error
      (Ott->Header.Signature != OXP_SIGNATURE_OUTPUT)) { s |= OXP_STATE_SIGNATURE_INVALID_BIT; }
  if (Ott->Header.TotalLength == 0) { s |= OXP_STATE_TOTAL_LENGTH_INVALID_BIT; }	// If Total Length zero, Indicate error
  if ((Ott->Header.HeaderLength == 0) ||						// If Header Length zero OR
      (Ott->Header.HeaderLength != sizeof (OXP_HEADER))) {     				//    Header Length invalid
    s |= OXP_STATE_HEADER_LENGTH_INVALID_BIT;						//   Indicate error
  }
  if ((Ott->Header.TotalLength < Ott->Header.HeaderLength) ||				// If (Total Length < Header Length) OR
      (((UINTN) Ott->Header.TotalLength) > *Size)) {					//    (Total Length > Read OOB Data size)
    s |= OXP_STATE_TOTAL_LENGTH_LESS_HEADER_LENGTH_BIT;					//   Indicate error
  }
  if (Ott->Header.Version == 0) { s |= OXP_STATE_VERSION_INVALID_BIT; }			// If Version zero, indicate error
  if ((CalculateSum8 ((UINT8*)(UINTN) Ott, (UINTN) Ott->Header.TotalLength)) != 0) {	// If Checksum not valid
    s |= OXP_STATE_CHECKSUM_INVALID_BIT;						//   Indicate error
  }

  if (s == 0) {										// If OOB data valid
    if ((Ott->Header.TaskAndResult & OXP_TASK_PERFORMED_BIT) == 0) { 			//   If some task to be performed
      if (Ott->Header.Signature != OXP_SIGNATURE_INPUT)  {				//     If not input signature
	s |= OXP_STATE_SIGNATURE_INVALID_BIT;						//       Indicate error
      }
    } else {										//   If no task to be performed
      if (Ott->Header.Signature != OXP_SIGNATURE_OUTPUT) {				//     If not output signature
	s |= OXP_STATE_SIGNATURE_INVALID_BIT;						//       Indicate error
      }
    }
  }

  *Size = Ott->Header.TotalLength;							// Actual OOB data size in bytes
  *State |= s;										// Return updated State
  DEBUG ((EFI_D_ERROR, "                                           Updated State 0x%08x, OOB Data Size 0x%08x bytes\n", s, *Size));
  DEBUG ((EFI_D_ERROR, "[OOB] ValidateOobOneTouchInputData  }\n"));
}

//-----------------------------------------------------------------------------------------------------
/**
  GenerateOobOneTouchOutputData(): Generate OOB Output data (to be writeen to Non-Volatile storage).

  This function generates the OOB output data but MUST NOT write the data to non-volatile storage.

  @param  Ott	INPUT: Ptr to OOB_TXT_TPM data
  @param  Size	INPUT/OUTPUT: UINTN*, Ptr to area containing OOB Data Size in bytes
				INPUT : Ptr to area containing OOB Input Data Size in bytes
				OUTPUT: Ptr to area containing OOB Output Data Size in bytes
  @param  State	INPUT/OUTPUT: UINT32*, Ptr to state of task

  @retval None

**/
VOID
GenerateOobOneTouchOutputData (
  IN     OOB_TXT_TPM	*Ott,
  IN OUT UINTN		*Size,
  IN OUT UINT32		*State
  )
{
  // Local data
  UINT32	s;
  UINT16	w;
  UINT8		*p;

  DEBUG ((EFI_D_ERROR, "[OOB] GenerateOobOneTouchOutputData()  { ->  Generate OOB Output Data\n"));
  s = *State;							// State

  if ((s & (OXP_STATE_TASK_PENDING_BIT |			// If Task not pending AND OOB data not generated from scratch
		OXP_STATE_OOB_DATA_FRESH_GENERATED_BIT)) == 0) {
    // Clear password data (if present) from OOB input data (to delete the password information from non-volatile storage)
    p = (UINT8*)(UINTN) &Ott->PasswordData;			//   Ptr to OXP_PASSWORD_DATA
    w = Ott->Header.TotalLength - Ott->Header.HeaderLength;	//   Length of OXP_PASSWORD_DATA in bytes
    if (w != 0) {						//   If OXP_PASSWORD_DATA supplied
      DEBUG ((EFI_D_ERROR, "      Delete OXP_PASSWORD_DATA (size 0x%04x bytes) from OOB Output Data\n", w));
      ZeroMem (p, w);						//     Clear OXP_PASSWORD_DATA
      s |= OXP_STATE_DATA_DELETED_OUTPUT_BIT;			//     OXP_PASSWORD_DATA deleted from output data
    }

    // Update different signature, total length in OXP_HEADER
    Ott->Header.Signature = OXP_SIGNATURE_OUTPUT;		// Signature
    Ott->Header.TotalLength = Ott->Header.HeaderLength;		// Total Length in bytes (just OXP_HEADER)
  }

  // Update different state, checksum in OXP_HEADER
  Ott->Header.State = s;					// Update state in OOB data
  Ott->Header.Checksum = 0;					// Init Checksum with 0
  Ott->Header.Checksum = CalculateCheckSum8 (			// Update Checksum
		(UINT8*)(UINTN) Ott, (UINTN) Ott->Header.TotalLength);

  // Return necessary information
  *State = s;							// Updated State
  *Size = Ott->Header.TotalLength;				// OOB Output data size in bytes
  DEBUG ((EFI_D_ERROR, "[OOB] GenerateOobOneTouchOutputData()  } ->  State 0x%08x, OOB Output Data Size 0x%04x bytes\n", s, *Size));
}

//---------------------	OOB Data Read, Write, Integrity, Initialization Functions } -------------------

//---------------------	Password Authentication Functions { -------------------------------------------
//-----------------------------------------------------------------------------------------------------
/**
  MatchOobPasswordAscii(): Check if given ASCII password matches with BIOS password.

  @param   Ott		INPUT: Ptr to OOB_TXT_TPM data
  @param   Opd		INPUT: Ptr to OT_PLATFORM_DATA
  @param   State	INPUT/OUTPUT: UINT32*, Ptr to state

  @retval  Status	OUTPUT: UINT8, Status code

**/
UINT8
MatchOobPasswordAscii (
  IN OOB_TXT_TPM	*Ott,
  IN OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32		*State
  )
{
  // Local data
  UINT32	i, n, s;
  CHAR16	c[PASSWORD_MAX_SIZE+1];
  UINT8		b, e, *p;

  DEBUG ((EFI_D_ERROR, "[OOB] MatchOobPasswordAscii()  { ->  Match ASCII Password\n"));

  // Init local data
  s = 0;							// State
  e = OXP_AUTHENTICATION_ERROR;					// Assume error
  p = (UINT8*)(UINTN) &Ott->PasswordData.Password;		// Ptr to Password information array
  n = (UINT32)(UINTN) (((UINTN) Ott->PasswordData.Length) -	// Length of suplied password information array in bytes
		       ((UINTN) p - (UINTN) &Ott->PasswordData));

  // Ensure internal data area of enough space to accommodate Unicode string generated from ASCII string
  DEBUG ((EFI_D_ERROR, "                Size of Internal Array to generate Unicode String: 0x%08x bytes\n", sizeof (c)));
  DEBUG ((EFI_D_ERROR, "      Size of Password ASCII String (including NULL terminator): 0x%08x bytes\n", n));
  if (n > (sizeof (c) / sizeof (CHAR16))) {			// If password information array size > internal space size
    *State |= OXP_STATE_PASSWORD_TOO_LARGE_BIT;			//   Update State
    return e;							//   Status code
  }

  // Convert ASCII string to Unicode string
  ZeroMem ((VOID*) &c[0], sizeof (c));				// Init unicode string area with 0
  b = 0xFF;							// Assume NULL terminator not found
  for (i = 0; i < n; i++) {					// Search for NULL terminator
    b = p[i];							//   Get next ASCII character
    c[i] = (UINT16)(UINTN) b;					//   Store as Unicode character
    if (b == 0) { break; }					//   NULL found, Stop
  }

  // b = 00, NULL terminator found, i = 1-based string length (in characters) excluding NULL terminator
  //   <>00, NULL terminator not found
  if (b != 0) {							// If NULL terminator not found
    DEBUG ((EFI_D_ERROR, "                Supplied Password string not NULL terminated\n"));
    s = OXP_STATE_PASSWORD_NOT_NULL_TERMINATED_BIT;		//   State
  } else {							// If NULL terminator found
    if (i == 0) {						//   If Password empty
      DEBUG ((EFI_D_ERROR, "                Supplied Password string empty\n"));
      s = OXP_STATE_PASSWORD_EMPTY_BIT;				//     State
    }
  }

  // Match password: s = state, i = 1-based string length (in characters) excluding NULL terminator
  //	p = ptr to given ASCII string, c = Ptr to converted Unicode string
  if (s == 0) {							// If success so far
    // Display ASCII string and Display Unicode string
    DEBUG ((EFI_D_ERROR, "                Input ASCIIZ String, Length 0x%08x bytes (including NULL terminator):\n", (i+1)));
    DisplayBinaryData (p, (i+1));				//   Display ASCIIZ String
    DEBUG ((EFI_D_ERROR, "                Converted UnicodeZ String, Length 0x%08x bytes (including NULL terminator):\n", ((i+1)*2)));
    DisplayBinaryData ((UINT8*)(UINTN) &c[0], sizeof (c));	//   Display UnicodeZ String

    // Match given password with BIOS Administrative password
// APTIOV_SERVER_OVERRIDE_RC_START: To use AMI's Setup password
    if (OT_MatchAdminPassword (&Opd->AmiTseData, &c[0])) {
// APTIOV_SERVER_OVERRIDE_RC_END: To use AMI's Setup password
      e = OXP_SUCCESS;						//     Success
    }
  }

  *State |= s;							// Update State

  DEBUG ((EFI_D_ERROR, "[OOB] MatchOobPasswordAscii()  } -> "));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status code
}

//-----------------------------------------------------------------------------------------------------
/**
  MatchOobPasswordUnicode(): Check if given Unicode password matches with BIOS password.

  @param   Ott		INPUT: Ptr to OOB_TXT_TPM data
  @param   Opd		INPUT: Ptr to OT_PLATFORM_DATA
  @param   State	INPUT/OUTPUT: UINT32*, Ptr to state

  @retval  Status	OUTPUT: UINT8, Status code

**/
UINT8
MatchOobPasswordUnicode (
  IN OOB_TXT_TPM	*Ott,
  IN OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32		*State
  )
{
  // Local data
  UINT32	i, n, s;
  CHAR16	w, *p, c[PASSWORD_MAX_SIZE+1];
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] MatchOobPasswordUnicode():  { -> Match Unicode Password\n"));

  // Init local data
  s = 0;							// State
  e = OXP_AUTHENTICATION_ERROR;					// Assume error
  p = (CHAR16*)(UINTN) &Ott->PasswordData.Password;		// Ptr to Password information array
  n = (UINT32)(UINTN) (((UINTN) Ott->PasswordData.Length) -	// Length of suplied password information array in bytes
		       ((UINTN) p - (UINTN) &Ott->PasswordData));

  // Ensure internal data area of enough space to accommodate Unicode string
  DEBUG ((EFI_D_ERROR, "                Size of Internal Array for Unicode String: 0x%08x bytes\n", sizeof (c)));
  DEBUG ((EFI_D_ERROR, "      Size of Password Unicode String (including NULL terminator): 0x%08x bytes\n", n));
  if (n > sizeof (c)) {						// If password information array size > internal space size
    *State |= OXP_STATE_PASSWORD_TOO_LARGE_BIT;			//   Update State
    return e;							//   Status code
  }

  // Copy given password to internal area: p = Ptr to Password information array
  //	n = Length of password information array in bytes
  ZeroMem ((VOID*) &c[0], sizeof (c));				// Init unicode string area with 0
  n >>= 1;							// Length of password information array in characters
  w = 0xFFFF;							// Assume NULL terminator not found
  for (i = 0; i < n; i++) {					// Search for NULL terminator
    w = p[i]; c[i] = w;						// Copy next char to internal area
    if (w == 0x0000) { break; }					// NULL found, Stop
  }

  // w = 00, NULL terminator found, i = 1-based string length (in characters) excluding NULL terminator
  //   <>00, NULL terminator not found
  if (w != 0) {							// If NULL terminator not found
    DEBUG ((EFI_D_ERROR, "                Supplied Password string not NULL terminated\n"));
    s = OXP_STATE_PASSWORD_NOT_NULL_TERMINATED_BIT;		//   State
  } else {							// If NULL terminator found
    if (i == 0) {						//   If Password empty
      DEBUG ((EFI_D_ERROR, "                Supplied Password string empty\n"));
      s = OXP_STATE_PASSWORD_EMPTY_BIT;				//     State
    }
  }

  // Match password: s = state, i = string length (in characters) without NULL terminator
  //	c = ptr to internal Unicode string
  if (s == 0) {							// If success so far
    // Display Unicode string
    DEBUG ((EFI_D_ERROR, "                Input UnicodeZ String, Length 0x%08x bytes (including NULL terminator):\n", ((i+1) * 2)));
    DisplayBinaryData ((UINT8*)(UINTN) &c[0], sizeof (c));	//   Display UnicodeZ String
    
   
    // Match given password with BIOS Administrative password
    // APTIOV_SERVER_OVERRIDE_RC_START: To use AMI's Setup password
    if (OT_MatchAdminPassword (&Opd->AmiTseData, &c[0])) {
    // APTIOV_SERVER_OVERRIDE_RC_END: To use AMI's Setup password
      e = OXP_SUCCESS;						//     Success
    }
  }

  *State |= s;							// Update State

  DEBUG ((EFI_D_ERROR, "[OOB] MatchOobPasswordUnicode():  } -> "));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status code
}

//-----------------------------------------------------------------------------------------------------
/**
  ValidateOobPasswordInfo(): Validate supplied password related information.

  @param[in]  Ott	INPUT: OOB_TXT_TPM*, Ptr to OOB_TXT_TPM data
  @param[in]  State	INPUT/OUTPUT: UINT32*, Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code
				OXP_SUCCESS	Success
				Other		Error
**/
UINT8
ValidateOobPasswordInfo (
  IN     OOB_TXT_TPM		*Ott,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT32	s;
  UINT16	w;
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] ValidateOobPasswordInfo()  { -> Validate Password related information\n"));

  s = 0; e = OXP_SUCCESS;					// Init local data

  // Validate password data length
  w = Ott->Header.TotalLength - Ott->Header.HeaderLength;	// Length of OXP_PASSWORD_DATA as calculated from header info
  if ((w == 0) || (w != Ott->PasswordData.Length)) {		// If password data length not valid
    s |= OXP_STATE_PASSWORD_DATA_LENGTH_INVALID_BIT;		//   Indicate password data length not valid
    DEBUG ((EFI_D_ERROR, "      Incorrect Password Data Length 0x%04x\n", w));
  }

  // Validate password data identifier
  if (Ott->PasswordData.Id != OXP_ID_PASSWORD) {		// If password data identifier not valid
    s |= OXP_STATE_PASSWORD_DATA_ID_INVALID_BIT;		//   Indicate password data identifier not valid
    DEBUG ((EFI_D_ERROR, "      Incorrect Password Data Identifier 0x%04x\n", Ott->PasswordData.Id));
  }

  // Check password data valid
  if ((Ott->PasswordData.Attribute & OXP_PASSWORD_DATA_VALID_BIT) == 0) { // If password data not valid
    s |= OXP_STATE_PASSWORD_DATA_VALID_NOT_SET_BIT;		//   Indicate password data not valid
    DEBUG ((EFI_D_ERROR, "      Supplied Password Data Not Valid 0x%04x\n", Ott->PasswordData.Attribute));
  }

  // Check if BIOS administrative password installed
  if ((Ott->Header.PasswordAttribute & OXP_PASSWORD_ADMIN_INSTALLED_BIT) == 0) { // If BIOS Administrative password not installed
    s |= OXP_STATE_PASSWORD_BIOS_ADMIN_NOT_INSTALLED_BIT;	//   Indicate BIOS Administrative password not installed
    DEBUG ((EFI_D_ERROR, "      BIOS Administrative Password Not Installed\n"));
  }

  if (s != 0) { e = OXP_AUTHENTICATION_ERROR; }			// Update Status code if error
  *State |= s;							// Return updated state
  DEBUG ((EFI_D_ERROR, "      Updated State 0x%08x, Status 0x%02x\n", *State, e));

  DEBUG ((EFI_D_ERROR, "[OOB] ValidateOobPasswordInfo()  } -> "));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status code
}

//-----------------------------------------------------------------------------------------------------
/**
  IfCheckOobPassword(): Determine whether to check/ignore password during authentication.

  @param[in]  Ott	INPUT: OOB_TXT_TPM*, Ptr to OOB_TXT_TPM data
  @param[in]  State	INPUT/OUTPUT: UINT32*, Ptr to state of task

  @retval     Result	OUTPUT: BOOLEAN	TRUE/FALSE -> Check/Ignore password during authentication

**/
BOOLEAN
IfCheckOobPassword (
  IN     OOB_TXT_TPM	*Ott,
  IN OUT UINT32		*State
  )
{
  // Local data
  UINT32	s;
  UINT16	w;
  BOOLEAN	b;

  DEBUG ((EFI_D_ERROR, "[OOB] IfCheckOobPassword()  { -> Determine whether to check/ignore password\n"));

//  Algorithm:
//	Step-0: If the caller asks to ignore the password, return FALSE.
//	Step-1: If the caller asks to check the password, the following truth-table is used to
//		determine whether to check/ignore password during authentication
//              |---------------|---------------|------------------------|
//              | BIOS          | Caller supply | Action                 |
//              | Administrative| Password in   |                        |
//              | Password      | OOB data      |                        |
//              |---------------|---------------|------------------------|
//        Case-0| Not Installed |       No      | FALSE: Ignore Password |
//        Case-1| Not Installed |       Yes     | FALSE: Ignore Password |
//        Case-2| Installed     |       No      | FALSE: Ignore Password |
//        Case-3| Installed     |       Yes     | TRUE : Check Password  |
//              |---------------|---------------|------------------------|
//		Summary of Truth-Table: If BIOS Administrative Password is installed AND the caller
//		has supplied the password in OOB data, return TRUE else return FALSE.

  s = OXP_STATE_PASSWORD_IGNORED_BIT; b = FALSE;			// State, Ignore password, Supplied Password Attribute
  w = Ott->Header.PasswordAttribute;					// Supplied Password Attribute

  if ((w & OXP_PASSWORD_IGNORE_BIT) == 0) {				// If caller asks to check password
    if (((w & OXP_PASSWORD_ADMIN_INSTALLED_BIT) != 0) &&		//   If BIOS Administrative password installed AND
	((Ott->Header.TotalLength - Ott->Header.HeaderLength) != 0)) {	//      Caller has supplied password
      s = 0; b = TRUE;							//     TRUE: Check Password
    }
  }

  *State |= s;								// Return updated state
  DEBUG ((EFI_D_ERROR, "      Updated State 0x%08x\n", *State));

  DEBUG ((EFI_D_ERROR, "[OOB] IfCheckOobPassword()  } -> "));
  if (b) { DEBUG ((EFI_D_ERROR, "Check password\n")); }
  else { DEBUG ((EFI_D_ERROR, "Ignore password\n")); }

  return b;								// TRUE/FALSE: Check/Ignore Password
}

//-----------------------------------------------------------------------------------------------------
/**
  AuthenticateOobPassword(): Authenticate supplied password data, if necessary.

  @param[in]  Ott	INPUT: OOB_TXT_TPM*, Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT: OT_PLATFORM_DATA*, Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: UINT32*, Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code
				OXP_SUCCESS	EITHER Authentication success OR Authentication not required
				Other		Authentication error
**/
UINT8
AuthenticateOobPassword (
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT32	s;
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] AuthenticateOobPassword()  { -> Authenticate OOB Password\n"));

  s = 0; e = OXP_SUCCESS;						// Init local data

  // Determine whether to check/ignore password during authentication
  if (IfCheckOobPassword (Ott, &s)) {					// If password to be checked
    e = ValidateOobPasswordInfo (Ott, &s);				//   Validate password related info in OOB data

  // Match Password: s = State, e = status
  if (s == 0) {							// If success so far
      e = OXP_PASSWORD_TYPE_NOT_SUPPORTED;				//     Assume Error
    switch (Ott->PasswordData.Attribute & OXP_PASSWORD_TYPE_BIT_MASK) {
      case OXP_PASSWORD_ASCII_BIT:				// ASCII Password
        if ((Opd->PasswordAttribute & OXP_PASSWORD_ASCII_BIT) != 0) {	// If password string type supported
	  e = MatchOobPasswordAscii (Ott, Opd, &s);		// Match password
	} else {						// If password string type not supported
	  s |= OXP_STATE_PASSWORD_STRING_TYPE_NOT_SUPPORTED_BIT;//   Not supported
	}
	break;

      case OXP_PASSWORD_UNICODE_BIT:				// Unicode Password
        if ((Opd->PasswordAttribute & OXP_PASSWORD_UNICODE_BIT) != 0) {	// If password string type supported
	  e = MatchOobPasswordUnicode (Ott, Opd, &s);		// Match password
	} else {						// If password string type not supported
	  s |= OXP_STATE_PASSWORD_STRING_TYPE_NOT_SUPPORTED_BIT;//   Not supported
	}
	break;

      default:							// Invalid password type
	//e = OXP_PASSWORD_TYPE_NOT_SUPPORTED;			// Status code
	s |= OXP_STATE_PASSWORD_SHA_TYPE_NOT_SUPPORTED_BIT;	// Not supported
	break;
    }
  }
  }

  *State |= s;							// Return updated state
  DEBUG ((EFI_D_ERROR, "          Updated State 0x%08x, Status 0x%02x\n", *State, e));

  DEBUG ((EFI_D_ERROR, "[OOB] AuthenticateOobPassword()  } -> "));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status code
}

//---------------------	Password Authentication Functions } -------------------------------------------

//---------------------	SubFunctions for different sub-operations { -----------------------------------
//-----------------------------------------------------------------------------------------------------
/**
  CheckEnableFTpm(): Enable fTPM.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
CheckEnableFTpm (
  IN     TPM2B_AUTH		*Auth,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] CheckEnableFTpm()  { -> Check and Enable fTPM\n"));

  e = OXP_SUCCESS;						// Assume success
  if ((Opd->FeatureSupported & OXP_FTPM_SUPPORT_BIT) != 0) {	// If supported
    if ((Opd->FeatureEnabled & OXP_DTPM_ENABLED_BIT) != 0) {	//   If dTPM enabled currently
      DEBUG ((EFI_D_ERROR, "      OT_DTpmDisable(): dTPM enabled currently, disable dTPM\n"));
      e = OT_DTpmDisable (Auth, Opd);				//     Disable dTPM
    }
    if (e == OXP_SUCCESS) {					//   If successs so far
      if ((Opd->FeatureEnabled & OXP_FTPM_ENABLED_BIT) == 0) {	//     If fTPM not enabled currently
	e = OT_FTpmEnable (Auth, Opd);				//       Enable fTPM
	DEBUG ((EFI_D_ERROR, "      Some fTPM Task may be pending until next reboot\n"));
	*State |= OXP_STATE_TASK_PENDING_BIT;			//     Indicate reset required, Task Pending
      }
    }
    *State |= OXP_STATE_RESET_BIT;				//   Indicate reset required
  } else {							// If not supported
    e = OXP_FTPM_NOT_SUPPORTED;					// Assume Error
  }

  DEBUG ((EFI_D_ERROR, "[OOB] CheckEnableFTpm()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status
}

//-----------------------------------------------------------------------------------------------------
/**
  CheckEnableDTpm(): Enable dTPM.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
CheckEnableDTpm (
  IN     TPM2B_AUTH		*Auth,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] CheckEnableDTpm()  { -> Check and Enable dTPM\n"));

  e = OXP_DTPM_NOT_SUPPORTED;					// Assume Error
  if ((Opd->FeatureSupported & OXP_DTPM_SUPPORT_BIT) != 0) {	// If supported
    if ((Opd->FeatureEnabled & OXP_FTPM_ENABLED_BIT) != 0) {	//   If fTPM enabled currently
      DEBUG ((EFI_D_ERROR, "      OT_FTpmDisable(): fTPM enabled currently, disable fTPM\n"));
      if ((e = OT_FTpmDisable (Auth, Opd)) == OXP_SUCCESS) {	//   Disable fTPM, If success
	DEBUG ((EFI_D_ERROR, "      Enable dTPM: Task pending until next reboot\n"));
	*State |= OXP_STATE_TASK_PENDING_BIT;			//   Indicate reset required, Task Pending
      }
    } else {							// If fTPM not enabled currently
      e = OT_DTpmEnable (Auth, Opd);				//   Enable dTPM
    }
    *State |= OXP_STATE_RESET_BIT;					//   Indicate reset required
  }

  DEBUG ((EFI_D_ERROR, "[OOB] CheckEnableDTpm()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;								// Status
}

//-----------------------------------------------------------------------------------------------------
/**
  CheckEnableTxt(): Enable TXT.

  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
CheckEnableTxt (
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] CheckEnableTxt()  { -> Check and Enable TXT\n"));

  e = OXP_TXT_NOT_SUPPORTED;					// Assume Error
  if ((Opd->FeatureSupported & OXP_TXT_SUPPORT_BIT) != 0) {	// If supported
    e = OT_TxtEnable (Opd);					// Enable TXT
    *State |= OXP_STATE_RESET_BIT;				//   Indicate reset required
  }

  DEBUG ((EFI_D_ERROR, "[OOB] CheckEnableTxt()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status
}

//---------------------	SubFunctions for different sub-operations } -----------------------------------


//---------------------	Main Functions for different operations { -------------------------------------
//-----------------------------------------------------------------------------------------------------
/**
  OobDTpmClear(): Perform the requested task: Clear dTPM.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Ott	INPUT:	Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
OobDTpmClear (
  IN     TPM2B_AUTH		*Auth,
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e = OXP_SUCCESS;				// Assume success

  DEBUG ((EFI_D_ERROR, "[OOB] OobDTpmClear()  { -> Clear dTPM\n"));

  if ((Opd->FeatureSupported & OXP_DTPM_SUPPORT_BIT) == 0) {	// If not supported
    e = OXP_DTPM_NOT_SUPPORTED;					//   Error
  } else {							// If supported
  if ((Opd->FeatureEnabled & OXP_DTPM_ENABLED_BIT) == 0) {	// If not enabled
    e = OXP_DTPM_NOT_ENABLED;					//   Error
  }
  }

  if (e == OXP_SUCCESS) {					// If success so far
    e = OT_DTpmClear (Auth, Opd);				// Clear TPM
    *State |= OXP_STATE_RESET_BIT;				// Indicate reset required
  }

  DEBUG ((EFI_D_ERROR, "[OOB] OobDTpmClear()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status
}

//-----------------------------------------------------------------------------------------------------
/**
  OobDTpmDisableOnly(): Perform the requested task: Disable dTPM Only.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Ott	INPUT:	Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
OobDTpmDisableOnly (
  IN     TPM2B_AUTH		*Auth,
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] OobDTpmDisableOnly()  { -> Disable dTPM Only\n"));

  e = OXP_DTPM_NOT_SUPPORTED;					// Assume Error
  if ((Opd->FeatureSupported & OXP_DTPM_SUPPORT_BIT) != 0) {	// If supported
    e = OXP_SUCCESS;						//   Assume Success
    if ((Opd->FeatureEnabled & OXP_DTPM_ENABLED_BIT) != 0) {	//   If dTPM enabled currently
      e = OT_DTpmDisable (Auth, Opd);				//     Disable dTPM
      *State |= OXP_STATE_RESET_BIT;				//     Indicate reset required
    }
  }

  DEBUG ((EFI_D_ERROR, "[OOB] OobDTpmDisableOnly()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status
}

//-----------------------------------------------------------------------------------------------------
/**
  OobDTpmEnableTxtEnable(): Perform the requested task: Enable dTPM, Enable TXT.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Ott	INPUT:	Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
OobDTpmEnableTxtEnable (
  IN     TPM2B_AUTH		*Auth,
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] OobDTpmEnableTxtEnable()  { -> Enable dTPM, Enable TXT\n"));

  e = CheckEnableDTpm (Auth, Opd, State);		// Enable TPM
  if (e == OXP_SUCCESS) {				// If success
    if ((*State & OXP_STATE_TASK_PENDING_BIT) == 0) {	//   If Task not pending
      e = CheckEnableTxt (Opd, State);			//     Enable TXT
    }
  }

  DEBUG ((EFI_D_ERROR, "[OOB] OobDTpmEnableTxtEnable()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;						// Status
}

//-----------------------------------------------------------------------------------------------------
/**
  OobDTpmEnableClearTxtEnable(): Perform the requested task: Enable dTPM, Clear dTPM, and Enable TXT.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Ott	INPUT:	Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
OobDTpmEnableClearTxtEnable (
  IN     TPM2B_AUTH		*Auth,
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] OobDTpmEnableClearTxtEnable()  { -> Enable dTPM, Clear dTPM, Enable TXT\n"));

  e = CheckEnableDTpm (Auth, Opd, State);			// Enable dTPM
  if ((e == OXP_SUCCESS) && (Opd->Tpm12ActivateStateChange)) {	// If Success AND TPM Activate State Changed
    *State |= OXP_STATE_TASK_PENDING_BIT;			//   Indicate Task pending
  }

  if (e == OXP_SUCCESS) {					// If success
    if ((*State & OXP_STATE_TASK_PENDING_BIT) == 0) {		//   If Task not pending
      e = OobDTpmClear (Auth, Ott, Opd, State);			//     Clear dTPM
      if (e == OXP_SUCCESS) {					//     If success
//	if ((*State & OXP_STATE_TASK_PENDING_BIT) == 0) {	//      If Task not pending
	  e = CheckEnableTxt (Opd, State);			//         Enable TXT
//	}
      }
    }
  }

  DEBUG ((EFI_D_ERROR, "[OOB] OobDTpmEnableClearTxtEnable()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status
}

//-----------------------------------------------------------------------------------------------------
/**
  OobDTpmEnableClear(): Perform the requested task: Enable dTPM, Clear dTPM.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Ott	INPUT:	Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
OobDTpmEnableClear (
  IN     TPM2B_AUTH		*Auth,
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] OobDTpmEnableClear()  { -> Enable dTPM, Clear dTPM\n"));

  e = CheckEnableDTpm (Auth, Opd, State);			// Enable dTPM
  if ((e == OXP_SUCCESS) && (Opd->Tpm12ActivateStateChange)) {	// If Success AND TPM Activate State Changed
    *State |= OXP_STATE_TASK_PENDING_BIT;			//   Indicate Task pending
  }

  if (e == OXP_SUCCESS) {					// If success
    if ((*State & OXP_STATE_TASK_PENDING_BIT) == 0) {		//   If Task not pending
      e = OobDTpmClear (Auth, Ott, Opd, State);			//     Clear dTPM
    }
  }

  DEBUG ((EFI_D_ERROR, "[OOB] OobDTpmEnableClear()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status
}

//-----------------------------------------------------------------------------------------------------
/**
  OobFTpmClear(): Perform the requested task: Clear fTPM.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Ott	INPUT:	Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
OobFTpmClear (
  IN     TPM2B_AUTH		*Auth,
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e = OXP_SUCCESS;				// Assume success

  DEBUG ((EFI_D_ERROR, "[OOB] OobFTpmClear()  { -> Clear fTPM\n"));

  if ((Opd->FeatureSupported & OXP_FTPM_SUPPORT_BIT) == 0) {	// If not supported
    e = OXP_FTPM_NOT_SUPPORTED;					//   Error
  } else {							// If supported 
  if ((Opd->FeatureEnabled & OXP_FTPM_ENABLED_BIT) == 0) {	// If not enabled
    e = OXP_FTPM_NOT_ENABLED;					//   Error
  }
  }

  if (e == OXP_SUCCESS) {					// If success so far
    e = OT_FTpmClear (Auth, Opd);				// Clear TPM
    *State |= OXP_STATE_RESET_BIT;				// Indicate reset required
  }

  DEBUG ((EFI_D_ERROR, "[OOB] OobFTpmClear()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status
}

//-----------------------------------------------------------------------------------------------------
/**
  OobFTpmDisableOnly(): Perform the requested task: Disable fTPM Only.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Ott	INPUT:	Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
OobFTpmDisableOnly (
  IN     TPM2B_AUTH		*Auth,
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] OobFTpmDisableOnly()  { -> Disable fTPM Only\n"));

  e = OXP_FTPM_NOT_SUPPORTED;					// Assume Error
  if ((Opd->FeatureSupported & OXP_FTPM_SUPPORT_BIT) != 0) {	// If supported
    e = OXP_SUCCESS;						//   Assume Success
    if ((Opd->FeatureEnabled & OXP_FTPM_ENABLED_BIT) != 0) {	//   If fTPM enabled currently
      e = OT_FTpmDisable (Auth, Opd);				//     Disable fTPM
      *State |= OXP_STATE_RESET_BIT;				//     Indicate reset required
    }
  }

  DEBUG ((EFI_D_ERROR, "[OOB] OobFTpmDisableOnly()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status
}

//-----------------------------------------------------------------------------------------------------
/**
  OobFTpmEnableTxtEnable(): Perform the requested task: Enable fTPM, Enable TXT.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Ott	INPUT:	Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
OobFTpmEnableTxtEnable (
  IN     TPM2B_AUTH		*Auth,
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] OobFTpmEnableTxtEnable()  { -> Enable fTPM, Enable TXT\n"));

  e = CheckEnableFTpm (Auth, Opd, State);					// Enable TPM
  if ((e == OXP_SUCCESS) && ((*State & OXP_STATE_TASK_PENDING_BIT) == 0)) {	// If success AND Task not pending
    e = CheckEnableTxt (Opd, State);						//   Enable TXT
  }

  DEBUG ((EFI_D_ERROR, "[OOB] OobFTpmEnableTxtEnable()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;									// Status
}

//-----------------------------------------------------------------------------------------------------
/**
  OobFTpmEnableClearTxtEnable(): Perform the requested task: Enable fTPM, Clear fTPM, and Enable TXT.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Ott	INPUT:	Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
OobFTpmEnableClearTxtEnable (
  IN     TPM2B_AUTH		*Auth,
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] OobFTpmEnableClearTxtEnable()  { -> Enable fTPM, Clear fTPM, Enable TXT\n"));

  e = CheckEnableFTpm (Auth, Opd, State);					// Enable fTPM
  if ((e == OXP_SUCCESS) && ((*State & OXP_STATE_TASK_PENDING_BIT) == 0)) {	// If success AND Task not pending
    e = OobFTpmClear (Auth, Ott, Opd, State);					//   Clear fTPM
    if ((e == OXP_SUCCESS) && ((*State & OXP_STATE_TASK_PENDING_BIT) == 0)) {	//   If success AND Task not pending
      e = CheckEnableTxt (Opd, State);						//     Enable TXT
    }
  }

  DEBUG ((EFI_D_ERROR, "[OOB] OobFTpmEnableClearTxtEnable()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;									// Status
}

//-----------------------------------------------------------------------------------------------------
/**
  OobFTpmEnableClear(): Perform the requested task: Enable fTPM, Clear fTPM.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Ott	INPUT:	Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
OobFTpmEnableClear (
  IN     TPM2B_AUTH		*Auth,
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] OobFTpmEnableClear()  { -> Enable fTPM, Clear fTPM\n"));

  e = CheckEnableFTpm (Auth, Opd, State);					// Enable fTPM
  if ((e == OXP_SUCCESS) && ((*State & OXP_STATE_TASK_PENDING_BIT) == 0)) {	// If success AND Task not pending
    e = OobFTpmClear (Auth, Ott, Opd, State);					//   Clear fTPM
  }

  DEBUG ((EFI_D_ERROR, "[OOB] OobFTpmEnableClear()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;									// Status
}

//-----------------------------------------------------------------------------------------------------
/**
  OobTxtDisableOnly(): Perform the requested task: Disable TXT Only.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Ott	INPUT:	Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: Ptr to state of task

  @retval     Result	OUTPUT: UINT8, Status code for performed task

**/
UINT8
OobTxtDisableOnly (
  IN     TPM2B_AUTH		*Auth,
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB] OobTxtDisableOnly()  { -> Disable TXT Only\n"));

  e = OXP_TXT_NOT_SUPPORTED;					// Assume Error
  if ((Opd->FeatureSupported & OXP_TXT_SUPPORT_BIT) != 0) {	// If supported
    e = OT_TxtDisable (Opd);					// Enable TXT
    *State |= OXP_STATE_RESET_BIT;				//   Indicate reset required
  }

  DEBUG ((EFI_D_ERROR, "[OOB] OobTxtDisableOnly()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status
}


//---------------------	Main Functions for different operations } -------------------------------------

//-----------------------------------------------------------------------------------------------------
/**
  PerformOobOneTouchTask(): Perform the requested OOB One-Touch Task.

  @param[in]  Auth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param[in]  Ott	INPUT:	Ptr to OOB_TXT_TPM data
  @param[in]  Opd	INPUT:	Ptr to OT_PLATFORM_DATA
  @param[in]  State	INPUT/OUTPUT: UINT32*, Ptr to state of task

  @retval     None

  NOTE: 
     1.	This function should perform the task and update all necessary fields in OOB Data (execept State field).
     2. This function MUST NOT update the State field of OOB Data.
	State field in OOB data is updated before writing updated OOB data, if necessary.
**/
VOID
PerformOobOneTouchTask (
  IN     TPM2B_AUTH		*Auth,
  IN     OOB_TXT_TPM		*Ott,
  IN     OT_PLATFORM_DATA	*Opd,
  IN OUT UINT32			*State
  )
{
  // Local data
  UINT32	s;							// State
  UINT8		e;							// Status

  s = *State;									// Current State
  DEBUG ((EFI_D_ERROR, "[OOB] PerformOobOneTouchTask()  { -> Perform requested OOB Task\n"));
  DEBUG ((EFI_D_ERROR, "          INPUT : TaskAndResult 0x%01x, Status 0x%01x, State 0x%08x\n", Ott->Header.TaskAndResult, Ott->Header.Status, s));

  // Update general info in OXP_HEADER, if necessary
  if ((Ott->Header.PasswordAttribute & (~(OXP_PASSWORD_IGNORE_BIT))) !=	// If Password Attribute changed
					Opd->PasswordAttribute) {
    Ott->Header.PasswordAttribute = (Opd->PasswordAttribute |		//   Update Password Attribute
		(Ott->Header.PasswordAttribute & OXP_PASSWORD_IGNORE_BIT));
    s |= OXP_STATE_PASSWORD_ATTRIBUTE_UPDATED_BIT;				//   Update State
  }
  if (Ott->Header.FeatureSupported != Opd->FeatureSupported) {			// If Feature Supported changed
    Ott->Header.FeatureSupported = Opd->FeatureSupported;			//   Update Feature Supported
    s |= OXP_STATE_FEATURE_SUPPORTED_UPDATED_BIT;				//   Update State
  }
  if (Ott->Header.FeatureEnabled != Opd->FeatureEnabled) {			// If Feature Enabled changed
    Ott->Header.FeatureEnabled = Opd->FeatureEnabled;				//   Update Feature Enabled
    s |= OXP_STATE_FEATURE_ENABLED_UPDATED_BIT;					//   Update State
  }

  if (Ott->Header.MajorVersionImp != OXP_MAJOR_VERSION_IMP) {		// If Implementation Major Version nor match
    Ott->Header.MajorVersionImp = OXP_MAJOR_VERSION_IMP;		//   Update Implementation Major Version
    s |= OXP_STATE_VERISON_IMP_UPDATED_BIT;				//   Update State
  }

  if (Ott->Header.MinorVersionImp != OXP_MINOR_VERSION_IMP) {		// If Implementation Minor Version nor match
    Ott->Header.MinorVersionImp = OXP_MINOR_VERSION_IMP;		//   Update Implementation Minor Version
    s |= OXP_STATE_VERISON_IMP_UPDATED_BIT;				//   Update State
  }

  // Check if any task to be performed: s = State
  if ((Ott->Header.TaskAndResult & OXP_TASK_PERFORMED_BIT) != 0) {		// If no task to be performed
    *State = s;									//   Update State
    DEBUG ((EFI_D_ERROR, "                  No task to perform\n"));
    DEBUG ((EFI_D_ERROR, "          OUTPUT: TaskAndResult 0x%01x, Status 0x%01x, State 0x%08x\n", Ott->Header.TaskAndResult, Ott->Header.Status, s));
    DEBUG ((EFI_D_ERROR, "[OOB] PerformOobOneTouchTask()  }\n"));
    return;
  }

  // Authenticate password data, if necessary
  e = OXP_SUCCESS;							// Assume success
  if (AuthenticationOobTaskRequired (Ott)) {				// If authentication required
  e = AuthenticateOobPassword (Ott, Opd, &s);					// Authenticate password
  }

  // Prepare TPM, if necessary
  if (e == OXP_SUCCESS) {						// If success so far
    if (TpmPreparationOobTaskRequired (Ott)) {				// If TPM preparation required
      e = OT_PrepareTpmBeforeOobTask (Opd);				//   Prepare TPM for OOB Task
      if (e != OXP_SUCCESS) {						// If not success
	s |= OXP_STATE_TPM_PREPARATION_BEFORE_TASK_ERROR_BIT;		//   Update state
      }
    }
  }

  // A task to be performed: s = state, e = status
  if (e == OXP_SUCCESS) {							// If success so far
    switch ((Ott->Header.TaskAndResult & OXP_TASK_BIT_MASK) >> OXP_TASK_BIT_N) {
      case OXP_TASK_TXT_TPM_SUPPORT_DISCOVERY:					//   01 TXT/TPM Support Discovery
      case OXP_TASK_TXT_TPM_ENABLE_STATUS_DISCOVERY:				//   02 TXT/TPM Enable Status Discovery
      case OXP_TASK_TPM_PRESENCE_STATUS_DISCOVERY:			//   07 TPM Presence Status Discovery
	//e = OXP_SUCCESS;	// e = OXP_SUCCESS already			//      Success
	break;

      case OXP_TASK_ENABLE_TXT_DTPM:					//   03 Enable TXT/dTPM
	e = OobDTpmEnableTxtEnable (Auth, Ott, Opd, &s);		//      Enable dTPM, Enable TXT
	break;

      case OXP_TASK_DTPM_OWNER_CLEAR_ONLY:				//   04 dTPM Owner Clear Only
	e = OobDTpmClear (Auth, Ott, Opd, &s);				//      Clear dTPM
	break;

      case OXP_TASK_DTPM_CLEAR_DTPM_ACTIVATION:				//   05 dTPM Clear and dTPM Activation
	e = OobDTpmEnableClear (Auth, Ott, Opd, &s);			//      Enable dTPM, Clear dTPM
	break;

      case OXP_TASK_DTPM_CLEAR_ENABLE_TXT_DTPM:				//   06 dTPM Clear and Enable TXT/dTPM
	e = OobDTpmEnableClearTxtEnable (Auth, Ott, Opd, &s);		//      Enable dTPM, Clear dTPM, Enable TXT
	break;

      case OXP_TASK_ENABLE_TXT_FTPM:					//   0B Enable TXT/fTPM
	e = OobFTpmEnableTxtEnable (Auth, Ott, Opd, &s);		//      Enable fTPM, Enable TXT
	break;

      case OXP_TASK_FTPM_OWNER_CLEAR_ONLY:				//   0C fTPM Owner Clear Only
	e = OobFTpmClear (Auth, Ott, Opd, &s);				//      Clear fTPM
      break;

      case OXP_TASK_FTPM_CLEAR_FTPM_ACTIVATION:				//   0D fTPM Clear and fTPM Activation
	e = OobFTpmEnableClear (Auth, Ott, Opd, &s);			//      Enable fTPM, Clear fTPM
	break;

      case OXP_TASK_FTPM_CLEAR_ENABLE_TXT_FTPM:				//   0E fTPM Clear and Enable TXT/fTPM
	e = OobFTpmEnableClearTxtEnable (Auth, Ott, Opd, &s);		//      Enable fTPM, Clear fTPM, Enable TXT
	break;

//      case OXP_TASK_TXT_LAUNCH_STATUS_DISCOVERY:			//   08	TXT Launch Status Discovery
//      case OXP_TASK_DTPM_OWNERSHIP_STATUS_DISCOVERY:			//   09	dTPM Ownership Status Discovery
//      case OXP_TASK_FTPM_OWNERSHIP_STATUS_DISCOVERY:			//   0A fTPM Ownership Status Discovery

      case OXP_TASK_DISABLE_TXT_ONLY:					//   0F	Disable TXT Only
	e = OobTxtDisableOnly (Auth, Ott, Opd, &s);			//      Disable TXT
	break;

      case OXP_TASK_DISABLE_DTPM_ONLY:					//   10	Disable dTPM Only
	e = OobDTpmDisableOnly (Auth, Ott, Opd, &s);			//      Disable dTPM
	break;

      case OXP_TASK_DISABLE_FTPM_ONLY:					//   11	Disable fTPM Only
	e = OobFTpmDisableOnly (Auth, Ott, Opd, &s);			//      Disable fTPM
	break;

      default:									// Invalid Task
	e = OXP_INVALID_TASK;							// Status
	break;
    }
  }

  // Prepare TPM, if necessary: s = State, e = status
  if ((OT_PrepareTpmAfterOobTask (Opd)) != OXP_SUCCESS) {		// Prepare TPM after task
    s |= OXP_STATE_TPM_PREPARATION_AFTER_TASK_ERROR_BIT;		//   If not success, Update state
  }

  // s = State, e = status
  if ((s & OXP_STATE_TASK_PENDING_BIT) != 0) {				// If Task pending (i.e. not yet performed)
    Ott->Header.TaskAndResult &= ~(OXP_TASK_PERFORMED_BIT);		//   Indicate task not yet performed in OOB data
  } else {								// If Task not pending (i.e. performed)
    Ott->Header.TaskAndResult |= OXP_TASK_PERFORMED_BIT;		//   Indicate task performed in OOB data
  }
  Ott->Header.Status = e;						// Update Status in OOB data
  *State = s;								// Return State
  DEBUG ((EFI_D_ERROR, "          OUTPUT: TaskAndResult 0x%01x, Status 0x%01x, State 0x%08x\n", Ott->Header.TaskAndResult, Ott->Header.Status, s));
  DEBUG ((EFI_D_ERROR, "[OOB] PerformOobOneTouchTask()  }\n"));

  return;
}

//-----------------------------------------------------------------------------------------------------
/**
  Check and execute the requested OOB One-Touch Request.

  @param  PlatformAuth	INPUT:	Ptr to TPM2B_AUTH, Platform auth value
				NULL means no platform auth change
  @param  State		OUTPUT:	State of performed OOB Task

  NOTE:
     1.	The control will return to the caller in the following cases:
	a) There is no OOB One-Touch request.
	b) There is an OOB One-Touch request which does not require a system reset.
     2.	The control will NOT return to the caller in the following cases:
	a) There is an OOB One-Touch request which requires a system reset.
**/
UINT32
ExecuteOobOneTouchRequest (
  IN  TPM2B_AUTH	*PlatformAuth
  )
{
  // LOcal data
  OOB_TXT_TPM		*Ottm, *Ottp;
  OT_PLATFORM_DATA	*Opd;
  UINTN			d, dm;
  UINT32		s;
  UINT16		f;

  DEBUG ((EFI_D_ERROR, "[OOB] ExecuteOobOneTouchRequest()  {\n"));

//JMP_$();

  // Init local data
  s = 0; f = 0;										// State of different operations, Fatal Error information

  // Check if S4 resume
  if (GetBootModeHob() == BOOT_ON_S4_RESUME) {						// If S4 Resume
    DEBUG ((EFI_D_ERROR, "      S4 Resume: OOB request Not Processed, State 0x%08x\n", s));//   Skip processing OOB request
    DEBUG ((EFI_D_ERROR, "[OOB] ExecuteOobOneTouchRequest()  }\n"));
    return s;										//   Assume success
  }

  // ------------------	Read Platform Information ----------------------------------------
  // Allocate memory for platform information
  Opd = OT_AllocatePlatformInformationArea();						// Allocate memory
  if (Opd == NULL) {									// If Memory not available
    f |= OOB_FATAL_MEMORY_NOT_AVAILABLE_BIT;						//   Update fatal error info
  } else {										// If memory available
    // Read platform information
    if (!(OT_ReadPlatformInformation (Opd))) {						// If Read platform information not success
      f |= OOB_FATAL_PLATFORM_DATA_READ_ERROR_BIT;					//   Update fatal error info
    }
  }

  // ------------------	Validate OOB Data Non-Volatile Storage Support -------------------
  // Validate OOB Non-Volatile Storage support
  d = 0; Ottm = NULL; Ottp = NULL;							// Init Non-volatile Storage Size in bytes, ptr to current/previous OOB data
  DEBUG ((EFI_D_ERROR, "[OOB] Non-Volatile Storage Supported? "));
  if (OobNvStorageSupported()) {							// If non-volatile Storage supported
    DEBUG ((EFI_D_ERROR, "Yes, "));
    if (MeNvStorage) { DEBUG ((EFI_D_ERROR, "Use ME Non-Volatile Storage\n")); }
    else { DEBUG ((EFI_D_ERROR, "Use EFI Variable\n")); }
    d = GetOobNvStorageSize();								//   Find Non-volatile Storage max size in bytes
    DEBUG ((EFI_D_ERROR, "      Maximum size of OOB Non-Volatile Storage: 0x%08x bytes\n", d));
  } else {										// If non-volatile Storage not supported
    DEBUG ((EFI_D_ERROR, "No\n"));
    f |= OOB_FATAL_NV_STORAGE_NOT_SUPPORTED_BIT;					//   Update fatal error info
  }

  // ------------------	Allocate memory for reading OOB data from non-volatile storage ---
  // s = state, f = fatal error info, d = Max size of Non-volatile storage in bytes
  if (d != 0) {										// If Non-volatile Storage size valid
    DEBUG ((EFI_D_ERROR, "          Allocate Memory for Old and New OOB Data (Total 0x%08x bytes): ", 2*d));
    Ottm = ((OOB_TXT_TPM*)(UINTN) AllocateZeroPool (d));				//   Allocate memory for current OOB data
    Ottp = ((OOB_TXT_TPM*)(UINTN) AllocateZeroPool (d));				//   Allocate memory for previous OOB data
    if ((Ottm == NULL) || (Ottp == NULL)) {						//   If Memory not available
      DEBUG ((EFI_D_ERROR, "Memory Not Available -> Error\n"));
      f |= OOB_FATAL_MEMORY_NOT_AVAILABLE_BIT;						//     Update fatal error info
    } else {										//   If Memory available
      DEBUG ((EFI_D_ERROR, "Allocated\n"));
    }
  } else {										// If Non-volatile storage size not valid
    f |= OOB_FATAL_NV_STORAGE_SIZE_ZERO_BIT;						//   Update fatal error info
  }

  // ------------------	Check whether OOB request can be processed -----------------------
  // s = state, f = Fatal Error info (<> 0 -> FATAL Error), d = Max size of Non-volatile storage in bytes
  if (f != 0) {										// If any Fatal Error
    DEBUG ((EFI_D_ERROR, "      OOB request cannot be processed -> State: 0x%08x, Fatal Error: 0x%04x\n", s, f));
    // OOB data not yet read, so no need to clear OOB data area
    DEBUG ((EFI_D_ERROR, "      Deallocate Memory used for OOB Data ->\n"));
    gBS->FreePool ((VOID*)(UINTN) Ottm); gBS->FreePool ((VOID*)(UINTN) Ottp);		// Deallocate memory used for OOB data
    DEBUG ((EFI_D_ERROR, "      Clear and Deallocate Memory used for Platform Information ->\n"));
    OT_ClearAndDeallocatePlatformInformationArea (Opd);					// Clear and Deallocate memory used for Platform Info
    DEBUG ((EFI_D_ERROR, "[OOB] ExecuteOobOneTouchRequest()  }\n"));
    return s;										//   State
  }

  // ------------------	Read OOB Data from Non-Volatile Storage --------------------------
  // Read OOB data from non-volatile storage: s = state, f = fatal error info, d = Max size of Non-volatile storage in bytes
  dm = d;										// Max size of Non-volatile storage in bytes
  if (ReadOobDataFromNvStorage ((VOID*)(UINTN) Ottm, &d)) {				// If OOB Data read success
    CopyMem ((VOID*) Ottp, (VOID*) Ottm, d);						//   Save original OOB Data
  } else {										// If OOB Data read not success
    d = 0; f |= OOB_FATAL_OOB_DATA_READ_ERROR_BIT;					//   Force read data size = 0, Update fatal error info
  }

  // ------------------	Validate OOB Data ------------------------------------------------
  // s = state, f = fatal error (<> 0 -> OOB data read Error)
  // d = OOB data size in bytes as read from NV storage, dm = Max size of NV storage in bytes
  if (d != 0) {										// If OOB data size valid
    ValidateOobOneTouchInputData (Ottm, &d, &s);					//   Validate OOB One-Touch Input Data
  } else {										// If OOB data read error
    InitOobOneTouchData (Ottm, Opd, &d);						//   Initialize OOB Data with default values
    s |= OXP_STATE_OOB_DATA_FRESH_GENERATED_BIT;					//   Update state
    f |= OOB_FATAL_OOB_DATA_READ_SIZE_ZERO_BIT;						//   OOB data size as read from NV storage = 0
  }

  if ((d != 0) && (d <= dm)) {
    DEBUG ((EFI_D_ERROR, "      Input OOB Data: Size = 0x%08x bytes\n", d));
    DisplayBinaryData ((UINT8*)(UINTN) Ottm, d);					// Display OOB Input Data
  }

  // ------------------	Perform OOB task -------------------------------------------------
  // s = state, f = fatal error, d = OOB data size in bytes as read from NV storage, dm = Max size of NV storage in bytes
  DEBUG ((EFI_D_ERROR, "      Updated State: 0x%08x, Fatal Error: 0x%04x\n", s, f));
  if (s == 0) { PerformOobOneTouchTask (PlatformAuth, Ottm, Opd, &s); }			// If success so far, Perform OOB One-Touch Request

  // ------------------	Write Platform Info and write OOB Data ---------------------------
  // s = state, f = fatal error, d = OOB data size in bytes as read from NV storage, dm = Max size of NV storage in bytes
  if (!(OT_WritePlatformInformation (Opd))) {						// If Write platform information not success
    DEBUG ((EFI_D_ERROR, "      Platform Information Write Error\n"));
    s |= OXP_STATE_PLATFORM_DATA_WRITE_ERROR_BIT;					//   Error in writing platform information
  } else {										// If Write platform information success 
    // Write OOB One-Touch Data: s = updated state, d = OOB data size in bytes as read from NV storage, dm = Max size of NV storage in bytes
    if ((d != 0) && (d <= dm)) {
      DEBUG ((EFI_D_ERROR, "      OOB Data after processing (before updating Checksum, State):\n"));
    DisplayBinaryData ((UINT8*)(UINTN) Ottm, d);					//   Display OOB Data
    }
    if ((s != 0) || ((CompareMem ((UINT8*)(UINTN) Ottp, (UINT8*)(UINTN) Ottm, d)) != 0)) {//   If state non-zero OR OOB Data modified
      DEBUG ((EFI_D_ERROR, "      State Non-Zero OR OOB Data modified\n"));
#if defined(SMCPKG_SUPPORT) && (SMCPKG_SUPPORT == 1)
      if (f & OOB_FATAL_OOB_DATA_READ_ERROR_BIT) {
         //
         // Writes OOB Data to NV Storage
         //
         if (!(WriteOobDataToNvStorage (Ottm, &d))) {
           DEBUG ((EFI_D_INFO, "Error, SPS file system is empty, BIOS writes current system configuration to SPS file system unsuccessfully.\n"));
           //
           //  OOB data write error
           //
           f |= OOB_FATAL_OOB_DATA_WRITE_ERROR_BIT;
         } else {
           DEBUG ((EFI_D_INFO, "Success, SPS file system is empty, BIOS writes current system configuration to SPS file system successfully.\n"));
         }
      }
#endif //#if defined(SMCPKG_SUPPORT) && (SMCPKG_SUPPORT == 1)
      if ((f == 0) && ((s & OXP_STATE_SIGNATURE_INVALID_BIT) == 0) && (d != 0)) {	//       If no fatal error AND valid signature present in OOB data AND OOB data size non-zero
	  DEBUG ((EFI_D_ERROR, "      OOB data read success AND valid input signature: Generate OOB Output Data\n"));
	  GenerateOobOneTouchOutputData (Ottm, &d, &s);					//         Prepare OOB One-Touch Output Data
	  DEBUG ((EFI_D_ERROR, "      OOB Output Data Size = 0x%08x bytes\n", d));
	if ((d != 0) && (d <= dm)) {
      DisplayBinaryData ((UINT8*)(UINTN) Ottm, d);					//     Display OOB Data
	    DEBUG ((EFI_D_ERROR, "      Write OOB Output Data:\n"));
	    if (!(WriteOobDataToNvStorage (Ottm, &d))) {				//           Write OOB Data to NV Storage
	      DEBUG ((EFI_D_ERROR, "Error\n"));
	      f |= OOB_FATAL_OOB_DATA_WRITE_ERROR_BIT;					//           OOB data write error
	    } else {
	      DEBUG ((EFI_D_ERROR, "Success\n"));
	    }
          } else {
	    DEBUG ((EFI_D_ERROR, "      Output OOB Data Size EITHER 0 OR > Max Size of Non-Volatile Storage\n"));
        }
      } else {										//       If OOB data read not success OR valid signature not present in OOB data
	DEBUG ((EFI_D_ERROR, "      OOB input data size 0 OR invalid signature: Do not write OOB Data\n"));
      }
    } else {	 									//   If state zero AND OOB Data not modified
      DEBUG ((EFI_D_ERROR, "      State Zero AND OOB Data not modified: No need to write OOB Data\n"));
    }
  }
  DEBUG ((EFI_D_ERROR, "      Updated State: 0x%08x, Fatal Error: 0x%04x\n", s, f));

  // ------------------	Deallocate memory used for Platform Info and OOB Data ------------
  // dm = Max size of NV storage in bytes
  DEBUG ((EFI_D_ERROR, "      Clear Memory used for OOB Data ->\n"));			// Before deallocating, clear data area to prevent the data from being hacked.
  ZeroMem ((UINT8*)(UINTN) Ottm, dm); ZeroMem ((UINT8*)(UINTN) Ottp, dm);
  DEBUG ((EFI_D_ERROR, "      Deallocate Memory used for OOB Data ->\n"));
  gBS->FreePool ((VOID*)(UINTN) Ottm); gBS->FreePool ((VOID*)(UINTN) Ottp);		// Deallocate memory used for OOB data

  DEBUG ((EFI_D_ERROR, "      Clear and Deallocate Memory used for Platform Information ->\n"));
  OT_ClearAndDeallocatePlatformInformationArea (Opd);					// Clear and Deallocate memory used for Platform Info

  // ------------------	Check if reset required: s = updated state -----------------------
  if ((s & OXP_STATE_RESET_BIT) != 0) {							// If Reset required
    DEBUG ((EFI_D_ERROR, "      Reset Required: Issue system reset....\n"));
    DEBUG ((EFI_D_ERROR, "[OOB] ExecuteOobOneTouchRequest()  }\n"));
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);				//   Reset system
    JMP_$();
  }

  DEBUG ((EFI_D_ERROR, "      No task to perform OR Reset not required for the performed task\n"));
  DEBUG ((EFI_D_ERROR, "[OOB] ExecuteOobOneTouchRequest()  }\n"));

  return s;										// State
}

//APTIOV_SERVER_OVERRIDE_RC_START : Hook to Connect Drivers Event
/**
  One of the Signal Connect Drivers Event.
  Performs PpiX request

  @param  NONE  INPUT:
              
  @param  NONE  OUTPUT:
**/
VOID
PerformPpiXRequest ( 
)
{
//-----------------------------------------------------------------------------------------------------
// Handle Out-Of-Band One Touch Request before handling In-Band Request
ExecuteOobOneTouchRequest (NULL);   // OOB One-Touch Request
}
//APTIOV_SERVER_OVERRIDE_RC_END : Hook to Connect Drivers Event
//-----------------------------------------------------------------------------------------------------
