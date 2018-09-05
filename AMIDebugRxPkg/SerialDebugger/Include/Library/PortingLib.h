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
// $Header: /AptioV/SRC/SerialDebugger/Include/Library/PortingLib.h 2     1/23/14 5:44p Sudhirv $
//
// $Revision: 2 $
//
// $Date: 1/23/14 5:44p $
//
//*********************************************************************
// Revision History
// ----------------
// $Log: /AptioV/SRC/SerialDebugger/Include/Library/PortingLib.h $
// 
// 2     1/23/14 5:44p Sudhirv
// Updated Copyright Info
// 
// 1     1/23/14 4:37p Sudhirv
// EIP I51926 - Serial Debugger - make Serial Debugger independent of
// AMIDebugRx and Debuger
// 
// 2     9/21/13 4:26a Sudhirv
// Updated for AMI Debug for UEFI 3.01.0006 Label.
// 
// 1     5/15/13 2:54p Sudhirv
// Updated for x86 DXE support patch binary generation.
// 
// 1     11/02/12 10:06a Sudhirv
// USBRedirection eModule for AMI Debug for UEFI 3.0
// 
// 1     10/09/12 12:16p Sudhirv
// [TAG]     EIP99567
// [Category] New Feature
// [Severity] Normal
// [Symptom] UsbRedirectionPkg for Aptio V
// 
// 1     7/17/09 7:54p Madhans
// AmiDebug RX Source Module
// 
// 2     7/09/09 11:21a Sudhirv
// Updated Copyright Information
// 
// 1     6/12/09 12:02a Madhans
// AMIDebugRx Module in sources
// 
// 6     6/13/07 3:16p Madhans
// Copyright Year updated.
// 
// 5     1/22/07 11:36a Madhans
// Modification made for Binary Release 1.04.0003.
// 
// 2     1/05/07 1:05p Ashrafj
// Latest version 1.04.0001 Beta
// Known issues from previous 1.04.000 Beta release has been fixed.
// 
// 1     12/26/06 7:09p Ashrafj
// Support added to support the Aptio 4.5 x64 EFI BIOS (use latest
// Host.zip).
// PEI Debugger in FV_MAIN (use new PEIDEBUGFIXUP).
// Support added for StatusCode redirection without Debug feature enabled
// (not working if DXE only and x64 is enabled).
// Plus, some new SDL tokens defined to support the Aptio 4.5 build
// process in Debug or non-debug mode.
// All the components of Debugger eModule in source form.
//
//*********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          PortingLib.h
//
// Description:   Prototype declarations and Macro Definitions exposed by the PortingLib library
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef __PORTINGLIB__H__
#define __PORTINGLIB__H__

#include "AmiDxeLib.h"
#include <protocol\DebugPort.h>
#include "Protocol\PciIo.h"
#include "Protocol\DevicePath.h"
#include "protocol\BlockIo.h"
#include "protocol\pDiskInfo.h"
#include "protocol\DriverBinding.h"
#include "protocol\PIDEController.h"
#include "protocol\ComponentName.h"
#include "protocol\PDiskInfo.h"
#include "protocol\PIDEBus.h"
#include "protocol\LoadedImage.h"

#define EfiInitializeDriverLib InitAmiLib
#define EfiCompareMem MemCmp
#define EfiZeroMem(buffer,size)	MemSet(buffer,size,0)
#define EfiCopyMem MemCpy
#define EfiStrCpy Wcscpy
#define EfiStrLen Wcslen
#define EfiAsciiStrLen Strlen
#define EfiAsciiStrCpy Strcpy
#define gBS pBS
#define gRT pRS

#define EfiDuplicateDevicePath 				DPCopy
#define EfiAppendDevicePathNode 			DPAddNode
//#define SetDevicePathNodeLength(a,l) SET_NODE_LENGTH(a,l)
//#define SetDevicePathEndNode(a)  {                       \
//          (a)->Type = END_DEVICE_PATH;              \
//          (a)->SubType = END_ENTIRE_SUBTYPE; \
//          SET_NODE_LENGTH(a,sizeof(EFI_DEVICE_PATH_PROTOCOL));      \
//          }

//Routines
EFI_DEVICE_PATH_PROTOCOL *EfiDevicePathFromHandle (
	  IN EFI_HANDLE  Handle) ;

EFI_STATUS EfiLibInstallAllDriverProtocols (
			  IN EFI_HANDLE                         ImageHandle,
  			  IN EFI_SYSTEM_TABLE                   *SystemTable,
			  IN EFI_DRIVER_BINDING_PROTOCOL        *DriverBinding,
			  IN EFI_HANDLE                         DriverBindingHandle,
			  IN EFI_COMPONENT_NAME_PROTOCOL        *ComponentName,        
			  IN void 							    *DriverConfiguration,
			  IN void								*DriverDiagnostics
			  ) ;

VOID *EfiLibAllocateZeroPool (IN UINTN AllocationSize);
VOID *EfiLibAllocatePool (IN UINTN AllocationSize);


// Unicode String Support
EFI_STATUS EfiLibLookupUnicodeString (
  CHAR8                     *Language,
  CHAR8                     *SupportedLanguages,
  EFI_UNICODE_STRING_TABLE  *UnicodeStringTable,
  CHAR16                    **UnicodeString
  );

BOOLEAN EfiLibCompareLanguage (
  CHAR8  *Language1,
  CHAR8  *Language2
  );

EFI_EVENT
	EfiLibCreateProtocolNotifyEvent (
	  IN EFI_GUID             *ProtocolGuid,
	  IN EFI_TPL              NotifyTpl,
	  IN EFI_EVENT_NOTIFY     NotifyFunction,
	  IN VOID                 *NotifyContext,
	  OUT VOID                *Registration
	  );

#endif
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
