//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================

/* FIFO�o�b�t�@  */
class FIFO {
// INPUTCTL�N���X�Ɍp������邩��
protected:
	unsigned int *buf, p, q, size, free_s_size;
	// *buf	: �L���o�b�t�@
	// p		: �������݈ʒu
	// q		: �ǂݍ��݈ʒu
	// size	: �o�b�t�@�T�C�Y
	// free_s_size : �o�b�t�@�󂫗e�ʂ̃T�C�Y

public:
	FIFO(const unsigned int datasize);
	~FIFO(void);
	char put(const unsigned int data);			// �f�[�^�ǉ�
	unsigned int get(void);							// �f�[�^�擾
	unsigned int status(void) const;				// �f�[�^�ʎ擾
};

