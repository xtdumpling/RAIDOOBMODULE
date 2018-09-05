//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

/** @file 
    Header file of the NvramSmi eModule.

**/

typedef struct
{
	UINT64 Control; 				//0x0000000000000000
	EFI_GUID Guid;					//Guid of the variable 
	UINT32 Attributes; 				//Attributes bitmask or 0xFFFFFFFFFFFFFFFF if this field should be ignored
	UINT32 Reserved;
	UINT64 VariableNameSize;   		//Size in bytes of the variable name including NULL-terminator 
	UINT64 DataSize;				//Size in bytes of the allocated buffer or 0 
//	CHAR16 VariableName[VariableNameSize]; 		//Null-terminated string of the variable name
//  UINT8 DataBuffer[DataSize]; 	// Variable data buffer
} SMI_GET_VARIABLE_BUFFER;

typedef struct
{
	UINT64 Control; 			//0x0000000000000001
	UINT64 DataSize;			//Size in bytes of VariableName buffer
	EFI_GUID Guid;				//Guid of the variable or array of zeroes for the first call
//	CHAR16 VariableName[DataSize]; 	//Null-terminated string of the variable name or zero array on first call
} SMI_GET_NEXT_VARIABLE_NAME_BUFFER;

typedef struct
{
	UINT64 Control; 				//0x0000000000000002
	EFI_GUID Guid;					//Guid of the variable 
	UINT32 Attributes;				//Attributes of the variable
	UINT32 Reserved;
	UINT64 VariableNameSize;   		//Size in bytes of the variable name including NULL-terminator 
	UINT64 DataSize;				//Size in bytes of the variable
	//CHAR16 VariableName[VariableNameSize]; 		//Null-terminated string of the variable name
	//UINT8 VariableData[DataSize] Variable data buffer
} SMI_SET_VARIABLE_BUFFER;

typedef struct
{
	UINT64 Control;							//0x0000000000000003
	UINT32 Attributes;						//Attributes bitmask
	UINT32 Reserved;
	UINT64 MaximumVariableStorageSize;		// Storage size in bytes or undefined if Status is EFI_ERROR
	UINT64 RemainingVariableStorageSize;	// Remaining size in bytes or undefined if Status is EFI_ERROR 
	UINT64 MaximumVariableSize;				//Maximum variable size in bytes or undefined if Status is EFI_ERROR
} SMI_QUERY_VARIABLE_INFO_BUFFER;

typedef struct
{
	UINT64 Control; 			//0x0000000000000004
	UINT64 DataSize;			//Size in bytes of VariableName buffer
	EFI_GUID Guid;				//Guid of the variable or array of zeroes for the first call
//	CHAR16 VariableName[DataSize]; 	//Null-terminated string of the variable name
} SMI_REQUEST_TO_LOCK_BUFFER;

typedef struct
{
	UINT64 Control; 			//0x0000000000000005
} SMI_EXIT_BOOT_SERVICES_BUFFER;

#define NVRAM_SMM_ERROR_BIT 0x8000000000000000ULL
// Control field of the NVRAM SMM Communication buffer is initialized with command number prior to calling SmmCommunicate.
// SMM Communicate SMI handler updated the field with the return status.
// In order to detect a situation when no SMI handler is available and Control is not updated,
// we are defining command numbers with highest bit set to treat them as error status codes (no-response error).
// We are using OEM status code range to distinguish no-response error from standard UEFI/PI error codes.
#define NVRAM_SMM_COMMAND_BIT 0xC000000000000000ULL
#define NVRAM_SMM_COMMAND(Value) (NVRAM_SMM_COMMAND_BIT|(Value))

#define NVRAM_SMM_COMMAND_GET_VARIABLE NVRAM_SMM_COMMAND(0)
#define NVRAM_SMM_COMMAND_GET_NEXT_VARIABLE_NAME NVRAM_SMM_COMMAND(1)
#define NVRAM_SMM_COMMAND_SET_VARIABLE NVRAM_SMM_COMMAND(2)
#define NVRAM_SMM_COMMAND_QUERY_VARIABLE_INFO NVRAM_SMM_COMMAND(3)
#define NVRAM_SMM_COMMAND_REQUEST_TO_LOCK NVRAM_SMM_COMMAND(4)
#define NVRAM_SMM_COMMAND_EXIT_BOOT_SERVICES NVRAM_SMM_COMMAND(5)

#define NVRAM_SMM_STATUS_TO_EFI_STATUS(NvramSmmStatus) ( ((NvramSmmStatus)&NVRAM_SMM_ERROR_BIT) ?  ENCODE_ERROR((EFI_STATUS)((NvramSmmStatus)&~NVRAM_SMM_ERROR_BIT)) : (EFI_STATUS)(NvramSmmStatus))
#define EFI_STATUS_TO_NVRAM_SMM_STATUS(EfiStatus) ( EFI_ERROR(EfiStatus) ? ((EfiStatus)&~MAX_BIT|NVRAM_SMM_ERROR_BIT) : EfiStatus)

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
