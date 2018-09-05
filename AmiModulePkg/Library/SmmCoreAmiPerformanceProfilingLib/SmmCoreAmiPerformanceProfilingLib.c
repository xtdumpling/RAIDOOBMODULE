//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
#include <Library/UefiBootServicesTableLib.h>
#include <Library/AmiPerformanceProfilingLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmAccess2.h>

GLOBAL_REMOVE_IF_UNREFERENCED AMI_PERFORMANCE_PROFILING_ENTRY *EntryList = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                            MaxEntries = 0 ;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                            CurrentEntry = 0;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMRAM_DESCRIPTOR             *AmiPerformanceProfilingSmramRanges = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                            AmiPerformanceProfilingSmramRangeCount = 0;

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
  IN UINT64         ImageHandle, OPTIONAL
  IN CONST CHAR8    *ProfilingName,
  OUT VOID          **Handle
  )
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;

    if(ProfilingName != NULL && Handle != NULL)
    {
        if(CurrentEntry >= MaxEntries)
        {
            AMI_PERFORMANCE_PROFILING_ENTRY *TempEntry = NULL;

            Status = gSmst->SmmAllocatePool(EfiRuntimeServicesData, sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*(MaxEntries + 20), (VOID**)&TempEntry);
            if(!EFI_ERROR(Status))
            {
                SetMem((VOID**)&(TempEntry[MaxEntries]), sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*20, 0);
                CopyMem((VOID*)TempEntry, (VOID*)EntryList, sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*MaxEntries);

                gSmst->SmmFreePool((VOID*)EntryList);
                EntryList = TempEntry;

                MaxEntries = MaxEntries + 20;
            }
        }

        Status = AmiPerformanceProfilingFindEntry(ImageHandle, ProfilingName, Handle);
        if(Status == EFI_SUCCESS)
        {
            Status = EFI_ALREADY_STARTED;
        }
        else if(Status == EFI_NOT_FOUND)
        {
            EntryList[CurrentEntry].Signature = AMI_PERFORMANCE_PROFILING_SIGNATURE;
            EntryList[CurrentEntry].ImageHandle = ImageHandle;
            EntryList[CurrentEntry].CumulativeDuration = 0;
            EntryList[CurrentEntry].TimesCalled = 0;
            EntryList[CurrentEntry].CurrentStart = 0;
            AsciiStrnCpy(EntryList[CurrentEntry].Interface, ProfilingName, AMI_PERFORMANCE_PROFILING_STRING_SIZE);
            EntryList[CurrentEntry].Interface[AMI_PERFORMANCE_PROFILING_STRING_SIZE - 1] = '\0';
            
            *Handle = &(EntryList[CurrentEntry]);
            
            CurrentEntry++;

            Status = EFI_SUCCESS;
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

    if(DataEntry != NULL)
    {
        Status = EFI_NOT_FOUND;
        if(Index < CurrentEntry)
        {
            *DataEntry = &(EntryList[Index]);
            Status = EFI_SUCCESS;
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
    UINTN i = 0;

    if(Handle != NULL && (ImageHandle != 0 || ProfilingName != NULL) )
    {
        Status = EFI_NOT_FOUND;
        for(i = 0; i < CurrentEntry; i++)
        {
            if( ((ImageHandle == 0 ) && (AsciiStrnCmp(EntryList[i].Interface, ProfilingName, AMI_PERFORMANCE_PROFILING_STRING_SIZE) == 0)) ||
                ((ProfilingName == NULL) && (EntryList[i].ImageHandle == ImageHandle)) ||
                ((EntryList[i].ImageHandle == ImageHandle) && (AsciiStrnCmp(EntryList[i].Interface, ProfilingName, AMI_PERFORMANCE_PROFILING_STRING_SIZE) == 0)))
            {
                *Handle = &(EntryList[i]);
                Status = EFI_SUCCESS;
                break;
            }
        }
    }
    return Status;
}

/**
  Function that checks if a buffer can be used by the performance profiling SMM Communication handler.
  
  @param Buffer Physical address of the start of the buffer
  @param Length Length of the buffer
  
  @return EFI_STATUS
  @retval EFI_SUCCESS The buffer can be used
**/
EFI_STATUS ValidateAmiProfilingHandlerBuffer(VOID* Buffer, UINTN BufferSize){
    UINTN  i;
    UINTN BufferAddress;

    if (Buffer == NULL) return EFI_INVALID_PARAMETER;
    if (AmiPerformanceProfilingSmramRangeCount==0) return EFI_NOT_FOUND;

    BufferAddress = (UINTN)Buffer;
    if (BufferAddress + BufferSize < BufferAddress) return EFI_INVALID_PARAMETER; // overflow
    for (i = 0; i < AmiPerformanceProfilingSmramRangeCount; i ++) {
        if (    BufferAddress >= AmiPerformanceProfilingSmramRanges[i].CpuStart
             && BufferAddress < AmiPerformanceProfilingSmramRanges[i].CpuStart + AmiPerformanceProfilingSmramRanges[i].PhysicalSize
        ) return EFI_ACCESS_DENIED; // Buffer starts in SMRAM
        if (    BufferAddress < AmiPerformanceProfilingSmramRanges[i].CpuStart
             && BufferAddress+BufferSize > AmiPerformanceProfilingSmramRanges[i].CpuStart
        ) return EFI_ACCESS_DENIED; // Buffer overlaps with SMRAM
    }
    
    return EFI_SUCCESS;
}

/**
  Using either the passed ImageHandle, or the passed ProfilingName or both, attempt to find a
  profiling entry that matches the parameters. If only ImageHandle is passed, and there are
  multiple profiling entries associated with the ImaegHandle, then only the first entry will be returned.

  @param DispatchHandle The unique handle assigned to this handler by SmiHandlerRegister().
  
  @param Context Points to an optional handler context which was specified when the handler was registered.
                         
  @param CommBuffer A pointer to a collection of data in memory that will be conveyed from a non-SMM environment into an SMM environment.
                         
  @param CommBufferSize The size of the CommBuffer.

  @return EFI_STATUS
  @return EFI_SUCCESS This handler was called correctly. The Return status for the caller will be returned in the buffer that is passed.
**/
EFI_STATUS
EFIAPI
AmiSmmPerformanceProfilingHandler (
  IN     EFI_HANDLE                    DispatchHandle,
  IN     CONST VOID                   *RegisterContext,
  IN OUT VOID                          *CommBuffer,
  IN OUT UINTN                         *CommBufferSize
  )
{
    AMI_SMM_PERFORMANCE_PROFILING_COMMUNICATE *AmiSmmCommunicate;
    UINTN Size;

    if (CommBufferSize == NULL) return EFI_SUCCESS;
    
    AmiSmmCommunicate = (AMI_SMM_PERFORMANCE_PROFILING_COMMUNICATE*)CommBuffer;
    Size = *CommBufferSize;
    
    if (    Size < sizeof(AMI_SMM_PERFORMANCE_PROFILING_COMMUNICATE)
         || EFI_ERROR(ValidateAmiProfilingHandlerBuffer(AmiSmmCommunicate, Size))
    ) return EFI_SUCCESS;
    
    if( AmiSmmCommunicate->NumberOfEntries < CurrentEntry)
    {
        AmiSmmCommunicate->Status = EFI_BUFFER_TOO_SMALL;
        AmiSmmCommunicate->NumberOfEntries = CurrentEntry;
    }
    else
    {
        Size = sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*CurrentEntry;
        AmiSmmCommunicate->Status = ValidateAmiProfilingHandlerBuffer(AmiSmmCommunicate->ProfilingEntry, Size);
        if (EFI_ERROR(AmiSmmCommunicate->Status)) return EFI_SUCCESS;
        CopyMem((VOID*)AmiSmmCommunicate->ProfilingEntry, (VOID*)EntryList, Size);
        AmiSmmCommunicate->NumberOfEntries = CurrentEntry;
    }
    return EFI_SUCCESS;
}

GLOBAL_REMOVE_IF_UNREFERENCED AMI_PERFORMANCE_PROFILING_PROTOCOL AmiPerformanceProtocol = {
  AmiPerformanceProfilingInitializeEntry,
  AmiPerformanceProfilingStartEntry,
  AmiPerformanceProfilingEndEntry,
  AmiPerformanceProfilingGetEntry,
  AmiPerformanceProfilingFindEntry
  };

/**
  Callback function which is executed when the SmmBase2 protocol is available (and memory allocations are available).

  @param  Event Callback Event
  @param  Context Context for the callback

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
VOID
EFIAPI
InitializeAmiSmmCorePerformanceProfilingLib (
  IN EFI_EVENT     Event,
  IN VOID          *Context
  )
{
    EFI_STATUS Status;
    VOID *Hob = NULL;
    EFI_HANDLE Handle = NULL;

    UINTN Size = 0;
    EFI_SMM_ACCESS2_PROTOCOL *SmmAccess = NULL;
    Status = gBS->LocateProtocol (&gEfiSmmAccess2ProtocolGuid, NULL, (VOID **)&SmmAccess);
    if(!EFI_ERROR(Status))
    {
        Status = SmmAccess->GetCapabilities (SmmAccess, &Size, NULL);
        if(Status == EFI_BUFFER_TOO_SMALL)
        {
            Status = gSmst->SmmAllocatePool(EfiRuntimeServicesData, Size, (VOID **)&AmiPerformanceProfilingSmramRanges);
            if(!EFI_ERROR(Status))
            {
                Status = SmmAccess->GetCapabilities (SmmAccess, &Size, AmiPerformanceProfilingSmramRanges);
                if(!EFI_ERROR(Status))
                    AmiPerformanceProfilingSmramRangeCount = Size / sizeof (EFI_SMRAM_DESCRIPTOR);
            }
        }
    }


    Status = gSmst->SmmAllocatePool(EfiRuntimeServicesData, sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_SMM, &EntryList);
    if(!EFI_ERROR(Status))
    {
        SetMem(EntryList, sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_SMM, 0);
        MaxEntries = AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_SMM;
        CurrentEntry = 0;

        Status = gSmst->SmmInstallProtocolInterface(    &Handle,
                                                        &gAmiPerformanceProfilingGuid, 
                                                        EFI_NATIVE_INTERFACE,
                                                        (VOID*)&AmiPerformanceProtocol
                                                        );

        Status = gSmst->SmiHandlerRegister (AmiSmmPerformanceProfilingHandler, &gAmiPerformanceProfilingGuid, &Handle);
    }
}

/**
  The constructor function initializes the Performance Profiling infrastructure for the SMM phase.
  The constructor function publishes the Ami Performance Profiling protocol, and allocates memory to log the 
  performance entries

  It will ASSERT() if one of these operations fails and it will always return EFI_SUCCESS.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
SmmCoreAmiPerformanceProfilingLibConstructor(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS Status;
    EFI_EVENT Event;
    VOID *Registration;
    if(AmiPerformanceProfilingMeasurementEnabled() == TRUE)
    {
        Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      InitializeAmiSmmCorePerformanceProfilingLib,
                      NULL,
                      &Event
                      );
        if(!EFI_ERROR(Status))
        {
            Status = gBS->RegisterProtocolNotify (
                          &gEfiSmmBase2ProtocolGuid,
                          Event,
                          &Registration
                          );
        }
    }
    return EFI_SUCCESS;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
