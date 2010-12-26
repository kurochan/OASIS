//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/*　アイドルタスク */
#include "kernel.h"
#include <stdio.h>


//タイマ管理
//このタスク以外では、timerctl.checkTimeout()を呼ばない事！
void timer_task(void) {

	Caller caller(timerctl,&TIMERCTL::checkTimeout);
	taskctl.set_input(TIMER_ID_S,&caller);

	taskctl.waitIO(MAX_ID);					//半永久的に復帰する事はない。いわば、完全なスリープ状態

	return ;
}

