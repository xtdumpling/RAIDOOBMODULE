//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file 
 * SioStaticSetup.c
 * Sub Module Strings override routines to dynamically change Setup.
**/ 
//**********************************************************************
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <AutoId.h>
#include <AmiGenericSio.h>
#include <SioSetup.h>
#if SIO_SUPPORT
#include "SIOElink.h"

STR_UID STR_TABLE_LIST[]= {SIO_Init_Str_LIST {dsNone, 0x00, 0x00}};
#else
STR_UID STR_TABLE_LIST[]= {{dsNone, 0x00, 0x00}};
#endif

//**********************************************************************
/**
        
    Certain strings in the terminal setup page can only be
    updated at runtime, which is what this function does.
    The pci serial port names are changed to contain the
    location (device and function numbers) of the port.
    If any serial ports are disabled, "Disabled"
    is concatenated to their names.

    @param HiiHandle 
    @param Class 

    @retval VOID


    @retval 

**/
//**********************************************************************
VOID EFIAPI SioSetupStringsInit(EFI_HII_HANDLE HiiHandle, UINT16 Class)
{
    static EFI_GUID    siopg=EFI_AMI_SIO_PROTOCOL_GUID;
    SIO_DEV2    *dev;
    UINTN        i,hcnt;
    EFI_HANDLE    *hbuff;
    EFI_STATUS    Status=0;
//------------------------------------
//DEBUG    
//EFI_DEADLOOP();    
//DEBUG    

    if(Class == ADVANCED_FORM_SET_CLASS) {
        Status=pBS->LocateHandleBuffer(ByProtocol,&siopg, NULL, &hcnt,&hbuff);
        if (EFI_ERROR(Status)) hcnt = 0;
        for (i=0; i<hcnt; i++) {
            Status = pBS->HandleProtocol ( hbuff[i],&siopg,(VOID*)&dev);
            ASSERT_EFI_ERROR(Status);
            if(dev->DeviceInfo->HasSetup) SetSioStrings(dev, HiiHandle);
        }
    }
}

/**
    This function provide SIO's each logic device Setup screen display string.

        
    @param Dev SPIO Device Private Data section
    @param HiiHandle 

    @retval VOID

**/
VOID EFIAPI SetSioStrings(
    VOID            *DevVoid,
    EFI_HII_HANDLE  HiiHandle)
{
    UINT8       i;
    CHAR16      pString16[80];
    SIO_DEV2    *Dev = (SIO_DEV2 *)DevVoid;

    if((!Dev->NvData.DevEnable) || (!Dev->VlData.DevImplemented)) return;

    if(Dev->VlData.DevDma1) {
        if(Dev->VlData.DevIrq1)
        	Swprintf(pString16, L"IO=%Xh; IRQ=%d; DMA=%d;", Dev->VlData.DevBase1,Dev->VlData.DevIrq1,Dev->VlData.DevDma1);
        else
        	Swprintf(pString16, L"IO=%Xh; DMA=%d;",Dev->VlData.DevBase1, Dev->VlData.DevDma1);
    }
    else {
        if(Dev->VlData.DevIrq1)
        	Swprintf(pString16, L"IO=%Xh; IRQ=%d;",Dev->VlData.DevBase1,Dev->VlData.DevIrq1);
        else
            Swprintf(pString16, L"IO=%Xh;",Dev->VlData.DevBase1);
    }

    for(i=0; i<(sizeof(STR_TABLE_LIST)/sizeof(STR_UID)); i++) {
        if((STR_TABLE_LIST[i].UID == Dev->DeviceInfo->Uid) && \
           (STR_TABLE_LIST[i].Type == Dev->DeviceInfo->Type)) {
            InitString(HiiHandle,STR_TABLE_LIST[i].StrTokenV,pString16);
            return;
        }

    }
    return;
}

/**
    This function Adjust the String shows in setup screen

        
    @param Value >Value need change to string
    @param Flags >Comma type
    @param Width >Character number for value
    @param Buffer >Temp string for this change
	

    @retval String Length

**/
UINTN EFIAPI
HHMEfiValueToString (
    IN  OUT CHAR16  *Buffer,
    IN  INT64       Value,
    IN  UINTN       Flags,
    IN  UINTN       Width
) {
    CHAR16      TempBuffer[CHARACTER_NUMBER_FOR_VALUE];
    CHAR16      *TempStr;
    CHAR16      *BufferPtr;
    UINTN       Count;
    UINTN       ValueCharNum;
    UINTN       Remainder;
    CHAR16      Prefix;
    UINTN       Index;
    BOOLEAN     ValueIsNegative;

    TempStr           = TempBuffer;
    BufferPtr         = Buffer;
    Count             = 0;
    ValueCharNum      = 0;
    ValueIsNegative   = FALSE;

    if (Width > CHARACTER_NUMBER_FOR_VALUE - 1) {
        Width = CHARACTER_NUMBER_FOR_VALUE - 1;
    }

    if (Value < 0) {
        *(TempStr++) = '-';
        Value        = -Value;
        ValueCharNum++;
        Count++;
        ValueIsNegative = TRUE;
    }
    do {
        if ((Width != 0) && (Count >= Width)) break;

        Value = (UINT64)Div64 ((UINT64)Value, 10, &Remainder);
        *(TempStr++) = (CHAR16)(Remainder + '0');
        ValueCharNum++;
        Count++;

        if ((Flags & COMMA_TYPE) == COMMA_TYPE) {
            if (Value != 0) {
                if ((ValueIsNegative && (ValueCharNum % 3 == 1)) || \
                    ((!ValueIsNegative) && (ValueCharNum % 3 == 0))) {
                    *(TempStr++) = ',';
                    Count++;
                }
            }
        }
    } while (Value != 0);

    if (Flags & PREFIX_ZERO) Prefix = '0';
    else Prefix = ' ';

    Index = Count;

    if (!(Flags & LEFT_JUSTIFY)) {
        for (; Index < Width; Index++)
            *(TempStr++) = Prefix;
    }
    //
    // Reverse temp string into Buffer.
    //
    if (Width == 0) {
        while (TempStr != TempBuffer)
            *(BufferPtr++) = *(--TempStr);
    }
    else {
        Index = 0;
        while ((TempStr != TempBuffer) && (Index++ < Width))
        *(BufferPtr++) = *(--TempStr);
    }
    *BufferPtr = 0;

    return Index;
}


/**
    Adjust string with float point

        
    @param Buffer > defautl string.  ": N/A    ". Buffer length should more than 0x10
    @param StringToChged > Raw data
    @param STCLen > String total length
    @param HeadBuf > Header of string
    @param HeadLen > Header length
    @param Flag > TRUE indicate is a float data, False indicate it's a integet data.
    @param MidPos > Float point position. eg: 0.076 is 0x3
    @param TailBuf > unit of string data
    @param TailLen > Length of unit

    @retval Return string in Buffer

**/
VOID EFIAPI AdjustString(
    IN OUT  CHAR16  * Buffer,
    IN      CHAR16  * StringToChged,
    IN      UINT8   STCLen,
    IN      CHAR16  * HeadBuf,
    IN      UINT8   HeadLen,
    IN      BOOLEAN Flag,
    IN      UINT8   MidPos,
    IN      CHAR16  * TailBuf,
    IN      UINT8   TailLen
) {
    CHAR16     *temp        = Buffer;
    CHAR16     *TempSrc     = StringToChged;
    CHAR16     dod[]        ={L"."};
    CHAR16     Zero[]       ={L"0"};

    pBS->SetMem(temp, STR_BUFFER_LENGTH * sizeof(CHAR16), 0);

    if(HeadLen) {
        //Add the leading string
        pBS->CopyMem(temp, HeadBuf, (HeadLen * sizeof(CHAR16)));
        temp +=  HeadLen;
    }
    if (!Flag) {
        //Add the float point->L"."
        pBS->CopyMem(temp, TempSrc, (STCLen * sizeof(CHAR16))); //Add the string before float point
        temp += STCLen; TempSrc += STCLen;
        goto not_float_data;
    }

    if(STCLen <= MidPos) {
        //make up with a zero
        pBS->CopyMem(temp, Zero, (0x01 * sizeof(CHAR16)));      //Copy a 0
        temp++;
    } else {
        pBS->CopyMem(temp, TempSrc, ((STCLen - MidPos) * sizeof(CHAR16))); //Add the string before float point
        temp += (STCLen - MidPos); TempSrc += (STCLen - MidPos);
    }
    pBS->CopyMem(temp, dod, 0x01 * sizeof(CHAR16));             //Add the float point->L"."
    temp++;
    if(STCLen < MidPos) {
        //make up with a zero
        pBS->CopyMem(temp, Zero, ((MidPos - STCLen) * sizeof(CHAR16)));//Copy a 0
        pBS->CopyMem(temp, TempSrc, ((STCLen) * sizeof(CHAR16)));    //Add the string after float point
        temp += MidPos; TempSrc += MidPos;
    } else {
        pBS->CopyMem(temp, TempSrc, ((MidPos) * sizeof(CHAR16)));    //Add the string after float point
        temp += MidPos; TempSrc += MidPos;
    }
    not_float_data:
    if (TailLen) {
        //Add the unit
        pBS->CopyMem(temp, TailBuf, (TailLen * sizeof(CHAR16)));
    }
    return;
}


/**
    Update option with the data read from register

        
    @param RegData data from SIO registers
    @param Func Fan Speed,Voltage and Temperature
    @param StrToken String token
    @param OddPos Odd position
    @param hiiHandle HII Handle

    @retval 
        Return string in Buffer

**/
VOID EFIAPI HHMCommon(
    IN      UINT64     RegData,
    IN      UINT8     Func,
    IN      UINT16    StrToken,
    IN      UINT8     OddPos,
    IN      EFI_HII_HANDLE    hiiHandle
) {
    CHAR16          TailRound[]     = L" RPM";          //Fan uint
    CHAR16          TailVoltg[]     = L" V";            //Voltage uint
//    CHAR16          TailTempt[]     = L" C";            //Temperature uint
    CHAR16          TailTempt[]     = L" \x2103";            //Temperature uint
    CHAR16          LeadingMini[]   = L": -";           //Fix
    CHAR16          LeadingPlus[]   = L": +";           //Fix
    CHAR16          LeadingSpac[]   = L": ";            //Fix
    CHAR16          AllSpace[]      = L": N/A       ";  //Fix
    CHAR16          *TempStr        = AllSpace;
    UINT8           StrLen;
    INT64           NData;
    BOOLEAN         PN_Flag=0;  //Positive(0) or Negative(1) value
    CHAR16          StrUp[STR_BUFFER_LENGTH] = L": N/A     "; //Don't change this line
    CHAR16          *StrUpdated = StrUp;

    NData = (INT64)(RegData);
    //add negative value handle
    if(NData<0) {
	   TRACE((-1,"NData=%d\n",NData));
       NData=-NData;
       PN_Flag=1;
	   TRACE((-1,"NData=%d\n",NData));
    }
    //The following may be ported by each SIO
    //As now, it support max length is five
    if(NData>9999)      StrLen = 0x5;
    else if(NData>999)  StrLen = 0x4;
    else if(NData>99)   StrLen = 0x3;
    else if(NData>9)    StrLen = 0x2;
    else                StrLen = 0x1;

    //When device not present, update to 'N/A'
    if(NData == 0x00) StrUpdated = StrUp;
    else {
        HHMEfiValueToString(TempStr, NData, 0, StrLen);
        switch(Func) {
            case    VOLTAGE:                       //Indicate it's voltage
                if(PN_Flag) {
                    AdjustString(StrUpdated, TempStr, StrLen, LeadingMini, 0x03,\
                                OddPos?TRUE:FALSE, OddPos, TailVoltg, 0x02);
                }else {
                    AdjustString(StrUpdated, TempStr, StrLen, LeadingPlus, 0x03,\
                                OddPos?TRUE:FALSE, OddPos, TailVoltg, 0x02);
                }
                break;
            case    TEMPERATURE:                   //Indicate it's Temperature
 //               if(PN_Flag) {
 //                   AdjustString(StrUpdated, TempStr, StrLen, LeadingMini, 0x03,\
 //                               OddPos?TRUE:FALSE, OddPos, TailTempt, 0x02);
 //               }else {
                    AdjustString(StrUpdated, TempStr, StrLen, LeadingPlus, 0x03,\
                                OddPos?TRUE:FALSE, OddPos, TailTempt, 0x02);
 //               }
                break;
            case    FAN_SPEED:                     //Indicate it's fan
                AdjustString(StrUpdated, TempStr, StrLen, LeadingSpac, 0x02,\
                            OddPos?TRUE:FALSE, OddPos, TailRound, 0x04);
                break;
            default :                              //Default to " N/A "
                break;
        }
    }
    InitString(hiiHandle, StrToken, StrUpdated);

    return;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
