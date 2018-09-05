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

//**********************************************************************
// $Header:  $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************


//**********************************************************************
//<AMI_FHDR_START>
//
// Name:	AmiGenericSio.h
//
// Description:	AMI Aptio V Sio Implementation Private Data Structure Definitions.
//
//<AMI_FHDR_END>
//**********************************************************************
#ifndef _AMI_GENERIC_SIO_H_
#define _AMI_GENERIC_SIO_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
#include <AmiDxeLib.h>
#include <SioSetup.h>
#include <Token.h>
#include <Protocol/DevicePath.h>
#include <Protocol/S3SaveState.h>
#include <Protocol/AmiSio.h>
#include <Protocol/PciIo.h>
#include <Protocol/SuperIo.h>
#include <Protocol/AmiBoardInfo2.h>
#include <Protocol/AmiBoardInitPolicy.h>
#include <Protocol/S3SmmSaveState.h>

//---------------------------------------------------------
//To exclude SIO Messages
#ifndef TRACE_SIO
#define TRACE_SIO TRACE_ALWAYS
#endif

#if SIO_DEBUG_MESSAGES
#define SIO_TRACE(Arguments) TRACE(Arguments)
#else
#define SIO_TRACE(Arguments)
#endif
//---------------------------------------------------------


#define PNP_HID 			EFI_PNP_ID
#define SIO_GFG_OP_TYPE2	UINT8
#define cfNone 			0
#define cfByteSeq 			1
#define	cfRoutine			2
#define cfMaxCfg			3

#define useIdx				1
#define useDat				0

#define doWrite				1
#define doRead				0

//---------------------------------------------------------
///Forward declaration
//---------------------------------------------------------
typedef struct _GSIO2 		GSIO2;
typedef struct _SMM_GSIO    SMM_GSIO;


typedef struct _SPIO_SCRIPT {
	BOOLEAN		IdxDat;	//1=IDX 0=DAT
	BOOLEAN		WrRd; 	//1=Write 0=Read
	UINT8		Value; 	//if WrRd=0 wait for this data to come	
} SPIO_SCRIPT;

typedef struct _SIO_SCRIPT_LST2 {
	SIO_GFG_OP_TYPE2	OpType;
	UINT8				InstrCount;
	SPIO_SCRIPT			Instruction[1]; 	//using woid pointer to specify
} SIO_SCRIPT_LST2;

//==================================================================
///Definitions of SIO Shared Resource Flags if this flags set 
///and will share following type of resources with PREVIOUSE device
#define SIO_SHR_NONE	0x00
#define SIO_SHR_IO1		0x01 ///>device shares resources programmed in SIO_1_BASE_REG
#define SIO_SHR_IO2		0x02 ///>device shares resources programmed in SIO_2_BASE_REG	
#define SIO_SHR_IO		(SIO_SHR_IO1 | SIO_SHR_IO2) ///>device shares resources programmed in both SIO_BASE_REG
#define SIO_SHR_IRQ1	0x04	
#define SIO_SHR_IRQ2	0x08
#define SIO_SHR_IRQ		(SIO_SHR_IRQ1 | SIO_SHR_IRQ2)
#define SIO_SHR_DMA1	0x10
#define SIO_SHR_DMA2	0x20
#define SIO_SHR_DMA		(SIO_SHR_DMA1 | SIO_SHR_DMA2)
#define SIO_SHR_ALL		(SIO_SHR_IO | SIO_SHR_IRQ | SIO_SHR_DMA)
#define SIO_NO_RES		0x80 //this bit will be set if device not using any resources but must be enabled
//all other values reserved

#if (SIO_SETUP_USE_APTIO_4_STYLE==1)
#define LD_NV_VAR_NAME_FORMAT 	L"PNP%04X_%X_NV"
#define LD_VV_VAR_NAME_FORMAT 	L"PNP%04X_%X_VV"
#else
#define LD_NV_VAR_NAME_FORMAT 	L"NV_SIO%X_LD%X"
#define LD_VV_VAR_NAME_FORMAT 	L"VV_SIO%X_LD%X"
#endif

// {39D1E68F-E0DB-45d0-B444-59B92246AE4F}
#define SMI_READY_TO_BOOT_COMMUNICATE_GUID \
{0x39d1e68f, 0xe0db, 0x45d0, 0xb4, 0x44, 0x59, 0xb9, 0x22, 0x46, 0xae, 0x4f}

//UEFI Sio Protocol Private Data
typedef struct _EFI_SIO_DATA{
    EFI_SIO_PROTOCOL    EfiSio;
    struct _SIO_DEV2    *Owner;        
}EFI_SIO_DATA;

//SPIO Device Private Data section
typedef struct _SIO_DEV2 {
	AMI_SIO_PROTOCOL			AmiSio;
	EFI_DEVICE_PATH_PROTOCOL 	*DevicePath;
	EFI_HANDLE					Handle;
	BOOLEAN						Started;
	BOOLEAN						Assigned;
	BOOLEAN						Initialized;
	BOOLEAN						IrqReserved;
	struct _SIO_DEV2			*ResOwner;
	struct _SIO_DEV2			*ResConsumer;
	EFI_EISAID					EisaId;
	T_ITEM_LIST					CRS;
	T_ITEM_LIST					PRS;
//we will need it to generate boot script for s3 resume
	UINT8						*LocalCfgDump;
	GSIO2						*Owner;
	AMI_SDL_LOGICAL_DEV_INFO	*DeviceInfo;
	SIO_DEV_NV_DATA				NvData;
	SIO_DEV_V_DATA				VlData;
	EFI_HANDLE					ImageHandle;
    EFI_SIO_DATA        		EfiSioData;
	UINTN						DevInfoIdx;		
	UINTN       				DevModeCnt;
	CHAR16						**DevModeStr;
	BOOLEAN						CompatibleMode;
} SIO_DEV2;

//Global Super IO device Private Structure
//Initial entity for SIO data.
typedef struct _GSIO2 {
	EFI_HANDLE						CntrHandle;
	EFI_HANDLE						SupportedHandle; //
	EFI_DEVICE_PATH_PROTOCOL 		*CntrDevPath;
	EFI_PCI_IO_PROTOCOL 			*IsaBrgPciIo;
	BOOLEAN							InCfgMode;
	UINTN							LdCount;
	SIO_DEV2						**DeviceList;
	UINT8							*GlobalCfgDump;
	AMI_SDL_SIO_CHIP_INFO			*SpioSdlInfo;
	EFI_S3_SAVE_STATE_PROTOCOL		*SaveState;
	AMI_SDL_LOGICAL_DEV_INFO		*LdSdlInfo;
	UINTN							SioInfoIdx;
	//List of valid registers inside SIO to check if they has to be saved
	//----------------------------------------
	//in S3_SAVE_SSTATE for S3 Resume
	//This is for global registers which are the same for all devices in SIO
	UINTN							GlobalInclRegCount;
	UINT8							*GlobalIncludeReg; //Buffer will be located in GSIO2 structure
	//This is for Local registers they are unique for each device in SIO
	UINTN							LocalInclRegCount;
	UINT8							*LocalIncludeReg; //Buffer will be located in SIO_DEV structure
	//----------------------------------------
	//How To enter/exit Configuration mode if any
	SIO_SCRIPT_LST2					*EnterCfgMode;
	SIO_SCRIPT_LST2					*ExitCfgMode;
    AMI_BOARD_INIT_PROTOCOL 		*SioInitProtocol;
//    AMI_SIO_PORT_COMPATIBILITY_PROTOCOL *SioCompProtocol;
    BOOLEAN							CompatibleData;
	UINT8							Reserved[7];
} GSIO2;

#define SIO_DEV_STATUS_VAR_NAME  L"SIO_DEV_STATUS_VAR"

#define SIO_DEV_STATUS_VAR_GUID \
	{0x5820de98, 0xfc8e, 0x4b0b, 0xa4, 0xb9, 0xa, 0x94, 0xd, 0x16, 0x2a, 0x7e}

//
// Bit values of various SIO devices defined in SIO_DEV_STATUS
//
#define SIO_DEV_COM1		0x0001
#define SIO_DEV_COM2		0x0002
#define SIO_DEV_LPT			0x0004
#define SIO_DEV_FDC			0x0008
#define SIO_DEV_SB16		0x0010
#define SIO_DEV_MIDI		0x0020
#define SIO_DEV_MSSAUDIO	0x0040
#define SIO_DEV_FMSYNTH		0x0080
#define SIO_DEV_GAME1		0x0100
#define SIO_DEV_GAME2		0x0200
#define SIO_DEV_KBC			0x0400
#define SIO_DEV_EC			0x0800
#define SIO_DEV_PS2MS		0x4000

typedef union {
	UINT16	DEV_STATUS;
	struct  {
		UINT16 	SerialA		: 1;	//bit 0 - Serial A
		UINT16 	SerialB 	: 1;	//bit 1	- Serial B
		UINT16  Lpt			: 1;	//bit 2 - LPT
		UINT16	Fdd			: 1;	//bit 3 - FDD
		UINT16	Sb16		: 1;	//bit 4 - SB16 Audio
		UINT16	Midi		: 1;	//bit 5 - MIDI
		UINT16	MssAudio	: 1;	//bit 6 - MSS Audio
		UINT16	FmSynth		: 1;	//bit 7 - AdLib sound (FM synth)
		UINT16	Game1		: 1;	//bit 8 - Game port #1
		UINT16	Game2		: 1;	//bit 9 - Game port #2
		UINT16	Key60_64	: 1;	//bit a - KBC
		UINT16	Ec62_66		: 1;	//bit b - EC
		UINT16	Res1		: 1;	//bit c - Reserved
		UINT16	Res2		: 1;	//bit d - Reserved
		UINT16	Ps2Mouse	: 1;	//bit e - PS/2 Mouse
		UINT16	Res3		: 1;	//bit f - Reserved
	};
} SIO_DEV_STATUS;


//SIO Device in SMM Private Data Structure
typedef struct _SMM_SIO_DEV {
    BOOLEAN                         Started;
    BOOLEAN                         Assigned;
    BOOLEAN                         Initialized;
    BOOLEAN                         CompatibleMode;
    SIO_DEV_NV_DATA                 NvData;
    SIO_DEV_V_DATA                  VlData;
    SMM_GSIO                        *Owner;
    AMI_SDL_LOGICAL_DEV_INFO        SdlDeviceInfo;
    UINT8                           *LocalCfgDump; //Owner->LocalInclRegCount bytes
}SMM_SIO_DEV;
    
//SIO Chip in SMM Private data
typedef struct _SMM_GSIO {
    BOOLEAN                         InCfgMode;
    BOOLEAN                         CompatibleData;
    UINTN                           LdCount;    
    SMM_SIO_DEV                     *DeviceList;
    UINT8                           *GlobalCfgDump;
    AMI_SDL_SIO_CHIP_INFO           SpioSdlInfo;
    //in S3_SAVE_SSTATE for S3 Resume
    //This is for global registers which are the same for all devices in SIO
    UINTN                           GlobalInclRegCount;
    UINT8                           *GlobalIncludeReg; //Buffer will be located in GSIO2 structure
    //This is for Local registers they are unique for each device in SIO
    UINTN                           LocalInclRegCount;
    UINT8                           *LocalIncludeReg; //Buffer will be located in SIO_DEV structure
    //----------------------------------------
    //How To enter/exit Configuration mode if any
    SIO_SCRIPT_LST2                 *EnterCfgMode;
    SIO_SCRIPT_LST2                 *ExitCfgMode;
    EFI_S3_SMM_SAVE_STATE_PROTOCOL  *SaveState;
}SMM_GSIO;



/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
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
