//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================

#include "unsigned.h"

//�f�o�C�XID

//�V�X�e����p
static const UINT TIMER_ID_S	= 1 ;
static const UINT KEYBOARD_ID	= 2 ;
static const UINT MOUSE_ID_S	= 3 ;
static const UINT TIMER_ID		= 4 ;

static const UINT KEY_ID		= 5 ;

static const UINT MAX_ID		= 6 ;

//INPUTCTL���FIFO�ł̃f�[�^�̏��24bit��ID���ʎq�ɂȂ��Ă���
//����āA�g����f�o�C�X�̐��͍ő�16777215�B���ꂾ���̃f�o�C�X��ڑ�����n���͂��Ȃ��Ǝv���̂ŁA���Ȃ��B
//MAX_ID�͓o�^����ID�̍ő�l�ɂP�𑫂�������

//INPUTCTL��FIFO�փA�N�Z�X���邽�߂̑���
class INPUTFIFO {
	unsigned int q;
public:
	bool idFlag[MAX_ID];					// �^�X�N���o�^����IO�f�o�C�X�̃t���O���Ǘ�����z��(INPUTCTL�ȊO����̓A�N�Z�X���Ȃ��ׂ�)
	INPUTFIFO(void);
	unsigned int get(void);					//INPUTCTL����f�[�^�擾
	void set_id(const unsigned dev_id);
	void remove_id(const unsigned dev_id);
};


//�O���f�o�C�X����̓��̓f�[�^���Ǘ����邽�߂̃X�[�p�[FIFO
class INPUTCTL : private FIFO {
public:
	INPUTCTL(void):FIFO(256){};
	void i_put(const unsigned int data);										//INPUTCTLFIFO�Ƀf�[�^��ǉ��@�e�N���X��put�ƑS������
	unsigned int i_get(unsigned int &i_q, const INPUTFIFO *Inputfifo);			//INPUTCTLFIFO����f�[�^�̎擾�iINPUTFIFO����̂݌Ăяo�����j
	void set_q(unsigned int &i_q);												//INPUTCTLFIFO�Ɠ����iINPUTFIFO����̂݌Ăяo�����j
} extern *inputctl;


struct MOUSE_DEC {
	unsigned char buf[3], phase, dat;
	int x, y, btn;
} extern mdec;

void enable_mouse(void);
void init_keyboard(void);
void wait_KBC_sendready(void) ;
