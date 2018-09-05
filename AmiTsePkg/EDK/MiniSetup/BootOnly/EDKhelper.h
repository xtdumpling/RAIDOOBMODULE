//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/EDKhelper.h $
//
// $Author: Mallikarjunanv $
//
// $Revision: 3 $
//
// $Date: 12/28/10 6:12p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**********************************************************************
/** @file EDKhelper.h
    Edk helper header file.

**/
//**********************************************************************

#ifndef _EDK_HELPER_H_
#define	_EDK_HELPER_H_

//AMI <--> EDK mapping
#define EFI_LOADED_IMAGE_INFORMATION_REVISION EFI_LOADED_IMAGE_PROTOCOL_REVISION
typedef DXE_SERVICES EFI_DXE_SERVICES;
#define EFI_TPL_APPLICATION TPL_APPLICATION
#define EFI_TPL_CALLBACK TPL_CALLBACK
#define EFI_TPL_NOTIFY TPL_NOTIFY
#define EFI_TPL_HIGH_LEVEL TPL_HIGH_LEVEL

#ifndef	END_DEVICE_PATH_TYPE
#define END_DEVICE_PATH_TYPE 0x7F
#endif

#define END_ENTIRE_DEVICE_PATH_SUBTYPE 0xFF
#define EFI_EVENT_EFI_SIGNAL_MASK               0x000000FF
#define EFI_EVENT_EFI_SIGNAL_MAX                4
#define EFI_TPL_DRIVER      6
#define EFI_DXE_SERVICES_SIGNATURE DXE_SERVICES_SIGNATURE
#define EFI_DXE_SERVICES_REVISION DXE_SERVICES_REVISION
#define EFI_DXE_ENTRY_POINT(EntryPoint)
#ifndef EFI_DEADLOOP
#define EFI_DEADLOOP() { volatile UINTN __DeadLoopVar__ = 1; while (__DeadLoopVar__); }
#endif
#define EFI_BREAKPOINT()  EFI_DEADLOOP()
#define EFI_BAD_POINTER          0xAFAFAFAF
//This is defined in FirmwareValume.h but has nothing to do with the protocol.
//It should be in DxeMain internal header
//#define FV_DEVICE_SIGNATURE EFI_SIGNATURE_32 ('_', 'F', 'V', '_')
//#define EFI_FVH_SIGNATURE FV_SIGNATURE
//#define EFI_FV_BLOCK_MAP_ENTRY FvBlockMapEntry
//This is define in TianoType.h, however, this is not in DXECIS
//
// attributes for reserved memory before it is promoted to system memory
//
#define EFI_MEMORY_PRESENT      0x0100000000000000
#define EFI_MEMORY_INITIALIZED  0x0200000000000000
#define EFI_MEMORY_TESTED       0x0400000000000000

//
// range for memory mapped port I/O on IPF
//
#define EFI_MEMORY_PORT_IO  0x4000000000000000
//
// A pointer to a function in IPF points to a plabel.
//
typedef struct {
  UINT64  EntryPoint;
  UINT64  GP;
} EFI_PLABEL;
////////////////////////////////////////////////
//defined for every CPU architecture
// Maximum legal IA-32 address
#define EFI_MAX_ADDRESS   0xFFFFFFFF
////////////////////////////////////////////////

// Protocols
#define EFI_INCOMPATIBLE_PCI_DEVICE_SUPPORT EFI_INCOMPATIBLE_PCI_DEVICE_SUPPORT_PROTOCOL
#define EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL_GUID FW_VOLUME_BLOCK_PROTOCOL_GUID
#define EFI_SIMPLE_TEXT_OUT_PROTOCOL_GUID EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID
#define EFI_SIMPLE_TEXT_OUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
#define EFI_SIMPLE_TEXT_IN_PROTOCOL_GUID EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID
#define EFI_SIMPLE_TEXT_IN_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL
#define LOAD_FILE_PROTOCOL_GUID EFI_LOAD_FILE_PROTOCOL_GUID
#define EFI_EBC_INTERPRETER_PROTOCOL_GUID EFI_EBC_PROTOCOL_GUID

//from EfiStdArg.h
#define _EFI_INT_SIZE_OF(n) ((sizeof (n) + sizeof (UINTN) - 1) &~(sizeof (UINTN) - 1))

//
// Also support coding convention rules for var arg macros
//
#ifndef VA_START

typedef CHAR8 *VA_LIST;
#define VA_START(ap, v) (ap = (VA_LIST) & (v) + _EFI_INT_SIZE_OF (v))
#define VA_ARG(ap, t)   (*(t *) ((ap += _EFI_INT_SIZE_OF (t)) - _EFI_INT_SIZE_OF (t)))
#define VA_END(ap)      (ap = (VA_LIST) 0)

#endif

//from EfiDevicePath.h
#define EFI_DP_TYPE_MASK                    0x7F
#define EFI_DP_TYPE_UNPACKED                0x80
#define END_INSTANCE_DEVICE_PATH_SUBTYPE    0x01

#define DP_IS_END_TYPE(a)
#define DP_IS_END_SUBTYPE(a)        ( ((a)->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE )

#define DevicePathType(a)           ( ((a)->Type) & EFI_DP_TYPE_MASK )
#define DevicePathSubType(a)        ( (a)->SubType )
#define DevicePathNodeLength(a)     ( ((a)->Length[0]) | ((a)->Length[1] << 8) )

#define IsDevicePathEndType(a)      ( DevicePathType(a) == END_DEVICE_PATH_TYPE )
#define IsDevicePathEndSubType(a)   ( (a)->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE )
#define IsDevicePathEnd(a)          ( IsDevicePathEndType(a) && IsDevicePathEndSubType(a) )
#define NextDevicePathNode(a)       ( (EFI_DEVICE_PATH_PROTOCOL *) ( ((UINT8 *) (a)) + DevicePathNodeLength(a)))

#define SetDevicePathNodeLength(a,l) {                \
          (a)->Length[0] = (UINT8) (l);               \
          (a)->Length[1] = (UINT8) ((l) >> 8);        \
          }

#define SetDevicePathEndNode(a)  {                       \
          (a)->Type = END_DEVICE_PATH_TYPE;              \
          (a)->SubType = END_ENTIRE_DEVICE_PATH_SUBTYPE; \
          (a)->Length[0] = sizeof(EFI_DEVICE_PATH_PROTOCOL);      \
          (a)->Length[1] = 0;                            \
          }



EFI_STATUS
EfiInitializeDriverLib (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

VOID *
EfiLibAllocateZeroPool (
  IN  UINTN   AllocationSize
  );

VOID *
EfiLibAllocatePool (
  IN  UINTN   AllocationSize
  );

BOOLEAN
EfiCompareGuid (
  IN EFI_GUID *Guid1,
  IN EFI_GUID *Guid2
  );

INTN
EfiCompareMem (
  IN VOID     *MemOne,
  IN VOID     *MemTwo,
  IN UINTN    Length
  );

UINTN
EfiStrLen (
  IN CHAR16   *String
  );

VOID
EfiStrCpy (
  IN CHAR16   *Destination,
  IN CHAR16   *Source
  );

INTN
EfiStrCmp (
  IN CHAR16   *String,
  IN CHAR16   *String2
  );

EFI_DEVICE_PATH_PROTOCOL *
EfiAppendDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  );


EFI_DEVICE_PATH_PROTOCOL        *
EfiAppendDevicePathNode (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  );

EFI_DEVICE_PATH_PROTOCOL *
EfiDevicePathFromHandle (
  IN EFI_HANDLE  Handle
  );

EFI_DEVICE_PATH_PROTOCOL *
EfiFileDevicePath (
  IN EFI_HANDLE               Device  OPTIONAL,
  IN CHAR16                   *FileName
  );

VOID
EFIAPI
TseEfiInitializeFwVolDevicepathNode (
  IN  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH     *FvDevicePathNode,
  IN EFI_GUID                               *NameGuid
  );


EFI_STATUS
EfiLibNamedEventSignal (
  IN EFI_GUID            *Name
  );

EFI_STATUS
EfiLibReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     Type,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId OPTIONAL,
  IN EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  );

//STATIC
VOID
EFIAPI
EventNotifySignalAllNullEvent (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  );

EFI_STATUS
EFIAPI
TseEfiCreateEventReadyToBoot (
  IN EFI_TPL                      NotifyTpl,
  IN EFI_EVENT_NOTIFY             NotifyFunction,
  IN VOID                         *NotifyContext,
  OUT EFI_EVENT                   *ReadyToBootEvent
  );

BOOLEAN
IsHexDigit (
  OUT UINT8      *Digit,
  IN  CHAR16      Char
  );


EFI_STATUS
HexStringToBuf (
  IN OUT UINT8                     *Buf,
  IN OUT UINTN                    *Len,
  IN     CHAR16                    *Str,
  OUT    UINTN                     *ConvertedStrLen  OPTIONAL
  );

#if !TSE_APTIO_5_SUPPORT
UINT64
MultU64x32 (
IN UINT64 Multiplicand,
IN UINTN Multiplier
);
#endif



UINTN
EfiDevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

VOID
EfiDebugAssert (
  IN CHAR8    *FileName,
  IN INTN     LineNumber,
  IN CHAR8    *Description
  );

VOID
EfiDebugPrint (
  IN  UINTN ErrorLevel,
  IN  CHAR8 *Format,
  ...
  );

// EDKHelper.c
#define CONST const
#define STATIC    static
#define VOLATILE  volatile

#define EfiCopyMem(_Destination, _Source, _Length)  gBS->CopyMem ((_Destination), (_Source), (_Length))
#define EfiZeroMem(_Destination, _Length)  gBS->SetMem ((_Destination), (_Length), 0)
//
// Length of temp string buffer to store value string.
//
#define CHARACTER_NUMBER_FOR_VALUE  30

UINTN
EfiStrSize (
  IN CHAR16   *String
  );

VOID
EfiStrCat (
  IN CHAR16   *Destination,
  IN CHAR16   *Source
  );

CHAR16*
 EfiStrStr (
   IN  CHAR16  *String,
   IN  CHAR16  *StrCharSet
   );

UINT64
AmiTseDivU64x32 (
  IN  UINT64  Dividend,
  IN  UINTN   Divisor,
  OUT UINTN   *Remainder OPTIONAL
  );

UINT64
EFIAPI
LShiftU64 (
  IN UINT64   Operand,
  IN UINTN    Count
  );

UINT64
EFIAPI
RShiftU64 (
  IN UINT64   Operand,
  IN UINTN    Count
  );

EFI_STATUS
BufToHexString (
  IN OUT CHAR16                    *Str,
  IN OUT UINTN                     *HexStringBufferLength,
  IN     UINT8                     *Buf,
  IN     UINTN                      Len
  );
VOID *
EfiLibAllocateCopyPool (
  IN  UINTN   AllocationSize,
  IN  VOID    *Buffer
  );

#endif /* _EDK_HELPER_H_ */

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**           5555 Oakbrook Pkwy, Norcross, Georgia 30093       **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
