//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//  Rev. 1.02
//       Bug fixed:   Fixed issue ACPI status does not be changed by IPMI when boot to Redhat Server 6.6 .
//       Reason:      
//       Auditor:     Chen Lin
//       Date:        Mar/31/2016
//
//  Rev. 1.01
//       Bug fixed:   Only disable IPMI device for XP and 2003
//       Reason:      Improve 1.00 to avoid newer OS _OSI not in the list.
//       Auditor:     Stephen Chen
//       Date:        Mar/16/2016
//
//  Rev. 1.00
//       Bug fixed:   Only enable IPMI device for Vista or later OS
//       Reason:      
//       Auditor:     Stephen Chen
//       Date:        Jan/13/2014
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

// @file IpmiKcs.asl
//
// ASL code for IPMI system interface using KCS interface.
//

//
// SPMI Scope
//
#include "TokenAsl.h"	// SMCPKG_SUPPORT

Scope(SPMI_SCOPE) {
    //
    // SPMI Device
    //
    Device(SPMI) {
        //
        // HID for the IPMI system interface in the system
        //
        Name(_HID, EISAID("IPI0001"))

        //
        // Optional, but recommended for identifying IPMI system interface.
        // The strings "IPMI_KCS" is for identifying the KCS interface.
        //
        Name(_STR, Unicode("IPMI_KCS"))

        //
        // UID for the primary IPMI system interface in the system
        //
        Name(_UID, 0)

        //
        // Operation region block
        //
        OperationRegion (IPST, SystemIO, \ICDP, 1)  //IPMI KCS status port
        Field (IPST, ByteAcc,NoLock,Preserve)
        { STAS,8,}

        //
        // _STA method definition
        //
        Method(_STA,0) { //Get IPMI status
            Store(STAS, Local0)
            If( LEqual(Local0, 0xFF) ) {  //KCS not decoded, Device not present.
                Return(0x0)
            } Else {    //Device present & Active
#if ASL_SMCPKG_SUPPORT
 
               If(LGreaterEqual(OSFL(),12)) { // Win Vista or later
                   Return(0x0F)
               }
               Else {
                   Return(0x0)
               }
                 
#else
                Return (0x0F)
#endif
            }
        } // End Method(_STA,0)

        //
        // Resource Template
        //
        Name(ICRS, ResourceTemplate() {
            IO(Decode16, 0, 0, 0, 0, BAS0)
            IO(Decode16, 0, 0, 0, 0, BAS1)
        })

        //
        // Get IPMI Current Resources
        //
        Method(_CRS,0) {
            if ( IDTP ) {
                CreateWordField(ICRS, ^BAS0._MIN, IPDB)
                CreateWordField(ICRS, ^BAS0._MAX, IPDH)
                CreateByteField(ICRS, ^BAS0._LEN, IPDL)
                Store(\IDTP, IPDB)  // Min Base address
                Store(\IDTP, IPDH)  // Max Base address
                Store(1, IPDL)  // Region length
            } 
            if (ICDP) {
                CreateWordField(ICRS, ^BAS1._MIN, IPCB)
                CreateWordField(ICRS, ^BAS1._MAX, IPCH)
                CreateByteField(ICRS, ^BAS1._LEN, IPCL)
                Store(\ICDP, IPCB)  // Min Base address
                Store(\ICDP, IPCH)  // Max Base address
                Store(1, IPCL)  // Region length
            }

            //
            // Return current resource
            //
            Return(ICRS)
        } // End Method(_CRS,0)

        //
        // Returns the interface type
        //
        Method(_IFT) {
            Return(0x01)    // IPMI KCS
        }

        //
        // Returns the interface specification revision
        //
        Method(_SRV) {
            Return(SRVV)  // IPMI Specification Revision
        }

        //
        //This interface does not support interrupt
        //

    } // End Device(SPMI)

} // End Scope(SPMI_SCOPE)

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
