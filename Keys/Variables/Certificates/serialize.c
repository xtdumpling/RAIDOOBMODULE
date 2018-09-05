#include <AmiCertificate.h>
#include "TimeStamp.h"
#define    PK     1
#define    KEK    2
#define    db     3
#define    dbt    4
#define    dbx    5
#define    dbr    6
#include "serialize.h"

#pragma pack(1)           // force byte alignment

typedef struct {
#if VarName == 1 || VarName == 3
    UINT8       Var[4];
#else
    UINT8       Var[6];
#endif
    EFI_GUID    GUID;
    UINT32      Attribs;
    EFI_TIME    TimeStamp;
} EFI_VAR_SERIALIZE;

EFI_VAR_SERIALIZE Serialize = {
#if VarName == 1
    'P', 00, 'K', 00,
    EFI_GLOBAL_VARIABLE,
#endif
#if VarName == 2
    'K', 00, 'E', 00, 'K', 00,
    EFI_GLOBAL_VARIABLE,
#endif
#if VarName == 3
    'd', 00, 'b', 00,
    EFI_IMAGE_SECURITY_DATABASE_GUID,
#endif
#if VarName == 4
    'd', 00, 'b', 00, 't', 00,
    EFI_IMAGE_SECURITY_DATABASE_GUID,
#endif
#if VarName == 5
    'd', 00, 'b', 00, 'x', 00,
    EFI_IMAGE_SECURITY_DATABASE_GUID,
#endif
#if VarName == 6
    'd', 00, 'b', 00, 'r', 00,
    EFI_IMAGE_SECURITY_DATABASE_GUID,
#endif
#if VarAppend == 1
    0x67,
#else
    0x27,
#endif
    FOUR_DIGIT_YEAR_INT, 
    TWO_DIGIT_MONTH_INT, 
    TWO_DIGIT_DAY_INT, 
    TWO_DIGIT_HOUR_INT, 
    TWO_DIGIT_MINUTE_INT, 
    TWO_DIGIT_SECOND_INT,
/*    2015,//FOUR_DIGIT_YEAR_INT, 
    01,//TWO_DIGIT_MONTH_INT, 
    01,//TWO_DIGIT_DAY_INT, 
    00,//TWO_DIGIT_HOUR_INT, 
    00,//TWO_DIGIT_MINUTE_INT, 
    00,//TWO_DIGIT_SECOND_INT,*/
    0,0,0,0,0,
};
