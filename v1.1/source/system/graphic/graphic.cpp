//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* 画面描画関係 */
#include "kernel.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

HSP::HSP(WINDOW *window) {
	_color=0;
	_window=window ;
	_fontx=10;
	_fonty=20;
}

//円を描画する
//Sx=0～(0)  : 矩形の左上X座標
//Sy=0～(0)  : 矩形の左上Y座標
//Ex=0～     : 矩形の右下X座標
//Ey=0～     : 矩形の右下Y座標
//flag=0～1(1) : 描画モード(0=線,1=塗りつぶし)
//Original source:http://ynomura.dip.jp/archives/2008/07/post_31.html
//Thanks for ynomura

/* Considering that Ox is 1/2 dot right to center if Wx is even
   -> ellipse is symmetric to x=-0.5 (same y for x=a and x=-1+a)
      X range is (Ox-Wx/2) - (Ox+Wx/2-1)
   The same goes for Oy if Wy is even */
void HSP::circle( short Sx, short Sy, short Ex, short Ey,  bool flag){

	swapvertex( Sx, Sy, Ex, Ey );

	short Wx=Ex-Sx;
	short Wy=Ey-Sy;
	int Ox = Sx + (Wx >> 1);	// Sx + Wx/2
	int Oy = Sy + (Wy >> 1);	// Sy + Wy/2
	int Wx2 = (Wx-1)*(Wx-1);	// 4 * (Wx/2 - 1/2)^2
	int Wy2 = (Wy-1)*(Wy-1);	// 4 * (Wy/2 - 1/2)^2
	int _Ex = ~Wx & 1;	// 1 if Wx is even
	int _Ey = ~Wy & 1;	// 1 if Wy is even
	int x, y, err, dedx, dedy;

	// plot from X axis ((Ox, Oy)+(Wx/2, 0))
	x = (Wx >> 1) - _Ex;	// Wx/2-1 if Wx is even
	y = 0;
	dedx = 2 * Wy2 * (x-1);	// Wy^2 * (2x-2)
	if (_Ey == 0){
		// center line is y=0
		dedy = Wx2;	// Wx^2 * (2*y+1) = Wx^2
		err = - Wy2*x + (Wy2 >> 2); // Wy^2 * (-x+1/4)
	}else{
		// center line is y=-0.5
		dedy = 2 * Wx2;	// Wx^2 * (2*y+2) = 2Wx^2
		err = - Wy2*x + ((Wx2+Wy2) >> 2); // Wy^2 * (-x+1/4) + Wx^2 * (1/4)
	}

	if (flag){
		boxf( Ox, Oy-1, Ox + x, Oy );
		boxf( Ox, Oy-1, Ox - x - _Ex, Oy );
	}else{
		pset( Ox + x, Oy );
		pset( Ox - x - _Ex, Oy );
	}
	pset( Ox + x, Oy);
	pset( Ox - x - _Ex, Oy);
	if (_Ey == 1){
		// y=0 and y=-1 are symmetric
		if (flag){
			boxf( Ox, Oy-1, Ox + x, Oy - 1);
			boxf( Ox, Oy-1, Ox - x - _Ex, Oy - 1);
		}else{
			pset( Ox + x, Oy - 1);
			pset( Ox - x - _Ex, Oy - 1);
		}
	}

	while(dedx >= dedy){	// until tangent line is 135 degree
		y++;
		err += dedy;
		dedy += Wx2 << 1;	// 2Wx^2

		if (err > 0){
			x--;
			err -= dedx;
			dedx -= Wy2 << 1;	// 2Wy^2
		}

		if (flag){
			boxf( Ox, Oy+y, Ox + x, Oy + y);
			boxf( Ox, Oy - y - _Ey, Ox + x, Oy - y - _Ey);			// X axis mirror
			boxf( Ox, Oy + y, Ox - x - _Ex, Oy + y);				// Y axis mirror
			boxf( Ox, Oy - y - _Ey, Ox - x - Ex, Oy - y - _Ey);	// mirror^2
		}else{
			pset( Ox + x, Oy + y);
			pset( Ox + x, Oy - y - _Ey);	// X axis mirror
			pset( Ox - x - _Ex, Oy + y);	// Y axis mirror
			pset( Ox - x - _Ex, Oy - y - _Ey); // mirror^2
		}
	}

	// plot from (0,Wy)
	x = 0;
	y = (Wy >> 1) - _Ey;	// Wy/2-1 if Wy is even
	dedy = 2 * Wx2 * (y-1);	// Wx^2 * (2y-2)
	dedx = Wy2;		// Wy^2 * (2*x+1) = Wy^2
	err = - Wx2*y + (Wx2 >> 2); // Wx^2 * (-y+1/4)
	if (_Ex == 0){
		// center line is x=0
		dedx = Wy2;		// Wy^2 * (2*x+1) = Wy^2
		err = - Wx2*y + (Wx2 >> 2); // Wx^2 * (-y+1/4)
	}else{
		// center linke is x=-0.5
		dedx = 2 * Wy2;		// Wy^2 * (2*x+2) = 2Wy^2
		err = - Wx2*y + ((Wx2+Wy2) >> 2); // Wx^2 * (-y+1/4) + Wy^2 * (1/4)
	}

	pset( Ox, Oy + y);
	pset( Ox, Oy - y - _Ey);
	if (_Ex == 1){
		// x=0 and x=-1 are symmetric
		pset( Ox - 1, Oy + y);
		pset( Ox - 1, Oy - y - _Ey);
	}

	while(dedx <= dedy){	// until tangent line is 135 degree
		x++;
		err += dedx;
		dedx += Wy2 << 1;	// 2Wy^2

		if (err > 0){
			y--;
			err -= dedy;
			dedy -= Wx2 << 1;	// 2Wx^2
		}

		if (flag){
			boxf( Ox, Oy + y, Ox + x, Oy + y);
			boxf( Ox, Oy - y - _Ey, Ox + x, Oy - y - _Ey);	// X mirror
			boxf( Ox, Oy + y, Ox - x - Ex, Oy + y);	// Y mirror
			boxf( Ox, Oy - y - _Ey, Ox - x - _Ex, Oy - y - _Ey);	// mirror^2
		}else{
			pset( Ox + x, Oy + y);
			pset( Ox + x, Oy - y - _Ey);	// X mirror
			pset( Ox - x - _Ex, Oy + y);	// Y mirror
			pset( Ox - x - _Ex, Oy - y - _Ey);	// mirror^2
		}
	}
	return;
}

void HSP::line( short x1, short y1, short x2, short y2) {
	if (x2==-32768)
		x2=_current_posx;
	if (y2==-32768)
		y2=_current_posy;

	_current_posx=x1;
	_current_posy=y1;

	short dx=x2-x1, dy=y2-y1 ;
	x2++;
	short mx=1, my=1;
	if (dx<0){
		dx*=-1;
		mx=-1;
		x2-=2;
	}
	if (dy<0){
		dy*=-1;
		my=-1;
	}
	dx++;
	dy++;
	short y=y1 ;
	short cnty=0 ;


	for (short x=x1; x!=x2; x+=mx ) {
		pset(x,y) ;
		cnty+=dy ;
		if (cnty>=dx) {
			y+=my ;
			cnty-=dx ;
		}
		for(;cnty>=dx; cnty-=dx ) {
			pset(x,y);
			y+=my ;
		}
	}

	return ;
}

short HSP::mousex(void) {
	return mouse->vx ;
}

short HSP::mousey(void) {
	return mouse->vy ;
}

/* 矩形塗りつぶし */
void g_boxf(struct SHEET *sheet, short vx, short vy, short xsize, short ysize, int color) {

	//指定箇所がシート外の場合は補正
	if ((vx+xsize-1)>=sheet->xsize)
		xsize=sheet->xsize-vx ;
	if ((vy+ysize-1)>=sheet->ysize)
		ysize=sheet->ysize-vy ;
	if (vx<0) {
		xsize+=vx;
		vx=0;
	}
	if (vy<0) {
		ysize+=vy;
		vy=0;
	}

	if (xsize<=0||ysize<=0)
		return ;	//値が不正だった時以外にも、範囲すべてがシート外に存在する場合はここに来る事もアリ

	int *buf=sheet->buffer + vx + vy*sheet->xsize;
	for(short cnty=0; cnty<ysize; cnty++) {
		for(short cntx=0;cntx<xsize;cntx++)
			buf[cntx]=color;
		buf+=sheet->xsize;
	}
	return ;
}

void g_boxf(struct WINDOW *window, short vx, short vy, short xsize, short ysize, int color) {

	//指定箇所がシート外の場合は補正
	if ((vx+xsize-1)>=window->xsize)
		xsize=window->xsize-vx ;
	if ((vy+ysize-1)>=window->ysize)
		ysize=window->ysize-vy ;
	if (vx<0) {
		xsize+=vx;
		vx=0;
	}
	if (vy<0) {
		ysize+=vy;
		vy=0;
	}

	if (xsize<=0||ysize<=0)
		return ;	//値が不正だった時以外にも、範囲すべてがシート外に存在する場合はここに来る事もアリ

	int *buf=window->this_s_p->buffer + vx+4 + (vy+20)*window->this_s_p->xsize;
	for(short cnty=0; cnty<ysize; cnty++) {
		for(short cntx=0;cntx<xsize;cntx++)
			buf[cntx]=color;
		buf+=window->this_s_p->xsize;
	}
	return;
}


/*
 *generate_middle
 (y-x)|(z-y)>=0 ならば、　x<=y<=z またはz<=y<=x　よって　中央値y
 (z-x)|(y-z)>=0 ならば　z
 (x-z)|(y-x)>=0 ならば　x

 */


//文字列描画メインルーチン(g_mes関数からの呼び出しを受ける)
void g_mes1(struct SHEET *sheet, int x, int y, char *font) {
	int *p;
	char d ;
	for (int i = 0; i < 16; i++) {
		if (y+i<0||sheet->ysize<=y+i)
			continue;
		p = (int *)(sheet->buffer + (y + i) * sheet->xsize + x);
		d = font[i];
		for (UCHAR q=0;q<8;q++) {
			if (x+q<0||sheet->xsize<=x+q)
				continue;
			if ((d & (1<<(7-q))) != 0) {
				p[q] = 0;
			}
		}
	}
	return;
}

extern char hankaku[4096];

//文字列描画仲介ルーチン（g_mes1で実際に描画）
void g_mes(struct SHEET *sheet, int x, int y, char *s) {
	int _x=x, _y=y;
	for (; *s != NULL; s++) {
		// if (strncmp(s,"\n",1)) {
			// _y+=16;
			// _x=x;
		// }else{
			g_mes1(sheet, _x, _y, hankaku + *s * 16);
			_x += 8;
		// }
	}
	return;
}

//文字列描画仲介ルーチン（g_mes1で実際に描画）
void g_mes(struct WINDOW *window, int x, int y, char *s) {
	x+=4;
	y+=20;
	int _x=x, _y=y;

	for (; *s != NULL; s++) {
		// if (!strncmp(s,"\n",1)) {
			// _y+=16;
			// _x=x;
		// }else{
			g_mes1(window->this_s_p, _x, _y, hankaku + *s * 16);
			_x += 8;
		// }
	}
	return;
}

struct SHEET *mouse, *taskbar;

/* マウスカーソル作成 */
void create_mouse(short x, short y){
	mouse=shtctl.create(x,y,10,20);
	g_boxf(mouse,0,0,10,20,0x000000) ;
	shtctl.all_refresh(mouse) ;
	return ;
}

void set_qemuvga0reg(int reg, int dat)
{
    io_out16(0x01ce, reg);
    io_out16(0x01cf, dat);
    return;
}

void init_qemuvga0(int x, int y, int c, int flag)
{
    set_qemuvga0reg(0x0004, 0x0000);
    set_qemuvga0reg(0x0001, x);
    set_qemuvga0reg(0x0002, y);
    set_qemuvga0reg(0x0003, c); /* 4, 8, 15, 16, 24, 32 */
    set_qemuvga0reg(0x0005, 0x0000);
    set_qemuvga0reg(0x0004, flag); /* リニアアクセスモードでVRAMの初期化をするなら0x41 */
	/* bit7 : VRAM初期化抑制, bit6 : リニアアクセスモード, bit0 : 設定有効 */
    sinfo->scrnx = x;
    sinfo->scrny = y;
    sinfo->vbitc = c;
    if ((flag & 0x40) == 0) {
        sinfo->vram = (int *) 0x000a0000;
    } else {
        sinfo->vram = (int *) 0xe0000000;
    }
    return;
}

void boot_GUI(void) {
	init_qemuvga0(1024, 768, 32, 0xc1);

	/* シートシステム初期化 */
	SHEET *back=shtctl.init(0, 0, sinfo->scrnx, sinfo->scrny);							// 壁紙用のシートを作成

	/* 壁紙、マウスのシートを作成 */
	g_boxf(back, 0, 0, back->xsize, back->ysize,0xFFFFFC) ;

	create_mouse((sinfo->scrnx - 10) >>1,(sinfo->scrny - 20) >>1) ;		//画面中央になるように座標計算して、マウスのシートを作成

	/* タスクバー */
	taskbar=shtctl.create(0,0,sinfo->scrnx,20);
	Aqua aqua;
	aqua.glassbox(taskbar,0,0,taskbar->xsize,taskbar->ysize, 0xE0E0E0) ;
	shtctl.all_refresh(taskbar) ;

	/* シート実描画 */
	shtctl.all_refresh(back) ;
	return ;
}
