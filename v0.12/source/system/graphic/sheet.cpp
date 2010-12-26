//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/*シート関係*/

#include "kernel.h"	

SHTCTL shtctl ;

/*
シートシステム初期化関数
シートを一枚作って、シートシステムを確立します。
シート関係の関数を使う場合は最初にこの関数を必ず呼び出すこと。
引数、戻り値：create_sheet関数と同じ
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
シート作成関数
戻り値：シート構造体へのポインタ
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
シート作成関数
create_sheet関数とsheet_init関数の共通部を関数化したもの
*/
struct SHEET *SHTCTL::g_sheet(short vx, short vy, short xsize, short ysize){

	SHEET *sheet=(SHEET *)malloc_s(xsize*ysize*4+sizeof(SHEET)) ;

	//引数代入
	sheet->vx=vx ;
	sheet->vy=vy ;
	sheet->xsize=xsize ;
	sheet->ysize=ysize ;

	//フラグセット、透過情報なし、表示
	sheet->flag=1 ;

	sheet->buffer=(int *)sheet+sizeof(SHEET) ;

	return sheet ;
}

/*
ウィンドウシート作成関数
戻り値：シート構造体へのポインタ
*/
//これ、マウスの次にウィンドウが来ちゃうから、修正する必要がある・・・
struct WINDOW *SHTCTL::wincreate(short vx, short vy, short xsize, short ysize) {

	_using.Lock();

	WINDOW *window=g_winsheet(vx, vy, xsize, ysize) ;

	top->next_sheet=&(window->this_s);
	top=&(window->this_s) ;

	_using.Free();
	return window ;
}


/*
ウィンドウシート作成関数
*/
struct WINDOW *SHTCTL::g_winsheet(short vx, short vy, short xsize, short ysize){

	WINDOW *window=(WINDOW *)malloc_s(xsize*ysize*4+sizeof(WINDOW)) ;
	window->this_s_p=&(window->this_s) ;

	//引数代入
	window->this_s.vx=vx ;
	window->this_s.vy=vy ;
	window->this_s.xsize=xsize ;
	window->this_s.ysize=ysize ;

	//フラグセット、透過情報なし、表示
	window->this_s.flag=1 ;

	window->this_s.buffer=(int *)window->this_s_p+sizeof(SHEET) ;

	return window ;
}


/*
シート破棄関数
引数
対象シートへのポインタ
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
				return ;				//無効なシート指定
		}

		c_sheet->next_sheet=sheet->next_sheet ;
		if (sheet==top)
			top=c_sheet ;
	}

	return ;
}

/*
シート非表示化関数
実際には非表示にするのではなく、背景シートの後ろへ持ってくる
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
シート最前面化関数
戻り値：なし
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
シート最前面化関数(ただし、マウス、タスクバーなどよりは後ろ)
主にウィンドウの最前面化用
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
set_top, set_wintop関数から呼び出す実際の処理ルーチン
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
二つの四角形の重なってる部分の値をもとめる関数
全く重なっていない場合は値がおかしくなるため、チェックし、戻り値を1にする
lx1,ly1:x1とvx1のうちの大きい方の値が代入される
lx2,ly2:x2とvx2のうちの小さい方の値が代入される
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

	//シートが重なっていない場合を弾く。
	if (*lx1>*lx2 || *ly1>*ly2)
		return 1 ;

	return 0 ;
}

//シートを描画
void SHTCTL::all_refresh(SHEET *t_sheet) {
	refresh(t_sheet, t_sheet->vx, t_sheet->vy, t_sheet->xsize, t_sheet->ysize) ;
	return ;
}

//スクリーン上の任意の短径を再描画する関数(絶対座標で指定)
//t_sheetより上のシートは描画されない（t_sheetは含める）
void SHTCTL::refresh(SHEET *t_sheet, short vx, short vy, short xsize, short ysize) {


	int nt_sheet_num=0, cnt1, cnt2, num_smat=0, o_num_smat ;
	short lx1, ly1, lx2, ly2, y ;
	char flag=0, num_r ;

	//指定箇所がＶＲＡＭ外の場合は補正
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
		return ;	//値が不正だった時以外にも、範囲すべてがVRAM外に存在する場合はここに来る事もアリ


	_using.Lock();
	SHEET *sheet=back ;

	//透明度を持たず、非表示でもないシートの数を調べる
	for(;;) {
		if (sheet->flag==1)
			nt_sheet_num++ ;

		if (sheet==top)
			break ;

		sheet=sheet->next_sheet ;
	}

	//smat構造体のメモリ確保
	SMAT *alloc_smat=new SMAT [nt_sheet_num*(nt_sheet_num+1)-1] ;
	SMAT *smat1=alloc_smat, *b_smat=alloc_smat, *smat2, *osmat1 ;
	
	sheet=back ;

	//シートマップテーブルを作成
	for(cnt1=0; cnt1<nt_sheet_num; cnt1++) {

		if (sheet->flag!=1) {
			sheet=sheet->next_sheet ;
			cnt1-- ;
			continue ;	//透過属性あり、もしくは非表示
		}

		if (cmp(&lx1,sheet->vx,vx,
					&ly1,sheet->vy,vy,
					&lx2,sheet->vx+sheet->xsize-1,vx+xsize-1,
					&ly2,sheet->vy+sheet->ysize-1,vy+ysize-1)
					==1) {
			sheet=sheet->next_sheet ;
			continue ;		//重なりがなかった
		}

		//現在のシートのマップを作る
		smat1->sheet=sheet ;
		smat1->x1=lx1 ;
		smat1->y1=ly1 ;
		smat1->x2=lx2 ;
		smat1->y2=ly2 ;
		smat1->flag=flag ;

		if (sheet==t_sheet)
			flag=1 ;

		num_smat++ ;

		//現在のシート(osmat1)とそれまで作ったすべてのシート(smat2)との当たり判定をする
		osmat1=smat1 ;
		smat2=b_smat ;
		o_num_smat=num_smat ;

		smat1=&smat1[1] ;

		for(cnt2=0; cnt2<o_num_smat-1; cnt2++) {

			if (smat2->sheet==(SHEET *)0) {
				smat2=&smat2[1];
				continue ;		//無効なマップデータ
			}
			num_r=divide_smat(smat1, osmat1, smat2) ;
			smat1=&smat1[num_r] ;		//シート分割
			num_smat+=num_r ;
			smat2=&smat2[1] ;
		}

		sheet=sheet->next_sheet ;
	}

	_using.Free();

	//シートマップテーブルを参考に、実描画
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
		return 0 ;		//重なりがなかった

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

