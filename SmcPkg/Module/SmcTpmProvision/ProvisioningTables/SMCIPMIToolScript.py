##****************************************************************************##
##****************************************************************************##
##**                                                                        **##
##**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **##
##**                          All Rights Reserved.                          **##
##**                                                                        **##
##****************************************************************************##
##****************************************************************************##
#
#  File History
#
#  Rev. 1.05
#    Bug Fix : N/A
#    Reason  : Fixed missed the second 32 bytes causes SUT gets incorrect hashing
#              algorithm number and then hanging up.
#    Auditor : Joe Jhang
#    Date    : Jan/26/2017
#
#  Rev. 1.04
#    Bug Fix : N/A
#    Reason  : Modifed script to prove the maximum capacity of single entry is
#              512 bytes and 01h-FFh entries in ME Storage have not been opened 
#              yet.
#    Auditor : Joe Jhang
#    Date    : Jan/25/2017
#
#  Rev. 1.03
#    Bug Fix : Fixed lost 32 bytes in TPM provisioning table during data transmission.
#    Reason  : Bug Fixed.
#    Auditor : Joe Jhang
#    Date    : Jan/20/2017
#
#  Rev. 1.02
#    Bug Fix : N/A.
#    Reason  : Implemented BMC IP input in Python for PPI-x OOB.
#    Auditor : Joe Jhang
#    Date    : Jan/13/2017
#
#  Rev. 1.01
#    Bug Fix : N/A.
#    Reason  : Follows Intel PPI-x OOB specification December 2016 Revision 0.9
#              Figure 4-2. PPI-x Logical Flow Diagram BMC: System RESET required.
#    Auditor : Joe Jhang
#    Date    : Jan/13/2017
#
#  Rev. 1.00
#    Bug Fix : N/A.
#    Reason  : Implemented IPMI ME Storage Services Write Command in Python
#              for sending TPM provisioning table remotely.
#    Auditor : Joe Jhang
#    Date    : Jan/12/2017
#
##**************************************************************************##
##**************************************************************************##

import subprocess, sys
import binascii
import struct

TpmProvisioningTable = sys.argv[1]
File=open(TpmProvisioningTable,'rb')
RawContent1 = File.read()
FileLength = len(RawContent1)

RawContent2 = binascii.hexlify(RawContent1)
HexList = map(''.join, zip(RawContent2[::2],RawContent2[1::2]))

#BinaryTableLen = len(RawContent2) / 2
BinaryTableLen = 512

BmcIpTemp=(raw_input('Please input BMC IP for SMCIPMITool:'))
BmcIp=[]
BmcIp.append(BmcIpTemp)

Divisible = 1
Remainder = BinaryTableLen % 32
if ((Remainder) != 0):
    Divisible = 0

PwrCycleRawCmdTemplate = ' ADMIN ADMIN ipmi raw 00 02 02'
RstServerRawCmdTemplate = ' ADMIN ADMIN ipmi raw 00 02 03'
IpmiMeStorageServWriteCmdTemplate = ['ADMIN ADMIN ipmi ipmb 00 2c 2e 91 57 01 00 00']

TotalLenRaw = struct.pack('<I', BinaryTableLen).encode('hex')
TotalLen9to12 = map(''.join, zip(*[iter(TotalLenRaw)]*2))

if (Divisible == 1):
    LastOffset = (BinaryTableLen // 32 - 1) * 32
else:
    LastOffset = BinaryTableLen // 32 * 32
LastOffsetRaw = struct.pack('<I', LastOffset).encode('hex')
LastOffset5to7 = map(''.join, zip(*[iter(LastOffsetRaw)]*2))
LastOffset5to7.pop(-1)

MessageCompleteDwordLenTemplate   = ['01']
MessageUnCompleteDwordLenTemplate = ['00']
LastOffset5to7MsgComplete = LastOffset5to7[:]
LastOffset5to7MsgComplete.extend(MessageCompleteDwordLenTemplate)
LastOffset5to7MsgComplete.extend(TotalLen9to12)

PwrCycleRawCmdArgv = []
PwrCycleRawCmdArgv.append(BmcIpTemp + PwrCycleRawCmdTemplate)
RstServerRawCmdArgv = []
RstServerRawCmdArgv.append(BmcIpTemp + RstServerRawCmdTemplate)

SmcIpmiToolArgvTemplateTemp1 = BmcIp[:]
SmcIpmiToolArgvTemplateTemp1.extend(IpmiMeStorageServWriteCmdTemplate)

def Combine (i, SmcIpmiToolArgvTemplate):
    OffsetDyn = i * 32
    OffsetRawDyn = struct.pack('<I', OffsetDyn).encode('hex')
    Offset5to7Dyn = map(''.join, zip(*[iter(OffsetRawDyn)]*2))
    Offset5to7Dyn.pop(-1)

    SmcIpmiToolArgvTemplate.extend(Offset5to7Dyn)
    SmcIpmiToolArgvTemplate.extend(MessageUnCompleteDwordLenTemplate)

def ExecuteSmciIpmiTool (SmcIpmiToolArgvTemplate):
    SmcIpmiToolArgvTemplate=' '.join(SmcIpmiToolArgvTemplate)
    SmcIpmiToolArgv=[]
    SmcIpmiToolArgv.append(SmcIpmiToolArgvTemplate)
    print(SmcIpmiToolArgv)
    subprocess.call(['SMCIPMITool.exe', SmcIpmiToolArgv], shell=True)

PackageNum = BinaryTableLen//32
for i in range (PackageNum - 1):
    if (i == (PackageNum - 2)):
        if (Divisible == 1):
            SmcIpmiToolArgvTemplate = SmcIpmiToolArgvTemplateTemp1[:]
            SmcIpmiToolArgvTemplate.extend(LastOffset5to7MsgComplete)
        else:
            SmcIpmiToolArgvTemplate = SmcIpmiToolArgvTemplateTemp1[:]
            Combine (i, SmcIpmiToolArgvTemplate)

        for j in range (32*(i+1), 32*(i+2)):
            SmcIpmiToolArgvTemplate.append(HexList[j])
    else:
        SmcIpmiToolArgvTemplate = SmcIpmiToolArgvTemplateTemp1[:]
        Combine (i, SmcIpmiToolArgvTemplate)

        for j in range (32*i, 32*(i+1)):
            SmcIpmiToolArgvTemplate.append(HexList[j])

    ExecuteSmciIpmiTool (SmcIpmiToolArgvTemplate)

if (Divisible == 0): 
    SmcIpmiToolArgvTemplate=SmcIpmiToolArgvTemplateTemp1[:]
    SmcIpmiToolArgvTemplate.extend (LastOffset5to7MsgComplete)
    print (LastOffset5to7MsgComplete)
    for j in range ((32*(i+2)), (32*(i+2))+Remainder):
        SmcIpmiToolArgvTemplate.append(HexList[j])

    ExecuteSmciIpmiTool (SmcIpmiToolArgvTemplate)

#
# Follows Intel PPI-x OOB specification December 2016 Revision 0.9 Figure 4-2. PPI-x Logical Flow Diagram BMC: System RESET required.
#
print('BMC: System RESET required!')
subprocess.call(['SMCIPMITool.exe', RstServerRawCmdArgv], shell=True)

File.close()
