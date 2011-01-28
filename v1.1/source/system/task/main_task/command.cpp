//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/*　コマンド関係 */

#include "kernel.h"
#include <stdio.h>

void CONSOLE::help(void){
	char s[40] ;
	_newline();
	_typeString("Help");
	_newline();
	_typeString("help    Display this information");
	_newline();
	_typeString("system  Display system information");
	_newline();
	_typeString("cls     Clear this Console");
	_newline();
	return ;
}


void CONSOLE::system(void){
	char s[40] ;
	_newline();
	_typeString("Memory Information");
	_newline();
	//	sprintf(s, "total %dMB", mem->FuncGetTotal() / (1024 * 1024));
	sprintf(s, " total %dKB", mem->FuncGetTotal() / 1024);
	_typeString(s);
	_newline();
	sprintf(s, " free  %dKB", mem->FuncGetUnusing() / 1024);
	_typeString(s);
	_newline();
	return ;
}

void CONSOLE::cls(void){
	g_boxf(_window, 0, 0, _window->xsize, _window->ysize, 0xFFFFFF) ;
	windowctl.all_refresh(_window);

	_charOfCorner = _charOfCurrent ;
	_charOfCurrent -= _bufferX ;
	return ;
}

