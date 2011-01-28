//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================

#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

#define PORT_KEYDAT		0x0060


#if (defined(__cplusplus))
extern "C" {
#endif

//割り込み
void inthandler07(int *esp);												// FPU使用時に発生
void inthandler20(int *esp);												// 
void inthandler21(int *esp) ;												// 
void inthandler2c(int *esp) ;												// 
void inthandler27(int *esp) ;												// 
void inthandler40(int *esp) ;												// 
void inthandler41(int *esp) ;												// 

#if (defined(__cplusplus))
}
#endif
