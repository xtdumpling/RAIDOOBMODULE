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

//
// @file BmcAcpi.asl
//
// ASL code to Handle/Set Bmc power states.
//

//
// Spmi Device Scope
//
Scope(SPMI_SCOPE.SPMI) {
    //
    // Operation region block
    //
    OperationRegion (IOB2, SystemIO, 0xB2, 1)   //MKF_SMIPORT
    Field (IOB2, ByteAcc, NoLock, Preserve) {
        SMIC,   8,  // SW-SMI ctrl port
    }

    //
    // _INI method definition
    //
    Method(_INI,0) {
        Store(BSMI, SMIC)   // SW SMI
    }

    //
    // BMCS method definition. This method is called from _PTS
    //
    Method (BMCS,1) {
        Add(Arg0, BSMI, SMIC)   // SW SMI
    }
    
    //
    // BMCW method definition. This method is called from _WAK
    //
    Method (BMCW,0) {
        Store(BSMI, SMIC)   // SW SMI
    } 

} // End Scope(SPMI_SCOPE.SPMI)

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
