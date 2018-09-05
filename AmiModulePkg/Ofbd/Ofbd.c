//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file 
Ofbd.c

This is the main file of OFBD module. We setup OFBD SMI handler here. 

Please also refer to Aptio SMM Module Porting guide.

*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <Efi.h>
#include <Token.h>
#include <Flash.h>
#include <Ofbd.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
//APTIOV_SERVER_OVERRIDE_START
//#if PI_SPECIFICATION_VERSION >= 0x1000A
//APTIOV_SERVER_OVERRIDE_END
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#define RETURN(status) {return status;}
//APTIOV_SERVER_OVERRIDE_START
//#else
//#include <Protocol/SmmBase.h>
//#include <Protocol/SmmSwDispatch.h>
//#define RETURN(status) {return ;}
//#endif
//APTIOV_SERVER_OVERRIDE_END
#include <Protocol/DevicePath.h>
#include <OfbdFuncInc.h>	// Build directory
#include <OfbdFuncElinks.h>	// Build directory
#if OFBD_FOR_ARM_SUPPORT == 1
#include <Protocol/AmiFri.h>
#else
#include <AmiSmm.h>
#include <Library/AmiBufferValidationLib.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Vars
//---------------------------------------------------------------------------
EFI_GUID gSwSmiCpuTriggerGuid = SW_SMI_CPU_TRIGGER_GUID;
//APTIOV_SERVER_OVERRIDE_START
//#if PI_SPECIFICATION_VERSION >= 0x1000A
//APTIOV_SERVER_OVERRIDE_END
EFI_GUID gEfiSmmCpuProtocolGuid = EFI_SMM_CPU_PROTOCOL_GUID;
EFI_SMM_BASE2_PROTOCOL          *gSmmBase2 = NULL;
EFI_SMM_CPU_PROTOCOL            *gSmmCpu = NULL;
//APTIOV_SERVER_OVERRIDE_START
//#else
//EFI_GUID gEfiSmmSwDispatchProtocolGuid = EFI_SMM_SW_DISPATCH_PROTOCOL_GUID;
//#endif
//APTIOV_SERVER_OVERRIDE_END
#endif
UINT64							gU64TempVPT = 0;

static OFBD_INIT_PARTS_FUNC * OFBDInitPartsTbl[] =
{
    OFBD_INIT_FUNC_LIST
    NULL
};

static OFBD_INIT_SMM_FUNC * OFBDInSMMFuncTbl[] =
{
    OFBD_IN_SMM_FUNC_LIST
    NULL
};

static OFBD_INIT_SMM_FUNC * OFBDNotInSMMFunc[] =
{
    OFBD_NOT_SMM_FUNC_LIST
    NULL
};

//---------------------------------------------------------------------------
/**
    Ofbd (Convert Buffer Pointer)

	This function will to convert the unknown virtual address to runtime pointer
	
    @param Buffer - Ofbd header.

    @return VOID
*/ 
VOID OfbdConvertBufferPointer(
	IN VOID             *Buffer
)
{
	
    OFBD_HDR            * pOFBDHdr = 0;
    OFBD_EXT_HDR        * pOFBDExtHdr = 0; 
    UINT8               * pOFBDTblEnd = 0;
    UINT64              * pTempVPT = 0;
    OFBD_TC_54_STD_EC_STRUCT * pECStructPtr = 0;
    
    pOFBDHdr = (OFBD_HDR *)Buffer;
    pOFBDExtHdr = (OFBD_EXT_HDR *)((UINT8 *)Buffer + pOFBDHdr->OFBD_HDR_SIZE);
    pOFBDTblEnd = (UINT8 *)((UINT8 *)pOFBDHdr + pOFBDHdr->OFBD_Size);
        
    pTempVPT = (UINT64 *)((UINT8 *)pOFBDTblEnd - sizeof(OFBD_END));
    gU64TempVPT = *pTempVPT;
   	
   	//TRACE((-1, "[OFBD] Buffer(%08X)\n", Buffer);)
   	//TRACE((-1, "[OFBD] gU64TempVPT(%08X)\n", gU64TempVPT);)
   	//TRACE((-1, "[OFBD] 0 pOFBDEnd->OFBD_END(%08X)\n", *pTempVPT);)
   	   	
    switch (pOFBDExtHdr->TypeCodeID)
    {
        case OFBD_EXT_TC_EC:
            pECStructPtr = (OFBD_TC_54_STD_EC_STRUCT *)((UINT8 *)pOFBDExtHdr + sizeof(OFBD_EXT_HDR));
            if (pECStructPtr->dbFlaSts & OFBD_TC_STD_EC_FLASH)
                *pTempVPT = (UINT64)(VOID *)((UINT8 *)pOFBDTblEnd + 128);	// Skip OEM string length, to avoid error message return function invalid.
            break;
    }
   	
   	//TRACE((-1, "[OFBD] 1 pOFBDEnd->OFBD_END(%08X)\n", *pTempVPT);)
}

//---------------------------------------------------------------------------
/**
    Ofbd (Virtual Notify Event)

	This function will be invoked to convert runtime pointers to virtual address
	
    @param EFI_EVENT - Not used.
    @param VOID - Not used.

    @return VOID
*/ 
VOID OfbdVirtualNotifyEvent (
	EFI_EVENT				Event,
	VOID					*Context
)
{
    UINTN		    i = 0;
    FlashVirtualFixup(pRS);
    for (i = 0; OFBDInitPartsTbl[i] != NULL; i++)
    {
        pRS->ConvertPointer(0, (VOID **)&(OFBDInitPartsTbl[i]));
    }
}

/**
    Ofbd (Smi Handles)

	The SMI handler of Ofbd services.
	
    @param DispatchHandle - Dispatch Handle.
    @param DispatchContext - Dispatch Context.

    @return EFI_STATUS
    @retval EFI_SUCCESS Function executed successfully
*/ 

//APTIOV_SERVER_OVERRIDE_START
//#if PI_SPECIFICATION_VERSION >= 0x1000A
//APTIOV_SERVER_OVERRIDE_END
EFI_STATUS 
OFBDSMIHandler (
        IN EFI_HANDLE                  	DispatchHandle,
		IN CONST VOID                   *Context OPTIONAL,
		IN OUT VOID                     *CommBuffer OPTIONAL,
		IN OUT UINTN                    *CommBufferSize OPTIONAL
)
//APTIOV_SERVER_OVERRIDE_START
//#else
//VOID OFBDSMIHandler (
//    IN  EFI_HANDLE                  DispatchHandle,
//    IN  EFI_SMM_SW_DISPATCH_CONTEXT *DispatchContext
//)
//#endif
//APTIOV_SERVER_OVERRIDE_END
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINTN		i = 0;
    OFBD_HDR    *OFBDHeader = NULL;
    UINT8       OFBDDataHandled = 0;

#if OFBD_FOR_ARM_SUPPORT == 0
//APTIOV_SERVER_OVERRIDE_START
//#if PI_SPECIFICATION_VERSION < 0x1000A    
//    EFI_SMM_CPU_SAVE_STATE	*pCpuSaveState = NULL;
//    SW_SMI_CPU_TRIGGER		*SwSmiCpuTrigger = NULL;
//#endif
//APTIOV_SERVER_OVERRIDE_END

    UINTN       Cpu = /*pSmstPi->CurrentlyExecutingCpu - 1;*/(UINTN)-1;
    UINT8		Data = 0;
    UINT64		BuffAddr = 0;
    UINT32		HighBufferAddress = 0;
    UINT32		LowBufferAddress = 0;

//APTIOV_SERVER_OVERRIDE_START
//#if PI_SPECIFICATION_VERSION >= 0x1000A
//APTIOV_SERVER_OVERRIDE_END

	Cpu = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->SwSmiCpuIndex;
    //
    // Found Invalid CPU number, return 
    //
    if(Cpu == (UINTN)-1) RETURN(Status);

    Status = gSmmCpu->ReadSaveState ( gSmmCpu, \
                                      4, \
                                      EFI_SMM_SAVE_STATE_REGISTER_RBX, \
                                      Cpu, \
                                      &LowBufferAddress );
    Status = gSmmCpu->ReadSaveState ( gSmmCpu, \
                                      4, \
                                      EFI_SMM_SAVE_STATE_REGISTER_RCX, \
                                      Cpu, \
                                      &HighBufferAddress );
                                                                                                      
    Data = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->CommandPort;
    	
//APTIOV_SERVER_OVERRIDE_START
//#else
//    for (i = 0; i < pSmstPi->NumberOfTableEntries; ++i) {
//        if (guidcmp(&pSmstPi->SmmConfigurationTable[i].VendorGuid, \
//                                        &gSwSmiCpuTriggerGuid) == 0) {
//            break;
//        }
//    }
//
//    //If found table, check for the CPU that caused the software Smi.
//    if (i != pSmstPi->NumberOfTableEntries) {
//        SwSmiCpuTrigger = pSmstPi->SmmConfigurationTable[i].VendorTable;
//        Cpu = SwSmiCpuTrigger->Cpu;
//    }
//
//    Data = (UINT8)DispatchContext->SwSmiInputValue;
//
//    pCpuSaveState = (EFI_SMM_CPU_SAVE_STATE *)pSmstPi->CpuSaveState;
//    HighBufferAddress = pCpuSaveState[Cpu].Ia32SaveState.ECX;
//    LowBufferAddress = pCpuSaveState[Cpu].Ia32SaveState.EBX;
//#endif
//APTIOV_SERVER_OVERRIDE_END

    BuffAddr = HighBufferAddress;
    BuffAddr = Shl64(BuffAddr, 32);
    BuffAddr += LowBufferAddress;

    //TRACE((-1,"\nOFBD address is:%x ------\n",BuffAddr));
		
    OFBDHeader = (OFBD_HDR *)BuffAddr;
    if ((Data == OFBD_SW_SMI_VALUE) && (OFBDHeader->OFBD_SIG == 'DBFO'))
    {

//APTIOV_SERVER_OVERRIDE_START: Security Vulnerability
        if(OFBDHeader->OFBD_Size < (sizeof(OFBD_HDR) + sizeof(OFBD_EXT_HDR) + sizeof(OFBD_END))) return EFI_ACCESS_DENIED;
//APTIOV_SERVER_OVERRIDE_END: Security Vulnerability

        if(EFI_ERROR(AmiValidateMemoryBuffer ((UINT8*)(UINTN)OFBDHeader, \
                                                OFBDHeader->OFBD_Size))) return EFI_ACCESS_DENIED;

        OFBDHeader->OFBD_VER = OFBD_VERSION;
        OFBDHeader->OFBD_RS |= OFBD_RS_SUPPORT;

        for (i = 0; OFBDInitPartsTbl[i] != NULL; i++)
        {
            OFBDInitPartsTbl[i]((VOID *)BuffAddr, &OFBDDataHandled);
        }
    }
#else
//APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    AFRI_OFBD_BUFFER  *pData = NULL;
      
      if (CommBuffer != NULL) 
      {
          pData = (AFRI_OFBD_BUFFER *)CommBuffer;
          
          //TRACE((-1, "[OFBD] Command(%02X) Size(%08X)\n", pData->Command, *CommBufferSize);)
          
          if (pData->Command == OFBD_SW_SMI_VALUE)
          {
              OFBDHeader = (OFBD_HDR *)((VOID *)pData->Data);
              
              if (OFBDHeader->OFBD_SIG == 'DBFO') {
                  
                  OFBDHeader->OFBD_VER = OFBD_VERSION;
                  OFBDHeader->OFBD_RS |= OFBD_RS_SUPPORT;
                  
                  OfbdConvertBufferPointer((VOID *)pData->Data);
                  
                  for (i = 0; OFBDInitPartsTbl[i] != NULL; i++)
                  {
                      OFBDInitPartsTbl[i]((VOID *)pData->Data, &OFBDDataHandled);
                  }
              }
          }
      }
//APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
#endif    
    RETURN(Status);
}

#if OFBD_FOR_ARM_SUPPORT == 0
/**
    Ofbd (In Smm Function)

	This function registers OFBD SMI Handler.
	
    @param ImageHandle - EFI Image Handle.
    @param SystemTable - EFI System Table.

    @retval EFI_STATUS, based on the result.
*/ 

EFI_STATUS InSmmFunction(
    IN EFI_HANDLE ImageHandle, 
    IN EFI_SYSTEM_TABLE *SystemTable)
{
//APTIOV_SERVER_OVERRIDE_START
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_HANDLE  Handle = NULL;
//APTIOV_SERVER_OVERRIDE_END
    UINT16 i = 0;

//APTIOV_SERVER_OVERRIDE_START
//#if PI_SPECIFICATION_VERSION >= 0x1000A
    EFI_SMM_SW_DISPATCH2_PROTOCOL    *pSwDispatch = NULL;
    EFI_SMM_SW_REGISTER_CONTEXT      SwContext = {OFBD_SW_SMI_VALUE};
//#else
//    EFI_SMM_SW_DISPATCH_PROTOCOL    *pSwDispatch;
//    EFI_SMM_SW_DISPATCH_CONTEXT     SwContext = {OFBD_SW_SMI_VALUE};
//#endif
    
//#if PI_SPECIFICATION_VERSION >= 0x1000A
//APTIOV_SERVER_OVERRIDE_END
    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    
    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, &gSmmBase2);
    
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pSmmBase->GetSmstLocation (gSmmBase2, &pSmst);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;


    Status = pSmst->SmmLocateProtocol( \
                        &gEfiSmmSwDispatch2ProtocolGuid, NULL, &pSwDispatch);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;


    Status = pSmst->SmmLocateProtocol(&gEfiSmmCpuProtocolGuid, NULL, &gSmmCpu);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

//APTIOV_SERVER_OVERRIDE_START
//#else
//    VERIFY_EFI_ERROR(pBS->LocateProtocol(
//                          &gEfiSmmSwDispatchProtocolGuid, NULL, &pSwDispatch));
//#endif

//APTIOV_SERVER_OVERRIDE_END

    Status = pSwDispatch->Register(pSwDispatch, OFBDSMIHandler, &SwContext, \
                                                                     &Handle);
    ASSERT_EFI_ERROR(Status);

    for (i = 0; OFBDInSMMFuncTbl[i] != NULL; i++)
    {
    	OFBDInSMMFuncTbl[i]();
    }

    return Status;
}

/**
    Ofbd (Not In Smm Function)

	This function will services OFBD Not In Smm Functions.
	
    @param ImageHandle - EFI Image Handle.
    @param SystemTable - EFI System Table.

    @retval EFI_STATUS, based on the result.
*/ 

EFI_STATUS NotInSmmFunction(
    IN EFI_HANDLE ImageHandle, 
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    UINT8 i;
    for (i = 0; OFBDNotInSMMFunc[i] != NULL; i++) OFBDNotInSMMFunc[i]();
    return EFI_SUCCESS;
}
#endif
/**
    Ofbd (Ami OFBD Entry Point)

	The main entry point of OFBD module.
	
    @param ImageHandle - EFI Image Handle.
    @param SystemTable - EFI System Table.

    @retval EFI_STATUS, based on the result.
*/ 

EFI_STATUS EFIAPI AmiOFBDEntryPoint (
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable )
{
#if OFBD_FOR_ARM_SUPPORT == 0    
    // 1. Setup AMI library
    InitAmiLib(ImageHandle, SystemTable);
    
    // 2. Utilize EfiLib to init
    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
    
#else    
//APTIOV_SERVER_OVERRIDE_START
    EFI_STATUS	                 status = EFI_SUCCESS;
//APTIOV_SERVER_OVERRIDE_END	
    EFI_HANDLE                   hOfbd = NULL;
    EFI_GUID                     gEfiOfbdHandlerGuid = OFBD_HANDLER_GUID;
 	      
#if AFRI_PASS_TO_AFRI_PROTOCOL == 0
    EFI_SMM_BASE2_PROTOCOL       *pSmmBase2 = NULL;
    EFI_SMM_SYSTEM_TABLE2        *pSmst2 = NULL;
        
    // Create event for address traslation of Ofbd Library
    InitAmiRuntimeLib(ImageHandle, SystemTable, NULL, OfbdVirtualNotifyEvent);
        
	// Register Ofbd handler to SMM Communication Protocol
	status = SystemTable->BootServices->LocateProtocol(
											&gEfiSmmBase2ProtocolGuid,
											NULL,
											(VOID **)&pSmmBase2);
	ASSERT_EFI_ERROR(status);

	status = pSmmBase2->GetSmstLocation(pSmmBase2, &pSmst2);
	ASSERT_EFI_ERROR(status);

	status = pSmst2->SmiHandlerRegister(OFBDSMIHandler, &gEfiOfbdHandlerGuid, &hOfbd);
	ASSERT_EFI_ERROR(status);

#else

    AMI_FRI_PROTOCOL            *afriProtocol = NULL;
	
    // Create event for address traslation of Ofbd Library
    InitAmiRuntimeLib(ImageHandle, SystemTable, NULL, OfbdVirtualNotifyEvent);
        
    // Register Ofbd handler to AFRI Protocol    	
    status = SystemTable->BootServices->LocateProtocol(
                                            &gAmiFriProtocolGuid,
                                            NULL,
                                            (VOID**)&afriProtocol);
    ASSERT_EFI_ERROR(status);

    status = afriProtocol->RegisterAfriHandler( OFBDSMIHandler,
                                                &gEfiOfbdHandlerGuid,
                                                &hOfbd );
    ASSERT_EFI_ERROR(status);
#endif
	
    //TRACE((-1, "[OFBD Module] Initialized ...\n");)
	
    return status;
#endif	
}

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
