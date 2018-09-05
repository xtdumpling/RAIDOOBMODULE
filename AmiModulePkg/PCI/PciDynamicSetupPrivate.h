//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************
// Revision History
// ----------------
//$Log:  $
// 

#ifndef _PCI_SETUP_PRIVATE_H_
#define _PCI_SETUP_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif
/****** DO NOT WRITE ABOWE THIS LINE *******/

#include <Efi.h>
#include <UefiHii.h>
#include <PciBus.h>
#include <Guid/MdeModuleHii.h>


#define SDATA_TYPE_DEVICE		0
#define SDATA_TYPE_PCIE1		1
#define SDATA_TYPE_PCIE2		2
#define SDATA_TYPE_HOTPL		3


typedef union {
	UINT16	VAR_ID;
	struct _IdField{
		UINT16	PciDevIndex		:12;	
		UINT16	DataType		:3;
		UINT16	Always1			:1;
	}IdField;
} PCI_VAR_ID;

//Define padding type for local function
#define optBus		1
#define optIo		2
#define optMmio		3
#define optMmio64	4

// PCI device driver config form data structure
typedef struct _PCI_DEV_FORM_DATA {
//--Device Form--------------------
    VOID *DevStartOpCodeHandle;
    VOID *DevEndOpCodeHandle;
    EFI_IFR_GUID_LABEL  *DevStartLabel;
    EFI_IFR_GUID_LABEL  *DevEndLabel;
    VOID *PciLatencyOpCodeHandle;
    VOID *PciXLatencyOpCodeHandle;
//--Pcie1 Form---------------------
    VOID *Pcie1StartOpCodeHandle;
    VOID *Pcie1EndOpCodeHandle;
    EFI_IFR_GUID_LABEL  *Pcie1StartLabel;
    EFI_IFR_GUID_LABEL  *Pcie1EndLabel;
    VOID *MaxPayLoadOpCodeHandle;
    VOID *MaxReadReqOpCodeHandle;
    VOID *AspmModeOpCodeHandle;
    VOID *LnkTrainRetryOpCodeHandle;
//--Pcie2 Form---------------------
    VOID *Pcie2StartOpCodeHandle;
    VOID *Pcie2EndOpCodeHandle;
    EFI_IFR_GUID_LABEL  *Pcie2StartLabel;
    EFI_IFR_GUID_LABEL  *Pcie2EndLabel;
    VOID *ComplTimeoutOpCodeHandle;
    VOID *LnkSpeedOpCodeHandle;
 //--Hot plug Form-----------------
    VOID *PciHpStartOpCodeHandle;
    VOID *PciHpEndOpCodeHandle;
    EFI_IFR_GUID_LABEL  *PciHpStartLabel;
    EFI_IFR_GUID_LABEL  *PciHpEndLabel;
    VOID *BusPaddOpCodeHandle;
    VOID *IoPaddOpCodeHandle;
    VOID *Mmio32OpCodeHandle;
    VOID *Mmio32PfOpCodeHandle;
    //VOID *Mmio64OpCodeHandle;
    VOID *Mmio64PfOpCodeHandle;
} PCI_DEV_FORM_DATA;

typedef struct _PCI_DEV_SETUP_GOTO_DATA{
	PCI_DEV_INFO				*PciDevInfo;
	PCI_DEV_INFO				*ParentBrgInfo;
	AMI_SDL_PCI_DEV_INFO		*PciSdlInfo;
	UINTN						PciDevIdx;
    EFI_STRING_ID   			GotoStringId;
    EFI_STRING_ID   			GotoHelpStringId;
    UINT16          			GotoKey;
    BOOLEAN						ShowItem;
} PCI_DEV_SETUP_GOTO_DATA;


// main SIO form data structure
typedef struct _PCI_IFR_INFO {
    VOID 					*StartOpCodeHandle;
    VOID 					*EndOpCodeHandle;
    EFI_IFR_GUID_LABEL  	*StartLabel;
    EFI_IFR_GUID_LABEL  	*EndLabel;
//Like T_ITEM_LIST
    UINTN					PciDevInitCnt;
    UINTN					PciDevCount;
    PCI_DEV_SETUP_GOTO_DATA	**PciDevSetupData;
} PCI_IFR_INFO;



/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif	//_PCI_SETUP_PRIVATE_H_


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2005, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

