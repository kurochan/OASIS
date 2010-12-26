//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================

/* タイマ */
class Timer {
public:
	short pid;
	Timer *next_timer ;
	UINT timeout ;
	bool _using;
	Timer(void);
	void set_timeout(unsigned int time, unsigned int count) ;
	Caller *FuncOfTimeout;
};

/* 複数あるタイマの管理 */
class TIMERCTL  {
	Timer _guardTimer ;					// 番兵用タイマ
	Timer *_firstTimerAddr ;			// 線形リスト上で一番最初にあるタイマ

public:
	UINT count;
	void init(void) ;						// 初期化
	void create(Timer *c_timer, unsigned int time, Caller *caller) ;	// 新しいタイマの作成
	void wait(Timer *c_timer, unsigned int time) ;					// 新しいタイマの作成
	void checkTimeout(UCHAR tmp) ;		// タイムアウトしたタイマがないかチェック、もしあれば、既定の動作をする 
	void reset(void) ;					// タイマのカウント数が上限を超えそうになった時にリセットする
	void inthandler20(int *esp) ;	// タイマ割り込みが起きた時の処理
	char check_count(unsigned int c_count) const ;		// カウント数のチェック
} extern timerctl;
