//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================
const unsigned int SYSTEM_MEMORY_SIZE = 0x400000 ;				//システムメモリの大きさ
const unsigned int MEM_BLOCK_SIZE = 0x1000 ;					//メモリ一ブロックのサイズ
const unsigned int MEM_BLOCK_MASK = ~(MEM_BLOCK_SIZE - 1) ;		//メモリーブロックのサイズ単位で切り捨てるためのマスク

//メモリの動的確保にはnew,malloc_sを必ず使うこと！
class Memory {
	//Memory Allocation Field
	struct MEMAF {
		unsigned int field_size ;
		void *field_addr ;
		struct MEMAF *next_field ;
	};
	//next_fieldに0が入っている場合は、そのフィールドが最後である
	//field_addrに0が入っている場合は、そのフィールドは存在しない
	
	//メモリ管理領域の構造
	class MemManager{
	public:
		int data_count;										//現在使用されている構造体の数
		MemManager *before_area;							//一つ前の管理領域へのポインタ
		MemManager *next_area;								//一つ後の管理領域へのポインタ	
		inline MEMAF* data(){				//sizeof(MemManager)が必要なのでクラス定義の外に関数定義を書く。
			return (MEMAF*)(((unsigned int)this) + sizeof(MemManager));
		}
		//このMemManagerが管理する最初のMEMAF構造体のポインタを返す
	};
	
	static const int MANAGER_DATA_NUM = (MEM_BLOCK_SIZE - sizeof(MemManager)) / sizeof(MEMAF);
	//一つのメモリ管理領域が持つMEMAF構造体の数(sizeof(MemManagerを12と置き換えては絶対ダメ！コンパイラの挙動によって値は変動する))
		
	unsigned int total_size ;								//コンピュータ上の総メモリサイズ
	MEMAF *first_alloc_field ;								//最初のＭＥＭＡＦ構造体
	MemManager *last_manage_area;							//最後のメモリ管理領域へのポインタ。基本的にはこの次の管理領域まで作成済
	
	MEMAF *getFreeSpace(char *createManager);				//最初の空き領域を取得
															//createManagerには管理領域の再構築フラグが入れられる

	static bool alreadyExist;								//2つ以上作成することを防ぐためのフラグ

	//衝突回避用
	AvoidDeadLock _using ;
	void *_alloc(unsigned int size) ;						//メモリ確保
	char _free_s(void *addr) ;								//メモリ解放

public:
	Memory(void) ;											//メモリーマネージメントシステム初期化
	unsigned int FuncGetUnusing(void) const;						//使用していないメモリの合計を算出
	unsigned int FuncGetTotal(void) const;
	void *alloc(unsigned int size) ;						//メモリ確保
	char free_s(void *addr) ;								//メモリ解放
}; 

//main.cpp
extern Memory *mem;

//あまり大きくない（malloc_sで取得すると無駄が出るようなもの）メモリを取得するための補助クラス
class SMALL_MEMORY {
	char flag ;											//小クラス有効フラグ（1:有効 0:無効）
	unsigned int mem_max, field_size ;
	void *first_s_field ;
	void *craster_realloc(void) ;
public:
	SMALL_MEMORY(unsigned int size) ;
	~SMALL_MEMORY(void) ;
	void *alloc(void) ;
	void free_s(void *addr) ;
};

