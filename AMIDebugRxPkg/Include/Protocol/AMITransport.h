//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Header: /AptioV/BIN/AMIDebugRxPkg/Include/Protocol/AMITransport.h 1     11/02/12 10:15a Sudhirv $
//
// $Revision: 1 $
//
// $Date: 11/02/12 10:15a $
//**********************************************************************

#ifndef _AMI_TRANSPORT_H_
#define _AMI_TRANSPORT_H_

//
// AMITransport protocol {35C64E50-405B-4485-B93C-8D4181E78A00}
//
#define AMI_TRANSPORT_PROTOCOL_GUID \
  { 0x35C64E50, 0x405B, 0x4485, 0xB9, 0x3C, 0x8D, 0x41, 0x81, 0xE7, 0x8A, 0x00 }

//extern EFI_GUID  gAMITransportProtocolGuid;

//
// AMI_TRANSPORT_PROTOCOL forward reference
//
typedef
struct _AMI_TRANSPORT_PROTOCOL AMI_TRANSPORT_PROTOCOL;

//
// AMITransport member functions
//
typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_INITIALIZE) (
  IN AMI_TRANSPORT_PROTOCOL		*This
  );

typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_RESTORE) (
  IN AMI_TRANSPORT_PROTOCOL		*This
  );

typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_SEND) (
  IN AMI_TRANSPORT_PROTOCOL		*This,
  IN VOID				*Buffer,
  IN UINT16				BufSize
  );

typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_SEND_NOACK) (
  IN AMI_TRANSPORT_PROTOCOL		*This,
  IN VOID				*Buffer,
  IN UINT16				BufSize
  );

typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_RECEIVE) (
  IN AMI_TRANSPORT_PROTOCOL		*This,
  IN OUT VOID				*Buffer,
  OUT UINT16				*BufSize,
  IN UINT16				Timeout
  );

typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_SYNC_PACKET_NUMBER) (
  IN AMI_TRANSPORT_PROTOCOL		*This
  );

typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_REGISTER_HANDLER) (
  IN AMI_TRANSPORT_PROTOCOL		*This,
  IN VOID				*Handler
  );

typedef
VOID
(EFIAPI *AMI_TRANSPORT_HANDLER) (
  VOID
  );

typedef
VOID
(EFIAPI *AMI_TRANSPORT_CALLBACK) (
  IN VOID	*data
  );

typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_REINITIALIZE) (
  IN AMI_TRANSPORT_PROTOCOL		*This
  );

typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_CONSOLEIN) (
  IN AMI_TRANSPORT_PROTOCOL		*This,
  IN OUT VOID				*ConBuffer,
  OUT UINTN				*ConBufSize
  );

typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_SENDEXPTN) (
  IN AMI_TRANSPORT_PROTOCOL		*This,
  IN UINT8				ExptnType,
  IN VOID				*Buffer,
  IN UINT16				BufSize
  );

typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_SMMREINIT) (
  IN AMI_TRANSPORT_PROTOCOL		*This,
  IN OUT UINT8 *SmmEntry,
  IN OUT UINT8 *SmmExit
  );

typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_SET_TARGET_STATE) (
  IN AMI_TRANSPORT_PROTOCOL		*This,
  IN BOOLEAN    TargetRunning
  );

typedef
EFI_STATUS
(EFIAPI *AMI_TRANSPORT_SMM_INFO) (
  IN AMI_TRANSPORT_PROTOCOL		*This,
  IN VOID				*Buffer,
  IN UINT16				BufSize
  );
//
// AMITransport protocol definition
//
typedef
struct _AMI_TRANSPORT_PROTOCOL {
  AMI_TRANSPORT_HANDLER			Handler;
  AMI_TRANSPORT_INITIALIZE		Initialize;
  AMI_TRANSPORT_RESTORE			Restore;
  AMI_TRANSPORT_SEND			Send;
  AMI_TRANSPORT_RECEIVE			Receive;
  AMI_TRANSPORT_SYNC_PACKET_NUMBER	SyncPacketNumber;
  AMI_TRANSPORT_REGISTER_HANDLER	RegisterHandler;
  AMI_TRANSPORT_CALLBACK		Callback;
  AMI_TRANSPORT_SEND_NOACK		SendNoAck;
  AMI_TRANSPORT_CONSOLEIN		ConsoleIn;
  AMI_TRANSPORT_SENDEXPTN		SendExptn;
  //
  //New interface for Alaska projects,use primarily for porting purpose,
  //to be invoked by the independent debug support modules
  //
  AMI_TRANSPORT_REINITIALIZE	ReInitialize;
  AMI_TRANSPORT_SMMREINIT	    SmmReInitialize;
  AMI_TRANSPORT_SET_TARGET_STATE UpdateTargetState;
  AMI_TRANSPORT_SMM_INFO	GetSmmInfo;

} AMI_TRANSPORT_PROTOCOL;

EFI_STATUS GetUSBControllerFrom_HOB(VOID);

//
// AMI_TRANSPORT variable definitions...
//

#endif /* _AMI_TRANSPORT_H_ */

//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
