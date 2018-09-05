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
#  Rev. 1.00
#    Bug Fix:  XML Override Function.
#    Reason:   
#    Auditor:  Hartmann Hsieh
#    Date:     Sep/03/2014
#
#***************************************************************************
#***************************************************************************

import re
import sys

MeXmlFileName = sys.argv[1]
ItemOverrideFileName = sys.argv[2]
OutFileName = sys.argv[3]

print "ME XML Item Override."

ERROR_OCCURRED = 0

PatternItemAttr = re.compile (r"^\s*([0-9a-zA-Z_]+),(.+)\s*$", re.I)

#
# Parse XML Start
#

class MeXmlTag:
    def __init__(self, FormatType, TagName, NameAttr, OtherAttrString):
        self.FormatType = FormatType
        self.TagName = TagName
        self.NameAttr = NameAttr
        self.OtherAttrString = OtherAttrString

def MapKeyIterFind (KeyMap, KeyForCmp):
    Elements = []
    for i in range (1000):
        TmpKey = "%s@%d" % (KeyForCmp, i)
        if KeyMap.has_key (TmpKey):
            Elements.append (KeyMap[TmpKey])
    return Elements

def MapKeyCount (KeyMap, KeyForCmp):
    Count = 0
    for i in range (1000):
        TmpKey = "%s@%d" % (KeyForCmp, i)
        if KeyMap.has_key (TmpKey):
            Count = Count + 1
    return Count

def MapAddItem (KeyMap, Key, Value):
    for i in range (1000):
        TmpKey = "%s@%d" % (Key, i)
        if not KeyMap.has_key (TmpKey):
            KeyMap[TmpKey] = Value
            return 1
    return 0

def MapGetItemByIndex (KeyMap, Key, Index):
    TmpKey = "%s@%d" % (Key, Index)
    return KeyMap[TmpKey]


def ParseXml (AllLines, KeyLinePositionMapOne, KeyLinePositionMapTwo, MeXmlTagInfoMap):
    LevelValue = 0
    LineIndex = 0
    XmlPathNameTree = []
    ParentXmlPathName = ""

    for line in AllLines:

        match6 = re.match(r"\s*<\s*([0-9a-zA-Z_]+)\s+.*name=\"([^\"]+)\"([^>]*)\s*>[^<>]*<\s*/\s*([0-9a-zA-Z_]+)\s*>\s*", line, re.I)
        match4 = re.match(r"\s*<\s*([0-9a-zA-Z_]+)\s+.*name=\"([^\"]+)\"([^>]*)\s*/>\s*", line, re.I)
        match1 = re.match(r"\s*<\s*([0-9a-zA-Z_]+)\s+.*name=\"([^\"]+)\"([^>]*)\s*>\s*", line, re.I)
        match7 = re.match(r"\s*<([0-9a-zA-Z_]+)\s+([^>]+)>[^<>]*<\s*/\s*([0-9a-zA-Z_]+)\s*>\s*", line, re.I)
        match5 = re.match(r"\s*<([0-9a-zA-Z_]+)\s+([^>]+)>\s*", line, re.I)
        match8 = re.match(r"\s*<([0-9a-zA-Z_]+)\s*>[^<>]*<\s*/\s*([0-9a-zA-Z_]+)\s*>\s*", line, re.I)
        match3 = re.match(r"\s*<([0-9a-zA-Z_]+)\s*>\s*", line, re.I)
        match2 = re.match(r"\s*<\s*/\s*([0-9a-zA-Z_]+)\s*>\s*", line, re.I)
        if match6:
            #print ("6>(%d) %s, %s" % (LevelValue, match6.group (1), match6.group (2)))
            #print ("6<")
            CurrentKey = match6.group (1) + "[@name='" + match6.group (2) + "']"
            if LevelValue >= 2:
                MapAddItem (KeyLinePositionMapOne, CurrentKey, LineIndex)
                KeyTemp = ParentXmlPathName + "/" + CurrentKey
                MapAddItem (KeyLinePositionMapTwo, KeyTemp, LineIndex)
                MeXmlTagInfoMap[LineIndex] = MeXmlTag (6, match6.group (1), match6.group (2), match6.group (3))
        elif match4:
            #print ("4>(%d) %s, %s" % (LevelValue, match4.group (1), match4.group (2)))
            #print ("4<")
            CurrentKey = match4.group (1) + "[@name='" + match4.group (2) + "']"
            if LevelValue >= 2:
                MapAddItem (KeyLinePositionMapOne, CurrentKey, LineIndex)
                KeyTemp = ParentXmlPathName + "/" + CurrentKey
                MapAddItem (KeyLinePositionMapTwo, KeyTemp, LineIndex)
                MeXmlTagInfoMap[LineIndex] = MeXmlTag (4, match4.group (1), match4.group (2), match4.group (3))
        elif match1:
            #print ("1>(%d) %s, %s" % (LevelValue, match1.group (1), match1.group (2)))
            CurrentKey = match1.group (1) + "[@name='" + match1.group (2) + "']"
            if LevelValue >= 2:
                MapAddItem (KeyLinePositionMapOne, CurrentKey, LineIndex)
                KeyTemp = ParentXmlPathName + "/" + CurrentKey
                MapAddItem (KeyLinePositionMapTwo, KeyTemp, LineIndex)
                MeXmlTagInfoMap[LineIndex] = MeXmlTag (1, match1.group (1), match1.group (2), match1.group (3))
            LevelValue = LevelValue + 1
            ParentXmlPathName = CurrentKey
            XmlPathNameTree.append (ParentXmlPathName)
        elif match7:
            #print ("7>(%d) %s, %s, %s" % (LevelValue, match7.group (1), match7.group (2), match7.group (3)))
            #print ("7<")
            CurrentKey = match7.group (1)
            if LevelValue >= 2:
                MapAddItem (KeyLinePositionMapOne, CurrentKey, LineIndex)
                KeyTemp = ParentXmlPathName + "/" + CurrentKey
                MapAddItem (KeyLinePositionMapTwo, KeyTemp, LineIndex)
                MeXmlTagInfoMap[LineIndex] = MeXmlTag (7, match7.group (1), "", match7.group (2))
        elif match5:
            #print ("5>(%d) %s, %s" % (LevelValue, match5.group (1), match5.group (2)))
            CurrentKey = match5.group (1)
            if LevelValue >= 2:
                MapAddItem (KeyLinePositionMapOne, CurrentKey, LineIndex)
                KeyTemp = ParentXmlPathName + "/" + CurrentKey
                MapAddItem (KeyLinePositionMapTwo, KeyTemp, LineIndex)
                MeXmlTagInfoMap[LineIndex] = MeXmlTag (5, match5.group (1), "", match5.group (2))
            LevelValue = LevelValue + 1
            ParentXmlPathName = CurrentKey
            XmlPathNameTree.append (ParentXmlPathName)
        elif match8:
            #print ("8>(%d) %s, %s" % (LevelValue, match8.group (1), match8.group (2)))
            #print ("8<")
            CurrentKey = match8.group (1)
            if LevelValue >= 2:
                MapAddItem (KeyLinePositionMapOne, CurrentKey, LineIndex)
                KeyTemp = ParentXmlPathName + "/" + CurrentKey
                MapAddItem (KeyLinePositionMapTwo, KeyTemp, LineIndex)
                MeXmlTagInfoMap[LineIndex] = MeXmlTag (8, match8.group (1), "", "")
        elif match3:
            #print ("3>(%d) %s" % (LevelValue, match3.group (1)))
            CurrentKey = match3.group (1)
            if LevelValue >= 2:
                MapAddItem (KeyLinePositionMapOne, CurrentKey, LineIndex)
                KeyTemp = ParentXmlPathName + "/" + CurrentKey
                MapAddItem (KeyLinePositionMapTwo, KeyTemp, LineIndex)
                MeXmlTagInfoMap[LineIndex] = MeXmlTag (3, match3.group (1), "", "")
            LevelValue = LevelValue + 1
            ParentXmlPathName = CurrentKey
            XmlPathNameTree.append (ParentXmlPathName)
        elif match2:
            #print ("2<")
            MeXmlTagInfoMap[LineIndex] = MeXmlTag (2, match2.group (1), "", "")
            LevelValue = LevelValue - 1
            ParentXmlPathName = XmlPathNameTree.pop ()
    
        LineIndex = LineIndex + 1

    #print "LevelValue = %d" % (LevelValue)

def ReParseXml (AllLines, KeyLinePositionMapOne, KeyLinePositionMapTwo, MeXmlTagInfoMap):
    KeyLinePositionMapOne.clear ()
    KeyLinePositionMapTwo.clear ()
    LineTagMap.clear ()
    ParseXml (AllLines, KeyLinePositionMapOne, KeyLinePositionMapTwo, MeXmlTagInfoMap)

XmlLines = []

f = open (MeXmlFileName, "r")
for line in f:
    XmlLines.append (line)
f.close()

KeyLineMapOne = {}
KeyLineMapTwo = {}
LineTagMap = {}

ParseXml (XmlLines, KeyLineMapOne, KeyLineMapTwo, LineTagMap)

"""
print "KeyLineMapOne : "
for key in KeyLineMapOne.keys ():
    print "%s\t%s" % (key, KeyLineMapOne[key])
print "\n"

print "KeyLineMapTwo : "
for key in KeyLineMapTwo.keys ():
    print "%s\t%s" % (key, KeyLineMapTwo[key])
print "\n"
"""

#
# Parse XML End
#


ItemOverrideLines = []
f = open (ItemOverrideFileName, "r")
for line in f:
    ItemOverrideLines.append (line)
f.close()

def GetXmlLinePosition (KeyLineMap, XmlPathStr, DupDataIndex):
    XmlPathNotFound = 1
    TheXmlLinePos = -1
    DupDataCount = MapKeyCount (KeyLineMap, XmlPathStr)
    if DupDataCount > 0:
        if DupDataCount == 1:
            #if XmlPathStr == "SdrClass[@name='SDR NM SmaRT&amp;CLST Sensor']":
            #    print "DEBUG05 : 00"
            TheXmlLinePos = MapGetItemByIndex (KeyLineMap, XmlPathStr, 0)#KeyLineMap[XmlPathStr]
            XmlPathNotFound = 0
        else:
            if DupDataIndex == -1:
                XmlPathNotFound = 1
                print "=================="
                print "<XIO Format Error>"
                print "File : %s" % (ItemOverrideFileName)
                print "Line : %s" % (line)
                print "Message : %s" % ("There are many duplicated data by XML path. Index should be set to locate one data.")
                print "=================="
                ERROR_OCCURRED = 1
            else:
                if DupDataIndex >= DupDataCount:
                    XmlPathNotFound = 1
                    print "=================="
                    print "<XIO Format Error>"
                    print "File : %s" % (ItemOverrideFileName)
                    print "Line : %s" % (line)
                    print "Message : %s" % ("Index is too big.")
                    print "=================="
                    ERROR_OCCURRED = 1
                else:
                    #if XmlPathStr == "SdrClass[@name='SDR NM SmaRT&amp;CLST Sensor']":
                    #    print "DEBUG05 : 01"
                    TheXmlLinePos = MapGetItemByIndex (KeyLineMap, XmlPathStr, DupDataIndex)
                    XmlPathNotFound = 0
    else:
        XmlPathNotFound = 1

    return (XmlPathNotFound, TheXmlLinePos)


def InsertItemIntoXml (Map1, Map2, MeXmlTagInfoMap, XLines, LineAfterPos, Item, ItemAttr):
    XLines.insert (LineAfterPos, "<" + Item + " " + ItemAttr + ">\n")
    XLines.insert (LineAfterPos + 1, "</" + Item + ">\n")
    ReParseXml (XLines, Map1, Map2, MeXmlTagInfoMap)


for line in ItemOverrideLines:

    #print line.strip()
    StartPos = 0
    StrLen = len (line)
    StringToken1 = ""
    StringToken2 = ""
    StringToken3 = ""
    StringToken4 = ""
    StringTokenFormatError = 0

    # Comment line
    match1 = re.match(r"^\s*#.*", line, re.I)
    if match1:
        continue

    # Empty line.
    match1 = re.match(r"^\s*$", line, re.I)
    if match1:
        continue

    pattern = re.compile (r"([0-9a-zA-Z_]+);", re.I)
    match5 = pattern.search (line, StartPos)
    if match5:
        StringToken1 = match5.group (1)
        StartPos = match5.end()
        #print "=== 1 : " + match5.group (1)
        StringTokenFormatError = 1
        while (StartPos < StrLen):
            #
            # Match the format - "IchStrap0[@name='PCH Strap 0']".
            #
            pattern = re.compile (r"([0-9a-zA-Z_]+\[@[0-9a-zA-Z_]+='[^']+'\])", re.I)
            match5 = pattern.search (line, StartPos)

            #
            # Match the format - "/FlashComponent1Size".
            #
            pattern2 = re.compile (r"/([^/;]+)", re.I)
            match6 = pattern2.search (line, StartPos)

            if match5:
                StartPos = match5.end()
                StringToken2 = StringToken2 + "/" + match5.group (1)
                StringTokenFormatError = 0
                #print "=== 2.1 : StartPos = %d" % (StartPos)
                #print "=== 2.1 : " + match5.group (1)
            elif match6:
                StartPos = match6.end()
                StringToken2 = StringToken2 + "/" + match6.group (1)
                StringTokenFormatError = 0
                #print "=== 2.2 : StartPos = %d" % (StartPos)
                #print "=== 2.2 : " + match6.group (1)
            else:
                break

        if StringTokenFormatError == 0:

            StringToken2 = StringToken2[1:] # Clear the first character - '/'.

            pattern = re.compile (r";([0-9]*);", re.I)
            match5 = pattern.search (line, StartPos)
            if match5:
                StringToken3 = match5.group (1)
                StartPos = match5.end()
                #print "=== 3 : StartPos = %d" % (StartPos)
                #print "=== 3 : " + match5.group (1)
                StringToken4 = line[StartPos:]
                #print "=== 4 : " + StringToken4
            else:
                StringTokenFormatError = 2

    else:
        StringTokenFormatError = 3

    #print (">>>StringToken1:" + StringToken1)
    #print (">>>StringToken2:" + StringToken2)
    #print (">>>StringToken3:" + StringToken3)
    #print (">>>StringToken4:" + StringToken4)

    #StringToken2 = StringToken2.replace ("&amp;", "&")
    #StringToken2 = StringToken2.replace ("&apos;", "'")
    #StringToken2 = StringToken2.replace ("&quot;", "\"")
    #StringToken2 = StringToken2.replace ("&lt;", "<")
    #StringToken2 = StringToken2.replace ("&gt;", ">")

    if StringTokenFormatError:
        print "=================="
        print "<XIO Format Error> 01"
        print "Error Code : 0x%x" % (StringTokenFormatError)
        print "File : %s" % (ItemOverrideFileName)
        print "Line : %s" % (line)
        print "=================="
        ERROR_OCCURRED = 1
    else:
        OperateStr = StringToken1
        XmlPathStr = StringToken2
        IndexInt = -1
        if re.match(r"^[0-9]+$", StringToken3, re.I):
            IndexInt = int (StringToken3)
        ValueStr = StringToken4

        if OperateStr == "AddItem":
            StartPos = 0
            StrLen = len (ValueStr)
            XmlPathNotFound = 1
            TheXmlLinePosition = -1;

            if "/" in XmlPathStr:
                (XmlPathNotFound, TheXmlLinePosition) = GetXmlLinePosition (KeyLineMapTwo, XmlPathStr, IndexInt)
            else:
                (XmlPathNotFound, TheXmlLinePosition) = GetXmlLinePosition (KeyLineMapOne, XmlPathStr, IndexInt)

            if not XmlPathNotFound and TheXmlLinePosition >= 0:
                match3 = PatternItemAttr.search (ValueStr)
                if match3:
                    print "%s" % (XmlPathStr)
                    print "Add Item <%s %s>" % (match3.group (1), match3.group (2))
                    InsertItemIntoXml (KeyLineMapOne, KeyLineMapTwo, LineTagMap, XmlLines, TheXmlLinePosition + 1, match3.group (1), match3.group (2))
                else:
                    print "=================="
                    print "<XIO Format Error>"
                    print "File : %s" % (ItemOverrideFileName)
                    print "Line : %s" % (line)
                    print "Message : Format errors in %s." % (ValueStr)
                    print "=================="
                    ERROR_OCCURRED = 1

            if XmlPathNotFound:
                print "=================="
                print "<The XML path can't be found in XML.>"
                print "File     : %s" % (ItemOverrideFileName)
                print "XML Path : %s" % (XmlPathStr)
                print "=================="
                ERROR_OCCURRED = 1

        elif (OperateStr == "ModAttr" or OperateStr == "AddAttr" or OperateStr == "DelAttr"):

            XmlPathNotFound = 1

            if "/" in XmlPathStr:
                (XmlPathNotFound, TheXmlLinePosition) = GetXmlLinePosition (KeyLineMapTwo, XmlPathStr, IndexInt)
            else:
                (XmlPathNotFound, TheXmlLinePosition) = GetXmlLinePosition (KeyLineMapOne, XmlPathStr, IndexInt)

            if not XmlPathNotFound and TheXmlLinePosition >= 0:
                print "%s" % (XmlPathStr)
                TmpXioAttrs = {}
                StartPos = 0
                StrLen = len (ValueStr)
                while (StartPos < StrLen):
                    pattern = re.compile (r"([0-9a-zA-Z_]+)=\"([^\"]+)\"", re.I)
                    match5 = pattern.search (ValueStr, StartPos)
                    if match5:
                        AttrName = match5.group (1)
                        AttrValue = match5.group (2)
                        #print "(%s=%s)" % (AttrName, AttrValue)
                        TmpXioAttrs[AttrName] = AttrValue
                        TmpXmlLine = XmlLines[TheXmlLinePosition]
                        if OperateStr == "ModAttr":
                            print "Modify Attribute %s=%s" % (AttrName, AttrValue)
                            PatternSingleAttr = re.compile(AttrName + "=\"[^\"]+\"")
                            XmlLines[TheXmlLinePosition] = PatternSingleAttr.sub( AttrName + "=\"" + AttrValue + "\"", TmpXmlLine)
                        elif OperateStr == "DelAttr":
                            print "Delete Attribute %s=%s" % (AttrName, AttrValue)
                            PatternSingleAttr = re.compile(AttrName + "=\"[^\"]+\"")
                            XmlLines[TheXmlLinePosition] = PatternSingleAttr.sub( "", TmpXmlLine)
                        elif OperateStr == "AddAttr":
                            print "Add Attribute %s=%s" % (AttrName, AttrValue)
                            PatternSingleAttr = re.compile(">")
                            XmlLines[TheXmlLinePosition] = PatternSingleAttr.sub( " " + AttrName + "=\"" + AttrValue + "\">", TmpXmlLine, count=1)
                        StartPos = match5.end()
                    else:
                        break

                TmpTagInfo = LineTagMap[TheXmlLinePosition]
                #print "TmpTagInfo.FormatType = %d" % (TmpTagInfo.FormatType)
                #print "TmpTagInfo.TagName = %s" % (TmpTagInfo.TagName)
                #print "TmpTagInfo.NameAttr = %s" % (TmpTagInfo.NameAttr)
                #print "TmpTagInfo.OtherAttrString = %s" % (TmpTagInfo.OtherAttrString)


            if XmlPathNotFound:
                print "=================="
                print "<The XML path can't be found in XML.>"
                print "File     : %s" % (ItemOverrideFileName)
                print "XML Path : %s" % (XmlPathStr)
                print "=================="
                ERROR_OCCURRED = 1




#
# Write output file.
#

fw = open (OutFileName, "w")
for ll in XmlLines:
    fw.write (ll)
fw.close()

