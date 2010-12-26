//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/*　メインタスク */
#include "kernel.h"
#include "main_task.h"
#include <stdio.h>
#include <string.h>

unsigned int x=0;

WINDOW *window ;
void pset(short x,short y,int color){
	sinfo->vram[1024*y+x]=color;
}

//システムモニター管理、マウスカーソルの移動など
void main_task(void) {

	char s[20];
	taskctl.set_input(MOUSE_ID_S, new Caller(mouse_signal)) ;
	enable_mouse();					// マウス有効化
	KEYBOARD keyboard;
	taskctl.set_input(KEYBOARD_ID, new Caller(keyboard, &KEYBOARD::FuncSignalProcess)) ;

	//割り込みを許可
	allow_interrupt();

	/* コンソール始動 */
	CONSOLE Console(50,10,390,200,48,128) ;

//	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);

	/* システムモニター始動 */
	window=windowctl.create(700,450,200,200,"System Monitor") ;
//	window=windowctl.create(300,50,200,200,"System Monitor") ;
	// 背景全体
	Aqua aqua;
	aqua.stripebox(window, 0, 0, 200, 200, 0xF3F3F3);

	sprintf(s,"total memory:%dKB",mem->FuncGetTotal()>>10) ; //MB単位にしたければ、>>10を>>20にするとよい
	mes(window,6,25,s);

	sprintf(s, "cursor:(%3d, %3d)", mouse->vx, mouse->vy);
	mes(window,6,65,s);

	windowctl.all_refresh(window) ;

	for(;;) {
		aqua.stripebox(window,6,45,194,15,0xF3F3F3) ;
		sprintf(s,"      free_s  :%dKB",mem->FuncGetUnusing()>>10) ;
		mes(window,6,45,s);
		windowctl.refresh(window, 6, 45, 194, 15) ;

		//入力信号処理
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
		taskctl.waitIO(MAX_ID);					//半永久的に復帰する事はない。いわば、完全なスリープ状態
	}
}

void mouse_signal(UCHAR dat) {

	short mousevx_old, mousevy_old ;
	char s[40] ;

	mdec.dat = dat;
	if (mouse_decode()) {
		/* データが3バイト揃ったので表示 */
		sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
		//3バイトデータのデコード
		if (mdec.btn & 0x01)
			s[1] = 'L';
		if (mdec.btn & 0x02)
			s[3] = 'R';
		if (mdec.btn & 0x04)
			s[2] = 'C';

		/* マウスカーソルの移動 */
		mousevx_old=mouse->vx ;
		mousevy_old=mouse->vy ;
		mouse->vx += mdec.x;
		mouse->vy += mdec.y;

		// 画面外補正
		if (mouse->vx < 0)
			mouse->vx = 0;
		if (mouse->vy < 0)
			mouse->vy = 0;
		if (mouse->vx >= sinfo->scrnx)
			mouse->vx = sinfo->scrnx-1 ;
		if (mouse->vy >= sinfo->scrny)
			mouse->vy = sinfo->scrny-1 ;

		/* マウス情報表示 */
		Aqua aqua ;
		aqua.stripebox(window, 6, 85, 194, 55, 0xF3F3F3);
		mes(window,6,85,"mouse input:");
		mes(window,6,105,s);
		sprintf(s, "mouse pos: (%3d, %3d)", mouse->vx, mouse->vy);
		mes(window,6,125,s);

		/* 実画面描画 */
		windowctl.refresh(window, 6, 85, 194, 55) ;				// 文字列描画
		shtctl.all_refresh(mouse) ;													// マウス描画
		shtctl.refresh(mouse, mousevx_old, mousevy_old, mouse->xsize, mouse->ysize) ;	// マウス消す
	}
	return ;
}

char phase_zero(void) {
	/* マウスの0xfaを待っている段階 */
	if (mdec.dat == 0xfa)
		mdec.phase = 1;
	return 0;
}

char phase_one(void) {
	/* マウスの1バイト目を待っている段階 */
	if ((mdec.dat & 0xc8) == 0x08) {
		/* 正しい1バイト目だった */
		mdec.buf[0] = mdec.dat;
		mdec.phase = 2;
	}
	return 0;
}

char phase_two(void) {
	/* マウスの2バイト目を待っている段階 */
	mdec.buf[1] = mdec.dat;
	mdec.phase = 3;
	return 0;
}

char phase_three(void) {
	/* マウスの3バイト目を待っている段階 */
	mdec.buf[2] = mdec.dat;
	mdec.phase = 1;
	mdec.btn = mdec.buf[0] ;
	mdec.x = mdec.buf[1];
	mdec.y = mdec.buf[2];
	if (mdec.buf[0] & 0x10)
		mdec.x |= 0xffffff00;
	if (mdec.buf[0] & 0x20)
		mdec.y |= 0xffffff00;
	mdec.y *= -1; /* マウスではy方向の符号が画面と反対 */
	return 1;
}


char mouse_decode(void) {
	char (*func[])(void) = { phase_zero, phase_one, phase_two, phase_three };
	if (mdec.phase<=3)
		return (*func[mdec.phase])();

	//ここに来る事はないはず
	for(;;)
		io_hlt();		//@例外処理の記述
}

KEYBOARD::KEYBOARD(void):_CmdFIFO(32) {

	_cmdWait = -1;
	_keyLEDs = (sinfo->leds >> 4) & 7 ;

	/* 最初にキーボード状態との食い違いがないように、設定しておくことにする */
	_CmdFIFO.put(KEYCMD_LED);
	_CmdFIFO.put(_keyLEDs);
	for(UCHAR cnt=0;cnt<0x80;cnt++)
		_keyParam[cnt]=0;
	return ;
}

void KEYBOARD::FuncSignalProcess(const UCHAR dat){


	char s[40];
	/* 入力コード表示 */
	sprintf(s, "keyboard input: %02X", dat);
	Aqua aqua ;
	aqua.stripebox(window, 6, 145, 194, 15, 0xF3F3F3);
	mes(window,6,145,s);
	windowctl.refresh(window, 6, 145, 194, 15) ;

	//特殊なキーコードの処理
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

		case 0xfa :		// キーボードがデータを無事に受け取った
			_cmdWait = -1;
			if (_CmdFIFO.status() > 0) {
				_cmdWait=_CmdFIFO.get();
				/* キーボードコントローラに送るデータがあれば、送る */
				wait_KBC_sendready();
				io_out8(PORT_KEYDAT, _cmdWait);
			}
			return ;

		case 0xfe :	// キーボードがデータを無事に受け取れなかった
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

	/* ウィンドウ切り替え */
	static char win_c_flag=0 ;
//	if (_keyParam[Key_Tab]&_keyParam[Key_Alt])
	if (_keyParam[Key_Tab]&_keyParam[Key_Ctrl]) {
		win_c_flag=1 ;
	}
//	if (!_keyParam[Key_Tab]&_keyParam[Key_Alt]&win_c_flag) {
	if (!_keyParam[Key_Tab]&_keyParam[Key_Ctrl]&win_c_flag) {
		win_c_flag=0 ;

/*		SHEET *w_change=shtctl.create(0,(sinfo->scrny>>1)-100,sinfo->scrnx,200) ;

		// 背景全体
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

	//キーコードの処理
	switch (dat) {
		case Key_BackSpace :	// バックスペース
			_key=8;
			break ;

		case Key_Enter :		// Enter
			_key=10;
			break ;
		
		case Key_Shift_L :
			break ;
		
		case Key_Shift_R :
			break ;

		default:			// キーコードを文字コードに変換
			if ( _keyParam[Key_Shift_L] || _keyParam[Key_Shift_R] ) {
				_key = keytable1[dat];
				// 入力文字がアルファベット
				if ('A' <= _key && _key <= 'Z')
					if (_keyLEDs & 4)
						_key += 0x20;	// 大文字を小文字に変換
			} else {
				_key = keytable0[dat];
				// 入力文字がアルファベット
				if ('A' <= _key && _key <= 'Z')
					if (!(_keyLEDs & 4))
						_key += 0x20;	// 大文字を小文字に変換
			}
			break ;
	}

	if (_key!=0)
		inputctl->i_put(_key + (KEY_ID<<8));		// 信号をinputctlＦＩＦＯへ

	return ;
}


