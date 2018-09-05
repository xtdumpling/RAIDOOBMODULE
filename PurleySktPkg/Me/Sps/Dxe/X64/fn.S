
        .text

ASM_GLOBAL ASM_PFX(ReadTimeStampCounter)
ASM_PFX(ReadTimeStampCounter):
        rdtsc
        ret
