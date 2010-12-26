//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* task_b */

#include "kernel.h"
#include <stdio.h>


void counter_task(void){
	unsigned int count=0;
	char s[40];

	/* タイマモニタウィンドウ作成 */
	struct WINDOW *window2=windowctl.create(750,350,140,40,"Timer Monitor") ;

	Aqua aqua;
	// 背景全体
	aqua.stripebox(window2, 0, 0, 140, 40, 0xF3F3F3);

	windowctl.all_refresh(window2) ;
	
	for(;;) {
		if (timerctl.check_count(300))
			break ;
		//タイマカウンタの表示
		aqua.stripebox(window2, 25, 5, 80, 15, 0xF3F3F3);
		sprintf(s, "%010d", timerctl.count);
		mes(window2,25,5,s);
		windowctl.refresh(window2, 25, 5, 80, 20) ;
	}

	//高速カウンタ
	for (;;) {
	
		//タイマカウンタの表示
/*		color(220,220,220) ;
		boxf(window2,20,5,94,15) ;
		sprintf(s, "%010d", timerctl.count);
		mes(window2,20,5,s);
		shtctl.refresh(window2, 20, 5, 94, 15) ;
*/
		count++ ;
		if (timerctl.check_count(1000)){
			//高速カウンタの表示
			aqua.stripebox(window2, 25, 25, 80, 15, 0xF3F3F3);
			sprintf(s, "%010d", count);
			mes(window2,25,25,s);
			windowctl.refresh(window2, 25, 25, 94, 15) ;
		}
	}
}
