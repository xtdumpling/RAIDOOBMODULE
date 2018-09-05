//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//     Rev. 1.00        From SmcPkg\Module\SmcDevReport\SmcVPD.h 1.05
//       Reason:        
//       Auditor:       Stephen Chen
//       Date:          Sep/29/16
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  SmcVPD.h
//
// Description:	Supermicro OOB VPD feature header file.
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Setup.h>
#include <Library/PcdLib.h>
#include <Protocol\IPMITransportProtocol.h>
#include <Protocol\AmiSmbios.h>
#include <Library/MmPciBaseLib.h>
#include <Library\UefiBootServicesTableLib.h>
#if defined SMC_PLATFORM_TYPE && SMC_PLATFORM_TYPE == 0x01 // Server
#include "SmcServerLib.h"
#endif

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
#define VpdLanMACSize                   12
#define Cmd_Retry                       3

#pragma pack(1)

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: VPD_DATA_HEADER
//
// Description: VPD structure header definition.
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// Signature                UINT8                   VPD data signature "_VPD"
// Version                  UINT16                  VPD data version
// FileSize                 UINT16                  Total size of VPD data(exclude header)
// Checksum                 UINT8                   VPD data Checksum(exclude header)
// offset                   UINT16                  Point to next VPD data
// NumOfLan                 UINT8                   Number of LAN which support VPD
// NumOfAOCLan              UINT8                   Number of AOC LAN card
// TotalLanOfAOC            UINT8                   Total LAN of AOC card, for multi-function LAN checking
// Reserve                  UINT8                   Reserved for future use
// 
//----------------------------------------------------------------------------
//<AMI_THDR_END>
typedef struct {
	UINT8	Signature[4];       //"_VPD"
	UINT16	Version;
	UINT16	FileSize;           //Total size of VPD data(exclude header)
	UINT8	Checksum;           //Checksum(exclude header)
	UINT16	offset;             //Point to next VPD data
	UINT8	NumOfLan;           //Number of LAN which support VPD
	UINT8   NumOfAOCLan;        //Number of AOC LAN card
	UINT8	TotalLanOfAOC;      //Total LAN of AOC card
	UINT8	Reserve[2];
}VPD_DATA_HEADER;

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: VPD_DATA
//
// Description: VPD structure definition.
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// header                   VPD_DATA_HEADER         VPD data header
// *data                    UINT8                   VPD data buffer
// 
//----------------------------------------------------------------------------
//<AMI_THDR_END>
typedef struct {
	VPD_DATA_HEADER header;
	UINT8	*data;
}VPD_DATA;

#pragma pack()

UINT8
SmcFindPciCapId (
  IN UINTN		DeviceBase,
  IN UINT8		CapId
  );

EFI_STATUS
GetVpdData (
  IN     UINTN		PciAddress,
  IN     UINT16		offset,
  IN OUT VOID		*VpdData,
  IN     UINT8		DataType
  );

EFI_STATUS
HandleLanVpd (
  IN UINTN		PciAddress,
  IN OUT VPD_DATA	*LanVpd,
  IN OUT UINT8		*LanMAC
  );

EFI_STATUS
X11DPTBVPDDataCollect(
  IN UINT8		LanBus,
  IN OUT UINT8		*LanMACBuffer
  );


//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2013, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
