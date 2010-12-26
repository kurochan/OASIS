//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================

#include "../../unsigned.h"

void mouse_signal(const UCHAR dat);
char mouse_decode(void);
void create_mouse(short x, short y) ;


class KEYBOARD{
	int _cmdWait, _keyLEDs ;
	FIFO _CmdFIFO ;
	UCHAR _key ;
	bool _keyParam[0x80] ;
	void _FuncGenerateKeyCode(UCHAR dat) ;

public:
	KEYBOARD(void);
	void FuncSignalProcess(const UCHAR dat);
};

#define		Key_Tab			0x0F
#define		Key_BackSpace	0x0E
#define		Key_Enter 		0x1C
#define		Key_Ctrl			0x1D
#define		Key_Shift_L		0x2A
#define		Key_Shift_R		0x36
#define		Key_Alt				0x38
#define		Key_CapsLock	0x3A
#define		Key_NumLock	0x45
#define		Key_ScrollLock	0x46
#define		Rem_Key			0x80

#define		KEYCMD_LED	0xED

