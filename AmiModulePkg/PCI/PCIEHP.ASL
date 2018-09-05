//**********************************************************************;
//     *****************************************************************;
//     **                                                             **;
//     **      (C)Copyright 1985-2004, American Megatrends, Inc.      **;
//     **                                                             **;
//     **                     All Rights Reserved.                    **;
//     **                                                             **;
//     **           6145-F Northbelt Pkwy, Norcross, GA 30071         **;
//     **                                                             **;
//     **                     Phone (770)-246-8600                    **;
//     **                                                             **;
//     *****************************************************************;
//-----------------------------------------------------------------------
// $Header: /Alaska/BIN/Modules/ACPI/Template/Core/PCIEHP.ASL 1     5/07/09 2:44p Yakovlevs $
//
// $Revision: 1 $
//
// $Date: 5/07/09 2:44p $
//****************************************************************;
//-----------------------------------------------------------------------
//Constants

Name (SLHC, 0x040)		// Slot Hot plug capable

Name (SPDS, 0x040)		// Slot Presence Detect state
Name (MRLS, 0x020)		// MRL open
Name (CCOM, 0x010)		// Command complete
Name (SPDC, 0x08)		// Slot Presence Detect Changes
Name (MRLC, 0x04)		// Slot MRL changed
Name (SPFD, 0x02)		// Slot power fault Detected
Name (SABP, 0x01)		// Slot attention button pressed

Name (SPOF, 0x400)		// Slot Power off
Name (SPON, 0x3FF)		// Slot Power on Mask


Name (ALMK, 0xFF3F)		// Slot atten. LED Mask
Name (ALON, 0x0040)		// Slot atten. LED on
Name (ALBL, 0x0080)		// Slot atten. LED Blink
Name (ALOF, 0x00C0)		// Slot atten. LED off

Name (PLMK, 0xFCFF)		// Slot Pwr. LED Mask
Name (PLON, 0x100)		// Slot Pwr. LED on
Name (PLBL, 0x200)		// Slot Pwr. LED Blink
Name (PLOF, 0x300)		// Slot Pwr. LED off

Name (HPEV, 0x0F)		// Possible interrupt events (all)


Scope (\_SB.PCI0) {						

	Mutex	(MUTH, 0)

//---------------------------------------
//	Hot plug controller command
//---------------------------------------
	Method (HPCC, 2) {		
// 	Arg0 : Memory mapped Address to PCIe Feature capability Structure
//	ARG1 : Slot Control Register value
		Acquire(MUTH, 0xFFF)
		Store (Rwpe(Add(Arg0, XSCT)), Local0)
		Store (Arg1, Local1)
		And (Local0, 0x7C0, Local0)
		And (Local1, 0x7C0, Local1)
//		If (LNotEqual(Local1, Local0)) { 
			Wbpe(Add(Arg0,XSST), CCOM)  // Clear the command complete status
			Wwpe(Add(Arg0, XSCT), Arg1)
			Store (0, Local0)
			Sleep (2)	  	    // Give time to complete the command
			While (LLess(Local0, 100)){ // Command completed ?
				Sleep (10)	  		
				Add (Local0, 10, Local0)
				if(LEqual(And(Rwpe(Add(Arg0, XSST)), CCOM), CCOM)){ // Command completed ?
					If (LGreaterEqual(Local0, 100)) { 
						Break		// May not work in all OSes
					}
					Store (100, Local0)
				}
			}
			Wbpe(Add(Arg0,XSST), CCOM)  // Clear the command complete status
//		}
//		else {					// Don't wait for command status to update
//			Wwpe(Add(Arg0, XSCT), Arg1)
//		}
		Release(MUTH)
	}						// end of HPCC

//---------------------------------------
//	Attention button Indicator
//---------------------------------------
	Method (ATCM, 2) {
// 	Arg0 : Memory mapped Address to PCIe Feature capability Structure
//	Arg1 : Attention Indicator Value
//		Check if Attention Indicator is present
		Store (Rwpe(Add(Arg0, XSCP)), Local0) // Read Slot Capability Register
		if (And(Local0, 0x08)) { 	      // Attention indicator present
			Store (Rwpe(Add(Arg0, XSCT)), Local0)// Read Slot Control Register
			And (Local0, ALMK, Local0) // Clear Attention indicator control	    
			If (LEqual(Arg1, 0x01)) {  // Attention indicator "ON"?
				Or (Local0, ALON, Local0)
			}				
			If (LEqual(Arg1, 0x02)) {  // Attention indicator "BLINK"?
				Or (Local0, ALBL, Local0)
			}				
			If (LEqual(Arg1, 0x03)) {  // Attention indicator "OFF"?
				Or (Local0, ALOF, Local0)
			}				
			HPCC (Arg0, local0)
		}
	}						// End of ATCM

//---------------------------------------
//	Power Indicator
//---------------------------------------
	Method (PWCM, 2) {
// 	Arg0 : Memory mapped Address to PCIe Feature capability Structure
//	Arg1 : Power Indicator Value
//	Check if Power Indicator is present
		Store (Rwpe(Add(Arg0, XSCP)), Local0) // Read Slot Capability Register
		if (And(Local0, 0x10)) { // Power indicator present
			Store (Rwpe(Add(Arg0, XSCT)), Local0)
			And (Local0, PLMK, Local0)
			If (LEqual(Arg1, 0x01)) {  // Power indicator "ON"?
				Or (Local0, PLON, Local0)
			}				
			If (LEqual(Arg1, 0x02)) {  // Power indicator "BLINK"?
				Or (Local0, PLBL, Local0)
			}				
			If (LEqual(Arg1, 0x03)) {  // Power indicator "OFF"?
				Or (Local0, PLOF, Local0)
			}				
			HPCC (Arg0, local0)
		}
	}						// End of PWCM

//---------------------------------------
//	Slot Power Control
//---------------------------------------
	Method (PWSL, 2) {
// 	Arg0 : Memory mapped Address to PCIe Feature capability Structure
//	Arg1 : Power Controller on/off
//		Check if Power controller is present?
		Store (Rwpe(Add(Arg0, XSCP)), Local0) // Read Slot Capability Register
		if (And(Local0, 0x2)) { // Power controller present
			Store (Rwpe(Add(Arg0, XSCT)), Local0)
			If (LEqual(Arg1, 0x0)) {  // Power slot "OFF"?
				Or (Local0, SPOF, Local0)
			}				
			If (LEqual(Arg1, 0x01)) {  // Power Slot "ON"?
				And (Local0, SPON, Local0)
			}				
			HPCC (Arg0, local0)
		}
	}					// End of PWSL

//---------------------------------------
//	Attention button Interupt Enable/Disable
//---------------------------------------
	Method (ABIE, 2) {
// 	Arg0 : Memory mapped Address to PCIe Feature capability Structure
//	Arg1 : Attention button Interrupt Enable/Disable
//		Check if Attention Button present
		Store (Rwpe(Add(Arg0, XSCP)), Local0) // Read Slot Capability Register
		if (And(Local0, SABP)) { 	      // Attention button present
			Store (Rwpe(Add(Arg0, XSCT)), Local0)
			If (LEqual(Arg1, 0x0)) {  // Disable Attention button interrupt
				And (Local0, 0xFFFE, Local0)
			}				
			If (LEqual(Arg1, 0x01)) {  // Enable Attention button interrupt
				Or (Local0, SABP, Local0)
			}				
			HPCC (Arg0, local0)
		}
	}					// End of ABIE


//---------------------------------------
// 	EJection Main Handler
//---------------------------------------
	Method (EJMH, 1) {			
		Store(0x62, DBG8)		// DBG8 name translates to IO port 80h
// 	Arg0 : Memory mapped Address to PCIe Feature capability Structure
		Store (Rwpe(Add(Arg0, XSCT)), Local0)
		Or (Local0, SPOF, Local0)	// Power COntroller to Power OFF
		Or (Local0, PLOF, Local0)	// Power Indicator to OFF	
		Or (Local0, ALOF, Local0)	// Attention "OFF"	
		HPCC (Arg0, Local0)
	}					// End of EJMH

//---------------------------------------
// 	Hnadler of Hot Plug Event
//---------------------------------------
	Method (HHPE, 1){			
// 	Arg0 : Memory mapped Address to PCIe Feature capability Structure
		Store (RdPe(Arg0), Local0)
		If (Lequal(Local0, 0xFFFFFFFF)) {Return (0xFF)}
		Store (RbPe(Add(Arg0,XSST)), Local0)
		If (And(HPEV, Local0)) { 	// Check for Slot status
			Store (PP4H(Arg0), Local0) // Hot plug interrupt Handler
			Return (Local0)		  // Return PP4H information
		}
		Else {
			Return (0xFF)		// This controller didn't interrupt
		}
	}

	Method (PP4H, 1) {			
// 	Arg0 : Memory mapped Address to PCIe Feature capability Structure 1
		Store (RbPe(Add(Arg0,XSST)), Local0)
		If (And(Local0, SABP)) {			// Attention button pressed? 2
			Wbpe(Add(Arg0,XSST),SABP)		// Clear interrupt status
			If (LEqual(And(Local0, SPDS), 0x00)) {	// Slot empty?	3
				PWSL (Arg0, 0x0)		// Power off
				PWCM (Arg0, 0x3)		// Power indicator off
				ATCM (Arg0, 0x3)		// Attention indicator off
				Return (0x03)			// Eject Request			
			}											// 3
//	Card is present and slot is already powered. User presses attention button to eject the card
			Store (RwPe(Add(Arg0,XSCT)), Local0)
			If (LEqual(And(Local0,SPOF), 0x00)) {	// Slot already powered 4
				ABIE  (Arg0, 0x0) 		// Disable Attention button interrupt
				PWCM (Arg0, 0x02) 		// Set power Indicator to blink	
				Store (0x0, Local0) 		// 5sec counter
				While (LNotEqual(And(Rwpe(Add(Arg0, XSST)), SABP), SABP)){ //Check for Attention button again 5
					Sleep (200) 		// Wiat 200msec
					Add (Local0, 200, Local0)
					If (Lequal (5000, Local0)){			// 6
						ABIE (Arg0, 0x1) // Enable Attention button interrupt
						Return (0x03)  // Continue with Eject Request
					}									// 6
				}										// 5
//	User presses attention button again to abort eject request			
				PWCM (Arg0, 0x01)		// Set power indicator off
				Wbpe (Add(Arg0, XSST), SABP) 	// Clear attention status
				ABIE (Arg0, 1)  		// Enable Attention button interrupt
				Return (0xff)			// Do nothing and abort
			}											// 4
//	Card is present and slot is not powered
//	User presses attention button to indicate card is inserted
			Else {					// Slot power is "OFF". So power up the slot 7
				ABIE (Arg0, 0)  		// Disable Attention button interrupt
				PWCM(Arg0, 0x2)			// Set power indicator to blink
//	Check if user presses attention button again to cancel the insertion
				Store (0x0, Local0) 		// Set 5 sec accumulator to 0
				While (LNotEqual(And(Rwpe(Add(Arg0, XSST)), SABP), SABP)){ //Check for Attention button again 8
					Sleep(200)
					Add(Local0, 200, Local0)
					if (LEqual(5000, Local0)){			// 9
						ABIE (Arg0, 1)   // Enable Attention button interrupt
						ATCM (Arg0, 0x3) // Set attention indicator off	
						PWSL (Arg0, 0x1) // Power the slot		
						Sleep (500) 	 // Wait for .5sec for the power to stabilize
//	Check for Power fault Detection
						If (LNotEqual(And(Rwpe(Add(Arg0, XSST)), SPFD), SPFD)) { // No power fault
							PWCM (Arg0, 0x1) // Set power indicator to "ON"		
							Return (0x0) // Insertion request
						}
						Else {			// Power Fault present 10
							PWCM (Arg0, 0x3) // Set power indicator to OFF
							PWSL (Arg0, 0x0) // Set power off
							ATCM (Arg0, 0x2) // Set attention indicator to Blink
							Return (0x3) 	// Eject Request
						}								// 10
					}									// 9
				}										// 8
//	User presses attention button again, Leave slot unpowered
				Wbpe (Add(Arg0, XSST), SABP) 		// Clear attention status
				ABIE (Arg0, 1) 				// Enable Attention button interrupt
				PWCM (Arg0, 0x3)			// Set Power indicator back to "OFF"
				Return (0xff)				// 
			}						// End if Slot power if "OFF"			7
		}							// End for Attention button Hot plug interrupt 2

		If (And(Rwpe(Add(Arg0, XSST)), SPFD)) {			// Check if power fault detected
			Wbpe (Add(Arg0, XSST), SPFD)			// Clear the power fault Status
			PWCM (Arg0, 0x3)				// Set power indicator to "OFF"
			PWSL (Arg0, 0x0)				// Set power off
			ATCM (Arg0, 0x2)				// Set attention indicator "Blink"
			Return (0x3)					// Eject request
		}							// End for Power Fault Interrupt

		
		If (And(Rwpe(Add(Arg0, XSST)), MRLC)) {			// Check if interrupt caused by the MRL sensor
			Wbpe (Add(Arg0, XSST), MRLC)			// Clear the MRL status
			Return (0x3)					// Eject request
		}

		If (And(Rwpe(Add(Arg0, XSST)), SPDC)) {			// Check if Presence Detect changed status 11
			Wbpe (Add(Arg0, XSST), SPDC)			// Clear Presence Detect Changed status
			If (LEqual(And(Rwpe(Add(Arg0, XSST)), SPDS), 0x00)) { // Check if Slot empty
				PWSL (Arg0, 0x0)			// Set power slot "OFF"
				PWCM (Arg0, 0x3)			// Set power indicator to "OFF"
				Return (0x3)				// Eject Request
			}
			Else {						// Card is present 12
				ABIE (Arg0, 0)				// Attention button Interrupt disable
				PWCM (Arg0, 0x2)			// Set power indicator to blink
				Store (0x0, Local0)			// set 5 sec accumulator to 0
				While (LEqual(And(Rwpe(Add(Arg0, XSST)), SABP), 0x00)){ //Check for Attention button again 13
					Sleep (200)			// wait 200ms
					Add (Local0, 200, Local0)
					If (LEqual(5000, Local0)){							// 14
						ABIE (Arg0, 1)    	// Enable Attention button interrupt
						ATCM (Arg0, 0x3) 	// Set attention indicator "OFF"
						PWSL (Arg0, 0x1) 	// Power the slot
						Sleep (500) 		// wair for 0.5 sec for power to stabilize
						If (LEqual(And(Rwpe(Add(Arg0, XSST)), SPFD), 0x00)) { // No power fault 16
							PWCM (Arg0, 0x1) // Set power indicator to "ON"
							Return (0x0) 	// Notify OS to load the driver
						}									// 16
						Else { 			// Power Fault detected		// 15
							PWCM (Arg0, 0x3) // Set power indicator to "OFF"
							PWSL (Arg0, 0x0) // Set power "OFF"
							ATCM (Arg0, 0x2) // Set attention indicator to "Blink"
							Return (0x3) 	// Eject request
						}				// End of Power Fault 15
					}										// 14
				}											// 13
//	Attention button pressed to abort the process	
				ABIE (Arg0, 1) 				// Enable Attention button interrupt
				PWCM (Arg0, 0x3)			// Set power indicator back to "OFF"
				Return (0xff)				//
			}						// End of Slot power on/off 12
		}							// End of Presence Detect changed Hot plug interrupt 11
		Return (0xff)						// Control should not come here		
	}								// End of PP4H	1
}									// End of \_SB.PCI0

