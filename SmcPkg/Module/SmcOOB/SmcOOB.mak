#****************************************************************************
#****************************************************************************
#**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
#****************************************************************************
#****************************************************************************
#  File History
#
#  Rev. 1.00
#    Bug Fix:  Initial revision.
#    Reason:   
#    Auditor:  Hartmann Hsieh
#    Date:     Mar/31/2015
#
#****************************************************************************

Prepare : SmcOOBPrepare

SmcOOBPrepare:

	@$(ECHO) Prepare SmcOOB.

	if exist Build\SMC.DAT $(RM) Build\SMC.DAT
	copy /Y SmcPkg\Module\SmcOOB\Binary\dummy.bin Build\SMC.DAT

	if exist OOB.log $(RM) OOB.log

CreateBinFile : SmcOOBCreateBinFile

SmcOOBCreateBinFile:

	@$(ECHO) call SmcPkg\Module\SmcOOB\Tool\CheckOobErrors.bat
	call SmcPkg\Module\SmcOOB\Tool\CheckOobErrors.bat

