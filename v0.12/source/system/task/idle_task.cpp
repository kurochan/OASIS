//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/*�@�A�C�h���^�X�N */
#include "kernel.h"


//�A�C�h���^�X�N
void idle_task(void) {

	//���荞�݂�����
	allow_interrupt();

	for(;;)
		asm("hlt");
}

