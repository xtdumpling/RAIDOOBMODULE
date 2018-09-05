//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/**@file HardwareSignatureManagement.c

 This file will install the HardwareChange protocol. The hardware signature data of the FACS table will be 
 updated when the ReadyToBoot event is signaled.
 
 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php.
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

//---------------------------------------------------------------------------
#include <Efi.h>
#include <Dxe.h>
#include <AmiDxeLib.h>
#include <Token.h>
#include <Acpi.h>
#include <Acpi20.h>
#include <Protocol/AcpiSupport.h>
#include <HardwareChangeProtocol.h>
#include <TimeStamp.h>
#include "HardwareChangeDetect.c"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
//---------------------------------------------------------------------------

#define HARDWARE_CONFIG_DATA_VARIABLE L"HardwareConfigData"

#define FIRMWARE_UPDATE_COUNT_VARIABLE L"FirmwareUpdateCountVar"

#ifndef AMITSE_EVENT_BEFORE_BOOT_GUID
#define AMITSE_EVENT_BEFORE_BOOT_GUID \
  { 0x3677770f, 0xefb2, 0x43b2, { 0xb8, 0xae, 0xb3, 0x2, 0xe9, 0x60, 0x48, 0x82 } }
#endif

#ifndef EFI_AMI_LEGACYBOOT_PROTOCOL_GUID
#define EFI_AMI_LEGACYBOOT_PROTOCOL_GUID            \
  { 0x120d28aa, 0x6630, 0x46f0, { 0x81, 0x57, 0xc0, 0xad, 0xc2, 0x38, 0x3b, 0xf5 } }
#endif
//---------------------------------------------------------------------------

EFI_GUID  gHardwareChangeProtocolGuid = HARDWARE_CHANGE_PROTOCOL_GUID;
EFI_GUID  gHardwareConfigDataGuid = HARDWARE_CONFIG_DATA_GUID;
EFI_GUID  gBeforeBootProtocolGuid = AMITSE_EVENT_BEFORE_BOOT_GUID;
EFI_GUID  gBeforeLegacyBootProtocolGuid = EFI_AMI_LEGACYBOOT_PROTOCOL_GUID;
EFI_GUID  gSetupUpdateCountGuid = SETUP_UPDATE_COUNT_GUID;

//
//Record the hardware change flag if there are some hardware 
//configurations changed. 
//
UINT32 gHardwareConfigChangeFlag = 0;

//
//Record the current hardware configuration data.
//
HW_CONFIG_DATA gCurHardwareConfigData;

//
//Record firmware update times
//
UINT32 gFwUpdateCount = 0;

//
//Record called times if other module called this protocol
//
UINT32 gExternalFunCallCount = 0;

//---------------------------------------------------------------------------   
//Function prototype
//
EFI_STATUS 
SetHardwareConfigDataToNvram (
    IN HW_CONFIG_DATA *HardwareConfigData );
        
//---------------------------------------------------------------------------

//
//Functions defined in HardwareChangeDetect.c
//
BOOLEAN 
IsDifferentMemorySize(
    IN UINT32 CurMemorySizeMb, 
    IN UINT32 PreMemorySizeMb);

EFI_STATUS 
GetMemMapData(
    IN OUT HW_MEMORY_MAP_DATA *MemMapConfigData);

EFI_STATUS 
GetPciDeviceData(
    IN OUT HW_PCI_DATA *HardwarePciData);

EFI_STATUS 
GetUsbDeviceData(
    IN OUT HW_USB_DATA *HardwareUsbData);
        
EFI_STATUS 
GetDiskInfoData(
    OUT HW_DISK_DATA *HardwareDiskData);

#if (DETECT_PS2_KEYBOARD  || DETECT_PS2_MOUSE )
EFI_STATUS 
DetectPs2KeyboardMouseState(
    IN HW_CONFIG_TYPE ConfigType, 
    IN OUT BOOLEAN *BitPresent);
#endif

EFI_STATUS 
GetVideoData(
    OUT HW_VIDEO_DATA *HardwareVideoData );

//
//Functions to detect and set Hardware configurations
//
EFI_STATUS 
DetectAndSetPs2KeyboardConfig(
    IN UINTN BufferSize, 
    IN VOID *Buffer);

#if DETECT_PS2_MOUSE
EFI_STATUS 
DetectAndSetPs2MouseConfig(
    IN UINTN BufferSize, 
    IN VOID *Buffer);
#endif

EFI_STATUS 
DetectAndSetMemorySizeConfig(
    IN UINTN BufferSize, 
    IN VOID *Buffer);

EFI_STATUS 
DetectAndSetMemoryMapConfig(
    IN UINTN BufferSize, 
    IN VOID *Buffer);

EFI_STATUS 
DetectAndSetPciConfig(
    IN UINTN BufferSize, 
    IN VOID *Buffer);

#if HARDWARE_SIGNATURE_USB_CHANGE
EFI_STATUS 
DetectAndSetUsbConfig(
    IN UINTN BufferSize, 
    IN VOID *Buffer);
#endif

EFI_STATUS 
DetectAndSetVideoConfig (
    IN UINTN BufferSize,
    IN VOID  *Buffer );

EFI_STATUS 
DetectAndSetDiskConfig (
    IN UINTN BufferSize,
    IN VOID  *Buffer );

EFI_STATUS 
EFIAPI
UpdateHardwareSignatureData(
    IN EFI_EVENT Event, 
    IN VOID *Context);

VOID 
QuickSort(
    IN OUT UINT32 *ChecksumData, 
    IN     UINT32 LChecksumPosition, 
    IN     UINT32 RChecksumPosition);

//---------------------------------------------------------------------------

EFI_HARDWARE_CHANGE_PROTOCOL gHardwareChangeProtocol = 
{
    SetHardwareConfigData,
    GetHardwareConfigDataFromNvram,
    SignalHardwareSignatureChange
};

/**
  Entry point of HardwareSignature.

  This function is a driver entry point that installs a protocol to set the  
  hardware configurations. These hardware configurations will determine the  
  hardware ignature of the FACS table.  

  This function also creates ReadyToBoot event to update the hardware 
  signature of the FACS table. 

  @param[in]  ImageHandle    Image handle
  @param[in]  SystemTable     Pointer to system table

  @retval EFI_SUCCESS          Function executed successfully.
  @retval Other                       Function executed unsuccessfully.

**/
EFI_STATUS 
EFIAPI 
HardwareSignatureEntryPoint (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{

    EFI_STATUS          Status;
    EFI_EVENT           ReadyToBootEvent;
    EFI_HANDLE          HardwareChangeHandle = NULL;
    UINT8               UsedProtocolCount;
    VOID                *Registration;
  
    InitAmiLib(ImageHandle,SystemTable);

    //clear the global variable first
    MemSet(&gCurHardwareConfigData, sizeof(HW_CONFIG_DATA),0);

    //Initial ReturnChecksum
    for(UsedProtocolCount=0;UsedProtocolCount<LIMIT_PROTOCOL_CALLED;UsedProtocolCount++)
        gCurHardwareConfigData.ReturnChecksum[UsedProtocolCount] = 0;

    Status = gBS->InstallProtocolInterface(
                      &HardwareChangeHandle,
                      &gHardwareChangeProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &gHardwareChangeProtocol
                      );

    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) 
        return Status;

      RegisterProtocolCallback(
              &gBeforeBootProtocolGuid,
              UpdateHardwareSignatureData,
              NULL, 
              &ReadyToBootEvent, 
              &Registration
              );

      RegisterProtocolCallback(
              &gBeforeLegacyBootProtocolGuid,
              UpdateHardwareSignatureData,
              NULL, 
              &ReadyToBootEvent, 
              &Registration
              );    
    return Status;
}

/**
  Set hardware config data.

  This function sets the hardware configuration data for each configuration
  type.

  @param[in]  ConfigType                   the hardware configuration type 
  @param[in]  BufferSize                    Size of buffer for recording the configuration data.
  @param[in]  Buffer                          Buffer for recording the config data.

  @retval EFI_SUCCESS                      Set the hardware configuration data successfully.
  @retval EFI_INVALID_PARAMETER    ConfigType is incorrect. Buffer is NULL and BufferSize is not zero.

**/
EFI_STATUS 
EFIAPI
SetHardwareConfigData (
    IN HW_CONFIG_TYPE ConfigType, 
    IN UINTN          BufferSize,
    IN VOID           *Buffer )
{
    EFI_STATUS Status;

    if( (Buffer == NULL && BufferSize != 0) || ConfigType >= MaxConfigType )
        return EFI_INVALID_PARAMETER;
  
    switch(ConfigType)
    {
#if DETECT_PS2_KEYBOARD     
        case Ps2KeyboardConfigType:
            Status = DetectAndSetPs2KeyboardConfig(BufferSize, Buffer);
            break;
#endif          
#if DETECT_PS2_MOUSE 
        case Ps2MouseConfigType:
            Status = DetectAndSetPs2MouseConfig(BufferSize, Buffer);
            break;   
#endif          
        case MemorySizeConfigType:
            Status = DetectAndSetMemorySizeConfig(BufferSize, Buffer);
            break;
          

        case MemoryMapConfigType:
            Status = DetectAndSetMemoryMapConfig(BufferSize, Buffer);
            break;
          
        case PciConfigType:
            Status = DetectAndSetPciConfig(BufferSize, Buffer);
            break;
#if HARDWARE_SIGNATURE_USB_CHANGE          
        case UsbConfigType:
            Status = DetectAndSetUsbConfig(BufferSize, Buffer);
            break;
#endif
        case VideoConfigType:
            Status = DetectAndSetVideoConfig(BufferSize, Buffer);
            break;
        case DiskConfigType:
            Status = DetectAndSetDiskConfig(BufferSize, Buffer);
            break;   
        default:
            Status = EFI_INVALID_PARAMETER;
            break;
    }
      
    return Status;
}

/**
  Get the hardware configuration data.

  This function gets the hardware configuration data from NVRAM.
  
  @param[in,out]  HardwareConfigData           The hardware configuration data of NVRAM.

  @retval              EFI_SUCCESS                    Get the hardware configuration data successfully.
  @retval              EFI_INVALID_PARAMETER   HardwareConfigData is NULL.
  @retval              EFI_NOT_FOUND                It is the first boot. There is no Hardware configuration data.

**/
EFI_STATUS 
EFIAPI
GetHardwareConfigDataFromNvram(
    IN OUT HW_CONFIG_DATA *HardwareConfigData )
{
    EFI_STATUS          Status;
    UINTN               VarSize = sizeof(HW_CONFIG_DATA);

    if(HardwareConfigData == NULL)
        return EFI_INVALID_PARAMETER;
    
    Status = gRT->GetVariable(
            HARDWARE_CONFIG_DATA_VARIABLE, 
            &gHardwareConfigDataGuid,
            NULL, 
            &VarSize, 
            HardwareConfigData ); 

    if( EFI_ERROR(Status) )
    {
        MemSet(HardwareConfigData,sizeof(HW_CONFIG_DATA),0);
    }
    
    return Status;    
}


/**
  Provide a protocol for other modules setting return value. 

  This function will update the global variable 'gExternalFunCallCount' and
  save the checksum value in the global variable
  gCurHardwareConfigData(ReturnChecksum array).

  The global variable gExternalFunCallCount will be increased.

  @param[in]  ReturnChecksum         The checksum value that external function gives.

  @retval EFI_SUCCESS                     Record the checksum which external function gives successfully
  @retval EFI_OUT_OF_RESOURCES    The global variable 'gExternalFunCallCount' is larger than 
                                                       and equal to LIMIT_PROTOCOL_CALLED.
  
**/
EFI_STATUS
EFIAPI
SignalHardwareSignatureChange ( 
    IN UINT32 ReturnChecksum )
{
    EFI_STATUS Status = EFI_SUCCESS;

    if (gExternalFunCallCount >= LIMIT_PROTOCOL_CALLED)
        return EFI_OUT_OF_RESOURCES;
    else
    {
        gCurHardwareConfigData.ReturnChecksum[gExternalFunCallCount]= ReturnChecksum;   
        gExternalFunCallCount++;
        return Status;
    }   
}

#if DETECT_PS2_KEYBOARD 
/**
  Set Ps2Keyboard state

  This function sets the PS2 Keyboard configurations to global variable 
  gCurHardwareConfigData.
  
  @param[in]  Present   If valuse is TRUE, PS2 Keyboard is present. 
                                   If valuse is FALSE, PS2 Keyboard is not present.   

  @retval         None.
  
**/
VOID 
SetPs2KeyboardState (
    IN BOOLEAN Present )
{
    if( Present )
    {    
        gHardwareConfigChangeFlag |= HW_CHANGE_PS2_KEYBOARD; 

        gCurHardwareConfigData.BitPs2Keyboard = TRUE;
    }
    else 
    {   
        gHardwareConfigChangeFlag |= HW_CHANGE_PS2_KEYBOARD; 
        //no PS2 Keyboard  
        gCurHardwareConfigData.BitPs2Keyboard = FALSE;
    }    
}
#endif

#if DETECT_PS2_MOUSE 
/**
  Set Ps2Mouse state

  This function sets the PS2 Mouse configurations to global variable 
  gCurHardwareConfigData.
  
  @param[in]  Present   If valuse is TRUE, PS2 Mouse is present. 
                                   If valuse is FALSE, PS2 Mouse is not present.   

  @retval        None.
  
**/
VOID 
SetPs2MouseState (
    IN BOOLEAN Present )
{
    if( Present )
    {    
        gHardwareConfigChangeFlag |= HW_CHANGE_PS2_MOUSE; 
        gCurHardwareConfigData.BitPs2Mouse = TRUE;
    }
    else 
    {
        gHardwareConfigChangeFlag |= HW_CHANGE_PS2_MOUSE;     
        //no PS2 Mouse
        gCurHardwareConfigData.BitPs2Mouse = FALSE;
    }
}
#endif
/**
  Set memory size config state.
  
  This function sets the memory size configurations to global variable 
  gCurHardwareConfigData.

  @param[in]  MemorySizeMb                    Current memory size (MB)         
  @param[in]  BufferSize                           Size of buffer for recording the memory size.

  @retval        EFI_SUCCESS                      Memory size config. sets successfully
  @retval        EFI_INVALID_PARAMETER    Buffer size is too small to record the memory size.
  
**/
EFI_STATUS 
SetMemorySizeConfigState (
    IN UINT32 MemorySizeMb, 
    IN UINTN  BufferSize )
{

    if(BufferSize < sizeof(UINT32)){
        return EFI_INVALID_PARAMETER;
    }
    
    gHardwareConfigChangeFlag |= HW_CHANGE_MEMORY_SIZE_CONFIG; 

    gCurHardwareConfigData.MemMapData.MemoryMbSize = MemorySizeMb;
        
    return EFI_SUCCESS;
}

/**
  Set memory map config state.
  
  This function sets the memory map configurations to global variable 
  gCurHardwareConfigData.

  @param[in]  MemMapConfig   Current memory map data  
  
  @retval        None.
  
**/
VOID 
SetMemoryMapConfigState (
    IN HW_MEMORY_MAP_DATA MemMapConfig )
{    
    gHardwareConfigChangeFlag |= HW_CHANGE_MEMORY_MAP_CONFIG;        
    gCurHardwareConfigData.MemMapData.MemMapCheckSum = MemMapConfig.MemMapCheckSum;
}

/**
  Set PCI configuration state.
  
  This function sets the PCI configurations to global variable 
  gCurHardwareConfigData.

  @param[in]  PciConfig   Current PCI data                        
  
  @retval        None.
  
**/
VOID 
SetPciConfigState (
    IN HW_PCI_DATA PciConfig )
{    
    gHardwareConfigChangeFlag |= HW_CHANGE_PCI_CONFIG; 
       
    gCurHardwareConfigData.PciData.PciNum = PciConfig.PciNum;
    gCurHardwareConfigData.PciData.BdfChecksum = PciConfig.BdfChecksum;
    gCurHardwareConfigData.PciData.VidDidChecksum = PciConfig.VidDidChecksum;           
}

#if HARDWARE_SIGNATURE_USB_CHANGE

/**
  Set USB configuration state.
  
  This function sets the USB configurations to global variable 
  gCurHardwareConfigData.

  @param[in]  UsbConfig   Current USB data                        
  
  @retval         None.
  
**/
VOID 
SetUsbConfigState (
    IN HW_USB_DATA UsbConfig )
{    
    gHardwareConfigChangeFlag |= HW_CHANGE_USB_CONFIG; 
       
    gCurHardwareConfigData.UsbData.UsbNum = UsbConfig.UsbNum;
    gCurHardwareConfigData.UsbData.VidPidChecksum = UsbConfig.VidPidChecksum;
    gCurHardwareConfigData.UsbData.PortNumInterfaceNumChecksum = UsbConfig.PortNumInterfaceNumChecksum;
           
}
#endif

/**
  Set Video configuration state.
  
  This function sets the Video configurations to global variable 
  gCurHardwareConfigData.

  @param[in]  VideoConfig   Current Video data                        
  
  @retval        None.
  
**/
VOID 
SetVideoConfigState (
    IN HW_VIDEO_DATA VideoConfig )
{    
    gHardwareConfigChangeFlag |= HW_CHANGE_VIDEO_CONFIG; 
       
    gCurHardwareConfigData.VideoData.EdidCheckNum = VideoConfig.EdidCheckNum;
    gCurHardwareConfigData.VideoData.ResolutionChecksum = VideoConfig.ResolutionChecksum;
           
}

/**
  Set disk configuration state.
  
  This function sets the Disk configurations to global variable 
  gCurHardwareConfigData.

  @param[in]  DiskConfig   Current Disk data                        
  
  @retval        None.
  
**/
VOID 
SetDiskConfigState (
    IN HW_DISK_DATA DiskConfig )
{    
    gHardwareConfigChangeFlag |= HW_CHANGE_DISK_CONFIG; 
       
    gCurHardwareConfigData.DiskData.DiskInfoChecksum= DiskConfig.DiskInfoChecksum;
}
#if DETECT_PS2_KEYBOARD 
/**
  Detect and set Ps2Keyboard configuration.
  
  This function detects the PS2 Keyboard configurations and sets this  
  configuration to global variable gCurHardwareConfigData.

  If Buffer is NULL and BufferSize is 0, the function will save the 
  detected data to gCurHardwareConfigData. Otherwise,  it will compare the 
  input data and the  detected data to determine the PS2 Keyboard is 
  present or not.

  @param[in]  BufferSize   Size of buffer for recording the PS2 Keyboard 
                                       configuration. It should be one byte. 
                                  
  @param[in]  Buffer         Buffer for recording the PS2 Keyboard configuration. 
                                       If *Buffer is 1, it means the PS2 Keyboard is present. 
                                       If *Buffer is 0, the PS2 Keyboard is not present                                  
  
  @retval  EFI_SUCCESS    PS2 Keyboard configuration is set successfully.
  @retval  Other                Set PS2 Keyboard configuration failed.
  
**/
EFI_STATUS 
DetectAndSetPs2KeyboardConfig (
    IN UINTN BufferSize,
    IN VOID  *Buffer )
{
    EFI_STATUS          Status;
    BOOLEAN             BitPs2KeyboardPresent = FALSE;
    BOOLEAN             DetectPs2KeyboardPresent = FALSE;

    Status = DetectPs2KeyboardMouseState( Ps2KeyboardConfigType, 
                                          &DetectPs2KeyboardPresent);

    if( BufferSize == 0 && Buffer == NULL)
    {
        BitPs2KeyboardPresent = DetectPs2KeyboardPresent;
    }
    else
    {     
        if((*(BOOLEAN *)Buffer) == 0 && DetectPs2KeyboardPresent == 0)
            BitPs2KeyboardPresent = FALSE;        
        else
            BitPs2KeyboardPresent = TRUE;
    }

    SetPs2KeyboardState(BitPs2KeyboardPresent);

    return Status;
    
}
#endif

#if DETECT_PS2_MOUSE 
/**
  Detect and set Ps2Mouse configuration.
  
  This function detects the PS2 Mouse configurations and sets this  
  configuration to global variable gCurHardwareConfigData.

  If Buffer is NULL and BufferSize is 0, the function will save the 
  detected data to gCurHardwareConfigData. Otherwise,  it will compare the 
  input data and the  detected data to determine the PS2 Mouse is 
  present or not.

  @param[in]  BufferSize   Size of buffer for recording the PS2 Mouse 
                                       configuration. It should be one byte. 
                                  
  @param[in]  Buffer         Buffer for recording the PS2 Mouse configuration. 
                                       If *Buffer is 1, it means the PS2 Mouse is present. 
                                       If *Buffer is 0, the PS2 Mouse is not present                                  
  
  @retval  EFI_SUCCESS    PS2 Mouse configuration is set successfully.
  @retval  Other                Set PS2 Mouse configuration failed.
  
**/
EFI_STATUS 
DetectAndSetPs2MouseConfig (
    IN UINTN BufferSize,
    IN VOID  *Buffer )
{
    EFI_STATUS          Status;
    BOOLEAN             BitPs2MousePresent = FALSE;
    BOOLEAN             DetectPs2MousePresent = FALSE;

    Status = DetectPs2KeyboardMouseState( Ps2MouseConfigType, 
                                          &DetectPs2MousePresent);

    if( BufferSize == 0 && Buffer == NULL)
    {
        BitPs2MousePresent = DetectPs2MousePresent;
    }
    else
    {     
        if((*(BOOLEAN *)Buffer) == 0 && DetectPs2MousePresent == 0)
            BitPs2MousePresent = FALSE;        
        else
            BitPs2MousePresent = TRUE;
    }


    SetPs2MouseState(BitPs2MousePresent);

    return Status;
    
}
#endif
/**
  Detect and set MemorySize configuration.
  
  This function detects the memory size configurations and sets this  
  configuration to global variable gCurHardwareConfigData.

  If Buffer is NULL and BufferSize is 0, the function will save the 
  detected size(DetectMemSize) to gCurHardwareConfigData. Otherwise, it  
  will compare the input data(Buffer) and the detected size(DetectMemSize) 
  to determine which one should be saved. 

  @param[in]  BufferSize         Size of buffer for recording the memory size                                  
  @param[in]  Buffer               Buffer for recording the memory size.                                
  
  @retval        EFI_SUCCESS    Memory size configuration is set successfully.
  @retval        Other                Detect the memory size failed or set the memory size 
                                             configuration failed.
  
**/
EFI_STATUS 
DetectAndSetMemorySizeConfig (
    IN UINTN BufferSize,
    IN VOID  *Buffer )
{
    EFI_STATUS          Status;
    UINT32              CurrentMemSize= 0;
    UINTN               BuffSize = 0;
    UINT32              DetectMemSize = 0;
    BOOLEAN             BitDifferent = FALSE;
    HW_MEMORY_MAP_DATA  DetectMemMapConfigData;

    MemSet(&DetectMemMapConfigData, sizeof(HW_MEMORY_MAP_DATA), 0);

    Status = GetMemMapData (&DetectMemMapConfigData);

    if(EFI_ERROR(Status)) 
        return Status;

    DetectMemSize = DetectMemMapConfigData.MemoryMbSize;
      
    if( BufferSize == 0 && Buffer == NULL)
    {
        CurrentMemSize = DetectMemSize;
        BuffSize = sizeof (CurrentMemSize);
    }
    else
    {     
        BitDifferent = IsDifferentMemorySize( DetectMemSize, (*(UINT32 *)Buffer) );
      
        if(BitDifferent)
          CurrentMemSize = DetectMemSize; 
        else
          CurrentMemSize = (*(UINT32 *)Buffer);

        BuffSize = BufferSize;
    }

    Status = SetMemorySizeConfigState(CurrentMemSize, BuffSize);

    return Status;
    
}

/**
  Detect and set MemoryMap configuration.
  
  This function detects the memory map configurations and sets this  
  configuration to global variable gCurHardwareConfigData.

  If Buffer is NULL and BufferSize is 0, the function will save the 
  detected memory map data to gCurHardwareConfigData. Otherwise, it will 
  save the input data(Buffer).

  @param[in]  BufferSize                           Size of buffer for recording the memory map data.                                  
  @param[in]  Buffer                                 Buffer for recording the memory map data.                                
  
  @retval        EFI_SUCCESS                      Memory map configuration is set successfully.
  @retval        EFI_INVALID_PARAMETER    BufferSize is incorrect.
  @retval        Other                                  Get memory map data failed.
  
**/
EFI_STATUS 
DetectAndSetMemoryMapConfig (
    IN UINTN BufferSize,
    IN VOID  *Buffer )
{
    EFI_STATUS          Status;
    UINTN               BuffSize = 0;
    HW_MEMORY_MAP_DATA  DetectMemMapConfigData;
    HW_MEMORY_MAP_DATA  CurMemMapConfigData;
    
    MemSet(&DetectMemMapConfigData, sizeof(HW_MEMORY_MAP_DATA), 0);

    Status = GetMemMapData (&DetectMemMapConfigData);

    if(EFI_ERROR(Status)) 
        return Status;
      
    if( BufferSize == 0 && Buffer == NULL)
    {
        BuffSize = sizeof(DetectMemMapConfigData.MemMapCheckSum);       
        MemCpy(&CurMemMapConfigData, &DetectMemMapConfigData, BuffSize);
    }
    else
    {     
        BuffSize = sizeof(DetectMemMapConfigData.MemMapCheckSum);
        if(BufferSize < BuffSize || BufferSize > sizeof(HW_MEMORY_MAP_DATA))
        {
            Status = EFI_INVALID_PARAMETER;
            return Status;
        }
        
        BuffSize = BufferSize;
        MemCpy(&CurMemMapConfigData, Buffer, BuffSize);
    }

    SetMemoryMapConfigState(CurMemMapConfigData);

    return Status;
    
}

/**
  Detect and set PCI configuration.
  
  This function detects the PCI configurations and sets this  
  configuration to global variable gCurHardwareConfigData.

  If Buffer is NULL and BufferSize is 0, the function will save the 
  detected PCI data to gCurHardwareConfigData. Otherwise, it will save 
  the input data (Buffer). 

  @param[in]  BufferSize                           Size of buffer for recording the PCI data.                                  
  @param[in]  Buffer                                 Buffer for recording the PCI data.                                
  
  @retval        EFI_SUCCESS                      PCI configuration is set successfully.
  @retval        EFI_INVALID_PARAMETER    BufferSize is incorrect.
  @retval        Other                                  Get PCI data failed.
  
**/
EFI_STATUS 
DetectAndSetPciConfig (
    IN UINTN BufferSize,
    IN VOID  *Buffer )
{
    EFI_STATUS          Status;
    UINTN               BuffSize = 0;
    HW_PCI_DATA         DetectPciConfigData;
    HW_PCI_DATA         CurrentPciConfigData;

    Status = GetPciDeviceData (&DetectPciConfigData);
      
    if(EFI_ERROR(Status)) 
        return Status;
      
    if( BufferSize == 0 && Buffer == NULL)
    {
        BuffSize = sizeof(HW_PCI_DATA);
        MemCpy(&CurrentPciConfigData, &DetectPciConfigData, BuffSize);
    }
    else
    {     
        BuffSize = sizeof(HW_PCI_DATA);
        if(BufferSize != BuffSize )
        {
            Status = EFI_INVALID_PARAMETER;
            return Status;
        }
        
        MemCpy(&CurrentPciConfigData, Buffer, BuffSize);
    }

    SetPciConfigState(CurrentPciConfigData);

    return Status;
    
}
#if HARDWARE_SIGNATURE_USB_CHANGE

/**
  Detect and set USB configuration.
  
  This function detects the USB configurations and sets this  
  configuration to global variable gCurHardwareConfigData.

  If Buffer is NULL and BufferSize is 0, the function will save the 
  detected USB data to gCurHardwareConfigData. Otherwise, it will save 
  the input data (Buffer). 

  @param[in]  BufferSize                          Size of buffer for recording the USB data.                                  
  @param[in]  Buffer                                Buffer for recording the USB data.                                
  
  @retval        EFI_SUCCESS                    USB configuration is set successfully.
  @retval        EFI_INVALID_PARAMETER  BufferSize is incorrect.
  @retval        Other                                 Get USB data failed.
  
**/
EFI_STATUS 
DetectAndSetUsbConfig (
    IN UINTN BufferSize,
    IN VOID  *Buffer )
{
    EFI_STATUS          Status;
    UINTN               BuffSize = 0;
    HW_USB_DATA         DetectUsbConfigData;
    HW_USB_DATA         CurrentUsbConfigData;

    Status = GetUsbDeviceData (&DetectUsbConfigData);
      
    if(EFI_ERROR(Status)) 
        return Status;
      
    if( BufferSize == 0 && Buffer == NULL)
    {
        BuffSize = sizeof(HW_USB_DATA);
        MemCpy(&CurrentUsbConfigData, &DetectUsbConfigData, BuffSize);
    }
    else
    {     
        BuffSize = sizeof(HW_USB_DATA);
        if(BufferSize != BuffSize )
        {
            Status = EFI_INVALID_PARAMETER;
            return Status;
        }
        
        MemCpy(&CurrentUsbConfigData, Buffer, BuffSize);
    }

    SetUsbConfigState(CurrentUsbConfigData);

    return Status;
    
}

#endif

/**
  Detect and set Video configuration.
  
  This function detects the Video configurations and sets this  
  configuration to global variable gCurHardwareConfigData.

  If Buffer is NULL and BufferSize is 0, the function will save the 
  detected Video data to gCurHardwareConfigData. Otherwise, it will save 
  the input data (Buffer). 

  @param[in]  BufferSize                           Size of buffer for recording the Video data.                                  
  @param[in]  Buffer                                 Buffer for recording the Video data.                                
  
  @retval        EFI_SUCCESS                     Video configuration is set successfully.
  @retval        EFI_INVALID_PARAMETER   BufferSize is incorrect.
  @retval        Other                                 Get Video data failed.
  
**/
EFI_STATUS 
DetectAndSetVideoConfig (
    IN UINTN BufferSize,
    IN VOID  *Buffer )
{
    EFI_STATUS          Status;
    UINTN               BuffSize = 0;
    HW_VIDEO_DATA       DetectVideoData;
    HW_VIDEO_DATA       CurrentVideoData;  
    
    Status = GetVideoData( &DetectVideoData);
      
    if(EFI_ERROR(Status)) 
        return Status;
      
    if( BufferSize == 0 && Buffer == NULL)
    {
        BuffSize = sizeof(HW_VIDEO_DATA);
        MemCpy(&CurrentVideoData, &DetectVideoData, BuffSize);
    }
    else
    {     
        BuffSize = sizeof(HW_VIDEO_DATA);
        if(BufferSize != BuffSize )
        {
            Status = EFI_INVALID_PARAMETER;
            return Status;
        }
        
        MemCpy(&CurrentVideoData, Buffer, BuffSize);
    }

    SetVideoConfigState(CurrentVideoData);

    return Status;
    
}

/**
  Detect and set Disk configuration.
  
  This function detects the Disk configurations and sets this  
  configuration to global variable gCurHardwareConfigData.

  If Buffer is NULL and BufferSize is 0, the function will save the 
  detected Disk data to gCurHardwareConfigData. Otherwise, it will save 
  the input data (Buffer). 

  @param[in]  BufferSize                           Size of buffer for recording the Disk data.                                  
  @param[in]  Buffer                                 Buffer for recording the Disk data.                                
  
  @retval        EFI_SUCCESS                     Disk configuration is set successfully.
  @retval        EFI_INVALID_PARAMETER   BufferSize is incorrect.
  @retval        Other                                  Get Disk data failed.
  
**/
EFI_STATUS 
DetectAndSetDiskConfig (
    IN UINTN BufferSize,
    IN VOID  *Buffer)
{
    EFI_STATUS          Status;
    UINTN               BuffSize = 0;
    HW_DISK_DATA        DetectDiskData;
    HW_DISK_DATA        CurrentDiskData;  

    Status = GetDiskInfoData( &DetectDiskData);
      
    if(EFI_ERROR(Status)) 
        return Status;
      
    if( BufferSize == 0 && Buffer == NULL)
    {
        BuffSize = sizeof(HW_DISK_DATA);
        MemCpy(&CurrentDiskData, &DetectDiskData, BuffSize);
    }
    else
    {     
        BuffSize = sizeof(HW_DISK_DATA);
        if(BufferSize != BuffSize )
        {
            Status = EFI_INVALID_PARAMETER;
            return Status;
        }
        
        MemCpy(&CurrentDiskData, Buffer, BuffSize);
    }

    SetDiskConfigState(CurrentDiskData);

    return Status;
    
}

/**
  Save Hardware configuration data to NVRAM.
  
  This function writes the hardware configurations to NVRAM.

  @param[in]  HardwareConfigData             current HW configuration data                        
  
  @retval        EFI_SUCCESS                      HW configuration is written successfully
  @retval        EFI_INVALID_PARAMETER    HardwareConfigData is invalid
  
**/
EFI_STATUS 
SetHardwareConfigDataToNvram (
    IN HW_CONFIG_DATA *HardwareConfigData )
{
    EFI_STATUS          Status;
    UINTN               VarSize = sizeof(HW_CONFIG_DATA);

    if(HardwareConfigData == NULL)
        return EFI_INVALID_PARAMETER;
    
    Status = gRT->SetVariable(
            HARDWARE_CONFIG_DATA_VARIABLE, 
            &gHardwareConfigDataGuid,
            EFI_VARIABLE_NON_VOLATILE |
            EFI_VARIABLE_BOOTSERVICE_ACCESS |
            EFI_VARIABLE_RUNTIME_ACCESS,
            VarSize, 
            HardwareConfigData
            ); 
    
    return Status;    
}


/**
  GetFacsTable
  
  This function returns address of memory where FACS ACPI table resides

  @param[in,out]  AcpiFacsTable        address of FACS table
  
  @retval              EFI_SUCCESS        Table was found successfully
  @retval              EFI_NOT_READY    Can't find the EfiAcpiSupportGuid protocol
  @retval              EFI_NOT_FOUND    Can't find the FADT table 

**/
EFI_STATUS 
GetFacsTable ( 
    IN OUT EFI_PHYSICAL_ADDRESS *AcpiFacsTable )
{
    EFI_STATUS                Status;
    EFI_GUID                  EfiAcpiSupportGuid = EFI_ACPI_SUPPORT_GUID;
    EFI_ACPI_SUPPORT_PROTOCOL *AcpiSupport;
    EFI_ACPI_TABLE_VERSION    Version;
    FACP_20                   *Table = NULL;
    UINT8                     FacpFlag = 0;
    UINTN                     Index;
    UINTN                     Handle;    
    
    Status = gBS->LocateProtocol(&EfiAcpiSupportGuid, NULL, &AcpiSupport);
    if(EFI_ERROR(Status))
    {
        return EFI_NOT_READY;
    }

    for(Index = 0; Index < 3; Index++)
    {
        AcpiFacsTable[Index] = 0;
    }

    //Find DSDT, XSDT ACPI table
    Index = 0;
    do
    {
        Status = AcpiSupport->GetAcpiTable(AcpiSupport, 
                                           Index, 
                                           &Table, 
                                           &Version, 
                                           &Handle);
        if (EFI_ERROR(Status)) 
            break;//no more tables left

        if ((Table->Header.Signature == FACP_SIG))
        {
            if( Version == EFI_ACPI_TABLE_VERSION_1_0B )
            {
                AcpiFacsTable[0] = (EFI_PHYSICAL_ADDRESS)Table->FIRMWARE_CTRL;
                FacpFlag |= BIT0;
            }
            else
            {
                AcpiFacsTable[1] = (EFI_PHYSICAL_ADDRESS)Table->FIRMWARE_CTRL;
                AcpiFacsTable[2] = (EFI_PHYSICAL_ADDRESS)Table->X_FIRMWARE_CTRL;     
                FacpFlag |= BIT1;
            }
                        
        }

        if(FacpFlag == 3)
            break;
        
        Index++;
    }while(1);

    if(FacpFlag == 0)
        return EFI_NOT_FOUND;
       
    return EFI_SUCCESS;
}

/**
  SwapChecksum
  
   This function swaps the two input data.

  @param[in,out]  LChecksumPosition   Left checksum position
  @param[in,out]  RChecksumPosition   Right checksum position
  
  @retval              None.

**/
VOID 
SwapChecksum(
    IN OUT UINT32 *LChecksumPosition,
    IN OUT UINT32 *RChecksumPosition )
{
    UINT32 Temp = *LChecksumPosition;
    *LChecksumPosition = *RChecksumPosition;
    *RChecksumPosition = Temp;
}

/**
  QuickSort
  
  This function sorts the external checksum in ascending order.
  The sorted data will be saved in ChecksumData.

  @param[in,out]  ChecksumData          all checksum data
  @param[in]        LChecksumPosition    the first position of checksum data 
  @param[in]        RChecksumPosition   the last position of checksum data 
  
  @retval  None.

**/
VOID 
QuickSort(
    IN OUT UINT32 *ChecksumData, 
    IN     UINT32 LChecksumPosition, 
    IN     UINT32 RChecksumPosition )
{
    UINT32 Pivot; 
    UINT32 LeftIncPosition;
    UINT32 RightDecPostion;

    if (LChecksumPosition >= RChecksumPosition) { return; }

    Pivot = ChecksumData[LChecksumPosition];

    LeftIncPosition = LChecksumPosition + 1;
    RightDecPostion = RChecksumPosition;

    while (1)
    {
        while (LeftIncPosition <= RChecksumPosition)
        {
            if (ChecksumData[LeftIncPosition] > Pivot)
                break;
            LeftIncPosition = LeftIncPosition + 1;
        }

        while (RightDecPostion > LChecksumPosition)
        {
            if (ChecksumData[RightDecPostion] < Pivot)
                break;
            RightDecPostion = RightDecPostion - 1;
        }

        if (LeftIncPosition > RightDecPostion)
            break;

        SwapChecksum(&ChecksumData[LeftIncPosition], &ChecksumData[RightDecPostion]);
    }

    SwapChecksum(&ChecksumData[LChecksumPosition], &ChecksumData[RightDecPostion]);

    QuickSort(ChecksumData, LChecksumPosition, RightDecPostion - 1);
    QuickSort(ChecksumData, RightDecPostion + 1, RChecksumPosition);

}

/**
  Update HardwareSignature Data
  
  This function will be called when ReadyToBoot event will be signaled and 
  will update the hardware signature data of the FACS table.

   This function also writes the hardware configurations to NVRAM.   

  @param[in]  Event                  signaled event
  @param[in]  Context               calling context
  
  @retval        EFI_SUCCESS       Hardware Signature was updated successfully.
  @retval        EFI_NOT_READY   GetFacsTable() can't find the EfiAcpiSupportGuid protocol.
  @retval        EFI_NOT_FOUND   GetFacsTable() can't find the FADT table. 
  @retval        Other                    Hardware Signature could't be updated.

**/
EFI_STATUS 
EFIAPI
UpdateHardwareSignatureData (
    IN EFI_EVENT Event, 
    IN VOID      *Context )
{   
    EFI_STATUS                     Status;
    FACS_20                        *FacsPtr = NULL;    
    EFI_PHYSICAL_ADDRESS           AcpiFacsTable[3];
    UINT8                          Index =0;
    UINTN                          VarSize = sizeof(HW_CONFIG_DATA);
    BOOLEAN                        BitPs2MousePresent = FALSE;
    UINTN                          BufferSize = 0;
    UINT32                         FacsSignature = 0;
    UINT32                         SetupCount=0;
    UINTN                          NumberOfExternelFunCall;
    static UINT8                   UpdateCount = 0;
    EFI_TIME                       BiosBuildTime = { FOUR_DIGIT_YEAR_INT, \
                                                        TWO_DIGIT_MONTH_INT, \
                                                        TWO_DIGIT_DAY_INT, \
                                                        TWO_DIGIT_HOUR_INT, \
                                                        TWO_DIGIT_MINUTE_INT, \
                                                        TWO_DIGIT_SECOND_INT, \
                                                        0,0,0,0,0};
    
    if(UpdateCount == 0)//To update the signature only once
        UpdateCount++;
    else
        return EFI_SUCCESS;

    //clear table address
    for(Index = 0; Index < 3; Index++)
    {
        AcpiFacsTable[Index] = 0;
    }
    //Get Facs table address. (RSDT, XSDT)
    Status = GetFacsTable(&AcpiFacsTable[0]);
    
    if (EFI_ERROR(Status)) 
        return Status;

    SetHardwareConfigData( Ps2KeyboardConfigType,0,NULL );

    BufferSize = sizeof(BitPs2MousePresent); 
    SetHardwareConfigData( Ps2MouseConfigType,BufferSize,(VOID *)&BitPs2MousePresent );

    SetHardwareConfigData( MemorySizeConfigType,0,NULL);

    SetHardwareConfigData( MemoryMapConfigType,0,NULL);

    SetHardwareConfigData( PciConfigType,0,NULL);
        
#if HARDWARE_SIGNATURE_USB_CHANGE
    SetHardwareConfigData( UsbConfigType,0,NULL);
#endif
    SetHardwareConfigData( DiskConfigType,0,NULL);

    SetHardwareConfigData( VideoConfigType,0,NULL);
    
    HWSIG_TRACE((-1,"[-------Hardware Signature Debug Message----START------]\n"));   
    
    gCurHardwareConfigData.BiosBuildYear = BiosBuildTime.Year;
    gCurHardwareConfigData.BiosBuildMonth = BiosBuildTime.Month;
    gCurHardwareConfigData.BiosBuildDay = BiosBuildTime.Day;
    gCurHardwareConfigData.BiosBuildHour = BiosBuildTime.Hour;
    gCurHardwareConfigData.BiosBuildMinute = BiosBuildTime.Minute;
    gCurHardwareConfigData.BiosBuildSecond = BiosBuildTime.Second;
    
    TRACE((-1,"BIOS Build Date:%d/%d/%d Time:%d:%d:%d\n",gCurHardwareConfigData.BiosBuildYear, \
                                                          gCurHardwareConfigData.BiosBuildMonth,\
                                                          gCurHardwareConfigData.BiosBuildDay, \
                                                          gCurHardwareConfigData.BiosBuildHour,\
                                                          gCurHardwareConfigData.BiosBuildMinute,\
                                                          gCurHardwareConfigData.BiosBuildSecond));
      
    HWSIG_TRACE((-1,"bPs2KeyBoard: %x, bPs2Mouse:%x\n", gCurHardwareConfigData.BitPs2Keyboard, gCurHardwareConfigData.BitPs2Mouse));
    HWSIG_TRACE((-1,"MemMapCheckSum: %x\n", gCurHardwareConfigData.MemMapData.MemMapCheckSum));
    HWSIG_TRACE((-1,"MemorySize: %x\n", gCurHardwareConfigData.MemMapData.MemoryMbSize));
    HWSIG_TRACE((-1,"PCI#: %x, BDF: %x, VID:%x\n", gCurHardwareConfigData.PciData.PciNum, gCurHardwareConfigData.PciData.BdfChecksum, gCurHardwareConfigData.PciData.VidDidChecksum));
#if HARDWARE_SIGNATURE_USB_CHANGE
    HWSIG_TRACE((-1,"Usb#: %x,VID: %x, Port: %x\n", gCurHardwareConfigData.UsbData.UsbNum, gCurHardwareConfigData.UsbData.VidPidChecksum, gCurHardwareConfigData.UsbData.PortNumInterfaceNumChecksum));
#endif
    HWSIG_TRACE((-1,"Edid Chk: 0x%x, Resolution Chk: 0x%x\n", gCurHardwareConfigData.VideoData.EdidCheckNum, gCurHardwareConfigData.VideoData.ResolutionChecksum));
    HWSIG_TRACE((-1,"DiskInfoChecksum: %x\n", gCurHardwareConfigData.DiskData.DiskInfoChecksum));

    Status = gRT->GetVariable(
            SETUP_UPDATE_COUNT_VARIABLE, 
            &gSetupUpdateCountGuid,
            NULL, 
            &VarSize, 
            &SetupCount ); 

    gCurHardwareConfigData.SetupUpdateCount = SetupCount;

    TRACE((-1,"Setup Update Count:%d\n",SetupCount));


    if(gExternalFunCallCount !=0)
    {
        for(NumberOfExternelFunCall=0;NumberOfExternelFunCall<gExternalFunCallCount;NumberOfExternelFunCall++)
            TRACE((-1,"Used Protocol Checksum:%x\n",gCurHardwareConfigData.ReturnChecksum[NumberOfExternelFunCall]));

        QuickSort( gCurHardwareConfigData.ReturnChecksum ,0 , gExternalFunCallCount-1 );

        for(NumberOfExternelFunCall=0;NumberOfExternelFunCall<gExternalFunCallCount;NumberOfExternelFunCall++)
            TRACE((-1,"After Sorting :%x\n",gCurHardwareConfigData.ReturnChecksum[NumberOfExternelFunCall]));
    }

    FacsSignature = Crc32Algorithm(&gCurHardwareConfigData.BitPs2Keyboard,(UINT16)(sizeof(HW_CONFIG_DATA)),0);
    HWSIG_TRACE((-1,"Update Current FACS Signature Variable: %X\n", FacsSignature));
    
    HWSIG_TRACE((-1,"[-------Hardware Signature Debug Message----END------]\n"));   

    //Update Hardware Signature of FACS table
    for(Index = 0; Index < 3; Index++)
    {        
        if(AcpiFacsTable[Index] == 0)
            continue;
        
        FacsPtr = (FACS_20 *)(UINTN)AcpiFacsTable[Index];
        FacsPtr->HardwareSignature = FacsSignature;        
    }
        
    gBS->CloseEvent(Event);

    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

