//=========================================
//	  OASIS Kernel main source code
//				Copyright (C) 2010 soS.A
//=========================================

/* �O�����̓f�o�C�X�֘A */

#include "kernel.h"
#include <stdio.h>


void INPUTCTL::i_put(const unsigned int data){
	put(data);
	return;
}

unsigned int INPUTCTL::i_get(unsigned int &i_q, const INPUTFIFO *Inputfifo){


	//�o�b�t�@������ۂ̂Ƃ��́A�Ƃ肠����0���Ԃ����
	if (i_q==p)
		return 0;


	unsigned int data = buf[i_q];
	i_q++;
	if (i_q >= size)
		i_q = 0;

	//�擾�����f�[�^���A�K�v�Ƃ���Ă���f�o�C�XID�̂��̂łȂ���΁A�������擾
	for( UINT cnt=0; cnt<MAX_ID; cnt++ )
		if (Inputfifo->idFlag[data>>8])
			return data;

	data=i_get(i_q,Inputfifo);

	return data;
}


void INPUTCTL::set_q(unsigned int &i_q){
	i_q=p;
	return ;
}

INPUTFIFO::INPUTFIFO(void){
	inputctl->set_q(q);
	for(UINT cnt=0; cnt<MAX_ID; cnt++)
		idFlag[cnt]=0;
}

unsigned int INPUTFIFO::get(void){
	return inputctl->i_get(q,this);
}

void INPUTFIFO::set_id(const unsigned dev_id){
	idFlag[dev_id]=1;
	return;
}

void INPUTFIFO::remove_id(const unsigned dev_id){
	idFlag[dev_id]=0;
	return;
}


#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02

void wait_KBC_sendready(void)
{
	/* �L�[�{�[�h�R���g���[�����f�[�^���M�\�ɂȂ�̂�҂� */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

#define PORT_KEYDAT				0x0060
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

/* �L�[�{�[�h�R���g���[���̏����� */
void init_keyboard(void) {
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

struct MOUSE_DEC mdec;


/* �}�E�X�L���� */
void enable_mouse(void) {
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	/* ���܂�������ACK(0xfa)�����M����Ă��� */
	mdec.phase = 0; /* �}�E�X��0xfa��҂��Ă���i�K */
	return;
}



