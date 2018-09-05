/*++

This file contains a 'Sample Driver' and is licensed as such
under the terms of your license agreement with Intel or your
vendor.  This file may be modified by the user, subject to  
the additional terms of the license agreement               

--*/

/*++

Copyright (c)  2009-13 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FwUpdateLib.h

Abstract:

  FW Update Local Sample Code Header file
  
--*/

#define INVALID_DATA_FORMAT_VERSION 0
#define INVALID_MANIFEST_DATA	      1
#define NO_FPT_IMAGE			          2
#define MANIFEST_BUFFER             0x1000
#define FPT_PARTITION_NAME_FPT      0x54504624

//=====
#define bool BOOLEAN

extern
InitializeLib(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
);

extern
ZeroMem(
  IN VOID  *Buffer,
  IN UINTN Size 
);

void _fltused();

typedef UINT32 STATUS;

typedef struct {
	UINT16 Major;
	UINT16 Minor;
	UINT16 Hotfix;
	UINT16 Build;
} FWVersion;

typedef struct {
	FWVersion code;
	FWVersion rcvy;
} FlashVersion;

typedef struct
{
   unsigned long  Data1;
   unsigned short Data2;
   unsigned short Data3;
   unsigned char  Data4[8];
} _UUID;

typedef enum
{
   FWU_ENV_MANUFACTURING = 0,   // Manufacturing update
   FWU_ENV_IFU,                 // Independent Firmware update
}FWU_ENVIRONMENT;

enum errorValues {
    FWU_ERROR_SUCCESS,
    FWU_IME_NO_DEVICE = 8193,
    FWU_UPD_VER_MIS   = 8199,
    FWU_VER_GET_ERR = 8204,
    FWU_CERT_ERR = 8213,
    FWU_REBOOT_NEEDED = 8703,
    FWU_SKU_MISMATCH,
    FWU_VER_MISMATCH,
    FWU_SGN_MISMATCH,
    FWU_GENERAL,
    FWU_UPD_PROCESS,
    FWU_NO_MEMORY = 8710,
    FWU_AUTH = 8712,
    FWU_IMG_HEADER,
    FWU_FILE_OPEN,
    FWU_HTTP_ERROR,
    FWU_USAGE_ERROR,
    FWU_HOSTNAME,
    FWU_UPDATE_TIMEOUT,
    FWU_LOCAL_DIS,
    FWU_SECURE_DIS,
    FWU_IME_UN_SUP_MESS = 8722,
    FWU_NO_UPDATE,
	FWU_IME_NOT_READY,
    FWU_LAST_STATUS,
    FWU_GET_VER_ERR = 8727,
    FWU_IME_SMALL_BUFF,
    FWU_WSMAN_NO = 8734,
    FWU_UNSUPPRT_OS = 8740,
    FWU_ERROR_FW,
    FWU_HECI,
    FWU_UNSUPPRT_PLAT,
    FWU_VERIFY_OEM_ID_ERR,
    FWU_INVALID_OEM_ID = 8745,
    FWU_INVALID_IMG_LENGTH,
    FWU_GLBL_BUFF_UNAVAILABLE,
    FWU_INVALID_FW_PARAMS,
    FWU_AMT_STATUS_INTERNAL_ERROR,
    FWU_AMT_STATUS_NOT_READY = 8750,
    FWU_AMT_STATUS_INVALID_AMT_MODE,
    FWU_AMT_STATUS_INVALID_MESSAGE_LENGTH,
    FWU_SAVE_RESTORE_POINT_ERROR,
    FWU_FILE_WRITE,
    FWU_GET_BLIST_ERROR = 8755,
    FWU_CHECK_VERSION_ERROR,
    FWU_DISPLAY_FW_VERSION,
    FWU_IMAGE_UNDER_VCN,
    FWU_IMAGE_VER_HIST_CHK_FAIL,
    FWU_DOWNGRADE_VETOED = 8760,
    FWU_FW_WRITE_FILE_FAIL,
    FWU_FW_READ_FILE_FAIL,
    FWU_FW_DELETE_FILE_FAIL,
    FWU_PARTITION_LAYOUT_NOT_COMP,
    FWU_DOWNGRADE_NOT_ALLOWED_DATA_MISMATCH = 8765,
	FWU_UPDATE_PASSWORD_NOT_MATCHED,
	FWU_UPDATE_PASSWORD_EXCEED_MAXIMUM_RETRY,
	FWU_UPDATE_PASSWORD_NOT_PROVIDED,
	FWU_UPDATE_POLLING_FAILED,
	FWU_FILE_ALREADY_EXISTS = 8770,
	FWU_FILE_INVALID,
	FWU_USAGE_ERROR_B,
	FWU_AUDIT_POLICY_FAILURE,
	FWU_ERROR_CREATING_FT,
	FWU_SAL_NOTIFICATION_ERROR = 8775,
	FWU_GET_PATTRIB_ERROR,
	FWU_GET_UPD_INFO_STATUS,
	FWU_PID_NOT_EXPECTED,
	FWU_UPDATE_INRECOVERY_MODE_RESTRICT_UPDATE_TO_ATTEMPTED_VERSION,
	FWU_BUFFER_COPY_FAILED,
	FWU_GET_ME_FWU_INFO,
	FWU_APP_REGISTER_OS_FAILURE,
	FWU_APP_UNREGISTER_OS_FAILURE,
	FWU_INVALID_PARTID,
	FWU_LIVE_PING_FAILURE,
	FWU_SERVICE_CONNECT_FAILURE,
	FWU_SERVICE_NOT_AVAILABLE,
	FWU_SERVICE_BUSY,
	FWU_USER_NOT_ADMIN,
	FWU_WMI_FAIL,
	FWU_CHK_BIT_LOCKER_FAIL,
	FWU_REG_CMD_FAIL,
	FWU_UPDATE_IMAGE_BLACKLISTED,
	FWU_DOWNGRADE_NOT_ALLOWED_SVN_RESTRICTION
};
//=====


// below structure is defiend by the fw team
// in fwucommon.h file
typedef struct _UPDATE_FLAGS_LIB
{
   unsigned int RestorePoint:1;           // If set indicate restore point
   unsigned int RestartOperation:1;       // If set indicate restart operation, like lost hard drive etc...
   unsigned int UserRollback:1;			// indicates user has initiated a rollback
   unsigned int Reserve:29;               //
}UPDATE_FLAGS_LIB;

// Used by the tool to perform FULL FW update
typedef enum _UPDATE_TYPE
{
   DOWNGRADE_SUCCESS = 0,
   DOWNGRADE_FAILURE,
   SAMEVERSION_SUCCESS,
   SAMEVERSION_FAILURE,
   UPGRADE_SUCCESS,
   UPGRADE_PROMPT
}UPDATE_TYPE;

//Image type to validate the binary sent to update
//For Full Update - only FULL image type is valid
//For Partial Update - only FULL and PARTIAL image type is valid
//FULL Image => Image with Flash Partition Table, FTPR, and NFTPR
//PARTIAL Image => Image with no Flash Partition Table or FTPR or NFTPR,
//						only WCOD or LOCL
typedef enum _IMAGE_TYPE
{
   FULL = 0,
   PARTIAL,
   RESTORE,
   INVALID
}IMAGE_TYPE;

typedef enum _SKU_TYPE {
   SKU_1_5_MB = 0,
   SKU_5_MB,
   SKU_INVALID
}SKU_TYPE;

//Used by the tool to retrieve FW version information
typedef struct {
	unsigned short Major;
	unsigned short Minor;
	unsigned short Hotfix;
	unsigned short Build;
} VersionLib;

//Should be used by both tool and UNS to retrieve the Updated UPV version
typedef struct _IPU_UPDATED_INFO
{
	unsigned int UpdatedUpvVer;//Version from the update image file that is for updating IPU
	unsigned int Reserved[4];
}IPU_UPDATED_INFO;

// disable the "zero-sized array" warning
#pragma warning(disable:4200)
typedef enum _FWU_STATUS {
	FWU_STATUS_SUCCESS = 0,
	FWU_STATUS_NO_MEMORY = 1,
	FWU_STATUS_NOT_READY = 2,
	FWU_STATUS_ILLEGAL_LENGTH = 3,
	FWU_STATUS_AUTHENTICATION_FAILED = 4,
	FWU_STATUS_INTERNAL_ERROR = 5,
	FWU_STATUS_SKU_FAILURE = 6,
	FWU_STATUS_VERSION_FAILURE = 7
}FWU_STATUS;

// From FWU_if.h

#pragma pack(1)

/**
 * FWU_GET_INFO_MESSAGE - get version
 *
 * @MessageType: FWU_MESSAGE_TYPE_GET_INFO
 */
typedef struct {
    UINT32 MessageType;
} FWU_GET_INFO_MESSAGE;

/**
 * FWU_DATA_MESSAGE - data fragment of the image
 *
 * @MessageType: FWU_MESSAGE_TYPE_DATA
 * @Length: The length of the data field, in Little Endian
 * @Reserved: Reserved, must be 0
 * @Data: The data of the image fragment
 */
typedef struct {
    UINT32 MessageType;
    UINT32 Length;
    UINT8 Reserved[3];
#ifndef _DOS
    UINT8 Data[0];
#endif
} FWU_DATA_MESSAGE;

#pragma pack()

bool 
IsUpdateStatusPending (
  IN  UINT32    status
)
/*++

Routine Description:

  Checks if ME FW Update client is ready to accept an update

Arguments:


Returns:

  true if ready

--*/
;

unsigned int 
CheckPolicyBuffer (
  IN  char*       buffer, 
  IN  int         bufferLength, 
  IN  int         AllowSV, 
  IN  UPDATE_TYPE *Upd_Type,
	IN  VersionLib  *ver
)
/*++

Routine Description:

  This routine determines if the update is allowed given the ME Upgrade/downgrade/same version policy

Arguments:

Returns:

  FWU_ERROR_SUCCESS if operation allowed
  
--*/
;

unsigned int 
FwUpdateFullBuffer (
  IN  char                *buffer,
  IN  unsigned int        bufferLength, 
  IN  char                *_pwd,
  IN  int                 _forceResetLib,
  IN  unsigned int        UpdateEnvironment,	
  IN  _UUID               OemID,
  IN  UPDATE_FLAGS_LIB    update_flags,
  IN  void(*func)(float, float)
)
/*++

Routine Description:

  This routine sends the buffer to the ME FW Update Client

Arguments:

Returns:

  FWU_ERROR_SUCCESS if operation allowed
  
--*/
;

unsigned int 
FWUpdate_QueryStatus_Get_Response (
  IN  unsigned int  *UpdateStatus,
  IN  unsigned int  *TotalStages,
  IN  unsigned int  *PercentWritten,
  IN  unsigned int  *LastUpdateStatus,
  IN  unsigned int  *LastResetType
)
/*++

Routine Description:

  This routine queries the ME Kernel for the update status

Arguments:

Returns:

  FWU_ERROR_SUCCESS if operation allowed
  
--*/
;

bool
VerifyOemId(
  IN _UUID      id
)
/*++

Routine Description:

  The ME FW when created has an OEM ID embedded in it.  For security
  reasons, we need to verify that the OEM ID of the application (mOemID)
  matches the OEM ID of the ME FW.

  NOTE: A connection to the client is required before this routine is 
  executed.

Arguments:

  UUID to check
  

Returns:

  true or false indicating if the match was succesfull
  
--*/
;

EFI_STATUS
GetLastStatus(
  OUT UINT32          *LastFwUpdateStatus
)
/*++

Routine Description:
  
  Get the last status of the FW Update Client - this is used to determine if a previous update
  requires a reboot

Arguments:

  UINT32 pointer for results of previous update. 
  

Returns:

  EFI Error code
  
--*/
;

EFI_STATUS
GetLastUpdateResetType (
  OUT UINT32            *LastResetType
)/*++

Routine Description:
  
  Get the last status of the FW Update Client - this is used to determine if a previous update
  requires a reboot

Arguments:

  UINT32 pointer for results of previous update. 
  

Returns:

  EFI Error code
  
--*/
;

EFI_STATUS
GetInterfaces (
  OUT UINT16          *interfaces
)
;

EFI_STATUS
CheckVersion(
  IN  UINT8           *FileBuffer,
  IN  BOOLEAN         AllowSV,
  OUT FlashVersion    *FlashVersion,
  OUT UINT32          *LastFwUpdateStatus
)
/*++

Routine Description:

  Policy routine to verify if the current FW version is older
  than the version in the image. 

  The routine will return the result of a previous ME FW Update.  If a reset is required
  before an update can occur, the return value will indicate it

  NOTE: A connection to the client is required before this routine is 
  executed.

Arguments:

  UINT8 pointer to update image
  BOOLEAN indicating TRUE for same version update
  FlashVersion pointer for return version numbers of FW Update image
  UINT32 pointer for results of previous update. 
  

Returns:

  EFI_UNSUPPORTED if version is of update image is not supported
  
--*/
;

EFI_STATUS 
StartUpdate(
  IN  UINT8   *FileBuffer,
  IN  UINT32  FileLength,
  IN  CHAR8   *Password,
  IN  VOID    (*DataProgressProc)(),
  IN  UINT32  DataProgessSteps,
  OUT STATUS  *FWU_Status
)
/*++

Routine Description:

  Start the update process by asking the ME FW Update client
  to allocate a buffer for the image.  Note that the ME does not update the flash until
  the FWU_END message is received. 

  If the current ME Flash image is password protected, then that password is required to unlock
  the update process

  Because the process of downloading the image is time consuming, a callback is provided for each
  chunk of data sent to the ME FW Update client.  If desired, the caller can specify the number of steps
  that the total data send will have (e.g. 100 means a total of 100 callbacks for the entire duration). The
  number of steps supplied will be divided by the total result of (FileLength / Chunksize) + 1. 

Arguments:

  UINT8 pointer to FW Update Image
  UINT32 FW Update image length
  CHAR16 pointer to password (if no password then NULL)
  VOID pointer to callback function (if no callback then NULL)
  UINT32 callback steps (1 means each packet sent will trigger a callback)
  UINT32 pointer to ME FW Update Client return status

Returns:

  EFI_STATUS
  
--*/
;

EFI_STATUS
EndUpdate (
  IN  VOID    (*EndProgressProc)(),
  IN  UINT32  EndProgressSteps,
  OUT STATUS  *FWU_Status
)
/*++

Routine Description:

  Send the FWU_END message to the ME after a successful data download.
  This operation will request the ME to validate the image and update the
  flash.  Depending on flash speed, the operation may take serveral minutes
  to finish.  Be patient...

  Because the process of updating the image is time consuming, a callback is provided each delay (5 seconds)
  that occurs waiting for the ME FW Update client to respond.  If desired, the caller can specify a number of timeouts before
  a callback (e.g. 1 means send wait 5 seconds then invoke the callback routine).

Arguments:

  VOID pointer to callback function (if no callback then NULL)
  UINT32 callback ratio (1 means wait 5 seconds and then trigger a callback)
  UINT32 pointer to ME FW Update Client return status

Returns:

  EFI_STATUS
  
--*/
;

EFI_STATUS
GetFwFlashVersion(
  IN  FWVersion  *fwVersion
)
/*++

Routine Description:

  Get the FW version of the currently running ME FW

  NOTE: A connection to the client is required before this routine is 
  executed.

Arguments:

  FWVersion pointer to FW Flash version

Returns:

  EFI_STATUS

--*/
;