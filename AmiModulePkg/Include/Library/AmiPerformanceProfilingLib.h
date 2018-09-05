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
#ifndef __AMI_PERFORMANCE_PROFILING_LIB_H__
#define __AMI_PERFORMANCE_PROFILING_LIB_H__

#include <Guid/AmiPerformanceProfiling.h>

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
  );

/**
  Creates a entry in for the ProfilingName, and returns a handle for future references to the interface

  @param ImageHandle, OPTIONAL The ImageHandle of the driver that is bring profiled

  @param ProfilingName Pointer to the string description that describes this interface

  @param Handle Pointer to buffer to return a item that should be used for starting/stopping performance measurements.

  @return EFI_STATUS
  @retval EFI_SUCCESS An entry was successfully created
  @retval EFI_INVALID_PARAMETER The entry could not be created because the ProfilingName or the Handle were null
  @retval EFI_ALREADY_STARTED The entry could not be created because it already exists
**/
EFI_STATUS
EFIAPI
AmiPerformanceProfilingInitializeEntry (
  IN UINT64         ImageHandle, OPTIONAL
  IN CONST CHAR8    *ProfilingName,
  OUT VOID          **Handle
  );

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
  );

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
  );


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
  );


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
  );

/**
  Macro to locate and start a performance measurement on a entry handle. This macro is best used
  when there is no ability to store a global variable for an entry handle. This macro pairs with the 
  AMI_PROFILING_END macro.
**/
#define AMI_PROFILING_START(ImageHandle, Name, TimeStamp)                                               \
  do {                                                                                                  \
   if(AmiPerformanceProfilingMeasurementEnabled() == TRUE)                                              \
   {                                                                                                    \
      VOID *Handle = NULL;                                                                              \
      if(!EFI_ERROR(AmiPerformanceProfilingFindEntry((UINT64)ImageHandle, Name, &Handle)))              \
      {                                                                                                 \
          AmiPerformanceProfilingStartEntry(Handle, TimeStamp);                                         \
      }                                                                                                 \
      else                                                                                              \
      {                                                                                                 \
          if(!EFI_ERROR(AmiPerformanceProfilingInitializeEntry((UINT64)ImageHandle, Name, &Handle)))    \
              AmiPerformanceProfilingStartEntry(Handle, TimeStamp);                                     \
      }                                                                                                 \
    }                                                                                                   \
  } while (FALSE);

/**
  Macro to locate and end a performance measurement on a entry handle. This macro is best used
  when there is no ability to store a global variable for an entry handle. This macro pairs with the 
  AMI_PROFILING_START macro.
**/
#define AMI_PROFILING_END(ImageHandle, Name, TimeStamp)                                     \
  do {                                                                                      \
    if(AmiPerformanceProfilingMeasurementEnabled() == TRUE)                                 \
    {                                                                                       \
      VOID *Handle = NULL;                                                                  \
      if(!EFI_ERROR(AmiPerformanceProfilingFindEntry((UINT64)ImageHandle, Name, &Handle)))  \
      {                                                                                     \
          AmiPerformanceProfilingEndEntry(Handle, TimeStamp);                               \
      }                                                                                     \
    }                                                                                       \
  } while (FALSE);

/**
  Macro that calls the AmiPerformanceProfilingInitializeEntry function. 
**/
#define AMI_PROFILING_INIT_ENTRY(ImageHandle, Name, Handle)                         \
  do {                                                                              \
    if(AmiPerformanceProfilingMeasurementEnabled() == TRUE)                         \
    {                                                                               \
      AmiPerformanceProfilingInitializeEntry((UINT64)ImageHandle, Name, Handle);    \
    }                                                                               \
  } while (FALSE)

/**
  Macro to start a performance measurement on an existing measurement handle.  This macro pairs 
  the with AMI_PROFILING_END_ENTRY macro and requires an Entry to have already been created.
**/
#define AMI_PROFILING_START_ENTRY(Handle, TimeStamp)            \
  do {                                                          \
    if(AmiPerformanceProfilingMeasurementEnabled() == TRUE)     \
    {                                                           \
       AmiPerformanceProfilingStartEntry(Handle, TimeStamp);    \
    }                                                           \
  } while (FALSE)

/**
  Macro to end a performance measurement on an existing measurement handle. This macro pairs 
  with the AMI_PROFILING_START_ENTRY macro and requires an Entry to have already been created.
**/
#define AMI_PROFILING_END_ENTRY(Handle, TimeStamp)          \
  do {                                                      \
    if(AmiPerformanceProfilingMeasurementEnabled() == TRUE) \
    {                                                       \
      AmiPerformanceProfilingEndEntry(Handle, TimeStamp);   \
    }                                                       \
  } while (FALSE)

#endif
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
