//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* タスク関係 */
#include "kernel.h"


TASKCTL taskctl ;


void TASKCTL::init(int task_addr) {

	taskmem=new SMALL_MEMORY(sizeof(struct TASK));
	status=1;
	next_tswitch=0;

	// アイドルタスクを設定(内部的にはf_allocとほとんど変わらない)
	set_segmdesc(TASK_GDT0, 103, (int) &(idle_task.tss), AR_TSS32);

	idle_task.tss.eflags = 0x00000202; /* IF = 1; */
	idle_task.tss.iomap = 0x40000000;

	load_tr(TASK_GDT0 * 8);
	task_fpu=0;
	status_fpu=0;


	// メインタスクを設定
	struct TASK *main_task=(TASK *)taskmem->alloc();
	set_segmdesc(TASK_GDT0+1, 103, (int) &(main_task->tss), AR_TSS32);

	main_task->tss.eflags = 0x00000202; /* IF = 1; */
	main_task->tss.iomap = 0x40000000;
	main_task->GDT_num=1;
	main_task->tss.eip = task_addr;
	
	first=main_task;
	current=main_task;
	t_init(main_task);

	return ;
}

void TASKCTL::create(int task_addr) {
	struct TSS32 *new_task=f_alloc();
	new_task->eip = task_addr;
	return ;
}

class TSS32 *TASKCTL::f_alloc(void) {

	struct TASK *c_task=first;
	//線形リストを調べる
	for(short cnt=1;;cnt++){
		
		if (c_task->flag==0){
			c_task->flag=1;

			//昔使われていたTSSには何が代入されているかわからないので、初期化
			c_task->tss.eax = 0;
			c_task->tss.ecx = 0;
			c_task->tss.edx = 0;
			c_task->tss.ebx = 0;
			c_task->tss.ebp = 0;
			c_task->tss.esi = 0;
			c_task->tss.edi = 0;
			c_task->tss.es = 0;
			c_task->tss.ds = 0;
			c_task->tss.fs = 0;
			c_task->tss.gs = 0;
			c_task->tss.ldtr = 0;
			fpu_init(c_task->fpu) ;

			break;
		}

		//既存の線形リスト上に空きがなかったので、新しくTASKクラスを作成
		if (c_task->next==first){

			if (cnt>=0x1fff)
				panic();

			struct TASK *old=c_task;
			c_task=(struct TASK *)taskmem->alloc();
			t_init(c_task);
			c_task->GDT_num=cnt+1;
			old->next=c_task;
			break;
		}
		c_task=c_task->next;

	}

	set_segmdesc(TASK_GDT0 + c_task->GDT_num, 103, (int) &(c_task->tss), AR_TSS32);

	c_task->tss.eflags = 0x00000202; /* IF = 1; */
	c_task->tss.iomap = 0x40000000;

	return &(c_task->tss);
}

void TASKCTL::t_init(TASK *task){

	task->flag=1;
	task->next=first;

	task->tss.esp = (int)((unsigned int)malloc_s(64 * 1024) + (64 * 1024));
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;

	fpu_init(task->fpu) ;

	task->inputfifo=new INPUTFIFO();
	return;
}

void TASKCTL::fpu_init(int *fpu){

	fpu[0] = 0x037f; /* CW(control word) */
	fpu[1] = 0x0000; /* SW(status word)  */
	fpu[2] = 0xffff; /* TW(tag word)     */
	for (char i = 3; i < 108 / 4; i++)
		fpu[i] = 0;

	return ;
}

void TASKCTL::_InitIOtss(TASK *task, UINT data){
	task->flag_old=task->flag;
	task->flag=4;
	
	task->tss_io.eip = (int)&StartupIOtask;
	task->tss_io.esp = (int)((UINT)malloc_s(64 * 1024) + (64 * 1024)-12);
	*((UINT *)task->tss_io.esp)  = (int)&G_DeleteIOtask;
	*((UINT *)task->tss_io.esp+4)= 0xFFFFFFFF;//data>>3;
	*((UINT *)task->tss_io.esp+8)= 0xFFFFFFFF;//(UCHAR)data&0xff;
	task->tss_io.es = 1 * 8;
	task->tss_io.cs = 2 * 8;
	task->tss_io.ss = 1 * 8;
	task->tss_io.ds = 1 * 8;
	task->tss_io.fs = 1 * 8;
	task->tss_io.gs = 1 * 8;
	task->tss_io.eax = 0;
	task->tss_io.ecx = 0;
	task->tss_io.edx = 0;
	task->tss_io.ebx = 0;
	task->tss_io.ebp = 0;
	task->tss_io.esi = 0;
	task->tss_io.edi = 0;
	task->tss_io.es = 0;
	task->tss_io.ds = 0;
	task->tss_io.fs = 0;
	task->tss_io.gs = 0;
	task->tss_io.ldtr = 0;
	task->tss_io.eflags = 0x00000202; /* IF = 1; */
	task->tss_io.iomap = 0x40000000;
	fpu_init(task->fpu_io);
	set_segmdesc(TASK_GDT0 + task->GDT_num, 103, (int) &(task->tss_io), AR_TSS32);

	return;
}

void TASKCTL::inthandler07(void){

	io_cli();
	asm_clts();

	if ((task_fpu != current)||(current->flag==4&&!status_fpu)||(current->flag!=4&&status_fpu)) {
		if (task_fpu != 0){
			if (status_fpu){
				asm_fnsave(task_fpu->fpu_io);
			}else{
				asm_fnsave(task_fpu->fpu);
			}
		}
		if (current->flag==4){
			asm_frstor(current->fpu_io);
			status_fpu=1;
		}else{
			asm_frstor(current->fpu);
			status_fpu=0;
		}
		task_fpu = current;
	}
	io_sti();
	return ;
}


void TASKCTL::set_input(int _id, Caller *caller){
	current->stcaller[_id]=caller ;
	current->inputfifo->set_id(_id);
	return;
}


void TASKCTL::remove_input(int id) {
	current->inputfifo->remove_id(id);
	return;
}

TASK *TASKCTL::GetCurrentTASK(void){
	return current;
}


short TASKCTL::get_pid(void){
	return current->GDT_num;
}

unsigned int TASKCTL::get_signal(void) {
	return current->inputfifo->get();
}


void panic2(void){
	io_cli();
	for(int x=0; x<sinfo->scrnx*sinfo->scrnx; x+=5)
		sinfo->vram[x]=0xff00;
	
	for(;;)
		asm("hlt");
}

void TASKCTL::tswitch(void) {
	io_cli();

	struct TASK *old=current;

	// スリープ状態のタスクはスキップ
	while(1) {
		current=current->next ;

		//I/O処理中は他のI/O処理を受け付けない
		if (current->flag!=4){
			for(;;){
				UINT data=current->inputfifo->get();
				if (data!=0){
					// 処理待ちのデータあり！
					if (current->flag==3 && (data>>8)==current->ioWaitID){
						current->flag=1;
					}else {
						current->stcaller[data>>8]->Execute((UCHAR)data&0xff);
/*						_InitIOtss(current,data);
						io_sti();
						farjmp(0,(current->GDT_num+TASK_GDT0)*8);*/
					}
				}else{
					break ;
				}
			}
		}

		if ( current->flag==1||current->flag==4 )
			break;

		// 全てのタスクがスリープ中
		// 多段フィードパックキューに移行すれば、status条件を削除できそう
		// status条件どころか、アイドルタスクをキュー内に組み込む事で、この部分を全て消せる
		if ( current==old ) {

			//全タスクスリープ状態突入時
			if (!status) {
				status=1;
				io_sti();
				farjmp(0,TASK_GDT0*8);		// アイドルタスク始動
			}
			io_sti();
			return;
		}
	}

	io_sti();
	if (current!=old) {
		status=0;
		farjmp(0,(current->GDT_num+TASK_GDT0)*8);
	}

	return;
}

void TASKCTL::tswitch_F(void) {
	tswitch();
	return;
}

//Caller::Execute関数内では絶対に呼び出してはいけない！
void TASKCTL::waitIO(const int _id){
	current->flag=3;
	current->ioWaitID=_id ;
	tswitch();
	return ;
}

void TASKCTL::continueProcess(const short pid){
	struct TASK *old=current;

	// スリープ状態のタスクはスキップ
	while(1) {
		current=current->next ;

		if ( current->GDT_num==pid )
			break;

		// 指定されたpidは存在しない
		if ( current==old )
			return;
	}

	current->flag=1;
//	farjmp(0,(current->GDT_num+TASK_GDT0)<<3);
	return ;
}

void TASKCTL::DeleteIOtask(void){
	current->flag=current->flag_old;
	set_segmdesc(TASK_GDT0 + current->GDT_num, 103, (int) &(current->tss), AR_TSS32);
	tswitch();
}

int getpid_CPP(void){
	return (int)taskctl.get_pid();
}

void StartupIOtask(UINT io_id, UCHAR data){
	TASK *current=taskctl.GetCurrentTASK() ;
	if (io_id==0){
		panic2();
	}else {
		panic();
	}

	current->stcaller[io_id]->Execute(data);
	return;
}

void G_DeleteIOtask(void){
	panic2();
	taskctl.DeleteIOtask();
}
