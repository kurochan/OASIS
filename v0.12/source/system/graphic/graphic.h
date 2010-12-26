/*
 *  hsp.h
 *  
 *
 *  Created by Liva on 10/12/17.
 *
 */

class HSP {

	int _color ;
	short _current_posx, _current_posy ;
	WINDOW *_window ;
public :
	int stat ;
	HSP(WINDOW *window) ;
	short ginfo_winx(void);
	short ginfo_winy(void);
	short mousex(void) ;
	short mousey(void) ;
	short ginfo_cx(void);
	short ginfo_cy(void);
	void pos(short current_posx, short current_posy) ;
	void pset(short posx, short posy) ;
	int pget(short posx, short posy) ;
	void line( short x1, short y1, short x2, short y2) ;
	void circle(short x1, short y1, short x2, short y2);
	void color(int color) ;
	void _boxf(void) ;
};

void boxf(struct SHEET *sheet, short vx, short vy, short xsize, short ysize, int color) ;		// 矩形塗りつぶし(シート用)
void boxf(struct WINDOW *window, short vx, short vy, short xsize, short ysize, int color) ;	// 矩形塗りつぶし(ウィンドウ用)
void mes(struct WINDOW *window, int x, int y, char *s) ;											// 文字列の描画(シート描画)
void mes(struct SHEET *sheet, int x, int y, char *s) ;											// 文字列の描画(ウィンドウ描画)
