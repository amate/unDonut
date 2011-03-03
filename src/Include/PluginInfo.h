/**
 *	@file	PluginInfo.h
 *  @brief	unDonutのプラグインに関する各種定義.
 *	@note
 *	unDonutのプラグインに関する各種定義を記述したヘッダです。
 *	プラグインを作成する際にはこのヘッダをインクルードしてください。
 */
#ifndef _x_PLUGININFO_H_x_
#define _x_PLUGININFO_H_x_

#pragma once

#include <windows.h>



// PluginInfo : プラグインの情報・設定を保有する構造体
//			  : 本体はGetPluginInfo()関数によってプラグインにこの構造体に情報を設定することを要求します。
typedef struct _PLUGININFO {
	int  	type;								// プラグインの種類とスタイルを組み合わせて設定します。
												// 重要なのはこの値だけなので、しっかりと設定してください。
	char 	name[100]; 							// プラグインの名称を設定します。
	char 	version[100];						// バージョンです。書式は自由です。
	char 	versionDate[100];					// 更新日時を入れることが推奨されますが、何に使っても構いません。
	char 	authorName[100];					// 作者名を設定します。
	char 	authorEmail[100];					// 作者の連絡先を設定します。(無くても構いません)
	char 	authorUrl[100];						// 必要であれば作者のWebサイトのURLを設定してください。
	char 	comment[1024 * 10];					// プラグインの説明や関連情報を設定してください。
} PLUGININFO;


// PluginType : PLUGININFO構造体のtypeメンバに設定するプラグインタイプの定数です。
//			  : この中から一つ選んで設定します。
typedef enum _PLUGIN_TYPE {
	PLT_TOOLBAR 	= 1,						// リバーに格納されるツールバープラグイン
	PLT_EXPLORERBAR = 2,						// プラグインバーに格納されるエクスプローラバープラグイン
	PLT_STATUSBAR	= 3,						// ステータスバーに格納されるステータスバープラグイン(未実装です)
	PLT_OPERATION	= 4,						// 表には表示されないバックグラウンドプラグイン(未実装です)
	PLT_DOCKINGBAR	= 5, 						// ドッキングウィンドウとして配置されるドッキングバープラグイン
} PLUGIN_TYPE;

#define PLUGIN_TYPECNT	5						// プラグインの種類数(暫定)



// PluginStyle : PLUGININFO構造体のtypeメンバに設定するプラグインのスタイル定数です。
//			   : PLUGIN_TYPEの定数と組み合わせて設定してください。
#define PLUGIN_STYLE_DERAYLOADING	0			//本体は必要になったときにプラグインにウィンドウ生成を要求します。
#define PLUGIN_STYLE_EARLYLOADING	16			//本体の初期化完了時にプラグインにウィンドウ生成を要求します。

//PLUGIN_STYLE_DERAYLOADINGとPLUGIN_STYLE_EARLYLOADINGは同時に指定することは出来ません。
//これらのフラグはエクスプローラバープラグインでのみ有効です。
//それ以外は本体の初期化中または初期化後にウィンドウ生成が行われます。


//PluginType :プラグイン関数が使う型名の定義です。そんなに大事なものでもありませんが。
#define FPARAM LONG_PTR
#define SPARAM LONG_PTR


// Plugin Event ID : プラグインに対して送信される本体側イベントの識別IDです。
//				   : 本体はPluginEvent()関数を通じてプラグインに各種イベントの発生を通知します。
#define DEVT_INITIALIZE_COMPLETE	1			//本体側の初期化が完了しました。
#define DEVT_TAB_OPENED 			2			//新しいタブが生成されました。
#define DEVT_TAB_CHANGEACTIVE		3			//アクティブなタブが変更されました。
#define DEVT_TAB_BEFORENAVIGATE 	4			//新しいページにナビゲートされようとしています。
#define DEVT_TAB_NAVIGATE			5			//ナビゲートが開始されました。
#define DEVT_TAB_DOCUMENTCOMPLETE	6			//ナビゲートが完了しました。
#define DEVT_TAB_CLOSE				7			//タブが閉じられました。
#define DEVT_TAB_MOVE				8			//タブが移動されました。(未実装です。ごめんなさい)
#define DEVT_CHANGESIZE 			10			//本体ウィンドウのサイズが変更されました。
#define DEVT_CHANGESTATUSTEXT		11			//ステータスバーのテキストが変更されました。
#define DEVT_BLOCKTITLE 			12			//タイトルによってタブのオープンがキャンセルされました。
#define DEVT_BLOCKPOPUP 			13			//URLによってタブのオープンがキャンセルされました。


// Plugin Functions' Parameters : イベントの副情報として送られてくるデータを格納する構造体です。
typedef struct _DEVTS_TAB_NAVIGATE {			//DEVT_TAG_NAVIGATEイベントのパラメータデータを格納します。
	int 		nIndex; 						//ナビゲートされるタブのインデックス
	LPCTSTR 	lpstrURL;						//ナビゲート先のURL
	LPCTSTR 	lpstrTargetFrame;				//ナビゲート先のフレーム名
} DEVTS_TAB_NAVIGATE;


typedef struct _DEVTS_TAB_DOCUMENTCOMPLETE {	//DEVT_TAB_DOCUMENTCOMPLETEイベントのパラメータデータを格納します。
	int 		nIndex; 						//ナビゲート完了したタブのインデックス
	LPCTSTR 	lpstrURL;						//タブの新しいURL
	LPCTSTR 	lpstrTitle; 					//タブの新しいタイトル
	BOOL		bMainDoc;						//最上位のドキュメント(フレーム)かどうか
} DEVTS_TAB_DOCUMENTCOMPLETE;



#endif
