#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

#**********************************************************************
# $Header:  $
#
# $Revision: $
#
# $Date:  $
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	AutoGenFboData.gawk
#
# Description: To generate FixedBootOrder Bcp Data into DefaultFixedBootOrder.sd
#              and FboGroupFrom.vfr                
#
#<AMI_FHDR_END>
#**********************************************************************

BEGIN {
########Global parameter and Template Area########
UefiTemplate=\
"suppressif ideqvallist SETUP_DATA.BootMode == 0 2 3 4 5;\n%s\
    oneof varid = SETUP_DATA.UefiPriorities[Device%d],\n\
    questionid = %d,\n\
    prompt = STRING_TOKEN(STR_UEFI_BOOT_ORDER_%d),\n\
    help = STRING_TOKEN(STR_BOOT_OPTION_HELP),\n%s\
    endoneof;\n\
endif;\n\n" ;

LegacyTemplate=\
"suppressif ideqvallist SETUP_DATA.BootMode == 1 2 3 4 5;\n%s\
    oneof varid = SETUP_DATA.LegacyPriorities[Device%d],\n\
    questionid = %d,\n\
    prompt = STRING_TOKEN(STR_LEGACY_BOOT_ORDER_%d),\n\
    help = STRING_TOKEN(STR_BOOT_OPTION_HELP),\n%s\
    endoneof;\n\
endif;\n\n" ;

DualTemplate=\
"suppressif ideqvallist SETUP_DATA.BootMode == 0 1 3 4 5;\n%s\
    oneof varid = SETUP_DATA.DualPriorities[Device%d],\n\
    questionid = %d,\n\
    prompt = STRING_TOKEN(STR_DUAL_BOOT_ORDER_%d),\n\
    help = STRING_TOKEN(STR_BOOT_OPTION_HELP),\n%s\
    endoneof;\n\
endif;\n\n" ;

Seperate=\
"SEPARATOR \n\
SUBTITLE(STRING_TOKEN(STR_BCP_%s_DEFAULT_SUBTITLE)) \n" ;

OptionTemplate=\
"        option text = STRING_TOKEN(STR_BOOT_%s), value = %s_VALUE, flags = INTERACTIVE | %s;\n"

DualTokenOptionTemplate=\
"        option text = STRING_TOKEN(STR_BOOT_DUAL_%s), value = %s_VALUE, flags = INTERACTIVE | %s;\n"

DisabledOptionTemplate =\
"        option text = STRING_TOKEN(STR_DISABLED), value = %s_DISABLED_VALUE, flags = INTERACTIVE | 0;\n"

UefiGroupGotoTemplate=\
"suppressif ideqvallist SETUP_DATA.BootMode == 0;\n\
    suppressif ideqvallist FIXED_BOOT_GROUP.Uefi%s == 0;\n\
        goto \n\
            formsetguid = FBO_GROUP_FORM_FORM_SET_GUID, \n\
            formid = AUTO_ID(%s_BOOT_FORM_ID),\n\
            question = 0, \n\
            prompt = STRING_TOKEN(STR_%s_BOOT_ORDER_SUBTITLE),\n\
            help = STRING_TOKEN(STR_%s_BOOT_ORDER_HELP);\n\
    endif;\n\
endif;\n\n"

LegacyGroupGotoTemplate=\
"suppressif ideqvallist SETUP_DATA.BootMode == 1;\n\
    suppressif ideqvallist FIXED_BOOT_GROUP.Legacy%s == 0;\n\
        goto \n\
            formsetguid = FBO_GROUP_FORM_FORM_SET_GUID, \n\
            formid = AUTO_ID(%s_BOOT_FORM_ID),\n\
            question = 0, \n\
            prompt = STRING_TOKEN(STR_%s_BOOT_ORDER_SUBTITLE),\n\
            help = STRING_TOKEN(STR_%s_BOOT_ORDER_HELP);\n\
    endif;\n\
endif;\n\n"
            
GroupFormTemplate=\
"form formid = %s_BOOT_FORM_ID,\n\
    title = STRING_TOKEN(STR_%s_BOOT_ORDER_SUBTITLE);\n\
    label  %s_BOOT_FORM_LABEL;\n\
endform;\n\n"

##################################################
    key = 0 ;
    if ( TYPE == "UEFI" )
    {
        key = 0x1080 ;
        Template = UefiTemplate ;
    }
    else if ( TYPE == "LEGACY" )
    {
        key = 0x1000 ;
        Template = LegacyTemplate ;
    }
    else if ( TYPE == "DUAL" )
    {
        key = 0x1100 ;
        Template = DualTemplate ;
    }
    else if ( TYPE == "UEFI_GROUP_GOTO" )
        Template = UefiGroupGotoTemplate ;
    else if ( TYPE == "LEGACY_GROUP_GOTO" )
        Template = LegacyGroupGotoTemplate ;
    else if ( TYPE == "UEFI_GROUP_FORM" || TYPE == "LEGACY_GROUP_FORM" )
        Template = GroupFormTemplate ;
    
    ##Seperate = sprintf(Seperate, TYPE) ;

    DevCount = split(DEVICE, dev, "+") ;
    for ( i = DevCount ; i >=1 ; i-- )
    {
        if ( dev[i] == "" || dev[i] == " " )
            DevCount-- ;
    }
    
    if ( TYPE == "UEFI" || TYPE == "LEGACY" || TYPE == "DUAL" )
    {
        for ( i = 1 ; i <= DevCount ; i++ )
        {
            Option = "";
            for ( x = 1 ; x <= DevCount ; x++ )
            {
                if ( (TYPE == "UEFI" || TYPE == "LEGACY") && DUAL_TOKEN == "11")
                    OptionTemplate = DualTokenOptionTemplate ;
                if ( i == x )
                    Data = sprintf(OptionTemplate, dev[x], dev[x], "DEFAULT") ;
                else
                    Data = sprintf(OptionTemplate, dev[x], dev[x], "0") ;
                Option = Option Data ; 
            }
            if(DISABLED)
            {
                Data = sprintf(DisabledOptionTemplate, TYPE) ;
                Option = Option Data ;
            }
        
            OptionData = sprintf(Template, "", i, key+i-1, i, Option) ;
            InsertGroup = InsertGroup "\n" OptionData ;
        }
    }
    else if ( TYPE == "UEFI_GROUP_GOTO" || TYPE == "LEGACY_GROUP_GOTO" ) 
    {
        for ( i = 1 ; i <= DevCount ; i++ )
        {   
            if ( TYPE == "UEFI_GROUP_GOTO" )
                Group = sprintf(Template, substr(dev[i], 6), dev[i], dev[i], dev[i]) ;
            else
                Group = sprintf(Template, dev[i], dev[i], dev[i], dev[i]) ;
            InsertGroup = InsertGroup "\n" Group ;
        }
    }
    else if ( TYPE == "UEFI_GROUP_FORM" || TYPE == "LEGACY_GROUP_FORM" ) 
    {
        for ( i = 1 ; i <= DevCount ; i++ )
        {
            if ( TYPE == "LEGACY_GROUP_FORM" )
                Group = sprintf(Template, dev[i], dev[i], dev[i]) ;
            else
                Group = sprintf(Template, dev[i], dev[i], dev[i]) ;
            InsertGroup = InsertGroup "\n" Group ;
        }
    }

    
    ## clear temp AutoGenFboData.sd"
    print > "Build/AutoGenFboData.sd"
}

{
    if ( TYPE == "UEFI" )
        sub(/MAKEFILE_INSERT_UEFI_OPTION_ITEM_FLAG/, InsertGroup);
    else if ( TYPE == "LEGACY" )
        sub(/MAKEFILE_INSERT_LEGACY_OPTION_ITEM_FLAG/, InsertGroup);
    else if ( TYPE == "DUAL" )
        sub(/MAKEFILE_INSERT_DUAL_OPTION_ITEM_FLAG/, InsertGroup); 
    else if ( TYPE == "UEFI_GROUP_GOTO" )
        sub(/MAKEFILE_INSERT_UEFI_GROUP_FORM_GOTO_ITEM_FLAG/, InsertGroup); 
    else if ( TYPE == "LEGACY_GROUP_GOTO" )
        sub(/MAKEFILE_INSERT_LEGACY_GROUP_FORM_GOTO_ITEM_FLAG/, InsertGroup); 
    else if ( TYPE == "UEFI_GROUP_FORM" )
        sub(/MAKEFILE_INSERT_UEFI_GROUP_FORM_ITEM_FLAG/, InsertGroup); 
    else if ( TYPE == "LEGACY_GROUP_FORM" )
        sub(/MAKEFILE_INSERT_LEGACY_GROUP_FORM_ITEM_FLAG/, InsertGroup);         
    print >> "Build/AutoGenFboData.sd"

}


END {
    close("Build/AutoGenFboData.sd")
}

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************