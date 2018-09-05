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
// Name:  <FDC.ASL>
//
// Description: Define ACPI method or namespce For Super IO
//
//<AMI_FHDR_END>
//*************************************************************************

//**********************************************************************;
// Floppy Disk Controller - FDC //
//**********************************************************************;
// Category # :0x03
//Device(FDC) {
//    Name(_HID, EISAID("PNP0700"))        //PnP Device ID
// _FDE : 5 Dword Package (1-4 are for each FDD present, #5 is for optional tape drive)
// 0 - Device is not present
// 1 - Device is Present
// 2 - Device is never Present
// >2- Reserved
// Tape drive is never present
    Name(_FDE, Buffer(){1,0,2,2,2})    //_FDE - Floppy Disk Enumerate

    Method(_STA, 0) {Return(^^SIO1.DSTA(3))} //Get status

    Method(_DIS, 0) {^^SIO1.DCNT(3, 0)}      //Disable FDC

    Method(_CRS, 0) {                   //Return FDC Current Resources
        ^^SIO1.DCRS(3, 1)               //Fill in Return buffer with DMA, Irq and 1st IO
        //Move resources from CRS1 ro CRS2 buffer
        Store(^^SIO1.IRQM, ^^SIO1.IRQE) //IRQ mask 0x1
        Store(^^SIO1.DMAM, ^^SIO1.DMAE) //DMA 0x04
        Store(^^SIO1.IO11, ^^SIO1.IO21) //1st IO Range Min Base Word 0x8
        Store(^^SIO1.IO12, ^^SIO1.IO22) //             Max Base Word 0xa
        Store(6, ^^SIO1.LEN2)

        Add(^^SIO1.IO21, 0x07, ^^SIO1.IO31)  //2nd IO range
        Store(^^SIO1.IO31, ^^SIO1.IO32)
        Store(1, ^^SIO1.LEN3)
        Return(^^SIO1.CRS2)
    }

//------------------------------------------------------------------------
//NOTE: _PRS MUST be the NAME not a METHOD object 
//to have GENERICSIO.C working right! 
//-----------------------------------------------------------------------
    Name(_PRS, ResourceTemplate() {         //FDC Possible Resources
        StartDependentFn(0, 0) {
            IO(Decode16, 0x3F0, 0x3F0, 1, 6)
            IO(Decode16, 0x3F7, 0x3F7, 1, 1)//0x3F6 port reserved for Legacy IDE
            IRQNoFlags() {6}
            DMA(Compatibility, NotBusMaster, Transfer8) {2}
        }
        StartDependentFnNoPri() {
            IO(Decode16, 0x3F0, 0x3F0, 1, 6)
            IO(Decode16, 0x3F7, 0x3F7, 1, 1)//0x3F6 port reserved for Legacy IDE
            IRQNoFlags() {3,4,5,6,7,10,11,12}
            DMA(Compatibility, NotBusMaster, Transfer8) {2,3}
        }
        StartDependentFnNoPri() {
            IO(Decode16, 0x370, 0x370, 1, 6)
            IO(Decode16, 0x377, 0x377, 1, 1)//0x376 port reserved for Legacy IDE
            IRQNoFlags() {3,4,5,6,7,10,11,12}
            DMA(Compatibility, NotBusMaster, Transfer8) {2,3}
        }
        EndDependentFn()
    })
    
    Method(_SRS, 1) {                           //Set resources/Enable FDC
        ^^SIO1.DSRS(Arg0, 3)

        CreateWordField(Arg0, ^^SIO1.IRQ2._INT, IRQE) //IRQ mask 0x1
        CreateByteField(Arg0, ^^SIO1.DMA2._DMA, DMAE) //DMA 0x4

        ^^SIO1.ENFG(^^SIO1.CGLD(3))             //Enter Config Mode, Select LDN
        // Set IRQ
        If(IRQE){
            FindSetRightBit(IRQE, Local0)
            Subtract(Local0, 1, ^^SIO1.INTR)
        }Else{
            Store(0, ^^SIO1.INTR)               // No IRQ used
        }
        // Set DMA
        If(DMAE){
            FindSetRightBit(DMAE, Local0)
            Subtract(Local0, 1, ^^SIO1.DMCH)
        }Else{
            Store(4, ^^SIO1.DMCH)               // No DMA
        }
        ^^SIO1.EXFG()                           //Exit Config Mode
    }


//---------Power Resources for FDD -------------------------
//
//    PowerResource(FDDP, 0, 0) {     // SystemLevel Parameter=0,
//        Method(_STA, 0) {
//            Return(FDCP)            // Get Power Status
//        }                           // end of _STA
//        Method(_ON) {
//            Store(1, FDCP)          // Power on
//        }                           // end of _ON
//        Method(_OFF){
//            Store(0, FDCP)          // Power off
//        }                           // end of _OFF
//    }
//    Name(_PR0, Package(){FDDP})     // Reference to PowerResources
//} // End Of FDC0 

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

