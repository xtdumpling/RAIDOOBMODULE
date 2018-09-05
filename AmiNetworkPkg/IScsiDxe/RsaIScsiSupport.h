//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

#ifndef _RSA_ISCSI_SUPPORT_H_
#define _RSA_ISCSI_SUPPORT_H_

#include <AmiDxeLib.h>
//#include "IScsiDxe/IScsiConfig.h"
#include "IScsiDxe/IScsiImpl.h"


// MACRO definitions for Request and Response fields

// REQUEST fields for Set/Get ISCSI Field

#define RSA_FLD_BOOT_PRIORITY		1	// type - boolbyte
#define RSA_FLD_INITIATOR_NAME		2	// type - string
#define RSA_FLD_INITIATOR_DHCP		3	// type - boolbyte
#define RSA_FLD_EXPANSION_ROM_MENU	4	// type - boolbyte
#define RSA_FLD_CONNECT_WAIT_TIME	5	// type - int
#define RSA_FLD_INITIATOR_IP		6	// type - byte[4]
#define RSA_FLD_INITIATOR_NETMASK	7	// type - byte[4]
#define RSA_FLD_GATEWAY				8	// type - byte[4]
#define RSA_FLD_ISCSI_VLAN			9	// type - word
#define RSA_FLD_TARGET_NAME			10	// type - string
#define RSA_FLD_TARGET_DHCP			11	// type - boolbyte
#define RSA_FLD_TARGET_IP			12	// type - byte[4]
#define RSA_FLD_TARGET_PORT			13	// type - word
#define RSA_FLD_BOOT_LUN			14	// type - byte
#define RSA_FLD_AUTH_METHOD			15	// type - byte
#define RSA_FLD_CHAP_USERNAME		16	// type - string[32]
#define RSA_FLD_CHAP_SECRET			17	// type - string[32]
#define RSA_FLD_MUTUAL_CHAP_SECRET	18	// type - string[32]
#define RSA_FLD_BOOT_FLAGS			19	// type - byte[4]
#define RSA_FLD_NIC					20	// type - byte

#define RSA_ISCSI_DEFAULT_PREFIX_LEN	0x00

#define RSA_TOTAL_REQ_FIELD_COUNT	0x14
#define RSA_BEGIN_OFFSET			0x00

// Field Max Values
#define RSA_MIN_SECRET_LEN          0x0C    // 12 bytes
#define RSA_MAX_SECRET_LEN          0X10    // 16 bytes
#define RSA_MAX_CHAP_USER_NAME_LEN  0x20    // 32 bytes
#define RSA_INIT_NAME_MAX_LEN       0x71    // 113 bytes
#define RSA_TARGET_NAME_MAX_LEN     0x71    // 113 bytes

// RESPONSE fields for Set/Get ISCSI Field

#define RSA_RESP_NORMAL					0x00
#define RSA_RESP_INVALID_CONTEXT		0x80
#define RSA_RESP_REQ_DATA_LEN_INVALID	0xC7
#define RSA_RESP_PARA_OUT_OF_RANGE		0xC9
#define RSA_RESP_NODE_NOT_PRESENT		0xD3

#define RSA_ISCSI_NET_FN				0x34
#define RSA_ISCSI_BMC_LUN				0x00

#define RSA_COMPLETE_CODE_NORMAL		0x00

#define RSA_ISCSI_SET_COMMAND_NUMBER	0x02
#define RSA_ISCSI_GET_COMMAND_NUMBER	0x03

#define RSA_ISCSI_AUTH_NO_CHAP			0x01
#define RSA_ISCSI_AUTH_ONE_WAY_CHAP		0x02
#define RSA_ISCSI_AUTH_MUTUAL_CHAP		0x03

// Get command Response fields.
#define RSA_RESP_INDEX_MAX_BYTES		0x00	// The maximum bytes possible for the requested field.
#define RSA_RESP_INDEX_RECEIVED_BYTES	0x01	// The actual number of bytes returned for the requested field.
#define RSA_RESP_INDEX_DATA_START		0x02	// The Actual data returned from BMC.

#define RSA_BMC_SUPPORTED_ATTEMPT_COUNT	0x01	// NUmber of attempts that BMC can hold now.
#define RSA_ISCSI_ATTEMPT_NAME "RSAIScsiAttempt"

/**
  This function to get initiator name from BMC is valid or not.
  
  @param[in]  RSAInitiatorName		String to Hold Initiator name of max length 256 ().
  
  @param[out] TRUE		If RSAInitiatorName is valid.
  	  	  	  FALSE		If RSAInitiatorName is invalid.

**/
EFI_STATUS
EFIAPI
RSAInitializeBMCInformation();

/**
  This function determines whether there is any attempt data available in BMC.
  
  @param[in]	VOID
  
  @param[out]	TRUE if BMC has Attempt's related data.
				FALSE if BMC doesn't have any Attempt's related data.


**/
BOOLEAN
EFIAPI
RSAIsBMCDataAvailable();

/**
  This function to get initiator name from BMC.
  
  @param[in]  RSAInitiatorName		String to Hold Initiator name of max length 256 (ISCSI_NAME_MAX_SIZE).
  
  @param[out] EFI_SUCCESS		If RSAInitiatorName is valid.

**/
EFI_STATUS
EFIAPI
RSAGetIScsiInitiatorName (
  IN CHAR8 *RSAInitiatorName
  );
  
/**
  This function to get ISCSI AttemptConfig order and size from BMC
  
  @param[in out]  AttemptConfigOrder		Attempt config order array.
  @param[in out]  RSAInitiatorName		Length of Attempt Config Order array.
  
  @param[out] EFI_SUCCESS		If RsaAttemptConfigData is valid.

**/
EFI_STATUS
EFIAPI
RSAGetIScsiAttemptConfigOrder (
  IN OUT UINT8 **AttemptConfigOrder,
  IN OUT UINT32 *AttemptConfigOrderSize
  );  


/**
  This function updates the temporary AttemptConfigData Cache in RSA Support Module.
  
  @param[in]  RsaAttemptConfigData	Attempt data to be udpated to the RSA Module Cache.
  
  @param[out] EFI_SUCCESS		If RSA is initialized and update success is valid.

**/
EFI_STATUS
EFIAPI
RSAGetBMCIScsiAttemptData (
  IN CHAR16 *PortString,
  IN OUT ISCSI_ATTEMPT_CONFIG_NVDATA **RsaAttemptConfigData
  );

/**
  Function to update the BMC cached attempt data for intermediate data save
  
  @param[in]  RsaAttemptConfigData		Attempt data to update.
  
  @param[out] EFI_SUCCESS		If update went success

**/
EFI_STATUS
EFIAPI
RSAUpdateBMCIScsiAttemptDataCache (
  IN CHAR16 *PortString,      
  IN OUT ISCSI_ATTEMPT_CONFIG_NVDATA *RsaAttemptConfigData
  );

/**
  Function to update the BMC cached attempt data for intermediate data save
  
  @param[in]  NetFunction		NetFUnction of the BMC.
  @param[in]  Lun				BMC LUN.
  @param[in]  Command			Command to be executed.
  @param[in]  CommandDataSize	Command data size.
  @param[in]  CommandData		Command data to be passed.
  @param[in]  ResponseData		Response data from BMC.
  @param[in]  ResponseDataSize	Response data size from BMC.
  
  @param[out] EFI_SUCCESS			If the command execution is success.
  @param[out] DIAG_BUFFER_TOO_SMALL	If memory not able to allocate.
  @param[out] EFI_DEVICE_ERROR		If the command not able to send.
  @param[out] EFI_INVALID_PARAMETER	If the command parameter invalid.

**/
EFI_STATUS ExecuteOemIpmiCommand(
	UINT8 NetFunction,UINT8 Lun,UINT8 Command,
	UINT8 *CommandData,UINT8 CommandDataSize,
	UINT8 **ResponseData,UINT8 *ResponseDataSize, UINT8 *CommandCompletionCode
	);
#endif //_RSA_ISCSI_SUPPORT_H_
