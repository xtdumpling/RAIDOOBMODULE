//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
#include <Guid/AmiPerformanceProfiling.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PcdLib.h>


GLOBAL_REMOVE_IF_UNREFERENCED AMI_PERFORMANCE_PROFILING_PROTOCOL *AmiPerformanceProfilingProtocol;

/**
  Check if the ami performance profiling measurement macros are enabled. 
  
  @return BOOLEAN
  @retval TRUE Performance profiling macros are enabled
  @retval FALSE Performance profiling macros are not enabled
**/
BOOLEAN
EFIAPI
AmiPerformanceProfilingMeasurementEnabled (
  VOID
  )
{
  return (BOOLEAN) PcdGetBool(AmiPcdPerformanceProfilingSupport);
}

/**
  Locate the AmiPerformanceProfilingProtocol.

  @return EFI_STATUS
  @retval EFI_NOT_FOUND The AmiPerformanceProfilingProtocol was not found
  @retval EFI_SUCCESS The AmiPerformanceProfilingProtocol was located
**/
static EFI_STATUS GetAmiPerformanceProfilingProtocol(VOID)
{
    EFI_STATUS Status = EFI_SUCCESS;
    if(AmiPerformanceProfilingProtocol == NULL)
    {
        Status = gSmst->SmmLocateProtocol(&gAmiPerformanceProfilingGuid, NULL, &AmiPerformanceProfilingProtocol);
    }
    return Status;
}
/**
  Start performance profiling for the entry referred to by Handle.  If a measurement was already
  started for the handle, but never stopped, then it will be overwritten and lost

  @param Handle The device to start profiling 

  @param Start OPTIONAL The starting time to use for updating this entry.

  @return EFI_STATUS
  @retval EFI_SUCCESS Performance measurements were started for the handle
  @retval EFI_INVALID_PARAMETER The handle is not valid
**/
EFI_STATUS
EFIAPI
AmiPerformanceProfilingInitializeEntry (
  IN UINT64          ImageHandle, OPTIONAL
  IN CONST CHAR8    *ProfilingName,
  OUT VOID          **Handle
  )
{
    EFI_STATUS Status;

    Status = GetAmiPerformanceProfilingProtocol();
    if(!EFI_ERROR(Status))
    {
        Status = AmiPerformanceProfilingProtocol->InitializeEntry(ImageHandle, ProfilingName, Handle);
    }
    return Status;
}

/**
  Start performance profiling for the entry referred to by Handle.  If a measurement was already
  started for the handle, but never stopped, then it will be overwritten and lost

  @param Handle The device to start profiling 

  @param Start OPTIONAL The starting time to use for updating this entry.

  @return EFI_STATUS
  @retval EFI_SUCCESS Performance measurements were started for the handle
  @retval EFI_INVALID_PARAMETER The handle is not valid
**/
EFI_STATUS
EFIAPI
AmiPerformanceProfilingStartEntry(
  IN VOID           *Handle,
  IN UINT64         Start OPTIONAL
  )
{
    EFI_STATUS Status;

    Status = GetAmiPerformanceProfilingProtocol();
    if(!EFI_ERROR(Status))
    {
        Status = AmiPerformanceProfilingProtocol->StartEntry(Handle, Start);
    }
    return Status;
}

/**
  Stop the performance profiling for the entry referred to by Handle.

  @param Handle The device to stop profiling
 
  @param End OPTIONAL The ending time to use for this measurement entry

  @return EFI_STATUS
  @retval EFI_SUCCESS Performance measurements were stopped for the handle
  @retval EFI_INVALID_PARAMETER The handle was not valid, or no performance measurements were started
**/
EFI_STATUS
EFIAPI
AmiPerformanceProfilingEndEntry(
  IN VOID           *Handle,
  IN UINT64         End OPTIONAL
  )
{
    EFI_STATUS Status;

    Status = GetAmiPerformanceProfilingProtocol();
    if(!EFI_ERROR(Status))
    {
        Status = AmiPerformanceProfilingProtocol->EndEntry(Handle, End);
    }
    return Status;
}

/**
  Get the performance measurement structure for the entry referred to by Index.  Function is used
  to loop through all entries created.  

  @param Index The index of the entry to retrieve.

  @param DataEntry Double pointer used to return a pointer to an AMI_PERFORMANCE_PROFILING_ENTRY

  @return EFI_STATUS
  @return EFI_SUCCESS A pointer was returned for the Index
  @return EFI_INVALID_PARAMETER The DataEntry pointer was NULL
  @return EFI_NOT_FOUND An entry for Index was not found
**/
EFI_STATUS
EFIAPI
AmiPerformanceProfilingGetEntry(
  IN  UINTN                             Index,
  OUT AMI_PERFORMANCE_PROFILING_ENTRY   **DataEntry
  )
{
    EFI_STATUS Status;

    Status = GetAmiPerformanceProfilingProtocol();
    if(!EFI_ERROR(Status))
    {
        Status = AmiPerformanceProfilingProtocol->GetEntry(Index, DataEntry);
    }
    return Status;
}

/**
  Using either the passed ImageHandle, or the passed ProfilingName or both, attempt to find a
  profiling entry that matches the parameters. If only ImageHandle is passed, and there are
  multiple profiling entries associated with the ImaegHandle, then only the first entry will be returned.

  @param ImageHandle, OPTIONAL The ImageHandle of the driver that is bring profiled

  @param ProfilingName, OPTIONAL Pointer to the string description to match

  @param Handle Pointer to buffer to return a item that should be used for starting/stopping performance measurements.

  @return EFI_STATUS
  @return EFI_SUCCESS The Handle was returned
  @return EFI_INVALID_PARAMETER The Handle double pointer was NULL, or both the ImageHandle and the ProfilingName were null
  @return EFI_NOT_FOUND An entry matching the parameters was not found
**/
EFI_STATUS
EFIAPI
AmiPerformanceProfilingFindEntry(
  IN UINT64         ImageHandle, OPTIONAL
  IN CONST CHAR8    *ProfilingName, OPTIONAL
  OUT VOID          **Handle
  )
{
    EFI_STATUS Status;
    
    Status = GetAmiPerformanceProfilingProtocol();
    if(!EFI_ERROR(Status))
    {
        Status = AmiPerformanceProfilingProtocol->FindEntry(ImageHandle, ProfilingName, Handle);
    }
    return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
