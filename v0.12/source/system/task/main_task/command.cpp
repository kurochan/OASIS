//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/*�@�R�}���h�֌W */

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
	
	/* ������ */
	IDEDriver* driver = new IDEDriver();
	IDEDriver_IDEDriver(driver, IDE_IRQ_PRIMARY, IDE_IRQ_SECONDARY);
	
	/* ATAPI �f�o�C�X�̌��� */
	int controller, deviceNo;
	if ( !IDEDriver_findDevice(driver, DEVICE_ATAPI, 0x05, &controller, &deviceNo) ) {
		return;
	}
	
	/* IRQ �̎擾 */
	//byte irq = (controller == PRIMARY) ? IDE_IRQ_PRIMARY : IDE_IRQ_SECONDARY;
	
	/* ATAPI �f�o�C�X�̎g�p�J�n */
	if ( !IDEDriver_selectDevice(driver, controller, deviceNo) ) {
		return;
	}
	
	/* ��{�{�����[���L�q�q�̓ǂݍ��� */
	PrimaryVolumeDescriptor* pvd = new PrimaryVolumeDescriptor();
	IDEDriver_read(driver, 16, pvd, SECTOR_SIZE); /* 16�Z�N�^�[�܂ł̓u�[�g�̈�Ȃ̂� */
	if (pvd->type[0] != ISO_PRIMARY_VOLUME_DESCRIPTOR || strncmp("CD001", pvd->id, 5) != 0) {
		return;
	}
	
	/* ���[�g�f�B���N�g����ǂݍ��� */
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
			dir->name[dir->name_len - 2] = '�_0';
			sprintf(s, "%8d  %s�_n", dir->size_l, dir->name);
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
