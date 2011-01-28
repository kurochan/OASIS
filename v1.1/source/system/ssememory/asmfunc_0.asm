

; TAB=4

[BITS 32]						; 32ビットモード用の機械語を作らせる
[CPU PRESCOTT]

		GLOBAL	asm_ssememcpy0


[SECTION .text]

asm_ssememcpy0:	;void asm_ssememcpy0( unsigned int _dst, unsigned int _src, unsigned int _size );
		mov		eax, [ESP+4]
		mov		esi, [ESP+8]
		mov		ecx, [ESP+12]
		sub		 eax, esi;
		shr		 ecx, 6;						 // ecx = ecx / 64;


LOOP_MAIN:
		movdqa	xmm0, [esi+0];
		movdqa	xmm1, [esi+16];
		movdqa	xmm2, [esi+32];
		movdqa	xmm3, [esi+48];
		MOVNTDQ	[esi+eax+ 0], xmm0;
		MOVNTDQ	[esi+eax+16], xmm1;
		MOVNTDQ	[esi+eax+32], xmm2;
		MOVNTDQ	[esi+eax+48], xmm3;
		add		esi, 64;
		loop	LOOP_MAIN;
		
		MOV		eax, esi

		RET

