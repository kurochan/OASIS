//=========================================
//	  OASIS Kernel main source code
//				Copyright (C) 2010 soS.A
//=========================================

#include "kernel.h"


//======================
//  �N���X
//======================
Memory 		*mem;			// �������Ǘ��p�N���X
INPUTCTL	*inputctl;		// �O�����̓f�o�C�X����̐M���𗭂߂�FIFO�o�b�t�@

//======================
//  �\����
//======================
struct S_INFO *sinfo = (struct S_INFO *) 0x00000ff0 ;

//======================
//  �֐��錾
//======================
void system_init(void) ;

//======================
//  �֐��{��
//======================

/*�@asmhead.nas���炱�����Ăяo����� */
void KernelMain(void) {
/*	if (sinfo->vbitc!=32)
		for(;;)
			io_hlt();						//@��O����*/

	/* �V�X�e�������� */
	Memory mem_def ;		//�@�������e�ʃ`�F�b�N�{�������Ǘ��V�X�e��������
	mem=&(mem_def);			//�@new��mem.alloc���Ăяo�����ɂȂ邽�߁AMemory�N���X�̏�������new���g���Ă͂Ȃ�Ȃ�
	//�@���̂Q�s��system_init�Ɏ��߂��������̂����A����������mem_def���J������Ă��܂��̂ŁA�s�\

	system_init() ;
	// init�n�֐������܂�
	idle_task();
}

/* �V�X�e���������n�����W�� */
void system_init(void) {

	init_gdtidt() ;						// GDT,IDT������
	init_pic() ;						// PIC������

	timerctl.init() ;					// �^�C�}�R���g���[���̏�����

	init_pit();							// �^�C�}������

	init_keyboard();					// �L�[�{�[�h������
	// �}�E�X�̏�������main_task�ł��

	inputctl=new INPUTCTL;	// �O�����̓f�o�C�X����̐M���𗭂߂�FIFO�o�b�t�@���쐬

	taskctl.init((int) &main_task);						// �^�X�N�R���g���[���̏�����

	boot_GUI() ;

	//�^�X�N�ǉ�
	taskctl.create((int) &timer_task);	//�^�C�}����
//	taskctl.create((int) &counter_task);	//�J�E���^
	taskctl.create((int) &aqua_task);	//AquaGUI�e�X�g
	taskctl.create((int) &sample_02_basic);	//d3module�e�X�g

	return ;

}

