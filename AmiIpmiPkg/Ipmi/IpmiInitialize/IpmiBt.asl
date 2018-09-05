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


// @file IpmiBt.asl
//
// ASL code for IPMI system interface using BT interface
//

//
// SPMI Scope
//
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
        // The strings "IPMI_BT" is for identifying the BT interface.
        //
        Name(_STR, Unicode("IPMI_BT"))

        //
        // UID for the primary IPMI system interface in the system
        //
        Name(_UID, 0)

        //
        // Resource Template
        //
        Name(ICRS, ResourceTemplate() {
            IO(Decode16, 0, 0, 0, 0, BAS0)
            IO(Decode16, 0, 0, 0, 0, BAS1)
            IO(Decode16, 0, 0, 0, 0, BAS2)
        })

        //
        // Get IPMI Current Resources
        //
        Method(_CRS,0){
            if (IPBT) {
                CreateWordField(ICRS, ^BAS0._MIN, BTCB)
                CreateWordField(ICRS, ^BAS0._MAX, BTCH)
                CreateByteField(ICRS, ^BAS0._LEN, CRDL)
                Store(\IPBT, BTCB)    // Min Base address
                Store(\IPBT, BTCH)    // Max Base address
                Store(1, CRDL)    // Region length
            }
            if (IBUF) {
                CreateWordField(ICRS, ^BAS1._MIN, BUFH)
                CreateWordField(ICRS, ^BAS1._MAX, BUFL)
                CreateByteField(ICRS, ^BAS1._LEN, BTBL)
                Store(\IBUF, BUFH)    // Min Base address
                Store(\IBUF, BUFL)    // Max Base address
                Store(1, BTBL)    // Region length
            }
            if (BTIM) {
                CreateWordField(ICRS, ^BAS2._MIN, IMBH)
                CreateWordField(ICRS, ^BAS2._MAX, IMBL)
                CreateByteField(ICRS, ^BAS2._LEN, BTIL)
                Store(\BTIM, IMBH)    // Min Base address
                Store(\BTIM, IMBL)    // Max Base address
                Store(1, BTIL)    // Region length
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
            Return(0x03) // IPMI BT
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
