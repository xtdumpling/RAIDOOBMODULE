//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
// $Header: /Alaska/BIN/Modules/ACPI/Template/Core/TimeAndAlarm.asl 1     1/23/14 5:25p Oleksiyy $
//
// $Revision: 1 $
//
// $Date: 1/23/14 5:25p $



        // !!! IMPORTANT !!! !!! IMPORTANT !!! !!! IMPORTANT !!! !!! IMPORTANT !!! 
        // This is an example of ACPI Time and Alarm Device implementation, proven to be working on Sharkbay platform. 
        // On different platforms this code may not work "as is" and may require light or heavy modification


        // ACPI RTC device
        //
        // Note: we assume 24-hr mode and date/time in BCD.

        Device(TAAD)
        {
            Name (_HID, "ACPI000E")
            Name (_CID, "ACPI000E")
            Name (_UID, 1)

            Name(PRWP, Package(){Zero, 4})             
            Method(_PRW, 0) { Return(PRWP) } // can wakeup from S4 state

            Name (TMZN, 2047)  // Timezone - set to 2047 to indicate value not specified. Will be used if no buffer for SMI was found
            Name (DAYL, 0)     // daylight - bit field to indicate daylight adjustment requirement. Will be used if no buffer for SMI was found
            Name (ALWS, 0xF)   // Init Alarm wake status - Stores alarm wake status; 0xF to indicate contents are invalid
          
            Name (ATBF, 0x100000000) //Adress of communication buffer
            Name (BUFU, 0x0) //Indicator that buffer was allocated and usable.
		
            //Mutex used to synchronize between methods	accessing RTC	
            Mutex(RTCL, 0) 

			// Region for SW SMI port
            OperationRegion (CSMI, SystemIO, SMIP , 0x1) 
            Field (CSMI, ByteAcc, NoLock, Preserve) 
            { 
                INQ,8
            } 

            OperationRegion (ADPS, SystemIO, PMBS, 4)
            Field (ADPS, ByteAcc,NoLock,Preserve) 
            {
                PMS, 16,
                PMEN, 16
            }

            // Regions for SW SMI data buffer.  Need two, one for byte access, one for nibble access.
            OperationRegion (DSMI, SystemMemory, ATBF, 0x4)
            Field (DSMI, AnyAcc, NoLock, Preserve) 
            { 
                BSTA, 8, 
                BDLY, 8,
                BTZN, 16 
            } 
            
    	    
            OperationRegion(VRTC, SystemCMOS, 0, 0x10)
            Field(VRTC, ByteAcc, Lock, Preserve) 
            {

                SEC, 8,
                SECA, 8, 
                
                MIN, 8,
                MINA, 8,
                
                HOR, 8,
                HORA, 8,
                
                DAYW, 8,                
                DAY, 8,
                
                MON, 8,
                YEAR, 8,

                STAA, 8,
                STAB, 8,
                STAC, 8,
                STAD, 8
            }

            Method(_S1W, 0) {
                return (0)
            }
            Method(_S2W, 0) {
                return (0)
            }
            Method(_S3W, 0) {
                return (3)
            }
            Method(_S4W, 0) {
                return (3)
            }

            Method(_DSW, 3) {

                // Disable the device's wake capabilities
                if( LEqual(Arg0, 0) ) {
                    And(PMEN, 0x0400, PMEN)
                }
                if( LEqual(Arg0, 1) ) {
                    if( LLessEqual(Arg1, 4) ){
                        Or(PMEN, 0x400, PMEN)
                    }                    
                }
                return
            }

            // RTC Allow Update
            Method (ALLW, 0x0, Serialized) {
                Acquire(RTCL, 2000)
                
                And(STAB, 0x7F, STAB)

                Release(RTCL)
                Return (One)
            }

            // RTC Inhibit Update
            Method (INHB, 0x00, Serialized) {
                Acquire(RTCL, 2000)
                Or(STAB, 0x80, STAB)
                Release(RTCL)
                Return (One)
            }

            Method (RTWT, 0x00) {
                Acquire(RTCL, 2000)

                // Loop until an update completes (the Update-Ended Flag (UF) is set)
                While( LNotEqual(And(STAC, 0x10), 0x10 )){
                    NoOp
                }

                Release(RTCL)
                return (One)
            }

            // Get Capability
            // This object is required and provides the OSPM with a bit mask of the device capabilities. The device 
            // can implement the time function inaddition to the wake function. The capabilities bitmask will 
            // indicate to the OSPM what support is implemented. If the platform implements both AC and DC 
            // timers then it is capable of waking up based on the power source

            // Only AC timer here, so it will be the wake source regardless of the power source	
            Method (_GCP, 0x0, NotSerialized)
            {
                Return (0x5) //Get/Set real time features implemented, real time accuracy in seconds and AC timer only;
            }
            
            
            // Get Real time Method
            // This object is required ifthe capabilities bit 2 is set to 1. The OSPM can use this object to get time. 
            // The return value is a buffer containing the time information
            Method (_GRT, 0x0, Serialized)
            {   
                Name(RBUF, Buffer(16) {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) //Buffer of 16 bytes - return value

                // Create fields to address different offsets in the buffer
                CreateWordField(RBUF, 0x0, TYAR)
                CreateByteField(RBUF, 0x2, TMON)
                CreateByteField(RBUF, 0x3, TDAY)
                CreateByteField(RBUF, 0x4, THOR)
                CreateByteField(RBUF, 0x5, TMIN)
                CreateByteField(RBUF, 0x6, TSEC)
                CreateByteField(RBUF, 0x7, TVAL)	// Time Valid if 1, not valid if 0
                CreateWordField(RBUF, 0xA, TZN)     // Timezone not supported through rRTC - return value stored in ASL
                CreateByteField(RBUF, 0xC, DYL)     // Daylight not supported through rRTC - emulate in ASL

                if (BUFU) 
                {
                    // Read value from NVRAM
                    Store (0x01, BSTA)
                    Store (ATAA, INQ)
                    Sleep (100)

                    if( LEqual(BSTA, 0x03 ) ) {
                        Store (BDLY, DAYL)
                        Store (BTZN, TMZN)
                    } else {
                        Store(2047, TMZN)
                        Store (0, DAYL)
                    }
                }

                RTWT()
		
                FromBCD(YEAR, TYAR)    // Get Year
                FromBCD (MON, TMON)    // Get Month
                FromBCD (DAY, TDAY)    // Get Day
                FromBCD (HOR, THOR)
                FromBCD (MIN, TMIN)
                FromBCD (SEC, TSEC)

                Store (TMZN, TZN)
                Store (DAYL, DYL)
                
                // we don't have centry byte
                ADD(TYAR, 2000, TYAR)

                Store (0x1, TVAL) // Data valid
                return (RBUF)
            }

            // Set Real Time
            // This object is required if the capabilities bit 2 is set to 1. The OSPM can use this object to set the 
            // time. The argument is a buffer containingthe time information

            Method (_SRT, 0x1, Serialized)
            {
                CreateWordField(Arg0, 0x0, TYAR)
                CreateByteField(Arg0, 0x2, TMON)
                CreateByteField(Arg0, 0x3, TDAY)
                CreateByteField(Arg0, 0x4, THOR)
                CreateByteField(Arg0, 0x5, TMIN)
                CreateByteField(Arg0, 0x6, TSEC)
                CreateWordField(Arg0, 0xA, TZN)     // Timezone not supported through rRTC - return value stored in ASL
                CreateByteField(Arg0, 0xC, DYL)     // Daylight not supported through rRTC
                
                if (BUFU) 
                {
                    // Send Daylight to NVRAM
                    Store (0x2, BSTA)
                    Store (DYL, BDLY)
                    Store (TZN, BTZN)

                    Store (ATAA, INQ)
                }

                RTWT()	

                ToBCD(Subtract(TYAR, 2000), YEAR)
                ToBCD (TMON, MON)   // Set month
                ToBCD (TDAY, DAY)   // Set day
                ToBCD (THOR, HOR)   // Set hour
                ToBCD (TMIN, MIN)   // Set minute
                ToBCD (TSEC, SEC)   // Set second

                Store (TZN, TMZN)
                Store (DYL, DAYL)

                If(LEqual(BSTA, 0x3)) // SMI handler set status to OK
                {
                    return (0x0) // return success
                }

                return (0xFFFFFFFF) // return error
            }

            // Get wake Status
            //This object is required and enables the OSPM to read the status ofwake alarms. Expired wake timers 
            //will wake the platform even if the transition to a sleep state was completed after the wake timer has 
            //expired. This method enables the OSPM to retrieve the status of wake timers and clear any of them if 
            //needed.

            Method (_GWS, 0x1, Serialized)
            {
                // From the ACPI specification
                // "Writes to the DC timer when AC only wake is supported will be ignored"
                if (LEqual(0x1, Arg0)) {
                    return (0x0)
                }
                
                // read alarm status from Register C and remember if it was set
                // if it wasn't set, keep alarm wake status field invalid
                // if it was set, indicate to the OS that the alarm expired and that it woke the system.
                // PORTING NOTE:  in some cases may be needed to found out if the alarm woke the system, 
                // for now assume it did (this may involve usage of SMI handlers.)

                if (LEqual(0xF, ALWS)) {
                    if (LAnd (STAC, 0x20)) {
                        Store (0x3, ALWS)
                        return (ALWS)
                    }
                }
                
                return (0x0)
            }	

            // Clear Wake alarm status.
            //This object is required and enables the OSPM to clear the status of wake alarms. Expired wake 
            //timers will wake the platform even if the transition to a sleep state was completed after the wake 
            //timer has expired. This method enables the OSPMto clear the status of expired wake timers.
            Method (_CWS, 0x1, Serialized) 
            { 
                // From the ACPI specification
                // "Writes to the DC timer when AC only wake is supported will be ignored"
                if (LEqual(1, Arg0)) {
                    return (0) 
                }
                
                RTWT()

                INHB()
                // Clear the alarm seconds, alarm minutes and alarm hours
                Store (0x00, SECA)
                Store (0x00, MINA)
                Store (0x00, HORA)
                ALLW()
                
                // Clear the "Date Alarm" in RTC_REGD (bits 5:0)
                And (STAD, 0x80, STAD)

                // Reading Register C should clear the "Alarm Flag (AF)" (Bit5)
                if (LEqual(0xF, ALWS)) {
                    Store (STAC, Local0)
                }

                // Disable the "Alarm Interrupt Enable" (AIE) bit in RTC_REGB     
                And(STAB, 0xDF, STAB)

                // Mark ALWS invalid
                Store (0xF, ALWS) 
                
                return (0x0)		
            }
            
            // Sets the timer wake policy. 
            
            //This object is required and sets the expired timer wake policy.The policy is applied when a 
            //corresponding timer expired but the wake signal was not asserted as a result of the power source. 
            //The platform accumulates elapsed time on the power source and asserts the wake signal when the 
            //elapsed timer on the power source exceeds the expired timer wake policy value. Power source 
            //transitions do not reset the expired timer wake policy values. When the Wake Alarm device asserts 
            //the wake, the expired timer wake policy values ofboth the AC timer and DC timer are reset to 
            //0xFFFFFFFF automatically by hardware. 

            Method (_STP, 0x2, Serialized) 
            {
                // From the ACPI specification
                // "Writes to the DC timer when AC only wake is supported will be ignored"
                if (LEqual(0x1, Arg0)) {
                    return (0)
                }

                //                 
                if (LGreater(Arg1, 0x01)) {
                    return (0x1)
                }

                return (0x0)
            }
           
            // Set the value in specified timer

            //This object is required and setsthe timer to the specified value. As defined in _TIV, the value 
            //indicates the number of seconds between the time when the timer is programmed and the time when 
            //it expires. When the Wake Alarm device asserts the wake signal, the timer value is automatically 
            //reset to 0xFFFFFFFF (disabled). 
            Method (_STV, 0x2, Serialized)
            {
                Name(THOR, 0)
                Name(TMIN, 0)
                Name(TSEC, 0)

                // From the ACPI specification
                // "Writes to the DC timer when AC only wake is supported will be ignored"
                if (LEqual(0x01, Arg0)) {
                    return (0x0)
                }

                
                // RTC hours/minutes/seconds cannot be set for more than 23 hours, 59 minutes
                //  and 59 seconds in the future, otherwise we would be setting the current time
                //  and the alarm would tigger immediatly
                if (LGreater(Arg1, 86399)) {
                    return (0x1)
                }
                
                //  Divide the time (which is passed in seconds) into hours, minutes and seconds
                Divide(Arg1, 60, TSEC, TMIN)
                Divide(TMIN, 60, TMIN, THOR)

                RTWT()
                // Add the current seconds to the number of seconds in the future to set the alarm
                //  and check if its greater than 59
                if( LGreater(Add(FromBCD(SEC), TSEC, TSEC), 59) ){
                    // It is greater than 59, so get the mod 60 and convert to BCD and store into the
                    //  the seconds alarm. Increment the TMIN too
                    ToBCD(Mod(TSEC, 60), Local1)
                    INCREMENT(TMIN)
                } else {
                    // Its less than 60, just conver to BCD and store it
                    ToBCD(TSEC, Local1)
                }
                Store(Local1, TSEC)

                // Add the current minutes to the number of minutes in the future
                //  and check if its greater than 59
                if( LGreater(Add(FromBCD(MIN), TMIN, TMIN), 59) ){
                    // It is greater than 59, so get the mod 60 and store it into 
                    //  the minutes alarm and increment the THORs
                    ToBCD(Mod(TMIN, 60), Local1)
                    INCREMENT(THOR)
                } else {
                    // Its less than 60, just conver to BCD and store it
                    ToBCD(TMIN, Local1)
                }
                Store(Local1, TMIN)
                
                // Add the current hours to the number of hours in the future
                //  and check if its greater than 24
                if( LGreater(Add(FromBCD(HOR), THOR, THOR), 23) ){
                    // It is greater than 63, so get the mod 24 and store it into 
                    //  the hours alarm
                    ToBCD(Mod(THOR, 60), Local1)
                } else {
                    ToBCD(THOR, Local1)
                }
                Store(Local1, THOR)

                // Inhibit updating and then program the Alarm
                INHB()
                Store(THOR, HORA)
                Store(TMIN, MINA)
                Store(TSEC, SECA)

                // Clear the Date Alarm in RTC_REGD (bits 5:0) to ensure they do not
                //  interfere the RTC alarm
                And (STAD, 0x80, STAD)

                // Write the Alarm Interrupt Enable (AIE) bit in RTC_REGB
                OR(STAB, 0x20, STAB) 
                ALLW()

                Or(PMEN, 0x0400, PMEN)
                Or(PMS,  0x0400, PMS)

                return (0x0)
            }

            // Get the timer wake policy of the specified timer
            //This object returns the current expired timer wake policy setting ofthe specified timer.

            // Ret Value is an Integer (DWORD) containing current expired timer wake policy:
            //0x00000000 - The timer will wake up the system instantly after the power source changes
            //0x00000001 - 0xFFFFFFFE: Time between the power source changes and the timer wakes up the 
            //system ( in units of seconds)
            //0xFFFFFFFF - The timer will never wake up the system after the power source changes

            Method (_TIP, 0x1, Serialized) {

                if( LEqual(Arg0, 1)) {
                    return (0xFFFFFFFF)
                }

                return (0x0)
            }            


            // Get the remaining timer value
            Method (_TIV, 0x1, Serialized)
            {
                Name(TTIM, 0x00000000)
                
                // If queried about the DC timer, return 
                if (LEqual(0x01, Arg0)) {
                    return (0xFFFFFFFF)
                }
 
                RTWT()
 
                if( LGreaterEqual(HORA, HOR) ) {
                    FromBCD(Subtract(HORA, HOR), Local0)
                } else {
                    Subtract(24, FromBCD(HOR), Local0)
                        Add(FromBCD(HORA), Local0, Local0)
                }
                Add(Multiply(Local0, 3600), TTIM, TTIM)

                if( LGreaterEqual(MINA, MIN) ) {
                    FromBCD(Subtract(MINA, MIN), Local0)
                } else {
                    Subtract(60, FromBCD(MIN), Local0)
                    Add(FromBCD(MINA), Local0, Local0)
                }
                Add(Multiply(Local0, 60), TTIM, TTIM)

                if( LGreaterEqual(SECA, SEC) ) {
                    FromBCD(Subtract(SECA, SEC), Local0)
                } else {
                    Subtract(60, FromBCD(SEC), Local0)
                    Add(FromBCD(SECA), Local0, Local0)
                }
                Add(Local0, TTIM, TTIM)
 
                return (TTIM)
            }
        }

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************