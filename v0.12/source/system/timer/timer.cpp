//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* �^�C�}�֌W */

#include "kernel.h"
#include <stdio.h>

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

extern char cursor ;
TIMERCTL timerctl;

void init_pit(void) {
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	return;
}


Timer::Timer(void){
	_using=0;
}

void Timer::set_timeout(unsigned int time, unsigned int count) {
	pid=taskctl.get_pid();
	timeout=time+count;
	return ;
} 

/* �^�C�}�R���g���[���̏����� */
// �ԕ��ݒu�Ȃ�
void TIMERCTL::init(void) {

	_guardTimer.set_timeout(0xFFFFF000,count) ;
	//guard.timeout=0xFFFFF000�ɂ��� (���̊֐����ĂԎ��_�ł̓^�C�}���荞�݂������Ă��Ȃ��̂ŁAcount=0)
	//0xFFFFFFFF�ɂ��ĂȂ��̂́Atimer�̃��Z�b�g����������ۂ̎��Ԃ��l���āA�]�T��������������

	_firstTimerAddr=&_guardTimer ;
	_guardTimer.next_timer=(Timer *)0 ;
	_guardTimer.pid=0;
	count=0;

	return ;
}


void TIMERCTL::create(Timer *c_timer, unsigned int time, Caller *caller) {

	if (c_timer->_using==1)
		return;
	
	c_timer->_using=1;

	Timer *seek1, *seek2=_firstTimerAddr ;
	io_cli() ;

	c_timer->set_timeout(time,count) ;
	c_timer->FuncOfTimeout=caller;
	if (c_timer->timeout<time)
		reset();


	if (c_timer->timeout<=seek2->timeout) {
		//��ԍŏ��ɑ}������ꍇ
		_firstTimerAddr=c_timer ;
		c_timer->next_timer=seek2 ;
		io_sti() ;
		return ;
	}
	
	for(;;) {
		seek1=seek2 ;
		seek2=seek1->next_timer ;
		if (c_timer->timeout<=seek2->timeout) {
			seek1->next_timer=c_timer ;
			c_timer->next_timer=seek2 ;
			io_sti() ;
			return ;
		}
	}
}

void TIMERCTL::wait(Timer *c_timer, unsigned int time) {
	create(c_timer, time, NULL);
	taskctl.waitIO(TIMER_ID);
	taskctl.tswitch();
	return ;
}


void TIMERCTL::reset(void) {
	io_cli();
	Timer *seek=_firstTimerAddr ;
	for(;;) {
		seek->timeout-=count ;
		seek=seek->next_timer ;
		if (seek==(Timer *)0)
			break ;
	}
	taskctl.next_tswitch-=count;
	count=0 ;
	io_sti() ;

	return ;
}

//�o�^�ς݃^�C�}�̏���
//�@�����̂������̓C���^�[�t�F�C�X����p�̉������B����āA�������Ă��K�v�͂Ȃ�
void TIMERCTL::checkTimeout(UCHAR tmp) {

	Timer *seek=_firstTimerAddr, *seek2;

	for(;;) {
		if ( seek->timeout > count )
			break ;

		//Execute����timerctl.create���Ăяo���ꂽ���̑΍�
		seek2=seek->next_timer ;
		_firstTimerAddr=seek2 ;
		seek->_using=0;
		if (seek->FuncOfTimeout==NULL){
			taskctl.continueProcess(seek->pid);
		}else{
			seek->FuncOfTimeout->Execute(0) ;
		}
		if (_firstTimerAddr!=seek)
			seek=seek2 ;
	}

	return ;
}

//�^�C�}���荞�݂̏���
void TIMERCTL::inthandler20(int *esp) {
	count++;

	//�ݒ肳�ꂽ�^�C�}�̎������߂����I
	if ( _firstTimerAddr->timeout <= count )
		inputctl->i_put(TIMER_ID_S<<8);		// �M����inputctlFIFO��

	if ((count&1)==1)
		taskctl.tswitch_F();

	return ;
}

//�^�C�}�J�E���^���w�莞�����z���Ă��Ȃ������ׂ�
//�߂�l
//1:�����Ă���
//0:�����Ă��Ȃ�
char TIMERCTL::check_count(unsigned int c_count)  const {
	if (c_count>count)
		return 0;

	return 1;
}
