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
// $Header:  $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:                PlatformPeiSystemInfoCPU.c
//
// Description:	  		Gathers CPU Information
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Library/PeiServicesLib.h>
#include "PlatformPeiSystemInformation.h"
#include "Pei.h"

#include <CpuBaseLib.h>
#include <Library/BaseLib.h>
#include <Include/Setup/IioUniversalData.h>
#include <Library/HobLib.h>

#include <Library/CpuPpmLib.h>

#define CORE_COUNT_OFFSET           (16)

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//  Procedure:   GetCpuVendor
//
//  Description: Gets Vendor name by reading the processor CPUID
//
//  Input:       ProcessorNumber
//
//  Output:      VendorName
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
CHAR8 *
GetCpuVendor (IN  CONST EFI_PEI_SERVICES        **PeiServices ,
    UINT8                                   ProcessorNumber
)
{
    EFI_CPUID_REGISTER		*CpuidRegisters;
    CHAR8			*VendorName;
    CHAR8			*Pos;
    EFI_STATUS			Status=EFI_SUCCESS;

    Status = (*PeiServices)->AllocatePool(PeiServices, 13, &VendorName);
    Pos = VendorName;
    //
    // Get CPU Vendor Name
    //
    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof(EFI_CPUID_REGISTER), &CpuidRegisters);
    AsmCpuid(0, &CpuidRegisters->RegEax, &CpuidRegisters->RegEbx, &CpuidRegisters->RegEcx, &CpuidRegisters->RegEdx);

    *(UINT32*)Pos = CpuidRegisters->RegEbx; Pos += 4;
    *(UINT32*)Pos = CpuidRegisters->RegEdx; Pos += 4;
    *(UINT32*)Pos = CpuidRegisters->RegEcx; Pos += 4;
    *Pos = '\0';

    return VendorName;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//  Procedure:   GetBrandString
//
//  Description:
//
//  Input:
//
//  Output:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
CHAR8 *
GetBrandString (IN  CONST EFI_PEI_SERVICES        **PeiServices ,
    UINT8                                   ProcessorNumber
)
{
    EFI_CPUID_REGISTER		*CpuidRegisters;
    CHAR8			*BrandString;
    CHAR8			*Pos;
    EFI_STATUS			Status=EFI_SUCCESS;
       
    Status = (*PeiServices)->AllocatePool(PeiServices, 49, &BrandString);
    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof(EFI_CPUID_REGISTER), &CpuidRegisters);
    Pos = BrandString;
    
    AsmCpuid(EFI_CPUID_EXTENDED_FUNCTION, &CpuidRegisters->RegEax, &CpuidRegisters->RegEbx, &CpuidRegisters->RegEcx, &CpuidRegisters->RegEdx);
    
    //
    // Get Brand string
    //
    AsmCpuid(EFI_CPUID_BRAND_STRING1, &CpuidRegisters->RegEax, &CpuidRegisters->RegEbx, &CpuidRegisters->RegEcx, &CpuidRegisters->RegEdx);
    *(UINT32*)Pos = CpuidRegisters->RegEax; Pos += 4;
    *(UINT32*)Pos = CpuidRegisters->RegEbx; Pos += 4;
    *(UINT32*)Pos = CpuidRegisters->RegEcx; Pos += 4;
    *(UINT32*)Pos = CpuidRegisters->RegEdx; Pos += 4;
       
    AsmCpuid(EFI_CPUID_BRAND_STRING2, &CpuidRegisters->RegEax, &CpuidRegisters->RegEbx, &CpuidRegisters->RegEcx, &CpuidRegisters->RegEdx);
    *(UINT32*)Pos = CpuidRegisters->RegEax; Pos += 4;
    *(UINT32*)Pos = CpuidRegisters->RegEbx; Pos += 4;
    *(UINT32*)Pos = CpuidRegisters->RegEcx; Pos += 4;
    *(UINT32*)Pos = CpuidRegisters->RegEdx; Pos += 4;

    AsmCpuid(EFI_CPUID_BRAND_STRING3, &CpuidRegisters->RegEax, &CpuidRegisters->RegEbx, &CpuidRegisters->RegEcx, &CpuidRegisters->RegEdx);
    *(UINT32*)Pos = CpuidRegisters->RegEax; Pos += 4;
    *(UINT32*)Pos = CpuidRegisters->RegEbx; Pos += 4;
    *(UINT32*)Pos = CpuidRegisters->RegEcx; Pos += 4;
    *(UINT32*)Pos = CpuidRegisters->RegEdx; Pos += 4;
    
    *Pos = '\0';
 
    //
    // Remove Initial spaces.
    //
    Pos = BrandString;
    while (*Pos == ' ' && *Pos !=0) ++Pos;
    if (Pos != BrandString) {
        INT32  NumToCopy = 49 - (UINT32)(Pos - BrandString);
        CHAR8 *Pos2 = BrandString;
        while (NumToCopy--) *Pos2++ = *Pos++;
    }

    return BrandString;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//  Procedure:   GetSupportedCoreThreadCounts
//
//  Description: This function gets the version information of processor,
//		including family ID, model ID, stepping ID and processor type.
//
//  Input:       None
//
//  Output:      family ID, model ID, stepping ID and processor type.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
EFIAPI
GetProcessorVersionInfo (IN  CONST EFI_PEI_SERVICES        **PeiServices ,
  OUT UINT32              *DisplayedFamily OPTIONAL,
  OUT UINT32              *DisplayedModel  OPTIONAL,       
  OUT UINT32              *SteppingId      OPTIONAL,
  OUT UINT32              *ProcessorType   OPTIONAL
  )
{
  EFI_CPUID_REGISTER      *VersionInfo;
  UINT32                  RegEax;
  UINT32                  FamilyId;
  UINT32                  ExtendedFamilyId;
  UINT32                  ExtendedModelId;

  //
  // Get CPUID(1).EAX
  //
  (*PeiServices)->AllocatePool(PeiServices, sizeof(EFI_CPUID_REGISTER), &VersionInfo);
  AsmCpuid(EFI_CPUID_VERSION_INFO, &VersionInfo->RegEax, &VersionInfo->RegEbx, &VersionInfo->RegEcx, &VersionInfo->RegEdx);
  RegEax      = VersionInfo->RegEax;

  //
  // Processor Type is CPUID(1).EAX[12:13]
  //
  if (ProcessorType != NULL) {
    *ProcessorType = BitFieldRead32 (RegEax, 12, 13);
  }

  //
  // Stepping ID is CPUID(1).EAX[0:3]
  //
  if (SteppingId != NULL) {
    *SteppingId = BitFieldRead32 (RegEax, 0, 3);
  }

  //
  // The Extended Family ID needs to be examined only when the Family ID is 0FH.
  // If Family ID is 0FH, Displayed Family ID = Family ID + Extended Family ID.
  // Otherwise, Displayed Family ID is Family ID
  //
  FamilyId = BitFieldRead32 (RegEax, 8, 11);
  if (DisplayedFamily != NULL) {
    *DisplayedFamily = FamilyId;
    if (FamilyId == 0x0f) {
      ExtendedFamilyId = BitFieldRead32 (RegEax, 20, 27);
      *DisplayedFamily += ExtendedFamilyId;
    }
  }

  //
  // The Extended Model ID needs to be examined only when the Family ID is 06H or 0FH.
  // If Family ID is 06H or 0FH, Displayed Model ID = Model ID + (Extended Model ID << 4).
  // Otherwise, Displayed Model ID is Model ID.
  //
  if (DisplayedModel != NULL) {
    *DisplayedModel = BitFieldRead32 (RegEax, 4, 7);
    if (FamilyId == 0x06 || FamilyId == 0x0f) {
      ExtendedModelId = BitFieldRead32 (RegEax, 16, 19);
      *DisplayedModel += (ExtendedModelId << 4);
    }
  }
}



//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//  Procedure:   InitializeCpuData
//
//  Description: Outputs CPU information to early video.
//
//  Input:       VOID
//
//  Output:      VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
InitializeCpuData (IN  CONST EFI_PEI_SERVICES        **PeiServices,
		AMI_PEI_TEXT_OUT	*This
)
{
    EFI_STATUS		Status=EFI_SUCCESS;
    CHAR8		*BrandString;
    CHAR8		*VendorName;
    UINT8		Index;
    UINT64		TurboLimit;

    CHAR8		Stepping[3];  
    CHAR8       	CpuFamilyName[30];
    CHAR8       	CpuModelIdName[30];
    MSR_REGISTER	CoreThreadCount;
    UINT8		CoreCount;
    
    IIO_UDS             *IioUds;
    EFI_GUID            UniversalDataGuid = IIO_UNIVERSAL_DATA_GUID; 
    EFI_HOB_GUID_TYPE   *GuidHob;
    
    UINT32		ProcessorNumber = 0;
    UINT32		FamilyId;
    UINT32		ModelId;
    UINT32		SteppingId;
    
    CHAR8                   TextString[160];
    
    GuidHob    = GetFirstGuidHob (&UniversalDataGuid);
    IioUds = GET_GUID_HOB_DATA (GuidHob);
    
    //
    // Fill the CPU details for the sockets populated
    //
    
    GetProcessorVersionInfo (PeiServices, &FamilyId, &ModelId, &SteppingId, NULL);
    
    // Fill FamilyID string based on value
    for (Index = 0; Index < sizeof(gFamilyIdNameMapping) / sizeof(VALUE_TO_STRING_MAPPING); ++Index) {
    	if(gFamilyIdNameMapping[Index].Value == FamilyId) {
    		AsciiSPrint(CpuFamilyName, sizeof (CpuFamilyName), gFamilyIdNameMapping[Index].String);
    		break;
    	}
    }
    
    // Fill ModelID string based on value
    for (Index = 0; Index < sizeof(gModelIdNameMapping) / sizeof(VALUE_TO_STRING_MAPPING); ++Index) {
    	if(gModelIdNameMapping[Index].Value == ModelId) {
    		AsciiSPrint(CpuModelIdName, sizeof (CpuModelIdName), gModelIdNameMapping[Index].String);
    		break;
    	}
    }
    
    // Fill SteppingID string based on value
    for (Index = 0; Index < sizeof(gSteppingIdNameMapping) / sizeof(VALUE_TO_STRING_MAPPING); ++Index) {
    	if (gSteppingIdNameMapping[Index].Value == SteppingId) {
    		AsciiSPrint(Stepping, sizeof (Stepping), gSteppingIdNameMapping[Index].String);
    		break;
    	}
    }
    
    // Fill BrandName String
    BrandString = GetBrandString(PeiServices, ProcessorNumber);
    
    // Fill CpuVendor String
    VendorName = GetCpuVendor(PeiServices, ProcessorNumber);
    
    // Read 0x1AD MSR bits 0-7. Its in units of 100Mhz which signifies the max Cpu frequency
    TurboLimit = 0xff & AsmReadMsr64(MSR_TURBO_RATIO_LIMIT);
    
    // Read the Core Count
    CoreThreadCount.Qword = AsmReadMsr64 (MSR_CORE_THREAD_COUNT);
    CoreCount = (UINT8) (CoreThreadCount.Dwords.Low >> CORE_COUNT_OFFSET);
    
    Status = This->SetAttribute(This, 0xF, 0, 0);
    
    AsciiSPrint (TextString, 160, "Intel RC Version: %x.%x.%x", 
		    (IioUds->SystemStatus.RcVersion & 0xFF000000) >> 24,
		    (IioUds->SystemStatus.RcVersion  & 0xFF0000) >> 16,
		    (IioUds->SystemStatus.RcVersion  & 0xFF00) >> 8);
    Status = This->WriteString(This, 0, TextString);
    
    AsciiSPrint (TextString, 160, " CPU Info:%a \n", BrandString);
    Status = This->WriteString(This, 0, TextString);
    AsciiSPrint (TextString, 160, "    Processors: %d Cores: %d Stepping: %a\n", 
		    IioUds->SystemStatus.numCpus,
		    CoreCount,
		    Stepping);  //Possibly set to 0 in Simics
    Status = This->WriteString(This, 0, TextString);
    Status = This->SetAttribute(This, 0x7, 0, 0);
    //o	CPU info : Intel (R) Xeon (R) CPU E5-XXXX v3 @  XXXGHz, , CPU count, Core count, Microcode version, CPU Stepping
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
