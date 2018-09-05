//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.03
//      Bug Fixed:  Fixed override KTI function work abnormally issue.
//      Reason:     
//      Auditor:    Leon Xu
//      Date:       Jan/17/2017
//
//  Rev. 1.02
//      Bug Fixed:  Add PCIE bifurcation setting in PEI.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/06/2016
//
//  Rev. 1.01
//      Bug Fixed:  Add board level override KTI setting
//      Reason:     
//      Auditor:    Salmon Chen
//      Date:       Mar/14/2016
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
// Name:		SuperMPeiDriver.h
//
// Description:
//  This file include SuperM PEI PPI information and  data structure
//
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

//----------------------------------------------------------------------------
// Include File(s)
//----------------------------------------------------------------------------
#ifndef _SUPERMPEIDRIVER_H_
#define _SUPERMPEIDRIVER_H_

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
//#include <KtiHost.h>
#include "token.h"
//#include <Efi.h>

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include "SuperMCommonDriver.h"
#include <../CpRcPkg/Library/BaseMemoryCoreLib/Chip/Skx/Include/Iio/IioRegs.h>
#include <../CpRcPkg/Library/BaseMemoryCoreLib/Chip/Skx/Include/Iio/IioConfig.h>

#define EFI_SUPERM_PEI_DRIVER_PROTOCOL_GUID \
  { \
     0x7ffc3700, 0x7c5e, 0x423f, 0xa0, 0x4b, 0x32, 0xe9, 0xf1, 0x6a, 0x67, 0x27 \
  }

#define EFI_PROJECT_OWN_PEI_DRIVER_PROTOCOL_GUID \
  { \
     0xa0e5a45f, 0x5374, 0x46b3, 0xb5, 0x1e, 0xd3, 0x10, 0xb2, 0xc4, 0x8a, 0xae \
  }

#ifndef __SMC_KTI_OVERRIDE__
#pragma pack(1)
typedef struct {
  UINT8  SocketID;
  UINT8  Freq;
  UINT32 Link;
  UINT32 AllLanesTXEQ;
  UINT8  CTLEPEAK;
} ALL_LANES_EPARAM_LINK_INFO;
#pragma pack()
#endif

//#define PCI_DEVICE_NUMBER_PCH_THERMAL   31
//#define PCI_FUNCTION_NUMBER_PCH_THERMAL 6
//#define R_PCH_THERMAL_SVID              0x2C

//<AMI_THDR_START>
//--------------------------------------------------------------------------------------------
// Name: SUPERM_PEI_DRIVER_PROTOCOL
//
// Description:
//
// Fields: Name                             Description
//--------------------------------------------------------------------------------------------
// GETSUPERMBOARDINFO	                    Get board information
//--------------------------------------------------------------------------------------------
//<AMI_THDR_END>

typedef struct _SUPERM_PEI_DRIVER_PROTOCOL SUPERM_PEI_DRIVER_PROTOCOL;

typedef EFI_STATUS 	(EFIAPI *GETSUPERMBOARDINFO)(
				IN CONST EFI_PEI_SERVICES **PeiServices,
				IN SUPERM_PEI_DRIVER_PROTOCOL *This,
				OUT SUPERMBOARDINFO *SuperMBoardInfoData);

//-----------------12/23/2014 11:50AM---------------
//  Coding for every platform
// --------------------------------------------------

typedef UINT32 	(EFIAPI *GETSUPERMBOARDEPARAM)(
                IN EFI_PEI_SERVICES **PeiServices,
                IN SUPERM_PEI_DRIVER_PROTOCOL *This,
                OUT UINT32 *SizeOfTable,
                OUT ALL_LANES_EPARAM_LINK_INFO **Ptr);


typedef EFI_STATUS 	(EFIAPI *GETSUPERMPCHDATA)(
				IN CONST EFI_PEI_SERVICES **PeiServices,
				IN SUPERM_PEI_DRIVER_PROTOCOL *This,
				IN OUT UINT8 PLATFORM_PCH_DATA[10]);

typedef EFI_STATUS 	(EFIAPI *GETSUPERMBIFURCATION)(
				IN CONST EFI_PEI_SERVICES **PeiServices,
				IN OUT	IIO_CONFIG	*SetupData);

typedef struct _SUPERM_PEI_DRIVER_PROTOCOL {
	GETSUPERMBOARDINFO GetSuperMBoardInfo;
	GETSUPERMBOARDEPARAM GetSuperMEParam;
	GETSUPERMPCHDATA GetSuperMPchInfo;
	GETSUPERMBIFURCATION GetSuperMPcieBifurcation;
} SUPERM_PEI_DRIVER_PROTOCOL;

//<AMI_THDR_START>
//--------------------------------------------------------------------------------------------
// Name: PROJECT_PEI_DRIVER_PROTOCOL
//
// Description:
//
// Fields: Name                             Description
//--------------------------------------------------------------------------------------------
// PROJECTCHECKADJUSTVID                    Check CPU/Memory VID adjust feature
// PROJECTCHECKBID		                    Get Board id
// PROJECTSYSTEMCONFIG	                    Get system configuration
// PROJECTHWARDWAREINFO	                    Get H/W information
// PROJECTBMCPRESENT	                    Check BMC present or not
// PROJECTRESERVE1		                    Reserve feature
// PROJECTRESERVE2		                    Reserve feature
// PROJECTRESERVE3		                    Reserve feature
//--------------------------------------------------------------------------------------------
//<AMI_THDR_END>


typedef struct _PROJECT_PEI_DRIVER_PROTOCOL PROJECT_PEI_DRIVER_PROTOCOL;


typedef UINT8		(EFIAPI *PROJECTCHECKADJUSTVID)(
				IN EFI_PEI_SERVICES **PeiServices,
				IN PROJECT_PEI_DRIVER_PROTOCOL *This);

typedef UINT8		(EFIAPI *PROJECTCHECKBID)(
				IN EFI_PEI_SERVICES **PeiServices,
				IN PROJECT_PEI_DRIVER_PROTOCOL *This);

typedef UINT8 		(EFIAPI *PROJECTBMCPRESENT)(
				IN EFI_PEI_SERVICES **PeiServices,
				IN PROJECT_PEI_DRIVER_PROTOCOL *This);

typedef EFI_STATUS 	(EFIAPI *PROJECTHWARDWAREINFO)(
				IN EFI_PEI_SERVICES **PeiServices,
				IN PROJECT_PEI_DRIVER_PROTOCOL *This,
				OUT UINT8 *HardWareInfoData,
				OUT UINT8 *LanMacInfoData);

typedef EFI_STATUS 	(EFIAPI *PROJECTSYSTEMCONFIG)(
				IN EFI_PEI_SERVICES **PeiServices,
				IN PROJECT_PEI_DRIVER_PROTOCOL *This,
				OUT UINT8 *SystemConfiguration);

//-----------------12/23/2014 11:50AM---------------
//  Coding for every platform
// --------------------------------------------------
typedef UINT32	(EFIAPI *PROJECTGETEPARAM)(
		IN EFI_PEI_SERVICES **PeiServices,
		IN PROJECT_PEI_DRIVER_PROTOCOL *This,
                OUT UINT32 *SizeOfTable,
                OUT ALL_LANES_EPARAM_LINK_INFO **Ptr);

typedef EFI_STATUS	(EFIAPI *PROJECTPCHDATA)(
		IN EFI_PEI_SERVICES **PeiServices,
		IN PROJECT_PEI_DRIVER_PROTOCOL *This,
                IN OUT UINT8 PLATFORM_PCH_DATA[10]);

typedef EFI_STATUS	(EFIAPI *PROJECTPCIEBIFURACATE)(
		IN EFI_PEI_SERVICES **PeiServices,
		IN OUT IIO_CONFIG *SetupData);

typedef EFI_STATUS	(EFIAPI *PROJECTRESERVE2)(
		IN EFI_PEI_SERVICES **PeiServices,
		IN PROJECT_PEI_DRIVER_PROTOCOL *This);

typedef EFI_STATUS	(EFIAPI *PROJECTRESERVE3)(
		IN EFI_PEI_SERVICES **PeiServices,
		IN PROJECT_PEI_DRIVER_PROTOCOL *This);

// Project Own Driver
typedef struct _PROJECT_PEI_DRIVER_PROTOCOL {
	PROJECTCHECKADJUSTVID	CheckAdjustVID;
	PROJECTCHECKBID		GetProjectBID;
	PROJECTSYSTEMCONFIG	GetProjectSystemConfiguration;
	PROJECTHWARDWAREINFO	GetProjectHardwareInfo;
	PROJECTBMCPRESENT	CheckBMCPresent;
	PROJECTGETEPARAM	GetProjectEParam;
	PROJECTPCHDATA		GetProjectPchData;
	PROJECTPCIEBIFURACATE	GetProjectPciEBifurcate;
	PROJECTRESERVE2		ProjectReserveFeature2;
	PROJECTRESERVE3		ProjectReserveFeature3;
} PROJECT_PEI_DRIVER_PROTOCOL;

#endif
//****************************************************************************
