/*!	@file	DonutSearchBar.cpp
	@brief	検索バー
*/

#include "stdafx.h"
#include "DonutSearchBar.h"
#include "MtlDragDrop.h"
#include "HlinkDataObject.h"
#include "ExStyle.h"
#include "FlatComboBox.h"
#include "FlatComboBox_SearchKeyword.h"
#include "option\SearchPropertyPage.h"
#include <boost/thread.hpp>
#include "Donut.h"
#include "ParseInternetShortcutFile.h"


#define ENGINENAME_FOR_NO_SEARCH_INI	_T("search.iniが無いのでGoogleのみ")

/// s_nMinimumLength以下の文字列を削除する
void	DeleteMinimumLengthWord(CString& strWord)
{
	if ( 1 < CSearchBarOption::s_nMinimumLength && strWord.IsEmpty() == FALSE) {
		std::vector<CString>	strSearchWords;
		Misc::SeptTextToWords(strSearchWords, strWord);
		strWord = _T("");
		std::vector<CString>::iterator it = strSearchWords.begin();
		for ( ; it != strSearchWords.end(); ++it ) {
			if (CSearchBarOption::s_nMinimumLength <= it->GetLength() ) {
				strWord += *it;
				strWord += _T(" ");
			}
		}
	}
}

/// コンボボックスの高さを指定する
void	_SetVerticalItemCount(CComboBox &cmb, int count)
{
	CRect rc;
	int nIndex = cmb.AddString(_T("DUMMY"));
	int   itemheight = cmb.GetItemHeight(0);
	cmb.DeleteString(nIndex);
	cmb.GetClientRect(&rc);
//	int dh	  = (itemheight > m_nDefDlgH) ? itemheight : m_nDefDlgH;
	int dh = itemheight;
	rc.bottom = rc.top + dh + (itemheight/3) + (itemheight * count) + 2;	//+++ だいたいの感じになるように、適当に計算
	cmb.MoveWindow(&rc);	// visualstyleが無効のとき動作する

	cmb.SetMinVisible(count);	// visualstyleが有効の時動作する
}

enum ESBSKNMODE {
	SBSKN_NORMAL = 0,
	SBSKN_HOT	 = 1,
	SBSKN_DISABLE= 2
};
/// 検索バースキンのイメージのパスを返す
CString	GetSkinSeachBarImagePath(ESBSKNMODE mode)
{
	ATLASSERT(0 <= mode && mode <= 2);
	static const TCHAR* tbl[] = {
		_T("SearchBar.bmp"),
		_T("SearchBarHot.bmp"),
		_T("SearchBarDis.bmp"),
	};
	return _GetSkinDir() + tbl[ mode ];
}

/// Search.iniの絶対パスを返す
CString GetSearchIniPath()
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

/// 全角スペースを半角スペースに置換する
void	FilterString(CString& str) 
{
	str.Replace( _T("　"), _T(" ") );
}

/// コンボボックス右にあるつまみの幅を返す
int	btnWidth()
{
	int btnW = ::GetSystemMetrics(SM_CXHTHUMB);
	if (IsExistManifestFile() == FALSE)
		btnW += 2 * 2;
	else
		btnW += 2;

	return btnW;
}

// --------------------------------------
// CDonutSearchBar::Impl

class CDonutSearchBar::Impl :
	public CWindowImpl<Impl>,
	public IDropTargetImpl<Impl>,
	public IDropSourceImpl<Impl>,
	protected CSearchBarOption

{
public:
	Impl();
	~Impl();

	HWND	Create(HWND hWndParent);
	void	ReloadSkin(int nCmbStyle);
	void	SetFont(HFONT hFont, BOOL bRedraw = TRUE);
	CEdit	GetEditCtrl() const { return m_editKeyword; }
	HWND	GetHWndToolBar() const { return m_wndToolBar; }
	bool	ForceSetHilightBtnOn(bool bOn);
	bool	GetHilightSw() const { return m_bHilightSw != 0; }
	CMenuHandle GetSearchEngineMenuHandle();
	CString GetSearchStr() const;
	void	SetSearchStr(const CString& strWord);
	CString GetSearchEngineStr() const;
	const SearchPostData&	GetSearchPostData() const { return m_PostData; }

	void	SearchWeb(CString str = CString());
	void	SearchWebWithEngine(CString str, CString strEngine);
	void	SearchWebWithIndex(CString str, int nIndex);
	void	SearchPage(bool bForward, int nNum = -1);
	void	SearchHilight();

	void	SetFocusToEngine();
	void	RefreshEngine() { _threadInitComboBox(); _SetCmbKeywordEmptyStr(); }

	BOOL	PreTranslateMessage(MSG *pMsg);

	// Overrides
	DROPEFFECT	OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint /*point*/);
	DROPEFFECT	OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint /*point*/, DROPEFFECT dropOkEffect);
	void		OnDragLeave();
	DROPEFFECT	OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint /*point*/);


	// Message map and handlers
	BEGIN_MSG_MAP_EX( Impl )
		MSG_WM_CREATE		( OnCreate		)
		MSG_WM_DESTROY		( OnDestroy		)
		MSG_WM_SIZE			( OnSize		)
		MSG_WM_ERASEBKGND	( OnEraseBkgnd  )
		MSG_WM_MOUSEMOVE	( OnMouseMove	)
		MSG_WM_LBUTTONDOWN	( OnLButtonDown	)
		MSG_WM_LBUTTONUP	( OnLButtonUp	)
		MSG_WM_CONTEXTMENU  ( OnContextMenu )

		COMMAND_ID_HANDLER_EX( ID_SEARCH_WEB, OnSearchWeb )
		COMMAND_ID_HANDLER_EX( ID_SEARCH_PAGE, OnSearchPage )
		COMMAND_ID_HANDLER_EX( ID_SEARCHBAR_HILIGHT, OnSearchHilight )
		COMMAND_RANGE_HANDLER_EX( ID_SEARCHBAR_WORD00, ID_SEARCHBAR_WORD19, OnSearchWord )
		MSG_WM_PARENTNOTIFY ( OnParentNotify )

		COMMAND_ID_HANDLER_EX( ID_SEARCHENGINE_MENU, OnSearchEngineMenu )
		NOTIFY_CODE_HANDLER_EX( TTN_GETDISPINFO, OnToolBarGetDispInfo )
		COMMAND_HANDLER_EX	( IDC_CMB_KEYWORD, CBN_SELCHANGE, OnKeywordSelChange	)
		COMMAND_HANDLER_EX	( IDC_CMB_ENGIN	 , CBN_SELCHANGE, OnEngineSelChange		)

	ALT_MSG_MAP(1)	// KeywordEdit
		MSG_WM_KEYDOWN	( OnKeywordKeyDown )
		MSG_WM_KILLFOCUS( OnKeywordKillFocus )
		//MSG_WM_LBUTTONDOWN( OnEditLButtonDown )
		MSG_WM_RBUTTONUP  ( OnEditRButtonUp )
		MSG_WM_CONTEXTMENU( OnEditContextMenu )
		
	ALT_MSG_MAP(2)	// SearchEnginComboBox
		MSG_WM_KEYDOWN	( OnEngineKeyDown  )
		MESSAGE_HANDLER_EX( WM_CTLCOLORLISTBOX, OnShowEngineListBox )
	ALT_MSG_MAP(3)	// KeywordComboBox
		COMMAND_CODE_HANDLER( EN_CHANGE	, OnEditChanged )
	END_MSG_MAP()


	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	void	OnDestroy();
	void	OnSize(UINT nType, CSize size);
	BOOL	OnEraseBkgnd(CDCHandle dc);
	void	OnMouseMove(UINT nFlags, CPoint point);
	void	OnLButtonDown(UINT nFlags, CPoint point);
	void	OnLButtonUp(UINT nFlags, CPoint point);
	void	OnContextMenu(CWindow wnd, CPoint point);

	void	OnSearchWeb(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSearchPage(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSearchHilight(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSearchEngineMenu(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSearchWord(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnParentNotify(UINT message, UINT nChildID, LPARAM lParam);

	LRESULT OnToolBarGetDispInfo(LPNMHDR pnmh);
	void	OnKeywordSelChange(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnEngineSelChange(UINT uNotifyCode, int nID, CWindow wndCtl);

	// KeywordEdit
	void	OnKeywordKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void	OnKeywordKillFocus(CWindow wndFocus);
	void	OnEditLButtonDown(UINT nFlags, CPoint point);
	void	OnEditRButtonUp(UINT nFlags, CPoint point);
	void	OnEditContextMenu(CWindow wnd, CPoint point);
	LRESULT OnEditChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	// SearchEngineComboBox
	void	OnEngineKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	LRESULT OnShowEngineListBox(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void	_AddToolBarIcon(CString strWord);
	int		_AddDefaultToolBarIcon_tinyWordButton(size_t nWordCount);
	void	_RemoveShortcutWord(CString& str);
	void	_threadInitComboBox();
	void	_SetCmbKeywordEmptyStr();
	void	_ResizeBar(int difA = 0, int difB = 0);
	void	_UpdateLayout(const CPoint& pt);
	void	_UpdateLayout2(const CPoint& pt);
	void	_OnEnterKeyDown();
	void	_AddToSearchBoxUnique(const CString& str);
	void	_EncodeString(CString& str, int dwEncode);
	void	_DrawDragEffect(bool bRemove);
	void	_SaveHistory();

	// Constants
	enum ECharEncode {
		ENCODE_NONE 			=	0,
		ENCODE_SHIFT_JIS		=	1,
		ENCODE_EUC				=	2,
		ENCODE_UTF8 			=	3,
	};

	enum {
		cxTbBmp		= 16,
		cyTbBmp		= 16,
		s_kcxGap	=  2
	};

	enum {
		m_nDefEditT =  1,
		//+++ m_nDefDlgH	= 22
	};


	// Data members
	CFlatComboBox_SearchKeyword		m_cmbKeyword;	// 検索文字列を入れるコンボボックス
	CEdit			 m_editKeyword;
	CListBox		 m_KeywordListBox;

	CFlatComboBox	 m_cmbEngine;
	CListBox		 m_EngineListBox;

	CToolBarCtrl	 m_wndToolBar;

	HCURSOR 		 m_hCursor;
	CPoint			 m_ptDragHist;
	CPoint			 m_ptDragStart;
	CPoint			 m_ptDragEnd;

	CContainedWindow m_wndKeyword;		// ComboBox内のエディットコントロール
	CContainedWindow m_wndEngine;
	CContainedWindow m_wndCmbKeyword;

	int 			 m_cxBtnsBtn;					///< 旧 横幅調整用
	int 			 m_has; 						//+++
	int 			 m_nKeywordWidth;				//+++
	int 			 m_nEngineWidth;				//+++

	CSize			 m_ButtonSize;					// ツールバーボタンの大きさ

	DWORD			 m_dwTinyWordButton;			//+++ 単語ボタンでなく5つの数字ボタンを使う場合

	BYTE/*bool*/	 m_bDragAccept;
	BYTE/*bool*/	 m_bDragFromItself;

	BYTE/*BOOL*/	 m_bExistManifest;				//+++ .exe.manifest ファイルの有無(XP Theme使用の有無)
	BYTE/*BOOL*/	 m_bHilightSw;					//+++ ハイライトボタンのon/off状態
	boost::thread	 m_threadInitComboBox;
	SearchPostData	 m_PostData;
	bool			 m_bWordLock;

};

/// コンストラクタ
CDonutSearchBar::Impl::Impl()
	: m_wndKeyword(this, 1)
	, m_wndEngine(this, 2)
	, m_wndCmbKeyword(this, 3)
	, m_cxBtnsBtn(0)			//+++ 旧横幅調整用
	, m_has(0)					//+++
	, m_nEngineWidth(0)			//+++
	, m_bHilightSw(0)			//+++
	, m_bExistManifest(IsExistManifestFile())	//+++
	, m_dwTinyWordButton(0)		//+++
	, m_hCursor(NULL)
	, m_bDragAccept(false)
	, m_bDragFromItself(false)
	, m_bWordLock(false)
{
}

/// デストラクタ
CDonutSearchBar::Impl::~Impl()
{
}

HWND	CDonutSearchBar::Impl::Create(HWND hWndParent)
{
	HWND hWnd = __super::Create(hWndParent);
	SetDlgCtrlID(IDC_SEARCHBAR);
	return hWnd;
}


/// スキンを再読み込みする
void CDonutSearchBar::Impl::ReloadSkin(int nCmbStyle)
{
	m_cmbEngine.SetDrawStyle(nCmbStyle);
	m_cmbKeyword.SetDrawStyle(nCmbStyle);

	if ( !m_wndToolBar.IsWindow() )
		return;

	m_bExistManifest	= IsExistManifestFile();				//+++

	CImageList	imgs	= m_wndToolBar.GetImageList();
	CImageList	imgsHot = m_wndToolBar.GetHotImageList();
	CImageList	imgsDis = m_wndToolBar.GetDisabledImageList();	//+++

	_ReplaceImageList(GetSkinSeachBarImagePath(SBSKN_NORMAL), imgs	 , IDB_SEARCHBUTTON);
	_ReplaceImageList(GetSkinSeachBarImagePath(SBSKN_HOT)	, imgsHot, IDB_SEARCHBUTTON_HOT);

  #if 1 //+++ Disabled画像の対応.
	CString str = GetSkinSeachBarImagePath(SBSKN_DISABLE);
	int 	dis = 0;
	if (::PathFileExists(str) == FALSE) {					//+++ 画像ファイルがない時
		if (::PathFileExists(GetSkinSeachBarImagePath(SBSKN_NORMAL))) {		//+++ 通常があれば
			str = GetSkinSeachBarImagePath(SBSKN_NORMAL);					//+++ 通常画で代用
		} else {											//+++ 通常もなければ
			dis = IDB_SEARCHBUTTON_DIS; 					//+++ デフォルトのDisable画を使う.
		}
	}
	_ReplaceImageList(str, imgsDis, dis);					//+++
  #endif

	Invalidate(TRUE);
	m_wndToolBar.Invalidate(TRUE);
}


/// フォントを変更する
void CDonutSearchBar::Impl::SetFont(HFONT hFont, BOOL bRedraw /*= TRUE*/)
{
	__super::SetFont(hFont, bRedraw);
	m_cmbEngine.SetFont(hFont, bRedraw);
	m_cmbKeyword.SetFont(hFont, bRedraw);
	m_wndToolBar.SetFont(hFont, bRedraw);

	/* 検索バーの高さを取得 */
	CRect	rc;
	m_cmbKeyword.GetWindowRect(&rc);
	int	height = rc.Height() + 1;

	/* 検索バーの高さを更新 */
	{
		CReBarCtrl	  rebar(GetParent());
		REBARBANDINFO rbBand = { sizeof (REBARBANDINFO) };
		rbBand.fMask  = RBBIM_CHILDSIZE;

		int nIndex	= rebar.IdToIndex( GetDlgCtrlID() );
		rebar.GetBandInfo(nIndex, &rbBand);

		if ( rbBand.cyMinChild != height ) {
			// Calculate the size of the band
			rbBand.cxMinChild = 0;
			rbBand.cyMinChild = height;

			rebar.SetBandInfo(nIndex, &rbBand);
		}
	}

	/* ツールバーの高さをKeywordComboBoxの高さに合わせる */
	CSize	size;
	m_wndToolBar.GetButtonSize(size);
	m_wndToolBar.SetButtonSize(size.cx, height);
	m_ButtonSize.SetSize(size.cx, height);

	_ResizeBar();
}

/// ハイライトボタンを強制的に設定する
/// @return 設定されればtrue、以前と同じならfalse
bool	CDonutSearchBar::Impl::ForceSetHilightBtnOn(bool bOn)
{
	bool bChanged = (BOOL)bOn != m_bHilightSw;
	m_bHilightSw = bOn;
	m_wndToolBar.PressButton(ID_SEARCHBAR_HILIGHT, m_bHilightSw);
	return bChanged;
}

/// サーチエンジンリストをメニューとして返す
CMenuHandle CDonutSearchBar::Impl::GetSearchEngineMenuHandle()
{
	CMenuHandle menu;
	menu.CreatePopupMenu();
	int num = m_cmbEngine.GetCount();
	if (num > ID_INSERTPOINT_SEARCHENGINE_END+1 - ID_INSERTPOINT_SEARCHENGINE) {
		ATLASSERT(FALSE);
		num = ID_INSERTPOINT_SEARCHENGINE_END+1 - ID_INSERTPOINT_SEARCHENGINE;
	}
	for (int i = 0; i < num; ++i) {
		CString 		  strName;
		m_cmbEngine.GetLBText(i, strName);
		menu.AppendMenu(MF_ENABLED | MF_STRING , ID_INSERTPOINT_SEARCHENGINE + i, strName);
	}
	return menu;
}

/// 現在キーワードComboBoxにある文字列を取得する
CString CDonutSearchBar::Impl::GetSearchStr() const
{
	CEdit edit = GetEditCtrl();
	int nLen = edit.GetWindowTextLength() + 1;
	CString str;
	edit.GetWindowText(str.GetBuffer(nLen), nLen);
	str.ReleaseBuffer();
	return str;
}

/// 検索バーに文字列を設定する(単語ボタンも設定する)
void	CDonutSearchBar::Impl::SetSearchStr(const CString& strWord)
{
	m_bWordLock = false;	// 単語ボタンのロックを解除
	GetEditCtrl().SetWindowText(strWord);
	_AddToolBarIcon(strWord);
	_SetCmbKeywordEmptyStr();	//!< 検索バーに文字列がないときエンジン名を表示する
	m_cmbKeyword.Invalidate();
}

/// 現在選択されている検索エンジン名取得
CString CDonutSearchBar::Impl::GetSearchEngineStr() const
{
	int nLen = m_cmbEngine.GetWindowTextLength() + 1;
	CString str;
	m_cmbEngine.GetWindowText(str.GetBuffer(nLen), nLen);
	str.ReleaseBuffer();
	return str;
}

/// 検索
void	CDonutSearchBar::Impl::SearchWeb(CString str)
{
	if (str.IsEmpty()) {
		str = GetSearchStr();
		if (str.IsEmpty())
			str = Donut_GetActiveSelectedText();
	}
	SearchWebWithEngine(str, GetSearchEngineStr());
}

/// 検索エンジンを指定しての検索
void	CDonutSearchBar::Impl::SearchWebWithEngine(CString str, CString strEngine)
{
	CString strOrg = str;
  #if 1
	bool bUrlSearch = false;
	if (::GetFocus() != m_wndKeyword.m_hWnd) {	// フォーカスが当たっていない場合
	   #if 0//\\+	//+++ 強引なので、あとで、仕組みを直す
		// 選択範囲があれば、それを優先する.
		CString strSel = Donut_GetActiveSelectedText();
		if (strSel.IsEmpty() == 0) {
			str = strSel;
		}
	   #endif
	   #if 1 //+++ v1.48c で変更.
		if (strEngine.IsEmpty()) {	// エンジンが空なら
			SearchWebWithIndex(str, 0);	// とりあえず一番上のエンジンで検索
			return;
		}
		//+++ addressbarの文字列を使う？
		CIniFileI		pr(GetSearchIniPath(), strEngine);
		DWORD			exPropOpt = pr.GetValue(_T("ExPropOpt"), 0);
		pr.Close();
		if (exPropOpt & 1) {	//+++ addressbarの文字を取ってくる場合.
			if (str.IsEmpty()	//+++ 検索文字列が空の場合.
				|| (_tcsncmp(LPCTSTR(str), _T("http:"),5) != 0 && _tcsncmp(LPCTSTR(str), _T("https:"), 6) != 0)	)
			{	//+++ 検索文字列がすでにurlならそれを用いるので、ここでは省く.
				CString strUrl = GetAddressBarText();
				if (strUrl.IsEmpty() == FALSE) {
					str 	  = strUrl;
					bUrlSearch = true;
				}
			}
		}
	  #endif
	}
  #endif

	//_AddToolBarIcon(str);	//+++ サーチのツールバー部に単語を設定

	CString strSearchIniPath = GetSearchIniPath();
	if ( str.IsEmpty() == FALSE ) {
		if (s_bUseShortcut) {	// ショートカットコードを使う
			if ( str.Left(1) == _T("\\") ) {	// '\'で始まっているかどうか
				int 	nFind		= str.Find(_T(" "));
				CString strShort	= str.Mid(1, nFind - 1);
				if ( strShort.IsEmpty() )
					goto NO_SHORTCUTCODE;	// ショートカットコードがなかった

				str = str.Mid(nFind + 1);	// 検索文字列の部分

				CString 	strBuf;
				CString 	strKey;
				CIniFileI	pr( strSearchIniPath, _T("Search-List") );
				int nListCount = (int)pr.GetValue( _T("ListCount") );	// 検索エンジンの数
				for (int i = 1; i <= nListCount; i++) {
					// エンジン名を取得
					strKey.Format(_T("%02d"), i);
					CString 	strskEngine = pr.GetStringUW( strKey );
					// ショートカットコードを取得
					pr.ChangeSectionName(strskEngine);
					CString strShortcutWord = pr.GetString(_T("ShortCutCode"));
					// 比較
					if (strShort == strShortcutWord) {
						strEngine = strskEngine;	// 見つかった
						break;
					}
					pr.ChangeSectionName(_T("Search-List"));
				}
			}

		}

		NO_SHORTCUTCODE:

		if ( strEngine.IsEmpty() ) {	// 一番上のエンジンで検索する
			m_cmbEngine.GetLBText(0, strEngine);
			if (strEngine.IsEmpty())
				return;	// なかった
		}

		//+++ 	search.iniがない場合は、Googleで適当にサーチする.
		if (strEngine == ENGINENAME_FOR_NO_SEARCH_INI) {
			if (s_bFiltering)
				FilterString(str);
			_EncodeString(str, ENCODE_UTF8);
			DonutOpenFile(m_hWnd, _T("http://www.google.co.jp/search?num=100&q=") + str, 0);
			return;
		}

		CIniFileI		pr(strSearchIniPath, strEngine);
		if (pr.GetValue(_T("Group"))) {	// グループ検索
			int	nListCnt = (int)pr.GetValue( _T("ListCount"), 0 );	// 検索グループ内の検索エンジン数
			for (int ii = 1; ii <= nListCnt; ++ii) {
				CString 	strKey;
				strKey.Format(_T("%02d"), ii);
				CString 	strSearchEng2 = pr.GetStringUW( strKey );
				if ( strSearchEng2.IsEmpty() )
					continue;

				SearchWebWithEngine(str, strSearchEng2);
			}
			return;
		}

		_EXPROP_ARGS		args;
		CString 			strOpenURL;
		std::vector<char>	vecPostData;
		if (pr.GetValue(_T("UsePost"))) {	// Postを使う
			// 検索URLの読み込み
			strOpenURL	 = pr.GetString( _T("FrontURL") );

			// 検索付加キーワードの読み込み
			CString 	 strFrontKeyWord = pr.GetString( _T("FrontKeyWord") );
			CString 	 strBackKeyWord  = pr.GetString( _T("BackKeyWord") );

			// 検索語の作成
			CString strSearchWord = strFrontKeyWord + str + strBackKeyWord;

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
			m_PostData.pPostData	= vecPostData.data();
			m_PostData.nPostBytes	= (int)vecPostData.size() - 1;
		} else {
			auto funcGetOpenUrlstr = [this] (CString& strOpenURL, const CString& strWord0, CIniFileI& pr, const CString& frontURL0) -> bool
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
			};

			if (funcGetOpenUrlstr(strOpenURL, str, pr, CString()) == false)
				return;	// 検索URLの作成に失敗
		}
		DWORD	dwOpenFlags = D_OPENFILE_ACTIVATE;
		if (s_bActiveWindow) 
			dwOpenFlags |= D_OPENFILE_NOCREATE;

		//DonutOpenFile(m_hWnd, strOpenURL, dwOpenFlags);
		
		args.strUrl 	   = strOpenURL;
		args.dwOpenFlag    = dwOpenFlags;
		args.strIniFile    = strSearchIniPath;
		args.strSection    = strEngine;
		_RemoveShortcutWord(str);
		args.strSearchWord = str;
		
		SetSearchStr(str);

		// メインフレームに投げる
		::SendMessage(GetTopLevelParent(), WM_OPEN_WITHEXPROP, (WPARAM) &args, 0);

		m_PostData.pPostData	= NULL;
		m_PostData.nPostBytes	= 0;

		if (bUrlSearch == false)		//+++ url検索だった場合は、履歴に入れないで置く.
			_AddToSearchBoxUnique(strOrg);
	}
}

/// 検索エンジンのインデックスを指定しての検索
void	CDonutSearchBar::Impl::SearchWebWithIndex(CString str, int nIndex)
{
	ATLASSERT(0 <= nIndex && nIndex < m_cmbEngine.GetCount());

	CString strEngine;
	m_cmbEngine.GetLBText(nIndex, strEngine);

	SearchWebWithEngine(str, strEngine);
}

/// ページ内検索
/// @param bForward がtrueで↓検索 falseで↑検索
/// @param nNum 単語ボタンのインデックス(0...)
void	CDonutSearchBar::Impl::SearchPage(bool bForward, int nNum /*= -1*/)
{
	CString str;
	CEdit	edit = GetEditCtrl();

	if (m_bWordLock) {	// ロックされている単語を使う
		m_wndToolBar.GetButtonText(ID_SEARCHBAR_WORD00 + nNum/*ID_SEARCHBAR_WORD00*/, str.GetBuffer(1024));
		str.ReleaseBuffer();
	} else {

		if (nNum == -1) {
			str 	= _GetSelectText_OnCursor(edit);
			//引用符・ショートカットワードは除外
			_RemoveShortcutWord(str);

			CString str00;
			m_wndToolBar.GetButtonText(ID_SEARCHBAR_WORD00, str00.GetBuffer(256));
			str00.ReleaseBuffer();
			if (str00.IsEmpty() == FALSE)
				str = str00;
		} else {
			str 	= _GetSelectText(edit);
			//引用符・ショートカットワードは除外
			_RemoveShortcutWord(str);
			if (s_bFiltering)
				FilterString(str);

			_AddToolBarIcon(str);
			if (str != GetSearchStr())
				m_bWordLock = true;

			std::vector<CString> strs;
			strs.reserve(10);
			Misc::SeptTextToWords(strs, str);
			if (size_t(nNum) < strs.size())
				str = strs[nNum];
			else
				str = strs[0];	//\\+
		}

		CString strExcept = _T(" \t\"\r\n　");
		str.TrimLeft(strExcept);
		str.TrimRight(strExcept);
		if (s_bFiltering)
			FilterString(str);
	}

	SendMessage(GetTopLevelParent(), WM_USER_FIND_KEYWORD, (WPARAM) str.GetBuffer(0), (LPARAM) bForward);
}

/// ハイライトボタンを押したとき
void	CDonutSearchBar::Impl::SearchHilight()
{
	m_bHilightSw = !m_bHilightSw;
	
	CString str = GetSearchStr();
	if (str.IsEmpty() && m_bHilightSw) {
		m_bHilightSw = FALSE;
		return;	// キーワードがない状態でハイライトボタンが押されたら帰る
	}
	_AddToolBarIcon(str);
	ForceSetHilightBtnOn(m_bHilightSw != 0);

	_RemoveShortcutWord(str);
	if (s_bFiltering)
		FilterString(str);

	SendMessage(GetTopLevelParent(), WM_USER_HILIGHT, (WPARAM)str.GetBuffer(0), 0);
}

// 検索エンジンコンボボックスにフォーカスをあてる
void CDonutSearchBar::Impl::SetFocusToEngine()
{
	m_cmbEngine.SetFocus();
	m_cmbEngine.ShowDropDown(TRUE);
}

/// メッセージフィルタ
BOOL	CDonutSearchBar::Impl::PreTranslateMessage(MSG *pMsg)
{
  #if 1	//+++ 手抜きな、描画更新チェック
	_SetCmbKeywordEmptyStr();		//+++ キーワード窓にエンジン名を表示するためのフォーカスチェック
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
			//\\+return _MTL_TRANSLATE_WANT;
			OnKeywordKeyDown(VK_RETURN, 0, 0);
			return _MTL_TRANSLATE_HANDLE;
		}

		// other key have to be passed
		if (VK_LBUTTON <= vKey && vKey <= VK_HELP) {
			BOOL bAlt = HIWORD(pMsg->lParam) & KF_ALTDOWN;

			if (!bAlt && ::GetKeyState(VK_SHIFT) >= 0 && ::GetKeyState(VK_CONTROL) >= 0)	// not pressed
				return _MTL_TRANSLATE_WANT; 												// pass to edit control
		}
	}
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


// Overrides

DROPEFFECT CDonutSearchBar::Impl::OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint /*point*/)
{
	if (m_bDragFromItself)
		return DROPEFFECT_NONE;

	_DrawDragEffect(false);

	m_bDragAccept = _MtlIsHlinkDataObject(pDataObject);
	return _MtlStandardDropEffect(dwKeyState);
}



DROPEFFECT CDonutSearchBar::Impl::OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint /*point*/, DROPEFFECT dropOkEffect)
{
	if (m_bDragFromItself || !m_bDragAccept)
		return DROPEFFECT_NONE;

	return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect) | DROPEFFECT_COPY;
}



void CDonutSearchBar::Impl::OnDragLeave()
{
	if (m_bDragFromItself)
		return;

	_DrawDragEffect(true);
}

DROPEFFECT CDonutSearchBar::Impl::OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint /*point*/)
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

		bool  bSts = s_bDropGo;	// Dropで検索
		if (::GetKeyState(VK_SHIFT) < 0)
			bSts = !bSts;

		if (bSts)
			SearchWeb(strText);

		return DROPEFFECT_COPY;
	}

	return DROPEFFECT_NONE;
}

// ドラッグされたとき枠を描く
void CDonutSearchBar::Impl::_DrawDragEffect(bool bRemove)
{
	CClientDC dc(m_wndKeyword.m_hWnd);

	CRect	  rect;
	m_wndKeyword.GetClientRect(rect);

	if (bRemove)
		MtlDrawDragRectFixed(dc.m_hDC, &rect, CSize(0, 0), &rect, CSize(2, 2), NULL, NULL);
	else
		MtlDrawDragRectFixed(dc.m_hDC, &rect, CSize(2, 2), NULL, CSize(2, 2),	NULL, NULL);
}



// Message map

/// 検索バーの初期化	
int		CDonutSearchBar::Impl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_cmbKeyword.Create(m_hWnd);	// キーワードコンボボックス作成
	m_cmbKeyword.SetDlgCtrlID(IDC_CMB_KEYWORD);
	COMBOBOXINFO info = { sizeof(COMBOBOXINFO) };
	m_cmbKeyword.GetComboBoxInfo(&info);
	m_editKeyword = info.hwndItem;
	m_KeywordListBox = info.hwndList;

	m_cmbEngine.Create(m_hWnd);		// 検索エンジンコンボボックス作成
	m_cmbEngine.SetDlgCtrlID(IDC_CMB_ENGIN);
	m_cmbEngine.GetComboBoxInfo(&info);
	m_EngineListBox = info.hwndList;
	if (s_bSearchListWidth) 
		m_cmbEngine.SetDroppedWidth(s_nSearchListWidth);

	// 設定を読み込む
	CIniFileI	pr( g_szIniFileName, _T("SEARCH") );
	m_nKeywordWidth 	 = pr.GetValue( _T("KeywordWidth" ), 150 );
	m_nEngineWidth		 = pr.GetValue( _T("EngWidth"	  ), 150 );
	pr.Close();

	// ドロップダウンリストビューの高さを設定
	if (s_bHeightCount) {
		ATLASSERT( 0 < s_nHeightCountCnt && s_nHeightCountCnt <= MAXHEIGHTCOUNT );

		_SetVerticalItemCount(m_cmbEngine , s_nHeightCountCnt);
		_SetVerticalItemCount(m_cmbKeyword, s_nHeightCountCnt);
	} else {	 //+++ vista以外でコンボボックスの高さがおかしい件を強制的に回避してみる...
		_SetVerticalItemCount(m_cmbEngine , MAXHEIGHTCOUNT);
		_SetVerticalItemCount(m_cmbKeyword, DEFAULT_HEIGHTCOUNT/*50*/);
	}

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
		int 	h		= rcEngine.Height();
		rcEngine.left	= 0;
		rcEngine.right	= m_nEngineWidth;
		rcEngine.top	= 0;
		rcEngine.bottom = h;
		m_cmbEngine.SetWindowPos(NULL, rcEngine, SWP_NOZORDER | SWP_NOSENDCHANGING);
	}

	/// サブクラス化
	m_wndKeyword.SubclassWindow( m_editKeyword );
	m_wndEngine.SubclassWindow( m_cmbEngine );
	m_wndCmbKeyword.SubclassWindow( m_cmbKeyword );

	//ツールバー初期化
	{
		CIniFileI		pr( g_szIniFileName, _T("SEARCH") );

		COLORREF clrMask = RGB(255, 0, 255);
		CImageList	imgs;
		MTLVERIFY( imgs.Create(cxTbBmp, cyTbBmp, ILC_COLOR24 | ILC_MASK, 1, 1) );
		CBitmap 	bmp = AtlLoadBitmapImage((LPCTSTR)GetSkinSeachBarImagePath(SBSKN_NORMAL), LR_LOADFROMFILE);
		if (bmp.m_hBitmap == NULL)	//+++	skinの処理がここにあるので、デフォルト素材もこの場で設定にしとく...
			bmp.LoadBitmap(IDB_SEARCHBUTTON/*nImageBmpID*/);
		imgs.Add(bmp, clrMask);

		CImageList	imgsHot;
		MTLVERIFY( imgsHot.Create(cxTbBmp, cyTbBmp, ILC_COLOR24 | ILC_MASK, 1, 1) );
		CBitmap 	bmpHot = AtlLoadBitmapImage((LPCTSTR)GetSkinSeachBarImagePath(SBSKN_HOT), LR_LOADFROMFILE);
		if (bmpHot.m_hBitmap == NULL)	//+++	skinの処理がここにあるので、デフォルト素材もこの場で設定にしとく...
			bmpHot.LoadBitmap(IDB_SEARCHBUTTON_HOT/*nHotImageBmpID*/);		
		imgsHot.Add(bmpHot, clrMask);

	  #if 1 //+++ Disableも用意する
		CString strDisableImgPath = GetSkinSeachBarImagePath(SBSKN_DISABLE);
		bool 	dis = false;
		if (::PathFileExists(strDisableImgPath) == FALSE) {		//+++ 画像ファイルがない時
			if (::PathFileExists(GetSkinSeachBarImagePath(SBSKN_NORMAL))) {		//+++ 通常があれば
				strDisableImgPath = GetSkinSeachBarImagePath(SBSKN_NORMAL);		//+++ 通常画で代用
			} else {											//+++ 通常もなければ
				dis = true; 					//+++ デフォルトのDisable画を使う.
			}
		}
		CImageList	imgsDis;
		MTLVERIFY( imgsDis.Create(cxTbBmp, cyTbBmp, ILC_COLOR24 | ILC_MASK, 1, 1) );
		CBitmap 	bmpDis = AtlLoadBitmapImage((LPCTSTR)strDisableImgPath, LR_LOADFROMFILE);
		if (bmpDis.m_hBitmap == NULL && dis)	//+++	skinの処理がここにあるので、デフォルト素材もこの場で設定にしとく...
			bmpDis.LoadBitmap(IDB_SEARCHBUTTON_DIS);
		imgsDis.Add(bmpDis, clrMask);
	  #endif

		//+++ 単語ボタンでなく、数字だけの5ボタンを使うか? (NoWordButton=1の時のみ有効)
		m_dwTinyWordButton = pr.GetValue(_T("NumberButton"));		

		/* ツールバーを作る */
		DWORD		flags  =  WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
							| CCS_NODIVIDER  /*| CCS_NORESIZE */ | CCS_NOPARENTALIGN | CCS_TOP
							| TBSTYLE_TOOLTIPS | TBSTYLE_FLAT ;
		m_wndToolBar.Create(m_hWnd, rcDefault, _T("SearchWordButton"), flags);
		m_wndToolBar.SetButtonStructSize();

		if (s_bNoWordButton == false || m_dwTinyWordButton)
		{	//+++ 追加 (検索単語の文字列を表示するには必要)
			m_wndToolBar.SetStyle(m_wndToolBar.GetStyle() | TBSTYLE_LIST | TBSTYLE_TRANSPARENT);
		}

		// イメージリストを設定
		m_wndToolBar.SetImageList(imgs);
		m_wndToolBar.SetHotImageList(imgsHot);
		m_wndToolBar.SetDisabledImageList(imgsDis); //+++

		_AddToolBarIcon(CString());	// ツールバーにボタンを追加する
	}

	// ドラッグドロップ初期化
	RegisterDragDrop();

	// スレッドを利用してコンボボックスにデータを登録(INIからの読み込みに時間がかかるため)
	m_threadInitComboBox	= boost::thread(boost::bind(&CDonutSearchBar::Impl::_threadInitComboBox, this));

	// SetCmbKeywordEmptyStr();	//+++ どうせまだエンジン名が登録されていないので、このタイミングはなし

	return 0;
}

void	CDonutSearchBar::Impl::OnDestroy()
{
	m_threadInitComboBox.join();

	if (s_bNoWordButton) { //+++ 旧処理のときの辻褄あわせ
		CRect			rcEngine;
		m_cmbEngine.GetWindowRect(rcEngine);
		m_nEngineWidth = rcEngine.Width();
	}

	{	// 設定保存
		CIniFileIO	pr( g_szIniFileName, _T("SEARCH") );
		pr.SetValue( (DWORD) m_nKeywordWidth, _T("KeywordWidth") );
		pr.SetValue( (DWORD) m_nEngineWidth  , _T("EngWidth") );

		if (s_bLastSel)
			pr.SetValue( m_cmbEngine.GetCurSel(), _T("SelIndex") );
	}

	// サブクラス化解除
	m_wndKeyword.UnsubclassWindow();
	m_wndEngine.UnsubclassWindow();
	m_cmbKeyword.UnsubclassWindow();

	// Drag&Drop登録解除
	RevokeDragDrop();

	_SaveHistory();
}

/// ウィンドウのサイズ変更時
void CDonutSearchBar::Impl::OnSize(UINT nType, CSize size)
{
	//+++ 検索単語を項目にするため、検索バーのサイズ調整方法を変更.
	if (s_bNoWordButton == false) { //+++ 単語ボタンありのとき
		_ResizeBar();
	} else {
		//+++ 単語ボタン無しの場合は旧処理.
		ATLASSERT(m_wndKeyword.IsWindow());

		CRect	rcDlg;
		GetClientRect(&rcDlg);
		if (rcDlg.right == 0)
			return ;	// 幅が0なので帰る

		// ツールバー移動
		CString str;
		CRect	rcToolbar(rcDlg);
		rcToolbar.left = rcDlg.right - m_cxBtnsBtn - 10;
		m_wndToolBar.SetWindowPos(NULL, rcToolbar, SWP_NOZORDER | SWP_NOSENDCHANGING);

		// 検索エンジンコンボボックス移動
		CRect	rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		int 	nEngineCX = rcEngine.Width();
		int 	nEngineCY = rcEngine.Height();
		if (nEngineCX <= btnWidth())	//+++
			nEngineCX = btnWidth();		// 小さければ強制的につまみの幅にする
		m_nKeywordWidth	 = nEngineCX;

		rcEngine		 = rcToolbar;
		rcEngine.right	 = rcToolbar.left - s_kcxGap;
		rcEngine.left	 = rcEngine.right - nEngineCX;
		rcEngine.top	 = m_nDefEditT; //minit
		rcEngine.bottom  = rcEngine.top + nEngineCY;
		m_cmbEngine.SetWindowPos(NULL, rcEngine, SWP_NOZORDER | SWP_NOSENDCHANGING);

		// キーワードコンボボックス移動
		CRect	rcKeyword(rcEngine);
		rcKeyword.left	= 0;
		rcKeyword.right = rcEngine.left - s_kcxGap;
		m_cmbKeyword.SetWindowPos(NULL, rcKeyword, SWP_NOZORDER | SWP_NOSENDCHANGING);
	  #if 1	//+++ 暫定対処... 本質的に修正できていないがon_
		m_cmbKeyword.SetEditSel(0, 0);	//+++ 無理やり範囲解除
	  #endif
		_SetCmbKeywordEmptyStr();	//+++

		m_wndToolBar.InvalidateRect(NULL, TRUE);
	}
}

/// リバーの背景をコピーする
BOOL CDonutSearchBar::Impl::OnEraseBkgnd(CDCHandle dc)
{
	// CAddressBarCtrlImplからコピペ
	HWND	hWnd	= GetParent();
	CPoint	pt;

	MapWindowPoints(hWnd, &pt, 1);
	::OffsetWindowOrgEx( (HDC) dc.m_hDC, pt.x, pt.y, NULL );
	LRESULT lResult = ::SendMessage(hWnd, WM_ERASEBKGND, (WPARAM)dc.m_hDC, 0L);
	::SetWindowOrgEx((HDC) dc.m_hDC, 0, 0, NULL);
	return 1; //lResult;
}

/// マウス移動
void	CDonutSearchBar::Impl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (s_bNoWordButton) {	//+++ 単語ボタン無しの場合は旧処理.
		
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);
		if ( abs(rcKeyword.right - point.x) > 5 && ::GetCapture() != m_hWnd )
			return;

		if (m_hCursor == NULL)
			m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
		::SetCursor(m_hCursor);

		if ( (nFlags & MK_LBUTTON) ) {
			_UpdateLayout(point);
		}
	} else {	//+++ 単語ボタンありの場合
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);
		if ((abs(rcKeyword.right - point.x) <= 6 || (m_has == 1 && ::GetCapture() == m_hWnd))
			&& (rcKeyword.top <= point.y && point.y < rcKeyword.bottom) ) 
		{
			m_has = 1;
			if (m_hCursor == NULL)
				m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
			::SetCursor(m_hCursor);
			if ( (nFlags & MK_LBUTTON) ) {
				_UpdateLayout(point);
			}
			return;
		}

		CRect	rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		ScreenToClient(&rcEngine);
		if ( (abs(rcEngine.right - point.x) <= 6 || (m_has == 2 && ::GetCapture() == m_hWnd))
			&& (rcEngine.top <= point.y && point.y < rcEngine.bottom) ) 
		{
			m_has = 2;
			if (m_hCursor == NULL)
				m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
			::SetCursor(m_hCursor);
			if ( (nFlags & MK_LBUTTON) ) {
				_UpdateLayout2(point);
			}
			return;
		}
		m_has = 0;
	}
}

/// 右クリック(検索バーに乗ってるウィンドウを動かす処理)
void	CDonutSearchBar::Impl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (s_bNoWordButton) {
		//+++ 単語ボタンなしの場合は旧処理
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);

		if (abs(rcKeyword.right - point.x) > 5)
			return;

		SetCapture();
		::SetCursor(m_hCursor);
		m_ptDragStart = point;
		m_ptDragHist  = point;
	} else {	//+++ 単語ボタンありの場合
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);
		// キーワードコンボボックスと検索エンジンコンボボックスの間なら
		if (abs(rcKeyword.right - point.x) <= 5) {	
			m_has = 1;		
			SetCapture();
			::SetCursor(m_hCursor);
			m_ptDragStart = point;
			m_ptDragHist  = point;
			return;
		}
		CRect	rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		ScreenToClient(&rcEngine);
		// 検索エンジンComboBoxとツールバーの間なら
		if (abs(rcEngine.right - point.x) <= 5) {
			m_has = 2;
			SetCapture();
			::SetCursor(m_hCursor);
			m_ptDragStart = point;
			m_ptDragHist  = point;
			return;
		}
		m_has = 0;
	}
}

/// 右クリック(検索バーに乗ってるウィンドウを動かす処理)
void	CDonutSearchBar::Impl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (::GetCapture() != m_hWnd)
		return;

	::ReleaseCapture();


	if (s_bNoWordButton) {	//+++ 単語ボタンなしのとき
		_UpdateLayout(point);
	} else {				//+++ 単語ボタンありの場合
		if (m_has == 1)
			_UpdateLayout(point);
		else if (m_has == 2)
			_UpdateLayout2(point);
	}
}

void	CDonutSearchBar::Impl::OnContextMenu(CWindow wnd, CPoint point)
{
	if (wnd.m_hWnd == m_wndToolBar.m_hWnd) {
		CPoint pt;
		::GetCursorPos(&pt);
		m_wndToolBar.ScreenToClient(&pt);
		int nIndex = m_wndToolBar.HitTest(&pt);

		if (nIndex == 0) {	// 右クリックがWEB検索ボタンの範囲内で押された場合、検索エンジンメニューを出す
			OnSearchEngineMenu(0, 0, 0);
			return;
		}
	} else if (wnd.m_hWnd == m_cmbEngine.m_hWnd) {
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
}

/// 検索
void	CDonutSearchBar::Impl::OnSearchWeb(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	SearchWeb();
}

/// ページ内検索
void	CDonutSearchBar::Impl::OnSearchPage(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	SearchPage(!(::GetKeyState(VK_SHIFT) < 0));
}

/// ハイライト
void	CDonutSearchBar::Impl::OnSearchHilight(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	SearchHilight();
}

/// 単語ボタン
void	CDonutSearchBar::Impl::OnSearchWord(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	SearchPage(!(::GetKeyState(VK_SHIFT) < 0), nID - ID_SEARCHBAR_WORD00);
}

/// 単語ボタンのロック用
void	CDonutSearchBar::Impl::OnParentNotify(UINT message, UINT nChildID, LPARAM lParam)
{
	if (nChildID == 0) {
		if (message == WM_MBUTTONDOWN) {
			if (m_bWordLock) {	// もう一度ミドルクリックすると解除
				m_bWordLock = false;
				_AddToolBarIcon(GetSearchStr());
				return;
			}

			CPoint pt;
			::GetCursorPos(&pt);
			m_wndToolBar.ScreenToClient(&pt);
			int nIndex = m_wndToolBar.HitTest(&pt);
			if (nIndex != -1) {
				int nCmdWord00 = m_wndToolBar.CommandToIndex(ID_SEARCHBAR_WORD00);
				if (nCmdWord00 <= nIndex) {
					int nID = ID_SEARCHBAR_WORD00 + (nIndex - nCmdWord00);
					CString strText;
					m_wndToolBar.GetButtonText(nID, strText.GetBuffer(1024));
					strText.ReleaseBuffer();
					_AddToolBarIcon(strText);
					m_bWordLock = true;	// 単語ボタンのロック
				}
			}
		}
	}
}


/// 検索エンジンメニューを開く
void	CDonutSearchBar::Impl::OnSearchEngineMenu(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	// ポップアップメニューを開く.
	::SetForegroundWindow(m_hWnd);
	POINT 	pt;
	::GetCursorPos(&pt);
	CMenu 	menu = GetSearchEngineMenuHandle();
	// ポップアップメニューを表示
	HRESULT hr = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
	if (hr < ID_INSERTPOINT_SEARCHENGINE || hr > ID_INSERTPOINT_SEARCHENGINE_END)
		return ;	// なにも選択されなかった

	// 選択されたものからサーチエンジン名を取得.
	hr -= ID_INSERTPOINT_SEARCHENGINE;
	CString 	strEngine;
	if (menu.GetMenuString(hr, strEngine, MF_BYPOSITION) == 0)
		return ;

	// 選択されたエンジンで、現在の検索文字列をweb検索.
	CString		strKeyword = GetSearchStr();
	SearchWebWithEngine(strKeyword, strEngine);
	return ;
}

/// ツールバーボタンの説明を表示
LRESULT CDonutSearchBar::Impl::OnToolBarGetDispInfo(LPNMHDR pnmh)
{
	LPNMTTDISPINFO pDispInfo  = (LPNMTTDISPINFO) pnmh;
	pDispInfo->szText[0] = _T('\0');
	if (pDispInfo->uFlags & TTF_IDISHWND)
		return 0;

	static CString strToolTip;
	strToolTip.Format(_T("\"%s\" を"), GetSearchStr());

	switch (pnmh->idFrom) {
	case ID_SEARCH_WEB:
		strToolTip += _T(" \"") + GetSearchEngineStr() + _T("\" で検索する");
		break;
	case ID_SEARCHBAR_HILIGHT:
		strToolTip += _T("ハイライトする");
		break;
	case ID_SEARCH_PAGE:
		strToolTip += _T("このページで検索する(↑:Shift+Enter ↓:Ctrl+Enter)");
		break;
	default:	// ID_SEARCH_WORD00..09
		return 0;
	}
	pDispInfo->lpszText = strToolTip.GetBuffer(0);
	return 0;
}

/// キーワードComboBoxの選択が変わった
/// 設定してあれば検索をかける
void	CDonutSearchBar::Impl::OnKeywordSelChange(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	bool bSts = s_bKeyChgGo;
	if (::GetKeyState(VK_SHIFT) < 0)
		bSts = !bSts;

	if (bSts)
		_OnEnterKeyDown();
}


/// 検索エンジンComboBoxの選択が変わった
void	CDonutSearchBar::Impl::OnEngineSelChange(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (::GetKeyState(VK_RBUTTON) < 0) {	// 検索エンジンのプロパティを開く
		int nIndex = m_EngineListBox.GetCaretIndex();
		CString 			strText;
		m_cmbEngine.GetLBText(nIndex, strText);
		CExPropertyDialog	dlg(GetSearchIniPath(), strText, 0);
		dlg.SetTitle(strText);
		dlg.DoModal();
		_SetCmbKeywordEmptyStr();	//+++
		return;
	}

	bool bSts = s_bEngChgGo;
	if (::GetKeyState(VK_SHIFT) < 0) 
		bSts = !bSts;

	if (bSts) {
		//+++ エンジンが切り替わった場合は、検索窓が空でもurl検索の場合があるので、こちら
		//x _OnEnterKeyDown(ENTER_KEYDOWN_SELCHANGE);
		SearchWeb();
	}
}


// KeywordEdit

/// キーワードコンボボックス内のエディットコントロールでキーが押された(PretranslateMessageから呼ばれる)
void CDonutSearchBar::Impl::OnKeywordKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SetMsgHandled(TRUE);

	// if not dropped, eat VK_DOWN
	if (   m_cmbKeyword.GetDroppedState() == FALSE 
		&& (nChar == VK_DOWN || nChar == VK_UP) ) {
		int nIndexEngine  = m_cmbEngine.GetCurSel();
		int nIndexKeyword = m_cmbKeyword.GetCurSel();

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
			_OnEnterKeyDown();	// 検索

		} else if (nChar == VK_DELETE) {
			if ( m_cmbKeyword.GetDroppedState() ) {
				auto funcDeleteKeywordHistory = [this] () -> bool {
					int nIndex = m_KeywordListBox.GetCurSel();
					if (nIndex == LB_ERR)
						return false;

					m_cmbKeyword.DeleteString(nIndex);
					return true;
				};
				funcDeleteKeywordHistory();
					//SetMsgHandled(FALSE);
			}
		} else if (nChar == VK_TAB) {
			m_cmbEngine.SetFocus();
		} else {
			SetMsgHandled(FALSE);
		}
	}

}

/// キーワードボックスからフォーカスが外れた
void CDonutSearchBar::Impl::OnKeywordKillFocus(CWindow wndFocus)
{
	//SetMsgHandled(false);
	m_wndKeyword.DefWindowProc();
	//_AddToolBarIcon(GetSearchStr());	/* ツールバーを更新する */
}


void	CDonutSearchBar::Impl::OnEditLButtonDown(UINT nFlags, CPoint point)
{
	m_wndKeyword.DefWindowProc();

	if (::GetFocus() != m_editKeyword.m_hWnd) {
		m_editKeyword.SetFocus();
		m_editKeyword.SetSelAll();
	}
}

void	CDonutSearchBar::Impl::OnEditRButtonUp(UINT nFlags, CPoint point)
{
	if (::GetFocus() != m_editKeyword.m_hWnd) {
		m_editKeyword.SetFocus();
		m_editKeyword.SetSelAll();
	}
	SetMsgHandled(FALSE);
}


void	CDonutSearchBar::Impl::OnEditContextMenu(CWindow wnd, CPoint point)
{
	enum {
		ID_UNDO			= 1,
		ID_CUT			= 2,
		ID_COPY			= 3,
		ID_PASTE		= 4,
		ID_PASTEANDMOVE	= 5,
		ID_CLEAR		= 6,
		ID_SELALL		= 7,
	};

	UINT nFlags = 0;
	CMenu menu;
	menu.CreatePopupMenu();
	{
		nFlags = m_editKeyword.CanUndo() ? 0 : MFS_DISABLED;
		menu.AppendMenu(nFlags, (UINT_PTR)ID_UNDO		  , _T("元に戻す(&U)"));
	}
	menu.AppendMenu(MF_SEPARATOR);

	int nStart, nEnd;
	m_editKeyword.GetSel(nStart, nEnd);
	{
		nFlags = (nStart == nEnd) ? MFS_DISABLED : 0;
		menu.AppendMenu(nFlags, (UINT_PTR)ID_CUT		  , _T("切り取り(&T)"));
	}
	{
		nFlags = (nStart < nEnd) ? 0 : MFS_DISABLED;
		menu.AppendMenu(nFlags, (UINT_PTR)ID_COPY		  , _T("コピー(&C)"));
	}
	CString strClipboard = MtlGetClipboardText();
	{
		nFlags = strClipboard.IsEmpty() ? MFS_DISABLED : 0;
		menu.AppendMenu(nFlags, (UINT_PTR)ID_PASTE		 , _T("貼り付け(&P)"));
		menu.AppendMenu(nFlags, (UINT_PTR)ID_PASTEANDMOVE, _T("クリップボードから検索(&S)"));
	}
	{
		nFlags = (nStart < nEnd) ? 0 : MFS_DISABLED;
		menu.AppendMenu(nFlags, (UINT_PTR)ID_CLEAR		 , _T("削除(&D)"));
	}
	menu.AppendMenu(MF_SEPARATOR);
	{
		if (nStart == 0 && nEnd == MtlGetWindowText(m_editKeyword).GetLength())
			nFlags = MFS_DISABLED;
		else
			nFlags = 0;
		menu.AppendMenu(nFlags, (UINT_PTR)ID_SELALL		 , _T("すべて選択(&A)"));
	}

	int nRet = menu.TrackPopupMenu(TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, m_hWnd);
	if (nRet == 0)
		return;

	switch (nRet) {
	case ID_UNDO:		
		m_editKeyword.Undo();
		break;

	case ID_CUT:			
		m_editKeyword.Cut();
		break;

	case ID_COPY:	
		m_editKeyword.Copy();
		break;

	case ID_PASTE:		
		m_editKeyword.Paste();
		break;

	case ID_PASTEANDMOVE:
		SearchWeb(strClipboard);
		break;

	case ID_CLEAR:		
		m_editKeyword.Clear();
		break;

	case ID_SELALL:		
		m_editKeyword.SetSelAll();
		break;
	}
}


/// リアルタイムに単語ボタンに反映していく
LRESULT CDonutSearchBar::Impl::OnEditChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (s_bNoWordButton == false) {
		//SetMsgHandled(false);
		m_bWordLock = false;

		int nIndex = m_wndToolBar.CommandToIndex(ID_SEARCHBAR_WORD00);
		while (m_wndToolBar.DeleteButton(nIndex)) ;

		CString str = GetSearchStr();
		_RemoveShortcutWord(str);
		if (s_bFiltering)
			FilterString(str);

		vector<CString> strs;
		strs.reserve(20);
		Misc::SeptTextToWords(strs, str);	// strを単語に分解する

		//+++ vectorで渡された複数の文字列(20個まで)を検索ツールバーに登録.
		TBBUTTONINFO	bi = { sizeof(TBBUTTONINFO) };
		bi.dwMask	= TBIF_TEXT/* | TBIF_STYLE*/;
		bi.fsStyle |= TBSTYLE_AUTOSIZE | TBSTYLE_NOPREFIX;

		size_t n	= strs.size();
		if (n > 20) n = 20;
		for (int i = 0; i < (int)n; ++i) {
			enum { STYLE = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | TBSTYLE_NOPREFIX }; // | TBSTYLE_LIST | TBSTYLE_TRANSPARENT };
			TBBUTTON	btn = { 2/*I_IMAGENONE*/, ID_SEARCHBAR_WORD00 +i, TBSTATE_ENABLED, STYLE };
			MTLVERIFY( m_wndToolBar.AddButton(&btn) );

			bi.pszText = strs[i].GetBuffer(0);
			MTLVERIFY( m_wndToolBar.SetButtonInfo(ID_SEARCHBAR_WORD00 + i, &bi) );
		}

		m_wndToolBar.SetButtonSize(m_ButtonSize);	
	}
	return 0;
}


// SearchEngineComboBox
/// 検索エンジンComboBoxでEnterキーを押した場合検索する
void	CDonutSearchBar::Impl::OnEngineKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_wndEngine.DefWindowProc();
	if (nChar == VK_RETURN)
		_OnEnterKeyDown();
}

/// 検索エンジンリストボックスを隠れないように移動する
LRESULT CDonutSearchBar::Impl::OnShowEngineListBox(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);

	HWND hWndListBox = (HWND)lParam;
	HMONITOR hMoni = MonitorFromWindow(hWndListBox, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info = { sizeof (info) };
	GetMonitorInfo(hMoni, &info);

	CRect rcList;
	::GetWindowRect(hWndListBox, &rcList);
	if (info.rcWork.right <= rcList.right) {
		int nDiff = rcList.right - info.rcWork.right;
		rcList.left -= nDiff;
		rcList.right-= nDiff;
		::MoveWindow(hWndListBox, rcList.left, rcList.top, rcList.Width(), rcList.Height(), FALSE);
	}
	return 0;
}


// private:

/// ツールバーアイコンを追加する
void CDonutSearchBar::Impl::_AddToolBarIcon(CString strWord)
{
	if (m_bWordLock)
		return;	// 単語ロック状態だと何もしない

	CString str;
	if (strWord.IsEmpty())
		str = _GetSelectText(GetEditCtrl());
	else 
		str = strWord;
	_RemoveShortcutWord(str);
	if (s_bFiltering)
		FilterString(str);

	vector<CString> strs;
	strs.reserve(20);
	Misc::SeptTextToWords(strs, str);	// strを単語に分解する

	// ツールバーボタンを一旦全部削除する
	while (m_wndToolBar.GetButtonCount())
		m_wndToolBar.DeleteButton(0);

	int hilightStat = m_bHilightSw ? TBSTATE_PRESSED : TBSTATE_ENABLED;
	if (strs.size() == 0 || s_bNoWordButton || s_bUsePageButton) {	//+++ 旧来の検索 (単独のページ内検索ボタンあり)
		if (s_bNoWordButton && m_dwTinyWordButton) {
			_AddDefaultToolBarIcon_tinyWordButton(strs.size());
			return;
		}

		static TBBUTTON	btns[] = {
			{ 0 , ID_SEARCH_WEB,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE },// Web
			{ 2 , ID_SEARCH_PAGE,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE },// Page
			{ 1 , ID_SEARCHBAR_HILIGHT, hilightStat    , TBSTYLE_CHECK	| TBSTYLE_AUTOSIZE },// Hilight
		};
		m_wndToolBar.AddButtons(3, btns);

		m_wndToolBar.SetBitmapSize(14,14);
		if (s_bNoWordButton)
			m_cxBtnsBtn 	 = (20) * 3 + 1;

	} else {				//+++ 単語専用ボタン有り

		static TBBUTTON	btns[] = {
			{ 0 , ID_SEARCH_WEB,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE },// Web
			{ 1 , ID_SEARCHBAR_HILIGHT, hilightStat    , TBSTYLE_CHECK	| TBSTYLE_AUTOSIZE },// Hilight
		};
		m_wndToolBar.AddButtons(2, btns);

	}

	if (s_bNoWordButton == false) {
		//+++ vectorで渡された複数の文字列(20個まで)を検索ツールバーに登録.
		TBBUTTONINFO	bi = { sizeof(TBBUTTONINFO) };
		bi.dwMask	= TBIF_TEXT/* | TBIF_STYLE*/;
		bi.fsStyle |= TBSTYLE_AUTOSIZE | TBSTYLE_NOPREFIX;

		size_t n	= strs.size();
		if (n > 20) n = 20;
		for (int i = 0; i < (int)n; ++i) {
			enum { STYLE = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | TBSTYLE_NOPREFIX }; // | TBSTYLE_LIST | TBSTYLE_TRANSPARENT };
			TBBUTTON	btn = { 2/*I_IMAGENONE*/, ID_SEARCHBAR_WORD00 +i, TBSTATE_ENABLED, STYLE };
			MTLVERIFY( m_wndToolBar.AddButton(&btn) );

			bi.pszText = strs[i].GetBuffer(0);
			MTLVERIFY( m_wndToolBar.SetButtonInfo(ID_SEARCHBAR_WORD00 + i, &bi) );
		}

		m_wndToolBar.SetButtonSize(m_ButtonSize);
	//	m_wndToolBar.AutoSize();
	//	m_wndToolBar.Invalidate();
		//ShowToolBarIcon(true);
	}
}


//+++ 単語ボタンの代わりに1～5の数字ボタンを設定.
//		※単語ボタンが実装されるまでの実験/代用処理だったもの.
//		  単語ボタンを使用しない設定で、かつ、noguiに"NumberButton=1"を設定したときのみ利用可能.
int	 CDonutSearchBar::Impl::_AddDefaultToolBarIcon_tinyWordButton(size_t nWordCount)
{
	if (nWordCount > 5)
		nWordCount = 5;
	int hilightStat = m_bHilightSw ? TBSTATE_PRESSED : TBSTATE_ENABLED;

	static TBBUTTON	btns[] = {
		{ 0 , ID_SEARCH_WEB,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE },	// Web
		{ 2 , ID_SEARCH_PAGE,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE },	// Page
		{ 1 , ID_SEARCHBAR_HILIGHT, hilightStat	   , TBSTYLE_CHECK	| TBSTYLE_AUTOSIZE },	// Hilight
	};
	MTLVERIFY( m_wndToolBar.AddButtons(3, btns) );
	m_cxBtnsBtn	 = (20) * (3+5) + 1;

	for (int i = 0; i < 5; ++i) {
		enum { STYLE = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE }; // | TBSTYLE_LIST | TBSTYLE_TRANSPARENT };
		TBBUTTON	btn = { I_IMAGENONE, ID_SEARCHBAR_WORD00 + i, TBSTATE_ENABLED, STYLE };
		MTLVERIFY( m_wndToolBar.AddButton(&btn) );

		TBBUTTONINFO bi = { sizeof(TBBUTTONINFO) };
		bi.dwMask	= TBIF_TEXT	/* | TBIF_STYLE*/;
		bi.fsStyle |= TBSTYLE_AUTOSIZE /*| TBBS_NOPREFIX*/ ;
		TCHAR str[4];
		str[0]		= _T('1'+i);
		str[1]		= _T('\0');
		bi.pszText	= str;
		MTLVERIFY( m_wndToolBar.SetButtonInfo(ID_SEARCHBAR_WORD00 + i, &bi) );
	}

	// サイズ調整など
	m_wndToolBar.SetMaxTextRows(1);
	CRect rcButton;
	m_wndToolBar.GetItemRect(3, rcButton);
	m_wndToolBar.SetButtonSize(rcButton.Size());
	m_wndToolBar.Invalidate();

	m_wndToolBar.AutoSize();
	return 3;
}

/// ショートカットワードを削除したものを返す
void CDonutSearchBar::Impl::_RemoveShortcutWord(CString& str)
{
	if (s_bUseShortcut) {
		if (str.Find( _T("\\") ) == 0 || str.Find( _T("/") ) == 0) {
			int nPos = str.Find( _T(" ") );

			if (nPos != -1)
				str = str.Mid(nPos + 1);
		}
	}
}

/// スレッドを使ってコンボボックスにデータを追加する
void CDonutSearchBar::Impl::_threadInitComboBox()
{
	{	// 検索エンジンをコンボボックスに登録する
		CComboBox	cmb = m_cmbEngine;

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
		if (s_bLastSel && nIndex < nListCnt) 
			nSelIndex = nIndex;			// 最後に選択したサーチエンジンを復元する
		
		cmb.SetCurSel(nSelIndex);
	}

	{	// 検索履歴をコンボボックスに登録する
		if (s_bHistorySave == false) 
			return;

		CComboBox	cmb = m_cmbKeyword;
	
		CIniFileI	pr( _GetFilePath( _T("WordHistory.ini") ), _T("SEARCH_HISTORY") );
		int	nHistoryCnt = pr.GetValuei(_T("HistorySaveCnt"));

		for (int ii = 0; ii < nHistoryCnt; ii++) {
			CString 	strKey;
			strKey.Format(_T("KEYWORD%d"), ii);
			CString 	strKeyWord = pr.GetStringUW( strKey );
			if ( strKeyWord.IsEmpty() )
				continue;

			cmb.AddString(strKeyWord);
		}
	}
}


///+++ キーワード入力欄が空の時にエンジン名を表示するための設定
void CDonutSearchBar::Impl::_SetCmbKeywordEmptyStr()
{
	m_cmbKeyword.setEmptyStr(GetSearchEngineStr(), IDC_EDIT/*1001*/, (m_nEngineWidth <= 8 + btnWidth()));
	m_cmbKeyword.redrawEmptyStr();
}

///+++ バーのサイズ変更. キーワード｜エンジン｜ツールバー  の境目での移動幅を順にdifA,difBとする.
void CDonutSearchBar::Impl::_ResizeBar(int difA, int difB)
{
	CRect	rcDlg;
	GetClientRect(&rcDlg);
	if (rcDlg.right == 0) 
		return;

	// キーワードコンボボックスを移動
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

	// 検索エンジンコンボボックスを移動
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

	// ツールバーを移動
	CRect	rcToolbar(rcDlg);
	rcToolbar.left = rcEngine.right + s_kcxGap;
	if (rcToolbar.left > rcToolbar.right)	// 左が右を超えてるなんて変なことになった場合
		rcToolbar.left = rcToolbar.right;
	m_wndToolBar.SetWindowPos(NULL, rcToolbar, SWP_NOZORDER | SWP_NOSENDCHANGING);
	m_wndToolBar.InvalidateRect(NULL, TRUE);
}



void CDonutSearchBar::Impl::_UpdateLayout(const CPoint& pt)
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
		_ResizeBar(pt.x - m_ptDragStart.x, 0);
		m_ptDragStart	 = pt;
		UpdateWindow();
	}
}


void CDonutSearchBar::Impl::_UpdateLayout2(const CPoint& pt)
{
	_ResizeBar(0, pt.x - m_ptDragStart.x);
	m_ptDragStart	 = pt;
	UpdateWindow();
}

void CDonutSearchBar::Impl::_OnEnterKeyDown()
{
	CString  str;

	int nIndexCmb = m_cmbKeyword.GetCurSel();
	if (nIndexCmb == -1) {
		str = MtlGetWindowText(m_cmbKeyword);
	} else {
		m_cmbKeyword.GetLBText(nIndexCmb, str);
	}

	SetSearchStr(str);	// 単語ボタンを設定

	if ( str.IsEmpty() == FALSE ) {
		SHORT sShift = ::GetKeyState(VK_SHIFT);
		SHORT sCtrl  = ::GetKeyState(VK_CONTROL);

		if (sShift >= 0 && sCtrl >= 0) {	// ShiftもCtrlも押されていない
			SearchWeb(str);
		} else {
		  #if 1 //+++ カーソル位置の単語だけを選択するようにしてみる.
			str = _GetSelectText_OnCursor( GetEditCtrl() );
		  #else
			str = _GetSelectText( GetEditCtrl() );
		  #endif

			if (sCtrl < 0)			
				SendMessage(GetTopLevelParent(), WM_USER_FIND_KEYWORD, (WPARAM) str.GetBuffer(0), TRUE );	// ↓検索
			else if (sShift < 0)	
				SendMessage(GetTopLevelParent(), WM_USER_FIND_KEYWORD, (WPARAM) str.GetBuffer(0), FALSE);	// ↑検索
		}
	} else {	// DropDownリストビューを消す
		m_cmbEngine.ShowDropDown(FALSE); //minit
	}
}

/// 重複してたら消して検索履歴に追加する
void CDonutSearchBar::Impl::_AddToSearchBoxUnique(const CString& str)
{
	// search the same string
	int nCount = m_cmbKeyword.GetCount();
	for (int n = 0; n < nCount; ++n) {
		CString 	temp;
		m_cmbKeyword.GetLBText(n, temp);
		if (str == temp) {
			m_cmbKeyword.DeleteString(n);
			break;
		}
	}

	m_cmbKeyword.InsertString(0, str);
	m_cmbKeyword.SetCurSel(0);

	boost::thread(boost::bind(&CDonutSearchBar::Impl::_SaveHistory, this));
	//_SaveHistory();	// 追加のたびに保存してみる
}

/// 文字列を指定された方法でエンコードする
void CDonutSearchBar::Impl::_EncodeString(CString& str, int dwEncode)	//minit
{
  #if 1	//+++ Unicode対応で作り直し
	if	    (dwEncode == ENCODE_SHIFT_JIS)
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

/// 検索履歴を保存する
void CDonutSearchBar::Impl::_SaveHistory()
{
	static bool bSaveing = false;
	if (bSaveing == false) {
		bSaveing = true;
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
		bSaveing = false;
	}
}



// --------------------------------------
// CDonutSearchBar

CDonutSearchBar* CDonutSearchBar::s_pThis = nullptr;

/// コンストラクタ
CDonutSearchBar::CDonutSearchBar() : pImpl(new Impl)
{
	ATLASSERT(s_pThis == nullptr);
	s_pThis = this;
}

/// デストラクタ
CDonutSearchBar::~CDonutSearchBar()
{
	delete pImpl;
}

HWND CDonutSearchBar::Create(HWND hWndParent)
{
	return pImpl->Create(hWndParent);
}

void	CDonutSearchBar::ReloadSkin(int nCmbStyle)
{
	pImpl->ReloadSkin(nCmbStyle);
}

void	CDonutSearchBar::SetFont(HFONT hFont, BOOL bRedraw)
{
	pImpl->SetFont(hFont, bRedraw);
}

CEdit	CDonutSearchBar::GetEditCtrl() const
{
	return pImpl->GetEditCtrl();
}

HWND	CDonutSearchBar::GetHWndToolBar() const
{
	return pImpl->GetHWndToolBar();
}

bool	CDonutSearchBar::ForceSetHilightBtnOn(bool bOn)
{
	return pImpl->ForceSetHilightBtnOn(bOn);
}


bool	CDonutSearchBar::GetHilightSw() const
{
	return pImpl->GetHilightSw();
}

void	CDonutSearchBar::SetSearchStr(const CString& strWord)
{
	return pImpl->SetSearchStr(strWord);
}

CMenuHandle CDonutSearchBar::GetSearchEngineMenuHandle()
{
	return pImpl->GetSearchEngineMenuHandle();
}

const SearchPostData&	CDonutSearchBar::GetSearchPostData() const
{
	return pImpl->GetSearchPostData();
}


void	CDonutSearchBar::SearchWeb(CString str)
{
	pImpl->SearchWeb(str);
}

void	CDonutSearchBar::SearchWebWithEngine(CString str, CString strEngine)
{
	pImpl->SearchWebWithEngine(str, strEngine);
}

void	CDonutSearchBar::SearchWebWithIndex(CString str, int nIndex)
{
	pImpl->SearchWebWithIndex(str, nIndex);
}

void	CDonutSearchBar::SearchPage(bool bForward)
{
	pImpl->SearchPage(bForward);
}

void	CDonutSearchBar::SearchHilight()
{
	pImpl->SearchHilight();
}

void	CDonutSearchBar::SetFocusToEngine()
{
	pImpl->SetFocusToEngine();
}

void	CDonutSearchBar::RefreshEngine()
{
	pImpl->RefreshEngine();
}

BOOL	CDonutSearchBar::PreTranslateMessage(MSG *pMsg)
{
	return pImpl->PreTranslateMessage(pMsg);
}



















