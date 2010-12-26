//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================
const unsigned int SYSTEM_MEMORY_SIZE = 0x400000 ;				//�V�X�e���������̑傫��
const unsigned int MEM_BLOCK_SIZE = 0x1000 ;					//��������u���b�N�̃T�C�Y
const unsigned int MEM_BLOCK_MASK = ~(MEM_BLOCK_SIZE - 1) ;		//�������[�u���b�N�̃T�C�Y�P�ʂŐ؂�̂Ă邽�߂̃}�X�N

//�������̓��I�m�ۂɂ�new,malloc_s��K���g�����ƁI
class Memory {
	//Memory Allocation Field
	struct MEMAF {
		unsigned int field_size ;
		void *field_addr ;
		struct MEMAF *next_field ;
	};
	//next_field��0�������Ă���ꍇ�́A���̃t�B�[���h���Ō�ł���
	//field_addr��0�������Ă���ꍇ�́A���̃t�B�[���h�͑��݂��Ȃ�
	
	//�������Ǘ��̈�̍\��
	class MemManager{
	public:
		int data_count;										//���ݎg�p����Ă���\���̂̐�
		MemManager *before_area;							//��O�̊Ǘ��̈�ւ̃|�C���^
		MemManager *next_area;								//���̊Ǘ��̈�ւ̃|�C���^	
		inline MEMAF* data(){				//sizeof(MemManager)���K�v�Ȃ̂ŃN���X��`�̊O�Ɋ֐���`�������B
			return (MEMAF*)(((unsigned int)this) + sizeof(MemManager));
		}
		//����MemManager���Ǘ�����ŏ���MEMAF�\���̂̃|�C���^��Ԃ�
	};
	
	static const int MANAGER_DATA_NUM = (MEM_BLOCK_SIZE - sizeof(MemManager)) / sizeof(MEMAF);
	//��̃������Ǘ��̈悪����MEMAF�\���̂̐�(sizeof(MemManager��12�ƒu�������Ă͐�΃_���I�R���p�C���̋����ɂ���Ēl�͕ϓ�����))
		
	unsigned int total_size ;								//�R���s���[�^��̑��������T�C�Y
	MEMAF *first_alloc_field ;								//�ŏ��̂l�d�l�`�e�\����
	MemManager *last_manage_area;							//�Ō�̃������Ǘ��̈�ւ̃|�C���^�B��{�I�ɂ͂��̎��̊Ǘ��̈�܂ō쐬��
	
	MEMAF *getFreeSpace(char *createManager);				//�ŏ��̋󂫗̈���擾
															//createManager�ɂ͊Ǘ��̈�̍č\�z�t���O���������

	static bool alreadyExist;								//2�ȏ�쐬���邱�Ƃ�h�����߂̃t���O

	//�Փˉ��p
	AvoidDeadLock _using ;
	void *_alloc(unsigned int size) ;						//�������m��
	char _free_s(void *addr) ;								//���������

public:
	Memory(void) ;											//�������[�}�l�[�W�����g�V�X�e��������
	unsigned int FuncGetUnusing(void) const;						//�g�p���Ă��Ȃ��������̍��v���Z�o
	unsigned int FuncGetTotal(void) const;
	void *alloc(unsigned int size) ;						//�������m��
	char free_s(void *addr) ;								//���������
}; 

//main.cpp
extern Memory *mem;

//���܂�傫���Ȃ��imalloc_s�Ŏ擾����Ɩ��ʂ��o��悤�Ȃ��́j���������擾���邽�߂̕⏕�N���X
class SMALL_MEMORY {
	char flag ;											//���N���X�L���t���O�i1:�L�� 0:�����j
	unsigned int mem_max, field_size ;
	void *first_s_field ;
	void *craster_realloc(void) ;
public:
	SMALL_MEMORY(unsigned int size) ;
	~SMALL_MEMORY(void) ;
	void *alloc(void) ;
	void free_s(void *addr) ;
};

