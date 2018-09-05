/**
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
**/
/** @file
  Serial I/O Port library functions with no library constructor/destructor

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express written
  consent of Intel Corporation.

  Module Name:  SerialPortLib.c

**/

#include <Base.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/SerialPortLib.h>
#include <Library/DebugLib.h>

// #include <PchRegs.h>
// #include <PchRegs\PchRegsLpc.h>
#include <IchRegs.h>
#include <SioRegs.h>
#include "Ns16550.h"

#include <Platform.h>
#include <PchAccess.h>


typedef struct {
  UINT8   Index;
  UINT8   Data;
} SIO_REG_TABLE;


STATIC  SIO_REG_TABLE  mASPEED2500Table [] = {
  { REG_LOGICAL_DEVICE,            ASPEED2500_SIO_UART1 },
  { ACTIVATE,                   0x00} ,   
  { BASE_ADDRESS_HIGH0,        (UINT8)(COM1_BASE >> 8) },
  { BASE_ADDRESS_LOW0,         (UINT8)(COM1_BASE & 0xFF) },
  { PRIMARY_INTERRUPT_SELECT,   0x04 },
  { ACTIVATE,                   0x01}
};

STATIC  SIO_REG_TABLE  mPilotIVTable [] = {
  { PILOTIV_LOGICAL_DEVICE,            PILOTIV_SIO_COM1 },
  { PILOTIV_BASE_ADDRESS_HIGH0,        (UINT8)(COM1_BASE >> 8) },
  { PILOTIV_BASE_ADDRESS_LOW0,         (UINT8)(COM1_BASE & 0xFF) },
  { PILOTIV_PRIMARY_INTERRUPT_SELECT,   0x04 },
  { PILOTIV_ACTIVATE,                   0x01}
};

STATIC  SIO_REG_TABLE  mPc8374Table [] = {  
  { PC8374_LOGICAL_DEVICE,        PC8374_SIO_COM1 },
  { PC8374_BASE_ADDRESS_HIGH0,    (UINT8)(COM1_BASE >> 8) },
  { PC8374_BASE_ADDRESS_LOW0,     (UINT8)(COM1_BASE & 0xFF) },
  { WAKEUP_ON_IRQ_EN,             0x04},
  { PC8374_ACTIVATE,              0x01},
  { PC8374_CLOCK_SELECT,          0},
  { PC8374_CLOCK_CONFIG,          0x80}
};

STATIC  SIO_REG_TABLE  mNCT5104DTable [] = {  
  { NCT5104D_LOGICAL_DEVICE,        NCT5104D_SIO_COM1 },
  { NCT5104D_ACTIVATE,              0x00},
  { NCT5104D_BASE_ADDRESS_HIGH0,    (UINT8)(COM1_BASE >> 8) },
  { NCT5104D_BASE_ADDRESS_LOW0,     (UINT8)(COM1_BASE & 0xFF) },
  { NCT5104D_WAKEUP_ON_IRQ_EN,             0x04},
  { NCT5104D_ACTIVATE,              0x01},
  { NCT5104D_LOGICAL_DEVICE,        NCT5104D_SIO_UARTA },
  { NCT5104D_ACTIVATE,              0x00},
};

UINT16 gComBase  = 0x3f8;
UINTN  gBps      = 115200;
UINT8  gData     = 8;
UINT8  gStop     = 1;
UINT8  gParity   = 0;
UINT8  gBreakSet = 0;


/**

    Read Aspeed AHB register.

    @param RegIndex: register index of Aspeed.

    @retval value of register.

**/
UINT32
ReadAHBDword(
  UINT32  RegIndex
){
  UINT8    bValue;
  UINT32   rdValue = 0;

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, REG_LOGICAL_DEVICE); 
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, ASPEED2500_SIO_SMI);  
  IoWrite8 (0xED, 0);//short delay.  

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0x30); 
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, 1);  
  IoWrite8 (0xED, 0);//short delay.  
  
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0xf8);   
  bValue = IoRead8(ASPEED2500_SIO_DATA_PORT);
  bValue &= 0xfc;
  bValue |= 2;  // 4 byte window.  
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, bValue);    
  IoWrite8 (0xED, 0);//short delay.  

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0xf0); 
  IoWrite8 (0xED, 0);//short delay.  
  IoWrite8 (ASPEED2500_SIO_DATA_PORT,  (UINT8)((RegIndex >> 24)& 0xff));   

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0xf1); 
  IoWrite8 (0xED, 0);//short delay.  
  IoWrite8 (ASPEED2500_SIO_DATA_PORT,  (UINT8)((RegIndex >> 16)& 0xff));    

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0xf2); 
  IoWrite8 (0xED, 0);//short delay.  
  IoWrite8 (ASPEED2500_SIO_DATA_PORT,  (UINT8)((RegIndex >> 8) & 0xff));     

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0xf3); 
  IoWrite8 (0xED, 0);//short delay.  
  IoWrite8 (ASPEED2500_SIO_DATA_PORT,  (UINT8)((RegIndex )& 0xff));    

  // trigger read
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0xfe); 
  IoRead8 (ASPEED2500_SIO_DATA_PORT); 


  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0xf4); 
  rdValue += IoRead8 (ASPEED2500_SIO_DATA_PORT); 
  rdValue <<= 8;

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0xf5); 
  rdValue += IoRead8 (ASPEED2500_SIO_DATA_PORT); 
  rdValue <<= 8;  

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0xf6); 
  rdValue += IoRead8 (ASPEED2500_SIO_DATA_PORT); 
  rdValue <<= 8;  

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0xf7); 
  rdValue += IoRead8 (ASPEED2500_SIO_DATA_PORT); 


  return rdValue;

}


/**

    GC_TODO: add routine description

    @param Exist - GC_TODO: add arg description

    @retval RETURN_SUCCESS - GC_TODO: add retval description

**/
UINT32  
IsSioExist (
  VOID
)
{
  //
  // IBMC will decode 0x4E/0x4F
  // 
  UINT32   SioExit;
  UINT32   DeviceID;

  SioExit = 0;
  
  //
  //ASPEED AST2500 
  //
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);  
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);    

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, REG_LOGICAL_DEVICE); 
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, ASPEED2500_SIO_UART1);   

  if(IoRead8(ASPEED2500_SIO_DATA_PORT) == ASPEED2500_SIO_UART1){
#ifdef ASPEED_ENABLE
    DeviceID=0;
    SioExit |= ASPEED2500_EXIST; 
#else     	
    // 
    //right now, maybe it is ASPEED. to detect the  device ID.
    //    
    DeviceID = ReadAHBDword(SCU7C);

    //
    //There is a Aspeed card need to support as well. it's type is AST2500 A1 EVB.
    //

    //AST2300-A0 0x01000003
    //AST2300-A1 0x01010303
    //AST1300-A1 0x01010003
    //AST1050-A1 0x01010203
    //AST2400-A0 0x02000303
    //AST2400-A1 0x02010303
    //AST1400-A1 0x02010103
    //AST1250-A1 0x02010303

    
    //AST2500-A0 0x04000303
    //AST2510-A0 0x04000103
    //AST2520-A0 0x04000203
    //AST2530-A0 0x04000403
    //AST2500-A1 0x04010303
    //AST2510-A1 0x04010103
    //AST2520-A1 0x04010203
    //AST2530-A1 0x04010403
    //
     if((DeviceID & 0xff0000ff) == 0x04000003){   
      SioExit |= ASPEED2500_EXIST;    
    }
#endif    
  }  
  
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_LOCK);    
  
  
  //
  //Pilot 4 will decode 0x2E/0x2F
  // 
  IoWrite8 (PILOTIV_SIO_INDEX_PORT, PILOTIV_SIO_UNLOCK);
  IoWrite8 (PILOTIV_SIO_INDEX_PORT , PILOTIV_CHIP_ID_REG);
  if (IoRead8  (PILOTIV_SIO_DATA_PORT) == PILOTIV_CHIP_ID) {
    SioExit |= PILOTIV_EXIST;
  }
  IoWrite8 (PILOTIV_SIO_INDEX_PORT, PILOTIV_SIO_LOCK);

  //
  // PC SIO will decode 0x2E/0x2F
  //
  IoWrite8 (PC8374_SIO_INDEX_PORT, PC8374_LOGICAL_DEVICE);
  IoWrite8 (PC8374_SIO_DATA_PORT , PC8374_SIO_COM1);
  if (IoRead8  (PC8374_SIO_DATA_PORT) == PC8374_SIO_COM1) {
    SioExit |= PC8374_EXIST;
  }

  //
  // Nuvoton NCT5104D
  //
  IoWrite8 (NCT5104D_SIO_INDEX_PORT, NCT5104D_ENTER_THE_EXTENDED_FUNCTION_MODE);
  IoWrite8 (NCT5104D_SIO_INDEX_PORT, NCT5104D_ENTER_THE_EXTENDED_FUNCTION_MODE);

  IoWrite8 (NCT5104D_SIO_INDEX_PORT , NCT5104D_CHIP_ID_REG);
  if (IoRead8 (NCT5104D_SIO_DATA_PORT) == NCT5104D_CHIP_ID) {
    SioExit = NCT5104D_EXIST;
  }
// APTIOV_SERVER_OVERRIDE_RC_START : Hang is observed with this call in SECCORE
//  DEBUG((EFI_D_ERROR, "[SIO] Current system SIO exist bit:%x \n", SioExit));
// APTIOV_SERVER_OVERRIDE_RC_END : Hang is observed with this call in SECCORE

  return SioExit;
}

/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
VOID
InitializeSio (
  VOID
  )
{

    UINT32  SioExist;
    UINT32  SioEnable;
    UINT32  Index;
    UINT32  Decode;
    UINT32  Enable;
    UINT32  SpiConfigValue;

     //
     // Enable LPC decode
     // Set COMA/COMB base
     //

    Decode =  ((V_PCH_LPC_IOD_COMA_3F8 << N_PCH_LPC_IOD_COMA) | (V_PCH_LPC_IOD_COMB_2F8 << N_PCH_LPC_IOD_COMB));

    SpiConfigValue = MmioRead32 (PCH_PCR_ADDRESS (PID_ESPISPI, R_PCH_PCR_SPI_CONF_VALUE));
    if (SpiConfigValue & B_ESPI_ENABLE_STRAP) {
      Enable =  ( B_PCH_LPC_IOE_ME2 | B_PCH_LPC_IOE_SE | B_PCH_LPC_IOE_ME1 \
                | B_PCH_LPC_IOE_CBE | B_PCH_LPC_IOE_CAE);
    } else {
      Enable =  ( B_PCH_LPC_IOE_ME2 | B_PCH_LPC_IOE_SE | B_PCH_LPC_IOE_ME1 \
                | B_PCH_LPC_IOE_KE | B_PCH_LPC_IOE_CBE | B_PCH_LPC_IOE_CAE);
    }

    IoWrite32 (R_ICH_IOPORT_PCI_INDEX, (UINT32) (ICH_LPC_CF8_ADDR (R_ICH_LPC_IO_DEC)));

    IoWrite32 (R_ICH_IOPORT_PCI_DATA, Decode | (Enable << 16));

    MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LPCIOD), (UINT16)Decode);
    MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LPCIOE), (UINT16)Enable);
    SioExist = IsSioExist ();

    if ((SioExist & (PILOTIV_EXIST | PC8374_EXIST)) == (PILOTIV_EXIST | PC8374_EXIST) ) {
      //
      // Both are there, we use DEFAULT_SIO as debug port anyway
      //
      if (DEFAULT_SIO == PILOTIV_EXIST) {
        SioEnable = PILOTIV_EXIST;
      } else {
        SioEnable = PC8374_EXIST;
      }
    } else {
      SioEnable = SioExist;
    }

    //
    //ASPEED AST2500 UARTA init.
    //
    
    if (SioEnable == ASPEED2500_EXIST) {
      IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);  
      IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);  

      //
      //COM1
      //
      IoWrite8 (ASPEED2500_SIO_INDEX_PORT, REG_LOGICAL_DEVICE);     
      IoWrite8 (ASPEED2500_SIO_DATA_PORT,  ASPEED2500_SIO_UART1);         

      //
      //active COM1
      //
      IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ACTIVATE);     
      IoWrite8 (ASPEED2500_SIO_DATA_PORT,  1);        
      
      IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_LOCK); 

    }

    if (SioEnable == PILOTIV_EXIST) {
      IoWrite8 (PILOTIV_SIO_INDEX_PORT, PILOTIV_SIO_UNLOCK);
      for (Index = 0; Index < sizeof(mPilotIVTable)/sizeof(SIO_REG_TABLE); Index++) {
        IoWrite8 (PILOTIV_SIO_INDEX_PORT, mPilotIVTable[Index].Index);
        IoWrite8 (PILOTIV_SIO_DATA_PORT, mPilotIVTable[Index].Data);
      }
     IoWrite8 (PILOTIV_SIO_INDEX_PORT, PILOTIV_SIO_LOCK);
    }

    if (SioEnable == PC8374_EXIST) {
      for (Index = 0; Index < sizeof(mPc8374Table)/sizeof(SIO_REG_TABLE); Index++) {
        IoWrite8 (PC8374_SIO_INDEX_PORT, mPc8374Table[Index].Index);
        IoWrite8 (PC8374_SIO_DATA_PORT,  mPc8374Table[Index].Data);
      }
    }

    if (SioEnable == NCT5104D_EXIST) {
      for (Index = 0; Index < sizeof(mNCT5104DTable)/sizeof(SIO_REG_TABLE); Index++) {
        IoWrite8 (NCT5104D_SIO_INDEX_PORT, mNCT5104DTable[Index].Index);
        IoWrite8 (NCT5104D_SIO_DATA_PORT,  mNCT5104DTable[Index].Data);
      }
        IoWrite8 (NCT5104D_SIO_INDEX_PORT, NCT5104D_EXIT_THE_EXTENDED_FUNCTION_MODE);

    }

}

/**

  Initialize Serial Port

    The Baud Rate Divisor registers are programmed and the LCR
    is used to configure the communications format. Hard coded
    UART config comes from globals in DebugSerialPlatform lib.

  @param None

  @retval None

**/
RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
  UINTN Divisor;
  UINT8 OutputData;
  UINT8 Data;

  InitializeSio();
  //
  // Some init is done by the platform status code initialization.
  //
  //
  // Map 5..8 to 0..3
  //
  Data = (UINT8) (gData - (UINT8) 5);

  //
  // Calculate divisor for baud generator
  //
  Divisor = 115200 / gBps;

  //
  // Set communications format
  //
  OutputData = (UINT8) ((DLAB << 7) | ((gBreakSet << 6) | ((gParity << 3) | ((gStop << 2) | Data))));
  IoWrite8 (gComBase + LCR_OFFSET, OutputData);

  //
  // Configure baud rate
  //
  IoWrite8 (gComBase + BAUD_HIGH_OFFSET, (UINT8) (Divisor >> 8));
  IoWrite8 (gComBase + BAUD_LOW_OFFSET, (UINT8) (Divisor & 0xff));

  //
  // Switch back to bank 0
  //
  OutputData = (UINT8) ((~DLAB << 7) | ((gBreakSet << 6) | ((gParity << 3) | ((gStop << 2) | Data))));
  IoWrite8 (gComBase + LCR_OFFSET, OutputData);

  return RETURN_SUCCESS;
}

/**
  Write data to serial device.

  If the buffer is NULL, then return 0;
  if NumberOfBytes is zero, then return 0.

  @param  Buffer           Point of data buffer which need to be writed.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval 0                Write data failed.
  @retval !0               Actual number of bytes writed to serial device.

**/
UINTN
EFIAPI
SerialPortWrite (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
)
{
  UINTN Result;
  UINT8 Data;

  if (NULL == Buffer) {
    return 0;
  }

  Result = NumberOfBytes;

  while (NumberOfBytes--) {
    //
    // Wait for the serail port to be ready.
    //
    do {
      Data = IoRead8 ((UINT16) PcdGet64 (PcdSerialRegisterBase) + LSR_OFFSET);
    } while ((Data & LSR_TXRDY) == 0);
    IoWrite8 ((UINT16) PcdGet64 (PcdSerialRegisterBase), *Buffer++);
  }

  return Result;
}


/*
  Read data from serial device and save the datas in buffer.

  If the buffer is NULL, then return 0;
  if NumberOfBytes is zero, then return 0.

  @param  Buffer           Point of data buffer which need to be writed.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval 0                Read data failed.
  @retval !0               Actual number of bytes raed to serial device.

**/
UINTN
EFIAPI
SerialPortRead (
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes
)
{
  UINTN Result;
  UINT8 Data;

  if (NULL == Buffer) {
    return 0;
  }

  Result = NumberOfBytes;

  while (NumberOfBytes--) {
    //
    // Wait for the serail port to be ready.
    //
    do {
      Data = IoRead8 ((UINT16) PcdGet64 (PcdSerialRegisterBase) + LSR_OFFSET);
    } while ((Data & LSR_RXDA) == 0);

    *Buffer++ = IoRead8 ((UINT16) PcdGet64 (PcdSerialRegisterBase));
  }

  return Result;
}

