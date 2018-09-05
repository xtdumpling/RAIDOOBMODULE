@REM #***************************************************************************
@REM #***************************************************************************
@REM #**                                                                       **
@REM #**       (C)Copyright 1993-2013 Supermicro Computer, Inc.                **
@REM #**                                                                       **
@REM #**                                                                       **
@REM #***************************************************************************
@REM #***************************************************************************
@REM #
@REM #  File History
@REM #
@REM #  Rev. 1.01
@REM #    Bug Fix:  When OOB building has errors, stop making.
@REM #    Reason:   
@REM #    Auditor:  Hartmann Hsieh
@REM #    Date:     Mar/13/14
@REM #
@REM #  Rev. 1.00
@REM #    Bug Fix:  Search for setup string define, setup string package and setup content in directory - "Build".
@REM #    Reason:   
@REM #    Auditor:  Hartmann Hsieh
@REM #    Date:     Oct/04/13
@REM #

for /f %%f in ('dir %1 /s/b') do copy %%f %2

