//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================

/* �^�C�} */
class Timer {
public:
	short pid;
	Timer *next_timer ;
	UINT timeout ;
	bool _using;
	Timer(void);
	void set_timeout(unsigned int time, unsigned int count) ;
	Caller *FuncOfTimeout;
};

/* ��������^�C�}�̊Ǘ� */
class TIMERCTL  {
	Timer _guardTimer ;					// �ԕ��p�^�C�}
	Timer *_firstTimerAddr ;			// ���`���X�g��ň�ԍŏ��ɂ���^�C�}

public:
	UINT count;
	void init(void) ;						// ������
	void create(Timer *c_timer, unsigned int time, Caller *caller) ;	// �V�����^�C�}�̍쐬
	void wait(Timer *c_timer, unsigned int time) ;					// �V�����^�C�}�̍쐬
	void checkTimeout(UCHAR tmp) ;		// �^�C���A�E�g�����^�C�}���Ȃ����`�F�b�N�A��������΁A����̓�������� 
	void reset(void) ;					// �^�C�}�̃J�E���g��������𒴂������ɂȂ������Ƀ��Z�b�g����
	void inthandler20(int *esp) ;	// �^�C�}���荞�݂��N�������̏���
	char check_count(unsigned int c_count) const ;		// �J�E���g���̃`�F�b�N
} extern timerctl;
