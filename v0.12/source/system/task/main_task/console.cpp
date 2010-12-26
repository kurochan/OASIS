/*
 *  console.cpp
 *  
 *
 *  Created by Liva on 10/12/06.
 *
 */

#include "kernel.h"
#include <string.h>
#include <stdio.h>


CONSOLE::CONSOLE(const short windowPosX, const short windowPosY, const short windowSizeX, const short windowSizeY, const UINT bufX, const UINT bufY){
	_window=windowctl.create( windowPosX, windowPosY, windowSizeX, windowSizeY, "Console" );
	_bufferX=bufX;
	_bufferY=bufY;
	_bufferSize=_bufferX*_bufferY ;
	_buffer=new char[_bufferSize] ;
	_lineOfY=windowSizeY/16;
	_charOfCorner=0;
	_cursor=0;

	_charOfCurrent=0;
	_typeString("OASIS v0.12.0") ;
	_newline();
	_typeString("Created by OASIS Developer Team on 2010/12") ;
	_newline();
	_typeString("License is GPL v2 only") ;
	_newline();
	_newline();
	_typeString("> ") ;

	// newを使った書き方はようわからん by Liva
	COMMAND tmp[] = {
		"system", &CONSOLE::system,
		"cls", &CONSOLE::cls,
		"dir", &CONSOLE::dir
	};
	_numberOfCommand=sizeof(tmp)/sizeof(tmp[0]);
	Command = (CONSOLE::COMMAND*)malloc_s(sizeof(tmp));
	for(UINT cnt=0; cnt<_numberOfCommand; cnt++)
		Command[cnt]=tmp[cnt];

	caller_timer.SetFunction(*this,&CONSOLE::cursor_func);
	timerctl.create(&timer, 30, &caller_timer);

	caller_key.SetFunction(*this, &CONSOLE::lootin);
	taskctl.set_input(KEY_ID,&caller_key) ;

	return;
}


void CONSOLE::lootin(UCHAR key) {
	if (!windowctl.check_active(_window))
		return;

	char *command;
	UINT commandlen;

	//カーソル消去
	_cursor=0xFFFFFFFF;
	cursor_func(0);			//0という数値に特に意味はない

	switch (key) {
		case 8 :		// バックスペース
			if ( _charOfCurrent==_charOfCorner )
				break;			//カーソルは左上にあるから、これ以上消せない
			
			if ( (_charOfCurrent%_bufferX) == 2 )
				break;			//これ以上消せない
			
			// カーソルをスペースで消してから、カーソルを1つ戻す
			if (_charOfCurrent==0) {
				_charOfCurrent=_bufferSize;
				_charOfCorner += _bufferSize ;
			}
			_charOfCurrent--;
			_buffer[_charOfCurrent]=' ';
			boxf(_window, (_charOfCurrent%_bufferX)*8, ((_charOfCurrent-_charOfCorner)/_bufferX)*16, 8, 16, 0xFFFFFF );
			windowctl.refresh(_window, (_charOfCurrent%_bufferX)*8, ((_charOfCurrent-_charOfCorner)/_bufferX)*16, 8, 16) ;
			break ;
			
		case 10 :		// Enter
			
			//コマンド実行
			_buffer[_charOfCurrent]=NULL;
			
			commandlen=strlen(&_buffer[_charOfCurrent-(_charOfCurrent%_bufferX)+2]) ;
			if (commandlen==0){
				_newline();
				_typeString("> ");
				return ;
			}
				
			command=new char[commandlen] ;
			strcpy(command,&_buffer[_charOfCurrent-(_charOfCurrent%_bufferX)+2]) ;

			_newline();
	
			for ( UINT i = 0 ; i < _numberOfCommand ; i++ ){
				if ( !strcmp(command,Command[i]._string) ){
					void (CONSOLE::*pfunction)(void)=Command[i]._pfunc;
					(this->*pfunction)();
					
					_newline();
					// プロンプト表示
					_typeString("> ") ;
					
					return;
				}
			}

			//コマンドではなく、さらに空行でもない
			_typeString(command) ;
			_typeString(": Bad command.") ;
			_newline();
			
			// プロンプト表示
			_typeString("> ") ;
			
			break ;
			
		default :
			// 一般文字
			
			if ( (_charOfCurrent%_bufferX) ==0 )
				break;			//これ以上打てない

			// 一文字表示してから、カーソルを1つ進める
			_buffer[_charOfCurrent] = key ;
			_typeChar();
			break ;
	}
	
	return;
}

void CONSOLE::_typeString(char *msg) {
	
	for (UINT cnt=0; cnt<strlen(msg); cnt++){
		_buffer[_charOfCurrent]=msg[cnt];
		_typeChar();
	}
	
	return ;
}

void CONSOLE::_typeChar(void) {
	
	_buffer[_charOfCurrent+1]=NULL;
	mes( _window, (_charOfCurrent%_bufferX)*8, ((_charOfCurrent-_charOfCorner)/_bufferX)*16, &_buffer[_charOfCurrent] );
	windowctl.refresh(_window, (_charOfCurrent%_bufferX)*8, ((_charOfCurrent-_charOfCorner)/_bufferX)*16, 8, 16) ;
	_charOfCurrent++;

	if (_charOfCurrent-_charOfCorner>=_lineOfY*_bufferX)
		_scroll();

	if (_charOfCurrent == _bufferSize){
		_charOfCurrent = 0;
		_charOfCorner -= _bufferSize ;
	}

	return ;
}

void CONSOLE::_newline(void) {
	_charOfCurrent -= _charOfCurrent % _bufferX ;
	_charOfCurrent += _bufferX ;

	if (_charOfCurrent-_charOfCorner >= _lineOfY*_bufferX)
		_scroll();

	return ;
}

void CONSOLE::_scroll(void) {
	
	_charOfCorner += _bufferX ;

	for( short cnty=20; cnty<_window->ysize-16+20; cnty++ )
		for( short cntx=4; cntx<_window->xsize+4; cntx++ )
			_window->this_s_p->buffer[ cnty*_window->this_s_p->xsize+cntx ]=_window->this_s_p->buffer[ (cnty+16)*_window->this_s_p->xsize+cntx ];

	boxf(_window, 0, _window->ysize-16, _window->xsize, 16, 0xFFFFFF );
	windowctl.all_refresh( _window );

	return ;
}

// tmpはインターフェイス統一用に用意した仮の変数。ここでは使用しない
void CONSOLE::cursor_func( UCHAR tmp ) {

	timerctl.create(&timer, 30, &caller_timer);

	if (!windowctl.check_active(_window))
		return;

	boxf( _window, (_charOfCurrent%_bufferX)*8, ((_charOfCurrent-_charOfCorner)/_bufferX)*16+15, 8, 1, 0xFFFFFF&_cursor );
	windowctl.refresh( _window, (_charOfCurrent%_bufferX)*8, ((_charOfCurrent-_charOfCorner)/_bufferX)*16+15, 8, 1 ) ;
	_cursor=~_cursor;

	return;
}


