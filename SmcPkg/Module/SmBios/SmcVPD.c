//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.04
//    Bug Fix:  Skip VPD data when VPD is not from SMC.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Mar/01/2017
//
//  Rev. 1.03
//    Bug Fix:  Skip empty VPD data and fill VB tag when SMC crad with VPD but no VB tag.
//    Reason:   IPMI need VB tag but can't find in AOC-S25G-m2S and AOC-S100G-m2C
//    Auditor:  Kasber Chen
//    Date:     Feb/24/2017
//
//  Rev. 1.02
//    Bug Fix:  Patch BIOS can't identify VPD data of AOC-S25G-m2S and AOC-S100G-m2C.
//    Reason:   Both AOC-S25G-m2S(Mellanox ConnectX4LX 4117) and AOC-S100G-m2C(Mellanox ConnectX4 4115) doesn't has 
//              Large resource type VPD-W Tag and then cause the code can't identify its VPD correctly.
//    Auditor:  Jimmy Chiu
//    Date:     Jan/10/2017
//
//  Rev. 1.01
//    Bug Fix:  Fixed system hang when plug the addon card which contained the non SMC format VPD data.
//    Reason:   Maximum SMBIOS Type 40 record size is 256 bytes (SmbiosHeader + AssetHeader + Vpddata).
//    Auditor:  Jimmy Chiu
//    Date:     Sep/06/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/09/2016
//
//****************************************************************************
//****************************************************************************
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Library/MmPciBaseLib.h>
#include <PciBus.h>
#include <AmiLib.h>
#include <Token.h>
#include "SmcVPD.h"

UINT8
SmcFindPciCapId(
    IN	UINT8	bus,
    IN	UINT8	dev,
    IN	UINT8	fun,
    IN	UINT8	reg
)
{
    UINT8	CapHeaderOffset;
    UINT8	CapHeaderId = 0;
    UINT8	i;

    CapHeaderOffset = 0x34;
///
/// Get Capability Header, A pointer value of 00h is used to indicate the last capability in the list.
///
    CapHeaderOffset = *(UINT8*)MmPciAddress(0, bus, dev, fun, CapHeaderOffset) & ~(BIT0 | BIT1);
    for(i = 0; (i < 20) && (CapHeaderOffset != 0) && (CapHeaderId != 0xFF); i++){
	CapHeaderId = *(UINT8*)MmPciAddress(0, bus, dev, fun, CapHeaderOffset);
	if(CapHeaderId == EFI_PCI_CAPABILITY_ID_VPD)
	    return	CapHeaderOffset;
///
/// Each capability must be DWORD aligned.
/// The bottom two bits of all pointers (including the initial pointer at 34h) are reserved
/// and must be implemented as 00b although software must mask them to allow for future uses of these bits.
///
	CapHeaderOffset = *(UINT8*)MmPciAddress(0, bus, dev, fun, CapHeaderOffset + 1) & ~(BIT0 | BIT1);
    }

    return 0;
}

EFI_STATUS
GetVpdData(
    IN	UINT8	bus,
    IN	UINT8	dev,
    IN	UINT8	fun,
    IN	UINT8	reg,
    IN	UINT16	Offset,
    IN OUT	UINT32	*data32
)
{
    EFI_STATUS	Status = EFI_NO_RESPONSE;
    UINT16	i, j;
    UINT32	Data[2];

    DEBUG((-1, "GetVpdData Start.\n"));
    
    for(j = 0; j < 2; j++){
	*(UINT16*)MmPciAddress(0, bus, dev, fun, reg) = (Offset & ~(0x03)) + (4 * j);
	for(i = 0; i < 10000; i++){
	    if(*(UINT16*)MmPciAddress(0, bus, dev, fun, reg) & 0x8000){
		Status = EFI_SUCCESS;
		Data[j] = *(UINT32*)MmPciAddress(0, bus, dev, fun, reg + 2);
		break;
	    }
	}
	if(Status)	return	EFI_NO_RESPONSE;
    }

    if((Offset % 4) == 0)	*data32 = Data[0];
    else if((Offset % 4) == 1)	*data32 = (Data[0] >> 8) | (Data[1] << 24);
    else if((Offset % 4) == 2)	*data32 = (Data[0] >> 16) | (Data[1] << 16);
    else if((Offset % 4) == 3)	*data32 = (Data[0] >> 24) | (Data[1] << 8);
    
    DEBUG((-1, "GetVpdData End. Data = %x.\n", *data32));
    return EFI_SUCCESS;
}

EFI_STATUS
VPDDataCollect(
    IN	UINT8	bus,
    IN	UINT8	dev,
    IN	UINT8	fun,
    IN	UINT8	reg,
    IN	UINT8	SlotType,
    IN	UINT8	SlotID,
    IN OUT	UINT8	*VPD_Data,
    IN OUT	UINT16	*VPD_Len
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	i, SMC_Flag = 0;
    UINT16	VPD_VB_Offset = 0, VPD_VB_Len = 0, VPD_Offset;
    UINT16      VPD_ID_Offset = 0, VPD_ID_Len = 0;
    UINT32	Data32;
    
    DEBUG ((-1, "VPDDataCollect Start.\n"));
// calculate total VPD lan
    for(i = 0; i < 3; i++){
        Status = GetVpdData(bus, dev, fun, reg, *VPD_Len, &Data32);
        if(Status)      return EFI_UNSUPPORTED;

        if((UINT8)Data32 == PCI_VPD_LRDT_ID_STRING){    //0x82
            VPD_ID_Offset = *VPD_Len;
            VPD_ID_Len = (UINT16)(Data32 >> 8) + 1 + 2;
            *VPD_Len += (UINT16)(Data32 >> 8) + 1 + 2;	// the "1" is Tag and "2" is len
        }else if((UINT8)Data32 == PCI_VPD_LRDT_RO_DATA){//0x90
            *VPD_Len += (UINT16)(Data32 >> 8) + 1 + 2;	// the "1" is Tag and "2" is len
        }else if((UINT8)Data32 == PCI_VPD_LRDT_RW_DATA){//0x91
            VPD_VB_Offset = *VPD_Len;
            VPD_VB_Len = (UINT16)(Data32 >> 8) + 1 + 2;
            *VPD_Len += (UINT16)(Data32 >> 8) + 1 + 2;	// the "1" is Tag and "2" is len
        }
    }
    DEBUG((-1, "VPD total len = %x.\n", *VPD_Len));
    DEBUG((-1, "VPD_ID_Offset = %x.\n", VPD_ID_Offset));
    DEBUG((-1, "VPD_VB_Offset = %x.\n", VPD_VB_Offset));

    if(*VPD_Len > 200)  return EFI_UNSUPPORTED; //Maximum SMBIOS Type 40 record size is 256 bytes (SmbiosHeader + AssetHeader + Vpddata)

    Status = GetVpdData(bus, dev, fun, reg, *VPD_Len, &Data32);
    if(Status || ((UINT8)Data32 != 0x78))       return EFI_UNSUPPORTED;
//collect VPD
    for(VPD_Offset = 0; VPD_Offset < *VPD_Len; VPD_Offset += 4){
        Status = GetVpdData(bus, dev, fun, reg, VPD_Offset, &Data32);
        if(Status)      return EFI_UNSUPPORTED;
	
        *(UINT32*)&(VPD_Data[VPD_Offset]) = Data32;
    }
//skip "Example VPD"
    for(i = 0; i < VPD_ID_Len; i++){
        if(!MemCmp(&VPD_Data[VPD_ID_Offset + i], "Supermicro", 10)){
            DEBUG((-1, "SMC card with VPD data.\n"));
            SMC_Flag = 1;
            break;
        }
    }
// if VPD is not from SMC, skip it
    if(!SMC_Flag)       return EFI_UNSUPPORTED;
// fill SlotID
    for(i = 0; i < VPD_VB_Len; i++){
        if(*(UINT16*)&(VPD_Data[VPD_VB_Offset + i]) == 0x4256){	//Tag = VB
            DEBUG((-1, "Find VB tag, fill slot type and ID.\n"));
            VPD_Data[VPD_VB_Offset + i + 3] = SlotType;
            VPD_Data[VPD_VB_Offset + i + 4] = SlotID;
        }
    }
// if LAN card is SMC but no VB tag in VPD, fill VB tag for IPMI
    if(!VPD_VB_Len && SMC_Flag){
        DEBUG((-1, "SMC card but no VB tag, create VB tag.\n"));
        *(UINT8*)&VPD_Data[*VPD_Len] = 0x91;    //RW signature
        (*VPD_Len)++;
        *(UINT16*)&VPD_Data[*VPD_Len] = 5;      //RW length
        (*VPD_Len) += 2;
        *(UINT16*)&VPD_Data[*VPD_Len] = 0x4256; //VB tag
        (*VPD_Len) += 2;
        VPD_Data[*VPD_Len] = 0x02;              // SlotInfoLen
        (*VPD_Len)++;
        VPD_Data[*VPD_Len] = SlotType; //Slot Type
        (*VPD_Len)++;
        VPD_Data[*VPD_Len] = SlotID;
        (*VPD_Len)++;
        VPD_Data[*VPD_Len] = PCI_VPD_STIN_END;  // end of tag
    }
    
    *VPD_Len += 1;	//end byte of VPD

    for(i = 0; i < *VPD_Len; i++){
        if(!(i % 16))	DEBUG((-1, "\n"));
        DEBUG((-1, " %x", VPD_Data[i]));
    }

    return EFI_SUCCESS;
}

EFI_STATUS
SmcVPDPkgCollect(
    IN	UINT8	bus,
    IN	UINT8	dev,
    IN	UINT8	fun,
    IN	UINT8	SlotType,
    IN	UINT8	SlotID,
    IN OUT	UINT8	*VPD_Data,
    IN OUT	UINT16	*VPD_Len
)
{
    EFI_STATUS	Status;
    UINT8	CapPtr;
    
    DEBUG((-1, "InitializeSmcVPD Start.\n"));

    *VPD_Len = 0;
    if(*(UINT16*)MmPciAddress(0, bus, dev, fun, 0x0a) != 0x200)
	return	EFI_UNSUPPORTED;	//not LAN device

    //Scan all PCI device for get VPD data which support it
    CapPtr = SmcFindPciCapId(bus, dev, fun, 0);
    if(!CapPtr)	return	EFI_UNSUPPORTED;	// no VPD data

    Status = VPDDataCollect(bus, dev, fun, CapPtr + 2, SlotType, SlotID, VPD_Data, VPD_Len);
    if(Status)	return	EFI_UNSUPPORTED;	// get VPD fail

    DEBUG((-1, "InitializeSmcVPD End.\n"));
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
