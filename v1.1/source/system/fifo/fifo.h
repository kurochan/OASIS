//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================

/* FIFOバッファ  */
class FIFO {
// INPUTCTLクラスに継承されるから
protected:
	// *buf	: 記憶バッファ
	// p		: 書き込み位置
	// q		: 読み込み位置
	// size	: バッファサイズ
	// free_s_size : バッファ空き容量のサイズ

public:
	UINT *buf, p, q, size, free_s_size;

	FIFO(const UINT datasize);
	~FIFO(void);
	void put(const UINT data);				// データ追加
	UINT get(void);							// データ取得
	UINT status(void) const;				// データ量取得
};

/* どのくらいデータが溜まっているかを報告する */
inline UINT FIFO::status(void) const{
	return size - free_s_size;
}
