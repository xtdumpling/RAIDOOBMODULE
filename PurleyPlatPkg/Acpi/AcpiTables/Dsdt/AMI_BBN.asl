//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
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
//<AMI_FHDR_START>
//
// Name:  BBN.ASL
//
// Description: Override Bridge Bus Base Number method for every root bridge
//
//<AMI_FHDR_END>
//**********************************************************************

Scope(\_SB.PC01) {
   Method(BN10, 0){          
        return(\BB01) 
    }
}  

Scope(\_SB.PC02) {  
   Method(BN20, 0){          
        return(\BB02) 
    }
}  

Scope(\_SB.PC03) {
   Method(BN28, 0){          
        return(\BB03) 
    }
}  

Scope(\_SB.PC04) {
   Method(BN30, 0){          
        return(\BB04) 
    }
} 

Scope(\_SB.PC05) { 
   Method(BN38, 0){          
        return(\BB05) 
    }
}  

Scope(\_SB.PC06) { 
   Method(BN40, 0){          
        return(\BB06) 
    }
}  

Scope(\_SB.PC07) { 
   Method(BN50, 0){          
        return(\BB07) 
    }
} 

Scope(\_SB.PC08) { 
   Method(BN60, 0){          
        return(\BB08) 
    }
} 

Scope(\_SB.PC09) { 
   Method(BN68, 0){          
        return(\BB09) 
    }
} 

Scope(\_SB.PC10) { 
   Method(BN70, 0){          
        return(\BB10) 
    }
} 

Scope(\_SB.PC11) { 
   Method(BN78, 0){          
        return(\BB11) 
    }
} 

Scope(\_SB.PC12) { 
   Method(BN80, 0){          
        return(\BB12) 
    }
} 
    
Scope(\_SB.PC13) { 
   Method(BN90, 0){          
        return(\BB13) 
    }
} 
    
Scope(\_SB.PC14) { 
   Method(BNA0, 0){          
        return(\BB14) 
    }
}

Scope(\_SB.PC15) { 
   Method(BNA8, 0){          
        return(\BB15) 
    }
}
Scope(\_SB.PC16) { 
   Method(BNB0, 0){          
        return(\BB16) 
    }
}

Scope(\_SB.PC17) { 
   Method(BNB8, 0){          
        return(\BB17) 
    }
}

Scope(\_SB.PC18) { 
   Method(BNC0, 0){          
        return(\BB18) 
    }
}

Scope(\_SB.PC19) { 
   Method(BND0, 0){          
        return(\BB19) 
    }
}

Scope(\_SB.PC20) { 
   Method(BNE0, 0){          
        return(\BB20) 
    }
} 

Scope(\_SB.PC21) { 
   Method(BNE8, 0){          
        return(\BB21) 
    }
} 

Scope(\_SB.PC22) { 
   Method(BNF0, 0){          
        return(\BB22) 
    }
} 

Scope(\_SB.PC23) { 
   Method(BNF8, 0){          
        return(\BB23) 
    }
}  