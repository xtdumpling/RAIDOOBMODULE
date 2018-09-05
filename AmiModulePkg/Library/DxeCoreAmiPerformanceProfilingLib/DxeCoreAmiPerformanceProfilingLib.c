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
#include <Guid/AmiGlobalVariable.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/AmiPerformanceProfilingLib.h>
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/HobLib.h>
#include <Protocol/VariableWrite.h>

GLOBAL_REMOVE_IF_UNREFERENCED AMI_PERFORMANCE_PROFILING_ENTRY *EntryList;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                            MaxEntries;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                            CurrentEntry;

/**
  Check if the AMI performance profiling measurement macros are enabled. 
  
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
        Status = AmiPerformanceProfilingFindEntry(ImageHandle, ProfilingName, Handle);
        if(Status == EFI_SUCCESS)
        {
            Status = EFI_ALREADY_STARTED;
        }
        else if(Status == EFI_NOT_FOUND)
        {
            if(CurrentEntry >= MaxEntries)
            {
                AMI_PERFORMANCE_PROFILING_ENTRY *TempEntry = NULL;
    
                Status = gBS->AllocatePool(EfiBootServicesData, sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*(MaxEntries + 20), &TempEntry);
                if(!EFI_ERROR(Status))
                {
                    gBS->SetMem(&(TempEntry[MaxEntries]), sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*20, 0);
                    gBS->CopyMem(TempEntry, EntryList, sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*MaxEntries);
    
                    gBS->FreePool(EntryList);
                    EntryList = TempEntry;
    
                    MaxEntries = MaxEntries + 20;
                }
            }
        
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

GLOBAL_REMOVE_IF_UNREFERENCED AMI_PERFORMANCE_PROFILING_PROTOCOL AmiPerformanceProtocol = {
  AmiPerformanceProfilingInitializeEntry,
  AmiPerformanceProfilingStartEntry,
  AmiPerformanceProfilingEndEntry,
  AmiPerformanceProfilingGetEntry,
  AmiPerformanceProfilingFindEntry
  };

/** @internal
  Store the Performance Counter Properties into a Volatile variable so that they can be consumed 
  by drivers/applications that do not have access to this platform's TimerLib.
  NOTE: We assume the same timer is used during all boot phases (PEI, DXE, BDS, and SMM).
**/
VOID EFIAPI StorePerformanceTimerProperties(
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  )
{
    AMI_PERFORMANCE_COUNTER_PROPERTIES PerformanceSettings;

    PerformanceSettings.Frequency = GetPerformanceCounterProperties(&PerformanceSettings.StartValue, &PerformanceSettings.EndValue);
    gRT->SetVariable(
            AMI_PERFORMANCE_COUNTER_PROPERTIES_VARIABLE_NAME, &gAmiGlobalVariableGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, 
            sizeof(AMI_PERFORMANCE_COUNTER_PROPERTIES), &PerformanceSettings
            );
    gBS->CloseEvent(Event);
}

/** @internal
  Registers callback on Variable Write Architectural Protocol.
**/
VOID RegisterVaribleWriteProtocolNotify(){
    EFI_STATUS Status;
    EFI_EVENT Event;
    VOID* Registration;
    
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
                    StorePerformanceTimerProperties, NULL,
                    &Event
                    );
    if (!EFI_ERROR(Status))
        gBS->RegisterProtocolNotify (
                &gEfiVariableWriteArchProtocolGuid,
                Event, &Registration
                );

}

/**
  The constructor function initializes the Performance Profiling infrastructure for the DXE phase.
  The constructor function publishes the Ami Performance Profiling protocol, and allocates memory to log the 
  performance entries

  It will ASSERT() if one of these operations fails and it will always return EFI_SUCCESS.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
DxeCoreAmiPerformanceProfilingLibConstructor(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS Status = EFI_SUCCESS;
    VOID *Hob = NULL;
    AMI_PERFORMANCE_PROFILING_ENTRY *Entry = NULL;
    UINTN i = 0;
    
    UINTN InitialEntries = (AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_PEI > AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_DXE) ? AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_PEI : AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_DXE;
    
    if(AmiPerformanceProfilingMeasurementEnabled() == TRUE)
    {
        Status = gBS->AllocatePool(EfiBootServicesData, sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*InitialEntries, &EntryList);
        if(!EFI_ERROR(Status))
        {
            gBS->SetMem(EntryList, sizeof(AMI_PERFORMANCE_PROFILING_ENTRY)*InitialEntries, 0);
            MaxEntries = InitialEntries;
            CurrentEntry = 0;
    
            Status = gBS->InstallMultipleProtocolInterfaces(&ImageHandle,
                                                            &gAmiPerformanceProfilingGuid, &AmiPerformanceProtocol,
                                                            NULL
                                                            );
            Hob = GetFirstGuidHob(&gAmiPerformanceProfilingGuid);
            if(Hob != NULL)
            {
                Entry = (AMI_PERFORMANCE_PROFILING_ENTRY*)(GET_GUID_HOB_DATA(Hob));
                for(i = 0; i < AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_PEI ; i++)
                {
                    if(Entry[i].Signature == AMI_PERFORMANCE_PROFILING_SIGNATURE)
                    {
                        EntryList[CurrentEntry].Signature = AMI_PERFORMANCE_PROFILING_SIGNATURE;
                        EntryList[CurrentEntry].ImageHandle = Entry[i].ImageHandle;
                        EntryList[CurrentEntry].CumulativeDuration = Entry[i].CumulativeDuration;
                        EntryList[CurrentEntry].TimesCalled = Entry[i].TimesCalled;
                        EntryList[CurrentEntry].CurrentStart = 0;
                        AsciiStrnCpy(EntryList[CurrentEntry].Interface, Entry[i].Interface, AMI_PERFORMANCE_PROFILING_STRING_SIZE);
                        EntryList[CurrentEntry].Interface[AMI_PERFORMANCE_PROFILING_STRING_SIZE - 1] = '\0';
                        
                        CurrentEntry++;
                    }
                }
            }
            RegisterVaribleWriteProtocolNotify();
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

