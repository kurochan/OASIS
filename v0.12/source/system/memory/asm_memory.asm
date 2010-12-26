
; TAB=4

[BITS 32]						; 32ビットモード用の機械語を作らせる

		GLOBAL	memcloot

[SECTION .text]


memcloot:
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		PUSH	ECX
		PUSH	EDX
		PUSH	EBP
		MOV		ESI,0xaa55aa55
		MOV		EDI,0x55aa55aa
		MOV		EAX,0x00400000
		CALL	memtestmain
		MOV		EAX,0
		JNZ		memclootret
		MOV		EAX,0xfffff000
		CALL	memtestmain
		JZ		memclootret
		SUB		EAX,0x00400000
		MOV		EAX,0x80000000
		MOV		ECX,0x40000000
		MOV		EBP,ECX
		MOV		EBX,0

memclootmain_s:
		CALL	memtestmain
		JNZ		memclootmain_f

memclootmain_t:
		MOV		EBP,EAX
		ADD		EAX,ECX
		JMP		memclootmain_e

memclootmain_f:
		SUB		EAX,ECX
		
memclootmain_e:
		SHR		ECX,1
		INC		BH
		CMP		BH,0x14
		JNE		memclootmain_s
		MOV		EAX,EBP
		ADD		EAX,0xffc
		CALL	memtestmain
		JZ		memclootret

memclootmain_em:
		SUB		EAX,0x1000

memclootret:
		ADD		EAX,4
		POP		EBP
		POP		EDX
		POP		ECX
		POP		EBX
		POP		ESI
		POP		EDI
		RET

memtestmain:
		MOV		EDX,[EAX]
		MOV		[EAX],ESI
		XOR		DWORD [EAX],0xffffffff
		CMP		EDI,[EAX]
		JNE		memtestRET
		XOR		DWORD [EAX],0xffffffff
		CMP		ESI,[EAX]

memtestRET:
		RET
