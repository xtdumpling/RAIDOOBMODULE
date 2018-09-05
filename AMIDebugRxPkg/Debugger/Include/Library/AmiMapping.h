//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//**********************************************************************


//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        AMIMAPPING.H
//
// Description: This file is the mapping header file that maps Tiano related
//              equates to AMI equates
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>


#ifndef __AMI_MAPPING__H__
#define __AMI_MAPPING__H__
#ifdef __cplusplus
extern "C" {
#endif
#define STATIC  static
#define EFI_PPI_DEFINITION( a )  EFI_STRINGIZE(Ppi/a.h)
#define EFI_STRINGIZE( a )       #a
//Compatibility
// PPI's
#define PEI_CPU_IO_PPI      EFI_PEI_CPU_IO_PPI
#define PEI_STALL_PPI       EFI_PEI_STALL_PPI
#define PEI_STALL_PPI_GUID  EFI_PEI_STALL_PPI_GUID
//
// Define macros to build data structure signatures from characters.
//
#define EFI_SIGNATURE_16( A, B )        ( (A) | (B << 8) )
#define EFI_SIGNATURE_32( A, B, C, D )  ( EFI_SIGNATURE_16( A, B ) | \
                                         (EFI_SIGNATURE_16( C, D ) << 16) )
#define EFI_SIGNATURE_64( A, B, C, D, E, F, G, H ) \
    ( EFI_SIGNATURE_32( A, B, C, D ) | ( (UINT64) \
                                        ( EFI_SIGNATURE_32 \ (E, F, G,\
                                                              H) ) << 32 ) )

#define PEI_CR( Record, TYPE, Field, Signature ) \
    _CR( Record, TYPE, Field )
//
//  CONTAINING_RECORD - returns a pointer to the structure
//      from one of it's elements.
//
#define _CR( Record, TYPE, Field )  ( (TYPE *) \
                                     ( (CHAR8 *) (Record) -\
                                      (CHAR8 *) &( ( (TYPE *) 0 )->Field ) ) )

//#define EFI_D_ERROR  0x80000000               // Error

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
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
