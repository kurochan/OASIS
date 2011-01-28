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
void asm_inthandler40(void);
void asm_inthandler41(void);

//== asmfunc_system.nas ==//

void io_stihlt(void) ;							// STIした直後にHLTをする関数
void io_hlt(void) ;
void io_sti(void) ;
void io_cli(void) ;
int io_in8(int port);
void io_out8(int port, int data);
int io_in16(int port);
void io_out16(int port, int data);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int io_load_eflags(void);
void io_store_eflags(int eflags);
int load_cr0(void);
void store_cr0(int cr0);
void load_tr(int tr);
// タスク切り替え用
void farjmp(int eip, int cs);
void farcall(int eip, int cs);
void func_retf(void);

// asmfunc_memory.nas
UINT memcloot(void);			// メモリ総容量チェック用サブルーチン

//== asmfunc_sse.nas ==//

int asm_ssememcpy0( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy1( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy2( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy3( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy4( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy5( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy6( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy7( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy8( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy9( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy10( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy11( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy12( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy13( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy14( UINT _dst, UINT _src, UINT _size );
int asm_ssememcpy15( UINT _dst, UINT _src, UINT _size );


#if (defined(__cplusplus))
}
#endif
