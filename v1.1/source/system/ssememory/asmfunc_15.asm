

; TAB=4

[BITS 32]						; 32ビットモード用の機械語を作らせる

		GLOBAL	asm_ssememcpy15


[SECTION .text]

		_SHIFT		equ	15
		_16SHIFT	equ	16-15

asm_ssememcpy15:	;void asm_ssememcpy15( unsigned int _dst, unsigned int _src, unsigned int _size );
		mov		eax, [ESP+4]
		mov		esi, [ESP+8]
		mov		ecx, [ESP+12]
		sub		eax, esi
		shr		ecx, 6
		movdqa  xmm0, [esi+ 0];
		movdqu  [esi+eax+ 0], xmm0;
		add		 eax, _16SHIFT;
		psrldq  xmm0, _16SHIFT;
 
LMAIN:
		movdqa  xmm1, [esi+16];
		movdqa  xmm3, [esi+32];
		movdqa  xmm2, xmm1;
		movdqa  xmm4, xmm3;
		pslldq  xmm1, _SHIFT;
		psrldq  xmm2, _16SHIFT;
		pslldq  xmm3, _SHIFT;
		psrldq  xmm4, _16SHIFT;
		por		 xmm1, xmm0;
		por		 xmm3, xmm2;
		MOVNTDQ [esi+eax+ 0], xmm1;
		MOVNTDQ [esi+eax+16], xmm3;
 
		movdqa  xmm1, [esi+48];
		movdqa  xmm3, [esi+64];
		movdqa  xmm2, xmm1;
		movdqa  xmm0, xmm3;
		pslldq  xmm1, _SHIFT;
		psrldq  xmm2, _16SHIFT;
		pslldq  xmm3, _SHIFT;
		psrldq  xmm0, _16SHIFT;
		por		 xmm1, xmm4;
		por		 xmm3, xmm2;
		MOVNTDQ [esi+eax+32], xmm1;
		MOVNTDQ [esi+eax+48], xmm3;
 	
		add		esi, 64;
		loop	LMAIN
		
		MOV		eax, esi

		RET

