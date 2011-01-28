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
			taskctl.Tswitch_I(_usingPID);
		}else{
			_usingFlag	= 1;
			_usingPID	= taskctl.GetPid();
			break;
		}
	}
	return ;
}


void AvoidDeadLock::Free(void){
	_usingFlag = 0;
	return ;
}
