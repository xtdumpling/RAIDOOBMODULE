//****************************************************************************
//**            (C)Copyright 1993-2014 Supermicro Computer, Inc.            **
//****************************************************************************
//  File History
//
//    Rev.  1.01
//     Bug Fix:     Including Adaptec devices into workaround
//     Reason:      Fixed Adaptec secondary controller is not initialized
//     Auditor:     Stephen Chen
//     Date:        May/08/2014
//
//    Rev.  1.00
//     Bug Fix:     More than one AOC-STG-B4S (Broadcom LAN), system will hang on POST code 0xB2.
//     Reason:      Shadow ram is not enough to shadow all oproms.
//     Auditor:     Kasber Chen
//     Date:        08/16/2013
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

#include <AmiDxeLib.h>
#include <Pci.h>
#include <Protocol\PciIo.h>
#include <Token.h>	//SMC R1.00

#define LSI_PCI_VENDOR_ID   0x1000
#define ADAPTEC_PCI_VENDOR_ID   0x9005

//SMC R1.00 S
typedef struct {
   UINTN        Seg;
   UINTN        Bus;
   UINTN        Dev;
   UINTN        Fun;
   UINTN        Flags;
   UINT8        DiskFrom;
   UINT8        DiskTo;
   VOID         *RomAddress;
   UINT32       RomSize;
} EXECUTED_PCI_ROM;

EFI_STATUS SiblingOpromSkip(
    EFI_PCI_IO_PROTOCOL *PciIo,
    EXECUTED_PCI_ROM *ExecutedRom
)
{
    EFI_STATUS	Status;
    UINTN	Seg, Bus, Dev, Func;
    EXECUTED_PCI_ROM    *er;
    UINT32	PciVid, SkipID[] = OEM_SKIP_SIBLING_OPROM_ID;
    UINT8	i;

    TRACE((TRACE_ALWAYS, "SiblingOpromSkip start.\n"));
    // Check Vendor ID
    Status = PciIo->Pci.Read(
			PciIo,
			EfiPciIoWidthUint32,
			PCI_VID,
			1,
			&PciVid
			);

    if(EFI_ERROR(Status)) return EFI_UNSUPPORTED;

    for(i = 0; i < (sizeof(SkipID) / 4); i++){
	if(PciVid != SkipID[i]) return EFI_UNSUPPORTED;
    }

    Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
    ASSERT_EFI_ERROR(Status);

    for(er = ExecutedRom; er->Seg | er->Bus | er->Dev | er->Fun; er--){
        if(Dev == 0x0 && Func != 0x0){
            TRACE((TRACE_ALWAYS, "CSM OPROM: skip OpROM execution for device B%x/d%x/F%x.\n",
                Bus, Dev, Func));
            return EFI_SUCCESS;
        }
    }

    return EFI_UNSUPPORTED;
}
//SMC R1.00 E
EFI_STATUS EnableLsiDecoding(
	IN EFI_PCI_IO_PROTOCOL *PciIo
)
{
    EFI_STATUS          Status;
    EFI_STATUS          TestStatus;
    UINT16              PciVid;
    EFI_HANDLE          *HandleBuffer;
    UINTN               NoHandles;
    UINTN               Count;
    EFI_PCI_IO_PROTOCOL *TestPciIo;
    UINT16              TestPciVid;
    UINT16              TestPciCmd;
#ifdef EFI_DEBUG
    UINTN               Seg, Bus, Dev, Fun;
#endif

    // Check Vendor ID
    Status = PciIo->Pci.Read(
        PciIo,
        EfiPciIoWidthUint16,
        PCI_VID,
        1,
        &PciVid
        );

    if (EFI_ERROR(Status)) return Status;

#ifdef EFI_DEBUG
    Status = PciIo->GetLocation(
        PciIo,
        &Seg,
        &Bus,
        &Dev,
        &Fun
        );

    TRACE((-1, "[EnableLsiDecoding] B%X|D%X|F%X Vendor ID = 0x%X\n",
        Bus, Dev, Fun, PciVid));
#endif

    if(!(PciVid == LSI_PCI_VENDOR_ID || PciVid == ADAPTEC_PCI_VENDOR_ID)) return EFI_UNSUPPORTED;

    // Since one LSI device is found, enabled decoding for all LSI devices

    Status = pBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiPciIoProtocolGuid,
        NULL,
        &NoHandles,
        &HandleBuffer
        );

    if (EFI_ERROR(Status)) return Status;

    TRACE((-1, "[EnableLsiDecoding] # of PciIo's = %d\n", NoHandles));

    for (Count = 0; Count < NoHandles; Count++)
    {
        TestStatus = pBS->HandleProtocol(
            HandleBuffer[Count],
            &gEfiPciIoProtocolGuid,
            &TestPciIo
            );

        // If this fails, something is wrong
        if (EFI_ERROR(TestStatus)) continue;

        // Check that we have PciIo unique from the one passed in
        if (TestPciIo == PciIo) continue;

        // Check Vendor ID to test for LSI device
        TestStatus = TestPciIo->Pci.Read(
            TestPciIo,
            EfiPciIoWidthUint16,
            PCI_VID,
            1,
            &TestPciVid
            );

        if (EFI_ERROR(TestStatus)) continue;

        TRACE((-1, "[EnableLsiDecoding] Handle 0x%X Vendor ID = 0x%X\n", HandleBuffer[Count], TestPciVid));

        if(!(TestPciVid == LSI_PCI_VENDOR_ID || TestPciVid == ADAPTEC_PCI_VENDOR_ID))  continue;

        // Enable decoding for LSI devices
        TestStatus = TestPciIo->Pci.Read(
            TestPciIo,
            EfiPciIoWidthUint16,
            PCI_CMD,
            1,
            &TestPciCmd
            );
        if (EFI_ERROR(TestStatus)) continue;

#ifdef EFI_DEBUG
        TestStatus = TestPciIo->GetLocation(
            TestPciIo,
            &Seg,
            &Bus,
            &Dev,
            &Fun
            );

        TRACE((-1, "[EnableLsiDecoding] Changing B%X|D%X|F%X CMD Reg from 0x%X to ",
            Bus, Dev, Fun, TestPciCmd));
#endif

        TestPciCmd |= PCI_CMD_IO_SPACE | PCI_CMD_MEMORY_SPACE | PCI_CMD_BUS_MASTER;

        TRACE((-1, "0x%X\n", TestPciCmd));

        TestStatus = TestPciIo->Pci.Write(
            TestPciIo,
            EfiPciIoWidthUint16,
            PCI_CMD,
            1,
            &TestPciCmd
            );

    }

	return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************