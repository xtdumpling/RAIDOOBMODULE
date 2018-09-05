//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: BiosGuardNvs.asl
//
// Description: BiosGuardNvs for Tool Interface
//
//<AMI_FHDR_END>
//*************************************************************************

OperationRegion(ABNV,SystemMemory,0xFFFF0000,0xAA55)
Field(ABNV,AnyAcc,Lock,Preserve)
{
    Offset(0),
    //
    // BIOS Guard
    //
    ABMA,  64,    //   (00) Ami BIOS Guard Memory Address for Tool Interface
    ABMS,   8,    //   (08) Ami BIOS Guard Memory Size for Tool Interface
    ABIA,  16,    //   (09) Ami BIOS Guard IoTrap Address for Tool Interface
    ABIL,  16,    //   (11) Ami BIOS Guard IoTrap Length for Tool Interface
    ABRA,  32,    //   (13) Ami BIOS Guard Recovery Address
    ABRS,  16     //   (17) Ami BIOS Guard Recovery Size
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
