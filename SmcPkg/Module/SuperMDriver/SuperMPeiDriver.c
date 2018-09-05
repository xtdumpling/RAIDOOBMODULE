//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//      Bug Fixed:  Add PCIE bifurcation setting in PEI.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/06/2016
//
//  Rev. 1.00
//      Bug Fixed:  Add board level override KTI setting
//      Reason:     
//      Auditor:    Salmon Chen
//      Date:       Mar/14/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/18/2014
//
//****************************************************************************
//****************************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		SuperMPeiDriver.c
//
// Description:
//  This file contains code for SuperMDriver PEI functions.  The
//  functions call Project PEI driver to get board level information.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
//#include <Efi.h>
//#include <Pei.h>
//#include <Hob.h>
#include <Setup.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
//#include <AmiPeiLib.h>
//#include <AmiCspLib.h>
#include "Include/SuperMPeiDriver.h"
#include "SuperMHob.h"
#include "SmcLib.h"

extern EFI_GUID gSuperMPeiPrococolGuid;
extern EFI_GUID gProjectOwnPeiProtocolGuid;

#define EFI_DEADLOOP()    { volatile int __iii; __iii = 1; while (__iii); } 

#define PEI_ASSERT(PeiServices,Condition) if(!(Condition)) { \
	DEBUG((EFI_D_INFO, "ASSERT in %s on %i: %s\n",__FILE__, __LINE__));\
	EFI_DEADLOOP() \
	}
#define ASSERT_PEI_ERROR(PeiServices,Status)  PEI_ASSERT(PeiServices,!EFI_ERROR(Status))

//#define PCICMD  0x04
//#define MBL     0x20
//#define BNUM    0x18
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : PeiGetSuperMBoardInfoCode
//
// Description : 1. Get data from PDR and save data into SMC_PDR_DATA_HOB
//               2.SuperM Pei driver use PROJECT_PEI_DRIVER_PROTOCOL PPI to get board information
//                 Board.
//				 3.Copy board information to SMCHOBINFO
//
//
// Parameters  :  IN EFI_PEI_SERVICES **PeiServices - Pointer to the PEI Core data Structure
// 				  IN SUPERM_PEI_DRIVER_PROTOCOL *This - Pointer to an instance of the SUPERM_PEI_DRIVER_PROTOCOL
//                IN OUT SUPERMBOARDINFO PeiSuperMBoardInfoData - Point to data structure of SUPERMBOARDINFO
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
PeiGetSuperMBoardInfoCode(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN SUPERM_PEI_DRIVER_PROTOCOL *This,
    OUT SUPERMBOARDINFO *PeiSuperMBoardInfoData
)
{
    EFI_STATUS	Status;
    PROJECT_PEI_DRIVER_PROTOCOL	*mProjectPeiInterface = NULL;
    // PCB Version Table
    UINT32	PCB_Version = 0x100;
    SMCHOBINFO	*pSHob;

    DEBUG((EFI_D_INFO, "PeiGetSuperMBoardInfoCode.\n"));

    //
    // Locate Project PPI
    //
    DEBUG((EFI_D_INFO, "Locate PPI Project Protocol\n"));
    Status = (*PeiServices)->LocatePpi(
		    PeiServices,
		    &gProjectOwnPeiProtocolGuid,
		    0,
		    NULL,
		    &mProjectPeiInterface);

    if(EFI_ERROR(Status)){
	    mProjectPeiInterface = NULL;
	    return Status;
    }
    
    (*PeiServices)->SetMem( PeiSuperMBoardInfoData, sizeof(SUPERMBOARDINFO), 0 );

    PeiSuperMBoardInfoData->MBSVID  = (SMC_SSID >> 16) & 0xFFFF;
    DEBUG((-1, "MV SVID : 0x%x\n", PeiSuperMBoardInfoData->MBSVID));

    PeiSuperMBoardInfoData->SMCCheckTPMPresent = CheckTPMPresent ();
    DEBUG((-1, "TPM Present : %d\n", PeiSuperMBoardInfoData->SMCCheckTPMPresent));

// BMC command 20/21 data
// SystemConfigInfo[0~1] = SSID
// SystemConfigInfo[2] = Sensor type. Always 7 in current design
// SystemConfigInfo[3] =Bit 4, For CPU 0/CPU1 revise
//                                     Bit 5, 1 for 83795-HF, 0 for 83795-F
//                                     Bit 6, 10G lan support
//                                     Bit 7, CPU temperature source.0 for H/W monitor,1 for ME
// SystemConfigInfo[4~9] = Onbard Lan1 MAC address
#if IPMI_SUPPORT
    PeiSuperMBoardInfoData->HardwareInfo[0] = (UINT8)((SMC_SSID >> 16) & 0xFF);
    PeiSuperMBoardInfoData->HardwareInfo[1] = (UINT8)((SMC_SSID >> 24) & 0xFF);
    PeiSuperMBoardInfoData->HardwareInfo[2] = (UINT8)(SystemConfigInfo_2_3 & 0xFF);
    PeiSuperMBoardInfoData->HardwareInfo[3] = (UINT8)((SystemConfigInfo_2_3 >> 8) & 0xFF);
    if(mProjectPeiInterface != NULL){
	if(mProjectPeiInterface->CheckAdjustVID != NULL){
	    PeiSuperMBoardInfoData->GPU_AdjustVID_Support = mProjectPeiInterface->CheckAdjustVID(PeiServices, mProjectPeiInterface);
	    DEBUG((-1, "GPU_AdjustVID_Support : %d\n", PeiSuperMBoardInfoData->GPU_AdjustVID_Support));
        }
        if(mProjectPeiInterface->CheckBMCPresent != NULL){
            PeiSuperMBoardInfoData->BMC_Present = mProjectPeiInterface->CheckBMCPresent(PeiServices, mProjectPeiInterface);
            DEBUG((-1, "BMC Present : %d\n", PeiSuperMBoardInfoData->BMC_Present));
        }
        // Get system information and all Lan Mac address
        if(mProjectPeiInterface->GetProjectHardwareInfo != NULL){
            Status = mProjectPeiInterface->GetProjectHardwareInfo(PeiServices, mProjectPeiInterface, PeiSuperMBoardInfoData->HardwareInfo, PeiSuperMBoardInfoData->LanMacInfo);
            // prepare CMD 0x99 data
            if(mProjectPeiInterface->GetProjectSystemConfiguration != NULL)
        	Status = mProjectPeiInterface->GetProjectSystemConfiguration(PeiServices, mProjectPeiInterface, PeiSuperMBoardInfoData->SystemConfiguration);
            
#if defined DEBUG_MODE && DEBUG_MODE == 0x01
            {
                UINT32	i = 0;
                DEBUG((-1, "HardWareInfoData : \n"));
                for(i = 0; i < sizeof (PeiSuperMBoardInfoData->HardwareInfo); i++)
                    DEBUG((-1, "%02x ", (UINT8)(*(PeiSuperMBoardInfoData->HardwareInfo + i))));

                DEBUG((-1, "\nSystemConfiguration : \n"));
                for(i = 0; i < sizeof(PeiSuperMBoardInfoData->HardwareInfo); i++)
                    DEBUG((-1, "%02x ", (UINT8)(*(PeiSuperMBoardInfoData->SystemConfiguration + i))));
                
                DEBUG((-1, "\nLanMacInfoData : \n"));
                for (i = 0; i < sizeof(PeiSuperMBoardInfoData->LanMacInfo); i++)
                    DEBUG((-1, "%02x ", (UINT8)(*(PeiSuperMBoardInfoData->LanMacInfo + i))));
                DEBUG((-1, "\n"));
            }
#endif
        }
    }
#endif

    if(mProjectPeiInterface->GetProjectBID != NULL){
        PeiSuperMBoardInfoData->PCB_Revision = mProjectPeiInterface->GetProjectBID(PeiServices, mProjectPeiInterface);
        DEBUG((-1, "PCB revision : %d\n", PeiSuperMBoardInfoData->PCB_Revision));
    }

	// Create HOB Data sample code
    Status = (**PeiServices).CreateHob(
		    PeiServices,
		    EFI_HOB_TYPE_GUID_EXTENSION,
		    sizeof(SMCHOBINFO),
		    &pSHob);

    if(!EFI_ERROR(Status)){
	DEBUG((-1, "Create SMCHOBINFO.\n"));
	pSHob->EfiHobGuidType.Name = gSmcBoardInfoHobGuid;
	pSHob->MBSVID = (SMC_SSID >> 16) & 0xFFFF;
	pSHob->SMCCheckTPMPresent = PeiSuperMBoardInfoData->SMCCheckTPMPresent;
	pSHob->BMC_Present = PeiSuperMBoardInfoData->BMC_Present;
	pSHob->PCB_Revision = PeiSuperMBoardInfoData->PCB_Revision;
	pSHob->GPU_AdjustVID_Support = PeiSuperMBoardInfoData->GPU_AdjustVID_Support;
	(*PeiServices)->CopyMem(pSHob->HardwareInfo, PeiSuperMBoardInfoData->HardwareInfo, sizeof(pSHob->HardwareInfo));
    }

    DEBUG((-1, "Exit PeiGetSuperMBoardInfoCode.\n"));

    return EFI_SUCCESS;

}

/*-----------------12/23/2014 2:24PM----------------
 * Coding for every platform
 * --------------------------------------------------*/
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :
//
// Description : Get board QPI EParam setting
//
// Parameters  :  IN EFI_PEI_SERVICES **PeiServices - Pointer to the PEI Core data Structure
// 				  IN SUPERM_PEI_DRIVER_PROTOCOL *This - Pointer to an instance of the SUPERM_PEI_DRIVER_PROTOCOL
//                OUT UINT32 *SizeOfTable - Size of HSX_ALL_LANES_EPARAM_LINK_INFO
//                OUT UINT32 *per_lane_SizeOfTable - Size of HSX_PER_LANE_EPARAM_LINK_INFO
//                OUT HSX_ALL_LANES_EPARAM_LINK_INFO **ptr - Pointer to HSX_ALL_LANES_EPARAM_LINK_INFO data structure address
//                OUT HSX_PER_LANE_EPARAM_LINK_INFO  **per_lane_ptr - Pointer to HSX_PER_LANE_EPARAM_LINK_INFO data structure address
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT32
PeiGetProjectEParam(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PROJECT_PEI_DRIVER_PROTOCOL *This,
    OUT UINT32 *SizeOfTable,
    OUT ALL_LANES_EPARAM_LINK_INFO **Ptr
)
{
    EFI_STATUS	Status;
    PROJECT_PEI_DRIVER_PROTOCOL	*mProjectPeiInterface = NULL;
    UINT32 Result = TRUE;

    Status = (*PeiServices)->LocatePpi(
		    PeiServices,
		    &gProjectOwnPeiProtocolGuid,
		    0,
		    NULL,
		    &mProjectPeiInterface);

    if(EFI_ERROR(Status)){
	    mProjectPeiInterface = NULL;
	    return FALSE;
    }

    if(mProjectPeiInterface->GetProjectEParam != NULL)
        Result = mProjectPeiInterface->GetProjectEParam(PeiServices,mProjectPeiInterface, SizeOfTable, Ptr);

    return Result;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : GetSuperMPchInfo
//
// Description : Get board PCH Data
//
// Parameters  :  IN OUT UINT8 PLATFORM_PCH_DATA[10]
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
GetSuperMPchInfo(
    IN EFI_PEI_SERVICES **PeiServices,
    IN SUPERM_PEI_DRIVER_PROTOCOL *This,
    IN OUT UINT8 PLATFORM_PCH_DATA[10]
)
{
    EFI_STATUS	Status;
    PROJECT_PEI_DRIVER_PROTOCOL	*mProjectPeiInterface = NULL;

    DEBUG((EFI_D_INFO, "GetSuperMPchInfo Entry.\n"));

    Status = (*PeiServices)->LocatePpi(
		    PeiServices,
		    &gProjectOwnPeiProtocolGuid,
		    0,
		    NULL,
		    &mProjectPeiInterface);

    if(EFI_ERROR(Status)){
	    mProjectPeiInterface = NULL;
	    return FALSE;
    }

    if(mProjectPeiInterface->GetProjectPchData != NULL)
	mProjectPeiInterface->GetProjectPchData(PeiServices,mProjectPeiInterface,PLATFORM_PCH_DATA);
    DEBUG((EFI_D_INFO, "GetSuperMPchInfo Exit.\n"));
    return EFI_SUCCESS;
}

EFI_STATUS
GetSuperMPcieBifurcation(
    IN EFI_PEI_SERVICES **PeiServices,
    IN OUT	IIO_CONFIG	*SetupData
)
{
    EFI_STATUS	Status;
    PROJECT_PEI_DRIVER_PROTOCOL	*mProjectPeiInterface = NULL;

    DEBUG((-1, "GetSuperMPcieBifurcation Entry.\n"));

    Status = (*PeiServices)->LocatePpi(
		    PeiServices,
		    &gProjectOwnPeiProtocolGuid,
		    0,
		    NULL,
		    &mProjectPeiInterface);

    if(EFI_ERROR(Status)){
	    mProjectPeiInterface = NULL;
	    return FALSE;
    }

    if(mProjectPeiInterface->GetProjectPciEBifurcate != NULL)
    	mProjectPeiInterface->GetProjectPciEBifurcate(PeiServices, SetupData);
    DEBUG((-1, "GetSuperMPcieBifurcation End.\n"));

    return EFI_SUCCESS;
}

SUPERM_PEI_DRIVER_PROTOCOL SuperMInterface =
{
	PeiGetSuperMBoardInfoCode,
	PeiGetProjectEParam,
	GetSuperMPchInfo,
	GetSuperMPcieBifurcation
};

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SuperMPeiDriverInit
//
// Description : Init SuperMPeiDriverInit
//
// Parameters:  FfsHeader   Pointer to the FFS file header
//              PeiServices Pointer to the PEI services table
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI SuperMPeiDriverInit(
    IN EFI_FFS_FILE_HEADER       *FfsHeader,
    IN EFI_PEI_SERVICES          **PeiServices)
{
    EFI_STATUS Status;
    EFI_PEI_PPI_DESCRIPTOR *mPpiList = NULL;
    SUPERMBOARDINFO PeiSuperMBoardInfoData;

    DEBUG((EFI_D_INFO, "SuperMPeiDriverInit.\n"));

    Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (EFI_PEI_PPI_DESCRIPTOR), &mPpiList);
    ASSERT_PEI_ERROR (PeiServices, Status);

    mPpiList->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    mPpiList->Guid = &gSuperMPeiPrococolGuid;
    mPpiList->Ppi = &SuperMInterface;

    Status = (**PeiServices).InstallPpi(PeiServices, mPpiList);
    if (EFI_ERROR(Status)){
	DEBUG((EFI_D_INFO, "SuperMPeiDriverInit fail.\n"));
	return Status;
    }

    //
    // Run PeiGetSuperMBoardInfoCode to create HOB.
    //
    PeiGetSuperMBoardInfoCode(PeiServices, &SuperMInterface, &PeiSuperMBoardInfoData);

    DEBUG((EFI_D_INFO, "Exit SuperMPeiDriverInit.\n"));
    return EFI_SUCCESS;
}

//****************************************************************************
