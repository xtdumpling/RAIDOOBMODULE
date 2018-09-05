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
//*************************************************************************

#ifndef __CRBLIB_H__
#define __CRBLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Macro Definition(s)
#define PCI_SAD_ALL_DEVICE_NUMBER                   0x1D 
#define PCI_SAD_ALL_FUNC_NUMBER                     0x00

#define R_PCI_SAD_ALL_PAM0123                       0x40   //PAM0123 Register Offset
#define  B_PAM0_HIENABLE                            BIT4|BIT5   //DRAM RW enable for region 0F0000-0FFFFF
#define R_PCI_SAD_ALL_PAM456                        0x44   //PAM456 Register Offset
#define  B_PAM4_LOENABLE                            BIT0|BIT1   //DRAM RW enable for region 0D8000-0DBFFF
#define  B_PAM4_HIENABLE                            BIT4|BIT5   //DRAM RW enable for region 0DC000-0DFFFF
#define  B_PAM5_LOENABLE                            BIT8|BIT9   //DRAM RW enable for region 0E0000-0E3FFF
#define  B_PAM5_HIENABLE                            BIT12|BIT13   //DRAM RW enable for region 0E4000-0E7FFF
#define  B_PAM6_LOENABLE                            BIT16|BIT17   //DRAM RW enable for region 0E8000-0EBFFF
#define  B_PAM6_HIENABLE                            BIT20|BIT21   //DRAM RW enable for region 0EC000-0EFFFF

//Enable DRAM RW for 0F0000-0FFFFF region specified in PAM0123 register
#define PAM0123_DRAM_RW_ENABLE                      B_PAM0_HIENABLE
//Enable DRAM RW for all regions specified in PAM456 register
#define PAM456_DRAM_RW_ENABLE                       B_PAM4_LOENABLE|B_PAM4_HIENABLE|B_PAM5_LOENABLE|B_PAM5_HIENABLE|B_PAM6_LOENABLE|B_PAM6_HIENABLE

VOID NbRuntimeShadowRamWrite(IN BOOLEAN Enable);

UINT8 getWakeupTypeForSmbios (VOID);

#ifdef __cplusplus
}
#endif
#endif


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
