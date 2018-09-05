

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PrintLib.h>





//
// Define the maximum message length 
//
#define MAX_DEBUG_MESSAGE_LENGTH  0x200

#define LSR_OFFSET                0x05
#define LSR_TXRDY                 0x20
#define SerialPortBase            0x3f8

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
SerialPortWriteUsra (
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
      Data = IoRead8 (SerialPortBase + LSR_OFFSET);
    } while ((Data & LSR_TXRDY) == 0);
    IoWrite8 (SerialPortBase, *Buffer++);
  }

  return Result;
}



/**
  Prints a message to the serial port.

  @param  Format      Format string for the message to print.
  @param  ...         Variable argument list whose contents are accessed 
                      based on the format string specified by Format.

**/
VOID
EFIAPI
InternalPrintMessage (
  IN  CONST CHAR8  *Format,
  ...
  )
{
  CHAR8    Buffer[MAX_DEBUG_MESSAGE_LENGTH];
  VA_LIST  Marker;

  //
  // Convert the message to an ASCII String
  //
  VA_START (Marker, Format);
  AsciiVSPrint (Buffer, sizeof (Buffer), Format, Marker);
  VA_END (Marker);

  //
  // Send the print string to a Serial Port 
  //
  SerialPortWriteUsra ((UINT8 *)Buffer, AsciiStrLen (Buffer));
}

