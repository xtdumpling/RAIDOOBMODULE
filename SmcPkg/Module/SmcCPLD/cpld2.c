#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <string.h>


#define X86_GPO0        0x7000
#define     BMC_EXTRST_N    (1<<4)
#define     EN_ADR_TRIGGER  (1<<5)
#define     EN_NMI          (1<<1)
#define     EN_RST          (1<<0)
#define X86_GPO1        0x7001
#define X86_GPI2        0x7002
#define X86_GPI3        0x7003
#define X86_GPI4        0x7004
#define X86_GPI5        0x7005
#define X86_GPI6        0x7006
#define X86_GPI7        0x7007
#define TCCR0           0x705E
#define TCTOPSET0       0x7060
#define TCTOPSET1       0x7061
#define TCCR2           0x7064
#define TCCNT0          0x7065
#define TCCNT1          0x7066
#define TCTOP0          0x7067
#define TCTOP1          0x7068
#define TCSR0           0x706D

#define VERSION "2.00"

#define OEM_DEBUG   0

#define EFI_DEADLOOP() { volatile unsigned int __DeadLoopVar__ = 1; while (__DeadLoopVar__); }

#define EFI_NMI 0
#define EFI_RST 1

#define BIT0  1
#define BIT1  (1 << 1)
#define BIT2  (1 << 2)
#define BIT3  (1 << 3)
#define BIT4  (1 << 4)
#define BIT5  (1 << 5)
#define BIT6  (1 << 6)
#define BIT7  (1 << 7)
#define BIT8  (1 << 8)
#define BIT9  (1 << 9)
#define BIT28 (1 << 28)

#define HST_STS         0x00
#define HST_CNT         0x02
#define HST_CMD         0x03
#define HST_SLVA        0x04
#define HST_D0          0x05
#define TIMEOUT_COUNT   1024

#define STATIC          static
#define VOID            void
#define UINT8           unsigned char
#define UINT16          unsigned short
#define UINT32          unsigned int
#define EFI_STATUS      UINT32

#define EFI_DEVICE_ERROR    7
#define EFI_TIMEOUT         18
#define EFI_SUCCESS         0

#define IoWrite8(_port, _data)      outp(_port, _data)
#define IoWrite16(_port, _data)     outpw(_port, _data)
#define IoWrite32(_port, _data)     outpd(_port, _data)
#define IoRead8(_port)              inp(_port)
#define IoRead16(_port)             inpw(_port)
#define IoRead32(_port)             inpd(_port)

#define MEXP_SMBUS_PCI  0x8000fc00  // 0x80008b00
#define SMBUS_DIDVID    0xA1A38086  // 0x8d7f8086

UINT16 SMBUS_ADDRESS = 0;

STATIC void IODELAY()
{
    UINT8 i = 100;
    while(i) {
        IoWrite8(0xeb, 0);
        i--;
    }
}

STATIC EFI_STATUS _SmbusReadByte(UINT8 addr, UINT8 offset, UINT8* data)
{
        UINT8   iTemp;
        UINT32  iTimeout;

        // 1.  Check host busy
        iTimeout = TIMEOUT_COUNT;
        while( iTimeout && ( IoRead8(SMBUS_ADDRESS+HST_STS) & BIT0 ) ) {
                IODELAY();
                iTimeout--;
        }

        if ( 0 == iTimeout )    // If the host always busy, return
                return EFI_TIMEOUT;

        // 2.  Clear the status register
        IODELAY();
        IoWrite8(SMBUS_ADDRESS+HST_STS, 0xff);
    
        // 3.  Set the slave device address
        IODELAY();
        IoWrite8(SMBUS_ADDRESS+HST_SLVA, addr | BIT0);

        // 4. Set the offset byte of the slave device
        IODELAY();
        IoWrite8(SMBUS_ADDRESS+HST_CMD, offset);

        // 5. Fire
        IODELAY();
        IoWrite8(SMBUS_ADDRESS+HST_CNT, 0x48);  // Byte

        // 6. Wait the host
        IODELAY();
        iTimeout = TIMEOUT_COUNT;
        do {
                iTemp = IoRead8(SMBUS_ADDRESS);
                IODELAY();
                if ( iTemp & BIT2 )     // DEV_ERR
                        return EFI_DEVICE_ERROR;

                if ( 0 == (iTemp & BIT0) )      // host busy?
                        break;

                iTimeout--;
        } while (iTimeout);

        if ( 0 == iTimeout )    // If the host always busy, return
                return EFI_TIMEOUT;
        
        // 7. Clear the status register
        IODELAY();
        IoWrite8(SMBUS_ADDRESS, iTemp);

        // 8. Read the data
        IODELAY();
        *data = IoRead8(SMBUS_ADDRESS+HST_D0);
    
        return EFI_SUCCESS;
}

STATIC EFI_STATUS _SmbusWriteByte(UINT8 addr, UINT8 offset, UINT8 data)
{
        UINT8   iTemp;
        UINT32  iTimeout;

        // 1.  Check host busy
        iTimeout = TIMEOUT_COUNT;
        while( iTimeout && ( IoRead8(SMBUS_ADDRESS+HST_STS) & BIT0 ) ) {
                IODELAY();
                iTimeout--;
        }

        if ( 0 == iTimeout )    // If the host always busy, return
                return EFI_TIMEOUT;

        // 2.  Clear the status register
        IODELAY();
        IoWrite8(SMBUS_ADDRESS+HST_STS, 0xff);
    

        // 3.  Set the slave device address
        IODELAY();
        IoWrite8(SMBUS_ADDRESS+HST_SLVA, addr);
    

        // 4. Set the offset byte of the slave device
        IODELAY();
        IoWrite8(SMBUS_ADDRESS+HST_CMD, offset);

        // 5. Set the write byte
        IODELAY();
        IoWrite8(SMBUS_ADDRESS+HST_D0, data);

        // 5. Fire
        IODELAY();
        IoWrite8(SMBUS_ADDRESS+HST_CNT, 0x48);  // Byte

        // 6. Wait the host
        IODELAY();
        iTimeout = TIMEOUT_COUNT;
        do {
                iTemp = IoRead8(SMBUS_ADDRESS);
                IODELAY();
                //printf("[sw]temp=0x%x", temp);
                if ( iTemp & BIT2 )     // DEV_ERR
                        return EFI_DEVICE_ERROR;

                if ( 0 == (iTemp & BIT0) )      // host busy?
                break;

                iTimeout--;
        } while (iTimeout);

        if ( 0 == iTimeout )    // If the host always busy, return
                return EFI_TIMEOUT;
        
        // 7. Clear the status register
        IODELAY();
        IoWrite8(SMBUS_ADDRESS, iTemp);

        return EFI_SUCCESS;
}

#define _SMC_CPLD_GET_SMBUS_ADDR(A)     ((A>>8)&0xff)
#define _SMC_CPLD_GET_SMBUS_OFFSET(A)   (A&0xff)

STATIC EFI_STATUS SmbusReadByte(UINT16 addr_offset, UINT8* data)
{
    return _SmbusReadByte(
                        (UINT8)_SMC_CPLD_GET_SMBUS_ADDR(addr_offset), 
                        (UINT8)_SMC_CPLD_GET_SMBUS_OFFSET(addr_offset),
                        data);
}

STATIC EFI_STATUS SmbusWriteByte(UINT16 addr_offset, UINT8 data)
{
    return _SmbusWriteByte(
                        (UINT8)_SMC_CPLD_GET_SMBUS_ADDR(addr_offset), 
                        (UINT8)_SMC_CPLD_GET_SMBUS_OFFSET(addr_offset),
                        data);
}

STATIC VOID GetTimerValue(UINT16 WDTSec, UINT8* byte_5e, UINT8* byte_60, UINT8* byte_61)
{
        UINT8 bitlshift = 16;
//      UINT16 temp = 1;
        
        if ( WDTSec > 0 && WDTSec <= 2 ) {
                *byte_5e = 1<<3;
                bitlshift -= 1;
        }
        else if ( WDTSec > 2 && WDTSec <= 16 ) {
                *byte_5e = 2<<3;
                bitlshift -= 4;
        }
        else if ( WDTSec > 16 && WDTSec <= 128 ) {
                *byte_5e = 3<<3;
                bitlshift -= 7;
        }
        else if ( WDTSec > 128 && WDTSec <= 512 ) {
                *byte_5e = 4<<3;
                bitlshift -= 9;
        }
        else if ( WDTSec > 512 && WDTSec <= 2048 ) {
                *byte_5e = 5<<3;
                bitlshift -= 11;
        }
        else {
                *byte_5e = 0;
                *byte_60 = *byte_61 = 0xff;
                return;
        }

        WDTSec = WDTSec << bitlshift;
        WDTSec--;
        *byte_60 = (UINT8)(WDTSec & 0xff);
        *byte_61 = (UINT8)((WDTSec>>8) & 0xff);
}

void showHead()
{
    printf("--------------------------------\n");
    printf(" Supermicro PLD Tool ");
    printf(" Version: %s\n", VERSION);
    printf(" leonx@supermicro.com\n");
    printf("--------------------------------\n");
}

void ShowUsage()
{
    showHead();
    printf("Usage:\n");
    printf("cpld2.exe /wdt xxx(Seconds)\n");
    printf("  Set PLD watchdog timer. Range from 1 to 2048. 0 means disable watchdog. \n");
    printf("cpld2.exe /pw\n");
    printf("  Show PLD watchdog registers(Top counter and current counter). \n");
    printf("cpld2.exe /nmi\n");
    printf("  Set action of WDT timeout to NMI.\n");
    printf("cpld2.exe /rst\n");
    printf("  Set action of WDT timeout to RESET.\n");
    printf("cpld2.exe /adr 1(0)\n");
    printf("  Enable or disable CPLD ADR.\n");
    printf("cpld2.exe /s [caterrstop]\n");
    printf("  Show the last boot status.\n");
    printf("cpld2.exe /c\n");
    printf("  Clear the last boot status\n");
    printf("cpld2.exe /rstadr 1(0)\n");
    printf("  Enable or disable reset button trigger ADR.\n");
    printf("cpld2.exe /X11EXTRST\n");
    printf("  Reset BMC by PIN EXTRST#. And VGA & COM should not be reseted.\n");
}

EFI_STATUS InitMEXPSmbus()
{
/*
    IoWrite32(0xcf8, MEXP_SMBUS_PCI);
    if ( IoRead32(0xcfc) != SMBUS_DIDVID ) {
        printf("[!!!ERROR!!!]Can not find SMBUS DEVICE.");
        return EFI_DEVICE_ERROR;
    }
*/
    IoWrite32(0xcf8, MEXP_SMBUS_PCI+0x20);
    SMBUS_ADDRESS = (UINT16)(IoRead32(0xcfc) & (~BIT0));

#if OEM_DEBUG
    printf("SMBUS_ADDRESS=0x%x\r\n", SMBUS_ADDRESS);
#endif

    IoWrite32(0xcf8, MEXP_SMBUS_PCI+0x40);   // 00:1F.4.40
    IoWrite32(0xcfc, IoRead32(0xcfc) & (~(BIT1+BIT2)) | BIT0);

    return EFI_SUCCESS;
}

int ClearLastStatus()
{
    UINT8 gpo0 =0;
    EFI_STATUS Status;
    
    Status = SmbusReadByte(X86_GPO0, &gpo0);
    if ( Status != EFI_SUCCESS )
        return 1;
        
    Status = SmbusWriteByte(X86_GPO0, gpo0 | BIT7);
    if ( Status != EFI_SUCCESS )
        return 1;

    Status = SmbusWriteByte(X86_GPO0, gpo0);
    if ( Status != EFI_SUCCESS )
        return 1;

    printf("Clear done\n");
    return 0;
}

int SetNMIRST(UINT16 iType)
{
    UINT8 data = 0;
    EFI_STATUS Status;
    
    Status = SmbusReadByte(X86_GPO0, &data);
    if ( Status != EFI_SUCCESS ) {
        printf("Set NMI/REST failed.\n");
        return 1;
    }

    data &= ~3;
    
    switch(iType) {
        case EFI_NMI:
            data |= BIT1;
            break;
        default:
            data |= BIT0;
            break;
    }
    Status = SmbusWriteByte(X86_GPO0, data);
    if ( Status != EFI_SUCCESS ) {
        printf("Set NMI/REST failed.\n");
        return 1;
    }

    printf("Set to %s\n", iType==EFI_NMI?"NMI":"Rest");
    return 0;
}

int SetWDT(UINT16 sec)
{
    UINT8   byte_5e, byte_60, byte_61, data, rstnmibits;

    // 1. Pause the Watchdog
    SmbusWriteByte(TCCR2, 0x01);

    if ( sec == 0 ) {
        return 0;
    }

    // 2. Set to ? min
    GetTimerValue(sec, &byte_5e, &byte_60, &byte_61);
    SmbusWriteByte(TCCR0, byte_5e);    // Set 512s
    SmbusWriteByte(TCTOPSET0, byte_60);    // Set 9600
    SmbusWriteByte(TCTOPSET1, byte_61);

	// 3. Reset rst/nmi bits
    SmbusReadByte(X86_GPO0, &data);
    rstnmibits = data & 3;
    data &= ~3;
    SmbusWriteByte(X86_GPO0, data);
    data |= rstnmibits;
    SmbusWriteByte(X86_GPO0, data);

    // 4. Start the counter   
    SmbusWriteByte(TCCR2, 0x02);
   
    return 0;
}

int ShowLastStatus(UINT8 caterrstop)
{
    UINT8   gpo0 =0, gpo2 = 0, gpo3 = 0;
    EFI_STATUS Status;

    Status = SmbusReadByte(X86_GPO0, &gpo0);
    if ( Status != EFI_SUCCESS )
        return 1;
        
    Status = SmbusReadByte(X86_GPI2, &gpo2);
    if ( Status != EFI_SUCCESS )
        return 1;
        
    Status = SmbusReadByte(X86_GPI3, &gpo3);
    if ( Status != EFI_SUCCESS )
        return 1;

    // HW Reboot Reason: +adr -wdt_reset -bmc_reset -reset_button +bmc_power -power_button +catastrophic_err -therm_trip

    IoWrite8(0x70, 0x04);           // Hour
    printf("[%x:", IoRead8(0x71));
    IoWrite8(0x70, 0x02);           // Minute
    printf("%x:", IoRead8(0x71));
    IoWrite8(0x70, 0x00);           // Second
    printf("%x]", IoRead8(0x71));
   
    printf("\nHW Reboot Reason: ");
    gpo2 & BIT6 ? printf("+") : printf("-"); printf("wdt_reset ");
    gpo2 & BIT5 ? printf("+") : printf("-"); printf("bmc_reset ");
    gpo2 & BIT4 ? printf("+") : printf("-"); printf("reset_button ");
    gpo2 & BIT1 ? printf("+") : printf("-"); printf("bmc_power ");
    gpo2 & BIT0 ? printf("+") : printf("-"); printf("power_button ");

    gpo3 & BIT1 ? printf("+") : printf("-"); printf("catastrophic_err ");
    gpo3 & BIT0 ? printf("+") : printf("-"); printf("therm_trip ");
    printf("\n");

    printf("\nPLD Settings:\n");
    gpo0 & BIT5 ? printf("+") : printf("-"); printf("ADR Function\n");
    gpo0 & BIT3 ? printf("+") : printf("-"); printf("Reset Button Trigger ADR\n");
    gpo0 & BIT1 ? printf("+") : printf("-"); printf("WDT timeout to NMI\n");
    gpo0 & BIT0 ? printf("+") : printf("-"); printf("WDT timeout to Reset\n");
    

    if ( caterrstop && (gpo3 & BIT1) ) {
        EFI_DEADLOOP();
    }
    
    return 0;
}

int do_adr(UINT32 a)
{
    UINT8   gpo0 =0;
    EFI_STATUS Status;
    
    Status = SmbusReadByte(X86_GPO0, &gpo0);
    if ( Status != EFI_SUCCESS )
        return 1;

    if ( a ) {
        gpo0 |= BIT5;
    }
    else {
        gpo0 &= ~BIT5;
    }
 
    Status = SmbusWriteByte(X86_GPO0, gpo0);
    if ( Status != EFI_SUCCESS )
        return 1;

    return 0;
}

int do_rstadr(UINT32 a)
{
    UINT8   gpo0 =0;
    EFI_STATUS Status;
    
    Status = SmbusReadByte(X86_GPO0, &gpo0);
    if ( Status != EFI_SUCCESS )
        return 1;

    if ( a ) {
        gpo0 |= BIT3;
    }
    else {
        gpo0 &= ~BIT3;
    }
 
    Status = SmbusWriteByte(X86_GPO0, gpo0);
    if ( Status != EFI_SUCCESS )
        return 1;

    return 0;
}

void PrintWDTRegister()
{
    UINT8   byte_5e, byte_60, byte_61, byte_65, byte_66;

    SmbusReadByte(TCCR0, &byte_5e);
    SmbusReadByte(TCTOPSET0, &byte_60);
    SmbusReadByte(TCTOPSET1, &byte_61);
    SmbusReadByte(TCCNT0, &byte_65);
    SmbusReadByte(TCCNT1, &byte_66);

    printf("0x5e=0x%x, Top Counter=0x%02x%02x, Current Counter=0x%02x%02x\r", byte_5e, byte_61, byte_60, byte_66, byte_65);
}

int huynh()
{
    UINT8   byte_5e, byte_60, byte_61;

    PrintWDTRegister();
    
    SmbusReadByte(TCCR0, &byte_5e);    
    SmbusReadByte(TCTOPSET0, &byte_60);
    SmbusReadByte(TCTOPSET1, &byte_61);

    
    if ( byte_5e == 0x20 && byte_60 == 0xff && byte_61 == 0x95 ) {
        printf("The WDT settings is correct. Reset it to 2s.\r");
        SetWDT(2);
    }
    else {
        printf("The WDT settings is not correct. STOP!!!.\r");
        SetWDT(0);
    }

    return 0;
}

void CheckX11EXTRST()
{
    UINT8 gpo0;

    SmbusReadByte(X86_GPO0, &gpo0);
    gpo0 |= BIT4;
    SmbusWriteByte(X86_GPO0, gpo0);
    SmbusReadByte(X86_GPO0, &gpo0);
    gpo0 &= ~BIT4;
    SmbusWriteByte(X86_GPO0, gpo0);
}

void CheckTegile()
{
    EFI_STATUS Status;
    
    volatile UINT32* pADREn = (UINT32*)0xfed1f3f0;
    UINT32  iADREn;
    UINT32  ADR_FEAT_EN, ADR_GPIO_SEL, ADR_GPIO_RST_EN;
    UINT8 gpo0;

    printf("pADREn = 0x%x\n", pADREn);

    iADREn = *pADREn;
    ADR_FEAT_EN = iADREn & 1;
    ADR_GPIO_SEL = ( iADREn >> 30 ) & 0x3;
    ADR_GPIO_RST_EN = ( iADREn >> 29 ) & 1;

    printf("ADREn = 0x%x\n", iADREn);
    printf("ADR_FEAT_EN = 0x%x\n", ADR_FEAT_EN);
    printf("ADR_GPIO_SEL = 0x%x\n", ADR_GPIO_SEL);
    printf("ADR_GPIO_RST_EN = 0x%x\n", ADR_GPIO_RST_EN);

    if ( ADR_FEAT_EN == 0 ) {
        printf("ADR_FEAT_EN is 0, deadloop\n");
        EFI_DEADLOOP();
    }

    Status = SmbusReadByte(X86_GPO0, &gpo0);
    if ( Status != EFI_SUCCESS ) {
        printf("SMBUS access error. deadloop\n");
        EFI_DEADLOOP();
    }

    if ( gpo0 & BIT5 ) {
        printf("CPLD ADR is enabled.\n");
    }
    else {
        EFI_DEADLOOP();
    }

    iADREn = 0xfffe;
    while(iADREn) {
        IODELAY();
        iADREn --;
    }

    printf("call int 19h.\n");
    __asm {
        int 0x19;
    }
}

int main(int argc, char *argv[])
{
    UINT8* type = NULL;
    UINT8* str = NULL;
    UINT8* str1 = NULL;
    //UINT8* smbus_base = NULL;
    UINT16 iSec;
    UINT32 Status;
    
#if OEM_DEBUG
    printf("UINT8=0x%x\n", sizeof(UINT8));
    printf("UINT16=0x%x\n", sizeof(UINT16));
    printf("UINT32=0x%x\n", sizeof(UINT32));
#endif

    // Get the parameters
    switch(argc)
    {
        case 2:
            type = argv[1];
            break;
        case 3:
            type = argv[1];
            str = argv[2];
            break;
        case 4:
            type = argv[1];
            str = argv[2];
            str1 = argv[3];
            break;
        default:
            ShowUsage();
            return 1;
    }


    if ( InitMEXPSmbus() != EFI_SUCCESS ) {
        return 1;
    }


    if ( !strcasecmp(type, "/wdt") ) {
        if ( str == NULL ) {
            printf("The parameters is not correct.\n");
            ShowUsage();
            return 1;
        }

        iSec = atoi(str);

 #if OEM_DEBUG
        printf("str=%s\n", str);
        printf("i=%d\n", iSec);
 #endif
  
        Status = SetWDT(iSec);
    }
    else if ( !strcasecmp(type, "/s") ) {
        if ( !strcasecmp(str, "caterrstop") ) {
            Status = ShowLastStatus(1);
        }
        else {
            Status = ShowLastStatus(0);
        }
    }
    else if ( !strcasecmp(type, "/dw") ) {
        if ( !strcasecmp(str, "caterrstop") ) {
            Status = ShowLastStatus(1);
        }
        else {
            Status = ShowLastStatus(0);
        }
    }
    else if ( !strcasecmp(type, "/c") ) {
        Status = ClearLastStatus();
    }
    else if ( !strcasecmp(type, "/adr") ) {
        if ( str == NULL ) {
            printf("The parameters is not correct.\n");
            ShowUsage();
            return 1;
        }
        iSec = atoi(str);
        Status = do_adr(iSec);
    }
    else if ( !strcasecmp(type, "/pw") ) {
       PrintWDTRegister();
    }
    else if ( !strcasecmp(type, "/huynh") ) {
       return huynh();
    }
    else if ( !strcasecmp(type, "/rstadr") ) {
        if ( str == NULL ) {
            printf("The parameters is not correct.\n");
            ShowUsage();
            return 1;
        }
        iSec = atoi(str);
        Status = do_rstadr(iSec);
    }
    else if ( !strcasecmp(type, "/rst") ) {
       Status = SetNMIRST(EFI_RST);
    }
    else if ( !strcasecmp(type, "/nmi") ) {
       Status = SetNMIRST(EFI_NMI);
    }
    else if ( !strcasecmp(type, "/tegile")) {
        CheckTegile();
    }
    else if ( !strcasecmp(type, "/X11EXTRST")) {
        CheckX11EXTRST();
    }
    else {
        printf("The parameters is not correct.\n");
        ShowUsage();
        return 1;    
    }

    return Status;
}
