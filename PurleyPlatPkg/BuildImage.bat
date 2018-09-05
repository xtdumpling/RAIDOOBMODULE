@REM #***************************************************************************
@REM #**                                                                       **
@REM #**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
@REM #**                                                                       **
@REM #**                                                                       **
@REM #***************************************************************************
@REM #***************************************************************************
@REM #
@REM #  File History
@REM #
@REM #  Rev. 1.04
@REM #    Bug Fix:  Add the default IE image(From BMC team).
@REM #    Reason:   
@REM #    Auditor:  Jimmy Chiu
@REM #    Date:     Mar/01/2017
@REM #
@REM #  Rev. 1.03
@REM #    Bug Fix:  Remove unused code.
@REM #    Reason:   
@REM #    Auditor:  Hartmann Hsieh
@REM #    Date:     Sep/02/2016
@REM #
@REM #  Rev. 1.02
@REM #    Bug Fix:  Add previous SPS - 4.0.2.53.
@REM #    Reason:   
@REM #    Auditor:  Hartmann Hsieh
@REM #    Date:     Aug/02/2016
@REM #
@REM #  Rev. 1.01
@REM #    Bug Fix:  Add Sps 4.0.2.2 for PCH A stepping.
@REM #    Reason:   
@REM #    Auditor:  Kasber Chen
@REM #    Date:     Jun/18/2016
@REM #
@REM #  Rev. 1.00
@REM #    Bug Fix:  Initial revision.
@REM #    Reason:   For board level build image
@REM #    Auditor:  Salmon Chen
@REM #    Date:     Feb/18/2016
@REM #
@REM #***************************************************************************
@REM #***************************************************************************
@REM #
@REM This file contains an 'Intel Peripheral Driver' and is
@REM licensed for Intel CPUs and chipsets under the terms of your
@REM license agreement with Intel or your vendor.  This file may
@REM be modified by the user, subject to additional terms of the
@REM license agreement
@REM
@REM #/*++
@REM #
@REM #  Copyright (c) 2010 - 2013, Intel Corporation. All rights reserved.<BR>
@REM #  This software and associated documentation (if any) is furnished
@REM #  under a license and may only be used or copied in accordance
@REM #  with the terms of the license. Except as permitted by such
@REM #  license, no part of this software or documentation may be
@REM #  reproduced, stored in a retrieval system, or transmitted in any
@REM #  form or by any means without the express written consent of
@REM #  Intel Corporation.
@REM #
@REM #  Module Name:
@REM #
@REM #    BuildImage.bat
@REM #
@REM #  Abstract:
@REM #
@REM #    Platform full 8M flash image build script.
@REM #
@REM #--*/

@REM # Supermicro + START
set FTOOL_DIR=%1
@REM # Supermicro - END

if not exist %OUTPUT_DIR%\ROM mkdir %OUTPUT_DIR%\ROM

@REM ---run FTool and create ROM images---

@REM # Supermicro + START

@echo on
echo %WORKSPACE%
echo %OUTPUT_DIR%

@REM #
@REM # Set the environment variable of project name.
@REM #

if not exist Build\setprj.bat goto SetPrjDone
call Build\setprj.bat
:SetPrjDone

@REM #
@REM # Set the environment variables of token that defined at "PurleyCrb.mak".
@REM #

if not exist Build\SetSmcEnv.bat goto SetSmcEnvDone
call Build\SetSmcEnv.bat
:SetSmcEnvDone

IF %SMCPKG_SUPPORT% NEQ 1 goto Not_OverrideSps_SUPPORT
IF %OverrideSps_SUPPORT% NEQ 1 goto Not_OverrideSps_SUPPORT

@REM #
@REM # Override SPS.
@REM #

mkdir .\Build\OverrideSps
copy .\SmcPkg\Project\%prj%\OverrideSps\*.* .\Build\OverrideSps\
copy .\SmcPkg\Binary\ie_region.bin .\Build\OverrideSps\
copy .\Build\TimeStamp.h .\Build\OverrideSps\
copy .\BuildTools\Smc\*.* .\Build\OverrideSps\

@REM #
@REM # Only Override XML, copy kernel's binaries and tool to SourceDir.
@REM #

mkdir .\Build\OverrideSps\platforms
mkdir .\Build\OverrideSps\sqldrivers
mkdir .\Build\OverrideSps\data
copy %FTOOL_DIR%\*.bin .\Build\OverrideSps
copy %FTOOL_DIR%\Config\*.* .\Build\OverrideSps
copy %FTOOL_DIR%\PTU\*.* .\Build\OverrideSps
copy %FTOOL_DIR%\10GBE\*.* .\Build\OverrideSps
copy %FTOOL_DIR%\Tools\FlashImageTool\*.* .\Build\OverrideSps
copy %FTOOL_DIR%\Tools\FlashImageTool\platforms\*.* .\Build\OverrideSps\platforms\
copy %FTOOL_DIR%\Tools\FlashImageTool\sqldrivers\*.* .\Build\OverrideSps\sqldrivers\
copy %FTOOL_DIR%\Tools\FlashImageTool\data\*.* .\Build\OverrideSps\data\

cd .\Build\OverrideSps

call RunPy.bat ParseTimeStamp.py TimeStamp.h DATER.BAT
if %errorlevel%==1 (
echo ParseTimeStamp failed.
EXIT /B 1
)
call DATER.BAT
call BuildSps.bat

goto END_BUILD
:Not_OverrideSps_SUPPORT

@REM # Supermicro - END
  cd %WORKSPACE%\%OUTPUT_DIR%\ROM
if "%3" == "ME" goto BUILD_ME

if "%~5"=="FPGA" (
  set IFWI_FLASH_SIZE=7
) else (
  set IFWI_FLASH_SIZE=6
)

@REM OEM RELEASE NOTE START: INTEGRATE RESTRICTED ITEM
@REM %WORKSPACE%\%1\Tools\FlashImageTool\spsFITC.exe -b -f %WORKSPACE%\%2\%3.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr %WORKSPACE%\%1\spsOperational.bin -rcv %WORKSPACE%\%1\spsRecovery.bin -gbe %WORKSPACE%\%OUTPUT_DIR%\ROM\GBE_NVM.bin -gbea %WORKSPACE%\%OUTPUT_DIR%\ROM\10GBE_NVM.bin -o %WORKSPACE%\%OUTPUT_DIR%\ROM\OutImage.bin
@REM if NOT "%~4" == "" %WORKSPACE%\%1\Tools\FlashImageTool\spsFITC.exe -b -f %WORKSPACE%\%2\%4.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr %WORKSPACE%\%1\spsOperational.bin -rcv %WORKSPACE%\%1\spsRecovery.bin -der1 %WORKSPACE%\%1\PTU\Purley_SpsNMPTU_signed.rom -gbe %WORKSPACE%\%OUTPUT_DIR%\ROM\GBE_NVM.bin -gbea %WORKSPACE%\%OUTPUT_DIR%\ROM\10GBE_NVM.bin -o %WORKSPACE%\%OUTPUT_DIR%\ROM\OutImage_NM.bin
%WORKSPACE%\%1\Tools\FlashImageTool\spsFITC.exe -b -f %WORKSPACE%\%2\%3.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr %WORKSPACE%\%1\spsOperational.bin -rcv %WORKSPACE%\%1\spsRecovery.bin -o %WORKSPACE%\%OUTPUT_DIR%\ROM\OutImage.bin
if NOT "%~4" == "" %WORKSPACE%\%1\Tools\FlashImageTool\spsFITC.exe -b -f %WORKSPACE%\%2\%4.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr %WORKSPACE%\%1\spsOperational.bin -rcv %WORKSPACE%\%1\spsRecovery.bin -o %WORKSPACE%\%OUTPUT_DIR%\ROM\OutImage_NM.bin


@REM OEM RELEASE NOTE END
@REM cd %WORKSPACE%
@REM cd %1\Tools\FlashImageTool
@REM  @del spsfitc.ini /f /q
goto END_BUILD

:BUILD_ME
%WORKSPACE%\%1\Tools\FlashImageTool\fit.exe -b -f %WORKSPACE%\%1\Tools\FlashImageTool\%2.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -me %WORKSPACE%\%OUTPUT_DIR%\ROM\ME_11.bin -gbe %WORKSPACE%\%OUTPUT_DIR%\ROM\GBE.bin -fp10gbe %WORKSPACE%\%OUTPUT_DIR%\ROM\10GBE_NVM.bin -o %WORKSPACE%\%OUTPUT_DIR%\ROM\OutImage_ME.bin

:END_BUILD
@echo off
