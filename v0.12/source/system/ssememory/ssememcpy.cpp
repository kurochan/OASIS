//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

#include "kernel.h"

void *memcpy(void *dest, const void *src, size_t n)
{
	UINT i;

	if (n == 0) return dest;

	for ( i = 0; i < n; i++ ){
		((char*)dest)[i] = ((char*)src)[i];
	}
	return dest;
}


void ssememcpy( void *dst, void *src, int size ) {

	UINT _dst=(UINT)dst, _src=(UINT)src, _size=(UINT)size;

//    if (_size < 64) {
        memcpy( dst, src, size );
        return ;
//    }


	UINT _src2=(_src+15) & 0xFFFFFFF0;
    memcpy((void *)_dst, (void *)_src, _src2-_src);
	size-=_src2 - _src;
	_dst+=size;

	static int (*asm_ssememcpy[16])( unsigned int _dst, unsigned int _src, unsigned int _size )={
		asm_ssememcpy0, asm_ssememcpy1, asm_ssememcpy2, asm_ssememcpy3, asm_ssememcpy4,
		asm_ssememcpy5, asm_ssememcpy6, asm_ssememcpy7, asm_ssememcpy8, asm_ssememcpy9,
		asm_ssememcpy10, asm_ssememcpy11, asm_ssememcpy12, asm_ssememcpy13, asm_ssememcpy14,
		asm_ssememcpy15
	};

	_src2=(*asm_ssememcpy[_dst&0xf])(_dst, _src2, _size) ;

    memcpy( (void *)(_dst + _src2 - _src), (void *)(_src2), _size & 0x0000003F );

	return ;
}

void ssememset( void *_dst, void *_src, int _size ) {
	return;
}
