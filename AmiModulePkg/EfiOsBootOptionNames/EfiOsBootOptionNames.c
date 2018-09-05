//#**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//***********************************************************************

/** @file EfiOsBootOptionNames.c

    Change the Name of boot Option in Setup into format of
    "OS Name(Px:HDD name)", or "OS Name(HDD name)" if there
    is no port, or "OS Name" if can't find controller name.
*/

#include <AmiDxeLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/DiskInfo.h>
#include "boot.h" // Part of TSE Binary
#include <Token.h>

//
//  External variables (part of AMITSEBin)
// 
extern UINTN gBootOptionCount;
extern BOOT_DATA* gBootData;

//
//  External functions
// 

/*
    This function locates the handle with BlockIo installed that provides block
    access to the partition containing the Windows boot loader.  The load option
    (BootXXXX variable) created by Windows starts with a HD node instead of a 
    complete device path.  The partition signature is used to find the complete
    device path.

    @param  DevicePath  

    @retval  EFI_DEVICE_PATH_PROTOCOL*  
*/
EFI_DEVICE_PATH_PROTOCOL* _DiscoverPartition(
    EFI_DEVICE_PATH_PROTOCOL *DevicePath
);

//
//  DEFINE
// 
#define CONTROLLER_NAME_BUFFER_LENGTH 100


/**
    Takes a handle with BlockIo providing partition access and returns
    the handle with the BlockIo providing raw access to the drive that
    contains the partition.

    @param  BlockIoHandle  Image handle with BlockIo partition access.

    @retval  EFI_HANDLE   Handle with BlockIo that provides raw access to
                          drive containing partition.
*/
EFI_HANDLE GetPhysicalBlockIoHandle (
    IN EFI_HANDLE BlockIoHandle
)
{
    EFI_BLOCK_IO_PROTOCOL *BlkIo;
    EFI_DEVICE_PATH_PROTOCOL *DevicePath, *Dp;
    EFI_STATUS Status;
    EFI_HANDLE Handle = BlockIoHandle;

    Status=pBS->HandleProtocol(
               Handle, &gEfiBlockIoProtocolGuid, &BlkIo
           );
    if (EFI_ERROR(Status)) return Handle;
    if (!BlkIo->Media->LogicalPartition) return Handle;
    Status=pBS->HandleProtocol(
               Handle, &gEfiDevicePathProtocolGuid, &DevicePath
           );
    if (EFI_ERROR(Status)) return Handle;
    Dp=DevicePath;
    while (BlkIo->Media->LogicalPartition) {
        EFI_DEVICE_PATH_PROTOCOL *PrevDp=Dp;
        //We need to cut Devicepath node to get Phisycal Partition
        Dp=DPCut(PrevDp);
        if (PrevDp != DevicePath) pBS->FreePool(PrevDp);
        if (Dp == NULL) break;
        PrevDp=Dp;
        Status=pBS->LocateDevicePath(
                   &gEfiBlockIoProtocolGuid,&PrevDp,&Handle
               );
        if (EFI_ERROR(Status)) break;
        Status=pBS->HandleProtocol(
                   Handle, &gEfiBlockIoProtocolGuid, &BlkIo
               );
        if (EFI_ERROR(Status)) break;
    }
    if (Dp != NULL && Dp != DevicePath) pBS->FreePool(Dp);
    //if physical Block I/O handle is not found, return original handle
    return (BlkIo->Media->LogicalPartition) ? BlockIoHandle : Handle;
}

/**
    Removes trailing spaces from *Name.

    @param  Name  String from which trailing spaces should be removed.
    @param  NumberOfCharacters  Length of string at *Name.

    @retval  UINTN  Length of string after spaces have been removed.
*/
UINTN RemoveTrailingSpaces (
    IN CHAR16 *Name,
    IN UINTN NumberOfCharacters
)
{
    //remove trailing spaces
    while (NumberOfCharacters>0 && Name[NumberOfCharacters-1] == L' ')
        NumberOfCharacters--;
    Name[NumberOfCharacters]=0;
    return NumberOfCharacters;
}

/**
    Construct controller name from parent Handle.

    @param  Handle    Handle with partition BlockIo.
    @param  Name      Caller allocated string to store name in.
    @param  NameSize  Size of string buffer.

    @retval  UINTN  Length of controller name string.
*/
UINTN ConstructBootOptionNameByHandle (
    IN EFI_HANDLE Handle,
    IN CHAR16 *Name,
    IN UINTN NameSize
)
{
    CHAR16 *ControllerName;
    UINTN  NumberOfCharacters;

    if (Handle == NULL) return 0;

    //Name from Controller Handle
    Handle = GetPhysicalBlockIoHandle(Handle);
    if (!GetControllerName(Handle, &ControllerName)) return 0;
    NumberOfCharacters = Swprintf_s(Name, NameSize, L"%s", ControllerName);
    return RemoveTrailingSpaces(Name, NumberOfCharacters);
}

/**
    OEM Variable, PLEASE reference SbSetup.c and modify this function to get correct port number.
*/
UINT16 gSATA[3][2] = {
    { 0, 1 },
    { 2, 3 },
    { 4, 5 }
};

/**
    Use handle and EFI_DISK_INFO_PROTOCOL to get sata hard disk port.

    @param  Handle  Use in locating EFI_DISK_INFO_PROTOCOL. 
  
    @retval  UINT16  Sata port number.
*/
UINT16 GetHDDPort( IN EFI_HANDLE Handle )
{
    EFI_STATUS Status;
    EFI_GUID gEfiDiskInfoProtocolGuid = EFI_DISK_INFO_PROTOCOL_GUID;
    EFI_DISK_INFO_PROTOCOL *DiskInfo;
    EFI_DEVICE_PATH_PROTOCOL *DevicePath;
    UINT32 IdeChannel;
    UINT32 IdeDevice;

    Status = pBS->HandleProtocol (
                 Handle,
                 &gEfiDevicePathProtocolGuid,
                 (VOID *) &DevicePath );

    if ( !EFI_ERROR( Status ))
    {
        EFI_DEVICE_PATH_PROTOCOL *DevicePathNode;
        EFI_DEVICE_PATH_PROTOCOL *MessagingDevicePath;
        PCI_DEVICE_PATH *PciDevicePath;

        DevicePathNode = DevicePath;
        while (!isEndNode (DevicePathNode))
        {
            if ((DevicePathNode->Type == HARDWARE_DEVICE_PATH)
                    && (DevicePathNode->SubType == HW_PCI_DP))
                PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
            else if (DevicePathNode->Type == MESSAGING_DEVICE_PATH)
                MessagingDevicePath = DevicePathNode;

            DevicePathNode = NEXT_NODE (DevicePathNode);
        }

        Status = pBS->HandleProtocol ( Handle, &gEfiDiskInfoProtocolGuid, &DiskInfo );
        if ( !EFI_ERROR(Status) )
        {
            Status = DiskInfo->WhichIde ( DiskInfo, &IdeChannel, &IdeDevice );
            if ( !EFI_ERROR(Status) )
            {
                if ( MessagingDevicePath->SubType == MSG_ATAPI_DP ) //IDE mode?
                {
                    if (PciDevicePath->Function == 5)
                        return gSATA[IdeDevice+2][IdeChannel];
                    else
                        return gSATA[IdeDevice][IdeChannel];
                }
                else
                    return IdeChannel;	//AHCI Port Number
            }
        }
    }
    return 0xff;
}
 
/**
    Create boot option name into format of "OS NAME(Px:HDD Name), or 
    "OS NAME(HDD Name) if there is no port, or "OS_NAME" if can't find
    controller name.

    @param  DevicePath  Use in getting complete device path of partition.
    @param  BootOptionName  Original boot option name.

    @retval CHAR16*  New boot option name string.
*/
CHAR16* CreateNameWithUefiOS( IN EFI_DEVICE_PATH_PROTOCOL *DevicePath, IN CHAR16 *BootOptionName )
{
    EFI_DEVICE_PATH_PROTOCOL* FullDevicePath;

    FullDevicePath = _DiscoverPartition(DevicePath);

    // Make sure that handle associated with full
    // device path supports Simple File System.
    if (FullDevicePath != NULL)
    {
        EFI_DEVICE_PATH_PROTOCOL* pTempDevicePath = FullDevicePath;
        EFI_HANDLE TempHandle = NULL;
        UINTN NewStringLength = 0;
        CHAR16* NewString = NULL;
        EFI_STATUS Status;

        Status = pBS->LocateDevicePath (
                     &gEfiSimpleFileSystemProtocolGuid,
                     &pTempDevicePath,
                     &TempHandle );

        if (EFI_ERROR(Status))
        {
            TRACE((-1, "Does not support Simple File System.\n"));
            NewStringLength = Wcslen(L" (Drive not present)") + Wcslen(BootOptionName) + 1;

            Status = pBS->AllocatePool ( EfiBootServicesData,
                                         NewStringLength * sizeof(CHAR16),
                                         &NewString );

            if (EFI_ERROR(Status))
            {
                TRACE((-1, "AllocatePool: %r\n", Status));
                return NULL;
            }

            Swprintf_s ( NewString,
                         NewStringLength,
                         L"%s (Drive not present)",
                         BootOptionName );

        }
        else
        {
            EFI_HANDLE BlkIoPartitionHandle = NULL;
            EFI_HANDLE ControllerHandle = NULL;
            CHAR16 ControllerNameBuffer[CONTROLLER_NAME_BUFFER_LENGTH];
            UINTN ControllerNameLength = 0;
            UINT16 PortNumber = 0;

            // Locate handle with above device path..
            Status = pBS->LocateDevicePath (
                         &gEfiBlockIoProtocolGuid,
                         &FullDevicePath,
                         &BlkIoPartitionHandle );

            if (EFI_ERROR(Status))
            {
                TRACE((-1, "LocateDevicePath: %r\n", Status));
                return NULL;
            }

            ControllerHandle = GetPhysicalBlockIoHandle(BlkIoPartitionHandle);
            if(ControllerHandle == BlkIoPartitionHandle)
                return NULL;

            ControllerNameLength = ConstructBootOptionNameByHandle (
                                       ControllerHandle,
                                       ControllerNameBuffer,
                                       CONTROLLER_NAME_BUFFER_LENGTH );

            // Allocate proper memory for new string and copy updated string to it.
            NewStringLength = Wcslen(ControllerNameBuffer) + Wcslen(BootOptionName) + 0xf;

            Status = pBS->AllocatePool (
                         EfiBootServicesData,
                         NewStringLength * sizeof(CHAR16),
                         &NewString );

            if (EFI_ERROR(Status))
            {
                TRACE((-1, "AllocatePool: %r\n", Status));
                return NULL;
            }

            // Append OS name and update bootdata pointer
            PortNumber = GetHDDPort( ControllerHandle );
            if (PortNumber != 0xff)
            {
                Swprintf_s ( NewString,
                             NewStringLength,
                             L"%s (P%d: %s)",
                             BootOptionName,
                             PortNumber,
                             ControllerNameBuffer );
            }
            else
            {
                if ( ControllerNameLength == 0)
                {
                    Swprintf_s ( NewString,
                                 NewStringLength,
                                 L"%s",
                                 BootOptionName );
                }
                else
                {
                    Swprintf_s ( NewString,
                                 NewStringLength,
                                 L"%s (%s)",
                                 BootOptionName,
                                 ControllerNameBuffer );
                }
            }
        }

        return NewString;

    }// if (FullDevicePath != NULL)

    return NULL;
}

/**
    Examines boot options and adds drive information string to them.
    This function is called at the ProcessEnterSetup hook provided by TSE.
*/
VOID ChangeUefiBootNames(
    VOID
)
{
    UINTN i = 0;

    // Loop through all present boot options
    for ( i = 0; i < gBootOptionCount; i++ )
    {
        BOOT_DATA *bootData = NULL;

        bootData = &(gBootData[i]);

        if ( bootData->DevicePath->Type == MEDIA_DEVICE_PATH
                && bootData->DevicePath->SubType == MEDIA_HARDDRIVE_DP )
        {
            CHAR16 *NewBootOptionName;
            NewBootOptionName = CreateNameWithUefiOS( bootData->DevicePath, bootData->Name);
            if ( NewBootOptionName )
            {
                TRACE((-1, "Change Name : %S\n", NewBootOptionName));
                bootData->Name = NewBootOptionName;
            }
            else
                TRACE((-1, "Name is NULL\n"));
        }
    }//for( i = 0; i < gBootOptionCount; i++ )
    return;
}

//***********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//***********************************************************************
