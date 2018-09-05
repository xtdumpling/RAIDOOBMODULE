//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
// Revision History
// ----------------
// $Log: $
// 
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:    CSP_MEUD.h
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************
#ifndef _EFI_CSP_MEUD_H_
#define _EFI_CSP_MEUD_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <Token.h>

#define FLASH_VALID_SIGNATURE 0x0FF0A55A

#ifndef _EFI_MMIO_ACCESS_H_
#define _EFI_MMIO_ACCESS_H_

#define MmioAddress(BaseAddr, Register) \
 ( (UINTN)BaseAddr + (UINTN)(Register) )

// 32-bit
#define Mmio32Ptr(BaseAddr, Register) \
 ( (volatile UINT32 *)MmioAddress(BaseAddr, Register) )

#define Mmio32(BaseAddr, Register) \
 *Mmio32Ptr(BaseAddr, Register)

// 16-bit
#define Mmio16Ptr(BaseAddr, Register) \
 ( (volatile UINT16 *)MmioAddress(BaseAddr, Register) )

#define Mmio16(BaseAddr, Register) \
 *Mmio16Ptr(BaseAddr, Register)

// 8-bit
#define Mmio8Ptr(BaseAddr, Register) \
 ( (volatile UINT8 *)MmioAddress(BaseAddr, Register) )

#define Mmio8(BaseAddr, Register) \
 *Mmio8Ptr(BaseAddr, Register)

#define MmioRW32(Addr, set, reset) \
    (Mmio32(Addr, 0) = ((Mmio32(Addr, 0) & (UINT32)~(reset)) | (UINT32)set))
#endif

#define HeciPciRead32(Register) PciRead32 (PCI_LIB_ADDRESS (0, 0x16, 0, Register))

#define SPS_CLIENTID_BIOS 0x00
#define SPS_CLIENTID_ME_MKHI 0x07

#define I40E_SR_SIZE_IN_WORDS			            (0x10000 >> 0x01) //64K
#define I40E_SR_VPD_MODULE_MAX_SIZE_IN_WORDS        (0x400 >> 0x01) //1K
#define I40E_SR_PCIE_ALT_MODULE_MAX_SIZE_IN_WORDS   (0x400 >> 0x01) //1K
#define I40E_SR_PCIE_ALT_MODULE_WORD_OFFSET         (I40E_SR_SIZE_IN_WORDS - I40E_SR_PCIE_ALT_MODULE_MAX_SIZE_IN_WORDS)
#define I40E_SR_SW_CHECKSUM_WORD_OFFSET             0x3F
#define I40E_SR_SW_CHECKSUM_BASE                    0xBABA

VOID InitializeSpiBase( VOID );

#pragma pack(1)
typedef struct
{
    UINT16  Header;
    UINT16  MacAddress[3];
}PF_MAC_ADDRESS_STRUCTURE;
#pragma pack()

typedef enum
{
    UpdateNonMeRegion,
    UpdateFtprToOpr,
    UpdateMfsb,
    UpdateFtpr,
    UpdateMfs,
    UpdateFlog,
    UpdateOpr
}CSP_MEUD_UPDATE_STEP;

typedef enum
{
    Save10GbeMac,
    Restore10GbeMac
}BEHAVIOR_OF_10GBE_MAC;

typedef
VOID
(ACCESS_10GBE)(
    IN  VOID    *Source,
    IN  VOID    *Destination,
    IN  UINTN   Length,
    IN  BOOLEAN Restore );

EFI_STATUS
Process10GbeMac(
    IN  VOID    *Region10Gbe,
    IN  UINT8   Behavior
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
