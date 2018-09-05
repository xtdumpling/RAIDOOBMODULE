//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************

#ifndef __SMCCPLDINTERFACE_H__
#define __SMCCPLDINTERFACE_H__
#ifdef __cplusplus
extern "C" {
#endif


EFI_GUID gSmcCPLDProtocolGuid = { 0xf735f67c, 0xa649, 0x41b9, { 0xa0, 0xf4, 0xdc, 0x9c, 0x7d, 0x8, 0x36, 0x31 } };

typedef enum  {
	SMC_CPLD_RESET = 0,
	SMC_CPLD_NMT
}SMC_WDT_TIMOUT_ACTION;


typedef struct _SMCCPLDFN {
	EFI_STATUS (*SetCPLDWatchdog)(UINT16 iSecond, SMC_WDT_TIMOUT_ACTION RstNMI);
	EFI_STATUS (*SetADR)(BOOLEAN bADREn, BOOLEAN bRstBtnADR);
		// Parameter #1 ADR enable or disable
		// Parameter #2 Reset Button will trigger ADR or not.
	EFI_STATUS (*SetCPLDBYBMC)(UINT32 byteFn);
		// byteFn 
		//       Bit[0] - ThrottleOnPowerFail
		//       Bit[31:1] - Reserve
} SMCCPLDFN;


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif // __SMCCPLDINTERFACE_H__