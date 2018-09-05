

#ifndef __LOGOID_H__
#define __LOGOID_H__



#define LOGO_SIGNATURE   0x44494C24      ///< '$LID' signature of the LOGO ID STRUCTURE


#pragma pack (1)
typedef struct{
UINT32      LogoID;                 ///< Signature '$LID'
UINT16      IntegrityCheck;         ///< Integrity Check
UINT8       Status;                 //<  For SmcChangelogo checking
}LOGO_ID;
#pragma pack ()





#endif

