// The ASL file for _OSC Method Implementation
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
//-----------------------------------------------------------------------

//;<AMI_PHDR_START>
//;------------------------------------------------------------------------
//;
//; Procedure:    _OSC
//; Description:  METHOD CALLED ON SYSTEM STARTUP TO NEGOTIATE OS/BIOS
//; SUPPORTED FEATURES
//; Input: 	Arg0 (Buffer): UUID
//;		Arg1 (Integer): Revision ID
//;		Arg2 (Integer): Count
//;		Arg3 (Buffer): Capabilities Buffer,
//;
//; Output: Returns modified Arg3.
//;
//;-------------------------------------------------------------------------
//;<AMI_PHDR_END>

		Method(_OSC,4,Serialized)
		{ 
			Name(SUPP,0) // PCI _OSC Support Field value
			Name(CTRL,0) // PCI _OSC Control Field value
			// Create DWord-adressable fields from the Capabilities Buffer
			CreateDWordField(Arg3,0,CDW1)
			CreateDWordField(Arg3,4,CDW2)
			CreateDWordField(Arg3,8,CDW3)
			// Check for proper UUID
			If(LEqual(Arg0,ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766")))
			{
				// Save Capabilities DWord2 & 3
				Store(CDW2,SUPP)
				Store(CDW3,CTRL)
				//Support Bits: 
				//0 - extended pci config operation region
				//1 - ASPM supported
				//2 - Clock Power Management Capability supported
				//3 - Pci Segment Groups supported
				//4 - MSI supported
		
				//Control Bits: 
				//0 - Pci Express Native Hot Plug control
				//1 - SHPC Native Hot Plug control
				//2 - Pci Express Native Power Management Events control
				//3 - Pci Express Advanced Error Reporting control
				//4 - Pci Express Capability Structure control
		
				// Only allow native hot plug control if OS supports:
				// * ASPM
				// * Clock PM
				// * MSI/MSI-X
		
				If(LNotEqual(And(SUPP, 0x16), 0x16))
				{
					And(CTRL,0x1E, CTRL) // Mask bit 0 (and undefined bits)
				}
				//Tokens ACPI.sdl
				if (LNot(PEHP))//BIOS Supports PCI Express HP? 
				{
					And(CTRL,0x1E,CTRL)
				}
				if (LNot(SHPC)) //BIOS Supports SHPC Hot Plug ? 
				{
					And(CTRL,0x1D,CTRL)
				}
				if (LNot(PEPM)) //BIOS Supports PCI Express ASPM?  
				{
					And(CTRL,0x1B,CTRL) //Have OS Control ASPM
					//Or(CTRL,0x04)     //Keep BIOS Settings.
				}
				if (LNot(PEER))	//BIOS Supports PCI Express Adv Error Reporting? 	 
				{
					And(CTRL,0x15,CTRL)
				}
				if (LNot(PECS)) //BIOS Provides PCI Express MCFG Table?  
				{
					And(CTRL,0xF,CTRL)
				}
		
				If(LNotEqual(Arg1,One))
				{ // Unknown revision
					Or(CDW1,0x08,CDW1)
				}
				If(LNotEqual(CDW3,CTRL))
				{ // Certain Capabilities bits were masked
					Or(CDW1,0x10,CDW1)
				}

				// Update DWORD3 in the buffer
				Store(CTRL,CDW3)

				Return(Arg3)
			} Else {
				Or(CDW1,4,CDW1) // Unrecognized UUID
				Return(Arg3)
			}
		} // End _OSC

//-----------------------------------------------------------------------
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
