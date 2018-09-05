//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix: Turn on Early-Video in earlier PEI.
//    Reason : To sync with Grantley
//    Auditor: Yenhsin Cho
//    Date   : May/27/2016
//  
//  Rev. 1.00
//    Bug Fix:  Fix system would hang up at 0x0D when JPG1 set as 2-3.
//    Reason:   
//    Auditor:  Matthew You
//    Date:     May/26/2015
//  
//****************************************************************************

#include "devlib.h"
#include "vgahw.h"
#include <Library/TimerLib.h>

//#if defined (__386__)
//#include "dos32.c"
//#endif
#include "iolib.c"
#include "pcilib.c"

//
// delay
//
VOID udelay(ULONG us)
{
//    ULONG i, count, value;
//	
//    count = (us + 14) / 15;		/* 15us */
//    for (i=0; i<count; i++)
//    {
//        value = GetReg (0x61, 0xFF);
//        if (value & 0x10)
//            while (GetReg (0x61, 0xFF) & 0x10) ;
//        else
//            while (!(GetReg (0x61, 0xFF) & 0x10)) ;	
//    }        
	MicroSecondDelay(us);
}
	
//
// MEM Access
//
ULONG ReadDD_SOC3(_DEVInfo *DevInfo, ULONG addr)
{
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF004) = (ULONG) ((addr) & 0xFFFF0000);
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF000) = (ULONG) (0x00000001);
 return ( *(volatile ULONG *) (DevInfo->pjMMIOLinear + 0x10000 + ((addr) & 0x0000FFFF)) );
}

VOID WriteDD_SOC3(_DEVInfo *DevInfo, ULONG addr, ULONG data)
{						
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF004) = (ULONG) ((addr) & 0xFFFF0000);	
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF000) = (ULONG) 0x00000001;	
 *(ULONG *) (DevInfo->pjMMIOLinear + 0x10000 + ((addr) & 0x0000FFFF)) = data;	
}

USHORT ReadDW_SOC3(_DEVInfo *DevInfo, ULONG addr)
{
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF004) = (ULONG) ((addr) & 0xFFFF0000);
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF000) = (ULONG) (0x00000001);
 return ( *(volatile USHORT *) (DevInfo->pjMMIOLinear + 0x10000 + ((addr) & 0x0000FFFF)) );
}

VOID WriteDW_SOC3(_DEVInfo *DevInfo, ULONG addr, USHORT data)
{						
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF004) = (ULONG) ((addr) & 0xFFFF0000);	
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF000) = (ULONG) 0x00000001;	
 *(USHORT *) (DevInfo->pjMMIOLinear + 0x10000 + ((addr) & 0x0000FFFF)) = data;	
}

UCHAR ReadDB_SOC3(_DEVInfo *DevInfo, ULONG addr)
{
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF004) = (ULONG) ((addr) & 0xFFFF0000);
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF000) = (ULONG) (0x00000001);
 return ( *(volatile UCHAR *) (DevInfo->pjMMIOLinear + 0x10000 + ((addr) & 0x0000FFFF)) );
}

VOID WriteDB_SOC3(_DEVInfo *DevInfo, ULONG addr, UCHAR data)
{						
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF004) = (ULONG) ((addr) & 0xFFFF0000);	
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF000) = (ULONG) 0x00000001;	
 *(UCHAR *) (DevInfo->pjMMIOLinear + 0x10000 + ((addr) & 0x0000FFFF)) = data;	
}

//
// I2C Access
//
VOID SetI2CReg(_DEVInfo *DevInfo, UCHAR jChannel, UCHAR jDevAddr, UCHAR jIndex, UCHAR jData )
{
    ULONG ulI2CRegBase = I2C_BASE + 0x40 * jChannel;
    ULONG retry = 0;
    ULONG ulData;
    
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x00, 0x0);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x04, 0x77743355);
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x04, 0x77776704);    
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x08, 0x0);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x00, 0x1);
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x0C, 0xAF);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x20, (ULONG) jDevAddr);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x03);
    retry = 0;
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
        udelay(15);
        if (retry++ > 1000)
            goto Exit_SetI2CReg;
    } while (!(ulData & 0x01));
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x20, (ULONG) jIndex);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x02);
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
    } while (!(ulData & 0x01)); 
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x20, (ULONG) jData);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x02);
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
    } while (!(ulData & 0x01)); 
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    //ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x0C);    
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x0C, ulData | 0x10);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x20);
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
    } while (!(ulData & 0x10));
    //ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x0C);        
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x0C, ulData & 0xffffffef);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    udelay(10);
    
Exit_SetI2CReg:
    ;
}

UCHAR GetI2CReg(_DEVInfo *DevInfo, UCHAR jChannel, UCHAR jDevAddr, UCHAR jIndex)
{
    ULONG ulI2CRegBase = I2C_BASE + 0x40 * jChannel;
    ULONG retry = 0;
    ULONG ulData;
    UCHAR jData;
    
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x00, 0x0);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x04, 0x77743355);
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x04, 0x77776704);        
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x08, 0x0);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x00, 0x1);
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x0C, 0xAF);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x20, (ULONG) jDevAddr);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x03);
    retry = 0;
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
        udelay(15);
        if (retry++ > 1000)
            return 0;
    } while (!(ulData & 0x01));
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x20, (ULONG) jIndex);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x02);
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
    } while (!(ulData & 0x01));  
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x20, (ULONG) jDevAddr + 1);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x1B);
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
    } while (!(ulData & 0x04));     
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    //ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x0C);    
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x0C, ulData | 0x10);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x20);
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
    } while (!(ulData & 0x10));
    //ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x0C);        
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x0C, ulData & 0xffffffef);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x20);        
    jData = (UCHAR) ((ulData & 0xFF00) >> 8);
    
    return (jData);	
}

//
// DevInfo
//
BOOL GetDevInfo(USHORT usVendorID, USHORT usDeviceID, _DEVInfo *DevInfo)
{
    ULONG PCIAddr;
    
    {
        PCIAddr = FindAstVgaDevice(usVendorID, usDeviceID);
        
        if (!PCIAddr) return 0;

        if (PCIAddr)
        {
    	    DevInfo->PCIAddr = PCIAddr;
            DevInfo->usVendorID = usVendorID;
            DevInfo->usDeviceID = usDeviceID;
            DevInfo->jRevision = (UCHAR) GetPCIReg(PCIAddr+0x08, 0x000000FF);
            
            SetPCIReg (PCIAddr + 0x04, 0xFFFFFFFF, 0x03);	//enable MEM & IO
                        
            DevInfo->usRelocateIOBase = (USHORT) GetPCIReg(PCIAddr+0x18, 0x0000FFFC);
               
            DevInfo->ulMMIOBase = GetPCIReg(PCIAddr+0x14, 0xFFFFFFF0);
            DevInfo->ulMMIOSize = 0x20000;			//default:128KB
//#if defined (__386__)       
//            DevInfo->pjMMIOLinear  = (UCHAR *) MapPhysicalToLinear (DevInfo->ulMMIOBase,DevInfo->ulMMIOSize);
            DevInfo->pjMMIOLinear  = (UCHAR *)DevInfo->ulMMIOBase;
//#endif 
    
//#if defined (__386__)       
//            DevInfo->pjA0000Linear  = (UCHAR *) MapPhysicalToLinear (0xA0000, 0x10000);
//            DevInfo->pjB0000Linear  = (UCHAR *) MapPhysicalToLinear (0xB0000, 0x10000); 
//            DevInfo->pjC0000Linear  = (UCHAR *) MapPhysicalToLinear (0xC0000, 0x10000);                                   
            DevInfo->pjA0000Linear  = (UCHAR *)0xA0000;
            DevInfo->pjB0000Linear  = (UCHAR *)0xB0000; 
            DevInfo->pjC0000Linear  = (UCHAR *)0xC0000;                                   
//#endif  

            //Get default info
            InitVGA_HW(DevInfo, Init_IO);				//enable vga io access
            GetVRAMInfo(DevInfo);
            GetDRAMInfo(DevInfo);
                        
            DevInfo->ulFBBase = GetPCIReg(PCIAddr+0x10, 0xFFFFFFF0);
            //DevInfo->ulFBSize = 8 * 1024 * 1024;		//default:8MB
//#if defined (__386__)       
            DevInfo->pjFBLinear  = (UCHAR *)DevInfo->ulFBBase;
//#endif      
                                
            return 1;
        }
    	
    }  	
    
    return 0;		
}
