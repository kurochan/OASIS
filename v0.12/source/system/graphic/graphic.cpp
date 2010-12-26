//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* 画面描画関係 */
#include "kernel.h"
#include <stdlib.h>


HSP::HSP(WINDOW *window) {
	_color=0;
	_window=window ;
}

short HSP::mousex(void) {
	return mouse->vx ;
}

short HSP::mousey(void) {
	return mouse->vy ;
}


short HSP::ginfo_winx(void) {
	return _window->xsize ;
}

short HSP::ginfo_winy(void) {
	return _window->ysize ;
}

short HSP::ginfo_cx(void) {
	return _current_posx ;
}

short HSP::ginfo_cy(void) {
	return _current_posy ;
}

void HSP::pos(short current_posx, short current_posy) {
	_current_posx=current_posx ;
	_current_posy=current_posy ;
	return ;
}

void HSP::pset(short posx, short posy) {
	if (posx<0||_window->xsize<=posx||posy<0||_window->ysize<=posy)
		return ;
	int *buf=_window->this_s_p->buffer + posx+4 + (posy+20)*_window->this_s_p->xsize;
	*buf=_color ;
	return ;
}

int HSP::pget(short posx, short posy) {
	if (posx<0||_window->xsize<=posx||posy<0||_window->ysize<=posy)
		return 0xFFFFFF;
	int color=_window->this_s_p->buffer[posx+4 + (posy+20)*_window->this_s_p->xsize];
	color&=0xFFFFFF;
	return color;
}


void HSP::line( short x1, short y1, short x2, short y2) {

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

//LONG radius, POINT center
void HSP::circle(short x1, short y1, short x2, short y2){
	short dx=abs(x2-x1), dy=abs(y2-y1);
    short cx=0, cy=dx/2, cy2, d=2-dx;
	short center_x=(x1+x2)/2, center_y=(y1+y2)/2;

	cy2=(cy*dy)/dx;
	// 開始点の描画
	pset( x1,center_y );  
	pset( x2,center_y ); 
	pset( center_x,y1 ); 
	pset( center_x,y2 ); 


    while (1) {
		if (d > -cy) {
			--cy;
			d += 1 - 2 * cy;
		}
		
		if (d <= cx) {
			++cx;
			d += 1 + 2 * cx;
		}
		
		if (!cy) return;    // 描画終了
		cy2=(cy*dy)/dx;

        // 描画
		pset( cx + center_x, cy2 + center_y );  	// 0～90度の間
		pset( -cx + center_x, cy2 + center_y ); 	// 90～180度の間
		pset( -cx + center_x, -cy2 + center_y );	// 180～270度の間
		pset( cx + center_x, -cy2 + center_y );		// 270～360度の間

    }
/*	short radius_b=radius, radius_a=radius;
	int radius_a2=radius_a*radius_a;
	short cy=radius_b, old_cy=radius_b;
	for(short cx=0;cx!=radius;cx++){
		cy=(sqrt(radius_a2-(cx+1)*(cx+1)));
		if (old_cy ==cy)
			old_cy++;

		boxf( _window, cx + center_x, cy + center_y, 1, old_cy - cy , _color);
		boxf( _window, -cx-1 + center_x, cy + center_y, 1, old_cy - cy , _color);
		boxf( _window, -cx-1 + center_x, -cy + center_y - old_cy + cy, 1, old_cy - cy , _color);
		boxf( _window, cx + center_x, -cy + center_y - old_cy + cy, 1, old_cy - cy , _color);
//		pset( cx + center_x, cy + center_y );      // 45-90    度の間
//		pset( -cx + center_x, cy + center_y );     // 90-135   度の間
//		pset( -cx + center_x, -cy + center_y );    // 225-270  度の間
//		pset( cx + center_x, -cy + center_y );     // 270-315  度の間
		old_cy=cy;
	}
*/
	return ;
}


void HSP::color(int color){
	_color=color;
	return ;
}

void HSP::_boxf(void){
	boxf(_window, 0,0, _window->xsize, _window->ysize, _color);
	return ;
}

/* 矩形塗りつぶし */
void boxf(struct SHEET *sheet, short vx, short vy, short xsize, short ysize, int color) {

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

void boxf(struct WINDOW *window, short vx, short vy, short xsize, short ysize, int color) {

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


//文字列描画メインルーチン(mes関数からの呼び出しを受ける)
void mes1(struct SHEET *sheet, int x, int y, char *font) {
	int i,*p;
	char d, q ;
	for (i = 0; i < 16; i++) {
		p = (int *)(sheet->buffer + (y + i) * sheet->xsize + x);
		d = font[i];
		for (q=0;q<8;q++) {
			if ((d & (1<<(7-q))) != 0) {
				p[q] = 0;
			}
		}
	}
	return;
}

extern char hankaku[4096];

//文字列描画仲介ルーチン（mes1で実際に描画）
void mes(struct SHEET *sheet, int x, int y, char *s) {
	int _x=x, _y=y;
	for (; *s != NULL; s++) {
		// if (strncmp(s,"\n",1)) {
			// _y+=16;
			// _x=x;
		// }else{
			mes1(sheet, _x, _y, hankaku + *s * 16);
			_x += 8;
		// }
	}
	return;
}

//文字列描画仲介ルーチン（mes1で実際に描画）
void mes(struct WINDOW *window, int x, int y, char *s) {
	x+=4;
	y+=20;
	int _x=x, _y=y;

	for (; *s != NULL; s++) {
		// if (!strncmp(s,"\n",1)) {
			// _y+=16;
			// _x=x;
		// }else{
			mes1(window->this_s_p, _x, _y, hankaku + *s * 16);
			_x += 8;
		// }
	}
	return;
}

struct SHEET *mouse, *taskbar;

/* マウスカーソル作成 */
void create_mouse(short x, short y){
	mouse=shtctl.create(x,y,10,20);
	boxf(mouse,0,0,10,20,0x000000) ;
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
	boxf(back, 0, 0, back->xsize, back->ysize,0xFFFFFC) ;

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
