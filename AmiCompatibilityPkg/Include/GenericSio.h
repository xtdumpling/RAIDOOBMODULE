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
/** @file
  AMI Aptio 4 compatible Data Structure Definitions.
*/

#ifndef _GENERIC_SIO_H_
#define _GENERIC_SIO_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
#include <Protocol/DevicePath.h>
#include <Protocol/S3SaveState.h>
#include <Protocol/AmiSio.h>
#include <Protocol/PciIo.h>
#include <AmiDxeLib.h>
#include <Protocol/SuperIo.h>
#include <Library/AmiSioDxeLib.h>
#include <Token.h>

//--------------------------------------------
//Defines how to enter Exit config Mode
typedef enum {
	cfgNone=0,		//Not required any sequence
	cfgByteSeq,
	cfgRoutine,
	cfgMaxCfg
} SPIO_GFG_OP_TYPE;

typedef VOID (*SPIO_ROUTINE)();


typedef struct SPIO_SCRIPT_LST {
	SPIO_GFG_OP_TYPE	OpType;
	UINTN				InstrCount;		
	VOID				*Instruction[]; 	//using woid pointer to specify 
										//script or routine
} SPIO_SCRIPT_LST;

//==================================================================
//SIO Component Initialization Routine ptrototype
typedef EFI_STATUS (*SIO_INIT)( IN AMI_SIO_PROTOCOL 	*This, 
								IN EFI_PCI_IO_PROTOCOL 	*PciIo, 
								IN SIO_INIT_STEP 		InitStep);

//Device table used for porting see YourSioNameDxe.c 
typedef struct _SPIO_DEV_LST {
	SIO_DEV_TYPE	Type;
	UINT8			LDN;		//LogicalDevice #
	UINT8			UID; 		//Unique ID if there are 2 or more 
								//devices of the same type in this SPIO 
	UINT16			PnpId;
	BOOLEAN     	Implemented;//Implemented on this Platform (has connector)
	BOOLEAN			HasSetup;	//TRUE If Device has SETUP screen and se
	UINT8			Flags;		//set if Device shares all or some of it's resources 
	UINT8			AslName[5]; //ASL DEVICE OBJECT NAME to look for _PRS in AML
								//if this field not 0,0,0,0 the rest of the table ignored.
	UINT16			ResBase[2];	//Base Address for SIO_1_BASE_REG and SIO_2_BASE_REG
	UINT8			ResLen[2];
	UINT16			IrqMask[2];
	UINT8			DmaMask[2];
	SIO_INIT		InitRoutine;
//	UINTN			SetupOffset; //offset in SIO_SETUP_VAR for this device if any
//	UINTN			VolatileOffset; //offset in SIO_SETUP_VOLATILE data for this device if any
} SPIO_DEV_LST;

//This structure will define a multiple SIO Porting Information
//It will be an array of such structures each of them will represent
//a different SIO Device.
typedef struct _SPIO_LIST_ITEM {
	//This Information is needed to identify right LPC bridge for the SIO
	// value of  -1 (0xF..F) means Do not check this parameter
	UINT32			IsaVenDevId; 
	UINT32			IsaSubVenId;
	UINT8			IsaBusNo;
	UINT8			IsaDevNo;
	UINT8			IsaFuncNo;
//	//This is the information Needed Get Sio Setup Data
//	//----------------------------------------
//	UINT16			*SetupVarName;
//	UINT16			*VolatileVarName;
//	EFI_GUID		*SioVarGuid;
//	//This is the information Needed to access SIO Generic Registers
//	//----------------------------------------
	UINT16			SioIndex;
	UINT16			SioData;
	// Dev Select and Activate
	UINT8			DevSel;
	UINT8			Activate;
	UINT8			ActivVal;
	UINT8			DeactVal;
	//Generic registers location
	UINT8			Base1Hi;
	UINT8			Base1Lo;
	UINT8			Base2Hi;
	UINT8			Base2Lo;
	UINT8			Irq1;
	UINT8			Irq2;
	UINT8			Dma1;
	UINT8			Dma2;
	//List of devices inside this SIO
	UINTN			DevCount;
	SPIO_DEV_LST	*SioDevList;
	//List of valid registers inside SIO to check if they has to be saved
	//----------------------------------------
	//in BOOT_SCRIPT_SAVE for S3 state Resume 
	//This is for global registers which are the same for all devices in SIO
	UINTN			GlobalInclRegCount;
	UINT8			*GlobalIncludeReg; //Buffer will be located in GSPIO structure
	//This is for Local registers they are unique for each device in SIO
	UINTN			LocalInclRegCount;
	UINT8			*LocalIncludeReg; //Buffer will be located in SIO_DEV structure
	//----------------------------------------
	//How To enter/exit Configuration mode if any 
	SPIO_SCRIPT_LST	*EnterCfgMode;
	SPIO_SCRIPT_LST	*ExitCfgMode;
}SPIO_LIST_ITEM;

//Forward Declaration...
typedef struct _GSPIO GSPIO;

//SPIO Device Private Data section
typedef struct _SPIO_DEV {
	AMI_SIO_PROTOCOL	AmiSio;
	EFI_DEVICE_PATH_PROTOCOL *DevicePath;
	EFI_HANDLE			Handle;
	BOOLEAN				Started;
	BOOLEAN				Assigned;
	BOOLEAN				Initialized;
	BOOLEAN				IrqReserved;
	struct _SPIO_DEV	*ResOwner;
	struct _SPIO_DEV	*ResConsumer;
	EFI_EISAID			EisaId;
	T_ITEM_LIST			CRS;
	T_ITEM_LIST			PRS;
//we will need it to generate boot script for s3 resume
	UINT8				*LocalCfgDump;
	GSPIO				*Owner;	
	SPIO_DEV_LST		*DeviceInfo;
	SIO_DEV_NV_DATA		NvData;
	SIO_DEV_V_DATA		VlData;	
	EFI_HANDLE			ImageHandle;
    EFI_SIO_DATA        EfiSioData;
} SPIO_DEV;

//Global Super IO device Private Structure
//Initial entity for SIO data.
struct _GSPIO {
	EFI_HANDLE		CntrHandle;
	EFI_HANDLE		SupportedHandle; //
	EFI_DEVICE_PATH_PROTOCOL 	*CntrDevPath;
	EFI_PCI_IO_PROTOCOL *IsaBrgPciIo;
	BOOLEAN			InCfgMode;
	UINTN			DeviceCount;
	SPIO_DEV		**DeviceList;
	UINT8			*GlobalCfgDump;
	SPIO_LIST_ITEM	*SpioInfo;
    EFI_BOOT_SCRIPT_SAVE_PROTOCOL *BootScript;
}; // GSPIO;

//_PRS Functions definition for ACPI 
EFI_STATUS GetPrsFromAml(SPIO_DEV* Dev, UINT8 *PrsName ,UINTN Index);
//and NO ACPI mode
EFI_STATUS SetUartPrs(SPIO_DEV *Dev);
EFI_STATUS SetLptPrs(SPIO_DEV *Dev, BOOLEAN UseDma);
EFI_STATUS SetFdcPrs(SPIO_DEV *Dev);
EFI_STATUS SetPs2kPrs(SPIO_DEV *Dev);
EFI_STATUS SetPs2mPrs(SPIO_DEV *Dev);
EFI_STATUS SetGamePrs(SPIO_DEV *Dev);
EFI_STATUS SetMpu401Prs(SPIO_DEV *Dev);


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
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
