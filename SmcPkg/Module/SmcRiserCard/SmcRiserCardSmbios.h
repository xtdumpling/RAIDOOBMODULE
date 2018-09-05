#ifndef _H_SmcRiserCardSmbios_
#define _H_SmcRiserCardSmbios_

#pragma pack(1)

typedef struct{
    UINT8                       PCIE_Bif_P;
    UINT8                       Device_Type_P;
    UINT8                       Device_Num_P;
    UINT8                       Slot_Type_P;
    UINT8                       Slot_Name_P;
    UINT8                       MUX_Address_P;
    UINT8                       MUX_Channel_P;
}RC_DEVICE_GROUP;

typedef struct{
    SMBIOS_STRUCTURE_HEADER     StructureType;  //0x00 ~ 0x03
    UINT16                      Header_Ver;     //0x04 ~ 0x05
    UINT16                      Header_Len;     //0x06 ~ 0x07
    UINT16                      Checksum;       //0x08 ~ 0x09
    UINT8                       SXB_Location;   //0x0A
    UINT16                      Card_ID;        //0x0B ~ 0x0C
    UINT16                      Card_Ver;       //0x0D ~ 0x0E
    UINT8                       Card_Name;      //0x0F
    UINT8                       Total_Lane;     //0x10
    RC_DEVICE_GROUP             Dev_Group;      //0x11 ~ 0x17
    UINT8                       Reserve[8];     //0x18 ~ 0x1F
}RC_SMBIOS_INFO;

#pragma pack()

#endif
