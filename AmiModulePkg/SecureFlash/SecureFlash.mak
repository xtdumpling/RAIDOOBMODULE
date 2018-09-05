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
# Name: SecureMod.mak
#
# Description: Includes main build module for Secure sub-components
# 
#   1. Create a help file with instructions how to sign Fw Capsule using CryptoCon.exe
#   2. Create extended Rom layout map file RomLayoutEx.bin 
#   3. Update FwCapsuleHdr ffs file inside BIOS.ROM with RomLayoutEx map 
#   4. Create signed BIOS image (Aptio FW Capsule)
#
#<AMI_FHDR_END>
#*************************************************************************

.PHONY : CLEAR_FWCAPSULE_FILES MAKE_FWCAPSULE_HELP_FILES MOD_FWCAPSULE_HDR_FFS $(FWCAPSULE_FILE)

#---------------------------------------------------------------------------
# Only for PKCS1v2.1 Key files the FWpub(public key part) can be derived 
# from FWpriv(full RSA key) if FWpub file is not available
#---------------------------------------------------------------------------
ifeq ($(FWCAPSULE_CERT_FORMAT),0)
ifneq ($(wildcard $(FWpub)), $(FWpub))
ifeq ($(wildcard $(FWpriv)),$(FWpriv))
FWpub := $(FWpriv)
endif
endif
endif

FWCAPSULE_MAKE_README := make_sign_capsule.txt
#---------------------------------------------------------------------------
#        Prepare Signed Capsule : FWCAPSULE_FILE
#---------------------------------------------------------------------------
clean  : CLEAR_FWCAPSULE_FILES
Prepare: CLEAR_FWCAPSULE_FILES MAKE_FWCAPSULE_HELP_FILES

CLEAR_FWCAPSULE_FILES:
ifneq ("$(wildcard  $(FWCAPSULE_FILE))", "")
	-$(RM) $(FWCAPSULE_FILE)
endif
ifneq ("$(wildcard  $(FWCAPSULE_MAKE_README))", "") 
	-$(RM) $(FWCAPSULE_MAKE_README)
endif

#---------------------------------------------------------------------------
#   1. Create readme file with instructions to build signed Aptio FW Capsule using CryptoCon
#---------------------------------------------------------------------------
MAKE_FWCAPSULE_HELP_FILES:
	@$(ECHO) "=============================================================$(EOL)\
===  SIMPLE GUIDE TO SIGNING OF APTIO FW USING CRYPTOCON  ===$(EOL)\
=============================================================$(EOL)\
1. Use BIOS image '$(UNSIGNED_BIOS_ROM)'$(EOL)\
   created on $(TODAY) at $(NOW), $(EOL)\
   with the folowing files present in the binary:$(EOL)\
    a)FWKey.ffs with a public key to verify signed BIOS updates$(EOL)\
    b)FwCapsuleHdr.ffs with the current Rom Layout map"\
> $(FWCAPSULE_MAKE_README)
ifeq ($(FWCAPSULE_CERT_FORMAT),1)
	@$(ECHO) "2. Provide RSA key in PKCS#12 PFX(private) and X.509 DER(public) formats."\
>> $(FWCAPSULE_MAKE_README)
else
	@$(ECHO) "2. Provide RSA2048 Key in PKCS#1v2.1 DER or PEM format.$(EOL)\
    '$(notdir $(FWrootKey))' - Root Certificate key (private RSA key)"\
>> $(FWCAPSULE_MAKE_README)
endif
	@$(ECHO) "   '$(notdir $(FWpriv))' - Sign Certificate key (private RSA key)$(EOL)\
   '$(notdir $(FWpub))' - Signing Certificate key (public key part)$(EOL)\
3. (Optional)$(EOL)\
    Re-build the Rom map if the original binary '$(notdir $(UNSIGNED_BIOS_ROM))' was modified$(EOL)\
    FWBUILD $(notdir $(UNSIGNED_BIOS_ROM)) -s -m $(notdir $(ROM_LAYOUT_EX))$(EOL)\
$(EOL)\
4. Run Cryptocon script to generate Signed FwCapsule '$(notdir $(FWCAPSULE_FILE))' $(EOL)\
$(EOL)\
CRYPTOCON -r $(notdir $(ROM_LAYOUT_EX)) $(notdir $(CRYPTOCON_CMDLINE_SIG)) -f $(notdir $(UNSIGNED_BIOS_ROM)) -o $(notdir $(FWCAPSULE_FILE))$(EOL)\
$(EOL)\
=============================================================$(EOL)\
=== Common Cryptocon FwCapsule build instructions$(EOL)\
=============================================================$(EOL)\
-c'FWrootPriv' -k'FWsignPriv' Make PKCS#1v1.5 signed FwCapsule(Note1)$(EOL)\
-c2 -x 'FWpriv'[,'pswd']      Make PKCS#7 signed FwCapsule(Note2,Note3)$(EOL)\
-f'file' input, un-signed BIOS image$(EOL)\
-o'file' output, signed FwCapsule image$(EOL)\
-y use embedded FwCapsule header to insert the signature$(EOL)\
-l'value' max size of a FwCapsule Header (file alignment)$(EOL)\
-n -k'key' insert Signer's public key inside of the signed image$(EOL)\
-r'rom.map' use a rom map from the external file$(EOL)\
-r2 use the RomMap from embedded FwCapsule header$(EOL)\
-r1 ignore the embedded RomMap - calculate hash over entire input file.$(EOL)\
-$$[Offs] offset within the file to adjust the RomMap ranges by$(EOL)\
=============================================================$(EOL)\
Note1. -c'key1'-k'key2'    :use PKCS#1v2.1 DER(PEM) encoded RSA2048 keys$(EOL)\
Note2. -c2 -x'key1'-k'key2':key1-PKCS#12(PFX) with optional PFX password;$(EOL)\
                            key2-X.509(DER) public key of 'key1'$(EOL)\
Note3. -c2 -x command will access Msft signtool.exe as external process$(EOL)\
=============================================================$(EOL)\
=== Extended instructions to build FwCapsule using 3rd party sign tools$(EOL)\
=============================================================$(EOL)\
1. Serialize to be signed BIOS regions$(EOL)\
   -c2 -s -r -f BIOS.ROM -o serial.bin$(EOL)\
2. Create PKCS#7 signature of the serial data using any available$(EOL)\
   signing utility, e.g. signtool.exe, openSSL etc.$(EOL)\
3. Merge the signature file into a final FwCapsule image$(EOL)\
   -c2 -s -x'p7.sig' -f BIOS.ROM"\
>> $(FWCAPSULE_MAKE_README)

#--------------------------------------------------------------------------
# 2. Creating Signing descriptor table (RomLayout map) file
#--------------------------------------------------------------------------
$(ROM_LAYOUT_EX): $(UNSIGNED_BIOS_ROM) 
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	FWBuild $< -v -m $@
else
	FWBuild $< /v /m $@
endif

#---------------------------------------------------------------------------
# 3. Embed Signing descriptor table "$(ROM_LAYOUT_EX)" inside "$(UNSIGNED_BIOS_ROM)"
#--------------------------------------------------------------------------
MOD_FWCAPSULE_HDR_FFS: $(ROM_LAYOUT_EX) $(UNSIGNED_BIOS_ROM)
#dbg
#	$(CP) $(UNSIGNED_BIOS_ROM) $(UNSIGNED_BIOS_ROM).ORG
#dbg	
	@echo ----Update "$(UNSIGNED_BIOS_ROM)" with extended rom map "$(ROM_LAYOUT_EX)"
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	chmod +x $(CRYPTCON:-@=)
endif
	$(CRYPTCON) $(CRYPTOCON_CMDLINE_MAP) -f $(UNSIGNED_BIOS_ROM) -o $(UNSIGNED_BIOS_ROM)

#---------------------------------------------------------------------------
# 4. Invoke cryptocon.exe to create Signed FwCapsule if CREATE_FWCAPSULE == 1
#---------------------------------------------------------------------------
# Should be the last step after creating of the ROM image. All fixups to the .ROM must be made prior to this step.
# check END target in the MAIN.MAK and all .MAK files to make sure this step is not overriden
#---------------------------------------------------------------------------
$(FWCAPSULE_BUILD_TARGET): $(FWCAPSULE_FILE)
$(FWCAPSULE_FILE): MOD_FWCAPSULE_HDR_FFS $(UNSIGNED_BIOS_ROM) $(ROM_LAYOUT_EX)
ifeq ($(CREATE_FWCAPSULE),1)
	@echo ----Create signed BIOS image "$(FWCAPSULE_FILE)"
	-$(CRYPTCON) $(CRYPTOCON_CMDLINE_SIG) -f $(UNSIGNED_BIOS_ROM) -o $(FWCAPSULE_FILE)
#####
#ifeq ($(FWCAPSULE_CERT_FORMAT),1)
#An example of alternative way of using cryptocon to build PKCS#7 signed Fw Capsule.
#Cryptocon is used for packaging the capsule while the signing is done by an external tool
#
#1. Optional step if new BIOS verification key $(FWpub) needs to be embedded into $(UNSIGNED_BIOS_ROM)
#	$(CRYPTCON) -y -c2 -n -k $(FWpub) -f $(UNSIGNED_BIOS_ROM) -o $(UNSIGNED_BIOS_ROM)
#
#2. Create serialized data stream "fwcap_serialized" to be signed in step 3
#	$(CRYPTCON) -y -c2 -s -f $(UNSIGNED_BIOS_ROM) -o fwcap_serialized
#
#3. Create .p7 detached certificate file by signing of "fwcap_serialized" using Microsoft Signtool.exe:
#   Make sure to use to use the version of Microsoft SignTool.exe that supports /p7 switch
#   Latest tool version can be downloaded as part of Win8/10 SDK from http://msdn.microsoft.com/en-us/windows/hardware/hh852363.aspx. 
#    3.1 sign using a certificate whose private key information is protected by a hardware cryptography module (e.g. HSM). 
#    SignTool sign /fd sha256 /p7 $(BUILD_DIR) /p7co 1.2.840.113549.1.7.1 /p7ce DetachedSignedData /csp "Hardware Cryptography Module" /f SigCert.cer /k KeyContainerName fwcap_serialized
#        A computer store is specified for the certification authority (CA) store; Certificate is identified by a Subject Name "My High Value Certificate" .
#    SignTool sign /fd sha256 /p7 $(BUILD_DIR) /p7co 1.2.840.113549.1.7.1 /p7ce DetachedSignedData /sm /n "My High Value Certificate" fwcap_serialized
#    3.2 sign using a certificate stored in a password-protected PFX file "$(FWpriv)"
#        using openSSL or Msft SignTool 
#	 openssl smime -sign -md sha256 -binary -noattr -in fwcap_serialized -out fwcap_serialized.p7 -outform DER -inkey $(FWpriv) -signer $(FWpub) -passin pass:$(FW_PFX_Password)
#    or
#	 SignTool sign /fd sha256 /p7 . /p7co 1.2.840.113549.1.7.1 /p7ce DetachedSignedData /p "$(FW_PFX_Password)" /f $(FWpriv) fwcap_serialized
#
#4. Creating Fw Capsule image $(FWCAPSULE_FILE) with .p7 signature embedded into a FwCap header
#	$(CRYPTCON) -y -c2 -s -x fwcap_serialized.p7 -f $(UNSIGNED_BIOS_ROM) -o $(FWCAPSULE_FILE)
#endif # ifeq ($(FWCAPSULE_CERT_FORMAT),1)
#####
#
#ifeq ($(FWCAPSULE_FILE_FORMAT),0)
# Target FWCAPSULE_FILE file is an original unsigned UNSIGNED_BIOS_ROM with Fw Signature block embedded as Ffs file
# Replace original UNSIGNED_BIOS_ROM with the signed one if this file will be used as input to Intel fitc or other Flash image packaging tool
#	@if exist $@ $(CP) $@ $(UNSIGNED_BIOS_ROM)
#endif
endif #ifeq  ($(CREATE_FWCAPSULE),1)
#---------------------------------------------------------------------------
ifneq ($(BUILD_OS), $(BUILD_OS_LINUX))
	@if not exist $@ @echo WARNING !!! Failed to create signed BIOS image '$@'
else
	@if [ ! -f "$@" ]; then echo "WARNING !!! Failed to create signed BIOS image '$@'"; fi
endif    
ifneq ($(wildcard $(FWpub)),$(FWpub))
	@ECHO WARNING !!! Cannot open a file '$(FWpub)' with RSA public key.
	@ECHO Final BIOS image includes the dummy key as a Platform Key placeholder.
	@ECHO WARNING !!! The flash updates will fail, unless the valid key is set in FWKey FFS (FV_BB).
	@ECHO Try re-building the BIOS with a valid key file or run cryptocon with switch '-n -k key' to embedd the key into the BIOS binary.
endif
ifeq ($(FWCAPSULE_CERT_FORMAT),1)
ifeq ($(CONFIG_PEI_PKCS7),0)
	@ECHO WARNING !!! The 'Capsule' or 'Recovery' flash updates will fail with PKCS#7 Signed FwCapsules.
	@ECHO Include PKCS#7 library in PEI by setting SDL Token CONFIG_PEI_PKCS7 to 1.
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
