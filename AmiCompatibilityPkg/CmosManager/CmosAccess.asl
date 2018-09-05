//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:    CmosAccess.asl
//
// Description:
//      This file contains an ASL method for accessing CMOS from
//      ASL code.
//
//<AMI_FHDR_END>
//*************************************************************************

Device(\CMOS)
{
    Name(_HID, EISAID("PNP0C02"))    // System board resources
    Name(_UID, 0x18)                // Unique ID. 
    
    OperationRegion (SRB0, SystemIO, SRSI, 1)        
    Field (SRB0, ByteAcc, NoLock, Preserve) 
    {
        SRSM,    8 // SW-SMI ctrl port
    }

    // Define a location in root scope to be updated during
    // boot to specify the location of a communication buffer.
    // Search CMOS_SMM_CONTEXT for data size/type info
    // for this operation region (communication buffer).
    // Search gAslContext for additional information regarding
    // initialization and implemenataion details.

    Name(\CCOM, 0x0123456789ABCDEF)  // root scope to allow dynamic updating

    OperationRegion (CMOP, SystemMemory, CCOM, 40)
    Field (CMOP, AnyAcc, NoLock, Preserve)
    {
        CODE,   8,   // 1 byte Opcode
        TOKN,   16,  // 2 byte CMOS Token
        BYTE,   8,   // 1 byte Data value
        STAT,   8    // 1 byte status value
    }

    //--------------------------------------------------------------------
    // ACCE: access (read from/write to) a CMOS location
    //
    // Input:
    //      Arg0 - access type code (0 => write, 1 => read)
    //      Arg1 - CMOS token
    //      Arg2 - CMOS value to be written (not used for writes)
    //
    // Return:
    //      A two-byte buffer is returned with values as follows:
    //
    //      Buffer[0] = status - 0 => fail
    //                           1 => success
    //      Buffer[1] = value that was read (not used for writes)
    //--------------------------------------------------------------------
    Method(ACCE, 3, Serialized){
        Name(BUF1, Buffer(2){0,0})
        CreateByteField(BUF1, 0, STAT)      // Status ... 0 => Fail, 1 => Success
        CreateByteField(BUF1, 1, CVAL)      // Value that was read

        Store(0, STAT)
        Store(0, CVAL)

        // If \CCOM is not updated, there was an error allocating the
        // communication buffer... abort execution with an error.
        if (LEqual(\CCOM, 0x0123456789ABCDEF)) {
            Return(BUF1)
        }

        // Update the communication buffer with the access type, 
        // CMOS token and CMOS value.
        Store(Arg0, ^CODE)
        Store(Arg1, ^TOKN)
        If (LEqual(Arg0, 0)) { // if writing to CMOS
            Store(Arg2, ^BYTE)
        }
        
        // Generate the SMI ...function ID is defined by the CSMI SDL token
        Store(\CSMI, ^SRSM)

        // Update the return buffer
        Store(^STAT, STAT)
        If (LEqual(Arg0, 1)) { // if reading from CMOS
            Store(^BYTE, CVAL)
        }

        Return(BUF1)
    }

    //--------------------------------------------------------------------
    // READ: read from a CMOS location
    //
    // Input:
    //      Arg0 - CMOS token code
    //
    // Return:
    //      A two-byte buffer is returned with values as follows:
    //
    //      Buffer[0] = status - 0 => fail
    //                           1 => success
    //      Buffer[1] = value that was read
    //--------------------------------------------------------------------
    Method(READ, 1, Serialized){
        Return(ACCE(1, Arg0, 0))
    }

    //--------------------------------------------------------------------
    // WRIT: write to a CMOS location
    //
    // Input:
    //      Arg0 - CMOS token code
    //      Arg1 - value to write
    //
    // Return:
    //      A two-byte buffer is returned with values as follows:
    //
    //      Buffer[0] = status - 0 => fail
    //                           1 => success
    //      Buffer[1] = not used for writes
    //--------------------------------------------------------------------
    Method(WRIT, 2, Serialized){
        Return(ACCE(0, Arg0, Arg1))
    }

}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
