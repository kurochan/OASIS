//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 Liva
//=========================================

/*　アクアＵＩテスト用タスク */
#include "kernel.h"
#include <stdio.h>
/*
char g_continueFlag;

*/
Timer	timer2;
Caller *caller;
//void await(UCHAR tmp) ;


void sample_02_basic(void) {

	struct WINDOW *window=windowctl.create(20,250,640,480,"S.Programs d3module sample") ;

	HSP hsp(window);
	d3module d3(hsp);



	short camx, camy, camz = 400 ;
/*
	caller=new Caller(await);
	timerctl.create(&timer2,30, caller) ;

	g_continueFlag=1;*/
	for(;;) {

		// カメラ座標準備

		// マウスボタン
//		getkey a, 1 : camz += a * 100
//		getkey a, 2 : camz -= a * 100
		// ホイール
//		camz += mousew * 2 ;

		camx = (hsp.mousex() - hsp.ginfo_winx() / 2) * 20 ;
		camy = (hsp.mousey() - hsp.ginfo_winy() / 2) * 20 ;


		// カメラ位置設定

		d3.setcam( camx, camy, camz,  0, 0, 0 ) ;



		hsp.color(0xff,0xff,0xff);
		hsp.boxf();


		// 座標軸描画
		hsp.color(0,0,0) ;

		//------------------------------------------------------------
		// メッセージを表示

		d3.mes( "X",  1100, 0, 0 );
		d3.mes( "Y",  0, 1100, 0 );
		d3.mes( "Z",  0, 0, 1100 );

		//      │   └┬───┘
		//      │    └ 表示する位置 (x, y, z)
		//      └ 表示する文字列
		//------------------------------------------------------------

		//------------------------------------------------------------
		// 矢印を描画

		d3.arrow( 1000, 0, 0,   -1000, 0, 0 );
		d3.arrow( 0, 1000, 0,   0, -1000, 0 );
		d3.arrow( 0, 0, 1000,   0, 0, -1000 );

		//        └┬───┘        └┬───┘
		//         │             └ 矢印の始点の位置 (x, y, z)
		//         └ 矢印の終点 (矢じり部分) の位置 (x, y, z)
		//------------------------------------------------------------


		// 具を描画
		hsp.color(0x80,0x80,0x80);

		//------------------------------------------------------------
		// 直線 (線分) を描画

		d3.line( 0, 0, 0,  -500, -500, -500) ;

		//        └┬─┘    └┬──────┘
		//         │       └ 線の端 2 の位置 (x, y, z)
		//         └ 線の端 1 の位置 (x, y, z)
		//------------------------------------------------------------

		//------------------------------------------------------------
		// 円 (球?) を描画

		d3.circle( -500, -500, -500,   300 );

		//            └┬───────┘         │
		//             │                 └ 円の半径
		//             └ 円の中心位置 (x, y, z)
		//------------------------------------------------------------


		d3.mes( "-500, -500, -500", -500, -500, -500 ) ;


		// ボックス 1 を描画
		hsp.color(0x00,0x00,0xff);
		d3.box( 100, 100, 100,  500, 500, 500 );


		// ボックス 2 を描画
		hsp.color(0xff,0x00,0x00);
		d3.box( 500, 500, 500,  1000, 1000, 1000 );


/*
		g_continueFlag--;
		for(;;) {
			taskctl.tswitch();
			if (g_continueFlag)
				break;
			asm("hlt");
		}
*/

		windowctl.all_refresh(window);
		Timer xtimer;
		timerctl.wait(&xtimer,3);
	}




/*	//------------------------------------------------------------
	// カメラ位置設定

	d3.setcam( 1400, 1200, 1000,  0, 0, 0 ) ;

	//          └┬──────┘         └┬──┘
	//           │                 └ カメラが見ている位置 (x, y, z)
	//           └ カメラの位置 (x, y, z)
	//-----------------------------------------------------------

	//------------------------------------------------------------
	// ボックスを描画 (すべての辺は座標軸に平行)

//	d3.box( 500, 500, 500,   0, 0, 0) ;

	//      └┬────┘        └┬──┘
	//       │              └ 端の座標 2 (x, y, z)
	//       └ 端の座標 1 (x, y, z)
	//------------------------------------------------------------
	Timer xtimer;
	timerctl.wait(&xtimer,300);
	for(int cnt=0;cnt<0xFFFFF;cnt++)
		hsp.circle(20,30,300,400);

	char s[40];
	sprintf(s, "%010d", timerctl.count);
	g_mes(window,25,5,s);*/

	windowctl.all_refresh(window);

	taskctl.waitIO(MAX_ID);					//半永久的に復帰する事はない。いわば、完全なスリープ状態
}
/*
void await(UCHAR tmp) {
	timerctl.create(&timer2,3, caller) ;
	g_continueFlag++;
	return ;
}
*/

