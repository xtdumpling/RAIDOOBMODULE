//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/ZTEICTcmOrdinals.h 7     3/29/11 12:26p Fredericko $
//
// $Revision: 7 $
//
// $Date: 3/29/11 12:26p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  ZTEICTcmOrdinals.h
//
// Description:
//       Header file for ZTEIC specific ordinals
//
//<AMI_FHDR_END>
//*************************************************************************
#define TCM_ORD_ActivateIdentity                  ((UINT32) 0x0000807A)
#define TCM_ORD_AuthorizeMigrationKey             ((UINT32) 0x0000802B)
#define TCM_ORD_CertifyKey                        ((UINT32) 0x00008032)
#define TCM_ORD_CertifyKey2                       ((UINT32) 0x00008033)
#define TCM_ORD_CertifySelfTest                   ((UINT32) 0x00008052)
#define TCM_ORD_ChangeAuth                        ((UINT32) 0x0000800C)
#define TCM_ORD_ChangeAuthAsymFinish              ((UINT32) 0x0000800F)
#define TCM_ORD_ChangeAuthAsymStart               ((UINT32) 0x0000800E)
#define TCM_ORD_ChangeAuthOwner                   ((UINT32) 0x00008010)
#define TCM_ORD_CMK_ApproveMA                     ((UINT32) 0x0000801D)
#define TCM_ORD_CMK_ConvertMigration              ((UINT32) 0x00008024)
#define TCM_ORD_CMK_CreateBlob                    ((UINT32) 0x0000801B)
#define TCM_ORD_CMK_CreateKey                     ((UINT32) 0x00008013)
#define TCM_ORD_CMK_CreateTicket                  ((UINT32) 0x00008012)
#define TCM_ORD_CMK_SetRestrictions               ((UINT32) 0x0000801C)
#define TCM_ORD_ContinueSelfTest                  ((UINT32) 0x00008053)
#define TCM_ORD_ConvertMigrationBlob              ((UINT32) 0x0000802A)
#define TCM_ORD_CreateCounter                     ((UINT32) 0x000080DC)
#define TCM_ORD_CreateEndorsementKeyPair          ((UINT32) 0x00008078)
#define TCM_ORD_CreateMaintenanceArchive          ((UINT32) 0x0000802C)
#define TCM_ORD_CreateMigrationBlob               ((UINT32) 0x00008028)
#define TCM_ORD_CreateRevocableEK                 ((UINT32) 0x0000807F)
#define TCM_ORD_CreateWrapKey                     ((UINT32) 0x0000801F)
#define TCM_ORD_DAA_JOIN                          ((UINT32) 0x00008029)
#define TCM_ORD_DAA_SIGN                          ((UINT32) 0x00008031)
#define TCM_ORD_Delegate_CreateKeyDelegation      ((UINT32) 0x000080D4)
#define TCM_ORD_Delegate_CreateOwnerDelegation    ((UINT32) 0x000080D5)
#define TCM_ORD_Delegate_LoadOwnerDelegation      ((UINT32) 0x000080D8)
#define TCM_ORD_Delegate_Manage                   ((UINT32) 0x000080D2)
#define TCM_ORD_Delegate_ReadTable                ((UINT32) 0x000080DB)
#define TCM_ORD_Delegate_UpdateVerification       ((UINT32) 0x000080D1)
#define TCM_ORD_Delegate_VerifyDelegation         ((UINT32) 0x000080D6)
#define TCM_ORD_DirRead                           ((UINT32) 0x0000801A)
#define TCM_ORD_DirWriteAuth                      ((UINT32) 0x00008019)
#define TCM_ORD_DisableForceClear                 ((UINT32) 0x0000805E)
#define TCM_ORD_DisableOwnerClear                 ((UINT32) 0x0000805C)
#define TCM_ORD_DisablePubekRead                  ((UINT32) 0x0000807E)
#define TCM_ORD_DSAP                              ((UINT32) 0x00008011)
#define TCM_ORD_EstablishTransport                ((UINT32) 0x000080E6)
#define TCM_ORD_EvictKey                          ((UINT32) 0x00008022)
#define TCM_ORD_ExecuteTransport                  ((UINT32) 0x000080E7)
#define TCM_ORD_Extend                            ((UINT32) 0x00008014)
#define TCM_ORD_FieldUpgrade                      ((UINT32) 0x000080AA)
#define TCM_ORD_FlushSpecific                     ((UINT32) 0x000080BA)
#define TCM_ORD_ForceClear                        ((UINT32) 0x0000805D)
#define TCM_ORD_GetAuditDigest                    ((UINT32) 0x00008085)
#define TCM_ORD_GetAuditDigestSigned              ((UINT32) 0x00008086)
#define TCM_ORD_GetAuditEvent                     ((UINT32) 0x00008082)
#define TCM_ORD_GetAuditEventSigned               ((UINT32) 0x00008083)
#define TCM_ORD_GetCapability                     ((UINT32) 0x00008065)
#define TCM_ORD_GetCapabilityOwner                ((UINT32) 0x00008066)
#define TCM_ORD_GetCapabilitySigned               ((UINT32) 0x00008064)
#define TCM_ORD_GetOrdinalAuditStatus             ((UINT32) 0x0000808C)
#define TCM_ORD_GetPubKey                         ((UINT32) 0x00008021)
#define TCM_ORD_GetRandom                         ((UINT32) 0x00008046)
#define TCM_ORD_GetTestResult                     ((UINT32) 0x00008054)
#define TCM_ORD_GetTicks                          ((UINT32) 0x000080F1)
#define TCM_ORD_IncrementCounter                  ((UINT32) 0x000080DD)
#define TCM_ORD_Init                              ((UINT32) 0x00008097)
#define TCM_ORD_KeyControlOwner                   ((UINT32) 0x00008023)
#define TCM_ORD_KillMaintenanceFeature            ((UINT32) 0x0000802E)
#define TCM_ORD_LoadAuthContext                   ((UINT32) 0x000080B7)
#define TCM_ORD_LoadContext                       ((UINT32) 0x000080B9)
#define TCM_ORD_LoadKey                           ((UINT32) 0x00008020)
#define TCM_ORD_LoadKey2                          ((UINT32) 0x00008041)
#define TCM_ORD_LoadKeyContext                    ((UINT32) 0x000080B5)
#define TCM_ORD_LoadMaintenanceArchive            ((UINT32) 0x0000802D)
#define TCM_ORD_LoadManuMaintPub                  ((UINT32) 0x0000802F)
#define TCM_ORD_MakeIdentity                      ((UINT32) 0x00008079)
#define TCM_ORD_MigrateKey                        ((UINT32) 0x00008025)
#define TCM_ORD_NV_DefineSpace                    ((UINT32) 0x000080CC)
#define TCM_ORD_NV_ReadValue                      ((UINT32) 0x000080CF)
#define TCM_ORD_NV_ReadValueAuth                  ((UINT32) 0x000080D0)
#define TCM_ORD_NV_WriteValue                     ((UINT32) 0x000080CD)
#define TCM_ORD_NV_WriteValueAuth                 ((UINT32) 0x000080CE)
#define TCM_ORD_OIAP                              ((UINT32) 0x0000800A)
#define TCM_ORD_OSAP                              ((UINT32) 0x0000800B)
#define TCM_ORD_OwnerClear                        ((UINT32) 0x0000805B)
#define TCM_ORD_OwnerReadInternalPub              ((UINT32) 0x00008081)
#define TCM_ORD_OwnerReadPubek                    ((UINT32) 0x0000807D)
#define TCM_ORD_OwnerSetDisable                   ((UINT32) 0x0000806E)
#define TCM_ORD_PCR_Reset                         ((UINT32) 0x000080C8)
#define TCM_ORD_PcrRead                           ((UINT32) 0x00008015)
#define TCM_ORD_PhysicalDisable                   ((UINT32) 0x00008070)
#define TCM_ORD_PhysicalEnable                    ((UINT32) 0x0000806F)
#define TCM_ORD_PhysicalSetDeactivated            ((UINT32) 0x00008072)
#define TCM_ORD_Quote                             ((UINT32) 0x00008016)
#define TCM_ORD_Quote2                            ((UINT32) 0x0000803E)
#define TCM_ORD_ReadCounter                       ((UINT32) 0x000080DE)
#define TCM_ORD_ReadManuMaintPub                  ((UINT32) 0x00008030)
#define TCM_ORD_ReadPubek                         ((UINT32) 0x0000807C)
#define TCM_ORD_ReleaseCounter                    ((UINT32) 0x000080DF)
#define TCM_ORD_ReleaseCounterOwner               ((UINT32) 0x000080E0)
#define TCM_ORD_ReleaseTransportSigned            ((UINT32) 0x000080E8)
#define TCM_ORD_Reset                             ((UINT32) 0x0000805A)
#define TCM_ORD_ResetLockValue                    ((UINT32) 0x00008040)
#define TCM_ORD_RevokeTrust                       ((UINT32) 0x00008080)
#define TCM_ORD_SaveAuthContext                   ((UINT32) 0x000080B6)
#define TCM_ORD_SaveContext                       ((UINT32) 0x000080B8)
#define TCM_ORD_SaveKeyContext                    ((UINT32) 0x000080B4)
#define TCM_ORD_SaveState                         ((UINT32) 0x00008098)
#define TCM_ORD_Seal                              ((UINT32) 0x00008017)
#define TCM_ORD_Sealx                             ((UINT32) 0x0000803D)
#define TCM_ORD_SelfTestFull                      ((UINT32) 0x00008050)
#define TCM_ORD_SetCapability                     ((UINT32) 0x0000803F)
#define TCM_ORD_SetOperatorAuth                   ((UINT32) 0x00008074)
#define TCM_ORD_SetOrdinalAuditStatus             ((UINT32) 0x0000808D)
#define TCM_ORD_SetOwnerInstall                   ((UINT32) 0x00008071)
#define TCM_ORD_SetOwnerPointer                   ((UINT32) 0x00008075)
#define TCM_ORD_SetRedirection                    ((UINT32) 0x0000809A)
#define TCM_ORD_SetTempDeactivated                ((UINT32) 0x00008073)
#define TCM_ORD_SHA1CompleteExtend                ((UINT32) 0x000080ED)
#define TCM_ORD_SHA1Start                         ((UINT32) 0x000080EA)
#define TCM_ORD_SHA1Update                        ((UINT32) 0x000080EB)
#define TCM_ORD_Sign                              ((UINT32) 0x0000803C)
#define TCM_ORD_Startup                           ((UINT32) 0x00008099)
#define TCM_ORD_StirRandom                        ((UINT32) 0x00008047)
#define TCM_ORD_TakeOwnership                     ((UINT32) 0x0000800D)
#define TCM_ORD_Terminate_Handle                  ((UINT32) 0x00008096)
#define TCM_ORD_TickStampBlob                     ((UINT32) 0x000080F2)
#define TCM_ORD_UnBind                            ((UINT32) 0x0000801E)
#define TCM_ORD_Unseal                            ((UINT32) 0x00008018)
#define TCM_TSC_ORD_PhysicalPresence                  ((UINT32) 0x4000800A)
#define TCM_TSC_ORD_ResetEstablishmentBit             ((UINT32) 0x4000800B)
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************