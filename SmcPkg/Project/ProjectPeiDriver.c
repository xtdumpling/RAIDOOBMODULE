//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//
//
//  Rev. 1.08
//   Bug Fixed:  Fixed system may occur IERR when injecting PCIE error.
//   Reason:     Remove workaround.
//   Auditor:    Chen Lin
//   Date:       May/08/2017
// 
//
//  Rev. 1.07
//   Bug Fixed:  Move NVDIMM code to SmcNVDIMM module.
//   Reason:     
//   Auditor:    Leon Xu
//   Date:       Mar/16/2017
//
//  Rev. 1.06
//   Bug Fixed:  Fixed system will hang up and no log when inject SERR/PERR with H0 CPU 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Mar/13/2017
//
//  Rev. 1.05
//    Bug Fixed:  Fixed swjpme2 can't work problem
//    Reason:     GPIO PADRSTCFG Bit[31:30] as 00 : maps to RSMRST for keep GPIO attribute during CF9 reset.
//    Auditor:    Jimmy Chiu
//    Date:       Sep/26/2016
//
//  Rev. 1.04
//   Bug Fixed:  Fixed compiling code has an error. 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Sep/26/2016
//
//  Rev. 1.03
//   Bug Fixed:  Fixe Memory Map out feature could not work. 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Sep/26/2016
//
//  Rev. 1.02
//    Bug Fixed:  Support SMC Memory map-out function.
//    Reason:     
//    Auditor:    Ivern Yeh
//    Date:       Jul/07/2016
//
//  Rev. 1.01
//    Bug Fix : Add a SMC feature - JPME2_Jumpless_SUPPORT
//    Reason  : It is one of SMC features.
//    Auditor : Hartmann Hsieh
//    Date    : Jun/08/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/19/2014
//
//****************************************************************************
//****************************************************************************
#include <Token.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/IoLib.h>  // JPME2_Jumpless_SUPPORT
#include <Sps.h>  // JPME2_Jumpless_SUPPORT
#include "MeAccess.h"  // JPME2_Jumpless_SUPPORT
#include <Library/PciLib.h>
#if MemoryMapOut_SUPPORT
#include <Guid/SetupVariable.h>
#include <ppi/ReadOnlyVariable2.h>
#include "PEI.h"
EFI_GUID gEfiPeiReadOnlyVariablePpiGuid = EFI_PEI_READ_ONLY_VARIABLE2_PPI_GUID;
#endif
#include <PchAccess.h>  // JPME2_Jumpless_SUPPORT
#include <SspTokens.h>
#include <SmcLibCommon.h>
#include <Setup.h>
#include <Guid/SetupVariable.h> //Supermicro
#include <Register/PchRegsPmc.h>

#include <Library/SetupLib.h>
#include <Library/HobLib.h>

#if JPME2_Jumpless_SUPPORT

BOOLEAN
MeIsInManufacturingMode (
  VOID
  )
{
    SPS_MEFS1  MeFs1;
    SPS_MEFS2  MeFs2;
    
    //
    // Note: We are checking if ME is in Recovery Mode actually.
    //       This follows the current status of Purley CRB.
    //       This may change in the future.
    //
    
    MeFs1.UInt32 = HeciPciRead32(0x40);
    if(MeFs1.UInt32 == 0xFFFFFFFF) MeFs1.UInt32 = 0;
    if (MeFs1.Bits.CurrentState != MEFS1_CURSTATE_RECOVERY) return FALSE;
    MeFs2.UInt32 = HeciPciRead32(0x48);
    if(MeFs2.UInt32 == 0xFFFFFFFF) MeFs2.UInt32 = 0;
    if (MeFs2.Bits.RecoveryCause != MEFS2_RCAUSE_MFGJMPR) return FALSE;
    
    return TRUE;
}

VOID Issue_Global_Reboot(
  IN EFI_PEI_SERVICES          **PeiServices,
  IN UINT32	tokenID,
  IN UINT8	CmosValue
)
{
    UINT32 i;
    
    SetCmosTokensValue(tokenID, CmosValue);
    
    i = *(UINT32*)MmPciAddress(0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_ETR3);
    i = i | B_PCH_PMC_ETR3_CF9GR;
    *(UINT32*)MmPciAddress(0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_ETR3) = i;
    IoWrite8(R_PCH_RST_CNT, V_PCH_RST_CNT_FULLRESET);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : Check_ME_Reboot
//
// Description : Check ME host fimware status meet GPIO setting or not and issue a
//               global reset to make status and setting consistence
// Parameters  : none
//
// Returns     : EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID Check_ME_Reboot(
  IN EFI_PEI_SERVICES          **PeiServices
)
{
    BOOLEAN  MeInManuMode = FALSE;
    UINT32   ReadGPIO32;
    UINT8    CMOS50, CMOS51, CMOS52;

    ReadGPIO32 = MmioRead32(JPME2_GPIO_CFG_MMIO_Address);
    // MEI Host Firmware Status
    MeInManuMode = MeIsInManufacturingMode();
    CMOS50 = GetCmosTokensValue (Q_MEUD);
    CMOS51 = GetCmosTokensValue (Q_MEUD_Reboot_CHK);
    CMOS52 = GetCmosTokensValue (Q_MEUD_HW_WDT);
    
    DEBUG((-1, "start Check_ME_Reboot\n"));

    if ( (CMOS50 == 0x5A) && ((CMOS51 & 0x0A) != 0x0A))
    {
        if (!MeInManuMode)
        {
            MmioWrite32(JPME2_GPIO_CFG_MMIO_Address, 0x04000201);
            ReadGPIO32 = MmioRead32(JPME2_GPIO_CFG_MMIO_Address);  // Take register effect.
            Issue_Global_Reboot(PeiServices, Q_MEUD_Reboot_CHK, 0x0A);  // set Q_MEUD_Reboot_CHK
        }
    }
    
    if ( (CMOS50 == 0x5A) && ((CMOS51 & 0x0A) == 0x0A))
    {
        // if Manufacturing mode is Enabled, fill 08h into CMOS 51h. 
        if (MeInManuMode) SetCmosTokensValue(Q_MEUD_Reboot_CHK, 0x08);
    }
    
    CMOS51 = GetCmosTokensValue (Q_MEUD_Reboot_CHK);
    if (((CMOS50 == 00) && (CMOS51 == 00)) || ((CMOS50 == 0x5A) && ((CMOS51 & 0x10) == 0x10)))
    {
        if (MeInManuMode)
        {
            MmioWrite32(JPME2_GPIO_CFG_MMIO_Address, 0x04000200);
            ReadGPIO32 = MmioRead32(JPME2_GPIO_CFG_MMIO_Address);  // Take register effect.
            SetCmosTokensValue(Q_MEUD, 0x00);
            if(CMOS52 <= 5){  // WD Flag for the ME enter by JPME2 HW jumper.
                SetCmosTokensValue (Q_MEUD_HW_WDT, (CMOS52 + 1)); // Increase time out flag.
                Issue_Global_Reboot(PeiServices, Q_MEUD_Reboot_CHK, 00); // set Q_MEUD_Reboot_CHK
            }
        }
    }
    if(CMOS52 > 5){
        SetCmosTokensValue(Q_MEUD, 0xFF);  // Clear the CMOS50 flag 
        SetCmosTokensValue(Q_MEUD_Reboot_CHK, 0xFF);  // Clear the CMOS51 flag 
        SetCmosTokensValue(Q_MEUD_HW_WDT, 0);  // Clear the WD flag
    }
}
#endif // #if JPME2_Jumpless_SUPPORT

#if MemoryMapOut_SUPPORT
VOID Check_MemMapOut(
  IN EFI_PEI_SERVICES          **PeiServices
){
	EFI_STATUS	Status = EFI_SUCCESS;
	EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
	UINTN	VariableSize;
	UINT32	Counter = 0;
	UINT32  Data32 = 0;
	BOOLEAN ClearMapOut = FALSE; 
#if DEBUG_MODE 	
	UINT8	Data; //Index, Data, maskbit;
#endif
 
 	Status = (*PeiServices)->LocatePpi (
					PeiServices,
					&gEfiPeiReadOnlyVariablePpiGuid,
					0,
					NULL,
					&ReadOnlyVariable
					);
	    if (EFI_ERROR (Status)) {
	    	DEBUG((-1, "Locating ReadOnlyVariable PPI Failed!, Status = %r\n", Status));
	    } else {
	        DEBUG((-1, "Locating ReadOnlyVariable PPI Success!, Status = %r\n", Status));
	    }
        VariableSize = sizeof(UINT32);
	    Status = ReadOnlyVariable->GetVariable (ReadOnlyVariable, L"MonotonicCounter", &gAmiGlobalVariableGuid, NULL, &VariableSize, &Counter);
	    if (EFI_ERROR (Status)) {
	      DEBUG((-1, "first boot,clear MEM_MAPOUT\n"));        
	      ClearMapOut = TRUE;
//	      DEBUG((-1, "first boot,clear PERR WA in cmos\n"));
//	      SetCmosTokensValue (PERR_WA_H, 0);
//          SetCmosTokensValue (PERR_WA_L, 0);   
	    }

	    //
	    if ( ClearMapOut != TRUE){
	      // clear map out when cold boot (boot up from G3, S5)  check register BIOSSCRATCHPAD0_UBOX_MISC_REG
	       Data32 = PciRead32 ( PCI_LIB_ADDRESS(0, 8, 2, 0xA0));
           DEBUG((-1, "Dev8 Fun2 RA0 = 0x%x\n", Data32)); 
	        if ( (Data32 & BIT1) == 0 ){
	          DEBUG((-1,"Cold boot,clear MEM_MAPOUT\n"));    
	          ClearMapOut = TRUE;
	        } 	       
	    }
	    
	    
	    if (ClearMapOut) {
	    	DEBUG((-1, "Clear MEM_MAPOUT CMOS\n"));
			  //clear cmos
			SetCmosTokensValue (MEM_MAPOUT, 0x0);
//			PcdSet8(PcdSMCMapOutDIMM, 0x0);
	    } else {
	    #if DEBUG_MODE 	        
	       //Get cmos
           Data = GetCmosTokensValue (MEM_MAPOUT);
           DEBUG((-1, "MapOutDIMM = 0x%x\n", Data));
        #endif   
		   return;
	    }   
    
}
#endif

#pragma warning ( disable : 4090 4028)
//
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : ProjectPeiDriverInit
//
// Description : Init ProjectPeiDriverInit
//
// Parameters  : none
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI ProjectPeiDriverInit(
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  )
{
	EFI_STATUS Status = EFI_SUCCESS;

	DEBUG((EFI_D_INFO, "ProjectPeiDriverInit.\n"));

#if MemoryMapOut_SUPPORT
    Check_MemMapOut(PeiServices);
#endif	

#if JPME2_Jumpless_SUPPORT && JPME2_GPIO_CFG_MMIO_Address != 0xFFFFFFFF 
    Check_ME_Reboot(PeiServices);
#endif

	DEBUG((EFI_D_INFO, "Exit ProjectPeiDriverInit.\n"));

	return EFI_SUCCESS;
}
