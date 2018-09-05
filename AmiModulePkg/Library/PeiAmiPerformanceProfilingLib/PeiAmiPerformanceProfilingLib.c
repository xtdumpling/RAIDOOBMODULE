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
#include <Library/AmiPerformanceProfilingLib.h>
#include <Library/HobLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
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
  Creates a entry in for the ProfilingName, and returns a handle for future references to the interface

  @param ImageHandle, OPTIONAL The ImageHandle of the driver that is bring profiled

  @param ProfilingName Pointer to the string description that describes this interface

  @param Handle Pointer to buffer to return a item that should be used for starting/stopping performance measurements.

  @return EFI_STATUS
  @retval EFI_SUCCESS An entry was successfully created
  @retval EFI_INVALID_PARAMETER The entry could not be created because the ProfilingName and the Handle were null
  @retval EFI_ALREADY_STARTED The entry could not be created because it already exists
**/
EFI_STATUS
EFIAPI
AmiPerformanceProfilingInitializeEntry (
  IN UINT64          ImageHandle, OPTIONAL
  IN CONST CHAR8    *ProfilingName,
  OUT VOID          **Handle
  )
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    VOID *Hob = NULL;
    AMI_PERFORMANCE_PROFILING_ENTRY *Entry = NULL;
    UINTN i = 0;

    if(ProfilingName != NULL && Handle != NULL)
    {
        Status = AmiPerformanceProfilingFindEntry(ImageHandle, ProfilingName, Handle);
        if(Status == EFI_SUCCESS)
        {
            Status = EFI_ALREADY_STARTED;
        }
        else if(Status == EFI_NOT_FOUND)
        {
            Status = EFI_INVALID_PARAMETER;

            Hob = GetFirstGuidHob(&gAmiPerformanceProfilingGuid);
            if(Hob != NULL)
                Entry = GET_GUID_HOB_DATA(Hob);
            else
            {
                Entry = BuildGuidHob(&gAmiPerformanceProfilingGuid, sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_PEI);
                if(Entry != NULL)
                    SetMem(Entry, sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_PEI, 0);
            }
            
            if(Entry != NULL)
            {
                for(i = 0; i < AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_PEI; i++)
                {
                    if(Entry[i].Signature != AMI_PERFORMANCE_PROFILING_SIGNATURE)
                    {
                        Entry[i].Signature = AMI_PERFORMANCE_PROFILING_SIGNATURE;
                        Entry[i].ImageHandle = (UINT64)(UINT32)ImageHandle;
                        Entry[i].CumulativeDuration = 0;
                        Entry[i].TimesCalled = 0;
                        Entry[i].CurrentStart = 0;
                        AsciiStrnCpy(Entry[i].Interface, ProfilingName, AMI_PERFORMANCE_PROFILING_STRING_SIZE);
                        Entry[i].Interface[AMI_PERFORMANCE_PROFILING_STRING_SIZE] = '\0';
                        
                        *Handle = (VOID*)&(Entry[i]);
                
                        Status = EFI_SUCCESS;
                        break;
                    }
                }
            }
        }
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
    AMI_PERFORMANCE_PROFILING_ENTRY *Entry = (AMI_PERFORMANCE_PROFILING_ENTRY*)Handle;

    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    if(Entry != NULL)
    {
        if(Entry->Signature == AMI_PERFORMANCE_PROFILING_SIGNATURE)
        {
            if(Start == 0)
                Start = GetPerformanceCounter();
                
            Entry->CurrentStart = Start;

            Status = EFI_SUCCESS;
        }
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
    AMI_PERFORMANCE_PROFILING_ENTRY *Entry = (AMI_PERFORMANCE_PROFILING_ENTRY*)Handle;
    EFI_STATUS Status = EFI_INVALID_PARAMETER;

    if(End == 0)
        End = GetPerformanceCounter();

    if(Entry != NULL)
    {
        if(Entry->Signature == AMI_PERFORMANCE_PROFILING_SIGNATURE)
        {
            if(Entry->CurrentStart != 0)
            {
                Entry->CumulativeDuration += (Entry->CurrentStart > End) ? (Entry->CurrentStart - End) : (End - Entry->CurrentStart);

                Entry->CurrentStart = 0;
                
                Entry->TimesCalled++;
                
                Status = EFI_SUCCESS;
            }
        }
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
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    UINTN i = 0;
    VOID *Hob = NULL;
    AMI_PERFORMANCE_PROFILING_ENTRY *Entry = NULL;

    if(DataEntry != NULL)
    {
        Status = EFI_NOT_FOUND;

        if(Index < AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_PEI)
        {
            Hob = GetFirstGuidHob(&gAmiPerformanceProfilingGuid);
            if(Hob != NULL)
            {
                Entry = GET_GUID_HOB_DATA(Hob);
                if(Entry != NULL)
                {
                    for(i = 0; i < AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_PEI; i++)
                    {
                        if(Entry[i].Signature != AMI_PERFORMANCE_PROFILING_SIGNATURE)
                            break;
                        else if(i == Index)
                        {
                            *DataEntry = (AMI_PERFORMANCE_PROFILING_ENTRY*)&(Entry[i]);
                            Status = EFI_SUCCESS;
                            break;
                        }
                    }
                }
            }
        }
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
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    AMI_PERFORMANCE_PROFILING_ENTRY *Entry = NULL;
    UINTN i = 0;
    VOID *Hob = NULL;

    if(Handle != NULL && (ImageHandle != 0 || ProfilingName != NULL) )
    {
        Status = EFI_NOT_FOUND;
        
        Hob = GetFirstGuidHob(&gAmiPerformanceProfilingGuid);
        if(Hob != NULL)
        {
            Entry = GET_GUID_HOB_DATA(Hob);
            if(Entry != NULL)
            {
                for(i = 0; i < AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_PEI; i++)
                {
                    if( ((ImageHandle == 0 ) && (AsciiStrnCmp(Entry[i].Interface, ProfilingName, AMI_PERFORMANCE_PROFILING_STRING_SIZE) == 0)) ||
                        ((ProfilingName == NULL) && (Entry[i].ImageHandle == ImageHandle)) ||
                        ((Entry[i].ImageHandle == ImageHandle) && (AsciiStrnCmp(Entry[i].Interface, ProfilingName, AMI_PERFORMANCE_PROFILING_STRING_SIZE) == 0)))
                    {
                        *Handle = (VOID*)&(Entry[i]);
                        Status = EFI_SUCCESS;
                        break;
                    }
                }
            }
        }
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

