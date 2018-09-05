//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.03
//       Bug Fix:  Fixed up power button is not reported in ACPI table when set "4 Seconds Override".
//       Reason:
//       Auditor:  Leon Xu
//       Date:     Feb/22/2017
//
//  Rev. 1.02
//       Bug Fix:  Add OS AER item.
//       Reason:
//       Auditor:  Kasber Chen
//       Date:     Dec/20/2016
//
//  Rev. 1.01
//       Bug Fix:  Fixed up power button override is not work in the OS.
//       Reason:
//       Auditor:  Jacker Yeh (Refer from Grantley)
//       Date:     Oct/27/2016
//
//  Rev. 1.00
//       Bug fixed:
//       Reason:      Fixed "Install Windows 7 USB support" function fail with "Quiet boot" disable. 
//       Auditor:     Sunny Yang
//       Date:        Sep/07/2016
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file 
    Main ACPI Driver File. It has ACPI Driver entry point,
    ACPISupport Protocol and ACPITable Protocol.

**/
//**********************************************************************

#include <Library/AmiAcpiCpuLib.h>
#include <AmiDxeLib.h>
#include <Token.h>
#include <AcpiRes.h>
#include "AcpiCore.h"
#include "Acpi50.h"
#include <Protocol/Cpu.h>
#if DSDT_BUILD
#include <Library/AmiSdlLib.h>
#endif
#include <AcpiOemElinks.h>
#include <Fid.h>
#if GenericSio_SUPPORT
#include <AmiGenericSio.h>
#endif
#if ATAD_SUPPORT == 1
#include "AtadSmi.h"
#endif
#include<IndustryStandard/Acpi60.h>
#include <Library/PcdLib.h>
/**
    Structure to store Aml update information, that will be used on ready to boot

 
**/
typedef struct _ACPI_AML_UPD_INFO {
	UINT64			Dsdt1Addr;///< Address of DSDT for ACPI v1.b
	UINT64			Dsdt2Addr;///< Address of DSDT for ACPI v2+
	ACPI_AML_RES	PciGap[3];
	BOOLEAN 		SS1;///< Value to update _S1 aml object with
	BOOLEAN 		SS2;///< Value to update _S2 aml object with
	BOOLEAN 		SS3;///< Value to update _S3 aml object with
	BOOLEAN 		SS4;///< Value to update _S4 aml object with
	UINT32			RomStart;
	UINT32			TopOfMemory;///< Value to update TOPM aml object with
#if GenericSio_SUPPORT
	SIO_DEV_STATUS	SioDevStatusVar;///< Variable with status of SIO devices, which will be used to update aml
#endif
#if SMCPKG_SUPPORT
	BOOLEAN                 W7KF;
	UINT8                   PEER;
#endif  //SMCPKG_SUPPORT
} ACPI_AML_UPD_INFO;

//--------------------------------------
//Some Global vars
UINTN 					gDsdtPages = 0;
EFI_GUID 				gSetupGuid = SETUP_GUID;
extern ACPI_DB          gAcpiData;
FACS_20                 *gFacs, *gxFacs;
extern RSDT_PTR_20      *gRsdtPtrStr;
UINTN                   gAcpiTblPages = 0;
ACPI_AML_UPD_INFO       *gAuiGlob = NULL;
UINT16                  gAcpiIaBootArch=0xFFFF;
EFI_GUID                gAcpi11TAbleGuid = EFI_ACPI_11_TABLE_GUID;
EFI_GUID                gAcpi20TAbleGuid = EFI_ACPI_20_TABLE_GUID;
extern UINT8            gForceAcpi1, gPublishedOnReadyToBoot;
#if FPDT_BUILD
EFI_FPDT_STRUCTURE      *FpdtVar = NULL;
#endif
#if ATAD_SUPPORT == 1
    VOID  *AtadBuffPtr = NULL;
#endif
//Here goes Interrupt Source Override MADT entry parameter table
//Generated From Information on IRQ_XX_OVERRIDE_ENABLE SDL tokens
ISO_PARAMETER_TABLE IsoTbl[]=
{
//UINT8 PicIrq; UINT8   Flags;                              UINT16  ApicInt
#if IRQ_00_OVERRIDE_ENABLE == 1
    {   0x00,       (IRQ_00_TRIGGER_MODE<<2)|IRQ_00_POLARITY,   IRQ_00_APIC_INT },
#else
    {   0xFF,       0,                                          0               },
#endif
#if IRQ_01_OVERRIDE_ENABLE == 1
    {   0x01,       (IRQ_01_TRIGGER_MODE<<2)|IRQ_01_POLARITY,   IRQ_01_APIC_INT },
#else
    {   0xFF,       0,                                          1               },
#endif
//just dummy entry instead of IRQ2 to keep array consistent
    {   0xFF,       0,                                          2               },
#if IRQ_03_OVERRIDE_ENABLE == 1
    {   0x03,       (IRQ_03_TRIGGER_MODE<<2)|IRQ_03_POLARITY,   IRQ_03_APIC_INT },
#else
    {   0xFF,       0,                                          3               },
#endif
#if IRQ_04_OVERRIDE_ENABLE == 1
    {   0x04,       (IRQ_04_TRIGGER_MODE<<2)|IRQ_04_POLARITY,   IRQ_04_APIC_INT },
#else
    {   0xFF,       0,                                          4               },
#endif
#if IRQ_05_OVERRIDE_ENABLE == 1
    {   0x05,       (IRQ_05_TRIGGER_MODE<<2)|IRQ_05_POLARITY,   IRQ_05_APIC_INT },
#else
    {   0xFF,       0,                                          5               },
#endif
#if IRQ_06_OVERRIDE_ENABLE == 1
    {   0x06,       (IRQ_06_TRIGGER_MODE<<2)|IRQ_06_POLARITY,   IRQ_06_APIC_INT },
#else
    {   0xFF,       0,                                          6               },
#endif
#if IRQ_07_OVERRIDE_ENABLE == 1
    {   0x07,       (IRQ_07_TRIGGER_MODE<<2)|IRQ_07_POLARITY,   IRQ_07_APIC_INT },
#else
    {   0xFF,       0,                                          7               },
#endif
    
#if IRQ_08_OVERRIDE_ENABLE == 1
    {   0x08,       (IRQ_08_TRIGGER_MODE<<2)|IRQ_08_POLARITY,   IRQ_08_APIC_INT },
#else
    {   0xFF,       0,                                          8               },
#endif
#if IRQ_09_OVERRIDE_ENABLE == 1
    {   0x09,       (IRQ_09_TRIGGER_MODE<<2)|IRQ_09_POLARITY,   IRQ_09_APIC_INT },
#else
    {   0xFF,       0,                                          9               },
#endif
#if IRQ_10_OVERRIDE_ENABLE == 1
    {   0x0A,       (IRQ_10_TRIGGER_MODE<<2)|IRQ_10_POLARITY,   IRQ_10_APIC_INT },
#else
    {   0xFF,       0,                                          10              },
#endif
#if IRQ_11_OVERRIDE_ENABLE == 1
    {   0x0B,       (IRQ_11_TRIGGER_MODE<<2)|IRQ_11_POLARITY,   IRQ_11_APIC_INT },
#else
    {   0xFF,       0,                                          11              },
#endif
#if IRQ_12_OVERRIDE_ENABLE == 1
    {   0x0C,       (IRQ_12_TRIGGER_MODE<<2)|IRQ_12_POLARITY,   IRQ_12_APIC_INT },
#else
    {   0xFF,       0,                                          12              },
#endif
#if IRQ_13_OVERRIDE_ENABLE == 1
    {   0x0D,       (IRQ_13_TRIGGER_MODE<<2)|IRQ_13_POLARITY,   IRQ_13_APIC_INT },
#else
    {   0xFF,       0,                                          13              },
#endif
#if IRQ_14_OVERRIDE_ENABLE == 1
    {   0x0E,       (IRQ_14_TRIGGER_MODE<<2)|IRQ_14_POLARITY,   IRQ_14_APIC_INT },
#else
    {   0xFF,       0,                                         14               },
#endif
#if IRQ_15_OVERRIDE_ENABLE == 1
    {   0x0F,       (IRQ_15_TRIGGER_MODE<<2)|IRQ_15_POLARITY,   IRQ_15_APIC_INT },
#else
    {   0xFF,       0,                                          15              },
#endif
};

static UINTN    IsoCnt=sizeof(IsoTbl)/sizeof(ISO_PARAMETER_TABLE);


UINT8    ACPI_OEM_ID[6]     = ACPI_OEM_ID_MAK;     
UINT8    ACPI_OEM_TBL_ID[8] = ACPI_OEM_TBL_ID_MAK; 

#if defined(OemActivation_SUPPORT) && (OemActivation_SUPPORT == 1)
#define EFI_OA3_MSDM_VARIABLE   L"OA3MSDMvariable"
/**
 	 Internal structure used by OEM Activation process
 
**/
typedef struct _EFI_OA3_MSDM_STRUCTURE {
    EFI_PHYSICAL_ADDRESS  XsdtAddress;///< Address of XSDT table.
    EFI_PHYSICAL_ADDRESS  MsdmAddress;///< Address of MSDM table.
    EFI_PHYSICAL_ADDRESS  ProductKeyAddress;///< Address of a product key.
} EFI_OA3_MSDM_STRUCTURE;

BOOLEAN gOA3Variable = FALSE;
EFI_OA3_MSDM_STRUCTURE gMsdmVariable;
#endif

extern EFI_GUID gAmiGlobalVariableGuid;

EFI_STATUS OemAcpiSetPlatformId(IN OUT ACPI_HDR *AcpiHdr);


/**
    This function creates ACPI table v 2.0+ header with specified signature

         
    @param TblSig ACPI table signature
    @param HdrPtr Pointer to memory, where the header should be placed

          
    @retval VOID

**/

void PrepareHdr20(UINT32 TblSig, ACPI_HDR *HeaderPtr, UINTN Vers)
{
    UINTN i;
    EFI_STATUS  Status;
    
    if (HeaderPtr==NULL) return;
    
    HeaderPtr->Signature=TblSig;
    
    //Check what Revision# header needs depends on TABLE we're building
    switch (TblSig)
    {
        case RSDT_SIG:
            HeaderPtr->Revision=ACPI_REV1;
            HeaderPtr->CreatorRev=CREATOR_REV_MS+1;
            break;
            
        case XSDT_SIG:
            HeaderPtr->Revision=ACPI_REV1;
            HeaderPtr->CreatorRev=CREATOR_REV_MS+2;
            break;
            
        case FACP_SIG:
            HeaderPtr->Revision=ACPI_REV3;
            HeaderPtr->CreatorRev=CREATOR_REV_MS+3;
            
            if (Vers > 2)
            {
            
                HeaderPtr->Revision=ACPI_REV4;
                HeaderPtr->CreatorRev++;
            }
            
            break;
        case APIC_SIG:
            HeaderPtr->Revision=ACPI_REV1;
            HeaderPtr->CreatorRev=CREATOR_REV_MS+4;
            
            if (Vers > 2)
            {
            
                HeaderPtr->Revision=ACPI_REV2;
                HeaderPtr->CreatorRev++;
            }
            
            if (Vers > 3)HeaderPtr->Revision=ACPI_REV3;
            
            break;
        case SBST_SIG:
            HeaderPtr->Revision=ACPI_REV1;
            break;
        case SPCR_SIG:
            HeaderPtr->Revision=ACPI_REV1;
            break;
        case ECDT_SIG:
            HeaderPtr->Revision=ACPI_REV1;
            break;
            //case DSDT_SIG:
            //HeaderPtr->CreatorRev=CREATOR_REV_MS+5;
            //HeaderPtr->Revision=ACPI_REV2;
            //break;
    }
    
    //instead of puting fixed revision number
    //HeaderPtr->Revision=ACPI_REV2;
    
    //Dont' touch Creator ID and Creator Revision;
    if (TblSig != DSDT_SIG)
    {
        if (TblSig == RSDT_SIG)
            HeaderPtr->CreatorId = CREATOR_ID_MS;
        else
            HeaderPtr->CreatorId = CREATOR_ID_AMI;
            
        HeaderPtr->CreatorRev = CREATOR_REV_MS;
        HeaderPtr->OemRev = ACPI_OEM_REV;
    }

    //Get the platform specific OEM_IDs
    Status=OemAcpiSetPlatformId(HeaderPtr);
    
    //if platform does not support OEM_ID overwrite
    if (EFI_ERROR(Status))
    {
        for (i=0; i<6; i++) HeaderPtr->OemId[i]=ACPI_OEM_ID[i];
        
        for (i=0; i<8; i++) HeaderPtr->OemTblId[i]=ACPI_OEM_TBL_ID[i];
    }

}  //PrepareHdr20

/**
    This function creates ACPI V1 table header with specified signature

         
    @param TblSig ACPI table signature
    @param HdrPtr Pointer to memory, where the header should be placed

          
    @retval VOID

    @note  
  Depends on type of memory provided, ACPI table header can be in 32 or 64 bit
  format

**/

void PrepareHdr1(UINT32 TblSig, ACPI_HDR* HdrPtr)
{
    UINTN i;
    EFI_STATUS  Status;
    
    if (HdrPtr==NULL) return;
    
    HdrPtr->Signature = TblSig;
    HdrPtr->Revision = ACPI_REV1;
    
    //Dont' touch Creator ID and Creator Revision;
    if (TblSig != DSDT_SIG)
    {
        if (TblSig == RSDT_SIG)
            HdrPtr->CreatorId = CREATOR_ID_MS;
        else
            HdrPtr->CreatorId = CREATOR_ID_AMI;
            
        HdrPtr->CreatorRev = CREATOR_REV_MS;
        HdrPtr->OemRev = ACPI_OEM_REV;
    }

    //Get the platform specific OEM_IDs
    Status=OemAcpiSetPlatformId(HdrPtr);
    
    //if platform does not support OEM_ID overwrite
    if (EFI_ERROR(Status))
    {
        for (i=0; i<6; i++) HdrPtr->OemId[i]=ACPI_OEM_ID[i];
        
        for (i=0; i<8; i++) HdrPtr->OemTblId[i]=ACPI_OEM_TBL_ID[i];
    }

}//PrepareHdr1
#if FPDT_BUILD
/**
    This function will be called to locate CPU Protocol
    and get Cpu frequency.

    @param Event signalled event
    @param Context calling context

    @retval EFI_STATUS

**/
VOID GetCpuNanoFreq (
            IN EFI_EVENT    Event,
            IN VOID         *Context
)
{
	UINT64                    CurrentTicker, TimerPeriod;
	EFI_STATUS                  Status;
    EFI_CPU_ARCH_PROTOCOL       *Cpu;
    EFI_EVENT   EvtCpu;
    VOID         *RegCpu =  NULL;

	Status = pBS->LocateProtocol (
              &gEfiCpuArchProtocolGuid,
              NULL,
              (VOID **)&Cpu
              );
	if (EFI_ERROR (Status)) {
		TRACE((-1,"Acpi BuildFPDT: No Cpu Protocol was found.\n"));
		 Status = RegisterProtocolCallback ( &gEfiCpuArchProtocolGuid,
                                            GetCpuNanoFreq,
                                            NULL,
                                            &EvtCpu,
                                            &RegCpu );
		return ;
	}

	Status = Cpu->GetTimerValue (Cpu, 0, &CurrentTicker, &TimerPeriod);
	if (!EFI_ERROR (Status)) {
		FpdtVar->NanoFreq = TimerPeriod;
		TRACE((-1,"Acpi BuildFPDT:  Cpu Protocol was found.\n"));
		pBS->CloseEvent(Event);
		}
        return ;
}

/**
    This function allocates memory for and fills FPDT struscure. It also
    creates Variable with the addres od S3 and Normal Boot performance srtuctures
    to be filled later. 
         
    @param TablVer Version of FACP table
        IN OUT **ACPI_HDR TablPtr - Pointer to memory, where the FACP table will resides.
        Filled by this procedure
    @retval 
  EFI_STATUS:
  EFI_OUT_OF_RESOURCES - Memory for the table could not be allocated
  EFI_SUCCESS - Table was successfully build

**/

EFI_STATUS BuildFPDT (IN UINTN TablVer, OUT ACPI_HDR **TablPtr)

{
    UINT8                       *TempPtr;
    PERF_TAB_HEADER             *S3PerfRecHdr, *BasBootPerfRecHdr;
    BASIC_S3_RESUME_PERF_REC    *S3PerfRec;
    BASIC_BOOT_PERF_REC         *BasBootPerfRec;
    UINTN                       RecordsSize;
    FPDT_50                     *FPDT;
    EFI_STATUS                  Status;
    EFI_FPDT_STRUCTURE          *OldFpdtVarAddress=NULL;
    UINTN                       VarSize = sizeof (UINT32);

//-----------------------------

    if (TablVer<1 || TablVer>4) return EFI_INVALID_PARAMETER;
    RecordsSize = (sizeof(EFI_FPDT_STRUCTURE) + sizeof(PERF_TAB_HEADER)*2 + 
                        sizeof(BASIC_S3_RESUME_PERF_REC)+
                        sizeof(BASIC_BOOT_PERF_REC));
    Status = pBS->AllocatePool(EfiRuntimeServicesData, RecordsSize, (VOID **)&TempPtr);
    if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
#ifdef EFIx64
    if (((UINT64)TempPtr) & 0xffffffff00000000)
    {
        EFI_PHYSICAL_ADDRESS Memory = 0x00000000ffffffff;
        Status = pBS->FreePool(TempPtr);
        ASSERT_EFI_ERROR(Status);
        Status = pBS->AllocatePages(AllocateMaxAddress, EfiRuntimeServicesData, 1, &Memory);
        ASSERT_EFI_ERROR(Status);
        
        if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
        TempPtr = (UINT8*)Memory;
    }
#endif
    pBS->SetMem(TempPtr, RecordsSize, 0);

    *TablPtr = MallocZ (sizeof(FPDT_50));

    if ((*TablPtr)==NULL)
    {
        ASSERT(*TablPtr);
        return EFI_OUT_OF_RESOURCES;
    }
    FpdtVar = (EFI_FPDT_STRUCTURE*) TempPtr;
    TempPtr += sizeof(EFI_FPDT_STRUCTURE);
    S3PerfRecHdr = (PERF_TAB_HEADER*) TempPtr;
    TempPtr += sizeof(PERF_TAB_HEADER);
    S3PerfRec = (BASIC_S3_RESUME_PERF_REC*)TempPtr;
    TempPtr += sizeof(BASIC_S3_RESUME_PERF_REC);
    BasBootPerfRecHdr = (PERF_TAB_HEADER*) TempPtr;
    TempPtr += sizeof(PERF_TAB_HEADER);
    BasBootPerfRec = (BASIC_BOOT_PERF_REC*)TempPtr;

    S3PerfRecHdr->Signature = 0x54503353;//   `S3PT'
    S3PerfRecHdr->Length = sizeof(PERF_TAB_HEADER) + sizeof(BASIC_S3_RESUME_PERF_REC);

    S3PerfRec->Header.PerfRecType = 0;
    S3PerfRec->Header.RecLength = sizeof(BASIC_S3_RESUME_PERF_REC);
    S3PerfRec->Header.Revision = 1;

    BasBootPerfRecHdr->Signature = 0x54504246;//  `FBPT'
    BasBootPerfRecHdr->Length = sizeof(PERF_TAB_HEADER) + sizeof(BASIC_BOOT_PERF_REC);

    BasBootPerfRec->Header.PerfRecType = 2;
    BasBootPerfRec->Header.RecLength = sizeof(BASIC_BOOT_PERF_REC);
    BasBootPerfRec->Header.Revision = 2;

    FPDT = (FPDT_50*)*TablPtr;
    PrepareHdr20(FPDT_SIG, (ACPI_HDR*)FPDT, TablVer);
    FPDT->Header.Length = sizeof(FPDT_50);
    FPDT->Header.Revision = 1;
    FPDT->BasS3Rec.PerfRecType = 1;
    FPDT->BasS3Rec.Length = sizeof(FPDT_PERF_RECORD);
    FPDT->BasS3Rec.Revision = 1;
    FPDT->BasS3Rec.Pointer = (EFI_PHYSICAL_ADDRESS)((UINTN)S3PerfRecHdr);

    FPDT->BasBootRec.PerfRecType = 0;
    FPDT->BasBootRec.Length = sizeof(FPDT_PERF_RECORD);
    FPDT->BasBootRec.Revision = 1;
    FPDT->BasBootRec.Pointer = (EFI_PHYSICAL_ADDRESS)((UINTN)BasBootPerfRecHdr);
    
    FPDT->Header.Checksum = ChsumTbl((UINT8*)FPDT, FPDT->Header.Length);
    //---------------------------------------------------------------------------------------------------------------------

    GetCpuNanoFreq (NULL, NULL);  // Get Cpu Frequency
  
    FpdtVar->S3Pointer = FPDT->BasS3Rec.Pointer;
    FpdtVar->BasBootPointer = FPDT->BasBootRec.Pointer;
    Status = pRS->SetVariable(
        EFI_FPDT_VARIABLE,
        &gAmiGlobalVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS |
        EFI_VARIABLE_RUNTIME_ACCESS,
        sizeof(UINT32),
        &FpdtVar
        );
    Status = pRS->GetVariable(
            L"FPDT_Variable_NV", &gAmiGlobalVariableGuid,
		    NULL, &VarSize, &OldFpdtVarAddress
            ); 
    if (EFI_ERROR(Status) || (FpdtVar != OldFpdtVarAddress)) 
    {
        Status = pRS->SetVariable(
            L"FPDT_Variable_NV",
            &gAmiGlobalVariableGuid,
            EFI_VARIABLE_NON_VOLATILE |
            EFI_VARIABLE_BOOTSERVICE_ACCESS,
            sizeof(UINT32),
            &FpdtVar
        );
        ASSERT_EFI_ERROR(Status);
    }

    return Status;
}
#endif
/**
    This function allocates memory for and fills FACP table v 1.1+ with
    predefined values from SDL tokens
         
    @param TablVer Version of FACP table
    @param TablPtr  Pointer to memory, where the FACP table will resides. Filled by this procedure
    @return EFI_STATUS
	@retval EFI_OUT_OF_RESOURCES  Memory for the table could not be allocated
    @retval EFI_SUCCESS  Table was successfully build

**/

#if FACP_BUILD  
EFI_STATUS BuildFacpAll (IN UINTN TablVer, OUT ACPI_HDR **TablPtr)

{
	EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE     *Facp;
    ACPI_HDR    *FACP_Hdr;
    UINT32       SizeOfFacp;
//-----------------------------

    if (TablVer<1 || TablVer>4) return EFI_INVALID_PARAMETER;
    
    if (ACPI_SPEC_VERSION >= 50) SizeOfFacp = sizeof(FACP_50);
    if (ACPI_SPEC_VERSION >= 60) SizeOfFacp = sizeof(EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE); // Add HypervisorVendorIdentity field for ACPI 6.0
    if (TablVer == 1) SizeOfFacp = 0x84;// size of compatability 1.1 structure
    *TablPtr = MallocZ (SizeOfFacp);
    if ((*TablPtr)==NULL)
    {
        ASSERT(*TablPtr);
        return EFI_OUT_OF_RESOURCES;
    }
    
    FACP_Hdr = *TablPtr;
    
    if (TablVer == 1)
    {
        PrepareHdr20(FACP_SIG, FACP_Hdr, 2);
        FACP_Hdr->Revision = ACPI_REV2;// compatability 1.1 structure
    }
    
    else
        PrepareHdr20(FACP_SIG, FACP_Hdr, TablVer);
        
    Facp = (EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE*) *TablPtr;
    
    Facp->Reserved0 = ACPI_INT_MODEL;
    
    Facp->PreferredPmProfile = ACPI_PM_PROFILE;
    
    Facp->Flags = 0;
    
#if HW_REDUCED_ACPI == 0
    Facp->SciInt       = ACPI_SCI_INT;
    Facp->SmiCmd       = SW_SMI_IO_ADDRESS;
    Facp->AcpiEnable   = SW_SMI_ACPI_ENABLE;

    Facp->AcpiDisable  = SW_SMI_ACPI_DISABLE;

    Facp->S4BiosReq    = SW_SMI_S4BIOS; 
    
    Facp->PstateCnt    = SW_SMI_PSTATE_CNT;
    
    if (PM1A_EVT_BLK_ADDRESS > 0xffffffff)
        Facp->Pm1aEvtBlk  = 0;
    else
        Facp->Pm1aEvtBlk  = PM1A_EVT_BLK_ADDRESS;
        
    if (PM1B_EVT_BLK_ADDRESS > 0xffffffff)
        Facp->Pm1bEvtBlk  = 0;
    else
        Facp->Pm1bEvtBlk  = PM1B_EVT_BLK_ADDRESS;
        
    if (PM1A_CNT_BLK_ADDRESS > 0xffffffff) 
        Facp->Pm1aCntBlk  = 0;
    else
        Facp->Pm1aCntBlk  = PM1A_CNT_BLK_ADDRESS;
        
    if (PM1B_CNT_BLK_ADDRESS > 0xffffffff)
        Facp->Pm1bCntBlk  = 0;
    else
        Facp->Pm1bCntBlk  = PM1B_CNT_BLK_ADDRESS;
        
    if (PM2_CNT_BLK_ADDRESS > 0xffffffff)
        Facp->Pm2CntBlk   = 0;
    else
        Facp->Pm2CntBlk   = PM2_CNT_BLK_ADDRESS;
        
    if (PM_TMR_BLK_ADDRESS > 0xffffffff)
        Facp->PmTmrBlk    = 0;
    else
        Facp->PmTmrBlk    = PM_TMR_BLK_ADDRESS;
        
    if (GPE0_BLK_ADDRESS > 0xffffffff)
        Facp->Gpe0Blk  = 0;
    else
        Facp->Gpe0Blk  = GPE0_BLK_ADDRESS;
        
    if (GPE1_BLK_ADDRESS > 0xffffffff) 
        Facp->Gpe1Blk  = 0;
    else
        Facp->Gpe1Blk  = GPE1_BLK_ADDRESS;
        
    Facp->Gpe0BlkLen          = GPE0_BLK_LENGTH; 
    Facp->Gpe1BlkLen          = GPE1_BLK_LENGTH; 
    Facp->Gpe1Base             = GPE1_BASE_OFFSET; 
    Facp->Pm1EvtLen   = PM1_EVT_LENGTH;
    Facp->Pm1CntLen   = PM1_CNT_LENGTH;
    Facp->Pm2CntLen   = PM2_CNT_LENGTH;
    Facp->PmTmrLen     = PM_TMR_LENGTH;
    
    Facp->CstCnt   = SW_SMI_CST_CNT;
    Facp->PLvl2Lat    = P_LVL2_LAT_VAL;
    Facp->PLvl3Lat    = P_LVL3_LAT_VAL;
    Facp->FlushSize    = FLUSH_SIZE_VAL;
    Facp->FlushStride  = FLUSH_STRIDE_VAL;
    Facp->DutyOffset   = DUTY_OFFSET_VAL;
    Facp->DutyWidth    = DUTY_WIDTH_VAL; 
    Facp->DayAlrm      = ACPI_ALARM_DAY_CMOS;
    Facp->MonAlrm      = ACPI_ALARM_MONTH_CMOS;
    Facp->Century       = ACPI_CENTURY_CMOS;
    Facp->IaPcBootArch = ACPI_IA_BOOT_ARCH;
    
    //--------Filling Flags for V.1----------------------
    
    if (FACP_FLAG_WBINVD_FLUSH) Facp->Flags |= 1<<1;
    
    if (FACP_FLAG_PROC_C1)      Facp->Flags |= 1<<2;
    
    if (FACP_FLAG_P_LVL2_UP)    Facp->Flags |= 1<<3;
    
    if (FACP_FLAG_RTC_S4)       Facp->Flags |= 1<<7;
    
    if (FACP_FLAG_TMR_VAL_EXT)  Facp->Flags |= 1<<8;
    
    if (FACP_FLAG_HEADLESS)         Facp->Flags |= 1<<12;
    
    if (FACP_FLAG_CPU_SW_SLP)       Facp->Flags |= 1<<13;
#if ACPI_SPEC_VERSION >= 30  
    
    if (FACP_FLAG_S4_RTC_STS_VALID)             Facp->Flags |= 1<<16;
    
    if (FACP_FLAG_REMOTE_POWER_ON_CAPABLE)  	Facp->Flags |= 1<<17;

    if (FACP_FLAG_PCI_EXP_WAK)      			Facp->Flags |= 1<<14;
    
#endif //#if ACPI_SPEC_VERSION >= 30  
    
#endif //#if HW_REDUCED_ACPI == 0
    
    if (FACP_FLAG_WBINVD)       Facp->Flags |= 1;
    
    if (FACP_FLAG_PWR_BUTTON)   Facp->Flags |= 1<<4;
    
    if (FACP_FLAG_SLP_BUTTON)   Facp->Flags |= 1<<5;
    
    if (FACP_FLAG_FIX_RTC)      Facp->Flags |= 1<<6;
    
    if (FACP_FLAG_DCK_CAP)      Facp->Flags |= 1<<9;
    
    
    //--------Filling Flags for V.2 and GAS compat structure for v.1----------------------
    
    if (FACP_FLAG_SEALED_CASE)      Facp->Flags |= 1<<11;
    

    
#if ACPI_SPEC_VERSION >= 30  
    
    if (FACP_FLAG_USE_PLATFORM_CLOCK)                   Facp->Flags |= 1<<15;
    
    if (TablVer > 2)
    {
        if (FACP_FLAG_FORCE_APIC_CLUSTER_MODEL)             Facp->Flags |= 1<<18;
        
        if (FACP_FLAG_FORCE_APIC_PHYSICAL_DESTINATION_MODE) Facp->Flags |= 1<<19;
        
    }
    
#endif
    
    // RESET_REG GAS_20 structure and value
    Facp->ResetReg.AddressSpaceId   = ACPI_RESET_REG_TYPE;
    Facp->ResetReg.RegisterBitWidth = ACPI_RESET_REG_BITWIDTH;
    Facp->ResetReg.RegisterBitOffset  = ACPI_RESET_REG_BITOFFSET;
    Facp->ResetReg.Address     = ACPI_RESET_REG_ADDRESS;
#ifdef ACPI_RESET_REG_ACCESSWIDTH
    Facp->ResetReg.AccessSize = ACPI_RESET_REG_ACCESSWIDTH;
#endif
    Facp->ResetValue           = ACPI_RESET_REG_VALUE;
    
    if (ACPI_RESET_REG_ADDRESS && FACP_FLAG_RESET_REG_SUP)
    {
        // Set FACP flag
        Facp->Flags |= 1<<10;
    }
    
    if (TablVer == 1)
    {
        Facp->Header.Length     = 0x84;
        Facp->Header.Checksum = 0;
        Facp->Header.Checksum = ChsumTbl((UINT8*)Facp, Facp->Header.Length);
        return EFI_SUCCESS;
    }
    
    //--------This is all for V.1-----------------------
#if HW_REDUCED_ACPI == 0    
    // PM1a_EVT_BLK GAS_20 structure
    Facp->XPm1aEvtBlk.AddressSpaceId  = PM1A_EVT_BLK_TYPE;
    Facp->XPm1aEvtBlk.RegisterBitWidth = PM1A_EVT_BLK_BITWIDTH;
    Facp->XPm1aEvtBlk.RegisterBitOffset = PM1A_EVT_BLK_BITOFFSET;
	// APTIOV_SERVER_OVERRIDE_START
    // Facp->XPm1aEvtBlk.AccessSize  = 2;
	Facp->XPm1aEvtBlk.AccessSize  = PM1A_EVT_BLK_ACCESS_SIZE;
	// APTIOV_SERVER_OVERRIDE_END
    Facp->XPm1aEvtBlk.Address    = PM1A_EVT_BLK_ADDRESS;
    
    // PM1a_CNT_BLK GAS_20 structure
    Facp->XPm1aCntBlk.AddressSpaceId  = PM1A_CNT_BLK_TYPE;
    Facp->XPm1aCntBlk.RegisterBitWidth = PM1A_CNT_BLK_BITWIDTH;
    Facp->XPm1aCntBlk.RegisterBitOffset = PM1A_CNT_BLK_BITOFFSET;
	// APTIOV_SERVER_OVERRIDE_START
    // Facp->XPm1aCntBlk.AccessSize  = 2;
	Facp->XPm1aCntBlk.AccessSize  = PM1A_CNT_BLK_ACCESS_SIZE;
	// APTIOV_SERVER_OVERRIDE_END
    Facp->XPm1aCntBlk.Address    = PM1A_CNT_BLK_ADDRESS;
    
    // PM1b_EVT_BLK GAS_20 structure
    Facp->XPm1bEvtBlk.AddressSpaceId  = PM1B_EVT_BLK_TYPE;
    Facp->XPm1bEvtBlk.RegisterBitWidth = PM1B_EVT_BLK_BITWIDTH;
    Facp->XPm1bEvtBlk.RegisterBitOffset = PM1B_EVT_BLK_BITOFFSET;
	// APTIOV_SERVER_OVERRIDE_START
    // Facp->XPm1bEvtBlk.AccessSize  = 2;
	Facp->XPm1bEvtBlk.AccessSize  = PM1B_EVT_BLK_ACCESS_SIZE;
	// APTIOV_SERVER_OVERRIDE_END
    Facp->XPm1bEvtBlk.Address    = PM1B_EVT_BLK_ADDRESS;
    
    // PM1b_CNT_BLK GAS_20 structure
    Facp->XPm1bCntBlk.AddressSpaceId  = PM1B_CNT_BLK_TYPE;
    Facp->XPm1bCntBlk.RegisterBitWidth = PM1B_CNT_BLK_BITWIDTH;
    Facp->XPm1bCntBlk.RegisterBitOffset = PM1B_CNT_BLK_BITOFFSET;
	// APTIOV_SERVER_OVERRIDE_START
    // Facp->XPm1bCntBlk.AccessSize    = 2;
	Facp->XPm1bCntBlk.AccessSize    = PM1B_CNT_BLK_ACCESS_SIZE;
	// APTIOV_SERVER_OVERRIDE_END
    Facp->XPm1bCntBlk.Address    = PM1B_CNT_BLK_ADDRESS;
    
    // PM1b_CNT_BLK GAS_20 structure
    Facp->XPm2CntBlk.AddressSpaceId   = PM2_CNT_BLK_TYPE;
    Facp->XPm2CntBlk.RegisterBitWidth = PM2_CNT_BLK_BITWIDTH;
    Facp->XPm2CntBlk.RegisterBitOffset  = PM2_CNT_BLK_BITOFFSET;
	// APTIOV_SERVER_OVERRIDE_START	
    //Facp->XPm2CntBlk.AccessSize    = 1;
	Facp->XPm2CntBlk.AccessSize    = PM2_CNT_BLK_ACCESS_SIZE;
	// APTIOV_SERVER_OVERRIDE_END
    Facp->XPm2CntBlk.Address     = PM2_CNT_BLK_ADDRESS;
    
    Facp->XPmTmrBlk.AddressSpaceId    = PM_TMR_BLK_TYPE;
    Facp->XPmTmrBlk.RegisterBitWidth  = PM_TMR_BLK_BITWIDTH;
    Facp->XPmTmrBlk.RegisterBitOffset   = PM_TMR_BLK_BITOFFSET;
	// APTIOV_SERVER_OVERRIDE_START	
    // Facp->XPmTmrBlk.AccessSize    = 3;
	Facp->XPmTmrBlk.AccessSize    = PM_TMR_BLK_ACCESS_SIZE;
	// APTIOV_SERVER_OVERRIDE_END	
    Facp->XPmTmrBlk.Address      = PM_TMR_BLK_ADDRESS;
    
    Facp->XGpe0Blk.AddressSpaceId      = GPE0_BLK_TYPE;
    Facp->XGpe0Blk.RegisterBitWidth    = GPE0_BLK_BITWIDTH;
    Facp->XGpe0Blk.RegisterBitOffset     = GPE0_BLK_BITOFFSET;
	// APTIOV_SERVER_OVERRIDE_START
    // Facp->XGpe0Blk.AccessSize    = 1;
	Facp->XGpe0Blk.AccessSize    = GPE0_BLK_ACCESS_SIZE;
	// APTIOV_SERVER_OVERRIDE_END
    Facp->XGpe0Blk.Address        = GPE0_BLK_ADDRESS;
    
    Facp->XGpe1Blk.AddressSpaceId      = GPE1_BLK_TYPE;
    Facp->XGpe1Blk.RegisterBitWidth    = GPE1_BLK_BITWIDTH;
    Facp->XGpe1Blk.RegisterBitOffset     = GPE1_BLK_BITOFFSET;
	// APTIOV_SERVER_OVERRIDE_START
    // Facp->XGpe1Blk.AccessSize    = 1;
	Facp->XGpe1Blk.AccessSize    = GPE1_BLK_ACCESS_SIZE;
	// APTIOV_SERVER_OVERRIDE_END
    Facp->XGpe1Blk.Address        = GPE1_BLK_ADDRESS;
#endif //#if HW_REDUCED_ACPI == 0   
    if (ACPI_SPEC_VERSION >= 50) 
    {
#if HW_REDUCED_ACPI      

        Facp->Flags |= 1<<20;

        Facp->SleepControlReg.AddressSpaceId   = SLEEP_CONTROL_REG_TYPE;
        Facp->SleepControlReg.RegisterBitWidth = SLEEP_CONTROL_REG_BITWIDTH;
        Facp->SleepControlReg.RegisterBitOffset  = SLEEP_CONTROL_REG_BITOFFSET;
        Facp->SleepControlReg.AccessSize  = SLEEP_CONTROL_REG_ACCESS_SIZE;
        Facp->SleepControlReg.Address     = SLEEP_CONTROL_REG_ADDRESS;

        Facp->SleepStatusReg.AddressSpaceId    = SLEEP_STATUS_REG_TYPE;
        Facp->SleepStatusReg.RegisterBitWidth  = SLEEP_STATUS_REG_BITWIDTH;
        Facp->SleepStatusReg.RegisterBitOffset   = SLEEP_STATUS_REG_BITOFFSET;
        Facp->SleepStatusReg.AccessSize   = SLEEP_STATUS_REG_ACCESS_SIZE;
        Facp->SleepStatusReg.Address      = SLEEP_STATUS_REG_ADDRESS;
#endif
    
        if (LOW_POWER_S0_IDLE_CAPABLE)       Facp->Flags |= 1<<21;
        Facp->Header.Revision = 5; // ACPI 5.0 revision
#if ACPI_SPEC_VERSION >= 51
        
        Facp->ArmBootArch = ACPI_ARM_BOOT_ARCH; //ARM flags added in ACPI 5.1 revision
        if ((ACPI_SPEC_VERSION == 51) || (ACPI_SPEC_VERSION == 61)) Facp->MinorVersion = 1; // FADT Minor Version = 1 for ACPI 5.1 revision
        if (ACPI_SPEC_VERSION >= 60) 
        {
        	Facp->Header.Revision = 6; // ACPI 6 revision
#ifdef HYPERVISOR_VENDOR_IDENTITY
        	Facp->HypervisorVendorIdentity = HYPERVISOR_VENDOR_IDENTITY;
#endif
        }
#endif    
    }
    Facp->Header.Length     = SizeOfFacp;
    Facp->Header.Checksum = 0;
    Facp->Header.Checksum = ChsumTbl((UINT8*)Facp, Facp->Header.Length);
    
    return EFI_SUCCESS;
    
}// end of BuildFacpAll
#endif

/**
    This function will probe IndReg and DataReg of discovered IOxAPIC base address
    and allocate memory for and builds Ioapic/Iosapic entry for MADT table

         
    @param BaseAddr Base Addrerss where IOxApic presence detected
    @param VecBase pointer to the System Vector Base variable that
        needs to be updated with IOAPIC.MAXREDIR# by this function


          
    @retval MADT_ENTRY_HEADER pointer to the Ioapic/Iosapi entry header for the MADT table
        if NULL - not enough memory

**/
#pragma optimize( "", off )
MADT_ENTRY_HEADER *BuildIoapicIosapic(IN UINT32 BaseAddr, OUT UINT32 *VecBase)
{
    UINT32          Tmp;
    UINT32  volatile        *DatPtr;
    UINT8   volatile        *IdxPtr;
    UINT32          IoapicId;
    UINT32          IoapicVer;
    IOAPIC_H20       *IoapicPtr;
    IOSAPIC_H20      *IosapicPtr;
    
//==============================
    //Read IOAPIC.ID reg and figure out
    //1.How many INTINs it has
    //2.What ID did BIOS gave to it
    IdxPtr=(UINT8*)(BaseAddr + IOA_IND_REG_OFFS);
    DatPtr=(UINT32*)(BaseAddr + IOA_DAT_REG_OFFS);
    
    *IdxPtr = IOA_ID_IND;                   // bits 27..24=IOAPIC_ID
    
    Tmp = *DatPtr;
    
    IoapicId = ((Tmp&IO_APIC_ID_READ_MASK)>>24);
    
    *IdxPtr = IOA_VER_IND;                  // IOAPIC_VER register
    
    Tmp=*DatPtr;
    
    IoapicVer=(Tmp&0xff);
    
    if (IoapicVer < IO_APIC_VERSION_PARAMETER)  // This is Ioapic
    {
        IoapicPtr = MallocZ(sizeof(IOAPIC_H20));
        ASSERT (IoapicPtr);
        
        if (!IoapicPtr) return NULL;
        
        IoapicPtr->Header.Type = (UINT8) AT_IOAPIC;     // 1 - For IOAPIC Structure
        IoapicPtr->Header.Length = (UINT8) sizeof (IOAPIC_H20);
        IoapicPtr->IoapicAddress = (UINT32) BaseAddr;
        IoapicPtr->IoapicId = (UINT8) IoapicId;
        IoapicPtr->SysVectBase = (UINT32) *VecBase;
        //Adjust VecBase with current Max Redirection value
        (*VecBase)+=((UINT32)((Tmp&0x00ff0000)>>16))+1;
        return (MADT_ENTRY_HEADER*) IoapicPtr;
    }
    
    else  // This is Iosapic
    {
        IosapicPtr = MallocZ(sizeof(IOSAPIC_H20));
        ASSERT (IosapicPtr);
        
        if (!IosapicPtr) return NULL;
        
        IosapicPtr->Header.Type = (UINT8) AT_IOSAPIC;     // 6 - For IOSAPIC Structure
        IosapicPtr->Header.Length = (UINT8) sizeof (IOSAPIC_H20);
        IosapicPtr->IosapicAddress = (UINT64) BaseAddr;
        IosapicPtr->IoapicId = (UINT8) IoapicId;
        IosapicPtr->SysVectBase = (UINT32) *VecBase;
        //Adjust VecBase with current Max Redirection value
        (*VecBase)+=((UINT32)((Tmp&0x00ff0000)>>16))+1;
        return (MADT_ENTRY_HEADER*) IosapicPtr;
    }
    
}// End of BuildIoapicIosapic
#pragma optimize( "", on )

/**
    Allocates memory for and builds LAPIC entry for MADT table filled with
    Dummy information

          
    @retval MADT_ENTRY_HEADER pointer to the LAPIC entry header for the MADT table
        if NULL - not enough memory

**/

MADT_ENTRY_HEADER *BuildIoapicManualy()
{
    IOAPIC_H20       *IoapicPtr;
    
    IoapicPtr = MallocZ(sizeof(IOAPIC_H20));
    ASSERT (IoapicPtr);
    
    if (!IoapicPtr) return NULL;
    
    IoapicPtr->Header.Type = (UINT8) AT_IOAPIC;     // 1 - For IOAPIC Structure
    IoapicPtr->Header.Length = (UINT8) sizeof (IOAPIC_H20);
    IoapicPtr->IoapicAddress = (UINT32) APCB;
    IoapicPtr->IoapicId = (UINT8) 0;
    IoapicPtr->SysVectBase = (UINT32) 0;
    return (MADT_ENTRY_HEADER*) IoapicPtr;
    
    
}// end of BuildIoapicManualy

/**
    Allocates memory for and builds ISO entry for MADT table, based
    on INTERRUPT SOURCE OVERRIDE info from predifined table

    @param IsoTblNumb - Number of entry in ISO table for which an entry should
        be build.

          
    @retval MADT_ENTRY_HEADER pointer to the ISO entry header for the MADT table
        if NULL - not enough memory

**/

MADT_ENTRY_HEADER *BuildIsoFromTbl (UINTN IsoTblNumb)
{
    ISO_H20     *IsoPtr;
    
    IsoPtr = MallocZ(sizeof(ISO_H20));
    ASSERT (IsoPtr);
    
    if (!IsoPtr) return NULL;
    
    IsoPtr->Header.Type = AT_ISO; // 2 - For ISO structure
    IsoPtr->Header.Length = sizeof(ISO_H20);
    IsoPtr->Bus = 0;
    IsoPtr->Source = (UINT8) IsoTbl[IsoTblNumb].PicIrq;
    IsoPtr->GlobalSysVect = (UINT32) IsoTbl[IsoTblNumb].ApicInt;
    IsoPtr->Flags = (UINT16) IsoTbl[IsoTblNumb].Flags;
    return (MADT_ENTRY_HEADER*) IsoPtr;
}//end of BuildIsoFromTbl

/**
    Creates MADT entries, stores them as T_ITEM_LIST, calculates
    space needed, allocates memory and builds MADT table from entries

         
    @param TablVer ACPI Spec. Version
    @param TablPtr Pointer, to the MADT table, filled by this function

    @return EFI_STATUS
    @retval EFI_OUT_OF_RESOURCES not enough memory
    @retval EFI_UNSUPPORTED CPU info hob not found
    @retval EFI_INVALID_PARAMETER invalid ACPI version
    @retval EFI_SUCCESS MADT table were successfully build

**/
#if ACPI_APIC_TBL == 1
EFI_STATUS BuildMadtAll(IN UINTN TablVer, OUT ACPI_HDR **TablPtr)
{
    UINTN                       LastItem;
    EFI_STATUS                  Status;
    MADT_ENTRIES                MadtTblEntries = {0, 0, NULL};
    MADT_ENTRY_HEADER           *HdrPtr;
    UINT32                      IoapicVbase = 0, AllStrLength = 0, IoapicAddr = 0;
    UINT8                       *DestinPtr;
    UINTN 			i;
    #if FEC00000_APIC_AUTODETECT == 1 || PCI_BUS_APIC_AUTODETECT == 1 
    UINTN 			j;
    #endif //FEC00000_APIC_AUTODETECT == 1 || PCI_BUS_APIC_AUTODETECT == 1 
    UINTN                       IoApicCnt=0;

    VOID *CpuEntries;
    UINT8 *CpuEntry;
    UINT32 NumCpuEntries;
    UINT32 CpuEntriesSize;
    
//--------------------------------
    if (TablVer<1 || TablVer>4) return EFI_INVALID_PARAMETER;

    Status = CreateCpuMadtEntries((UINT32)TablVer, &CpuEntries, &NumCpuEntries, &CpuEntriesSize);

    //Add CPU MadtTblEntries
    if (!EFI_ERROR(Status)) {
        CpuEntry = (UINT8*)CpuEntries;
        while(NumCpuEntries--){
            UINT8 Length = ((MADT_ENTRY_HEADER*)CpuEntry)->Length;
            HdrPtr = (MADT_ENTRY_HEADER*)Malloc(Length);
            MemCpy(HdrPtr, CpuEntry, Length);
            AppendItemLst ((T_ITEM_LIST*)&MadtTblEntries, (VOID*) HdrPtr);// First entry
            CpuEntry += Length;
        }
        pBS->FreePool(CpuEntries);
    }

    //Set Marker at last item before IOAPIC/IOsAPIC entries
    LastItem = MadtTblEntries.MadtEntCount;
    
//The folloving code may enable IOAPIC Devices found on PCI BUS
//This IOAPICs might use their ABARs to clame MMIO in FEC0_0000 space.
//So we need to run this code prior  if we don't want to miss any IOAPICs.
//--------------------------------------------------------------------
#if PCI_BUS_APIC_AUTODETECT == 1
    {
        EFI_HANDLE *pHandleBuffer;
        UINTN   NumberOfHandles;
        UINT8   PciData[4];
        EFI_GUID PciIoProtocolGuid = EFI_PCI_IO_PROTOCOL_GUID;
        EFI_PCI_IO_PROTOCOL *pPciIoProtocol;
        
        Status = pBS->LocateHandleBuffer(ByProtocol, &PciIoProtocolGuid,
                                         NULL, &NumberOfHandles, &pHandleBuffer);
                                         
        //The protocol might not be available when function runs first time.
        //we will rerun it on READY_TO_BOOT event again.
        if (!EFI_ERROR(Status))
        {
        
            for (i = 0; i < NumberOfHandles; i++)
            {
                Status = pBS->HandleProtocol(pHandleBuffer[i], &PciIoProtocolGuid,
                                             (VOID**)&pPciIoProtocol);
                                             
                if (EFI_ERROR(Status))
                    continue;
                    
                //read class code information at 0x8 offset in PCI header
                Status = pPciIoProtocol->Pci.Read(pPciIoProtocol, EfiPciIoWidthUint32,
                                                  0x8, 1, &PciData[0]);
                ASSERT_EFI_ERROR(Status);
                
                if (EFI_ERROR(Status)) //problem
                    continue;
                    
                //if IO APIC device
                if ((PciData[3] == 0x8) && (PciData[2] == 0) && (PciData[1] >= 0x10))
                {
                    UINT64  Attr=0, OldAttr=0;
                    //----------------------
                    //1. make sure it is Enabled and Decoding it's resources
                    Status=pPciIoProtocol->Attributes(pPciIoProtocol,EfiPciIoAttributeOperationGet, Attr, &OldAttr);
                    ASSERT_EFI_ERROR(Status);
                    
                    if (EFI_ERROR(Status)) continue;
                    
                    Status=pPciIoProtocol->Attributes(pPciIoProtocol,EfiPciIoAttributeOperationSupported, 0, &Attr);
                    ASSERT_EFI_ERROR(Status);
                    
                    if (EFI_ERROR(Status)) continue;
                    
                    Status=pPciIoProtocol->Attributes(pPciIoProtocol,EfiPciIoAttributeOperationSet, Attr&(EFI_PCI_DEVICE_ENABLE), NULL);
                    ASSERT_EFI_ERROR(Status);
                    
                    if (EFI_ERROR(Status)) continue;
                    
                    
                    //2. collect info
                    Status = pPciIoProtocol->Pci.Read(pPciIoProtocol, EfiPciIoWidthUint32,
                                                      0x10, 1, (VOID*)&IoapicAddr);
                    //problem or mapped to default address range
                    ASSERT_EFI_ERROR(Status);
                    
                    if ( ! (EFI_ERROR(Status) || (IoapicAddr == 0)) )
                    {
                        HdrPtr = BuildIoapicIosapic(IoapicAddr, &IoapicVbase);
                        
                        if (!HdrPtr) return EFI_OUT_OF_RESOURCES;
                        
                        if (MadtTblEntries.MadtEntCount == LastItem)   // Adding entries in groving IOapicId/IOsapicId value order
                        {
                            Status = AppendItemLst ((T_ITEM_LIST*)&MadtTblEntries, (VOID*) HdrPtr);// First entry
                        }
                        
                        else
                        {
                            for (j = (LastItem); j < MadtTblEntries.MadtEntCount; j++)  // No need to handle LSAPIC entry in different way
                            {
                                // IOapicId and IOsapicId filds are on the same place in bouth structures
                                if (((IOAPIC_H32*)HdrPtr)->IoapicId < ((IOAPIC_H32*)MadtTblEntries.MadtEntries[j])->IoapicId) break;
                            }   // found entry with bigger IOapicId/IOsapicId
                            
                            if (j == MadtTblEntries.MadtEntCount)
                            {
                                Status = AppendItemLst ((T_ITEM_LIST*)&MadtTblEntries, (VOID*) HdrPtr);
                            }
                            
                            else
                            {
                                Status = InsertItemLst ((T_ITEM_LIST*)&MadtTblEntries, (VOID*) HdrPtr, j);
                            }
                            
                            ASSERT_EFI_ERROR(Status);
                            
                            if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
                            
                            IoApicCnt++;
                        }
                    }//if( ! (EFI_ERROR(Status) || (IoapicAddr == 0)) )
                    
#if PCI_BUS_APIC_LEAVE_ENABLE != 0
                    //Restore attributes of the device
                    Status=pPciIoProtocol->Attributes(pPciIoProtocol,EfiPciIoAttributeOperationSet, OldAttr, NULL);
                    ASSERT_EFI_ERROR(Status);
                    
                    if (EFI_ERROR(Status)) continue;
                    
#endif
                }//if((PciData[3] == 0x8) && (PciData[2] == 0) && (PciData[1] >= 0x10))
            }//for(i = 0; i < NumberOfHandles; i++)
            
            pBS->FreePool(pHandleBuffer);
        }//if(!EFI_ERROR(Status))
    }
#endif //PCI_BUS_APIC_AUTODETECT == 1
    
//--------------------------------------------------------------------
//---Creating IOAPIC or/and IOSAPIC Entries
#if (APCB != 0)
    IoapicAddr = APCB;
#else
    IoapicAddr = IOA_BASE_BOT;
#endif
    
#if FEC00000_APIC_AUTODETECT == 1
    
    //trying to check if something alive present at 0xFEC00000..0xFED00000
    for ( ; IoapicAddr < IO_APIC_BASE_ADDRESS_TOP; IoapicAddr += 0x1000)
    { //If so read IOAPIC.ID reg and figure out
        //1.How many INTINs it has
        //2.What ID BIOS give to it
        if (*((UINT8 *)(IoapicAddr + IOA_IND_REG_OFFS)) != 0xFF)
        {
            HdrPtr = BuildIoapicIosapic(IoapicAddr, &IoapicVbase);
            
            if (!HdrPtr) return EFI_OUT_OF_RESOURCES;
            
            if (MadtTblEntries.MadtEntCount == LastItem)  // Adding entries in groving IOapicId/IOsapicId value order
                Status = AppendItemLst ((T_ITEM_LIST*)&MadtTblEntries, (VOID*) HdrPtr);// First entry
            else
            {
                for (j = (LastItem); j < MadtTblEntries.MadtEntCount; j++)
                { // No need to handle LSAPIC entry in different way
                    if (((IOAPIC_H32*)HdrPtr)->IoapicId < ((IOAPIC_H32*)MadtTblEntries.MadtEntries[j])->IoapicId) break; // IOapicId and IOsapicId filds are on the same place in bouth structures
                }   // found entry with bigger IOapicId/IOsapicId
                
                if (j == MadtTblEntries.MadtEntCount)
                {
                    Status = AppendItemLst ((T_ITEM_LIST*)&MadtTblEntries, (VOID*) HdrPtr);
                }
                
                else
                {
                    Status = InsertItemLst ((T_ITEM_LIST*)&MadtTblEntries, (VOID*) HdrPtr, j);
                }
            }
            
            ASSERT_EFI_ERROR(Status);
            
            if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
            
            IoApicCnt++;
        }
    }
    
#endif //FEC00000_APIC_AUTODETECT == 1
    
//-------------------------------------------------------------------------------------------
    if (!IoApicCnt)
    {
        HdrPtr = BuildIoapicManualy();
        
        if (!HdrPtr) return EFI_OUT_OF_RESOURCES;
        
        Status = AppendItemLst ((T_ITEM_LIST*)&MadtTblEntries, (VOID*) HdrPtr);
        ASSERT_EFI_ERROR(Status);
        
        if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
    }
    
    
//-------Build ISO Structure----------------------------
    for (i=0; i<IsoCnt; i++)
    {
        if (IsoTbl[i].PicIrq == 0xFF) continue; //no override for this entry
        
        HdrPtr = BuildIsoFromTbl(i);
        
        if (!HdrPtr) return EFI_OUT_OF_RESOURCES;
        
        Status = AppendItemLst ((T_ITEM_LIST*)&MadtTblEntries, (VOID*) HdrPtr);
        ASSERT_EFI_ERROR(Status);
        
        if (EFI_ERROR(Status)) return Status;
    }
    
#if (NMIs_QUANTITY > 0)
    HdrPtr = MallocZ(sizeof(NMI_H20));
    ASSERT (HdrPtr);
    
    if (!HdrPtr) return EFI_OUT_OF_RESOURCES;
    
    ((NMI_H20*) HdrPtr)->Header.Type = (UINT8) AT_NMI;       //Type 3 - indicating NMIs Entry
    ((NMI_H20*) HdrPtr)->Header.Length = (UINT8) sizeof(NMI_H20);
    ((NMI_H20*) HdrPtr)->Flags = (UINT16)((NMI_0_TRIGGER_MODE<<2) | NMI_0_POLARITY);
    ((NMI_H20*) HdrPtr)->GlobalSysVect = (UINT32) NMI_GLOBAL_SYS_INT_0;
    Status = AppendItemLst ((T_ITEM_LIST*)&MadtTblEntries, (VOID*) HdrPtr);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return Status;
    
#if (NMIs_QUANTITY > 1)
    HdrPtr = MallocZ(sizeof(NMIH_20));
    ASSERT (HdrPtr);
    
    if (!HdrPtr) return EFI_OUT_OF_RESOURCES;
    
    ((NMI_H20*) HdrPtr)->Header.Type = (UINT8) AT_NMI;       //Type 3 - indicating NMIs Entry
    ((NMI_H20*) HdrPtr)->Header.Length = (UINT8) sizeof(NMI_H20);
    ((NMI_H20*) HdrPtr)->Flags = (UINT16)((NMI_1_TRIGGER_MODE<<2) | NMI_1_POLARITY);
    ((NMI_H20*) HdrPtr)->GlobalSysVect = (UINT32) NMI_GLOBAL_SYS_INT_1;
    Status = AppendItemLst ((T_ITEM_LIST*)&MadtTblEntries, (VOID*) HdrPtr);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return Status;
#endif
    
//---Porting Hook 1 ------ If NMIs_QUANTITY > 2 - Add more structures
#endif

    //---Tis ia all for Ver 1 - building a table----------------------
//----Here starts entries for V 2 and 3 which are empty for now----------------------------------
//
//          Here entries for LAPIC Address overrride structure and Platforme interrupt source structure
//

    for (i = 0; i < MadtTblEntries.MadtEntCount; i++)
    {
    
        AllStrLength += MadtTblEntries.MadtEntries[i]->Length;
    }
    
    *TablPtr = MallocZ(sizeof(APIC_20H) + (UINTN) AllStrLength);
    ASSERT (*TablPtr);
    
    if (!(*TablPtr)) return EFI_OUT_OF_RESOURCES;
    
    if (TablVer == 1) PrepareHdr1 (APIC_SIG, (PACPI_HDR) *TablPtr);
    else PrepareHdr20 (APIC_SIG, (PACPI_HDR) *TablPtr, TablVer);
    
    (*TablPtr)->Length = (sizeof(APIC_20H) + AllStrLength);
    ((APIC_20H*)*TablPtr)->LAPIC_Address    = LOCAL_APIC_BASE;
    ((APIC_20H*)*TablPtr)->Flags    = ACPI_APIC_FLAGS;
    DestinPtr = ((UINT8*) *TablPtr + sizeof(APIC_20H));
    
    for (i = 0; i < MadtTblEntries.MadtEntCount; i++)
    {
    
    	pBS->CopyMem (DestinPtr, (UINT8*)MadtTblEntries.MadtEntries[i], (UINT32) MadtTblEntries.MadtEntries[i]->Length);
        DestinPtr += MadtTblEntries.MadtEntries[i]->Length;
        pBS->FreePool((VOID*)MadtTblEntries.MadtEntries[i]);
        ASSERT_EFI_ERROR(Status);
        
        if (EFI_ERROR(Status)) return Status;
    }
    
    (*TablPtr)->Checksum = 0;
    (*TablPtr)->Checksum = ChsumTbl((UINT8*)*TablPtr, (*TablPtr)->Length);
    
    return EFI_SUCCESS;
}// end of BuildMadtAll-----------------------------------------
#endif

/**
    Allocates ACPI NVS memory and builds FACS table from values,
    defined by SDL tokens

    @param VOID


    @return EFI_STATUS
    @retval EFI_OUT_OF_RESOURCES not enough memory
    @retval EFI_SUCCESS FACS table were successfully build

**/

EFI_STATUS BuildFacs ()
{
    EFI_STATUS      Status;
    UINTN           Size;
    
    
    if (gForceAcpi1) Size = sizeof(FACS_20);
    else Size = sizeof(FACS_20)*2;
    Status = pBS->AllocatePool(EfiACPIMemoryNVS, Size+64, (VOID **)&gFacs);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
    

#ifdef EFIx64
    if (((UINT64)gFacs) & 0xffffffff00000000)
    {
        EFI_PHYSICAL_ADDRESS Memory = 0x00000000ffffffff;
        Status = pBS->FreePool(gFacs);
        ASSERT_EFI_ERROR(Status);
        Status = pBS->AllocatePages(AllocateMaxAddress, EfiACPIMemoryNVS, 1, &Memory);
        ASSERT_EFI_ERROR(Status);
        
        if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
        
        gFacs = (FACS_20 *) Memory;
    }
#endif

	gFacs = (FACS_20*)((UINTN) gFacs + 64);
    gFacs = (FACS_20*)((UINTN) gFacs & (~0x3F));
    pBS->SetMem(gFacs, Size, 0);
    gFacs->Signature=(UINT32)FACS_SIG;
    gFacs->Length=sizeof(FACS_20);
    gFacs->Flags=FACS_FLAG_S4BIOS;

    if (gForceAcpi1) return EFI_SUCCESS;
    
    gxFacs = (FACS_20*)((UINTN) gFacs + sizeof(FACS_20));
    gxFacs->Signature=(UINT32)FACS_SIG;
    gxFacs->Length=sizeof(FACS_20);
    
    if (ACPI_SPEC_VERSION < 40)
    {
        gxFacs->Flags=FACS_FLAG_S4BIOS;
        gxFacs->Version=ACPI_REV1;
    }
    
    else
    {
        gxFacs->Flags = FACS_FLAG_S4BIOS | (FACS_FLAG_64BIT_WAKE_SUPPORTED << 1);
        gxFacs->Version=ACPI_REV2;
    }
    
    
    
    
    return EFI_SUCCESS;
}// end of  BuildFacs ---------------------------------------------

/**
    This function finds DSDT table in firmvare volume

         
    @param Dsdt1 pointer to memory where DSDT v1.1 table will be stored
    @param Dsdt2 pointer to memory where DSDT v2.0+ table will be stored

          
    @retval EFI_SUCCESS Function executed successfully
    @retval EFI_ABORTED ACPI storage file not found
    @retval EFI_NOT_FOUND DSDT table not found in firmware volume

**/
#if DSDT_BUILD
EFI_STATUS GetDsdtFv(OUT ACPI_HDR **Dsdt2)
{
	EFI_STATUS	Status;
	EFI_PHYSICAL_ADDRESS Memory = 0x00000000ffffffff;
	
    //In current AmiBoardInfo implementation separate instance of
    //DSDT for ACPI version 1.1b  DOES OT SUPPORTED!
	Status=AmiSdlInitBoardInfo();
	if(EFI_ERROR(Status)){
		TRACE((-1,"AcpiCore: No AmiBoardInfo Found: Status=%r\n", Status));
		return EFI_NOT_FOUND;
	}
	gDsdtPages = EFI_SIZE_TO_PAGES(((ACPI_HDR*)gAmiBoardInfo2Protocol->BrdAcpiInfo)->Length);   
    
	Status = pBS->AllocatePages(AllocateMaxAddress, EfiBootServicesData, 
									gDsdtPages, &Memory);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
    {
    	gDsdtPages = 0;
    	return EFI_OUT_OF_RESOURCES;
    }   
    *Dsdt2 = (ACPI_HDR *)((UINTN)Memory);
    
    if (*Dsdt2==NULL) return EFI_OUT_OF_RESOURCES;
    
    pBS->CopyMem(*Dsdt2, gAmiBoardInfo2Protocol->BrdAcpiInfo,
                 ((ACPI_HDR*)gAmiBoardInfo2Protocol->BrdAcpiInfo)->Length);

    
    if  (*Dsdt2 == NULL)
    {
        TRACE((-1,"Acpi: No DSDT was FOUND: Status=EFI_NOT_FOUND\n"));
        return EFI_NOT_FOUND;
    }
    
    PrepareHdr20(DSDT_SIG, (ACPI_HDR*)(*Dsdt2),2);
    
    return EFI_SUCCESS;
    
}// end of GetDsdtFv -----------------------------------------------
#endif
/**
    This function Updates FACP with the new values for DSDT and Facs
    pointers

    @param gFacs1 - Pointer to gFacs Table for V 1
    @param gFacs2 - Pointer to gFacs Table for V 2 or 3

    @retval EFI_SUCCESS Function executed successfully
    @retval EFI_ABORTED Error


   @note  Modifies  gAcpiData

**/

EFI_STATUS UpdateFacp () 
{
    ACPI_HDR    *Facp1 = NULL, *Facp2 = NULL, *Dsdt1 = NULL, *Dsdt2 = NULL;
    UINTN       i;
    EFI_STATUS  Status = EFI_SUCCESS;
    
    for (i = 0; i < gAcpiData.AcpiEntCount; i++)
    {
        if (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == FACP_SIG)
        {
            if (gAcpiData.AcpiEntries[i]->AcpiTblVer == EFI_ACPI_TABLE_VERSION_1_0B)
                Facp1 = gAcpiData.AcpiEntries[i]->BtHeaderPtr; // Find FACP for V 1.1
            else
                Facp2 = gAcpiData.AcpiEntries[i]->BtHeaderPtr; // Find FACP for V 2+
        }
        
        if (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == DSDT_SIG)
        {
            if (gAcpiData.AcpiEntries[i]->AcpiTblVer == EFI_ACPI_TABLE_VERSION_1_0B)
                Dsdt1 = gAcpiData.AcpiEntries[i]->BtHeaderPtr; // Find DSDT for V 1.1
            else
                Dsdt2 = gAcpiData.AcpiEntries[i]->BtHeaderPtr; // Find DSDT for V 2+
        }
    }
    
    if (Dsdt1 == NULL) Dsdt1 = Dsdt2;  // The same DSDT for V1.1
    
    if (Dsdt2 == NULL) Dsdt2 = Dsdt1;  // The same DSDT for V2    
    
//--- Updating FACP for V1.1 and 2+---------------------------
    if (Facp1)
    {
    	((FACP32*) Facp1)->FIRMWARE_CTRL = (UINT32) gFacs;
    	if (Dsdt1) 
    		((FACP32*) Facp1)->DSDT = (UINT32) Dsdt1;
    	else
    	    ((FACP32*) Facp1)->DSDT = 0;
    
    	Facp1->Checksum = 0;
    	Facp1->Checksum = ChsumTbl((UINT8*)Facp1, Facp1->Length);
    	if (gForceAcpi1) return Status;
    }
    if (Facp2)
    {
    	if (ACPI_SPEC_VERSION >= 40) ((FACP_20*) Facp2)->FIRMWARE_CTRL = (UINT32) gxFacs;
    	else ((FACP_20*) Facp2)->FIRMWARE_CTRL = (UINT32) gFacs;
    
    	if (Dsdt1)
    		((FACP_20*) Facp2)->DSDT = (UINT32) Dsdt1;
    	else
    		((FACP_20*) Facp2)->DSDT = 0;
    
    	if (ACPI_SPEC_VERSION < 40)((FACP_20*) Facp2)->X_FIRMWARE_CTRL = (UINT64) ((UINTN) gxFacs);
    
    	if (Dsdt2)
    		((FACP_20*) Facp2)->X_DSDT = (UINT64) ((UINTN)Dsdt2);
    	else
    		((FACP_20*) Facp2)->X_DSDT = 0;
    	Facp2->Checksum = 0;
    	Facp2->Checksum = ChsumTbl((UINT8*)Facp2, Facp2->Length);
    }
    return Status;
}// end of UpdateFacp

/**
    Copys table from position pointed by FromPtr to a position pointed
    by ToPtr (which is in EfiACPIReclaimMemory) and fills RSDT and XSDT
    pointers with ToPtr value


    @param RsdtPtr - pionter to RSDT
    @param XsdtPtr - pionter to XSDT
    @param FromPtr - pointer to a table which should be copyed
    @param ToPtr - pointer to EfiACPIReclaimMemory where table should be placed

    @retval UINT8 Pointer to the next avaiable space in allocated EfiACPIReclaimMemory
        right after copyed table (alligned already)
        If NUUL - Invalid parameters.

**/

UINT8 *FillAcpiStr (RSDT32 *RsdtPtr, XSDT_20 *XsdtPtr, VOID *FromPtr, VOID *ToPtr)
{
    UINT8       *NextPtr;
    UINTN       i;
    
    
    if ((RsdtPtr == NULL) && (XsdtPtr == NULL)) return NULL;
    
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    if ((FromPtr == NULL) || (ToPtr == NULL)) 
    {	
    	ASSERT(0);
    	return NULL;
    }
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
    pBS->CopyMem(ToPtr, FromPtr, ((ACPI_HDR*)FromPtr)->Length);
    
    if (RsdtPtr != NULL)
    {
        for (i = 0; RsdtPtr->Ptrs[i] != 0; i++); // Find first unfilled (last) entry in RSDT
        
        RsdtPtr->Ptrs[i] = (UINT32) ToPtr;
    }
    
    if (XsdtPtr != NULL)
    {
        for (i = 0; XsdtPtr->Ptrs[i] != 0; i++); // Find first unfilled (last) entry in XSDT
        
        XsdtPtr->Ptrs[i] = (UINT64) ((UINTN)ToPtr);
    }
    
    NextPtr = (UINT8*) ((UINT8*)ToPtr+((ACPI_HDR*)ToPtr)->Length + 7);
    
    NextPtr = (UINT8 *)((UINTN)NextPtr & (~0x7));
    
    return NextPtr;
    
}//end of FillAcpiStr

/**
    Creates or rewrites RSDT_PTR structure and copys tables, stored
    in gAcpiData structure in allocated EfiACPIReclaimMemory.


    @param RsdtBuild if 1 - Build RSDT and copy tables of Ver 1.1
    @param XsdtBuild if 1 - Build XSDT and copy tables of Ver 2+

    @retval EFI_OUT_OF_RESOURCES not enough memory
    @retval EFI_ABORTED invalid parameters
    @retval EFI_SUCCESS RSDT_PTR structure was successfully build

**/
EFI_STATUS PublishTbl (IN UINT8 RsdtBuild, IN UINT8 XsdtBuild)
{
    ACPI_HDR    *Facp = NULL, *FacpX = NULL, *Dsdt = NULL, *DsdtX = NULL;
    RSDT32      *Rsdt = NULL;
    XSDT_20     *Xsdt = NULL;
    UINTN       i, j, RsdtCnt = 0, XsdtCnt = 0, SpaceNeeded = 0, RsdtSize = 0, XsdtSize = 0, Skip;
    EFI_STATUS  Status;
    UINT8       *Ptr = NULL, *Dummy = NULL, OneDsdt = 0;
    EFI_PHYSICAL_ADDRESS Memory = 0x00000000ffffffff;
    
    if (((RsdtBuild == 0) && (XsdtBuild == 0)) || (RsdtBuild > 1 ) || (XsdtBuild > 1)) return EFI_ABORTED;
#if defined(OemActivation_SUPPORT) && (OemActivation_SUPPORT == 1)
    if (!gOA3Variable)
    {
        UINTN SizeOfMsdm = sizeof(EFI_OA3_MSDM_STRUCTURE);
        //Looking for Var which signals that MSDM table may be updated on runtime
        Status = pRS->GetVariable(
            EFI_OA3_MSDM_VARIABLE,
            &gAmiGlobalVariableGuid,
            NULL,
            &SizeOfMsdm,
            &gMsdmVariable
        );
        if (Status == EFI_SUCCESS) 
        {
            gOA3Variable = TRUE;
            XsdtCnt++; // Reserving Entry for MSDM (OEM Activation)
        }
    }
    else XsdtCnt++;
#endif
    for (i = 0; i < gAcpiData.AcpiEntCount; i++)
    {
        if (gAcpiData.AcpiEntries[i]->AcpiTblVer < EFI_ACPI_TABLE_VERSION_1_0B) continue;
        
        if (gAcpiData.AcpiEntries[i]->AcpiTblVer == EFI_ACPI_TABLE_VERSION_1_0B)
        {
            RsdtCnt ++;
            
            if (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == FACP_SIG)
                Facp = gAcpiData.AcpiEntries[i]->BtHeaderPtr;
                
            if (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == DSDT_SIG)
                Dsdt = gAcpiData.AcpiEntries[i]->BtHeaderPtr;
        }
        
        if (gAcpiData.AcpiEntries[i]->AcpiTblVer > EFI_ACPI_TABLE_VERSION_1_0B)
        {
            XsdtCnt ++;
            RsdtCnt ++;
            
            if (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == FACP_SIG)
            {
			// EIP251219 >>> Workaround for too many FACPs
                if (FacpX != NULL) {
                    FacpX = gAcpiData.AcpiEntries[i]->BtHeaderPtr;
                    XsdtCnt --;
                    RsdtCnt --;
                    continue;
                }
			// EIP251219 <<<
                FacpX = gAcpiData.AcpiEntries[i]->BtHeaderPtr;
                //if (Facp != NULL) RsdtCnt --;
                //continue;
            }
            
            if (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == DSDT_SIG)
            {
                DsdtX = gAcpiData.AcpiEntries[i]->BtHeaderPtr;
            }
            
            for (j = 0; j < gAcpiData.AcpiEntCount; j++)
            {
                if (j == i) continue;
                
                if ((gAcpiData.AcpiEntries[j]->BtHeaderPtr->Signature == gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature)
                        && (gAcpiData.AcpiEntries[j]->AcpiTblVer == EFI_ACPI_TABLE_VERSION_1_0B)) RsdtCnt --;
            }
        }
    }
    if ((FacpX == NULL) && (Facp == NULL))
    {
    	TRACE((DEBUG_INFO,"In PublishTbl: No Facp and FacpX! Publishing will be delayed until they will be available. \n"));
    	return EFI_SUCCESS;
    }
    if ((Dsdt == NULL) && (DsdtX == NULL))
    {
        TRACE((DEBUG_INFO,"In PublishTbl: No Dsdt and DsdtX! Publishing will be delayed until they will be available. \n"));
        return EFI_SUCCESS;
    }
    if (Facp == NULL)
    {
    	if (XsdtBuild == 0)
    	{
    		TRACE((DEBUG_INFO,"In PublishTbl: No Facp and XsdtBuild = 0! Publishing will be delayed until it will be available. \n"));
    		return EFI_SUCCESS;
    	}
    	TRACE((DEBUG_INFO,"In PublishTbl: No Facp. RSDT will not be published.\n"));
    	RsdtBuild = 0;
    }
    if (FacpX == NULL)
    {
    	if (RsdtBuild == 0)
    	{
    	    TRACE((DEBUG_INFO,"In PublishTbl: No FacpX and RsdtBuild = 0! Publishing will be delayed until it will be available. \n"));
    	    return EFI_SUCCESS;
    	}
        TRACE((DEBUG_INFO,"In PublishTbl: No FacpX. XSDT will not be published.\n"));
        XsdtBuild = 0;
    }
    
    if ((Dsdt == NULL) || (DsdtX == NULL)) OneDsdt = 1;
    
    if (Dsdt == NULL) Dsdt = DsdtX;
    
    if (DsdtX == NULL) DsdtX = Dsdt;
    
    
    
    RsdtSize = RsdtBuild * (sizeof(ACPI_HDR) + (RsdtCnt-1) * sizeof(UINT32) + 7); //DSDT does not goes in RSDT (RsdtCnt-1)
    
    XsdtSize = XsdtBuild * (sizeof(ACPI_HDR) + (XsdtCnt-1) * sizeof(UINT64) + 7); //DSDT does not goes in XSDT (XsdtCnt-1)
    SpaceNeeded = sizeof(RSDT_PTR_20) + RsdtSize + XsdtSize + gAcpiData.AcpiLength + (gAcpiData.AcpiEntCount + 1) * 8;
    
    if (gRsdtPtrStr != NULL) 
    {
        Status = pBS->FreePages((EFI_PHYSICAL_ADDRESS)(UINTN) gRsdtPtrStr, gAcpiTblPages);
        ASSERT_EFI_ERROR(Status);
    }

    gAcpiTblPages = EFI_SIZE_TO_PAGES(SpaceNeeded);
#if defined(OemActivation_SUPPORT) && (OemActivation_SUPPORT == 1)
    if (gOA3Variable)
        Status = pBS->AllocatePages(AllocateMaxAddress, EfiACPIMemoryNVS, gAcpiTblPages , &Memory);
    else
#endif
    Status = pBS->AllocatePages(AllocateMaxAddress, EfiACPIReclaimMemory, gAcpiTblPages , &Memory);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
    
    gRsdtPtrStr = (RSDT_PTR_20*)Memory;
    
    pBS->SetMem(gRsdtPtrStr, SpaceNeeded, 0);
    
    if (RsdtBuild)
    {
        Dummy = (UINT8*) ((UINT8*)gRsdtPtrStr + sizeof(RSDT_PTR_20) + 7);
        Dummy = (UINT8*)((UINTN) Dummy & (~0x7));
        Rsdt = (RSDT32*) Dummy;
        Ptr = (UINT8*)gRsdtPtrStr + sizeof(RSDT_PTR_20) + RsdtSize + XsdtSize + 7;
        Ptr = (UINT8*)((UINTN) Ptr & (~0x7));
        Dummy = FillAcpiStr (Rsdt, NULL, (VOID*) Facp, (VOID*) Ptr);// The first table in RSDT must be Facp
        Facp = (ACPI_HDR*) Ptr;
        Ptr = Dummy;
        pBS->CopyMem((VOID*)Ptr, (VOID*)Dsdt, Dsdt->Length);
        
        if (Dsdt == DsdtX) Dsdt = DsdtX = (ACPI_HDR*) Ptr;
        else Dsdt = (ACPI_HDR*) Ptr;
        
        Ptr += (Dsdt->Length + 7);
        Ptr = (UINT8*)((UINTN) Ptr & (~0x7));
        ((FACP32*)Facp)->FIRMWARE_CTRL = (UINT32) gFacs;
        ((FACP32*)Facp)->DSDT = (UINT32) Dsdt;
        Facp->Checksum = 0;
        Facp->Checksum = ChsumTbl((UINT8*)Facp, Facp->Length);
    }
    
    if ((XsdtBuild) && (!gForceAcpi1))
    {
        Dummy = (UINT8*) ((UINT8*)gRsdtPtrStr + sizeof(RSDT_PTR_20) + RsdtSize + 7);
        Dummy = (UINT8*)((UINTN) Dummy & (~0x7));
        Xsdt = (XSDT_20*) Dummy;
        
        if (Ptr == NULL)
        {
            Ptr = (UINT8*) ((UINT8*)gRsdtPtrStr + sizeof(RSDT_PTR_20) + RsdtSize + XsdtSize + 7);

    		Ptr = (UINT8*)((UINTN) Ptr & (~0x7));
        }
        
        Dummy = FillAcpiStr (NULL, Xsdt, (VOID*) FacpX,( VOID*) Ptr);
        FacpX = (ACPI_HDR*) Ptr;
        Ptr = Dummy;
        
        if ((!OneDsdt) || (!RsdtBuild))
        {
            pBS->CopyMem((VOID*)Ptr, (VOID*)DsdtX, DsdtX->Length);
            DsdtX = (ACPI_HDR*) Ptr;
            Ptr += (DsdtX->Length + 7);
         	Ptr = (UINT8*)((UINTN) Ptr &(~0x7));
        }
        
        if (ACPI_SPEC_VERSION >= 40) ((FACP_20*)FacpX)->FIRMWARE_CTRL = (UINT32) gxFacs;
        else ((FACP_20*)FacpX)->FIRMWARE_CTRL = (UINT32) gFacs;
        
        ((FACP_20*)FacpX)->DSDT = (UINT32) Dsdt;
        
        if (ACPI_SPEC_VERSION < 40)((FACP_20*)FacpX)->X_FIRMWARE_CTRL = (UINT64) ((UINTN)gxFacs);
        
        ((FACP_20*)FacpX)->X_DSDT = (UINT64) ((UINTN)DsdtX);
        FacpX->Checksum = 0;
        FacpX->Checksum = ChsumTbl((UINT8*)FacpX, FacpX->Length);
    }
    
    for (i = 0; i < gAcpiData.AcpiEntCount; i++)
    {
        if ((gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == FACP_SIG) ||
                (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == DSDT_SIG) ||
                (gAcpiData.AcpiEntries[i]->AcpiTblVer < EFI_ACPI_TABLE_VERSION_1_0B)) continue;
                
        if (gAcpiData.AcpiEntries[i]->AcpiTblVer == EFI_ACPI_TABLE_VERSION_1_0B)
        {
            Dummy = FillAcpiStr (Rsdt, NULL, (VOID*) gAcpiData.AcpiEntries[i]->BtHeaderPtr,( VOID*) Ptr);
            
            if (Dummy != NULL) Ptr = Dummy;
        }
        
        else
        {
            Skip = 0;
            
            for (j = 0; j < gAcpiData.AcpiEntCount; j++)
            {
                if (j == i) continue;
                
                if ((gAcpiData.AcpiEntries[j]->BtHeaderPtr->Signature == gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature)
                        && (gAcpiData.AcpiEntries[j]->AcpiTblVer == EFI_ACPI_TABLE_VERSION_1_0B)) Skip = 1;
            }
            
            if (Skip) Dummy = FillAcpiStr (NULL, Xsdt, (VOID*) gAcpiData.AcpiEntries[i]->BtHeaderPtr,( VOID*) Ptr);
            else Dummy = FillAcpiStr (Rsdt, Xsdt, (VOID*) gAcpiData.AcpiEntries[i]->BtHeaderPtr,( VOID*) Ptr);
            
            if (Dummy != NULL) Ptr = Dummy;
        }
    }
    
    if (Ptr > ((UINT8*)gRsdtPtrStr + SpaceNeeded))  Status = EFI_OUT_OF_RESOURCES;
    
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return Status;
    
    gRsdtPtrStr->Signature = RSDP_SIG;
    
    if ((!XsdtBuild) || (gForceAcpi1)) gRsdtPtrStr->Revision = 0; //Reserved in ver 1
    else  gRsdtPtrStr->Revision = ACPI_REV2; // 2 for ver 2 and 3
    
    gRsdtPtrStr->Length = sizeof(RSDT_PTR_20);//this is the length of entire structure
    
    for (i=0; i<6; i++) gRsdtPtrStr->OEMID[i]=ACPI_OEM_ID[i];
    
    gRsdtPtrStr->RsdtAddr = (UINT32)(UINTN) Rsdt;
    gRsdtPtrStr->XsdtAddr = (UINT64)(UINTN) Xsdt;
    gRsdtPtrStr->Checksum = ChsumTbl((UINT8*)gRsdtPtrStr, 20);
    gRsdtPtrStr->XtdChecksum = ChsumTbl((UINT8*)gRsdtPtrStr, sizeof (RSDT_PTR_20));
    
    if (RsdtBuild)
    {
        PrepareHdr1 (RSDT_SIG, (ACPI_HDR*) Rsdt);
        Rsdt->Header.Length = (UINT32) (RsdtSize - 7);
        Rsdt->Header.Checksum = 0;
        Rsdt->Header.Checksum = ChsumTbl((UINT8*)Rsdt, Rsdt->Header.Length);
    }
    
    if (XsdtBuild)
    {
        if (ACPI_SPEC_VERSION >= 30) PrepareHdr20 (XSDT_SIG, (ACPI_HDR*) Xsdt, 3);
        else PrepareHdr20 (XSDT_SIG, (ACPI_HDR*) Xsdt, 2);
#if defined(OemActivation_SUPPORT) && (OemActivation_SUPPORT == 1)
        if (gOA3Variable)
            Xsdt->Header.Length = (UINT32) (XsdtSize - sizeof(UINT64) - 7);//one reserved position for MSDM
        else
#endif        
        Xsdt->Header.Length = (UINT32) (XsdtSize - 7);
        Xsdt->Header.Checksum = 0;
        Xsdt->Header.Checksum = ChsumTbl((UINT8*)Xsdt, Xsdt->Header.Length);
//    if (RsdtBuild) Status = pBS->InstallConfigurationTable(&gAcpi11TAbleGuid, NULL); // Delete V1.1 entry - Only one
        Status = pBS->InstallConfigurationTable(&gAcpi20TAbleGuid, (VOID*) gRsdtPtrStr);
        TRACE((-1,"Installing ACPI 2.0: %r, %X\n",Status,gRsdtPtrStr));
    }
    
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return Status;
    
	if (RsdtBuild) 
	{
    	Status = pBS->InstallConfigurationTable(&gAcpi11TAbleGuid, (VOID*) gRsdtPtrStr);
    	TRACE((-1,"Installing ACPI 1.1: %r, %X\n",Status,gRsdtPtrStr));
        ASSERT_EFI_ERROR(Status);
        
        if (EFI_ERROR(Status)) return Status;
    }
#if defined(OemActivation_SUPPORT) && (OemActivation_SUPPORT == 1)
    if ((Xsdt != NULL) && gOA3Variable)
    {
        gMsdmVariable.XsdtAddress = (EFI_PHYSICAL_ADDRESS) Xsdt;
        Status = pRS->SetVariable(
            EFI_OA3_MSDM_VARIABLE,
            &gAmiGlobalVariableGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
            sizeof(EFI_OA3_MSDM_STRUCTURE),
            &gMsdmVariable
        );
        ASSERT_EFI_ERROR(Status);
    }
#endif
    return EFI_SUCCESS;
    
}// end of PublishTbl


/**
    This function updates AML objects with values provided in ACPI_AML_UPD_INFO
    structure

         
    @param AmlUpdInfo pointer to ACPI_AML_UPD_INFO structure

          
    @retval EFI_SUCCESS AML objects updated successfully
    @retval EFI_ERROR some error occured during update process

**/

EFI_STATUS UpdateAml(ACPI_AML_UPD_INFO  *AmlUpdInfo)
{
    UINTN           i;
    ACPI_HDR    *Dsdt;
    EFI_STATUS      Status = EFI_SUCCESS;
    UINT8           *SxPointer = NULL;
    
//-------------------------

    for (i = 0; i < 2; i++)
    {
        if (i) Dsdt = (ACPI_HDR*)AmlUpdInfo->Dsdt1Addr;
        else Dsdt = (ACPI_HDR*)AmlUpdInfo->Dsdt2Addr;
        
        if (Dsdt)
        {
#if ATAD_SUPPORT == 1
        	if (AtadBuffPtr)
        	{
        		TRACE((-1, "AtadBuffPtr=0x%lX ", (UINT64)AtadBuffPtr));
        		Status = UpdateAslNameObject(Dsdt, "ATBF", (UINT64)AtadBuffPtr);
        		ASSERT_EFI_ERROR (Status);
        		if (!EFI_ERROR(Status))
        			Status = UpdateAslNameObject(Dsdt, "BUFU", 1);
        		ASSERT_EFI_ERROR (Status);
        	}
#endif
        	if (AmlUpdInfo->SS3 == 0)
        	{
        		SxPointer=FindAslObjectName((UINT8*)Dsdt+sizeof(ACPI_HDR),"_S3",Dsdt->Length-sizeof(ACPI_HDR)-1);
            	if (SxPointer!=NULL) SxPointer[0]='X';
        	}
        	if (AmlUpdInfo->SS1 == 0)
        	{
        		SxPointer=FindAslObjectName((UINT8*)Dsdt+sizeof(ACPI_HDR),"_S1",Dsdt->Length-sizeof(ACPI_HDR)-1);
        	    if (SxPointer!=NULL) SxPointer[0]='X';
        	}
        	if (AmlUpdInfo->SS4 == 0)
        	{
        		SxPointer=FindAslObjectName((UINT8*)Dsdt+sizeof(ACPI_HDR),"_S4",Dsdt->Length-sizeof(ACPI_HDR)-1);
        	    if (SxPointer!=NULL) SxPointer[0]='X';
        	}
            //Update TOPM Object
            Status = UpdateAslNameObject(Dsdt, "TOPM", AmlUpdInfo->TopOfMemory);
            ASSERT_EFI_ERROR (Status);
            
            if (EFI_ERROR(Status)) return Status;
            
            //Update ROMS Object
            //Status = UpdateNameObject(Dsdt, AML_NAME_ROMS, AmlUpdInfo->RomStart);
            //ASSERT_EFI_ERROR(Status)
            //if(EFI_ERROR(Status)) return Status;

            //Update IOST Object
#if GenericSio_SUPPORT
            Status = UpdateAslNameObject(Dsdt, "IOST", (UINT64)AmlUpdInfo->SioDevStatusVar.DEV_STATUS);
            ASSERT_EFI_ERROR(Status);
            
            if (EFI_ERROR(Status)) return Status;
#endif
            Status = UpdateAslNameObject(Dsdt, "SS4", (UINT64)AmlUpdInfo->SS4);
            ASSERT_EFI_ERROR(Status);
            
            if (EFI_ERROR(Status)) return Status;
            
            Status = UpdateAslNameObject(Dsdt, "SS3", (UINT64)AmlUpdInfo->SS3);
            ASSERT_EFI_ERROR(Status);
            
            if (EFI_ERROR(Status)) return Status;
            
            Status = UpdateAslNameObject(Dsdt, "SS2", (UINT64)AmlUpdInfo->SS2);
            ASSERT_EFI_ERROR(Status);
            
            if (EFI_ERROR(Status)) return Status;
            
            Status = UpdateAslNameObject(Dsdt, "SS1", (UINT64)AmlUpdInfo->SS1);
            ASSERT_EFI_ERROR(Status);
#if SMCPKG_SUPPORT
            Status = UpdateAslNameObject(Dsdt, "W7KF", (UINT64)AmlUpdInfo->W7KF);
            ASSERT_EFI_ERROR(Status);

            Status = UpdateAslNameObject(Dsdt, "PEER", (UINT64)AmlUpdInfo->PEER);
            ASSERT_EFI_ERROR(Status);
#endif  //SMCPKG_SUPPORT
            
            if (EFI_ERROR(Status)) return Status;

            Dsdt->Checksum = 0;
            Dsdt->Checksum = ChsumTbl((UINT8*)Dsdt, Dsdt->Length);

            gxFacs->HardwareSignature = Dsdt->Checksum;

            gFacs->HardwareSignature = Dsdt->Checksum;
        }
    }
    
    return Status;
    
    
}//end of UpdateAml

/**
    This function Hides Legacy Resources from OS by destroing _PRS method
    in each Legacy Device ASL Object in DSDT

         
    @param AmlUpdInfo pointer to ACPI_AML_UPD_INFO structure

          
    @retval VOID

**/
#if GenericSio_SUPPORT
VOID LockLegacyRes (ACPI_AML_UPD_INFO   *AmlUpdInfo)

{

    UINTN               HandleCnt, j, i;
    EFI_HANDLE          *HandleBuffer = NULL;
    SIO_DEV2            *SpIoDev;
    ASL_OBJ_INFO        AslObj={0};
    ACPI_HDR            *Dsdt;
    EFI_STATUS          Status;
    EFI_SIO_DATA        *EfiSioData;

    Status = pBS->LocateHandleBuffer(ByProtocol,&gEfiSioProtocolGuid, NULL, &HandleCnt, &HandleBuffer);
    //Locate all handles for SIO
    
    if (HandleBuffer == NULL) return;
    
    for (j = 0; j < HandleCnt; j++)
    {
        Status = pBS->HandleProtocol(HandleBuffer[j], &gEfiSioProtocolGuid, &EfiSioData);
        if (EFI_ERROR(Status)) continue;
        SpIoDev = EfiSioData->Owner;
        if (SpIoDev->DeviceInfo->AslName[0])
//        TRACE ((-1,"Found SIO Protocol. Name: %s\n",(UINT32)SpIoDev->DeviceInfo->AslName));
            // If this device has ASL Name and is present in DSDT
        {
            for (i = 0; i < 2; i++)
            {
                if (i) Dsdt = (ACPI_HDR*)AmlUpdInfo->Dsdt1Addr;
                else Dsdt = (ACPI_HDR*)AmlUpdInfo->Dsdt2Addr;
                
                if (Dsdt)
                {
//                    TRACE ((-1,"Looking DSDT for Name: %s\n", SpIoDev->DeviceInfo->AslName));
                    Status = GetAslObj((UINT8*)Dsdt+sizeof(ACPI_HDR),
                                       Dsdt->Length-sizeof(ACPI_HDR)-1, &SpIoDev->DeviceInfo->AslName[0],
                                       otDevice, &AslObj);
                    // Get Asl object associated with this Legacy device
                    ASSERT_EFI_ERROR(Status);
                    
//                    TRACE ((-1,"Going to hide object Data statr= %x, Length= %x\n",AslObj.DataStart, AslObj.Length));
                    if (!EFI_ERROR(Status)) HideAslMethodFromOs (&AslObj, "_PRS");
                    
                    // Lock this Device by destroing _PRS method of this object
                }
            }
        }
    }
    
    for (i = 0; i < 2; i++)
    {
        if (i) Dsdt = (ACPI_HDR*)AmlUpdInfo->Dsdt1Addr;
        else Dsdt = (ACPI_HDR*)AmlUpdInfo->Dsdt2Addr;
        
        if (Dsdt)
        {
            Dsdt->Checksum = 0;
            Dsdt->Checksum = ChsumTbl((UINT8*)Dsdt, Dsdt->Length);
        }
    }
    
    pBS->FreePool(HandleBuffer);
}
#endif
/**
    This function will be called when ReadyToBoot event will be signaled and
    will update IO devices status and then update AML binary. It allso publish all
    ACPI tables.

         
    @param Event signalled event
    @param Context calling context

          
    @retval VOID

**/

VOID CollectAmlUpdInfo(EFI_EVENT Event, VOID *Context)
{
    ACPI_AML_UPD_INFO   *Aui=Context;
    FACP_20     *xFacp = NULL;
//    ACPI_HDR    *Dsdt1 = NULL, *Dsdt2 = NULL;
//    FACP32      *Facp = NULL;
    FACP_20     *Facp = NULL;
    UINTN               i;
    EFI_STATUS          Status = EFI_SUCCESS;
    SETUP_DATA          *SetupData=NULL;
#if GenericSio_SUPPORT
    UINTN SioDevStatusVarSize = sizeof(SIO_DEV_STATUS);
    EFI_GUID SioDevStatusVarGuid = SIO_DEV_STATUS_VAR_GUID;
#endif
//-------------------------------------------
    //Init Setup Sleep States with Default Values;
    TRACE((-1,"IN Collect AML Info: %x\n",0));
    
    //we need to recreate MADT table instances on READY_TO_BOOT event.
    for (i=0; i<gAcpiData.AcpiEntCount; i++)
    {
        ACPI_TBL_ITEM   *AcpiTable = gAcpiData.AcpiEntries[i];
        //-------------------------------------------
        
        if (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == FACP_SIG)
        {
            if (gAcpiData.AcpiEntries[i]->AcpiTblVer == EFI_ACPI_TABLE_VERSION_1_0B)
                Facp = (FACP_20*) gAcpiData.AcpiEntries[i]->BtHeaderPtr; // Find FACP for V 1.1
            else
                xFacp = (FACP_20*) gAcpiData.AcpiEntries[i]->BtHeaderPtr; // Find FACP for V 2+
        }
        
        if (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == DSDT_SIG)
        {
            if (gAcpiData.AcpiEntries[i]->AcpiTblVer == EFI_ACPI_TABLE_VERSION_1_0B)
                Aui->Dsdt1Addr = (UINT64) gAcpiData.AcpiEntries[i]->BtHeaderPtr; // Find DSDT for V 1.1
            else
                Aui->Dsdt2Addr = (UINT64) gAcpiData.AcpiEntries[i]->BtHeaderPtr; // Find DSDT for V 2+
        }
#if ACPI_APIC_TBL == 1
        if (AcpiTable->BtHeaderPtr->Signature == APIC_SIG )
        {

			UINTN           AcpiVer;
            switch (AcpiTable->AcpiTblVer)
            {
                case EFI_ACPI_TABLE_VERSION_1_0B: AcpiVer=1; break;
                case EFI_ACPI_TABLE_VERSION_2_0:  AcpiVer=2; break;
                case EFI_ACPI_TABLE_VERSION_3_0:  AcpiVer=3; break;
                case EFI_ACPI_TABLE_VERSION_4_0:  AcpiVer=4; break;
                default: AcpiVer=0;
            }
           
            gAcpiData.AcpiLength -= AcpiTable->BtHeaderPtr->Length;
            
            pBS->FreePool(AcpiTable->BtHeaderPtr);
            AcpiTable->BtHeaderPtr=NULL;
            
            Status = BuildMadtAll (AcpiVer, &AcpiTable->BtHeaderPtr);
            ASSERT_EFI_ERROR(Status);
            
            if (EFI_ERROR(Status)) return;
            
            gAcpiData.AcpiLength += AcpiTable->BtHeaderPtr->Length;

        }
#endif
    }
    
    
    ASSERT_EFI_ERROR(Status);
    TRACE((-1,"IN Collect AML Info after PT: %x\n",Status));
    
    Aui->SS1=DEFAULT_SS1;
    Aui->SS2=DEFAULT_SS2;
    Aui->SS3=DEFAULT_SS3;
    Aui->SS4=DEFAULT_SS4;
    
    Status = GetEfiVariable(L"Setup",&gSetupGuid,NULL,&i,&SetupData);
    
    if (!EFI_ERROR(Status))
    {
#if SMCPKG_SUPPORT
        Aui->PEER = SetupData->AerEnable;
#endif  //SMCPKG_SUPPORT
        //Such Variable exists so use customer choices
        if (!SetupData->AcpiAuto)
        {
            Aui->SS4=SetupData->AcpiHibernate;
            
            switch (SetupData->AcpiSleepState)
            {
                case 0:
                    Aui->SS3=0;
                    Aui->SS1=0;
                    break;
                case 1:
                    Aui->SS1=1;
                    Aui->SS3=0;
                    break;
                case 2:
                    Aui->SS3=1;
#if A_S1 == 1
                    Aui->SS1=1;
#else
                    Aui->SS1=0;
#endif
                    break;
            }
        }
    }
    
#if SMCPKG_SUPPORT
    if(SetupData->SmcW7USB)
        Aui->W7KF = 1;
#endif  //SMCPKG_SUPPORT
    gAcpiIaBootArch &= (~((UINT16)(IA_LEGACY | IA_8042)));// clear first 2 bits of gAcpiIaBootArch,
    // while preserving others. This 2 bits will be updated later.
#if GenericSio_SUPPORT
    Status = pRS->GetVariable(SIO_DEV_STATUS_VAR_NAME, &SioDevStatusVarGuid, NULL,
                              &SioDevStatusVarSize, &Aui->SioDevStatusVar.DEV_STATUS);
                              
    if (EFI_ERROR(Status)) Aui->SioDevStatusVar.DEV_STATUS = 0;

    
    //Dynamically update IA_PC_BOOT_ARCHITECTURE flag based on SIO_DEV_STATUS_VAR
    if (
        (Aui->SioDevStatusVar.SerialA == 1 ) ||
        (Aui->SioDevStatusVar.SerialB == 1 ) ||
        (Aui->SioDevStatusVar.Lpt == 1 ) ||
        (Aui->SioDevStatusVar.Fdd == 1 ) ||
        (Aui->SioDevStatusVar.Game1 == 1 ) ||
        (Aui->SioDevStatusVar.Game2 == 1 )
    )
        gAcpiIaBootArch |= IA_LEGACY;
        
    if (
        (Aui->SioDevStatusVar.Key60_64 == 1) ||
        (Aui->SioDevStatusVar.Ps2Mouse == 1) ||
        (Aui->SioDevStatusVar.Ec62_66 == 1)
    )
        gAcpiIaBootArch |= IA_8042;        
#endif  
    
    if ( xFacp != NULL)
    {
        xFacp->IAPC_BOOT_ARCH &= gAcpiIaBootArch;
        xFacp->Header.Checksum = 0;
        xFacp->Header.Checksum = ChsumTbl((UINT8*)&xFacp->Header, xFacp->Header.Length);
    }
    
    if ( Facp != NULL )
    {
        //ACPI v1.0b don't have IAPC_BOOT_ARCH flag field!
        //Legacy free extension has but.. 1.1b Legacy free extension has. REV==2 and higher
        if (Facp->Header.Revision > ACPI_REV1)
        {
            Facp->IAPC_BOOT_ARCH &= gAcpiIaBootArch;
            Facp->Header.Checksum = 0;
            Facp->Header.Checksum = ChsumTbl((UINT8*)&Facp->Header, Facp->Header.Length);
        }
    }
    
    Status=UpdateAml(Aui);
    ASSERT_EFI_ERROR(Status);
#if GenericSio_SUPPORT  
    if (SetupData->AcpiLockLegacyRes) LockLegacyRes (Aui); // LockLegacyDev
#endif    
    if (!gForceAcpi1) Status = PublishTbl (1, 1);
    else Status = PublishTbl (1, 0);
    ASSERT_EFI_ERROR(Status);
    gPublishedOnReadyToBoot = 1;
    // Do not need to close event because we can return here with changies in configuration
}// end of CollectAmlUpdInfo

/**
    This function builds mandatory ACPI tables

         
    @param VOID

          
    @retval EFI_SUCCESS Function executed successfully, ACPI_SUPPORT_PROTOCOL installed
    @retval EFI_ABORTED Dsdt table not found or table publishing failed
    @retval EFI_ALREADY_STARTED driver already started
    @retval EFI_OUT_OF_RESOURCES not enough memory to perform operation

    @note  
  This function also creates ReadyToBoot event to update AML objects before booting

**/

EFI_STATUS BuildMandatoryAcpiTbls ()
{
    EFI_STATUS      Status = EFI_SUCCESS;
#if DSDT_BUILD
    ACPI_HDR        *Dsdt2Ptr = NULL;
#endif
    static EFI_GUID Acpisupguid = EFI_ACPI_SUPPORT_GUID;
//	    static EFI_GUID AcpiTableProtocolGuid = EFI_ACPI_TABLE_PROTOCOL_GUID;
    UINTN           AcpiVer;
    EFI_ACPI_TABLE_VERSION  EfiAcpiVer;
    ACPI_TBL_ITEM   *AcpiTable = NULL;
#if FORCE_TO_ACPI1_SETUP_ENABLE
    SETUP_DATA      *SetupData = NULL;
    UINTN           SetupSize = 0;
#endif
#if ATAD_SUPPORT == 1
	EFI_GUID    AtadSmiGuid = ATAD_SMI_GUID;
	UINTN AtadVarSize = sizeof(AtadBuffPtr);
#endif
//------------------------
  
    AcpiVer = 2;
    EfiAcpiVer = EFI_ACPI_TABLE_VERSION_2_0;
    
    if (ACPI_SPEC_VERSION >= 30)
    {
        AcpiVer = 3;
        EfiAcpiVer = EFI_ACPI_TABLE_VERSION_3_0;
    }
    
    if (ACPI_SPEC_VERSION >= 40)
    {
        AcpiVer = 4;
        EfiAcpiVer = EFI_ACPI_TABLE_VERSION_4_0;
    }
#if FORCE_TO_ACPI1_SETUP_ENABLE
    Status = GetEfiVariable(L"Setup",&gSetupGuid,NULL,&SetupSize,&SetupData);
    ASSERT_EFI_ERROR(Status);
    if (!EFI_ERROR (Status)) 
        if (SetupData->ForceToAcpi1 ==1) 
        {
            AcpiVer = gForceAcpi1 = 1;
            EfiAcpiVer = EFI_ACPI_TABLE_VERSION_1_0B;
        }
#endif
#if ATAD_SUPPORT == 1

    Status = pBS->AllocatePool(EfiRuntimeServicesData, 4, &AtadBuffPtr);
    if (!EFI_ERROR(Status) && AtadBuffPtr)
    {
        Status = pRS->SetVariable ( L"AtadSmiBuffer",
        							&AtadSmiGuid,
        							EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
        							AtadVarSize,
                                    &AtadBuffPtr );
        ASSERT_EFI_ERROR(Status);
    }

#endif
    PcdSet64 (PcdAcpiDefaultOemTableId, *(UINT64*)&ACPI_OEM_TBL_ID[0]);
    Status = BuildFacs ();
    ASSERT_EFI_ERROR(Status);

    if (EFI_ERROR (Status)) return EFI_OUT_OF_RESOURCES;
#if FACP_BUILD    
    AcpiTable = MallocZ (sizeof (ACPI_TBL_ITEM));
    ASSERT(AcpiTable);
    
    if (!AcpiTable) return EFI_OUT_OF_RESOURCES;
    
    Status = BuildFacpAll (1, &AcpiTable->BtHeaderPtr);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR (Status)) return EFI_OUT_OF_RESOURCES;
    
    AcpiTable->AcpiTblVer = EFI_ACPI_TABLE_VERSION_1_0B;
    Status = AppendItemLst ((T_ITEM_LIST*)&gAcpiData, (VOID*) AcpiTable);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
    
    gAcpiData.AcpiLength += gAcpiData.AcpiEntries[gAcpiData.AcpiEntCount-1]->BtHeaderPtr->Length;

    if (!gForceAcpi1)
    {
        AcpiTable = MallocZ (sizeof (ACPI_TBL_ITEM));
        ASSERT(AcpiTable);
    
        if (!AcpiTable) return EFI_OUT_OF_RESOURCES;
    
        Status = BuildFacpAll (AcpiVer, &AcpiTable->BtHeaderPtr);
        ASSERT_EFI_ERROR(Status);
    
        if (EFI_ERROR (Status)) return EFI_OUT_OF_RESOURCES;
    
        AcpiTable->AcpiTblVer = EfiAcpiVer;
        Status = AppendItemLst ((T_ITEM_LIST*)&gAcpiData, (VOID*) AcpiTable);
        ASSERT_EFI_ERROR(Status);
    
        if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
    
        gAcpiData.AcpiLength += gAcpiData.AcpiEntries[gAcpiData.AcpiEntCount-1]->BtHeaderPtr->Length;
    }

    TRACE((-1,"IN ACPI 1: %x\n", Status));
#endif  
#if DSDT_BUILD
    Status = GetDsdtFv(&Dsdt2Ptr);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return EFI_ABORTED;
    
    if (Dsdt2Ptr != NULL)
    {
        TRACE((-1,"DSDT21 addres 0x%lX; -> %r \n", Dsdt2Ptr, Status));
        Dsdt2Ptr->Checksum = 0;
        Dsdt2Ptr->Checksum = ChsumTbl((UINT8*)Dsdt2Ptr, Dsdt2Ptr->Length);
        AcpiTable = MallocZ (sizeof (ACPI_TBL_ITEM));
        ASSERT(AcpiTable);
        
        if (!AcpiTable) return EFI_OUT_OF_RESOURCES;
        
        AcpiTable->BtHeaderPtr = Dsdt2Ptr;
        AcpiTable->AcpiTblVer = EFI_ACPI_TABLE_VERSION_2_0;
        Status = AppendItemLst ((T_ITEM_LIST*)&gAcpiData, (VOID*) AcpiTable);
        ASSERT_EFI_ERROR(Status);
        
        if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
        
        gAcpiData.AcpiLength += gAcpiData.AcpiEntries[gAcpiData.AcpiEntCount-1]->BtHeaderPtr->Length;
    }
    else return EFI_ABORTED;
    
    Status = UpdateFacp ();
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return EFI_ABORTED;
#endif
#if ACPI_APIC_TBL == 1        
    AcpiTable = MallocZ (sizeof (ACPI_TBL_ITEM));
    ASSERT(AcpiTable);
    
    if (!AcpiTable) return EFI_OUT_OF_RESOURCES;
    
    Status = BuildMadtAll (AcpiVer, &AcpiTable->BtHeaderPtr);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
    
    AcpiTable->AcpiTblVer = EfiAcpiVer;
    Status = AppendItemLst ((T_ITEM_LIST*)&gAcpiData, (VOID*) AcpiTable);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
    
    gAcpiData.AcpiLength += gAcpiData.AcpiEntries[gAcpiData.AcpiEntCount-1]->BtHeaderPtr->Length;
#endif    

//------ Performance Measurment ------------------------------------
#if FPDT_BUILD
    AcpiTable = MallocZ (sizeof (ACPI_TBL_ITEM));
    ASSERT(AcpiTable);
    
    if (AcpiTable)
    {
        Status = BuildFPDT (AcpiVer, &AcpiTable->BtHeaderPtr);

        if (!EFI_ERROR(Status)) 
        {
            AcpiTable->AcpiTblVer = EfiAcpiVer;
            Status = AppendItemLst ((T_ITEM_LIST*)&gAcpiData, (VOID*) AcpiTable);
            ASSERT_EFI_ERROR(Status);
            gAcpiData.AcpiLength += gAcpiData.AcpiEntries[gAcpiData.AcpiEntCount-1]->BtHeaderPtr->Length;
        }
    }
#endif
//------ Performance Measurment End --------------------------------
    
    return EFI_SUCCESS;
}

/**
    This function craeate Ready to Boot event

         
    @param VOID

    @retval 
        EFI_STATUS, based on result


**/

EFI_STATUS AcpiReadyToBootEvent() 
{
	EFI_EVENT               EvtReadyToBoot;
	
    gAuiGlob = MallocZ(sizeof(ACPI_AML_UPD_INFO));
    
    if (gAuiGlob == NULL) 
    	return EFI_OUT_OF_RESOURCES;
    
    return  CreateReadyToBootEvent(
                 TPL_CALLBACK, CollectAmlUpdInfo, gAuiGlob, &EvtReadyToBoot
             );
}   

/**
    Library constructor - init Ami Lib

         
    @param ImageHandle Image handle
    @param SystemTable pointer to system table

    @retval 
        EFI_STATUS, based on result

    @note  
  This function also creates ReadyToBoot event to update AML objects before booting

**/
EFI_STATUS EFIAPI AmiAcpiLibConstructor (IN EFI_HANDLE ImageHandle,IN EFI_SYSTEM_TABLE *SystemTable)
{
	  InitAmiLib(ImageHandle,SystemTable);
	  return EFI_SUCCESS;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
