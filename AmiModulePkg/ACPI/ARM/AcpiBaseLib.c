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


/** @file ARM/AcpiBaseLib.c
    Main ACPI Driver File. It has ACPI Driver entry point,
    ACPISupport Protocol and ACPITable Protocol.

**/
//**********************************************************************

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
#include <Library/DebugLib.h>
#include <Guid/ArmMpCoreInfo.h>
#if ATAD_SUPPORT == 1
#include "AtadSmi.h"
#endif
#include<IndustryStandard/Acpi60.h>
#include <Library/PcdLib.h>
//--------------------------------------
//Some Global vars
UINTN 					gDsdtPages = (UINTN)-1;// Sygnal to AcpiCore that DSDT memory was allocated in ARM instance.
extern ACPI_DB          gAcpiData;
FACS_20                 *gxFacs;
extern RSDT_PTR_20      *gRsdtPtrStr;
UINTN                   gAcpiTblPages = 0;
EFI_GUID                gAcpi20TAbleGuid = EFI_ACPI_20_TABLE_GUID;
extern UINT8            gPublishedOnReadyToBoot;
#if ATAD_SUPPORT == 1
    VOID  *AtadBuffPtr = NULL;
#endif


UINT8    ACPI_OEM_ID[6]     = ACPI_OEM_ID_MAK;     
UINT8    ACPI_OEM_TBL_ID[8] = ACPI_OEM_TBL_ID_MAK; 


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
    This function allocates memory for and fills FACP table v 1.1+ with
    predefined values from SDL tokens
         
    @param TablVer version of FACP table
    @param TablPtr pointer to memory, where the FACP table will resides.
        	Filled by this procedure
    @retval 
  EFI_OUT_OF_RESOURCES - Memory for the table could not be allocated
  EFI_SUCCESS - Table was successfully build

**/

#if ARM_FACP_BUILD 
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
    Facp->XPm1aEvtBlk.AccessSize  = 2;
    Facp->XPm1aEvtBlk.Address    = PM1A_EVT_BLK_ADDRESS;
    
    // PM1a_CNT_BLK GAS_20 structure
    Facp->XPm1aCntBlk.AddressSpaceId  = PM1A_CNT_BLK_TYPE;
    Facp->XPm1aCntBlk.RegisterBitWidth = PM1A_CNT_BLK_BITWIDTH;
    Facp->XPm1aCntBlk.RegisterBitOffset = PM1A_CNT_BLK_BITOFFSET;
    Facp->XPm1aCntBlk.AccessSize  = 2;
    Facp->XPm1aCntBlk.Address    = PM1A_CNT_BLK_ADDRESS;
    
    // PM1b_EVT_BLK GAS_20 structure
    Facp->XPm1bEvtBlk.AddressSpaceId  = PM1B_EVT_BLK_TYPE;
    Facp->XPm1bEvtBlk.RegisterBitWidth = PM1B_EVT_BLK_BITWIDTH;
    Facp->XPm1bEvtBlk.RegisterBitOffset = PM1B_EVT_BLK_BITOFFSET;
    Facp->XPm1bEvtBlk.AccessSize  = 2;
    Facp->XPm1bEvtBlk.Address    = PM1B_EVT_BLK_ADDRESS;
    
    // PM1b_CNT_BLK GAS_20 structure
    Facp->XPm1bCntBlk.AddressSpaceId  = PM1B_CNT_BLK_TYPE;
    Facp->XPm1bCntBlk.RegisterBitWidth = PM1B_CNT_BLK_BITWIDTH;
    Facp->XPm1bCntBlk.RegisterBitOffset = PM1B_CNT_BLK_BITOFFSET;
    Facp->XPm1bCntBlk.AccessSize    = 2;
    Facp->XPm1bCntBlk.Address    = PM1B_CNT_BLK_ADDRESS;
    
    // PM1b_CNT_BLK GAS_20 structure
    Facp->XPm2CntBlk.AddressSpaceId   = PM2_CNT_BLK_TYPE;
    Facp->XPm2CntBlk.RegisterBitWidth = PM2_CNT_BLK_BITWIDTH;
    Facp->XPm2CntBlk.RegisterBitOffset  = PM2_CNT_BLK_BITOFFSET;
    Facp->XPm2CntBlk.AccessSize    = 1;
    Facp->XPm2CntBlk.Address     = PM2_CNT_BLK_ADDRESS;
    
    Facp->XPmTmrBlk.AddressSpaceId    = PM_TMR_BLK_TYPE;
    Facp->XPmTmrBlk.RegisterBitWidth  = PM_TMR_BLK_BITWIDTH;
    Facp->XPmTmrBlk.RegisterBitOffset   = PM_TMR_BLK_BITOFFSET;
    Facp->XPmTmrBlk.AccessSize    = 3;
    Facp->XPmTmrBlk.Address      = PM_TMR_BLK_ADDRESS;
    
    Facp->XGpe0Blk.AddressSpaceId      = GPE0_BLK_TYPE;
    Facp->XGpe0Blk.RegisterBitWidth    = GPE0_BLK_BITWIDTH;
    Facp->XGpe0Blk.RegisterBitOffset     = GPE0_BLK_BITOFFSET;
    Facp->XGpe0Blk.AccessSize    = 1;
    Facp->XGpe0Blk.Address        = GPE0_BLK_ADDRESS;
    
    Facp->XGpe1Blk.AddressSpaceId      = GPE1_BLK_TYPE;
    Facp->XGpe1Blk.RegisterBitWidth    = GPE1_BLK_BITWIDTH;
    Facp->XGpe1Blk.RegisterBitOffset     = GPE1_BLK_BITOFFSET;
    Facp->XGpe1Blk.AccessSize    = 1;
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

#if GTDT_BUILD
/**
    This function allocates memory for and fills GTDT struscure.
         
    @param TablVer Version of GTDT table
    @param TablPtr - Pointer to memory, where the GTDT table will resides.
        	Filled by this procedure
    @retval 
  EFI_OUT_OF_RESOURCES - Memory for the table could not be allocated
  EFI_SUCCESS - Table was successfully build

**/

EFI_STATUS BuildGTDT (IN UINTN TablVer, OUT ACPI_HDR **TablPtr)

{
    EFI_ACPI_5_0_GENERIC_TIMER_DESCRIPTION_TABLE    *GTDT;
    UINTN	i;

    if (TablVer<1 || TablVer>4) return EFI_INVALID_PARAMETER;
    *TablPtr = MallocZ (sizeof(EFI_ACPI_5_0_GENERIC_TIMER_DESCRIPTION_TABLE));

    if ((*TablPtr)==NULL)
    {
        ASSERT(*TablPtr);
        return EFI_OUT_OF_RESOURCES;
    }

    GTDT = (EFI_ACPI_5_0_GENERIC_TIMER_DESCRIPTION_TABLE*)*TablPtr;
    GTDT->Header.Signature = EFI_ACPI_5_0_GENERIC_TIMER_DESCRIPTION_TABLE_SIGNATURE;
	GTDT->Header.Length = sizeof(EFI_ACPI_5_0_GENERIC_TIMER_DESCRIPTION_TABLE);
	GTDT->Header.Revision = EFI_ACPI_5_0_GENERIC_TIMER_DESCRIPTION_TABLE_REVISION;
	
	for (i=0; i<6; i++) GTDT->Header.OemId[i] = ACPI_OEM_ID[i];

	GTDT->Header.CreatorId = CREATOR_ID_AMI;
	GTDT->Header.CreatorRevision = CREATOR_REV_MS;
	
	//for (i=0; i<8; i++) GTDT->Header.OemTableId[i] = ACPI_OEM_TBL_ID[i];
	MemCpy(&GTDT->Header.OemTableId, &ACPI_OEM_TBL_ID, sizeof(ACPI_OEM_TBL_ID));

	GTDT->Header.OemRevision = 1;

	//Fill GTDT members
	GTDT->PhysicalAddress			= GTDT_PHYSICAL_ADDRESS; 
	GTDT->GlobalFlags				= GTDT_GLOBAL_FLAGS; 
	GTDT->SecurePL1TimerGSIV		= GTDT_SECURE_PL1_TIMER_GSIV; 
	GTDT->SecurePL1TimerFlags		= GTDT_SECURE_PL1_TIMER_FLAGS; 
	GTDT->NonSecurePL1TimerGSIV		= GTDT_NON_SECURE_PL1_TIMER_GSIV; 
	GTDT->NonSecurePL1TimerFlags    = GTDT_NON_SECURE_PL1_TIMER_FLAGS;
	GTDT->VirtualTimerGSIV			= GTDT_VIRTUAL_TIMER_GSIV;
	GTDT->VirtualTimerFlags			= GTDT_VIRTUAL_TIMER_FLAGS;
	GTDT->NonSecurePL2TimerGSIV		= GTDT_NON_SECURE_PL2_TIMER_GSIV;
	GTDT->NonSecurePL2TimerFlags    = GTDT_NON_SECURE_PL2_TIMER_FLAGS;
	
	GTDT->Header.Checksum = ChsumTbl((UINT8*)GTDT, GTDT->Header.Length);

    return EFI_SUCCESS;
}
#endif
#if ARM_MADT_BUILD

EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER		*Hdr;
EFI_ACPI_5_0_GIC_STRUCTURE      						*Gic;
EFI_ACPI_5_0_GIC_DISTRIBUTOR_STRUCTURE 					*GicDistributor;

/**
    This function allocates memory for and fills MADT structure.
         
    @param TablVer Version of MADT table
    @param TablPtr Pointer to memory, where the MADT table will resides.
        	Filled by this procedure
    @retval 
  EFI_OUT_OF_RESOURCES - Memory for the table could not be allocated
  EFI_SUCCESS - Table was successfully build

**/

EFI_STATUS BuildMADT (IN UINTN TablVer, OUT ACPI_HDR **TablPtr)

{
    UINT8 					i;
    UINTN					Index;
    ARM_PROCESSOR_TABLE   	*ArmProcessorTable;
	ARM_CORE_INFO    	    *ArmCoreInfoTable;
	UINT8					*MADT;
	
	for (Index=0; Index < pST->NumberOfTableEntries; Index++) {
		// Check for correct GUID type
		if (CompareGuid (&gArmMpCoreInfoGuid, &(pST->ConfigurationTable[Index].VendorGuid))) {
			// Get pointer to ARM processor table
			ArmProcessorTable = (ARM_PROCESSOR_TABLE *)pST->ConfigurationTable[Index].VendorTable;
			ArmCoreInfoTable = ArmProcessorTable->ArmCpus;
			break;
		}
	}

    if (TablVer<1 || TablVer>4) return EFI_INVALID_PARAMETER;
    
	*TablPtr = MallocZ (sizeof(EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER) + sizeof(EFI_ACPI_5_0_GIC_STRUCTURE) * ArmProcessorTable->NumberOfEntries + sizeof(EFI_ACPI_5_0_GIC_DISTRIBUTOR_STRUCTURE) );
    if ((*TablPtr)==NULL)
    {
        ASSERT(*TablPtr);
        return EFI_OUT_OF_RESOURCES;
    }

	MADT = (UINT8*)*TablPtr;	
    Hdr = (EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER*)MADT;
    PrepareHdr20(APIC_SIG, &(Hdr->Header), TablVer);
	Hdr->Header.Length = sizeof(EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER) + sizeof(EFI_ACPI_5_0_GIC_STRUCTURE) * ArmProcessorTable->NumberOfEntries + sizeof(EFI_ACPI_5_0_GIC_DISTRIBUTOR_STRUCTURE) ;
    Hdr->LocalApicAddress = 0;
    Hdr->Flags = 1;

	MADT = MADT + sizeof(EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);
    //Fill GIC members
    for(i=0; i< ArmProcessorTable->NumberOfEntries; i++)
    {
		Gic = (EFI_ACPI_5_0_GIC_STRUCTURE *)MADT;
        Gic->Type = EFI_ACPI_5_0_GIC;
        Gic->Length = sizeof(EFI_ACPI_5_0_GIC_STRUCTURE);
        Gic->GicId = i;
        Gic->AcpiProcessorUid = i;
        Gic->Flags = ARM_MADT_GIC_FLAGS;
        Gic->ParkedAddress = ArmCoreInfoTable[i].MailboxSetAddress; 
        Gic->PhysicalBaseAddress = (UINT64) PcdGet32(PcdGicInterruptInterfaceBase);
		MADT = MADT + sizeof(EFI_ACPI_5_0_GIC_STRUCTURE);
    }

    //Fill GIC distrubutor
    GicDistributor = (EFI_ACPI_5_0_GIC_DISTRIBUTOR_STRUCTURE *)MADT;
    GicDistributor->Type = EFI_ACPI_5_0_GICD;
    GicDistributor->Length = sizeof(EFI_ACPI_5_0_GIC_DISTRIBUTOR_STRUCTURE);
    GicDistributor->GicId = ARM_MADT_GIC_DISTR_ID;
    GicDistributor->PhysicalBaseAddress = (UINT64) PcdGet32(PcdGicDistributorBase);
    
    Hdr->Header.Checksum = ChsumTbl((UINT8*)*TablPtr, Hdr->Header.Length);
    
    return EFI_SUCCESS;
}
#endif
/**
    Allocates ACPI NVS memory and builds FACS table from values,
    defined by SDL tokens

    @param VOID



    @retval EFI_OUT_OF_RESOURCES not enough memory
    @retval EFI_SUCCESS FACS table were successfully build

**/

EFI_STATUS BuildFacs ()
{
    EFI_STATUS      Status;
    UINTN           Size = sizeof(FACS_20);
    
    

    Status = pBS->AllocatePool(EfiACPIMemoryNVS, Size+64, (VOID **)&gxFacs);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
    
	gxFacs = (FACS_20*)((UINTN) gxFacs + 64);
    gxFacs = (FACS_20*)((UINTN) gxFacs & (~0x3F));
    pBS->SetMem(gxFacs, Size, 0);
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
	
    //In current AmiBoardInfo implementation separate instance of
    //DSDT for ACPI version 1.1b  DOES OT SUPPORTED!
	Status=AmiSdlInitBoardInfo();
	if(EFI_ERROR(Status)){
		TRACE((-1,"AcpiCore: No AmiBoardInfo Found: Status=%r\n", Status));
		return EFI_NOT_FOUND;
	}

    *Dsdt2 = Malloc(((ACPI_HDR*)gAmiBoardInfo2Protocol->BrdAcpiInfo)->Length);
    ASSERT(*Dsdt2);
    
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

    @param VOID

    @retval EFI_SUCCESS Function executed successfully
    @retval EFI_ABORTED Error


    @note    Modifies  gAcpiData

**/

EFI_STATUS UpdateFacp () 
{
    ACPI_HDR    *Facp2 = NULL, *Dsdt2 = NULL;
    UINTN       i;
    
    for (i = 0; i < gAcpiData.AcpiEntCount; i++)
    {
        if (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == FACP_SIG)
        {
        	Facp2 = gAcpiData.AcpiEntries[i]->BtHeaderPtr; // Find FACP for V 2+
        }
        
        if (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == DSDT_SIG)
        {
        	Dsdt2 = gAcpiData.AcpiEntries[i]->BtHeaderPtr; // Find DSDT for V 2+
        }
    }
    
    
    if (Facp2 == NULL)
    	return EFI_SUCCESS;
    
    ((FACP_20*) Facp2)->FIRMWARE_CTRL = 0;
    
    ((FACP_20*) Facp2)->DSDT = 0;
    
    ((FACP_20*) Facp2)->X_FIRMWARE_CTRL = (UINT64) ((UINTN) gxFacs);
    
    if (Dsdt2 != NULL)
    	((FACP_20*) Facp2)->X_DSDT = (UINT64) ((UINTN)Dsdt2);
	else 
	    ((FACP_20*) Facp2)->X_DSDT = 0;
    Facp2->Checksum = 0;
    Facp2->Checksum = ChsumTbl((UINT8*)Facp2, Facp2->Length);
    return EFI_SUCCESS;
}// end of UpdateFacp

/**
    Copys table from position pointed by FromPtr to a position pointed
    by ToPtr (which is in EfiACPIReclaimMemory) and fills RSDT and XSDT
    pointers with ToPtr value


    @param RsdtPtr pionter to RSDT
    @param XsdtPtr pionter to XSDT
    @param FromPtr pointer to a table which should be copyed
    @param ToPtr pointer to EfiACPIReclaimMemory where table should be placed

    @retval  Pointer to the next avaiable space in allocated EfiACPIReclaimMemory
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
    ACPI_HDR    *FacpX = NULL, *DsdtX = NULL;
    RSDT32      *Rsdt = NULL;
    XSDT_20     *Xsdt = NULL;
    UINTN       i, XsdtCnt = 0, SpaceNeeded = 0, XsdtSize = 0;
    EFI_STATUS  Status;
    UINT8       *Ptr = NULL, *Dummy = NULL;
    EFI_PHYSICAL_ADDRESS Memory;
	UINTN       AcpiTblPages = 0;
    
    if ((XsdtBuild == 0)  || (XsdtBuild > 1)) return EFI_ABORTED;

    for (i = 0; i < gAcpiData.AcpiEntCount; i++)
    {
    	XsdtCnt ++;
            
        if (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == FACP_SIG)
        	FacpX = gAcpiData.AcpiEntries[i]->BtHeaderPtr;
                
        if (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == DSDT_SIG)
            DsdtX = gAcpiData.AcpiEntries[i]->BtHeaderPtr;
    }
    
    if ((FacpX == NULL) || (DsdtX == NULL))
    {
    	TRACE((-1,"In PublishTbl: No FacpX or DsdtX! Publishing will be delayed until they will be available. \n"));
       	return EFI_SUCCESS;
    }    
    
    XsdtSize = XsdtBuild * (sizeof(ACPI_HDR) + (XsdtCnt-1) * sizeof(UINT64) + 7); //DSDT does not goes in XSDT (XsdtCnt-1)
    SpaceNeeded = sizeof(RSDT_PTR_20) + XsdtSize + gAcpiData.AcpiLength + (gAcpiData.AcpiEntCount + 1) * 8;
    AcpiTblPages = gAcpiTblPages;
    gAcpiTblPages = EFI_SIZE_TO_PAGES(SpaceNeeded);

    Status = pBS->AllocatePages(AllocateAnyPages, EfiACPIReclaimMemory, gAcpiTblPages , &Memory);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) 
	{
		gAcpiTblPages = AcpiTblPages;
		return EFI_OUT_OF_RESOURCES;
	}
    if (gRsdtPtrStr != NULL) 
    {
    	Status = pBS->FreePages((EFI_PHYSICAL_ADDRESS)(UINTN) gRsdtPtrStr, AcpiTblPages);
        ASSERT_EFI_ERROR(Status);
    }

    gRsdtPtrStr = (RSDT_PTR_20*)Memory;
    
    pBS->SetMem(gRsdtPtrStr, SpaceNeeded, 0);    
    
 

    Dummy = (UINT8*) ((UINT8*)gRsdtPtrStr + sizeof(RSDT_PTR_20) + 7);
    Dummy = (UINT8*)((UINTN) Dummy & (~0x7));
    Xsdt = (XSDT_20*) Dummy;
        
    Ptr = (UINT8*) ((UINT8*)gRsdtPtrStr + sizeof(RSDT_PTR_20) + XsdtSize + 7);
    Ptr = (UINT8*)((UINTN) Ptr & (~0x7));

    Dummy = FillAcpiStr (NULL, Xsdt, (VOID*) FacpX,( VOID*) Ptr);
    FacpX = (ACPI_HDR*) Ptr;
    Ptr = Dummy;
        
    pBS->CopyMem((VOID*)Ptr, (VOID*)DsdtX, DsdtX->Length);
    DsdtX = (ACPI_HDR*) Ptr;
    Ptr += (DsdtX->Length + 7);
    Ptr = (UINT8*)((UINTN) Ptr &(~0x7));

    if (0xFFFFFFFF00000000 & ((UINT64) ((UINTN)gxFacs)))
    {
    	((FACP_20*)FacpX)->FIRMWARE_CTRL = 0;
    	((FACP_20*)FacpX)->X_FIRMWARE_CTRL = (UINT64)((UINTN)gxFacs);
    }
    else
    {
    	((FACP_20*)FacpX)->FIRMWARE_CTRL = (UINT32)((UINTN)gxFacs);
    	((FACP_20*)FacpX)->X_FIRMWARE_CTRL = 0;
    }
    ((FACP_20*)FacpX)->X_DSDT = (UINT64) ((UINTN)DsdtX);
    if (0xFFFFFFFF00000000 & ((UINT64) ((UINTN)DsdtX)))	
    {
    	((FACP_20*)FacpX)->DSDT = 0;   
    }
    else
    {
    	((FACP_20*)FacpX)->DSDT = (UINT32)((UINTN)DsdtX);

    }
    FacpX->Checksum = 0;
    FacpX->Checksum = ChsumTbl((UINT8*)FacpX, FacpX->Length);

    
    for (i = 0; i < gAcpiData.AcpiEntCount; i++)
    {
        if ((gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == FACP_SIG) ||
                (gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == DSDT_SIG) ||
                (gAcpiData.AcpiEntries[i]->AcpiTblVer < EFI_ACPI_TABLE_VERSION_1_0B)) 
        	continue;
        
        Dummy = FillAcpiStr (NULL, Xsdt, (VOID*) gAcpiData.AcpiEntries[i]->BtHeaderPtr,( VOID*) Ptr);        
        if (Dummy != NULL) Ptr = Dummy;
  
    }
    
    if (Ptr > ((UINT8*)gRsdtPtrStr + SpaceNeeded))  Status = EFI_OUT_OF_RESOURCES;
    
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return Status;
    
    gRsdtPtrStr->Signature = RSDP_SIG;

    gRsdtPtrStr->Revision = ACPI_REV2; // 2 for ver ACPI 2.0 and up
    
    gRsdtPtrStr->Length = sizeof(RSDT_PTR_20);//this is the length of entire structure
    
    for (i=0; i<6; i++) gRsdtPtrStr->OEMID[i]=ACPI_OEM_ID[i];
    
    gRsdtPtrStr->XsdtAddr = (UINT64)(UINTN) Xsdt;
    gRsdtPtrStr->Checksum = ChsumTbl((UINT8*)gRsdtPtrStr, 20);
    gRsdtPtrStr->XtdChecksum = ChsumTbl((UINT8*)gRsdtPtrStr, sizeof (RSDT_PTR_20));

    PrepareHdr20 (XSDT_SIG, (ACPI_HDR*) Xsdt, 3);

    Xsdt->Header.Length = (UINT32) (XsdtSize - 7);
    Xsdt->Header.Checksum = 0;
    Xsdt->Header.Checksum = ChsumTbl((UINT8*)Xsdt, Xsdt->Header.Length);

    Status = pBS->InstallConfigurationTable(&gAcpi20TAbleGuid, (VOID*) gRsdtPtrStr);
    TRACE((-1,"Installing ACPI 2.0: %r, %X\n",Status,gRsdtPtrStr));
    
    ASSERT_EFI_ERROR(Status);
    
    return Status;

    
}// end of PublishTbl

/**
    This function builds mandatory ACPI tables

         
    @param VOID

          
    @retval EFI_SUCCESS Function executed successfully, ACPI_SUPPORT_PROTOCOL installed
    @retval EFI_ABORTED Dsdt table not found or table publishing failed
    @retval EFI_ALREADY_STARTED driver already started
    @retval EFI_OUT_OF_RESOURCES not enough memory to perform operation

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
  
	AcpiVer = 4;
    EfiAcpiVer = EFI_ACPI_TABLE_VERSION_4_0;


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
#if ARM_FACP_BUILD    
    AcpiTable = MallocZ (sizeof (ACPI_TBL_ITEM));
    ASSERT(AcpiTable);
    
    if (!AcpiTable) return EFI_OUT_OF_RESOURCES;
    
    Status = BuildFacpAll (AcpiVer, &AcpiTable->BtHeaderPtr);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR (Status)) return EFI_OUT_OF_RESOURCES;
    
    AcpiTable->AcpiTblVer = EFI_ACPI_TABLE_VERSION_2_0;
    Status = AppendItemLst ((T_ITEM_LIST*)&gAcpiData, (VOID*) AcpiTable);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
    
    gAcpiData.AcpiLength += gAcpiData.AcpiEntries[gAcpiData.AcpiEntCount-1]->BtHeaderPtr->Length;
    
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
#endif
 
//------ Generic Timer Description Table ---------------------------
#if GTDT_BUILD
   AcpiTable = MallocZ (sizeof (ACPI_TBL_ITEM));
   ASSERT(AcpiTable);

   if (AcpiTable)
   {
       Status = BuildGTDT (AcpiVer, &AcpiTable->BtHeaderPtr);
       if (!EFI_ERROR(Status))
       {
    	   AcpiTable->AcpiTblVer = EfiAcpiVer;
           Status = AppendItemLst ((T_ITEM_LIST*)&gAcpiData, (VOID*) AcpiTable);
           ASSERT_EFI_ERROR(Status);
           gAcpiData.AcpiLength += gAcpiData.AcpiEntries[gAcpiData.AcpiEntCount-1]->BtHeaderPtr->Length;
       }
   }
#endif
//------ Generic Timer Description Table End ------------------------

//------ Multiple APIC Description Table ---------------------------
#if ARM_MADT_BUILD
   AcpiTable = MallocZ (sizeof (ACPI_TBL_ITEM));
   ASSERT(AcpiTable);

   if (AcpiTable)
   {
	   Status = BuildMADT(AcpiVer, &AcpiTable->BtHeaderPtr);
       if (!EFI_ERROR(Status))
       {
    	   AcpiTable->AcpiTblVer = EfiAcpiVer;
           Status = AppendItemLst ((T_ITEM_LIST*)&gAcpiData, (VOID*) AcpiTable);
           ASSERT_EFI_ERROR(Status);
           gAcpiData.AcpiLength += gAcpiData.AcpiEntries[gAcpiData.AcpiEntCount-1]->BtHeaderPtr->Length;
       }
   }
#endif
   return Status;
}
//------ Multiple APIC Description Table End ------------------------

/**
    This function will be called when ReadyToBoot event will be signaled and
    will publish all ACPI tables 

         
    @param Event signalled event
    @param Context calling context

          
    @retval VOID

**/
VOID ArmAcpiReadyToBootFunction(EFI_EVENT Event, VOID *Context)
{
	EFI_STATUS          Status;
	Status = PublishTbl (0, 1);
	
	ASSERT_EFI_ERROR(Status);
	gPublishedOnReadyToBoot = 1;
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
    
    return  CreateReadyToBootEvent(
                 TPL_CALLBACK, ArmAcpiReadyToBootFunction, NULL, &EvtReadyToBoot
             );
}   

/**
    Library constructor - Init Ami Lib

         
    @param ImageHandle Image handle
    @param SystemTable pointer to system table

    @retval 
        EFI_SUCCESS


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
