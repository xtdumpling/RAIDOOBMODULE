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
/** @file
  ProcessorSubClass definitions.
*/

#ifndef __PROCESSOR_SUBCLASS_H__
#define __PROCESSOR_SUBCLASS_H__

#include <Efi.h>
#include <DataHubSubClass.h>

// Include files from EDKII
// IntelFrameworkPkg:
#include <Guid/DataHubRecords.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EFI_PROCESSOR_CLASS EFI_DATA_CLASS_DATA

//Core Frequency
#define EFI_PROCESSOR_FREQUENCY_RECORD_NUMBER		0x00000001

//Front Side Bus Frequency
#define EFI_PROCESSOR_BUS_FREQUENCY_RECORD_NUMBER	0x00000002

//Version
#define EFI_PROCESSOR_VERSION_RECORD_NUMBER			0x00000003

//Manufacturer
#define EFI_PROCESSOR_MANUFACTURER_RECORD_NUMBER	0x00000004

//Serial Number
#define EFI_PROCESSOR_SERIAL_NUMBER_RECORD_NUMBER	0x00000005

//ID
#define EFI_PROCESSOR_ID_RECORD_NUMBER				0x00000006

//Type
#define EFI_PROCESSOR_TYPE_RECORD_NUMBER			0x00000007

//Family
#define EFI_PROCESSOR_FAMILY_RECORD_NUMBER			0x00000008
typedef EFI_PROCESSOR_FAMILY_DATA EFI_PROCESSOR_TYPE;

//Volatage
#define EFI_PROCESSOR_VOLTAGE_RECORD_NUMBER			0x00000009

//Processor APIC ID
#define EFI_PROCESSOR_APIC_ID_RECORD_NUMBER			0x0000000B

//APIC Version Number
#define EFI_PROCESSOR_APIC_VER_NUMBER_RECORD_NUMBER	0x0000000C

//Microcode Update Revision
#define EFI_PROCESSOR_MICROCODE_REVISION_RECORD_NUMBER	0x0000000D

//Processor Status
#define EFI_PROCESSOR_STATUS_RECORD_NUMBER			0x0000000E

//Socket Type
#define EFI_PROCESSOR_SOCKET_TYPE_RECORD_NUMBER		0x0000000F

//Socket Name
#define EFI_PROCESSOR_SOCKET_NAME_RECORD_NUMBER		0x00000010

//Cache Association
#define EFI_SUBCLASS_ASSOCIATION_RECORD_NUMBER		0x00000011

//Processor Maximum Frequency
#define EFI_PROCESSOR_MAX_FREQUENCY_RECORD_NUMBER	0x00000012
typedef EFI_EXP_BASE10_DATA EFI_PROCESSOR_MAX_FREQUENCY_DATA;

//Processor Asset Tag
#define EFI_PROCESSOR_ASSET_TAG_RECORD_NUMBER		0x000000013

//Processor Maximum Bus Frequency
#define EFI_PROCESSOR_MAX_FSB_FREQUENCY_RECORD_NUMBER	0x000000014

//Processor Package Number
#define EFI_PROCESSOR_PACKAGE_NUMBER_RECORD_NUMBER	0x00000015

//Core Frequency List
#define EFI_PROCESSOR_FREQUENCY_LIST_RECORD_NUMBER	0x00000016

//Front Side Bus Frequency List
#define EFI_PROCESSOR_BUS_FREQUENCY_LIST_RECORD_NUMBER	0x00000017


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
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
