//=========================================
//         OASIS Kernel header code
//                Copyright (C) 2010 soS.A
//=========================================

struct WINDOW{
	short vx, vy ;
	unsigned short xsize, ysize;
	char *title;
	char flag ;		// �r�b�g���
	int icon[16*16] ;					// 16*16�̃A�C�R�����
	struct WINDOW *next_win ; //next_win�ɂ͈���̃E�B���h�E�ւ̃|�C���^������
	struct SHEET *this_s_p ;	// this_s�\���̂ւ̃|�C���^(�\�[�X�̉ǐ���}�邽��)
	struct SHEET this_s ;			// �E�B���h�E�̃V�[�g��f�o�b�t�@
};


class WINDOWCTL{
	WINDOW	*active_win, *bottom ;		// �E�B���h�E�̏��Ԃ����߂���`���X�g�̒[
	UINT	window_num ;						// �E�B���h�E�̐�
	static bool _firstFlag;

	void _create(WINDOW *window) ;
	void _del(WINDOW *window);
	void _active(WINDOW *window);
	void active_titlebar(void) const ;
	void non_active_titlebar(void) const ;
	
	AvoidDeadLock _using ;

public:
	WINDOW *create(short vx, short vy, short xsize, short ysize, char *w_title) ;		// �E�B���h�E�쐬
	void del(WINDOW *window);
	void active(WINDOW *window);
	bool check_active(struct WINDOW *window) ;
	void refresh(struct WINDOW *window, short vx, short vy, short xsize, short ysize) const ;
	void all_refresh(struct WINDOW *window) const ;
	void win_change(void) ;
} extern windowctl ;
