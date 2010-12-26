//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* �E�B���h�E�֌W */

#include "kernel.h"

#define	TITLEBAR_H		18


WINDOWCTL		windowctl;

bool WINDOWCTL::_firstFlag=1;

/* �E�B���h�E�쐬�֐� */
//�ɗ�xsize�Avx�Avy��4�̔{���ɂ���̂��D�܂����iSSE2�����������R�s�[���g�����߁j
WINDOW *WINDOWCTL::create(short vx, short vy, short xsize, short ysize, char *w_title) {

	if (!_firstFlag) {
		_firstFlag=0;
		window_num=0;
	}


	WINDOW *window=shtctl.wincreate(vx,vy,xsize+8,ysize+24) ;			// �E�B���h�E���V�[�g�̈��Ȃ̂ŁA�V�[�g���쐬
	window->vx=vx;
	window->vy=vy;
	window->xsize=xsize;
	window->ysize=ysize;

	//�����ɔ���ĕ`�悷�邪�ASSE2���߂ō����ɕ`�悷�邽�߁A���ꂪ��Ԍ������悢

	boxf(window->this_s_p,0,20,4,ysize,0xDCDCDC) ;
	boxf(window->this_s_p,0,ysize+22,1,1,0xDCDCDC) ;
	boxf(window->this_s_p,xsize+4,20,4,ysize,0xDCDCDC) ;
	boxf(window->this_s_p,0,ysize+20,xsize+8,2,0xDCDCDC) ;
	boxf(window->this_s_p,1,20,1,ysize+3,0xFFFFFF) ;

	boxf(window->this_s_p,1,ysize+22,xsize+6,1,0xA0A0A0) ;
	boxf(window->this_s_p,xsize+6,20,1,ysize+2,0xA0A0A0) ;
	boxf(window->this_s_p,0,0,xsize+8,1,0xAAAAAA) ;
	boxf(window->this_s_p,0,19,xsize+8,1,0xAAAAAA) ;

	boxf(window->this_s_p,0,ysize+23,xsize+8,1,0x606060);
	boxf(window->this_s_p,xsize+7,20,1,ysize+3,0x606060);

	boxf(window,0,0,xsize,ysize,0xFFFFFF) ;

	window->title=w_title;

	_using.Lock();
	_create(window);
	_using.Free();

	return window ;
}

void WINDOWCTL::_create(WINDOW *window) {
	if (window_num==0) {
		bottom=window ;
	}else{
		non_active_titlebar();
	}
	window_num++;
	window->next_win=active_win ;
	active_win=window ;
	active_titlebar();
	
	shtctl.set_top(mouse) ;
	shtctl.all_refresh(window->this_s_p);	
	return ;
}

void WINDOWCTL::del(WINDOW *window) {

	_using.Lock();
	_del(window) ;
	_using.Free();
	shtctl.del(window->this_s_p);
	return;
}

void WINDOWCTL::_del(WINDOW *window) {
	window_num--;
	if (window_num==0)
		_firstFlag=1;

	if (window==active_win) {
		active_win=window->next_win ;
	}else{
		WINDOW *c_win=active_win ;
		for(;;) {
			if (c_win->next_win==window)
				break ;

			c_win=c_win->next_win ;
			if (c_win==bottom){
				window_num++;
				return ;				//�����ȃE�B���h�E�w��
			}
		}

		c_win->next_win=window->next_win ;
		if (window==bottom)
			bottom=c_win ;
	}
	return ;
}

void WINDOWCTL::active(struct WINDOW *window) {
	if (window==active_win)
		return ;

	_using.Lock();
	_active(window) ;
	_using.Free();
	return;
}

void WINDOWCTL::_active(WINDOW *window) {

	WINDOW *c_win=active_win ;
	for(;;) {
		if (c_win->next_win==window)
			break ;

		c_win=c_win->next_win ;
		if (c_win==bottom){
			return ;				//�����ȃE�B���h�E�w��
		}
	}

	c_win->next_win=window->next_win ;
	if (window==bottom)
		bottom=c_win ;
	window->next_win=active_win ;
	non_active_titlebar() ;
	SHEET *old_active=active_win->this_s_p ;
	active_win=window ;
	active_titlebar() ;
	shtctl.set_wintop(window->this_s_p, old_active) ;

	return ;
}

// *window���A�N�e�B�u���ǂ������`�F�b�N
// �߂�l
// 1:�A�N�e�B�u
// 0:�A�N�e�B�u�łȂ�
bool WINDOWCTL::check_active(struct WINDOW *window) {
	if (window==active_win)
		return 1;
	return 0;
}


// �E�B���h�E���΍��W�Ŏw��
void WINDOWCTL::refresh(struct WINDOW *window, short vx, short vy, short xsize, short ysize) const {

	// �E�B���h�E����͂ݏo���Ă�����A�␳
	if ((vx+xsize-1) >= window->xsize)
		xsize=window->xsize-vx ;
	if ((vy+ysize-1) >= window->ysize)
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
		return ;	// �l���s�����������ȊO�ɂ��A�͈͂��ׂĂ��E�B���h�E�O�ɑ��݂���ꍇ�͂����ɗ��鎖���A��

	shtctl.refresh(window->this_s_p,window->vx+vx+4,window->vy+vy+20,xsize,ysize);
	
	return;
}

void WINDOWCTL::all_refresh(struct WINDOW *window) const {
	shtctl.refresh(window->this_s_p,window->vx+4,window->vy+20,window->xsize,window->ysize);
	return ;
}

void WINDOWCTL::active_titlebar(void) const {

	Aqua aqua ;
	aqua.glassbox(active_win->this_s_p, 0, 0, active_win->xsize+8, 20, 0xCDC7FF) ;
	mes(active_win->this_s_p,27,2,active_win->title);

	// �A�C�R���̕`��
	for(char cnt=0;cnt<16;cnt++)
		ssememcpy(active_win->this_s_p->buffer+(active_win->xsize+8)*(cnt+2)+3,&active_win->icon[cnt*16],16*4) ;

	return ;
}

void WINDOWCTL::non_active_titlebar(void) const {

	// �^�C�g���o�[
	Aqua aqua ;
	aqua.stripebox(active_win->this_s_p, 0, 0, active_win->xsize+8, 20, 0xE0E0E0) ;
	mes(active_win->this_s_p,27,2,active_win->title);

	// �A�C�R���̕`��
	for(char cnt=0;cnt<16;cnt++)
		ssememcpy(active_win->this_s_p->buffer+(active_win->xsize+8)*(cnt+2)+3,&active_win->icon[cnt*16],16*4) ;

	shtctl.refresh(active_win->this_s_p, active_win->vx, active_win->vy, active_win->xsize+8, 20);
	return ;
}

void WINDOWCTL::win_change(void)  {
	active(active_win->next_win) ;
	return ;
}
