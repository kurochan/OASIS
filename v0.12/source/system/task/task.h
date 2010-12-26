//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================


#define TASK_GDT0		3
#define AR_TSS32		0x0089

struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip,eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};


//TASK�\���̂Ń��[�v�^���`���X�g���\�z����
struct TASK{
	char flag;								// flag:0(GDT���o�^) 1�i�ғ����j3(I/O�f�o�C�X�҂����X���[�v)4(I/O�f�o�C�X������)
	char flag_old;
	UINT ioWaitID;
	char stPriority;						// �D��x(�l�����������D��x������)1���ŏ�
	short GDT_num;							// GDT�ԍ���1~1fff�܂�
	TASK *next;
	TSS32 tss;
	TSS32 tss_io;					// I/O�����p�ɉ��z�I�ɐ��ݏo�����^�X�N�̂��߂�TSS
	INPUTFIFO	*inputfifo;
	int fpu[108/4];
	int fpu_io[108/4];
	Caller *stcaller[MAX_ID];				// IO�f�o�C�X����̐M������������֐��̓o�^�ꏊ
};


// �e�^�X�N�ŗL�̂��̂��Ǘ�(���̓f�o�C�X����̐M���Ƃ�)
class TASKCTL {

	TASK *task_fpu;
	bool status_fpu;							// ����FPU���g�p���Ă���^�X�N��I/O�f�o�C�X���������ǂ���
	void fpu_init(int *fpu);

	TASK	idle_task;						//�A�C�h���^�X�N
	TASK *first, *current;				//first:TASK�\���̂����`���X�g���\������ۂ̐擪�^�X�N current:���ݎ��s���̃^�X�N
	SMALL_MEMORY *taskmem;						//TASK�\���̂��m�ۂ��邽�߂̓��I�m�ۃV�X�e��
	TSS32 *f_alloc(void) ;
	void t_init(TASK *task);
	void _InitIOtss(TASK *task, UINT data) ;
	bool status;								// 1:�S�^�X�N�X���[�v���@0:�ғ���
	static const UINT stPriority_MAX = 10;
public:
	UINT next_tswitch;
	void init(int task_addr);
	void create(int task_addr) ;
	unsigned int get_signal(void);
	void set_input(int _id, Caller *caller);
	void remove_input(int id);
	short get_pid(void);
	TASK *GetCurrentTASK(void);
	void tswitch(void);
	void tswitch_F(void);
	void waitIO(const int _id);					//Caller::Execute�֐����ł͐�΂ɌĂяo���Ă͂����Ȃ��I
	void continueProcess(const short pid);
	void inthandler07(void);
	void DeleteIOtask(void);
} extern taskctl;

void StartupIOtask(UINT io_id, UCHAR data);
void G_DeleteIOtask(void);

/*
�v���O�����I����

    if (taskctl->task_fpu == task) {
        taskctl->task_fpu = 0;
    }
*/
//���K�v�I
