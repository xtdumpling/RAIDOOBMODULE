//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.03
//    Bug Fix:  Add Full SmBios InBand support.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/05/2016
//
//  Rev. 1.02
//    Bug Fix:  Re-write SmcOutBand module.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/02/2016
//
//  Rev. 1.01
//    Bug Fix:  Extend FileSize in InBand header.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/22/2016
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     May/31/2016
//
//****************************************************************************
#ifndef	_H_SmcInBandSmbios_
#define	_H_SmcInBandSmbios_

//#include "Token.h"
#include <AmiLib.h>

#pragma pack(1)

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: InBand_Smbios_Data_Header
//
// Description: Structure for InBand SMbios Data header.
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// "Signature[4]"           UINT8                   Signature - "_SMB"
// Version                  UINT16                  Version
// FileSize                 UINT32                  File size
// Checksum                 UINT8                   Checksum
// Source                   UINT8                   1 : Data come from SW or BMC, 0 : Changed by amidedos
// "Reserve[6]"             UINT8                   Reserve area
//----------------------------------------------------------------------------
//<AMI_THDR_END>

typedef struct {
    UINT8	Signature[4];
    UINT16	Version;
    UINT32	FileSize;	//excluding header
    UINT8	Checksum;	//excluding header
    UINT8	Source;
    UINT8	Reserve[6];
}InBand_Smbios_Data_Header;

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: DMIHeader
//
// Description: Structure for DMI header.
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// structType               UINT8                   Type
// structLength             UINT8                   Length
// structHandle             UINT16                  Handle
//----------------------------------------------------------------------------
//<AMI_THDR_END>

typedef struct {
    UINT8	structType;
    UINT8	structLength;
    UINT16	structHandle;
}DMIHeader;

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: DMIType0
//
// Description: BIOS Information Structure (Type 0)
//----------------------------------------------------------------------------
//<AMI_THDR_END>

typedef struct {
    DMIHeader header;
    UINT8	vendor;
    UINT8	BIOSVersion;
    UINT16	BIOSStartAddrSeg;
    UINT8	BIOSReleaseDate;
    UINT8	BIOSROMSize;
    UINT64	BIOSCharacteristics;
    UINT8	BIOSCharacteristicsExt;
}DMIType0;

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: DMIType1
//
// Description: System Information Structure (Type 1)
//----------------------------------------------------------------------------
//<AMI_THDR_END>

typedef struct {
    DMIHeader	header;
    UINT8	manufacturer;
    UINT8	productName;
    UINT8	version;
    UINT8	serialNumber;
    UINT8	UUID[16];
    UINT8	wakeUpType;
    UINT8	skuNumber;
    UINT8	family;
}DMIType1;

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:  DMIType2
//
// Description: Base Board (or Module) Information Structure (Type 2)
//----------------------------------------------------------------------------
//<AMI_SHDR_END>

typedef struct {
    DMIHeader	header;
    UINT8	manufacturer;
    UINT8	productName;
    UINT8	version;
    UINT8	serialNumber;
    UINT8	assetTag;
}DMIType2;

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:  DMIType3
//
// Description: System Enclosure or Chassis Information Structure (Type 3)
//----------------------------------------------------------------------------
//<AMI_SHDR_END>

typedef struct {
    DMIHeader	header;
    UINT8	manufacturer;
    UINT8	chassisType;
    UINT8	version;
    UINT8	serialNumber;
    UINT8	assetTagNumber;
    UINT8	bootupState;
    UINT8	powerSupplyState;
    UINT8	thermalState;
    UINT8	securityStatus;
    UINT32	oemdefined;
}DMIType3;

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:  DMIType11
//
// Description: OEM Strings Structure (Type 11)
//----------------------------------------------------------------------------
//<AMI_SHDR_END>

typedef struct {
    DMIHeader	header;
    UINT8	count;
}DMIType11;

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:  DMIType39
//
// Description: System Power Supply (Type 39)
//----------------------------------------------------------------------------
//<AMI_SHDR_END>

typedef struct {
    DMIHeader	header;
    UINT8	powerUnitGroup;
    UINT8	location;
    UINT8	deviceName;
    UINT8	manufacturer;
    UINT8	serialNumber;
    UINT8	assetTagNumber;
    UINT8	modelPartNumber;
    UINT8	revisionLevel;
    UINT16	maxPowerCapaccity;
    UINT16	powerSupplyChar;
    UINT16	inputVoltage;
    UINT16	coolingDevice;
    UINT16	inputCurrent;
}DMIType39;

#pragma pack()

VOID InBandUpdateSmbios(void);

VOID CombineSmbios(void);

#endif
