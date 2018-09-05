//***************************************************************************
//**       (C)Copyright 1993-2015 Supermicro Computer, Inc.                **
//***************************************************************************
//
//  File History
//
//  Rev. 1.01
//		Bug Fix:  Fixed build error when disable SMCPKG. 
//		Reason:   GetDevicePathNodeOfType() is redefine in BootOptions.
//		Auditor:  Chen Lin
//		Date:     Oct/27/15
//
//  Rev. 1.00
//		Bug Fix:  Implement SMC BCP boot order style.
//		Reason:   Initialize revision.
//		Auditor:  Jacker Yeh
//		Date:     Jul/09/13
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log: $
// 
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  BcpBootOrder.c
//
// Description: Generate default boot order according to BCP boot order.
//
//<AMI_FHDR_END>
//**********************************************************************

#include <BootOptions.h>
#include <Protocol/DevicePath.h>
#include "BcpBootOrder.h"
#include <Protocol/LegacyBios.h>
#include <token.h>
#include <Setup.h>

BOOLEAN IsFirstBootFlag = FALSE;	//SMC 1.05
#if BOOT_ORDER_STYLE_SUPPORT
UINT8 BootOrderStyle = 0;
#endif
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LocateSectionResource
//
// Description: This function locate resource binary by GUID. 
//
// Input:
//  IN  EFI_GUID *ResourceGuid          - pointer to resource GUID
//  IN  EFI_GUID *ResourceSectionGuid   - pointer to resource section GUID
//  OUT UINT8 **Address                 - pointer to returning address of the resource
//
// Output:  EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS LocateSectionResource(
    IN  EFI_GUID *ResourceFileGuid,
    IN  EFI_GUID *ResourceSectionGuid,
    OUT UINT8 **Address )
{
    EFI_STATUS Status;
    EFI_GUID EfiFirmwareVolumeProtocolGuid = EFI_FIRMWARE_VOLUME2_PROTOCOL_GUID;
    EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVolumeProtocol = NULL;
    EFI_HANDLE *HandleBuffer = NULL;
    UINTN  NumberOfHandles;
    UINTN  Index;
    UINT32 Authentication;
    UINTN  i;
    EFI_GUID *SectionGuid = NULL;
    UINTN  SectionSize;

    // Locate the Firmware volume protocol
    Status = pBS->LocateHandleBuffer(
        ByProtocol,
        &EfiFirmwareVolumeProtocolGuid,
        NULL,
        &NumberOfHandles,
        &HandleBuffer
    );
    if (EFI_ERROR(Status)) 
        return EFI_NOT_FOUND;
    
    // Find and read raw data
    for (Index = 0; Index < NumberOfHandles; Index++) {

        Status = pBS->HandleProtocol(
            HandleBuffer[Index],
            &EfiFirmwareVolumeProtocolGuid,
            &FwVolumeProtocol
        );
        if (EFI_ERROR(Status)) 
            continue;
        
        i = 0;
        while(1) {
            SectionSize = 0;
            Status = FwVolumeProtocol->ReadSection(
                FwVolumeProtocol,
                ResourceFileGuid,
                EFI_SECTION_FREEFORM_SUBTYPE_GUID,
                i++,
                &SectionGuid,
                &SectionSize,
                &Authentication
            );
            if (EFI_ERROR(Status))
                break;
            else if ( !guidcmp(SectionGuid, ResourceSectionGuid) ) {
                *Address = (UINT8*)SectionGuid;
                goto LocateSectionResource_exit;
            }
            pBS->FreePool(SectionGuid);
            SectionGuid = NULL;
        } 
    }
    
LocateSectionResource_exit:
    pBS->FreePool(HandleBuffer);
    return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   GetDevicePathNodeOfType
//
// Description: This function looking for specified device path type.
//
// Input:       
//  EFI_DEVICE_PATH_PROTOCOL *Dp - pointer to device path
//  UINT8 Type - requested device path type
//
// Output:     EFI_DEVICE_PATH_PROTOCOL*  
//                          pointer to EFI_DEVICE_PATH_PROTOCOL node founded by type.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
//SMC+
//EFI_DEVICE_PATH_PROTOCOL* GetDevicePathNodeOfType( EFI_DEVICE_PATH_PROTOCOL *Dp, UINT8 Type )
static EFI_DEVICE_PATH_PROTOCOL* GetDevicePathNodeOfType( EFI_DEVICE_PATH_PROTOCOL *Dp, UINT8 Type )
//SMC-
{
    if (Dp == NULL) 
        return NULL;

    for( ; !(isEndNode(Dp)); Dp = NEXT_NODE(Dp) )
        if (Dp->Type == Type) 
            return Dp;
    return NULL;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   IsDeviceMatch
//
// Description: This function checking device from BDS option list. 
//
// Input:       
//  IN BCP_BOOT_ORDER_ITEM_INFO *BcpBootOrderItemInfo - pointer to BCP item info 
//  IN BOOT_OPTION *Option - pointer to BDS option
//
// Output: BOOLEAN
//  TRUE  - device match
//  FALSE - device not match
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

BOOLEAN IsDeviceMatch( 
    IN BCP_BOOT_ORDER_ITEM_INFO *BcpBootOrderItemInfo, 
    IN BOOT_OPTION *Option )
{
    EFI_STATUS Status;
    EFI_DEVICE_PATH_PROTOCOL *Dp = NULL;
    UINTN BusOrder = BCP_UNDEFINED;

    // check item by BBS type
//-    if ( BcpBootOrderItemInfo->BbsType != Option->BbsEntry->DeviceType ){
    if ( BcpBootOrderItemInfo->BbsType != GetLegacyDevOrderType(Option) )
        return FALSE;
    
    // Check if Messaging Device Path Type defined
    if (BcpBootOrderItemInfo->MsgDpSubType != BCP_UNDEFINED) {

        // Locate device path by handle
        Status = pBS->HandleProtocol(
            Option->DeviceHandle,
            &gEfiDevicePathProtocolGuid,
            &Dp
        );
	if (EFI_ERROR(Status)){
		if (((Dp == NULL) || (Dp->SubType != BcpBootOrderItemInfo->MsgDpSubType)) && (BcpBootOrderItemInfo->MsgDpSubType == MSG_SATA_DP))	//0x12
		{
			TRACE((TRACE_ALWAYS,"Patch SAS/RAID device tag problem\n"));
			Dp->SubType = MSG_SATA_DP;
			return TRUE;
		}
	}
        if (EFI_ERROR(Status)) 
            return FALSE;

        // looking for messaging device path
        Dp = GetDevicePathNodeOfType(Dp, MESSAGING_DEVICE_PATH);
        if ( (Dp == NULL) || (Dp->SubType != BcpBootOrderItemInfo->MsgDpSubType) )
            return FALSE;

        switch(Dp->SubType) {
    
            // 0=PM, 1=PS, 2=SM, 3=SS
            case MSG_ATAPI_DP:
                BusOrder = 0;
                if ( ((ATAPI_DEVICE_PATH*)Dp)->PrimarySecondary ) 
                    BusOrder |= BIT1; // zero for primary, one for secondary
                if ( ((ATAPI_DEVICE_PATH*)Dp)->SlaveMaster )
                    BusOrder |= BIT0; // zero for master, one for slave mode
            break;
    
            // SATA SATA_DEVICE_PATH includes location information
            case MSG_SATA_DP:
                BusOrder = ((SATA_DEVICE_PATH*)Dp)->HBAPortNumber;
            break;
    
            default:
                BusOrder = BCP_UNDEFINED;
            break;
        }
    }

    if (BcpBootOrderItemInfo->BusOrder != BusOrder) 
        return FALSE;

    return TRUE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   CheckBootOrderStyle
//
// Description: 
//  This function is to check the SmcBootStyle setup option to update BootOrderStyle flag.
//  BootOrderStyle = 0 => Follow AMI style define by BOOT_OPTION_TAG_PRIORITIES token. 
//  BootOrderStyle = 1 => New detected boot device will be set to the last boot option.
//  BootOrderStyle = 2 => When detect new boot device, the boot order will reset follow AMIBCP setting.
//
// Input:       VOID
//
// Output:      VOID
//
//----------------------------------------------------------------------------
#if BOOT_ORDER_STYLE_SUPPORT
VOID CheckBootOrderStyle(VOID)
{
    UINTN VariableSize = 0;
    EFI_STATUS Status;
    EFI_GUID  gSetupGuid = SETUP_GUID;
    SETUP_DATA	SetupData;

    VariableSize = sizeof(SETUP_DATA);
    Status = pRS->GetVariable(
	    	L"Setup",
		&gSetupGuid,
		NULL,
		&VariableSize,
		&SetupData
		);

    if (!EFI_ERROR(Status)){
        BootOrderStyle = 2;//follow BCP
//SMC R1.06+START
	    if (SetupData.SmcNotFirstBoot == 0) {
	        IsFirstBootFlag = TRUE;
	    }
	}	
//SMC R1.06-END
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   CheckIfFirstBoot
//
// Description: 
//  Entry point - if first boot detected then sets global variable to signal
//  SetDefaultBcpBootOptionPriorities function that sorting is needed.
//
// Input:       VOID
//
// Output:      VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID CheckIfFirstBoot(VOID)
{
    DLINK *Link = NULL;
    BOOT_OPTION *Option = NULL;

    // Newly connected devices are have LOWEST_BOOT_OPTION_PRIORITY priority.
    // During the very first there are no boot options in the NVRAM, 
    // which means that all the boot option in the boot list must have 
    // LOWEST_BOOT_OPTION_PRIORITY priority.
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
    {
        // Check if priority not equal LOWEST_BOOT_OPTION_PRIORITY 
        // then it's not a first boot, no action needed.

#if BOOT_ORDER_STYLE_SUPPORT
	if(BootOrderStyle == 2)
                if ( (Option->Priority == LOWEST_BOOT_OPTION_PRIORITY) || (((Option->Priority) & 0x00FF) != 0)) {
			    IsFirstBootFlag = TRUE;	//SMC 1.05
			}
#endif

        if ( Option->Priority != LOWEST_BOOT_OPTION_PRIORITY ) {

#if BOOT_ORDER_STYLE_SUPPORT
			if(BootOrderStyle != 2)
#endif

            return;
        }
    }

#if BOOT_ORDER_STYLE_SUPPORT	
	if(BootOrderStyle != 2)
#endif

    	IsFirstBootFlag = TRUE;	//SMC 1.05
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SetDefaultBcpBootOptionPriorities
//
// Description: 
//  The BCP boot order defines priorities of boot devices during first system 
//  boot if there are no boot options in the NVRAM. Our hook is called after 
//  adding all boot options to the boot list and before setting priorities. 
//  The boot options in the boot list come from two sources: NVRAM and newly 
//  connected devices. NVRAM boot options have priority derived from BootOrder
//  NVRAM variable. 
//
// Input:       VOID
//
// Output:      VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID SetDefaultBcpBootOptionPriorities(VOID)
{
    static EFI_GUID BcpBootOrderSectionGuid = BCP_BOOT_ORDER_SECTION_GUID;
    static EFI_GUID BcpBootOrderFileGuid = BCP_BOOT_ORDER_FILE_GUID;
    static BCP_BOOT_ORDER_ITEM_INFO BcpBootOrderItemInfo[] = BCP_DEVICE_INFO_ARRAY;

    DLINK *Link = NULL;
    BOOT_OPTION *Option = NULL;
    EFI_STATUS Status;
    BCP_BOOT_ORDER_SECTION *BcpSection = NULL;
    UINT32 Priority = 0, UefiPriority = 0;
    UINTN  i, j;
#if BOOT_ORDER_STYLE_SUPPORT
    UINT32 Legacy_LargestPriority = 0;
#endif

//    if (!IsFirstBoot) 
//        return;
    TRACE((TRACE_ALWAYS,"SetDefaultBcpBootOptionPriorities: first boot detected\n"));

    // Read BCP Default Boot Order Section from the FFS file
    // the format of the section is defined by the BCP_BOOT_ORDER_SECTION type.
    // The values in the Option array are defined by the BCP_BOOT_ORDER_ITEM array
    
    // Locate default BCP Boot Order
    Status = LocateSectionResource(
        &BcpBootOrderFileGuid, 
        &BcpBootOrderSectionGuid, 
        &(UINT8*)BcpSection
     );
    if (EFI_ERROR(Status)) {
        TRACE((TRACE_ALWAYS, "BCP Boot Order Section is Not Found!!!\n"));
        return;
    }

    SortList(BootOptionList, ComparePriorityThenBootOptionNumber);
    DUMP_BOOT_OPTION_LIST(BootOptionList,"Before BCP Processing");
    Priority = 0xFF00; 
    UefiPriority = 0;
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
    {
        if (IsLegacyBootOption(Option)){
			if(IsFirstBootFlag){	//SMC 1.05
		 		Option->Priority = Priority;
		 	}
#if BOOT_ORDER_STYLE_SUPPORT
		if(BootOrderStyle == 1){
 		 	Legacy_LargestPriority = Option->Priority;
			//TRACE((TRACE_ALWAYS,"Legacy devices LargestPriority = %x\n", Option->Priority));
		}	
#endif			
 		 	Option->Tag = Priority;
		 	Priority++;
		 }
        else {
			if(IsFirstBootFlag){	//SMC 1.05
				Option->Priority = UefiPriority++;
			}
#if BOOT_ORDER_STYLE_SUPPORT
			else if (((Option->Priority) & 0xFFF0) ==  Legacy_LargestPriority){//not first boot, so check is new detected UEFI device or not.
				if(BootOrderStyle == 1){
				Option->Priority = Option->Priority + 0x100;
			    //TRACE((TRACE_ALWAYS,"Rearrange UEFI devices priority to avoid conflict legacy devices priority \n"));
				}
			}
#endif
		}
    }
    Priority = UefiPriority;

    // Go though BCP boot order
    for ( i = 0; i < BcpSection->NumberOfOptions; i++ ) {

        // Find description item for option from BCP order
        for ( j = 0; j < ( sizeof(BcpBootOrderItemInfo) / sizeof(BCP_BOOT_ORDER_ITEM_INFO) ); j++ ) {
            if ( BcpBootOrderItemInfo[j].TokenNumber == BcpSection->Option[i] )
                break;
        }
    
        // Search boot option to set priority
        FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
        {
            // skip non-legacy boot options
            if (!IsLegacyBootOption(Option)) 
				{
					if (IsFirstBootFlag){	//SMC 1.05
                	Option->Priority = UNASSIGNED_HIGHEST_TAG;//set uefi boot device to lowest priority.
					}
	                Option->Tag = UNASSIGNED_HIGHEST_TAG;//set uefi boot device to lowest tag.
                	continue;
				}
            if (Option->GroupHeader){
                if (   (Option->Tag&0xFF00)== 0xFF00 
                    && GetLegacyDevOrderType(Option)==BcpBootOrderItemInfo[j].BbsType)
				   {
			    TRACE((TRACE_ALWAYS,"Group Device Priority before set=> %4x\n", Option->Priority));				   
			    TRACE((TRACE_ALWAYS,"Group Device Tag before set=> %4x\n", Option->Tag));
				if (IsFirstBootFlag){	//SMC 1.05
				 Option->Priority = Priority;
				 }
#if BOOT_ORDER_STYLE_SUPPORT
				 else if (((Option->Priority) & 0x00FF) != 0){//not first boot, so check is new detected device or not.
					if(BootOrderStyle == 1){
				 	Option->Priority = ((Option->Priority) & 0x00FF) + Legacy_LargestPriority;
					//TRACE((TRACE_ALWAYS,"not first boot, arrange new detected device to last devices(GROUP header)\n"));
					}
}
#endif
					//TRACE((TRACE_ALWAYS,"Group Option->Priority = %x\n", 	Option->Priority));
                 Option->Tag = Priority;
				 Priority++;
 			    TRACE((TRACE_ALWAYS,"Group Device Priority after set=> %4x\n", Option->Priority));
			    TRACE((TRACE_ALWAYS,"Group Device Tag after set=> %4x\n", Option->Tag));
				}
                continue;				
            }

            // set priority if BCP boot order item according to bus order 
            if ( (Option->Tag&0xFF00)== 0xFF00 && IsDeviceMatch(&BcpBootOrderItemInfo[j],Option) ) {
			    TRACE((TRACE_ALWAYS,"Legacy Device Priority before set=> %4x\n", Option->Priority));
				if (IsFirstBootFlag){	//SMC 1.05
                Option->Priority = Priority;
				}
#if BOOT_ORDER_STYLE_SUPPORT
				else if (((Option->Priority) & 0x00FF) != 0){//not first boot, so check is new detected device or not.
					if(BootOrderStyle == 1){
				    TRACE((TRACE_ALWAYS,"not first boot, arrange new detected device to last devices\n"));
					Option->Priority = ((Option->Priority) & 0x00FF) + Legacy_LargestPriority;
					}
				}
#endif
                Option->Tag = Priority;
                Priority++;
			    TRACE((TRACE_ALWAYS,"Legacy Device Priority after set=> %4x\n", Option->Priority));
            }
        }
    }
    
    Link = BootOptionList->pHead;
    while(Link != NULL) {
        UINT16 OptionNumber;
        BOOT_OPTION *Option2;
        UINT32 OptionPriority;

        Option = OUTTER(Link,Link,BOOT_OPTION);
        OptionNumber = Option->BootOptionNumber;
        OptionPriority = LOWEST_BOOT_OPTION_PRIORITY;

        while(Link != NULL) {
            Option2 = OUTTER(Link, Link, BOOT_OPTION);
            if (OptionNumber != Option2->BootOptionNumber) 
                break;
            if (Option->Priority < OptionPriority) 
                OptionPriority = Option->Priority;
            Link = Link->pNext;
        }
        Link = &Option->Link;
        while(Link != NULL) {
            Option2 = OUTTER(Link,Link,BOOT_OPTION);
            if (OptionNumber != Option2->BootOptionNumber) 
                break;
            Option2->Priority |= OptionPriority << 16;
            Link = Link->pNext;
        }
    }

    DUMP_BOOT_OPTION_LIST(BootOptionList,"After BCP Processing");
    pBS->FreePool(BcpSection);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
