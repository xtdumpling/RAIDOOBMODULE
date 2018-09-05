include token.equ

ifndef EFIx64 
.model small
endif

.data
    db 'MPDT'   ;Microcode Patch Description Table
    db 0        ;Revision
    db 1        ;[0] = 0 Non boot block, [0] = 1 boot block
    db 0        ;CPU Manufacture: 0 = Intel
    db 0        ;Reserved
    dd MKF_MICROCODE_ALIGNMENT  ;Alignment - Include token .equ
    dw 0        ;Reserved
    dw 16       ;size of Table
END