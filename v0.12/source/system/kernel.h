//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================



#include "unsigned.h"
#include "funcCaller.h"
#include "resource.h"
#include "int.h"
#include "memory/memory.h"
#include "memory/new.h"
#include "graphic/sheet.h"
#include "graphic/window.h"
#include "graphic/graphic.h"
#include "graphic/aqua.h"
#include "graphic/d3module.h"
#include "timer/timer.h"
#include "fifo/fifo.h"
#include "inputdev/inputdev.h"
#include "task/task.h"
#include "task/tasks.h"
#include "task/main_task/console.h"
#include "disk/ide.h"

#include "asmfunc.h"



//== HSV2RGB.cpp ==//
void HSV2RGB( UCHAR &R, UCHAR &G, UCHAR &B, UCHAR H, UCHAR S, UCHAR V ) ;


//======================
//  構造体
//======================

//== main.cpp ==//

/* システム情報の格納 */
struct S_INFO {
	char cyls;						// ブートセクタはどこまでディスクを読んだのか
	char leds;						// ブート時のキーボードのLEDの状態
	char vbitc;						// 1ピクセルあたりのビット数
	char reserve;					// 予約領域
	unsigned short scrnx, scrny;	// 解像度X,Y
	int *vram ;						// VRAM先頭アドレスへのポインタ
} extern *sinfo ;
// 値の代入はasmhead.nasにて


//======================
//  関数宣言
//======================

#if (defined(__cplusplus))
extern "C" {
#endif

//== main.cpp ==//

void KernelMain(void);		// asmhead.nasからまず最初に呼び出される関数


//== dsctbl.cpp ==//

void init_gdtidt(void) ;					// GDT/IDTの初期化
void panic(void);
void set_segmdesc(short i, unsigned int limit, int base, int ar);		// セグメントをＧＤＴに登録


//== int.cpp ==//

void init_pic(void) ;						// PICの初期化
void allow_interrupt(void);			// 割り込みを許可

/* 割り込みハンドラ */
void inthandler07(int *esp);		// FPU演算時のCPUからの割り込み
void inthandler20(int *esp);		// タイマからの割り込み
void inthandler21(int *esp) ;		// PS/2キーボードからの割り込み
void inthandler2c(int *esp) ;		// PS/2マウスからの割り込み
void inthandler27(int *esp) ;		// 一部チップセットで起きる電気的ノイズ対策


//== graphic.cpp ==//
void boot_GUI(void) ;															// GUIの初期化
extern struct SHEET *mouse, *taskbar;

//== timer.cpp ==//

void init_pit(void);				// タイマの初期化


//== ssememcpy.cpp ==//

void ssememcpy( void *dst, void *src, int size );					// ＳＳＥを使った高速なメモリコピー

#if (defined(__cplusplus))
}
#endif
