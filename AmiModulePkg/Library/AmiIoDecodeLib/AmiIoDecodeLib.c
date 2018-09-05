//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  AmiIoDecodeLib.C
//
// Description: Library Class for AMI SIO Driver.
//
//
//<AMI_FHDR_END>
//*************************************************************************
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------

#include <Efi.h>
#include <Protocol/PciIo.h>
#include <Protocol/AmiSio.h>
#include <Library/PciCf8Lib.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------



//This definitions also available in SB.h
#define ICH_REG_LPC_IBDF            0x6C
#define ICH_REG_LPC_IODEC           0x80
#define ICH_REG_LPC_EN              0x82
#define ICH_REG_LPC_GEN1_DEC        0x84
#define ICH_REG_LPC_GEN2_DEC        0x88
#define ICH_REG_LPC_GEN3_DEC        0x8C
#define ICH_REG_LPC_GEN4_DEC        0x90
#define SIO_SB_BUS_NUMBER    	   	0x00
#define SIO_SB_DEV_NUMBER   	   	0x1F
#define SIO_SB_FUNC_NUMBER  	   	0x00

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SbLib_SetLpcGenericDecoding
//
// Description: This function set LPC Bridge Generic Decoding
//
// Input:       *LpcPciIo - Pointer to LPC PCI IO Protocol
//              Base      - I/O base address
//              Length    - I/O Length
//              Enabled   - Enable/Disable the generic decode range register
//
// Output:      EFI_STATUS
//                  EFI_SUCCESS - Set successfully.
//                  EFI_UNSUPPORTED - This function is not implemented or the
//                                    Length more than the maximum supported
//                                    size of generic range decoding.
//                  EFI_INVALID_PARAMETER - the Input parameter is invalid.
//                  EFI_OUT_OF_RESOURCES - There is not available Generic
//                                         Decoding Register.
//                  EFI_NOT_FOUND - the generic decode range will be disabled
//                                  is not found.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS EFIAPI AmiSioLibSetLpcGenericDecoding (
    IN VOID      				*LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable )
{
    // Porting Required
    UINT32                  IoGenDecode32, NumOfBdf;
    UINT16                  IoGenDecIndex;
    UINT16                  Buffer16;
    UINT8                   Bsf8 = 0;
    UINT8                   Bsr8 = 0;
    EFI_STATUS				Status=EFI_SUCCESS;
    EFI_PCI_IO_PROTOCOL		*LpcPciIoProtocol=(EFI_PCI_IO_PROTOCOL*)LpcPciIo;

    if (Length > 0x100) return EFI_UNSUPPORTED;

    if (Length == 0) return EFI_INVALID_PARAMETER;

    if (Length < 4) Length = 4;

    if (LpcPciIo==NULL) NumOfBdf = (UINT32)(PCI_CF8_LIB_ADDRESS(SIO_SB_BUS_NUMBER,SIO_SB_DEV_NUMBER, SIO_SB_FUNC_NUMBER, 0));

    // Read I/O Generic Decodes Register.
    for (IoGenDecIndex = 0; IoGenDecIndex < 4; IoGenDecIndex++) {
        //IoGenDecode32 = READ_PCI32_SB(ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4);
        if (LpcPciIo==NULL){
            IoGenDecode32=PciCf8Read32(NumOfBdf+(ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4));
        } else {
            Status=LpcPciIoProtocol->Pci.Read(LpcPciIoProtocol, EfiPciIoWidthUint32,(ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4),1,&IoGenDecode32);
            if (EFI_ERROR(Status)) return Status;
        }

        if (Enable) {
            if ((IoGenDecode32 & 1) == 0) break;
            else if ((IoGenDecode32 & 0xfffc) == Base) break;
        } else {
            if (((IoGenDecode32 & 0xfffc) == Base) && (IoGenDecode32 & 1)) {
                IoGenDecode32 = 0; // Disable & clear the base/mask fields
                break;
            }
        }
    }

    if (Enable) {
        if (IoGenDecIndex == 4) return EFI_OUT_OF_RESOURCES;

        Buffer16 = Length;
        while ((Buffer16 % 2) == 0) {
            Buffer16 /= 2;
            Bsf8++;
        }

        while (Length) {
            Length >>= 1;
            Bsr8++;
        }

        if (Bsf8 == (Bsr8 - 1)) Bsr8--;

        Length = (1 << Bsr8) - 1 ;

        Base &= (~Length);

        IoGenDecode32 = Base | (UINT32)((Length >> 2) << 18) | 1;

    } else {
        if (IoGenDecIndex == 4) return EFI_NOT_FOUND;
    }

//    MmPci32(LPC_BUS, LPC_DEVICE, LPC_FUNC, (ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4)) = IoGenDecode32; // 0x84
    if (LpcPciIo==NULL){
        PciCf8Write32(NumOfBdf+(ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4),IoGenDecode32);
	} else {
        Status=LpcPciIoProtocol->Pci.Write(LpcPciIoProtocol, EfiPciIoWidthUint32,(ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4),1,&IoGenDecode32);
        if (EFI_ERROR(Status)) return Status;
    }

    // Porting End

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SbLib_SetLpcDeviceDecoding
//
// Description: This function sets LPC Bridge Device Decoding
//
// Input:       *LpcPciIo - Pointer to LPC PCI IO Protocol(NULL in PEI)
//              Base      - I/O base address, if Base is 0 means disabled the
//                          decode of the device
//              DevUid    - The device Unique ID
//              Type      - Device Type, please refer to AMISIO.h
//
// Output:      EFI_STATUS
//                  EFI_SUCCESS - Set successfully.
//                  EFI_UNSUPPORTED - There is not proper Device Decoding
//                                    register for the device UID.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI AmiSioLibSetLpcDeviceDecoding (
    IN VOID      				*LpcPciIo,
    IN UINT16                   Base,
    IN UINT8                    DevUid,
    IN SIO_DEV_TYPE             Type )
{
    EFI_STATUS              Status = EFI_UNSUPPORTED;
    UINT32                  NumOfBdf;
    // Porting Required
    UINT16                  ComRange[] = { 0x3f8, 0x2f8, 0x220, 0x228,\
                                           0x238, 0x2e8, 0x338, 0x3e8, 0x2f0, 0x2e0, 0};
    UINT16                  LptRange[] = { 0x378, 0x278, 0x3bc, 0};
    UINT16                  FpcRange[] = { 0x3f0, 0x370, 0};
    UINT16                  IoRangeMask16 = 0xffff;
    UINT16                  IoRangeSet16 = 0;
    UINT16                  IoEnMask16 = 0xffff;
    UINT16                  IoEnSet16 = 0;
    UINT8                   i;
    UINT16					tmp16;
    EFI_PCI_IO_PROTOCOL		*LpcPciIoProtocol=(EFI_PCI_IO_PROTOCOL*)LpcPciIo;

    switch (Type) {
        // FDC Address Range
        case (dsFDC) :
            if (Base == 0) IoEnMask16 &= ~BIT03;
            else {
                for (i = 0; (FpcRange[i] != 0) && (FpcRange[i] != Base); i++);
                if (FpcRange[i]) {
                    IoEnSet16 |= BIT03;
                    IoRangeMask16 &= ~BIT12;
                    IoRangeSet16 |= (i << 12);
                }
                else return EFI_UNSUPPORTED;
            }
            break;

        // LPT Address Range
        case (dsLPT) :
            if (Base == 0) IoEnMask16 &= ~BIT02;
            else {
                for (i = 0; (LptRange[i] != 0) && (LptRange[i] != Base); i++);
                if (LptRange[i]) {
                    IoEnSet16 |= BIT02;
                    IoRangeMask16 &= ~(BIT09 | BIT08);
                    IoRangeSet16 |= (i << 8);
                } else return EFI_UNSUPPORTED;
            }
            break;

        // ComA Address Range
        case (dsUART) :
            if (Base == 0) {
                if (DevUid) IoEnMask16 &= ~BIT01;
                else IoEnMask16 &= ~BIT00;
            } else {
                for (i = 0; (ComRange[i] != 0) && (ComRange[i] != Base); i++);
                if (ComRange[i]) {
                    if (DevUid) {
                        IoEnSet16 |= BIT01;
                        IoRangeMask16 &= ~(BIT06 | BIT05 | BIT04);
                        IoRangeSet16 |= (i << 4);
                    } else {
                        IoEnSet16 |= BIT00;
                        IoRangeMask16 &= ~(BIT02 | BIT01 | BIT00);
                        IoRangeSet16 |= i;
                    }
                } else return EFI_UNSUPPORTED;
            }
            break;

        // KBC Address Enable
        case (dsPS2K) :
        case (dsPS2M) :
        case (dsPS2CK) :
        case (dsPS2CM) :
            if (Base == 0) IoEnMask16 &= ~BIT10;
            else IoEnSet16 |= BIT10;
            break;

        // Game Port Address Enable
        case (dsGAME) :
            if (Base == 0) IoEnMask16 &= ~(BIT09 | BIT08);
            else {
                if (Base == 0x200) {
                    IoEnSet16 |= BIT08;
                } else {
                    if (Base == 0x208) IoEnSet16 |= BIT09;
                    else return EFI_UNSUPPORTED;
                }
            }
            break;

        // LPC CFG Address Enable
        case (0xff) :
            if (Base == 0x2e) IoEnSet16 |= BIT12;
            else {
                if (Base == 0x4e) IoEnSet16 |= BIT13;
                else {
                    if (Base == 0x62) IoEnSet16 |= BIT11;
                    else {
                    if (Base) AmiSioLibSetLpcGenericDecoding( LpcPciIo, \
                                                           Base , \
                                                           4, \
                                                           TRUE );
                        else return EFI_UNSUPPORTED;
                    }
                }
            }
            break;

        default :
            return EFI_UNSUPPORTED;
    }

    if (LpcPciIo==NULL) NumOfBdf = (UINT32)(PCI_CF8_LIB_ADDRESS(SIO_SB_BUS_NUMBER,SIO_SB_DEV_NUMBER, SIO_SB_FUNC_NUMBER, 0));

    //    RW_PCI16_SB(ICH_REG_LPC_IODEC, IoRangeSet16, ~IoRangeMask16);
    if (LpcPciIo==NULL){
        tmp16=PciCf8Read16((UINTN)NumOfBdf+ICH_REG_LPC_IODEC);
	} else {
        Status=LpcPciIoProtocol->Pci.Read(LpcPciIoProtocol, EfiPciIoWidthUint16,ICH_REG_LPC_IODEC,1,&tmp16);
        if (EFI_ERROR(Status)) return Status;
    }
	
    tmp16=tmp16&IoRangeMask16;
    tmp16=tmp16|IoRangeSet16;
    
	if (LpcPciIo==NULL){    
        PciCf8Write16((UINTN)NumOfBdf+ICH_REG_LPC_IODEC,tmp16);
    } else {
        Status=LpcPciIoProtocol->Pci.Write(LpcPciIoProtocol, EfiPciIoWidthUint16,ICH_REG_LPC_IODEC,1,&tmp16);
        if (EFI_ERROR(Status)) return Status;
    }
	
    //RW_PCI16_SB(ICH_REG_LPC_EN, IoEnSet16, ~IoEnMask16);
    if (LpcPciIo==NULL){
        tmp16=PciCf8Read16((UINTN)NumOfBdf+ICH_REG_LPC_EN);
	} else {
        Status=LpcPciIoProtocol->Pci.Read(LpcPciIoProtocol, EfiPciIoWidthUint16,ICH_REG_LPC_EN,1,&tmp16);
        if (EFI_ERROR(Status)) return Status;
    }
    tmp16=tmp16&IoEnMask16;
    tmp16=tmp16|IoEnSet16;
	
	if (LpcPciIo==NULL){    
        PciCf8Write16((UINTN)NumOfBdf+ICH_REG_LPC_EN,tmp16);
    } else {
        Status=LpcPciIoProtocol->Pci.Write(LpcPciIoProtocol, EfiPciIoWidthUint16,ICH_REG_LPC_EN,1,&tmp16);
        if (EFI_ERROR(Status)) return Status;
    }

    // Porting End

    return EFI_SUCCESS;
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

