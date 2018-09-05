//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log: $
// 
// 
//**********************************************************************
/** @file Meud.h

**/
//**********************************************************************
#ifndef _EFI_MEUD_H_
#define _EFI_MEUD_H_
#ifdef __cplusplus
extern "C" {
#endif

VOID        MeudEntry(VOID *Buffer,UINT8 *pOFBDDataHandled);
VOID        MeudInSmm(VOID);
UINT32      GetFlashCapacity(VOID);
UINT32      GetHfs(VOID);
EFI_STATUS  HmrfpoLockMsg(VOID);
EFI_STATUS  HmrfpoEnableMsg(VOID);
VOID        CspMeudInSmm(VOID);
EFI_STATUS  CspReportMeInfo(UINT8 Func_Num, UINT32* BASE_Address, UINT32* Length);
EFI_STATUS
GetRegionOffset(
    IN UINT8    Region,
    IN OUT UINT32*  Offset,
    IN OUT UINT32*  Length
);
#define MEUD_ERROR_GUID \
{ 0x732BD39, 0xD6B0, 0x4039, 0xB6, 0xC2, 0x96, 0x54, 0x46, 0x6D, 0xE5, 0x25 }

#ifdef FLASH_BASE_ADDRESS
#undef FLASH_BASE_ADDRESS
#endif
#define FLASH_BASE_ADDRESS(a) (UINTN)(0xFFFFFFFF - GetFlashCapacity() + 1 + (UINTN)a)


#define ME_BLK_PROTECT BIT00

typedef enum
{
    //Old definition
    FDT_BLK,    //Region0 - Descriptor
    PDR_BLK,    //Region4 - Platform Data
    GBE_BLK,    //Region3 - GbE
    ME_BLK,     //Region2 - ME
    ME_OPR_BLK,
    BIOS_BLK,   //Region1 - Bios
    DER_BLK,    //Region5 - DeviceExpansion1
    //Programming guide definition
    REG7_BLK,
    REG8_BLK,   //Region8 - BMC, EC
    REG9_BLK,   //Region9 - DeviceExpansion2
    REGA_BLK,   //Region10 - IE
    REGB_BLK,   //Region11 - 10 GbE
    REGC_BLK,   //Region12 - Oprom
    REGD_BLK,
    REGE_BLK,
    REGF_BLK,
    //Confilict with old definition
    BIOS_2ND_BLK,    //Region6 - 2nd BIOS, conflict with DER_BLK
    MAX_BLK
}ME_BLOCK_TYPE;

#pragma pack(1)
typedef struct
{
    UINT8   FlashRegion;
    UINT8   Command[5];
}FLASH_REGIONS_DESCRIPTOR;

typedef struct
{
    CHAR8   Command[4];
    CHAR8   Description[64];
    UINT32	StartAddress;
    UINT32	BlockSize;
    UINT8	Type;
    UINT8	Status;
}ME_BLOCK_T;

typedef struct
{
    UINT8	bReturnStatus;
    UINT8	bSubFunction;
    UINT16	TotalBlocks;
    ME_BLOCK_T  BlockInfo[MAX_BLK];
}OFBD_TC_55_ME_INFO_STRUCT;

typedef struct
{
    UINT8	bReturnStatus;
    UINT8	bSubFunction;
    UINT32	TotalBlocks;
    UINT32	BlockIndex;
    UINT8	bHandleRequest;
    UINT8	bBlockType;
    UINT16	UpdateResult;
    UINT32	ddMeDataBuffer;
    UINT32	ddMeDataSize;
    UINT32	ddMeDataChecksum;
    UINT32	ddMessageBuffer;
}OFBD_TC_55_ME_PROCESS_STRUCT;
#pragma pack()
VOID
MEProcessHandler(
    IN OUT OFBD_TC_55_ME_PROCESS_STRUCT  **MEProcessStructPtr
);

    /****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
