
call ..\SetSmcEnv.bat

@if %ReleaseBuild% == 1 goto official

:debug_bios

@set BIOS_NAME=%prj%_T%TODAY4%%NOW3%.bin
echo set BIOS_NAME=%BIOS_NAME% > %WORKSPACE%\Build\SmcSetBiosName.bat
echo %prj%_T%TODAY4%%NOW3% >> %WORKSPACE%\romlist.txt

@if %BIOS_SIZE% == 0x4000000 goto Sps_64MB
@if %BIOS_SIZE% == 0x2000000 goto Sps_32MB
set IFWI_FLASH_SIZE=5
goto Sps_build

:Sps_64MB
set IFWI_FLASH_SIZE=7
goto Sps_build


:Sps_32MB
set IFWI_FLASH_SIZE=6
goto Sps_build

:Sps_build
@REM spsFITC.exe -b -f %prj%.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -ie ie_region.bin -o %WORKSPACE%\%BIOS_NAME% -flashsize1 %IFWI_FLASH_SIZE%
spsFITC.exe -b -f %prj%.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -o %WORKSPACE%\%BIOS_NAME% -flashsize1 %IFWI_FLASH_SIZE%

:SpsDone

goto end

:official
@set year=%THIS_YEAR:~3,1%

@set month=%THIS_MONTH:~1,1%
@if %THIS_MONTH% == 10 set month=A
@if %THIS_MONTH% == 11 set month=B
@if %THIS_MONTH% == 12 set month=C

@set BIOS_NAME=%prj%%year%.%month%%THIS_DAY%
echo set BIOS_NAME=%BIOS_NAME% > %WORKSPACE%\Build\SmcSetBiosName.bat
echo %prj%%year% >> %WORKSPACE%\romlist.txt
@REM # SiEn
@REM # spsFITC.exe -b -f %prj%_SiEn.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -ie ie_region.bin -o %WORKSPACE%\%BIOS_NAME%
@REM # NM
@REM spsFITC.exe -b -f %prj%.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -ie ie_region.bin -o %WORKSPACE%\%BIOS_NAME% -flashsize1 7
spsFITC.exe -b -f %prj%.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -o %WORKSPACE%\%BIOS_NAME% -flashsize1 7

:end


