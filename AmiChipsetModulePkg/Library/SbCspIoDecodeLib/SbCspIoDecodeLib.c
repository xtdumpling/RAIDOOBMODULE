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

/** @file SbCspIoDecodeLib.c
    This file contains code for South Bridge LPC IO Decode
    initialization in the Library stage

*/

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------

#include <Token.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/AmiSio.h>
#include <Library/PchCycleDecodingLib.h>
#include <PchAccess.h>
#include <Protocol/PciIo.h>
#include <Library/PciCf8Lib.h>

/**
    This function set LPC Bridge Generic Decoding

    @param *LpcPciIo - Pointer to LPC PCI IO Protocol (This is not used but kept for backwards compatible)
    @param Base      - I/O base address
    @param Length    - I/O Length
    @param Enabled   - Enable/Disable the generic decode range register
    
    @retval EFI_SUCCESS - Set successfully.
    @retval EFI_UNSUPPORTED - This function is not implemented or the Length more than the maximum supported size of generic range decoding.
    @retval EFI_INVALID_PARAMETER - the Input parameter is invalid.
    @retval EFI_OUT_OF_RESOURCES - There is not available Generic Decoding Register.
    @retval EFI_NOT_FOUND - the generic decode range will be disabled is not found.

**/
EFI_STATUS AmiSioLibSetLpcGenericDecoding (
    IN VOID           *LpcPciIo,
    IN UINT16         Base,
    IN UINT16         Length,
    IN BOOLEAN        Enable
    )
{

    UINTN             D31F0Addr;
    UINT32            IoGenDecode32, BaseIoGenDecReg, DmiBaseIoGenericDecReg;
    UINT16            IoGenDecIndex, MaxIoGenDecIndex;
    UINT16            Buffer16;
    UINT8             Bsf8 = 0;
    UINT8             Bsr8 = 0;
    EFI_STATUS				Status=EFI_SUCCESS;
    EFI_PCI_IO_PROTOCOL		*LpcPciIoProtocol=(EFI_PCI_IO_PROTOCOL*)LpcPciIo;

    if(LpcPciIo != NULL) {
        if (Enable) {
            return PchLpcGenIoRangeSet (Base, Length, LPC_ESPI_FIRST_SLAVE);
        } else {
            return EFI_UNSUPPORTED;
        }
    }
	
    if (Length > 0x100) return EFI_UNSUPPORTED;

    if (Length == 0) return EFI_INVALID_PARAMETER;

    if (Length < 4) Length = 4;

    if (LpcPciIo==NULL) D31F0Addr = (UINT32)(PCI_CF8_LIB_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, 0));

    #if defined(PILOT4_LPC_ESPI_SLAVE_CONNECTION) && PILOT4_LPC_ESPI_SLAVE_CONNECTION
        MaxIoGenDecIndex = 1;
        BaseIoGenDecReg = R_PCH_LPC_PCCS1GIR1;
        DmiBaseIoGenericDecReg = R_PCH_PCR_DMI_SEGIR;
    #else
        MaxIoGenDecIndex = 4;
        BaseIoGenDecReg = R_PCH_LPC_GEN1_DEC;
        DmiBaseIoGenericDecReg = R_PCH_PCR_DMI_LPCLGIR1;
    #endif

    //
    // Read I/O Generic Decodes Register.
    //
    for (IoGenDecIndex = 0; IoGenDecIndex < MaxIoGenDecIndex; IoGenDecIndex++) {
        if (LpcPciIo==NULL){
            IoGenDecode32=PciCf8Read32(D31F0Addr+(BaseIoGenDecReg + IoGenDecIndex * 4));
        } else {
            Status=LpcPciIoProtocol->Pci.Read(LpcPciIoProtocol, EfiPciIoWidthUint32,(BaseIoGenDecReg + IoGenDecIndex * 4),1,&IoGenDecode32);
            if (EFI_ERROR(Status)) return Status;
        }

      if (Enable) {
          if ((IoGenDecode32 & 1) == 0) break;
      	  else if ((IoGenDecode32 & 0xfffc) == Base) break;
      } else {
          if (((IoGenDecode32 & 0xfffc) == Base) && (IoGenDecode32 & 1)) {
            //
            // Disable & clear the base/mask fields
            //
            IoGenDecode32 = 0;
            break;
          }
      }
    }

    if (Enable) {
        if (IoGenDecIndex == MaxIoGenDecIndex) return EFI_OUT_OF_RESOURCES;

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
        if (IoGenDecIndex == MaxIoGenDecIndex) return EFI_NOT_FOUND;
    }
    if (LpcPciIo==NULL){
        PciCf8Write32(D31F0Addr+(BaseIoGenDecReg + IoGenDecIndex * 4),IoGenDecode32);
        MmioWrite32 (PCH_PCR_ADDRESS(PID_DMI, DmiBaseIoGenericDecReg + IoGenDecIndex * 4), IoGenDecode32);
	} else {
        Status=LpcPciIoProtocol->Pci.Write(LpcPciIoProtocol, EfiPciIoWidthUint32,(BaseIoGenDecReg + IoGenDecIndex * 4),1,&IoGenDecode32);
        if (EFI_ERROR(Status)) return Status;
        MmioWrite32 (PCH_PCR_ADDRESS(PID_DMI, DmiBaseIoGenericDecReg + IoGenDecIndex * 4), IoGenDecode32);
    }

    return EFI_SUCCESS;
}

/**
    This function sets LPC Bridge Device Decoding

    @param *LpcPciIo - Pointer to LPC PCI IO Protocol (This is not used but kept for backwards compatible)
    @param Base      - I/O base address, if Base is 0 means disabled the decode of the device
    @param DevUid    - The device Unique ID
    @param Type      - Device Type, please refer to AMISIO.h
    
    @retval EFI_SUCCESS - Set successfully.
    @retval EFI_UNSUPPORTED - There is not proper Device Decoding register for the device UID.

**/
EFI_STATUS AmiSioLibSetLpcDeviceDecoding (
    IN VOID                 *LpcPciIo,
    IN UINT16               Base,
    IN UINT8                DevUid,
    IN SIO_DEV_TYPE         Type
    )
{
    EFI_STATUS              Status = EFI_UNSUPPORTED;
    UINT16                  ComRange[] = { 0x3f8, 0x2f8, 0x220, 0x228,\
                                           0x238, 0x2e8, 0x338, 0x3e8, 0x2f0, 0x2e0, 0};
    UINT16                  LptRange[] = { 0x378, 0x278, 0x3bc, 0};
    UINT16                  FpcRange[] = { 0x3f0, 0x370, 0};
    UINT16                  IoRangeMask16 = 0xffff;
    UINT16                  IoRangeSet16 = 0;
    UINT16                  IoEnMask16 = 0xffff;
    UINT16                  IoEnSet16 = 0;
    UINT8                   Index;
    UINTN                   D31F0Addr;
    UINT16                  tmp16;
    EFI_PCI_IO_PROTOCOL		*LpcPciIoProtocol=(EFI_PCI_IO_PROTOCOL*)LpcPciIo;
	UINT8					IoRoutingEnablesReg;

    switch (Type) {
      case (dsFDC) :
        //
        // FDC Address Range
        //
        if (Base == 0) IoEnMask16 &= ~BIT03;
        else {
            for (Index = 0; (FpcRange[Index] != 0) && (FpcRange[Index] != Base); Index++);
            if (FpcRange[Index]) {
                IoEnSet16 |= BIT03;
                IoRangeMask16 &= ~BIT12;
                IoRangeSet16 |= (Index << 12);
            }
            else return EFI_UNSUPPORTED;
        }
        break;

      case (dsLPT) :
        //
        // LPT Address Range
        //
        if (Base == 0) IoEnMask16 &= ~BIT02;
        else {
            for (Index = 0; (LptRange[Index] != 0) && (LptRange[Index] != Base); Index++);
            if (LptRange[Index]) {
                IoEnSet16 |= BIT02;
                IoRangeMask16 &= ~(BIT09 | BIT08);
                IoRangeSet16 |= (Index << 8);
            } else return EFI_UNSUPPORTED;
        }
        break;

      case (dsUART) :
        //
        // ComA Address Range
        //
        if (Base == 0) {
            if (DevUid) IoEnMask16 &= ~BIT01;
            else IoEnMask16 &= ~BIT00;
        } else {
            for (Index = 0; (ComRange[Index] != 0) && (ComRange[Index] != Base); Index++);
            if (ComRange[Index]) {
                if (DevUid) {
                    IoEnSet16 |= BIT01;
                    IoRangeMask16 &= ~(BIT06 | BIT05 | BIT04);
                    IoRangeSet16 |= (Index << 4);
                } else {
                    IoEnSet16 |= BIT00;
                    IoRangeMask16 &= ~(BIT02 | BIT01 | BIT00);
                    IoRangeSet16 |= Index;
                }
            } else return EFI_UNSUPPORTED;
        }
        break;

      //
      // KBC Address Enable
      //
      case (dsPS2K) :
      case (dsPS2M) :
      case (dsPS2CK) :
      case (dsPS2CM) :
        if (Base == 0) IoEnMask16 &= ~BIT10;
        else IoEnSet16 |= BIT10;
        break;

      case (dsGAME) :
        //
        // Game Port Address Enable
        //
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

      case (0xff) :
        //
        // LPC CFG Address Enable
        //
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

    if (LpcPciIo==NULL) D31F0Addr = (UINT32)(PCI_CF8_LIB_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, 0));

    //    RW_PCI16_SB(R_PCH_LPC_IOD, IoRangeSet16, ~IoRangeMask16);
    if (LpcPciIo==NULL){
        tmp16=PciCf8Read16((UINTN)D31F0Addr+R_PCH_LPC_IOD);
	} else {
        Status=LpcPciIoProtocol->Pci.Read(LpcPciIoProtocol, EfiPciIoWidthUint16,R_PCH_LPC_IOD,1,&tmp16);
        if (EFI_ERROR(Status)) return Status;
    }
	
    tmp16=tmp16&IoRangeMask16;
    tmp16=tmp16|IoRangeSet16;
    
	if (LpcPciIo==NULL){    
        PciCf8Write16((UINTN)D31F0Addr+R_PCH_LPC_IOD,tmp16);
        MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LPCIOD), (UINT16)tmp16);
    } else {
        Status = PchLpcIoDecodeRangesSet (tmp16);
        if (EFI_ERROR(Status)) return Status;
    }

	#if defined(PILOT4_LPC_ESPI_SLAVE_CONNECTION) && PILOT4_LPC_ESPI_SLAVE_CONNECTION
      IoRoutingEnablesReg = R_PCH_LPC_PCCS1IORE;
	#else
      IoRoutingEnablesReg = R_PCH_LPC_IOE;
	#endif

    if (LpcPciIo==NULL){
        tmp16=PciCf8Read16((UINTN)D31F0Addr+IoRoutingEnablesReg);
	} else {
        Status=LpcPciIoProtocol->Pci.Read(LpcPciIoProtocol, EfiPciIoWidthUint16,IoRoutingEnablesReg,1,&tmp16);
        if (EFI_ERROR(Status)) return Status;
    }
    tmp16=tmp16&IoEnMask16;
    tmp16=tmp16|IoEnSet16;

	if (LpcPciIo==NULL){    
        PciCf8Write16((UINTN)D31F0Addr+IoRoutingEnablesReg,tmp16);
        #if defined(PILOT4_LPC_ESPI_SLAVE_CONNECTION) && PILOT4_LPC_ESPI_SLAVE_CONNECTION
            //
            // For eSPI CS#1 device program PCI offset 82h respectively
            //
            PciCf8Or16((UINTN)D31F0Addr+R_PCH_LPC_IOE, tmp16);
        #endif

        MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LPCIOE), (UINT16)tmp16);
    } else {
        #if defined(PILOT4_LPC_ESPI_SLAVE_CONNECTION) && PILOT4_LPC_ESPI_SLAVE_CONNECTION
            UINT8 SlaveDevice = ESPI_SECONDARY_SLAVE;
        #else
            UINT8 SlaveDevice = LPC_ESPI_FIRST_SLAVE;
        #endif

        Status = PchLpcIoEnableDecodingSet (tmp16, SlaveDevice);
        if (EFI_ERROR(Status)) return Status;
    }

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
