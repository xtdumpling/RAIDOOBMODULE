//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
//*************************************************************************
// $Header:$
//
// $Revision:  $
//
// $Date: $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:    AmiPciExpressLib.h
//
// Description: 
//
// Notes:
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef _AMI_PCI_EXPRESS_LIB_H_
#define _AMI_PCI_EXPRESS_LIB_H_
#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Pci.h>
#include <PciE.h>
#include <PciE21.h>
#include <PciE30.h>
#include <PciBus.h>
#include <AmiLib.h>
#include <AcpiRes.h>
#include <Protocol/PciIo.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//Variable, Prototype, and External Declarations
//-------------------------------------------------------------------------
extern T_ITEM_LIST  gPcieEpList;


//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//  EFI_SUCCESS - Set successfully.
//  EFI_INVALID_PARAMETER - the Input parameter is invalid.
//
// Notes:
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieDoubleCheckCard(PCI_DEV_INFO *Brg, PCI_CFG_ADDR *DevAddr, UINT32 *VenDevId);


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieCalcMaxPayLoad()
//
// Description: This function will calculate MAX PayLoad Size needed to work
// correctly.
//
// Input:
//  PCI_DEV_INFO    *EndPoint   Pointer to PCI Device Private Data structure.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT16 PcieCalcMaxPayLoad(PCI_DEV_INFO *EndPoint);


//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//  EFI_SUCCESS - Set successfully.
//  EFI_INVALID_PARAMETER - the Input parameter is invalid.
//
// Notes:
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID PcieRecordBootScript(PCI_DEV_INFO *Brg, EFI_S3_SAVE_STATE_PROTOCOL *S3SaveStateProtocol, BOOLEAN RecordScript);


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieConvertLatency()
//
// Description: This function will convert Latency value from Values read from.
// PPCIe.LNK_CNT_REG to Nanoseconds and opposite.
//
// Input:
//  UINTN   Latency     Latency Value to convert
//  BOOLEAN L0s         Tells if L0s or L1 Exit latency
//  BOOLEAN ToNs        Tells how to convert:
//                      TRUE  - to the units of nano Seconds;
//                      FALSE - to the LNK_CNT_REG.LxExitLat register value
//
// Output:	EFI_STATUS
//  UINTN               Converted Value (-1) - means Invalid Value passed;
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
//Return value in nS of Max L0s, L1 Exit latency .
UINTN PcieConvertLatency(UINTN Latency, BOOLEAN L0s, BOOLEAN ToNs);



//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieCalcLatency()
//
// Description: This function will calculate MAX Latency needed to exit from
// ome of the ASPM Sattes L0s or L1.
//
// Input:
//  PCI_DEV_INFO    *Device Pointer to PCI Device Private Data structure.
//  BOOLEAN         L0s     Tells which ASPM state Latency we are calculating
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINTN PcieCalcLatency(PCI_DEV_INFO *EndPoint, BOOLEAN L0s);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieCalcMaxPayLoad()
//
// Description: This function will calculate MAX PayLoad Size needed to work
// correctly.
//
// Input:
//  PCI_DEV_INFO    *EndPoint   Pointer to PCI Device Private Data structure.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT16 PcieCalcMaxPayLoad(PCI_DEV_INFO *EndPoint);


//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure:  
//
// Description:
//
// Input:
//
// Output:
//  EFI_SUCCESS - Set successfully.
//  EFI_INVALID_PARAMETER - the Input parameter is invalid.
//
// Notes:
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieAllocateInitPcieData(PCI_DEV_INFO *Device, UINT8 PcieCapOffset);


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieUpdateClockPm()
//
// Description: This function will update Device's referenced as "Func0"
// Clock Power Management based on Setup Settings and hardware capabilities.
//
// Input:
//  PCI_DEV_INFO    *Func0    Pointer to PCI Device Private Data structure.
//  CNT_REG         *LnkCnt   Pointer to the Device's LNK_CNT register data.
//
// Output:	NOTHING
//
// NOTE: Must be called only for PCI Express devices (Device->PciExpress!=NULL)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID PcieUpdateClockPm(PCI_DEV_INFO *Func0, PCIE_LNK_CNT_REG *LnkCnt);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieDisableLink()
//
// Description: This function will Disable Link of DNSTREAM port referenced as
// "Device" based on Setup Settings and hardware STATUS. Used to disable "EMPTY"
// links to save some POWER or "Force" LINK to Disable state if Link can not be
// trained correctly.
//
// Input:
//  PCI_DEV_INFO    *Device    Pointer to PCI Device Private Data structure.
//  BOOLEAN         Force      Flag to indicate to disable link unconditionally.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
// NOTE: Must be called only for PCI Express devices (Device->PciExpress!=NULL)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieDisableLink(PCI_DEV_INFO *Device, BOOLEAN Force);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieProgramLink()
//
// Description: This function will Program Link with provided VALUEs
// trained correctly.
//
// Input:
//  PCI_DEV_INFO    *Device    Pointer to PCI Device Private Data structure.
//  UINT16          Data       Value to put in LNK_CNT register
//  BOOLEAN         Reg2       Flag to select LNK_CNT or LNK_CNT2 register.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
// NOTE: Must be called only for PCI Express devices (Device->PciExpress!=NULL)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieProgramLink(PCI_DEV_INFO *Device, UINT16 Data, BOOLEAN Reg2);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieResetLink()
//
// Description: This function will issue HOT RESET on a LINK referenced by "Device".
//
// Input:
//  PCI_DEV_INFO    *Device    Pointer to PCI Device Private Data structure.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
// NOTE: Must be called only for PCI Express devices (Device->PciExpress!=NULL)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieResetLink(PCI_DEV_INFO *Device, UINT8 LnkSpeed);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieSetSlotProperties()
//
// Description: This function will Initialize SLT_CNT and SLT_STA registers
// of the "Device".
//
// Input:
//  PCI_DEV_INFO    *Device    Pointer to PCI Device Private Data structure.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
// NOTE: Must be called only for PCI Express devices (Device->PciExpress!=NULL)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS PcieSetSlotProperties(PCI_DEV_INFO *Device);


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieRetrainLink()
//
// Description: This function will initiate LINK training of the
// Down Stream Port referenced as "Device".
//
// Input:
//  PCI_DEV_INFO    *Device    Pointer to PCI Device Private Data structure.
//  UINT16          Data       Value to put in LNK_CNT register
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
// NOTE: Must be called only for PCI Express devices (Device->PciExpress!=NULL)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieRetrainLink(	PCI_DEV_INFO 		*Device,
							UINT16 				Data,
							UINT8 				LnkSpeed,
							UINTN 				RetryCount,
							UINTN 				TimeOut);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieGetCaps()
//
// Description: This function will Collect PCI Express Capabilities Data
// of the "Device".
//
// Input:
//  PCI_DEV_INFO    *Device    Pointer to PCI Device Private Data structure.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
// NOTE: Must be called only for PCI Express devices (Device->PciExpress!=NULL)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieGetCaps(PCI_DEV_INFO *Device);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieSetLnkProperties()
//
// Description: This function will Select values for Link Control2 register on
// both sides of the LINK based on Setup Settiongs and hardware capabilities.
//
// Input:
//  PCI_DEV_INFO        *DnStreamPort   Pointer to PCI Device Private Data of Downstream Port of the link.
//  PCIE_LNK_CNT2_REG   *DnLnkCnt      Pointer to the LNK_CNT Reg of the Downstream Port of the link.
//  PCI_DEV_INFO        *UpStreamPort   Pointer to PCI Device Private Data of Upwnstream Port of the link.
//  PCIE_LNK_CNT2_REG   *UpLnkCnt      Pointer to the LNK_CNT Reg of the Downstream Port of the link.
//  BOOLEAN             *LinkTrainingRequired Flag to modify if Link will need RETRAINING after programming.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieSetLnkProperties( PCI_DEV_INFO           *DnStreamPort,
                                 PCIE_LNK_CNT_REG       *DnLnkCnt,
                                 PCI_DEV_INFO           *UpStreamPort,
                                 PCIE_LNK_CNT_REG       *UpLnkCnt,
                                 BOOLEAN                *LinkTrainingRequired
                                 );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieInitLink()
//
// Description: This function will Initialize Pcie Link on both sides starting
// fronm DownStream Port.
//
// Input:
//  PCI_DEV_INFO    *DnPort    Pointer to PCI Device Private Data structure.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieInitLink(PCI_DEV_INFO 	*DnPort);



//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieIsDownStreamPort()
//
// Description: This function will check if *PCI_DEV_INFO passed belongs to
// PCI Express DOWNSTREAM PORT:
//  - RootPort of PCIe Root Complex;
//  - DownStream Port of PCIe Switch;
//  - PCI/PCI-X to PCIe Bridge.
//
// Input:
//  PCI_DEV_INFO    *Dev    Pointer to PCI Device Private Data structure.
//
// Output:	BOOLEAN
//  TRUE         When PCI_DEV_INFO passed belongs to DOWNSTREAM PORT
//  FALSE        All other cases...
//
// NOTE: Must be called only for PCI Express devices (Device->PciExpress!=NULL)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN PcieIsDownStreamPort(PCI_DEV_INFO *Device);



//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//  EFI_SUCCESS - Set successfully.
//  EFI_INVALID_PARAMETER - the Input parameter is invalid.
//
// Notes:
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN PcieCheckPcieCompatible(PCI_DEV_INFO	*Device);


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieCollectDeviceExtCap()
//
// Description: This function will Fill out structure of PciE Ext Cap data
//
// Input:
//  PCI_DEV_INFO    *Device     Pointer to PCI Device Private Data structure.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieCollectDeviceExtCap(	PCI_DEV_INFO 	*Device,
								VOID 			**DataPtr,
								UINTN 			DataSize,
								UINT32 			CapHdrOffset,
								BOOLEAN 		Cap16,
								UINTN 			OffsetFromCapHdr);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieSetSlotPower()
//
// Description: This function will power ON or OFF PCI Express Slot
//
// Input:
//  PCI_DEV_INFO    *Dev    Pointer to PCI Device Private Data structure.
//  BOOLEAN         PwrOn   Tells to turn slot power ON or OFF
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieSetSlotPower(PCI_DEV_INFO* Device, BOOLEAN PwrOn);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieRemoveEpFromLst()
//
// Description: This function will removed Device referencesd as EndPoint from
// the gPcieEpLst data structure used as a database of available EndPoint devices.
//
// Input:
//  PCI_DEV_INFO    *EndPoint   Pointer to PCI Device Private Data structure.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_NOT_FOUND           EndPoint not in the gPcieEpLst.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieRemoveEpFromLst(PCI_DEV_INFO *EndPoint, BOOLEAN AllFunc);


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieInitVc()
//
// Description: This function will Initialize "Device"'s Virtual Channel properties
// based on Setup Settings and hardware capabilities.
//
// Input:
//  PCI_DEV_INFO        *Device   Pointer to PCI Device Private Data.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieInitVc(PCI_DEV_INFO  *Device);


//----------------------------------------------------------------------------
// Procedure:   PcieSetAspm()
//
// Description: This function will Select values for Link Control2 register on
// both sides of the LINK based on Setup Settings and hardware capabilities.
//
// Input:
//  PCI_DEV_INFO        *DnPort   Pointer to PCI Device Private Data of Downstream Port of the link.
//  PCI_DEV_INFO        *UpPort   Pointer to PCI Device Private Data of Upwnstream Port of the link.
//  UINT16              Aspm      ASPM value to programm.
//  BOOLEAN             *LinkHotResetRequired Flag to modify if Link will need HOT RESET after programming.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieSetAspm(PCI_DEV_INFO *DnPort,PCI_DEV_INFO *UpPort,UINT16 Aspm);


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieSetDevProperties()
//
// Description: This function will Select values for DEVICE CONTROL register
// based on Setup Settings and hardware capabilities.
//
// Input:
//  PCI_DEV_INFO        *Device         Pointer to PCI Device Private Data.
//  UINT16              *MaxPayload     Pointer to the MaxPayload value.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieSetDevProperties(PCI_DEV_INFO *Device, UINT16 MaxPayload );


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieApplyDevSettings()
//
// Description: This function will Select values for DEVICE CONTROL register
// based on Setup Settings and hardware capabilities.
//
// Input:
//  PCI_DEV_INFO    *EndPoint   Pointer to PCI Device Private Data.
//  UINT16          MaxPayload  MaxPayload value.
//  UINT16          LatencyL0s  Latency L0s value.
//  UINT16          LatencyL1   Latency L1 value.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieApplyDevSettings(PCI_DEV_INFO *EndPoint, UINT16 MaxPayLoad, UINT16 LatencyL0s, UINT16 LatencyL1);


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PcieInitDevChain()
//
// Description: This function will detect optimal settings and programm -
// MaxPayLoadSize, ReadRequestSize, ASPM settings and Virtual Channels
// for all devices in the PCIE Links Chain.
//
// Input:
//  PCI_DEV_INFO    *Device Pointer to PCI Device Private Data structure.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
// Notes:
//  RootPortComplex<=>Switch0...<=>...SwitchN<=>EndPoint
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieInitDevChain(PCI_DEV_INFO* Device);


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   InitDeviceVc()
//
// Description: This function will Init VC_DATA structure of the "Device".
//
// Input:
//  PCI_DEV_INFO    *Device     Pointer to PCI Device Private Data.
//  PCI_CFG_ADDR    *DevAddr    Pointer to Device Address value.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PcieInitDeviceVc(PCI_DEV_INFO *Device, PCI_CFG_ADDR *DevAddr);


//----------------------------------------------------------------------------
// Procedure:   PcieProbeDevice()
//
// Description: This function will collect information about PCIE Device
// and initialize it based on information collected.
//
// Input:
//  PCI_DEV_INFO    *Device Pointer to PCI Device Private Data structure.
//
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_OUT_OF_RESOURCES    When system runs out of resources.
//  EFI_DEVICE_ERROR        When driver fails access PCI Bus.
//
// Notes: The function will do the following steps
//  1.disable ASPM if any;
//  2.disable PMI Generation if any;
//  3.power up PCIE Slot (if capable) if the slot has device behind it;
//  4.if nothing connected to slot, power it down to make it HOTPLUG READY;
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
//EFI_STATUS PcieProbeDevice(PCI_DEV_INFO *Device, UINT8 *MaxBusFound, BOOLEAN SriovSupport);
EFI_STATUS PcieProbeDevice(PCI_DEV_INFO *Device, BOOLEAN SriovSupport);


EFI_STATUS PcieInitDevice(PCI_DEV_INFO *Device, UINT8 *MaxBusFound);

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: PcieCheckIntegratedDevice()
//
// Description: Checks and initializes according to the setup settings 
// Integratred PCI Express Device like Integrated End Point and RC 
// Event Collector. This Device/Port type value == PCIE_TYPE_RC_INTEGR_EP
// or PCIE_TYPE_RC_EVT_COLLECT don't have a Link assotiated with them and 
// from standard PCIe registers have only Device CAP|CTL|STA registers set.
// This devices not supporting ASPM and only settings thy can accept -  
//	- Enable Relaxed Ordering;
//	- Extended Tag Field Enable;
//	- Enable No Snoop;
//	- Max_Payload_Size;
//	- Max_Read_Request_Size;
//
// Input:
//  PCI_DEV_INFO    *IntPcieDev - a Pointer to PCI Device Private Data structure.
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS PcieCheckIntegratedDevice(PCI_DEV_INFO *IntPcieDev);





/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif //_AMI_PCI_EXPRESS_LIB_H_
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

