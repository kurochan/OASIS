//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* メモリ関係 */
//memoryクラスメンバ群



#include "kernel.h"

#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

extern struct S_INFO *sinfo ;

bool Memory::alreadyExist = 0;

/*メモリ総要領チェック関数*/
//メモリ管理システム初期化。メモリ確保、解放系関数を使う前に必ず呼び出す必要がある
Memory::Memory(void) {

	if(alreadyExist)
		panic();

	alreadyExist = 1;

	unsigned int cr0 = load_cr0();
	cr0 |= CR0_CACHE_DISABLE; /* キャッシュ禁止 */
	store_cr0(cr0);

	total_size = (memcloot() + MEM_BLOCK_SIZE - 1) & MEM_BLOCK_MASK;	//asmfunc_memory.nasのメモリ総要領チェックルーチン

	//システムメモリ分の要領すらない！？
	if (total_size == 0)
		panic();


	cr0 = load_cr0();
	cr0 &= ~CR0_CACHE_DISABLE; /* キャッシュ許可 */
	store_cr0(cr0);

	/* MEMAFの初期化 */

	//最初の二つのＭＥＭＡＦ構造体は、最初のMemManager+それ以前の0x400000を登録
	last_manage_area = (MemManager*)SYSTEM_MEMORY_SIZE ;
	last_manage_area->data_count = 0;
	first_alloc_field = last_manage_area->data();

	first_alloc_field[0].field_addr = (void*)0 ;
	first_alloc_field[0].field_size = SYSTEM_MEMORY_SIZE ;

	last_manage_area->data_count++;

	first_alloc_field[0].next_field = &first_alloc_field[1];
	first_alloc_field[1].field_addr = (void*)SYSTEM_MEMORY_SIZE ;
	first_alloc_field[1].field_size = MEM_BLOCK_SIZE ;

	last_manage_area->data_count++;

	//0x400000からのメモリ管理領域があふれた時の次のメモリ管理領域を作成
	first_alloc_field[1].next_field = &first_alloc_field[2];
	first_alloc_field[2].field_addr = (void *) (SYSTEM_MEMORY_SIZE + MEM_BLOCK_SIZE);
	first_alloc_field[2].field_size = MEM_BLOCK_SIZE ;

	last_manage_area->data_count++;
	last_manage_area->next_area = (MemManager*)(first_alloc_field[2].field_addr);
	last_manage_area->next_area->before_area = last_manage_area;

	//番兵を作成
	first_alloc_field[2].next_field = &first_alloc_field[3];
	first_alloc_field[3].field_addr = (void *) (total_size - MEM_BLOCK_SIZE);
	first_alloc_field[3].field_size = MEM_BLOCK_SIZE ;
	first_alloc_field[3].next_field = (MEMAF *)0;

	last_manage_area->data_count++;


	if ((unsigned int)(sinfo->vram) > total_size - MEM_BLOCK_SIZE)
		return ;

	if ((unsigned int)(sinfo->vram) < SYSTEM_MEMORY_SIZE)
		return ;

	//MEMAF構造体とＶＲＡＭが重なっていた時
	if ((unsigned int)sinfo->vram < SYSTEM_MEMORY_SIZE + MEM_BLOCK_SIZE * 2)
		panic();


	//ＶＲＡＭのメモリ確保
	unsigned int vmem = (sinfo->scrnx*sinfo->scrny*(sinfo->vbitc>>3) + MEM_BLOCK_SIZE - 1) & MEM_BLOCK_MASK;

	first_alloc_field[2].next_field = &first_alloc_field[4] ;
	first_alloc_field[4].field_addr = (void *)sinfo->vram ;
	first_alloc_field[4].field_size = vmem ;
	first_alloc_field[4].next_field = &first_alloc_field[3] ;

	last_manage_area->data_count++;

}


/* 空きサイズの合計を報告 */
unsigned int Memory::FuncGetUnusing(void) const{

	unsigned int usedSize = total_size;

	for (MEMAF *af = first_alloc_field; af != 0 ; af = af->next_field )
		usedSize -= af->field_size;

	return usedSize;
}


/* システム総メモリサイズを報告 */
unsigned int Memory::FuncGetTotal(void) const{
	return total_size;
}


Memory::MEMAF *Memory::getFreeSpace(char *createManager) {
	//空いているメモリ空間（一番最後）を調べる
	if(last_manage_area->data_count == MANAGER_DATA_NUM){
		//その管理領域に次のMEMAFを作ることができなければ
		*createManager = 1;
		return last_manage_area->next_area->data();
	}

	return &(last_manage_area->data()[last_manage_area->data_count]);
}

/*
メモリ確保関数
引数
バイト単位でのメモリ確保サイズ
戻り値
0	確保失敗（メモリ不足など）
その他	確保成功、戻り値のアドレスが確保したメモリ帯域の先頭アドレス
*/
void *Memory::alloc(unsigned int size) {

	if (size == 0)
		return 0;

	//切り上げて、4kバイト単位にする
	size = (size + MEM_BLOCK_SIZE - 1) & MEM_BLOCK_MASK;

	_using.Lock() ;
	void *val=_alloc(size);
	_using.Free();

	return val;
}

void *Memory::_alloc(unsigned int size) {

	for ( MEMAF *af = first_alloc_field ; af->next_field != 0 ; af = af->next_field ) {

		unsigned int nextAddr =  (unsigned int)(af->next_field->field_addr) ;
		//次の空きメモリ幅のサイズを計算
		unsigned int free_s_size = nextAddr - (unsigned int)af->field_addr - af->field_size ;

		//メモリを確保できる場合
		if (free_s_size >= size) {
			char createManager = 0;

			MEMAF *newAF = getFreeSpace(&createManager) ;

			newAF->field_addr = (void *)(nextAddr - size) ;
			newAF->field_size = size ;
			newAF->next_field = af->next_field ;
			af->next_field = newAF ;

			memset(newAF->field_addr, 0, size);

			//メモリ管理領域が満杯になり、新しく確保する必要が出た時
			if (createManager) {
				last_manage_area = last_manage_area->next_area;
				last_manage_area->data_count++;
				MemManager *newManager = (MemManager*)alloc(MEM_BLOCK_SIZE) ;
				newManager->before_area = last_manage_area;
				last_manage_area->next_area = newManager;
			}else{
				last_manage_area->data_count++;
			}

			return newAF->field_addr;
		}
	}

	//メモリが足りない
	panic();
}


/*
メモリ解放関数
m_allocで取得したメモリ幅を解放
戻り値
0	正常終了
-1	指定アドレスに誤り。エラー
*/
char Memory::free_s(void *addr) {

	if (!addr)
		return -1;

	if ((int)addr & (MEM_BLOCK_SIZE - 1))
		return -1 ;		//addrを先頭とするメモリフィールドは存在しない

	_using.Lock() ;
	char val=_free_s(addr);
	_using.Free() ;

	return val;
}

char Memory::_free_s(void *addr) {

	/*
	アルゴリズム
	・シークして、addrに対応するメモリフィールドを探索
	・見つかればそこを解放、一番最後のメモリフィールドを解放部に持ってくる（メモリフィールドの断片化を防ぎ、メモリを効率的に使うため）
	・return
	*/

	MEMAF *lastAF = &last_manage_area->data()[last_manage_area->data_count - 1];	//メモリ上の最後のMEMAF

	MEMAF *target = 0;			//解放対象のMEMAFのリスト上での一つ前のMEMAF
	MEMAF *bLast = 0;			//メモリ上の最後のMEMAFのリスト上での一つ前のMEMAF

	for ( MEMAF *af = first_alloc_field ; af->next_field != 0 ; af = af->next_field ) {
		if (af->next_field->field_addr == addr){	//検索対象を発見
			target = af;
		}

		if(af->next_field == lastAF){					//最後の一つ前のMEMAFを発見
			bLast = af;
		}

		if(target && bLast) break;
	}

	if(!target || !bLast)
		return -1;

	MEMAF *delAF = target->next_field;
	target->next_field = delAF->next_field;

	if(delAF == lastAF){
		//解放対象が線形リスト上で一番最後だった場合
		memset(delAF, 0, sizeof(MEMAF));
	}else{
		//そうでない場合は、メモリ上で最後のMEMAFを空いた領域に移動
		memcpy(delAF, lastAF, sizeof(MEMAF));
		memset(lastAF, 0, sizeof(MEMAF));
		bLast->next_field = delAF;
	}

	last_manage_area->data_count--;

	if(last_manage_area->data_count == 0){
		//もしメモリ管理領域がからになった場合は解放する
		last_manage_area->before_area->next_area = last_manage_area->next_area;
		last_manage_area->next_area->before_area = last_manage_area->before_area;
		void *delAddr = last_manage_area;
		last_manage_area = last_manage_area->before_area;
		free_s(delAddr);
	}

	return 0;
}


SMALL_MEMORY::SMALL_MEMORY(unsigned int size) {

	field_size=size ;
	mem_max = (4096-4-field_size)/field_size ;	//1クラスタ内の小メモリの数
	if (mem_max == 0) {
		flag = 0;
		return;
	}

	first_s_field = malloc_s(4096) ;
	flag = 1;
	return;
}

SMALL_MEMORY::~SMALL_MEMORY(void) {
	if (!flag)
		return;

	int *d_field = (int *)first_s_field, next_field ;

	for(;;) {
		next_field = d_field[4092] ;
		if (!next_field){
			free_s(d_field) ;
			break ;
		}
		free_s(d_field) ;
		d_field=(int *)next_field ;
	}
	return ;
}

void* SMALL_MEMORY::craster_realloc(void) {
	return malloc_s(4096) ;
}

void* SMALL_MEMORY::alloc(void) {
	if (!flag)
		return (void *)-1;

	char *flag = (char *)first_s_field ;
	short x = 0;

	for(;;) {
		if (!flag[x]) {
			flag[x]=1 ;
			return (flag+x+4) ;
		}
		x += field_size+4 ;
		if (x > 4092) {
			unsigned int *a = (unsigned int *)(flag + 4092) ;
			if (!*a){
				*a = (unsigned int)craster_realloc() ;
			}

			flag = (char *)(*a) ;
			x=0 ;
		}
	}
}

void SMALL_MEMORY::free_s(void *addr) {
	if (!flag)
		return;

	char *flag = (char *)addr ;
	short x = 0;
	*(flag-4)=0;

	flag = (char *)((unsigned int)flag&0xfffff000) ;
	for(;;) {
		if (flag[x])
			return;

		x+=field_size+4 ;
		if (x>4092) {
			free_s(flag) ;
			return;
		}
	}
}
