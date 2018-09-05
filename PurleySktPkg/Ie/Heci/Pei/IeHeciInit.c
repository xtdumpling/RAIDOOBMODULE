/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
 **/
/**

Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  IeHeciInit.c

@brief:

 IE-HECI PEIM, implements IE-HECI PPI.

**/
#include <PiPei.h>
#include <Uefi.h>
    
#include <Ppi/IeHeci.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/PciCfg.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/PciCfg.h>

#include <Library/PeiServicesLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciBaseLib.h>

#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/IeHeciCoreLib.h>
#include <Library/TimerLib.h>

#include <IeHeciRegs.h>
#include <IeHeciMsgs.h>
#include <Ppi/IePlatformPolicyPei.h>
#include <Library/PchPmcLib.h>

/*****************************************************************************
 * Local definitions
 *****************************************************************************/
typedef struct _IE_HECI1_PPI_EXT_
{
  PEI_IE_HECI_PPI    Ppi;
  IE_HECI_DEVICE     Heci;
  HECI_MSG_HEADER    MsgBuf[HECI_MSG_MAXLEN / sizeof(HECI_MSG_HEADER)];
} IE_HECI1_PPI_EXT;


/*****************************************************************************
 * Local function prototypes.
 *****************************************************************************/
EFI_STATUS
IeHeciPpiInstall(
  IN CONST EFI_PEI_SERVICES    **ppPeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *pNotifyDesc,
  IN VOID                      *pPpi);

EFI_STATUS
IeHeciSendwAck(
  IN     PEI_IE_HECI_PPI              *This,
  IN OUT UINT32                       *Message,
  IN OUT UINT32                       *Length,
  IN     UINT8                        HostAddress,
  IN     UINT8                        IeAddress
  );

EFI_STATUS
IeHeciReadMsg(
  IN     PEI_IE_HECI_PPI              *This,
  IN     UINT32                       Blocking,
  IN     UINT32                       *MessageBody,
  IN OUT UINT32                       *Length
  );

EFI_STATUS
IeHeciSendMsg(
  IN     PEI_IE_HECI_PPI              *This,
  IN     UINT32                       *Message,
  IN     UINT32                       Length,
  IN     UINT8                        HostAddress,
  IN     UINT8                        IeAddress
  );

EFI_STATUS
IeHeciInitialize(
  IN     PEI_IE_HECI_PPI              *This
  );

EFI_STATUS
IeHeciMemoryInitDoneNotify(
  IN      PEI_IE_HECI_PPI             *This,
  IN      UINT32                      MrcStatus,
  IN OUT  UINT8                       *Action
  );

/*****************************************************************************
 * Variables
 *****************************************************************************/
static EFI_PEI_NOTIFY_DESCRIPTOR  mIePolicyNotifyList[] =
{
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiIePlatformPolicyPpiGuid,
    IeHeciPpiInstall
  }
};


/*****************************************************************************
 * Function definitions.
 *****************************************************************************/
/**
  The entry point of the HECI PEIM.

  It installs IE HECI-1 PPI interface if IE Policy is alread available, or
  installs notify callback for IE Policy to be called when policy is installed.
  IE Policy must be initialized before IE HECI-1 PPI is initialized.

  @param[in]  FileHandle  Handle of the file being invoked.
  @param[in]  PeiServices Describes the list of possible PEI Services.
  
  @retval EFI_SUCCESS     HECI-1 PPI initialization completed successfully
  @retval EFI_UNSUPPORTED HECI-1 is disabled or not present in this system
  @retval EFI_OUT_OF_RESOURCES Could not allocate resource for HECI-1 PPI
 **/
EFI_STATUS
IeHeciPeimEntryPoint(
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **ppPeiServices
  )
{
  EFI_STATUS Status;
  PEI_IE_PLATFORM_POLICY_PPI *pIePolicy;

  if (PchIsDwrFlow()) {
  
    DEBUG((DEBUG_WARN, "[IE] WARNING: DWR detected, HECI PPI not installed\n"));
    return EFI_UNSUPPORTED;
  }
  Status = (*ppPeiServices)->LocatePpi(ppPeiServices,
                                       &gPeiIePlatformPolicyPpiGuid,
                                       0, NULL, &pIePolicy);
  if (EFI_ERROR(Status)) {
  
    Status = (*ppPeiServices)->NotifyPpi(ppPeiServices, &mIePolicyNotifyList[0]);
    ASSERT_EFI_ERROR(Status);
    
  } else {
  
    Status = IeHeciPpiInstall(ppPeiServices, NULL, NULL);
  }
  return Status;
}


/**
  This function installs HECI-1 PPI interface for IE.

  When calling this function IE Policy must be initialized and HECI-1 enabled
  or disabled according to the policy. If HECI-1 is disabled this function
  does not install the PPI but returns EFI_UNSUPPORTED.

  @param[in] ppPeiServices Describes the list of possible PEI Services
  @param[in] pNotifyDesc   Notify descriptor
  @param[in] pPpi          The notified PPI
  
  @retval EFI_SUCCESS     HECI-1 PPI initialization completed successfully
  @retval EFI_UNSUPPORTED HECI-1 is disabled or not present in this system
  @retval EFI_OUT_OF_RESOURCES Could not allocate resource for HECI-1 PPI
 **/
EFI_STATUS
IeHeciPpiInstall(
  IN CONST EFI_PEI_SERVICES    **ppPeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *pNotifyDesc,
  IN VOID                      *pPpi)
{
  EFI_STATUS Status;
  UINT16     Vid;

  struct _HECI_PPI_
  {
    EFI_PEI_PPI_DESCRIPTOR    Desc;
    IE_HECI1_PPI_EXT          Heci1PpiExt;
  } *pHeciPpi;

  Vid = MmioRead16(MmPciBase(IE_BUS, IE_DEV, IE_FUN_HECI1));
  if (Vid == 0xFFFF) { // Vendor id must be not 0xFFFF for enabled HECI
  
    DEBUG((DEBUG_WARN, "[IE] WARNING: HECI-1 disabled, HECI PPI not installed\n"));
    return EFI_UNSUPPORTED;
  }
  //
  // Install HECI PPI
  //
  //
  pHeciPpi = AllocatePool(sizeof(*pHeciPpi));
  if (pHeciPpi != NULL) {
  
    pHeciPpi->Desc.Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    pHeciPpi->Desc.Guid = &gPeiIeHeciPpiGuid;
    pHeciPpi->Desc.Ppi = &pHeciPpi->Heci1PpiExt;
    pHeciPpi->Heci1PpiExt.Ppi.SendwAck = IeHeciSendwAck;
    pHeciPpi->Heci1PpiExt.Ppi.ReadMsg = IeHeciReadMsg;
    pHeciPpi->Heci1PpiExt.Ppi.SendMsg = IeHeciSendMsg;
    pHeciPpi->Heci1PpiExt.Ppi.InitializeHeci = IeHeciInitialize;
    pHeciPpi->Heci1PpiExt.Ppi.MemoryInitDoneNotify = IeHeciMemoryInitDoneNotify;
    pHeciPpi->Heci1PpiExt.Heci.Bus = IE_BUS;
    pHeciPpi->Heci1PpiExt.Heci.Dev = IE_DEV;
    pHeciPpi->Heci1PpiExt.Heci.Fun = IE_FUN_HECI1;
    pHeciPpi->Heci1PpiExt.Heci.Hidm = HECI_HIDM_MSI;
    pHeciPpi->Heci1PpiExt.Heci.Mbar = HECI1_IE_MBAR_DEFAULT;
    
    Status = (**ppPeiServices).InstallPpi(ppPeiServices, &pHeciPpi->Desc);
    
  } else {
    Status = EFI_OUT_OF_RESOURCES;
  }
  if (EFI_ERROR(Status)) {
    
    DEBUG((DEBUG_ERROR, "[IE] ERROR: Cannot install HECI PPI\n"));
    ASSERT_EFI_ERROR(Status);
    
  } else {
    DEBUG((DEBUG_INFO, "[IE] HeciPpi installed\n"));
  }
  return Status;
} // IeHeciPpiInstall()


/**
  Initialize HECI-1 for use in PEI phase.

  Determines if the HECI-1 device is present and, if present, initializes it for
  use by the BIOS.

  param[in] This            The address of HECI PPI

  retval EFI_SUCCESS        The function completed successfully.
  retval EFI_DEVICE_ERROR   No HECI device
 **/
EFI_STATUS
IeHeciInitialize (
  IN     PEI_IE_HECI_PPI                 *This
  )
{
  UINT32 Timeout = PEI_HECI_INIT_TIMEOUT / HECI_TIMEOUT_UNIT;
  
  if (This == NULL){
    return EFI_INVALID_PARAMETER;
  }
  return HeciInit(&((IE_HECI1_PPI_EXT*)This)->Heci, &Timeout);
} // HeciInitialize()


/**
  Read a message from HECI queue.

  @param[in] This              The address of HECI PPI.
  @param[in] Blocking          Used to determine if the read is BLOCKING or NON_BLOCKING
  @param[in] MessageBody       Pointer to a buffer used to receive a message.
  @param[in, out] Length       Pointer to the length of the buffer on input and the length
                               of the message on return. (in bytes)

  @retval EFI_SUCCESS          One message packet read
  @retval EFI_TIMEOUT          HECI is not ready for communication
  @retval EFI_DEVICE_ERROR     Zero-length message packet read
  @retval EFI_BUFFER_TOO_SMALL The caller's buffer was not large enough
 **/
EFI_STATUS
IeHeciReadMsg (
  IN     PEI_IE_HECI_PPI        *This,
  IN     UINT32                 Blocking,
  IN     UINT32                 *MessageBody,
  IN OUT UINT32                 *Length
  )
{
  EFI_STATUS       Status;
  UINT32           i, Timeout, MsgLen;
  UINT32           FrgLen;
  UINT8            *pFrgBdy;
  HECI_MSG_HEADER  *pFrgHdr;
  
  if (This == NULL || MessageBody == NULL || Length == NULL){
    return EFI_INVALID_PARAMETER;
  }
  Timeout = (Blocking) ? (PEI_HECI_READ_TIMEOUT / HECI_TIMEOUT_UNIT) : 0;
  MsgLen = 0;
  pFrgHdr = &((IE_HECI1_PPI_EXT*)This)->MsgBuf[0];
  pFrgBdy = (UINT8*)&((IE_HECI1_PPI_EXT*)This)->MsgBuf[1];
  do{
    FrgLen = sizeof(((IE_HECI1_PPI_EXT*)This)->MsgBuf);
    Status = HeciMsgRecv(&((IE_HECI1_PPI_EXT*)This)->Heci, &Timeout, pFrgHdr, &FrgLen);
    if (EFI_ERROR(Status)){
      break;
    } else {
      FrgLen -= sizeof(HECI_MSG_HEADER);
      if (MsgLen + FrgLen > *Length){
        FrgLen = *Length - MsgLen;
      }
      for (i = 0; i < FrgLen; i++){
        ((UINT8*)MessageBody)[MsgLen++] = pFrgBdy[i];
      }
    }
  }
  while (!pFrgHdr->Bits.MsgComplete);
  
  *Length = MsgLen;
  return Status;
}


/**
  Function sends one message (of any length) through the HECI circular buffer.

  @param[in] This                 The address of HECI PPI.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] IeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HEC
 **/
EFI_STATUS
IeHeciSendMsg (
  IN      PEI_IE_HECI_PPI          *This,
  IN      UINT32                   *Message,
  IN      UINT32                   Length,
  IN      UINT8                    HostAddress,
  IN      UINT8                    IeAddress)
{
  EFI_STATUS      Status;
  UINT32          i, Timeout;
  UINT32          Len = Length;
  UINT8           *pMsg = (UINT8*)Message;
  UINT8           *pFrgBdy;
  HECI_MSG_HEADER *pFrgHdr;
  
  if (This == NULL || Message == NULL){
    return EFI_INVALID_PARAMETER;
  }
  Timeout = PEI_HECI_SEND_TIMEOUT / HECI_TIMEOUT_UNIT;
  //
  // Fragment the message into queue-sized packets and loop until completely sent
  //
  pFrgHdr = &((IE_HECI1_PPI_EXT*)This)->MsgBuf[0];
  pFrgBdy = (UINT8*)&((IE_HECI1_PPI_EXT*)This)->MsgBuf[1];
  pFrgHdr->DWord = 0;
  pFrgHdr->Bits.IeAddress = IeAddress;
  pFrgHdr->Bits.HostAddress = HostAddress;
  ASSERT(((IE_HECI1_PPI_EXT*)This)->Heci.HMtu > 0); // make sure HECI is initialized when sending
  do{
    if (Len <= ((IE_HECI1_PPI_EXT*)This)->Heci.HMtu){
      pFrgHdr->Bits.Length = Len;
      pFrgHdr->Bits.MsgComplete = 1; // Set msg complete in last fragment

    } else {
      pFrgHdr->Bits.Length = ((IE_HECI1_PPI_EXT*)This)->Heci.HMtu;
    }
    for (i = 0; i < pFrgHdr->Bits.Length; i++){
      pFrgBdy[i] = pMsg[i];
    }
    pMsg += pFrgHdr->Bits.Length;
    Len -= pFrgHdr->Bits.Length;
    Status = HeciMsgSend(&((IE_HECI1_PPI_EXT*)This)->Heci, &Timeout, pFrgHdr);
    if (EFI_ERROR(Status)){
      goto GetOut;
    }
  }
  while (Len > 0);
  
GetOut:
  return Status;
}


/**
  Send HECI message and wait for response.

  @param[in,out]  This                 The address of HECI PPI.
  @param[in]      Message              Pointer to the message data to be sent.
  @param[in]      Length               Length of the message in bytes.
  @param[in]      HostAddress          The address of the host processor.
  @param[in]      IeAddress            Address of the ME subsystem the message is being sent to.


 **/
EFI_STATUS
IeHeciSendwAck(
  IN      PEI_IE_HECI_PPI              *This,
  IN OUT  UINT32                       *Message,
  IN OUT  UINT32                       *Length,
  IN      UINT8                        HostAddress,
  IN      UINT8                        IeAddress)
{
  EFI_STATUS  Status;
  
  Status = IeHeciSendMsg(This, Message, *Length, HostAddress, IeAddress);
  if (!EFI_ERROR(Status)){
    Status = IeHeciReadMsg(This, TRUE, Message, Length);
  }
  return Status;
} // HeciSendwAck()

/**
  Send HECI message to signal that memory is ready to use after MRC phase.

  @param[in]      This                 The address of HECI PPI.
  @param[in]      MrcStatus            Status returned by MRC.
  @param[in,out]  Action               Requested action at the bios side.

  @retval EFI_SUCCESS                  If message was sent.
 **/
EFI_STATUS IeHeciMemoryInitDoneNotify(
  IN      PEI_IE_HECI_PPI    *This,
  IN      UINT32              MrcStatus,
  IN OUT  UINT8              *Action)
{
  EFI_STATUS             Status;
  MEMORY_INIT_DONE_MSG   Msg;
  UINT32                 MsgLength;
  IEFS1                  Iefs1;

  // Read IE FW status
  Iefs1.DWord = IeHeciPciReadIefs1();

  // Check the IE Firmware status
  if (Iefs1.Bits.ErrorCode != 0 || Iefs1.Bits.CurrentState <= 1) {
    DEBUG((DEBUG_ERROR, "[IE] ERROR: IE not working (IEFS: %08X)\n", Iefs1.DWord));
    return EFI_DEVICE_ERROR;
  }else{
    DEBUG((DEBUG_INFO, "[IE] IEFS: %08X\n", Iefs1.DWord));
  }

  // Initialize IE-HECI
  Status = This->InitializeHeci(This);

  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "[IE] ERROR: HECI Init failed (%r)\n", Status));
    return Status;
  }else{
    DEBUG((DEBUG_INFO, "[IE] HECI Init done.\n"));
  }

  DEBUG((DEBUG_INFO, "[IE] Memory Init Done procedure started.\n"));

  Msg.Request.Header.Data               = 0;
  Msg.Request.Header.Fields.Command     = MEMORY_INIT_DONE_CMD;
  Msg.Request.Header.Fields.IsResponse  = 0;
  Msg.Request.Header.Fields.GroupId     = BUP_COMMON_GROUP_ID;
  Msg.Request.Header.Fields.Reserved    = 0;
  Msg.Request.Header.Fields.Result      = 0;
  Msg.Request.Data.Fields.Status        = MrcStatus;

  MsgLength = sizeof(Msg.Request);

  Status = This->SendwAck (
                      This,
                      (UINT32*) (&Msg),
                      &MsgLength,
                      BIOS_FIXED_HOST_ADDR,
                      HECI_CORE_MESSAGE_ADDR
                      );

  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "[IE] ERROR: Memory Init Done procedure fails (%r)\n",Status));
    *Action = 0;
  } else{
    DEBUG((DEBUG_INFO, "[IE] Memory Init Done procedure ends. Action = 0x%X\n", Msg.Response.Action));
    *Action = Msg.Response.Action;
  }

  return Status;
}

