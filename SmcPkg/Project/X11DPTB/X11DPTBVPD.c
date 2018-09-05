//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//     Rev. 1.00        From SmcPkg\Module\SmcDevReport\SmcVPD.c 1.16
//       Reason:        
//       Auditor:       Stephen Chen
//       Date:          Sep/29/16
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  SmcVPD.c
//
// Description:	Supermicro OOB VPD feature source file.
//
//<AMI_FHDR_END>
//**********************************************************************

#include "Token.h"
//#ifdef DYNAMIC_MMCFG_BASE_FLAG 
//#include <Library/PciExpressLib.h> //AptioV server override: dynamic MMCFG base change support.
//#endif
#include "X11DPTBVPD.h"

extern EFI_IPMI_TRANSPORT	*IpmiTransport;

//EFI_DRIVER_ENTRY_POINT (SmcVPDEntry);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  SmcFindPciCapId
//
// Description: 
//  Find the Offset to a given Capabilities ID
//  CAPID list:
//             0x01 = PCI Power Management Interface
//             0x04 = Slot Identification
//             0x05 = MSI Capability
//             0x10 = PCI Express Capability
//             0x03 = VPD Capability
//
// Input:       
//      IN UINTN DeviceBase - PCI base address to search for
//      IN UINT8 CapId - CAPID to search for
//
// Output:      
//      0 - CAPID not found
//      Other - CAPID found, Offset of desired CAPID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8
SmcFindPciCapId (
  IN UINTN   DeviceBase,
  IN UINT8   CapId
  )
{
    UINT8 CapHeaderOffset;
    UINT8 CapHeaderId;

    CapHeaderOffset = 0x34;
    
    ///
    /// Get Capability Header, A pointer value of 00h is used to indicate the last capability in the list.
    ///
    CapHeaderId = 0;
    CapHeaderOffset = MmioRead8(DeviceBase + CapHeaderOffset) & ((UINT8) ~(BIT0 | BIT1));
    while (CapHeaderOffset != 0 && CapHeaderId != 0xFF) {
      CapHeaderId = MmioRead8(DeviceBase + CapHeaderOffset);
      if (CapHeaderId == CapId) {
        return CapHeaderOffset;
      }
      ///
      /// Each capability must be DWORD aligned.
      /// The bottom two bits of all pointers (including the initial pointer at 34h) are reserved
      /// and must be implemented as 00b although software must mask them to allow for future uses of these bits.
      ///
      CapHeaderOffset = MmioRead8(DeviceBase + CapHeaderOffset + 1) & ((UINT8) ~(BIT0 | BIT1));
    }    
    
    return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  GetVpdData
//
// Description: 
//  Get the VPD data from given PCI address
//
// Input:       
//      IN UINTN    PciAddress - The PCI address with VPD capability offset of the LAN device which support PCI VPD feature.
//      IN UINT16   offset - The VPD data offset
//      IN OUT VOID vpddata - The VPD data point
//      IN UINT8    DataType - The return data size(VpdDataWidth8 / VpdDataWidth32)
//
// Output:
//      EFI_STATUS - Get VPD data success or fail
//      VPD data - vpddata
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GetVpdData (
  IN     UINTN    PciAddress,
  IN     UINT16   offset,
  IN OUT VOID     *VpdData,
  IN     UINT8    DataType
  )
{
    UINT16	counter = 0;
    EFI_STATUS Status = EFI_SUCCESS;
	
    MmioWrite16(PciAddress, offset);
    while( !(MmioRead16(PciAddress) & 0x8000) ){
        if(counter > 10000){
            Status = EFI_LOAD_ERROR;
            break;	//Prevant read VPD but not response 
        }
        counter++;
    }
    
    if (EFI_ERROR(Status)){
        TRACE ((TRACE_ALWAYS, "VPD_DEBUG : VPD offset:0x%x data:failed\n", offset));
        return Status;
    } else {
	if(DataType == VpdDataWidth32){
	    *(volatile UINT32*)VpdData = MmioRead32(PciAddress+2);
	    //TRACE ((TRACE_ALWAYS, "VPD_DEBUG : VPD offset:0x%04x data:0x%08x\n", offset, *(UINT32*)VpdData));
	} else if(DataType == VpdDataWidth8){
	    *(volatile UINT8*)VpdData = MmioRead8(PciAddress+2);
	    //TRACE ((TRACE_ALWAYS, "VPD_DEBUG : VPD offset:0x%04x data:0x%02x\n", offset, *(UINT8*)VpdData));
	}
    }
			
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  HandleLanVpd
//
// Description: 
//  Get whole VPD data from PCI device
//
//  VPD Data Structure
//    Large Resource Type ID String tag(02h)
//      ID String Length(2 Byte)
//      ID String Data
//    Large Resource Type VPD-R Tag(10h)
//      VPD-R Length(2 Byte)
//      VPD-R Data
//    Large Resource Type VPD-W Tag(10h)
//      VPD-W Length(2 Byte)
//      VPD-W Data  
//    Small Resource Type End Tag(0Fh)
//
// Input:       
//      IN UINTN PciAddress - The PCI address with VPD capability offset of the LAN device which support PCI VPD feature.
//      IN OUT VPD_DATA LanVpd - The VPD data point for store VPD data
//      IN OUT SN_DATA  LanSN - The data structure for store LAN serial number
//      IN UINT16 SlotInfo - LAN device slot information, high byte : slot type, low byte slot id.
//
// Output:
//      EFI_STATUS - Get VPD data success or fail
//      VPD data - LanVpd
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
HandleLanVpd (
  IN UINTN        PciAddress,
  IN OUT VPD_DATA *LanVpd,
  IN OUT UINT8    *LanMAC
  )
{
    EFI_STATUS    Status = EFI_SUCCESS;
    UINT8         *vpd;
    UINT16        vpd_point, vpd_index, i, j;
    UINT16        vpd_data_leng;
    UINT32        data32;
    BOOLEAN       getVpdR = FALSE, getVpdW = FALSE;
    UINT16        LanMACPoint, LanMACKey;
    
    TRACE ((TRACE_ALWAYS, "VPD_DEBUG : Handle LAN VPD Start.\n"));
    
    vpd = &LanVpd->data[LanVpd->header.offset];	//Point to the total VPD data buffer
    
    //Get ID string data
    Status = GetVpdData(PciAddress, 0, (VOID*)&data32, VpdDataWidth32);
    if (EFI_ERROR(Status)) {
        TRACE ((TRACE_ALWAYS, "VPD_DEBUG : Get ID string failed.\n"));
        return Status;
    } else {
        *(UINT32*)(vpd) = data32;		    
    }
    
    if(vpd[0] != PCI_VPD_LRDT_ID_STRING) {
        TRACE ((TRACE_ALWAYS, "VPD_DEBUG : Missing VPD ID string\n"));
        return EFI_NOT_FOUND;
    } else {
        vpd_data_leng = *(UINT16*)(vpd+1);	//ID String length offset
        vpd_point = 3;				//ID String data offset
        vpd_index = 4;				//Next reading index
        TRACE ((TRACE_ALWAYS, "VPD_DEBUG : VPD ID string : 0x%x, Length:0x%x\n", vpd[0], vpd_data_leng));
    }

    for(i=0; i < (vpd_data_leng/4); i++) {      //ID string data
        Status = GetVpdData(PciAddress, vpd_index+i*4, (VOID*)&data32, VpdDataWidth32);
        *(UINT32*)(vpd+vpd_index+i*4) = data32; //Start from offset 0x04
    }
    if((vpd_data_leng%4)) {                    //ID string data
        Status = GetVpdData(PciAddress, vpd_index+(vpd_data_leng/4)*4, (VOID*)&data32, VpdDataWidth32);
        *(UINT32*)(vpd+vpd_index+(vpd_data_leng/4)*4) = data32;
    }
    vpd_point+=vpd_data_leng;
    vpd_index+=vpd_data_leng;    //Next reading index

    if((vpd_data_leng%4)) {
        vpd_index+=(4 - vpd_data_leng%4);
    }
    
    //Get VPD-R and VPD-RW
    Status = GetVpdData(PciAddress, vpd_index, (VOID*)&data32, VpdDataWidth32);
    *(UINT32*)(vpd+vpd_index) = data32;

    while(vpd[vpd_point] != PCI_VPD_SRDT_END){
	    
        vpd_data_leng =  *(UINT16*)(vpd+vpd_point+1);	//Point to VPD-R or VPD-RW data length offset
	
        switch(vpd[vpd_point]){
            case PCI_VPD_LRDT_RO_DATA: //Get VPD-R Data
        	TRACE ((TRACE_ALWAYS, "VPD_DEBUG : VPD VPD-R Tag:0x%x, Length:0x%04x\n", vpd[vpd_point], vpd_data_leng));
        	getVpdR = TRUE;
        	getVpdW = FALSE;
        	break;

            case PCI_VPD_LRDT_RW_DATA: //Get VPD-W Data
        	TRACE ((TRACE_ALWAYS, "VPD_DEBUG : VPD VPD-W Tag:0x%x, Length:0x%04x\n", vpd[vpd_point], vpd_data_leng));
        	getVpdW = TRUE;
        	getVpdR = FALSE;
        	break;
                
            default:
        	vpd_data_leng = 0;    //If tag not equal VPD-R or VPD-W, set length to 0 and break later
        	getVpdR = FALSE;
        	getVpdW = FALSE;
        	Status = EFI_INVALID_PARAMETER;
        	break;
        }
        
        if(vpd_data_leng == 0) break;//Break to prevent endless reading VPD data

        //Get LAN MAC address search offset which start from VPD-R data
        if(getVpdR || getVpdW) {
            LanMACPoint = vpd_point;
        }
        
        vpd_point+=3;	//VPD-R or VPD-RW Data offset

        for(i=0; i < (vpd_data_leng/4); i++) {
            Status = GetVpdData(PciAddress, vpd_index+i*4, (VOID*)&data32, VpdDataWidth32);
            *(UINT32*)(vpd+vpd_index+i*4) = data32;
        }
        if((vpd_data_leng%4)) {
            Status = GetVpdData(PciAddress, vpd_index+(vpd_data_leng/4)*4, (VOID*)&data32, VpdDataWidth32);
            *(UINT32*)(vpd+vpd_index+(vpd_data_leng/4)*4) = data32;
        }

        //Copy LAN serial number to LanMAC buffer
        if(getVpdR || getVpdW) {
            LanMACKey = 0;
            for(i=0; i < (vpd_data_leng -1); i++) {
                if(getVpdR) {
                    if( *(UINT16*)(vpd+LanMACPoint+i) == 0x4156) {    //"VA"
                        LanVpd->header.TotalLanOfAOC = *(UINT8*)(vpd+LanMACPoint+i+3) - 0x30; //Transfer from ASCII to HEX
                        TRACE ((TRACE_ALWAYS, "VPD_DEBUG : Found Multi-function LAN, total %x LAN of this AOC.\n", LanVpd->header.TotalLanOfAOC));
                    }
                    if( *(UINT16*)(vpd+LanMACPoint+i) == 0x3256) {    //"V2"
                        LanMACKey = LanMACPoint+i;
                        TRACE ((TRACE_ALWAYS, "VPD_DEBUG : Found LAN1 MAC offset : 0x%04x\n", LanMACKey));
                    }
                }
            }
            if(getVpdR) 
            {
                if(LanMACKey) 
                {
                    for(j=0; j < LanVpd->header.TotalLanOfAOC; j++)
                    {
                        TRACE ((TRACE_ALWAYS, "VPD_DEBUG : LAN%x MAC is : ",(j+1)));
                        for(i=0; i < vpd[LanMACKey+2]; i++) {
                	    LanMAC[(j*12)+i] = vpd[LanMACKey+3+i];
                	    TRACE ((TRACE_ALWAYS, "%02x ", LanMAC[(j*12)+i]));
                        }
                        LanMACKey += (vpd[LanMACKey+2]+3);
                        TRACE ((TRACE_ALWAYS, "\n"));
                    }
                    TRACE ((TRACE_ALWAYS, "\n"));
                } else {
                    TRACE ((TRACE_ALWAYS, "VPD_DEBUG : Not found MAC address data\n"));
                    return Status;
                }
            }
        }
        
        vpd_point+=vpd_data_leng;
        vpd_index+=vpd_data_leng;    //Next reading index
	
        if((vpd_data_leng%4)) {
            vpd_index+=(4 - vpd_data_leng%4);
        }

        Status = GetVpdData(PciAddress, vpd_index, (VOID*)&data32, VpdDataWidth32);
        *(UINT32*)(vpd+vpd_index) = data32;

    } TRACE ((TRACE_ALWAYS, "VPD_DEBUG : VPD End Tag : 0x%x\n", vpd[vpd_point]));
    
    if (EFI_ERROR(Status)) {
        TRACE ((TRACE_ALWAYS, "VPD_DEBUG : Get VPD data failed\n"));
        return Status;
    }   

    LanVpd->header.offset += (vpd_point+1);		//Point to next LAN VPD data
    TRACE ((TRACE_ALWAYS, "VPD_DEBUG : Handle LAN VPD End.\n"));
    
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  SmcVPDDataCollect
//
// Description:
//  Smc VPD Data collection.
//  Search and collect VPD data from onboard LAN and add-on LAN card.
//  The VPD data will be send to BMC after getting it from whole LAN on M/B.
//
// Steps :
//  1. Check IPMI protocol installed.
//  2. Search all PCI device to find LAN device.
//  3. Check the LAN has VPD capability. 
//  4. Get VPD data from LAN device.
//  5. Upload whole VPD data to BMC.
//
// Input:
//      IN EFI_HANDLE ImageHandle
//                  - Image handle
//      IN EFI_SYSTEM_TABLE *SystemTable
//                  - System table pointer
//
// Output:
//      EFI_STATUS (Return Value)
//                  = EFI_SUCCESS if successful
//                  = or other valid EFI error code
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
X11DPTBVPDDataCollect(
    IN UINT8		LanBus,
    IN OUT UINT8	*LanMACBuffer
  )
{
    EFI_STATUS    Status = EFI_SUCCESS;
    VPD_DATA	  SmcVpd;
    UINT8         CapPtr;
    UINTN         DeviceBase;

    TRACE ((TRACE_ALWAYS, "VPD_DEBUG : InitializeSmcVPD Start.\n"));  

    //Allocate 64k memory buffer for VPD data
    Status = gBS->AllocatePool(EfiBootServicesData, VPD_TOTAL_DATA_SIZE, &SmcVpd.data);
    if (EFI_ERROR(Status)) {
        TRACE ((TRACE_ALWAYS, "VPD_DEBUG : Can't locate memory for VPD data\n"));
        return Status;
    } else {
        gBS->SetMem(SmcVpd.data, VPD_TOTAL_DATA_SIZE, 0x00); 
        TRACE ((TRACE_ALWAYS, "VPD_DEBUG : Locate memory for VPD data successful\n"));    
    }

    //Initial VPD data structure header
    TRACE ((TRACE_ALWAYS, "VPD_DEBUG : Initial VPD data structure header\n"));  
    MemCpy(&SmcVpd.header.Signature, "_VPD", 4);
    SmcVpd.header.Version = VPD_VERSION;
    SmcVpd.header.offset = 0;
    SmcVpd.header.NumOfLan = 0;
    SmcVpd.header.NumOfAOCLan = 0;
    SmcVpd.header.TotalLanOfAOC = 0;

    //Get VPD data from SIOM
    DeviceBase = (UINTN)MmPciAddress(0, LanBus, 0, 0, 0);
    CapPtr =  SmcFindPciCapId (DeviceBase, EFI_PCI_CAPABILITY_ID_VPD);
    TRACE ((-1, "VPD_DEBUG : CapPtr offset:0x%02x\n", CapPtr));
    TRACE ((-1, "VPD_DEBUG : VPD data point offset:%04x\n", SmcVpd.header.offset));
    Status = HandleLanVpd(DeviceBase + CapPtr + 2, &SmcVpd, LanMACBuffer);
                    
    SmcVpd.header.FileSize = SmcVpd.header.offset;
    TRACE ((TRACE_ALWAYS, "VPD_DEBUG : VPD file size is : 0x%04x\n", SmcVpd.header.FileSize));
    
    TRACE ((TRACE_ALWAYS, "VPD_DEBUG : InitializeSmcVPD End.\n"));  
    gBS->FreePool(SmcVpd.data);
    return Status;
}

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2013, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
