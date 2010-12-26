//=========================================
//
//                 T-OS Kernel header code
//
//=========================================
//�A�Z���u���R�[�h�p�̃w�b�_�[�t�@�C��

#if (defined(__cplusplus))
extern "C" {
#endif

//== asmfunc.asm ==//

void asm_clts(void);
void asm_fnsave(int *addr);
void asm_frstor(int *addr);

//== asmfunc_int.nas ==//

// ���荞�݃n���h���ɂ�����A�Z���u����̏���
void asm_inthandler07(void);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);

//== asmfunc_system.nas ==//

void io_stihlt(void) ;							// STI���������HLT������֐�
void io_hlt(void) ;							// HLT����֐�
void io_sti(void) ;							// STI����֐�
void io_cli(void) ;								// CLI����֐�
int io_in8(int port);							// �f�o�C�X����̐M���̎�M
void io_out8(int port, int data);			// �f�o�C�X�ւ̐M���̑��M
int io_in16(int port);							// �f�o�C�X����̐M���̎�M
void io_out16(int port, int data);			// �f�o�C�X�ւ̐M���̑��M
void load_gdtr(int limit, int addr);		// 
void load_idtr(int limit, int addr);
int io_load_eflags(void);
void io_store_eflags(int eflags);
int load_cr0(void);
void store_cr0(int cr0);
void load_tr(int tr);
void farjmp(int eip, int cs);				// �^�X�N�؂�ւ��Ȃǂ̂��߂�FARJMP����


// asmfunc_memory.nas
unsigned int memcloot(void);			// ���������e�ʃ`�F�b�N�p�T�u���[�`��

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
