In order to build Aptio V project the following VeB Configuration has to be performed:
NOTE: All the paths described below should have no back slash at the end of the path.
For example: it should be "C:\Tools" and not "C:\Tools\"

Tools->Options->Env Variables:
 CCX86DIR – 32-bit C compiler path. Default WDK path: C:\WinDDK\7600.16385.1\bin\x86\x86 
 CCX64DIR - 64-bit C compiler path. Default WDK path: C:\WinDDK\7600.16385.1\bin\x86\amd64
 TOOLS_DIR - Aptio V build tools path. The directory where this file resides.

Tools->Options->Tools Directories:
 Aptio V build tools path (TOOLS_DIR value)
 Name of the directory where nmake.exe resides (Default WDK path: C:\WinDDK\7600.16385.1\bin\x86)
