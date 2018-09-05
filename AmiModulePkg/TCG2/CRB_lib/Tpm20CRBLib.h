//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Libraries/Tpm20CrbLib/Tpm20CRBLib.h 1     10/08/13 11:59a Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 11:59a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Libraries/Tpm20CrbLib/Tpm20CRBLib.h $
//
// 1     10/08/13 11:59a Fredericko
// Initial Check-In for Tpm-Next module
//
// 2     9/16/13 1:51p Fredericko
//
// 1     7/10/13 5:51p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Efi.h>
#include <Token.h>


//Defines ACPI Interface for Control Resource Buffer Access
#define TPM20H2NS(x)        ((((x) << 8) | ((x) >> 8)) & 0xffff)
#define TPM20H2NL(x)        (TPM20H2NS ((x) >> 16) | (TPM20H2NS ((x) & 0xffff) << 16))

#define CONTROL_AREA_RSVD                  0x00
#define CONTROL_AREA_ERROR                 0x04
#define CONTROL_AREA_CANCEL                0x08
#define CONTROL_AREA_START                 0x0C
#define CONTROL_AREA_INT                   0x10
#define CONTROL_AREA_CMD_SZ                0x18
#define CONTROL_AREA_CMD_BASE              0x1C
#define CONTROL_AREA_RSP_SZ                0x24
#define CONTROL_AREA_RSP_BASE              0x28

//Timeouts
#define TIMEOUT_A         500           ///< 500 microseconds
#define TIMEOUT_B         500 * 1000    ///< 10ms (max command processing time in PK-TPM ca. 3ms)
#define TIMEOUT_C         1000 * 1000   ///< 1s
#define TIMEOUT_D         500 * 1000    ///< 500 ms
#define POLLING_PERIOD    140           ///< Poll register every 140 microsecondss

#define CRB_DEVICE_CMD     0x40
#define CRB_DEVICE_STS     0x44

#define TPM20_CRB_IDLE     0x02

#define TPM20_MAX_COMMAND_SIZE     0x0F80
#define StartStatusFieldSet        0x00000001

#define TPM20_CRB_CMD_BASE 0x80
#define TPM20_CRB_RSP_BASE 0x80

#define TPM20_CRBSTATUS_START      0x00000001
#define TPM20_CRB_ALL_CLEAR        0xFFFFFFFF

#define RESPONSE_HEADER_SIZE  12

#define B_CRB_LOCALITY_STATE_LOCALITY_ASSIGNED  0x00000002 ///< BIT1
#define V_CRB_LOCALITY_STATE_ACTIVE_LOC_MASK    0x0000001C /// Bits [4:2]


typedef struct
{
    UINT16 tag;
    UINT32 paramSize;
    UINT32 responseCode;
} CHK_RESPONSE_HEADER;

#pragma pack(1)

typedef struct _STATUS_FIELD_
{
    UINT32   Rsvd;
    UINT32   Error;
    UINT32   Cancel;
    UINT32   Start;
} STATUS_FIELD;


typedef struct
{
    EFI_HOB_GUID_TYPE     EfiHobGuidType;
    UINT64                PttBufferAddress;
    UINT32                LocalityState;
} ME_DATA_HOB;


typedef struct _CONTROL_AREA_LAYOUT_
{
    STATUS_FIELD StatusFieled;
    UINT64       IntCntrl;
    UINT32       Cmd_Sz;
    UINT64       Cmd_Addrs;
    UINT32       Rsp_Sz;
    UINT64       Rsp_Addrs;
} CONTROL_AREA_LAYOUT;

typedef struct _GENERIC_RESP_HDR_
{
    UINT16 Tag;
    UINT32 RespSize;
    UINT32 RespCode;
} GENERIC_RESP_HDR;

typedef struct _TPM_CRB_ACCESS_REG_
{
    UINT32 TpmlocState; //00-03
    UINT32 Reserved;    //04-07
    UINT32 TpmlocCtrl;  //08-0B
    UINT32 TpmlocSts;   //0C-0F
    UINT32 Reserved1[0x8];    //10-2F
    UINT32 TpmCrbIntfId[2];   //30-37
    UINT32 TpmCrbCtrlExt[2];  //38-3F
    UINT32 TpmCrbCtrlReq;     //40-43
    UINT32 TpmCrbCtrlSts;     //44-47
    UINT32 TpmCrbCtrlCncl;    //48-4B
    UINT32 TpmCrbCtrlStrt;    //4C-4F
    UINT32 TpmCrbCtrlInt[2];  //50-57
    UINT32 TpmCrbCtrlCmdSize; //58-5B
    UINT32 TpmCrbCtrlCmdLAddr; //5C-5F
    UINT32 TpmCrbCtrlCmdHAddr; //60-63
    UINT32 TpmCrbCtrlRespSize; //64-67
    UINT32 TpmCrbCtrlResdLAddr; //68-6B
    UINT32 TpmCrbCtrlResdHAddr; //6C-6F
    UINT32 Reserved2[0x4];     //70-7F
} TPM_CRB_ACCESS_REG;


typedef volatile TPM_CRB_ACCESS_REG *TPM_CRB_ACCESS_REG_PTR;

#pragma pack()


EFI_STATUS
EFIAPI
CrbSubmitCmd(
    IN      UINT8     *InputBuffer,
    IN      UINT32     InputBufferSize,
    OUT     UINT8     *OutputBuffer,
    OUT     UINT32    *OutputBufferSize);

BOOLEAN isTpm20CrbPresent();

UINT8 GetCurrentInterfaceType();
BOOLEAN IsPTP();
BOOLEAN CrbSupported();
BOOLEAN FIFOSupported();
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
