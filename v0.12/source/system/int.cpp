//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* ���荞�݊֌W */

#include "kernel.h"


/* PIC�̏����� */
void init_pic(void) {

	io_out8(PIC0_IMR,  0xff  );		// �S�Ă̊��荞�݂��󂯕t���Ȃ�
	io_out8(PIC1_IMR,  0xff  );		// �S�Ă̊��荞�݂��󂯕t���Ȃ�

	io_out8(PIC0_ICW1, 0x11  );	// �G�b�W�g���K���[�h
	io_out8(PIC0_ICW2, 0x20  );	// IRQ0-7�́AINT20-27�Ŏ󂯂�
	io_out8(PIC0_ICW3, 1 << 2);	// PIC1��IRQ2�ɂĐڑ�
	io_out8(PIC0_ICW4, 0x01  );	// �m���o�b�t�@���[�h

	io_out8(PIC1_ICW1, 0x11  );	// �G�b�W�g���K���[�h
	io_out8(PIC1_ICW2, 0x28  );	// IRQ8-15�́AINT28-2f�Ŏ󂯂�
	io_out8(PIC1_ICW3, 2     );		// PIC1��IRQ2�ɂĐڑ�
	io_out8(PIC1_ICW4, 0x01  );	// �m���o�b�t�@���[�h

	io_out8(PIC0_IMR,  0xfb  );		// 11111011 PIC1�ȊO�͑S�ċ֎~
	io_out8(PIC1_IMR,  0xff  );		// 11111111 �S�Ă̊��荞�݂��󂯕t���Ȃ�

	return;
}

/* ���荞�݂����� */
void allow_interrupt(void) {
	io_out8(PIC0_IMR, 0xf8);		// PIT��PIC1�ƃL�[�{�[�h������(11111000)
	io_out8(PIC1_IMR, 0xef);		// �}�E�X������(11101111)
	io_sti();								// ���荞�݂�����

	return ;
}

/* FPU���Z����CPU����̊��荞�� */
void inthandler07(int *esp) {
	taskctl.inthandler07();
	return;
}

/* �^�C�}�[����̊��荞�� */
void inthandler20(int *esp) {
	io_out8(PIC0_OCW2, 0x60);	// IRQ-00��t������PIC�ɒʒm
	timerctl.inthandler20(esp) ;		// ������timerctl�N���X��
	return;
}

/* PS/2�L�[�{�[�h����̊��荞�� */
void inthandler21(int *esp) {
	io_out8(PIC0_OCW2, 0x61);	// IRQ-01��t������PIC�ɒʒm
	inputctl->i_put(io_in8(PORT_KEYDAT) + (KEYBOARD_ID<<8));		// �M����inputctl�e�h�e�n��
	return ;
}

/* PS/2�}�E�X����̊��荞�� */
void inthandler2c(int *esp) {
	io_out8(PIC1_OCW2, 0x64);	// IRQ-12��t������PIC1�ɒʒm
	io_out8(PIC0_OCW2, 0x62);	// IRQ-02��t������PIC0�ɒʒm

	inputctl->i_put(io_in8(PORT_KEYDAT) + (MOUSE_ID_S<<8));			// �M����inputctl�e�h�e�n��
	return;
}

/* PIC0����̕s���S���荞�ݑ΍� */
void inthandler27(int *esp) {
	io_out8(PIC0_OCW2, 0x67);				// IRQ-07��t������PIC�ɒʒm(7-1�Q��)
	return;
}
// Athlon64X2�@�Ȃǂł̓`�b�v�Z�b�g�̓s���ɂ��PIC�̏��������ɂ��̊��荞�݂�1�x����������
// ���̊��荞�ݏ����֐��́A���̊��荞�݂ɑ΂��ĉ������Ȃ��ł��߂���
// �Ȃ��������Ȃ��Ă����́H
// ��  ���̊��荞�݂�PIC���������̓d�C�I�ȃm�C�Y�ɂ���Ĕ����������̂Ȃ̂ŁA
// �܂��߂ɉ����������Ă��K�v���Ȃ��B
