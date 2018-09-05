//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Add riser card name to IIO and oprom control items
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jun/21/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/18/2014
//
//****************************************************************************
//****************************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		SuperMDxeDriver.h
//
// Description:
//  This file include SuperM Protocal information and  data structure
//
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>


#ifndef _SUPERMDXEDRIVER_H_
#define _SUPERMDXEDRIVER_H_

#define MAX_SOCKET 4 
#define NUMBER_PORTS_PER_SOCKET       21
#if MAX_SOCKET > 4
#define TOTAL_CB3_DEVICES             64   // Todo Check SKX CB3 devices (IOAT_TOTAL_FUNCS * MAX_SOCKET). Note: this covers up to 8S.
#define MAX_TOTAL_PORTS               84   //NUMBER_PORTS_PER_SOCKET * MaxIIO. As now, treats setup S0-S3 = S4_S7 as optimal
#else
#define TOTAL_CB3_DEVICES             32   // Todo Check SKX CB3 devices.
#define MAX_TOTAL_PORTS               84   //NUMBER_PORTS_PER_SOCKET * MaxIIO
#endif
#if MAX_SOCKET > 4
#define MAX_NTB_PORTS                 24   // NUMBER_NTB_PORTS_PER_SOCKET * MAX_SOCKET
#else
#define MAX_NTB_PORTS                 12   // NUMBER_NTB_PORTS_PER_SOCKET * MAX_SOCKET
#endif
#if MAX_SOCKET > 4
#define MAX_VMD_PORTS                 96   // VMD_PORTS_PER_SOCKET * MAX_SOCKET
#define MAX_VMD_STACKS                24   // VMD_STACK_PER_SOCKET * MAX_SOCKET
#else
#define MAX_VMD_PORTS                 48   // VMD_PORTS_PER_SOCKET * MAX_SOCKET
#define MAX_VMD_STACKS                12   // VMD_STACK_PER_SOCKET * MAX_SOCKET
#endif
#define NUM_DEVHIDE_REGS                              8 
// Hide all 8 Devices for every Stack 
#define MAX_DEVHIDE_REGS                             (MAX_IIO_STACK * NUM_DEVHIDE_REGS)
#if MaxIIO > 4
#define MAX_DEVHIDE_REGS_PER_SYSTEM                  384 //(MAX_DEVHIDE_REGS * MaxIIO)
#else
#define MAX_DEVHIDE_REGS_PER_SYSTEM                  192 //(MAX_DEVHIDE_REGS * MaxIIO)
#endif
//----------------------------------------------------------------------------
// Include File(s)
//----------------------------------------------------------------------------
#include "token.h"
#include "SuperMCommonDriver.h"
#include <../CpRcPkg/Library/BaseMemoryCoreLib/Chip/Skx/Include/Iio/IioRegs.h>
#include <../CpRcPkg/Library/BaseMemoryCoreLib/Chip/Skx/Include/Iio/IioConfig.h>
#include <UefiHii.h>

#define EFI_SUPERM_DRIVER_PROTOCOL_GUID \
  { \
     0x2272865b, 0xebf6, 0x4047, 0xb0, 0x8, 0x78, 0x89, 0x49, 0x7f, 0x53, 0xbd \
  }

#define EFI_PROJECT_OWN_PROTOCOL_GUID \
  { \
     0x6b588d7c, 0xc72e, 0x4a78, 0x88, 0x58, 0x77, 0xe1, 0xda, 0x8f, 0xc7, 0xf7 \
  }

typedef VOID (EFIAPI *InitStringSmc)(EFI_HII_HANDLE HiiHandle, STRING_REF StrRef, CHAR16 *sFormat, ...);
typedef VOID (EFIAPI *HiiSetStringSmc)(EFI_HII_HANDLE HiiHandle, EFI_STRING_ID StringId, CONST EFI_STRING Stringt, CONST CHAR8 *SupportedLanguages);

typedef EFI_STATUS	(EFIAPI *GETSUPERMBOARDINFO)(OUT SUPERMBOARDINFO *SuperMBoardInfoData);
typedef EFI_STATUS 	(EFIAPI *GETSUPERMPCIEBIFURCATE)(IN OUT IIO_CONFIG *SetupData);
//typedef EFI_STATUS 	(EFIAPI *SETSUPERMPCIECTLE)(IN OUT IIO_GLOBALS *IioGlobalData,IN UINT8 Iio,IN UINT8 BusNumber);
typedef EFI_STATUS 	(EFIAPI *SETSUPERMSETUPSTRING)(IN EFI_HII_HANDLE HiiHandle, IN UINT16 Class, IN InitStringSmc InitString);
typedef EFI_STATUS 	(EFIAPI *SETINTELRCSETUPSTRING)(IN EFI_HII_HANDLE HiiHandle, IN HiiSetStringSmc	HiiSetString, IN EFI_STRING_ID *StringId);

//<AMI_THDR_START>
//--------------------------------------------------------------------------------------------
// Name: SUPERMDRIVER_PROTOCOL
//
// Description:
//
// Fields: Name                             Description
//--------------------------------------------------------------------------------------------
// GETSUPERMBOARDINFO	                    Get board information
// GETSUPERMHIDDENITEM                      Get board hidden setup item list
// GETSUPERMPCIEBIFURCATE                   Get IIO PCIE bifuracate setting
//--------------------------------------------------------------------------------------------
//<AMI_THDR_END>
typedef struct _SUPERMDRIVER_PROTOCOL {
	GETSUPERMBOARDINFO	GetSuperMBoardInfo;
	GETSUPERMPCIEBIFURCATE	GetSuperMPciEBifuracate;
//	SETSUPERMPCIECTLE   	SetSuperMPciECTLE;
	SETSUPERMSETUPSTRING	SetSuperMSetupString;
	SETINTELRCSETUPSTRING	SetIntelRCSetupString;
} SUPERMDRIVER_PROTOCOL;

//<AMI_THDR_START>
//--------------------------------------------------------------------------------------------
// Name: PROJECTDRIVER_PROTOCOL
//
// Description:
//
// Fields: Name                             Description
//--------------------------------------------------------------------------------------------
// PROJECTCHECKADJUSTVID                    Check CPU/Memory VID adjust feature
// PROJECTGETGPUDID                         Get GPU information
// PROJECTCHECKBID                          Get Board id
// PROJECTRESERVE                           Reserve feature
// PROJECTRESERVE                           Reserve feature
// PROJECTBMCPRESENT                        Check BMC present or not
// PROJECTAUDIOHANDLE                       Audio handle for each board
// PROJECTHIDESETUP                         Get setup item hidden list
// PROJECTRESERVE                           Reserve feature
// PROJECTRESERVE                           Reserve feature
// PROJECTRESERVE                           Reserve feature
//--------------------------------------------------------------------------------------------
//<AMI_THDR_END>
typedef UINT8		(EFIAPI *PROJECTCHECKADJUSTVID)(VOID);
typedef UINT16		(EFIAPI *PROJECTGETGPUDID)(IN UINT8 SlotNum);
typedef UINT8		(EFIAPI *PROJECTCHECKBID)(VOID);
typedef UINT8 		(EFIAPI *PROJECTBMCPRESENT)(VOID);
typedef EFI_STATUS 	(EFIAPI *PROJECTHWARDWAREINFO)(OUT UINT8 *HardWareInfoData, OUT UINT8 *LanMacInfoData);
typedef EFI_STATUS 	(EFIAPI *PROJECTSYSTEMCONFIG)(OUT UINT8 *SystemConfiguration);
typedef EFI_STATUS 	(EFIAPI *PROJECTAUDIOHANDLE)(VOID);
typedef EFI_STATUS 	(EFIAPI *PROJECTPCIEBIFURACATE)(IN OUT IIO_CONFIG *SetupData);
//typedef EFI_STATUS 	(EFIAPI *PROJECTPCIECTLE)(IN OUT IIO_GLOBALS *IioGlobalData, IN UINT8 Iio, IN UINT8 BusNumber);
typedef EFI_STATUS 	(EFIAPI *PROJECTIIOSTRING)(IN EFI_HII_HANDLE HiiHandle, IN HiiSetStringSmc HiiSetString, IN EFI_STRING_ID *StringId);
typedef EFI_STATUS 	(EFIAPI *PROJECTRESERVE)(VOID);
// Project Own Driver

typedef struct _PROJECTDRIVER_PROTOCOL {
	PROJECTCHECKADJUSTVID	CheckAdjustVID;
	PROJECTGETGPUDID	GetGPUDID;
	PROJECTCHECKBID		GetProjectBID;
	PROJECTRESERVE		ProjectReserveFeature;
	PROJECTHWARDWAREINFO	GetProjectHardwareInfo;
	PROJECTBMCPRESENT	CheckBMCPresent;
	PROJECTAUDIOHANDLE	CheckAudioHandle;
	PROJECTPCIEBIFURACATE	GetProjectPciEBifurcate;
//	PROJECTPCIECTLE		SetProjectPciECTLE;
	PROJECTIIOSTRING	SetProjectIioString;
	PROJECTRESERVE		ProjectReserveFeature2;
	PROJECTRESERVE		ProjectReserveFeature3;
} PROJECTDRIVER_PROTOCOL;

#endif // _SUPERMDXEDRIVER_H_

//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2012 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************

