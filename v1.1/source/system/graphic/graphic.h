/*
 *  hsp.h
 *
 *
 *  Created by Liva on 10/12/17.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>

class HSP {

	int _color ;
	short _current_posx, _current_posy ;
	short _ginfo_mesx, _ginfo_mesy;
	short _fontx, _fonty;
	WINDOW *_window ;
	void swapvertex( short &x1, short &y1, short &x2, short &y2 );
public :
	int stat ;
	HSP(WINDOW *window) ;
	double atan(double y, double x=1.0);
	void boxf(short x1=0, short y1=0, short x2=-32768, short y2=-32768);
	void circle( short Sx, short Sy, short Ex, short Ey,  bool flag=1);
	void color(char R, char G, char B);
	short ginfo_cx(void);
	short ginfo_cy(void);
	short ginfo_mesx(void);
	short ginfo_mesy(void);
	short ginfo_winx(void);
	short ginfo_winy(void);
	void line( short x1, short y1, short x2=-32768, short y2=-32768 ) ;
	void mes(char *msg);
	short mousex(void) ;
	short mousey(void) ;
	void pos(short current_posx, short current_posy) ;
	void pset(short posx=-32768, short posy=-32768) ;
	int pget(short posx, short posy) ;
};


void g_boxf(struct SHEET *sheet, short vx, short vy, short xsize, short ysize, int color) ;		// 矩形塗りつぶし(シート用)
void g_boxf(struct WINDOW *window, short vx, short vy, short xsize, short ysize, int color) ;	// 矩形塗りつぶし(ウィンドウ用)
void g_mes(struct WINDOW *window, int x, int y, char *s) ;											// 文字列の描画(シート描画)
void g_mes(struct SHEET *sheet, int x, int y, char *s) ;											// 文字列の描画(ウィンドウ描画)


inline double HSP::atan(double y, double x){
	if (x==0.0)
		return 0.5*M_PI;
	
	return atan2(y,x);
}

inline void HSP::swapvertex( short &x1, short &y1, short &x2, short &y2 ){
	short tmp;
	if(x1>x2){
		tmp=x2;
		x2=x1;
		x1=tmp;
	}
	if(y1>y2){
		tmp=y2;
		y2=y1;
		y1=tmp;
	}
	return;
}

//矩形を塗りつぶす
//x1=0～(0)  : 矩形の左上X座標
//y1=0～(0)  : 矩形の左上Y座標
//x2=0～     : 矩形の右下X座標
//y2=0～     : 矩形の右下Y座標
inline void HSP::boxf(short x1, short y1, short x2, short y2){
	if (x2==-32768)
		x2=_window->xsize - 1;
	if (y2==-32768)
		y2=_window->ysize - 1;
	swapvertex( x1, y1, x2, y2 );
	x2++;
	y2++;
	g_boxf(_window, x1, y1, x2-x1, y2-y1, _color);
	return ;
}

inline void HSP::color(char R, char G, char B){
	char tmp[3]={B,G,R};
	int *tmp2=(int *)tmp;
	_color=*tmp2;
	return ;
}

inline short HSP::ginfo_cx(void) {
	return _current_posx ;
}

inline short HSP::ginfo_cy(void) {
	return _current_posy ;
}

inline short HSP::ginfo_mesx(void){
	return _ginfo_mesx;
}

inline short HSP::ginfo_mesy(void){
	return _ginfo_mesy;
}

inline short HSP::ginfo_winx(void) {
	return _window->xsize ;
}

inline short HSP::ginfo_winy(void) {
	return _window->ysize ;
}

inline void HSP::mes(char *msg){
	g_mes( _window, _current_posx, _current_posy, msg );
	_current_posy+=_fonty;
	_ginfo_mesx=_fontx*strlen(msg);
	_ginfo_mesy=_fonty;
	return;
}

inline void HSP::pos(short current_posx, short current_posy) {
	_current_posx=current_posx ;
	_current_posy=current_posy ;
	return ;
}

inline void HSP::pset(short posx, short posy) {
	if (posx==-32768)
		posx=_current_posx;
	if (posy==-32768)
		posy=_current_posy;
	
	if (posx<0||_window->xsize<=posx||posy<0||_window->ysize<=posy)
		return ;
	int *buf=_window->this_s_p->buffer + posx+4 + (posy+20)*_window->this_s_p->xsize;
	*buf=_color ;
	return ;
}

inline int HSP::pget(short posx, short posy) {
	if (posx<0||_window->xsize<=posx||posy<0||_window->ysize<=posy)
		return 0xFFFFFF;
	int color=_window->this_s_p->buffer[posx+4 + (posy+20)*_window->this_s_p->xsize];
	color&=0xFFFFFF;
	return color;
}
