//**********************************************************************
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
//**********************************************************************


/** @file EfiOsBootOrder.c

    Deal with UEFI Boot Devices that contain UEFI OS.

    Create boot option if device is existed, delete 
    boot option if there is a duplicated one and then 
    adjust the BootOrder.
*/

#include <BootOptions.h>
#include "EfiOsNamesFilePathMaps.h"
#include <Protocol/DiskInfo.h>
#include <Token.h>
#include <Setup.h>
#include <EfiOsBootOptionNames.h>

#if FIXED_BOOT_ORDER_SUPPORT
#include "FixedBootOrder/FixedBootOrder.h"
#endif

typedef EFI_STATUS (CREATE_EFI_OS_BOOT_OPTIONS) (EFI_HANDLE Handle);
extern CREATE_EFI_OS_BOOT_OPTIONS CREATE_EFI_OS_BOOT_OPTIONS_FUNC_PTR;
CREATE_EFI_OS_BOOT_OPTIONS* CreateEfiOsBootOptionsFuncPtr = CREATE_EFI_OS_BOOT_OPTIONS_FUNC_PTR;

typedef UINTN (CREATE_EFI_OS_BOOT_OPTION_FUNCS) (EFI_HANDLE Handle, EFI_FILE_PROTOCOL* File);
extern CREATE_EFI_OS_BOOT_OPTION_FUNCS CREATE_EFI_OS_BOOT_OPTION_FUNCTIONS EndOfCreateEfiOsBootOptionFunctions;
CREATE_EFI_OS_BOOT_OPTION_FUNCS* CreateEfiOsBootOptionFunctions[] = {CREATE_EFI_OS_BOOT_OPTION_FUNCTIONS NULL};

typedef BOOLEAN (CREATE_TARGET_EFI_OS_BOOT_OPTION) (EFI_HANDLE Handle, NAME_MAP* NameMap);
extern CREATE_TARGET_EFI_OS_BOOT_OPTION CREATE_TARGET_EFI_OS_BOOT_OPTION_FUNC_PTR;
CREATE_TARGET_EFI_OS_BOOT_OPTION* CreateTargetEfiOsBootOptionFuncPtr = CREATE_TARGET_EFI_OS_BOOT_OPTION_FUNC_PTR;

typedef VOID (ADJUST_EFI_OS_BOOT_ORDER_FUNCS) (SIMPLE_BOOT_OPTION* Options, UINTN OptionCount);
extern ADJUST_EFI_OS_BOOT_ORDER_FUNCS ADJUST_EFI_OS_BOOT_ORDER_FUNCTIONS EndOfAdjustEfiOsbootOrderFunctions;
ADJUST_EFI_OS_BOOT_ORDER_FUNCS* AdjustEfiOsBootOrderFunctions[] =  {ADJUST_EFI_OS_BOOT_ORDER_FUNCTIONS NULL};

typedef BOOLEAN (CHECK_UEFI_OS_BOOT_OPTION_FUNCS) (BOOT_OPTION* Options);
extern CHECK_UEFI_OS_BOOT_OPTION_FUNCS CHECK_UEFI_OS_BOOT_OPTION_FUNCTIONS EndOfCheckUefiOsFunctions;
CHECK_UEFI_OS_BOOT_OPTION_FUNCS* CheckUefiOsBootOptionFunctions[] =  {CHECK_UEFI_OS_BOOT_OPTION_FUNCTIONS NULL};

/**
    Arrary of eLinks that contain many OS image paths and their corresponding OS names.
*/
NAME_MAP FILE_NAME_MAPS[] = { EfiOsFilePathMaps {NULL,NULL} };	//(EIP103672+)

extern BOOLEAN NormalizeBootOptionName ;
extern BOOLEAN NormalizeBootOptionDevicePath ;

#if DISPLAY_FULL_OPTION_NAME_WITH_FBO
extern EFI_HANDLE GetPhysicalBlockIoHandle (IN EFI_HANDLE BlockIoHandle) ;
extern UINTN RemoveTrailingSpaces(CHAR16 *Name, UINTN NumberOfCharacters);
extern UINT16 gSATA[3][2] ;
#endif

EFI_DEVICE_PATH_PROTOCOL **NewEfiOsOptionDpList = NULL ;
UINTN NewEfiOsOptionDpListCount = 0 ;

/**
    Locates HD node in DevicePath associated with Handle.

    @param  Handle    Handle with DevicePath protocol for which HD
                      node should be located.
    @param  DevPath   Pointer to HD node, if found.

    @retval  EFI_SUCCESS    HD node was found and returned.
    @retval  EFI_NOT_FOUND  No HD node was found.
    @return  Other errors, if Handle does not have DevicePath protocol.
*/
EFI_STATUS GetHdNode (
    IN EFI_HANDLE Handle,
    OUT EFI_DEVICE_PATH_PROTOCOL** DevPath
)
{
    EFI_STATUS Status;
    EFI_DEVICE_PATH_PROTOCOL* DevicePath;

    *DevPath = NULL;

    // Get DevicePath attached to handle.
    Status = pBS->HandleProtocol (
                 Handle,
                 &gEfiDevicePathProtocolGuid,
                 &DevicePath
             );
    if (EFI_ERROR(Status)) {
        TRACE((-1, "HandleProtocol: %r\n", Status));
        return Status;
    }

    // Find hard drive node.
    while (!isEndNode(DevicePath)) {

        if ((DevicePath->Type == MEDIA_DEVICE_PATH) &&
                (DevicePath->SubType == MEDIA_HARDDRIVE_DP)) {

            *DevPath = DevicePath;
            return EFI_SUCCESS;
        }

        DevicePath = NEXT_NODE(DevicePath);
    }

    // HD node was not found.  Return error.
    return EFI_NOT_FOUND;
}

/**
    Non case sensitive string comparison.

    @param  Str1   String to compare
    @param  Str2   String be compared to

    @retval  TRUE    Strings are the same
    @retval  FALSE   Strings aren't the same
**/
BOOLEAN StrNoCaseCmp(CHAR16* Str1, CHAR16* Str2)
{
    while(*Str1)
    {
        if(*Str1 != *Str2)
        {
            CHAR16 Char = *Str1;

            if(Char >= 0x41 && Char <= 0x5a)
                Char += 0x20;
            else if(Char >= 0x61 && Char <= 0x7a)
                Char -= 0x20;
            else
                return FALSE;

            if(Char != *Str2)
                return FALSE;
        }

        Str1++;
        Str2++;
    }

    return (*Str1 == *Str2);
    
}

#if DISPLAY_FULL_OPTION_NAME_WITH_FBO
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
                    return IdeChannel;  //AHCI Port Number
            }
        }
    }
    return 0xff;
}
#endif

/**
    Search GPT HDD and return Hard disk handle.

    @param  DevicePath  Converting to HARDDRIVE_DEVICE_PATH.
    
    @retval  EFI_HANDLE  Hard Disk handle or NULL.
*/
EFI_HANDLE GetGptPartitionHandle(EFI_DEVICE_PATH_PROTOCOL *DevicePath)
{
    EFI_STATUS	Status;
    EFI_HANDLE	*Handle, TempHandle = NULL;
    UINTN	Count, i;

    HARDDRIVE_DEVICE_PATH* BootParitionDevicePath  = (HARDDRIVE_DEVICE_PATH*)DevicePath;

    //get list of available Block I/O devices
    Status = pBS->LocateHandleBuffer(ByProtocol,&gEfiBlockIoProtocolGuid,NULL,&Count,&Handle);
    if (EFI_ERROR(Status)) return NULL;

    for ( i=0; i<Count; i++ )
    {
        EFI_BLOCK_IO_PROTOCOL		*BlockIo;
        EFI_DEVICE_PATH_PROTOCOL	*PartitionDevicePath, *TmpDevicePath;
        HARDDRIVE_DEVICE_PATH*		PartitionNode;

        Status = pBS->HandleProtocol(Handle[i],&gEfiBlockIoProtocolGuid,&BlockIo);
        if (EFI_ERROR(Status))
            continue;

        // if this is not partition, continue
        if (!BlockIo->Media->LogicalPartition)
            continue;

        Status = pBS->HandleProtocol(Handle[i],&gEfiDevicePathProtocolGuid,&PartitionDevicePath);
        if (EFI_ERROR(Status))
            continue;

        // Get last node of the device path. It should be partition node
        PartitionNode = (HARDDRIVE_DEVICE_PATH*)PartitionDevicePath;

        for ( TmpDevicePath = PartitionDevicePath;
                !isEndNode(TmpDevicePath);
                TmpDevicePath=NEXT_NODE(TmpDevicePath) )
        {
            PartitionNode = (HARDDRIVE_DEVICE_PATH*)TmpDevicePath;
        }

        //Check if our partition matches Boot partition
        if (PartitionNode->Header.Type != MEDIA_DEVICE_PATH || PartitionNode->Header.SubType != MEDIA_HARDDRIVE_DP)
            continue;

        if ( PartitionNode->PartitionNumber == BootParitionDevicePath->PartitionNumber &&
                PartitionNode->SignatureType == BootParitionDevicePath->SignatureType &&
                !MemCmp(PartitionNode->Signature,BootParitionDevicePath->Signature,16) )
        {
            //Match found
            TempHandle = Handle[i];
            break;
        }
    }

    pBS->FreePool(Handle);
    return TempHandle;
}
//(EIP126686+)>
/**
    Compare whether two HDD device paths are the same.

    @param  DevDp1  Device path in comparison.
    @param  DevDp2  Device path in comparison.

    @retval  EFI_SUCCESS    Two HDD paths are the same.
    @retval  EFI_NOT_FOUND  Two Hdd paths are not the same.
*/
EFI_STATUS CompareHddDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevDp1, EFI_DEVICE_PATH_PROTOCOL *DevDp2 )
{

    if ( DevDp1->Type == MEDIA_DEVICE_PATH &&
            DevDp1->SubType == MEDIA_HARDDRIVE_DP )
    {
        if (MemCmp(DevDp1+1, DevDp2+1, sizeof(HARDDRIVE_DEVICE_PATH)-sizeof(EFI_DEVICE_PATH_PROTOCOL)) == 0) //Skip Header EFI_DEVICE_PATH_PROTOCOL.
        {
            DevDp1 = NEXT_NODE(DevDp1);
            if ( DevDp1->Type == MEDIA_DEVICE_PATH &&
                    DevDp1->SubType == MEDIA_FILEPATH_DP ) Wcsupr( (CHAR16*)DevDp1+1 );

            DevDp2 = NEXT_NODE(DevDp2);
            if ( DevDp2->Type == MEDIA_DEVICE_PATH &&
                    DevDp2->SubType == MEDIA_FILEPATH_DP ) Wcsupr( (CHAR16*)DevDp2+1 );

            if (MemCmp(DevDp1, DevDp2, DPLength(DevDp2)) == 0)
                return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}
//<(EIP126686+)

/**
  	Search all BootOptionList and found out the matched HDD device path.

    @param  HdDevPath  HDD device path.

    @retval  EFI_SUCCESS    Find out the matched HDD device path.
    @retval  EFI_NOT_FOUND  Can't find any matched HDD device path.
*/
EFI_STATUS CheckBootOptionMatch (
    IN EFI_DEVICE_PATH_PROTOCOL* HdDevPath
)
{
    DLINK *Link;
    BOOT_OPTION *Option;

    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option) {

        //(EIP126686+)>
        if ( CompareHddDevicePath(Option->FilePathList, HdDevPath) == EFI_SUCCESS )
            return EFI_SUCCESS;
        //<(EIP126686+)
    }
    return EFI_NOT_FOUND;
}

//(EIP103672+)>
/**
    Count the elements in eLink FILE_NAME_MAPS.

    @retval  UINT16  Number of elements in eLink FILE_NAME_MAPS. 
*/
UINT16 GetEfiOsBootNameItemCount(void)
{
    UINT16 ItemCount=0;

    do {

        if ( FILE_NAME_MAPS[ItemCount].FilePath == NULL ) break;

        ItemCount++;

    } while (1);

    return ItemCount;
}
//<(EIP103672+)


/**
    Create target efi os boot option.

    @param  FileSystemHandle   Handle to get file system
    @param  NameMap            Contain boot file name and boot option name

    @retval  TRUE    if find any matched boot option / create boot option successfully
    @retval  FALSE   if one of inputs is invalid / fail to allocate pool / fail to get hdd node 

**/
BOOLEAN CreateTargetEfiOsBootOption(EFI_HANDLE FileSystemHandle, NAME_MAP* NameMap)
{
    EFI_STATUS Status;
    UINTN OptionSize;
    UINT8* BytePtr;
    EFI_DEVICE_PATH_PROTOCOL *DevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL *HdDevPath = NULL;
    FILEPATH_DEVICE_PATH *FpDevPath = NULL;
    BOOT_OPTION *Option;

    if(!FileSystemHandle || !NameMap)
        return FALSE;

    // Find total size of new boot option.
    OptionSize = 	sizeof(HARDDRIVE_DEVICE_PATH) + // Partition node
                  sizeof(FILEPATH_DEVICE_PATH) + // FilePath node
                  ((Wcslen(NameMap->FilePath) ) * sizeof(CHAR16)) +	//(EIP120976)
                  sizeof(EFI_DEVICE_PATH_PROTOCOL); //+ // End node			//(EIP103870)

    Status = pBS->AllocatePool (
                 EfiBootServicesData,
                 OptionSize,
                 &DevicePath);

    if (EFI_ERROR(Status)) {
        TRACE((-1, "AllocatePool %r\n", Status));
        return FALSE;
    }

    // Get HD node of device path associated with handle.
    Status = GetHdNode (
                 FileSystemHandle,
                 &HdDevPath );

    if (EFI_ERROR(Status)) {
        TRACE((-1, "GetHdNode: %r\n", Status));
        pBS->FreePool(DevicePath);
        return FALSE;
    }

    BytePtr = (UINT8*)DevicePath;
    // Copy to FilePath.
    MemCpy(BytePtr, HdDevPath, NODE_LENGTH(HdDevPath));

    // Point to next node.
    BytePtr += NODE_LENGTH(HdDevPath);
    FpDevPath = (FILEPATH_DEVICE_PATH*)BytePtr;

    // Set Filepath node.
    FpDevPath->Header.Type = MEDIA_DEVICE_PATH;
    FpDevPath->Header.SubType = MEDIA_FILEPATH_DP;
    SET_NODE_LENGTH(&(FpDevPath->Header), 4 + (UINT16)((Wcslen(NameMap->FilePath) + 1) * sizeof(CHAR16)));

    // Set Filepath PathName.
    MemCpy(FpDevPath->PathName, NameMap->FilePath, (Wcslen(NameMap->FilePath) + 1) * sizeof(CHAR16));

    // Point to next node.
    BytePtr += NODE_LENGTH(&(FpDevPath->Header));
    ((EFI_DEVICE_PATH_PROTOCOL*)BytePtr)->Type = END_DEVICE_PATH;
    ((EFI_DEVICE_PATH_PROTOCOL*)BytePtr)->SubType = END_ENTIRE_SUBTYPE;
    SET_NODE_LENGTH((EFI_DEVICE_PATH_PROTOCOL*)BytePtr, END_DEVICE_PATH_LENGTH);

    // Point to signature.
    BytePtr += END_DEVICE_PATH_LENGTH;

#if DISPLAY_FULL_OPTION_NAME_WITH_FBO
    //
    // Uefi OS in Setup as style "OS Name(PX: DeviceName)" because of 
    // EfiOsBootOptionNames module creates it by ChangeUefiBootNames
    // eLink to change bootData->Name, however, FixedBootOrder module
    // creates its oneof items with BootXXXX variable's description 
    // which saved according to Option->Description, that makes only
    // "OS Name" appear in setup, hence we need to use FixedBootOrder
    // protcol to set new description.
    //
    {
        CHAR16 *ControllerName ;
        CHAR16 *DeviceName ;
        UINT16 PortNumber = 0xff ;
        UINTN  NumberOfCharacters = 0, DeviceNameLength = 0;
        EFI_HANDLE Handle ;
        EFI_FIXED_BOOT_ORDER_PROTOCOL *Fbo = NULL ;
        EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID ;
        
        Status = pBS->LocateProtocol(&FixedBootOrderGuid, NULL, &Fbo);
        Handle = GetPhysicalBlockIoHandle(FileSystemHandle);
        if (GetControllerName(Handle, &ControllerName) && !EFI_ERROR(Status)) 
        {   
            DeviceNameLength = (StrLen(ControllerName)+1)*sizeof(CHAR16) ;
            DeviceName = MallocZ(DeviceNameLength) ;
            PortNumber = GetHDDPort( Handle );
            NumberOfCharacters = Swprintf_s(DeviceName, DeviceNameLength, L"%s", ControllerName);
            DeviceNameLength = RemoveTrailingSpaces(DeviceName, NumberOfCharacters);
            if ( DeviceNameLength )
            {
                FBO_DEVICE_INFORM *inform ;
                CHAR16 *String ;
                UINTN StringLength = DeviceNameLength*sizeof(CHAR16)
                                     + StrLen(NameMap->BootOptionName)*sizeof(CHAR16)
                                     + 0xf ;
                String = MallocZ(StringLength) ;
                if ( PortNumber != 0xff )
                {
                    Swprintf_s ( String,
                                StringLength,
                                L"%s (P%d: %s)",
                                NameMap->BootOptionName,
                                PortNumber,
                                DeviceName );
                    TRACE((-1, "EfiOsBootOrder.c Reset the Boot Option Name:%S \n", String)) ;
                }
                else  // It don't have port number
                {
                    Swprintf_s ( String,
                                StringLength,
                                L"%s (%s)",
                                NameMap->BootOptionName,
                                DeviceName );
                    TRACE((-1, "EfiOsBootOrder.c Reset the Boot Option Name:%S \n", String)) ;
                }
                
                inform = MallocZ(sizeof(FBO_DEVICE_INFORM)) ;
                inform->DevName = String ;
                inform->DevPath = MallocZ(DPLength(DevicePath)) ;
                MemCpy(inform->DevPath, DevicePath, DPLength(DevicePath)) ;
                Status = Fbo->SetNewDescription(inform) ;
                if (EFI_ERROR(Status))
                    TRACE((-1, "Can't set new description with fbo protocol")) ;
            }
            pBS->FreePool(DeviceName) ;
        }
    }
#endif

    if ( CheckBootOptionMatch( DevicePath ) == EFI_SUCCESS )
    {
        TRACE((-1,"CheckBootOptionMatch Matched.....\n" ));
        pBS->FreePool(DevicePath);
        return TRUE;
    }

    Option = CreateBootOption(BootOptionList);
    Option->Attributes = LOAD_OPTION_ACTIVE;           //(EIP138397)
    Option->FwBootOption = DefaultFwBootOption;                                    //(EIP138397+)

    pBS->AllocatePool (
        EfiBootServicesData,
        ((Wcslen(NameMap->BootOptionName) + 1) * sizeof(CHAR16)),
        &Option->Description);

    MemCpy(Option->Description,
           NameMap->BootOptionName,
           ((Wcslen(NameMap->BootOptionName) + 1) * sizeof(CHAR16)) );

    
    Option->FilePathList = DevicePath;
    Option->FilePathListLength = OptionSize;
           
    {
        VOID **ptr = NULL ;
        ptr = MallocZ(sizeof(VOID**)
                      * (NewEfiOsOptionDpListCount+1)) ;
        if (ptr)
        {
            MemCpy(ptr, NewEfiOsOptionDpList, sizeof(VOID**)
                                          * NewEfiOsOptionDpListCount) ;
            *(ptr+NewEfiOsOptionDpListCount) = Option->FilePathList ;

            NewEfiOsOptionDpListCount++ ;
            pBS->FreePool(NewEfiOsOptionDpList) ;
            NewEfiOsOptionDpList = (EFI_DEVICE_PATH_PROTOCOL**)ptr ;
        }
    }

    return TRUE;
}

/**
    Create boot option with uefi file name if there is no other matched path.

    @param  Handle                     Handle to locate file system

**/    
EFI_STATUS CreateBootOptionWithUefiFileName(EFI_HANDLE Handle, EFI_FILE_PROTOCOL* File)
{
    EFI_STATUS Status;
    EFI_FILE_PROTOCOL* NewFile;

    if(!Handle || !File) return EFI_INVALID_PARAMETER;

    TRACE((-1, "\nCreateBootOptionsWithUefiFileName\n"));

    Status = File->Open (
                 File,
                 &NewFile,
                 EFI_BOOT_FILE_NAME,
                 EFI_FILE_MODE_READ,
                 0
             );

    TRACE((-1, "Open(%S): %r\n", EFI_BOOT_FILE_NAME, Status));
    if(!EFI_ERROR(Status))
    {
        static NAME_MAP NameMap = {EFI_BOOT_FILE_NAME, NAME_OF_UEFI_OS};
        CreateTargetEfiOsBootOptionFuncPtr(Handle, &NameMap);

        NewFile->Close(NewFile);
    }

    return Status;
}

/**
    Create boot options by map.

    @param  Handle   Handle to locate file system.

    @retval  UINTN   Number of boot option created
**/
UINTN CreateBootOptionsByMap(EFI_HANDLE Handle, EFI_FILE_PROTOCOL* File)
{
    UINTN j;
    UINTN Count = 0;
    EFI_STATUS Status;
    UINT16 AUTO_BOOT_ENTRY_COUNT = GetEfiOsBootNameItemCount();

    if(!Handle || !File) return Count;

    TRACE((-1, "\nCreateBootOptionsByMap\n"));

    for (j = 0; j < AUTO_BOOT_ENTRY_COUNT; j++)
    {

        EFI_FILE_PROTOCOL* NewFile;

        Status = File->Open (
                     File,
                     &NewFile,
                     FILE_NAME_MAPS[j].FilePath,
                     EFI_FILE_MODE_READ,
                     0
                 );

        TRACE((-1, "Open(%S): %r\n", FILE_NAME_MAPS[j].FilePath, Status));
        if (EFI_ERROR(Status)) continue;
        NewFile->Close(NewFile);

        if(CreateTargetEfiOsBootOptionFuncPtr(Handle, &FILE_NAME_MAPS[j]))
            Count++;
    }

    return Count;
}

/**
    Create efi os boot options.

    @param  Handle   Handle to check and create boot options.

    @retval  Status = EFI_SUCCESS    Creation is done
    @retval  Status != EFI_SUCCESS   Handle is null or HandleProtocol failed
                                     or OpenVolume failed
**/
EFI_STATUS CreateEfiOsBootOptions(EFI_HANDLE Handle)
{
    EFI_STATUS Status;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* SimpleFileSystem;

    if(!Handle) return EFI_INVALID_PARAMETER;

    Status = pBS->HandleProtocol (
                 Handle,
                 &gEfiSimpleFileSystemProtocolGuid,
                 &SimpleFileSystem
             );

    if(!EFI_ERROR(Status))
    {
        EFI_FILE_PROTOCOL *File;
        Status = SimpleFileSystem->OpenVolume (
                     SimpleFileSystem,
                     &File);

        if(!EFI_ERROR(Status))
        {
            UINTN i = 0, Count = 0;
            for (; CreateEfiOsBootOptionFunctions[i]; i++)
                Count += CreateEfiOsBootOptionFunctions[i](Handle, File);

#if CREATE_BOOT_OPTION_WITH_UEFI_FILE_NAME_POLICY == 1
            if(!Count)
                CreateBootOptionWithUefiFileName(Handle, File);
#endif
        }
    }

    return Status;
}

/**
	Try to find UEFI OSs and create the boot options for them if 
    they haven't been listed in BootOptionList.

    @retval  EFI_SUCCESS   Do not encounter any errors.
    @retval  !=EFI_SUCESS  Any errors.
*/
EFI_STATUS CreateEfiOsBootOption(VOID)
{
    EFI_STATUS   Status;
    EFI_HANDLE*  HandleBuffer = NULL;
    UINTN        HandleCount;
    UINTN        i;
    EFI_BLOCK_IO_PROTOCOL *BlkIo;

    Status = pBS->LocateHandleBuffer (
                 ByProtocol,
                 &gEfiSimpleFileSystemProtocolGuid,
                 NULL,
                 &HandleCount,
                 &HandleBuffer
             );
    if (EFI_ERROR(Status)) {
        TRACE((-1, "LocateHandleBuffer: %r\n", Status));
        return Status;
    }
    // For each handle found, check if eLink files exist.
    for (i = 0; i < HandleCount; i++) {

        Status=pBS->HandleProtocol( HandleBuffer[i], &gEfiBlockIoProtocolGuid, &BlkIo );
        if ( EFI_ERROR(Status) || BlkIo->Media->RemovableMedia ) continue;	//skip removable device

        CreateEfiOsBootOptionsFuncPtr(HandleBuffer[i]);
    }

    if ( HandleBuffer )
        pBS->FreePool(HandleBuffer);

    return EFI_SUCCESS;
}

                                                                 //(EIP138397+)>
/**
    Remove the AMI specific device path from SrcDevicePath.

    @param  SrcDevicePath   Device Path that has AMI mask device path.
    @param  DevicePathSize  Size of SrcDevicePath.
*/   
VOID RemoveAmiMaskDevicePath( EFI_DEVICE_PATH_PROTOCOL **SrcDevicePath, UINT16 DevicePathSize)
{
    EFI_DEVICE_PATH_PROTOCOL *TmpDevicePath = *SrcDevicePath;
    EFI_GUID AmiMaskedDevicePathGuid = AMI_MASKED_DEVICE_PATH_GUID;

    if(  TmpDevicePath->Type == HARDWARE_DEVICE_PATH
     && TmpDevicePath->SubType == HW_VENDOR_DP 
     && guidcmp(&AmiMaskedDevicePathGuid, &((VENDOR_DEVICE_PATH*)TmpDevicePath)->Guid) == 0 )
    {

        do{
 
            if( TmpDevicePath->Type == MEDIA_DEVICE_PATH 
             && TmpDevicePath->SubType == MEDIA_HARDDRIVE_DP )
            {
                *(SrcDevicePath) = TmpDevicePath;
                break;
            }
            else
            {
                DevicePathSize -= (*(UINT16*)&(TmpDevicePath)->Length[0]);
                TmpDevicePath = NEXT_NODE( TmpDevicePath );
            }

        }while( DevicePathSize > 0 );
    }
}
                                                                 //<(EIP138397+)

/**
    Check whether input boot option number is created by OS.

    @param  BootOptionNumber   Number to check
    
    @retval  TRUE    Created by OS
    @retval  FALSE   Isn't created by OS
*/    
BOOLEAN IsOsCreatedBootOption(UINT16 BootOptionNumber)
{
    EFI_STATUS Status;
    EFI_GUID   EfiOsBootOptionNamesGuid = EFI_OS_BOOT_OPTION_NAMES_GUID;
    UINT16*    EfiOsBootOrder = NULL;
    UINTN      Size = 0;
    BOOLEAN    Ret = TRUE;

    Status = GetEfiVariable(
                L"EfiOsBootOrder",
                &EfiOsBootOptionNamesGuid,
                NULL,
                &Size,
                &EfiOsBootOrder);

    if(!EFI_ERROR(Status))
    {
        UINTN      i;

        Size = Size/sizeof(UINT16);
        for(i = 0; i < Size; i++)
        {
            if(EfiOsBootOrder[i] == BootOptionNumber)
            {
                Ret = FALSE;
                break;
            }
        }

        pBS->FreePool(EfiOsBootOrder);
    }

    return Ret;
}

/**
    Check whether the uefi os boot option without boot file.

    @param Dp   Device path to check

    @retval  TRUE    Find out the boot file.
    @retval  FALSE   Doesn't find out the boot file.

**/
BOOLEAN IsBootOptionWithoutBootFile(EFI_DEVICE_PATH_PROTOCOL *Dp)
{
    if(Dp)
    {
        EFI_HANDLE DevHandle = GetGptPartitionHandle(Dp);

        if(DevHandle)
        {
            EFI_STATUS Status;
            EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* SimpleFileSystem = NULL;
            EFI_FILE_PROTOCOL *File= NULL;

            Status = pBS->HandleProtocol (
                    DevHandle,
                    &gEfiSimpleFileSystemProtocolGuid,
                    &SimpleFileSystem);

            if(!EFI_ERROR(Status))
            {
                Status = SimpleFileSystem->OpenVolume (
                            SimpleFileSystem,
                            &File);

                if(!EFI_ERROR(Status))
                {
                    EFI_FILE_PROTOCOL* NewFile;

                    Status = File->Open (
                              File,
                              &NewFile,
                              EFI_BOOT_FILE_NAME,
                              EFI_FILE_MODE_READ,
                              0);

                    if(!EFI_ERROR(Status))
                    {
                        NewFile->Close(NewFile);
                    }
                    else if(Status == EFI_NOT_FOUND)
                    {
                        return TRUE;
                    }
                }
            }
        }
    }

    return FALSE;
}


/**
    Read part of information from boot options into SIMPLE_BOOT_OPTION structure.

    @param  Options   To get the simple boot options

    @retval  UINT16   Number of simple options
**/
UINT16 GetSimpleBootOptions(SIMPLE_BOOT_OPTION **Options)
{
    EFI_STATUS  Status;
    UINT16      i;
    UINTN       OrderSize;
    UINT16*     Order = NULL;

    Status = GetEfiVariable(
                L"BootOrder", &EfiVariableGuid, NULL, &OrderSize, (VOID**)&Order);

    if(!EFI_ERROR(Status))
    {
        UINT16 OptionIndex = 0;
        UINT16 OrderCount;
        SIMPLE_BOOT_OPTION *SimpleBootOptions;

        OrderCount = (UINT16) OrderSize / sizeof(OrderCount);
        SimpleBootOptions = (SIMPLE_BOOT_OPTION*) MallocZ(sizeof(SIMPLE_BOOT_OPTION) * OrderCount);
        if(!SimpleBootOptions) return 0;
        
        for (i = 0; i < OrderCount; i++)
        {
            EFI_DEVICE_PATH_PROTOCOL*   Dp;
            CHAR16                      BootStr[9];
            UINT32*                     OptionalData;
            UINTN                       DescriptionSize;
            UINTN                       OptionalDataSize;

            EFI_LOAD_OPTION*            NvramOption;
            UINTN                       NvramOptionSize;
            SIMPLE_BOOT_OPTION*         Option;
               

            NvramOption = NULL;
            Swprintf(BootStr,L"Boot%04X", Order[i]);
            Status = GetEfiVariable(
                       BootStr, &EfiVariableGuid, NULL, &NvramOptionSize, (VOID**)&NvramOption);
            if (EFI_ERROR(Status)) continue;
            TRACE((-1,"Get Boot Option Boot%04X\n", Order[i]));

            DescriptionSize = (Wcslen((CHAR16*)(NvramOption+1))+1)*sizeof(CHAR16);
            Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)(NvramOption+1)+DescriptionSize);

            OptionalData =  (UINT32*)( (UINT8*)Dp + NvramOption->FilePathListLength);
            OptionalDataSize =  (UINT8*)NvramOption + NvramOptionSize - (UINT8*)OptionalData;

            RemoveAmiMaskDevicePath(&Dp, NvramOption->FilePathListLength);

            Option = &SimpleBootOptions[OptionIndex++];
            Option->NvramOption = NvramOption;
            Option->Size = NvramOptionSize;
            Option->Dp = Dp;
            Option->Num = Order[i];

            if(OptionalDataSize >= sizeof(UINT32)
                && (ReadUnaligned32(OptionalData) == AMI_SIMPLE_BOOT_OPTION_SIGNATURE
                    || ReadUnaligned32(OptionalData) == AMI_GROUP_BOOT_OPTION_SIGNATURE) )
            {
                Option->Fw = TRUE;
            }
        }

        pBS->FreePool((VOID*)Order);

        if(OptionIndex)
        {
            *Options = SimpleBootOptions;
            return OptionIndex;
        }
    }

    return 0;
}

/**
    Remove boot option without uefi boot file in partition.

    @param  Options       Boot Options to check
    @param  OptionCount   Number of boot options
**/
VOID RemoveBootOptionWithoutUefiBootFileInPartition(SIMPLE_BOOT_OPTION *Options, UINTN OptionCount)
{
    if(Options && OptionCount)
    {
        UINTN i;

        for (i = 0; i < OptionCount; i++)
        {
            if(IsBootOptionWithoutBootFile(Options[i].Dp))
            {
                CHAR16  BootStr[9];

                //Clear variable Boot####
                Swprintf(BootStr,L"Boot%04X",Options[i].Num);
                pRS->SetVariable(
                    BootStr, &EfiVariableGuid,
                    BOOT_VARIABLE_ATTRIBUTES, 0, NULL);

                Options[i].Invalid = TRUE;

                TRACE((-1, "BootOptionNumber(%X) Deleted\n", Options[i].Num));
            }
        }
    }
}

/**
    if UEFI OS is existed, kill the duplicated UEFI OS boot options into one and move it to 
    one of their boot order position, and kill all UEFI OS boot options if deivces are disappeared
    in system.
*/
VOID AdjustEfiOsBootOrder(VOID)
{
    UINT16  BootIndex=0;
    BOOLEAN UpdateBootOrder=FALSE;
    UINTN i, j;

    SIMPLE_BOOT_OPTION *Options;
    UINT16             BootOrderCount;

    TRACE((-1,"EfiOsBootOrder.....\n"));

    BootOrderCount = GetSimpleBootOptions(&Options);
    if (!BootOrderCount) return;

    //
    //  Elinks before doing the adjust
    //
    for (i = 0; AdjustEfiOsBootOrderFunctions[i]; i++)
        AdjustEfiOsBootOrderFunctions[i](Options, BootOrderCount);

    for (i=0; i<BootOrderCount; i++)
    {
        CHAR16                      BootStr[9];
        SIMPLE_BOOT_OPTION*         SrcOpt;
        EFI_DEVICE_PATH_PROTOCOL*   SrcDp;
        
        SrcOpt = &Options[i];
        if( SrcOpt->Invalid ) continue;                   //(EIP138397)   
        SrcDp = SrcOpt->Dp;
        
        if ( SrcDp->Type == MEDIA_DEVICE_PATH
          &&  SrcDp->SubType == MEDIA_HARDDRIVE_DP )
        {
            EFI_HANDLE GptHandle;
            TRACE((-1,"EfiOsBootOrder.c :: BootOptionNumber(%X)\n", SrcOpt->Num));
            GptHandle = GetGptPartitionHandle(SrcDp);
            TRACE((-1,"EfiOsBootOrder.c :: GptHandle=%d\n", GptHandle));
	    
            if ( GptHandle != NULL)
            {
                // Only Non Fw Boot Option Kill Other Boot Option
                // This module check duplicate Boot Option exist or not when create boot option
                // So, ignore Fw Boot Option
                if(SrcOpt->Fw)
                    continue;
                
                for (j = 0 ; j< BootOrderCount ; j++)
                {
                    SIMPLE_BOOT_OPTION *DstOpt;

                    DstOpt = &Options[j];
                    if( DstOpt->Invalid || (i == j)) continue;

                    TRACE((-1,"EfiOsBootOrder.c :: Search BootOptionNumber(%x)\n", DstOpt->Num));
                    
#if KeepDuplicateNonFWBootOption
                    if(!DstOpt->Fw)
                        continue;
#endif
                    if ( CompareHddDevicePath(SrcDp, DstOpt->Dp) == EFI_SUCCESS )     //(EIP126686+)
                    {
                        UINTN  ReserveIndex;
                        UINTN  DeleteIndex; 

                        TRACE((-1,"EfiOsBootOrder.c :: Matched BootOptionNumber(%x)\n", DstOpt->Num));

                        if(!DefaultFwBootOption && !IsOsCreatedBootOption(SrcOpt->Num))
                        {
                            //
                            //  BootOrder[i] is not created by OS, but BootOrder[j]
                            //
                            ReserveIndex = j;
                            DeleteIndex = i;
                        }
                        else
                        {
                            ReserveIndex = i;
                            DeleteIndex = j;
                        }
                        //Delete EfiOsBootOptionNames boot option.												
                        //Clear variable Boot####
                        Swprintf(BootStr,L"Boot%04X",Options[DeleteIndex].Num);
                        pRS->SetVariable(
                            BootStr, &EfiVariableGuid,
                            BOOT_VARIABLE_ATTRIBUTES, 0, NULL);

                        //BootOrder_Flag[DeleteIndex] = 1;
                        Options[DeleteIndex].Invalid = TRUE;
                        UpdateBootOrder = TRUE;

#if FIXED_BOOT_ORDER_SUPPORT
                        // Handle Fixed Boot Order Priority issue.
                        {
                            UINTN               DevOrderSize = 0;
                            UEFI_DEVICE_ORDER   *DevOrder = NULL, *DevOrder2 = NULL;
                            EFI_GUID            gFixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
                            UINTN               k = 0;
                            UINT32              Attrib = 0;
                            EFI_STATUS          Status;
                            
                            Status = GetEfiVariable(L"UefiDevOrder", &gFixedBootOrderGuid, &Attrib, &DevOrderSize, &DevOrder);
                            if(!EFI_ERROR(Status))
                            {
                                DevOrder2 = DevOrder;
                                
                                for (DevOrder = DevOrder2
                                    ; (UINT8*)DevOrder < (UINT8*)DevOrder2 + DevOrderSize
                                    ; DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
                                {
                                    for (k = 0; k < DEVORDER_COUNT(DevOrder); k++)
                                    {
                                        TRACE((-1,"DevOrder->Device[%x] %x,Options[%x] = %x \n",k,DevOrder->Device[k], DeleteIndex, Options[DeleteIndex].Num));
                                        if(DevOrder->Device[k] == Options[DeleteIndex].Num)
                                        {
                                            DevOrder->Device[k] = Options[ReserveIndex].Num;
                                            TRACE((-1,"Options[%x].Num %x \n", ReserveIndex, Options[ReserveIndex].Num));
                                        }
                                    }
                                }
                                Status = pRS->SetVariable(L"UefiDevOrder", &gFixedBootOrderGuid, Attrib, DevOrderSize, DevOrder2);
                                pBS->FreePool(DevOrder2); 
                            }
                        }
#endif         
                    }

                } //for (j=0; j<BootOrderSize/sizeof(UINT16); j++)
            } //if ( GptHandle != NULL)
            else
             //GPT HDD NOT FOUND, Remove This BootOrder
            {
                if ( (((HARDDRIVE_DEVICE_PATH*)SrcDp)->MBRType == MBR_TYPE_EFI_PARTITION_TABLE_HEADER) &&
                        SrcOpt->Fw)
                {
                    Swprintf(BootStr,L"Boot%04X",SrcOpt->Num);
                    //Clear variable Boot####
                    pRS->SetVariable(
                       BootStr, &EfiVariableGuid,
                       BOOT_VARIABLE_ATTRIBUTES, 0, NULL);

                    UpdateBootOrder = TRUE;
                    //BootOrder_Flag[i] = 1;
                    Options[i].Invalid = TRUE;
                }
            }
        }
    }

    if ( UpdateBootOrder )
    {
        UINT16* NewBootOrder;

        TRACE((-1,"EfiOsBootOrder.c :: Update BootOrder, Dump New BootOrder \n" ));

        NewBootOrder = MallocZ( BootOrderCount * sizeof(UINT16));
        if(NewBootOrder)
        {
            for(i = 0 ; i < BootOrderCount ; i++)
            {
                if(Options[i].Invalid)
                    continue;
                
                TRACE((-1,"%x ",Options[i].Num));
                NewBootOrder[BootIndex++] = Options[i].Num;
            }
            TRACE((-1,"\n "));
            
            pRS->SetVariable(
                L"BootOrder", &EfiVariableGuid,
                BOOT_VARIABLE_ATTRIBUTES, BootIndex * sizeof(UINT16), NewBootOrder);

            pBS->FreePool(NewBootOrder);
        }
    }

    for(i = 0; i < BootOrderCount; i++)
        pBS->FreePool(Options[i].NvramOption);
    pBS->FreePool(Options);
}

#if (CSM_SUPPORT == 1) && (RemoveLegacyGptHddDevice == 1)
/**
    Determine whether boot device is a UEFI HDD(GPT format).

    @param  Device  Boot device to be checked.

    @retval  TRUE   Boot Device is a UEFI HDD.
    @retval  FALSE  Boot Device is not a UEFI HDD.
*/
BOOLEAN RemoveLegacyGptHdd(BOOT_DEVICE *Device) {
    EFI_BLOCK_IO_PROTOCOL *BlkIo;
    EFI_STATUS Status;
    UINT8 *Buffer = NULL;
    UINTN index;
    PARTITION_ENTRY *pEntries;

    if (   Device->DeviceHandle == INVALID_HANDLE
            || Device->BbsEntry == NULL
       ) return FALSE;

    if ( Device->BbsEntry->DeviceType != BBS_HARDDISK ) return FALSE;

    Status=pBS->HandleProtocol(
               Device->DeviceHandle, &gEfiBlockIoProtocolGuid, &BlkIo
           );

    if (EFI_ERROR(Status) || BlkIo->Media->RemovableMedia) return FALSE;	//USB device?

    Status = pBS->AllocatePool( EfiBootServicesData, BlkIo->Media->BlockSize, &Buffer );
    if ( Buffer == NULL ) return FALSE;

    // read the first sector
    BlkIo->ReadBlocks ( BlkIo,
                        BlkIo->Media->MediaId,
                        0,
                        BlkIo->Media->BlockSize,
                        (VOID*)Buffer);

    if(Buffer[0x1fe]==(UINT8)0x55 && Buffer[0x1ff]==(UINT8)0xaa)	//MBR Signature
    {
        pEntries=(PARTITION_ENTRY *)(Buffer+0x1be);

        for (index=0; index<4; index++)
        {
            if ( pEntries[index].PartitionType == 0xee) 	//Check GPT Partition?
            {
                pBS->FreePool( Buffer );
                return TRUE;			//Set Can't Boot.
            }
        } //for(index=0;index<4;index++)
    }//if(Buffer[0x1fe] == 0x55 && Buffer[0x1ff] == 0xaa)

    pBS->FreePool( Buffer );
    return FALSE;
}
#endif

/**
    Check whether the target boot option by NameMap.

    @param  Option    Boot option to check
    @param  NameMap   Used to check the boot option

    @retval  TRUE   Is the target boot option
    @retval  FALSE  Isn't the target boot option
**/
BOOLEAN IsMatchedBootOptionByNameMap(BOOT_OPTION* Option, NAME_MAP* NameMap)
{
    if(Option && NameMap)
    {
        // check option name length, if OptionName != FileNameMaps, don't skip boot option
        if ( Wcslen(Option->Description) == Wcslen(NameMap->BootOptionName))
        {
            // check option string
            if ( !MemCmp( Option->Description, NameMap->BootOptionName, StrSize(NameMap->BootOptionName)) )
            {
                // check option device path.
                EFI_DEVICE_PATH_PROTOCOL *Dp = Option->FilePathList ;
                BOOLEAN HardDriveFlag=FALSE, MediaFilePathFlag=FALSE ;
                while(!isEndNode(Dp))
                {
                    if (Dp->Type == MEDIA_DEVICE_PATH && Dp->SubType == MEDIA_HARDDRIVE_DP)
                        HardDriveFlag = TRUE ;
                    if (Dp->Type == MEDIA_DEVICE_PATH && Dp->SubType == MEDIA_FILEPATH_DP)
                        MediaFilePathFlag = TRUE ;
                    Dp = NEXT_NODE(Dp) ;
                }
                
                if (HardDriveFlag && MediaFilePathFlag)
                {
                    TRACE((-1,"%S don't normalize.\n", Option->Description )) ;
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

/**
    Check whether uefi os boot option by boot file(ex.BootX64.efi).

    @param  Option   Boot option to check

    @retval  TRUE   Match the map
    @retval  FALSE  Doesn't match the map
**/
BOOLEAN CheckUefiOsBootOptionByUefiBootFile(BOOT_OPTION* Option)
{
    if(Option)
    {
        STATIC NAME_MAP NameMap = {EFI_BOOT_FILE_NAME, NAME_OF_UEFI_OS};

        if(IsMatchedBootOptionByNameMap(Option, &NameMap))
            return TRUE;
    }

    return FALSE;
}

/**
    Check whether uefi os boot option by map (FILE_NAME_MAPS).

    @param  Option   Boot option to check

    @retval  TRUE   Match the map
    @retval  FALSE  Doesn't match the map
**/
BOOLEAN CheckUefiOsBootOptionByMap(BOOT_OPTION* Option)
{
    if(Option)
    {
        UINTN i;

        for(i = 0; FILE_NAME_MAPS[i].FilePath; i++)
        {
            if(IsMatchedBootOptionByNameMap(Option, &FILE_NAME_MAPS[i]))
                return TRUE;
        }
    }

    return FALSE;
}

/**
    Check whether specified uefi os boot option.

    @param  Option   boot option to check

    @retval  TRUE    Is specified uefi os boot option
    @retval  FALSE   Isn't specified uefi os boot option
*/
BOOLEAN IsSpecifiedUefiOsBootOption(BOOT_OPTION *Option)
{
    if(Option && !IsLegacyBootOption(Option))
    {
        UINTN i;

        for (i = 0; CheckUefiOsBootOptionFunctions[i]; i++)
        {
            if(CheckUefiOsBootOptionFunctions[i](Option))
                return TRUE;
        }
    }

    return FALSE;
}

/**
    If normalization is enabled, regenerates all the description strings
    and/or file path lists.
    This function override BDS kernel, because Uefi Os Boot Option can't
    be normalize file path and names.
*/
VOID EfiOsName_NormalizeBootOptions(){
    DLINK *Link;
    BOOT_OPTION *Option;

    // Normalize boot options 
    //(regenerate the description string and the file path list)
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){

        if (   !Option->FwBootOption || !IsBootOptionWithDevice(Option)
            || Option->BootOptionNumber == INVALID_BOOT_OPTION_NUMBER
        ) continue;
        
        // Skip Uefi Hdd Os boot option
        if (IsSpecifiedUefiOsBootOption(Option)) continue ;
        
        if (NormalizeBootOptionDevicePath){
            EFI_DEVICE_PATH_PROTOCOL *OldFilePathList = Option->FilePathList;
            UINTN OldFilePathListLength = Option->FilePathListLength;
            Option->FilePathList = NULL;
            Option->FilePathListLength = 0;
            BuildBootOptionFilePath(Option);
            if (Option->FilePathList == NULL){
                Option->FilePathList = OldFilePathList;
                Option->FilePathListLength = OldFilePathListLength;
            }else if (OldFilePathList != NULL){
                pBS->FreePool(OldFilePathList);
            }
        }
        if (NormalizeBootOptionName){
            CHAR16 *OldDescription = Option->Description;
            Option->Description = NULL;
            ConstructBootOptionName(Option);
            if (Option->Description == NULL) 
                Option->Description = OldDescription;
            else if (OldDescription != NULL) 
                pBS->FreePool(OldDescription);
        }
    }
}

/**
    Adjust new UEFI OS boot option priority.
    Follow the policy NEW_UEFI_OS_OPTION_ORDER_POLICY to change priority with new
    UEFI OS boot option.
    
*/
VOID
AdjustNewUefiOsOptionPriority()
{
    EFI_STATUS  Status;
    DLINK       *Link;   
    BOOT_OPTION *Option;
    UINTN     Size;
    UINTN     x = 0;
    UINT32    Priority = 0;
    UINT8     NewOptionPolicy;
    EFI_GUID  EfiOsBootOptionNamesGuid = EFI_OS_BOOT_OPTION_NAMES_GUID;
    
    TRACE((-1,"EfiOsBootOrder.c : Update new Uefi OS option priority\n")) ;

    // if no new EFI OS boot option added, just return. 
    if (!NewEfiOsOptionDpListCount) return ;
	
	Size = sizeof (UINT8);
    Status = pRS->GetVariable (
              L"NewOptionPolicy",
              &EfiOsBootOptionNamesGuid,
              NULL,
              &Size,
              (VOID *) &NewOptionPolicy
              );
    
    if(EFI_ERROR(Status))
    {
        pBS->FreePool(NewEfiOsOptionDpList) ;
        return;
    }
        
    if (NewOptionPolicy == 0)
    {
        pBS->FreePool(NewEfiOsOptionDpList) ;
        return;
    }
    else if (NewOptionPolicy == 1)
    {
        Priority = LOWEST_BOOT_OPTION_PRIORITY ;
        FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
        {
            if (Option->Priority < Priority)
                Priority = Option->Priority ;
            Option->Priority += (UINT32)NewEfiOsOptionDpListCount ;
        }   
    }
    else if (NewOptionPolicy == 2)
    {
        Priority = 0 ;
        FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
        {
            if (Option->Priority > Priority)
                Priority = Option->Priority + 1  ;
        }
    }
    
    for ( x=0 ; x<NewEfiOsOptionDpListCount ; x++ )
    {
        EFI_DEVICE_PATH_PROTOCOL *Dp;
        
        Dp = *(NewEfiOsOptionDpList+x);
        FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
        {
            if (Option->FilePathList == Dp)
                Option->Priority = Priority++ ;
        }
    } 
    pBS->FreePool(NewEfiOsOptionDpList) ;
}

/**
    Save the boot order to check later, 
    then we can know which one is created by OS.
*/
VOID SaveEfiOsBootOrder()
{
    EFI_STATUS Status;
    UINTN      BootOrderSize = 0;
    UINT16*    BootOrder = NULL;

    Status = GetEfiVariable(
                L"BootOrder", 
                &EfiVariableGuid, 
                NULL, 
                &BootOrderSize, 
                &BootOrder);

    if(!EFI_ERROR(Status))
    {
        EFI_GUID  EfiOsBootOptionNamesGuid = EFI_OS_BOOT_OPTION_NAMES_GUID;

        pRS->SetVariable(
            L"EfiOsBootOrder", 
            &EfiOsBootOptionNamesGuid,
            EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
            BootOrderSize, 
            (VOID*)BootOrder);

        pBS->FreePool(BootOrder);
    }
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
