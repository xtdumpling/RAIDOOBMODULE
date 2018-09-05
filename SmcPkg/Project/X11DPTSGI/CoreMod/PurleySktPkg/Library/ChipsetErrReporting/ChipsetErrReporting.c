/**
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c) 2009-2016 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file ChipsetErrorReporting

    Contains Library Implementation file that supports Chipset error handling

---------------------------------------------------------------------------**/

#include <Library/ChipsetErrReporting.h>
#include <ChipsetErrReportingPrivate.h>
#include <ChipsetErrReportingExternal.h>
// APTIOV_SERVER_OVERRIDE_RC_START
#include <ProcessIioErrors.h>
// APTIOV_SERVER_OVERRIDE_RC_END

// IOH_DATA structure is used in runtime when IOH UDS protocol is not available.
IOH_INFO  mIohInfo[MAX_SOCKET] = {0};

EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocolLib;
EFI_RAS_SYSTEM_TOPOLOGY               *mRasTopologyLib;
SYSTEM_RAS_SETUP                      *mRasSetupLib;
SYSTEM_RAS_CAPABILITY                 *mRasCapabilityLib;

EFI_CPU_CSR_ACCESS_PROTOCOL           *mCpuCsrAccessLib = NULL;
BOOLEAN   mNmiOnSerr  = FALSE;
BOOLEAN   mNmiOnPerr  = FALSE;


///
/// IIO Local Error Status Registers List.
/// List in the order in which these have to be cleared
/// 
IIO_ERRSTS_REG  mLocalMultiStackRegList[] = {
    //
    // Local Error Status Log
    //
    { VTUNCERRSTS_IIO_VTD_REG      },
    { TCERRST_IIO_RAS_REG          },  
    { IRPP0ERRST_IIO_RAS_REG       },
    { IRPRINGERRST_IIO_RAS_REG     },
    //
    // Following registers cleared last in this list
    //
    { LFERRST_IIO_RAS_REG          },
    { LNERRST_IIO_RAS_REG          },
    { LCERRST_IIO_RAS_REG          }  
};

IIO_ERRSTS_REG  mLocalSingleStackRegList[] = {
    //
    // CBDMA Error Status Registers
    //
    { DMAUNCERRSTS_IIOCB_FUNC0_REG },
    { CHANERR_INT_IIOCB_FUNC0_REG  },
    { CHANERR_INT_IIOCB_FUNC1_REG  },
    { CHANERR_INT_IIOCB_FUNC2_REG  },
    { CHANERR_INT_IIOCB_FUNC3_REG  },
    { CHANERR_INT_IIOCB_FUNC5_REG  },
    { CHANERR_INT_IIOCB_FUNC6_REG  },
    { CHANERR_INT_IIOCB_FUNC7_REG  }
};

///
/// IIO Global Error Status Registers List.
/// List in the order in which these have to be cleared
/// 
IIO_ERRSTS_REG  mIioGlobalErrStsRegList[] = {
    { IIO_GF_ERRST_UBOX_CFG_REG  },
    { IIO_GNF_ERRST_UBOX_CFG_REG },
    { IIO_GC_ERRST_UBOX_CFG_REG  },  
    // (RO) { BOXINST_0, IIO_GSYSST_UBOX_CFG_REG    }
};

IIO_ERRSTS_REG  mIioGlblErrPinRegList[] = {
    { IIO_ERRPINSTS_UBOX_CFG_REG    }
};


/**
  Check For Iio Errors.

  This function check if IioErrors is enabled then check
  if any errors were reported in the GSYSST status register

  @retval  BOOLEAN  return TRUE if an IIO error is detected
**/
BOOLEAN
CheckForIioErrors (
  VOID
  )
{
  UINT8                       RootBridgeLoop;
  IIO_GSYSST_UBOX_CFG_STRUCT  GSysSt;
  UINT8                       SocketId;
  BOOLEAN                     ErrorsExist;

  ErrorsExist = FALSE;
  if (mRasSetupLib->Iio.IioErrorEn) {
    for (RootBridgeLoop = 0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {
      if (mRasTopologyLib->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.Valid) {
        SocketId = mIohInfo[RootBridgeLoop].SocketId;
        GSysSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId,0,IIO_GSYSST_UBOX_CFG_REG);
        if (GSysSt.Bits.severity0 == 1 || GSysSt.Bits.severity1 == 1 || GSysSt.Bits.severity2 == 1) {
          DEBUG ((DEBUG_ERROR, "CheckForIioErrors, GSysSt: %x  \n", GSysSt.Data)); 
          ErrorsExist = TRUE;
        }
      }
     }
  }

  return ErrorsExist;
}

/**
  This function initialize the IOH info structure.

  @retval    None.
**/
VOID
InitIOHInfo (
   VOID
   )
{
  UINT8 RootBridgeLoop;
  UINT8 i;
  UINT8 PciPort;

  for (RootBridgeLoop = 0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {
    if (mRasTopologyLib->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.Valid == 1) {
      mIohInfo[RootBridgeLoop].Valid = 1;
      mIohInfo[RootBridgeLoop].SocketId = mRasTopologyLib->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.SocketId;
      mIohInfo[RootBridgeLoop].BusNum  = mRasTopologyLib->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.IIOBusBase;
      mIohInfo[RootBridgeLoop].Core20BusNum = mRasTopologyLib->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.IIOBusBase;
      mIohInfo[RootBridgeLoop].FpgaStack =  mRasTopologyLib->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.FpgaStack;

      for (PciPort = 0; PciPort < NUMBER_PORTS_PER_SOCKET; PciPort++) {
        mIohInfo[RootBridgeLoop].Port[PciPort].Bus = mRasTopologyLib->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.PciPortInfo[PciPort].Bus;
        mIohInfo[RootBridgeLoop].Port[PciPort].Device = mRasTopologyLib->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.PciPortInfo[PciPort].Device;
        mIohInfo[RootBridgeLoop].Port[PciPort].Function = mRasTopologyLib->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.PciPortInfo[PciPort].Function;
        mIohInfo[RootBridgeLoop].Port[PciPort].StackPerPort = mRasTopologyLib->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.PciPortInfo[PciPort].StackPerPort;
        RASDEBUG((EFI_D_INFO, "RootBridgeLoop % x, Port : %x StackPerPort :%x",RootBridgeLoop, PciPort, mIohInfo[RootBridgeLoop].Port[PciPort].StackPerPort));
        RASDEBUG((EFI_D_INFO, "  Bus % x, Device : %x Func :%x\n",mIohInfo[RootBridgeLoop].Port[PciPort].Bus, mIohInfo[RootBridgeLoop].Port[PciPort].Device, mIohInfo[RootBridgeLoop].Port[PciPort].Function));
      }

      for (i = 0; i < MAX_IIO_STACK; i++) {
        mIohInfo[RootBridgeLoop].IioStackInfo[i].StackValid = mRasTopologyLib->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.IioStackInfo[i].Valid;
        mIohInfo[RootBridgeLoop].IioStackInfo[i].StackBus = mRasTopologyLib->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.IioStackInfo[i].BusBase;
      }
    } else {
      mIohInfo[RootBridgeLoop].Valid = 0;
    }
  }
}


//SGI+
/**
  Mask IIO Errors.

  This function masks IIO Correctable errors reporting.

    @param None

  @retval Status.
**/
EFI_STATUS
MaskIIOCoRErrorReporting(
  VOID
)
{
  UINT8   RootBridgeLoop;
  UINT8                     IioStack;
  LCERRMASK_IIO_RAS_STRUCT  LcerrMask;


  LcerrMask.Data = LCERR_MASK_DATA;

  for ( RootBridgeLoop =0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {
    if(mIohInfo[RootBridgeLoop].Valid != 1) continue;
    for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
      if (mIohInfo[RootBridgeLoop].IioStackInfo[IioStack].StackValid != 1) continue;
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridgeLoop].SocketId, IioStack, LCERRMASK_IIO_RAS_REG, LcerrMask.Data);        
    }
  }
    
  return EFI_SUCCESS;
}
//SGI-

/**
  Mask IIO Errors.

  This function masks IIO errors reporting.

    @param None

  @retval Status.
**/
EFI_STATUS
MaskIIOErrorReporting(
  VOID
)
{
  UINT8   RootBridgeLoop;
  UINT8                     IioStack;
  LNERRMASK_IIO_RAS_STRUCT  LnerrMask;
  LFERRMASK_IIO_RAS_STRUCT  LferrMask;
  LCERRMASK_IIO_RAS_STRUCT  LcerrMask;
  LERRCTL_IIO_RAS_STRUCT    LerrCtlMask; 

  LnerrMask.Data = LNERR_MASK_DATA;
  LferrMask.Data = LFERR_MASK_DATA;
  LcerrMask.Data = LCERR_MASK_DATA;
  LerrCtlMask.Data = LERRCTL_MASK_DATA;

  for ( RootBridgeLoop =0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {
    if(mIohInfo[RootBridgeLoop].Valid != 1) continue;
    for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
      if (mIohInfo[RootBridgeLoop].IioStackInfo[IioStack].StackValid != 1) continue;

      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridgeLoop].SocketId, IioStack, LNERRMASK_IIO_RAS_REG, LnerrMask.Data);
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridgeLoop].SocketId, IioStack, LFERRMASK_IIO_RAS_REG, LferrMask.Data);
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridgeLoop].SocketId, IioStack, LCERRMASK_IIO_RAS_REG, LcerrMask.Data);        
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridgeLoop].SocketId, IioStack, LERRCTL_IIO_RAS_REG, LerrCtlMask.Data); 
    }
  }
    
  return EFI_SUCCESS;
}


/**
  Clear IIO Error Status Registers which require write "1" to clear (RW1C).

  @param[in]  Arg1  Socket ID
  @param[in]  Arg2  Error Register Type Local, Global, etc.

  @retval  EFI_SUCCESS 
**/
EFI_STATUS 
ClearErrStsRegisters (
  IN UINT8    SktId,
  IN UINT8    BoxInst,
  IN UINT8    IIO_ERRSTS_REG_TYPE
  )
{
  UINTN   RegListSize;
  UINTN   i;
  UINT32  Data;

  IIO_ERRSTS_REG *RegPtr;

  switch (IIO_ERRSTS_REG_TYPE) {

    case LocalSingleStackRegs:
      RegPtr = mLocalSingleStackRegList;
      RegListSize = sizeof(mLocalSingleStackRegList) / sizeof(IIO_ERRSTS_REG);
      break;

    case LocalMultiStackRegs:
      RegPtr = mLocalMultiStackRegList;
      RegListSize = sizeof(mLocalMultiStackRegList) / sizeof(IIO_ERRSTS_REG);
      break;

    case IioErrStsGlobalRegs:
      RegPtr = mIioGlobalErrStsRegList;
      RegListSize = sizeof(mIioGlobalErrStsRegList) / sizeof(IIO_ERRSTS_REG);
      break;

    case IioErrStsGlblErrPinRegs:
      if(mRasSetupLib->Iio.IioErrorPinEn)  {
        RegPtr = mIioGlblErrPinRegList;
        RegListSize = sizeof(mIioGlblErrPinRegList) / sizeof(IIO_ERRSTS_REG);
    } else {
        return  EFI_UNSUPPORTED;
      }
      break;

    default:
      return  EFI_UNSUPPORTED;
  };

  for (i = 0 ; i < RegListSize ; i++ ) {
     Data = 0;
     Data = mCpuCsrAccessLib->ReadCpuCsr(SktId, BoxInst, RegPtr->RegDef);
     // Debug Hook location: Add some debug hook to print register list 
     if (Data != 0) {
        mCpuCsrAccessLib->WriteCpuCsr(SktId, BoxInst, RegPtr->RegDef, Data);
  }
     RegPtr++;
  }
  
  return EFI_SUCCESS;
}


/**
  Displays local and global IIO Errors.

  This function displays local and global IIO status registers.

  @param[in] SktId   The socket ID.

  @retval  None.

**/
VOID
DisplayIioPciExErrors (
  IN      UINT8   SktId
)
{
  UINT8   RootBridgeLoop;
  UINT8   Port, Bus, Device, Function;
  UINT32  Data32;

  RASDEBUG((DEBUG_INFO, "DisplayIioPciExErrors\n"));

  for (RootBridgeLoop = 0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {
    if ((mIohInfo[RootBridgeLoop].Valid == 1) && (SktId == mIohInfo[RootBridgeLoop].SocketId)) {

      for(Port = 0; Port < NUMBER_PORTS_PER_SOCKET; Port++) {
        Bus = mIohInfo[RootBridgeLoop].Port[Port].Bus;
        Device = mIohInfo[RootBridgeLoop].Port[Port].Device;
        Function = mIohInfo[RootBridgeLoop].Port[Port].Function;

        // Check if it is a root port.
        if (PCIeIsRootPort (RootBridgeLoop, Bus, Device, Function)) {
         Data32 = MmioRead32 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPUNCERRSTS_IIO_PCIE_REG)));

          if (Data32 != 0) {
            RASDEBUG((DEBUG_INFO, "XPUNCERRSTS_IIO_PCIE_REG = 0x%x\n", Data32));
          }
          Data32 = MmioRead32 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRSTS_IIO_PCIE_REG)));

          if (Data32 != 0) {
            RASDEBUG((DEBUG_INFO, "XPCORERRSTS_IIO_PCIE_REG = 0x%x\n", Data32));
          }
          Data32 = MmioRead32 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRCOUNTER_IIO_PCIE_REG)));

          if (Data32 != 0) {
            RASDEBUG((DEBUG_INFO, "XPCORERRCOUNTER_IIO_PCIE_REG = 0x%x\n", Data32));
          }
          Data32 = MmioRead32 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (UNCERRSTS_IIO_PCIE_REG)));

          if (Data32 != 0) {
            RASDEBUG((DEBUG_INFO, "UNCERRSTS_IIO_PCIE_REG = 0x%x\n", Data32));
          }
          Data32 = MmioRead32 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (CORERRSTS_IIO_PCIE_REG)));

          if (Data32 != 0) {
            RASDEBUG((DEBUG_INFO, "CORERRSTS_IIO_PCIE_REG = 0x%x\n", Data32));
          }
        }
      }

      break;
    }
  }
}


/**
  Clears local and global IIO Errors.

  This function clears local and global IIO status registers.

  @param[in] SktId   The socket ID.
  
  @retval  None.

**/
VOID
ClearIioErrorRegisterStatus (
  IN      UINT8   SktId
)
{
  UINT8   IioStack;
  UINT8   RootBridgeLoop;
  UINT8   Port, Bus, Device, Function; 
  UINT16  Data;
  UINT32  Data32;
  SECSTS_IIO_PCIE_STRUCT SecStsIioPcie;
  XPUNCERRSTS_IIO_PCIE_STRUCT XpUncErrSts;
  XPCORERRSTS_IIO_PCIE_STRUCT XpCorErrSts;
  LNKSTS_IIO_PCIE_STRUCT LnkSts;
  
  if (mRasSetupLib->Iio.IioErrRegistersClearEn == 0) {
    RASDEBUG((DEBUG_INFO, "Clearing IIO Error registers is disabled, so IIO Error registers are not cleared.\n"));
    return;
  }

  RASDEBUG((DEBUG_INFO, "ClearIioErrorRegisterStatus\n"));
  ClearErrStsRegisters (SktId, 0, LocalSingleStackRegs );  // Local single stack

  for (RootBridgeLoop = 0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {
    if ((mIohInfo[RootBridgeLoop].Valid == 1) && (SktId == mIohInfo[RootBridgeLoop].SocketId)) {
          
      for(Port = 0; Port < NUMBER_PORTS_PER_SOCKET; Port++) {
        Bus = mIohInfo[RootBridgeLoop].Port[Port].Bus;
        Device = mIohInfo[RootBridgeLoop].Port[Port].Device;
        Function = mIohInfo[RootBridgeLoop].Port[Port].Function;

        // Check if it is a root port.
        if (PCIeIsRootPort (RootBridgeLoop, Bus, Device, Function)) {
          SecStsIioPcie.Data = MmioRead16 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (SECSTS_IIO_PCIE_REG)));

          if (SecStsIioPcie.Bits.rma != 0) {
            SecStsIioPcie.Bits.rma = 1;
            MmioWrite16 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (SECSTS_IIO_PCIE_REG)), SecStsIioPcie.Data);

          }
         
          Data = MmioRead16 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (PCISTS_IIO_PCIE_REG)));

          if (Data != 0) {
            MmioWrite16 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (PCISTS_IIO_PCIE_REG)), Data);
          }

          XpUncErrSts.Data = MmioRead32 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPUNCERRSTS_IIO_PCIE_REG)));

          if (XpUncErrSts.Bits.received_pcie_completion_with_ur_status != 0) {
            XpUncErrSts.Bits.received_pcie_completion_with_ur_status = 1;
            MmioWrite32 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPUNCERRSTS_IIO_PCIE_REG)), XpUncErrSts.Data);
          }
          XpCorErrSts.Data = MmioRead32 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRSTS_IIO_PCIE_REG)));

          if (XpCorErrSts.Bits.pci_link_bandwidth_changed_status != 0) {
        	LnkSts.Data = MmioRead16 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (LNKSTS_IIO_PCIE_REG)));
        	LnkSts.Bits.link_bandwidth_management_status = 1;
        	MmioWrite16 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (LNKSTS_IIO_PCIE_REG)), LnkSts.Data);
        	XpCorErrSts.Bits.pci_link_bandwidth_changed_status = 1;
        	MmioWrite32 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRSTS_IIO_PCIE_REG)), XpCorErrSts.Data);
          }
          Data32 = MmioRead32 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (RPERRSTS_IIO_PCIE_REG)));
          if (Data32 != 0) {
        	MmioWrite32 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (RPERRSTS_IIO_PCIE_REG)), Data32);
          }
          Data = MmioRead16 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPGLBERRSTS_IIO_PCIE_REG)));

          if (Data != 0) {
        	MmioWrite16 (MmPciAddress (SktId, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPGLBERRSTS_IIO_PCIE_REG)), Data);
          }
        }
      }

      for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
        if (mIohInfo[RootBridgeLoop].IioStackInfo[IioStack].StackValid) {
          ClearErrStsRegisters (SktId, IioStack, LocalMultiStackRegs);    
        }
      }
      break;
    }
  }

  ClearErrStsRegisters (SktId, 0, IioErrStsGlblErrPinRegs);
  ClearErrStsRegisters (SktId, 0, IioErrStsGlobalRegs);   // Globals are cleared last
}


/**
  Clears IIO Errors.

  This function clears IIO status registers.

  @retval  None.

**/
VOID
ClearIioErrors (
  VOID
)
{
  UINT8   RootBridgeLoop;

  for (RootBridgeLoop = 0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {
    if (mIohInfo[RootBridgeLoop].Valid) {
      ClearIioErrorRegisterStatus (mIohInfo[RootBridgeLoop].SocketId);
    }
  }
}


/**
  This function will enable all the RP PIO errors.

  @param[in] Ioh        The IOH number.

  @retval    Status.
**/
EFI_STATUS
RppIoErrEnable(
  IN       UINT8                  Ioh
  )
{
  UINT8                           Port;
  UINT8                           Bus;
  UINT8                           Device;
  UINT8                           Function;
  RPPIOERR_HF_IIO_PCIE_STRUCT     RppioErrHf;
  RPPIOERR_MASK_IIO_PCIE_STRUCT   RppioErrMask;
  RPPIOERR_STATUS_IIO_PCIE_STRUCT RppioErrSts;
  UINT16                          CapabilitiesOffset;

#ifdef RASCM_TODO
  //
  // Need to reflect the multiple IIO for Purley platfrom 
  //
#endif 
  for (Port = 0; Port < NUMBER_PORTS_PER_SOCKET; Port++) {
    Bus = mIohInfo[Ioh].Port[Port].Bus;
    Device = mIohInfo[Ioh].Port[Port].Device;
    Function = mIohInfo[Ioh].Port[Port].Function;
   
    // Verify any PciE Device exist at the Bus, Device and Function
    if (!PciErrLibIsDevicePresent (Ioh, Bus, Device, Function)) {
      continue;
    }

    // Check if it is a root Port.
    if (!PCIeIsRootPort (Ioh, Bus, Device, Function)) {
      continue;
    }

    // Get the capability offset for RPPIOERR
    CapabilitiesOffset = PciErrLibGetExtendedCapabilitiesOffset (Ioh, Bus, Device, Function, PCIE_EXT_CAP_HEADER_RPPIOERR_CAPID, PCIE_EXT_CAP_HEADER_RPPIOERR_VSECID);
    if (CapabilitiesOffset == 0) {
      continue;
    }
    //Clear RppioErrStatus before modifying rppioerrmask
    RppioErrSts.Data = MmioRead32 (MmPciAddress(Ioh, Bus, Device, Function, (CapabilitiesOffset + R_RPPIOERRSTS)));
    MmioWrite32 (MmPciAddress(Ioh, Bus, Device, Function, (CapabilitiesOffset + R_RPPIOERRSTS)), RppioErrSts.Data);
    
    RppioErrHf.Data = MmioRead32 (MmPciAddress(Ioh, Bus, Device, Function, (CapabilitiesOffset + R_RPPIOERRHF)));

    // Clear corresponding uncorrectable errors in the HF register
    RppioErrHf.Bits.cfg_ur_hf = 0;
    RppioErrHf.Bits.cfg_to_hf = 0;
    RppioErrHf.Bits.io_ur_hf = 0;
    RppioErrHf.Bits.io_to_hf = 0;
    RppioErrHf.Bits.mem_ur_hf = 0;
    RppioErrHf.Bits.mem_to_hf = 0;

    // Set the corresponding Mask bits not to trigger LER
    RppioErrMask.Bits.cfg_ur_mask = 1;
    RppioErrMask.Bits.cfg_to_mask = 1;
    RppioErrMask.Bits.io_ur_mask = 1;
    RppioErrMask.Bits.io_to_mask = 1; 
    RppioErrMask.Bits.mem_ur_mask = 1;
    RppioErrMask.Bits.mem_to_mask = 1;    

    //Do not enable LER for Soft Fail errors
    RppioErrMask.Bits.cfg_ca_mask = 1;
    RppioErrMask.Bits.io_ca_mask = 1;
    RppioErrMask.Bits.mem_ca_mask = 1;

    MmioWrite32 (MmPciAddress (Ioh, Bus, Device, Function, (CapabilitiesOffset + R_RPPIOERRHF)), RppioErrHf.Data);
    MmioWrite32 (MmPciAddress (Ioh, Bus, Device, Function, (CapabilitiesOffset + R_RPPIOERRMSK)), RppioErrMask.Data);
  }

  return EFI_SUCCESS;
}


/**
  This function enable Disable IoMca.

  @param[in] Buffer        The pointer to private data buffer.

  @retval    Status.
**/ 
EFI_STATUS
IIODisableIoMca (
  IN      VOID    *Buffer
  )
{
  UINTN     CpuNumber;
  UINT64    Data;
  UINT32    MCiCtlAddr;

  CpuNumber = GetAPCpuNumber();
  MCiCtlAddr = MSR_MC06_CTL;
  Data = AsmReadMsr64 (MCiCtlAddr); //MC12_CTL (IIO)
  Data &= ~((UINT64)(BIT0 | BIT1)); // Clearing bit 0 & 1 Disable IIO Fatal MCA and Non-Fatal MCA respectively
  AsmWriteMsr64 (MCiCtlAddr, Data);
  
  MpSyncReleaseAp(CpuNumber);
  return EFI_SUCCESS;
}

/**
  This function enable Enable IoMca.

  @param[in] Buffer        The pointer to private data buffer.

  @retval    Status.
**/
EFI_STATUS
IIOEnableIoMca (
  IN      VOID  *Buffer
)
{
  UINT64        Data;
  UINT32        MCiCtlAddr;
  UINTN         CpuNumber;

  CpuNumber = GetAPCpuNumber();
  MCiCtlAddr = MSR_MC06_CTL;
  Data = AsmReadMsr64 (MCiCtlAddr); //MC_CTL (IIO)
  Data |= (UINT64)(BIT0 | BIT1); // Setting bit 0 & 1 Enable IIO Fatal MCA and Non-Fatal MCA respectively
  AsmWriteMsr64 (MCiCtlAddr, Data);  
  
  MpSyncReleaseAp(CpuNumber);
  return EFI_SUCCESS;
}

/**
  This function will enable IIO PCIE error reporting.

  @param[in] RootBridgeLoop        The socket number.

  @retval    Status.
**/
EFI_STATUS
IohPcieErrorEnable (
  IN      UINT8   RootBridgeLoop
  )
{
  UINT8                            Segment;
  UINT8                            Port;
  UINT8                            Bus;
  UINT8                            Device;
  UINT8                            Function;
  UINT8                            StackPerPort;
  UINT8                            SecondaryBusNumber;
  MISCCTRLSTS_0_IIO_PCIE_STRUCT    MiscCtrlSts0;
  MISCCTRLSTS_1_IIO_PCIE_STRUCT    MiscCtrlSts1;
  UNCERRMSK_IIO_PCIE_STRUCT        UncErrMsk;
  UNCEDMASK_IIO_PCIE_STRUCT        UncEdMask;
  XPCORERRMSK_IIO_PCIE_STRUCT      XpCorErrMsk;
  XPCOREDMASK_IIO_PCIE_STRUCT      XpCorEdMask;
  XPUNCEDMASK_IIO_PCIE_STRUCT      XpUncEdMask;
  XPUNCERRMSK_IIO_PCIE_STRUCT      XpUncErrMsk;
  XPUNCERRSEV_IIO_PCIE_STRUCT      XpUncErrSev;
  PCIERRSV_IIO_RAS_STRUCT          PciErrSv;
  CORERRMSK_IIO_PCIE_STRUCT        CorErrMsk;
  COREDMASK_IIO_PCIE_STRUCT        CorEdMask;

  //SKX_TODO: This register is not yet implemented
  //Si CCB: https://vthsd.sc.intel.com/hsd/skylake_server/default.aspx#issue/default.aspx?issue_id=1451345
  XPCORERRCOUNTER_IIO_PCIE_STRUCT   XpCorErrCntr;
  XPCORERRTHRESHOLD_IIO_PCIE_STRUCT XpCorErrThres;

  UINT16      AerOffset;
  //TODO this valid for Skylake?
  //GERRCTL_IIO_RAS_STRUCT           GErrCtl;

  RASDEBUG((DEBUG_ERROR, "IohPcieErrorEnable: Inside the function: 0x%x\n", IohPcieErrorEnable));

  if (RootBridgeLoop >= MAX_SOCKET || mIohInfo[RootBridgeLoop].Valid != 1) {
    return EFI_SUCCESS;
  }

  //GErrCtl.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridgeLoop].SocketId, 0, GERRCTL_IIO_RAS_REG);
  Segment = 0;

  for(Port = 0; Port < NUMBER_PORTS_PER_SOCKET; Port++) {
    Bus = mIohInfo[RootBridgeLoop].Port[Port].Bus;
    Device = mIohInfo[RootBridgeLoop].Port[Port].Device;
    Function = mIohInfo[RootBridgeLoop].Port[Port].Function;

    StackPerPort = mIohInfo[RootBridgeLoop].Port[Port].StackPerPort;
    if (mIohInfo[RootBridgeLoop].IioStackInfo[StackPerPort].StackValid != 1)
      continue;
    
    // Check if the root port is enabled.
    if (!PciErrLibIsDevicePresent (RootBridgeLoop, Bus, Device, Function)) {
      //GErrCtl.Data |= (1 << (Port + 5));
      continue;
    }

    RASDEBUG ((DEBUG_INFO, "Bus:0x%x, Device:0x%x, Function:0x%x, StackPerPort:0x%x\n", Bus, Device, Function, StackPerPort));

    // Check if it is a root port.
    if (!PCIeIsRootPort (RootBridgeLoop, Bus, Device, Function)) {
      PciErrLibEnableElogDevice (RootBridgeLoop, Bus, Device, Function);
    } else {
      // Disable MSI generation on RAS errors from PCI-E Devices
      MiscCtrlSts0.Data = MmioRead32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (MISCCTRLSTS_0_IIO_PCIE_REG)));
      MiscCtrlSts0.Bits.enable_system_error_only_for_aer = 1;
      MmioWrite32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (MISCCTRLSTS_0_IIO_PCIE_REG)), MiscCtrlSts0.Data);

      // Override AER programming for IIO root ports
      UncErrMsk.Data = MmioRead32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (UNCERRMSK_IIO_PCIE_REG)));
      UncErrMsk.Data |= IIO_AER_UEM_OVERRIDE;
      MmioWrite32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (UNCERRMSK_IIO_PCIE_REG)), UncErrMsk.Data);

      CorErrMsk.Data = MmioRead32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (CORERRMSK_IIO_PCIE_REG)));
      CorErrMsk.Data |= IIO_AER_CEM_OVERRIDE;
      MmioWrite32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (CORERRMSK_IIO_PCIE_REG)), CorErrMsk.Data);

    }

    //Rootport specific AER setting goes here
    AerOffset = PciErrLibGetExtendedCapabilitiesOffset(RootBridgeLoop, Bus, Device, Function,PCI_EXPRESS_EXTENDED_CAPABILITY_ADVANCED_ERROR_REPORTING_ID,0xFF);

    if(AerOffset != 0) {
      CorEdMask.Data = IIO_PCIEAER_CORRERR_MSK;
      if (mRasSetupLib->Pcie.PcieAerCorrErrEn == 1) {  
        CorEdMask.Data &= ~mRasSetupLib->Pcie.PcieAerCorrErrBitMap;
      }
      MmioWrite32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (COREDMASK_IIO_PCIE_REG)), CorEdMask.Data);

      UncEdMask.Data = IIO_PCIEAER_UNCORRERR_MSK;
      if (mRasSetupLib->Pcie.PcieAerAdNfatErrEn == 1) {
        // Only (BIT12, BIT15, BIT16, BIT20) needs to be changed, the rest are masked 
        UncEdMask.Data &= mRasSetupLib->Pcie.PcieAerAdNfatErrBitMap;     
      }
      if (mRasSetupLib->Pcie.PcieAerNfatErrEn == 1) {
        // Only (BIT14, BIT19, BIT21) needs to be changed, the rest are masked
        UncEdMask.Data &= mRasSetupLib->Pcie.PcieAerNfatErrBitMap;  
        }
      if (mRasSetupLib->Pcie.PcieAerFatErrEn == 1) {
        // Only (BIT4, BIT5, BIT13, BIT17, BIT18) needs to be changed, the rest are masked 
        UncEdMask.Data &= mRasSetupLib->Pcie.PcieAerFatErrBitMap;  
      }
      MmioWrite32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (UNCEDMASK_IIO_PCIE_REG)), UncEdMask.Data);

    }
    
    MiscCtrlSts1.Data = MmioRead32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (MISCCTRLSTS_1_IIO_PCIE_REG)));

    if (mRasSetupLib->Pcie.PcieCorrErrEn == 1) {
      MiscCtrlSts1.Bits.override_system_error_on_pcie_correctable_error_enable = 1;
      }
    if (mRasSetupLib->Pcie.PcieUncorrErrEn == 1) {
      MiscCtrlSts1.Bits.override_system_error_on_pcie_non_fatal_error_enable = 1;
      }
    if (mRasSetupLib->Pcie.PcieFatalErrEn == 1) {
      MiscCtrlSts1.Bits.override_system_error_on_pcie_fatal_error_enable = 1;
    }
    MiscCtrlSts1.Bits.problematic_port_for_lock_flows = 0;
    MmioWrite32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (MISCCTRLSTS_1_IIO_PCIE_REG)), MiscCtrlSts1.Data);


    // XP Correctable Error Mask Register

    XpCorErrMsk.Data = MmioRead32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRMSK_IIO_PCIE_REG)));
    XpCorEdMask.Data = MmioRead32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCOREDMASK_IIO_PCIE_REG)));

    // Adaptec RAID (model) encounters SMI storm when pci link bandwidth error is unmasked.
    // Mask error to prevent system hang.
    if (mRasSetupLib->Iio.IioPcieAddCorrErrorEn == 1) {
      XpCorErrMsk.Bits.pci_link_bandwidth_changed_mask = 1;
      XpCorEdMask.Bits.pci_link_bandwidth_changed_detect_mask = 0;
    } else {
      XpCorErrMsk.Bits.pci_link_bandwidth_changed_mask = 1;
      XpCorEdMask.Bits.pci_link_bandwidth_changed_detect_mask = 1;
    }

    MmioWrite32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRMSK_IIO_PCIE_REG)), XpCorErrMsk.Data);
    MmioWrite32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCOREDMASK_IIO_PCIE_REG)), XpCorEdMask.Data);

    // XP Un-Correctable Error Mask Register - Mask bit6 (received_pcie_completion_with_ur_detect_mask)
    XpUncEdMask.Data = IIO_PCIEADD_UNCORR_MSK;
    XpUncErrMsk.Data = IIO_PCIEADD_UNCORR_MSK; 
    XpUncErrSev.Data = 0;
    if (mRasSetupLib->Iio.IioPcieAddUnCorrEn == 1) {  
      // Only (BIT1, BIT3, BIT4, BIT5, BIT6, BIT8, BIT9) needs to be changed, the rest are masked       
      XpUncEdMask.Data &= ~mRasSetupLib->Iio.IioPcieAddUnCorrBitMap;   
      XpUncErrMsk.Data &= ~mRasSetupLib->Iio.IioPcieAddUnCorrBitMap;
      XpUncErrSev.Data = mRasSetupLib->Iio.IioPcieAddUnCorrSevBitMap & IIO_PCIEADD_UNCORR_MSK;
    }

    MmioWrite32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPUNCEDMASK_IIO_PCIE_REG)), XpUncEdMask.Data);
    MmioWrite32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPUNCERRMSK_IIO_PCIE_REG)), XpUncErrMsk.Data);
    MmioWrite32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPUNCERRSEV_IIO_PCIE_REG)), XpUncErrSev.Data);


    // Enable PCIe corrected Error counter threshold and set the mask if counter is enabled

    if (mRasSetupLib->Pcie.PcieCorErrCntr == 1) {
      XpCorErrThres.Data = MmioRead16 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRTHRESHOLD_IIO_PCIE_REG)));
      XpCorErrCntr.Data  = MmioRead32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRCOUNTER_IIO_PCIE_REG)));

      XpCorErrCntr.Data |= mRasSetupLib->Pcie.PcieCorErrMaskBitMap;

      XpCorErrThres.Bits.error_threshold_enable = 1;
      XpCorErrThres.Bits.error_threshold = mRasSetupLib->Pcie.PcieCorErrThres;

      MmioWrite16 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRTHRESHOLD_IIO_PCIE_REG)), XpCorErrThres.Data);
      MmioWrite32 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRCOUNTER_IIO_PCIE_REG)), XpCorErrCntr.Data);

      RASDEBUG((DEBUG_INFO, "XpCorErrThres.Data: 0x%x\n", XpCorErrThres.Data));
      RASDEBUG((DEBUG_INFO, "XpCorErrCntr.Data: 0x%x\n", XpCorErrCntr.Data));
    }

    // Clear Extended error status registers
    ClearIohExtPcieDeviceStatus (RootBridgeLoop, Bus, Device, Function);

    // Clear the status registers
    PciErrLibClearPcieDeviceStatus (RootBridgeLoop, Bus, Device, Function);
    
    // Do not enable error logging if there are no Devices behind the bridge.
    SecondaryBusNumber = MmioRead8 (MmPciAddress (RootBridgeLoop, Bus, Device, Function, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));

    if (!(SecondaryBusNumber == 0) && (PciErrLibDeviceOnBus (SecondaryBusNumber))) {
      RASDEBUG((DEBUG_INFO, "Secondary device on B%x:D%x:F%x\n", Bus, Device, Function));      
      // Enable error reporting in the root port and all subordinate Devices
      PciErrLibEnableElogDeviceAndSubDevices (RootBridgeLoop, Bus, Device, Function);
    }
  }

  //mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridgeLoop].SocketId, 0, GERRCTL_IIO_RAS_REG, GErrCtl.Data);

  // Error Severity register: Correctable - severity 0, Nonfatal - Severity 1, Fatal - severity 2
  PciErrSv.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridgeLoop].SocketId, 0, PCIERRSV_IIO_RAS_REG);
  PciErrSv.Bits.pciecorerr_map = 0;
  PciErrSv.Bits.pcienonfaterr_map = 1;
  PciErrSv.Bits.pciefaterr_map = 2;
  mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridgeLoop].SocketId, 0, PCIERRSV_IIO_RAS_REG, PciErrSv.Data);

  return EFI_SUCCESS;
}


/**
  This function will enable IIO error reporting.

  @param[in] RootBridge        The socket number.

  @retval    Status.
**/
EFI_STATUS
IIOErrorEnable(
  IN      UINT8   RootBridge
  )
{
  EFI_STATUS                Status;
  UINT8                     IioStack;
  SYSMAP_IIO_RAS_STRUCT               LSysMap;
  IIO_GSYSMAP_UBOX_CFG_STRUCT         GSysMap;
  IIO_ERRPINCTL_UBOX_CFG_STRUCT       GErrPinCtl;
  LSYSCTL_IIO_RAS_STRUCT              LSysCtl;
  LBITMAP_IIO_RAS_STRUCT              LBitMap; // LBITMAP_IIO_RAS_REG
  IRPPERRSV_N0_IIO_RAS_STRUCT         IrpErrSvN0; //IRPPERRSV_N0_IIO_RAS_REG
  ITCERRSEV_IIO_RAS_STRUCT            ItcErrSev; // ITCERRSEV_IIO_RAS_REG
  OTCERRSEV_IIO_RAS_STRUCT            OtcErrSev; // OTCERRSEV_IIO_RAS_REG
  VTUNCERRSEV_IIO_VTD_STRUCT          VtUncErrSev; //VTUNCERRSEV_IIO_VTD_REG
  MIERRSV_IIO_RAS_STRUCT              MiErrSv; // MIERRSV_IIO_RAS_REG
  IRPRINGERRSV_N0_IIO_RAS_STRUCT      IrpRingErrSvN0;  //IRPRINGERRSV_N0_IIO_RAS_REG
  CHANERRSEV_INT_IIOCB_FUNC0_STRUCT   ChanErrSev; // CHANERRSEV_INT_IIOCB_FUNC0_REG
  IRPP0ERRCNT_IIO_RAS_STRUCT          Irpp0ErrCnt; // IRPP0ERRCNT_IIO_RAS_REG 
  IRPP0ERRCNTSEL_IIO_RAS_STRUCT       Irpp0ErrCntSel; // IRPP0ERRCNTSEL_IIO_RAS_REG
  MIERRCNT_IIO_RAS_STRUCT             MiErrCnt; // MIERRCNT_IIO_RAS_REG
  MIERRCNTSEL_IIO_RAS_STRUCT          MiErrCntSel; // MIERRCNTSEL_IIO_RAS_REG 
  IRPRINGERRCNT_IIO_RAS_STRUCT        IrpRingErrCnt; //IRPRINGERRCNT_IIO_RAS_REG
  IRPRINGERRCNTSEL_IIO_RAS_STRUCT     IrpRingErrCntSel; //IRPRINGERRCNTSEL_IIO_RAS_REG
  TCERRCNT_IIO_RAS_STRUCT             TcErrCnt; //TCERRCNT_IIO_RAS_REG
  TCERRCNTSEL_IIO_RAS_STRUCT          TcErrCntSel; //TCERRCNTSEL_IIO_RAS_REG
  TCERRCTL_IIO_RAS_STRUCT             TcErrCtl;
  IRPP0ERRCTL_IIO_RAS_STRUCT          Irpp0ErrCtl; // IRPP0ERRCTL_IIO_RAS_REG
  IIOERRCTL_IIO_RAS_STRUCT            IioErrCtl; // IIOERRCTL_IIO_RAS_REG
  IIOMISCCTRL_N1_IIO_VTD_STRUCT       IioMiscCtrl;
  CIPINTRC_IIO_VTD_STRUCT             CipIntrCtrl;
  VTUNCERRMSK_IIO_VTD_STRUCT          VtUncErrMsk; // VTUNCERRMSK_IIO_VTD_REG
  MIERRCTL_IIO_RAS_STRUCT             MiErrCtl; // MIERRCTL_IIO_RAS_REG
  CHANERRMSK_INT_IIOCB_FUNC0_STRUCT   ChanErrMsk; // CHANERRMSK_INT_IIOCB_FUNC0_REG
  IIO_GSYSCTL_UBOX_CFG_STRUCT         GSysCtl; //IIO_GSYSCTL_UBOX_CFG_REG
  EFI_AP_PROCEDURE                    IoMcaFunc;
  BOOLEAN                             EnableIoMca = FALSE;

  if(RootBridge >= MAX_SOCKET) {
    return EFI_INVALID_PARAMETER;
  }

  IoMcaFunc = NULL;
  Status = EFI_SUCCESS;
  // TODO What is OsSigOnSerrEn supposed to be used for?
  // TODO need to update to use capabilities
  RASDEBUG ((DEBUG_INFO,"IIOErrorEnable: Inside the function: 0x%x\n", IIOErrorEnable));

  if (mRasSetupLib->Pcie.SerrPropEn == 1) {
    mNmiOnSerr = TRUE;
  }
  if (mRasSetupLib->Pcie.PerrPropEn == 1) {
    mNmiOnPerr = TRUE;
  }
  if (mRasSetupLib->Iio.IioErrorEn != 1) {
    MaskIIOErrorReporting ();
  }
  //SGI+
  if ( mRasSetupLib->Iio.MaskPcieCorrError && mRasSetupLib->Iio.IioErrorEn  ){
   MaskIIOCoRErrorReporting();
  }
  
  DisplayIioPciExErrors(mIohInfo[RootBridge].SocketId);

  ClearIioErrorRegisterStatus (mIohInfo[RootBridge].SocketId);

  if (mRasSetupLib->Pcie.PcieErrEn) {
    Status  = IohPcieErrorEnable (RootBridge);

    if (mRasTopologyLib->SystemInfo.SystemRasType == ADVANCED_RAS) {
      if (mRasSetupLib->Iio.LerEn) {
        Status = IIOLerEnable(RootBridge);
        for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
          if (mIohInfo[RootBridge].IioStackInfo[IioStack].StackValid) {
            IioErrCtl.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, IIOERRCTL_IIO_RAS_REG);
            if (mRasSetupLib->Iio.DisableLerMAerrorLogging == 0) {
              IioErrCtl.Bits.outbound_ler_ma_mca_disable = 1;
              IioErrCtl.Bits.inbound_ler_ma_mca_disable = 1;
            } else {
              IioErrCtl.Bits.outbound_ler_ma_mca_disable = 0;
              IioErrCtl.Bits.inbound_ler_ma_mca_disable = 0;
            }
            mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, IIOERRCTL_IIO_RAS_REG, IioErrCtl.Data);
          }
        }    
      }
    }

    RppIoErrEnable (RootBridge);  // TODO is this needed for Skylake?
  } else {
    IohPcieMaskErrors ();
  }
  
  GSysCtl.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridge].SocketId, 0,  IIO_GSYSCTL_UBOX_CFG_REG);
  GErrPinCtl.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridge].SocketId, 0,  IIO_ERRPINCTL_UBOX_CFG_REG);
  GSysMap.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridge].SocketId, 0, IIO_GSYSMAP_UBOX_CFG_REG);

  if (mRasSetupLib->Iio.IioErrorEn == 0) {
    GSysCtl.Bits.severity0_en = 0;
    GSysCtl.Bits.severity1_en = 0;
    GSysCtl.Bits.severity2_en = 0;
    GErrPinCtl.Bits.pin0 = V_ERRPINCTL_DIS;
    GErrPinCtl.Bits.pin1 = V_ERRPINCTL_DIS;
    GErrPinCtl.Bits.pin2 = V_ERRPINCTL_DIS;
    GSysMap.Bits.severity0_map = V_SYSMAP_NoInband;
    GSysMap.Bits.severity1_map = V_SYSMAP_NoInband;
    GSysMap.Bits.severity2_map = V_SYSMAP_NoInband;
    IoMcaFunc = IIODisableIoMca;

    for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
      if (mIohInfo[RootBridge].IioStackInfo[IioStack].StackValid) {
        LSysCtl.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridge].SocketId, IioStack,  LSYSCTL_IIO_RAS_REG);
        LSysMap.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, SYSMAP_IIO_RAS_REG);
        IioMiscCtrl.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, IIOMISCCTRL_N1_IIO_VTD_REG); 

        LSysCtl.Bits.sev0_en = 0;
        LSysCtl.Bits.sev1_en = 0;
        LSysCtl.Bits.sev2_en = 0;

        //Disable Inband events
        LSysMap.Bits.sev0_map = V_SYSMAP_NoInband;
        LSysMap.Bits.sev1_map = V_SYSMAP_NoInband;
        LSysMap.Bits.sev2_map = V_SYSMAP_NoInband;

        // Clear enable bit on IIOMISCCTRL to disable IO_MCA
        IioMiscCtrl.Bits.enable_io_mca = 0;
        IioMiscCtrl.Bits.enable_pcc_eq0_sev1 = 0;

        // Program Iiomiscctrl to disable IOMCA
        if (mRasCapabilityLib->SiliconRasCapability.IoMcaCap == TRUE) {
          mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, IIOMISCCTRL_N1_IIO_VTD_REG, IioMiscCtrl.Data);
        }
        // Program SysMap
        mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, SYSMAP_IIO_RAS_REG, LSysMap.Data);
        //Program LSysCtl
        mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, LSYSCTL_IIO_RAS_REG, LSysCtl.Data); 
      }
    }
  } else {
    if ((mRasSetupLib->Iio.IioDmiErrorEn) && (mRasSetupLib->SystemErrorEn == 1)) {
      IioDmiErrorEnable();
    }
    // Program Severities and Masks
    if (mRasSetupLib->Iio.IioDmaErrorEn == 1) {
      ChanErrSev.Data = mRasSetupLib->Iio.IioDmaSevBitMap & IIO_DMA_MSK;
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRSEV_INT_IIOCB_FUNC0_REG, ChanErrSev.Data);
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRSEV_INT_IIOCB_FUNC1_REG, ChanErrSev.Data);
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRSEV_INT_IIOCB_FUNC2_REG, ChanErrSev.Data);
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRSEV_INT_IIOCB_FUNC3_REG, ChanErrSev.Data);
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRSEV_INT_IIOCB_FUNC4_REG, ChanErrSev.Data);
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRSEV_INT_IIOCB_FUNC5_REG, ChanErrSev.Data);
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRSEV_INT_IIOCB_FUNC6_REG, ChanErrSev.Data);
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRSEV_INT_IIOCB_FUNC7_REG, ChanErrSev.Data);
    }

    ChanErrMsk.Data =  IIO_DMA_MSK;
    if (mRasSetupLib->Iio.IioDmaErrorEn == 0x01) {
      ChanErrMsk.Data &= ~mRasSetupLib->Iio.IioDmaBitMap | BIT31; // Per EDS always set BIT31
    }
    mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRMSK_INT_IIOCB_FUNC0_REG, ChanErrMsk.Data);
    mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRMSK_INT_IIOCB_FUNC1_REG, ChanErrMsk.Data);
    mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRMSK_INT_IIOCB_FUNC2_REG, ChanErrMsk.Data);
    mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRMSK_INT_IIOCB_FUNC3_REG, ChanErrMsk.Data);
    mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRMSK_INT_IIOCB_FUNC4_REG, ChanErrMsk.Data);
    mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRMSK_INT_IIOCB_FUNC5_REG, ChanErrMsk.Data);
    mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRMSK_INT_IIOCB_FUNC6_REG, ChanErrMsk.Data);
    mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, CHANERRMSK_INT_IIOCB_FUNC7_REG, ChanErrMsk.Data);
  
    for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
      if (mIohInfo[RootBridge].IioStackInfo[IioStack].StackValid) {

        if (mRasSetupLib->Iio.IioVtdErrorEn == 1) {
          VtUncErrSev.Data = mRasSetupLib->Iio.IioVtdSevBitMap & IIO_VTD_MSK;
          mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, VTUNCERRSEV_IIO_VTD_REG, VtUncErrSev.Data);

          VtUncErrMsk.Data = IIO_VTD_MSK;
          VtUncErrMsk.Data &= ~mRasSetupLib->Iio.IioVtdBitMap;
          mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, VTUNCERRMSK_IIO_VTD_REG, VtUncErrMsk.Data);
        }

        if (mRasSetupLib->Iio.IioMiscErrorEn == 1) {  
          MiErrSv.Data = mRasSetupLib->Iio.IioMiscErrorSevMap & IIO_MISC_ERROR_SEVMSK;
          mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, MIERRSV_IIO_RAS_REG, MiErrSv.Data);

          MiErrCnt.Data = 0xff;
          mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, MIERRCNT_IIO_RAS_REG, MiErrCnt.Data);

          MiErrCntSel.Data = 0;
          MiErrCntSel.Bits.dfx_inj_err = 1;
          MiErrCntSel.Bits.vpp_err_sts = 1;
          MiErrCntSel.Bits.jtag_tap_sts = 1;
          MiErrCntSel.Bits.cfg_reg_par = 1;
          mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, MIERRCNTSEL_IIO_RAS_REG, MiErrCntSel.Data);

          MiErrCtl.Data = 0;         
          MiErrCtl.Data = mRasSetupLib->Iio.IioMiscErrorBitMap & IIO_MISC_ERROR_MSK;    
          mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, MIERRCTL_IIO_RAS_REG, MiErrCtl.Data);
        } 

        if (mRasSetupLib->Iio.IioIrpErrorEn == 1)  {        
          IrpErrSvN0.Data = mRasSetupLib->Iio.IioCohSevBitMap & IIO_COH_SEV_MSK;
          mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, IRPPERRSV_N0_IIO_RAS_REG, IrpErrSvN0.Data);

          if (mIohInfo[RootBridge].IioStackInfo[IioStack].StackBus != 0) { // Skip for C-stack
            IrpRingErrSvN0.Data = mCpuCsrAccessLib->ReadCpuCsr(mIohInfo[RootBridge].SocketId, IioStack, IRPRINGERRSV_N0_IIO_RAS_REG);
            IrpRingErrSvN0.Bits.bl_parity_error = IIO_FATAL;
            mCpuCsrAccessLib->WriteCpuCsr(mIohInfo[RootBridge].SocketId, IioStack, IRPRINGERRSV_N0_IIO_RAS_REG, IrpRingErrSvN0.Data);

            IrpRingErrCnt.Data = 0xff;
            mCpuCsrAccessLib->WriteCpuCsr(mIohInfo[RootBridge].SocketId, IioStack, IRPRINGERRCNT_IIO_RAS_REG,IrpRingErrCnt.Data);

            IrpRingErrCntSel.Data = 0;   
            IrpRingErrCntSel.Bits.irpring_error_count_select = 1;
            mCpuCsrAccessLib->WriteCpuCsr(mIohInfo[RootBridge].SocketId, IioStack, IRPRINGERRCNTSEL_IIO_RAS_REG,IrpRingErrCntSel.Data);
          }
          
          // Program Error Counters
          Irpp0ErrCnt.Data = 0xff;
          mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, IRPP0ERRCNT_IIO_RAS_REG, Irpp0ErrCnt.Data);

          Irpp0ErrCntSel.Data = 0;
          Irpp0ErrCntSel.Bits.irp_error_count_select = 0x7ffff;
          mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, IRPP0ERRCNTSEL_IIO_RAS_REG, Irpp0ErrCntSel.Data);

          Irpp0ErrCtl.Data = mRasSetupLib->Iio.IioIrpp0ErrCtlBitMap & IIO_COH_INFACE_MSK;
          mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, IRPP0ERRCTL_IIO_RAS_REG, Irpp0ErrCtl.Data);

        }

        // TODO Add ITC setup question?
        ItcErrSev.Data = mRasSetupLib->Iio.IioItcErrSevBitMap & IIO_ITC_MSK;
        ItcErrSev.Bits.itc_vtmisc_hdr_rf = 0x2;//WA ITC HSD4929000
        //w/a: 5331846
        ItcErrSev.Bits.itc_hw_assert = 2;
        ItcErrSev.Bits.itc_misc_prh_overflow = 2;
        ItcErrSev.Bits.itc_enq_overflow = 2;
        ItcErrSev.Bits.itc_mtc_tgt_err = 2;
        ItcErrSev.Bits.itc_mabort = 0;
        ItcErrSev.Bits.itc_cabort = 0;
        ItcErrSev.Bits.itc_ecc_uncor_rf = 2;
        ItcErrSev.Bits.itc_ecc_cor_rf = 0;
        ItcErrSev.Bits.itc_par_addr_rf = 2;
        ItcErrSev.Bits.itc_vtmisc_hdr_rf = 2;
        ItcErrSev.Bits.itc_par_hdr_rf = 2;
        ItcErrSev.Bits.itc_par_pcie_dat = 2;
        ItcErrSev.Bits.itc_irp_cred_of = 2;
        ItcErrSev.Bits.itc_irp_cred_uf = 2;
        mCpuCsrAccessLib->WriteCpuCsr(mIohInfo[RootBridge].SocketId, IioStack, ITCERRSEV_IIO_RAS_REG, ItcErrSev.Data);

        // TODO Add OTC setup question?
        OtcErrSev.Data = mRasSetupLib->Iio.IioOtcErrSevBitMap & IIO_OTC_MSK;
        if((mRasTopologyLib->SystemInfo.CpuType == CPU_SKX) && (mRasTopologyLib->SystemInfo.CpuStepping < B0_REV_SKX)) {
          OtcErrSev.Data = BIT12;
        }
        //w/a: 5331846
        OtcErrSev.Bits.otc_hw_assert = 2;
        OtcErrSev.Bits.otc_irp_addr_par = 2;
        OtcErrSev.Bits.otc_irp_dat_par = 2;
        OtcErrSev.Bits.otc_par_rte = 2;
        OtcErrSev.Bits.otc_mctp_bcast_to_dmi = 2;
        OtcErrSev.Bits.otc_ecc_cor_rf = 0;
        OtcErrSev.Bits.otc_mtc_tgt_err = 2;
        OtcErrSev.Bits.otc_mabort = 0;
        OtcErrSev.Bits.otc_cabort = 0;
        OtcErrSev.Bits.otc_ecc_uncor_rf = 2;
        OtcErrSev.Bits.otc_par_addr_rf = 2;
        OtcErrSev.Bits.otc_par_hdr_rf = 2;
        OtcErrSev.Bits.otc_ob_cred_of = 2;
        OtcErrSev.Bits.otc_ob_cred_uf = 2;
        mCpuCsrAccessLib->WriteCpuCsr(mIohInfo[RootBridge].SocketId, IioStack, OTCERRSEV_IIO_RAS_REG, OtcErrSev.Data);

        TcErrCnt.Data = 0xff;
        mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, TCERRCNT_IIO_RAS_REG, TcErrCnt.Data);

        TcErrCntSel.Data = 0;
        TcErrCntSel.Bits.otc_ob_cred_uf = 1;
        TcErrCntSel.Bits.otc_ob_cred_of = 1;
        TcErrCntSel.Bits.otc_par_hdr_rf = 1;
        TcErrCntSel.Bits.otc_par_addr_rf = 1;
        TcErrCntSel.Bits.otc_ecc_uncor_rf = 1;
        TcErrCntSel.Bits.otc_cabort = 1;
        TcErrCntSel.Bits.otc_mabort = 1;
        TcErrCntSel.Bits.otc_mtc_tgt_err = 1;
        mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, TCERRCNTSEL_IIO_RAS_REG, TcErrCntSel.Data);

        TcErrCtl.Data = mCpuCsrAccessLib->ReadCpuCsr(mIohInfo[RootBridge].SocketId, IioStack, TCERRCTL_IIO_RAS_REG);
        if((mRasTopologyLib->SystemInfo.CpuType == CPU_SKX) && (mRasTopologyLib->SystemInfo.CpuStepping < B0_REV_SKX)) {
          TcErrCtl.Bits.otc_mabort = 0;
        }
        mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, TCERRCTL_IIO_RAS_REG, TcErrCtl.Data);
  
        // Program Local Control and Map registers
        LSysMap.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, SYSMAP_IIO_RAS_REG);
        CipIntrCtrl.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, CIPINTRC_IIO_VTD_REG);
        IioMiscCtrl.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, IIOMISCCTRL_N1_IIO_VTD_REG); 
        LSysCtl.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridge].SocketId, IioStack,  LSYSCTL_IIO_RAS_REG);
        LBitMap.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridge].SocketId, IioStack,  LBITMAP_IIO_RAS_REG);

        LSysCtl.Bits.sev0_en = 1;
        LSysCtl.Bits.sev1_en = 1;
        LSysCtl.Bits.sev2_en = 1;

        LBitMap.Bits.bit_select = IioStack;
        LBitMap.Bits.select_agent_id = 1;

        if (mRasSetupLib->Iio.IioPcieAerSpecCompEn == 0x01) {
          IioMiscCtrl.Bits.en_poismsg_spec_behavior = 1;  
        } else {
          IioMiscCtrl.Bits.en_poismsg_spec_behavior = 0;
        }

        //We do not enable IOMCA for SKL FPGA SKUs
        if (mRasCapabilityLib->SiliconRasCapability.IoMcaCap && mRasSetupLib->Iio.IoMcaEn) {
          if ((mRasTopologyLib->SystemInfo.FpgaSktActive & (1 << RootBridge)) == 0 ){
        	EnableIoMca = TRUE;
          } else if (mRasTopologyLib->SystemInfo.FpgaSktActive & (1 << RootBridge) && IioStack != mIohInfo[RootBridge].FpgaStack) {
            EnableIoMca = TRUE;
          } else {
            EnableIoMca = FALSE;
          }
        } else {
          EnableIoMca = FALSE;
        }

        if (EnableIoMca) {
          IoMcaFunc = IIOEnableIoMca;
          // Set enable bit on IIOMISCCTRL to enable IO_MCA
          IioMiscCtrl.Bits.enable_io_mca = 1;
          IioMiscCtrl.Bits.enable_pcc_eq0_sev1 = 0;

          // Disable Inband events
          LSysMap.Bits.sev0_map = V_SYSMAP_NoInband;
          LSysMap.Bits.sev1_map = V_SYSMAP_NoInband;
          LSysMap.Bits.sev2_map = V_SYSMAP_NoInband;
          GSysCtl.Bits.severity0_en = 1;
          GSysMap.Bits.severity0_map = V_SYSMAP_SMI_PMI;

        } else {
          IoMcaFunc = IIODisableIoMca;
          IioMiscCtrl.Bits.enable_io_mca = 0;
          IioMiscCtrl.Bits.enable_pcc_eq0_sev1 = 0;
          if (mRasSetupLib->Iio.IioErrorPinEn == FALSE) {

            //disable error pin reporting     
            GErrPinCtl.Bits.pin0 = V_ERRPINCTL_DIS;
            GErrPinCtl.Bits.pin1 = V_ERRPINCTL_DIS;
            GErrPinCtl.Bits.pin2 = V_ERRPINCTL_DIS;

            GSysMap.Bits.severity0_map = V_SYSMAP_SMI_PMI;
            GSysMap.Bits.severity1_map = V_SYSMAP_SMI_PMI;
            GSysMap.Bits.severity2_map = V_SYSMAP_SMI_PMI;
          } else {
          
            LSysMap.Bits.sev0_map = V_SYSMAP_NoInband;
            LSysMap.Bits.sev1_map = V_SYSMAP_NoInband;
            LSysMap.Bits.sev2_map = V_SYSMAP_NoInband;
            //
            // Error Pin Control Register - enable for error severity
            //
            GErrPinCtl.Bits.pin0 = V_ERRPINCTL_SEV;
            GErrPinCtl.Bits.pin1 = V_ERRPINCTL_SEV;
            GErrPinCtl.Bits.pin2 = V_ERRPINCTL_SEV;  

            GSysMap.Bits.severity0_map = V_SYSMAP_NoInband;
            GSysMap.Bits.severity1_map = V_SYSMAP_NoInband;
            GSysMap.Bits.severity2_map = V_SYSMAP_NoInband;
          }
          GSysCtl.Bits.severity0_en = 1;
          GSysCtl.Bits.severity1_en = 1;
          GSysCtl.Bits.severity2_en = 1;
        }//IoMcaEn
        if (mRasCapabilityLib->SiliconRasCapability.IoMcaCap == TRUE) {
          mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, IIOMISCCTRL_N1_IIO_VTD_REG, IioMiscCtrl.Data);
        }
        mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, CIPINTRC_IIO_VTD_REG, CipIntrCtrl.Data);
        mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, SYSMAP_IIO_RAS_REG, LSysMap.Data);
        mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, LSYSCTL_IIO_RAS_REG, LSysCtl.Data); 
        mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, IioStack, LBITMAP_IIO_RAS_REG, LBitMap.Data); 
      }
    }
    
    // Program Error Signals
    ProgramViralPoison ();  

  }

  if (mRasCapabilityLib->SiliconRasCapability.IoMcaCap == TRUE && IoMcaFunc != NULL ) {
    // Scope is package level.
    // Program MC_CTL to enable Fatal and non fatal MC reporting
    SmmStartupPackageApBlocking (mIohInfo[RootBridge].SocketId, IoMcaFunc, NULL);
  }
  //program Global error pin control 
  mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, IIO_ERRPINCTL_UBOX_CFG_REG, GErrPinCtl.Data);

  //Program Global GSysMap
  mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, IIO_GSYSMAP_UBOX_CFG_REG, GSysMap.Data);

  // Program GSysCtl 
  mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridge].SocketId, 0, IIO_GSYSCTL_UBOX_CFG_REG, GSysCtl.Data);
  RASDEBUG ((DEBUG_INFO, "IioErrorEnable: GSysMap.Data: %x \n", GSysMap.Data));
  RASDEBUG ((DEBUG_INFO, "IioErrorEnable: GSysCtl.Data: %x \n", GSysCtl.Data));

  ClearIioErrorRegisterStatus (mIohInfo[RootBridge].SocketId);

  return Status;
}


/**
  This function check for viral errors.

  @param[in] Ioh            The IOH number.

  @retval    Status         return TRUE if viral error is reported.
**/
BOOLEAN
SocketInViralState (
  UINT8 Iio
)
{
  BOOLEAN                                   ViralState = FALSE;
  VIRAL_IIO_RAS_STRUCT                      ViralIioRas;
  UINT8                                     IioStack;

  for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
    if (mIohInfo[Iio].IioStackInfo[IioStack].StackValid != 1) continue;
   
    // read the VIRAL_IIO_RAS to check for Viral State
    ViralIioRas.Data = mCpuCsrAccessLib->ReadCpuCsr (Iio, IioStack, VIRAL_IIO_RAS_REG);

    if (ViralIioRas.Bits.iio_viral_state) {
      ViralState = TRUE;
      break;
    }

  }
   
  return ViralState;
}

/**
  This function puts the machine in a CpuDeadLoop if the socket 
  is in viral state.

  @retval Status.
**/
EFI_STATUS
ProcessViralError (
   VOID
   )
{
  UINT8    Socket;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (mRasTopologyLib->SystemInfo.SocketBitMap & (1 << Socket)) {
      if (SocketInViralState (Socket)) {
        RASDEBUG ((DEBUG_INFO,"ProcessViralError: Viral state detected on Socket %x\n", Socket));
        CpuDeadLoop ();
      }
    }
  }
  return EFI_SUCCESS;
}

/**
  This function enable IIO viral error reporting.

  @param[in] IIO        The IIO number.

  @retval    EFI_SUCCESS Return success if no error.
  @retval    EFI_INVALID_PARAMETER

**/
EFI_STATUS
IioEnableViral (
  IN      UINT8   Iio
  )
{
  VIRAL_IIO_RAS_STRUCT      IioViral; //VIRAL_IIO_RAS_REG
  UINT32                    IioViralMask; 
  UINT8                     IioStack;

  if (Iio >= MAX_SOCKET) {
    return EFI_INVALID_PARAMETER;
  }

  for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
    if (mIohInfo[Iio].IioStackInfo[IioStack].StackValid != 1) continue;
    // Set Viral Enable on the IIO
    IioViral.Data = mCpuCsrAccessLib->ReadCpuCsr (Iio, IioStack, VIRAL_IIO_RAS_REG);
    if (mRasSetupLib->ClearViralStatus == 1){
      IioViral.Bits.iio_viral_status = 1;
      IioViralMask = 0xc0000000;
    }  else {
      IioViral.Bits.iio_viral_status = 0;
      IioViralMask = 0x80000000;
    }
    IioViral.Bits.iio_viral_state = 1;
    do {
      mCpuCsrAccessLib->WriteCpuCsr (Iio, IioStack, VIRAL_IIO_RAS_REG, IioViral.Data);
      IioViral.Data = (mCpuCsrAccessLib->ReadCpuCsr (Iio, IioStack, VIRAL_IIO_RAS_REG) & IioViralMask);
    } while (IioViral.Data);

    IioViral.Data =  mCpuCsrAccessLib->ReadCpuCsr(Iio, IioStack, VIRAL_IIO_RAS_REG) ;
    if (IioViral.Bits.iio_viral_status == 1){
      DEBUG ((DEBUG_ERROR, "IIO viral status found in asserted state. IIO Socket ID :  %d \n", Iio));
      IioViral.Bits.iio_viral_status = 0;
    }
    if (mRasSetupLib->Iio.IoMcaEn == 1) {
      IioViral.Bits.iio_signal_global_fatal = 1;
    }
    IioViral.Bits.iio_global_viral_mask = 1;
    IioViral.Bits.iio_fatal_viral_alert_enable = 1;
    IioViral.Bits.generate_viral_alert = 0;
    mCpuCsrAccessLib->WriteCpuCsr(Iio, IioStack, VIRAL_IIO_RAS_REG,IioViral.Data);
  } //for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++)

  return EFI_SUCCESS;
}

/**
  This function enable forwarding of Poison indication with the data.

  @param[in] Ioh        The IOH number.

  @retval    Status.
**/ 
EFI_STATUS
EnablePoisonIoh (
  IN      UINT8   Ioh
  )
{
  UINT8                         Port;
  UINT8                         Bus;
  UINT8                         Device;
  UINT8                         Function;
  UINT8                         IioStack;
  UNCEDMASK_IIO_PCIE_STRUCT     UncEdMask;
  COREDMASK_IIO_PCIE_STRUCT     CorEdMask;
  CORERRMSK_IIO_PCIE_STRUCT     CorErrMsk;
  IIOMISCCTRL_N1_IIO_VTD_STRUCT IioMiscCtrlN1;

  for (Port = 0; Port < NUMBER_PORTS_PER_SOCKET; Port++) {
    Bus    =  mIohInfo[Ioh].Port[Port].Bus;
    Device = mIohInfo[Ioh].Port[Port].Device;
    Function = mIohInfo[Ioh].Port[Port].Function;

    // Set UNCEDMASK for Port
    UncEdMask.Data = MmioRead32 (MmPciAddress(Ioh,
    		                                      Bus,
                                              Device,
                                              Function,
                                              ONLY_REGISTER_OFFSET (UNCEDMASK_IIO_PCIE_REG)));

    UncEdMask.Bits.poisoned_tlp_detect_mask = 0;

    MmioWrite32 (MmPciAddress(Ioh,
    		                      Bus,
                              Device,
                              Function,
                              ONLY_REGISTER_OFFSET (UNCEDMASK_IIO_PCIE_REG)),
                              UncEdMask.Data);

    // Set COREDMASK for Port
    CorEdMask.Data = MmioRead32 (MmPciAddress(Ioh,
                                              Bus,
                                              Device,
                                              Function,
                                              ONLY_REGISTER_OFFSET (COREDMASK_IIO_PCIE_REG)));

    CorEdMask.Bits.advisory_non_fatal_error_detect_mask = 0;

    MmioWrite32 (MmPciAddress(Ioh,
                              Bus,
                              Device,
                              Function,
                              ONLY_REGISTER_OFFSET (COREDMASK_IIO_PCIE_REG)),
                              CorEdMask.Data);

    // Set CORERRMSK for Port
    CorErrMsk.Data = MmioRead32 (MmPciAddress(Ioh,
                                              Bus,
                                              Device,
                                              Function,
                                              ONLY_REGISTER_OFFSET (CORERRMSK_IIO_PCIE_REG)));

    CorErrMsk.Bits.advisory_non_fatal_error_mask = 0;

    MmioWrite32 (MmPciAddress(Ioh,
                              Bus,
                              Device,
                              Function,
                              ONLY_REGISTER_OFFSET (CORERRMSK_IIO_PCIE_REG)),
                              CorErrMsk.Data);


  }

  for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
    if (mIohInfo[Ioh].IioStackInfo[IioStack].StackValid) {
      // Set Iio poision forward enable
      IioMiscCtrlN1.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[Ioh].SocketId, IioStack, IIOMISCCTRL_N1_IIO_VTD_REG);
      IioMiscCtrlN1.Bits.poisfen = 1;
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[Ioh].SocketId, IioStack, IIOMISCCTRL_N1_IIO_VTD_REG, IioMiscCtrlN1.Data);
    }
  }
  return EFI_SUCCESS;
  }



/**
  This function enables or disables poison forwarding and viral

  @retval None.

**/
VOID
ProgramViralPoison(
  VOID
  )
{

  UINT8        RootBridgeLoop;

  if(mRasCapabilityLib->SiliconRasCapability.PoisonCap == 0)
    return;

  // Enable poison control in IOH
  for (RootBridgeLoop = 0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {
    if (mIohInfo[RootBridgeLoop].Valid) {
      if (mRasSetupLib->PoisonEn == 1) {
        EnablePoisonIoh (RootBridgeLoop);
        if (mRasSetupLib->ViralEn == 1) { //Viral can only be enabled if poison is enabled
          IioEnableViral (RootBridgeLoop);
        }
      }
    }    
  } 
}


/**
  IVT-EX sighting 4031306 : CLONE from ivytown: IVT B0 - RAS - SMI not getting delivered
  to UBox when in VIRAL & SMI is generated on fatal error.

  @retval    None.
**/
VOID
SetupIioCipIntrCtrl (
  IN      UINT8   RootBridgeLoop
  )
{
  UINT8                         IioStack;
  CIPINTRC_IIO_VTD_STRUCT       IioCipIntrCtrl;

  for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
    if (mIohInfo[RootBridgeLoop].IioStackInfo[IioStack].StackValid) {
      IioCipIntrCtrl.Data = mCpuCsrAccessLib->ReadCpuCsr (mIohInfo[RootBridgeLoop].SocketId, IioStack, CIPINTRC_IIO_VTD_REG);
      IioCipIntrCtrl.Bits.smi_mask = 0;
      IioCipIntrCtrl.Bits.smi_msi_en = 1;

      //Program IIOCIPINTRC
      mCpuCsrAccessLib->WriteCpuCsr (mIohInfo[RootBridgeLoop].SocketId, IioStack, CIPINTRC_IIO_VTD_REG, IioCipIntrCtrl.Data);
    }
  }
}


/**
  This Function will enable all the Pcie errors, that need to be reported as LER.

  @param[in] Ioh        The IOH number.

  @retval    Status.
**/
EFI_STATUS
IIOLerEnable (
  IN      UINT8   Ioh
  )
{
  UINT8                             Port;
  UINT8                             Bus;
  UINT8                             Device;
  UINT8                             Function;
  LER_XPUNCERRMSK_IIO_PCIE_STRUCT   LerXpUnCerrMask;
  LER_RPERRMSK_IIO_PCIE_STRUCT      LerRootPortErrorMask;
  LER_CTRLSTS_IIO_PCIE_STRUCT       LerControl;
  LER_UNCERRMSK_IIO_PCIE_STRUCT     LerUnCerrMask;
    UINT16   CapabilitiesOffset;

  for (Port = 0; Port < NUMBER_PORTS_PER_SOCKET; Port++) {

    Bus = mIohInfo[Ioh].Port[Port].Bus;
    Device = mIohInfo[Ioh].Port[Port].Device;
    Function = mIohInfo[Ioh].Port[Port].Function;

    // Verify any PciE Device exist at the Bus, Device and Function
    if (!PciErrLibIsDevicePresent (Ioh, Bus, Device, Function)) {
      continue;
    }

    // Check if it is a root Port.
    if (!PCIeIsRootPort (Ioh, Bus, Device, Function)) {
      continue;
}

    // Get the capability offset for LER   
    CapabilitiesOffset = PciErrLibGetExtendedCapabilitiesOffset (Ioh, Bus, Device, Function, PCIE_EXT_CAP_HEADER_LERC, PCIE_EXT_CAP_HEADER_LER_VSECID);
    if (CapabilitiesOffset == 0) {
      continue;
    }

    // Below code will enable the PciE errors, which need to be considered as LER
    LerUnCerrMask.Data = MmioRead32(MmPciAddress (Ioh, Bus, Device, Function, (CapabilitiesOffset + 0xC)));
    LerUnCerrMask.Bits.data_link_layer_protocol_error_mask = 0;
    LerUnCerrMask.Bits.surprise_down_error_mask = 0;
    LerUnCerrMask.Bits.poisoned_tlp_mask = 0;
    LerUnCerrMask.Bits.flow_control_protocol_error_mask = 0;
    LerUnCerrMask.Bits.completion_time_out_mask = 0;
    LerUnCerrMask.Bits.completer_abort_mask = 0;
    LerUnCerrMask.Bits.unexpected_completion_mask = 0;
    LerUnCerrMask.Bits.malformed_tlp_mask = 0;
    if(mRasSetupLib->Pcie.PcieAerAdNfatErrEn == 1) { 
      LerUnCerrMask.Bits.unsupported_request_error_mask = 0;
    } else {
      LerUnCerrMask.Bits.unsupported_request_error_mask = 1;
    }
    LerUnCerrMask.Bits.acs_violation_mask = 0;
    LerUnCerrMask.Bits.receiver_buffer_overflow_mask=0;

    MmioWrite32 (MmPciAddress (Ioh, Bus, Device, Function, (CapabilitiesOffset + 0xC)), LerUnCerrMask.Data);

    LerXpUnCerrMask.Data = MmioRead32(MmPciAddress (Ioh, Bus, Device, Function, (CapabilitiesOffset + 0x10)));
    LerXpUnCerrMask.Bits.sent_completion_with_ca_mask = 1;
    LerXpUnCerrMask.Bits.sent_completion_with_ur_mask = 1;
    LerXpUnCerrMask.Bits.received_pcie_completion_with_ca_status_mask = 1;
    LerXpUnCerrMask.Bits.received_pcie_completion_with_ur_status_mask = 1;
    LerXpUnCerrMask.Bits.outbound_poisoned_data_mask = 1;
    MmioWrite32 (MmPciAddress (Ioh, Bus, Device, Function, (CapabilitiesOffset + 0x10)), LerXpUnCerrMask.Data);

    LerRootPortErrorMask.Data = MmioRead32(MmPciAddress (Ioh, Bus, Device, Function, (CapabilitiesOffset + 0x14)));
    LerRootPortErrorMask.Bits.non_fatal_error_message_received_mask = 1;
    LerRootPortErrorMask.Bits.fatal_error_message_received_mask = 1;
    MmioWrite32 (MmPciAddress (Ioh, Bus, Device, Function, (CapabilitiesOffset + 0x14)), LerRootPortErrorMask.Data);

    LerControl.Data = MmioRead32(MmPciAddress (Ioh, Bus, Device, Function, (CapabilitiesOffset + 0x8)));
    LerControl.Bits.ler_ss_severity_en = 1;
    LerControl.Bits.ler_ss_inten = 0;
    LerControl.Bits.ler_ss_drop_txn = 1;
    LerControl.Bits.ler_ss_enable = 1;
    MmioWrite32 (MmPciAddress (Ioh, Bus, Device, Function, (CapabilitiesOffset + 0x8)), LerControl.Data);

  }
  return EFI_SUCCESS;
}

/**

    IIO Local error handler routine.

    @param SocketId   Id of the socket 

    @retval EFI_SUCCESS if the call is succeed.

**/
EFI_STATUS
ProcessLocalIioErrors (
  IN      UINT8   SocketIndex
)
{
  UINT8                     SocketId;
  UINT8                     IioStack;
  LSYSST_IIO_RAS_STRUCT     LSysSt;
  LSYSCTL_IIO_RAS_STRUCT    LSysCtl;
  LFERRST_IIO_RAS_STRUCT    LFErrSt;
  LNERRST_IIO_RAS_STRUCT    LNErrSt;
  LCERRST_IIO_RAS_STRUCT    LCErrSt;
  UINT32                    PcieFErr;
  UINT32                    PcieNErr;
  UINT32                    PcieCErr;

  if(SocketIndex >= MAX_SOCKET) {
    return EFI_INVALID_PARAMETER;
  }

  for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
    if (mIohInfo[SocketIndex].IioStackInfo[IioStack].StackValid) {
      SocketId = mRasTopologyLib->SystemInfo.SocketInfo[SocketIndex].UncoreIioInfo.SocketId;

      LSysSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LSYSST_IIO_RAS_REG);
      LSysCtl.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LSYSCTL_IIO_RAS_REG);
      if (LSysCtl.Data & LSysSt.Data) {
        //
        // Local IIO Error Status
        //
        LFErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LFERRST_IIO_RAS_REG); 
        LNErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LNERRST_IIO_RAS_REG); 
        LCErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, IioStack, LCERRST_IIO_RAS_REG);

        if (LFErrSt.Data) {
          DEBUG ((DEBUG_ERROR, "ProcessLocalIioErrors : SocketId = %x, IioStack = %x LFErrSt = %x \n", SocketIndex, IioStack, LFErrSt.Data));
  }
        if (LNErrSt.Data) {
          DEBUG ((DEBUG_ERROR, "ProcessLocalIioErrors : SocketId = %x, IioStack = %x LNErrSt = %x \n", SocketIndex, IioStack, LNErrSt.Data));
  }
        if (LCErrSt.Data) {
          DEBUG ((DEBUG_ERROR, "ProcessLocalIioErrors : SocketId = %x, IioStack = %x LCErrSt = %x \n", SocketIndex, IioStack, LCErrSt.Data));
  }

        // Check if it is Pci-ex/DMI error.
        PcieFErr = LFErrSt.Data & IIO_GxERRST_PCIE_MASK;
        PcieNErr = LNErrSt.Data & IIO_GxERRST_PCIE_MASK;
        PcieCErr = LCErrSt.Data & IIO_GxERRST_PCIE_MASK;


        if (PcieFErr || PcieNErr || PcieCErr) {
          IohPcieErrorHandler(SocketIndex, IioStack);
        }
        // APTIOV_SERVER_OVERRIDE_RC_START : Changes done to Support VTD/ITC/DMA/OTC Errors
        if (LFErrSt.Bits.vtd || LNErrSt.Bits.vtd) {
             HandleVtdErrors (SocketId, IioStack);
        }

        if ( (LFErrSt.Data & ITC_ERROR_STATUS_BIT )  || ( LNErrSt.Data & ITC_ERROR_STATUS_BIT ) ) {  //LFErrSt.Bits.ioh needs tobe changed to meaningful name
             HandleItcErrors (SocketId, IioStack);
        }

        if ( (LFErrSt.Data & OTC_ERROR_STATUS_BIT )  || ( LNErrSt.Data & OTC_ERROR_STATUS_BIT ) ) {  //LFErrSt.Bits.thermal needs to be changed to meaningful name
             HandleOtcErrors (SocketId, IioStack);
        }

        if (LFErrSt.Bits.dma || LNErrSt.Bits.dma) {
             HandleDmaErrors (SocketId, IioStack);
        }    
        // APTIOV_SERVER_OVERRIDE_RC_END : Changes done to Support VTD/ITC/DMA/OTC Errors
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  IIO error main handler routine.

  This function performs the IOH error checking and processing functions

  @retval Status.
**/
EFI_STATUS
ProcessIioErrors (
  VOID
  )
{
  UINT8                           SocketIndex;
  UINT8                           SocketId;
  IIO_GSYSST_UBOX_CFG_STRUCT      GSysSt;
  IIO_GF_ERRST_UBOX_CFG_STRUCT    GFErrSt;
  IIO_GNF_ERRST_UBOX_CFG_STRUCT   GNErrSt;
  IIO_GC_ERRST_UBOX_CFG_STRUCT    GCErrSt;
  IIO_GSYSCTL_UBOX_CFG_STRUCT     GSysCtl;

  //
  // Check for viral error. The system goes into a deadloop if viral is found on any of the IIO
  //
  ProcessViralError ();
    
  //
  // Global Register (UBOX) check
  //
  for ( SocketIndex =0; SocketIndex < MAX_SOCKET; SocketIndex++) {
    if(mRasTopologyLib->SystemInfo.SocketInfo[SocketIndex].UncoreIioInfo.Valid) {
      SocketId = mRasTopologyLib->SystemInfo.SocketInfo[SocketIndex].UncoreIioInfo.SocketId;

      GSysSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, 0, IIO_GSYSST_UBOX_CFG_REG);
      GSysCtl.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, 0, IIO_GSYSCTL_UBOX_CFG_REG);
      if (GSysCtl.Data & GSysSt.Data) {

      //
      // Check Global IIO Error Status
      //
        GFErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, 0, IIO_GF_ERRST_UBOX_CFG_REG) & IIO_GLBL_ERR_BITMASK; 
        GNErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, 0, IIO_GNF_ERRST_UBOX_CFG_REG) & IIO_GLBL_ERR_BITMASK; 
        GCErrSt.Data = mCpuCsrAccessLib->ReadCpuCsr(SocketId, 0, IIO_GC_ERRST_UBOX_CFG_REG) & IIO_GLBL_ERR_BITMASK;

        if (GFErrSt.Data) {
          DEBUG ((DEBUG_ERROR, "ProcessIioErrors : SocketId = %x, GFErrSt = %x \n", SocketId, GFErrSt.Data));
        }
        if (GNErrSt.Data) {
          DEBUG ((DEBUG_ERROR, "ProcessIioErrors : SocketId = %x, GNErrSt = %x \n", SocketId, GNErrSt.Data));
        }
        if (GCErrSt.Data) {
          DEBUG ((DEBUG_ERROR, "ProcessIioErrors : SocketId = %x, GCErrSt = %x \n", SocketId, GCErrSt.Data));
        }

        //
        // If error, check Local Register (IIO) of specific IIO Stack
        // 
        if ( GFErrSt.Data | GNErrSt.Data | GCErrSt.Data ) {
          ProcessLocalIioErrors (SocketIndex);
        }
      
        //
        // Clear Global registers device status
        //
        ClearIioErrorRegisterStatus(SocketId);
      }
    }
  }
  return EFI_SUCCESS;
}


/**
  Initialize PCIE_IIO specific RAS capabilities. 

  @param[in, out] Capability  Pointer to the Silicon Ras capability structure.

  @retval Status.

**/
EFI_STATUS
InitPcieSiliconCap (
  IN       OUT   SILICON_RAS_CAPABILITY  *Capability
  )
{  
  switch(mRasTopologyLib->SystemInfo.SystemRasType) {
    case ADVANCED_RAS:
    case FPGA_RAS:
      Capability->IoMcaCap = TRUE;
      Capability->LerCap = TRUE;
      break;

    case STANDARD_RAS:
    case CORE_RAS:
      Capability->IoMcaCap = TRUE;
      Capability->LerCap = FALSE;
      break;

    default:
      Capability->IoMcaCap = FALSE;
      Capability->LerCap = FALSE;
      break;
  }
  Capability->EnhanRpErrRepCap = TRUE;
  
  return EFI_SUCCESS;
}



/**

    Allocates protocols and initialize internal structure required by the library.  

    @param ImageHandle - refers to the image handle of the driver.
    @param SystemTable - points to the EFI System Table.

    @retval EFI_SUCCESS - Library constructor executed correctly 

**/
EFI_STATUS
EFIAPI
ChipsetErrReportingConstructor (
IN EFI_HANDLE ImageHandle,
IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS Status;
  Status = gSmst->SmmLocateProtocol (
                    &gEfiPlatformRasPolicyProtocolGuid,
                    NULL,
                    &mPlatformRasPolicyProtocolLib
                    );
  ASSERT_EFI_ERROR (Status);
  
  mRasTopologyLib = mPlatformRasPolicyProtocolLib->EfiRasSystemTopology;
  mRasSetupLib = mPlatformRasPolicyProtocolLib->SystemRasSetup;
  mRasCapabilityLib = mPlatformRasPolicyProtocolLib->SystemRasCapability;

  Status = gSmst->SmmLocateProtocol (
                    &gEfiCpuCsrAccessGuid, 
                    NULL, 
                    &mCpuCsrAccessLib
                    );
  ASSERT_EFI_ERROR (Status);


  return EFI_SUCCESS;
}







