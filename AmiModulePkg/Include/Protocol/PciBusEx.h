//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2005, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/BIN/Core/Modules/PciBus/PciBusEx.h 2     11/14/08 1:36p Yakovlevs $
//
// $Revision: 2 $
//
// $Date: 11/14/08 1:36p $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:	PciBusEx.h
//
// Description:	Defines PciBus Extended protocol Interface to provide
//              additional PCI Bus related functions.
//
//<AMI_FHDR_END>
//**********************************************************************
#ifndef __PCI_BUS_EX__H__
#define __PCI_BUS_EX__H__
#ifdef __cplusplus
extern "C" {
#endif


#include <Efi.h>
#include <PciBus.h>
#include <Protocol/PciIo.h>

#define AMI_PCI_BUS_EXT_PROTOCOL_GUID \
{ 0xf42a009d, 0x977f, 0x4f08, 0x94, 0x40, 0xbc, 0xa5, 0xa3, 0xbe, 0xd9, 0xaf };

//-------------------------------------------------
//Forward Declarations
//-------------------------------------------------
typedef struct _AMI_PCI_EXT_PROTOCOL AMI_PCI_EXT_PROTOCOL;


//-------------------------------------------------
// Protocol Function Definitions
//-------------------------------------------------
typedef EFI_STATUS (EFIAPI * AMI_PCI_EXT_IS_PCI_EXPRESS)(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		    OPTIONAL,
    OUT PCIE_DATA                                       **PciExpData    OPTIONAL
);

//-------------------------------------------------
typedef EFI_STATUS (EFIAPI * AMI_PCI_EXT_IS_PCI_X)(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		    OPTIONAL,
    OUT PCIX_DATA                                       **PciXData    OPTIONAL
);

//-------------------------------------------------
typedef EFI_STATUS (EFIAPI * AMI_PCI_EXT_IS_P2P_BRG)(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		    OPTIONAL,
    OUT PCI_BRG_EXT                                     **BrgData       OPTIONAL
);

//-------------------------------------------------
typedef EFI_STATUS (EFIAPI * AMI_PCI_EXT_IS_CRD_BRG)(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		    OPTIONAL,
    OUT PCI_BRG_EXT                                     **BrgData       OPTIONAL
);

//-------------------------------------------------
typedef EFI_STATUS (EFIAPI * AMI_PCI_EXT_IS_REG_DEVICE)(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		OPTIONAL
);

typedef EFI_STATUS (EFIAPI * AMI_PCI_EXT_GET_CLASS_CODES_INFO) (
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		OPTIONAL,
	OUT PCI_DEV_CLASS									*CassCodes
);

typedef EFI_STATUS (EFIAPI * AMI_PCI_EXT_GET_PCI_PIC_IRQ) (
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		OPTIONAL,
    OUT PCI_IRQ_PIC_ROUTE                               **PicIrqTblEntry,
    EFI_PCI_CONFIGURATION_ADDRESS                       **ParentDevices,
    OUT UINTN                                           *EntryCount
);

typedef EFI_STATUS (EFIAPI * AMI_PCI_EXT_GET_PCI_APIC_IRQ) (
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		OPTIONAL,
    OUT PCI_IRQ_APIC_ROUTE                              **ApicIrqTblEntry,
    EFI_PCI_CONFIGURATION_ADDRESS                       **ParentDevices,
    OUT UINTN                                           *EntryCount
);



typedef struct _AMI_PCI_EXT_PROTOCOL {
	AMI_PCI_EXT_IS_PCI_EXPRESS			IsPciExpress;
	AMI_PCI_EXT_IS_PCI_X				IsPciX;
	AMI_PCI_EXT_IS_P2P_BRG				IsPci2PciBridge;
	AMI_PCI_EXT_IS_CRD_BRG				IsPci2CrdBridge;
	AMI_PCI_EXT_IS_REG_DEVICE			IsPciDevice;
	AMI_PCI_EXT_GET_CLASS_CODES_INFO	GetClassCodesInfo;
    AMI_PCI_EXT_GET_PCI_PIC_IRQ         GetPciPicIrq;
    AMI_PCI_EXT_GET_PCI_APIC_IRQ        GetPciApicIrq;
    EFI_HANDLE                          PciExtHanle;
} AMI_PCI_EXT_PROTOCOL;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2005, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************