import sys
import os
import binascii
import uuid



############################################################
# 1. Get BIOS File.
############################################################

BIOS = sys.argv[1]
#print "BIOS Name is", BIOS
f = open(BIOS,'rb')
BIOSData = f.read()
f.close()

############################################################
# 1. Find the IntegrityCheck of the BMP.
############################################################

"""
#define EFI_DEFAULT_BMP_LOGO_GUID \
  {0x7BB28B99,0x61BB,0x11d5,0x9A,0x5D,0x00,0x90,0x27,0x3F,0xC1,0x4D}

typedef struct {
  EFI_GUID                Name;            00-15
  EFI_FFS_INTEGRITY_CHECK IntegrityCheck;  16-17
  EFI_FV_FILETYPE         Type;            18
  EFI_FFS_FILE_ATTRIBUTES Attributes;      19
  UINT8                   Size[3];         20-22
  EFI_FFS_FILE_STATE      State;           23
} EFI_FFS_FILE_HEADER;

"""

BMPguid = "998BB27BBB61d5119a5d0090273fc14d"
BMPstring = BMPguid.decode("hex")
FfsBmpStart = BIOSData.rfind(BMPstring)

if FfsBmpStart == -1:
    print "FFS BMP not found"
    sys.exit()
#print "FFS BMP found, Start is", hex(FfsBmpStart)

Index = FfsBmpStart
IntegrityCheckL = ord(BIOSData[Index+0x10])
IntegrityCheckH = ord(BIOSData[Index+0x11])
#print "IntegrityCheck: 0x%x%x" %(IntegrityCheckL,IntegrityCheckH)

############################################################
# 2. Fill Checksum to IntegrityCheck field of the LID
############################################################

"""
typedef struct{
UINT32      LogoID[4];              ///< Signature '$LID'
UINT16      IntegrityCheck;         ///< Integrity Check
UINT8       Status;                 //<  For SmcChangelogo checking
}LOGO_ID;
"""

f = open("PBIOS.rom", 'wb')

if IntegrityCheckH < 0x10 :
    IntegrityCheckH = ''.join("0%x"%IntegrityCheckH)
else:
    IntegrityCheckH = ''.join("%x"%IntegrityCheckH)
IntegrityCheckH = binascii.a2b_hex(IntegrityCheckH)

if IntegrityCheckL < 0x10 :
    IntegrityCheckL = ''.join("0%x"%IntegrityCheckL)
else:
    IntegrityCheckL = ''.join("%x"%IntegrityCheckL)
IntegrityCheckL = binascii.a2b_hex(IntegrityCheckL)

Index = BIOSData.rfind('$LID')
Low = Index + 4
High = Index + 6
BinData = BIOSData[:Low] + IntegrityCheckL + IntegrityCheckH + BIOSData[High:]


f.write(BinData)
f.close()



