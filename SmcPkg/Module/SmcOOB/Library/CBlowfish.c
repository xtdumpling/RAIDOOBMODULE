//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Compress and encipher HII data for sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/25/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        CBlowfish.c
//
// Description: OOB library code.
//
//<AMI_FHDR_END>
//**********************************************************************

#include "EFI.h"
#include "Token.h"
#include <AmiLib.h>
#include "CBlowfish.h"
#include "CBlowfish.h2"
#include <Library/BaseLib.h>

UINT8 good;
UINT16 chunkSize;
UINT32 P[16 + 2];
UINT32 S[4][256];

VOID CBlowFishInit (CHAR8* Key)
{
    //stringstream errmsg;
    UINT32 i;
    UINT32 j;
    UINT32 k;
    //UINT32 KeyLen = SmcStrLen (Key);
    UINT32 ErrorFound = 0;
    UINT32 data, datal, datar;

    good = 1;

    if (Strlen (Key) > 54 || Strlen (Key) < 4) {
        //errmsg << "Key size should be 4~54 bytes. But we receive " << key.size() << " bytes." << endl;
        good = 0;
        ErrorFound++;
        return;
        //throw CInvalidCBlowFishParam(UtilityInternalError, errmsg.str(),"24.1",__FILE__,__func__,__LINE__);
    }

    chunkSize = sizeof(UINT32) * 2;

    for (i = 0; i < (UINT32)(N + 2); ++i) {
        P[i] = SEED_P[i];
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 256; ++j)
            S[i][j] = SEED_S[i][j];
    }

    j = 0;
    for (i = 0; i < (UINT32)(N + 2); ++i) {
        data = 0x00000000;
        for (k = 0; k < 4; ++k) {
            data = (data << 8) | Key[j];
            j = j + 1;
            if (j >= (INT32) Strlen (Key))
                j = 0;
        }
        P[i] = P[i] ^ data;
    }

    datal = 0x00000000;
    datar = 0x00000000;

    for (i = 0; i < (UINT32)(N + 2); i += 2) {
        CBlowFishEncipher(&datal, &datar);
        P[i] = datal;
        P[i + 1] = datar;
    }

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 256; j += 2) {
            CBlowFishEncipher(&datal, &datar);
            S[i][j] = datal;
            S[i][j + 1] = datar;
        }
    }
}

UINT32 F(unsigned int x) {
    UINT16 a, b, c, d;
    UINT32 y;

    d = x & 0x00FF;
    x >>= 8;
    c = x & 0x00FF;
    x >>= 8;
    b = x & 0x00FF;
    x >>= 8;
    a = x & 0x00FF;
    y = S[0][a] + S[1][b];
    y = y ^ S[2][c];
    y = y + S[3][d];

    return y;
}

VOID CBlowFishEncipher(UINT32 *xl, UINT32 *xr)
{
    UINT32 Xl;
    UINT32 Xr;
    UINT32 temp;
    UINT32 i;

    Xl = *xl;
    Xr = *xr;

    for (i = 0; i < N; ++i) {
        Xl = Xl ^ P[i];
        Xr = F(Xl) ^ Xr;

        temp = Xl;
        Xl = Xr;
        Xr = temp;
    }

    temp = Xl;
    Xl = Xr;
    Xr = temp;

    Xr = Xr ^ P[N];
    Xl = Xl ^ P[N + 1];

    *xl = Xl;
    *xr = Xr;
}

VOID CBlowFishDecipher(UINT32 *xl, UINT32 *xr)
{
	UINT32 Xl;
	UINT32 Xr;
	UINT32 temp;
    UINT32 i;

	Xl = *xl;
	Xr = *xr;

	for (i = N + 1; i > 1; --i) {
		Xl = Xl ^ P[i];
		Xr = F(Xl) ^ Xr;

		/* Exchange Xl and Xr */
		temp = Xl;
		Xl = Xr;
		Xr = temp;
	}

	/* Exchange Xl and Xr */
	temp = Xl;
	Xl = Xr;
	Xr = temp;

	Xr = Xr ^ P[1];
	Xl = Xl ^ P[0];

	*xl = Xl;
	*xr = Xr;
}

UINT8 CBlowFishEncipherCharArray(UINT8* buffer, UINT32 BufferSize, UINT32 bSize)
{
    UINT32 i;
	UINT32 *L;
	UINT32 *R;
    UINT32 ErrorFound = 0;
	//stringstream errmsg;

	if (bSize == 0) {
		//errmsg << "Data size [" << bSize <<"] should be positive." << endl;
		//throw CInvalidCBlowFishParam(UtilityInternalError,errmsg.str(),"27.1",__FILE__,__func__,__LINE__);
        ErrorFound++;
        return 0;
	}

	if (bSize > BufferSize) {
		//errmsg << "Buffer size ["  << buffer.size() <<"] should not be less than data size [" << bSize << "]." << endl;
		//throw CInvalidCBlowFishParam(UtilityInternalError,errmsg.str(),"27.2",__FILE__,__func__,__LINE__);
        ErrorFound++;
        return 0;
	}

	if (bSize % chunkSize != 0) {
		//errmsg << "Data size error. Data size [" << bSize << "] is not 0Mod" << chunkSize << endl;
		//throw CInvalidCBlowFishParam(UtilityInternalError,errmsg.str(),"27.3",__FILE__,__func__,__LINE__);
        ErrorFound++;
        return 0;
	}

	for (i = 0; i < bSize / chunkSize; i++) {
		L = (UINT32*)&(buffer[i * chunkSize]);
		R = (UINT32*)&(buffer[i * chunkSize + chunkSize / 2]);
		CBlowFishEncipher(L, R);
	}

	return 1;
}

UINT8 CBlowFishDecipherChararray(UINT8* buffer, UINT32 BufferSize, UINT32 bSize)
{
    UINT32 i;
	UINT32 *L;
	UINT32 *R;
    UINT32 ErrorFound = 0;

	//stringstream errmsg;
	if (bSize == 0) {
		//errmsg << "Data size [" << bSize <<"] should be positive." << endl;
		//throw CInvalidCBlowFishParam(UtilityInternalError,errmsg.str(),"28.1",__FILE__,__func__,__LINE__);
        ErrorFound++;
        return 0;
	}

	if (bSize > BufferSize) {
		//errmsg << "Buffer size ["  << buffer.size() <<"] should not be less than data size [" << bSize << "]." << endl;
		//throw CInvalidCBlowFishParam(UtilityInternalError,errmsg.str(),"28.2",__FILE__,__func__,__LINE__);
        ErrorFound++;
        return 0;
	}

	if (bSize % chunkSize != 0) {
		//errmsg << "Data size error. Data size [" << bSize << "] is not 0Mod" << chunkSize << endl;
		//throw CInvalidCBlowFishParam(UtilityInternalError,errmsg.str(),"28.3",__FILE__,__func__,__LINE__);
        ErrorFound++;
        return 0;
	}

	for (i = 0; i < bSize / chunkSize; i++) {
		L = (UINT32*)&(buffer[i * chunkSize]);
		R = (UINT32*)&(buffer[i * chunkSize + chunkSize / 2]);
		CBlowFishDecipher(L, R);
	}

	return 1;
}


