//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 2016, American Megatrends, Inc.         	  **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include "RsaIScsiSupport.h"
#include <Protocol/IpmiUsbTransportProtocol.h>

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
  @param[out] EFI_INVALID_PARAMETER	If the command paramter invalid.

**/
EFI_STATUS ExecuteOemIpmiCommand(
	UINT8 NetFunction,UINT8 Lun,UINT8 Command,
	UINT8 *CommandData,UINT8 CommandDataSize,
	UINT8 **ResponseData,UINT8 *ResponseDataSize, UINT8 *CommandCompletionCode
	)
{
	EFI_GUID EfiIpmiGuid = gEfiDxeIpmiUsbTransportProtocolGuid;
	IPMI_USB_TRANSPORT  *gIpmiUsbTransport = NULL;
	EFI_STATUS	Status = EFI_SUCCESS;
	UINT8	*pTempResponse;
	UINT8	TempSize;
	
	DEBUG((DEBUG_INFO,"\n RSAISCSI: Inside ExecuteOemIpmiCommand \n"));
	if(ResponseDataSize == NULL)
		TempSize = 0;
	else
		TempSize = *ResponseDataSize;

	DEBUG((DEBUG_INFO,"\n RSAISCSI: Locate the IPMI USB Protocol \n"));
	// Locate the IPMI USB Protocol
	Status = gBS->LocateProtocol (&EfiIpmiGuid, NULL, &gIpmiUsbTransport);
	DEBUG((DEBUG_INFO,"\n RSAISCSI: gIpmiUsbTransport LocateProtocol Status :: %r",Status));
	if (EFI_SUCCESS == Status)
	{
		pTempResponse = (UINT8*)AllocatePool(TempSize);
		if (NULL==pTempResponse)
		{
			Status = EFI_BUFFER_TOO_SMALL;
		}
		else
		{
			Status = gIpmiUsbTransport->SendIpmiCommand(gIpmiUsbTransport, NetFunction, Lun, Command,
					CommandData, CommandDataSize, pTempResponse, &TempSize);
			*CommandCompletionCode = gIpmiUsbTransport->CommandCompletionCode;
			
			if (Status == EFI_BUFFER_TOO_SMALL)
			{
				pTempResponse = (UINT8*)AllocatePool(TempSize);
				if (pTempResponse == NULL)
				{
					Status = EFI_BUFFER_TOO_SMALL;
				}
				else
				{
					Status = gIpmiUsbTransport->SendIpmiCommand(gIpmiUsbTransport, NetFunction, Lun, Command,
						CommandData, CommandDataSize, pTempResponse, &TempSize);
					*CommandCompletionCode = gIpmiUsbTransport->CommandCompletionCode;
				}
			}

			if (Status == EFI_SUCCESS)
			{
				if (0 != TempSize){
					*ResponseDataSize = TempSize;
					*ResponseData = (UINT8*)AllocatePool(TempSize);
	
					if (NULL == *ResponseData)
					{
						Status = EFI_BUFFER_TOO_SMALL;
					}
					else
						CopyMem(*ResponseData,pTempResponse,*ResponseDataSize);
				}else{
					*ResponseDataSize=0;
				}
			}

			if(pTempResponse)
			{
				FreePool(pTempResponse);
			}
		}
	}

	return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 2016, American Megatrends, Inc.         	  **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
