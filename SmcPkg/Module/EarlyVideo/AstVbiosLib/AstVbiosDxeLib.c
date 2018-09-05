//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix: Implement SMC_SIMPLETEXTOUT_PROTOCOL.
//    Reason : Use the protocol instead of the previous lib.
//    Auditor: Yenhsin Cho
//    Date   : May/27/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/05/2014
//
//****************************************************************************
//****************************************************************************
#include <Library/UefiLib.h>
//#include <Protocol\DriverBinding.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
//#include <AmiStatusCodes.h>
#include <AmiCspLib.h>
#include <Library/PciLib.h>
#include "SmcSimpleTextOutProtocol.h"


#define TurnOn     TRUE
#define TurnOff    FALSE

#define DumpPciBrigde(_B, _D, _F) \
        {TRACE((TRACE_ALWAYS, "## PciBrigde: BUS:(%d)-VID:[%04x]_DID:[%04x]_CMD[%x]_0x3E:[%x]_0x18:[%x]0x19:[%x]0x1A:[%x]\n", _B, \
                               PciRead16(PCI_LIB_ADDRESS(_B, _D, _F, 0x00)), PciRead16(PCI_LIB_ADDRESS(_B, _D, _F, 0x02)), PciRead16(PCI_LIB_ADDRESS(_B, _D, _F, 0x04)), \
                               PciRead16(PCI_LIB_ADDRESS(_B, _D, _F, 0x3e)), PciRead8(PCI_LIB_ADDRESS(_B, _D, _F, 0x18)), PciRead8(PCI_LIB_ADDRESS(_B, _D, _F, 0x19)), \
                               PciRead8(PCI_LIB_ADDRESS(_B, _D, _F, 0x1a))));}
#define DumpPciVgaDev(_B, _D, _F) \
        {TRACE((TRACE_ALWAYS, "## PciVgaDev: BUS:(%d)-VID:[%04x]_DID:[%04x]_CMD[%x]_0x09:[%x]0x0A:[%x]0x0B:[%x]\n", _B, \
                               PciRead16(PCI_LIB_ADDRESS(_B, _D, _F, 0x00)), PciRead16(PCI_LIB_ADDRESS(_B, _D, _F, 0x02)), PciRead16(PCI_LIB_ADDRESS(_B, _D, _F, 0x04)), \
                               PciRead8(PCI_LIB_ADDRESS(_B, _D, _F, 0x09)), PciRead8(PCI_LIB_ADDRESS(_B, _D, _F, 0x0a)), PciRead8(PCI_LIB_ADDRESS(_B, _D, _F, 0x0b)))); }

typedef struct
{
  UINT8           BUS;
  UINT8           DEV;
  UINT8           FUN;
} _PCIDEVICE_BDF;

static BOOLEAN         FindVgaDev = FALSE;
static _PCIDEVICE_BDF  AspeedRPT_BDF = {0}, AspeedVGA_BDF = {0}, AddonRPT_BDF = {0}, AddonVGA_BDF = {0};

extern SMC_SIMPLETEXTOUT_PROTOCOL  *pmSmcSimpleTextOutProtocol;


//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: GetDriverNameWorker
//
// Description:
//      This function is the get Component Driver Name.
//
//  Input:
//    IN  EFI_GUID     *ProtocolGuid        -  Driver image guid
//    IN  EFI_HANDLE    DriverBindingHandle -  Driver image handle.
//    OUT CHAR16      **DriverName          -  Driver name string.
//
//  Output: VOID
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GetDriverNameWorker (
  IN  EFI_GUID       *ProtocolGuid,
  IN  EFI_HANDLE      DriverBindingHandle,
  OUT CHAR16        **DriverName
  )
{
  EFI_STATUS                    Status;
  EFI_COMPONENT_NAME_PROTOCOL  *ComponentName =NULL;

  Status = pBS->OpenProtocol (
                    DriverBindingHandle,
                    ProtocolGuid,
                    (VOID **)&ComponentName,
                    NULL,
                    NULL,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = ComponentName->GetDriverName (
                             ComponentName,
                             ComponentName->SupportedLanguages,
                             DriverName
                             );
  return Status;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: LibGetDriverName
//
// Description:
//      This function is bring the EfiComponentNameProtocolGuid/
//      EfiComponentName2ProtocolGuid into the GetDriverNameWorker function
//      to get the driver name.
//
//  Input:
//     IN  EFI_HANDLE  DriverBindingHandle -  Driver image handle.
//     OUT CHAR16             **DriverName -  Driver name string.
//
//  Output: VOID
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
LibGetDriverName (
  IN  EFI_HANDLE   DriverBindingHandle,
  OUT CHAR16     **DriverName
  )
{
  EFI_STATUS    Status;

  Status = GetDriverNameWorker (&gEfiComponentName2ProtocolGuid,
                                      DriverBindingHandle, DriverName);
  if (EFI_ERROR (Status)) {
    Status = GetDriverNameWorker (&gEfiComponentNameProtocolGuid,
                                      DriverBindingHandle, DriverName);
  }
  return Status;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: WCmpStrNonCase
//
// Description:
//      This function is the ignore uppercase or lowercase to compare two
//      unicode string whether are same.
//
//  Input:
//          String1 - first string.
//          String2 - second string.
//
//  Output:
//             zero - is same string.
//   not equal zero - is different string.
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
UINTN
WCmpStrNonCase (CHAR16* String1, CHAR16* String2)
{
  CHAR16 TempString1[64] ={0}, TempString2[64] ={0};

  Wcscpy (TempString1, String1);
  Wcsupr (TempString1);
  Wcscpy (TempString2, String2);
  Wcsupr (TempString2);
  return Wcscmp(TempString1, TempString2);
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: CheckTheVideoSelect
//
// Description:
//      This function is the return current select VGA is the Onboard/
//      Offboard device.
//
//  Input: VOID
//
//  Output:
//      1 - OnBoard  VGA (ASPEED)
//      2 - OffBoard VGA (PCIE)
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
UINTN
CheckTheVideoSelect (VOID)
{
  EFI_STATUS   Status;
  UINT16       VenID =0, DevID =0, PCIR =*(UINT16*)0x0c0018;
  UINT32       ClassCode =0;
  EFI_HANDLE  *DriverImageHandleBuffer =NULL;
  UINTN        DriverImageHandleCount  =0;
  CHAR16      *DriverName =NULL;
  UINTN        i =0, VideoSelect =2;

  ClassCode = *(UINT32*)(0x0c0000 + PCIR + 0x0d);
  ClassCode = ((ClassCode >> 8) & 0x0ffff);
  if (ClassCode == 0x0300)
  {
     VenID = *(UINT16*)(0x0c0000 + PCIR + 0x04);
     DevID = *(UINT16*)(0x0c0000 + PCIR + 0x06);
     if ((VenID == 0x1a03) && (DevID == 0x2000))
     {
         VideoSelect =1;
         goto _EXIT;
     }
  }
  else
  {
    Status = pBS->LocateHandleBuffer (
                        ByProtocol, &gEfiDriverBindingProtocolGuid,
                        NULL,
                        &DriverImageHandleCount,
                        &DriverImageHandleBuffer);
    if (!EFI_ERROR (Status))
    {
      for (i = 0; i < DriverImageHandleCount; i++)
      {
         Status = LibGetDriverName (DriverImageHandleBuffer[i], &DriverName);
         if (!EFI_ERROR (Status))
         {
            if (WCmpStrNonCase(DriverName, L"ASPEED Graphics Driver") == 0)
            {
                VideoSelect =1;
                goto _EXIT;
            }
         }
      }
    }
  }
_EXIT:
  return VideoSelect;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: _Find_AddonVGA_BDF
//
// Description:
//      This function is the scan VGA device, and store device location
//      to variable.
//
//  Input:
//      IN  UINT8  BUS  -  Input the Pci Bus.
//      IN  UINT8  DEV  -  Input the Pci Device.
//      IN  UINT8  FUN  -  Input the Pci Function.
//
//  Output:
//      return 0  -  can't find VGA device.
//      return 1  -  find OnBoard Aspeed VGA Card.
//      return 2  -  find Addon VGA Card. (ATI VGA or NVIDIA VGA)
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
_Find_AddonVGA_BDF (
  IN  UINT8          BUS,
  IN  UINT8          DEV,
  IN  UINT8          FUN  )
{
  UINT16  VID = 0, DID = 0;
  UINT8   VGADEV = 0;

  if ((PciRead8(PCI_LIB_ADDRESS(BUS, DEV, FUN, 0x0a)) == 0x00) &&
      (PciRead8(PCI_LIB_ADDRESS(BUS, DEV, FUN, 0x0b)) == 0x03))
  {
     TRACE ((TRACE_ALWAYS, "## _Find_AddonVGA_BDF ##\n"));
     VID = PciRead16(PCI_LIB_ADDRESS(BUS, DEV, FUN, 00));
     DID = PciRead16(PCI_LIB_ADDRESS(BUS, DEV, FUN, 02));
     DumpPciVgaDev (BUS, DEV, FUN);
     // OnBoard Aspeed VGA Card.
     if ((VID == 0x01A03) && (DID == 0x2000))
     {
          VGADEV = 1;
          AspeedVGA_BDF.BUS = BUS;  AspeedVGA_BDF.DEV = DEV;
          AspeedVGA_BDF.FUN = FUN;
          goto _EXIT;
     }
     // ATI VGA or NVIDIA VGA Card.
     if ((VID == 0x1002) || (VID == 0x10DE))
     {
        if (PciRead8(PCI_LIB_ADDRESS(BUS, DEV, FUN, 0x04)) == 0x07)
        {
            VGADEV = 2;
            AddonVGA_BDF.BUS = BUS;  AddonVGA_BDF.DEV = DEV;
            AddonVGA_BDF.FUN = FUN;
            goto _EXIT;
        }
     }
  }
_EXIT:
   return (VGADEV);
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: _Find_RootPort_BDF
//
// Description:
//      This function is the scan RootPort device, and store device location
//      to variable.
//
//  Input:
//      IN  UINT8  BUS  -  Input the Pci Bus.
//      IN  UINT8  DEV  -  Input the Pci Device.
//      IN  UINT8  FUN  -  Input the Pci Function.
//
//  Output: VOID
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
_Find_RootPort_BDF (
  IN  UINT8          BUS,
  IN  UINT8          DEV,
  IN  UINT8          FUN  )
{
  UINT8    SecBus = 0, SubBus = 0, VGADEV = 0;
  UINT8    i = 0, j = 0, z = 0;

  if ((PciRead16(PCI_LIB_ADDRESS(BUS, DEV, FUN, 0x00)) == 0x8086) &&
      (PciRead8(PCI_LIB_ADDRESS(BUS, DEV, FUN, 0x04)) == 0x07) &&
      (PciRead8(PCI_LIB_ADDRESS(BUS, DEV, FUN, 0x0a)) == 0x04) &&
      (PciRead8(PCI_LIB_ADDRESS(BUS, DEV, FUN, 0x0b)) == 0x06) )
  {
     TRACE ((TRACE_ALWAYS, "## _Find_RootPort_BDF ##\n"));
     SecBus = PciRead8(PCI_LIB_ADDRESS(BUS, DEV, FUN, 0x19));
     SubBus = PciRead8(PCI_LIB_ADDRESS(BUS, DEV, FUN, 0x1A));
     for (i = SecBus; i <= SubBus; i++) {
       for (j = 0; j <= 0x1f; j++) {
       for (z = 0; z <= 0x08; z++) {
         VGADEV = _Find_AddonVGA_BDF (i, j, z);
         if (VGADEV != 0)
         {
           DumpPciBrigde (BUS, DEV, FUN);
           // OnBoard Aspeed VGA Card.
           if (VGADEV == 1)
           {
               AspeedRPT_BDF.BUS = BUS; AspeedRPT_BDF.DEV = DEV;
               AspeedRPT_BDF.FUN = FUN;
               goto _EXIT;
           }
           // ATI VGA or NVIDIA VGA Card.
           if (VGADEV == 2)
           {
               FindVgaDev = TRUE;
               AddonRPT_BDF.BUS = BUS; AddonRPT_BDF.DEV = DEV;
               AddonRPT_BDF.FUN = FUN;
               goto _EXIT;
           }
         }
       }
       }
     }
  }
_EXIT:
  return;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: SwitchOfVGADevice
//
// Description:
//      This function is the turn On or turn Off the VGA device.
//
//  Input:
//      IN  BOOLEAN          SW   -  Input the turn On of Off value.
//      IN  _PCIDEVICE_BDF  *RPT  -  Input the RootPort device variable.
//      IN  _PCIDEVICE_BDF  *VGA  -  Input the VGA device variable.
//
//  Output: VOID
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID SwitchOfVGADevice (
  IN  BOOLEAN                SW,
  IN  _PCIDEVICE_BDF        *RPT,
  IN  _PCIDEVICE_BDF        *VGA
  )
{
   TRACE ((TRACE_ALWAYS, "## Bus:(%d)-Dev:(%d)-Fun(%d)_VID:(%04x):DID:(%04x)_", \
                                VGA->BUS, VGA->DEV, VGA->FUN, \
                                PciRead16(PCI_LIB_ADDRESS(VGA->BUS, VGA->DEV, VGA->FUN, 00)), \
                                PciRead16(PCI_LIB_ADDRESS(VGA->BUS, VGA->DEV, VGA->FUN, 02))));
   if (SW == TurnOn)
   {
       TRACE ((TRACE_ALWAYS, "Enable VGA Device ##\n"));
       PciWrite8(PCI_LIB_ADDRESS(VGA->BUS, VGA->DEV, VGA->FUN, 0x04), 0x07);
       PciWrite8(PCI_LIB_ADDRESS(RPT->BUS, RPT->DEV, RPT->FUN, 0x3e), 0x18);
   }
   else
   {
       TRACE ((TRACE_ALWAYS, "Disable VGA Device ##\n"));
       PciWrite8(PCI_LIB_ADDRESS(VGA->BUS, VGA->DEV, VGA->FUN, 0x04), 0x00);
       PciWrite8(PCI_LIB_ADDRESS(RPT->BUS, RPT->DEV, RPT->FUN, 0x3e), 0x10);
   }
   DumpPciBrigde (RPT->BUS, RPT->DEV, RPT->FUN);
   DumpPciVgaDev (VGA->BUS, VGA->DEV, VGA->FUN);
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: TraceRootPortAndAddonVGABus
//
// Description:
//      This function is the trace RootPort device and VGA device,
//      from Bus:0 - Bus:0x80
//
//  Input: VOID
//
//  Output: VOID
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID TraceRootPortAndAddonVGABus (VOID)
{
  UINT8  Bus = 0, DEV = 0, FUN = 0;
  
  if (pmSmcSimpleTextOutProtocol == NULL) {
    pBS->LocateProtocol(
           &gSmcSimpleTextOutProtocolGuid,
           NULL,
           (VOID **) &pmSmcSimpleTextOutProtocol
           );
  }

  // CPU1 form Bus:0
  for (DEV = 0; DEV <= 0x1f; DEV++) {
   for (FUN = 0; FUN <= 0x08; FUN++) {
        _Find_RootPort_BDF (0x00, DEV, FUN);
   }
  }
  // CPU2 form Bus:0x80
  for (DEV = 0; DEV <= 0x1f; DEV++) {
   for (FUN = 0; FUN <= 0x08; FUN++) {
        _Find_RootPort_BDF (0x80, DEV, FUN);
   }
  }
  Bus = PciRead8(PCI_LIB_ADDRESS(AspeedRPT_BDF.BUS, AspeedRPT_BDF.DEV, AspeedRPT_BDF.FUN, 0x19));
  if (FindVgaDev == TRUE) {
	PciWrite8(PCI_LIB_ADDRESS(Bus, 0, 0, 0x3e), 0x18);
      SwitchOfVGADevice (TurnOff, &AddonRPT_BDF, &AddonVGA_BDF);
      SwitchOfVGADevice (TurnOn , &AspeedRPT_BDF, &AspeedVGA_BDF);
	
      TRACE ((TRACE_ALWAYS, "## write string to screen ##\n"));
      pmSmcSimpleTextOutProtocol->OutputString (00, 23, 0x04, "  iKVM doesn't support add-on VGA device.");
      pmSmcSimpleTextOutProtocol->OutputString (00, 24, 0x04, "  Please change the D-SUB connector to Add-on VGA device...");

      PciWrite8(PCI_LIB_ADDRESS(Bus, 0, 0, 0x3e), 0x00);
      SwitchOfVGADevice (TurnOff, &AspeedRPT_BDF, &AspeedVGA_BDF);
      SwitchOfVGADevice (TurnOn , &AddonRPT_BDF , &AddonVGA_BDF);
  }
  return;
}
