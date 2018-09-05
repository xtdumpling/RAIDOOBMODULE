//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2011, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Inc/variable.h $
//
// $Author: Arunsb $
//
// $Revision: 9 $
//
// $Date: 1/30/12 1:33a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file variable.h
    Variable handling header

**/

#ifndef	_VARIABLE_H_
#define	_VARIABLE_H_

typedef struct _NVRAM_VARIABLE
{
	UINT8	*Buffer;
	UINTN	Size;
}
NVRAM_VARIABLE;

#define VAR_ZERO_OFFSET         0

#define	VAR_COMMAND_GET_VALUE	0
#define	VAR_COMMAND_SET_VALUE	1

#define	VAR_COMMAND_GET_NVRAM	0
#define	VAR_COMMAND_SET_NVRAM	1

// Variable IDs
#define VARIABLE_ID_SETUP			    		0
#define VARIABLE_ID_LANGUAGE		    		1		
#define VARIABLE_ID_BOOT_TIMEOUT	   		2
#define VARIABLE_ID_USER_DEFAULTS	   	3
#define VARIABLE_ID_ERROR_MANAGER	   	4
#define VARIABLE_ID_AMITSESETUP       		5
#define VARIABLE_ID_IDE_SECURITY      		6
#define VARIABLE_ID_BOOT_ORDER         	7
#define VARIABLE_ID_BBS_ORDER           	8
#define VARIABLE_ID_DEL_BOOT_OPTION     	9
#define VARIABLE_ID_ADD_BOOT_OPTION     	10
#define VARIABLE_ID_BOOT_MANAGER        	11
#define VARIABLE_ID_BOOT_NOW            	12
#define VARIABLE_ID_LEGACY_DEV_INFO     	13
#define VARIABLE_ID_AMI_CALLBACK        	14
#define VARIABLE_ID_LEGACY_GROUP_INFO   	15
#define VARIABLE_ID_OEM_TSE_VAR		    	17
#define VARIABLE_ID_DYNAMIC_PAGE_COUNT		18
#define VARIABLE_ID_DRV_HLTH_ENB				19
#define VARIABLE_ID_DRV_HLTH_COUNT			20
#define VARIABLE_ID_DRV_HLTH_CTRL_COUNT		21
#define VARIABLE_ID_DRIVER_MANAGER			22
#define VARIABLE_ID_DRIVER_ORDER				23	
#define VARIABLE_ID_ADD_DRIVER_OPTION   	24
#define VARIABLE_ID_DEL_DRIVER_OPTION   	25
#define VARIABLE_ID_PORT_OEM1					26
#define VARIABLE_ID_PORT_OEM2					27
#define VARIABLE_ID_PORT_OEM3					28
#define VARIABLE_ID_PORT_OEM4					29
#define VARIABLE_ID_PORT_OEM5					30
#define VARIABLE_ID_PORT_OEM6					31
#define VARIABLE_ID_PORT_OEM7					32
#define VARIABLE_ID_PORT_OEM8					33
#define VARIABLE_ID_PORT_OEM9					34
#define VARIABLE_ID_PORT_OEM10				35
#define VARIABLE_ID_PORT_OEM11				36
#define VARIABLE_ID_PORT_OEM12				37
#define VARIABLE_ID_PORT_OEM13				38
#define VARIABLE_ID_PORT_OEM14				39
#define VARIABLE_ID_PORT_OEM15				40
#define VARIABLE_ID_PORT_OEM16				41
#define VARIABLE_ID_PORT_OEM17				42
#define VARIABLE_ID_PORT_OEM18				43
#define VARIABLE_ID_PORT_OEM19				44
#define VARIABLE_ID_PORT_OEM20				45
#define VARIABLE_ID_PORT_OEM21				46
#define VARIABLE_ID_PORT_OEM22				47
#define VARIABLE_ID_PORT_OEM23				48
#define VARIABLE_ID_PORT_OEM24				49
#define VARIABLE_ID_PORT_OEM25				50
#define VARIABLE_ID_PORT_OEM26				51
#define VARIABLE_ID_PORT_OEM27				52
#define VARIABLE_ID_PORT_OEM28				53
#define VARIABLE_ID_PORT_OEM29				54                            
#define VARIABLE_ID_DYNAMIC_PAGE_GROUP      55
#define VARIABLE_ID_DYNAMIC_PAGE_DEVICE     56
#define VARIABLE_ID_DYNAMIC_PAGE_GROUP_CLASS	57

EFI_STATUS GetNvramVariableList(NVRAM_VARIABLE **RetNvramVarList);
VOID CleanTempNvramVariableList();
EFI_STATUS CopyNvramVariableList(NVRAM_VARIABLE *SrcVarList, NVRAM_VARIABLE **DestVarList);
VOID CleanNvramVariableList(NVRAM_VARIABLE **InvVarlist); //For memleak
 
EFI_STATUS VarLoadVariables( VOID **list, NVRAM_VARIABLE *defaultList );
EFI_STATUS VarBuildDefaults( VOID );
VOID *VarGetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size );
EFI_STATUS VarSetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 attributes, VOID *buffer, UINTN size );
VOID *VarGetNvram( UINT32 variable, UINTN *size );
VOID *VarGetNvramQuestionValue(UINT32 variable, UINTN Offset, UINTN Size);
EFI_STATUS VarSetNvram( UINT32 variable, VOID *buffer, UINTN size );
EFI_STATUS VarGetDefaults( UINT32 variable, UINT32 offset, UINTN size, VOID *buffer );
EFI_STATUS VarGetValue( UINT32 variable, UINT32 offset, UINTN size, VOID *buffer );
EFI_STATUS VarSetValue( UINT32 variable, UINT32 offset, UINTN size, VOID *buffer );
VOID VarUpdateVariable(UINT32 variable);
VOID VarUpdateDefaults(UINT32 variable);
VOID *VarGetVariable( UINT32 variable, UINTN *size );

EFI_STATUS _VarGetData( UINT32 variable, UINT32 offset, UINTN size, VOID *buffer, BOOLEAN useDefaults );
EFI_STATUS _VarGetSetValue( UINTN command, NVRAM_VARIABLE *list, UINT32 variable, UINT32 offset, UINTN size, VOID *buffer );

#endif /* _VARIABLE_H_ */

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**         (C)Copyright 2011, American Megatrends, Inc.             **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093       **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
