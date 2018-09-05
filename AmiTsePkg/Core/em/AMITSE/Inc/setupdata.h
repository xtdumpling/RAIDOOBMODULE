//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Inc/setupdata.h $
//
// $Author: Rajashakerg $
//
// $Revision: 7 $
//
// $Date: 1/20/12 12:46a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file setupdata.h
    Header file for setup data

**/

#ifndef _SETUPDATA_H_
#define	_SETUPDATA_H_



// forward declarations
typedef struct _SETUP_PKG SETUP_PKG;
typedef struct _SCREEN_INFO SCREEN_INFO;
typedef struct _PAGE_LIST PAGE_LIST;
typedef struct _PAGE_INFO PAGE_INFO;
typedef struct _PAGE_FLAGS PAGE_FLAGS;
typedef struct _CONTROL_LIST CONTROL_LIST;
typedef struct _CONTROL_INFO CONTROL_INFO;
typedef struct _CONTROL_FLAGS CONTROL_FLAGS;
typedef struct _VARIABLE_LIST VARIABLE_LIST;
typedef struct _VARIABLE_INFO VARIABLE_INFO;
typedef struct _HPK_INFO HPK_INFO;
typedef struct _GUID_LIST GUID_LIST;
typedef struct _GUID_FLAGS GUID_FLAGS;
typedef struct _GUID_INFO GUID_INFO;
typedef struct _PAGE_ID_LIST PAGE_ID_LIST;
typedef struct _PAGE_ID_INFO PAGE_ID_INFO;

#define VARIABLE_NAME_LENGTH    40
// Control types
#define	CONTROL_TYPE_NULL			0x0000
#define	CONTROL_TYPE_SUBMENU		0x0001
#define	CONTROL_TYPE_LABEL			0x0002
#define	CONTROL_TYPE_TEXT			0x0003
#define	CONTROL_TYPE_DATE			0x0004
#define	CONTROL_TYPE_TIME			0x0005
#define	CONTROL_TYPE_POPUPSEL		0x0006
#define CONTROL_TYPE_MEMO			0x0008
#define CONTROL_TYPE_MSGBOX			0x0009
#define CONTROL_TYPE_CHECKBOX		0x000A 
#define CONTROL_TYPE_NUMERIC		0x000B
#define CONTROL_TYPE_EDIT			0x000C
#define CONTROL_TYPE_PASSWORD		0x000D
#define CONTROL_TYPE_MENU			0x000E
#define CONTROL_TYPE_ORDERED_LIST	0x000F
#define CONTROL_TYPE_POPUPEDIT		0x0010
#define	CONTROL_TYPE_VARSTORE_SELECT		0x0011 
#define	CONTROL_TYPE_VARSTORE_SELECT_PAIR	0x0012
#define CONTROL_TYPE_POPUP_STRING	0x0013
#define CONTROL_TYPE_VARSTORE		0x0014
#define INCONSISTENT_IF				0x0015

// UEFI 2.1 Defines
#define NO_SUBMIT_IF				0x0016
#define DISABLE_IF					0x0017
#define CONTROL_TYPE_VARSTORE_NAME_VALUE	0x0018
#define CONTROL_TYPE_VARSTORE_EFI			0x0019
#define CONTROL_TYPE_VARSTORE_DEVICE		0x001A
#define	CONTROL_TYPE_ACTION			0x001B
#define	CONTROL_TYPE_RESET			0x001C
#define CONTROL_TYPE_RULE			0x001D
#define CONTROL_TYPE_REF2           0x001E 
#define CONTROL_TYPE_SLIDE			0x001F
#define WARN_IF		0x0020

// Used in Edit.c
#ifndef	IFR_PASSWORD_OP
#define IFR_PASSWORD_OP             0x08
#endif

#if BUILD_OS != BUILD_OS_LINUX
#pragma warning( disable : 4201 )
#endif

#pragma pack(8)
struct _SETUP_PKG
{
	CHAR8 		PackageSignature[4];	//'$SPF'		Signature to identify the package file
	UINT16		StructureRevision;		//0x0100		Revision of this structure 
	UINT32		PackageVersion;			//0x00000100	Version number of the structure data
	UINT8		Reserved[6];			//0				Reserved for future expansion
	EFI_GUID	PackageGuid;			//Varies		EFI_GUID for the entire setup package
	UINT32		PackageVariable;		//0x00000001	ID of the default GUID-Name variable store for this package
	UINT32		PackageScreen;			//Varies		Offset to start of SCREEN_INFO structure
	UINT32		PackagePageList;		//Varies		Offset to start of PAGE_LIST structure
	UINT32		PackageControlOffset;	//Varies		Offset to start of controls
	UINT32		PackageVariableList;	//Varies		Offset to start of VARIABLE_LIST structure
	UINT32		PackageHpkData;			//Varies		Offset to source HPK info that we need
	UINT32		PackageGuidList;		//Varies		List of OEM GUIDs requiring application launch
	UINT32		PackagePageIdList;		//Varies		Offset to list of Page ID triplets guid class subclass
	UINT32		PackageIfrData;			//Varies		IFR data that is used to build our 'static' pages
};

struct _SCREEN_INFO		//Structure containing global information used by all pages
{
	union {
	/*EFI_HII_HANDLE*/VOID *	MainHandle;		//Varies	HII Handle to the formset that contains the global screen info
	UINT8	res[8];
	};
	UINT16			MainTitle;		//Varies	Token for title string
	UINT16			MainCopyright;	//Varies	Token for copyright string
	UINT16			MainHelpTitle;	//Varies	Token for help 'window' title
	UINT16			MainMenuCount;	//Varies	Number of entries in main menu (0 = no main menu)
	UINT16			MainNavCount;	//Varies	Number of nav help strings on the page
	//UINT16 		MenuEntries[];	//Varies	Tokens for menu strings
	//UINT16	 	MainNavToken[];	//Varies	Tokens for all navigational help strings
};

struct _PAGE_LIST	//This is the listing of all the pages in setup
{
	UINT32	PageCount;		//Varies	Number of pages in Setup
	UINT32 	PageList[1];	//Varies	Offsets to each of the page structures
};

struct _PAGE_FLAGS		//Structure containing the attributes global to a page
{
	UINT32 	PageVisible : 1;		//TRUE	Controls whether or not this page is visible
	UINT32 	PageHelpVisible : 1;	//TRUE	Controls whether or not help is visible on this page
	UINT32 	PageDisableHotKeys : 1;	//FALSE	Controls whether globals hot keys are enabled for this page
	UINT32  PageRefresh : 1;        //TRUE Controls whether the page is refreshable
	UINT32  PageModal : 1;          //TRUE Controls whether the page is Modal
	UINT32  PageDynamic : 1;          //TRUE Controls whether the page is Dynamic
    UINT32 	PageStdMap : 1;         // set 0 if Form map is not found otherwise set 1
	UINT32	PageRefreshID : 1;			//TRUE Controls whether the page is having RefreshID 
	UINT32 	Reserved : 24;			//0		Reserved for future use
};

struct _CONTROL_LIST	//Structure containing the offsets of all the control data
{
	UINT32	ControlCount;	//Varies	Number of controls in this list
	UINT32 	ControlList[1];	//Varies	Offsets to each of the control structures
};

struct _PAGE_INFO		//Structure containing the layout of the page
{
	union {
	/*EFI_HII_HANDLE*/VOID *	PageHandle;		//Varies	HII Handle to the formset that contains this control
	UINT8	res[8];
	};
	UINT16			PageIdIndex;	//varies	Index in to PageIdList triplets
	UINT16			PageFormID;		//Varies	Form ID within the formset for this page
	UINT16			PageTitle;		//Varies	Allows a page to override the main title bar
	UINT16			PageSubTitle;	//Varies	Token for the subtitle string for this page
	UINT16			PageID;			//Varies	unique ID for this page
	UINT16			PageParentID;	//Varies	Identify this page's parent page
	PAGE_FLAGS		PageFlags;		//3			Various attributes for a specific page
	UINT32			PageVariable;	//Varies	Overrides the variable ID for this page (0 = use default)
	CONTROL_LIST	PageControls;	//Varies	Listing of controls for this page
};


struct _CONTROL_FLAGS			//Structure containing the attributes for a specific control
{
	UINT32	ControlVisible : 1;	//TRUE	Whether or not a control is shown to the user
#define CONTROL_ACCESS_DEFAULT	0 //Do not override VFR
#define CONTROL_ACCESS_EXT_USER	1 //Admin - Editable; User - Editable;
#define CONTROL_ACCESS_USER		2 //Admin - Editable; User - Visible but not editable;
#define CONTROL_ACCESS_ADMIN	3 //Admin - Editable; User - Invisible;
	UINT32	ControlAccess : 2;	//0		User access to control
	UINT32	ControlReset : 1;	//FALSE	Whether or not changing a value requires a system reset
	UINT32  ControlReadOnly : 1; //FALSE Weather or not the control is read only.
	UINT32 	ControlInteractive : 1;	//FALSE Weather the Control is interactive or not
    UINT32  ControlEvaluateDefault : 1; // FALSE If set Default is obtained by expression evaluation
	UINT32	Reserved1 : 1;		//0		Reserved for future use
	UINT32	ControlRefresh : 8;	//0		Refresh timer (in 0.1s intervals) (0 = refresh disabled) 
	UINT32	RefreshID : 1;				//TRUE or FALSE to denote RefreshID status
	UINT32	ControlRWEvaluate : 1; // set 0 if read/write expression is not found otherwise set 1
	UINT32	ControlReconnect : 1;	//FALSE	Whether or not changing a value requires a system reconnect
	UINT32	Reserved2 : 13;		//0		Reserved for future use
};

struct _CONTROL_INFO		//Structure containing control data
{
	union {
	/*EFI_HII_HANDLE*/VOID *	ControlHandle;			//Varies	HII Handle to the formset that contains this control
	UINT8	res[8];
	};
	UINT32			ControlVariable;		//Varies	Overrides the variable ID for this control (0 = use default)
	union
	{
        UINT32		ControlConditionalVariable[2]; 	//Varies	Overrides the condition variable ID for this control (0 = use default)
		struct
		{
            UINT16	ControlKey;				// Varies	Variable unique identifier same as QuestionID in UEFI2.1
            UINT16  DevicePathId;			// Device Path as specified by VarStoreDevicePath nested within the scope of this question.
            UINT8	DefaultStoreCount;		// Constant No of Defaults (n) values other than Optimal and Failsafe for that control.
			UINT8	Reserved[1]; 			// Reserved space for use in future.
            UINT16  DestQuestionID;         // This is for EFI_IFR_REFX support
        };
	};
	UINT16			ControlType;			//Varies	Type of the control on the page
	UINT16			ControlPageID;			//Varies	Page ID that contains this control
	UINT16			ControlDestPageID;		//FFFFh		Only needed for controls of type CONTROL_TYPE_SUBMENU
	CONTROL_FLAGS	ControlFlags;			//1			Various attributes for a specific control
	UINT16			ControlHelp;			//Varies	Token for help string for this control
	UINT16			ControlLabel;			//Varies	One-based label number that this control is 'linked to'
	UINT16			ControlIndex;			//Varies	Zero-based control number from the label, not formset
	union{
	UINT16			ControlLabelCount;		//Varies	number of opcodes associated with this control's label
	UINT16 			PageIdIndex;
	};
	union {
	VOID *			ControlPtr;				//Varies	Pointer to control data in HII
	UINT64	res1;
	};
	union {
	VOID *			ControlConditionalPtr;	//Varies	Pointer to control condition data in HII
	UINT64	res2;
	};
	UINT16			ControlDataLength;		//Varies	Length of HII data for control
	UINT16			ControlDataWidth;		//Varies	Width of data (in bytes) for this control
	UINT16			QuestionId;				//Varies	Offset for data of this control in the Variable
	//UINT8			ControlFailsafeValue;	//Varies	Used when user selects load failsafe value
	//UINT8 		ControlOptimalValue;	//Varies	Used when user selects load optimal value
};

struct _VARIABLE_LIST
{
	UINT32	VariableCount;		//Varies	Number of GUID-Name pairs in this list
	UINT32 	VariableList[1];	//Varies	Offsets to each of the variable structures
};

struct _VARIABLE_INFO			//Structure containing GUID-Name pairs for variable access
{
	EFI_GUID	VariableGuid;		//Varies	EFI_GUID for the NVRAM variable store for this data
	CHAR16 		VariableName[VARIABLE_NAME_LENGTH];	//Varies	Variable store name
	UINT32		VariableAttributes;	//Varies	Attributes of the variable
	UINT32		VariableID;			//Varies	unique variable number, 0x0FFFF for our own variables.
#define	VARIABLE_ATTRIBUTE_VARSTORE		0x0000	// Buffer varstore accessed via Config. Access protocol exposed by the driver.
#define	VARIABLE_ATTRIBUTE_EFI_VARSTORE	0x1000	// EFI Variable, GetVeriable and SetVariable Can be used.
#define	VARIABLE_ATTRIBUTE_NAMEVALUE	0x2000	// Namevalue varstore accessed via Config. Access protocol exposed by the driver.
#define AMI_SPECIAL_NO_DEFAULTS			0x00000001
#define AMI_SPECIAL_VARIABLE_NO_SET		0x00000002
#define AMI_SPECIAL_VARIABLE_NO_GET		0x00000004
	UINT32		ExtendedAttibutes;	//Varies	Extended Attributes to identify type of Variable.
	UINT16		VariableNameId;		//Varies	Id for the Variable Name
	UINT8		Reserved[2];		// Reserved space for use in future.
	union
	{
		/*EFI_HII_HANDLE*/VOID*	VariableHandle;	//Varies	HII Handle to the formset that contains this VarStore
		UINT64	res;
	};
	UINT16		VariableSize;		//Varies	The size of Buffer Varstore
};

struct _HPK_INFO
{
	UINT32	Length;
	UINT16	Class;
	UINT16	SubClass;
};

struct _GUID_LIST
{
	UINT32	GuidCount;		//Varies	Number of GUID-Name pairs in this list
	UINT32 	GuidList[1];	//Varies	Offsets to each of the variable structures
};

struct _GUID_FLAGS
{
	UINT32		GuidLaunchCallback : 1;
	UINT32		GuidLaunchGuid : 1;
	UINT32		GuidLaunchFilename : 1;
	UINT32		GuidLaunchCallbackUpdateTemplate : 1;
	UINT32		Reserved : 28;
};

struct _GUID_INFO
{
	EFI_GUID	GuidValue;
	CHAR16		GuidName[VARIABLE_NAME_LENGTH];
	GUID_FLAGS	GuidFlags;
	UINT32		GuidKey;
};

struct _PAGE_ID_LIST
{
	UINT32	PageIdCount;	//Varies	Number of guid class subclass triplets
	UINT32 	PageIdList[1];	//Varies	Offsets to each of the PageId structures
};

struct _PAGE_ID_INFO			//Structure containing page id triplets guid class and subclass
{
	EFI_GUID	PageGuid;		//Varies	EFI_GUID for the pages
	UINT16		PageClass;		//Varies	Class for the pages
	UINT16		PageSubClass;	//Varies	SubClass for the pages
};

#pragma pack()

#endif /* _SETUPDATA_H_ */


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
