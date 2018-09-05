//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//  Rev. 1.00
//    Bug fixed:   Add sSATA and ISATA into HDD information.
//    Reason:
//    Auditor:     Jacker Yeh (Refer from Grantley)
//    Date:        Oct/27/2016
//
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
/** @file CRBCspDxeLib.c

    This file contains code for CRB platform initialization in the Library stage
*/


#include <Token.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol\LegacyBiosPlatform.h>
#include <PCI.h>
#include <Protocol\PciIo.h>
#include <Protocol\IioUds.h>
#include <Guid\SetupVariable.h>
#include <Include\Register\PchRegsEva.h>
#include <Include\Register\PchRegsSata.h>

#include "BootOptions.h"
#include "BootOptioneLinks.h"
#include "Protocol\IdeControllerInit.h"

#include "Protocol\PciIo.h"


#include "Protocol\DiskInfo.h"
#include "Protocol\AmiAhciBus.h"

#include "Protocol\LegacyRegion2.h"
#include "Guid/SocketMpLinkVariable.h"
#define SOCKET_MP_LINK_CONFIGURATION_NAME L"SocketMpLinkConfig"
//#include "Protocol/PchPlatformPolicy.h"
#include <Library/S3BootScriptLib.h>

#include <AmiCspLib.h>

EFI_GUID  EfiLegacyRegion2ProtocolGuid = EFI_LEGACY_REGION2_PROTOCOL_GUID;
extern EFI_GUID gCompletePowerManagementProtocolGuid;

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

//
// GUID Definition(s)
//

//<AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:   SignalExitPmAuthProtocolEvent
//
// Description: The elink will signal gExitPmAuthProtocolGuid Event.
//
// Parameters:  VOID
//
//
// Returns:     None
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID SignalExitPmAuthProtocolEvent (
  VOID
)
{
  EFI_HANDLE  Handle = NULL;

  //
  // Signaling gExitPmAuthProtocolGuid Event
	//
  pBS->InstallProtocolInterface (
        &Handle,
        &gExitPmAuthProtocolGuid,
        EFI_NATIVE_INTERFACE,
        NULL
        );
}
// Changes to add CompletePowerManagementprotocol
VOID SignalCompletePowerManagementProtocolEvent (
  VOID
)
{
  EFI_HANDLE  Handle = NULL;

  //
  // Signaling gCompletePowerManagementProtocolGuid Event
  //
  pBS->InstallProtocolInterface (
        &Handle,
        &gCompletePowerManagementProtocolGuid,
        EFI_NATIVE_INTERFACE,
        NULL
        );
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:    OEMProgramIioSvidSid
//
// Description: Program OEM Sub system ID and Sub system Vendor ID for 
//				IIO devices.
//
// Input:
//  VOID
//
// Output:
//  VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
OEMProgramIioSvidSid (
  IN UINT8  IioNumber,
  IN UINT8  CoreBusNumber )
{
#if 0	
    UINTN                       Index;
    UINTN                       PciEAddressBase;
    UINTN                       SizeofTable;

    IIO_SVID_SID_INIT_ENTRY     OemIioSvidSidInitList[] = {
            //
            // Add Entries here.
            //
            { 0xFF, 0xFF, 0xFF, {0xFFFF, 0xFFFF} }
    };

    //
    // Calculate the Size of Table
    //
    SizeofTable = ( sizeof(OemIioSvidSidInitList)/sizeof(IIO_SVID_SID_INIT_ENTRY) );

    for ( Index = 0; Index<(SizeofTable-1); Index++ ) {

        //
        // Read PCIE Base Address
        //
        PciEAddressBase = MmPciAddress (
                            0,
                            CoreBusNumber,
                            OemIioSvidSidInitList[Index].DeviceNumber,
                            OemIioSvidSidInitList[Index].FunctionNumber,
                            0 );

        //
        // Check if the Device and Function is available by reading VID and DID.
        //
        if ( 0xFFFFFFFF == MmioRead32 (PciEAddressBase) ) {
            continue;
        }

        //
        // Program device Subsystem Vendor Identifier (SVID)
        //
        MmioWrite16 (
          (UINTN) (PciEAddressBase + OemIioSvidSidInitList[Index].SvidRegOffset),
          OemIioSvidSidInitList[Index].DefaultSvidSid.SubSystemVendorId
          );
        S3BootScriptSaveMemWrite (
          S3BootScriptWidthUint16,
          (UINTN) (PciEAddressBase + OemIioSvidSidInitList[Index].SvidRegOffset),
          1,
          (VOID *) (UINTN) (PciEAddressBase + OemIioSvidSidInitList[Index].SvidRegOffset)
          );

        //
        // Program device Subsystem Identifier (SID)
        //
        MmioWrite16 (
          (UINTN) (PciEAddressBase + OemIioSvidSidInitList[Index].SvidRegOffset + 2),
          OemIioSvidSidInitList[Index].DefaultSvidSid.SubSystemId
          );  
        S3BootScriptSaveMemWrite (
          S3BootScriptWidthUint16,
          (UINTN) (PciEAddressBase + OemIioSvidSidInitList[Index].SvidRegOffset + 2),
          1,
          (VOID *) (PciEAddressBase + OemIioSvidSidInitList[Index].SvidRegOffset + 2)
          );
    }
#endif    
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:    OEMProgramPchSvidSid
//
// Description: Program OEM Sub system ID and Sub system Vendor ID for 
//				PCH devices.
//
// Input:
//  VOID
//
// Output:
//  VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
OEMProgramPchSvidSid (
  VOID )
{
#if 0 //Revisit    
    UINTN                       Index;
    UINTN                       PciEAddressBase;
    UINTN                       SizeofTable;

    PCH_SVID_SID_INIT_ENTRY     OemPchSvidSidInitList[] = {
            //
            // Add Entries here.
            //
            { 0xFF, 0xFF, 0xFF, {0xFFFF, 0xFFFF} }
    };

    //
    // Calculate the Size of Table
    //
    SizeofTable = ( sizeof(OemPchSvidSidInitList)/sizeof(PCH_SVID_SID_INIT_ENTRY) );

    for ( Index = 0; Index<(SizeofTable-1); Index++ ) {

        //
        // Read PCIE Base Address
        //
        PciEAddressBase = MmPciAddress (
                            0,
                            0,
                            OemPchSvidSidInitList[Index].DeviceNumber,
                            OemPchSvidSidInitList[Index].FunctionNumber,
                            0
                            );

        //
        // Check if the Device and Function is available by reading VID and DID.
        //
        if ( 0xFFFFFFFF == MmioRead32 ( PciEAddressBase ) ) {
            continue;
        }

        //
        // Program device Subsystem Vendor Identifier (SVID)
        //
        MmioWrite16 (
          (UINTN) (PciEAddressBase + OemPchSvidSidInitList[Index].SvidRegOffset),
          OemPchSvidSidInitList[Index].DefaultSvidSid.SubSystemVendorId
          );
        S3BootScriptSaveMemWrite (
          S3BootScriptWidthUint16,
          (UINTN) (PciEAddressBase + OemPchSvidSidInitList[Index].SvidRegOffset),
          1,
          (VOID *) (UINTN) (PciEAddressBase + OemPchSvidSidInitList[Index].SvidRegOffset)
          );

        //
        // Program device Subsystem Identifier (SID)
        //
        MmioWrite16 (
          (UINTN) (PciEAddressBase + OemPchSvidSidInitList[Index].SvidRegOffset + 2),
          OemPchSvidSidInitList[Index].DefaultSvidSid.SubSystemId
          );  
        S3BootScriptSaveMemWrite (
          S3BootScriptWidthUint16,
          (UINTN) (PciEAddressBase + OemPchSvidSidInitList[Index].SvidRegOffset + 2),
          1,
          (VOID *) (PciEAddressBase + OemPchSvidSidInitList[Index].SvidRegOffset + 2)
          );
    }
#endif
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:    CRBGetPlatformVgaHandle
//
// Description: Returns Vga handle for the Platform. Onboard/Off-board Vga
//              is decided based upon user setting for Vga priority.
// Input:
//  This            Indicates the EFI_LEGACY_BIOS_PLATFORM_PROTOCOL instance.
//  Mode            Specifies what handle to return.
//                      GetVgaHandle
//  Type            Handle Modifier - Mode specific
//  HandleBuffer    Pointer to buffer containing all Handles matching the
//                  specified criteria. Handles are sorted in priority order.
//                  Type EFI_HANDLE is defined in InstallProtocolInterface()
//                  in the EFI 1.10 Specification.
//  HandleCount     Number of handles in HandleBuffer.
//  AdditionalData  Pointer to additional data returned - mode specific..
//
// Output:
//  EFI_SUCCESS The handle is valid.
//  EFI_UNSUPPORTED Mode is not supported on this platform.
//  EFI_NOT_FOUND   The handle is not known.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
CRBGetPlatformVgaHandle (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL     *This,
  IN  EFI_GET_PLATFORM_HANDLE_MODE          Mode,
  IN  UINT16                                Type,
  OUT EFI_HANDLE                            **HandleBuffer,
  OUT UINTN                                 *HandleCount,
  OUT VOID                                  OPTIONAL **AdditionalData
  )
{
    EFI_HANDLE              *hBuffer = NULL;
    EFI_HANDLE              hVga = NULL;
    EFI_HANDLE              hOffboardVideo = NULL;
    EFI_STATUS              Status;
    UINT8                   dData[4];
    UINT32                  BARAddrReg;
    UINT8                   BarIndex;
    UINT8                   Offset=0x10;
    BOOLEAN                 IsBARNonZero=FALSE;
    EFI_PCI_IO_PROTOCOL     *PciIo;
    EFI_STATUS              VgaStatus = EFI_NOT_FOUND;
    UINTN                   VariableSize = sizeof(SYSTEM_CONFIGURATION);
    UINTN                   SocketVariableSize = sizeof(SOCKET_MP_LINK_CONFIGURATION);
    UINTN                   n, Count, Seg, Bus, Dev, Fun;
    UINT8                   SocketNum, StackNum;
    BOOLEAN                 SocketStackNumFound = FALSE;
    UINT64                  PciAttributes;
    EFI_IIO_UDS_PROTOCOL    *IioUds;
    SYSTEM_CONFIGURATION    SystemConfiguration;
    SOCKET_MP_LINK_CONFIGURATION  SocketMpLinkConfiguration;
    UINT32                  VariableAttr, SocketVariableAttr;

    if (Mode == EfiGetPlatformVgaHandle) {
        Status = pRS->GetVariable(  L"IntelSetup",
                        &gEfiSetupVariableGuid,
                        &VariableAttr,
                        &VariableSize,
                        &SystemConfiguration
                        );
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR (Status)) {
            return Status;
        }
        Status = pRS->GetVariable( SOCKET_MP_LINK_CONFIGURATION_NAME,
                                &gEfiSocketMpLinkVariableGuid,
                                &SocketVariableAttr,
                                &SocketVariableSize,
                                &SocketMpLinkConfiguration
                                );
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR (Status)) {
            return Status;
        }
    
        //
        // Locate all PciIo handles
        //
        Status = pBS->LocateHandleBuffer (
                        ByProtocol,
                        &gEfiPciIoProtocolGuid,
                        NULL,
                        &Count,
                        &hBuffer);
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR (Status)) {
            return Status;
        }
        
        for (n = 0; n < Count; n++) {
            Status = pBS->HandleProtocol (
                            hBuffer[n],
                            &gEfiPciIoProtocolGuid,
                            &PciIo);           // Get PciIo protocol
            ASSERT_EFI_ERROR (Status);
            if (EFI_ERROR(Status)) break;
            Status = PciIo->Pci.Read(
                                PciIo,
                                EfiPciIoWidthUint32,
                                8,  // offset
                                1,  // width
                                dData);
            
            ASSERT_EFI_ERROR(Status);
            if (EFI_ERROR(Status)) break;
            //
            // Check for VGA-compatible Display controller
            // Base Class: 03h (Display Controller), Sub-Class: 00h (VGA-compatible controller)
            //
            if (dData[3] == PCI_CL_DISPLAY && dData[2] == PCI_CL_DISPLAY_SCL_VGA) {

                    for(BarIndex=0, IsBARNonZero=FALSE, Offset=0x10; BarIndex<6; BarIndex++,Offset+=4){
                        //Get Device BAR Address Values
                        Status = PciIo->Pci.Read(
                                            PciIo,
                                            EfiPciIoWidthUint32,
                                            Offset,  // offset
                                            1,  // width
                                            &BARAddrReg);
                        if(BARAddrReg & BIT0){
                            // Io Space: Mask BIT1:BIT0 and check BAR is Zero
                            if(BARAddrReg & 0xFFFFFFFC){
                                IsBARNonZero = TRUE;
                            }
                        }else{
                            //Memory Space: Mask BIT3:BIT0 and Check if BAR is ZERO
                            if(BARAddrReg & 0xFFFFFFF0){
                                IsBARNonZero = TRUE;
                            }
                            //Memory Space: Mask BIT2:BIT1 and Check if 64-Bit BAR
                            if((BARAddrReg & (BIT2 | BIT1)) == BIT2){
                                BarIndex++;
                                Offset+=4;
                                //Get Device BAR Address Values
                                Status = PciIo->Pci.Read(
                                                    PciIo,
                                                    EfiPciIoWidthUint32,
                                                    Offset,  // offset
                                                    1,  // width
                                                    &BARAddrReg);
                                //Memory Space: Check if upper 32-bit of BAR is not ZERO
                                if(BARAddrReg){
                                    IsBARNonZero = TRUE;
                                }
                            }
                        }
                    }
                    if((BarIndex == 6) && !IsBARNonZero){
                        DEBUG((EFI_D_INFO, "VGA Detected with No Resources allocated\n"));
                        continue;
                    }

                Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationGet, 0, &PciAttributes);
                VgaStatus = EFI_SUCCESS;
                if (EFI_ERROR(Status))
                    return Status;

                //
                // Check for on-board device(EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE is set).
                // If no onboard video is installed, and offboard video detected, then offboard VGA handle is returned.
                //
                if ((PciAttributes & EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE)) {
                    hVga = hBuffer[n];               	
                    if (SystemConfiguration.VideoSelect == 1) {// If user priority set to Onboard video, break the loop
                        break;
                    }
                }
                //
                // Handle only the 1st Offboard Video device identified.
                // If no offboard video is installed, then onboard VGA handle is returned when present.
                //
                if(hOffboardVideo == NULL && !(PciAttributes & EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE)) { //Offboard Video
                    hOffboardVideo = hBuffer[n];
                    if(SystemConfiguration.VideoSelect == 2) {
                        hVga = hOffboardVideo;
                        break; // If user priority set to Offboard Video, break the loop
                    }
                }
                
            } // if PCI_CL_DISPLAY
        } // end of for loop
        
        pBS->FreePool(hBuffer);
        
        if(VgaStatus != EFI_SUCCESS){
            **HandleBuffer = 0;
            *HandleCount = 0;
            return VgaStatus;
        }
        
        //
        // Set priority to offboard if Video Select is Auto
        //
        //
        if( (SystemConfiguration.VideoSelect == 0) && (hOffboardVideo != NULL) ) {
            hVga = hOffboardVideo;
        }
            
        // Check for offboard VGA handle when there is no onboard video and user sets priority to onboard video
        //
        if(hOffboardVideo != NULL && hVga == NULL) {
            hVga = hOffboardVideo;
        }
        
        Status = pBS->HandleProtocol (
                        hVga,
                        &gEfiPciIoProtocolGuid,
                        &PciIo);           // Get PciIo protocol
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status))
            return Status;

        Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fun);
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status))
            return Status;

        //
        // Get the Cpu No. of the assigned Vga controller
        //
        Status = pBS->LocateProtocol(
                        &gEfiIioUdsProtocolGuid,
                        NULL,
                        &IioUds
                        );
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status))
            return Status;
        
        //
        // Loop through all IIO stacks of all sockets that are present
        //
        for (SocketNum = 0; SocketNum < MAX_SOCKET; SocketNum++) {
            if (!IioUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].Valid) {
              continue;
            }
            //
            // #MultiSegment: Need to have condition check for Pci Segment if Multi segment is supported.
            // Currently, Multi segment is not supported so modify the code after the support is available.
            //
            //if( Seg != ) {
            //    continue;
            //}
            if( (Bus < IioUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].BusBase) || \
                (Bus > IioUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].BusLimit) ) {
                continue;
            }
            for (StackNum = 0; StackNum < MAX_IIO_STACK; StackNum++) {
                if (!((IioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNum].stackPresentBitmap) & (1 << StackNum))) {
                    continue;
                }
                //
                // #MultiSegment: Need to have condition check for Pci Segment if Multi segment is supported.
                // Currently, Multi segment is not supported so modify the code after the support is available.
                //
                //if( Seg != ) {
                //    continue;
                //}
                if( (Bus >= IioUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].StackRes[StackNum].BusBase) && \
                    (Bus <= IioUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].StackRes[StackNum].BusLimit) ) {
                    SocketStackNumFound = TRUE;
                    break;
                }
            }
            //
            // Break Socket loop if SocketStackNumFound is TRUE
            //
            if( SocketStackNumFound ) {
                break;
            }
        }
        if( SocketStackNumFound == FALSE ) {
            return EFI_NOT_FOUND;
        }
        
        **HandleBuffer = hVga;
        *HandleCount = 1;        
        
        //
        // Issue Cold Reset if the Socket or Stack number for Vga changes from previous boot
        //
        if( (SocketMpLinkConfiguration.LegacyVgaSoc != SocketNum) || \
            (SocketMpLinkConfiguration.LegacyVgaStack != StackNum) ) { 
            DEBUG((EFI_D_INFO, "Current: LegacyVgaSoc: %x LegacyVgaStack: %x\n", SocketMpLinkConfiguration.LegacyVgaSoc, SocketMpLinkConfiguration.LegacyVgaStack));
            SocketMpLinkConfiguration.LegacyVgaSoc = SocketNum;
            SocketMpLinkConfiguration.LegacyVgaStack = StackNum;
            DEBUG((EFI_D_INFO, "Updated: LegacyVgaSoc: %x LegacyVgaStack: %x\n", SocketMpLinkConfiguration.LegacyVgaSoc, SocketMpLinkConfiguration.LegacyVgaStack));
            Status = pRS->SetVariable (
                            SOCKET_MP_LINK_CONFIGURATION_NAME,
                            &gEfiSocketMpLinkVariableGuid,
                            SocketVariableAttr,
                            SocketVariableSize,
                            &SocketMpLinkConfiguration
                            );
            ASSERT_EFI_ERROR(Status);
            DEBUG((EFI_D_INFO, "Issue Cold Reset for change in VGA range target \n"));
            pRS->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
        }

        return VgaStatus;
   }
    else {
        return EFI_UNSUPPORTED;
    }
	
}

// Fucntion to get Controller Number
#ifdef CSM_SUPPORT
VOID 
GetControllerNumber (
    IN OUT UINTN                *gControllerNoAddr,
    IN UINT16                   wBusDevFunc
)
{
    UINT8       i;
    UINT32      Addr16=0;
    UINT16     Func;

    Func = wBusDevFunc & 0x07;

    for(i=0 ; i<MaxControllerFound ; i++) {
        if(ControllerInfo[i].BusDevFun == wBusDevFunc) {

            // Updatng the respective controller number.
            *gControllerNoAddr = ControllerInfo[i].ControllerNo;
            return;
        }
    }

    //
    // A new controller is found so BusDevFunc and ControllerNo is added
    // into the array of structure.
    //
    ControllerInfo[MaxControllerFound].BusDevFun    = wBusDevFunc;
    switch( Func ) {
        case PCI_FUNCTION_NUMBER_PCH_SATA:
            ControllerInfo[MaxControllerFound].ControllerNo = SATACONTROLLER0;
            break;

        case PCI_FUNCTION_NUMBER_EVA_SSATA:
            ControllerInfo[MaxControllerFound].ControllerNo = SSATACONTROLLER;
            break;
    }
    *gControllerNoAddr= ControllerInfo[MaxControllerFound].ControllerNo;
    MaxControllerFound++;
    return;
}

//  Function to Create AHCI drive Boot Option
EFI_STATUS
CreateBootOptionNameForAhciDrive ( 
    UINT8  ControllerNo,
    UINT8  PortNumber,
    UINT8  *SerialNumber,
    UINT8  *DriveString
)
{
    EFI_STATUS Status;
    UINT8   s[32] = {0};
    UINT8   TempStr[12] = "xSATAx Px: ";   //"[Controller] P[PortNo]: "
    UINTN   Index;
    EFI_LEGACY_REGION2_PROTOCOL *LegacyRegion2Protocol;
    UINT32  Granularity;
    
    TempStr[5] = ControllerNo + 0x30;
    TempStr[8] = PortNumber + 0x30;
    TempStr[0] = 0x20;

    if( ControllerNo == SSATACONTROLLER ) {
        TempStr[0] = 's';
        TempStr[5] = 0x20;
        pBS->CopyMem(s,TempStr,11);
        Index = 11;
    } else {
#if SMCPKG_SUPPORT
	TempStr[0] = 'I';
	TempStr[5] = 0x20;
	pBS->CopyMem(s,TempStr,11);
	Index = 11;
#else
        pBS->CopyMem(s,&TempStr[1],10);
        Index = 10;
#endif//SMCPKG_SUPPORT
    }
    pBS->CopyMem(&s[Index],SerialNumber,20);

    Status = pBS->LocateProtocol(&EfiLegacyRegion2ProtocolGuid,NULL,&LegacyRegion2Protocol);
          
    if(EFI_ERROR(Status)) {
        return Status;
    }
          
    LegacyRegion2Protocol->UnLock(LegacyRegion2Protocol,(UINT32)DriveString,32,&Granularity);

    pBS->CopyMem(DriveString,s,32);

    LegacyRegion2Protocol->Lock(LegacyRegion2Protocol,(UINT32)DriveString,32,&Granularity);

   return EFI_SUCCESS;
    
}



///  Fucntion to create boot option string to all drives
EFI_STATUS CreateStringForAllDrives(BBS_TABLE *BbsTable)
{
    EFI_STATUS  Status;
    static EFI_GUID DiskInfoGuid      = EFI_DISK_INFO_PROTOCOL_GUID;
    UINT32                            Bus = BbsTable->Bus;
    UINT32                            Device = BbsTable->Device;
    UINT32                            Function = BbsTable->Function;    
    UINT8                             Class = BbsTable->Class;
    UINT8                             SubClass = BbsTable->SubClass;
    UINTN                             gControllerNo=0,i;
    VOID                              *Dev;
    EFI_HANDLE                        DeviceHandle = (EFI_HANDLE)*(UINTN*)&BbsTable->IBV1;

    EFI_DISK_INFO_PROTOCOL            *DiskInfo;
    UINT16                            IdentifyData[256];
    UINT8                             SerialNumber[20];
    UINT8                             data8;
    UINT32                            Size;

    // Call to get Controller Number of the Drive
    GetControllerNumber(&gControllerNo,((UINT16)Bus<<8) + ((UINT16)Device<<3) + ((UINT16)Function) );

    Status = pBS->HandleProtocol( DeviceHandle, &DiskInfoGuid, &DiskInfo );

    if (EFI_ERROR(Status)) return 0;

    Size = sizeof(IdentifyData);
    
    Status = DiskInfo->Identify ( DiskInfo, IdentifyData, &Size );

    pBS->CopyMem(SerialNumber,&IdentifyData[27],20);
      
    for (i=0; i< 20; i+=2) {
        data8 = SerialNumber[i];
        SerialNumber[i] = SerialNumber[i+1];
        SerialNumber[i+1] = data8;
    }

    // BAsed on SubClass Value calling AHCI drive Option
    if(SubClass == 6) {
        Dev = (SATA_DEVICE_INTERFACE *)((SATA_DISK_INFO*)DiskInfo)->SataDevInterface;

        CreateBootOptionNameForAhciDrive((UINT8)gControllerNo,
                                         ((SATA_DEVICE_INTERFACE *)Dev)->PortNumber,
                                         SerialNumber,
                                         (UINT8*)(((UINT32)BbsTable->DescStringSegment << 4)+((UINT32)BbsTable->DescStringOffset & 0x0FFFF)) );
    }

    return Status;
}



/// OEM hook function 
UINTN OEMConstructBootOptionNameByBbsDescription (
        BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
)
{
    if (Option->BbsEntry == NULL) return 0;

    // Function to Change the Boot Option Name of the Drive
    CreateStringForAllDrives(Option->BbsEntry);

    return 0;
  
}

 
#endif


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
