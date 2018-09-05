/*************************************************************************\
*  Module Name: iotool.c
*
*  Descriprtion:
*
*  Date:
\**************************************************************************/
#ifndef CLibCombined

#include "typedef.h"
#include "iolib.h"
#include <AmiPeiLib.h>
#include <Library/TimerLib.h>

#endif


VOID SetReg (USHORT usPORT, UCHAR jMask, UCHAR jData)
{
    UCHAR jtemp=jData;  
  
    jtemp = (UCHAR) IoRead8 ((SHORT)usPORT);    
    IoWrite8 ((SHORT)usPORT, ((SHORT)jtemp & (SHORT)jMask) | (SHORT)jData);  	
  
}

VOID SetReg2 (USHORT usPORT, UCHAR jData)
{
  
    IoWrite8 ((SHORT)usPORT, (SHORT)jData);  	
  
}

UCHAR GetReg (USHORT usPORT, UCHAR jMask)
{
     
    return ( ((UCHAR)IoRead8 ((SHORT)usPORT)) & jMask);  	
  
}

UCHAR GetReg2 (USHORT usPORT)
{
     
    return ( ((UCHAR)IoRead8 ((SHORT)usPORT)) );  	
  
}

VOID SetIndexReg (USHORT usPORT, UCHAR jIndex, UCHAR jMask, UCHAR jData)
{
    UCHAR jtemp=jData;  
  
    IoWrite8 ((SHORT)usPORT++, (SHORT)jIndex);
    jtemp = (UCHAR) IoRead8 ((SHORT)usPORT);    
    IoWrite8 ((SHORT)usPORT, ((SHORT)jtemp & (SHORT)jMask) | (SHORT)jData);  	
  
}

VOID SetIndexReg2 (USHORT usPORT, UCHAR jIndex, UCHAR jData)
{
  
    IoWrite8 ((SHORT)usPORT++, (SHORT)jIndex);
    IoWrite8 ((SHORT)usPORT, (SHORT)jData);  	
  
}



UCHAR GetIndexReg (USHORT usPORT, UCHAR jIndex, UCHAR jMask)
{
     
    IoWrite8((SHORT)usPORT++, (SHORT)jIndex);
    return ( ((UCHAR)IoRead8 ((SHORT)usPORT)) & jMask );  	
//    outp ((SHORT)usPORT++, (SHORT)jIndex);
//        
//    return ( ((UCHAR)inp ((SHORT)usPORT)) & jMask );  	
  
}

UCHAR GetIndexReg2 (USHORT usPORT, UCHAR jIndex)
{
     
    IoWrite8((SHORT)usPORT++, (SHORT)jIndex);
    return ( ((UCHAR)IoRead8 ((SHORT)usPORT)) );  	
//    outp ((SHORT)usPORT++, (SHORT)jIndex);
//        
//    return ( ((UCHAR)inp ((SHORT)usPORT)) );  	
  
}

VOID Delay15us (VOID)
{
	
//    USHORT value;
//	
//    value = GetReg (0x61, 0xFF);
//
//    if (value & 0x10)
//        while (GetReg (0x61, 0xFF) & 0x10) ;
//    else
//        while (!(GetReg (0x61, 0xFF) & 0x10)) ;
	MicroSecondDelay(15);
    
}
