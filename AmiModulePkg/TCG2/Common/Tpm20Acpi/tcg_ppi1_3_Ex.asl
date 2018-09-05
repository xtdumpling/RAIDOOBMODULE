//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:    _DSM PPI Method for TPM device 
//
// Description:  Implement Phisical Presence Interface using WordAcc
//
// Input:        \_SB.PCI0.LPCB.TP
//
// Output:      PPI result
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
Scope(\_SB.TPM)
{    
    OperationRegion (TSMI, SystemIO, SMIA , 0x2) 
    Field (TSMI, WordAcc, NoLock, Preserve) 
    { 
        SMI,16,
    } 

    OperationRegion (ATNV, SystemMemory, PPIM, PPIL)
    Field (ATNV, AnyAcc, NoLock, Preserve)
    {
        RQST,   32, //   PPI Request
        RCNT,   32, //   Most Recent PPI request
        ERRO,   32, //   PPI operation Response
        FLAG,   32, //   Function Request flag
        MISC,   32, //   MISC
        OPTN,   32, //   Optional PPI Request Parameter
        SRSP,   32  //   PPI Function Request response
    }


    Method( _DSM , 4, Serialized)
    {
        if( LEqual(Arg0,ToUUID("3DDDFAA6-361B-4EB4-A424-8D10089D1653")))
        {
                switch(ToInteger(Arg2))            
                {
                    //
                    // Function 0: Return supported functons
                    //
                    case(0)
                    {
                        return (Buffer() {0xff,0x01}) //support functions 0-6                       
                    }

                    //
                    // Function 1: Ge PPI Version
                    //
                    case(1)
                    {
                        If (LEqual (PPIV, 0)) {
                             return ("1.2")                        
                        }Else{
                             return ("1.3")                        
                        }
                    }

                    //
                    // Function 2: Submit TPM Operation request
                    // Arg3[0]: Integer - Operation Value
                    case(2)
                    {
                        Store(DeRefOf(Index(Arg3,0)), RQST)  //save request in temp flag
                        Store (0, SRSP)
                        Store(0x02, FLAG)  
                        Store(OFST, TMF1)  
                        Store (0, SRSP)                            
                        Store(TMF1,SMI) 
                        return(SRSP)
                    }

                    //
                    // Function 3: Get pending TPM operation
                    case(3)
                    {
                        Name(PPI1, Package(){0,0})
                        Store(RQST, Index(PPI1,1))
                        return(PPI1)
                    }

                    //
                    // Function 4: Get platform-specific action to transition 
                    // ot Pre-OS
                    // Returns: 
                    // 0: None
                    // 1: Shutdown
                    // 2: Reboot
                    // 3: OS Vendor Specific
                    case(4)
                    {      
                        return (TRST) //Shutdown                       
                    }

                    //
                    // Function 5: Return TPM responce
                    //    
                    case(5)
                    {
                        Name(PPI2, Package(){0,0,0})
                        Store (0, SRSP)  
                        Store(0x05, FLAG)                           
                        Store(OFST, SMI)
                        Store (RCNT, Index (PPI2, 1))
                        Store (ERRO, Index (PPI2, 2))
                        return(PPI2) 
                    }


                    //
                    // Function 6: Submit preferred user language
                    // Ppi Spec 1.2 section 2.1.6      
                    //  Arg3[0]: String  - preferred language code
                    case(6)
                    {
                        return ( 0x03 ) //Not Implemented                             
                    }
                    
                    
                    //
                    // Function 7: Submit TPM Operation Request to Pre-OS Environment 2
                    // Ppi Spec 1.2 section 2.1.7   
                    //  Arg3[0]: String  - preferred language code
                    case(7)
                    {
                        Store(DeRefOf(Index(Arg3,0)), RQST)  //save request in temp flag
                        Store(0x07, FLAG) 
                        Store (0, OPTN) 
                        If (LEqual (RQST, 23)) {
                            ToInteger(DerefOf(Index(Arg3,0x01)),OPTN)
                        }
                        Store(OFST, TMF1)  
                        Store (0, SRSP)                           
                        Store(TMF1,SMI) 
                        return(SRSP)
                    }

                    case(8)
                    {
                          Store(DeRefOf(Index(Arg3,0)), RQST)  //save request in temp flag          
                          Store(0x08, FLAG)  
                          Store(OFST, TMF1)
                          Store (0, SRSP)                              
                          Store(TMF1,SMI) 
                          return (SRSP)
                    }

                    default { }                    
                }
        } else {if(LEqual(Arg0,
        ToUUID("376054ED-CC13-4675-901C-4756D7F2D45D"))){
            //
            // Reset Atack Mitigation
            //
             switch(ToInteger(Arg2))            
                {
                    //
                    // Function 0: Return supported funcitons
                    //
                    case(0)
                    {
                        return (Buffer() {0x3}) //support functions 0 and 1
                    }

                    //
                    // Function 1: Set MOR Bit State
                    //
                    case(1)
                    {   
                        Store(DeRefOf(Index(Arg3,0)), RQST)  //save request in temp flag
                        Store(0x09, FLAG) 
                        Store(OFST, TMF1)                            
                        Store (0, SRSP)                                
                        Store(TMF1,SMI)  
                        return(SRSP)
                    }
                    default { }                    
                }
            
        }}               
        // TPM2 ACPI Start Method
        //
        If(LEqual(Arg0, ToUUID ("cf8e16a5-c1e8-4e25-b712-4f54a96702c8")))
        {
          Return (CRYF (Arg1, Arg2, Arg3))
        }

        If(LEqual(Arg0, ToUUID ("6bbf6cab-5463-4714-b7cd-f0203c0368d4")))
        {
          Return (STRT (Arg1, Arg2, Arg3))
        }
        return (Buffer() {0})
    }
            
        //
        // PTS Reset MOR Bit When S4, S5
        //
    Method (TPTS, 1) 
    {
#if defined ASL_MOR_RESET_S4S5 && ASL_MOR_RESET_S4S5 == 1
        Switch (ToInteger(Arg0))
        {
            case (4)    // S4
            {
                Store(0, RQST)  //save request in temp flag
                Store(0x09, FLAG) 
                Store (0, SRSP)
                Store(OFST,SMI) 
            }
            case (5)    // S5
            {
                Store(0, RQST)  //save request in temp flag
                Store(0x09, FLAG) 
                Store (0, SRSP)
                Store(OFST,SMI) 
            }
        }
#endif
// When OS is reboot/resume/shutdown, 
// OS will execute the TPM_Shutdown command, 
// But OS will not wait the TPM have already fininshed the internal command, 
// we need to have some gap time for wait the TPM finished it internal command.
#if defined ASL_TCG_WAIT_OS_TPM_READY && ASL_TCG_WAIT_OS_TPM_READY == 1  
        Sleep(300);     // Sleep 300ms
#endif
    }
}
