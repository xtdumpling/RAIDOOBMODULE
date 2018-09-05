//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  <xxx.H>
//
// Description: GUID or structure Of Setup related Routines.
//
//<AMI_FHDR_END>
//*************************************************************************

#ifndef _SIO_SETUP_H_
#define _SIO_SETUP_H_
#ifdef __cplusplus
extern "C" {
#endif


#define SIO_VAR_GUID \
    {0x560bf58a, 0x1e0d, 0x4d7e, 0x95, 0x3f, 0x29, 0x80, 0xa2, 0x61, 0xe0, 0x31}

#ifdef VFRCOMPILE
//**********************************************************************//
//                        Below is for SD files                         //
//**********************************************************************//
#define AMI_SIO_VARSTORE(ldxn, PNPxxxx_n) \
varstore ldxn##_V_DATA,\
    key    = ldxn##_V_DATA_KEY,\
    name    = PNPxxxx_n##_VV,\
    guid    = SIO_VAR_GUID;\
varstore ldxn##_NV_DATA,\
    key    = ldxn##_NV_DATA_KEY,\
    name    = PNPxxxx_n##_NV,\
    guid    = SIO_VAR_GUID;

#define LDX_XV_DATA(ldxn) \
typedef struct {\
    UINT8        DevImplemented;\
    UINT16       DevBase1;\
    UINT16       DevBase2;\
    UINT8        DevIrq1;\
    UINT8        DevIrq2;\
    UINT8        DevDma1;\
    UINT8        DevDma2;\
} ldxn##_V_DATA;\
typedef struct {\
    UINT8        DevEnable;\
    UINT8        DevPrsId;\
    UINT8        DevMode;\
} ldxn##_NV_DATA;

#else

//#include <Protocol\AmiSio.h>

#define STR_BUFFER_LENGTH   0x10
//Defination of function
#define VOLTAGE             0x01
#define TEMPERATURE         0x02
#define FAN_SPEED           0x03

#define LEFT_JUSTIFY              0x01
#define PREFIX_SIGN               0x02
#define PREFIX_BLANK              0x04
#define COMMA_TYPE                0x08
#define LONG_TYPE                 0x10
#define PREFIX_ZERO               0x20

#define CHARACTER_NUMBER_FOR_VALUE 30

typedef struct _STR_UID{
    UINT8   Type;
    UINT8   UID;
    UINT16  StrTokenV;
}STR_UID;

typedef struct {
	UINT16      Token;  // String token value
	UINT8       Type;   // For what? Temperature, Fan, Voltage...
	UINT16      Value;  // Monitor value
	UINT8       OddPos; // Value precision
} HWM_DATA;

typedef void (SIO_EXTERNAL_FUN)(
    IN OUT HWM_DATA * Data
);

void AdjustString(
    IN OUT  CHAR16  * Buffer,
    IN      CHAR16  * StringToChged,
    IN      UINT8   STCLen,
    IN      CHAR16  * HeadBuf,
    IN      UINT8   HeadLen,
    IN      BOOLEAN Flag,
    IN      UINT8   MidPos,
    IN      CHAR16  * TailBuf,
    IN      UINT8   TailLen
);

UINTN
HHMEfiValueToString (
    IN  OUT CHAR16  *Buffer,
    IN  INT64       Value,
    IN  UINTN       Flags,
    IN  UINTN       Width
);

void HHMCommon(
//tag20120326>function declare and definition parameter type is not matched
//    IN      UINTN   RegData,
    IN      UINT64   RegData,
//tag20120326<
    IN      UINT8   Func,
    IN      UINT16  StrToken,
    IN      UINT8   RefValue,
    EFI_HII_HANDLE    hiiHandle
);

VOID SetSioStrings(
    VOID	        *Dev, //Must be type casted to (SIO_DEV2*)
    EFI_HII_HANDLE  HiiHandle);
#endif //VFR_COMPILE

#define SIO_FORM_SET_CLASS 0x99

// {941BC855-BF7E-4fcb-882F-7AEAD15C9D47}
#define SIO_FORM_SET_GUID {0x941BC855, 0xBF7E, 0x4fcb, 0x88, 0x2F, 0x7A, 0xEA, 0xD1, 0x5C, 0x9D, 0x47}
#define SIO_VARSTORE_GUID	SIO_VAR_GUID	

#define SIO_MAIN_FORM_ID 	1
#define SIO_LD_FORM_ID 		2

#define SIO_LD_LAUNCH_KEY 	0x55

#pragma pack(push, 1)

typedef struct _SIO_DEV_V_DATA {
	UINT8		DevImplemented;	
	UINT16		DevBase1;
	UINT16		DevBase2;
	UINT8		DevIrq1;
	UINT8		DevIrq2;
	UINT8		DevDma1;
	UINT8		DevDma2;
} SIO_DEV_V_DATA;

typedef struct _SIO_DEV_NV_DATA {
	UINT8		DevEnable;
	UINT8		DevPrsId;
	UINT8		DevMode;
} SIO_DEV_NV_DATA;

#pragma pack(pop)

#define SIO_GOTO_ID_BASE 		0x8000
#define SIO_GOTO_LABEL_START	0xC001
#define SIO_GOTO_LABEL_END		0xC002
#define SIO_LD_LABEL_START		0xC003
#define SIO_LD_LABEL_END		0xC004

#define SIO_LD_ENABLE_QID_BASE	0x1000
#define SIO_LD_RESSEL_QID_BASE	0x2000
#define SIO_LD_MODE_QID_BASE	0x3000



/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
