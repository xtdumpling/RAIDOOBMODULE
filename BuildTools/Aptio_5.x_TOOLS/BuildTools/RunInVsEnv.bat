@echo off
if defined VCINSTALLDIR goto Build
if defined VS140COMNTOOLS (
	call "%VS140COMNTOOLS%\vsvars32.bat"
	goto Build
)
if defined VS120COMNTOOLS (
	call "%VS120COMNTOOLS%\vsvars32.bat"
	goto Build	
)
if defined VS110COMNTOOLS (
	call "%VS110COMNTOOLS%\vsvars32.bat"
	goto Build
)
if defined VS100COMNTOOLS (
	call "%VS100COMNTOOLS%\vsvars32.bat"
	goto Build
)
if defined VS90COMNTOOLS (
	call "%VS90COMNTOOLS%\vsvars32.bat"
	goto Build
)
if defined VS80COMNTOOLS (
	call "%VS80COMNTOOLS%\vsvars32.bat"
	goto Build
)
if defined VS71COMNTOOLS (
	call "%VS71COMNTOOLS%\vsvars32.bat"
	goto Build
)
echo ERROR: RUN_VSVARS_BAT SDL Token is on and Visual Studio is not found
exit 1
:Build
%*
