#ifndef _DRIVER_NAME_LIB_H_
#define _DRIVER_NAME_LIB_H_

#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/LoadedImage.h>
#include <Library/PrintLib.h>

extern EFI_SYSTEM_TABLE                         *gDxeCoreST;

CHAR8* GetDriverName(EFI_DRIVER_BINDING_PROTOCOL *pDriver);
BOOLEAN GetImageNameByHandle(EFI_HANDLE ImageHandle, CHAR8 *sName, UINTN size);
#endif
