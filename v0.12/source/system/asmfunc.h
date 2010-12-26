//=========================================
//
//                 T-OS Kernel header code
//
//=========================================
//アセンブリコード用のヘッダーファイル

#if (defined(__cplusplus))
extern "C" {
#endif

//== asmfunc.asm ==//

void asm_clts(void);
void asm_fnsave(int *addr);
void asm_frstor(int *addr);

//== asmfunc_int.nas ==//

// 割り込みハンドラにおけるアセンブリ上の処理
void asm_inthandler07(void);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);

//== asmfunc_system.nas ==//

void io_stihlt(void) ;							// STIした直後にHLTをする関数
void io_hlt(void) ;							// HLTする関数
void io_sti(void) ;							// STIする関数
void io_cli(void) ;								// CLIする関数
int io_in8(int port);							// デバイスからの信号の受信
void io_out8(int port, int data);			// デバイスへの信号の送信
int io_in16(int port);							// デバイスからの信号の受信
void io_out16(int port, int data);			// デバイスへの信号の送信
void load_gdtr(int limit, int addr);		// 
void load_idtr(int limit, int addr);
int io_load_eflags(void);
void io_store_eflags(int eflags);
int load_cr0(void);
void store_cr0(int cr0);
void load_tr(int tr);
void farjmp(int eip, int cs);				// タスク切り替えなどのためのFARJMP命令


// asmfunc_memory.nas
unsigned int memcloot(void);			// メモリ総容量チェック用サブルーチン

//== asmfunc_sse.nas ==//

int asm_ssememcpy0( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy1( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy2( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy3( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy4( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy5( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy6( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy7( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy8( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy9( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy10( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy11( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy12( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy13( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy14( unsigned int _dst, unsigned int _src, unsigned int _size );
int asm_ssememcpy15( unsigned int _dst, unsigned int _src, unsigned int _size );


#if (defined(__cplusplus))
}
#endif
