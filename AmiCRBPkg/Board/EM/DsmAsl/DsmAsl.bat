@REM **********************************************************************
@REM **********************************************************************
@REM **                                                                  **
@REM **        (C)Copyright 1985-2010, American Megatrends, Inc.         **
@REM **                                                                  **
@REM **                       All Rights Reserved.                       **
@REM **                                                                  **
@REM **      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
@REM **                                                                  **
@REM **                       Phone: (770)-246-8600                      **
@REM **                                                                  **
@REM **********************************************************************
@REM **********************************************************************

@REM **********************************************************************
@REM <AMI_FHDR_START>
@REM
@REM Name:           DsmAsl.bat
@REM
@REM Description:    Batch file to build Pcislot.asl which implements
@REM                 _DSM methods.
@REM
@REM <AMI_FHDR_END>
@REM **********************************************************************
@for /F "tokens=1,2,3 delims=()," %%i in (%1) do @(
(@echo Scope(%%k%^){
@echo   Method ( _DSM, 4, NotSerialized, 0, UnknownObj, {BuffObj, IntObj, IntObj,PkgObj}^)
@echo   {
@echo         If (LEqual(Arg0, ToUUID ("E5C937D0-3553-4d7a-9117-EA4D19C3434D"^)^)^) {
@echo           If (Land( Lequal(Arg1, 2 ^), Lequal(Arg2,0 ^)^)^){
@echo              return (Buffer(^) {0x80}^)
@echo           }
@echo           If (Land( Lequal(Arg1, 2^), Lequal(Arg2,7 ^)^)^){
@echo              Return ( Package(2 ^){
@echo                          %%i%,
@echo                          Unicode("%%j%"^)
@echo                      }
@echo                  ^)
@echo           }
@echo          }
@echo              return(Buffer(^){0}^)
@echo   }
@echo })>>Build\PciSlot.asl 
)


@REM **********************************************************************
@REM **********************************************************************
@REM **                                                                  **
@REM **        (C)Copyright 1985-2010, American Megatrends, Inc.         **
@REM **                                                                  **
@REM **                       All Rights Reserved.                       **
@REM **                                                                  **
@REM **      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
@REM **                                                                  **
@REM **                       Phone: (770)-246-8600                      **
@REM **                                                                  **
@REM **********************************************************************
@REM **********************************************************************