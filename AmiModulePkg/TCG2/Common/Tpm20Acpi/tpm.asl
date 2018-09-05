//tpm.asl
External(CRBI)
    Device(\_SB.TPM)
    {
        Method(_HID, 0){		//PnP Device ID
           If(TCMF)		
           {
              Return(EISAID("ZIT0101"))
           }
           Else
           {
               If(LEqual(TTDP, 0)){
                    Return(EISAID("PNP0C31"))
                }else{
                    Return("MSFT0101")
                }
           }
        }
        
#if defined (CID_SUPPORT) && (CID_SUPPORT == 1)
        Method(_CID, 0){       //Redline RS2 changes; Added CID
           If(LEqual(TCMF, 0))     
           {
               If(LEqual(TTDP, 0)){
                    Return(EISAID("PNP0C31"))
               }else{
                    Return("MSFT0101")
               }
           }
        }
#endif
       

	    Method(_STR,0)
        {
           If(LEqual(TTDP, 0)){
                Return (Unicode ("TPM 1.2 Device"))
           }else {
                Return (Unicode ("TPM 2.0 Device"))
           }
        }
        
	    Name(_UID,0x01)
	    
      Name(CRST,ResourceTemplate()
	  {
            Memory32Fixed (ReadOnly, 0x00000000, 0x1000,PCRB)
            Memory32Fixed (ReadOnly, 0xFED70000, 0x1000,PCRC)
	  })
	  
	  Name(CRSD,ResourceTemplate()
      {
            Memory32Fixed (ReadWrite, 0xFED40000, 0x5000, PCRE)
      })

      Name(CRSI,ResourceTemplate()
	  {
            Memory32Fixed (ReadWrite, 0xFED40000, 0x5000,PCRS)            
	  })
      //
      // Return the resource consumed by TPM device
      //
      Method(_CRS,0,Serialized)
	  {
         If(LEqual(AMDT, 1))
         {
             CreateDWordField(CRST, ^PCRB._BAS, MTFB) // Min
             CreateDWordField(CRST, ^PCRB._LEN, LTFB) // Length
                   
             Store(TPMB, MTFB)
             Store(0x1000, LTFB)

             CreateDWordField(CRST, ^PCRC._BAS, MTFC) // Min
             CreateDWordField(CRST, ^PCRC._LEN, LTFC) // Length
               
             Store(TPMC, MTFC)
             Store(0x1000, LTFC)
            
             Return (CRST)
         }Else{
            If(LEqual(DTPT,1))
            {
               CreateDWordField(CRSD, ^PCRE._BAS, MTFE) // Min
               CreateDWordField(CRSD, ^PCRE._LEN, LTFE) // Length
               
               Store(0x0FED40000, MTFE)
               Store(0x00005000, LTFE)
                              
               Return(CRSD)
            }            
            ElseIf(LEqual(TTPF, 1))
            {
               CreateDWordField(CRSI, ^PCRS._BAS, MTFD) // Min
               CreateDWordField(CRSI, ^PCRS._LEN, LTFD) // Length
               
               Store(0x0FED40000, MTFD)
               Store(0x00005000, LTFD)
               Return (CRSI)
            }ElseIf(LEqual(TTPF, 0))
            {
               CreateDWordField(CRSI, ^PCRS._BAS, MTFF) // Min
               Store(TPMM, MTFF)
               Return (CRSI)
            }

            Store(0x00000000, MTFE)
            Store(0x00000000, LTFE)
            Return (CRSI)  
         }
            Return (CRSI)
      }

	  OperationRegion(TMMB, SystemMemory, 0x0FED40000, 0x5000)
	  Field(TMMB, ByteAcc, Lock, Preserve)
	  {
	    Offset(0x04),
	    LCST, 32,          // LOC_STATE
	    Offset(0x40),
        CREQ, 32,          // CTRL_REQ
	    Offset(0x44), 
	    CSTS, 32,          // CTRL STS
	    Offset(0x4C),
	    SCMD, 32,          // CTRL STRT
	  }

      OperationRegion (CRBD, SystemMemory, TPMM, 0x48)
      Field (CRBD, AnyAcc, NoLock, Preserve)
      {
        Offset(0x04),
        HERR, 32,
        Offset (0x40),
        HCMD, 32,
        Offset(0x44), 
        HSTS, 32,
      }
      

      Method(_STA, 0){
	   	If(LEqual(TTDP, 0)){
            If(TPMF){
                 Return(0x0F)  // TPM Support 
             }
             Return(0x00)  // No TPM Support
         }ElseIF(LEqual(TTDP, 1)){
            If(TPMF){
             Return(0x0F)  // TPM Support 
            }
            Return (0x00)
         } 
         Return (0x00)
	   }
   
       Method (STRT, 3, Serialized, 0, IntObj, {UnknownObj, UnknownObj, UnknownObj}) // IntObj, IntObj, PkgObj
       {
          OperationRegion (TPMR, SystemMemory, FTPM, 0x1000)
          Field (TPMR, AnyAcc, NoLock, Preserve)
          {
            Offset(0x04),
            FERR, 32,
            Offset(0x0c),
            BEGN, 32,
          }
          
          Name (TIMR, 0)  
          If( LNotEqual( ToInteger (Arg0), 0 ))
          {
            // For Tools check Arg0, do nothing...
            // Return (1)   // 1: General Failure
          }
		  
          //
          // Switch by function index
          //
          Switch (ToInteger (Arg1))
          {
              Case (0)
              {
                //
                // Standard query, supports function 1-1
                //
                Return (Buffer () {0x03})
              }
              Case (1)
              {   
                Store(0, TIMR)
                
                If(LEqual(AMDT, 1))
                {
                  While(LAND(LEqual(BEGN, One), LLESS(TIMR, 0x200)))
       		      {
        			 If(LEqual(BEGN, One))
    				 {
    				   Sleep(0x1)
    				   Increment(TIMR)
    		         }
    			  }

                }Else{
#if defined( ASL_TPM20_CRB_WITH_START_METHOD ) && (ASL_TPM20_CRB_WITH_START_METHOD == 1)

                Store(0, TIMR);
                //
                // Wait STR bit clear, 90 sec for time out
                //
                While(LAND(LLessEqual(TIMR, 90000),LNotEqual(And(SCMD,0x00000001),0)))
                {
                    Sleep(1)
                    Increment(TIMR)
                }
#else
                  If(LEqual(Or(And(HSTS,0x00000002),And(HSTS,0x00000001)),0x00000003))
                  {
                    //
                    // Trigger the FTPM_CMD interrupt
                    //
                    Store (0x00000001, HCMD)
                  }              
                  Else
                  {
                    //Set Error Bit
                    Store(0x00000001,FERR)
                    //Clear Start Bit
                    Store(0x00000000,BEGN)
                  }
#endif

              }
              Return(Zero)
           }    
        }
	  
	    Return (1)    
	  }

      Method (CRYF, 3, Serialized, 0, {BuffObj, PkgObj}, {UnknownObj, UnknownObj, UnknownObj}) // IntObj, IntObj, PkgObj
      {
        If( LNotEqual( ToInteger (Arg0), 1 ))
        {
          // For Tools check Arg0, do nothing...
          // Return (Buffer () {0x00})  // 0: Failure
        }
          
        //
        // Switch by function index
        //
        Switch (ToInteger(Arg1))
        {
          Case (0)
          {
            //
            // Standard query
            //
            Return (Buffer () {0x03})
          }
          Case (1)
          {
            //
            // Return failure if no TPM present
            //
            Name(TPMV, Package () {0x01, Package () {0x1, 0x20}})
            if (LEqual (_STA (), 0x00))
            {
              Return (Package () {0x00})
            }
            Return (TPMV)
          }
        }
        Return (Buffer () {0})
      }

  }