//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  USRA trace library

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

//#include "CommonAccessLib.h"
//#include <CommonAccessType.h>
#include <UsraAccessApi.h>
//#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include "USRATrace.h"
#include "SysFunc.h"
#include "RcRegs.h"
#include <CpuPciAccess.h>


VOID
EFIAPI
InternalPrintMessage (
  IN  CONST CHAR8  *Format,
  ...
  );
#define DEBUGOEM(Expression)   InternalPrintMessage Expression

/**
  this routine is for print register info to serial port.

  @param  Address: A pointer of the address of the Common Address Structure to write to.
  @param  AlignedAddress: physical address converted from Address.
  @param  ReadFlag: TRUE -- read; FALSE -- Write.
  @param  Buffer A pointer of buffer for the value write to the CSR.

  @retval VOID

**/
VOID
USRATrace(
  IN USRA_ADDRESS    *Address,
  IN UINTN                     AlignedAddress,
  IN UINT8                     Operation,
  IN VOID                     *Buffer1,
  IN VOID                     *Buffer2
){
    UINT8  bDynamic;
    //
    //use Io port 081h to dynamic control trace of USRA.
    // Bit  7     6     5       4          3                    2:0
    //    Read, Write Modify, GetAddr  Long/Short Trace    0x5 as signature
    //
    //
    bDynamic = IoRead8(0x81);

    if((bDynamic & 0x07) != 0x5){
      return;
    }
    // in case read, bit 7 should set.
    if(Operation == TraceRead){
      if((bDynamic & 0x80) == 0 ){
        return;
      }
    }
    // in case read, bit 6 should set.
    if(Operation == TraceWrite){
      if((bDynamic & 0x40) == 0 ){
        return;
      }
    }
    // in case modify, bit 5 should set.
    if(Operation == TraceModify){
      if((bDynamic & 0x20) == 0 ){
        return;
      }
    }
    // in case get address, bit 4 should set.
    if(Operation == TraceGetAddr){
      if((bDynamic & 0x10) == 0 ){
        return;
      }
    }
   //check if is SBSP SYSTEMSEMAPHORE0_UBOX_MISC_REG, skip this semaphore debug display
   if(Address->Attribute.AddrType == AddrTypeCSR && Address->Csr.SocketId == 0  && \
     ((CSR_OFFSET *)&Address->Csr.Offset)->Bits.boxtype == BOX_UBOX && ((CSR_OFFSET *)&Address->Csr.Offset)->Bits.funcblk == 2 && ((CSR_OFFSET *)&Address->Csr.Offset)->Bits.offset == (SYSTEMSEMAPHORE0_UBOX_MISC_REG & 0xFFF)){
     return;
   }
   switch (Operation)
   {
   case TraceRead:
     DEBUGOEM(("[USRA TRACE] Read "));
     break;
   case TraceWrite:
     DEBUGOEM(("[USRA TRACE] Write "));
     break;
   case TraceModify:
     DEBUGOEM(("[USRA TRACE] Modify "));
     break;
   case TraceGetAddr:
     DEBUGOEM(("[USRA TRACE] GetAddr "));
     break;
   default:
     break;
   }

   switch ((UINT32)Address->Attribute.AccessWidth)
   {
   case  UsraWidth8:
     DEBUGOEM(("Byte  "));
     break;
   case  UsraWidth16:
     DEBUGOEM(("Word  "));
     break;
   case  UsraWidth32:
     DEBUGOEM(("DWord "));
     break;
   default:
     DEBUGOEM(("QWord "));
     break;
   }

   if( (bDynamic & 0x08) != 0 ){   // check if bit3 =1, to Display long trace
     if(Address->Attribute.AddrType == AddrTypeCSR){
        DEBUGOEM((" Socket:%d  BoxInst:%d  ", Address->Csr.SocketId, Address->Csr.InstId));
        switch(((CSR_OFFSET *)&Address->Csr.Offset)->Bits.boxtype){
          case  BOX_CHA_MISC:
              DEBUGOEM(("CHA_MISC  "));
              break;
          case  BOX_CHA_PMA:
              DEBUGOEM(("CHA_PMA  "));
              break;
          case  BOX_CHA_CMS:
              DEBUGOEM(("CHA CMS  "));
              break;
          case  BOX_CHABC:
              DEBUGOEM(("CHABC  "));
              break;
          case  BOX_PCU:
              DEBUGOEM(("PCU  "));
              break;
          case  BOX_VCU:
              DEBUGOEM(("VCU  "));
              break;
          case  BOX_M2MEM:
              DEBUGOEM(("M2MEM  "));
              break;
          case  BOX_MC:
              DEBUGOEM(("MC  "));
              break;
          case  BOX_MCIO:
              DEBUGOEM(("MCIO  "));
              break;
          case  BOX_KTI:
              DEBUGOEM(("KTI  "));
              break;
          case  BOX_M3KTI:
              DEBUGOEM(("M3KTI  "));
              break;
          case  BOX_MCDDC:
              DEBUGOEM(("MCDDC  "));
              break;
          case  BOX_M2UPCIE:
              DEBUGOEM(("M2UPCIE  "));
              break;
          case  BOX_IIO_PCIE_DMI:
              DEBUGOEM(("IIO PCIE DMI  "));
              break;
          case  BOX_IIO_PCIE:
              DEBUGOEM(("IIO PCIE  "));
              break;
          case  BOX_IIO_PCIE_NTB:
              DEBUGOEM(("IIO PCIE NTB  "));
              break;
          case  BOX_IIO_CB:
              DEBUGOEM(("IIO CB  "));
              break;
          case  BOX_IIO_VTD:
              DEBUGOEM(("IIO VTD  "));
              break;
          case  BOX_IIO_DFX:
              DEBUGOEM(("IIO DFX  "));
              break;
          case  BOX_UBOX:
              DEBUGOEM(("UBOX  "));
              break;
          default:
              break;
          }

       DEBUGOEM(("FuncBlk:%x  offset:%x ", ((CSR_OFFSET *)(&Address->Csr.Offset))->Bits.funcblk, (UINT16)((CSR_OFFSET *)(&Address->Csr.Offset))->Bits.offset));
     } else if(Address->Attribute.AddrType == AddrTypePCIE){
         DEBUGOEM(("Seg:%d  Bus:%x  Dev:%x  Func:%x  Offset:%x ", Address->Pcie.Seg,Address->Pcie.Bus,Address->Pcie.Dev,
         Address->Pcie.Func,Address->Pcie.Offset));
     }
   }  //

   if (Operation == TraceRead || Operation == TraceWrite) {
     DEBUGOEM((" (0x%x)  ", AlignedAddress));
     switch ((UINT32)Address->Attribute.AccessWidth)
     {
     case  UsraWidth8:
       DEBUGOEM(("Value: %x \n", *((UINT8*)Buffer1)));
       break;
     case  UsraWidth16:
       DEBUGOEM(("Value: %x \n", *((UINT16*)Buffer1)));
       break;
     case  UsraWidth32:
       DEBUGOEM(("Value: %x \n", *((UINT32*)Buffer1)));
       break;
     default:
       DEBUGOEM(("Value: %lx \n",  *((UINT64*)Buffer1)));
       break;
     }
   } else if (Operation == TraceModify) {
     switch ((UINT32)Address->Attribute.AccessWidth)
     {
     case  UsraWidth8:
       DEBUGOEM(("AndMask: %x", *((UINT8*)Buffer1)));
       DEBUGOEM(("  OrMask: %x \n", *((UINT8*)Buffer2)));
       break;
     case  UsraWidth16:
       DEBUGOEM(("AndMask: %x", *((UINT16*)Buffer1)));
       DEBUGOEM(("  OrMask: %x \n", *((UINT16*)Buffer2)));
       break;
     case  UsraWidth32:
       DEBUGOEM(("AndMask: %x", *((UINT32*)Buffer1)));
       DEBUGOEM(("  OrMask: %x \n", *((UINT32*)Buffer2)));
       break;
     default:
       DEBUGOEM(("AndMask: %x", *((UINT64*)Buffer1)));
       DEBUGOEM(("  OrMask: %x \n", *((UINT64*)Buffer2)));
       break;
     }
   } else {
     DEBUGOEM(("Physical Address: 0x%x \n", AlignedAddress));
   }
}
