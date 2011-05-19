/**
 *	@file	PluginManager.cpp
 *	@brief	プラグイン・マネージャ
 */

#include "stdafx.h"
#include "PluginManager.h"
#include "IniFile.h"
#include "DonutDefine.h"
#include "DonutPFunc.h"
#include "option/PluginDialog.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//using namespace 	std;
//using namespace 	MTL;

//////////////////////////////////////////////////////////
// CPluginManager

// 定義
boost::array<CPluginManager::PluginArray, PLUGIN_TYPECNT + 1>	CPluginManager::m_arrPluginData;

/*
	nKind は 1 Origin 1toPLUGIN_TYPECNT
	ということを忘れて書いてひどい目にあった
 */


//--------------------------------------------
/// プラグイン情報をもつ構造体のポインタを取得する
CPluginManager::PluginData *CPluginManager::GetDataPtr(int nKind, int nIndex)
{
	if (nKind <= 0 || PLUGIN_TYPECNT < nKind)
		return NULL;

	PluginArray *pary = &m_arrPluginData[nKind];

	if (nIndex < 0 || (int) pary->size() <= nIndex)
		return NULL;

	return &pary->at(nIndex);
}


//--------------------------------------------
/// ロードに失敗したプラグインを片付ける
void CPluginManager::SettleBadPlugin(int nKind, int nIndex, PluginData &data, HWND hWnd)
{
	TCHAR 	szBuf[MAX_PATH] = _T("\0");
	::GetModuleFileName( data.hInstDLL, szBuf, MAX_PATH );
	ErrMsg_FailLoad(data.strCaption, szBuf, hWnd);

	RemovePlugin(nKind, nIndex);
}



BOOL CPluginManager::ReleasePluginData(PluginData &data)
{
	if ( ::IsWindow(data.hWnd) ) {
		try {	//+++
			::DestroyWindow(data.hWnd);
			data.hWnd = NULL;
		} catch (...) {
		  #ifdef _DEBUG
			FILE *fp = _wfopen(Misc::GetExeDirectory() + L"errlog.txt", L"a");
			if (fp) {
				fprintf(fp, "プラグイン\"%s\"の終了時にエラーが発生しました。\n", data.strCaption);
				fclose(fp);
			}
		  #endif //_DEBUG
		}
	}

	if (data.hInstDLL) {
		::FreeLibrary(data.hInstDLL);
		data.hInstDLL = NULL;
	}

	return TRUE;
}


//----------------------------------------------
/// プラグイン情報を配列から削除する（ウィンドウ等の開放も行う）
BOOL CPluginManager::RemovePlugin(int nKind, int nIndex)
{
	if (nKind <= 0 || PLUGIN_TYPECNT < nKind)
		return FALSE;

	PluginArray *pary = &m_arrPluginData[nKind];

	if (nIndex < 0 || (int) pary->size() <= nIndex)
		return FALSE;

	if ( ReleasePluginData( pary->at(nIndex) ) )
		pary->erase(pary->begin() + nIndex);
	else
		return FALSE;

	return TRUE;
}



void CPluginManager::ErrMsg_FailLoad(LPCTSTR lpstrName, LPCTSTR lpstrPath, HWND hWnd)
{
	CString strMsg;
	strMsg.Format(_T("\"%s\"(%s)\nをロードすることができませんでした。\n次回起動時は無効になります。"), lpstrName, lpstrPath);
	MessageBox(hWnd, strMsg, _T("Plugin情報"), MB_OK | MB_ICONSTOP);
}



CPluginManager::CPluginManager()
{
}



CPluginManager::~CPluginManager()
{
}



void CPluginManager::Init()
{
}



void CPluginManager::Term()
{
	DeleteAllPlugin();
}


//---------------------------------------------
/// プラグインのタイトルを取得する
/// BOOL型の領域へのポインタが指定されれば成功・失敗をそこに返す
CString CPluginManager::GetCaption(int nKind, int nIndex, BOOL *pbRet)
{
	PluginData *pdata = GetDataPtr(nKind, nIndex);

	if (pdata) {
		if (pbRet)
			*pbRet = TRUE;

		return pdata->strCaption;
	} else {
		if (pbRet)
			*pbRet = FALSE;

		return _T("");
	}
}


//----------------------------------------------
/// プラグインのウィンドウハンドルを取得する
/// 失敗したとき、もしくは作成されていない場合はNULLを返す
HWND CPluginManager::GetHWND(PLUGIN_TYPE nKind, int nIndex)
{
	PluginData *pdata = GetDataPtr(nKind, nIndex);

	if (!pdata)
		return NULL;

	return pdata->hWnd;
}

//--------------------------------------
/// 指定された種類のプラグイン数を返す
int CPluginManager::GetCount(PLUGIN_TYPE nKind)
{
	if (nKind <= 0 || PLUGIN_TYPECNT < nKind)
		return -1;

	return m_arrPluginData[nKind].size();
}



bool CPluginManager::IsEarlyLoading(PLUGIN_TYPE nKind, int nIndex)
{
	PluginData *pdata = GetDataPtr(nKind, nIndex);
	if (!pdata)
		return false;

	return (pdata->nStyle & PLUGIN_STYLE_EARLYLOADING) != 0;	//+++ ? true : false;
}


//----------------------------------------
/// DLLのロード及びプラグイン情報の読み込みを行う
BOOL CPluginManager::ReadPluginData(PLUGIN_TYPE nKind, HWND hWnd)
{
	if (nKind <= 0 || PLUGIN_TYPECNT < nKind)
		return FALSE;

	CString 	strKey;
	strKey.Format(_T("Plugin%02d"), nKind);

	CIniFileIO	pr(g_szIniFileName, strKey);

	CString 	strDLLPath = Misc::GetExeDirectory() + PluginDir();
	int	nCount = pr.GetValuei(_T("Count"));
	for (int nIndex = 0; nIndex < nCount; ++nIndex) {
		try {	//+++
			strKey.Format(_T("%02d"), nIndex);
			//DLLのロード
			CString   strDLLFile = strDLLPath + pr.GetString( strKey );
			HINSTANCE hInstDLL	 = ::LoadLibrary( strDLLFile );

			if (hInstDLL) {
				//データの初期化
				PluginData		 data;
				data.hInstDLL  = hInstDLL;
				data.strIniKey = strKey;
				data.fpEvent   = GetPtr_PluginEvent(hInstDLL);

				//GetPluginInfo関数を使ってプラグイン固有の情報を取得
				LPFGETPLUGININFO	pfGetPluginInfo = GetPtr_GetPluginInfo(hInstDLL);
				if (pfGetPluginInfo) {
					PLUGININFO *	ppi = new PLUGININFO;
					pfGetPluginInfo(ppi);	// 取得
					data.strCaption 	= ppi->name;
					data.nStyle 		= ppi->type & 0xFFFFFFF0;
					delete 		ppi;
				  #if 1	//+++ ドッキングバープラグインをエクスプローラバープラグイン扱いにする.
					if (nKind == PLT_DOCKINGBAR) {
						nKind 	    =  PLT_EXPLORERBAR;
						//+++ data.flags |= 1;
					}
				  #endif
				}

				//データを登録
				m_arrPluginData[nKind].push_back(data);
			} else {
				//ロードに失敗したので登録を取り消す
				ErrMsg_FailLoad(_T("N/A"), strDLLFile, hWnd);
				pr.DeleteValue(strKey);
			}
		} catch (...) {
			ATLASSERT(0);
		}
	}

	return TRUE;
}


//-------------------------------------------------------------------
/// プラグインのロードを行う
/// 引数はプラグインの種類と番号と、組み込む親ウィンドウのハンドル
BOOL CPluginManager::LoadPlugin(PLUGIN_TYPE nKind, int nIndex, HWND hWndParent, bool bForce)
{
	PluginData *pdata	 = GetDataPtr(nKind, nIndex);

	if (!pdata)
		return FALSE;

	if ( ::IsWindow(pdata->hWnd) )
		return TRUE;

											//既にロード済み
	bool		bFuncPtr = false;			//*
	bool		bDelay	 = (pdata->nStyle & PLUGIN_STYLE_EARLYLOADING) == 0;

	if (bDelay && !bForce)
		return TRUE;						//後で読み込む

	try {	//+++
		switch (nKind) {
		case PLT_TOOLBAR:
			{
				//ツールバープラグインのロード - CreateToolBar関数の実行
				LPFCREATETOOLBAR pfCreateToolBar = GetPtr_CreateToolBar(pdata->hInstDLL);
				bFuncPtr = pfCreateToolBar != 0;	//+++ ? true : false;

				if (bFuncPtr) {
					pdata->hWnd = pfCreateToolBar(hWndParent, IDC_PLUGIN_TOOLBAR + nIndex);
				}
			}
			break;

		case PLT_EXPLORERBAR:
			{
				//エクスプローラバープラグインのロード - CreateExplorerPane関数の実行
				LPFCREATEEXPLORERPANE pfCreateExplorerBarPane = GetPtr_CreateExplorerPane(pdata->hInstDLL);
				bFuncPtr = pfCreateExplorerBarPane != 0;	//+++ ? true : false;

				if (bFuncPtr) {
					pdata->hWnd = pfCreateExplorerBarPane(hWndParent, IDC_PLUGIN_EXPLORERBAR + nIndex);
				}
			  #if 1	//+++ 無理やりPLT_DOCKINGBARバーをPLT_TOOLBAR化している場合...
				else /*if (pdata->flags & 1)*/ {
					//ドッキングプラグインのロード - CreatePluginWindow関数の実行
					LPFCREATEPLUGINWINDOW pfCreatePluginWindow = GetPtr_CreatePluginWindow(pdata->hInstDLL);
					bFuncPtr = pfCreatePluginWindow != 0;	//+++ ? true : false;
					if (bFuncPtr) {
						pdata->hWnd = pfCreatePluginWindow(hWndParent);
						::SetParent(pdata->hWnd, hWndParent);
					}
				}
			  #endif
			}
			break;

		case PLT_DOCKINGBAR:
			{
				//ドッキングプラグインのロード - CreatePluginWindow関数の実行
				LPFCREATEPLUGINWINDOW pfCreatePluginWindow = GetPtr_CreatePluginWindow(pdata->hInstDLL);
				bFuncPtr = pfCreatePluginWindow != 0;	//+++ ? true : false;

				if (bFuncPtr) {
					pdata->hWnd = pfCreatePluginWindow(hWndParent);
					::SetParent(pdata->hWnd, hWndParent);		//+++ 強引に設定...
				}
			}
			break;

		case PLT_OPERATION:			//+++ ここはこないと思われるが、念のため用意.
			return TRUE;			//+++

	  #if 0
		case PLT_STATUSBAR:			//+++ 無理やり用意してみる.
			{
				// プラグインのロード - CreatePluginWindow関数の実行
				LPFCREATEPLUGINWINDOW pfCreatePluginWindow = GetPtr_CreatePluginWindow(pdata->hInstDLL);
				bFuncPtr = pfCreatePluginWindow != 0;
				if (bFuncPtr) {
					pdata->hWnd = pfCreatePluginWindow(hWndParent);
					::SetParent(pdata->hWnd, hWndStatusBar);
				}
			}
			break;
	   #endif

		default:
			//それ以外の種類のプラグイン - 未実装なのでアサートする
			ATLASSERT(FALSE);
		}

		//失敗の場合の後片付け
		if ( !bFuncPtr || !::IsWindow(pdata->hWnd) ) {
			SettleBadPlugin(nKind, nIndex, *pdata, hWndParent);
			return FALSE;
		}

		} catch (...) {
		ATLASSERT(0);
	}

	return TRUE;
}

//----------------------------------------
/// 全てのプラグインを読み込む
BOOL CPluginManager::LoadAllPlugin(PLUGIN_TYPE nKind, HWND hWndParent, bool bForce)
{
	if (nKind <= 0 || PLUGIN_TYPECNT < nKind)
		return FALSE;

	int nCount = GetCount(nKind);

	for (int i = 0; i < nCount; i++) {
		if ( !LoadPlugin(nKind, i, hWndParent, bForce) ) {				//ロード失敗するとデータも消される
			i--;
			nCount--;
		}
	}

	return TRUE;
}

//---------------------------------------------
/// 指定された種類のプラグインをすべて解放する(-1ですべての種類を解放)
void CPluginManager::DeleteAllPlugin(int nKind)
{
	int 	nStart;
	int		nEnd;

	if (nKind == -1) {
		nStart = 1;
		nEnd   = PLUGIN_TYPECNT;
	} else if (1 <= nKind && nKind <= PLUGIN_TYPECNT) {
		nStart = nEnd = nKind;
	} else {
		ATLASSERT(FALSE);
		return;
	}

	for (int i = nStart; i <= nEnd; i++) {
		PluginArray *		  pary = &m_arrPluginData[i];

		for (PluginArray::iterator it = pary->begin(); it != pary->end(); ++it)
			ReleasePluginData(*it);

		pary->clear();
	}
}



// プラグインの関数呼び出しルーチン

void CPluginManager::Call_ShowExplorerMenu(int nIndex, int x, int y)	//エクスプローラバーのみサポート
{
	PluginData *		pdata			   = GetDataPtr(PLT_EXPLORERBAR, nIndex);

	if (!pdata)
		return;

	LPFSHOWEXPLORERMENU fpShowExplorerMenu = GetPtr_ShowExplorerMenu(pdata->hInstDLL);

	if (!fpShowExplorerMenu)
		return;

	fpShowExplorerMenu(x, y);
}



int CPluginManager::Call_PluginEvent(int nKind, int nIndex, UINT uMsg , FPARAM fParam, SPARAM sParam)
{
	PluginData *   pdata		 = GetDataPtr(nKind, nIndex);

	if (!pdata)
		return 0;

	try {	//+++
		LPFPLUGINEVENT fpPluginEvent = GetPtr_PluginEvent(pdata->hInstDLL);

		if (!fpPluginEvent)
			return 0;

		return fpPluginEvent(uMsg, fParam, sParam);
	} catch (...) {	//+++
		ATLASSERT(0);
	}
	return 0;
}



BOOL CPluginManager::Call_PreTranslateMessage(int nKind, int nIndex, MSG *lpMsg)
{
	PluginData *		   pdata	= GetDataPtr(nKind, nIndex);
	if (!pdata)
		return FALSE;
	try {	//+++
		LPFPRETRANSLATEMESSAGE fpPreTranslateMessage = GetPtr_PreTranslateMessage(pdata->hInstDLL);

		if (!fpPreTranslateMessage)
			return FALSE;

		return fpPreTranslateMessage(lpMsg);
	} catch (...) {
		ATLASSERT(0);
	}
	return FALSE;
}



void CPluginManager::Call_PluginSetting(int nKind, int nIndex)
{
	PluginData *	 pdata	= GetDataPtr(nKind, nIndex);
	if (!pdata)
		return;

	LPFPLUGINSETTING fpPluginSetting = GetPtr_PluginSetting(pdata->hInstDLL);

	if (!fpPluginSetting)
		return;

	fpPluginSetting();
}



BOOL CPluginManager::Call_Event_TabChanged(int nKind, int nIndex, int nNewTabIndex, IWebBrowser *pWB)	//obsolete
{
	PluginData *		pdata = GetDataPtr(nKind, nIndex);
	if (!pdata)
		return FALSE;

	LPFEVENT_TABCHANGED fpEvent_TabChanged = GetPtr_Event_TabChanged(pdata->hInstDLL);

	if (!fpEvent_TabChanged)
		return FALSE;

	return fpEvent_TabChanged(nNewTabIndex, pWB);
}



void CPluginManager::Call_ShowToolBarMenu(int nKind, int nIndex, UINT uID)								//ツールバープラグインのみのサポート
{
	PluginData *	   pdata = GetDataPtr(nKind, nIndex);
	if (!pdata)
		return;

	LPFSHOWTOOLBARMENU fpShowToolBarMenu = GetPtr_ShowToolBarMenu(pdata->hInstDLL);

	if (!fpShowToolBarMenu)
		return;

	fpShowToolBarMenu(uID);
}


//-----------------------------------
/// 全てのプラグインに対してイベント発生を送る
void CPluginManager::BroadCast_PluginEvent(UINT uMsg, FPARAM fParam, SPARAM sParam)
{
	for (int nKind = 1; nKind <= PLUGIN_TYPECNT; nKind++) {
		int nCount = m_arrPluginData[nKind].size();

		for (int i = 0; i < nCount; i++) {
			Call_PluginEvent(nKind, i, uMsg, fParam, sParam);
		}
	}
}


//------------------------------------
/// 全てのプラグインに対してイベント発生を順に送り、非0の値が返された時点で終了する
int CPluginManager::ChainCast_PluginEvent(UINT uMsg, FPARAM fParam, SPARAM sParam)
{
	for (int nKind = 1; nKind <= PLUGIN_TYPECNT; nKind++) {
		int nCount = m_arrPluginData[nKind].size();

		for (int i = 0; i < nCount; i++) {
			int nRet = Call_PluginEvent(nKind, i, uMsg, fParam, sParam);
			if (nRet != 0)
				return nRet;
		}
	}

	return 0;
}


//--------------------------------------
///+++ "Plugin"フォルダ名を返す. まず Plugin32\ (64ビット版は Plugin64\) が存在すればそれを返し無ければ"Plugin"を返す.
CString CPluginManager::PluginDir()
{
  #ifdef WIN64
	const TCHAR* dir = _T("Plugin64");
  #else
	const TCHAR* dir = _T("Plugin32");
  #endif
	if (::PathFileExists(Misc::GetExeDirectory() + dir) == FALSE)
		dir = _T("Plugin");
	return CString(dir) + _T('\\');
}

