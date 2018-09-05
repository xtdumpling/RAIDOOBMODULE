//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//       Bug fixed:
//       Reason:      Fixed "Install Windows 7 USB support" function fail with "Quiet boot" disable. 
//       Auditor:     Sunny Yang
//       Date:        Sep/07/2016
//
//*****************************************************************************
// OEM ASL file. Defines ASL Name fields to be linked to AML Exchange bufer location
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
// $Header: /Alaska/BIN/Modules/ACPI/Template/Core/AMLUPD.asl 10    10/14/10 4:59p Oleksiyy $
//
// $Revision: 10 $
//
// $Date: 10/14/10 4:59p $
//
//****************************************************************************
//----------------------------------------------------------------------


//;<AMI_THDR_START>
//;------------------------------------------------------------------------/
//;
//; Name:	BIOS
//;
//; Type: OperationRegion
//;
//; Description:	Set of name objects used as an exchange buffer for BIOS to 
//;					pass the System information to AML code
//; 				List of mandatory Name Objects to be defined:
//;					SS1 - SS4 - BIOS setup enabled sleep states.
//; 				MG1B, MG1L - Non-shadowed memory range below 1Mb. Available for PCI0 bus.
//; 				MG2B, MG2L - Memory range above available System memory. Available for PCI0 bus.
//;
//; Notes: Memory location and all Name values are updated in BIOS POST
//; Referrals: AMLDATA
//;-------------------------------------------------------------------------
//;<AMI_THDR_END>

///////////////////////////////////////////////////////////////////////////////////
//Values are set like this to have ASL compiler reserve enough space for objects 
///////////////////////////////////////////////////////////////////////////////////
// Available Sleep states
Name(SS1,1)		// is S1 enabled in setup?
Name(SS2,0)
Name(SS3,1)
Name(SS4,1)
#if ASL_SMCPKG_SUPPORT 
Name(W7KF,0)
#endif
///////////////////////////////////////////////////////////////////////////////////
// Bit mask of statuses of IO peripheral devices, enabled in Setup
//      0x0 - Serial A (Modem)
//        1 - Serial B (IR)
//        2 - LPT
//        3 - FDD
//----------------------------
//        4 - SB16 Audio
//        5 - Midi
//        6 - Mss Audio
//        7 - Adlib sound (FM synth)
//----------------------------
//        8 - Game 1 port
//        9 - Game 2 port
//        a - KBC 60 & 64
//        b - EC 62 & 66
//----------------------------
//		  c - reserved
//		  d	- reserved
//        e - PS2Mouse
//        f - reserved
///////////////////////////////////////////////////////////////////////////////////
Name(IOST,0xFFFF)		// Bit mask of statuses of IO peripheral devices, 
						// enabled in Setup
///////////////////////////////////////////////////////////////////////////////////
Name(TOPM,0xFFFFF)   	// System memory top address below 4GB, bytes

///////////////////////////////////////////////////////////////////////////////////
Name(ROMS,0xffe00000)	// Boot strap BIOS image address at top 4Gbyte

///////////////////////////////////////////////////////////////////////////////////
Name(VGAF, 0x1)         //Flag to indicate Legacy VGA Resources were clamed already

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
