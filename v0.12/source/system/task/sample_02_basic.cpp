//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 Liva
//=========================================

/*�@�A�N�A�t�h�e�X�g�p�^�X�N */
#include "kernel.h"
#include <stdio.h>
/*
char g_continueFlag;


Timer	timer2;
Caller *caller;
void await(UCHAR tmp) ;
*/

void sample_02_basic(void) {

	struct WINDOW *window=windowctl.create(20,250,640,480,"S.Programs d3module sample") ;

	HSP hsp(window);
	d3module d3(hsp);
	


/*	short camx, cmay, camz = 400 ;
	
	caller=new Caller(awair);
	timerctl.create(&timer2,30, caller) ;

	g_continueFlag=1;
	for(;;) {

		// �J�������W����

		// �}�E�X�{�^��
//		getkey a, 1 : camz += a * 100
//		getkey a, 2 : camz -= a * 100
		// �z�C�[��
//		camz += mousew * 2 ;

		camx = (hsp.mousex() - hsp.ginfo_winx() / 2) * 20 ;
		camy = (hsp.mousey() - hsp.ginfo_winy() / 2) * 20 ;


		// �J�����ʒu�ݒ�

		d3.setcam( camx, camy, camz,  0, 0, 0 ) ;



		hsp.color(0xffffff);
		hsp._boxf();


		; ���W���`��
		hsp.color(0) ;

		;------------------------------------------------------------
		; ���b�Z�[�W��\��

		d3.mes( "X",  1100, 0, 0 );
		d3.mes( "Y",  0, 1100, 0 );
		d3.mes( "Z",  0, 0, 1100 );

		;      ��   ������������
		;      ��    �� �\������ʒu (x, y, z)
		;      �� �\�����镶����
		;------------------------------------------------------------

		;------------------------------------------------------------
		; ����`��

		d3.arrow( 1000, 0, 0,   -1000, 0, 0 );
		d3.arrow( 0, 1000, 0,   0, -1000, 0 );
		d3.arrow( 0, 0, 1000,   0, 0, -1000 );
 
		;        ������������        ������������
		;         ��             �� ���̎n�_�̈ʒu (x, y, z)
		;         �� ���̏I�_ (��蕔��) �̈ʒu (x, y, z)
		;------------------------------------------------------------


		; ���`��
		_hsp.color(0x808080);

		;------------------------------------------------------------
		; ���� (����) ��`��

		d3.line( 0, 0, 0,  -500, -500, -500) ;

		;        ��������    ������������������
		;         ��       �� ���̒[ 2 �̈ʒu (x, y, z)
		;         �� ���̒[ 1 �̈ʒu (x, y, z)
		;------------------------------------------------------------

		;------------------------------------------------------------
		; �~ (��?) ��`��

		d3.circle( -500, -500, -500,   300 );

		;            ��������������������         ��
		;             ��                 �� �~�̔��a
		;             �� �~�̒��S�ʒu (x, y, z)
		;------------------------------------------------------------


		d3.mes( "-500, -500, -500", -500, -500, -500 ) ;


		; �{�b�N�X 1 ��`��
		_hsp.color(0x0000ff);
		d3.box( 100, 100, 100,  500, 500, 500 );


		; �{�b�N�X 2 ��`��
		_hsp.color(0xff0000);
		d3.box( 500, 500, 500,  1000, 1000, 1000 );


		_windowctl.all_refresh(_window);

		g_continueFlag--;
		for(;;) {
			taskctl.tswitch();
			if (g_continueFlag)
				break;
			asm("hlt");
		}
		
	}
*/

	

	//------------------------------------------------------------
	// �J�����ʒu�ݒ�
	
	d3.setcam( 1400, 1200, 1000,  0, 0, 0 ) ;
	
	//          ������������������         ����������
	//           ��                 �� �J���������Ă���ʒu (x, y, z)
	//           �� �J�����̈ʒu (x, y, z)
	//-----------------------------------------------------------
	
	//------------------------------------------------------------
	// �{�b�N�X��`�� (���ׂĂ̕ӂ͍��W���ɕ��s)
	
	d3.box( 500, 500, 500,   0, 0, 0) ;
	
	//      ��������������        ����������
	//       ��              �� �[�̍��W 2 (x, y, z)
	//       �� �[�̍��W 1 (x, y, z)
	//------------------------------------------------------------
	Timer xtimer;
	timerctl.wait(&xtimer,300);
	for(int cnt=0;cnt<0xFFFFF;cnt++)
		hsp.circle(0,0,40,460);

	char s[40];
	sprintf(s, "%010d", timerctl.count);

	mes(window,25,5,s);
	windowctl.all_refresh(window);

	taskctl.waitIO(MAX_ID);					//���i�v�I�ɕ��A���鎖�͂Ȃ��B����΁A���S�ȃX���[�v���
}
/*
void await(UCHAR tmp) {
	timerctl.create(&timer2,3, caller) ;
	g_continueFlag++;
	return ;
}
*/

