@call Build\setprj.bat
@copy /Y SmcPkg\Project\%prj%\OverrideSps\%prj%.xml Build\
@Build\ChkMEsetting.exe Build\ChkMEsettings.cfg -f Build\%prj%.xml
