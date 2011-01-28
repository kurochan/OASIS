//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* 割り込み関係 */

#include "kernel.h"


/* PICの初期化 */
void init_pic(void) {

	io_out8(PIC0_IMR,  0xff  );		// 全ての割り込みを受け付けない
	io_out8(PIC1_IMR,  0xff  );		// 全ての割り込みを受け付けない

	io_out8(PIC0_ICW1, 0x11  );	// エッジトリガモード
	io_out8(PIC0_ICW2, 0x20  );	// IRQ0-7は、INT20-27で受ける
	io_out8(PIC0_ICW3, 1 << 2);	// PIC1はIRQ2にて接続
	io_out8(PIC0_ICW4, 0x01  );	// ノンバッファモード

	io_out8(PIC1_ICW1, 0x11  );	// エッジトリガモード
	io_out8(PIC1_ICW2, 0x28  );	// IRQ8-15は、INT28-2fで受ける
	io_out8(PIC1_ICW3, 2     );		// PIC1はIRQ2にて接続
	io_out8(PIC1_ICW4, 0x01  );	// ノンバッファモード

	io_out8(PIC0_IMR,  0xfb  );		// 11111011 PIC1以外は全て禁止
	io_out8(PIC1_IMR,  0xff  );		// 11111111 全ての割り込みを受け付けない

	return;
}

/* 割り込みを許可 */
void allow_interrupt(void) {
	io_out8(PIC0_IMR, 0xf8);		// PITとPIC1とキーボードを許可(11111000)
	io_out8(PIC1_IMR, 0xef);		// マウスを許可(11101111)
	io_sti();								// 割り込みを許可

	return ;
}

/* FPU演算時のCPUからの割り込み */
void inthandler07(int *esp) {
	taskctl.inthandler07();
	return;
}

/* タイマーからの割り込み */
void inthandler20(int *esp) {
	io_out8(PIC0_OCW2, 0x60);	// IRQ-00受付完了をPICに通知
	timerctl.inthandler20(esp) ;		// 処理はtimerctlクラスへ
	return;
}

/* PS/2キーボードからの割り込み */
void inthandler21(int *esp) {
	io_out8(PIC0_OCW2, 0x61);	// IRQ-01受付完了をPICに通知
	inputctl->i_put(io_in8(PORT_KEYDAT) + (KEYBOARD_ID<<8));		// 信号をinputctlＦＩＦＯへ
	taskctl.Tswitch_IO();
	return ;
}

/* PS/2マウスからの割り込み */
void inthandler2c(int *esp) {
	io_out8(PIC1_OCW2, 0x64);	// IRQ-12受付完了をPIC1に通知
	io_out8(PIC0_OCW2, 0x62);	// IRQ-02受付完了をPIC0に通知

	inputctl->i_put(io_in8(PORT_KEYDAT) + (MOUSE_ID_S<<8));			// 信号をinputctlＦＩＦＯへ
	taskctl.Tswitch_IO();
	return;
}

/* PIC0からの不完全割り込み対策 */
void inthandler27(int *esp) {
	io_out8(PIC0_OCW2, 0x67);				// IRQ-07受付完了をPICに通知(7-1参照)
	return;
}
// Athlon64X2機などではチップセットの都合によりPICの初期化時にこの割り込みが1度だけおこる
// この割り込み処理関数は、その割り込みに対して何もしないでやり過ごす
// なぜ何もしなくていいの？
// →  この割り込みはPIC初期化時の電気的なノイズによって発生したものなので、
// まじめに何か処理してやる必要がない。

void inthandler40(int *esp) {
	taskctl.Tswitch_V();
	return;
}

void inthandler41(int *esp) {
	return;
}
