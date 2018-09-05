//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/TcgLegacy/TcgLegacy.h 1     10/08/13 12:04p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:04p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/TcgLegacy/TcgLegacy.h $
//**********************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  TcgLegacy.h
//
// Description:
// Header file for TCG Legacy subcomponent
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Efi.h>

#pragma pack (push, 1)

EFI_FORWARD_DECLARATION( EFI_LEGACY_TCG_PROTOCOL );

typedef struct _TCG_LEGX16_HEADER TCG_LEGX16_HEADER;
struct _TCG_LEGX16_HEADER
{
    UINT16 wHeaderSize;
    UINT16 wLeg16Ver;
    UINT32 wTpm32hdr;
    UINT16 wTpm32entryPtr;
    UINT16 int1a_dispatch;
    UINT16 int19Ev;
    UINT16 intLoaderEv;
    UINT16 BevBcvEv;
    UINT16 InitCode;
    UINT16 Reserved;
    UINT16 regMOR; //8-15: byte offset; 0-7: bit position
};

typedef struct _TpmBinaryStruct TpmBinaryStruct;
struct _TpmBinaryStruct
{
    UINT16 wLegX16Seg;
    UINT16 int1a_dispatch;
    UINT16 int19Ev;
    UINT16 intLoaderEv;
    UINT16 BevBcvEv;
};

typedef struct _TPMBIOSDriverHeader TPMBIOSDriverHeader;
struct _TPMBIOSDriverHeader
{
    UINT16 TPMSignature;       // 55AAh
    UINT32 TPMCodeOffset;       // Offset to Entry Point
    UINT16 TPMCodeSize;
    UINT32 TPMBaseAddr;       // Base Adress of CMD register
    UINT32 TPMBaseAddrOpt;
    UINT8  TPMIRQ;
    UINT8  TPMDMA;
    UINT8  TPMCheckSum;
    UINT8  TPMReserve1;       // 00h
    UINT32 TPMPCIPFA;
    UINT32 TPMUSB;
    UINT32 TPMReserve2;       // 00000000h
    UINT16 TPMOEM0;
    UINT16 TPMOEM1;
};

typedef struct _MEM_OBJECT MEM_OBJECT;
struct _MEM_OBJECT
{
    UINT32 memptr; //void* memptr;
    UINT32 dwSize;
    UINT32 reserved1;
};

typedef struct _TPM32HEADER TPM32HEADER;
struct _TPM32HEADER
{
    UINT32 Signature;         //Signature of TPM32 module
    UINT32 entryPoint;        //offset to the dispatch code relative to the
    //start of the TPM32 module
    UINT32     MP_HDR;            //TPMBIOSDriverHeader* MP_HDR;//offset to the start of the MP driver in MP_xSEG
    MEM_OBJECT scratch;

    //errType is one of { TCPA_ERRGRP_TPM,TCPA_ERRGRP_TCPA,or TCPA_ERRGRP_OTHER}
    //see tcpatpm.inc
    UINT32 errType;
    UINT8  TPMAcDeact;
    //meaning of errCode depends on errType: TPM err code, TCPA err code
    //or own error code
    UINT32 errCode;

    UINT32 ptrOnTPMFailue;

    MEM_OBJECT log;
    UINT32     freelog;
    UINT32     last;
    UINT32     nextevent;
    UINT32     lastEventShadow; // UINT8**
    UINT8      Tpm20Enabled;
    UINT64     efi_log_event_ptr;
};

typedef struct _SHA1_TCG_PCR_EVENT
{
    UINT32                            PCRIndex;  // PCRIndex event extended to
    UINT32                            EventType; // TCG EFI event type
    UINT8                             Digest[20];    // Value extended into PCRIndex
    UINT32                            EventSize; // Size of the event data
    UINT8                             Event[1];  // The event data
} SHA1_PCR_EVENT;

#pragma pack (pop)
//
// TcgLegacy imports
//
EFI_STATUS GetRawImage (
    IN EFI_GUID  *NameGuid,
    IN OUT VOID  **Buffer,
    IN OUT UINTN *Size );

EFI_STATUS GetPCIOPROMImage (
    IN EFI_HANDLE PciHandle,
    OUT VOID      *RomImage,
    OUT UINTN     *Romsize,
    OUT UINTN     *Flags );
#ifdef EFI_DEBUG
#define EFI_VERIFY( x ) ASSERT( !EFI_ERROR( x ))
#else
#define EFI_VERIFY( x )            x
#endif
#define TPM32_SCRATCHMEM_SIZE    0x400
#define TPM32_LOG_SIZE            0x1000
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
