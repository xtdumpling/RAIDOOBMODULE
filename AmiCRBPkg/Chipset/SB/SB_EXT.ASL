//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
    OperationRegion(LPCR, PCI_Config, 0x80, 4)
    Field(LPCR, ByteAcc, NoLock, Preserve) {
        CADR, 3,        // COMA Decode Range
        , 1,
        CBDR, 3,        // COMB Decode Range
        , 1,
        LTDR, 2,        // LPT Decode Range
        , 2,
        FDDR, 1,        // FDD Decode Range
        , 3,
        CALE, 1,        // COM Port A Enable
        CBLE, 1,        // COM Port B Enable
        LTLE, 1,        // Parallel Port Enable
        FDLE, 1,        // Floppy Drive Enable
        , 4,
        GLLE, 1,        // Low Game Port Enable
        GHLE, 1,        // High Game Port Enable
        KCLE, 1,        // Keyboard Enable
        MCLE, 1,        // Microcontroller Enable
        C1LE, 1,        // Super I/O Enable 1
        C2LE, 1,        // Super I/O Enable 2
        , 2,
    }
                                        // <(EIP54487)


//;<AMI_PHDR_START>
//;------------------------------------------------------------------------
//;
//; Procedure:    UXDV
//;
//; Description:  Convert UART port address to the decoded value in LPC bridge
//;
//; Input:        Arg0 = Port to Route/Release 
//;
//; Output:       UART Port Decoded Value in the LPC bridge
//;
//;------------------------------------------------------------------------
//;<AMI_PHDR_END>

    Method(UXDV, 1, Serialized) {
        Store(0xff, Local0) // Unknown
        Switch (Add(Arg0, 0)) {
            Case (0x3F8) { Store(0, Local0) }
            Case (0x2F8) { Store(1, Local0) }
            Case (0x220) { Store(2, Local0) }
            Case (0x228) { Store(3, Local0) }
            Case (0x238) { Store(4, Local0) }
            Case (0x2E8) { Store(5, Local0) }
            Case (0x338) { Store(6, Local0) }
            Case (0x3E8) { Store(7, Local0) }
        }
        return (Local0)
    }


//;<AMI_PHDR_START>
//;------------------------------------------------------------------------
//;
//; Procedure:    RRIO
//;
//; Description:  Route/Release I/O resources from/to EIO/LPC Bus 
//;
//; Input:        Arg0 = Device Category
//;               Arg1 = 0/1 Disable/Enable resource decoding
//;               Arg2 = Port to Route/Release
//;               Arg3 = Port SIZE to Route
//;
//; Output:       Nothing
//;
//;------------------------------------------------------------------------
//;<AMI_PHDR_END>
    Method(RRIO, 4, Serialized) {
        Switch (Add(Arg0, 0)) {
            Case (0) {  // UART 0 or 2
                Store(0, CALE) // Disable the decoding
                Store (UXDV(Arg2), Local0)
                If (LNotEqual(Local0, 0xff)) { Store(Local0, CADR)}
                If (Arg1) { Store(1, CALE) }
            }
            Case (1) {  // UART 1 or 3
                Store(0, CBLE) // Disable the decoding
                Store (UXDV(Arg2), Local0)
                If (LNotEqual(Local0, 0xff)) { Store(Local0, CBDR)}
                If (Arg1) { Store(1, CBLE) }
            }
            Case (2) {  // LPT
                Store(0, LTLE) // Disable the decoding
                If (LEqual(Arg2, 0x378)) { Store(0, LTDR) }
                If (LEqual(Arg2, 0x278)) { Store(1, LTDR) }
                If (LEqual(Arg2, 0x3BC)) { Store(2, LTDR) }
                If (Arg1) { Store(1, LTLE) }
            }
            Case (3) {  // FDD
                Store(0, FDLE) // Disable the decoding
                If (LEqual(Arg2, 0x3F0)) { Store(0, FDDR) }
                If (LEqual(Arg2, 0x370)) { Store(1, FDDR) }
                If (Arg1) { Store(1, FDLE) }
            }
            Case (8) {  // Game 1
                If (LEqual(Arg2, 0x200)) {
                    If (Arg1) { Store(1, GLLE) }
                    Else { Store(0, GLLE) }
                }
                If (LEqual(Arg2, 0x208)) {
                    If (Arg1) { Store(1, GHLE) }
                    Else { Store(0, GHLE) }
                }
            }
            Case (9) {  // Game 2
                If (LEqual(Arg2, 0x200)) {
                    If (Arg1) { Store(1, GLLE) }
                    Else { Store(0, GLLE) }
                }
                If (LEqual(Arg2, 0x208)) {
                    If (Arg1) { Store(1, GHLE) }
                    Else { Store(0, GHLE) }
                }
            }
            Case (10) {  // Keyboard Controller
                If (LOr(LEqual(Arg2, 0x60), LEqual(Arg2, 0x64))) {
                    If (Arg1) { Store(1, KCLE) }
                    Else { Store(0, KCLE) }
                }
            }
            Case (11) {  // MicroController
                If (LOr(LEqual(Arg2, 0x62), LEqual(Arg2, 0x66))) {
                    If (Arg1) { Store(1, MCLE) }
                    Else { Store(0, MCLE) }
                }
            }
            Case (12) {  // Super I/O Enable 1
                If (LEqual(Arg2, 0x2E)) {
                    If (Arg1) { Store(1, C1LE) }
                    Else { Store(0, C1LE) }
                }
                If (LEqual(Arg2, 0x4E)) {
                    If (Arg1) { Store(1, C2LE) }
                    Else { Store(0, C2LE) }
                }
            }
            Case (13) {  // Super I/O Enable 2
                If (LEqual(Arg2, 0x2E)) {
                    If (Arg1) { Store(1, C1LE) }
                    Else { Store(0, C1LE) }
                }
                If (LEqual(Arg2, 0x4E)) {
                    If (Arg1) { Store(1, C2LE) }
                    Else { Store(0, C2LE) }
                }
            }

        }
        
    }


//;<AMI_PHDR_START>
//;------------------------------------------------------------------------
//;
//; Procedure:    RDMA
//;
//; Description:  Route/Release DMA channel from/to being ISA/PCI mode
//;
//; Input:        Arg0 = Device Category
//;               Arg1 = 0/1 Disable/Enable resource decoding
//;               Arg2 = DMA channel to Route/Release
//;
//; Output:       Nothing
//;
//;------------------------------------------------------------------------
//;<AMI_PHDR_END>

    Method(RDMA, 3) {
    // Porting If needed.
    }

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
