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


//TASK構造体でループ型線形リストを構築する
struct TASK{
	char flag;								// flag:0(GDT未登録) 1（稼働中）3(I/Oデバイス待ち＝スリープ)4(I/Oデバイス処理中)
	char flag_old;
	UINT ioWaitID;
	char stPriority;						// 優先度(値が小さい程優先度が高い)1が最小
	short GDT_num;							// GDT番号は1~1fffまで
	TASK *next;
	TSS32 tss;
	TSS32 tss_io;					// I/O処理用に仮想的に生み出されるタスクのためのTSS
	INPUTFIFO	*inputfifo;
	int fpu[108/4];
	int fpu_io[108/4];
	Caller *stcaller[MAX_ID];				// IOデバイスからの信号を処理する関数の登録場所
};


// 各タスク固有のものを管理(入力デバイスからの信号とか)
class TASKCTL {

	TASK *task_fpu;
	bool status_fpu;							// 現在FPUを使用しているタスクがI/Oデバイス処理中かどうか
	void fpu_init(int *fpu);

	TASK	idle_task;						//アイドルタスク
	TASK *first, *current;				//first:TASK構造体が線形リストを構成する際の先頭タスク current:現在実行中のタスク
	SMALL_MEMORY *taskmem;						//TASK構造体を確保するための動的確保システム
	TSS32 *f_alloc(void) ;
	void t_init(TASK *task);
	void _InitIOtss(TASK *task, UINT data) ;
	bool status;								// 1:全タスクスリープ中　0:稼働中
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
	void waitIO(const int _id);					//Caller::Execute関数内では絶対に呼び出してはいけない！
	void continueProcess(const short pid);
	void inthandler07(void);
	void DeleteIOtask(void);
} extern taskctl;

void StartupIOtask(UINT io_id, UCHAR data);
void G_DeleteIOtask(void);

/*
プログラム終了時

    if (taskctl->task_fpu == task) {
        taskctl->task_fpu = 0;
    }
*/
//が必要！
