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
/** @file
  Header file that defines Board Initialization Protocol.
**/
#ifndef __AMI_INIT_POLICY_H__
#define __AMI_INIT_POLICY_H__

#define AMI_BOARD_PCI_INIT_PROTOCOL_GUID \
		{ 0xEC63428D, 0x66CA, 0x4bf9, 0x82, 0xAE, 0x84, 0x0F, 0x6D, 0x5C, 0x23, 0x05 }
extern EFI_GUID gAmiBoardPciInitProtocolGuid;


#define AMI_BOARD_SIO_INIT_PROTOCOL_GUID \
		{ 0x9d36f7ef, 0x6078, 0x4419, 0x8c, 0x46, 0x2b, 0xbd, 0xb0, 0xe0, 0xc7, 0xb3 }
extern EFI_GUID gAmiBoardSioInitProtocolGuid;

typedef enum {
	isPciNone			= 0,	//0	
	isPciGetSetupConfig,		//1
	isPciSkipDevice,			//2
	isPciSetAttributes,			//3
	isPciProgramDevice,			//4
	isPcieInitLink,				//5
	isPcieSetAspm,				//6
	isPcieSetLinkSpeed,			//7
	isPciGetOptionRom,			//8
	isPciOutOfResourcesCheck,	//9
	isPciReadyToBoot,			//10	
	isPciQueryDevice,			//11
	isHbBasicInit,				//12
	isRbCheckPresence,			//13
	isRbBusUpdate,				//14
	isHpUpdatePadding,			//15
	isPciGetRbAslName,			//16
	isPciMaxStep				//17...so far
} PCI_INIT_STEP;


typedef enum {
	itNone=0,	
	itHost,		//1
	itRoot,		//2
	itDevice,	//3
	itMaxType	//4
}PCI_INIT_TYPE;


typedef enum {
	isSioNone=0,
	isPrsSelect, 			//1
	isBeforeActivate,		//2
	isAfterActivate,		//3
	isGetSetupData,			//4
    isAfterBootScript, 		//5. this initialization step will be invoked after all SIO registers are added to the boot script
                       		//   for each device which has initialization function implemented.  
    isGetModeData,	   		//6. Provides device specific modes number and setup strings 	
    isSetResources,     	//7. If SIO is not of Standard Type, it must implement a Routine to Program resources.
    isSioDetect,        	//8. For platforms where Super I/O device has to be dynamically detected, chose this routine to detect 
                        	//   which SIO is installed on the platform. 
    isAfterSmmBootScript,	//9. this initialization step replaces isAfterBootScript in GenericSio_06 and up
    						//   it uses different parameters type casting. In GenericSio_06 and HIGHER isAfterBootScript 
    						//   will never be invoked, this initialization step will be passed instead.

	isMaxSioInitStep
} SIO_INIT_STEP;

typedef enum {
    srNone,
    srIrqReg1,
    srIrqReg2,
    srBaseAddrReg1,
    srBaseAddrReg2,
    srDmaReg1,  
    srDmaReg2,
    srMaxResReg
} SIO_RESOURCE_REG_TYPE;


typedef struct _AMI_BOARD_INIT_PROTOCOL AMI_BOARD_INIT_PROTOCOL;

//Initialization Routine Parameters Block...
typedef struct _AMI_BOARD_INIT_PARAMETER_BLOCK {
	IN UINT64				Signature;	//"$PCIINIT" or "$SIOINIT" to validate parameter structure;
	IN UINT32				InitStep; 	//uint32 same as SIO_INIT_STEP enum.
	IN OUT VOID				*Param1;	//Up to 4 parameters.
	IN OUT VOID				*Param2;	//Based on InitStep some or none of this will be used.
	IN OUT VOID				*Param3;
	IN OUT VOID				*Param4;
} AMI_BOARD_INIT_PARAMETER_BLOCK;


typedef EFI_STATUS (EFIAPI *AMI_BOARD_INIT_FUNCTION) (
	IN AMI_BOARD_INIT_PROTOCOL				*This,
	IN UINTN								*Function,
	IN OUT AMI_BOARD_INIT_PARAMETER_BLOCK	*ParameterBlock
);


#define AMI_SIO_PARAM_SIG 0x54494E494F495324 //"$SIOINIT"

#define AMI_PCI_PARAM_SIG 0x54494E4949435024 //"$PCIINIT"



//AMI Board Int Protocol Definition.
typedef struct _AMI_BOARD_INIT_PROTOCOL {
	UINTN					Uid;		//Ordering Number of SIO Device in AMI_SDL_SIO_DATA structure
	UINTN					FunctionCount;
	AMI_BOARD_INIT_FUNCTION	Functions[1];
} AMI_BOARD_INIT_PROTOCOL;

//Define Macro to determine definition of AMI_BOARD_INIT_FUNCTION
#define AMI_BOARD_SIO_INIT_FUNCTION_ITEM(name) EFI_STATUS EFIAPI name(AMI_BOARD_INIT_PROTOCOL *This, UINTN Func,AMI_BOARD_INIT_PARAMETER_BLOCK *Param)

#define AMI_BOARD_PCI_INIT_FUNCTION_ITEM(name) EFI_STATUS EFIAPI name(AMI_BOARD_INIT_PROTOCOL *This, UINTN Func,AMI_BOARD_INIT_PARAMETER_BLOCK *Param)


#endif //__AMI_INIT_POLICY_H__
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
