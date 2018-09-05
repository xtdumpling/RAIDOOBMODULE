//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Add SSID and SVID into AssetInfo structure.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Aug/24/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     Jul/25/2016
//
//****************************************************************************
//****************************************************************************
#ifndef	_H_SmcVPD_
#define	_H_SmcVPD_

#define EFI_PCI_CAPABILITY_ID_VPD       0x03
#define PCI_VPD_LRDT                    0x80    /* Large Resource Data Type */
#define PCI_VPD_LRDT_ID(x)              (x | PCI_VPD_LRDT)

/* Large Resource Data Type Tag Item Names */
#define PCI_VPD_LTIN_ID_STRING          0x02    /* Identifier String */
#define PCI_VPD_LTIN_RO_DATA            0x10    /* Read-Only Data */
#define PCI_VPD_LTIN_RW_DATA            0x11    /* Read-Write Data */

#define PCI_VPD_LRDT_ID_STRING          PCI_VPD_LRDT_ID(PCI_VPD_LTIN_ID_STRING)
#define PCI_VPD_LRDT_RO_DATA            PCI_VPD_LRDT_ID(PCI_VPD_LTIN_RO_DATA)
#define PCI_VPD_LRDT_RW_DATA            PCI_VPD_LRDT_ID(PCI_VPD_LTIN_RW_DATA)

/* Small Resource Data Type Tag Item Names */
#define PCI_VPD_STIN_END                0x78    /* End */
#define PCI_VPD_SRDT_END                PCI_VPD_STIN_END
#define VPD_LENGTH                      0x1000

#define VpdDataWidth8                   8
#define VpdDataWidth16                  16
#define VpdDataWidth32                  32
#define VpdLanSNSize                    VPD_LENGTH_OF_LAN_SN
#define Cmd_Retry                       3

#pragma pack(1)

typedef struct{
    UINT8	Signature[4];		// "_AST"
    UINT8	Version;
    UINT8	AST_H_Len;		// Asset information header length
    UINT8	VID_DID[4];		// vendor ID, device ID
    UINT8	SVID_SSID[4];		// sub vendor ID, sub system ID
    UINT8	ClassCode;		// device class code
    UINT8	SubClassCode;		// device sub class code
    UINT8	ProgramInterface;       // program interface
}AssetInfo_Header;

typedef struct{
    UINT8	Signature[4];		// "_VPD"
    UINT8	Version;		// Byte 4: Major number, Byte 5: Minor number
    UINT8	VPD_H_Length;		// Total size of VPD data(exclude header)
}VPD_HEADER;

typedef struct{
    UINT8	Type;
    UINT8	Length;
    UINT16	Handle;
    UINT8       NumAdditionalInfoEntries;
    UINT8	EntryLength;
    UINT16	RefHandle;
    UINT8	RefOffset;
    UINT8	StringNum;
    AssetInfo_Header    AssetInfo_H;
}SMBIOS_ADDITIONAL_INFO; 

#pragma pack()

EFI_STATUS
SmcVPDPkgCollect(
    IN	UINT8	bus,
    IN	UINT8	dev,
    IN	UINT8	fun,
    IN	UINT8	SlotType,
    IN	UINT8	SlotID,
    IN OUT	UINT8	*VPD_Data,
    IN OUT	UINT16	*VPD_Len
);
#endif
//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2013, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
