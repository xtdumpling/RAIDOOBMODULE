#***************************************************************************
#**                                                                       **
#**       (C)Copyright 1993-2013 Supermicro Computer, Inc.                **
#**                                                                       **
#**                                                                       **
#***************************************************************************
#***************************************************************************
#
#  File History
#
#  Rev. 1.01
#    Bug Fix:  Fix compiler fail if add debug token in smcbuild.sdl
#    Reason:   
#    Auditor:  Salmon Chen
#    Date:     Mar/24/2015
#
#  Rev. 1.00
#    Bug Fix:  Add DOS env variable of project name.
#    Reason:   
#    Auditor:  Hartmann Hsieh
#    Date:     05/24/13
#
#***************************************************************************
#***************************************************************************

import re

f = open ("SmcBuild.sdl", "r")

matchObj = re.compile(r".+[\"\s]([xX][1][0-9a-zA-Z_]+)_SUPPORT.+", re.I)
for line in f:
    print line
    m = matchObj.match(line)
    if m:
        print "Found : " + m.group(1)
        fw = open("Build\\setprj.bat", "w")
        fw.writelines("set prj=" + m.group(1))
        fw.close()

f.close()
