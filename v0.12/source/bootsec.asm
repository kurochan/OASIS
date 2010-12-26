;=========================================
;         OASIS Kernel source code
;                Copyright (C) 2010 soS.A
;=========================================

; OASIS Boot Loader
; �u�[�g�Z�N�^��
; TAB=4

CYLS	EQU		10				; �ǂ��܂œǂݍ��ނ�
ORG		0x7c00			; ���̃v���O�������ǂ��ɓǂݍ��܂��̂�


[SECTION .text]

; �ȉ��͕W���I��FAT12�t�H�[�}�b�g�t���b�s�[�f�B�X�N�̂��߂̋L�q

		JMP		SHORT	start
		DB		0x90
		DB		"OASIS   "		; �u�[�g�Z�N�^�̖��O�����R�ɏ����Ă悢�i8�o�C�g�j
		DW		512				; 1�Z�N�^�̑傫���i512�ɂ��Ȃ���΂����Ȃ��j
		DB		1				; �N���X�^�̑傫���i1�Z�N�^�ɂ��Ȃ���΂����Ȃ��j
		DW		1				; FAT���ǂ�����n�܂邩�i���ʂ�1�Z�N�^�ڂ���ɂ���j
		DB		2				; FAT�̌��i2�ɂ��Ȃ���΂����Ȃ��j
		DW		224				; ���[�g�f�B���N�g���̈�̑傫���i���ʂ�224�G���g���ɂ���j
		DW		2880			; ���̃h���C�u�̑傫���i2880�Z�N�^�ɂ��Ȃ���΂����Ȃ��j
		DB		0xf0			; ���f�B�A�̃^�C�v�i0xf0�ɂ��Ȃ���΂����Ȃ��j
		DW		9				; FAT�̈�̒����i9�Z�N�^�ɂ��Ȃ���΂����Ȃ��j
		DW		18				; 1�g���b�N�ɂ����̃Z�N�^�����邩�i18�ɂ��Ȃ���΂����Ȃ��j
		DW		2				; �w�b�h�̐��i2�ɂ��Ȃ���΂����Ȃ��j
		DD		0				; �p�[�e�B�V�������g���ĂȂ��̂ł����͕K��0
		DD		2880			; ���̃h���C�u�傫����������x����
		DB		0,0,0x29		; �悭�킩��Ȃ����ǂ��̒l�ɂ��Ă����Ƃ����炵��
		DD		0xffffffff		; ���Ԃ�{�����[���V���A���ԍ�
		DB		"OASIS v0.01"	; �f�B�X�N�̖��O�i11�o�C�g�j
		DB		"FAT12   "		; �t�H�[�}�b�g�̖��O�i8�o�C�g�j
		TIMES	18 DB 0				; �Ƃ肠����18�o�C�g�����Ă���

; �v���O�����{��

start:
		MOV		AX,0
		MOV		SS,AX
		MOV		DS,AX
		MOV		SP,0x7c00

		MOV		AX,0x03
		INT		0x10

		MOV		SI,startup
		MOV		BX,0x0007
		MOV		CX,61
		CALL	message


l_disk:
; �f�B�X�N��ǂ�
		MOV		AX,0x0820
		MOV		ES,AX
		MOV		DH,0			; �w�b�h0
		MOV		CH,0			; �V�����_0
		MOV		CL,2			; �Z�N�^2
		MOV		DL,0

readloop:
		MOV		SI,0			; ���s�񐔂𐔂��郌�W�X�^
retry:
		MOV		AH,0x02			; AH=0x02 : �f�B�X�N�ǂݍ���
		MOV		AL,1			; 1�Z�N�^
		MOV		BX,0
		INT		0x13			; �f�B�X�NBIOS�Ăяo��
		JNC		next			; �G���[�������Ȃ����next��
		INC		SI				; SI��1�𑫂�
		CMP		SI,4			; SI��4���r
		JA		fin		; SI > 4 ��������error��(SI >= 5)
		MOV		AH,0x00
		INT		0x13			; �h���C�u�̃��Z�b�g
		JMP		retry

next:
		MOV		AX,ES			; �A�h���X��0x20�i�߂�
		ADD		AX,0x0020
		MOV		ES,AX			; ADD ES,0x020 �Ƃ������߂��Ȃ��̂ł������Ă���
		INC		CL				; CL��1�𑫂�
		CMP		CL,19			; CL��19���r
		JB		readloop		; CL < 19 ��������readloop��
		MOV		CL,1
		INC		DH
		CMP		DH,2
		JB		readloop		; DH < 2 ��������readloop��
		MOV		DH,0
		INC		CH
		CMP		CH,CYLS
		JB		readloop		; CH < CYLS ��������readloop��

; �ǂݏI������̂�haribote.sys�����s���I
		MOV		[0x0ff0],CH		; IPL���ǂ��܂œǂ񂾂̂�������
		JMP		0xc200


diskerror:
		MOV		BX,0x000c
		MOV		CX,64
		MOV		SI,disk_err_msg
		CALL	message
		JMP		fin

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

fin:
		HLT
		JMP		fin

startup:
		DB		0x0d,0x0a
		DB		"==================="
		DB		0x0d,0x0a
		DB		"    OASIS v0.02"
		DB		0x0d,0x0a
		DB		"==================="
		DB		0x0d,0x0a

disk_err_msg:
		DB		0x0d,0x0a
		DB		" Disk load failed."
		DB		0x0d,0x0a
		DB		" Press Reset key to restart your computer."

		TIMES 0x1fe-($-$$) DB 0		; 0x7dfe�܂ł�0x00�Ŗ��߂閽��

		DB		0x55, 0xaa