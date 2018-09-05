//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file KbcEmul.h
    KBC emulation header file
**/

//---------------------------------------------------------------------------
#include <Efi.h>
#include <AmiDxeLib.h>
#include "Token.h"
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmUsbDispatch2.h>
#include <Protocol/Emul6064MsInput.h>
#include <Protocol/Emul6064KbdInput.h>
#include <Protocol/Emul6064Trap.h>
#include <Protocol/SmmCpu.h>
#include "Usb/Rt/UsbDef.h"
#include "Protocol/AmiUsbController.h"

//---------------------------------------------------------------------------

#define _CR(Record, TYPE, Field)  ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))

#define SCROLL_SUPPORT_MOUSE_ID     0x03
#define MOUSE_4BYTE_DATA_BIT_MASK   0x10

/*#define  MAX_DEBUG_LEVEL        8

#define DEBUG_LEVEL_8           8
#define DEBUG_LEVEL_7           7
#define DEBUG_LEVEL_6           6
#define DEBUG_LEVEL_5           5
#define DEBUG_LEVEL_4           4
#define DEBUG_LEVEL_3           3
#define DEBUG_LEVEL_2           2
#define DEBUG_LEVEL_1           1
#define DEBUG_LEVEL_0           0*/

typedef struct _HC_DEV_STRUC {
    UINT16          wPCIDev;
    UINT16          wHCType;
} HC_DEV_STRUC;

extern EFI_SMM_SYSTEM_TABLE2 *gSmst1;

//
//Checks if Trap status is set
//
BOOLEAN     SmmGetTrapStatus();
BOOLEAN     TrapEnable(BOOLEAN);

#if OHCI_EMUL_SUPPORT == 1
VOID    SetHceOutput (UINT8);
#endif

#if PI_SPECIFICATION_VERSION >= 0x0001000A
extern EFI_SMM_CPU_PROTOCOL            *SmmCpuProtocol;
#endif
EFI_STATUS
RegisterAcpiEnableCallBack();

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

