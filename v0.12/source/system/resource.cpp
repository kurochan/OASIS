/*
 *  resource.cpp
 *
 *  OASIS Project
 *
 *  Created by Liva on 10/12/25.
 *
 */

#include "kernel.h"


AvoidDeadLock::AvoidDeadLock(void){
	_usingFlag = 0;
}

void AvoidDeadLock::Lock(void){
	for(;;) {
		if (_usingFlag){
			taskctl.tswitch();
		}else{
			_usingFlag	= 1;
			_usingPID	= taskctl.get_pid();
			break;
		}
	}
	return ;
}


void AvoidDeadLock::Free(void){
	_usingFlag = 0;
	return ;
}
