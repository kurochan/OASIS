//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================



#include "unsigned.h"
#include "funcCaller.h"
#include "resource.h"
#include "int.h"
#include "memory/memory.h"
#include "memory/new.h"
#include "graphic/sheet.h"
#include "graphic/window.h"
#include "graphic/graphic.h"
#include "graphic/aqua.h"
#include "graphic/d3module.h"
#include "timer/timer.h"
#include "fifo/fifo.h"
#include "inputdev/inputdev.h"
#include "task/task.h"
#include "task/tasks.h"
#include "task/main_task/console.h"
#include "disk/ide.h"

#include "asmfunc.h"



//== HSV2RGB.cpp ==//
void HSV2RGB( UCHAR &R, UCHAR &G, UCHAR &B, UCHAR H, UCHAR S, UCHAR V ) ;


//======================
//  �\����
//======================

//== main.cpp ==//

/* �V�X�e�����̊i�[ */
struct S_INFO {
	char cyls;						// �u�[�g�Z�N�^�͂ǂ��܂Ńf�B�X�N��ǂ񂾂̂�
	char leds;						// �u�[�g���̃L�[�{�[�h��LED�̏��
	char vbitc;						// 1�s�N�Z��������̃r�b�g��
	char reserve;					// �\��̈�
	unsigned short scrnx, scrny;	// �𑜓xX,Y
	int *vram ;						// VRAM�擪�A�h���X�ւ̃|�C���^
} extern *sinfo ;
// �l�̑����asmhead.nas�ɂ�


//======================
//  �֐��錾
//======================

#if (defined(__cplusplus))
extern "C" {
#endif

//== main.cpp ==//

void KernelMain(void);		// asmhead.nas����܂��ŏ��ɌĂяo�����֐�


//== dsctbl.cpp ==//

void init_gdtidt(void) ;					// GDT/IDT�̏�����
void panic(void);
void set_segmdesc(short i, unsigned int limit, int base, int ar);		// �Z�O�����g���f�c�s�ɓo�^


//== int.cpp ==//

void init_pic(void) ;						// PIC�̏�����
void allow_interrupt(void);			// ���荞�݂�����

/* ���荞�݃n���h�� */
void inthandler07(int *esp);		// FPU���Z����CPU����̊��荞��
void inthandler20(int *esp);		// �^�C�}����̊��荞��
void inthandler21(int *esp) ;		// PS/2�L�[�{�[�h����̊��荞��
void inthandler2c(int *esp) ;		// PS/2�}�E�X����̊��荞��
void inthandler27(int *esp) ;		// �ꕔ�`�b�v�Z�b�g�ŋN����d�C�I�m�C�Y�΍�


//== graphic.cpp ==//
void boot_GUI(void) ;															// GUI�̏�����
extern struct SHEET *mouse, *taskbar;

//== timer.cpp ==//

void init_pit(void);				// �^�C�}�̏�����


//== ssememcpy.cpp ==//

void ssememcpy( void *dst, void *src, int size );					// �r�r�d���g���������ȃ������R�s�[

#if (defined(__cplusplus))
}
#endif
