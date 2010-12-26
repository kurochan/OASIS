//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

#include "kernel.h"


//Bay-OS���ڐA
/*
 C++����� new / delete �T�|�[�g
 ����� C����� malloc_s / free_s �T�|�[�g
*/

void* operator new(size_t size) {
	void *addr=mem->alloc(size) ;
	return addr;
}

void operator delete(void* address) {
	mem->free_s(address);
	return;
}

void* operator new[](size_t size) {
	void *addr=mem->alloc(size) ;
	return addr;
}

void operator delete[](void* address) {
	mem->free_s(address);
	return;
}

void* malloc_s(size_t size) {
	void *addr=mem->alloc(size) ;
	return addr;
}

void free_s(void* address) {
	mem->free_s(address);
	return;
}
