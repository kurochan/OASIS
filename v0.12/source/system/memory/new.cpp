//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

#include "kernel.h"


//Bay-OSより移植
/*
 C++言語の new / delete サポート
 および C言語の malloc_s / free_s サポート
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
