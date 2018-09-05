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

/** @file SbPciHotPlugLib.c

    Library Class for AMI PCI Hotplug function.
*/

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <AmiLib.h>
#include <Library/DebugLib.h>
#include <AcpiRes.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciHotplugInit.h>
#include <Protocol/DevicePath.h>
#include <Library/AmiPciBusLib.h>
#include <Library/AmiPciExpressLib.h>
#include <Library/AmiSdlLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Setup.h>
#include <Guid/SetupVariable.h>
// PCH / IIO Resource reservation for Hot-Plug
#include <Guid/PchRcVariable.h>
#include <SocketIioVariable.h>
#include <SbPciHotPlugLib.h>
//#include <Library/PchPcieRpLib.h>
#include <Protocol/IioSystem.h>
// Intel Thunderbolt Support
#if Thunderbolt_SUPPORT
#include <Library/AmiTbtCspLib.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#include <Library/UefiLib.h>
#include <Guid/TbtHobInfo.h>
#endif

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
// PCH / IIO Resource reservation for Hot-Plug
typedef struct {
 UINT8 Socket;
 UINT8 Stack;
 UINT8 Bus;
 UINT8 Device;
 CHAR8 *AslName;
 UINT8 Index;
} IIO_PCIE_IDX_TABLE;

IIO_PCIE_IDX_TABLE IioPcieIdxTable [] =
{
  // Socket, Stack, Bus, Device, AslName, Index
  //
  // Socket 0
  // 
/*
  { 0, 0x0, 0xFF, 0x00, "PC00", 0 },
  { 0, 0x0, 0xFF, 0x01, "PC00", 0 },
  { 0, 0x0, 0xFF, 0x02, "PC00", 0 },
  { 0, 0x0, 0xFF, 0x03, "PC00", 0 },
*/  
  { 0, 0x1, 0xFF, 0x00, "BR1A", 1 }, // S0, 1A
  { 0, 0x1, 0xFF, 0x01, "BR1B", 2 }, // S0, 1B
  { 0, 0x1, 0xFF, 0x02, "BR1C", 3 }, // S0, 1C
  { 0, 0x1, 0xFF, 0x03, "BR1D", 4 }, // S0, 1D
  
  { 0, 0x2, 0xFF, 0x00, "BR2A", 5 }, // S0, 2A
  { 0, 0x2, 0xFF, 0x01, "BR2B", 6 }, // S0, 2B
  { 0, 0x2, 0xFF, 0x02, "BR2C", 7 }, // S0, 2C
  { 0, 0x2, 0xFF, 0x03, "BR2D", 8 }, // S0, 2D
  
  { 0, 0x3, 0xFF, 0x00, "BR3A", 9 },  // S0, 3A
  { 0, 0x3, 0xFF, 0x01, "BR3B", 10 }, // S0, 3B
  { 0, 0x3, 0xFF, 0x02, "BR3C", 11 }, // S0, 3C
  { 0, 0x3, 0xFF, 0x03, "BR3D", 12 }, // S0, 3D

  { 0, 0x4, 0xFF, 0x00, "PC04", 13 },
  { 0, 0x4, 0xFF, 0x01, "PC04", 14 },
  { 0, 0x4, 0xFF, 0x02, "PC04", 15 },
  { 0, 0x4, 0xFF, 0x03, "PC04", 16 },
  
  { 0, 0x5, 0xFF, 0x00, "PC05", 17 },
  { 0, 0x5, 0xFF, 0x01, "PC05", 18 },
  { 0, 0x5, 0xFF, 0x02, "PC05", 19 },
  { 0, 0x5, 0xFF, 0x03, "PC05", 20 },

  //
  // Socket 1
  // 
  { 1, 0x0, 0xFF, 0x00, "PC06", 21 },
  { 1, 0x0, 0xFF, 0x01, "PC06", 21 },
  { 1, 0x0, 0xFF, 0x02, "PC06", 21 },
  { 1, 0x0, 0xFF, 0x03, "PC06", 21 },

  { 1, 0x1, 0xFF, 0x00, "QR1A", 22 },
  { 1, 0x1, 0xFF, 0x01, "QR1B", 23 },
  { 1, 0x1, 0xFF, 0x02, "QR1C", 24 },
  { 1, 0x1, 0xFF, 0x03, "QR1D", 25 },
  
  { 1, 0x2, 0xFF, 0x00, "QR2A", 26 },
  { 1, 0x2, 0xFF, 0x01, "QR2B", 27 },
  { 1, 0x2, 0xFF, 0x02, "QR2C", 28 },
  { 1, 0x2, 0xFF, 0x03, "QR2D", 29 },
  
  { 1, 0x3, 0xFF, 0x00, "QR3A", 30 },
  { 1, 0x3, 0xFF, 0x01, "QR3B", 31 },
  { 1, 0x3, 0xFF, 0x02, "QR3C", 32 },
  { 1, 0x3, 0xFF, 0x03, "QR3D", 33 },

  { 1, 0x4, 0xFF, 0x00, "PC10", 34 },
  { 1, 0x4, 0xFF, 0x01, "PC10", 35 },
  { 1, 0x4, 0xFF, 0x02, "PC10", 36 },
  { 1, 0x4, 0xFF, 0x03, "PC10", 37 },
  
  { 1, 0x5, 0xFF, 0x00, "PC11", 38 },
  { 1, 0x5, 0xFF, 0x01, "PC11", 39 },
  { 1, 0x5, 0xFF, 0x02, "PC11", 40 },
  { 1, 0x5, 0xFF, 0x03, "PC11", 41 },

  //
  // Socket 2
  // 
  { 2, 0x0, 0xFF, 0x00, "PC12", 42 },
  { 2, 0x0, 0xFF, 0x01, "PC12", 42 },
  { 2, 0x0, 0xFF, 0x02, "PC12", 42 },
  { 2, 0x0, 0xFF, 0x03, "PC12", 42 },

  { 2, 0x1, 0xFF, 0x00, "RR1A", 43 },
  { 2, 0x1, 0xFF, 0x01, "RR1B", 44 },
  { 2, 0x1, 0xFF, 0x02, "RR1C", 45 },
  { 2, 0x1, 0xFF, 0x03, "RR1D", 46 },
  
  { 2, 0x2, 0xFF, 0x00, "RR2A", 47 },
  { 2, 0x2, 0xFF, 0x01, "RR2B", 48 },
  { 2, 0x2, 0xFF, 0x02, "RR2C", 49 },
  { 2, 0x2, 0xFF, 0x03, "RR2D", 50 },
  
  { 2, 0x3, 0xFF, 0x00, "RR3A", 51 },
  { 2, 0x3, 0xFF, 0x01, "RR3B", 52 },
  { 2, 0x3, 0xFF, 0x02, "RR3C", 53 },
  { 2, 0x3, 0xFF, 0x03, "RR3D", 54 },

  { 2, 0x4, 0xFF, 0x00, "PC16", 55 },
  { 2, 0x4, 0xFF, 0x01, "PC16", 56 },
  { 2, 0x4, 0xFF, 0x02, "PC16", 57 },
  { 2, 0x4, 0xFF, 0x03, "PC16", 58 },
  
  { 2, 0x5, 0xFF, 0x00, "PC17", 59 },
  { 2, 0x5, 0xFF, 0x01, "PC17", 60 },
  { 2, 0x5, 0xFF, 0x02, "PC17", 61 },
  { 2, 0x5, 0xFF, 0x03, "PC17", 62 },

  //
  // Socket 3
  // 
  { 3, 0x0, 0xFF, 0x00, "PC18", 63 },
  { 3, 0x0, 0xFF, 0x01, "PC18", 63 },
  { 3, 0x0, 0xFF, 0x02, "PC18", 63 },
  { 3, 0x0, 0xFF, 0x03, "PC18", 63 },

  { 3, 0x1, 0xFF, 0x00, "SR1A", 64 },
  { 3, 0x1, 0xFF, 0x01, "SR1B", 65 },
  { 3, 0x1, 0xFF, 0x02, "SR1C", 66 },
  { 3, 0x1, 0xFF, 0x03, "SR1D", 67 },
  
  { 3, 0x2, 0xFF, 0x00, "SR2A", 68 },
  { 3, 0x2, 0xFF, 0x01, "SR2B", 69 },
  { 3, 0x2, 0xFF, 0x02, "SR2C", 70 },
  { 3, 0x2, 0xFF, 0x03, "SR2D", 71 },
  
  { 3, 0x3, 0xFF, 0x00, "SR3A", 72 },
  { 3, 0x3, 0xFF, 0x01, "SR3B", 73 },
  { 3, 0x3, 0xFF, 0x02, "SR3C", 74 },
  { 3, 0x3, 0xFF, 0x03, "SR3D", 75 },

  { 3, 0x4, 0xFF, 0x00, "PC22", 76 },
  { 3, 0x4, 0xFF, 0x01, "PC22", 77 },
  { 3, 0x4, 0xFF, 0x02, "PC22", 78 },
  { 3, 0x4, 0xFF, 0x03, "PC22", 79 },

  { 3, 0x5, 0xFF, 0x00, "PC23", 80 },
  { 3, 0x5, 0xFF, 0x01, "PC23", 81 },
  { 3, 0x5, 0xFF, 0x02, "PC23", 82 },
  { 3, 0x5, 0xFF, 0x03, "PC23", 83 },
};

UINT8 IioPcieIdxTblLen = sizeof (IioPcieIdxTable) / sizeof(IIO_PCIE_IDX_TABLE);

//ONLY ONE INSTANCE ALOUD IN THE SYSTEM!!!
//static EFI_GUID                   gEfiPchRcVariableGuid = PCH_RC_CONFIGURATION_GUID;
EFI_PCI_HOT_PLUG_INIT_PROTOCOL    *gRhpcInitProtocol       = NULL;
BOOLEAN                           gRhpcNotFound            = FALSE;
EFI_IIO_SYSTEM_PROTOCOL           *gIioSystemProtocol;
SOCKET_IIO_CONFIGURATION          gIioConfiguration;
PCH_RC_CONFIGURATION              gPchConfiguration;
UINTN                             PchRcVariableSize = sizeof (PCH_RC_CONFIGURATION);
UINTN                             IioRcVariableSize = sizeof (SOCKET_IIO_CONFIGURATION);
// Intel Thunderbolt Support
EFI_GUID                          gSetupGuid = SETUP_GUID;
// Build fail if HOTPLUG_SUPPORT is enabled.
SETUP_DATA                        gSetupData;
UINTN                             SetupDataSize = sizeof(SETUP_DATA);
#if Thunderbolt_SUPPORT
STATIC TBT_INFO_HOB               *gTbtInfoHob;
#endif

VOID UpdateIioPcieIdxTable ( VOID )
{
  UINTN                   i, idx;
  AMI_SDL_PCI_DEV_INFO    *sdltmp = NULL;
  AMI_SDL_PCI_DEV_INFO    *sdltmpParent = NULL;

  for (i = 0; i < gSdlPciData->PciDevCount; i++) {
    sdltmp       = &gSdlPciData->PciDevices[i];
    sdltmpParent = &gSdlPciData->PciDevices[sdltmp->ParentIndex];
    if (sdltmpParent->PciDevFlags.Bits.RootBridgeType && sdltmpParent->PciDevFlags.Bits.FixedBus){
      if (sdltmp->PciDevFlags.Bits.Pci2PciBridgeType) {
        for (idx = 0; idx < IioPcieIdxTblLen; idx++) {
          if ((sdltmp->Device == IioPcieIdxTable[idx].Device) && \
    	      (sdltmp->AslName[0] == *(IioPcieIdxTable[idx].AslName+0)) && \
              (sdltmp->AslName[1] == *(IioPcieIdxTable[idx].AslName+1)) && \
              (sdltmp->AslName[2] == *(IioPcieIdxTable[idx].AslName+2)) && \
              (sdltmp->AslName[3] == *(IioPcieIdxTable[idx].AslName+3))) {
            if (IioPcieIdxTable[idx].Bus != 0xFF) continue;
            IioPcieIdxTable[idx].Bus = sdltmpParent->Bus;
            break;
          }
        }
      }
    }
  }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    HpcCheckHpCompatibleHost()
//
// Description: This function will check if "HostBrg" passed is the Bridge
// with hotplug support.
//
// Input:
//  PCI_HOST_BRG_DATA *HostBrg     Pointer to PCI HB Private Data structure.
//
// Output:  BOOLEAN
//  TRUE    If "Device" is a Bridge with HPC on it.
//  FALSE   Otherwice.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN HpcCheckHpCompatibleHost(PCI_HOST_BRG_DATA *HostBrg){
    //if(    HostBrg->HpcData!=NULL && HostBrg->HpcData->HpcLocCount > 0 ) return TRUE;
    if(HostBrg->HpcData == NULL)
        DEBUG((EFI_D_INFO,"HostBrg->HpcData is NULL\n"));
    
    if(HostBrg->HpcData->HpcLocCount > 0){
        DEBUG((EFI_D_INFO,"HpcLocCount is %d\n", HostBrg->HpcData->HpcLocCount));
        return TRUE;
    }
    
    DEBUG((EFI_D_INFO, __FUNCTION__" function return FALSE\n"));
    return FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    IsHpb()
//
// Description: This function will check if "Device" passed is the Bridge
// Type Device with hotplug support.
//
// Input:
//  PCI_DEV_INFO    *Device     Pointer to PCI Device Private Data structure.
//
// Output:  BOOLEAN
//  TRUE    If "Device" is a Bridge with HPC on it.
//  FALSE   Otherwice.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN HpIsHpb(PCI_DEV_INFO *Device){

    //check if it is the right type of device to have Hotplug capabilities
    if(!((Device->Type==tPci2PciBrg)||(Device->Type==tPci2CrdBrg)||(Device->Type==tPciRootBrg))){
        DEBUG((EFI_D_INFO,"PciHP: Device @ B%X|D%X|F%X is NOT HP CAPABLE (Brg Type)\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
        return FALSE;
    }

    //Check if it has Root Hpc or PciExpress capabilities
    if((Device->HotPlug==NULL) && !PcieCheckPcieCompatible(Device)){
        DEBUG((EFI_D_INFO,"PciHP: Device @ B%X|D%X|F%X is NOT HP CAPABLE (SHPC/PCIe)\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
        return FALSE;
    }
    //If Device is PciExpress, check if SLOT CAPABILITIES Register supports Hotplug
    if(PcieCheckPcieCompatible(Device) && (!Device->PciExpress->SlotCap.HpCapable)){
        DEBUG((EFI_D_INFO,"PciHP: Device @ B%X|D%X|F%X is NOT HP CAPABLE (SltCap.HpCap)\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
//DEBUG    start all PCIe is HP Capable for debugging. Uncomment 
        //DEBUG((EFI_D_INFO,"DEBUG: Device @ B%X|D%X|F%X become HP CAPABLE (SltCap.HpCap)\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
        return FALSE; //DEBUG    all PCIe is HP Capable for debugging. comment this line!
//DEBUG    end.
    }

    DEBUG((EFI_D_INFO,"PciHP: Device @ B%X|D%X|F%X is HP CAPABLE returning TRUE\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
    return TRUE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    CheckRootHotplug()
//
// Description: This function will update pointer to PCI_RHPC_INFO of
// Bridge Type device which creates a hot plug bus. Also if "Device"
// creates the 'home bus' for Root HPC it will initialize Root HPC and
// record the HPC state;
//
// Input:
//  PCI_DEV_INFO    *Device     Pointer to PCI Device Private Data structure.
//
// Output:    EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_NOT_FOUND           When Device not present in the system.
//  EFI_OUT_OF_RESOURCES    When system runs out of resources.
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HpCheckRootHotplug(PCI_DEV_INFO *Device, UINT8 MaxBusFound){
    EFI_STATUS            Status=EFI_SUCCESS;
    EFI_DEVICE_PATH_PROTOCOL *dp=NULL;
    PCI_HPC_INFO        *rhpc;
    UINTN                i;
//---------------
    if(Device->Type==tPci2PciBrg || Device->Type==tPci2CrdBrg || Device->Type==tPciRootBrg){
        //we have identify and Init all the Root Hpc and HPB
        if(Device->HostData->InitRhpcCount==Device->HostData->RhpcCount) return EFI_SUCCESS;
        PROGRESS_CODE(DXE_PCI_BUS_HPC_INIT);
        //here we must check if 2 things:
        //    1.If "Device" Device Path mutches with one of Root Hpc Device pathes
        //    2.If "Device" Device Path mutches with one of Root Hpb Device pathes
        for(i=0; i<Device->HostData->RhpcCount; i++){
            rhpc=Device->HostData->RhpcList[i];
            //if this controller already has been initialized - keep going
            if(rhpc->Initialized && rhpc->BusFound)    continue;


            //check if it is a secondary interface of the bridge where RHPC is sitting
            if(!rhpc->BusFound){
                if(!DPCmp(Device->DevicePath, rhpc->HpcLocation->HpbDevicePath)){
                    rhpc->HpbBridge=Device;
                    rhpc->BusFound=TRUE;
                    Device->HotPlug=rhpc;
                    DEBUG((EFI_D_INFO,"PciHP: Found HP Bus Bridge @ B%X|D%X|F%X \n\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
                }
            }

            if(!rhpc->Initialized){
                //We will cut the last node of the HPC device path
                //resulting Device Path will correspond to the ParentBridge.DevicePath of the HPC
                dp=DPCut(rhpc->HpcLocation->HpcDevicePath);
                //if the HPC sits behind this bridge get the secondary I/F bus number
                if(!DPCmp(Device->DevicePath,dp)){
                    EFI_DEVICE_PATH_PROTOCOL    *tdp;
                //-----------------------------------
                    rhpc->HpcPciAddr.Addr.Bus=MaxBusFound;
                    tdp=DPGetLastNode(rhpc->HpcLocation->HpcDevicePath);

                    rhpc->HpcPciAddr.Addr.Device=((PCI_DEVICE_PATH*)tdp)->Device;
                    rhpc->HpcPciAddr.Addr.Function=((PCI_DEVICE_PATH*)tdp)->Function;

                    //we will not set up an event to signal when HPC initialization is ready
                    //that will give us a possibility to enumerate PCI BUS in one pass
                    Status=Device->HostData->HpInitProt->InitializeRootHpc(
                            Device->HostData->HpInitProt, rhpc->HpcLocation->HpcDevicePath,
                            rhpc->HpcPciAddr.ADDR, NULL, &rhpc->HpcState);

                    ASSERT_EFI_ERROR(Status);
                    rhpc->Owner=Device;
                    rhpc->Initialized=TRUE;
                    DEBUG((EFI_D_INFO,"PciBus: Found RHPC @ B%X|D%X|F%X \n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
                }
            }

            //if we come here we did some step of the initialization process
            //let's check if this controller has been completly initialized.
            //if so we did our job. there are no reasons to stay in this loop any longer
            if(rhpc->Initialized && rhpc->BusFound)    {
                Device->HostData->InitRhpcCount++;
                break;
            }

        }//for
    }
    if(dp)pBS->FreePool(dp);
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   GetHpbResPadding()
//
// Description: This function will get and apply resource padding
// requirements for the PCI to PCI Bridge or Card Bus Bridge, if this
// Bridge is supports hot plug.
//
// Input:
//  PCI_DEV_INFO    *Device     Pointer to PCI Device Private Data structure.
//
// Output:    EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_NOT_FOUND           When Device not present in the system.
//  EFI_OUT_OF_RESOURCES    When system runs out of resources.
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HpGetHpbResPadding(PCI_DEV_INFO *Device){
    EFI_STATUS        Status=EFI_SUCCESS;
//-----------------------------
    if(HpIsHpb(Device)){
        PCI_BRG_EXT        *ext=(PCI_BRG_EXT*)(Device+1);
        ASLR_QWORD_ASD    *cnf=NULL, *res=NULL;
        PCI_BAR            *bar;
    //----------------------------
        Status=Device->HostData->HpInitProt->GetResourcePadding(Device->HostData->HpInitProt,
                        Device->HotPlug->HpcLocation->HpcDevicePath, Device->HotPlug->HpcPciAddr.ADDR,
                        &Device->HotPlug->HpcState,&cnf,&ext->PaddAttr);

        ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)) return Status;

        if(ext->PaddAttr==EfiPaddingPciRootBridge){
            pBS->FreePool(cnf);
            return Status;
        }
        
        DEBUG((EFI_D_INFO,"PciHp Padding: Processing QWORD RD for [B%X|D%X|F%X]:", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
        //Check if we got a valid descriptors in cfg buffer
        if(!ValidateDescriptorBlock(cnf, tResAll, FALSE)){
            DEBUG((EFI_D_ERROR," !!!INVALID DESCRIPTOR!!!\n"));
            return EFI_INVALID_PARAMETER;
        }

        DEBUG((EFI_D_INFO,"\n"));

        res=cnf;
        while(res->Hdr.HDR!=ASLV_END_TAG_HDR) {

            DEBUG((EFI_D_INFO,"\tType=0x%lX; TFlags=0x%lX; _MIN=0x%lX; _MAX=0x%lX; _LEN=0x%lX; _GRA=0x%lX;\n", 
            res->Type,res->TFlags,res->_MIN,res->_MAX,res->_LEN,res->_GRA));

            
            bar=NULL;
            switch(res->Type){
                case ASLRV_SPC_TYPE_BUS    :
                    bar=&ext->Pad[rtBus];
                break;
                case ASLRV_SPC_TYPE_IO    :
                        bar=&ext->Pad[rtIo16];
                        bar->Type=tBarIo16;
                        if(res->_GRA==32){
                            bar=&ext->Pad[rtIo32];
                            bar->Type=tBarIo32;
                        }
                break;
                case ASLRV_SPC_TYPE_MEM    :
                    if(res->_GRA==32){
                        if(res->TFlags.MEM_FLAGS._MEM==ASLRV_MEM_CEPF)    {
                            bar=&ext->Pad[rtMmio32p];
                            bar->Type=tBarMmio32pf;
                        } else {
                            bar=&ext->Pad[rtMmio32];
                            bar->Type=tBarMmio32;
                        }
                    } else {
                        if(res->_GRA==64){
                            if(res->TFlags.MEM_FLAGS._MEM==ASLRV_MEM_CEPF){
                                bar=&ext->Pad[rtMmio64p];
                                bar->Type=tBarMmio64pf;
                            } else {
                                bar=&ext->Pad[rtMmio64];
                                bar->Type=tBarMmio64;
                            }
                        } else {
                            Status=EFI_INVALID_PARAMETER; //no other options alloud
                            goto ExitLbl;
                        }
                    }
                break;
                default: {
                    Status=EFI_INVALID_PARAMETER; //no other options alloud
                    goto ExitLbl;
                }
            } //switch

            bar->Length=res->_LEN;
            bar->Gran=res->_MAX;
            
            res++;
        } //while
ExitLbl:    
        if(cnf!=NULL)pBS->FreePool(cnf);
        DEBUG((EFI_D_INFO,"PciHp: Padding for [B%X|D%X|F%X] Updated...Status=%r\n", 
        Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function, Status));
        
    }

    return Status;
}


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
BOOLEAN HpCheckHpCompatible(PCI_DEV_INFO *Device){
    if(Device->HotPlug!=NULL) return TRUE;
    else return FALSE;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS HpAllocateInitHpData(PCI_DEV_INFO *Device, UINT16 HpCapOffset){
    //we may process this controller as root HPC so check that.
    if(Device->HotPlug==NULL){
        Device->HotPlug=MallocZ(sizeof(PCI_HPC_INFO));
        ASSERT(Device->HotPlug);
        if(!Device->HotPlug) return EFI_OUT_OF_RESOURCES;

        //Save Current Offset of PCIE Std Compatibility Registers Block
        Device->HotPlug->HpcLocation->HpcDevicePath=Device->DevicePath;
        Device->HotPlug->HpcLocation->HpcDevicePath=Device->DevicePath;
        Device->HotPlug->HpcPciAddr.ADDR=Device->Address.ADDR;
        Device->HotPlug->HpbBridge=Device;
        Device->HotPlug->BusFound=TRUE;
        //For SHPC it must be initialized and enabled by hardware
        Device->HotPlug->HpcState=EFI_HPC_STATE_INITIALIZED|EFI_HPC_STATE_ENABLED;
        Device->HotPlug->HpcOffs=HpCapOffset;
        Device->HotPlug->Owner=Device;
    }

    return EFI_SUCCESS;
}


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS HpLocateProtocolSortRhpc(PCI_HOST_INFO    *PciHpcHost, PCI_DEV_INFO    *RootBridge){
    EFI_STATUS                    Status;
    UINTN                        ghpccnt=0, j;
    EFI_HPC_LOCATION            *ghpclst=NULL;
    EFI_DEVICE_PATH_PROTOCOL    *rbdp;
//------------------------------
    //if we were there and not found what was looking for just exit...
    if(gRhpcNotFound==TRUE) return EFI_SUCCESS;

    if(gRhpcInitProtocol==NULL){
        Status=pBS->LocateProtocol(&gEfiPciHotPlugInitProtocolGuid, NULL, &gRhpcInitProtocol);
        if(EFI_ERROR(Status)){
            if(Status == EFI_NOT_FOUND){
                gRhpcNotFound=TRUE;
                return EFI_SUCCESS;
            } else return Status;
        }
    }

    //If we here, we got gRhpcInterface
    //It MUST be only one INSTANCE in the system, even if system has multiple hosts!
    //now we must initialize internal data with of PCI_HOST_DATA information.
    if(PciHpcHost->HpInitProt==NULL) PciHpcHost->HpInitProt=gRhpcInitProtocol;
    rbdp=RootBridge->DevicePath;

    //Get Root HPC List
    Status=gRhpcInitProtocol->GetRootHpcList(gRhpcInitProtocol,&ghpccnt,&ghpclst);
    ASSERT_EFI_ERROR(Status);


    //sort out Root Hot Plug Controlers list items to where they belongs
    DEBUG((EFI_D_INFO,"PciBus: Get Location - HpcLocCount=%d; RbDp=[HID(%X)UID(%X)]\n",
        ghpccnt,((ACPI_HID_DEVICE_PATH*)rbdp)->HID,((ACPI_HID_DEVICE_PATH*)rbdp)->UID));
    DEBUG((EFI_D_INFO,"-------------------------------------------------\n"));

    for (j=0; j<ghpccnt; j++){
        if( !MemCmp(rbdp, ghpclst[j].HpcDevicePath, sizeof(ACPI_HID_DEVICE_PATH)) ){
            PCI_HPC_INFO    *rhpc;
        //-----------------------------
            rhpc=MallocZ(sizeof(PCI_HPC_INFO));
            ASSERT(rhpc);
            if(!rhpc) return EFI_OUT_OF_RESOURCES;
            //Init PCI_ROOT_HPC structure
            rhpc->Root=TRUE;
            rhpc->HpcLocation=&ghpclst[j];

            DEBUG((EFI_D_INFO,"HpcDP=[HID(%X)UID(%X)]; HpbDP[HID(%X)UID(%X)]\n",
            ((ACPI_HID_DEVICE_PATH*)rhpc->HpcLocation->HpcDevicePath)->HID,((ACPI_HID_DEVICE_PATH*)rhpc->HpcLocation->HpcDevicePath)->UID,
            ((ACPI_HID_DEVICE_PATH*)rhpc->HpcLocation->HpbDevicePath)->HID,((ACPI_HID_DEVICE_PATH*)rhpc->HpcLocation->HpbDevicePath)->UID));

            Status=AppendItemLst((T_ITEM_LIST*)&PciHpcHost->RhpcInitCnt,rhpc);
            ASSERT_EFI_ERROR(Status);
        }
    }
    DEBUG((EFI_D_INFO,"-------------------------------------------------\n"));

    return Status;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
VOID HpClearPaddingData(PCI_BAR *PaddingBar){
    PCI_DEV_INFO        *owner;
//---------------------------
    //Here we about to clear PCI_BAR Object w/o touching Owner field.
    owner=PaddingBar->Owner;
    pBS->SetMem(PaddingBar, sizeof(PCI_BAR),0);
    PaddingBar->Owner=owner;
}


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS HpApplyResPadding(PCI_BAR *PaddingBar, PCI_BAR *BridgeBar){

    if(PaddingBar->Length){
        DEBUG((EFI_D_INFO,"PciHp: Bridge BAR  BAR_TYPE=%X; Len=%lX; Gran=%lX;\n",
                BridgeBar->Type, BridgeBar->Length, BridgeBar->Gran ));
        
        if(PaddingBar->Length > BridgeBar->Length) BridgeBar->Length = PaddingBar->Length;
        if(BridgeBar->Gran < PaddingBar->Gran) BridgeBar->Gran=PaddingBar->Gran;
        BridgeBar->Type=PaddingBar->Type;
        
        DEBUG((EFI_D_INFO,"PciHp: Padding BAR BAR_TYPE=%X; Len=%lX; Gran=%lX;\n",
                PaddingBar->Type, PaddingBar->Length, PaddingBar->Gran ));
        
        return EFI_SUCCESS;
    }

    return EFI_UNSUPPORTED;
}


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS HpApplyBusPadding(PCI_DEV_INFO *Brg, UINT8 OldMaxBus, UINT8 *MaxBusFound){
    PCI_BRG_EXT        *ext;
//-----------------------
    if(!HpCheckHpCompatible(Brg)) return EFI_SUCCESS;

    ext=(PCI_BRG_EXT*)(Brg+1);

    HpGetHpbResPadding(Brg);

    //Here we come with information about resources padding
    //Brg->Hotplug->Padd[rtBus].Length will have Bus numbers needed to pad for this bridge
    //mMaxBusFound will have number of buses actualy present behind this bridge
    //take care of buses now the rest of resources will be taken care of when
    //CalculateBridge resources will be called
    if(ext->PaddAttr==EfiPaddingPciBus){
        //EIP 19106 If 1 Bus set to padding in Setup it did not do the padding.
        if((ext->Pad[rtBus].Length!=0) && (ext->Pad[rtBus].Length > (*MaxBusFound)-ext->Res[rtBus].Base))
        {
            *MaxBusFound=OldMaxBus+(UINT8)ext->Pad[rtBus].Length;
        }
    }
    return EFI_SUCCESS;

}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS HpcUpdateLocationData(
    PCI_HOST_BRG_DATA       *HostBrg, 
    PCI_ROOT_BRG_DATA       *RootBrg, 
    AMI_SDL_PCI_DEV_INFO    *HpBrgSdlData,
    UINTN                   HpBrgIndex,
    UINTN                   HpSlotCount,
    UINTN                   *HpSlotIdxArray)
{
    EFI_STATUS                  Status;
    AMI_SDL_PCI_DEV_INFO        *parent;
    PCI_DEVICE_PATH             pcidp;
    EFI_DEVICE_PATH_PROTOCOL    *tmpdp;
    HPC_LOCATION_DATA           *locdata=NULL;
// PCH / IIO Resource reservation for Hot-Plug
    UINT8                       idx;
    UINT8                       NbPcieIndex;
// Intel Thunderbolt Support
#if Thunderbolt_SUPPORT
    UINTN                       RpDev;
    UINTN                       RpFunc;
#endif
        
//-------------------------------------
    //If has not been allocated yet..
    if(HostBrg->HpcData==NULL){
        HostBrg->HpcData=MallocZ(sizeof(PCI_HPC_DATA));
        if(HostBrg->HpcData==NULL) {
            Status=EFI_OUT_OF_RESOURCES;
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
    }
    
    //Initialize PCI Device Path variable we will not touch header...
    pcidp.Header.SubType=HW_PCI_DP;
    pcidp.Header.Type=HARDWARE_DEVICE_PATH;
    pcidp.Device=HpBrgSdlData->Device;
    pcidp.Function=HpBrgSdlData->Function;
    SET_NODE_LENGTH(&pcidp.Header,HW_PCI_DEVICE_PATH_LENGTH);
    
    //Get Parent of HP_BRG... 
    parent=&gSdlPciData->PciDevices[HpBrgSdlData->ParentIndex];
    
    //Generate the device path for the HPController\HPBus we have pci device path reserved there.
    //this will add END OF DP to the PCI_DP.
    tmpdp=DPAddNode(NULL, (EFI_DEVICE_PATH_PROTOCOL*)&pcidp);  
    if(tmpdp==NULL) {
        Status=EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    while (!parent->PciDevFlags.Bits.RootBridgeType){
        VOID                        *oldp=tmpdp;
        EFI_DEVICE_PATH_PROTOCOL    *ppcidp;
    //---------------------------------------
        //ppcidp=NULL;
        pcidp.Device=parent->Device;
        pcidp.Function=parent->Function;
        ppcidp=DPAddNode(NULL, (EFI_DEVICE_PATH_PROTOCOL*)&pcidp);
 
        if(ppcidp==NULL) {
            Status=EFI_OUT_OF_RESOURCES;
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
                
        tmpdp=DPAdd(ppcidp, tmpdp);
        pBS->FreePool(ppcidp);

        if(tmpdp==NULL) {
            Status=EFI_OUT_OF_RESOURCES;
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
        
        if(oldp!=NULL)pBS->FreePool(oldp);
        
        parent=&gSdlPciData->PciDevices[parent->ParentIndex];
        
        //should not come here but who knows.. if wrong porting...
        if(parent->ParentIndex==0) break;
    }

    DEBUG((EFI_D_INFO,"PciDP Dev:%d Fun:%d\n", pcidp.Device, pcidp.Function));
    //Here DevicePath where points "tmpdp" Is Ready but missing root bridge DP...
    //Get some Memory ....
    locdata=MallocZ(sizeof(HPC_LOCATION_DATA));
    if(locdata==NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    } else Status=EFI_SUCCESS;
                
    locdata->Owner=RootBrg;
    locdata->SdlHpBrgData=HpBrgSdlData;
    locdata->SdlHpBrgIndex=HpBrgIndex;
    
    //Now we have to generate PCI_DP down to parent Root Bridge of this HpBrgSdlData 
    //For SHPC and PCIExpress Hotplug HPC==HPB = tmpdp.
    //make a copy of generated DP into hpclocation buffer
    locdata->HpcLocation.HpcDevicePath=DPAdd(RootBrg->DevPath, tmpdp);            
    locdata->HpcLocation.HpbDevicePath=DPAdd(RootBrg->DevPath, tmpdp);            
    
    pBS->FreePool(tmpdp);

    if(locdata->HpcLocation.HpbDevicePath==NULL || locdata->HpcLocation.HpcDevicePath==NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    Status=AppendItemLst((T_ITEM_LIST*)&HostBrg->HpcData->HpclInitCnt, locdata);
    ASSERT_EFI_ERROR(Status);
    
    //Update get Padding Setup Data.
    DEBUG((EFI_D_INFO,"Update get Padding Setup Data in HpcUpdateLocationData function\n"));

// PCH / IIO Resource reservation for Hot-Plug
// Intel Thunderbolt Support
    Status = pRS->GetVariable ( L"Setup", \
                                &gSetupGuid, \
                                NULL, \
                                &SetupDataSize, \
                                &gSetupData );

    if (parent->PciDevFlags.Bits.FixedBus){
        if ((parent->Bus == 0 && pcidp.Device == 0x1B) || \
            (parent->Bus == 0 && pcidp.Device == 0x1C) || \
            (parent->Bus == 0 && pcidp.Device == 0x1D)) {
        	if (pcidp.Device == 0x1C) {
        	  idx = 0;
        	} else if (pcidp.Device == 0x1D) {
        	  idx = 8;
        	} else if (pcidp.Device == 0x1B) {
        	  idx = 16;
        	}
        	idx += pcidp.Function;
            locdata->HpSetup.ArrayField.BusPadd      = (UINT64)gPchConfiguration.PcieExtraBusRsvd[idx];
            locdata->HpSetup.ArrayField.IoPadd       = (UINT64)gPchConfiguration.PcieIoRsvd[idx] * 0x400;
            locdata->HpSetup.ArrayField.Mmio32Padd   = (UINT64)gPchConfiguration.PcieMemRsvd[idx] * 0x100000;
            locdata->HpSetup.ArrayField.Mmio32PfPadd = (UINT64)gPchConfiguration.PciePFMemRsvd[idx] * 0x100000;
            locdata->HpSetup.ArrayField.Mmio64PfPadd = (UINT64)gPchConfiguration.Pcie64PFMemRsvd[idx] * 0x100000;
        } else {
            for (idx = 0; idx < IioPcieIdxTblLen; idx++) {
              if ((parent->Bus == IioPcieIdxTable[idx].Bus) && (pcidp.Device == IioPcieIdxTable[idx].Device)) {
                NbPcieIndex = IioPcieIdxTable[idx].Index;
                locdata->HpSetup.ArrayField.BusPadd      = (UINT64)gIioConfiguration.IioExtraBusRsvd[NbPcieIndex];
                locdata->HpSetup.ArrayField.IoPadd       = (UINT64)gIioConfiguration.IioIoRsvd[NbPcieIndex] * 0x400;
                locdata->HpSetup.ArrayField.Mmio32Padd   = (UINT64)gIioConfiguration.IioMemRsvd[NbPcieIndex] * 0x100000;
                locdata->HpSetup.ArrayField.Mmio32PfPadd = (UINT64)gIioConfiguration.IioPFMemRsvd[NbPcieIndex] * 0x100000;
                locdata->HpSetup.ArrayField.Mmio64PfPadd = (UINT64)gIioConfiguration.Iio64PFMemRsvd[NbPcieIndex] * 0x100000;
                break;
              }
            }
        }
#if Thunderbolt_SUPPORT
// Build fail if HOTPLUG_SUPPORT is enabled.
        if (gTbtInfoHob->TbtSelector != 0) {
          Status = GetTbtRpDevFun (gTbtInfoHob->TbtSelector - 1, &RpDev, &RpFunc);

          if (((parent->Bus == 0) && (pcidp.Device == RpDev) && (pcidp.Function == RpFunc)) || \
              ((parent->Bus == IioPcieIdxTable[idx].Bus) && (pcidp.Device == RpDev) && (pcidp.Function == 0)))
          {
            locdata->HpSetup.ArrayField.BusPadd        = (UINT64)gSetupData.TbtPcieExtraBusRsvd;
            locdata->HpSetup.ArrayField.IoPadd         = (UINT64)gSetupData.TbtPcieIoRsvd * 0x400;
            locdata->HpSetup.ArrayField.Mmio32Padd     = (UINT64)gSetupData.TbtPcieMemRsvd * 0x100000;
            if (gSetupData.Above4gDecode)
              locdata->HpSetup.ArrayField.Mmio64PfPadd = (UINT64)gSetupData.TbtPciePMemRsvd * 0x100000;
            else
              locdata->HpSetup.ArrayField.Mmio32PfPadd = (UINT64)gSetupData.TbtPciePMemRsvd * 0x100000;
          }
        }
#endif

        DEBUG((EFI_D_INFO,"Setup Data: Rsvd Bus:%lx, Rsvd Mem:%lx, Rsvd PFMem:%lx, Rsvd 64PFMem:%lx, Rsvd Io:%lx\n",
        		locdata->HpSetup.ArrayField.BusPadd,
        		locdata->HpSetup.ArrayField.Mmio32Padd,
        		locdata->HpSetup.ArrayField.Mmio32PfPadd,
        		locdata->HpSetup.ArrayField.Mmio64PfPadd,
        		locdata->HpSetup.ArrayField.IoPadd));
    } // parent->PciDevFlags.Bits.FixedBus
    
/*
#if (PCI_SETUP_USE_APTIO_4_STYLE == 0)
    if(HpBrgSdlData->PciDevFlags.Bits.HasSetup){
        //if Bridge has setup data just ignore what slots can have, it naturally coming from the bridge...
        Status=AmiPciGetPciHpSetupData(&locdata->HpSetup.ArrayField, HpBrgSdlData, HpBrgIndex, FALSE);
        if(EFI_ERROR(Status)){
            if(Status==EFI_NOT_FOUND){
                //Use SDL PCI DATA object for padding...
                locdata->HpSetup.ArrayField.BusPadd=HpBrgSdlData->ResourcePadding[ptBus];
                locdata->HpSetup.ArrayField.IoPadd=HpBrgSdlData->ResourcePadding[ptIo];
                locdata->HpSetup.ArrayField.Io32Padd=HpBrgSdlData->ResourcePadding[ptIo32];
                locdata->HpSetup.ArrayField.Mmio32Padd=HpBrgSdlData->ResourcePadding[ptMmio32];   
                locdata->HpSetup.ArrayField.Mmio32PfPadd=HpBrgSdlData->ResourcePadding[ptMmio32pf];
                locdata->HpSetup.ArrayField.Mmio64Padd=HpBrgSdlData->ResourcePadding[ptMmio64];
                locdata->HpSetup.ArrayField.Mmio64PfPadd=HpBrgSdlData->ResourcePadding[ptMmio64pf];
                Status=AmiPciGetPciHpSetupData(&locdata->HpSetup.ArrayField, HpBrgSdlData, HpBrgIndex, TRUE);
            }  
            if(EFI_ERROR(Status))return Status;
        }
    } else {
        //check how many slots come here with setup option set...
        //if we have setup for each slot 
        if(HpSlotCount){
            UINTN                    i, x;
            PCI_HP_SETUP_DATA        hps;
            AMI_SDL_PCI_DEV_INFO    *sdl;
        //------------------------------    
            //locdata->HpSetup at that moment initialized with all 0 
            for(i=0; i<HpSlotCount; i++){
                x=HpSlotIdxArray[i];
                Status=AmiSdlFindIndexRecord(x, &sdl);
                ASSERT_EFI_ERROR(Status);
                Status=AmiPciGetPciHpSetupData(&hps, sdl, x, FALSE);
                if(EFI_ERROR(Status)){
                    if(Status==EFI_NOT_FOUND){
                        //Use SDL PCI DATA object for Default padding... 
                        pBS->CopyMem(&hps,&HpBrgSdlData->ResourcePadding, sizeof(PCI_HP_SETUP_DATA));
                        Status=AmiPciGetPciHpSetupData(&hps, sdl, x, TRUE);
                    }  
                    if(EFI_ERROR(Status))return Status;
                }
                locdata->HpSetup.ArrayField.BusPadd+=hps.BusPadd;
                locdata->HpSetup.ArrayField.IoPadd+=hps.IoPadd;
                locdata->HpSetup.ArrayField.Io32Padd+=hps.Io32Padd;
                locdata->HpSetup.ArrayField.Mmio32Padd+=hps.Mmio32Padd;   
                locdata->HpSetup.ArrayField.Mmio32PfPadd+=hps.Mmio32PfPadd;
                locdata->HpSetup.ArrayField.Mmio64Padd+=hps.Mmio64Padd;
                locdata->HpSetup.ArrayField.Mmio64PfPadd+=hps.Mmio64PfPadd;
            }
            
        } else {
            //Use SDL PCI DATA object for padding...
            locdata->HpSetup.ArrayField.BusPadd=HpBrgSdlData->ResourcePadding[ptBus];
            locdata->HpSetup.ArrayField.IoPadd=HpBrgSdlData->ResourcePadding[ptIo];
            locdata->HpSetup.ArrayField.Io32Padd=HpBrgSdlData->ResourcePadding[ptIo32];
            locdata->HpSetup.ArrayField.Mmio32Padd=HpBrgSdlData->ResourcePadding[ptMmio32];   
            locdata->HpSetup.ArrayField.Mmio32PfPadd=HpBrgSdlData->ResourcePadding[ptMmio32pf];
            locdata->HpSetup.ArrayField.Mmio64Padd=HpBrgSdlData->ResourcePadding[ptMmio64];
            locdata->HpSetup.ArrayField.Mmio64PfPadd=HpBrgSdlData->ResourcePadding[ptMmio64pf];
        }
    }    
#else
    //Get global HP Padding Setup data of Aptio 4.x style setup
    Status=AmiPciGetSetupData(NULL,NULL,&locdata->HpSetup);
    ASSERT_EFI_ERROR(Status);
#endif    
//*/
    return Status;
    
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    HpcFindHpSlots
//
// Description:
//  This function find
//  information and initialize an instance of PCI Root Hotplug Controller
//  Initialization Protocol.
//
// Input:        PCI_HOST_BRG_DATA   *HostBrg - Pointer on Private Data
//              structure for which PCI Hot Plug Init Protocol going to
//              be initialized
//
// Output:        EFI_SUCCESS is OK
//              EFI_NOT_FOUND no Hotplug slots where found.
//
// Notes:        CHIPSET AND/OR BOARD PORTING NEEDED
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HpcFindSlots(PCI_HOST_BRG_DATA *HostBrg, PCI_ROOT_BRG_DATA *RootBrg)
{
    EFI_STATUS              Status=EFI_NOT_FOUND;
    UINTN                   i,j, idx;
    AMI_SDL_PCI_DEV_INFO    *brgdata=NULL;
    AMI_SDL_PCI_DEV_INFO    *sdltmp=NULL;
    AMI_SDL_PCI_DEV_INFO    *sdltmpParent=NULL;
    AMI_SDL_PCI_DEV_INFO    **sltdata;
    UINTN                   hpscnt, hpbcnt, sltcnt;
    UINTN                   *sltidxbuff;
    BOOLEAN                 br;
    PCI_DEVICE_SETUP_DATA   devs;
// EIP250562: PCH / IIO Resource reservation for Hot-Plug
    UINT8                   NbPcieIndex;
//-------------------------

    UpdateIioPcieIdxTable ();
    
    Status = pRS->GetVariable ( \
		    SOCKET_IIO_CONFIGURATION_NAME, \
		    &gEfiSocketIioVariableGuid, \
		    NULL, \
		    &IioRcVariableSize, \
		    &gIioConfiguration);
    ASSERT_EFI_ERROR(Status);
    
    Status = pRS->GetVariable ( \
		    PCH_RC_CONFIGURATION_NAME, \
		    &gEfiPchRcVariableGuid, \
		    NULL, \
		    &PchRcVariableSize, \
		    &gPchConfiguration);
    ASSERT_EFI_ERROR(Status);
// Intel Thunderbolt Support
#if Thunderbolt_SUPPORT
// Build fail if HOTPLUG_SUPPORT is enabled.
    Status = pRS->GetVariable (
		    L"Setup",
		    &gSetupGuid,
		    NULL,
		    &SetupDataSize,
		    &gSetupData);
    ASSERT_EFI_ERROR(Status);

    //
    // Get TBT INFO HOB
    //
    Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &gTbtInfoHob);
    ASSERT_EFI_ERROR(Status);

    gTbtInfoHob = GetNextGuidHob (&gTbtInfoHobGuid, gTbtInfoHob);
    if (gTbtInfoHob == NULL) {
        DEBUG((EFI_D_ERROR,"PciHp: gTbtInfoHob is NULL!!\n"));
    }
#endif
    
    DEBUG((EFI_D_INFO,"---------------------------------------------------------------\n"));
    DEBUG((EFI_D_INFO,"PciHp: Looking for HP Bridges of RB '%s'; SdlIndex 0x%X(%d)...\n",
            RootBrg->RbAslName,RootBrg->RbSdlIndex,RootBrg->RbSdlIndex));

    Status = EFI_NOT_FOUND;
    DEBUG((EFI_D_INFO,"PciDevCount = %d in SDL\n", gSdlPciData->PciDevCount));

    for(i=0,hpbcnt=0; i<gSdlPciData->PciDevCount; i++){
      sdltmp=&gSdlPciData->PciDevices[i];
      sltdata=NULL;
      DEBUG((EFI_D_INFO,"PciDev location @ B:%d|D:%d|F:%d %s ParentIndex:%d FixedBus flag:%d\n", sdltmp->Bus, sdltmp->Device, sdltmp->Function, sdltmp->AslName, sdltmp->ParentIndex, sdltmp->PciDevFlags.Bits.FixedBus));
      // Update Hotplug cap setting with Setup items
      sdltmpParent = &gSdlPciData->PciDevices[sdltmp->ParentIndex];
      if(sdltmpParent->PciDevFlags.Bits.RootBridgeType && sdltmpParent->PciDevFlags.Bits.FixedBus){
          DEBUG((EFI_D_INFO,"Parent = %s in SDL\n", sdltmpParent->AslName));
// PCH / IIO Resource reservation for Hot-Plug
        if ((sdltmpParent->Bus == 0 && sdltmp->Device == 0x1B) ||
            (sdltmpParent->Bus == 0 && sdltmp->Device == 0x1C) ||
            (sdltmpParent->Bus == 0 && sdltmp->Device == 0x1D)) {
            DEBUG((EFI_D_INFO,"Make sure Pci device is SB PCIE root port %s\n", sdltmp->AslName));
          idx = 0;
          if (sdltmp->Device == 0x1C) idx += sdltmp->Function;
          if (sdltmp->Device == 0x1D) idx += (sdltmp->Function + 8);
          if (sdltmp->Device == 0x1B) idx += (sdltmp->Function + 16);
          sdltmp->PciDevFlags.Bits.HotPlug = gPchConfiguration.PcieRootPortHPE[idx];
        } else {
          for (idx = 0; idx < IioPcieIdxTblLen; idx++) {
            if ((sdltmpParent->Bus == IioPcieIdxTable[idx].Bus) && (sdltmp->Device == IioPcieIdxTable[idx].Device)) {
              NbPcieIndex = IioPcieIdxTable[idx].Index;
              DEBUG((EFI_D_INFO,"Make sure Pci device is NB PCIE root port: %s of CPU socket: %d Setup Index: %d\n", sdltmp->AslName, sdltmpParent->Bus, NbPcieIndex));
              sdltmp->PciDevFlags.Bits.HotPlug = gIioConfiguration.SLOTHPCAP[NbPcieIndex];
              break;
            }
          }
        }
      }
        
      if( sdltmp->PciDevFlags.Bits.HotPlug && sdltmp->PciDevFlags.Bits.Pci2PciBridgeType){
          //Check if It has Setup and if HP feature enabled Selectively for this particular bridge...
          if(sdltmp->PciDevFlags.Bits.HasSetup){
            Status=AmiPciGetPciDevSetupData(&devs, sdltmp, i, FALSE);
            ASSERT_EFI_ERROR(Status);
            //If HP Disabled on HPBrg level - just keep going and don't update location data...
            if(devs.HpDisable) continue;
          }
          //This bridge produces HP Bus. Check if i has slots behind it...
          if(AmiSdlCheckIfParent(i, RootBrg->RbSdlIndex)){
            //So far... BRG with HP caps belongs to the right RB
            //Now check if it has slot s behind it and  how many
            //One slot is enough for the location entry in HostBrg->HpcData. 
            brgdata=sdltmp;
            
            Status=AmiSdlPciGetChildsOfParentIndex(&sltdata,&hpscnt,i);
            if(EFI_ERROR(Status)){
                if(Status==EFI_NOT_FOUND){
                    DEBUG((EFI_D_WARN,"\t!!!WARNING!!! HP Capable Bridge has no Children SdlIndex=0x%X(%d)!!!\n", i,i));
                  continue;
                } else return Status;
            }
            
            sltidxbuff=MallocZ(sizeof(UINTN)*hpscnt);
            if(sltidxbuff==NULL) return EFI_OUT_OF_RESOURCES;
                  
            for(j=0,br=TRUE,sltcnt=0; j<hpscnt; j++){
                //Try to check if there are any slots behind the bridge..
                sdltmp=sltdata[j];
                AmiSdlFindRecordIndex(sdltmp,&idx);
                if(sdltmp->PciDevFlags.Bits.OnBoard){
                  //It has to be slot!
                  DEBUG((EFI_D_WARN,"\t!!!WARNING Found OnBoard Device @HP Bus, SdlIndex 0x%X(%d)!!!\n", idx, idx));
                  continue;
                } else {
                  //Found a SLOT behind HP cap bridge!
                  
                  if(sdltmp->PciDevFlags.Bits.HasSetup){
                      Status=AmiPciGetPciDevSetupData(&devs, sdltmp, idx, FALSE);
                      ASSERT_EFI_ERROR(Status);
                  }
                  DEBUG((EFI_D_INFO,"\tFound Slot# 0x%X(%d), SdlIndex 0x%X(%d) @ B:%d|D:%d|F:%d; HpDisable=%d; ",
                        sdltmp->Slot, sdltmp->Slot,idx,idx, sdltmp->Bus, sdltmp->Device, sdltmp->Function, devs.HpDisable));
                  
                  //If HP Disabled on HPBrg level - just keep going and don't update location data...
                  if(sdltmp->PciDevFlags.Bits.HasSetup && devs.HpDisable) {
                      DEBUG((EFI_D_INFO,"Slot Skipped...\n"));
                      continue;
                  }
                  DEBUG((EFI_D_INFO,"\n"));
                  
                  if(br){
                      //One HPC_LOCATION per bridge.
                      hpbcnt++;
                      br=FALSE;
                  }
                  //but if other slots has SETUP Item we need to create/update setup VARs Associated for the slot
                  //so keep going until the last children of a bridge in that list
                  //For PCIe HP it is 1 slot per bridge, but for Std HPC it could be a couple...
                  //If each SHPC slot has Setup Option with Resource Padding for each slot we have to add each Padding
                  if(sdltmp->PciDevFlags.Bits.HasSetup){
                      sltidxbuff[sltcnt]=idx;
                      sltcnt++;
                  }
                }                
            } //For j...
            //Free memory allocated for the sdldata...
            if(sltdata!=NULL) pBS->FreePool(sltdata);
            //Now we have to generate PCI_DP down to this HP_BRG parent Root Bridge
            //If we found slots....
            if(!br) {
                DEBUG((EFI_D_INFO,"Updating HPData...Status="));
                Status=HpcUpdateLocationData(HostBrg,RootBrg,brgdata, i, sltcnt, sltidxbuff);
                DEBUG((EFI_D_INFO,"%r.\n", Status));
                   ASSERT_EFI_ERROR(Status);
            }
            
          }//if AmiSdlCheckIfParent() 
      }
      if(EFI_ERROR(Status)&& Status!=EFI_NOT_FOUND )break;
    }
    
    //now get memory buffer to store HP
    DEBUG((EFI_D_INFO,"---------------------------------------------------------------\n"));
    DEBUG((EFI_D_INFO,"Total found %d P2P HP Bridges with HP Slots. Returning %r\n", hpbcnt, Status));

    return Status;

}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    HpcInitGetRootHpcList
//
// Description:    GetRootHpcList() Function of the PCI Root Hotplug Controller
// Initialization Protocol. See PI 1.1 Spec or details
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HpcInitGetRootHpcList(IN EFI_PCI_HOT_PLUG_INIT_PROTOCOL      *This,
                                 OUT UINTN                              *HpcCount,
                                 OUT EFI_HPC_LOCATION                   **HpcList)
{
    EFI_STATUS          Status=EFI_SUCCESS;
    UINTN               i;
    EFI_HPC_LOCATION    *hpcl;
    EFI_HPC_LOCATION    *lp=NULL;
    PCI_HPC_DATA        *hpcdata=(PCI_HPC_DATA*)This;
//------------------------
    if(hpcdata==NULL) return EFI_INVALID_PARAMETER;

    DEBUG((EFI_D_INFO,"PciHb: Get Location - HpcLocCount=%d\n", hpcdata->HpcLocCount));
    DEBUG((EFI_D_INFO,"-------------------------------------------------\n"));

    //Allocate Buffer for HPC List
    hpcl=MallocZ(sizeof(EFI_HPC_LOCATION) * hpcdata->HpcLocCount);

    //Check conditions
    if(hpcl==NULL){
        Status=EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    //Fill data in RHPC List Data...
    for(i=0, lp=hpcl; i<hpcdata->HpcLocCount; i++, lp++){
        HPC_LOCATION_DATA   *locdata=hpcdata->HpcLocData[i];
    //----------------
        MemCpy(lp, &locdata->HpcLocation, sizeof(EFI_HPC_LOCATION));
        DEBUG((EFI_D_INFO,"  lp->HpCtrollerDP=[HID(%X)UID(%X)]; lp->HpBusDP[HID(%X)UID(%X)]\n",
            ((ACPI_HID_DEVICE_PATH*)lp->HpcDevicePath)->HID,((ACPI_HID_DEVICE_PATH*)lp->HpcDevicePath)->UID,
            ((ACPI_HID_DEVICE_PATH*)lp->HpbDevicePath)->HID,((ACPI_HID_DEVICE_PATH*)lp->HpbDevicePath)->UID));
    }
    DEBUG((EFI_D_INFO,"-------------------------------------------------\n"));

    *HpcCount=hpcdata->HpcLocCount;
    *HpcList=hpcl;
    DEBUG((EFI_D_INFO, __FUNCTION__" return %r\n", Status));
    
    return Status;
}


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS FindHpcLocData(  IN EFI_DEVICE_PATH_PROTOCOL *HpcDevicePath,
                            IN PCI_HPC_DATA             *HpcData,
                            OUT HPC_LOCATION_DATA       **HpcLocData )
{
    UINTN               i;
    HPC_LOCATION_DATA   *locdata;
//------------------

    for(i=0; i< HpcData->HpcLocCount; i++){
        locdata=HpcData->HpcLocData[i];
        if(DPCmp(HpcDevicePath, locdata->HpcLocation.HpcDevicePath) == 0 ){
            *HpcLocData=locdata;
            return EFI_SUCCESS;
        }
    }

    DEBUG((EFI_D_ERROR,"PciHp: Can't find matching HPC for DevicePath @ 0x%X \n",HpcDevicePath));

    return EFI_NOT_FOUND;

}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    HpcInitInitializeRootHpc
//
// Description:    InitializeRootHpc() Function of the PCI Root Hotplug Controller
// Initialization Protocol. See PI 1.1 Spec or details
//
// NOTE:
//  This function call will be used to overwrite default resource Padding settings.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HpcInitInitializeRootHpc(IN EFI_PCI_HOT_PLUG_INIT_PROTOCOL       *This,
                                    IN  EFI_DEVICE_PATH_PROTOCOL            *HpcDevicePath,
                                    IN  UINT64                              HpcPciAddress,
                                    IN  EFI_EVENT                             Event, OPTIONAL
                                    OUT EFI_HPC_STATE                        *HpcState)
{
    EFI_STATUS          Status;
    HPC_LOCATION_DATA   *locdata;
    PCI_HPC_DATA        *hpcdata=(PCI_HPC_DATA*)This;
//------------------------
    if(hpcdata==NULL) return EFI_INVALID_PARAMETER;

    //Find which exactly HPC PCI BUS Driver means..
    Status=FindHpcLocData(HpcDevicePath, hpcdata,&locdata);
    if(EFI_ERROR(Status)) return Status;

//TODO Change this call to outside lib OEM Porting function through Platform Init Protocol.
//   //Call Porting function to overwrite HP Padding Settings if needed;
//    PciPortSetOemPadding(hpcdata, locdata, HpcPciAddress);
//TODO...
    *HpcState=(EFI_HPC_STATE_INITIALIZED|EFI_HPC_STATE_ENABLED);

    return EFI_SUCCESS;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
VOID HpcFillDescriptor(ASLR_QWORD_ASD *Descriptor, HP_PADD_RES_TYPE PaddType, UINT64  Length, UINT32 Alignment){

//---------------------

    Descriptor->Hdr.HDR=0x8A;
    Descriptor->Hdr.Length=0x2B;

    Descriptor->_LEN=Length;
    if(PaddType==ptBus)Descriptor->_MAX=1;
    else if (Alignment > 1) Descriptor->_MAX = (1 << Alignment) - 1;
    else{
        if(IsPowerOfTwo(Length))Descriptor->_MAX=Length-1;
        else Descriptor->_MAX=1;
    }

    Descriptor->GFlags._MIF=1;
    Descriptor->GFlags._MAF=1;
    Descriptor->TFlags.TFLAGS=0;

    switch(PaddType){
        case ptBus:
            Descriptor->Type=ASLRV_SPC_TYPE_BUS;
            break;
        case ptIo:
            Descriptor->Type=ASLRV_SPC_TYPE_IO;
            break;
        case ptMmio32:
            Descriptor->Type=ASLRV_SPC_TYPE_MEM;
            Descriptor->_GRA=32;
            break;
        case ptMmio32pf:
            Descriptor->Type=ASLRV_SPC_TYPE_MEM;
            Descriptor->TFlags.TFLAGS=0x06;
            Descriptor->_GRA=32;
            break;
        case ptMmio64:   //4
            Descriptor->Type=ASLRV_SPC_TYPE_MEM;
            Descriptor->_GRA=64;
            break;
        case ptMmio64pf:  //5
            Descriptor->Type=ASLRV_SPC_TYPE_MEM;
            Descriptor->TFlags.TFLAGS=0x06;
            Descriptor->_GRA=64;
            break;
    }

}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: HpcGetResourcePadding
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS HpcGetResourcePadding(IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL *This,
                                 IN  EFI_DEVICE_PATH_PROTOCOL       *HpcDevicePath,
                                 IN  UINT64                         HpcPciAddress,
                                 OUT EFI_HPC_STATE                  *HpcState,
                                 OUT VOID                           **Padding,
                                 OUT EFI_HPC_PADDING_ATTRIBUTES     *Attributes)
{
    EFI_STATUS              Status;
    HPC_LOCATION_DATA       *locdata;
    UINTN                   i, cnt;
    ASLR_QWORD_ASD          *pd, *qw;
    PCI_HPC_DATA            *hpcdata=(PCI_HPC_DATA*)This;
    UINT8                   Bus, Dev, Fun;
// PCH / IIO Resource reservation for Hot-Plug
    UINT8                   PcieIndex, Socket, PortIndex;
// Intel Thunderbolt Support
#if Thunderbolt_SUPPORT
    UINTN                       RpDev;
    UINTN                       RpFunc;
#endif
//------------------------
    
    DEBUG((EFI_D_INFO, __FUNCTION__" Entry !!!\n"));
    
// PCH / IIO Resource reservation for Hot-Plug
    Status = pBS->LocateProtocol( \
		    &gEfiIioSystemProtocolGuid, \
		    NULL, \
		    &gIioSystemProtocol);

    Status = pRS->GetVariable ( \
		    SOCKET_IIO_CONFIGURATION_NAME, \
		    &gEfiSocketIioVariableGuid, \
		    NULL, \
		    &IioRcVariableSize, \
		    &gIioConfiguration);
    ASSERT_EFI_ERROR(Status);
    
    Status = pRS->GetVariable ( \
		    PCH_RC_CONFIGURATION_NAME, \
		    &gEfiPchRcVariableGuid, \
		    NULL, \
		    &PchRcVariableSize, \
		    &gPchConfiguration);
    ASSERT_EFI_ERROR(Status);
    
    if(hpcdata==NULL) return EFI_INVALID_PARAMETER;

    Bus = (UINT8)(HpcPciAddress >> 24);
    Dev = (UINT8)(HpcPciAddress >> 16);
    Fun = (UINT8)(HpcPciAddress >> 8);
    DEBUG((EFI_D_INFO,"Hotplug location: %x -> B:%x|D:%x|F:%x\n", HpcPciAddress, Bus, Dev, Fun));
    
    //Find which exactly HPC PCI BUS Driver means..
    Status=FindHpcLocData(HpcDevicePath, hpcdata,&locdata);
    if(EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR,"Get Hotplug resource information for this device %r\n", Status));
        return Status;
    }

    //Calculate how many ACPI_QW_DESC we need for Padding Info.
    for(i=0, cnt=0; i<ptMaxType; i++){
        if(i==ptIo32) continue;
        if(locdata->HpSetup.ARRAY[i]!=0) cnt++;
    }
    DEBUG((EFI_D_INFO,"Calculate how many ACPI_QW_DESC we need for Padding Info. cnt = %x\n", cnt));

    //Get memory for it.
    pd=MallocZ(sizeof(ASLR_QWORD_ASD)*cnt+sizeof(ASLR_EndTag));
    if (pd==NULL) {
        Status=EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    //fill out QW Resource Descriptors for resource requirements.
    DEBUG((EFI_D_INFO,"fill out QW Resource Descriptors for resource requirements.\n"));
    for(i=0, qw=pd; i<ptMaxType; i++){
        if(i==ptIo32)continue;
        if(locdata->HpSetup.ARRAY[i]!=0){
// PCH / IIO Resource reservation for Hot-Plug
            if ((Bus == 0 && Dev == 0x1B) || (Bus == 0 && Dev == 0x1C) || (Bus == 0 && Dev == 0x1D)){  // SB PCIE root port

                 switch (Dev) {
                    default:
                    case 0x1C:  // Port 0 ~ 7
                      PcieIndex = 0; // Start from 0
                      break;
                    case 0x1D:  // Port 8 ~ 15
                      PcieIndex = 8;  // Start from 8
                      break;
                    case 0x1B:  // Port 16 ~ 19
                      PcieIndex = 16;  // Start from 16
                      break;
                 }
                 
                 PcieIndex += Fun;
                 
                 switch(i){
                    case ptMmio32:
                    case ptMmio64:
                        HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i], gPchConfiguration.PcieMemRsvdalig[PcieIndex]);
                        break;
                    case ptMmio32pf:
                        HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i], gPchConfiguration.PciePFMemRsvdalig[PcieIndex]);
                        break;
                    case ptMmio64pf:
                        HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i], gPchConfiguration.Pcie64PFMemRsvdalig[PcieIndex]);
                        break;
                    default:
                        HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i], 1);
                        break;
                }
            } else if ((Dev == 0) || (Dev == 1) || (Dev == 2) || (Dev == 3)){
                for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
                  for (PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
                    if ((Bus == gIioSystemProtocol->IioGlobalData->IioVar.IioVData.SocketPortBusNumber[Socket][PortIndex]) &&
                        (Dev == gIioSystemProtocol->IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device)) {
                      switch (Socket) {
                        default:
                        case 0:
                          PcieIndex = 0;
                          break;
                        case 1:
                          PcieIndex = 21;
                          break;
                        case 2:
                          PcieIndex = 42;
                          break;
                        case 3:
                          PcieIndex = 63;
                          break;
                      }
                      
                      PcieIndex += PortIndex;

                      switch(i) {
                        case ptMmio32:
                        case ptMmio64:
                          HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i], gIioConfiguration.IioMemRsvdalig[PcieIndex]);
                          break;
                        case ptMmio32pf:
                          HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i], gIioConfiguration.IioPFMemRsvdalig[PcieIndex]);
                          break;
                        case ptMmio64pf:
                          HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i], gIioConfiguration.Iio64PFMemRsvdalig[PcieIndex]);
                          break;
                        default:
                          HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i], 1);
                      }
                   }
                }
              }
            } else {
                HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i], 1);
            }
// Intel Thunderbolt Support
// Build fail if HOTPLUG_SUPPORT is enabled.
#if Thunderbolt_SUPPORT
            if (gTbtInfoHob->TbtSelector != 0) {

              Status = GetTbtRpDevFun (gTbtInfoHob->TbtSelector - 1, &RpDev, &RpFunc);

           	  if ((Bus == gTbtInfoHob->Bus) && (Dev == RpDev) && (Fun == 0)) {
                switch(i) {
                  case ptMmio32:
                  case ptMmio64:
                    HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i], gSetupData.TbtPcieMemAddrRngMax);
                    break;
                  case ptMmio32pf:
                  case ptMmio64pf:
                    HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i], gSetupData.TbtPciePMemAddrRngMax);
                    break;
                  default:
                    HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i], 1);
                }
              }
            }
#endif            
            DEBUG((EFI_D_INFO,"Resource type: %x. Resource size: %lx\n", i, locdata->HpSetup.ARRAY[i]));
            qw++;
        }
    }

    ((ASLR_EndTag*)qw)->Hdr.HDR=ASLV_END_TAG_HDR;
    ((ASLR_EndTag*)qw)->Chsum=0;

    *Padding=pd;
    *HpcState=(EFI_HPC_STATE_INITIALIZED|EFI_HPC_STATE_ENABLED);
    *Attributes=EfiPaddingPciBus;

    return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    HpcInitProtocol
//
// Description:    This function will collect all information HP related
// information and initialize an instance of PCI Root Hotplug Controller
// Initialization Protocol.
//
// Input:        PCI_HOST_BRG_DATA   *HostBrg - Pointer on Private Data
//              structure for which PCI Hot Plug Init Protocol going to
//              be initialized
//
// Output:        EFI_SUCCESS is OK
//              EFI_NOT_FOUND no Hotplug slots where found.
//
// Notes:        CHIPSET AND/OR BOARD PORTING NEEDED
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HpcInstallHpProtocol(PCI_HOST_BRG_DATA *HostBrg)
{
    EFI_STATUS          Status;
//---------------------------

    Status=AmiSdlInitBoardInfo();
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;

    DEBUG((EFI_D_INFO,"PciHP: "__FUNCTION__"()... "));

    if( !HpcCheckHpCompatibleHost(HostBrg)) { 
        DEBUG((EFI_D_ERROR,"System is NOT HP CAPABLE\n"));
        return EFI_SUCCESS;
    }    

    //So far so good... Init Protocol Instance Functions
    HostBrg->HpcData->RootHotplugProtocol.GetRootHpcList=HpcInitGetRootHpcList;
    HostBrg->HpcData->RootHotplugProtocol.InitializeRootHpc=HpcInitInitializeRootHpc;
    HostBrg->HpcData->RootHotplugProtocol.GetResourcePadding=HpcGetResourcePadding;

    Status=pBS->InstallMultipleProtocolInterfaces(
        &HostBrg->HbHandle,        //it is NOT NULL now
        &gEfiPciHotPlugInitProtocolGuid, &HostBrg->HpcData->RootHotplugProtocol,
        NULL);                                    //terminator
    if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR,"Status=%r\n", Status));
    }

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

