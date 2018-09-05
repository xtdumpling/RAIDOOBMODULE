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

/** @file NvmeController.h
    Nvme Controller related definition

**/


#ifndef _NVME_CONTROLLER_H_
#define _NVME_CONTROLLER_H_

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

#define NVME_BAR0_INDEX 0x00

// Refer to NVM Spec second 3.1 NVMe 1.2 which describes the register map for the controller
#define Offset_Cap      0x0
#define Offset_Version  0x8
#define Offset_Intms    0xC
#define Offset_Intmc    0x10
#define Offset_CC       0x14
#define Offset_CSTS     0x1C
#define CSTS_RDY        0x1
#define CSTS_CFS        0x2
#define Offset_Nssrs    0x20
#define Offset_Aqa      0x24
#define Offset_Asq      0x28
#define Offset_Acq      0x30
#define Offset_CMBLOC   0x38
#define Offset_CMBSZ    0x3C
#define Offset_Admin_Submission_Queue_Tail_DoorBell 0x1000

// Admin Cmd Set Figure 38 NVM Express 1.2
#define DELETE_IO_SUBMISSION_QUEUE  0x00
#define CREATE_IO_SUBMISSION_QUEUE  0x01
#define GET_LOG_PAGE                0x02
#define DELETE_IO_COMPLETION_QUEUE  0x04
#define CREATE_IO_COMPLETION_QUEUE  0x05
#define IDENTIFY                    0x06
#define ABORT                       0x08
#define SET_FEATURES                0x09
#define GET_FEATURES                0x0A
#define ASYNC_EVENT_REQUEST         0x0C
#define NAMESPACE_MANAGEMENT        0x0D
#define FIRMWARE_ACTIVATE           0x10
#define FIRMWARE_IMAGE_DOWNLOAD     0x11
#define DEVICE_SELF_TEST            0x14
#define NAMESPACE_ATTACHMENT        0x15
#define FORMAT_NVM                  0x80
#define SECURITY_SEND               0x81
#define SECURITY_RECEIVE            0x82

//Figure 149: Opcodes for NVM Commands NVM Express 1.2
#define NVME_FLUSH                  0x00
#define NVME_WRITE                  0x01
#define NVME_READ                   0x02
#define NVME_WRITE_UNCORRECTABLE    0x04
#define NVME_COMPARE                0x05
#define NVME_WRITE_ZEROES           0x08
#define NVME_DATASET_MANAGEMENT     0x09
#define NVME_RESERVATION_REGISTER   0x0D
#define NVME_RESERVATION_REPORT     0x0E
#define NVME_RESERVATION_ACQUIRE    0x11
#define NVME_RESERVATION_RELEASE    0x15

#define NVME_SECUIRTY_SECP          0xEF
#define NVME_SECURITY_LOCK_MASK     0x400
#define NVME_SECURITY_FREEZE_MASK   0x800

#define NVME_SECURITY_RECEIVE_BUFFER_SIZE 0x10
#define NVME_SECURITY_SEND_BUFFER_SIZE    0x24

typedef struct {
    UINT64      ControllerCapabilities;
    UINT32      Version;
    UINT32      InterruptMaskSet;
    UINT32      InterruptMaskClear;
    UINT32      ControllerConfiguration;
    UINT32      Reserved1;
    UINT32      ControllerStatus;
    UINT32      NVMSubSystemReset;
    UINT32      AdminQueueAttributes;
    UINT64      AdminSubmissionQueueBaseAddress;
    UINT64      AdminCompletionQueueBaseAddress;
    // Variable offsets follow
} NVME_CONTROLLER_REGISTER;

//Figure 25  NVM Express 1.1 Spec
typedef struct {
    UINT32      DW0;
    UINT32      DW1;
    UINT16      SQHeadPointer;
    UINT16      SQIdentifier;
    UINT16      CommandIdentifier;
    UINT16      PhaseTag :  1;
    UINT16      StatusCode :  8;
    UINT16      StatusCodeType : 3;
    UINT16      Reservered1 : 2;
    UINT16      More : 1;
    UINT16      DoNotRetry  : 1;
} COMPLETION_QUEUE_ENTRY;



// Figure 93 NVM Express 1.2 Spec
typedef struct {
    
    UINT16      MS;                     // Bits 15:0
    UINT8       LBADS;                  // Bits 23:16
    UINT8       RP : 2;                 // Bits 25:24
    UINT8       Reserved1 :6;           // Bits 31:26
    
} LBA_FORMAT_DATA;


// Figure 92 NVM Express 1.2 Spec
typedef struct {
    
    UINT64             NSIZE;                  // Offset 7:0
    UINT64             NCAP;                   // Offset 15:8
    UINT64             NUSE;                   // Offset 23:16
    UINT8              NSFEAT;                 // Offset 24
    UINT8              NLBAF;                  // Offset 25
    UINT8              FLBAS;                  // Offset 26
    UINT8              MC;                     // Offset 27
    UINT8              DPC;                    // Offset 28
    UINT8              DPS;                    // Offset 29
    UINT8              NMIC;                   // Offset 30
    UINT8              RESCAP;                 // Offset 31
    UINT8              FPI;                    // Offset 32
    UINT8              Reserved1;              // Offset 33
    UINT16             NAWUN;                  // Offset 35:34
    UINT16             NAWUPF;                 // Offset 37:36
    UINT16             NACWU;                  // Offset 39:38
    UINT16             NABSN;                  // Offset 41:40
    UINT16             NABO;                   // Offset 43:42
    UINT16             NABSPF;                 // Offset 45:44
    UINT8              Reserved2[2];           // Offset 46:47
    UINT8              NVMCAP[16];             // Offset 63:48
    UINT8              Reserved3[40];          // Offset 103:64
    UINT8              NGUID[16];              // Offset 119:104
    UINT64             EUI64;                  // Offset 127:120
    LBA_FORMAT_DATA    LBAF[16];               // Offset 191:128
    UINT8              Reserved4[192];         // Offset 383:192
    UINT8              VS[3712];               // Offset 4095:384
    
} IDENTIFY_NAMESPACE_DATA;

#define QUEUE_DOORBELL_OFFSET(QUEUE_NUM, TAIL_HEAD, DoorBellStride) \
        (0x1000 + (((QUEUE_NUM * 2)  + TAIL_HEAD) *  (4 << DoorBellStride)))

//MMIO Access

#define NvmeMmAddress( BaseAddr, Register ) \
        ((UINT64)(BaseAddr) + \
        (UINTN)(Register) \
         )
#define NvmeMm32Ptr( BaseAddr, Register ) \
        ((volatile UINT32 *)NvmeMmAddress (BaseAddr, Register ))

#define NvmeMm16Ptr( BaseAddr, Register ) \
        ((volatile UINT16 *)NvmeMmAddress (BaseAddr, Register ))

#define NvmeMm8Ptr( BaseAddr, Register ) \
        ((volatile UINT8 *)NvmeMmAddress (BaseAddr, Register ))

//Controller Generic Registers

#define     CONTROLLER_REG32( pNvmeController, Register ) \
            (ReadDataDword ((pNvmeController), (Register)))

#define     CONTROLLER_WRITE_REG32( pNvmeController, Register, Data ) \
            (WriteDataDword( pNvmeController, Register, Data ))

#define     CONTROLLER_REG16( pNvmeController, Register ) \
            (ReadDataWord( pNvmeController, Register ))

#define     CONTROLLER_WRITE_REG16( pNvmeController, Register, Data ) \
            (WriteDataWord( pNvmeController, Register, Data ))

#define     CONTROLLER_REG8( pNvmeController, Register ) \
            (ReadDataByte ((pNvmeController), (Register)))

#define     CONTROLLER_WRITE_REG8( pNvmeController, Register, Data ) \
            (WriteDataByte( pNvmeController, Register, Data ))

#define     CONTROLLER_REG8_OR( pNvmeController, Register, OrData) \
            CONTROLLER_WRITE_REG8(pNvmeController, Register, ((CONTROLLER_REG8 ((pNvmeController), (Register))) | ((UINT8) (OrData))))

#define     CONTROLLER_REG16_OR( pNvmeController, Register, OrData) \
            CONTROLLER_WRITE_REG16(pNvmeController, Register, ((CONTROLLER_REG16 ((pNvmeController), (Register))) | ((UINT16) (OrData))))

#define     CONTROLLER_REG32_OR( pNvmeController, Register, OrData) \
            CONTROLLER_WRITE_REG32(pNvmeController, Register, ((CONTROLLER_REG32 ((pNvmeController), (Register))) | ((UINT32) (OrData))))

#define     CONTROLLER_REG8_AND( pNvmeController, Register, AndData) \
            CONTROLLER_WRITE_REG8(pNvmeController, Register, ((CONTROLLER_REG8 ((pNvmeController), (Register))) & ((UINT8) (AndData))))

#define     CONTROLLER_REG16_AND( pNvmeController, Register, AndData) \
            CONTROLLER_WRITE_REG16(pNvmeController, Register, ((CONTROLLER_REG16 ((pNvmeController), (Register))) & ((UINT16) (AndData))))
 
#define     CONTROLLER_REG32_AND( pNvmeController, Register, AndData) \
            CONTROLLER_WRITE_REG32(pNvmeController, Register, ((CONTROLLER_REG32 ((pNvmeController), (Register))) & ((UINT32) (AndData))))

#define     CONTROLLER_REG8_AND_OR( pNvmeController, Register, AndData, OrData) \
            CONTROLLER_WRITE_REG8(pNvmeController, Register, ((CONTROLLER_REG8 ((pNvmeController), (Register))) & ((UINT8) (AndData)) | ((UINT8) (OrData))))

#define     CONTROLLER_REG16_AND_OR( pNvmeController, Register, AndData, OrData) \
            CONTROLLER_WRITE_REG16(pNvmeController, Register, ((CONTROLLER_REG16 ((pNvmeController), (Register))) & ((UINT16) (AndData)) | ((UINT16) (OrData))))

#define     CONTROLLER_REG32_AND_OR( pNvmeController, Register,AndData,  OrData) \
            CONTROLLER_WRITE_REG32(pNvmeController, Register, ((CONTROLLER_REG32 ((pNvmeController), (Register))) & ((UINT32) (AndData)) | ((UINT32) (OrData))))

#pragma pack()

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
