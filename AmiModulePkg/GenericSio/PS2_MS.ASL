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
// Name:  <PS2ms.ASL>
//
// Description: Define ACPI method or namespce For Super IO
//
//<AMI_FHDR_END>
//*************************************************************************

//**********************************************************************;
// PS2 Mouse Device, IO category # - 12
//---------------------------------------------------------------------
//Device(PS2M) {
//        Name(_HID, EISAID("PNP0F03"))	// Hardware Device ID - Microsoft mouse
					// check if MSFT Mouse driver supports D3 properly on all MSFT OSes. 
					// It may prevent OS to go to S3 sleep state
					// Use Logitech _HID instead if OS rejecting to go to S3.
//        Name(_HID, EISAID("PNP0F12"))	// Logitech PS2 Mouse ID
        Name(_CID, EISAID("PNP0F13"))	// Compatible ID

        Method(_STA, 0) {
                // Check if PS2Mouse detected in BIOS Post
                // IOST - bit mask of enabled devices, 0x4000 - PS2M mask
                If(And(\IOST, 0x4000)){		// Check if PS2MS detected in BIOS Post
                	Return(0x0f) 
				} else {
					Return(0x00)// device's not present
				}
        }
        Name(CRS1, ResourceTemplate()
        {
                IRQNoFlags(){12}
        })
        Name(CRS2, ResourceTemplate()
	{
		IO(Decode16, 0x60, 0x60, 0, 0x1)
		IO(Decode16, 0x64, 0x64, 0, 0x1)
                IRQNoFlags(){12}
	})
        Method(_CRS,0)
        {
        	If(And(\IOST, 0x0400)){	// PS2K is present, I/O resources 0x60 & 0x64 are reserved there
       		    Return(CRS1)
		} else {	        // single PS/2 mouse(no PS/2 kbd), need to supply I/O resources 0x60 & 0x64 for PS/2 Controller.
		    Return(CRS2)
		}
   	}

//-----------------------------------------------------------------------
//NOTE: _PRS MUST be the NAME not a METHOD object 
//to have GENERICSIO.C working right! 
//-----------------------------------------------------------------------
	Name(_PRS, ResourceTemplate(){
		StartDependentFn(0, 0) { 
			IRQNoFlags(){12} 
		}
		EndDependentFn()
	})

	// Mouse 2nd Level wake up control method
	Method(_PSW, 1){
		Store(Arg0, \MSFG)
	}

//}// End of PS2M

Scope(\){
	Name(\MSFG, 0x01)	//Mouse wake-up flag default enable
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
