/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * CPU Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2013 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/

#include "rcentry.h"
#include "sysfunc.h"
#include "uart16550Init.h"
#include "uart16550.h"


/**

  Read 8 bit IO port

  @param ioAddress - IO address to read

  @retval IO port value

**/
UINT8
InPort8Local (
  UINT16 ioAddress
  )
{
   UINT8 alReg;

   _asm {
      mov   dx, ioAddress
      in    al, dx
      mov   alReg, al
   }
   return alReg;
}

/**

  Write to 8 bit IO port

  @param ioAddress - IO port address
  @param data      - Data to write

  @retval VOID

**/
void
OutPort8Local (
  UINT16 ioAddress,
  UINT8  data
  )
{
   _asm {
      mov   dx, ioAddress
      mov   al, data
      out   dx, al
   }
}

/**

  Write 32-bit IO port

  @param ioAddress - IO port address
  @param data      - Data to write

  @retval VOID

**/
void
OutPort32Local (
  UINT16 ioAddress,
  UINT32 data
  )
{
   _asm {
      mov   dx, ioAddress
      mov   eax, data
      out   dx, eax
   }
}

/**

  Read 32-bit IO port

  @param ioAddress - IO port address

  @retval Value read from IO port

**/
UINT32
InPort32Local (
  UINT16 ioAddress
  )
{
   UINT32 eaxReg;

   _asm {
      mov   dx, ioAddress
      in    eax, dx
      mov   eaxReg, eax
   }
   return eaxReg;
}

#ifdef SERIAL_DBG_MSG
/**

  Initialize IO port

  @param ComPort - base address of comport
  @param UartNumber - 0 = ComA, 1 = Comb

  @retval SUCCESS -- COMPORT initialized
  @retval FAILURE -- COMPORT not initialized successfully

**/
UINT32
InitSerialPortLocal (
  UINT16 ComPort,
  UINT8 UartNumber,
  PSYSHOST host
  )
{
   UINT32   Decode;
   UINT32   Enable;
   UINT8    UartStatus;
   UINT32   WaitTimeInMsec = 0;
   UINT64_STRUCT       startTsc  = { 0, 0 };
   UINT64_STRUCT       currentTsc    = { 0, 0 };

   //
   // Enable LPC decode on ICH9
   //
   Decode =  (1 << 4);

   OutPort32Local (ICH_IOPORT_PCI_INDEX, (UINT32)(ICH_LPC_CF8_ADDR (ICH_LPC_IO_DEC)));
   Enable = InPort32Local (ICH_IOPORT_PCI_DATA);

   Enable |=  ( ICH_LPC_ENABLES_ME2 | ICH_LPC_ENABLES_SE  | ICH_LPC_ENABLES_KE \
              | ICH_LPC_ENABLES_FDE | ICH_LPC_ENABLES_PPE | ICH_LPC_ENABLES_CBE \
              | ICH_LPC_ENABLES_CAE) << 16;

   OutPort32Local (ICH_IOPORT_PCI_DATA,  Decode | Enable);


   OutPort8Local(WPC_INDEX_PORT0, DEV_ID_REGISTER);
   if (InPort8Local(WPC_DATA_PORT0) == V_WPC_DEV_ID) {

       //
       // Select COM to program
       //
       OutPort8Local (WPC_INDEX_PORT0, REG_LOGICAL_DEVICE);
       OutPort8Local (WPC_DATA_PORT0, (UartNumber ? WPC_LDN_UART1 : WPC_LDN_UART0));

       //
       // Set the Baseaddress to gComBase
       //
       OutPort8Local (WPC_INDEX_PORT0, BASE_ADDRESS_LOW);
       OutPort8Local (WPC_DATA_PORT0, (UINT8) (ComPort & 0x00FF));

       OutPort8Local (WPC_INDEX_PORT0, BASE_ADDRESS_HIGH);
       OutPort8Local (WPC_DATA_PORT0, (UINT8) (ComPort >> 8));

       //
       // Interrupt Number
       //
       OutPort8Local (WPC_INDEX_PORT0, 0x70);
       OutPort8Local (WPC_DATA_PORT0, (UartNumber ? 0x03 : 0x04));

       //
       // Activate
       //
       OutPort8Local (WPC_INDEX_PORT0, ACTIVATE);
       OutPort8Local (WPC_DATA_PORT0, 0x01);

   } else {
       OutPort8Local(BMC_INDEX_PORT0, BMC_ACTIVATE_CFG_MODE);
       // Ensure the BMC is alive.
       ReadTsc64 (&startTsc);
       do{
           UartStatus = InPort8Local (BMC_INDEX_PORT0);
           ReadTsc64 (&currentTsc);
           WaitTimeInMsec = TimeDiff (host, startTsc, currentTsc, TDIFF_UNIT_MS);
           if (WaitTimeInMsec > (8*1024)) return FAILURE;   // MaX wait for BMC is 8 Sec
       } while (UartStatus == 0xff);

       //
       // Init SIO to enable the COM port.
       // Internal Clock selection
       //
       OutPort8Local (BMC_INDEX_PORT0, 0x2D);
       OutPort8Local (BMC_DATA_PORT0, 0x00);

       //
       // Enable 48 MHz clock
       //
       OutPort8Local (BMC_INDEX_PORT0, 0x29);
       OutPort8Local (BMC_DATA_PORT0, 0x80);

       //
       // Select COM1 to program
       //
       OutPort8Local (BMC_INDEX_PORT0, REG_LOGICAL_DEVICE);
       OutPort8Local (BMC_DATA_PORT0, (UartNumber ? BMC_LDN_UART1 : BMC_LDN_UART0));

       //
       // Set the Baseaddress to gComBase
       //
       OutPort8Local (BMC_INDEX_PORT0, BASE_ADDRESS_LOW);
       OutPort8Local (BMC_DATA_PORT0, (UINT8) (ComPort & 0x00FF));

       OutPort8Local (BMC_INDEX_PORT0, BASE_ADDRESS_HIGH);
       OutPort8Local (BMC_DATA_PORT0, (UINT8) (ComPort >> 8));

       //
       // Interrupt Number
       //
       OutPort8Local (BMC_INDEX_PORT0, 0x70);
       OutPort8Local (BMC_DATA_PORT0, (UartNumber ? 0x03 : 0x04));

       //
       // Activate
       //
       OutPort8Local (BMC_INDEX_PORT0, ACTIVATE);
       OutPort8Local (BMC_DATA_PORT0, 0x01);

   }

   //
   // Set Baud rate to 115k in Divisor Latch
   //
   OutPort8Local (ComPort + LINE_CONTROL, DIVISOR_LATCH_ACCESS);
   OutPort8Local (ComPort + DIVISOR_LATCH_LOW, 0x01);
   OutPort8Local (ComPort + DIVISOR_LATCH_HIGH, 0x00);

   OutPort8Local (ComPort + LINE_CONTROL, NO_STOP | NO_PARITY | BITS8);
   OutPort8Local (ComPort + INTERRUPT_ENABLE, DISABLE_INTERRUPTS);

   //
   // Disable FIFO in 16550
   //
   UartStatus = InPort8Local (ComPort + FIFO_CONTROL);
   OutPort8Local (ComPort + FIFO_CONTROL, UartStatus & FIFO_DISABLE_MASK);

   return SUCCESS;
}
#endif  // SERIAL_DBG_MSG

#ifdef SSA_FLAG
#define  RC_HEAP_SIZE   (128*1024) //Increase heap size for BSSA in miniBIOS?
#else
#define  RC_HEAP_SIZE   (32*1024)
#endif //SSA_FLAG

/**

  Main entry point for CPU code

  @param None

  @retval VOID

**/
void
CpuMain (
  VOID
  )
{
   UINT8 heap[RC_HEAP_SIZE];
   struct sysHost host;
   UINT32 status;
   struct sysSetup *setup;

   //
   // create a pointer to sysSetup structure
   //
   setup = (struct sysSetup *)&host.setup;

   //
   // Initialize data structures
   //
   MemSetLocal((UINT8 *)&host, 0x00, sizeof (SYSHOST));

   host.var.common.heapBase = (UINT32)heap;
   host.var.common.heapSize = RC_HEAP_SIZE;

   InitCpuTimer(&host);

#ifdef   SERIAL_DBG_MSG
   if (InitSerialPortLocal(ConsoleComPort, 0, &host) == SUCCESS) setup->common.consoleComPort = (UINT16)ConsoleComPort;

   setup->common.usbDebug.Enable = 0x01;
   setup->common.usbDebug.Bus = PCI_EHCI_DEFAULT_BUS_NUMBER;
   setup->common.usbDebug.Device = PCI_EHCI_DEFAULT_DEVICE_NUMBER;
   setup->common.usbDebug.Function = PCI_EHCI_DEFAULT_FUNCTION_NUMBER;
#endif


   //
   // Reference Code entry point
   //
   status = ( *( UINT32 (*) (struct sysHost *) ) RCINITENTRYPOINT )(&host);

}
