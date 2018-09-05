//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
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
// $Header: /AptioV/SRC/SerialDebugger/Include/Library/AMIPeiGUIDs.h 2     1/23/14 5:44p Sudhirv $
//
// $Revision: 2 $
//
// $Date: 1/23/14 5:44p $
//*****************************************************************
//*****************************************************************
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/SerialDebugger/Include/Library/AMIPeiGUIDs.h $
// 
// 2     1/23/14 5:44p Sudhirv
// Updated Copyright Info
// 
// 1     1/23/14 4:37p Sudhirv
// EIP I51926 - Serial Debugger - make Serial Debugger independent of
// AMIDebugRx and Debuger
// 
// 1     11/02/12 10:07a Sudhirv
// AMIDebugRxPkg modulepart for AMI Debug for UEFI 3.0
// 
// 2     8/03/12 6:32p Sudhirv
// Updated before making binaries.
//
// 1     7/15/12 8:12p Sudhirv
// AMIDebugRx Module for Aptio 5
//
//
//*****************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          AmiPeiGUID.C
//
// Description:   Contains the GUIDs that are refered in DEBUGGER
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#define PEI_DBGSUPPORT_DBGR_GUID  \
  {0x4AAAAE15, 0x5AEB, 0x4c11, 0xB9, 0x1D, 0xA3, 0x96, 0x6A, 0xC0, 0x48, 0x47}
  // {4AAAAE15-5AEB-4c11-B91D-A3966AC04847}

#define PEI_DBGSUPPORT_XPORT_GUID  \
  {0xC7E8BB67, 0x1C3F, 0x41ba, 0x82, 0x0F, 0x3D, 0x0E, 0x9C, 0x36, 0x50, 0x42}
  // {C7E8BB67-1C3F-41ba-820F-3D0E9C365042}

#define PEI_DBGSUPPORT_DATA_GUID  \
	{0x41cac730, 0xe64e, 0x463b, 0x89, 0x72, 0x25, 0x5e, 0xec, 0x55, 0x55, 0xc2}
// {41CAC730-E64E-463b-8972-255EEC5555C2}

#define PEI_DBGR_REPORTSTATUSCODE_GUID	\
   {0x9D67E0E3, 0x949D, 0x4d68, 0xBA, 0x82, 0xD0, 0x80, 0xD9, 0x34, 0xAE, 0xF5}
// {9D67E0E3-949D-4d68-BA82-D080D934AEF5}

#define PEI_DBGSUPPORT_DRIVER_GUID  \
	{0xA47438D5, 0x94E9, 0x49b3, 0xBC, 0x31, 0x7e, 0x6b, 0xc9, 0x36, 0x38, 0x14}
// {A47438D5-94E9-49b3-BC31-7E6BC9363814}

#define DXE_DBG_DATA_GUID  \
	{0x6c8ed77f, 0x6ec3, 0x4c61, 0x96, 0x4e, 0x6, 0xdb, 0x5d, 0x96, 0xc7, 0x7c}
// {6C8ED77F-6EC3-4c61-964E-06DB5D96C77C}

#define AMI_DBG_REPORTSTATUS_CAR_PEI_XPORT		1
#define AMI_DBG_REPORTSTATUS_X32_PEI_XPORT		2
#define AMI_DBG_REPORTSTATUS_X64_PEI_XPORT		3
#define AMI_DBG_REPORTSTATUS_DXE_XPORT			4

typedef struct _PEI_DBGR_REPORTSTATUSCODE{
	EFI_HOB_GUID_TYPE	GuidHob;
	UINT64				ReportStatusCode;
	UINT8				State;
}PEI_DBGR_REPORTSTATUSCODE;

// {511D0266-F2E0-4df8-AE3A-FDB98523BFB9}
#define PEI_DBG_XPORTX64_GUID \
	{ 0x511d0266, 0xf2e0, 0x4df8, 0xae, 0x3a, 0xfd, 0xb9, 0x85, 0x23, 0xbf, 0xb9 }

// {C253ED0A-C48B-4ee3-A65E-75F61F3AD251}
#define PEI_DBG_DBGRX64_GUID \
	{ 0xc253ed0a, 0xc48b, 0x4ee3, 0xa6, 0x5e, 0x75, 0xf6, 0x1f, 0x3a, 0xd2, 0x51 }

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
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