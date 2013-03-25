/**
 *	@file	ParseInternetShortcutFile.cpp
 *	@brief	インターネットショートカット(.url)からurlを取得. (検索拡張あり)
 *	@note
 *	+++ MtlWeb.cpp にあった MdlParseInternetShortcutFile を、検索関係の拡張をしたもの.
 *		検索バーやアドレスバーを呼び出してて、すでに MTL の範疇外だが、
 *		Mtl::MDITabCtrl からも呼ばれるので、とりあえず MTL のフリをする.
 *		(MDITabCtrlからの呼び方を１クッションおくようにすべき...)
 */

#include "stdafx.h"
#include "DonutPFunc.h"
#include "DonutViewOption.h"
#include "MtlDragDrop.h"
#include "HlinkDataObject.h"
#include "MenuDropTargetWindow.h"
#include "FlatComboBox.h"
#include "DonutFavoritesMenu.h"
#include "ExStyle.h"
#include "DonutSearchBar.h"
#include "MtlBrowser.h"
#include "DonutAddressBar.h"
#include "Donut.h"		// CString Donut_GetActiveSelectedText() のため
#include "ParseInternetShortcutFile.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



namespace MTL {


///+++ アドレスバーの文字列を返す.
///    ※ CSearchBar向けに用意. 本来は CDonutAddressBar::の同名関数を呼べばいいだけだが、
/// 	  DonutAddressBar.hをSearchBar.hでincludeすると後がしんどいので...
CString GetAddressBarText()
{
	return	CDonutAddressBar::GetAddressBarText();
}



///+++ strFilePath がインターネットショートカットだったらば、実際のurlにして返す.
///    このとき、拡張プロパティ([unDonut])で検索指定が設定されていたらそれを反映.
/// @return 0:インターネットショートカットじゃない	1:有効なurlだった. 2:検索指定が反映されたurlになった.
int ParseInternetShortcutFile_SearchMode(CString &strFilePath, const CString& strSearch)
{
	CString 	strExt = strFilePath.Right(4);
	if (strExt.CompareNoCase( _T(".url") ) != 0)
		return 0;

	CIniFileI	pr0(strFilePath, _T("InternetShortcut"));
	CString 	strUrl = pr0.GetString(_T("URL"), NULL, INTERNET_MAX_PATH_LENGTH );
	pr0.Close();
	if (strUrl.IsEmpty())
		return 0;

  #if 0 //+++ 拡張プロパティがonされているかチェック.
	CIniFileI	pr(strFilePath, DONUT_SECTION);
	if (pr.GetValue(EXPROP_KEY_ENABLED, 0) == 0) {
		strFilePath = strUrl;
		return 1;
	}
  #endif

	//+++ 非常にばっちぃ...が、とにかく動くこと優先...
	CDonutSearchBar* pSearch = CDonutSearchBar::GetInstance();
  #if 1 //ndef NDEBUG
	if (pSearch) {
	//	CDonutSearchBar *pSearch0 = pSearch;
	//	pSearch = (CDonutSearchBar *) ::SendMessage(pSearch->GetTopLevelParent(), WM_GET_SEARCHBAR, 0, 0);
	//	ATLASSERT( pSearch == pSearch0 );
	}
  #endif
	if (pSearch) {
		bool		bUrlSearch = false;

		CString 	strIniFile	= strFilePath;
		CString 	strWord 	= strSearch;		// 検索文字列.

		CIniFileI	pr(strIniFile, DONUT_SECTION/*_T("unDonut")*/);
		bool		bAdrBar = pr.GetValue(_T("ExPropOpt"), 0) & 1;
		pr.Close();

	   #if 0	//強引なので、跡で、仕組みを直す
		// 選択範囲があれば、それを優先する.
		CString 		strSel = Donut_GetActiveSelectedText();
		if (strSel.IsEmpty() == 0) {
			strWord = strSel;
			if (bAdrBar)	// アドレスバーから取ってくる指定があるとき.
				bUrlSearch = true;
		}
	   #endif
		if (strWord.IsEmpty()) {
			if (bAdrBar) {	// アドレスバーから取ってくる指定があるとき.
				strWord    = CDonutAddressBar::GetAddressBarText();
				bUrlSearch = true;
			}
			if (strWord.IsEmpty()) {	// まだ検索文字列が設定されていない場合は、検索バーのテキストを取得.
				strWord    = MtlGetWindowText(pSearch->GetEditCtrl());
				             //pSearch->RemoveShortcutWord( MtlGetWindowText( pSearch->GetEditCtrl() ) );
				bUrlSearch = false;
			}
		  #if 0
			if (strWord.IsEmpty()) {	// まだ文字列が空だったらば、テキスト選択があればそれを採用.
				strWord    = Donut_GetActiveSelectedText();
			}
		  #endif
		}
		strFilePath   = strUrl;
		return 2;	//\\test
#if 0
		if (strWord.IsEmpty() == FALSE) {	// 検索文字列が設定されていたら、そのようにUrlを作成.
			CIniFileI	pr(strIniFile, DONUT_SECTION/*_T("unDonut")*/);
			bool rc = pSearch->GetOpenURLstr(strFilePath, strWord, pr, CString()/*strUrl*/);
			pr.Close();
			if (rc && bUrlSearch == 0) {	// url検索の時以外は履歴に入れる.
				pSearch->_AddToSearchBoxUnique(strWord);
			}
			return 2;
		}
#endif
	} else {
		strFilePath = strUrl;
	}
	return 1;
}



///+++ strFilePath がインターネットショートカットだったらば、実際のurlにして返す.
///    このとき、拡張プロパティで検索指定(unDonutSearch)が設定されていたらそれを反映.
bool ParseInternetShortcutFile(CString &strFilePath)
{
	return ParseInternetShortcutFile_SearchMode(strFilePath, CString()) != 0;
}


}	// MTL
