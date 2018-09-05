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

#ifndef _DXE_OOB_ONE_TOUCH_H_
#define _DXE_OOB_ONE_TOUCH_H_

#include <IndustryStandard/Tpm20.h>
#include <IndustryStandard/TcgPhysicalPresence.h>
//APTIOV_SERVER_OVERRIDE_RC_START : To use AMI's Tree protocol instead Intels
#if 0
#include <Protocol/Tcg2Protocol.h>
#endif
//APTIOV_SERVER_OVERRIDE_RC_END : To use AMI's Tree protocol instead Intels

//-----------------------------------------------------------------------------------------------------
#pragma pack (push, 1)
//-----------------------------------------------------------------------------------------------------
// ********************	OOB Specification *************************************************************
//-----------------------------------------------------------------------------------------------------
// Implementation Version Number
//	Bug Solve should update the Minor Version.
//	Ehancement/New Feature MUST update the Major Version.
//
#define	OXP_MAJOR_VERSION_IMP	0x00	// Major Version of Implementation
#define	OXP_MINOR_VERSION_IMP	0x07	// Minor Version of Implementation
//
// OXP_HEADER
//
typedef		struct	{
    UINT32	Signature;		// Signature
    UINT16	TotalLength;		// Length of OOB_TXT_TPM structure in bytes
    UINT16	HeaderLength;		// Length of header in bytes
    UINT8	Version;		// Version
    UINT8	Checksum;		// Checksum
    UINT8	TaskAndResult;		// Bit-mapped info of task and result
    UINT8	Status;			// Status
    UINT16	PasswordAttribute;	// Bit-mapped info of supported password
    UINT16	FeatureSupported;	// Bit-mapped info of supported TXT/TPM features
    UINT16	FeatureEnabled;		// Bit-mapped info of enabled TXT/TPM features
    UINT32	State;			// Bit-mapped info of state of task
    UINT8	MajorVersionImp;	// Major Version of Implementation
    UINT8	MinorVersionImp;	// Minor Version of Implementation
    UINT8	Reserved [0x08];	// Reserved for future use
} OXP_HEADER;
//
// Signature: Indicates the signature of the OOB TXT/TPM information.
//	On input, signature is ‘$OXP’. Offset-0 contains ‘$’, Offset-1 contains ‘O’, Offset-2 contains ‘X’,
//	and Offset-3 contains ‘P’. Caller must supply this signature while requesting an OOB task.
//	On output, signature is ‘PXO$’. Offset-0 contains ‘P’, Offset-1 contains ‘X’, Offset-2 contains ‘O’,
//		and Offset-3 contains ‘$’. BIOS must output this signature after performing the requested task.
#define	OXP_SIGNATURE_INPUT		0x50584F24	// $OXP signature on Input
#define	OXP_SIGNATURE_OUTPUT		0x244F5850	// PXO$ signature on Output
//
// TotalLength: Indicates the total length of all information in bytes i.e. length of OOB_TXT_TPM structure.
//	The content of this field depends on the specific TXT/TPM task.
//
// HeaderLength: Indicates the length of the OXP_HEADER in bytes.
//	The header length is 0x20 for this specification and is not changed after it is initialized.
//
// Version: Indicates the version number of the information.
//	The version number is 0x01 for this specification and is not changed after it is initialized.
#define	OXP_VERSION			0x01		// OXP Version
//
// Checksum: Provides the checksum of OOB_TXT_TPM structure. The checksum is calculated in a manner such
//	that byte-addition of all bytes (including this field) in OOB_TXT_TPM is 0.
//	The content of this field is changed whenever any information in OOB_TXT_TPM is modified.
//
// TaskAndResult: Bit-mapped information of TXT/TPM task and the result.
#define	OXP_TASK_BIT_MASK			0x7F	// Bit6-0: Task# or Use-Case#
#define	  OXP_TASK_BIT_N			0x00	//	#of bit shift for alignment
							//	Task#	Description
							//	00	Reserved for historical purpose
#define	  OXP_TASK_TXT_TPM_SUPPORT_DISCOVERY	   0x01	//	01	TXT/TPM Support Discovery
#define	  OXP_TASK_TXT_TPM_ENABLE_STATUS_DISCOVERY 0x02	//	02	TXT/TPM Enable Status Discovery
#define	  OXP_TASK_ENABLE_TXT_DTPM		   0x03	//	03	Enable TXT/dTPM
#define	  OXP_TASK_DTPM_OWNER_CLEAR_ONLY	   0x04	//	04	dTPM Owner Clear Only
#define	  OXP_TASK_DTPM_CLEAR_DTPM_ACTIVATION	   0x05	//	05	dTPM Clear and dTPM Activation
#define	  OXP_TASK_DTPM_CLEAR_ENABLE_TXT_DTPM	   0x06	//	06	dTPM Clear and Enable TXT/dTPM
#define	  OXP_TASK_TPM_PRESENCE_STATUS_DISCOVERY   0x07	//	07	TPM Presence Status Discovery
#define	  OXP_TASK_TXT_LAUNCH_STATUS_DISCOVERY	   0x08	//	08	TXT Launch Status Discovery
#define	  OXP_TASK_DTPM_OWNERSHIP_STATUS_DISCOVERY 0x09	//	09	dTPM Ownership Status Discovery
#define	  OXP_TASK_FTPM_OWNERSHIP_STATUS_DISCOVERY 0x0A	//	0A	fTPM Ownership Status Discovery
#define	  OXP_TASK_ENABLE_TXT_FTPM		   0x0B	//	0B	Enable TXT/fTPM
#define	  OXP_TASK_FTPM_OWNER_CLEAR_ONLY	   0x0C	//	0C	fTPM Owner Clear Only
#define	  OXP_TASK_FTPM_CLEAR_FTPM_ACTIVATION	   0x0D	//	0D	fTPM Clear and TPM Activation
#define	  OXP_TASK_FTPM_CLEAR_ENABLE_TXT_FTPM	   0x0E	//	0E	fTPM Clear and Enable TXT/fTPM
#define	  OXP_TASK_DISABLE_TXT_ONLY		   0x0F	//	0F	Disable TXT only
#define	  OXP_TASK_DISABLE_DTPM_ONLY		   0x10	//	10	Disable DTPM only
#define	  OXP_TASK_DISABLE_FTPM_ONLY		   0x11	//	11	Disable FTPM only
							//	12-7F	Reserved for future use
#define	OXP_TASK_PERFORMED_BIT			0x80	// Bit-7 : 0/1, Task Not/Yes performed
							//	   A value of 1 indicates that task is performed and
							//	   the Status field indicates success/error.
							//	   The caller must set Bit-7 = 0 while requesting
							//	   a task to be performed
// Status: Indicates the status of the performed task.
//	This field is valid only when Bit-7 of TaskAndResult field is set to 1.
//	The status codes 0x00-0xBF are defined by the specification.
//	The status codes 0xC0-0xDF are reserved for Vendor-specific use.
//	The status codes 0xE0-0xFF are reserved for Intel internal use (e.g. debugging, etc.)
#define	OXP_SUCCESS			0x00		// Success
#define	OXP_INVALID_TASK		0x01		// Invalid task
#define	OXP_TPM_LOCKED				   0x02	// TPM Locked (valid for TPM1.2)
#define	OXP_TPM_PHYSICAL_PRESENCE_CMD_ENABLE_ERROR 0x03	// TPM Physical Presence command enable error (valid for TPM1.2)
#define	OXP_TPM_PHYSICAL_PRESENCE_ERROR		   0x04	// TPM Physical Presence set error (valid for TPM1.2)
							// 0x05-0x0F: Reserved for generic error codes
#define	OXP_TXT_NOT_SUPPORTED		0x10		// TXT not supported
#define	OXP_TXT_ENABLE_ERROR		0x11		// TXT enable error
							// 0x12-0x1F: Reserved for TXT related error codes
#define	OXP_DTPM_NOT_SUPPORTED			   0x20	// dTPM not supported (and requested task cannot be performed)
#define	OXP_DTPM_ENABLE_ERROR			   0x21	// dTPM enable error
#define	OXP_DTPM_CLEAR_ERROR			   0x22	// dTPM clear error
#define	OXP_DTPM_DISABLE_ERROR			   0x23	// dTPM disable error
#define	OXP_DTPM_NOT_ENABLED			   0x24	// dTPM not enabled (and requested task cannot be performed)
#define	OXP_DTPM_NOT_ACTIVATED			   0x25	// dTPM not activated (and requested task cannot be performed)
#define	OXP_DTPM_ACTIVATE_ERROR			   0x26	// dTPM activate error
#define	OXP_DTPM_DEACTIVATE_ERROR		   0x27	// dTPM deactivate error
							// 0x28-0x2F: Reserved for dTPM related error codes
#define	OXP_AUTHENTICATION_ERROR	0x30		// Authentication failure
#define	OXP_PASSWORD_TYPE_NOT_SUPPORTED	0x31		// Specified password data type not supported
							// 0x32-0x3F: Reserved for authentication related error codes
#define	OXP_FTPM_NOT_SUPPORTED			   0x40	// fTPM not supported (and requested task cannot be performed)
#define	OXP_FTPM_ENABLE_ERROR			   0x41	// fTPM enable error
#define	OXP_FTPM_CLEAR_ERROR			   0x42	// fTPM clear error
#define	OXP_FTPM_DISABLE_ERROR			   0x43	// fTPM disable error
#define	OXP_FTPM_NOT_ENABLED			   0x44	// fTPM not enabled (and requested task cannot be performed)
							// 0x45-0x4F: Reserved for fTPM related error codes
							// 0x50-0xBF: Reserved for future use
							// 0xC0-0xDF: Reserved for Vendor specific use
							// 0xE0-0xFE: Reserved for Intel internal use
#define	OXP_UNKNOWN_ERROR			   0xFF	// Unknown error (cause cannot be determined --> BUG)
//
// PasswordAttribute: Bit-mapped information of password supported in the platform.
//	Only one bit (either Bit-1 or Bit-0) must be set to 1 indicating ASCII or Unicode string.
//	If Bit-2 is 0, it indicates that SHA1 value of Password string is not supported.
//	If Bit-2 is 1, it indicates that SHA1 value of Password string is supported and Bit1-0 setting indicates whether the SHA1 value is calculated on ASCII string or Unicode string.
//	If SHA value of password string is supported, Bit-14 indicates whether the SHA value of password string must be calculated by including or not including the NULL-termination character.
#define	OXP_PASSWORD_ASCII_BIT			0x01	// Bit-0:   0/1, Password ASCII string Not/Yes supported
#define	OXP_PASSWORD_UNICODE_BIT		0x02	// Bit-1:   0/1, Password Unicode string Not/Yes supported
#define	OXP_PASSWORD_SHA1_BIT			0x04	// Bit-2:   0/1, SHA1 value of password Not/Yes supported
							// Bit12-3: Reserved for future use (set to 0)
#define	OXP_PASSWORD_IGNORE_BIT			0x2000	// Bit-13:  0/1, Use/Ignore BIOS Administrative password during authentication
#define	OXP_PASSWORD_SHA_INCLUDE_NULL_BIT	0x4000	// Bit-14:  0/1, SHA value Not/Yes include NULL-termination character of password string
#define	OXP_PASSWORD_ADMIN_INSTALLED_BIT	0x8000	// Bit-15:  0/1, Administrative password Not/Yes installed
//
#define	OXP_PASSWORD_TYPE_BIT_MASK	(OXP_PASSWORD_ASCII_BIT | OXP_PASSWORD_UNICODE_BIT | OXP_PASSWORD_SHA1_BIT)
//
// FeatureSupported: Bit-mapped information of TXT/TPM features supported in the platform.
#define	OXP_TXT_SUPPORT_BIT			0x01	// Bit-0:   0/1, TXT Not/Yes supported
#define	OXP_TPM_SUPPORT_BIT_MASK		0x06	// Bit2-1:  TPM support information
#define	  OXP_DTPM_SUPPORT_BIT			0x02	//	Bit-1: 0/1, dTPM Not/Yes supported
#define	  OXP_FTPM_SUPPORT_BIT			0x04	//	Bit-2: 0/1, fTPM Not/Yes supported
//#define	  OXP_TPM_SUPPORT_BIT_N			0x01	//	#of bit shift for alignment
//#define	  OXP_TPM_NOT_SUPPORT			0x00	//		00b: TPM not supported
//#define	  OXP_DTPM_SUPPORT			0x01	//		01b: dTPM supported
//#define	  OXP_FTPM_SUPPORT			0x02	//		10b: fTPM supported
//#define	  OXP_DTPM_FTPM_SUPPORT			0x03	//		11b: dTPM, fTPM supported
							// Bit15-3: Reserved for future use (set to 0)
//
// FeatureEnabled: Bit-mapped information of TXT/TPM features that are currently enabled in the platform.
//	Since this information may change from boot to boot, it represents the features that are enabled
//	in the current boot (if the task is performed in runtime i.e. when the system is ON and running)
//	or in the last boot (if the task is performed while the system is OFF).
#define	OXP_TXT_ENABLED_BIT			0x01	// Bit-0:   0/1, TXT Not/Yes enabled
#define	OXP_TPM_ENABLED_BIT_MASK		0x06	// Bit2-1:  TPM enabled information
#define	  OXP_DTPM_ENABLED_BIT			0x02	//	Bit-1: 0/1, dTPM Not/Yes enabled
#define	  OXP_FTPM_ENABLED_BIT			0x04	//	Bit-2: 0/1, fTPM Not/Yes enabled
//#define	  OXP_TPM_ENABLED_BIT_N			0x01	//	#of bit shift for alignment
//#define	  OXP_TPM_NOT_ENABLED			0x00	//		00b: TPM not enabled
//#define	  OXP_DTPM_ENABLED			0x01	//		01b: dTPM enabled
//#define	  OXP_FTPM_ENABLED			0x02	//		10b: fTPM enabled
//#define	  OXP_DTPM_FTPM_ENABLED_INVALID		0x03	//		11b: Invalid, since either dTPM or fTPM (not both) can be enabled
							// Bit15-3:	Reserved for future use (set to 0)
//
// State: Bit-mapped information of state of task (BIOS INTERNAL USE ONLY, caller may not change this field).
#define	OXP_STATE_SIGNATURE_INVALID_BIT			 0x00000001	// Bit-0:   0/1, Valid/Invalid Signature in OXP_HEADER
#define	OXP_STATE_TOTAL_LENGTH_INVALID_BIT		 0x00000002	// Bit-1:   0/1, Valid/Invalid Total Length in OXP_HEADER
#define	OXP_STATE_HEADER_LENGTH_INVALID_BIT		 0x00000004	// Bit-2:   0/1, Valid/Invalid Header Length OXP_HEADER
#define	OXP_STATE_TOTAL_LENGTH_LESS_HEADER_LENGTH_BIT	 0x00000008	// Bit-3:   0/1, Total Length (>=)/(<) Header Length in OXP_HEADER
#define	OXP_STATE_VERSION_INVALID_BIT			 0x00000010	// Bit-4:   0/1, Valid/Invalid Version in OXP_HEADER
#define	OXP_STATE_CHECKSUM_INVALID_BIT			 0x00000020	// Bit-5:   0/1, Valid/Invalid Checksum in OXP_HEADER
									// Bit7-6........Reserved for future use
#define	OXP_STATE_VERISON_IMP_UPDATED_BIT		 0x00000100	// Bit-8:   0/1, Implementation Version Number not/yes updated in OXP_HEADER
#define	OXP_STATE_DATA_DELETED_OUTPUT_BIT		 0x00000200	// Bit-9:   0/1, OXP_PASSWORD_DATA not/yes deleted from OOB output data
#define	OXP_STATE_PASSWORD_IGNORED_BIT			 0x00000400	// Bit-10:  0/1, Supplied Password checked/ignored during authentication
#define	OXP_STATE_FEATURE_ENABLED_UPDATED_BIT		 0x00000800	// Bit-11:  0/1, Feature Enabled not/yes updated in OXP_HEADER
#define	OXP_STATE_FEATURE_SUPPORTED_UPDATED_BIT		 0x00001000	// Bit-12:  0/1, Feature Supported not/yes updated in OXP_HEADER
#define	OXP_STATE_PASSWORD_ATTRIBUTE_UPDATED_BIT	 0x00002000	// Bit-13:  0/1, Password Attribute not/yes updated in OXP_HEADER
									// Bit-14........Reserved for future use
#define	OXP_STATE_TASK_PENDING_BIT			 0x00008000	// Bit-15:  0/1, Task not/yes pending
									//		 1: Task is only accepted but not yet performed (to be performed following the reset)
#define	OXP_STATE_PASSWORD_TOO_LARGE_BIT		 0x00010000	// Bit-16:  0/1, OXP_PASSWORD_DATA password larger than internal area, increase internal size
#define	OXP_STATE_PASSWORD_EMPTY_BIT			 0x00020000	// Bit-17:  0/1, OXP_PASSWORD_DATA password empty
#define	OXP_STATE_PASSWORD_NOT_NULL_TERMINATED_BIT	 0x00040000	// Bit-18:  0/1, OXP_PASSWORD_DATA password string yes/not NULL terminated
#define	OXP_STATE_PASSWORD_SHA_TYPE_NOT_SUPPORTED_BIT	 0x00080000	// Bit-19:  0/1, OXP_PASSWORD_DATA password SHA type yes/not supported
#define	OXP_STATE_PASSWORD_STRING_TYPE_NOT_SUPPORTED_BIT 0x00100000	// Bit-20:  0/1, OXP_PASSWORD_DATA password string type yes/not supported
#define	OXP_STATE_PASSWORD_BIOS_ADMIN_NOT_INSTALLED_BIT	 0x00200000	// Bit-21:  0/1, OXP_PASSWORD_DATA BIOS Admin password yes/not installed
#define	OXP_STATE_PASSWORD_DATA_VALID_NOT_SET_BIT	 0x00400000	// Bit-22:  0/1, OXP_PASSWORD_DATA yes/not valid
#define	OXP_STATE_PASSWORD_DATA_ID_INVALID_BIT		 0x00800000	// Bit-23:  0/1, OXP_PASSWORD_DATA Identifier yes/not valid
#define	OXP_STATE_PASSWORD_DATA_LENGTH_INVALID_BIT	 0x01000000	// Bit-24:  0/1, OXP_PASSWORD_DATA length yes/not valid
#define	OXP_STATE_TPM_PREPARATION_BEFORE_TASK_ERROR_BIT	 0x02000000	// Bit-25:  0/1, TPM preparation before task not/yes error
#define	OXP_STATE_TPM_PREPARATION_AFTER_TASK_ERROR_BIT	 0x04000000	// Bit-26:  0/1, TPM preparation after task not/yes error
									// Bit28-27......Reserved for future use
#define	OXP_STATE_PLATFORM_DATA_WRITE_ERROR_BIT		 0x20000000	// Bit-29:  0/1, Write BIOS Setup Option data Success/Error
#define	OXP_STATE_OOB_DATA_FRESH_GENERATED_BIT		 0x40000000	// Bit-30:  0/1, OOB data not/yes generated from scratch
#define	OXP_STATE_RESET_BIT				 0x80000000	// Bit-31:  0/1, Reset Not/Yes resulted from last performed task
//
// MajorVersionImp, MinorVersionImp: Indicates the major and minor version of the implementation.
//	The version number must be updated with any change (bug solve, enhancememnt, etc.) in the implementation.
//
//--------------------------------------------------------
// OXP_PASSWORD_DATA: For the task (Use-Case) that requires user authentication, the caller must supply
//	the administrative password in OXP_PASSWORD_DATA format as follows:
//
typedef		struct	{
    UINT8	Id;			// Identifier of the data
    UINT16	Length;			// Length of OXP_PASSWORD_DATA
    UINT16	Attribute;		// Bit-mapped Attribute
    UINT8	Password [0x14];	// Password information: Array of bytes, length P depends on the information type (ASCII, Unicode, SHA1, etc.)
					// Currently max length of this field is 0x14 bytes for SHA1
} OXP_PASSWORD_DATA;
//
// Id: Identifies the nature of the data.
							// 0x00:	Reserved for historical purpose
#define	OXP_ID_PASSWORD				0x01	// 0x01:	Password Information
							// 0x0x2-0xFE:	Reserved for future use
							// 0xFF:	Reserved for historical purpose
//
// Length: Provides the length D of OXP_PASSWORD_DATA in bytes.
//
// Attribute: Indicates the nature of password information present in Password field.
//	Only one bit (either Bit-1 or Bit-0) must be set to 1 indicating ASCII or Unicode string.
//	If Bit-2 is 0, it indicates that Password information is supplied as string and the string type
//		(ASCII or Unicode) is indicated by Bit1-0 setting.
//	If Bit-2 is 1, it indicates that Password information is supplied as SHA1 value and Bit1-0 setting
//		indicates whether the SHA1 value is calculated on ASCII string or Unicode string.
//	If SHA value of password string is supplied, Bit-14 indicates whether the SHA value of password
//		string is calculated by including or not including the NULL-termination character.
//
							// Bit12-0: Identical to Bit12-0 of OXP_HEADER.PasswordAttribute field
							// Bit-13:  NOT USED (IGNORED)
							// Bit-14:  Identical to Bit-14 of OXP_HEADER.PasswordAttribute field
#define	OXP_PASSWORD_DATA_VALID_BIT		0x8000	// Bit-15:  0/1, Password data Not/Yes valid
//
// Password: Provides the information of the password as indicated by Attribute field. The length P of this
//	field is P = (D – 5) bytes, where D = Length of OXP_PASSWORD_DATA (i.e. the value in Length field).
//
//--------------------------------------------------------
// OOB_TXT_TPM structure: Data format stored in ME non-volatile space.
//
typedef	struct	{
  OXP_HEADER		Header;			// Header: Valid for all OOB tasks
  OXP_PASSWORD_DATA	PasswordData;		// Password Data: Valid for OOB tasks requiring authentication
} OOB_TXT_TPM;
//
//-----------------------------------------------------------------------------------------------------
// ********************	OOB Implementation ************************************************************
//-----------------------------------------------------------------------------------------------------
//
#define EFI_OOB_TXT_TPM_DATA_GUID \
  { \
    0x2a8ec022, 0x11d0, 0x43bb, { 0xa7, 0x18, 0xf0, 0x8, 0x22, 0x39, 0xd8, 0xd0 } \
  }
//
#define EFI_OOB_TXT_TPM_DATA_VARIABLE  L"OobTxtTpmData"
//
//------------------------------------------------------------------------
// FATAL Error during OOB data processing (NOT stored anywhere, but displayed in DEBUG messages)
#define	OOB_FATAL_MEMORY_NOT_AVAILABLE_BIT		 0x00000001	// Bit-0:  0/1, Memory yes/not available for platform information
#define	OOB_FATAL_NV_STORAGE_SIZE_ZERO_BIT		 0x00000002	// Bit-1:  0/1, OOB Data Non-Volatile Storage Size not/yes Zero bytes
#define	OOB_FATAL_NV_STORAGE_NOT_SUPPORTED_BIT		 0x00000004	// Bit-2:  0/1, OOB Data Non-Volatile Storage yes/not supported
#define	OOB_FATAL_PLATFORM_DATA_READ_ERROR_BIT		 0x00000008	// Bit-3:  0/1, Platform Data Read Success/Error
#define	OOB_FATAL_OOB_DATA_READ_ERROR_BIT		 0x00000010	// Bit-4:  0/1, Read OOB Data from NV Storage Success/Error
#define	OOB_FATAL_OOB_DATA_WRITE_ERROR_BIT		 0x00000020	// Bit-5:  0/1, Write OOB Data to NV Storage Success/Error
#define	OOB_FATAL_OOB_DATA_READ_SIZE_ZERO_BIT		 0x00000040	// Bit-6:  0/1, Read OOB Data size from to NV Storage not/yes Zero bytes
									// Bit15-7......Reserved for future use
//
//-----------------------------------------------------------------------------------------------------
// ****************************	FUNCTION PROTOTYPE ****************************************************
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
  );
//
//-----------------------------------------------------------------------------------------------------
#pragma pack (pop)
//-----------------------------------------------------------------------------------------------------

#endif
