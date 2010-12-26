//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================

#include <string.h>

void* operator new(size_t size) ;
void operator delete(void* address) ;
void* operator new[](size_t size) ;
void operator delete[](void* address) ;
void* operator new(size_t, void*) throw();
void operator delete(void*) throw();
void* operator new[](size_t, void*) throw();
void operator delete[](void*) throw();
void* malloc_s(size_t size) ;
void free_s(void* address) ;

