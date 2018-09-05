set

@if %ReleaseBuild% == 1 goto official
@set BIOS_NAME=%prj%_T%TODAY4%%NOW3%.bin
echo set BIOS_NAME=%BIOS_NAME% > %WORKSPACE%\Build\SmcSetBiosName.bat
echo %prj%_T%TODAY4%%NOW3% >> %WORKSPACE%\romlist.txt

@if %BIOS_SIZE% == 0x1000000 goto Sps_16MB

@REM # 32MB ROM, NM
spsFITC.exe -b -f %prj%.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -gbea LBG_B1_X557_AT2_2x10G_P01_NCSI_v3.45_80000B5D.bin -o %WORKSPACE%\%BIOS_NAME%

goto SpsDone

:Sps_16MB

@REM # 16MB ROM, NM
spsFITC.exe -b -f %prj%_16M.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -o %WORKSPACE%\%BIOS_NAME%

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

@REM # NM 32M
spsFITC.exe -b -f %prj%.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -gbea LBG_B1_X557_AT2_2x10G_P01_NCSI_v3.45_80000B5D.bin -o %WORKSPACE%\%BIOS_NAME%

@REM # NM 16M
@REM # spsFITC.exe -b -f %prj%_16M.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -o %WORKSPACE%\%BIOS_NAME%

:end


