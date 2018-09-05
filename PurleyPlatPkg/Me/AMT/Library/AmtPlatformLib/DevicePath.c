/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2004 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file DevicePath.c

 BDS internal function define the default device path string, it can be
 replaced by platform device path.

**/
#if AMT_SUPPORT

#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/PrintLib.h>
// APTIOV_SERVER_OVERRIDE_RC_START
//#include <Csm/IntelRaidBiosThunk/IntelRaidBiosThunk.h>
// APTIOV_SERVER_OVERRIDE_RC_END
#include "Library/BdsLib.h"
// APTIOV_SERVER_OVERRIDE_RC_START
#include <Library/BaseLib.h>
#include <Library/DebugLib.h.>

#define UNKNOWN_DEVICE_GUID \
  { 0x01314f3e, 0x8bd7, 0x4fb3,  {0xa6, 0x4e, 0x67, 0x22, 0x71, 0xd6, 0xb7, 0x5a} }
// APTIOV_SERVER_OVERRIDE_RC_END

EFI_GUID UnknownDeviceGuid = UNKNOWN_DEVICE_GUID;

EFI_GUID mEfiMsgPcAnsiGuid = DEVICE_PATH_MESSAGING_PC_ANSI;
EFI_GUID mEfiMsgVt100Guid = DEVICE_PATH_MESSAGING_VT_100;
EFI_GUID mEfiMsgVt100PlusGuid = DEVICE_PATH_MESSAGING_VT_100_PLUS;
EFI_GUID mEfiMsgVt100Utf8Guid = DEVICE_PATH_MESSAGING_VT_UTF8;

CHAR16 *
EFIAPI
CatPrint (
    IN OUT POOL_PRINT *Str,
    IN CHAR16 *fmt,
    ...
)
/**

    Concatenates a formatted unicode string to allocated pool.
    The caller must free the resulting buffer.

    @param Str         - Tracks the allocated pool, size in use, and
                         amount of pool allocated.

    fmt         - The format string

    @retval Allocated buffer with the formatted string printed in it.
    @retval The caller must free the allocated buffer.   The buffer
    @retval allocation is not packed.

**/
{
    UINT16 *AppendStr;
    VA_LIST args;
    UINTN strsize;

    AppendStr = AllocateZeroPool (0x1000);
    if (AppendStr == NULL)
    {
        return Str->Str;
    }

    VA_START (args, fmt);
    UnicodeVSPrint (AppendStr, 0x1000, fmt, args);
    VA_END (args);
    if (NULL == Str->Str)
    {
        strsize = StrSize (AppendStr);
        Str->Str = AllocateZeroPool (strsize);
        ASSERT (Str->Str != NULL);
    }
    else
    {
        strsize = StrSize (AppendStr) + StrSize (Str->Str) - sizeof (UINT16);
        Str->Str = ReallocatePool (
                       StrSize (Str->Str),
                       strsize,
                       Str->Str
                   );
        ASSERT (Str->Str != NULL);
    }

    Str->Maxlen = MAX_CHAR * sizeof (UINT16);
    //strsize - total size of buffer in bytes, strlen - no. of chars not incl. NULL
    if (strsize < Str->Maxlen)
    {
        StrCatS (Str->Str, strsize/sizeof(UINT16), AppendStr);

        Str->Len = strsize - sizeof (UINT16);
    }

    FreePool (AppendStr);
    return Str->Str;
}

/**

  Function unpacks a device path data structure so that all the nodes
  of a device path are naturally aligned.

  @param DevPath        - A pointer to a device path data structure

  @retval If the memory for the device path is successfully allocated, then a
  @retval pointer to the new device path is returned.  Otherwise, NULL is returned.

**/
EFI_DEVICE_PATH_PROTOCOL *
BdsLibUnpackDevicePath (
    IN EFI_DEVICE_PATH_PROTOCOL *DevPath
)
{
    EFI_DEVICE_PATH_PROTOCOL *Src;
    EFI_DEVICE_PATH_PROTOCOL *Dest;
    EFI_DEVICE_PATH_PROTOCOL *NewPath;
    UINTN Size;

    //
    // Walk device path and round sizes to valid boundries
    //
    Src = DevPath;
    Size = 0;
    for (;;)
    {
        Size += DevicePathNodeLength (Src);
        Size += ALIGN_SIZE (Size);

        if (IsDevicePathEnd (Src))
        {
            break;
        }

        Src = NextDevicePathNode (Src);
    }
    //
    // Allocate space for the unpacked path
    //
    NewPath = AllocateZeroPool (Size);
    if (NewPath)
    {

        ASSERT (((UINTN) NewPath) % MIN_ALIGNMENT_SIZE == 0);

        //
        // Copy each node
        //
        Src = DevPath;
        Dest = NewPath;
        for (;;)
        {
            Size = DevicePathNodeLength (Src);
            CopyMem (Dest, Src, Size);
            Size += ALIGN_SIZE (Size);
            SetDevicePathNodeLength (Dest, Size);
            Dest = (EFI_DEVICE_PATH_PROTOCOL *) (((UINT8 *) Dest) + Size);

            if (IsDevicePathEnd (Src))
            {
                break;
            }

            Src = NextDevicePathNode (Src);
        }
    }

    return NewPath;
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathPci (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    PCI_DEVICE_PATH *Pci;

    Pci = DevPath;
    CatPrint (Str, L"Pci(%x|%x)", Pci->Device, Pci->Function);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathPccard (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    PCCARD_DEVICE_PATH *Pccard;

    Pccard = DevPath;
    CatPrint (Str, L"Pcmcia(Function%x)", Pccard->FunctionNumber);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathMemMap (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    MEMMAP_DEVICE_PATH *MemMap;

    MemMap = DevPath;
    CatPrint (
        Str,
        L"MemMap(%d:%.lx-%.lx)",
        MemMap->MemoryType,
        MemMap->StartingAddress,
        MemMap->EndingAddress
    );
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathController (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    CONTROLLER_DEVICE_PATH *Controller;

    Controller = DevPath;
    CatPrint (Str, L"Ctrl(%d)", Controller->ControllerNumber);
}

/**

  Convert Vendor device path to device name

  @param Str     - The buffer store device name
  @param DevPath - Pointer to vendor device path

  @retval When it return, the device name have been stored in *Str.

**/
VOID
DevPathVendor (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    VENDOR_DEVICE_PATH *Vendor;
    CHAR16 *Type;
    INT32 *Temp;

    Vendor = DevPath;
    Temp = (INT32 *) (&Vendor->Guid);

    switch (DevicePathType (&Vendor->Header))
    {
        case HARDWARE_DEVICE_PATH:
            //
            // If the device is a winntbus device, we will give it a readable device name.
            //
            Type = L"Hw";
            break;

        case MESSAGING_DEVICE_PATH:
            //
            // If the device is a winntbus device, we will give it a readable device name.
            //
            if (CompareGuid (&Vendor->Guid, &mEfiMsgPcAnsiGuid))
            {
                CatPrint (Str, L"%s", L"PC-ANSI");
                return ;
            }
            else if (CompareGuid (&Vendor->Guid, &mEfiMsgVt100Guid))
            {
                CatPrint (Str, L"%s", L"VT100");
                return ;
            }
            else if (CompareGuid (&Vendor->Guid, &mEfiMsgVt100PlusGuid))
            {
                CatPrint (Str, L"%s", L"VT100+");
                return ;
            }
            else if (CompareGuid (&Vendor->Guid, &mEfiMsgVt100Utf8Guid))
            {
                CatPrint (Str, L"%s", L"VT100-UTF8");
                return ;
            }
            else
            {
                Type = L"Msg";
                break;
            }

        case MEDIA_DEVICE_PATH:
            Type = L"Media";
            break;

        default:
            Type = L"?";
            break;
    }

    CatPrint (Str, L"Ven%s(%g)", Type, &Vendor->Guid);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathAcpi (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    ACPI_HID_DEVICE_PATH *Acpi;

    Acpi = DevPath;
    if ((Acpi->HID & PNP_EISA_ID_MASK) == PNP_EISA_ID_CONST)
    {
        CatPrint (Str, L"Acpi(PNP%04x,%x)", EISA_ID_TO_NUM (Acpi->HID), Acpi->UID);
    }
    else
    {
        CatPrint (Str, L"Acpi(%08x,%x)", Acpi->HID, Acpi->UID);
    }
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathAtapi (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    ATAPI_DEVICE_PATH *Atapi;

    Atapi = DevPath;
    CatPrint (
        Str,
        L"Ata(%s,%s)",
        Atapi->PrimarySecondary ? L"Secondary" : L"Primary",
        Atapi->SlaveMaster ? L"Slave" : L"Master"
    );
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathScsi (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    SCSI_DEVICE_PATH *Scsi;

    Scsi = DevPath;
    CatPrint (Str, L"Scsi(Pun%x,Lun%x)", Scsi->Pun, Scsi->Lun);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathFibre (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    FIBRECHANNEL_DEVICE_PATH *Fibre;

    Fibre = DevPath;
    CatPrint (Str, L"Fibre(Wwn%lx,Lun%x)", Fibre->WWN, Fibre->Lun);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPath1394 (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    F1394_DEVICE_PATH *F1394;

    F1394 = DevPath;
    CatPrint (Str, L"1394(%g)", &F1394->Guid);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathUsb (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    USB_DEVICE_PATH *Usb;

    Usb = DevPath;
    CatPrint (Str, L"Usb(%x, %x)", Usb->ParentPortNumber, Usb->InterfaceNumber);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathUsbClass (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    USB_CLASS_DEVICE_PATH *UsbClass;

    UsbClass = DevPath;
    CatPrint (
        Str,
        L"Usb Class(%x, %x, %x, %x, %x)",
        UsbClass->VendorId,
        UsbClass->ProductId,
        UsbClass->DeviceClass,
        UsbClass->DeviceSubClass,
        UsbClass->DeviceProtocol
    );
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathI2O (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    I2O_DEVICE_PATH *I2O;

    I2O = DevPath;
    CatPrint (Str, L"I2O(%x)", I2O->Tid);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathMacAddr (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    MAC_ADDR_DEVICE_PATH *MAC;
    UINTN HwAddressSize;
    UINTN Index;

    MAC = DevPath;

    HwAddressSize = sizeof (EFI_MAC_ADDRESS);
    if (MAC->IfType == 0x01 || MAC->IfType == 0x00)
    {
        HwAddressSize = 6;
    }

    CatPrint (Str, L"Mac(");

    for (Index = 0; Index < HwAddressSize; Index++)
    {
        CatPrint (Str, L"%02x", MAC->MacAddress.Addr[Index]);
    }

    CatPrint (Str, L")");
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathIPv4 (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    IPv4_DEVICE_PATH *IP;

    IP = DevPath;
    CatPrint (
        Str,
        L"IPv4(%d.%d.%d.%d:%d)",
        IP->RemoteIpAddress.Addr[0],
        IP->RemoteIpAddress.Addr[1],
        IP->RemoteIpAddress.Addr[2],
        IP->RemoteIpAddress.Addr[3],
        IP->RemotePort
    );
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathIPv6 (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    IPv6_DEVICE_PATH *IP;

    IP = DevPath;
    CatPrint (Str, L"IP-v6(not-done)");
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathInfiniBand (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    INFINIBAND_DEVICE_PATH *InfiniBand;

    InfiniBand = DevPath;
    CatPrint (Str, L"InfiniBand(not-done)");
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathUart (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    UART_DEVICE_PATH *Uart;
    CHAR8 Parity;

    Uart = DevPath;
    switch (Uart->Parity)
    {
        case 0:
            Parity = 'D';
            break;

        case 1:
            Parity = 'N';
            break;

        case 2:
            Parity = 'E';
            break;

        case 3:
            Parity = 'O';
            break;

        case 4:
            Parity = 'M';
            break;

        case 5:
            Parity = 'S';
            break;

        default:
            Parity = 'x';
            break;
    }

    if (Uart->BaudRate == 0)
    {
        CatPrint (Str, L"Uart(DEFAULT %c", Parity);
    }
    else
    {
        CatPrint (Str, L"Uart(%d %c", Uart->BaudRate, Parity);
    }

    if (Uart->DataBits == 0)
    {
        CatPrint (Str, L"D");
    }
    else
    {
        CatPrint (Str, L"%d", Uart->DataBits);
    }

    switch (Uart->StopBits)
    {
        case 0:
            CatPrint (Str, L"D)");
            break;

        case 1:
            CatPrint (Str, L"1)");
            break;

        case 2:
            CatPrint (Str, L"1.5)");
            break;

        case 3:
            CatPrint (Str, L"2)");
            break;

        default:
            CatPrint (Str, L"x)");
            break;
    }
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathHardDrive (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    HARDDRIVE_DEVICE_PATH *Hd;

    Hd = DevPath;
    switch (Hd->SignatureType)
    {
        case SIGNATURE_TYPE_MBR:
            CatPrint (
                Str,
                L"HD(Part%d,Sig%08x)",
                Hd->PartitionNumber,
                *((UINT32 *) (&(Hd->Signature[0])))
            );
            break;

        case SIGNATURE_TYPE_GUID:
            CatPrint (
                Str,
                L"HD(Part%d,Sig%g)",
                Hd->PartitionNumber,
                (EFI_GUID *) &(Hd->Signature[0])
            );
            break;

        default:
            CatPrint (
                Str,
                L"HD(Part%d,MBRType=%02x,SigType=%02x)",
                Hd->PartitionNumber,
                Hd->MBRType,
                Hd->SignatureType
            );
            break;
    }
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathCDROM (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    CDROM_DEVICE_PATH *Cd;

    Cd = DevPath;
    CatPrint (Str, L"CDROM(Entry%x)", Cd->BootEntry);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathFilePath (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    FILEPATH_DEVICE_PATH *Fp;

    Fp = DevPath;
    CatPrint (Str, L"%s", Fp->PathName);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathMediaProtocol (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    MEDIA_PROTOCOL_DEVICE_PATH *MediaProt;

    MediaProt = DevPath;
    CatPrint (Str, L"%g", &MediaProt->Protocol);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathFvFilePath (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *FvFilePath;

    FvFilePath = DevPath;
    CatPrint (Str, L"%g", &FvFilePath->FvFileName);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathBssBss (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    BBS_BBS_DEVICE_PATH *Bbs;
    CHAR16 *Type;

    Bbs = DevPath;
    switch (Bbs->DeviceType)
    {
        case BBS_TYPE_FLOPPY:
            Type = L"Floppy";
            break;

        case BBS_TYPE_HARDDRIVE:
            Type = L"Harddrive";
            break;

        case BBS_TYPE_CDROM:
            Type = L"CDROM";
            break;

        case BBS_TYPE_PCMCIA:
            Type = L"PCMCIA";
            break;

        case BBS_TYPE_USB:
            Type = L"Usb";
            break;

        case BBS_TYPE_EMBEDDED_NETWORK:
            Type = L"Net";
            break;

        default:
            Type = L"?";
            break;
    }
    //
    // Since current Print function hasn't implemented %a (for ansi string)
    // we will only print Unicode strings.
    //
    CatPrint (Str, L"Legacy-%s", Type);
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval None

**/
VOID
DevPathEndInstance (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    CatPrint (Str, L",");
}

/**

    GC_TODO: add routine description

    @param Str     - GC_TODO: add arg description
    @param DevPath - GC_TODO: add arg description

    @retval NULL       - GC_TODO: add retval description
    @retval Str.Str    - GC_TODO: add retval description
    @retval NewDevPath - GC_TODO: add retval description

**/
VOID
DevPathNodeUnknown (
    IN OUT POOL_PRINT *Str,
    IN VOID *DevPath
)
{
    CatPrint (Str, L"?");
}

DEVICE_PATH_STRING_TABLE DevPathTable[] = {
            HARDWARE_DEVICE_PATH,
            HW_PCI_DP,
            DevPathPci,
            HARDWARE_DEVICE_PATH,
            HW_PCCARD_DP,
            DevPathPccard,
            HARDWARE_DEVICE_PATH,
            HW_MEMMAP_DP,
            DevPathMemMap,
            HARDWARE_DEVICE_PATH,
            HW_VENDOR_DP,
            DevPathVendor,
            HARDWARE_DEVICE_PATH,
            HW_CONTROLLER_DP,
            DevPathController,
            ACPI_DEVICE_PATH,
            ACPI_DP,
            DevPathAcpi,
            MESSAGING_DEVICE_PATH,
            MSG_ATAPI_DP,
            DevPathAtapi,
            MESSAGING_DEVICE_PATH,
            MSG_SCSI_DP,
            DevPathScsi,
            MESSAGING_DEVICE_PATH,
            MSG_FIBRECHANNEL_DP,
            DevPathFibre,
            MESSAGING_DEVICE_PATH,
            MSG_1394_DP,
            DevPath1394,
            MESSAGING_DEVICE_PATH,
            MSG_USB_DP,
            DevPathUsb,
            MESSAGING_DEVICE_PATH,
            MSG_USB_CLASS_DP,
            DevPathUsbClass,
            MESSAGING_DEVICE_PATH,
            MSG_I2O_DP,
            DevPathI2O,
            MESSAGING_DEVICE_PATH,
            MSG_MAC_ADDR_DP,
            DevPathMacAddr,
            MESSAGING_DEVICE_PATH,
            MSG_IPv4_DP,
            DevPathIPv4,
            MESSAGING_DEVICE_PATH,
            MSG_IPv6_DP,
            DevPathIPv6,
            MESSAGING_DEVICE_PATH,
            MSG_INFINIBAND_DP,
            DevPathInfiniBand,
            MESSAGING_DEVICE_PATH,
            MSG_UART_DP,
            DevPathUart,
            MESSAGING_DEVICE_PATH,
            MSG_VENDOR_DP,
            DevPathVendor,
            MEDIA_DEVICE_PATH,
            MEDIA_HARDDRIVE_DP,
            DevPathHardDrive,
            MEDIA_DEVICE_PATH,
            MEDIA_CDROM_DP,
            DevPathCDROM,
            MEDIA_DEVICE_PATH,
            MEDIA_VENDOR_DP,
            DevPathVendor,
            MEDIA_DEVICE_PATH,
            MEDIA_FILEPATH_DP,
            DevPathFilePath,
            MEDIA_DEVICE_PATH,
            MEDIA_PROTOCOL_DP,
            DevPathMediaProtocol,
            MEDIA_DEVICE_PATH,
            MEDIA_PIWG_FW_FILE_DP,
            DevPathFvFilePath,
            BBS_DEVICE_PATH,
            BBS_BBS_DP,
            DevPathBssBss,
            END_DEVICE_PATH_TYPE,
            END_INSTANCE_DEVICE_PATH_SUBTYPE,
            DevPathEndInstance,
            0,
            0,
            NULL
        };

/**

  Turns the Device Path into a printable string.  Allcoates
  the string from pool.  The caller must SafeFreePool the returned
  string.

**/
CHAR16 *
EFIAPI
DevicePathToStr (
    IN EFI_DEVICE_PATH_PROTOCOL *DevPath
)
{
    POOL_PRINT Str;
    EFI_DEVICE_PATH_PROTOCOL *DevPathNode;
    VOID (*DumpNode) (POOL_PRINT *, VOID *);

    UINTN Index;
    UINTN NewSize;

    ZeroMem (&Str, sizeof (Str));

    if (DevPath == NULL)
    {
        goto Done;
    }
    //
    // Unpacked the device path
    //
    DevPath = BdsLibUnpackDevicePath (DevPath);
    ASSERT (DevPath);
    if (DevPath == NULL)
    {
        goto Done;
    }

    //
    // Process each device path node
    //
    DevPathNode = DevPath;
    while (!IsDevicePathEnd (DevPathNode))
    {
        //
        // Find the handler to dump this device path node
        //
        DumpNode = NULL;
        for (Index = 0; DevPathTable[Index].Function; Index += 1)
        {

            if (DevicePathType (DevPathNode) == DevPathTable[Index].Type &&
                    DevicePathSubType (DevPathNode) == DevPathTable[Index].SubType
               )
            {
                DumpNode = DevPathTable[Index].Function;
                break;
            }
        }
        //
        // If not found, use a generic function
        //
        if (!DumpNode)
        {
            DumpNode = DevPathNodeUnknown;
        }
        //
        //  Put a path seperator in if needed
        //
        if (Str.Len && DumpNode != DevPathEndInstance)
        {
            CatPrint (&Str, L"/");
        }
        //
        // Print this node of the device path
        //
        DumpNode (&Str, DevPathNode);

        //
        // Next device path node
        //
        DevPathNode = NextDevicePathNode (DevPathNode);
    }
    //
    // Shrink pool used for string allocation
    //
    FreePool (DevPath);

Done:
    NewSize = (Str.Len + 1) * sizeof (CHAR16);
    Str.Str = ReallocatePool (NewSize, NewSize, Str.Str);
    ASSERT (Str.Str != NULL);
    if (Str.Str == NULL) {
      return NULL;
    }
    Str.Str[Str.Len] = 0;
    return Str.Str;
}

/**

  Function creates a device path data structure that identically matches the
  device path passed in.

  @param DevPath      - A pointer to a device path data structure.

  @retval The new copy of DevPath is created to identically match the input.
  @retval Otherwise, NULL is returned.

**/
EFI_DEVICE_PATH_PROTOCOL *
LibDuplicateDevicePathInstance (
    IN EFI_DEVICE_PATH_PROTOCOL *DevPath
)
{
    EFI_DEVICE_PATH_PROTOCOL *NewDevPath;
    EFI_DEVICE_PATH_PROTOCOL *DevicePathInst;
    EFI_DEVICE_PATH_PROTOCOL *Temp;
    UINTN Size;

    //
    // get the size of an instance from the input
    //
    Temp = DevPath;
    DevicePathInst = GetNextDevicePathInstance (&Temp, &Size);

    //
    // Make a copy
    //
    NewDevPath = NULL;
    if (Size)
    {
        NewDevPath = AllocateZeroPool (Size);
        ASSERT (NewDevPath != NULL);
    }

    if (NewDevPath)
    {
        CopyMem (NewDevPath, DevicePathInst, Size);
    }

    return NewDevPath;
}
#endif // AMT_SUPPORT
