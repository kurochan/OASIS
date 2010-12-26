//=========================================
//	  OASIS Kernel startup code
//				Copyright (C) 2010 soS.A
//=========================================

#include "kernel.h"

/*
KernelMainを呼び出すだけのコード
*/

//マングル回避用
#if (defined(__cplusplus))
extern "C" {
#endif

void KernelStartup(void) ;

#if (defined(__cplusplus))
}
#endif

void KernelStartup(void)
{
	/* 将来KernelMainの実行に先立って何かしたくなったら、ここに書き足す */

	KernelMain();
	/* 将来KernelMainの終了後に何か処理をさせたくなったら、ここに書き足す */

	return;
}

