//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date:  $
//**********************************************************************;
//**********************************************************************;
// UART6 //
// Category # :0x10
//Device(UAR6) {
//    Name(_HID, EISAID("PNP0501"))    //PnP Device ID 16550 Type
//	Name(_UID, 6)	                            //Generic ID for COMF

//	Method(_HID, 0)	{Return(^^SIO1.UHID(0x10))}	//PnP Device ID
	Method(_STA, 0) {Return(^^SIO1.DSTA(0x10))}	//Get UART status
	Method(_DIS, 0) {^^SIO1.DCNT(0x10, 0)}		//Disable UART
	Method(_CRS, 0) {Return(^^SIO1.DCR4(0x10, 0))}//Get UART current resources
	Method(_SRS, 1) {^^SIO1.DSRS(Arg0, 0x10)} 		//Set UART recources

//-----------------------------------------------------------------------
// UART4 Possible Resources
//-----------------------------------------------------------------------
//NOTE: _PRS MUST be the NAME not a METHOD object 
//to have GENERICSIO.C working right! 
//-----------------------------------------------------------------------
	Name(_PRS, ResourceTemplate() {
		StartDependentFn(0, 0) {
			IO(Decode16, 0x2E0, 0x2E0, 1, 8)
			IRQ(Edge,ActiveLow,Shared) {10}
			DMA(Compatibility, NotBusMaster, Transfer8) {}
		}
		StartDependentFnNoPri() {
			IO(Decode16, 0x3F8, 0x3F8, 1, 8)
			IRQ(Edge,ActiveLow,Shared) {3,4,5,7,9,10,11,12}
			DMA(Compatibility, NotBusMaster, Transfer8) {}
		}
		StartDependentFnNoPri() {
			IO(Decode16, 0x2F8, 0x2F8, 1, 8)
			IRQ(Edge,ActiveLow,Shared) {3,4,5,7,9,10,11,12}
			DMA(Compatibility, NotBusMaster, Transfer8) {}
		}
		StartDependentFnNoPri() {
			IO(Decode16, 0x3E8, 0x3E8, 1, 8)
			IRQ(Edge,ActiveLow,Shared) {3,4,5,7,9,10,11,12}
			DMA(Compatibility, NotBusMaster, Transfer8) {}
		}
		StartDependentFnNoPri() {
			IO(Decode16, 0x2E8, 0x2E8, 1, 8)
			IRQ(Edge,ActiveLow,Shared) {3,4,5,7,9,10,11,12}
			DMA(Compatibility, NotBusMaster, Transfer8) {}
		}
        StartDependentFnNoPri() {
			IO(Decode16, 0x2F0, 0x2F0, 1, 8)
			IRQ(Edge,ActiveLow,Shared) {3,4,5,7,9,10,11,12}
			DMA(Compatibility, NotBusMaster, Transfer8) {}
		}
        StartDependentFnNoPri() {
			IO(Decode16, 0x2E0, 0x2E0, 1, 8)
			IRQ(Edge,ActiveLow,Shared) {3,4,5,7,9,10,11,12}
			DMA(Compatibility, NotBusMaster, Transfer8) {}
		}
		EndDependentFn()
	})

//} // End Of UAR4
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
