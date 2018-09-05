//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Leon Xu
//    Date:     20-Jul-2017
//
//****************************************************************************
//****************************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		ProjectPeiDriver.c
//
// Description:
//  This file implement function of PROJECTDRIVER_PROTOCOL
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include <token.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
#include "SmcCspLibBmc.h"
#include <Setup.h>
#include <Library/IoLib.h>
#include "SmcLibCommon.h"
#include "SmcLibBmcPrivate.h"
#include <Library/SetupLib.h>

/*
Hi Leon
Performance mode setting as below

:stop IPMI polling
smcipmitool %1 ADMIN ADMIN ipmi raw 30 70 df 0

:set page(CPU1)
smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a0 0 0 3f
:password
smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a0 0 2a B2 8a
smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a0 0 27 7c b3

:set page(CPU2)
smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a4 0 0 3f
:password

smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a4 0 2a b2 8a
smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a4 0 27 7c b3


:lp0_icur_scale = 82.45%
smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a0 0 0 20
smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a0 0 7A 2c f1
smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a4 0 0 20
smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a4 0 7A 2c f1

:lp0_vout_offset = +200mv
:smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a0 0 0 20
smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a0 0 4C 00 28
:smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a4 0 0 20
smcipmitool %1 ADMIN ADMIN ipmi raw 6 52 01 a4 0 4C 00 28

:start IPMI polling
smcipmitool %1 ADMIN ADMIN ipmi raw 30 70 df 1
*/
typedef struct {
	UINT8 size;
	UINT8 data[8];
} VRM_ARRAY;

#if X11DPU_SUPPORT
VRM_ARRAY gX11DPUMaxVrmSettings[] = {
                    {5, {0x01, 0xa0, 0x00, 0x00, 0x3f}},          //set page(CPU1)
                    {6, {0x01, 0xa0, 0x00, 0x2a, 0xB2, 0x8a}},    //CPU1 password
                    {6, {0x01, 0xa0, 0x00, 0x27, 0x7c, 0xb3}},    //CPU1 password
                    {5, {0x01, 0xa4, 0x00, 0x00, 0x3f}},          //set page(CPU2)
                    {6, {0x01, 0xa4, 0x00, 0x2a, 0xB2, 0x8a}},    //CPU2 password
                    {6, {0x01, 0xa4, 0x00, 0x27, 0x7c, 0xb3}},    //CPU2 password
                    {5, {0x01, 0xa0, 0x00, 0x00, 0x20}},          //lp0_icur_scale = 82.45%
                    {6, {0x01, 0xa0, 0x00, 0x7A, 0x18, 0xf1}},    //lp0_icur_scale = 82.45%
                    {5, {0x01, 0xa4, 0x00, 0x00, 0x20}},          //lp0_icur_scale = 82.45%
                    {6, {0x01, 0xa4, 0x00, 0x7A, 0x18, 0xf1}},    //lp0_icur_scale = 82.45%
                    {5, {0x01, 0xa0, 0x00, 0x00, 0x20}},          //lp0_vout_offset = +200mv
                    {6, {0x01, 0xa0, 0x00, 0x4C, 0x00, 0x28}},    //lp0_vout_offset = +200mv
                    {5, {0x01, 0xa4, 0x00, 0x00, 0x20}},          //lp0_vout_offset = +200mv
                    {6, {0x01, 0xa4, 0x00, 0x4C, 0x00, 0x28}}    //lp0_vout_offset = +200mv
};

VRM_ARRAY gX11DPUOrgVrmSettings[] = {
                    {5, {0x01, 0xa0, 0x00, 0x00, 0x3f}},          //set page(CPU1)
                    {6, {0x01, 0xa0, 0x00, 0x2a, 0xB2, 0x8a}},    //CPU1 password
                    {6, {0x01, 0xa0, 0x00, 0x27, 0x7c, 0xb3}},    //CPU1 password
                    {5, {0x01, 0xa4, 0x00, 0x00, 0x3f}},          //set page(CPU2)
                    {6, {0x01, 0xa4, 0x00, 0x2a, 0xB2, 0x8a}},    //CPU2 password
                    {6, {0x01, 0xa4, 0x00, 0x27, 0x7c, 0xb3}},    //CPU2 password
                    {5, {0x01, 0xa0, 0x00, 0x00, 0x20}},          //lp0_icur_scale = 82.45%
                    {6, {0x01, 0xa0, 0x00, 0x7A, 0x18, 0x01}},    //lp0_icur_scale = 82.45%
                    {5, {0x01, 0xa4, 0x00, 0x00, 0x20}},          //lp0_icur_scale = 82.45%
                    {6, {0x01, 0xa4, 0x00, 0x7A, 0x18, 0x01}},    //lp0_icur_scale = 82.45%
                    {5, {0x01, 0xa0, 0x00, 0x00, 0x20}},          //lp0_vout_offset = +200mv
                    {6, {0x01, 0xa0, 0x00, 0x4C, 0x00, 0x00}},    //lp0_vout_offset = +200mv
                    {5, {0x01, 0xa4, 0x00, 0x00, 0x20}},          //lp0_vout_offset = +200mv
                    {6, {0x01, 0xa4, 0x00, 0x4C, 0x00, 0x00}}     //lp0_vout_offset = +200mv
};
#endif

#if X11DPUZ_SUPPORT
VRM_ARRAY gX11DPUZMaxVrmSettings[] = {
		{ 5, {0x01, 0xa0, 0x00, 0x00, 0x3f, 0xff, 0xff} },	// #1
		
		{ 6, {0x01, 0xa0, 0x00, 0x2a, 0xB2, 0x8a, 0xff} },	// #2
		{ 6, {0x01, 0xa0, 0x00, 0x27, 0x7c, 0xb3, 0xff} },	// #3
		
		{ 5, {0x01, 0xa4, 0x00, 0x00, 0x3f, 0xff, 0xff} },	// #4
		
		{ 6, {0x01, 0xa4, 0x00, 0x2a, 0xb2, 0x8a, 0xff} },	// #5
		{ 6, {0x01, 0xa4, 0x00, 0x27, 0x7c, 0xb3, 0xff} },	// #6
		
		{ 5, {0x01, 0xa0, 0x00, 0x00, 0x20, 0xff, 0xff} },	// #7
		{ 6, {0x01, 0xa0, 0x00, 0x7a, 0x2c, 0xf1, 0xff} },	// #8
		{ 5, {0x01, 0xa4, 0x00, 0x00, 0x20, 0xff, 0xff} },	// #9
		{ 6, {0x01, 0xa4, 0x00, 0x7a, 0x2c, 0xf1, 0xff} },	// #10
		
		{ 5, {0x01, 0xa0, 0x00, 0x00, 0x20, 0xff, 0xff} },	// #11
		{ 6, {0x01, 0xa0, 0x00, 0x4c, 0x00, 0x28, 0xff} },	// #12
		{ 5, {0x01, 0xa4, 0x00, 0x00, 0x20, 0xff, 0xff} },	// #13
		{ 6, {0x01, 0xa4, 0x00, 0x4c, 0x00, 0x28, 0xff} }	// #14
};

VRM_ARRAY gX11DPUZOrgVrmSettings[] = {
		{ 5, {0x01, 0xa0, 0x00, 0x00, 0x3f, 0xff, 0xff} },	// #1
		
		{ 6, {0x01, 0xa0, 0x00, 0x2a, 0x00, 0x00, 0xff} },	// #2
		{ 6, {0x01, 0xa0, 0x00, 0x27, 0x7c, 0xb3, 0xff} },	// #3
		
		{ 5, {0x01, 0xa4, 0x00, 0x00, 0x3f, 0xff, 0xff} },	// #4
		
		{ 6, {0x01, 0xa4, 0x00, 0x2a, 0x00, 0x00, 0xff} },	// #5
		{ 6, {0x01, 0xa4, 0x00, 0x27, 0x7c, 0xb3, 0xff} },	// #6
		
		{ 5, {0x01, 0xa0, 0x00, 0x00, 0x20, 0xff, 0xff} },	// #7
		{ 6, {0x01, 0xa0, 0x00, 0x7a, 0x18, 0x01, 0xff} },	// #8
		{ 5, {0x01, 0xa4, 0x00, 0x00, 0x20, 0xff, 0xff} },	// #9
		{ 6, {0x01, 0xa4, 0x00, 0x7a, 0x18, 0x01, 0xff} },	// #10
		
		{ 5, {0x01, 0xa0, 0x00, 0x00, 0x20, 0xff, 0xff} },	// #11
		{ 6, {0x01, 0xa0, 0x00, 0x4c, 0x00, 0x00, 0xff} },	// #12
		{ 5, {0x01, 0xa4, 0x00, 0x00, 0x20, 0xff, 0xff} },	// #13
		{ 6, {0x01, 0xa4, 0x00, 0x4c, 0x00, 0x00, 0xff} }	// #14
};
#endif

#if X11DDW_SUPPORT
VRM_ARRAY gX11DDWMaxVrmSettings[] = {
		{5, {0x01, 0xb0, 0x00, 0x00, 0x00}},          //set page(CPU1)
		{5, {0x01, 0xb0, 0x00, 0xd5, 0xe0}},          //CPU_vout_offset = -60mv
		{5, {0x01, 0xc0, 0x00, 0x00, 0x00}},          //set page(CPU2)
		{5, {0x01, 0xc0, 0x00, 0xd5, 0xe0}},          //CPU_vout_offset = -60mv                   
		{5, {0x01, 0xb8, 0x00, 0x00, 0x00}},          //set page(MEM CHA)
		{5, {0x01, 0xb8, 0x00, 0xd5, 0xe0}},          //MEM_vout_offset = -60mv
		{5, {0x01, 0xbc, 0x00, 0x00, 0x00}},          //set page(MEM CHB)
		{5, {0x01, 0xbc, 0x00, 0xd5, 0xe0}},          //MEM_vout_offset = -60mv
		{5, {0x01, 0xc8, 0x00, 0x00, 0x00}},          //set page(MEM CHC)
		{5, {0x01, 0xc8, 0x00, 0xd5, 0xe0}},          //MEM_vout_offset = -60mv
		{5, {0x01, 0xcc, 0x00, 0x00, 0x00}},          //set page(MEM CHD)
		{5, {0x01, 0xcc, 0x00, 0xd5, 0xe0}},          //MEM_vout_offset = -60mv
};

VRM_ARRAY gX11DDWOrgVrmSettings[] = {
		{5, {0x01, 0xb0, 0x00, 0x00, 0x00}},          //set page(CPU1)
		{5, {0x01, 0xb0, 0x00, 0xd5, 0x00}},          //CPU_vout_offset = 0mv
		{5, {0x01, 0xc0, 0x00, 0x00, 0x00}},          //set page(CPU2)
		{5, {0x01, 0xc0, 0x00, 0xd5, 0x00}},          //CPU_vout_offset = 0mv                   
		{5, {0x01, 0xb8, 0x00, 0x00, 0x00}},          //set page(MEM CHA)
		{5, {0x01, 0xb8, 0x00, 0xd5, 0x00}},          //MEM_vout_offset = 0mv
		{5, {0x01, 0xbc, 0x00, 0x00, 0x00}},          //set page(MEM CHB)
		{5, {0x01, 0xbc, 0x00, 0xd5, 0x00}},          //MEM_vout_offset = 0mv
		{5, {0x01, 0xc8, 0x00, 0x00, 0x00}},          //set page(MEM CHC)
		{5, {0x01, 0xc8, 0x00, 0xd5, 0x00}},          //MEM_vout_offset = 0mv
		{5, {0x01, 0xcc, 0x00, 0x00, 0x00}},          //set page(MEM CHD)
		{5, {0x01, 0xcc, 0x00, 0xd5, 0x00}},          //MEM_vout_offset = 0mv
};
#endif

EFIAPI SmcOptimizeIPMICallback(
	IN EFI_PEI_SERVICES 		 **PeiServices,
	IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
	IN VOID 					 *Ppi)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	INTEL_SETUP_DATA	SetupData;

	UINT8 ipmi_return[32];
	UINT8 ipmi_return_size = 32;
	UINT32 VrmSettingsCount = 0, i = 0, iRetry = 5;
	VRM_ARRAY* vrmsettings = NULL;
	UINT8 pause_bmc[2] = {0xdf, 0x00};
	UINT8 resume_bmc[2] = {0xdf, 0x00};
	
		
	DEBUG ((-1, "SmcOptimizePeiInit.\n"));

	Status = GetEntireConfig (&SetupData);
	ASSERT_EFI_ERROR (Status);

#if X11DPUZ_SUPPORT
	if ( SetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS == SMC_OPTIMIZE_CPU2006 && 
		SetupData.SocketConfig.PowerManagementConfig.CpuPm == 2 /*custom*/&&
		SetupData.SocketConfig.PowerManagementConfig.PwrPerfTuning == 1 /*BIOS*/ ) {
		VrmSettingsCount = sizeof(gX11DPUZMaxVrmSettings)/sizeof(VRM_ARRAY);
		vrmsettings = gX11DPUZMaxVrmSettings;
	}
	else {
		VrmSettingsCount = sizeof(gX11DPUZOrgVrmSettings)/sizeof(VRM_ARRAY);
		vrmsettings = gX11DPUZOrgVrmSettings;
	}
#endif

#if X11DPU_SUPPORT
	if ( SetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS == SMC_OPTIMIZE_CPU2006 && 
		SetupData.SocketConfig.PowerManagementConfig.CpuPm == 2 /*custom*/&&
		SetupData.SocketConfig.PowerManagementConfig.PwrPerfTuning == 1 /*BIOS*/ ) {
		VrmSettingsCount = sizeof(gX11DPUMaxVrmSettings)/sizeof(VRM_ARRAY);
		vrmsettings = gX11DPUMaxVrmSettings;
	}
	else {
		VrmSettingsCount = sizeof(gX11DPUOrgVrmSettings)/sizeof(VRM_ARRAY);
		vrmsettings = gX11DPUOrgVrmSettings;
	}
#endif

#if X11DDW_SUPPORT
	if ( SetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS == SMC_OPTIMIZE_SPECPOWER && 
		SetupData.SocketConfig.PowerManagementConfig.CpuPm == 2 /*custom*/&&
		SetupData.SocketConfig.PowerManagementConfig.PwrPerfTuning == 1 /*BIOS*/ ) {
		VrmSettingsCount = sizeof(gX11DDWMaxVrmSettings)/sizeof(VRM_ARRAY);
		vrmsettings = gX11DDWMaxVrmSettings;
	}
	else {
		VrmSettingsCount = sizeof(gX11DDWOrgVrmSettings)/sizeof(VRM_ARRAY);
		vrmsettings = gX11DDWOrgVrmSettings;
	}
#endif

	// Set VRM values
	DEBUG((-1, "vrm setting count=%d\n", VrmSettingsCount));
	Status = EFI_SUCCESS;
	for(i=0; i<VrmSettingsCount; i++) {
		iRetry = 5;
		while(1) {
			DEBUG((-1, "vrmsettings[i].data=0x%x\n", vrmsettings[i].data));
			DEBUG((-1, "vrmsettings[i].size=0x%x\n", vrmsettings[i].size));
			DEBUG((-1, "0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", vrmsettings[i].data[0], vrmsettings[i].data[1], vrmsettings[i].data[2], vrmsettings[i].data[3], vrmsettings[i].data[4],vrmsettings[i].data[5]));
			Status = SMC_IPMICmd(0x06, 0x00, 0x52, vrmsettings[i].data, vrmsettings[i].size, ipmi_return, &ipmi_return_size);
			DEBUG((-1, "Status=%r\n", Status));
			if ( !EFI_ERROR(Status) || iRetry == 0 ) {
				break;
			}
			iRetry--;
		}
	}
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SmcOptimizePeiInit
//
// Description : Init Project at PEI phase
//
// Parameters:  FfsHeader   Pointer to the FFS file header
//              PeiServices Pointer to the PEI services table
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
EFIAPI
SmcOptimizePeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices )
{
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_PEI_NOTIFY_DESCRIPTOR *pIPMICallback = NULL;
	INTEL_SETUP_DATA	SetupData;
	
	DEBUG ((-1, "SmcOptimizePeiInit.\n"));

	Status = GetEntireConfig (&SetupData);
	ASSERT_EFI_ERROR (Status);

	DEBUG ((-1, "SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorVmxEnable = 0x%x.\n", SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorVmxEnable));
	DEBUG ((-1, "SetupData.SocketConfig.MemoryConfig.ImcInterleaving = 0x%x\n", SetupData.SocketConfig.MemoryConfig.ImcInterleaving));
	DEBUG ((-1, "SetupData.SocketConfig.CsiConfig.LLCDeadLineAlloc = 0x%x\n", SetupData.SocketConfig.CsiConfig.LLCDeadLineAlloc));
	DEBUG ((-1, "SetupData.SocketConfig.CsiConfig.QpiLinkSpeed = 0x%x\n", SetupData.SocketConfig.CsiConfig.QpiLinkSpeed));
	DEBUG ((-1, "SetupData.SocketConfig.CsiConfig.SncEn = 0x%x\n", SetupData.SocketConfig.CsiConfig.SncEn));
	DEBUG ((-1, "SetupData.SocketConfig.CsiConfig.StaleAtoSOptEn = 0x%x\n", SetupData.SocketConfig.CsiConfig.StaleAtoSOptEn));	
	DEBUG ((-1, "SetupData.SocketConfig.PowerManagementConfig.ProcessorHWPMEnable = 0x%x\n", SetupData.SocketConfig.PowerManagementConfig.ProcessorHWPMEnable));

	Status = (**PeiServices).AllocatePool(
			       PeiServices,
			       sizeof (EFI_PEI_NOTIFY_DESCRIPTOR),
			       &pIPMICallback);
	if ( !EFI_ERROR(Status) ) {
		pIPMICallback->Flags = EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
							| EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
		pIPMICallback->Guid   = &gEfiPeiIpmiTransportPpiGuid;
		pIPMICallback->Notify = SmcOptimizeIPMICallback;
		(*PeiServices)->NotifyPpi( PeiServices, pIPMICallback);
	}

	DEBUG ((-1, "Exit SmcOptimizePeiInit.\n"));

	return EFI_SUCCESS;
}
