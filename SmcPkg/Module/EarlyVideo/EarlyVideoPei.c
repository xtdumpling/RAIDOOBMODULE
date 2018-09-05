//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.06
//    Bug Fix:  Modify location of graphic mode progress-code.
//    Reason:   
//    Auditor:  YuLin Yang
//    Date:     Mar/17/2017
//
//  Rev. 1.05
//    Bug Fix:  Set Foreground color to white and Background color to black
//              when Changed Logo by AMI utility ChangeLogo.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Mar/16/2017
//
//  Rev. 1.04
//    Bug Fix:  Early Graphic Logo Support.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Mar/14/2017
//
//  Rev. 1.03
//    Bug Fix:  Add AERON and MCEON message.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Dec/20/2016
//
//  Rev. 1.02
//    Bug Fix:  Use AMIBCP to disable boot procedure messages displaying.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jun/21/2016
//
//  Rev. 1.01
//    Bug Fix: Turn on Early-Video in earlier PEI.
//    Reason : To sync with Grantley
//    Auditor: Yenhsin Cho
//    Date   : May/27/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/04/2014
//
//****************************************************************************
//****************************************************************************
#include <Pei.h>
#include <Hob.h>
#include <Setup.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <PeiSimpleTextOutPpi.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <AmiCspLib.h>
#include <Guid/SetupVariable.h>

extern EFI_STATUS InitializeEarlyVideoStatus(
    IN  CONST EFI_PEI_SERVICES  **PeiServices
);

VOID VbiosInitEntry(
    IN  EFI_PEI_SERVICES        **PeiServices
);

#if EarlyVideo_Mode
VOID DrawInitImage(EFI_PEI_SERVICES **PeiServices);
BOOLEAN CheckIfChangelogo(EFI_PEI_SERVICES **PeiServices);
#endif


EFI_GUID gEfiPeiSimpleTextOutPPIGuid = EFI_PEI_SIMPLETEXTOUT_PPI_GUID;
EFI_GUID gEfiPeiReadOnlyVariablePpiGuid = EFI_PEI_READ_ONLY_VARIABLE2_PPI_GUID;

VOID
PrintInitString(
    IN  EFI_PEI_SERVICES        **PeiServices
)
{
    EFI_STATUS  SetupStatus = EFI_SUCCESS;
    EFI_STATUS  AMITSESetupStatus = EFI_SUCCESS;
    EFI_STATUS  StatusIntelSetup;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *PeiVariable;
    UINTN       VariableSize;
    UINT32      VarAttrib;
    EFI_GUID    SetupGuid = SETUP_GUID;
    SETUP_DATA  SetupData;
    AMITSESETUP AmiTseSetupData;
    EFI_GUID    AmiTseSetupGuid = AMITSESETUP_GUID;
    EFI_STATUS  Status;
    EFI_PEI_SIMPLETEXTOUT_PPI   *mSimpleTextOutPpi = NULL;
    UINT8       DisplayPostMessages = 1;
    UINT8       DisplayEarlyVideoLogo = 1;
    SYSTEM_CONFIGURATION        SysCfg;
#if EarlyVideo_Mode
    UINT8       Attr, AttrBds;
#endif

  //
  // Locate Variable PPI
  //
    Status = (**PeiServices).LocatePpi(
                    PeiServices,
                    &gEfiPeiReadOnlyVariablePpiGuid,
                    0,
                    NULL,
                    &PeiVariable);

    if(!EFI_ERROR(Status)){
        VariableSize = sizeof(SETUP_DATA);
        SetupStatus = PeiVariable->GetVariable(
                        PeiVariable,
                        L"Setup",
                        &SetupGuid,
                        &VarAttrib,
                        &VariableSize,
                        &SetupData);

        if(!EFI_ERROR(SetupStatus)){
            if(!SetupData.SmcBootProcedureMsg_Support)
                DisplayPostMessages = 0;
        }

        VariableSize = sizeof(AMITSESETUP);
        AMITSESetupStatus = PeiVariable->GetVariable(
                        PeiVariable,
                        L"AMITSESetup",
                        &AmiTseSetupGuid,
                        &VarAttrib,
                        &VariableSize,
                        &AmiTseSetupData);

        if(!EFI_ERROR(SetupStatus) && !EFI_ERROR(AMITSESetupStatus)){
            if(!(SetupData.EarlyVideo_Logo && AmiTseSetupData.AMISilentBoot))
                DisplayEarlyVideoLogo = 0;
        }
        
        VariableSize = sizeof(SysCfg);
        StatusIntelSetup = PeiVariable->GetVariable(
                        PeiVariable,
                        L"IntelSetup",
                        &SetupGuid,
                        &VarAttrib,
                        &VariableSize,
                        &SysCfg);
#if EarlyVideo_Mode
        // Get Color Attribute and update PcdSmcColor
        if(!AmiTseSetupData.AMISilentBoot) {
            Attr = 0xF0; // Set Foreground color to white and Background color to black when QuietBoot disabled
            AttrBds = Attr;
        }
        else if(CheckIfChangelogo(PeiServices)) {
            Attr = 0xF0;    // Set Foreground color to white and Background color to black when Changed Logo by AMI utility ChangeLogo
            AttrBds = (SetupData.HotKey_Msg_Foreground_Color << 4) + (SetupData.HotKey_Msg_Background_Color & 0x0F);
        }
        else {
            Attr = (SetupData.HotKey_Msg_Foreground_Color << 4) + (SetupData.HotKey_Msg_Background_Color & 0x0F);
            AttrBds = Attr;
        }

        PcdSet8(PcdSmcColor, Attr);
        PcdSet8(PcdSmcBdsColor, AttrBds);
#endif
    }

    Status = (*PeiServices)->LocatePpi(
                    PeiServices,
                    &gEfiPeiSimpleTextOutPPIGuid,
                    0,
                    NULL,
                    &mSimpleTextOutPpi);

    if(!EFI_ERROR(Status)){
#if EarlyVideo_Mode
        if(DisplayEarlyVideoLogo)
            DrawInitImage(PeiServices);
        if(DisplayPostMessages){
            mSimpleTextOutPpi->OutputString(00, SMC_MAX_ROW-2, Attr, "  System Initializing.. ");
            mSimpleTextOutPpi->OutputString(23, SMC_MAX_ROW-2, Attr, ".");
        }
        if(!EFI_ERROR(StatusIntelSetup) && SysCfg.CorrMemErrEn)
            mSimpleTextOutPpi->OutputString(SMC_MAX_COL-16, SMC_MAX_ROW-1, Attr, "MCEON");
        if(!EFI_ERROR(SetupStatus) && SetupData.AerEnable)
            mSimpleTextOutPpi->OutputString(SMC_MAX_COL-16+5, SMC_MAX_ROW-1, Attr, " AERON");
#else
        mSimpleTextOutPpi->ClearScreen();
        mSimpleTextOutPpi->EnableCursor(FALSE);
        if(DisplayEarlyVideoLogo){
            mSimpleTextOutPpi->ClearScreen();
            mSimpleTextOutPpi->EnableCursor(FALSE);
            mSimpleTextOutPpi->OutputString(7, 8, 0x01,  " #####                                                    ");
            mSimpleTextOutPpi->OutputString(7, 9, 0x01,  "#     # #    # #####  ###### #####  #    # #  ####  ##### ");
            mSimpleTextOutPpi->OutputString(7, 10, 0x01, "#       #    # #    # #      #    # ##  ## # #    # #    #");
            mSimpleTextOutPpi->OutputString(7, 11, 0x01, " #####  #    # #    # #####  #    # # ## # # #      #    #");
            mSimpleTextOutPpi->OutputString(7, 12, 0x01, "      # #    # #####  #      #####  #    # # #      ##### ");
            mSimpleTextOutPpi->OutputString(7, 13, 0x01, "#     # #    # #      #      #   #  #    # # #    # #   # ");
            mSimpleTextOutPpi->OutputString(7, 14, 0x01, " #####   ####  #      ###### #    # #    # #  ####  #    #");
            mSimpleTextOutPpi->OutputString(65, 9, 0x04,  "   ####  ");
            mSimpleTextOutPpi->OutputString(65, 10, 0x04, "  ###### ");
            mSimpleTextOutPpi->OutputString(65, 11, 0x04, " ########");
            mSimpleTextOutPpi->OutputString(65, 12, 0x04, " ########");
            mSimpleTextOutPpi->OutputString(65, 13, 0x04, "  ###### ");
            mSimpleTextOutPpi->OutputString(65, 14, 0x04, "   ####  ");
        }
        if(DisplayPostMessages){
            mSimpleTextOutPpi->OutputString(00, SMC_MAX_ROW-1, 0x0F, "  System Initializing.. ");
            mSimpleTextOutPpi->OutputString(23, SMC_MAX_ROW-1, 0x8F, ".");
        }
        if(!EFI_ERROR(StatusIntelSetup) && SysCfg.CorrMemErrEn)
            mSimpleTextOutPpi->OutputString(SMC_MAX_COL-16, SMC_MAX_ROW-1, 0x0F, "MCEON");
        if(!EFI_ERROR(SetupStatus) && SetupData.AerEnable)
            mSimpleTextOutPpi->OutputString(SMC_MAX_COL-16+5, SMC_MAX_ROW-1, 0x0F, " AERON");
#endif //#if EarlyVideo_Mode
    }
}

EFI_STATUS
EarlyVideoPeiInit(
    IN  EFI_FFS_FILE_HEADER     *FfsHeader,
    IN  EFI_PEI_SERVICES        **PeiServices
)
{
    EFI_STATUS  Status = EFI_SUCCESS;

    VbiosInitEntry(PeiServices);
    PrintInitString(PeiServices);
    InitializeEarlyVideoStatus(PeiServices);

    return Status;
}
