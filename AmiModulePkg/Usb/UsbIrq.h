//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Pkwy, Norcross, GA 30093                 **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

/** @file UsbIrq.h

**/

#ifndef _AMIUSB_IRQ_H
#define _AMIUSB_IRQ_H

#define AMI_USB_ISR_PROTOCOL_GUID  \
    {0xE39B0498, 0x59A5, 0x427A, 0xA0, 0xAA, 0x18, 0x80, 0x3F, 0x46, 0x6A, 0x60}

typedef struct _AMI_USB_ISR_PROTOCOL AMI_USB_ISR_PROTOCOL;

typedef EFI_STATUS (EFIAPI *AMI_INSTALL_USB_ISR) (
	IN AMI_USB_ISR_PROTOCOL	*This
);

typedef EFI_STATUS (EFIAPI *AMI_INSTALL_USB_LEGACY_ISR) (
	IN AMI_USB_ISR_PROTOCOL	*This
);

typedef struct _AMI_USB_ISR_PROTOCOL {
	AMI_INSTALL_USB_ISR             InstallUsbIsr;
	AMI_INSTALL_USB_LEGACY_ISR      InstallLegacyIsr;
};

#endif

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Pkwy, Norcross, GA 30093                 **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
