//**********************************************************************;
//     *****************************************************************;
//     **                                                             **;
//     **      (C)Copyright 1985-2003, American Megatrends, Inc.      **;
//     **                                                             **;
//     **                     All Rights Reserved.                    **;
//     **                                                             **;
//     **           6145-F Northbelt Pkwy, Norcross, GA 30071         **;
//     **                                                             **;
//     **                     Phone (770)-246-8600                    **;
//     **                                                             **;
//     *****************************************************************;
//-----------------------------------------------------------------------
// $Header: /Alaska/BIN/Modules/ACPI/Template/Core/PCIEACCESS.ASL 1     5/07/09 2:44p Yakovlevs $
//
// $Revision: 1 $
//
// $Date: 5/07/09 2:44p $
//****************************************************************;
//-----------------------------------------------------------------------

Scope (\_SB) {

//PCIe Capability Structure pointer

Name (XCPD, 0x0)			// PCIe Cap ID
Name (XNPT, 0x1)			// Next Pointer Cap Pointer
Name (XCAP, 0x2)			// PCIe Capability pointer
Name (XDCP, 0x4)			// Device Capability
Name (XDCT, 0x8)			// Device Control
Name (XDST, 0xa)			// Device Status
Name (XLCP, 0xc)			// Link Capability
Name (XLCT, 0x10)			// Link Control
Name (XLST, 0x12)			// Link Status
Name (XSCP, 0x14)			// Slot Capability
Name (XSCT, 0x18)			// Slot Control
Name (XSST, 0x1A)			// Slot Status
Name (XRCT, 0x1C)			// Root Control
Mutex(MUTe, 0)				//Mutex object to syncronize Memory mapped access



//	Read PCIe Byte Arg0 : Offset (Zero based)

	Method (RbPe, 1) {			
		Acquire(MUTe, 1000)
		Add (Arg0, \PEBS, Local0) 		// PEBS :  PCIe MM base address
		OperationRegion (PCFG, SystemMemory, Local0, 0x1)

		Field (PCFG, ByteAcc, Nolock, Preserve){
			XCFG, 8 ,	
		}
		Release (MUTe)		
		Return (XCFG)
	}					// End of \SB.RbPe

//	Read PCIe Word Arg0 : Offset (Zero based)
	Method (RwPe, 1) {			
		Acquire(MUTe, 1000)
		And (Arg0, 0xFFFFFFFE, Arg0)
		Add (Arg0, \PEBS, Local0) 		// PEBS :  PCIe MM base address
		OperationRegion (PCFG, SystemMemory, Local0, 0x2)

		Field (PCFG, WordAcc, Nolock, Preserve){
			XCFG, 16 ,	
		}
		Release (MUTe)		
		Return (XCFG)
	}					// End of \SB.RwPe

//	Read PCIe Dword Arg0 : Offset (Zero based)
	Method (RdPe, 1) {			
		Acquire(MUTe, 1000)
		And (Arg0, 0xFFFFFFFC, Arg0)	
		Add (Arg0, \PEBS, Local0) 		// PEBS :  PCIe MM base address
		OperationRegion (PCFG, SystemMemory, Local0, 0x4)

		Field (PCFG, DwordAcc, Nolock, Preserve){
			XCFG, 32 ,	
		}
		Release (MUTe)		
		Return (XCFG)
	}					// End of \SB.RdPe

//	Write PCIe byte Arg0 : Offset (Zero based), Arg1 : Value (Byte)
	Method (WbPe, 2) {			 
		Acquire(MUTe, 0xfff)
		Add (Arg0, \PEBS, Local0) 
		OperationRegion (PCFG, SystemMemory, Local0, 0x1)
		Field (PCFG, ByteAcc, Nolock, Preserve){
			XCFG, 8 ,	
		}
		Store (Arg1,XCFG)
		Release (MUTe)
	}					// End of \SB.WbPe

//	Write PCIe word Arg0 : Offset (Zero based), Arg1 : Value (word)
	Method (WwPe, 2) {			 
		Acquire(MUTe, 1000)
		And (Arg0, 0xFFFFFFFE, Arg0)		
		Add (Arg0, \PEBS, Local0) 
		OperationRegion (PCFG, SystemMemory, Local0, 0x2)
		Field (PCFG, WordAcc, Nolock, Preserve){
			XCFG, 16 ,	
		}
		Store (Arg1,XCFG)
		Release (MUTe)
	}					// End of \SB.WwPe

//	Write PCIe Dword Arg0 : Offset (Zero based), Arg1 : Value (Dword)
	Method (WdPe, 2) {			 
		Acquire(MUTe, 1000)
		And (Arg0, 0xFFFFFFFC, Arg0)		
		Add (Arg0, \PEBS, Local0) 
		OperationRegion (PCFG, SystemMemory, Local0, 0x4)

		Field (PCFG, DwordAcc, Nolock, Preserve){
			XCFG, 32 ,	
		}
		Store (Arg1,XCFG)
		Release (MUTe)
	}					// End of \SB.WdPe


//	RW PCIe Dword Arg0 : Offset (Zero based), Arg1 : Value (Dword), Arg2 : Mask (Dword)
	Method (RWdP, 3) {			 
		Acquire(MUTe, 1000)
		And (Arg0, 0xFFFFFFFC, Arg0)	
		Add (Arg0, \PEBS, Local0) 
		OperationRegion (PCFG, SystemMemory, Local0, 0x4)

		Field (PCFG, DwordAcc, Nolock, Preserve){
			XCFG, 32 ,	
		}
		And (XCFG, Arg2, Local1)
		Or (Local1, Arg1, XCFG)
		Release (MUTe)		
	}					// End of \_SB.RWdP

//	Checks for PME status in Root port status register
	Method (RPME, 1) {			// Arg0 Enhanced config. memory address map
		add (Arg0, 0x84, Local0)			
		Store (\_SB.RdPe(Local0), Local1)
		if (LEQUAL(Local1, 0xffffffff)) {
			Return (0)		// PME not asserted
		}  	
		else {
			if (Land(Local1, 0x10000)) {
				\_SB.WdPe(Local0, And(Local1, 0x10000))
				Return (1)		// PME asserted
			}
			Return (0)		// PME not asserted
		}
	}					// End of RPME
}						// End of \_SB
