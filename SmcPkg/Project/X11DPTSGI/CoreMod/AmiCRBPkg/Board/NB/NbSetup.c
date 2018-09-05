//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: $
//
// $Revision:  $
//
// $Date:  $
//*************************************************************************

//<AMI_FHDR_START>
//-----------------------------------------------------------------------
//
// Name:        NbSetup.c
//
// Description: This C file contains code related to NB setup routines
//
//-----------------------------------------------------------------------
//<AMI_FHDR_END>

#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <Guid/MemoryMapData.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PchInfoLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciBaseLib.h>
#include <Protocol/PlatformType.h>
//#include <Protocol/IioUds.h>
#include <Cpu/CpuRegs.h>

#define PCI_DEVICE_ID_OFFSET                        0x02

/**

  Converts an ascii string to unicode string 16 chars at a time.

  @param AsciiString    -  Address of Ascii string.
  @param UnicodeString  -  Address of Unicode String.

  @retval None.

**/
VOID
AsciiToUnicode (
  IN CHAR8     *AsciiString,
  IN CHAR16    *UnicodeString
  )
{

    UINT8 Index;

    Index = 0;
    while (AsciiString[Index] != 0) {
        UnicodeString[Index] = (CHAR16) AsciiString[Index];
        Index++;
    }
    UnicodeString[Index] = 0;
}

/**
    Display Board information.

    @param HiiHandle - Handle number

    @return VOID

**/

VOID
InitBoardInfoStrings (
  IN EFI_HII_HANDLE  HiiHandle
)
{
    EFI_STATUS                   Status;
    EFI_PLATFORM_TYPE_PROTOCOL   *PlatformType;
    UINT32                       RegEax;
    CHAR8                        Buffer1[32];
    CHAR16                       UniBuffer1[32];
    CHAR8                        Buffer2[32];
    CHAR16                       UniBuffer2[32];
    UINT32                       BufferSize;
    UINTN                        LpcBaseAddress;

    Status = gBS->LocateProtocol (
                    &gEfiPlatformTypeProtocolGuid,
                    NULL,
                    &PlatformType
                    );
    if (EFI_ERROR(Status)) {
        return;
    }

    //
    // CPU ID Information
    //
    AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);

    //
    // Update Platform type
    //
    InitString(
        HiiHandle,
        STRING_TOKEN(STR_PLATFORM_INFO_VALUE),
        L"%s",
        (CHAR16 *)PlatformType->TypeStringPtr
        );

    LpcBaseAddress = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_LPC,
                       PCI_FUNCTION_NUMBER_PCH_LPC
                       );
 
    BufferSize = sizeof (Buffer1);
    PchGetSteppingStr (PchStepping (), Buffer1, &BufferSize);
    AsciiToUnicode (Buffer1, UniBuffer1);

    BufferSize = sizeof (Buffer2);
    PchGetSkuStr (MmioRead16 (LpcBaseAddress + PCI_DEVICE_ID_OFFSET), Buffer2, &BufferSize);
    AsciiToUnicode (Buffer2, UniBuffer2);

    //
    // Update PCH type value
    //
    InitString(
            HiiHandle,
            STRING_TOKEN(STR_PCH_INFO_VALUE),
            L"%s - %s",
            UniBuffer2,
            UniBuffer1
            );

    //
    // Update skylake CPU information
    //
    if (PlatformType->CpuType == CPU_FAMILY_SKX) {
        switch (PlatformType->CpuStepping) {

            case (A0_CPU_STEP):
                InitString(
                    HiiHandle,
                    STRING_TOKEN(STR_PROCESSOR_INFO_VALUE),
                    L"%5X - SKX A0",
                    RegEax
                    );
            break;
            case (A2_CPU_STEP):
                InitString(
                    HiiHandle,
                    STRING_TOKEN(STR_PROCESSOR_INFO_VALUE),
                    L"%5X - SKX A2",
                    RegEax
                    );
            break;
            case (B0_CPU_STEP):
                InitString(
                    HiiHandle,
                    STRING_TOKEN(STR_PROCESSOR_INFO_VALUE),
                    L"%5X - SKX B0",
                    RegEax
                    );
            break;
            case (B1_CPU_STEP):
                InitString(
                    HiiHandle,
                    STRING_TOKEN(STR_PROCESSOR_INFO_VALUE),
                    L"%5X - SKX B1",
                    RegEax
                    );
            break;
            case (H0_CPU_STEP):
                InitString(
                    HiiHandle,
                    STRING_TOKEN(STR_PROCESSOR_INFO_VALUE),
                    L"%5X - SKX H0",
                    RegEax
                    );
            break;
            default:
                InitString(
                    HiiHandle,
                    STRING_TOKEN(STR_PROCESSOR_INFO_VALUE),
                    L"%5X - SKX Unknown",
                    RegEax
                    );

        }
    }
    return;
}

/**
    Display Reference code revision information. 

    @param HiiHandle - Handle number
    @param Class     - Form Class number

    @return VOID

**/

VOID
InitRefCodeVersionStrings (
  IN EFI_HII_HANDLE      HiiHandle
)
{

//    EFI_STATUS                     Status;
//    EFI_IIO_UDS_PROTOCOL           *IioUds;

/*    Status = gBS->LocateProtocol (
                    &gEfiIioUdsProtocolGuid,
                    NULL,
                    &IioUds
                    );
    if (EFI_ERROR(Status)) {
        return;
    }
*/
    InitString(
            HiiHandle,
            STRING_TOKEN(STR_REFCODE_REVISION_VALUE),
            RC_REVISION_VALUE // Hard coding SDL value need to check if any alternate available
/*            (IioUds->IioUdsPtr->SystemStatus.RcVersion & 0xFF000000) >> 24,
            (IioUds->IioUdsPtr->SystemStatus.RcVersion & 0xFF0000) >> 16,
            (IioUds->IioUdsPtr->SystemStatus.RcVersion & 0xFF00) >> 8*/
    );

    return;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   InitNbStrings
//
// Description: Initializes North Bridge Setup String
//
// Input:       HiiHandle - Handle to HII database
//              Class - Indicates the setup class
//
// Output:      None
//
// Notes:       PORTING REQUIRED
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
InitNbStrings(
    IN EFI_HII_HANDLE      HiiHandle,
    IN UINT16              Class
)
{
    EFI_HOB_GUID_TYPE               *GuidHob;
    struct SystemMemoryMapHob       *SystemMemoryMap;
    UINT32                          MemorySize;
//SGI+    
    UINT32                          MemorySpeed;
//SGI-    
    if(Class == MAIN_FORM_SET_CLASS) {

        GuidHob = GetFirstGuidHob (&gEfiMemoryMapGuid);
        if (GuidHob == NULL) {
            return;
        }

        SystemMemoryMap = (struct SystemMemoryMapHob*) GET_GUID_HOB_DATA (GuidHob);
        MemorySize = SystemMemoryMap->memSize << 6;  //mamory size in 1MB units

            InitString(
                    HiiHandle,
                    STRING_TOKEN(STR_MEMORY_SIZE_VALUE),
                    L"%4d MB",
                    MemorySize
            );

      //SGI+
        MemorySpeed = SystemMemoryMap->memFreq; 
            InitString(
                    HiiHandle,
                    STRING_TOKEN(STR_MEMORY_SPEED_VALUE),
                    L"%4d MT/s",
                    MemorySpeed
            );        
      //SGI-
            InitBoardInfoStrings (HiiHandle);

            InitRefCodeVersionStrings (HiiHandle);

        }
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
