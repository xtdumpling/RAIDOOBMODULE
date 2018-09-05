//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug fixed:   Implement UEFI PXE index function.
//    Reason:
//    Auditor:     Jacker Yeh
//    Date:        Dec/21/2015
//
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************

//<AMI_FHDR_START>
//---------------------------------------------------------------------------
//
// Name:        CRBCspDxeLib.c
//
// Description:
//  This file contains code for CRB platform
//   initialization in the Library stage
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>

//#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol\LegacyBiosPlatform.h>
//#include <PCI.h>
#include <Protocol\PciIo.h>
//#include <Protocol\IioUds.h>
#include <Guid\SetupVariable.h>
//#include <Include\Register\PchRegsEva.h>
//#include <Include\Register\PchRegsSata.h>

#include "BootOptions.h"
#include "BootOptioneLinks.h"
#include "Protocol\IdeControllerInit.h"

//#include "Protocol\PciIo.h"


#include "Protocol\DiskInfo.h"
#include "Protocol\AmiAhciBus.h"

#include "Protocol\LegacyRegion2.h"
//AptioV server overrride start: Dynamic mmcfg base address change
//#ifdef DYNAMIC_MMCFG_BASE_FLAG 
//#include "PchAccess.h"
//#endif
//AptioV server overrride end: Dynamic mmcfg base address change
//#include <Library/S3BootScriptLib.h>

//#include <AmiCspLib.h>


// Controller info Structure
typedef struct _CONTROLLER_INFO_STRUC {
    UINT16      BusDevFun;      // Bus, Device and Function number for a controller
    UINTN       ControllerNo;   // Controler number for a controller
} CONTROLLER_INFO_STRUC;

CONTROLLER_INFO_STRUC ControllerInfo[30];  // need to use macro
UINTN           MaxControllerFound;

#define SATACONTROLLER0 0
#define SATACONTROLLER1 1
#define SSATACONTROLLER 2

#pragma pack(1)
typedef struct {
  UINT16  SubSystemVendorId;      ///< Default Subsystem Vendor ID of the PCH devices.
  UINT16  SubSystemId;            ///< Default Subsystem ID of the PCH devices.
} IIO_DEFAULT_SVID_SID;

typedef struct {
  UINT8 DeviceNumber;
  UINT8 FunctionNumber;
  UINT8 SvidRegOffset;
  IIO_DEFAULT_SVID_SID DefaultSvidSid;
} IIO_SVID_SID_INIT_ENTRY;
#pragma pack()

#if SMCPKG_SUPPORT
#if SMC_UEFI_PXE_NAME_STYLE == 2
typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  ///
  /// The MAC address for a network interface padded with 0s.
  ///
  EFI_MAC_ADDRESS                 MacAddress;
  ///
  /// Network interface type(i.e. 802.3, FDDI).
  ///
  UINT8                           IfType;
} AMITSE_MAC_ADDR_DEVICE_PATH;
#endif
#endif

//
// GUID Definition(s)
//

#if 1
UINTN SMCConstructUefiPXEBootNamebyBDF(BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize)
{
	EFI_STATUS Status;
	EFI_LOAD_FILE_PROTOCOL *pLoadFileProtocol = NULL;
	EFI_DEVICE_PATH_PROTOCOL *pDevicePath = NULL;
    
	if(IsLegacyBootOption(Option)) return 0;

	if(Option->DeviceHandle == INVALID_HANDLE) goto ProcessDone;

	Status = pBS->HandleProtocol(Option->DeviceHandle, &gEfiLoadFileProtocolGuid, (VOID**)&pLoadFileProtocol);
	if(EFI_ERROR(Status)) goto ProcessDone;

	Status = pBS->HandleProtocol(Option->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID**)&pDevicePath);
	if(EFI_ERROR(Status)) goto ProcessDone;

	//
	// To check it is IPV4 or IPV6 from device path data
	//
	while (!isEndNode (pDevicePath))
	{
		if (((pDevicePath->Type == MESSAGING_DEVICE_PATH) && (pDevicePath->SubType == MSG_IPv4_DP)) || 
			((pDevicePath->Type == MESSAGING_DEVICE_PATH) && (pDevicePath->SubType == MSG_IPv6_DP)) )
		{

		    EFI_HANDLE NewHandle;
		    EFI_PCI_IO_PROTOCOL *pPciIo;
		    EFI_DEVICE_PATH_PROTOCOL *mDevicePath;    
		    UINTN PciSegment = 0, PciBus = 0, PciDeviceNumber = 0, PciFunction = 0;
		    
            Status=pBS->HandleProtocol( Option->DeviceHandle, &gEfiDevicePathProtocolGuid, &mDevicePath);
            if(EFI_ERROR(Status)) goto ProcessDone;

            Status = pBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &mDevicePath, &NewHandle);
            if(EFI_ERROR(Status)) goto ProcessDone;

            Status=pBS->HandleProtocol( NewHandle, &gEfiPciIoProtocolGuid, &pPciIo);
            if(EFI_ERROR(Status)) goto ProcessDone;

            Status = pPciIo->GetLocation(pPciIo, &PciSegment, &PciBus, &PciDeviceNumber, &PciFunction);

        	return Swprintf(Name, L"UEFI: %02x%x%x ", PciBus, PciDeviceNumber, PciFunction); 

ProcessDone:

			break;
		}
		pDevicePath = NEXT_NODE (pDevicePath);
	}
	return Swprintf(Name, L"UEFI: "); 
}
#endif
