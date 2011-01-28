OBJS_KERNEL = system/asmfunc.o system/asmfunc_system.o system/asmfunc_int.o \
			\
			system/main.o system/dsctbl.o system/resource.o system/int.o system/HSV2RGB.o \
			\
			system/memory/memory.o system/memory/new.o system/memory/asm_memory.o \
			\
			system/graphic/sheet.o system/graphic/window.o system/graphic/graphic.o \
			system/graphic/aqua.o system/graphic/d3module.o \
			\
			system/fifo/fifo.o \
			\
			system/timer/timer.o \
			\
			system/taskctl/main.o \
			system/taskctl/io.o \
			\
			system/task/main_task/main_task.o system/task/main_task/console.o system/task/main_task/command.o \
			system/task/timer_task.o system/task/idle_task.o \
			system/task/aqua_task.o system/task/sample_03_basic.o \
			\
			system/inputdev/inputdev.o \
			\
			system/ssememory/ssememcpy.o \
			system/ssememory/asmfunc_0.o \
			system/ssememory/asmfunc_1.o \
			system/ssememory/asmfunc_2.o \
			system/ssememory/asmfunc_3.o \
			system/ssememory/asmfunc_4.o \
			system/ssememory/asmfunc_5.o \
			system/ssememory/asmfunc_6.o \
			system/ssememory/asmfunc_7.o \
			system/ssememory/asmfunc_8.o \
			system/ssememory/asmfunc_9.o \
			system/ssememory/asmfunc_10.o \
			system/ssememory/asmfunc_11.o \
			system/ssememory/asmfunc_12.o \
			system/ssememory/asmfunc_13.o \
			system/ssememory/asmfunc_14.o \
			system/ssememory/asmfunc_15.o \
			\
			system/obj/pointer.o system/obj/hankaku.o


TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/oasis/
GCCPATH	= ../gcc/bin/

MAKE     = make -r
NASM     = nasm
# --prefix _
GCC			= $(GCCPATH)i386-elf-gcc -O2 -Wall -Wextra -m32 -mtune=prescott -fno-exceptions -fno-rtti -I ./system -c -save-temps
# -fleading-underscore -save-temps
#CPP      = $(TOOLPATH)c++ -I$(INCPATH) -O2 -Wall -Wextra -m32 -mtune=prescott -nostdlib -fno-builtin -fno-exceptions -fno-rtti -fleading-underscore -B$(TOOLPATH) -c -save-temps
LINK		= $(GCCPATH)i386-elf-g++ -O2 -Wall -Wextra -m32 -mtune=prescott
OBJCOPY		= $(GCCPATH)i386-elf-objcopy
MAKEFONT = $(TOOLPATH)makefont
BIN2OBJ  = $(TOOLPATH)bin2obj
EDIMG    = $(TOOLPATH)edimg
BOCHS    = $(TOOLPATH)Bochs/bochs
MKISOFS  = mkisofs
DEL      = @rm -f
#NASK     = $(TOOLPATH)nask
#GAS2NASK = $(TOOLPATH)gas2nask -a
#OBJ2BIM  = $(TOOLPATH)obj2bim
#BIM2HRB  = $(TOOLPATH)bim2hrb
#RULEFILE = $(INCPATH)oasis.rul
#IMGTOL   = $(TOOLPATH)imgtol.com
#GOLIB    = $(TOOLPATH)golib00 
#COPY     = cp


ifeq ($(OS),Windows_NT)
# for Windows
NASM     = $(TOOLPATH)nasm
MKISOFS  = $(TOOLPATH)mkisofs
#DEL      = del /s
#COPY     = xcopy
else
UNAME = ${shell uname}
endif

ifeq ($(UNAME),Linux)
# for Linux
endif

ifeq ($(UNAME),Darwin)
# for MacOSX
endif




# デフォルト動作
default :
	$(MAKE) iso


# ファイル生成規則

bootsec.bin : bootsec.asm Makefile
	$(NASM) bootsec.asm -fbin -l bootsec.lst -o bootsec.bin

asmhead.bin : asmhead.asm Makefile
	$(NASM) asmhead.asm -fbin -l asmhead.lst -o asmhead.bin

#kernel.bim : $(OBJS_KERNEL) Makefile
#	$(OBJ2BIM) @$(RULEFILE) out:kernel.bim stack:3136k map:kernel.map $(OBJS_KERNEL)
# 3MB+64KB=3136KB

#kernel.hrb : kernel.bim Makefile
#	$(BIM2HRB) kernel.bim kernel.hrb 0

kernel.hrb : system/gcc/crt0.o system/gcc/syscalls.o $(OBJS_KERNEL) Makefile
	$(LINK) -o kernel.hrb -nostartfiles -Wl,-Map,kernel.map,-Toasis.ld \
		system/gcc/crt0.o $(OBJS_KERNEL)  system/gcc/syscalls.o
	$(OBJCOPY) -I elf32-i386 -O binary kernel.hrb kernel.hrb

oasis.sys : asmhead.bin kernel.hrb Makefile
	cat asmhead.bin kernel.hrb >oasis.sys
#	copy /B asmhead.bin+kernel.hrb oasis.sys

system/gcc/syscalls.o : system/gcc/syscalls.c Makefile
	$(GCC) -o system/gcc/syscalls.o system/gcc/syscalls.c

system/obj/pointer.o : system/obj/pointer.bmp Makefile
	$(BIN2OBJ) system/obj/pointer.bmp system/obj/pointer.o pointer

system/obj/hankaku.bin : system/obj/hankaku.txt Makefile
	$(MAKEFONT) system/obj/hankaku.txt system/obj/hankaku.bin

system/obj/hankaku.o : system/obj/hankaku.bin Makefile
	$(BIN2OBJ) system/obj/hankaku.bin system/obj/hankaku.o hankaku

oasis.img : bootsec.bin oasis.sys Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:bootsec.bin len:512 from:0 to:0 \
		copy from:oasis.sys to:@: \
		imgout:oasis.img

oasis.iso : oasis.img Makefile
	$(MKISOFS) -v -iso-level 1 -b oasis.img -o oasis.iso .

# 一般規則

#%.gas : %.c Makefile
#	$(CC1) -o $*.gas $*.c

#%.nas : %.gas Makefile
#	$(GAS2NASK) $*.gas $*.nas

#%.o : %.nas Makefile
#	$(NASK) $*.nas $*.o $*.lst

%.o : %.s Makefile
	$(GCC) -o $*.o $*.s

%.o : %.asm Makefile
	$(NASM) $*.asm -fcoff -l $*.lst -o $*.o

%.o : %.cpp Makefile
	$(GCC) -o $*.o $*.cpp


# コマンド

# コマンド

img :
	$(MAKE) oasis.img

iso :
	-$(DEL) oasis.iso
	$(MAKE) oasis.iso 

runHDD :
	$(MAKE) img
	$(MAKE) -C $(TOOLPATH)qemu hdd

runCD :
	$(MAKE) iso
	$(MAKE) -C $(TOOLPATH)qemu cd
#	$(COPY) "oasis.img" "$(TOOLPATH)bochs/oasis.img" /D /C /F /R /Y
#	$(MAKE) -C $(TOOLPATH)Bochs

q_run :
	$(MAKE) -C $(TOOLPATH)qemu q_run

clean :
	-$(DEL) oasis.iso
	-$(DEL) oasis.img
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.tmp
	-$(DEL) *.o
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) *.hrb
	-$(DEL) *.sys
	-$(DEL) *.ii
	-$(DEL) *.i
	-$(DEL) *.s
	$(MAKE) -C ./system clean


