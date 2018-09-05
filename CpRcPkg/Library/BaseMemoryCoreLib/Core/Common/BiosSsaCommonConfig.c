/** @file
  This file contains the BIOS implementation of the BIOS-SSA Common Configuration API.

@copyright
  Copyright (c) 2016 Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#include "SysFunc.h"

#ifndef MINIBIOS_BUILD
#include "Library/IoLib.h"  //MmioRead8, etc
#include "Library/BaseMemoryLib.h"
#include "Library/HobLib.h"
#endif  //MINIBIOS_BUILD

#ifdef SSA_FLAG
/**
  Reads a variable-sized value from a memory mapped register using an absolute address.
  This function takes advantage of any caching implemented by BIOS.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width   - The size of the value to write.
  @param[in]      Address - Address of the register to be accessed.
  @param[out]     Buffer  - Value storage location.

  @retval Nothing.
**/
VOID
(EFIAPI BiosReadMem) (
  SSA_BIOS_SERVICES_PPI *This,
  REG_WIDTH             Width,
  EFI_PHYSICAL_ADDRESS  Address,
  VOID                  *Buffer
  )
{
#ifndef MINIBIOS_BUILD
  struct sysHost            *host;
  MMIO_BUFFER               *MmioBuffer;

  MmioBuffer = (MMIO_BUFFER *) Buffer;
  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosReadMem (Width:%d, Address:0x%x)\n", Width, Address));
#endif

  switch (Width) {
    case RegWidth8:
      MmioBuffer->Data8 = MmioRead8 ((UINT32) Address);
      break;

    case RegWidth16:
      MmioBuffer->Data16 = MmioRead16 ((UINT32) Address);
      break;

    case RegWidth32:
      MmioBuffer->Data32 = MmioRead32 ((UINT32) Address);
      break;

    case RegWidth64:
      MmioBuffer->Data64 = MmioRead64 ((UINT32) Address);
      break;

    default:
      break;
  }
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosReadMem\n"));
#endif
#endif  //MINIBIOS_BUILD
}

/**
  Writes a variable sized value to a memory mapped register using an absolute address.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width   - The size of the value to write.
  @param[in]      Address - Address of the register to be accessed.
  @param[in]      Buffer  - Value to write.

  @retval Nothing.
**/
VOID
(EFIAPI BiosWriteMem) (
  SSA_BIOS_SERVICES_PPI *This,
  REG_WIDTH             Width,
  EFI_PHYSICAL_ADDRESS  Address,
  VOID                  *Buffer
  )
{
#ifndef MINIBIOS_BUILD
  struct sysHost            *host;
  MMIO_BUFFER               *MmioBuffer;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosWriteMem  (Width:%d, Address:%d)\n", Width, Address));
#endif

  MmioBuffer = (MMIO_BUFFER *) Buffer;
  switch (Width) {
    case RegWidth8:
      MmioWrite8 ((UINT32) Address, MmioBuffer->Data8);
      break;

    case RegWidth16:
      MmioWrite16 ((UINT32) Address, MmioBuffer->Data16);
      break;

    case RegWidth32:
      MmioWrite32 ((UINT32) Address, MmioBuffer->Data32);
      break;

    case RegWidth64:
      MmioWrite64 ((UINT32) Address, MmioBuffer->Data64);
      break;

    default:
      break;
  }
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosWriteMem\n"));
#endif
#endif  //MINIBIOS_BUILD
}

/**
  Reads a variable sized value from I/O.
  This function takes advantage of any caching implemented by BIOS.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width   - The size of the value to write.
  @param[in]      Address - Address of the I/O to be accessed.
  @param[out]     Buffer  - Value storage location.

  @retval Nothing.
**/
VOID
(EFIAPI BiosReadIo) (
  SSA_BIOS_SERVICES_PPI *This,
  REG_WIDTH             Width,
  UINT32                Address,
  VOID                  *Buffer
  )
{
#ifndef MINIBIOS_BUILD
  struct sysHost            *host;
  IO_BUFFER                 *IoBuffer;

  IoBuffer = (IO_BUFFER *) Buffer;
  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosReadIo (Width:%d, Address:0x%x)\n", Width, Address));
#endif

  switch (Width) {
    case RegWidth8:
      IoBuffer->Data8 = IoRead8 ((UINT16) Address);
      break;

    case RegWidth16:
      IoBuffer->Data16 = IoRead16 ((UINT16) Address);
      break;

    case RegWidth32:
      IoBuffer->Data32 = IoRead32 ((UINT16) Address);
      break;

    default:
      break;
  }
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosReadIo\n"));
#endif
#endif //MINIBIOS_BUILD
}

/**
  Writes a variable sized value to I/O.

  @param[in, out] This    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width   - The size of the value to write.
  @param[in]      Address - Address of the I/O to be accessed.
  @param[in]      Buffer  - Value to write.

  @retval Nothing.
**/
VOID
(EFIAPI BiosWriteIo) (
  SSA_BIOS_SERVICES_PPI *This,
  REG_WIDTH             Width,
  UINT32                Address,
  VOID                  *Buffer
  )
{
#ifndef MINIBIOS_BUILD
  struct sysHost            *host;
  IO_BUFFER                 *IoBuffer;

  IoBuffer = (IO_BUFFER *) Buffer;
  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosWriteIo (Width:%d, Address:%d)\n", Width, Address));
#endif

  switch (Width) {
    case RegWidth8:
      IoWrite8 ((UINT16) Address, IoBuffer->Data8);
      break;

    case RegWidth16:
      IoWrite16 ((UINT16) Address, IoBuffer->Data16);
      break;

    case RegWidth32:
      IoWrite32 ((UINT16) Address, IoBuffer->Data32);
      break;

    default:
      break;
  }
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosWriteIo\n"));
#endif
#endif //MINIBIOS_BUILD
}

/**
  Reads a variable sized value from the PCI config space register.
  This function takes advantage of any caching implemented by BIOS.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width      - The size of the value to write.
  @param[in]      Address    - Address of the I/O to be accessed. Must be modulo 'Width'.
  @param[out]     Buffer     - Value storage location.

  @retval Nothing.
**/
VOID
(EFIAPI BiosReadPci) (
  SSA_BIOS_SERVICES_PPI           *This,
  REG_WIDTH                       Width,
  EFI_PEI_PCI_CFG_PPI_PCI_ADDRESS *Address,
  VOID                            *Buffer
  )
{
#ifndef MINIBIOS_BUILD
  UINT32               Value;
  struct sysHost       *host;
  PCI_BUFFER           *PciBuffer;
  PCI_CONFIG_SPACE     PciAddress;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosReadPci (Width:%d, Address:0x%x)\n", Width, Address));
#endif

  PciBuffer                 = (PCI_BUFFER *) Buffer;
  PciAddress.Value          = Address->Register; // 8 bit register offset modifed with other Union bits
  PciAddress.Bits.Bus       = Address->Bus;// 8 bit
  PciAddress.Bits.Device    = Address->Device;// 8 bit
  PciAddress.Bits.Function  = Address->Function;// 8 bit
  PciAddress.Bits.Reserved0 = 0;
  PciAddress.Bits.Enable    = 1;

  IoWrite32 (0x0CF8, PciAddress.Value); // We pass the 32bit ADDRESS to PciIndex = 0x0CF8
  Value = IoRead32 (0x0CFC);//Read value at address stored in CF8. PciData = 0x0CFC
  switch (Width) {
    case RegWidth8:
      PciBuffer->Data8 = (UINT8) Value;
      break;

    case RegWidth16:
      PciBuffer->Data16 = (UINT16) Value;
      break;

    case RegWidth32:
      PciBuffer->Data32 = Value;
      break;

    default:
      break;
  }
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosReadPci\n"));
#endif
#endif //MINIBIOS_BUILD
}

/**
  Writes a variable sized value to the PCI config space register.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width      - The size of the value to write.
  @param[in]      Address    - Address of the I/O to be accessed. Must be modulo 'Width'.
  @param[in]      Buffer     - Value to write.

  @retval Nothing.
**/
VOID
(EFIAPI BiosWritePci) (
  SSA_BIOS_SERVICES_PPI           *This,
  REG_WIDTH                       Width,
  EFI_PEI_PCI_CFG_PPI_PCI_ADDRESS *Address,
  VOID                            *Buffer
  )
{
#ifndef MINIBIOS_BUILD
  PCI_BUFFER        *PciBuffer;
  struct sysHost    *host;
  PCI_CONFIG_SPACE  PciAddress;
  BOOLEAN           DoIt;
  UINT32            Value;
  UINT32            Value1;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosWritePci (Width:%d, Address:0x%x)\n", Width, Address));
#endif

  PciBuffer                 = (PCI_BUFFER *) Buffer;
  DoIt                      = TRUE;

  PciAddress.Value          = Address->Register; // 8 bit register OR offset/ value = 32 bits
  PciAddress.Bits.Bus       = Address->Bus;// 8 bit
  PciAddress.Bits.Device    = Address->Device;// 8 bit
  PciAddress.Bits.Function  = Address->Function;// 8 bit
  PciAddress.Bits.Reserved0 = 0;
  PciAddress.Bits.Enable    = 1;
  Value                     = 0;

  switch (Width) {
    case RegWidth8:
      BiosReadPci (This, RegWidth32, Address, (PCI_BUFFER *) &Value);
      Value &= ~0xFF;  // doing a type conversion to 8bit as Value is 32 bit
      Value |= PciBuffer->Data8;
      break;

    case RegWidth16:
      BiosReadPci (This, RegWidth32, Address, (PCI_BUFFER *) &Value);
      Value &= ~0xFFFF;  // doing a type conversion to 16bit as Value is 32 bit
      Value |= PciBuffer->Data16;
      break;

    case RegWidth32:
      Value = PciBuffer->Data32;
      break;

    default:
      Value = 0;
      DoIt  = FALSE;
      break;
  }

  if (DoIt) {
      IoWrite32 (0x0CF8, PciAddress.Value); //8 bit offset written to IO address port
      IoWrite32 (0xCFC, Value); //Value written to location pointed by above 8 bit offset
  }
 //  Value1 = IoRead32 (0x0CFC);  //Read value at address stored in CF8. PciData = 0x0CFC
  IoWrite32 (0x0CF8, PciAddress.Value); // We pass the 32bit ADDRESS to PciIndex = 0x0CF8
  Value1 = IoRead32 (0x0CFC);//Read value at address stored in CF8. PciData = 0x0CFC
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosWritePci\n"));
#endif
#endif //MINIBIOS_BUILD
}

/**
  Gets a base address to be used in the different memory map or MMIO register access functions.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Index           - Additional index to locate the register.
  @param[in]      BaseAddressType - Value that indicates the type of base address to be retrieved.
  @param[out]     BaseAddress     - Where to write the base address

  @retval NotAvailable if the system does not support this feature.  Else UnsupportedValue if BaseAddressType is out of range.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosGetBaseAddress) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Index,
  BASE_ADDR_TYPE        BaseAddressType,
  EFI_PHYSICAL_ADDRESS  *BaseAddress
  )
{
  return NotAvailable;  //Function not available in server MRC
}

/**
  Function used to dynamically allocate memory.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Size - Amount of memory in bytes to allocate.

  @retval Returns a pointer to an allocated memory block on success or NULL on failure.
**/
VOID *
(EFIAPI BiosMalloc) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                Size
  )
{
  UINT32              *bufferBase;
  struct sysHost      *host;
  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosMalloc (Size:%d)\n", Size));
#endif

  if ((Size == 0) || (host->var.common.heapSize == 0)) {
    return NULL;
  }

 //Calling rcMalloc in MRC
  bufferBase = rcMalloc (host, Size);

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosMalloc buffer:0x%x\n", bufferBase));
#endif
  return bufferBase;   // return the base of the memory range allocated.
}

/**
  Function used to release memory allocated using Malloc.

  @param[in, out] This   - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Buffer - The buffer to return to the free pool.

  @retval Nothing.
**/
VOID
(EFIAPI BiosFree) (
  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *Buffer
  )
{
  struct sysHost             *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosFree(0x%x)\n", Buffer));
#endif
  rcFree (host, Buffer);  //Calling rcfree() in MRC
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosFree\n"));
#endif
}

/**
  Function used to output debug messages to the output logging device.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      PrintLevel   - The severity level of the string.
  @param[in]      FormatString - The reduced set of printf style format specifiers.
  @param          %[flags][width]type
  @param          [flags] '-' left align
  @param          [flags] '+' prefix with sign (+ or -)
  @param          [flags] '0' zero pad numbers
  @param          [flags] ' ' prefix blank in front of postive numbers
  @param          [width] non negative decimal integer that specifies the width to print a value.
  @param          [width] '*' get the width from a int argument on the stack.
  @param          type    'd'|'i' signed decimal integer
  @param          type    'u' unsigned integer
  @param          type    'x'|'X' hexadecimal using "ABCDEF"
  @param          type    'c' print character
  @param          type    'p' print a pointer to void
  @param          type    's' print a null terminated string
  @param[in]      ...          - Variable list of output values.

  @retval Nothing.
**/
VOID
(EFIAPI BiosSsaDebugPrint) (
  SSA_BIOS_SERVICES_PPI *This,
  PRINT_LEVEL           PrintLevel,
  UINT8                 *FormatString,
  ...
  )
{
  va_list arguments;

  struct sysHost *host;
  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;

  va_start(arguments, FormatString);

#ifdef SERIAL_DBG_MSG
  if (FormatString != NULL) {
#ifndef MINIBIOS_BUILD
    rcVprintf(host, FormatString, arguments);
#else
    rcVprintf(host, (const INT8 *)FormatString, arguments);
#endif
  }
#endif //SERIAL_DBG_MSG

  return;
}

/**
  Function used to stall the software progress for the given period of time.

  @param[in, out] This  - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Delay - The amount of time to wait, in microseconds.

  @retval Nothing.
**/
VOID
(EFIAPI BiosWait) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                Delay
  )
{
  struct sysHost        *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "\nStart: BiosWait()\n"));
#endif
  FixedDelay (host, Delay);
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosWait\n"));
#endif
}

/**
  Function used to add data to the BDAT v4 or later structure that will be created by BIOS in reserved physical memory.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      SchemaGuid  - The Schema ID GUID.  This is a unique identifier that indicates the format of the data to be added to the BDAT structure.
  @param[in]      Data        - The location of the data block to add to the BDAT structure.
  @param[in]      DataSize    - The size of the data to be added to the BDAT structure in bytes.

  @retval NotAvailable if the system does not support this feature.  Else UnsupportedValue if there is insufficient memory to add the data to the structure.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosSaveToBdat) (
  SSA_BIOS_SERVICES_PPI *This,
  EFI_GUID              *SchemaGuid,
  CONST UINT8           *Data,
  UINT32                DataSize
  )
{
#if defined(SSA_LOADER_FLAG) && (!defined(RC_SIM) && !defined(MINIBIOS_BUILD)) && defined(BDAT_SUPPORT)
  struct sysHost                    *host;
  UINT8                             guidIdx = 0;
  BOOLEAN                           GuidAlreadyExists = FALSE;
  VOID                              *HobPtr = NULL;
  UINT32                            RealHobSize;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
  host->var.common.HobPtrSsaResults = 0;

  if (DataSize > MAX_HOB_ENTRY_SIZE) {  //If Data Size of Blob is > 60kB exit
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
    rcPrintf ((host, "Datasize is biger tha max HOB size\n"));
#endif
    return UnsupportedValue;
  }

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "\nStart: BiosSaveToBdat (DataSize:%d, Data Location:0x%x)\n", DataSize, Data));
#endif

  //Checking if this guid already exists, if yes, we update the number of hobs with the same guid
  for (guidIdx = 0; guidIdx < host->var.mem.bssaNumberDistinctGuids; guidIdx++) {
    if (CompareGuid(&host->var.mem.bssaBdatGuid[guidIdx], SchemaGuid)) { //if not new guid, enter and update hobs per guid
      if (host->var.mem.bssaNumberHobs[guidIdx] > 254) {
        //Run out of space to store this guid. Shouldn't happen because we shouldn't have enough memory to hit this.
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
        rcPrintf ((host, "Too many HOBS for a single guid.\n"));
#endif //ENBL_BIOS_SSA_DEBUG_MSGS
        return UnsupportedValue;
      } else {
        host->var.mem.bssaNumberHobs[guidIdx] += 1;
        GuidAlreadyExists = TRUE;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
        rcPrintf ((host, "Guid found in table. guidIdx: %d, numhobs: %d\n", guidIdx, host->var.mem.bssaNumberHobs[guidIdx]));
#endif //ENBL_BIOS_SSA_DEBUG_MSGS
        break;
      }
    }
  }

  //Trying to add the guid; for a new guid we update "bssaNumberDistinctGuids", to indicate no. of unique guids
  if (!GuidAlreadyExists) {
    if (host->var.mem.bssaNumberDistinctGuids >= MAX_NUMBER_SSA_BIOS_RESULTS) {
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
      rcPrintf ((host, "Reached max number of unique Guids\n"));
#endif //ENBL_BIOS_SSA_DEBUG_MSGS
      return UnsupportedValue;
    } else {
      host->var.mem.bssaBdatGuid[host->var.mem.bssaNumberDistinctGuids] = *SchemaGuid;
      host->var.mem.bssaNumberHobs[host->var.mem.bssaNumberDistinctGuids] = 1;
      host->var.mem.bssaNumberDistinctGuids += 1;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
      rcPrintf ((host, "New Guid found. guidIdx: %d, numhobs: %d\n", host->var.mem.bssaNumberDistinctGuids, host->var.mem.bssaNumberHobs[host->var.mem.bssaNumberDistinctGuids]));
#endif //ENBL_BIOS_SSA_DEBUG_MSGS
    }
  }

  //Create the HOB
  HobPtr = BuildGuidDataHob (SchemaGuid, (UINT8 *)Data, DataSize);
  if (HobPtr == NULL) {
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
    rcPrintf ((host, "Couldn't create HOB\n"));
    if (GuidAlreadyExists) {
      host->var.mem.bssaNumberHobs[guidIdx] -= 1;
    } else {
      host->var.mem.bssaNumberDistinctGuids -= 1;
      host->var.mem.bssaNumberHobs[host->var.mem.bssaNumberDistinctGuids] = 0;
    }
#endif //ENBL_BIOS_SSA_DEBUG_MSGS
    return UnsupportedValue;
  }

  RealHobSize = DataSize;
  if ((RealHobSize % 8) != 0) {
    //BuildGuidDataHob function description states that the size of the created GUID is
    //multiple of 8. Correcting size to match this.
    RealHobSize += (8 - (RealHobSize % 8));
  }

  host->var.mem.bssaBdatSize  += RealHobSize; //Updating total size of all HOBs being created

  host->var.common.HobPtrSsaResults = (UINT32)HobPtr;

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "HOB location: 0x%x\n", HobPtr));
  rcPrintf ((host, "End: BiosSaveToBdat\n"));
#endif //ENBL_BIOS_SSA_DEBUG_MSGS
  return Success;
#else //!defined(SSA_LOADER_FLAG) && (!defined(RC_SIM) && !defined(MINIBIOS_BUILD)) && defined(BDAT_SUPPORT)
  return NotAvailable;
#endif //!defined(SSA_LOADER_FLAG) && (!defined(RC_SIM) && !defined(MINIBIOS_BUILD)) && defined(BDAT_SUPPORT)
}

/**
  This function to register a new handler for a hook.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      HookGuid    - GUID defining the hook to invoke the handlers for.
  @param[in]      HookHandler - Function pointer to the new hook handler. This hook handler will be invoked whenever a corresponding call to RunHook() is made with the correct hook GUID.

  @retval NotAvailable if the system does not support this feature.  Else UnsupportedValue if HookGuid or HookHandler is NULL or HookGuid is invalid.  Else Failure if unable to allocate a buffer to store the hook.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosRegisterHookHandler) (
  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *HookGuid,
  SSA_BIOS_HOOK_HANDLER HookHandler
  )
{
  return NotAvailable;
}

/**
  This function invokes all registered handlers for a specific hook.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      HookGuid    - GUID defining the hook to invoke the handlers for.
  @param[in]      Parameters  - Data passed to the hook handlers. The type and format of this data differs depending on what hook is being invoked.  The type and format of the data must be the same as all other cases where RunHook() is called with a specific hook GUID.  This value can be NULL if the hook does not pass any additional data.

  @retval NotAvailable if the system does not support this feature.  Else UnsupportedValue if HookGuid is NULL or invalid.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosRunHook) (
  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *HookGuid,
  VOID                  *Parameters
  )
{
  return NotAvailable;
}

/**
  This function saves a pointer into a table that can later be retrieved.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      PointerId   - GUID that uniquely identifies the pointer.
  @param[in]      Pointer     - The pointer to save.

  @retval NotAvailable if the system does not support this feature.  Else UnsupportedValue if PointerId or Pointer is NULL or PointerId is invalid.  Else Failure if unable to allocate a buffer to store the pointer.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosSaveDataPointer) (
  SSA_BIOS_SERVICES_PPI *This,
  EFI_GUID              *PointerId,
  VOID                  *Pointer
  )
{
  return NotAvailable;
}

/**
  This function returns a pointer that was earlier saved using SaveDataPointer function.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      PointerId   - GUID that uniquely identifies the pointer.

  @retval The value of the pointer or NULL if the pointer for the specified GUID does not exist.
**/
VOID *
(EFIAPI BiosRestoreDataPointer) (
  SSA_BIOS_SERVICES_PPI *This,
  EFI_GUID              *PointerId
  )
{
  return NULL;
}

/**
  This function disables the watchdog timer.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.

  @retval Success when the watchdog timer is successfully disabled or if the platform does not have a watchdog timer.  Else Failure.
**/
VOID
(EFIAPI BiosDisableWatchdogTimer) (
  SSA_BIOS_SERVICES_PPI *This
  )
{
  //NotAvailable
}

/**
  Function used to get the elapsed time since the last CPU reset in milliseconds.

  @param[in, out] This      - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     Timestamp - Pointer to were the elapsed time since the last CPU reset in milliseconds will be written.

  @retval Nothing.
**/
VOID
(EFIAPI BiosGetTimestamp) (
  SSA_BIOS_SERVICES_PPI    *This,
  UINT32                   *Timestamp
  )
{
  struct sysHost             *host;
  UINT64_STRUCT startTsc  = {0, 0};  //Current time
  UINT64_STRUCT endTsc    = {0, 0};  //Time at last CPU reset = 0

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "Start: BiosGetTimestamp\n"));
#endif

  ReadTsc64 (&endTsc);  //Current timestamp

  *Timestamp = TimeDiff(host, startTsc, endTsc, TDIFF_UNIT_MS); //Converting current TimeStamp to MS (Time Difference since the last CPU reset() and current time)

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosGetTimestamp\n"));
#endif
}

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
// end file BiosSsaCommonConfig.c
