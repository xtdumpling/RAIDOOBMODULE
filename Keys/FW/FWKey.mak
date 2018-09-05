#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************

#*************************************************************************
#<AMI_FHDR_START>
#
# Name: FWKey.mak
#
# Description: Include FWkey ffs with Root Platform key into the BIOS FV_BB
# 
#<AMI_FHDR_END>
#*************************************************************************
Prepare: $(BUILD_DIR)$(PATH_SLASH)FWKey.inf
#-------------------------------------------------------------------------
#   Include Platform Firmware Root Key (FWKey) .FFS
#-------------------------------------------------------------------------
###########################################################################
###	Creating FWKey.inf file
###########################################################################
$(BUILD_DIR)$(PATH_SLASH)FWKey.inf:
	@$(ECHO) \
"## @file$(EOL)\
#   This file is auto-generated from FWKey.mak$(EOL)\
##$(EOL)\
[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = FWKey$(EOL)\
  FILE_GUID                      = $(FWKEY_FILE_GUID)$(EOL)\
  MODULE_TYPE                    = USER_DEFINED$(EOL)\
$(EOL)\
[Packages]$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  MdePkg/MdePkg.dec$(EOL)"\
> $@
ifneq ($(wildcard $(FWpub)), $(FWpub))
#-------------------------------------------------------------------------
#   Create a dummy key placeholder file that will be updated with the 
#   real pub key at the time of signing the BIOS image
#-------------------------------------------------------------------------
	@$(ECHO) "\
#pragma pack(1)$(EOL)\
char pad[$(FWKEY_FILE_SIZE)] = {'$$','B','A','D','K','E','Y','$$'};"\
> $(basename $@).cbin
	@$(ECHO) \
"[Sources]$(EOL)\
  FWKey.cbin"\
>> $@
else
#-------------------------------------------------------------------------
#   FWpub key is present
#   Prepare Platform Firmware Root Key (FWKey) bin file
#-------------------------------------------------------------------------
	@$(ECHO) \
"[Binaries]$(EOL)\
  BIN|FWKey.bin|*"\
>> $@
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	chmod +x $(CRYPTCON:-@=)
endif
ifeq ($(FWCAPSULE_CERT_FORMAT),1)
ifeq ($(FWKEY_FILE_FORMAT),0)
# n-modulus of RSA2048
$(error  *** PKCS_7: Unsupported FWKEY_FILE_FORMAT(0). Please select between (1) SHA256 Hash of X.509 Tbs cert section or (2) X.509 DER encoded Key Certificate)
endif
ifeq ($(FWKEY_FILE_FORMAT),1)
#get SHA256 Hash of x509 Tbs cert section. Supported from Cryptocon.exe v4.7.31.2014
	$(CRYPTCON) -h2 -x $(FWpub) -o $(basename $@).bin
endif
ifeq ($(FWKEY_FILE_FORMAT),2)
# x509 Cert Key
	$(CP) $(FWpub) $(basename $@).bin
endif
else #$(FWCAPSULE_CERT_FORMAT),0)
ifeq ($(FWKEY_FILE_FORMAT),2)
# x509 Cert Key
$(error  *** PKCS_1v1.5: Unsupported FWKEY_FILE_FORMAT(2). Please select between (0) n-modulus of RSA Key and (1) SHA256 Hash of n-modulus of RSA Key)
endif
#Extract 256byte n-modulus from either X.509 DER or PKCS#1v2 ASN.1 encoded RSA Key certificate file
# n-modulus of RSA2048
	$(CRYPTCON) -w -k $(FWpub) -o $(basename $@).bin
ifeq ($(FWKEY_FILE_FORMAT),1)
#get SHA256 Hash of n-modulus of RSA Key
	$(CRYPTCON) -h2 -f $(basename $@).bin -o $(basename $@).bin
endif
endif
endif

#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************
