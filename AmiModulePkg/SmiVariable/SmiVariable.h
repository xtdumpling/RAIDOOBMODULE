//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2012, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************


/** @file SmiVariable.h
    Header file of the SmiVariable eModule.

**/

//---------------------------------------------------------------------------

#define EFI_HII_EXPORT_DATABASE_GUID \
    { 0x1b838190, 0x4625, 0x4ead, 0xab, 0xc9, 0xcd, 0x5e, 0x6a, 0xf1, 0x8f, 0xe0 }

#define FUNCTION_NUMBER                     0xef
#define GET_VARIABLE_SUBFUNCTION            0x01
#define GET_NEXT_VARIABLE_SUBFUNCTION       0x02
#define SET_VARIABLE_SUBFUNCTION            0x03
#define EXPORT_DATABASE_SUBFUNCTION         0x04
#define COPY_MEMORY_SUBFUNCTION             0x05


/**
    This table contains the list of error codes are supported by this function.

 Fields: 
 Value Name Description 

 00h NO_ERROR No error. Maps to EFI_SUCCESS
 82h INVALID_VALUE One of the parameters has an invalid value. Maps to EFI_INVALID_PARAMETER.
 85h BUFFER_TOO_SMALL The VariableNameSize is too small for the result. VariableNameSize has been updated with the size needed to complete the request. Maps to EFI_BUFFER_TOO_SMALL.
 86h NOT_SUPPORTED Interface not supported. Maps to standard INT 0x15 error codes.
 87h DEVICE_ERROR The variable could not be retrieved due to a hardware error. Maps to EFI_DEVICE_ERROR.
 8Eh NOT_FOUND The variable was not found. Maps to EFI_NOT_FOUND.

**/

#define NO_ERROR         0x00
#define INVALID_VALUE    0x82
#define BUFFER_TOO_SMALL 0x85
#define NOT_SUPPORTED    0x86
#define DEVICE_ERROR     0x87
#define NOT_FOUND        0x8e
#define ACCESS_DENIED    0x8f

#define CARRY_FLAG       0x00000001

//---------------------------------------------------------------------------

#pragma pack(1)

/**
    Description of the VARIABLE_BLOCK variables.

 Fields: 
 Name Type Description

 VendorGuid EFI_GUID A unique identifier for the vendor.
 VariableName UINT32 32-bit physical address of the null-terminated Unicode string that is the name of the vendor's variable. The caller must ensure natural alignment.
 VariableNameSize UINT32 On input, the size in bytes of the Variable Name buffer. Updated with the actual size needed if the input size was too small.
 Attributes UINT32 The attributes bitmask for the variable. This information is always returned to the caller. The corresponding EFI function defined this as an optional parameter. The attributes are defined in the EFI specification.
 DataSize UINT32 On input, the size in bytes of the return Data buffer. On output the size of data returned in Data.
 Data UINT32 32-bit physical address of the buffer for returning the value of the variable. No specific alignment requirements exist.

**/

typedef struct _VARIABLE_BLOCK {
    EFI_GUID VendorGuid;
    UINT32 VariableName;
    UINT32 Attributes;
    UINT32 DataSize;
    UINT32 Data;
} VARIABLE_BLOCK;


/**
    Description of the GET_NEXT_VARIABLE_BLOCK variables.

 Fields: 
 Name Type Description

 VendorGuid EFI_GUID A unique identifier for the vendor.
 VariableName UINT32 32-bit physical address of the null-terminated Unicode string that is the name of the vendor's variable. The caller must ensure natural alignment.
 VariableNameSize UINT32 On input, the size in bytes of the Variable Name buffer. Updated with the actual size needed if the input size was too small.

**/

typedef struct _GET_NEXT_VARIABLE_BLOCK {
    EFI_GUID VendorGuid;
    UINT32 VariableName;
    UINT32 VariableNameSize;
} GET_NEXT_VARIABLE_BLOCK;


/**
    Description of the EXPORT_HIIDB_BLOCK variables. This is a type of structure,
    which stores Size and Physical address of the HII DB. 

 Fields: 
 Name         Type        Description

 DataSize     UINT32      Size of caller's buffer.
 Data         UINT32      Physical address of the HII DB buffer.

**/

typedef struct _EXPORT_HIIDB_BLOCK {
    UINT32 DataSize;
    UINT32 Data;
} EXPORT_HIIDB_BLOCK;


/**
    Description of the COPY_MEMORY_BLOCK variables. This is a type of structure,
    which containes data for copy operation.  

 Fields: 
 Name         Type        Description

 SrcAddr      UINT32      32-bit physical address of the source.
 DestAddr     UINT32      32-bit physical address of the destination.
 Size         UINT32      32-bit data size (number of bytes to copy).

**/

typedef struct _COPY_MEMORY_BLOCK {
    UINT32 SrcAddr;
    UINT32 DestAddr;
    UINT32 Size;
} COPY_MEMORY_BLOCK;


/**
    Description of the REG_BLOCK variables. This is a type of structure, 
    which stores values of registers.

 Fields: 
 Name         Type        Description

 EAX          UINT32      Variable for register EAX
 EBX          UINT32      Variable for register EBX
 ECX          UINT32      Variable for register ECX
 EDX          UINT32      Variable for register EDX
 ESX          UINT32      Variable for register ESI
 EDI          UINT32      Variable for register EDI
 EFLAGS       UINT32      Variable for register EFLAGS
 ES           UINT32      Variable for register ES 
 CS           UINT32      Variable for register CS 
 SS           UINT32      Variable for register SS 
 DS           UINT32      Variable for register DS 
 FS           UINT32      Variable for register FS 
 GS           UINT32      Variable for register GS 
 EBP          UINT32      Variable for register EBP
 ESP          UINT32      Variable for register ESP

**/

typedef struct _REG_BLOCK {
    UINT32 EAX;
    UINT32 EBX;
    UINT32 ECX;
    UINT32 EDX;
    UINT32 ESI;
    UINT32 EDI;
    UINT32 EFLAGS;
    UINT16 ES;
    UINT16 CS;
    UINT16 SS;
    UINT16 DS;
    UINT16 FS;
    UINT16 GS;
    UINT32 EBP;
    UINT32 ESP;
} REG_BLOCK;

#pragma pack()

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2012, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
