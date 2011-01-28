//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================

#include "unsigned.h"

//デバイスID

//システム専用
static const UINT TIMER_ID_S	= 1 ;
static const UINT KEYBOARD_ID	= 2 ;
static const UINT MOUSE_ID_S	= 3 ;
static const UINT TIMER_ID		= 4 ;

static const UINT KEY_ID			= 5 ;

static const UINT MAX_ID			= 6 ;

//INPUTCTL上のFIFOでのデータの上位24bitがID識別子になっている
//よって、使えるデバイスの数は最大16777215個。これだけのデバイスを接続する馬鹿はいないと思うので、問題なし。
//MAX_IDは登録するIDの最大値に１を足したもの

//INPUTCTLのFIFOへアクセスするための窓口
class INPUTFIFO {
	UINT q;
public:
	bool idFlag[MAX_ID];					// タスクが登録したIOデバイスのフラグを管理する配列(INPUTCTL以外からはアクセスしないべき)
	INPUTFIFO(void);
	UINT get(void);					//INPUTCTLからデータ取得
	void set_id(const unsigned dev_id);
	void remove_id(const unsigned dev_id);
};


//外部デバイスからの入力データを管理するためのスーパーFIFO
class INPUTCTL : private FIFO {
public:
	INPUTCTL(void):FIFO(256){};
	void i_put(const UINT data);										//INPUTCTLFIFOにデータを追加　親クラスのputと全く同じ
	UINT i_get(UINT &i_q, const INPUTFIFO *Inputfifo);			//INPUTCTLFIFOからデータの取得（INPUTFIFOからのみ呼び出される）
	void set_q(UINT &i_q);												//INPUTCTLFIFOと同期（INPUTFIFOからのみ呼び出される）
} extern *inputctl;

inline void INPUTCTL::i_put(const UINT data){
	put(data);
	return;
}

inline void INPUTCTL::set_q(UINT &i_q){
	i_q=p;
	return ;
}


inline INPUTFIFO::INPUTFIFO(void){
	inputctl->set_q(q);
	for(UINT cnt=0; cnt<MAX_ID; cnt++)
		idFlag[cnt]=0;
}

inline UINT INPUTFIFO::get(void){
	return inputctl->i_get(q,this);
}

inline void INPUTFIFO::set_id(const unsigned dev_id){
	idFlag[dev_id]=1;
	return;
}

inline void INPUTFIFO::remove_id(const unsigned dev_id){
	idFlag[dev_id]=0;
	return;
}


struct MOUSE_DEC {
	UCHAR buf[3], phase, dat;
	int x, y, btn;
} extern mdec;

void enable_mouse(void);
void init_keyboard(void);
void wait_KBC_sendready(void) ;
