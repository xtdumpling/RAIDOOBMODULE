//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file NvmeDynamicSetup.c
    Updates Nvme setup page dynamically

**/

#include <Token.h>

//Library used
#include <Library/HiiLib.h>
#include <Library/AmiHiiUpdateLib.h>
#include <Guid/MdeModuleHii.h>
#include <Protocol/HiiString.h>
#include "NvmeDynamicSetup.h"
#include "Nvme/NvmeIncludes.h"
#include "Nvme/NvmeBus.h"
#include <AmiDxeLib.h>

//-------------------------------------------------------------------------
// Global Variable Definitions
//-------------------------------------------------------------------------

static EFI_HII_STRING_PROTOCOL        *HiiString = NULL;
// APTIOV_SERVER_OVERRIDE_RC_START : To Remove Duplicated NVME information displayed in Setup
static CHAR8                          *gSupportedLanguages = NULL;
// APTIOV_SERVER_OVERRIDE_RC_END : To Remove Duplicated NVME information displayed in Setup
static EFI_HII_HANDLE gNvmeHiiHandle = NULL;
// APTIOV_SERVER_OVERRIDE_RC_START : To Remove Duplicated NVME information displayed in Setup
LIST_ENTRY            gNvmeControllerList;
// APTIOV_SERVER_OVERRIDE_RC_END : To Remove Duplicated NVME information displayed in Setup

static EFI_GUID gNvmeFormSetGuid = NVME_FORM_SET_GUID;
// APTIOV_SERVER_OVERRIDE_RC_START : To Remove Duplicated NVME information displayed in Setup
//EFI_STRING_ID gNvmeControllerLocationStrIds[MAXIMUM_NVME_CONTROLLER_DISPLAY] = {0};
//EFI_STRING_ID gNvmeControllerNameStrIds[MAXIMUM_NVME_CONTROLLER_DISPLAY] = {0};
//EFI_STRING_ID gNvmeControllerSizeStrIds[MAXIMUM_NVME_CONTROLLER_DISPLAY] = {0};
//EFI_STRING_ID gNvmeControllerNotPresent = 0;
//EFI_STRING_ID gNvmeControllerNvmeSize = 0;
EFI_HII_CONFIG_ACCESS_PROTOCOL CallBack = { NULL, NULL, NvmeCallback };
CALLBACK_INFO SetupCallBack[] = {
    // Last field in every structure will be filled by the Setup
    { &gNvmeFormSetGuid, &CallBack, NVME_FORM_SET_CLASS, 0, 0},
};

/**
    Initializes NVME controller Text Item string ID

    @param  NvmeControllerInfo  Points to the NVME_CONTROLLER_INFORMATION.

    @return VOID

**/
VOID
InitializeNvmeItemStrId(
    NVME_CONTROLLER_INFORMATION    *NvmeControllerInfo
)
{
    NvmeControllerInfo->BasicInfo.ModelNumberStrId.Prompt = STRING_TOKEN(STR_NVME_MODEL_NUMBER);
    NvmeControllerInfo->BasicInfo.SizeStrId.Prompt = STRING_TOKEN(STR_NVME_SIZE);
    NvmeControllerInfo->BasicInfo.BusStrId.Prompt = STRING_TOKEN(STR_NVME_BUS_NUMBER);
    NvmeControllerInfo->BasicInfo.VendorStrId.Prompt = STRING_TOKEN(STR_NVME_VENDOR_ID);
    NvmeControllerInfo->BasicInfo.DeviceStrId.Prompt = STRING_TOKEN(STR_NVME_DEVICE_ID);

    return;
}

/**
    Add/Set the String to HII Database using HiiString Protocol

    @param HiiHandle   Handle of the HiiDatabase entry.
    @param String      Points to the string.
    @param StringId    points to the Id of the string.

    @return VOID

**/
VOID
NvmeHiiAddStringInternal (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  CHAR16 *        String,
    IN  EFI_STRING_ID   *StringId
)
{
// APTIOV_SERVER_OVERRIDE_RC_END : To Remove Duplicated NVME information displayed in Setup
    EFI_STATUS      Status;
    CHAR8*          Languages = NULL;
    UINTN           LangSize = 0;
    CHAR8*          CurrentLanguage;
    BOOLEAN         LastLanguage = FALSE;

    if(HiiString == NULL) {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) &HiiString);
        if(EFI_ERROR(Status)) {
// APTIOV_SERVER_OVERRIDE_RC_START : To Remove Duplicated NVME information displayed in Setup
            return;
        }
    }
    if (gSupportedLanguages == NULL) {
// APTIOV_SERVER_OVERRIDE_RC_END : To Remove Duplicated NVME information displayed in Setup
        Status = HiiString->GetLanguages(HiiString, HiiHandle, Languages, &LangSize);
        if(Status == EFI_BUFFER_TOO_SMALL) {
            Status = pBS->AllocatePool(EfiBootServicesData, LangSize, (VOID**)&Languages);
            if(EFI_ERROR(Status)) {
                //
                //not enough resources to allocate string
                //
                return;
// APTIOV_SERVER_OVERRIDE_RC_END : To Remove Duplicated NVME information displayed in Setup
            }
            Status = HiiString->GetLanguages(HiiString, HiiHandle, Languages, &LangSize);
            if(EFI_ERROR(Status)) {
// APTIOV_SERVER_OVERRIDE_RC_START : To Remove Duplicated NVME information displayed in Setup
                return;
            }
        }
        gSupportedLanguages=Languages;
    } else {
        Languages=gSupportedLanguages;
    }
// APTIOV_SERVER_OVERRIDE_RC_END : To Remove Duplicated NVME information displayed in Setup
    while(!LastLanguage) {
        //
        //point CurrentLanguage to start of new language
        //
        CurrentLanguage = Languages;
        while(*Languages != ';' && *Languages != 0)
            Languages++;

        //
        //Last language in language list
        //
        if(*Languages == 0) {
            LastLanguage = TRUE;
// APTIOV_SERVER_OVERRIDE_RC_START : To Remove Duplicated NVME information displayed in Setup
            if (*StringId == 0) {
                Status = HiiString->NewString(HiiString, HiiHandle, StringId, CurrentLanguage, NULL, String, NULL);
            } else {
                Status = HiiString->SetString(HiiString, HiiHandle, *StringId, CurrentLanguage, String, NULL);
            }
            if(EFI_ERROR(Status)) {
                return;
// APTIOV_SERVER_OVERRIDE_RC_END : To Remove Duplicated NVME information displayed in Setup
            }
        } else {
            //
            //put null-terminator
            //
            *Languages = 0;
// APTIOV_SERVER_OVERRIDE_RC_START : To Remove Duplicated NVME information displayed in Setup
            if (*StringId == 0) {
                Status = HiiString->NewString(HiiString, HiiHandle, StringId, CurrentLanguage, NULL, String, NULL);
            } else {
                Status = HiiString->SetString(HiiString, HiiHandle, *StringId, CurrentLanguage, String, NULL);
// APTIOV_SERVER_OVERRIDE_RC_END : To Remove Duplicated NVME information displayed in Setup
            }
            *Languages = ';';       //restore original character
            Languages++;
            if(EFI_ERROR(Status)) {
// APTIOV_SERVER_OVERRIDE_RC_START : To Remove Duplicated NVME information displayed in Setup
                return;
            }
        }
    }
//   return StringId;
// APTIOV_SERVER_OVERRIDE_RC_END : To Remove Duplicated NVME information displayed in Setup
}
// APTIOV_SERVER_OVERRIDE_RC_START : To Remove Duplicated NVME information displayed in Setup
/**
    Add the String to HII Database using HiiString Protocol

    @param HiiHandle   Handle of the HiiDatabase entry.
    @param String      Pointer to the string.
    @param StringId    pointer to the Id of the string.

    @return VOID

**/
VOID
NvmeHiiAddString (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  CHAR16 *        String,
    IN  EFI_STRING_ID    *StringId
)
{
    NvmeHiiAddStringInternal(HiiHandle, String, StringId);
}
#if 0
/**
    Set the String to HII Database using HiiString Protocol

    @param HiiHandle
    @param String
    @param StringId

    @retval VOID

**/
EFI_STRING_ID
NvmeHiiSetString (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  CHAR16 *        String,
    IN  EFI_STRING_ID   StringId
)
{
    return NvmeHiiAddStringInternal(HiiHandle, String, StringId);
}

/**
    This function will dynamically add VFR contents to Nvme BIOS setup page
    using HII library functions.

    @param VOID

    @retval VOID

**/
VOID
NvmeInitDynamicMainFormContents(
    VOID
)
{
    VOID                                *StartOpCodeHandle;
    VOID                                *EndOpCodeHandle;
    EFI_IFR_GUID_LABEL                  *StartLabel;
    EFI_IFR_GUID_LABEL                  *EndLabel;
    UINTN                               Index;
    CHAR16                              *String = NULL;
    EFI_STATUS                          Status;
    UINTN                               HandleCount;
    EFI_HANDLE                          *HandleBuffer;
    UINTN                               HandleCountPciIo;
    EFI_HANDLE                          *HandleBufferPciIo;
    EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
    EFI_DEVICE_PATH_PROTOCOL            *DevicePathNode;
    EFI_PCI_IO_PROTOCOL                 *PciIO;
    AMI_NVME_CONTROLLER_PROTOCOL        *NvmeController = NULL;
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
    UINT8                               DriveIndex = 0;
    ACTIVE_NAMESPACE_DATA               *ActiveNameSpace;
    LIST_ENTRY                          *LinkData;
    UINT64                              NameSpaceSizeInBytes = 0;
    UINTN                               RemainderInBytes = 0;
    UINT32                              NameSpaceSizeInGB = 0;
    UINTN                               NumTenthsOfGB = 0;
    
    DEBUG((EFI_D_ERROR,"\nNvmeInitDynamicMainFormContents Entry \n"));

    StartOpCodeHandle = HiiAllocateOpCodeHandle ();
    EndOpCodeHandle = HiiAllocateOpCodeHandle ();
    
    if(StartOpCodeHandle == NULL || EndOpCodeHandle == NULL) {
        return;
    }
    
    // Create Hii Extended Label OpCode as the start and end opcode
    StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
            StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));

    EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
            EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    
    if(StartLabel == NULL || EndLabel == NULL) {
        return;
    }
    
    StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
    EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    StartLabel->Number = NVME_MAIN_FORM_LABEL_START;
    EndLabel->Number = NVME_MAIN_FORM_LABEL_END;
    
    // Collect all DevicePath protocol and check for NVMe Controller
    Status = pBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiDevicePathProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                 );
    
    if (EFI_ERROR(Status)) {
        return;
    }
    
    for (Index = 0; (Index < HandleCount)&&(DriveIndex < MAXIMUM_NVME_CONTROLLER_DISPLAY); Index++) {
        
        Status = pBS->HandleProtocol(
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
        Status = pBS->LocateHandleBuffer(
                     ByProtocol,
                     &gEfiPciIoProtocolGuid,
                     NULL,
                     &HandleCountPciIo,
                     &HandleBufferPciIo
                     );
        
        for (IndexPciIo = 0; IndexPciIo < HandleCountPciIo; IndexPciIo++) {
            Status = pBS->HandleProtocol(
                          HandleBufferPciIo[IndexPciIo],
                          &gEfiPciIoProtocolGuid,
                          (VOID *)&PciIO
                          );
                    
            ASSERT_EFI_ERROR(Status);
            if(EFI_ERROR(Status)) {
                continue;
            }
            
            Status = pBS->HandleProtocol(
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
            Status = pBS->OpenProtocolInformation(
                          HandleBufferPciIo[IndexPciIo],
                          &gEfiPciIoProtocolGuid,
                          &OpenInfo,
                          &OpenInfoCount
                          );
            
            ASSERT_EFI_ERROR(Status);
            if(EFI_ERROR(Status)) {
                continue;
            }                
            
            for (IndexOpenInfo = 0;( IndexOpenInfo < OpenInfoCount)&&(DriveIndex < MAXIMUM_NVME_CONTROLLER_DISPLAY); IndexOpenInfo++) {
                
                //Check if the handle is opened BY_CHILD and also compare the device handle.
                if ((OpenInfo[IndexOpenInfo].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) &&
                        (OpenInfo[IndexOpenInfo].ControllerHandle == DeviceHandle)){
                
                    DriverBindingHandle = OpenInfo[IndexOpenInfo].AgentHandle;
                    // Get the handle for the Controller
                    ControllerHandle = HandleBufferPciIo[IndexPciIo]; 
                    
                    // Now PCI controller and DriverBinding handle is found. Get the Component2Protocol now.
                    Status = pBS->HandleProtocol(
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
                    
                    Status = pBS->AllocatePool( EfiBootServicesData,
                                       (UINTN)50,
                                       (VOID**)&String );
                    ASSERT_EFI_ERROR(Status);
                    
                    gNvmeControllerNameStrIds[DriveIndex] = NvmeHiiAddString(gNvmeHiiHandle, DriveNameUni);
                    
                    pBS->SetMem(String, (UINTN)50, 0);
                                          
                    Status = PciIO->GetLocation (
                                     PciIO,
                                     &SegmentNumber,
                                     &BusNumber,
                                     &DeviceNumber,
                                     &FunctionNumber
                                     );
                    ASSERT_EFI_ERROR(Status);
      
                    Swprintf(String, L"Bus:%X Dev:%X Func:%X", BusNumber, DeviceNumber, FunctionNumber);
                    gNvmeControllerLocationStrIds[DriveIndex] = NvmeHiiAddString(gNvmeHiiHandle, String);
                    
                    pBS->SetMem(String, (UINTN)50, 0);
                    NumTenthsOfGB=0;
                    NameSpaceSizeInGB=0;
                    for (LinkData = NvmeController->ActiveNameSpaceList.ForwardLink; 
                                LinkData != &NvmeController->ActiveNameSpaceList; 
                                LinkData = LinkData->ForwardLink) {
                            
                        ActiveNameSpace = BASE_CR(LinkData ,ACTIVE_NAMESPACE_DATA, Link);
                    
                        // NameSpaceSize In Bytes
                        NameSpaceSizeInBytes = Mul64((UINT64)ActiveNameSpace->IdentifyNamespaceData.NSIZE,\
                                                   ActiveNameSpace->NvmeBlockIO.Media->BlockSize);
                        
                        // NameSpaceSizeInGB is NameSpaceSizeInBytes / 1 GB (1 Decimal GB = 10^9 bytes)
                        NameSpaceSizeInGB += (UINT32) Div64(NameSpaceSizeInBytes, 1000000000, &RemainderInBytes);
                     
                        // Convert the Remainder, which is in bytes, to number of tenths of a Decimal GB.
                        NumTenthsOfGB += RemainderInBytes / 100000000;
                       
                     }

                     // Display Total Size of Nvme (All ActiveNameSpace Size) in GB
                    Swprintf(String, L"%d.%dGB", NameSpaceSizeInGB, NumTenthsOfGB);
                    gNvmeControllerSizeStrIds[DriveIndex] = NvmeHiiAddString(gNvmeHiiHandle, String);
                    
                    DriveIndex++;
                    
                    if (String!= NULL) {
                        pBS->FreePool(String);
                    }
                }
            }
            pBS->FreePool(OpenInfo);              
        }
        pBS->FreePool(HandleBufferPciIo);
    }

    pBS->FreePool(HandleBuffer);
    
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_NVME_SUBTITLE_STRING),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_EMPTY),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    
    if (DriveIndex == 0) {
        Status = pBS->AllocatePool( EfiBootServicesData,
                           (UINTN)50,
                           (VOID**)&String );
        ASSERT_EFI_ERROR(Status);
        
        Swprintf(String, L"No NVME Device Found");
        gNvmeControllerNotPresent = NvmeHiiAddString(gNvmeHiiHandle, String);
        
        HiiCreateSubTitleOpCode(StartOpCodeHandle,
                gNvmeControllerNotPresent,
                STRING_TOKEN(STR_EMPTY),
                0,0);
        
        if (String!= NULL) {
            pBS->FreePool(String);
        }
    } else {
        
        Status = pBS->AllocatePool( EfiBootServicesData,
                                   (UINTN)50,
                                   (VOID**)&String );
        ASSERT_EFI_ERROR(Status);
        
        Swprintf(String, L"Nvme Size");
        gNvmeControllerNvmeSize = NvmeHiiAddString(gNvmeHiiHandle, String);
        
        for(Index = 0; Index < DriveIndex; Index++) {
            // Updating NVME controller information dynamically
            HiiCreateTextOpCode(StartOpCodeHandle,
                    gNvmeControllerLocationStrIds[Index],
                    STRING_TOKEN(STR_EMPTY),
                    gNvmeControllerNameStrIds[Index]);
            
            HiiCreateTextOpCode(StartOpCodeHandle,
                    gNvmeControllerNvmeSize,
                    STRING_TOKEN(STR_EMPTY),
                    gNvmeControllerSizeStrIds[Index]);
            
            HiiCreateSubTitleOpCode(StartOpCodeHandle,
                    STRING_TOKEN(STR_EMPTY),
                    STRING_TOKEN(STR_EMPTY),
                    0,0);
        }
        if (String!= NULL) {
            pBS->FreePool(String);
        }
    }
    
    // We are done!! Updating form
    HiiUpdateForm (
      gNvmeHiiHandle,
      &gNvmeFormSetGuid,
      NVME_MAIN_FORM_ID,
      StartOpCodeHandle,
      EndOpCodeHandle
    );
    
    if (StartOpCodeHandle != NULL) {
        HiiFreeOpCodeHandle (StartOpCodeHandle);
    }
    
    if (EndOpCodeHandle != NULL) {
        HiiFreeOpCodeHandle (EndOpCodeHandle);
    }
    
    DEBUG((EFI_D_ERROR,"\nNvmeInitDynamicMainFormContents Exit \n"));
}
/**
    This function initializes the Nvme IFR contents.

    @param EFI_EVENT Event - Event to signal
    @param VOID* Context - Event specific context

    @retval VOID

**/

VOID
EFIAPI
InitNvmeDynamicIfrContents (
        IN EFI_EVENT Event,
        IN VOID *Context
)
{
    NvmeInitDynamicMainFormContents();
}
#endif

/**
  Nvme HiiCreateTextOpCode

  @param   StartOpCodeHandle  Points to the start opcode handle.
  @param   TextItem			  Points to the NVME_TEXT_ITEM.
  @param   ItemCnt			  Number of items to be connected.
    
  @return  VOID
**/
VOID
NvmeHiiCreateTextOpCode(
    IN VOID             *StartOpCodeHandle,    
    IN NVME_TEXT_ITEM   *TextItem,
    IN UINT32           ItemCnt
)
{
    UINT32 i = 0;
    
    for(i=0;i<ItemCnt;i++) {
        if((TextItem->Prompt != 0) || (TextItem->TextTwo !=0)) {
            HiiCreateTextOpCode(StartOpCodeHandle,
                                TextItem->Prompt,
                                TextItem->Help,
                                TextItem->TextTwo);
        }
        TextItem++;
    }
}

/**
  This function will return the Controller Name for Nvme to display
  in Setup, by matching PciRoot UID and Hardware device path of Nvme 
  Controller with information ported through Elink 
  NvmeControllerNamelist.
    
  @param  NvmeCtrlDevPath Pointer to DevicePath of NvmeController  

  @return OemNvmeName     Pointer contains OEM Name for NvmeController

**/
CHAR8*
GetNvmeControllerName (EFI_DEVICE_PATH_PROTOCOL *NvmeCtrlDevPath)
{
    EFI_STATUS                    Status;
    PCI_DEVICE_PATH               *PciDpNode;
    UINT8                         IndexElink;
    UINT8                         IndexPciNode;
    UINT8                         Length = 0;
    CHAR8                         *OemNvmeName = NULL;
    CHAR8                         *String = NULL;
    NVME_CONTROLLER_NAME_LIST     NvmeControllerNamelist[] = { NVME_CONTROLLER_NAME_ELINK_LIST
                                                            { {0x0},{{0xFF,0xFF}}, NULL }
                                                            };
	
    ACPI_HID_DEVICE_PATH *AcpiDp = (ACPI_HID_DEVICE_PATH *)NvmeCtrlDevPath;
    
    // Check whether the device path given through Elink is valid.  
    if (NvmeControllerNamelist[0].PciDevFunInfo[0].Dev != 0xFF){
       
        for (IndexElink=0;IndexElink < MAXIMUM_NVME_CONTROLLER_DISPLAY; IndexElink++) {
            
            //Match the PciRoot UID from AcpiDevPath and the ported UID value from NvmeControllerNamelist ELINK.           
            if (NvmeControllerNamelist[IndexElink].PciRootUID == AcpiDp->UID) {
                
                //From NvmeCtrlDevPath, get the first node of PCI Hardware device path 	
                while(!isEndNode (NvmeCtrlDevPath))	{
                    if ((NvmeCtrlDevPath->Type == HARDWARE_DEVICE_PATH) &&
                            (NvmeCtrlDevPath->SubType == HW_PCI_DP)) {
                        break;
                    }
                    NvmeCtrlDevPath = NEXT_NODE(NvmeCtrlDevPath);
                }
                //Match the Hardware DevicePath obtained for Nvme controller among all DevicePath ported through Elink(NvmeControllerNamelist)  . 
                //If proper match found, then loads the OemNvmeName with the ControllerName ported through Elink.
                IndexPciNode = 0;
                PciDpNode = (PCI_DEVICE_PATH*)NvmeCtrlDevPath;
                do {
                    if ((NvmeControllerNamelist[IndexElink].PciDevFunInfo[IndexPciNode].Dev == PciDpNode->Device) && 
                            (NvmeControllerNamelist[IndexElink].PciDevFunInfo[IndexPciNode].Fun == PciDpNode->Function)){ 
                    
                        IndexPciNode++;
                        PciDpNode = (PCI_DEVICE_PATH*)NEXT_NODE(&PciDpNode->Header);
                        
                    } else {
                        break;
                    }
                } while((NvmeControllerNamelist[IndexElink].PciDevFunInfo[IndexPciNode].Dev != 0xFF) && (PciDpNode->Header.Type == HARDWARE_DEVICE_PATH));
                
                //Checks whether there is no more Hardware DevicePath is available from DevicePath  and Elink for Matching
                if((NvmeControllerNamelist[IndexElink].PciDevFunInfo[IndexPciNode].Dev == 0xFF)&& (PciDpNode->Header.Type != HARDWARE_DEVICE_PATH)) {
                    OemNvmeName = NvmeControllerNamelist[IndexElink].ControllerName;
                    
                    while (*OemNvmeName != '\0') {
                        Length++;
                        OemNvmeName++;
                    }
                    Status = pBS->AllocatePool( EfiBootServicesData,(UINTN)(Length + 1),(VOID **)&OemNvmeName); 
                    if (EFI_ERROR(Status)){
                        DEBUG((DEBUG_ERROR,"Error in AllocatePool for OemNvmeName. Status = %r \n",Status)); 
                    }
                    pBS->SetMem(OemNvmeName,(Length + 1),0);
                    pBS->CopyMem(OemNvmeName,(NvmeControllerNamelist[IndexElink].ControllerName),(Length + 1));
                }
            } 
        } // End of for loop
    } 
    return  OemNvmeName;
}

/**
  Collects details about connected NVMe controllers

  @param   VOID

  @retval  EFI_SUCEESS   controller details are collected and updated successfully.
  @return  Other         Return error status.

**/

EFI_STATUS 
GetNvmeDeviceDetails(VOID) {
    EFI_STATUS                            Status;
    UINTN                                 HandleCount;
    EFI_HANDLE                            *HandleBuffer;
    UINTN                                 Index, i;
    EFI_PCI_IO_PROTOCOL                   *PciIO;
    NVME_CONTROLLER_INFORMATION           *NvmeControllerInfo = NULL;
    CHAR16                                *String = NULL; 
    CHAR8                                 *DeviceString = NULL;
    UINT32                                PciVidDid;
    AMI_NVME_CONTROLLER_PROTOCOL          *NvmeController = NULL;
    UINT64                                NameSpaceSizeInBytes = 0;
    UINTN                                 RemainderInBytes = 0;
    UINT32                                NameSpaceSizeInGB = 0;
    UINT32                                TotalNameSpaceSizeInGB = 0;
    UINTN                                 NumTenthsOfGB = 0;
    UINTN                                 TotalNumTenthsOfGB = 0;
    NVME_TEXT_ITEM                        *NvmeTextItem = NULL;
    ACTIVE_NAMESPACE_DATA                 *ActiveNameSpaceData;
    LIST_ENTRY                            *LinkData;
    UINT32                                Nsid = 1;
  
    
    Status = pBS->AllocatePool(EfiBootServicesData, 0x100, (VOID**)&String);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }
       
    Status = pBS->AllocatePool( EfiBootServicesData, 0x100, (VOID**)&DeviceString );
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) {
        return Status;
    }
    
    // Initializing the List to store the Nvme device details
    InitializeListHead(&gNvmeControllerList);
    
    // Locates all NVMe controller handles
    Status = pBS->LocateHandleBuffer(
                         ByProtocol,
                         &gAmiNvmeControllerProtocolGuid,
                         NULL,
                         &HandleCount,
                         &HandleBuffer
                        );
    
    if (EFI_ERROR(Status)) {
        return Status;
    }
           
    for (Index = 0; Index < HandleCount; Index++) {
        Status = pBS->HandleProtocol(
                               HandleBuffer[Index],
                               &gAmiNvmeControllerProtocolGuid,
                               (VOID *)&NvmeController
                               );
                 
        ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)) {
            continue;
        }
         
        Status = pBS->HandleProtocol(
                              HandleBuffer[Index],
                              &gEfiPciIoProtocolGuid,
                              (VOID *)&PciIO
                              );
                            
        ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)) {
            continue;
        }
         
        // Allocating memory for storing the Controller details
        Status = pBS->AllocatePool (EfiBootServicesData,
                                          sizeof(NVME_CONTROLLER_INFORMATION),
                                          (VOID**)&NvmeControllerInfo
                                           );
        if(EFI_ERROR(Status)) {
            continue;
        }
        
        pBS->SetMem(NvmeControllerInfo, sizeof(NVME_CONTROLLER_INFORMATION), 0);
         
        Status = pBS->AllocatePool (EfiBootServicesData,
                                     sizeof(NVME_TEXT_ITEM) * NvmeController->IdentifyControllerData->NN,
                                     (VOID**)&NvmeControllerInfo->ActiveNameSpace
                                     );
        if(EFI_ERROR(Status)) {
            continue;
        }
                  
        pBS->SetMem(NvmeControllerInfo->ActiveNameSpace, sizeof(NVME_TEXT_ITEM) * NvmeController->IdentifyControllerData->NN, 0);
         
        InitializeNvmeItemStrId(NvmeControllerInfo);
        
        NvmeControllerInfo->NvmeHandle = HandleBuffer[Index];
        
        // Get Model Number
        pBS->SetMem(DeviceString, (UINTN)0x100, 0);
        pBS->CopyMem(DeviceString, NvmeController->IdentifyControllerData->ModelNumber, 40);
                 
        // Remove the spaces from the end of the device name
        DeviceString[40] =0;
        for (i = 39; i > 0; i-- ) {
            if (DeviceString[i] != 0x20) { 
                break;
            }
            DeviceString[i] = 0;
        }
        pBS->SetMem(String, (UINTN)0x100, 0);
        Swprintf(String, L"%a", DeviceString);
        NvmeHiiAddString(gNvmeHiiHandle, String, &NvmeControllerInfo->BasicInfo.ModelNumberStrId.TextTwo);
                 
        Status = PciIO->GetLocation ( PciIO,
                                      &NvmeControllerInfo->SegNum,
                                      &NvmeControllerInfo->BusNum,
                                      &NvmeControllerInfo->DevNum,
                                      &NvmeControllerInfo->FunNum
                                      );
        ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)) {
            continue;
        }
         
        // Get  SegNum, Bus, Device, Function Number
        Swprintf(String, L"%02X:%02X:%02X:%02X", NvmeControllerInfo->SegNum,NvmeControllerInfo->BusNum, NvmeControllerInfo->DevNum, NvmeControllerInfo->FunNum);
        NvmeHiiAddString(gNvmeHiiHandle, String, &NvmeControllerInfo->BasicInfo.BusStrId.TextTwo);
                 
        // Retrieves Nvme controllers PCI Vendor ID and Device ID of the controller.
        Status = PciIO->Pci.Read (
                                 PciIO,
                                 EfiPciIoWidthUint32,
                                 PCI_VID_DID,
                                 1,
                                 &PciVidDid
                                 );
        if(EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            continue;
        }
         
        NvmeControllerInfo->VendorId = (UINT16)(PciVidDid & 0xffff);
        NvmeControllerInfo->DeviceId = (UINT16)(PciVidDid >> 16);
         
        // Publish Vendor ID & Device ID
        Swprintf(String, L"%04X", NvmeControllerInfo->VendorId);
        NvmeHiiAddString(gNvmeHiiHandle, String, &NvmeControllerInfo->BasicInfo.VendorStrId.TextTwo);
            
        Swprintf(String, L"%04X", NvmeControllerInfo->DeviceId);
        NvmeHiiAddString(gNvmeHiiHandle, String, &NvmeControllerInfo->BasicInfo.DeviceStrId.TextTwo);
         
        NvmeTextItem = (NVME_TEXT_ITEM *)NvmeControllerInfo->ActiveNameSpace;
         
         for (LinkData = NvmeController->ActiveNameSpaceList.ForwardLink; 
                     LinkData != &NvmeController->ActiveNameSpaceList; LinkData = LinkData->ForwardLink) {
               
            pBS->SetMem(String, 0x100, 0);
            ActiveNameSpaceData = BASE_CR(LinkData ,ACTIVE_NAMESPACE_DATA, Link);
            
            // NameSpaceSize In Bytes
            NameSpaceSizeInBytes = Mul64((UINT64)ActiveNameSpaceData->IdentifyNamespaceData.NSIZE,\
                                             ActiveNameSpaceData->NvmeBlockIO.Media->BlockSize);
                                          
            // NameSpaceSizeInGB is NameSpaceSizeInBytes / 1 GB (1 Decimal GB = 10^9 bytes)
            NameSpaceSizeInGB = (UINT32) Div64(NameSpaceSizeInBytes, 1000000000,\
                                                                  &RemainderInBytes);
            TotalNameSpaceSizeInGB += NameSpaceSizeInGB;
                                      
            // Convert the Remainder, which is in bytes, to number of tenths of a Decimal GB.
            NumTenthsOfGB = RemainderInBytes / 100000000;
            TotalNumTenthsOfGB += NumTenthsOfGB;
            
           
            Swprintf(String, L"Namespace: %d", ActiveNameSpaceData->ActiveNameSpaceID);
            NvmeHiiAddString(gNvmeHiiHandle, String, &NvmeTextItem->Prompt);
            
            Swprintf(String, L"Size: %d.%d GB", NameSpaceSizeInGB, NumTenthsOfGB);
            NvmeHiiAddString(gNvmeHiiHandle, String, &NvmeTextItem->TextTwo);
            
            NvmeTextItem++;
            Nsid++;
         }
            
        // Active Namespace count
        NvmeControllerInfo->NamespaceCount = Nsid - 1;
            
        Swprintf(String, L"%d.%d GB", TotalNameSpaceSizeInGB, TotalNumTenthsOfGB);
        NvmeHiiAddString(gNvmeHiiHandle, String, &NvmeControllerInfo->BasicInfo.SizeStrId.TextTwo);
        TotalNameSpaceSizeInGB = 0;
        TotalNumTenthsOfGB = 0;
        
        InsertTailList(&gNvmeControllerList, &NvmeControllerInfo->NvmeControllerLink);
    }
            
    if(HandleBuffer != NULL) {
        pBS->FreePool(HandleBuffer);
    }
            
    if(DeviceString != NULL) {
        pBS->FreePool(DeviceString);
    }
             
    if(String != NULL) {
        pBS->FreePool(String);
    }
             
    return Status;
}

/**
  This function will dynamically add VFR contents to HDD Acoustic BIOS setup page
  using HII library functions.

  @param  ControllerNum  Number of the controller to which the data has to be
            			 updated.

  @return VOID

**/
VOID
NvmeInitDynamicSubFormContents(
    UINT16      ControllerNum
)
{
    VOID                            *StartOpCodeHandle;
    VOID                            *EndOpCodeHandle;
    EFI_IFR_GUID_LABEL              *StartLabel;
    EFI_IFR_GUID_LABEL              *EndLabel;
    UINT16                          Count = 0;
    NVME_CONTROLLER_INFORMATION     *NvmeControllerInfo = NULL;
    LIST_ENTRY                      *Link;
  
    StartOpCodeHandle = HiiAllocateOpCodeHandle ();
    EndOpCodeHandle = HiiAllocateOpCodeHandle ();
	
    // Create Hii Extended Label OpCode as the start and end opcode
    StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
	            StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));

    EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
 	            EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
	    
    if(StartLabel == NULL || EndLabel == NULL) {
        return;
    }
	    
    StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
    EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    StartLabel->Number = NVME_SUB_FORM_LABEL_START;
    EndLabel->Number = NVME_SUB_FORM_LABEL_END;
   
    // Traverse linked list to get controller data
    for (Link = gNvmeControllerList.ForwardLink; Link != &gNvmeControllerList; Link = Link->ForwardLink) {
        NvmeControllerInfo = BASE_CR(Link, NVME_CONTROLLER_INFORMATION, NvmeControllerLink );
        if(Count == ControllerNum) {
            break;
        }
        Count++;
    }
       
    if(NvmeControllerInfo == NULL) {
        return;
    }
   
    // Publish Basic Info
    NvmeHiiCreateTextOpCode(StartOpCodeHandle, 
                           (NVME_TEXT_ITEM *)&(NvmeControllerInfo->BasicInfo),
                           (UINT32)(sizeof(NVME_BASIC_INFO)/sizeof(NVME_TEXT_ITEM)));
   
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
                           STRING_TOKEN(STR_EMPTY),
                           STRING_TOKEN(STR_EMPTY),
                           0,0);
   
    NvmeHiiCreateTextOpCode(StartOpCodeHandle, 
                           (NVME_TEXT_ITEM *)NvmeControllerInfo->ActiveNameSpace,
                           (UINT32)NvmeControllerInfo->NamespaceCount);
	    
    // We are done!!
    HiiUpdateForm (
	     		gNvmeHiiHandle,
	            &gNvmeFormSetGuid,
	            NVME_SUB_FORM_ID,
	            StartOpCodeHandle,
	            EndOpCodeHandle
	            );

    if (StartOpCodeHandle != NULL) {
        HiiFreeOpCodeHandle (StartOpCodeHandle);
        HiiFreeOpCodeHandle (EndOpCodeHandle);
    }
	
    return;

}


/**
  This function will dynamically add VFR contents to Nvme BIOS setup page
  using HII library functions.

  @param  Event    Event whose notification function is being invoked.
  @param  Context  pointer to the notification function's context.

  @return VOID

**/
VOID
NvmeInitDynamicMainFormContents(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS                         Status;
    LIST_ENTRY                         *Link;
    NVME_CONTROLLER_INFORMATION        *NvmeControllerInfo = NULL;
    UINTN                              Count = 0;
    CHAR16                             *String = NULL;
    EFI_STRING_ID                      StringId = 0;
    
    VOID                                *StartOpCodeHandle;
    VOID                                *EndOpCodeHandle;
    EFI_IFR_GUID_LABEL                  *StartLabel;
    EFI_IFR_GUID_LABEL                  *EndLabel;
    EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
    CHAR8                               *ControllerName = NULL;
    
    DEBUG((DEBUG_INFO,"\nNvmeInitDynamicMainFormContents Entry \n"));

    StartOpCodeHandle = HiiAllocateOpCodeHandle ();
    EndOpCodeHandle = HiiAllocateOpCodeHandle ();
   
    if(StartOpCodeHandle == NULL || EndOpCodeHandle == NULL) {
        return;
    }
   
    // Create Hii Extended Label OpCode as the start and end opcode
    StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
                 StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));

    EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
               EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
   
    if(StartLabel == NULL || EndLabel == NULL) {
        return;
    }
   
    StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
    EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    StartLabel->Number = NVME_MAIN_FORM_LABEL_START;
    EndLabel->Number = NVME_MAIN_FORM_LABEL_END;
    
    Status = pBS->AllocatePool( EfiBootServicesData,
                                       (UINTN)50,
                                       (VOID**)&String );
    ASSERT_EFI_ERROR(Status);
    if( EFI_ERROR(Status) ) {
        return;
    }
   
    // Collect details about connected NVMe controllers
    Status = GetNvmeDeviceDetails();
    if(!EFI_ERROR(Status)) {
       for (Link = gNvmeControllerList.ForwardLink; Link != &gNvmeControllerList; Link = Link->ForwardLink, Count++) {
           NvmeControllerInfo = BASE_CR(Link, NVME_CONTROLLER_INFORMATION, NvmeControllerLink );
           
           
           Status = pBS->HandleProtocol(
                                      NvmeControllerInfo->NvmeHandle,
                                      &gEfiDevicePathProtocolGuid,
                                      (VOID *)&DevicePath
                                      );
           ASSERT_EFI_ERROR(Status);
                 
           //Gets the OEM name for Nvme Controller
           ControllerName = GetNvmeControllerName (DevicePath);
           
           if(ControllerName){
               Swprintf(String, L"%a",ControllerName);
               NvmeHiiAddString(gNvmeHiiHandle, String, &StringId);
           }
           
           if(NvmeControllerInfo->BasicInfo.ModelNumberStrId.TextTwo != 0) {
               // Add goto control
               HiiCreateGotoOpCode (
                   StartOpCodeHandle,
                   NVME_SUB_FORM_ID,
                   ControllerName ? StringId : NvmeControllerInfo->BasicInfo.ModelNumberStrId.TextTwo,
                   STRING_TOKEN(STR_EMPTY),
                   EFI_IFR_FLAG_CALLBACK,
                   (EFI_QUESTION_ID)(NVME_DEVICE_BASE_KEY + Count)
                   );
           }
           
           if(ControllerName != NULL){
               pBS->FreePool(ControllerName);
               ControllerName = NULL;
               StringId = 0;
           }
           
       }
    } else {
       
        Swprintf(String, L"No NVME Device Found");
        NvmeHiiAddString(gNvmeHiiHandle, String, &StringId);
        HiiCreateSubTitleOpCode(StartOpCodeHandle,
                                StringId,
                                STRING_TOKEN(STR_EMPTY),
                                0,0);
        StringId = 0;
    }
       
    // We are done!! Updating form
    HiiUpdateForm (
                  gNvmeHiiHandle,
                  &gNvmeFormSetGuid,
                  NVME_MAIN_FORM_ID,
                  StartOpCodeHandle,
                  EndOpCodeHandle
                  );
    
    if (String!= NULL) {
        pBS->FreePool(String);
    }
   
    if (StartOpCodeHandle != NULL) {
        HiiFreeOpCodeHandle (StartOpCodeHandle);
        HiiFreeOpCodeHandle (EndOpCodeHandle);
    }
   
    DEBUG((DEBUG_INFO,"\nNvmeInitDynamicMainFormContents Exit \n"));
    
    return;
}

/**
  This function is called by Setup browser to perform callback

  @param This             Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action           Specifies the type of action taken by the browser.
  @param QuestionId       A unique value which is sent to the original exporting driver
                          so that it can identify the type of data to expect.
  @param Type             The type of value for the question.
  @param Value            A pointer to the data being sent to the original exporting driver.
  @param ActionRequest    On return, points to the action requested by the callback function.

  
  @retval EFI_SUCCESS     The callback successfully handled the action.
  @retval EFI_UNSUPPORTED The action taken by the browser is not supported.

**/
EFI_STATUS 
NvmeCallback(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
)
{
	UINT16      ControllerNum;
	
    if (Action != EFI_BROWSER_ACTION_CHANGING) {
        return EFI_UNSUPPORTED; 
    }
    
    if((KeyValue >= 0x100) && (KeyValue < 0x200)) {
        ControllerNum = KeyValue - NVME_DEVICE_BASE_KEY;        
        NvmeInitDynamicSubFormContents(ControllerNum);
    }
	   
    return EFI_SUCCESS;
}


/**
    This function is the entry point. Registers callback on TSE event to update IFR data.

    @param  EFI_HANDLE          ImageHandle
    @param  EFI_SYSTEM_TABLE    *SystemTable

    @retval Status

**/
// APTIOV_SERVER_OVERRIDE_RC_END : To Remove Duplicated NVME information displayed in Setup
EFI_STATUS
EFIAPI
NvmeDynamicSetupEntryPoint (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS      Status;
    VOID            *SetupRegistration;
    EFI_GUID        SetupEnterGuid = AMITSE_SETUP_ENTER_GUID;
    EFI_EVENT       SetupEnterEvent;
    
    InitAmiLib(ImageHandle, SystemTable);
// APTIOV_SERVER_OVERRIDE_RC_START : To Remove Duplicated NVME information displayed in Setup
    // Load setup page and update strings
    Status = LoadResources(ImageHandle, sizeof(SetupCallBack) / sizeof(CALLBACK_INFO), SetupCallBack, NULL);
    
    if (!EFI_ERROR(Status)) {
        RegisterProtocolCallback(
        &SetupEnterGuid, NvmeInitDynamicMainFormContents,
        NULL, &SetupEnterEvent, &SetupRegistration
        );
    
    // Loads ImageHandle related strings
//    Status = LoadStrings( ImageHandle, &gNvmeHiiHandle );
//    ASSERT_EFI_ERROR(Status);
        gNvmeHiiHandle = SetupCallBack[0].HiiHandle;
    }    
// APTIOV_SERVER_OVERRIDE_RC_END : To Remove Duplicated NVME information displayed in Setup
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
