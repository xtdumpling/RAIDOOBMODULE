//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Register/LocalApic.h>

#include <Library/LocalApicLib.h>

VOID
EFIAPI
WriteLocalApicReg (
  IN UINTN  MmioOffset,
  IN UINT32 Value
  );

UINT32
EFIAPI
ReadLocalApicReg (
  IN UINTN  MmioOffset
  );

/**
  Dummy function for Backward compatibility

  Initialize the state of the SoftwareEnable bit in the Local APIC
  Spurious Interrupt Vector register.

  @param  Enable  If TRUE, then set SoftwareEnable to 1
                  If FALSE, then set SoftwareEnable to 0.

**/
VOID
EFIAPI
InitializeLocalApicSoftwareEnable (
  IN BOOLEAN  Enable
  ) {
    LOCAL_APIC_SVR  Svr;

    //
    // Set local APIC software-enabled bit.
    //
    Svr.Uint32 = ReadLocalApicReg (XAPIC_SPURIOUS_VECTOR_OFFSET);
    if (Enable) {
      if (Svr.Bits.SoftwareEnable == 0) {
        Svr.Bits.SoftwareEnable = 1;
        WriteLocalApicReg (XAPIC_SPURIOUS_VECTOR_OFFSET, Svr.Uint32);
      }
    } else {
      if (Svr.Bits.SoftwareEnable == 1) {
        Svr.Bits.SoftwareEnable = 0;
        WriteLocalApicReg (XAPIC_SPURIOUS_VECTOR_OFFSET, Svr.Uint32);
      }
    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
