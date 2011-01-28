//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/*　アイドルタスク */
#include "kernel.h"


//アイドルタスク
void idle_task(void) {

	//割り込みを許可
	allow_interrupt();
	taskctl.Create(main_task);

	for(;;)
		asm("hlt");
}

