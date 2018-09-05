#include <Token.h>
#include <AmiDxeLib.h>
#include <Pci.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadFile.h>
#include <Protocol/BlockIo.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LegacyBios.h>

typedef struct{
	DLINK Link;
	UINT32 Attributes;
	CHAR16 *Description;
	EFI_DEVICE_PATH_PROTOCOL *FilePathList;
	UINTN FilePathListLength;
	UINT8 *OptionalData;
    UINTN OptionalDataSize;
	UINT16 BootOptionNumber;
	UINT32 Priority;
	UINT32 Tag; // Just a number. Devices with the same number have something in common; 
				// Can be used as a sort key.
	EFI_HANDLE DeviceHandle;
	UINT16 BbsIndex;
	BBS_TABLE *BbsEntry; 
	BOOLEAN FwBootOption; 
    BOOLEAN GroupHeader;
} BOOT_OPTION;


typedef struct{
	DLINK Link;	
	EFI_HANDLE DeviceHandle;
	UINT16 BbsIndex;
	BBS_TABLE *BbsEntry; 
} BOOT_DEVICE;

typedef BOOLEAN (DEVICE_PATH_TO_DEVICE_MATCH_TEST)(
	EFI_DEVICE_PATH_PROTOCOL *OptionDevicePath, BOOT_DEVICE *Device
);

typedef BOOLEAN (BOOT_OPTION_TO_DEVICE_MATCH_TEST)(
	BOOT_OPTION *Option, BOOT_DEVICE *Device
);

typedef BOOLEAN (FILTER_BOOT_DEVICE_TEST)(BOOT_DEVICE *Device);

typedef UINTN (CONSTRUCT_BOOT_OPTION_NAME)(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
);

typedef UINT16 (GET_BBS_ENTRY_DEVICE_TYPE)(BBS_TABLE *BbsEntry);

typedef BOOLEAN (BUILD_BOOT_OPTION_FILE_PATH_LIST)(BOOT_OPTION *Option);

#pragma pack(push,1)
typedef struct{
	UINT32 Signature;
	UINT32 Size;
	EFI_LOAD_OPTION Option;
} NESTED_BOOT_OPTION;

typedef struct{
// Guid = AMI_BBS_DEVICE_PATH_GUID
	VENDOR_DEVICE_PATH Header;
	UINT8 Bus;
	UINT8 Device;
	UINT8 Function;
	UINT8 Class;
    UINT8 SubClass;
    UINT8 Instance;
}AMI_BBS_DEVICE_PATH;

typedef struct{
// Guid = AMI_DEVICE_NAME_DEVICE_PATH_GUID
	VENDOR_DEVICE_PATH Header;
// UINT8 DeviceName[n]
// n = <node length from the header> - sizeof(AMI_DEVICE_NAME_DEVICE_PATH)
}AMI_DEVICE_NAME_DEVICE_PATH;
#pragma pack(pop)

typedef INT32 ( *COMPARE_FUNCTION ) (DLINK *p, DLINK *q);

typedef VOID (BDS_CONTROL_FLOW_FUNCTION)();

#define INVALID_HANDLE NULL
#define INVALID_BBS_INDEX 0xFFFF
#define INVALID_BOOT_OPTION_NUMBER 0xFFFF
#define LOWEST_BOOT_OPTION_PRIORITY 0xFFFFFFFF
#define UNASSIGNED_LOWEST_TAG 0
#define UNASSIGNED_HIGHEST_TAG 0xFFFFFFFF
#define INVALID_STR_TOKEN 0xFFFF
// The upper two bytes of the signature must be zeroes.
// Some EFI applications (for example UEFI Shell) treats optional data 
// as a command line string.
// These applications will treat this signature as an empty Unicode string.
#define AMI_BOOT_OPTION_SIGNATURE(Letter) (('O'<<24)+((Letter)<<16))

#define GetBaseTag(Tag) ((Tag)&0xFFFF)
#define BootOptionTag(BaseTag,Priority) ((BaseTag)|((Priority)<<16))

#define AMI_SIMPLE_BOOT_OPTION_SIGNATURE AMI_BOOT_OPTION_SIGNATURE('B')//'AMBO'
#define AMI_GROUP_BOOT_OPTION_SIGNATURE AMI_BOOT_OPTION_SIGNATURE('G')//'AMGO'
#define AMI_NESTED_BOOT_OPTION_SIGNATURE AMI_BOOT_OPTION_SIGNATURE('N')//'AMNO'
#define AMI_NESTED_BOOT_OPTION_HEADER_SIZE EFI_FIELD_OFFSET(NESTED_BOOT_OPTION,Option)

#define STARTING_BOOT_OPTION_PRIORITY 0x100
#define DEFAULT_PRIORITY_INCREMENT 0x100
#define GetNextBootOptionPriority(CurrentPriority) ((CurrentPriority)+DEFAULT_PRIORITY_INCREMENT)
#define IndexToBootPriority(Index) (STARTING_BOOT_OPTION_PRIORITY+(Index)*DEFAULT_PRIORITY_INCREMENT)

#define IsLegacyBootOption(Option) (\
    ((Option)->FilePathList==NULL) ? (Option)->BbsEntry!=NULL : (Option)->FilePathList->Type==BBS_DEVICE_PATH\
)
#define IsBootOptionWithDevice(Option) ((Option)->BbsEntry!=NULL || (Option)->DeviceHandle!=INVALID_HANDLE)

#define BOOT_VARIABLE_ATTRIBUTES \
	(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)

#define AMI_BBS_DEVICE_PATH_GUID \
	{ 0x1db184ae, 0x81f5, 0x4e72, { 0x85, 0x44, 0x2b, 0xab, 0xc, 0x2c, 0xac, 0x5c } }

#define AMI_MASKED_DEVICE_PATH_GUID \
    { 0x99e275e7, 0x75a0, 0x4b37, { 0xa2, 0xe6, 0xc5, 0x38, 0x5e, 0x6c, 0x0, 0xcb } }

#define AMI_DEVICE_NAME_DEVICE_PATH_GUID \
    { 0x2d6447ef, 0x3bc9, 0x41a0, { 0xac, 0x19, 0x4d, 0x51, 0xd0, 0x1b, 0x4c, 0xe6 } }

#define SHELL_GUID \
	{0xc57ad6b7,0x0515,0x40a8,{0x9d,0x21,0x55,0x16,0x52,0x85,0x4e,0x37}}

#define AMI_MEDIA_DEVICE_PATH_GUID \
    { 0x5023b95c, 0xdb26, 0x429b, { 0xa6, 0x48, 0xbd, 0x47, 0x66, 0x4c, 0x80, 0x12 } }

#define FOR_EACH_LIST_ELEMENT(FirstLinkPtr,LinkPtr,ElementPtr,ELEMENT_TYPE)\
	for(  LinkPtr=(FirstLinkPtr)\
        ;    LinkPtr!=NULL\
          && (ElementPtr=OUTTER(LinkPtr,Link,ELEMENT_TYPE),LinkPtr=LinkPtr->pNext,TRUE)\
        ;\
    )

#define FOR_EACH_BOOT_OPTION(List,Link,Element)\
    FOR_EACH_LIST_ELEMENT((List)->pHead,Link,Element,BOOT_OPTION)
#define FOR_EACH_BOOT_DEVICE(List,Link,Element)\
    FOR_EACH_LIST_ELEMENT((List)->pHead,Link,Element,BOOT_DEVICE)

#define NUMBER_OF_BBS_DEVICE_TYPES 8
#define BBS_DEVICE_TYPE_TO_INDEX(DeviceType)\
    (  (DeviceType)<=BBS_EMBED_NETWORK && (DeviceType)!=0\
     ? (DeviceType)-1\
     :    (DeviceType)==BBS_BEV_DEVICE\
        ? NUMBER_OF_BBS_DEVICE_TYPES-2\
        : NUMBER_OF_BBS_DEVICE_TYPES-1\
    )
#ifdef EFI_DEBUG
#define DUMP_BOOT_OPTION_LIST(List,Caption) DumpBootOptionList(List,Caption)
#else
#define DUMP_BOOT_OPTION_LIST(List,Caption)
#endif

#define ORPHAN_BOOT_OPTIONS_POLICY_KEEP 0
#define ORPHAN_BOOT_OPTIONS_POLICY_DELETE 1
#define ORPHAN_BOOT_OPTIONS_POLICY_DISABLE 2
#define ORPHAN_BOOT_OPTIONS_POLICY_HIDE 3

extern EFI_GUID EfiVariableGuid;
extern EFI_GUID AmiBbsDevicePathGuid;
extern EFI_GUID AmiMaskedDevicePathGuid;
extern DEVICE_PATH_TO_DEVICE_MATCH_TEST *DpMatchingFunction[];
extern BOOT_OPTION_TO_DEVICE_MATCH_TEST *MatchingFunction[];
extern FILTER_BOOT_DEVICE_TEST *FilteringFunction[];
extern CONSTRUCT_BOOT_OPTION_NAME *BuildNameFunctions[];
extern BUILD_BOOT_OPTION_FILE_PATH_LIST *BuildFilePathFunctions[];
extern EFI_HII_HANDLE HiiHandle;
extern const STRING_REF UnknownDeviceToken;
extern CONSTRUCT_BOOT_OPTION_NAME *ConstructBootOptionNamePrefix;
extern CONSTRUCT_BOOT_OPTION_NAME *ConstructBootOptionNameSuffix;
extern GET_BBS_ENTRY_DEVICE_TYPE *GetBbsEntryDeviceType;
extern BOOLEAN NormalizeBootOptionName;
extern BOOLEAN NormalizeBootOptionDevicePath;
extern DLIST *BootOptionList;
extern DLIST *BootDeviceList;

#define	LEGACY_DEV_ORDER_GUID	\
	{ 0xA56074DB, 0x65FE, 0x45F7, 0xBD, 0x21, 0x2D, 0x2B, 0xDD, 0x8E, 0x96, 0x52 }

typedef struct{
	UINT32 Type;
	UINT16 Length;
	UINT16 Device[1]; 
} LEGACY_DEVICE_ORDER;

extern EFI_GUID LegacyDevOrderGuid;

//defined in BootOptions.c
VOID SortList(DLIST *List, COMPARE_FUNCTION Compare);
INT32 CompareTagThenPriority(DLINK *Link1, DLINK *Link2);
INT32 ComparePriorityThenBootOptionNumber(DLINK *Link1, DLINK *Link2);

BOOT_OPTION* CreateBootOption(DLIST *BootOptionList);
VOID DeleteBootOption(DLIST *BootOptionList, BOOT_OPTION *Option);
BOOT_DEVICE* CreateBootDevice(
    DLIST *BootDeviceList, EFI_HANDLE DeviceHandle, 
	UINT16 BbsIndex, BBS_TABLE *BbsEntry
);
VOID DeleteBootDevice(DLIST *BootDeviceList, BOOT_DEVICE* Device);
VOID UpdateBootOptionWithBootDeviceInfo(
    BOOT_OPTION *Option, BOOT_DEVICE* Device
);

BOOLEAN IsUefiHddBootDevice(BOOT_DEVICE *Device);

BOOLEAN LocateDevicePathTest(
    EFI_DEVICE_PATH_PROTOCOL *OptionDevicePath, BOOT_DEVICE *Device
);
BOOLEAN PartitionDevicePathTest(
	EFI_DEVICE_PATH_PROTOCOL *OptionDevicePath, BOOT_DEVICE *Device
);
BOOLEAN BbsDevicePathTest(
	EFI_DEVICE_PATH_PROTOCOL *OptionDevicePath, BOOT_DEVICE *Device
);
BOOLEAN AmiBbsDevicePathTest(
	EFI_DEVICE_PATH_PROTOCOL *OptionDevicePath, BOOT_DEVICE *Device
);

UINTN ConstructBootOptionNameByHandle(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
);
UINTN ConstructBootOptionNameByBbsDescription(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
);
VOID BuildLegacyDevOrderBuffer(
    LEGACY_DEVICE_ORDER **DevOrderBuffer, UINTN *BufferSize
);
UINTN ConstructBootOptionNameByHandleDevicePath(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
);
UINTN ConstructBootOptionNameByFilePathList(
    BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
);
UINTN ConstructBootOptionName(BOOT_OPTION *Option);

BOOLEAN BuildLegacyFilePath(BOOT_OPTION *Option);
BOOLEAN BuildEfiFilePath(BOOT_OPTION *Option);
BOOLEAN BuildBootOptionFilePath(BOOT_OPTION *Option);

VOID DumpBootOptionList(DLIST *BootOptionList, CHAR8 *ListCaption);
VOID MaskFilePathList(BOOT_OPTION *Option);

//defined in BdsBoard.c
UINT16 GetLegacyDevOrderType(BOOT_OPTION *Option);
STRING_REF DevicePathNodeToStrRef(EFI_DEVICE_PATH_PROTOCOL *Dp);
VOID ApplyOrphanBootOptionPolicy(
    DLIST *BootOptionList, BOOT_OPTION *Option, const int Policy
);

