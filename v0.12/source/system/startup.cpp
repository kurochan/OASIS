//=========================================
//	  OASIS Kernel startup code
//				Copyright (C) 2010 soS.A
//=========================================

#include "kernel.h"

/*
KernelMain���Ăяo�������̃R�[�h
*/

//�}���O�����p
#if (defined(__cplusplus))
extern "C" {
#endif

void KernelStartup(void) ;

#if (defined(__cplusplus))
}
#endif

void KernelStartup(void)
{
	/* ����KernelMain�̎��s�ɐ旧���ĉ����������Ȃ�����A�����ɏ������� */

	KernelMain();
	/* ����KernelMain�̏I����ɉ������������������Ȃ�����A�����ɏ������� */

	return;
}

