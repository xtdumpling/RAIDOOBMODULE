//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     May/16/2016
//**************************************************************************//
#include <Token.h>
#if IPMI_SUPPORT
#include <Protocol/IPMITransportProtocol.h>
#include <IpmiNetFnChassisDefinitions.h>
#endif // defined IPMI_SUPPORT && IPMI_SUPPORT == 0x01
#include <Setup.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include "SmcCspLibSb.h"
#include "SmcSetPowerStatus.h"

#if IPMI_SUPPORT
EFI_IPMI_TRANSPORT  *mIpmiTransport;
#endif

#define BIOS_STAY_OFF      0x00
#define BIOS_POWER_ON      0x01
#define BIOS_LAST_STATE    0x02

#define IPMI_STAY_OFF      0x00
#define IPMI_LAST_STATE    0x01
#define IPMI_POWER_ON      0x02
#define IPMI_NO_CHANGE     0x03

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SmcSetPowerStatusEntry
//
// Description:
//  Driver entry point. Depend on AC loss policy, get power policy from 
//  BIOS/IPMI and save setting to IPMI/BIOS.
//
// Input:
//  ImageHandle     - Handle for the image of this driver
//  SystemTable     - Pointer to the EFI System Table
//
// Output:      
//  EFI_SUCCESS     - Protocol successfully started and installed
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
SmcSetPowerStatusEntry(
    IN	EFI_HANDLE		ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
#if IPMI_SUPPORT
    EFI_STATUS      Status;
    EFI_GUID        SetupGuid = SETUP_GUID;
    SETUP_DATA      SetupData;
    UINTN           Size = sizeof(SETUP_DATA);
    UINT8           Buffer[100], DataSize, IpmiPowerPolicy, CommandRetry;

    Status = gBS->LocateProtocol(
		    &gEfiDxeIpmiTransportProtocolGuid, 
		    NULL,
		    &mIpmiTransport);

    if(EFI_ERROR(Status))	return EFI_SUCCESS;

    Status = gRT->GetVariable(
		    L"Setup", 
		    &SetupGuid, 
		    NULL, 
		    &Size, 
		    &SetupData);

    if(EFI_ERROR(Status))	return EFI_SUCCESS;
    
    CommandRetry = 0x03;
    while(CommandRetry){
	DataSize = 100;
	Status = mIpmiTransport->SendIpmiCommand(
			mIpmiTransport,
			IPMI_NETFN_CHASSIS,
			BMC_LUN,
			IPMI_CHASSIS_GET_STATUS,
			NULL,
			0,
			(UINT8*)&Buffer,
			&DataSize);

	if(!Status)	break;
	else	gBS->Stall(100000);	// delay 100ms
	
	CommandRetry--;
    }
    if(Status)	return EFI_SUCCESS; 

    IpmiPowerPolicy = ((Buffer[0] >> 5) & 0x03);

    // AcLossPolicy = 0x00 : set from BIOS
    // AcLossPolicy = 0x01 : set from IPMI
    if(SetupData.AcLossPolicy == 0x00){
	switch(SetupData.SmcAfterG3Save){
	case BIOS_STAY_OFF:
	    Buffer[0] = IPMI_STAY_OFF;
	    break;
	case BIOS_POWER_ON:
	    Buffer[0] = IPMI_POWER_ON;
	    break;
	case BIOS_LAST_STATE:
	    Buffer[0] = IPMI_LAST_STATE;
	    break;
	default:
	    Buffer[0] = IPMI_NO_CHANGE;
	    break;
	}

	CommandRetry = 0x03;
	while(CommandRetry){
	    DataSize = 100;
	    Status = mIpmiTransport->SendIpmiCommand(
			    mIpmiTransport,
			    IPMI_NETFN_CHASSIS,
			    BMC_LUN,
			    IPMI_CHASSIS_SET_POWER_RESTORE_POLICY,
			    (UINT8*) &Buffer,
			    1,
			    (UINT8*) &Buffer,
			    &DataSize);
      
	    if(!Status)	break;
	    else	gBS->Stall(100000);	// delay 100ms
        
	    CommandRetry--;
	}
	if(Status)	return EFI_SUCCESS;
    } 
    else if(SetupData.AcLossPolicy == 0x01){
	switch(IpmiPowerPolicy){
	case IPMI_STAY_OFF:
	    SetupData.SmcAfterG3Save = IPMI_STAY_OFF;
	    break;
	case IPMI_POWER_ON:
	    SetupData.SmcAfterG3Save = BIOS_POWER_ON;
	    break;
	case IPMI_LAST_STATE:
	    SetupData.SmcAfterG3Save = BIOS_LAST_STATE;
	    break;
	default:
	    break;
	}

	Status = gRT->SetVariable(
			L"Setup",
			&SetupGuid,
			EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
			sizeof (SETUP_DATA),
			&SetupData);

	if(EFI_ERROR(Status))	return EFI_SUCCESS;
    }

    if(SetupData.SmcAfterG3Save == 0)
	Status = AfterG3Setting(1);
    else
	Status = AfterG3Setting(0);
#endif
    return EFI_SUCCESS;
}

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2013, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
