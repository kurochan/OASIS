;=========================================
;         OASIS Kernel source code
;                Copyright (C) 2010 soS.A
;=========================================

; OASIS Boot Loader
; ブートセクタ部
; TAB=4

CYLS	EQU		10				; どこまで読み込むか
ORG		0x7c00			; このプログラムがどこに読み込まれるのか


[SECTION .text]

; 以下は標準的なFAT12フォーマットフロッピーディスクのための記述

		JMP		SHORT	start
		DB		0x90
		DB		"OASIS   "		; ブートセクタの名前を自由に書いてよい（8バイト）
		DW		512				; 1セクタの大きさ（512にしなければいけない）
		DB		1				; クラスタの大きさ（1セクタにしなければいけない）
		DW		1				; FATがどこから始まるか（普通は1セクタ目からにする）
		DB		2				; FATの個数（2にしなければいけない）
		DW		224				; ルートディレクトリ領域の大きさ（普通は224エントリにする）
		DW		2880			; このドライブの大きさ（2880セクタにしなければいけない）
		DB		0xf0			; メディアのタイプ（0xf0にしなければいけない）
		DW		9				; FAT領域の長さ（9セクタにしなければいけない）
		DW		18				; 1トラックにいくつのセクタがあるか（18にしなければいけない）
		DW		2				; ヘッドの数（2にしなければいけない）
		DD		0				; パーティションを使ってないのでここは必ず0
		DD		2880			; このドライブ大きさをもう一度書く
		DB		0,0,0x29		; よくわからないけどこの値にしておくといいらしい
		DD		0xffffffff		; たぶんボリュームシリアル番号
		DB		"OASIS v0.01"	; ディスクの名前（11バイト）
		DB		"FAT12   "		; フォーマットの名前（8バイト）
		TIMES	18 DB 0				; とりあえず18バイトあけておく

; プログラム本体

start:
		MOV		AX,0
		MOV		SS,AX
		MOV		DS,AX
		MOV		SP,0x7c00

		MOV		AX,0x03
		INT		0x10

		MOV		SI,startup
		MOV		BX,0x0007
		MOV		CX,61
		CALL	message


l_disk:
; ディスクを読む
		MOV		AX,0x0820
		MOV		ES,AX
		MOV		DH,0			; ヘッド0
		MOV		CH,0			; シリンダ0
		MOV		CL,2			; セクタ2
		MOV		DL,0

readloop:
		MOV		SI,0			; 失敗回数を数えるレジスタ
retry:
		MOV		AH,0x02			; AH=0x02 : ディスク読み込み
		MOV		AL,1			; 1セクタ
		MOV		BX,0
		INT		0x13			; ディスクBIOS呼び出し
		JNC		next			; エラーがおきなければnextへ
		INC		SI				; SIに1を足す
		CMP		SI,4			; SIと4を比較
		JA		fin		; SI > 4 だったらerrorへ(SI >= 5)
		MOV		AH,0x00
		INT		0x13			; ドライブのリセット
		JMP		retry

next:
		MOV		AX,ES			; アドレスを0x20進める
		ADD		AX,0x0020
		MOV		ES,AX			; ADD ES,0x020 という命令がないのでこうしている
		INC		CL				; CLに1を足す
		CMP		CL,19			; CLと19を比較
		JB		readloop		; CL < 19 だったらreadloopへ
		MOV		CL,1
		INC		DH
		CMP		DH,2
		JB		readloop		; DH < 2 だったらreadloopへ
		MOV		DH,0
		INC		CH
		CMP		CH,CYLS
		JB		readloop		; CH < CYLS だったらreadloopへ

; 読み終わったのでharibote.sysを実行だ！
		MOV		[0x0ff0],CH		; IPLがどこまで読んだのかをメモ
		JMP		0xc200


diskerror:
		MOV		BX,0x000c
		MOV		CX,64
		MOV		SI,disk_err_msg
		CALL	message
		JMP		fin

message:
		MOV		AL,CL
		MOV		AH,0x03
		INT		0x10
		MOV		CX,0
		MOV		CL,AL
		MOV		AX,0
		MOV		ES,AX
		MOV		BP,SI
		MOV		AX,0x1301
		INT		0x10
		RET

return:
		RET

fin:
		HLT
		JMP		fin

startup:
		DB		0x0d,0x0a
		DB		"==================="
		DB		0x0d,0x0a
		DB		"    OASIS v0.02"
		DB		0x0d,0x0a
		DB		"==================="
		DB		0x0d,0x0a

disk_err_msg:
		DB		0x0d,0x0a
		DB		" Disk load failed."
		DB		0x0d,0x0a
		DB		" Press Reset key to restart your computer."

		TIMES 0x1fe-($-$$) DB 0		; 0x7dfeまでを0x00で埋める命令

		DB		0x55, 0xaa
