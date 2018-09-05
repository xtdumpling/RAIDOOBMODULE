//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/**
 *  @file BootOptions.c
 *  Contains the code for dealing with boot options and their maintenance
 *
 */
#include "BootOptions.h"
#include <Protocol/DiskInfo.h>
#include <Protocol/UsbIo.h>

EFI_HII_HANDLE HiiHandle=0;
EFI_GUID AmiBbsDevicePathGuid = AMI_BBS_DEVICE_PATH_GUID;
EFI_GUID AmiMaskedDevicePathGuid = AMI_MASKED_DEVICE_PATH_GUID;
EFI_GUID AmiDeviceNameDevicePathGuid = AMI_DEVICE_NAME_DEVICE_PATH_GUID;
#ifdef CSM_SUPPORT
EFI_GUID LegacyDevOrderGuid = LEGACY_DEV_ORDER_GUID;
#endif

DLIST BootOptionListStructure;
DLIST BootDeviceListStructure;
DLIST *BootOptionList = &BootOptionListStructure;
DLIST *BootDeviceList = &BootDeviceListStructure;

// Helper functions 
// Defined in Bds.c
EFI_STATUS SetVariableWithNewAttributes(
    IN CHAR16 *Name, IN EFI_GUID *Guid, IN UINT32 Attributes,
    IN UINTN DataSize, IN VOID *Data    
);
// Defined in BdsBoard.c
UINT32 GetSetupVariablesAttributes();

/**
 *  Sort the list of DLIST items using the passed COMPARE_FUNCTION to
 *  determine the correct ordering of items
 *
 *  @param List Pointer to list of items to sort
 *  @param Compare Function to use to to sort the list
 */
VOID SortList(DLIST *List,  COMPARE_FUNCTION Compare){
    DLIST MergeList;
    DLINK *p, *q, *e;
    UINTN pSize, qSize, inSize;
    UINT32 NoMerges;
    UINTN i;

    if(List->Size <= 1)  //nothing to sort
        return;

    inSize = 1;
    MergeList = *List;

    while(1)
    {
        p = MergeList.pHead;
        DListInit(&MergeList);      //clear list
        NoMerges = 0;

        while(p != NULL)
        {
            NoMerges++;
            q = p;
            for(i = 0, pSize = 0; i < inSize; i++)
            {
                pSize++;
                q = q->pNext;
                if(q == NULL)
                    break;
            }

            qSize = inSize;
            while(pSize > 0 || (qSize > 0 && q != NULL))
            {
               if(pSize == 0)
                {
                    e = q;
                    q = q->pNext;
                    DListAdd(&MergeList, e);
                    qSize--;
                }
                else if(qSize == 0 || q == NULL)
                {
                    e = p;
                    p = p->pNext;
                    DListAdd(&MergeList, e);
                    pSize--;
                }
                else if(Compare(p, q) > 0)
                {
                    e = q;
                    q = q->pNext;
                    DListAdd(&MergeList, e);
                    qSize--;
                }
                else
                {
                    e = p;
                    p = p->pNext;
                    DListAdd(&MergeList, e);
                    pSize--;
                }
            }
            p = q;
        }
        if(NoMerges <= 1)
            break;
        inSize *= 2;
    }
    *List = MergeList;
}

/**
 *  Compare the two boot options passed and determine the priority of
 *  the first parameter in relation to the second parameter
 *  Try to compare based on the following parameters in order,
 *  Tag, then Group Header, then Priority
 *
 *  @param Link1 Pointer to boot option 1
 *  @param Link2 Pointer to boot option 2
 *
 *  @retval INT32 the comparison result Less than zero - Boot option 1 is lower priority than boot option 2
 *  @retval zero boot option 1 is same priority as boot option 2 greater than zero - boot option 1 is a higher priority than boot option 2
 */
INT32 CompareTagThenPriority(DLINK *Link1, DLINK *Link2){
	BOOT_OPTION *Option1 = (BOOT_OPTION*)Link1;
	BOOT_OPTION *Option2 = (BOOT_OPTION*)Link2;
	if (Option1->Tag < Option2->Tag) return -1;
	else if (Option1->Tag > Option2->Tag) return 1;
    if (Option1->GroupHeader != Option2->GroupHeader)
        return (Option1->GroupHeader) ? -1 : 1;
	if (Option1->Priority < Option2->Priority) return -1;
	else if (Option1->Priority > Option2->Priority) return 1;
    return 0;
}

/**
 *  Compare the two passed boot options first by their priority
 *  if their priorities are equal, compare them based on their
 *  boot option number
 *
 *  @param Link1 Pointer to boot option 1
 *  @param Link2 Pointer to boot option 2
 *
 *  @retval INT32 the comparison result Less than zero - Boot option 1 is lower priority than boot option 2
 *  @retval zero boot option 1 is same priority as boot option 2 greater than zero - boot option 1 is a higher priority than boot option 2
 */
INT32 ComparePriorityThenBootOptionNumber(DLINK *Link1, DLINK *Link2){
	BOOT_OPTION *Option1 = (BOOT_OPTION*)Link1;
	BOOT_OPTION *Option2 = (BOOT_OPTION*)Link2;
	if (Option1->Priority < Option2->Priority) return -1;
	else if (Option1->Priority > Option2->Priority) return 1;
	if (Option1->BootOptionNumber < Option2->BootOptionNumber) return -1;
	else if (Option1->BootOptionNumber > Option2->BootOptionNumber) return 1;
    if (Option1->GroupHeader != Option2->GroupHeader)
        return (Option1->GroupHeader) ? -1 : 1;
    return 0;
}

/**
 *  Create a BOOT_OPTION for the first entry in the passed BootOptionList
 *
 *  @param BootOptionList The list entry that needs a BOOT_OPTION structure created
 *
 *  @retval BOOT_OPTION Pointer to the created BOOT_OPTION
 */
BOOT_OPTION* CreateBootOption(DLIST *BootOptionList){
    static BOOT_OPTION BootOptionTemplate = {
    	{NULL,NULL}, LOAD_OPTION_ACTIVE, NULL, NULL, 0, NULL, 0,
        INVALID_BOOT_OPTION_NUMBER, LOWEST_BOOT_OPTION_PRIORITY,
        UNASSIGNED_HIGHEST_TAG,
        INVALID_HANDLE, INVALID_BBS_INDEX, NULL, FALSE, FALSE
    };

	BOOT_OPTION *Option = Malloc(sizeof(BOOT_OPTION));
    ASSERT(Option!=NULL);
    *Option = BootOptionTemplate;
	DListAdd(BootOptionList, &Option->Link);
    return Option;
}

/**
 *  Delete the passed BOOT_OPTION from the BootOptionList
 *
 *  @param BootOptionList the master boot option list
 *  @param Option the option that should be deleted from the master boot option list
 */
VOID DeleteBootOption(DLIST *BootOptionList, BOOT_OPTION *Option){
	DListDelete(BootOptionList,&Option->Link);
	if (Option->Description!=NULL) pBS->FreePool(Option->Description);
	if (Option->FilePathList!=NULL) pBS->FreePool(Option->FilePathList);
	if (Option->OptionalDataSize!=0) pBS->FreePool(Option->OptionalData);
	pBS->FreePool(Option);
}

/**
 *  Create a boot device entry and add it to the BootDeviceList
 *  fill the entry with the information from the passed parameters
 *
 *  @param BootDeviceList The master boot device list to add the new boot device entry
 *  @param DeviceHandle The handle of the new device
 *  @param BbsIndex The index in the BBS table of the device
 *  @param BbsEntry The entire BBS table entry for the device
 *
 *  @retval BOOT_DEVICE Pointer to the new boot device
 */
BOOT_DEVICE* CreateBootDevice(
    DLIST *BootDeviceList, EFI_HANDLE DeviceHandle,
	UINT16 BbsIndex, BBS_TABLE *BbsEntry
){
	BOOT_DEVICE *Device = Malloc(sizeof(BOOT_DEVICE));
    ASSERT(Device!=NULL);
	Device->BbsEntry = BbsEntry;
	Device->BbsIndex = BbsIndex;
	Device->DeviceHandle = DeviceHandle;
 	DListAdd(BootDeviceList, &Device->Link);
    return Device;
}

/**
 * Delete the Device out of the passed BootDeviceList
 *
 * @param BootDeviceList The list to remove the Device from
 * @param Device The entry to remove from the BootDeviceList
*/
VOID DeleteBootDevice(DLIST *BootDeviceList, BOOT_DEVICE* Device){
    DListDelete(BootDeviceList, &Device->Link);
    pBS->FreePool(Device);
}

/**
 * Update the boot option with the information from the passed boot device information
 *
 * @param Option The boot option that needs updated
 * @param Device The boot device that contains the information that the boot option needs updated with
*/
VOID UpdateBootOptionWithBootDeviceInfo(
    BOOT_OPTION *Option, BOOT_DEVICE* Device
){
	Option->BbsEntry = Device->BbsEntry;
	Option->BbsIndex = Device->BbsIndex;
	Option->DeviceHandle = Device->DeviceHandle;
}

/**
 * Create a boot option using the NVRAM information
 *
 * @param BootOptionList The master boot option list
 * @param BootOptionNumber The XXXX of the BootXXXX boot option
 * @param NvramOption The associated EFI_LOAD_OPTION
 * @param NvramOptionSize The size of the NVRAM option
 * @param Priority The priority of the boot option
 *
 * @retval BOOT_OPTION The created boot option
*/
BOOT_OPTION* CreateBootOptionsFromNvramOption(
	DLIST *BootOptionList, UINT16 BootOptionNumber,
	EFI_LOAD_OPTION *NvramOption, UINTN NvramOptionSize,
	UINT32 *Priority
){
	BOOT_OPTION *Option;
	UINTN DescriptionSize;
	UINT32 *OptionalData;
	UINT8 *FilePathList;
	UINTN OptionalDataSize;

	Option = CreateBootOption(BootOptionList);
	Option->Attributes=NvramOption->Attributes;
	DescriptionSize = (Wcslen((CHAR16*)(NvramOption+1))+1)*sizeof(CHAR16);
	Option->Description = Malloc(DescriptionSize);
	MemCpy(Option->Description, NvramOption+1, DescriptionSize);

	FilePathList = (UINT8*)(NvramOption+1)+DescriptionSize;
	Option->FilePathListLength = NvramOption->FilePathListLength;
	Option->FilePathList = Malloc(Option->FilePathListLength);
	MemCpy(
		Option->FilePathList, FilePathList, Option->FilePathListLength
	);

	OptionalData =  (UINT32*)(FilePathList + NvramOption->FilePathListLength);
    OptionalDataSize =  (UINT8*)NvramOption
							  + NvramOptionSize
							  - (UINT8*)OptionalData;
	Option->BootOptionNumber = BootOptionNumber;
	Option->Priority = *Priority;
	*Priority = GetNextBootOptionPriority(*Priority);
	if ( OptionalDataSize >= sizeof(UINT32) ){
		if ( ReadUnaligned32(OptionalData) == AMI_SIMPLE_BOOT_OPTION_SIGNATURE ){
			OptionalDataSize -= sizeof(UINT32);
			OptionalData++;
			Option->FwBootOption = TRUE;
		}else if ( ReadUnaligned32(OptionalData) == AMI_GROUP_BOOT_OPTION_SIGNATURE ){
			Option->FwBootOption = TRUE;
            Option->GroupHeader = TRUE;
			OptionalDataSize -= sizeof(UINT32);
			OptionalData++;
			while (    OptionalDataSize > AMI_NESTED_BOOT_OPTION_HEADER_SIZE){
				NESTED_BOOT_OPTION *NestedBootOption = (NESTED_BOOT_OPTION*)OptionalData;
				if (   NestedBootOption->Signature != AMI_NESTED_BOOT_OPTION_SIGNATURE
					|| NestedBootOption->Size > OptionalDataSize
				) break;
				CreateBootOptionsFromNvramOption(
					BootOptionList, BootOptionNumber, &NestedBootOption->Option,
					NestedBootOption->Size-AMI_NESTED_BOOT_OPTION_HEADER_SIZE,
					Priority
				);
				OptionalDataSize -= NestedBootOption->Size;
				OptionalData = (UINT32*)((UINT8*)OptionalData+NestedBootOption->Size);
			}
		}
	}

	if (OptionalDataSize==0){
		Option->OptionalData = NULL;
		Option->OptionalDataSize = 0;
	}else{
		Option->OptionalData = Malloc(OptionalDataSize);
		MemCpy(
			Option->OptionalData, OptionalData, OptionalDataSize
		);
		Option->OptionalDataSize = OptionalDataSize;
	}
	return Option;
}

/**
 * The function checks if boot option is well-formed
 *
 * @param NvramOption Pointer to the EFI_LOAD_OPTION structure
 * @param NvramOptionSize The size of the EFI_LOAD_OPTION, including the optional data section
 *
 * @retval TRUE boot option is valid, FALSE otherwise
*/
BOOLEAN IsBootOptionValid(EFI_LOAD_OPTION *NvramOption, UINTN NvramOptionSize){
    CHAR16 *Char;
    CHAR16 *EndOfDescription;
    EFI_DEVICE_PATH_PROTOCOL *Dp;
    UINTN DevicePathSize;

    // The boot option must have at least the header,
    // an empty (just the NULL-terminator) description,
    // and an empty device path (End-of-Device-Path node).
    if (NvramOption->FilePathListLength<sizeof(EFI_DEVICE_PATH_PROTOCOL)) return FALSE;
    if (NvramOptionSize < sizeof(*NvramOption)+sizeof(CHAR16)) return FALSE;
    NvramOptionSize -= sizeof(*NvramOption);

    if (  NvramOption->FilePathListLength >= NvramOptionSize ) return FALSE;
    NvramOptionSize -= NvramOption->FilePathListLength;

    // The description must include at least the NULL-terminator
    if (NvramOptionSize < sizeof(CHAR16)) return FALSE;

    // The description must be NULL-terminated
    Char = (CHAR16*)(NvramOption+1);
    EndOfDescription = (CHAR16*)((CHAR8*)Char+NvramOptionSize);
    while( *Char && Char < EndOfDescription) Char++;
    if (Char==EndOfDescription) return FALSE;

    // Validate the device path;
    Dp = (EFI_DEVICE_PATH_PROTOCOL*)(Char+1); // skip the terminating zero.
    DevicePathSize = NvramOption->FilePathListLength;

    while (TRUE){
        UINTN Length;
        if (EFI_ERROR(IsValidDevicePath(Dp))) return FALSE;
        Length = DPLength(Dp);
        if (Length > DevicePathSize) return FALSE;
        DevicePathSize -= Length;
		Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)Dp+Length);
        if (DevicePathSize < sizeof(EFI_DEVICE_PATH_PROTOCOL)){
            if (DevicePathSize != 0 ) return FALSE;
            //The last node must be an End-of-Device-Path node.
            Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)Dp - sizeof(EFI_DEVICE_PATH_PROTOCOL));
            return
                   isEndNode(Dp)
                && Dp->SubType == END_ENTIRE_SUBTYPE
                && NODE_LENGTH(Dp) == sizeof(EFI_DEVICE_PATH_PROTOCOL)
            ;
        }
    }
    //Should never reach this point.
    return FALSE;
}

/**
 * Read the boot options from NVRAM and create associated boot options in the master boot option list for each boot option
*/
VOID ReadBootOptions(){
	UINT16 *BootOrder = NULL;
	UINTN BootOrderSize = 0;
	EFI_STATUS Status;
	UINTN i;
	EFI_LOAD_OPTION *NvramOption = NULL;
	UINT32 Priority=IndexToBootPriority(0);
	UINTN NvramOptionSize;
    UINT16 MaxBootOptionNumber=0;

	Status=GetEfiVariable(
		L"BootOrder", &EfiVariableGuid, NULL, &BootOrderSize,(VOID**)&BootOrder
	);
	if (EFI_ERROR(Status)) return;
	for(i=0; i<BootOrderSize/sizeof(UINT16); i++){
		CHAR16 BootStr[9];

		// Get Boot Option
		Swprintf(BootStr,L"Boot%04X",BootOrder[i]);
		Status=GetEfiVariable(
			BootStr, &EfiVariableGuid, NULL, &NvramOptionSize,(VOID**)&NvramOption
		);
		if (EFI_ERROR(Status)){
			if (Status==EFI_NOT_FOUND){
				// Workaround for non-UEFI specification complaint OS.
				// Some OS create BootXXXX variables using lower case letters A-F.
				// Search for lower case boot option variable if no upper case variable found.
				Swprintf(BootStr,L"Boot%04x",BootOrder[i]);
				Status=GetEfiVariable(
					BootStr, &EfiVariableGuid, NULL, &NvramOptionSize,(VOID**)&NvramOption
				);
			}
			if (EFI_ERROR(Status)) continue;
		}
        //Let's verify if boot option is well-formed
        if (!IsBootOptionValid(NvramOption, NvramOptionSize)){
            TRACE((TRACE_DXE_CORE,"The boot option Boot%04X is ill-formed. Deleting...\n",
                   BootOrder[i]
            ));
			pRS->SetVariable(BootStr, &EfiVariableGuid, 0, 0, NULL);
            continue;
        }
		CreateBootOptionsFromNvramOption(
			BootOptionList, BootOrder[i], NvramOption, NvramOptionSize, &Priority
		);
        if (BootOrder[i] > MaxBootOptionNumber)
            MaxBootOptionNumber = BootOrder[i];
	}
	pBS->FreePool(NvramOption);
	pBS->FreePool(BootOrder);
}


/**
 * Prepend the masking pattern to the device path of the passed BOOT_OPTION
 *
 * @param Option The boot option which needs to have the device path updated
 */
VOID MaskFilePathList(BOOT_OPTION *Option){
    static struct {
		VENDOR_DEVICE_PATH vendor;
		EFI_DEVICE_PATH_PROTOCOL end;
	} MaskedDp = {
		{
          {HARDWARE_DEVICE_PATH, HW_VENDOR_DP, {sizeof(VENDOR_DEVICE_PATH), 0}},
		  AMI_MASKED_DEVICE_PATH_GUID
        },
		{END_DEVICE_PATH,END_ENTIRE_SUBTYPE,sizeof(EFI_DEVICE_PATH_PROTOCOL)}
	};
    static UINT32 MaskedDpLength = sizeof(MaskedDp);
    UINT8 *NewDp;

    NewDp = Malloc(Option->FilePathListLength + MaskedDpLength);
    if (NewDp==NULL) return;
    MemCpy(NewDp, &MaskedDp, MaskedDpLength);
    MemCpy(
        NewDp+MaskedDpLength, Option->FilePathList,
        Option->FilePathListLength
    );
    pBS->FreePool(Option->FilePathList);
    Option->FilePathList = (EFI_DEVICE_PATH_PROTOCOL*)NewDp;
    Option->FilePathListLength += MaskedDpLength;
}


/**
 * This function unmasks orphan devices. The orphan legacy devices are masked by the MaskOrphanDevices function.
 * TSE does not like legacy boot options for devices that are not currently attached to the system. To trick TSE
 * we we are camouflaging such boot options by adding a vendor GUIDed device path. The device path is added by
 * the MaskOrphanDevices function before saving, and is removed by UnmaskOrphanDevices immediately after reading
 */
VOID UnmaskOrphanDevices(){
	DLINK *Link;
    BOOT_OPTION *Option;
    VENDOR_DEVICE_PATH *MaskedDp;
    UINTN MaskedDpLength;

	FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        MaskedDp = (VENDOR_DEVICE_PATH*)Option->FilePathList;
        if (   MaskedDp->Header.Type != HARDWARE_DEVICE_PATH
            || MaskedDp->Header.SubType != HW_VENDOR_DP
            || guidcmp(&AmiMaskedDevicePathGuid, &MaskedDp->Guid) != 0
        ) continue;
        MaskedDpLength = DPLength(&MaskedDp->Header);
        if (Option->FilePathListLength <= MaskedDpLength) continue;
        Option->FilePathListLength -=MaskedDpLength;
        MemCpy(
            Option->FilePathList, (UINT8*)Option->FilePathList+MaskedDpLength,
            Option->FilePathListLength
        );
        if(!IsLegacyBootOption(Option))
            Option->Attributes &= ~LOAD_OPTION_HIDDEN;
    }
}

#ifdef CSM_SUPPORT
/**
 * Using the passed position, return the BBS index from the legacy dev order group
 *
 * @param Group The legacy device order
 * @param Position the index into the legacy dev order to return
 *
 * @retval UINT32 the BBS index from the the legacy dev order
 *
 * @note  This function is only available, and used, if CSM_SUPPORT token is defined and enabled
 */
UINT32 GetBbsIndexByPositionInTheGroup(
	LEGACY_DEVICE_ORDER *Group, UINT16 Position
){
	UINT16* EndOfGroup = (UINT16*)(
        (UINT8*)Group + Group->Length + sizeof(UINT32)
    );
	UINT16 *IndexPtr = &Group->Device[0];
    UINT32 Counter = 0;

	while(IndexPtr<EndOfGroup){
		if (Counter == Position)
			return *IndexPtr;
		IndexPtr++;
        Counter++;
	}
	return 0;
}

/**
 * Get the corresponding device index in the LEGACY_DEV_ORDER based on the bbs index number.  Legacy
 * dev order organizes the boot priorities of a class of devices (Cdrom, hdd, bev, etc) and the
 * individual indexes of the device are the relative priorities in which they should be used to
 * attempt a legacy boot.
 *
 * @param Group pointer to the legacy dev order group
 * @param BbsIndex index of the device trying to be matched
 *
 * @retval UINT32 the index of the device inside of the LEGACY_DEV_ORDER
 *
 * @note  function only available, and used, if CSM_SUPPORT token is defined and enabled
 */
UINT32 FindPositionInTheGroupByBbsIndex(
	LEGACY_DEVICE_ORDER *Group, UINT16 BbsIndex
){
	UINT16* EndOfGroup = (UINT16*)(
        (UINT8*)Group + Group->Length + sizeof(UINT32)
    );
	UINT16 *IndexPtr = &Group->Device[0];
    UINT32 Counter = 0;

	while(IndexPtr<EndOfGroup){
		if (*(UINT8*)IndexPtr == *(UINT8*)&BbsIndex)
			return Counter;
		IndexPtr++;
        Counter++;
	}
	return 0;
}

/**
 * Go through the legacy device order structure and find the legacy dev order index that matches the BBS index.
 *
 * @param DevOrder pointer to the legacy device order
 * @param DevOrderSize size of the legacy dev order structure
 * @param BbsIndex index of the BBS device to match
 * @retval LEGACY_DEVICE_ORDER *
 *
 * @note  function only available, and used, if CSM_SUPPORT token is defined and enabled
 */
LEGACY_DEVICE_ORDER* FindLegacyDeviceGroupByBbsIndex(
	LEGACY_DEVICE_ORDER *DevOrder, UINTN DevOrderSize, UINT16 BbsIndex
){
	UINT16 *EndOfDevOrder = (UINT16*)((UINT8*)DevOrder+DevOrderSize);
	LEGACY_DEVICE_ORDER* EndOfGroup = DevOrder;
	LEGACY_DEVICE_ORDER* GroupStart = DevOrder;
	UINT16 *IndexPtr = (UINT16*)EndOfGroup;

	while(IndexPtr<EndOfDevOrder){
		if (IndexPtr==(UINT16*)EndOfGroup){
			IndexPtr = &EndOfGroup->Device[0];
            GroupStart = EndOfGroup;
			EndOfGroup = (LEGACY_DEVICE_ORDER*)(
				(UINT8*)EndOfGroup + EndOfGroup->Length + sizeof(UINT32)
			);
		}
		if (*(UINT8*)IndexPtr == *(UINT8*)&BbsIndex) return GroupStart;
        IndexPtr++;
	}
	return NULL;
}

/**
 * Go through the legacy dev order structure and adjust it to match the current boot priorities
 *
 * @param BootOptionList the master boot option list
 *
 * @note Function only available, and used, if CSM_SUPPORT token is defined and enabled
 */
VOID AdjustLegacyBootOptionPriorities(){
	EFI_STATUS Status;
	UINTN DevOrderSize, OldDevOrderSize;
	LEGACY_DEVICE_ORDER *DevOrder=NULL, *OldDevOrder=NULL;
	DLINK *Link;
	UINT16 BootOptionNumber;
	UINT16 *IndexPtr;
	UINT16 *EndOfDeviceList;
    BOOT_OPTION *Option;
    UINT16 GroupSize=0;
    INT32 IndexInTheGroup=0;
    LEGACY_DEVICE_ORDER *CurrentGroup;

	Status = GetEfiVariable(
		L"LegacyDevOrder", &LegacyDevOrderGuid, NULL,
		&DevOrderSize, &DevOrder
	);
	if(EFI_ERROR(Status) || DevOrderSize < sizeof(LEGACY_DEVICE_ORDER)) return ;
	Status = GetEfiVariable(
		L"OldLegacyDevOrder", &LegacyDevOrderGuid, NULL,
		&OldDevOrderSize, &OldDevOrder
	);
	if(    EFI_ERROR(Status)
		|| OldDevOrderSize!=DevOrderSize
		|| MemCmp(DevOrder,OldDevOrder, DevOrderSize)==0
	){
		pBS->FreePool(DevOrder);
		if (!EFI_ERROR(Status)) pBS->FreePool(OldDevOrder);
		return ;
	}

	BootOptionNumber = INVALID_BOOT_OPTION_NUMBER;
	IndexPtr = (UINT16*)DevOrder;
	EndOfDeviceList = (UINT16*)((UINT8*)IndexPtr+DevOrderSize);

	FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        INT32 NewIndexInTheGroup;
        LEGACY_DEVICE_ORDER *OldGroup;
        UINT16 BbsIndex;

        if ( !IsLegacyBootOption(Option) || Option->GroupHeader) continue;
		// this should never happen during the normal course of operation
		if (IndexPtr>=EndOfDeviceList) break;
		// during the normal course of operation both conditions should
		// happen simultaneously
		if (BootOptionNumber != Option->BootOptionNumber || GroupSize==0){
			BootOptionNumber = Option->BootOptionNumber;
			IndexPtr += GroupSize;
            CurrentGroup = (LEGACY_DEVICE_ORDER*)IndexPtr;
			GroupSize = (CurrentGroup->Length)/sizeof(UINT16)-1;
			IndexPtr = &CurrentGroup->Device[0];
            IndexInTheGroup = 0;
		}
        OldGroup = FindLegacyDeviceGroupByBbsIndex(OldDevOrder,DevOrderSize,*IndexPtr);
        BbsIndex = GetBbsIndexByPositionInTheGroup(OldGroup,IndexInTheGroup);
        NewIndexInTheGroup = FindPositionInTheGroupByBbsIndex(CurrentGroup,BbsIndex);

        Option->Priority +=
            (NewIndexInTheGroup-IndexInTheGroup)*DEFAULT_PRIORITY_INCREMENT;
		if ((CurrentGroup->Device[NewIndexInTheGroup] & 0xff00)!=0)
            Option->Attributes &= ~LOAD_OPTION_ACTIVE;
        else
            Option->Attributes |= LOAD_OPTION_ACTIVE;
		IndexPtr++;
		GroupSize--;
        IndexInTheGroup++;
	}
	pBS->FreePool(DevOrder);
	pBS->FreePool(OldDevOrder);
}

/**
 * Go through the master boot option list and create memory representation of the legacy dev order variable
 *
 * @param BootOptionList the master boot option list
 *
 * @note  function only available, and used, if CSM_SUPPORT token is defined and enabled
 */
VOID BuildLegacyDevOrderBuffer(
    LEGACY_DEVICE_ORDER **DevOrderBuffer, UINTN *BufferSize
){
	UINTN DevOrderSize;
	LEGACY_DEVICE_ORDER *DevOrder, *GroupPtr;
    UINT16 *DevPtr;
	DLINK *Link;
    UINT16 BootOptionNumber = INVALID_BOOT_OPTION_NUMBER;
    BOOT_OPTION *Option;

    if (DevOrderBuffer == NULL || BufferSize == NULL) return;
    if (BootOptionList->Size == 0)
        DevOrder = Malloc(sizeof(LEGACY_DEVICE_ORDER));
    else
        DevOrder = Malloc(BootOptionList->Size*sizeof(LEGACY_DEVICE_ORDER));
    GroupPtr = DevOrder;
    DevPtr = (UINT16*)DevOrder;

    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
		if ( !IsLegacyBootOption(Option) || Option->GroupHeader) continue;
        if (Option->BootOptionNumber!=BootOptionNumber){
            GroupPtr->Length = (UINT16)((UINT8*)DevPtr - (UINT8*)&GroupPtr->Length);
            GroupPtr = (LEGACY_DEVICE_ORDER*)DevPtr;
            BootOptionNumber = Option->BootOptionNumber;
            GroupPtr->Type = GetLegacyDevOrderType(Option);
            DevPtr = GroupPtr->Device;
        }
        if ((Option->Attributes&LOAD_OPTION_ACTIVE)==0)
            *DevPtr = Option->BbsIndex | 0xff00;
        else
            *DevPtr = Option->BbsIndex;
        DevPtr++;
    }
    GroupPtr->Length = (UINT16)((UINT8*)DevPtr - (UINT8*)&GroupPtr->Length);
    DevOrderSize = (UINT8*)DevPtr - (UINT8*)DevOrder;

	*DevOrderBuffer = DevOrder;
    *BufferSize = DevOrderSize;
}

/**
 * This function masks orphan devices before saving them. TSE does not like legacy boot
 * options for devices that are not in the system. To trick TSE we we are camouflaging
 * such boot options by adding GUIDed device path. The device path is added by
 * MaskOrphanDevices function right before saving and removed by UnmaskOrphanDevices
 * right after reading
 *
 * @param BootOptionList the master boot option list
 *
 * @note  function only available, and used, if CSM_SUPPORT token is defined and enabled
 */
VOID MaskOrphanDevices(){
	DLINK *Link;
    BOOT_OPTION *Option;
    BOOT_OPTION *GroupHeader = NULL;

    SortList(BootOptionList, ComparePriorityThenBootOptionNumber);
	FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        if (!IsLegacyBootOption(Option))
            continue;
        if (Option->GroupHeader){
            if (GroupHeader!=NULL) MaskFilePathList(GroupHeader);
            GroupHeader=Option;
            continue;
        }
        if (IsBootOptionWithDevice(Option)){
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.		
            if (GroupHeader!=NULL && Option->BootOptionNumber==GroupHeader->BootOptionNumber)
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
                GroupHeader=NULL;
            continue;
        }
        MaskFilePathList(Option);
    }
    if (GroupHeader!=NULL) MaskFilePathList(GroupHeader);
}

/**
 * Go through the master boot option list and use it to update the legacy dev order variable
 *
 * @param BootOptionList the master boot option list
 *
 * @note  function only available, and used, if CSM_SUPPORT token is defined and enabled
**/
VOID SaveLegacyDevOrder(){
	UINTN DevOrderSize;
	LEGACY_DEVICE_ORDER *DevOrder;

    SortList(BootOptionList, ComparePriorityThenBootOptionNumber);
    BuildLegacyDevOrderBuffer(&DevOrder, &DevOrderSize);
    SetVariableWithNewAttributes(
        L"LegacyDevOrder", &LegacyDevOrderGuid,
        GetSetupVariablesAttributes(),DevOrderSize,DevOrder
	);
    SetVariableWithNewAttributes(
        L"OldLegacyDevOrder", &LegacyDevOrderGuid,
        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,DevOrderSize,DevOrder
	);
	pBS->FreePool(DevOrder);
}

/**
 * Go through the BBS table and create a entry in the master boot order list for each
 * BBS table entry
 *
 * @param BootDeviceList the master boot list
 *
 * @note  function only available, and used, if CSM_SUPPORT token is defined and enabled
 */
VOID CollectBbsDevices(DLIST *BootDeviceList){
    EFI_STATUS Status;
    EFI_LEGACY_BIOS_PROTOCOL *LegacyBios;
    UINT16 HddCount;
    UINT16 BbsCount;
    HDD_INFO *HddInfo;
    BBS_TABLE *BbsTable;
    UINT16 i;

    Status = pBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
    if(EFI_ERROR(Status)) return ;
	LegacyBios->GetBbsInfo(LegacyBios, &HddCount, &HddInfo, &BbsCount, &BbsTable);

    for(i = 0; i < BbsCount; i++){
		if(BbsTable[i].BootPriority == 0xffff) continue;
        CreateBootDevice(
            BootDeviceList,
            (EFI_HANDLE)*(UINTN*)&BbsTable[i].IBV1,
            i,&BbsTable[i]
        );
    }
}

#endif //#ifdef CSM_SUPPORT

/**
 * Collect a list of all handles with a particular protocol installed on them and create a boot device for each handle.
 *
 * @param BootDeviceList the master boot list that needs boot devices added to it.
 * @param ProtocolGuid the protocol to use when getting a list of device handles
 */
VOID CollectProtocolDevices(DLIST *BootDeviceList, EFI_GUID *ProtocolGuid){
    EFI_HANDLE *Devices;
    UINTN NumberOfDevices;
	EFI_STATUS Status;
	UINTN i;

    Status = pBS->LocateHandleBuffer(
        ByProtocol,ProtocolGuid, NULL,
        &NumberOfDevices, &Devices
    );
    if (EFI_ERROR(Status)) return;
	for(i=0; i<NumberOfDevices; i++){
		CreateBootDevice(BootDeviceList,Devices[i],INVALID_BBS_INDEX,NULL);
	}
	pBS->FreePool(Devices);
}

/**
 * Helper function to generate a master boot list based on the load file protocol,
 * the simple file system protocol and, if CSM support is enabled, the legacy bbs
 * table
 */
VOID CollectBootDevices(){
	CollectProtocolDevices(BootDeviceList,&gEfiLoadFileProtocolGuid);
	CollectProtocolDevices(BootDeviceList,&gEfiSimpleFileSystemProtocolGuid);
#ifdef CSM_SUPPORT
	CollectBbsDevices(BootDeviceList);
#endif
}

/**
 * Determine if this boot device is a UEFI HDD
 *
 * @param Device the device in question
 *
 * @retval BOOLEAN TRUE - Device is a UEFI HDD Boot Device and it should be removed from the boot order list
 * @retval FALSE Device is not a UEFI HDD and it should be left in the boot order
 */
BOOLEAN IsUefiHddBootDevice(BOOT_DEVICE *Device){
    EFI_BLOCK_IO_PROTOCOL *BlkIo;
    EFI_STATUS Status;

    if (   Device->DeviceHandle == INVALID_HANDLE
        || Device->BbsEntry != NULL
    ) return FALSE;


    Status=pBS->HandleProtocol(
        Device->DeviceHandle, &gEfiBlockIoProtocolGuid, (VOID**)&BlkIo
    );
    if (EFI_ERROR(Status)) return FALSE;
    return !BlkIo->Media->RemovableMedia;
}

/**
 * Master filter handler.  Function will call all ELINK functions linked into the
 * BootOptionBootDeviceFilteringFunctions list. If any ELINK function returns FALSE, the device will
 * be removed from the function list
 */
VOID FilterBootDeviceList(){
// Filter Devices
    BOOT_DEVICE *Device;
    DLINK *Link;

    FOR_EACH_BOOT_DEVICE(BootDeviceList, Link, Device){
	    UINT32 i;
	    for(i=0; FilteringFunction[i]!=NULL; i++)
		    if (FilteringFunction[i](Device)){
                DeleteBootDevice(BootDeviceList, Device);
                break;
            }
    }
}

/**
 * Attempt to match the Boot Option Device path to this device by locating the handle associated
 * with the passed device path and comparing it against the BOOT_DEVICE handle.
 *
 * @param OptionDevicePath The device path to match
 * @param Device The device to match against
 *
 * @retval TRUE The device Path matches the device
 * @retval FALSE The device is different than the device
 */
BOOLEAN LocateDevicePathTest(EFI_DEVICE_PATH_PROTOCOL *OptionDevicePath, BOOT_DEVICE *Device){
	EFI_STATUS Status;
	EFI_HANDLE Handle;

	if (Device->DeviceHandle==INVALID_HANDLE) return FALSE;
    Status=pBS->LocateDevicePath(
		&gEfiDevicePathProtocolGuid, &OptionDevicePath, &Handle
	);
	if (EFI_ERROR(Status)) return FALSE;
    if (Handle != Device->DeviceHandle) return FALSE;
    if (isEndNode(OptionDevicePath)) return TRUE;
    if (OptionDevicePath->Type != MEDIA_DEVICE_PATH) return FALSE;
    return    OptionDevicePath->SubType==MEDIA_FILEPATH_DP
           || OptionDevicePath->SubType==MEDIA_FV_FILEPATH_DP;
}

/**
 * Attempt to match the passed device path to the passed boot device. Because
 * of partial device paths that some OSes use, this requires extra logic perform.
 *
 * @param OptionDevicePath The device path to check
 * @param Device The boot device
 *
 * @retval TRUE The device path matches the device
 * @retval FALSE The device path does not match the device
 */
BOOLEAN PartitionDevicePathTest(
	EFI_DEVICE_PATH_PROTOCOL *OptionDevicePath, BOOT_DEVICE *Device
){
	EFI_STATUS Status;
	EFI_BLOCK_IO_PROTOCOL *BlockIo;
	EFI_DEVICE_PATH_PROTOCOL *PartitionDevicePath;
	HARDDRIVE_DEVICE_PATH* BootParitionDevicePath;
	HARDDRIVE_DEVICE_PATH* PartitionNode;

	if (Device->DeviceHandle==INVALID_HANDLE) return FALSE;
	if (    OptionDevicePath->Type!=MEDIA_DEVICE_PATH
         || OptionDevicePath->SubType!=MEDIA_HARDDRIVE_DP
    ) return FALSE;
	BootParitionDevicePath  = (HARDDRIVE_DEVICE_PATH*)OptionDevicePath;
	Status = pBS->HandleProtocol(
		Device->DeviceHandle,&gEfiBlockIoProtocolGuid,(VOID**)&BlockIo
	);
	if (EFI_ERROR(Status)) return FALSE;
    // if this is not partition, continue
	if (!BlockIo->Media->LogicalPartition) return FALSE;
	Status = pBS->HandleProtocol(
		Device->DeviceHandle,&gEfiDevicePathProtocolGuid,(VOID**)&PartitionDevicePath
	);
    if (EFI_ERROR(Status)) return FALSE;
	// Get last node of the device path. It should be partition node
	PartitionNode = (HARDDRIVE_DEVICE_PATH*)DPGetLastNode(PartitionDevicePath);
	//Check if our partition matches Boot partition
	if (   PartitionNode->Header.Type!=MEDIA_DEVICE_PATH
		|| PartitionNode->Header.SubType!=MEDIA_HARDDRIVE_DP
	) return FALSE;
	if (   PartitionNode->PartitionNumber==BootParitionDevicePath->PartitionNumber
		&& PartitionNode->SignatureType==BootParitionDevicePath->SignatureType
		&& !MemCmp(PartitionNode->Signature,BootParitionDevicePath->Signature,16)
	){
		return TRUE;
	}
	return FALSE;
}

/**
 * Attempt to locate a device path node in the device path that matches the passed type, and
 * return the sub-type of that node
 *
 * @param Dp The device path to search
 * @param Type The type to search for
 *
 * @retval Zero No node was found with the Type
 * @retval Non-Zero A node was found the sub-type was returned
 */
UINT8 GetDevicePathSubtype(EFI_DEVICE_PATH_PROTOCOL *Dp, UINT8 Type){
    UINT8 SubType;

    if (Dp == NULL) return 0;
    SubType = 0;

    for( ; !(isEndNode(Dp)); Dp=NEXT_NODE(Dp))
        if (Dp->Type == Type) SubType = Dp->SubType;
    return SubType;
}

/**
 * Attempt to locate a device path node in the passed device path of type Type. 
 * If DevicePath contains multiple nodes of type Type, the last node is returned.
 *
 * @param Dp The device path to search
 * @param Type The type of node to search for
 *
 * @retval NULL No node was found
 * @retval Non-null A node was found, and the pointer was returned. 
 *         If DevicePath contains multiple nodes of type Type, the last node is returned.
 */
EFI_DEVICE_PATH_PROTOCOL* GetLastDevicePathNodeOfType(EFI_DEVICE_PATH_PROTOCOL *Dp, UINT8 Type){
	EFI_DEVICE_PATH_PROTOCOL *Node = NULL;

    if (Dp == NULL) return NULL;

    for( ; !(isEndNode(Dp)); Dp=NEXT_NODE(Dp))
        if (Dp->Type == Type) Node = Dp;
    return Node;
}

/**
 * Attempt to locate a device path node in the passed device path of type Type.
 * If DevicePath contains multiple nodes of type Type, the first node is returned.
 *
 * @param Dp The device path to search
 * @param Type The type of node to search for
 *
 * @retval NULL No node was found
 * @retval Non-null A node was found, and the pointer was returned.
 *         If DevicePath contains multiple nodes of type Type, the first node is returned.
 */
EFI_DEVICE_PATH_PROTOCOL* GetFirstDevicePathNodeOfType(EFI_DEVICE_PATH_PROTOCOL *Dp, UINT8 Type){
    if (Dp == NULL) return NULL;

    for( ; !(isEndNode(Dp)); Dp=NEXT_NODE(Dp))
        if (Dp->Type == Type) return Dp;
    return NULL;
}

/**
 * Attempt to match the passed device path to the passed BOOT_DEVICE. Handle the partial device
 * path cases.
 *
 * @param OptionDevicePath The device path to match
 * @param Device The device to attempt to match to the device path
 *
 * @retval TRUE The device path matches the device
 * @retval FALSE The device path does not match the device
*/
BOOLEAN DeviceTypeDevicePathTest(EFI_DEVICE_PATH_PROTOCOL *OptionDevicePath, BOOT_DEVICE *Device){
	EFI_STATUS Status;
    EFI_DEVICE_PATH_PROTOCOL *Dp;
    UINT8 DeviceInterface, OptionInterface;
    EFI_DEVICE_PATH *DeviceMedia, *OptionMedia;

	if (Device->DeviceHandle==INVALID_HANDLE){
	    AMI_BBS_DEVICE_PATH *AmiBbsDp;

	    if (Device->BbsEntry==NULL) return FALSE;
	    AmiBbsDp = (AMI_BBS_DEVICE_PATH*)OptionDevicePath;
	    return AmiBbsDp->Header.Header.Type == HARDWARE_DEVICE_PATH
	        && AmiBbsDp->Header.Header.SubType == HW_VENDOR_DP
	        && guidcmp(&AmiBbsDevicePathGuid, &AmiBbsDp->Header.Guid) == 0
	        && AmiBbsDp->Class == Device->BbsEntry->Class
	        && AmiBbsDp->SubClass == Device->BbsEntry->SubClass;
	}

    Status=pBS->HandleProtocol(
        Device->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID**)&Dp
    );
    if (EFI_ERROR(Status)) return FALSE;

    DeviceInterface = GetDevicePathSubtype(
        Dp, MESSAGING_DEVICE_PATH
    );
    //If DeviceInterface is 0, the interface type is unknown.
    //Can't do the matching.
    if (DeviceInterface == 0) return FALSE;
    if (DeviceInterface == MSG_SATA_DP)
        DeviceInterface = MSG_ATAPI_DP;
    OptionInterface = GetDevicePathSubtype(
        OptionDevicePath, MESSAGING_DEVICE_PATH
    );
    if (OptionInterface == MSG_SATA_DP)
        OptionInterface = MSG_ATAPI_DP;
    if (DeviceInterface != OptionInterface) return FALSE;

    DeviceMedia = GetLastDevicePathNodeOfType(Dp,MEDIA_DEVICE_PATH);
    OptionMedia = GetLastDevicePathNodeOfType(OptionDevicePath,MEDIA_DEVICE_PATH);
    if (   DeviceMedia != NULL && OptionMedia != NULL
    	&& NODE_LENGTH(DeviceMedia) == NODE_LENGTH(OptionMedia)
    	&& MemCmp(DeviceMedia,OptionMedia,NODE_LENGTH(DeviceMedia)) == 0
    ){
        NormalizeBootOptionDevicePath = TRUE;
        return TRUE;
    }

    return FALSE;
}

#define MSG_USB_DP_PRESENT            0x1
#define MSG_USB_CLASS_DP_PRESENT      0x2
#define MSG_USB_WWID_CLASS_DP_PRESENT 0x4

/**
 * Check if the passed device path corresponds to a USB device
 *
 * @param Dp The device path to check for belonging to a USB device
 * @param AvailableNodes OPTIONAL A bitmask of the USB device nodes encountered while parsing the device path
 *
 * @retval TRUE The device path is a USB device
 * @retval FALSE The device path is not a USB device
 */
BOOLEAN IsUsbDp(
    IN EFI_DEVICE_PATH_PROTOCOL *Dp,
    OUT UINT32 *AvailableNodes OPTIONAL
)
{
    UINT32 Flags = 0;

    for( ; !(isEndNode(Dp)); Dp = NEXT_NODE(Dp)) {
        if(Dp->Type == MESSAGING_DEVICE_PATH) {
            if(Dp->SubType == MSG_USB_DP) {
                Flags |= MSG_USB_DP_PRESENT;
                continue;
            }
            if(Dp->SubType == MSG_USB_CLASS_DP) {
                Flags |= MSG_USB_CLASS_DP_PRESENT;
                continue;
            }
            if(Dp->SubType == MSG_USB_WWID_CLASS_DP) {
                Flags |= MSG_USB_WWID_CLASS_DP_PRESENT;
            }
        }
    }
    if(AvailableNodes != NULL)
        *AvailableNodes = Flags;

    return (Flags != 0) ? TRUE : FALSE;
}

/**
 * Attempt to match the Device path to the passed Device of type USB
 *
 * @param OptionDevicePath The device path to attempt to match
 * @param Device The device to attempt to match
 *
 * @retval TRUE The device path matches the device
 * @retval FALSE The device path does not match the device
 */
BOOLEAN UsbClassDevicePathTest(
    IN EFI_DEVICE_PATH_PROTOCOL *OptionDevicePath,
    IN BOOT_DEVICE *Device
)
{
    EFI_STATUS Status;
    USB_CLASS_DEVICE_PATH *UsbDp;
    EFI_DEVICE_PATH_PROTOCOL *DevDp;
    UINT32 AvailableNodes;

    if(OptionDevicePath->Type != MESSAGING_DEVICE_PATH ||
       OptionDevicePath->SubType != MSG_USB_CLASS_DP)
        return FALSE;           //boot option is not USB class boot option

    Status = pBS->HandleProtocol(Device->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID**)&DevDp);
    if(EFI_ERROR(Status))
        return FALSE;           //device doesn't support EFI_DEVICE_PATH protocol

    if(!IsUsbDp(DevDp, &AvailableNodes))
        return FALSE;           //device is not a USB device

    UsbDp = (USB_CLASS_DEVICE_PATH *)OptionDevicePath;
    return (UsbDp->VendorId == 0xffff &&
                UsbDp->ProductId == 0xffff &&
                UsbDp->DeviceClass == 0xff &&
                UsbDp->DeviceSubClass == 0xff &&
                UsbDp->DeviceProtocol == 0xff) ? TRUE : FALSE;

	// We don't support matching of a USB Class device path node to a specific USB device.
	// We only support blanket matching to any USB device as per
    // the UEFI 2.4 specification, Section 3.1.2:
    //"If any of the ID, Product ID, Device Class, Device Subclass, or Device Protocol contain all F's 
    // (0xFFFF or 0xFF), this element is skipped for the purpose of matching."
}

#ifdef CSM_SUPPORT
/**
 * Attempt to match the passed boot option device path to a boot device (of BBS type)
 *
 * @param OptionDevicePath The device path to attempt to match
 * @param Device The device to attempt to match it to
 *
 * @retval TRUE The device path matches the device
 * @retval FALSE The device path does not match the device
 *
 * @note  function only available, and used, if CSM_SUPPORT token is defined and enabled
 */
BOOLEAN BbsDevicePathTest(
	EFI_DEVICE_PATH_PROTOCOL *OptionDevicePath, BOOT_DEVICE *Device
){
    BBS_BBS_DEVICE_PATH *BbsDp;
	if (OptionDevicePath->Type!=BBS_DEVICE_PATH) return FALSE;
	if (Device->BbsEntry==NULL) return FALSE;
    BbsDp = (BBS_BBS_DEVICE_PATH*)OptionDevicePath;
	return     BbsDp->DeviceType == GetBbsEntryDeviceType(Device->BbsEntry);

}

/**
 * Go through the current BbsTable, passed in via the BbsEntry parameter, and attempt to find an entry
 * that matches the current entry
 *
 * @param BbdsIndex The current index of the BBS device
 * @param BbdsEntry Pointer to the BBS table
 *
 * @retval UINT8 The index that was matched
 */
UINT8 GetBbsDeviceInstance(	UINT16 BbsIndex, BBS_TABLE *BbsEntry){
	UINT8 Bus;
	UINT8 Device;
	UINT8 Function;
	UINT8 Class;
    UINT8 SubClass;
    UINT16 Index;

    if (BbsIndex==0) return 0;
    Bus = BbsEntry->Bus;
    Device = BbsEntry->Device;
    Function = BbsEntry->Function;
    Class = BbsEntry->Class;
    SubClass = BbsEntry->SubClass;
    BbsEntry -= BbsIndex;

    for(  Index = BbsIndex-1
        ; Index != 0xFFFF
        ; Index--
    ){
        if(    Bus != BbsEntry[Index].Bus
            || Device != BbsEntry[Index].Device
            || Function != BbsEntry[Index].Function
            || Class != BbsEntry[Index].Class
            || SubClass != BbsEntry[Index].SubClass
        ) return BbsIndex - 1 - Index;
    }
    return BbsIndex - 1 - Index;
}

/**
 * Attempt to match the passed device path to a device for devices of type AMI_BBS_DEVICE_PATH
 *
 * @param OptionDevicePath The device path to attempt to match
 * @param Device The device to attempt to match
 *
 * @retval TRUE The device matches the device path
 * @retval FALSE The device patch does not match the device
 */
BOOLEAN AmiBbsDevicePathTest(
	EFI_DEVICE_PATH_PROTOCOL *OptionDevicePath, BOOT_DEVICE *Device
){
    AMI_BBS_DEVICE_PATH *AmiBbsDp;

	if (Device->BbsEntry==NULL) return FALSE;
    AmiBbsDp = (AMI_BBS_DEVICE_PATH*)OptionDevicePath;
    return AmiBbsDp->Header.Header.Type == HARDWARE_DEVICE_PATH
        && AmiBbsDp->Header.Header.SubType == HW_VENDOR_DP
        && guidcmp(&AmiBbsDevicePathGuid, &AmiBbsDp->Header.Guid) == 0
        && AmiBbsDp->Bus == Device->BbsEntry->Bus
        && AmiBbsDp->Device == Device->BbsEntry->Device
        && AmiBbsDp->Function == Device->BbsEntry->Function
        && AmiBbsDp->Class == Device->BbsEntry->Class
        && AmiBbsDp->SubClass == Device->BbsEntry->SubClass
        && AmiBbsDp->Instance == GetBbsDeviceInstance(Device->BbsIndex,Device->BbsEntry);
    ;
}
#endif

/**
 * Attempt to match the passed device path to the device by calling all functions elinked into
 * the list of device path matching functions
 *
 * @param OptionDp The device path to attempt to match
 * @param Device The device to attempt to match
 *
 * @retval TRUE The device matches the device path
 * @retval FALSE The device patch does not match the device
 */
BOOLEAN MatchDevicePathToDevice(EFI_DEVICE_PATH_PROTOCOL *OptionDp, BOOT_DEVICE *Device){
	UINT32 i;
	for(i=0; DpMatchingFunction[i]!=NULL; i++)
		if (DpMatchingFunction[i](OptionDp,Device)) return TRUE;
	return FALSE;
}

/**
 * Function to ensure that a UEFI boot option is not matched to a legacy boot device. This case occurs
 * especially with Floppy devices because of the lack of partition device paths for floppy devices
 *
 * @param Option The boot option
 * @param Device The boot device
 *
 * @return TRUE The boot option and boot device are a legacy device
 * @return FALSE The boot option and boot device are not a leagcy device
 */
BOOLEAN MatchUefiFloppyDrive(BOOT_OPTION *Option, BOOT_DEVICE *Device){
    // This function makes sure that UEFI boot option is not matched
    // with the legacy boot device.
    // A legacy boot device is a device with a valid BbsIndex.
    // A UEFI boot option is a boot option with FilePathList does not start
    // with the BBS device path.
    // When this function returns FALSE, the matching process fails.
    // The function is executed via the BootOptionMatchingFunctions eLink.
    return Device->BbsIndex==INVALID_BBS_INDEX || IsLegacyBootOption(Option);
}

/**
 * Attemp to match the boot option to the boot device by calling all the matching functions
 * elinked inot the list of MatchingFunctions
 *
 * @param Option The boot option
 * @param Device The boot device
 *
 * @return TRUE The boot option and boot device are a legacy device
 * @return FALSE The boot option and boot device are not a leagcy device
 */
BOOLEAN MatchBootOpionToDevice(BOOT_OPTION *Option, BOOT_DEVICE *Device){
	UINT32 i;
    //all the functions have to return TRUE
	for(i=0; MatchingFunction[i]!=NULL; i++)
		if (!MatchingFunction[i](Option,Device)) return FALSE;
	return TRUE;
}

/**
 * After collecting all the boot devices, and reading all the existing boot options, go through both
 * lists and attempt to match each boot device to a boot option.
 */
VOID MatchBootOptionsToDevices(){
	DLINK *OptionLink, *DeviceLink;
    BOOT_OPTION *Option;
    BOOT_DEVICE *Device;

    FOR_EACH_BOOT_DEVICE(BootDeviceList,DeviceLink,Device){
        BOOLEAN DeviceIsMatched = FALSE;
        FOR_EACH_BOOT_OPTION(BootOptionList,OptionLink,Option){
			EFI_DEVICE_PATH_PROTOCOL *OptionDp = Option->FilePathList;
			EFI_DEVICE_PATH_PROTOCOL *DpInstance;
            //Skip the group headers and the options that have already been matched.
            if (Option->GroupHeader || IsBootOptionWithDevice(Option)) continue;
			do {
                EFI_DEVICE_PATH_PROTOCOL *TmpDp = OptionDp;
				while(   (DpInstance = DPNextInstance(&TmpDp,NULL))!=NULL
					  && MatchDevicePathToDevice(DpInstance,Device)
				) pBS->FreePool(DpInstance);

				OptionDp = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)OptionDp+DPLength(OptionDp));
			} while(   Option->FwBootOption && DpInstance==NULL
				    && (UINT8*)OptionDp<(UINT8*)Option->FilePathList+Option->FilePathListLength
			);
			if (DpInstance == NULL && MatchBootOpionToDevice(Option,Device)){
				UpdateBootOptionWithBootDeviceInfo(Option,Device);
                DeviceIsMatched = TRUE;
                // Legacy device can't be matched with more than one boot option.
                if (IsLegacyBootOption(Option)) break;
			}else{
				if (DpInstance!=NULL) pBS->FreePool(DpInstance);
			}
		}
        if (DeviceIsMatched) DeleteBootDevice(BootDeviceList, Device);
	}
}

/**
 * Go through the list of boot devices and delete entries for UEFI HDDs entries. These are considered
 * fixed media devices, according to the UEFI specification, and should not be dynamically recreated.
 */
VOID DeleteUnmatchedUefiHddBootDevices(){
    BOOT_DEVICE *Device;
    DLINK *Link;

    FOR_EACH_BOOT_DEVICE(BootDeviceList, Link, Device){
        if (IsUefiHddBootDevice(Device)){
            DeleteBootDevice(BootDeviceList, Device);
        }
    }
}

/**
 * Get the parent Handle for the Block IO Handle passed. This is necessary because each
 * partition can have its own Block I/O instance that will just allow communicating with that
 * partition. This function will get the Block I/O Instance from the parent device that the partition
 * is encompassed by.
 *
 * @param BlockIoHandle The Block I/O Handle that may be from a partition
 *
 * @retval EFI_HANDLE The handle of the parent Block I/O Device
 */
EFI_HANDLE GetPhysicalBlockIoHandle(EFI_HANDLE BlockIoHandle){
    EFI_BLOCK_IO_PROTOCOL *BlkIo;
    EFI_DEVICE_PATH_PROTOCOL *DevicePath, *Dp;
    EFI_STATUS Status;
    EFI_HANDLE Handle = BlockIoHandle;

    Status=pBS->HandleProtocol(
        Handle, &gEfiBlockIoProtocolGuid, (VOID**)&BlkIo
    );
    if (EFI_ERROR(Status)) return Handle;
    if (!BlkIo->Media->LogicalPartition) return Handle;
    Status=pBS->HandleProtocol(
        Handle, &gEfiDevicePathProtocolGuid, (VOID**)&DevicePath
    );
    if (EFI_ERROR(Status)) return Handle;
    Dp=DevicePath;
    while(BlkIo->Media->LogicalPartition){
        EFI_DEVICE_PATH_PROTOCOL *PrevDp=Dp;
        //We need to cut Devicepath node to get Phisycal Partition
        Dp=DPCut(PrevDp);
        if (PrevDp!=DevicePath) pBS->FreePool(PrevDp);
        if (Dp == NULL) break;
        PrevDp=Dp;
        Status=pBS->LocateDevicePath(
            &gEfiBlockIoProtocolGuid,&PrevDp,&Handle
        );
        if(EFI_ERROR(Status)) break;
        Status=pBS->HandleProtocol(
            Handle, &gEfiBlockIoProtocolGuid, (VOID**)&BlkIo
        );
        if(EFI_ERROR(Status)) break;
    }
    if (Dp!=NULL && Dp!=DevicePath) pBS->FreePool(Dp);
    //if physical Block I/O handle is not found, return original handle
    return (BlkIo->Media->LogicalPartition) ? BlockIoHandle : Handle;
}

/**
 * Return the handle of the device for this child handle
 *
 * @param[in] Handle The child handle
 * @param[out] IpType The IP type (IP4/IP6) that this handle corresponds to.
 * @param[out] Uri TRUE if this an URI device path.
 *
 * @return EFI_HANDLE the handle of the network controller itself.
 */
EFI_HANDLE GetPhysicalNetworkCardHandle(EFI_HANDLE Handle, UINT8 *IpType, BOOLEAN *Uri){
    EFI_LOAD_FILE_PROTOCOL *LoadFile;
    EFI_DEVICE_PATH_PROTOCOL *DevicePath, *Dp, *TmpDevicePath, *TmpDp, *MsgDp;
    EFI_STATUS Status;
    EFI_HANDLE NewHandle;

    Status=pBS->HandleProtocol(
        Handle, &gEfiLoadFileProtocolGuid, (VOID**)&LoadFile
    );
    if (EFI_ERROR(Status)) return Handle;

    Status=pBS->HandleProtocol(
        Handle, &gEfiDevicePathProtocolGuid, (VOID**)&DevicePath
    );
    if (EFI_ERROR(Status)) return Handle;
    MsgDp = GetFirstDevicePathNodeOfType(DevicePath, MESSAGING_DEVICE_PATH);
    if (MsgDp == NULL || MsgDp->SubType != MSG_MAC_ADDR_DP) return Handle;
    
    MsgDp=NEXT_NODE(MsgDp);
    if (isEndNode(MsgDp)) return Handle;
    TmpDevicePath = TmpDp = DPCopy(DevicePath);
    Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)MsgDp - (UINT8*)DevicePath + (UINT8*)TmpDp);
    Dp->Type = END_DEVICE_PATH;
    Dp->SubType = END_ENTIRE_SUBTYPE;
    SET_NODE_LENGTH(Dp,sizeof(*Dp));
    Status = pBS->LocateDevicePath(&gEfiDevicePathProtocolGuid, &TmpDp, &NewHandle);
    pBS->FreePool(TmpDevicePath);
    if (EFI_ERROR(Status) || TmpDp!=Dp) return Handle;
    Handle = NewHandle;
    if ( IpType != NULL) *IpType = 0;
    if (Uri != NULL) *Uri = FALSE;
    
    MsgDp = GetFirstDevicePathNodeOfType(MsgDp, MESSAGING_DEVICE_PATH);
    if (MsgDp == NULL) return Handle;
    if (IpType != NULL) *IpType = MsgDp->SubType;

    if (Uri != NULL){
        MsgDp = GetFirstDevicePathNodeOfType(NEXT_NODE(MsgDp), MESSAGING_DEVICE_PATH);
        if (MsgDp != NULL && MsgDp->SubType == MSG_URI_DP) *Uri = TRUE;
    }

    return Handle;
}

/**
 * Remove any trailing spaces from the passed parameter
 *
 * @param Name The string to remove the trailing spaces
 * @param NumberOfCharacters The number of characters in the string
 *
 * @retval the new length of the string
 */
UINTN RemoveTrailingSpaces(CHAR16 *Name, UINTN NumberOfCharacters){
    //remove trailing spaces
    while(NumberOfCharacters>0 && Name[NumberOfCharacters-1]==L' ')
        NumberOfCharacters--;
    Name[NumberOfCharacters]=0;
    return NumberOfCharacters;
}

/**
 * Create a string description for the boot option pointed to by BOOT_OPTION.
 *
 * @param Option The Boot option to create the string description
 * @param Name The buffer to fill with the description string
 * @param NameSize The size of the buffer for the description string
 *
 * @retval Number of Unicode characters printed into the Name buffer excluding the terminating zero.
 */
UINTN ConstructBootOptionNameByHandle(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
){
    EFI_HANDLE Handle;
    CHAR16 *ControllerName;
    CHAR16 PrefixBuffer[20] = {L'\0'};
    CHAR16 PrefixBuffer2[20] = {L'\0'};
    UINTN  PrefixSize;
    CHAR16 *Prefix = PrefixBuffer;
    CHAR16 *Prefix2 = PrefixBuffer2;
    UINTN  NumberOfCharacters;
    UINT8  IpType;
    BOOLEAN HttpBoot;

    if (Option->DeviceHandle == INVALID_HANDLE) return 0;

    //Name from Controller Handle
    Handle = GetPhysicalBlockIoHandle(Option->DeviceHandle);
    if (Handle==Option->DeviceHandle){
        EFI_HANDLE OldHandle = Handle;
        Handle = GetPhysicalNetworkCardHandle(Option->DeviceHandle,&IpType,&HttpBoot);
        if (Handle != OldHandle) {
            PrefixSize = sizeof(PrefixBuffer)/sizeof(CHAR16);
            
            if ( IpType == MSG_IPv4_DP ){
                if (EFI_ERROR(HiiLibGetString(HiiHandle, STRING_TOKEN(STR_IPV4_PREFIX), &PrefixSize, Prefix))) Prefix=L"IP4 ";
            } else if ( IpType == MSG_IPv6_DP ){
                if (EFI_ERROR(HiiLibGetString(HiiHandle, STRING_TOKEN(STR_IPV6_PREFIX), &PrefixSize, Prefix))) Prefix=L"IP6 ";
            }
                      
            PrefixSize = sizeof(PrefixBuffer2)/sizeof(CHAR16);
            if ( HttpBoot ){
                if (EFI_ERROR(HiiLibGetString(HiiHandle, STRING_TOKEN(STR_HTTP_PREFIX), &PrefixSize, Prefix2))) Prefix2=L"HTTP ";
            } else {
                if (EFI_ERROR(HiiLibGetString(HiiHandle, STRING_TOKEN(STR_PXE_PREFIX), &PrefixSize, Prefix2))) Prefix2=L"PXE ";
            }
        }
    }
    if (!GetControllerName(Handle, &ControllerName)) return 0;

    NumberOfCharacters = Swprintf_s(Name, NameSize, L"%s%s%s", Prefix2, Prefix, ControllerName);
    return RemoveTrailingSpaces(Name, NumberOfCharacters);
}

#ifdef CSM_SUPPORT
/**
 * Create a boot option description string from the BBS information for the passed boot option.
 *
 * @param Option The Boot option to create the string description
 * @param Name The buffer to fill with the description string
 * @param NameSize The size of the buffer for the description string
 *
 * @retval UINTN The length of the description string returned
 *
 * @note  function only available, and used, if CSM_SUPPORT token is defined and enabled
 */
UINTN ConstructBootOptionNameByBbsDescription(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
){
    CHAR8 *AsciiNameStr;
    UINTN  NumberOfCharacters;

    if (Option->BbsEntry == NULL) return 0;

    //Name from BBS table
    AsciiNameStr = (CHAR8*)(UINTN)(
          (Option->BbsEntry->DescStringSegment<<4)
        + Option->BbsEntry->DescStringOffset
    );
    if (AsciiNameStr == NULL) return 0;
    for(  NumberOfCharacters = 0
        ; NumberOfCharacters < NameSize-1
        ; NumberOfCharacters++
    ){
        if (!AsciiNameStr[NumberOfCharacters]) break;
        Name[NumberOfCharacters] = AsciiNameStr[NumberOfCharacters];
    }
    Name[NumberOfCharacters]=0;
    return RemoveTrailingSpaces(Name, NumberOfCharacters);
}
#endif

/**
 * Create a device description string in the passed Name buffer by parsing the device
 * path and creating a string based upon the nodes encountered
 *
 * @param DevicePath The device path to use to create the description string
 * @param Name The buffer to fill with the description string
 * @param NameSize The size of the buffer for the description string
 *
 * @retval UINTN The length of the description string returned
 */
UINTN DevicePathToNameString(
    EFI_DEVICE_PATH_PROTOCOL *DevicePath, CHAR16 *Name, UINTN NameSize
){
    STRING_REF StrToken;
    EFI_DEVICE_PATH_PROTOCOL *Dp;
    UINTN BufferSize;
    UINTN NumberOfCharacters = 0;

    for(  Dp = DevicePath; !(isEndNode(Dp)); Dp=NEXT_NODE(Dp)){
        StrToken = DevicePathNodeToStrRef(Dp);
        BufferSize = (NameSize-NumberOfCharacters)*sizeof(CHAR16);
        if (   StrToken!= INVALID_STR_TOKEN
            && !EFI_ERROR(HiiLibGetString(
                    HiiHandle,StrToken,&BufferSize, &Name[NumberOfCharacters]
                ))
            && BufferSize != 0
        ){
            NumberOfCharacters += (BufferSize-1)/sizeof(CHAR16);
            if (NumberOfCharacters < NameSize - 1){
                Name[NumberOfCharacters++]=L' ';
            }
        }
    }//for ;
    if (NumberOfCharacters !=0 ) Name[--NumberOfCharacters]=0; //override last space with the terminating zero
    return NumberOfCharacters;
}

/**
 * Create a device description string by parsing the device handle.
 *
 * @param Option The Boot option to create the string description
 * @param Name The buffer to fill with the description string
 * @param NameSize The size of the buffer for the description string
 *
 * @retval UINTN The length of the description string returned
*/
UINTN ConstructBootOptionNameByHandleDevicePath(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
){
    EFI_DEVICE_PATH_PROTOCOL *DevicePath;

    if (Option->DeviceHandle == INVALID_HANDLE) return 0;
     //Name from Device Path

    if (!EFI_ERROR(pBS->HandleProtocol(
        Option->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID**)&DevicePath
    ))) return DevicePathToNameString(DevicePath, Name, NameSize);
    return 0;
}

/**
 * Create a device description string by parsing the boot options device path.
 *
 * @param Option The Boot option to create the string description
 * @param Name The buffer to fill with the description string
 * @param NameSize The size of the buffer for the description string
 *
 * @retval UINTN The length of the description string returned
 */UINTN ConstructBootOptionNameByFilePathList(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
){
    if (Option->FilePathList == NULL) return 0;
    return DevicePathToNameString(Option->FilePathList, Name, NameSize);
}

/**
 * Create a device description string by calling all the functions linked into the BuildNameFunction
 * eLink list
 *
 * @param Option The Boot option to create the string description
 * @param Name The buffer to fill with the description string
 * @param NameSize The size of the buffer for the description string
 *
 * @retval Number of Unicode characters printed into the Name buffer excluding the terminating zero.
*/
UINTN ConstructBootOptionBaseName(BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize){
    UINTN NumberOfCharacters;
	UINT32 i;

	for(i=0; BuildNameFunctions[i]!=NULL; i++){
        NumberOfCharacters = BuildNameFunctions[i](Option, Name, NameSize);
        if ( NumberOfCharacters!=0 ) return NumberOfCharacters;
    }

    return 0;
}

/**
 * Create a boot option name by calling the ConstructBootOptionNamePrefix, ConstructBootOptionNameSuffix,
 * and ConstructBootOptionBaseName. Combine all those into the device's Description string
 *
 * @param Option The boot option
 *
 * @retval UINTN The length of the string created
 */

UINTN ConstructBootOptionName(BOOT_OPTION *Option){
    CHAR16 Name[1024];
    UINTN Length = sizeof(Name)/sizeof(CHAR16);
    UINTN NumberOfCharacters, BaseNameLength;
	
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    do {
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
        NumberOfCharacters = ConstructBootOptionNamePrefix(Option,Name,Length);
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.		
        ASSERT(NumberOfCharacters < Length);
        if (NumberOfCharacters >= Length) break;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.		
        Length -= NumberOfCharacters;
        BaseNameLength = ConstructBootOptionBaseName(
            Option, &Name[NumberOfCharacters], Length
        );
        NumberOfCharacters += BaseNameLength;
        if (BaseNameLength==0){
            //Unknown Device
            UINTN BufferSize = Length*sizeof(CHAR16);
            if (EFI_ERROR(
                    HiiLibGetString(
                        HiiHandle, STRING_TOKEN(STR_UNKNOWN), &BufferSize, &Name[NumberOfCharacters]
                    )
            )){
                NumberOfCharacters += Swprintf_s(
                    &Name[NumberOfCharacters], Length, L"Unknown Device"
                );
            }else{
                NumberOfCharacters += (BufferSize-1)/sizeof(CHAR16);
            }
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.			
            ASSERT(NumberOfCharacters < Length);
            if (NumberOfCharacters >= Length) break;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.			
            Length -= NumberOfCharacters;
        }
        NumberOfCharacters += ConstructBootOptionNameSuffix(
            Option, &Name[NumberOfCharacters], Length
        );
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
        ASSERT(NumberOfCharacters < Length);
    } while (FALSE);
    if (NumberOfCharacters >= Length){
        NumberOfCharacters = Length-1;
        Name[NumberOfCharacters] = 0;
    }
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    //convert number of characters into string buffer size
    Length = (NumberOfCharacters+1)*sizeof(CHAR16);
    Option->Description = Malloc(Length);
    MemCpy(Option->Description,Name,Length);
    return NumberOfCharacters;
}

/**
 * Create a description string for the device by reading the ATA serial number
 *
 * @param Option The boot option
 * @param Name The buffer to fill with the ATA serial number
 * @param NameSize The current index of the last character in the Name parameter
 *
 * @retval UINTN the new index of the last character in the Name parameter
 */
UINTN ConstructAtaBootOptionNameBySerialNumber(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
){
    EFI_HANDLE Handle;
    UINTN  NumberOfCharacters;
    EFI_DISK_INFO_PROTOCOL *DiskInfo;
    EFI_STATUS Status;
    UINT16 IdentifyData[256];
    UINT32 Size;
    CHAR8 SerialNumber[21];

    if (Option->DeviceHandle == INVALID_HANDLE) return 0;

    Handle = GetPhysicalBlockIoHandle(Option->DeviceHandle);
    Status = pBS->HandleProtocol(
        Handle, &gEfiDiskInfoProtocolGuid, (VOID**)&DiskInfo
    );
    if (EFI_ERROR(Status)) return 0;
    Size = sizeof(IdentifyData);
    Status = DiskInfo->Identify ( DiskInfo, IdentifyData, &Size );
    if (EFI_ERROR(Status)) return 0;
    MemCpy(SerialNumber, IdentifyData+10, 20);
    SerialNumber[20] = 0;
    NumberOfCharacters = Swprintf_s(Name, NameSize, L"%S", SerialNumber);
    return NumberOfCharacters;
}

/**
 * Create a description string for the device by reading the USB serial number
 *
 * @param Option The boot option
 * @param Name The buffer to fill with the USB serial number
 * @param NameSize The current index of the last character in the Name parameter
 *
 * @retval UINTN the new index of the last character in the Name parameter
**/
UINTN ConstructUsbBootOptionNameBySerialNumber(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
){

    EFI_HANDLE Handle;
    UINTN  NumberOfCharacters;
    EFI_STATUS Status;

	EFI_USB_IO_PROTOCOL *UsbIo = NULL;
	EFI_USB_DEVICE_DESCRIPTOR DevDesc = {0};
	CHAR16 *UsbSerialNumber=NULL;

    if (Option->DeviceHandle == INVALID_HANDLE) return 0;

    Handle = GetPhysicalBlockIoHandle(Option->DeviceHandle);

	Status = pBS->HandleProtocol(Handle, &gEfiUsbIoProtocolGuid, (VOID**)&UsbIo);
	if(EFI_ERROR(Status)) return 0;
	Status = UsbIo->UsbGetDeviceDescriptor(UsbIo, &DevDesc);
	if(EFI_ERROR(Status)) return 0;

    if (DevDesc.StrSerialNumber) {
            Status = UsbIo->UsbGetStringDescriptor(UsbIo, 0x0409, DevDesc.StrSerialNumber, &UsbSerialNumber);
			if(EFI_ERROR(Status)) return 0;
			pBS->CopyMem( Name, UsbSerialNumber, (Wcslen(UsbSerialNumber)+1)*2 );
			NumberOfCharacters = Wcslen(UsbSerialNumber);
    		return NumberOfCharacters;
	}
	return 0;
}

/**
 * Create a description string for the device by calling the Internal Build name functions
 *
 * @param Option The boot option
 * @param Name The buffer to fill with the boot option name
 * @param NameSize The current index of the last character in the Name parameter
 *
 * @retval UINTN the new index of the last character in the Name parameter
 */
UINTN ConstructInternalBootOptionName(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
){
    static CONSTRUCT_BOOT_OPTION_NAME *IntBuildNameFunctions[] = {
        ConstructAtaBootOptionNameBySerialNumber,
		ConstructUsbBootOptionNameBySerialNumber,
        ConstructBootOptionNameByHandle,
#ifdef CSM_SUPPORT
        ConstructBootOptionNameByBbsDescription,
#endif
        NULL
    };
    UINTN NumberOfCharacters;
	UINT32 i;

	for(i=0; IntBuildNameFunctions[i]!=NULL; i++){
        NumberOfCharacters = IntBuildNameFunctions[i](Option, Name, NameSize);
        if ( NumberOfCharacters!=0 ) return NumberOfCharacters;
    }

    return 0;
}

/**
 * Create a description string for the by calling the internal helper functions
 * UpdateBootOptionWithBootDeviceInfo and ConstructInternalBootOptionName
 *
 * @param Device The boot device
 * @param Name The buffer to fill with the device name
 * @param NameSize The current index of the last character in the Name parameter
 *
 * @retval UINTN the new index of the last character in the Name parameter
 */
UINTN ConstructInternalDeviceName(
    BOOT_DEVICE *Device, CHAR16 *Name, UINTN NameSize
){
    static BOOT_OPTION BootOption = {
    	{NULL,NULL}, LOAD_OPTION_ACTIVE, NULL, NULL, 0, NULL, 0,
        INVALID_BOOT_OPTION_NUMBER, LOWEST_BOOT_OPTION_PRIORITY,
        UNASSIGNED_HIGHEST_TAG,
        INVALID_HANDLE, INVALID_BBS_INDEX, NULL, TRUE, FALSE
    };
	UpdateBootOptionWithBootDeviceInfo(&BootOption,Device);
	return ConstructInternalBootOptionName(&BootOption, Name, NameSize);
}

/**
 * Attempt to match the OptionDevicePath to the Boot Device by directly comparing
 * the device paths.
 *
 * @param OptionDevicePath The device path to test against the boot device
 * @param Device The boot device
 *
 * @retval TRUE The device path matches the boot device
 * @retval FALSE The device path does not match the boot device
 */
BOOLEAN AmiDeviceNameDevicePathTest(
	EFI_DEVICE_PATH_PROTOCOL *OptionDevicePath, BOOT_DEVICE *Device
){
    AMI_DEVICE_NAME_DEVICE_PATH *NameDp;
    CHAR16 Name[1024];
    UINTN Size = sizeof(Name)/sizeof(CHAR16);
    UINTN NumberOfCharacters;

    NameDp = (AMI_DEVICE_NAME_DEVICE_PATH*)OptionDevicePath;
    if(    NameDp->Header.Header.Type != HARDWARE_DEVICE_PATH
        || NameDp->Header.Header.SubType != HW_VENDOR_DP
        || guidcmp(
                &AmiDeviceNameDevicePathGuid,
                &NameDp->Header.Guid
           ) != 0
    ) return FALSE;

    NumberOfCharacters = ConstructInternalDeviceName(
        Device, Name, Size
    );
    if (NumberOfCharacters==0) return FALSE;
    //convert number of charcters into string buffer size
    Size = (NumberOfCharacters+1)*sizeof(CHAR16);
    return
           Size == NODE_LENGTH(&NameDp->Header.Header)-sizeof(*NameDp)
        && !MemCmp(Name,NameDp+1,Size);
}

/**
 * Adds another device path to an array of boot option device paths
 *
 * @param FilePathListPtr
 *      On input, pointer to the current boot option FilePathList
 *      On output, pointer to the new FilePathList. Memory used by original FilePathList is deallocated.
 * @param FilePathListLength
 *      On input, pointer to the length of the current FilePathList
 *      On output, pointer to the length of the new FilePathList
 * @param DevicePath Device path to add to the FilePathList
 */
VOID AddDevicePathToFilePathList(
    EFI_DEVICE_PATH_PROTOCOL **FilePathListPtr, UINTN *FilePathListLength,
    EFI_DEVICE_PATH_PROTOCOL *DevicePath
)
{
	UINTN DevicePathLength;
	EFI_DEVICE_PATH_PROTOCOL *NewFilePathList;

    if (   FilePathListPtr == NULL
        || FilePathListLength == NULL
        || DevicePath == NULL
    ) return;

    DevicePathLength = DPLength(DevicePath);
    if (*FilePathListPtr == NULL) *FilePathListLength = 0;

	NewFilePathList = Malloc(*FilePathListLength + DevicePathLength);
    if ( *FilePathListPtr != NULL ){
	    MemCpy(NewFilePathList, *FilePathListPtr, *FilePathListLength);
        pBS->FreePool(*FilePathListPtr);
    }
	MemCpy(
        (UINT8*)NewFilePathList+*FilePathListLength,
        DevicePath, DevicePathLength
    );
    *FilePathListLength += DevicePathLength;
	*FilePathListPtr = NewFilePathList;
}

/**
 * Create a new device path for the boot option by copying the handle's device path
 *
 * @param Option The boot option to crate a device path
 *
 * @retval TRUE The device path was created
 * @retval FALSE The device path was not created
 */
BOOLEAN BuildEfiFilePath(BOOT_OPTION *Option){
	EFI_DEVICE_PATH_PROTOCOL *Dp;
	EFI_STATUS Status;

    if (   Option->FilePathList!=NULL
        || Option->BbsEntry != NULL
        || Option->DeviceHandle == INVALID_HANDLE
    ) return FALSE;
	Status = pBS->HandleProtocol(Option->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID**)&Dp);
    if (EFI_ERROR(Status)) return FALSE;
	Option->FilePathList = DPCopy(Dp);
	Option->FilePathListLength = DPLength(Dp);
    return TRUE;
}

#ifdef CSM_SUPPORT
/**
 * Build a legacy device path for this device by parsing the device's actual device path
 *
 * @param Option The boot option to create a device path for
 *
 * @retval TRUE The device path was created
 * @retval FALSE The device path was not created
 *
 * @note  function only available, and used, if CSM_SUPPORT token is defined and enabled
 */
BOOLEAN BuildLegacyFilePath(BOOT_OPTION *Option){
    static struct {
		BBS_BBS_DEVICE_PATH bbs;
		EFI_DEVICE_PATH_PROTOCOL end;
	} BbsDpTemplate =  {
		{
            {BBS_DEVICE_PATH,BBS_BBS_DP,sizeof(BBS_BBS_DEVICE_PATH)},
            BBS_HARDDISK,0,0
        },
		{END_DEVICE_PATH,END_ENTIRE_SUBTYPE,sizeof(EFI_DEVICE_PATH_PROTOCOL)}
	};

    if (Option->FilePathList!=NULL || Option->BbsEntry == NULL) return FALSE;
    BbsDpTemplate.bbs.DeviceType=GetBbsEntryDeviceType(Option->BbsEntry);
	Option->FilePathList = DPCopy(&BbsDpTemplate.bbs.Header);
	Option->FilePathListLength = DPLength(Option->FilePathList);

	return TRUE;
}

/**
 * Build a AMI BBS Device Path instance for this boot option
 *
 * @param Option Boot option to create an AMI BBS device path instance for
 *
 * @retval TRUE The device path was created
 * @retval FALSE The device path was not created
 *
 * @note  function only available, and used, if CSM_SUPPORT token is defined and enabled
**/
BOOLEAN BuildLegacyLocationFilePath(BOOT_OPTION *Option){
    static struct {
		AMI_BBS_DEVICE_PATH amibbs;
		EFI_DEVICE_PATH_PROTOCOL end;
	} AmiBbsDpTemplate = {
		{
		    {
              { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, sizeof(AMI_BBS_DEVICE_PATH) },
		      AMI_BBS_DEVICE_PATH_GUID
            },
            0, 0, 0, 0, 0, 0
        },
		{END_DEVICE_PATH,END_ENTIRE_SUBTYPE,sizeof(EFI_DEVICE_PATH_PROTOCOL)}
	};

    EFI_DEVICE_PATH_PROTOCOL *Dp;

    if (Option->BbsEntry == NULL) return FALSE;
	if (   Option->DeviceHandle == INVALID_HANDLE
        || EFI_ERROR(pBS->HandleProtocol(Option->DeviceHandle, &gEfiDevicePathProtocolGuid, &Dp))
    ){
        Dp = &AmiBbsDpTemplate.amibbs.Header.Header;
        AmiBbsDpTemplate.amibbs.Bus = (UINT8)Option->BbsEntry->Bus;
    	AmiBbsDpTemplate.amibbs.Device = (UINT8)Option->BbsEntry->Device;
    	AmiBbsDpTemplate.amibbs.Function = (UINT8)Option->BbsEntry->Function;
    	AmiBbsDpTemplate.amibbs.Class = (UINT8)Option->BbsEntry->Class;
        AmiBbsDpTemplate.amibbs.SubClass = (UINT8)Option->BbsEntry->SubClass;
        AmiBbsDpTemplate.amibbs.Instance = GetBbsDeviceInstance(Option->BbsIndex,Option->BbsEntry);
    }
    AddDevicePathToFilePathList(
        &Option->FilePathList, &Option->FilePathListLength, Dp
    );
	return TRUE;
}
#endif

/**
 * Build a device path name for this device by calling the ConstructInternalBootOptionName function
 *
 * @param Option The boot option to create a device path for
 *
 * @retval TRUE The device path was created
 * @retval FALSE The device path was not created
 */
BOOLEAN BuildNameFilePath(BOOT_OPTION *Option){
	static AMI_DEVICE_NAME_DEVICE_PATH AmiNameDpTemplate = {
        {
            { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, sizeof(AMI_DEVICE_NAME_DEVICE_PATH) },
            AMI_DEVICE_NAME_DEVICE_PATH_GUID
        }
	};
    static EFI_DEVICE_PATH_PROTOCOL EndOfDevicePathNode = {
        END_DEVICE_PATH, END_ENTIRE_SUBTYPE,
        {sizeof(EFI_DEVICE_PATH_PROTOCOL),0}
    };

    CHAR16 Name[1024];
    UINTN Size = sizeof(Name)/sizeof(CHAR16);
    UINTN NumberOfCharacters;
    AMI_DEVICE_NAME_DEVICE_PATH *NameDp;
    EFI_DEVICE_PATH_PROTOCOL    *Dp;

    NumberOfCharacters = ConstructInternalBootOptionName(
        Option, Name, Size
    );
    if (NumberOfCharacters==0) return FALSE;
    //convert number of charcters into string buffer size
    Size = (NumberOfCharacters+1)*sizeof(CHAR16);

    Dp = Malloc(sizeof(AmiNameDpTemplate)+Size+sizeof(EndOfDevicePathNode));
    ASSERT(Dp!=NULL);
    if (Dp==NULL) return FALSE;
    NameDp = (AMI_DEVICE_NAME_DEVICE_PATH*)Dp;

    *NameDp = AmiNameDpTemplate;
    SET_NODE_LENGTH(Dp,(UINT16)(sizeof(AmiNameDpTemplate)+Size));
    MemCpy(NameDp+1, Name, Size);
    MemCpy(NEXT_NODE(Dp), &EndOfDevicePathNode, sizeof(EndOfDevicePathNode));
    AddDevicePathToFilePathList(
        &Option->FilePathList, &Option->FilePathListLength, Dp
    );
    pBS->FreePool(Dp);
    return TRUE;
}

/**
 * Build a device path name for this device by calling the BuildFilePathFunctions elink list
 *
 * @param Option The boot option to create a device path for
 *
 * @retval TRUE The device path was created
 * @retval FALSE The device path was not created
 */
BOOLEAN BuildBootOptionFilePath(BOOT_OPTION *Option){
	UINT32 i;
    BOOLEAN FilePathCreated = FALSE;

	for(i=0; BuildFilePathFunctions[i]!=NULL; i++){
        FilePathCreated |= BuildFilePathFunctions[i](Option);
    }
    return FilePathCreated;
}

/**
 * Create boot options for the the newly discovered boot devices that have not been matched to
 * existing boot options
 */
VOID CreateBootOptionsForNewBootDevices(){
	DLINK *Link;
    BOOT_DEVICE *Device;

    FOR_EACH_BOOT_DEVICE(BootDeviceList,Link,Device){
		BOOT_OPTION *Option = CreateBootOption(BootOptionList);
		UpdateBootOptionWithBootDeviceInfo(Option,Device);
        DeleteBootDevice(BootDeviceList, Device);
		Option->FwBootOption = TRUE;
		ConstructBootOptionName(Option);
		if (!BuildBootOptionFilePath(Option)){
			Option->FilePathList=NULL;
			Option->FilePathListLength=0;
		}
	}
    DUMP_BOOT_OPTION_LIST(BootOptionList,"Before Processing");
}

/**
 * If normalization is enabled, regenerates all the description strings and/or file path lists instead
 * of attempting to rely on the information that was read out the boot options
 */
VOID NormalizeBootOptions(){
    DLINK *Link;
    BOOT_OPTION *Option;

    // Normalize boot options
	//(regenerate the description string and the file path list)
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
		if (   !Option->FwBootOption || !IsBootOptionWithDevice(Option)
            || Option->BootOptionNumber == INVALID_BOOT_OPTION_NUMBER
        ) continue;
        if (NormalizeBootOptionDevicePath){
        	EFI_DEVICE_PATH_PROTOCOL *OldFilePathList = Option->FilePathList;
            UINTN OldFilePathListLength = Option->FilePathListLength;
            Option->FilePathList = NULL;
            Option->FilePathListLength = 0;
		    BuildBootOptionFilePath(Option);
            if (Option->FilePathList == NULL){
                Option->FilePathList = OldFilePathList;
                Option->FilePathListLength = OldFilePathListLength;
            }else if (OldFilePathList != NULL){
                pBS->FreePool(OldFilePathList);
            }
        }
        if (NormalizeBootOptionName){
	        CHAR16 *OldDescription = Option->Description;
            Option->Description = NULL;
		    ConstructBootOptionName(Option);
            if (Option->Description == NULL)
                Option->Description = OldDescription;
            else if (OldDescription != NULL)
                pBS->FreePool(OldDescription);
        }
	}
}

/**
 * Debug function to dump the current BootOptionList (in the order its encountered) and output the
 * information for debugging
 *
 * @param BootOptionList Pointer to the list of boot options
 * @param ListCaption The caption to output to describe this list
 */
VOID DumpBootOptionList(DLIST *BootOptionList, CHAR8 *ListCaption){
	DLINK *Link;
    BOOT_OPTION *Option;
    if (ListCaption!=NULL)
        TRACE((TRACE_DXE_CORE,"%s:\n",ListCaption));
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        CHAR8 *Details1, *Details2, *Details3;
        if (Option->GroupHeader) Details1="(group header)";
        else if (!IsBootOptionWithDevice(Option)) Details1 ="(orphan)";
        else Details1="";
        if ((Option->Attributes&LOAD_OPTION_ACTIVE)!=LOAD_OPTION_ACTIVE) Details2="(disabled)";
        else Details2="";
        if ((Option->Attributes&LOAD_OPTION_HIDDEN)==LOAD_OPTION_HIDDEN) Details3="(hidden)";
        else Details3="";
        TRACE((TRACE_DXE_CORE,
               "%X(%X/%X).%S%s%s%s\n",
               Option->BootOptionNumber,Option->Priority,Option->Tag,Option->Description,
               Details1,Details2,Details3
        ));
    }
}

/**
 * Return the required size to store this boot option
 *
 * @param Option Pointer to the boot option
 *
 * @retval UINTN The size required to store this boot option
 */
UINTN GetBootOptionPackedSize(BOOT_OPTION *Option){
	return
		  sizeof(EFI_LOAD_OPTION)
		+ (Wcslen(Option->Description)+1)*sizeof(CHAR16)
		+ Option->FilePathListLength
		+ Option->OptionalDataSize;
}

/**
 * Using the passed NvramOption as a starting point, pack the passed Option into the OptionalData area of the
 * NvramOption. Return a pointer to the new memory.
 *
 * @param Option The Boot Option to pack behind the NvramOption
 * @param NvramOption The start of the NvramOption
 *
 * @return Pointer to the new memory
 */
VOID* PackBootOption(BOOT_OPTION *Option, EFI_LOAD_OPTION *NvramOption){
		UINTN DescriptionSize;
		UINT8 *Ptr;

		DescriptionSize = (Wcslen(Option->Description)+1)*sizeof(CHAR16);
		NvramOption->Attributes = Option->Attributes;
		NvramOption->FilePathListLength = (UINT16)Option->FilePathListLength;
		MemCpy(NvramOption+1,Option->Description,DescriptionSize);
		Ptr = (UINT8*)(NvramOption+1)+DescriptionSize;
		MemCpy(Ptr, Option->FilePathList, Option->FilePathListLength);
		Ptr += Option->FilePathListLength;
		return Ptr;
}

/**
 * Save the boot options into NVRAM as L"BootXXXX" variables, and then update the
 * L"BootOrder" NVRAM Variable
 */
VOID SaveBootOptions(){
	UINTN BootOrderSize;
	UINT16 *BootOrder;
	DLINK *Link;
	UINTN BootOrderIndex = 0;
    BOOT_OPTION *Option;

    //PRECONDITION: All Boot Option Numbers are set
    SortList(BootOptionList, ComparePriorityThenBootOptionNumber);
    DUMP_BOOT_OPTION_LIST(BootOptionList, "Before Saving");
	BootOrderSize = BootOptionList->Size*sizeof(UINT16);
	if (BootOrderSize==0) return;
	BootOrder = Malloc(BootOrderSize);

    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
		EFI_LOAD_OPTION *NvramOption;
        BOOT_OPTION *NestedOption;
		DLINK *TmpLink;
		UINTN NvramOptionSize;
		UINT8 *Ptr;
		CHAR16 BootStr[9];
		EFI_STATUS Status;
		BOOLEAN HasNestedOptions = FALSE;

        //Make sure the boot option is well-formed
        if(    Option->FilePathListLength == 0
            || Option->FilePathList == NULL
            || Option->BootOptionNumber == INVALID_BOOT_OPTION_NUMBER
        ){
            TRACE((TRACE_DXE_CORE,
                   "SaveBootOptions: skipping invalid boot option '%X.%S'\n",
                   Option->BootOptionNumber,Option->Description
            ));
            continue;
        }

		NvramOptionSize =  GetBootOptionPackedSize(Option);
		if (Option->FwBootOption){
			NvramOptionSize += sizeof(UINT32); //signature
            //TRACE((-1,"(%X) before size loop: size=%X\n",Option->BootOptionNumber,NvramOptionSize));
            FOR_EACH_LIST_ELEMENT(Option->Link.pNext, TmpLink, NestedOption, BOOT_OPTION){
				if (Option->BootOptionNumber != NestedOption->BootOptionNumber)
                    break;
				NvramOptionSize +=   AMI_NESTED_BOOT_OPTION_HEADER_SIZE +
				                   + GetBootOptionPackedSize(NestedOption)
                                   + sizeof(UINT32); //signature;
                //TRACE((-1,"Nested(%X) size loop: size=%X\n",Option->BootOptionNumber,NvramOptionSize));
				HasNestedOptions = TRUE;
			}
		}
		NvramOption = Malloc(NvramOptionSize);
		Ptr = PackBootOption(Option,NvramOption);
		if (Option->FwBootOption){
			if (HasNestedOptions || Option->GroupHeader)
				WriteUnaligned32((UINT32*)Ptr,AMI_GROUP_BOOT_OPTION_SIGNATURE);
			else
				WriteUnaligned32((UINT32*)Ptr,AMI_SIMPLE_BOOT_OPTION_SIGNATURE);
			Ptr += sizeof(UINT32);
            //TRACE((-1,"(%X) before save loop: size=%X\n",Option->BootOptionNumber,Ptr-(UINT8*)NvramOption));
            if (HasNestedOptions){
                FOR_EACH_LIST_ELEMENT(Option->Link.pNext, TmpLink, NestedOption, BOOT_OPTION){
    				NESTED_BOOT_OPTION *NestedPackedOption;

    				if (Option->BootOptionNumber != NestedOption->BootOptionNumber)
                        break;
    				NestedPackedOption = (NESTED_BOOT_OPTION*)Ptr;
    				NestedPackedOption->Signature = AMI_NESTED_BOOT_OPTION_SIGNATURE;
    				Ptr = PackBootOption(NestedOption,&NestedPackedOption->Option);
					WriteUnaligned32((UINT32*)Ptr,AMI_SIMPLE_BOOT_OPTION_SIGNATURE);
    				Ptr += sizeof(UINT32);
    				if (NestedOption->OptionalDataSize!=0){
    					MemCpy(
    						Ptr, NestedOption->OptionalData, NestedOption->OptionalDataSize
    					);
    				    Ptr += NestedOption->OptionalDataSize;
    				}
    				NestedPackedOption->Size = (UINT32)(Ptr - (UINT8*)NestedPackedOption);
                    //delete nested option
                    DeleteBootOption(BootOptionList,NestedOption);
                    //TRACE((-1,"Nested(%X) save loop: size=%X\n",Option->BootOptionNumber,Ptr-(UINT8*)NvramOption));
    			}
            }
		}
		if (Option->OptionalDataSize!=0){
			MemCpy(
				Ptr, Option->OptionalData, Option->OptionalDataSize
			);
		}
        //TRACE((-1,"Saving %d: %X; %X; ods=%d\n",Option->BootOptionNumber,Ptr+Option->OptionalDataSize,(UINT8*)NvramOption+NvramOptionSize,Option->OptionalDataSize));
		ASSERT(Ptr+Option->OptionalDataSize == (UINT8*)NvramOption+NvramOptionSize);
		Swprintf(BootStr,L"Boot%04X",Option->BootOptionNumber);
		Status = pRS->SetVariable(
			BootStr, &EfiVariableGuid,
			BOOT_VARIABLE_ATTRIBUTES, NvramOptionSize, NvramOption
		);
        BootOrder[BootOrderIndex]=Option->BootOptionNumber;
        Link = Option->Link.pNext;
        DeleteBootOption(BootOptionList,Option);
		pBS->FreePool(NvramOption);
		if (EFI_ERROR(Status)) continue;
        BootOrderIndex++;
	}
	pRS->SetVariable(
		L"BootOrder", &EfiVariableGuid,
		BOOT_VARIABLE_ATTRIBUTES, BootOrderIndex*sizeof(UINT16), BootOrder
	);
	pBS->FreePool(BootOrder);
}

/**
 * This function initializes the global variables. Must be called before any other boot option processing function can be used.
*/
VOID UpdateBootOptionVariables(){
    LoadStrings(TheImageHandle, &HiiHandle);
	DListInit(BootOptionList);
	DListInit(BootDeviceList);
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
