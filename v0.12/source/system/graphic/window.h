//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================

struct WINDOW{
	short vx, vy ;
	unsigned short xsize, ysize;
	char *title;
	char flag ;		// ビット情報
	int icon[16*16] ;					// 16*16のアイコン情報
	struct WINDOW *next_win ; //next_winには一つ下のウィンドウへのポインタが入る
	struct SHEET *this_s_p ;	// this_s構造体へのポインタ(ソースの可読性を図るため)
	struct SHEET this_s ;			// ウィンドウのシート画素バッファ
};


class WINDOWCTL{
	WINDOW	*active_win, *bottom ;		// ウィンドウの順番を決める線形リストの端
	UINT	window_num ;						// ウィンドウの数
	static bool _firstFlag;

	void _create(WINDOW *window) ;
	void _del(WINDOW *window);
	void _active(WINDOW *window);
	void active_titlebar(void) const ;
	void non_active_titlebar(void) const ;
	
	AvoidDeadLock _using ;

public:
	WINDOW *create(short vx, short vy, short xsize, short ysize, char *w_title) ;		// ウィンドウ作成
	void del(WINDOW *window);
	void active(WINDOW *window);
	bool check_active(struct WINDOW *window) ;
	void refresh(struct WINDOW *window, short vx, short vy, short xsize, short ysize) const ;
	void all_refresh(struct WINDOW *window) const ;
	void win_change(void) ;
} extern windowctl ;
