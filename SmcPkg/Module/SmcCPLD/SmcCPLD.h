//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************

#include "SmcCPLDInterface.h"

#define X86_GPO0	0x7000
#define 	EN_NMI				(1<<1)
#define 	EN_RST				(1<<0)
#define	BMC_EXTRST_N		(1<<4)
#define	EN_ADR_TRIGGER	(1<<5)

#define X86_GPO1	0x7001
#define X86_GPI2	0x7002
#define X86_GPI3	0x7003
#define X86_GPI4	0x7004
#define X86_GPI5	0x7005
#define X86_GPI6	0x7006
#define X86_GPI7	0x7007
#define TCCR0		0x705E
#define TCTOPSET0	0x7060
#define TCTOPSET1	0x7061
#define TCCR2		0x7064
#define TCCNT0		0x7065
#define TCCNT1		0x7066
#define TCTOP0		0x7067
#define TCTOP1		0x7068
#define TCSR0		0x706D

/*
#define HST_STS		0x00
#define HST_CNT		0x02
#define HST_CMD		0x03
#define HST_SLVA	0x04
#define HST_D0		0x05
#define TIMEOUT_COUNT	1024
*/

#define _SMC_CPLD_GET_SMBUS_ADDR(A)		((A>>8)&0xff)
#define _SMC_CPLD_GET_SMBUS_OFFSET(A)	(A&0xff)

#define SMC_CPLD_SMBUS_RETRY		5
#define SMC_CPLD_IPMI_RETRY		16
/*
typedef enum  {
	SMC_BY_SMBUS
}SMC_START_COUNTER_TYPE;
*/



typedef enum  {
	SMC_PLD_WDT_DISABLED = 0,
	SMC_PLD_WDT_POWERON,
	SMC_PLD_WDT_POST,
	SMC_PLD_WDT_OS
}SMC_PLD_WDT_TYPE;

EFI_STATUS SMCCPLDSmbusReadByte(UINT8, UINT8, UINT8*);
EFI_STATUS SMCCPLDSmbusWriteByte(UINT8, UINT8, UINT8);

STATIC EFI_STATUS _SMCCPLDSmbusReadByte(UINT16 addr_offset, UINT8* data)
{
	return SMCCPLDSmbusReadByte(
			(UINT8)_SMC_CPLD_GET_SMBUS_ADDR(addr_offset), 
			(UINT8)_SMC_CPLD_GET_SMBUS_OFFSET(addr_offset),
			data);
}

STATIC EFI_STATUS _SMCCPLDSmbusWriteByte(UINT16 addr_offset, UINT8 data)
{
	return SMCCPLDSmbusWriteByte(
			(UINT8)_SMC_CPLD_GET_SMBUS_ADDR(addr_offset), 
			(UINT8)_SMC_CPLD_GET_SMBUS_OFFSET(addr_offset),
			data);
}

STATIC VOID _SMCCPLDGetTimerValue(
	UINT16 WDTSec, 
	UINT8* byte_5e, 
	UINT8* byte_60, 
	UINT8* byte_61)
{
	UINT8 bitlshift = 16;
	UINT16 temp = 1;
	
	if ( WDTSec > 0 && WDTSec <= 2 ) {
		*byte_5e = 1<<3;
		bitlshift -= 1;
	}
	else if ( WDTSec > 2 && WDTSec <= 16 ) {
		*byte_5e = 2<<3;
		bitlshift -= 4;
	}
	else if ( WDTSec > 16 && WDTSec <= 128 ) {
		*byte_5e = 3<<3;
		bitlshift -= 7;
	}
	else if ( WDTSec > 128 && WDTSec <= 512 ) {
		*byte_5e = 4<<3;
		bitlshift -= 9;
	}
	else if ( WDTSec > 512 && WDTSec <= 2048 ) {
		*byte_5e = 5<<3;
		bitlshift -= 11;
	}
	else {
		*byte_5e = 0;
		*byte_60 = *byte_61 = 0xff;
		return;
	}

	WDTSec = WDTSec << bitlshift;
	WDTSec--;
	*byte_60 = (UINT8)(WDTSec & 0xff);
	*byte_61 = (UINT8)((WDTSec>>8) & 0xff);
}


STATIC EFI_STATUS _ResetOrNMI(SMC_WDT_TIMOUT_ACTION RstNMI)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	UINT8 data;
	
	Status = _SMCCPLDSmbusReadByte(X86_GPO0, &data);
	data &= ~3;
	
	Status = _SMCCPLDSmbusWriteByte(X86_GPO0, data);		// To re-enable it, CPLD need set Bit[1:0] to 0 then set Bit[1:0] to 01/10/11. or CPLD will do not nothing when WDT timeout
	
	switch(RstNMI) {
		case SMC_CPLD_NMT:
			data |= BIT1;
			break;
		case SMC_CPLD_RESET:
		default:
			data |= BIT0;
			break;
	}
	
	Status = _SMCCPLDSmbusWriteByte(X86_GPO0, data);

	return Status;
}

STATIC EFI_STATUS _SetCPLDWatchdog(UINT16 sec, SMC_WDT_TIMOUT_ACTION RstNMI)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	
	UINT8	byte_5e, byte_60, byte_61, data;
	UINT8	chkbyte_5e, chkbyte_60, chkbyte_61;

	// 1. Pause the Watchdog
	Status = _SMCCPLDSmbusWriteByte(TCCR2, 0x01);

	if ( sec == 0 ) {
		return Status;
	}
	
	// 2. Set the counter
	_SMCCPLDGetTimerValue(sec, &byte_5e, &byte_60, &byte_61);
	Status = _SMCCPLDSmbusWriteByte(TCCR0, byte_5e);
	Status = _SMCCPLDSmbusWriteByte(TCTOPSET0, byte_60);
	Status = _SMCCPLDSmbusWriteByte(TCTOPSET1, byte_61);
	
	// 3 Double check the register is set correctly.
	chkbyte_5e = chkbyte_60 = chkbyte_61 = 0x5a;
	Status = _SMCCPLDSmbusReadByte(TCCR0, &chkbyte_5e);
    	Status = _SMCCPLDSmbusReadByte(TCTOPSET0, &chkbyte_60);
	Status = _SMCCPLDSmbusReadByte(TCTOPSET1, &chkbyte_61);
	
	if ( chkbyte_5e == byte_5e &&
		chkbyte_60 == byte_60 &&
		chkbyte_61 == byte_61 ) {
		;
	}
	else {
		DEBUG((-1, "Failed to set 5E, 60, 61\n"));
		return EFI_DEVICE_ERROR;
	}
	
	// 4 Enable reset
	Status = _SMCCPLDSmbusReadByte(X86_GPO0, &data);
	data &= ~3;
	Status = _SMCCPLDSmbusWriteByte(X86_GPO0, data);
	Status = _ResetOrNMI(RstNMI);
	
	// 5. Start the counter
	Status = _SMCCPLDSmbusWriteByte(TCCR2, 0x02);

	return Status;
}

STATIC EFI_STATUS _SetADR(BOOLEAN bADREn, BOOLEAN bRstBtnADR)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	UINT8 data;

	if ( bADREn ) {
		// Set X86_GPO0
		Status = _SMCCPLDSmbusReadByte(X86_GPO0, &data);
		data |= BIT5;
		if ( bRstBtnADR )
			data |= BIT3;
		else
			data &= ~BIT3;
		Status = _SMCCPLDSmbusWriteByte(X86_GPO0, data);

		// Set X86_GPO1
		Status = _SMCCPLDSmbusReadByte(X86_GPO1, &data);
		data |= BIT1+BIT2+BIT3;
		Status = _SMCCPLDSmbusWriteByte(X86_GPO1, data);
	}
	else {
		Status = _SMCCPLDSmbusReadByte(X86_GPO0, &data);
		data &= ~BIT5;
		Status = _SMCCPLDSmbusWriteByte(X86_GPO0, data);
	}

	return Status;
}

