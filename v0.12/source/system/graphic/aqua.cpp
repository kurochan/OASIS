//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* アクアグラフィックス */
#include "kernel.h"

void Aqua::stripebox(struct WINDOW *window, short x, short y, short w, short h, int color) {
	stripebox(window->this_s_p,x+4,y+20,w,h,color);
	return;
}


void Aqua::stripebox(struct SHEET *sheet, short x, short y, short w, short h, int color) {

	if (h * w <= 0)
		return;

	UCHAR o_color[4];
	for(char tcnt=0;tcnt<3;tcnt++)
		o_color[tcnt]=(UCHAR)(color>>(tcnt<<3))&0xff ;
	o_color[3]=0xff;
	UCHAR c_color[3];
	int *tmp=(int *)&c_color[0];
	for(short cnt=0;cnt<h;cnt++) {
		int c = ((y+cnt)&2)*5 ;
		for(char tcnt=0;tcnt<3;tcnt++) {
			int a=(int)(o_color[tcnt] + c);
			if (a>255)
				a=255;
			if (a<0)
				a=0;
			c_color[tcnt]=(UCHAR)a;
		}
		boxf(sheet, x, y+cnt, w, 1, *tmp);
	}
	return;
}

void Aqua::glassbox(struct WINDOW *window, short x, short y, short w, short h, int color) {
	glassbox(window->this_s_p,x+4,y+20,w,h,color);
	return;
}

void Aqua::glassbox(struct SHEET *sheet, short x, short y, short w, short h, int color) {
	if (h * w <= 0)
		return;

	static int sr[] = {0,1,7,10,16,21,27,30,36,38,42,42,43,41,42,42,42,41,42,41,41,40,39,38,38,37,36,35,34,33,32,32,30,30,28,28,26,26,24,24,22,22,19,19,17,17,15,16,13,13,11,8,-1,-12,-21,-26,-28,-27,-27,-26,-26,-26,-26,-26,-26,-26,-25,-25,-24,-24,-23,-23,-22,-22,-21,-21,-20,-20,-19,-19,-17,-17,-16,-16,-15,-14,-12,-13,-11,-12,-10,-10,-8,-8,-6,-6,-5,-5,-3,-3,-2,-2,-1,0,1,2,2,3,3,4,5,6,6,7,7,8,8,10,9,11,11,12,12,13,13,14,13,15};


	UCHAR o_color[4];
	for(char tcnt=0;tcnt<3;tcnt++)
		o_color[tcnt]=(UCHAR)(color>>(tcnt<<3))&0xff ;
	o_color[3]=0xff;
	UCHAR c_color[3];
	int *tmp=(int *)&c_color[0];
	for(short cnt=0;cnt<h;cnt++) {
		char c = (cnt << 7) / h;
		for(char tcnt=0;tcnt<3;tcnt++) {
			short a=(short)(o_color[tcnt] + sr[c]);
			if (a>255)
				a=255;
			if (a<0)
				a=0;
			c_color[tcnt]=(UCHAR)a;
		}
		boxf(sheet, x, y+cnt, w, 1, *tmp);
	}
	boxf(sheet, x, y, 1, h,color);
	boxf(sheet, x+w-1, y, 1, h,color);
	boxf(sheet, x, y+h-1, w, 1,color);
	return;
}
