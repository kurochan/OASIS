//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/*　アクアＵＩテスト用タスク */
#include "kernel.h"
#include <stdio.h>



void aqua_task(void) {

	struct WINDOW *window=windowctl.create(550,30,420,250,"Aqua UI Test") ;

	Aqua aqua;
	// 背景全体
	aqua.stripebox(window, 0, 0, 420, 250, 0xF3F3F3);

	// ボタン領域の背景
	short x = 10, y = 10, w = 400, h = 120;
	aqua.stripebox(window, x-1, y, w+2, h, 0xC0C0C0) ;
	aqua.stripebox(window, x, y-1, w, h+2, 0xC0C0C0) ;
	aqua.stripebox(window, x, y, w, h, 0xE0E0E0) ;
	windowctl.all_refresh(window) ;
/*
	short px = x+(w>>1)-40, py = y+h+10 ;

	color 128, 128, 128
	pos px, py
	g_mes "ボタンサンプル"
*/
	// カラーボタン見本
	for(short cnt=0; cnt<16; cnt++) {
		x = (cnt>>2) * 100 + 15 ;
		y = (cnt % 4) * 30 + 15 ;

		// ボタンの周囲の影
		aqua.stripebox(window, x-1, y, 90+2, 20, 0xC0C0C0) ;
		aqua.stripebox(window, x, y-1, 90, 20+2, 0xC0C0C0) ;

		UCHAR color[4];
		HSV2RGB( color[2], color[1], color[0], cnt*12, 128, 192 );
		color[3]=0;
		int *tmp=(int *)&color[0];

		if (cnt & 2) {
			aqua.glassbox(window, x, y, 90, 20, *tmp)	; 	// グラス
		} else {
			aqua.stripebox(window, x, y, 90, 20, *tmp)	;	// ストライプ
		}
	}

	// プログレスバー見本
	x = 10 ;
	y = 170 ;
	w = 400 ;
	h = 20 ;
	aqua.stripebox(window, x-1, y, w+2, h, 0xC0C0C0) ;
	aqua.stripebox(window, x, y-1, w, h+2, 0xC0C0C0) ;
	aqua.glassbox(window, x, y, w, h, 0xE0E0E0) ;

	windowctl.all_refresh(window) ;

	short px = x+(w>>1)-40, py = y+h+10 ;

	char s[6] ;
	Timer	timer2;
	timerctl.wait(&timer2,10);

	for (short cnt=0; cnt<w / 3; cnt++) {
		aqua.glassbox(window, x, y, cnt, h, 0x4A70E0) ;
		windowctl.refresh(window, x, y, cnt, h) ;

		aqua.stripebox(window, px, py, 100, 20, 0xF3F3F3) ;
		sprintf(s, "%d %%",cnt*100/w) ;
		g_mes(window, px, py, s) ;
		windowctl.refresh(window, px, py, 100, 20) ;

		timerctl.wait(&timer2,10);
	}
	taskctl.WaitIO(MAX_ID);					//半永久的に復帰する事はない。いわば、完全なスリープ状態
}

