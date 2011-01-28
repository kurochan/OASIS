//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================


class SHTCTL{

	/* シートマッピングアロケーションテーブル */
	struct SMAT {
		struct SHEET *sheet ;
		short x1, y1, x2, y2 ;
		char flag ;
	} ;

	struct SHEET *top, *bottom, *back ;
	struct SHEET *g_sheet(short vx, short vy, short xsize, short ysize) ;
	struct WINDOW *g_winsheet(short vx, short vy, short xsize, short ysize) ;
	void _del(SHEET *sheet);
	void _hide(SHEET *sheet);
	char divide_smat(struct SMAT *smat1, SMAT *osmat1, SMAT *smat2) const ;
	char cmp(short *lx1, short x1, short vx1, short *ly1, short y1, short vy1, short *lx2, short x2, short vx2, short *ly2, short y2, short vy2) const ;
	void set_top_m(SHEET *sheet, SHEET *c_top);

	AvoidDeadLock _using ;						//衝突回避用フラグ

	static bool alreadyExist;

public:
	struct SHEET *init(short vx, short vy, short xsize, short ysize);
	struct SHEET *create(short vx, short vy, short xsize, short ysize) ;
	struct WINDOW *wincreate(short vx, short vy, short xsize, short ysize) ;
	void del(SHEET *sheet);
	void hide(SHEET *sheet);
	void set_top(SHEET *sheet);
	void set_wintop(SHEET *sheet, SHEET *active);
	void refresh(SHEET *t_sheet, short vx, short vy, short xsize, short ysize) ;
	void all_refresh(SHEET *t_sheet) ;
} extern shtctl ;



struct SHEET{
	short vx, vy ;
	USHORT xsize, ysize;
	struct SHEET *next_sheet ; //next_sheetには一つ上のシートへのポインタが入る
	char flag ;		// ビット情報：<透明><表示>
	int *buffer ;
};

