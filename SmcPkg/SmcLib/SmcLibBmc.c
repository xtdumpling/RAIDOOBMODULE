//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.25
//    Bug Fix:  Update necessary code with newest SmcLibBmc.c
//    Reason:   
//    Auditor:  Durant Lin
//    Date:     Dec/21/2017
//
//  Rev. 1.24
//    Bug Fix:  Fix failed to apply BIOS CFG in stress test.
//    Reason:   
//    Auditor:  Jason Hsu
//    Date:     Jun/23/2017
//
//  Rev. 1.23
//    Bug Fix : N/A
//    Reason  : Implemented BIOS-based TPM provisioning finished flag for SUM.
//    Auditor : Joe Jhang
//    Date    : May/24/2017
//
//  Rev. 1.22
//    Bug Fix:  Add IPMI command retry.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     May/16/2017
//
//  Rev. 1.21
//    Bug Fix:  Fix BIOS settings cannot be applied by SUM command on some boards.
//    Reason:   VRAM's data doesn't sync to BMC's DRAM.
//    Auditor:  Jason Hsu
//    Date:     May/05/2017
//
//  Rev. 1.20
//    Bug Fix:  Add WDT timer out value selection feature.
//    Reason:   
//    Auditor:  Jimmy Chiu (Refer Greenlow - Mark Chen)
//    Date:     May/05/2017
//
//  Rev. 1.19
//    Bug Fix:  Set/Reset BMC WatchDog when setup item "Watch Dog Function" Enabled/Disabled.
//    Reason:   Prevent incorrect reset before WDT count down then cause the WDT still counting.
//    Auditor:  Jimmy Chiu
//    Date:     May/05/2017
//
//  Rev. 1.18
//    Bug Fix:  Support SmcRomHole preserve in IPMI.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/06/2017
//
//  Rev. 1.17
//    Bug Fix:  Send 0x30 0x70 0x70 command.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Dec/19/2016
//
//  Rev. 1.16
//    Bug Fix:  Correct command 0x30 0x70 0x3a data length.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Dec/15/2016
//
//  Rev. 1.15
//    Bug Fix:  Fix sum ChangeDmiInfo no function.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/02/2016
//
//  Rev. 1.14
//    Bug Fix:  Fix some errors for upload and download OOB files.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/20/2016
//
//  Rev. 1.13
//    Bug Fix:  Rewrite OOB download file code.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/19/2016
//
//  Rev. 1.12
//   Bug Fix:
//   Reason:	Fix WDT no function.
//    Auditor:  Kasber Chen
//    Date:     Sep/10/2016
//
//  Rev. 1.11
//   Bug Fix:
//   Reason:	Get CPLD version Command update by BMC
//    Auditor:  Sunny Yang
//    Date:     Sep/08/2016
//  Rev. 1.10
//   Bug Fix:   add CPLD version support
//   Reason:	Get CPLD version from BMC
//    Auditor:  Sunny Yang
//    Date:     Sep/01/2016
//
//  Rev. 1.09
//    Bug Fix:  Extend BIOS version string for BMC.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/25/2016
//
//  Rev. 1.08
//    Bug Fix:  Fix some errors in SmcOutBand module.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/10/2016
//
//  Rev. 1.07
//    Bug Fix:  Re-write SmcOutBand module.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/02/2016
//
//  Rev. 1.06
//    Bug Fix:  Review BMC OEM command
//    Reason:   Add SMC_IPMICmd30_99
//    Auditor:  Jimmy Chiu
//    Date:     Jul/28/2016
//
//  Rev. 1.05
//    Bug Fix:  Change OEM commands for IPMI video ram data transfer library.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/27/2016
//
//  Rev. 1.04
//    Bug Fix:  Add IPMI video ram data transfer library.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/27/2016
//
//  Rev. 1.03
//    Bug Fix:  Add SendBIOSVerBuildTimeDXE to sync BIOS version with SMBIOS Type 0.
//    Reason:   To avoid BIOS version not synchronize problem.
//    Auditor:  Jacker Yeh
//    Date:     Jul/19/2016
//
//  Rev. 1.02
//    Bug Fix:  Review BMC OEM command.
//    Reason:   New BMC command implemented.
//    Auditor:  Kasber Chen
//    Date:     Jul/11/2016
//
//  Rev. 1.01
//    Bug Fix:  Fix some IPMI commands didn't be sent.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jun/13/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     May/26/2016
//
//****************************************************************************
//****************************************************************************
#include "Setup.h"
#include "Token.h"
#include "EFI.h"
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/IPMITransportProtocol.h>
#include "TimeStamp.h"
#include <SuperMCommonDriver.h>
#include "SmcCspLibBmc.h"
#include "SmcLibBmcPrivate.h"
#include "SmcLibSmBios.h"
#include "SmcLibBmc.h"

#ifndef EFI_SM_INTEL_OEM
#define EFI_SM_INTEL_OEM    0x30
#endif

EFI_IPMI_TRANSPORT *mIpmiTransport = NULL;
UINT8* IVBar = NULL; // IPMI video ram memory pointer.
UINT8* AstMmioBar = NULL; // AST2500 MMIO bar pointer.
UINT8  AstRevision;
UINT32 VramData;
UINT32 *mP2AEnable;
UINT32 *mP2ARemapping;
UINT8  *mP2Abar;
UINT8  mDramConfig;
UINT32 mDramSize;
UINT32 mDramBase;
UINT32 mRemappingBase;

extern EFI_BOOT_SERVICES *gBS;
extern EFI_RUNTIME_SERVICES *gRT;

EFI_GUID    gEfiIpmiTransportProtocolGuid = EFI_DXE_IPMI_TRANSPORT_PROTOCOL_GUID;

EFI_STATUS
SmcLibLocateIpmiProtocol(VOID)
{
    EFI_STATUS	Status = EFI_SUCCESS;

    if(mIpmiTransport == NULL) {
        Status = gBS->LocateProtocol(
                     &gEfiDxeIpmiTransportProtocolGuid,
                     NULL,
                     &mIpmiTransport);
    }
    return Status;
}

EFI_STATUS
SmcLibBmcPostCpl(
    IN	BOOLEAN	BmcPostCpl
)
{
    SMC_IPMICmd30_70_70();
    SmcCspLibBmcPostCpl(TRUE);
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  EnterEnhanceMode
//
// Description:
//  Set AST2400 VGA enter Enhance Mode for data transfer with BMC.
//
// Input:
//      IN UINT16 IVIO -
//	    Primary Bus Number Register.
//
// Output:
//      EFI_STATUS - Return from IPMI command.
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EnterEnhanceMode(
    IN	UINT16	IVIO
)
{
    UINT8 b54, b55;

    IoWrite8(IVIO + 0x43, 0x01);
    IoWrite8(IVIO + 0x42, 0x0B);
    IoWrite8(IVIO + 0x54, 0x80);
    IoWrite8(IVIO + 0x55, 0xA8);

    IoWrite8(IVIO + 0x44, 0x04);
    IoWrite8(IVIO + 0x45, 0x08);
    IoWrite8(IVIO + 0x4E, 0x06);
    IoWrite8(IVIO + 0x4F, 0x01);

    IoWrite8(IVIO + 0x44, 0x02);
    IoWrite8(IVIO + 0x45, 0x0F);
    IoWrite8(IVIO + 0x54, 0xA1);
    IoWrite8(IVIO + 0x55, 0x04);

    IoWrite8(IVIO + 0x54, 0xA0);
    IoWrite8(IVIO + 0x55, 0x60);

    b54 = IoRead8(IVIO + 0x54);
    b55 = IoRead8(IVIO + 0x55);

    DEBUG ((-1, "VGA Enhance 0x%X 0x%X \n", b54, b55));

    IoWrite8(IVIO + 0x54, 0xA3);
    IoWrite8(IVIO + 0x55, 0x08);

    if(b54 == 0xA0 && b55 == 0x60) return EFI_SUCCESS;

    DEBUG ((-1, "EnterEnhanceMode : Can't enter enhance mode.\n"));

    return EFI_NOT_READY;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  GetIPMIVideoBar
//
// Description:
//  Find Aspeed VGA device video memory address.
//
// Input:
//	None.
//
// Output:
//      UINT8* - Pointer of IPMI video ram.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8* GetIPMIVideoBar(VOID)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8	bus;
    UINT32	SVID = 0x0;
    UINT16	IVIO = 0x0;

    if (IVBar == NULL) {

        //
        // Search for Aspeed VGA device number.
        //
        for(bus = 1; bus < 255 ; bus++) {
            SVID = PciRead32 (PCI_LIB_ADDRESS (bus, 0x0, 0x0, 0x0));
            if(SVID == IPMI_VGA_SVID) {
                IVBar = (UINT8*)PciRead32 (PCI_LIB_ADDRESS (bus, 0x0, 0x0, 0x10)) + IPMI_VIDEO_MEMORY_OFFSET;
                AstMmioBar = (UINT8*)PciRead32 (PCI_LIB_ADDRESS (bus, 0x0, 0x0, 0x14));
                AstRevision = PciRead8 (PCI_LIB_ADDRESS (bus, 0x0, 0x0, 0x08));
                IVIO = PciRead32 (PCI_LIB_ADDRESS (bus, 0x0, 0x0, 0x18)) & 0xFFF0;
                DEBUG ((-1, "GetIPMIVideoBar = 0x%x.\n", IVBar));
                break;
            }
        }

        if(SVID != IPMI_VGA_SVID) {
            DEBUG ((-1, "GetIPMIVideoBar : EFI_DEVICE_ERROR.\n"));
            IVBar = NULL;
            return NULL;
        }

        if(IVIO == 0x00 || IVIO == 0xFFF0) {
            DEBUG ((-1, "GetIPMIVideoBar : EFI_NOT_READY.\n"));
            IVBar = NULL;
            return NULL;
        }

#ifdef OOB_READ_DATA_FROM_DRAM
        if (mDramBase == 0 && AstMmioBar != NULL && AstRevision != 0) {
          mP2AEnable       = (UINT32 *) (AstMmioBar + 0xF000);
          mP2ARemapping    = (UINT32 *) (AstMmioBar + 0xF004);
          mP2Abar          = AstMmioBar + 0x10000;
          
          *mP2ARemapping   = 0x1E6E0000;
          *mP2AEnable      = 0x00000001;
          
          mDramConfig      = (*(mP2Abar+4)) & (BIT1|BIT0);
          
          if ((AstRevision >> 4) == 4) {
            //
            // Calculate AST2500 DRAM base and size
            //
            mDramBase = 0x80000000;
            switch (mDramConfig) {
              case 3:
                mDramSize = 0x40000000; // 1024MB
                break;
              case 2:
                mDramSize = 0x20000000; // 512MB
                break;
              case 1:
                mDramSize = 0x10000000; // 256MB
                break;
              case 0:
                mDramSize = 0x8000000;  // 128MB
                break;
              default:
                break;
            }
          } else if ((AstRevision >> 4) == 3) {
            //
            // Calculate AST2400 DRAM base and size
            //
            mDramBase = 0x40000000;
            switch (mDramConfig) {
              case 3:
                mDramSize = 0x20000000; // 512MB
                break;
              case 2:
                mDramSize = 0x10000000; // 256MB
                break;
              case 1:
                mDramSize = 0x8000000;  // 128MB
                break;
              case 0:
                mDramSize = 0x4000000;  // 64MB
                break;
              default:
                break;
            }
          }
          
          //
          // Calculate DRAM mapping address.
          //
          mRemappingBase = mDramBase + mDramSize - 0x100000;
          *mP2ARemapping = mRemappingBase;
          *mP2AEnable    = 0x00000000;
        }
#else
        Status = EnterEnhanceMode(IVIO);
        if(EFI_ERROR (Status)) {
            DEBUG ((-1, "GetIPMIVideoBar : EnterEnhanceMode return error.\n"));
            IVBar = NULL;
            return NULL;
        }
#endif
    }

    return IVBar;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  DataToBMCPrepareData
//
// Description:
//  BIOS copy data to IPMI video ram.
//
// Input:
//      IN UINT8* Buffer
//      IN UINT32 BufferLength
//
// Output:
//      EFI_STATUS
//          EFI_NOT_READY - Fail
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS DataToBMCPrepareData (
    IN UINT8* Buffer,
    IN UINT32 BufferLength,
    OUT UINT8* VramCheckSum
)
{
    UINT8 CheckSum = 0;
    UINT32 ii = 0;
    IVBar = GetIPMIVideoBar ();

#ifdef OOB_READ_DATA_FROM_DRAM  
        if (mDramBase != 0) {
          *mP2ARemapping = mRemappingBase;
          *mP2AEnable    = 0x00000001;
          
          while (BufferLength != 0 && BufferLength <= 0x100000) {
            if (BufferLength <= 0x10000) {
              gBS->CopyMem (mP2Abar, Buffer, BufferLength);
			  for(ii=0; ii < BufferLength;CheckSum = (UINT8)((UINT32)mP2Abar[ii++] + (UINT32)CheckSum));
              BufferLength = 0;
            } else {
              gBS->CopyMem (mP2Abar, Buffer, 0x10000);
			  for(ii=0; ii < 0x10000;CheckSum = (UINT8)((UINT32)mP2Abar[ii++] + (UINT32)CheckSum));
              *mP2ARemapping += 0x10000;
              Buffer += 0x10000;
              BufferLength -= 0x10000;
            }
          }
          *VramCheckSum = (UINT8)(CheckSum & 0xFF);
          *mP2AEnable    = 0x00000000;
        } else {
          return EFI_NOT_READY;
        }
#else
    if (IVBar != NULL) {
        gBS->CopyMem(IVBar, Buffer, BufferLength);
        for (ii = 0; ii < BufferLength; ii++) {
            CheckSum = (UINT8)((UINT32)IVBar[ii] + (UINT32)CheckSum);
        }
        *VramCheckSum = CheckSum;
    }
    else
        return EFI_NOT_READY;
#endif

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  DataToBMCReadyNotify
//
// Description:
//  BIOS query BMC to get the checksum status.
//
//  IPMI command :
//    0x30, BMC_LUN, 0xA0, 0x0C, FileType
//
// Input:
//      IN UINT8 FileType -
//        0 : Current OOB Hii
//        1 : Current OOB BIN
//        2 : Current OOB DMI
//        3 : Full SMBIOS
//        4 : VPD
//        5 : ASSET Information include BIOS HDD
//        6 : Reserved (for IPMI Feature Flag file)
//        7 : BIOS Feature Flag file
//
//      IN UINT32 FileLength
//        Length of data send to BMC
//
// Output:
//      EFI_STATUS - Return from IPMI command.
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS DataToBMCReadyNotify(
    IN	UINT8	FileType,
    IN	UINT32	FileLength,
    OUT UINT8   *ResponseStatus
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	CommandRetry = OOB_IPMI_COMMAND_RETRY;
    DataToBMCReadyNotify_REQ	CommandData;
    UINT8	D2BMCCompletionCode;
    UINT8	D2BMCCompletionCodeSize;

    DEBUG ((-1, "OOB_DEBUG : DataToBMCReadyNotify entry.\n"));

    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS) return EFI_NOT_READY;

    CommandData.SubCMD = 0x0C;
    CommandData.FileType = FileType;
    *(UINT32*)CommandData.FileLength = FileLength;

    while(CommandRetry) {
        D2BMCCompletionCodeSize = sizeof(UINT8);
        Status = mIpmiTransport->SendIpmiCommand(
                     mIpmiTransport,
                     0x30, // NetFn
                     BMC_LUN,
                     0xA0, // command
                     (UINT8*)&CommandData, // command data
                     sizeof(DataToBMCReadyNotify_REQ), // command size
                     (UINT8*)&D2BMCCompletionCode, // respond data
                     (UINT8*)&D2BMCCompletionCodeSize // respond size
                 );
        DEBUG ((-1, "OOB_DEBUG : IPMI CMD - 0x30, 0xA0, 0x0C Status = %r.\n", Status));
        *ResponseStatus = D2BMCCompletionCode;
        if(!Status)	{ // return success
            break;
        }
        else {
            DEBUG ((-1, "OOB_DEBUG : Completion code = 0x%x.\n", D2BMCCompletionCode));
        }

        gBS->Stall(10000);			// delay 10000 microseconds
        CommandRetry--;
        DEBUG ((-1, "OOB_DEBUG : DataToBMCReadyNotify retry.\n"));
    }

    DEBUG ((-1, "OOB_DEBUG : DataToBMCReadyNotify end.\n"));

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  DataToBMCDone
//
// Description:
//  BIOS query BMC to get the checksum status.
//
//  IPMI command :
//    0x30, BMC_LUN, 0xA0, 0x0E, FileType
//
// Input:
//      IN UINT8 FileType -
//        Same as 'DataToBMCReadyNotify'.
//
// Output:
//      EFI_STATUS - Return from IPMI command.
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS DataToBMCDone(
    IN	UINT8	FileType,
    OUT UINT8 *ResponseStatus
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	CommandRetry = OOB_IPMI_COMMAND_RETRY * 3; // Some files is big, so increase the retry times.
    DataToBMCDone_REQ	CommandData;
    DataToBMCDone_RSP	D2BMCDCompletionCode;
    UINT8	D2BMCDCompletionCodeSize;

    DEBUG ((-1, "OOB_DEBUG : DataToBMCDone entry.\n"));

    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS) return EFI_NOT_READY;

    CommandData.SubCMD = 0x0E;
    CommandData.FileType = FileType;
    while(CommandRetry) {
        D2BMCDCompletionCodeSize = sizeof(DataToBMCDone_RSP);
        Status = mIpmiTransport->SendIpmiCommand(
                     mIpmiTransport,
                     0x30, // NetFn
                     BMC_LUN,
                     0xA0, // command
                     (UINT8*)&CommandData, // command data
                     sizeof(DataToBMCDone_REQ), // command size
                     (UINT8*)&D2BMCDCompletionCode, // respond data
                     (UINT8*)&D2BMCDCompletionCodeSize // respond size
                 );
        DEBUG ((-1, "OOB_DEBUG : IPMI CMD - 0x30, 0xA0, 0x0E Status = %r.\n", Status));
        *ResponseStatus = D2BMCDCompletionCode.ChecksumStatus;
        if(!Status) {	//return success
            DEBUG ((-1, "OOB_DEBUG : ChecksumStatus = 0x%x.\n", D2BMCDCompletionCode.ChecksumStatus));
            if(D2BMCDCompletionCode.ChecksumStatus == 0x02)	//checksum pass
                return EFI_SUCCESS;
            else if(D2BMCDCompletionCode.ChecksumStatus == 0x01)//checksum fail
                return EFI_ABORTED;
        }
        else {
            DEBUG ((-1, "OOB_DEBUG : Completion code = 0x%x.\n", D2BMCDCompletionCode.ChecksumStatus));
        }

        gBS->Stall(100000);    // delay 100000 microseconds
        CommandRetry--;
        DEBUG ((-1, "OOB_DEBUG : DataToBMCDone retry.\n"));
    }

    DEBUG ((-1, "OOB_DEBUG : DataToBMCDone end.\n"));

    return EFI_ABORTED;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  DataToBIOSReadData
//
// Description:
//  BIOS read data from IPMI video ram.
//
// Input:
//      IN OUT UINT8* Buffer
//      IN UINT32 BufferLength
//
// Output:
//      EFI_STATUS
//          EFI_NOT_READY - Fail
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS DataToBIOSReadData (
    IN OUT UINT8* Buffer,
    IN UINT32 BufferLength
)
{
#ifdef OOB_READ_DATA_FROM_DRAM
  UINT32 *P2AEnable;
  UINT32 *P2ARemapping;
  UINT8  *P2Abar;
  UINT32 OldP2AEnable;
  UINT32 OldP2ARemapping;
  UINT8  DramConfig = 0;
  UINT32 DramSize = 0;
  UINT32 DramBase = 0;
#endif
      
    IVBar = GetIPMIVideoBar ();

#ifdef OOB_READ_DATA_FROM_DRAM
    if (AstMmioBar != NULL && AstRevision != 0) {
      P2AEnable       = (UINT32 *) (AstMmioBar + 0xF000);
      P2ARemapping    = (UINT32 *) (AstMmioBar + 0xF004);
      P2Abar          = AstMmioBar + 0x10000;

      OldP2AEnable    = *P2AEnable;
      OldP2ARemapping = *P2ARemapping;

      *P2ARemapping   = 0x1E6E0000;
      *P2AEnable      = 0x00000001;

      DramConfig      = (*(P2Abar+4)) & (BIT1|BIT0);

      if ((AstRevision >> 4) == 4) {
        //
        // Calculate AST2500 DRAM base and size
        //
        DramBase = 0x80000000;
        switch (DramConfig) {
          case 3:
            DramSize = 0x40000000; // 1024MB
            break;
          case 2:
            DramSize = 0x20000000; // 512MB
            break;
          case 1:
            DramSize = 0x10000000; // 256MB
            break;
          case 0:
            DramSize = 0x8000000;  // 128MB
            break;
          default:
            break;
        }
      } else if ((AstRevision >> 4) == 3) {
        //
        // Calculate AST2400 DRAM base and size
        //
        DramBase = 0x40000000;
        switch (DramConfig) {
          case 3:
            DramSize = 0x20000000; // 512MB
            break;
          case 2:
            DramSize = 0x10000000; // 256MB
            break;
          case 1:
            DramSize = 0x8000000;  // 128MB
            break;
          case 0:
            DramSize = 0x4000000;  // 64MB
            break;
          default:
            break;
        }
      }

      //
      // Calculate DRAM mapping address.
      //
      *P2ARemapping = DramBase + DramSize - 0x100000;
  }
#endif  

#ifdef OOB_READ_DATA_FROM_DRAM
    if (DramBase != 0 && BufferLength <= 0x10000) {
      gBS->CopyMem(Buffer, P2Abar, BufferLength);
    } else if (IVBar != NULL) {
      gBS->CopyMem(Buffer, IVBar, BufferLength);
    } else {
      *P2ARemapping = OldP2ARemapping;
      *P2AEnable = OldP2AEnable;
      return EFI_NOT_READY;
    }
    
    *P2ARemapping = OldP2ARemapping;
    *P2AEnable = OldP2AEnable;
#else
    if (IVBar != NULL) {
        gBS->CopyMem(Buffer, IVBar, BufferLength);
    }
    else
        return EFI_NOT_READY;
#endif

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  DataToBIOSReadyNotify
//
// Description:
//  BIOS starts to get data from the video memory.
//
//  IPMI command :
//    0x30, BMC_LUN, 0xA0, 0x09, FileType
//
// Input:
//      IN UINT8 FileType -
//        Same as 'DataToBMCReadyNotify'.
//
// Output:
//	OUT UINT32 *FileLength
//	    Length of data from video memory.
//
//      EFI_STATUS - Return from IPMI command.
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS DataToBIOSReadyNotify(
    IN UINT8 FileType,
    OUT UINT8* RspStatus,
    OUT UINT32 *FileLength
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	CommandRetry = OOB_IPMI_COMMAND_RETRY;
    DataToBIOSReadyNotify_REQ	CommandData;
    DataToBIOSReadyNotify_RSP	D2BIOSCompletionCode;
    UINT8	D2BIOSCompletionCodeSize;

    DEBUG ((-1, "OOB_DEBUG : DataToBIOSReadyNotify entry.\n"));

    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)	return EFI_NOT_READY;

    CommandData.SubCMD = 0x09;
    CommandData.FileType = FileType;
    while(CommandRetry) {
        D2BIOSCompletionCodeSize = sizeof(DataToBIOSReadyNotify_RSP);
        Status = mIpmiTransport->SendIpmiCommand(
                     mIpmiTransport,
                     0x30,				// NetFn
                     BMC_LUN,
                     0xA0,				// command
                     (UINT8*)&CommandData,		// command data
                     sizeof(DataToBIOSReadyNotify_REQ),		// command size
                     (UINT8*)&D2BIOSCompletionCode,	// respond data
                     (UINT8*)&D2BIOSCompletionCodeSize// respond size
                 );
        DEBUG ((-1, "OOB_DEBUG : IPMI CMD - 0x30, 0xA0, 0x09 Status = %r.\n", Status));
        if (!EFI_ERROR (Status)) {	//return success
            *FileLength = *(UINT32*)D2BIOSCompletionCode.FileLength;
            *RspStatus = D2BIOSCompletionCode.Status;
            DEBUG ((-1, "OOB_DEBUG : DataToBIOSReadyNotify FileLength = 0x%x.\n", *FileLength));
            DEBUG ((-1, "OOB_DEBUG : DataToBIOSReadyNotify RspStatus = 0x%x.\n", *RspStatus));
            break;
        }
        else {	//EFI_DEVICE_ERROR
            DEBUG ((-1, "OOB_DEBUG : conpletion code = 0x%x.\n", D2BIOSCompletionCode.FileLength[0]));
            if(D2BIOSCompletionCode.FileLength[0] == 0xCC)
                break;
        }

        gBS->Stall(10000); // delay 10000 microseconds
        CommandRetry--;
        DEBUG ((-1, "OOB_DEBUG : DataToBIOSReadyNotify retry.\n"));
    }

    DEBUG ((-1, "OOB_DEBUG : DataToBIOSReadyNotify end.\n"));

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  DataToBIOSWait
//
// Description:
//  BIOS starts to get data from the video memory.
//
//  IPMI command :
//    0x30, BMC_LUN, 0xA0, 0x0A, FileType
//
// Input:
//      IN UINT8 FileType -
//        Same as 'DataToBMCReadyNotify'.
//
// Output:
//      EFI_STATUS - Return from IPMI command.
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS DataToBIOSWait (
    IN UINT8 FileType,
    OUT UINT8* RspStatus
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8 CommandRetry = OOB_IPMI_COMMAND_RETRY;
    DataToBIOSWait_REQ CommandData;
    DataToBIOSWait_RSP D2BIOSCompletionCode;
    UINT8 D2BIOSCompletionCodeSize;

    DEBUG ((-1, "OOB_DEBUG : DataToBIOSWait entry.\n"));

    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)	return EFI_NOT_READY;

    CommandData.SubCMD = 0x0A;
    CommandData.FileType = FileType;
    while(CommandRetry) {
        D2BIOSCompletionCodeSize = sizeof(DataToBIOSWait_RSP);
        Status = mIpmiTransport->SendIpmiCommand(
                     mIpmiTransport,
                     0x30, // NetFn
                     BMC_LUN,
                     0xA0, // command
                     (UINT8*)&CommandData, // command data
                     sizeof(DataToBIOSWait_REQ), // command size
                     (UINT8*)&D2BIOSCompletionCode, // respond data
                     (UINT8*)&D2BIOSCompletionCodeSize // respond size
                 );
        DEBUG ((-1, "OOB_DEBUG : IPMI CMD - 0x30, 0xA0, 0x0A Status = %r.\n", Status));
        if (!EFI_ERROR (Status)) { // return success
            *RspStatus = D2BIOSCompletionCode.Status;
            DEBUG ((-1, "OOB_DEBUG : DataToBIOSWait RspStatus = 0x%x.\n", *RspStatus));
            break;
        }
        else { // EFI_DEVICE_ERROR
            //DEBUG ((-1, "OOB_DEBUG : conpletion code = 0x%x.\n", D2BIOSCompletionCode.FileLength[0]));
            //if(D2BIOSCompletionCode.FileLength[0] == 0xCC)
            //    break;
        }

        gBS->Stall(10000); // delay 10000 microseconds
        CommandRetry--;
    }

    DEBUG ((-1, "OOB_DEBUG : DataToBIOSWait end.\n"));

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  DataToBIOSDone
//
// Description:
//  BIOS notifies BMC that the transmission is done.
//
//  IPMI command :
//    0x30, BMC_LUN, 0xA0, 0x0B, FileType, DownloadStatus
//
// Input:
//      IN UINT8 FileType -
//        Same as 'DataToBMCReadyNotify'.
//
//	IN UINT8 DownloadStatus -
//	    0x00 : success
//	    0x01 : failure
//
// Output:
//      EFI_STATUS - Return from IPMI command.
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS DataToBIOSDone(
    IN	UINT8	FileType,
    IN	UINT8	DownloadStatus
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	CommandRetry = OOB_IPMI_COMMAND_RETRY;
    DataToBIOSDone_REQ	CommandData;
    UINT8	D2BIOSDCompletionCode;
    UINT8	D2BIOSDCompletionCodeSize;

    DEBUG ((-1, "OOB_DEBUG : DataToBIOSDone entry.\n"));

    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)	return EFI_NOT_READY;

    CommandData.SubCMD = 0x0B;
    CommandData.FileType = FileType;
    CommandData.DownloadStatus = DownloadStatus;
    while(CommandRetry) {
        D2BIOSDCompletionCodeSize = sizeof(UINT8);
        Status = mIpmiTransport->SendIpmiCommand(
                     mIpmiTransport,
                     0x30,				// NetFn
                     BMC_LUN,
                     0xA0,				// command
                     (UINT8*)&CommandData,		// command data
                     sizeof(DataToBIOSDone_REQ),		// command size
                     (UINT8*)&D2BIOSDCompletionCode,	// respond data
                     (UINT8*)&D2BIOSDCompletionCodeSize// respond size
                 );
        DEBUG ((-1, "OOB_DEBUG : IPMI CMD - 0x30, 0xA0, 0x0B Status = %r.\n", Status));
        if (!EFI_ERROR (Status)) // return success
            break;
        else
            DEBUG ((-1, "OOB_DEBUG : Completion code = 0x%x.\n", D2BIOSDCompletionCode));

        gBS->Stall(10000); // delay 10000 microseconds
        CommandRetry--;
        DEBUG ((-1, "OOB_DEBUG : DataToBIOSDone retry.\n"));
    }

    DEBUG ((-1, "OOB_DEBUG : DataToBIOSDone end.\n"));

    return Status;
}

EFI_STATUS
SMC_IPMICmd30_68_1B (
  IN     UINT8                       GetSet, // 0 - Get, 1 - Set
  IN OUT BIOS_TPM_CAPABILITIES_REQ   *CommandData
  )
{
  EFI_STATUS  Status             = EFI_SUCCESS;
  UINT8       CompletionCode[8]  = {0};
  UINT8       CompletionCodeSize = 8;
  UINT8       CommandRetry       = SMC_IPMI_RETRY;

  DEBUG((-1, "SMC_IPMICmd30_68_1B entry.\n"));

  if (SmcLibLocateIpmiProtocol() != EFI_SUCCESS)       return EFI_NOT_READY;

  CommandData->SubCMD = 0x1B;
  CommandData->GetSet = GetSet;

  while (CommandRetry){
    Status = mIpmiTransport->SendIpmiCommand(
                               mIpmiTransport,
                               0x30, // NetFunc
                               BMC_LUN, 
                               0x68, // Command
                               (UINT8*)CommandData,
                               sizeof(BIOS_TPM_CAPABILITIES_REQ),
                               CompletionCode,
                               &CompletionCodeSize
                               );
    if (!EFI_ERROR(Status))  break;
    gBS->Stall(10000);     // Delay 10000 microseconds.
    CommandRetry--;
  }

  DEBUG((-1, "CompletionCodeSize = 0x%x.\n", CompletionCodeSize));
  if (!GetSet && CompletionCodeSize == 8) {
    CopyMem (&(CommandData->StatusFlag), CompletionCode, 8);
    DEBUG((-1, "CommandData->StatusFlag = 0x%x.\n", CommandData->StatusFlag));
    DEBUG((-1, "CommandData->Reserved1 = 0x%x.\n", CommandData->Reserved1));
    DEBUG((-1, "CommandData->Reserved2 = 0x%x.\n", CommandData->Reserved2));
    DEBUG((-1, "CommandData->Reserved3 = 0x%x.\n", CommandData->Reserved3));
    DEBUG((-1, "CommandData->Reserved4 = 0x%x.\n", CommandData->Reserved4));
    DEBUG((-1, "CommandData->Reserved5 = 0x%x.\n", CommandData->Reserved5));
    DEBUG((-1, "CommandData->Reserved6 = 0x%x.\n", CommandData->Reserved6));
    DEBUG((-1, "CommandData->Reserved7 = 0x%x.\n", CommandData->Reserved7));
  }

  DEBUG((-1, "SMC_IPMICmd30_68_1B end.\n"));
  return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  SetBIOSIdentify
//
// Description:
//  Send BIOS OOB Identify to BMC.
//
//  we implement retry to make sure transfer data successfully.
//  In some special case, BMC is too busy to handle upload commands.
//
//  IPMI command :
//    0x30, BMC_LUN, 0xA0, 0x12, Set, (The Board_Info of current BIOS)
//
// Input:
//      None.
//
// Output:
//      EFI_STATUS
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
SMC_IPMICmd30_A0_12 (
  IN  UINT32                            BoardId,
  IN  UINT32                            Date,
  IN  UINT16                            Time
  )
{
    EFI_STATUS  Status = EFI_SUCCESS;
    BIOS_OOB_Identify_Set       CommandData;
    UINT8       OOBIdentifyCompletionCode;
    UINT8       OOBIdentifyCompletionCodeSize;
    UINT8       CommandRetry = OOB_IPMI_COMMAND_RETRY;

    DEBUG((-1, "OOB_DEBUG : SMC_IPMICmd30_A0_12 entry.\n"));

    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)       return EFI_NOT_READY;

    CommandData.SubCMD = 0x12;
    CommandData.GetSet = OOB_SET;

    *(UINT32*)(CommandData.BoardInfo.SVID) = BoardId;
    *(UINT32*)(CommandData.BoardInfo.Date) = Date;
    *(UINT16*)(CommandData.BoardInfo.Time) = Time;

    while(CommandRetry){
        OOBIdentifyCompletionCodeSize = sizeof(UINT8);
        Status = mIpmiTransport->SendIpmiCommand(
                        mIpmiTransport,
                        0x30,                                   // NetFn
                        BMC_LUN,
                        0xA0,                                   // command
                        (UINT8 *)&CommandData,                  // command data
                        sizeof(BIOS_OOB_Identify_Set),          // command size
                        (UINT8 *)&OOBIdentifyCompletionCode,    // respond data
                        (UINT8 *)&OOBIdentifyCompletionCodeSize // respond size
        );
        if(!EFI_ERROR(Status))  break;
        gBS->Stall(100000);   // delay 100000 microseconds
        CommandRetry--;
    }
    DEBUG((-1, "OOB_DEBUG : SMC_IPMICmd30_A0_12 Status = %r\n", Status));
    DEBUG((-1, "OOB_DEBUG : SMC_IPMICmd30_A0_12 end.\n"));

    return Status;
}

EFI_STATUS
SMC_IPMICmd30_A0_15(    //SMBIOS ERASE STATUS
    IN  UINT8   GetSet, // 0 - Get, 1 - Set
    IN OUT      UINT8   *FlashFlag
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    BIOS_SMBIOS_ERASE_REQ       CommandData;
    UINT8       CompletionCode;
    UINT8       CompletionCodeSize = sizeof(CompletionCode);
    UINT8       CommandRetry = SMC_IPMI_RETRY;
    

    DEBUG((-1, "SMC_IPMICmd30_A0_15 entry.\n"));

    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)       return EFI_NOT_READY;

    CommandData.SubCMD = 0x15;
    CommandData.GetSet = GetSet;
    if(!GetSet)
        CommandData.FlashFlag = 0;      //get
    else
        CommandData.FlashFlag = *FlashFlag;      //clear

    while(CommandRetry){
        Status = mIpmiTransport->SendIpmiCommand(
                        mIpmiTransport,
                        0x30,
                        BMC_LUN,
                        0xA0,
                        (UINT8*)&CommandData,
                        sizeof(CommandData),
                        &CompletionCode,
                        &CompletionCodeSize);
        if(!EFI_ERROR(Status))  break;
        gBS->Stall(10000);     // delay 10000 microseconds
        CommandRetry--;
    }

    *FlashFlag = CompletionCode;
    DEBUG((-1, "FlashFlag = %x.\n", *FlashFlag));
    DEBUG((-1, "SMC_IPMICmd30_A0_15 end.\n"));
    return Status;
}

EFI_STATUS
SMC_IPMICmd30_70_70(VOID)
{
    EFI_STATUS  Status;
    UINT8       CommandData[6] = {0x70, 1, 1, 0, 0, 0};
    UINT8       DataSize;
    UINT8       CommandRetry = SMC_IPMI_RETRY;

    DEBUG((-1, "SMC_IPMICmd30_70_70 entry.\n"));
    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)       return EFI_NOT_READY;

    while(CommandRetry){
        Status = mIpmiTransport->SendIpmiCommand(
                        mIpmiTransport,
                        EFI_SM_INTEL_OEM,//0x30
                        BMC_LUN,
                        0x70,
                        CommandData,//command data
                        6,//command size
                        NULL,
                        &DataSize);

        if(!EFI_ERROR(Status))  break;
        gBS->Stall(10000);     // delay 10000 microseconds
        CommandRetry--;
    }

    DEBUG((-1, "SMC_IPMICmd30_70_70 end.\n"));
    return Status;
}

EFI_STATUS
SMC_IPMICmd30_A1(
    IN  UINT8   *BMCCMDData,
    IN  UINT8   *pFRUBlockData
)
{
    UINT8       FRUBlockDataSize = 32;
    EFI_STATUS  Status;
    UINT8       CommandRetry = SMC_IPMI_RETRY;

    DEBUG((-1, "BMCGetPowerInfo entry.\n"));
    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)       return EFI_NOT_READY;

    while(CommandRetry){
        Status = mIpmiTransport->SendIpmiCommand(
                        mIpmiTransport,
                        EFI_SM_INTEL_OEM,       // 0x30
                        BMC_LUN,
                        0xA1,                   // command
                        BMCCMDData,             // command data, system type
                        2,                      // command data size
                        pFRUBlockData,          // Read back data buffer
                        &FRUBlockDataSize);     // Read back data buffer size

        if(!EFI_ERROR(Status))  break;
        gBS->Stall(10000);     // delay 10000 microseconds
        CommandRetry--;
    }
    DEBUG((-1, "BMCGetPowerInfo end.\n"));
    return Status;
}

EFI_STATUS
SMC_IPMICmd30_70_DF(	//Set BMC Sensor Polling
    IN  UINT8   FunStatus
)
{
    EFI_STATUS  Status;
    UINT8       TempData[2];
    UINT8       ResponseData, ResponseDataSize = sizeof(ResponseData);
    UINT8       CommandRetry = SMC_IPMI_RETRY;

    DEBUG((-1, "SMC_IPMICmd30_70_DF entry.\n"));
    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)       return EFI_NOT_READY;

    TempData[0] = 0xDF;
    TempData[1] = FunStatus;	//1: enable, 0: disable
    
    while(CommandRetry){
        Status = mIpmiTransport->SendIpmiCommand(
                        mIpmiTransport,
                        EFI_SM_INTEL_OEM,       //0x30
                        BMC_LUN,                //0x00
                        0x70,                   //command
                        TempData,               //command data
                        2,                      //command size
                        &ResponseData,
                        &ResponseDataSize);

        if(!EFI_ERROR(Status))  break;
        gBS->Stall(10000);     // delay 10000 microseconds
        CommandRetry--;
    }

    DEBUG((-1, "SMC_IPMICmd30_70_DF end.\n"));
    return EFI_SUCCESS;
}

EFI_STATUS
SMC_IPMICmd30_70_3A(void)	//BMC and BIOS handshaking
{
    EFI_STATUS  Status;
    UINT8       TempData[16];
    UINT8       ResponseData, ResponseDataSize = sizeof(ResponseData);
    UINT8       CommandRetry = SMC_IPMI_RETRY;

    DEBUG((-1, "SMC_IPMICmd30_70_3A entry.\n"));
    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)       return EFI_NOT_READY;

    gBS->SetMem(TempData, 16, 0x00);
    TempData[0] = 0x3A;
    TempData[1] |= BIT0;

    while(CommandRetry){
        Status = mIpmiTransport->SendIpmiCommand(
                        mIpmiTransport,
                        EFI_SM_INTEL_OEM,       //0x30
                        BMC_LUN,                //0x00
                        0x70,                   //command
                        TempData,               //command data
                        16,                     //command size
                        &ResponseData,
                        &ResponseDataSize);

        if(!EFI_ERROR(Status))  break;
        gBS->Stall(10000);     // delay 10000 microseconds
        CommandRetry--;
    }

    DEBUG((-1, "SMC_IPMICmd30_70_3A end.\n"));
    return EFI_SUCCESS;
}

EFI_STATUS
SMC_IPMICmd30_97(	//Enable/Disable Watchdog function
    IN  UINT8   SmcWatchDogMode,
    IN  UINT8   SmcWatchDogTime
)
{
    EFI_STATUS  Status;
    UINT8       TempData[] = {SmcWatchDogMode, SmcWatchDogTime};// Default time out value 5 minutes
    UINT8       ResponseData, ResponseDataSize = sizeof(ResponseData);
    UINT8       CommandRetry = SMC_IPMI_RETRY;

    DEBUG((-1, "SMC_IPMICmd30_97 entry.\n"));
    DEBUG((-1, "SmcWatchDogMode = %d\n", SmcWatchDogMode));
    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)       return EFI_NOT_READY;

    while(CommandRetry){
        Status = mIpmiTransport->SendIpmiCommand(
                        mIpmiTransport,
                        EFI_SM_INTEL_OEM,       //0x30
                        BMC_LUN,                //0x00
                        0x97,                   //command
                        TempData,               //command data
                        2,                      //command size
                        &ResponseData,
                        &ResponseDataSize);

        if(!EFI_ERROR(Status))  break;
        gBS->Stall(10000);     // delay 10000 microseconds
        CommandRetry--;
    }

    DEBUG((-1, "SetWDTViaBmc End.\n"));
    return EFI_SUCCESS;
}

EFI_STATUS
SMC_IPMICmd30_98(void)	//Reset watchdog timer
{
    EFI_STATUS  Status;
    UINT8       ResponseData, ResponseDataSize = sizeof(ResponseData);
    UINT8       CommandRetry = SMC_IPMI_RETRY;

    DEBUG((-1, "SMC_IPMICmd30_98 entry.\n"));
    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)	return EFI_NOT_READY;

    while(CommandRetry){
        Status = mIpmiTransport->SendIpmiCommand(
                        mIpmiTransport,
                        EFI_SM_INTEL_OEM,       //0x30
                        BMC_LUN,                //0x00
                        0x98,                   //command
                        NULL,                   //command data
                        0,                      //command size
                        &ResponseData,
                        &ResponseDataSize);
    
        if(!EFI_ERROR(Status))  break;
        gBS->Stall(10000);     // delay 10000 microseconds
        CommandRetry--;
    }

    DEBUG((-1, "SMC_IPMICmd30_98 end.\n"));
    return EFI_SUCCESS;
}

EFI_STATUS
SMC_IPMICmd30_20(void)	//GET/SET_MBID
{
    EFI_STATUS	Status;
    UINT8	SmcHardwareInfo[10] = {0};
    UINT8	ResponseData, ResponseDataSize, i;

    DEBUG((-1, "SMC_IPMICmd30_20 \n"));

    Status = GetHardwareInfo(SmcHardwareInfo);

    if(EFI_ERROR(Status)) {
        DEBUG((-1, "Get Hardware Info fail \n"));
        return	EFI_NOT_READY;
    }

    DEBUG((-1, "SmcHardwareInfo = "));
    for(i = 0; i < 10; i++ ) {
        DEBUG((-1, "%x ",SmcHardwareInfo[i]));
    }
    DEBUG((-1, "\n"));

    Status = SMC_IPMICmd(
                 EFI_SM_INTEL_OEM,      //NetFunction
                 0,                     // LUN
                 0x20,                  //Command
                 SmcHardwareInfo,			     // *CommandData
                 10,	// Size of CommandData
                 &ResponseData,		    // *ResponseData
                 &ResponseDataSize);

    DEBUG((-1, "SMC_IPMICmd30_20 end. %r\n", Status));

    return EFI_SUCCESS;
}

EFI_STATUS
SMC_IPMICmd30_9E(void)	//Set (2st, 3st, ...) System Lan mac
{
    EFI_STATUS  Status;
    UINT8       SmcLanMAC[MAX_LAN_MAC_INFO_SIZE] = {0};
    UINT8       ResponseData, ResponseDataSize, i;

    DEBUG((-1, "SMC_IPMICmd30_9E \n"));

    Status = GetLanMacInfo(SmcLanMAC);

    if(EFI_ERROR(Status)) {
        DEBUG((-1, "GetLan fail \n"));
        return EFI_NOT_READY;
    }

    DEBUG((-1, "SmcLanMAC = "));
    for(i = 0; i < MAX_LAN_MAC_INFO_SIZE; i++) {
        DEBUG((-1, "%x ",SmcLanMAC[i]));
    }
    DEBUG((-1, "\n"));

    Status = SMC_IPMICmd(
                 EFI_SM_INTEL_OEM,      //NetFunction
                 0,                     // LUN
                 0x9E,                  //Command
                 SmcLanMAC,			     // *CommandData
                 (UINT8)MAX_LAN_MAC_INFO_SIZE,	// Size of CommandData
                 &ResponseData,		    // *ResponseData
                 &ResponseDataSize);



    DEBUG((-1, "SMC_IPMICmd30_9E end. %r\n", Status));
    return EFI_SUCCESS;
}

EFI_STATUS
SMC_IPMICmd30_99(void)
{
    EFI_STATUS	Status;
    UINT8   SmcSystemConfig[10] = {0};
    UINT8   ResponseData;
    UINT8   ResponseDataSize;
    UINT8   i;

    DEBUG((-1, "SMC_IPMICmd30_99 \n"));
    if(SmcLibLocateIpmiProtocol () != EFI_SUCCESS)	return EFI_NOT_READY;

    Status = GetSystemConfig(SmcSystemConfig);

    if( EFI_ERROR(Status) ) {
        DEBUG((-1, "Get Hardware Info fail \n"));
        return EFI_NOT_READY;
    }

    DEBUG((-1, "SmcSystemConfig = "));
    for( i = 0; i < 10; i++ ) {
        DEBUG((-1, "%x ",SmcSystemConfig[i]));
    }

//bit0 - Infiniband 0-No, 1-Yes
//bit1 - Analog Device(Hotswap Controller)
//bit2 - 10G sensor
//bit3 - HDD status support
//bit4 - BIOS OOB support to update BIOS current settings(From BIOS and SW checks it)
//bit5 - Onboard LSI HDD existing
//bit6-  Add-on LSI HDD existing
//bit7-  Mask NVMe hotplug function, default is 0. If 1, please mask out NVMe hotplug function.
    
//check 10G device
    if(CheckBoardNameFromFruData())
        SmcSystemConfig[0] |= BIT2;

//check OOB support
#if defined SmcOutBand_SUPPORT && SmcOutBand_SUPPORT != 0
    SmcSystemConfig[0] |= BIT4;
#endif

    Status = SMC_IPMICmd(
                 EFI_SM_INTEL_OEM,      //NetFunction
                 0,                     // LUN
                 0x99,                  //Command
                 SmcSystemConfig,       // *CommandData
                 10,                    // Size of CommandData
                 &ResponseData,         // *ResponseData
                 &ResponseDataSize);

    DEBUG((-1, "SMC_IPMICmd30_99 end. %r\n", Status));
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: CheckBoardNameFromFruData
//
// Description:
//    The code will read FRU data by send FruDeviceId = 1, FruOffset = 8, Count = 10, size = 4 to IPMI
//    To get board name to check set 10G bit for BMC or not.
//    If your project name does not contain "T" after "-" but need to support 10G/1G, please update the BIT in project level.
//
// Input:
// Input:
//   EFI_IPMI_BMC_INSTANCE_DATA	*IpmiInstance - Data structure describing BMC
//										variables and used for sending commands
//  IN  EFI_PEI_SERVICES    **PeiServices - Address for EFI_PEI_SERVICES.

//
// Output:
//     BOOLEAN - TRUE: 10G system. FALSE: 1G system.
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

BOOLEAN
CheckBoardNameFromFruData(void)
{
    UINT8	CommandData[4] = {1,0x18,0,0x10};//Hard code to get Board Name
    UINT8	ResponseData[20], ResponseDataSize = sizeof(ResponseData);
    EFI_STATUS	Status;
    UINT32	i, j = 0;

    DEBUG((-1, "CheckBoardNameFromFruData entry.\n"));
    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)      return  FALSE;
// Read FRU data
//
    Status = mIpmiTransport->SendIpmiCommand(
                 mIpmiTransport,
                 0x0A,	//EFI_SM_NETFN_STORAGE,
                 BMC_LUN,
                 0x11,	//EFI_STORAGE_READ_FRU_DATA,
                 CommandData,
                 4,
                 ResponseData,
                 &ResponseDataSize);

    if(EFI_ERROR(Status))	return FALSE;

//Check Respond data for the Board NAME.
    for(i = 0; i < ResponseDataSize; i++) {
        if(ResponseData[i] != 0x2D)//chekc "-" ASCII code.
            j++;
        else {//check if ResponseData contain "T" ASCII code for 10G flag.
            for( ; j < ResponseDataSize; j++) {
                if(ResponseData[j] == 0x54) {
                    DEBUG((-1, "Board contain T after -, it is a 10G system. \n"));
                    return TRUE;
                }
            }
            DEBUG((-1, "Board not contain T ASCII \n"));
            return FALSE;
        }//end of else
    }
    DEBUG((-1, "CheckBoardNameFromFruData end.\n"));
    return FALSE;
}

EFI_STATUS
SMC_IPMICmd30_AC(void)	//SendBIOSVerBuildTimeDXE(void)
{
    EFI_STATUS  Status;
    UINT8 ResponseData[100], ResponseDataSize;
    UINT8 BIOSVersion[17];
    UINT8 BIOSDate[18];

    DEBUG((-1, "SendBIOSVerBuildTime in DXE\n"));

    SmcLibGetBiosInforInSmbios(NULL, &BIOSVersion[2], &BIOSDate[2]);

    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)	return EFI_NOT_READY;

    *(UINT16*)BIOSVersion = 0x0100;
    *(UINT16*)BIOSDate = 0x0101;

    ResponseDataSize = sizeof (ResponseData);
    Status = mIpmiTransport->SendIpmiCommand(
                 mIpmiTransport,
                 EFI_SM_INTEL_OEM,
                 0,
                 0xAC,
                 BIOSVersion,
                 sizeof(BIOSVersion),
                 (UINT8*)ResponseData,
                 &ResponseDataSize);

    ResponseDataSize = sizeof (ResponseData);
    Status = mIpmiTransport->SendIpmiCommand(
                 mIpmiTransport,
                 EFI_SM_INTEL_OEM,
                 0,
                 0xAC,
                 BIOSDate,
                 sizeof(BIOSDate),
                 (UINT8*)ResponseData,
                 &ResponseDataSize);

    return Status;
}

EFI_STATUS
SendIPMIGetCPLDVersion ()
{
    EFI_STATUS	Status;
    UINT8	Senddata[]= {0x03, 0x70, 0x01, 0};
    UINT8	DataSize;
    UINT32	CPLDRevision = 0;
    UINT8	*pByte = (UINT8*)&CPLDRevision;
    UINT8 	Index, Retry;

    if(SmcLibLocateIpmiProtocol() != EFI_SUCCESS)	return EFI_NOT_READY;

    // It's 3-byte CPLD definition
    for (Index = 0; Index < 3 ; Index++) { //3
        Senddata[3] = 5+Index;//write the offset
        Retry = 4;
        while( --Retry ) {
            DataSize = 1;
            Status = mIpmiTransport->SendIpmiCommand (
                         mIpmiTransport,
                         0x06, //EFI_SM_NETFN_APP,
                         0,
                         0x52, //EFI_APP_MASTER_WRITE_READ,
                         Senddata,
                         sizeof(Senddata),
                         &pByte[Index],
                         &DataSize
                     );
            gBS->Stall(50000); // 50msec
            if(  !EFI_ERROR(Status) && mIpmiTransport->CommandCompletionCode == 0) break;
        }
        if(Retry > 0 ) continue;
        else break;
    }

    if( Index >= 3 ) {
        PcdSet32(PcdSmcCPLDRevision,CPLDRevision);
    } else
        PcdSet32(PcdSmcCPLDRevision,0);

    return Status;
}


//****************************************************************************
