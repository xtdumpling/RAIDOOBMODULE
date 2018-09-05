//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Aug/13/2018
//
//****************************************************************************

#ifndef	_H_SmcLsiRaidOOB_SETUP_VFR_
#define	_H_SmcLsiRaidOOB_SETUP_VFR_

#ifdef __cplusplus
extern "C" {
#endif

#define SMC_LSI_OOB_FORM_SET_GUID    { 0xca113bd9, 0x21cc, 0xaa1b, 0x33, 0x58, 0xa9, 0x73, 0x78, 0xdd, 0x3c, 0x5e }

#define SMC_LSI_OOB_VAR_GUID    { 0x98775d3c, 0xC22A, 0x1197, 0x71, 0xCC, 0xF1, 0x36, 0x72, 0x16, 0xAC, 0xD9 }

#define LSI_3108_HF003_VAR_GUID    {0x83058311, 0xA423, 0x4997, 0x82, 0x03, 0xDB, 0xCF, 0xFB, 0x75, 0xDD, 0xCC }

#define SMC_LSI_OOB_FORM_GOTO_LABEL		0x1000
#define SMC_LSI_OOB_FORM             	0x2000
#define SMC_LSI_OOB_FORM_START			0x2200
#define	SMC_LSI_OOB_Q_ID_START			0x3000
#define SMC_LSI_OOB_VAR_ID_START		0x4000

#pragma pack(1)


#pragma pack()

#ifdef VFRCOMPILE

//VarID should increment.
#define SMC_LSI_VAR_HF003_3108_00\
    varstore MIDDLE_VAR, varid = 0x4000, name = HF003_3108_00, guid = SMC_LSI_OOB_VAR_GUID;


#endif
#ifdef __cplusplus
}
#endif
#endif
