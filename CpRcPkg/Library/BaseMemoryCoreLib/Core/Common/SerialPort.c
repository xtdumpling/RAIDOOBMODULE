/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
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

//
// -----------------------------------------------------------------------------
//
#include "SysFunc.h"
#include "uart16550.h"


#ifdef SERIAL_DBG_MSG
UINT32
CompressUsbBuffer (
                 PSYSHOST host
                 )
/*++

  Compress buffer before sending

  @param host  - Pointer to the system host (root) structure

  @retval 0 - Success
  @retval 1 - Failure

--*/
{
  UINT8  tempBuf[USB_BUF_SIZE];
  UINT32 dstSize;
  UINT32 status = SUCCESS;

  dstSize = sizeof(tempBuf);
  status = CompressRc (host, host->var.common.packet.usbBuffer, host->var.common.usbBufCount, tempBuf, &dstSize);
  // Copy compressed data to usbBuffer
  if (status == SUCCESS) {
    MemCopy (host->var.common.packet.usbBuffer, tempBuf, dstSize);

    // Update size
    host->var.common.usbBufCount = dstSize;
  }
  return status;

}

UINT32
GetSPPacket (
           PSYSHOST host,
           UINT8    **packetPtr,
           UINT32   *packetCount
          )
/*++

  Build packet for sending. Does compression and per socket thread handling.

  @param host         - Pointer to the system host (root) structure
  @param packetPtr    - Pointer to packet to send
  @param packetCount  - Pointer to packet character count (compressed)

  @retval 0 - Success
  @retval 1 - Failure

--*/
{
  UINT32                    i;
  UINT8                     sum;
  UINT32                    tempCount;
  UINT8                     socket;

  socket = host->var.common.socketId;

  if (host->var.common.serialPipeNum == SERIAL_THREAD_PIPE) {
    if (host->var.common.serialPipeCompress) {
      host->nvram.mem.socket[socket].serialUncompressedBytes += host->var.common.usbBufCount;
      // Encode the buffer if necessary
      CompressUsbBuffer(host);

      // Thread packet format is:
      // - 1 byte packet type/thread number
      // - 2 byte packet size (max 4KB)
      // - packet payload (size - 4)
      // - 1 byte checksum

      // Set bit-7=1 to indicate binary packet format
      host->var.common.packet.threadNum = host->var.common.socketId | 0x80;
    }
    else {
      host->var.common.packet.threadNum = host->var.common.socketId;
    }
    tempCount = host->var.common.usbBufCount + 4;
    host->var.common.packet.size = (UINT16)tempCount;

    // Calculate checksum byte
    sum = 0;
    for (i = 0; i < tempCount - 1; i++) {
      sum += *((UINT8 *)&host->var.common.packet.threadNum + i);
    }
    *((UINT8 *)&host->var.common.packet.threadNum + i) = 0 - sum;

    //fprintf(stderr, "\nsocket %d, packetSize %d", host->var.common.packet.threadNum, host->var.common.packet.size);
    //fflush(stderr);

    *packetPtr   = (UINT8 *)&host->var.common.packet.threadNum;
    *packetCount = tempCount;

    // track statistics
    host->nvram.mem.socket[socket].serialPacketNum++;
    host->nvram.mem.socket[socket].serialCompressedBytes += tempCount;
  } else {
    *packetPtr   = host->var.common.packet.usbBuffer;
    *packetCount = host->var.common.usbBufCount;
  }

  return 0;
} // GetSPPacket

UINT32
SerialSendBuffer (
                 PSYSHOST host,
                 UINT8    now
                 )
/*++

  Sends a byte to the

  @param host  - Pointer to the system host (root) structure
  @param now   - 0 - wait for delay
                 1 - send immediately

    @retval 0 - Success
    @retval 1 - Failure

--*/
{
  UINT32                    ret;
  UINT32                    i;
  USB2_DEBUG_PORT_INSTANCE  *Instance;
  UINT8                     release;
  UINT32                    packetCount;
  UINT8                     *packetPtr;

  ret     = SUCCESS;
  release = 0;

  //
  // host null pointer check
  //
  if (host == NULL) {
    return FAILURE;
  }

  if (host->var.common.serialBufEnable) {

    if ((host->var.common.usbBufCount > USB_BUF_LIMIT) ||
        (GetDelay (host, host->var.common.usbStartCount) > 75000) ||
        now
       ) {

      //
      // Get serial port packet (compressed)
      //
      GetSPPacket (host, &packetPtr, &packetCount);

      //
      // Acquire serial comm semaphore
      //
      if (host->var.common.printfSync) {
        release = getPrintFControl (host);
      }

      //
      // Hook to enable Oem packet processing
      //
      OemSendCompressedPacket(host, packetPtr, packetCount);

      Instance = &host->var.common.usbDebugPort;
      if (Instance->Ready == TRUE) {
        //
        // Send the USB data
        //
        ret = Usb2DebugPortSend (host, host->var.common.packet.usbBuffer, &host->var.common.usbBufCount);
      } else
      {
        //
        // Send the Serial port data
        //
        if (host->var.common.cpuFreq.lo != 0) FixedDelay(host, 50*1000);
        for (i = 0; i < packetCount; i++) {
           RcPutchar (host, packetPtr[i]);
        }
      }

      //
      // Release serial comm semaphore
      //
      if (host->var.common.printfSync && release) {
        releasePrintFControl (host);
      }
      //
      // Reset counters
      //
      host->var.common.usbBufCount    = 0;
      host->var.common.usbStartCount  = GetCount (host);
    }
  }

  return ret;
}

UINT32
SerialBufferChar (
                 PSYSHOST host,
                 INT8     c
                 )
/*++

  Put character into serial buffer

  @param host  - Pointer to the system host (root) structure
  @param c     - character to send

  @retval 0 - Success
  @retval 1 - Failure

--*/
{
  UINT32  ret;

  ret = FAILURE;

  if (!host->var.common.serialBufEnable) {
    RcPutchar (host, c);
    ret = SUCCESS;

  } else {
    //
    // Buffer the data
    //
    if (host->var.common.usbBufCount < USB_BUF_SIZE - 1) {
      host->var.common.packet.usbBuffer[host->var.common.usbBufCount++]  = c;
      ret = SUCCESS;
    }
  }
  return ret;
}

INT8
putchar_buffer (
               PSYSHOST host,
               INT8     c
               )
/*++

  Put character into serial buffer

  @param host  - Pointer to the system host (root) structure
  @param c     - character to send

  @retval 0 - Success
  @retval 1 - Failure

--*/
{
  SerialBufferChar (host, c);
  return c;
}
INT8
RcPutchar (
        PSYSHOST host,
        INT8 c
        )
/*++

  Put character to UART

  @param host  - Pointer to the system host (root) structure
  @param c - character to put to UART

  @retval character placed into UART buffer

--*/
{
  //
  // Dont attempt COM port accesses if globalComPort is NULL
  // or serial debug messages are disabled
  //
  if (host->setup.common.serialDebugMsgLvl) {
    UINT8 UartStatus;

    if (host->var.common.globalComPort) {
      do {
        UartStatus = InPort8_ (host->var.common.globalComPort + LINE_STATUS);
      } while ((UartStatus & TRANS_HOLDING_REG_EMPTY) != TRANS_HOLDING_REG_EMPTY);

      OutPort8_ (host->var.common.globalComPort + TRANSMIT_HOLDING, (UINT8) c);
    }
  }
  return c;
}

INT8
getchar (
        PSYSHOST host
        )
/*++

  Get character from serial buffer

  @param host  - Pointer to the system host (root) structure

  @retval Character received from serial buffer

--*/
{
  INT8  c;

  getchar_timeout (host, &c, NO_TIMEOUT);
  return c;
}

INT8
getchar_echo (
             PSYSHOST host
             )
/*++

  Get character from serial buffer

  @param host  - Pointer to the system host (root) structure

  @retval Character received from serial buffer

--*/
{
  INT8  c;
  c = getchar (host);
  //
  //   if (host->var.common.binMode == 0) {
  //      putchar_buffer(host, c);    // echo char
  //      if (c == '\r') putchar_buffer(host, '\n');
  //      SerialSendBuffer(host, 1);
  //   }
  //
  return c;
}

UINT32
getchar_timeout (
                PSYSHOST host,
                INT8     *c,
                UINT32   usTimeout
                )
/*++

  Get character from serial buffer with a timeout

  @param host      - Pointer to the system host (root) structure
  @param c         - pointer to character received from serial buffer
  @param usTimeout - timeout in microseconds

  @retval 0 - successful
  @retval other - failure

--*/
{
  INT8    tempChar;
  UINT8   UartStatus;
  UINT32  status;

  UINT32  startCount = 0;
  status      = Usb2DebugGetChar (host, &tempChar, usTimeout);
  if (status == EFI_DEVICE_ERROR) {

    if (usTimeout != NO_TIMEOUT) {
      startCount = GetCount (host);
    }

    //
    // Dont attempt COM port accesses if globalComPort is NULL
    //
    if (host->var.common.globalComPort) {
      do {
        UartStatus = InPort8 (host, host->var.common.globalComPort + LINE_STATUS);
        if ((UartStatus & DATA_READY) == DATA_READY) {
          tempChar  = (INT8) InPort8 (host, host->var.common.globalComPort + RECEIVER_BUFFER);
          status    = SUCCESS;
          break;
        }
      } while ((usTimeout == NO_TIMEOUT) || (GetDelay (host, startCount) < usTimeout));
    }
  }

  *c = tempChar;
  return status;
}

//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// {2D5B1B9A-A888-4902-9B8A-4C57190D737E}
//
GUID_RC SelectThreadPipe = { 0x2d5b1b9a, 0xa888, 0x4902, { 0x9b, 0x8a, 0x4c, 0x57, 0x19, 0xd, 0x73, 0x7e } };

// {1E6DCA89-DBF4-4b9b-850E-635752A955A5}
//
GUID_RC SelectBsdPipe = { 0x1e6dca89, 0xdbf4, 0x4b9b, { 0x85, 0xe, 0x63, 0x57, 0x52, 0xa9, 0x55, 0xa5 } };

// {9FC129FA-9D8B-4eb0-8F34-0E787F56D368}
//
GUID_RC SelectConsolePipe = { 0x9fc129fa, 0x9d8b, 0x4eb0, { 0x8f, 0x34, 0xe, 0x78, 0x7f, 0x56, 0xd3, 0x68 } };

/**

Selects the which pipe is active for debug information output

@param host  - Pointer to the system host (root) structure
@param serialPipeNum   - 0 = Console pipe; 1 = Debug pipe; 2 = Thread pipe

@retval N/A

**/
void
SwitchSerialPipe (
  PSYSHOST host,
  UINT32 serialPipeNum
  )
{
  UINT32  i;
  INT8    c;
  GUID_RC *g = &SelectConsolePipe;

  // Return if not enabled
  if (host->var.common.serialPipeEnable == 0) {
    return;
  }

  // Select GUID
  switch (serialPipeNum) {
  case SERIAL_CONSOLE_PIPE:
    g = &SelectConsolePipe;
    rcPrintf ((host, "switch to console pipe\n"));
    break;
  case SERIAL_BSD_PIPE:
    g = &SelectBsdPipe;
    rcPrintf((host, "switch to bsd pipe\n"));
    break;
  case SERIAL_THREAD_PIPE:
    g = &SelectThreadPipe;
    rcPrintf ((host, "switch to threaded pipe\n"));
    break;
  }

  host->var.common.packet.packetNum = 0;

  //
  // Flush the output buffer
  //
  SerialSendBuffer (host, 1);

  //
  // Flush the input buffer
  //
  while (!getchar_timeout (host, &c, 10));

  //
  // Load GUID into the buffer
  //
  for (i = 0; i < sizeof (GUID_RC); i++) {
    putchar_buffer (host, *(((INT8 *)g) + i));
  }
  //
  // Transmit the GUID
  //
  host->var.common.serialPipeNum = SERIAL_CONSOLE_PIPE;
  SerialSendBuffer (host, 1);

  host->var.common.serialPipeNum = serialPipeNum;
}


/**

Setup Serial Debug Messages for reference code

@param host = pointer to sysHost structure

@retval N/A

**/
void
SetupSerialDebugMessages (
  PSYSHOST host
  )
{
#ifdef SERIAL_DBG_MSG

  rcPrintf((host, "BIOSSIM: SetupSerialDebugMessages() \n"));

  //
  // Enable serial buffer based on setup option
  //
  host->var.common.serialBufEnable = host->setup.common.serialBufEnable;

  //
  // OEM hook to enable USB debug port
  //
  OemInitSerialDebug(host);

  if (host->var.common.usbDebugPort.Ready) {
    rcPrintf((host, "\nUSB Serial Debug Enabled\n"));
  } else {
    rcPrintf((host, "\nUART Serial Debug Enabled\n"));
  }

  //
  // Flush serial buffer before enabling serial pipe
  //
  SerialSendBuffer(host, 1);

  //
  // Enable serial threaded pipe and compression based on setup options
  //
  host->var.common.serialPipeEnable = host->setup.common.serialPipeEnable;
  host->var.common.serialPipeCompress = host->setup.common.serialPipeCompress;

  //
  // Print default startup message to console pipe
  //
  rcPrintf((host, "Status Code Available"));

  //
  // Select threaded serial pipe
  //
  SwitchSerialPipe(host, SERIAL_THREAD_PIPE);

#endif //SERIAL_DBG_MSG

} //SetupSerialDebugMessages


/**

Restore Serial Debug Messages after reference code

@param host = pointer to sysHost structure

@retval N/A

**/
void
RestoreSerialDebugMessages(
  PSYSHOST host
  )
{

#ifdef SERIAL_DBG_MSG
  // Flush serial buffer out - switch to console for rest of boot
  SwitchSerialPipe(host, SERIAL_CONSOLE_PIPE);

  // Flush serial buffer
  SerialSendBuffer(host, 1);

#endif // SERIAL_DBG_MSG

} //RestoreSerialDebugMessages


#endif
