

; TAB=4

[BITS 32]						; 32ビットモード用の機械語を作らせる

		GLOBAL	io_hlt, io_sti, io_cli, io_stihlt
		GLOBAL	io_load_eflags, io_store_eflags
		GLOBAL	load_gdtr, load_idtr
		GLOBAL	io_in8, io_out8
		GLOBAL	io_in16, io_out16
		GLOBAL	load_cr0, store_cr0
		GLOBAL	load_tr
		GLOBAL	farcall, farjmp, func_retf

[SECTION .text]


io_hlt:	; void io_hlt(void);
		HLT
		RET

io_cli:	; void io_cli(void);
		CLI
		RET

io_sti:	; void io_sti(void);
		STI
		RET

io_stihlt:	; void io_stihlt(void);
		STI
		HLT
		RET


load_gdtr:		; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

load_idtr:		; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; PUSH EFLAGS という意味
		POP		EAX
		RET

io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS という意味
		RET

io_in8:	; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

io_out8:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

io_in16:	; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

io_out16:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AX,[ESP+8]		; data
		OUT		DX,AX
		RET

load_cr0:		; int load_cr0(void);
		MOV		EAX,CR0
		RET

store_cr0:		; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

load_tr:		; void load_tr(int tr);
		LTR		[ESP+4]			; tr
		RET

farcall:		; void farcall(int eip, int cs);
		CALL	FAR	[ESP+4]				; eip, cs
		RET

farjmp:		; void farjmp(int eip, int cs);
		JMP	FAR	[ESP+4]				; eip, cs
		RET

func_retf:			; void func_retf(void);
		RETF

