/** @file
  ACPI SSDT table

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

DefinitionBlock (
    "UsbPortXhciNcCrb.aml",
    "SSDT",
    0x02, // SHAD revision.
        // A Revision field value greater than or equal to 2 signifies that integers 
        // declared within the Definition Block are to be evaluated as 64-bit values
    "INTEL", // OEM ID (6 byte string)
    "xh_nccrb", // OEM table ID  (8 byte string)
    0x0 // OEM version of DSDT table (4 byte Integer)
    )
{

  External(\_SB.PC00.XHCI.RHUB, DeviceObj)

  External(\_SB.PC00.XHCI.RHUB.HS01, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS02, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS03, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS04, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS05, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS06, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS07, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS08, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS09, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS10, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS11, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS12, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS13, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS14, DeviceObj)
  
  External(\_SB.PC00.XHCI.RHUB.SS01, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS02, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS03, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS04, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS05, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS06, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS07, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS08, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS09, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS10, DeviceObj)
  
  External(\_SB.PC00.XHCI.RHUB.USR1, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.USR2, DeviceObj)



  Scope (\_SB.PC00.XHCI.RHUB) {



    //
    // Method for creating generic _PLD buffers
    // _PLD contains lots of data, but for purpose of internal validation we care only about
    // ports' visibility and pairing (this requires group position)
    // so these are the only 2 configurable parameters (User Visible, Group Position)
    //
    Method(GPLD, 2, Serialized) {
      Name(PCKG, Package() { Buffer(0x10) {} } )
      CreateField(DerefOf(Index(PCKG,0)), 0, 7, REV)
      Store(1,REV)
      CreateField(DerefOf(Index(PCKG,0)), 64, 1, VISI)
      Store(Arg0, VISI)
      CreateField(DerefOf(Index(PCKG,0)), 87, 8, GPOS)
      Store(Arg1, GPOS)


      return (PCKG)
    }

    //
    // Method for creating generic _UPC buffers
    // Similar to _PLD, for internal testing we only care about 1 parameter (port connectable)
    //
    Method(GUPC, 1, Serialized) {
      Name(PCKG, Package(4) { 0, 0xFF, 0, 0 } )
      Store(Arg0,Index(PCKG,0))


      return (PCKG)
    }



  } //end scope RHUB

  Scope (\_SB.PC00.XHCI.RHUB.HS01) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,1)) } //Rear Panel A [CONN27] - Upper - USB2 port
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS02) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,2)) } //Rear Panel A [CONN27] - Center - USB2 port
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS03) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,3)) } //Rear Panel A [CONN27] - Bottom - USB2 port
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS04) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,4)) } //Internal Type A 1 [CONN9] - Right - USB2 port
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS05) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,5)) } //Front Panel A [CONN20K] - Right - USB2 port
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS06) {
    Method(_UPC) { Return (GUPC(0)) } //X11SPM
    Method(_PLD) { Return (GPLD(0,0)) } //X11SPM
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS07) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) } //Internal Type A 3 [CONN4] - ? - USB2 port - routed to BMC by default
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS08) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) } //Internal Type A 3 [CONN4] - ? - USB2 port - routed to BMC by default
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS09) {
    Method(_UPC) { Return (GUPC(0)) } //X11SPM
    Method(_PLD) { Return (GPLD(0,0)) } //X11SPM
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS10) {
    Method(_UPC) { Return (GUPC(0)) } //X11SPM
    Method(_PLD) { Return (GPLD(0,0)) } //X11SPM
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS11) {
    Method(_UPC) { Return (GUPC(0)) } //X11SPM
    Method(_PLD) { Return (GPLD(0,0)) } //X11SPM
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS12) {
    Method(_UPC) { Return (GUPC(0)) } //X11SPM
    Method(_PLD) { Return (GPLD(0,0)) } //X11SPM
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS13) {
    Method(_UPC) { Return (GUPC(0)) } //X11SPM
    Method(_PLD) { Return (GPLD(0,0)) } //X11SPM
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS14) {
    Method(_UPC) { Return (GUPC(0)) } //X11SPM
    Method(_PLD) { Return (GPLD(0,0)) } //X11SPM
  }

  Scope (\_SB.PC00.XHCI.RHUB.USR1) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) }
  }

  Scope (\_SB.PC00.XHCI.RHUB.USR2) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) }
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS01) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,1)) } //Rear Panel A [CONN27] - Upper - USB3 port
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS02) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,2)) } //Rear Panel A [CONN27] - Center - USB3 port
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS03) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,3)) } //Rear Panel A [CONN27] - Bottom - USB3 port
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS04) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,4)) } //Internal Type A 1 [CONN9] - Right - USB3 port
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS05) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,5)) } //Front Panel A [CONN20K] - Right - USB3 port
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS06) {
    Method(_UPC) { Return (GUPC(0)) } //X11SPM
    Method(_PLD) { Return (GPLD(0,0)) } //X11SPM
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS07) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) } //N/A
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS08) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) } //N/A
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS09) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) } //N/A
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS10) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) } //N/A
  }
}
