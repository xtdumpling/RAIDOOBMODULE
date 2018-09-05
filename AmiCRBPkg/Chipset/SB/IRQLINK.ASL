// Generic PCI IRQ Link devices
// File is included to ASL root
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
// $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/PatsburgPCH/Chipset/IRQLINK.asl 1     12/09/11 1:19p Archanaj $
//
// $Revision: 1 $
//
// $Date: 12/09/11 1:19p $
//**********************************************************************;
// Revision History
// ----------------
// $Log: /Alaska/BIN/Chipset/Intel/SouthBridge/PatsburgPCH/Chipset/IRQLINK.asl $
// 
// 1     12/09/11 1:19p Archanaj
// 
// 2     7/07/10 2:52p Bhimanadhunik
// Intial check-in for Patsburg.
// 
// 1     2/24/10 11:04p Tonywu
// Intel Cougar Point/SB eChipset initially releases.
//
//**********************************************************************;
//      Generic PCI IRQ Link devices
//-----------------------------------------------------------------------
// Note. All Link devices are placed under \_SB scope
//-----------------------------------------------------------------------
//-	OperationRegion(\_SB.PC00.SBRG.PIX0, PCI_Config, 0x60, 12)
//-	Field(\_SB.PC00.SBRG.PIX0,ByteAcc,NoLock,Preserve){
//	OperationRegion(\_SB.PC00.LPC0.PIX0, PCI_Config, 0x60, 12)
//	Field(\_SB.PC00.LPC0.PIX0,ByteAcc,NoLock,Preserve){
//		PIRA,8, 		// IRQA route register
//		PIRB,8, 		// IRQB route register
//		PIRC,8, 		// IRQC route register
//		PIRD,8, 		// IRQD route register
//		, 32,
//		PIRE,8, 		// IRQE route register
//		PIRF,8, 		// IRQF route register
//		PIRG,8, 		// IRQG route register
//		PIRH,8, 		// IRQH route register
//		}

Scope(\_SB) {

// Not required as SDL wizard creates PWRB device when Power button notification is enabled for any PCI device by setting "PWRBnotify = Yes" in SDL object
//	Device(PWRB){
//		Name(_HID,EISAID("PNP0C0C"))
//		Name(_UID, 0xAA)	// UID is hardwired to 0xAA
//		Name(_STA, 0x0B)	// device present/hidden in OS Device Manager
//		Method(_PRW, 0){ Return(GPRW(0x1D, 3)) }
//	}

	Name(BUFA,
		ResourceTemplate()
		{
			IRQ(Level, ActiveLow, Shared ) {15}
	})
	CreateWordField(BUFA, 0x1, IRA0)	// IRQ Mask
//--------------------------------------------------------------------------//
	Device(LNKA){
                Name(_HID, EISAID("PNP0C0F"))
                Name(_UID, 1)

                Method(_STA,0) {
			And(PARC, 0x80, Local0)
			If(Local0) {Return (0x9)} // LNKA desabled/present/not visible in UI
			Else   {Return (0xb)} // LNKA enabled
		}

		Method(_PRS,0){Return(PRSA)}

		Method(_DIS){Or(PARC, 0x80 ,PARC)} // Disable PIRQ routing  

		Method(_CRS){
			And(PARC, 0x0f, Local0)
			ShiftLeft(0x1, Local0, IRA0)
			Return(BUFA)
		}					// End Method _CRS

		Method(_SRS, 1){	// ARG0 = PnP Resource String 
			CreateWordField(ARG0, 0x01, IRA)// IRQ Low Mask
			FindSetRightBit(IRA, Local0)
			Decrement(Local0)
			Store(Local0, PARC)
		}					// End Method _SRS
	} // end Device LNKA
//--------------------------------------------------------------------------//
	Device(LNKB){
                Name(_HID, EISAID("PNP0C0F"))
                Name(_UID, 2)

                Method(_STA,0) {
			And(PBRC, 0x80, Local0)
			If(Local0) {Return (0x9)} // LNKB desabled/present/not visible in UI
			Else   {Return (0xb)} // LNKB enabled
		}

		Method(_PRS,0){Return(PRSB)}

		Method(_DIS){Or(PBRC, 0x80 ,PBRC)} // Disable PIRQ routing  

		Method(_CRS){
			And(PBRC, 0x0f, Local0)
			ShiftLeft(0x1, Local0, IRA0)
			Return(BUFA)
		}					// End Method _CRS

		Method(_SRS, 1){	// ARG0 = PnP Resource String 
			CreateWordField(ARG0, 0x01, IRA)// IRQ Low Mask
			FindSetRightBit(IRA, Local0)
			Decrement(Local0)
			Store(Local0, PBRC)
		}					// End Method _SRS
	} // end Device LNKB
//--------------------------------------------------------------------------//
	Device(LNKC){
                Name(_HID, EISAID("PNP0C0F"))
                Name(_UID, 3)

                Method(_STA,0) {
			And(PCRC, 0x80, Local0)
			If(Local0) {Return (0x9)} // LNKC desabled/present/not visible in UI
			Else   {Return (0xb)} // LNKC enabled
		}

		Method(_PRS,0){Return(PRSC)}

		Method(_DIS){Or(PCRC, 0x80 ,PCRC)} // Disable PIRQ routing  

		Method(_CRS){
			And(PCRC, 0x0f, Local0)
			ShiftLeft(0x1, Local0, IRA0)
			Return(BUFA)
		}					// End Method _CRS

		Method(_SRS, 1){	// ARG0 = PnP Resource String 
			CreateWordField(ARG0, 0x01, IRA)// IRQ Low Mask
			FindSetRightBit(IRA, Local0)
			Decrement(Local0)
			Store(Local0, PCRC)
		}					// End Method _SRS
	} // end Device LNKC
//--------------------------------------------------------------------------//
	Device(LNKD){
                Name(_HID, EISAID("PNP0C0F"))
                Name(_UID, 4)

                Method(_STA,0) {
			And(PDRC, 0x80, Local0)
			If(Local0) {Return (0x9)} // LNKD desabled/present/not visible in UI
			Else   {Return (0xb)} // LNKD enabled
		}

		Method(_PRS,0){Return(PRSD)}

		Method(_DIS){Or(PDRC, 0x80 ,PDRC)} // Disable PIRQ routing  

		Method(_CRS){
			And(PDRC, 0x0f, Local0)
			ShiftLeft(0x1, Local0, IRA0)
			Return(BUFA)
		}					// End Method _CRS

		Method(_SRS, 1){	// ARG0 = PnP Resource String 
			CreateWordField(ARG0, 0x01, IRA)// IRQ Low Mask
			FindSetRightBit(IRA, Local0)
			Decrement(Local0)
			Store(Local0, PDRC)
		}					// End Method _SRS
	} // end Device LNKD
//--------------------------------------------------------------------------//
	Device(LNKE){
                Name(_HID, EISAID("PNP0C0F"))
                Name(_UID, 5)

                Method(_STA,0) {
			And(PERC, 0x80, Local0)
			If(Local0) {Return (0x9)} // LNKE desabled/present/not visible in UI
			Else   {Return (0xb)} // LNKE enabled
		}

		Method(_PRS,0){Return(PRSE)}

		Method(_DIS){Or(PERC, 0x80 ,PERC)} // Disable PIRQ routing  

		Method(_CRS){
			And(PERC, 0x0f, Local0)
			ShiftLeft(0x1, Local0, IRA0)
			Return(BUFA)
		}					// End Method _CRS

		Method(_SRS, 1){	// ARG0 = PnP Resource String 
			CreateWordField(ARG0, 0x01, IRA)// IRQ Low Mask
			FindSetRightBit(IRA, Local0)
			Decrement(Local0)
			Store(Local0, PERC)
		}					// End Method _SRS
	} // end Device LNKE
//--------------------------------------------------------------------------//
	Device(LNKF){
                Name(_HID, EISAID("PNP0C0F"))
                Name(_UID, 6)

                Method(_STA,0) {
			And(PFRC, 0x80, Local0)
			If(Local0) {Return (0x9)} // LNKF desabled/present/not visible in UI
			Else   {Return (0xb)} // LNKF enabled
		}

		Method(_PRS,0){Return(PRSF)}

		Method(_DIS){Or(PFRC, 0x80 ,PFRC)} // Disable PIRQ routing  

		Method(_CRS){
			And(PFRC, 0x0f, Local0)
			ShiftLeft(0x1, Local0, IRA0)
			Return(BUFA)
		}					// End Method _CRS

		Method(_SRS, 1){	// ARG0 = PnP Resource String 
			CreateWordField(ARG0, 0x01, IRA)// IRQ Low Mask
			FindSetRightBit(IRA, Local0)
			Decrement(Local0)
			Store(Local0, PFRC)
		}					// End Method _SRS
	} // end Device LNKF
//--------------------------------------------------------------------------//
	Device(LNKG){
                Name(_HID, EISAID("PNP0C0F"))
                Name(_UID, 7)

                Method(_STA,0) {
			And(PGRC, 0x80, Local0)
			If(Local0) {Return (0x9)} // LNKG desabled/present/not visible in UI
			Else   {Return (0xb)} // LNKG enabled
		}

		Method(_PRS,0){Return(PRSG)}

		Method(_DIS){Or(PGRC, 0x80 ,PGRC)} // Disable PIRQ routing  

		Method(_CRS){
			And(PGRC, 0x0f, Local0)
			ShiftLeft(0x1, Local0, IRA0)
			Return(BUFA)
		}					// End Method _CRS

		Method(_SRS, 1){	// ARG0 = PnP Resource String 
			CreateWordField(ARG0, 0x01, IRA)// IRQ Low Mask
			FindSetRightBit(IRA, Local0)
			Decrement(Local0)
			Store(Local0, PGRC)
		}					// End Method _SRS
	} // end Device LNKG
//--------------------------------------------------------------------------//
	Device(LNKH){
                Name(_HID, EISAID("PNP0C0F"))
                Name(_UID, 8)

                Method(_STA,0) {
			And(PHRC, 0x80, Local0)
			If(Local0) {Return (0x9)} // LNKH desabled/present/not visible in UI
			Else   {Return (0xb)} // LNKH enabled
		}

		Method(_PRS,0){Return(PRSH)}

		Method(_DIS){Or(PHRC, 0x80 ,PHRC)} // Disable PIRQ routing  

		Method(_CRS){
			And(PHRC, 0x0f, Local0)
			ShiftLeft(0x1, Local0, IRA0)
			Return(BUFA)
		}					// End Method _CRS

		Method(_SRS, 1){	// ARG0 = PnP Resource String 
			CreateWordField(ARG0, 0x01, IRA)// IRQ Low Mask
			FindSetRightBit(IRA, Local0)
			Decrement(Local0)
			Store(Local0, PHRC)
		}					// End Method _SRS
	} // end Device LNKH
//--------------------------------------------------------------------------//
} // end _SB scope
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
//		DON'T CHANGE ANY CODE UNLESS REQUIRED !!!
//*************************************************************************;
