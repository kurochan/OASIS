//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* FIFO���C�u���� */

#include "kernel.h"

//#define FLAGS_OVERRUN		0x0001

/* FIFO�o�b�t�@�̏����� */
FIFO::FIFO(const unsigned int datasize) {
	size=datasize;			// �o�b�t�@�T�C�Y
	buf = (unsigned int *)malloc_s(datasize<<2) ;	//@����̓X�^�b�N���g���������������ƁE�E�E
	free_s_size = datasize; 	// ��
	p = 0; 						// �������݈ʒu
	q = 0; 						// �ǂݍ��݈ʒu
}

/* FIFO�o�b�t�@�̖��� */
FIFO::~FIFO(void) {
	free_s(buf) ;
}

/* FIFO�փf�[�^�𑗂荞��Œ~���� */
char FIFO::put(const unsigned int data){
	// �󂫂��Ȃ��Ă��ӂꂽ
	if (!free_s_size) {
		free_s_size=(size>>2)+1;
		q+=(size>>2)+1 ;	// �Â����͏���

		if (q >= size)
			q-= size;
	}

	buf[p] = data;			// �������݈ʒu�Ƀf�[�^��}��

	// �������݈ʒu�����ցi�����������݈ʒu���o�b�t�@�T�C�Y�𒴂��Ă���A�O�ցj
	p++;
	if (p >= size)
		p = 0;

	free_s_size--;							// �f�[�^�������݂ɂ��A�󂫃T�C�Y������

	return 0;
}

/* FIFO����f�[�^�������Ă��� */
unsigned int FIFO::get(void) {
	unsigned int data;

	// �o�b�t�@������ۂ̂Ƃ��́A�Ƃ肠����0xFFFFFFFF���Ԃ����
	if (free_s_size == size)
		return 0xFFFFFFFF;

	data = buf[q];						// �f�[�^�ݒ�
	// �ǂݍ��݈ʒu�����炷�i�e�h�e�n�̃T�C�Y����͂ݏo���Ă�����A�␳�j
	q++;
	if (q >= size)
		q = 0;
	// ���łɋ󂫗e�ʂ����₵�Ă���
	free_s_size++;

	return data;
}

/* �ǂ̂��炢�f�[�^�����܂��Ă��邩��񍐂��� */
unsigned int FIFO::status(void) const{
	return size - free_s_size;
}
