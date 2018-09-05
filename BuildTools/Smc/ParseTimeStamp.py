#***************************************************************************
#**                                                                       **
#**       (C)Copyright 1993-2014 Supermicro Computer, Inc.                **
#**                                                                       **
#**                                                                       **
#***************************************************************************
#***************************************************************************
#
#  File History
#
#  Rev. 1.01
#    Bug Fix:  Fix building error in Python 2.6.
#    Reason:   
#    Auditor:  Hartmann Hsieh
#    Date:     Apr/17/14
#
#  Rev. 1.00
#    Bug Fix:  Fix the time of ROM file name is different from the time of SMBIOS BIOS version.
#    Reason:   
#    Auditor:  Hartmann Hsieh
#    Date:     Apr/15/14
#
#***************************************************************************
#***************************************************************************

import re
import sys

TimeStampDefineFile = sys.argv[1]
OutPutFile = sys.argv[2]

DefNamesStr = "THIS_YEAR,THIS_MONTH,THIS_DAY,THIS_HOUR,THIS_MINUTE,THIS_SECOND"

DtStr = {}
DtVal = {}

matchList = {}

DefNames = DefNamesStr.split(',')

for DefName in DefNames:
    matchList[DefName] = re.compile(r"\s*#define\s+" + DefName + "\s+([0-9]+)\s*", re.I)

#
# Parse DefNames in TimeStampDefineFile.
#

f = open (TimeStampDefineFile, "r")

for line in f:
    mList = {}
    for DefName in DefNames:
        mList[DefName] = matchList[DefName].match(line)
        if mList[DefName]:
            DtStr[DefName] = mList[DefName].group (1)
            DtVal[DefName] = int (DtStr[DefName], 10)
            break

f.close()

#
# Check error.
#

for DefName in DefNames:
    if not DefName in DtVal:
        sys.exit(1)
    else:
        print "%s = %02d" % (DefName, DtVal[DefName])

#
# Write output file.
#

fw = open (OutPutFile, "w")

fw.write ("SET TODAY=%02d/%02d/%04d\n" % (DtVal["THIS_MONTH"], DtVal["THIS_DAY"], DtVal["THIS_YEAR"]))
fw.write ("SET TODAY1=%02d%02d%04d\n" % (DtVal["THIS_MONTH"], DtVal["THIS_DAY"], DtVal["THIS_YEAR"]))
fw.write ("SET TODAY2=%02d/%02d/%02d\n" % (DtVal["THIS_MONTH"], DtVal["THIS_DAY"], DtVal["THIS_YEAR"] - 2000))
fw.write ("SET TODAY3=%02d%02d%02d\n" % (DtVal["THIS_MONTH"], DtVal["THIS_DAY"], DtVal["THIS_YEAR"] - 2000))
fw.write ("SET TODAY4=%04d%02d%02d\n" % (DtVal["THIS_YEAR"], DtVal["THIS_MONTH"], DtVal["THIS_DAY"]))
fw.write ("SET TODAY5=%02d%02d%02d\n" % (DtVal["THIS_YEAR"] - 2000, DtVal["THIS_MONTH"], DtVal["THIS_DAY"]))
fw.write ("SET THIS_DAY=%02d\n" % (DtVal["THIS_DAY"]))
fw.write ("SET THIS_MONTH=%02d\n" % (DtVal["THIS_MONTH"]))
fw.write ("SET THIS_YEAR=%02d\n" % (DtVal["THIS_YEAR"]))
fw.write ("SET NOW=%02d:%02d:%02d\n" % (DtVal["THIS_HOUR"], DtVal["THIS_MINUTE"], DtVal["THIS_SECOND"]))
fw.write ("SET NOW1=%02d%02d%02d\n" % (DtVal["THIS_HOUR"], DtVal["THIS_MINUTE"], DtVal["THIS_SECOND"]))
fw.write ("SET NOW2=%02d:%02d\n" % (DtVal["THIS_HOUR"], DtVal["THIS_MINUTE"]))
fw.write ("SET NOW3=%02d%02d\n" % (DtVal["THIS_HOUR"], DtVal["THIS_MINUTE"]))

fw.close()


