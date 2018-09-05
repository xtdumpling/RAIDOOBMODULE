/** @file
  Smart card aware application invokes this protocol to get access to an inserted smart card in the
  reader or to the reader itself.

  Copyright (c) 2014 - 2015, American Megatrends, Inc. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __EFI_SMART_CARD_READER_PROTOCOL_H__
#define __EFI_SMART_CARD_READER_PROTOCOL_H__

typedef struct _EFI_SMART_CARD_READER_PROTOCOL EFI_SMART_CARD_READER_PROTOCOL;

#define EFI_SMART_CARD_READER_PROTOCOL_GUID \
  {0x2a4d1adf, 0x21dc, 0x4b81, {0xa4, 0x2f, 0x8b, 0x8e, 0xe2, 0x38, 0x00, 0x60} }

//
// Codes for access mode
//
///
#define SCARD_AM_READER 0x0001 // Exclusive access to reader
#define SCARD_AM_CARD 0x0002 // Exclusive access to card
///

//
// Codes for card action
//
///
#define SCARD_CA_NORESET 0x0000 // Don’t reset card
#define SCARD_CA_COLDRESET 0x0001 // Perform a cold reset
#define SCARD_CA_WARMRESET 0x0002 // Perform a warm reset
#define SCARD_CA_UNPOWER 0x0003 // Power off the card
#define SCARD_CA_EJECT 0x0004 // Eject the card
///

//
// Protocol types
//
///
#define SCARD_PROTOCOL_UNDEFINED 0x0000
#define SCARD_PROTOCOL_T0 0x0001
#define SCARD_PROTOCOL_T1 0x0002
#define SCARD_PROTOCOL_RAW 0x0004
///

//
// Codes for state type
//
///
#define SCARD_UNKNOWN 0x0000 ///< state is unknown
#define SCARD_ABSENT 0x0001 ///< Card is absent
#define SCARD_INACTIVE 0x0002 ///< Card is present and not powered
#define SCARD_ACTIVE 0x0003 ///< Card is present and powered
///

//
// Macro to generate a ControlCode & PC/SC part 10 control code
//
///
#define SCARD_CTL_CODE(code) (0x42000000 + (code))
#define CM_IOCTL_GET_FEATURE_REQUEST SCARD_CTL_CODE(3400)
///

/**
    This function requests connection to the smart card or the reader, using the appropriate reset type
    and protocol.

    @param This               A pointer to the EFI_SMART_CARD_READER_PROTOCOL instance
    @param AccessMode         Access mode (SCARD_AM_READER or SCARD_AM_CARD)
    @param CardAction         Card action(reset type) that that has to be performed
                              (SCARD_CA_NORESET, SCARD_CA_COLDRESET or SCARD_CA_WARMRESET)
    @param PreferredProtocols Bit mask of acceptable protocols
    @param ActiveProtocol     A flag that indicates the active protocol

    @retval EFI_SUCCESS           The requested command completed successfully
    @retval EFI_INVALID_PARAMETER Either AccessMode or CardAction is not valid or
                                  invalid combination of AccessMode/CardAction/PreferredProtocols
    @retval EFI_NOT_READY         A smart card is inserted but failed to return an ATR
    @retval EFI_DEVICE_ERROR      Any other error condition, typically a reader removal
    @retval EFI_ACCESS_DENIED     Access is already locked by a previous SCardConnect call
    @retval EFI_NO_MEDIA          AccessMode is set to SCARD_AM_CARD but there is no smart
                                  card inserted
    @retval EFI_UNSUPPORTED       PreferredProtocols does not contain an available protocol to use

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMART_CARD_READER_PROTOCOL_CONNECT) (
  IN EFI_SMART_CARD_READER_PROTOCOL *This,
  IN UINT32                         AccessMode,
  IN UINT32                         CardAction,
  IN UINT32                         PreferredProtocols,
  OUT UINT32                        *ActiveProtocol
  );

/**
    This function releases a connection previously taken by SCardConnect.

    @param This        A pointer to the EFI_SMART_CARD_READER_PROTOCOL instance
    @param CardAction  Card action(reset type) that that has to be performed
                       (SCARD_CA_NORESET, SCARD_CA_COLDRESET or SCARD_CA_WARMRESE

    @retval EFI_SUCCESS           The requested command completed successfully
    @retval EFI_INVALID_PARAMETER CardAction value is unknown 
    @retval EFI_DEVICE_ERROR      Any other error condition, typically a reader removal
    @retval EFI_UNSUPPORTED       Reader does not support Eject card feature (disconnect was not
                                  performed).

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMART_CARD_READER_PROTOCOL_DISCONNECT) (
  IN EFI_SMART_CARD_READER_PROTOCOL *This,
  IN UINT32                         CardAction
  );

/**
    This function retrieves some basic information about the smart card and reader.

    @param This              A pointer to the EFI_SMART_CARD_READER_PROTOCOL instance
    @param ReaderName        A pointer to a NULL terminated string that will contain the reader name
    @param ReaderNameLength  On input, a pointer to the variable that holds the maximal size of 
                             ReaderName in bytes.
                             On output, the actual size, in bytes, of the returned ReaderName string.
    @param State             Current state of the smart card reader
    @param CardProtocol      Current protocol used to communicate with the smart card
    @param Atr               Pointer to retrieve the ATR of the smart card
    @param AtrLength         On input, a pointer to hold the maximum size, in bytes, of Atr (usually 33).
                             On output, the required size, in bytes, for the smart card ATR

    @retval EFI_SUCCESS           The requested command completed successfully.
    @retval EFI_INVALID_PARAMETER Atr is not NULL but AtrLength is NULL or
                                  ReaderName is not NULL but ReaderNameLength is NULL
    @retval EFI_DEVICE_ERROR      Any other error condition, typically a reader removal
    @retval EFI_BUFFER_TOO_SMALL  ReaderNameLength is not big enough to hold the reader name.
                                  ReaderNameLength has been updated to the required value
                                  or 
                                  AtrLength is not big enough to hold the ATR.
                                  AtrLength has been updated to the required value.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMART_CARD_READER_PROTOCOL_STATUS) (
  IN EFI_SMART_CARD_READER_PROTOCOL *This,
  OUT CHAR16                        *ReaderName,
  IN OUT UINTN                      *ReaderNameLength,
  OUT UINT32                        *State,
  OUT UINT32                        *CardProtocol,
  OUT UINT8                         *Atr,
  IN OUT UINTN                      *AtrLength
  );

/**
    This function sends a command to the card or reader and returns its response.

    @param This         A pointer to the EFI_SMART_CARD_READER_PROTOCOL instance
    @param CAPDU        Pointer to a byte array that contains the Command APDU to 
                        send to the smart card or reader
    @param CAPDULength  Command APDU size in bytes
    @param RAPDU        Pointer to a byte array that will contain the Response APDU
    @param RAPDULength  On input, the maximum size of the Response APDU in bytes
                        On output, the size of the Response APDU in bytes

    @retval EFI_SUCCESS           The requested command completed successfully.
    @retval EFI_INVALID_PARAMETER CAPDU is NULL or CAPDULength is 0. 
    @retval EFI_DEVICE_ERROR      Any other error condition, typically a reader removal
    @retval EFI_NO_MEDIA          There is no card in the reader.
    @retval EFI_BUFFER_TOO_SMALL  RAPDULength is not big enough to hold the response APDU.
                                  RAPDULength has been updated to the required value.
    @retval EFI_TIMEOUT           The reader did not respond
    @retval EFI_ACCESS_DENIED     A communication with the reader/card is already pending
    @retval EFI_NOT_READY         Card is not powered

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMART_CARD_READER_PROTOCOL_TRANSMIT) (
  IN EFI_SMART_CARD_READER_PROTOCOL *This,
  IN UINT8                          *CAPDU,
  IN UINTN                          CAPDULength,
  OUT UINT8                         *RAPDU,
  IN OUT UINTN                      *RAPDULength
  );

/**
    This function provides direct access to the reader.

    @param This            A pointer to the EFI_SMART_CARD_READER_PROTOCOL instance
    @param ControlCode     Control code for the operation to perform
    @param InBuffer        Pointer to the input parameters
    @param InBufferLength  Size of the input parameters in bytes
    @param OutBuffer       Pointer to the output parameters
    @param OutBufferLength On input, maximal size to store output parameters in bytes
                           On output, the size of output parameters in bytes

    @retval EFI_SUCCESS           The requested command completed successfully
    @retval EFI_INVALID_PARAMETER OutBuffer is not NULL but OutBufferLength is NULL or
                                  ControlCode is not supported.
    @retval EFI_DEVICE_ERROR      Any other error condition, typically a reader removal
    @retval EFI_NO_MEDIA          There is no card in the reader and the control code specified requires
                                  one
    @retval EFI_BUFFER_TOO_SMALL  OutBufferLength is not big enough to hold the output
                                  parameters. OutBufferLength has been updated to the required value
    @retval EFI_TIMEOUT           The reader did not respond
    @retval EFI_ACCESS_DENIED     A communication with the reader/card is already pending
    @retval EFI_NOT_READY         ControlCode requires a powered card to operate
    @retval EFI_PROTOCOL_ERROR    A protocol error has occurred   
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMART_CARD_READER_PROTOCOL_CONTROL) (
  IN EFI_SMART_CARD_READER_PROTOCOL *This,
  IN UINT32                         ControlCode,
  IN UINT8                          *InBuffer,
  IN UINTN                          *InBufferLength,
  OUT UINT8                         *OutBuffer,
  IN OUT UINTN                      *OutBufferLength
  );

/**
    This function retrieves a reader or smart card attribute.

    @param This            A pointer to the EFI_SMART_CARD_READER_PROTOCOL instance
    @param Attrib          Identifier for the attribute to retrieve
    @param OutBuffer       Pointer to a buffer that will contain attribute data
    @param OutBufferLength On input, maximal size to store attribute data in bytes
                           On output, the size of the attribute data in bytes

    @retval EFI_SUCCESS           The requested command completed successfully.
    @retval EFI_INVALID_PARAMETER OutBuffer is NULL or OutBufferLength is 0
    @retval EFI_UNSUPPORTED       Attrib is not supported                                 
    @retval EFI_NO_MEDIA          There is no card in the reader and Attrib value requires one
    @retval EFI_BUFFER_TOO_SMALL  OutBufferLength is not big enough to hold the output
                                  parameters. OutBufferLength has been updated to the required value
    @retval EFI_PROTOCOL_ERROR    A protocol error has occurred
    @retval EFI_TIMEOUT           The reader did not respond
    @retval EFI_NOT_READY         Attrib requires a powered card to operate
    @retval EFI_DEVICE_ERROR      Any other error condition, typically a reader removal
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMART_CARD_READER_PROTOCOL_GET_ATTRIB) (
  IN EFI_SMART_CARD_READER_PROTOCOL *This,
  IN UINT32                         Attrib,
  OUT UINT8                         *OutBuffer,
  IN OUT UINTN                      *OutBufferLength
  );

typedef struct _EFI_SMART_CARD_READER_PROTOCOL {
  EFI_SMART_CARD_READER_PROTOCOL_CONNECT       SCardConnect;
  EFI_SMART_CARD_READER_PROTOCOL_DISCONNECT    SCardDisConnect;
  EFI_SMART_CARD_READER_PROTOCOL_STATUS        SCardStatus;
  EFI_SMART_CARD_READER_PROTOCOL_TRANSMIT      ScardTransmit;
  EFI_SMART_CARD_READER_PROTOCOL_CONTROL       SCardControl;
  EFI_SMART_CARD_READER_PROTOCOL_GET_ATTRIB    SCardGetAttrib;
};

extern EFI_GUID gEfiSmartCardReaderProtocolGuid;

#endif
