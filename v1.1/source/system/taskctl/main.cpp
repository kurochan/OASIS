/*
 *  task.cpp
 *  OASIS Project
 *
 *  Created by Liva on 11/01/12.
 *  Generated by TDD
 *
 */

#include "kernel.h"

TASKCTL taskctl;

//初期化（_io_taskと_idle_taskの追加）
void TASKCTL::Init(void){

	//TASKCTLクラスの初期化
	next_tswitch=0;
	taskmem = new SMALL_MEMORY(sizeof(TASK));
	task_fpu=0;
	_procWaitingKilled = new FIFO(5);

	//最優先タスク（番兵としての役割、及び外部入力処理を担当する）
	_io_task.priorityNext	= &_idle_task;
	_io_task.taskNext		= &_idle_task;
	_io_task.status			= OPERATING_IO_FUNC;
	//本来ならNOW_OPERATINGが妥当なのだろうが、コードの最適化のためにこの値に設定している
	_io_task.priority		= 0;
	_io_task.pid			= 1;
	set_segmdesc(TASK_GDT0 + _io_task.pid, 103, (int) &(_io_task.tss), AR_TSS32);
	char *tmp=new char[STACK_SIZE];
	_io_task.stack_entry=(UINT)tmp;
	_io_task.stack_entry+=STACK_SIZE;
	_TaskInit(&_io_task);
	_io_task.tss.eip=(int)&PrepareIOTask;


	_idle_task.priorityNext		= (TASK *)0;
	_idle_task.taskNext			= (TASK *)0;
	_idle_task.status			= OPERATING_IO_FUNC;
	//本来ならNOW_OPERATINGが妥当なのだろうが、コードの最適化のためにこの値に設定している
	_idle_task.priority			= MAX_PRIORITY+1;
	_idle_task.pid				= 0;
	set_segmdesc(TASK_GDT0+_idle_task.pid, 103, (int) &(_idle_task.tss), AR_TSS32);
	_idle_task.stack_entry = *((int *)0x28000C);															//仕方がないので直書きで
	_TaskInit(&_idle_task);
	load_tr(_idle_task.pid+TASK_GDT0 * 8);

	current=&_idle_task;
	_next_GDT_NUM=2;
	return;
}

void TASKCTL::_TaskInit(TASK *task){

	task->tss.eax	= 0;
	task->tss.ecx	= 0;
	task->tss.edx	= 0;
	task->tss.ebx	= 0;
	task->tss.ebp	= 0;
	task->tss.esi	= 0;
	task->tss.edi	= 0;
	task->tss.es	= 0;
	task->tss.ds	= 0;
	task->tss.fs	= 0;
	task->tss.gs	= 0;
	task->tss.ldtr	= 0;
	task->tss.esp	= (int)(task->stack_entry);
	task->tss.es	= 1 * 8;
	task->tss.cs	= 2 * 8;
	task->tss.ss	= 1 * 8;
	task->tss.ds	= 1 * 8;
	task->tss.fs	= 1 * 8;
	task->tss.gs	= 1 * 8;

	task->tss.eflags	= 0x00000202; /* IF = 1; */
	task->tss.iomap	= 0x40000000;

	_FpuInit(task->fpu) ;

	task->inputfifo=new INPUTFIFO();
	return;
}

void TASKCTL::_FpuInit(int *fpu){

	fpu[0] = 0x037f; /* CW(control word) */
	fpu[1] = 0x0000; /* SW(status word)  */
	fpu[2] = 0xffff; /* TW(tag word)     */
	for (UCHAR i = 3; i < 108 / 4; i++)
		fpu[i] = 0;

	return ;
}

void TASKCTL::Create(void (*task_func)()) {

	short result=Fork();
	if (result==-1)
		return;
	if (result!=0){
		task_func();
		Kill();
	}

	return ;
}

short TASKCTL::Fork(void){

	//GDT番号の決定
	bool gdtUsingFlag[MAX_TASK_NUM];
	{//使用中GDTテーブルの初期化
		for(int i=0;i<MAX_TASK_NUM;i++){
			gdtUsingFlag[i]=0;
		}
		for(TASK *task=&_io_task;task!=(TASK *)0;task=task->taskNext){
			gdtUsingFlag[task->pid]=1;
		}
	}

	asm("cli");

	TASK *new_task		= (TASK *)taskmem->alloc();
	*new_task			= *current;
	new_task->priority	= 1;
	new_task->parent	= current;

	{//使用中でないGDT番号を取得
		int i;
		//@タスクが足りなくなった時が大変かも・・・
		for(i=_next_GDT_NUM;gdtUsingFlag[i]!=0;i++){
			if (i>=MAX_TASK_NUM){
				i=0;
			}
		}
		new_task->pid=i;
		_next_GDT_NUM=i+1;		//ある程度の効率化が計れるはず・・・
	}

	{//線形リストの構築
		//優先度順に並べた時の線形リスト
		new_task->priorityNext=_io_task.priorityNext;
		_io_task.priorityNext=new_task;
		//全てのタスクを並べた時の線形リスト
		new_task->taskNext=_io_task.taskNext;
		_io_task.taskNext=new_task;
	}
	asm("sti");



	char *tmp=new char[STACK_SIZE];
	new_task->stack_entry=(UINT)tmp;
	new_task->stack_entry+=STACK_SIZE;

	new_task->inputfifo			= new INPUTFIFO(*(current->inputfifo));
	set_segmdesc(TASK_GDT0 + new_task->pid, 103, (int) &(new_task->tss), AR_TSS32);

	asm volatile ("movl	$entry,%0;":"=r"(new_task->tss.eip):);

	UINT ebp, esp;
	asm volatile ("movl %%ebp,%0;movl %%esp,%1;": "=r" (ebp), "=r" (esp));
	new_task->tss.ebp			= (UINT)new_task->stack_entry-(UINT)current->stack_entry + ebp;
	new_task->tss.esp			= (UINT)new_task->stack_entry-(UINT)current->stack_entry + esp;

	memmove((void *)(new_task->stack_entry - STACK_SIZE),(void *)(current->stack_entry - STACK_SIZE),STACK_SIZE);

	//entry以降のコードを削られないようにするための苦肉の策
	volatile bool hoge=1;
	if (hoge)
		return 0;
	asm ("entry:");
	return new_task->pid;
}

void TASKCTL::_DelCntFromPrtyLst(void){
	TASK *task;
	for(task=&_io_task;task->priorityNext!=current;task=task->priorityNext){
		if (task->priorityNext==&_idle_task){
			panic();		//なんかよくわからんけど、指定されたタスクが存在しなかった
		}
	}
	task->priorityNext=current->priorityNext;
	return;
}
void TASKCTL::_DelCntFromPrtyLst2(void){
	TASK *task;
	for(task=&_io_task;task->priorityNext!=current;task=task->priorityNext){
		if (task->priorityNext==&_idle_task){
			panic3();		//なんかよくわからんけど、指定されたタスクが存在しなかった
		}
	}
	task->priorityNext=current->priorityNext;
	return;
}

void TASKCTL::_Tswitch(void){

	if (_procWaitingKilled->status()!=0){
		_KillIOProc();
	}

	asm("cli");
	TASK *old=current;
	current=_io_task.priorityNext;
	next_tswitch = timerctl.GetCount() + (1<<current->priority);

	if (DEBUGG){
		static int tmp_x=0;
		sinfo->vram[tmp_x+1024*current->priority+1024*50]=0xff00;
		sinfo->vram[tmp_x+1024*current->pid]=0xff0000;
		tmp_x++;
	}

	asm("sti");
	if (current!=old)
		farjmp(0,(current->pid+TASK_GDT0)*8);
	return;
}

void TASKCTL::Tswitch_F(void){

	if (current!=&_io_task && current->priority<MAX_PRIORITY){
		asm("cli");
		current->priority++;
		{//優先順位を落として線形リストを繋ぎ変える
			_DelCntFromPrtyLst2();
			TASK *task=&_io_task;
			for(;task->priorityNext->priority<current->priority+1;task=task->priorityNext){
				if (task->priorityNext==&_idle_task){
					panic();		//なんかよくわからんけど、指定されたタスクが存在しなかった
				}
			}
			current->priorityNext=task->priorityNext;
			task->priorityNext=current;
		}
		asm("sti");
	}
	_Tswitch();

	return;
}

void TASKCTL::Tswitch_V(void){
	asm("cli");
	if (current->priority>1){
		current->priority--;
	}
	//線形リストから消去
	_DelCntFromPrtyLst();
	asm("sti");
	_Tswitch();

	return;
}

void TASKCTL::Tswitch_I(const short pid){
	if(current->pid==pid)
		return;
	io_cli();
	_SearchTaskFromPID(pid);
	next_tswitch = timerctl.GetCount() + 2;
	io_sti();
	farjmp(0,(pid+TASK_GDT0)*8);
	return;
}

void TASKCTL::Tswitch_IO(void){

	if (current!=&_io_task){
		current=&_io_task;
		next_tswitch = timerctl.GetCount() + 1;

		farjmp(0,(_io_task.pid+TASK_GDT0)*8);
	}

	return;
}

void TASKCTL::_SearchTaskFromPID(const short pid){

	for(current=&_io_task; current->pid!=pid ;current=current->taskNext){
		if (current->taskNext==&_idle_task){
			panic();		//なんかよくわからんけど、指定されたタスクが存在しなかった
		}
	}

	return;
}

void TASKCTL::ContinueProc(const short pid){

	_SearchTaskFromPID(pid);
	current->status=NOW_OPERATING;
	{
		TASK *task=&_io_task;
		for(;task->priorityNext->priority<current->priority;task=task->priorityNext){
			if (task->priorityNext==&_idle_task){
				panic();		//なんかよくわからんけど、指定されたタスクが存在しなかった
			}
		}
		current->priorityNext=task->priorityNext;
		task->priorityNext=current;
	}
	farjmp(0,(current->pid+TASK_GDT0)<<3);
	return ;
}

//@子供も殺せる仕組みに
void TASKCTL::Kill(void){
	if (current==&_idle_task||current==&_io_task)
		return;

	if (current->status!=OPERATING_IO_FUNC){
		_KillIOProc();
	}

	{//線形リスト上からタスクを消去
		TASK *task;
		for(task=&_io_task;task->taskNext!=current;task=task->taskNext){
			if (task->taskNext==&_idle_task){
				panic();		//なんかよくわからんけど、指定されたタスクが存在しなかった
			}
		}
		task->taskNext=current->taskNext;
		{
			TASK *target=current;
			if (current->status==NOW_OPERATING){
				_DelCntFromPrtyLst();
				task->priorityNext=target->priorityNext;
			}
		}
	}


    if (task_fpu == current) {
        task_fpu = 0;
    }

	delete current->inputfifo;
	delete [] (char *)(current->stack_entry-STACK_SIZE);
	taskmem->free_s(current);
	_Tswitch();
}

void TASKCTL::SetInput(int _id, Caller *caller){
	current->stcaller[_id]=caller ;
	current->inputfifo->set_id(_id);
	return;
}


void TASKCTL::RemoveInput(int id) {
	current->inputfifo->remove_id(id);
	return;
}

TASK *TASKCTL::GetCurrentTASK(void){
	return current;
}

short TASKCTL::GetPid(void){
	return current->pid;
}

void TASKCTL::inthandler07(void){

	asm("cli");
	asm_clts();

	if (task_fpu != current){
		if (task_fpu != 0){
			asm_fnsave(task_fpu->fpu);
		}
		asm_frstor(current->fpu);
		task_fpu = current;
	}
	asm("sti");
	return ;
}

extern "C" int getpid_CPP(void);

int getpid_CPP(void){
	return (int)taskctl.GetPid();
}
