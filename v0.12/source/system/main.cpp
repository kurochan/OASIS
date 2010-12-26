//=========================================
//	  OASIS Kernel main source code
//				Copyright (C) 2010 soS.A
//=========================================

#include "kernel.h"


//======================
//  クラス
//======================
Memory 		*mem;			// メモリ管理用クラス
INPUTCTL	*inputctl;		// 外部入力デバイスからの信号を溜めるFIFOバッファ

//======================
//  構造体
//======================
struct S_INFO *sinfo = (struct S_INFO *) 0x00000ff0 ;

//======================
//  関数宣言
//======================
void system_init(void) ;

//======================
//  関数本体
//======================

/*　asmhead.nasからここが呼び出される */
void KernelMain(void) {
/*	if (sinfo->vbitc!=32)
		for(;;)
			io_hlt();						//@例外処理*/

	/* システム初期化 */
	Memory mem_def ;		//　メモリ容量チェック＋メモリ管理システム初期化
	mem=&(mem_def);			//　newはmem.allocを呼び出す事になるため、Memoryクラスの初期化にnewを使ってはならない
	//　この２行はsystem_initに収めたかったのだが、それをするとmem_defが開放されてしまうので、不可能

	system_init() ;
	// init系関数ここまで
	idle_task();
}

/* システム初期化系統を集約 */
void system_init(void) {

	init_gdtidt() ;						// GDT,IDT初期化
	init_pic() ;						// PIC初期化

	timerctl.init() ;					// タイマコントローラの初期化

	init_pit();							// タイマ初期化

	init_keyboard();					// キーボード初期化
	// マウスの初期化はmain_taskでやる

	inputctl=new INPUTCTL;	// 外部入力デバイスからの信号を溜めるFIFOバッファを作成

	taskctl.init((int) &main_task);						// タスクコントローラの初期化

	boot_GUI() ;

	//タスク追加
	taskctl.create((int) &timer_task);	//タイマ処理
//	taskctl.create((int) &counter_task);	//カウンタ
	taskctl.create((int) &aqua_task);	//AquaGUIテスト
	taskctl.create((int) &sample_02_basic);	//d3moduleテスト

	return ;

}

