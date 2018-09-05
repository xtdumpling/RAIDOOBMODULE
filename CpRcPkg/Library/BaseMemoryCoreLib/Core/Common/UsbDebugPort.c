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
// APTIOV_SERVER_OVERRIDE_RC_START : CAR_BASE_ADDRESS must be used from AMI SDL output
#include "Token.h"
// APTIOV_SERVER_OVERRIDE_RC_END : CAR_BASE_ADDRESS must be used from AMI SDL output
#include "SysFunc.h"
#ifdef SERIAL_DBG_MSG

  #define STATIC  static

//
// Warning 4740 : flow in or out of asm code suppresses global optimization
//
#ifdef _MSC_VER
  #pragma warning(disable : 4740)
#endif

  #include "UsbDebugPort.h"
  #include "UsbDebugPortConfig.h"

UINT32
Usb2DebugPortOut (
                 PSYSHOST  host,
                 IN UINT8  *Buffer,
                 IN UINT32 Length,
                 IN UINT32 TimeOut,
                 IN UINT8  Token,
                 IN UINT8  DataToggle
                 );

UINT32
Usb2DebugPortIn (
                PSYSHOST      host,
                OUT UINT8     *Buffer,
                OUT UINT32    *Length,
                IN  UINT32    TimeOut,
                IN  UINT8     DataToggle
                );

STATIC USB2_SETUP_PACKET  mSetAddress = {
  USB2_REQUEST_TYPE_HOST_TO_DEVICE | USB2_REQUEST_TYPE_STANDARD | USB2_REQUEST_TYPE_DEVICE,
  USB2_REQUEST_SET_ADDRESS,
  {
    USB2_DEBUG_PORT_DEFAULT_ADDRESS,
    0x00
  },
  0x00,
  0x00
};

STATIC USB2_SETUP_PACKET  mGetDebugDescriptor = {
  USB2_REQUEST_TYPE_DEVICE_TO_HOST | USB2_REQUEST_TYPE_STANDARD | USB2_REQUEST_TYPE_DEVICE,
  USB2_REQUEST_GET_DESCRIPTOR,
  {
    0x00,
    USB2_DESCRIPTOR_TYPE_DEBUG
  },
  0x0000,
  sizeof (USB2_DEBUG_DESCRIPTOR_TYPE)
};

STATIC USB2_SETUP_PACKET  mSetDebugFeature = {
  USB2_REQUEST_TYPE_HOST_TO_DEVICE | USB2_REQUEST_TYPE_STANDARD | USB2_REQUEST_TYPE_DEVICE, // 0x00,
  USB2_REQUEST_SET_FEATURE, // 0x03,
  {
    USB2_FEATURE_DEBUG_MODE,
    0x00
  },                        // 0x06, 0x00,
  0x0000,
  0x0000
};


void EhciDebugPortWorkaround (
                             PSYSHOST host
                             )
/*++ 

  PCH BIOS Spec Update Rev 0.71 Sighting #3305753 : High Speed USB Devices May Not Be Detected
  with RMH Enabled.                                                                           
  System BIOS is required to follow steps below in early chipset initialization before RMH is 
  enabled to reduce the failure rate.                                                         
  1. D26/D29:F7:F4[31] = 1b                                                                   
  2. D26/D29:F7:F4[19:16] = 1111b                                                             
  3. D26/D29:F7:F4[31] = 0b                                                                   
 
  @param host  - Pointer to the system host (root) structure

  @retval N/A

--*/
{
  UINT32 Rcba = SB_RCBA;
  UINT8  Bus;
  UINT8  Device;
  UINT8  Function;
  UINT8  Offset;
  UINT32 temp;

  // Disable RMH by setting RCBA+3598h [0] = 1b
  SET_R32_BIT((Rcba + 0x3598), BIT0);

  // PCH BIOS Spec Update Rev 0.71 Sighting #3305753 : High Speed USB Devices May Not Be Detected
  // with RMH Enabled.
  // System BIOS is required to follow steps below in early chipset initialization before RMH is
  // enabled to reduce the failure rate.
  // 1. D26/D29:F7:F4[31] = 1b
  // 2. D26/D29:F7:F4[19:16] = 1111b
  // 3. D26/D29:F7:F4[31] = 0b
  //
  Bus = USB_DEBUG_WA_BUS;
  Device = USB_DEBUG_WA_DEVICE;
  Function = USB_DEBUG_WA_FUNCTION;
  Offset = USB_DEBUG_WA_OFFSET;
  temp = ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, Offset, sizeof (UINT32)));
  temp = temp | BIT31;
  WritePciCfg (host, 0, REG_ADDR (Bus, Device, Function, Offset, sizeof (UINT32)), temp);
  temp = temp | BIT16 | BIT17 | BIT18 | BIT19;
  WritePciCfg (host, 0, REG_ADDR (Bus, Device, Function, Offset, sizeof (UINT32)), temp);
  temp = temp & ~BIT31;
  WritePciCfg (host, 0, REG_ADDR (Bus, Device, Function, Offset, sizeof (UINT32)), temp);

  Device = 29;
  temp = ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, Offset, sizeof (UINT32)));
  temp = temp | BIT31;
  WritePciCfg (host, 0, REG_ADDR (Bus, Device, Function, Offset, sizeof (UINT32)), temp);
  temp = temp | BIT16 | BIT17 | BIT18 | BIT19;
  WritePciCfg (host, 0, REG_ADDR (Bus, Device, Function, Offset, sizeof (UINT32)), temp);
  temp = temp & ~BIT31;
  WritePciCfg (host, 0, REG_ADDR (Bus, Device, Function, Offset, sizeof (UINT32)), temp);

  //Enable RMH
  CLR_R32_BIT((Rcba + 0x3598), BIT0);
  return;
}


UINT8
GetCapabilityPtr(
                PSYSHOST host,
                UINT8  Bus,
                UINT8  Device,
                UINT8  Function,
                UINT8  CapId
                )
/*++

    Return the offset to the CapId register for the requested PCI address

    @param host     - Pointer to the system host (root) structure
    @param Bus      - PCI Bus number
    @param Device   - PCI Device number
    @param Function - PCI Function number
    @param CapId    - Capability ID

    @retval Offset to the CapId register

--*/
{
  UINT8  Value;
  UINT8  Offset;

  Offset = PCI_PRIMARY_STATUS_OFFSET;
  Value = (UINT8)ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, Offset, sizeof (UINT8)));
  if (Value == 0xff) return 0;
  if (!(Value & (1 << 4))) return 0;

  Offset = PCI_CAPBILITY_POINTER_OFFSET;
  for (;;) {

    Value = (UINT8)ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, Offset, sizeof (UINT8)));
    if (!Value) return 0;

    Offset = Value;
    Value = (UINT8)ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, Offset, sizeof (UINT8)));
    if (Value == CapId) return(UINT8)Offset;

    Offset++;
  }
}

UINT32
Usb2DebugPortControlTransfer (
                             PSYSHOST                                host,
                             IN      USB2_SETUP_PACKET               *SetupPacket,
                             OUT UINT8                               *Data,
                             OUT UINT32                              *DataLength
                             )
/*++

  Transfer debug port control

  @param host        - Pointer to the system host (root) structure
  @param SetupPacket - USB2 setup packet
  @param Data        - Data to transfer
  @param DataLength  - length of data to transfer

  @retval EFI_UNSUPPORTED - Invalid input
  @retval 0 - Success

--*/
{
  UINT32  Status;
  //
  //  USB2_DEBUG_PORT_INSTANCE *This = &host->.var.common.usbDebugPort;
  //
  if ((SetupPacket->Length_ > 0) && (!IS_BIT_SET (&SetupPacket->RequestType, USB2_REQUEST_TYPE_DEVICE_TO_HOST))) {
    return FAILURE;
  }

  Status = Usb2DebugPortOut (host, (UINT8 *) SetupPacket, sizeof (USB2_SETUP_PACKET), 1000000, USB2_PID_TOKEN_SETUP, 0);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // data phase
  //
  if (SetupPacket->Length_ > 0) {
    if (IS_BIT_SET (&SetupPacket->RequestType, USB2_REQUEST_TYPE_DEVICE_TO_HOST)) {
      Status = Usb2DebugPortIn (host, Data, DataLength, 1000000, 1);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }
  //
  // status handshake
  //
  if (IS_BIT_SET (&SetupPacket->RequestType, USB2_REQUEST_TYPE_DEVICE_TO_HOST)) {
    Status = Usb2DebugPortOut (host, NULL, 0, 1000000, USB2_PID_TOKEN_OUT, 1);
  } else {
    Status = Usb2DebugPortIn (host, NULL, &Status, 1000000, 1);
  }

  return Status;
}

UINT32
Usb2DebugPortIn (
                PSYSHOST                                 host,
                OUT UINT8                                *Buffer,
                OUT UINT32                               *Length,
                IN      UINT32                           TimeOut,
                IN      UINT8                            DataToggle
                )
/*++

  Get usb2 debug port data

  @param host        - Pointer to the system host (root) structure
  @param Buffer      - Pointer to stream store location
  @param Length      - Pointer to length of stream buffer stored
  @param TimeOut     - number of attempts to try
  @param DataToggle  - toggle byte value

  @retval EFI_SUCCESS - Stream captured successfully
  @retval EFI_DEVICE_ERROR - Device error while capturing stream
  @retval EFI_TIMEOUT - Timeout while waiting from input

--*/
{
  UINT32                    BufferIndex;
  UINT32                    RetryCount;
  USB2_DEBUG_PORT_INSTANCE  *This;
  USB2_DEBUG_PORT_REGISTER  *DebugRegisterPtr;

  RetryCount        = 3;
  This              = &host->var.common.usbDebugPort;

  *Length           = 0;

  DebugRegisterPtr  = (USB2_DEBUG_PORT_REGISTER *) This->DebugRegister;
  //
  // setup PIDs
  //
  DebugRegisterPtr->TokenPid = USB2_PID_TOKEN_IN;
  if (DataToggle) {
    DebugRegisterPtr->SendPid = USB2_PID_DATA1;
  } else {
    DebugRegisterPtr->SendPid = USB2_PID_DATA0;
  }

  while (TimeOut && RetryCount) {
    CLR_R32_BIT (&DebugRegisterPtr->ControlStatus, USB2_DEBUG_PORT_STATUS_WRITE);
    SET_R32_BIT (&DebugRegisterPtr->ControlStatus, USB2_DEBUG_PORT_STATUS_GO);

    //
    // Wait for complete
    //
    while (!IS_BIT_SET (&DebugRegisterPtr->ControlStatus, USB2_DEBUG_PORT_STATUS_DONE)) {
      //
      //       FixedDelay(host, 1);
      //
    }

    if (IS_BIT_SET (&DebugRegisterPtr->ControlStatus, USB2_DEBUG_PORT_STATUS_ERROR)) {
      --RetryCount;
    } else if (DebugRegisterPtr->ReceivedPid == DebugRegisterPtr->SendPid) {
      // APTIOV_SERVER_OVERRIDE_RC_START : Added check for condition when Buffer is NULL
      if (Buffer == NULL) {
        return (UINT32) EFI_INVALID_PARAMETER;
      }
      // APTIOV_SERVER_OVERRIDE_RC_END : Added check for condition when Buffer is NULL
      *Length = DebugRegisterPtr->ControlStatus & USB2_DEBUG_PORT_STATUS_LENGTH;
      for (BufferIndex = 0; BufferIndex < *Length; ++BufferIndex) {
        Buffer[BufferIndex] = DebugRegisterPtr->DataBuffer[BufferIndex];
      }

      return EFI_SUCCESS;
    } else {
      FixedDelay (host, 1);
    }

    --TimeOut;
  }

  if (RetryCount == 0) {
    return(UINT32) EFI_DEVICE_ERROR;
  }

  return(UINT32) EFI_TIMEOUT;
}

UINT32
Usb2DebugPortOut (
                 PSYSHOST                                 host,
                 IN      UINT8                            *Buffer,
                 IN      UINT32                           Length,
                 IN      UINT32                           TimeOut,
                 IN      UINT8                            Token,
                 IN      UINT8                            DataToggle
                 )
/*++

  Output steam to USB2 debug port

  @param host        - Pointer to the system host (root) structure
  @param Buffer      - Pointer to stream to output
  @param Length      - Length of stream to output
  @param TimeOut     - number of attempts to try
  @param Token       - USBdebug Token PID
  @param DataToggle  - Data toggle byte

  @retval EFI_SUCCESS - Stream captured successfully
  @retval EFI_DEVICE_ERROR - Device error while capturing stream
  @retval EFI_TIMEOUT - Timeout while waiting from input

--*/
{
  UINT32                    BufferIndex;
  UINT32                    RetryCount;
  USB2_DEBUG_PORT_INSTANCE  *This;
  USB2_DEBUG_PORT_REGISTER  *DebugRegisterPtr;

  RetryCount                  = 3;
  This                        = &host->var.common.usbDebugPort;

  DebugRegisterPtr            = (USB2_DEBUG_PORT_REGISTER *) This->DebugRegister;

  DebugRegisterPtr->TokenPid  = Token;
  if (DataToggle) {
    DebugRegisterPtr->SendPid = USB2_PID_DATA1;
  } else {
    DebugRegisterPtr->SendPid = USB2_PID_DATA0;
  }

  for (BufferIndex = 0; BufferIndex < Length; ++BufferIndex) {
    DebugRegisterPtr->DataBuffer[BufferIndex] = Buffer[BufferIndex];
  }
  //
  // Fill the data length
  //
  CLR_R32_BIT (&DebugRegisterPtr->ControlStatus, USB2_DEBUG_PORT_STATUS_LENGTH);
  SET_R32_BIT (&DebugRegisterPtr->ControlStatus, Length);
  while (TimeOut && RetryCount) {
    SET_R32_BIT (&DebugRegisterPtr->ControlStatus, USB2_DEBUG_PORT_STATUS_WRITE);
    SET_R32_BIT (&DebugRegisterPtr->ControlStatus, USB2_DEBUG_PORT_STATUS_GO);

    //
    // Wait for complete
    //
    while (!IS_BIT_SET (&DebugRegisterPtr->ControlStatus, USB2_DEBUG_PORT_STATUS_DONE)) {
      //
      //       FixedDelay(host, 1);
      //
    }

    if (IS_BIT_SET (&DebugRegisterPtr->ControlStatus, USB2_DEBUG_PORT_STATUS_ERROR)) {
      --RetryCount;
    } else {
      //
      // Debug
      //
      if (DebugRegisterPtr->ReceivedPid == USB2_PID_HANDSHAKE_ACK) {
        return EFI_SUCCESS;
      } else if (DebugRegisterPtr->ReceivedPid == USB2_PID_HANDSHAKE_NYET) {
        return EFI_SUCCESS;
      } else {
        FixedDelay (host, 1);
      }
    }

    --TimeOut;
  }

  if (RetryCount == 0) {
    return(UINT32) EFI_DEVICE_ERROR;
  }

  return(UINT32) EFI_TIMEOUT;
}

UINT32
Usb2DebugPortSend (
                  PSYSHOST                                 host,
                  IN      UINT8                            *Data,
                  IN  OUT UINT32                           *Length
                  )
/*++

  Send data to the USB2 debug port

  @param host    - Pointer to the system host (root) structure
  @param Data    - Pointer to data to send
  @param Length  - Size of data to send

  @retval 0 - Success
  @retval Other - failure

--*/
{
  UINT32                    Status;
  UINT32                    BytesToSend;
  USB2_DEBUG_PORT_INSTANCE  *Instance;
  USB2_DEBUG_PORT_REGISTER  *DebugRegisterPtr;

  Instance          = &host->var.common.usbDebugPort;

  DebugRegisterPtr  = (USB2_DEBUG_PORT_REGISTER *) Instance->DebugRegister;

  if (Instance->Ready == TRUE) {
    DebugRegisterPtr->UsbAddress  = USB2_DEBUG_PORT_DEFAULT_ADDRESS;
    DebugRegisterPtr->UsbEndPoint = Instance->WriteEndpoint;

    BytesToSend                   = 0;
    while (*Length > 0) {
      BytesToSend = ((*Length) > 8) ? 8 : *Length;
      Status = Usb2DebugPortOut (
                                host,
                                Data,
                                BytesToSend,
                                1000000,
                                (UINT8) USB2_PID_TOKEN_OUT,
                                Instance->WriteEndpointDataToggle
                                );
      if (EFI_ERROR (Status)) {
        return Status;
      }

      Instance->WriteEndpointDataToggle ^= 0x01;

      *Length -= BytesToSend;
      Data += BytesToSend;
    }
    //
    // Send zero-length packet to end of this send session, if the length of data
    // is integer number of wMaxPacketSize (8 bytes for debug port)
    //
    Status = Usb2DebugPortOut (host, Data, 0, 1000000, (UINT8) USB2_PID_TOKEN_OUT, Instance->WriteEndpointDataToggle);
    if (!EFI_ERROR (Status)) {
      Instance->WriteEndpointDataToggle ^= 0x01;
    }
  }

  return EFI_SUCCESS;
}

UINT32
Usb2DebugPortReceive (
                     PSYSHOST                                 host,
                     OUT UINT8                                *Data,
                     IN  OUT UINT32                           *Length
                     )
/*++

  Get data from USB2 debug port

  @param host    - Pointer to the system host (root) structure
  @param Data    - Pointer to data buffer
  @param Length  - Pointer to received string length

  @retval 0 - success
  @retval Other - failure

--*/
{
  UINT32                    Status;
  UINT32                    BytesReceived;
  UINT32                    AllBytesReceived;
  UINT32                    Index;
  USB2_DEBUG_PORT_INSTANCE  *Instance;
  UINT8                     *tempPtr;
  //
  // debug
  //
  UINT32                    i;
  USB2_DEBUG_PORT_REGISTER  *DebugRegisterPtr;

  Status            = EFI_SUCCESS;
  Instance          = &host->var.common.usbDebugPort;

  DebugRegisterPtr  = (USB2_DEBUG_PORT_REGISTER *) Instance->DebugRegister;

  if (Instance->Ready == TRUE) {
    DebugRegisterPtr->UsbAddress  = USB2_DEBUG_PORT_DEFAULT_ADDRESS;
    DebugRegisterPtr->UsbEndPoint = Instance->ReadEndpoint;

    if (*Length <= 0 || Data == NULL) {
      *Length = 0;
      //
      // if incorrect buffer or length specified, read data into internal buffer
      //
    }

    AllBytesReceived  = 0;
    BytesReceived     = 0;
    do {
      if (Instance->TempDataLength == 0 || *Length == 0) {
        if (Instance->TempDataLength == 0) {
          Instance->TempDataIndex = 0;
        }

        if (*Length >= USB2_DEBUG_PORT_DEVICE_BUFFER_MAX) {
          //
          // if there's enough space in buffer, try storing directly the data in user's buffer
          //
          Status = Usb2DebugPortIn (host, Data, &BytesReceived, 1, Instance->ReadEndpointDataToggle);

          //
          // debug
          //
          if (!EFI_ERROR (Status)) {
            host->var.common.usbDebugPort.Ready = FALSE;
            Data[BytesReceived]                 = 0;
            rcPrintf ((host, "\nret = %d, data = %s, len = %d\n", Status, Data, BytesReceived));
            for (i = 0; i < BytesReceived; i++) {
              rcPrintf ((host, "0x%02X ", Data[i]));
            }

            SerialSendBuffer (host, 1);
            host->var.common.usbDebugPort.Ready = TRUE;
          }

          if (EFI_ERROR (Status)) {
            break;
          }
        } else if ((
                   USB2_DEBUG_PORT_DRIVER_BUFFER_MAX -
                   Instance->TempDataIndex -
                   Instance->TempDataLength
                   ) >= USB2_DEBUG_PORT_DEVICE_BUFFER_MAX
                  ) {
          //
          // debug
          //
          tempPtr = Instance->TempData + Instance->TempDataIndex;

          //
          // otherwise, we have to store received data in a temporary buffer first to avoid overflow
          //
          Status = Usb2DebugPortIn (
                                   host,
                                   Instance->TempData + Instance->TempDataIndex,
                                   &BytesReceived,
                                   1,
                                   Instance->ReadEndpointDataToggle
                                   );

          //
          // debug
          //
          if (!EFI_ERROR (Status)) {
            host->var.common.usbDebugPort.Ready = FALSE;
            tempPtr[BytesReceived]              = 0;
            rcPrintf ((host, "\nret = %d, data = %s, len = %d\n", Status, tempPtr, BytesReceived));
            for (i = 0; i < BytesReceived; i++) {
              rcPrintf ((host, "0x%02X ", tempPtr[i]));
            }

            SerialSendBuffer (host, 1);
            host->var.common.usbDebugPort.Ready = TRUE;
          }

          if (EFI_ERROR (Status)) {
            break;
          }

          Instance->TempDataLength += BytesReceived;
        } else {
          Status = (UINT32) EFI_OUT_OF_RESOURCES;
          break;
        }

        Instance->ReadEndpointDataToggle ^= 0x01;
        if (BytesReceived == 0) {
          //
          // got a ZLP (Zero-Length Packet), stop receiving and send received data up
          //
          break;
        }
      }
      //
      // move the data from internal temporary buffer to user's buffer, if any
      //
      if (Instance->TempDataLength > 0) {
        BytesReceived = (*Length > (UINT32) Instance->TempDataLength) ? Instance->TempDataLength : *Length;
        for (Index = 0; Index < BytesReceived; ++Index) {
          Data[Index] = Instance->TempData[Instance->TempDataIndex++];
          if (Instance->TempDataIndex >= USB2_DEBUG_PORT_DRIVER_BUFFER_MAX) {
            Instance->TempDataIndex = 0;
          }
        }

        Instance->TempDataLength -= BytesReceived;
      }

      *Length -= BytesReceived;
      Data += BytesReceived;
      AllBytesReceived += BytesReceived;
    } while (*Length > 0);

    *Length = AllBytesReceived;
    if (*Length > 0) {
      return EFI_SUCCESS;
    }
  }

  return Status;
}

UINT32
Usb2DebugGetChar (
                 PSYSHOST host,
                 INT8     *c,
                 UINT32   usTimeout
                 )
/*++

  Get character from debug port

  @param host      - Pointer to the system host (root) structure
  @param c         - Pointer to store character
  @param usTimeout - timeout to wait fo get character in microseconds

  @retval 0 - Success
  @retval Other - Failure

--*/
{
  UINT32                    len;
  UINT32                    ret;
  USB2_DEBUG_PORT_INSTANCE  *Instance;
  UINT32                    startCount;

  ret         = (UINT32) EFI_DEVICE_ERROR;
  startCount  = 0;

  Instance    = &host->var.common.usbDebugPort;
  if (Instance->Ready == TRUE) {
    //
    // Read data from the USB debug port if needed
    //
    if (!host->var.common.serialInCount || (host->var.common.serialInByte >= host->var.common.serialInCount)) {
      //
      // Wait for the data
      //
      ret = (UINT32) EFI_TIMEOUT;
      if (usTimeout != NO_TIMEOUT) {
        startCount = GetCount (host);
      }

      host->var.common.serialInCount  = 0;
      host->var.common.serialInByte   = 0;
      do {
        len = 8;
        ret = Usb2DebugPortReceive (host, &host->var.common.serialInBuf[host->var.common.serialInCount], &len);
        if (!ret && len) {
          host->var.common.serialInCount += len;
          *c  = host->var.common.serialInBuf[host->var.common.serialInByte++];
          ret = EFI_SUCCESS;
          break;
        }
      } while ((usTimeout == NO_TIMEOUT) || (GetDelay (host, startCount) < usTimeout));
    }
    //
    // Read data from the buffer
    //
    else {
      *c  = host->var.common.serialInBuf[host->var.common.serialInByte++];
      ret = EFI_SUCCESS;
    }
  }

  return ret;
}
#endif  // SERIAL_DBG_MSG

UINT32
DiscoverUsb2DebugPort (
                      PSYSHOST host
                      )
/*++

  Find the USB2 debug port

  @param host  - Pointer to the system host (root) structure

  @retval EFI_DEVICE_ERROR - couldnt find port
  @retval EFI_SUCCESS - USB2 device discovered

--*/
{
  //
  // If serial debug is disabled, we simply return an error
  //
#ifndef SERIAL_DBG_MSG
  return (UINT32) EFI_DEVICE_ERROR;
#else
  UINT8                           Bus;
  UINT8                           Device;
  UINT8                           Function;
  UINT8                           PwrMgmtReg;
  UINT8                           DbgPrtReg;
  UINT16                          CmdReg;
  UINT16                          BarOffset;
  UINT8                           BarIndex;
  UINT32                          DebugPortBase;
  UINT32                          UsbBase;
  UINT32                          UsbSpaceSize;
  USB2_DEBUG_PORT_INSTANCE        *This = &host->var.common.usbDebugPort;
  USB2_EHCI_CAPABILITY_REGISTER   *EhciCapRegisterPtr;
  USB2_EHCI_OPERATIONAL_REGISTER  *EhciOpRegisterPtr;

  UsbSpaceSize = 0;
  DebugPortBase = 0;

  if (host->var.common.emulation & EMULATION_FLAG_TO_DISABLE_USB_DEBUG) return FAILURE;

  if (!host->setup.common.usbDebug.Enable) return FAILURE;

  // Apply PCH workaround
  EhciDebugPortWorkaround(host);


  //
  // Find USB2 whose ClassCode = 0C 03 20
  //    Base        0C (Serial Bus Controller)
  //    Sub-Class   03 (USB)
  //    Interface:  20 (EHCI)
  //                          10 (OHCI) 00 (UHCI) 80 (non-specific) FE (USB device)
  //
  Bus = PCI_EHCI_DEFAULT_BUS_NUMBER;
  Device = PCI_EHCI_DEFAULT_DEVICE_NUMBER;
  Function = PCI_EHCI_DEFAULT_FUNCTION_NUMBER;


  if ((ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_CLASSCODE_OFFSET + 2, sizeof (UINT8))) != PCI_CLASS_SERIAL) ||
      (ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_CLASSCODE_OFFSET + 1, sizeof (UINT8))) != PCI_CLASS_SERIAL_USB) ||
      (ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_CLASSCODE_OFFSET, sizeof (UINT8))) != PCI_CLASS_SERIAL_USB_EHCI)) {

    return FAILURE;
  }

  //  Disable the device
  CmdReg = (UINT16) ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_COMMAND_OFFSET, sizeof (UINT16)));
  if (CmdReg & 2) {
    WritePciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_COMMAND_OFFSET, sizeof (UINT16)), 0);
  }

  // Assign MMIO base address register
  UsbBase = ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_BASE_ADDRESSREG_OFFSET, sizeof (UINT32)));
  if (UsbBase == 0) {

    // Detect size
    UsbBase = 0xFFFFFFFF;
    WritePciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_BASE_ADDRESSREG_OFFSET, sizeof (UINT32)), UsbBase);
    UsbSpaceSize  = ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_BASE_ADDRESSREG_OFFSET, sizeof (UINT32)));
    UsbSpaceSize = (~(UsbSpaceSize & 0xFFFFFFF0)) + 1;

    // Write base address register
//    UsbBase = PCI_EHCI_DEFAULT_USBBASE_ADDRESS;
    UsbBase = CAR_BASE_ADDRESS - UsbSpaceSize;

    WritePciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_BASE_ADDRESSREG_OFFSET, sizeof (UINT32)), UsbBase);
    UsbBase = ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_BASE_ADDRESSREG_OFFSET, sizeof (UINT32)));

    if (UsbBase == 0) {
      return FAILURE;
    }
  }

  // Set the Power state of the device to D0
  PwrMgmtReg = GetCapabilityPtr(host, Bus, Device, Function, PWR_MGT_CAP_ID);
  WritePciCfg (host, 0, REG_ADDR (Bus, Device, Function, PwrMgmtReg + 4, sizeof (UINT16)), 0x8000);

  // Locate the Debug port register Interface location
  DbgPrtReg = GetCapabilityPtr(host, Bus, Device, Function, DBG_PRT_CAP_ID);
  BarOffset = (UINT16) ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, DbgPrtReg + 2, sizeof (UINT16))) & 0x1FFF;
  BarIndex = (UINT8) (ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, DbgPrtReg + 2, sizeof (UINT16))) >> 13) - 1;

  // Find the base address of debug port register in Debug Port capability register
  DebugPortBase = ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_BASE_ADDRESSREG_OFFSET + BarIndex * 4, sizeof (UINT32)));
  if (DebugPortBase == 0) {
    DebugPortBase = UsbBase + UsbSpaceSize;
    WritePciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_BASE_ADDRESSREG_OFFSET + BarIndex * 4, sizeof (UINT32)), DebugPortBase);
    DebugPortBase = ReadPciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_BASE_ADDRESSREG_OFFSET + BarIndex * 4, sizeof (UINT32)));
    if (DebugPortBase == 0) {
      return FAILURE;
    }
  }
  // Enable EHCI device & make it Bus master
  CmdReg = CmdReg | 0x06;
  WritePciCfg (host, 0, REG_ADDR (Bus, Device, Function, PCI_COMMAND_OFFSET, sizeof (UINT16)), CmdReg);


  if (UsbBase != This->EhciCapRegister) {
    //
    // find EHCI register: PORTSC, HCSPARAMS
    //
    This->EhciCapRegister = UsbBase;
    EhciCapRegisterPtr = (USB2_EHCI_CAPABILITY_REGISTER *)(This->EhciCapRegister);
    This->EhciOpRegister = UsbBase + EhciCapRegisterPtr->CapLength;
    EhciOpRegisterPtr = (USB2_EHCI_OPERATIONAL_REGISTER *)(This->EhciOpRegister);

    //
    // find debug port number: indicated by a 4-bit field (20-23) in the HCSPARAMS register
    //
    This->PortNumber = (UINT8)((EhciCapRegisterPtr->HcsParams & 0x00F00000) >> 20);
    This->DebugRegister = DebugPortBase + BarOffset;
    This->PortSc = (UINT32)&EhciOpRegisterPtr->PortSc[This->PortNumber - 1];

    This->PciBusNumber      = Bus;
    This->PciDeviceNumber   = Device;
    This->PciDeviceFunction = Function;

    This->BarIndex          = BarIndex;
    This->BarOffset         = BarOffset;
    This->PortBase          = DebugPortBase;
  }

  return SUCCESS;
#endif // Serial Debug Enabled
}

UINT32
InitializeUsb2DebugPort (
                        PSYSHOST host
                        )
/*++

  Initialize USB2 debug port

  @param host  - Pointer to the system host (root) structure

  @retval EFI_DEVICE_ERROR - device failed
  @retval EFI_SUCCESS - USB2 port init successful

--*/
{
//
// If serial debug is disabled, we simply return an error
//
#ifndef SERIAL_DBG_MSG
  return (UINT32) EFI_DEVICE_ERROR;
#else
  USB2_DEBUG_DESCRIPTOR_TYPE      DebugDescriptor;
  UINT32                          Length;
  UINT32                          Status;
  USB2_DEBUG_PORT_INSTANCE        *Instance;
  USB2_EHCI_CAPABILITY_REGISTER   *EhciCapRegisterPtr;
  USB2_EHCI_OPERATIONAL_REGISTER  *EhciOpRegisterPtr;
  USB2_DEBUG_PORT_REGISTER        *DebugRegisterPtr;
  UINT64_STRUCT startTsc = { 0, 0 };
  UINT64_STRUCT endTsc = { 0, 0 };
  UINT32 msDelay;
  
  msDelay = 2; //assuming using default timings, only need to wait 2ms for controller to halt.
  Instance = &host->var.common.usbDebugPort;

  /*
   U64 startTsc = {0,0};
   U64 endTsc = {0,0};
   UINT32 testLatency = 0;
   UINT32 i;
*/

  DebugRegisterPtr    = (USB2_DEBUG_PORT_REGISTER *) Instance->DebugRegister;
  EhciOpRegisterPtr   = (USB2_EHCI_OPERATIONAL_REGISTER *) Instance->EhciOpRegister;
  EhciCapRegisterPtr  = (USB2_EHCI_CAPABILITY_REGISTER *) Instance->EhciCapRegister;

  //
  // Only execute for SBSP (socket 0)
  //
  if (!host->var.common.socketId) {
    //
    // Reset host controller if necessary
    //
    if (!IS_BIT_SET (&EhciOpRegisterPtr->UsbStatus, USB2_EHCI_USBSTS_HC_HALTED)) {
      CLR_R32_BIT (&EhciOpRegisterPtr->UsbCommand, USB2_EHCI_USBCMD_RUN);
      ReadTsc64(&startTsc);
      ReadTsc64(&endTsc);
      while (!IS_BIT_SET(&EhciOpRegisterPtr->UsbStatus, USB2_EHCI_USBSTS_HC_HALTED) && TimeDiff(host, startTsc, endTsc, TDIFF_UNIT_MS) < msDelay) {
        ReadTsc64(&endTsc);
      }
      if (!IS_BIT_SET(&EhciOpRegisterPtr->UsbStatus, USB2_EHCI_USBSTS_HC_HALTED)){
        //log a warning that host controller cannot halt in 2 ms...
        OutputWarning(host, 0, 0, 0, 0, 0, 0);//none of the codes match, socket/channel/dimm/rank doesn't make sense. Should this be a Fatal error?
      }
    }

    SET_R32_BIT (&EhciOpRegisterPtr->UsbCommand, USB2_EHCI_USBCMD_RESET);
    //
    // ensure that the host controller is reset (RESET bit must be cleared after reset)
    //
    while (IS_BIT_SET (&EhciOpRegisterPtr->UsbCommand, USB2_EHCI_USBCMD_RESET)) {
      FixedDelay (host, 10);
    }
    //
    // Start the host controller if it's not running
    //
    if (IS_BIT_SET (&EhciOpRegisterPtr->UsbStatus, USB2_EHCI_USBSTS_HC_HALTED)) {
      SET_R32_BIT (&EhciOpRegisterPtr->UsbCommand, USB2_EHCI_USBCMD_RUN);
      //
      // ensure that the host controller is started (HALTED bit must be cleared)
      //
      ReadTsc64(&startTsc);
      ReadTsc64(&endTsc);
      while (IS_BIT_SET(&EhciOpRegisterPtr->UsbStatus, USB2_EHCI_USBSTS_HC_HALTED) && TimeDiff(host, startTsc, endTsc, TDIFF_UNIT_MS) < msDelay) {
        ReadTsc64(&endTsc);
      }
      if (IS_BIT_SET(&EhciOpRegisterPtr->UsbStatus, USB2_EHCI_USBSTS_HC_HALTED)){
        //log a warning that host controller cannot clear halted bit within the 2 ms after run bit is set
        OutputWarning(host, 0, 0, 0, 0, 0, 0);//none of the codes match, socket/channel/dimm/rank doesn't make sense. Should this be a Fatal error?
      }
    }
    //
    // Take ownership of the debug port
    //
    SET_R32_BIT (&DebugRegisterPtr->ControlStatus, USB2_DEBUG_PORT_STATUS_OWNER);

    //
    // Power-up the debug port
    //
    if (IS_BIT_SET (&EhciCapRegisterPtr->HcsParams, 0x00000010)) {
      //
      // we can control the port power only if PPC in HCSPARAMS is set
      //
      if (IS_BIT_SET ((UINT32 *)Instance->PortSc, USB2_EHCI_PORTSC_PORT_POWER)) {
        CLR_R32_BIT ((UINT32 *)Instance->PortSc, USB2_EHCI_PORTSC_PORT_POWER);
        FixedDelay (host, 20000);
      }

      SET_R32_BIT ((UINT32 *)Instance->PortSc, USB2_EHCI_PORTSC_PORT_POWER);
    }
    //
    // wait for 250ms
    //
    FixedDelay (host, 250000);

    //
    // Start to reset
    //
    SET_R32_BIT ((UINT32 *)Instance->PortSc, USB2_EHCI_PORTSC_PORT_RESET);
    //
    // wait for 50ms
    //
    FixedDelay (host, 50000);

    //
    // Wait for the reset bit to go low...
    //
    CLR_R32_BIT ((UINT32 *)Instance->PortSc, USB2_EHCI_PORTSC_PORT_RESET);

    //
    // When software writes a zero to this bit there may be a delay before the bit
    // status changes to a zero. The bit status will not read as a zero until after
    // the reset has completed.
    //
    while (!IS_BIT_CLEAR ((UINT32 *)Instance->PortSc, USB2_EHCI_PORTSC_PORT_RESET)) {
      FixedDelay (host, 10);
    }
    //
    // Check to see if the port was enabled...
    //
    if (!IS_BIT_SET ((UINT32 *)Instance->PortSc, USB2_EHCI_PORTSC_PORT_ENABLED)) {
      //
      // ENABLED bit must be set to 1 after reset, if the device is attached and is high speed device.
      //
      return(UINT32) EFI_DEVICE_ERROR;
    }
    //
    // Set that the port is enabled and owned in the DebugPortControl Register
    // Also set the s/w InUse flag and clear the done flag
    //
    SET_R32_BIT (
                &DebugRegisterPtr->ControlStatus,
                USB2_DEBUG_PORT_STATUS_ENABLED |
                USB2_DEBUG_PORT_STATUS_OWNER |
                USB2_DEBUG_PORT_STATUS_INUSE |
                USB2_DEBUG_PORT_STATUS_DONE
                );

    //
    // Reset the PORT Enabled/Disabled bit so that the system host controller
    // driver doesn't see an enabled port when it is first loaded
    //
    CLR_R32_BIT ((UINT32 *)Instance->PortSc, USB2_EHCI_PORTSC_PORT_ENABLED);
    //
    // Turn off the host controller by setting the RUN/STOP bit to a zero
    //
    CLR_R32_BIT (&EhciOpRegisterPtr->UsbCommand, USB2_EHCI_USBCMD_RUN);

    // Start timer for serial buffer
    host->var.common.usbStartCount  = GetCount (host);
    host->var.common.serialBufEnable = 1;

    //
    // try to get the debug descriptor at address 127
    //
    DebugRegisterPtr->UsbAddress  = USB2_DEBUG_PORT_DEFAULT_ADDRESS;
    DebugRegisterPtr->UsbEndPoint = 0;
    Status                        = Usb2DebugPortControlTransfer (host, &mGetDebugDescriptor, (UINT8 *) &DebugDescriptor, &Length);
    if (EFI_ERROR (Status)) {
      //
      // try address 0
      //
      DebugRegisterPtr->UsbAddress  = 0;
      Status                        = Usb2DebugPortControlTransfer (host, &mGetDebugDescriptor, (UINT8 *) &DebugDescriptor, &Length);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      //
      // set address to 127
      //
      Status = Usb2DebugPortControlTransfer (host, &mSetAddress, NULL, NULL);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      FixedDelay (host, 100000);
      DebugRegisterPtr->UsbAddress  = USB2_DEBUG_PORT_DEFAULT_ADDRESS;
    }
    //
    //    Instance->ReadEndpoint = DebugDescriptor.DebugInEndpoint;
    //    Instance->WriteEndpoint = DebugDescriptor.DebugOutEndpoint;
    //    rcPrintf (("DebugInEndpoint = 0x%2x,  DebugOutEndpoint = 0x%2x\n", DebugDescriptor.DebugInEndpoint, DebugDescriptor.DebugOutEndpoint));
    //
    // SetFeature - DEBUG_MODE
    //
    Status = Usb2DebugPortControlTransfer (host, &mSetDebugFeature, NULL, NULL);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  //
  // if(!host->var.common.socketId)
  // Hard-code variables for AP threads
  //
  Instance->ReadEndpoint        = EP_IN;
  Instance->WriteEndpoint       = EP_OUT;

  //
  // Init the data toggle and other fields
  //
  Instance->WriteEndpointDataToggle = 0;
  Instance->ReadEndpointDataToggle  = 0;

  if (!Instance->Ready) {

    Instance->Ready = TRUE;

    //
    //      Instance->Ready = FALSE;
    // Debug
    // rcPrintf (("123456789abcdefghijk\n"));
    // rcPrintf (("123456789abcdefghijk\n"));
    // rcPrintf (("123456789abcdefghijk\n"));
    //
    /*
      // Measure latency of USB Debug port
      ReadTsc64(&startTsc);
      for (i = 0; i < 16 *1024; i++) {
         rcPrintf (("This is a performance test of the serial interface\n"));
      }
      SerialSendBuffer(1);

      ReadTsc64(&endTsc);
      testLatency = TimeDiff(host, startTsc, endTsc, TDIFF_MS);
      rcPrintf (("\nUSB debug port test latency = %d ms\n", testLatency));
      SerialSendBuffer(1);

      // Measure latency of Serial port
      Instance->Ready = FALSE;
      for (i = 0; i < 16 *1024; i++) {
         rcPrintf (("This is a performance test of the serial interface\n"));
      }
      SerialSendBuffer(1);

      Instance->Ready = TRUE;
      ReadTsc64(&endTsc);
      testLatency = TimeDiff(host, startTsc, endTsc, TDIFF_MS);
      rcPrintf (("\nSerial port test latency = %d ms\n\n", testLatency));
      SerialSendBuffer(1);
*/
  }

  return EFI_SUCCESS;
#endif // Serial Debug enabled
}
