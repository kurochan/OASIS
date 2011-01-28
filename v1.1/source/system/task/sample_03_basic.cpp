//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 Liva
//=========================================

#include "kernel.h"
#include <stdlib.h>
#include <math.h>

Timer	timer2;
Caller *caller;

#define MAX 16

void sample_03_basic(void) {

	struct WINDOW *window=windowctl.create(20,250,640,480,"S.Programs d3module sample") ;
	HSP hsp(window);
	d3module d3(hsp);

	double x[MAX], y[MAX], z[MAX];
	UINT count=0;

	for( int _cnt=0;; _cnt++ ){

		hsp.color( 255, 255, 255 );
		hsp.boxf();

		// ボックスの座標を生成
		int ct = _cnt ;
		srand(0);

		for (int cnt=0; cnt<MAX; cnt++) {
			x[cnt] = cos( 0.002 * (rand()%10 + 10) * ct ) * 600;
			y[cnt] = sin( 0.002 * (rand()%10 + 10) * ct ) * 600;
			z[cnt] = sin( 0.002 * (rand()%10 + 10) * ct ) * 60;
		}



		// カメラ座標設定 (自動切換え)
		if ((_cnt / 150) & 1) {

			// 移動カメラ
			d3.setcam( x[0], y[0], z[0],  x[1], y[1], z[1] ) ;

		} else {

			// ふかんカメラ
			d3.setcam( cos(0.01 * ct) * 1500, sin(0.01 * ct) * 1500, 1000 );
		}



		// 座標平面描画
		hsp.color( 64, 192, 64 ) ;

		for (int cnt=0; cnt<31; cnt++){
			double a = cnt * 2000 / 30 - 1000 ;
			d3.line( a, 1000, 0,  a, -1000, 0 );
			d3.line( 1000, a, 0,  -1000, a, 0 );
		}



		// 座標軸描画
		hsp.color( 32, 128, 32 );

		d3.mes( "X",  2200, 0, 0 );
		d3.mes( "Y",  0, 2200, 0 );
		d3.mes( "Z",  0, 0, 1100 );

		d3.arrow( 2000, 0, 0,  -2000, 0, 0) ; //x
		d3.arrow( 0, 2000, 0,  0, -2000, 0) ; //y
		d3.arrow( 0, 0, 1000,  0, 0, -1000) ; //z



		// ボックスを描画
		hsp.color ( 0, 0, 255 ) ;
		char r = 30 ;
		for( int cnt=0; cnt<MAX; cnt++ )
			d3.box( x[cnt] - r, y[cnt] - r, z[cnt] - r, x[cnt] + r, y[cnt] + r, z[cnt] + r ) ;



		// 箱 0 (カメラ位置) を円で囲む
		hsp.color( 0, 0, 128 );
		d3.circle( x[0], y[0], z[0],  100 );



		// 箱 1 (ターゲット座標) を円で囲む
		hsp.color( 128, 0, 0 ) ;
		d3.circle( x[1], y[1], z[1],  100 );


		static char x=0;
		x++;
		if (x==2){
			windowctl.all_refresh(window);
			x=0;
		}
		if (DEBUGG) {
			static int x=0;
			x++;
			sinfo->vram[1024*120+x]=0xff0000;
		}
		count = timerctl.GetCount() - count ;
		if (count>=3){
			continue;
		}else{
			count=3-count;
		}
		Timer xtimer;
		timerctl.wait(&xtimer,count);
		count=timerctl.GetCount();

	}


	taskctl.WaitIO(MAX_ID);					//半永久的に復帰する事はない。いわば、完全なスリープ状態

}
