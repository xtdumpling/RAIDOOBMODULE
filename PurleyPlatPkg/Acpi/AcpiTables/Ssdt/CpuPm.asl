//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++
  
Copyright 1996-2016, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.

  Module Name:
  
    CpuPm.asl
  
  Abstract:
  
    Contains root level name space objects for Cpu 0..F with GV3 (Intel Speedstep Technology) support
  
NOTES:-
 This Processor Power Management BIOS Source Code is furnished under license
 and may only be used or copied in accordance with the terms of the license.
 The information in this document is furnished for informational use only, is
 subject to change without notice, and should not be construed as a commitment
 by Intel Corporation. Intel Corporation assumes no responsibility or liability
 for any errors or inaccuracies that may appear in this document or any
 software that may be provided in association with this document.

 Except as permitted by such license, no part of this document may be
 reproduced, stored in a retrieval system, or transmitted in any form or by
 any means without the express written consent of Intel Corporation.

 WARNING: You are authorized and licensed to install and use this BIOS code
 ONLY on an IST PC. This utility may damage any system that does not
 meet these requirements.

  An IST PC is a computer which
  (1) Is capable of seamlessly and automatically transitioning among
      multiple performance states (potentially operating at different
      efficiency ratings) based upon power source changes, end user
      preference, processor performance demand, and thermal conditions; and
  (2) Includes an Intel Pentium II processors, Intel Pentium III
      processor, Mobile Intel Pentium III Processor-M, Mobile Intel Pentium 4
      Processor-M, Intel Pentium M Processor, or any other future Intel 
      processors that incorporates the capability to transition between 
      different performance states by altering some, or any combination of, 
      the following processor attributes: core voltage, core frequency, bus 
      frequency, number of processor cores available, or any other attribute 
      that changes the efficiency (instructions/unit time-power) at which the 
      processor operates.

--*/

// Note: 
// These tables are static as defined here. But they are patched during
// POST from UpdateTables () function in AcpiPlatformHooks.c  to make 
// them dynamic and reflect the user configuration and capabilities for 
// particular system boot and OS session

DefinitionBlock (
  "CPUPM.aml",      // Output Filename
  "SSDT",           // Signature
  2,                // SSDT Revision
  "INTEL",          // OEMID
  "SSDT  PM",       // Table ID
  0x4000            // OEM Revision
  ) {

  External (PSEN, FieldUnitObj)
  External (HWAL, FieldUnitObj)
  External (TSEN, FieldUnitObj)
  External (FGTS, FieldUnitObj)
  External (CSEN, FieldUnitObj)
  External (HWEN, FieldUnitObj) //HWPM

#if SPS_SUPPORT
  //
  // SPS Node Manager defines current P-State & T-State limit
  // Objects PSLM and TSLM are referenced in _PPC and _TPC
  // PETE is a bitmask of P-states T-states support enabled in OS.
  //
  Name(PETE, 0)
  Name(PSLM, Package(){0,0,0,0,0,0,0,0}) // P-State limit per socket
  Name(TSLM, Package(){0,0,0,0,0,0,0,0}) // T-State limit per socket
#endif

    Name(HI1,0)    // Handle to EIST
    Name(HW1,0)    // Handle to HWP

    Name(SDTL,0x00)        // Loaded SSDT Flags.
    // SDTL[0]   = EIST SSDT Loaded
    // SDTL[1]   = HWP SSDT Loaded
    // SDTL[2]   = TST SSDT Loaded
    // SDTL[3]   = CST SSDT Loaded

  #undef   ToName
  #define  ToName(x)  x##00
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##01
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##02
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##03
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##04
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##05
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##06
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##07
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##08
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##09
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##10
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##11
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##12
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##13
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##14
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##15
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##16
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##17
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##18
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##19
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##20
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##21
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##22
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##23
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##24
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##25
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##26
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##27
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##28
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##29
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##2A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##2B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##2C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##2D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##2E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##2F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##30
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##31
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##32
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##33
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##34
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##35
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##36
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##37
  #include "ProcPm.asi"

}

