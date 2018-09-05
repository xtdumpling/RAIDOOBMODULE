//*********************************************************************
//*********************************************************************
//**                                                                 **
//**       (C)Copyright 1993-2015 Supermicro Computer, Inc.          **
//**                                                                 **
//*********************************************************************
//*********************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Skip SUC settings to avoid system hang in eSPI mode.
//      Reason:     
//      Auditor:    Jacker Yeh
//      Date:       Mar/10/2016
//
//**********************************************************************
//
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
/** @file AST2500PeiInit.c
  Porting for PEI phase.Just for necessary devices porting.
**/

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <token.h>
#include <AmiPeiLib.h>
#include <Library/AmiSioPeiLib.h>

//----------------------------------------------------------------------
// Define Structs
//----------------------------------------------------------------------
typedef struct _LPC_AHB_DATA{
    UINT8           Offset;
    UINT32           And;
    UINT32           Or;
} LPC_AHB_DATA;

extern IO_DECODE_DATA AST2500PeiDecodeTable[];
extern UINT8 AST2500PeiDecodeTableCount;
extern SIO_DEVICE_INIT_DATA AST2500PeiInitTable[];
extern UINT8 AST2500PeiInitTableCount;
extern LPC_AHB_DATA AST2500_SUC_access_Table[];
extern UINT8 AST2500_SUC_access_Table_Count;
//-------------------------------------------------------------------------
// global funtion declare ; these funtions are used for other files.
//-------------------------------------------------------------------------
//*************************************************************************
// belows are functions defined
    VOID lpc_read(
        IN    UINT32        Address,
        OUT   UINT32         *Value
    );

    VOID lpc_write(
        IN    UINT32        Address,
        IN    UINT32         Vaule
    );

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

  @retval  EFI_SUCESS       The entry point of AST2500PeiInit executes successfully.
  @retval  Others           Some error occurs during the execution of this function. 
**/
EFI_STATUS AST2500PeiInitEntryPoint(
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    UINT8           index;
#if SMCPKG_SUPPORT == 0
    UINTN           i;
    UINT32          address;
    UINT32          Value;
#endif
    for(index=0; index<AST2500PeiDecodeTableCount; index++)
        AmiSioLibSetLpcDeviceDecoding(NULL, AST2500PeiDecodeTable[index].BaseAdd, AST2500PeiDecodeTable[index].UID, AST2500PeiDecodeTable[index].Type);

    ProgramRtRegisterTable(0, AST2500PeiInitTable, AST2500PeiInitTableCount);
    
    // initialize SUC part.
#if SMCPKG_SUPPORT == 0
    for(i=0; i< AST2500_SUC_access_Table_Count; i++) {
        address = (UINT32)(AST2500_SUCBASE | (AST2500_SUC_access_Table[i].Offset));
        lpc_read(address , &Value);
        Value &= (AST2500_SUC_access_Table[i].And);
        Value |= (AST2500_SUC_access_Table[i].Or);
        lpc_write(address,Value);
    }
#endif
    return EFI_SUCCESS;
}

//VOID lpc_read (ULONG ulAddress, BYTE jmode)
VOID lpc_read (
        IN      UINT32 ulAddress,
        OUT     UINT32  *Value)
{
//    ULONG    uldata = 0, ultemp = 0;
    UINT32     uldata = 0;
    UINT8     jtemp;

    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_ENTER_VALUE);
    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_ENTER_VALUE);

    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_LDN_SEL_REGISTER);
    IoWrite8 (AST2500_CONFIG_DATA, AST2500_LDN_ILPC2AHB);

    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_ACTIVATE_REGISTER);
    jtemp = IoRead8 (AST2500_CONFIG_DATA); //
    IoWrite8 (AST2500_CONFIG_DATA, (jtemp | 0x01));//Active
//Write Address
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF0);
    IoWrite8 (AST2500_CONFIG_DATA, ((ulAddress & 0xFF000000) >> 24));
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF1);
    IoWrite8 (AST2500_CONFIG_DATA, ((ulAddress & 0x00FF0000) >> 16));
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF2);
    IoWrite8 (AST2500_CONFIG_DATA, ((ulAddress & 0x0000FF00) >> 8));
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF3);
    IoWrite8 (AST2500_CONFIG_DATA, (ulAddress & 0x000000ff));
//Write Mode
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF8);
    jtemp = IoRead8 (AST2500_CONFIG_DATA);
    IoWrite8 (AST2500_CONFIG_DATA, (jtemp & 0xFC) | 0x02);
//Fire
    IoWrite8 (AST2500_CONFIG_INDEX, 0xFE);
    jtemp = IoRead8 (AST2500_CONFIG_DATA);
//Get Data
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF4);
    uldata |= (IoRead8 (AST2500_CONFIG_DATA) << 24);
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF5);
    uldata |= (IoRead8 (AST2500_CONFIG_DATA) << 16);
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF6);
    uldata |= (IoRead8 (AST2500_CONFIG_DATA) << 8);
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF7);
    uldata |= (IoRead8 (AST2500_CONFIG_DATA));
    
    *Value = uldata;

    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_EXIT_VALUE);
}

//VOID lpc_write (ULONG ulAddress, ULONG uldata, BYTE jmode)
VOID lpc_write (
        IN      UINT32  ulAddress,
        IN      UINT32   uldata)
{
    UINT8     jtemp;
    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_ENTER_VALUE);
    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_ENTER_VALUE);

    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_LDN_SEL_REGISTER);
    IoWrite8 (AST2500_CONFIG_DATA, AST2500_LDN_ILPC2AHB);

    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_ACTIVATE_REGISTER);
    jtemp = IoRead8 (AST2500_CONFIG_DATA); //
    IoWrite8 (AST2500_CONFIG_DATA, (jtemp | 0x01));//Active
//Write Address
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF0);
    IoWrite8 (AST2500_CONFIG_DATA, ((ulAddress & 0xff000000) >> 24));
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF1);
    IoWrite8 (AST2500_CONFIG_DATA, ((ulAddress & 0x00ff0000) >> 16));
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF2);
    IoWrite8 (AST2500_CONFIG_DATA, ((ulAddress & 0x0000ff00) >> 8));
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF3);
    IoWrite8 (AST2500_CONFIG_DATA, (ulAddress & 0x000000ff));
//Write Data
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF4);
    IoWrite8 (AST2500_CONFIG_DATA, ((uldata & 0xff000000) >> 24));
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF5);
    IoWrite8 (AST2500_CONFIG_DATA, ((uldata & 0x00ff0000) >> 16));
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF6);
    IoWrite8 (AST2500_CONFIG_DATA, ((uldata & 0x0000ff00) >> 8));
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF7);
    IoWrite8 (AST2500_CONFIG_DATA, (uldata & 0x000000ff));
//Write Mode
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF8);
    jtemp = IoRead8 (AST2500_CONFIG_DATA);
    IoWrite8 (AST2500_CONFIG_DATA, (jtemp & 0xFC) | 0x02);
//Fire
    IoWrite8 (AST2500_CONFIG_INDEX, 0xFE);
    IoWrite8 (AST2500_CONFIG_DATA, 0xCF);

    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_EXIT_VALUE);
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

