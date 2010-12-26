

; TAB=4

[BITS 32]								; 32ビットモード用の機械語を作らせる


        GLOBAL  asm_clts, asm_fnsave, asm_frstor, asm_inthandler07
        EXTERN  inthandler07

[SECTION .text]

asm_clts:          ; void clts(void);
        CLTS
        RET

asm_fnsave:        ; void fnsave(int *addr);
        MOV     EAX,[ESP+4]     ; addr
        FNSAVE  [EAX]
        RET

asm_frstor:        ; void frstor(int *addr);
        MOV     EAX,[ESP+4]     ; addr
        FRSTOR  [EAX]
        RET

asm_inthandler07:
        STI
        PUSH    ES
        PUSH    DS
        PUSHAD
        MOV     EAX,ESP
        PUSH    EAX
        MOV     AX,SS
        MOV     DS,AX
        MOV     ES,AX
        CALL    inthandler07
        CMP     EAX,0
;        JNE     asm_end_app
        POP     EAX
        POPAD
        POP     DS
        POP     ES
        IRETD                   ; INT07では ESP += 4; はいらない
