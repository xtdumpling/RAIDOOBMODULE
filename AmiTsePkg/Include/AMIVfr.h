//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Add Password Check setup item in setup menu security page.
//    Reason:
//    Auditor:  Jacker Yeh (Refer from Grantley)
//    Date:     Oct/27/2016
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Include/AMIVfr.h $
//
// $Author: Arunsb $
//
// $Revision: 10 $
//
// $Date: 1/30/12 3:30a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		AMIVfr.h
//
// Description:	VFR related header
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _AMI_VFR_H_
#define _AMI_VFR_H_

#include <token.h>  //SMCPKG_SUPPORT

#ifndef EFI_GLOBAL_VARIABLE_GUID
#define	EFI_GLOBAL_VARIABLE_GUID	\
	{ 0x8BE4DF61, 0x93CA, 0x11D2, 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C }
#endif

#define	SYSTEM_ACCESS_GUID	\
	{ 0xE770BB69, 0xBCB4, 0x4D04, 0x9E, 0x97, 0x23, 0xFF, 0x94, 0x56, 0xFE, 0xAC }

#define	LEGACY_DEV_ORDER_GUID	\
	{ 0xA56074DB, 0x65FE, 0x45F7, 0xBD, 0x21, 0x2D, 0x2B, 0xDD, 0x8E, 0x96, 0x52 }

#define	BOOT_MANAGER_GUID \
	{ 0xB4909CF3, 0x7B93, 0x4751, 0x9B, 0xD8, 0x5B, 0xA8, 0x22, 0x0B, 0x9B, 0xB2 }

#define BOOT_NOW_COUNT_GUID \
	{ 0x052e6eb0, 0xf240, 0x42c5, 0x83, 0x09, 0x45, 0x87, 0x45, 0x45, 0xc6, 0xb4 }

#define	AMI_CALLBACK_GUID	\
	{ 0x9CF0F18E, 0x7C7D, 0x49DE, 0xB5, 0xAA, 0xBB, 0xBA, 0xD6, 0xB2, 0x10, 0x07 }

#define	ERROR_MANAGER_GUID	\
	{ 0xADDEBF82, 0xA560, 0x46B9, 0xA2, 0x80, 0x78, 0xC6, 0xAB, 0x61, 0xAE, 0xDA }

#define	DEL_BOOT_OPTION_GUID \
	{ 0xF6C73719, 0xF34C, 0x479C, 0xB3, 0x2F, 0x27, 0x7F, 0xCB, 0xBC, 0xFE, 0x4F }

#define	ADD_BOOT_OPTION_GUID \
    { 0x19d96d3f, 0x6a6a, 0x47d2, 0xb1, 0x95, 0x7b, 0x24, 0x32, 0xda, 0x3b, 0xe2 }

#define	USER_DEFAULTS_GUID	\
    { 0xC4CC0DE8, 0x0687, 0x4422, 0x99, 0xC1, 0x65, 0x35, 0x1A, 0x5D, 0x5F, 0x95 }

#ifndef EFI_SHELL_FILE_GUID
#define EFI_SHELL_FILE_GUID  \
	{ 0xc57ad6b7, 0x0515, 0x40a8, 0x9d, 0x21, 0x55, 0x16, 0x52, 0x85, 0x4e, 0x37 }
#endif

#ifndef EFI_SHELL2_0_FILE_GUID
#define EFI_SHELL2_0_FILE_GUID  \
	{ 0x6302d008, 0x7f9b, 0x4f30, 0x87, 0xac, 0x60, 0xc9, 0xfe, 0xf5, 0xda, 0x4e }
//	{ 0x7C04A583, 0x9E3E, 0x4f1c, 0xAD, 0x65, 0xE0, 0x52, 0x68, 0xD0, 0xB4, 0xD1}
#endif

#define	AMITSESETUP_GUID	\
	{ 0xc811fa38, 0x42c8, 0x4579, 0xa9, 0xbb, 0x60, 0xe9, 0x4e, 0xdd, 0xfb, 0x34 }

#define IDE_SECURITY_CONFIG_GUID \
	{ 0x3dd0de67, 0x2d7, 0x4129, 0x91, 0x4a, 0x9f, 0x37, 0x7c, 0xc3, 0x4b, 0xd}

// TSE Notifications in single place.
// These #define removed from setup.h and minisetup.h.
#define AMITSE_SETUP_ENTER_GUID \
  { 0x71202EEE, 0x5F53, 0x40d9, 0xAB, 0x3D, 0x9E, 0x0C, 0x26, 0xD9, 0x66, 0x57 }

#define AMITSE_AFTER_FIRST_BOOT_OPTION_GUID \
  { 0xC48D651C, 0x9D0E, 0x4ce7, 0xAD, 0x39, 0xED, 0xD1, 0xAB, 0x83, 0x6B, 0x30 }


// {1CBFC9B6-7F1E-4e9e-801F-FBCE4D92A76D}
#define AMITSE_BEFORE_TIMEOUT_GUID \
	{ 0x1cbfc9b6, 0x7f1e, 0x4e9e, 0x80, 0x1f, 0xfb, 0xce, 0x4d, 0x92, 0xa7, 0x6d }

// {CAD40D6E-C871-4398-910D-2D74EE94711A}
#define AMITSE_AFTER_TIMEOUT_GUID \
	{ 0xcad40d6e, 0xc871, 0x4398, 0x91, 0xd, 0x2d, 0x74, 0xee, 0x94, 0x71, 0x1a }


// {3677770F-EFB2-43b2-B8AE-B302E9604882}
#define AMITSE_EVENT_BEFORE_BOOT_GUID \
  { 0x3677770f, 0xefb2, 0x43b2, 0xb8, 0xae, 0xb3, 0x2, 0xe9, 0x60, 0x48, 0x82 }

// {8c12A959-70BC-4362-B437-bBB80514A1916E}
#define AMITSE_EVENT_AFTER_BOOT_GUID \
  {0x8c12a959, 0x70bc, 0x4362, 0xb4, 0x37, 0xb8, 0x05, 0x14, 0xa1, 0x91, 0x6e}

// {1B6BC809-C986-4937-934F-1EA58622FE50}
#define AMITSE_BOOT_ORDER_CHANGE_GUID \
{ 0x1b6bc809, 0xc986, 0x4937, 0x93, 0x4f, 0x1e, 0xa5, 0x86, 0x22, 0xfe, 0x50 }

// {D84BEFF0-159A-4b60-9AB9-AC5C474BD3B1}
#define AMITSE_NVRAM_UPDATE_GUID \
{ 0xd84beff0, 0x159a, 0x4b60, 0x9a, 0xb9, 0xac, 0x5c, 0x47, 0x4b, 0xd3, 0xb1 }

// {073E7E01-2611-4e85-B896-A3B6767CBA00}
#define AMITSE_PASSWORD_PROMPT_ENTER_GUID \
{ 0x73e7e01, 0x2611, 0x4e85, 0xb8, 0x96, 0xa3, 0xb6, 0x76, 0x7c, 0xba, 0x0 }

// {B9B038B0-E2B6-4aab-9435-4165ECFED032}
#define AMITSE_PASSWORD_PROMPT_EXIT_GUID \
{ 0xb9b038b0, 0xe2b6, 0x4aab, 0x94, 0x35, 0x41, 0x65, 0xec, 0xfe, 0xd0, 0x32 }

// {AB1404CA-4801-4208-98BF-30D521DAD4D3}
#define AMITSE_USER_PASSWORD_VALID_GUID \
{ 0xab1404ca, 0x4801, 0x4208, 0x98, 0xbf, 0x30, 0xd5, 0x21, 0xda, 0xd4, 0xd3 }

// {541D5A75-95EE-43c7-9E5D-2394DC486249}
#define AMITSE_ADMIN_PASSWORD_VALID_GUID \
{ 0x541d5a75, 0x95ee, 0x43c7, 0x9e, 0x5d, 0x23, 0x94, 0xdc, 0x48, 0x62, 0x49 }

// {D69240C0-DD40-4f2d-9863-4848DA6E615F}
#define AMITSE_INVALID_PASSWORD_GUID \
{ 0xd69240c0, 0xdd40, 0x4f2d, 0x98, 0x63, 0x48, 0x48, 0xda, 0x6e, 0x61, 0x5f }

#define	DYNAMIC_PAGE_COUNT_GUID	\
    { 0xb63bf800, 0xf267, 0x4f55, 0x92, 0x17, 0xe9, 0x7f, 0xb3, 0xb6, 0x98, 0x46 }

// 537F1F99-6FC4-40b5-80B2-390B0753C4D0
#define DYNAMIC_PAGE_GROUP_COUNT_GUID  \
{ 0x537f1f99, 0x6fc4, 0x40b5, 0x80, 0xb2, 0x39, 0xb, 0x7, 0x53, 0xc4, 0xd0 }

// DD616C5C-7CFC-41dc-B8D9-CD24F6BB3A5D
#define DYNAMIC_PAGE_DEVICE_COUNT_GUID  \
{ 0xdd616c5c, 0x7cfc, 0x41dc, 0xb8, 0xd9, 0xcd, 0x24, 0xf6, 0xbb, 0x3a, 0x5d }

//551941E5-0011-4D7F-83ED-E462CBABCCC9
#define DYNAMIC_PAGE_GROUP_CLASS_COUNT_GUID  \
{ 0x551941e5, 0x0011, 0x4d7f, 0x83, 0xed, 0xe4, 0x62, 0xcb, 0xab, 0xcc, 0xc9 }

// {0885F288-418C-4be1-A6AF-8BAD61DA08FE}
#define AMITSE_DRIVER_HEALTH_ENB_GUID \
    { 0x885f288, 0x418c, 0x4be1, 0xa6, 0xaf, 0x8b, 0xad, 0x61, 0xda, 0x8, 0xfe }

// {7459A7D4-6533-4480-BBA7-79E25A4443C9}
#define	AMITSE_DRIVER_HEALTH_GUID	\
    { 0x7459a7d4, 0x6533, 0x4480, 0xbb, 0xa7, 0x79, 0xe2, 0x5a, 0x44, 0x43, 0xc9 }

// {58279C2D-FB19-466e-B42E-CD437016DC25}
#define	AMITSE_DRIVER_HEALTH_CTRL_GUID	\
    { 0x58279c2d, 0xfb19, 0x466e, 0xb4, 0x2e, 0xcd, 0x43, 0x70, 0x16, 0xdc, 0x25 }

// {C0B4FB05-15E5-4588-9FE9-B3D39C067715}
#define	DRIVER_MANAGER_GUID  \
	{ 0xc0b4fb05, 0x15e5, 0x4588, 0x9f, 0xe9, 0xb3, 0xd3, 0x9c, 0x6, 0x77, 0x15 }
	
	// {17A3C5DA-F51F-437c-91B6-002B6F262593}
#define	DEL_DRIVER_OPTION_GUID \
	{ 0x17a3c5da, 0xf51f, 0x437c, 0x91, 0xb6, 0x0, 0x2b, 0x6f, 0x26, 0x25, 0x93}

#define	ADD_DRIVER_OPTION_GUID \
    { 0x7df66c0f, 0xd40a, 0x4748, 0xac, 0x5a, 0x6c, 0xb, 0xea, 0x78, 0x6a, 0x1a}

#define	OEM_TSE_VAR_GUID	\
	{ 0xF4B2C007, 0x94A1, 0x4cd5, 0xA7, 0x10, 0xf4, 0x14, 0x1f, 0xce, 0xbc, 0xA0 }

//EDA2B104-7A10-4519-B0A1-EBA5C52ACFCE
#define	OEM_PORT_VARIABEL1 \
	{ 0xEDA2B104, 0x7A10, 0x4519, 0xB0, 0xA1, 0xEB, 0xA5, 0xC5, 0x2A, 0xCF, 0xCE }

//EF402953-B819-4cc2-A44C-4C9B4CFBC889
#define	OEM_PORT_VARIABEL2 \
	{ 0xEF402953, 0xB819, 0x4CC2, 0xA4, 0x4C, 0x4C, 0x9B, 0x4C, 0xFB, 0xC8, 0x89 }

//1D6F853E-0006-40d8-9B4B-79618A5733B0
#define	OEM_PORT_VARIABEL3 \
	{ 0x1D6F853E, 0x0006, 0x40d8, 0x9B, 0x4B, 0x79, 0x61, 0x8A, 0x57, 0x33, 0xB0 }

//47E87E67-FBC0-4c01-9002-4A0A6ECB92FF
#define	OEM_PORT_VARIABEL4 \
	{ 0x47E87E67, 0xFBC0, 0x4c01, 0x90, 0x02, 0x4A, 0x0A, 0x6E, 0xCB, 0x92, 0xFF }

//2799A453-FC10-4334-9E66-D3D6EF09D0EE
#define	OEM_PORT_VARIABEL5 \
	{ 0x2799A453, 0xFC10, 0x4334, 0x9E, 0x66, 0xD3, 0xD6, 0xEF, 0x09, 0xD0, 0xEE }

//D77A3FE1-51FE-4613-A81D-82AE24464CFD
#define	OEM_PORT_VARIABEL6 \
	{ 0xD77A3FE1, 0x51FE, 0x4613, 0xA8, 0x1D, 0x82, 0xAE, 0x24, 0x46, 0x4C, 0xFD }

//B5CEC017-74CB-4e10-BAC3-924A4CC629C8
#define	OEM_PORT_VARIABEL7 \
	{ 0xB5CEC017, 0x74CB, 0x4e10, 0xBA, 0xC3, 0x92, 0x4A, 0x4C, 0xC6, 0x29, 0xC8 }

//78A6FB37-8F27-40a4-9C4E-9FC9E438D5E2
#define	OEM_PORT_VARIABEL8 \
	{0x78A6FB37, 0x8F27, 0x40a4, 0x9C, 0x4E, 0x9F, 0xC9, 0xE4, 0x38, 0xD5, 0xE2 }

//8DAC5195-3FD5-4e32-9C2A-1A65BD699932
#define	OEM_PORT_VARIABEL9 \
	{ 0x8DAC5195, 0x3FD5, 0x4e32,0x9C, 0x2A, 0x1A, 0x65, 0xBD, 0x69, 0x99, 0x32 }

//A6DB3378-CFD7-4bfd-A1C4-2709FEC52F8B
#define	OEM_PORT_VARIABEL10 \
	{ 0xA6DB3378, 0xCFD7, 0x4bfd, 0xA1, 0xC4, 0x27, 0x09, 0xFE, 0xC5, 0x2F, 0x8B }

//EA629472-D7C6-4c0e-8820-2B811C7EE3AF
#define	OEM_PORT_VARIABEL11 \
	{ 0xEA629472, 0xD7C6, 0x4c0e, 0x88, 0x20, 0x2B, 0x81, 0x1C, 0x7E, 0xE3, 0xAF }

//4CD97B67-12BB-4fc5-8B2A-580ADB6A6368
#define	OEM_PORT_VARIABEL12 \
	{ 0x4CD97B67, 0x12BB, 0x4fc5, 0x8B, 0x2A, 0x58, 0x0A, 0xDB, 0x6A, 0x63, 0x68 }

//C03DDD47-DC0A-4319-9FEC-44B078C242E6
#define	OEM_PORT_VARIABEL13 \
	{ 0xC03DDD47, 0xDC0A, 0x4319, 0x9F, 0xEC, 0x44, 0xB0, 0x78, 0xC2, 0x42, 0xE6 }

//2AAEC318-E84B-4e0b-BF41-AC65F17E9E2F
#define	OEM_PORT_VARIABEL14 \
	{ 0x2AAEC318, 0xE84B, 0x4e0b, 0xBF, 0x41, 0xAC, 0x65, 0xF1, 0x7E, 0x9E, 0x2F }

//1967167B-A13D-4e45-801B-A605D11946D5
#define	OEM_PORT_VARIABEL15 \
	{ 0x1967167B, 0xA13D, 0x4e45, 0x80, 0x1B, 0xA6, 0x05, 0xD1, 0x19, 0x46, 0xD5}

//466A7291-50DF-4b40-A2EB-CEE67D5428C9
#define	OEM_PORT_VARIABEL16 \
	{ 0x466A7291, 0x50DF, 0x4b40, 0xA2, 0xEB, 0xCE, 0xE6, 0x7D, 0x54, 0x28, 0xC9 }

//4639A701-74F9-4635-B126-9FF048D6B0D0
#define	OEM_PORT_VARIABEL17 \
	{ 0x4639A701, 0x74F9, 0x4635, 0xB1, 0x26, 0x9F, 0xF0, 0x48, 0xD6, 0xB0, 0xD0 }

//EF02529C-A283-48cd-8D08-6A1E9C3C03D3
#define	OEM_PORT_VARIABEL18 \
	{ 0xEF02529C, 0xA283, 0x48cd, 0x8D, 0x08, 0x6A, 0x1E, 0x9C, 0x3C, 0x03, 0xD3 }

//4CD28F5E-4BBB-4602-ACB8-741BAA7EB313
#define	OEM_PORT_VARIABEL19 \
	{ 0x4CD28F5E, 0x4BBB, 0x4602, 0xAC, 0xB8, 0x74, 0x1B, 0xAA, 0x7E, 0xB3, 0x13 }

//83C233EB-D01A-45d8-9FA1-AF40206FCC9B
#define	OEM_PORT_VARIABEL20 \
	{ 0x83C233EB, 0xD01A, 0x45d8, 0x9F, 0xA1, 0xAF, 0x40, 0x20, 0x6F, 0xCC, 0x9B }

//6169D4F6-0659-4fb5-B03C-3645D8C80DC8
#define	OEM_PORT_VARIABEL21 \
	{ 0x6169D4F6, 0x0659, 0x4fb5, 0xB0, 0x3C, 0x36, 0x45, 0xD8, 0xC8, 0x0D, 0xC8 }

//1C1EAB81-9229-42ac-B940-757C498A09ED
#define	OEM_PORT_VARIABEL22 \
	{ 0x1C1EAB81, 0x9229, 0x42ac, 0xB9, 0x40, 0x75, 0x7C, 0x49, 0x8A, 0x09, 0xED }

//0ECEDD30-67EC-4570-9EFB-308DE53EE93D
#define	OEM_PORT_VARIABEL23 \
	{ 0x0ECEDD30, 0x67EC, 0x4570, 0x9E, 0xFB, 0x30, 0x8D, 0xE5, 0x3E, 0xE9, 0x3D }

//913FA238-7B16-4b72-A5F0-815E7D2C8C1F
#define	OEM_PORT_VARIABEL24 \
	{ 0x913FA238, 0x7B16, 0x4b72, 0xA5, 0xF0, 0x81, 0x5E, 0x7D, 0x2C, 0x8C, 0x1F }

//FE075D44-D9F1-4da1-AE6A-8250553CDE66
#define	OEM_PORT_VARIABEL25 \
	{ 0xFE075D44, 0xD9F1, 0x4da1, 0xAE, 0x6A, 0x82, 0x50, 0x55, 0x3C, 0xDE, 0x66 }

//471F5144-6037-4952-8B01-CFF586FDE6B3
#define	OEM_PORT_VARIABEL26 \
	{ 0x471F5144, 0x6037, 0x4952, 0x8B, 0x01, 0xCF, 0xF5, 0x86, 0xFD, 0xE6, 0xB3}

//1318FED0-2765-42b5-803E-3215A3C093C5
#define	OEM_PORT_VARIABEL27 \
	{ 0x1318FED0, 0x2765, 0x42b5, 0x80, 0x3E, 0x32, 0x15, 0xA3, 0xC0, 0x93, 0xC5 }

//FF5A0DC3-DFAC-434a-B3DF-FFD4C1EA7E41
#define	OEM_PORT_VARIABEL28 \
	{ 0xFF5A0DC3, 0xDFAC, 0x434a, 0xB3, 0xDF, 0xFF, 0xD4, 0xC1, 0xEA, 0x7E, 0x41 }

//3F8DC333-CCD5-4e58-B8CF-7B9A8F31EA96
#define	OEM_PORT_VARIABEL29 \
	{ 0x3F8DC333, 0xCCD5, 0x4e58, 0xB8, 0xCF, 0x7B, 0x9A, 0x8F, 0x31, 0xEA, 0x96 }

#define	OEM_PORT_GUID1	\
	{ 0xecfd4bce, 0x4279, 0x40f8, 0xba, 0xf2, 0xdc, 0xb7, 0x96, 0x38, 0xd4, 0x1e }

#define	OEM_PORT_GUID2	\
	{ 0x23f69cd0, 0xff46, 0x4db6, 0xb9, 0x82, 0x63, 0xed, 0xf1, 0xa9, 0x1, 0xff }

//{06449bc5-4494-4f8c-8773-1d2464c0e7ec}
#define AMITSE_AFTER_LAST_BOOT_OPTION_GUID \
  { 0x06449bc5, 0x4494, 0x4f8c, 0x87, 0x73, 0x1d, 0x24, 0x64, 0xc0, 0xe7, 0xec }

//{B4D311E6-721D-4051-956D-2410C1D789AA}
#define AMITSE_SWITCHING_TO_POST_SCREEN_GUID \
  { 0xB4D311E6, 0x721D, 0x4051, 0x95, 0x6D, 0x24, 0x10, 0xC1, 0xD7, 0x89, 0xAA }

// {FDFBBFAB-BC61-4961-B917-77445641507B}
#define AMITSE_AFTER_POST_INVPwD_PROMPT_GUID \
	{ 0xfdfbbfab, 0xbc61, 0x4961, 0xb9, 0x17, 0x77, 0x44, 0x56, 0x41, 0x50, 0x7b }

#define	EXIT_KEY_ID				        0xF001
#define	AMI_CALLBACK_KEY_ID		        0xF002
#define	BOOT_MANAGER_KEY_ID		        0xF003
#define	BOOT_OPTIONS_KEY_ID		        0xF004
#define	BOOT_TIMEOUT_KEY_ID		        0xF005
#define	BOOT_ORDER_KEY_ID	            0xF006
#define	LEGACY_DEV_KEY_ID		        0xF007
#define	LEGACY_ORDER_KEY_ID	            0xF008
#define	LANGUAGE_KEY_ID			        0xF009
#define	LANGUAGE_CODES_KEY_ID	        0xF00A
#define	EFI_SHELL_KEY_ID		        0xF00B
#define	ERROR_MANAGER_KEY_ID	        0xF00C
#define	NEW_BOOT_OPTION_KEY_ID	        0xF00D
#define	DEL_BOOT_OPTION_KEY_ID	        0xF00E
#define	USER_DEFAULTS_KEY_ID	        0xF00F
#define SYSTEM_ACCESS_KEY		        0xF010
#define ADMIN_PASSWORD_KEY		        0xF011
#define USER_PASSWORD_KEY		        0xF012
#define AMITSESETUP_KEY		            0xF013
#define BOOT_NOW_COUNT_KEY_ID           0xF014
#define	IDE_SECURITY_HD_1_KEY_ID        0xF015
#define LEGACY_GROUP_KEY_ID             0xF016
#define SETUP_SAVE_STATE_INFO_KEY_ID   	0xF017
#define DYNAMIC_PAGE_COUNT_KEY_ID       0xF018
#define DRIVER_HEALTH_ENB_KEY_ID	    0xF019
#define DRIVER_HEALTH_KEY_ID		    0xF020
#define DRIVER_HEALTH_CTRL_KEY_ID	    0xF021
#define	DRIVER_MANAGER_KEY_ID		    0xF022		
#define	DRIVER_ORDER_KEY_ID	            0xF023
#define	NEW_DRIVER_OPTION_KEY_ID	    0xF024
#define	DEL_DRIVER_OPTION_KEY_ID	    0xF025
#define DYNAMIC_PAGE_GROUP_COUNT_KEY_ID  0xF026
#define DYNAMIC_PAGE_DEVICE_COUNT_KEY_ID 0xF027
#define DYNAMIC_PAGE_GROUP_CLASS_COUNT_KEY_ID  0xF028
#define OEM_TSE_VAR_KEY_ID              0xF100

#define	SYSTEM_PASSWORD_ADMIN	0
#define	SYSTEM_PASSWORD_USER	1

#define	SAVE_AND_EXIT_VALUE			1
#define	DISCARD_AND_EXIT_VALUE		2
#define	SAVE_VALUE					3
#define	DISCARD_VALUE				4
#define	RESTORE_DEFAULTS_VALUE		5
#define	SAVE_USER_DEFAULTS_VALUE	6
#define	RESTORE_USER_DEFAULTS_VALUE	7
#define	SAVE_BOOT_OPTION			8
#define	SAVE_AND_RESET_VALUE		9
#define	DISCARD_AND_RESET_VALUE		10
#define INTERACTIVE_TEXT_VALUE    	11
#define LAUNCH_FILE_PATH            12		
#define LAUNCH_FILE_PATH_DRIVER     13
#define SAVE_DRIVER_OPTION					14	
#define SAVE_AND_EXIT_BOOTPAGE			15
#define DISCARD_AND_EXIT_FRONTPAGE		16
#define LAUNCH_SHELL_FROM_FRONTPAGE		17	
#define PASSWORD_CLEAR              18
#define PASSWORD_INVALID            19
#define HELP_MESSAGEBOX             20
#define NO_VALID_FILES                      21
#define POST_MANAGER                    22
#define ADD_BOOT_DRIVER_INVALID                    23
#define SAVE_AND_EXIT_BBSPAGE			24	
#define	ADD_FORM_ID				0x2000
#define	DEL_FORM_ID				0x2001
#define	ADD_DRIVER_FORM_ID		0x2002		
#define	DEL_DRIVER_FORM_ID		0x2003

#define	HARD_DISK_ORDER_FORM	0x1000
#define	CDROM_ORDER_FORM		0x1001
#define	FLOPPY_DISK_ORDER_FORM	0x1002
#define	NETWORK_ORDER_FORM		0x1003
#define	BEV_ORDER_FORM			0x1004

typedef struct
{
	UINT16 HardDiskCount;
	UINT16 CDROMCount;
	UINT16 FloppyCount;
	UINT16 NetworkCount;
	UINT16 BevCount;
}
LEGACY_DEV_LIST;

#define	SETUP_RESET_NOT_REQUIRED			0
#define	SETUP_SAVED_AND_RESET_REQUIRED		1

#pragma pack(1)
typedef struct
{
	UINT16 SetupSaveState;
}
SETUP_SAVE_STATE_INFO;


typedef struct
{
	UINT16 GroupCount;
}
LEGACY_DEV_INFO;

typedef struct
{
	UINT16 DeviceCount;
}
LEGACY_GROUP_INFO;

typedef struct _SYSTEM_ACCESS
{
	UINT8	Access;
}
SYSTEM_ACCESS;

typedef struct
{
	UINT16 BootCount;
}
BOOT_MANAGER;

typedef struct{
  UINT16            BootCount;
} BOOT_NOW_COUNT;

typedef struct
{
	UINT16 Option;
}
DEL_BOOT_OPTION;

typedef struct 
{
	UINT16	Value;
}
AMI_CALLBACK;

typedef struct 
{
	UINT16	Value;
}
TIMEOUT;

typedef struct{
  UINT16            Value;
} BOOT_ORDER;

typedef struct{
  UINT16            Value;
} LEGACY_DEV_ORDER;

typedef struct 
{
	UINT16	Value;
}
LANGUAGE;

typedef struct 
{
	UINT16	Value;
}
LANGUAGE_CODES;

typedef struct 
{
	UINT16	Value;
}
EFI_SHELL;

typedef struct{
  UINT16            PageCount;
} DYNAMIC_PAGE_COUNT;

typedef struct{
  UINT16        GroupCount;
} DYNAMIC_PAGE_GROUP_COUNT;

typedef struct{
  UINT16        DeviceCount;
} DYNAMIC_PAGE_DEVICE_COUNT;

typedef struct{
UINT16 NonDeviceClassCount;
UINT16 DiscDeviceClassCount;
UINT16 VideoDeviceClassCount;
UINT16 NetworkDeviceClassCount;
UINT16 InputDeviceClassCount;
UINT16 OnBoardDeviceClassCount;
UINT16 OtherDeviceClassCount;
} DYNAMIC_PAGE_GROUP_CLASS_COUNT;

typedef struct
{
	UINT16 Value;

}OEMTSEVAR;

typedef struct
{
	UINT16 Enable;
}
DRIVER_HEALTH_ENABLE;

typedef struct
{
	UINT16 DrvHandleCount;
}
DRIVER_HEALTH;

typedef struct			//Number of controllers support driver health handle
{
	UINT16 DrvHealthCtrlCnt;
}
DRIVER_HEALTH_CTRL_COUNT;

typedef struct
{
	UINT16 DriverCount;
} DRIVER_MANAGER;

typedef struct{
  UINT16            Value;
} DRIVER_ORDER;

typedef struct
{
	UINT16 Option;
}DEL_DRIVER_OPTION;

#define	ERROR_MANAGER_NOP			0x0000
#define	ERROR_MANAGER_ALL_ERRORS	0xFFFF

#define OEM_TSE_VARSTORE\
    varstore OEMTSEVAR, key = OEM_TSE_VAR_KEY_ID,\
        name  = OemTSEVar, guid  = OEM_TSE_VAR_GUID;


#ifndef VFRCOMPILE
typedef struct _ERROR_LIST
{
	VOID*	Handle;
	UINT16	Summary;
	UINT16	DetailInfo;
}
ERROR_LIST ;
#endif

typedef struct _ERROR_MANAGER
{
	UINT16		ErrorCount;
	UINT16		ErrorFunction;
#ifndef VFRCOMPILE
	ERROR_LIST	ErrorList[1];
#endif
}
ERROR_MANAGER;

typedef struct _NEW_BOOT_OPTION
{
    CHAR16	Label[20];
    UINT16  FsCount;
    UINT16  SelFs;
    CHAR16	Path[120];
}
NEW_BOOT_OPTION;

typedef struct _NEW_DRIVER_OPTION
{
    CHAR16	Label[20];
    UINT16  FsCount;
    UINT16  SelFs;
    CHAR16	DriverPath[120];
}
NEW_DRIVER_OPTION;

typedef struct
{
	UINT16	Count;
}
USER_DEFAULTS;

typedef struct _IDE_SECURITY_CONFIG {
	UINT16		Count;
    UINT16      Supported;
    UINT16      Enabled;
	UINT16		Locked;
    UINT16      Frozen; 
    UINT16      UserPasswordStatus; 
    UINT16      MasterPasswordStatus; 
    UINT16      ShowMaster;   
	UINT16      IDEUserPassword[32];
    UINT16      IDEMasterPassword[32];
}IDE_SECURITY_CONFIG;

typedef struct _AMITSESETUP
{
    // Passwords
    UINT16  UserPassword[SETUP_PASSWORD_LENGTH];
    UINT16  AdminPassword[SETUP_PASSWORD_LENGTH];
#if SMCPKG_SUPPORT
    UINT8   CheckPassword;
#endif
	// AMI SilentBoot
    UINT8   AMISilentBoot;
}
AMITSESETUP;
extern EFI_GUID gAmiTseSetupEnterGuid ;
extern EFI_GUID gAmiTseAfterFirstBootOptionGuid ;
extern EFI_GUID gAmiTseEventBeforeBootGuid;
extern EFI_GUID gAmiTseBootOrderChangeGuid ;
extern EFI_GUID gAmiTseNVRAMUpdateGuid ;
extern  EFI_GUID gAmiTsePasswordPromptEnterGuid ;
extern  EFI_GUID gAmiTsePasswordPromptExitGuid;
extern EFI_GUID gAmiTseUserPasswordValidGuid ;
extern  EFI_GUID gAmiTseAdminPasswordValidGuid ;
extern  EFI_GUID gAmiTseInvalidPasswordGuid ;
extern  EFI_GUID gAmiTseBeforeTimeOutGuid;
extern EFI_GUID gAmiTseAfterTimeOutGuid ;
extern EFI_GUID gAmiTseOemTSEVarGuid ;
extern EFI_GUID gAmiTseOemPortingVar1Guid;
extern EFI_GUID gAmiTseOemPortingVar2Guid;
extern EFI_GUID gAmiTseOemPortingVar3Guid ;
extern EFI_GUID gAmiTseOemPortingVar4Guid ;
extern EFI_GUID gAmiTseOemPortingVar5Guid ;
extern EFI_GUID gAmiTseOemPortingVar6Guid ;
extern EFI_GUID gAmiTseOemPortingVar7Guid ;
extern EFI_GUID gAmiTseOemPortingVar8Guid ;
extern EFI_GUID gAmiTseOemPortingVar9Guid ;
extern EFI_GUID gAmiTseOemPortingVar10Guid ; 
extern EFI_GUID gAmiTseOemPortingVar11Guid ;
extern EFI_GUID gAmiTseOemPortingVar12Guid ;
extern EFI_GUID gAmiTseOemPortingVar13Guid ;
extern EFI_GUID gAmiTseOemPortingVar14Guid ;
extern EFI_GUID gAmiTseOemPortingVar15Guid ;
extern EFI_GUID gAmiTseOemPortingVar16Guid ;
extern EFI_GUID gAmiTseOemPortingVar17Guid ;
extern EFI_GUID gAmiTseOemPortingVar18Guid ;
extern EFI_GUID gAmiTseOemPortingVar19Guid ;
extern EFI_GUID gAmiTseOemPortingVar20Guid ;
extern EFI_GUID gAmiTseOemPortingVar21Guid ;
extern EFI_GUID gAmiTseOemPortingVar22Guid ;
extern EFI_GUID gAmiTseOemPortingVar23Guid ;
extern EFI_GUID gAmiTseOemPortingVar24Guid ;
extern EFI_GUID gAmiTseOemPortingVar25Guid ;
extern EFI_GUID gAmiTseOemPortingVar26Guid ;
extern EFI_GUID gAmiTseOemPortingVar27Guid ;
extern EFI_GUID gAmiTseOemPortingVar28Guid ;
extern EFI_GUID gAmiTseOemPortingVar29Guid ;
extern EFI_GUID gAmiTseOemPortingGuid1 ;
extern EFI_GUID gAmiTseOemPortingGuid2 ;
extern EFI_GUID gTSEInvalidateBgrtStatusProtocolGuid ;
extern EFI_GUID  gTSESwitchingToPostScreenGuid ;
extern EFI_GUID gTSEAfterPostInvPwdPromptGuid;
extern EFI_GUID gSetupCompleteRedrawGuid; 
extern EFI_GUID gEvaluateDefaults4FirstBootGuid;
extern EFI_GUID gPlatformRecoveryOrderGuid;

#pragma pack()

#endif /* _AMI_VFR_H_ */
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
