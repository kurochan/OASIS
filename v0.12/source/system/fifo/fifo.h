//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================

/* FIFOバッファ  */
class FIFO {
// INPUTCTLクラスに継承されるから
protected:
	unsigned int *buf, p, q, size, free_s_size;
	// *buf	: 記憶バッファ
	// p		: 書き込み位置
	// q		: 読み込み位置
	// size	: バッファサイズ
	// free_s_size : バッファ空き容量のサイズ

public:
	FIFO(const unsigned int datasize);
	~FIFO(void);
	char put(const unsigned int data);			// データ追加
	unsigned int get(void);							// データ取得
	unsigned int status(void) const;				// データ量取得
};

