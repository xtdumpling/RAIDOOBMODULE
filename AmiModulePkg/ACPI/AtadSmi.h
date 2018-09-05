//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2013, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

/** @file AtadSmi.h
    Header file of the SmiVariable eModule.

**/

//---------------------------------------------------------------------------
#define ATAD_USE_COMMUNICATE 0

#define ATAD_SW_SMI_UPDATE       2
#define ATAD_SW_SMI_GET     1
#define ATAD_SW_SMI_OK      3
#define ATAD_SW_SMI_NOT_GOOD      0xA5


#define ATAD_SMI_GUID \
{0x6F431B9F, 0x57E2, 0x1049, 0x5A, 0x3B, 0x69, 0x50, 0x4A, 0xED, 0x4F, 0x57}
#define EFI_TIME_VARIABLE_GUID \
{0x9d0da369, 0x540b, 0x46f8, 0x85, 0xa0, 0x2b, 0x5f, 0x2c, 0x30, 0x1e, 0x15}

#pragma pack(push, 1)

/**
	Structure to store Timezone and Daylight info

**/
typedef struct {
    INT16 TimeZone;
    UINT8 Daylight;
    } TIME_VARIABLE;

/**
	Structure to pass Timezone and Daylight info, as well as operation to perform,
	to ACPI Time And Alarm SMI handler

**/   
typedef struct
{
    UINT8      Signature;///< Operation to perform
    UINT8      DayLight;///< DayLight info
    UINT16     TimeZone;///< TimeZone info

} ATAD_VARIABLE;

#pragma pack(pop)
//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2013, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
