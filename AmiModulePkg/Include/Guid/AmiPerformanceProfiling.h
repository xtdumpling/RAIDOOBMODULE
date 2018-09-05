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
#ifndef __AMI_PERFORMANCE_PROFILING_H__
#define __AMI_PERFORMANCE_PROFILING_H__

/**
  GUID definition for the HOBs, Protocols and Smm Communicate handler
**/
#define AMI_PERFORMANCE_PROFILING_GUID \
  { 0x28d4dd89, 0x169e, 0x49dd, { 0x84, 0x86, 0xa2, 0x0, 0xa2, 0xfd, 0x3c, 0x21 } }

/**
  Maximum number of PEI profiling measurement entries.
**/
#define AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_PEI   20

/**
  Initial number of DXE profiling measurement entries
**/
#define AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_DXE   100

/**
  Initial number of SMM performance measurement entries
**/
#define AMI_PERFORMANCE_PROFILING_INITIAL_ENTRIES_SMM   20

/** 
  String Size definitions for the AMI_PERFORMANCE_PROFILING_ENTRY string
**/
#define AMI_PERFORMANCE_PROFILING_STRING_SIZE   32

/**
  The AMI Performance Profiling Entry signature
**/
#define AMI_PERFORMANCE_PROFILING_SIGNATURE SIGNATURE_32('A', 'P', 'P', 'E')

typedef struct _AMI_PERFORMANCE_PROFILING_PROTOCOL AMI_PERFORMANCE_PROFILING_PROTOCOL;

/**
  Performance Profiling Entry structure for storing performance entries
**/
#pragma pack(push, 1)
typedef struct {
  UINT32        Signature;                                          ///< Signature field used to determine field validity
  UINT64        ImageHandle;                                        ///< Image handle associated with the item being measured
  CHAR8         Interface[AMI_PERFORMANCE_PROFILING_STRING_SIZE];   ///< Name of the interface being measured
  UINT64        CumulativeDuration;                                 ///< The total time spent in this 
  UINT64        TimesCalled;                                        ///< The number of times that the interface has been measured
  UINT64        CurrentStart;                                       ///< The current start time stamp
} AMI_PERFORMANCE_PROFILING_ENTRY;

/**
  Structure definition is the payload of the EFI_SMM_COMMUNICATE_HEADER structure for communicating with the SmmHandler
**/
typedef struct {
  EFI_STATUS                        Status;             ///< Return Status of the Communicate call
  UINTN                             NumberOfEntries;    ///< The number of entries allocated by the ProfilingEntry
  AMI_PERFORMANCE_PROFILING_ENTRY  *ProfilingEntry;     ///< Pointer to a buffer to fill with the performance profiling entries
} AMI_SMM_PERFORMANCE_PROFILING_COMMUNICATE;
#pragma pack(pop)

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
typedef
EFI_STATUS
(EFIAPI * AMI_PERFORMANCE_PROFILING_INITIALIZE_ENTRY)(
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
typedef
EFI_STATUS
(EFIAPI * AMI_PERFORMANCE_PROFILING_START_ENTRY)(
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
typedef
EFI_STATUS
(EFIAPI * AMI_PERFORMANCE_PROFILING_END_ENTRY)(
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
typedef
EFI_STATUS
(EFIAPI * AMI_PERFORMANCE_PROFILING_GET_ENTRY)(
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
typedef
EFI_STATUS
(EFIAPI * AMI_PERFORMANCE_PROFILING_FIND_ENTRY)(
  IN UINT64         ImageHandle, OPTIONAL
  IN CONST CHAR8    *ProfilingName, OPTIONAL
  OUT VOID          **Handle
  );

struct _AMI_PERFORMANCE_PROFILING_PROTOCOL {
  AMI_PERFORMANCE_PROFILING_INITIALIZE_ENTRY    InitializeEntry;
  AMI_PERFORMANCE_PROFILING_START_ENTRY         StartEntry;
  AMI_PERFORMANCE_PROFILING_END_ENTRY           EndEntry;
  AMI_PERFORMANCE_PROFILING_GET_ENTRY           GetEntry;
  AMI_PERFORMANCE_PROFILING_FIND_ENTRY          FindEntry;
};

/**
  UEFI Variable with information about properties of the timer used by profiling library.
  The variable can be used by UEFI application that accesses profiling data to convert tick numbers to seconds.
  __NOTE: We assume the same timer is used during all boot phases (PEI, DXE, BDS, and SMM).
  Variable name: AMI_PERFORMANCE_COUNTER_PROPERTIES_VARIABLE_NAME
  Variable GUID: gAmiGlobalVariableGuid
  Variable data buffer format: AMI_PERFORMANCE_COUNTER_PROPERTIES
**/
typedef struct {
    UINT64 Frequency;
    UINT64 StartValue;
    UINT64 EndValue;
} AMI_PERFORMANCE_COUNTER_PROPERTIES;

#define AMI_PERFORMANCE_COUNTER_PROPERTIES_VARIABLE_NAME L"TimerProperties"

extern EFI_GUID gAmiPerformanceProfilingGuid;

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
