/** @file
  Ihis library is intended to be used by Out-Of-Band One-Touch module.

Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials 
are licensed and made available under the terms and conditions of the BSD License 
which accompanies this distribution.  The full text of the license may be found at 
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DXE_ONE_TOUCH_PLATFORM_H_
#define _DXE_ONE_TOUCH_PLATFORM_H_

// APTIOV_SERVER_OVERRIDE_RC_START : To use AMI's Tree protocol instead Intel's TCG2 protocol
#if 0
#include <Library/Tpm2CommandLib.h>
#include <Protocol/Tcg2Protocol.h>
#else
#include <TCG2\Common\Tpm20PlatformDxe\Tpm20PlatformDxe.h>
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : To use AMI's Tree protocol instead Intel's TCG2 protocol
#include <Protocol/TcgService.h>
#include <Guid/SocketProcessorCoreVariable.h>
#include <Guid/SocketIioVariable.h>
#include <Guid/SetupVariable.h>
#include <Guid/MeRcVariable.h>
// APTIOV_SERVER_OVERRIDE_RC_START : Necessary includes are taken into module's header file
#if 0
#include "../../../../SecurityPkg/Tcg/Tcg2Config/Tcg2ConfigNvData.h"
#endif
#include <Setup.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Necessary includes are taken into module's header file

//-----------------------------------------------------------------------------------------------------
#pragma pack (push, 1)

// APTIOV_SERVER_OVERRIDE_RC_START: To ignore the use of Tcg2ConfigNvData.h

#define TPM_DEVICE_NULL           0
#define TPM_DEVICE_1_2            1
#define TPM_DEVICE_2_0_DTPM       2

typedef struct {
  UINT8   TpmDeviceDetected;
} TCG2_DEVICE_DETECTION;
// APTIOV_SERVER_OVERRIDE_RC_END: To ignore the use of Tcg2ConfigNvData.h

//-----------------------------------------------------------------------------------------------------
// ****************************	PLATFORM SPECIFIC DATA STRUCTURE **************************************
//-----------------------------------------------------------------------------------------------------
// OT_PLATFORM_DATA: Platform specific data as implemented in BIOS.
typedef	struct	{
  SYSTEM_CONFIGURATION			*SystemConfig;		// SYSTEM_CONFIGURATION setup data
//APTIOV_SERVER_OVERRIDE_RC_START : To Use AMI's Setup data for password
  AMITSESETUP                   AmiTseData;        // AMI TSE setup data
//APTIOV_SERVER_OVERRIDE_RC_END : To Use AMI's Setup data for password 
  SOCKET_PROCESSORCORE_CONFIGURATION	Spcc;			// SOCKET_PROCESSORCORE_CONFIGURATION setup data
  SOCKET_IIO_CONFIGURATION		Sic;			// SOCKET_IIO_CONFIGURATION setup data
  TCG2_DEVICE_DETECTION			T2dd;			// TCG2_DEVICE_DETECTION setup data
// APTIOV_SERVER_OVERRIDE_RC_START : To use AMI' setup to get TCG2 configuration
#if 0
  TCG2_CONFIGURATION			T2c;			// TCG2_CONFIGURATION setup data
#else
  SETUP_DATA                    SetupData;
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : To use AMI' setup to get TCG2 configuration
  ME_RC_CONFIGURATION			Merc;			// ME_RC_CONFIGURATION setup data
  UINT32				Sca;			// SYSTEM_CONFIGURATION attribute
  UINT32				Spcca;			// SOCKET_PROCESSORCORE_CONFIGURATION attribute
  UINT32				Sica;			// SOCKET_IIO_CONFIGURATION attribute
  UINT32				T2dda;			// TCG2_DEVICE_DETECTION attribute
// APTIOV_SERVER_OVERRIDE_RC_START
#if 0
  UINT32				T2ca;			// TCG2_CONFIGURATION attribute
#else
  UINT32                Sda;            // SETUP_DATA attribute
#endif
// APTIOV_SERVER_OVERRIDE_RC_END
  UINT32				Merca;			// ME_RC_CONFIGURATION attribute
  BOOLEAN				Scm;			// TRUE: SYSTEM_CONFIGURATION setup data modified
  BOOLEAN				Spccm;			// TRUE: SOCKET_PROCESSORCORE_CONFIGURATION setup data modified
  BOOLEAN				Sicm;			// TRUE: SOCKET_IIO_CONFIGURATION setup data modified
  BOOLEAN				T2cm;			// TRUE: TCG2_CONFIGURATION setup data modified
  BOOLEAN				Mercm;			// TRUE: ME_RC_CONFIGURATION setup data modified
  // APTIOV_SERVER_OVERRIDE_RC_START: For build purpose used AMI's TREE protocol instead Intel's TCG2 since both applies same
#if 0
  EFI_TCG2_PROTOCOL			*Tcg2Protocol;		// Ptr to EFI_TCG2_PROTOCOL (valid for TPM2.0)
#else
  EFI_TREE_PROTOCOL			*Tcg2Protocol;		// Ptr to EFI_TCG2_PROTOCOL (valid for TPM2.0)
#endif
  // APTIOV_SERVER_OVERRIDE_RC_END: For build purpose used AMI's TREE protocol instead Intel's TCG2 since both applies same
  EFI_TCG_PROTOCOL			*TcgProtocol;		// Ptr to EFI_TCG_PROTOCOL (valid for TPM1.2)
  TPM_PERMANENT_FLAGS			Tpm12PermanentFlags;	// TPM1.2 Permanent Flags (read on entry, updated during task execution) (valid for TPM1.2)
  BOOLEAN				Tpm12PreparedForTask;	// TRUE: TPM1.2 prepared for task execution (valid for TPM1.2)
  BOOLEAN				Tpm12ActivateStateChange;// TRUE: TPM1.2 Activate/Deactivate State Changed (valid for TPM1.2)
								//	TPM1.2 Activate <--> Deactivate requires reboot even before any TPM1.2 task can be performed
  UINT16				PasswordAttribute;	// Bit-mapped Password Attribute (definition same as OXP_HEADER.PasswordAttribute)
  UINT16				FeatureSupported;	// Bit-mapped Feature Supported (definition same as OXP_HEADER.FeatureSuported)
  UINT16				FeatureEnabled;		// Bit-mapped Feature Enabled (definition same as OXP_HEADER.FeatureEnabled)
} OT_PLATFORM_DATA;
//
// Bit-mapped definition of different password
#define	OT_NO_PASSWORD			0x00
#define	OT_ADMIN_PASSWORD		0x01
#define	OT_USER_PASSWORD		0x02
//
//-----------------------------------------------------------------------------------------------------
// ****************************	FUNCTION PROTOTYPE ****************************************************
//-----------------------------------------------------------------------------------------------------
/**
  DisplayBinaryData(): Display binary data.

  @param  Data	INPUT: UINT8*, Ptr to data
  @param  Size	INPUT: UINTN, Data size in bytes

  @retval None
**/
VOID
DisplayBinaryData (
  UINT8      *Data,
  UINTN     Size
  );
//
//-----------------------------------------------------------------------------------------------------
/**
  OT_AllocatePlatformInformationArea(): Allocate platform information area.

  @param  None		INPUT: None
  @param  Opd		OUTPUT: Ptr to OT_PLATFORM_DATA, Success
				NULL, Error
**/
OT_PLATFORM_DATA*
OT_AllocatePlatformInformationArea (
  );
//
//------------------------------------------------------------------------------------------------------
/**
  OT_ClearAndDeallocatePlatformInformationArea(): Clear and Deallocate platform information area.

  @param  Opd		INPUT: Ptr to OT_PLATFORM_DATA
  @param  None		OUTPUT

  Note:	Before deallocating, clear platform information area to prevent the data from being hacked.

**/
VOID
OT_ClearAndDeallocatePlatformInformationArea (
  IN  OT_PLATFORM_DATA	*Opd
  );
//
//------------------------------------------------------------------------------------------------------
/**
  OT_ReadPlatformInformation(): Read platform information.

  @param  Opd		INPUT/OUTPUT: Ptr to OT_PLATFORM_DATA where platform information to be read
  @param  Result	OUTPUT: BOOLEAN, TRUE/FALSE: Success/Error

**/
BOOLEAN
OT_ReadPlatformInformation (
  IN OUT OT_PLATFORM_DATA	*Opd
  );
//
//-----------------------------------------------------------------------------------------------------
/**
  OT_WritePlatformInformation(): Write platform information, if necessary.

  @param  Opd		INPUT: Ptr to OT_PLATFORM_DATA having platform information
  @param  Result	OUTPUT: BOOLEAN, TRUE/FALSE: Success/Error

**/
BOOLEAN
OT_WritePlatformInformation (
  IN OUT OT_PLATFORM_DATA	*Opd
  );
//
//-------------------------------------------------------------------------------------------------------
/**
  Returns the information of different installed passwords.

  @param[in]	Data		SYSTEM_CONFIGURATION*: Ptr to System configuration

  @retval	PasswordBitmap	UINT8: Bit-mapped information of installed passwords
				OT_NO_PASSWORD	No password installed
				OT_ADMIN_PASSWORD	Administrative password installed
				OT_USER_PASSWORD	User password installed
**/
UINT8
OT_CheckIfPasswordInstalled (
//APTIOV_SERVER_OVERRIDE_RC_START : To use AMI's setup password
#if 0
    IN  SYSTEM_CONFIGURATION  *Data
#else
    IN  AMITSESETUP           *Data
#endif
//APTIOV_SERVER_OVERRIDE_RC_END : To use AMI's setup password
);
//
//-------------------------------------------------------------------------------------------------------
/**
  Read system configuration.

  @param[in]	Data		SYSTEM_CONFIGURATION*: Ptr to System configuration
  @param[out]	Result		OUTPUT: BOOLEAN, TRUE/FALSE: Success/Error

**/
VOID*
OT_ReadSystemConfiguration (
  IN  SYSTEM_CONFIGURATION	*Data
  );
//
//-------------------------------------------------------------------------------------------------------
/**
  Match the given UnicodeZ password with the BIOS Administrative Password.

  @param[in]	*Scd		SYSTEM_CONFIGURATION*: Ptr to SYSTEM_CONFIGURATION
  @param[in]	*Password	CHAR16*: Ptr to given UnicodeZ password

  @retval	BOOLEAN		TRUE/FALSE: Success/Error

**/
BOOLEAN
OT_MatchAdminPassword (
// APTIOV_SERVER_OVERRIDE_RC_START: To use AMI's setup Password instead Intel's
#if 0
  IN  SYSTEM_CONFIGURATION	*Scd,
#else
  IN  AMITSESETUP   *AmiTseData,
#endif
// APTIOV_SERVER_OVERRIDE_RC_END: To use AMI's setup Password instead Intel's
  IN  CHAR16	*Password
  );
//
//------------------------------------------------------------------------------------------------------
// ****************************	dTPM Function Prototypes ***********************************************
//-----------------------------------------------------------------------------------------------------
/**
  OT_PrepareTpmBeforeOobTask(): Prepare TPM before performing OOB Task.

  @param[in]  Opd	INPUT: Ptr to OT_PLATFORM_DATA

  @retval     Result	OUTPUT: UINT8, Status code

**/
UINT8
OT_PrepareTpmBeforeOobTask (
  IN     OT_PLATFORM_DATA	*Opd
  );
//
//-----------------------------------------------------------------------------------------------------
/**
  OT_PrepareTpmAfterOobTask(): Prepare TPM after performing OOB Task.

  @param[in]  Opd	INPUT: Ptr to OT_PLATFORM_DATA

  @retval     Result	OUTPUT: UINT8, Status code

**/
UINT8
OT_PrepareTpmAfterOobTask (
  IN     OT_PLATFORM_DATA	*Opd
  );
//
//------------------------------------------------------------------------------------------------------
/**
  OT_DTpmClear(): Clear dTPM.

  @param  Auth		INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_DTPM_CLEAR_ERROR
**/
UINT8
OT_DTpmClear (
  IN     TPM2B_AUTH		*Auth,
  IN OUT OT_PLATFORM_DATA	*Opd
  );
//
//------------------------------------------------------------------------------------------------------
/**
  OT_DTpmEnable(): Enable dTPM.

  @param  Auth		INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_DTPM_ENABLE_ERROR, OXP_DTPM_ACTIVATE_ERROR
**/
UINT8
OT_DTpmEnable (
  IN     TPM2B_AUTH		*Auth,
  IN OUT OT_PLATFORM_DATA	*Opd
  );
//
//------------------------------------------------------------------------------------------------------
/**
  OT_DTpmDisable(): Disable dTPM.

  @param  Auth		INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_DTPM_DISABLE_ERROR
**/
UINT8
OT_DTpmDisable (
  IN     TPM2B_AUTH		*Auth,
  IN OUT OT_PLATFORM_DATA	*Opd
  );
//
//------------------------------------------------------------------------------------------------------
// ****************************	fTPM Function Prototypes ***********************************************
//------------------------------------------------------------------------------------------------------
/**
  OT_FTpmClear(): Clear fTPM.

  @param  Auth		INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_DTPM_CLEAR_ERROR
**/
UINT8
OT_FTpmClear (
  IN     TPM2B_AUTH		*Auth,
  IN OUT OT_PLATFORM_DATA	*Opd
  );
//
//------------------------------------------------------------------------------------------------------
/**
  OT_FTpmEnable(): Enable fTPM.

  @param  Auth		INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_FTPM_ENABLE_ERROR
**/
UINT8
OT_FTpmEnable (
  IN     TPM2B_AUTH		*Auth,
  IN OUT OT_PLATFORM_DATA	*Opd
  );
//
//------------------------------------------------------------------------------------------------------
/**
  OT_FTpmDisable(): Disable fTPM.

  @param  Auth		INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_FTPM_DISABLE_ERROR
**/
UINT8
OT_FTpmDisable (
  IN     TPM2B_AUTH		*Auth,
  IN OUT OT_PLATFORM_DATA	*Opd
  );
//
//------------------------------------------------------------------------------------------------------
// ****************************	TXT Function Prototypes ************************************************
//------------------------------------------------------------------------------------------------------
/**
  OT_TxtEnable(): Enable TXT.

  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_TXT_ENABLE_ERROR
**/
UINT8
OT_TxtEnable (
  IN OUT OT_PLATFORM_DATA	*Opd
  );
//
//------------------------------------------------------------------------------------------------------
/**
  OT_TxtDisable(): Disable TXT.

  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_TXT_ENABLE_ERROR
**/
UINT8
OT_TxtDisable (
  IN OUT OT_PLATFORM_DATA	*Opd
  );
//
//-----------------------------------------------------------------------------------------------------
#pragma pack (pop)
//-----------------------------------------------------------------------------------------------------

#endif
