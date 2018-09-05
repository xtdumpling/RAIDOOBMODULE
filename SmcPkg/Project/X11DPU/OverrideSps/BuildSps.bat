set

call ..\SetSmcEnv.bat

@if %ReleaseBuild% == 1 goto official
@set BIOS_NAME=%prj%_T%TODAY4%%NOW3%.bin
echo set BIOS_NAME=%BIOS_NAME% > %WORKSPACE%\Build\SmcSetBiosName.bat
echo %prj%_T%TODAY4%%NOW3% >> %WORKSPACE%\romlist.txt

@if %BIOS_SIZE% == 0x2000000 goto Sps_32MB

@REM # 16MB ROM, NM
@set BIOS_NAME=%prj%_T%TODAY4%%NOW3%_16M.bin
spsFITC.exe -b -f %prj%_16MB.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -o %WORKSPACE%\%BIOS_NAME%

goto Sps_Done
:Sps_32MB

@REM # 32MB ROM, NM
spsFITC.exe -b -f %prj%.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -o %WORKSPACE%\%BIOS_NAME%

:Sps_Done


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

@if %BIOS_SIZE% == 0x2000000 goto Sps_32MB_Official

@REM # 16MB, NM
@set BIOS_NAME=%prj%%year%.%month%%THIS_DAY%_16MB
spsFITC.exe -b -f %prj%_16MB.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -o %WORKSPACE%\%BIOS_NAME%

goto Sps_Done_Official
:Sps_32MB_Official

@REM # 32MB, NM
spsFITC.exe -b -f %prj%.xml -bios %WORKSPACE%\%OUTPUT_DIR%\FV\AMIROM.fd -opr spsOperational.bin -rcv spsRecovery.bin -o %WORKSPACE%\%BIOS_NAME%

:Sps_Done_Official

:end


