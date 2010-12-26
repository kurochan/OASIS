//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/*　コマンド関係 */

#include "kernel.h"
#include <stdio.h>


void CONSOLE::system(void){
	char s[40] ;
	_newline();
	_typeString("Memory Information");
	_newline();
	//	sprintf(s, "total %dMB", mem->FuncGetTotal() / (1024 * 1024));
	sprintf(s, " total %dKB", mem->FuncGetTotal() / 1024);
	_typeString(s);
	_newline();
	sprintf(s, " free %dKB", mem->FuncGetUnusing() / 1024);
	_typeString(s);
	_newline();
	return ;
}

void CONSOLE::cls(void){
	boxf(_window, 0, 0, _window->xsize, _window->ysize, 0xFFFFFF) ;
	windowctl.all_refresh(_window);
	
	_charOfCorner = _charOfCurrent ;
	_charOfCurrent -= _bufferX ;
	return ;
}

//Original Code was written by HigePon
void CONSOLE::dir(void){

	char s[60];
	
	/* 初期化 */
	IDEDriver* driver = new IDEDriver();
	IDEDriver_IDEDriver(driver, IDE_IRQ_PRIMARY, IDE_IRQ_SECONDARY);
	
	/* ATAPI デバイスの検索 */
	int controller, deviceNo;
	if ( !IDEDriver_findDevice(driver, DEVICE_ATAPI, 0x05, &controller, &deviceNo) ) {
		return;
	}
	
	/* IRQ の取得 */
	//byte irq = (controller == PRIMARY) ? IDE_IRQ_PRIMARY : IDE_IRQ_SECONDARY;
	
	/* ATAPI デバイスの使用開始 */
	if ( !IDEDriver_selectDevice(driver, controller, deviceNo) ) {
		return;
	}
	
	/* 基本ボリューム記述子の読み込み */
	PrimaryVolumeDescriptor* pvd = new PrimaryVolumeDescriptor();
	IDEDriver_read(driver, 16, pvd, SECTOR_SIZE); /* 16セクターまではブート領域なので */
	if (pvd->type[0] != ISO_PRIMARY_VOLUME_DESCRIPTOR || strncmp("CD001", pvd->id, 5) != 0) {
		return;
	}
	
	/* ルートディレクトリを読み込む */
	DirectoryEntry* root = (DirectoryEntry *)( pvd->root_directory_record );
	dword size = ((dword)((root->size_l + SECTOR_SIZE - 1) / SECTOR_SIZE)) * SECTOR_SIZE;
	byte* buffer = new byte[size];
	IDEDriver_read(driver, root->extent_l, buffer, size);
	dword position;
	for (position = 0; position < size;) {
		DirectoryEntry* dir = (DirectoryEntry *) (buffer + position);
		if (dir->size_l == 0) {
			break;
		} else if (dir->directory == 0) {
			dir->name[dir->name_len - 2] = '＼0';
			sprintf(s, "%8d  %s＼n", dir->size_l, dir->name);
			_typeString(s);
/*#if 0
			if (strncmp(dir->name, "HOGEHOGE.HRB", 12) == 0) {
				byte* data = (byte *) memman_alloc_4k(memman, dir->size_l);
				IDEDriver_read(driver, dir->extent_l, data, dir->size_l);
				return 0;
			}
#endif*/
		}
		position += dir->length;
	}
	delete[] buffer;
	delete driver;

	_newline();
	return;
}
