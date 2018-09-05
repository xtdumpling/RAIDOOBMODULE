import sys
import os
import binascii


PicNum = 48

f=open("SmcPkg\Binary\Logo\SuperM1.bmp",'rb')
ImgData=f.read()
f.close()


ImgWidth = ord(ImgData[0x13])*0x100 + ord(ImgData[0x12])
ImgHeight = ord(ImgData[0x17])*0x100 + ord(ImgData[0x16])
BmpOffset = ord(ImgData[0x0D])*0x1000000 + ord(ImgData[0x0C])*0x10000 + ord(ImgData[0x0B])*0x100 + ord(ImgData[0x0A])
PicWidth = ImgWidth*ImgHeight*3/PicNum

#print   "ImgWidth=%d ImgHeight=%d BmpOffset=0x%x PicWidth=0x%x" %(ImgWidth, ImgHeight, BmpOffset, PicWidth)

index = PicNum
Start = BmpOffset
End = Start + PicWidth
while index > 0:
    f = open("Build\SuperM1_%d.bin " %index, 'wb')
#    print "index:%d S:0x%x E:0x%x" %(index, Start, End)
    BinData = ImgData[Start:End]
    f.write(BinData)
    f.close()
    Start = Start + PicWidth
    End = Start + PicWidth
    index = index - 1






