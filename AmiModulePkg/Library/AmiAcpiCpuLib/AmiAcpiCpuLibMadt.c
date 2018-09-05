#include <Token.h>
#include <AmiHobs.h>
#include <IndustryStandard/Acpi40.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>

EFI_GUID gCpuInfoHobGuid = AMI_CPUINFO_HOB_GUID;
CPUINFO_HOB *gCpuInfoHob = NULL;

#include <Protocol/AmiCpuInfo2.h>
AMI_CPU_INFO_2_PROTOCOL* gAmiCpu2Info = NULL;
EFI_GUID gAmiCpuInfo2ProtocolGuid = AMI_CPU_INFO_2_PROTOCOL_GUID;

static VOID CreateLxApicEntry(
    IN OUT UINT8 **MadtEntry,
    IN UINT8 ProcId,
    IN UINT8 ApicId,
    IN UINT32 Flags
)
{
     EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC_STRUCTURE	*xApicEntry;
     xApicEntry = (EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC_STRUCTURE*)*MadtEntry;
     xApicEntry->Type = EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC;
     xApicEntry->Length = sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC_STRUCTURE);
     xApicEntry->AcpiProcessorId = ProcId;
     xApicEntry->ApicId = ApicId;
     xApicEntry->Flags = Flags;
    *MadtEntry += sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC_STRUCTURE);
}

static VOID CreateLxApicNmiEntry(
    IN OUT UINT8 **MadtEntry,
    IN UINT8 ProcId,
    IN UINT16 Flags,
    IN UINT8 Lint
)
{
    EFI_ACPI_4_0_LOCAL_APIC_NMI_STRUCTURE			*xApicNmiEntry;
    xApicNmiEntry = (EFI_ACPI_4_0_LOCAL_APIC_NMI_STRUCTURE*)*MadtEntry;
    xApicNmiEntry->Type = EFI_ACPI_4_0_LOCAL_APIC_NMI;
    xApicNmiEntry->Length =sizeof(EFI_ACPI_4_0_LOCAL_APIC_NMI_STRUCTURE);
    xApicNmiEntry->AcpiProcessorId = ProcId;
    xApicNmiEntry->Flags = Flags;
    xApicNmiEntry->LocalApicLint =Lint;
    *MadtEntry += sizeof(EFI_ACPI_4_0_LOCAL_APIC_NMI_STRUCTURE);
}

static VOID CreateLSApicEntry(
    IN OUT UINT8 **MadtEntry,
    IN UINT8 ProcId,
    IN UINT8 SapicId,
    IN UINT8 SapicEid,
    IN UINT32 Flags,
    IN UINT32 ProcUid
)
{
    //Note: ACPI VER 1 & 2 doesn't have ACPIProcessorUIDValue.
    EFI_ACPI_4_0_PROCESSOR_LOCAL_SAPIC_STRUCTURE	*SapicEntry;
    SapicEntry = (EFI_ACPI_4_0_PROCESSOR_LOCAL_SAPIC_STRUCTURE*)*MadtEntry;
    SapicEntry->Type = EFI_ACPI_4_0_LOCAL_SAPIC;
    SapicEntry->Length = sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_SAPIC_STRUCTURE);
    SapicEntry->AcpiProcessorId = ProcId;
    SapicEntry->LocalSapicId = SapicId;
    SapicEntry->LocalSapicEid = SapicEid;
    SapicEntry->Reserved[0] = 0;
    SapicEntry->Reserved[1] = 0;
    SapicEntry->Reserved[2] = 0;
    SapicEntry->Flags = Flags;
    SapicEntry->ACPIProcessorUIDValue = ProcUid;
    *MadtEntry += sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_SAPIC_STRUCTURE);
}


static VOID CreateLx2ApicEntry(
    IN OUT UINT8 **MadtEntry,
    IN UINT32  ProcId,
    IN UINT32 ApicId,
    IN UINT32 Flags
)
{
EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE 	*x2ApicEntry;

    x2ApicEntry = (EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE*)*MadtEntry;
    x2ApicEntry->Type = EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC;
    x2ApicEntry->Length = sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE);
    x2ApicEntry->Reserved[0] = 0;
    x2ApicEntry->Reserved[1] = 0;
    x2ApicEntry->X2ApicId = ApicId;
    x2ApicEntry->Flags = Flags;
    x2ApicEntry->AcpiProcessorUid = ProcId;
    *MadtEntry+=sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE);
}

static VOID CreateLx2ApicNmiEntry(
    IN OUT UINT8 **MadtEntry,
    IN UINT32 ProcId,
    IN UINT16 Flags,
    IN UINT8 Lint
)
{
    EFI_ACPI_4_0_LOCAL_X2APIC_NMI_STRUCTURE		*x2ApicNmiEntry;

    x2ApicNmiEntry = (EFI_ACPI_4_0_LOCAL_X2APIC_NMI_STRUCTURE*)*MadtEntry;
    x2ApicNmiEntry->Type = EFI_ACPI_4_0_LOCAL_X2APIC_NMI;
    x2ApicNmiEntry->Length = sizeof(EFI_ACPI_4_0_LOCAL_X2APIC_NMI_STRUCTURE);
    x2ApicNmiEntry->Flags = Flags;
    x2ApicNmiEntry->AcpiProcessorUid = ProcId;
    x2ApicNmiEntry->LocalX2ApicLint = Lint;
    x2ApicNmiEntry->Reserved[0] = 0;
    x2ApicNmiEntry->Reserved[1] = 0;
    x2ApicNmiEntry->Reserved[2] = 0;
    *MadtEntry += sizeof(EFI_ACPI_4_0_LOCAL_X2APIC_NMI_STRUCTURE);
}

//This function creates the MADT entries to match the Aptio 4 Nehalem module.
static EFI_STATUS BuildMadtByProtocol(IN UINT32 TableVer, OUT VOID **Entries, OUT UINT32 *NumEntries, OUT UINT32 *TableSize)
{
    ACPI_PROCESSOR_INFO *AcpiProcInfo;
    EFI_STATUS  Status;
    UINT32      Index;
    UINT32      NumProtEntries;
    UINT8       *MadtEntry;

    *TableSize = 0;

    //Use AMI_CPU_INFO_2_PROTOCOL if published.
    if (gAmiCpu2Info == NULL) {
        Status = gBS->LocateProtocol(&gAmiCpuInfo2ProtocolGuid, NULL, &gAmiCpu2Info);
        if (EFI_ERROR(Status)) return Status;
    }

    if (gAmiCpu2Info->ProtocolVer < 3) return EFI_UNSUPPORTED;

    Status = gAmiCpu2Info->GetAcpiInfo(
        gAmiCpu2Info, (VOID**) &AcpiProcInfo, &NumProtEntries
    );
    if (EFI_ERROR(Status)) return Status;

    //Calculate size needed for CPU entries. Each CPU will have an Local APIC and NMI entry, when using the the protocol.
    for (Index = 0; Index < NumProtEntries; ++Index) {
        if (AcpiProcInfo[Index].LocalApicType == ACPI_PROC_INFO_x2APIC) {
            *TableSize +=
            sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE)
            + sizeof(EFI_ACPI_4_0_LOCAL_X2APIC_NMI_STRUCTURE);
        } else {
            *TableSize +=
            sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC_STRUCTURE)
            + sizeof(EFI_ACPI_4_0_LOCAL_APIC_NMI_STRUCTURE);
        }
    }

    MadtEntry = (UINT8*) AllocatePool(*TableSize);
    ASSERT(MadtEntry != NULL);

    *Entries = MadtEntry;
    *NumEntries = NumProtEntries * 2;    //Each CPU logical processor will have 2 entries: 1 APIC and 1 NMI.

    for (Index = 0; Index < NumProtEntries; ++Index) {
        if (AcpiProcInfo[Index].LocalApicType == ACPI_PROC_INFO_x2APIC) {
            //The function will updated MadtEntry for the next entry.
            CreateLx2ApicEntry(&MadtEntry, AcpiProcInfo[Index].ProcId, AcpiProcInfo[Index].ApicId, EFI_ACPI_4_0_LOCAL_APIC_ENABLED);
            CreateLx2ApicNmiEntry(&MadtEntry, AcpiProcInfo[Index].ProcId, AcpiProcInfo[Index].NmiFlags, AcpiProcInfo[Index].LintnPin);
        } else {
            CreateLxApicEntry(&MadtEntry, AcpiProcInfo[Index].ProcId, AcpiProcInfo[Index].ApicId, EFI_ACPI_4_0_LOCAL_APIC_ENABLED);
            CreateLxApicNmiEntry(&MadtEntry, AcpiProcInfo[Index].ProcId, AcpiProcInfo[Index].NmiFlags, AcpiProcInfo[Index].LintnPin);
        }
    }

    ASSERT((MadtEntry - (UINT8*)*Entries) == *TableSize);

    return EFI_SUCCESS;
}

typedef struct {
    UINT8 CpuNum;
    UINT8 ApicId;
} CPU_NUM_APICID;

//This function creates the MADT entries to match the Aptio 4 CPU modules other than Nehalem.
static EFI_STATUS BuildMadtByHob(IN UINT32 TableVer, OUT VOID **Entries, OUT UINT32 *NumEntries, OUT UINT32 *TableSize)
{
    UINT32      PopulatedCpus;
    BOOLEAN     LocalXapicSupport;
    UINT8       *MadtEntry;
    UINT8       *Table;
    CPU_NUM_APICID  *CpuNumApicId;
    UINT32      Index;

    *TableSize = 0;

    if (gCpuInfoHob == NULL) {
        gCpuInfoHob = (CPUINFO_HOB*) GetFirstGuidHob (&gCpuInfoHobGuid);
        if (gCpuInfoHob == NULL) return EFI_NOT_FOUND;
    }

    PopulatedCpus = gCpuInfoHob->CpuCount;

    //There may be one entry for each CPU or maximum number of CPUs supported by the platform depending on SDL token.
#if SKIP_DUMMY_LAPICS == 0 && NCPU > 0
    *NumEntries = NCPU; // NCPU - maximum processors supported by platform.
#else
    *NumEntries = PopulatedCpus;
#endif

    LocalXapicSupport = gCpuInfoHob->Cpuinfo[0].ApicVer < LOCAL_APIC_VERSION_PARAMETER;

    //Create Table of Apic IDs.
    CpuNumApicId = (CPU_NUM_APICID*) AllocatePool(sizeof(CPU_NUM_APICID) * PopulatedCpus);
    ASSERT(CpuNumApicId != NULL);

    for (Index = 0; Index < PopulatedCpus; ++Index) {
        CpuNumApicId[Index].CpuNum = Index;
        CpuNumApicId[Index].ApicId = gCpuInfoHob->Cpuinfo[Index].ApicId;
    }

    //Sort entries. Maybe pre-sorted, thus no sort done.
    for (Index = 0; Index < PopulatedCpus - 1; ++Index) {
        UINT32 Index2;
        BOOLEAN Sorted = TRUE;
        for (Index2 = 0; Index2 < PopulatedCpus - 1; ++Index2) {
            if (CpuNumApicId[Index2].ApicId > CpuNumApicId[Index2 + 1].ApicId) {
                CPU_NUM_APICID Temp;
                Temp.CpuNum = CpuNumApicId[Index2].CpuNum;
                Temp.ApicId = CpuNumApicId[Index2].ApicId;
                CpuNumApicId[Index2].CpuNum = CpuNumApicId[Index2+1].CpuNum;
                CpuNumApicId[Index2].ApicId = CpuNumApicId[Index2+1].ApicId;
                CpuNumApicId[Index2+1].CpuNum = Temp.CpuNum;
                CpuNumApicId[Index2+1].ApicId = Temp.ApicId;
                Sorted = FALSE;
            }
        }
        if (Sorted) break;
    }

    //Calculate Table Size.
    *TableSize = *NumEntries *
        (LocalXapicSupport ? sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC_STRUCTURE) : sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_SAPIC_STRUCTURE));

    //Calculate NMI Entries.
    //For each system, there is usually one NMI entry.
    #if (LAPIC_QUANTITY > 0)
            if (LAPIC_0_INT_TYPE == 1)    {// This is NMI - so build a table for it
                ++*NumEntries;
                *TableSize += sizeof(EFI_ACPI_4_0_LOCAL_APIC_NMI_STRUCTURE);
            }
    #endif
    #if (LAPIC_QUANTITY > 1)
            if (LAPIC_1_INT_TYPE == 1)  {    // This is NMI - so build a table for it
                ++*NumEntries;
                *TableSize += sizeof(EFI_ACPI_4_0_LOCAL_APIC_NMI_STRUCTURE);
            }
    #endif
    #if (LAPIC_QUANTITY > 2)
            if (LAPIC_2_INT_TYPE == 1)  { // This is NMI - so build a table for it
                ++*NumEntries;
                *TableSize += sizeof(EFI_ACPI_4_0_LOCAL_APIC_NMI_STRUCTURE);
            }
    #endif
	    //Update code , if LAPIC_QUANTITY > 3 - Add more structures

    	Table = (UINT8*) AllocatePool(*TableSize);
        ASSERT(Table != NULL);

        *Entries = Table;

    MadtEntry = Table;
    for (Index = 0; Index < PopulatedCpus; ++Index) {
        UINT32 CpuNum = CpuNumApicId[Index].CpuNum;
        if (LocalXapicSupport) {
            //The function will updated MadtEntry for the next entry.
        	CreateLxApicEntry(
                    &MadtEntry,
                    CpuNum + 1,
                    gCpuInfoHob->Cpuinfo[CpuNum].ApicId,
                    gCpuInfoHob->Cpuinfo[CpuNum].Valid ? EFI_ACPI_4_0_LOCAL_APIC_ENABLED : 0
            );
        } else {
            CreateLSApicEntry(
                    &MadtEntry,
                    CpuNum + 1,
                    gCpuInfoHob->Cpuinfo[CpuNum].ApicId,
                    gCpuInfoHob->Cpuinfo[CpuNum].ApicEId,
                    gCpuInfoHob->Cpuinfo[CpuNum].Valid ? EFI_ACPI_4_0_LOCAL_APIC_ENABLED : 0,
                    0
            );
        }
    }
    FreePool(CpuNumApicId);

    //Create dummy entries
    for (; Index < NCPU; ++Index) {
        if (LocalXapicSupport) {
            CreateLxApicEntry(&MadtEntry, Index + 1, 0, 0);
        } else  {
            CreateLSApicEntry(&MadtEntry, Index + 1, 0, 0, 0, 0);
        }
    }

    //Build LAPIC NMI

#if (LAPIC_QUANTITY > 0)
    if (LAPIC_0_INT_TYPE == 1)   {  // This is NMI - so build a table for it

        CreateLxApicNmiEntry(
            &MadtEntry,
            0xff,
            (LAPIC_0_TRIGGER_MODE<<2) | LAPIC_0_POLARITY,
            LAPIC_0_DEST_LINTIN
        );
    }
#endif
#if (LAPIC_QUANTITY > 1)
    if (LAPIC_1_INT_TYPE == 1)   {  // This is NMI - so build a table for it
        CreateLxApicNmiEntry(
            &MadtEntry,
            0xff,
            (LAPIC_1_TRIGGER_MODE<<2) | LAPIC_1_POLARITY,
            LAPIC_1_DEST_LINTIN
       );
    }
#endif
#if (LAPIC_QUANTITY > 2)
    if (LAPIC_2_INT_TYPE == 1)   {  // This is NMI - so build a table for it
        CreateLxApicNmiEntry(
            &MadtEntry,
            0xff,
            (LAPIC_2_TRIGGER_MODE<<2) | LAPIC_2_POLARITY,
            LAPIC_2_DEST_LINTIN
        );
    }
#endif
//------Update code, if LAPIC_QUANTITY > 3 - Add more structures

    ASSERT((MadtEntry-Table) == *TableSize);

    return EFI_SUCCESS;
}

EFI_STATUS CreateCpuMadtEntries(IN UINT32 TableVer, OUT VOID **Entries, OUT UINT32 *NumEntries, OUT UINT32 *TableSize)
{
    EFI_STATUS Status;

    Status = BuildMadtByProtocol(TableVer, Entries, NumEntries, TableSize);
    if (!EFI_ERROR(Status)) return EFI_SUCCESS;
    Status =  BuildMadtByHob(TableVer, Entries, NumEntries, TableSize);
    return Status;
}

