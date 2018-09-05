
/**@file

@copyright
 Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/

  //
  // Define PCH NVS Area operation region.
  //
  
#ifndef _PCH_NVS_AREA_H_
#define _PCH_NVS_AREA_H_

#pragma pack (push,1)
typedef struct {
  UINT32   RcRevision;                              ///< Offset 0       RC Revision
  UINT16   PchSeries;                               ///< Offset 4       PCH Series
  UINT16   PchGeneration;                           ///< Offset 6       PCH Generation
  UINT32   RpAddress[20];                           ///< Offset 8       Root Port address 1
                                                    ///< Offset 12      Root Port address 2
                                                    ///< Offset 16      Root Port address 3
                                                    ///< Offset 20      Root Port address 4
                                                    ///< Offset 24      Root Port address 5
                                                    ///< Offset 28      Root Port address 6
                                                    ///< Offset 32      Root Port address 7
                                                    ///< Offset 36      Root Port address 8
                                                    ///< Offset 40      Root Port address 9
                                                    ///< Offset 44      Root Port address 10
                                                    ///< Offset 48      Root Port address 11
                                                    ///< Offset 52      Root Port address 12
                                                    ///< Offset 56      Root Port address 13
                                                    ///< Offset 60      Root Port address 14
                                                    ///< Offset 64      Root Port address 15
                                                    ///< Offset 68      Root Port address 16
                                                    ///< Offset 72      Root Port address 17
                                                    ///< Offset 76      Root Port address 18
                                                    ///< Offset 80      Root Port address 19
                                                    ///< Offset 84      Root Port address 20
  UINT64   NHLA;                                    ///< Offset 88      HD-Audio NHLT ACPI address
  UINT32   NHLL;                                    ///< Offset 96      HD-Audio NHLT ACPI length
  UINT32   ADFM;                                    ///< Offset 100     HD-Audio DSP Feature Mask
  UINT32   SBRG;                                    ///< Offset 104     SBREG_BAR
  UINT32   GPEM;                                    ///< Offset 108     GPP_X to GPE_DWX mapping
  UINT32   GP2T[13];                                ///< Offset 112     GPE 2-tier level edged enabled Gpio pads (Group Index 0)
                                                    ///< Offset 116     GPE 2-tier level edged enabled Gpio pads (Group Index 1)
                                                    ///< Offset 120     GPE 2-tier level edged enabled Gpio pads (Group Index 2)
                                                    ///< Offset 124     GPE 2-tier level edged enabled Gpio pads (Group Index 3)
                                                    ///< Offset 128     GPE 2-tier level edged enabled Gpio pads (Group Index 4)
                                                    ///< Offset 132     GPE 2-tier level edged enabled Gpio pads (Group Index 5)
                                                    ///< Offset 136     GPE 2-tier level edged enabled Gpio pads (Group Index 6)
                                                    ///< Offset 140     GPE 2-tier level edged enabled Gpio pads (Group Index 7)
                                                    ///< Offset 144     GPE 2-tier level edged enabled Gpio pads (Group Index 8)
                                                    ///< Offset 148     GPE 2-tier level edged enabled Gpio pads (Group Index 9)
  	  	  	  	  	  	  	  	  	  	  	  	    ///< Offset 152     GPE 2-tier level edged enabled Gpio pads (Group Index 10)
  	  	  	  	  	  	  	  	  	  	  	  	    ///< Offset 156     GPE 2-tier level edged enabled Gpio pads (Group Index 11)
													///< Offset 160     GPE 2-tier level edged enabled Gpio pads (Group Index 12)

  UINT16   PcieLtrMaxSnoopLatency[20];              ///< Offset 164     PCIE LTR max snoop Latency 1
                                                    ///< Offset 166     PCIE LTR max snoop Latency 2
                                                    ///< Offset 168     PCIE LTR max snoop Latency 3
                                                    ///< Offset 170     PCIE LTR max snoop Latency 4
                                                    ///< Offset 172     PCIE LTR max snoop Latency 5
                                                    ///< Offset 174     PCIE LTR max snoop Latency 6
                                                    ///< Offset 176     PCIE LTR max snoop Latency 7
                                                    ///< Offset 178     PCIE LTR max snoop Latency 8
                                                    ///< Offset 180     PCIE LTR max snoop Latency 9
                                                    ///< Offset 182     PCIE LTR max snoop Latency 10
                                                    ///< Offset 184     PCIE LTR max snoop Latency 11
                                                    ///< Offset 186     PCIE LTR max snoop Latency 12
                                                    ///< Offset 188     PCIE LTR max snoop Latency 13
                                                    ///< Offset 190     PCIE LTR max snoop Latency 14
                                                    ///< Offset 192     PCIE LTR max snoop Latency 15
                                                    ///< Offset 194     PCIE LTR max snoop Latency 16
                                                    ///< Offset 196     PCIE LTR max snoop Latency 17
                                                    ///< Offset 198     PCIE LTR max snoop Latency 18
                                                    ///< Offset 200     PCIE LTR max snoop Latency 19
                                                    ///< Offset 202     PCIE LTR max snoop Latency 20
  UINT16   PcieLtrMaxNoSnoopLatency[20];            ///< Offset 204     PCIE LTR max no snoop Latency 1
                                                    ///< Offset 206     PCIE LTR max no snoop Latency 2
                                                    ///< Offset 208     PCIE LTR max no snoop Latency 3
                                                    ///< Offset 210     PCIE LTR max no snoop Latency 4
                                                    ///< Offset 212     PCIE LTR max no snoop Latency 5
                                                    ///< Offset 214     PCIE LTR max no snoop Latency 6
                                                    ///< Offset 216     PCIE LTR max no snoop Latency 7
                                                    ///< Offset 218     PCIE LTR max no snoop Latency 8
                                                    ///< Offset 220     PCIE LTR max no snoop Latency 9
                                                    ///< Offset 222     PCIE LTR max no snoop Latency 10
                                                    ///< Offset 224     PCIE LTR max no snoop Latency 11
                                                    ///< Offset 226     PCIE LTR max no snoop Latency 12
                                                    ///< Offset 228     PCIE LTR max no snoop Latency 13
                                                    ///< Offset 230     PCIE LTR max no snoop Latency 14
                                                    ///< Offset 232     PCIE LTR max no snoop Latency 15
                                                    ///< Offset 234     PCIE LTR max no snoop Latency 16
                                                    ///< Offset 236     PCIE LTR max no snoop Latency 17
                                                    ///< Offset 238     PCIE LTR max no snoop Latency 18
                                                    ///< Offset 240     PCIE LTR max no snoop Latency 19
                                                    ///< Offset 242     PCIE LTR max no snoop Latency 20
  UINT32   SerialIoDebugUart0Bar0;                  ///< Offset 244    SerialIo Hidden UART0 BAR 0
  UINT32   SerialIoDebugUart1Bar0;                  ///< Offset 248    SerialIo Hidden UART1 BAR 0
  UINT32   ADPM;                                    ///< Offset 252     HD-Audio DSP Post-Processing Module Mask
  UINT8    XHPC;                                    ///< Offset 256     Number of HighSpeed ports implemented in XHCI controller
  UINT8    XRPC;                                    ///< Offset 257     Number of USBR ports implemented in XHCI controller
  UINT8    XSPC;                                    ///< Offset 258     Number of SuperSpeed ports implemented in XHCI controller
  UINT8    XSPA;                                    ///< Offset 259     Address of 1st SuperSpeed port
  UINT32   HPTB;                                    ///< Offset 260     HPET base address
  UINT8    HPTE;                                    ///< Offset 264     HPET enable
  //110-bytes large SerialIo block
  UINT8    SMD[11];                                 ///< Offset 265     SerialIo controller 0 (sdma) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 266     SerialIo controller 1 (i2c0) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 267     SerialIo controller 2 (i2c1) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 268     SerialIo controller 3 (spi0) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 269     SerialIo controller 4 (spi1) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 270     SerialIo controller 5 (ua00) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 271     SerialIo controller 6 (ua01) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 272     SerialIo controller 7 (shdc) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 273     SerialIo controller 8 (shdc) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 274     SerialIo controller 9 (shdc) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 275     SerialIo controller A (shdc) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  UINT8    SIR[11];                                 ///< Offset 276     SerialIo controller 0 (sdma) irq number
                                                    ///< Offset 277     SerialIo controller 1 (i2c0) irq number
                                                    ///< Offset 278     SerialIo controller 2 (i2c1) irq number
                                                    ///< Offset 279     SerialIo controller 3 (spi0) irq number
                                                    ///< Offset 280     SerialIo controller 4 (spi1) irq number
                                                    ///< Offset 281     SerialIo controller 5 (ua00) irq number
                                                    ///< Offset 282     SerialIo controller 6 (ua01) irq number
                                                    ///< Offset 283     SerialIo controller 7 (shdc) irq number
                                                    ///< Offset 284     SerialIo controller 8 (shdc) irq number
                                                    ///< Offset 285     SerialIo controller 9 (shdc) irq number
                                                    ///< Offset 286     SerialIo controller A (shdc) irq number
  UINT32   SB0[11];                                 ///< Offset 287     SerialIo controller 0 (sdma) BAR0
                                                    ///< Offset 291     SerialIo controller 1 (i2c0) BAR0
                                                    ///< Offset 295     SerialIo controller 2 (i2c1) BAR0
                                                    ///< Offset 299     SerialIo controller 3 (spi0) BAR0
                                                    ///< Offset 303     SerialIo controller 4 (spi1) BAR0
                                                    ///< Offset 307     SerialIo controller 5 (ua00) BAR0
                                                    ///< Offset 311     SerialIo controller 6 (ua01) BAR0
                                                    ///< Offset 315     SerialIo controller 7 (shdc) BAR0
                                                    ///< Offset 319     SerialIo controller 8 (shdc) BAR0
                                                    ///< Offset 323     SerialIo controller 9 (shdc) BAR0
                                                    ///< Offset 327     SerialIo controller A (shdc) BAR0
  UINT32   SB1[11];                                 ///< Offset 331     SerialIo controller 0 (sdma) BAR1
                                                    ///< Offset 335     SerialIo controller 1 (i2c0) BAR1
                                                    ///< Offset 339     SerialIo controller 2 (i2c1) BAR1
                                                    ///< Offset 343     SerialIo controller 3 (spi0) BAR1
                                                    ///< Offset 347     SerialIo controller 4 (spi1) BAR1
                                                    ///< Offset 351     SerialIo controller 5 (ua00) BAR1
                                                    ///< Offset 355     SerialIo controller 6 (ua01) BAR1
                                                    ///< Offset 359     SerialIo controller 7 (shdc) BAR1
                                                    ///< Offset 363     SerialIo controller 8 (shdc) BAR1
                                                    ///< Offset 367     SerialIo controller 9 (shdc) BAR1
                                                    ///< Offset 371     SerialIo controller A (shdc) BAR1
  //end of SerialIo block
  UINT8    GPEN;                                    ///< Offset 375     GPIO enabled
  UINT8    SGIR;                                    ///< Offset 376     GPIO IRQ
  UINT8    RstPcieStorageInterfaceType[3];          ///< Offset 377     RST PCIe Storage Cycle Router#1 Interface Type
                                                    ///< Offset 378     RST PCIe Storage Cycle Router#2 Interface Type
                                                    ///< Offset 379     RST PCIe Storage Cycle Router#3 Interface Type
  UINT8    RstPcieStoragePmCapPtr[3];               ///< Offset 380     RST PCIe Storage Cycle Router#1 Power Management Capability Pointer
                                                    ///< Offset 381     RST PCIe Storage Cycle Router#2 Power Management Capability Pointer
                                                    ///< Offset 382     RST PCIe Storage Cycle Router#3 Power Management Capability Pointer
  UINT8    RstPcieStoragePcieCapPtr[3];             ///< Offset 383     RST PCIe Storage Cycle Router#1 PCIe Capabilities Pointer
                                                    ///< Offset 384     RST PCIe Storage Cycle Router#2 PCIe Capabilities Pointer
                                                    ///< Offset 385     RST PCIe Storage Cycle Router#3 PCIe Capabilities Pointer
  UINT16   RstPcieStorageL1ssCapPtr[3];             ///< Offset 386     RST PCIe Storage Cycle Router#1 L1SS Capability Pointer
                                                    ///< Offset 388     RST PCIe Storage Cycle Router#2 L1SS Capability Pointer
                                                    ///< Offset 390     RST PCIe Storage Cycle Router#3 L1SS Capability Pointer
  UINT8    RstPcieStorageEpL1ssControl2[3];         ///< Offset 392     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data2
                                                    ///< Offset 393     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data2
                                                    ///< Offset 394     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data2
  UINT32   RstPcieStorageEpL1ssControl1[3];         ///< Offset 395     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data1
                                                    ///< Offset 399     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data1
                                                    ///< Offset 403     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data1
  UINT16   RstPcieStorageLtrCapPtr[3];              ///< Offset 407     RST PCIe Storage Cycle Router#1 LTR Capability Pointer
                                                    ///< Offset 409     RST PCIe Storage Cycle Router#2 LTR Capability Pointer
                                                    ///< Offset 411     RST PCIe Storage Cycle Router#3 LTR Capability Pointer
  UINT32   RstPcieStorageEpLtrData[3];              ///< Offset 413     RST PCIe Storage Cycle Router#1 Endpoint LTR Data
                                                    ///< Offset 417     RST PCIe Storage Cycle Router#2 Endpoint LTR Data
                                                    ///< Offset 421     RST PCIe Storage Cycle Router#3 Endpoint LTR Data
  UINT16   RstPcieStorageEpLctlData16[3];           ///< Offset 425     RST PCIe Storage Cycle Router#1 Endpoint LCTL Data
                                                    ///< Offset 427     RST PCIe Storage Cycle Router#2 Endpoint LCTL Data
                                                    ///< Offset 429     RST PCIe Storage Cycle Router#3 Endpoint LCTL Data
  UINT16   RstPcieStorageEpDctlData16[3];           ///< Offset 431     RST PCIe Storage Cycle Router#1 Endpoint DCTL Data
                                                    ///< Offset 433     RST PCIe Storage Cycle Router#2 Endpoint DCTL Data
                                                    ///< Offset 435     RST PCIe Storage Cycle Router#3 Endpoint DCTL Data
  UINT16   RstPcieStorageEpDctl2Data16[3];          ///< Offset 437     RST PCIe Storage Cycle Router#1 Endpoint DCTL2 Data
                                                    ///< Offset 439     RST PCIe Storage Cycle Router#2 Endpoint DCTL2 Data
                                                    ///< Offset 441     RST PCIe Storage Cycle Router#3 Endpoint DCTL2 Data
  UINT16   RstPcieStorageRpDctl2Data16[3];          ///< Offset 443     RST PCIe Storage Cycle Router#1 RootPort DCTL2 Data
                                                    ///< Offset 445     RST PCIe Storage Cycle Router#2 RootPort DCTL2 Data
                                                    ///< Offset 447     RST PCIe Storage Cycle Router#3 RootPort DCTL2 Data
  UINT32   RstPcieStorageUniqueTableBar[3];         ///< Offset 449     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 453     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 457     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR
  UINT32   RstPcieStorageUniqueTableBarValue[3];    ///< Offset 461     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 465     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 469     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR value
  UINT32   RstPcieStorageUniquePbaBar[3];           ///< Offset 473     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 477     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 481     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR
  UINT32   RstPcieStorageUniquePbaBarValue[3];      ///< Offset 485     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 489     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 493     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR value
  UINT8    ExitBootServicesFlag;                    ///< Offset 497     Flag indicating Exit Boot Service, to inform SMM
  UINT32   SxMemBase;                               ///< Offset 498     Sx handler reserved MMIO base
  UINT32   SxMemSize;                               ///< Offset 502     Sx handler reserved MMIO size
  UINT8    Cio2EnabledAsAcpiDevice;                 ///< Offset 506     Cio2 Device Enabled as ACPI device
  UINT8    Cio2IrqNumber;                           ///< Offset 507     Cio2 Interrupt Number
  UINT8    ThermalDeviceAcpiEnabled;                ///< Offset 508     Thermal Device Acpi mode enabled
  UINT8    ThermalDeviceInterruptLine;              ///< Offset 509     Thermal Device IRQ number
  UINT32   XhciRsvdMemBase;                         ///< Offset 510     XHCI memory base address
  UINT8    EMH4;                                    ///< Offset 514     eMMC HS400 mode enabled
  UINT8    CpuSku;                                  ///< Offset 515     CPU SKU
  UINT16   IoTrapAddress[4];                        ///< Offset 516
                                                    ///< Offset 518
                                                    ///< Offset 520
                                                    ///< Offset 522
  UINT8    IoTrapStatus[4];                         ///< Offset 524
                                                    ///< Offset 525
                                                    ///< Offset 526
                                                    ///< Offset 527
  UINT8    LtrEnable[20];                           ///< Offset 528     Latency Tolerance Reporting Enable
                                                    ///< Offset 529     Latency Tolerance Reporting Enable
                                                    ///< Offset 530     Latency Tolerance Reporting Enable
                                                    ///< Offset 531     Latency Tolerance Reporting Enable
                                                    ///< Offset 532     Latency Tolerance Reporting Enable
                                                    ///< Offset 533     Latency Tolerance Reporting Enable
                                                    ///< Offset 534     Latency Tolerance Reporting Enable
                                                    ///< Offset 535     Latency Tolerance Reporting Enable
                                                    ///< Offset 536     Latency Tolerance Reporting Enable
                                                    ///< Offset 537     Latency Tolerance Reporting Enable
                                                    ///< Offset 538     Latency Tolerance Reporting Enable
                                                    ///< Offset 539     Latency Tolerance Reporting Enable
                                                    ///< Offset 540     Latency Tolerance Reporting Enable
                                                    ///< Offset 541     Latency Tolerance Reporting Enable
                                                    ///< Offset 542     Latency Tolerance Reporting Enable
                                                    ///< Offset 543     Latency Tolerance Reporting Enable
                                                    ///< Offset 544     Latency Tolerance Reporting Enable
                                                    ///< Offset 545     Latency Tolerance Reporting Enable
                                                    ///< Offset 546     Latency Tolerance Reporting Enable
                                                    ///< Offset 547     Latency Tolerance Reporting Enable
  UINT8    ObffEnable[20];                          ///< Offset 548     Optimized Buffer Flush and Fill
                                                    ///< Offset 549     Optimized Buffer Flush and Fill
                                                    ///< Offset 550     Optimized Buffer Flush and Fill
                                                    ///< Offset 551     Optimized Buffer Flush and Fill
                                                    ///< Offset 552     Optimized Buffer Flush and Fill
                                                    ///< Offset 553     Optimized Buffer Flush and Fill
                                                    ///< Offset 554     Optimized Buffer Flush and Fill
                                                    ///< Offset 555     Optimized Buffer Flush and Fill
                                                    ///< Offset 556     Optimized Buffer Flush and Fill
                                                    ///< Offset 557     Optimized Buffer Flush and Fill
                                                    ///< Offset 558     Optimized Buffer Flush and Fill
                                                    ///< Offset 559     Optimized Buffer Flush and Fill
                                                    ///< Offset 560     Optimized Buffer Flush and Fill
                                                    ///< Offset 561     Optimized Buffer Flush and Fill
                                                    ///< Offset 562     Optimized Buffer Flush and Fill
                                                    ///< Offset 563     Optimized Buffer Flush and Fill
                                                    ///< Offset 564     Optimized Buffer Flush and Fill
                                                    ///< Offset 565     Optimized Buffer Flush and Fill
                                                    ///< Offset 566     Optimized Buffer Flush and Fill
                                                    ///< Offset 567     Optimized Buffer Flush and Fill
  UINT8    PciDelayOptimizationEcr;                 ///< Offset 568     External Change Request
  UINT64   HdaDspPpModCustomGuid1Low;               ///< Offset 569     HDA PP module custom GUID 1 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid1High;              ///< Offset 577     HDA PP module custom GUID 1 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid2Low;               ///< Offset 585     HDA PP module custom GUID 2 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid2High;              ///< Offset 593     HDA PP module custom GUID 2 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid3Low;               ///< Offset 601     HDA PP module custom GUID 3 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid3High;              ///< Offset 609     HDA PP module custom GUID 3 - second 64bit [64-127]
  UINT32   MmCfgAddress ;                           ///< Offset 617     PcieMmCfgBaseAddress
} PCH_NVS_AREA;

#pragma pack(pop)
#endif
