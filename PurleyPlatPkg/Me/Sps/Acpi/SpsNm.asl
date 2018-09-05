/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++
  
  Copyright (c) 2001-2016 Intel Corporation.  All rights reserved.

  This software and associated documentation (if any) is furnished under 
  a license and may only be used or copied in accordance with the terms 
  of the license.  Except as permitted by such license, no part of this 
  software or documentation may be reproduced, stored in a retrieval system, 
  or transmitted in any form or by any means without the express written 
  consent of Intel Corporation.
  
  Module Name:
  
    SpsNm.asl
  
  Abstract:

    This file defines ASL code for Node Manager (NM) support.
  
--*/
#define POWER_METER          1 // Can temporary disable Power Meter if needed
#define PROCESSOR_AGGREGATOR 1 // Can temporary disable Processor Aggregator if needed

#if 0
#define DbgTrace(a) Store(a, Debug)
#else
#define DbgTrace(a)
#endif

#define Notify80Socket0()  Notify(\_SB.SCK0.CP00, 0x80)  \
                           Notify(\_SB.SCK0.CP01, 0x80)  \
                           Notify(\_SB.SCK0.CP02, 0x80)  \
                           Notify(\_SB.SCK0.CP03, 0x80)  \
                           Notify(\_SB.SCK0.CP04, 0x80)  \
                           Notify(\_SB.SCK0.CP05, 0x80)  \
                           Notify(\_SB.SCK0.CP06, 0x80)  \
                           Notify(\_SB.SCK0.CP07, 0x80)  \
                           Notify(\_SB.SCK0.CP08, 0x80)  \
                           Notify(\_SB.SCK0.CP09, 0x80)  \
                           Notify(\_SB.SCK0.CP0A, 0x80)  \
                           Notify(\_SB.SCK0.CP0B, 0x80)  \
                           Notify(\_SB.SCK0.CP0C, 0x80)  \
                           Notify(\_SB.SCK0.CP0D, 0x80)  \
                           Notify(\_SB.SCK0.CP0E, 0x80)  \
                           Notify(\_SB.SCK0.CP0F, 0x80)  \
                           Notify(\_SB.SCK0.CP10, 0x80)  \
                           Notify(\_SB.SCK0.CP11, 0x80)  \
                           Notify(\_SB.SCK0.CP12, 0x80)  \
                           Notify(\_SB.SCK0.CP13, 0x80)  \
                           Notify(\_SB.SCK0.CP14, 0x80)  \
                           Notify(\_SB.SCK0.CP15, 0x80)  \
                           Notify(\_SB.SCK0.CP16, 0x80)  \
                           Notify(\_SB.SCK0.CP17, 0x80)  \
                           Notify(\_SB.SCK0.CP18, 0x80)  \
                           Notify(\_SB.SCK0.CP19, 0x80)  \
                           Notify(\_SB.SCK0.CP1A, 0x80)  \
                           Notify(\_SB.SCK0.CP1B, 0x80)  \
                           Notify(\_SB.SCK0.CP1C, 0x80)  \
                           Notify(\_SB.SCK0.CP1D, 0x80)  \
                           Notify(\_SB.SCK0.CP1E, 0x80)  \
                           Notify(\_SB.SCK0.CP1F, 0x80)  \
                           Notify(\_SB.SCK0.CP20, 0x80)  \
                           Notify(\_SB.SCK0.CP21, 0x80)  \
                           Notify(\_SB.SCK0.CP22, 0x80)  \
                           Notify(\_SB.SCK0.CP23, 0x80)  \
                           Notify(\_SB.SCK0.CP24, 0x80)  \
                           Notify(\_SB.SCK0.CP25, 0x80)  \
                           Notify(\_SB.SCK0.CP26, 0x80)  \
                           Notify(\_SB.SCK0.CP27, 0x80)  \
                           Notify(\_SB.SCK0.CP28, 0x80)  \
                           Notify(\_SB.SCK0.CP29, 0x80)  \
                           Notify(\_SB.SCK0.CP2A, 0x80)  \
                           Notify(\_SB.SCK0.CP2B, 0x80)  \
                           Notify(\_SB.SCK0.CP2C, 0x80)  \
                           Notify(\_SB.SCK0.CP2D, 0x80)  \
                           Notify(\_SB.SCK0.CP2E, 0x80)  \
                           Notify(\_SB.SCK0.CP2F, 0x80)  \
                           Notify(\_SB.SCK0.CP30, 0x80)  \
                           Notify(\_SB.SCK0.CP31, 0x80)  \
                           Notify(\_SB.SCK0.CP32, 0x80)  \
                           Notify(\_SB.SCK0.CP33, 0x80)  \
                           Notify(\_SB.SCK0.CP34, 0x80)  \
                           Notify(\_SB.SCK0.CP35, 0x80)  \
                           Notify(\_SB.SCK0.CP36, 0x80)  \
                           Notify(\_SB.SCK0.CP37, 0x80)

#define Notify82Socket0()  Notify(\_SB.SCK0.CP00, 0x82)  \
                           Notify(\_SB.SCK0.CP01, 0x82)  \
                           Notify(\_SB.SCK0.CP02, 0x82)  \
                           Notify(\_SB.SCK0.CP03, 0x82)  \
                           Notify(\_SB.SCK0.CP04, 0x82)  \
                           Notify(\_SB.SCK0.CP05, 0x82)  \
                           Notify(\_SB.SCK0.CP06, 0x82)  \
                           Notify(\_SB.SCK0.CP07, 0x82)  \
                           Notify(\_SB.SCK0.CP08, 0x82)  \
                           Notify(\_SB.SCK0.CP09, 0x82)  \
                           Notify(\_SB.SCK0.CP0A, 0x82)  \
                           Notify(\_SB.SCK0.CP0B, 0x82)  \
                           Notify(\_SB.SCK0.CP0C, 0x82)  \
                           Notify(\_SB.SCK0.CP0D, 0x82)  \
                           Notify(\_SB.SCK0.CP0E, 0x82)  \
                           Notify(\_SB.SCK0.CP0F, 0x82)  \
                           Notify(\_SB.SCK0.CP10, 0x82)  \
                           Notify(\_SB.SCK0.CP11, 0x82)  \
                           Notify(\_SB.SCK0.CP12, 0x82)  \
                           Notify(\_SB.SCK0.CP13, 0x82)  \
                           Notify(\_SB.SCK0.CP14, 0x82)  \
                           Notify(\_SB.SCK0.CP15, 0x82)  \
                           Notify(\_SB.SCK0.CP16, 0x82)  \
                           Notify(\_SB.SCK0.CP17, 0x82)  \
                           Notify(\_SB.SCK0.CP18, 0x82)  \
                           Notify(\_SB.SCK0.CP19, 0x82)  \
                           Notify(\_SB.SCK0.CP1A, 0x82)  \
                           Notify(\_SB.SCK0.CP1B, 0x82)  \
                           Notify(\_SB.SCK0.CP1C, 0x82)  \
                           Notify(\_SB.SCK0.CP1D, 0x82)  \
                           Notify(\_SB.SCK0.CP1E, 0x82)  \
                           Notify(\_SB.SCK0.CP1F, 0x82)  \
                           Notify(\_SB.SCK0.CP20, 0x82)  \
                           Notify(\_SB.SCK0.CP21, 0x82)  \
                           Notify(\_SB.SCK0.CP22, 0x82)  \
                           Notify(\_SB.SCK0.CP23, 0x82)  \
                           Notify(\_SB.SCK0.CP24, 0x82)  \
                           Notify(\_SB.SCK0.CP25, 0x82)  \
                           Notify(\_SB.SCK0.CP26, 0x82)  \
                           Notify(\_SB.SCK0.CP27, 0x82)  \
                           Notify(\_SB.SCK0.CP28, 0x82)  \
                           Notify(\_SB.SCK0.CP29, 0x82)  \
                           Notify(\_SB.SCK0.CP2A, 0x82)  \
                           Notify(\_SB.SCK0.CP2B, 0x82)  \
                           Notify(\_SB.SCK0.CP2C, 0x82)  \
                           Notify(\_SB.SCK0.CP2D, 0x82)  \
                           Notify(\_SB.SCK0.CP2E, 0x82)  \
                           Notify(\_SB.SCK0.CP2F, 0x82)  \
                           Notify(\_SB.SCK0.CP30, 0x82)  \
                           Notify(\_SB.SCK0.CP31, 0x82)  \
                           Notify(\_SB.SCK0.CP32, 0x82)  \
                           Notify(\_SB.SCK0.CP33, 0x82)  \
                           Notify(\_SB.SCK0.CP34, 0x82)  \
                           Notify(\_SB.SCK0.CP35, 0x82)  \
                           Notify(\_SB.SCK0.CP36, 0x82)  \
                           Notify(\_SB.SCK0.CP37, 0x82)

#if MAX_SOCKET > 1
#define Notify80Socket1()  Notify(\_SB.SCK1.CP00, 0x80)  \
                           Notify(\_SB.SCK1.CP01, 0x80)  \
                           Notify(\_SB.SCK1.CP02, 0x80)  \
                           Notify(\_SB.SCK1.CP03, 0x80)  \
                           Notify(\_SB.SCK1.CP04, 0x80)  \
                           Notify(\_SB.SCK1.CP05, 0x80)  \
                           Notify(\_SB.SCK1.CP06, 0x80)  \
                           Notify(\_SB.SCK1.CP07, 0x80)  \
                           Notify(\_SB.SCK1.CP08, 0x80)  \
                           Notify(\_SB.SCK1.CP09, 0x80)  \
                           Notify(\_SB.SCK1.CP0A, 0x80)  \
                           Notify(\_SB.SCK1.CP0B, 0x80)  \
                           Notify(\_SB.SCK1.CP0C, 0x80)  \
                           Notify(\_SB.SCK1.CP0D, 0x80)  \
                           Notify(\_SB.SCK1.CP0E, 0x80)  \
                           Notify(\_SB.SCK1.CP0F, 0x80)  \
                           Notify(\_SB.SCK1.CP10, 0x80)  \
                           Notify(\_SB.SCK1.CP11, 0x80)  \
                           Notify(\_SB.SCK1.CP12, 0x80)  \
                           Notify(\_SB.SCK1.CP13, 0x80)  \
                           Notify(\_SB.SCK1.CP14, 0x80)  \
                           Notify(\_SB.SCK1.CP15, 0x80)  \
                           Notify(\_SB.SCK1.CP16, 0x80)  \
                           Notify(\_SB.SCK1.CP17, 0x80)  \
                           Notify(\_SB.SCK1.CP18, 0x80)  \
                           Notify(\_SB.SCK1.CP19, 0x80)  \
                           Notify(\_SB.SCK1.CP1A, 0x80)  \
                           Notify(\_SB.SCK1.CP1B, 0x80)  \
                           Notify(\_SB.SCK1.CP1C, 0x80)  \
                           Notify(\_SB.SCK1.CP1D, 0x80)  \
                           Notify(\_SB.SCK1.CP1E, 0x80)  \
                           Notify(\_SB.SCK1.CP1F, 0x80)  \
                           Notify(\_SB.SCK1.CP20, 0x80)  \
                           Notify(\_SB.SCK1.CP21, 0x80)  \
                           Notify(\_SB.SCK1.CP22, 0x80)  \
                           Notify(\_SB.SCK1.CP23, 0x80)  \
                           Notify(\_SB.SCK1.CP24, 0x80)  \
                           Notify(\_SB.SCK1.CP25, 0x80)  \
                           Notify(\_SB.SCK1.CP26, 0x80)  \
                           Notify(\_SB.SCK1.CP27, 0x80)  \
                           Notify(\_SB.SCK1.CP28, 0x80)  \
                           Notify(\_SB.SCK1.CP29, 0x80)  \
                           Notify(\_SB.SCK1.CP2A, 0x80)  \
                           Notify(\_SB.SCK1.CP2B, 0x80)  \
                           Notify(\_SB.SCK1.CP2C, 0x80)  \
                           Notify(\_SB.SCK1.CP2D, 0x80)  \
                           Notify(\_SB.SCK1.CP2E, 0x80)  \
                           Notify(\_SB.SCK1.CP2F, 0x80)  \
                           Notify(\_SB.SCK1.CP30, 0x80)  \
                           Notify(\_SB.SCK1.CP31, 0x80)  \
                           Notify(\_SB.SCK1.CP32, 0x80)  \
                           Notify(\_SB.SCK1.CP33, 0x80)  \
                           Notify(\_SB.SCK1.CP34, 0x80)  \
                           Notify(\_SB.SCK1.CP35, 0x80)  \
                           Notify(\_SB.SCK1.CP36, 0x80)  \
                           Notify(\_SB.SCK1.CP37, 0x80)

#define Notify82Socket1()  Notify(\_SB.SCK1.CP00, 0x82)  \
                           Notify(\_SB.SCK1.CP01, 0x82)  \
                           Notify(\_SB.SCK1.CP02, 0x82)  \
                           Notify(\_SB.SCK1.CP03, 0x82)  \
                           Notify(\_SB.SCK1.CP04, 0x82)  \
                           Notify(\_SB.SCK1.CP05, 0x82)  \
                           Notify(\_SB.SCK1.CP06, 0x82)  \
                           Notify(\_SB.SCK1.CP07, 0x82)  \
                           Notify(\_SB.SCK1.CP08, 0x82)  \
                           Notify(\_SB.SCK1.CP09, 0x82)  \
                           Notify(\_SB.SCK1.CP0A, 0x82)  \
                           Notify(\_SB.SCK1.CP0B, 0x82)  \
                           Notify(\_SB.SCK1.CP0C, 0x82)  \
                           Notify(\_SB.SCK1.CP0D, 0x82)  \
                           Notify(\_SB.SCK1.CP0E, 0x82)  \
                           Notify(\_SB.SCK1.CP0F, 0x82)  \
                           Notify(\_SB.SCK1.CP10, 0x82)  \
                           Notify(\_SB.SCK1.CP11, 0x82)  \
                           Notify(\_SB.SCK1.CP12, 0x82)  \
                           Notify(\_SB.SCK1.CP13, 0x82)  \
                           Notify(\_SB.SCK1.CP14, 0x82)  \
                           Notify(\_SB.SCK1.CP15, 0x82)  \
                           Notify(\_SB.SCK1.CP16, 0x82)  \
                           Notify(\_SB.SCK1.CP17, 0x82)  \
                           Notify(\_SB.SCK1.CP18, 0x82)  \
                           Notify(\_SB.SCK1.CP19, 0x82)  \
                           Notify(\_SB.SCK1.CP1A, 0x82)  \
                           Notify(\_SB.SCK1.CP1B, 0x82)  \
                           Notify(\_SB.SCK1.CP1C, 0x82)  \
                           Notify(\_SB.SCK1.CP1D, 0x82)  \
                           Notify(\_SB.SCK1.CP1E, 0x82)  \
                           Notify(\_SB.SCK1.CP1F, 0x82)  \
                           Notify(\_SB.SCK1.CP20, 0x82)  \
                           Notify(\_SB.SCK1.CP21, 0x82)  \
                           Notify(\_SB.SCK1.CP22, 0x82)  \
                           Notify(\_SB.SCK1.CP23, 0x82)  \
                           Notify(\_SB.SCK1.CP24, 0x82)  \
                           Notify(\_SB.SCK1.CP25, 0x82)  \
                           Notify(\_SB.SCK1.CP26, 0x82)  \
                           Notify(\_SB.SCK1.CP27, 0x82)  \
                           Notify(\_SB.SCK1.CP28, 0x82)  \
                           Notify(\_SB.SCK1.CP29, 0x82)  \
                           Notify(\_SB.SCK1.CP2A, 0x82)  \
                           Notify(\_SB.SCK1.CP2B, 0x82)  \
                           Notify(\_SB.SCK1.CP2C, 0x82)  \
                           Notify(\_SB.SCK1.CP2D, 0x82)  \
                           Notify(\_SB.SCK1.CP2E, 0x82)  \
                           Notify(\_SB.SCK1.CP2F, 0x82)  \
                           Notify(\_SB.SCK1.CP30, 0x82)  \
                           Notify(\_SB.SCK1.CP31, 0x82)  \
                           Notify(\_SB.SCK1.CP32, 0x82)  \
                           Notify(\_SB.SCK1.CP33, 0x82)  \
                           Notify(\_SB.SCK1.CP34, 0x82)  \
                           Notify(\_SB.SCK1.CP35, 0x82)  \
                           Notify(\_SB.SCK1.CP36, 0x82)  \
                           Notify(\_SB.SCK1.CP37, 0x82)
#else
#define Notify80Socket1()
#define Notify82Socket1()
#endif

#if MAX_SOCKET > 2
#define Notify80Socket2()  Notify(\_SB.SCK2.CP00, 0x80)  \
                           Notify(\_SB.SCK2.CP01, 0x80)  \
                           Notify(\_SB.SCK2.CP02, 0x80)  \
                           Notify(\_SB.SCK2.CP03, 0x80)  \
                           Notify(\_SB.SCK2.CP04, 0x80)  \
                           Notify(\_SB.SCK2.CP05, 0x80)  \
                           Notify(\_SB.SCK2.CP06, 0x80)  \
                           Notify(\_SB.SCK2.CP07, 0x80)  \
                           Notify(\_SB.SCK2.CP08, 0x80)  \
                           Notify(\_SB.SCK2.CP09, 0x80)  \
                           Notify(\_SB.SCK2.CP0A, 0x80)  \
                           Notify(\_SB.SCK2.CP0B, 0x80)  \
                           Notify(\_SB.SCK2.CP0C, 0x80)  \
                           Notify(\_SB.SCK2.CP0D, 0x80)  \
                           Notify(\_SB.SCK2.CP0E, 0x80)  \
                           Notify(\_SB.SCK2.CP0F, 0x80)  \
                           Notify(\_SB.SCK2.CP10, 0x80)  \
                           Notify(\_SB.SCK2.CP11, 0x80)  \
                           Notify(\_SB.SCK2.CP12, 0x80)  \
                           Notify(\_SB.SCK2.CP13, 0x80)  \
                           Notify(\_SB.SCK2.CP14, 0x80)  \
                           Notify(\_SB.SCK2.CP15, 0x80)  \
                           Notify(\_SB.SCK2.CP16, 0x80)  \
                           Notify(\_SB.SCK2.CP17, 0x80)  \
                           Notify(\_SB.SCK2.CP18, 0x80)  \
                           Notify(\_SB.SCK2.CP19, 0x80)  \
                           Notify(\_SB.SCK2.CP1A, 0x80)  \
                           Notify(\_SB.SCK2.CP1B, 0x80)  \
                           Notify(\_SB.SCK2.CP1C, 0x80)  \
                           Notify(\_SB.SCK2.CP1D, 0x80)  \
                           Notify(\_SB.SCK2.CP1E, 0x80)  \
                           Notify(\_SB.SCK2.CP1F, 0x80)  \
                           Notify(\_SB.SCK2.CP20, 0x80)  \
                           Notify(\_SB.SCK2.CP21, 0x80)  \
                           Notify(\_SB.SCK2.CP22, 0x80)  \
                           Notify(\_SB.SCK2.CP23, 0x80)  \
                           Notify(\_SB.SCK2.CP24, 0x80)  \
                           Notify(\_SB.SCK2.CP25, 0x80)  \
                           Notify(\_SB.SCK2.CP26, 0x80)  \
                           Notify(\_SB.SCK2.CP27, 0x80)  \
                           Notify(\_SB.SCK2.CP28, 0x80)  \
                           Notify(\_SB.SCK2.CP29, 0x80)  \
                           Notify(\_SB.SCK2.CP2A, 0x80)  \
                           Notify(\_SB.SCK2.CP2B, 0x80)  \
                           Notify(\_SB.SCK2.CP2C, 0x80)  \
                           Notify(\_SB.SCK2.CP2D, 0x80)  \
                           Notify(\_SB.SCK2.CP2E, 0x80)  \
                           Notify(\_SB.SCK2.CP2F, 0x80)  \
                           Notify(\_SB.SCK2.CP30, 0x80)  \
                           Notify(\_SB.SCK2.CP31, 0x80)  \
                           Notify(\_SB.SCK2.CP32, 0x80)  \
                           Notify(\_SB.SCK2.CP33, 0x80)  \
                           Notify(\_SB.SCK2.CP34, 0x80)  \
                           Notify(\_SB.SCK2.CP35, 0x80)  \
                           Notify(\_SB.SCK2.CP36, 0x80)  \
                           Notify(\_SB.SCK2.CP37, 0x80)

#define Notify82Socket2()  Notify(\_SB.SCK2.CP00, 0x82)  \
                           Notify(\_SB.SCK2.CP01, 0x82)  \
                           Notify(\_SB.SCK2.CP02, 0x82)  \
                           Notify(\_SB.SCK2.CP03, 0x82)  \
                           Notify(\_SB.SCK2.CP04, 0x82)  \
                           Notify(\_SB.SCK2.CP05, 0x82)  \
                           Notify(\_SB.SCK2.CP06, 0x82)  \
                           Notify(\_SB.SCK2.CP07, 0x82)  \
                           Notify(\_SB.SCK2.CP08, 0x82)  \
                           Notify(\_SB.SCK2.CP09, 0x82)  \
                           Notify(\_SB.SCK2.CP0A, 0x82)  \
                           Notify(\_SB.SCK2.CP0B, 0x82)  \
                           Notify(\_SB.SCK2.CP0C, 0x82)  \
                           Notify(\_SB.SCK2.CP0D, 0x82)  \
                           Notify(\_SB.SCK2.CP0E, 0x82)  \
                           Notify(\_SB.SCK2.CP0F, 0x82)  \
                           Notify(\_SB.SCK2.CP10, 0x82)  \
                           Notify(\_SB.SCK2.CP11, 0x82)  \
                           Notify(\_SB.SCK2.CP12, 0x82)  \
                           Notify(\_SB.SCK2.CP13, 0x82)  \
                           Notify(\_SB.SCK2.CP14, 0x82)  \
                           Notify(\_SB.SCK2.CP15, 0x82)  \
                           Notify(\_SB.SCK2.CP16, 0x82)  \
                           Notify(\_SB.SCK2.CP17, 0x82)  \
                           Notify(\_SB.SCK2.CP18, 0x82)  \
                           Notify(\_SB.SCK2.CP19, 0x82)  \
                           Notify(\_SB.SCK2.CP1A, 0x82)  \
                           Notify(\_SB.SCK2.CP1B, 0x82)  \
                           Notify(\_SB.SCK2.CP1C, 0x82)  \
                           Notify(\_SB.SCK2.CP1D, 0x82)  \
                           Notify(\_SB.SCK2.CP1E, 0x82)  \
                           Notify(\_SB.SCK2.CP1F, 0x82)  \
                           Notify(\_SB.SCK2.CP20, 0x82)  \
                           Notify(\_SB.SCK2.CP21, 0x82)  \
                           Notify(\_SB.SCK2.CP22, 0x82)  \
                           Notify(\_SB.SCK2.CP23, 0x82)  \
                           Notify(\_SB.SCK2.CP24, 0x82)  \
                           Notify(\_SB.SCK2.CP25, 0x82)  \
                           Notify(\_SB.SCK2.CP26, 0x82)  \
                           Notify(\_SB.SCK2.CP27, 0x82)  \
                           Notify(\_SB.SCK2.CP28, 0x82)  \
                           Notify(\_SB.SCK2.CP29, 0x82)  \
                           Notify(\_SB.SCK2.CP2A, 0x82)  \
                           Notify(\_SB.SCK2.CP2B, 0x82)  \
                           Notify(\_SB.SCK2.CP2C, 0x82)  \
                           Notify(\_SB.SCK2.CP2D, 0x82)  \
                           Notify(\_SB.SCK2.CP2E, 0x82)  \
                           Notify(\_SB.SCK2.CP2F, 0x82)  \
                           Notify(\_SB.SCK2.CP30, 0x82)  \
                           Notify(\_SB.SCK2.CP31, 0x82)  \
                           Notify(\_SB.SCK2.CP32, 0x82)  \
                           Notify(\_SB.SCK2.CP33, 0x82)  \
                           Notify(\_SB.SCK2.CP34, 0x82)  \
                           Notify(\_SB.SCK2.CP35, 0x82)  \
                           Notify(\_SB.SCK2.CP36, 0x82)  \
                           Notify(\_SB.SCK2.CP37, 0x82)
#else
#define Notify80Socket2()
#define Notify82Socket2()
#endif

#if MAX_SOCKET > 3
#define Notify80Socket3()  Notify(\_SB.SCK3.CP00, 0x80)  \
                           Notify(\_SB.SCK3.CP01, 0x80)  \
                           Notify(\_SB.SCK3.CP02, 0x80)  \
                           Notify(\_SB.SCK3.CP03, 0x80)  \
                           Notify(\_SB.SCK3.CP04, 0x80)  \
                           Notify(\_SB.SCK3.CP05, 0x80)  \
                           Notify(\_SB.SCK3.CP06, 0x80)  \
                           Notify(\_SB.SCK3.CP07, 0x80)  \
                           Notify(\_SB.SCK3.CP08, 0x80)  \
                           Notify(\_SB.SCK3.CP09, 0x80)  \
                           Notify(\_SB.SCK3.CP0A, 0x80)  \
                           Notify(\_SB.SCK3.CP0B, 0x80)  \
                           Notify(\_SB.SCK3.CP0C, 0x80)  \
                           Notify(\_SB.SCK3.CP0D, 0x80)  \
                           Notify(\_SB.SCK3.CP0E, 0x80)  \
                           Notify(\_SB.SCK3.CP0F, 0x80)  \
                           Notify(\_SB.SCK3.CP10, 0x80)  \
                           Notify(\_SB.SCK3.CP11, 0x80)  \
                           Notify(\_SB.SCK3.CP12, 0x80)  \
                           Notify(\_SB.SCK3.CP13, 0x80)  \
                           Notify(\_SB.SCK3.CP14, 0x80)  \
                           Notify(\_SB.SCK3.CP15, 0x80)  \
                           Notify(\_SB.SCK3.CP16, 0x80)  \
                           Notify(\_SB.SCK3.CP17, 0x80)  \
                           Notify(\_SB.SCK3.CP18, 0x80)  \
                           Notify(\_SB.SCK3.CP19, 0x80)  \
                           Notify(\_SB.SCK3.CP1A, 0x80)  \
                           Notify(\_SB.SCK3.CP1B, 0x80)  \
                           Notify(\_SB.SCK3.CP1C, 0x80)  \
                           Notify(\_SB.SCK3.CP1D, 0x80)  \
                           Notify(\_SB.SCK3.CP1E, 0x80)  \
                           Notify(\_SB.SCK3.CP1F, 0x80)  \
                           Notify(\_SB.SCK3.CP20, 0x80)  \
                           Notify(\_SB.SCK3.CP21, 0x80)  \
                           Notify(\_SB.SCK3.CP22, 0x80)  \
                           Notify(\_SB.SCK3.CP23, 0x80)  \
                           Notify(\_SB.SCK3.CP24, 0x80)  \
                           Notify(\_SB.SCK3.CP25, 0x80)  \
                           Notify(\_SB.SCK3.CP26, 0x80)  \
                           Notify(\_SB.SCK3.CP27, 0x80)  \
                           Notify(\_SB.SCK3.CP28, 0x80)  \
                           Notify(\_SB.SCK3.CP29, 0x80)  \
                           Notify(\_SB.SCK3.CP2A, 0x80)  \
                           Notify(\_SB.SCK3.CP2B, 0x80)  \
                           Notify(\_SB.SCK3.CP2C, 0x80)  \
                           Notify(\_SB.SCK3.CP2D, 0x80)  \
                           Notify(\_SB.SCK3.CP2E, 0x80)  \
                           Notify(\_SB.SCK3.CP2F, 0x80)  \
                           Notify(\_SB.SCK3.CP30, 0x80)  \
                           Notify(\_SB.SCK3.CP31, 0x80)  \
                           Notify(\_SB.SCK3.CP32, 0x80)  \
                           Notify(\_SB.SCK3.CP33, 0x80)  \
                           Notify(\_SB.SCK3.CP34, 0x80)  \
                           Notify(\_SB.SCK3.CP35, 0x80)  \
                           Notify(\_SB.SCK3.CP36, 0x80)  \
                           Notify(\_SB.SCK3.CP37, 0x80)

#define Notify82Socket3()  Notify(\_SB.SCK3.CP00, 0x82)  \
                           Notify(\_SB.SCK3.CP01, 0x82)  \
                           Notify(\_SB.SCK3.CP02, 0x82)  \
                           Notify(\_SB.SCK3.CP03, 0x82)  \
                           Notify(\_SB.SCK3.CP04, 0x82)  \
                           Notify(\_SB.SCK3.CP05, 0x82)  \
                           Notify(\_SB.SCK3.CP06, 0x82)  \
                           Notify(\_SB.SCK3.CP07, 0x82)  \
                           Notify(\_SB.SCK3.CP08, 0x82)  \
                           Notify(\_SB.SCK3.CP09, 0x82)  \
                           Notify(\_SB.SCK3.CP0A, 0x82)  \
                           Notify(\_SB.SCK3.CP0B, 0x82)  \
                           Notify(\_SB.SCK3.CP0C, 0x82)  \
                           Notify(\_SB.SCK3.CP0D, 0x82)  \
                           Notify(\_SB.SCK3.CP0E, 0x82)  \
                           Notify(\_SB.SCK3.CP0F, 0x82)  \
                           Notify(\_SB.SCK3.CP10, 0x82)  \
                           Notify(\_SB.SCK3.CP11, 0x82)  \
                           Notify(\_SB.SCK3.CP12, 0x82)  \
                           Notify(\_SB.SCK3.CP13, 0x82)  \
                           Notify(\_SB.SCK3.CP14, 0x82)  \
                           Notify(\_SB.SCK3.CP15, 0x82)  \
                           Notify(\_SB.SCK3.CP16, 0x82)  \
                           Notify(\_SB.SCK3.CP17, 0x82)  \
                           Notify(\_SB.SCK3.CP18, 0x82)  \
                           Notify(\_SB.SCK3.CP19, 0x82)  \
                           Notify(\_SB.SCK3.CP1A, 0x82)  \
                           Notify(\_SB.SCK3.CP1B, 0x82)  \
                           Notify(\_SB.SCK3.CP1C, 0x82)  \
                           Notify(\_SB.SCK3.CP1D, 0x82)  \
                           Notify(\_SB.SCK3.CP1E, 0x82)  \
                           Notify(\_SB.SCK3.CP1F, 0x82)  \
                           Notify(\_SB.SCK3.CP20, 0x82)  \
                           Notify(\_SB.SCK3.CP21, 0x82)  \
                           Notify(\_SB.SCK3.CP22, 0x82)  \
                           Notify(\_SB.SCK3.CP23, 0x82)  \
                           Notify(\_SB.SCK3.CP24, 0x82)  \
                           Notify(\_SB.SCK3.CP25, 0x82)  \
                           Notify(\_SB.SCK3.CP26, 0x82)  \
                           Notify(\_SB.SCK3.CP27, 0x82)  \
                           Notify(\_SB.SCK3.CP28, 0x82)  \
                           Notify(\_SB.SCK3.CP29, 0x82)  \
                           Notify(\_SB.SCK3.CP2A, 0x82)  \
                           Notify(\_SB.SCK3.CP2B, 0x82)  \
                           Notify(\_SB.SCK3.CP2C, 0x82)  \
                           Notify(\_SB.SCK3.CP2D, 0x82)  \
                           Notify(\_SB.SCK3.CP2E, 0x82)  \
                           Notify(\_SB.SCK3.CP2F, 0x82)  \
                           Notify(\_SB.SCK3.CP30, 0x82)  \
                           Notify(\_SB.SCK3.CP31, 0x82)  \
                           Notify(\_SB.SCK3.CP32, 0x82)  \
                           Notify(\_SB.SCK3.CP33, 0x82)  \
                           Notify(\_SB.SCK3.CP34, 0x82)  \
                           Notify(\_SB.SCK3.CP35, 0x82)  \
                           Notify(\_SB.SCK3.CP36, 0x82)  \
                           Notify(\_SB.SCK3.CP37, 0x82)
#else
#define Notify80Socket3()
#define Notify82Socket3()
#endif

#if MAX_SOCKET > 4
#define Notify80Socket4()  Notify(\_SB.SCK4.CP00, 0x80)  \
                           Notify(\_SB.SCK4.CP01, 0x80)  \
                           Notify(\_SB.SCK4.CP02, 0x80)  \
                           Notify(\_SB.SCK4.CP03, 0x80)  \
                           Notify(\_SB.SCK4.CP04, 0x80)  \
                           Notify(\_SB.SCK4.CP05, 0x80)  \
                           Notify(\_SB.SCK4.CP06, 0x80)  \
                           Notify(\_SB.SCK4.CP07, 0x80)  \
                           Notify(\_SB.SCK4.CP08, 0x80)  \
                           Notify(\_SB.SCK4.CP09, 0x80)  \
                           Notify(\_SB.SCK4.CP0A, 0x80)  \
                           Notify(\_SB.SCK4.CP0B, 0x80)  \
                           Notify(\_SB.SCK4.CP0C, 0x80)  \
                           Notify(\_SB.SCK4.CP0D, 0x80)  \
                           Notify(\_SB.SCK4.CP0E, 0x80)  \
                           Notify(\_SB.SCK4.CP0F, 0x80)  \
                           Notify(\_SB.SCK4.CP10, 0x80)  \
                           Notify(\_SB.SCK4.CP11, 0x80)  \
                           Notify(\_SB.SCK4.CP12, 0x80)  \
                           Notify(\_SB.SCK4.CP13, 0x80)  \
                           Notify(\_SB.SCK4.CP14, 0x80)  \
                           Notify(\_SB.SCK4.CP15, 0x80)  \
                           Notify(\_SB.SCK4.CP16, 0x80)  \
                           Notify(\_SB.SCK4.CP17, 0x80)  \
                           Notify(\_SB.SCK4.CP18, 0x80)  \
                           Notify(\_SB.SCK4.CP19, 0x80)  \
                           Notify(\_SB.SCK4.CP1A, 0x80)  \
                           Notify(\_SB.SCK4.CP1B, 0x80)  \
                           Notify(\_SB.SCK4.CP1C, 0x80)  \
                           Notify(\_SB.SCK4.CP1D, 0x80)  \
                           Notify(\_SB.SCK4.CP1E, 0x80)  \
                           Notify(\_SB.SCK4.CP1F, 0x80)  \
                           Notify(\_SB.SCK4.CP20, 0x80)  \
                           Notify(\_SB.SCK4.CP21, 0x80)  \
                           Notify(\_SB.SCK4.CP22, 0x80)  \
                           Notify(\_SB.SCK4.CP23, 0x80)  \
                           Notify(\_SB.SCK4.CP24, 0x80)  \
                           Notify(\_SB.SCK4.CP25, 0x80)  \
                           Notify(\_SB.SCK4.CP26, 0x80)  \
                           Notify(\_SB.SCK4.CP27, 0x80)  \
                           Notify(\_SB.SCK4.CP28, 0x80)  \
                           Notify(\_SB.SCK4.CP29, 0x80)  \
                           Notify(\_SB.SCK4.CP2A, 0x80)  \
                           Notify(\_SB.SCK4.CP2B, 0x80)  \
                           Notify(\_SB.SCK4.CP2C, 0x80)  \
                           Notify(\_SB.SCK4.CP2D, 0x80)  \
                           Notify(\_SB.SCK4.CP2E, 0x80)  \
                           Notify(\_SB.SCK4.CP2F, 0x80)  \
                           Notify(\_SB.SCK4.CP30, 0x80)  \
                           Notify(\_SB.SCK4.CP31, 0x80)  \
                           Notify(\_SB.SCK4.CP32, 0x80)  \
                           Notify(\_SB.SCK4.CP33, 0x80)  \
                           Notify(\_SB.SCK4.CP34, 0x80)  \
                           Notify(\_SB.SCK4.CP35, 0x80)  \
                           Notify(\_SB.SCK4.CP36, 0x80)  \
                           Notify(\_SB.SCK4.CP37, 0x80)

#define Notify82Socket4()  Notify(\_SB.SCK4.CP00, 0x82)  \
                           Notify(\_SB.SCK4.CP01, 0x82)  \
                           Notify(\_SB.SCK4.CP02, 0x82)  \
                           Notify(\_SB.SCK4.CP03, 0x82)  \
                           Notify(\_SB.SCK4.CP04, 0x82)  \
                           Notify(\_SB.SCK4.CP05, 0x82)  \
                           Notify(\_SB.SCK4.CP06, 0x82)  \
                           Notify(\_SB.SCK4.CP07, 0x82)  \
                           Notify(\_SB.SCK4.CP08, 0x82)  \
                           Notify(\_SB.SCK4.CP09, 0x82)  \
                           Notify(\_SB.SCK4.CP0A, 0x82)  \
                           Notify(\_SB.SCK4.CP0B, 0x82)  \
                           Notify(\_SB.SCK4.CP0C, 0x82)  \
                           Notify(\_SB.SCK4.CP0D, 0x82)  \
                           Notify(\_SB.SCK4.CP0E, 0x82)  \
                           Notify(\_SB.SCK4.CP0F, 0x82)  \
                           Notify(\_SB.SCK4.CP10, 0x82)  \
                           Notify(\_SB.SCK4.CP11, 0x82)  \
                           Notify(\_SB.SCK4.CP12, 0x82)  \
                           Notify(\_SB.SCK4.CP13, 0x82)  \
                           Notify(\_SB.SCK4.CP14, 0x82)  \
                           Notify(\_SB.SCK4.CP15, 0x82)  \
                           Notify(\_SB.SCK4.CP16, 0x82)  \
                           Notify(\_SB.SCK4.CP17, 0x82)  \
                           Notify(\_SB.SCK4.CP18, 0x82)  \
                           Notify(\_SB.SCK4.CP19, 0x82)  \
                           Notify(\_SB.SCK4.CP1A, 0x82)  \
                           Notify(\_SB.SCK4.CP1B, 0x82)  \
                           Notify(\_SB.SCK4.CP1C, 0x82)  \
                           Notify(\_SB.SCK4.CP1D, 0x82)  \
                           Notify(\_SB.SCK4.CP1E, 0x82)  \
                           Notify(\_SB.SCK4.CP1F, 0x82)  \
                           Notify(\_SB.SCK4.CP20, 0x82)  \
                           Notify(\_SB.SCK4.CP21, 0x82)  \
                           Notify(\_SB.SCK4.CP22, 0x82)  \
                           Notify(\_SB.SCK4.CP23, 0x82)  \
                           Notify(\_SB.SCK4.CP24, 0x82)  \
                           Notify(\_SB.SCK4.CP25, 0x82)  \
                           Notify(\_SB.SCK4.CP26, 0x82)  \
                           Notify(\_SB.SCK4.CP27, 0x82)  \
                           Notify(\_SB.SCK4.CP28, 0x82)  \
                           Notify(\_SB.SCK4.CP29, 0x82)  \
                           Notify(\_SB.SCK4.CP2A, 0x82)  \
                           Notify(\_SB.SCK4.CP2B, 0x82)  \
                           Notify(\_SB.SCK4.CP2C, 0x82)  \
                           Notify(\_SB.SCK4.CP2D, 0x82)  \
                           Notify(\_SB.SCK4.CP2E, 0x82)  \
                           Notify(\_SB.SCK4.CP2F, 0x82)  \
                           Notify(\_SB.SCK4.CP30, 0x82)  \
                           Notify(\_SB.SCK4.CP31, 0x82)  \
                           Notify(\_SB.SCK4.CP32, 0x82)  \
                           Notify(\_SB.SCK4.CP33, 0x82)  \
                           Notify(\_SB.SCK4.CP34, 0x82)  \
                           Notify(\_SB.SCK4.CP35, 0x82)  \
                           Notify(\_SB.SCK4.CP36, 0x82)  \
                           Notify(\_SB.SCK4.CP37, 0x82)
#else
#define Notify80Socket4()
#define Notify82Socket4()
#endif

#if MAX_SOCKET > 5
#define Notify80Socket5()  Notify(\_SB.SCK5.CP00, 0x80)  \
                           Notify(\_SB.SCK5.CP01, 0x80)  \
                           Notify(\_SB.SCK5.CP02, 0x80)  \
                           Notify(\_SB.SCK5.CP03, 0x80)  \
                           Notify(\_SB.SCK5.CP04, 0x80)  \
                           Notify(\_SB.SCK5.CP05, 0x80)  \
                           Notify(\_SB.SCK5.CP06, 0x80)  \
                           Notify(\_SB.SCK5.CP07, 0x80)  \
                           Notify(\_SB.SCK5.CP08, 0x80)  \
                           Notify(\_SB.SCK5.CP09, 0x80)  \
                           Notify(\_SB.SCK5.CP0A, 0x80)  \
                           Notify(\_SB.SCK5.CP0B, 0x80)  \
                           Notify(\_SB.SCK5.CP0C, 0x80)  \
                           Notify(\_SB.SCK5.CP0D, 0x80)  \
                           Notify(\_SB.SCK5.CP0E, 0x80)  \
                           Notify(\_SB.SCK5.CP0F, 0x80)  \
                           Notify(\_SB.SCK5.CP10, 0x80)  \
                           Notify(\_SB.SCK5.CP11, 0x80)  \
                           Notify(\_SB.SCK5.CP12, 0x80)  \
                           Notify(\_SB.SCK5.CP13, 0x80)  \
                           Notify(\_SB.SCK5.CP14, 0x80)  \
                           Notify(\_SB.SCK5.CP15, 0x80)  \
                           Notify(\_SB.SCK5.CP16, 0x80)  \
                           Notify(\_SB.SCK5.CP17, 0x80)  \
                           Notify(\_SB.SCK5.CP18, 0x80)  \
                           Notify(\_SB.SCK5.CP19, 0x80)  \
                           Notify(\_SB.SCK5.CP1A, 0x80)  \
                           Notify(\_SB.SCK5.CP1B, 0x80)  \
                           Notify(\_SB.SCK5.CP1C, 0x80)  \
                           Notify(\_SB.SCK5.CP1D, 0x80)  \
                           Notify(\_SB.SCK5.CP1E, 0x80)  \
                           Notify(\_SB.SCK5.CP1F, 0x80)  \
                           Notify(\_SB.SCK5.CP20, 0x80)  \
                           Notify(\_SB.SCK5.CP21, 0x80)  \
                           Notify(\_SB.SCK5.CP22, 0x80)  \
                           Notify(\_SB.SCK5.CP23, 0x80)  \
                           Notify(\_SB.SCK5.CP24, 0x80)  \
                           Notify(\_SB.SCK5.CP25, 0x80)  \
                           Notify(\_SB.SCK5.CP26, 0x80)  \
                           Notify(\_SB.SCK5.CP27, 0x80)  \
                           Notify(\_SB.SCK5.CP28, 0x80)  \
                           Notify(\_SB.SCK5.CP29, 0x80)  \
                           Notify(\_SB.SCK5.CP2A, 0x80)  \
                           Notify(\_SB.SCK5.CP2B, 0x80)  \
                           Notify(\_SB.SCK5.CP2C, 0x80)  \
                           Notify(\_SB.SCK5.CP2D, 0x80)  \
                           Notify(\_SB.SCK5.CP2E, 0x80)  \
                           Notify(\_SB.SCK5.CP2F, 0x80)  \
                           Notify(\_SB.SCK5.CP30, 0x80)  \
                           Notify(\_SB.SCK5.CP31, 0x80)  \
                           Notify(\_SB.SCK5.CP32, 0x80)  \
                           Notify(\_SB.SCK5.CP33, 0x80)  \
                           Notify(\_SB.SCK5.CP34, 0x80)  \
                           Notify(\_SB.SCK5.CP35, 0x80)  \
                           Notify(\_SB.SCK5.CP36, 0x80)  \
                           Notify(\_SB.SCK5.CP37, 0x80)

#define Notify82Socket5()  Notify(\_SB.SCK5.CP00, 0x82)  \
                           Notify(\_SB.SCK5.CP01, 0x82)  \
                           Notify(\_SB.SCK5.CP02, 0x82)  \
                           Notify(\_SB.SCK5.CP03, 0x82)  \
                           Notify(\_SB.SCK5.CP04, 0x82)  \
                           Notify(\_SB.SCK5.CP05, 0x82)  \
                           Notify(\_SB.SCK5.CP06, 0x82)  \
                           Notify(\_SB.SCK5.CP07, 0x82)  \
                           Notify(\_SB.SCK5.CP08, 0x82)  \
                           Notify(\_SB.SCK5.CP09, 0x82)  \
                           Notify(\_SB.SCK5.CP0A, 0x82)  \
                           Notify(\_SB.SCK5.CP0B, 0x82)  \
                           Notify(\_SB.SCK5.CP0C, 0x82)  \
                           Notify(\_SB.SCK5.CP0D, 0x82)  \
                           Notify(\_SB.SCK5.CP0E, 0x82)  \
                           Notify(\_SB.SCK5.CP0F, 0x82)  \
                           Notify(\_SB.SCK5.CP10, 0x82)  \
                           Notify(\_SB.SCK5.CP11, 0x82)  \
                           Notify(\_SB.SCK5.CP12, 0x82)  \
                           Notify(\_SB.SCK5.CP13, 0x82)  \
                           Notify(\_SB.SCK5.CP14, 0x82)  \
                           Notify(\_SB.SCK5.CP15, 0x82)  \
                           Notify(\_SB.SCK5.CP16, 0x82)  \
                           Notify(\_SB.SCK5.CP17, 0x82)  \
                           Notify(\_SB.SCK5.CP18, 0x82)  \
                           Notify(\_SB.SCK5.CP19, 0x82)  \
                           Notify(\_SB.SCK5.CP1A, 0x82)  \
                           Notify(\_SB.SCK5.CP1B, 0x82)  \
                           Notify(\_SB.SCK5.CP1C, 0x82)  \
                           Notify(\_SB.SCK5.CP1D, 0x82)  \
                           Notify(\_SB.SCK5.CP1E, 0x82)  \
                           Notify(\_SB.SCK5.CP1F, 0x82)  \
                           Notify(\_SB.SCK5.CP20, 0x82)  \
                           Notify(\_SB.SCK5.CP21, 0x82)  \
                           Notify(\_SB.SCK5.CP22, 0x82)  \
                           Notify(\_SB.SCK5.CP23, 0x82)  \
                           Notify(\_SB.SCK5.CP24, 0x82)  \
                           Notify(\_SB.SCK5.CP25, 0x82)  \
                           Notify(\_SB.SCK5.CP26, 0x82)  \
                           Notify(\_SB.SCK5.CP27, 0x82)  \
                           Notify(\_SB.SCK5.CP28, 0x82)  \
                           Notify(\_SB.SCK5.CP29, 0x82)  \
                           Notify(\_SB.SCK5.CP2A, 0x82)  \
                           Notify(\_SB.SCK5.CP2B, 0x82)  \
                           Notify(\_SB.SCK5.CP2C, 0x82)  \
                           Notify(\_SB.SCK5.CP2D, 0x82)  \
                           Notify(\_SB.SCK5.CP2E, 0x82)  \
                           Notify(\_SB.SCK5.CP2F, 0x82)  \
                           Notify(\_SB.SCK5.CP30, 0x82)  \
                           Notify(\_SB.SCK5.CP31, 0x82)  \
                           Notify(\_SB.SCK5.CP32, 0x82)  \
                           Notify(\_SB.SCK5.CP33, 0x82)  \
                           Notify(\_SB.SCK5.CP34, 0x82)  \
                           Notify(\_SB.SCK5.CP35, 0x82)  \
                           Notify(\_SB.SCK5.CP36, 0x82)  \
                           Notify(\_SB.SCK5.CP37, 0x82)
#else
#define Notify80Socket5()
#define Notify82Socket5()
#endif

#if MAX_SOCKET > 6
#define Notify80Socket6()  Notify(\_SB.SCK6.CP00, 0x80)  \
                           Notify(\_SB.SCK6.CP01, 0x80)  \
                           Notify(\_SB.SCK6.CP02, 0x80)  \
                           Notify(\_SB.SCK6.CP03, 0x80)  \
                           Notify(\_SB.SCK6.CP04, 0x80)  \
                           Notify(\_SB.SCK6.CP05, 0x80)  \
                           Notify(\_SB.SCK6.CP06, 0x80)  \
                           Notify(\_SB.SCK6.CP07, 0x80)  \
                           Notify(\_SB.SCK6.CP08, 0x80)  \
                           Notify(\_SB.SCK6.CP09, 0x80)  \
                           Notify(\_SB.SCK6.CP0A, 0x80)  \
                           Notify(\_SB.SCK6.CP0B, 0x80)  \
                           Notify(\_SB.SCK6.CP0C, 0x80)  \
                           Notify(\_SB.SCK6.CP0D, 0x80)  \
                           Notify(\_SB.SCK6.CP0E, 0x80)  \
                           Notify(\_SB.SCK6.CP0F, 0x80)  \
                           Notify(\_SB.SCK6.CP10, 0x80)  \
                           Notify(\_SB.SCK6.CP11, 0x80)  \
                           Notify(\_SB.SCK6.CP12, 0x80)  \
                           Notify(\_SB.SCK6.CP13, 0x80)  \
                           Notify(\_SB.SCK6.CP14, 0x80)  \
                           Notify(\_SB.SCK6.CP15, 0x80)  \
                           Notify(\_SB.SCK6.CP16, 0x80)  \
                           Notify(\_SB.SCK6.CP17, 0x80)  \
                           Notify(\_SB.SCK6.CP18, 0x80)  \
                           Notify(\_SB.SCK6.CP19, 0x80)  \
                           Notify(\_SB.SCK6.CP1A, 0x80)  \
                           Notify(\_SB.SCK6.CP1B, 0x80)  \
                           Notify(\_SB.SCK6.CP1C, 0x80)  \
                           Notify(\_SB.SCK6.CP1D, 0x80)  \
                           Notify(\_SB.SCK6.CP1E, 0x80)  \
                           Notify(\_SB.SCK6.CP1F, 0x80)  \
                           Notify(\_SB.SCK6.CP20, 0x80)  \
                           Notify(\_SB.SCK6.CP21, 0x80)  \
                           Notify(\_SB.SCK6.CP22, 0x80)  \
                           Notify(\_SB.SCK6.CP23, 0x80)  \
                           Notify(\_SB.SCK6.CP24, 0x80)  \
                           Notify(\_SB.SCK6.CP25, 0x80)  \
                           Notify(\_SB.SCK6.CP26, 0x80)  \
                           Notify(\_SB.SCK6.CP27, 0x80)  \
                           Notify(\_SB.SCK6.CP28, 0x80)  \
                           Notify(\_SB.SCK6.CP29, 0x80)  \
                           Notify(\_SB.SCK6.CP2A, 0x80)  \
                           Notify(\_SB.SCK6.CP2B, 0x80)  \
                           Notify(\_SB.SCK6.CP2C, 0x80)  \
                           Notify(\_SB.SCK6.CP2D, 0x80)  \
                           Notify(\_SB.SCK6.CP2E, 0x80)  \
                           Notify(\_SB.SCK6.CP2F, 0x80)  \
                           Notify(\_SB.SCK6.CP30, 0x80)  \
                           Notify(\_SB.SCK6.CP31, 0x80)  \
                           Notify(\_SB.SCK6.CP32, 0x80)  \
                           Notify(\_SB.SCK6.CP33, 0x80)  \
                           Notify(\_SB.SCK6.CP34, 0x80)  \
                           Notify(\_SB.SCK6.CP35, 0x80)  \
                           Notify(\_SB.SCK6.CP36, 0x80)  \
                           Notify(\_SB.SCK6.CP37, 0x80)

#define Notify82Socket6()  Notify(\_SB.SCK6.CP00, 0x82)  \
                           Notify(\_SB.SCK6.CP01, 0x82)  \
                           Notify(\_SB.SCK6.CP02, 0x82)  \
                           Notify(\_SB.SCK6.CP03, 0x82)  \
                           Notify(\_SB.SCK6.CP04, 0x82)  \
                           Notify(\_SB.SCK6.CP05, 0x82)  \
                           Notify(\_SB.SCK6.CP06, 0x82)  \
                           Notify(\_SB.SCK6.CP07, 0x82)  \
                           Notify(\_SB.SCK6.CP08, 0x82)  \
                           Notify(\_SB.SCK6.CP09, 0x82)  \
                           Notify(\_SB.SCK6.CP0A, 0x82)  \
                           Notify(\_SB.SCK6.CP0B, 0x82)  \
                           Notify(\_SB.SCK6.CP0C, 0x82)  \
                           Notify(\_SB.SCK6.CP0D, 0x82)  \
                           Notify(\_SB.SCK6.CP0E, 0x82)  \
                           Notify(\_SB.SCK6.CP0F, 0x82)  \
                           Notify(\_SB.SCK6.CP10, 0x82)  \
                           Notify(\_SB.SCK6.CP11, 0x82)  \
                           Notify(\_SB.SCK6.CP12, 0x82)  \
                           Notify(\_SB.SCK6.CP13, 0x82)  \
                           Notify(\_SB.SCK6.CP14, 0x82)  \
                           Notify(\_SB.SCK6.CP15, 0x82)  \
                           Notify(\_SB.SCK6.CP16, 0x82)  \
                           Notify(\_SB.SCK6.CP17, 0x82)  \
                           Notify(\_SB.SCK6.CP18, 0x82)  \
                           Notify(\_SB.SCK6.CP19, 0x82)  \
                           Notify(\_SB.SCK6.CP1A, 0x82)  \
                           Notify(\_SB.SCK6.CP1B, 0x82)  \
                           Notify(\_SB.SCK6.CP1C, 0x82)  \
                           Notify(\_SB.SCK6.CP1D, 0x82)  \
                           Notify(\_SB.SCK6.CP1E, 0x82)  \
                           Notify(\_SB.SCK6.CP1F, 0x82)  \
                           Notify(\_SB.SCK6.CP20, 0x82)  \
                           Notify(\_SB.SCK6.CP21, 0x82)  \
                           Notify(\_SB.SCK6.CP22, 0x82)  \
                           Notify(\_SB.SCK6.CP23, 0x82)  \
                           Notify(\_SB.SCK6.CP24, 0x82)  \
                           Notify(\_SB.SCK6.CP25, 0x82)  \
                           Notify(\_SB.SCK6.CP26, 0x82)  \
                           Notify(\_SB.SCK6.CP27, 0x82)  \
                           Notify(\_SB.SCK6.CP28, 0x82)  \
                           Notify(\_SB.SCK6.CP29, 0x82)  \
                           Notify(\_SB.SCK6.CP2A, 0x82)  \
                           Notify(\_SB.SCK6.CP2B, 0x82)  \
                           Notify(\_SB.SCK6.CP2C, 0x82)  \
                           Notify(\_SB.SCK6.CP2D, 0x82)  \
                           Notify(\_SB.SCK6.CP2E, 0x82)  \
                           Notify(\_SB.SCK6.CP2F, 0x82)  \
                           Notify(\_SB.SCK6.CP30, 0x82)  \
                           Notify(\_SB.SCK6.CP31, 0x82)  \
                           Notify(\_SB.SCK6.CP32, 0x82)  \
                           Notify(\_SB.SCK6.CP33, 0x82)  \
                           Notify(\_SB.SCK6.CP34, 0x82)  \
                           Notify(\_SB.SCK6.CP35, 0x82)  \
                           Notify(\_SB.SCK6.CP36, 0x82)  \
                           Notify(\_SB.SCK6.CP37, 0x82)
#else
#define Notify80Socket6()
#define Notify82Socket6()
#endif

#if MAX_SOCKET > 7
#define Notify80Socket7()  Notify(\_SB.SCK7.CP00, 0x80)  \
                           Notify(\_SB.SCK7.CP01, 0x80)  \
                           Notify(\_SB.SCK7.CP02, 0x80)  \
                           Notify(\_SB.SCK7.CP03, 0x80)  \
                           Notify(\_SB.SCK7.CP04, 0x80)  \
                           Notify(\_SB.SCK7.CP05, 0x80)  \
                           Notify(\_SB.SCK7.CP06, 0x80)  \
                           Notify(\_SB.SCK7.CP07, 0x80)  \
                           Notify(\_SB.SCK7.CP08, 0x80)  \
                           Notify(\_SB.SCK7.CP09, 0x80)  \
                           Notify(\_SB.SCK7.CP0A, 0x80)  \
                           Notify(\_SB.SCK7.CP0B, 0x80)  \
                           Notify(\_SB.SCK7.CP0C, 0x80)  \
                           Notify(\_SB.SCK7.CP0D, 0x80)  \
                           Notify(\_SB.SCK7.CP0E, 0x80)  \
                           Notify(\_SB.SCK7.CP0F, 0x80)  \
                           Notify(\_SB.SCK7.CP10, 0x80)  \
                           Notify(\_SB.SCK7.CP11, 0x80)  \
                           Notify(\_SB.SCK7.CP12, 0x80)  \
                           Notify(\_SB.SCK7.CP13, 0x80)  \
                           Notify(\_SB.SCK7.CP14, 0x80)  \
                           Notify(\_SB.SCK7.CP15, 0x80)  \
                           Notify(\_SB.SCK7.CP16, 0x80)  \
                           Notify(\_SB.SCK7.CP17, 0x80)  \
                           Notify(\_SB.SCK7.CP18, 0x80)  \
                           Notify(\_SB.SCK7.CP19, 0x80)  \
                           Notify(\_SB.SCK7.CP1A, 0x80)  \
                           Notify(\_SB.SCK7.CP1B, 0x80)  \
                           Notify(\_SB.SCK7.CP1C, 0x80)  \
                           Notify(\_SB.SCK7.CP1D, 0x80)  \
                           Notify(\_SB.SCK7.CP1E, 0x80)  \
                           Notify(\_SB.SCK7.CP1F, 0x80)  \
                           Notify(\_SB.SCK7.CP20, 0x80)  \
                           Notify(\_SB.SCK7.CP21, 0x80)  \
                           Notify(\_SB.SCK7.CP22, 0x80)  \
                           Notify(\_SB.SCK7.CP23, 0x80)  \
                           Notify(\_SB.SCK7.CP24, 0x80)  \
                           Notify(\_SB.SCK7.CP25, 0x80)  \
                           Notify(\_SB.SCK7.CP26, 0x80)  \
                           Notify(\_SB.SCK7.CP27, 0x80)  \
                           Notify(\_SB.SCK7.CP28, 0x80)  \
                           Notify(\_SB.SCK7.CP29, 0x80)  \
                           Notify(\_SB.SCK7.CP2A, 0x80)  \
                           Notify(\_SB.SCK7.CP2B, 0x80)  \
                           Notify(\_SB.SCK7.CP2C, 0x80)  \
                           Notify(\_SB.SCK7.CP2D, 0x80)  \
                           Notify(\_SB.SCK7.CP2E, 0x80)  \
                           Notify(\_SB.SCK7.CP2F, 0x80)  \
                           Notify(\_SB.SCK7.CP30, 0x80)  \
                           Notify(\_SB.SCK7.CP31, 0x80)  \
                           Notify(\_SB.SCK7.CP32, 0x80)  \
                           Notify(\_SB.SCK7.CP33, 0x80)  \
                           Notify(\_SB.SCK7.CP34, 0x80)  \
                           Notify(\_SB.SCK7.CP35, 0x80)  \
                           Notify(\_SB.SCK7.CP36, 0x80)  \
                           Notify(\_SB.SCK7.CP37, 0x80)

#define Notify82Socket7()  Notify(\_SB.SCK7.CP00, 0x82)  \
                           Notify(\_SB.SCK7.CP01, 0x82)  \
                           Notify(\_SB.SCK7.CP02, 0x82)  \
                           Notify(\_SB.SCK7.CP03, 0x82)  \
                           Notify(\_SB.SCK7.CP04, 0x82)  \
                           Notify(\_SB.SCK7.CP05, 0x82)  \
                           Notify(\_SB.SCK7.CP06, 0x82)  \
                           Notify(\_SB.SCK7.CP07, 0x82)  \
                           Notify(\_SB.SCK7.CP08, 0x82)  \
                           Notify(\_SB.SCK7.CP09, 0x82)  \
                           Notify(\_SB.SCK7.CP0A, 0x82)  \
                           Notify(\_SB.SCK7.CP0B, 0x82)  \
                           Notify(\_SB.SCK7.CP0C, 0x82)  \
                           Notify(\_SB.SCK7.CP0D, 0x82)  \
                           Notify(\_SB.SCK7.CP0E, 0x82)  \
                           Notify(\_SB.SCK7.CP0F, 0x82)  \
                           Notify(\_SB.SCK7.CP10, 0x82)  \
                           Notify(\_SB.SCK7.CP11, 0x82)  \
                           Notify(\_SB.SCK7.CP12, 0x82)  \
                           Notify(\_SB.SCK7.CP13, 0x82)  \
                           Notify(\_SB.SCK7.CP14, 0x82)  \
                           Notify(\_SB.SCK7.CP15, 0x82)  \
                           Notify(\_SB.SCK7.CP16, 0x82)  \
                           Notify(\_SB.SCK7.CP17, 0x82)  \
                           Notify(\_SB.SCK7.CP18, 0x82)  \
                           Notify(\_SB.SCK7.CP19, 0x82)  \
                           Notify(\_SB.SCK7.CP1A, 0x82)  \
                           Notify(\_SB.SCK7.CP1B, 0x82)  \
                           Notify(\_SB.SCK7.CP1C, 0x82)  \
                           Notify(\_SB.SCK7.CP1D, 0x82)  \
                           Notify(\_SB.SCK7.CP1E, 0x82)  \
                           Notify(\_SB.SCK7.CP1F, 0x82)  \
                           Notify(\_SB.SCK7.CP20, 0x82)  \
                           Notify(\_SB.SCK7.CP21, 0x82)  \
                           Notify(\_SB.SCK7.CP22, 0x82)  \
                           Notify(\_SB.SCK7.CP23, 0x82)  \
                           Notify(\_SB.SCK7.CP24, 0x82)  \
                           Notify(\_SB.SCK7.CP25, 0x82)  \
                           Notify(\_SB.SCK7.CP26, 0x82)  \
                           Notify(\_SB.SCK7.CP27, 0x82)  \
                           Notify(\_SB.SCK7.CP28, 0x82)  \
                           Notify(\_SB.SCK7.CP29, 0x82)  \
                           Notify(\_SB.SCK7.CP2A, 0x82)  \
                           Notify(\_SB.SCK7.CP2B, 0x82)  \
                           Notify(\_SB.SCK7.CP2C, 0x82)  \
                           Notify(\_SB.SCK7.CP2D, 0x82)  \
                           Notify(\_SB.SCK7.CP2E, 0x82)  \
                           Notify(\_SB.SCK7.CP2F, 0x82)  \
                           Notify(\_SB.SCK7.CP30, 0x82)  \
                           Notify(\_SB.SCK7.CP31, 0x82)  \
                           Notify(\_SB.SCK7.CP32, 0x82)  \
                           Notify(\_SB.SCK7.CP33, 0x82)  \
                           Notify(\_SB.SCK7.CP34, 0x82)  \
                           Notify(\_SB.SCK7.CP35, 0x82)  \
                           Notify(\_SB.SCK7.CP36, 0x82)  \
                           Notify(\_SB.SCK7.CP37, 0x82)
#else
#define Notify80Socket7()
#define Notify82Socket7()
#endif

DefinitionBlock("SpsNm.aml", "SSDT", 2, "INTEL", "SpsNm   ", 2)
{
  External(\HWEN, FieldUnitObj)       // HWPM enabled as provided in BIOS_ACPI_PARAM::HWPMEnable
  External(\PETE, IntObj)
  External(\PSLM, PkgObj)
  External(\TSLM, PkgObj)
  
  External(\_SB.PRAD, DeviceObj)
  
  External(\_SB.SCK0, DeviceObj)
  External(\_SB.SCK0.CP00, DeviceObj)
  External(\_SB.SCK0.CP01, DeviceObj)
  External(\_SB.SCK0.CP02, DeviceObj)
  External(\_SB.SCK0.CP03, DeviceObj)
  External(\_SB.SCK0.CP04, DeviceObj)
  External(\_SB.SCK0.CP05, DeviceObj)
  External(\_SB.SCK0.CP06, DeviceObj)
  External(\_SB.SCK0.CP07, DeviceObj)
  External(\_SB.SCK0.CP08, DeviceObj)
  External(\_SB.SCK0.CP09, DeviceObj)
  External(\_SB.SCK0.CP0A, DeviceObj)
  External(\_SB.SCK0.CP0B, DeviceObj)
  External(\_SB.SCK0.CP0C, DeviceObj)
  External(\_SB.SCK0.CP0D, DeviceObj)
  External(\_SB.SCK0.CP0E, DeviceObj)
  External(\_SB.SCK0.CP0F, DeviceObj)
  External(\_SB.SCK0.CP10, DeviceObj)
  External(\_SB.SCK0.CP11, DeviceObj)
  External(\_SB.SCK0.CP12, DeviceObj)
  External(\_SB.SCK0.CP13, DeviceObj)
  External(\_SB.SCK0.CP14, DeviceObj)
  External(\_SB.SCK0.CP15, DeviceObj)
  External(\_SB.SCK0.CP16, DeviceObj)
  External(\_SB.SCK0.CP17, DeviceObj)
  External(\_SB.SCK0.CP18, DeviceObj)
  External(\_SB.SCK0.CP19, DeviceObj)
  External(\_SB.SCK0.CP1A, DeviceObj)
  External(\_SB.SCK0.CP1B, DeviceObj)
  External(\_SB.SCK0.CP1C, DeviceObj)
  External(\_SB.SCK0.CP1D, DeviceObj)
  External(\_SB.SCK0.CP1E, DeviceObj)
  External(\_SB.SCK0.CP1F, DeviceObj)
  External(\_SB.SCK0.CP20, DeviceObj)
  External(\_SB.SCK0.CP21, DeviceObj)
  External(\_SB.SCK0.CP22, DeviceObj)
  External(\_SB.SCK0.CP23, DeviceObj)
  External(\_SB.SCK0.CP24, DeviceObj)
  External(\_SB.SCK0.CP25, DeviceObj)
  External(\_SB.SCK0.CP26, DeviceObj)
  External(\_SB.SCK0.CP27, DeviceObj)
  External(\_SB.SCK0.CP28, DeviceObj)
  External(\_SB.SCK0.CP29, DeviceObj)
  External(\_SB.SCK0.CP2A, DeviceObj)
  External(\_SB.SCK0.CP2B, DeviceObj)
  External(\_SB.SCK0.CP2C, DeviceObj)
  External(\_SB.SCK0.CP2D, DeviceObj)
  External(\_SB.SCK0.CP2E, DeviceObj)
  External(\_SB.SCK0.CP2F, DeviceObj)
  External(\_SB.SCK0.CP30, DeviceObj)
  External(\_SB.SCK0.CP31, DeviceObj)
  External(\_SB.SCK0.CP32, DeviceObj)
  External(\_SB.SCK0.CP33, DeviceObj)
  External(\_SB.SCK0.CP34, DeviceObj)
  External(\_SB.SCK0.CP35, DeviceObj)
  External(\_SB.SCK0.CP36, DeviceObj)
  External(\_SB.SCK0.CP37, DeviceObj)

#if MAX_SOCKET > 1
  External(\_SB.SCK1, DeviceObj)
  External(\_SB.SCK1.CP00, DeviceObj)
  External(\_SB.SCK1.CP01, DeviceObj)
  External(\_SB.SCK1.CP02, DeviceObj)
  External(\_SB.SCK1.CP03, DeviceObj)
  External(\_SB.SCK1.CP04, DeviceObj)
  External(\_SB.SCK1.CP05, DeviceObj)
  External(\_SB.SCK1.CP06, DeviceObj)
  External(\_SB.SCK1.CP07, DeviceObj)
  External(\_SB.SCK1.CP08, DeviceObj)
  External(\_SB.SCK1.CP09, DeviceObj)
  External(\_SB.SCK1.CP0A, DeviceObj)
  External(\_SB.SCK1.CP0B, DeviceObj)
  External(\_SB.SCK1.CP0C, DeviceObj)
  External(\_SB.SCK1.CP0D, DeviceObj)
  External(\_SB.SCK1.CP0E, DeviceObj)
  External(\_SB.SCK1.CP0F, DeviceObj)
  External(\_SB.SCK1.CP10, DeviceObj)
  External(\_SB.SCK1.CP11, DeviceObj)
  External(\_SB.SCK1.CP12, DeviceObj)
  External(\_SB.SCK1.CP13, DeviceObj)
  External(\_SB.SCK1.CP14, DeviceObj)
  External(\_SB.SCK1.CP15, DeviceObj)
  External(\_SB.SCK1.CP16, DeviceObj)
  External(\_SB.SCK1.CP17, DeviceObj)
  External(\_SB.SCK1.CP18, DeviceObj)
  External(\_SB.SCK1.CP19, DeviceObj)
  External(\_SB.SCK1.CP1A, DeviceObj)
  External(\_SB.SCK1.CP1B, DeviceObj)
  External(\_SB.SCK1.CP1C, DeviceObj)
  External(\_SB.SCK1.CP1D, DeviceObj)
  External(\_SB.SCK1.CP1E, DeviceObj)
  External(\_SB.SCK1.CP1F, DeviceObj)
  External(\_SB.SCK1.CP20, DeviceObj)
  External(\_SB.SCK1.CP21, DeviceObj)
  External(\_SB.SCK1.CP22, DeviceObj)
  External(\_SB.SCK1.CP23, DeviceObj)
  External(\_SB.SCK1.CP24, DeviceObj)
  External(\_SB.SCK1.CP25, DeviceObj)
  External(\_SB.SCK1.CP26, DeviceObj)
  External(\_SB.SCK1.CP27, DeviceObj)
  External(\_SB.SCK1.CP28, DeviceObj)
  External(\_SB.SCK1.CP29, DeviceObj)
  External(\_SB.SCK1.CP2A, DeviceObj)
  External(\_SB.SCK1.CP2B, DeviceObj)
  External(\_SB.SCK1.CP2C, DeviceObj)
  External(\_SB.SCK1.CP2D, DeviceObj)
  External(\_SB.SCK1.CP2E, DeviceObj)
  External(\_SB.SCK1.CP2F, DeviceObj)
  External(\_SB.SCK1.CP30, DeviceObj)
  External(\_SB.SCK1.CP31, DeviceObj)
  External(\_SB.SCK1.CP32, DeviceObj)
  External(\_SB.SCK1.CP33, DeviceObj)
  External(\_SB.SCK1.CP34, DeviceObj)
  External(\_SB.SCK1.CP35, DeviceObj)
  External(\_SB.SCK1.CP36, DeviceObj)
  External(\_SB.SCK1.CP37, DeviceObj)
#endif

#if MAX_SOCKET > 2
  External(\_SB.SCK2, DeviceObj)
  External(\_SB.SCK2.CP00, DeviceObj)
  External(\_SB.SCK2.CP01, DeviceObj)
  External(\_SB.SCK2.CP02, DeviceObj)
  External(\_SB.SCK2.CP03, DeviceObj)
  External(\_SB.SCK2.CP04, DeviceObj)
  External(\_SB.SCK2.CP05, DeviceObj)
  External(\_SB.SCK2.CP06, DeviceObj)
  External(\_SB.SCK2.CP07, DeviceObj)
  External(\_SB.SCK2.CP08, DeviceObj)
  External(\_SB.SCK2.CP09, DeviceObj)
  External(\_SB.SCK2.CP0A, DeviceObj)
  External(\_SB.SCK2.CP0B, DeviceObj)
  External(\_SB.SCK2.CP0C, DeviceObj)
  External(\_SB.SCK2.CP0D, DeviceObj)
  External(\_SB.SCK2.CP0E, DeviceObj)
  External(\_SB.SCK2.CP0F, DeviceObj)
  External(\_SB.SCK2.CP10, DeviceObj)
  External(\_SB.SCK2.CP11, DeviceObj)
  External(\_SB.SCK2.CP12, DeviceObj)
  External(\_SB.SCK2.CP13, DeviceObj)
  External(\_SB.SCK2.CP14, DeviceObj)
  External(\_SB.SCK2.CP15, DeviceObj)
  External(\_SB.SCK2.CP16, DeviceObj)
  External(\_SB.SCK2.CP17, DeviceObj)
  External(\_SB.SCK2.CP18, DeviceObj)
  External(\_SB.SCK2.CP19, DeviceObj)
  External(\_SB.SCK2.CP1A, DeviceObj)
  External(\_SB.SCK2.CP1B, DeviceObj)
  External(\_SB.SCK2.CP1C, DeviceObj)
  External(\_SB.SCK2.CP1D, DeviceObj)
  External(\_SB.SCK2.CP1E, DeviceObj)
  External(\_SB.SCK2.CP1F, DeviceObj)
  External(\_SB.SCK2.CP20, DeviceObj)
  External(\_SB.SCK2.CP21, DeviceObj)
  External(\_SB.SCK2.CP22, DeviceObj)
  External(\_SB.SCK2.CP23, DeviceObj)
  External(\_SB.SCK2.CP24, DeviceObj)
  External(\_SB.SCK2.CP25, DeviceObj)
  External(\_SB.SCK2.CP26, DeviceObj)
  External(\_SB.SCK2.CP27, DeviceObj)
  External(\_SB.SCK2.CP28, DeviceObj)
  External(\_SB.SCK2.CP29, DeviceObj)
  External(\_SB.SCK2.CP2A, DeviceObj)
  External(\_SB.SCK2.CP2B, DeviceObj)
  External(\_SB.SCK2.CP2C, DeviceObj)
  External(\_SB.SCK2.CP2D, DeviceObj)
  External(\_SB.SCK2.CP2E, DeviceObj)
  External(\_SB.SCK2.CP2F, DeviceObj)
  External(\_SB.SCK2.CP30, DeviceObj)
  External(\_SB.SCK2.CP31, DeviceObj)
  External(\_SB.SCK2.CP32, DeviceObj)
  External(\_SB.SCK2.CP33, DeviceObj)
  External(\_SB.SCK2.CP34, DeviceObj)
  External(\_SB.SCK2.CP35, DeviceObj)
  External(\_SB.SCK2.CP36, DeviceObj)
  External(\_SB.SCK2.CP37, DeviceObj)
#endif

#if MAX_SOCKET > 3
  External(\_SB.SCK3, DeviceObj)
  External(\_SB.SCK3.CP00, DeviceObj)
  External(\_SB.SCK3.CP01, DeviceObj)
  External(\_SB.SCK3.CP02, DeviceObj)
  External(\_SB.SCK3.CP03, DeviceObj)
  External(\_SB.SCK3.CP04, DeviceObj)
  External(\_SB.SCK3.CP05, DeviceObj)
  External(\_SB.SCK3.CP06, DeviceObj)
  External(\_SB.SCK3.CP07, DeviceObj)
  External(\_SB.SCK3.CP08, DeviceObj)
  External(\_SB.SCK3.CP09, DeviceObj)
  External(\_SB.SCK3.CP0A, DeviceObj)
  External(\_SB.SCK3.CP0B, DeviceObj)
  External(\_SB.SCK3.CP0C, DeviceObj)
  External(\_SB.SCK3.CP0D, DeviceObj)
  External(\_SB.SCK3.CP0E, DeviceObj)
  External(\_SB.SCK3.CP0F, DeviceObj)
  External(\_SB.SCK3.CP10, DeviceObj)
  External(\_SB.SCK3.CP11, DeviceObj)
  External(\_SB.SCK3.CP12, DeviceObj)
  External(\_SB.SCK3.CP13, DeviceObj)
  External(\_SB.SCK3.CP14, DeviceObj)
  External(\_SB.SCK3.CP15, DeviceObj)
  External(\_SB.SCK3.CP16, DeviceObj)
  External(\_SB.SCK3.CP17, DeviceObj)
  External(\_SB.SCK3.CP18, DeviceObj)
  External(\_SB.SCK3.CP19, DeviceObj)
  External(\_SB.SCK3.CP1A, DeviceObj)
  External(\_SB.SCK3.CP1B, DeviceObj)
  External(\_SB.SCK3.CP1C, DeviceObj)
  External(\_SB.SCK3.CP1D, DeviceObj)
  External(\_SB.SCK3.CP1E, DeviceObj)
  External(\_SB.SCK3.CP1F, DeviceObj)
  External(\_SB.SCK3.CP20, DeviceObj)
  External(\_SB.SCK3.CP21, DeviceObj)
  External(\_SB.SCK3.CP22, DeviceObj)
  External(\_SB.SCK3.CP23, DeviceObj)
  External(\_SB.SCK3.CP24, DeviceObj)
  External(\_SB.SCK3.CP25, DeviceObj)
  External(\_SB.SCK3.CP26, DeviceObj)
  External(\_SB.SCK3.CP27, DeviceObj)
  External(\_SB.SCK3.CP28, DeviceObj)
  External(\_SB.SCK3.CP29, DeviceObj)
  External(\_SB.SCK3.CP2A, DeviceObj)
  External(\_SB.SCK3.CP2B, DeviceObj)
  External(\_SB.SCK3.CP2C, DeviceObj)
  External(\_SB.SCK3.CP2D, DeviceObj)
  External(\_SB.SCK3.CP2E, DeviceObj)
  External(\_SB.SCK3.CP2F, DeviceObj)
  External(\_SB.SCK3.CP30, DeviceObj)
  External(\_SB.SCK3.CP31, DeviceObj)
  External(\_SB.SCK3.CP32, DeviceObj)
  External(\_SB.SCK3.CP33, DeviceObj)
  External(\_SB.SCK3.CP34, DeviceObj)
  External(\_SB.SCK3.CP35, DeviceObj)
  External(\_SB.SCK3.CP36, DeviceObj)
  External(\_SB.SCK3.CP37, DeviceObj)
#endif

#if MAX_SOCKET > 4
  External(\_SB.SCK4, DeviceObj)
  External(\_SB.SCK4.CP00, DeviceObj)
  External(\_SB.SCK4.CP01, DeviceObj)
  External(\_SB.SCK4.CP02, DeviceObj)
  External(\_SB.SCK4.CP03, DeviceObj)
  External(\_SB.SCK4.CP04, DeviceObj)
  External(\_SB.SCK4.CP05, DeviceObj)
  External(\_SB.SCK4.CP06, DeviceObj)
  External(\_SB.SCK4.CP07, DeviceObj)
  External(\_SB.SCK4.CP08, DeviceObj)
  External(\_SB.SCK4.CP09, DeviceObj)
  External(\_SB.SCK4.CP0A, DeviceObj)
  External(\_SB.SCK4.CP0B, DeviceObj)
  External(\_SB.SCK4.CP0C, DeviceObj)
  External(\_SB.SCK4.CP0D, DeviceObj)
  External(\_SB.SCK4.CP0E, DeviceObj)
  External(\_SB.SCK4.CP0F, DeviceObj)
  External(\_SB.SCK4.CP10, DeviceObj)
  External(\_SB.SCK4.CP11, DeviceObj)
  External(\_SB.SCK4.CP12, DeviceObj)
  External(\_SB.SCK4.CP13, DeviceObj)
  External(\_SB.SCK4.CP14, DeviceObj)
  External(\_SB.SCK4.CP15, DeviceObj)
  External(\_SB.SCK4.CP16, DeviceObj)
  External(\_SB.SCK4.CP17, DeviceObj)
  External(\_SB.SCK4.CP18, DeviceObj)
  External(\_SB.SCK4.CP19, DeviceObj)
  External(\_SB.SCK4.CP1A, DeviceObj)
  External(\_SB.SCK4.CP1B, DeviceObj)
  External(\_SB.SCK4.CP1C, DeviceObj)
  External(\_SB.SCK4.CP1D, DeviceObj)
  External(\_SB.SCK4.CP1E, DeviceObj)
  External(\_SB.SCK4.CP1F, DeviceObj)
  External(\_SB.SCK4.CP20, DeviceObj)
  External(\_SB.SCK4.CP21, DeviceObj)
  External(\_SB.SCK4.CP22, DeviceObj)
  External(\_SB.SCK4.CP23, DeviceObj)
  External(\_SB.SCK4.CP24, DeviceObj)
  External(\_SB.SCK4.CP25, DeviceObj)
  External(\_SB.SCK4.CP26, DeviceObj)
  External(\_SB.SCK4.CP27, DeviceObj)
  External(\_SB.SCK4.CP28, DeviceObj)
  External(\_SB.SCK4.CP29, DeviceObj)
  External(\_SB.SCK4.CP2A, DeviceObj)
  External(\_SB.SCK4.CP2B, DeviceObj)
  External(\_SB.SCK4.CP2C, DeviceObj)
  External(\_SB.SCK4.CP2D, DeviceObj)
  External(\_SB.SCK4.CP2E, DeviceObj)
  External(\_SB.SCK4.CP2F, DeviceObj)
  External(\_SB.SCK4.CP30, DeviceObj)
  External(\_SB.SCK4.CP31, DeviceObj)
  External(\_SB.SCK4.CP32, DeviceObj)
  External(\_SB.SCK4.CP33, DeviceObj)
  External(\_SB.SCK4.CP34, DeviceObj)
  External(\_SB.SCK4.CP35, DeviceObj)
  External(\_SB.SCK4.CP36, DeviceObj)
  External(\_SB.SCK4.CP37, DeviceObj)
#endif

#if MAX_SOCKET > 5
  External(\_SB.SCK5, DeviceObj)
  External(\_SB.SCK5.CP00, DeviceObj)
  External(\_SB.SCK5.CP01, DeviceObj)
  External(\_SB.SCK5.CP02, DeviceObj)
  External(\_SB.SCK5.CP03, DeviceObj)
  External(\_SB.SCK5.CP04, DeviceObj)
  External(\_SB.SCK5.CP05, DeviceObj)
  External(\_SB.SCK5.CP06, DeviceObj)
  External(\_SB.SCK5.CP07, DeviceObj)
  External(\_SB.SCK5.CP08, DeviceObj)
  External(\_SB.SCK5.CP09, DeviceObj)
  External(\_SB.SCK5.CP0A, DeviceObj)
  External(\_SB.SCK5.CP0B, DeviceObj)
  External(\_SB.SCK5.CP0C, DeviceObj)
  External(\_SB.SCK5.CP0D, DeviceObj)
  External(\_SB.SCK5.CP0E, DeviceObj)
  External(\_SB.SCK5.CP0F, DeviceObj)
  External(\_SB.SCK5.CP10, DeviceObj)
  External(\_SB.SCK5.CP11, DeviceObj)
  External(\_SB.SCK5.CP12, DeviceObj)
  External(\_SB.SCK5.CP13, DeviceObj)
  External(\_SB.SCK5.CP14, DeviceObj)
  External(\_SB.SCK5.CP15, DeviceObj)
  External(\_SB.SCK5.CP16, DeviceObj)
  External(\_SB.SCK5.CP17, DeviceObj)
  External(\_SB.SCK5.CP18, DeviceObj)
  External(\_SB.SCK5.CP19, DeviceObj)
  External(\_SB.SCK5.CP1A, DeviceObj)
  External(\_SB.SCK5.CP1B, DeviceObj)
  External(\_SB.SCK5.CP1C, DeviceObj)
  External(\_SB.SCK5.CP1D, DeviceObj)
  External(\_SB.SCK5.CP1E, DeviceObj)
  External(\_SB.SCK5.CP1F, DeviceObj)
  External(\_SB.SCK5.CP20, DeviceObj)
  External(\_SB.SCK5.CP21, DeviceObj)
  External(\_SB.SCK5.CP22, DeviceObj)
  External(\_SB.SCK5.CP23, DeviceObj)
  External(\_SB.SCK5.CP24, DeviceObj)
  External(\_SB.SCK5.CP25, DeviceObj)
  External(\_SB.SCK5.CP26, DeviceObj)
  External(\_SB.SCK5.CP27, DeviceObj)
  External(\_SB.SCK5.CP28, DeviceObj)
  External(\_SB.SCK5.CP29, DeviceObj)
  External(\_SB.SCK5.CP2A, DeviceObj)
  External(\_SB.SCK5.CP2B, DeviceObj)
  External(\_SB.SCK5.CP2C, DeviceObj)
  External(\_SB.SCK5.CP2D, DeviceObj)
  External(\_SB.SCK5.CP2E, DeviceObj)
  External(\_SB.SCK5.CP2F, DeviceObj)
  External(\_SB.SCK5.CP30, DeviceObj)
  External(\_SB.SCK5.CP31, DeviceObj)
  External(\_SB.SCK5.CP32, DeviceObj)
  External(\_SB.SCK5.CP33, DeviceObj)
  External(\_SB.SCK5.CP34, DeviceObj)
  External(\_SB.SCK5.CP35, DeviceObj)
  External(\_SB.SCK5.CP36, DeviceObj)
  External(\_SB.SCK5.CP37, DeviceObj)
#endif

#if MAX_SOCKET > 6
  External(\_SB.SCK6, DeviceObj)
  External(\_SB.SCK6.CP00, DeviceObj)
  External(\_SB.SCK6.CP01, DeviceObj)
  External(\_SB.SCK6.CP02, DeviceObj)
  External(\_SB.SCK6.CP03, DeviceObj)
  External(\_SB.SCK6.CP04, DeviceObj)
  External(\_SB.SCK6.CP05, DeviceObj)
  External(\_SB.SCK6.CP06, DeviceObj)
  External(\_SB.SCK6.CP07, DeviceObj)
  External(\_SB.SCK6.CP08, DeviceObj)
  External(\_SB.SCK6.CP09, DeviceObj)
  External(\_SB.SCK6.CP0A, DeviceObj)
  External(\_SB.SCK6.CP0B, DeviceObj)
  External(\_SB.SCK6.CP0C, DeviceObj)
  External(\_SB.SCK6.CP0D, DeviceObj)
  External(\_SB.SCK6.CP0E, DeviceObj)
  External(\_SB.SCK6.CP0F, DeviceObj)
  External(\_SB.SCK6.CP10, DeviceObj)
  External(\_SB.SCK6.CP11, DeviceObj)
  External(\_SB.SCK6.CP12, DeviceObj)
  External(\_SB.SCK6.CP13, DeviceObj)
  External(\_SB.SCK6.CP14, DeviceObj)
  External(\_SB.SCK6.CP15, DeviceObj)
  External(\_SB.SCK6.CP16, DeviceObj)
  External(\_SB.SCK6.CP17, DeviceObj)
  External(\_SB.SCK6.CP18, DeviceObj)
  External(\_SB.SCK6.CP19, DeviceObj)
  External(\_SB.SCK6.CP1A, DeviceObj)
  External(\_SB.SCK6.CP1B, DeviceObj)
  External(\_SB.SCK6.CP1C, DeviceObj)
  External(\_SB.SCK6.CP1D, DeviceObj)
  External(\_SB.SCK6.CP1E, DeviceObj)
  External(\_SB.SCK6.CP1F, DeviceObj)
  External(\_SB.SCK6.CP20, DeviceObj)
  External(\_SB.SCK6.CP21, DeviceObj)
  External(\_SB.SCK6.CP22, DeviceObj)
  External(\_SB.SCK6.CP23, DeviceObj)
  External(\_SB.SCK6.CP24, DeviceObj)
  External(\_SB.SCK6.CP25, DeviceObj)
  External(\_SB.SCK6.CP26, DeviceObj)
  External(\_SB.SCK6.CP27, DeviceObj)
  External(\_SB.SCK6.CP28, DeviceObj)
  External(\_SB.SCK6.CP29, DeviceObj)
  External(\_SB.SCK6.CP2A, DeviceObj)
  External(\_SB.SCK6.CP2B, DeviceObj)
  External(\_SB.SCK6.CP2C, DeviceObj)
  External(\_SB.SCK6.CP2D, DeviceObj)
  External(\_SB.SCK6.CP2E, DeviceObj)
  External(\_SB.SCK6.CP2F, DeviceObj)
  External(\_SB.SCK6.CP30, DeviceObj)
  External(\_SB.SCK6.CP31, DeviceObj)
  External(\_SB.SCK6.CP32, DeviceObj)
  External(\_SB.SCK6.CP33, DeviceObj)
  External(\_SB.SCK6.CP34, DeviceObj)
  External(\_SB.SCK6.CP35, DeviceObj)
  External(\_SB.SCK6.CP36, DeviceObj)
  External(\_SB.SCK6.CP37, DeviceObj)
#endif

#if MAX_SOCKET > 7
  External(\_SB.SCK7, DeviceObj)
  External(\_SB.SCK7.CP00, DeviceObj)
  External(\_SB.SCK7.CP01, DeviceObj)
  External(\_SB.SCK7.CP02, DeviceObj)
  External(\_SB.SCK7.CP03, DeviceObj)
  External(\_SB.SCK7.CP04, DeviceObj)
  External(\_SB.SCK7.CP05, DeviceObj)
  External(\_SB.SCK7.CP06, DeviceObj)
  External(\_SB.SCK7.CP07, DeviceObj)
  External(\_SB.SCK7.CP08, DeviceObj)
  External(\_SB.SCK7.CP09, DeviceObj)
  External(\_SB.SCK7.CP0A, DeviceObj)
  External(\_SB.SCK7.CP0B, DeviceObj)
  External(\_SB.SCK7.CP0C, DeviceObj)
  External(\_SB.SCK7.CP0D, DeviceObj)
  External(\_SB.SCK7.CP0E, DeviceObj)
  External(\_SB.SCK7.CP0F, DeviceObj)
  External(\_SB.SCK7.CP10, DeviceObj)
  External(\_SB.SCK7.CP11, DeviceObj)
  External(\_SB.SCK7.CP12, DeviceObj)
  External(\_SB.SCK7.CP13, DeviceObj)
  External(\_SB.SCK7.CP14, DeviceObj)
  External(\_SB.SCK7.CP15, DeviceObj)
  External(\_SB.SCK7.CP16, DeviceObj)
  External(\_SB.SCK7.CP17, DeviceObj)
  External(\_SB.SCK7.CP18, DeviceObj)
  External(\_SB.SCK7.CP19, DeviceObj)
  External(\_SB.SCK7.CP1A, DeviceObj)
  External(\_SB.SCK7.CP1B, DeviceObj)
  External(\_SB.SCK7.CP1C, DeviceObj)
  External(\_SB.SCK7.CP1D, DeviceObj)
  External(\_SB.SCK7.CP1E, DeviceObj)
  External(\_SB.SCK7.CP1F, DeviceObj)
  External(\_SB.SCK7.CP20, DeviceObj)
  External(\_SB.SCK7.CP21, DeviceObj)
  External(\_SB.SCK7.CP22, DeviceObj)
  External(\_SB.SCK7.CP23, DeviceObj)
  External(\_SB.SCK7.CP24, DeviceObj)
  External(\_SB.SCK7.CP25, DeviceObj)
  External(\_SB.SCK7.CP26, DeviceObj)
  External(\_SB.SCK7.CP27, DeviceObj)
  External(\_SB.SCK7.CP28, DeviceObj)
  External(\_SB.SCK7.CP29, DeviceObj)
  External(\_SB.SCK7.CP2A, DeviceObj)
  External(\_SB.SCK7.CP2B, DeviceObj)
  External(\_SB.SCK7.CP2C, DeviceObj)
  External(\_SB.SCK7.CP2D, DeviceObj)
  External(\_SB.SCK7.CP2E, DeviceObj)
  External(\_SB.SCK7.CP2F, DeviceObj)
  External(\_SB.SCK7.CP30, DeviceObj)
  External(\_SB.SCK7.CP31, DeviceObj)
  External(\_SB.SCK7.CP32, DeviceObj)
  External(\_SB.SCK7.CP33, DeviceObj)
  External(\_SB.SCK7.CP34, DeviceObj)
  External(\_SB.SCK7.CP35, DeviceObj)
  External(\_SB.SCK7.CP36, DeviceObj)
  External(\_SB.SCK7.CP37, DeviceObj)
#endif
  
  External(\_SB.PC00, DeviceObj)
  External(\_SB.PC00.HEC1, DeviceObj)
  External(\_SB.PC00.HEC2, DeviceObj)
  
  Scope(\)
  {
#if PROCESSOR_AGGREGATOR
    Name(PURA, 0)
    Name(PURB, 0)
    Name(PURE, 0)
#endif
    Name(THNU, 0x800)
  }
  
  Scope(\_SB.PC00.HEC1)
  {
    //
    // HECI-1 HFS register
    // It holds ME Firmware Status #1
    //
    OperationRegion(MFS1, PCI_Config, 0x40, 4)
    Field(MFS1, DWordAcc, NoLock, Preserve)
    {
      MEFS, 32
    }
    //
    // Return HECI-1 device status according to HECI visibility on PCI bus.
    //
    Method(_STA, 0)
    {
      DbgTrace("H1: _STA")
      DbgTrace(MEFS)
      If (LEqual(MEFS, 0xFFFFFFFF)) // if HECI-1 hidden on disabled
      {
        Return(0x00)
      }
      Return(0x0F)
    }
  }

  Scope(\_SB.PC00.HEC2)
  {
    //
    // H2ST is HECI-2 device status and is checked by OSPM before _INI
    // is called. Initially it is defined as not present and it is
    // updated by SPS ACPI hooks according to true ME state.
    // If ME firmware is not operational, or NM is not enabled
    // HECI-2 is reported present but disabled. If ME is operational
    // HECI-2 is present and functioning, but hidden from UI.
    //
    Name(H2ST, 0x04)
    Method(_STA, 0)
    {
      DbgTrace("H2: _STA")
      DbgTrace(H2ST)
      Return(H2ST)
    }
    
    //
    // Define the region for HECI-2 registers mapped to HECI-2 MBAR.
    //
    //OperationRegion(HCSR, PCIBARTarget, 0x10, 0x10)
    // The PCIBARTarget is not implemented in several OSes.
    // The address below is just stub to be updated in UpdateAslCode().
    //
    OperationRegion(H2CS, SystemMemory, 0x8000000080000000, 0x10)
    Field(H2CS, DWordAcc, NoLock, Preserve)
    {
      CBWW,32, // write window - R_HECI_HOST_CB_WW
      // Bits of R_HECI_HOST_H_CSR (Host Control Status Register)
      HIE, 1,
      HIS, 1,
      HIG, 1,
      HRD, 1,
      HRS, 1,
         , 3,
      HRP, 8, // host read pointer
      HWP, 8, // host write pointer
      HBD, 8, // host buffer depth
      CBRW,32, // read window - R_HECI_ME_CB_RW
      // Bits of R_HECI_ME_CSR_HA (ME Control Status Register - Host Access)
      MIE, 1,
      MIS, 1,
      MIG, 1,
      MRD, 1,
      MRS, 1,
         , 3,
      MRP, 8, // ME read pointer
      MWP, 8, // ME write pointer
      MBD, 8  // ME buffer depth
    }
    
    //
    // Init method enables HECI-2 on OS boot
    //
    Method(_INI, 0, Serialized)
    {
      DbgTrace("H2: Init")
      If (LGreater(HWEN, 1))
      {
        DbgTrace("H2: Hardware P-states enabled without legacy support")
        Or(PETE, 0x01, PETE) // Set bit 0 to indicate Hardware P-states for ME
      }
      HWAK()
    }
    
    //
    // Wake-up handler method - called from platform _WAK() method
    //
    Method(HWAK, 0, Serialized)
    {
      DbgTrace("H2: Wake-up")
      If (And(H2ST, 0x08)) // If HECI-2 functional
      {
        Store(1, HRD) // set host ready
        Store(1, HIE) // set interrupt enable
        Store(1, HIG) // kick ME - I'm ready now
      }
    }
    
    //
    // Prepare-to-sleep handler method - called from platform _PTS() method
    //
    Method(HPTS, 0, Serialized)
    {
      DbgTrace("H2: Prepare-to-sleep")
      If (And(H2ST, 0x08)) // If HECI-2 functional
      {
        Store(0, HIE) // clear interrupts enable
        Store(0, HRD) //clear host ready
        Store(1, HIG) // kick ME - I'm not ready any more
      }
    }
    
    //
    // Send one dword message via HECI-2
    //
    // Arg0 - The body of the message to send.
    //
    Method(SEND, 1, Serialized)
    {
      DbgTrace("H2: Send:")
      DbgTrace(Arg0)
      If (LAnd(HRD, MRD))  // If host and ME side ready
      {
        Store(0x80040011, CBWW)
        Store(Arg0, CBWW)
        Store(1, HIG)
        Store(1, HIE)
      }
    }
    
    //
    // Trigger HECI-2 reset
    //
    Method(RSET, 0, Serialized)
    {
      DbgTrace("H2: Resetting HECI")
      Store(1, HIS)
      Store(1, HRS)
      Store(1, HIG)
      Store(1, HIE)
    }
    
    //
    // HECI-2 queue read method. It retrieves and dispatches ME requests.
    //
    Method(H2RD, 0, Serialized)
    {
      If (LNot(And(H2ST, 0x08))) // If HECI-2 not functional just return
      {
        Return(0xFF);
      }
      If (MRS) // If reset request from ME - respond with host reset
      {
        DbgTrace("H2: HECI reset requested")
        RSET()
      }
      Else
      {
        If (MRD) // If ME is ready
        {
          If (LNot(HRD)) // ME signals ready, make this side ready too
          {
            DbgTrace("H2: HECI ready")
            Store(0, HRS)
            Store(1, HRD)
            Store(1, HIG)                    // kick ME - I'm ready now
          }
          If (LNotEqual(MWP, MRP))           // If buffer not empty
          {
            Store(CBRW, Local0)              // Read message header
            Store(CBRW, Local1)              // Read message DW1
            DbgTrace("H2: Got message:")
            DbgTrace(Local0)
            DbgTrace(Local1)
            Store(1, HIG)                    // message consumed kick ME - queue is not full
            
            And(Local0, 0x01FF0000, Local6)  // extract msg length and store in Local6
            And(Local1, 0x000000FF, Local7)  // extract msg command and store in Local7
            
            If (LEqual(Local7, 0x00))        // if performance change request
            {
              If (LEqual(Local6, 0x00040000)) // If it is 4 bytes long legacy message
              {
                DbgTrace("H2: Handle Performance Change legacy msg")
                //
                // Copy P-state and T-state to Local2 and Local3 to allow handling it
                // as if it was the new message format.
                //
                And(Local1, 0x00FF0000, Local2)
                Or(Local2,  ShiftLeft(And(Local1, 0x00FF0000), 8), Local2)
                Or(Local2, ShiftRight(And(Local1, 0x00FF0000), 8), Local2)
                Or(Local2, ShiftRight(And(Local1, 0x00FF0000),16), Local2)
                Store(Local2, Local3)
                And(Local1, 0xFF000000, Local4)
                Or(Local4, ShiftRight(And(Local1, 0xFF000000), 8), Local4)
                Or(Local4, ShiftRight(And(Local1, 0xFF000000),16), Local4)
                Or(Local4, ShiftRight(And(Local1, 0xFF000000),24), Local4)
                Store(Local4, Local5)
              }
              ElseIf (LEqual(Local6, 0x00140000)) // if new 20 bytes long message
              {
                DbgTrace("H2: Handle Performance Change")
                Store(CBRW, Local2)
                Store(CBRW, Local3)
                Store(CBRW, Local4)
                Store(CBRW, Local5)
              }
              Else
              {
                DbgTrace("H2: Unknown message, resetting HECI")
                RSET()
                Return (0xFF)
              }
              If (And(\PETE, 0x80))
              {
                //
                // If P-states supported update current P-state limit
                //
                DbgTrace("H2: Notify 80 to processors")
                And(           Local2    , 0xFF, Index(\PSLM, 0))
                And(ShiftRight(Local2, 8), 0xFF, Index(\PSLM, 1))
                And(ShiftRight(Local2,16), 0xFF, Index(\PSLM, 2))
                And(ShiftRight(Local2,24), 0xFF, Index(\PSLM, 3))
                And(           Local3    , 0xFF, Index(\PSLM, 4))
                And(ShiftRight(Local3, 8), 0xFF, Index(\PSLM, 5))
                And(ShiftRight(Local3,16), 0xFF, Index(\PSLM, 6))
                And(ShiftRight(Local3,24), 0xFF, Index(\PSLM, 7))
                //
                // Notify processor objects to recalculate limits
                //
                Notify80Socket0()
                Notify80Socket1()
                Notify80Socket2()
                Notify80Socket3()
                Notify80Socket4()
                Notify80Socket5()
                Notify80Socket6()
                Notify80Socket7()
              }
              If (And(\PETE, 0x40))
              {
                //
                // If T-states supported update current T-state limit
                //
                DbgTrace("H2: Notify 82 to processors")
                And(           Local4    , 0xFF, Index(\TSLM, 0))
                And(ShiftRight(Local4, 8), 0xFF, Index(\TSLM, 1))
                And(ShiftRight(Local4,16), 0xFF, Index(\TSLM, 2))
                And(ShiftRight(Local4,24), 0xFF, Index(\TSLM, 3))
                And(           Local5    , 0xFF, Index(\TSLM, 4))
                And(ShiftRight(Local5, 8), 0xFF, Index(\TSLM, 5))
                And(ShiftRight(Local5,16), 0xFF, Index(\TSLM, 6))
                And(ShiftRight(Local5,24), 0xFF, Index(\TSLM, 7))
                //
                // Notify processor objects to recalculate limit
                //
                Notify82Socket0()
                Notify82Socket1()
                Notify82Socket2()
                Notify82Socket3()
                Notify82Socket4()
                Notify82Socket5()
                Notify82Socket6()
                Notify82Socket7()
              }
              //
              // Send ACK for P/T-state message
              //
              SEND(Or(Local1, \PETE))
            } // If cmd is HECI_CMD_MAXPTSTATE
#if PROCESSOR_AGGREGATOR
            ElseIf (LAnd(LEqual(Local7, 0x03), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Processor Utilisation req")
              If (\PURE)
              {
                Store(Local1, \PURA)
                Notify(\_SB.PRAD, 0x80)
              }
              else
              {
                SEND(Local1)
              }
            }
#endif
#if POWER_METER
            ElseIf (LAnd(LEqual(Local7, 0x04), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Measurement req")
              Store(Multiply(And(ShiftRight(Local1, 0x10), 0xFFFF), 1000), PWR)
              SEND(Or(Local1, 0x80))
            }
            ElseIf (LAnd(LEqual(Local7, 0x05), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Get Averaging Interval req")
              Store(Multiply(And(ShiftRight(Local1, 0x10), 0xFFFF), 1000), PMAI)
              SEND(Or(Local1, 0x80))
              Notify(\_SB.PMT1, 0x84)
            }
            ElseIf (LAnd(LEqual(Local7, 0x06), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Get HW Limit req")
              Store(Multiply(And(ShiftRight(Local1, 0x10), 0xFFFF), 1000), HWL)
              SEND(Or(Local1, 0x80))
              Notify(\_SB.PMT1, 0x82)
            }
            ElseIf (LAnd(LEqual(Local7, 0x0C), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Power State Change req")
              //
              // ACK: Rsvd:Active:SeqNo:Command
              //
              SEND(Or(ShiftLeft(PMAV, 0x10), And(Or(Local1, 0x80), 0xFFFF)))
              If (LEqual(1, PMAV)) // If Power Meter is active
              {
                // capabilities possible change
                If (LEqual(And(ShiftRight(Local1, 0x10), 0xFF), 1))
                {
                  \_SB.PC00.HEC2.SSFW(0, 0x08, 0, 0)
                }
                Store(Divide(HWL, 1000), Local2)
                \_SB.PC00.HEC2.SSFW(0, 0x07, Local2, 0) // Set HWL
                Store(Divide(PMAI, 1000), Local2)
                \_SB.PC00.HEC2.SSFW(0, 0x0A, Local2, 0) // Set PAI
                Store(Divide(PTPU, 1000), Local2)
                Store(Divide(PTPL, 1000), Local3)
                \_SB.PC00.HEC2.SSFW(1, 0x0B, Local2, Local3) // Set PAI
              }
            }
            // if Get Current Capabilities ACK, or Report Capabilities REQ
            ElseIf (LAnd(Or(LEqual(Local7, 0x88),
                             LEqual(Local7, 0x0D)), LEqual(Local6, 0x00200000)))
            {
              DbgTrace("H2: Handle Get Current Capabilities ack or Report Capabilities req")
              Store(And(ShiftRight(Local1, 0x18), 0xFF), Index(PMCP, 0)) // supported capabilities
              Store(And(ShiftRight(Local1, 0x88), 0x01), Index(PMCP, 2)) // measurement type
              If (LEqual(And(ShiftRight(Local1, 0x10), 0x01), 0x01))    // Is hw limit configurable
              {
                Store(0xFFFFFFFF, Index(PMCP, 8))
              }
              Else
              {
                Store(0x00000000, Index(PMCP, 8))
              }
              Store(CBRW, Index(PMCP, 3))
              Store(CBRW, Index(PMCP, 4))
              Store(CBRW, Index(PMCP, 5))
              Store(CBRW, Index(PMCP, 6))
              Store(CBRW, Index(PMCP, 7))
              Store(CBRW, Index(PMCP, 9))
              Store(CBRW, Index(PMCP, 10))
              If (LEqual(Local7, 0x0D))
              {
                SEND(And(Or(Local1, 0x80), 0xFFFF))
              }
              Else
              {
                Store(1, HIG)
              }
              Store(1, PMAV)
              Notify(\_SB.PMT1, 0x80)
            }
            ElseIf (LAnd(LEqual(Local7, 0x09), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Power Trip Points exceeded req")
              SEND(Or(Local1, 0x80))
              Notify(\_SB.PMT1, 0x81)
            }
            ElseIf (LAnd(LEqual(Local7, 0x87), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Set Hardware Limit ack")
              Store(And(ShiftRight(Local1, 0x08), 0xFF), HLST)
            }
            ElseIf (LAnd(LEqual(Local7, 0x8A), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Power Averaging Interval ack")
              Store(And(ShiftRight(Local1, 0x08), 0xFF), AIST)
            }
            ElseIf (LAnd(LEqual(Local7, 0x8B), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Power Trip Points ack")
              Store(And(ShiftRight(Local1, 0x08), 0xFF), TPST)
            }
#endif // POWER_METER
            Else
            {
              DbgTrace("H2: Unknown message, resetting HECI")
              RSET()
              Return (0xFF)
            }
            Return(Local7) // The command recently handled is returned by H2RD
          } // If (buffer not empty)
        } // If (MRD)
      } // if (MRS )
      Return(0xFF) // There is no command to return
    } // Method(H2RD,...)
    
    //
    // SCI handler method - called from _L24() SCI handler
    // 
    Method(HSCI, 0, Serialized)
    {
      DbgTrace("H2: SCI Handler for HECI-2")
      If (HIS) // If Host Interrupt Status is Set (when ME_IG is set to 1)
      {
        Store(1, HIS)    // clear interrupt status before reading queue
        Store(8, Local0) // set timeout counter
        While (LNotEqual(H2RD(), 0xFF))
        {
          Decrement(Local0)
          If (LNot(Local0))
          {
            //
            // Too many requests from ME, reset queue to slow it down.
            //
            DbgTrace("H2: Got 8 messages in a row, resetting HECI-2")
            RSET()
            Break
          }
        }
      } // If (HIS)
      //
      // If ME reset occurred during H2RD() HECI may be isolated.
      // We need to make sure HECI Interrupt Enable (HIE) is set
      // before leaving HECI SCI.
      //
      Store(300, Local0) // 3 seconds timeout
      While (LAnd(LNot(HIE), Local0))
      {
        Decrement(Local0)
        Sleep(10)
        Store(1, HIE)
      }
    } // Method(HSCI,...)

#if POWER_METER
    //
    // Send request to ME and wait for answer.
    // It sends to ME message frame of 1 or 2 DWORDs,
    // Message header plus depending on input parameter.
    // Arg0  Message size: 0 - 1 DWORD (_SHL, _PAI), 1 - 2 DWORDS (_PTP).
    // Arg1  Command.
    // Arg2  Setting param. Depending on command, it represents
    //         Hardware limit [W], or averaging interval [s], or upper trip point [W]
    // Arg3  Setting param in 2 DWORDs message. Lower trip point [sec].
    // 
    Method(SSFW, 4, Serialized)
    {
      If (LAnd(HRD, MRD))  // If host and ME side ready
      {
        Store(HIE, Local1) // Save HECI-2 SCI interrupt enable state
        Store(0, HIE)      // Disable HECI-2 SCI interrupt
        If (LEqual(Arg0, 0)) 
        {
          Store(0x80040011, CBWW)
          Store(Or(ShiftLeft(Arg2, 0x10), And(Arg1, 0x00FF)), CBWW)
        }
        ElseIf (LEqual(Arg0, 1))
        {
          Store(0x80080011, CBWW)
          Store(And (Arg1, 0xFF), CBWW)
          Store(Or(ShiftLeft(Arg3, 0x10), And(Arg2, 0xFFFF)), CBWW)
        }
        Store(1, HIG)
        Store(0, Local0)  // Timeout counter
        While (LNotEqual(Local0, 10))
        {
          If (LEqual(1, HIS))
          {
            Store(1, HIS) // Clear interrupt status
            If (LEqual(H2RD(), Or(Arg1, 0x80)))
            {
              Break
            }
          }
          Add(Local0, 1, Local0)
          Sleep(100)
        }
        Store(Local1, HIE)  // Restore HECI-2 SCI interrupt enable state
      }
    }
#endif // POWER_METER
  } // Scope(\_SB.PC00.HEC2)

#if POWER_METER
  Scope (\_SB)
  {
    Name(PWR, 0xFFFFFFFF)  // Power reading
    Name(PMAI, 0xFFFFFFFF) // Power meter averaging interval
    Name(HWL, 0xFFFFFFFF)  // HW limit
    Name(PTPU, 0)          // Upper Trip Point
    Name(PTPL, 0)          // Lower Trip Point
    Name(PMAV, 0)          // Is Power Meter interface active
    Name(HLST, 0)          // Set HW Limit Status, return code
    Name(AIST, 0)          // Set Averaging Interval Status, return code
    Name(TPST, 0)          // Set Trip Points Status, return code
    
    // Power Meter capabilities
    Name(PMCP, Package()
    {
      0x00000000, // Supported capabilities, (from LSB) -> measurement, trip points, HW enforced limit, notifications when HW limit is enforced, 3 x reserved, report only when discharging
      0x00000000, // Measurement unit, mW only 
      0x00000000, // Measurement input power
      0x00000000, // Accuracy of PMT measurements, 95000 = 95%
      0xFFFFFFFF, // Sampling time of PMT in ms
      0x00000000, // Min averaging interval in ms
      0x00000000, // Max averaging interval in ms
      0xFFFFFFFF, // Hysteresis margin (mW)
      0x00000000, // Is HW limit configurable, 0x00000000 = read only, 0xFFFFFFFF = writeable
      0x00000000, // Min configurable HW limit (mW)
      0x00000000, // Max configurable HW limit (mW)
      "Intel(R) Node Manager",// PMT model number, empty string if not supported
      "",         // PMT serial number,  empty string if not supported
      "Meter measures total domain" // OEM Info
    })
    
    Device(PMT1) //Power Meter # 1
    {
      Name(_HID, "ACPI000D")
      Name(_UID,  0x00)      // Unique ID required if many power meters

      //
      // Device status handler method - called by OSPM before _INI.
      // Power Meter status follows HECI-2 status but is not hidden from UI.
      //
      Method(_STA, 0, NotSerialized)
      {
        Return(Or(\_SB.PC00.HEC2._STA, 0x04))
      }

      //
      // Power Meter Capabilities
      //
      Method(_PMC, 0, NotSerialized)
      {
        If (LEqual(PMAV, 0))
        {
          \_SB.PC00.HEC2.SSFW(0, 0x08, 0, 0)
        }
        Return(PMCP)
      }
      
      //
      // Power Meter Devices
      //
      Method(_PMD, 0, NotSerialized)
      {
        Return(Package(){\_SB})
      }
      
      //
      // Power Meter Measurement
      //
      Method(_PMM, 0, NotSerialized)
      {
        Return(PWR)
      }
      
      //
      // Power Meter Averaging Interval
      //
      Method(_PAI, 1, NotSerialized)
      {
        Store(Divide(Arg0, 1000), Local0)
        Store(0x00000002, AIST)
        \_SB.PC00.HEC2.SSFW(0, 0x0A, Local0, 0) 
        If (LEqual(And(AIST, 0xFF), 0x00))
        {
          Store(Arg0, PMAI)
          Notify(\_SB.PMT1, 0x84)
        }
        Return(AIST)
      }
      
      //
      // Get Averaging Interval
      //
      Method(_GAI, 0, NotSerialized)
      {
        Return(PMAI)
      }
      
      //
      // Power Trip Points
      //
      Method(_PTP, 2, NotSerialized)
      {
        Store(Divide(Arg0, 1000), Local0)
        //
        // Round lower trip point to higher value
        //
        Store(Add(Arg1, 500), Local1)
        Store(Divide(Arg1, 1000), Local1)
        Store(0x00000002, TPST)
        \_SB.PC00.HEC2.SSFW(1, 0x0B, Local0, Local1)
        If (LEqual(And(TPST, 0xFF), 0))
        {
          Store(Arg0, PTPU)
          Store(Arg1, PTPL)
        }
        Return (TPST)
      }
      
      //
      //Get Hardware Limit
      //
      Method(_GHL, 0, NotSerialized)
      {
        Return(HWL)
      }
      
      //
      // Set Hardware Limit
      //
      Method(_SHL, 1, NotSerialized)
      {
        Store(Divide(Arg0, 1000), Local0)
        Store(0x00000002, HLST)
        \_SB.PC00.HEC2.SSFW (0, 0x07, Local0, 0)
        If (LEqual(And(HLST, 0xFF), 0x00))
        {
          Store(Arg0, HWL)
          Notify(\_SB.PMT1, 0x83)
        }
        Return (HLST)
      }
    } // Device(PMT1)
  } // Scope(SB)
#endif // POWER_METER

#if PROCESSOR_AGGREGATOR
  Scope(\_SB)
  {
    //
    // Processor Aggregator Device
    //
    Device(PRAD)
    {
      Name(_HID, "ACPI000C")

      //
      // Device status handler method - called by OSPM before _INI.
      // If OSPM support Processor Aggregator Device its status follows HECI-2
      // status, but is not hidden from UI. If not then report "not present".
      //
      Method(_STA)
      {
        If (\_OSI("Processor Aggregator Device"))
        {
          DbgTrace("PRAD: OS does support Processor Aggregator Device")
          Store(1, \PURE)
          Return(Or(\_SB.PC00.HEC2._STA, 0x04))
        }
        Else
        {
          DbgTrace("PRAD: OS does NOT support Processor Aggregator Device")
          Return(0)
        }
      }

      //
      // Processor Utilization Request method returns the
      // number of processors that platform wants to idle.
      //
      Method(_PUR, 0, Serialized)
      {
        DbgTrace("PRAD: Entering _PUR")
        Name(IDL, Package() {1, 0})
        And(Or(\PURA, 0x80), 0xFFFF, \PURB)
        And(ShiftRight(\PURA, 16), 0xffff, Index(IDL, 1))
        Return(IDL)
      }

      //
      // OSPM Status Indication method
      //
      Method(_OST, 3, Serialized)
      {
        DbgTrace("PRAD: Entering _OST")
        //
        // If _OST is called after _PUR send ACK to ME.
        // Arg2 conveys number of threads currently idled.
        // Calculate number of active threads and send it to ME.
        //
        If (LEqual(And(\PURB, 0xff), 0x83))
        {
          Subtract(\THNU, ToInteger(Arg2), Local0)
          Or(\PURB, ShiftLeft(Local0, 16), \PURB)
          \_SB.PC00.HEC2.SEND(\PURB) // Send ACK for core idling
        }
      } // _OST
    } // Device PRAD
  } // Scope SB
#endif // PROCESSOR_AGGREGATOR

  Scope(\_GPE)
  {
    Method(_L6C, 0x0, Serialized)
    { 
      \_SB.PC00.HEC2.HSCI()
    }
  }
}// Definition Block

