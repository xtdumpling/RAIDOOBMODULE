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
// Name:  <PS2kb.ASL>
//
// Description: Define ACPI method or namespce For Super IO
//
//<AMI_FHDR_END>
//*************************************************************************

//**********************************************************************;
// PS2 Keyboard Device, IO category # - 10
//---------------------------------------------------------------------


//Device(PS2K) {
//    Name(_HID,EISAID("PNP0303"))        // Standard Keyboard 101/102
    Name(_CID,EISAID("PNP030b"))        // Compatible ID, PC/AT Enhanced Keyboard 101/102
    Method(_STA,0) {
		If(And(\IOST, 0x0400)){
			Return (0x0F)
		} else {
			Return (0x00)
		}
	}

    Name(_CRS,ResourceTemplate()
    {
        IO(Decode16, 0x60, 0x60, 0, 0x1)    //PS2 resource
		IO(Decode16, 0x64, 0x64, 0, 0x1)
		IRQNoFlags(){1}
    })

//-----------------------------------------------------------------------
//NOTE: _PRS MUST be the NAME not a METHOD object 
//to have GENERICSIO.C working right! 
//-----------------------------------------------------------------------
	Name(_PRS, ResourceTemplate(){
		StartDependentFn(0, 0) {  
            IO(Decode16, 0x60, 0x60, 0, 0x1)  
            IO(Decode16, 0x64, 0x64, 0, 0x1)
			IRQNoFlags(){1}
		}
		EndDependentFn()
	})

	// Keyboard 2nd Level wake up control method
	Method(_PSW, 1){
		Store(Arg0, \KBFG)
	}
//}// End of PS2K

Scope(\){
	Name(\KBFG, 0x01)	//Keyboard wake-up flag default enable
}
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
