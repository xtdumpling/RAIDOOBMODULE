//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.11
//    Bug Fix:  Add the "PEI--IPMI Initialization" Post-Help message.
//    Reason:   Function improvement
//    Auditor:  Jimmy Chiu
//    Date:     Aug/9/2017
//
//  Rev. 1.10
//    Bug Fix:  Change to read KCS COMMAND/STATUS register instead of DATA register
//    Reason:   Avoid KCS timeout due to data register sometimes always 0xFF which is valid.
//    Auditor:  Stephen Chen
//    Date:     Apr/14/2017
//
//  Rev. 1.09
//    Bug Fix:  Enhance SMC debug card function code.
//    Reason:   To show correct information on debug card when system is waiting IPMI self test.
//    Auditor:  Jacker Yeh
//    Date:     Jan/21/2017
//
//  Rev. 1.08
//    Bug Fix:  Skip 0x30 0x20 command if SSID is the same.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Nov/25/2016
//
//  Rev. 1.07
//    Bug Fix:  Extend BIOS version string for BMC.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/25/2016
//
//  Rev. 1.06
//    Bug Fix:  Add MAC into OEM command 0x30 0x20.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jul/29/2016
//
//  Rev. 1.05
//    Bug Fix:  Follow BMC team request to modify command 0x20 format.
//    Reason:   Command 0x20 byte 4 need to be 1, otherwise BIOS date/time and MAC will not show.
//    Auditor:  Jacker Yeh
//    Date:     Jul/28/2016
//
//  Rev. 1.04
//    Bug Fix:  Review BMC OEM command
//    Reason:   Rollback BMC OEM command
//    Auditor:  Jimmy Chiu
//    Date:     Jul/28/2016
//
//  Rev. 1.03
//    Bug Fix:  Add Full SMBIOS module.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/27/2016
//
//  Rev. 1.02
//    Bug Fix:  Remove SendBIOSVerBuildTime routine.
//    Reason:   Move to DXE phase to sync with SMBIOS type 0.
//    Auditor:  Jacker Yeh
//    Date:     Jul/19/2016
//
//  Rev. 1.01
//    Bug Fix:  Add a item for skipping the IPMI commands for reducing POST time.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/01/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     Jun/07/2016
//
//****************************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file PeiIpmiBmcInitialize.c
    PEI IPMI Transport Driver.

**/

//---------------------------------------------------------------------------

#include "Token.h"
#include "IpmiBmc.h"
#include "IpmiInitHooks.h"
#include <Include/ServerMgmtSetupVariable.h>

//---------------------------------------------------------------------------

#if SMCPKG_SUPPORT
#include "Setup.h"
#include "SuperMPeiDriver.h"
#include "TimeStamp.h"
#include <PeiSimpleTextOutPpi.h>
#define ___INTERNAL_CONVERT_TO_STRING___(a) #a
#define CONVERT_TO_STRING(a) ___INTERNAL_CONVERT_TO_STRING___(a)
#endif

//---------------------------------------------------------------------------

extern INIT_LPC_BMC INIT_LPC_BMC_HOOK_LIST EndOfInitLpcBmcHookListFunctions;
INIT_LPC_BMC* gInitLpcBmcHookList[] = {INIT_LPC_BMC_HOOK_LIST NULL};

#if BMC_INIT_DELAY
extern BMC_INIT_DELAY_HOOK BMC_INIT_DELAY_HOOK_LIST EndOfBmcInitDelayHookListFunctions;
BMC_INIT_DELAY_HOOK* gBmcInitDelayHookList[] = {BMC_INIT_DELAY_HOOK_LIST NULL};
#endif

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
extern IPMI_OEM_PEI_INIT_HOOK IPMI_OEM_PEI_INIT_HOOK_LIST EndOfIpmiOemPeiInitHookListFunctions;
IPMI_OEM_PEI_INIT_HOOK* gIpmiOemPeiInitHookList[] = {IPMI_OEM_PEI_INIT_HOOK_LIST NULL};
#endif

//---------------------------------------------------------------------------

/**
    This routine calls the InitLpcBmc function or platform hooked function.

    @param PeiServices Pointer to PEI Services.

    @return EFI_STATUS Return Status

**/
EFI_STATUS
InitLpcBmcHook (
  IN CONST EFI_PEI_SERVICES          **PeiServices )
{
    EFI_STATUS Status;
    UINTN i;

    for(i = 0; gInitLpcBmcHookList[i]; i++) {
        Status = gInitLpcBmcHookList[i](PeiServices);
    }

    return Status;
}

#if BMC_INIT_DELAY
/**
    This routine calls the BmcInitDelay function or platform hooked function.

    @param PeiServices Pointer to PEI Services.

    @return BOOLEAN

**/
BOOLEAN
BmcInitDelayHook (
  IN CONST EFI_PEI_SERVICES          **PeiServices )
{
    BOOLEAN IsPowerFail;
    UINTN   i;

    for(i = 0; gBmcInitDelayHookList[i]; i++) {
        IsPowerFail = gBmcInitDelayHookList[i](PeiServices);
    }

    return IsPowerFail;
}
#endif

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
/**
    Initiates the PEI hooks of OEM.

    @param PeiServices      - Pointer the set of PEI services.
    @param MmioBaseAddress  - Pointer to the MMIO base address.
    @param BaseAddressRange - Pointer to the base address range.

    @return EFI_STATUS      - Return status of the Hook.
*/

EFI_STATUS
IpmiOemPeiInitHook (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       UINTN                        *MmioBaseAddress,
  IN       UINTN                        *BaseAddressRange
)
{
    EFI_STATUS  Status = EFI_DEVICE_ERROR;
    UINTN       Index;
    for (Index = 0; gIpmiOemPeiInitHookList[Index]; Index++) {
        Status = gIpmiOemPeiInitHookList[Index] (PeiServices, MmioBaseAddress, BaseAddressRange);
    }
    return Status;
}
#endif

/**
    Execute the Get Device ID command to determine whether or not the BMC is in
    Force Update Mode. If it is, then report it to the error manager.

    @param IpmiInstance Data structure describing BMC
               variables and used for sending commands

    @return EFI_STATUS Return Status of the Sent Command
    @retval EFI_SUCCESS Command executed Successfully

**/
EFI_STATUS
GetDeviceId (
  IN  OUT  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance )
{
    EFI_STATUS                            Status;
    UINT8                                 DataSize;
    IPMI_GET_DEVICE_ID_RESPONSE           GetDeviceIdResponse;

    DataSize = sizeof (GetDeviceIdResponse);

    //
    // Get the device ID information for the BMC.
    //
    Status = IpmiInstance->IpmiTransport.SendIpmiCommand (
                &IpmiInstance->IpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_DEVICE_ID,
                NULL,
                0,
                (UINT8*)&GetDeviceIdResponse,
                &DataSize );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get Device ID Command: Status %r\n", Status));
    //
    // If there is no error then proceed to check the data returned by the BMC
    //
    if (!EFI_ERROR(Status)) {

        //
        // If the controller is in Update Mode then set the BMC Status to indicate
        // the BMC is in force update mode.
        //
        if (GetDeviceIdResponse.UpdateMode != 0) {
            IpmiInstance->BmcStatus = EFI_BMC_UPDATE_IN_PROGRESS;
        }
    }
    return Status;
}

#if SMCPKG_SUPPORT
VOID
SendBoardId(
    IN	CONST EFI_PEI_SERVICES	**PeiServices,
    IN	EFI_IPMI_BMC_INSTANCE_DATA	*IpmiInstance
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	ResponseData, ResponseDataSize = sizeof(ResponseData);
    SUPERM_PEI_DRIVER_PROTOCOL	*mSuperMPeiDriver;
    SUPERMBOARDINFO		mBoardPeiInfo;

    DEBUG((-1, "SMC SendBoardId.\n"));
    Status = (*PeiServices)->LocatePpi(
    		    PeiServices,
    		    &gSuperMPeiPrococolGuid,
    		    0,
    		    NULL,
    		    &mSuperMPeiDriver);

    if(Status)	return;

    Status = mSuperMPeiDriver->GetSuperMBoardInfo(
    			PeiServices,
    			mSuperMPeiDriver,
    			&mBoardPeiInfo);

    if(Status)	return;
    
    if(mBoardPeiInfo.MBSVID == (UINT16)(SMC_SSID >> 16))
        return;

    Status = IpmiInstance->IpmiTransport.SendIpmiCommand(
		    &IpmiInstance->IpmiTransport,
		    0x30,	//EFI_SM_INTEL_OEM,
		    0,
		    0x20,	//Get/Set SVID
		    mBoardPeiInfo.HardwareInfo,
		    sizeof(mBoardPeiInfo.HardwareInfo),
		    &ResponseData,
		    &ResponseDataSize);
}

EFI_STATUS SMC_IPMICmd30_A0_20 (
    IN	CONST EFI_PEI_SERVICES	**PeiServices,
    IN OUT	EFI_IPMI_BMC_INSTANCE_DATA	*IpmiInstance
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8 Data[1];
    UINT8 ResponseData[5], ResponseDataSize = sizeof (ResponseData);

    DEBUG((-1, "SMC_IPMICmd30_A0_20 Entry\n"));

    Data[0] = 0x20;
    ResponseDataSize = sizeof (ResponseData);
    Status = IpmiInstance->IpmiTransport.SendIpmiCommand(
                 &IpmiInstance->IpmiTransport,
                 0x30, // EFI_SM_INTEL_OEM,
                 0,
                 0xA0,
                 Data,
                 1,
                 &(ResponseData[0]),
                 &ResponseDataSize);

    if (!EFI_ERROR (Status)) {
        PcdSet32 (PcdGetOobFileStatus0, ResponseData[1]);
        PcdSet32 (PcdGetOobFileStatus1, ResponseData[2]);
        PcdSet32 (PcdGetOobFileStatus2, ResponseData[3]);
        PcdSet32 (PcdGetOobFileStatus3, ResponseData[4]);
    }
    else {
        PcdSet32 (PcdGetOobFileStatus0, 0);
        PcdSet32 (PcdGetOobFileStatus1, 0);
        PcdSet32 (PcdGetOobFileStatus2, 0);
        PcdSet32 (PcdGetOobFileStatus3, 0);
    }

    DEBUG((-1, "SMC_IPMICmd30_A0_20 End\n"));

    return Status;
}

VOID
SmcIPMIPEISetting(
    IN	CONST EFI_PEI_SERVICES	**PeiServices,
    IN OUT	EFI_IPMI_BMC_INSTANCE_DATA	*IpmiInstance
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	CommandData[MAX_TEMP_DATA];
    UINT8	DataSize;
    SETUP_DATA	SetupData;
    EFI_GUID	SetupGuid = SETUP_GUID;
    UINTN	VariableSize = sizeof(SETUP_DATA);
    EFI_PEI_READ_ONLY_VARIABLE2_PPI	*ReadOnlyVariable = NULL;

    SERVER_IPMI_DEBUG((EFI_D_LOAD, "SmcIPMIPEISetting Start\n"));

    Status = (*PeiServices)->LocatePpi(
		    PeiServices,
		    &gEfiPeiReadOnlyVariable2PpiGuid,
		    0,
		    NULL,
		    &ReadOnlyVariable);

    if(!EFI_ERROR(Status)){
	Status = ReadOnlyVariable->GetVariable(
			ReadOnlyVariable,
			L"Setup",
			&SetupGuid,
			NULL,
			&VariableSize,
			&SetupData);
		
	if(!EFI_ERROR(Status)){
#if defined BMC_UART2Mode_Support && BMC_UART2Mode_Support == 0x01
	    CommandData[0] = 0x06;
	    CommandData[1] = SetupData.SmcUART2Mode;

	    DataSize = MAX_TEMP_DATA;
	    Status = IpmiInstance->IpmiTransport.SendIpmiCommand(
			    &IpmiInstance->IpmiTransport,
			    0x30,	//EFI_SM_INTEL_OEM,
			    0,
			    0x70,
			    CommandData,
			    2,
			    IpmiInstance->TempData,
			    &DataSize);
#endif
	}
    }

    SendBoardId(PeiServices, IpmiInstance);
    MicroSecondDelay(DELAY_OF_THIRTY_SECOND/15);
    Status = SMC_IPMICmd30_A0_20 (PeiServices, IpmiInstance);

    SERVER_IPMI_DEBUG ((EFI_D_LOAD, "SmcIPMIPEISetting Start\n"));
}
#endif

/**
    Execute the Get System Interface Capability command and update Ipmi Instance.
    
    @ IpmiInstance Pointer to Data structure describing BMC
                   variables and used for sending commands

    @retval  VOID

**/
VOID
GetSystemInterfaceCapability (
  IN  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance
)
{
    EFI_STATUS                                     Status;
    EFI_GET_SYSTEM_INTERFACE_CAPABILITY_COMMAND    GetSystemInterfaceCapabilityCmd;
    EFI_IPMI_SSIF_INTERFACE_CAPABILITY             GetSsifInterfaceCapability;
    UINT8                                          DataSize = sizeof (GetSsifInterfaceCapability);

    GetSystemInterfaceCapabilityCmd.SystemInterfaceType = 0x0; // SSIF
    GetSystemInterfaceCapabilityCmd.Reserved = 0x0;
    
    Status = IpmiInstance->IpmiTransport.SendIpmiCommand (
                &IpmiInstance->IpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_SYSTEM_INTERFACE_CAPABILITIES,
                (UINT8*) &GetSystemInterfaceCapabilityCmd,
                sizeof (GetSystemInterfaceCapabilityCmd),
                (UINT8*) &GetSsifInterfaceCapability,
                &DataSize);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get System Interface Capability status %r\n", Status));

    if (!EFI_ERROR (Status)) {
        IpmiInstance->Interface.SSIF.RwSupport = GetSsifInterfaceCapability.TransactionSupport;
        IpmiInstance->Interface.SSIF.PecSupport = GetSsifInterfaceCapability.PecSupport;
    }

}

/**
    Execute the Set Global Enable command to enable receive message queue interrupt.

    @param IpmiInstance Data structure describing BMC
                        variables and used for sending commands

    @return VOID
**/
VOID
SetGlobalEnable (
  IN  OUT  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance )
{
    EFI_STATUS                      Status;
    EFI_BMC_GLOBAL_ENABLES          BmcGlobalEnables;
    UINT32                          ResponseDataSize = sizeof (BmcGlobalEnables);

    //
    // Get Global Enable Information.
    //
    Status = IpmiInstance->IpmiTransport.SendIpmiCommand(
                &IpmiInstance->IpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_SET_BMC_GLOBAL_ENABLES,
                NULL,
                0,
                (UINT8 *) (&BmcGlobalEnables),
                (UINT8 *) &ResponseDataSize
                );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get BMC Global Enable status %r\n", Status));

    if (EFI_ERROR(Status)) {
        return;
    }

    //
    // Set SmbAlertSupport based on ReceiveMsgQueueInterrupt bit
    //

    IpmiInstance->Interface.SSIF.SmbAlertSupport = BmcGlobalEnables.ReceiveMsgQueueInterrupt;
}

/**
    Setup and initialize the BMC for the PEI phase. In order to
    verify the BMC is functioning as expected, the BMC Self-test is performed.
    The results are then checked and any errors are reported to the error manager.
    Errors are collected throughout this routine and reported just prior to 
    installing the driver.  If there are more errors than BST_LOG_SIZE, then they
    will be ignored.

    @param FileHandle    Pointer to image file handle.
    @param PeiServices   Pointer to the PEI Core data Structure

    @return EFI_STATUS  Status of Driver execution
    @retval EFI_SUCCESS Successful driver initialization

**/
EFI_STATUS
InitializeIpmiPhysicalLayer (
  IN        EFI_PEI_FILE_HANDLE     FileHandle,
  IN  CONST EFI_PEI_SERVICES        **PeiServices )
{

    EFI_STATUS                              Status = EFI_SUCCESS;
    EFI_IPMI_BMC_INSTANCE_DATA              *IpmiInstance = NULL;
    UINT8                                   DataSize;
    UINT8                                   Index;
    EFI_PEI_PPI_DESCRIPTOR                  *PeiIpmiPpiDesc = NULL;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI         *PeiVariable;
    SERVER_MGMT_CONFIGURATION_DATA          ServerMgmtConfiguration;
    IPMI_SELF_TEST_RESULT_RESPONSE          BstStatus;
    UINTN                                   Size;
    EFI_PEI_I2C_MASTER_PPI                  *MasterTransmit = NULL;
    EFI_PEI_SMBUS2_PPI                      *EfiPeiSmbus2Ppi = NULL;
#if SMCPKG_SUPPORT
    SUPERMBOARDINFO		mBoardPeiInfo;
    SUPERM_PEI_DRIVER_PROTOCOL	*mSuperMPeiDriver;
    UINT8			CommandData[MAX_TEMP_DATA], RetryCount = 100;
#if EarlyVideo_SUPPORT
    SETUP_DATA                              SetupData;
    EFI_GUID                                SetupGuid = SETUP_GUID;
    UINTN                                   VariableSize = sizeof(SETUP_DATA);
    EFI_PEI_READ_ONLY_VARIABLE2_PPI         *ReadOnlyVariable = NULL;
    BOOLEAN                                 BootProcedureMsgSupport = TRUE;
    EFI_PEI_SIMPLETEXTOUT_PPI               *mSimpleTextOutPpi = NULL;
    CHAR8*                                  strBlank = " ";  
    EFI_GUID                                gEfiPeiSimpleTextOutPPIGuid = EFI_PEI_SIMPLETEXTOUT_PPI_GUID;
#if EarlyVideo_Mode
    UINT8                                   Attr;
#endif
#endif
#endif
    //
    // Locate the Setup configuration value.
    //
    Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gEfiPeiReadOnlyVariable2PpiGuid,
                0,
                NULL,
                (VOID **)&PeiVariable);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gEfiPeiReadOnlyVariable2PpiGuid Status %r\n", Status));
    ASSERT_EFI_ERROR (Status);

    if (!EFI_ERROR(Status)) {
        Size = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
        Status = PeiVariable->GetVariable (
                    PeiVariable,
                    L"ServerSetup",
                    &gEfiServerMgmtSetupVariableGuid,
                    NULL,
                    &Size,
                    &ServerMgmtConfiguration);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "PeiVariable->GetVariable Status %r ServerMgmtConfiguration.BmcSupport: %x \n", Status, ServerMgmtConfiguration.BmcSupport));

        if (!EFI_ERROR(Status) && !ServerMgmtConfiguration.BmcSupport) {
            return EFI_UNSUPPORTED;
        }
    }

#if SMCPKG_SUPPORT == 0
#if BMC_INIT_DELAY
    if (EFI_ERROR(Status)) {
         ServerMgmtConfiguration.WaitForBmc = 0; //Load default value.
    }   
#endif
#endif

    //
    // Allocate memory for IPMI Instance
    //
    IpmiInstance = (EFI_IPMI_BMC_INSTANCE_DATA*)AllocateZeroPool (sizeof (EFI_IPMI_BMC_INSTANCE_DATA));
    if (IpmiInstance == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for IpmiInstance! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Allocate memory for PPI Descriptor
    //
    PeiIpmiPpiDesc = (EFI_PEI_PPI_DESCRIPTOR*)AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
    if (PeiIpmiPpiDesc == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for PeiIpmiPpiDesc! \n"));
        //
        // AllocateZeroPool has failed, so free the allocated memory and return EFI_OUT_OF_RESOURCES.
        //
        FreePool (IpmiInstance);
        return EFI_OUT_OF_RESOURCES;
    }

    //
    //Platform Hook to initialize LPC for BMC and Host interface.
    //
    Status = InitLpcBmcHook (PeiServices);
    if (EFI_ERROR(Status)) {
        //
        // InitLpcBmcHook has failed, so free the allocated memory and return Status.
        //
        FreePool (IpmiInstance);
        FreePool (PeiIpmiPpiDesc);
        return Status;
    }

    if (IPMI_DEFAULT_HOOK_SUPPORT) {
        //
        // Initialize IPMI Instance Data
        //
        if (IPMI_SYSTEM_INTERFACE == KCS_INTERFACE) { // KCS Interface
            IpmiInstance->Interface.KCS.DataPort        = IPMI_KCS_DATA_PORT;       // KCS Data Port
            IpmiInstance->Interface.KCS.CommandPort     = IPMI_KCS_COMMAND_PORT;    // KCS Command Port
        } else if (IPMI_SYSTEM_INTERFACE == BT_INTERFACE) { // BT_Interface
            IpmiInstance->Interface.BT.CtrlPort    = IPMI_BT_CTRL_PORT;     // BT Control Port
            IpmiInstance->Interface.BT.ComBuffer   = IPMI_BT_BUFFER_PORT; // BT Buffer Port
            IpmiInstance->Interface.BT.IntMaskPort = IPMI_BT_INTERRUPT_MASK_PORT; // BT IntMask Port
        }
        IpmiInstance->MmioBaseAddress = 0;
        IpmiInstance->AccessType = IPMI_IO_ACCESS;
        IpmiInstance->BaseAddressRange = 0;
    } else {
#if (!IPMI_DEFAULT_HOOK_SUPPORT)
        IpmiInstance->AccessType = IPMI_MMIO_ACCESS;
        Status = IpmiOemPeiInitHook (PeiServices, &IpmiInstance->MmioBaseAddress, &IpmiInstance->BaseAddressRange);
        if (EFI_ERROR (Status)) {
            //
            // IpmiOemPeiInitHook has failed, so free the allocated memory and return Status.
            //
            FreePool (IpmiInstance);
            FreePool (PeiIpmiPpiDesc);
            SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IpmiOemPeiInitHook is failed and Status is %r.\n", Status));
            return Status;
        }
#endif
    }

    if (IPMI_SYSTEM_INTERFACE == KCS_INTERFACE) { // KCS Interface
        IpmiInstance->Interface.KCS.KcsRetryCounter = IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
        IpmiInstance->IpmiTransport.SendIpmiCommand = EfiIpmiSendCommand;       // KCS SendCommand
        IpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiIpmiSendCommandEx; // KCS SendCommand Extension
    } else if (IPMI_SYSTEM_INTERFACE == BT_INTERFACE) { // BT_Interface
        IpmiInstance->Interface.BT.BtDelay     = BT_DELAY; // BT Delay
        IpmiInstance->IpmiTransport.SendIpmiCommand = EfiBtIpmiSendCommand; // BT SendCommand
        IpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiBtIpmiSendCommandEx; // BT SendCommand Extension
    } else if (IPMI_SYSTEM_INTERFACE == SSIF_INTERFACE) { // SSIF Interface
        //
        // Locate the SMBUS PPI for Communication.
        //
        Status = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gEfiPeiSmbus2PpiGuid,
                                   0,
                                   NULL,
                                   (VOID **)&EfiPeiSmbus2Ppi);

        if (EFI_ERROR(Status)) {
            //
            // LocatePpi has failed, so freeing the allocated memory and return Status.
            //
            FreePool (IpmiInstance);
            FreePool (PeiIpmiPpiDesc);
            return Status;
        }

        IpmiInstance->IpmiTransport.SendIpmiCommand = EfiSsifIpmiSendCommand;   // SSIF SendCommand
        IpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiSsifIpmiSendCommandEx; // SSIF SendCommand Extension
        IpmiInstance->Interface.SSIF.SsifRetryCounter = IPMI_SSIF_COMMAND_REQUEST_RETRY_COUNTER; // SSIF retry counter
        IpmiInstance->Interface.SSIF.PecSupport = FALSE; // SSIF PEC support
        IpmiInstance->Interface.SSIF.RwSupport = 0x0; // SSIF multi-part reads/writes support
        IpmiInstance->Interface.SSIF.SmbAlertSupport = FALSE; // Smb alert pin support
    } else {
        //
        // Locate the I2C Ppi for communication.
        // Return if Ppi is not available.
        //
        Status = (*PeiServices)->LocatePpi (
                        PeiServices,
                        &gEfiPeiI2cMasterPpiGuid,
                        0,
                        NULL,
                        (VOID **)&MasterTransmit);
        if(EFI_ERROR(Status)) {
            //
            // LocatePpi has failed, so freeing the allocated memory and return Status.
            //
            FreePool (IpmiInstance);
            FreePool (PeiIpmiPpiDesc);
            return Status;
        }
        IpmiInstance->IpmiTransport.SendIpmiCommand = EfiIpmbIpmiSendCommand; // IPMB SendCommand
        IpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiIpmbIpmiSendCommandEx; // IPMB SendCommand Extension
    }

    IpmiInstance->Signature     = SM_IPMI_BMC_SIGNATURE;
    IpmiInstance->SlaveAddress  = IPMI_BMC_SLAVE_ADDRESS;
    IpmiInstance->BmcStatus     = EFI_BMC_OK;
    IpmiInstance->IpmiTransport.GetBmcStatus = EfiIpmiBmcStatus;

#if SMCPKG_SUPPORT == 0
#if BMC_INIT_DELAY

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "ServerMgmtConfiguration.WaitForBmc: %x \n", ServerMgmtConfiguration.WaitForBmc));

    //
    // Add 30 Sec delay before sending Self Test command during First AC Power Cycle On.
    // BMC starts simultaneously with BIOS so BMC takes 30 Sec to respond.
    // Give a delay of 30 Sec if the Power Failure (PWR_FLR) bit is set and WaitForBmc Setup
    // option is enabled.
    //
    if (ServerMgmtConfiguration.WaitForBmc && BmcInitDelayHook(PeiServices)) {

        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Giving DELAY_OF_THIRTY_SECOND Sec delay.. start \n"));
        MicroSecondDelay (DELAY_OF_THIRTY_SECOND);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Giving DELAY_OF_THIRTY_SECOND Sec delay.. end \n"));

    }
#endif
#endif

    //
    // Check SSIF interface capability
    //
    if (IPMI_SYSTEM_INTERFACE == SSIF_INTERFACE) {
        GetSystemInterfaceCapability(IpmiInstance);
        SetGlobalEnable(IpmiInstance);
    }

    //
    // Get the BMC SELF TEST Results.
    //
#if SMCPKG_SUPPORT
    Status = (*PeiServices)->LocatePpi(
		    PeiServices,
		    &gSuperMPeiPrococolGuid,
		    0,
		    NULL,
		    &mSuperMPeiDriver);

    if(!Status){
	Status = mSuperMPeiDriver->GetSuperMBoardInfo(
			PeiServices,
			mSuperMPeiDriver,
			&mBoardPeiInfo);

	if(!Status && !mBoardPeiInfo.BMC_Present)
	    Status = EFI_UNSUPPORTED;
    }
    
    if(!ServerMgmtConfiguration.IpmiFunctionSupport)
	RetryCount = 1;

#if EarlyVideo_SUPPORT == 1
    Status = (*PeiServices)->LocatePpi(
		    PeiServices,
		    &gEfiPeiReadOnlyVariable2PpiGuid,
		    0,
		    NULL,
		    &ReadOnlyVariable);

    if(!EFI_ERROR(Status)){
        Status = ReadOnlyVariable->GetVariable(
			ReadOnlyVariable,
			L"Setup",
			&SetupGuid,
			NULL,
			&VariableSize,
			&SetupData);
	  
        if (!EFI_ERROR(Status)) {
            BootProcedureMsgSupport = SetupData.SmcBootProcedureMsg_Support;
        }
    }

    if (BootProcedureMsgSupport) {
	// Write "IPMI Initialization" to VGA
	Status = (*PeiServices)->LocatePpi(
			PeiServices,
			&gEfiPeiSimpleTextOutPPIGuid,
			0,
			NULL,
			&mSimpleTextOutPpi );
        if ( !EFI_ERROR(Status) ) {
#if EarlyVideo_Mode
            Attr = PcdGet8(PcdSmcColor);
            for(DataSize=0; DataSize<50; DataSize++)
                mSimpleTextOutPpi->OutputString (DataSize, SMC_MAX_ROW-2, Attr, strBlank);	
	    
            mSimpleTextOutPpi->OutputString(00, SMC_MAX_ROW-2, Attr, "  PEI--IPMI Initialization..");
            mSimpleTextOutPpi->OutputString(sizeof("  PEI--IPMI Initialization..")-1, SMC_MAX_ROW-2, Attr, ".");
#else
            for(DataSize=0; DataSize<50; DataSize++)
                mSimpleTextOutPpi->OutputString (DataSize, SMC_MAX_ROW-1, 0x07, strBlank);	

            mSimpleTextOutPpi->OutputString(0, SMC_MAX_ROW-1, 0x0F, "  PEI--IPMI Initialization..");
            mSimpleTextOutPpi->OutputString(sizeof("  PEI--IPMI Initialization..")-1 , SMC_MAX_ROW-1, 0x8F, ".");	    
#endif
        }
    }
#endif

    for(Index = 0; (Index < RetryCount) && !Status; Index++){
	if(IoRead8(IPMI_KCS_COMMAND_PORT) != 0xff)
	    break;
	IoWrite8(0x80, 0x55);
	MmioWrite8(SMC_DEBUG_ADDRESS, 0x55);//SMC Debug card support	
	MicroSecondDelay(1000000);
    }

    if(IoRead8(IPMI_KCS_COMMAND_PORT) == 0xff)	Status = EFI_UNSUPPORTED;

    for(Index = 0; (Index < IPMI_SELF_TEST_COMMAND_RETRY_COUNT) && !Status; Index++){
#else
    for (Index = 0; Index < IPMI_SELF_TEST_COMMAND_RETRY_COUNT; Index++) {
#endif
        DataSize = sizeof (BstStatus);
        Status = IpmiInstance->IpmiTransport.SendIpmiCommand (
                    &IpmiInstance->IpmiTransport,
                    IPMI_NETFN_APP,
                    BMC_LUN,
                    IPMI_APP_GET_SELFTEST_RESULTS,
                    NULL,
                    0,
                   (UINT8*) &BstStatus,
                    &DataSize );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get Self test results Index: %x Status %r\n", Index, Status));
        if (!EFI_ERROR(Status)) {
            break;
        }
    }

    DEBUG ((EFI_D_INFO, "Self test result Status: %r\n", Status));

    //
    // If Status indicates a Device error, then the BMC is not responding, so send an error.
    //
    if (EFI_ERROR (Status)) {
        IpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
    } else {

        //
        // Check the self test results.  Cases 55h - 58h are IPMI defined test results.
        // Additional Cases are device specific test results.
        //
        switch (BstStatus.Result) {
            case IPMI_APP_SELFTEST_NO_ERROR: // 0x55
            case IPMI_APP_SELFTEST_NOT_IMPLEMENTED: // 0x56
            case IPMI_APP_SELFTEST_RESERVED: // 0xFF
                IpmiInstance->BmcStatus = EFI_BMC_OK;
                break;

            case IPMI_APP_SELFTEST_ERROR: // 0x57
                IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
                break;

            default: // 0x58 and Other Device Specific Hardware Error
                IpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
                break;
        }

        //
        // The Device ID information is only retrieved when the self test has passed or generated a SOFTFAIL.
        // The BMC Status may be overwritten to show the BMC is in Force Update mode.
        // But the BMC Status will always be preserved if a hard fail has occurred since this code won't execute.
        //
        if ((IpmiInstance->BmcStatus == EFI_BMC_OK) || (IpmiInstance->BmcStatus == EFI_BMC_SOFTFAIL))
        {
            //
            // Send Get Device ID command
            //
            GetDeviceId (IpmiInstance);
        }
    }

    DEBUG ((EFI_D_INFO, "BMC Status %x\n", IpmiInstance->BmcStatus));

    //
    // Now install the PPI if the BMC is not in a Hard Fail State
    //
    if (IpmiInstance->BmcStatus != EFI_BMC_HARDFAIL) {
        PeiIpmiPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
        PeiIpmiPpiDesc->Guid = &gEfiPeiIpmiTransportPpiGuid;
        PeiIpmiPpiDesc->Ppi = &IpmiInstance->IpmiTransport;

        Status = (*PeiServices)->InstallPpi (
                    PeiServices,
                    PeiIpmiPpiDesc);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "gEfiPeiIpmiTransportPpiGuid PPI  Status %r\n", Status));
        if (EFI_ERROR(Status)) {
            //
            // Error in installing the PPI. So free the allocated memory.
            //
            FreePool (IpmiInstance);
            FreePool (PeiIpmiPpiDesc);
        }

#if SMCPKG_SUPPORT
        else if(!EFI_ERROR(Status))
            SmcIPMIPEISetting(PeiServices, IpmiInstance);

        DataSize = MAX_TEMP_DATA;
        CommandData[0] = 0xb5;	// oem command 
        CommandData[1] = 0x0;	// offset 0

check_halt_flag:
	Status = IpmiInstance->IpmiTransport.SendIpmiCommand(
			&IpmiInstance->IpmiTransport,
			0x30,		//EFI_SM_INTEL_OEM
			0,
			0x70,
			CommandData,
			2,
			IpmiInstance->TempData,
			&DataSize);

	if((Status == EFI_SUCCESS) && (IpmiInstance->TempData[0] == 1)){
	    MicroSecondDelay(1000000);	//wait for 1s
	    goto check_halt_flag;
	}
#endif
        return Status;

    } else {
        //
        // SELF test has failed, so free the memory and return EFI_UNSUPPORTED to unload driver from memory.
        //
        FreePool (IpmiInstance);
        FreePool (PeiIpmiPpiDesc);
        return EFI_UNSUPPORTED;
    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
