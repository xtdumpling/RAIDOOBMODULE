#include "devlib.h"
#include "vgahw.h"

/* init VGA */
ast_ExtRegInfo astExtReg[] = {
    {0xA0, 0x0F},
    {0xA1, 0x04},
    {0xA2, 0x1C},
    {0xB6, 0x04},
    {0xB7, 0x00},
    {0xFF, 0xFF},
};

ast_ExtRegInfo astExtReg_AST2300[] = {
    {0xA0, 0x0F},
    {0xA1, 0x04},
    {0xA2, 0x1F},
    {0xB6, 0x24},
    {0xB7, 0x00},
    {0xFF, 0xFF},
};

/*
 * common API
 */
void enable_vga(_DEVInfo *DevInfo)
{
    if (!(GetReg2(VGA_ENABLE) & 0x01))
    {
        SetReg2(VGA_ENABLE, 0x01);
        SetReg2(MISC_PORT_WRITE, 0xE3);
    }
}

void open_key(_DEVInfo *DevInfo)
{
   SetIndexReg2(CRTC_PORT,0x80, 0xA8);
}

void DisplayOFF(_DEVInfo *DevInfo)
{   	
   
   SetIndexReg(SEQ_PORT,0x01, 0xDF, 0x20);		/* screen off */   
   
}

void DisplayON(_DEVInfo *DevInfo)
{   	
   
   SetIndexReg(SEQ_PORT,0x01, 0xDF, 0x00);		/* screen on */   
   
}

//void SetDPMSOFF(_DEVInfo *DevInfo)
//{   	
//   open_key(DevInfo);
//   SetIndexReg(CRTC_PORT,0xB6, 0xFC, 0x03);		/* dpms off */      
//}

//void SetDPMSON(_DEVInfo *DevInfo)
//{   	 
//   open_key(DevInfo);	  
//   SetIndexReg(CRTC_PORT,0xB6, 0xFC, 0x00);		/* dpms on */   
//}

/* Init DRAM */
__inline ULONG MIndwm(UCHAR *mmiobase, ULONG r)
{
    *(ULONG *) (mmiobase + 0xF004) = r & 0xFFFF0000;
    *(ULONG *) (mmiobase + 0xF000) = 0x1;

    return ( *(volatile ULONG *) (mmiobase + 0x10000 + (r & 0x0000FFFF)) );

}

__inline void MOutdwm(UCHAR *mmiobase, ULONG r, ULONG v)
{

    *(ULONG *) (mmiobase + 0xF004) = r & 0xFFFF0000;
    *(ULONG *) (mmiobase + 0xF000) = 0x1;

    *(volatile ULONG *) (mmiobase + 0x10000 + (r & 0x0000FFFF)) = v;
}

/*
 * AST2100/2150 DLL CBR Setting
 */
#define CBR_SIZE_AST2150             ((16 << 10) - 1)
#define CBR_PASSNUM_AST2150          5
#define CBR_THRESHOLD_AST2150        10
#define CBR_THRESHOLD2_AST2150       10
#define TIMEOUT_AST2150              5000000

#define CBR_PATNUM_AST2150           8

ULONG pattern_AST2150[14] ={
0xFF00FF00,
0xCC33CC33,
0xAA55AA55,
0xFFFE0001,
0x683501FE,
0x0F1929B0,
0x2D0B4346,
0x60767F02,
0x6FBE36A6,
0x3A253035,
0x3019686D,
0x41C6167E,
0x620152BF,
0x20F050E0};

typedef struct _AST2150DRAMParam {
    UCHAR	*pjMMIOVirtualAddress;
} AST2150DRAMParam, *PAST2150DRAMParam;

ULONG MMCTestBurst2_AST2150(PAST2150DRAMParam  param, ULONG datagen)
{
  ULONG data, timeout;
  UCHAR *mmiobase;

  mmiobase = param->pjMMIOVirtualAddress;

  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000001 | (datagen << 3));
  timeout = 0;
  do{
    data = MIndwm(mmiobase, 0x1E6E0070) & 0x40;
    if(++timeout > TIMEOUT_AST2150){
      MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
      return(-1);
    }
  }while(!data);
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000003 | (datagen << 3));
  timeout = 0;
  do{
    data = MIndwm(mmiobase, 0x1E6E0070) & 0x40;
    if(++timeout > TIMEOUT_AST2150){
      MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
      return(-1);
    }
  }while(!data);
  data = (MIndwm(mmiobase, 0x1E6E0070) & 0x80) >> 7;
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  return(data);
}

ULONG MMCTestSingle2_AST2150(PAST2150DRAMParam  param, ULONG datagen)
{
  ULONG data, timeout;
  UCHAR *mmiobase;

  mmiobase = param->pjMMIOVirtualAddress;

  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000005 | (datagen << 3));
  timeout = 0;
  do{
    data = MIndwm(mmiobase, 0x1E6E0070) & 0x40;
    if(++timeout > TIMEOUT_AST2150){
      MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
      return(-1);
    }
  }while(!data);
  data = (MIndwm(mmiobase, 0x1E6E0070) & 0x80) >> 7;
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  return(data);
}

int CBRTest_AST2150(PAST2150DRAMParam  param)
{
  UCHAR *mmiobase;

  mmiobase = param->pjMMIOVirtualAddress;

  if(MMCTestBurst2_AST2150(param, 0) ) return(0);
  if(MMCTestBurst2_AST2150(param, 1) ) return(0);
  if(MMCTestBurst2_AST2150(param, 2) ) return(0);
  if(MMCTestBurst2_AST2150(param, 3) ) return(0);
  if(MMCTestBurst2_AST2150(param, 4) ) return(0);
  if(MMCTestBurst2_AST2150(param, 5) ) return(0);
  if(MMCTestBurst2_AST2150(param, 6) ) return(0);
  if(MMCTestBurst2_AST2150(param, 7) ) return(0);
  return(1);

}

int CBRScan_AST2150(PAST2150DRAMParam  param, int busw)
{
  ULONG patcnt, loop;
  UCHAR *mmiobase;

  mmiobase = param->pjMMIOVirtualAddress;

  for(patcnt = 0;patcnt < CBR_PATNUM_AST2150;patcnt++){
    MOutdwm(mmiobase, 0x1E6E007C, pattern_AST2150[patcnt]);
    for(loop = 0;loop < CBR_PASSNUM_AST2150;loop++){
      if(CBRTest_AST2150(param)){
        break;
      }
    }
    if(loop == CBR_PASSNUM_AST2150){
      return(0);
    }
  }
  return(1);

}

void CBRDLLI_AST2150(PAST2150DRAMParam  param, int busw)
{
  ULONG dllmin[4], dllmax[4], dlli, data, passcnt;
  UCHAR *mmiobase;

  mmiobase = param->pjMMIOVirtualAddress;

  CBR_START:
  dllmin[0] = dllmin[1] = dllmin[2] = dllmin[3] = 0xff;
  dllmax[0] = dllmax[1] = dllmax[2] = dllmax[3] = 0x0;
  passcnt = 0;
  MOutdwm(mmiobase, 0x1E6E0074, CBR_SIZE_AST2150);
  for(dlli = 0;dlli < 100;dlli++){
    MOutdwm(mmiobase, 0x1E6E0068, dlli | (dlli << 8) | (dlli << 16) | (dlli << 24));
    data = CBRScan_AST2150(param, busw);
    if(data != 0){
      if(data & 0x1){
        if(dllmin[0] > dlli){
          dllmin[0] = dlli;
        }
        if(dllmax[0] < dlli){
          dllmax[0] = dlli;
        }
      }
      passcnt++;
    }else if(passcnt >= CBR_THRESHOLD_AST2150){
      break;
    }
  }
  if(dllmax[0] == 0 || (dllmax[0]-dllmin[0]) < CBR_THRESHOLD_AST2150){
    goto CBR_START;
  }
  dlli = dllmin[0] + (((dllmax[0] - dllmin[0]) * 7) >> 4);
  MOutdwm(mmiobase, 0x1E6E0068, dlli | (dlli << 8) | (dlli << 16) | (dlli << 24));
}

typedef struct _AST_DRAMStruct {

    USHORT 	Index;
    ULONG	Data;

} AST_DRAMStruct, *PAST_DRAMStruct;

AST_DRAMStruct AST2000DRAMTableData[] = {
    { 0x0108, 0x00000000 },
    { 0x0120, 0x00004a21 },
    { 0xFF00, 0x00000043 },
    { 0x0000, 0xFFFFFFFF },
    { 0x0004, 0x00000089 },
    { 0x0008, 0x22331353 },
    { 0x000C, 0x0d07000b },
    { 0x0010, 0x11113333 },
    { 0x0020, 0x00110350 },
    { 0x0028, 0x1e0828f0 },
    { 0x0024, 0x00000001 },
    { 0x001C, 0x00000000 },
    { 0x0014, 0x00000003 },
    { 0xFF00, 0x00000043 },
    { 0x0018, 0x00000131 },
    { 0x0014, 0x00000001 },
    { 0xFF00, 0x00000043 },
    { 0x0018, 0x00000031 },
    { 0x0014, 0x00000001 },
    { 0xFF00, 0x00000043 },
    { 0x0028, 0x1e0828f1 },
    { 0x0024, 0x00000003 },
    { 0x002C, 0x1f0f28fb },
    { 0x0030, 0xFFFFFE01 },
    { 0xFFFF, 0xFFFFFFFF }
};

AST_DRAMStruct AST1100DRAMTableData[] = {
    { 0x2000, 0x1688a8a8 },
    { 0x2020, 0x000041f0 },
    { 0xFF00, 0x00000043 },
    { 0x0000, 0xfc600309 },
    { 0x006C, 0x00909090 },
    { 0x0064, 0x00050000 },
    { 0x0004, 0x00000585 },
    { 0x0008, 0x0011030f },
    { 0x0010, 0x22201724 },
    { 0x0018, 0x1e29011a },
    { 0x0020, 0x00c82222 },
    { 0x0014, 0x01001523 },
    { 0x001C, 0x1024010d },
    { 0x0024, 0x00cb2522 },
    { 0x0038, 0xffffff82 },
    { 0x003C, 0x00000000 },
    { 0x0040, 0x00000000 },
    { 0x0044, 0x00000000 },
    { 0x0048, 0x00000000 },
    { 0x004C, 0x00000000 },
    { 0x0050, 0x00000000 },
    { 0x0054, 0x00000000 },
    { 0x0058, 0x00000000 },
    { 0x005C, 0x00000000 },
    { 0x0060, 0x032aa02a },
    { 0x0064, 0x002d3000 },
    { 0x0068, 0x00000000 },
    { 0x0070, 0x00000000 },
    { 0x0074, 0x00000000 },
    { 0x0078, 0x00000000 },
    { 0x007C, 0x00000000 },
    { 0x0034, 0x00000001 },
    { 0xFF00, 0x00000043 },
    { 0x002C, 0x00000732 },
    { 0x0030, 0x00000040 },
    { 0x0028, 0x00000005 },
    { 0x0028, 0x00000007 },
    { 0x0028, 0x00000003 },
    { 0x0028, 0x00000001 },
    { 0x000C, 0x00005a08 },
    { 0x002C, 0x00000632 },
    { 0x0028, 0x00000001 },
    { 0x0030, 0x000003c0 },
    { 0x0028, 0x00000003 },
    { 0x0030, 0x00000040 },
    { 0x0028, 0x00000003 },
    { 0x000C, 0x00005a21 },
    { 0x0034, 0x00007c03 },
    { 0x0120, 0x00004c41 },
    { 0xffff, 0xffffffff },
};

AST_DRAMStruct AST2100DRAMTableData[] = {
    { 0x2000, 0x1688a8a8 },
    { 0x2020, 0x00004120 },
    { 0xFF00, 0x00000043 },
    { 0x0000, 0xfc600309 },
    { 0x006C, 0x00909090 },
    { 0x0064, 0x00070000 },
    { 0x0004, 0x00000489 },
    { 0x0008, 0x0011030f },
    { 0x0010, 0x32302926 },
    { 0x0018, 0x274c0122 },
    { 0x0020, 0x00ce2222 },
    { 0x0014, 0x01001523 },
    { 0x001C, 0x1024010d },
    { 0x0024, 0x00cb2522 },
    { 0x0038, 0xffffff82 },
    { 0x003C, 0x00000000 },
    { 0x0040, 0x00000000 },
    { 0x0044, 0x00000000 },
    { 0x0048, 0x00000000 },
    { 0x004C, 0x00000000 },
    { 0x0050, 0x00000000 },
    { 0x0054, 0x00000000 },
    { 0x0058, 0x00000000 },
    { 0x005C, 0x00000000 },
    { 0x0060, 0x0f2aa02a },
    { 0x0064, 0x003f3005 },
    { 0x0068, 0x02020202 },
    { 0x0070, 0x00000000 },
    { 0x0074, 0x00000000 },
    { 0x0078, 0x00000000 },
    { 0x007C, 0x00000000 },
    { 0x0034, 0x00000001 },
    { 0xFF00, 0x00000043 },
    { 0x002C, 0x00000942 },
    { 0x0030, 0x00000040 },
    { 0x0028, 0x00000005 },
    { 0x0028, 0x00000007 },
    { 0x0028, 0x00000003 },
    { 0x0028, 0x00000001 },
    { 0x000C, 0x00005a08 },
    { 0x002C, 0x00000842 },
    { 0x0028, 0x00000001 },
    { 0x0030, 0x000003c0 },
    { 0x0028, 0x00000003 },
    { 0x0030, 0x00000040 },
    { 0x0028, 0x00000003 },
    { 0x000C, 0x00005a21 },
    { 0x0034, 0x00007c03 },
    { 0x0120, 0x00005061 },
    { 0xffff, 0xffffffff },
};

void vInitDRAMReg(_DEVInfo *DevInfo)
{
    AST_DRAMStruct *pjDRAMRegInfo;
    AST2150DRAMParam param;
    ULONG i, ulTemp, ulData;
    UCHAR jReg;

    jReg = GetIndexReg(CRTC_PORT, 0xD0, 0xFF);
    if ((jReg & 0x80) == 0)			/* VGA only */
    {

    	if (DevInfo->jRevision >= 0x20)	/* AST2100/1100 */
    	{
    	    //if (dinfo->chipset == AST2100)
            //    pjDRAMRegInfo = AST2100DRAMTableData;
    	    //else
                pjDRAMRegInfo = AST1100DRAMTableData;

            MOutdwm(DevInfo->pjMMIOLinear, 0x1e6e2000, 0x1688a8a8);
            do {
                ulTemp = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e2000);
            } while (ulTemp != 0x01);

            MOutdwm(DevInfo->pjMMIOLinear, 0x1e6e0000, 0xfc600309);
            do {
                ulTemp = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e0000);
            } while (ulTemp != 0x01);
        }
    	else /* AST2000 */
    	{
            pjDRAMRegInfo = AST2000DRAMTableData;

            MOutdwm(DevInfo->pjMMIOLinear, 0x1e6e0100, 0xa8);
            do {
                ulTemp = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e0100);
            } while (ulTemp != 0xa8);

        }

        while (pjDRAMRegInfo->Index != 0xFFFF)
        {
            if (pjDRAMRegInfo->Index == 0xFF00)			/* Delay function */
            {
            	for (i=0; i<15; i++)
        	    udelay(pjDRAMRegInfo->Data);
            }
            else if ( (pjDRAMRegInfo->Index == 0x0004) && (DevInfo->jRevision >= 0x20) )
            {
            	ulData = pjDRAMRegInfo->Data;

            	if (DevInfo->jDRAMType == DRAMTYPE_1Gx16)
            	    ulData = 0x00000d89;
            	else if (DevInfo->jDRAMType == DRAMTYPE_1Gx32)
            	    ulData = 0x00000c8d;

                ulTemp  = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e2070);
                ulTemp &= 0x0000000C;
                ulTemp <<= 2;
                MOutdwm(DevInfo->pjMMIOLinear, 0x1e6e0004, ulData | ulTemp);
            }
            else
            {
                MOutdwm(DevInfo->pjMMIOLinear, 0x1e6e0000 + pjDRAMRegInfo->Index, pjDRAMRegInfo->Data);
            }

            pjDRAMRegInfo++;
        }

        /* AST2100/2150 DRAM Calibration, ycchen@021511 */
        ulData  = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e0120);
        if (ulData == 0x5061)		/* 266MHz */
        {
            param.pjMMIOVirtualAddress = DevInfo->pjMMIOLinear;
            ulData  = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e0004);
            if (ulData & 0x40)
                CBRDLLI_AST2150(&param, 16);		/* 16bits */
            else
                CBRDLLI_AST2150(&param, 32);            /* 32bits */
        }

    	if (DevInfo->jRevision >= 0x10)	/* AST2100/1100 */
        {
            ulData  = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e2040);
            ulData |= 0xC0;
            MOutdwm(DevInfo->pjMMIOLinear, 0x1e6e2040, ulData);
        }
        else
        {
            ulData  = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e0140);
            ulData |= 0xC0;
            MOutdwm(DevInfo->pjMMIOLinear, 0x1e6e0140, ulData);
        }

    } /* Init DRAM */

    /* wait ready */
    do {
        jReg = GetIndexReg(CRTC_PORT, 0xD0, 0xFF);
    } while ((jReg & 0x40) == 0);

} /* vInitDRAMReg */

/*
 * AST2300 DRAM settings modules
 */
#define	DDR3		0
#define	DDR2		1

typedef struct _AST2300DRAMParam {
    UCHAR	*pjMMIOVirtualAddress;
    ULONG	DRAM_Type;
    ULONG	DRAM_ChipID;
    ULONG	DRAM_Freq;
    ULONG   VRAM_Size;
    ULONG	ENABLE_ODT;				/* 0/75/150 */
    ULONG	ENABLE_WODT;			/* 0/40/60/120 */
    ULONG	ENABLE_RODT;

    ULONG	DRAM_CONFIG;
    ULONG	REG_PERIOD;
    ULONG  	REG_MADJ;
    ULONG	REG_SADJ;
    ULONG	REG_MRS;
    ULONG	REG_EMRS;
    ULONG	REG_AC1;
    ULONG	REG_AC2;
    ULONG	REG_tRFC;    
    ULONG	REG_DQSIC;
    ULONG	REG_DRV;
    ULONG	REG_IOZ;
    ULONG	REG_DQIDLY;
    ULONG	REG_FREQ;
    ULONG   MADJ_MAX;
    ULONG   DLL2_FINETUNE_STEP;
} AST2300DRAMParam, *PAST2300DRAMParam;

/*
 * DQSI DLL CBR Setting
 */
#define CBR_SIZE0            ((1  << 10) - 1)
#define CBR_SIZE1            ((4  << 10) - 1)
#define CBR_SIZE2            ((64 << 10) - 1)
#define CBR_PASSNUM          5
#define CBR_PASSNUM2         5
#define CBR_THRESHOLD        10
#define CBR_THRESHOLD2       10
#define TIMEOUT              5000000
#define CBR_PATNUM           8

ULONG pattern[8] ={
0xFF00FF00,
0xCC33CC33,
0xAA55AA55,
0x88778877,
0x92CC4D6E,
0x543D3CDE,
0xF1E843C7,
0x7C61D253};

int MMCTestBurst(PAST2300DRAMParam  param, ULONG datagen)
{
  ULONG data, timeout;
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0070, 0x000000C1 | (datagen << 3));
  timeout = 0;
  do{
    data = MIndwm(mmiobase, 0x1E6E0070) & 0x3000;
    if(data & 0x2000){
      return(0);
    }
    if(++timeout > TIMEOUT){
      //printf("Timeout!!\n");
      MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
      return(0);
    }
  }while(!data);
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  return(1);
}

ULONG MMCTestBurst2(PAST2300DRAMParam  param, ULONG datagen)
{
  ULONG data, timeout;
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000041 | (datagen << 3));
  timeout = 0;
  do{
    data = MIndwm(mmiobase, 0x1E6E0070) & 0x1000;
    if(++timeout > TIMEOUT){
      //printf("Timeout!!\n");
      MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
      return(-1);
    }
  }while(!data);
  data = MIndwm(mmiobase, 0x1E6E0078);
  data = (data | (data >> 16)) & 0xFFFF;
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  return(data);
}

int MMCTestSingle(PAST2300DRAMParam  param, ULONG datagen)
{
  ULONG data, timeout;
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000085 | (datagen << 3));
  timeout = 0;
  do{
    data = MIndwm(mmiobase, 0x1E6E0070) & 0x3000;
    if(data & 0x2000){
      return(0);
    }
    if(++timeout > TIMEOUT){
      //printf("Timeout!!\n");
      MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
      return(0);
    }
  }while(!data);
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  return(1);
}

ULONG MMCTestSingle2(PAST2300DRAMParam  param, ULONG datagen)
{
  ULONG data, timeout;
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000005 | (datagen << 3));
  timeout = 0;
  do{
    data = MIndwm(mmiobase, 0x1E6E0070) & 0x1000;
    if(++timeout > TIMEOUT){
      //printf("Timeout!!\n");
      MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
      return(-1);
    }
  }while(!data);
  data = MIndwm(mmiobase, 0x1E6E0078);
  data = (data | (data >> 16)) & 0xFFFF;
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  return(data);
}

int CBRTest(PAST2300DRAMParam  param)
{
  ULONG data;

  data  = MMCTestSingle2(param, 0); if((data & 0xff) && (data & 0xff00)) return(0);
  data |= MMCTestBurst2(param, 0);  if((data & 0xff) && (data & 0xff00)) return(0);
  data |= MMCTestBurst2(param, 1);  if((data & 0xff) && (data & 0xff00)) return(0);
  data |= MMCTestBurst2(param, 2);  if((data & 0xff) && (data & 0xff00)) return(0);
  data |= MMCTestBurst2(param, 3);  if((data & 0xff) && (data & 0xff00)) return(0);
  data |= MMCTestBurst2(param, 4);  if((data & 0xff) && (data & 0xff00)) return(0);
  data |= MMCTestBurst2(param, 5);  if((data & 0xff) && (data & 0xff00)) return(0);
  data |= MMCTestBurst2(param, 6);  if((data & 0xff) && (data & 0xff00)) return(0);
  data |= MMCTestBurst2(param, 7);  if((data & 0xff) && (data & 0xff00)) return(0);
  if(!data)            return(3);
  else if(data & 0xff) return(2);
  else                 return(1);
}

int CBRScan(PAST2300DRAMParam  param)
{
  ULONG data, data2, patcnt, loop;
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  data2 = 3;
  for(patcnt = 0;patcnt < CBR_PATNUM;patcnt++){
    MOutdwm(mmiobase, 0x1E6E007C, pattern[patcnt]);
    for(loop = 0;loop < CBR_PASSNUM2;loop++){
      if((data = CBRTest(param)) != 0){
        data2 &= data;
        if(!data2){
          return(0);
        }
        break;
      }
    }
    if(loop == CBR_PASSNUM2){
      return(0);
    }
  }
  return(data2);
}

ULONG CBRTest2(PAST2300DRAMParam  param)
{
  ULONG data;

  data  = MMCTestBurst2(param, 0);  if(data == 0xffff) return(0);
  data |= MMCTestSingle2(param, 0); if(data == 0xffff) return(0);
  return(~data & 0xffff);
}

ULONG CBRScan2(PAST2300DRAMParam  param)
{
  ULONG data, data2, patcnt, loop;
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  data2 = 0xffff;
  for(patcnt = 0;patcnt < CBR_PATNUM;patcnt++){
    MOutdwm(mmiobase, 0x1E6E007C, pattern[patcnt]);
    for(loop = 0;loop < CBR_PASSNUM2;loop++){
      if((data = CBRTest2(param)) != 0){
        data2 &= data;
        if(!data2){
          return(0);
        }
        break;
      }
    }
    if(loop == CBR_PASSNUM2){
      return(0);
    }
  }
  return(data2);
}

ULONG CBRTest3(PAST2300DRAMParam  param)
{
  if(!MMCTestBurst(param, 0)) return(0);
  if(!MMCTestSingle(param, 0)) return(0);
  return(1);
}

ULONG CBRScan3(PAST2300DRAMParam  param)
{
  ULONG patcnt, loop;
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  for(patcnt = 0;patcnt < CBR_PATNUM;patcnt++){
    MOutdwm(mmiobase, 0x1E6E007C, pattern[patcnt]);
    for(loop = 0;loop < 2;loop++){
      if(CBRTest3(param)){
        break;
      }
    }
    if(loop == 2){
      return(0);
    }
  }
  return(1);
}

void finetuneDQI_L(PAST2300DRAMParam  param)
{
  ULONG gdll1;                                                                                      // V1 added, 2012.11.29
  ULONG gold_sadj[2], dllmin[16], dllmax[16], dlli, data, cnt, mask, passcnt;
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  FINETUNE_START:
  for(cnt = 0;cnt < 16;cnt++){
    dllmin[cnt] = 0xff;
    dllmax[cnt] = 0x0;
  }
  passcnt = 0;
  gdll1 = MIndwm(mmiobase, 0x1E6E0068) & 0xFFFF;                                                    // V1 added, 2012.11.29
  for(dlli = 0;dlli < 76;dlli++){
//  MOutdwm(mmiobase, 0x1E6E0068, 0x00001400 | (dlli << 16) | (dlli << 24));                        // V1 removed
    MOutdwm(mmiobase, 0x1E6E0068, gdll1 | (dlli << 16) | (dlli << 24));                             // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E0074, CBR_SIZE1);
    data = CBRScan2(param);
    if(data != 0){
      mask = 0x00010001;
      for(cnt = 0;cnt < 16;cnt++){
        if(data & mask){
          if(dllmin[cnt] > dlli){
            dllmin[cnt] = dlli;
          }
          if(dllmax[cnt] < dlli){
            dllmax[cnt] = dlli;
          }
        }
        mask <<= 1;
      }
      passcnt++;
    }else if(passcnt >= CBR_THRESHOLD2){
      break;
    }
  }
  gold_sadj[0] = 0x0;
  passcnt = 0;
  for(cnt = 0;cnt < 16;cnt++){
    if((dllmax[cnt] > dllmin[cnt]) && ((dllmax[cnt] - dllmin[cnt]) >= CBR_THRESHOLD2)){
      gold_sadj[0] += dllmin[cnt];
      passcnt++;
    }
  }
  if(passcnt != 16){
    goto FINETUNE_START;
  }
  gold_sadj[0] = gold_sadj[0] >> 4;
  gold_sadj[1] = gold_sadj[0];
  //printf("Target DLLI MIN = %d\n",gold_sadj[0]);

  data = 0;
  for(cnt = 0;cnt < 8;cnt++){
    data >>= 3;
    if((dllmax[cnt] > dllmin[cnt]) && ((dllmax[cnt] - dllmin[cnt]) >= CBR_THRESHOLD2)){
      dlli = dllmin[cnt];
      if(gold_sadj[0] >= dlli){
        dlli = ((gold_sadj[0] - dlli) * 19) >> 5;
        if(dlli > 3){
          dlli = 3;
        }
      }else{
        dlli = ((dlli - gold_sadj[0]) * 19) >> 5;
        if(dlli > 4){
          dlli = 4;
        }
        dlli = (8 - dlli) & 0x7;
      }
      data |= dlli << 21;
    }
  }
  MOutdwm(mmiobase, 0x1E6E0080, data);

  data = 0;
  for(cnt = 8;cnt < 16;cnt++){
    data >>= 3;
    if((dllmax[cnt] > dllmin[cnt]) && ((dllmax[cnt] - dllmin[cnt]) >= CBR_THRESHOLD2)){
      dlli = dllmin[cnt];
      if(gold_sadj[1] >= dlli){
        dlli = ((gold_sadj[1] - dlli) * 19) >> 5;
        if(dlli > 3){
          dlli = 3;
        }else{
          dlli = (dlli - 1) & 0x7;
        }
      }else{
        dlli = ((dlli - gold_sadj[1]) * 19) >> 5;
        dlli += 1;
        if(dlli > 4){
          dlli = 4;
        }
        dlli = (8 - dlli) & 0x7;
      }
      data |= dlli << 21;
    }
  }
  MOutdwm(mmiobase, 0x1E6E0084, data);

  //debug
  //for(cnt = 0;cnt < 16;cnt++){
  //  MOutdwm(mmiobase, 0x1E720010 + (cnt << 2), dllmax[cnt] << 8 | dllmin[cnt]);
  //}
}

void finetuneDQSI(PAST2300DRAMParam  param)
{
  ULONG gdll1;                                                                                      // V1 added, 2012.11.29
  ULONG dlli, dqsip, dqidly;
  ULONG reg_mcr18, passcnt[2], diff;
  ULONG g_dqidly, g_dqsip, g_margin, g_side, max_margin;                                            // V1, modified, 2012.11.29
  unsigned short pass[32][2][2];
  char tag[2][76];
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  // Disable DQI CBR
//reg_mcr0c  = MIndwm(mmiobase, 0x1E6E000C);                                                        // V1 removed
  reg_mcr18  = MIndwm(mmiobase, 0x1E6E0018);
  reg_mcr18 &= 0x0000ffff;
  MOutdwm(mmiobase, 0x1E6E0018, reg_mcr18);
  gdll1 = MIndwm(mmiobase, 0x1E6E0068) & 0xFFFF;                                                    // V1 added, 2012.11.29

  for(dlli = 0;dlli < 76;dlli++){
    tag[0][dlli] = 0x0;
    tag[1][dlli] = 0x0;
  }
  for(dqidly = 0;dqidly < 32;dqidly++){
    pass[dqidly][0][0] = 0xff;    // dqsip=0,dllmin
    pass[dqidly][0][1] = 0x0;     // dqsip=0,dllmax
    pass[dqidly][1][0] = 0xff;    // dqsip=1,dllmin
    pass[dqidly][1][1] = 0x0;     // dqsip=1,dllmax
  }
  for(dqidly = 0;dqidly < 32;dqidly++){
    passcnt[0] = passcnt[1] = 0;
    for(dqsip = 0;dqsip < 2;dqsip++){
//    MOutdwm(mmiobase, 0x1E6E000C, 0);                                                             // V1 removed
      MOutdwm(mmiobase, 0x1E6E0018, reg_mcr18 | (dqidly << 16) | (dqsip << 23));
//    MOutdwm(mmiobase, 0x1E6E000C, reg_mcr0c);                                                     // V1 removed
      udelay(10); // delay 10 us                                                                     // V1 added, 2012.11.29
      for(dlli = 0;dlli < 76;dlli++){
//      MOutdwm(mmiobase, 0x1E6E0068, 0x00001300 | (dlli << 16) | (dlli << 24));                    // V1 removed
        MOutdwm(mmiobase, 0x1E6E0068, gdll1 | (dlli << 16) | (dlli << 24));                         // V1 added, 2012.11.29
        MOutdwm(mmiobase, 0x1E6E0070, 0);
        MOutdwm(mmiobase, 0x1E6E0074, CBR_SIZE0);
        if(CBRScan3(param)){
          if(dlli == 0){
            break;
          }
          passcnt[dqsip]++;
          tag[dqsip][dlli] = 'P';
          if(dlli < pass[dqidly][dqsip][0]){
            pass[dqidly][dqsip][0] = (USHORT)dlli;
          }
          if(dlli > pass[dqidly][dqsip][1]){
            pass[dqidly][dqsip][1] = (USHORT)dlli;
          }
        }else if(passcnt[dqsip] >= 5){
          break;
        }else{
          pass[dqidly][dqsip][0] = 0xff;
          pass[dqidly][dqsip][1] = 0x0;
        }
      }
    }
    //printf("DQIDLY = %d, MarginP = %2d, MarginN = %2d\n",dqidly,passcnt[0],passcnt[1]);
    if(passcnt[0] == 0 && passcnt[1] == 0){
      dqidly++;
    }
  }
  //MOutdwm(mmiobase, 0x1E6E000C, reg_mcr0c);                                                         // V1 added, 2012.11.29
  // Search margin
  g_dqidly = g_dqsip = g_margin = g_side = max_margin = 0;                                          // V1 modified, 2012.11.29

  for(dqidly = 0;dqidly < 32;dqidly++){
    for(dqsip = 0;dqsip < 2;dqsip++){
      if(pass[dqidly][dqsip][0] > pass[dqidly][dqsip][1]){
        continue;
      }
      diff = pass[dqidly][dqsip][1] - pass[dqidly][dqsip][0];
      // define +/- 2 steps of margin
      if((diff+2) < max_margin){                                                                    // V1 modified, 2012.11.29
        continue;
      }
      passcnt[0] = passcnt[1] = 0;
      for(dlli = pass[dqidly][dqsip][0];dlli > 0  && tag[dqsip][dlli] != 0;dlli--,passcnt[0]++);
      for(dlli = pass[dqidly][dqsip][1];dlli < 76 && tag[dqsip][dlli] != 0;dlli++,passcnt[1]++);
      if(passcnt[0] > passcnt[1]){
        passcnt[0] = passcnt[1];
      }
//    passcnt[1] = 0;                                                                               // V1 removed
//    if(passcnt[0] > g_side){                                                                      // V1 removed
//      passcnt[1] = passcnt[0] - g_side;                                                           // V1 removed
//    }                                                                                             // V1 removed
//    if(diff > (g_margin+1) && (passcnt[1] > 0 || passcnt[0] > 8)){                                // V1 removed
      if((diff >  g_margin    && passcnt[0] >= g_side && dqidly <= 20) ||                           // V1 added, 2012.11.29
         (diff > (g_margin+1) && passcnt[0] >= g_side)){                                            // V1 added, 2012.11.29
        max_margin = diff;                                                                          // V1 added, 2012.11.29
        g_margin = diff;
        g_dqidly = dqidly;
        g_dqsip  = dqsip;
        g_side   = passcnt[0];
//    }else if(passcnt[1] > 1 && g_side < 8){                                                       // V1 removed
      }else if(passcnt[0] > g_side && g_side < 8){                                                  // V1 added, 2012.11.29
        if(diff > max_margin){                                                                      // V1 modified, 2012.11.29
          max_margin = diff;                                                                        // V1 modified, 2012.11.29
        }
        g_margin = diff;                                                                            // V1 added, 2012.11.29
        g_dqidly = dqidly;
        g_dqsip  = dqsip;
        g_side   = passcnt[0];
      }
    }
  }
  //debug
#if 0  
  printf("\n     |");
  for(cnt = 0;cnt < 64;cnt++){
    printf("%0d",cnt/10);
  }
  printf("\n     |");
  for(cnt = 0;cnt < 64;cnt++){
    printf("%0d",cnt%10);
  }
  printf("\n-----|");
  for(cnt = 0;cnt < 64;cnt++){
    printf("-");
  }
  printf("\nPedge|");
  for(cnt = 0;cnt < 64;cnt++){
    if(tag[0][cnt]){
      printf("*");
    }else{
      printf(".");
    }
  }
  printf("\nNedge|");
  for(cnt = 0;cnt < 64;cnt++){
    if(tag[1][cnt]){
      printf("*");
    }else{
      printf(".");
    }
  }
#endif  
  //printf("\nFinal DQIDLY = %d, DQSIP = %d, Side margin = %d\n",g_dqidly,g_dqsip,g_side);
  reg_mcr18 = reg_mcr18 | (g_dqidly << 16) | (g_dqsip << 23);
  MOutdwm(mmiobase, 0x1E6E0018, reg_mcr18);
  udelay(10); // delay 10 us                                                                        // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x1E6E000C, 0x00005C01);                                                        // V1 added, 2012.11.29
}

void finetuneMask(PAST2300DRAMParam  param)                                                                                 // V2 added, 2013.07.31
{
  ULONG reg_mcr18, passcnt[2], min[2], i, j;
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  reg_mcr18  = MIndwm(mmiobase, 0x1E6E0018) & 0xffffffe0;

  passcnt[0] = 0;
  passcnt[1] = 0;
  min[0] = min[1] = 0xFF;
  for(i = 0;i < 2;i++){
    for(j = 0;j < 16;j++){
      MOutdwm(mmiobase, 0x1E6E0018, reg_mcr18 | (i << 4) | j);
      MOutdwm(mmiobase, 0x1E6E0070, 0);
      MOutdwm(mmiobase, 0x1E6E0074, CBR_SIZE1);
      if(CBRScan3(param)){
        passcnt[i]++;
        if(min[i] > j){
          min[i] = j;
        }
      }
    }
  }
  if(min[0] != 0 && min[1] != 0){
    //printf("DQS Mask calibration fail!\n");
  }else if((min[1] == 0 && passcnt[0] < passcnt[1]) ||
           (min[1] == 0 && min[0] != 0)){
    reg_mcr18 |= 0x10;
    //printf("DQS Mask delay mode : Delay 0.5T\n");
  }else{
    //printf("DQS Mask delay mode : No Delay\n");
  }
  MOutdwm(mmiobase, 0x1E6E0018, reg_mcr18);
}

void CBRDLL2(PAST2300DRAMParam  param)
{
  ULONG gdll1;                                                                                      // V1 added, 2012.11.29
  ULONG dllmin[2], dllmax[2], dlli, data, passcnt;
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  finetuneDQSI(param);
  finetuneDQI_L(param);

  CBR_START2:
  dllmin[0] = dllmin[1] = 0xff;
  dllmax[0] = dllmax[1] = 0x0;
  passcnt = 0;
  gdll1 = MIndwm(mmiobase, 0x1E6E0068) & 0xFFFF;                                                    // V1 added, 2012.11.29
  for(dlli = 0;dlli < 76;dlli++){
//  MOutdwm(mmiobase, 0x1E6E0068, 0x00001300 | (dlli << 16) | (dlli << 24));                        // V1 removed
    MOutdwm(mmiobase, 0x1E6E0068, gdll1 | (dlli << 16) | (dlli << 24));                             // V1 added, 2012.11.29
    data = MIndwm(mmiobase, 0x1E6E0000); // dummy read                                              // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E0074, CBR_SIZE2);
    data = CBRScan(param);
    if(data != 0){
      if(data & 0x1){
        if(dllmin[0] > dlli){
          dllmin[0] = dlli;
        }
        if(dllmax[0] < dlli){
          dllmax[0] = dlli;
        }
      }
      if(data & 0x2){
        if(dllmin[1] > dlli){
          dllmin[1] = dlli;
        }
        if(dllmax[1] < dlli){
          dllmax[1] = dlli;
        }
      }
      passcnt++;
    }else if(passcnt >= CBR_THRESHOLD){
      break;
    }
  }
  if(dllmax[0] == 0 || (dllmax[0]-dllmin[0]) < CBR_THRESHOLD){
    //printf("Retry CBR2, L=%d:%d\n",dllmin[0],dllmax[0]);
    goto CBR_START2;
  }
  if(dllmax[1] == 0 || (dllmax[1]-dllmin[1]) < CBR_THRESHOLD){
    //printf("Retry CBR2, H=%d:%d\n",dllmin[1],dllmax[1]);
    goto CBR_START2;
  }
  dlli  = (dllmin[1] + dllmax[1] + 1) >> 1;                                                         // V1 modified, 2012.11.29
  dlli <<= 8;
  dlli += (dllmin[0] + dllmax[0] + 1) >> 1;                                                         // V1 modified, 2012.11.29

  //debug
  //MOutdwm(mmiobase, 0x1E720008, ((dlli & 0xff)   << 16) | (dllmax[0] << 8) | dllmin[0]);
  //MOutdwm(mmiobase, 0x1E72000C, ((dlli & 0xff00) <<  8) | (dllmax[1] << 8) | dllmin[1]);
  //printf("DLL2.0 min = %d, max = %d\n",dllmin[0],dllmax[0]);
  //printf("DLL2.1 min = %d, max = %d\n",dllmin[1],dllmax[1]);
//MOutdwm(mmiobase, 0x1E6E0068, MIndwm(mmiobase, 0x1E720058) | (dlli << 16));                       // V1 removed
  MOutdwm(mmiobase, 0x1E6E0068, gdll1 | (dlli << 16));                                              // V1 added, 2012.11.29

  finetuneMask(param);                                                                                   // V2 added, 2013.07.31
}

void CBRDLL1_2300(PAST2300DRAMParam  param, int mode2400)                                                                     // V1 added, 2012.11.29
{
  ULONG madjmax;
  ULONG data, data2, dll, dllend, dllmin, dllmax, dllcnt, dllcnt2;
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  madjmax = MIndwm(mmiobase, 0x1E6E0064) & 0xFF;

  CBRDLL1_START:
  dllmin = 0xff;
  dllmax = 0;
  dllcnt = 0;
  dllcnt2 = 0;
  dllend = madjmax;
  for(dll = 0;dll < dllend;dll++){
    MOutdwm(mmiobase, 0x1E6E0018, 0x00008130);
    if(dll >= madjmax){
      data = dll - madjmax;
    }else{
      data = dll;
    }
    MOutdwm(mmiobase, 0x1E6E0068, data);
    data = MIndwm (mmiobase, 0x1E6E0000);  // dummy read
    MOutdwm(mmiobase, 0x1E6E0018, 0x00000130);
    data = MIndwm (mmiobase, 0x1E6E0000);  // dummy read
    data = MIndwm (mmiobase, 0x1E6E0000);  // dummy read
    data = (MIndwm(mmiobase, 0x1E6E001C) >> 16) & 0xFF;
    if((data & 0x96) == 0x96){
      if(dll == 0){
        dll = 15;
        dllend += 16;
        continue;
      }
      if(data == 0x96){
        if(dllcnt2 >= 2){
          dllmin = dll;
          dllmax = dll;
          break;
        }
        dllcnt2++;
      }
      if(dllmin > dll){
        dllmin = dll;
      }
      if(dllmax < dll){
        dllmax = dll;
      }
      dllcnt = 0;
    }else if(dllmax != 0){
      if(++dllcnt >= 2){
        dllend = dll;
      }
    }
  }
  if(dllmax != 0){
    dll = (dllmin + dllmax) >> 1;
  }else{
    data = MIndwm(mmiobase, 0x1E6E0064) & 0x3FFFF;
    MOutdwm(mmiobase, 0x1E6E0064, data);
    MOutdwm(mmiobase, 0x1E6E0068, 0);
    udelay(10); // delay 10 us
    MOutdwm(mmiobase, 0x1E6E0064, data | 0xC0000);
    udelay(10); // delay 10 us
    //printf("Retry CBRDLL1!\n");
    goto CBRDLL1_START;
  }
  if(dll >= madjmax){
    dll = dll - madjmax;
  }
  if(mode2400 == 1){
    data = dll + (madjmax >> 2) + 5;
    if(data >= madjmax){
      data = data - madjmax;
    }
  }else{
    data = dll + (madjmax >> 2);
    if(data >= madjmax){
      data = data - madjmax;
    }
    data2 = 0;
    if(data & 0x8){
      while(data & 0x8 || data2 < 5){
        data2++;
        if(++data >= madjmax){
          data = data - madjmax;
        }
      }
    }else{
      while((data & 0x7) != 0x7 && data2 < 5){
        data2++;
        if(++data >= madjmax){
          data = data - madjmax;
        }
      }
    }
    //printf("DLL1 value = %02X:%02X\n",data,dll);
  }
  dll = dll | (data << 8);
  MOutdwm(mmiobase, 0x1E6E0068, dll);
  data = MIndwm (mmiobase, 0x1E6E0000);  // dummy read
  MOutdwm(mmiobase, 0x1E6E0018, 0x00008130);
  MOutdwm(mmiobase, 0x1E6E0018, 0x00000130);
}

void CBRDLL1_2400(PAST2300DRAMParam  param)                                                                                 // V1 added, 2012.11.29
{
  ULONG data, data2;
  ULONG drv, duty, dqidly;
  ULONG gdll, gduty, gdutysum;
  UCHAR *mmiobase = param->pjMMIOVirtualAddress;

  MOutdwm(mmiobase, 0x1E6E0034, 0x00000020);
  drv     = MIndwm(mmiobase, 0x1E6E0060) | 0x2000;
  gdutysum = 0x0;
  for(duty = 0;duty < 32;duty++){
    MOutdwm(mmiobase, 0x1E6E0018, 0x00008130);
    MOutdwm(mmiobase, 0x1E6E0060, (drv | (duty << 8)) ^ 0x1000);
    data = MIndwm(mmiobase, 0x1E6E0000); // dummy read

    CBRDLL1_2300(param, 1);

    // Calculate the Duty
    data2 = 0;
    for(dqidly = 1;dqidly < 5;dqidly++){
      MOutdwm(mmiobase, 0x1E6E0018, 0x00200130 | (dqidly << 16));
      data = MIndwm(mmiobase, 0x1E6E0000); // dummy read
      MOutdwm(mmiobase, 0x1E6E0018, 0x10200130 | (dqidly << 16));
      data = 0;
      while(!(data & 0x80000000)){
        data = MIndwm(mmiobase, 0x1E6E0080);
      }
      data  = MIndwm(mmiobase, 0x1E6E008C);
      if((data & 0xFFFF) == 0xFFFF){
        data = data >> 16;
      }
      data   = data & 0xFFFF;
      data2 += data;
      if(data < 0xF000){
        break;
      }
    }
    data2 = data2 >> 2;
    if(data2 > gdutysum){
      gdll  = MIndwm(mmiobase, 0x1E6E0068);
      gduty = duty ^ 0x10;
      gdutysum = data2;
      //printf("DLL = %X, DUTY = %X, SUM = %X\n",gdll,gduty,gdutysum);
    }
    if(gdutysum > 0xFA00 && duty >= 15){
      break;
    }
  }
  MOutdwm(mmiobase, 0x1E6E0060, drv | (gduty << 8));
  MOutdwm(mmiobase, 0x1E6E0068, gdll);
  data = MIndwm(mmiobase, 0x1E6E0000); // dummy read
  MOutdwm(mmiobase, 0x1E6E0018, 0x00008130);
  MOutdwm(mmiobase, 0x1E6E0018, 0x00000130);
  //printf("DUTY = %X, DLL1 value = %04X\n",gduty,gdll);
}

void GetDDR2Info(PAST2300DRAMParam param)
{
  UCHAR *mmiobase;
  ULONG trap, TRAP_AC2, TRAP_MRS, PLLSET;

  mmiobase = param->pjMMIOVirtualAddress;
  MOutdwm(mmiobase, 0x1E6E2000, 0x1688A8A8);

  /* Get PLLSET */
  if((MIndwm(mmiobase, 0x1E6E007C) >> 24) == 0x2){                                                  // V1 added, 2012.11.29
    if(MIndwm(mmiobase, 0x1E6E2070) & 0x00800000){                                                  // V1 added, 2012.11.29
      PLLSET = 0x017001D0;                                                                          // V1 added, 2012.11.29
    }else{                                                                                          // V1 added, 2012.11.29
      PLLSET = 0x033103F1;                                                                          // V1 added, 2012.11.29
    }                                                                                               // V1 added, 2012.11.29
  }else{                                                                                            // V1 added, 2012.11.29
    PLLSET = 0x033103F1;                                                                            // V1 added, 2012.11.29
  }                                                                                                 // V1 added, 2012.11.29
  
  /* Ger trap info */
  trap = (MIndwm(mmiobase, 0x1E6E2070) >> 25) & 0x3;
  TRAP_AC2  = (trap << 20) | (trap << 16);
  TRAP_AC2 += 0x00110000;
  TRAP_MRS  = 0x00000040 | (trap << 4);

//param->REG_MADJ                   = 0x00034C4C;                                                          // V1 removed
  param->REG_MADJ                   = 0x00034C58;                                                          // V1 added, 2012.11.29
  param->REG_SADJ                   = 0x00001800;
  param->REG_DRV                    = 0x000000F0;
// REG_PERIOD                 = freq;
  param->ENABLE_WODT                = 0;
  param->ENABLE_RODT                = 0;
  switch(param->DRAM_Freq){
    case 336 : MOutdwm(mmiobase, 0x1E6E2020, PLLSET >> 16);                                         // V1 modified, 2012.11.29
               param->ENABLE_WODT   = 1;
               param->REG_AC1       = 0x22201613;
               param->REG_AC2       = 0xAA00903B | TRAP_AC2;                                               // V1 modified, 2012.11.29
               param->REG_DQSIC     = 0x000000BA;
//             param->REG_MRS       = 0x00000A02 | TRAP_MRS;                                               // V1 removed
               param->REG_MRS       = 0x00000A03 | TRAP_MRS;                                               // V1 added, 2012.11.29
               param->REG_EMRS      = 0x00000040;
               param->REG_DRV       = 0x000000FA;
//             param->REG_IOZ       = 0x00000013;                                                          // V1 removed
               param->REG_IOZ       = 0x00000023;                                                          // V1 added, 2012.11.29
               param->REG_DQIDLY    = 0x00000074;
               param->REG_FREQ      = 0x00004DC0;
               param->REG_tRFC      = 0x3B231612;                                                          // V1 added, 2012.11.29
               param->MADJ_MAX      = 96;
               param->DLL2_FINETUNE_STEP = 3; 
               break;
               
    default:
    case 408 : MOutdwm(mmiobase, 0x1E6E2020, PLLSET & 0xFFFF);                                      // V1 modified, 2012.11.29
               param->ENABLE_WODT   = 1;
               param->ENABLE_RODT   = 0;
               param->REG_AC1       = 0x33302714;
               param->REG_AC2       = 0xCC00B03F | TRAP_AC2;                                               // V1 modified, 2012.11.29
               param->REG_DQSIC     = 0x000000E2;
//             param->REG_MRS       = 0x00000C02 | TRAP_MRS;                                               // V1 removed
               param->REG_MRS       = 0x00000C03 | TRAP_MRS;                                               // V1 added, 2012.11.29
               param->REG_EMRS      = 0x00000040;
               param->REG_DRV       = 0x000000FA;
//             param->REG_IOZ       = 0x00000013;                                                          // V1 removed
               param->REG_IOZ       = 0x00000023;                                                          // V1 added, 2012.11.29
               param->REG_DQIDLY    = 0x00000089;
               param->REG_FREQ      = 0x000050C0;
               param->REG_tRFC      = 0x3F2B1B16;                                                          // V1 added, 2012.11.29
               param->MADJ_MAX      = 96;
               param->DLL2_FINETUNE_STEP = 4;
               break;
  }

  //Skip for auto-sizing, ycchen@030614  
#if 0  
  switch (param->DRAM_ChipID)
  {
  case DRAMTYPE_512Mx16:
      param->DRAM_CONFIG = 0x100;
      break;
  default:
  case DRAMTYPE_1Gx16:
      param->DRAM_CONFIG = 0x121;
      break;
   case DRAMTYPE_2Gx16:
      param->DRAM_CONFIG = 0x122;
      break;
  case DRAMTYPE_4Gx16:
      param->DRAM_CONFIG = 0x123;
      break;
  }; /* switch size */

  switch (param->VRAM_Size)
  {
  default:
  case VRAM_SIZE_008M:
      param->DRAM_CONFIG |= 0x00;
      break;
  case VRAM_SIZE_016M:
      param->DRAM_CONFIG |= 0x04;
      break;
  case VRAM_SIZE_032M:
      param->DRAM_CONFIG |= 0x08;
      break;
  case VRAM_SIZE_064M:
      param->DRAM_CONFIG |= 0x0c;
      break;
  }
#endif

}

void DDR2_Init(PAST2300DRAMParam param)
{
  ULONG data, chip2400;
  UCHAR *mmiobase;

  mmiobase = param->pjMMIOVirtualAddress;

  MOutdwm(mmiobase, 0x1E6E0000, 0xFC600309);
  MOutdwm(mmiobase, 0x1E6E0064, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0034, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0018, 0x00000100);
  MOutdwm(mmiobase, 0x1E6E0024, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0064, param->REG_MADJ);
  MOutdwm(mmiobase, 0x1E6E0068, param->REG_SADJ);
  udelay(10);
  MOutdwm(mmiobase, 0x1E6E0064, param->REG_MADJ | 0xC0000);
  udelay(10);

  //MOutdwm(mmiobase, 0x1E6E0004, param->DRAM_CONFIG);
  MOutdwm(mmiobase, 0x1E6E0004, 0x00000510);  
  MOutdwm(mmiobase, 0x1E6E0008, 0x90040f);
  MOutdwm(mmiobase, 0x1E6E0010, param->REG_AC1);
  MOutdwm(mmiobase, 0x1E6E0014, param->REG_AC2);
  MOutdwm(mmiobase, 0x1E6E0020, param->REG_DQSIC);
  MOutdwm(mmiobase, 0x1E6E0080, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0084, 0x00FFFFFF);
  MOutdwm(mmiobase, 0x1E6E0088, param->REG_DQIDLY);
  MOutdwm(mmiobase, 0x1E6E0018, 0x4000A130);
  //MOutdwm(mmiobase, 0x1E6E0018, 0x00002330);
  MOutdwm(mmiobase, 0x1E6E0018, 0x00000130);  
  MOutdwm(mmiobase, 0x1E6E0038, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0040, 0xFF808000);
  MOutdwm(mmiobase, 0x1E6E0044, 0x88848466);
  MOutdwm(mmiobase, 0x1E6E0048, 0x44440008);
  MOutdwm(mmiobase, 0x1E6E004C, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0050, 0x80000000);
  MOutdwm(mmiobase, 0x1E6E0050, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0054, 0);
  MOutdwm(mmiobase, 0x1E6E0060, param->REG_DRV);
  MOutdwm(mmiobase, 0x1E6E0004, MIndwm(mmiobase, 0x1E6E0004) | 0x400);                              // V1 added, 2012.11.29
  chip2400 = MIndwm(mmiobase, 0x1E6E0004) >> 10;                                                    // V1 added, 2012.11.29
  if(chip2400){                                                                                     // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E006C, (param->REG_IOZ << 8) | (param->REG_IOZ - 0x11));                               // V1 added, 2012.11.29
  }else{                                                                                            // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E006C, param->REG_IOZ);                                                         // V1 added, 2012.11.29
  }                                                                                                 // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0074, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0078, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E007C, 0x00000000);
  
  /* Wait MCLK2X lock to MCLK */
  if(chip2400){                                                                                     // V1 added, 2012.11.29
    CBRDLL1_2400(param);                                              								// V1 added, 2012.11.29
  }else{                                                                                            // V1 added, 2012.11.29
    CBRDLL1_2300(param, 0);                                            								// V1 added, 2012.11.29
  }                                                                                                 // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x1E6E0120, MIndwm(mmiobase, 0x1E6E0068) & 0xffff);                             // V1 added, 2012.11.29
  
  MOutdwm(mmiobase, 0x1E6E0034, 0x00000001);
  MOutdwm(mmiobase, 0x1E6E000C, 0x00000000);
  udelay(50);                                                         // Delay 400 us
  // Mode Register Setting
  MOutdwm(mmiobase, 0x1E6E002C, param->REG_MRS | 0x100);
  MOutdwm(mmiobase, 0x1E6E0030, param->REG_EMRS);
  MOutdwm(mmiobase, 0x1E6E0028, 0x00000005);
  MOutdwm(mmiobase, 0x1E6E0028, 0x00000007);
  MOutdwm(mmiobase, 0x1E6E0028, 0x00000003);
  MOutdwm(mmiobase, 0x1E6E0028, 0x00000001);
  MOutdwm(mmiobase, 0x1E6E000C, 0x00005C08);
  MOutdwm(mmiobase, 0x1E6E002C, param->REG_MRS);
  MOutdwm(mmiobase, 0x1E6E0028, 0x00000001);
  MOutdwm(mmiobase, 0x1E6E0030, param->REG_EMRS | 0x380);
  MOutdwm(mmiobase, 0x1E6E0028, 0x00000003);
  MOutdwm(mmiobase, 0x1E6E0030, param->REG_EMRS);
  MOutdwm(mmiobase, 0x1E6E0028, 0x00000003);

  MOutdwm(mmiobase, 0x1E6E000C, 0x00002001);                                                        // V1 modified, 2012.11.29
  data = 0;
  if(param->ENABLE_WODT){
    data = 0x500;
  }
  if(param->ENABLE_RODT){
    data = data | 0x0000 | ((param->REG_AC2 & 0x60000) >> 3);
  }
  MOutdwm(mmiobase, 0x1E6E0034, data | 0x3);
  //MOutdwm(mmiobase, 0x1E6E0120, REG_FREQ);                                                          // V1 removed

  // Calibrate the DQSI delay
  CBRDLL2(param);

  // Check DRAM Size
  data = MIndwm(mmiobase, 0x1E6E0004) & 0xFFFFFFDC;                                                           // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x1E6E0004, data | 0x23);                                                                 // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x40000000, 0x1817191A);                                                                  // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x40002000, 0x73616532);                                                                  // V1 added, 2012.11.29
  if(MIndwm(mmiobase, 0x40000000) != 0x1817191A){               // 512Mbit                                    // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E0004, data);                                                                      // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E0014, (MIndwm(mmiobase, 0x1E6E0014) & 0xFFFFFF00) | (param->REG_tRFC & 0xFF));           // V1 added, 2012.11.29
  }else{                                                                                                      // V1 added, 2012.11.29
    data |= 0x20;                                                                                             // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x50100000, 0x41424344);                                                                // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x48100000, 0x25262728);                                                                // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x40100000, 0x191A1B1C);                                                                // V1 added, 2012.11.29
    if(MIndwm(mmiobase, 0x50100000) == 0x41424344){             // 4Gbit                                      // V1 added, 2012.11.29
      MOutdwm(mmiobase, 0x1E6E0004, data | 0x3);                                                              // V1 added, 2012.11.29
      MOutdwm(mmiobase, 0x1E6E0014, (MIndwm(mmiobase, 0x1E6E0014) & 0xFFFFFF00) | ((param->REG_tRFC >> 24) & 0xFF)); // V1 added, 2012.11.29
    }else if(MIndwm(mmiobase, 0x48100000) == 0x25262728){       // 2Gbit                                      // V1 added, 2012.11.29
      MOutdwm(mmiobase, 0x1E6E0004, data | 0x2);                                                              // V1 added, 2012.11.29
      MOutdwm(mmiobase, 0x1E6E0014, (MIndwm(mmiobase, 0x1E6E0014) & 0xFFFFFF00) | ((param->REG_tRFC >> 16) & 0xFF)); // V1 added, 2012.11.29
    }else{                                                                                                    // V1 added, 2012.11.29
      MOutdwm(mmiobase, 0x1E6E0004, data | 0x1);                                                              // V1 added, 2012.11.29
      MOutdwm(mmiobase, 0x1E6E0014, (MIndwm(mmiobase, 0x1E6E0014) & 0xFFFFFF00) | ((param->REG_tRFC >> 8 ) & 0xFF)); // V1 added, 2012.11.29
    }                                                                                                         // V1 added, 2012.11.29
  }                                                                                                           // V1 added, 2012.11.29

  MOutdwm(mmiobase, 0x1E6E0120, param->REG_FREQ);                                                          // V1 added, 2012.11.29
// ECC Memory Initialization
//#ifdef ECC                                                                                        // V1 removed
//  MOutdwm(mmiobase, 0x1E6E007C, 0x00000000);                                                      // V1 removed
//  MOutdwm(mmiobase, 0x1E6E0070, 0x221);                                                           // V1 removed
//  do{                                                                                             // V1 removed
//    data = MIndwm(mmiobase, 0x1E6E0070);                                                          // V1 removed
//  }while(!(data & 0x00001000));                                                                   // V1 removed
//  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);                                                      // V1 removed
//  MOutdwm(mmiobase, 0x1E6E0050, 0x80000000);                                                      // V1 removed
//  MOutdwm(mmiobase, 0x1E6E0050, 0x00000000);                                                      // V1 removed
//#endif                                      
}

void GetDDR3Info(PAST2300DRAMParam param)
{
  UCHAR *mmiobase;
  ULONG trap, TRAP_AC2, TRAP_MRS, PLLSET;

  mmiobase = param->pjMMIOVirtualAddress;
  MOutdwm(mmiobase, 0x1E6E2000, 0x1688A8A8);

  if((MIndwm(mmiobase, 0x1E6E007C) >> 24) == 0x2){                                                  // V1 added, 2012.11.29
    if(MIndwm(mmiobase, 0x1E6E2070) & 0x00800000){                                                  // V1 added, 2012.11.29
      PLLSET = 0x017001D0;                                                                          // V1 added, 2012.11.29
    }else{                                                                                          // V1 added, 2012.11.29
      PLLSET = 0x033103F1;                                                                          // V1 added, 2012.11.29
    }                                                                                               // V1 added, 2012.11.29
  }else{                                                                                            // V1 added, 2012.11.29
    PLLSET = 0x033103F1;                                                                            // V1 added, 2012.11.29
  }                                                                                                 // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x1E6E2000, 0x1688A8A8);
  trap = (MIndwm(mmiobase, 0x1E6E2070) >> 25) & 0x3;
  TRAP_AC2  = 0x00020000 + (trap << 16);
  TRAP_AC2 |= 0x00300000 +((trap & 0x2) << 19);
  TRAP_MRS  = 0x00000010 + (trap << 4);
  TRAP_MRS |=              (trap & 0x2) << 18;

  //param->REG_MADJ                   = 0x00034C4C;                                                          // V1 removed
  param->REG_MADJ                   = 0x00034C58;                                                          // V1 added, 2012.11.29
  param->REG_SADJ                   = 0x00001800;
  //param->REG_PERIOD                 = freq;
  param->ENABLE_RODT                = 0;
    
  switch(param->DRAM_Freq){
    case 336 : MOutdwm(mmiobase, 0x1E6E2020, PLLSET >> 16);                                         // V1 modified, 2012.11.29
               param->ENABLE_WODT   = 1;
               param->REG_AC1       = 0x22202725;
               param->REG_AC2       = 0xAA007636 | TRAP_AC2;                                               // V1 modified, 2012.11.29
               param->REG_DQSIC     = 0x000000BA;
               param->REG_MRS       = 0x04001400 | TRAP_MRS;
               param->REG_EMRS      = 0x00000000;
               param->REG_IOZ       = 0x00000023;
               param->REG_DRV       = 0x000000FA;
               param->REG_DQIDLY    = 0x00000074;
               param->REG_FREQ      = 0x00004DC0;
               param->REG_tRFC       = 0x00361C13;                                                          // V1 added, 2012.11.29
               param->MADJ_MAX      = 96;
               param->DLL2_FINETUNE_STEP = 3;
               break;	  

    default:
    case 396 :
    case 408 : MOutdwm(mmiobase, 0x1E6E2020, PLLSET & 0xFFFF);                                      // V1 modified, 2012.11.29
               param->ENABLE_WODT   = 1;
               param->ENABLE_RODT   = 0;
               param->REG_AC1       = 0x33302825;
               param->REG_AC2       = 0xCC00963F | TRAP_AC2;                                               // V1 modified, 2012.11.29
               param->REG_DQSIC     = 0x000000E2;
               param->REG_MRS       = 0x04001600 | TRAP_MRS;
               param->REG_EMRS      = 0x00000000;
               param->REG_IOZ       = 0x00000023;
               param->REG_DRV       = 0x000000FA;
               param->REG_DQIDLY    = 0x00000089;
               param->REG_FREQ      = 0x000050C0;
               param->REG_tRFC      = 0x003F2217;                                                          // V1 added, 2012.11.29
               param->MADJ_MAX      = 96;
               param->DLL2_FINETUNE_STEP = 4;
               break;
                   
  } /* switch freq */

  //Skip for auto-sizing, ycchen@030614
#if 0   
  switch (param->DRAM_ChipID)
  {
  case DRAMTYPE_512Mx16:
      param->DRAM_CONFIG = 0x130;
      break;
  default:
  case DRAMTYPE_1Gx16:
      param->DRAM_CONFIG = 0x131;
      break;
   case DRAMTYPE_2Gx16:
      param->DRAM_CONFIG = 0x132;
      break;
  case DRAMTYPE_4Gx16:
      param->DRAM_CONFIG = 0x133;
      break;
  }; /* switch size */

  switch (param->VRAM_Size)
  {
  default:
  case VRAM_SIZE_008M:
      param->DRAM_CONFIG |= 0x00;
      break;
  case VRAM_SIZE_016M:
      param->DRAM_CONFIG |= 0x04;
      break;
  case VRAM_SIZE_032M:
      param->DRAM_CONFIG |= 0x08;
      break;
  case VRAM_SIZE_064M:
      param->DRAM_CONFIG |= 0x0c;
      break;
  }
#endif

}

void DDR3_Init(PAST2300DRAMParam param)
{
  ULONG data, chip2400;
  UCHAR *mmiobase;

  mmiobase = param->pjMMIOVirtualAddress;

  MOutdwm(mmiobase, 0x1E6E0000, 0xFC600309);
  MOutdwm(mmiobase, 0x1E6E0064, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0034, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0018, 0x00000100);
  MOutdwm(mmiobase, 0x1E6E0024, 0x00000000);
  udelay(10);
  MOutdwm(mmiobase, 0x1E6E0064, param->REG_MADJ);
  MOutdwm(mmiobase, 0x1E6E0068, param->REG_SADJ);
  udelay(10);
  MOutdwm(mmiobase, 0x1E6E0064, param->REG_MADJ | 0xC0000);
  udelay(10);

  //MOutdwm(mmiobase, 0x1E6E0004, param->DRAM_CONFIG);
  MOutdwm(mmiobase, 0x1E6E0004, 0x00000531);  
  MOutdwm(mmiobase, 0x1E6E0008, 0x90040f);
  MOutdwm(mmiobase, 0x1E6E0010, param->REG_AC1);
  MOutdwm(mmiobase, 0x1E6E0014, param->REG_AC2);
  MOutdwm(mmiobase, 0x1E6E0020, param->REG_DQSIC);
  MOutdwm(mmiobase, 0x1E6E0080, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0084, 0x00FFFFFF);
  MOutdwm(mmiobase, 0x1E6E0088, param->REG_DQIDLY);
  MOutdwm(mmiobase, 0x1E6E0018, 0x4000A170);
  //MOutdwm(mmiobase, 0x1E6E0018, 0x00002370);
  MOutdwm(mmiobase, 0x1E6E0018, 0x00000170);  
  MOutdwm(mmiobase, 0x1E6E0038, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0040, 0xFF444444);
  MOutdwm(mmiobase, 0x1E6E0044, 0x22222222);
  MOutdwm(mmiobase, 0x1E6E0048, 0x22222222);
  MOutdwm(mmiobase, 0x1E6E004C, 0x00000002);
  MOutdwm(mmiobase, 0x1E6E0050, 0x80000000);
  MOutdwm(mmiobase, 0x1E6E0050, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0054, 0);
  MOutdwm(mmiobase, 0x1E6E0060, param->REG_DRV);
  MOutdwm(mmiobase, 0x1E6E0004, MIndwm(mmiobase, 0x1E6E0004) | 0x400);                              // V1 added, 2012.11.29
  chip2400 = MIndwm(mmiobase, 0x1E6E0004) >> 10;                                                    // V1 added, 2012.11.29
  if(chip2400){                                                                                     // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E006C, (param->REG_IOZ << 8) | (param->REG_IOZ - 0x11));                               // V1 added, 2012.11.29
  }else{                                                                                            // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E006C, param->REG_IOZ);                                                         // V1 added, 2012.11.29
  }                                                                                                 // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0074, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0078, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E007C, 0x00000000);

  // Wait MCLK2X lock to MCLK
  if(chip2400){                                                                                     // V1 added, 2012.11.29
    CBRDLL1_2400(param);                                                                                 // V1 added, 2012.11.29
  }else{                                                                                            // V1 added, 2012.11.29
    CBRDLL1_2300(param, 0);                                                                                // V1 added, 2012.11.29
  }                                                                                                 // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x1E6E0018, MIndwm(mmiobase, 0x1E6E0018) | 0x040);                              // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x1E6E0120, MIndwm(mmiobase, 0x1E6E0068) & 0xffff);                             // V1 added, 2012.11.29
  
  MOutdwm(mmiobase, 0x1E6E0034, 0x00000001);
  MOutdwm(mmiobase, 0x1E6E000C, 0x00000040);
  udelay(50);                                                         // Delay 400 us
  // Mode Register Setting
  MOutdwm(mmiobase, 0x1E6E002C, param->REG_MRS | 0x100);
  MOutdwm(mmiobase, 0x1E6E0030, param->REG_EMRS);
  MOutdwm(mmiobase, 0x1E6E0028, 0x00000005);
  MOutdwm(mmiobase, 0x1E6E0028, 0x00000007);
  MOutdwm(mmiobase, 0x1E6E0028, 0x00000003);
  MOutdwm(mmiobase, 0x1E6E0028, 0x00000001);
  MOutdwm(mmiobase, 0x1E6E002C, param->REG_MRS);
  MOutdwm(mmiobase, 0x1E6E000C, 0x00005C08);
  MOutdwm(mmiobase, 0x1E6E0028, 0x00000001);

  MOutdwm(mmiobase, 0x1E6E000C, 0x00002001);                                                        // V1 modified, 2012.11.29
  data = 0;
  if(param->ENABLE_WODT){
    data = 0x300;
  }
  if(param->ENABLE_RODT){
    data = data | 0x2000 | ((param->REG_AC2 & 0x60000) >> 3);
  }
  MOutdwm(mmiobase, 0x1E6E0034, data | 0x3);

  // Calibrate the DQSI delay
  CBRDLL2(param);

  // Check DRAM Size
  data = MIndwm(mmiobase, 0x1E6E0004) & 0xFFFFFFFC;                                                           // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x1E6E0004, data | 0x3);                                                                  // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x50100000, 0x41424344);                                                                  // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x48100000, 0x25262728);                                                                  // V1 added, 2012.11.29
  MOutdwm(mmiobase, 0x40100000, 0x191A1B1C);                                                                  // V1 added, 2012.11.29
  if(MIndwm(mmiobase, 0x50100000) == 0x41424344){               // 4Gbit                                      // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E0004, data | 0x3);                                                                // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E0014, (MIndwm(mmiobase, 0x1E6E0014) & 0xFFFFFF00) | ((param->REG_tRFC >> 24) & 0xFF));   // V1 added, 2012.11.29
  }else if(MIndwm(mmiobase, 0x48100000) == 0x25262728){         // 2Gbit                                      // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E0004, data | 0x2);                                                                // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E0014, (MIndwm(mmiobase, 0x1E6E0014) & 0xFFFFFF00) | ((param->REG_tRFC >> 16) & 0xFF));   // V1 added, 2012.11.29
  }else{                                                                                                      // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E0004, data | 0x1);                                                                // V1 added, 2012.11.29
    MOutdwm(mmiobase, 0x1E6E0014, (MIndwm(mmiobase, 0x1E6E0014) & 0xFFFFFF00) | ((param->REG_tRFC >> 8 ) & 0xFF));   // V1 added, 2012.11.29
  }                                                                                                           // V1 added, 2012.11.29

  MOutdwm(mmiobase, 0x1E6E0120, param->REG_FREQ);
// ECC Memory Initialization
//#ifdef ECC                                                                                        // V1 removed
//  MOutdwm(mmiobase, 0x1E6E007C, 0x00000000);                                                      // V1 removed
//  MOutdwm(mmiobase, 0x1E6E0070, 0x221);                                                           // V1 removed
//  do{                                                                                             // V1 removed
//    data = MIndwm(mmiobase, 0x1E6E0070);                                                          // V1 removed
//  }while(!(data & 0x00001000));                                                                   // V1 removed
//  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);                                                      // V1 removed
//  MOutdwm(mmiobase, 0x1E6E0050, 0x80000000);                                                      // V1 removed
//  MOutdwm(mmiobase, 0x1E6E0050, 0x00000000);                                                      // V1 removed
//#endif                                        
 
}

void vInitAST2300DRAMReg(_DEVInfo *DevInfo)
{
    AST2300DRAMParam param;
    ULONG ulTemp;
    UCHAR jReg;

    jReg = GetIndexReg(CRTC_PORT, 0xD0, 0xFF);
    if ((jReg & 0x80) == 0)			/* VGA only */
    {
        MOutdwm(DevInfo->pjMMIOLinear, 0x1e6e2000, 0x1688a8a8);
	    do {
	    ulTemp = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e2000);
        } while	 (ulTemp != 0x01);

        MOutdwm(DevInfo->pjMMIOLinear, 0x1e6e0000, 0xfc600309);
	    do {
	    ulTemp = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e0000);
        } while	 (ulTemp != 0x01);

    	/* Slow down CPU/AHB CLK in VGA only mode */
	    ulTemp = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e2008);
        ulTemp |= 0x73;
        MOutdwm(DevInfo->pjMMIOLinear, 0x1e6e2008, ulTemp);

        /* Init DRAM Chip Type */
        param.pjMMIOVirtualAddress = DevInfo->pjMMIOLinear;
        param.DRAM_Type = DDR3;			/* DDR3 */
        ulTemp = MIndwm(param.pjMMIOVirtualAddress, 0x1E6E2070);
        if (ulTemp & 0x01000000)
            param.DRAM_Type = DDR2;		/* DDR2 */
        param.DRAM_ChipID = (ULONG) DevInfo->jDRAMType;
        param.DRAM_Freq = DevInfo->ulMCLK;
        param.VRAM_Size = DevInfo->ulFBSize;

        if (param.DRAM_Type == DDR3)
        {
            GetDDR3Info(&param);
            DDR3_Init(&param);
        }
        else
        {
            GetDDR2Info(&param);
            DDR2_Init(&param);
        }

        ulTemp  = MIndwm(param.pjMMIOVirtualAddress, 0x1E6E2040);
//      MOutdwm(param.pjMMIOVirtualAddress, 0x1E6E2040, ulTemp | 0x40);
        MOutdwm(param.pjMMIOVirtualAddress, 0x1E6E2040, 0xC0);
    }

    /* wait ready */
    do {
        jReg = GetIndexReg(CRTC_PORT, 0xD0, 0xFF);
    } while ((jReg & 0x40) == 0);

} /* vInitAST2300DRAMReg */

//
// AST2500 DRAM Settings
//
//#define TIMEOUT              50000

#define REGTBL_NUM           17
#define REGIDX_010           0
#define REGIDX_014           1
#define REGIDX_018           2
#define REGIDX_020           3
#define REGIDX_024           4
#define REGIDX_02C           5
#define REGIDX_030           6
#define REGIDX_214           7
#define REGIDX_2E0           8
#define REGIDX_2E4           9
#define REGIDX_2E8           10
#define REGIDX_2EC           11
#define REGIDX_2F0           12
#define REGIDX_2F4           13
#define REGIDX_2F8           14
#define REGIDX_RFC           15
#define REGIDX_PLL           16

ULONG ddr3_1600_timing_table[REGTBL_NUM] = {
0x64604D38,                  // 0x010
0x29690599,                  // 0x014
0x00000300,                  // 0x018
0x00000000,                  // 0x020
0x00000000,                  // 0x024
0x02181E70,                  // 0x02C
0x00000040,                  // 0x030
0x00000024,                  // 0x214
0x02001300,                  // 0x2E0
0x0E0000A0,                  // 0x2E4
0x000E001B,                  // 0x2E8
0x35B8C105,                  // 0x2EC
0x08090408,                  // 0x2F0
0x9B000800,                  // 0x2F4
0x0E400A00,                  // 0x2F8
0x9971452F,                  // tRFC
0x000071C1};                 // PLL

ULONG ddr4_1600_timing_table[REGTBL_NUM] = {
0x63604E37,                  // 0x010
0xE97AFA99,                  // 0x014
0x00019000,                  // 0x018
0x08000000,                  // 0x020
0x00000400,                  // 0x024
0x00000410,                  // 0x02C
0x00000101,                  // 0x030
0x00000024,                  // 0x214
0x03002900,                  // 0x2E0
0x0E0000A0,                  // 0x2E4
0x000E001C,                  // 0x2E8
0x35B8C106,                  // 0x2EC
0x08080607,                  // 0x2F0
0x9B000900,                  // 0x2F4
0x0E400A00,                  // 0x2F8
0x99714545,                  // tRFC
0x000071C1};                 // PLL

int MMCTestBurst_AST2500(_DEVInfo *DevInfo, ULONG datagen)
{
  ULONG data, timecnt;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0070, 0x000000C1 | (datagen << 3));
  timecnt = 0;
  do{
    data = MIndwm(mmiobase, 0x1E6E0070) & 0x3000;
    if(data & 0x2000){
      return FALSE;
    }
    if(++timecnt > TIMEOUT){
      //printf("Timeout!!\n");
      MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
      return FALSE;
    }
  }while(!data);
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  return TRUE;
}

int MMCTestSingle_AST2500(_DEVInfo *DevInfo, ULONG datagen)
{
  ULONG data, timecnt;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000085 | (datagen << 3));
  timecnt = 0;
  do{
    data = MIndwm(mmiobase, 0x1E6E0070) & 0x3000;
    if(data & 0x2000){
      return(0);
    }
    if(++timecnt > TIMEOUT){
      //printf("Timeout!!\n");
      MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
      return(0);
    }
  }while(!data);
  MOutdwm(mmiobase, 0x1E6E0070, 0x00000000);
  return(1);
}

ULONG CBRTest_AST2500(_DEVInfo *DevInfo)
{
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  MOutdwm(mmiobase, 0x1E6E0074, 0x0000FFFF);
  MOutdwm(mmiobase, 0x1E6E007C, 0xFF00FF00);
  if(!MMCTestBurst_AST2500(DevInfo, 0)) return FALSE;
  if(!MMCTestSingle_AST2500(DevInfo, 0)) return FALSE;
  return TRUE;
}

void DDR_Init_Common(_DEVInfo *DevInfo)
{
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;
	
  MOutdwm(mmiobase, 0x1E6E0034,0x00020080);
  MOutdwm(mmiobase, 0x1E6E0008,0x2003000F);
  MOutdwm(mmiobase, 0x1E6E0038,0x00000FFF);
  MOutdwm(mmiobase, 0x1E6E0040,0x88448844);
  MOutdwm(mmiobase, 0x1E6E0044,0x24422288);
  MOutdwm(mmiobase, 0x1E6E0048,0x22222222);
  MOutdwm(mmiobase, 0x1E6E004C,0x22222222);
  MOutdwm(mmiobase, 0x1E6E0050,0x80000000);
  MOutdwm(mmiobase, 0x1E6E0208,0x00000000);
  MOutdwm(mmiobase, 0x1E6E0218,0x00000000);
  MOutdwm(mmiobase, 0x1E6E0220,0x00000000);
  MOutdwm(mmiobase, 0x1E6E0228,0x00000000);
  MOutdwm(mmiobase, 0x1E6E0230,0x00000000);
  MOutdwm(mmiobase, 0x1E6E02A8,0x00000000);
  MOutdwm(mmiobase, 0x1E6E02B0,0x00000000);
  MOutdwm(mmiobase, 0x1E6E0240,0x86000000);
  MOutdwm(mmiobase, 0x1E6E0244,0x00008600);
  MOutdwm(mmiobase, 0x1E6E0248,0x80000000);
  MOutdwm(mmiobase, 0x1E6E024C,0x80808080);
}

void Do_DDRPHY_Init(_DEVInfo *DevInfo)
{
  ULONG data, pass, timecnt;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  pass = 0;
  MOutdwm(mmiobase, 0x1E6E0060,0x00000005);
  while(!pass){
    for(timecnt = 0;timecnt < TIMEOUT;timecnt++){
      data = MIndwm(mmiobase, 0x1E6E0060) & 0x1;
      if(!data){
        break;
      }
    }
    if(timecnt != TIMEOUT){
      data = MIndwm(mmiobase, 0x1E6E0300) & 0x000A0000;
      if(!data){
        pass = 1;
      }
    }
    if(!pass){
      MOutdwm(mmiobase, 0x1E6E0060,0x00000000);
      udelay(10); // delay 10 us
      MOutdwm(mmiobase, 0x1E6E0060,0x00000005);
    }
  }

  MOutdwm(mmiobase, 0x1E6E0060,0x00000006);
}

/******************************************************************************
 Check DRAM Size
 1Gb : 0x80000000 ~ 0x87FFFFFF
 2Gb : 0x80000000 ~ 0x8FFFFFFF
 4Gb : 0x80000000 ~ 0x9FFFFFFF
 8Gb : 0x80000000 ~ 0xBFFFFFFF
 *****************************************************************************/
void Check_DRAM_Size(_DEVInfo *DevInfo, ULONG tRFC)
{
  ULONG reg_04, reg_14;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  //printf("Check DRAM Size...");

  reg_04 = MIndwm(mmiobase, 0x1E6E0004) & 0xfffffffc;
  reg_14 = MIndwm(mmiobase, 0x1E6E0014) & 0xffffff00;

  MOutdwm(mmiobase, 0xA0100000, 0x41424344);
  MOutdwm(mmiobase, 0x90100000, 0x35363738);
  MOutdwm(mmiobase, 0x88100000, 0x292A2B2C);
  MOutdwm(mmiobase, 0x80100000, 0x1D1E1F10);

  // Check 8Gbit
  if(MIndwm(mmiobase, 0xA0100000) == 0x41424344){
    reg_04 |= 0x03;
    reg_14 |= (tRFC >> 24) & 0xFF;
    //printf("8Gbit\n");
  // Check 4Gbit
  }else if(MIndwm(mmiobase, 0x90100000) == 0x35363738){
    reg_04 |= 0x02;
    reg_14 |= (tRFC >> 16) & 0xFF;
    //printf("4Gbit\n");
  // Check 2Gbit
  }else if(MIndwm(mmiobase, 0x88100000) == 0x292A2B2C){
    reg_04 |= 0x01;
    reg_14 |= (tRFC >> 8) & 0xFF;
    //printf("2Gbit\n");
  }else{
    reg_14 |= tRFC & 0xFF;
    //printf("1Gbit\n");
  }
  MOutdwm(mmiobase, 0x1E6E0004, reg_04);
  MOutdwm(mmiobase, 0x1E6E0014, reg_14);
}

void Enable_Cache(_DEVInfo *DevInfo)
{
  ULONG reg_04, data;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  reg_04 = MIndwm(mmiobase, 0x1E6E0004);
  MOutdwm(mmiobase, 0x1E6E0004, reg_04 | 0x1000);

  do{
    data = MIndwm(mmiobase, 0x1E6E0004);
  }while(!(data & 0x80000));
  MOutdwm(mmiobase, 0x1E6E0004, reg_04 | 0x400);
}

///////////////////////////////////////////////////////////////////////////////
// Set MPLL
///////////////////////////////////////////////////////////////////////////////
void Set_MPLL(_DEVInfo *DevInfo)
{
  ULONG addr, data, param;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  // Reset MMC
  MOutdwm(mmiobase, 0x1E6E0000,0xFC600309);
  MOutdwm(mmiobase, 0x1E6E0034,0x00020080);
  for(addr = 0x1e6e0004;addr < 0x1e6e0090;){
    MOutdwm(mmiobase, addr, 0x0);
    addr += 4;
  }
  MOutdwm(mmiobase, 0x1E6E0034,0x00020000);

  MOutdwm(mmiobase, 0x1E6E2000, 0x1688A8A8);
  data = MIndwm(mmiobase, 0x1E6E2070) & 0x00800000;
  if(data){                  // CLKIN = 25MHz
    //printf("CLKIN = 25MHz\n");
    param = 0x930023E0;
  }else{
    //printf("CLKIN = 24MHz\n");
    param = 0x93002400;
  }
  MOutdwm(mmiobase, 0x1E6E2020, param);
  udelay(100); // delay 3 ms
}

///////////////////////////////////////////////////////////////////////////////
void DDR3_Init_AST2500(_DEVInfo *DevInfo, ULONG *ddr_table)
{
  //ULONG data, data2;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  //printf("DDR3 Init.....\n");
  MOutdwm(mmiobase, 0x1E6E0004,0x00000303);
  MOutdwm(mmiobase, 0x1E6E0010,ddr_table[REGIDX_010]);
  MOutdwm(mmiobase, 0x1E6E0014,ddr_table[REGIDX_014]);
  MOutdwm(mmiobase, 0x1E6E0018,ddr_table[REGIDX_018]);
  MOutdwm(mmiobase, 0x1E6E0020,ddr_table[REGIDX_020]);          // MODEREG4/6
  MOutdwm(mmiobase, 0x1E6E0024,ddr_table[REGIDX_024]);          // MODEREG5
  MOutdwm(mmiobase, 0x1E6E002C,ddr_table[REGIDX_02C] | 0x100);  // MODEREG0/2
  MOutdwm(mmiobase, 0x1E6E0030,ddr_table[REGIDX_030]);          // MODEREG1/3

  // DDR PHY Setting
  //printf("Config DDRPHY\n");
  MOutdwm(mmiobase, 0x1E6E0200,0x02492AAE);
  MOutdwm(mmiobase, 0x1E6E0204,0x00001001);
  MOutdwm(mmiobase, 0x1E6E020C,0x55E00B0B);
  MOutdwm(mmiobase, 0x1E6E0210,0x20000000);
  MOutdwm(mmiobase, 0x1E6E0214,ddr_table[REGIDX_214]);
  MOutdwm(mmiobase, 0x1E6E02E0,ddr_table[REGIDX_2E0]);
  MOutdwm(mmiobase, 0x1E6E02E4,ddr_table[REGIDX_2E4]);
  MOutdwm(mmiobase, 0x1E6E02E8,ddr_table[REGIDX_2E8]);
  MOutdwm(mmiobase, 0x1E6E02EC,ddr_table[REGIDX_2EC]);
  MOutdwm(mmiobase, 0x1E6E02F0,ddr_table[REGIDX_2F0]);
  MOutdwm(mmiobase, 0x1E6E02F4,ddr_table[REGIDX_2F4]);
  MOutdwm(mmiobase, 0x1E6E02F8,ddr_table[REGIDX_2F8]);
  MOutdwm(mmiobase, 0x1E6E0290,0x00100008);
  MOutdwm(mmiobase, 0x1E6E02C0,0x00000006);

  // Controller Setting
  MOutdwm(mmiobase, 0x1E6E0034,0x00020091);

  // Wait DDR PHY init done
  Do_DDRPHY_Init(DevInfo);

  MOutdwm(mmiobase, 0x1E6E0120,ddr_table[REGIDX_PLL]);
  MOutdwm(mmiobase, 0x1E6E000C,0x42AA5C81);
  MOutdwm(mmiobase, 0x1E6E0034,0x0001AF93);

  Check_DRAM_Size(DevInfo, ddr_table[REGIDX_RFC]);
  Enable_Cache(DevInfo);
  MOutdwm(mmiobase, 0x1E6E001C,0x00000008);
  MOutdwm(mmiobase, 0x1E6E0038,0xFFFFFF00);
}

///////////////////////////////////////////////////////////////////////////////
void DDR4_Init_AST2500(_DEVInfo *DevInfo, ULONG *ddr_table)
{
  ULONG  data, data2,/*timecnt,*/ pass/*, cnt*/;
  ULONG ddr_vref, phy_vref;
  ULONG min_ddr_vref, min_phy_vref;
  ULONG max_ddr_vref, max_phy_vref;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  //printf("DDR4 Init.....\n");
  MOutdwm(mmiobase, 0x1E6E0004,0x00000313);
  MOutdwm(mmiobase, 0x1E6E0010,ddr_table[REGIDX_010]);
  MOutdwm(mmiobase, 0x1E6E0014,ddr_table[REGIDX_014]);
  MOutdwm(mmiobase, 0x1E6E0018,ddr_table[REGIDX_018]);
  MOutdwm(mmiobase, 0x1E6E0020,ddr_table[REGIDX_020]);          // MODEREG4/6
  MOutdwm(mmiobase, 0x1E6E0024,ddr_table[REGIDX_024]);          // MODEREG5
  MOutdwm(mmiobase, 0x1E6E002C,ddr_table[REGIDX_02C] | 0x100);  // MODEREG0/2
  MOutdwm(mmiobase, 0x1E6E0030,ddr_table[REGIDX_030]);          // MODEREG1/3

  // DDR PHY Setting
  //printf("Config DDRPHY\n");
  MOutdwm(mmiobase, 0x1E6E0200,0x42492AAE);
  MOutdwm(mmiobase, 0x1E6E0204,0x09002000);
  MOutdwm(mmiobase, 0x1E6E020C,0x55E00B0B);
  MOutdwm(mmiobase, 0x1E6E0210,0x20000000);
  MOutdwm(mmiobase, 0x1E6E0214,ddr_table[REGIDX_214]);
  MOutdwm(mmiobase, 0x1E6E02E0,ddr_table[REGIDX_2E0]);
  MOutdwm(mmiobase, 0x1E6E02E4,ddr_table[REGIDX_2E4]);
  MOutdwm(mmiobase, 0x1E6E02E8,ddr_table[REGIDX_2E8]);
  MOutdwm(mmiobase, 0x1E6E02EC,ddr_table[REGIDX_2EC]);
  MOutdwm(mmiobase, 0x1E6E02F0,ddr_table[REGIDX_2F0]);
  MOutdwm(mmiobase, 0x1E6E02F4,ddr_table[REGIDX_2F4]);
  MOutdwm(mmiobase, 0x1E6E02F8,ddr_table[REGIDX_2F8]);
  MOutdwm(mmiobase, 0x1E6E0290,0x00100008);
  MOutdwm(mmiobase, 0x1E6E02C4,0x3C183C3C);
  MOutdwm(mmiobase, 0x1E6E02C8,0x00631E0E);

  // Controller Setting
  MOutdwm(mmiobase, 0x1E6E0034,0x0001A991);

  // Train PHY Vref first
  max_phy_vref = 0x0;
  pass = 0;
  MOutdwm(mmiobase, 0x1E6E02C0,0x00001C06);
  for(phy_vref = 0x40;phy_vref < 0x80;phy_vref++){
    MOutdwm(mmiobase, 0x1E6E000C,0x00000000);
    MOutdwm(mmiobase, 0x1E6E0060,0x00000000);
    MOutdwm(mmiobase, 0x1E6E02CC,phy_vref | (phy_vref << 8));
    // Fire DFI Init
    Do_DDRPHY_Init(DevInfo);
    MOutdwm(mmiobase, 0x1E6E000C,0x00005C01);
    if(CBRTest_AST2500(DevInfo)){
      pass++;
      data = MIndwm(mmiobase, 0x1E6E03D0);
      data2 = data >> 8;
      data  = data & 0xff;
      if(data > data2){
        data = data2;
      }

      if(max_phy_vref < data){
        max_phy_vref = data;
        min_phy_vref = phy_vref;
      }
    }else if(pass > 0){
      break;
    }
  }
  MOutdwm(mmiobase, 0x1E6E02CC,min_phy_vref | (min_phy_vref << 8));
  //printf("Target PHY Vref = %x\n",min_phy_vref);

  // Train DDR Vref next
  min_ddr_vref = 0xFF;
  max_ddr_vref = 0x0;
  pass = 0;
  for(ddr_vref = 0x00;ddr_vref < 0x40;ddr_vref++){
    MOutdwm(mmiobase, 0x1E6E000C,0x00000000);
    MOutdwm(mmiobase, 0x1E6E0060,0x00000000);
    MOutdwm(mmiobase, 0x1E6E02C0,0x00000006 | (ddr_vref << 8));
    // Fire DFI Init
    Do_DDRPHY_Init(DevInfo);
    MOutdwm(mmiobase, 0x1E6E000C,0x00005C01);
    if(CBRTest_AST2500(DevInfo)){
      pass++;
      if(min_ddr_vref > ddr_vref){
        min_ddr_vref = ddr_vref;
      }
      if(max_ddr_vref < ddr_vref){
        max_ddr_vref = ddr_vref;
      }
    }else if(pass != 0){
      break;
    }
  }
  MOutdwm(mmiobase, 0x1E6E000C,0x00000000);
  MOutdwm(mmiobase, 0x1E6E0060,0x00000000);
  ddr_vref = (min_ddr_vref + max_ddr_vref + 1) >> 1;
  MOutdwm(mmiobase, 0x1E6E02C0,0x00000006 | (ddr_vref << 8));
  //printf("Target DDR Vref = %x\n",ddr_vref);

  // Wait DDR PHY init done
  Do_DDRPHY_Init(DevInfo);

  MOutdwm(mmiobase, 0x1E6E0120,ddr_table[REGIDX_PLL]);
  MOutdwm(mmiobase, 0x1E6E000C,0x42AA5C81);
  MOutdwm(mmiobase, 0x1E6E0034,0x0001AF93);

  Check_DRAM_Size(DevInfo, ddr_table[REGIDX_RFC]);
  Enable_Cache(DevInfo);
  MOutdwm(mmiobase, 0x1E6E001C,0x00000008);
  MOutdwm(mmiobase, 0x1E6E0038,0xFFFFFF00);
}

///////////////////////////////////////////////////////////////////////////////
int DRAM_Init_AST2500(_DEVInfo *DevInfo)
{
  ULONG data;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;
  
#if 0  
  data = MIndwm(mmiobase, 0x1E6E2070);
  // Check if ARM/ColdfireV1 disabled, if not then delay 300 ms
  if(!(data & 0x01)){
    Delay(300);
  }
  data = MIndwm(mmiobase, 0x1E6E2040);
  // Check if BMC had initialized the DRAM
  if(data & 0x80){
    return(0);
  }

  // Disable watchdog
  MOutdwm(mmiobase, 0x1E78502C,0x00000000);
  MOutdwm(mmiobase, 0x1E78504C,0x00000000);
#endif

  Set_MPLL(DevInfo);
  DDR_Init_Common(DevInfo);
  data = MIndwm(mmiobase, 0x1E6E2070);
  if(data & 0x01000000){
    DDR4_Init_AST2500(DevInfo, ddr4_1600_timing_table);
  }else{
    DDR3_Init_AST2500(DevInfo, ddr3_1600_timing_table);
  }
  MOutdwm(mmiobase, 0x1E6E2040, MIndwm(mmiobase, 0x1E6E2040) | 0x41);
  // Patch code
  data = MIndwm(mmiobase, 0x1E6E200C) & 0xF9FFFFFF;
  MOutdwm(mmiobase, 0x1E6E200C, data | 0x10000000);
  return TRUE;
}

void  vInitAST2500DRAMReg(_DEVInfo *DevInfo)
{
    ULONG ulTemp;
    UCHAR jReg;
    UCHAR *mmiobase = DevInfo->pjMMIOLinear;

    //a7printf("vInitAST2500DRAMReg Start\n");
    
    jReg = GetIndexReg(CRTC_PORT, 0xD0, 0xFF);
    if ((jReg & 0x80) == 0)			/* VGA only */
    {
        MOutdwm(mmiobase, 0x1e6e2000, 0x1688a8a8);
	    do {
	    ulTemp = MIndwm(mmiobase, 0x1e6e2000);
        } while	 (ulTemp != 0x01);

        MOutdwm(mmiobase, 0x1e6e0000, 0xfc600309);
	    do {
	    ulTemp = MIndwm(mmiobase, 0x1e6e0000);
        } while	 (ulTemp != 0x01);

    	/* Slow down CPU/AHB CLK in VGA only mode */
	    ulTemp = MIndwm(mmiobase, 0x1e6e2008);
        ulTemp |= 0x73;
        MOutdwm(mmiobase, 0x1e6e2008, ulTemp);

		DRAM_Init_AST2500(DevInfo);

        ulTemp  = MIndwm(mmiobase, 0x1E6E2040);
//      MOutdwm(mmiobase, 0x1E6E2040, ulTemp | 0x40);
        MOutdwm(mmiobase, 0x1E6E2040, 0xC0);
    }

    /* wait ready */
    do {
        jReg = GetIndexReg(CRTC_PORT, 0xD0, 0xFF);
    } while ((jReg & 0x40) == 0);

    //a7printf("vInitAST2500DRAMReg End\n");
} /* vInitAST2500DRAMReg */

/*
 * Export
 */
void GetVRAMInfo(_DEVInfo *DevInfo)
{
    UCHAR jReg;

    /* Get Framebuffer Size */
    jReg = GetIndexReg(CRTC_PORT, 0xAA, 0xFF);
    switch (jReg & 0x03)
    {
    case 0x03:
        DevInfo->ulFBSize = VRAM_SIZE_064M;
        break;
    case 0x02:
        DevInfo->ulFBSize = VRAM_SIZE_032M;
        break;
    case 0x01:
        DevInfo->ulFBSize = VRAM_SIZE_016M;
        break;
    default:
        DevInfo->ulFBSize = VRAM_SIZE_008M;
        break;
    }
} /* GetVRAMInfo */

void GetDRAMInfo(_DEVInfo *DevInfo)
{
    ULONG ulData;

    if (DevInfo->jRevision >= 0x20)
    {
    	DevInfo->jDRAMType      = DRAMTYPE_1Gx16;
    	DevInfo->ulMCLK         = 396;
        DevInfo->ulDRAMBusWidth = 16;

        //Marks for auto-sizing, ycchen@030614
#if 0        
        ulData = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e2070);
    	switch (ulData & 0x18000000)	//D[28:27]
    	{
    	case 0x00000000:
    	    DevInfo->jDRAMType = DRAMTYPE_512Mx16;
    	    break;
    	case 0x08000000:
    	    DevInfo->jDRAMType = DRAMTYPE_1Gx16;
    	    break;
    	case 0x10000000:
    	    DevInfo->jDRAMType = DRAMTYPE_2Gx16;
    	    break;
    	case 0x18000000:
    	    DevInfo->jDRAMType = DRAMTYPE_4Gx16;
    	    break;
	    }
#endif	    
    }
    else if (DevInfo->jRevision >= 0x10)
    {
    	DevInfo->jDRAMType      = DRAMTYPE_1Gx16;
    	DevInfo->ulMCLK         = 266;
        DevInfo->ulDRAMBusWidth = 32;
        ulData = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e207c);
        if ((ulData & 0x0300) == 0x0200)	/* AST1100/2050/2150 */
        {
    	    DevInfo->ulMCLK         = 200;
            DevInfo->ulDRAMBusWidth = 16;
        }
    }
    else	/* AST2000 */
    {
    	DevInfo->jDRAMType      = DRAMTYPE_512Mx32;
    	DevInfo->ulMCLK         = 166;
        DevInfo->ulDRAMBusWidth = 32;
    }

} /* GetDRAMInfo */

//void vInitDVO(_DEVInfo *DevInfo)
//{
//    UCHAR *mmiobase = (UCHAR *) DevInfo->pjMMIOLinear;	
//    ULONG ulData;
//    UCHAR jReg;
// 
//    jReg = GetIndexReg(CRTC_PORT, 0xD0, 0xFF);
//    if ((jReg & 0x80) == 0)			/* VGA only */
//    {
//        MOutdwm(mmiobase, 0x1E6E2000, 0x1688a8a8);
//	         
//        ulData  = MIndwm(mmiobase, 0x1E6E2078);	/* DVO, single-edge */
//        MOutdwm(mmiobase, 0x1E6E2078, ulData | 0x03);
//
//        ulData  = MIndwm(mmiobase, 0x1E6E2074);	/* enable DVO pins */
//        MOutdwm(mmiobase, 0x1E6E2074, ulData | 0x00c10000);
//    }   
// 
//} /* vInitDVO */
//
//void vInitAST2300DVO(_DEVInfo *DevInfo)
//{
//    UCHAR *mmiobase = (UCHAR *) DevInfo->pjMMIOLinear;	
//    ULONG ulData;
//    UCHAR jReg;
// 
//    jReg = GetIndexReg(CRTC_PORT, 0xD0, 0xFF);
//    if ((jReg & 0x80) == 0)			/* VGA only */
//    {  
//        MOutdwm(mmiobase, 0x1E6E2000, 0x1688a8a8);
//     
//        ulData  = MIndwm(mmiobase, 0x1E6E2004);
//        ulData &= 0xFFFFFFFB;
//        MOutdwm(mmiobase, 0x1E6E2004, ulData);
//    
//        ulData  = MIndwm(mmiobase, 0x1E6E2008);	/* delay compension */
//        ulData &= 0xFFFFF0FF;
//        ulData |= 0x00000c00;
//        MOutdwm(mmiobase, 0x1E6E2008, ulData);
//    
//        ulData  = MIndwm(mmiobase, 0x1E6E2084);	/* multi-pins */
//        MOutdwm(mmiobase, 0x1E6E2084, ulData | 0xff3a0000);
//    
//        ulData  = MIndwm(mmiobase, 0x1E6E2088);	/* multi-pins */
//        MOutdwm(mmiobase, 0x1E6E2088, ulData | 0x000fffff);
//    
//        ulData  = MIndwm(mmiobase, 0x1E6E2090);	/* DVO, single-edge */
//        MOutdwm(mmiobase, 0x1E6E2090, ulData | 0x20);
//    
//        ulData  = MIndwm(mmiobase, 0x1E6E202c);	/* select DVO source */
//        MOutdwm(mmiobase, 0x1E6E202c, ulData & 0xfff8ffff);
//    }
//    
//} /* vInitAST2300DVO */
//		
//void vInitAST2400DVO(_DEVInfo *DevInfo)
//{
//    UCHAR *mmiobase = (UCHAR *) DevInfo->pjMMIOLinear;	
//    ULONG ulData;
//    UCHAR jReg;
//
//    jReg = GetIndexReg(CRTC_PORT, 0xD0, 0xFF);
//    if ((jReg & 0x80) == 0)			/* VGA only */
//    {     
//        MOutdwm(mmiobase, 0x1E6E2000, 0x1688a8a8);
//     
//        ulData  = MIndwm(mmiobase, 0x1E6E2004);
//        ulData &= 0xFFFFFFFB;
//        MOutdwm(mmiobase, 0x1E6E2004, ulData);
//   
//        ulData  = MIndwm(mmiobase, 0x1E6E2008);	/* delay compension */
//        ulData &= 0xFFFFF0FF;
//        ulData |= 0x00000c00;
//       MOutdwm(mmiobase, 0x1E6E2008, ulData);
//    
//        ulData  = MIndwm(mmiobase, 0x1E6E2088);	/* multi-pins */
//        MOutdwm(mmiobase, 0x1E6E2088, ulData | 0x30000000);
//    
//        ulData  = MIndwm(mmiobase, 0x1E6E208c);	/* multi-pins */
//        MOutdwm(mmiobase, 0x1E6E208c, ulData | 0xcf);
//    
//        ulData  = MIndwm(mmiobase, 0x1E6E20a4);	/* multi-pins */
//        MOutdwm(mmiobase, 0x1E6E20a4, ulData | 0xffff0000);
//    
//        ulData  = MIndwm(mmiobase, 0x1E6E20a8);	/* multi-pins */
//        MOutdwm(mmiobase, 0x1E6E20a8, ulData | 0xf);
//    
//        ulData  = MIndwm(mmiobase, 0x1E6E2094);	/* DVO, single-edge */
//        MOutdwm(mmiobase, 0x1E6E2094, ulData | 0x02);
//    
//        ulData  = MIndwm(mmiobase, 0x1E6E202c);	/* select DVO source */
//        MOutdwm(mmiobase, 0x1E6E202c, ulData & 0xfffbffff);
//    }
//    
//} /* vInitAST2400DVO */
/*
 * DP501
 */
//
//LONG GetFileSize (FILE *fp)
//{
//    LONG save_pos, filesize;
//    
//    save_pos = ftell(fp);
//    fseek(fp, 0L, SEEK_END);
//    filesize = ftell(fp);
//    fseek(fp, save_pos, SEEK_SET);
//    return (filesize);	
//}	
//BOOL LoadM68K(_DEVInfo *DevInfo, CHAR *FW)
//{
//    UCHAR *mmiobase = (UCHAR *) DevInfo->pjMMIOLinear;	
//    ULONG Data, BootAddress;
//    UCHAR jReg;
//    	
//    FILE *fp;
//    LONG i, j, lFileSize;
//    UCHAR ch[4];
// 
//    /* open file */
//    if ((fp=fopen(FW,"rb"))==NULL)
//    {
//        printf("Open FW Image Failed \n");
//        return FALSE;
//    }   
//    
//    /* check DP501 */    
//    if (!DP501_Find(DevInfo))
//    {
//	    printf("Cannot find DP501\n");    
//	    return FALSE;
//    }    
//
//    jReg = GetIndexReg(CRTC_PORT, 0xD1, 0x0E);
//    if (jReg != 0x0C)	/* VBIOS handle DP501 */
//    {         
//        /* Reset Co-processor */
//        MOutdwm(mmiobase, 0x1e6e2100, 0x03);        	    
//        do {
//            Data = MIndwm(mmiobase, 0x1e6e2100);        
//        } while (Data != 0x03);   
//    			        
//        /* Get BootAddress */
//        MOutdwm(mmiobase, 0x1e6e2000, 0x1688a8a8);	// open passwd
//        Data = MIndwm(mmiobase, 0x1e6e0004);    
//        switch (Data & 0x03)
//        {
//	    case 0x00:	/* 64M */
//	        BootAddress = 0x44000000;
//	        break;
//	    default:    
//	    case 0x01:	/* 128MB */
//	        BootAddress = 0x48000000;
//	        break;	
//	    case 0x02:	/* 256MB */
//	        BootAddress = 0x50000000;
//	        break;	
//	    case 0x03:	/* 512MB */
//	        BootAddress = 0x60000000;
//	        break;	    
//        }
//        BootAddress -= 0x200000;	/* - 2MB */
//	
//        /* copy image to buffer */
//        lFileSize = (ULONG) GetFileSize(fp) - 0;
//        fseek(fp, 0, SEEK_SET);
//        for (i=0; i<lFileSize; i+=4)
//        {
//	        for (j=0; j<4; j++)    
//                ch[j] = getc(fp);
//            MOutdwm(mmiobase, BootAddress + i, *(ULONG *)(ch));                
//        }
//             
//        /* Init SCU */	
//        MOutdwm(mmiobase, 0x1e6e2000, 0x1688a8a8);	// open passwd
//         
//        /* Launch FW */
//        MOutdwm(mmiobase, 0x1e6e2104, 0x80000000 + BootAddress);                    	            	                                                            
//        MOutdwm(mmiobase, 0x1e6e2100, 1);
//        
//        /* Update Scratch */
//        Data  = MIndwm(mmiobase, 0x1e6e2040) & 0xFFFFF1FF;			//D[11:9] = 100b: UEFI handling
//        Data |= 0x0800;
//        MOutdwm(mmiobase, 0x1e6e2040, Data);
//
//        jReg  = GetIndexReg(CRTC_PORT, 0x99, 0xFC);					//D[1:0]: Reserved Video Buffer
//        jReg |= 0x02;												//2MB
//        SetIndexReg(CRTC_PORT, 0x99, 0x00, jReg);
//                                             	            	                                                            
//    } /* UEFI Driver Handling */   	    
//        
//    fclose(fp);
//    
//    return TRUE;
//} /* LoadM68K */
//	
//BOOL UnloadM68K(_DEVInfo *DevInfo)
//{
//    UCHAR *mmiobase = (UCHAR *) DevInfo->pjMMIOLinear;	
//	
//    /* Reset Co-processor */
//    MOutdwm(mmiobase, 0x1e6e2100, 0x03);
//    
//    /* check DP501 */    
//    if (DP501_Find(DevInfo))
//    {
//        Disable_Video_Output(DevInfo);
//        DisplayOFF(DevInfo);
//    }    
//    
//    return TRUE;    	
//}
// 
	
/*
 * InitVGA
 */
VOID InitVGA_HW(_DEVInfo *DevInfo, ULONG InitOption)
{
    ast_ExtRegInfo *ExtReg;
    int	i;

    if (InitOption & Init_IO)
    {
        enable_vga(DevInfo);
        open_key(DevInfo);

        for (i=0x81; i<0x9E; i++)
            SetIndexReg2(CRTC_PORT, (UCHAR) i, 0x00);
        
        ExtReg = astExtReg;
        if (DevInfo->jRevision >= 0x20)
            ExtReg = astExtReg_AST2300;
        do {
            SetIndexReg2(CRTC_PORT,ExtReg->jIndex, ExtReg->jData);
            ExtReg++;
        } while (ExtReg->jIndex != 0xFF);
        
        if (DevInfo->jRevision == 0x40)	//for AST2500a0
    	{
            SetIndexReg2(CRTC_PORT,0xbc, 0x40);
            SetIndexReg2(CRTC_PORT,0xbd, 0x38);
            SetIndexReg2(CRTC_PORT,0xbe, 0x3a);
            SetIndexReg2(CRTC_PORT,0xbf, 0x38);
            SetIndexReg2(CRTC_PORT,0xcf, 0x70);
            SetIndexReg2(CRTC_PORT,0xb5, 0xa8);
            SetIndexReg2(CRTC_PORT,0xbb, 0x43);
    	}
    }

    /* Dram Init */
    if (InitOption & Init_MEM)
    {
        if (DevInfo->jRevision >= 0x40){
            vInitAST2500DRAMReg(DevInfo);
        }else if (DevInfo->jRevision >= 0x20){
            vInitAST2300DRAMReg(DevInfo);
        }else
            vInitDRAMReg(DevInfo);
    }

    /* DVO Init */
//    if (InitOption & Init_DVO)
//    {
//        if (DevInfo->jRevision >= 0x30)
//            vInitAST2400DVO(DevInfo);
//        else if (DevInfo->jRevision >= 0x20)
//            vInitAST2300DVO(DevInfo);            
//   	    else
//            vInitDVO(DevInfo);   	    
//    }	    
//
//    /* DVO Init */
//    if (InitOption & Init_DP501)
//    {
//	    LoadM68K(DevInfo, "boot.bin"); 
//    }    
	   
} /* InitVGA_HW */
