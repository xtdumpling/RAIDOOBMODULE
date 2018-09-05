/*++

INTEL CONFIDENTIAL
Copyright 2005-2013 Intel Corporation All Rights Reserved.

The source code contained or described herein and all documents
related to the source code ("Material") are owned by Intel Corporation
or its suppliers or licensors. Title to the Material remains with
Intel Corporation or its suppliers and licensors. The Material
contains trade secrets and proprietary and confidential information of
Intel or its suppliers and licensors. The Material is protected by
worldwide copyright and trade secret laws and treaty provisions. No
part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed, or disclosed in
any way without Intel's prior express written permission.

No license under any patent, copyright, trade secret or other
intellectual property right is granted to or conferred upon you by
disclosure or delivery of the Materials, either expressly, by
implication, inducement, estoppel or otherwise. Any license under such
intellectual property rights must be express and approved by Intel in
writing.

File Name:

   me_status.h

Abstract:

   Defines the ME Status codes

Authors:

   Hemaprabhu Jayanna
   
--*/

#ifndef _ME_STATUS_H_
#define _ME_STATUS_H_

///////////////////////////////////////////////////////////////////////////////
//                        ERROR & STATUS CODE RANGES                         //
///////////////////////////////////////////////////////////////////////////////
//          RANGE           //                 DESCRIPTION                   //
///////////////////////////////////////////////////////////////////////////////
// 0x0000FFFF - 0x00000000  // Pre-defined standard error values used by all //
//                          // ME Components and interfaces                  //
///////////////////////////////////////////////////////////////////////////////
// 0x0001FFFF - 0x00010000  // Capability Module specific error codes        //
///////////////////////////////////////////////////////////////////////////////
// 0xFFFFFFFF - 0x00020000  // Reserved for future use.                      //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//                    KERNEL STATUS AND ERROR CODES                          //
//The following are the status and error codes returned by the ME Kernel.    //
//ThreadX Codes range from (0x00 to 0x7F)                                    //
//Other Kernel codes range from (0x80 to 0xFF)                               //
///////////////////////////////////////////////////////////////////////////////




//
//STATUS_SUCCESS
//    The operation completed successfully.
//

/**
 * @brief The operation completed successfully.
 */
#define STATUS_SUCCESS                0x0

//
//STATUS_DELETED
//    The requested object has been deleted.
// 
#define STATUS_DELETED                0x1

//
//STATUS_POOL_ERROR
//    An invalid memory pool pointer (NULL) was given or the pool is 
//    already created.
//
#define STATUS_POOL_ERROR             0x2

//
//STATUS_PTR_ERROR
//    An invalid pointer was provided.
//
#define STATUS_PTR_ERROR              0x3
 
//
//STATUS_WAIT_ERROR
//    The specified wait option is invalid.
//
#define STATUS_WAIT_ERROR             0x4
 
//
//STATUS_SIZE_ERROR
//    The size of the specified object (memory pool, stack, message queue, 
//    etc.) is invalid. 
//
#define STATUS_SIZE_ERROR             0x5
 
//
//STATUS_GROUP_ERROR
//    An invalid group pointer was provided.  The pointer is NULL or points 
//    to an already created group.
//
#define STATUS_GROUP_ERROR            0x6
 
//
//STATUS_NO_EVENTS
//    The requested event could not be found.
//
#define STATUS_NO_EVENTS              0x7
 
//
//STATUS_OPTION_ERROR
//    Invalid event flag set option (AND, OR).
//
#define STATUS_OPTION_ERROR           0x8
 
//
//STATUS_QUEUE_ERROR
//    An invalid queue pointer passed or the queue is already created.
//
#define STATUS_QUEUE_ERROR            0x9
 
//
//STATUS_QUEUE_EMPTY
//    The requested queue is empty.
//
#define STATUS_QUEUE_EMPTY            0xA
 
//
//STATUS_QUEUE_FULL
//    The requested queue is full.
//
#define STATUS_QUEUE_FULL             0xB
 
//
//STATUS_SEMAPHORE_ERROR
//    An invalid semaphore pointer provided or the semaphore is already created.
//
#define STATUS_SEMAPHORE_ERROR        0xC
 
//
//STATUS_NO_INSTANCE
//    The requested instance was not found (semaphore count is 0x0).
//
#define STATUS_NO_INSTANCE            0xD
 
//
//STATUS_THREAD_ERROR
//    An invalid thread control pointer provided or the thread is already created.
//
#define STATUS_THREAD_ERROR           0xE
 
//
//STATUS_PRIORITY_ERROR
//    An invalid thread priority level has been given.
//
#define STATUS_PRIORITY_ERROR         0xF
 
//
//STATUS_START_ERROR
//    An invalid auto-start selection was given during thread creation.
//    NOTE: This value is the same a as STATUS_NO_MEMORY.
//
#define STATUS_START_ERROR            0x10

//
//STATUS_DELETE_ERROR
//    The requested object can not be deleted.  This can occur if an attempt is 
//    made to delete a thread that is not in a terminated or completed state.
//
#define STATUS_DELETE_ERROR           0x11

//
//STATUS_RESUME_ERROR
//    The thread can not be resumed (e.g. the specified thread is not in a 
//    suspended state or was suspended by a driver other than the RTOS protocol).
// 
#define STATUS_RESUME_ERROR           0x12

//
//STATUS_CALLER_ERROR
//    Invalid caller of this service.
// 
#define STATUS_CALLER_ERROR           0x13

//
//STATUS_SUSPEND_ERROR
//    The thead can not be suspended (e.g. the specified thread is in a 
//    terminated or completed state).
// 
#define STATUS_SUSPEND_ERROR          0x14

//
//STATUS_TIMER_ERROR
//    An invalid timer object pointer was given.
// 
#define STATUS_TIMER_ERROR            0x15

//
//STATUS_TICK_ERROR
//    An invalid value (0x0) supplied for the initial tick count.
// 
#define STATUS_TICK_ERROR             0x16

//
//STATUS_ACTIVATE_ERROR
//    The object is already active.
// 
#define STATUS_ACTIVATE_ERROR         0x17
 
//
//STATUS_THRESH_ERROR
//    An invalid preemption threashold was specified. 
//
#define STATUS_THRESH_ERROR           0x18

//
//STATUS_SUSPEND_LIFTED
//    An attempt to resume a thread marked for delayed suspension has 
//    occurred.
// 
#define STATUS_SUSPEND_LIFTED         0x19

//
//STATUS_WAIT_ABORTED
//    The wait condition on which the thread is waiting has been aborted.
// 
#define STATUS_WAIT_ABORTED           0x1A

//
//STATUS_WAIT_ABORT_ERROR
//    An attempt to abort a waiting thread was attempted on a thread that is 
//    not in a waiting condition (terminated, ready, completed, etc.).
// 
#define STATUS_WAIT_ABORT_ERROR       0x1B

//
//STATUS_MUTEX_ERROR
//    An invalid mutex pointer was provided.
// 
#define STATUS_MUTEX_ERROR            0x1C

//
//STATUS_NOT_AVAILABLE
//    An attempt to remove a mutex on which a thread is suspended has been 
//    attempted.
// 
#define STATUS_NOT_AVAILABLE          0x1D

//
//STATUS_NOT_OWNED
//    An attempt to access a mutex not owned by the caller was made.
// 
#define STATUS_NOT_OWNED              0x1E

//
//STATUS_INHERIT_ERROR
//    An invalid inherit option was given when trying to create a mutex.
// 
#define STATUS_INHERIT_ERROR          0x1F

//
//STATUS_TIMER_RUNNING
//    An attempt to set an alarm was made while alarm is already enabled.
// 
#define STATUS_TIMER_RUNNING          0x20

//
//STATUS_INVALID_FUNCTION
//    The requested function was not found.
//
#define STATUS_INVALID_FUNCTION       0x80

//
//STATUS_NOT_FOUND
//    The requested item could not be found.
// 
#define STATUS_NOT_FOUND              0x81

//
//STATUS_ACCESS_DENIED
//    Access to the requested object could not be granted.
// 
#define STATUS_ACCESS_DENIED          0x82
 
//
//STATUS_INVALID_HANDLE
//    The provided handle does not exist.
//
#define STATUS_INVALID_HANDLE         0x83
 
//
//STATUS_INVALID_ACCESS
//    An attempt was made to access an inaccessible or unavailable object.
//
#define STATUS_INVALID_ACCESS         0x84

//
//STATUS_INVALID_PARAMS
//    One or more parameters are invalid.
// 
#define STATUS_INVALID_PARAMS         0x85

//
//STATUS_INVALID_PARAMS
//    Request could not be processed because a Power Management event is already
//    in progress.
// 
#define STATUS_PM_EVENT_IN_PROGRESS   0x86

//
//STATUS_WRITE_PROTECTED
//    The media is write protected.  When writing to flash, this means 
//    the flash block is write protected.
// 
#define STATUS_WRITE_PROTECTED        0x87

//
//STATUS_NOT_READY
//    The h/w device is not ready.
// 
#define STATUS_NOT_READY              0x88

//
//STATUS_NOT_SUPPORTED
//    The function, message or request is not recognized or supported.
// 
#define STATUS_NOT_SUPPORTED          0x89

//
//STATUS_NETWORK_BUSY
//    The network device is busy.
// 
#define STATUS_NETWORK_BUSY           0x8A

//
//STATUS_DEVICE_ERROR
//    The h/w device reported an error while attempting the operation.
// 
#define STATUS_DEVICE_ERROR           0x8B

//
//STATUS_INVALID_ADDRESS
//    The address provided in invalid.
// 
#define STATUS_INVALID_ADDRESS        0x8C

//
//STATUS_INVALID_COMMAND
//    The requested command is unrecognized or invalid.
// 
#define STATUS_INVALID_COMMAND        0x8D

//
//STATUS_STACK_OVERFLOW 
//    The request caused a stack overflow.
//
#define STATUS_STACK_OVER_FLOW        0x8E

//
//STATUS_BUFFER_TOO_SMALL
//    The provided buffer is too small to hold the requested data.
// 
#define STATUS_BUFFER_TOO_SMALL       0x8F

//
//STATUS_LOAD_ERROR 
//    The image failed to load.
//
#define STATUS_LOAD_ERROR             0x90

//
//STATUS_INVALID_BUFFER 
//    The buffer was not the proper size for the request.
//
#define STATUS_INVALID_BUFFER         0x91

//
//STATUS_NO_RESPONSE
//    No response was received from the requested target.
// 
#define STATUS_NO_RESPONSE            0x92

//
//STATUS_TIMED_OUT
//    The device or object timed out and was unable to complete the request.
// 
#define STATUS_TIMED_OUT              0x93

// 
//STATUS_NOT_STARTED
//    The requested operation could not be started.  For protocols, this 
//    error indicates the protocol has not been started.
//
#define STATUS_NOT_STARTED            0x94

//
//STATUS_ALREADY_STARTED
//    The requested operation has already been started.  
//    - For protocols, this error 
//    indicates the protocol has already been started.  
//    - For Policy Manager this error means
//    that the MKHI group ID is already registered.
//    For FW update, we only allow one process to do the update at a time, this means error
//    case where FW update is already in progress, the current process will have to try later.
// 
#define STATUS_ALREADY_STARTED        0x95

//
//STATUS_PROTOCOL_ERROR
//    The protocol in use has generated an error.  For network traffic this error 
//    means the networking protocol returned and error.  When returned from a 
//    protocol this means the protocol itself generated an error.
// 
#define STATUS_PROTOCOL_ERROR         0x96

//
//STATUS_INCOMPATIBLE_VERSION
//    The version of the object is incompatible with the request.  For protocols, 
//    this indicates that the protocol version number is not supported.
// 
#define STATUS_INCOMPATIBLE_VERSION   0x97

//
//STATUS_SECURITY_VIOLATION
//    A security violation has occurred.
// 
#define STATUS_SECURITY_VIOLATION     0x98

//
//STATUS_AUTHENTICATION_FAIL
//    The authentication of an object has failed.
// 
#define STATUS_AUTHENTICATION_FAIL    0x99
 
//
//STATUS_INVALID_MEM_TYPE
//    The target memory is invalid for the requested operation (e.g. DMA to 
//    ROM or cypto DMA from system memory).
//
#define STATUS_INVALID_MEM_TYPE       0x9A
 
//
//STATUS_MODE_NOT_SUPPORTED
//    The type of blocking mode is not supported.  Refer to the documentation on 
//    the specific interface.
//
#define STATUS_MODE_NOT_SUPPORTED     0x9B
 
//
//STATUS_RESOURCE_BUSY
//    The requested resource is busy or can not be acquired.
//
#define STATUS_RESOURCE_BUSY          0x9C

//
//STATUS_OUT_OF_RESOURCES
//    Insufficient resources to perform the request. Used by components operating
//    in non-blocking mode. A caller will be returned this error when a component
//    does not have enough resources like command buffers, PRDs etc... to entertain
//    a new request.
//
#define STATUS_OUT_OF_RESOURCES       0x9D

//
//STATUS_FAILURE
//    A unknown error encountered while performing a requested transaction.
//
#define STATUS_FAILURE                0x9E

//
//STATUS_QUEUED
//    A requested has been successfully placed in a command queue. This status
//    is returned by non-blocking components after successfully queuing a request
//    in their command queue.
//
#define STATUS_QUEUED                 0x9F

//
//STATUS_WEAR_OUT_VIOLATION
//    The partition manager returns this error code when it can not perform a 
//    requested erase operation due to flash wear out constraints.
#define STATUS_WEAR_OUT_VIOLATION     0xA0 
 
//
//STATUS_GENERAL_ERROR
//    This is a general error code used for errors that do not require a specific 
//    code.
#define STATUS_GENERAL_ERROR          0xA1
 
//
//STATUS_SMB_EMPTY
//    The smbus slave circular buffer does not contain any smbus transactions data.
#define STATUS_SMB_EMPTY              0xA2
 
//
//STATUS_SMB_NO_MORE_ENTRIES
//    The smbus slave has reached its maximal number of registered transactions. 
//    New transactions can not be added until the removal of one of the current 
//    registered ones.
#define STATUS_SMB_NO_MORE_ENTRIES     0xA3

//
//STATUS_BUS_ERROR
//    Generic error code for bus errors. Common cause could be losing arbitration
//    on a multi-mastered bus.
#define STATUS_BUS_ERROR               0xA4

//
//STATUS_IMAGE_INVALID
//    The FW Update image content is invalid.
#define STATUS_IMAGE_INVALID           0xA5

//
//STATUS_UPDATE_AUTH_FAILED
//    The FW Update image authentication failed.
#define STATUS_UPDATE_AUTH_FAILED      0xA6

//
//STATUS_UPDATE_ALLOWED
//    Used in the policy manager to indicate whether a policy can be updated at
//    the time of this request.
#define STATUS_UPDATE_ALLOWED          0xA7

//
//STATUS_UPDATE_NOT_ALLOWED
//    Used in the policy manager to indicate that a policy cannot be updated at
//    the time of this request.
#define STATUS_UPDATE_NOT_ALLOWED      0xA8

//
//STATUS_LOCKED
//    Indicates policy element has been already locked and cannot be updated.
#define STATUS_LOCKED                  0xA9

//
//STATUS_NOT_INITIALIZED
//    Indicates that SDM core is not initialized
#define STATUS_NOT_INITIALIZED     0xAA //REQUIRES COORDINATION!!!

//
//STATUS_END_OF_FILE
//    Indicates that EOF is reached while accessing a file
#define STATUS_END_OF_FILE     0xAB //REQUIRES COORDINATION!!!

//
//STATUS_NO_STORAGE_AVAILABLE
//    Indicates policy element could not be stored in the NVAR.
#define STATUS_NO_STORAGE_AVAILABLE    0xB0

//
//STATUS_LOCKING_NOT_ALLOWED
//    Indicates policy element cannot be locked.
#define STATUS_LOCKING_NOT_ALLOWED     0xB1

//
//STATUS_UNKNOWN_LAN_FUSE_CAPS
//    Indicates a unknown capability of the LAN device is being set.
#define STATUS_UNKNOWN_LAN_FUSE_CAPS   0xB2

//
//STATUS_INVALID_FEATURE_ID
//    Indicates an invalid feature ID.
#define STATUS_INVALID_FEATURE_ID      0xB3

//
//STATUS_PET_TRANSMIT_DISABLED
//    The transmission of pet packets is disabled.
#define STATUS_PET_TRANSMIT_DISABLED   0xB4

// STATUS_MAX_KERB_DOMAIN_REACHED 
// indicates that in the kerberos ACL there are users from MAX DOMAINS (4).
#define STATUS_MAX_KERB_DOMAIN_REACHED 0xB5

// STATUS_UPDATE_MISMATCH_HW_SKU 
//    Indicates that there is a mismatch between the current HW SKU and the 
//    one in the new image.
#define STATUS_UPDATE_MISMATCH_HW_SKU  0xB6

// STATUS_UPDATE_MISMATCH_FW_SKU 
//    Indicates that there is a mismatch between the current FW SKU and the 
//    one in the new image.
#define STATUS_UPDATE_MISMATCH_FW_SKU  0xB7

// STATUS_UPDATE_MISMATCH_VERSION 
//    Indicates that there is a mismatch between the current FW version and the 
//    one in the new image.
#define STATUS_UPDATE_MISMATCH_VERSION 0xB8

// STATUS_EVENT_DISABLED_CONFIG
//    This wake event is configured to be disabled (via MEBx setup or remote 
//    configuration).
#define STATUS_EVENT_DISABLED_CONFIG   0xB9

// STATUS_NOT_REGISTERED
//    Caller is trying to use an interface that requires registration, but the
//    registration process has not been completed.
#define STATUS_NOT_REGISTERED          0xBA


// STATUS_INVALID_EVENT
//    Caller is trying to use a wake event code which is invalid
#define STATUS_INVALID_EVENT 0xBB

// STATUS_INVALID_EVENT
//    Caller is trying to use an event context which is invalid
#define STATUS_BAD_CONTEXT 0xBC


//
//STATUS_NET_RESTART_NEEDED
//    The network stack need restart to continue working.
#define STATUS_NET_RESTART_NEEDED      0xBD

//
//STATUS_OUT_OF_MEMORY
//    This error can be returned for the following reasons: 
//       1.Attempt to allocate memory and no memory is available
//       2.Attempt to free a memory block on which a thread is suspended
//    NOTE: This value is the same a as STATUS_START_ERROR.
// 
#define STATUS_OUT_OF_MEMORY              0xBE


//
//STATUS_COUNTER_ROLLOVER
//    This error can be returned by MC_PROTOCOL IncrementCounter() 
//    if given monotonic counter reaches maximum value.
//    Along with error, counter will be initialized with value "1"  
// 
//    The value will also be returned by TIME_PROTOCOL GetPRTC()
//    if the PRTC rolls over
#define STATUS_COUNTER_ROLLOVER              0xBF


//
// BLOB SERVICE SPECIFIC FAILURES
//
//  STATUS_BLOB_INTEGRITY_FAILED
//  STATUS_BLOB_CONFIDENTIALITY_FAILED
//  STATUS_BLOB_AR_FAILED
#define STATUS_BLOB_INTEGRITY_FAILED        0xC0
#define STATUS_BLOB_CONFIDENTIALITY_FAILED  0xC1
#define STATUS_BLOB_AR_FAILED               0xC2



//STATUS_PROCESSING
//    Caller is asking for process result that is not already finished
#define STATUS_PROCESSING              0xC3

//STATUS_REGISTERED
//    Caller registered for something that itself or others already did
#define STATUS_REGISTERED               0xC4
//
//STATUS_EAC_NOT_PERMITTED
//    This error can be returned for the following reasons: 
//       1.Attempt to enable EAC when sign certificate is not set

// 
#define STATUS_EAC_NOT_PERMITTED            0xC5

//
//STATUS_EAC_NO_ASSOCIATION
//    This error can be returned for the following reasons: 
//       1.Attempt to receive a posture or posture hash sign certificate is not set

// 
#define STATUS_EAC_NO_ASSOCIATION            0xC6

//
//STATUS_AUDIT_FAIL
//    This error can be returned when these conditions are met: 
//       1. The action should be logged to the Audit Log
//       2. The event was defined as critical
//       3. The Audit Log is enabled
//       4. Either:
//          a. The Audit Log is currently Locked
//          b. The storage for the Audit Log is full

// 
#define STATUS_AUDIT_FAIL                    0xC7

#define STATUS_DUPLICATED                    0xC8

//
//STATUS_IPP_INTERNAL_ERROR
//    This error can bb returned in case of internal IPP function failed.
#define STATUS_IPP_INTERNAL_ERROR            0xC9

//
//STATUS_IPP_CORRUPTED_KEY
//    This error can be returned when trying to load or validate corrupted RSA key 
//    using the IPP stack.
//    Can be returned RsaEncryptDecrypt and RsaValidateKey functions.
#define STATUS_IPP_CORRUPTED_KEY             0xCA

//
//STATUS_IPP_DATA_NOT_ALIGNED
//    This error can be returned when trying to load, create or validate RSA key using the 
//    IPP stack, and the data is not aligned.
//    Can be returned RsaGenerateKey, RsaEncryptDecrypt and RsaValidateKey functions.
#define STATUS_IPP_DATA_NOT_ALIGNED          0xCB

//
//STATUS_IPP_OPERATION_ABORTED
//    This error will be returned if the IPP aborted key generation before completion.
#define STATUS_IPP_OPERATION_ABORTED         0xCC

//
//STATUS_IPP_CACHE_CONVERTION_FAILED        
//    This error can be returned if the TRAMD-Cache conversion failed.
#define STATUS_IPP_CACHE_CONVERTION_FAILED   0xCD

//
//STATUS_IPP_EXPONENT_CHANGED
//    This error will be returned if the IPP decided to use differnt exponent (E)
//    than the one the caller supplied.
#define STATUS_IPP_EXPONENT_CHANGED          0xCE

// STATUS_PERMIT_EXPIRED
//    iCLS Permit has expired
#define STATUS_PERMIT_EXPIRED                0xCF

// STATUS_PERMIT_RESET_REQUIRED
//    iCLS permit is invalid in some way (for example expired)
//    and HW fuses are overided by permit still
#define STATUS_PERMIT_RESET_REQUIRED         0xD0

// STATUS_CLOSED_BY_HOST
//    HECI will return this in the event that AddBuffer call is failing because
//    the connection is in a close pending status.
//    This will indicate that freeing the SendBufferQ and BufferQ is not permitted at
//    this time and the client should wait for a HIE_CLOSED message to cleanup HECI_CONNECTION.
#define STATUS_CLOSED_BY_HOST                0xD1


// STATUS_DISABLED_BY_POLICY
//    This error will be returned if the call is blocked by previous defined policy
#define STATUS_DISABLED_BY_POLICY            0xD2

//STATUS_INVALID_COMP_HANDLE
//    The provided component handle does not exist.
//
#define STATUS_INVALID_COMP_HANDLE           0xD3

//
//Status Codes used by state manager to indicate ME Database status 
//and general Client/Daemon registration problems in state manager 
//that prohibit the Daemon from returning more exact status. 
//
 
//
//STATUS_ALREADY_REGISTERED 
//    This error is used to indicate if a Daemon is already
//    registered with state manager.
#define STATUS_ALREADY_REGISTERED            0xD4

//
//STATUS_DATABASE_IN_USE 
//    This error is used to indicate if a ME Database is currently in
//    use when a delete is called on it.
#define STATUS_DATABASE_IN_USE               0xD5

//
//STATUS_ENTRY_IN_USE 
//    This error is used to indicate if an entry in a ME Database is
//    currently in use when a release is called on it.     
#define STATUS_ENTRY_IN_USE                  0xD6

//
//STATUS_UNABLE_TO_REGISTER 
//    This error is used to indicate that a Daemon cannot register
//    with state manager.
#define STATUS_UNABLE_TO_REGISTER            0xD7

//
// STATUS_TLB_ENTRY_NOT_FOUND
//    This error indicates that a free TLB entry or the specified TLB entry
//    could not be found.  
//
#define STATUS_TLB_ENTRY_NOT_FOUND           0xD8

//
//STATUS_UNABLE_TO_UNREGISTER 
//    This error code is used to indicate that a Daemon cannot
//    unregister with the state manager.
#define STATUS_UNABLE_TO_UNREGISTER          0xD9

//
//STATUS_TIMER_VALUE_NOT_SET
//  Error used by PRTC to indicate the alarm value
//  has not been set prior to enabling the alarm.
#define STATUS_TIMER_VALUE_NOT_SET           0xDA

//
//STATUS_ICV_CHECK_ERROR
//  HW reported an ICV check failure
#define STATUS_ICV_CHECK_ERROR               0xDB

// STATUS_SUCCESS_WITH_ERRORS
//    This error will be returned if the opration completed but had some errors (e,eg Transfer AHCI for DT)
//    It is shared between Danbury components
#define STATUS_SUCCESS_WITH_ERRORS      0xDC

// STATUS_SUCCESS_HOST_RESET_REQUIRED
//    This status will be returned by FwUpdateMgr after successfully updating FW 
//    if Danbury is enabled
#define STATUS_SUCCESS_HOST_RESET_REQUIRED   0xDD

// STATUS_FIPS_FAILURE
//    This status will be returned by when FIPS self-tests fail
#define STATUS_FIPS_FAILURE   0xDE

//STATUS_PRIVILEGE_CHECK
//  Privileged component access
#define STATUS_PRIVILEGE_CHECK               0xDF

// STATUS_INCOMPLETE
//  This error indicates that the operation is incomplete
#define STATUS_INCOMPLETE                    0xE0

// STATUS_RETRY
//  This error indicates that the operation is being retried or needs to be retried
#define STATUS_RETRY                    0xE1

#define STATUS_NOT_RUN                       0xE2

#define STATUS_NOT_IMPLEMENTED               0xE3


#define STATUS_INVALID_INDEX                 0xE4
#define STATUS_SLOT_IN_USE                   0xE5
#define STATUS_SLOT_EMPTY                    0xE6

// STATUS_OVERRIDDEN
//  This error indicates that the operation can't be performed because
// it was overridden by some other logic/request
#define STATUS_OVERRIDDEN                    0xE7

//STATUS_PERMIT_IS_DEACTIVE
//    iCLS Permit has been deactivated
#define STATUS_PERMIT_IS_DEACTIVE            0xF0

//Return this if it is a Patsburg chipset 
#define STATUS_UNKNOWN_CPUID                 0xF1

#define STATUS_CRC_ERROR                     0xF2

#if 1
//STATUS for task isolation
#define STATUS_HECI_CONNECTION_ACCEPT         0xF3
#define STATUS_HECI_CONNECTION_REJECT         0xF4
#define STATUS_RETURN_NOT_AVAILABLE           0xF5
#endif
//STATUS for PG
#define STATUS_PG_ENTRY_IN_PROGRESS           0xF6

// Intel Secret Key Unavailable
// RCR CCG0100111613
#define STATUS_BLOB_UNAVAILABLE               0xF6

#define STATUS_FPF_READ_MISMATCH    0xF7
#define STATUS_FPF_ARRAY_FULL            0xF8
#define STATUS_FPF_WRITE_FAILED         0xF9
#define STATUS_FPF_FILE_INVALID        0xFA
#define STATUS_FPF_FILE_FULL           0xFB
#define STATUS_FPF_FILE_LOCKED     0xFC
#define STATUS_FPF_NOT_AVAILABLE     0xFD
#define STATUS_FPF_BUSY          0xFE // FPF write currently in progress
#define STATUS_FPF_FATAL_ERROR 0xFF // Fatal error, fuses are no longer valid (bad PCH)
#define STATUS_FPF_FILE_EMPTY   0x100 // File has not been written to
#define STATUS_FPF_ALREADY_COMMITTED 0x101
#define STATUS_FPF_COMMIT_FAILED_EOM_NOT_SET 0x102
#define STATUS_FPF_INVALID_SB_VALUES 0x103
#define STATUS_FPF_NOT_COMMITTED 0x104
#define STATUS_FPF_NVAR_MISMATCH 0x105
#define STATUS_FPF_FILE_UNLOCKED 0x106
#define STATUS_FPF_COMMIT_NOT_ALLOWED 0x107
#define STATUS_FPF_CANARY_FAILURE 0x108
#define STATUS_FPF_SENSE_FAILED 0x109
#if 1
// STATUS for FWU kernel between  0x200  to 0x2FF
#define FW_UPDATE_STATUS UINT32

#define   NO_UPDATE  0
#define   STATUS_UPDATE_SUCCESS                                               0x0   // Zero for sucess anything else is consider failures 
#define   STATUS_UPDATE_IMAGE_INVALID                                         0x201
#define   STATUS_UPDATE_INTEGRITY_FAILURE                                     0x202
#define   STATUS_UPDATE_SKU_MISMATCH                                          0x203
#define   STATUS_UPDATE_FW_VERSION_MISMATCH                                   0x204    
#define   STATUS_UPDATE_GENERAL_FAILURE                                       0x205    
#define   STATUS_UPDATE_OUT_OF_RESOURCES                                      0x206    
#define   STATUS_UPDATE_AUDIT_POLICY_FAILURE                                  0x207
#define   STATUS_UPDATE_ERROR_CREATING_FT                                     0x208
#define   STATUS_UPDATE_SAL_NOTIFICATION_ERROR                                0x209
#define   STATUS_UPDATE_IMG_LOADING                                           0x20A
#define   STATUS_UPDATE_IMG_AUTHENTICATING                                    0x20B
#define   STATUS_UPDATE_IMG_PROCESSING                                        0x20C
#define   STATUS_UPDATE_CREATING_FT                                           0x20D
#define   STATUS_UPDATE_UPDATING_CODE                                         0x20E
#define   STATUS_UPDATE_UPDATING_NFT                                          0x20F
#define   STATUS_UPDATE_FLASH_CODE_PARTITION_INVALID                          0x210
#define   STATUS_UPDATE_FLASH_NFT_PARTITION_INVALID                           0x211
#define   STATUS_UPDATE_ILLEGAL_IMAGE_LENGTH                                  0x212
#define   STATUS_UPDATE_NOT_READY                                             0x213
                                                                              
#define   STATUS_UPDATE_HOST_RESET_REQUIRED                                   0x214
#define   STATUS_INVALID_GLUT                                                 0x215
#define   STATUS_INVALID_OEM_ID                                               0x216
                                                                              
// New for CPT add below here                                                 
#define   STATUS_UPDATE_IMAGE_BLACKLISTED                                     0x217
#define   STATUS_UPDATE_IMAGE_VERSION_HISTORY_CHECK_FAILURE                   0x218
#define   STATUS_UPDATE_DOWNGRADE_VETOED                                      0x219
#define   STATUS_UPDATE_WRITE_FILE_FAILURE                                    0x22A
#define   STATUS_UPDATE_READ_FILE_FAILURE                                     0x22B
#define   STATUS_UPDATE_DELETE_FILE_FAILURE                                   0x22C
#define   STATUS_UPDATE_PARTITION_LAYOUT_NOT_COMPATIBLE                       0x22D    // FW Update is not possible due to partition move
#define   STATUS_DOWNGRADE_NOT_ALLOWED_DATA_MISMATCHED                        0x22E
#define   STATUS_UPDATE_FW_UPDATE_IS_DISABLED                                 0x22F
#define   STATUS_UPDATE_PASSWORD_NOT_MATCHED                                  0x230
#define   STATUS_UPDATE_PASSWORD_EXCEED_MAXIMUM_RETRY                         0x231
#define   STATUS_UPDATE_INRECOVERY_MODE_RESTRICT_UPDATE_TO_ATTEMPTED_VERSION  0x232    // They have to update with the same image that they started with.


// New for Partial FW update
#define   STATUS_UPDATE_UPV_VERSION_MISMATCHED                                0x233    // UPV version mismatched update is not allow
#define   STATUS_UPDATE_INSTID_IS_NOT_EXPECTED_ID                             0x234    // Reject update, instance ID sent is not one of expected ID
#define   STATUS_UPDATE_INFO_NOT_AVAILABLE                                    0x235    // While in the middle of update IPU attrib info will bot be available
#define   STATUS_UPDATE_REJ_IPU_FULL_UPDATE_NEEDED                            0x236    // Can't do IPU update while we are in Full recovery mode.
#define   STATUS_UPDATE_IPU_NAMEID_NOT_FOUND                                  0x237    // IPU name not found when compare with UPV extension

#define   STATUS_UPDATE_RESTORE_POINT_INVALID                                 0x238
#define   STATUS_UPDATE_RESTORE_POINT_VALID_BUT_NOT_LATEST                    0x239
#define   STATUS_UPDATE_RESTORE_POINT_OPERATION_NOT_ALLOWED                   0x23A
#define   STATUS_DOWNGRADE_NOT_ALLOWED_SVN_RESTRICTION                        0x23B
#define   STATUS_DOWNGRADE_NOT_ALLOWED_VCN_RESTRICTION                        0x23C
#define   STATUS_INVALID_SVN												  0x23D
#define   STATUS_UPDATE_OUT_OF_SVN_RESOURCES								  0x23E
#define   STATUS_UPDATE_REJECT_RESTORE_POINT_REQUEST_FLASH_IN_RECOVERY   	  0x23F
#define   STATUS_UPDATE_REJECTED_BY_UPDATE_POLICY                             0x240
#define   STATUS_UPDATE_REJECTED_INCOMPATIBLE_TOOL_USAGE                      0x241
#define   STATUS_UPDATE_REJECTED_CROSSPOINT_UPDATE_NOT_ALLOWED                0x242
#define   STATUS_UPDATE_REJECTED_CROSSHOTFIX_UPDATE_NOT_ALLOWED               0x243
#define   STATUS_UPDATE_REJECTED_CURRENT_FW_NOT_ELIGIBLE_FOR_UPDATE           0x244
#define   STATUS_UPDATE_REJECTED_WRONG_UPDATE_OPERATION                       0x245
#define   STATUS_UPDATE_REJECTED_WRONG_UPDATE_IMAGE_FOUND                     0x246
#define   STATUS_UPDATE_REJECTED_IFR_UPDATE_NOT_ALLOWED                       0x247
#define   STATUS_UPDATE_FAILURE_OCCURRED_DURING_ROLLBACK                      0x248

//.........................................................................
//.........................................................................            // Reserve for FWU usage
#define   STATUS_UPDATE_LAST_STATUS_CODE                                      0x2FF 

// Hotham-specific error codes
#define STATUS_HTM_HOST_NOT_RDY                                               0x300
#define STATUS_HTM_INTERNAL_ERROR                                             0x301
#define STATUS_HTM_REQ_IN_PROGRESS                                            0x302
#define STATUS_HTM_RESP_IN_PROGRESS                                           0x303 
#define STATUS_HTM_INVALID_FRAGMENT                                           0x304
#define STATUS_HTM_RESP_NONE_ACTIVE                                           0x305
#define STATUS_HTM_NOT_CONNECTED                                              0x306  
//...
//...                                                                         Reserved for future use
#define STATUS_HTM_LAST_STATUS_CODE                                           0x31F  




// means internal FW operation errors 
//#define   STATUS_UPDATE_UNKNOWN = 0xFFFFFFFF


#endif


#endif // _ME_STATUS_H_
