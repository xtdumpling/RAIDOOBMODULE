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
/** @file Pilot4PeiInit.c
  Porting for PEI phase.Just for necessary devices porting.
**/

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <Token.h>
#include <AmiPeiLib.h>
#include <Library/AmiSioPeiLib.h>

extern IO_DECODE_DATA Pilot4PeiDecodeTable[];
extern UINT8 Pilot4PeiDecodeTableCount;
extern SIO_DEVICE_INIT_DATA Pilot4PeiInitTable[];
extern UINT8 Pilot4PeiInitTableCount;
//----------------------------------------------------------------------
// internal function declare; these functions are only used by this file.
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// global function declare ; these functions are used for other files.
//----------------------------------------------------------------------
//**********************************************************************
// below are functions defined
//**********************************************************************
/**
  This function provide PEI phase SIO initialization

  @param  FileHandle     Handle of the file being invoked.
  @param  PeiServices   Describes the list of possible PEI Services.

  @retval  EFI_SUCESS       The entry point of Pilot4PeiInit executes successfully.
  @retval  Others           Some error occurs during the execution of this function. 
**/
EFI_STATUS Pilot4PeiInitEntryPoint(
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    UINT8 index;

    for(index=0; index<Pilot4PeiDecodeTableCount; index++)
        AmiSioLibSetLpcDeviceDecoding(NULL, Pilot4PeiDecodeTable[index].BaseAdd, Pilot4PeiDecodeTable[index].UID, Pilot4PeiDecodeTable[index].Type);

    ProgramRtRegisterTable(0, Pilot4PeiInitTable, Pilot4PeiInitTableCount);

    return EFI_SUCCESS;
}

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

