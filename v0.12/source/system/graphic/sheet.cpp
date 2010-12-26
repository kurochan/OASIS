//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/*�V�[�g�֌W*/

#include "kernel.h"	

SHTCTL shtctl ;

/*
�V�[�g�V�X�e���������֐�
�V�[�g���ꖇ����āA�V�[�g�V�X�e�����m�����܂��B
�V�[�g�֌W�̊֐����g���ꍇ�͍ŏ��ɂ��̊֐���K���Ăяo�����ƁB
�����A�߂�l�Fcreate_sheet�֐��Ɠ���
*/
struct SHEET *SHTCTL::init(short vx, short vy, short xsize, short ysize) {
	SHEET *sheet ;

	sheet=g_sheet(vx, vy, xsize, ysize) ;

	top=sheet ;
	back=sheet ;
	bottom=sheet ;

	return sheet ;
}


/*
�V�[�g�쐬�֐�
�߂�l�F�V�[�g�\���̂ւ̃|�C���^
*/
struct SHEET *SHTCTL::create(short vx, short vy, short xsize, short ysize) {

	_using.Lock();

	SHEET *sheet=g_sheet(vx, vy, xsize, ysize) ;

	top->next_sheet=sheet;
	top=sheet ;

	_using.Free();
	return sheet ;
}


/*
�V�[�g�쐬�֐�
create_sheet�֐���sheet_init�֐��̋��ʕ����֐�����������
*/
struct SHEET *SHTCTL::g_sheet(short vx, short vy, short xsize, short ysize){

	SHEET *sheet=(SHEET *)malloc_s(xsize*ysize*4+sizeof(SHEET)) ;

	//�������
	sheet->vx=vx ;
	sheet->vy=vy ;
	sheet->xsize=xsize ;
	sheet->ysize=ysize ;

	//�t���O�Z�b�g�A���ߏ��Ȃ��A�\��
	sheet->flag=1 ;

	sheet->buffer=(int *)sheet+sizeof(SHEET) ;

	return sheet ;
}

/*
�E�B���h�E�V�[�g�쐬�֐�
�߂�l�F�V�[�g�\���̂ւ̃|�C���^
*/
//����A�}�E�X�̎��ɃE�B���h�E�������Ⴄ����A�C������K�v������E�E�E
struct WINDOW *SHTCTL::wincreate(short vx, short vy, short xsize, short ysize) {

	_using.Lock();

	WINDOW *window=g_winsheet(vx, vy, xsize, ysize) ;

	top->next_sheet=&(window->this_s);
	top=&(window->this_s) ;

	_using.Free();
	return window ;
}


/*
�E�B���h�E�V�[�g�쐬�֐�
*/
struct WINDOW *SHTCTL::g_winsheet(short vx, short vy, short xsize, short ysize){

	WINDOW *window=(WINDOW *)malloc_s(xsize*ysize*4+sizeof(WINDOW)) ;
	window->this_s_p=&(window->this_s) ;

	//�������
	window->this_s.vx=vx ;
	window->this_s.vy=vy ;
	window->this_s.xsize=xsize ;
	window->this_s.ysize=ysize ;

	//�t���O�Z�b�g�A���ߏ��Ȃ��A�\��
	window->this_s.flag=1 ;

	window->this_s.buffer=(int *)window->this_s_p+sizeof(SHEET) ;

	return window ;
}


/*
�V�[�g�j���֐�
����
�ΏۃV�[�g�ւ̃|�C���^
*/
void SHTCTL::del(SHEET *sheet) {

	_using.Lock();

	_del(sheet) ;
	
	_using.Free();
	free_s(sheet) ;
	return ;
}

void SHTCTL::_del(SHEET *sheet) {

	if (sheet==bottom) {
		bottom=sheet->next_sheet ;
	}else{
		SHEET *c_sheet=back ;
		for(;;) {
			if (c_sheet->next_sheet==sheet)
				break ;

			c_sheet=c_sheet->next_sheet ;
			if (c_sheet==top)
				return ;				//�����ȃV�[�g�w��
		}

		c_sheet->next_sheet=sheet->next_sheet ;
		if (sheet==top)
			top=c_sheet ;
	}

	return ;
}

/*
�V�[�g��\�����֐�
���ۂɂ͔�\���ɂ���̂ł͂Ȃ��A�w�i�V�[�g�̌��֎����Ă���
*/
void SHTCTL::hide(SHEET *sheet) {

	_using.Lock();

	_hide(sheet) ;
	
	_using.Free();
	refresh(back,sheet->vx,sheet->vy,sheet->xsize,sheet->ysize) ;
	
	return ;
}

void SHTCTL::_hide(SHEET *sheet) {
	SHEET *c_sheet=back ;

	for(;;) {
		if (c_sheet->next_sheet==sheet)
			break ;

		c_sheet=c_sheet->next_sheet ;
		if (c_sheet==top)
			return ;
	}

	c_sheet=sheet->next_sheet ;
	sheet->next_sheet=back->next_sheet ;
	back->next_sheet=sheet ;
	return ;
}


/*
�V�[�g�őO�ʉ��֐�
�߂�l�F�Ȃ�
*/
void SHTCTL::set_top(SHEET *sheet) {
	if (sheet==top)
		return;

	_using.Lock();
	set_top_m(sheet, top) ;
	top=sheet ;
	_using.Free();
	
	all_refresh(sheet) ;
	return ;
}

/*
�V�[�g�őO�ʉ��֐�(�������A�}�E�X�A�^�X�N�o�[�Ȃǂ��͌��)
��ɃE�B���h�E�̍őO�ʉ��p
*/
void SHTCTL::set_wintop(SHEET *sheet, SHEET *active) {
	if (sheet==active)
		return;

	_using.Lock();
	set_top_m(sheet, active) ;
	_using.Free();

	all_refresh(sheet) ;
	return ;
}


/*
set_top, set_wintop�֐�����Ăяo�����ۂ̏������[�`��
*/
void SHTCTL::set_top_m(SHEET *sheet, SHEET *c_top) {

	if (sheet==bottom) {
		bottom=sheet->next_sheet ;
	}else{
		for ( SHEET *c_sheet=bottom; c_sheet!=c_top; c_sheet=c_sheet->next_sheet ) {
			if (c_sheet->next_sheet==sheet) {
				c_sheet->next_sheet=sheet->next_sheet ;
				break ;
			}
		}
	}

	sheet->next_sheet=c_top->next_sheet ;
	c_top->next_sheet=sheet;
	return ;
}


/*
��̎l�p�`�̏d�Ȃ��Ă镔���̒l�����Ƃ߂�֐�
�S���d�Ȃ��Ă��Ȃ��ꍇ�͒l�����������Ȃ邽�߁A�`�F�b�N���A�߂�l��1�ɂ���
lx1,ly1:x1��vx1�̂����̑傫�����̒l����������
lx2,ly2:x2��vx2�̂����̏��������̒l����������
*/
char SHTCTL::cmp(short *lx1, short x1, short vx1, short *ly1, short y1, short vy1, short *lx2, short x2, short vx2, short *ly2, short y2, short vy2) const {

	if (x1>vx1)
		*lx1=x1 ;
	else
		*lx1=vx1 ;

	if (y1>vy1)
		*ly1=y1 ;
	else
		*ly1=vy1 ;

	if (x2<vx2)
		*lx2=x2 ;
	else
		*lx2=vx2 ;

	if (y2<vy2)
		*ly2=y2 ;
	else
		*ly2=vy2 ;

	//�V�[�g���d�Ȃ��Ă��Ȃ��ꍇ��e���B
	if (*lx1>*lx2 || *ly1>*ly2)
		return 1 ;

	return 0 ;
}

//�V�[�g��`��
void SHTCTL::all_refresh(SHEET *t_sheet) {
	refresh(t_sheet, t_sheet->vx, t_sheet->vy, t_sheet->xsize, t_sheet->ysize) ;
	return ;
}

//�X�N���[����̔C�ӂ̒Z�a���ĕ`�悷��֐�(��΍��W�Ŏw��)
//t_sheet����̃V�[�g�͕`�悳��Ȃ��it_sheet�͊܂߂�j
void SHTCTL::refresh(SHEET *t_sheet, short vx, short vy, short xsize, short ysize) {


	int nt_sheet_num=0, cnt1, cnt2, num_smat=0, o_num_smat ;
	short lx1, ly1, lx2, ly2, y ;
	char flag=0, num_r ;

	//�w��ӏ����u�q�`�l�O�̏ꍇ�͕␳
	if ((vx+xsize-1)>=sinfo->scrnx)
		xsize=sinfo->scrnx-vx ;
	if ((vy+ysize-1)>=sinfo->scrny)
		ysize=sinfo->scrny-vy ;
	if (vx<0) {
		xsize+=vx;
		vx=0;
	}
	if (vy<0) {
		ysize+=vy;
		vy=0;
	}

	if (xsize<=0||ysize<=0)
		return ;	//�l���s�����������ȊO�ɂ��A�͈͂��ׂĂ�VRAM�O�ɑ��݂���ꍇ�͂����ɗ��鎖���A��


	_using.Lock();
	SHEET *sheet=back ;

	//�����x���������A��\���ł��Ȃ��V�[�g�̐��𒲂ׂ�
	for(;;) {
		if (sheet->flag==1)
			nt_sheet_num++ ;

		if (sheet==top)
			break ;

		sheet=sheet->next_sheet ;
	}

	//smat�\���̂̃������m��
	SMAT *alloc_smat=new SMAT [nt_sheet_num*(nt_sheet_num+1)-1] ;
	SMAT *smat1=alloc_smat, *b_smat=alloc_smat, *smat2, *osmat1 ;
	
	sheet=back ;

	//�V�[�g�}�b�v�e�[�u�����쐬
	for(cnt1=0; cnt1<nt_sheet_num; cnt1++) {

		if (sheet->flag!=1) {
			sheet=sheet->next_sheet ;
			cnt1-- ;
			continue ;	//���ߑ�������A�������͔�\��
		}

		if (cmp(&lx1,sheet->vx,vx,
					&ly1,sheet->vy,vy,
					&lx2,sheet->vx+sheet->xsize-1,vx+xsize-1,
					&ly2,sheet->vy+sheet->ysize-1,vy+ysize-1)
					==1) {
			sheet=sheet->next_sheet ;
			continue ;		//�d�Ȃ肪�Ȃ�����
		}

		//���݂̃V�[�g�̃}�b�v�����
		smat1->sheet=sheet ;
		smat1->x1=lx1 ;
		smat1->y1=ly1 ;
		smat1->x2=lx2 ;
		smat1->y2=ly2 ;
		smat1->flag=flag ;

		if (sheet==t_sheet)
			flag=1 ;

		num_smat++ ;

		//���݂̃V�[�g(osmat1)�Ƃ���܂ō�������ׂẴV�[�g(smat2)�Ƃ̓����蔻�������
		osmat1=smat1 ;
		smat2=b_smat ;
		o_num_smat=num_smat ;

		smat1=&smat1[1] ;

		for(cnt2=0; cnt2<o_num_smat-1; cnt2++) {

			if (smat2->sheet==(SHEET *)0) {
				smat2=&smat2[1];
				continue ;		//�����ȃ}�b�v�f�[�^
			}
			num_r=divide_smat(smat1, osmat1, smat2) ;
			smat1=&smat1[num_r] ;		//�V�[�g����
			num_smat+=num_r ;
			smat2=&smat2[1] ;
		}

		sheet=sheet->next_sheet ;
	}

	_using.Free();

	//�V�[�g�}�b�v�e�[�u�����Q�l�ɁA���`��
	smat1=b_smat ;	
	for(cnt2=0; cnt2<num_smat; cnt2++) {

		if (smat1->sheet==(SHEET *)0 || smat1->flag==1) {
			smat1=&smat1[1] ;
			continue ;
		}

		sheet=smat1->sheet ;
		int *scrnbuf=&(sinfo->vram[smat1->y1*sinfo->scrnx + smat1->x1]);
		int *sheetbuf=&(sheet->buffer[(smat1->y1 - sheet->vy)*sheet->xsize + smat1->x1 - sheet->vx]);
		int size=(smat1->x2 - smat1->x1 + 1)<<2;
		for(y=smat1->y1; y<=smat1->y2; y++) {
			ssememcpy(scrnbuf, sheetbuf , size);

			scrnbuf+=sinfo->scrnx;
			sheetbuf+=sheet->xsize;

			/*for(short x=smat1->x1; x<=smat1->x2; x++) {
				sinfo->vram[y*sinfo->scrnx+x]=sheet->buffer[(y - sheet->vy)*sheet->xsize + x - sheet->vx] ;
			}*/
		}

		smat1=&smat1[1] ;
	}

	delete [] alloc_smat ;

	return ;
}

char SHTCTL::divide_smat( SMAT *smat1, SMAT *osmat1, SMAT *smat2) const {
	short lx1, ly1, lx2, ly2 ;
	char num_r=0 ;

	if (shtctl.cmp(&lx1,osmat1->x1,smat2->x1,
					&ly1,osmat1->y1,smat2->y1,
					&lx2,osmat1->x2,smat2->x2,
					&ly2,osmat1->y2,smat2->y2)
					==1)
		return 0 ;		//�d�Ȃ肪�Ȃ�����

	if (smat2->y1!=ly1) {
		smat1->sheet=smat2->sheet ;
		smat1->x1=smat2->x1 ;
		smat1->y1=smat2->y1 ;
		smat1->x2=smat2->x2 ;
		smat1->y2=ly1-1 ;
		smat1->flag=smat2->flag ;
		smat1=&smat1[1] ;
		num_r++ ;
	}

	if (smat2->x1!=lx1) {
		smat1->sheet=smat2->sheet ;
		smat1->x1=smat2->x1 ;
		smat1->y1=ly1 ;
		smat1->x2=lx1-1 ;
		smat1->y2=ly2 ;
		smat1->flag=smat2->flag ;

		smat1=&smat1[1] ;
		num_r++ ;
	}

	if (lx2!=smat2->x2) {

		smat1->sheet=smat2->sheet ;
		smat1->x1=lx2+1 ;
		smat1->y1=ly1 ;
		smat1->x2=smat2->x2 ;
		smat1->y2=ly2 ;
		smat1->flag=smat2->flag ;

		smat1=&smat1[1] ;
		num_r++ ;
	}


	if (ly2!=smat2->y2) {
		smat2->y1=ly2+1 ;
	}else{
		smat2->sheet=(SHEET *)0 ;
	}

	return num_r;
}

