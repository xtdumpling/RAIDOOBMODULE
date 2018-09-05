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
/** @file AST2500PlatformHookSerialPortLib.c
  The file initialize the serial port for PlatformHookLib.
**/

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <token.h>
#include <Library/AmiSioPeiLib.h>

typedef struct _LPC_AHB_DATA{
    UINT8           Offset;
    UINT32           And;
    UINT32           Or;
} LPC_AHB_DATA;

extern IO_DECODE_DATA AST2500SerialPortDebugDecodeTable[];
extern UINT8 AST2500SerialPortDebugDecodeTableCount;
extern SIO_DEVICE_INIT_DATA AST2500SerialPortDebugInitTable[];
extern UINT8 AST2500SerialPortDebugInitTableCount;
extern LPC_AHB_DATA AST2500_SUC_Access_For_Debug_Table[];
extern UINT8 AST2500_SUC_Access_For_Debug_Table_Count;

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
    
/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not intiailzie the serial port hardware itself.  Instead, it initializes
  hardware devices that are required for the CPU to access the serial port
  hardware.  This function may be called more than once.

  @retval  RETURN_SUCCESS    The platform specific initialization succeeded.
  @retval  Others            The platform specific initialization could not be completed.
**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (VOID)
{
    UINT8 index;
#if SMCPKG_SUPPORT == 0
    UINTN           i;
    UINT32          address;
    UINT32          Value;
#endif    
    //1. Decode
    for(index=0; index<AST2500SerialPortDebugDecodeTableCount; index++)
        AmiSioLibSetLpcDeviceDecoding(NULL, \
                                      AST2500SerialPortDebugDecodeTable[index].BaseAdd, \
                                      AST2500SerialPortDebugDecodeTable[index].UID, \
                                      AST2500SerialPortDebugDecodeTable[index].Type);
    //2. Enable IO
    //3. Set Serial port address
    ProgramRtRegisterTable(0x00, AST2500SerialPortDebugInitTable, AST2500SerialPortDebugInitTableCount);
#if SMCPKG_SUPPORT == 0
    // initialize SUC part.
    for(i=0; i< AST2500_SUC_Access_For_Debug_Table_Count; i++) {
        address = (UINT32)(AST2500_SUCBASE | (AST2500_SUC_Access_For_Debug_Table[i].Offset));
        lpc_read(address , &Value);
        Value &= (AST2500_SUC_Access_For_Debug_Table[i].And);
        Value |= (AST2500_SUC_Access_For_Debug_Table[i].Or);
        lpc_write(address,Value);
    }
#endif    
    return  RETURN_SUCCESS;
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

