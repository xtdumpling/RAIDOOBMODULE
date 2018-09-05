//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <UART4.asl>
//
// Description: Define ACPI device for UART4.
//
//<AMI_FHDR_END>
//**********************************************************************
// Category # :0x12
//======================================================================
//Following commented line will be included in PciTree.asl 
//Device(UAR4) {
//    Name(_UID, 4)                                 //Generic ID for COMD
//    Name(_HID, EISAID("PNP0501"))                 //PnP Device ID 16550 Type
//======================================================================
    Method(_STA, 0) {Return(^^SIO1.ESTA(0x12))}     //Get UART status
    Method(_DIS, 0) {^^SIO1.DCNT(0x12, 0)}          //Disable UART
    Method(_CRS, 0) {Return(^^SIO1.DCR3(0x12, 0))}  //Get UART current resources
    Method(_SRS, 1) {^^SIO1.DSR3(Arg0, 0x12)}       //Set UART resources
//----------------------------------------------------------------------
// UART4 Possible Resources
//----------------------------------------------------------------------
//NOTE: _PRS MUST be the NAME not a METHOD object 
//to have GENERICSIO.C working right! 
//----------------------------------------------------------------------
    Name(_PRS, ResourceTemplate() {
        StartDependentFn(0, 0) {
            IO(Decode16, 0x2E8, 0x2E8, 1, 8)
            IRQ(Level,ActiveLow,Shared) {7}
            DMA(Compatibility, NotBusMaster, Transfer8) {}
        }
        StartDependentFnNoPri() {
            IO(Decode16, 0x3E8, 0x3E8, 1, 8)
            IRQ(Level,ActiveLow,Shared) {3,4,5,6,7,10,11,12}
            DMA(Compatibility, NotBusMaster, Transfer8) {}
        }
        StartDependentFnNoPri() {
            IO(Decode16, 0x2E8, 0x2E8, 1, 8)
            IRQ(Level,ActiveLow,Shared) {3,4,5,6,7,10,11,12}
            DMA(Compatibility, NotBusMaster, Transfer8) {}
        }
        StartDependentFnNoPri() {
            IO(Decode16, 0x2F0, 0x2F0, 1, 8)
            IRQ(Level,ActiveLow,Shared) {3,4,5,6,7,10,11,12}
            DMA(Compatibility, NotBusMaster, Transfer8) {}
        }
        StartDependentFnNoPri() {
            IO(Decode16, 0x2E0, 0x2E0, 1, 8)
            IRQ(Level,ActiveLow,Shared) {3,4,5,6,7,10,11,12}
            DMA(Compatibility, NotBusMaster, Transfer8) {}
        }
        EndDependentFn()
    })
//======================================================================
//Following commented line will be included in PciTree.asl 
// }// End Of UAR4
//======================================================================
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

