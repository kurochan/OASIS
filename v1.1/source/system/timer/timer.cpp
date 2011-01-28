//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* タイマ関係 */

#include "kernel.h"
#include <stdio.h>

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

extern char cursor ;

void init_pit(void) {
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	return;
}


Timer::Timer(void){
	_using=0;
}

void Timer::set_timeout(UINT time, UINT count) {
	pid=taskctl.GetPid();
	timeout=time+count;
	return ;
}

TIMERCTL timerctl;
bool TIMERCTL::alreadyExist = 0;

/* タイマコントローラの初期化 */
// 番兵設置など
void TIMERCTL::init(void) {

	if(alreadyExist)
		panic();
	alreadyExist = 1;

	count=0;
	_guardTimer.set_timeout(0xFFFFF000,count) ;
	//guard.timeout=0xFFFFF000にする (この関数を呼ぶ時点ではタイマ割り込みを許可していないので、count=0)
	//0xFFFFFFFFにしてないのは、timerのリセット処理をする際の時間を考えて、余裕を持たせたから

	_firstTimerAddr=&_guardTimer ;
	_guardTimer.next_timer=(Timer *)0 ;
	_guardTimer.pid=0;
//	_resetFunc.SetFunction(*this,&TIMERCTL::reset);
//	_guardTimer.FuncOfTimeout=&_resetFunc;

	return ;
}


void TIMERCTL::create(Timer *c_timer, UINT time, Caller *caller) {

	if (c_timer->_using==1)
		return;

	c_timer->_using=1;

	Timer *seek1, *seek2=_firstTimerAddr ;
	io_cli() ;

	c_timer->set_timeout(time,count) ;
	c_timer->FuncOfTimeout=caller;
	if (c_timer->timeout<time)
		reset(0);


	if (c_timer->timeout<=seek2->timeout) {
		//一番最初に挿入する場合
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

void TIMERCTL::wait(Timer *c_timer, UINT time) {
	create(c_timer, time, NULL);
	taskctl.WaitIO(TIMER_ID);
	return ;
}


void TIMERCTL::reset(UCHAR tmp) {
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

//登録済みタイマの処理
//　引数のｔｍｐはインターフェイス統一用の仮引数。よって、何もしてやる必要はない
void TIMERCTL::checkTimeout(UCHAR tmp) {

	Timer *seek=_firstTimerAddr, *seek2;

	for(;;) {
		if (DEBUGG) {
			static int x=0;
			x++;
			sinfo->vram[1024*100+x]=0xff00;
		}
		if ( seek->timeout > count ){
			break ;
		}

		//Execute内でtimerctl.createが呼び出された時の対策
		seek2=seek->next_timer ;
		_firstTimerAddr=seek2 ;
		seek->_using=0;
		if (seek->FuncOfTimeout==NULL){
			taskctl.ContinueProc(seek->pid);
		}else{
			seek->FuncOfTimeout->Execute(0) ;
		}
		if (_firstTimerAddr!=seek)
			seek=seek2 ;
	}

	return ;
}

//タイマ割り込みの処理
void TIMERCTL::inthandler20(int *esp) {
	count++;

	//設定されたタイマの時刻を過ぎた！
	if ( _firstTimerAddr->timeout <= count ){
		inputctl->i_put(TIMER_ID_S<<8);		// 信号をinputctlFIFOへ
		taskctl.Tswitch_IO();
	}

	io_sti();
	if (count>=taskctl.next_tswitch)
		taskctl.Tswitch_F();

	return ;
}

//タイマカウンタが指定時刻を越えていないか調べる
//戻り値
//1:超えている
//0:超えていない
char TIMERCTL::check_count(UINT c_count)  const {
	if (c_count>count)
		return 0;

	return 1;
}

UINT TIMERCTL::GetCount() const{
	return count;
}

