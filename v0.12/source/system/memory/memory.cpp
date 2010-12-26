//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* �������֌W */
//memory�N���X�����o�Q



#include "kernel.h"

#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

extern struct S_INFO *sinfo ;

bool Memory::alreadyExist = 0;

/*���������v�̃`�F�b�N�֐�*/
//�������Ǘ��V�X�e���������B�������m�ہA����n�֐����g���O�ɕK���Ăяo���K�v������
Memory::Memory(void) {

	if(alreadyExist)
		panic();

	alreadyExist = 1;

	unsigned int cr0 = load_cr0();
	cr0 |= CR0_CACHE_DISABLE; /* �L���b�V���֎~ */
	store_cr0(cr0);

	total_size = (memcloot() + MEM_BLOCK_SIZE - 1) & MEM_BLOCK_MASK;	//asmfunc_memory.nas�̃��������v�̃`�F�b�N���[�`��

	//�V�X�e�����������̗v�̂���Ȃ��I�H
	if (total_size == 0)
		panic();


	cr0 = load_cr0();
	cr0 &= ~CR0_CACHE_DISABLE; /* �L���b�V������ */
	store_cr0(cr0);

	/* MEMAF�̏����� */

	//�ŏ��̓�̂l�d�l�`�e�\���̂́A�ŏ���MemManager+����ȑO��0x400000��o�^
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

	//0x400000����̃������Ǘ��̈悪���ӂꂽ���̎��̃������Ǘ��̈���쐬
	first_alloc_field[1].next_field = &first_alloc_field[2];
	first_alloc_field[2].field_addr = (void *) (SYSTEM_MEMORY_SIZE + MEM_BLOCK_SIZE);
	first_alloc_field[2].field_size = MEM_BLOCK_SIZE ;

	last_manage_area->data_count++;
	last_manage_area->next_area = (MemManager*)(first_alloc_field[2].field_addr);
	last_manage_area->next_area->before_area = last_manage_area;

	//�ԕ����쐬
	first_alloc_field[2].next_field = &first_alloc_field[3];
	first_alloc_field[3].field_addr = (void *) (total_size - MEM_BLOCK_SIZE);
	first_alloc_field[3].field_size = MEM_BLOCK_SIZE ;
	first_alloc_field[3].next_field = (MEMAF *)0;

	last_manage_area->data_count++;


	if ((unsigned int)(sinfo->vram) > total_size - MEM_BLOCK_SIZE)
		return ;

	if ((unsigned int)(sinfo->vram) < SYSTEM_MEMORY_SIZE)
		return ;

	//MEMAF�\���̂Ƃu�q�`�l���d�Ȃ��Ă�����
	if ((unsigned int)sinfo->vram < SYSTEM_MEMORY_SIZE + MEM_BLOCK_SIZE * 2)
		panic();


	//�u�q�`�l�̃������m��
	unsigned int vmem = (sinfo->scrnx*sinfo->scrny*(sinfo->vbitc>>3) + MEM_BLOCK_SIZE - 1) & MEM_BLOCK_MASK;

	first_alloc_field[2].next_field = &first_alloc_field[4] ;
	first_alloc_field[4].field_addr = (void *)sinfo->vram ;
	first_alloc_field[4].field_size = vmem ;
	first_alloc_field[4].next_field = &first_alloc_field[3] ;

	last_manage_area->data_count++;

}


/* �󂫃T�C�Y�̍��v��� */
unsigned int Memory::FuncGetUnusing(void) const{

	unsigned int usedSize = total_size;

	for (MEMAF *af = first_alloc_field; af != 0 ; af = af->next_field )
		usedSize -= af->field_size;

	return usedSize;
}


/* �V�X�e�����������T�C�Y��� */
unsigned int Memory::FuncGetTotal(void) const{
	return total_size;
}


Memory::MEMAF *Memory::getFreeSpace(char *createManager) {
	//�󂢂Ă��郁������ԁi��ԍŌ�j�𒲂ׂ�
	if(last_manage_area->data_count == MANAGER_DATA_NUM){
		//���̊Ǘ��̈�Ɏ���MEMAF����邱�Ƃ��ł��Ȃ����
		*createManager = 1;
		return last_manage_area->next_area->data();
	}

	return &(last_manage_area->data()[last_manage_area->data_count]);
}

/*
�������m�ۊ֐�
����
�o�C�g�P�ʂł̃������m�ۃT�C�Y
�߂�l
0	�m�ێ��s�i�������s���Ȃǁj
���̑�	�m�ې����A�߂�l�̃A�h���X���m�ۂ����������ш�̐擪�A�h���X
*/
void *Memory::alloc(unsigned int size) {

	if (size == 0)
		return 0;

	//�؂�グ�āA4k�o�C�g�P�ʂɂ���
	size = (size + MEM_BLOCK_SIZE - 1) & MEM_BLOCK_MASK;

	_using.Lock() ;
	void *val=_alloc(size);
	_using.Free();

	return val;
}

void *Memory::_alloc(unsigned int size) {

	for ( MEMAF *af = first_alloc_field ; af->next_field != 0 ; af = af->next_field ) {

		unsigned int nextAddr =  (unsigned int)(af->next_field->field_addr) ;
		//���̋󂫃��������̃T�C�Y���v�Z
		unsigned int free_s_size = nextAddr - (unsigned int)af->field_addr - af->field_size ;

		//���������m�ۂł���ꍇ
		if (free_s_size >= size) {
			char createManager = 0;

			MEMAF *newAF = getFreeSpace(&createManager) ;

			newAF->field_addr = (void *)(nextAddr - size) ;
			newAF->field_size = size ;
			newAF->next_field = af->next_field ;
			af->next_field = newAF ;

			memset(newAF->field_addr, 0, size);

			//�������Ǘ��̈悪���t�ɂȂ�A�V�����m�ۂ���K�v���o����
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

	//������������Ȃ�
	panic();
}


/*
����������֐�
m_alloc�Ŏ擾�����������������
�߂�l
0	����I��
-1	�w��A�h���X�Ɍ��B�G���[
*/
char Memory::free_s(void *addr) {

	if (!addr)
		return -1;

	if ((int)addr & (MEM_BLOCK_SIZE - 1))
		return -1 ;		//addr��擪�Ƃ��郁�����t�B�[���h�͑��݂��Ȃ�

	_using.Lock() ;
	char val=_free_s(addr);
	_using.Free() ;

	return val;
}

char Memory::_free_s(void *addr) {

	/*
	�A���S���Y��
	�E�V�[�N���āAaddr�ɑΉ����郁�����t�B�[���h��T��
	�E������΂���������A��ԍŌ�̃������t�B�[���h��������Ɏ����Ă���i�������t�B�[���h�̒f�Љ���h���A�������������I�Ɏg�����߁j
	�Ereturn
	*/

	MEMAF *lastAF = &last_manage_area->data()[last_manage_area->data_count - 1];	//��������̍Ō��MEMAF

	MEMAF *target = 0;			//����Ώۂ�MEMAF�̃��X�g��ł̈�O��MEMAF
	MEMAF *bLast = 0;			//��������̍Ō��MEMAF�̃��X�g��ł̈�O��MEMAF

	for ( MEMAF *af = first_alloc_field ; af->next_field != 0 ; af = af->next_field ) {
		if (af->next_field->field_addr == addr){	//�����Ώۂ𔭌�
			target = af;
		}

		if(af->next_field == lastAF){					//�Ō�̈�O��MEMAF�𔭌�
			bLast = af;
		}

		if(target && bLast) break;
	}

	if(!target || !bLast)
		return -1;

	MEMAF *delAF = target->next_field;
	target->next_field = delAF->next_field;

	if(delAF == lastAF){
		//����Ώۂ����`���X�g��ň�ԍŌゾ�����ꍇ
		memset(delAF, 0, sizeof(MEMAF));
	}else{
		//�����łȂ��ꍇ�́A��������ōŌ��MEMAF���󂢂��̈�Ɉړ�
		memcpy(delAF, lastAF, sizeof(MEMAF));
		memset(lastAF, 0, sizeof(MEMAF));
		bLast->next_field = delAF;
	}

	last_manage_area->data_count--;

	if(last_manage_area->data_count == 0){
		//�����������Ǘ��̈悪����ɂȂ����ꍇ�͉������
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
	mem_max = (4096-4-field_size)/field_size ;	//1�N���X�^���̏��������̐�
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
