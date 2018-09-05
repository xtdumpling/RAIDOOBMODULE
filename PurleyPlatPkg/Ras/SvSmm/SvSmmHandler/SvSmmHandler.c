/**

Copyright (c) 2008 - 2014, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


   @file SvSmmHandler.c

  SV SMM Driver implementation.

**/

#include "SvSmmHandler.h"
#include <Library/BaseMemoryLib.h>

//
// Modular variables needed by this driver
//

BIOS_ACPI_PARAM       		  *mAcpiParameter;
SV_SMM_PARAMETER      		  *mSvSmmExtenderPtr;
EFI_SV_SERVICE_PROTOCOL       *mSvService;
EFI_QUIESCE_SUPPORT_PROTOCOL  *mQuiesceSupport = NULL;
EFI_MEM_RAS_PROTOCOL          *mMemRas = NULL;
EFI_CPU_CSR_ACCESS_PROTOCOL   *mCpuCsrAccess;

//MyTest Stuff
static UINT32 MaxIternation = 1;

VOID MyTest(VOID);

//Test using pointer:
QUIESCE_DATA_TABLE_ENTRY *MyQuiesceDataPtr;

//Testing code
QUIESCE_DATA_TABLE_ENTRY MyQuiesceTest[] = {
  //write to Socket0 FW 11
  {
    EnumQuiesceWrite,       //operation
    4, //dword              //AccessWidth
    BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG,     //CsrAddress
    0,                      //AndMask
    QUIESCE_TEST_DATA1,             //WriteData
  },

  //write to Socket1 FW 11
  {
    EnumQuiesceWrite,
    4, //dword
    BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG,
    0,
    QUIESCE_TEST_DATA1,
  },

  //write to Socket2 FW 11
  {
    EnumQuiesceWrite,
    4, //dword
	BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG,
    0,
    QUIESCE_TEST_DATA1,
  },

  //write to Socket3 FW 11
  {
    EnumQuiesceWrite,
    4, //dword
	BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG,
    0,
    QUIESCE_TEST_DATA1,
  },

  //poll Socket0 FW 11
  {
    EnumQuiescePoll,        //poll 
    4,           			//4byte data
    BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG,  //FW 11 CSR address
    0xFF,        			//AndMask
    QUIESCE_TEST_DATA2,        			//is byte0 match A5?
  },
/***
  //poll Socket1 FW 11
  {
    EnumQuiescePoll,        //poll 
    4,           			//4byte data
    BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG,  //FW 11 CSR address
    0xFF00,        			//AndMask
    0xA500,        			//is byte0 match A5?
  },

  //poll Socket2 FW 11
  {
    EnumQuiescePoll,        //poll 
    4,           			//4byte data
    BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG,  //FW 11 CSR address
    0xFFFF0000,    			//AndMask
    0xA5A50000,    			//is byte0 match A5?
  },

  //poll Socket3 FW 11
  {
    EnumQuiescePoll,        //poll 
    4,           			//4byte data
    BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG,  //FW 11 CSR address
    0x00FF00FF,  			//AndMask
    0x00A500A5,  			//Is byte2/1 match?
  },
  {
    EnumQuiesceOperationEnd,
    0,
    0,
    0,
    0,
  }
**/
};

//
// Prototypes
//
EFI_STATUS
SvSmmCallback (
    IN EFI_HANDLE  DispatchHandle,
    IN CONST VOID  *Context         OPTIONAL,
    IN OUT VOID    *CommBuffer      OPTIONAL,
    IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

EFI_STATUS
RegisterToDispatchDriver (
  VOID
  );

EFI_STATUS
GetSvCommand(
  IN OUT UINT64 *Command
  );


// Stuff related to ASM begin
UINTN
ReadCr3 (
   VOID
  );

EFI_STATUS
BuildMyQuiesceTstData(
  VOID
);

// stuff related ASM end

//
// Driver entry point
//
/**

  Initializes the SMM SV Handler Driver.

  @param ImageHandle  -  The image handle of Wake On Lan driver.
  @param SystemTable  -  The starndard EFI system table.
    
  @retval EFI_OUT_OF_RESOURCES  -  Insufficient resources to complete function.
  @retval EFI_SUCCESS           -  Function has completed successfully.
  @retval Other                 -  Error occured during execution. 

**/
EFI_STATUS
EFIAPI
InitSvSmmHandler (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS            Status;
  UINTN                 Addr;

  EFI_GLOBAL_NVS_AREA_PROTOCOL    *AcpiNvsProtocol = NULL; 

  if (!PcdGetBool(PcdDfxAdvDebugJumper)) {
    Status = EFI_SUCCESS;
    return Status;
  }
  DEBUG((EFI_D_INFO, "Advanced Debug Jumper set - Load SMM SV Handler Driver\n"));

  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid, 
                  NULL, 
                  &AcpiNvsProtocol
                  );
  ASSERT_EFI_ERROR (Status);
  mAcpiParameter = AcpiNvsProtocol->Area;
  ASSERT (mAcpiParameter);

  Status = gBS->LocateProtocol (&gEfiSvSmmProtocolGuid, NULL, &mSvService);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiMemRasProtocolGuid, NULL, &mMemRas);
  ASSERT_EFI_ERROR(Status);

  Status = gBS->LocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  if (EFI_ERROR(Status))return Status;

  //Init Quiesce Comm Data struct
  Status = gSmst->SmmLocateProtocol (&gEfiQuiesceProtocolGuid, NULL, &mQuiesceSupport);
  ASSERT_EFI_ERROR (Status);

  mSvService->AcpiService.GetSvosGasAddress(&Addr);

  //Init SvSmmExtender
  mSvSmmExtenderPtr = (SV_SMM_PARAMETER *)Addr;
  
  mSvService->SmmService.SVExtenderHandler = NULL; //SVExtenderHandler; 

  //DEBUG ((EFI_D_ERROR, "Update SvSmmExtenderAddress @ %x Addr %x\n", mSvSmmExtenderPtr, Addr));

  //Register SV SMM handler Callback
  Status = RegisterToDispatchDriver ();
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Register SvSmmHandler failed: %x\n", Status));
    return Status;
  }

  SetMem(mSvSmmExtenderPtr, 0x1000, 0);     //clear SVOS ACPI GAS (4K)

  return Status;
}

/**

  Register to dispatch driver.
  
  @param None.
      
  @retval EFI_SUCCESS  -  Successfully init the device.
  @retval Other        -  Error occured whening calling Dxe lib functions.
  
**/
EFI_STATUS
RegisterToDispatchDriver (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    SvosSmmHandle;
  EFI_SMM_SW_DISPATCH2_PROTOCOL *SvosSwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT   SvosSwContext;
  UINT8                         SvosSmiDoorBell;


  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SvosSwDispatch);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Register SVOS SMM Handler
  //
  mSvService->AcpiService.GetSvosSmiDoorBell(&SvosSmiDoorBell);
  SvosSwContext.SwSmiInputValue = SvosSmiDoorBell; //mSvSmm->SmiDoorBell;
  Status = SvosSwDispatch->Register (SvosSwDispatch, SvSmmCallback, &SvosSwContext, &SvosSmmHandle);

  return Status;
}

/**

    Setup and call SV specific Quiesce handling.

    @param MonarchSktId      -Monarch Socket ID
    @param SvQuiesceCodeAddr - Entry point of SV Quiesce code
    @param SvQuiesceCodeSize - size of SV quiesce code
    @retval EFI_SUCCESS  -  Successful setup and execution of quiesce code
    @retval Other        -  Error occurred

**/
EFI_STATUS
RunSvQuiesceCode(
  IN UINT8  MonarchSktId,
  IN UINT64 SvQuiesceCodeAddr,
  IN UINT32 SvQuiesceCodeSize
  )
{
    EFI_STATUS                  status;
    QUIESCE_DATA_TABLE_ENTRY    *quiesceTableEntry;

    //
    // Setup  quiesce buffer to run Sv Quiesce code
    //
    status = mQuiesceSupport->AquireDataBuffer((EFI_PHYSICAL_ADDRESS *)(UINTN)&quiesceTableEntry);
    ASSERT_EFI_ERROR(status);
    quiesceTableEntry->Operation = EnumQuiesceRunSvQuiesceCode;
    quiesceTableEntry++;
    status = mQuiesceSupport->ReleaseDataBuffer((EFI_PHYSICAL_ADDRESS)(UINTN)quiesceTableEntry);

    return status;
}

/**

  Callback function entry for Sx sleep state.

  @param DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param Context         Points to an optional handler context which was specified when the
                         handler was registered.
  @param CommBuffer      A pointer to a collection of data in memory that will
                         be conveyed from a non-SMM environment into an SMM environment.
  @param CommBufferSize  The size of the CommBuffer.
    
  @retval EFI_SUCCESS            -  Operation successfully performed.
  @retval EFI_INVALID_PARAMETER  -  Invalid parameter passed in.
  
**/
EFI_STATUS
SvSmmCallback (
    IN EFI_HANDLE  DispatchHandle,
    IN CONST VOID  *Context         OPTIONAL,
    IN OUT VOID    *CommBuffer      OPTIONAL,
    IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS  Status;
  UINT64      Command;
  UINT32      Data32;
  UINT8	      SocketId = 0;

  GPIO_PAD    GpioPad;
  UINT32      InvertState;

  // Add SVOS ACPI (at offset 0xFE0) counter for SV SMIRAND test - start
  mSvSmmExtenderPtr->Parameter[508] += 1;  

  // Add CPU Non-sticky scratchpad15 to use as counter for SV SMIRAND test - start
  Data32 = mCpuCsrAccess->ReadCpuCsr(SocketId, 0, BIOSNONSTICKYSCRATCHPAD15_UBOX_MISC_REG);
  Data32++;
  mCpuCsrAccess->WriteCpuCsr(SocketId, 0, BIOSNONSTICKYSCRATCHPAD15_UBOX_MISC_REG, Data32);

  GetSvCommand(&Command);
  if ((Command == SVMemoryFailOver) || (Command == SVMemoryOnOffLine) || (Command == SVMemoryMigration) || (Command == SVIohOnOffline) || (Command == SVCpuOnOffline)) {
	
    GpioPad = GPIO_SKL_H_GPP_E6;
    Status = GpioGetInputInversion (GpioPad,&InvertState);
    InvertState = InvertState ^ 1;
    Status = GpioSetInputInversion(GpioPad,InvertState);
  }
  
  Status = EFI_SUCCESS;

  if(mSvSmmExtenderPtr->Command == 0xFFFFFFFE) {
    CpuDeadLoop();
    MyTest();
  }

  GetSvCommand(&Command);
  
  // 
  switch(Command) {
    case SVQuiesceUnQuiesce:
      Status = mQuiesceSupport->QuiesceMain(); 
      //
      // Update Status on Sv Gas struct
      // For Quiesce/UnQuiesce, status returns on Paramter[0]
      //
      if(Status == EFI_SUCCESS) {
        mSvSmmExtenderPtr->Parameter[0] = 1;
        //
        // Update counter (at offset 0xff8 from SVOS GAS base) for SV Quiesce back 2 back
        // Remember, command also need 8byte
        //
        mSvSmmExtenderPtr->Parameter[510] += 1; 
      } else {
        mSvSmmExtenderPtr->Parameter[0] = 2;
        //
        // Update failure count
        //
        mSvSmmExtenderPtr->Parameter[509] += 1;         
      }        
      break;

    case SVMemoryChipUnerase:
    case SVChipSparing_StartSpare:
    case SVMemoryPatrolScrub:
      mSvSmmExtenderPtr->Parameter[2]      = IN_PROGRESS;
      mMemRas->B2BOperation( Command, mSvSmmExtenderPtr->Parameter );
      break;

    case SVMemoryFailOver:                  
      mAcpiParameter->SmiRequestParam[0]   = SMI_MEM_MIRROR_REPLACE;
      mAcpiParameter->SmiRequestParam[1]   = (UINT32)(((mSvSmmExtenderPtr->Parameter[0] & 0xFF)<<24) | (mSvSmmExtenderPtr->Parameter[1] & 0xFFFF));
      mAcpiParameter->SmiRequestParam[2]   = 0;
      mAcpiParameter->SmiRequestParam[3]   = (UINT32)(UINTN)&mSvSmmExtenderPtr->Parameter[2];
      mSvSmmExtenderPtr->Parameter[2]      = IN_PROGRESS;
      break;

    case SVMemoryOnOffLine:
      mAcpiParameter->SmiRequestParam[0]   = SMI_MEM_ONLINE_OFFLINE;
      mAcpiParameter->SmiRequestParam[1]   = (UINT32)(((mSvSmmExtenderPtr->Parameter[0] & 0xFF)<<24) | (mSvSmmExtenderPtr->Parameter[1] & 0x00FF));      
      mAcpiParameter->SmiRequestParam[2]   = 0;
      mAcpiParameter->SmiRequestParam[3]   = (UINT32)(UINTN)&mSvSmmExtenderPtr->Parameter[2];
      mSvSmmExtenderPtr->Parameter[2]      = IN_PROGRESS;
      break;

    case SVMemoryMigration:
      mAcpiParameter->SmiRequestParam[0]   = SMI_MEM_SPARE_MIGRATION;
      mAcpiParameter->SmiRequestParam[1]   = (UINT32)((mSvSmmExtenderPtr->Parameter[2] & 0xFF) << 24); 
      mAcpiParameter->SmiRequestParam[1]  |= (UINT32)((mSvSmmExtenderPtr->Parameter[0] & 0xFF) << 8); 
      mAcpiParameter->SmiRequestParam[1]  |= (UINT32)(mSvSmmExtenderPtr->Parameter[1] & 0xFF); 
      mAcpiParameter->SmiRequestParam[2]   = 0;
      mAcpiParameter->SmiRequestParam[3]   = (UINT32)(UINTN)&mSvSmmExtenderPtr->Parameter[3];
      mSvSmmExtenderPtr->Parameter[3]      = IN_PROGRESS;
      break;

    case SVIohOnOffline:
      mAcpiParameter->SmiRequestParam[0]   = SMI_IOH_ONLINE_OFFLINE;
      mAcpiParameter->SmiRequestParam[1]   = (UINT32) mSvSmmExtenderPtr->Parameter[0];  // 0 for onlining, 1 for offlining.
      mAcpiParameter->SmiRequestParam[2]   = (UINT32) mSvSmmExtenderPtr->Parameter[1];  // Processor socket ID.
      mAcpiParameter->SmiRequestParam[3]   = (UINT32)(UINTN)&mSvSmmExtenderPtr->Parameter[3];
      mSvSmmExtenderPtr->Parameter[3]      = IN_PROGRESS;      
      mAcpiParameter->SciRequestParam[0]   = 0; // First time calling IOH RAS handler.  
      break;

    case SVCpuOnOffline:
      mAcpiParameter->SmiRequestParam[0]   = SMI_CPU_ONLINE_OFFLINE;
      mAcpiParameter->SmiRequestParam[1]   = (UINT32) mSvSmmExtenderPtr->Parameter[0];  // 0 for onlining, 1 for offlining.
      mAcpiParameter->SmiRequestParam[2]   = (UINT32) mSvSmmExtenderPtr->Parameter[1];  // Processor socket ID.
      mAcpiParameter->SmiRequestParam[3]   = (UINT32)(UINTN)&mSvSmmExtenderPtr->Parameter[3];
      mAcpiParameter->SciRequestParam[0]   = 0; // First time calling CPU RAS handler.  
      mSvSmmExtenderPtr->Parameter[3]      = IN_PROGRESS;
      break;  

    case SVQuiesceCodeRun:
        // IN Param[0]bits[7:0] is Monarch Socket ID
        // IN Param[1]bits [63:0] is Entry point of SV Quiesce code
        // IN Param[2] [31:0] is size of SV quiesce code
        // OUT Param[3] - Status is returned in param 3
      mSvSmmExtenderPtr->Parameter[3] = RunSvQuiesceCode(
              (UINT8) mSvSmmExtenderPtr->Parameter[0],
              mSvSmmExtenderPtr->Parameter[1],
              (UINT32)mSvSmmExtenderPtr->Parameter[2]
                       );
      break;
    default:
      Status = EFI_INVALID_PARAMETER; //EFI_UNSUPPORTED;
      break;
  }

  return Status;
}


/**

    Get SV command

    @param Command - pointer to populate the SV command

    @retval EFI_SUCCESS - Successfully updated the pointer to SVCommand

**/
EFI_STATUS
GetSvCommand(
  IN OUT UINT64 *Command
  )
{
  *Command = mSvSmmExtenderPtr->Command;
  
  return EFI_SUCCESS;

}

/**

    SV Extended handler

    @param None

    @retval EFI_SUCCESS - Operation Successful

**/
EFI_STATUS
EFIAPI
SVExtenderHandler(
  VOID)
{

  return EFI_SUCCESS;

}

/**

    Build Quiesce test data for test call

    @param None

    @retval EFI_SUCCESS  -  Successful setup and execution of quiesce code
    @retval Other        -  Error occurred

**/
EFI_STATUS
BuildMyQuiesceTstData(
  VOID
)
{
  EFI_STATUS            Status;
  UINT8                 Size = 4;

  Status = EFI_SUCCESS;

  //
  // Init Data write a5a5a5a5 to socket0 FW 11
  //
  MyQuiesceDataPtr[0].Operation = EnumQuiesceWrite;
  MyQuiesceDataPtr[0].AccessWidth = 4;
  MyQuiesceDataPtr[0].CsrAddress = (UINT64)mCpuCsrAccess->GetCpuCsrAddress (0, 0, BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG, &Size);
  MyQuiesceDataPtr[0].AndMask = 0;
  MyQuiesceDataPtr[0].DataMask.Data = QUIESCE_TEST_DATA1;

  //
  // Write a5a5a5a5 to socket1 FW 11
  //
  MyQuiesceDataPtr[1].Operation = EnumQuiesceWrite;
  MyQuiesceDataPtr[1].AccessWidth = 4;
  MyQuiesceDataPtr[1].CsrAddress = (UINT64)mCpuCsrAccess->GetCpuCsrAddress (1, 0, BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG, &Size);
  MyQuiesceDataPtr[1].AndMask = 0;
  MyQuiesceDataPtr[1].DataMask.Data = QUIESCE_TEST_DATA1;

  MyQuiesceDataPtr++;
  MyQuiesceDataPtr++;

  //
  // Write a5a5a5a5 to socket2 FW 11
  //
  MyQuiesceDataPtr->Operation = EnumQuiesceWrite;
  MyQuiesceDataPtr->AccessWidth = 4;
  MyQuiesceDataPtr->CsrAddress = (UINT64)mCpuCsrAccess->GetCpuCsrAddress (2, 0, BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG, &Size);
  MyQuiesceDataPtr->AndMask = 0;
  MyQuiesceDataPtr->DataMask.Data = QUIESCE_TEST_DATA1;

  MyQuiesceDataPtr++;

  //
  // Write a5a5a5a5 to socket3 FW 11
  //
  MyQuiesceDataPtr->Operation = EnumQuiesceWrite;
  MyQuiesceDataPtr->AccessWidth = 4;
  MyQuiesceDataPtr->CsrAddress = (UINT64)mCpuCsrAccess->GetCpuCsrAddress (3, 0, BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG, &Size);
  MyQuiesceDataPtr->AndMask = 0;
  MyQuiesceDataPtr->DataMask.Data = QUIESCE_TEST_DATA1;

  MyQuiesceDataPtr++;
  //
  // Poll Socket0 FW 11 byte0
  //
  MyQuiesceDataPtr->Operation = EnumQuiescePoll;
  MyQuiesceDataPtr->AccessWidth = 4;
  MyQuiesceDataPtr->CsrAddress = (UINT64)mCpuCsrAccess->GetCpuCsrAddress (0, 0, BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG, &Size);
  MyQuiesceDataPtr->AndMask = 0xFF;
  MyQuiesceDataPtr->DataMask.Data = QUIESCE_TEST_DATA2;
  
/******
  MyQuiesceDataPtr++;
  //
  // Poll Socket1 FW 11 byte1
  //
  MyQuiesceDataPtr->Operation = EnumQuiescePoll;
  MyQuiesceDataPtr->AccessWidth = 4;
  MyQuiesceDataPtr->CsrAddress = (UINT64)mCpuCsrAccess->GetCpuCsrAddress (1, 0, BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG, &Size);
  MyQuiesceDataPtr->AndMask = 0xFF00;
  MyQuiesceDataPtr->DataMask.Data = 0xa500;

  MyQuiesceDataPtr++;
  //
  // Poll Socket2 FW 11 byte3/2
  //
  MyQuiesceDataPtr->Operation = EnumQuiescePoll;
  MyQuiesceDataPtr->AccessWidth = 4;
  MyQuiesceDataPtr->CsrAddress = (UINT64)mCpuCsrAccess->GetCpuCsrAddress (2, 0, BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG, &Size);
  MyQuiesceDataPtr->AndMask = 0xFFFF0000;
  MyQuiesceDataPtr->DataMask.Data = 0xa5a50000;

  MyQuiesceDataPtr++;
  //
  // Poll Socket2 FW 11 byte2/0
  //
  MyQuiesceDataPtr->Operation = EnumQuiescePoll;
  MyQuiesceDataPtr->AccessWidth = 4;
  MyQuiesceDataPtr->CsrAddress = (UINT64)mCpuCsrAccess->GetCpuCsrAddress (3, 0, BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG, &Size);
  MyQuiesceDataPtr->AndMask = 0x00FF00FF;
  MyQuiesceDataPtr->DataMask.Data = 0x00a500a5;

  // 
  // End of operation
  //
  //MyQuiesceDataPtr->Operation = EnumQuiesceOperationEnd;

  //
  // Point to begining again
  //
  //MyQuiesceDataPtr = (QUIESCE_DATA_TABLE_ENTRY *)DatBufferAddr;
*********/

  return Status;

}


VOID MyTest(VOID)
{
 EFI_STATUS Status;
 UINTN  QuiesceDataBaseAddress;
 UINTN  QuiesceDataSize;
 EFI_PHYSICAL_ADDRESS  DataBuffAddress;
 UINT32   Size;
 UINT32  i;

  //DataBuffer full test Start
  mQuiesceSupport->GetQuiesceDataMemoryInfo(&QuiesceDataBaseAddress, &QuiesceDataSize);

  Size = (UINT32)QuiesceDataSize;

  for(i = 0; i < MaxIternation; i++) {
    Status = mQuiesceSupport->AquireDataBuffer(&DataBuffAddress);

    if(Status == EFI_SUCCESS) {
  
      if(DataBuffAddress >=  QuiesceDataBaseAddress) {
        Size = (UINT32)QuiesceDataSize - (UINT32) (DataBuffAddress - QuiesceDataBaseAddress); 
      } else { 
        Size = (UINT32)QuiesceDataSize - (UINT32) (QuiesceDataBaseAddress - DataBuffAddress); 
      }

      if(Size > sizeof(MyQuiesceTest)) {
        MyQuiesceDataPtr = (QUIESCE_DATA_TABLE_ENTRY *)DataBuffAddress;
        Status = BuildMyQuiesceTstData();

        Status = mQuiesceSupport->ReleaseDataBuffer((EFI_PHYSICAL_ADDRESS)MyQuiesceDataPtr);
  

      } else {
        Status = mQuiesceSupport->ReleaseDataBuffer((EFI_PHYSICAL_ADDRESS)MyQuiesceDataPtr);
        Status = EFI_INVALID_PARAMETER;
        MaxIternation = 1; //reset Max Counter
      }

    }

    if(Status != EFI_SUCCESS) {
      break;
    }
    
    DataBuffAddress = 0;
    Size = 0;
  }
  //Peter's DataBuffer full test End

  MaxIternation++;

}
