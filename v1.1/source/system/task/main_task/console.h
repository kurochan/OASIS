/*
 *  console.h
 *
 *
 *  Created by Liva on 10/12/06.
 *
 */


class CONSOLE {
	UINT _bufferX, _bufferY, _bufferSize ;	//_bufferSize=_bufferX*_bufferY
	UINT _lineOfY;					// スクリーン上に表示できる行数
	UINT _charOfCorner ;			// 左上の文字のバッファオフセット
	UINT _charOfCurrent ;			// 現在の文字のバッファオフセット

	Timer timer;
	Caller caller_key, caller_timer;
	int _cursor;

	UINT _numberOfCommand ;
	char *_buffer;
	WINDOW *_window ;
	void _typeString(char *msg) ;
	void _typeChar(void);
	void _newline(void);
	void _scroll(void);

	// command
	void system(void) ;
	void cls(void) ;
	void help(void) ;

	struct COMMAND {
		char *_string;
		void (CONSOLE::*_pfunc)(void);
	} *Command;

public:
	CONSOLE( const short windowPosX, const short windowPosY, const short windowSizeX, const short windowSizeY, const UINT bufX, const UINT bufY );
	void lootin( UCHAR key );
	void cursor_func( UCHAR tmp );
} ;
