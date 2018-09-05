//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Use SATA RAID driver option to control RAID OPROM.
//    Reason:   Follow previous platform behavior.
//    Auditor:  Jacker Yeh
//    Date:     Jul/17/2016
//
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

/** @file CRBDXE.c

    This file contains code for Chipset Reference Board Template initialization in the DXE stage
*/

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Efi.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Dxe.h>
#include <PCI.h>
#include <AmiHobs.h>
#include <UpdateSmbiosTable.h>
#include <AmiCspLib.h>
#if defined CIRA_SAMPLE_SUPPORT && CIRA_SAMPLE_SUPPORT == 1
#include <ME.h>
#include <AMT.h>
#endif

#include <CrbDxeInitElink.h>
// Produced Protocols

// Consumed Protocols
#include <Protocol\PciIO.h>
#include <Protocol\PciRootBridgeIo.h>
#include <Protocol\BootScriptSave.h>

#if CMOS_MANAGER_SUPPORT
#include <Guid/SetupVariable.h>
#include <Include/CmosAccess.h>
#include <Register/PchRegsSata.h>
#include <Library/UefiLib.h>
#endif
#include <Include/Guid/PchRcVariable.h>

#include <Setup.h>
#include <Library/DebugLib.h>


#include <Protocol/AmiBoardInfo2.h>	
#include <Protocol/IioSystem.h> 

#include <PchPolicyCommon.h> 


//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)


// GUID Definition(s)

EFI_GUID    gAmiBoardInfo2Guid = AMI_BOARD_INFO2_PROTOCOL_GUID;

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   UpdateLRSlotNumber
//
// Description: Read the slotpsp in global data to determine riser populated.
//		Then update slot numbers in AmiBoard2Protocol.
//
// Input:       None
//
// Output:      None
//
// Return :     VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
UpdateLRSlotNumber (
  IN  EFI_EVENT Event,
  IN  VOID      *Context )
{
	EFI_STATUS			Status;
	EFI_IIO_SYSTEM_PROTOCOL		*IioSystemProtocol;
	IIO_GLOBALS			*IioGlobalData;
	AMI_BOARD_INFO2_PROTOCOL    	*amibrd=NULL;
	UINT32				Index;
	
    DEBUG((EFI_D_INFO, __FUNCTION__" Start \n"));
	  
	Status = pBS->LocateProtocol (
			&gEfiIioSystemProtocolGuid,
			NULL,
			&IioSystemProtocol
			);
	if ( EFI_ERROR ( Status ) ){
	      return;
	    }
	
	IioGlobalData = IioSystemProtocol->IioGlobalData;
	
	Status=pBS->LocateProtocol(&gAmiBoardInfo2Guid, NULL, &amibrd);
	
	if ( EFI_ERROR ( Status ) ){
	      return;
	    }
	
    DEBUG((EFI_D_INFO, "PciDevCount %x \n", amibrd->PciBrdData->PciDevCount));
	for(Index = 0; ((Index < amibrd->PciBrdData->PciDevCount)); Index++){
		
		switch(amibrd->PciBrdData->PciDevices[Index].Slot){
		case 0:	//No Slot
			break;
		case 1:
			amibrd->PciBrdData->PciDevices[Index].Slot = IioGlobalData->SetupData.SLOTPSP[SOCKET_3_INDEX + PORT_3A_INDEX];
			break;
		case 2:
			amibrd->PciBrdData->PciDevices[Index].Slot = IioGlobalData->SetupData.SLOTPSP[SOCKET_0_INDEX + PORT_3A_INDEX];
			break;
		case 3:
			amibrd->PciBrdData->PciDevices[Index].Slot = IioGlobalData->SetupData.SLOTPSP[SOCKET_3_INDEX + PORT_2A_INDEX];
			break;
		case 4:
			//There should be no slot 4 by default.
			break;
		case 5:
			amibrd->PciBrdData->PciDevices[Index].Slot = IioGlobalData->SetupData.SLOTPSP[SOCKET_1_INDEX + PORT_1A_INDEX];
			break;
		case 6:
			amibrd->PciBrdData->PciDevices[Index].Slot = IioGlobalData->SetupData.SLOTPSP[SOCKET_2_INDEX + PORT_3A_INDEX];
			break;
		case 7:
			amibrd->PciBrdData->PciDevices[Index].Slot = IioGlobalData->SetupData.SLOTPSP[SOCKET_1_INDEX + PORT_2A_INDEX];
			break;
		case 8:
			amibrd->PciBrdData->PciDevices[Index].Slot = IioGlobalData->SetupData.SLOTPSP[SOCKET_2_INDEX + PORT_1A_INDEX];
			break;
		default:
			break;
			
		}
						
	}

}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SetRSTeBootInfo
//
// Description: Read the user settings for SCU and SATA OROM and update into the
//              CMOS token for use in the Get RSTe OROM Boot Info Int 15 handler.
//
// Input:       None
//
// Output:      None
//
// Return :     VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
SetRSTeBootInfo (
  IN  EFI_EVENT Event,
  IN  VOID      *Context )
{

    EFI_STATUS					Status;
    EFI_CMOS_ACCESS_INTERFACE   *pCmosAccess;
    PCH_RC_CONFIGURATION      PchRcConfiguration;
    EFI_GUID 					SetupGuid = SETUP_GUID;
    UINTN 						VariableSize = 0;
    UINT8 						RSTeBootInfo = 0; 
    UINT8						MaskValue = CMOSMGR_RSTE_BOOT_INFO_OFFSET_MASK;
    UINT8						BitPos = 0;

    Status = pBS->LocateProtocol( &gAmiCmosAccessDxeProtocolGuid, NULL, &pCmosAccess ); 
    if ( EFI_ERROR ( Status ) ){
      return;
    }
    VariableSize = sizeof( PCH_RC_CONFIGURATION );
    Status = pRS->GetVariable(
    		L"PchRcConfiguration",
    		&gEfiPchRcVariableGuid,
    		NULL,
    		&VariableSize,
    		&PchRcConfiguration
    		);
    if ( EFI_ERROR ( Status ) ) {
	// PCH setup variable is moved to PCH_RC_CONFIGURATION
        PchRcConfiguration.SataLegacyBootContSel = 1; //default value
        PchRcConfiguration.sSataLegacyBootContSel = 1; //default value
    }

    pCmosAccess->Read( pCmosAccess, CMOSMGR_RSTE_BOOT_INFO_OFFSET, &RSTeBootInfo );
    RSTeBootInfo &= ~CMOSMGR_RSTE_BOOT_INFO_OFFSET_MASK; // Both SATA and sSATA controller is selected as boot controller.    
	
    if (( PchRcConfiguration.sSataInterfaceMode == PchSataModeRaid && PchRcConfiguration.sSataLegacyBootContSel ) &&
    ( PchRcConfiguration.SataInterfaceMode == PchSataModeRaid && PchRcConfiguration.SataLegacyBootContSel )){ // PCH setup variable is moved to PCH_RC_CONFIGURATION
	    goto RsteBootInfoExit; // Both SATA nor SCU controller is selected as boot controller.
    }
    
    if ( (!(PchRcConfiguration.sSataLegacyBootContSel)) && (!(PchRcConfiguration.SataLegacyBootContSel)) ) { // PCH setup variable is moved to PCH_RC_CONFIGURATION
    	RSTeBootInfo |= CMOSMGR_RSTE_BOOT_INFO_OFFSET_MASK; // Neither SATA nor SCU controller is selected as boot controller.
    	goto RsteBootInfoExit;
    }

    // Get the Boot Info bits position from masked data. 
    while ( TRUE ) {
    	if ( MaskValue & BIT00 )
    		break;
    	BitPos++; 
    	MaskValue = MaskValue >> 1;       
    }          

    if ( PchRcConfiguration.sSataInterfaceMode == PchSataModeRaid && PchRcConfiguration.sSataLegacyBootContSel ) // PCH setup variable is moved to PCH_RC_CONFIGURATION
    	RSTeBootInfo |= ( BIT00 << BitPos );  // sSATA OROM is enabled.

    if ( PchRcConfiguration.SataInterfaceMode == PchSataModeRaid && PchRcConfiguration.SataLegacyBootContSel ) // PCH setup variable is moved to PCH_RC_CONFIGURATION
    	RSTeBootInfo |= ( BIT01 << BitPos ); // SATA OROM is enabled.

RsteBootInfoExit:
    Status = pCmosAccess->Write( pCmosAccess, CMOSMGR_RSTE_BOOT_INFO_OFFSET, RSTeBootInfo );
    if ( EFI_ERROR ( Status ) ){
        DEBUG ((EFI_D_ERROR," Get RSTe OROM Boot Info BIOS Services: Unable to write to CMOS variable holding RSTe Boot info. Status %r\n", Status ));
        return;
    }
	
    pBS->CloseEvent(Event);
}


//----------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   CRBDXE_Init
//
// Description: This function is the entry point for this DXE. This function
//              initializes the CRB
//
// Parameters:  ImageHandle Image handle
//              SystemTable Pointer to the system table
//
// Returns:     Return Status based on errors that occurred while waiting for
//              time to expire.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS CRBDXE_Init (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS Status = EFI_SUCCESS;
#if PLATFORM_TYPE == 1  
    VOID                *AmiBoard2ProtocolRegistration;
#endif

#if SMCPKG_SUPPORT
    PCH_RC_CONFIGURATION      PchRcConfiguration;
    UINTN 						VariableSize = 0;
    VOID                *AmiCmosAccessProtocolRegistration;
#else
    VOID                *UpdateSmbiosTableReg;
    EFI_EVENT           UpdateSmbiosTableEvent;
#endif

#if defined CIRA_SAMPLE_SUPPORT && CIRA_SAMPLE_SUPPORT == 1
    AMI_AMT_PROTOCOL *mAmtProtocol;
    EFI_GUID mAmtProtocolGuid = AMI_AMT_PROTOCOL_GUID;
    EFI_GUID mSetupGuid = SETUP_GUID;
    UINTN mSetupDataSize = NULL;
    SETUP_DATA mSetupData;
#endif

    InitAmiLib(ImageHandle, SystemTable);

#if defined CIRA_SAMPLE_SUPPORT && CIRA_SAMPLE_SUPPORT == 1
    mSetupDataSize = sizeof(SETUP_DATA);
    Status = pRS->GetVariable (  
                                L"Setup",
                                &mSetupGuid,
                                NULL,
                                &mSetupDataSize,
                                &mSetupData 
                              );

    if (mSetupData.CIRATrigger == 1)
    {
        Status = pBS->LocateProtocol(&mAmtProtocolGuid, NULL, &mAmtProtocol);
        if (!EFI_ERROR(Status))
            mAmtProtocol->SetCIRA(mAmtProtocol, CIRA_ENABLE, CIRA_DEFFAULT_TIME);
    }
#endif
#if SMCPKG_SUPPORT == 0
    UpdateSmbiosTableEvent = EfiCreateProtocolNotifyEvent (
                                            &gEfiSmbiosProtocolGuid,
                                            TPL_CALLBACK,
                                            UpdateSmbiosTables,
                                            NULL,
                                            &UpdateSmbiosTableReg );
#endif
 //
 // RSTe Support Starts
 //
#if CMOS_MANAGER_SUPPORT && CSM_SUPPORT
#if SMCPKG_SUPPORT
    VariableSize = sizeof( PCH_RC_CONFIGURATION ); //AptioV Purley Override
    Status = pRS->GetVariable(
    		L"PchRcConfiguration", //AptioV Purley Override
    		&gEfiPchRcVariableGuid, //AptioV Purley Override
    		NULL,
    		&VariableSize,
    		&PchRcConfiguration  //AptioV Purley Override
    		);

    if(EFI_ERROR(Status)) {
		PchRcConfiguration.SataRaidLoadEfiDriver = 1;     //EFI option rom
		PchRcConfiguration.sSataRaidLoadEfiDriver = 1;    //EFI option rom
    }
    
    if((PchRcConfiguration.SataRaidLoadEfiDriver = 2) || (PchRcConfiguration.sSataRaidLoadEfiDriver = 2)) {
		
    	EfiCreateProtocolNotifyEvent (
        	&gAmiCmosAccessDxeProtocolGuid,
        	TPL_CALLBACK,
        	SetRSTeBootInfo,
        	NULL,
        	&AmiCmosAccessProtocolRegistration
        	);
    }
#else // #if SMCPKG_SUPPORT
    {
	    VOID                *AmiCmosAccessProtocolRegistration;

    	EfiCreateProtocolNotifyEvent (
    			&gAmiCmosAccessDxeProtocolGuid,
    			TPL_CALLBACK,
    			SetRSTeBootInfo,
    			NULL,
    			&AmiCmosAccessProtocolRegistration );
    }
#endif // #if SMCPKG_SUPPORT
#endif

#if PLATFORM_TYPE == 1    
    EfiCreateProtocolNotifyEvent (
		    &gEfiIioSystemProtocolGuid,
		    TPL_CALLBACK,
		    UpdateLRSlotNumber,
		    NULL,
		    &AmiBoard2ProtocolRegistration );
#endif

    return EFI_SUCCESS;
}

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
