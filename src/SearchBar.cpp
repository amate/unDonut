/**
 *	@file	SearchBar.cpp
 *	@brief	検索バー
 *	@note
 *		templateで実装されていた SearchBar.h を普通のclassにして .h, .cpp 化したもの.
 */


#include "stdafx.h"
#include "DonutPFunc.h"
#include "DonutViewOption.h"
#include "MtlDragDrop.h"
#include "HlinkDataObject.h"
#include "ExStyle.h"
#include <atlctrls.h>
//#include "StringEncoder.h"			//+++ 不要化
#include <winnls32.h>
#include "ParseInternetShortcutFile.h"
#include "Donut.h"

#include "SearchBar.h"



#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define ENGINENAME_FOR_NO_SEARCH_INI	_T("search.iniが無いのでGoogleのみ")


//////////////////////////////////////////////////////////////////////////////
// CDonutSearchBar

//+++	ParseInternetShortcutFile()用に手抜きで自身のポインタを用意...
CDonutSearchBar*	CDonutSearchBar::s_pThis_ = 0;

// Constructor
CDonutSearchBar::CDonutSearchBar()
	: m_wndKeyword(this, 1)
	, m_wndEngine  (this, 2)
	, m_wndKeywordCombo(this, 3)
	, m_cxBtnsBtn(0)			//+++ 旧横幅調整用
	, m_clrMask(0)				//+++
	, m_hWndKeywordList(0)		//+++
	, m_has(0)					//+++
	, m_nEngineWidth(0)			//+++
	, m_bHilightSw(0)			//+++
	//, m_nHilightBtnNo(0)		//+++
	, m_bExistManifest(IsExistManifestFile())	//+++
	, m_dwTinyWordButton(0)		//+++
	, m_hCursor(NULL)
	, m_bDragAccept(false)
	, m_bShowToolBar(FALSE)
	, m_bLoadedToolBar(FALSE)
	, m_bDragFromItself(false)
{
	//+++ 1つしかインスタンスが作られない、だろうとして、ParseInternetShortcutFile()用に手抜きな自分を指すポインタ.
	ATLASSERT(s_pThis_ == NULL);
	s_pThis_ = this;
}


// Search.iniの絶対パスを返す
CString CDonutSearchBar::GetSearchIniPath()
{
	CIniFileI pr( g_szIniFileName, _T("Search") );
	CString 	strPath = pr.GetStringUW(_T("Path"));
	pr.Close();

	if (strPath.IsEmpty()) {
		strPath = _GetFilePath( _T("Search\\Search.ini") );
		if (::PathFileExists(strPath) == FALSE)
			strPath = _GetFilePath( _T("Search.ini") ); 		//以前の仕様でGo
	}

	return strPath;
}


///+++ 検索文字列を取得する
CString	CDonutSearchBar::GetSearchStr()
{
	HWND hWnd	= GetEditCtrl();

	//+++ なるべく、アロケートが発生しないようにしてみる.
	enum { NAME_LEN = 0x1000 };
	TCHAR	name[ NAME_LEN ] = _T("\0");
	int 	nLen	= ::GetWindowTextLength(hWnd);
	if (nLen >= NAME_LEN)
		nLen	 	= NAME_LEN - 1;
	int 	nRetLen = ::GetWindowText(hWnd, name, nLen + 1);
	name[nLen]		= _T('\0');	//+++1.48c: いつぞやのバグ報告の反映. nLen+1は不味い...
	m_strKeyword	= name;

	return m_strKeyword;
}

///+++ エンジン名の取得.
CString CDonutSearchBar::GetSearchEngineStr()	//+++
{
	HWND	hWnd	= m_cmbEngine;

	//+++ なるべく、アロケートが発生しないようにしてみる.
	enum { NAME_LEN = 0x1000 };
	TCHAR	name[ NAME_LEN ] = _T("\0");
	int 	nLen	= ::GetWindowTextLength(hWnd);
	if (nLen >= NAME_LEN)
		nLen	 	= NAME_LEN - 1;
	int 	nRetLen = ::GetWindowText(hWnd, name, nLen + 1);
	name[nLen]		= _T('\0');	//+++1.48c: いつぞやのバグ報告の反映. nLen+1は不味い...
	m_strEngine		= name;

	return m_strEngine;
}


// サーチエンジンリストをメニューとして返す
CMenuHandle CDonutSearchBar::GetSearchEngineMenuHandle()
{
  #if 1	//+++
	CMenu		menu0;
	_MakeSearchEngineMenu(menu0);
	CMenuHandle menu = menu0.GetSubMenu(0);
	menu0.RemoveMenu(0, MF_BYPOSITION);
	return 		menu;
  #else
	if (m_engineMenu.m_hMenu == 0)
		MakeSearchEngineMenu(m_engineMenu);
	if (m_engineMenu.m_hMenu == 0)
		return 0;
	CMenuHandle menu = m_engineMenu.GetSubMenu(0);
	return menu;
  #endif
}



/// AddressBar側で使うコンボ文字列の設定
void CDonutSearchBar::InitComboBox_for_AddressBarPropertyPage(CComboBox& rCmbCtrl, CComboBox& rCmbShift)
{
	int nCount = m_cmbEngine.GetCount();
	for (int i = 0; i < nCount; i++) {
		CString strBuf;
		m_cmbEngine.GetLBText(i, strBuf);
		rCmbCtrl.AddString(strBuf);
		rCmbShift.AddString(strBuf);
	}
}



//なにやら描画がおかしくなるバグをfix minit
// リバーの背景をコピーする
LRESULT CDonutSearchBar::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	// CAddressBarCtrlImplからコピペ
	HWND	hWnd	= GetParent();
	CPoint	pt;

	MapWindowPoints(hWnd, &pt, 1);
	::OffsetWindowOrgEx( (HDC) wParam, pt.x, pt.y, NULL );
	LRESULT lResult = ::SendMessage(hWnd, WM_ERASEBKGND, wParam, 0L);
	::SetWindowOrgEx((HDC) wParam, 0, 0, NULL);
	return 1; //lResult;
}



void CDonutSearchBar::SetSearchStr(const CString& strWord)
{
	::SetWindowText(GetEditCtrl(), strWord);
}


// private:
// ツールバーを初期化する
void CDonutSearchBar::_InitToolBar(int cx, int cy, COLORREF clrMask, UINT nFlags /*= ILC_COLOR24*/)
{
	m_clrMask	= clrMask;

	CImageList	imgs;
	MTLVERIFY( imgs.Create(cx, cy, nFlags | ILC_MASK, 1, 1) );
	CBitmap 	bmp = AtlLoadBitmapImage(LPCTSTR(GetSkinSeachBarPath(0)), LR_LOADFROMFILE);
	if (bmp.m_hBitmap == NULL)
		bmp.LoadBitmap(IDB_SEARCHBUTTON/*nImageBmpID*/);			//+++	skinの処理がここにあるので、デフォルト素材もこの場で設定にしとく...
	imgs.Add(bmp, clrMask);

	CImageList	imgsHot;
	MTLVERIFY( imgsHot.Create(cx, cy, nFlags | ILC_MASK, 1, 1) );
	CBitmap 	bmpHot = AtlLoadBitmapImage(LPCTSTR(GetSkinSeachBarPath(1)), LR_LOADFROMFILE);
	if (bmpHot.m_hBitmap == NULL)
		bmpHot.LoadBitmap(IDB_SEARCHBUTTON_HOT/*nHotImageBmpID*/);		//+++	skinの処理がここにあるので、デフォルト素材もこの場で設定にしとく...
	imgsHot.Add(bmpHot, clrMask);

  #if 1 //+++ Disableも用意する
	CString str = GetSkinSeachBarPath(2);
	int 	dis = 0;
	if (::PathFileExists(str) == FALSE) {						//+++ 画像ファイルがない時
		if (Misc::IsExistFile(GetSkinSeachBarPath(0))) {	//+++ 通常があれば
			str = GetSkinSeachBarPath(0);					//+++ 通常画で代用
		} else {											//+++ 通常もなければ
			dis = IDB_SEARCHBUTTON_DIS; 					//+++ デフォルトのDisable画を使う.
		}
	}
	CImageList	imgsDis;
	MTLVERIFY( imgsDis.Create(cx, cy, nFlags | ILC_MASK, 1, 1) );
	CBitmap 	bmpDis = AtlLoadBitmapImage(LPCTSTR(str), LR_LOADFROMFILE);
	if (bmpDis.m_hBitmap == NULL && dis)
		bmpDis.LoadBitmap(dis); 					//+++	skinの処理がここにあるので、デフォルト素材もこの場で設定にしとく...
	imgsDis.Add(bmpDis, clrMask);
  #endif

	CIniFileI	pr( g_szIniFileName, _T("SEARCH") );
	//DWORD 	jikken = pr.GetValue("Jikken");
	m_dwTinyWordButton = pr.GetValue(_T("NumberButton"));		//+++ 単語ボタンでなく、数字だけの5ボタンを使うか? (NoWordButton=1の時のみ有効)
	pr.Close();

	/* ツールバーを作る */
	DWORD		flags  =  WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
						| CCS_NODIVIDER  /*| CCS_NORESIZE */ | CCS_NOPARENTALIGN | CCS_TOP
						| TBSTYLE_TOOLTIPS | TBSTYLE_FLAT ;
	m_wndToolBar.Create(m_hWnd, rcDefault, _T("SearchWordButton"), flags);

	//+++ どうも古いosでは、CreateWindow時に設定したらまずいらしい?...ので後づけでSetWindowLongする必要がある?... 関係なかった模様.
	if (s_bNoWordButton == 0 || m_dwTinyWordButton)
	{
		flags	|= (UINT)m_wndToolBar.GetWindowLong(GWL_STYLE);
		flags	|= TBSTYLE_LIST | TBSTYLE_TRANSPARENT;	//+++ 追加 (検索単語の文字列を表示するには必要)
		m_wndToolBar.SetWindowLong(GWL_STYLE, flags);
	}

	m_wndToolBar.SetButtonStructSize();
	m_wndToolBar.SetImageList(imgs);
	m_wndToolBar.SetHotImageList(imgsHot);
	m_wndToolBar.SetDisabledImageList(imgsDis); //+++

	if (s_bNoWordButton) {
		addDefaultToolBarIcon(0);		//+++ 単語ボタン無しモードでの、ボタン設定.
	} else {
		toolBarAddButtons();			//+++ ツールバーにボタンを追加.(単語ボタンアリの場合)
	}

	m_bShowToolBar	 = TRUE;
	m_bLoadedToolBar = TRUE;

  #if 0	//+++ この関数抜けたあとで、調整入るのでここでしちゃだめ
	ShowToolBarIcon(true);
  #endif
}

void	CDonutSearchBar::_RefreshBandInfo(int nHeight)
{
	HWND		  hWndReBar = GetParent();
	CReBarCtrl	  rebar(hWndReBar);
	REBARBANDINFO rbBand = { sizeof (REBARBANDINFO) };
	rbBand.fMask  = RBBIM_CHILDSIZE;

	int nIndex	= rebar.IdToIndex( GetDlgCtrlID() );
	rebar.GetBandInfo(nIndex, &rbBand);

	if ( rbBand.cyMinChild != nHeight ) {
		// Calculate the size of the band
		rbBand.cxMinChild = 0;
		rbBand.cyMinChild = nHeight;

		rebar.SetBandInfo(nIndex, &rbBand);
	}
}




LRESULT CDonutSearchBar::OnToolTipText(int idCtrl, LPNMHDR pnmh, BOOL & /*bHandled*/)
{
	LPNMTTDISPINFO pDispInfo  = (LPNMTTDISPINFO) pnmh;

	pDispInfo->szText[0] = 0;
	if (pDispInfo->uFlags & TTF_IDISHWND)
		return S_OK;

  //#if 1		//+++
  //  CString 	   strKeyword = GetSearchStr();
  //#else
  //  CString 	   strKeyword = MtlGetWindowText(m_wndKeyword);
  //#endif
	CString 	   strHeader  = _T('\"');
	CString 	   strHelp	  = _T("\" を");
	CString 	   strEngine  = GetSearchEngineStr();	// MtlGetWindowText(m_wndEngine);

	TCHAR		   szBuff[80+4];
	szBuff[0]	= 0;
	AtlCompactPathFixed( szBuff, GetSearchStr()/*strKeyword*/, 80 - strHelp.GetLength() - strHeader.GetLength() );

	strHeader	+= szBuff;
	strHeader	+= strHelp;

	switch (idCtrl) {
	case ID_SEARCH_WEB:
		strHeader += _T("\"") + strEngine + _T("\"で検索する");
		break;
	case ID_SEARCHBAR_HILIGHT:
		strHeader += _T("ハイライトする");
		break;
	case ID_SEARCH_PAGE:
		strHeader += _T("このページで検索する(↑:Shift+Enter ↓:Ctrl+Enter)");
		break;
	default:	// ID_SEARCH_WORD00..09
		return S_OK;
	}

	::lstrcpyn(pDispInfo->szText, strHeader, 80);

	return S_OK;
}



void CDonutSearchBar::OnCommand(UINT uFlag, int nID, HWND hWndCtrl)
{
	if (hWndCtrl == m_wndToolBar.m_hWnd) {
	  //#if 1	//+++
	  //	CString str  = GetSearchStr();
	  //#else
	  //	CEdit	edit = GetEditCtrl();
	  //	CString str  = MtlGetWindowText(edit);
	  //#endif
		switch (nID) {
		case ID_SEARCH_WEB:
		  #if 1
			SearchWeb();
		  #else
			_OnCommand_SearchWeb( GetSearchStr() /*str*/ );
		  #endif
			break;
		case ID_SEARCHBAR_HILIGHT:	//ID_SEARCH_HILIGHT:
			//GetHilightBtnFlag();
			//m_bHilightSw^=1;
		  #if 1	//+++
			SearchHilight();
		  #else
			//_OnCommand_SearchHilight( str);
		  #endif
			break;
		case ID_SEARCH_PAGE:
			_OnCommand_SearchPage( (::GetKeyState(VK_SHIFT) >= 0));
			break;
		case ID_SEARCHBAR_WORD00:
		case ID_SEARCHBAR_WORD01:
		case ID_SEARCHBAR_WORD02:
		case ID_SEARCHBAR_WORD03:
		case ID_SEARCHBAR_WORD04:
		case ID_SEARCHBAR_WORD05:
		case ID_SEARCHBAR_WORD06:
		case ID_SEARCHBAR_WORD07:
		case ID_SEARCHBAR_WORD08:
		case ID_SEARCHBAR_WORD09:
		case ID_SEARCHBAR_WORD10:
		case ID_SEARCHBAR_WORD11:
		case ID_SEARCHBAR_WORD12:
		case ID_SEARCHBAR_WORD13:
		case ID_SEARCHBAR_WORD14:
		case ID_SEARCHBAR_WORD15:
		case ID_SEARCHBAR_WORD16:
		case ID_SEARCHBAR_WORD17:
		case ID_SEARCHBAR_WORD18:
		case ID_SEARCHBAR_WORD19:
			_OnCommand_SearchPage((::GetKeyState(VK_SHIFT) >= 0), nID-ID_SEARCHBAR_WORD00);
			break;

		default:
			ATLASSERT(0);
		}

		SetMsgHandled(TRUE);
		return;
	}

	SetMsgHandled(FALSE);
}




/** ハイライトボタンを強制的に設定する
 */
bool CDonutSearchBar::ForceSetHilightBtnOn(bool sw)
{
	bool rc = (BOOL(sw) != m_bHilightSw);
	m_bHilightSw = sw;
	int hilightStat = m_bHilightSw ? TBSTATE_PRESSED : TBSTATE_ENABLED;
	m_wndToolBar.SetButtonInfo(ID_SEARCHBAR_HILIGHT, TBIF_STATE, 0, hilightStat, 0, 0, 0, 0, 0);
	return rc;
}



//+++
void CDonutSearchBar::OnSearchWeb_engineId(UINT code, int id, HWND hWnd)
{
	ATLASSERT(ID_INSERTPOINT_SEARCHENGINE <= id && id <= ID_INSERTPOINT_SEARCHENGINE_END);
	unsigned n = id - ID_INSERTPOINT_SEARCHENGINE;
	CString 	strEngine;
	MtlGetLBTextFixed(m_cmbEngine, n, strEngine);
	//\\CString str = GetSearchStr();
	CString str = Donut_GetActiveSelectedText();	//\\+ また別でバグるかも
	SearchWeb_str_engine(str, strEngine);
}



void CDonutSearchBar::SearchWeb()
{
  #if 1	//+++
	CString str = GetSearchStr();
	_OnCommand_SearchWeb( str );
  #endif
}


void	CDonutSearchBar::_OnCommand_SearchWeb(CString &str)
{
	int nTarCate = m_cmbEngine.GetCurSel();
	if (nTarCate == -1) return;

	SearchWeb_str_engine(str, GetSearchEngineStr());
}


//+++ エンジンの選択をできるように_OnCommand_SearchWebの実体を分離.
void	CDonutSearchBar::SearchWeb_str_engine(CString &str, const CString&	strSearchEng)
{
  #if 1
	//x CString 	strSearchEng = MtlGetWindowText(m_cmbEngine);
	bool bUrlSearch = false;
	if (::GetFocus() != m_wndKeyword.m_hWnd) {
	   #if 0//\\+	//+++ 強引なので、あとで、仕組みを直す
		// 選択範囲があれば、それを優先する.
		CString strSel = Donut_GetActiveSelectedText();
		if (strSel.IsEmpty() == 0) {
			str = strSel;
		}
	   #endif
	   #if 1 //+++ v1.48c で変更.
		if (strSearchEng.IsEmpty())	//サーチエンジン名がカラなら検索しない.
			return;
		//+++ addressbarの文字列を使う？
		{
			CIniFileI		pr(GetSearchIniPath(), strSearchEng);
			DWORD			exPropOpt = pr.GetValue(_T("ExPropOpt"), 0);
			pr.Close();
			if (exPropOpt & 1) {	//+++ addressbarの文字を取ってくる場合.
				if (str.IsEmpty()	//+++ 検索文字列が空の場合.
					|| (_tcsncmp(LPCTSTR(str), _T("http:"),5) != 0 && _tcsncmp(LPCTSTR(str), _T("https:"), 6) != 0)	//+++ 検索文字列がすでにurlならそれを用いるので、ここでは省く.
				){
					CString strUrl = GetAddressBarText();
					if (strUrl.IsEmpty() == 0) {
						str 	  = strUrl;
						bUrlSearch = true;
					}
				}
			}
		}
	  #endif
	}
  #endif

	GetHilightBtnFlag();				//+++ ハイライトボタンの状態チェック
	toolBarAddButtons(true/*str*/);			//+++ サーチのツールバー部に単語を設定

	if ( !str.IsEmpty() ) {
	  #if 1 //+++	OnItemSelectedを分解したのでOpenSearchを呼ぶように変更.
		BOOL	bFirst		 = TRUE;
		int 	nLoopCtn	 = 0;
		OpenSearch(str, strSearchEng, nLoopCtn, bFirst);
	  #elif 1 //+++	templateをやめたのでthisでいい.
		this->OnItemSelected(str, strSearchEng);
	  #else
		T *pT = static_cast<T *>(this);
		pT->OnItemSelected(str, strSearchEng);
	  #endif
		if (bUrlSearch == false)		//+++ url検索だった場合は、履歴に入れないで置く.
			_AddToSearchBoxUnique(str);
	}

  #if 1	//+++ 不要かもだが...テスト的に...
	//+++ 表示→ツールバー→検索バーボタンをonの時のみ
	ShowToolBarIcon(m_bShowToolBar/*true*/);
  #endif
}



// ハイライトボタンを押したとき
void CDonutSearchBar::_OnCommand_SearchHilight(CString &str)
{
	GetHilightBtnFlag();
	m_bHilightSw ^= 1;

	checkToolBarWords();	//+++
//toolBarAddButtons(false);	//+++ ためし...

	str = RemoveShortcutWord(str);
	if (s_bFiltering) {
		FilterString(str);
	}

  #if 0	//+++ ここで、はじくと、ハイライトボタンのonが瞬間でおわってoffに戻るので、メッセージは投げておく
	if (str.IsEmpty()) return;
  #endif
	SendMessage(GetTopLevelParent(), WM_USER_HILIGHT, (WPARAM) LPCTSTR(str)/*str.GetBuffer(0)*/, 0);
	GetHilightBtnFlag();				//+++ ハイライトボタンの状態チェック
}



void CDonutSearchBar::_OnCommand_SearchPage(BOOL bForward, int no /*=-1*/)
{
	GetHilightBtnFlag();				//+++ ハイライトボタンの状態チェック
	checkToolBarWords();				//+++

	CEdit	edit	= GetEditCtrl();
  #if 1 //+++ カーソル位置の単語だけを選択するようにしてみる.
	CString str;
	if (no >= 0) {
		str 	= _GetSelectText(edit);
		//引用符・ショートカットワードは除外
		str = RemoveShortcutWord(str);
		if (s_bFiltering)
			FilterString(str);
		std::vector<CString> strs;
		strs.reserve(10);
		Misc::SeptTextToWords(strs, str);
		if (size_t(no) < strs.size())
			str = strs[no];
	} else {
		str 	= _GetSelectText_OnCursor(edit);
		//引用符・ショートカットワードは除外
		str = RemoveShortcutWord(str);
	}
  #else
	CString str 	= _GetSelectText(edit);
	//引用符・ショートカットワードは除外
	str = RemoveShortcutWord(str);
  #endif

	CString strExcept = _T(" \t\"\r\n　");
	str.TrimLeft(strExcept);
	str.TrimRight(strExcept);
	if (s_bFiltering)
		FilterString(str);

	SendMessage(GetTopLevelParent(), WM_USER_FIND_KEYWORD, (WPARAM) str.GetBuffer(0), (LPARAM) bForward);
}




//public
const CString CDonutSearchBar::RemoveShortcutWord(const CString &str)
{
	if (s_bUseShortcut) {
		if (str.Find( _T("\\") ) == 0 || str.Find( _T("/") ) == 0) {
			int nPos = str.Find( _T(" ") );

			if (nPos != -1)
				return str.Mid(nPos + 1);
		}
	}

	return str;
}



///+++
int CDonutSearchBar::btnWidth() const
{
  #if 1
	int  btnW = ::GetSystemMetrics(SM_CXHTHUMB);
	if (m_bExistManifest == 0)
		btnW += 2 * 2;
	else
		btnW += 2;
	return btnW;
  #else
	return 21;
  #endif
}



///+++ キーワード入力欄が空の時にエンジン名を表示するための設定
void	CDonutSearchBar::SetCmbKeywordEmptyStr()
{
	m_cmbKeyword.setEmptyStr(GetSearchEngineStr(), IDC_EDIT/*1001*/, (m_nEngineWidth <= 8 + btnWidth()));
	m_cmbKeyword.redrawEmptyStr();
}



// private:
LRESULT CDonutSearchBar::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  #ifndef USE_DIET
	//+++ 検索単語を項目にするため、検索バーのサイズ調整方法を変更.
	if (s_bNoWordButton == false) { //+++ 単語ボタンありのとき
		return ResizeBar(0,0);
	} else {
		//+++ 単語ボタン無しの場合は旧処理.

		//if (!m_wndToolBar.m_hWnd)
		//	return S_FALSE;
		if (!m_wndKeyword.m_hWnd)
			return S_FALSE;

		CRect	rcDlg;
		GetClientRect(&rcDlg);

		CRect	rcToolbar(rcDlg);
		CString str;

		if (rcDlg.right == 0)
			return S_FALSE;

		if ( GetToolIconState() && m_wndToolBar.m_hWnd ) {
			rcToolbar.left = rcDlg.right - m_cxBtnsBtn - 10;
			m_wndToolBar.SetWindowPos(NULL, rcToolbar, SWP_NOZORDER | SWP_NOSENDCHANGING);
		} else {
			//非表示
			rcToolbar.left = rcDlg.right;
		}

		CRect	rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		int 	nEngineCX = rcEngine.Width();
		int 	nEngineCY = rcEngine.Height();
		if (nEngineCX <= btnWidth())	//+++
			nEngineCX = btnWidth();
		m_nKeywordWidth	 = nEngineCX;
		rcEngine		 = rcToolbar;
		rcEngine.right	 = rcToolbar.left - s_kcxGap;
		rcEngine.left	 = rcEngine.right - nEngineCX;
		rcEngine.top	 = m_nDefEditT; //minit
		rcEngine.bottom  = rcEngine.top + nEngineCY;

		m_cmbEngine.SetWindowPos(NULL, rcEngine, SWP_NOZORDER | SWP_NOSENDCHANGING);

		CRect	rcKeyword(rcEngine);
		rcKeyword.left	= 0;
		rcKeyword.right = rcEngine.left - s_kcxGap;
		m_cmbKeyword.SetWindowPos(NULL, rcKeyword, SWP_NOZORDER | SWP_NOSENDCHANGING);
	  #if 1	//+++ 暫定対処... 本質的に修正できていないがon_
		m_cmbKeyword.SetEditSel(0,0);	//+++ 無理やり範囲解除
	  #endif
		SetCmbKeywordEmptyStr();	//+++

		m_wndToolBar.InvalidateRect(NULL, TRUE);

		return S_OK;
	}
  #else	// USE_DIET時は 単語ボタンのみ
	return ResizeBar(0,0);
  #endif
}



///+++ バーのサイズ変更. キーワード｜エンジン｜ツールバー  の境目での移動幅を順にdifA,difBとする.
int CDonutSearchBar::ResizeBar(int difA, int difB)
{
	if (!m_wndKeyword.m_hWnd) {
		return S_FALSE;
	}

	CRect	rcDlg;
	GetClientRect(&rcDlg);
	if (rcDlg.right == 0) {
		return S_FALSE;
	}

	//int	h = rcDlg.Height();

	CRect	rcKeyword;
	m_cmbKeyword.GetWindowRect(&rcKeyword);
	m_nKeywordWidth = rcKeyword.Width();
	int 		h	= rcKeyword.Height();
	m_nKeywordWidth += difA;
	if (m_nKeywordWidth <= btnWidth())
		m_nKeywordWidth = btnWidth();
	rcKeyword.left	= 0;
	rcKeyword.top	= m_nDefEditT;
	rcKeyword.right = m_nKeywordWidth;
	rcKeyword.bottom= rcKeyword.top + h;
	m_cmbKeyword.SetWindowPos(NULL, rcKeyword, SWP_NOZORDER | SWP_NOSENDCHANGING);


	CRect	rcEngine;
	m_cmbEngine.GetWindowRect(&rcEngine);
	h			  = rcEngine.Height();
	m_nEngineWidth = rcEngine.Width();
	m_nEngineWidth += difB;
	if (m_nEngineWidth <= btnWidth()) {
		m_nEngineWidth = btnWidth();
	}
	rcEngine.left	 = rcKeyword.right + s_kcxGap;
	rcEngine.top	 = m_nDefEditT;
	rcEngine.right	 = rcEngine.left	+ m_nEngineWidth;
	rcEngine.bottom  = rcEngine.top + h;
	m_cmbEngine.SetWindowPos(NULL, rcEngine, SWP_NOZORDER | SWP_NOSENDCHANGING);

	CRect	rcToolbar(rcDlg);
	rcToolbar.left = rcEngine.right + s_kcxGap;
	if (rcToolbar.left > rcToolbar.right)
		rcToolbar.left = rcToolbar.right;
	if ( GetToolIconState() ) {
	} else {
		//非表示
		rcToolbar.left = rcDlg.right;
	}
	if (m_wndToolBar.m_hWnd)
		m_wndToolBar.SetWindowPos(NULL, rcToolbar, SWP_NOZORDER | SWP_NOSENDCHANGING);

	m_wndToolBar.InvalidateRect(NULL, TRUE);

	return S_OK;
}


#if 1	//test
bool	CDonutSearchBar::checkEngineNameEnable()
{
	if (::IsWindow(m_hWnd) == 0)
		return false;
	if (::IsWindow(m_cmbKeyword.m_hWnd) == 0)
		return false;

	if (::GetFocus() == GetEditCtrl())
		return false;

	CRect	rcEngine;
	m_cmbEngine.GetWindowRect(&rcEngine);
	int engineW		= rcEngine.Width();
	if (engineW > btnWidth())
		return false;

	CRect	rcKeyword;
	m_cmbKeyword.GetWindowRect(&rcKeyword);
	int keywordW	= rcKeyword.Width();
	if (keywordW <= btnWidth())
		return false;

	//CPaintDC dc(m_cmbKeyword.m_hWnd);
	//dc.TextOut(rcKeyword.left, rcKeyword.top, "Test");
	//dc.TextOut(0, 0, "ABCDEF");

	return true;
}
#endif


void	CDonutSearchBar::checkToolBarWords()	//+++
{
	if (toolBarAddButtons(true)) {
	  #if 1	//+++ 不要かもだが...テスト的に...
		ShowToolBarIcon(m_bShowToolBar/*true*/);	//+++ trueだとボタン消してるときに不味い
	  #endif
	}
}



//+++ 検索ワードの文字列を分解して、検索ツールバーに追加
bool CDonutSearchBar::toolBarAddButtons(bool chk)
{
	if (s_bNoWordButton && m_dwTinyWordButton == 0)	//単語ボタンを表示しない場合は直帰.
		return 0;

	CEdit	edit	= GetEditCtrl();

	CString str		= _GetSelectText(edit);
	//引用符・ショートカットワードは除外
	str = RemoveShortcutWord(str);
	if (s_bFiltering) {
		FilterString(str);
	}

	if (chk && m_toolBarAddWordStr == str) {
		return ForceSetHilightBtnOn(m_bHilightSw != 0);	//+++ ハイライトボタンのon/offは変わっている可能性があるので更新.
	}
	m_toolBarAddWordStr = str;

	std::vector<CString> strs;
	strs.reserve(20);
	Misc::SeptTextToWords(strs, str);

	if (m_wndToolBar.m_hWnd == 0) return 0;

	//+++ 検索ツールバーの中身を一旦すべて削除
	unsigned num = m_wndToolBar.GetButtonCount();
	for (int i = num; --i >= 0;) {
		m_wndToolBar.DeleteButton(i);
	}

	addDefaultToolBarIcon((UINT)strs.size());		//+++ デフォルトのアイコンを追加

	if (s_bNoWordButton == false) {			//+++ 数字ボタンの時はワードボタンを登録させない.
		toolBarAddWordTbl(strs);
	}

	return true;
}



//+++  デフォルトのアイコンを追加
int  CDonutSearchBar::addDefaultToolBarIcon(unsigned n)
{
	int hilightStat = m_bHilightSw ? TBSTATE_PRESSED : TBSTATE_ENABLED;
	if (n == 0 || s_bNoWordButton || s_bUsePageButton) {	//+++ 旧来の検索 (単独のページ内検索ボタンあり)
		if (s_bNoWordButton && m_dwTinyWordButton) {
			return addDefaultToolBarIcon_tinyWordButton(n);
		}
		//static
		static const TBBUTTON	btns[] = {
			{ 0 , ID_SEARCH_WEB,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE	, 0, 0 },// Web
			{ 2 , ID_SEARCH_PAGE,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE	, 0, 0 },// Page
			{ 1 , ID_SEARCHBAR_HILIGHT, hilightStat    , TBSTYLE_CHECK	| TBSTYLE_AUTOSIZE	, 0, 0 },// Hilight
		};
		//m_nHilightBtnNo = 2;
	  #if 0	//+++ 本来はこっちでいい
		MTLVERIFY( m_wndToolBar.AddButtons(3, (TBBUTTON *) btns ) );
	  #else	//+++ テスト
		for (unsigned i = 0; i < 3; ++i)
			MTLVERIFY( m_wndToolBar.AddButton( (TBBUTTON *) &btns[i] ) );
	  #endif
		m_wndToolBar.SetBitmapSize(14,14);
		if (s_bNoWordButton) {
			m_cxBtnsBtn 	 = (20) * 3 + 1;
		}
		return 3;
	} else {				//+++ 単語専用ボタン有り
		//static
		static const TBBUTTON	btns[] = {
			{ 0 , ID_SEARCH_WEB,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE	, 0, 0 },// Web
			{ 1 , ID_SEARCHBAR_HILIGHT, hilightStat    , TBSTYLE_CHECK	| TBSTYLE_AUTOSIZE	, 0, 0 },// Hilight
		};
		//m_nHilightBtnNo = 1;
	  #if 0	//+++ 本来はこっちでいい
		MTLVERIFY( m_wndToolBar.AddButtons(2, (TBBUTTON *) btns ) );
	  #else	//+++ テスト
		for (unsigned i = 0; i < 2; ++i)
			MTLVERIFY( m_wndToolBar.AddButton( (TBBUTTON *) &btns[i] ) );
	  #endif
		return 2;
	}
}



#ifndef USE_DIET
//+++ 単語ボタンの代わりに1～5の数字ボタンを設定.
//		※単語ボタンが実装されるまでの実験/代用処理だったもの.
//		  単語ボタンを使用しない設定で、かつ、noguiに"NumberButton=1"を設定したときのみ利用可能.
int	CDonutSearchBar::addDefaultToolBarIcon_tinyWordButton(unsigned n)
{
	if (n > 5)
		n = 5;
	int hilightStat = m_bHilightSw ? TBSTATE_PRESSED : TBSTATE_ENABLED;
	//static
	static const TBBUTTON	btns[3] = {
		{ 0 		, ID_SEARCH_WEB,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE	, 0, 0 },	// Web
		{ 2 		, ID_SEARCH_PAGE,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE	, 0, 0 },	// Page
		{ 1 		, ID_SEARCHBAR_HILIGHT, hilightStat	   , TBSTYLE_CHECK	| TBSTYLE_AUTOSIZE	, 0, 0 },	// Hilight
	};
	//m_nHilightBtnNo = 2;
  #if 0	//+++ 本来はこっちでいい
	MTLVERIFY( m_wndToolBar.AddButtons(3, (TBBUTTON *) btns ) );
  #else	//+++ テスト
	for (unsigned i = 0; i < 3; ++i)
		MTLVERIFY( m_wndToolBar.AddButton( (TBBUTTON *) &btns[i] ) );
  #endif
	m_cxBtnsBtn	 = (20) * (3+5) + 1;

	enum { STYLE = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE }; // | TBSTYLE_LIST | TBSTYLE_TRANSPARENT };
	for (unsigned i = 0; i < 5; ++i) {
		const TBBUTTON	btn = { I_IMAGENONE, ID_SEARCHBAR_WORD00+i, (i < n) ? TBSTATE_ENABLED : 0/*TBSTATE_CHECKED*/, STYLE, 0, 0 };
		MTLVERIFY( m_wndToolBar.AddButton( (TBBUTTON *)&btn) );

		CVersional<TBBUTTONINFO> bi;
		bi.cbSize	= sizeof(TBBUTTONINFO);
		bi.dwMask	= TBIF_TEXT	/* | TBIF_STYLE*/;
		bi.fsStyle |= TBSTYLE_AUTOSIZE /*| TBBS_NOPREFIX*/ ;
		TCHAR str[4];
		str[0]		= _T('1'+i);
		str[1]		= _T('\0');
		bi.pszText	= str;
		MTLVERIFY( m_wndToolBar.SetButtonInfo(ID_SEARCHBAR_WORD00+i, &bi) );
	}

	// サイズ調整など
	m_wndToolBar.SetMaxTextRows(1);
	CRect rcButton;
	m_wndToolBar.GetItemRect(3, rcButton);
	m_wndToolBar.SetButtonSize(rcButton.Size());
	m_wndToolBar.SetButtonSize(rcButton.Size());
	m_wndToolBar.Invalidate();

	m_wndToolBar.AutoSize();
	return 3;
}
#endif



//+++ vectorで渡された複数の文字列(20個まで)を検索ツールバーに登録.
void CDonutSearchBar::toolBarAddWordTbl(std::vector<CString>& tbl)
{
	TBBUTTONINFO	bi = { sizeof(TBBUTTONINFO) };
	bi.dwMask	= TBIF_TEXT/* | TBIF_STYLE*/;
	bi.fsStyle |= TBSTYLE_AUTOSIZE | TBSTYLE_NOPREFIX;

	unsigned n	= unsigned(tbl.size());
	if (n > 20) n = 20;
	for (unsigned i = 0; i < n; ++i) {
		enum { STYLE = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | TBSTYLE_NOPREFIX };
		// | TBSTYLE_LIST | TBSTYLE_TRANSPARENT };
		TBBUTTON	btn = { 2/*I_IMAGENONE*/, ID_SEARCHBAR_WORD00 +i, TBSTATE_ENABLED, STYLE, 0, 0 };
		MTLVERIFY( m_wndToolBar.AddButton( (TBBUTTON *)&btn) );

		bi.pszText = (LPTSTR) LPCTSTR(tbl[i]);
		MTLVERIFY( m_wndToolBar.SetButtonInfo(ID_SEARCHBAR_WORD00+i, &bi) );

		//m_wndToolBar.AutoSize();
		//CRect rcButton;
		//m_wndToolBar.GetItemRect(TOP+i, rcButton);
		//m_wndToolBar.SetButtonSize(rcButton.Size());
	}

	m_wndToolBar.SetButtonSize(m_ButtonSize);
//	m_wndToolBar.AutoSize();
	m_wndToolBar.Invalidate();
	//ShowToolBarIcon(true);
}



CString 	CDonutSearchBar::GetSkinSeachBarPath(int mode/*BOOL bHot*/)
{
	ATLASSERT(mode >= 0 && mode <= 2);
	static const TCHAR* tbl[] = {
		_T("SearchBar.bmp"),
		_T("SearchBarHot.bmp"),
		_T("SearchBarDis.bmp"),
	};
	return _GetSkinDir() + tbl[ mode ];
}


int		CDonutSearchBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_cmbKeyword.Create(m_hWnd);
	m_cmbKeyword.SetDlgCtrlID(IDC_CMB_KEYWORD);
	m_cmbEngine.Create(m_hWnd);
	m_cmbEngine.SetDlgCtrlID(IDC_CMB_ENGIN);

//	CRect		rcDlg;
//	GetWindowRect(&rcDlg);
//	SetWindowPos(NULL, 0, 0, 0, m_nDefDlgH , 0);

	// 設定を読み込む
	CIniFileI	pr( g_szIniFileName, _T("SEARCH") );
	m_nKeywordWidth 	 = pr.GetValue( _T("KeywordWidth" ), 150 );
	m_nEngineWidth		 = pr.GetValue( _T("EngWidth"	  ), 150 );
	pr.Close();

	//コンボボックス初期化
//	m_cmbKeyword.FlatComboBox_Install( cmbkeyword/*GetDlgItem(IDC_CMB_KEYWORD)*/ );
//	m_cmbEngine.FlatComboBox_Install  ( GetDlgItem(IDC_CMB_ENGIN  ) );
//	m_cmbEngine.SetDroppedWidth(150);
	if (s_bSearchListWidth) {
		m_cmbEngine.SetDroppedWidth(s_nSearchListWidth);
	}

	_InitCombo();

	// 幅を設定
	if (m_nKeywordWidth != 0) {
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		int 	h		 = rcKeyword.Height();
		rcKeyword.left	 = 0;
		rcKeyword.right  = m_nKeywordWidth;
		rcKeyword.top	 = 0;
		rcKeyword.bottom = h;
		m_cmbKeyword.SetWindowPos(NULL, rcKeyword, SWP_NOZORDER | SWP_NOSENDCHANGING);
	}
	if (m_nEngineWidth != 0) {
		CRect	rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		int 	h		 = rcEngine.Height();
		rcEngine.left	= 0;
		rcEngine.right	= m_nEngineWidth;
		rcEngine.top	= 0;
		rcEngine.bottom = h;
		m_cmbEngine.SetWindowPos(NULL, rcEngine, SWP_NOZORDER | SWP_NOSENDCHANGING);
	}

	m_wndKeywordCombo.SubclassWindow(m_cmbKeyword.m_hWnd); //minit
	m_wndKeyword.SubclassWindow( m_cmbKeyword.GetDlgItem(IDC_EDIT/*1001*/) );
	m_wndEngine.SubclassWindow(m_cmbEngine.m_hWnd);


	//ツールバー初期化
	{
		DWORD	dwShowToolBar = STB_SHOWTOOLBARICON;
		CIniFileI		pr( g_szIniFileName, _T("SEARCH") );
		pr.QueryValue( dwShowToolBar, _T("Show_ToolBarIcon") );
		pr.Close();
		m_bShowToolBar	= (dwShowToolBar & STB_SHOWTOOLBARICON) == STB_SHOWTOOLBARICON;
	}

  #if 1 //+++ 無理やり全部初期化(メモリーの無駄だが安全&見栄え優先)
	_InitToolBar(m_nBmpCX, m_nBmpCY, RGB(255, 0, 255));
	ShowToolBarIcon(m_bShowToolBar);
  #else //+++ 元の処理。表示しなけりゃメモリー等お得。だが、途中で設定した場合font反映なし.
	if (m_bShowToolBar) {
		_InitToolBar( m_nBmpCX, m_nBmpCY, RGB(255, 0, 255) );
	}
  #endif

	//ドラッグドロップ初期化
	RegisterDragDrop();

	//スレッドを利用してコンボボックスにデータを登録(INIからの読み込みに時間がかかるため)
	// Thread Create
	m_tdInitComboBox	= boost::thread(boost::bind(&CDonutSearchBar::_SearchThread, this));

	// SetCmbKeywordEmptyStr();	//+++ どうせまだエンジン名が登録されていないので、このタイミングはなし

	return 0;
}

void CDonutSearchBar::OnDestroy()
{
	// Thread Remove
	m_tdInitComboBox.join();

  #if 0
	CRect			rcKeyword;
	m_wndKeyword.GetWindowRect(rcKeyword);
	DWORD			keywordW = rcKeyword.Width();
	CRect			rcEngine;
	m_wndEngine.GetWindowRect(rcEngine);
	DWORD			enginW	 = rcEngine.Width();
  #else

	if (s_bNoWordButton) { //+++ 旧処理のときの辻褄あわせ
		CRect			rcEngine;
		m_wndEngine.GetWindowRect(rcEngine);
		m_nEngineWidth = rcEngine.Width();
	}
  #endif

	{
		CIniFileIO	pr( g_szIniFileName, _T("SEARCH") );
		pr.SetValue( (DWORD) m_nKeywordWidth, _T("KeywordWidth") );
		pr.SetValue( (DWORD) m_nEngineWidth  , _T("EngWidth") );

		DWORD	dwShowToolBar = m_bShowToolBar ? STB_SHOWTOOLBARICON : 0;
		pr.SetValue( dwShowToolBar, _T("Show_ToolBarIcon") );

		if (s_bLastSel) {
			pr.SetValue( m_cmbEngine.GetCurSel(), _T("SelIndex") );
		}
	}

	m_wndKeyword.UnsubclassWindow();
	m_wndEngine.UnsubclassWindow();
	m_wndKeywordCombo.UnsubclassWindow();

	RevokeDragDrop();

	SaveHistory();
}



//スレッドを利用してコンボボックスにデータを登録(INIからの読み込みに時間がかかるため)
void	CDonutSearchBar::_SearchThread()
{
	_InitialEngine( m_cmbEngine.m_hWnd	);
	_InitialKeyword( m_cmbKeyword.m_hWnd );
}


//private:


void CDonutSearchBar::OnEngineKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN) {
		_OnEnterKeyDown(ENTER_KEYDOWN_RETURN);
		SetMsgHandled(TRUE);
	} else if (nChar == VK_TAB) {
		m_cmbKeyword.SetFocus();
		SetMsgHandled(TRUE);
	} else {
		SetMsgHandled(FALSE);
	}
}



void CDonutSearchBar::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN) {
		SetMsgHandled(TRUE);
	} else {
		SetMsgHandled(FALSE);
	}
	//checkToolBarWords();	//+++
}

void CDonutSearchBar::OnKeywordKillFocus(CWindow wndFocus)
{
	SetMsgHandled(FALSE);
	/* ツールバーを更新する */
	checkToolBarWords();
}


LRESULT CDonutSearchBar::OnCtlColorListBox(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled		  = FALSE;
	m_hWndKeywordList = (HWND) lParam;
	return 0;
}



void CDonutSearchBar::OnKeywordKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// if not dropped, eat VK_DOWN
	if ( !m_cmbKeyword.GetDroppedState() && (nChar == VK_DOWN || nChar == VK_UP) ) {
		int nIndexEngine  = m_cmbEngine.GetCurSel();
		int nIndexKeyword = m_cmbKeyword.GetCurSel();

		SetMsgHandled(TRUE);

		if (nChar == VK_UP) {
			if (::GetKeyState(VK_CONTROL) < 0) {
				if (0 > nIndexEngine - 1)
					m_cmbEngine.SetCurSel(m_cmbEngine.GetCount() - 1);
				else
					m_cmbEngine.SetCurSel(nIndexEngine - 1);
				//SetCmbKeywordEmptyStr();	//+++
			} else {
				if (0 > nIndexKeyword - 1)
					m_cmbKeyword.SetCurSel(m_cmbKeyword.GetCount() - 1);
				else
					m_cmbKeyword.SetCurSel(nIndexKeyword - 1);
			}

		} else if (nChar == VK_DOWN) {
			if (::GetKeyState(VK_CONTROL) < 0) {
				int nIndex = m_cmbEngine.GetCurSel();

				if (m_cmbEngine.GetCount() > nIndexEngine + 1)
					m_cmbEngine.SetCurSel(nIndexEngine + 1);
				else
					m_cmbEngine.SetCurSel(0);
				//SetCmbKeywordEmptyStr();	//+++
			} else {
				if (m_cmbKeyword.GetCount() > nIndexKeyword + 1)
					m_cmbKeyword.SetCurSel(nIndexKeyword + 1);
				else
					m_cmbKeyword.SetCurSel(0);
			}
		}
	} else {
		if (nChar == VK_RETURN) {
			_OnEnterKeyDown(ENTER_KEYDOWN_RETURN);
			SetMsgHandled(TRUE);
		} else if (nChar == VK_DELETE) {
			if ( m_cmbKeyword.GetDroppedState() ) {
				if ( DeleteKeywordHistory() )
					SetMsgHandled(TRUE);
			} else
				SetMsgHandled(FALSE);
		} else if (nChar == VK_TAB) {
			m_cmbEngine.SetFocus();
			SetMsgHandled(TRUE);
		} else {
			SetMsgHandled(FALSE);
		}
	}

	//SetCmbKeywordEmptyStr();	//+++	PreTranseで毎度チェックするからここは無しに.
}



LRESULT CDonutSearchBar::OnKeywordCbnDropDown(LPNMHDR pnmh)
{
	if (m_cmbKeyword.GetCount() == 0)
		::MessageBox(NULL, _T(""), _T(""), MB_OK);

	return FALSE;
}



BOOL CDonutSearchBar::DeleteKeywordHistory()
{
	if ( !::IsWindow(m_hWndKeywordList) )
		return FALSE;

	CListBox List	= m_hWndKeywordList;
	int 	 nIndex = List.GetCurSel();

	if (nIndex == LB_ERR)
		return FALSE;

	m_cmbKeyword.DeleteString(nIndex);
	return TRUE;
}



void CDonutSearchBar::OnEngineRButtonUp(UINT nFlags, CPoint point)
{
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu( 0, 1, _T("拡張プロパティ") );
	menu.AppendMenu(MF_SEPARATOR);
	int			nIndex = 1;
	auto funcMakeSearchFileListMenu = [&nIndex, &menu](CString strFile) {
		nIndex++;
		menu.AppendMenu(0, nIndex, MtlGetFileName(strFile));
	};
	MtlForEachFileSortEx( Misc::GetExeDirectory() + _T("Search\\"), funcMakeSearchFileListMenu, _T("*.ini") );

	CPoint	pos;

	//GetCursorPos(&pos);
	ATLTRACE("mouseposition1 : left=%4d top=%4d", point.x, point.y);
	::ClientToScreen(m_cmbEngine.m_hWnd, &point);
	ATLTRACE("mouseposition2 : left=%4d top=%4d", point.x, point.y);

	int 	nRet	 = menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
							point.x,
							point.y,
							m_cmbEngine.m_hWnd);
	if (nRet == 0)
		return;

	if (nRet == 1) {	// 拡張プロパティ
		if (!m_cmbEngine.IsWindow() || m_cmbEngine.GetCurSel() == CB_ERR)
			return;
		CString 		  strText;
		m_cmbEngine.GetLBText(m_cmbEngine.GetCurSel(), strText);
		CExPropertyDialog dlg(GetSearchIniPath(), strText, 0);
		dlg.SetTitle(strText);
		dlg.DoModal();	// 検索エンジンの設定ダイアログを表示する
		return;
	}

	CString 		strTitle;
	menu.GetMenuString(nRet, strTitle, MF_BYCOMMAND);
	CString 		strPath = Misc::GetExeDirectory() + _T("Search\\") + strTitle;

	{
		CIniFileO	pr( g_szIniFileName, _T("Search") );
		pr.SetStringUW( strPath, _T("Path") );
	}

	RefreshEngine();
}



void	CDonutSearchBar::OnEngineSetFocus(HWND hWndBefore)
{
	SetMsgHandled(FALSE);
	::WINNLSEnableIME(m_cmbEngine, FALSE);

	SetCmbKeywordEmptyStr();	//+++
}



void	CDonutSearchBar::OnEngineKillFocus(HWND hWndNew)
{
	SetMsgHandled(FALSE);
	::WINNLSEnableIME(m_cmbEngine, TRUE);

	SetCmbKeywordEmptyStr();	//+++
}



void	CDonutSearchBar::OnMouseMove(UINT nFlags, const CPoint& pt)
{
	if (s_bNoWordButton) {
		//+++ 単語ボタン無しの場合は旧処理.
		CRect	rcKeyword;

		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);

		if ( abs(rcKeyword.right - pt.x) > 5 && ::GetCapture() != m_hWnd )
			return;

		if (m_hCursor == NULL)
			m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);

		::SetCursor(m_hCursor);

		if ( (nFlags & MK_LBUTTON) ) {
			UpdateLayout(pt);
		}
	} else {	//+++ 単語ボタンありの場合
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);
		if ((abs(rcKeyword.right - pt.x) <= 6 || (m_has == 1 && ::GetCapture() == m_hWnd))
			&& (rcKeyword.top <= pt.y && pt.y < rcKeyword.bottom) ) 
		{
			m_has = 1;
			if (m_hCursor == NULL)
				m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
			::SetCursor(m_hCursor);
			if ( (nFlags & MK_LBUTTON) ) {
				UpdateLayout(pt);
			}
			return;
		}

		CRect	rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		ScreenToClient(&rcEngine);
		if ( (abs(rcEngine.right - pt.x) <= 6 || (m_has == 2 && ::GetCapture() == m_hWnd))
			&& (rcEngine.top <= pt.y && pt.y < rcEngine.bottom) ) 
		{
			m_has = 2;
			if (m_hCursor == NULL)
				m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
			::SetCursor(m_hCursor);
			if ( (nFlags & MK_LBUTTON) ) {
				UpdateLayout2(pt);
			}
			return;
		}
		m_has = 0;
		return;
	}
}


#if 0
///+++ エンジン選択の改造実験... 失敗on_.
void CDonutSearchBar::OnSelDblClkForEngine(UINT code, int id, HWND hWnd)
{
	int 	nTarCate	 = m_cmbEngine.GetCurSel();
	if (nTarCate == -1)
		return;

	TCHAR	buf[4096];
	m_cmbEngine.GetLBText(nTarCate, buf);
	CString strSearchEng = buf;
	BOOL	bFirst		 = TRUE;
	int 	nLoopCtn	 = 0;

	CString  str;
	int nIndexCmb = m_cmbKeyword.GetCurSel();
	if (nIndexCmb == -1)
		str = MtlGetWindowText(m_cmbKeyword);
	else
		MtlGetLBTextFixed(m_cmbKeyword.m_hWnd, nIndexCmb, str);
	if ( !str.IsEmpty() )
		OpenSearch(str, strSearchEng, nLoopCtn, bFirst);
	return;
}
#endif



///+++ エンジン選択の改造実験... 失敗on_...だけど、ちょっとだけ有用.
void CDonutSearchBar::OnSelChangeForEngine(UINT code, int id, HWND hWnd)
{
	if (::GetKeyState(VK_RBUTTON) < 0) {	// 検索エンジンのプロパティを開く
		CString 			strText;
		m_cmbEngine.GetLBText(m_cmbEngine.GetCurSel(), strText);
		CExPropertyDialog	dlg(GetSearchIniPath(), strText, 0);
		dlg.SetTitle(strText);
		dlg.DoModal();
		SetCmbKeywordEmptyStr();	//+++
		return;
	}

	bool bSts = false;
	if (id == IDC_CMB_ENGIN) {
		bSts = s_bEngChgGo;
	}

	if (::GetKeyState(VK_SHIFT) < 0) bSts = !bSts;

	if (bSts) {
		//+++ エンジンが切り替わった場合は、検索窓が空でもurl検索の場合があるので、こちら
		//x _OnEnterKeyDown(ENTER_KEYDOWN_SELCHANGE);
		SearchWeb();
	}
}



void CDonutSearchBar::OnSelChange(UINT code, int id, HWND hWnd)
{
	bool bSts = false;
	//x if	(id == IDC_CMB_ENGIN)	 bSts = dwStatus & STS_ENG_CHANGE_GO; else		//+++ エンジンは別関数に.
	if (id == IDC_CMB_KEYWORD)		 bSts = s_bKeyChgGo;
	if (::GetKeyState(VK_SHIFT) < 0) bSts = !bSts;

	if (bSts) {
		_OnEnterKeyDown(ENTER_KEYDOWN_SELCHANGE);
	}
}



void CDonutSearchBar::OnLButtonDown(UINT nFlags, const CPoint& pt)
{
	if (s_bNoWordButton) {
		//+++ 単語ボタンなしの場合は旧処理
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);

		if (abs(rcKeyword.right - pt.x) > 5)
			return;

		SetCapture();
		::SetCursor(m_hCursor);
		m_ptDragStart = pt;
		m_ptDragHist  = pt;
	} else {	//+++ 単語ボタンありの場合
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);
		if (abs(rcKeyword.right - pt.x) <= 5) {
			m_has = 1;
			SetCapture();
			::SetCursor(m_hCursor);
			m_ptDragStart = pt;
			m_ptDragHist  = pt;
			return;
		}
		CRect	rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		ScreenToClient(&rcEngine);
		if (abs(rcEngine.right - pt.x) <= 5) {
			m_has = 2;
			SetCapture();
			::SetCursor(m_hCursor);
			m_ptDragStart = pt;
			m_ptDragHist  = pt;
			return;
		}
		m_has = 0;
	}
}



void CDonutSearchBar::OnLButtonUp(UINT nFlags, const CPoint& pt)
{
	if (::GetCapture() != m_hWnd)
		return;

	::ReleaseCapture();


	if (s_bNoWordButton) {	//+++ 単語ボタンなしのとき
		UpdateLayout(pt);
	} else {				//+++ 単語ボタンありの場合
		if (m_has == 1)
			UpdateLayout(pt);
		else if (m_has == 2)
			UpdateLayout2(pt);
	}
}



void CDonutSearchBar::UpdateLayout(const CPoint& pt)
{
	if (s_bNoWordButton) { // 単語ボタン無しの場合
		int		btnW   = btnWidth();		//+++
		int 	nMoveX = m_ptDragStart.x - pt.x;
		CRect	rcKeyword;

		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);
		rcKeyword.right -= nMoveX;
	  #if 1	//+++
		if (rcKeyword.right < rcKeyword.left+btnW) {
			nMoveX			= rcKeyword.right - rcKeyword.left+btnW;
			rcKeyword.right = rcKeyword.left  + btnW;
		}
	  #endif

		CRect rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		ScreenToClient(&rcEngine);
		rcEngine.left	-= nMoveX;

	  #if 1	//+++
		if (rcEngine.left > rcEngine.right - btnW) {
			rcEngine.left	= rcEngine.right - btnW;
			rcKeyword.right = rcEngine.left-2;
		}
		m_nEngineWidth = rcEngine.right - rcEngine.left;
	  #endif

		if (rcEngine.left >= rcEngine.right)
			return;

		if (rcKeyword.left >= rcKeyword.right)
			return;

		m_cmbKeyword.SetWindowPos(NULL, rcKeyword, SWP_NOZORDER);
		m_cmbEngine.SetWindowPos(NULL, rcEngine, SWP_NOZORDER);

		m_ptDragStart	 = pt;
		UpdateWindow();
	} else {
		//+++ 単語ボタンありの場合
		ResizeBar(pt.x - m_ptDragStart.x, 0);
		m_ptDragStart	 = pt;
		UpdateWindow();
	}
}



void CDonutSearchBar::UpdateLayout2(const CPoint& pt)
{
	ResizeBar(0, pt.x - m_ptDragStart.x);
	m_ptDragStart	 = pt;
	UpdateWindow();
}


//public: //+++ あとでprivateに戻すがとりあえずテスト.
void CDonutSearchBar::_AddToSearchBoxUnique(const CString &strURL)
{
	// search the same string
	int nCount = m_cmbKeyword.GetCount();
	for (int n = 0; n < nCount; ++n) {
		CString 	str;
		m_cmbKeyword.GetLBText(n, str);
		if (strURL == str) {
			m_cmbKeyword.DeleteString(n);
			break;
		}
	}

	m_cmbKeyword.InsertString(0, strURL);
	m_cmbKeyword.SetCurSel(0);
}


//private:
void CDonutSearchBar::_OnEnterKeyDown(int flag)
{
	CString  str;

	int nIndexCmb = m_cmbKeyword.GetCurSel();
	if (nIndexCmb == -1) {
		str = MtlGetWindowText(m_cmbKeyword);
	} else {
		MtlGetLBTextFixed(m_cmbKeyword.m_hWnd, nIndexCmb, str);
	}

	//	m_cmbKeyword.GetLBText(nIndexCmb, str);

	GetHilightBtnFlag();				//+++ ハイライトボタンの状態チェック
	checkToolBarWords();				//+++

	if ( !str.IsEmpty() ) {
		SHORT sShift = ::GetKeyState(VK_SHIFT);
		SHORT sCtrl  = ::GetKeyState(VK_CONTROL);

		if (sShift >= 0 && sCtrl >= 0) {
			_OnCommand_SearchWeb(str);
		} else {
		  #if 1 //+++ カーソル位置の単語だけを選択するようにしてみる.
			str = _GetSelectText_OnCursor( GetEditCtrl() );
		  #else
			str = _GetSelectText( GetEditCtrl() );
		  #endif

			if (sCtrl < 0)
				SendMessage(GetTopLevelParent(), WM_USER_FIND_KEYWORD, (WPARAM) str.GetBuffer(0), TRUE );
			else if (sShift < 0)
				SendMessage(GetTopLevelParent(), WM_USER_FIND_KEYWORD, (WPARAM) str.GetBuffer(0), FALSE);
		}
	} else {
		m_cmbEngine.ShowDropDown(FALSE); //minit
	}
}



LRESULT CDonutSearchBar::OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;

	if (g_bNoReposition == true)
		return 0;

	CRect	rcDesktop;
	::GetWindowRect(::GetDesktopWindow(), &rcDesktop);

	CRect	rcWnd;
	::GetWindowRect( (HWND) lParam, &rcWnd );

	if (rcDesktop.right <= rcWnd.right) {
		int    nDiff = rcWnd.right - rcDesktop.right;
		rcWnd.left	-= nDiff;
		rcWnd.right -= nDiff;
		::SetWindowPos( (HWND) lParam, NULL, rcWnd.left, rcWnd.top, 0, 0, SWP_NOSIZE );
	}

	return 0;
}



#if 0
void CDonutSearchBar::OnItemSelected(const CString& str, const CString& strSearchEng)
{
	int 	nTarCate	 = m_cmbEngine.GetCurSel();
	if (nTarCate == -1)
		return;

	//x CString strSearchEng = MtlGetWindowText(m_cmbEngine);

	BOOL	bFirst		 = TRUE;
	int 	nLoopCtn	 = 0;
	OpenSearch(str, strSearchEng, nLoopCtn, bFirst);
}
#endif

//public:
//+++ ".url"中に記述した拡張情報での検索に対応するため、修正.
void CDonutSearchBar::OpenSearch(CString strWord, CString strSearchEng, int &nLoopCnt, BOOL &bFirst)
{
	if (s_bUseShortcut) {	// ショートカットコードを使う
		_ShortcutSearch(strWord, strSearchEng);
	}

	if ( strSearchEng.IsEmpty() ) {	// 一番上のエンジンで検索する
		m_cmbEngine.GetLBText(0, strSearchEng);
		if (strSearchEng.IsEmpty())
			return;
	}

	CString 	strSearchPath = GetSearchIniPath();

  #if 1	//+++ 	search.iniがない場合は、Googleで適当にサーチする.
	if (::PathFileExists(strSearchPath) == FALSE && strSearchEng == ENGINENAME_FOR_NO_SEARCH_INI) {
		if (s_bFiltering)
			FilterString(strWord);		// 全角スペースの置換
		_EncodeString(strWord, ENCODE_UTF8);
		DonutOpenFile(m_hWnd, _T("http://www.google.co.jp/search?num=100&q=") + strWord, 0);
		return;
	}
  #endif

	CIniFileI		pr(strSearchPath, strSearchEng);
	if (pr.GetValue(_T("Group"), 0 ) != 0) {	// グループ検索
		pr.Close();
		OpenSearchGroup(strWord, strSearchEng, nLoopCnt, bFirst);
	} else {
		_EXPROP_ARGS		args;
		CString 			strOpenURL;
		std::vector<char>	vecPostData;
		if (pr.GetValue(_T("UsePost"))) {
			// 検索URLの読み込み
			strOpenURL	 = pr.GetString( _T("FrontURL") );

			// 検索付加キーワードの読み込み
			CString 	 strFrontKeyWord = pr.GetString( _T("FrontKeyWord") );
			CString 	 strBackKeyWord  = pr.GetString( _T("BackKeyWord") );

			// 検索語の作成
			CString strSearchWord = strFrontKeyWord + strWord + strBackKeyWord;

			DWORD	dwEncode = pr.GetValue(_T("Encode"), 0);	// エンコード
			switch (dwEncode) {
			case ENCODE_SHIFT_JIS:
				vecPostData	= Misc::tcs_to_sjis(strSearchWord);
				break;
			case ENCODE_EUC:
				vecPostData	= Misc::tcs_to_eucjp(strSearchWord);
				break;
			case ENCODE_UTF8:
				vecPostData	= Misc::tcs_to_utf8(strSearchWord);
				break;
			default:
				vecPostData	= Misc::tcs_to_sjis(strSearchWord);
				break;
			}
			m_PostData.pPostData	= &vecPostData[0];
			m_PostData.nPostBytes	= (int)vecPostData.size() - 1;
		} else {
			if (GetOpenURLstr(strOpenURL, strWord, pr, CString()) == false)
				return;	// 検索URLの作成に失敗
		}
		DWORD	dwOpenFlags = 0;
		if (bFirst) {
			dwOpenFlags |= D_OPENFILE_ACTIVATE;
			bFirst		 = FALSE;
		}
		if (s_bActiveWindow) 
			dwOpenFlags |= D_OPENFILE_NOCREATE;

		//DonutOpenFile(m_hWnd, strOpenURL, dwOpenFlags);
		
		args.strUrl 	   = strOpenURL;
		args.dwOpenFlag    = dwOpenFlags;
		args.strIniFile    = strSearchPath;
		args.strSection    = strSearchEng;
		args.strSearchWord = RemoveShortcutWord( strWord/*GetSearchStr()*/ );

		SetSearchStr(strWord);
		// メインフレームに投げる
		::SendMessage(GetTopLevelParent(), WM_OPEN_WITHEXPROP, (WPARAM) &args, 0);

		m_PostData.pPostData	= NULL;
		m_PostData.nPostBytes	= 0;
	}
}


// 検索URLを作成
bool CDonutSearchBar::GetOpenURLstr(CString& strOpenURL, const CString& strWord0, CIniFileI& pr, const CString& frontURL0)
{
	//検索URLの読み込み
	CString 	 strFrontURL	 = pr.GetString( _T("FrontURL") );
	if ( strFrontURL.IsEmpty() ) {
		if (frontURL0.IsEmpty())
			return false;
		strFrontURL = frontURL0;
	}
	CString 	 strBackURL 	 = pr.GetString( _T("BackURL") );

	//検索付加キーワードの読み込み
	CString 	 strFrontKeyWord = pr.GetString( _T("FrontKeyWord") );
	CString 	 strBackKeyWord  = pr.GetString( _T("BackKeyWord") );

	//検索語の作成
	CString strWord = strFrontKeyWord + strWord0 + strBackKeyWord;

	if (s_bFiltering)
		FilterString(strWord);		// 全角スペースの置換

	DWORD	dwEncode = pr.GetValue(_T("Encode"), 0);	// エンコード
	if (dwEncode != 0)
		_EncodeString(strWord, dwEncode);

	strOpenURL = strFrontURL + strWord + strBackURL;

	return true;
}



//private:
void CDonutSearchBar::OpenSearchGroup(const CString& strWord, const CString& strSearchEng, int &nLoopCnt, BOOL &bFirst)
{
	nLoopCnt++;
	if (nLoopCnt > 10)
		return;	// 10で打ち止め

	CString 	strSearchPath = GetSearchIniPath();
	CIniFileI	pr(strSearchPath, strSearchEng);

	int	nListCnt = (int)pr.GetValue( _T("ListCount"), 0 );	// 検索グループ内の検索エンジン数
	for (int ii = 1; ii <= nListCnt; ++ii) {
		CString 	strKey;
		strKey.Format(_T("%02d"), ii);
		CString 	strSearchEng2 = pr.GetStringUW( strKey );
		if ( strSearchEng2.IsEmpty() )
			continue;

		OpenSearch(strWord, strSearchEng2, nLoopCnt, bFirst);
	}
}


// 文字列を指定された方法でエンコードする
void CDonutSearchBar::_EncodeString(CString &str, int dwEncode)	//minit
{
  #if 1	//+++ Unicode対応で作り直し
	if	 (dwEncode == ENCODE_SHIFT_JIS)
		str = Misc::urlstr_encode( Misc::tcs_to_sjis(str) );
	else if (dwEncode == ENCODE_EUC)
		str = Misc::urlstr_encode( Misc::tcs_to_eucjp(str) );
	else if (dwEncode == ENCODE_UTF8)
		str = Misc::urlstr_encode( Misc::tcs_to_utf8(str) );
	else
		return;
  #else
	CURLEncoder enc;

	if (dwEncode == 0)
		return;
	else if (dwEncode == ENCODE_SHIFT_JIS)
		enc.URLEncode_SJIS(str);
	else if (dwEncode == ENCODE_EUC)
		enc.URLEncode_EUC(str);
	else if (dwEncode == ENCODE_UTF8)
		enc.URLEncode_UTF8(str);
	else
		return;
	//ATLASSERT(FALSE);
  #endif
}


// strWordからショートカットコードを見つけてそれに合うエンジン名を見つける
void CDonutSearchBar::_ShortcutSearch(CString &strWord, CString &strSearchEng)
{
	CString 	strSearchPath = GetSearchIniPath();

	if ( strWord.Left(1) == _T("\\") ) {	// '\'で始まっているかどうか
		int 	nFind		= strWord.Find(_T(" "));
		CString strShort	= strWord.Mid(1, nFind - 1);

		CIniFileI	pr( strSearchPath, _T("Search-List") );
		int nListCount = (int)pr.GetValue( _T("ListCount") );	// 検索エンジンの数

		if ( strShort.IsEmpty() )
			return;

		strWord = strWord.Mid(nFind + 1);	// 検索文字列の部分

		CString 	strBuf;
		CString 	strKey;
		for (int i = 1; i <= nListCount; i++) {
			// エンジン名を取得
			strKey.Format(_T("%02d"), i);
			CString 		strEngine = pr.GetStringUW( strKey );
			// ショートカットコードを取得
			CString strShortcutWord = GetShortcutWord(strEngine);

			// 比較
			if (strShort == strShortcutWord) {
				strSearchEng = strEngine;	// 見つかった
				return;
			}
		}
	}
}


// strSearchEngのショートカットワードを返す
CString CDonutSearchBar::GetShortcutWord(const CString& strSearchEng)
{
	CIniFileI	pr(GetSearchIniPath(), strSearchEng);
	return	pr.GetString( _T("ShortCutCode") );
}


// 検索エンジンをコンボボックスに登録する
void CDonutSearchBar::_InitialEngine(LPVOID lpV)
{
	CComboBox	cmb( (HWND) lpV );

	cmb.ResetContent(); 	//minit
	//::WINNLSEnableIME(cmb,FALSE);

	CString 	strSearchPath 	= GetSearchIniPath();
	int			nListCnt		= 0;
	if (::PathFileExists(strSearchPath)) {
		CIniFileI	pr( strSearchPath, _T("Search-List") );
		nListCnt	= pr.GetValue( _T("ListCount"), 0 );
		CString 	strKey;
		for (int ii = 1; ii <= nListCnt; ii++) {
			strKey.Format(_T("%02d"), ii);
			CString 	strTitle = pr.GetStringUW( strKey );
			if ( strTitle.IsEmpty() )
				continue;

			cmb.AddString(strTitle);
		}
	} else {	// Search.iniがなかった
		cmb.AddString(ENGINENAME_FOR_NO_SEARCH_INI);
	}

	CIniFileI	pr( g_szIniFileName, _T("SEARCH") );
	int	nIndex = pr.GetValue(_T("SelIndex"));
	int nSelIndex = 0;
	if (s_bLastSel && nIndex < nListCnt) {
		nSelIndex = nIndex;			// 最後に選択したサーチエンジンを復元する
	}
	cmb.SetCurSel(nSelIndex);
}


// 検索履歴をコンボボックスに登録する
void CDonutSearchBar::_InitialKeyword(LPVOID lpV)
{
	if (s_bHistorySave == false) 
		return;

	CComboBox	cmb( (HWND) lpV );
	
	CIniFileI	pr( _GetFilePath( _T("WordHistory.ini") ), _T("SEARCH_HISTORY") );
	int	nHistoryCnt = (int)pr.GetValue(_T("HistorySaveCnt"));

	for (int ii = 0; ii < nHistoryCnt; ii++) {
		CString 	strKey;
		strKey.Format(_T("KEYWORD%d"), ii);
		CString 	strKeyWord = pr.GetStringUW( strKey );
		if ( strKeyWord.IsEmpty() )
			continue;

		cmb.AddString(strKeyWord);
	}
}



// public:
BYTE CDonutSearchBar::PreTranslateMessage(MSG *pMsg)
{
  #if 1	//+++ 手抜きな、描画更新チェック
	SetCmbKeywordEmptyStr();		//+++ キーワード窓にエンジン名を表示するためのフォーカスチェック
  #endif

	UINT msg  = pMsg->message;
	int  vKey =  (int) pMsg->wParam;

	if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP || msg == WM_KEYDOWN) {
		if ( !IsWindowVisible() || !IsChild(pMsg->hwnd) )									// ignore
			return _MTL_TRANSLATE_PASS;

		// left or right pressed, check shift and control key.
		if (	vKey == VK_UP || vKey == VK_DOWN || vKey == VK_LEFT || vKey == VK_RIGHT
			 || vKey == VK_HOME || vKey == VK_END
			 || vKey == (0x41 + 'C' - 'A')
			 || vKey == (0x41 + 'V' - 'A')
			 || vKey == (0x41 + 'X' - 'A')
			 || vKey == VK_INSERT)
		{
			if (::GetKeyState(VK_SHIFT) < 0 || ::GetKeyState(VK_CONTROL) < 0)
				return _MTL_TRANSLATE_WANT; 												// pass to edit control
		}

		// return key have to be passed
		if (vKey == VK_RETURN) {
			return _MTL_TRANSLATE_WANT;
		}

		// other key have to be passed
		if (VK_LBUTTON <= vKey && vKey <= VK_HELP) {
			BOOL bAlt = HIWORD(pMsg->lParam) & KF_ALTDOWN;

			if (!bAlt && ::GetKeyState(VK_SHIFT) >= 0 && ::GetKeyState(VK_CONTROL) >= 0)	// not pressed
				return _MTL_TRANSLATE_WANT; 												// pass to edit control
		}
	}
  #if 1	//+++	とりあえず、むりやり、右クリックがWEB検索ボタンの範囲内で押された場合、検索エンジンメニューを出す
	else if (msg == WM_RBUTTONUP) {
		CRect		rect;
		if (m_wndToolBar.GetRect(ID_SEARCH_WEB, rect)) {
			m_wndToolBar.ClientToScreen( rect );
			POINT 	pt;
			::GetCursorPos(&pt);
			if (pt.x >= rect.left && pt.x < rect.right && pt.y >= rect.top && pt.y < rect.bottom) {
				//MtlSendCommand(m_hWnd, ID_SEARCHENGINE_MENU);
				BOOL	dmy=0;
				OnSearchEngineMenu(0,0,0,dmy);
				return _MTL_TRANSLATE_HANDLE;
			}
		}
	}
  #endif
  #if 1 //*+++	強引対処:検索バーにカーソルがある状態でCTRL+RETURNで頁内次検索をしたとき、
		//		どこかの処理がCTRL+ENTERでエラー音を出しているようなのだが、誰が犯人か
		//		わからないので、ここで強引にキーを食って誤魔化す.
	else if (msg == WM_CHAR && (vKey == VK_RETURN || vKey == 0x0a) && ::GetKeyState(VK_CONTROL) < 0) {
		if ( !IsWindowVisible() || !IsChild(pMsg->hwnd) )									// ignore
			return _MTL_TRANSLATE_PASS;
		return _MTL_TRANSLATE_HANDLE;
	}
  #endif

	return _MTL_TRANSLATE_PASS;
}



#if 0	//+++ WEB検索ボタンで右クリックしたら検索エンジンメニューを表示してみる
void CDonutSearchBar::OnToolBarRButtonUp(UINT nFlags, const CPoint& pt)
{
	CRect		rect;
	m_wndToolBar.GetRect(0, rect);
	if (pt.x >= rect.left && pt.x < rect.right && pt.y >= rect.top && pt.y < rect.bottom) {
		//MtlSendCommand(m_hWnd, ID_SEARCHENGINE_MENU);
		BOOL	dmy=0;
		OnSearchEngineMenu(0,0,0,dmy);
	}
}
#endif


// キーワードのエディットコントロールを返す
CEdit CDonutSearchBar::GetEditCtrl()
{
	return CEdit( m_cmbKeyword.GetDlgItem(IDC_EDIT/*1001*/) );
}


// 検索エンジンコンボボックスにフォーカスをあてる
void CDonutSearchBar::SetFocusToEngine()
{
	::SetFocus(m_cmbEngine.m_hWnd);
	m_cmbEngine.ShowDropDown(TRUE);
}



//private:
// 検索履歴を保存する
void CDonutSearchBar::SaveHistory()
{
	int nItemCount = m_cmbKeyword.GetCount();
	if (nItemCount > s_nHistorySaveCnt) {
		nItemCount = s_nHistorySaveCnt;
	}

	CString 	strFileName = _GetFilePath( _T("WordHistory.ini") );
	CIniFileO	pr(strFileName, _T("SEARCH_HISTORY"));
	pr.DeleteSection();
	pr.SetValue( nItemCount, _T("HistorySaveCnt") );

	if (s_bHistorySave) {
		for (int ii = 0; ii < nItemCount; ii++) {
			CString 	strKeyWord;
			m_cmbKeyword.GetLBText(ii, strKeyWord);

			CString 	strKey;
			strKey.Format(_T("KEYWORD%d"), ii);
			pr.SetStringUW(strKeyWord, strKey);
		}
	}
}



//public:
DROPEFFECT CDonutSearchBar::OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint /*point*/)
{
	if (m_bDragFromItself)
		return DROPEFFECT_NONE;

	_DrawDragEffect(false);

	m_bDragAccept = _MtlIsHlinkDataObject(pDataObject);
	return _MtlStandardDropEffect(dwKeyState);
}



DROPEFFECT CDonutSearchBar::OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint /*point*/, DROPEFFECT dropOkEffect)
{
	if (m_bDragFromItself || !m_bDragAccept)
		return DROPEFFECT_NONE;

	return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect) | DROPEFFECT_COPY;
}



void CDonutSearchBar::OnDragLeave()
{
	if (m_bDragFromItself)
		return;

	_DrawDragEffect(true);
}



//private:
// ドラッグされたとき枠を描く
void CDonutSearchBar::_DrawDragEffect(bool bRemove)
{
	CClientDC dc(m_wndKeyword.m_hWnd);

	CRect	  rect;
	m_wndKeyword.GetClientRect(rect);

	if (bRemove)
		MtlDrawDragRectFixed(dc.m_hDC, &rect, CSize(0, 0), &rect, CSize(2, 2), NULL, NULL);
	else
		MtlDrawDragRectFixed(dc.m_hDC, &rect, CSize(2, 2), NULL, CSize(2, 2),	NULL, NULL);
}



//public:
DROPEFFECT CDonutSearchBar::OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint /*point*/)
{
	if (m_bDragFromItself)
		return DROPEFFECT_NONE;

	_DrawDragEffect(true);

	CString 	strText;

	if (   MtlGetHGlobalText( pDataObject, strText)
		|| MtlGetHGlobalText( pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) )
	{
		CEdit edit = GetEditCtrl();
		edit.SendMessage(WM_CHAR, 'P'); //m_cmbKeyword.GetCurSel() == -1にするための苦肉の策 minit
		edit.SetWindowText(strText);

		BOOL  bSts = s_bDropGo;
		if (::GetKeyState(VK_SHIFT) < 0)
			bSts = !bSts;

		if (bSts) {
			_OnCommand_SearchWeb(strText);
		}

		return DROPEFFECT_COPY;
	}

	return DROPEFFECT_NONE;
}


// フォントを変更する
void CDonutSearchBar::SetFont(HFONT hFont, BOOL bRedraw /*= TRUE*/)
{
//	CDialogImpl<CDonutSearchBar>::SetFont(hFont, bRedraw);
	__super::SetFont(hFont, bRedraw);
	m_cmbEngine.SetFont(hFont, bRedraw);
	m_cmbKeyword.SetFont(hFont, bRedraw);
	m_wndKeyword.SetFont(hFont, bRedraw);
	if (m_wndToolBar.m_hWnd) {
		m_wndToolBar.SetFont(hFont, bRedraw);
	}

	/* 検索バーの高さを取得 */
	CRect	rc;
	m_cmbKeyword.GetWindowRect(&rc);
	int	height = rc.Height() + 1;

	/* 検索バーの高さを更新 */
	_RefreshBandInfo(height);

	/* ツールバーの高さをKeywordComboBoxの高さに合わせる */
	CSize	size;
	m_wndToolBar.GetButtonSize(size);
	m_wndToolBar.SetButtonSize(size.cx, height);
	m_ButtonSize.SetSize(size.cx, height);

	//_InitCombo();								//+++
	ResizeBar(0, 0);
}



//private:
void CDonutSearchBar::_SetVerticalItemCount(CComboBox &cmb, int count)
{
	CRect rc;
	int nIndex = cmb.AddString(_T("DUMMY"));
	int   itemheight = cmb.GetItemHeight(nIndex);
	cmb.DeleteString(nIndex);
	cmb.GetClientRect(&rc);
//	int dh	  = (itemheight > m_nDefDlgH) ? itemheight : m_nDefDlgH;
	int dh = itemheight;
	rc.bottom = rc.top + dh + (itemheight/3) + (itemheight * count) + 2;	//+++ だいたいの感じになるように、適当に計算
	cmb.MoveWindow(&rc);
}



void CDonutSearchBar::_InitCombo()						//minit
{
//	m_cmbEngine.SetItemHeight(-1, m_nItemFontH);
//	m_cmbKeyword.SetItemHeight(-1, m_nItemFontH);

	if (s_bHeightCount) {
		ATLASSERT( 0 < s_nHeightCountCnt && s_nHeightCountCnt < MAXHEIGHTCOUNT );

		_SetVerticalItemCount(m_cmbEngine , s_nHeightCountCnt);
		_SetVerticalItemCount(m_cmbKeyword, s_nHeightCountCnt);
	}
  #if 1	//+++ vista以外でコンボボックスの高さがおかしい件を強制的に回避してみる...
	else {
		_SetVerticalItemCount(m_cmbEngine , MAXHEIGHTCOUNT);
		_SetVerticalItemCount(m_cmbKeyword, DEFAULT_HEIGHTCOUNT/*50*/);
	}
  #endif
}



//public:
void CDonutSearchBar::ShowToolBarIcon(BOOL flag)
{
	if (flag) {
		if (m_bLoadedToolBar) {
			if ( ::IsWindow(m_wndToolBar.m_hWnd) )
				m_wndToolBar.ShowWindow(SW_NORMAL);
		} else {
			_InitToolBar( m_nBmpCX, m_nBmpCY, RGB(255, 0, 255) );
			m_wndToolBar.ShowWindow(SW_NORMAL);
		}
	} else {
		if (m_bLoadedToolBar) {
			m_wndToolBar.ShowWindow(SW_HIDE);
		}
	}

	m_bShowToolBar = flag;

	//サイズ変更
	CRect	rect;
	GetWindowRect(rect);
	CWindow(GetParent()).ScreenToClient(rect);
	int 	width	= rect.right  - rect.left - 1;
	int 	height	= rect.bottom - rect.top;
//	SetWindowPos(NULL, rect.left, rect.top, width,	 height, SWP_NOZORDER | SWP_NOREDRAW);
//	SetWindowPos(NULL, rect.left, rect.top, width+1, height, SWP_NOZORDER);
}

// s_nMinimumLength以下の文字列を削除する
void	CDonutSearchBar::DeleteMinimumLengthWord(CString &strWord)
{
	if ( 1 < s_nMinimumLength && strWord.IsEmpty() == FALSE) {
		std::vector<CString>	strSearchWords;
		Misc::SeptTextToWords(strSearchWords, strWord);
		strWord = _T("");
		std::vector<CString>::iterator it = strSearchWords.begin();
		for ( ; it != strSearchWords.end(); ++it ) {
			if (s_nMinimumLength <= it->GetLength() ) {
				strWord += *it;
				strWord += _T(" ");
			}
		}
	}
}

// ハイライトボタンを押したとき
void CDonutSearchBar::SearchHilight()
{
  #if 1 //+++
	CString str  = GetSearchStr();
  #else
	CEdit	edit = GetEditCtrl();
	CString str  = MtlGetWindowText(edit);
  #endif
	str = RemoveShortcutWord(str);
	//if (! str.IsEmpty())
		_OnCommand_SearchHilight(str);
}



void CDonutSearchBar::SearchPage(BOOL bForward)
{
	_OnCommand_SearchPage(bForward);
}


// スキンを再読み込みする
void CDonutSearchBar::ReloadSkin(int nCmbStyle)
{
	SetComboStyle(nCmbStyle);

	if ( !m_wndToolBar.IsWindow() )
		return;

	m_bExistManifest	= IsExistManifestFile();				//+++

	CImageList	imgs	= m_wndToolBar.GetImageList();
	CImageList	imgsHot = m_wndToolBar.GetHotImageList();
	CImageList	imgsDis = m_wndToolBar.GetDisabledImageList();	//+++

	_ReplaceImageList(GetSkinSeachBarPath(0), imgs	 , IDB_SEARCHBUTTON);
	_ReplaceImageList(GetSkinSeachBarPath(1), imgsHot, IDB_SEARCHBUTTON_HOT);

  #if 1 //+++ Disabled画像の対応.
	CString str = GetSkinSeachBarPath(2);
	int 	dis = 0;
	if (::PathFileExists(str) == FALSE) {					//+++ 画像ファイルがない時
		if (::PathFileExists(GetSkinSeachBarPath(0))) {		//+++ 通常があれば
			str = GetSkinSeachBarPath(0);					//+++ 通常画で代用
		} else {											//+++ 通常もなければ
			dis = IDB_SEARCHBUTTON_DIS; 					//+++ デフォルトのDisable画を使う.
		}
	}
	_ReplaceImageList(str, imgsDis, dis);					//+++
  #endif

	Invalidate(TRUE);
	m_wndToolBar.Invalidate(TRUE);
}



void	CDonutSearchBar::SetComboStyle(int nCmbStyle)
{
	m_cmbEngine.SetDrawStyle (nCmbStyle);
	m_cmbKeyword.SetDrawStyle(nCmbStyle);
}




//=========================================================================



//+++ サーチエンジンメニューを作る
bool CDonutSearchBar::_MakeSearchEngineMenu(CMenu& menu0)
{
	menu0.LoadMenu(IDR_SEARCHENGINE_MENU);
	if (menu0.m_hMenu == NULL)
		return false;
	CMenuHandle menu = menu0.GetSubMenu(0);
	if (menu.m_hMenu == NULL)
		return false;
	menu.DeleteMenu(0, MF_BYPOSITION );
	ATLASSERT( menu.GetMenuItemCount() == 0 );
	int 	num = m_cmbEngine.GetCount();
	if (num > ID_INSERTPOINT_SEARCHENGINE_END+1 - ID_INSERTPOINT_SEARCHENGINE) {
		ATLASSERT(FALSE);
		num = ID_INSERTPOINT_SEARCHENGINE_END+1 - ID_INSERTPOINT_SEARCHENGINE;
	}
	for (int i = 0; i < num; ++i) {
		CString 		  strName;
		m_cmbEngine.GetLBText(i, strName);
		menu.AppendMenu(MF_ENABLED | MF_STRING , ID_INSERTPOINT_SEARCHENGINE + i, strName);
	}
	return true;
}



//+++
bool CDonutSearchBar::OnSearchEngineMenu(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
 #if 1	//+++
	// ポップアップメニューを開く.
	::SetForegroundWindow(m_hWnd);
	POINT 	pt;
	::GetCursorPos(&pt);
	CMenu 	menu = GetSearchEngineMenuHandle();
	// ポップアップメニューを表示
	HRESULT hr = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
	if (hr < ID_INSERTPOINT_SEARCHENGINE || hr > ID_INSERTPOINT_SEARCHENGINE_END)
		return false;	// なにも選択されなかった

	// 選択されたものからサーチエンジン名を取得.
	hr -= ID_INSERTPOINT_SEARCHENGINE;
	CString 	strEngine;
	if (menu.GetMenuString(hr, strEngine, MF_BYPOSITION) == 0)
		return false;

	// 選択されたエンジンで、現在の検索文字列をweb検索.
	CString		strKeyword = GetSearchStr();
	SearchWeb_str_engine(strKeyword, strEngine);
	return true;
 #else
	// サーチエンジンメニューを用意.
	GetSearchEngineMenu();

	// ポップアップメニューを開く.
	::SetForegroundWindow(m_hWnd);
	POINT 	pt;
	::GetCursorPos(&pt);
	CMenuHandle 	menu = m_engineMenu.GetSubMenu(0);
	HRESULT hr = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd, NULL);
	if (hr <= 0 || hr > menu.GetMenuItemCount())
		return 0;

	// 選択されたものからサーチエンジン名を取得.
	hr -= 1;
	CString 	strEngine;
	if (menu.GetMenuString(hr, strEngine, MF_BYCOMMAND) == 0)
		return 0;

	// 選択されたエンジンで、現在の検索文字列をweb検索.
	SearchWeb_str_engine( GetSearchStr(), strEngine );
	return 1;
 #endif
}



//=========================================================================
//++++



#if SEARCH_JIKKEN == 1


LRESULT CDonutSearchBar::OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMREBARCHEVRON lpnm = (LPNMREBARCHEVRON) pnmh;

	if ( lpnm->wID != GetDlgCtrlID() ) {
		bHandled = FALSE;
		return 1;
	}

	CMenuHandle 	 menu = PrepareChevronMenu();
	DisplayChevronMenu(menu, lpnm);
	CleanupChevronMenu(menu, lpnm);

	return 0;
}


CMenuHandle CDonutSearchBar::PrepareChevronMenu()
{
#if 0
	CMenuHandle menuCmdBar(m_wndToolBar.m_hMenu);

	// build a menu from hidden items
	CMenuHandle menu;
	BOOL		bRet = menu.CreatePopupMenu();

	ATLASSERT(bRet);
	RECT		rcClient = {0};
	bRet = GetClientRect(&rcClient);
	ATLASSERT(bRet);
	int client_right = rcClient.right;
	unsigned	num = m_arrBtn.GetSize();
	for (unsigned i = 0; i < num; ++i) {
		CCmdBarButton cbb	   = m_arrBtn[i];
		bool		  bEnabled = ( (cbb.m_fsState & CBSTATE_ENABLED) != 0 );

		int cbb_btn_right = cbb.m_rcBtn.right;
		if (cbb_btn_right > client_right) {
			TCHAR			szBuff[100];
			CMenuItemInfo	mii;
			mii.fMask	   = MIIM_TYPE | MIIM_SUBMENU;
			mii.dwTypeData = szBuff;
			mii.cch 	   = sizeof (szBuff) / sizeof (TCHAR);
			bRet		   = menuCmdBar.GetMenuItemInfo(i, TRUE, &mii);
			ATLASSERT(bRet);
			// Note: CmdBar currently supports only drop-down items
			ATLASSERT( ::IsMenu(mii.hSubMenu) );
			bRet		   = menu.AppendMenu( MF_STRING|MF_POPUP|(bEnabled ? MF_ENABLED : MF_GRAYED),
											 (UINT_PTR) mii.hSubMenu,
											 mii.dwTypeData );
			ATLASSERT(bRet);
		}
	}
	if (menu.m_hMenu && menu.GetMenuItemCount() == 0) { // no hidden buttons after all
		menu.DestroyMenu();
		return NULL;
	}

	return menu;
#else
	return NULL;
#endif
}


void CDonutSearchBar::DisplayChevronMenu(CMenuHandle menu, LPNMREBARCHEVRON lpnm)
{
	if (menu.m_hMenu == 0)
		return;

	// convert chevron rect to screen coordinates
	CWindow   wndFrom	 = lpnm->hdr.hwndFrom;
	RECT	  rc		 = lpnm->rc;

	wndFrom.ClientToScreen(&rc);
	// set up flags and rect
	UINT		uMenuFlags = TPM_LEFTBUTTON | TPM_VERTICAL | TPM_LEFTALIGN | TPM_TOPALIGN
						   | (!AtlIsOldWindows() ? TPM_VERPOSANIMATION : 0);
	TPMPARAMS	TPMParams;
	TPMParams.cbSize	= sizeof (TPMPARAMS);
	TPMParams.rcExclude = rc;
	::TrackPopupMenuEx(menu.m_hMenu, uMenuFlags, rc.left, rc.bottom, m_wndParent, &TPMParams);
}


void CDonutSearchBar::CleanupChevronMenu(CMenuHandle menu, LPNMREBARCHEVRON lpnm)
{
	if (menu.m_hMenu) {
		for (int i = menu.GetMenuItemCount() - 1; i >= 0; i--)
			menu.RemoveMenu(i, MF_BYPOSITION);
	}
	menu.DestroyMenu();
	CWindow wndFrom = lpnm->hdr.hwndFrom;
	RECT	rc		= lpnm->rc;
	wndFrom.ClientToScreen(&rc);
	MtlEatNextLButtonDownOnChevron(m_wndParent, rc);
}




#elif SEARCH_JIKKEN == 2


//+++
LRESULT CDonutSearchBar::OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
{
	ATLASSERT( ( (LPNMREBARCHEVRON) pnmh )->wID == GetDlgCtrlID() );

	if ( !PushChevron( pnmh, GetTopLevelParent() ) ) {
		bHandled = FALSE;
		return 1;
	}

	return 0;
}



//+++
HMENU CDonutSearchBar::ChevronHandler_OnGetChevronMenu(int nCmdID, HMENU &hMenuDestroy)
{
	bool		bDestroy = 0;
	bool		bSubMenu = 0;
	CMenuHandle menu = _GetDropDownMenu(nCmdID, bDestroy, bSubMenu);

	if (bDestroy)
		hMenuDestroy = menu.m_hMenu;

	if (bSubMenu)
		return menu.GetSubMenu(0);
	else
		return menu;
}



// Implemantation
HMENU CDonutSearchBar::_GetDropDownMenu(int nCmdID, bool &bDestroy, bool &bSubMenu)
{
	bDestroy = true;
	bSubMenu = false;
#if 0
	CEdit	edit = GetEditCtrl();
	CString str  = MtlGetWindowText(edit);

	switch (nCmdID) {
	case ID_SEARCH_WEB: 		_OnCommand_SearchWeb(str);								break;
	case ID_SEARCHBAR_HILIGHT:	_OnCommand_SearchHilight(str);							break;	//ID_SEARCH_HILIGHT:
	//+++ case ID_SEARCH_PAGE:	_OnCommand_SearchPage( (::GetKeyState(VK_SHIFT) < 0) ? FALSE : TRUE );	break;
	case ID_SEARCH_PAGE:		_OnCommand_SearchPage( (::GetKeyState(VK_SHIFT) >= 0)); break;
	case ID_SEARCHBAR_WORD00:
	case ID_SEARCHBAR_WORD01:
	case ID_SEARCHBAR_WORD02:
	case ID_SEARCHBAR_WORD03:
	case ID_SEARCHBAR_WORD04:
	case ID_SEARCHBAR_WORD05:
	case ID_SEARCHBAR_WORD06:
	case ID_SEARCHBAR_WORD07:
	case ID_SEARCHBAR_WORD08:
	case ID_SEARCHBAR_WORD09:	_OnCommand_SearchPage((::GetKeyState(VK_SHIFT) >= 0), nCmdID-ID_SEARCHBAR_WORD00);	break;
	default:					ATLASSERT(0);
	}
#endif
	return 0;
}


//+++
void	CDonutSearchBar::Chevronhandler_OnCleanupChevronMenu()
{
}
#endif
