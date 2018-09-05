//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/AmiTcgPlatform/AmiTcgPlatformDxe/AmiTcgPlatformDxeLib.h 4     7/25/11 3:45a Fredericko $
//
// $Revision: 4 $
//
// $Date: 7/25/11 3:45a $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:	AmiTcgPlatformDxeLib.h
//
// Description:	Header firl for AmiTcgPlatformDxe library
//
//<AMI_FHDR_END>
//*************************************************************************
//#include <Protocol\SmmBase.h>
//#include <Protocol\SmmSwDispatch.h>
#include <Token.h>
#include <Include/Acpi.h>
#include <Include/Acpi11.h>
#include <Include/Setup.h>
//#include <AmiDxeLib.h>
#include <Library/BaseMemoryLib.h>
#include <AmiTcg/TCGMisc.h>
#include <Protocol/TcgService.h>
#include <Protocol/TpmDevice.h>
#include <Protocol/CpuIo.h>
#include <Protocol/AcpiSupport.h>
#include <AmiTcg/TcgPc.h>

#define TCPA_PPIOP_ENABLE                       1
#define TCPA_PPIOP_DISABLE                      2
#define TCPA_PPIOP_ACTIVATE                     3
#define TCPA_PPIOP_DEACTIVATE                   4
#define TCPA_PPIOP_CLEAR                        5
#define TCPA_PPIOP_ENABLE_ACTV                  6
#define TCPA_PPIOP_DEACT_DSBL                   7
#define TCPA_PPIOP_OWNER_ON                     8
#define TCPA_PPIOP_OWNER_OFF                    9
#define TCPA_PPIOP_ENACTVOWNER                  10
#define TCPA_PPIOP_DADISBLOWNER                 11
#define TCPA_PPIOP_UNOWNEDFIELDUPGRADE          12
#define TCPA_PPIOP_SETOPAUTH                    13
#define TCPA_PPIOP_CLEAR_ENACT                  14
#define TCPA_PPIOP_SETNOPPIPROVISION_FALSE      15
#define TCPA_PPIOP_SETNOPPIPROVISION_TRUE       16
#define TCPA_PPIOP_SETNOPPICLEAR_FALSE          17
#define TCPA_PPIOP_SETNOPPICLEAR_TRUE           18
#define TCPA_PPIOP_SETNOPPIMAINTENANCE_FALSE    19
#define TCPA_PPIOP_SETNOPPIMAINTENANCE_TRUE     20
#define TCPA_PPIOP_ENABLE_ACTV_CLEAR            21
#define TCPA_PPIOP_ENABLE_ACTV_CLEAR_ENABLE_ACTV   22
#define EFI_TPL_DRIVER           6

#define TCGAML_PR_BYTE      0x0A
#define TCGAML_PR_WORD      0x0B
#define TCGAML_PR_DWORD     0x0C
#define TCGAML_PR_QWORD     0x0E
#define TCGAML_ZERO_OP      0x00
#define TCGAML_ONE_OP       0x01

//Opcode Definition for Name Object
#define TCGAML_OP_ALIAS     0x06
#define TCGAML_OP_NAME      0x08
#define TCGAML_OP_SCOPE     0x10
#define TCGAML_OP_ALIAS     0x06
#define TCGAML_OP_NAME      0x08
#define TCGAML_OP_SCOPE     0x10
#define TCGAML_OP_BUFFER    0x11
#define TCGAML_OP_PACKAGE   0x12
#define TCGAML_OP_VARPACK   0x13
#define TCGAML_OP_METHOD    0x14
#define TCGAML_PR_EXTOP     0x5B    //'['
#define TCGAML_PR_PARENT    0x5E    //'^'
#define TCGAML_PR_ROOT      0x5C    //'\'

#define TCGAML_OP_RETURN    0xA4

#define TCGAML_OP_OPREG     0x80
#define TCGAML_OP_DEVICE    0x82
#define TCGAML_OP_PROC      0x83
#define TCGAML_OP_PWRRES    0x84
#define TCGAML_OP_THERMAL   0x85

#ifndef SMBIOS_WAKEUP_TYPE_UNKNOWN
#define SMBIOS_WAKEUP_TYPE_UNKNOWN      0x02
#endif
#ifndef SMBIOS_WAKEUP_TYPE_POWER_SWITCH
#define SMBIOS_WAKEUP_TYPE_POWER_SWITCH     0x06
#endif

#define AMI_TPM_HARDWARE_PRESET        0
#define AMI_TPM_HARDWARE_NOTPRESET     1
#define AMI_TPM_HARDWARE_SETUP_REQUEST_ERROR     1


EFI_STATUS GetDsdt(
    IN OUT mem_in *dsdt );

EFI_STATUS getSetupData (
    SETUP_DATA** ppsd,
    UINT32    * pattr,
    UINTN     * psz );

EFI_STATUS GetTcgWakeEventType(
    IN OUT UINT8 *pWake  );

EFI_STATUS UpDateASL(
    IN UINT8 TcgSupport );

UINT8 ReadPpiRequest( );

void Update_PpiVar(
    IN UINT8 value );

void WritePpiResult(
    IN UINT8  last_op,
    IN UINT16 status );

VOID* LocateATcgHob(
    IN UINTN                   NoTableEntries,
    IN EFI_CONFIGURATION_TABLE *ConfigTable,
    IN EFI_GUID                *HOB_guid );

EFI_STATUS GetNextGuidHob(
        IN OUT VOID          **HobStart,
        IN EFI_GUID          * Guid,
        OUT VOID             **Buffer,
        OUT UINTN            *BufferSize OPTIONAL );


#if TPM_PASSWORD_AUTHENTICATION
BOOLEAN check_authenticate_set();

BOOLEAN check_user_is_administrator( );
#endif


EFI_STATUS readTpmSetup(
    IN OUT UINT8* enable_old,
    IN OUT UINT8* enable,
    IN OUT UINT8* operation,
    IN OUT UINT8* support );


EFI_STATUS UpdateTpmSetup(
    IN UINT8 enabled,
    IN UINT8 op );

EFI_STATUS UpdateTpmCapSetup(
    IN UINT8 EnaDis,
    IN UINT8 ActDeact,
    IN UINT8 OwnUnOwned,
    IN UINT8 Hardware,
    IN UINT8 TpmError);
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
