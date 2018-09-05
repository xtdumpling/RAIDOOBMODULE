@set pathsave=%PATH%

set CCX86DIR=%cd%\BuildTools\WINDDK\7600.16385.1\bin\x86\x86
set CCX64DIR=%cd%\BuildTools\WINDDK\7600.16385.1\bin\x86\amd64
set TOOLS_DIR=%cd%\BuildTools\Aptio_5.x_TOOLS\BuildTools
set VEB_BUILD_DIR=Build
set PATH=%cd%;%TOOLS_DIR%;%cd%\BuildTools\WINDDK\7600.16385.1\bin\x86;%path%

set VEB=PurleyCrb


if "%1"=="clean" goto reclean
IF "%1" == "rebuild" goto rebuildall
IF "%1" == "build" goto build
goto build

:rebuildall
@if exist *.rom del *.rom
@if exist *.map del *.map
make rebuild
goto end

:build
make 
goto end

:reclean
make clean
@if exist *.aml del *.aml
@if exist *.asl del *.asl
@if exist *.bin del *.bin
@if exist *.brs del *.brs
@if exist *.cod del *.cod
@if exist *.dat del *.dat
@if exist *.idb del *.idb
@if exist *.lfo del *.lfo
@if exist *.map del *.map
@if exist *.pdb del *.pdb
@if exist *.rom del *.rom
@if exist *.map del *.map
@if exist NULL del NULL@
@if exist AmiCrbMeRoms rd /s /q AmiCrbMeRoms
:end

if exist Build\ copy SmcPkg\Module\SmcOOB\Tool\CopyEfi.bat Build\
if exist Build\ call Build\CopyEfi.bat

@set path=%pathsave%
