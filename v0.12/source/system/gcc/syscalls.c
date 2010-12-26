/*
 *  syscall.c
 *  
 *
 *  Created by Liva on 10/11/29.
 *  Copyright 2010 OASIS Developer Team. All rights reserved.
 *
 */


/* note these headers are all provided by newlib - you don't need to provide them */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>

void panic(void);
int getpid_CPP(void);

void _exit(){
	panic();
	return ;
}

int close(int file){
	panic();
	return -1 ;
}

char **environ; /* pointer to array of char * strings that define the current environment variables */

int execve(char *name, char **argv, char **env){
	panic();
	return -1 ;
}

int fork(){
	panic();
	return -1 ;
}

int fstat(int file, struct stat *st){
	panic();
	return -1 ;
}

int getpid(){
//	return getpid_CPP() ;
	panic();
	return -1 ;
}

int isatty(int file){
	panic();
	return -1 ;
}

int kill(int pid, int sig){
	panic();
	return -1 ;
}

int link(char *old, char *_new){
	panic();
	return -1 ;
}

int lseek(int file, int ptr, int dir){
	panic();
	return -1 ;
}

int open(const char *name, int flags, ...){
	panic();
	return -1 ;
}

int read(int file, char *ptr, int len){
	panic();
	return -1 ;
}

caddr_t sbrk(int incr){
	extern char end;  /* Defined by the linker */
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0) {
		heap_end = &end;
	}
	prev_heap_end = heap_end;
	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

int stat(const char *file, struct stat *st){
		panic();
		return -1 ;
}

clock_t times(struct tms *buf){
	panic();
	return -1 ;
}

int unlink(char *name){
	panic();
	return -1 ;
}

int wait(int *status){
	panic();
	return -1 ;
}

int write(int file, char *ptr, int len){
	panic();
	return -1 ;
}

int settimeofday(const struct timeval *p, const struct timezone *z){
	panic();
	return -1 ;
}

