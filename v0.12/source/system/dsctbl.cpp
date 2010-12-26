//=========================================
//         OASIS descriptor table code
//                Copyright (C) 2010 soS.A
//=========================================

/* GDTやIDT関係 */

#include "kernel.h"
#include <stdio.h>

#define LIMIT_IDT		0x000007ff			// 登録できるIDTの数のMAX値
#define LIMIT_GDT		0x0000ffff			// 登録できるGDTの数のMAX値
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e

struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
} *gdt;

struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
} *idt;

void set_gatedesc(int i, int offset, int selector, int ar);

extern char hankaku[4096];

void mes1_s( int x, int y, char *font) {
	int i,*p;
	char d, q ;
	for (i = 0; i < 16; i++) {
		p = (int *)(sinfo->vram + (y + i) * sinfo->scrnx + x);
		d = font[i];
		for (q=0;q<8;q++) {
			if ((d & (1<<(7-q))) != 0) {
				p[q] = 0;
			}
		}
	}
	return;
}

//カーネルパニック時専用の特殊文字描画ルーチン
void mes_s(int x, int y, char *s) {
	int _x=x, _y=y;
	for (; *s != NULL; s++) {
		// if (strncmp(s,"\n",1)) {
			// _y+=16;
			// _x=x;
		// }else{
			mes1_s( _x, _y, hankaku + *s * 16);
			_x += 8;
		// }
	}
	return;
}

//カーネル内部での矛盾発生時に呼び出すパニック関数
void panic(void){
	io_cli();
	for(int x=0; x<sinfo->scrnx*sinfo->scrnx; x+=3)
		sinfo->vram[x]=0xff;

	for(;;)
		asm("hlt");
}

//例外発生時に呼び出すパニック関数（CSもEIPも違う気がする。インテルの資料の読み間違い？）
void panic_CPU(int CS, int EIP){
	io_cli();
	for(int x=0; x<sinfo->scrnx*sinfo->scrnx; x+=3)
		sinfo->vram[x]=0xff;

	char x[70];
	sprintf(x,"EIP:0x%X",EIP);
	mes_s(10,10,x);
	sprintf(x,"CS:0x%X",CS);
	mes_s(10,30,x);
	sprintf(x,"Warning: This value will not help your debug. It must be wrong.");
	mes_s(10,50,x);

	for(;;)
		asm("hlt");
}

void init_gdtidt(void)
{
	gdt=new SEGMENT_DESCRIPTOR[LIMIT_GDT / 8];

	set_segmdesc(1, 0xffffffff,   0x00000000, AR_DATA32_RW);
	set_segmdesc(2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);
	load_gdtr(LIMIT_GDT, (int)gdt);

	idt=new GATE_DESCRIPTOR[LIMIT_IDT / 8];

	load_idtr(LIMIT_IDT, (int)idt);

	for (char cnt=0; cnt<0x07; cnt++)
		set_gatedesc(cnt, (int) panic_CPU, 2 * 8, AR_INTGATE32);
	set_gatedesc(0x07, (int) asm_inthandler07, 2 * 8, AR_INTGATE32);
	for (char cnt=0x08; cnt<0x20; cnt++)
		set_gatedesc(cnt, (int) panic_CPU, 2 * 8, AR_INTGATE32);
	set_gatedesc(0x20, (int) asm_inthandler20, 2 * 8, AR_INTGATE32);
	set_gatedesc(0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);
	set_gatedesc(0x27, (int) asm_inthandler27, 2 * 8, AR_INTGATE32);
	set_gatedesc(0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);

	delete gdt ;

	return;
}

void set_segmdesc(short i, unsigned int limit, int base, int ar)
{
	//セグメントは8192個しか登録できないので、それ以上の数を指定しようとすると、エラーになる
	if (i > LIMIT_GDT / 8)
		for(;;)
			io_hlt();		//@例外処理を記述する必要あり！

	if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	(gdt+i)->limit_low    = limit & 0xffff;
	(gdt+i)->base_low     = base & 0xffff;
	(gdt+i)->base_mid     = (base >> 16) & 0xff;
	(gdt+i)->access_right = ar & 0xff;
	(gdt+i)->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	(gdt+i)->base_high    = (base >> 24) & 0xff;
	return;
}

void set_gatedesc(int i, int offset, int selector, int ar)
{
	if (i>LIMIT_IDT / 8)
		for(;;)
			io_hlt();		//@例外処理を記述する必要あり！
	(idt+i)->offset_low   = offset & 0xffff;
	(idt+i)->selector     = selector;
	(idt+i)->dw_count     = (ar >> 8) & 0xff;
	(idt+i)->access_right = ar & 0xff;
	(idt+i)->offset_high  = (offset >> 16) & 0xffff;
	return;
}
