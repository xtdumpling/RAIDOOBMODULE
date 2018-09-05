set Script="%WORKSPACE%\%BUILD_ROOT%\BeforeGenFds.bat"
if exist %Script% call %Script%
_GenFds %*
