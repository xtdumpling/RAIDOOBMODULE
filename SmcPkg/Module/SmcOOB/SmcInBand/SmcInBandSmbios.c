//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.05
//    Bug Fix:  Sync solution from purley to prevent SUM test case 306 failed.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Jan/30/2018
//
//  Rev. 1.04
//    Bug Fix:  Prevent SMBIOS combining failed. The checksum of combining SMBIOS maybe zero.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Mar/17/2017
//
//  Rev. 1.03
//    Bug Fix:  Check the string of DMI, if it has invalid char, convert to empty string.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Mar/02/2017
//
//  Rev. 1.02
//    Bug Fix:  Extend FileSize in InBand header.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/22/2016
//
//  Rev. 1.01
//    Bug Fix:  Fix get smbios error.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jul/14/2016
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     May/31/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcInBandSmbios.c
//
// Description: SMC In-Band SMBIOS feature.
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/PrintLib.h>
#include "SmcInBand.h"
#include "AmiCompatibilityPkg/Smbios/SmbiosDmiEdit/SmbiosDMIEdit.h"
#include "Protocol/SMBios.h"
#include "Protocol/AmiSmbios.h"
#include "SmcRomHole.h"
#include <Guid/SmcOobPlatformPolicy.h>
#include <Protocol/SmcRomHoleProtocol.h>
#include <AmiLib.h>

extern	AMI_SMBIOS_PROTOCOL	*SmbiosProtocol;

extern  SMC_OOB_PLATFORM_POLICY      *mSmcOobPlatformPolicyPtr;
extern  SMC_ROM_HOLE_PROTOCOL        *mSmcRomHoleProtocol;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  GetOffsetVaries
//
// Description:
//  Get the Varies from "TempBuff" then fill the Verise
//  to "SmbiosData" and renew the index - "SmbiosDataIndex".
//  TempBuff : Get from system by SmbiosReadStructure.
//  SmbiosData : final date we want.
//  SmbiosData format defined by SMC:
//
//  Offset        Size         Description
//  =======================================
//  0             1            TypeNumber
//  1             2            TypeHandle
//  3             1            Veries Offset in Type
//  4             VariesSize   Verise data and a endding char 0x00
//
// Input:
//      IN UINT8 TypeNumber - Type number.
//      IN UINT16 TypeHandle - Type handle.
//      IN UINT8 OffsetIndex - Offset index.
//      IN UINT8 VariesSize - Varies size.
//
// Output:
//      IN OUT UINT32 *SmbiosDataIndex - Smbios data index.
//      IN OUT UINT8 *SmbiosData - Smbios data we want.
//      IN OUT UINT8 *TempBuff - Smbios data get by SmbiosReadStructure.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
GetOffsetVaries(
    IN	UINT8	TypeNumber,
    IN	UINT16	TypeHandle,
    IN	UINT8	OffsetIndex,
    IN	UINT8	VariesSize,
    IN	OUT	UINT32	*SmbiosDataIndex,
    IN	OUT	UINT8	*SmbiosData,
    IN	OUT	UINT8	*TempBuff
)
{
    DEBUG((-1, "GetOffsetVaries entry.\n"));
// fill TypeNumber, TypeHandle, Veries offset and point index to Veries data space
    *(UINT32*)(&SmbiosData[*SmbiosDataIndex]) = \
            TypeNumber + ((UINT32)TypeHandle << 8) + ((UINT32)OffsetIndex << 24);
    (*SmbiosDataIndex) += 4;
// fill Veries data, detail please refer SMBIOS spec.
    CopyMem(&SmbiosData[*SmbiosDataIndex], &TempBuff[OffsetIndex], VariesSize);
    (*SmbiosDataIndex) += VariesSize;
    SmbiosData[*SmbiosDataIndex] = 0x00;		//add a endding char
    (*SmbiosDataIndex)++;	// index point to next smbios format.
}

BOOLEAN IsVisibleString (CHAR8* StrBuff)
{
    UINT32 Index = 0;
    for (Index = 0; Index < 0xffff; Index++) {
        if (StrBuff[Index] == 0) break;
        if (StrBuff[Index] < 0x20 || StrBuff[Index] > 0x7E) {
            return FALSE;
        }
    }
    return TRUE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  GetOffsetString
//
// Description:
//  Get the string from "TempBuff" then fill the string
//  to "SmbiosData" and renew the index - "SmbiosDataIndex".
//  TempBuff : Get from system by SmbiosReadStructure.
//  SmbiosData : final date we want.
//  SmbiosData format defined by SMC:
//
//  Offset        Size          Description
//  =======================================
//  0             1             TypeNumber
//  1             2             TypeHandle
//  3             1             OffsetIndex
//  4             String Length string data and a endding char 0x00
//
// Input:
//      IN UINT8 TypeNumber - Type number.
//      IN UINT16 TypeHandle - Type handle.
//      IN UINT8 OffsetIndex - Offset index.
//      IN UINT8 OffsetString - Offset string.
//      IN UINT8 structLength - Smbios header length get by SmbiosReadStructureand
//				defined by SMBIOS spec, it point to start address of string.
//      IN UINT32 TempBuffSize - Buffer size.
//
// Output:
//      IN OUT UINT16 *SmbiosDataIndex - Smbios data index.
//      IN OUT UINT8 *SmbiosData - Smbios data we want.
//      IN OUT UINT8 *TempBuff - Smbios data get by SmbiosReadStructure.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
GetOffsetString(
    IN	UINT8	TypeNumber,
    IN	UINT16	TypeHandle,
    IN	UINT8	OffsetIndex,
    IN	UINT8	OffsetString,
    IN	UINT8	structLength,
    IN	UINT32	TempBuffSize,
    IN	OUT	UINT32	*SmbiosDataIndex,
    IN	OUT	UINT8	*SmbiosData,
    IN	OUT	UINT8	*TempBuff
)
{
    UINT32	TempBuffIndex;
    UINT32	SmbiosDataIndexOri;

    DEBUG((-1, "GetOffsetString entry.\n"));
// fill TypeNumber, TypeHandle, Veries offset and point index to Veries data space
    *(UINT32*)(&SmbiosData[*SmbiosDataIndex]) = \
            TypeNumber + ((UINT32)TypeHandle << 8) + ((UINT32)OffsetIndex << 24);
    (*SmbiosDataIndex) += 4;
// fill string data, detail please refer SMBIOS spec.
    SmbiosDataIndexOri = *SmbiosDataIndex;
    for(TempBuffIndex = structLength; TempBuffIndex < TempBuffSize; TempBuffIndex++) {
        if(OffsetString == 1) {	// found the string we want.
            if (!IsVisibleString (&TempBuff[TempBuffIndex])) {
                break;
            }
            Strcpy(&SmbiosData[*SmbiosDataIndex], &TempBuff[TempBuffIndex]);
            (*SmbiosDataIndex) += (UINT32)AsciiStrLen(&TempBuff[TempBuffIndex]);
            SmbiosData[*SmbiosDataIndex] = 0x00;	//add a endding char
            (*SmbiosDataIndex)++;
            break;
        }
        else {	// not found so point to next string
            TempBuffIndex += (UINT32)Strlen(&TempBuff[TempBuffIndex]);
            OffsetString--;
        }
    }

    //
    // If no string, add a ending char('\0') to prevent format error.
    //
    if(SmbiosDataIndexOri == *SmbiosDataIndex) {
        SmbiosData[*SmbiosDataIndex] = 0x00; //add a endding char
        (*SmbiosDataIndex)++;
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  CombineSmbios
//
// Description:
//  Parsing and combine SmBios data we can change then save it to flash part.
//  All Smbios data we can changed are same with AMI smbios tool.
//  TypeNumber - The types need to be saved to flash part.
//  Type00 ~ Type39 -  The offset of Typexx.
//
//  Steps :
//  1. Loop all SmBios tables.
//       Search for the types (TypeNumber)) we need to save.
//       Combine the offsets Veries and String into buffer.
//  2. Calculate checksum of the buffer.
//  3. Combine header.
//  4. Save the header into flash part SMBIOS area(current, previous and default).
//
//  Each variable is a sequence of data bytes.
//  The first 1 byte represents the type of the variable.
//  The next 2 bytes represents the handle of the type.
//  The next 1 byte represents the offset of variable in type and is followed variable string or value.
//  The next 1 byte is a end of byte !¡±00!¡L.
//
//  Header : First 16 bytes are SMCI SMBIOS binary file header.
//    Byte 00 ~ 03 : "_SMB"
//    Byte 04 ~ 05 : In Band Smbios version
//    Byte 06 ~ 07 : SMCI SMBIOS binary file size without header
//    Byte 08      : checksum without header
//    Byte 09      : data source
//	Bit 0 : 0->come from BIOS, 1->come from BMC or SW
//
// Input:
//      None
//
// Output:
//      None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID CombineSmbios(void)
{
    UINT32	TempBuffSize, SmbiosDataSize;
    UINT8	*TempBuff, *SmbiosData, OffsetString, OffsetIndex;
    UINT8	TypeNumber[6] = {0, 1, 2, 3, 11, 39}, TypeNumIndex;
    UINT8	Type00[] = {0x05, 0x08};
    UINT8	Type01[] = {0x04, 0x05, 0x06, 0x07, 0x08, 0x19, 0x1A};
    UINT8	Type02[] = {0x04, 0x05, 0x06, 0x07, 0x08};
    UINT8	Type03[] = {0x04, 0x05, 0x06, 0x07, 0x08, 0x0D};
    UINT8	Type11[] = {0x01, 0x02};
    UINT8	Type39[] = {0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0E, 0x10, 0x12, 0x14};
    UINT8	structLength, checksum = 0, i;
    EFI_STATUS	Status = EFI_SUCCESS;
    CHAR16	*DmiArrayVar = L"DmiArray";
    DMI_VAR	DmiArray[DMI_ARRAY_COUNT] = {0};
    UINTN	DmiArraySize = DMI_ARRAY_COUNT * sizeof(DMI_VAR);

    DEBUG((-1, "CombineSmbios entry.\n"));

    gBS->AllocatePool(EfiBootServicesData, mSmcRomHoleProtocol->SmcRomHoleSize(CURRENT_SMBIOS_REGION), &SmbiosData);
    gBS->SetMem(SmbiosData, mSmcRomHoleProtocol->SmcRomHoleSize(CURRENT_SMBIOS_REGION), 0xff);
//combine data
    SmbiosDataSize = sizeof(InBand_Smbios_Data_Header);
// Step 1: Loop SmBios handle.
    for(TypeNumIndex = 0; TypeNumIndex < sizeof(TypeNumber); TypeNumIndex++) {
        Status = EFI_SUCCESS;
        for(i = 1; !EFI_ERROR(Status); i++) {
            Status = SmbiosProtocol->SmbiosReadStrucByType(
                         TypeNumber[TypeNumIndex],
                         i,
                         &TempBuff,
                         (UINT16*)&TempBuffSize);

            if(Status)	continue;

            DEBUG((-1, "CombineSmbios type[%x].\n", TypeNumber[TypeNumIndex]));
            structLength = ((DMIHeader*)TempBuff)->structLength;

            switch(TypeNumber[TypeNumIndex]) {
            case 0:	//EFI_SMBIOS_TYPE_BIOS_INFORMATION
                for(OffsetIndex = 0; OffsetIndex < sizeof(Type00); OffsetIndex++) {
                    OffsetString = 0;
                    switch(Type00[OffsetIndex]) {
                    case 0x06:
                    case 0x12:
                        GetOffsetVaries(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, \
                                        Type00[OffsetIndex], sizeof(UINT16), &SmbiosDataSize, SmbiosData, TempBuff);
                        break;
                    case 0x09:
                    case 0x14:
                    case 0x15:
                    case 0x16:
                    case 0x17:
                        GetOffsetVaries(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, \
                                        Type00[OffsetIndex], sizeof(UINT8), &SmbiosDataSize, SmbiosData, TempBuff);
                        break;
                    case 0x0A:
                        GetOffsetVaries(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, \
                                        Type00[OffsetIndex], sizeof(UINT64), &SmbiosDataSize, SmbiosData, TempBuff);
                        break;
                    default:	//4, 5, 8,
                        OffsetString = TempBuff[Type00[OffsetIndex]];
                        break;
                    }

                    if(OffsetString) {
                        GetOffsetString(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, Type00[OffsetIndex], \
                                        OffsetString, structLength, TempBuffSize, &SmbiosDataSize, SmbiosData, TempBuff);
                    }
                }
                break;
            case 1:	//EFI_SMBIOS_TYPE_SYSTEM_INFORMATION
                for(OffsetIndex = 0; OffsetIndex < sizeof(Type01); OffsetIndex++) {
                    OffsetString = 0;
                    switch(Type01[OffsetIndex]) {
                    case 0x08:	//UUID[16]
                        GetOffsetVaries(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, \
                                        Type01[OffsetIndex], 16, &SmbiosDataSize, SmbiosData, TempBuff);
                        break;
                    case 0x18:
                        GetOffsetVaries(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, \
                                        Type01[OffsetIndex], sizeof(UINT8), &SmbiosDataSize, SmbiosData, TempBuff);
                        break;
                    default:	//4, 5, 6, 7, 19, 1A
                        OffsetString = TempBuff[Type01[OffsetIndex]];
                        break;
                    }

                    if(OffsetString) {
                        GetOffsetString(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, Type01[OffsetIndex], \
                                        OffsetString, structLength, TempBuffSize, &SmbiosDataSize, SmbiosData, TempBuff);
                    }
                }
                break;
            case 2:	//EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION
                for(OffsetIndex = 0; OffsetIndex < sizeof(Type02); OffsetIndex++) {
                    OffsetString = 0;
                    switch(Type02[OffsetIndex]) {
                    case 0x09:
                    case 0x0D:
                    case 0x0E:
                        GetOffsetVaries(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, \
                                        Type02[OffsetIndex], sizeof(UINT8), &SmbiosDataSize, SmbiosData, TempBuff);
                        break;
                    case 0x0B:
                        GetOffsetVaries(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, \
                                        Type02[OffsetIndex], sizeof(UINT16), &SmbiosDataSize, SmbiosData, TempBuff);
                        break;
                    default:	//4, 5, 6, 7, 8, A,
                        OffsetString = TempBuff[Type02[OffsetIndex]];
                        break;
                    }

                    if(OffsetString) {
                        GetOffsetString(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, Type02[OffsetIndex], \
                                        OffsetString, structLength, TempBuffSize, &SmbiosDataSize, SmbiosData, TempBuff);
                    }
                }
                break;
            case 3:	//EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE
                for(OffsetIndex = 0; OffsetIndex < sizeof(Type03); OffsetIndex++) {
                    OffsetString = 0;
                    switch(Type03[OffsetIndex]) {
                    case 0x05:	//chassis type
                    case 0x09:
                    case 0x0A:
                    case 0x0B:
                    case 0x0C:
                    case 0x11:
                    case 0x12:
                    case 0x13:
                    case 0x14:
                    case 0x15:
                        GetOffsetVaries(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, \
                                        Type03[OffsetIndex], sizeof(UINT8), &SmbiosDataSize, SmbiosData, TempBuff);
                        break;
                    case 0x0D:
                        GetOffsetVaries(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, \
                                        Type03[OffsetIndex], sizeof(UINT32), &SmbiosDataSize, SmbiosData, TempBuff);
                        break;
                    default:	//4, 6, 7, 8,
                        OffsetString = TempBuff[Type03[OffsetIndex]];
                        break;
                    }

                    if(OffsetString) {
                        GetOffsetString(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, Type03[OffsetIndex], \
                                        OffsetString, structLength, TempBuffSize, &SmbiosDataSize, SmbiosData, TempBuff);
                    }
                }
                break;
            case 11:	//EFI_SMBIOS_TYPE_OEM_STRINGS
                for(OffsetIndex = 0; OffsetIndex < sizeof(Type11); OffsetIndex++) {
                    OffsetString = 0;
                    OffsetString = Type11[OffsetIndex];

                    if(OffsetString) {
                        GetOffsetString(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, Type11[OffsetIndex], \
                                        OffsetString, structLength, TempBuffSize, &SmbiosDataSize, SmbiosData, TempBuff);
                    }
                }
                break;
            case 39:	//EFI_SMBIOS_TYPE_SYSTEM_POWER_SUPPLY
                for(OffsetIndex = 0; OffsetIndex < sizeof(Type39); OffsetIndex++) {
                    OffsetString = 0;
                    switch(Type39[OffsetIndex]) {
                    case 0x04:
                        GetOffsetVaries(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, \
                                        Type39[OffsetIndex], sizeof(UINT8), &SmbiosDataSize, SmbiosData, TempBuff);
                        break;
                    case 0x0C:
                    case 0x0E:
                    case 0x10:
                    case 0x12:
                    case 0x14:
                        GetOffsetVaries(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, \
                                        Type39[OffsetIndex], sizeof(UINT16), &SmbiosDataSize, SmbiosData, TempBuff);
                        break;
                    default:	//5, 6, 7, 8, 9, A, B
                        OffsetString = TempBuff[Type39[OffsetIndex]];
                        break;
                    }

                    if(OffsetString) {
                        GetOffsetString(TypeNumber[TypeNumIndex], ((DMIHeader*)TempBuff)->structHandle, Type39[OffsetIndex], \
                                        OffsetString, structLength, TempBuffSize, &SmbiosDataSize, SmbiosData, TempBuff);
                    }
                }
                break;
            }
        }
    }
    gBS->FreePool(TempBuff);

    if (SmbiosDataSize == sizeof(InBand_Smbios_Data_Header)) {
        DEBUG((-1, "No SMBIOS data is combined.\n"));
        gBS->FreePool(SmbiosData);
        return;
    }

// Step 2. Calculate checksum of the buffer.
    for(TempBuffSize = sizeof(InBand_Smbios_Data_Header); TempBuffSize < SmbiosDataSize; TempBuffSize++)
        checksum += SmbiosData[TempBuffSize];

// Step 3. combine header
    gBS->SetMem(SmbiosData, sizeof(InBand_Smbios_Data_Header), 0x00);
    CopyMem(((InBand_Smbios_Data_Header*)SmbiosData)->Signature, "_SMB", 4);
    ((InBand_Smbios_Data_Header*)SmbiosData)->Version = mSmcOobPlatformPolicyPtr->OobConfigurations.SmbiosVersion;
    ((InBand_Smbios_Data_Header*)SmbiosData)->FileSize = SmbiosDataSize - sizeof(InBand_Smbios_Data_Header);
    ((InBand_Smbios_Data_Header*)SmbiosData)->Checksum = checksum;

    if(SmbiosDataSize > mSmcRomHoleProtocol->SmcRomHoleSize(CURRENT_SMBIOS_REGION)) {
        DEBUG((-1, "CombineSmbios FileSize over temp buffer.\n"));
        gBS->FreePool(SmbiosData);
        return;
    }

// Stpe 4. Save the header into flash part SMBIOS area(current, previous).
// empty tempbuffer and save to ROM
    DEBUG((-1, "CombineSmbios save to ROM.\n"));

//save previous smbios table.
//move old current smbios table to be previous smbios table.
        mSmcRomHoleProtocol->SmcRomHoleWriteRegion(PREVIOUS_SMBIOS_REGION, SmbiosData);

//save current smbios table.
    mSmcRomHoleProtocol->SmcRomHoleWriteRegion(CURRENT_SMBIOS_REGION, SmbiosData);
    gBS->FreePool(SmbiosData);

    DEBUG((-1, "CombineSmbios end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  UpdateToNVRAM
//
// Description:
//  Save changed smbios date to NVRAM. refer AMI standard method.
//
// Input:
//      IN	UINT8	TypeNumber - Type Number
//	IN	UINT16	HandleNum - Type Handle Number
//	IN	UINT8	TypeOffset - Type Offset
//	IN	UINT8	*Data - Changed Data string or Veries
//	IN	UINT8	DataSize - Changed Data Size, DataSize is 0 when Data is string.
//
// Output:
//	None.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID UpdateToNVRAM(
    IN	UINT8	TypeNumber,
    IN	UINT16	HandleNum,
    IN	UINT8	TypeOffset,
    IN	UINT8	*Data,
    IN	UINT8	DataSize
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    CHAR16	*DmiArrayVar = L"DmiArray", *S1 = L"                ";
    DMI_VAR	DmiArray[DMI_ARRAY_COUNT] = {0};
    UINTN	DmiArraySize = DMI_ARRAY_COUNT * sizeof(DMI_VAR);
    CHAR8	Buffer[0x400];
    UINT8	Index;
    UINTN	BufferSize = sizeof(Buffer);

    DEBUG((-1, "UpdateToNVRAM entry.\n"));
//if load default string.
    if((!DataSize) && (Data[0] == 0xFE)) {
        DEBUG((-1, "UpdateToNVRAM load default string.\n"));
        Swprintf(S1, L"DmiVar%02x%04x%02x%02x",
                 TypeNumber,
                 HandleNum,
                 TypeOffset,
                 0);

        Status = gRT->GetVariable(
                     S1,
                     &gAmiSmbiosNvramGuid,
                     NULL,
                     &BufferSize,
                     &Buffer);

        if(Status == EFI_NOT_FOUND)	return;
        DEBUG((-1, "del %s variable.\n", S1));
        Status = gRT->SetVariable(
                     S1,
                     &gAmiSmbiosNvramGuid,
                     EFI_VARIABLE_RUNTIME_ACCESS |\
                     EFI_VARIABLE_BOOTSERVICE_ACCESS |\
                     EFI_VARIABLE_NON_VOLATILE,
                     DataSize,
                     Data);
        DEBUG((-1, "del variable Status = %r.\n", Status));
        Status = gRT->GetVariable(
                     DmiArrayVar,
                     &gAmiSmbiosNvramGuid,
                     NULL,
                     &DmiArraySize,
                     &DmiArray);

        for(Index = 1; Index <= DmiArray[0].Type; Index++) {
            if((DmiArray[Index].Type == TypeNumber) &&
                    (DmiArray[Index].Handle == HandleNum) &&
                    (DmiArray[Index].Offset == TypeOffset))
                break;
        }

        for(Index; Index <= DmiArray[0].Type; Index++) {
            DmiArray[Index].Type = DmiArray[Index + 1].Type;
            DmiArray[Index].Handle = DmiArray[Index + 1].Handle;
            DmiArray[Index].Offset = DmiArray[Index + 1].Offset;
            DmiArray[Index].Flags = DmiArray[Index + 1].Flags;
        }

        DmiArray[0].Type--;
        DEBUG((-1, "DmiArraySize = %x.\n", DmiArraySize));
        Status = gRT->SetVariable(
                     DmiArrayVar,
                     &gAmiSmbiosNvramGuid,
                     EFI_VARIABLE_RUNTIME_ACCESS | \
                     EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                     EFI_VARIABLE_NON_VOLATILE,
                     DmiArraySize,
                     &DmiArray);
        DEBUG((-1, "del DmiArrayVar Status = %r.\n", Status));
        return;
    }
//if update user string
    DEBUG((-1, "UpdateToNVRAM user string.\n"));
    Status = gRT->GetVariable(
                 DmiArrayVar,
                 &gAmiSmbiosNvramGuid,
                 NULL,
                 &DmiArraySize,
                 &DmiArray);
    DEBUG((-1, "GetVariable - DmiArray Status = %r\n", Status));

    if(Status == EFI_SUCCESS) {
        Index = DmiArray[0].Type;
        ++Index;
    }
    else {
        Index = 1;
    }

    Swprintf(S1, L"DmiVar%02x%04x%02x%02x",
             TypeNumber,
             HandleNum,
             TypeOffset,
             0);

    DmiArray[Index].Type = TypeNumber;
    DmiArray[Index].Handle = HandleNum;
    DmiArray[Index].Offset = TypeOffset;
    DmiArray[Index].Flags = 0;

    Status = gRT->GetVariable(
                 S1,
                 &gAmiSmbiosNvramGuid,
                 NULL,
                 &BufferSize,
                 &Buffer);
    DEBUG((-1, "GetVariable - %s Status = %r\n", S1, Status));

    if(Status == EFI_NOT_FOUND) {
        // Record not present, increment record count
        DmiArray[0].Type += 1;

        Status = gRT->SetVariable(
                     DmiArrayVar,
                     &gAmiSmbiosNvramGuid,
                     EFI_VARIABLE_RUNTIME_ACCESS | \
                     EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                     EFI_VARIABLE_NON_VOLATILE,
                     DmiArraySize,
                     &DmiArray);
        DEBUG((-1, "SetVariable - DmiArray Status = %r\n", Status));
    }

    if(Status == EFI_BUFFER_TOO_SMALL || Status == EFI_SUCCESS) {
        if(!DataSize)	//if update is string
            DataSize = (UINT8)Strlen(Data) + 1;	//add a 0x00 for string endding

        Status = gRT->SetVariable(
                     S1,
                     &gAmiSmbiosNvramGuid,
                     EFI_VARIABLE_RUNTIME_ACCESS |\
                     EFI_VARIABLE_BOOTSERVICE_ACCESS |\
                     EFI_VARIABLE_NON_VOLATILE,
                     DataSize,
                     Data);
        DEBUG((-1, "SetVariable - %s Status = %r\n", S1, Status));
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  UpdateOffsetData
//
// Description:
//  Update SmbiosData from by user to system.
//  Parsing SmbiosData and comparing current SmbiosData with previous SmbiosData.
//  If comparing result is different, update the data to system.
//  If data is string and data is 0xFE, load default string, else change string.
//
//  SmbiosData format defined by SMC:
//
//  Offset        Size          Description
//  =======================================
//  0             1             TypeNumber
//  1             2             TypeHandle
//  3             1             OffsetIndex
//  4             String Length string data and a endding char 0x00
//
// Input:
//	IN	UINT8	DataSize - Data size, 0->String
//	IN	UINT8	*SmbiosData - current smbios table in ROM
//	IN	UINT8	*PreSmbiosData - previous smbios table in ROM
//
// Output:
//	IN	OUT	UINT16	*SmbiosDataIndex - current smbios table index
//	IN	OUT	UINT16	*PreSmbiosDataIndex - previous smbios table index
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID UpdateOffsetData(
    IN	UINT8	DataSize,			//varies size
    IN	UINT8	*SmbiosData,			//current smbios table
    IN	UINT8	*PreSmbiosData,			//previous smbios table
    IN	OUT	UINT32	*SmbiosDataIndex,	//current smbios data index
    IN	OUT	UINT32	*PreSmbiosDataIndex	//previous smbios data index
)
{
    UINT8	TypeNumber, TypeOffset;
    UINT16	HandleNum;
    CHAR8	Data[0x400] = {0};

// point to data
    TypeNumber = SmbiosData[*SmbiosDataIndex];
    HandleNum = *(UINT16*)&SmbiosData[*SmbiosDataIndex + 1];
    TypeOffset = SmbiosData[*SmbiosDataIndex + 3];
    (*SmbiosDataIndex) += 4;	// type + handle + offset = 4 bytes, point to data
    (*PreSmbiosDataIndex) += 4;

    if(DataSize) {	//data is varies
        DEBUG((-1, "UpdateOffsetData Varies.\n"));
        if(CompareMem(&PreSmbiosData[*PreSmbiosDataIndex], &SmbiosData[*SmbiosDataIndex], DataSize)) {
            DEBUG((-1, "UpdateOffsetData Varies updated.\n"));
            CopyMem(Data, &SmbiosData[*SmbiosDataIndex], DataSize);
#if DEBUG_MODE == 1
            {
                UINT32 ii;
                DEBUG((-1, "    "));
                for (ii = 0; ii < DataSize; ii++) {
                    DEBUG((-1, "%x ", Data[ii]));
                }
                DEBUG((-1, "\n"));
            }
#endif // #if DEBUG_MODE == 1
            UpdateToNVRAM(TypeNumber, HandleNum, TypeOffset, Data, DataSize);
        }
        (*SmbiosDataIndex) += DataSize;
        (*PreSmbiosDataIndex) += DataSize;
    }
    else {		//data is string
        DEBUG((-1, "UpdateOffsetData String.\n"));
        if((TypeNumber == 11) || (TypeNumber == 12))
            --TypeOffset;
        if(Strcmp(&PreSmbiosData[*PreSmbiosDataIndex], &SmbiosData[*SmbiosDataIndex])) {
            DEBUG((-1, "UpdateOffsetData string updated.\n"));
            Strcpy(Data, &SmbiosData[*SmbiosDataIndex]);
#if DEBUG_MODE == 1
            {
                UINT16 StrBuffW[64];
                AsciiStrToUnicodeStr (Data, StrBuffW);
                DEBUG((-1, "    %s\n", StrBuffW));
            }
#endif // #if DEBUG_MODE == 1
            UpdateToNVRAM(TypeNumber, HandleNum, TypeOffset, Data, DataSize);
        }
// point to next Smbios data
        (*SmbiosDataIndex) += (UINT32)Strlen(&SmbiosData[*SmbiosDataIndex]);
        (*PreSmbiosDataIndex) += (UINT32)Strlen(&PreSmbiosData[*PreSmbiosDataIndex]);
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  InBandUpdateSmbios
//
// Description:
//  Read SmBios data(default, previous and current) from ROM and update changed data to NVRAM.
//
// Steps:
//   1. Read SmbiosDate(default, previous and current) from ROM.
//   2. Determine what datas must update.
//	If there is no "_SMB" in SmbiosData header -> wrong data. return.
//	If "Source" is not 1 -> the data is not uploaded by user. return.
//	If data checksum is not the same -> data transfer error. return.
//   3. Loop whole current SmbiosData from user.
//   4. Clear "Source" bit
//
// Input:
//      None.
//
// Output:
//      None.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
InBandUpdateSmbios(void)
{
    UINT8	*PreSmbiosData, *SmbiosData;
    UINT32	PreSmbiosDataIndex, SmbiosDataIndex;
    UINT16	HandleNum;
    UINT8	TypeNumber, TypeOffset, checksum = 0, DataSize;
    InBand_Smbios_Data_Header	*InBandSmbiosHeader;

    DEBUG((-1, "InBandUpdateSmbios entry.\n"));

    gBS->AllocatePool(EfiBootServicesData, mSmcRomHoleProtocol->SmcRomHoleSize(CURRENT_SMBIOS_REGION), &SmbiosData);
    gBS->AllocatePool(EfiBootServicesData, mSmcRomHoleProtocol->SmcRomHoleSize(PREVIOUS_SMBIOS_REGION), &PreSmbiosData);

// Step 1. Read SmbiosDate from ROM.
// get previous smbios table from ROM
    mSmcRomHoleProtocol->SmcRomHoleReadRegion(PREVIOUS_SMBIOS_REGION, PreSmbiosData);
// get current smbios table from ROM
    mSmcRomHoleProtocol->SmcRomHoleReadRegion(CURRENT_SMBIOS_REGION, SmbiosData);

    InBandSmbiosHeader = (InBand_Smbios_Data_Header*)SmbiosData;

    //
    // Check "_SMB" in SmbiosData heade.
    //
    if (MemCmp (InBandSmbiosHeader->Signature, "_SMB", 4) != 0) {
        DEBUG((-1, "InBandUpdateSmbios, No SMBIOS data. Don't need to update Smbios.\n"));
        gBS->FreePool(SmbiosData);
        gBS->FreePool(PreSmbiosData);
        return;
    }

    //
    // Check Source bit.
    //
    if (!(InBandSmbiosHeader->Source)) {
        DEBUG((-1, "InBandUpdateSmbios, Source bit is 0. Don't need to update Smbios.\n"));
        gBS->FreePool(SmbiosData);
        gBS->FreePool(PreSmbiosData);
        return;
    }

    //
    // Calculate data checksum
    //
    for(SmbiosDataIndex = 0; SmbiosDataIndex < InBandSmbiosHeader->FileSize; SmbiosDataIndex++)
        checksum += SmbiosData[SmbiosDataIndex + sizeof(InBand_Smbios_Data_Header)];

    //
    // Check checksum.
    //
    if (checksum != InBandSmbiosHeader->Checksum) {
        DEBUG((-1, "ERROR : Inband SMBIOS checksum error.\n"));
        gBS->FreePool(SmbiosData);
        gBS->FreePool(PreSmbiosData);
        return;
    }

    SmbiosDataIndex = sizeof(InBand_Smbios_Data_Header);
    PreSmbiosDataIndex = sizeof(InBand_Smbios_Data_Header);
// Step 3. Loop whole current SmbiosData from user.
    while(SmbiosDataIndex < (InBandSmbiosHeader->FileSize + sizeof(InBand_Smbios_Data_Header))) {
        TypeNumber = SmbiosData[SmbiosDataIndex];
        HandleNum = *(UINT16*)&SmbiosData[SmbiosDataIndex + 1];
        TypeOffset = SmbiosData[SmbiosDataIndex + 3];
        DataSize = 0;

        DEBUG((-1, "InBandUpdateSmbios Type = 0x%x, Handle = 0x%x, Offset = 0x%x\n", TypeNumber, HandleNum, TypeOffset));
        switch(TypeNumber) {
        case 0:
            switch(TypeOffset) {
            case 0x06:
            case 0x12:
                DataSize = sizeof(UINT16);
                break;
            case 0x09:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
                DataSize = sizeof(UINT8);
                break;
            case 0x0A:
                DataSize = sizeof(UINT64);
                break;
            default:
                break;
            }
            break;
        case 1:
            switch(TypeOffset) {
            case 0x08:	//UUID[16]
                DataSize = 16;
                break;
            case 0x18:
                DataSize = sizeof(UINT8);
                break;
            default:	//4, 5, 6, 7, 19, 1A
                break;
            }
            break;
        case 2:
            switch(TypeOffset) {
            case 0x09:
            case 0x0D:
            case 0x0E:
                DataSize = sizeof(UINT8);
                break;
            case 0x0B:
                DataSize = sizeof(UINT16);
                break;
            default:	//4, 5, 6, 7, 8, A,
                break;
            }
            break;
        case 3:
            switch(TypeOffset) {
            case 0x05:	//chassis type
            case 0x09:
            case 0x0A:
            case 0x0B:
            case 0x0C:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
                DataSize = sizeof(UINT8);
                break;
            case 0x0D:
                DataSize = sizeof(UINT32);
                break;
            default:	//4, 6, 7, 8,
                break;
            }
            break;
        case 11:
            break;
        case 39:
            switch(TypeOffset) {
            case 0x04:
                DataSize = sizeof(UINT8);
                break;
            case 0x0C:
            case 0x0E:
            case 0x10:
            case 0x12:
            case 0x14:
                DataSize = sizeof(UINT16);
                break;
            default:	//5, 6, 7, 8, 9, A, B
                break;
            }
            break;
        }
        UpdateOffsetData(DataSize, SmbiosData, PreSmbiosData, \
                         &SmbiosDataIndex, &PreSmbiosDataIndex);

        SmbiosDataIndex++;	//point to next type and offset in Smbios
        PreSmbiosDataIndex++;
    }

// Step 4.clear "Source" bit
    InBandSmbiosHeader->Source = 0;

    mSmcRomHoleProtocol->SmcRomHoleWriteRegion(CURRENT_SMBIOS_REGION, SmbiosData);

    gBS->FreePool(SmbiosData);
    gBS->FreePool(PreSmbiosData);

    IoWrite8(0xcf9, 0x06);	// reset system

    DEBUG((-1, "InBandUpdateSmbios end.\n"));
}
