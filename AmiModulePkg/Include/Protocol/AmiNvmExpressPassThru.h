/** @file
  The EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL provides services that allow NVM Express commands
  to be sent to an NVM Express controller or to a specific namespace in a NVM Express controller.

  Copyright (c) 2014 - 2015, American Megatrends, Inc. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL_H__
#define __EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL_H__

typedef struct _EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL;

#define EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL_GUID \
  {0x52c78312, 0x8edc, 0x4233, { 0x98, 0xf2, 0x1a, 0x1a, 0xa5, 0xe3, 0x88, 0xa5 } }

///
/// If this bit is set, then the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL interface
/// is for directly addressable namespaces.
///
#define EFI_NVM_EXPRESS_PASS_THRU_ATTRIBUTES_PHYSICAL      0x0001
///
/// If this bit is set, then the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL interface
/// is for a single volume logical namespace comprised of multiple namespaces.
///
#define EFI_NVM_EXPRESS_PASS_THRU_ATTRIBUTES_LOGICAL       0x0002
///
/// If this bit is set, then the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL interface
/// supports non-blocking I/O.
///
#define EFI_NVM_EXPRESS_PASS_THRU_ATTRIBUTES_NONBLOCKINGIO 0x0004
///
/// If this bit is set, then the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL interface
/// supports NVM command set.
///
#define EFI_NVM_EXPRESS_PASS_THRU_ATTRIBUTES_CMD_SET_NVME  0x0008
#define EFI_NVM_EXPRESS_PASS_THRU_ATTRIBUTES_CMD_SET_NVM   0x0008

///
/// FusedOperation
///
///
#define NORMAL_CMD          0x00
#define FUSED_FIRST_CMD     0x01
#define FUSED_SECOND_CMD    0x02
///

///
/// Flags
///
///
#define CDW2_VALID       0x01
#define CDW3_VALID       0x02
#define CDW10_VALID      0x04
#define CDW11_VALID      0x08
#define CDW12_VALID      0x10
#define CDW13_VALID      0x20
#define CDW14_VALID      0x40
#define CDW15_VALID      0x80
///

typedef struct {
  UINT32    Attributes;
  UINT32    IoAlign;
  UINT32    NvmeVersion;
} EFI_NVM_EXPRESS_PASS_THRU_MODE;

typedef struct { 
  UINT8    OpCode;
  UINT8    FusedOperation;
  UINT16   Unused;
} NVME_CDW0;

typedef struct {
  NVME_CDW0  Cdw0;
  UINT8      Flags;
  UINT32     Nsid;
  UINT32     Cdw2;
  UINT32     Cdw3;
  UINT32     Cdw10;
  UINT32     Cdw11;
  UINT32     Cdw12;
  UINT32     Cdw13;
  UINT32     Cdw14;
  UINT32     Cdw15;
} EFI_NVM_EXPRESS_COMMAND;

///
/// This structure maps to the NVM Express specification Completion Queue Entry 
///
typedef struct { 
  UINT32    DW0;
  UINT32    DW1;
  UINT32    DW2;
  UINT32    DW3;
} EFI_NVM_EXPRESS_COMPLETION;

typedef struct _EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET {
  /// The timeout, in 100 ns units, to use for the execution of this 
  /// NVM Express Command Packet. A Timeout value of 0 means that this 
  /// function will wait indefinitely for the command to execute. 
  /// If Timeout is greater than zero, then this function will return 
  /// EFI_TIMEOUT if the time required to execute the NVM Express 
  /// command is greater than Timeout
  UINT64                      CommandTimeout;
  /// 
  /// A pointer to the data buffer to transfer between the host, and the 
  /// NVM Express controller for read, write, and bi-directional commands
  ///
  VOID                        *TransferBuffer;
  ///
  /// On input, the size in bytes of TransferBuffer. On output, the number
  /// of bytes transferred to the NVM Express controller, or namespace
  ///
  UINT32                      TransferLength;
  ///
  /// A pointer to the optional metadata buffer to transfer between the host,
  /// and the NVM Express controller.If this field is not NULL, then it must
  /// be aligned on the boundary specified by the IoAlign field in the 
  /// EFI_NVM_EXPRESS_PASS_THRU_MODE structure.
  ///
  VOID                        *MetaDataBuffer;
  ///
  /// On Input, the size in bytes of MetadataBuffer. On output, the number 
  /// of bytes transferred to/from the NVM Express controller, or namespace
  ///
  UINT32                      MetadataLength;
  ///
  /// The type of the queue that the NVMe command should be posted to.
  /// A value of 0 indicates it should be posted to the Admin Submission Queue.
  /// A value of 1 indicates it should be posted to an I/O Submission Queue
  ///
  UINT16                      QueueType;
  ///
  /// A pointer to an NVM Express Command Packet.
  ///
  EFI_NVM_EXPRESS_COMMAND     *NvmeCmd;
  ///
  /// The raw NVM Express completion queue 
  ///
  EFI_NVM_EXPRESS_COMPLETION  *NvmeCompletion;
} EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET;

/**
    Sends an NVM Express Command Packet to an NVM Express controller or namespace

    @param This         A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance
    @param NamespaceId  A 32 bit namespace ID as defined in the NVMe specification 
                        to which the NVM Express Command Packet will be sent.  
    @param Packet       A pointer to the NVM Express Command Packet to send to
                        the NVMe namespace specified by NamespaceId. 
    @param Event        Event to be signalled when the NVM Express Command Packet completes
                        and non-blocking I/O is supported

    @retval EFI_SUCCESS          The NVM Express Command Packet was sent by the host successfully
    @retval EFI_BAD_BUFFER_SIZE  The number of bytes that could be transferred is not valid
    @retval EFI_NOT_READY        The NVM Express Command Packet could not be sent as the controller
                                 is not ready
    @retval EFI_DEVICE_ERROR     Device error
    @retval EFI_UNSUPPORTED      The command described by the NVM Express Command Packet is not 
                                 supported by the host adapter
    @retval EFI_TIMEOUT          A timeout occurred while waiting for the NVM Express Command Packet
                                 to execute

**/
typedef
EFI_STATUS 
(EFIAPI *EFI_NVM_EXPRESS_PASS_THRU_PASSTHRU) ( 
  IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL            *This,
  IN UINT32                                        NamespaceId,
  IN OUT EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET  *Packet,
  IN EFI_EVENT                                     Event
  );

/**
    Used to retrieve the next namespace ID for this NVM Express controller.  

    @param This         A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance
    @param NamespaceId  On input, a pointer to a valid namespace ID on this NVM Express controller
	                    or a pointer to the value 0xFFFFFFFF. A pointer to the value 0xFFFFFFFF retrieves 
						the first valid namespace ID on this NVM Express controller.

    @retval EFI_SUCCESS   The Namespace ID of the next Namespace was returned
    @retval EFI_NOT_FOUND There are no more namespaces defined on this controller
    @retval EFI_INVALID_PARAMETER The Namespace ID was not valid

**/
typedef 
EFI_STATUS
(EFIAPI *EFI_NVM_EXPRESS_PASS_THRU_GET_NEXT_NAMESPACE) (
  IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL    *This,
  IN OUT UINT32                            *NamespaceId
  );

/**
    Used to allocate and build a device path node for an NVM Express namespace 
    on an NVM Express controller

    @param This         A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance
    @param NamespaceId  The NVM Express namespace ID for which a device path node is
                        to be allocated and built
    @param DevicePath   A pointer to a single device path node that describes the 
                        NVM Express namespace specified by NamespaceId. 
						This function is responsible for allocating the buffer DevicePath
						with the boot service AllocatePool(). It is the caller's responsibility
						to free DevicePath when the caller is finished with DevicePath.

    @retval EFI_SUCCESS           The device path node that describes the NVM Express 
                                  namespace specified by NamespaceId was allocated and 
                                  returned in DevicePath
    @retval EFI_NOT_FOUND         The NamespaceIdis not valid
    @retval EFI_INVALID_PARAMETER DevicePath is NULL
    @retval EFI_OUT_OF_RESOURCES  There are not enough resources to allocate the 
                                  DevicePath node.
**/
typedef
EFI_STATUS 
(EFIAPI *EFI_NVM_EXPRESS_PASS_THRU_BUILD_DEVICE_PATH) (
  IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL    *This,
  IN UINT32                                NamespaceId,
  IN OUT EFI_DEVICE_PATH_PROTOCOL          **DevicePath
  ); 

/**
    Used to translate a device path node to a namespace ID
    
    @param This         A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance
    @param DevicePath   A pointer to the device path node that describes an NVM 
                        Express namespace on the NVM Express controller. 
    @param NamespaceId  The NVM Express namespace ID contained in the device path node
 
    @retval EFI_SUCCESS    Namespace ID is returned in NamespaceID

**/
typedef
EFI_STATUS 
(EFIAPI *EFI_NVM_EXPRESS_PASS_THRU_GET_NAMESPACE) (
  IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL    *This,
  IN EFI_DEVICE_PATH_PROTOCOL              *DevicePath,
  OUT UINT32                               *NamespaceId
  ); 

struct _EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL {
  EFI_NVM_EXPRESS_PASS_THRU_MODE                 *Mode;
  EFI_NVM_EXPRESS_PASS_THRU_PASSTHRU             PassThru;
  EFI_NVM_EXPRESS_PASS_THRU_GET_NEXT_NAMESPACE   GetNextNamespace;
  EFI_NVM_EXPRESS_PASS_THRU_BUILD_DEVICE_PATH    BuildDevicePath;
  EFI_NVM_EXPRESS_PASS_THRU_GET_NAMESPACE        GetNameSpace;
};

extern EFI_GUID gEfiNvmExpressPassThruProtocolGuid;

#endif
