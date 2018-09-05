
if exist Build\SmcOOB\RELEASE_MYTOOLS goto CopyEfiBin
if exist Build\SmcOOB\DEBUG_MYTOOLS goto CopyEfiDbg
goto CopyEfiDone

:CopyEfiBin
copy SmcPkg\Module\SmcOOB\Tool\SearchOneAndCopy.bat Build\SmcOOB\RELEASE_MYTOOLS
cd Build\SmcOOB\RELEASE_MYTOOLS
call SearchOneAndCopy.bat SmcAssetInfo.efi ..\..\..\SmcPkg\Module\SmcOOB_\SmcAssetInfo\SmcAssetInfoBin.efi
call SearchOneAndCopy.bat SmcInBandDxe.efi ..\..\..\SmcPkg\Module\SmcOOB_\SmcInBand\SmcInBandDxeBin.efi
call SearchOneAndCopy.bat SmcInBandSmm.efi ..\..\..\SmcPkg\Module\SmcOOB_\SmcInBand\SmcInBandSmmBin.efi
call SearchOneAndCopy.bat SmcOutBand.efi ..\..\..\SmcPkg\Module\SmcOOB_\SmcOutBand\SmcOutBandBin.efi
call SearchOneAndCopy.bat SmcLsiRaidOOBLib.lib ..\..\..\SmcPkg\Module\SmcOOB_\SmcLsiRaidOOB\SmcLsiRaidOOBLib.lib
cd ..\..\..\
goto CopyEfiDone

:CopyEfiDbg
copy SmcPkg\Module\SmcOOB\Tool\SearchOneAndCopy.bat Build\SmcOOB\DEBUG_MYTOOLS
cd Build\SmcOOB\DEBUG_MYTOOLS
call SearchOneAndCopy.bat SmcAssetInfo.efi ..\..\..\SmcPkg\Module\SmcOOB_\SmcAssetInfo\SmcAssetInfoDbg.efi
call SearchOneAndCopy.bat SmcInBandDxe.efi ..\..\..\SmcPkg\Module\SmcOOB_\SmcInBand\SmcInBandDxeDbg.efi
call SearchOneAndCopy.bat SmcInBandSmm.efi ..\..\..\SmcPkg\Module\SmcOOB_\SmcInBand\SmcInBandSmmDbg.efi
call SearchOneAndCopy.bat SmcOutBand.efi ..\..\..\SmcPkg\Module\SmcOOB_\SmcOutBand\SmcOutBandDbg.efi
call SearchOneAndCopy.bat SmcLsiRaidOOBLib.lib ..\..\..\SmcPkg\Module\SmcOOB_\SmcLsiRaidOOB\SmcLsiRaidOOBLibDbg.lib
cd ..\..\..\
goto CopyEfiDone

:CopyEfiDone
