IF exist c:\Python27 goto add27
IF exist c:\Python26 goto add26

:add27
@echo "C:\Python27 found"
set PATH=%PATH%;c:\Python27;
goto	continue

:add26
@echo "C:\Python26 found"
set PATH=%PATH%;c:\Python26;

:continue

path

python %1 %2 %3 %4 %5 %6 %7 %8 %9

