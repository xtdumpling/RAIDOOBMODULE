//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//   Rev. 1.16
//     Bug Fix: Fix the lane reverse issue on IOU #3 & #4
//     Reason:     
//     Auditor: Max Mu
//     Date:    Aug/22/2017
//
//   Rev. 1.15
//     Bug Fix: Support bridge solution AOC card.
//     Reason:     
//     Auditor: Kasber Chen
//     Date:    Aug/21/2017
//
//   Rev. 1.14
//     Bug Fix: Add extend flag for slot information.
//     Reason:     
//     Auditor: Kasber Chen
//     Date:    Jul/25/2017
//
//   Rev. 1.13
//     Bug Fix: Support AOC NVMe card redriver function.
//     Reason:     
//     Auditor: Kasber Chen
//     Date:    Apr/20/2017
//
//   Rev. 1.12
//     Bug Fix: Fix OPROM control no function when x8 AOC card in x16 lane reverse slot.
//     Reason:  Can't get bus number
//     Auditor: Kasber Chen
//     Date:    Mar/08/2017
//
//   Rev. 1.11
//     Reason:  Correct VPP address and port assign.
//     Auditor: Kasber Chen
//     Date:    Jan/24/2017
//
//   Rev. 1.10
//     Reason:  Fix AOC-SLG3-4E4T will not enable hotplug issue.
//     Auditor: Kasber Chen
//     Date:    Jan/20/2017
//
//   Rev. 1.10
//     Reason:  Enable sideband hotplug control with surprised hotplug disabled for legacy mode
//     Auditor: Donald Han
//     Date:    Jan/18/2017
//
//   Rev. 1.09
//     Reason:  Fix LED fail after VMD mode change to legacy mode.
//     Auditor: Kasber Chen
//     Date:    Jan/18/2017
//
//   Rev. 1.08
//     Reason:  Support NVMe device location mask function.
//     Auditor: Kasber Chen
//     Date:    Jan/17/2017
//
//   Rev. 1.07
//     Reason:  Add AOC-SLG3 card port number..
//     Auditor: Kasber Chen
//     Date:    Jan/16/2017
//
//   Rev. 1.06
//     Reason:  Sync with branch SVN_689 for NVMe legacy mode hotplug.
//     Auditor: Jacker Yeh
//     Date:    Dec/20/2016
//
//   Rev. 1.05
//     Reason:  Enable SLOTABP and SLOTPCP when NVMe work in legacy mode.
//     Auditor: Kasber Chen
//     Date:    12/19/2016
//
//   Rev. 1.04
//     Reason:  Set VPP add from CPU1 IIO0->IIO1->IIO2, CPU2 IIO0->IIO1->IIO2.
//     Auditor: Kasber Chen
//     Date:    12/06/2016
//
//   Rev. 1.03
//     Reason:  Fix AOC NVMe card detection issue in x4x4x8 IIO.
//     Auditor: Kasber Chen
//     Date:    11/24/2016
//
//   Rev. 1.02
//     Reason:  Add VPP address and port settings.
//     Auditor: Kasber Chen
//     Date:    11/16/2016
//
//   Rev. 1.01
//     Reason:  Fix VMD port can't enable.
//     Auditor: Kasber Chen
//     Date:    11/09/2016
//
//   Rev. 1.00
//     Reason:  Initialize revision.
//     Auditor: Kasber Chen
//     Date:    10/03/2016
//
//**************************************************************************//
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <PciBus.h>
#include "SmcLib.h"
#include "SmcAOCCommon.h"
#include "SmcAOC.h"
#include "SmcAOCDB.h"
#include "SmcAOCPei.h"

EFI_STATUS EFIAPI
SmcAOCBifurcation(
    IN	OUT	IIO_CONFIG	*SetupData,
    IN	UINT8	DefaultIOU0[],
    IN	UINT8	DefaultIOU1[],
    IN	UINT8	DefaultIOU2[]
)
{
    UINT8       i, j, RP_Bifurcate, Extend_Flag, CPU_NUM, IIO_NUM;
    UINT8       CPU1_VPP = 0, CPU2_VPP = 0, NVMe_Count = 0;
    UINT64      NVMe_Mode, SLOT_AOC, SLOT_AOC_FLAG;
    
    DEBUG((-1, "SmcAOCBifurcation entry.\n"));
//SLOT_AOC: CPU0, bit0  ~ bit 7 is IIO0, bit8  ~ bit15 is IIO1, bit16 ~ bit23 is IIO2
//          CPU1, bit32 ~ bit39 is IIO0, bit40 ~ bit47 is IIO1, bit48 ~ bit55 is IIO2
//one root port two bits, 3=x16 root port, 2=x8 root port, 1=x4 root port
//SLOT_AOC_FLAG: 1=lane reverse
    SLOT_AOC = PcdGet64(PcdSmcAOCLocation);
    SLOT_AOC_FLAG = PcdGet64(PcdSmcAOCExtendFlag);
    NVMe_Mode = PcdGet64(PcdSmcAOCNVMeMode);
    DEBUG((-1, "SLOT_AOC = %lx.\n", SLOT_AOC));
    DEBUG((-1, "SLOT_AOC_FLAG = %lx.\n", SLOT_AOC_FLAG));

    for(i = 0; i < sizeof(SLOT_AOC); i++){
        RP_Bifurcate = (UINT8)RShiftU64(SLOT_AOC, (i * 8));
        Extend_Flag = (UINT8)RShiftU64(SLOT_AOC_FLAG, (i * 8));
        if(!(RP_Bifurcate || Extend_Flag) || (i == 3) || (i == 7))
            continue;

        if(i < 4){	//CPU1
            CPU_NUM = 0;
            IIO_NUM = i;
        } else if(i < 8){	//CPU2
            CPU_NUM = 1;
            IIO_NUM = i - 4;
        }

        if(RP_Bifurcate == 0x03){       //set x4x4x4x4
            DEBUG((-1, "CPU%x, IIO%x set to x4x4x4x4\n", CPU_NUM + 1, IIO_NUM));
            if(IIO_NUM == 0){           //CPU1 or CPU2 IIO0
                if(DefaultIOU0[CPU_NUM] != IIO_BIFURCATE_AUTO)  continue;
                SetupData->ConfigIOU0[CPU_NUM] = IIO_BIFURCATE_x4x4x4x4;
            } else if(IIO_NUM == 1){      //CPU1 or CPU2 IIO1
                if(DefaultIOU1[CPU_NUM] != IIO_BIFURCATE_AUTO)  continue;
                SetupData->ConfigIOU1[CPU_NUM] = IIO_BIFURCATE_x4x4x4x4;
            } else if(IIO_NUM == 2){      //CPU1 or CPU2 IIO2
                if(DefaultIOU2[CPU_NUM] != IIO_BIFURCATE_AUTO)  continue;
                SetupData->ConfigIOU2[CPU_NUM] = IIO_BIFURCATE_x4x4x4x4;
            }

            if(!Extend_Flag){   //normal pcie slot
                DEBUG((-1, "normal pcie slot.\n"));
                for(j = 1; j <= 4; j++){
                    SetupData->SLOTIMP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = \
                            SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1];
                    SetupData->VppEnable[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    if(CPU_NUM == 0){
                        SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x01;
                        SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x0e;
                        CPU1_VPP++;
                    } else {
                        SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x01;
                        SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x0e;
                        CPU2_VPP++;
                    }
                }
            } else if(Extend_Flag == 0x01){    //lane reverse
                DEBUG((-1, "lane reverse slot.\n"));
                for(j = 4; j >= 1; j--){
                    SetupData->SLOTIMP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = \
                            SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1];
                    SetupData->VppEnable[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    if(CPU_NUM == 0){
                        SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x01;
                        SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x0e;
                        CPU1_VPP++;
                    } else {
                        SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x01;
                        SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x0e;
                        CPU2_VPP++;
                    }
                } 
            }

            if(SetupData->VMDEnabled[IIO_NUM + CPU_NUM * 3] &&
                    SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12]){
                DEBUG((-1, "Enable 4 VMD port from %x\n", IIO_NUM * 4 + CPU_NUM * 12));
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 1] = 1;
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 2] = 1;
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 3] = 1;
                for(j = 1; j <= 4; j++){
                    SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 0;
//                  SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 0;
                }
                NVMe_Mode |= LShiftU64((UINT64)0x0f, NVMe_Count);
            } else {
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12] = 0;
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 1] = 0;
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 2] = 0;
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 3] = 0;
                for(j = 1; j <= 4; j++){
                    SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 0;
                }
            }
            NVMe_Count += 4;
        } else if(RP_Bifurcate == 0x30){
            DEBUG((-1, "x8 NVMe card in x16 slot and slot lane reverse.\n"));
            DEBUG((-1, "CPU%x, IIO%x set to x4x4x8\n", CPU_NUM + 1, IIO_NUM));
            if(IIO_NUM == 0){               //CPU1 or CPU2 IIO0
                if(DefaultIOU0[CPU_NUM] != IIO_BIFURCATE_AUTO)      continue;
                SetupData->ConfigIOU0[CPU_NUM] = IIO_BIFURCATE_x4x4xxx8;
            } else if(IIO_NUM == 1){    //CPU1 or CPU2 IIO1
                if(DefaultIOU1[CPU_NUM] != IIO_BIFURCATE_AUTO)      continue;
                SetupData->ConfigIOU1[CPU_NUM] = IIO_BIFURCATE_x4x4xxx8;
            } else if(IIO_NUM == 2){    //CPU1 or CPU2 IIO2
                if(DefaultIOU1[CPU_NUM] != IIO_BIFURCATE_AUTO)      continue;
                SetupData->ConfigIOU2[CPU_NUM] = IIO_BIFURCATE_x4x4xxx8;
            }

            for(j = 4; j >= 3; j--){
                SetupData->SLOTIMP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = \
                        SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1];
                SetupData->VppEnable[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                if(CPU_NUM == 0){
                    SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x01;
                    SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x0e;
                    CPU1_VPP++;
                } else {
                    SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x01;
                    SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x0e;
                    CPU2_VPP++;
                }
            }

            if(SetupData->VMDEnabled[IIO_NUM + CPU_NUM * 3] &&
                    SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12]){
                DEBUG((-1, "Enable 2 VMD port from %x\n", IIO_NUM * 4 + CPU_NUM * 12 + 2));
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 2] = 1;
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 3] = 1;
                for(j = 3; j <= 4; j++){
                    SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 0;
//                  SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 0;
                }
                NVMe_Mode |= LShiftU64((UINT64)0x03, NVMe_Count);
            } else {
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 2] = 0;
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 3] = 0;
                for(j = 3; j <= 4; j++){
                    SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                    SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 0;
                }
            }
            NVMe_Count += 2;
        } else {
            if((RP_Bifurcate & 0x0f) == 0x02){
                if(IIO_NUM == 0){               //CPU1 or CPU2 IIO0
                    if(DefaultIOU0[CPU_NUM] != IIO_BIFURCATE_AUTO)      continue;
                    if((SetupData->ConfigIOU0[CPU_NUM] == IIO_BIFURCATE_AUTO) ||
                            (SetupData->ConfigIOU0[CPU_NUM] == IIO_BIFURCATE_xxxxxx16) ||
                            (SetupData->ConfigIOU0[CPU_NUM] == IIO_BIFURCATE_xxx8xxx8)){
                        DEBUG((-1, "CPU%x, IIO%x set to x8x4x4\n", CPU_NUM + 1, IIO_NUM));
                        SetupData->ConfigIOU0[CPU_NUM] = IIO_BIFURCATE_xxx8x4x4;
                    } else if(SetupData->ConfigIOU0[CPU_NUM] == IIO_BIFURCATE_x4x4xxx8){
                        DEBUG((-1, "CPU%x, IIO%x set to x4x4x4x4\n", CPU_NUM + 1, IIO_NUM));
                        SetupData->ConfigIOU0[CPU_NUM] = IIO_BIFURCATE_x4x4x4x4;
                    }
                } else if(IIO_NUM == 1){    //CPU1 or CPU2 IIO1
                    if(DefaultIOU1[CPU_NUM] != IIO_BIFURCATE_AUTO)      continue;
                    if((SetupData->ConfigIOU1[CPU_NUM] == IIO_BIFURCATE_AUTO) ||
                            (SetupData->ConfigIOU1[CPU_NUM] == IIO_BIFURCATE_xxxxxx16) ||
                            (SetupData->ConfigIOU1[CPU_NUM] == IIO_BIFURCATE_xxx8xxx8)){
                        DEBUG((-1, "CPU%x, IIO%x set to x8x4x4\n", CPU_NUM + 1, IIO_NUM));
                        SetupData->ConfigIOU1[CPU_NUM] = IIO_BIFURCATE_xxx8x4x4;
                    } else if(SetupData->ConfigIOU1[CPU_NUM] == IIO_BIFURCATE_x4x4xxx8){
                        DEBUG((-1, "CPU%x, IIO%x set to x4x4x4x4\n", CPU_NUM + 1, IIO_NUM));
                        SetupData->ConfigIOU1[CPU_NUM] = IIO_BIFURCATE_x4x4x4x4;
                    }
                } else if(IIO_NUM == 2){    //CPU1 or CPU2 IIO2
                    if(DefaultIOU1[CPU_NUM] != IIO_BIFURCATE_AUTO)      continue;
                    if((SetupData->ConfigIOU2[CPU_NUM] == IIO_BIFURCATE_AUTO) ||
                            (SetupData->ConfigIOU2[CPU_NUM] == IIO_BIFURCATE_xxxxxx16) ||
                            (SetupData->ConfigIOU2[CPU_NUM] == IIO_BIFURCATE_xxx8xxx8)){
                        DEBUG((-1, "CPU%x, IIO%x set to x8x4x4\n", CPU_NUM + 1, IIO_NUM));
                        SetupData->ConfigIOU2[CPU_NUM] = IIO_BIFURCATE_xxx8x4x4;
                    } else if(SetupData->ConfigIOU2[CPU_NUM] == IIO_BIFURCATE_x4x4xxx8){
                        DEBUG((-1, "CPU%x, IIO%x set to x4x4x4x4\n", CPU_NUM + 1, IIO_NUM));
                        SetupData->ConfigIOU2[CPU_NUM] = IIO_BIFURCATE_x4x4x4x4;
                    }
                }

                if(!(Extend_Flag & 0x0f)){      //normal pcie slot
                    for(j = 1; j <= 2; j++){
                        DEBUG((-1, "normal pcie slot.\n"));
                        SetupData->SLOTIMP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = \
                                SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1];
                        SetupData->VppEnable[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        if(CPU_NUM == 0){
                            SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x01;
                            SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x0e;
                            CPU1_VPP++;
                        } else {
                            SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x01;
                            SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x0e;
                            CPU2_VPP++;
                        }
                    }
                } else if((Extend_Flag & 0x0f) == 0x01){       //lane reverse
                    DEBUG((-1, "lane reverse slot.\n"));
                    for(j = 2; j >= 1; j--){
                        SetupData->SLOTIMP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = \
                                SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1];
                        SetupData->VppEnable[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        if(CPU_NUM == 0){
                            SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x01;
                            SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x0e;
                            CPU1_VPP++;
                        } else {
                            SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x01;
                            SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x0e;
                            CPU2_VPP++;
                        }
                    }
                }

                if(SetupData->VMDEnabled[IIO_NUM + CPU_NUM * 3] &&
                        SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12]){
                    DEBUG((-1, "Enable 2 VMD port from %x\n", IIO_NUM * 4 + CPU_NUM * 12));
                    SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 1] = 1;
                    for(j = 1; j <= 2; j++){
                        SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 0;
//                      SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 0;
                    }
                    NVMe_Mode |= LShiftU64((UINT64)0x03, NVMe_Count);
                } else {
                    SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12] = 0;
                    SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 1] = 0;
                    for(j = 1; j <= 2; j++){
                        SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 0;
                    }
                }
                NVMe_Count += 2;
            }
            if((RP_Bifurcate & 0xf0) == 0x20){
                if(IIO_NUM == 0){               //CPU1 or CPU2 IIO0
                    if(DefaultIOU0[CPU_NUM] != IIO_BIFURCATE_AUTO)      continue;
                    if((SetupData->ConfigIOU0[CPU_NUM] == IIO_BIFURCATE_AUTO) ||
                            (SetupData->ConfigIOU0[CPU_NUM] == IIO_BIFURCATE_xxxxxx16) ||
                            (SetupData->ConfigIOU0[CPU_NUM] == IIO_BIFURCATE_xxx8xxx8)){
                        DEBUG((-1, "CPU%x, IIO%x set to x4x4x8\n", CPU_NUM + 1, IIO_NUM));
                        SetupData->ConfigIOU0[CPU_NUM] = IIO_BIFURCATE_x4x4xxx8;
                    } else if(SetupData->ConfigIOU0[CPU_NUM] == IIO_BIFURCATE_xxx8x4x4){
                        DEBUG((-1, "CPU%x, IIO%x set to x4x4x4x4\n", CPU_NUM + 1, IIO_NUM));
                        SetupData->ConfigIOU0[CPU_NUM] = IIO_BIFURCATE_x4x4x4x4;
                    }
                } else if(IIO_NUM == 1){    //CPU1 or CPU2 IIO1
                    if(DefaultIOU1[CPU_NUM] != IIO_BIFURCATE_AUTO)      continue;
                    if((SetupData->ConfigIOU1[CPU_NUM] == IIO_BIFURCATE_AUTO) ||
                            (SetupData->ConfigIOU1[CPU_NUM] == IIO_BIFURCATE_xxxxxx16) ||
                            (SetupData->ConfigIOU1[CPU_NUM] == IIO_BIFURCATE_xxx8xxx8)){
                        DEBUG((-1, "CPU%x, IIO%x set to x4x4x8\n", CPU_NUM + 1, IIO_NUM));
                        SetupData->ConfigIOU1[CPU_NUM] = IIO_BIFURCATE_x4x4xxx8;
                    } else if(SetupData->ConfigIOU1[CPU_NUM] == IIO_BIFURCATE_xxx8x4x4){
                        DEBUG((-1, "CPU%x, IIO%x set to x4x4x4x4\n", CPU_NUM + 1, IIO_NUM));
                        SetupData->ConfigIOU1[CPU_NUM] = IIO_BIFURCATE_x4x4x4x4;
                    }
                } else if(IIO_NUM == 2){    //CPU1 or CPU2 IIO2
                    if(DefaultIOU1[CPU_NUM] != IIO_BIFURCATE_AUTO)      continue;
                    if((SetupData->ConfigIOU2[CPU_NUM] == IIO_BIFURCATE_AUTO) ||
                            (SetupData->ConfigIOU2[CPU_NUM] == IIO_BIFURCATE_xxxxxx16) ||
                            (SetupData->ConfigIOU2[CPU_NUM] == IIO_BIFURCATE_xxx8xxx8)){
                        DEBUG((-1, "CPU%x, IIO%x set to x4x4x8\n", CPU_NUM + 1, IIO_NUM));
                        SetupData->ConfigIOU2[CPU_NUM] = IIO_BIFURCATE_x4x4xxx8;
                    } else if(SetupData->ConfigIOU2[CPU_NUM] == IIO_BIFURCATE_xxx8x4x4){
                        DEBUG((-1, "CPU%x, IIO%x set to x4x4x4x4\n", CPU_NUM + 1, IIO_NUM));
                        SetupData->ConfigIOU2[CPU_NUM] = IIO_BIFURCATE_x4x4x4x4;
                    }
                }

                if(!(Extend_Flag & 0xf0)){      //normal pcie slot
                    for(j = 3; j <= 4; j++){
                        SetupData->SLOTIMP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = \
                                SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3];
                        SetupData->VppEnable[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        if(CPU_NUM == 0){
                            SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x01;
                            SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x0e;
                            CPU1_VPP++;
                        } else {
                            SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x01;
                            SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x0e;
                            CPU2_VPP++;
                        }
                    }
                } else if((Extend_Flag & 0xf0) == 0x10){       //lane reverse
                    for(j = 4; j >= 3; j--){
                        SetupData->SLOTIMP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = \
                                SetupData->SLOTPSP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3];
                        SetupData->VppEnable[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        if(CPU_NUM == 0){
                            SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x01;
                            SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU1_VPP & 0x0e;
                            CPU1_VPP++;
                        } else {
                            SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x01;
                            SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = CPU2_VPP & 0x0e;
                            CPU2_VPP++;
                        }
                    }
                }

                if(SetupData->VMDEnabled[IIO_NUM + CPU_NUM * 3] &&
                        SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 2]){
                    DEBUG((-1, "Enable 2 VMD port from %x\n", IIO_NUM * 4 + CPU_NUM * 12 + 2));
                    SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 3] = 1;
                    for(j = 3; j <= 4; j++){
                        SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 0;
//                      SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 0;
                    }
                    NVMe_Mode |= LShiftU64((UINT64)0x03, NVMe_Count);
                } else {
                    SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 2] = 0;
                    SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 3] = 0;
                    for(j = 3; j <= 4; j++){
                        SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 1;
                        SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + j] = 0;
                    }
                }
                NVMe_Count += 2;
            }
            if(RP_Bifurcate & 0x01){
                SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
                SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
                SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
                SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
                SetupData->VppEnable[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
                if(CPU_NUM == 0){
                    SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = CPU1_VPP & 0x01;
                    SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = CPU1_VPP & 0x0e;
                    CPU1_VPP++;
                } else {
                    SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = CPU2_VPP & 0x01;
                    SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = CPU2_VPP & 0x0e;
                    CPU2_VPP++;
                }

                if(SetupData->VMDEnabled[IIO_NUM + CPU_NUM * 3] &&
                        SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12]){
                    DEBUG((-1, "Enable VMD port %x\n", IIO_NUM * 4 + CPU_NUM * 12));
                    SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 0;
//                  SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 0;
                    NVMe_Mode |= LShiftU64((UINT64)0x01, NVMe_Count);
                } else {
                    SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12] = 0;
                    SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
                    SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
                    SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 0;
                }
                NVMe_Count++;
            }
            if(RP_Bifurcate & 0x04){
                SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
                SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
                SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
                SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
                SetupData->VppEnable[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
                if(CPU_NUM == 0){
                    SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = CPU1_VPP & 0x01;
                    SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = CPU1_VPP & 0x0e;
                    CPU1_VPP++;
                } else {
                    SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = CPU2_VPP & 0x01;
                    SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = CPU2_VPP & 0x0e;
                    CPU2_VPP++;
                }

                if(SetupData->VMDEnabled[IIO_NUM + CPU_NUM * 3] &&
                        SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 1]){
                    DEBUG((-1, "Enable VMD port %x\n", IIO_NUM * 4 + CPU_NUM * 12 + 1));
                    SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 0;
//                  SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 0;
                    NVMe_Mode |= LShiftU64((UINT64)0x01, NVMe_Count);
                } else {
                    SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 1] = 0;
                    SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
                    SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
                    SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 0;
                }
                NVMe_Count++;
            }
            if(RP_Bifurcate & 0x10){
                SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
                SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
                SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
                SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
                SetupData->VppEnable[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
                if(CPU_NUM == 0){
                    SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = CPU1_VPP & 0x01;
                    SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = CPU1_VPP & 0x0e;
                    CPU1_VPP++;
                } else {
                    SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = CPU2_VPP & 0x01;
                    SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = CPU2_VPP & 0x0e;
                    CPU2_VPP++;
                }

                if(SetupData->VMDEnabled[IIO_NUM + CPU_NUM * 3] &&
                        SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 2]){
                    DEBUG((-1, "Enable VMD port %x\n", IIO_NUM * 4 + CPU_NUM * 12 + 2));
                    SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 0;
//                  SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 0;
                    NVMe_Mode |= LShiftU64((UINT64)0x01, NVMe_Count);
                } else {
                    SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 2] = 0;
                    SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
                    SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
                    SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 0;
                }
                NVMe_Count++;
            }
            if(RP_Bifurcate & 0x40){
                SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
                SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
                SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
                SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
                SetupData->VppEnable[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
                if(CPU_NUM == 0){
                    SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = CPU1_VPP & 0x01;
                    SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = CPU1_VPP & 0x0e;
                    CPU1_VPP++;
                } else {
                    SetupData->VppPort[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = CPU2_VPP & 0x01;
                    SetupData->VppAddress[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = CPU2_VPP & 0x0e;
                    CPU2_VPP++;
                }

                if(SetupData->VMDEnabled[IIO_NUM + CPU_NUM * 3] &&
                        SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 3]){
                    DEBUG((-1, "Enable VMD port %x\n", IIO_NUM * 4 + CPU_NUM * 12 + 3));
                    SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 0;
//                  SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 0;
                    NVMe_Mode |= LShiftU64((UINT64)0x01, NVMe_Count);
                } else {
                    SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 3] = 0;
                    SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
                    SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
                    SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 0;
                }
                NVMe_Count++;
            }
        }
//PLX NVMe
        if((SLOT_AOC_FLAG & 0x03) == 0x02){
            SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
            SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
            SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
            SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
            if(SetupData->VMDEnabled[IIO_NUM + CPU_NUM * 3] &&
                        SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12]){
                DEBUG((-1, "Enable VMD port %x\n", IIO_NUM * 4 + CPU_NUM * 12));
                SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 0;
//                SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 0;
            } else {
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12] = 0;
                SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
                SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 1;
                SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 1] = 0;
            }
        }
        if((SLOT_AOC_FLAG & 0x0C) == 0x08){
            SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
            SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
            SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
            SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
            if(SetupData->VMDEnabled[IIO_NUM + CPU_NUM * 3] &&
                        SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 1]){
                DEBUG((-1, "Enable VMD port %x\n", IIO_NUM * 4 + CPU_NUM * 12 + 1));
                SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 0;
//                SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 0;
            } else {
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 1] = 0;
                SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
                SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 1;
                SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 2] = 0;
            }
        }
        if((SLOT_AOC_FLAG & 0x30) == 0x20){
            SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
            SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
            SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
            SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
            if(SetupData->VMDEnabled[IIO_NUM + CPU_NUM * 3] &&
                        SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 2]){
                DEBUG((-1, "Enable VMD port %x\n", IIO_NUM * 4 + CPU_NUM * 12 + 2));
                SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 0;
//                SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 0;
            } else {
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 2] = 0;
                SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
                SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 1;
                SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 3] = 0;
            }
        }
        if((SLOT_AOC_FLAG & 0xC0) == 0x80){
            SetupData->SLOTHPCAP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
            SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
            SetupData->SLOTAIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
            SetupData->SLOTPIP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
            if(SetupData->VMDEnabled[IIO_NUM + CPU_NUM * 3] &&
                        SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 3]){
                DEBUG((-1, "Enable VMD port %x\n", IIO_NUM * 4 + CPU_NUM * 12 + 3));
                SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 0;
//                SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 0;
            } else {
                SetupData->VMDPortEnable[IIO_NUM * 4 + CPU_NUM * 12 + 3] = 0;
                SetupData->SLOTABP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
                SetupData->SLOTPCP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 1;
                SetupData->SLOTHPSUP[IIO_NUM * 4 + CPU_NUM * NUMBER_PORTS_PER_SOCKET + 4] = 0;
            }
        }
        if(SLOT_AOC || SLOT_AOC_FLAG)
            SetupData->PcieHotPlugEnable = 1;

        DEBUG((-1, "NVMe_Mode = %lx.\n", NVMe_Mode));
        PcdSet64(PcdSmcAOCNVMeMode, NVMe_Mode);
    }
    DEBUG((-1, "SmcAOCBifurcation end.\n"));
    return EFI_SUCCESS;
}

EFI_STATUS
SearchAOCTbl(
    IN		UINT8	*AOC_Name,
    IN OUT	UINT8	*Index,
    IN OUT      UINT8   *PortNum
)
{
    EFI_STATUS	Status = EFI_UNSUPPORTED;
    UINT8	i;

    DEBUG((-1, "SearchAOCTbl entry.\n"));

    for(i = 0; i < sizeof(AOC_TBL)/sizeof(AOC_INFO); i++){
	if(!AsciiStrCmp((CHAR8*)AOC_Name, AOC_TBL[i].AOC_NAME)){
	    *Index = i;
	    *PortNum = AOC_TBL[i].PortNum;
	    Status = EFI_SUCCESS;
	}
    }

    DEBUG((-1, "SearchAOCTbl end.\n"));
    return Status;
}

VOID
GetRedriverData(
    IN  UINT8   AOC_Index,
    IN  UINT8   Port_Index,
    IN OUT      AOC_ReD ReD_Data[]
)
{
    DEBUG((-1, "GetRedriverData Entry.\n"));

    ReD_Data[0].Reg = AOC_TBL[AOC_Index].ReD_Add[Port_Index];     //redriver address
    ReD_Data[0].Data = AOC_TBL[AOC_Index].ReD_Data_Tbl_S[Port_Index] / sizeof(AOC_ReD);
    CopyMem(&ReD_Data[1], AOC_TBL[AOC_Index].ReD_Data_Tbl[Port_Index],
            AOC_TBL[AOC_Index].ReD_Data_Tbl_S[Port_Index]);
    
    DEBUG((-1, "GetRedriverData End.\n"));
}
