//=========================================
//         OASIS Kernel source code
//                Copyright (C) 2010 soS.A
//=========================================

/* FIFOライブラリ */

#include "kernel.h"

//#define FLAGS_OVERRUN		0x0001

/* FIFOバッファの初期化 */
FIFO::FIFO(const UINT datasize) {
	size=datasize;			// バッファサイズ
	buf = new UINT[datasize] ;	//@これはスタックを使った方がいいかと・・・
	free_s_size = datasize; 	// 空き
	p = 0; 						// 書き込み位置
	q = 0; 						// 読み込み位置
}

/* FIFOバッファの抹消 */
FIFO::~FIFO(void) {
	delete [] buf ;
}

/* FIFOへデータを送り込んで蓄える */
void FIFO::put(const UINT data){
	// 空きがなくてあふれた
	if (!free_s_size) {
		free_s_size=(size>>2)+1;
		q+=(size>>2)+1 ;	// 古い情報は消去

		if (q >= size)
			q-= size;
	}

	buf[p] = data;			// 書き込み位置にデータを挿入

	// 書き込み位置を次へ（もし書き込み位置がバッファサイズを超えてたら、０へ）
	p++;
	if (p >= size)
		p = 0;

	free_s_size--;							// データ書き込みにより、空きサイズも減少

	return ;
}

/* FIFOからデータを一つ取ってくる */
UINT FIFO::get(void) {
	UINT data;

	// バッファが空っぽのときは、とりあえず0xFFFFFFFFが返される
	if (free_s_size == size)
		return 0xFFFFFFFF;

	data = buf[q];						// データ設定
	// 読み込み位置をずらす（ＦＩＦＯのサイズからはみ出していたら、補正）
	q++;
	if (q >= size)
		q = 0;
	// ついでに空き容量も増やしておく
	free_s_size++;

	return data;
}
