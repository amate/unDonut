/**
 *	@file	PluginManager.h
 *	@brief	プラグイン・マネージャ
 */
#pragma once

//プラグインのデータ型・定数の定義
#include "./include/PluginInfo.h"


//+++ GetProcAddressはTCHAR未対応なので、_Tは削除.

#define ENTRY_PLUGINPROC(func, type)						 \
	static type GetPtr_##func(HINSTANCE hInst)				 \
	{														 \
		if (!hInst) 										 \
			return NULL;									 \
		/*return (type) ::GetProcAddress( hInst, _T(# func) );*/ \
		return (type) ::GetProcAddress( hInst, #func );		\
	}



class CPluginManager {
public:
	typedef int  (WINAPI* LPFPLUGINEVENT)(UINT, FPARAM, SPARAM);
	typedef void (WINAPI* LPFGETPLUGININFO)(PLUGININFO *);
	typedef HWND (WINAPI* LPFCREATEPLUGINWINDOW)(HWND);
	typedef HWND (WINAPI* LPFCREATEEXPLORERPANE)(HWND, UINT);
	typedef HWND (WINAPI* LPFCREATETOOLBAR)(HWND, UINT);
	typedef void (WINAPI* LPFSHOWEXPLORERMENU)(int, int);
	typedef BOOL (WINAPI* LPFPRETRANSLATEMESSAGE)(MSG *);
	typedef void (WINAPI* LPFPLUGINSETTING)();
	typedef void (WINAPI* LPFSHOWTOOLBARMENU)(UINT);
	typedef BOOL (WINAPI* LPFEVENT_TABCHANGED)(int, IWebBrowser *); //obsolete

	//内部処理用のプラグインデータ構造体
	struct PluginData {
		HINSTANCE	   hInstDLL;
		HWND		   hWnd;
		CString 	   strCaption;
		CString 	   strIniKey;
		LPFPLUGINEVENT fpEvent;
		int 		   nStyle;
		//int		   flags;	//+++	無理やりな処理で使う...やっぱやめ.

		PluginData()
		{
			this->hInstDLL 	= NULL;
			this->hWnd	 	= NULL;
			this->fpEvent	= NULL;
			this->nStyle	= PLUGIN_STYLE_DERAYLOADING;
			//this->flags	= 0;		//+++
		}
	};

	static CString PluginDir(); 	//+++ プラグインフォルダを返す. 最後は\つき.

private:
	//メンバ変数
	typedef std::vector<PluginData> 	PluginArray;
	static PluginArray *	m_avecData; 	//プラグインタイプ毎のデータの配列(vector)

  #ifdef _DEBUG
	static int				m_nLoadCount;
	static int				m_nCreateCount;
  #endif	//_DEBUG

	//プラグイン関数のポインタ取得関数に展開されるマクロ
	//GetPtr_関数名でDLLのインスタンスハンドルを引数にして呼び出す
	//ex. LPFGETPLUGININFO pfGetPInfo = GetPtr_GetPluginInfo(hInstDLL);
	ENTRY_PLUGINPROC(GetPluginInfo		, LPFGETPLUGININFO		)
	ENTRY_PLUGINPROC(PluginEvent		, LPFPLUGINEVENT		)
	ENTRY_PLUGINPROC(CreatePluginWindow , LPFCREATEPLUGINWINDOW )
	ENTRY_PLUGINPROC(CreateExplorerPane , LPFCREATEEXPLORERPANE )
	ENTRY_PLUGINPROC(CreateToolBar		, LPFCREATETOOLBAR		)
	ENTRY_PLUGINPROC(ShowExplorerMenu	, LPFSHOWEXPLORERMENU	)
	ENTRY_PLUGINPROC(PreTranslateMessage, LPFPRETRANSLATEMESSAGE)
	ENTRY_PLUGINPROC(PluginSetting		, LPFPLUGINSETTING		)
	ENTRY_PLUGINPROC(Event_TabChanged	, LPFEVENT_TABCHANGED	)
	ENTRY_PLUGINPROC(ShowToolBarMenu	, LPFSHOWTOOLBARMENU	)

	//プラグイン情報をもつ構造体のポインタを取得する
	static PluginData * GetDataPtr(int nKind, int nIndex);

	//ロードに失敗したプラグインを片付ける
	static void 		SettleBadPlugin(int nKind, int nIndex, PluginData &data, HWND hWnd = NULL);
	static BOOL 		ReleasePluginData(PluginData &data);

	//プラグイン情報を配列から削除する（ウィンドウ等の開放も行う）
	static BOOL 		RemovePlugin(int nKind, int nIndex);
	static void 		ErrMsg_FailLoad(LPCTSTR lpstrName, LPCTSTR lpstrPath, HWND hWnd = NULL);

public:
	CPluginManager();
	~CPluginManager();
	static void 		Init();
	static void 		Term();


	//プラグインのタイトルを取得する
	//BOOL型の領域へのポインタが指定されれば成功・失敗をそこに返す
	static CString		GetCaption(int nKind, int nIndex, BOOL *pbRet = NULL);


	//プラグインのウィンドウハンドルを取得する
	//失敗したとき、もしくは作成されていない場合はNULLを返す
	static HWND 		GetHWND(int nKind, int nIndex);

	bool				IsEarlyLoading(int nKind, int nIndex);

	static int			GetCount(int nKind);


	//DLLのロード及びプラグイン情報の読み込みを行う
	static BOOL 		ReadPluginData(int nKind, HWND hWnd = NULL);


	//プラグインのロードを行う
	//引数はプラグインの種類と番号と、組み込む親ウィンドウのハンドル
	static BOOL 		LoadPlugin(int nKind, int nIndex, HWND hWndParent, bool Force = true);

	static BOOL 		LoadAllPlugin(int nKind, HWND hWndParent, bool bForce = false);

	static void 		DeleteAllPlugin(int nKind = -1);


	//プラグインの関数呼び出しルーチン
	static void 		Call_ShowExplorerMenu(int nIndex, int x, int y);
	static int			Call_PluginEvent(int nKind, int nIndex, UINT uMsg , FPARAM fParam, SPARAM sParam);
	static BOOL 		Call_PreTranslateMessage(int nKind, int nIndex, MSG *lpMsg);
	static void 		Call_PluginSetting(int nKind, int nIndex);
	static BOOL 		Call_Event_TabChanged(int nKind, int nIndex, int nNewTabIndex, IWebBrowser *pWB);
	static void 		Call_ShowToolBarMenu(int nKind, int nIndex, UINT uID);


	//全てのプラグインに対してイベント発生を送る
	static void 		BroadCast_PluginEvent(UINT uMsg, FPARAM fParam, SPARAM sParam);


	//全てのプラグインに対してイベント発生を順に送り、非0の値が返された時点で終了する
	static int			ChainCast_PluginEvent(UINT uMsg, FPARAM fParam, SPARAM sParam);

};
