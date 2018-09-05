#***************************************************************************
#**                                                                       **
#**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
#**                                                                       **
#**                                                                       **
#***************************************************************************
#***************************************************************************
#
#  File History
#
#  Rev. 1.03
#    Bug Fix:  Correct compile error if enable Skip_Integrity_Check_From_Lid.
#    Reason:   
#    Auditor:  William Hsu
#    Date:     Apr/14/2017
#
#  Rev. 1.02
#    Bug Fix:  Support Early Graphic Logo with TSE_ROMHOLE_SUPPORT enabled.
#    Reason:   
#    Auditor:  Isaac Hsu
#    Date:     Mar/22/2017
#
#  Rev. 1.01
#    Bug Fix:  Patch IntegrigyCheck before signed BIOS when
#              SecureMod_SUPPORT enabled.
#    Reason:   
#    Auditor:  Isaac Hsu
#    Date:     Mar/17/2017
#
#  Rev. 1.00
#    Bug Fix:  Early Graphic Logo Support.
#    Reason:   
#    Auditor:  Isaac Hsu
#    Date:     Mar/14/2017
#
#***************************************************************************
#***************************************************************************



Prepare: PrepareEarlyLogoFile

PrepareEarlyLogoFile:
# Split LOGO BMP to severals pics for EarlyVideo usage
	call BuildTools\Smc\RunPy.bat BuildTools\Smc\SplitImg.py


ifeq ($(SecureMod_SUPPORT),0)
CreateBinFile : PatchIntegrityCheck
else
$(FWCAPSULE_FILE) : PatchIntegrityCheck
endif


PatchIntegrityCheck:
# Patch the IntegrityCheck of the BMP LOGO FFS to the IntegrityCheck of the $LID
ifeq ($(Skip_Integrity_Check_From_Lid),0)
	call BuildTools\Smc\RunPy.bat BuildTools\Smc\PatchIntegrityChk.py $(OUTPUT_DIR)\FV\AMIROM.fd
	copy /Y PBIOS.rom $(OUTPUT_DIR)\FV\AMIROM.fd
	del PBIOS.rom
endif

