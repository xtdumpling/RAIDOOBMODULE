//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
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
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  PlatformPeiSystemInformation.h
//
// Description:	Definitions used by PlatformPeiSystemInformation.
//
//<AMI_FHDR_END>
//**********************************************************************

#include <AmiLib.h>
#include <timestamp.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/EndOfPeiPhase.h>

#include <PPI/AmiPeiTextOut.h>
#include <UncoreCommonIncludes.h>

#include "Token.h"
#include <SysHostChip.h>

typedef struct {
  UINT16        Value;
  CHAR8         *String;
} VALUE_TO_STRING_MAPPING;

static VALUE_TO_STRING_MAPPING gFamilyIdNameMapping[] = {
{0x06, "Intel Nehalem Family"}
};

static VALUE_TO_STRING_MAPPING gModelIdNameMapping[] = {
		{0x55, "Skylake Server Model"},
		{0x4E, "Skylake Client Model"},
		{0x3F, "Haswell Server Model"},
		{0x3C, "Haswell Client Model"},
		{0x3E, "IvyBridge Server Model"},
		{0x3A, "IvyBridge Client Model"},
		{0x2D, "SandyBridge Server Model"},
		{0x2A, "SandyBridge Client Model"},
		{0x2F, "WestMere EX Model"},
		{0x2E, "Nehalem EX Model"},
};

static VALUE_TO_STRING_MAPPING gSteppingIdNameMapping[] = {
		{A0_REV_SKX, "A0/A1"},
		{A2_REV_SKX, "A2"},
		{B0_REV_SKX, "B0"},
		{C0_REV_SKX, "C0"}
};

static VALUE_TO_STRING_MAPPING gDimmMfgIdNameMapping[] = {
		{0x5705, "AENEON"},
		{0x5105, "Qimonda"},
		{0x1000, "NEC"},
		{0xB300, "IDT"},
		{0x9700, "Texas Instruments"},
		{0xAD00, "SK Hynix"},
		{0x2C00, "Micron Technology"},
		{0xC100, "Infineon (Siemens)"},
		{0xCE00, "Samsung"},
		{0x3D00, "Tektronix"},
		{0x9801, "Kingston"},
		{0xFE02, "Elpida"},
		{0x9401, "Smart Modular"},
		{0xC802, "Agilent Technologies"},
		{0x0B03, "Nanya Technology"},
		{0xB304, "Inphi Corporation"},
		{0x3206, "Montage Technology Group"}
};


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
