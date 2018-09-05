//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

// @file IpmiSsif.asl
//
// ASL code for IPMI system interface using SSIF interface.
//

//
// Spmi Scope
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
        // The strings "IPMI_SSIF" is for identifying the SSIF interface.
        //
        Name(_STR, Unicode("IPMI_SSIF"))

        //
        // UID for the primary IPMI system interface in the system
        //
        Name(_UID, 0)

        //
        // Returns the interface type
        //
        Method(_IFT) {
            Return(0x04)    // IPMI SSIF
        }

        //
        // Returns the SSIF slave address
        //
        Method(_ADR) {
            Return(SADR)  // SSIF slave address
        }

        //
        // Returns the interface specification revision
        //
        Method(_SRV) {
            Return(SRVV)  // IPMI Specification Revision
        }

    } // End Device(SPMI)

} // End Scope(SPMI_SCOPE)

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
