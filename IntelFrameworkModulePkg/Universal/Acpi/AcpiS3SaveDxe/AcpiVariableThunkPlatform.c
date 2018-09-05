/** @file
  This is an implementation of the AcpiVariable platform field for ECP platform.

Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions
of the BSD License which accompanies this distribution.  The
full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

==

typedef struct {
  EFI_PHYSICAL_ADDRESS  AcpiReservedMemoryBase;  <<===
  UINT32                AcpiReservedMemorySize;  <<===
  EFI_PHYSICAL_ADDRESS  S3ReservedLowMemoryBase;
  EFI_PHYSICAL_ADDRESS  AcpiBootScriptTable;
  EFI_PHYSICAL_ADDRESS  RuntimeScriptTableBase;
  EFI_PHYSICAL_ADDRESS  AcpiFacsTable;
  UINT64                SystemMemoryLength;      <<===
  ACPI_CPU_DATA_COMPATIBILITY         AcpiCpuData;
  EFI_PHYSICAL_ADDRESS  VideoOpromAddress;
  UINT32                VideoOpromSize;
  EFI_PHYSICAL_ADDRESS  S3DebugBufferAddress; 
  EFI_PHYSICAL_ADDRESS  S3ResumeNvsEntryPoint;    
} ACPI_VARIABLE_SET_COMPATIBILITY;

**/

#include <FrameworkDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
//*** AMI PORTING BEGIN ***//
//Commenting out not used functionality
//#include <Protocol/MpService.h>
//#include <Protocol/VariableLock.h>
//*** AMI PORTING END *****//
#include <Guid/AcpiVariableCompatibility.h>
#include <Guid/AcpiS3Context.h>

///
/// ACPI CPU Data Structure
///
typedef struct {
  BOOLEAN               APState;
  BOOLEAN               S3BootPath;
  EFI_PHYSICAL_ADDRESS  WakeUpBuffer;
  EFI_PHYSICAL_ADDRESS  GdtrProfile;
  EFI_PHYSICAL_ADDRESS  IdtrProfile;
  EFI_PHYSICAL_ADDRESS  CpuPrivateData;
  EFI_PHYSICAL_ADDRESS  StackAddress;
  EFI_PHYSICAL_ADDRESS  MicrocodePointerBuffer;
  EFI_PHYSICAL_ADDRESS  SmramBase;
  EFI_PHYSICAL_ADDRESS  SmmStartImageBase;
  UINT32                SmmStartImageSize;
  UINT32                NumberOfCpus;
} ACPI_CPU_DATA;



typedef struct {
  EFI_PHYSICAL_ADDRESS  AcpiReservedMemoryBase; ///< ACPI Reserved Memory Base (* Required)
  UINT32                AcpiReservedMemorySize; ///< ACPI Reserved Memory Size (* Required)
  EFI_PHYSICAL_ADDRESS  S3ReservedLowMemoryBase;///< S3 Reserved Low memory Base Address
  EFI_PHYSICAL_ADDRESS  AcpiBootScriptTable;    ///< Acpi Boot Script Table address
  EFI_PHYSICAL_ADDRESS  RuntimeScriptTableBase; ///< Runtime Script Table address
  EFI_PHYSICAL_ADDRESS  AcpiFacsTable;          ///< ACPI FACS Table address
  UINT64                SystemMemoryLength;     ///< System Memory Length
  ACPI_CPU_DATA         AcpiCpuData;            ///< ACPI CPU Data Structure
  EFI_PHYSICAL_ADDRESS  VideoOpromAddress;      ///< VGA OPROM to support Video Re-POST for Linux S3
  UINT32                VideoOpromSize;         ///< VGA OPROM to support Video Re-POST for Linux S3
  EFI_PHYSICAL_ADDRESS  S3DebugBufferAddress;   ///< S3 Debug extension
  EFI_PHYSICAL_ADDRESS  S3ResumeNvsEntryPoint;  ///< S3 Debug extension
} ACPI_VARIABLE_SET;


ACPI_VARIABLE_SET               *mAcpiVariableSetCompatibility = NULL;

/**
  Allocate memory below 4G memory address.

  This function allocates memory below 4G memory address.

  @param  MemoryType   Memory type of memory to allocate.
  @param  Size         Size of memory to allocate.
  
  @return Allocated address for output.

**/
VOID*
AllocateMemoryBelow4G (
  IN EFI_MEMORY_TYPE    MemoryType,
  IN UINTN              Size
  );

/**
  Hook point for AcpiVariableThunkPlatform for S3Ready.

  @param AcpiS3Context   ACPI s3 context
**/
VOID
S3ReadyThunkPlatform (
  IN ACPI_S3_CONTEXT      *AcpiS3Context
  )
{
  EFI_PHYSICAL_ADDRESS                          AcpiMemoryBase;
  UINT32                                        AcpiMemorySize;
  EFI_PEI_HOB_POINTERS                          Hob;
  UINT64                                        MemoryLength;

  DEBUG ((DEBUG_INFO, "S3ReadyThunkPlatform\n"));

  if (mAcpiVariableSetCompatibility == NULL) {
    return;
  }

  //
  // Allocate ACPI reserved memory under 4G
  //
  AcpiMemorySize = PcdGet32 (PcdS3AcpiReservedMemorySize);
  //*** AMI PORTING BEGIN ***//
  //AcpiMemoryBase = (EFI_PHYSICAL_ADDRESS)(UINTN)AllocateAcpiNvsMemoryBelow4G (PcdGet32 (PcdS3AcpiReservedMemorySize));
  AcpiMemoryBase = (EFI_PHYSICAL_ADDRESS)(UINTN) AllocateMemoryBelow4G (EfiACPIMemoryNVS, AcpiMemorySize);
  //*** AMI PORTING ENd ***//
  ASSERT (AcpiMemoryBase != 0);
  

  //
  // Calculate the system memory length by memory hobs
  //
  MemoryLength  = 0x100000;
  Hob.Raw = GetFirstHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR);
  ASSERT (Hob.Raw != NULL);
  while ((Hob.Raw != NULL) && (!END_OF_HOB_LIST (Hob))) {
    if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) {
      //
      // Skip the memory region below 1MB
      //
      if (Hob.ResourceDescriptor->PhysicalStart >= 0x100000) {
        MemoryLength += Hob.ResourceDescriptor->ResourceLength;
      }
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
    Hob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, Hob.Raw);
  }

  mAcpiVariableSetCompatibility->AcpiReservedMemoryBase = AcpiMemoryBase;
  mAcpiVariableSetCompatibility->AcpiReservedMemorySize = AcpiMemorySize;
  mAcpiVariableSetCompatibility->SystemMemoryLength     = MemoryLength;

  DEBUG((DEBUG_INFO, "AcpiVariableThunkPlatform: AcpiMemoryBase is 0x%8x\n", mAcpiVariableSetCompatibility->AcpiReservedMemoryBase));
  DEBUG((DEBUG_INFO, "AcpiVariableThunkPlatform: AcpiMemorySize is 0x%8x\n", mAcpiVariableSetCompatibility->AcpiReservedMemorySize));
  DEBUG((DEBUG_INFO, "AcpiVariableThunkPlatform: SystemMemoryLength is 0x%8x\n", mAcpiVariableSetCompatibility->SystemMemoryLength));

  return ;
}

/**
  Register callback function upon VariableLockProtocol
  to lock ACPI_GLOBAL_VARIABLE variable to avoid malicious code to update it.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
//*** AMI PORTING BEGIN ***//
/*
VOID
EFIAPI
VariableLockAcpiGlobalVariable (
  IN  EFI_EVENT                             Event,
  IN  VOID                                  *Context
  )
{
  EFI_STATUS                    Status;
  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLock;
  //
  // Mark ACPI_GLOBAL_VARIABLE variable to read-only if the Variable Lock protocol exists
  //
  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
  if (!EFI_ERROR (Status)) {
    Status = VariableLock->RequestToLock (VariableLock, ACPI_GLOBAL_VARIABLE, &gEfiAcpiVariableCompatiblityGuid);
    ASSERT_EFI_ERROR (Status);
  }
}
*/
//*** AMI PORTING ENd ***//
/**
  Hook point for AcpiVariableThunkPlatform for InstallAcpiS3Save.
**/
VOID
InstallAcpiS3SaveThunk (
  VOID
  )
{
  EFI_STATUS                           Status;
  //*** AMI PORTING BEGIN ***//
//  EFI_MP_SERVICES_PROTOCOL             *MpService;
//  UINTN                                VarSize;
//  VOID                                 *Registration;
  //*** AMI PORTING END ***//
  ///
  /// To eliminate the non MdePkg interfaces for MpService protocol and meet PI v1.2 compliance
  /// HSD #2281222
  ///
  //*** AMI PORTING BEGIN ***//
/*
  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID**) &MpService
                  );
  if (!EFI_ERROR (Status)) {
  

    //
    // On ECP platform, if framework CPU drivers are in use, The compatible version of ACPI variable set 
    // should be produced by CPU driver. 
    //
    VarSize = sizeof (mAcpiVariableSetCompatibility);
    Status = gRT->GetVariable (
                    ACPI_GLOBAL_VARIABLE,
                    &gEfiAcpiVariableCompatiblityGuid,
                    NULL,
                    &VarSize,
                    &mAcpiVariableSetCompatibility
                    );
    if (EFI_ERROR (Status) || (VarSize != sizeof (mAcpiVariableSetCompatibility))) {
      DEBUG ((DEBUG_ERROR, "FATAL ERROR: AcpiVariableSetCompatibility was not saved by CPU driver correctly. OS S3 may fail!\n"));
      mAcpiVariableSetCompatibility = NULL;
    }
  } else {  */
    //
    // Allocate/initialize the compatible version of Acpi Variable Set since Framework chipset/platform 
    // driver need this variable. ACPI_GLOBAL_VARIABLE variable is not used in runtime phase,
    // so RT attribute is not needed for it.
    //
   
    //mAcpiVariableSetCompatibility = AllocateAcpiNvsMemoryBelow4G (sizeof(ACPI_VARIABLE_SET_COMPATIBILITY));
  //*** AMI PORTING END ***//
    mAcpiVariableSetCompatibility = AllocateMemoryBelow4G (EfiACPIMemoryNVS, sizeof(ACPI_VARIABLE_SET_COMPATIBILITY));
    Status = gRT->SetVariable (
                    ACPI_GLOBAL_VARIABLE,
                    &gEfiAcpiVariableCompatiblityGuid,
                    //&gEfiAcpiVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    sizeof(mAcpiVariableSetCompatibility),
                    &mAcpiVariableSetCompatibility
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Register callback function upon VariableLockProtocol
      // to lock ACPI_GLOBAL_VARIABLE variable to avoid malicious code to update it.
      //
   //*** AMI PORTING BEGIN ***//
     // EfiCreateProtocolNotifyEvent (
      //  &gEdkiiVariableLockProtocolGuid,
      //  TPL_CALLBACK,
      //  VariableLockAcpiGlobalVariable,
      //  NULL,
      //  &Registration
      //  );
   //*** AMI PORTING END ***//
    } else {
      DEBUG ((DEBUG_ERROR, "FATAL ERROR: AcpiVariableSetCompatibility cannot be saved: %r. OS S3 may fail!\n", Status));
      gBS->FreePages (
             (EFI_PHYSICAL_ADDRESS) (UINTN) mAcpiVariableSetCompatibility,
             EFI_SIZE_TO_PAGES (sizeof (ACPI_VARIABLE_SET_COMPATIBILITY))
             );
      mAcpiVariableSetCompatibility = NULL;
    }
  //}
  DEBUG((-1,  "AcpiVariableSetCompatibility !!!! Status = %r, Name = %S, Guid = %g\n", Status, ACPI_GLOBAL_VARIABLE, &gEfiAcpiVariableCompatiblityGuid));
  DEBUG((DEBUG_INFO, "AcpiVariableSetCompatibility is 0x%8x\n", mAcpiVariableSetCompatibility));
}
