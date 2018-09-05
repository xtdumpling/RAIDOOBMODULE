//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2013, American Megatrends, Inc.        **//
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
// $Archive:  $
//
// $Author:  $
//
// $Revision:  $
//
// $Date:  $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		AmiPlatformToDriverAgent.h
//
// Description:	Header file for AmiPlatformConfig protocol.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef __AMI_PLATFORM_TO_DRIVER_AGENT_PROTOCOL__H__
#define __AMI_PLATFORM_TO_DRIVER_AGENT_PROTOCOL__H__

#ifdef __cplusplus
extern "C"
{
#endif

// {1DCFBACA-6ADA-4c0d-86ED-AF658BDFEC0C}
#define AMI_PLATFORM_TO_DRIVER_AGENT_PROTOCOL_GUID \
	{ 0x1dcfbaca, 0x6ada, 0x4c0d, { 0x86, 0xed, 0xaf, 0x65, 0x8b, 0xdf, 0xec, 0xc } }


extern EFI_GUID gAmiPlatformToDriverAgentProtocolGuid;

typedef struct _AMI_PLATFORM_TO_DRIVER_AGENT_PROTOCOL AMI_PLATFORM_TO_DRIVER_AGENT_PROTOCOL;

typedef EFI_STATUS
(EFIAPI *AMI_PLATFORM_TO_DRIVER_AGENT_SUPPORTED)(
    IN AMI_PLATFORM_TO_DRIVER_AGENT_PROTOCOL *This,
	IN EFI_HANDLE 							 ControllerHandle,
    IN EFI_HANDLE 							 ChildHandle OPTIONAL
);

typedef EFI_STATUS 
(EFIAPI *AMI_PLATFORM_TO_DRIVER_AGENT_QUERY) (
    IN AMI_PLATFORM_TO_DRIVER_AGENT_PROTOCOL *This,
    IN  EFI_HANDLE 							 ControllerHandle,
    IN  EFI_HANDLE 							 ChildHandle OPTIONAL,
    IN  UINTN      							 *Instance,
    OUT EFI_GUID   							 **ParameterTypeGuid,
    OUT VOID       							 **ParameterBlock,
    OUT UINTN      							 *ParameterBlockSize
);

typedef EFI_STATUS 
(EFIAPI *AMI_PLATFORM_TO_DRIVER_AGENT_RESPONSE) (
    IN AMI_PLATFORM_TO_DRIVER_AGENT_PROTOCOL *This,
    IN EFI_HANDLE                        	 ControllerHandle,
    IN EFI_HANDLE                        	 ChildHandle OPTIONAL,
    IN UINTN                             	 *Instance,
    IN EFI_GUID                          	 *ParameterTypeGuid,
    IN VOID                              	 *ParameterBlock,
    IN UINTN                             	 ParameterBlockSize,
    IN UINT32                                ConfigurationAction
);

struct _AMI_PLATFORM_TO_DRIVER_AGENT_PROTOCOL {
    AMI_PLATFORM_TO_DRIVER_AGENT_SUPPORTED Supported;
    AMI_PLATFORM_TO_DRIVER_AGENT_QUERY     Query;  	  	
	AMI_PLATFORM_TO_DRIVER_AGENT_RESPONSE  Response;
};


    /****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2012, American Megatrends, Inc.        **//
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
