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

/** @file AmiDef.h
    AMI USB driver definitions, framework specific

**/

// Avoid including multiple instance of this file

#ifndef     __AMI_H
#define     __AMI_H

#include    <Token.h>

#define USB_DRIVER_VERSION  USB_DRIVER_MAJOR_VER

#include <Library/DebugLib.h>
#include <AmiDxeLib.h>

#define _FAR_

//  Basic type definitions of various size

#if (__STDC_VERSION__ < 199901L )

    /*  No ANSI C 1999/2000 stdint.h integer width declarations  */

    #if _MSC_EXTENSIONS

        /*  Use Microsoft C compiler integer width declarations  */
#if _64_BIT_EXTENSIONS
        typedef unsigned __int64    UINT64;
        typedef __int64             INT64;
typedef UINT64                  QWORD;
typedef QWORD *                 PQWORD;
typedef QWORD _FAR_ *           FPQWORD;
typedef UINT64 *                    PUINT64;
typedef INT64  *                    PINT64;
typedef UINT64 _FAR_ *          FPUINT64;
typedef INT64 _FAR_ *           FPINT64;
#endif
//        typedef unsigned __int32    UINT32;
//        typedef __int32             INT32;
//        typedef unsigned __int16    UINT16;
//        typedef __int16             INT16;
//        typedef unsigned __int8     UINT8;
//        typedef __int8              INT8;
     #else
#if _64_BIT_EXTENSIONS
#endif
    #endif
#endif

// Return code definition
typedef INT16           RETCODE;

//  64-bit extenstion definition
#if _64_BIT_EXTENSIONS
#endif

#ifndef NULL
#define NULL                    0
#endif

// Private and public definitions
#define PRIVATE             static
#define PUBLIC

// Function IN/OUT definitions
#define IN
#define OUT
#define IN_OUT

// Return codes
#define SUCCESS         0
#define FAILURE         -1

#ifndef TRUE
#define TRUE            1
#endif
#ifndef FALSE
#define FALSE           0
#endif
/*
// Bit equates
#define BIT0                0x01
#define BIT1                0x02
#define BIT2                0x04
#define BIT3                0x08
#define BIT4                0x10
#define BIT5                0x20
#define BIT6                0x40
#define BIT7                0x80
#define BIT8                0x100
#define BIT9                0x200
*/
/* defined in AmiDxeLib
#define BIT10               0x400
#define BIT11               0x800
#define BIT12               0x1000
#define BIT13               0x2000
#define BIT14               0x4000
#define BIT15               0x8000
#define BIT16               0x10000
#define BIT17               0x20000
#define BIT18               0x40000
#define BIT19               0x80000
#define BIT20               0x100000
#define BIT21               0x200000
#define BIT22               0x400000
#define BIT23               0x800000
#define BIT24               0x1000000
#define BIT25               0x2000000
#define BIT26               0x4000000
#define BIT27               0x8000000
#define BIT28               0x10000000
#define BIT29               0x20000000
#define BIT30               0x40000000
#define BIT31               0x80000000
*/
/*
#define SCAN_NULL EFI_SCAN_NULL
#define SCAN_ESC EFI_SCAN_ESC
#define SCAN_F1 EFI_SCAN_F1
#define SCAN_F2 EFI_SCAN_F2
#define SCAN_F3 EFI_SCAN_F3
#define SCAN_F4 EFI_SCAN_F4
#define SCAN_F5 EFI_SCAN_F5
#define SCAN_F6 EFI_SCAN_F6
#define SCAN_F7 EFI_SCAN_F7
#define SCAN_F8 EFI_SCAN_F8
#define SCAN_F9 EFI_SCAN_F9
#define SCAN_F10 EFI_SCAN_F10
#define SCAN_F11 EFI_SCAN_F11
#define SCAN_F12 EFI_SCAN_F12
#define SCAN_INSERT EFI_SCAN_INS
#define SCAN_HOME EFI_SCAN_HOME
#define SCAN_PAGE_UP EFI_SCAN_PGUP
#define SCAN_DELETE EFI_SCAN_DEL
#define SCAN_END EFI_SCAN_END
#define SCAN_PAGE_DOWN EFI_SCAN_PGDN
#define SCAN_RIGHT EFI_SCAN_RIGHT
#define SCAN_LEFT EFI_SCAN_LEFT
#define SCAN_DOWN EFI_SCAN_DN
#define SCAN_UP EFI_SCAN_UP
*/


//#define MESSAGING_DEVICE_PATH MESSAGING_DEVICE_PATH_TYPE
//#define MSG_USB_CLASS_DP MSG_USB_CLASS_DEVICE_PATH_SUBTYPE
#define EFI_END_ENTIRE_DEVICE_PATH          END_DEVICE_PATH
#define EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE  END_ENTIRE_SUBTYPE
//#define MSG_SCSI_DP       MSG_SCSI_DEVICE_PATH_SUBTYPE
//#define Pun TargetId

#define EFI_DRIVER_ENTRY_POINT(x)
//#define EFI_LOADED_IMAGE_PROTOCOL EFI_LOADED_IMAGE
#define EfiInitializeSmmDriverLib(ImageHandle, SystemTable) InitAmiLib(ImageHandle, SystemTable)
#define EfiInitializeDriverLib(ImageHandle, SystemTable) InitAmiLib(ImageHandle,SystemTable)
#define gBS pBS
#define gRT pRS
#define EfiDuplicateDevicePath              DPCopy


// Loop forever macro
#define LOOP_FOREVER    while(1)

#if defined(USB_DEBUG_MESSAGES) && (USB_DEBUG_MESSAGES != 0)
//#define EfiDebugVPrint(EFI_D_ERROR, Message, ArgList) PrintDebugMessageVaList(-1, Message, ArgList)
//extern  RETCODE         PrintDebugMsg (int, int, char *, ...);
//#define USB_DEBUG       PrintDebugMsg
#if USB_DEBUG_MESSAGES == 1
#define USB_DEBUG(EdkErrorLevel, UsbErrorLevel, ...) \
    if ((UsbErrorLevel == 0) || ((UsbErrorLevel <= TopDebugLevel) && \
    (UsbErrorLevel >= BottomDebugLevel))) \
        DEBUG((-1, __VA_ARGS__))
#else if USB_DEBUG_MESSAGES == 2
#define USB_DEBUG(EdkErrorLevel, UsbErrorLevel, ...) \
    if ((UsbErrorLevel == 0) || ((UsbErrorLevel <= TopDebugLevel) && \
    (UsbErrorLevel >= BottomDebugLevel))) \
        DEBUG((EdkErrorLevel, __VA_ARGS__))
#endif
#define USB_DEBUG_LEVEL DEBUG_LEVEL_3
void DEBUG_DELAY();
#else
//extern  RETCODE         PrintDebugMsg (int, int, char *, ...);
#define USB_DEBUG
#define DEBUG_DELAY
#endif

#if GENERIC_USB_CABLE_SUPPORT
#undef USB_DEBUG
#define USB_DEBUG
#endif

#define  MAX_DEBUG_LEVEL        8

#define DEBUG_LEVEL_8           8
#define DEBUG_LEVEL_7           7
#define DEBUG_LEVEL_6           6
#define DEBUG_LEVEL_5           5
#define DEBUG_LEVEL_4           4
#define DEBUG_LEVEL_3           3
#define DEBUG_LEVEL_2           2
#define DEBUG_LEVEL_1           1
#define DEBUG_LEVEL_0           0

#define MICROSECOND     10
#define MILLISECOND     (1000 * MICROSECOND)
#define ONESECOND       (1000 * MILLISECOND)

#define USB_DATA_EBDA_OFFSET		0x104
#define USB_PROTOCOL_EBDA_OFFSET	0x32		//(EIP55275)

#define GET_CPUSAVESTATE_REG(x) (URP_STRUC*)(UINTN)gSmst->CpuSaveState->Ia32SaveState.x
//#define GET_FV_NAME(pImage) &((FV_FILE_PATH_DEVICE_PATH*)(pImage->FilePath))->Name
#define GET_FV_NAME(pImage) &((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)(pImage->FilePath))->NameGuid
#define EfiDevicePathNodeLength(a)  (((a)->Length[0]) | ((a)->Length[1] << 8))
//#define EfiNextDevicePathNode(a)    ((EFI_DEVICE_PATH_PROTOCOL *) ( ((UINT8 *) (a)) + \
//                                      EfiDevicePathNodeLength(a)))
#define EfiNextDevicePathNode(a) NEXT_NODE(a)

#define EfiDevicePathType(a)          ( ((a)->Type) & 0x7f )
#define EfiIsDevicePathEndType(a)     (EfiDevicePathType(a) == 0x7f)


#define EfiIsDevicePathEndSubType(a)          ((a)->SubType == 0xFF)
#define EfiIsDevicePathEndInstanceSubType(a)  ((a)->SubType == 1)

#define EfiIsDevicePathEnd(a)         ( EfiIsDevicePathEndType(a) && \
                                        EfiIsDevicePathEndSubType(a) )
#define EfiIsDevicePathEndInstance(a) ( EfiIsDevicePathEndType(a) && \
                                        EfiIsDevicePathEndInstanceSubType(a) )
//#define SetDevicePathNodeLength(a,l) { (a)->Length = (l); }
#define SetDevicePathNodeLength(a,l) ( SET_NODE_LENGTH(a,l) )
//#define DevicePathNodeLength(a)     ( ((a)->Length[0]) | ((a)->Length[1] << 8) )
#define DevicePathNodeLength(a)      ( NODE_LENGTH(a) )
//#define NextDevicePathNode(a)       ( (EFI_DEVICE_PATH_PROTOCOL *) ( ((UINT8 *) (a)) + DevicePathNodeLength(a)))
#define NextDevicePathNode(a)       ( (EFI_DEVICE_PATH_PROTOCOL *) ( ((UINT8 *) (a)) + NODE_LENGTH(a)))
#define SetDevicePathEndNode(a)  {                       \
          (a)->Type = 0x7F;              \
          (a)->SubType = 0xFF; \
          (a)->Length = sizeof(EFI_DEVICE_PATH_PROTOCOL);      \
          }
/*
#define VA_LIST va_list
#define VA_START va_start
#define VA_ARG va_arg
#define VA_END va_end
*/
//
//  CONTAINING_RECORD - returns a pointer to the structure
//      from one of it's elements.
//
#define _CR(Record, TYPE, Field)  \
    ((TYPE *) ( (CHAR8 *)(Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))


//
// Define macros to build data structure signatures from characters.
//
#define EFI_SIGNATURE_16(A,B)             ((A) | (B<<8))
#define EFI_SIGNATURE_32(A,B,C,D)         (EFI_SIGNATURE_16(A,B)     | (EFI_SIGNATURE_16(C,D)     << 16))
#define EFI_SIGNATURE_64(A,B,C,D,E,F,G,H) (EFI_SIGNATURE_32(A,B,C,D) | ((UINT64)(EFI_SIGNATURE_32(E,F,G,H)) << 32))

#define EfiCopyMem(_Destination, _Source, _Length)  gBS->CopyMem ((_Destination), (_Source), (_Length))
#define EfiSetMem(_Destination, _Length, _Value)   gBS->SetMem  ((_Destination), (_Length), (_Value))
#define EfiZeroMem(_Destination, _Length)          gBS->SetMem  ((_Destination), (_Length), 0)

#define EFI_TPL_NOTIFY TPL_NOTIFY
#define EFI_TPL_CALLBACK TPL_CALLBACK
#define EFI_TPL_HIGH_LEVEL TPL_HIGH_LEVEL

#define EFI_CHECK(x) {EFI_STATUS status = (x);if(status!=EFI_SUCCESS)return status;}
#define COUNTOF(x) (sizeof(x)/sizeof((x)[0]))

#define EfiAppendDevicePathNode             DPAddNode
// redefined in core 4.05 #define DPLENGTH(x)                       ((x).Length)

void cp( UINT8 code);

VOID    USB_SmiQueuePut(VOID*);

EFI_STATUS  USBSB_InstallSmiEventHandlers(VOID);
EFI_STATUS	USBSB_InstallXhciHwSmiHandler(VOID);
EFI_STATUS	USBSB_InstallUsbIntTimerHandler(VOID);
EFI_STATUS	USBSB_UninstallTimerHandlers(VOID);

#endif      // __AMI_H

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
