//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file UsbBus.c
    USB Bus driver implementation

**/

#include "AmiDef.h"
#include "UsbDef.h"
#include "Uhcd.h"

#include "UsbDef.h"
#include "UsbBus.h"
#include "ComponentName.h"

//#pragma warning(disable: 4244)

EFI_DRIVER_BINDING_PROTOCOL gUSBBusDriverBinding = {
    UsbBusSupported,
    UsbBusStart,
    UsbBusStop,
    USB_DRIVER_VERSION,
    NULL,
    NULL
};

extern USB_GLOBAL_DATA*     gUsbData;

TREENODE_T  UsbRootRoot = {0,};
TREENODE_T* gUsbRootRoot = &UsbRootRoot;
EFI_EVENT   gEvUsbEnumTimer=0;
int         gCounterUsbEnumTimer=0;
int         gBustreeLock = 0;

/**
    This function returns a pointer to USB device from UsbIo.

**/

USBDEV_T* UsbIo2Dev(EFI_USB_IO_PROTOCOL* UsbIo)
{
    return (USBDEV_T*)((char*)UsbIo - (UINTN)&((USBDEV_T*)0)->io );
}


/**
    Predicate for searching host controller node in the tree
    by bHcNumber

**/

int HcByIndex(VOID* n, VOID* d)
{
    USBBUS_HC_T* HcNode = (USBBUS_HC_T*)n;

    return n && (HcNode->type == NodeHC)
         && (HcNode->hc_data->bHCNumber == *(UINT8*)d );
}


/**
    Predicate for searching host controller node in the tree
    by EFI controller handle

**/

int HcByHandle(VOID* n, VOID* d)
{
    USBBUS_HC_T* HcNode = (USBBUS_HC_T*)n;
    return (HcNode->type == NodeHC) && (HcNode->hc_data->Controller == *(EFI_HANDLE*)d );
}


/**
    Predicate for searching device node in the tree
    by index of the DEV_INFO structure in the aDevInfoTable
    array of USB data

**/

int DevByIndex(VOID* n, VOID* d)
{
    USBDEV_T* Dev = (USBDEV_T*)n;
    return (Dev->type == NodeDevice) && (Dev->dev_info ==
        gUsbData->aDevInfoTable  + *(UINT8*)d );
}


/**
    Predicate for searching device node in the tree
    by USB address of the device

**/

int DevGrpByAddr(VOID* n, VOID* d)
{
    USBDEV_T* Dev = (USBDEV_T*)n;
    return (Dev->type == NodeDevice || Dev->type ==  NodeGroup) &&
        (Dev->dev_info->bDeviceAddress == *(UINT8*)d );
}

/**
    Predicate for searching device node in the tree
    by parent hub port of the device, interface and LUN

**/

int DevGrpByPortIf(VOID* n, VOID* d)
{
    USBDEV_T* Dev = (USBDEV_T*)n;
    return (Dev->type == NodeDevice || Dev->type ==  NodeGroup) &&
        (Dev->dev_info->bHubPortNumber == ((DEV_INFO*)d)->bHubPortNumber ) &&
        (Dev->dev_info->bInterfaceNum == ((DEV_INFO*)d)->bInterfaceNum ) &&
        (Dev->dev_info->bLUN == ((DEV_INFO*)d)->bLUN );
}


/**
    Predicate for searching device node in the tree
    by comparing pointers to the DEV_INFO structure

**/

int DevByInfo(VOID* n, VOID* d )
{
    USBDEV_T* Dev = (USBDEV_T*)n;
    return (Dev->type == NodeDevice) && (Dev->dev_info == (DEV_INFO*)d );
}


/**
    Predicate for searching device node in the tree
    by USB address and interface number of the device

**/

int DevByAdrIf(VOID* n, VOID* d)
{
    USBDEV_T* Dev = (USBDEV_T*)n;

    return ((Dev->type == NodeDevice) &&
        (Dev->dev_info->bDeviceAddress == ((DEV_INFO*)d)->bDeviceAddress ) &&
        (Dev->dev_info->bInterfaceNum == ((DEV_INFO*)d)->bInterfaceNum )&&
        (Dev->dev_info->bLUN == ((DEV_INFO*)d)->bLUN));
}


/**
    Retrieve DEVGROUP_T that is parent of
    the specified USB device in the USB Bus tree

    @param Device for which the parent is requested

**/

DEVGROUP_T* UsbDevGetGroup(USBDEV_T* Dev)
{
    return (DEVGROUP_T*)Dev->node.parent->data;
}

/**
    Search gUsbData for information about Device linked to an EFI handle

    @param 
        Controller - Device handle

**/

DEV_INFO* 
FindDevStruc(
    EFI_HANDLE Controller
)
{
    UINTN   i;
 
    for (i = 0; i < MAX_DEVICES; i++) {
        if ((gUsbData->aDevInfoTable[i].Flag & DEV_INFO_VALIDPRESENT) 
            != DEV_INFO_VALIDPRESENT) {
            continue;
        }
        if (Controller == (EFI_HANDLE)*(UINTN*)gUsbData->aDevInfoTable[i].Handle) {
            return &gUsbData->aDevInfoTable[i];
        }
    }
    
    return NULL;
}


/**
    Builds a new path appending a USB segment

    @retval Pointer to a callee allocated memory buffer

**/

EFI_DEVICE_PATH_PROTOCOL*
DpAddUsbSegment(
    EFI_DEVICE_PATH_PROTOCOL*   Dp,
    UINT8                       bHubPortNumber,
    UINT8                       bInterfaceNum
)
{
    USB_DEVICE_PATH DpNewSegment = {0,};
    DpNewSegment.Header.Type = MESSAGING_DEVICE_PATH;
    DpNewSegment.Header.SubType = MSG_USB_DP;
    SET_NODE_LENGTH(&DpNewSegment.Header, sizeof(DpNewSegment));

    DpNewSegment.InterfaceNumber = bInterfaceNum;
    DpNewSegment.ParentPortNumber = bHubPortNumber;
    return  EfiAppendDevicePathNode(Dp,(EFI_DEVICE_PATH_PROTOCOL*)&DpNewSegment);
}


/**
    Builds a new path appending a LUN node


    @retval Pointer to a callee allocated memory buffer

**/

EFI_DEVICE_PATH_PROTOCOL*
DpAddLun(
    EFI_DEVICE_PATH_PROTOCOL* Dp,
    UINT8   Lun
)
{
    DEVICE_LOGICAL_UNIT_DEVICE_PATH DpNewSegment = {0,};

    DpNewSegment.Header.Type = MESSAGING_DEVICE_PATH;
    DpNewSegment.Header.SubType = MSG_DEVICE_LOGICAL_UNIT_DP;
    SET_NODE_LENGTH(&DpNewSegment.Header, sizeof(DpNewSegment));
    DpNewSegment.Lun = Lun;
    return  EfiAppendDevicePathNode(Dp,(EFI_DEVICE_PATH_PROTOCOL*)&DpNewSegment);
}


/**
    This function executes a get descriptor command to the
    given USB device and endpoint

    @param dev         a pointer to USBDEV_T corresponding to the device
    @param fpBuffer    Buffer to be used for the transfer
    @param wLength     Size of the requested descriptor
    @param bDescType   Requested descriptor type
    @param bDescIndex  Descriptor index
    @param wLangIndex  LangIndex

    @retval Pointer to memory buffer containing the descriptor, NULL on error

**/

UINT8*
ReadUsbDescriptor( USBDEV_T* Dev,
    UINT8*  Buffer,
    UINT16  Length,
    UINT8   DescType,
    UINT8   DescIndex,
    UINT16  LangIndex )
{
    HC_STRUC*   HcStruc = Dev->hc_info;
    DEV_INFO*   DevInfo = Dev->dev_info;
    UINT8       GetDescIteration;
    UINT16      Reg;
    UINT16      Status;

    for (GetDescIteration = 0; GetDescIteration < 3; GetDescIteration++) {
        Reg = (UINT16)((DescType << 8) + DescIndex);
        Status = UsbSmiControlTransfer(
                        HcStruc,
                        DevInfo,
                        (UINT16)USB_RQ_GET_DESCRIPTOR,
                        (UINT16)LangIndex,
                        Reg,
                        Buffer,
                        Length);
        if (Status) {
            return Buffer;
        }
        if (gUsbData->dLastCommandStatusExtended & USB_TRNSFR_TIMEOUT) {
            break;
        }
        pBS->Stall(10 * 1000);
    }

    return NULL;
}


/**
    Allocates memory necessary to hold complete descriptor
    and returns the descriptor there

    @param dev         a pointer to USBDEV_T corresponding to the device
    @param type        Requested descriptor type
    @param index       Descriptor index
    @param langindex   LangIndex

    @retval Pointer to memory buffer containing the descriptor, NULL on error

**/

USB_DESCRIPTOR_T*
GetUsbDescriptor(
    USBDEV_T*   Dev,
    UINT8       Type,
    UINT8       Index,
    UINT16      LangIndex)
{
    UINT8               Buffer[0xFF] = {0};
    USB_DESCRIPTOR_T    *Desc;
    UINT8*              DescBuffer;

    DescBuffer= ReadUsbDescriptor(Dev, Buffer, sizeof(Buffer), Type, Index, LangIndex);

    if ((DescBuffer == NULL) || 
        (((USB_DESCRIPTOR_T*)Buffer)->DescriptorType != Type) ||
        (((USB_DESCRIPTOR_T*)Buffer)->Length == 0)) {
        return NULL;
    }
    gBS->AllocatePool(EfiBootServicesData, ((USB_DESCRIPTOR_T*)Buffer)->Length, &Desc);

    EfiCopyMem(Desc, Buffer, ((USB_DESCRIPTOR_T*)Buffer)->Length);

    return Desc;

/*
	EfiZeroMem(Desc, ((USB_DESCRIPTOR_T*)Buffer)->Length);
    DescBuffer = ReadUsbDescriptor(Dev, (UINT8*)Desc,
        ((USB_DESCRIPTOR_T*)Buffer)->Length, Type, Index, LangIndex);
    //ASSERT(Desc->DescriptorType == Type);     //(EIP60640-)
    if (DescBuffer == NULL){
        gBS->FreePool(Desc);
        return NULL;
    }
    //
    //Decriptor Type cannot be 0, this case means that Get Descriptor cmd timed out
    //
    if (Desc->DescriptorType == 0) {
        gBS->FreePool(Desc);
        return NULL;
    } else {
      return Desc;
    }
*/
}


/**
    Delocates memory that was used by the descriptor

**/

VOID FreeConfigDesc( VOID* Desc )
{
    if (Desc != 0) gBS->FreePool(Desc);
}


/**
    Returns pointer to the next descriptor for the pack of
    USB descriptors located in continues memory segment
    - result of reading CONFIG_DESCRIPTOR
    @note  
              Uses TotalLength of the CONFIG_DESCRIPTOR and Length
              field of each USB descriptor found inside the pack

**/

BOOLEAN
NextDescriptor(
    IN EFI_USB_CONFIG_DESCRIPTOR* Desc,
    IN OUT UINTN* Offset
)
{
    if( Desc == NULL || *Offset >= Desc->TotalLength ) return FALSE;
    if( ((EFI_USB_CONFIG_DESCRIPTOR*)((char*)Desc+*Offset))->Length == 0) return FALSE;
    *Offset += ((EFI_USB_CONFIG_DESCRIPTOR*)((char*)Desc+*Offset))->Length;
    if( *Offset >= Desc->TotalLength ) return FALSE;

    return TRUE;
}


/**
    Returns a pointer to the memory containing CONFIG_DESCRIPTOR
    reported by the USB device

**/

EFI_USB_CONFIG_DESCRIPTOR*
DevGroupConfigDesc( DEVGROUP_T* Grp ){
    return Grp->f_DevDesc && (Grp->active_config != -1)? 
        Grp->configs[Grp->active_config]:NULL;
}

EFI_USB_CONFIG_DESCRIPTOR*
UsbDevConfigDesc( USBDEV_T* Dev ){
    return DevGroupConfigDesc( UsbDevGetGroup(Dev));
}

#if USB_IAD_PROTOCOL_SUPPORT

/**
    Parses through the configuration descriptor searching for IAD
    If found, IAD details are added to the DEVGROUP_T structure

**/
EFI_STATUS
FindIadDetails(DEVGROUP_T* Grp)
{
    EFI_USB_CONFIG_DESCRIPTOR *CfgDesc;
    EFI_USB_INTERFACE_DESCRIPTOR  *TmpDesc;
    UINTN Offset;
    IAD_DETAILS *IadDetails;
    UINT8 i;
    EFI_STATUS Status;

    // Count IADs
    for(i = 0; i < Grp->config_count; i++)
    {
        CfgDesc = Grp->configs[i];
        for(Offset = 0; NextDescriptor(CfgDesc, &Offset);)
        {
            TmpDesc = (EFI_USB_INTERFACE_DESCRIPTOR *)((UINT8*)CfgDesc + Offset);
            if (TmpDesc->DescriptorType == DESC_TYPE_IAD)
            {
                Grp->iad_count++;
            }
        }
    }
    if (Grp->iad_count == 0) return EFI_NOT_FOUND;    // No IADs found
    
    // Allocate iad_details
    Status = gBS->AllocatePool (EfiBootServicesData,
        Grp->iad_count*sizeof(IAD_DETAILS), (VOID *)&Grp->iad_details);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    
    EfiZeroMem(Grp->iad_details, Grp->iad_count*sizeof(IAD_DETAILS));
    
    // Fill in the IAD details
    IadDetails = Grp->iad_details;
    for(i = 0; i < Grp->config_count; i++)
    {
        CfgDesc = Grp->configs[i];
        for(Offset = 0; NextDescriptor(CfgDesc, &Offset);)
        {
            TmpDesc = (EFI_USB_INTERFACE_DESCRIPTOR *)((UINT8*)CfgDesc + Offset);
            if (TmpDesc->DescriptorType != DESC_TYPE_IAD) continue;

            IadDetails->ConfigIndex = i;
            IadDetails->Descriptor = (AMI_USB_INTERFACE_ASSOCIATION_DESCRIPTOR*)TmpDesc;

            // Allocate and zero IadDetails->Data
            Status = gBS->AllocatePool (EfiBootServicesData,
                IadDetails->Descriptor->bInterfaceCount*sizeof(USBIAD_DATA_T),
                (VOID *)&IadDetails->Data);
            ASSERT_EFI_ERROR(Status);
            if (EFI_ERROR(Status)) return Status;
            
            EfiZeroMem(IadDetails->Data, IadDetails->Descriptor->bInterfaceCount*sizeof(USBIAD_DATA_T));
            
            IadDetails++;
        }
    }
    USB_DEBUG(DEBUG_INFO, 3, "IAD details: totally %d IADs:\n", Grp->iad_count);
    IadDetails = Grp->iad_details;
    for (i = 0; i < Grp->iad_count; i++, IadDetails++)
    {
        USB_DEBUG(DEBUG_INFO, 3, " IAD#%d. ConfigIndex %d, IAD Descriptor Ofs %x-%x=%x (%d), ifFrom %d ifCount %d\n",
                i, IadDetails->ConfigIndex,
                Grp->configs[IadDetails->ConfigIndex],
                IadDetails->Descriptor,
                (UINT8*)IadDetails->Descriptor - (UINT8*)Grp->configs[IadDetails->ConfigIndex],
                (UINT8*)IadDetails->Descriptor - (UINT8*)Grp->configs[IadDetails->ConfigIndex],                
                IadDetails->Descriptor->bFirstInterface, IadDetails->Descriptor->bInterfaceCount);
    }
    return EFI_SUCCESS;
}

#endif


/**
    Reads DEVICE and CONFIG descriptors for each
    configuration available in the device. Marks
    the index of the buffer containing CONFIG descriptor
    for active configurations currently selected in
    USB device

**/

VOID
UsbDevLoadAllDescritors(
    DEVGROUP_T* Dev
)
{
    UINT8 i;
	EFI_STATUS Status;

    //ASSERT( Dev->f_DevDesc == 0 );
    Dev->configs = NULL;
    //
    // Device descriptor
    //
    gBS->CopyMem((UINT8*)&Dev->dev_desc, (UINT8*)&Dev->dev_info->DevDesc, sizeof(Dev->dev_desc));
    
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USB Bus: dev descr: ");
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "cls:%x;subcls:%x;proto:%x;vndr:%x;id:%x\n",
                Dev->dev_desc.DeviceClass,
                Dev->dev_desc.DeviceSubClass,
                Dev->dev_desc.DeviceProtocol,
                Dev->dev_desc.IdVendor,
                Dev->dev_desc.IdProduct);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "\t\tstr1:%x,str2:%x,str3:%x\n",
                Dev->dev_desc.StrManufacturer,
                Dev->dev_desc.StrProduct,
                Dev->dev_desc.StrSerialNumber );

    Dev->f_DevDesc = TRUE;

    //
    // Config descriptor
    //
    Dev->config_count = Dev->dev_desc.NumConfigurations;
//  dev->configs = (EFI_USB_CONFIG_DESCRIPTOR**)MallocZ(dev->config_count*sizeof(EFI_USB_CONFIG_DESCRIPTOR*));
    Status = gBS->AllocatePool (EfiBootServicesData,
        Dev->config_count*sizeof(EFI_USB_CONFIG_DESCRIPTOR*), (VOID *)&Dev->configs);
	ASSERT_EFI_ERROR(Status);
    EfiZeroMem(Dev->configs, Dev->config_count*sizeof(EFI_USB_CONFIG_DESCRIPTOR*));

    Dev->active_config = -1;
    for(i=0; i<Dev->config_count; ++i){
        //read each configuration
        //first failed read will terminate loop

        //Optimization: allloc&read MAX size first
        //      and read second time only if total length is greater
        //Read 1 : get total length
        EFI_USB_CONFIG_DESCRIPTOR tmp = {0,0,};
        UINT8* p = ReadUsbDescriptor((USBDEV_T*)Dev,(UINT8*)&tmp,
            sizeof(tmp),DESC_TYPE_CONFIG,i, 0 );
        //ASSERT(tmp.DescriptorType == DESC_TYPE_CONFIG);
        //ASSERT(tmp.TotalLength >= sizeof(tmp));
        //
        //Addressing timeouts caused by device errors - empty DESC structure will be returned
        //
        if( (p == NULL) || (tmp.DescriptorType == 0) ) {
          break;
        }

        //Read 2: Actual content
//      dev->configs[i] = MallocZ(tmp.TotalLength);
        Status = gBS->AllocatePool (EfiBootServicesData, tmp.TotalLength, &Dev->configs[i]);
		ASSERT_EFI_ERROR(Status);
        EfiZeroMem(Dev->configs[i], tmp.TotalLength);

        p = ReadUsbDescriptor((USBDEV_T*)Dev, (UINT8*)Dev->configs[i],
            tmp.TotalLength, DESC_TYPE_CONFIG, i, 0);
        //ASSERT(Dev->configs[i]->DescriptorType == DESC_TYPE_CONFIG);  //(EIP60640-)
        //
        //Addressing timeouts caused by device errors - empty DESC structure will be returned
        //
        if( (p == NULL) || (Dev->configs[i]->DescriptorType == 0) ){
            gBS->FreePool(Dev->configs[i]);
            Dev->configs[i] = 0;
            break;
        }

        if (Dev->configs[i]->Length == 0) {
            USB_DEBUG(DEBUG_WARN, 3, "The length of the configuration descriptor is 0.\n");
            Dev->configs[i]->Length = 0x09;
        }
        //config Desc is here

        //Active Config
        if( Dev->configs[i]->ConfigurationValue == Dev->dev_info->bConfigNum ){
            Dev->active_config = i;
        }
    }
}

EFI_STATUS
EnableEndponts(
    USBDEV_T    *Device,
    UINT8       AltSetting
)
{
    EFI_PHYSICAL_ADDRESS Address = 0;
    EFI_STATUS Status;
    USB_DEBUG(DEBUG_INFO, 3, "EnableEndpoints: HcType is %x\n", Device->hc_info->bHCType);
    if (Device->hc_info->bHCType != USB_HC_XHCI) return EFI_UNSUPPORTED;
    if (Device->dev_info->IsocDetails.XferRing == 0)
    {
        /*
         Allocate transfer ring's BASE address and store it in Device->dev_info->IsocDetails.XferRing
         Note that after UsbSmiEnableEndpoints call this field will change and will point to BASE's
         parent structure. That is why while freeing we will use *Device->dev_info->IsocDetails.XferRing
        */ 
        Status = gBS->AllocatePages(AllocateAnyPages, EfiBootServicesData, EFI_SIZE_TO_PAGES(0x20000), &Address);
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR(Status)) return Status;
        Device->dev_info->IsocDetails.XferRing = (UINTN)Address;
    }
    
    UsbSmiEnableEndpoints(
        Device->hc_info, Device->dev_info, (UINT8*)Device->descIF[AltSetting]
    );

    return EFI_SUCCESS;
}
        
/**
    Locates information about each endpoint inside the
    descriptors pack loaded with CONFIG descriptor

**/

EFI_STATUS
UsbDevLoadEndpoints(
    USBDEV_T* Dev
)
{
    EFI_USB_INTERFACE_DESCRIPTOR  *TmpDesc;
    EFI_USB_ENDPOINT_DESCRIPTOR  *EpDesc;
    EFI_USB_CONFIG_DESCRIPTOR  *CfgDesc;
    int j;
    UINTN   Offset;
    DEVGROUP_T *Grp = UsbDevGetGroup(Dev);
    UINT8 AltIfSetting = Dev->dev_info->bAltSettingNum;
    
    ASSERT(AltIfSetting < USB_MAX_ALT_IF);
    
    if ((Dev->LoadedAltIfMap & (1 << AltIfSetting)) != 0) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: LoadEndpoints have been loaded\n");
        return EFI_SUCCESS;
    }
    
    Dev->first_endpoint[AltIfSetting] = Grp->endpoint_count;
    Dev->end_endpoint[AltIfSetting] = Grp->endpoint_count;

    CfgDesc = UsbDevConfigDesc(Dev);
    
    if (CfgDesc == NULL) {
        USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_USBBUS, "USBBUS: LoadEndpoints aborted - no Configuration descriptor\n");
        return EFI_NOT_FOUND;
    }
    
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS,
        "\tScanning config desc: %x(type:%x;len:%x;val:%x;total:%x)\n",
        CfgDesc,CfgDesc->DescriptorType,CfgDesc->Length,
        CfgDesc->ConfigurationValue,CfgDesc->TotalLength );

    //
    // Search interface descriptor
    //
    for (Offset = 0; NextDescriptor(CfgDesc, &Offset);)
    {
        TmpDesc = (EFI_USB_INTERFACE_DESCRIPTOR*)((UINT8*)CfgDesc + Offset);
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS,
            "\t\tdesc: %x(type:%x;len:%x;if:%x;aif:%x)\n",
            TmpDesc,TmpDesc->DescriptorType, TmpDesc->Length,
            TmpDesc->InterfaceNumber,  TmpDesc->AlternateSetting);
        
        if (TmpDesc->DescriptorType == DESC_TYPE_INTERFACE &&
            TmpDesc->InterfaceNumber == Dev->dev_info->bInterfaceNum &&
            TmpDesc->AlternateSetting == AltIfSetting)
        {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "\t...IF found.\n" );

            Dev->descIF[AltIfSetting] = TmpDesc;

            ASSERT(TmpDesc->NumEndpoints < COUNTOF(Grp->endpoints));
            for(j = 0; j < TmpDesc->NumEndpoints && NextDescriptor(CfgDesc, &Offset);)
            {
                EpDesc = (EFI_USB_ENDPOINT_DESCRIPTOR*)((UINT8*)CfgDesc + Offset);
                if (EpDesc->DescriptorType == DESC_TYPE_ENDPOINT)
                {
                    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS,
                        "\t\tendpoint desc: %x index:%x;type:%x;len:%x;addr:%x;pcksz:%x;attr:%x,interval:%x)\n",
                        EpDesc, Grp->endpoint_count, EpDesc->DescriptorType,EpDesc->Length,
                        EpDesc->EndpointAddress, EpDesc->MaxPacketSize, EpDesc->Attributes, EpDesc->Interval);
                    Grp->endpoints[Grp->endpoint_count++] = EpDesc;
                    Grp->a2endpoint[COMPRESS_EP_ADR(EpDesc->EndpointAddress)] = EpDesc;
                    j++;
                }
                if (EpDesc->DescriptorType == DESC_TYPE_INTERFACE){
                    //Oops, we stepped into another interface
                    break;
                }
            }
            Dev->end_endpoint[AltIfSetting] = Grp->endpoint_count;
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS,
                    "\t\tinterface endpoints: first:%x; last:%x, grp.endpoint_count:%x\n",
                        Dev->first_endpoint[AltIfSetting], Dev->end_endpoint[AltIfSetting], Grp->endpoint_count);
            Dev->LoadedAltIfMap |= (1 << AltIfSetting);
            if ((AltIfSetting != 0) && (Dev->dev_info->bBaseClass != BASE_CLASS_HUB)) {
                USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "\t\t...Enable endpoints for ALT_IF_SETTING %d...\n", AltIfSetting);
                EnableEndponts(Dev, AltIfSetting);
            }
            break;
        }
    }
    return EFI_SUCCESS;
}


/**
    Retrieves information about a max packet size
    for the specified endpoint of the device

**/

UINT16
GetMaxPacket(
    UINT8       Endpoint,
    USBDEV_T*   Dev
)
{
    DEVGROUP_T *Grp = UsbDevGetGroup(Dev);
    EFI_USB_ENDPOINT_DESCRIPTOR* Desc = Grp->a2endpoint[COMPRESS_EP_ADR(Endpoint)];
    if (Desc == 0) return 0;
    return Desc->MaxPacketSize;
}


/**
    Retrieves endpoint descriptor pointer

**/

EFI_USB_ENDPOINT_DESCRIPTOR*
GetEndpointDesc(
    UINT8       Endpoint,
    USBDEV_T*   Dev
)
{
    DEVGROUP_T *Grp = UsbDevGetGroup(Dev);
    EFI_USB_ENDPOINT_DESCRIPTOR* Desc;

    if (((Endpoint & 0x7F)==0) || ((Endpoint & 0x7F) > 0xF))
        return NULL;
    Desc = Grp->a2endpoint[COMPRESS_EP_ADR(Endpoint)];

    return Desc;
}


/**
**/

EFI_STATUS
EFIAPI
UsbIoControlTransfer(
    IN EFI_USB_IO_PROTOCOL      *UsbIo,
    IN EFI_USB_DEVICE_REQUEST   *Request,
    IN EFI_USB_DATA_DIRECTION   Direction,
    IN UINT32                   Timeout,
    IN OUT VOID                 *Data,
    IN UINTN                    DataLength,
    OUT UINT32                  *UsbStatus
)
{
    USBDEV_T*   Dev;
    EFI_STATUS  Status;
    EFI_TPL     OldTpl;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);
    
    if (Request == NULL || UsbStatus == NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }
    
    if (Direction > EfiUsbNoData) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if (!(Dev->dev_info->Flag & DEV_INFO_DEV_PRESENT)) {
        gBS->RestoreTPL(OldTpl);
        return EFI_DEVICE_ERROR;
    }

    if ((Request->Request == (USB_RQ_SET_INTERFACE >> 8)) &&
         (Request->RequestType == (USB_RQ_SET_INTERFACE & 0x0F) ) &&
         (Request->Index == Dev->dev_info->bInterfaceNum)) {
        Dev->dev_info->bAltSettingNum = (UINT8)Request->Value;
        Status = UsbDevLoadEndpoints(Dev);
        if (EFI_ERROR(Status)) {
            gBS->RestoreTPL(OldTpl);
            return EFI_DEVICE_ERROR;
        }
    }

    Status = Dev->hc->ControlTransfer(
        Dev->hc, Dev->dev_info->bDeviceAddress, GetSpeed(Dev),
        (UINTN)Dev->dev_info->wEndp0MaxPacket,				//(EIP81612)
        Request, Direction, Data, &DataLength, Timeout, NULL, UsbStatus);

    gBS->RestoreTPL(OldTpl);

    return Status;
}


/**
**/

EFI_STATUS
EFIAPI
UsbIoBulkTransfer(
  IN EFI_USB_IO_PROTOCOL    *UsbIo,
  IN UINT8                  Endpoint,
  IN OUT VOID               *Data,
  IN OUT UINTN              *DataLength,
  IN UINTN                  Timeout,
  OUT UINT32                *UsbStatus
)
{
    USBDEV_T*   Dev;
    UINT16      MaxPacket;
	UINT8       ToggleBit = (Endpoint & 0xF) - 1;
	UINT16		*DataSync;
    UINT8       Toggle;
    EFI_STATUS  Status;
    EFI_USB_ENDPOINT_DESCRIPTOR* EpDesc;
    DEV_INFO *  DevInfoToDataSync;
    EFI_TPL     OldTpl;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);
    EpDesc = GetEndpointDesc(Endpoint, Dev);
 
    if (((Endpoint & 0x7F)==0) || ((Endpoint & 0x7F) > 0xF)) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if (Data == NULL || DataLength == NULL || UsbStatus == NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if (EpDesc == NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }
    
    MaxPacket = EpDesc->MaxPacketSize;
    ASSERT(Dev->dev_info);
    ASSERT(Dev->hc);

    if (UsbStatus == NULL || MaxPacket == 0) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if ((EpDesc->Attributes & EP_DESC_FLAG_TYPE_BITS) != EP_DESC_FLAG_TYPE_BULK) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if (!(Dev->dev_info->Flag & DEV_INFO_DEV_PRESENT)) {
        gBS->RestoreTPL(OldTpl);
        return EFI_DEVICE_ERROR;
    }

    if (Dev->dev_info->fpLUN0DevInfoPtr) {
        DevInfoToDataSync = Dev->dev_info->fpLUN0DevInfoPtr;
    }else {
        DevInfoToDataSync = Dev->dev_info;
    }

    if (Endpoint & 0x80) {
        DataSync = &DevInfoToDataSync->wDataInSync;
    }else {
        DataSync = &DevInfoToDataSync->wDataOutSync;
    }

    GETBIT(*DataSync, Toggle, ToggleBit);

    Status = Dev->hc->BulkTransfer(
        Dev->hc, Dev->dev_info->bDeviceAddress, Endpoint, GetSpeed(Dev),
        MaxPacket, 1, &Data, DataLength, &Toggle, Timeout, NULL, UsbStatus);

    SETBIT(*DataSync, Toggle, ToggleBit);

    gBS->RestoreTPL(OldTpl);

    return Status;
}


/**
**/

EFI_STATUS
EFIAPI
UsbIoAsyncInterruptTransfer(
    IN EFI_USB_IO_PROTOCOL  *UsbIo,
    IN UINT8                Endpoint,
    IN BOOLEAN              IsNewTransfer,
    IN UINTN                PollingInterval,
    IN UINTN                DataLength,
    IN EFI_ASYNC_USB_TRANSFER_CALLBACK InterruptCallback,
    IN VOID                 *Context
)
{
    USBDEV_T*   Dev;
    UINT8       Toggle;
	UINT8		ToggleBit = (Endpoint & 0xF) - 1;
	UINT16		*DataSync;
    EFI_STATUS  Status;
    EFI_USB_ENDPOINT_DESCRIPTOR* EpDesc;
    DEV_INFO    *DevInfoToDataSync;
    EFI_TPL     OldTpl;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);
    EpDesc = GetEndpointDesc(Endpoint, Dev);

	// Check whether Endpoint is valid
    if (EpDesc == NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if ((EpDesc->Attributes & EP_DESC_FLAG_TYPE_BITS) != EP_DESC_FLAG_TYPE_INT) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

	if (IsNewTransfer && (PollingInterval < 1 || PollingInterval > 255)) {
        gBS->RestoreTPL(OldTpl);
		return EFI_INVALID_PARAMETER;
	}

    if (IsNewTransfer) {
        if (!(Dev->dev_info->Flag & DEV_INFO_DEV_PRESENT)) {
            gBS->RestoreTPL(OldTpl);
            return EFI_DEVICE_ERROR;
        }
    }

    if (Dev->dev_info->fpLUN0DevInfoPtr) {
        DevInfoToDataSync = Dev->dev_info->fpLUN0DevInfoPtr;
    } else {
        DevInfoToDataSync = Dev->dev_info;
    }

    if (Endpoint & 0x80) {
        DataSync = &DevInfoToDataSync->wDataInSync;
    } else {
        DataSync = &DevInfoToDataSync->wDataOutSync;
    }

    GETBIT(*DataSync, Toggle, ToggleBit);

    Status = Dev->hc->AsyncInterruptTransfer(
        Dev->hc, Dev->dev_info->bDeviceAddress, Endpoint,
        GetSpeed(Dev), EpDesc->MaxPacketSize, IsNewTransfer,
        &Toggle, PollingInterval, DataLength, NULL,
        InterruptCallback, Context);

    SETBIT(*DataSync, Toggle, ToggleBit);

	if (!EFI_ERROR(Status)) {
		Dev->async_endpoint = IsNewTransfer ? Endpoint : 0;
	}
    
    gBS->RestoreTPL(OldTpl);
    
    return Status;
}


/**
**/

EFI_STATUS
EFIAPI
UsbIoSyncInterruptTransfer(
    IN     EFI_USB_IO_PROTOCOL  *UsbIo,
    IN     UINT8                Endpoint,
    IN OUT VOID                 *Data,
    IN OUT UINTN                *DataLength,
    IN     UINTN                Timeout,
    OUT    UINT32               *UsbStatus
)
{
    USBDEV_T*   Dev;
    UINT8       Toggle;
	UINT8		ToggleBit = (Endpoint & 0xF) - 1;
	UINT16		*DataSync;
    EFI_STATUS  Status;
    EFI_USB_ENDPOINT_DESCRIPTOR* EpDesc;
    DEV_INFO *  DevInfoToDataSync;
    EFI_TPL     OldTpl;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);
    EpDesc = GetEndpointDesc(Endpoint, Dev);

	ASSERT(Dev->dev_info);
	ASSERT(Dev->hc);

	// Check whether Endpoint is valid
    if (EpDesc == NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if ((EpDesc->Attributes & EP_DESC_FLAG_TYPE_BITS) != EP_DESC_FLAG_TYPE_INT) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if (Data == NULL || DataLength == NULL || UsbStatus == NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if (!(Dev->dev_info->Flag & DEV_INFO_DEV_PRESENT)) {
        gBS->RestoreTPL(OldTpl);
        return EFI_DEVICE_ERROR;
    }
										//(EIP84215+)>
    if (Dev->dev_info->fpLUN0DevInfoPtr) {
        DevInfoToDataSync = Dev->dev_info->fpLUN0DevInfoPtr;
    } else {
        DevInfoToDataSync = Dev->dev_info;
    }

    if (Endpoint & 0x80) {
        DataSync = &DevInfoToDataSync->wDataInSync;
    } else {
        DataSync = &DevInfoToDataSync->wDataOutSync;
    }
										//<(EIP84215+)
    GETBIT(*DataSync, Toggle, ToggleBit);
    Status = Dev->hc->SyncInterruptTransfer(
        Dev->hc, Dev->dev_info->bDeviceAddress, Endpoint,
        GetSpeed(Dev), EpDesc->MaxPacketSize,
        Data, DataLength, &Toggle, Timeout, NULL, UsbStatus);
    
    SETBIT(*DataSync, Toggle, ToggleBit);

    gBS->RestoreTPL(OldTpl);

    return Status;
}


/**
**/

EFI_STATUS
EFIAPI
UsbIoIsochronousTransfer(
    IN EFI_USB_IO_PROTOCOL  *UsbIo,
    IN     UINT8            Endpoint,
    IN OUT VOID             *Data,
    IN     UINTN            DataLength,
    OUT    UINT32           *UsbStatus
)
{
    USBDEV_T*   Dev;
    EFI_USB_ENDPOINT_DESCRIPTOR* EpDesc;
    EFI_TPL     OldTpl;
    EFI_STATUS  Status;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);
    EpDesc = GetEndpointDesc(Endpoint, Dev);
    
    ASSERT( Dev->dev_info );
    ASSERT( Dev->hc );

    if (!(Dev->dev_info->Flag & DEV_INFO_DEV_PRESENT)) {
        USB_DEBUG(DEBUG_ERROR, 3, "UsbIoIsochronousTransfer ERROR: device is not present\n");
        gBS->RestoreTPL(OldTpl);
        return EFI_DEVICE_ERROR;
    }
    
    if (EpDesc == NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }
    
    if ((EpDesc->Attributes & EP_DESC_FLAG_TYPE_BITS) != EP_DESC_FLAG_TYPE_ISOC) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if ( Endpoint != Dev->dev_info->IsocDetails.Endpoint ) {
        USB_DEBUG(DEBUG_INFO, 3, "UsbIoIsochronousTransfer ERROR: Endpoint %x whereas dev_info->IsocEndpoint %x\n",
                Endpoint, Dev->dev_info->IsocDetails.Endpoint);
        gBS->RestoreTPL(OldTpl);
        return EFI_UNSUPPORTED;
    }

    Status = Dev->hc->IsochronousTransfer(
        Dev->hc, Dev->dev_info->bDeviceAddress, Endpoint, GetSpeed(Dev),
        Dev->dev_info->IsocDetails.EpMaxPkt, 1, &Data, DataLength, NULL, UsbStatus);

    gBS->RestoreTPL(OldTpl);

    return Status;
}


/**
**/

EFI_STATUS
EFIAPI
UsbIoAsyncIsochronousTransfer(
    IN EFI_USB_IO_PROTOCOL  *UsbIo,
    IN UINT8                Endpoint,
    IN OUT VOID             *Data,
    IN     UINTN            DataLength,
    IN EFI_ASYNC_USB_TRANSFER_CALLBACK  IsochronousCallback,
    IN VOID                 *Context
)
{
    USBDEV_T* Dev;
    EFI_USB_ENDPOINT_DESCRIPTOR* EpDesc;
    EFI_TPL     OldTpl;
    EFI_STATUS  Status;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);
    EpDesc = GetEndpointDesc(Endpoint, Dev);
    
    ASSERT( Dev->dev_info );
    ASSERT( Dev->hc );

    if (!(Dev->dev_info->Flag & DEV_INFO_DEV_PRESENT)) {
        USB_DEBUG(DEBUG_ERROR, 3, "UsbIoAsyncIsochronousTransfer ERROR: device is not present\n");
        gBS->RestoreTPL(OldTpl);
        return EFI_DEVICE_ERROR;
    }
    
    if (EpDesc == NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }
    
    if ((EpDesc->Attributes & EP_DESC_FLAG_TYPE_BITS) != EP_DESC_FLAG_TYPE_ISOC) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if (Endpoint != Dev->dev_info->IsocDetails.Endpoint) {
        USB_DEBUG(DEBUG_ERROR, 3, "UsbIoAsyncIsochronousTransfer ERROR: Endpoint %x whereas dev_info->IsocEndpoint %x\n",
                Endpoint, Dev->dev_info->IsocDetails.Endpoint);
        gBS->RestoreTPL(OldTpl);
        return EFI_UNSUPPORTED;
    }

    Status = Dev->hc->AsyncIsochronousTransfer(
        Dev->hc, Dev->dev_info->bDeviceAddress, Endpoint, GetSpeed(Dev),
        Dev->dev_info->IsocDetails.EpMaxPkt, 1, &Data, DataLength, NULL, IsochronousCallback, Context);

    gBS->RestoreTPL(OldTpl);

    return Status;
}


/**
**/

EFI_STATUS
EFIAPI
UsbIoPortReset(
    IN EFI_USB_IO_PROTOCOL  *UsbIo
)
{
    USBDEV_T* Dev;
	DEVGROUP_T* Grp;
	UINT8	Status;
	UINT8	i;
    EFI_TPL OldTpl;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);
    Grp = UsbDevGetGroup(Dev);

    if (Dev->dev_info->bDeviceType == BIOS_DEV_TYPE_HUB) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

	Status = UsbResetAndReconfigDev(Dev->hc_info, Dev->dev_info);
	if (Status != USB_SUCCESS) {
        gBS->RestoreTPL(OldTpl);
		return EFI_DEVICE_ERROR;
	}

	if( UsbSmiGetDescriptor(Dev->hc_info, Dev->dev_info, (UINT8*)&Grp->dev_desc,
        sizeof(Grp->dev_desc), DESC_TYPE_DEVICE, 0) == 0 ){
        gBS->RestoreTPL(OldTpl);
        return EFI_DEVICE_ERROR;
    }

	for (i = 0; i < Grp->dev_desc.NumConfigurations; i++) {
		if (ReadUsbDescriptor(Dev, (UINT8*)Grp->configs[i], 
			Grp->configs[i]->TotalLength, DESC_TYPE_CONFIG, i, 0) == 0) {
			gBS->RestoreTPL(OldTpl);
			return EFI_DEVICE_ERROR;
		}
	}

    gBS->RestoreTPL(OldTpl);

    return EFI_SUCCESS;
}


/**
    Retrieves the USB Device Descriptor.

    @param 
        UsbIo       A pointer to the EFI_USB_IO_PROTOCOL instance. Type
        EFI_USB_IO_PROTOCOL is defined in Section 14.2.5.
        Desc        A pointer to the caller allocated USB Device Descriptor.

**/

EFI_STATUS
EFIAPI
UsbIoGetDeviceDescriptor(
    IN EFI_USB_IO_PROTOCOL          *UsbIo,
    OUT EFI_USB_DEVICE_DESCRIPTOR   *Desc
)
{
    USBDEV_T* Dev;
    DEVGROUP_T* Grp;
    EFI_TPL OldTpl;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);
    Grp = UsbDevGetGroup(Dev);
    
    ASSERT( Dev->dev_info );
    ASSERT( Dev->hc );
    
    if (Desc == NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if (Grp->f_DevDesc) {
        *Desc = Grp->dev_desc;
        gBS->RestoreTPL(OldTpl);
        return EFI_SUCCESS;
    } else {
        gBS->RestoreTPL(OldTpl);
        return EFI_NOT_FOUND;
    }
}


/**
**/

EFI_STATUS
EFIAPI
UsbIoGetConfigDescriptor(
    IN EFI_USB_IO_PROTOCOL          *UsbIo,
    OUT EFI_USB_CONFIG_DESCRIPTOR   *Desc
)
{
    USBDEV_T*   Dev;
    DEVGROUP_T* Grp;
    EFI_TPL OldTpl;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);
    Grp = UsbDevGetGroup(Dev);
    
    ASSERT( Dev->dev_info );
    ASSERT( Dev->hc );
    
    if (Desc == NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if (Grp->configs && Grp->active_config != -1 &&
        Grp->configs[Grp->active_config]) {
        *Desc = *Grp->configs[Grp->active_config];
        gBS->RestoreTPL(OldTpl);
        return EFI_SUCCESS;
    } else {
        gBS->RestoreTPL(OldTpl);
        return EFI_NOT_FOUND;
    }
}


/**
**/

EFI_STATUS
EFIAPI
UsbIoGetInterfaceDescriptor(
    IN EFI_USB_IO_PROTOCOL              *UsbIo,
    OUT EFI_USB_INTERFACE_DESCRIPTOR    *Desc
)
{
    USBDEV_T*   Dev;
    EFI_TPL     OldTpl;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);

    ASSERT( Dev->dev_info );
    ASSERT( Dev->hc );
    if (Desc == NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    if (Dev->descIF[Dev->dev_info->bAltSettingNum] == NULL) {
        USB_DEBUG(DEBUG_ERROR, 3, "IF NOT FOUND on UsbIo %x Dev %x\n", UsbIo, Dev);
        gBS->RestoreTPL(OldTpl);
        return EFI_NOT_FOUND;
    }

    *Desc = *Dev->descIF[Dev->dev_info->bAltSettingNum];

    gBS->RestoreTPL(OldTpl);

    return EFI_SUCCESS;
}


/**
**/

EFI_STATUS
EFIAPI
UsbIoGetEndpointDescriptor(
    IN EFI_USB_IO_PROTOCOL  *UsbIo,
    IN  UINT8               EndpointIndex,
    OUT EFI_USB_ENDPOINT_DESCRIPTOR *Desc
)
{
    USBDEV_T* Dev;
    DEVGROUP_T *Grp;
    EFI_USB_ENDPOINT_DESCRIPTOR* DescCopy;
    UINT8       FirstEp;
    UINT8       LastEp;
    EFI_TPL     OldTpl;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);

    if (Desc == NULL || EndpointIndex >= 0x10) {
        gBS->RestoreTPL(OldTpl);
        return EFI_INVALID_PARAMETER;
    }

    ASSERT(Dev->dev_info);
    ASSERT(Dev->hc);
    
    FirstEp = Dev->first_endpoint[Dev->dev_info->bAltSettingNum];
    LastEp = Dev->end_endpoint[Dev->dev_info->bAltSettingNum];

    USB_DEBUG(DEBUG_INFO, 3, "Get Endpoint desc: devaddr: 0x%x; Endpoint: 0x%x\n",
            Dev->dev_info->bDeviceAddress, EndpointIndex);
    USB_DEBUG(DEBUG_INFO, 3, "\tfirst Endpoint: 0x%x; last Endpoint: 0x%x\n",
            FirstEp, LastEp - 1);

    if (FirstEp + EndpointIndex >= LastEp) {
        gBS->RestoreTPL(OldTpl);
        return EFI_NOT_FOUND;
    }

    ASSERT(FirstEp + EndpointIndex < 0x20);

    Grp = UsbDevGetGroup(Dev);
    DescCopy = Grp->endpoints[FirstEp + EndpointIndex];

    ASSERT( DescCopy );

    if (DescCopy==NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_NOT_FOUND;
    }

    USB_DEBUG(DEBUG_INFO, 3, "\tendp addr: 0x%x; attr: 0x%x; MaxPacket: 0x%x\n",
            DescCopy->EndpointAddress,
            DescCopy->Attributes, DescCopy->MaxPacketSize );

    *Desc = *DescCopy;

    gBS->RestoreTPL(OldTpl);

    return EFI_SUCCESS;
}


/**
**/
EFI_STATUS
EFIAPI
UsbIoGetStringDescriptor(
    IN EFI_USB_IO_PROTOCOL  *UsbIo,
    IN  UINT16              LangId,
    IN  UINT8               StringId,
    OUT CHAR16              **String
)
{
    USBDEV_T*                       Dev;
    EFI_USB_STRING_DESCRIPTOR*      StrDesc = NULL;
    UINT16                          Index;
    UINT16                          *LangIdTable;
    DEVGROUP_T*                     Grp;
    EFI_STATUS                      Status = EFI_NOT_FOUND;
    EFI_TPL                         OldTpl;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);

    ASSERT(Dev->dev_info);
    ASSERT(Dev->hc);

    Grp = UsbDevGetGroup(Dev);

    if (StringId == 0 || Grp->lang_table == NULL) {
        //reserved ids: stringid==0 => langid table descripto;
        gBS->RestoreTPL(OldTpl);
        return Status;
    }
    // search langid_table
    LangIdTable = Grp->lang_table->langID;
    for (Index = 0; Index < Grp->lang_table->len ; Index++) {
        if (LangId == LangIdTable[Index]) {
            break;
        }
    }
    
    if (Index == Grp->lang_table->len) {
        gBS->RestoreTPL(OldTpl);
        return Status;
    }
    
    if (LangId == USB_US_LAND_ID) {
        if (StringId == Grp->dev_desc.StrSerialNumber) {
            StrDesc = Grp->SerialNumberStrDesc;
        } else if (StringId == Grp->dev_desc.StrProduct) {
            StrDesc = Grp->ProductStrDesc;
        } else if (StringId == Grp->dev_desc.StrManufacturer) {
            StrDesc = Grp->ManufacturerStrDesc;
        }
    }
    //
    // Get string descriptor: variable size
    //
    if (StrDesc == NULL) {
        StrDesc = (EFI_USB_STRING_DESCRIPTOR*)GetUsbDescriptor(Dev, DESC_TYPE_STRING,
			        StringId, LangId);
    }
    
    if (StrDesc == NULL) {
        gBS->RestoreTPL(OldTpl);
        return Status;
    }

    if (StrDesc->Length > 2 && StrDesc->DescriptorType == DESC_TYPE_STRING) {
        //
        // Allocate memory for string & copy
        //
        if (String != NULL) {
            gBS->AllocatePool(EfiBootServicesData, StrDesc->Length, String);
            EfiZeroMem(*String, StrDesc->Length);
            EfiCopyMem(*String, StrDesc->String, StrDesc->Length -2);
        }
        Status = EFI_SUCCESS;
    }
    if ((StrDesc != Grp->SerialNumberStrDesc) && (StrDesc != Grp->ProductStrDesc) &&
        (StrDesc != Grp->ManufacturerStrDesc)) {
        gBS->FreePool(StrDesc);
    }

    gBS->RestoreTPL(OldTpl);
    
    return Status;
}


/**
**/

EFI_STATUS
EFIAPI
UsbIoGetSupportedLanguages(
    IN EFI_USB_IO_PROTOCOL  *UsbIo,
    OUT UINT16              **LangIdTable,
    OUT UINT16              *TableSize )
{
    USBDEV_T*       Dev;
    DEVGROUP_T*     Grp;
    EFI_TPL         OldTpl;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

    Dev = UsbIo2Dev(UsbIo);

    ASSERT( Dev->dev_info );
    ASSERT( Dev->hc );
    
    Grp = UsbDevGetGroup(Dev);
    
    if (LangIdTable == NULL || TableSize == NULL) {
        gBS->RestoreTPL(OldTpl);
        return EFI_SUCCESS;
    }
    
    if (Grp->lang_table == NULL) {
        *LangIdTable  = 0;
        *TableSize = 0;
    } else {
        *LangIdTable = Grp->lang_table->langID;
        *TableSize = Grp->lang_table->len * 2;
    }

    gBS->RestoreTPL(OldTpl);
    return EFI_SUCCESS;
}


/**
    loads STRING descriptor that corresponds to
    the name of the USB device

**/

CHAR16*
LoadName(
    USBDEV_T* Dev
)
{
    EFI_USB_INTERFACE_DESCRIPTOR DescIf = {0,};
    EFI_USB_DEVICE_DESCRIPTOR DescDev = {0,};
    DEVGROUP_T* Grp = UsbDevGetGroup(Dev);
    CHAR16* StrIf = 0;
    CHAR16* StrProduct = 0;
//  CHAR16* StrManufact=0;
    CHAR16 Lang;
    CHAR16  *MassStorageName;
    UINT8   *p;
    UINT8   i;

    for (i = 0; i < 64; i++) {
        if (Dev->dev_info->DevNameString[i] != 0) {
            break;
        }
    }

    if (i != 64) {
        gBS->AllocatePool (EfiBootServicesData, 128, &MassStorageName);
        EfiZeroMem(MassStorageName, 128);
        for (p = (UINT8*)&Dev->dev_info->DevNameString, i=0; i<64; i++) {
            if (p[i] == 0) break;
            MassStorageName[i] = (CHAR16)p[i];
        }
        return MassStorageName;
    }

    if( Grp->lang_table == 0 || Grp->lang_table->len == 0 ) return 0;

    Lang = Grp->lang_table->langID[0];

    UsbIoGetInterfaceDescriptor(&Dev->io,&DescIf);
    if( DescIf.Interface && !EFI_ERROR(
        UsbIoGetStringDescriptor(&Dev->io, Lang,
        DescIf.Interface, &StrIf )))
        return StrIf;

    UsbIoGetDeviceDescriptor(&Dev->io, &DescDev);
    if( DescDev.StrProduct && !EFI_ERROR(
        UsbIoGetStringDescriptor(&Dev->io, Lang,
        DescDev.StrProduct, &StrProduct )))
        return StrProduct;

    return NULL;
}

#if USB_IAD_PROTOCOL_SUPPORT

/**
**/
IAD_DETAILS*
IadProt2Iad(
    AMI_USB_IAD_PROTOCOL* IadProtocol
)
{
    return (IAD_DETAILS*)((char*)IadProtocol - (UINTN)&((IAD_DETAILS*)0)->Iad);
}


/**
**/

EFI_STATUS
EFIAPI
UsbIadGetIad(
    IN AMI_USB_IAD_PROTOCOL                       *This,
    OUT AMI_USB_INTERFACE_ASSOCIATION_DESCRIPTOR  *IaDesc
)
{
    IAD_DETAILS*   Iad = IadProt2Iad(This);
    *IaDesc = *Iad->Descriptor;

    return EFI_SUCCESS;
}


/**
	AMI_USB_IAD_PROTOCOL interface function. Returns USB device handle for a given IAD
**/

EFI_STATUS
EFIAPI
UsbIadGetUsbHandle(
    IN AMI_USB_IAD_PROTOCOL   *This,
    IN UINT8                  UsbIoIndex,
    OUT EFI_HANDLE            *UsbIoHandle
)
{
    IAD_DETAILS*   Iad = IadProt2Iad(This);

    if (UsbIoIndex >= Iad->Descriptor->bInterfaceCount)
    {
        return EFI_INVALID_PARAMETER;
    }
    ASSERT(Iad->Data[UsbIoIndex].Handle);

    *UsbIoHandle = Iad->Data[UsbIoIndex].Handle;

    return EFI_SUCCESS;
}


/**
	AMI_USB_IAD_PROTOCOL interface function. Returns the pointer for the
	device class specific data corresponding to the given UsbIo.
**/

EFI_STATUS
EFIAPI
UsbIadGetInterfaceData(
    IN AMI_USB_IAD_PROTOCOL   *This,
    IN UINT8                  UsbIoIndex,
    IN OUT UINTN              *DataSize,
    OUT VOID                  *Data
)
{
    EFI_STATUS  Status;
    IAD_DETAILS *Iad = IadProt2Iad(This);
    USBDEV_T    *Dev;
    EFI_USB_IO_PROTOCOL *UsbIo;

    if (UsbIoIndex >= Iad->Descriptor->bInterfaceCount)
    {
        return EFI_INVALID_PARAMETER;
    }
    
    ASSERT(Iad->Data[UsbIoIndex].DataSize);
    
    if (*DataSize < Iad->Data[UsbIoIndex].DataSize)
    {
        *DataSize = Iad->Data[UsbIoIndex].DataSize;
        return EFI_BUFFER_TOO_SMALL;
    }

    Status = gBS->HandleProtocol(Iad->Data[UsbIoIndex].Handle, &gEfiUsbIoProtocolGuid, &UsbIo);
    ASSERT_EFI_ERROR(Status);
    
    Dev = UsbIo2Dev(UsbIo);
    EfiCopyMem(Data, Dev->descIF[0], Iad->Data[UsbIoIndex].DataSize);
    
    *DataSize = Iad->Data[UsbIoIndex].DataSize;

    return EFI_SUCCESS;
}


/**
   This function checks if a given USBDEV_T is a part of any IAD listed 
   in the given DEVGROUP_T. If so, check for the completion of IAD. If
   IAD is fully populated, install AMI_USB_IAD_PROTOCOL.
   Note: this function called after UsbIo protocol is installed and
   USBDEV_T has a valid UsbIo handle.
**/

EFI_STATUS ProcessIad(DEVGROUP_T* Grp, USBDEV_T* Dev)
{    
    UINT8 i;
    UINT8 j;
    IAD_DETAILS *IadDetails;
    UINT8 Interface;
    UINT8 IfFrom;
    UINT8 IfTo;

    USB_DEBUG(DEBUG_INFO, 3,"ProcessIad: iad_count %d\n", Grp->iad_count);

    if (Grp->iad_count == 0) return EFI_NOT_FOUND;
    
    IadDetails = Grp->iad_details;
    for (i = 0; i < Grp->iad_count; i++, IadDetails++)
    {
        EFI_USB_CONFIG_DESCRIPTOR *CfgDesc;

        CfgDesc = Grp->configs[IadDetails->ConfigIndex];

        //USB_DEBUG(DEBUG_INFO, 3,"ProcessIad[%d]: config %d %d\n", i, IadDetails->ConfigIndex, Dev->dev_info->bConfigNum);
        if (CfgDesc->ConfigurationValue != Dev->dev_info->bConfigNum) continue;
        //USB_DEBUG(DEBUG_INFO, 3,"ProcessIad[%d]: handle %x\n", i, IadDetails->IadHandle);
        if (IadDetails->IadHandle != NULL) continue;

        // Configuration number match, check for interface number
        Interface = Dev->descIF[0]->InterfaceNumber;
        IfFrom = Grp->iad_details[i].Descriptor->bFirstInterface;
        IfTo = IfFrom + Grp->iad_details[i].Descriptor->bInterfaceCount - 1;

        USB_DEBUG(DEBUG_INFO, 3,"ProcessIad[%d]: intrf %d (%d...%d)\n", i, Interface, IfFrom, IfTo);

        if (Interface >= IfFrom && Interface <= IfTo)
        {
            // Interface found, update Grp->iad_details->Data
            UINT8 IfIndex = Interface - IfFrom;
            USBIAD_DATA_T* Data = &IadDetails->Data[IfIndex];
            UINTN IfOffset;
            UINTN Offset;
            EFI_USB_INTERFACE_DESCRIPTOR *TmpDesc;

            ASSERT(Data->Handle == NULL);
            if (Data->Handle != NULL) return EFI_DEVICE_ERROR;   // Duplicate interface found

            Data->Handle = Dev->handle;

            // Calculate this interface data size
            IfOffset = (UINTN)Dev->descIF[0] - (UINTN)Grp->configs[IadDetails->ConfigIndex];
            Offset = IfOffset;

            while (NextDescriptor(CfgDesc, &Offset))
            {
                TmpDesc = (EFI_USB_INTERFACE_DESCRIPTOR *)((UINT8*)CfgDesc + Offset);
                if (TmpDesc->DescriptorType == DESC_TYPE_INTERFACE &&
                        TmpDesc->AlternateSetting == 0) break;
                if (TmpDesc->DescriptorType == DESC_TYPE_IAD) break;
            }

            Data->DataSize = Offset - IfOffset;
            
            // Check if all UsbIo is found for this IAD. If so, install IadIoProtocol
            for (j = 0; j < Grp->iad_details[i].Descriptor->bInterfaceCount; j++)
            {
                if (IadDetails->Data[j].Handle == NULL) break;
            }
            if (j == Grp->iad_details[i].Descriptor->bInterfaceCount)
            {
                EFI_STATUS Status;
                
                IadDetails->Iad.GetIad = UsbIadGetIad;
                IadDetails->Iad.GetInterfaceData = UsbIadGetInterfaceData;
                IadDetails->Iad.GetUsbHandle = UsbIadGetUsbHandle;
                Status = gBS->InstallMultipleProtocolInterfaces(
                        &IadDetails->IadHandle,
                        &gAmiUsbIadProtocolGuid, &IadDetails->Iad,
                        NULL);
                ASSERT_EFI_ERROR(Status);
                USB_DEBUG(DEBUG_INFO, 3, "ProcessIad: Install IadIo protocol: %r\n", Status);

                Status = gBS->ConnectController(IadDetails->IadHandle,NULL,NULL,TRUE);

                break;
            }
        }
    }
    if (i == Grp->iad_count) return EFI_NOT_FOUND;
    
    return EFI_SUCCESS;
}

#endif


/**
    Adds a device to the tree; creates an EFI handle for the
    usb device; installs USB_IO and DEVICEPATH protocols
    on a new device handle; connects a new device to
    EFI device drivers

**/

TREENODE_T*
UsbDevHubNode(
    TREENODE_T  *HcNode,
    DEV_INFO    *DevInfo
)
{
//    int i;
    TREENODE_T *HubNode=0;
    TREENODE_T *HubGrpNode;

    HubGrpNode = TreeSearchDeep(HcNode->child, DevGrpByAddr, &DevInfo->bHubDeviceNumber );
    if (HubGrpNode != NULL){
        HubNode = HubGrpNode->child;
        ASSERT(HubNode);
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: parent Hub found: %x\n", HubNode );
        return HubNode;
    }
/*
    for( i=0;i<COUNTOF(gUsbData->aDevInfoTable) && (HubNode==0);i++){
        if((gUsbData->aDevInfoTable[i].Flag & (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT))
            != (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT))
            continue;
        if( gUsbData->aDevInfoTable[i].bHCNumber == DevInfo->bHCNumber &&
            gUsbData->aDevInfoTable[i].bDeviceAddress == DevInfo->bHubDeviceNumber )
        {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: UsbDevHubNode:  valid hub info [%d]: %x\n",i, gUsbData->aDevInfoTable +i );
            InstallDevice( gUsbData->aDevInfoTable +i );
            HubNode = TreeSearchDeep(HcNode->child, DevByInfo, gUsbData->aDevInfoTable +i );
            ASSERT(HubNode);
        }
    }
*/
    return HubNode;
}

/*
VOID PrintData(VOID *Data)
{
    USBBUS_HC_T *hc;
    DEVGROUP_T  *grp;
    USBDEV_T    *dev;

    if (Data == NULL) return;

    switch (*((int*)Data)) {
        case NodeHC:
            hc = (USBBUS_HC_T*)Data;
            USB_DEBUG(DEBUG_INFO, 3, "\tHC #%d, type %x pci %x\n",
                hc->hc_data->bHCNumber, hc->hc_data->bHCType, hc->hc_data->wBusDevFuncNum);
            break;

        case NodeDevice:
            dev = (USBDEV_T*)Data;
            USB_DEBUG(DEBUG_INFO, 3, "\tDEV devinfo %x, #%d, type %d, VID%x DID%x\n",
                dev->dev_info, dev->dev_info->bDeviceAddress, dev->dev_info->bDeviceType, dev->dev_info->wVendorId, dev->dev_info->wDeviceId);
            break;
        case NodeGroup:
            grp = (DEVGROUP_T*)Data;
            USB_DEBUG(DEBUG_INFO, 3, "\tGRP devinfo %x, number of endpoints %d\n", grp->dev_info, grp->endpoint_count);
            break;
    }
}

VOID PrintNode(TREENODE_T* node)
{
    if (node == 0) return;
    USB_DEBUG(DEBUG_INFO, 3,"%x <- %x -> %x, child %x parent %x\n",
                    node->left, node, node->right, node->child, node->parent);
    PrintData(node->data);
    PrintNode(node->right);
    PrintNode(node->child);
}

VOID PrintTree(TREENODE_T* Root)
{
    USB_DEBUG(DEBUG_INFO, 3,"********USB DEVICE TREE************\n");
    PrintNode(Root);
}
*/

VOID InstallDevice(DEV_INFO* DevInfo)
{
    TREENODE_T* HcNode;
    TREENODE_T* HubNode;
    TREENODE_T* ParentNode;
    USBDEV_T* Dev;
    DEVGROUP_T* Grp;
    EFI_STATUS Status;
	EFI_TPL OldTpl;
    UINT16  Index;
    UINT16  *LangId;
    UINT16  OrgTimeOutValue;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: InstallDevice ");
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "(hc:%x,hub:%x,port:%x,addr:%x,if:%x,aif:%x,lun:%x)\n",
        DevInfo->bHCNumber, DevInfo->bHubDeviceNumber, DevInfo->bHubPortNumber, DevInfo->bDeviceAddress,
        DevInfo->bInterfaceNum, DevInfo->bAltSettingNum, DevInfo->bLUN );

    // Find HC node in tree
    HcNode = TreeSearchSibling(gUsbRootRoot->child, HcByIndex, &DevInfo->bHCNumber );

    // Do not assert here: it's fine to see a DEV_INFO from not-yet-installed HC
    if( HcNode == NULL ) return;
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: HC node found: %x\n", HcNode );

    // Find a hub node in tree
    if( DevInfo->bHubDeviceNumber & BIT7){ // hub is a root HC
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: Connecting to root Hub\n", DevInfo->bHCNumber );
        ASSERT( (DevInfo->bHubDeviceNumber & ~BIT7 )== DevInfo->bHCNumber );
        HubNode = HcNode;
    } else { // hub is usb hub device
        HubNode = UsbDevHubNode(HcNode, DevInfo);
        // Do not assert here: it may be to see a DEV_INFO from not-yet-installed hub.
        //ASSERT(HubNode != NULL);
        if (HubNode == NULL) return;
    }

    ParentNode = NULL;
    ParentNode = TreeSearchSibling(HubNode->child, 
        DevGrpByAddr, &DevInfo->bDeviceAddress );

    if( ParentNode == NULL ){
        // Create group
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: group created\n" );
        Status = gBS->AllocatePool (EfiBootServicesData, sizeof(DEVGROUP_T), &Grp);
		ASSERT_EFI_ERROR(Status);
        EfiZeroMem(Grp, sizeof(DEVGROUP_T));

        Grp->dev_info = DevInfo;
        Grp->hc = ((USBBUS_HC_T*)HcNode->data)->hc;
        Grp->hc_info  = ((USBBUS_HC_T*)HcNode->data)->hc_data;
        Grp->f_DevDesc = FALSE;
        Grp->configs = NULL;
        Grp->config_count = 0;
        Grp->ManufacturerStrDesc = NULL;
        Grp->ProductStrDesc = NULL;
        Grp->SerialNumberStrDesc = NULL;
        Grp->type = NodeGroup;
        Grp->active_config = 0;
        UsbDevLoadAllDescritors(Grp);
        //
        // Check at least for Device Descriptor present before proceeding
        //
        if(Grp->f_DevDesc == FALSE) {
          //
          //When  no Device Descriptor present quit installing the device
          //
          USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: dev install aborted - no device descriptor\n");
          return;
        }
        if (Grp->active_config == -1) {
            USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: dev install aborted - no active Configuration descriptor\n");
            return;
        }
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: descriptors loaded\n" );

#if USB_IAD_PROTOCOL_SUPPORT
        // Add IAD information if necessary
        Grp->iad_count = 0;
        Grp->iad_details = NULL;
        if (Grp->dev_desc.DeviceClass == DEV_BASE_CLASS_MISC
            && Grp->dev_desc.DeviceSubClass == DEV_SUB_CLASS_COMMON
            && Grp->dev_desc.DeviceProtocol == DEV_PROTOCOL_IAD)
        {
            FindIadDetails(Grp);
        }
#endif
 
                                        //(EIP66231+)>
        if ((Grp->dev_desc.StrManufacturer != 0) || (Grp->dev_desc.StrProduct != 0) || 
            (Grp->dev_desc.StrSerialNumber != 0)) {
            // Load langid table
            Grp->lang_table = (lang_table_t*)GetUsbDescriptor((USBDEV_T*)Grp, DESC_TYPE_STRING, 0, 0);
            if (Grp->lang_table && Grp->lang_table->len != 0) {
                Grp->lang_table->len = (Grp->lang_table->len -2) / sizeof(UINT16);
                USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: LangID table loaded\n" );
                
                LangId = Grp->lang_table->langID;
                
                for (Index = 0; Index < Grp->lang_table->len; Index++) {
                    if (LangId[Index] == USB_US_LAND_ID) {
                        break;
                    }
                }
             
                if (Index != Grp->lang_table->len) {
                    // Some devices may not respond getting string descriptors 
                    // whcih describing manufacturer, product set the timeout value to 100 ms.
                    OrgTimeOutValue = gUsbData->wTimeOutValue;
                    gUsbData->wTimeOutValue = USB_GET_STRING_DESC_TIMEOUT_MS;
                    if (Grp->dev_desc.StrManufacturer != 0) {
                        Grp->ManufacturerStrDesc = 
                            (EFI_USB_STRING_DESCRIPTOR*)GetUsbDescriptor(
                            (USBDEV_T*)Grp, DESC_TYPE_STRING, 
                            Grp->dev_desc.StrManufacturer, USB_US_LAND_ID);
                    }
                    if (Grp->dev_desc.StrProduct != 0) {
                        Grp->ProductStrDesc = 
                            (EFI_USB_STRING_DESCRIPTOR*)GetUsbDescriptor(
                            (USBDEV_T*)Grp, DESC_TYPE_STRING, 
                            Grp->dev_desc.StrProduct, USB_US_LAND_ID);
                    }
                    if (Grp->dev_desc.StrSerialNumber != 0) {
                        // Set timeout value to 3000 ms for the serial number string descriptor.
                        gUsbData->wTimeOutValue = USB_GET_SERIAL_NUMBER_DESC_TIMEOUT_MS;
                        Grp->SerialNumberStrDesc = 
                            (EFI_USB_STRING_DESCRIPTOR*)GetUsbDescriptor(
                            (USBDEV_T*)Grp, DESC_TYPE_STRING, 
                            Grp->dev_desc.StrSerialNumber, USB_US_LAND_ID);
                    }
                    gUsbData->wTimeOutValue = OrgTimeOutValue;
                }
            }
        }
                                        //<(EIP66231+)
        TreeAddChild(HubNode,(ParentNode = TreeCreate(&Grp->node, Grp)));
    } else {
        // Old group was found in tree
        TREENODE_T *tmp = TreeSearchSibling(ParentNode->child, DevByAdrIf, DevInfo );
        if(tmp){
            USB_DEBUG(DEBUG_WARN, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: IF is already in tree: %x\n", tmp );
            return;
        }

        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: multi-function; group node found: %x\n", ParentNode );
        Grp = (DEVGROUP_T*)ParentNode->data;
    }

    // Create new device node as a child of HubNode
    gBS->AllocatePool (EfiBootServicesData, sizeof(USBDEV_T), &Dev);
    ASSERT(Dev);
    if (Dev == NULL) return;
    EfiZeroMem(Dev, sizeof(USBDEV_T));

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: device node created: %x\n",
        &Dev->node );
    Dev->type = NodeDevice;
    TreeAddChild(ParentNode, TreeCreate(&Dev->node, Dev));
    Dev->dev_info = DevInfo;
    Dev->hc = ((USBBUS_HC_T*)HcNode->data)->hc;
    Dev->hc_info  = ((USBBUS_HC_T*)HcNode->data)->hc_data;
    Dev->f_connected = FALSE;

    Status = UsbDevLoadEndpoints(Dev);
    
    if (EFI_ERROR(Status)) {
        return;
    }

    // Speed 00/10/01 - High/Full/Low
                                        //(EIP81612)>
    switch (Dev->dev_info->bEndpointSpeed) {
        case USB_DEV_SPEED_SUPER_PLUS:
            Dev->speed = EFI_USB_SPEED_SUPER_PLUS; 
            break;
        case USB_DEV_SPEED_SUPER:
            Dev->speed = EFI_USB_SPEED_SUPER; 
            break;
        case USB_DEV_SPEED_FULL:
            Dev->speed = EFI_USB_SPEED_FULL;
            break;
        case USB_DEV_SPEED_LOW:
            Dev->speed = EFI_USB_SPEED_LOW;
            break;
        case USB_DEV_SPEED_HIGH:
            Dev->speed = EFI_USB_SPEED_HIGH;
    }
                                        //<(EIP81612)
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: endpoints loaded\n" );

    // Create Device Path
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: preparing DP...\n" );

    ASSERT(((USBDEV_T*)HubNode->data)->dp);
    Dev->dp = DpAddUsbSegment(((USBDEV_T*)HubNode->data)->dp,
        DevInfo->bHubPortNumber - 1, DevInfo->bInterfaceNum);
    if (DevInfo->bLUN) {
        Dev->dp = DpAddLun(Dev->dp, DevInfo->bLUN);
    }
    ASSERT(Dev->dp);

    // Install USB_IO protocol
    Dev->io.UsbControlTransfer          = UsbIoControlTransfer;
    Dev->io.UsbBulkTransfer             = UsbIoBulkTransfer;
    Dev->io.UsbAsyncInterruptTransfer   = UsbIoAsyncInterruptTransfer;
    Dev->io.UsbSyncInterruptTransfer    = UsbIoSyncInterruptTransfer;
    Dev->io.UsbIsochronousTransfer      = UsbIoIsochronousTransfer;
    Dev->io.UsbAsyncIsochronousTransfer = UsbIoAsyncIsochronousTransfer;

    Dev->io.UsbGetDeviceDescriptor      = UsbIoGetDeviceDescriptor;
    Dev->io.UsbGetConfigDescriptor      = UsbIoGetConfigDescriptor;
    Dev->io.UsbGetInterfaceDescriptor   = UsbIoGetInterfaceDescriptor;
    Dev->io.UsbGetEndpointDescriptor    = UsbIoGetEndpointDescriptor;
    Dev->io.UsbGetStringDescriptor      = UsbIoGetStringDescriptor;
    Dev->io.UsbGetSupportedLanguages    = UsbIoGetSupportedLanguages;

    Dev->io.UsbPortReset                = UsbIoPortReset;

//    DEBUG_DELAY();
    //Install DP_ protocol
	Status = gBS->InstallMultipleProtocolInterfaces (
        &Dev->handle,
        &gEfiUsbIoProtocolGuid, &Dev->io,
        &gEfiDevicePathProtocolGuid, Dev->dp,
        NULL);
	ASSERT_EFI_ERROR(Status);
	
    *(UINTN*)Dev->dev_info->Handle = (UINTN)Dev->handle;
	Dev->dev_info->Flag |= DEV_INFO_DEV_BUS;
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: USB_IO %x installed on Dev %x\n", &Dev->io, Dev );

    {
        VOID* tmp;
        Status = gBS->OpenProtocol(
            Dev->hc_info->Controller,
            &gEfiUsb2HcProtocolGuid,
            &tmp,
            gUSBBusDriverBinding.DriverBindingHandle,
            Dev->handle,
            EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);
        
        ASSERT(Status == EFI_SUCCESS);

    }

    PROGRESS_CODE(DXE_USB_HOTPLUG);

    OldTpl = gBS->RaiseTPL(TPL_HIGH_LEVEL);
    gBS->RestoreTPL(TPL_CALLBACK);

    // Connect controller to start device drvirs
    Status = gBS->ConnectController(Dev->handle,NULL,NULL,TRUE);

    gBS->RaiseTPL(TPL_HIGH_LEVEL);
    gBS->RestoreTPL(OldTpl);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: instdev: connect controller: %r\n", Status );

    if( !EFI_ERROR(Status)){
        Dev->f_connected = TRUE;
    }
#if USB_IAD_PROTOCOL_SUPPORT
    ProcessIad(Grp, Dev);
#endif
}

#if USB_IAD_PROTOCOL_SUPPORT

/**
	This function uinstalls AMI_USB_IADIO_PROTOCOL and frees memory
	associated with it
**/

EFI_STATUS UninstallIad(USBDEV_T *Dev)
{
    UINT8 i;
    IAD_DETAILS *IadDetails;
    DEVGROUP_T* Grp = UsbDevGetGroup(Dev);
    
    if (Grp == NULL) return EFI_NOT_FOUND;
    if (Grp->iad_count == 0) return EFI_NOT_FOUND;

    IadDetails = Grp->iad_details;
    
    for (i = 0; i < Grp->iad_count; i++)
    {
        gBS->DisconnectController(IadDetails->IadHandle, NULL, NULL);
        gBS->FreePool(IadDetails->Data);      
        gBS->UninstallMultipleProtocolInterfaces(
            IadDetails->IadHandle,
            &gAmiUsbIadProtocolGuid, &IadDetails->Iad,
            NULL);
        IadDetails++;
    }
    gBS->FreePool(Grp->iad_details);
    Grp->iad_details = NULL;
    Grp->iad_count = 0;

    return EFI_SUCCESS;
}

#endif


/**
    Disconnects device; uninstalls USB_IO and DEVICEPATH protocols
    delocates all memory used USB Bus driver to support the device
    removes device node from the tree; if device has children, all
    UninstallDevice procedure is repeated for each child.

    In case if disconnect or protocol uninstall fails, it reports
    error stauts returned from Boot service procedure.

**/

EFI_STATUS UninstallDevice(USBDEV_T* Dev)
{
    EFI_STATUS Status;
	EFI_TPL OldTpl;

#if USB_IAD_PROTOCOL_SUPPORT
	// Uninstall IAD protocol, if any
    UninstallIad(Dev);
#endif

    //
    // Uninstall connected devices if it's a hub
    //
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: UninstallDevice: node %x; ", &Dev->node);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "info:%x [adr:%d;if:%d] uninstalling children...\n",
        Dev->dev_info,Dev->dev_info->bDeviceAddress, Dev->dev_info->bInterfaceNum);
    if( TreeSearchSibling( Dev->node.child, eUninstallDevice, &Status ))
        return Status;

    OldTpl = gBS->RaiseTPL(TPL_HIGH_LEVEL);
    gBS->RestoreTPL(TPL_CALLBACK);

	Status = gBS->DisconnectController(Dev->handle, NULL, NULL);

    gBS->RaiseTPL(TPL_HIGH_LEVEL);
    gBS->RestoreTPL(OldTpl);

    if (EFI_ERROR(Status)) {
        QueuePut(&gUsbData->QueueCnnctDisc, Dev->dev_info);
        Dev->dev_info->Flag |= DEV_INFO_IN_QUEUE;
        return Status;
    }

	if (Dev->async_endpoint != 0) {
		Status = Dev->io.UsbAsyncInterruptTransfer(&Dev->io, Dev->async_endpoint, FALSE, 
			0, 0, NULL, NULL);
		Dev->async_endpoint = 0;
	}

    Status = gBS->CloseProtocol (
        Dev->hc_info->Controller,
        &gEfiUsb2HcProtocolGuid,
        gUSBBusDriverBinding.DriverBindingHandle,
        Dev->handle
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    //
    // Try to uninstall protocols
    //
    // they can be denied to uninstall, which result in
    // keeping this device and all bus alive
    //
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: uninstdev: [%d:%d] uninstalling protocols...",
        Dev->dev_info->bDeviceAddress, Dev->dev_info->bInterfaceNum);
    Status = gBS->UninstallMultipleProtocolInterfaces (
        Dev->handle,
        &gEfiUsbIoProtocolGuid, &Dev->io,
        &gEfiDevicePathProtocolGuid, Dev->dp,
        NULL);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "%r\n", Status );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

	Dev->dev_info->Flag &= ~DEV_INFO_DEV_BUS;
	if (!(Dev->dev_info->Flag & (DEV_INFO_MASS_DEV_REGD | DEV_INFO_DEV_PRESENT))) {
		Dev->dev_info->Flag &= ~DEV_INFO_VALID_STRUC;
	}

    if (Dev->dev_info->IsocDetails.XferRing != 0)
    {
        UINTN Address = *(UINTN*)Dev->dev_info->IsocDetails.XferRing;
        gBS->FreePages((EFI_PHYSICAL_ADDRESS)Address, EFI_SIZE_TO_PAGES(0x20000));
        Dev->dev_info->IsocDetails.XferRing = 0;
    }

    //
    // Unistall succeeded, free usbdev
    //
    TreeRemove(&Dev->node);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: uninstdev: [%d:%d] done.\n",
        Dev->dev_info->bDeviceAddress, Dev->dev_info->bInterfaceNum);

    if(Dev->name)
        gBS->FreePool(Dev->name);
    gBS->FreePool(Dev);

    return EFI_SUCCESS;
}


/**
    Enumeration call-back function that is usded
    uninstall all enumerated device nodes
    Stops enumeration as soon as error was recieved
    @param 
        Node - tree node of the USB device or group
        Contex - pointer to the EFI_STATUS variable that
        recieves status information if error
        was recieved
    @retval TRUE  on error found; this will stop enumeration
        FALSE on succesfull uninstall operation

**/
int
eUninstallDevice(
    VOID* Node,
    VOID* Context
)
{
    EFI_STATUS *Status = (EFI_STATUS*)Context;
    DEVGROUP_T* Grp = (DEVGROUP_T*)Node;
    ASSERT(Status);

    if (Grp->type == NodeGroup) {
        //
        // Uninstall all CONNECTED devices within group
        //
        TreeSearchSibling( Grp->node.child, eUninstallDevice, Status );
        if (EFI_ERROR(*Status)) {
            USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_USBBUS, "USBBUS: devgroup uninstall failed: devaddr:0x%x\n",
                Grp->dev_info->bDeviceAddress );
            return TRUE; //stop enumeration
        }
        // Free devgroup
        TreeRemove(&Grp->node);
        if (Grp->configs) {
            int i;
            for (i = 0; i < Grp->config_count; ++i) {
                if (Grp->configs[i]) {
                    gBS->FreePool(Grp->configs[i]);
                }
            }
            gBS->FreePool(Grp->configs);
        }
#if USB_IAD_PROTOCOL_SUPPORT
        if (Grp->iad_details) {
            gBS->FreePool(Grp->iad_details);
        }
#endif
        if (Grp->ManufacturerStrDesc) {
            gBS->FreePool(Grp->ManufacturerStrDesc);
        }
        if (Grp->ProductStrDesc) {
            gBS->FreePool(Grp->ProductStrDesc);
        }
        if (Grp->SerialNumberStrDesc) {
            gBS->FreePool(Grp->SerialNumberStrDesc);
        }
        gBS->FreePool(Grp);
    } else if (Grp->type == NodeDevice ) {
        //
        //Uninstall Device
        //
        *Status = UninstallDevice((USBDEV_T*)Node);
        if (EFI_ERROR(*Status)) {
            USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_USBBUS, "USBBUS: usbdev uninstall failed: if:%d\n",
                ((USBDEV_T*)Node)->dev_info->bInterfaceNum );
            return TRUE; //stop enumeration
        }
    }
    return FALSE;// continue enumeration
}


/**
    Finds the USB hub node that sits on the same
    path (seq. of (hub ports,if) ) but have different
    USB address or DEV_INFO node. This could be the result
    of lost disconnect event or previous error to uninstall
    USB_IO
    @param 
        pDevInfo - DEV_INFO structure that is checked for
        echoes in the bus
    @retval EFI_SUCCESS echo wasn't found or was succesfully removed
        otherwise return status resulted from attemp to remove the node

**/

EFI_STATUS
RemoveHubEcho(
    TREENODE_T  *HcNode,
    UINT8       Addr
)
{
    while(!( Addr  & BIT7 )){
        //
        // Find hub DEV_INFO
        //
        int i;
        TREENODE_T *HubNode;
        TREENODE_T* DevNode;
        DEV_INFO* DevInfo=NULL;

        for ( i= 1; i < MAX_DEVICES; i++)   {
            if( ((gUsbData->aDevInfoTable[i].Flag &
            (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT | DEV_INFO_DEV_DUMMY))
            != (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT)) &&
             gUsbData->aDevInfoTable[i].bDeviceAddress == Addr )
        {
                DevInfo = gUsbData->aDevInfoTable+i;
                break;
            }
        }
        if ( DevInfo == NULL )
            return EFI_NOT_FOUND;

        //
        // Search for parent hub
        //
        if( DevInfo->bHubDeviceNumber  & BIT7 ){
            //Root hub
            HubNode = HcNode;
        } else {
            //USB hub device
            TREENODE_T* HubGrpNode = TreeSearchDeep(HcNode->child,
                DevGrpByAddr, &DevInfo->bHubDeviceNumber );
            if( HubGrpNode != NULL ){
                USBDEV_T* Dev;

                HubNode = HubGrpNode->child;
                ASSERT(HubNode);
                DevNode = TreeSearchSibling(HubNode->child,
                            DevGrpByPortIf, DevInfo);
                if(DevNode==NULL) return EFI_SUCCESS;
                Dev = (USBDEV_T*)DevNode->data;
                if( Dev && (DevInfo->bDeviceAddress !=
                    Dev->dev_info->bDeviceAddress ||
                    DevInfo != Dev->dev_info ))
                {
                    //
                    // The disconnect event must have been droped
                    // disconnect old info now
                    //

                    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: echo found [%x]:\n" );
                    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS,
                        "\t(hc:0x%x,hub:0x%x,port:%d,addr:0x%x,if:%d)\n",
                        Dev->dev_info->bHCNumber,
                        Dev->dev_info->bHubDeviceNumber,
                        Dev->dev_info->bHubPortNumber,
                        Dev->dev_info->bDeviceAddress,
                        Dev->dev_info->bInterfaceNum);
                    return RemoveDevInfo(Dev->dev_info);
                }
                return EFI_SUCCESS;
            } else {
                //
                // Either hub wasn't added to bus yet; or there is echo for the
                // brunch. The the later case succesfull removal of the burnch
                // removes an echo for this device info
                //
                Addr = DevInfo->bHubDeviceNumber;

            }
        }
    }
    return EFI_SUCCESS;
}


/**
    Finds the USB device node that sits on the same
    path (seq. of (hub ports,if) ) but have different
    USB address or DEV_INFO node. This could be the result
    of lost disconnect event or previous error to uninstall
    USB_IO
    @param 
        DevInfo - DEV_INFO structure that is checked for
        echoes in the bus
    @retval EFI_SUCCESS echo wasn't found or was succesfully removed
        otherwise return status resulted from attemp to remove the node

**/

EFI_STATUS
RemoveDevInfoEcho(
    DEV_INFO    *DevInfo
)
{
    USBDEV_T    *Dev;
    TREENODE_T  *DevNode;
    TREENODE_T  *HcNode, *HubNode;

    HcNode = TreeSearchSibling(gUsbRootRoot->child,HcByIndex,
        &DevInfo->bHCNumber);
    if(HcNode==NULL) return EFI_SUCCESS;

    if( DevInfo->bHubDeviceNumber  & BIT7 ){
        //Root hub
        HubNode = HcNode;
    } else {
        //USB hub device
        TREENODE_T* HubGrpNode = TreeSearchDeep(HcNode->child,
            DevGrpByAddr, &DevInfo->bHubDeviceNumber );
        if( HubGrpNode != NULL ){
            HubNode = HubGrpNode->child;
            ASSERT(HubNode);
        } else {
            //
            // Either hub wasn't added to bus yet; or there is echo for the
            // brunch. The the later case succesfull removal of the burnch
            // removes an echo for this device info
            //
            //return RemoveHubEcho(HcNode, DevInfo->bHubDeviceNumber);
            return EFI_SUCCESS;
        }
    }
    DevNode = TreeSearchSibling(HubNode->child, DevGrpByPortIf, DevInfo);
    if(DevNode==NULL) return EFI_SUCCESS;
    Dev = (USBDEV_T*)DevNode->data;
    if (Dev != NULL) {
        //
        // The disconnect event must have been droped
        // disconnect old info now
        //

        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: echo found [%x]:\n" );
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS,
            "\t(hc:0x%x,hub:0x%x,port:%d,addr:0x%x,if:%d)\n",
            Dev->dev_info->bHCNumber,
            Dev->dev_info->bHubDeviceNumber,
            Dev->dev_info->bHubPortNumber,
            Dev->dev_info->bDeviceAddress,
            Dev->dev_info->bInterfaceNum);
        return RemoveDevInfo(Dev->dev_info);
    }
    return EFI_SUCCESS;
}


/**
    Removes device node from the USB bus tree. Device node
    corresponds to the DEV_INFO. Device gets removed in response to
    the pending removal event sheduled from SMM when disconnect
    was detected on USB
    @param 
        DevInfo - DEV_INFO structure that was disconnect

    @retval EFI_SUCCESS echo wasn't found or was succesfully removed
        otherwise return status resulted from attemp to remove the node

**/

EFI_STATUS RemoveDevInfo(DEV_INFO* DevInfo)
{
    TREENODE_T  *DevNode;
    TREENODE_T  *HcNode;
    EFI_STATUS  Status;

    HcNode = TreeSearchSibling(gUsbRootRoot->child, HcByIndex,
        &DevInfo->bHCNumber);
    if(HcNode==NULL) return EFI_NOT_FOUND;

    //Check for echoes

    DevNode = TreeSearchDeep(HcNode->child,DevGrpByAddr,
        &DevInfo->bDeviceAddress);
    if (DevNode==NULL){
        USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_USBBUS, "\tdevice is not found in the tree...\n" );
        return EFI_NOT_FOUND;
    }
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "\tdevice found in the tree...\n" );

    Status = EFI_SUCCESS;
    eUninstallDevice(DevNode->data, &Status);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "\tDisconnect complete: %r.\n", Status );

    return Status;
}


/**
    Timer call-back routine that is
    is used to monitor changes on USB Bus
    It monitors the state of queueCnnct and queueDiscnct queues
    which get populated by UsbSmiNotify call-back routine

    When this routine finds a new device connected to usb it
    will install a device node for that device by calling
    InstallDevice

    When this routine finds a disconneced device it uninstalls the
    device node by calling UninstallDevice

**/

VOID
EFIAPI
UsbHcOnTimer(
    EFI_EVENT   Event,
    VOID        *Context
)
{
    DEV_INFO* EventCnnct=0;
    static int i=0;
    int Lock;
    EFI_STATUS  Status;

    UsbSmiPeriodicEvent();

	if (Event) {
		gBS->SetTimer(Event, TimerCancel, ONESECOND /10);
	}

    ATOMIC({Lock = gBustreeLock; gBustreeLock = 1;});

    if (Lock) {
        USB_DEBUG(DEBUG_WARN, DEBUG_LEVEL_USBBUS, "UsbHcOnTimer::  is locked; return\n");
        return;
    }

    do {
        ATOMIC({EventCnnct = QueueGet(&gUsbData->QueueCnnctDisc);});

        if (EventCnnct == NULL) {
            break;
        }
        EventCnnct->Flag &= ~DEV_INFO_IN_QUEUE;
        //
        // There is no need to raise tpl here: this is callback of Event with
        // TPL_CALLBACK, so this code doesn't reenter; the Install and Uninstall
        // are also called from driver stop and start. Stop and start protect
        // the code at TPL_CALLBACK level
        //
        if( (EventCnnct->Flag & (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT |
            DEV_INFO_DEV_DUMMY)) == (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT) ){
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "UsbHcOnTimer:: event connect [%d]: %x\n", i++, EventCnnct);

            RemoveDevInfoEcho(EventCnnct);
            InstallDevice(EventCnnct);
        } else if ((EventCnnct->Flag & (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT |
            DEV_INFO_DEV_DUMMY)) == DEV_INFO_VALID_STRUC) {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS,
                "UsbHcOnTimer:: event disconnect [%d]: %x\n", i++, EventCnnct);
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS,
                "\t(hc:0x%x,hub:0x%x,port:%d,addr:0x%x,if:%d)\n",
                EventCnnct->bHCNumber, EventCnnct->bHubDeviceNumber,
                EventCnnct->bHubPortNumber, EventCnnct->bDeviceAddress,
                EventCnnct->bInterfaceNum);
            //RemoveDevInfoEcho(EventCnnct);
            Status = RemoveDevInfo(EventCnnct);
            if (EFI_ERROR(Status)) {
        		break;
            }
        }
    } while ( EventCnnct != NULL );
    
    gBustreeLock = 0;

	if (Event) {
		gBS->SetTimer(Event, TimerPeriodic, ONESECOND / 10);
	}
}


/**
    Enumerate all DEV_INFO structures in the aDevInfoTable array
    and install all currently connected device
**/

VOID
PopulateTree(
    HC_STRUC    *HcStruc
)
{
    UINT16  i;

    PROGRESS_CODE(DXE_USB_DETECT);

	UsbHcOnTimer(NULL, NULL);

    for (i = 1; i < COUNTOF(gUsbData->aDevInfoTable); i++) {
        if ((gUsbData->aDevInfoTable[i].Flag & (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT | 
            DEV_INFO_DEV_DUMMY | DEV_INFO_DEV_BUS)) == (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT)) {
            if (HcStruc != gUsbData->HcTable[gUsbData->aDevInfoTable[i].bHCNumber - 1]) {
                continue;
            }
			//
			// Valid and present device behind specified HC, so insert it in the tree
			//
			USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USBBUS: PopulateTree: found valid dev info[%d]: %x\n",i, gUsbData->aDevInfoTable +i );
			
			//RemoveDevInfoEcho(&gUsbData->aDevInfoTable[i]);
			//InstallDevice( gUsbData->aDevInfoTable + i );
            if (!(gUsbData->aDevInfoTable[i].Flag & DEV_INFO_IN_QUEUE)) {
                QueuePut(&gUsbData->QueueCnnctDisc, &gUsbData->aDevInfoTable[i]);
                gUsbData->aDevInfoTable[i].Flag |= DEV_INFO_IN_QUEUE;
            }
        }
    }
}


/**
    This function is a part of binding protocol, it returns
    the string with the controller name.

**/

CHAR16*
UsbBusGetControllerName(
    EFI_HANDLE  Controller,
    EFI_HANDLE  Child
)
{
                                        //(EIP60745+)>
    EFI_STATUS Status;

    Status = gBS->OpenProtocol ( Controller,
                            &gEfiUsb2HcProtocolGuid,
                            NULL,
                            gUSBBusDriverBinding.DriverBindingHandle,
                            Controller,
                            EFI_OPEN_PROTOCOL_TEST_PROTOCOL );

    if (Status != EFI_SUCCESS && Status != EFI_ALREADY_STARTED) {
        return NULL;
    }
                                        //<(EIP60745+)
    if(Child) {
        //Get name for USB device
        EFI_USB_IO_PROTOCOL *UsbIo;
        USBDEV_T            *Dev ;
        if( EFI_ERROR(gBS->HandleProtocol(Child,& gEfiUsbIoProtocolGuid, &UsbIo)))
        {
            return NULL;
        }
        Dev = UsbIo2Dev(UsbIo);
        if( Dev->name == 0)
            Dev->name = LoadName(Dev);
        return Dev->name;
    } else {
        //Get name for USB HC
        return L"USB Host Controller (USBBUS)";
    }
}


/**
    This is AmiUsbProtocol API function. It activates
    endpoint polling for a given interrupt endpoint.

**/

EFI_STATUS UsbActivatePolling(
    EFI_USB_PROTOCOL* AmiUsbProtocol,
    VOID*   UsbIo,
    UINT8   IntEndpoint,
    UINT16  IntMaxPktSize,
    UINT8   IntInterval
)
{
    USBDEV_T* Dev = UsbIo2Dev((EFI_USB_IO_PROTOCOL*)UsbIo);
    if (Dev == NULL) return EFI_NOT_FOUND;
    
    Dev->dev_info->IntInEndpoint = IntEndpoint;
    Dev->dev_info->IntInMaxPkt = IntMaxPktSize;
    Dev->dev_info->PollingLength = IntInterval;
    Dev->dev_info->bPollInterval = IntInterval;
    UsbSmiActivatePolling(Dev->hc_info, Dev->dev_info);
    
    return EFI_SUCCESS;
}

/**
    This is AmiUsbProtocol API function. It deactivates
    interrupt endpoint polling for a given UsbIo.
**/

EFI_STATUS UsbDeactivatePolling(
    EFI_USB_PROTOCOL* AmiUsbProtocol,
    VOID*   UsbIo
)
{
    USBDEV_T* Dev = UsbIo2Dev((EFI_USB_IO_PROTOCOL*)UsbIo);

    if (Dev == NULL) return EFI_NOT_FOUND;
    UsbSmiDeactivatePolling(Dev->hc_info, Dev->dev_info);
    
    return EFI_SUCCESS;
}


/**
    This call executes non-blocking Asyncronous Isoch transfer. The
    standard UsbIo->UsbAsyncIsochronousTransfer is extended with the
    transfer completion information: XferComplete is updated when the
    requested amount of data (Length) is transferred.
    This call may be useful for isochronous streaming, where XferComplete
    indicator may signal the event indicating the certain amount transfer
    completion.
**/

EFI_STATUS
EFIAPI
UsbAccumulateIsochronousData(
    EFI_USB_PROTOCOL*   AmiUsbProtocol,
    VOID*   UsbIo,
    UINT8*  Buffer,
    UINT8   IsocEndpoint,
    UINT16  MaxPkt,
    UINT8   Mult,
    UINT32  Length,
    UINT32* TransferDetails,
    UINT8*  XferComplete
)
{
    USBDEV_T* Dev = UsbIo2Dev((EFI_USB_IO_PROTOCOL*)UsbIo);

    if (Dev == NULL) return EFI_NOT_FOUND;

    // Update DEV_INFO with the isoc endpoint details
    Dev->dev_info->IsocDetails.EpMaxPkt = MaxPkt;
    Dev->dev_info->IsocDetails.EpMult = Mult;
    Dev->dev_info->IsocDetails.XferDetails = TransferDetails;
    Dev->dev_info->IsocDetails.Endpoint = IsocEndpoint;

    ASSERT(Dev->dev_info->IsocDetails.Endpoint);
    
    CRITICAL_CODE( EFI_TPL_NOTIFY,
    {
        UsbSmiIsocTransfer(
            Dev->hc_info,
            Dev->dev_info,
            Dev->dev_info->IsocDetails.Endpoint & 0x80,
            Buffer,
            Length,
            XferComplete
        );
    )};

    return EFI_SUCCESS;
}


/**
    Install driver binding and controller name protocols
    for the USB bus driver. Hook up UsbProtocol with the
    UsbBus related functions.

**/

EFI_STATUS
UsbBusInit(
    EFI_HANDLE  ImageHandle,
    EFI_HANDLE  ServiceHandle
)
{
                                        //(EIP59272)>
    static NAME_SERVICE_T usbbus_names;
    EFI_USB_PROTOCOL    *AmiUsbProtocol;
    EFI_STATUS Status;

    Status = pBS->LocateProtocol(&gEfiUsbProtocolGuid, NULL, &AmiUsbProtocol);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
    {
        return Status;
    }

    AmiUsbProtocol->UsbAccumulateIsochronousData = UsbAccumulateIsochronousData;

    gUSBBusDriverBinding.DriverBindingHandle = ServiceHandle;
    gUSBBusDriverBinding.ImageHandle = ImageHandle;

    return gBS->InstallMultipleProtocolInterfaces(
                &gUSBBusDriverBinding.DriverBindingHandle,
                &gEfiDriverBindingProtocolGuid, &gUSBBusDriverBinding,
                &gEfiComponentName2ProtocolGuid, InitNamesProtocol(&usbbus_names,
                L"USB bus", UsbBusGetControllerName),
                NULL);
                                        //<(EIP59272)
}


/**
    This is a binding protocol function that returns EFI_SUCCESS
    for USB controller and EFI_UNSUPPORTED for any other kind of
    controller.

**/

EFI_STATUS
EFIAPI
UsbBusSupported(
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE Controller,
    EFI_DEVICE_PATH_PROTOCOL *DevicePath
)
{
    EFI_STATUS  Status;
    VOID*       Ptr = 0;
    HC_STRUC    *HcStruc;

    Status = gBS->OpenProtocol ( Controller, &gEfiUsb2HcProtocolGuid,
        &Ptr, This->DriverBindingHandle, Controller, EFI_OPEN_PROTOCOL_BY_DRIVER );

    if (Status != EFI_SUCCESS && Status != EFI_ALREADY_STARTED) {
        return EFI_UNSUPPORTED;
    } else if (Status == EFI_ALREADY_STARTED) {
        return Status;
    }

    gBS->CloseProtocol ( Controller, &gEfiUsb2HcProtocolGuid,
        This->DriverBindingHandle, Controller);

    HcStruc = FindHcStruc(Controller);

    if (HcStruc == NULL) {
        return EFI_UNSUPPORTED;
    }

    Status = gBS->OpenProtocol ( Controller, &gEfiDevicePathProtocolGuid,
        NULL, This->DriverBindingHandle, Controller, EFI_OPEN_PROTOCOL_TEST_PROTOCOL );

    if (Status != EFI_SUCCESS && Status != EFI_ALREADY_STARTED) {
        return EFI_UNSUPPORTED;
    }

    return Status;
}

/**
    This function is part of binding protocol installed on USB
    controller. It stops the controller and removes all the
    children.

**/

EFI_STATUS
EFIAPI
UsbBusStop (
   EFI_DRIVER_BINDING_PROTOCOL     *This,
   EFI_HANDLE                      Controller,
   UINTN                           NumberOfChildren,
   EFI_HANDLE                      *Children  )
{
    EFI_STATUS	Status = EFI_SUCCESS;
    TREENODE_T	*HcNode;
    USBBUS_HC_T	*HcDev;

    //EFI_TPL SaveTpl = gBS->RaiseTPL (EFI_TPL_NOTIFY);
    //ASSERT(SaveTpl <= EFI_TPL_NOTIFY );

	HcNode = TreeSearchSibling(gUsbRootRoot->child, HcByHandle, &Controller );
	ASSERT(HcNode);
	if( HcNode == NULL ) {
		//gBS->RestoreTPL(SaveTpl);
		return EFI_DEVICE_ERROR;
	}
	HcDev = (USBBUS_HC_T*)HcNode->data;
    
	Status = gBS->SetTimer(gEvUsbEnumTimer, TimerCancel, ONESECOND/10);
	
    ASSERT(Status == EFI_SUCCESS);

    if (EFI_ERROR(Status)) {
        return Status;
    }

    UsbHcOnTimer(NULL, NULL);
    
	if (TreeSearchSibling(HcNode->child, eUninstallDevice, &Status) != NULL)
	{
		USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, 
			"USBBUS: Stop HC: [%d] failed to uninstall children\n",
			((USBBUS_HC_T*)HcNode->data)->hc_data->bHCNumber);
		//gBS->RestoreTPL(SaveTpl);
		Status = gBS->SetTimer(gEvUsbEnumTimer, TimerPeriodic, ONESECOND/10);

	    ASSERT(Status == EFI_SUCCESS);

		return Status;
	}

	if (NumberOfChildren == 0) {
		TreeRemove(HcNode);

		//
		// Close Protocols opened by driver
		//
		gBS->CloseProtocol (
			Controller, &gEfiUsb2HcProtocolGuid,
			This->DriverBindingHandle, Controller);
		
		gBS->FreePool(HcDev);
		
		if(--gCounterUsbEnumTimer==0){
		    Status = gBS->SetTimer(gEvUsbEnumTimer, TimerCancel, ONESECOND/10);

		    ASSERT(Status == EFI_SUCCESS);
	    
		    Status = gBS->CloseEvent(gEvUsbEnumTimer);
		    
		    ASSERT(Status == EFI_SUCCESS);
		    
			gEvUsbEnumTimer=0;
		}
	}

    if (gCounterUsbEnumTimer != 0) {
        Status = gBS->SetTimer(gEvUsbEnumTimer, TimerPeriodic, ONESECOND/10);
        ASSERT(Status == EFI_SUCCESS);
    }
    //gBS->RestoreTPL(SaveTpl);

    return EFI_SUCCESS;
}


/**
    This function is part of binding protocol installed on USB
    controller. It starts the USB bus for a given controller.

**/

EFI_STATUS
EFIAPI
UsbBusStart(
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE                  Controller,
    EFI_DEVICE_PATH_PROTOCOL    *DevicePathProtocol
)
{
    USBBUS_HC_T* HcDev = 0;
	EFI_STATUS	Status = EFI_UNSUPPORTED;

    Status = gBS->AllocatePool(
            EfiBootServicesData,
            sizeof(USBBUS_HC_T),
            &HcDev);
            
    ASSERT(Status == EFI_SUCCESS);

    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    HcDev->type = NodeHC;
    HcDev->hc_data = FindHcStruc(Controller);

//    ASSERT(HcDev->hc_data);
    if (HcDev->hc_data == NULL) {
        gBS->FreePool(HcDev);
        return EFI_DEVICE_ERROR;
    }

    //
    // Open Protocols
    //
	Status = gBS->OpenProtocol ( Controller,
		&gEfiUsb2HcProtocolGuid,	&HcDev->hc,
		This->DriverBindingHandle, Controller,
		EFI_OPEN_PROTOCOL_BY_DRIVER );
	
	ASSERT(Status == EFI_SUCCESS);
	
	if (EFI_ERROR(Status)) {
		return Status;
	}

    Status = gBS->OpenProtocol ( Controller,
        &gEfiDevicePathProtocolGuid,
        &HcDev->dp, This->DriverBindingHandle,
        Controller, EFI_OPEN_PROTOCOL_GET_PROTOCOL );
    
    ASSERT(Status == EFI_SUCCESS);
    
	if (EFI_ERROR(Status)) {
		return Status;
	}

    //
    // Install Polling timer
    //
    {
        //EFI_TPL SaveTpl = gBS->RaiseTPL (EFI_TPL_NOTIFY);
        //ASSERT( SaveTpl <= EFI_TPL_NOTIFY);
        if (gEvUsbEnumTimer != 0) {
            Status = gBS->SetTimer(gEvUsbEnumTimer, TimerCancel, ONESECOND/10);
            ASSERT(Status == EFI_SUCCESS);
        }
        //
        // Critical section

//        HookSmiNotify(1);

        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS,"**** \tnew hc_struc: %x(type:%x,number:%x)\n",
                HcDev->hc_data, HcDev->hc_data->bHCType, HcDev->hc_data->bHCNumber);

        //
        // Create HC branch in the USB root
        //
        TreeAddChild(gUsbRootRoot, TreeCreate(&HcDev->node,HcDev));

        gCounterUsbEnumTimer++;
        PopulateTree(HcDev->hc_data);

		UsbHcOnTimer(NULL, NULL);

        // Setting up global: gUsbDeviceToDisconnect, gUsbDeviceToConnect
        if (gEvUsbEnumTimer == 0) {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: Start: setup timer callback %x\n", &UsbHcOnTimer );
            Status = gBS->CreateEvent( EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL,
                EFI_TPL_CALLBACK, UsbHcOnTimer, 0, &gEvUsbEnumTimer);
            
            ASSERT(Status == EFI_SUCCESS);
        }
        Status = gBS->SetTimer(gEvUsbEnumTimer, TimerPeriodic, ONESECOND / 10);
        ASSERT(Status == EFI_SUCCESS);
        //gBS->RestoreTPL(SaveTpl);
    }

    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
