;=========================================
;         OASIS Kernel source code
;                Copyright (C) 2010 soS.A
;=========================================

;main.c�̃R�[�h�̑O�Ɏ��s�����A�A�Z���u���R�[�h
;CPU�`�F�b�N�A�𑜓x�ύX�A�v���e�N�g���[�h�ւ̈ڍs

; core 04_day harib01a
; system 14_day harib11i
; betasystem 15_day harib12a
; T-OS boot asm
; TAB=4

ORG		0xc200			; ���̃v���O�������ǂ��ɓǂݍ��܂��̂�

KERNEL	EQU		0x00280000		; kernel�̃��[�h��
DSKCAC	EQU		0x00100000		; �f�B�X�N�L���b�V���̏ꏊ
DSKCAC0	EQU		0x00008000		; �f�B�X�N�L���b�V���̏ꏊ�i���A�����[�h�j

; BOOT_INFO�֌W
CYLS	EQU		0x0ff0			; �u�[�g�Z�N�^���ݒ肷��
LEDS	EQU		0x0ff1
VBITC	EQU		0x0ff2			; �F���Ɋւ�����B���r�b�g�J���[���H
SCRNX	EQU		0x0ff4			; �𑜓x��X
SCRNY	EQU		0x0ff6			; �𑜓x��Y
VRAM	EQU		0x0ff8			; �O���t�B�b�N�o�b�t�@�̊J�n�Ԓn



[SECTION .data]
;�{����.text�̂͂������A.text�ɂ���ƂȂ���asmhead.bin�̐擪��0x4600�o�C�g����0x00���ǉ�����Ă��܂�
;0xc200-0x7c00=0x4600���֌W���Ă���̂��낤���E�E�E�B

check_8086:
		PUSHF
		POP		AX
		MOV		CX,AX
		AND		AX,0x0FFF
		PUSH	AX
		POPF
		PUSHF
		POP		AX
		AND		AX,0xF000
		CMP		AX,0xF000
		JNZ		check_286
		JMP		cpu_msg

; ���R�[�h
;		JNZ		check_286
;		PUSH	SP
;		POP		DX
;		CMP		DX,SP
;		JNE		cpu_msg
;		JMP		cpu_msg
;
; �C�~�t�B����cpu_msg�ɔ��


check_286:
		OR		CX,0xF000
		PUSH	CX
		POPF
		PUSHF
		POP		AX
		AND		AX,0xF000
		JNZ		check_386
		JMP		cpu_msg


check_386:
		PUSHFD
		POP		EAX
		MOV		ECX,EAX
		XOR		EAX,0x40000
		PUSH	EAX
		POPFD
		PUSHFD
		POP		EAX
		XOR		EAX,ECX
		JNZ		check_486
		JMP		cpu_msg

check_486:
		PUSH	ECX
		POPFD
		MOV		EAX,ECX
		XOR		EAX,0x200000
		PUSH	EAX
		POPFD
		PUSHFD
		POP		EAX
		XOR		EAX,ECX
		JNE		cpuid_check
		JMP		cpu_msg			;486�ȍ~�̂b�o�t�ł͂Ȃ��I�����H�H�H

cpuid_check:
		MOV		EAX,0x0
		DB		0x0f,0xa2			; CPU_ID(NASM�ɑ������߂���������Ă��Ȃ�)
		CMP		EAX,0x0
		JNE		cpucheck
		JMP		cpu_notsupported	; EAX���W�X�^�̒l��0���傫���Ȃ��Ă͂����Ȃ�

; CPU����
cpucheck:

		PUSH	ECX
		PUSH	EBX
		PUSH	EDX
		MOV		BX,0x0007
		MOV		CX,16
		MOV		SI,cpu_vendor
		CALL	message
		POP		EDX
		POP		ECX
		POP		EDI
		MOV		AH,0x0e
		MOV		BX,0
		CALL	cpuid_m
		MOV		ECX,EDX
		CALL	cpuid_m
		MOV		ECX,EDI
		CALL	cpuid_m

		MOV		EAX,0x1
		DB		0x0f,0xa2		; CPU_ID
		SHL		ECX,16

		MOV		BX,0x0007
		MOV		CX,58
		MOV		SI,cpuc_start
		CALL	message

		MOV		SI,cpuc_mmx
		MOV		EDI,0x800000
		CALL	cpuc_sse_l

		MOV		SI,cpuc_fxsr
		MOV		EDI,0x1000000
		CALL	cpuc_sse_l

		MOV		SI,cpuc_sse
		MOV		EDI,0x2000000
		CALL	cpuc_sse_l

		MOV		SI,cpuc_sse2
		MOV		EDI,0x4000000
		CALL	cpuc_sse_l

		MOV		SI,cpuc_sse3
		MOV		BX,0x0007
		MOV		CX,5
		CALL	message
		MOV		BX,0x0007
		MOV		CX,13
		MOV		SI,cpuc_msg_s
		CALL	message
		TEST	ECX,0x10000
		JNZ		cpu_check_ok
		JMP		sse_no

cpu_check_ok:
		MOV		BX,0x000a
		MOV		CX,2
		MOV		SI,cpuc_ok_msg
		CALL	message
		MOV		BX,0x0007
		MOV		CX,4
		MOV		SI,cpuc_msg_e
		CALL	message

		MOV		EAX,80000000h
		DB		0x0f,0xa2		; CPU_ID
		CMP		EAX,80000004h
		JA		print_brandstring
		JMP		brandstring_notsupported

print_brandstring:

		MOV		SI,cpu_bs
		MOV		BX,0x0007
		MOV		CX,21
		CALL	message

		MOV		EAX,80000002h
		CALL	mes_brandstring

		MOV		EAX,80000003h
		CALL	mes_brandstring

		MOV		EAX,80000004h
		CALL	mes_brandstring
		JMP		VESA

cpu_msg:
		MOV		BX,0x000c
		MOV		CX,41
		MOV		SI,cpu_err_msg
		CALL	message
		JMP		fin

cpuid_m:
		MOV		AL,CL
		INT		0x10			; �r�f�IBIOS�Ăяo��
		SHR		ECX,8
		MOV		AL,CL
		INT		0x10
		SHR		ECX,8
		MOV		AL,CL
		INT		0x10
		SHR		ECX,8
		MOV		AL,CL
		INT		0x10
		RET

cpuc_sse_l:
		MOV		BX,0x0007
		MOV		CX,5
		CALL	message
		MOV		BX,0x0007
		MOV		CX,13
		MOV		SI,cpuc_msg_s
		CALL	message
		TEST	EDX,EDI
		JZ		sse_no
		MOV		BX,0x000a
		MOV		CX,2
		MOV		SI,cpuc_ok_msg
		CALL	message
		MOV		BX,0x0007
		MOV		CX,4
		MOV		SI,cpuc_msg_e
		CALL	message
		RET

sse_no:
		MOV		BX,0x000c
		MOV		CX,2
		MOV		SI,cpuc_no
		CALL	message
		MOV		BX,0x0007
		MOV		CX,4
		MOV		SI,cpuc_msg_e
		CALL	message

cpu_notsupported:
		MOV		BX,0x000c
		MOV		CX,43
		MOV		SI,cpu_err_msg
		CALL	message
		JMP		VESA
		JMP		fin

brandstring_notsupported:
		MOV		BX,0x000c
		MOV		CX,106
		MOV		SI,cpubs_msg
		CALL	message
		JMP		VESA
		JMP		fin

fin:
		HLT						; ��������܂�CPU���~������
		JMP		fin				; �������[�v


message:
		MOV		AL,CL
		MOV		AH,0x03
		INT		0x10
		MOV		CX,0
		MOV		CL,AL
		MOV		AX,0
		MOV		ES,AX
		MOV		BP,SI
		MOV		AX,0x1301
		INT		0x10
		RET

return:
		RET

mes_brandstring:
		DB		0x0f,0xa2		; CPU_ID
		MOV		[cpu_brandstring_A],EAX
		MOV		[cpu_brandstring_B],EBX
		MOV		[cpu_brandstring_C],ECX
		MOV		[cpu_brandstring_D],EDX
		MOV		SI,cpu_brandstring_A
		MOV		BX,0x0007
		MOV		CX,4
		CALL	message
		MOV		SI,cpu_brandstring_B
		MOV		BX,0x0007
		MOV		CX,4
		CALL	message
		MOV		SI,cpu_brandstring_C
		MOV		BX,0x0007
		MOV		CX,4
		CALL	message
		MOV		SI,cpu_brandstring_D
		MOV		BX,0x0007
		MOV		CX,4
		CALL	message
		RET

cpu_err_msg:
		DB		0x0d, 0x0a, 0x0a, 0x0a
		DB		" Sorry, OASIS doesn't support your CPU."

cpu_vendor:
		DB		0x0d, 0x0a, 0x0a, 0x0a
		DB		" CPU Vendor:"

cpubs_msg:
		DB		0x0d, 0x0a, 0x0a, 0x0a
		DB		" Sorry, your CPU doesn't support Extended CPUID Command."
		DB		0x0d,0x0a
		DB		" So, OASIS can't know your CPU Brand String."

vesa_err_msg:
		DB		0x0d, 0x0a, 0x0a, 0x0a
		DB		" Sorry! This computer doesn't support VESA2.0."
		DB		0x0d, 0x0a
		DB		" Press power button to shutdown the computer."
		DB		0x0d, 0x0a

cpuc_start:
		DB		0x0d, 0x0a,0x0a
		DB		" First, OASIS have to check SSE support on your CPU."
		DB		0x0d, 0x0a,0x0a

cpuc_mmx:
		DB		" MMX "
		DB		0

cpuc_fxsr:
		DB		" FXSR"
		DB		0

cpuc_sse:
		DB		" SSE "
		DB		0

cpuc_sse2:
		DB		" SSE2"
		DB		0

cpuc_sse3:
		DB		" SSE3"
		DB		0

cpuc_msg_s:
		DB		" support...[ "
		DB		0

cpuc_ok_msg:
		DB		"OK"
		DB		0

cpuc_no:
		DB		"NO"
		DB		0

cpuc_msg_e:
		DB		" ]"
		DB		0x0d, 0x0a
		DB		0

cpu_brandstring_A:
		DD		0x0

cpu_brandstring_B:
		DD		0x0

cpu_brandstring_C:
		DD		0x0

cpu_brandstring_D:
		DD		0x0

cpu_bs:
		DB		0x0d, 0x0a, 0x0a
		DB		" CPU Brand String:"

; VESA�����p�ł��Ȃ��ꍇ
error:
		MOV		BX,0x000c
		MOV		CX,99
		MOV		SI,vesa_err_msg
		CALL	message
		JMP		fin

; VESA��ʃ��[�h��ݒ�
VESA:
		MOV		AX,0x9000
		MOV		ES,AX
		MOV		DI,0
		MOV		AX,0x4f00		; VESA�C���t�H���[�V�����擾
		INT		0x10

		CMP		AL,0x4f
		JNE		error
		MOV		AX,[ES:DI+4]
		CMP		AX,0x0200
		JB		error


; VESA�ŉ�ʃ��[�h���g�p�ł��邩�ǂ����m�F
CheckVESA:
		MOV		AX,0x9000
		MOV		ES,AX
		MOV		DI,0
		MOV		CX,0x144		; 1024*768 32bit
		MOV		AX,0x4f01		; VESA��ʃ��[�h���擾
		INT		0x10

		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080
		JZ		error			; ���[�h������bit7��0�������̂ł�����߂�

		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		AX,[ES:DI+0x19]
		MOV		[VBITC],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		
SetVESA:
		MOV		AX,0x4f02		; VESA��ʃ��[�h�ݒ�
		MOV		BX,0xc144		; 800*600 32bit�J���[
		INT		0x10


keystatus:
		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL


; PIC����؂̊��荞�݂��󂯕t���Ȃ��悤�ɂ���
;	AT�݊��@�̎d�l�ł́APIC�̏�����������Ȃ�A
;	������CLI�O�ɂ���Ă����Ȃ��ƁA���܂Ƀn���O�A�b�v����
;	PIC�̏������͂��Ƃł��

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; OUT���߂�A��������Ƃ��܂������Ȃ��@�킪����炵���̂�
		OUT		0xa1,AL

		CLI						; �����CPU���x���ł����荞�݋֎~

; CPU����1MB�ȏ�̃������ɃA�N�Z�X�ł���悤�ɁAA20GATE��ݒ�

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout


; �v���e�N�g���[�h�ڍs

		LGDT	[GDTR0]			; �b��GDT��ݒ�
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; bit31��0�ɂ���i�y�[�W���O�֎~�̂��߁j
		OR		EAX,0x00000001	; bit0��1�ɂ���i�v���e�N�g���[�h�ڍs�̂��߁j
		MOV		CR0,EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8			;  �ǂݏ����\�Z�O�����g32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; kernel�̓]��

		MOV		ESI,kernel	; �]����
		MOV		EDI,KERNEL		; �]����
		MOV		ECX,512*1024/4
		CALL	memcpy

; ���łɃf�B�X�N�f�[�^���{���̈ʒu�֓]��

; �܂��̓u�[�g�Z�N�^����

		MOV		ESI,0x7c00		; �]����
		MOV		EDI,DSKCAC		; �]����
		MOV		ECX,512/4
		CALL	memcpy

; �c��S��

		MOV		ESI,DSKCAC0+512	; �]����
		MOV		EDI,DSKCAC+512	; �]����
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; �V�����_������o�C�g��/4�ɕϊ�
		SUB		ECX,512/4		; IPL�̕�������������
		CALL	memcpy

; asmhead�ł��Ȃ���΂����Ȃ����Ƃ͑S�����I������̂ŁA
;	���Ƃ�kernel�ɔC����

; kernel�̋N��

		MOV		EBX,KERNEL
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4;
		JZ		skip			; �]������ׂ����̂��Ȃ�
		MOV		ESI,[EBX+20]	; �]����
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; �]����
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; �X�^�b�N�����l
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; AND�̌��ʂ�0�łȂ����waitkbdout��
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; �����Z�������ʂ�0�łȂ����memcpy��
		RET
; memcpy�̓A�h���X�T�C�Y�v���t�B�N�X�����Y��Ȃ���΁A�X�g�����O���߂ł�������

		ALIGN	16, DB 0

GDT0:
		TIMES	8 DB 0				; �k���Z���N�^
		DW		0xffff,0x0000,0x9200,0x00cf	; �ǂݏ����\�Z�O�����g32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; ���s�\�Z�O�����g32bit�ikernel�p�j

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
kernel:
