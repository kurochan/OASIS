//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/*�@�A�C�h���^�X�N */
#include "kernel.h"
#include <stdio.h>


//�^�C�}�Ǘ�
//���̃^�X�N�ȊO�ł́Atimerctl.checkTimeout()���Ă΂Ȃ����I
void timer_task(void) {

	Caller caller(timerctl,&TIMERCTL::checkTimeout);
	taskctl.set_input(TIMER_ID_S,&caller);

	taskctl.waitIO(MAX_ID);					//���i�v�I�ɕ��A���鎖�͂Ȃ��B����΁A���S�ȃX���[�v���

	return ;
}

