//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
AMI defined Protocol header file for HDD Smart Protocol
 **/

#ifndef __AMI_HDD_SMART_PROTOCOL__H__
#define __AMI_HDD_SMART_PROTOCOL__H__

/**
 * HDD Smart Protocol Definition
*/
#define AMI_HDD_SMART_PROTOCOL_GUID \
        { 0xFFBD9AD2, 0xF1DB, 0x4F92, { 0xA6, 0x49, 0xEB, 0x9E, 0xED, 0xEA, 0x86, 0xB5 } }

typedef struct _AMI_HDD_SMART_PROTOCOL  AMI_HDD_SMART_PROTOCOL;

/**
    Starts Smart SelfTest. SelfTestType will indicate whether to run
    short or extended self test.

    @param This     Ami Smart Protocol Interface 
    @param SelfTestType ( 0 : short, 1 :extended)
    @param PollPeriod (Minutes)

    @retval 
        EFI_STATUS
        PollPeriod will indicate the recommended time interval between polling.
        Frequent polling will either abort the self test or may prolong it.


**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SMART_SELF_TEST) (
    IN  AMI_HDD_SMART_PROTOCOL      *This,
    IN  UINT8                       SelfTestType,
    OUT UINT16                      *PollPeriod
);

/**
    Starts Smart SelfTest. SelfTestType will indicate whether to run
    short or extended self test.

    @param This     Ami Smart Protocol Interface 
    @param TestStatus 

    @retval 
        EFI_STATUS : TestStatus is valid only when EFI_STATUS is EFI_SUCCESS
        TestStatus : Will indicate the % of test completed.
        TestStatus = 0xFF Indicates Self Test Failed and EFI_STATUS = EFI_ERROR
  Example: When TestStatus = 10, 10% of the test has been done.

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SMART_SELF_TEST_STATUS) (
    IN  AMI_HDD_SMART_PROTOCOL      *This,
    OUT UINT8                       *TestStatus
);

/**
    Returns Smart status

    @param This     AMI Smart Protocol Interface 
	@param SmartFailure - SMART Status of the HDD

    @retval EFI_SUCCESS  SMART status has been returned via SmartFailure pointer.
**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SMART_RETURN_STATUS) (
    IN  AMI_HDD_SMART_PROTOCOL   *This,
    OUT BOOLEAN                  *SmartFailure
);

/**
    Sends any Non-Data Smart Command. 

    @param This         Ami Smart Protocol Interface 
    @param SubCommand 
    @param AutoSaveEnable   OPTIONAL
    @param SelfTestType     OPTIONAL

    @retval 
        EFI_STATUS

    @note  Returns EFI_UNSUPPORTED when Smart feature is not enabled.

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SEND_SMART_COMMAND) (
    IN AMI_HDD_SMART_PROTOCOL       *This,
    IN UINT8                        SubCommand,
    IN UINT8                        AutoSaveEnable, 
    IN UINT8                        SelfTestType    
);

/**
    Sends any Data In Smart command. 

    @param This         Ami Smart Protocol Interface  
    @param SubCommand   Smart Command
    @param Buffer       Buffer Address
    @param LogAddress   Log Address     //OPTIONAL    
    @param SectorCount  Sector Count    //OPTIONAL  
    @retval 
        EFI_STATUS

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SMART_READ_DATA) (
    IN  AMI_HDD_SMART_PROTOCOL      *This,
    IN  UINT8                       SubCommand,
    OUT VOID                        *Buffer,
    IN  UINT8                       LogAddress,   
    IN  UINT8                       SectorCount   
);

/**
    Sends any Data Out Smart command.

    @param This         Ami Smart Protocol Interface 
    @param SubCommand   Smart Command
    @param Buffer       Buffer Address
    @param LogAddress   Log Address
    @param SectorCount  Sector Count
    @retval 
        EFI_STATUS

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SMART_WRITE_DATA) (
    IN AMI_HDD_SMART_PROTOCOL       *This,
    IN UINT8                        SubCommand,
    IN VOID                         *Buffer,
    IN UINT8                        LogAddress,    
    IN UINT8                        SectorCount    
);

/**
    Aborts Smart Self Test. 
        
    @param This     Ami Smart Protocol Interface

    @retval 
        EFI_STATUS

**/
typedef 
EFI_STATUS (EFIAPI *AMI_HDD_SMART_ABORT_SELF_TEST) (
    IN  AMI_HDD_SMART_PROTOCOL      *This
);

/**
    Get Smart data of the ATA device.
        
    @param This     Ami Smart Protocol Interface 

    @retval 
        OUT UINT8   **SmartData

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SMART_GET_SMART_DATA) (
    IN  AMI_HDD_SMART_PROTOCOL      *This,
    OUT UINT8                       **SmartData
);

struct _AMI_HDD_SMART_PROTOCOL { 
    UINT8                               Revision;
    UINT32                              Signature;
    AMI_HDD_SMART_SELF_TEST             SmartSelfTest;
    AMI_HDD_SMART_SELF_TEST_STATUS      SmartSelfTestStatus;
    AMI_HDD_SMART_RETURN_STATUS         SmartReturnStatus;
    AMI_HDD_SEND_SMART_COMMAND          SendSmartCommand;
    AMI_HDD_SMART_READ_DATA             SmartReadData;
    AMI_HDD_SMART_WRITE_DATA            SmartWriteData;
    AMI_HDD_SMART_ABORT_SELF_TEST       SmartAbortSelfTest;
    AMI_HDD_SMART_GET_SMART_DATA        SmartGetSmartData;
};

extern EFI_GUID gAmiHddSmartProtocolGuid;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
