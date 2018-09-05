//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
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
 * @file BdsBoard.c
 * This file contains BDS/CORE_DXE related OEM code.  There are
 * variables defined in this file that might change for each
 * OEM project
 */

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "BootOptions.h"
#include <Setup.h>
#include <BootOptioneLinks.h>
#ifdef CSM_SUPPORT
#include <Protocol/LegacyBiosExt.h>
#endif
#include <Library/PcdLib.h>

//---------------------------------------------------------------------------
// MACRO Constants
//---------------------------------------------------------------------------
#ifndef FW_ORPHAN_BOOT_OPTIONS_POLICY
#define FW_ORPHAN_BOOT_OPTIONS_POLICY ORPHAN_BOOT_OPTIONS_POLICY_DELETE
#endif
#ifndef NON_FW_ORPHAN_BOOT_OPTIONS_POLICY
#define NON_FW_ORPHAN_BOOT_OPTIONS_POLICY ORPHAN_BOOT_OPTIONS_POLICY_KEEP
#endif
#ifndef ORPHAN_GROUP_HEADERS_POLICY
#define ORPHAN_GROUP_HEADERS_POLICY ORPHAN_BOOT_OPTIONS_POLICY_DELETE
#endif

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------
typedef enum{
//    BoTagLegacyXxx
//    BoTagUefiXxx
//    BoTagXxx
    BoTagLegacyFloppy,
    BoTagLegacyHardDisk,
    BoTagLegacyCdrom,
    BoTagLegacyPcmcia,
    BoTagLegacyUsb,
    BoTagLegacyEmbedNetwork,
    BoTagLegacyBevDevice,
    BoTagUefi,
    BoTagEmbeddedShell
} BOOT_OPTION_TAG;

//---------------------------------------------------------------------------
// Constant and Variables declarations
//---------------------------------------------------------------------------
#ifndef DEFAULT_BOOT_TIMEOUT
#define DEFAULT_BOOT_TIMEOUT 1
#endif
const   UINT16  DefaultTimeout  = DEFAULT_BOOT_TIMEOUT;

STRING_REF BbsDevTypeNameToken[] = {
	STRING_TOKEN(STR_FD),
	STRING_TOKEN(STR_HD),
	STRING_TOKEN(STR_CD),
	STRING_TOKEN(STR_PCMCIA),
	STRING_TOKEN(STR_USB),
	STRING_TOKEN(STR_NET),
	STRING_TOKEN(STR_BEV),
	STRING_TOKEN(STR_UNKNOWN)
};

struct{
    UINT8   Type;
    UINT8	SubType; 			
    STRING_REF StrToken;
} DpStrings[] = {   
    {MESSAGING_DEVICE_PATH, MSG_ATAPI_DP, STRING_TOKEN(STR_ATAPI)},
    {MESSAGING_DEVICE_PATH, MSG_SCSI_DP, STRING_TOKEN(STR_SCSI)},
	{MESSAGING_DEVICE_PATH, MSG_USB_DP, STRING_TOKEN(STR_USB)},
	{MESSAGING_DEVICE_PATH, MSG_MAC_ADDR_DP, STRING_TOKEN(STR_NET)},
	{MEDIA_DEVICE_PATH, MEDIA_HARDDRIVE_DP, STRING_TOKEN(STR_HD)},
	{MEDIA_DEVICE_PATH, MEDIA_CDROM_DP, STRING_TOKEN(STR_CD)}
};

//these GUIDs are used by BDS.c
EFI_GUID    SetupEnterProtocolGuid=AMITSE_SETUP_ENTER_GUID;
EFI_GUID    SecondBootOptionProtocolGuid=AMITSE_AFTER_FIRST_BOOT_OPTION_GUID;
EFI_GUID    BeforeBootProtocolGuid = AMITSE_EVENT_BEFORE_BOOT_GUID;
#ifndef EFI_AMI_LEGACYBOOT_PROTOCOL_GUID
#define EFI_AMI_LEGACYBOOT_PROTOCOL_GUID            \
  {0x120d28aa, 0x6630, 0x46f0, 0x81, 0x57, 0xc0, 0xad, 0xc2, 0x38, 0x3b, 0xf5}
#endif
EFI_GUID    BeforeLegacyBootProtocolGuid = EFI_AMI_LEGACYBOOT_PROTOCOL_GUID;

struct {
	VENDOR_DEVICE_PATH media;
	EFI_DEVICE_PATH_PROTOCOL end;
} ShellDp = {
	{
        {
            MEDIA_DEVICE_PATH, MEDIA_VENDOR_DP,
            sizeof(VENDOR_DEVICE_PATH)
        },
        AMI_MEDIA_DEVICE_PATH_GUID
    },
	{
        END_DEVICE_PATH, END_ENTIRE_SUBTYPE,
        sizeof(EFI_DEVICE_PATH_PROTOCOL)
    }
};

const STRING_REF UnknownDeviceToken = STRING_TOKEN(STR_UNKNOWN);

BOOT_OPTION_TAG LegacyBootOptionTags[] = {
    BoTagLegacyFloppy,
    BoTagLegacyHardDisk,
    BoTagLegacyCdrom,
    BoTagLegacyPcmcia,
    BoTagLegacyUsb,
    BoTagLegacyEmbedNetwork,
    BoTagLegacyBevDevice,
    UNASSIGNED_HIGHEST_TAG
};

BOOT_OPTION_TAG BootOptionTagPriorities[] = {
    BOOT_OPTION_TAG_PRIORITIES,
    UNASSIGNED_HIGHEST_TAG
};

BOOLEAN NormalizeBootOptionName = NORMALIZE_BOOT_OPTION_NAME;
BOOLEAN NormalizeBootOptionDevicePath = NORMALIZE_BOOT_OPTION_DEVICE_PATH;

//---------------------------------------------------------------------------
// External variables
//---------------------------------------------------------------------------
extern  EFI_GUID        EfiVariableGuid;

//---------------------------------------------------------------------------
// Function Implementations
//---------------------------------------------------------------------------

/**
 * Search for, and return, a pointer to the specific device path node of type/subtype.
 *
 * @param DevicePath Device Path to search
 * @param Type Device path type to find
 * @param SubType Device path subtype to find
 *
 * @return EFI_DEVICE_PATH_PROTOCOL Pointer to the desired device path node
 * @retval NULL No device path nodes were found
 */
static EFI_DEVICE_PATH_PROTOCOL* 
FindDeviceNodeInDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN UINT8 Type,
  IN UINT8 SubType)
{
    while(DevicePath != NULL && !isEndNode(DevicePath))
    {
        if(DevicePath->Type == Type && DevicePath->SubType == SubType)
            return DevicePath;
        DevicePath = NEXT_NODE(DevicePath);
    }
    return NULL;
}      

/**
 * This function returns a value of attributes that should be used for UEFI Variables used in Setup
 * 
 * @retval UINT32 variable attributes
 */
UINT32 GetSetupVariablesAttributes(){
    SETUP_DATA *SetupData = NULL;
    UINTN VariableSize = 0;
    UINT32 Attributes;
    EFI_STATUS Status;
    static EFI_GUID SetupGuid = SETUP_GUID;
    
    Status = GetEfiVariable(L"Setup", &SetupGuid, &Attributes, &VariableSize, (VOID**)&SetupData);
    if (EFI_ERROR(Status)) {
        return EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
    } else {
        pBS->FreePool(SetupData);
        return Attributes;
    }
}

/**
 * This function converts node from the device path to a string. Once the whole 
 * device path is converted the string is used as a boot option name. This 
 * function is only used as a component name protocol, it does not report 
 * the device name.
 * 
 * @param Dp Pointer to the device path node
 * 
 * @retval STRING_REF string token (-1, if the string token is not available)
 */
STRING_REF DevicePathNodeToStrRef(EFI_DEVICE_PATH_PROTOCOL *Dp){
    UINTN i;
    //Built in Shell is a special case
    if (   NODE_LENGTH(Dp)==NODE_LENGTH(&ShellDp.media.Header)
        && MemCmp(Dp,&ShellDp.media.Header,NODE_LENGTH(Dp))==0
    ) return STRING_TOKEN(STR_SHELL);
    //BBS device path is another special case
#ifdef CSM_SUPPORT
    if ( Dp->Type==BBS_DEVICE_PATH ){
        return BbsDevTypeNameToken[BBS_DEVICE_TYPE_TO_INDEX(((BBS_BBS_DEVICE_PATH*)Dp)->DeviceType)];
    }
#endif
    for (i=0; i < sizeof(DpStrings)/sizeof(DpStrings[0]); i++){
        if ((Dp->Type==DpStrings[i].Type) && (Dp->SubType==DpStrings[i].SubType)){
            return DpStrings[i].StrToken;
        }		
    }//for i

    return INVALID_STR_TOKEN;
}

/**
 * Using the passed BBS_TABLE entry pointer, determine the device type of the associated device
 * 
 * @param BbsEntry Pointer to a BBS_TABLE entry
 * 
 * @retval UINT16 device type of the BBS entry, see LegacyBios.h for actual values
 *          BBS_FLOPPY
 *          BBS_HARDDISK
 *          BBS_CDROM
 *          BBS_PCMCIA
 *          BBS_USB
 *          BBS_EMBED_NETWORK
 *          BBS_BEV_DEVICE
 */
UINT16 GetBbsEntryDeviceTypeDefault(BBS_TABLE *BbsEntry){
#ifdef CSM_SUPPORT
    UINT16 DeviceType = BbsEntry->DeviceType;
#if BBS_USB_DEVICE_TYPE_SUPPORT
    if (   BbsEntry->Class == PCI_CL_SER_BUS
        && BbsEntry->SubClass == PCI_CL_SER_BUS_SCL_USB
    ) return BBS_USB;
#endif
#if BBS_NETWORK_DEVICE_TYPE_SUPPORT
    if (   BbsEntry->Class == PCI_CL_NETWORK
        && BbsEntry->DeviceType == BBS_BEV_DEVICE
    ) return BBS_EMBED_NETWORK;
#endif
    return DeviceType;
#else
    return 0;
#endif
}

/**
 * Using the passed BOOT_OPTION structure, which should correspond to a legacy device entry, determine the device type 
 * 
 * @param Option pointer to the BOOT_OPTION structure for the device in question
 * 
 * @retval UINT16 device type of the BOOT_OPTION item, see LegacyBios.h for actual values
 *          BBS_FLOPPY
 *          BBS_HARDDISK
 *          BBS_CDROM
 *          BBS_PCMCIA
 *          BBS_USB
 *          BBS_EMBED_NETWORK
 *          BBS_BEV_DEVICE
 */
UINT16 GetLegacyDevOrderType(BOOT_OPTION *Option){
    return ((BBS_BBS_DEVICE_PATH*)Option->FilePathList)->DeviceType;
}

/**
 * For the passed Tag entry, return the correct boot priority based on  the BootOptionTagPriorities global variable that is filled out based on the SDL Token BOOT_OPTION_TAG_PRIORITIES.
 * 
 * @param Tag one of the following items of the BOOT_OPTION_TAG enum:
 *          BoTagLegacyFloppy
 *          BoTagLegacyHardDisk
 *          BoTagLegacyCdrom
 *          BoTagLegacyPcmcia
 *          BoTagLegacyUsb
 *          BoTagLegacyEmbedNetwork
 *          BoTagLegacyBevDevice
 *          BoTagUefi
 *          BoTagEmbeddedShell
 * 
 * @retval The index of this item in the BootOptionTagPriorities structure, which also corresponds to the boot priority that should be assigned to this class of device
 */
UINT32 FindTagPriority(UINT16 Tag){
    UINT32 i;
    for(i=0; BootOptionTagPriorities[i]!=UNASSIGNED_HIGHEST_TAG; i++)
        if (Tag==BootOptionTagPriorities[i]) return i;
    return UNASSIGNED_HIGHEST_TAG;
}

/**
 * Determine if the passed BOOT_OPTION is the built in EFI Shell
 * 
 * @param Option the boot option in question
 * 
 * @retval BOOLEAN TRUE - this boot option represent the built in EFI Shell
 * @retval FALSE this is not the built in EFI Shell
 */
BOOLEAN IsShellBootOption(BOOT_OPTION *Option){
    EFI_DEVICE_PATH_PROTOCOL *Dp = Option->FilePathList;

    if (Dp==NULL) return FALSE;
    if (   NODE_LENGTH(Dp)==NODE_LENGTH(&ShellDp.media.Header)
        && MemCmp(Dp,&ShellDp.media.Header,NODE_LENGTH(Dp))==0
    ) return TRUE;

    return FALSE;
}

/**
 * Go through the entire boot option list and Apply priorities for each entry in the list.
 * 
 * @param BootOptionList the entire Boot Option List
 * 
 * @note  To change boot order priorities, do not modify this function, modify the SDL Token BootOptionTagPriorities.
 */
VOID SetBootOptionTags(){
	DLINK *Link;
    BOOT_OPTION *Option;
    UINT32 UefiBootOptionsInc = 0x100;

    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        UINT32 TagPriority;
        UINT32 BaseTag = UNASSIGNED_HIGHEST_TAG;
#ifdef CSM_SUPPORT
        if (IsLegacyBootOption(Option)){
            UINT16 DeviceType;
            DeviceType = ((BBS_BBS_DEVICE_PATH*)Option->FilePathList)->DeviceType;
            BaseTag = LegacyBootOptionTags[BBS_DEVICE_TYPE_TO_INDEX(DeviceType)];
        }else
#endif
        if (IsShellBootOption(Option)) BaseTag = BoTagEmbeddedShell;
        else BaseTag = BoTagUefi;
        if (BaseTag == UNASSIGNED_HIGHEST_TAG) continue;
        TagPriority = FindTagPriority(BaseTag);
        //UEFI boot options must have unique tags, otherwise then will be grouped when 
        //GROUP_BOOT_OPTIONS_BY_TAG tokens is enabled
        if (BaseTag == BoTagUefi) BaseTag += UefiBootOptionsInc++;
        Option->Tag = BootOptionTag(BaseTag, TagPriority);
	}
}

/**
 * Create a Group Header Entry for the passed BOOT_OPTION and add the group header 
 * to the master boot options list
 * 
 * @param BootOptionList the master boot options list to add the newly created group item
 * @param FirstGroupOption the boot option which needs linked to a group item
 */
VOID CreateGroupHeader(DLIST *BootOptionList, BOOT_OPTION *FirstGroupOption){
#ifdef CSM_SUPPORT
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

    BOOT_OPTION *Option;

	if (!IsLegacyBootOption(FirstGroupOption)) return;
	Option = CreateBootOption(BootOptionList);
	Option->BootOptionNumber = FirstGroupOption->BootOptionNumber;
	Option->Priority = FirstGroupOption->Priority;
	Option->Tag = FirstGroupOption->Tag;
	Option->FwBootOption = TRUE;
    Option->GroupHeader = TRUE;
    BbsDpTemplate.bbs.DeviceType=GetBbsEntryDeviceType(FirstGroupOption->BbsEntry);
	Option->FilePathList = DPCopy(&BbsDpTemplate.bbs.Header);
	Option->FilePathListLength = DPLength(Option->FilePathList);
    ConstructBootOptionName(Option);
#endif
}

/**
 * Go through the boot option list and set the priorities of each group of devices
 * 
 * @param BootOptionList the master list of boot options
 */
VOID SetBootOptionPriorities(){
	DLINK *Link;
#if GROUP_BOOT_OPTIONS_BY_TAG	
    UINT16 PreviousBootOptionNumber = INVALID_BOOT_OPTION_NUMBER;
    UINT32 PreviousTag = UNASSIGNED_HIGHEST_TAG;
#endif    
    UINT32 PreviousPriority=0;
    BOOT_OPTION *Option;
    UINT16 NextOptionNumber;

    //Detect first unused boot option number
    NextOptionNumber = 0;
    if (!DListEmpty(BootOptionList)){
        FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
            if (   Option->BootOptionNumber != INVALID_BOOT_OPTION_NUMBER 
                && Option->BootOptionNumber > NextOptionNumber 
            ) NextOptionNumber = Option->BootOptionNumber;
        }
        NextOptionNumber++;
    }

	SortList(BootOptionList, CompareTagThenPriority);
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
#if GROUP_BOOT_OPTIONS_BY_TAG
        if (Option->BootOptionNumber==INVALID_BOOT_OPTION_NUMBER){
            if (   PreviousTag != Option->Tag 
                || Option->Tag==UNASSIGNED_LOWEST_TAG 
                || Option->Tag==UNASSIGNED_HIGHEST_TAG
            ) PreviousBootOptionNumber=NextOptionNumber++;
            Option->BootOptionNumber=PreviousBootOptionNumber;
            Option->Priority=++PreviousPriority;
        }else{
            PreviousBootOptionNumber = Option->BootOptionNumber;
            PreviousPriority = Option->Priority;
        }
		PreviousTag = Option->Tag;
#else
        if (Option->BootOptionNumber==INVALID_BOOT_OPTION_NUMBER){
            Option->BootOptionNumber=(NextOptionNumber)++;
            Option->Priority=++PreviousPriority;
        }else{
            PreviousPriority = Option->Priority;
        }
#endif
	}
    DUMP_BOOT_OPTION_LIST(BootOptionList,"After Setting Priorities");
}

/**
 * Go through the the boot option list and  create group header items for
 * groups of items with the same tag value.
 * 
 * @param Pointer to the group of boot options
 * 
 * @note  This function assumes that the BootOptionList is already sorted by tag and then by priority.  \
 * Failure to adhere to those assumptions will cause undesired behavior
 */
VOID CreateGroupHeaders(DLIST *BootOptionList){
    DLINK *Link;
    BOOT_OPTION *Option;
    UINT32 PreviousTag = UNASSIGNED_LOWEST_TAG;

    //PRECONDITION: Boot Option List is sorted by tag then by priority

    DUMP_BOOT_OPTION_LIST(BootOptionList,"Before Adding Group Headers");
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        if (PreviousTag == Option->Tag) continue;
        PreviousTag = Option->Tag;
        if (   Option->Tag==UNASSIGNED_LOWEST_TAG 
            || Option->Tag==UNASSIGNED_HIGHEST_TAG
        ) continue;
        if (!Option->GroupHeader) CreateGroupHeader(BootOptionList,Option);
	}
}


/**
 * Apply the correct policy to the passed orphaned boot options
 * 
 * @param BootOptionList the master boot option list
 * @param Option The orphaned boot option
 * @param Policy the policy to follow, valid values are 
 *      ORPHAN_BOOT_OPTIONS_POLICY_DELETE
 *      ORPHAN_BOOT_OPTIONS_POLICY_DISABLE
 *      ORPHAN_BOOT_OPTIONS_POLICY_KEEP
 */
VOID ApplyOrphanBootOptionPolicy(
    DLIST *BootOptionList, BOOT_OPTION *Option, const int Policy
){
    if (Policy==ORPHAN_BOOT_OPTIONS_POLICY_DELETE){
        if (Option->BootOptionNumber!=INVALID_BOOT_OPTION_NUMBER){
            CHAR16 BootStr[9];
    		Swprintf(BootStr,L"Boot%04X",Option->BootOptionNumber);
    		pRS->SetVariable(
    			BootStr, &EfiVariableGuid, 0, 0, NULL
    		);
        }    
        DeleteBootOption(BootOptionList, Option);
    }else if (Policy==ORPHAN_BOOT_OPTIONS_POLICY_DISABLE){
        Option->Attributes &= ~LOAD_OPTION_ACTIVE;
    }else if (Policy==ORPHAN_BOOT_OPTIONS_POLICY_HIDE){
        Option->Attributes |= LOAD_OPTION_HIDDEN;
        MaskFilePathList(Option);
    }
}

/**
 * Attempts to find stale boot options in the master boot option list, and apply the specified policy to them. 
 * 
 * @note Policy is based on SDL tokens: FW_ORPHAN_BOOT_OPTIONS_POLICY and NON_FW_ORPHAN_BOOT_OPTIONS_POLICY
 */
VOID PreProcessBootOptions(){
    DLINK *Link;
    BOOT_OPTION *Option;

    // Process boot options not associated with the particular boot device.
    // We can't process group headers just yet because groups that
    // are non-empty now, can become empty at the end of this loop
    // once their orphan members are deleted.
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        //skip group headers for now; we'll process them later 
		//in the PostProcessBootOptions
        if (Option->GroupHeader || IsBootOptionWithDevice(Option))
            continue;
        if (Option->FwBootOption) 
            ApplyOrphanBootOptionPolicy(
                BootOptionList, Option, FW_ORPHAN_BOOT_OPTIONS_POLICY
            );
        else
            ApplyOrphanBootOptionPolicy(
                BootOptionList, Option, NON_FW_ORPHAN_BOOT_OPTIONS_POLICY
            );
    }
}

/**
 * Go through the master boot option list and apply orphan boot option policy to the boot option groups
 */
VOID PostProcessBootOptions(){
    DLINK *Link;
    BOOT_OPTION *Option;

    SortList(BootOptionList, CompareTagThenPriority);
    //Now we are ready for the processing of orphan group headers.
    //process empty groups ( groups with just the header)
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        if (!Option->GroupHeader) continue;
        if (   Link==NULL 
            || OUTTER(Link,Link,BOOT_OPTION)->Tag != Option->Tag
        ) ApplyOrphanBootOptionPolicy(
            BootOptionList, Option, ORPHAN_GROUP_HEADERS_POLICY
          );
    }

#if GROUP_BOOT_OPTIONS_BY_TAG
    CreateGroupHeaders(BootOptionList);
#endif
}

/**
 * Based on the passed boot option, determine if a prefix needs prepended to the front 
 * of the boot option name.  If return value is non-zero, then the returned number of 
 * CHAR16s from the Name buffer should prepended to the front of the boot option name
 * 
 * @param Option boot option in question
 * @param Name pointer to the buffer in which to return the prefix
 * @param NameSize size of the buffer being passed
 * 
 * @retval Number of Unicode characters printed into the Name buffer excluding the terminating zero.
 */
UINTN ConstructBootOptionNamePrefixDefault(BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize){
    EFI_STATUS Status;
    if (IsLegacyBootOption(Option)) return 0;
    Status = HiiLibGetString(HiiHandle, STRING_TOKEN(STR_UEFI_PREFIX), &NameSize, Name);
    if(!EFI_ERROR(Status))
        return (NameSize/sizeof(CHAR16))-1;
    else
        return Swprintf(Name, L"UEFI: ");
}

/**
 * Based on the passed boot option, determine if anything needs appended
 * to the boot option name string.  If return value is not zero, then the
 * returned number of characters should be appended to the end of the name
 * string buffer.
 * 
 * @param Option the boot option in question
 * @param Name pointer to the buffer to return the append string
 * @param NameSize the size of the buffer being passed
 */
UINTN ConstructBootOptionNameSuffixDefault(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
){
    EFI_DEVICE_PATH_PROTOCOL *Dp = NULL;
    HARDDRIVE_DEVICE_PATH *HddDp = NULL;

    EFI_STATUS Status;
    CHAR16 Buffer[30];
    UINTN Size = sizeof(Buffer);
    
    if (IsLegacyBootOption(Option)) return 0;
    Status = pBS->HandleProtocol(Option->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID**)&Dp);
    if(!EFI_ERROR(Status)) {
        HddDp = (HARDDRIVE_DEVICE_PATH*)FindDeviceNodeInDevicePath(Dp, MEDIA_DEVICE_PATH, MEDIA_HARDDRIVE_DP);
        if(HddDp != NULL) {
            Status = HiiLibGetString(HiiHandle, STRING_TOKEN(STR_PARTITION), &Size, Buffer);
            if(!EFI_ERROR(Status)) {
                NameSize = Swprintf(Name, L", %s %d", Buffer, HddDp->PartitionNumber);
                return NameSize;
            }
        }
    }
    return 0;
}

extern DEVICE_PATH_TO_DEVICE_MATCH_TEST BOOT_OPTION_DP_MATCHING_FUNCTIONS EndOfDpMatchingFunctions;
DEVICE_PATH_TO_DEVICE_MATCH_TEST *DpMatchingFunction[] = {
    BOOT_OPTION_DP_MATCHING_FUNCTIONS NULL
};

extern BOOT_OPTION_TO_DEVICE_MATCH_TEST BOOT_OPTION_MATCHING_FUNCTIONS EndOfMatchingFunctions;
BOOT_OPTION_TO_DEVICE_MATCH_TEST *MatchingFunction[] = {
    BOOT_OPTION_MATCHING_FUNCTIONS NULL
};

extern FILTER_BOOT_DEVICE_TEST BOOT_OPTION_BOOT_DEVICE_FILTERING_FUNCTIONS EndOfFilteringFunctions;
FILTER_BOOT_DEVICE_TEST *FilteringFunction[] = {
    BOOT_OPTION_BOOT_DEVICE_FILTERING_FUNCTIONS NULL
};

extern CONSTRUCT_BOOT_OPTION_NAME BOOT_OPTION_BUILD_NAME_FUNCTIONS EndOfBuildNameFunctions;
CONSTRUCT_BOOT_OPTION_NAME *BuildNameFunctions[] = {
    BOOT_OPTION_BUILD_NAME_FUNCTIONS NULL
};

extern BUILD_BOOT_OPTION_FILE_PATH_LIST BOOT_OPTION_BUILD_FILE_PATH_FUNCTIONS EndOfBuildFilePathFunctions;
BUILD_BOOT_OPTION_FILE_PATH_LIST *BuildFilePathFunctions[] = {
  BOOT_OPTION_BUILD_FILE_PATH_FUNCTIONS NULL
};

extern CONSTRUCT_BOOT_OPTION_NAME BOOT_OPTION_NAME_PREFIX_FUNCTION;
CONSTRUCT_BOOT_OPTION_NAME *ConstructBootOptionNamePrefix = BOOT_OPTION_NAME_PREFIX_FUNCTION;

extern CONSTRUCT_BOOT_OPTION_NAME BOOT_OPTION_NAME_SUFFIX_FUNCTION;
CONSTRUCT_BOOT_OPTION_NAME *ConstructBootOptionNameSuffix = BOOT_OPTION_NAME_SUFFIX_FUNCTION;

#ifndef BOOT_OPTION_GET_BBS_ENTRY_DEVICE_TYPE_FUNCTION
#define BOOT_OPTION_GET_BBS_ENTRY_DEVICE_TYPE_FUNCTION GetBbsEntryDeviceTypeDefault
#endif
extern GET_BBS_ENTRY_DEVICE_TYPE BOOT_OPTION_GET_BBS_ENTRY_DEVICE_TYPE_FUNCTION;
GET_BBS_ENTRY_DEVICE_TYPE *GetBbsEntryDeviceType = BOOT_OPTION_GET_BBS_ENTRY_DEVICE_TYPE_FUNCTION;

#if FAST_BOOT_SUPPORT
VOID FastBoot();
BOOLEAN IsFastBoot();
/**
 * Helper function which calls the IsFastBoot() function to check if the system in in the
 * fast boot path. If it returns TRUE, then the FastBoot function is called.
 */
VOID FastBootHook(){
    if(IsFastBoot()) FastBoot();
}
#endif

extern BDS_CONTROL_FLOW_FUNCTION BDS_CONTROL_FLOW EndOfBdsControlFlowFunctions;
BDS_CONTROL_FLOW_FUNCTION *BdsControlFlowFunctions[] = {
    BDS_CONTROL_FLOW NULL
};
CHAR8 *BdsControlFlowFunctionNames[] = {
    BDS_CONTROL_FLOW_NAMES NULL
};

CONST CHAR16 *FirmwareVendorString = CONVERT_TO_WSTRING(CORE_VENDOR);
CONST UINT32 FirmwareRevision = CORE_COMBINED_VERSION;

UINT32 BootOptionSupport = BOOT_MANAGER_CAPABILITIES;
UINT64 OsIndicationsSupported = OS_INDICATIONS_SUPPORTED ;

#ifndef BDS_MEMORY_USAGE_INFO_DEFINED
typedef struct{
    UINT32 Type;
    UINT32 Previous,Current,Next;
} BDS_MEMORY_USAGE_INFO;

typedef BOOLEAN (BDS_UPDATE_MEMORY_USAGE_INFORMATION)(BDS_MEMORY_USAGE_INFO *MemoryInfo, UINTN NumberOfElements);
#endif
extern BDS_UPDATE_MEMORY_USAGE_INFORMATION BDS_UPDATE_MEMORY_USAGE_INFORMATION_FUNCTION;
BDS_UPDATE_MEMORY_USAGE_INFORMATION *UpdateMemoryUsageInformation = BDS_UPDATE_MEMORY_USAGE_INFORMATION_FUNCTION;

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
