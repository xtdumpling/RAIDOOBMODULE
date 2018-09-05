
Use the feature didn't need to enable "DEBUG_MODE".
It can only throw message to "Samedebug".

1. How to enable the feature.
Ans : Enable the token - "SMC_ACPI_DEBUG_SUPPORT".

2. How to use the function in ASL code.
Ans : 
  DSDT table has externalized the methods by default.
  So user needn't to externalize the methods in DSDT.
  Other tables need to externalize the methods before using.
  EX : 
    External(\MDBS, MethodObj)
    External(\MDBN, MethodObj)

  Throw a string : 
    MDBS("Hello\n")
  Throw a format string : 
    MDBN("NUM1=%d, NUM2=%d, NUM3=0x%x\n", 3, 4, 0x55)
    MDBN("NUM1=%d\n", 3, 0, 0)

-------------------------------------------------------------------------------

Method : MDBS

Description :
  Print a string.
  DSDT table has externalized the method by default.
  Other tables need to externalize the method before using.
  EX : External(\MDBS, MethodObj)

Arguments :
  Arg0     : String

Example :
  MDBS("Hello\n")

-------------------------------------------------------------------------------

Method : MDBN

Description :
  Print a format string.
  The method needs 4 arguments.
  DSDT table has externalized the method by default.
  Other tables need to externalize the method before using.
  EX : External(\MDBN, MethodObj)

Arguments :
  Arg0     : Format String
  Arg1     : Integer
  Arg2     : Integer
  Arg3     : Integer

Example :
  MDBN("NUM1=%d, NUM2=%d, NUM3=0x%x\n", 3, 4, 0x55)
  MDBN("NUM1=%d\n", 3, 0, 0)

-------------------------------------------------------------------------------
