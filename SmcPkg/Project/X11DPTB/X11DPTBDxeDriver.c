//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.02
//    Bug Fix:  To avoid system hang 0xEE when install SKX-F CPU on system problem.
//    Reason:   Temp solution.
//    Auditor:  Jacker Yeh
//    Date:     Sep/27/2016
//
//  Rev. 1.01
//    Bug Fix:  Add get MAC address for SIOM card.
//    Reason:   Refer from Grantley X10DRT-PS.
//    Auditor:  Jacker Yeh
//    Date:     Jul/14/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Jacker Yeh
//    Date:     Feb/04/2016
//
//****************************************************************************
//****************************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		ProjectDxeDriver.c
//
// Description:
//  This file implement function of PROJECTDRIVER_PROTOCOL
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include <Token.h>
//#include <AmiDxeLib.h>
#include <Library\UefiBootServicesTableLib.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include "SmcSetupModify.h"
#include "HideSetupItem.h"
#include "SmcLib.h"
#include <Protocol/SmbusHc.h>
#include <Guid\EventGroup.h>
#include <Library\GpioLib.h>
#include <GpioPinsSklH.h>
#include <Library/MmPciBaseLib.h>
#include "X11DPTBDxeDriver.h"
#include "X11DPTBVPD.h"
#include <Protocol/AmiSmbios.h>

#define IIO_BIFURCATE_x4x4x4x4  0
#define IIO_BIFURCATE_x4x4xxx8  1
#define IIO_BIFURCATE_xxx8x4x4  2
#define IIO_BIFURCATE_xxx8xxx8  3
#define IIO_BIFURCATE_xxxxxx16  4

EFI_GUID gProjectOwnProtocolGuid  = EFI_PROJECT_OWN_PROTOCOL_GUID;
extern	EFI_GUID	gBdsAllDriversConnectedProtocolGuid;
UINT8 LAN_NUMBER;
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  OVERRIDE_IOBP
//
// Description :  override IOBP setting for USB3/ SATA SI
//
// Parameters:  None
//
// Returns     :  Override IOBP setting in PEI phase
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID OVERRIDE_IOBP (VOID)
{
	DEBUG((EFI_D_INFO, "OVERRIDE_IOBP entry.\n"));

	DEBUG((EFI_D_INFO, "OVERRIDE_IOBP end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  ProjectCheckAdjustVID
//
// Description :  Send out the Project Adjust VID
//
// Parameters  :  NONE
//
// Returns     :  Project Adjust VID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT8 EFIAPI ProjectCheckAdjustVID(VOID)
{
    return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  GetGPUDID
//
// Description :  Get GPU device id
//
// Parameters  :  NONE
//
// Returns     :  Get GPU device id
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT16 EFIAPI GetGPUDID(VOID)
{
    return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  ProjectGetBID
//
// Description :  Send out the Project BID
//
// Parameters  :  NONE
//
// Returns     :  Project BID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT8 EFIAPI GetProjectBID(VOID)
{
    return 0x00;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  ProjectCheckBMCPresent
//
// Description :  Check BMC Present or not
//
// Parameters  :  NONE
//
// Returns     :  1: BMC Present  0: MBC not present
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT8 EFIAPI ProjectCheckBMCPresent(VOID)
{
    UINT8 ReturnVal = 1;
    return ReturnVal;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  GetOnBoardLanMacAddress
//
// Description: 
//  Get onboard lan Mac address in PEI phase. BIOS should decode
//  PCIE root port bus temporary and then to set Lan resource to
//  get Mac address. BIOS must remove all temporary setting before
//  exit this routine
//
// Input:
//      IN UINT16 LanRootPortCfg - Onbaord lan pcie root port BDF
//      IN OUT UINT8 *LanMacAddressStart1 - Point to start address of LAN 1 MAC structure
//      IN OUT UINT8 *LanMacAddressStart2 - Point to start address of LAN 2 MAC structure
//
// Output:      
//      EFI_SUCCESS (return)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GetOnBoardLanMacAddress (
    IN UINT16 LanRootPortCfg,
    OUT UINT8 *LanMacAddressStart1,
    OUT UINT8 *LanMacAddressStart2
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32 Data32, BusNum;
    BOOLEAN Is710 = FALSE, IsBroadcom = FALSE;
    UINT8 LanRootPortBus = (LanRootPortCfg >> 8);
    UINT8 LanRootPortDev = ((LanRootPortCfg >> 3) & 0x1F);
    UINT8 LanRootPortFun = (LanRootPortCfg & 0x07);
    UINT8 LanBus, Temp1, NumberOfLanPorts, i, j, RetryCount;
    UINT32 SubSystemID = 0;
    UINT32 LanOffset = 0x5400;
    UINTN LanResourceAddress;
    UINT8 *LanMACBuffer = 0;
    UINT8 *LanMacAddressStart = 0;
    UINT32 ReturnVal0 = 0, ReturnVal1 = 0, ReturnVal2 = 0, ReturnVal3 = 0, SIOM_ID = 0;

//Check SIOM GPIO for special SIOM card.
/*
    AOC-MTG-I4T/I2T   : 1100b
    AOC-MHIBF-2QG/1QG : 1010b
    AOC-MHIBF-m2Q2G
    AOC-MHIBF-m1Q2G
    AOC-MH25G-b2S2G   : 1001b
    AOC-MH25G-m2S2T   : 0111b
    AOC-ML-4S         : 0001b
*/
    GpioGetInputValue(SIOM_ID0, &ReturnVal0);
    GpioGetInputValue(SIOM_ID1, &ReturnVal1);
    GpioGetInputValue(SIOM_ID2, &ReturnVal2);
    GpioGetInputValue(SIOM_ID3, &ReturnVal3);   
    SIOM_ID = (ReturnVal3 << 3) | (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;

    //X550 W/A
    DEBUG((EFI_D_INFO, "X11DPTB X550 W/A.\n"));
    RetryCount = GetCmosTokensValue (Q_X550WA_0);
    
    if(((SIOM_ID == 0x0C) || (SIOM_ID == 0x07))) {
      if (PciRead8(PCI_LIB_ADDRESS(LanRootPortBus, LanRootPortDev, LanRootPortFun, R_PCIE_PORT_PXPLNKSTS)) != 0x43) {
	if (RetryCount < 5) {
          SetCmosTokensValue(Q_X550WA_0, RetryCount+1);
          IoWrite8(0xCF9, 0x06);	// reset system
	}
      } else {
        SetCmosTokensValue(Q_X550WA_0, 0);
      }
    }

    if(SIOM_ID == 0x01) {
        BusNum = PciRead32(PCI_LIB_ADDRESS(LanRootPortBus+2, 0x03, 0x00, 0x18));
    } else {
        Data32 = PciRead32(PCI_LIB_ADDRESS(LanRootPortBus, LanRootPortDev, LanRootPortFun, 0x0));
        BusNum = PciRead32(PCI_LIB_ADDRESS(LanRootPortBus, LanRootPortDev, LanRootPortFun, 0x18));
    }

    LanBus = (BusNum >> 8) & 0xFF;

    if (LanBus != 0) {

        //Check second X550 chip on AOC-MTG-I4T
        RetryCount = GetCmosTokensValue (Q_X550WA_1);
        if((SIOM_ID == 0x0C) && (PciRead32(PCI_LIB_ADDRESS(LanBus, 0, 0, 0x2C)) == 0x091F15D9)) {
          if (PciRead8(PCI_LIB_ADDRESS(LanRootPortBus, LanRootPortDev-2, LanRootPortFun, R_PCIE_PORT_PXPLNKSTS)) != 0x43) {
            if (RetryCount < 5) {
              SetCmosTokensValue(Q_X550WA_1, RetryCount+1);
              IoWrite8(0xCF9, 0x06);	// reset system
            }
          } else {
            SetCmosTokensValue(Q_X550WA_1, 0);
          }
        }

        DEBUG((EFI_D_INFO, "X11DPTB GetOnBoardLanMacAddress start.\n"));

        Data32 = PciRead32(PCI_LIB_ADDRESS(LanBus, 0, 0, 0));
        if ((Data32 == 0x15728086) || (Data32 == 0x37D08086))   //710, 722 SFP+
        {
          Is710 = TRUE;
          LanOffset = 0x1E2120;
        }

        SubSystemID  = PciRead32(PCI_LIB_ADDRESS(LanBus, 0, 0, 0x2C));

   	if(Is710)
          *LanMacAddressStart2 = 3;
        else if((PciRead32(PCI_LIB_ADDRESS(LanBus, 0, 0, 0))==0x15338086) || (PciRead32(PCI_LIB_ADDRESS(LanBus, 0, 0, 0))==0x24F08086))	//I210 or Omni-Path
          *LanMacAddressStart2 = 0;
        else //These card only has two port
          *LanMacAddressStart2 = 1;

        switch(SubSystemID)
        {
          case 0x091315D9:  //AOC-MG-i4         I350
          case 0x091F15D9:  //AOC-MTG-i4T       X550
          case 0x094915D9:  //AOC-MH25G-m2S2T	X550 + Mellanox 25G
          case 0x094E15D9:  //AOC-MH25G-b2S2G	I350 + Broadcom 25G
          case 0x098315D9:  //AOC-M25G-m4S      Mellanox 25G
            *LanMacAddressStart2 = 3;
            break;
          default:
            break;
        }
        LAN_NUMBER = *LanMacAddressStart2;
        //Allocate memory buffer for LAN MAC Address data
        Status = gBS->AllocatePool(EfiBootServicesData, VPD_NUMBER_OF_LAN*VpdLanMACSize, &LanMACBuffer);
        if (EFI_ERROR(Status)) {
            DEBUG((EFI_D_INFO, "VPD_DEBUG : Can't locate memory for LAN MAC address data\n"));
        } else {
            gBS->SetMem(LanMACBuffer, VPD_NUMBER_OF_LAN*VpdLanMACSize, 0x00);
            DEBUG((EFI_D_INFO, "VPD_DEBUG : Locate memory for LAN MAC address data successful\n"));
            Status = X11DPTBVPDDataCollect(LanBus, LanMACBuffer);
        }

        if((SIOM_ID == 0x01) || (EFI_ERROR(Status))) {
          DEBUG((EFI_D_INFO, "Use Original Way to pass LAN MAC address data\n"));
       	  NumberOfLanPorts = PcdGet8(PcdSmcMultiFuncLanChip1);

          //
          // Lan 1
          //

          if ((PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (0%NumberOfLanPorts), 0)) == 0x16D114E4) || ( PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (0%NumberOfLanPorts), 0)) == 0x16D714E4)) {
            IsBroadcom = TRUE;
            LanResourceAddress = (UINT32)MmPciAddress(0, LanBus, 0, (0%NumberOfLanPorts), 0x140);
          } else {
            IsBroadcom = FALSE;
            Temp1 = PciRead8(PCI_LIB_ADDRESS(LanBus, 0, (0%NumberOfLanPorts), 0x04));
            PciWrite8(PCI_LIB_ADDRESS(LanBus, 0, (0%NumberOfLanPorts), 0x04), 0x07);
            Data32 = PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (0%NumberOfLanPorts), 0x10));
            LanResourceAddress = (Data32 & 0xFFFFFFF0) + LanOffset;
          }

          for (i = 0; i < 6; i++) {
            if (PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (0%NumberOfLanPorts), 0)) == 0x101515B3) {
              *(LanMacAddressStart1 + i) = 0xFF;
            } else if(Is710){ 
              if(i < 4)
                *(LanMacAddressStart1 + i) = *(UINT8*)(LanResourceAddress + i);
              else
                *(LanMacAddressStart1 + i) = *(UINT8*)(LanResourceAddress + 0x20 + i);
            } else if(IsBroadcom) {
                if (i < 3)
                  *(LanMacAddressStart1 + i) = *(UINT8*)(LanResourceAddress + (7-i));
                else if (i < 5)
                  *(LanMacAddressStart1 + i) = *(UINT8*)(LanResourceAddress + (5-i));
                else
                  *(LanMacAddressStart1 + i) = *(UINT8*)(LanResourceAddress + (5-i)) + (0%NumberOfLanPorts);
            } else {          
                *(LanMacAddressStart1 + i) = *(UINT8*)(LanResourceAddress + i);
            }
          }

//          if (!IsBroadcom)
//            PciWrite8(PCI_LIB_ADDRESS(LanBus, 0, (0%NumberOfLanPorts), 0x04), Temp1);

          //
          // Lan 2
          //

          if (NumberOfLanPorts == 1) {
            LanRootPortCfg = SmcBDFTransfer(PcdGet32(PcdSmcOBLan2BDF));
            Data32 = PciRead32(PCI_LIB_ADDRESS(((LanRootPortCfg >> 8) & 0xFF), ((LanRootPortCfg >> 3) & 0x1F), (LanRootPortCfg & 0x07), 0x0));
            BusNum = PciRead32(PCI_LIB_ADDRESS(((LanRootPortCfg >> 8) & 0xFF), ((LanRootPortCfg >> 3) & 0x1F), (LanRootPortCfg & 0x07), 0x18));
            LanBus = (BusNum >> 8) & 0xFF;
          }

          if ((PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (1%NumberOfLanPorts), 0)) == 0x16D114E4) || ( PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (1%NumberOfLanPorts), 0)) == 0x16D714E4)) {
            IsBroadcom = TRUE;
            LanResourceAddress = (UINT32)MmPciAddress(0, LanBus, 0, (1%NumberOfLanPorts), 0x140);
          } else {
            IsBroadcom = FALSE;
            Temp1 = PciRead8(PCI_LIB_ADDRESS(LanBus, 0, (1%NumberOfLanPorts), 0x04));
            PciWrite8(PCI_LIB_ADDRESS(LanBus, 0, (1%NumberOfLanPorts), 0x04), 0x07);
            Data32 = PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (1%NumberOfLanPorts), 0x10));
            LanResourceAddress = (Data32 & 0xFFFFFFF0) + LanOffset;
          }

          for (i = 0; i < 6; i++) {
            if (PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (1%NumberOfLanPorts), 0)) == 0x101515B3) {
              *(LanMacAddressStart2 + 1 + i) = 0xFF;
            } else if(Is710){ 
              if(i < 4)
                *(LanMacAddressStart2 + 1 + i) = *(UINT8*)(LanResourceAddress + i);
              else
                *(LanMacAddressStart2 + 1 + i) = *(UINT8*)(LanResourceAddress + 0x20 + i);
            } else if(IsBroadcom) {
                if (i < 3)
                  *(LanMacAddressStart2 + 1 + i) = *(UINT8*)(LanResourceAddress + (7-i));
                else if (i < 5)
                  *(LanMacAddressStart2 + 1 + i) = *(UINT8*)(LanResourceAddress + (5-i));
                else
                  *(LanMacAddressStart2 + 1 + i) = *(UINT8*)(LanResourceAddress + (5-i)) + (1%NumberOfLanPorts);
            } else {          
                *(LanMacAddressStart2 + 1 + i) = *(UINT8*)(LanResourceAddress + i);
            }
          }

//          if (!IsBroadcom)
//            PciWrite8(PCI_LIB_ADDRESS(LanBus, 0, (1%NumberOfLanPorts), 0x04), Temp1);
	
          //
          // Lan 3
          //

          if ((NumberOfLanPorts == 1) || (NumberOfLanPorts == 2)) {
            LanRootPortCfg = SmcBDFTransfer(PcdGet32(PcdSmcOBLan3BDF));
            Data32 = PciRead32(PCI_LIB_ADDRESS(((LanRootPortCfg >> 8) & 0xFF), ((LanRootPortCfg >> 3) & 0x1F), (LanRootPortCfg & 0x07), 0x0));
            BusNum = PciRead32(PCI_LIB_ADDRESS(((LanRootPortCfg >> 8) & 0xFF), ((LanRootPortCfg >> 3) & 0x1F), (LanRootPortCfg & 0x07), 0x18));
            LanBus = (BusNum >> 8) & 0xFF;
          }
        
          if ((PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (2%NumberOfLanPorts), 0)) == 0x16D114E4) || ( PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (2%NumberOfLanPorts), 0)) == 0x16D714E4)) {
            IsBroadcom = TRUE;
            LanResourceAddress = (UINT32)MmPciAddress(0, LanBus, 0, (2%NumberOfLanPorts), 0x140);            
          } else {
            IsBroadcom = FALSE;
            Temp1 = PciRead8(PCI_LIB_ADDRESS(LanBus, 0, (2%NumberOfLanPorts), 0x04));
            PciWrite8(PCI_LIB_ADDRESS(LanBus, 0, (2%NumberOfLanPorts), 0x04), 0x07);
            Data32 = PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (2%NumberOfLanPorts), 0x10));
            LanResourceAddress = (Data32 & 0xFFFFFFF0) + LanOffset;
          }

          for (i = 0; i < 6; i++) {
            if (PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (2%NumberOfLanPorts), 0)) == 0x101515B3) {        	  
              *(LanMacAddressStart2 + 7 + i) = 0xFF;
            } else if(Is710){ 
              if(i < 4)
                *(LanMacAddressStart2 + 7 + i) = *(UINT8*)(LanResourceAddress + i);
              else
                *(LanMacAddressStart2 + 7 + i) = *(UINT8*)(LanResourceAddress + 0x20 + i);
            } else if(IsBroadcom) {
                if (i < 3)
                  *(LanMacAddressStart2 + 7 + i) = *(UINT8*)(LanResourceAddress + (7-i));
                else if (i < 5)
                  *(LanMacAddressStart2 + 7 + i) = *(UINT8*)(LanResourceAddress + (5-i));
                else
                  *(LanMacAddressStart2 + 7 + i) = *(UINT8*)(LanResourceAddress + (5-i)) + (2%NumberOfLanPorts);
            } else {          
                *(LanMacAddressStart2 + 7 + i) = *(UINT8*)(LanResourceAddress + i);
            }
          }

//          if (!IsBroadcom)
//            PciWrite8(PCI_LIB_ADDRESS(LanBus, 0, (2%NumberOfLanPorts), 0x04), Temp1);
		
          //
          // Lan 4
          //

          if (NumberOfLanPorts == 1) {
            LanRootPortCfg = SmcBDFTransfer(PcdGet32(PcdSmcOBLan4BDF));
            Data32 = PciRead32(PCI_LIB_ADDRESS(((LanRootPortCfg >> 8) & 0xFF), ((LanRootPortCfg >> 3) & 0x1F), (LanRootPortCfg & 0x07), 0x0));
            BusNum = PciRead32(PCI_LIB_ADDRESS(((LanRootPortCfg >> 8) & 0xFF), ((LanRootPortCfg >> 3) & 0x1F), (LanRootPortCfg & 0x07), 0x18));
            LanBus = (BusNum >> 8) & 0xFF;
          }

          if ((PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (3%NumberOfLanPorts), 0)) == 0x16D114E4) || ( PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (3%NumberOfLanPorts), 0)) == 0x16D714E4)) {
            IsBroadcom = TRUE;
            LanResourceAddress = (UINT32)MmPciAddress(0, LanBus, 0, (3%NumberOfLanPorts), 0x140);
          } else {
            IsBroadcom = FALSE;
            Temp1 = PciRead8(PCI_LIB_ADDRESS(LanBus, 0, (3%NumberOfLanPorts), 0x04));
            PciWrite8(PCI_LIB_ADDRESS(LanBus, 0, (3%NumberOfLanPorts), 0x04), 0x07);
            Data32 = PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (3%NumberOfLanPorts), 0x10));
            LanResourceAddress = (Data32 & 0xFFFFFFF0) + LanOffset;
          }

          for (i = 0; i < 6; i++) {
            if (PciRead32(PCI_LIB_ADDRESS(LanBus, 0, (3%NumberOfLanPorts), 0)) == 0x101515B3) {        	  
              *(LanMacAddressStart2 + 13 + i) = 0xFF;
            } else if(Is710){ 
              if(i < 4)
                *(LanMacAddressStart2 + 13 + i) = *(UINT8*)(LanResourceAddress + i);
              else
                *(LanMacAddressStart2 + 13 + i) = *(UINT8*)(LanResourceAddress + 0x20 + i);
            } else if(IsBroadcom) {
                if (i < 3)
                  *(LanMacAddressStart2 + 13 + i) = *(UINT8*)(LanResourceAddress + (7-i));
                else if (i < 5)
                  *(LanMacAddressStart2 + 13 + i) = *(UINT8*)(LanResourceAddress + (5-i));
                else
                  *(LanMacAddressStart2 + 13 + i) = *(UINT8*)(LanResourceAddress + (5-i)) + (3%NumberOfLanPorts);
            } else {          
                *(LanMacAddressStart2 + 13 + i) = *(UINT8*)(LanResourceAddress + i);
            }
          }

//          if (!IsBroadcom)
//            PciWrite8(PCI_LIB_ADDRESS(LanBus, 0, (3%NumberOfLanPorts), 0x04), Temp1);

        } else {
          DEBUG((EFI_D_INFO, "Use VPD to pass LAN MAC address data\n"));
          for(j=0; j<4; j++)
          {
            if(j == 0)
              LanMacAddressStart = LanMacAddressStart1;
            else
              LanMacAddressStart = (LanMacAddressStart2 + 1 + ((j-1)*6));

            DEBUG((EFI_D_INFO, "VPD LAN %x MAC address = ", (j+1)));
            for(i=0; i<12; i++)
            {
              Temp1 = LanMACBuffer[(j*12)+i];
              DEBUG((EFI_D_INFO, "%02x ", Temp1));
              if((Temp1 >= '0')&&(Temp1 <= '9'))
                Temp1 -= '0';
              else if((Temp1 >= 'A')&&(Temp1 <= 'F'))
                Temp1 -= 'A'-0xA;
              if(i%2==0)
                *LanMacAddressStart = (Temp1 << 4);
              else {
                *LanMacAddressStart |= Temp1;
                LanMacAddressStart+=1;
              }
            }
            DEBUG((EFI_D_INFO, "\n"));
          }
        } //VPD
        pBS->FreePool(LanMACBuffer);
    }
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  ProjectHardwareInfo
//
// Description :  Create Project own Hardware Information
//
// Parameters  :  OUT: HardWareInfoData
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI ProjectHardwareInfo(
	OUT UINT8 *HardWareInfoData,
	OUT UINT8 *LanMacInfoData
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    Status = GetOnBoardLanMacAddress(SmcBDFTransfer(PcdGet32(PcdSmcOBLan1BDF)), (HardWareInfoData + 4), (LanMacInfoData));

    // LanMacInfoData parameter data is used by 0x9E command
    // IPMI 0x9E command data:
    // Byte 0    : LAN number N  (exclude NumLan of 0x30 0x20 command)
    // Byte 1-6  : LAN port M MAC address
    // Byte 7-12 : LAN port M+1 MAC address
    //Update in GetOnBoardLanMacAddress for this project *LanMacInfoData = 0x01;
 
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  CheckAudioHandle
//
// Description :  Handle audio function in board level
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI CheckAudioHandle(VOID)
{
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  GetProjectPciEBifurcate
//
// Description :  Handle audio function in board level
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI GetProjectPciEBifurcate(IN OUT IIO_CONFIG *SetupData)
{
    DEBUG((EFI_D_INFO, "GetProjectPciEBifurcate DXE Start.\n"));
    DEBUG((EFI_D_INFO, "GetProjectPciEBifurcate End.\n"));
    return EFI_SUCCESS;
}

/*-----------------12/23/2014 8:39PM----------------
 * Coding for every platform
 * --------------------------------------------------*/
/*
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  SetProjectPciECTLE
//
// Description :  Handle PciE CTLE override in board level
// 		Instance	Function	Port	Description
//		0		D6:F0 		0	DMI without Gen3 support
//		1		D6:F1		1	Port 1 CTLE
//		3		D6:F3		2	Port 2 CTLE
//		7		D7:F0		3	Port 3 CTLE
// Input:
//	IN IIO_GLOBALS	*IioGlobalData
//	IN UINT8	Iio
//	IN UINT8	BusNumber
//
// Output:      
//      EFI_SUCCESS (return)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>

EFI_STATUS EFIAPI
SetProjectPciECTLE(
    IN IIO_GLOBALS	*IioGlobalData,
    IN UINT8	Iio,
    IN UINT8	BusNumber
)
{
    TRACE((-1, "SetProjectPciECTLE entry.\n"));
    TRACE((-1, "SetProjectPciECTLE end.\n"));
    return EFI_SUCCESS;
}
 */

PROJECTDRIVER_PROTOCOL ProjectInterface =
{
    ProjectCheckAdjustVID,
    GetGPUDID,
    GetProjectBID,
    NULL,
    ProjectHardwareInfo,
    ProjectCheckBMCPresent,
    CheckAudioHandle,
    GetProjectPciEBifurcate,
    NULL,
    NULL,
    NULL
};


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   X11DPTBReadyToBootCallBack
//
// Description: Run at ready to boot.
//
// Input:       IN EFI_EVENT   Event
//              IN VOID        *Context
//
// Output:      None
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
X11DPTBReadyToBootCallBack (
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS Status = EFI_SUCCESS;
	DEBUG((EFI_D_INFO, "X11DPTBReadyToBootCallBack Entry.\n"));
    gBS->CloseEvent(Event);
}

VOID
Type41SmbiosUpdate(
    IN	EFI_EVENT	Event,
    IN	VOID		*Context
)
{
    EFI_STATUS  Status;
    UINT8       *TempBuffer;
    UINT16	TempBufferSize;
    UINT16          Handle;
    AMI_SMBIOS_PROTOCOL	*SmbiosProtocol;

    TRACE((-1, "LAN_NUMBER = %x.\n", LAN_NUMBER));

    Status = pBS->LocateProtocol(&gAmiSmbiosProtocolGuid, NULL, &SmbiosProtocol);
    if(Status)	return;
//Update for type41
    if(LAN_NUMBER <= 1){//Delete LAN3 and 4
    Status = SmbiosProtocol->SmbiosReadStrucByType(41, 5, &TempBuffer, &TempBufferSize);
        if (!EFI_ERROR(Status)) {
            Handle = ((SMBIOS_STRUCTURE_HEADER*)TempBuffer)->Handle;
            SmbiosProtocol->SmbiosDeleteStructure(Handle); //4th LAN
            pBS->FreePool(TempBuffer);
        }
	        
    Status = SmbiosProtocol->SmbiosReadStrucByType(41, 4, &TempBuffer, &TempBufferSize);
        if (!EFI_ERROR(Status)) {
            Handle = ((SMBIOS_STRUCTURE_HEADER*)TempBuffer)->Handle;
            SmbiosProtocol->SmbiosDeleteStructure(Handle); //3rd LAN
            pBS->FreePool(TempBuffer);
        }
        
    if(LAN_NUMBER == 0){//Delete LAN2 for i210 SIOM case
        Status = SmbiosProtocol->SmbiosReadStrucByType(41, 3, &TempBuffer, &TempBufferSize);
        if (!EFI_ERROR(Status)) {
            Handle = ((SMBIOS_STRUCTURE_HEADER*)TempBuffer)->Handle;
            SmbiosProtocol->SmbiosDeleteStructure(Handle); //2nd LAN
            pBS->FreePool(TempBuffer);
        }		    
    }
    }

    TRACE((-1, "Type41SmbiosUpdate end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : X11DPTBDriverInit
//
// Description : Project DXE initial code
//
// Parameters  : None
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
X11DPTBDxeDriverInit(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS	Status;
    EFI_HANDLE	Handle = NULL;
    EFI_EVENT	X11DPTBReadyToBootEvent, HideItemEvent,  SmbiosEvent;
    static EFI_GUID guidReadyToBoot = EFI_EVENT_GROUP_READY_TO_BOOT;
    VOID *HideItemReg;
    VOID *SmbiosReg;

    InitAmiLib(ImageHandle, SystemTable);

	DEBUG((EFI_D_INFO, "X11DPTBProjectDriver Init\n"));

    Status = gBS->InstallProtocolInterface (
                 &Handle,
                 &gProjectOwnProtocolGuid ,
                 EFI_NATIVE_INTERFACE,
                 &ProjectInterface);

    ASSERT_EFI_ERROR(Status);

    gBS->CreateEventEx(
        EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
        X11DPTBReadyToBootCallBack,
        NULL, &guidReadyToBoot,
        &X11DPTBReadyToBootEvent);

    Status = gBS->CreateEvent(
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  X11DPTBHideSetupItem,
                  NULL,
                  &HideItemEvent);

    if(!EFI_ERROR (Status)) {
        Status = gBS->RegisterProtocolNotify(
                  &gSetupModifyProtocolGuid,
                  HideItemEvent,
                  &HideItemReg);
    }

    OVERRIDE_IOBP();

    Status = pBS->CreateEvent(
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  Type41SmbiosUpdate,
                  NULL,
                  &SmbiosEvent);
    if(!Status){
	Status = pBS->RegisterProtocolNotify(
                  &gBdsAllDriversConnectedProtocolGuid,
                  SmbiosEvent,
                  &SmbiosReg);
    }

	DEBUG((EFI_D_INFO, "Exit X11DPTBProjectDriver Init\n"));

    return Status;
}
