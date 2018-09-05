//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


#include <IndustryStandard/Acpi30.h>
#include <Acpi.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiSupport.h>
#include <Acpi20.h>


#define FACP_SIG 0x50434146  //"FACP" Fixed ACPI Description Table (FADT)
#define DSDT_SIG 0x54445344  //"DSDT" Differentiated System Description Table

#define EFI_ACPI_TABLE_VERSION_X        (EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0 | EFI_ACPI_TABLE_VERSION_4_0)
#define EFI_ACPI_TABLE_VERSION_ALL      (EFI_ACPI_TABLE_VERSION_1_0B|EFI_ACPI_TABLE_VERSION_X)

//Type of object
typedef enum
{
    otNone=0,
    otScope,
    otBuffer,
    otName,
    otProc,
    otTermal,
    otDevice,
    otMethod,
    otPwrRes,
    otOpReg
} ASL_OBJ_TYPE;

//Structure to store ASL Object Information
typedef struct _ASL_OBJ_INFO
{
    VOID            *Object;
    VOID            *ObjName;
    VOID            *DataStart;
    ASL_OBJ_TYPE    ObjType;
    UINTN           Length;
} ASL_OBJ_INFO;

typedef union _AML_NAME_SEG
{
    UINT32      NAME;
    UINT8       Name[4];
} AML_NAME_SEG;


//AMP OP_CODE TYPE Definition
#define AML_OP_CODE     UINT8   //Normal OP_CODE
#define AML_EOP_CODE    UINT16  //Extended OP_CODE


//AML Name Object Size Preffix
#define AML_PR_BYTE     0x0A
#define AML_PR_WORD     0x0B
#define AML_PR_DWORD    0x0C
#define AML_PR_STRING   0x0D
#define AML_PR_QWORD    0x0E

#define AML_PR_DUALNAME 0x2E
#define AML_PR_MULTNAME 0x2F


#define AML_PR_EXTOP    0x5B    //'['
#define AML_PR_ROOT     0x5C    //'\'
#define AML_PR_PARENT   0x5E    //'^'


//Opcode Definition for Name Object
#define AML_OP_ZERO         0x00
#define AML_OP_ONE          0x01
#define AML_OP_ONES         0xFF


#define AML_OP_ALIAS        0x06
#define AML_OP_NAME         0x08
#define AML_OP_SCOPE        0x10
#define AML_OP_BUFFER       0x11
#define AML_OP_PACKAGE      0x12
#define AML_OP_VARPACK      0x13
#define AML_OP_METHOD       0x14

#define AML_OP_RETURN       0xA4

#define AML_OP_OPREG        0x80
#define AML_OP_DEVICE       0x82
#define AML_OP_PROC         0x83
#define AML_OP_PWRRES       0x84
#define AML_OP_THERMAL      0x85


#define AML_NULL_NAME       0x00

typedef struct _AML_PACKAGE_LBYTE
{
    UINT8       Length    : 6;
    UINT8       ByteCount : 2;
    //Package Length Following Bytes 0..3
} AML_PACKAGE_LBYTE;
