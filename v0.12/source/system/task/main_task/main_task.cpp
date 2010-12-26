//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/*�@���C���^�X�N */
#include "kernel.h"
#include "main_task.h"
#include <stdio.h>
#include <string.h>

unsigned int x=0;

WINDOW *window ;
void pset(short x,short y,int color){
	sinfo->vram[1024*y+x]=color;
}

//�V�X�e�����j�^�[�Ǘ��A�}�E�X�J�[�\���̈ړ��Ȃ�
void main_task(void) {

	char s[20];
	taskctl.set_input(MOUSE_ID_S, new Caller(mouse_signal)) ;
	enable_mouse();					// �}�E�X�L����
	KEYBOARD keyboard;
	taskctl.set_input(KEYBOARD_ID, new Caller(keyboard, &KEYBOARD::FuncSignalProcess)) ;

	//���荞�݂�����
	allow_interrupt();

	/* �R���\�[���n�� */
	CONSOLE Console(50,10,390,200,48,128) ;

//	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);

	/* �V�X�e�����j�^�[�n�� */
	window=windowctl.create(700,450,200,200,"System Monitor") ;
//	window=windowctl.create(300,50,200,200,"System Monitor") ;
	// �w�i�S��
	Aqua aqua;
	aqua.stripebox(window, 0, 0, 200, 200, 0xF3F3F3);

	sprintf(s,"total memory:%dKB",mem->FuncGetTotal()>>10) ; //MB�P�ʂɂ�������΁A>>10��>>20�ɂ���Ƃ悢
	mes(window,6,25,s);

	sprintf(s, "cursor:(%3d, %3d)", mouse->vx, mouse->vy);
	mes(window,6,65,s);

	windowctl.all_refresh(window) ;

	for(;;) {
		aqua.stripebox(window,6,45,194,15,0xF3F3F3) ;
		sprintf(s,"      free_s  :%dKB",mem->FuncGetUnusing()>>10) ;
		mes(window,6,45,s);
		windowctl.refresh(window, 6, 45, 194, 15) ;

		//���͐M������
/*		unsigned int data=taskctl.get_signal();
		if (data){
			if ((data>>8)&MOUSE_ID)
				mouse_signal((unsigned char)(data&0xff));
			if ((data>>8)&KEYBOARD_ID){
				key=0;
				keyboard_signal((unsigned char)(data&0xff));
				if (key!=0)
					Console.lootin(key) ;
			}
		}
*/
		taskctl.waitIO(MAX_ID);					//���i�v�I�ɕ��A���鎖�͂Ȃ��B����΁A���S�ȃX���[�v���
	}
}

void mouse_signal(UCHAR dat) {

	short mousevx_old, mousevy_old ;
	char s[40] ;

	mdec.dat = dat;
	if (mouse_decode()) {
		/* �f�[�^��3�o�C�g�������̂ŕ\�� */
		sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
		//3�o�C�g�f�[�^�̃f�R�[�h
		if (mdec.btn & 0x01)
			s[1] = 'L';
		if (mdec.btn & 0x02)
			s[3] = 'R';
		if (mdec.btn & 0x04)
			s[2] = 'C';

		/* �}�E�X�J�[�\���̈ړ� */
		mousevx_old=mouse->vx ;
		mousevy_old=mouse->vy ;
		mouse->vx += mdec.x;
		mouse->vy += mdec.y;

		// ��ʊO�␳
		if (mouse->vx < 0)
			mouse->vx = 0;
		if (mouse->vy < 0)
			mouse->vy = 0;
		if (mouse->vx >= sinfo->scrnx)
			mouse->vx = sinfo->scrnx-1 ;
		if (mouse->vy >= sinfo->scrny)
			mouse->vy = sinfo->scrny-1 ;

		/* �}�E�X���\�� */
		Aqua aqua ;
		aqua.stripebox(window, 6, 85, 194, 55, 0xF3F3F3);
		mes(window,6,85,"mouse input:");
		mes(window,6,105,s);
		sprintf(s, "mouse pos: (%3d, %3d)", mouse->vx, mouse->vy);
		mes(window,6,125,s);

		/* ����ʕ`�� */
		windowctl.refresh(window, 6, 85, 194, 55) ;				// ������`��
		shtctl.all_refresh(mouse) ;													// �}�E�X�`��
		shtctl.refresh(mouse, mousevx_old, mousevy_old, mouse->xsize, mouse->ysize) ;	// �}�E�X����
	}
	return ;
}

char phase_zero(void) {
	/* �}�E�X��0xfa��҂��Ă���i�K */
	if (mdec.dat == 0xfa)
		mdec.phase = 1;
	return 0;
}

char phase_one(void) {
	/* �}�E�X��1�o�C�g�ڂ�҂��Ă���i�K */
	if ((mdec.dat & 0xc8) == 0x08) {
		/* ������1�o�C�g�ڂ����� */
		mdec.buf[0] = mdec.dat;
		mdec.phase = 2;
	}
	return 0;
}

char phase_two(void) {
	/* �}�E�X��2�o�C�g�ڂ�҂��Ă���i�K */
	mdec.buf[1] = mdec.dat;
	mdec.phase = 3;
	return 0;
}

char phase_three(void) {
	/* �}�E�X��3�o�C�g�ڂ�҂��Ă���i�K */
	mdec.buf[2] = mdec.dat;
	mdec.phase = 1;
	mdec.btn = mdec.buf[0] ;
	mdec.x = mdec.buf[1];
	mdec.y = mdec.buf[2];
	if (mdec.buf[0] & 0x10)
		mdec.x |= 0xffffff00;
	if (mdec.buf[0] & 0x20)
		mdec.y |= 0xffffff00;
	mdec.y *= -1; /* �}�E�X�ł�y�����̕�������ʂƔ��� */
	return 1;
}


char mouse_decode(void) {
	char (*func[])(void) = { phase_zero, phase_one, phase_two, phase_three };
	if (mdec.phase<=3)
		return (*func[mdec.phase])();

	//�����ɗ��鎖�͂Ȃ��͂�
	for(;;)
		io_hlt();		//@��O�����̋L�q
}

KEYBOARD::KEYBOARD(void):_CmdFIFO(32) {

	_cmdWait = -1;
	_keyLEDs = (sinfo->leds >> 4) & 7 ;

	/* �ŏ��ɃL�[�{�[�h��ԂƂ̐H���Ⴂ���Ȃ��悤�ɁA�ݒ肵�Ă������Ƃɂ��� */
	_CmdFIFO.put(KEYCMD_LED);
	_CmdFIFO.put(_keyLEDs);
	for(UCHAR cnt=0;cnt<0x80;cnt++)
		_keyParam[cnt]=0;
	return ;
}

void KEYBOARD::FuncSignalProcess(const UCHAR dat){


	char s[40];
	/* ���̓R�[�h�\�� */
	sprintf(s, "keyboard input: %02X", dat);
	Aqua aqua ;
	aqua.stripebox(window, 6, 145, 194, 15, 0xF3F3F3);
	mes(window,6,145,s);
	windowctl.refresh(window, 6, 145, 194, 15) ;

	//����ȃL�[�R�[�h�̏���
	switch (dat) {

		case 0x3a :		// CapsLock
			_keyLEDs ^= 4;
			_CmdFIFO.put(KEYCMD_LED);
			_CmdFIFO.put(_keyLEDs);
			break ;

		case  0x45 :	// NumLock
			_keyLEDs ^= 2;
			_CmdFIFO.put(KEYCMD_LED);
			_CmdFIFO.put(_keyLEDs);
			break ;

		case 0x46 :	// ScrollLock
			_keyLEDs ^= 1;
			_CmdFIFO.put(KEYCMD_LED);
			_CmdFIFO.put(_keyLEDs);
			break ;

		case 0xfa :		// �L�[�{�[�h���f�[�^�𖳎��Ɏ󂯎����
			_cmdWait = -1;
			if (_CmdFIFO.status() > 0) {
				_cmdWait=_CmdFIFO.get();
				/* �L�[�{�[�h�R���g���[���ɑ���f�[�^������΁A���� */
				wait_KBC_sendready();
				io_out8(PORT_KEYDAT, _cmdWait);
			}
			return ;

		case 0xfe :	// �L�[�{�[�h���f�[�^�𖳎��Ɏ󂯎��Ȃ�����
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, _cmdWait);
			return ;
	}

	if (dat<0x80) {
		_keyParam[dat]=1 ;
		_FuncGenerateKeyCode(dat) ;
	}else{
		_keyParam[dat-0x80]=0 ;
	}

	/* �E�B���h�E�؂�ւ� */
	static char win_c_flag=0 ;
//	if (_keyParam[Key_Tab]&_keyParam[Key_Alt])
	if (_keyParam[Key_Tab]&_keyParam[Key_Ctrl]) {
		win_c_flag=1 ;
	}
//	if (!_keyParam[Key_Tab]&_keyParam[Key_Alt]&win_c_flag) {
	if (!_keyParam[Key_Tab]&_keyParam[Key_Ctrl]&win_c_flag) {
		win_c_flag=0 ;

/*		SHEET *w_change=shtctl.create(0,(sinfo->scrny>>1)-100,sinfo->scrnx,200) ;

		// �w�i�S��
		aqua.stripebox(window1, 0, 0, 200, 200, 0xF3F3F3);
*/
		windowctl.win_change() ;
	}

	return ;
}

void KEYBOARD::_FuncGenerateKeyCode(UCHAR dat) {

	_key=0;

	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};

	//�L�[�R�[�h�̏���
	switch (dat) {
		case Key_BackSpace :	// �o�b�N�X�y�[�X
			_key=8;
			break ;

		case Key_Enter :		// Enter
			_key=10;
			break ;
		
		case Key_Shift_L :
			break ;
		
		case Key_Shift_R :
			break ;

		default:			// �L�[�R�[�h�𕶎��R�[�h�ɕϊ�
			if ( _keyParam[Key_Shift_L] || _keyParam[Key_Shift_R] ) {
				_key = keytable1[dat];
				// ���͕������A���t�@�x�b�g
				if ('A' <= _key && _key <= 'Z')
					if (_keyLEDs & 4)
						_key += 0x20;	// �啶�����������ɕϊ�
			} else {
				_key = keytable0[dat];
				// ���͕������A���t�@�x�b�g
				if ('A' <= _key && _key <= 'Z')
					if (!(_keyLEDs & 4))
						_key += 0x20;	// �啶�����������ɕϊ�
			}
			break ;
	}

	if (_key!=0)
		inputctl->i_put(_key + (KEY_ID<<8));		// �M����inputctl�e�h�e�n��

	return ;
}


