//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
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
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  <UART2.ASL>
//
// Description: Define ACPI method or namespce For Super IO
//
//<AMI_FHDR_END>
//*************************************************************************
// UART2 //
// Category # :0x01
//Device(UAR2) {
//    Name(_HID, EISAID("PNP0501"))    //PnP Device ID 16550 Type
//	Name(_UID, 2)	//Generic ID for COMB

// Use the following if not using SI1P or only have 1 SIO	
//	Method(_HID, 0)	{Return(^^SIO1.UHID(1))}	//PnP Device ID
	Method(_STA, 0) {Return(^^SIO1.DSTA(1))}	//Get UART status
	Method(_DIS, 0) {^^SIO1.DCNT(1, 0)}			//Disable UART
    Method(_CRS, 0) {Return(^^SIO1.DCRS(1, 0))}	//Get UART current resources
	Method(_SRS, 1) {^^SIO1.DSRS(Arg0, 1)}		//Set UART resources

// Use the following if using SI1P
//	Method(_HID, 0) {	//PnP Device ID
//		if(SI1P){Return(^^SIO1.UHID(1))}
//		else{Return(^^SIO2.UHID(1))}
//	}
//	Method(_STA, 0) {	//Get UART status
//		if(SI1P){Return(^^SIO1.DSTA(1))}
//		else{Return(^^SIO2.DSTA(1))}
//	}
//	Method(_DIS, 0) {	//Disable UART
//		if(SI1P){^^SIO1.DCNT(1, 0)}
//		else{^^SIO2.DCNT(1, 0)}
//	}
//	Method(_CRS, 0) {	//Get UART current resources
//		if(SI1P){Return(^^SIO1.DCRS(1, 0))}
//		else{Return(^^SIO2.DCRS(1, 0))}
//	}
//	Method(_SRS, 1) {	//Set UART resources
//		if(SI1P){^^SIO1.DSRS(Arg0, 1)}
//		else{^^SIO2.DSRS(Arg0, 1)}
//	}

//------------------------------------------------------------------------
// UART2 Possible Resources
//------------------------------------------------------------------------
//NOTE: _PRS MUST be the NAME not a METHOD object 
//to have GENERICSIO.C working right! 
//-----------------------------------------------------------------------
	Name(_PRS, ResourceTemplate() {
		StartDependentFn(0, 0) {
			IO(Decode16, 0x2F8, 0x2F8, 1, 8)
			IRQNoFlags() {3}
			DMA(Compatibility, NotBusMaster, Transfer8) {}
		}
		StartDependentFnNoPri() {
			IO(Decode16, 0x3F8, 0x3F8, 1, 8)
			IRQNoFlags() {3,4,5,7,9,10,11,12}
			DMA(Compatibility, NotBusMaster, Transfer8) {}
		}
		StartDependentFnNoPri() {
			IO(Decode16, 0x2F8, 0x2F8, 1, 8)
			IRQNoFlags() {3,4,5,7,9,10,11,12}
			DMA(Compatibility, NotBusMaster, Transfer8) {}
		}
		StartDependentFnNoPri() {
			IO(Decode16, 0x3E8, 0x3E8, 1, 8)
			IRQNoFlags() {3,4,5,7,9,10,11,12}
			DMA(Compatibility, NotBusMaster, Transfer8) {}
		}
		StartDependentFnNoPri() {
			IO(Decode16, 0x2E8, 0x2E8, 1, 8)
			IRQNoFlags() {3,4,5,7,9,10,11,12}
			DMA(Compatibility, NotBusMaster, Transfer8) {}
		}
		EndDependentFn()
	})

//} // End Of UAR2
//-----------------------------------------------------------------------

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
