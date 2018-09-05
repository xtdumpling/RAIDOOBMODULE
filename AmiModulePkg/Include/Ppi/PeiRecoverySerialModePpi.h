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
/** @file
  @brief The file contains definition of the Serial Recovery Mode PPI.
**/

#ifndef __PEI_RECOVERY_SERIAL_MODE_PPI__H__
#define __PEI_RECOVERY_SERIAL_MODE_PPI__H__

#define PEI_RECOVERY_SERIAL_MODE_PPI_GUID \
  { 0x5e7063d3, 0xc12, 0x475b, { 0x98, 0x35, 0x14, 0xab, 0xb1, 0xcb, 0xe, 0xe9 } }

typedef struct _PEI_RECOVERY_SERIAL_MODE_PPI {
	UINT64  SerialDeviceBaseAddress;
} PEI_RECOVERY_SERIAL_MODE_PPI;

/// GUID Variable Declaration
extern EFI_GUID gSerialRecoveryDevicePpiGuid;

#endif

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
