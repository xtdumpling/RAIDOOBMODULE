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

#include "RsaIScsiSupport.h"

static BOOLEAN gRSAInitDone = FALSE;
static BOOLEAN gRsaDataAvailable = FALSE;
static ISCSI_ATTEMPT_CONFIG_NVDATA gRSAAttemptConfigData = {0};
static CHAR8 gRSAISCSIInitiatorName[ISCSI_NAME_MAX_SIZE] = {0};

#define DUMMY_DATA_SIMULATION 0

#if DUMMY_DATA_SIMULATION
#define DUMMY_RESPONSE_FLD1 {0x01, 0x01, 0x01} // Boolean 1 primary
#define DUMMY_RESPONSE_FLD2 {0x71, 0x25, 0x69, 0x71, 0x6E, 0x2E, 0x32, 0x30, 0x31, 0x32, 0x2D, 0x30, 0x35, 0x2E, 0x6C, 0x6F, 0x63, 0x61, 0x6C, 0x2E, 0x6D, 0x79, 0x6E, 0x65, 0x74, 0x3A, 0x73, 0x74, 0x6F, 0x72, 0x61, 0x67, 0x65, 0x2E, 0x73, 0x79, 0x73, 0x31} //iqn.2012-05.local.mynet:storage.sys1
#define DUMMY_RESPONSE_FLD3 {0x01, 0x01, 0x00} // Initiator DHCP
#define DUMMY_RESPONSE_FLD4 {0x01, 0x01, 0x00} // Expansion ROM unused
#define DUMMY_RESPONSE_FLD5 {0x08, 0x02, 0x00, 0x01} // Connection WaitTime 1000
#define DUMMY_RESPONSE_FLD6 {0x04, 0x04, 0xc0, 0xa8, 0xb8, 0x0a} // Initiator IP
#define DUMMY_RESPONSE_FLD7 {0x04, 0x04, 0xff, 0xff, 0xff, 0x00} // Initiator Subnet
#define DUMMY_RESPONSE_FLD8 {0x04, 0x04, 0xc0, 0xa8, 0xb8, 0x01} // Initiator Gateway
#define DUMMY_RESPONSE_FLD9 {0x02, 0x01, 0x00} // Iscsi VLAN - unused now
#define DUMMY_RESPONSE_FLD10 {0x71, 0x25, 0x69, 0x71, 0x6E, 0x2E, 0x32, 0x30, 0x31, 0x32, 0x2D, 0x30, 0x35, 0x2E, 0x6C, 0x6F, 0x63, 0x61, 0x6C, 0x2E, 0x6D, 0x79, 0x6E, 0x65, 0x74, 0x3A, 0x73, 0x74, 0x6F, 0x72, 0x61, 0x67, 0x65, 0x2E, 0x73, 0x79, 0x73, 0x30} //iqn.2012-05.local.mynet:storage.sys0
#define DUMMY_RESPONSE_FLD11 {0x01, 0x01, 0x00} // Target DHCP
#define DUMMY_RESPONSE_FLD12 {0x04, 0x04, 0xc0, 0xa8, 0xb8, 0x01} // Target IP
#define DUMMY_RESPONSE_FLD13 {0x02, 0x02, 0xbc, 0x0c} // Target Port
#define DUMMY_RESPONSE_FLD14 {0x01, 0x01, 0x00} // Target Boot LUN
#define DUMMY_RESPONSE_FLD15 {0x01, 0x01, 0x01} // Auth Type - Made none
#define DUMMY_RESPONSE_FLD16 {0x71, 0x05, 0x63, 0x68, 0x61, 0x70, 0x31} // CHAP Name
#define DUMMY_RESPONSE_FLD17 {0x71, 0x08, 0x63, 0x68, 0x61, 0x70, 0x31, 0x32, 0x33, 0x34} // CHAP Secret - chap1234
#define DUMMY_RESPONSE_FLD18 {0x71, 0x08, 0x63, 0x68, 0x61, 0x70, 0x31, 0x32, 0x33, 0x34} // REV CHAP Secret - chap1234
#define DUMMY_RESPONSE_FLD19 {0x04, 0x04, 0x00, 0x00, 0x00, 0x00} // Boot Flags
#define DUMMY_RESPONSE_FLD20 {0x01, 0x01, 0x01} // NIC Index - 1

UINT8 DummyResponse[][0x100] = {
		DUMMY_RESPONSE_FLD1,
		DUMMY_RESPONSE_FLD2,
		DUMMY_RESPONSE_FLD3,
		DUMMY_RESPONSE_FLD4,
		DUMMY_RESPONSE_FLD5,
		DUMMY_RESPONSE_FLD6,
		DUMMY_RESPONSE_FLD7,
		DUMMY_RESPONSE_FLD8,
		DUMMY_RESPONSE_FLD9,
		DUMMY_RESPONSE_FLD10,
		DUMMY_RESPONSE_FLD11,
		DUMMY_RESPONSE_FLD12,
		DUMMY_RESPONSE_FLD13,
		DUMMY_RESPONSE_FLD14,
		DUMMY_RESPONSE_FLD15,
		DUMMY_RESPONSE_FLD16,
		DUMMY_RESPONSE_FLD17,
		DUMMY_RESPONSE_FLD18,
		DUMMY_RESPONSE_FLD19,
		DUMMY_RESPONSE_FLD20
};
#endif

EFI_STATUS
EFIAPI
RSAReadBMCAttemptData ();

EFI_STATUS
EFIAPI
RSAMapBMCAttemptData (
  IN UINT8 FieldValue,
  IN UINT8 *ResponseData,
  IN UINT8 ResponseDataSize
  );

/**
  This function populates the data obtained after issuing OemIpmiCommand.
  
  @param[in]  FieldValue			Field value of the command issued.
  @param[in]  ResponseData			Response obtained from BMC command.
  @param[in]  ResponseDataSize		Size of the response data.
  
  @param[out] EFI_SUCCESS		If Response data is copied to the gRSAAttemptConfigData successfully..

**/
EFI_STATUS
EFIAPI
RSAMapBMCAttemptData (
  IN UINT8 FieldValue,
  IN UINT8 *ResponseData,
  IN UINT8 ResponseDataSize
  )
{
	EFI_STATUS	Status =  EFI_SUCCESS;
	CHAR16	TempString16[ISCSI_NAME_MAX_SIZE] = {0};
	UINT8	ChapType = 0;
	
	if(ResponseDataSize == 0)
	    return Status;
	
	DEBUG((DEBUG_INFO,"\n RSAISCSI: Inside RSAMapBMCAttemptData, Field#%d",FieldValue));
//    {
//    	CHAR16 TempStr16[ISCSI_NAME_MAX_SIZE] = {0};
//    	AsciiStrToUnicodeStr(gRSAISCSIInitiatorName,TempStr16);
//    	DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_INITIATOR_NAME :: %s Length :: %d \n",TempStr16,StrLen(TempStr16)));
//    }
	
	
	switch(FieldValue)
	{
		case RSA_FLD_BOOT_PRIORITY: // Field#1
		    // Copy the value (UINT8)(ResponseData[RSA_RESP_INDEX_DATA_START]) to the required variable.
			// Do nothing as of now.
			break;
			
		case RSA_FLD_INITIATOR_NAME: // Field#2
			CopyMem(gRSAISCSIInitiatorName,&ResponseData[RSA_RESP_INDEX_DATA_START],ResponseData[RSA_RESP_INDEX_RECEIVED_BYTES]);
			gRSAISCSIInitiatorName[ResponseData[RSA_RESP_INDEX_RECEIVED_BYTES]] = '\0';
			
			AsciiStrToUnicodeStr(gRSAISCSIInitiatorName,TempString16);
			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_INITIATOR_NAME :: %s \n",TempString16));
			break;
			
		case RSA_FLD_INITIATOR_DHCP: // Field#3
			gRSAAttemptConfigData.SessionConfigData.InitiatorInfoFromDhcp = (BOOLEAN)(ResponseData[RSA_RESP_INDEX_DATA_START]);
			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_INITIATOR_DHCP :: 0x%X \n",gRSAAttemptConfigData.SessionConfigData.InitiatorInfoFromDhcp));
			break;
			
		case RSA_FLD_EXPANSION_ROM_MENU: // Field#4
		    // Copy the value (BOOLEAN)(ResponseData[RSA_RESP_INDEX_DATA_START]) to the required variable.
			// Do nothing as of now.
			break;
			
		case RSA_FLD_CONNECT_WAIT_TIME: // Field#5
		    // What we copy here is in seconds.
		    gRSAAttemptConfigData.SessionConfigData.ConnectTimeout = (UINT16)*((INT32*)&ResponseData[RSA_RESP_INDEX_DATA_START]);
		    DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_CONNECT_WAIT_TIME Before Conversion :: %d \n",gRSAAttemptConfigData.SessionConfigData.ConnectTimeout));
		    
		    // convert it to milliseconds
		    gRSAAttemptConfigData.SessionConfigData.ConnectTimeout *= 1000;
		
			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_CONNECT_WAIT_TIME After Conversion:: %d \n",gRSAAttemptConfigData.SessionConfigData.ConnectTimeout));
			break;
			
		case RSA_FLD_INITIATOR_IP: // Field#6
			CopyMem(&gRSAAttemptConfigData.SessionConfigData.LocalIp,&ResponseData[RSA_RESP_INDEX_DATA_START],sizeof(EFI_IPv4_ADDRESS));
			
			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_INITIATOR_IP"));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.LocalIp[0] :: 0x%X",gRSAAttemptConfigData.SessionConfigData.LocalIp.v4.Addr[0]));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.LocalIp[1] :: 0x%X",gRSAAttemptConfigData.SessionConfigData.LocalIp.v4.Addr[1]));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.LocalIp[2] :: 0x%X",gRSAAttemptConfigData.SessionConfigData.LocalIp.v4.Addr[2]));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.LocalIp[3] :: 0x%X\n",gRSAAttemptConfigData.SessionConfigData.LocalIp.v4.Addr[3]));
			break;
			
		case RSA_FLD_INITIATOR_NETMASK: // Field#7
			CopyMem(&gRSAAttemptConfigData.SessionConfigData.SubnetMask,&ResponseData[RSA_RESP_INDEX_DATA_START],sizeof(EFI_IPv4_ADDRESS));
			
			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_INITIATOR_NETMASK"));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.SubnetMask[0] :: 0x%X",gRSAAttemptConfigData.SessionConfigData.SubnetMask.Addr[0]));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.SubnetMask[1] :: 0x%X",gRSAAttemptConfigData.SessionConfigData.SubnetMask.Addr[1]));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.SubnetMask[2] :: 0x%X",gRSAAttemptConfigData.SessionConfigData.SubnetMask.Addr[2]));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.SubnetMask[3] :: 0x%X\n",gRSAAttemptConfigData.SessionConfigData.SubnetMask.Addr[3]));
			break;
			
		case RSA_FLD_GATEWAY: // Field#8
			CopyMem(&gRSAAttemptConfigData.SessionConfigData.Gateway,&ResponseData[RSA_RESP_INDEX_DATA_START],sizeof(EFI_IPv4_ADDRESS));
			
			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_GATEWAY"));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.Gateway[0] :: 0x%X",gRSAAttemptConfigData.SessionConfigData.Gateway.v4.Addr[0]));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.Gateway[1] :: 0x%X",gRSAAttemptConfigData.SessionConfigData.Gateway.v4.Addr[1]));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.Gateway[2] :: 0x%X",gRSAAttemptConfigData.SessionConfigData.Gateway.v4.Addr[2]));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.Gateway[3] :: 0x%X \n",gRSAAttemptConfigData.SessionConfigData.Gateway.v4.Addr[3]));
			break;
			
		case RSA_FLD_ISCSI_VLAN: // Field#9
			// Do nothing as of now.
			break;
			
		case RSA_FLD_TARGET_NAME: // Field#10
			CopyMem(gRSAAttemptConfigData.SessionConfigData.TargetName,&ResponseData[RSA_RESP_INDEX_DATA_START],ResponseData[RSA_RESP_INDEX_RECEIVED_BYTES]);
			gRSAAttemptConfigData.SessionConfigData.TargetName[ResponseData[RSA_RESP_INDEX_DATA_START]] = '\0';
			
			AsciiStrToUnicodeStr(gRSAAttemptConfigData.SessionConfigData.TargetName,TempString16);
			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_TARGET_NAME :: %s \n",TempString16));
			break;
			
		case RSA_FLD_TARGET_DHCP: // Field#11
			gRSAAttemptConfigData.SessionConfigData.TargetInfoFromDhcp = (BOOLEAN)(ResponseData[RSA_RESP_INDEX_DATA_START]);
			
			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_TARGET_DHCP :: 0x%X \n",gRSAAttemptConfigData.SessionConfigData.TargetInfoFromDhcp));
			break;
			
		case RSA_FLD_TARGET_IP: // Field#12
			CopyMem(&gRSAAttemptConfigData.SessionConfigData.TargetIp,&ResponseData[RSA_RESP_INDEX_DATA_START],sizeof(EFI_IPv4_ADDRESS));
			
			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_TARGET_IP"));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.TargetIp[0] :: 0x%X",gRSAAttemptConfigData.SessionConfigData.TargetIp.v4.Addr[0]));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.TargetIp[1] :: 0x%X",gRSAAttemptConfigData.SessionConfigData.TargetIp.v4.Addr[1]));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.TargetIp[2] :: 0x%X",gRSAAttemptConfigData.SessionConfigData.TargetIp.v4.Addr[2]));
			DEBUG((DEBUG_INFO,"\n gRSAAttemptConfigData.SessionConfigData.TargetIp[3] :: 0x%X\n",gRSAAttemptConfigData.SessionConfigData.TargetIp.v4.Addr[3]));
			break;
		case RSA_FLD_TARGET_PORT: // Field#13
		    gRSAAttemptConfigData.SessionConfigData.TargetPort = (UINT16)*((UINT16*)&ResponseData[RSA_RESP_INDEX_DATA_START]);

			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_TARGET_PORT :: %d \n",gRSAAttemptConfigData.SessionConfigData.TargetPort));
			break;
			
		case RSA_FLD_BOOT_LUN: // Field#14
		    ZeroMem(&gRSAAttemptConfigData.SessionConfigData.BootLun,sizeof(gRSAAttemptConfigData.SessionConfigData.BootLun));
//			CopyMem(&gRSAAttemptConfigData.SessionConfigData.BootLun,&ResponseData[RSA_RESP_INDEX_DATA_START],ResponseData[RSA_RESP_INDEX_RECEIVED_BYTES]);
			gRSAAttemptConfigData.SessionConfigData.BootLun[0] = (UINT8)(ResponseData[RSA_RESP_INDEX_DATA_START]);
			
			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_BOOT_LUN :: 0x%X \n",gRSAAttemptConfigData.SessionConfigData.BootLun));
			break;
			
		case RSA_FLD_AUTH_METHOD: // Field#15
			// In BMC side 0 - No Chap 1 - Oneway Chap and 2 - Mutual Chap
			// In iSCSI side 0 - Oneway Chap  1 - Mutual Chap
			ChapType = (UINT8)(ResponseData[RSA_RESP_INDEX_DATA_START]);
			
			DEBUG((DEBUG_INFO,"\n RSAISCSI: ChapType :: 0x%X \n",ChapType));
			
			if(ChapType == RSA_ISCSI_AUTH_ONE_WAY_CHAP)
			{
				gRSAAttemptConfigData.AuthenticationType = ISCSI_AUTH_TYPE_CHAP;
				gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPType = ISCSI_CHAP_UNI;
			}
			else if(ChapType == RSA_ISCSI_AUTH_MUTUAL_CHAP)
			{
				gRSAAttemptConfigData.AuthenticationType = ISCSI_AUTH_TYPE_CHAP;
				gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPType = ISCSI_CHAP_MUTUAL;
			}
			else
			{
				gRSAAttemptConfigData.AuthenticationType = ISCSI_AUTH_TYPE_NONE;
				gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPType = 2;
			}
			
			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_AUTH_METHOD :: 0x%X \n",gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPType));
			break;
			
		case RSA_FLD_CHAP_USERNAME: // Field#16
			
			if(gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPType == ISCSI_CHAP_UNI){
				CopyMem(gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPName,&ResponseData[RSA_RESP_INDEX_DATA_START],ResponseData[RSA_RESP_INDEX_RECEIVED_BYTES]);
				gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPName[ResponseData[RSA_RESP_INDEX_DATA_START]] = '\0';
				
				AsciiStrToUnicodeStr(gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPName,TempString16);
				DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_CHAP_USERNAME :: %s \n",TempString16));
			}
			
			if(gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPType == ISCSI_CHAP_MUTUAL)
			{
				CopyMem(gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPName,&ResponseData[RSA_RESP_INDEX_DATA_START],ResponseData[RSA_RESP_INDEX_RECEIVED_BYTES]);
				gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPName[ResponseData[RSA_RESP_INDEX_DATA_START]] = '\0';
				AsciiStrCpy(gRSAAttemptConfigData.AuthConfigData.CHAP.ReverseCHAPName,gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPName);
				
				AsciiStrToUnicodeStr(gRSAAttemptConfigData.AuthConfigData.CHAP.ReverseCHAPName,TempString16);
				DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_REVCHAP_USERNAME :: %s \n",TempString16));
			}
			break;
			
		case RSA_FLD_CHAP_SECRET: // Field#17
			if(gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPType == ISCSI_CHAP_UNI || 
					gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPType == ISCSI_CHAP_MUTUAL)
			{
				CopyMem(gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPSecret,&ResponseData[RSA_RESP_INDEX_DATA_START],ResponseData[RSA_RESP_INDEX_RECEIVED_BYTES]);
				gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPSecret[ResponseData[RSA_RESP_INDEX_DATA_START]] = '\0';
				
				AsciiStrToUnicodeStr(gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPSecret,TempString16);
				DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_CHAP_SECRET :: %s \n",TempString16));
			}
			break;
			
		case RSA_FLD_MUTUAL_CHAP_SECRET: // Field#18
			if(gRSAAttemptConfigData.AuthConfigData.CHAP.CHAPType == ISCSI_CHAP_MUTUAL)
			{
				CopyMem(gRSAAttemptConfigData.AuthConfigData.CHAP.ReverseCHAPSecret,&ResponseData[RSA_RESP_INDEX_DATA_START],ResponseData[RSA_RESP_INDEX_RECEIVED_BYTES]);		
				gRSAAttemptConfigData.AuthConfigData.CHAP.ReverseCHAPSecret[ResponseData[RSA_RESP_INDEX_DATA_START]] = '\0';
				
				AsciiStrToUnicodeStr(gRSAAttemptConfigData.AuthConfigData.CHAP.ReverseCHAPSecret,TempString16);
				DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_MUTUAL_CHAP_SECRET :: %s \n",TempString16));
			}
			break;
			
		case RSA_FLD_BOOT_FLAGS: // Field#19
			// Do nothing as of now.
			break;
			
		case RSA_FLD_NIC: // Field#20
			gRSAAttemptConfigData.NicIndex = (UINT8)(ResponseData[RSA_RESP_INDEX_DATA_START]);
			DEBUG((DEBUG_INFO,"\n RSAISCSI: RSA_FLD_NIC :: 0x%X \n",gRSAAttemptConfigData.NicIndex));
			//gAllFieldsRead = TRUE;
			break;
			
		default:	// It should not come here.
			break;
	}
	return Status;
}

/**
  This function to get initiator name from BMC is valid or not.
  
  @param[in]  VOID			
  
  @param[out] EFI_SUCCESS	If data is read from BMC successfully and gRSAAttemptConfigData is initialized.
**/
EFI_STATUS
EFIAPI
RSAInitializeBMCInformation()
{
    // Add initialization code here
    // Populate AttemptConfigData and initiator name using BMC IPMI Commands defined in RSA
    // Fill the default values
    // Fill Additional Needed values Eg:
    // Set the gRSAInitDone and gRsaDataAvailable in accordance.
	EFI_STATUS Status = EFI_NOT_READY;
	ISCSI_NIC_INFO  *NicInfo = (ISCSI_NIC_INFO*)NULL;
	CHAR16          MacString[ISCSI_MAX_MAC_STRING_LEN] = {0};
		
	ZeroMem(&gRSAAttemptConfigData,sizeof(ISCSI_ATTEMPT_CONFIG_NVDATA));
	DEBUG((DEBUG_INFO,"\n RSAISCSI: Inside RSAInitializeBMCInformation \n"));
	if(!gRSAInitDone)
	{
		Status = RSAReadBMCAttemptData();
		DEBUG((DEBUG_INFO,"\n RSAISCSI: RSAReadBMCAttemptData Status :: %r \n",Status));
        if(EFI_ERROR(Status))
        {
            gRsaDataAvailable = FALSE;
        }
        else
        {    
            // Once we update NicIndex (last possible field), assign all other default values.
            gRSAAttemptConfigData.AttemptConfigIndex = 1;	                        // It will always be 1
            AsciiStrCpy(gRSAAttemptConfigData.AttemptName,RSA_ISCSI_ATTEMPT_NAME);  // Default Name
            gRSAAttemptConfigData.AutoConfigureSuccess = FALSE;	                    // Doesn't support IPv6
            gRSAAttemptConfigData.SessionConfigData.Enabled = TRUE;                 // Always treat as enabled
            gRSAAttemptConfigData.SessionConfigData.IpMode = IP_MODE_IP4;	        // Use IPv4 mode always.
            gRSAAttemptConfigData.SessionConfigData.ConnectRetryCount = CONNECT_MIN_RETRY;	        // Use default retry count.
            gRSAAttemptConfigData.SessionConfigData.PrefixLength = RSA_ISCSI_DEFAULT_PREFIX_LEN;
            
            // Update MACString here
            // **# What if invalid nic index, info not available. Be careful here
            
            NicInfo = IScsiGetNicInfoByIndex (gRSAAttemptConfigData.NicIndex);
            DEBUG((DEBUG_INFO,"\n RSAISCSI: gRSAAttemptConfigData.NicIndex :: 0x%X \n",gRSAAttemptConfigData.NicIndex));
            if(NicInfo)
            {
                IScsiMacAddrToStr (&NicInfo->PermanentAddress, NicInfo->HwAddressSize, NicInfo->VlanId, MacString);
                UnicodeStrToAsciiStr(MacString, gRSAAttemptConfigData.MacString);
                DEBUG((DEBUG_INFO,"\n RSAISCSI: MacString :: %s \n",MacString));
                
                CopyMem (gRSAAttemptConfigData.SessionConfigData.IsId, &NicInfo->PermanentAddress, 6);
                gRSAAttemptConfigData.SessionConfigData.IsId[0] = (UINT8) (gRSAAttemptConfigData.SessionConfigData.IsId[0] & 0x3F);
                // Set the gRSAInitDone and gRsaDataAvailable if all the fields are read.       
                gRsaDataAvailable = TRUE;
                Status = EFI_SUCCESS;
            }
            
            gRSAInitDone = TRUE;
        }
	}
	
	if(gRsaDataAvailable)
	    return EFI_SUCCESS;
    
	return Status;
}

/**
  This function to get initiator name from BMC.
  
  @param[in]  RSAInitiatorName      String to Hold Initiator name of max length 256 (ISCSI_NAME_MAX_SIZE).
  
  @param[out] EFI_SUCCESS       If RSAInitiatorName is valid.

**/
EFI_STATUS
EFIAPI
RSAGetIScsiInitiatorName (
  IN OUT CHAR8 *RSAInitiatorName
  )
{
	if(!RSAInitiatorName)
		return EFI_INVALID_PARAMETER;
	
	// Check gRSAInitDone and copy the Initiator Name.
	if(!gRSAInitDone)
		return EFI_NOT_READY;

	// Copy Initiator name string.
	if(gRSAISCSIInitiatorName)
		AsciiStrCpy(RSAInitiatorName, gRSAISCSIInitiatorName);
    {
    	CHAR16 TempStr16[ISCSI_NAME_MAX_SIZE] = {0};
    	AsciiStrToUnicodeStr(gRSAISCSIInitiatorName,TempStr16);
    	DEBUG((DEBUG_INFO,"\n RSAISCSI: Inside RSAGetIScsiInitiatorName, gRSAISCSIInitiatorName :: %s Length :: %d \n",TempStr16,StrLen(TempStr16)));
    }

    return EFI_SUCCESS;
}

/**
  This function validates whether the data obtained from BMC is valid or not.
  
  @param[in]  PortString                PortString for Attempt.
  @param[in]  RsaAttemptConfigData		Attempt data read from BMC.
  
  @param[out] TRUE		If RsaAttemptConfigData is valid.
  	  	  	  FALSE		If RsaAttemptConfigData is invalid.

**/
EFI_STATUS
EFIAPI
RSAGetBMCIScsiAttemptData (
  IN CHAR16 *PortString,
  IN OUT ISCSI_ATTEMPT_CONFIG_NVDATA **RsaAttemptConfigData
  )
{
	EFI_STATUS		Status = EFI_INVALID_PARAMETER;
	ISCSI_NIC_INFO	*NicInfo = (ISCSI_NIC_INFO*)NULL;
	CHAR16			ComparePortString[ISCSI_NAME_IFR_MAX_SIZE];
	CHAR16          MacString[ISCSI_MAX_MAC_STRING_LEN] = {0};
	
	DEBUG((DEBUG_INFO,"\n RSAISCSI: Inside RSAGetBMCIScsiAttemptData, PortString :: %s \n",PortString));
	if(RsaAttemptConfigData == (ISCSI_ATTEMPT_CONFIG_NVDATA **)NULL)
		return EFI_INVALID_PARAMETER;
	
	if(!(gRSAInitDone && gRsaDataAvailable))
		return EFI_NOT_READY;
	
	*RsaAttemptConfigData = AllocateZeroPool (sizeof (ISCSI_ATTEMPT_CONFIG_NVDATA));
    if (*RsaAttemptConfigData == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }
    
	// Validate the input PortString match for available stored NicIndex
	// Populate the input array with the gRSAAttemptConfigData if gRSAInitDone and gRsaDataAvailable are set
    AsciiStrToUnicodeStr(gRSAAttemptConfigData.MacString, MacString);
    UnicodeSPrint (
            ComparePortString,
            (UINTN) 128,
            L"%s%d",
            MacString,
            (UINTN) gRSAAttemptConfigData.AttemptConfigIndex
            );
    if(ComparePortString)
    {
        if(!StrCmp(PortString,ComparePortString))
        {
            **RsaAttemptConfigData = gRSAAttemptConfigData;
            Status = EFI_SUCCESS;
        }
    }
	
	return Status;		
}

/**
  This function determines whether there is any attempt data available in BMC.
  
  @param[in]  VOID
  
  @param[out]  BOOLEAN		Value of gRsaDataAvailable.

**/
BOOLEAN
EFIAPI
RSAIsBMCDataAvailable()
{
    // Initialize BMC information if first call.
    if(!gRSAInitDone)
    {
        RSAInitializeBMCInformation();
    }    
	
    // return the BMC information availability status.
	return gRsaDataAvailable;
}

/**
  This function to get ISCSI AttemptConfig order and size from BMC.
  
  @param[in out]  AttemptConfigOrder		Attempt config order array.
  @param[in out]  RSAInitiatorName			Length of Attempt Config Order array.
  
  @param[out] EFI_SUCCESS		If RsaAttemptConfigData is valid.

**/
EFI_STATUS
EFIAPI
RSAGetIScsiAttemptConfigOrder (
  IN OUT UINT8 **AttemptConfigOrder,
  IN OUT UINT32 *AttemptConfigOrderSize
  )
{
	if((!AttemptConfigOrder) || (!AttemptConfigOrderSize))
	        return EFI_INVALID_PARAMETER;
	        
	if(!(gRSAInitDone && gRsaDataAvailable))
	        return EFI_NOT_READY;
	
	*AttemptConfigOrder = AllocateZeroPool (sizeof(UINT8));
	if(!(*AttemptConfigOrder))
		return EFI_OUT_OF_RESOURCES;	
	
	(*AttemptConfigOrder)[0] = 1; //BMC support only single attempt
	
	// Since BMC will have a single attempt's data.
	*AttemptConfigOrderSize = RSA_BMC_SUPPORTED_ATTEMPT_COUNT;
    return EFI_SUCCESS;
}

/**
  Function to update the BMC cached attempt data for intermediate data save
  
  @param[in]  PortString                PortString for Attempt.
  @param[in]  RsaAttemptConfigData		Attempt data to update.
  
  @param[out] EFI_SUCCESS				If update went success.
  @param[out] EFI_INVALID_PARAMETER		If RsaAttemptConfigData is NULL.
  @param[out] EFI_NOT_READY				If BMC data is not initialized.

**/
EFI_STATUS
EFIAPI
RSAUpdateBMCIScsiAttemptDataCache (
  IN CHAR16 *PortString,
  IN OUT ISCSI_ATTEMPT_CONFIG_NVDATA *RsaAttemptConfigData
  )
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    CHAR16      ComparePortString[ISCSI_NAME_IFR_MAX_SIZE] = {0};
    CHAR16      MacString[ISCSI_MAX_MAC_STRING_LEN] = {0};
        
    if(RsaAttemptConfigData == (ISCSI_ATTEMPT_CONFIG_NVDATA *)NULL)
        return EFI_INVALID_PARAMETER;
        
    if(!(gRSAInitDone && gRsaDataAvailable))
        return EFI_NOT_READY;
    
    // Validate the input PortString match for available stored NicIndex
    // Populate the input array with the gRSAAttemptConfigData if gRSAInitDone and gRsaDataAvailable are set
    AsciiStrToUnicodeStr(gRSAAttemptConfigData.MacString, MacString);
    UnicodeSPrint (
            ComparePortString,
            (UINTN) 128,
            L"%s%d",
            MacString,
            (UINTN) gRSAAttemptConfigData.AttemptConfigIndex
            );
    if(ComparePortString)
    {
        if(!StrCmp(PortString,ComparePortString))
        {
            gRSAAttemptConfigData = *RsaAttemptConfigData;
            Status = EFI_SUCCESS;
        }
    }
        
    return Status;
}

/**
  This function validates whether the data obtained from BMC is valid or not.
  
  @param[in]  RsaAttemptConfigData		Attempt data read from BMC.
  
  @param[out] TRUE		If RsaAttemptConfigData is valid.
  	  	  	  FALSE		If RsaAttemptConfigData is invalid.

**/
EFI_STATUS
EFIAPI
RSAReadBMCAttemptData()
{
	EFI_STATUS Status = EFI_NOT_FOUND;
	UINT8	Field = 0;
	UINT8 *BmcResponseData = NULL;
	UINT8 BmcResponseDataSize = 0;
	UINT8 CommandCompletionCode = 0;
	
	DEBUG((DEBUG_INFO,"\n RSAISCSI: Inside RSAReadBMCAttemptData \n"));
	for(Field = 1; Field <= RSA_TOTAL_REQ_FIELD_COUNT; Field++)
	{
		// ExecuteOemIpmiCommand() Arguments
		// 1. NetFunction
		// 2. Lun
		// 3. Command Number
		// 4. Field
		// 5. ResponseData
		// 6. ResponseDataSize
		// 7. CommandCompletionCode
#if(DUMMY_DATA_SIMULATION == 0)
		// Issue the BMC command.
		Status = ExecuteOemIpmiCommand(RSA_ISCSI_NET_FN,RSA_ISCSI_BMC_LUN,RSA_ISCSI_GET_COMMAND_NUMBER,&Field,1,&BmcResponseData,&BmcResponseDataSize,&CommandCompletionCode);
		DEBUG((DEBUG_INFO,"\n RSAISCSI: CommandCompletionCode for Field#%d :: 0x%X",Field,CommandCompletionCode));
		if( !EFI_ERROR(Status) && (CommandCompletionCode == RSA_COMPLETE_CODE_NORMAL) )
			Status = RSAMapBMCAttemptData(Field,BmcResponseData,BmcResponseDataSize);
#else		
		UINT8* DummyResponseCopy = NULL;
		DummyResponseCopy=DummyResponse[Field-1];
		Status = RSAMapBMCAttemptData(Field,DummyResponseCopy,BmcResponseDataSize);
#endif	// DUMMY_DATA_SIMULATION
		
		if(EFI_ERROR(Status))
		{
		    Status = EFI_NOT_FOUND;
		    break;
		}
	}
	return Status;
}

