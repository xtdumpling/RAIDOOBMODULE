//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Libraries/Tpm20CrbLib/Tpm20CRBLib.c 1     10/08/13 11:59a Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 11:59a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Libraries/Tpm20CrbLib/Tpm20CRBLib.c $
//
// 1     10/08/13 11:59a Fredericko
// Initial Check-In for Tpm-Next module
//
// 2     10/03/13 1:48p Fredericko
//
// 1     7/10/13 5:51p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Efi.h>
#include "Tpm20CRBLib.h"
#include <Library/BaseLib.h>
#include<Library/IoLib.h>
#include <Token.h>
#include <AmiTcg/Tpm20.h>
#include<Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>


#define CRB_TIMEOUT_A   (750 * 1000)       //microseconds
#define CRB_TIMEOUT_B   (2 * 1000 * 1000)  
#define CRB_TIMEOUT_C   (30 * 1000)
#define CRB_TIMEOUT_D   (200 * 1000)

//Porting, Use correct GUID
#if defined(PTT_SX_HANG_WRKARND) && (PTT_SX_HANG_WRKARND == 1)
EFI_GUID gMeDataHobGuid = gTcgMeDataHobGuid;
VOID *
EFIAPI
GetFirstGuidHob (
    IN CONST EFI_GUID         *Guid
);
#endif
//Porting end


//Forward declare functions
#if FTpmPlatformProfile == 1

EFI_STATUS
iTpmExecuteCommand (
    IN       VOID                 *CommandBuffer,
    IN       UINT32               CommandSize,
    IN OUT   VOID                 *ResponseBuffer,
    IN OUT   UINT32               *ResponseSize
);

EFI_STATUS
iTpmGetInfo (
    IN OUT   UINTN                *iTPMStatus
);


#else

EFI_STATUS
EFIAPI
PttHciReceive(
    OUT     UINT8     *FtpmBuffer,
    OUT     UINT32    *RespSize
);

EFI_STATUS
EFIAPI
PttHciSend(
    IN     UINT8      *FtpmBuffer,
    IN     UINT32     DataLength
);

BOOLEAN
EFIAPI
PttHciPresenceCheck();
#endif

#pragma optimize("",off)

UINT8 *
EFIAPI
CrbMmioReadBuffer8 (
    IN  UINTN       StartAddress,
    IN  UINTN       Length,
    OUT UINT8      *Buffer
)
{
    UINT8   *ReturnBuffer;

    ASSERT ((Length - 1) <=  (MAX_ADDRESS - StartAddress));
    ASSERT ((Length - 1) <=  (MAX_ADDRESS - (UINTN) Buffer));

    ReturnBuffer = Buffer;

    while (Length-- != 0)
    {
        *(Buffer++) = MmioRead8 (StartAddress++);
    }

    return ReturnBuffer;
}


UINT32 *
EFIAPI
CrbMmioWriteBuffer8 (
    IN  UINTN        StartAddress,
    IN  UINTN        Length,
    IN  CONST UINT8  *Buffer
)
{
    VOID* ReturnBuffer;

    ASSERT ((Length - 1) <=  (MAX_ADDRESS - StartAddress));
    ASSERT ((Length - 1) <=  (MAX_ADDRESS - (UINTN) Buffer));

    ReturnBuffer = (UINT8 *) Buffer;

    while (Length-- != 0)
    {
        MmioWrite8 (StartAddress++, *(Buffer++));
    }

    return ReturnBuffer;
}

BOOLEAN IsPTP()
{
    TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr = (TPM_CRB_ACCESS_REG_PTR)(( UINTN ) (PORT_TPM_IOMEMBASE));
    UINT32 PTPSupport = dCrbAccessRegPtr->TpmCrbIntfId[0];
    
    if( (PTPSupport == 0xFFFFFFFF) || (PTPSupport == 0x0)) return 0;
    else return 1;
}

UINT8 GetCurrentInterfaceType()
{
    TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr = (TPM_CRB_ACCESS_REG_PTR)(( UINTN ) (PORT_TPM_IOMEMBASE));
    //0 = Tis Interface
    //1 = CRB Interface

    UINT8  CurrInterface = (UINT8) (dCrbAccessRegPtr->TpmCrbIntfId[0] & 0x0F);
    UINT8 Data = 0;
#if FTpmPlatformProfile == 1
    UINTN  Info;
#endif

    if(!IsPTP())
    {
#if FTpmPlatformProfile == 1
        if(!EFI_ERROR(iTpmGetInfo(&Info)))
        {
            return Data | 1;
        }
#else
        if(PttHciPresenceCheck())
        {
            return Data | 1;
        }
#endif
        return 0;
    }
    else return (CurrInterface & 0x0F);
}

UINT8 CrbSupported()
{
    UINT16 CapCRB = *((UINT16 *)(PORT_TPM_IOMEMBASE + 0x30));
    UINT8 Data = 0;
#if FTpmPlatformProfile == 1
    UINTN  Info;
#endif

    if(!IsPTP())
    {
#if FTpmPlatformProfile == 1
        if(!EFI_ERROR(iTpmGetInfo(&Info)))
        {
            return Data | 1;
        }
#else
        if(PttHciPresenceCheck())
        {
            return Data | 1;
        }
#endif
        return 0;
    }
    CapCRB = CapCRB >> 14;

    if(CapCRB & 0x01)
    {
        return Data | 0x2;
    }
    return 0;
}



//Make sure the TPM is present before calling this library
//function. Results are not accurate otherwise
UINT8 TisSupported()
{
    UINT16 Cap = *((UINT16 *)(PORT_TPM_IOMEMBASE + 0x30));

    if(IsPTP())
    {

        if(!GetCurrentInterfaceType())return 1;

        Cap = Cap >> 13;

        if(Cap & 0x01)
        {
            return 1;
        }
        return 0;
    }
    return 1;
}


BOOLEAN FIFOSupported()
{
    UINT16 CapCRB = *((UINT16 *)(PORT_TPM_IOMEMBASE + 0x30));
    if(!IsPTP())return 1;
    CapCRB = CapCRB >> 13;
    return (CapCRB & 0x01);
}

BOOLEAN isTpm20CrbPresent()
{
#if FTpmPlatformProfile == 1
    UINTN  Info;
    if(!EFI_ERROR(iTpmGetInfo(&Info)))return TRUE;
#else
    if(PttHciPresenceCheck())return TRUE;
#endif
    if(IsPTP() && (GetCurrentInterfaceType()==1))
    {
        return TRUE;
    }
    return FALSE;
}



BOOLEAN dTPMCrbLocality0Granted(TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr)
{
    UINTN       ActiveLocality = 0;
    UINT32     u32TpmlocState = 0;

    u32TpmlocState = dCrbAccessRegPtr->TpmlocState;
    ActiveLocality = u32TpmlocState >> 2;

    if( ((ActiveLocality & 0x7)==0)             &&      // Locality is 0.
            (dCrbAccessRegPtr->TpmlocState & 0x02)  &&      // Locality is assign.
            (dCrbAccessRegPtr->TpmlocState & 0x80))     // TPM is Valid.
    {
        return TRUE;
    }

    return FALSE;
}



EFI_STATUS dTPMCrbSetLocality(TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr)
{
    UINTN       DelayTime = CRB_DELAY_TIMEOUT;
    UINTN       Timeout   = CRB_TIMEOUT_A / CRB_COUNT_TIMEOUT, Count =0;
#if FTpmPlatformProfile == 0
#if defined(PTT_SX_HANG_WRKARND) && (PTT_SX_HANG_WRKARND == 1)
    ME_DATA_HOB   *MeDataHob;
    UINT32        LocalityState = 0; 

    //check if ME has populated the locality structure
    //if not then we have to request the locality ourselves
    MeDataHob       = NULL;
    MeDataHob       = GetFirstGuidHob (&gMeDataHobGuid);

    if (MeDataHob != NULL)
    {
        LocalityState = MeDataHob->LocalityState;                        
    }

    if((((LocalityState & V_CRB_LOCALITY_STATE_ACTIVE_LOC_MASK) >> 2) == 0) &&
            ((LocalityState & B_CRB_LOCALITY_STATE_LOCALITY_ASSIGNED) != 0))
    {
        DEBUG ((DEBUG_INFO, "PTT Locality already assigned\n"));
        return EFI_SUCCESS;
    }
#endif
#endif


    while( FALSE == dTPMCrbLocality0Granted(dCrbAccessRegPtr) )
    {
        dCrbAccessRegPtr->TpmlocCtrl = 0x1;
        MicroSecondDelay(DelayTime); //delay 50us
        ++Count;
        if(Count == Timeout)return EFI_DEVICE_ERROR;
    }
    
#if FTpmPlatformProfile == 0
#if defined(PTT_SX_HANG_WRKARND) && (PTT_SX_HANG_WRKARND == 1)
    if( NULL != MeDataHob )
    {
		// We need to sync the ME Hob, when the Locality have already been taken.
        // For Intel PTT, when the locality have been assign, we should not release the locality.
        MeDataHob->LocalityState = dCrbAccessRegPtr->TpmlocState;
    }
#endif
#endif

    return EFI_SUCCESS;
}


EFI_STATUS dTPMCrbWaitStrtClr(TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr)
{   
    UINTN       DelayTime = CRB_DELAY_TIMEOUT;
    UINTN       Timeout   = CRB_TIMEOUT_B / CRB_COUNT_TIMEOUT, Count =0;

    while(dCrbAccessRegPtr->TpmCrbCtrlStrt != 0x0)
    {
        MicroSecondDelay(DelayTime); //delay 50us
        if(Count == Timeout)break;
        Count +=1;
    }

    if(Count == Timeout)return EFI_DEVICE_ERROR;
    return EFI_SUCCESS;
}

UINT8 isTPMIdle(TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr)
{
    if((dCrbAccessRegPtr->TpmCrbCtrlSts & 1) == 1)
    {
        return 0xFF; //FATAL TPM ERROR
    }
    if((dCrbAccessRegPtr->TpmCrbCtrlSts & 2) == 2)
    {
        return 1; //idle state
    }
    return 0;
}


EFI_STATUS dTPMCrbSetReqReadyState(TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr)
{
    UINTN       DelayTime = CRB_DELAY_TIMEOUT;
    UINTN       Timeout   = CRB_TIMEOUT_A / CRB_COUNT_TIMEOUT, Count =0;  //We should be in Timeout_C but 
                                                                          //some TPM's need more time.

    if( (isTPMIdle(dCrbAccessRegPtr)) == 0  )
    {
        return EFI_SUCCESS;
    }

    do
    {
        dCrbAccessRegPtr->TpmCrbCtrlReq = 0x01;
        MicroSecondDelay(DelayTime); //delay 50us
        if(Count == Timeout)break;
        Count +=1;
    }
    while( (isTPMIdle(dCrbAccessRegPtr)) == 1 );    //wait till ready

    if(Count == Timeout)return EFI_DEVICE_ERROR;
    return EFI_SUCCESS;
}


EFI_STATUS dTPMCrbSetReqIdleState(TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr)
{
    UINTN       DelayTime = CRB_DELAY_TIMEOUT;
    UINTN       Timeout   = CRB_TIMEOUT_A / CRB_COUNT_TIMEOUT, Count =0; //We should be in Timeout_C but 
                                                                         //some TPM's need more time.
    
#if FTpmPlatformProfile == 0
#if defined(PTT_SX_HANG_WRKARND) && (PTT_SX_HANG_WRKARND == 1)
    ME_DATA_HOB   *MeDataHob;
    UINT32        LocalityState = 0;

    //check if ME has populated the locality structure
    //if not then we have to request the locality ourselves
    MeDataHob       = NULL;
    MeDataHob       = GetFirstGuidHob (&gMeDataHobGuid);

    if (MeDataHob != NULL) {
      LocalityState = MeDataHob->LocalityState;
    }

    // Here, for Intel Skylake PTT spec, when PTT have been take locality, and ready status
    // we will not need to set TPM into idle mode.

    if((((LocalityState & V_CRB_LOCALITY_STATE_ACTIVE_LOC_MASK) >> 2) == 0) &&
        ((LocalityState & B_CRB_LOCALITY_STATE_LOCALITY_ASSIGNED) != 0)){
      DEBUG ((DEBUG_INFO, "PTT Locality already assigned\n"));
      return EFI_SUCCESS;
    }
#endif
#endif

    // TPM have already in Idle mode
    if( 1 == isTPMIdle(dCrbAccessRegPtr) )
        return EFI_SUCCESS;

    do
    {
        dCrbAccessRegPtr->TpmCrbCtrlReq = 0x2;
        MicroSecondDelay(DelayTime); //delay 100us
        if(Count == Timeout)break;
        Count +=1;
    }
    while( (isTPMIdle(dCrbAccessRegPtr)) == 0 );   //wait till idle

    if(Count == Timeout)return EFI_DEVICE_ERROR;
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
CrbSend(
    IN     UINT8      *InputBuffer,
    IN     UINT32     DataLength
)
{
    TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr = (TPM_CRB_ACCESS_REG_PTR)(( UINTN ) (PORT_TPM_IOMEMBASE));
    UINTN   CmdBase;
    EFI_STATUS Status;


    if(CrbSupported()==0x02)
    {

        Status = dTPMCrbSetLocality(dCrbAccessRegPtr);
        if(EFI_ERROR(Status))return Status;

        Status = dTPMCrbSetReqReadyState(dCrbAccessRegPtr);
        if(EFI_ERROR(Status))
        {
            return Status;
        }

        if( dCrbAccessRegPtr->TpmCrbCtrlCmdLAddr == 0 )
        {
            dCrbAccessRegPtr->TpmCrbCtrlCmdLAddr = 0xFED40080;
            dCrbAccessRegPtr->TpmCrbCtrlCmdSize = 0x500;
            dCrbAccessRegPtr->TpmCrbCtrlResdLAddr = 0xFED40080;
            dCrbAccessRegPtr->TpmCrbCtrlRespSize = 0x500;
        }

        if(dCrbAccessRegPtr->TpmCrbCtrlCmdLAddr !=0)
        {

            CmdBase = (UINTN)dCrbAccessRegPtr->TpmCrbCtrlCmdLAddr;

            CrbMmioWriteBuffer8(CmdBase, DataLength, InputBuffer);

            Status = dTPMCrbWaitStrtClr(dCrbAccessRegPtr);
            if(EFI_ERROR(Status))return Status;

            dCrbAccessRegPtr->TpmCrbCtrlStrt = 1;

            return EFI_SUCCESS;
        }
    }
    else
    {
#if FTpmPlatformProfile == 0
        return (PttHciSend(InputBuffer, DataLength));
#else
        return EFI_UNSUPPORTED;
#endif
    }
    return EFI_ABORTED;
}


EFI_STATUS
EFIAPI
CrbReceive(
    OUT     UINT8     *OutBuffer,
    OUT     UINT32    *RespSize
)
{
    TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr = (TPM_CRB_ACCESS_REG_PTR)(( UINTN ) (PORT_TPM_IOMEMBASE));
    UINTN RspBase;
    UINT16 Data16;
    UINT32 Data32;
    EFI_STATUS Status;

    if(CrbSupported()==0x02)
    {

        Status = dTPMCrbWaitStrtClr(dCrbAccessRegPtr);
        if(EFI_ERROR(Status))return Status;

        if(dCrbAccessRegPtr->TpmCrbCtrlResdLAddr !=0)
        {
            RspBase = (UINTN) dCrbAccessRegPtr->TpmCrbCtrlResdLAddr;

            CrbMmioReadBuffer8(RspBase, RESPONSE_HEADER_SIZE, OutBuffer);

            CopyMem (&Data16, OutBuffer, sizeof (UINT16));

            if(SwapBytes16(Data16) == TPM_ST_RSP_COMMAND)
            {
                return EFI_DEVICE_ERROR;
            }

            CopyMem(&Data32, (OutBuffer + 2), sizeof(UINT32));

            *RespSize = SwapBytes32(Data32);

            if(*RespSize > dCrbAccessRegPtr->TpmCrbCtrlRespSize)
            {
                return EFI_BUFFER_TOO_SMALL;
            }

            if(*RespSize <  sizeof(GENERIC_RESP_HDR))
            {
                return EFI_DEVICE_ERROR;
            }

            CrbMmioReadBuffer8(RspBase, *RespSize,  OutBuffer);

#if defined (SET_TPM_IDLE) && (SET_TPM_IDLE == 1)
            Status = dTPMCrbSetReqIdleState(dCrbAccessRegPtr);
            if(EFI_ERROR(Status))return Status;
#endif

            return EFI_SUCCESS;
        }
    }
    else
    {
#if FTpmPlatformProfile == 0
        return (PttHciReceive(OutBuffer, RespSize));
#else
        return EFI_UNSUPPORTED;
#endif
    }
    return EFI_ABORTED;
}



EFI_STATUS
EFIAPI
CrbSubmitCmd(
    IN      UINT8     *InputBuffer,
    IN      UINT32     InputBufferSize,
    OUT     UINT8     *OutputBuffer,
    OUT     UINT32    *OutputBufferSize
)
{
    EFI_STATUS Status;
    UINT8 FtpmDeviceType = (UINT8)FTpmPlatformProfile;
    TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr = (TPM_CRB_ACCESS_REG_PTR)(( UINTN ) (PORT_TPM_IOMEMBASE));

    if(InputBuffer == NULL || OutputBuffer == NULL || InputBufferSize == 0)
    {
        return EFI_INVALID_PARAMETER;
    }

    if(CrbSupported())
    {
        if(FtpmDeviceType == 0 || CrbSupported() == 2)
        {
            Status = CrbSend(InputBuffer, InputBufferSize);
            if (EFI_ERROR (Status))
            {
                return Status;
            }

            ///
            /// Receive the response data from TPM
            ///
            Status = CrbReceive(OutputBuffer, OutputBufferSize);
            if (EFI_ERROR (Status))
            {
                return Status;
            }
            return Status;
        }

        if(FtpmDeviceType == 1 && CrbSupported() == 1)
        {
#if FTpmPlatformProfile == 1
            Status = iTpmExecuteCommand(InputBuffer,InputBufferSize,OutputBuffer, OutputBufferSize );
            return Status;
#endif
            return EFI_UNSUPPORTED;
        }
    }

    return EFI_UNSUPPORTED;
}

