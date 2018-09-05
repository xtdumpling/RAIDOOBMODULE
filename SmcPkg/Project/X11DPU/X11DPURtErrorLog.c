//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2017, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//  
//  History
//
//  Rev. 1.00
//    Bug Fix : Add memory error LED function.
//    Reason  : Use CMOS 0x70~0x74 to store memory error flag and error DIMM location.
//    Auditor : Jimmy Chiu
//    Date    : Jul/03/2017
//
//**********************************************************************//
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		RtErrorLogSmc.c
//
// Description:	Runtime error log SMC library function definitions
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "X11DPURtErrorLog.h"
#include <Library/IoLib.h>

VOID WriteCMOS (
  IN UINT8  Offset,
  IN OUT UINT8 Value )
{
    IoWrite8(0x70, Offset);
    IoWrite8(0xEB, Offset);
    IoWrite8(0x71, Value);
}

UINT8 ReadCMOS (
  IN UINT8  Offset)
{
    UINT8 Value;
    IoWrite8(0x70, Offset);
    IoWrite8(0xEB, Offset);
    Value = IoRead8(0x71);
    return Value;
}	

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HandleMemErrLED
//
// Description: 
//	Turn on/off memory error LED when memory error occur
//
// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
HandleMemErrLED (	
  AMI_MEMORY_ERROR_SECTION *MemInfo
 )
{
    EFI_STATUS Status = EFI_SUCCESS;
    
    EFI_SMBUS_HC_PROTOCOL* Smbus;
    EFI_SMBUS_DEVICE_ADDRESS	SlaveAddress;
    UINTN Len = 1;
    UINT8 tempData[4];
    UINT8 PCA9539_ADDR1 = 0x75;
    UINT8 PCA9539_ADDR2 = 0x76;
    UINT32 ErrDimm, ErrDimmMap;	
    UINT8 DimmNum, Socket, Channel;
    IMC_DIMM_NUM IMC_DIMM = CPU1_DIMM_NUM;
    
    Status = gSmst->SmmLocateProtocol (&gEfiSmcSmmSmbusProtocolGuid, NULL, &Smbus);
    if (EFI_ERROR (Status)) {
        return Status;
    }

    WriteCMOS(CMOS_MEM_ERR_FLAG_0, 0xAA); //Set memory error flag
    WriteCMOS(CMOS_MEM_ERR_FLAG_1, 0x55); //Set memory error flag
    
    Socket = (UINT8)(NODE_TO_SKT(MemInfo->Node));
    Channel = (UINT8)(NODE_TO_MC(MemInfo->Node)) * 3 + (UINT8)(MemInfo->Card);
    DimmNum = (UINT8)(MemInfo->Module);
    
    ErrDimm = Channel * (IMC_DIMM.DIMM_NUM[0]) + DimmNum;
    ErrDimmMap = 0x00000000;
    
    if(Socket == 0)
        ErrDimmMap = 0x00000001;
    else if(Socket == 1)
        ErrDimmMap = 0x00010000;
    
    if(ErrDimm != 0)
        ErrDimmMap = ErrDimmMap << ErrDimm;  
    
    //Get error DIMM information first
    tempData[0] = ReadCMOS(CMOS_ERR_DIMM_LOC0); //Get memory error DIMM location 0
    tempData[1] = ReadCMOS(CMOS_ERR_DIMM_LOC1); //Get memory error DIMM location 1
    tempData[2] = ReadCMOS(CMOS_ERR_DIMM_LOC2); //Get memory error DIMM location 2
    tempData[3] = ReadCMOS(CMOS_ERR_DIMM_LOC3); //Get memory error DIMM location 3
	
    //Set error DIMM location of this time, keep previous one
    tempData[0] |= (UINT8)(ErrDimmMap & 0x0000003F); //CPU1 CH A&B&C
    tempData[1] |= (UINT8)((ErrDimmMap & 0x00000FC0) >> 6); //CPU1 CH D&E&F
    tempData[2] |= (UINT8)((ErrDimmMap & 0x003F0000) >> 16); //CPU2 A&B&C
    tempData[3] |= (UINT8)((ErrDimmMap & 0x0FC00000) >> 22); //CPU2 CH D&E&F

    WriteCMOS(CMOS_ERR_DIMM_LOC0, tempData[0]); //Set memory error DIMM location 0
    WriteCMOS(CMOS_ERR_DIMM_LOC1, tempData[1]); //Set memory error DIMM location 1
    WriteCMOS(CMOS_ERR_DIMM_LOC2, tempData[2]); //Set memory error DIMM location 2
    WriteCMOS(CMOS_ERR_DIMM_LOC3, tempData[3]); //Set memory error DIMM location 3

    SlaveAddress.SmbusDeviceAddress = PCA9539_ADDR1;
    Status = Smbus->Execute(Smbus, SlaveAddress, 2, EfiSmbusWriteByte, 0, &Len, &tempData[0]);
    Status = Smbus->Execute(Smbus, SlaveAddress, 3, EfiSmbusWriteByte, 0, &Len, &tempData[1]);
    		
    SlaveAddress.SmbusDeviceAddress = PCA9539_ADDR2;
    Status = Smbus->Execute(Smbus, SlaveAddress, 2, EfiSmbusWriteByte, 0, &Len, &tempData[2]);
    Status = Smbus->Execute(Smbus, SlaveAddress, 3, EfiSmbusWriteByte, 0, &Len, &tempData[3]);

    return Status;
}	

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	SmcClrMemErrCmosFlag
//
// Description:	Clear memory error flag after flash BIOS.
//
// Input:
//      VOID
//
// Output:
//      VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID SmcClrMemErrCmosFlag (VOID)
{
    WriteCMOS(CMOS_MEM_ERR_FLAG_0, 0x55); //Reset memory error flag
    WriteCMOS(CMOS_MEM_ERR_FLAG_1, 0xAA); //Reset memory error flag
}
