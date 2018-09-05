  /**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**
Copyright (c) 2006 - 2017, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file UpdateASL.c

  This code updates ASL code that supports SPS firmware in ME.

**/
#include <PiDxe.h>
    
#include <Guid/GlobalVariable.h>
#include <Guid/EventLegacyBios.h>
#include <Guid/HobList.h>
#include <Guid/SpsInfoHobGuid.h>
#include <Guid/SpsAcpiTableLocatorGuid.h>
#include <Sps.h>
#include <MeAccess.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include "Library/MmPciBaseLib.h"


#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/AcpiAml.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/CpuIo.h>
#include <Protocol/MpService.h>
#include "MeChipset.h"
#include "HeciRegs.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/
#define AML_STA_PRESENT    0x01
#define AML_STA_ENABLED    0x02
#define AML_STA_VISIBLE    0x04
#define AML_STA_FUNCTIONAL 0x08
#define AML_STA_BATTERY    0x10

#define AML_REGTYPE_SYSMEM 0
#define AML_REGTYPE_SYSIO  1
#define AML_REGTYPE_PCICFG 2
#define AML_REGTYPE_EMBEDD 3
#define AML_REGTYPE_SMBUS  4
#define AML_REGTYPE_CMOS   5
#define AML_REGTYPE_PCIBAR 6
#define AML_REGTYPE_IPMI   7
#define AML_REGTYPE_GPIO   8
#define AML_REGTYPE_GSBUS  9

#define AML_SIGNATURE_16(A, B)        ((A) | (B << 8))
#define AML_SIGNATURE_32(A, B, C, D)  (AML_SIGNATURE_16(A, B) | (AML_SIGNATURE_16(C, D) << 16))
#define AML_SIGNATURE_64(A, B, C, D, E, F, G, H) \
       (AML_SIGNATURE_32(A, B, C, D) | ((UINT64)(AML_SIGNATURE_32(E, F, G, H)) << 32))

#define SPS_ASL_SIG_NVS  AML_SIGNATURE_64('S', 'p', 's', 'N', 'v', 's', ' ', ' ')
#define SPS_ASL_SIG_NM   AML_SIGNATURE_64('S', 'p', 's', 'N', 'm', ' ', ' ', ' ')

#ifndef EFI_ACPI_CREATOR_ID
#define EFI_ACPI_CREATOR_ID       0x4C544E49          // "INTL"
#define EFI_ACPI_CREATOR_REVISION 0x20091013          // Oct 13 2009
#endif

#define GetHeci1Bar() ((MmioRead32(MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + 0x10) & ~0xF) |\
                        (UINT64)MmioRead32(MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + 0x14) << 32)
#define GetHeci2Bar() ((MmioRead32(MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + 0x10) & ~0xF) |\
                        (UINT64)MmioRead32(MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + 0x14) << 32)


/*****************************************************************************
 * Prototypes
 *****************************************************************************/


/*****************************************************************************
 * Variables
 *****************************************************************************/


/*****************************************************************************
 * Functions
 *****************************************************************************/

/*****************************************************************************
 @brief
  Load AML files required to support SPS firmware.

  @param[in] TBD
  @param[in] TBD

  @return Standard status codes are returned.
**/
EFI_STATUS
EFIAPI
SpsUpdateAslForMe(
  IN EFI_HANDLE                  ImageHandle,
  IN EFI_SYSTEM_TABLE           *pSystemTable)
{
  EFI_STATUS                    Status;
  SPS_NMFS                      NmFs;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *pFwVolProtocol;
  EFI_HANDLE                    *pHandleBuffer;
  UINTN                         NumberOfHandles, ii;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  INTN                          Instance;
  EFI_ACPI_DESCRIPTION_HEADER   *pAcpiHeader;
  UINTN                         AcpiTableHandle;
  UINT32                        Signature;
  UINT8                         *pCurr;
  UINTN                         MpsThreads, MpsThreadsEnabled;
  EFI_MP_SERVICES_PROTOCOL      *pMpServices;
  EFI_GUID                      MpServicesGuid = EFI_MP_SERVICES_PROTOCOL_GUID;
  EFI_ACPI_TABLE_PROTOCOL       *pAcpiProtocol;
  BOOLEAN                       updateACPI = TRUE;

  DEBUG((DEBUG_INFO, "[SPS] ACPI Hook\n"));

  NmFs.UInt32 = Heci2PciRead32(SPS_REG_NMFS);
  if (NmFs.UInt32 == 0xFFFFFFFF || !NmFs.Bits.NmEnabled ||
     (Heci2PciRead8(R_HIDM) & HECI_INTERRUPUT_GENERATE_SCI) != HECI_INTERRUPUT_GENERATE_SCI)
  {
    //
    // If SPS Node Manager is not enabled in ME just return.
    // The NM objects in ACPI are defined inactive. If we return here
    // they will just remain inactive.
    //
    DEBUG((DEBUG_WARN, "[SPS] WARNING: Node Manager not enabled in ME\n"));
    updateACPI = FALSE;
  }
  //
  // Get ACPI protocol, will need it later
  //
  Status = gBS->LocateProtocol(&gEfiAcpiTableProtocolGuid, NULL, &pAcpiProtocol);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate ACPI protocol (%r)\n", Status));
    return Status;
  }
  //
  // Locate firmware volume handles.
  // There is little chance we can't find an FV protocol.
  //
  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiFirmwareVolume2ProtocolGuid, NULL,
                                   &NumberOfHandles, &pHandleBuffer);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR,
           "[SPS] ERROR: Cannot locate firmware volume handles (status: %r)\n", Status));
    return Status;
  }
  //
  // Looking for FV with SPS AML code file
  //
  pFwVolProtocol = NULL;
  for (ii = 0; ii < NumberOfHandles; ii++)
  {
    Status = gBS->HandleProtocol(pHandleBuffer[ii],
                                 &gEfiFirmwareVolume2ProtocolGuid, &pFwVolProtocol);
    if (!EFI_ERROR(Status))
    {
      //
      // See if it has the SPS AML file
      //
      Size      = 0;
      FvStatus  = 0;
      Status = pFwVolProtocol->ReadFile(pFwVolProtocol, &gEfiSpsAcpiTableLocatorGuid, NULL,
                                        &Size, &FileType, &Attributes, &FvStatus);
      //
      // If we found it, then we are done
      //
      if (!EFI_ERROR(Status))
      {
        break;
      }
    }
    else
    {
      ASSERT_EFI_ERROR(Status);
    }
  }
  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  //
  //
  // Free any allocated buffers
  //
  gBS->FreePool(pHandleBuffer);
  
  if (pFwVolProtocol == NULL)
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate SPS AML firmware volume\n"));
    return EFI_NOT_FOUND;
  }
  
  //
  // Read tables from the storage file.
  //
  for (Instance = 0, pAcpiHeader = NULL;
       !EFI_ERROR(pFwVolProtocol->ReadSection(pFwVolProtocol, &gEfiSpsAcpiTableLocatorGuid,
                                              EFI_SECTION_RAW, Instance,
                                              (VOID**)&pAcpiHeader, &Size, &FvStatus));
       Instance++, pAcpiHeader = NULL)
  {
    //
    // Check the table ID to modify the table
    //
    DEBUG((DEBUG_INFO, "[SPS] AML file '%c%c%c%c%c%c%c%c' found\n",
            ((pAcpiHeader->OemTableId >>  0) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >>  0) & 0xFF) : '.',
            ((pAcpiHeader->OemTableId >>  8) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >>  8) & 0xFF) : '.',
            ((pAcpiHeader->OemTableId >> 16) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >> 16) & 0xFF) : '.',
            ((pAcpiHeader->OemTableId >> 24) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >> 24) & 0xFF) : '.',
            ((pAcpiHeader->OemTableId >> 32) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >> 32) & 0xFF) : '.',
            ((pAcpiHeader->OemTableId >> 40) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >> 40) & 0xFF) : '.',
            ((pAcpiHeader->OemTableId >> 48) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >> 48) & 0xFF) : '.',
            ((pAcpiHeader->OemTableId >> 56) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >> 56) & 0xFF) : '.'));
    if (updateACPI)
    {
      switch (pAcpiHeader->OemTableId)
      {
        case SPS_ASL_SIG_NM:
          //
          // In this table need to update 'H2CR' region address with the actual HECI-2 MBAR address.
          // In case PCIBARTarget was used (currently not supported by several OSes) do not modify.
          //
          for (pCurr = (UINT8*)&pAcpiHeader[1];
               pCurr < (UINT8*)pAcpiHeader + pAcpiHeader->Length; pCurr++)
          {
            Signature = *(UINT32*)pCurr;
            switch (Signature)
            {
              case AML_SIGNATURE_32('H', '1', 'S', 'T'):
                //
                // HECI-1 status for _STA. Check if it is declaration, not a reference.
                //
                if (pCurr[4] == AML_BYTE_PREFIX)
                {
                  DEBUG((DEBUG_INFO, "[SPS] Updating 'H1ST' 0x%02X -> 0x%02X\n",
                        *(UINT8*)&pCurr[5], AML_STA_PRESENT | AML_STA_ENABLED | AML_STA_FUNCTIONAL));
                  *(UINT8*)&pCurr[5] = AML_STA_PRESENT | AML_STA_ENABLED | AML_STA_FUNCTIONAL;
                }
                break;
              case AML_SIGNATURE_32('H', '2', 'S', 'T'):
                //
                // HECI-2 status for _STA. Check if it is declaration, not a reference.
                //
                if (pCurr[4] == AML_BYTE_PREFIX)
                {
                  DEBUG((DEBUG_INFO, "[SPS] Updating 'H2ST' 0x%02X -> 0x%02X\n",
                        *(UINT8*)&pCurr[5], AML_STA_PRESENT | AML_STA_ENABLED | AML_STA_FUNCTIONAL));
                  *(UINT8*)&pCurr[5] = AML_STA_PRESENT | AML_STA_ENABLED | AML_STA_FUNCTIONAL;
                }
                break;
              case AML_SIGNATURE_32('H', '1', 'C', 'S'):
                //
                // HECI-1 CSR region. Check if it is declaration, not a reference.
                //
                if (pCurr[4] == AML_REGTYPE_SYSMEM && pCurr[5] == AML_QWORD_PREFIX)
                {
                  DEBUG((DEBUG_INFO, "[SPS] Updating 'H1CR' address 0x%08X%08X -> 0x%08X%08X\n",
                         *(UINT32*)&pCurr[10], *(UINT32*)&pCurr[6],
                         (UINT32)(GetHeci1Bar() >> 32), (UINT32)GetHeci1Bar()));
                  *(UINT64*)&pCurr[6] = GetHeci1Bar();
                }
                break;
              case AML_SIGNATURE_32('H', '2', 'C', 'S'):
                //
                // HECI-2 CSR region. Check if it is declaration, not a reference.
                //
                if (pCurr[4] == AML_REGTYPE_SYSMEM &&  pCurr[5] == AML_QWORD_PREFIX)
                {
                  DEBUG((DEBUG_INFO, "[SPS] Updating 'H2CR' address 0x%08X%08X -> 0x%08X%08X\n",
                         *(UINT32*)&pCurr[10], *(UINT32*)&pCurr[6],
                         (UINT32)(GetHeci2Bar() >> 32), (UINT32)GetHeci2Bar()));
                  *(UINT64*)&pCurr[6] = GetHeci2Bar();
                }
                break;
              case AML_SIGNATURE_32('T', 'H', 'N', 'U'):
                //
                // Processor threads number. Check if it is declaration, not a reference.
                //
                if (pCurr[4] == AML_WORD_PREFIX)
                {
                  //
                  // To get the number of enabled processor threads use MP Services protocol.
                  // In case MP Services API is not available, just take 1. There is at lease 1 thread.
                  //
                  MpsThreadsEnabled = 1;
                  Status = gBS->LocateProtocol(&MpServicesGuid, NULL, &pMpServices);
                  if (EFI_ERROR(Status))
                  {
                    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate MP Services protocol (%r)\n", Status));
                  }
                  else
                  {
                    Status = pMpServices->GetNumberOfProcessors(pMpServices, &MpsThreads, &MpsThreadsEnabled);
                    if (EFI_ERROR(Status))
                    {
                      DEBUG((DEBUG_ERROR, "[SPS] ERROR: GetNumberOfProcessors failed (%r)\n", Status));
                    }
                  }
                  DEBUG((DEBUG_INFO, "[SPS] Updating 'THNU' 0x%X -> %d\n",
                         *(UINT16*)&pCurr[5], MpsThreadsEnabled));
                  *(UINT16*)&pCurr[5] = (UINT16)MpsThreadsEnabled;
                }
                break;
            } // switch (Signature)
          } // for (pCurr...)
          break;

        default:
          DEBUG((DEBUG_ERROR, "[SPS] ERROR: Unknown AML file '%c%c%c%c%c%c%c%c' found\n",
                 ((pAcpiHeader->OemTableId >>  0) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >>  0) & 0xFF) : '.',
                 ((pAcpiHeader->OemTableId >>  8) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >>  8) & 0xFF) : '.',
                 ((pAcpiHeader->OemTableId >> 16) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >> 16) & 0xFF) : '.',
                 ((pAcpiHeader->OemTableId >> 24) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >> 24) & 0xFF) : '.',
                 ((pAcpiHeader->OemTableId >> 32) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >> 32) & 0xFF) : '.',
                 ((pAcpiHeader->OemTableId >> 40) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >> 40) & 0xFF) : '.',
                 ((pAcpiHeader->OemTableId >> 48) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >> 48) & 0xFF) : '.',
                 ((pAcpiHeader->OemTableId >> 56) & 0xFF) >= ' ' ? ((pAcpiHeader->OemTableId >> 56) & 0xFF) : '.'));
          goto SkippTable; // do not load unknown file
      }
    } // if (updateACPI)

    if (PcdGetBool(PcdUpdateSpsAcpiOemTableId))
    {
      UINT64  TempOemTableId;
      //
      // Update the OEMID and OEM Table ID.
      //
      TempOemTableId = PcdGet64(PcdAcpiDefaultOemTableId);

      CopyMem (pAcpiHeader->OemId, PcdGetPtr(PcdAcpiDefaultOemId), sizeof(pAcpiHeader->OemId));
      CopyMem (&pAcpiHeader->OemTableId, &TempOemTableId, sizeof(pAcpiHeader->OemTableId));

      //
      // Update the creator ID
      //
      pAcpiHeader->CreatorId = EFI_ACPI_CREATOR_ID;

      //
      // Update the creator revision
      //
      pAcpiHeader->CreatorRevision = EFI_ACPI_CREATOR_REVISION;
    }

    AcpiTableHandle = 0;
    DEBUG((DEBUG_INFO, "[SPS] Registering AML file in ACPI\n"));
    Status = pAcpiProtocol->InstallAcpiTable(pAcpiProtocol, pAcpiHeader, Size, &AcpiTableHandle);
    ASSERT_EFI_ERROR(Status);
    
   SkippTable:
    gBS->FreePool(pAcpiHeader);
  } // for (Instance...)

  return EFI_SUCCESS;
} // SpsUpdateAslForMe()


