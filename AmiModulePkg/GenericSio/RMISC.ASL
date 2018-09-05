// Miscellaneous I/O and memory resources
// System board extension Device node for ACPI BIOS
// THIS FILE IS INCLUDED to LPC/ISA PCI bridge scope
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
// $Header: $
//
// $Revision: $
//
// $Date:  $
//
//**********************************************************************;

//;<AMI_PHDR_START>
//;------------------------------------------------------------------------
//;
//; Procedure:	RMSC
//;
//; Description:	Chipset Miscellaneous I/O and memory resources
//; This table should contain any I/O port that is not used by a specific
//; device but does not return FFh when read.  Some examples of I/O ports
//; that should be reserved here are:
//;   Any I/O port in the range 00 - FF that is not used by any other dev node
//;   The IRQ edge/level control ports (4D0/4D1)
//;
//;  Input: Nothing
//;
//;  Output: _CRS buffer/
//;
//;-------------------------------------------------------------------------
//;<AMI_PHDR_END>

	Device(RMSC) {

		Name(_HID, EISAID("PNP0C02"))	// System board resources device node ID
		Name(_UID, 0x10)		// Unique ID. Used first I/O range address from _CRS buffer.

		Name(CRS1, ResourceTemplate()
		{
			IO(Decode16,0x10, 0x10, 0,0x10)
			IO(Decode16,0x22, 0x22, 0,0x1e)
			IO(Decode16,0x44, 0x44, 0,0x1c)
			IO(Decode16,0x62, 0x62, 0,0x02)
			IO(Decode16,0x65, 0x65, 0,0x0b)
			IO(Decode16,0x72, 0x72, 0,0x0e)
			IO(Decode16,0x80, 0x80, 0,0x01)
			IO(Decode16,0x84, 0x84, 0,0x03)
			IO(Decode16,0x88, 0x88, 0,0x01)
			IO(Decode16,0x8c, 0x8c, 0,0x03)
			IO(Decode16,0x90, 0x90, 0,0x10)
			IO(Decode16,0xa2, 0xa2, 0,0x1e)
			IO(Decode16,0xe0, 0xe0, 0,0x10)
// Decoded but not used by FDC. Reserved in FDC device node resources
//	              IO(Decode16, 0x3f3, 0x3f3, 0, 0x1)
// Reserve  4D0 and 4D1 for IRQ edge/level control port
			IO(Decode16, 0x4d0, 0x4d0, 0, 0x2)
//S.Y Not existing resource
// SB NVRAM 1 I/O space
//			IO(Decode16, 0xDE00, 0xDE00, 0, 0x80)
// SB NVRAM 2 I/O space
//			IO(Decode16, 0xDE80, 0xDE80, 0, 0x80)

		}               // End of ResourceTemplate
		)               // end of CRS1
		Name(CRS2, ResourceTemplate()
		{
			IO(Decode16,0x10, 0x10, 0,0x10)
			IO(Decode16,0x22, 0x22, 0,0x1e)
			IO(Decode16,0x44, 0x44, 0,0x1c)            
			IO(Decode16,0x72, 0x72, 0,0x0e)
			IO(Decode16,0x80, 0x80, 0,0x01)
			IO(Decode16,0x84, 0x84, 0,0x03)
			IO(Decode16,0x88, 0x88, 0,0x01)
			IO(Decode16,0x8c, 0x8c, 0,0x03)
			IO(Decode16,0x90, 0x90, 0,0x10)
			IO(Decode16,0xa2, 0xa2, 0,0x1e)
			IO(Decode16,0xe0, 0xe0, 0,0x10)
// Decoded but not used by FDC. Reserved in FDC device node resources
//	              IO(Decode16, 0x3f3, 0x3f3, 0, 0x1)
// Reserve  4D0 and 4D1 for IRQ edge/level control port
			IO(Decode16, 0x4d0, 0x4d0, 0, 0x2)
//S.Y Not existing resource
// SB NVRAM 1 I/O space
//			IO(Decode16, 0xDE00, 0xDE00, 0, 0x80)
// SB NVRAM 2 I/O space
//			IO(Decode16, 0xDE80, 0xDE80, 0, 0x80)

		}               // End of ResourceTemplate
		)               // end of CRS2
		Method (_CRS, 0)
		{
			If(And(\MBEC, 0xFFFF)){//Check if there is an EC in system
                Return(CRS1)
            }else{
                Return(CRS2)
            }
		}
}
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
