//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AmiNvmeController.h
    Protocol Header file for the Nvme Controller 

**/

#ifndef _NVME_CONTROLLER_PROTOCOL_H_
#define _NVME_CONTROLLER_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <Base.h>
#include <Library/UefiLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/StorageSecurityCommand.h>

#define NVME_MANUFACTUREID_LENGTH   30
#define NVME_CONTROLLER_SIG       SIGNATURE_32('N','V','M','C')

typedef struct _AMI_NVME_CONTROLLER_PROTOCOL AMI_NVME_CONTROLLER_PROTOCOL;

#pragma pack(1)

// Figure 10 NVM Express 1.2

typedef struct {
    UINT8   Opcode;
    UINT8   FusedOperation : 2;
    UINT8   RESERVED1 : 4;
    UINT8   PSDT : 2;
    UINT16  CommandIdentifier;
} STRUC_CMD0;

typedef struct {
    STRUC_CMD0  CMD0;
    UINT32      NSID;
    UINT64      RESERVED1;
    UINT64      MPTR;
    UINT64      PRP1;
    UINT64      PRP2;
    UINT32      CDW10;
    UINT32      CDW11;
    UINT32      CDW12;
    UINT32      CDW13;
    UINT32      CDW14;
    UINT32      CDW15;
} NVME_ADMIN_COMMAND;


// Figure 83 NVM Express 1.1 Spec
typedef struct {
    UINT16      MP;                 // Bits 15:0
    UINT8       Reserved1;          // Bits 23:16
    UINT8       MPS : 1;            // Bits 24
    UINT8       NOPS : 1;           // Bits 25
    UINT8       Reserved2 : 6;      // Bits 31:26
    UINT32      ENLAT;              // Bits 63:32
    UINT32      EXLAT;              // Bits 95:64
    UINT8       RRT : 5;            // Bits 100:96
    UINT8       Reserved3 : 3;      // Bits 103:101
    UINT8       RRL : 5;            // Bits 108:104
    UINT8       Reserved4 : 3;      // Bits 111:109
    UINT8       RWT : 5;            // Bits 116:112
    UINT8       Reserved5 : 3;      // Bits 119:117
    UINT8       RWL : 5;            // Bits 124:120
    UINT8       Reserved6 : 3;      // Bits 127:125
    UINT16      IDLP;               // Bits 143:128
    UINT8       Reserved7 : 6;      // Bits 149:144
    UINT8       IPS :2;             // Bits 151:150
    UINT8       Reserved8;          // Bits 159:152
    UINT16      ACTP;               // Bits 175:160
    UINT8       APW : 3;            // Bits 178:176
    UINT8       Reserved9 : 3;      // Bits 181:179
    UINT8       APS : 2;            // Bits 183:182
    UINT8       Reserved10[9];      // Bits 255:184
} POWER_STATE_DESCRIPTOR;

// Figure 90 NVM Express 1.2 spec
typedef struct {
    UINT16                  VID;                    // Offset 01:00                
    UINT16                  SSVID;                  // Offset 03:02
    UINT8                   SerialNumber[20];       // Offset 23:04
    UINT8                   ModelNumber[40];        // Offset 63:24
    UINT8                   FirmwareRevision[8];    // Offset 71:64
    UINT8                   ArbitrationBurst;       // Offset 72
    UINT8                   IEEEOUIIdentifier[3];   // Offset 75:73
    UINT8                   CMIC;                   // Offset 76
    UINT8                   MDTS;                   // Offset 77
    UINT16                  ControllerID;           // Offset 79:78
    UINT32                  VER;                    // Offset 83:80
    UINT32                  RTD3R;                  // Offset 87:84
    UINT32                  RTD3E;                  // Offset 91:88
    UINT32                  OAES;                   // Offset 95:92
    UINT8                   Reserved1[160];         // Offset 255:96
    UINT16                  OACS;                   // Offset 257:256
    UINT8                   ACL;                    // Offset 258
    UINT8                   AERL;                   // Offset 259
    UINT8                   FRMW;                   // Offset 260
    UINT8                   LPA;                    // Offset 261
    UINT8                   ELPE;                   // Offset 262
    UINT8                   NPSS;                   // Offset 263
    UINT8                   AVSCC;                  // Offset 264
    UINT8                   APSTA;                  // Offset 265
    UINT16                  WCTEMP;                 // Offset 267:266
    UINT16                  CCTEMP;                 // Offset 269:268
    UINT16                  MTFA;                   // Offset 271:270
    UINT32                  HMPRE;                  // Offset 275:272
    UINT32                  HMMIN;                  // Offset 279:276
    UINT8                   TNVMCAP[16];            // Offset 295:280
    UINT8                   UNVMCAP[16];            // Offset 311:296
    UINT32                  RPMBS;                  // Offset 315:312
    UINT8                   Reserved2[196];         // Offset 511:316
    UINT8                   SQES;                   // Offset 512
    UINT8                   CQES;                   // Offset 513
    UINT16                  Reserved3;              // Offset 515:514
    UINT32                  NN;                     // Offset 519:516
    UINT16                  ONCS;                   // Offset 521:520
    UINT16                  FUSES;                  // Offset 523:522
    UINT8                   FNA;                    // Offset 524
    UINT8                   VWC;                    // Offset 525
    UINT16                  AWUN;                   // Offset 527:526
    UINT16                  AWUPF;                  // Offset 529:528
    UINT8                   NVSCC;                  // Offset 530
    UINT8                   Reserved4;              // Offset 531
    UINT16                  ACWU;                   // Offset 533:532
    UINT16                  Reserved5;              // Offset 535:536
    UINT32                  SGLS;                   // Offset 539:536
    UINT8                   Reserved6[164];         // Offset 703:540
    UINT8                   Reserved7[1344];        // Offset 2047:704
    POWER_STATE_DESCRIPTOR  psdd[32];               // Offset 3071:2048
    UINT8                   VS[1024];               // Offset 4095:3072
} IDENTIFY_CONTROLLER_DATA;

#pragma pack()

// Structure is defined in BIG ENIDIAN format
typedef struct {
    UINT8    Reserved;
    UINT8    SecRecvBufferSize;  // Nvme Security Receive Data Size
    UINT16   SecEraseTime;       // Security erase Time MSB value
    UINT16   EnhancedSecEraseT;  // Enhanced Security erase Time MSB value
    UINT16   MasterPwdIdentifier;
    UINT16   NvmeSecStatus;
    UINT16   Reserved1[3];
} NVME_SECURITY_RECEIVE_BUFFER;

typedef struct {
    NVME_ADMIN_COMMAND          NvmCmd;
    BOOLEAN                     AdminOrNVMCmdSet;           // TRUE for Admin
    UINT16                      SQIdentifier;               // Queue 0 for Admin cmds and 1 for NVM cmds
    UINT32                      CmdTimeOut;                 // In msec.
    UINT8                       RetryCount;
} NVME_COMMAND_WRAPPER;

typedef EFI_STATUS
(EFIAPI *AMI_NVME_CONTROLLER_INIT)(
    IN  AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController
);

// Saved in reserved memory for each NVMe controller. Both DXE and SMM driver will share the data.
typedef
struct {
    UINT16                      CommandIdentifierAdmin;
    UINT16                      CommandIdentifierQueue1;

    UINT16                      AdminCompletionQueueHeadPtr;
    UINT16                      AdminCompletionQueueTailPtr;
    UINT16                      AdminSubmissionQueueHeadPtr;      // Updated from Completion queue entry
    UINT16                      AdminSubmissionQueueTailPtr;      // Updated when new cmds are issued
    UINT8                       AdminPhaseTag : 1;
    UINT8                       RESERVED : 7;

    UINT16                      Queue1CompletionQueueHeadPtr;
    UINT16                      Queue1CompletionQueueTailPtr;
    UINT16                      Queue1SubmissionQueueHeadPtr;     // Updated from Completion queue entry
    UINT16                      Queue1SubmissionQueueTailPtr;     // Updated when new cmds are issued
    UINT8                       Queue1PhaseTag : 1;
    UINT8                       RESERVED1 : 7;
} NVME_DXE_SMM_DATA_AREA;

typedef
struct _AMI_NVME_CONTROLLER_PROTOCOL{
    UINT32                      Signature;
    UINT64                      NvmeBarOffset;
    EFI_PCI_IO_PROTOCOL         *PciIO;
    EFI_HANDLE                  ControllerHandle;
    UINT64                      RawControllerCapabilities;
    UINT32                      MaxQueueEntrySupported;
    BOOLEAN                     ContiguousQueueRequired;
    UINT8                       ArbitrationMechanismSupport;
    UINT8                       TimeOut;
    UINT8                       DoorBellStride;
    BOOLEAN                     NVMResetSupport;
    UINT8                       CmdSetsSupported;
    UINT32                      MemoryPageSizeMin;
    UINT32                      MemoryPageSizeMax;
    UINT32                      MemoryPageSize;                   // In Bytes
    NVME_COMMAND_WRAPPER        *NvmeCmdWrapper;                  // Will be used by internal routines
    
//----------------------------Admin Queue--------------------------------------------------------------
    UINT64                      AdminSubmissionUnAligned;         // Points to the start of the buffer
    UINT64                      AdminSubmissionQueue;             // Points to the start of the buffer Aligned
    UINT64                      AdminSubmissionQueueMappedAddr;   // Mapped address for the controller to use
    VOID                        *AdminSubmissionQueueUnMap;       // 
    UINTN                       AdminSubmissionUnAlignedSize;     // in Pages
    UINT64                      AdminCompletionUnAligned;
    UINT64                      AdminCompletionQueue;
    UINT64                      AdminCompletionQueueMappedAddr;
    VOID                        *AdminCompletionQueueUnMap;
    UINTN                       AdminCompletionUnAlignedSize;
    UINT16                      AdminSubmissionQueueSize;         // Max is 4KB
    UINT16                      AdminCompletionQueueSize;         // Max is 4KB

//----------------------------Queue1 for Nvme Cmds------------------------------------------------------
    UINT16                      NVMQueueNumber;
    UINT64                      Queue1SubmissionUnAligned;        // Points to the start of the buffer
    UINTN                       Queue1SubmissionUnAlignedSize;    // Original size
    UINT64                      Queue1SubmissionQueue;            // Aligned address
    UINT64                      Queue1SubmissionQueueMappedAddr;  // Address for controller to use
    VOID                        *Queue1SubmissionQueueUnMap;      
    UINT64                      Queue1CompletionUnAligned;
    UINTN                       Queue1CompletionUnAlignedSize;
    UINT64                      Queue1CompletionQueue;
    UINT64                      Queue1CompletionQueueMappedAddr;
    VOID                        *Queue1CompletionQueueUnMap;
    UINT32                      Queue1SubmissionQueueSize;
    UINT32                      Queue1CompletionQueueSize;
    BOOLEAN                     ReInitializingQueue1;             // Flag used to prevent initializing Queue1 in an infinite loop
  
  //-----------------------------------------------------------------------------------------------------  
    AMI_NVME_CONTROLLER_INIT    NvmeInitController;              // Function to initialize Nvme in S3 path
    BOOLEAN                     NvmeInitControllerFlag;          // Flag denotes that Nvme Controller is initialized
                                                                 // in S3 resume path

    VOID                        *LegacyNvmeBuffer;
    VOID                        *SecurityInterface;               // Pointer to AMI_HDD_SECURITY_INTERFACE structure
    NVME_SECURITY_RECEIVE_BUFFER SecReceiveBuffer;
    VOID                        *EfiNvmExpressPassThru;
    VOID                        *SmartInterface;

    EFI_STORAGE_SECURITY_COMMAND_PROTOCOL StorageSecurityInterface;  // Pointer to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL structure
    VOID                                  *OpalConfig;               // Pointer to OPAL_DEVICE

    NVME_DXE_SMM_DATA_AREA      *NvmeDxeSmmDataArea;
    BOOLEAN                     NvmeInSmm;
    IDENTIFY_CONTROLLER_DATA    *IdentifyControllerData;
    EFI_UNICODE_STRING_TABLE    *UDeviceName;
    UINT32                      *ActiveNameSpaceIDs;  
    EFI_GUID                    FirmwareCapsuleUpdateGuid;
    LIST_ENTRY                  ActiveNameSpaceList;              // Linked list of Active Name space found in this controller
} ;


extern EFI_GUID gAmiNvmeControllerProtocolGuid;
extern EFI_GUID gAmiSmmNvmeControllerProtocolGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
