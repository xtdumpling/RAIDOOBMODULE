//***************************************************************************
//**       (C)Copyright 1993-2011 Supermicro Computer, Inc.                **
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//		Bug Fix:  Fix USB boot group device can not be recognized when use GetLegacyDevOrderType function call problem.
//		Reason:   
//		Auditor:  Jacker Yeh
//		Date:     07/20/11
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012 American Megatrends Inc.           **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway Suite 200 Norcross GA 30093           **
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
//<AMI_FHDR_START>
//
// Name:  BcpBootOrder.h
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef __BcpBootOrder__H__
#define __BcpBootOrder__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
#include <token.h>

//GUID of the section: 962BA95F-4704-478e-8E03-470951F0F99A
#define BCP_BOOT_ORDER_SECTION_GUID \
    { 0x962BA95F, 0x4704, 0x478e, 0x8E, 0x03, 0x47, 0x09, 0x51, 0xF0, 0xF9, 0x9A }

#define BCP_BOOT_ORDER_FILE_GUID \
    { 0x12bfca88, 0x7a2f, 0x4ab5, 0x9a, 0x5d, 0xc4, 0x0c, 0xa6, 0x8b, 0xf7, 0x5f }


#define BCP_FDD0                    0
#define BCP_FDD1                    1
#define BCP_USB_FDD                 2
#define BCP_PM_CD                   16
#define BCP_PS_CD                   17
#define BCP_SM_CD                   18
#define BCP_SS_CD                   19
#define BCP_SATA0_CD                20
#define BCP_SATA1_CD                21
#define BCP_SATA2_CD                22
#define BCP_SATA3_CD                23
#define BCP_SATA4_CD                24
#define BCP_SATA5_CD                25
#define BCP_SATA6_CD                26
#define BCP_SATA7_CD                27
#define BCP_USB_CD                  28
#define BCP_PM_HDD                  32
#define BCP_PS_HDD                  33
#define BCP_SM_HDD                  34
#define BCP_SS_HDD                  35
#define BCP_SATA0_HDD               36
#define BCP_SATA1_HDD               37
#define BCP_SATA2_HDD               38
#define BCP_SATA3_HDD               39
#define BCP_SATA4_HDD               40
#define BCP_SATA5_HDD               41
#define BCP_SATA6_HDD               42
#define BCP_SATA7_HDD               43
#define BCP_USB_HDD                 44
#define BCP_EMBEDDED_NETWORK        48
#define BCP_BEV                     56

#define BCP_UNDEFINED               0xFF

//   BCP Type               BBS Type            MSG_DP SubType  Order 
#define BCP_DEVICE_INFO_ARRAY \
   {{BCP_FDD0,              BBS_FLOPPY,         BCP_UNDEFINED,  BCP_UNDEFINED},\
    {BCP_FDD1,              BBS_FLOPPY,         BCP_UNDEFINED,  BCP_UNDEFINED},\
    {BCP_USB_FDD,           BBS_USB,         	MSG_USB_DP,     BCP_UNDEFINED},\
    {BCP_PM_CD,             BBS_CDROM,          MSG_ATAPI_DP,   0},\
    {BCP_PS_CD,             BBS_CDROM,          MSG_ATAPI_DP,   1},\
    {BCP_SM_CD,             BBS_CDROM,          MSG_ATAPI_DP,   2},\
    {BCP_SS_CD,             BBS_CDROM,          MSG_ATAPI_DP,   3},\
    {BCP_SATA0_CD,          BBS_CDROM,          MSG_SATA_DP,    0},\
    {BCP_SATA1_CD,          BBS_CDROM,          MSG_SATA_DP,    1},\
    {BCP_SATA2_CD,          BBS_CDROM,          MSG_SATA_DP,    2},\
    {BCP_SATA3_CD,          BBS_CDROM,          MSG_SATA_DP,    3},\
    {BCP_SATA4_CD,          BBS_CDROM,          MSG_SATA_DP,    4},\
    {BCP_SATA5_CD,          BBS_CDROM,          MSG_SATA_DP,    5},\
    {BCP_SATA6_CD,          BBS_CDROM,          MSG_SATA_DP,    6},\
    {BCP_SATA7_CD,          BBS_CDROM,          MSG_SATA_DP,    7},\
    {BCP_USB_CD,            BBS_USB,          	MSG_USB_DP,     BCP_UNDEFINED},\
    {BCP_PM_HDD,            BBS_HARDDISK,       MSG_ATAPI_DP,   0},\
    {BCP_PS_HDD,            BBS_HARDDISK,       MSG_ATAPI_DP,   1},\
    {BCP_SM_HDD,            BBS_HARDDISK,       MSG_ATAPI_DP,   2},\
    {BCP_SS_HDD,            BBS_HARDDISK,       MSG_ATAPI_DP,   3},\
    {BCP_SATA0_HDD,         BBS_HARDDISK,       MSG_SATA_DP,    0},\
    {BCP_SATA1_HDD,         BBS_HARDDISK,       MSG_SATA_DP,    1},\
    {BCP_SATA2_HDD,         BBS_HARDDISK,       MSG_SATA_DP,    2},\
    {BCP_SATA3_HDD,         BBS_HARDDISK,       MSG_SATA_DP,    3},\
    {BCP_SATA4_HDD,         BBS_HARDDISK,       MSG_SATA_DP,    4},\
    {BCP_SATA5_HDD,         BBS_HARDDISK,       MSG_SATA_DP,    5},\
    {BCP_SATA6_HDD,         BBS_HARDDISK,       MSG_SATA_DP,    6},\
    {BCP_SATA7_HDD,         BBS_HARDDISK,       MSG_SATA_DP,    7},\
    {BCP_USB_HDD,           BBS_USB,      	MSG_USB_DP,     BCP_UNDEFINED},\
    {BCP_EMBEDDED_NETWORK,  BBS_EMBED_NETWORK,  BCP_UNDEFINED,  BCP_UNDEFINED},\
    {BCP_BEV,               BBS_BEV_DEVICE,     BCP_UNDEFINED,  BCP_UNDEFINED},\
    {BCP_UNDEFINED,         BCP_UNDEFINED,      BCP_UNDEFINED,  BCP_UNDEFINED}}


typedef struct {
    UINT8  TokenNumber;
    UINT16 BbsType;
    UINT8  MsgDpSubType;
    UINT8  BusOrder;
} BCP_BOOT_ORDER_ITEM_INFO;


typedef struct {
    EFI_GUID Guid;
    UINT16   NumberOfOptions;
    UINT16   Option[1];
} BCP_BOOT_ORDER_SECTION;


typedef struct {
    UINT16 Lenght;
    UINT16 Order[BCP_DEFAULT_BOOT_ORDER_LENGTH];
} BCP_BOOT_ORDER_STRUC;


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012 American Megatrends Inc.           **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway Suite 200 Norcross GA 30093           **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
