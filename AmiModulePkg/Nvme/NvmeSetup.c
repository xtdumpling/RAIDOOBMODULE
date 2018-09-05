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

/** @file NvmeSetup.c
    Display the Nvme Controller and device information in the Setup

**/

//---------------------------------------------------------------------------

#include <Setup.h>
#include "NvmeIncludes.h"
#include "Nvme/NvmeBus.h"
#include "AmiProtocol.h"
#include "AmiLib.h"
#include "NvmeStaticSetup.h"

//---------------------------------------------------------------------------

#define     MSG_NVME_DP     23
#define     DRIVEMAXCOUNT   4

extern EFI_RUNTIME_SERVICES     *pRS;

NVME_DEVICE_MANAGER             gNvmeDeviceManagerVariable;

typedef struct {
    STRING_REF  ControllerString;
    STRING_REF  DeviceString;
    STRING_REF  Size;
} NVME_SETUP_STRUC;

NVME_SETUP_STRUC   NVMeSetupStruc[] = {
    {STRING_TOKEN(STR_NVME0_CONTROLLER), STRING_TOKEN(STR_NVME0_NAME),STRING_TOKEN(STR_NVME0_SIZE)},
    {STRING_TOKEN(STR_NVME1_CONTROLLER), STRING_TOKEN(STR_NVME1_NAME),STRING_TOKEN(STR_NVME1_SIZE)},
    {STRING_TOKEN(STR_NVME2_CONTROLLER), STRING_TOKEN(STR_NVME2_NAME),STRING_TOKEN(STR_NVME2_SIZE)},
    {STRING_TOKEN(STR_NVME3_CONTROLLER), STRING_TOKEN(STR_NVME3_NAME),STRING_TOKEN(STR_NVME3_SIZE)}
};

extern EFI_GUID gAmiNvmeControllerProtocolGuid;
EFI_GUID gNvmeDeviceManagerGuid = NVME_DEVICE_MANAGER_GUID;

/**
    This function initializes the SB related setup option values

    @param HiiHandle -- Handle to HII database
    @param Class     -- Indicates the setup class

    @retval VOID

**/

VOID
InitNvmeStrings (
    IN  EFI_HII_HANDLE  HiiHandle, 
    IN  UINT16          Class
)
{

    EFI_STATUS                          Status;
    UINTN                               Index;
    UINTN                               HandleCount;
    EFI_HANDLE                          *HandleBuffer;
    UINTN                               HandleCountPciIo;
    EFI_HANDLE                          *HandleBufferPciIo;
    EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
    EFI_DEVICE_PATH_PROTOCOL            *DevicePathNode;
    EFI_PCI_IO_PROTOCOL                 *PciIO;
    AMI_NVME_CONTROLLER_PROTOCOL    	*NvmeController = NULL;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *OpenInfo;
    UINTN                               OpenInfoCount;
    UINTN                               IndexPciIo;
    UINTN                               IndexOpenInfo;
    EFI_HANDLE                          DeviceHandle;
    EFI_HANDLE                          ControllerHandle;
    EFI_HANDLE                          DriverBindingHandle;
    EFI_COMPONENT_NAME2_PROTOCOL        *ComponentName2Protocol;
    CHAR16                              *DriveNameUni;
    UINTN                               SegmentNumber;
    UINTN                               BusNumber;
    UINTN                               DeviceNumber;
    UINTN                               FunctionNumber;
    UINT8                               DrivePresence[DRIVEMAXCOUNT] = {0};
    UINT8                               DriveIndex = 0;
    ACTIVE_NAMESPACE_DATA               *ActiveNameSpace;
    LIST_ENTRY                          *LinkData;
    CHAR16                              NvmeCardSize[30];
    UINT64                              NameSpaceSizeInBytes = 0;
    UINTN                               RemainderInBytes = 0;
    UINT32                              NameSpaceSizeInGB = 0;
    UINTN                               NumTenthsOfGB = 0;
    CHAR16                              NvmeSetupString[] = L"NvmeDriverManager";
    
    if (Class == ADVANCED_FORM_SET_CLASS) { 
        // Collect all DevicePath protocol and check for NVMe Controller
        Status = gBS->LocateHandleBuffer(
                      ByProtocol,
                      &gEfiDevicePathProtocolGuid,
                      NULL,
                      &HandleCount,
                      &HandleBuffer
                     );
        
        if (EFI_ERROR(Status)) return;
        
        for (Index = 0; (Index < HandleCount) && (DriveIndex < DRIVEMAXCOUNT); Index++) {
            
            Status = gBS->HandleProtocol(
                          HandleBuffer[Index],
                          &gEfiDevicePathProtocolGuid,
                          (VOID *)&DevicePath
                          );
            
             ASSERT_EFI_ERROR(Status);
             if(EFI_ERROR(Status)) {
                 continue;
             }
             
             DevicePathNode = DevicePath;
             while (!isEndNode (DevicePathNode)) {
                 if ((DevicePathNode->Type == MESSAGING_DEVICE_PATH) &&
                         (DevicePathNode->SubType == MSG_NVME_DP)) {
                     break;
                 }

                 DevicePathNode = NEXT_NODE(DevicePathNode);
            }
             
            if (DevicePathNode == NULL || isEndNode (DevicePathNode)) {
                continue;
            }
            
            // NVMe Device Handle is found.
            DeviceHandle = HandleBuffer[Index];
            
            // NVMe device is found. Now get the CONTROLLER. Check all the PCIio handles.
            Status = gBS->LocateHandleBuffer(
                         ByProtocol,
                         &gEfiPciIoProtocolGuid,
                         NULL,
                         &HandleCountPciIo,
                         &HandleBufferPciIo
                         );
            
            for (IndexPciIo = 0; IndexPciIo < HandleCountPciIo; IndexPciIo++) {
                Status = gBS->HandleProtocol(
                              HandleBufferPciIo[IndexPciIo],
                              &gEfiPciIoProtocolGuid,
                              (VOID *)&PciIO
                              );
                        
                ASSERT_EFI_ERROR(Status);
                if(EFI_ERROR(Status)) {
                    continue;
                }
                
                Status = gBS->HandleProtocol(
                              HandleBufferPciIo[IndexPciIo],
                              &gAmiNvmeControllerProtocolGuid,
                              (VOID *)&NvmeController
                              );
                
                // If Ami Nvme Controller Protocol not found on the Controller handle ( PciIo handle)
                // do not process the Pci Io Handle
                if(EFI_ERROR(Status)) {
                    continue;
                }
                
                OpenInfoCount = 0;
                Status = gBS->OpenProtocolInformation(
                              HandleBufferPciIo[IndexPciIo],
                              &gEfiPciIoProtocolGuid,
                              &OpenInfo,
                              &OpenInfoCount
                              );
                
                ASSERT_EFI_ERROR(Status);
                if(EFI_ERROR(Status)) {
                    continue;
                }
                               
                for (IndexOpenInfo = 0; (IndexOpenInfo < OpenInfoCount)&& (DriveIndex < DRIVEMAXCOUNT); IndexOpenInfo++) {// for 3
                    //Check if the handle is opened BY_CHILD and also compare the device handle.
                    if ((OpenInfo[IndexOpenInfo].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) &&
                            (OpenInfo[IndexOpenInfo].ControllerHandle == DeviceHandle)){
                    
                        DriverBindingHandle = OpenInfo[IndexOpenInfo].AgentHandle;
                        // Get the handle for the Controller
                        ControllerHandle = HandleBufferPciIo[IndexPciIo]; 
                        
                        // Now PCI controller and DriverBinding handle is found. Get the Component2Protocol now.
                        Status = gBS->HandleProtocol(
                                      DriverBindingHandle,
                                      &gEfiComponentName2ProtocolGuid,
                                      (VOID *)&ComponentName2Protocol
                                      );
                        
                        ASSERT_EFI_ERROR(Status);
                        if(EFI_ERROR(Status)) {
                            continue;
                        }     
                        
                        Status = ComponentName2Protocol->GetControllerName (
                                                         ComponentName2Protocol,
                                                         ControllerHandle,
                                                         DeviceHandle,
                                                         LANGUAGE_CODE_ENGLISH,
                                                         &DriveNameUni
                                                         );
                        
                        ASSERT_EFI_ERROR(Status);
                        if(EFI_ERROR(Status)) {
                            continue;
                        }     
                                                
                        InitString(
                                HiiHandle,
                                NVMeSetupStruc[DriveIndex].DeviceString,
                                L"%s", 
                                DriveNameUni
                                );
                                                
                                              
                         Status = PciIO->GetLocation (
                                         PciIO,
                                         &SegmentNumber,
                                         &BusNumber,
                                         &DeviceNumber,
                                         &FunctionNumber
                                         );
          
                         InitString(
                                 HiiHandle,
                                 NVMeSetupStruc[DriveIndex].ControllerString,
                                 L"Bus:%X Dev:%x Func:%x",
                                 BusNumber, DeviceNumber, FunctionNumber
                                 );
                         gBS->SetMem(NvmeCardSize,60,0);         
                         for (LinkData = NvmeController->ActiveNameSpaceList.ForwardLink; 
                                     LinkData != &NvmeController->ActiveNameSpaceList; 
                                     LinkData = LinkData->ForwardLink) {
                                 
                             ActiveNameSpace = BASE_CR(LinkData ,ACTIVE_NAMESPACE_DATA, Link);
                         
                             // NameSpaceSize In Bytes
                             
                             NameSpaceSizeInBytes = Mul64((UINT64)ActiveNameSpace->IdentifyNamespaceData.NSIZE,\
                            		                       ActiveNameSpace->NvmeBlockIO.Media->BlockSize);
                             
                             //NameSpaceSizeInGB is NameSpaceSizeInBytes / 1 GB (1 Decimal GB = 10^9 bytes)
                             
                             NameSpaceSizeInGB += (UINT32) Div64(NameSpaceSizeInBytes, 1000000000, &RemainderInBytes);
                          
                             //
                             //Convert the Remainder, which is in bytes, to number of tenths of a Decimal GB.
                             //
                             NumTenthsOfGB += RemainderInBytes / 100000000;
                            
                          }
                          //
                          //  Display Total Size of Nvme (All ActiveNameSpace Size) in GB
                          //
                         
                          Swprintf(NvmeCardSize, L"%d.%dGB", NameSpaceSizeInGB, NumTenthsOfGB);
                          InitString(HiiHandle, NVMeSetupStruc[DriveIndex].Size, L"%s", NvmeCardSize);
                          DrivePresence[DriveIndex] = 1;
                          DriveIndex++;
                    }
                }
                gBS->FreePool(OpenInfo);
            }
            gBS->FreePool(HandleBufferPciIo);
        }

        gBS->FreePool(HandleBuffer);
                
        // Update setup data to show whether strings need to be displayed or not
        gNvmeDeviceManagerVariable.DeviceCount = DriveIndex;
        for (DriveIndex = 0; DriveIndex < DRIVEMAXCOUNT; DriveIndex++) {
            gNvmeDeviceManagerVariable.ShowNvmeDrive[DriveIndex] = DrivePresence[DriveIndex];
        }
        Status = pRS->SetVariable ( NvmeSetupString,
                                    &gNvmeDeviceManagerGuid,
                                    (EFI_VARIABLE_BOOTSERVICE_ACCESS),
                                    sizeof(NVME_DEVICE_MANAGER),
                                    &gNvmeDeviceManagerVariable);
        ASSERT_EFI_ERROR (Status);
                            
    }

    return;
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
