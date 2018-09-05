//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include <TcgAslLib.h>
#include <Library/IoLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>

extern EFI_GUID gEfiAcpiSdtProtocolGuid;
extern EFI_GUID gEfiAcpiTableGuid;

VOID * TcgAslGetConfigurationTable(
    IN EFI_SYSTEM_TABLE *SystemTable,
    IN EFI_GUID         *Guid
)
{
    EFI_CONFIGURATION_TABLE *Table = SystemTable->ConfigurationTable;
    UINTN i = SystemTable->NumberOfTableEntries;

    for (; i; --i,++Table)
    {
        if (CompareMem(&Table->VendorGuid,Guid, sizeof(EFI_GUID))==0) return Table->VendorTable;
    }
    return 0;
}

EFI_STATUS TcgLibGetDsdt(EFI_PHYSICAL_ADDRESS *DsdtAddr, EFI_ACPI_TABLE_VERSION Version)
{
    static EFI_PHYSICAL_ADDRESS Dsdt1 = 0;
    static EFI_PHYSICAL_ADDRESS Dsdt2 = 0;

#if PI_SPECIFICATION_VERSION < 0x10014
    ACPI_HDR                    *tbl = NULL;
    EFI_ACPI_SDT_PROTOCOL       *as;
#else
    EFI_ACPI_SDT_HEADER         *tbl = NULL;
    EFI_ACPI_SDT_PROTOCOL       *as;
#endif
    UINT8                       *Rsdtptr=NULL;
    EFI_STATUS                  Status = EFI_NOT_FOUND;
    UINTN                       i;
    EFI_ACPI_TABLE_VERSION      ver;
    UINTN                       AcpiHandle;
    EFI_GUID                    gEfiAcpiSdtProtocolGuid = EFI_ACPI_SDT_PROTOCOL_GUID;

//---------------------------
    if (!(Dsdt1 && Dsdt2))
    {
        Status=gBS->LocateProtocol(&gEfiAcpiSdtProtocolGuid, NULL, &as);

        if (EFI_ERROR(Status))
        {
            DEBUG((DEBUG_ERROR,"AcpiResLib: LibGetDsdt(): LocateProtocol(ACPISupport) returned %r \n", Status));
            return EFI_NOT_AVAILABLE_YET;
        }
        else
        {
            Rsdtptr = TcgAslGetConfigurationTable(gST, &gEfiAcpiTableGuid);
            if(Rsdtptr == NULL) return EFI_NOT_AVAILABLE_YET;
            
            for (i=0; ; i++)
            {
                Status = as->GetAcpiTable(i, &tbl, &ver, &AcpiHandle);
                if (EFI_ERROR(Status))
                {
                    DEBUG((DEBUG_ERROR,"Can't find Dsdt table -> %r search %d Tables\n", Status, i));
                    break;
                }

                if (tbl->Signature == FACP_SIG)
                {
                    if (ver == EFI_ACPI_TABLE_VERSION_1_0B)
                    {
                        Dsdt1 = (EFI_PHYSICAL_ADDRESS)(((FACP_20 *)tbl)->DSDT);
                        DEBUG((DEBUG_INFO,"AcpiResLib: LibGetDsdt(): Found v1.0b   RSDT->DSDT @ 0x%lX; -> %r \n", Dsdt1, Status));
                    }

                    if ((ver & EFI_ACPI_TABLE_VERSION_X)!= 0)
                    {
                        Dsdt2 = (EFI_PHYSICAL_ADDRESS)(((FACP_20 *)tbl)->X_DSDT);
                        DEBUG((DEBUG_INFO,"AcpiResLib: LibGetDsdt(): Found v2.0&UP XSDT->DSDT @ 0x%lX; -> %r \n", Dsdt2, Status));
                    }
                }
                else
                {
                    if(tbl!=NULL){
                        gBS->FreePool((VOID *)tbl);
                    }
                }

                if (Dsdt1 && Dsdt2)
                    break;
            }
        }
    }

    if (Dsdt2 && ((Version & EFI_ACPI_TABLE_VERSION_X) || (Version & EFI_ACPI_TABLE_VERSION_NONE)))
    {
        *DsdtAddr = Dsdt2;
        Status = EFI_SUCCESS;
    }
    else
    {
        if (Dsdt1 && ((Version & EFI_ACPI_TABLE_VERSION_1_0B) || (Version & EFI_ACPI_TABLE_VERSION_NONE)))
        {
            *DsdtAddr = Dsdt1;
            Status = EFI_SUCCESS;
        }
    }
    return Status;
}



UINT32 ConvertAslName(UINT8 *AslName)
{
    AML_NAME_SEG    n;
    UINTN           l=AsciiStrLen(AslName);
//-------------------------
    n.NAME=0x5F5F5F5F;
    gBS->CopyMem(&n.Name[0],AslName,l);
    return n.NAME;
}

VOID* FindAslObjectName(UINT8 *Start, UINT8 *ObjName, UINTN Len)
{
    unsigned int    i;
    UINT8           *p;
    AML_NAME_SEG    *obj,nm;
//------------------------------
    nm.NAME=ConvertAslName(ObjName);

    p=Start;

    for (i=0; i<Len; i++)
    {
        obj=(AML_NAME_SEG*)(&p[i]);

        if (obj->NAME!=nm.NAME)continue;

        return &p[i];
    }

    return NULL;
}


UINTN GetPackageLen(AML_PACKAGE_LBYTE *Pck, UINT8 *BytesCnt)
{
    UINTN   len=0;
    UINT8   *p=(UINT8*)Pck;

//------------------
    if (BytesCnt)*BytesCnt=0;

    if (!Pck->ByteCount) len=Pck->Length;
    else
    {
        len|=(Pck->Length&0x0F);

        if (BytesCnt)*BytesCnt=Pck->ByteCount;

        switch (Pck->ByteCount)
        {
            case 1:
            {
                UINT8   b=*(p+1);
                len|=(b<<4);
            }
            break;
            case 2:
            {
                UINT16 w=(UINT16)(*((UINT16*)(p+1)));
                len|=(w<<4);
            }
            break;
            case 3:
            {
                UINT32 d=(UINT32)(*((UINT32*)(p+1)));
                d&=0xffffff;
                len|=(d<<4);
            }
            break;
        } //switch
    }

    return len;
}


BOOLEAN CheckOpCode(UINT8 *NameStart, UINT8 OpCode, UINT8 ExtOpCode, ASL_OBJ_INFO *ObjInfo)
{
    UINT8   *p, *p1;
    INTN    i;

//---------------------
    //Maximum number of bytes in PackageLength is 4
    if ( (*(NameStart-1))==AML_PR_ROOT || (*(NameStart-1))==AML_PR_PARENT) NameStart--;

    //if we Looking For ASL Name Object its a special case.
    if (OpCode==AML_OP_NAME && !ExtOpCode)
    {
        p=NameStart-1;

        if (*p==OpCode)
        {
            ObjInfo->Object=p;
            ObjInfo->ObjName=NameStart;
            ObjInfo->Length=0;
            return TRUE;
        }

        return FALSE;
    }

    //if we Looking For ASL Name Object its a special case.
    if (OpCode==AML_OP_OPREG && ExtOpCode)
    {
        p=NameStart-2;

        if (*p==ExtOpCode && *(p+1)==OpCode)
        {
            ObjInfo->Object=p;
            ObjInfo->ObjName=NameStart;
            ObjInfo->Length=0;
            return TRUE;
        }

        return FALSE;
    }

    for (i=2; i<6; i++)
    {
        p=NameStart-i;

        if (ExtOpCode) p1=p-1;
        else p1=NULL;

        if (p1)if (*p1!=ExtOpCode) continue;

        if (*p!=OpCode) continue;

        if (p1)ObjInfo->Object=p1;
        else ObjInfo->Object=p;

        ObjInfo->Length=GetPackageLen((AML_PACKAGE_LBYTE*)(p+1),NULL);
        ObjInfo->ObjName=NameStart;
        return TRUE;
    }

    return FALSE;
}



EFI_STATUS GetAslObj(UINT8 *Start, UINTN Length, UINT8 *Name, ASL_OBJ_TYPE ObjType, ASL_OBJ_INFO *ObjInfo)
{
    UINT8           *pn, *s, eop,op;
    UINTN           l, i;
    BOOLEAN         objfound=FALSE;
//  ASL_OBJ_INFO    objinf;
//---------------------------
    //if(!CheckDsdt(PDsdt)) return FALSE;

    //s=((UINT8*)PDsdt)+sizeof(ACPI_HDR);
    s=Start;
    //l=PDsdt->Length-sizeof(ACPI_HDR)-1;
    l=Length;

    while (!objfound)
    {

        pn=FindAslObjectName(s,Name,l);

        if (!pn) return EFI_NOT_FOUND;

        //If we found the name let's check if it is the Object we are looking for
        //it could be just object referance, or field definition inside the object,
        //or double name, or multiple name definition we must filter such situations
        // and make sure we have got the actual object but not its reference instance
        switch (ObjType)
        {
            case    otScope     :
                i=0;
                eop=0;
                op=AML_OP_SCOPE;
                break;
            case    otName      :
                i=0;
                eop=0;
                op=AML_OP_NAME;
                break;
            case    otProc      :
                i=6;
                eop=AML_PR_EXTOP;
                op=AML_OP_PROC;
                break;
            case    otTermal    :
                i=0;
                eop=AML_PR_EXTOP;
                op=AML_OP_THERMAL;
                break;
            case    otDevice    :
                i=0;
                eop=AML_PR_EXTOP;
                op=AML_OP_DEVICE;
                break;
            case    otMethod        :
                i=1;
                eop=0;
                op=AML_OP_METHOD;
                break;
            case    otPwrRes    :
                i=3;
                eop=AML_PR_EXTOP;
                op=AML_OP_PWRRES;
                break;
            case    otOpReg :
                i=0;
                eop=AML_PR_EXTOP;
                op=AML_OP_OPREG;
                break;

            default:
                return EFI_NOT_FOUND;
        } //switch

        objfound=CheckOpCode(pn, op, eop, ObjInfo);

        if (!objfound)
        {
            l-=(UINTN)(pn-s)-4;
            s=pn+4;
            continue;
        }

        ObjInfo->ObjType=ObjType;
        ObjInfo->DataStart=pn+i+4;

        return EFI_SUCCESS;
    } //while ()

    return EFI_NOT_FOUND;
}



EFI_STATUS  UpdateAslNameBuffer(UINT8 *BufferStart, UINTN BufferLen, UINT8 *ObjName, UINT64 Value)
{
    EFI_STATUS          Status;
    ASL_OBJ_INFO        obj;
//---------------
    Status=GetAslObj(BufferStart,BufferLen, ObjName, otName, &obj);

    if (EFI_ERROR(Status)) return Status;

    switch (*((UINT8*)obj.DataStart))
    {
        case AML_PR_BYTE:
        {
            UINT8   *p = (UINT8*)((UINT8*)obj.DataStart+1);
            //----------------------------------
            *p=(UINT8)Value;
        }
        break;

        case AML_PR_WORD:
        {
            UINT16  *p = (UINT16*)((UINT8*)obj.DataStart+1);
            //----------------------------------
            *p=(UINT16)Value;
        }
        break;

        case AML_PR_DWORD:
        {
            UINT32  *p = (UINT32*)((UINT8*)obj.DataStart+1);
            //----------------------------------
            *p=(UINT32)Value;
        }
        break;

        case AML_PR_QWORD:
        {
            UINT64  *p = (UINT64*)((UINT8*)obj.DataStart+1);
            //----------------------------------
            *p=(UINT64)Value;
        }
        break;

        case AML_ZERO_OP:
        case AML_ONE_OP:
        {
            if ((Value == 0) || (Value == 1))
            {
                UINT8 *p = (UINT8*)((UINT8*)obj.DataStart);
                *p = (UINT8)Value;
            }

            else
            {
                return EFI_INVALID_PARAMETER;
            }
        }
        break;
        default:
        {
            return EFI_INVALID_PARAMETER;
        }
    }

    return EFI_SUCCESS;
}

BOOLEAN CheckDsdt(ACPI_HDR *Dsdt)
{
    if (Dsdt->Signature!=DSDT_SIG) return FALSE;

    return TRUE;
}



EFI_STATUS  TcgUpdateAslNameObject(ACPI_HDR *PDsdt, UINT8 *ObjName, UINT64 Value)
{
//------------------------------------
    if (!CheckDsdt(PDsdt)) return EFI_INVALID_PARAMETER;

    return UpdateAslNameBuffer((UINT8*)(PDsdt+1),PDsdt->Length-sizeof(ACPI_HDR)-1, ObjName, Value);
}
