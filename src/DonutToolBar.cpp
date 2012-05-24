/**
 *	@file	DonutToolBar.cpp
 *	@brief	ツールバー
 */

#include "stdafx.h"
#include "DonutToolBar.h"
#include "MtlProfile.h"
#include "DonutPFunc.h"
#include "MtlWin.h"
#include "MtlUser.h"
#include "ChevronHandler.h"
#include "option/ToolBarDialog.h"
#include "ToolTipManager.h"


using namespace 	MTL;


/////////////////////////////////////////////////////////////////////////////
// Adding the special form to the toolbar's customize dialog

class CStdToolBarAdditionalCustomizeDlg
	: public CInitDialogImpl<CStdToolBarAdditionalCustomizeDlg>
	, public CWinDataExchange<CStdToolBarAdditionalCustomizeDlg>
{
public:
	enum { IDD = IDD_FORM_TOOLBAR };

private:
	typedef CInitDialogImpl<CStdToolBarAdditionalCustomizeDlg>	baseClass;

	// Data members
	static HHOOK								s_hCreateHook;
	static CStdToolBarAdditionalCustomizeDlg *	s_pThis;

	CContainedWindow							m_wndParentDlg;
	CContainedWindow							m_wndToolBar;

	CSize	m_sizeDlg;
public:
	DWORD	m_dwStdToolBarStyle;
private:
	int 	m_nIcon;
	int 	m_nText;

public:
	// Ctor/Dtor
	CStdToolBarAdditionalCustomizeDlg() : m_wndParentDlg(this, 1), m_wndToolBar(this, 2) { }

	~CStdToolBarAdditionalCustomizeDlg()
	{
		if ( m_wndParentDlg.IsWindow() )
			m_wndParentDlg.UnsubclassWindow();

		if ( m_wndToolBar.IsWindow() )
			m_wndToolBar.UnsubclassWindow();
	}


	// Methods
	void Install(DWORD dwStyle, HWND hWndToolBar)
	{
		m_wndToolBar.SubclassWindow(hWndToolBar);

		::EnterCriticalSection(&_Module.m_csWindowCreate);

		ATLASSERT(s_hCreateHook == NULL);

		s_pThis 			= this;
		m_dwStdToolBarStyle = dwStyle;

		s_hCreateHook		= ::SetWindowsHookEx( WH_CBT, _CreateHookProc, _Module.GetModuleInstance(), GetCurrentThreadId() );
		ATLASSERT(s_hCreateHook != NULL);
	}


private:
	void Uninstall()
	{
		::UnhookWindowsHookEx(s_hCreateHook);
		s_hCreateHook = NULL;
		s_pThis 	  = NULL;

		::LeaveCriticalSection(&_Module.m_csWindowCreate);
	}


	void _Setup(HWND hWndDlg)
	{
		// uninstall here! otherwize, it will be hanged up.
		Uninstall();

		if (m_wndParentDlg.m_hWnd == NULL) {
			Create(hWndDlg);
			m_wndParentDlg.SubclassWindow(hWndDlg);
			ATLASSERT( ::IsWindow(m_hWnd) );
		}
	}


	// Implementation - Hook procs
	static LRESULT CALLBACK _CreateHookProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRet = 0;

		if (nCode == HCBT_CREATEWND) {
			HWND 	hWndDlg = (HWND) wParam;
			TCHAR	szClassName[16];
			szClassName[0]	= 0;	//+++
			::GetClassName(hWndDlg, szClassName, 7);

			if ( !lstrcmp(_T("#32770"), szClassName) ) {
				//stbTRACE( _T(" Customize dialog found!!\n") );
				s_pThis->_Setup(hWndDlg);
			}
		} else if (nCode < 0) {
			lRet = ::CallNextHookEx(s_hCreateHook, nCode, wParam, lParam);
		}

		return lRet;
	}


	// Message map and handlers
	BEGIN_MSG_MAP( CStdToolBarAdditionalCustomizeDlg )
		MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
		MESSAGE_HANDLER( WM_DESTROY   , OnDestroy	 )
	ALT_MSG_MAP(1) // original dlg
		MESSAGE_HANDLER( WM_INITDIALOG, OnParentInitDialog )
	ALT_MSG_MAP(2)
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_RESET, OnTbnReset )
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &bHandled )
	{
		//stbTRACE( _T("OnInitDialog\n") );
		bHandled  = FALSE;

		CRect rc;
		GetClientRect(&rc);
		m_sizeDlg = CSize( rc.Width(), rc.Height() );

		ExecuteDlgInit();
		_SetData();

		return TRUE;
	}


	LRESULT OnParentInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
	{
		//stbTRACE( _T("OnParentInitDialog\n") );

		LRESULT lRet = m_wndParentDlg.DefWindowProc(uMsg, wParam, lParam);

		CRect	rcParent;
		m_wndParentDlg.GetWindowRect(&rcParent);
		CRect	rcClient;
		m_wndParentDlg.GetClientRect(&rcClient);

		rcParent.bottom += m_sizeDlg.cy;
		m_wndParentDlg.MoveWindow(&rcParent);

		MoveWindow(0, rcClient.Height(), m_sizeDlg.cx, m_sizeDlg.cy);
		ShowWindow(SW_SHOWNORMAL);

		// now no need.
		m_wndParentDlg.UnsubclassWindow();

		return lRet;
	}


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		//stbTRACE( _T("OnDestroy\n") );
		_GetData();
		return 0;
	}


	LRESULT OnTbnReset(LPNMHDR pnmh)
	{
		SetMsgHandled(FALSE);
		m_dwStdToolBarStyle = STD_TBSTYLE_DEFAULT;
		_SetData();
		return 0;
	}


public:
	BEGIN_DDX_MAP(CStdToolBarAdditionalCustomizeDlg)
		DDX_CBINDEX(IDC_COMBO_STDTB_TEXT, m_nText)
		DDX_CBINDEX(IDC_COMBO_STDTB_ICON, m_nIcon)
	END_DDX_MAP()


private:
	void _GetData()
	{
		DoDataExchange(TRUE);

		m_dwStdToolBarStyle = 0;

		if (m_nText == 0)
			m_dwStdToolBarStyle |= STD_TBSTYLE_SHOWLABEL;
		else if (m_nText == 1)
			m_dwStdToolBarStyle |= STD_TBSTYLE_SHOWLABELRIGHT;
		else if (m_nText == 2)
			m_dwStdToolBarStyle |= STD_TBSTYLE_HIDELABEL;

		if (m_nIcon == 0)
			m_dwStdToolBarStyle |= STD_TBSTYLE_SMALLICON;
		else if (m_nIcon == 1)
			m_dwStdToolBarStyle |= STD_TBSTYLE_LARGEICON;
	}


	void _SetData()
	{
		if ( _check_flag(STD_TBSTYLE_HIDELABEL			, m_dwStdToolBarStyle) )
			m_nText = 2;
		else if ( _check_flag(STD_TBSTYLE_SHOWLABEL		, m_dwStdToolBarStyle) )
			m_nText = 0;
		else if ( _check_flag(STD_TBSTYLE_SHOWLABELRIGHT, m_dwStdToolBarStyle) )
			m_nText = 1;
		else
			ATLASSERT(FALSE);

		if ( _check_flag(STD_TBSTYLE_SMALLICON			, m_dwStdToolBarStyle) )
			m_nIcon = 0;
		else if ( _check_flag(STD_TBSTYLE_LARGEICON		, m_dwStdToolBarStyle) )
			m_nIcon = 1;
		else
			ATLASSERT(FALSE);

		DoDataExchange(FALSE);
	}
};

__declspec(selectany) HHOOK 								CStdToolBarAdditionalCustomizeDlg::s_hCreateHook	= NULL;
__declspec(selectany) CStdToolBarAdditionalCustomizeDlg *	CStdToolBarAdditionalCustomizeDlg::s_pThis			= NULL;





/////////////////////////////////////////////////
// CDonutToolBar::Impl

class CDonutToolBar::Impl 
	: public CWindowImpl<Impl, CToolBarCtrl>
	, public CChevronHandler<Impl>
	, protected CToolBarOption
{
	enum { s_kcxSeparator	= 8 };

public:
	DECLARE_WND_SUPERCLASS(_T("DonutToolBar"), CToolBarCtrl::GetWndClassName())

	Impl() { };
	~Impl() { };

	HWND	Create(HWND hWndParent);
	void	SetFont(HFONT hFont) { __super::SetFont(hFont); }
	void	SetDropDownMenu(HMENU hMenu, HMENU hMenuUser, HMENU hMenuCSS);
	void	ReloadSkin();
	void	Customize();
	function<void ()> GetInitButtonfunction();

	// Overrides
	HMENU	ChevronHandler_OnGetChevronMenu(int nCmdID, HMENU &hMenuDestroy);

	// MessageMap
	BEGIN_MSG_MAP( Impl )
		MSG_WM_RBUTTONUP	( OnRButtonUp )
		MSG_WM_LBUTTONDBLCLK( OnLButtonDblClk )
		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnToolTipText)
		REFLECTED_NOTIFY_CODE_HANDLER( RBN_CHEVRONPUSHED, OnChevronPushed)
		REFLECTED_NOTIFY_CODE_HANDLER( TBN_DROPDOWN		, OnDropDown   )
		// Customize
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_QUERYINSERT  , OnTbnQueryInsert	)
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_QUERYDELETE  , OnTbnQueryDelete	)
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_GETBUTTONINFO, OnTbnGetButtonInfo )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_TOOLBARCHANGE, OnTbnToolBarChange )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_RESET 	   , OnTbnReset 		)
	END_MSG_MAP()

	void	OnRButtonUp(UINT nFlags, CPoint point);
	void	OnLButtonDblClk(UINT nFlags, CPoint point);
	LRESULT OnToolTipText(LPNMHDR pnmh);
	LRESULT OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnDropDown(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

	LRESULT OnTbnQueryInsert(LPNMHDR pnmh) { return TRUE; }
	LRESULT OnTbnQueryDelete(LPNMHDR pnmh) { return TRUE; }
	LRESULT OnTbnGetButtonInfo(LPNMHDR pnmh);
	LRESULT OnTbnToolBarChange(LPNMHDR pnmh);
	LRESULT OnTbnReset(LPNMHDR pnmh);

private:
	void	_InitButton();
	void	_UpdateImageLIst();
	void	_AdjustStyle();
	void	_HideLabel();
	void	_ShowLabel();
	void	_ShowLabelRight();
	void	_AddStrings();
	void	_UpdateBandInfo();
	HMENU	_GetDropDownMenu(int nCmdID, bool &bDestroy, bool &bSubMenu);


	// Data members;
	CMenuHandle 	m_menuFavorites;
	CMenuHandle 	m_menuFavoritesUser;
	CMenuHandle 	m_menuCSS;

};


//------------------------------
/// DonutToolBarウィンドウ作成
HWND	CDonutToolBar::Impl::Create(HWND hWndParent)
{
	HWND hWnd = __super::Create(hWndParent, rcDefault, NULL, ATL_SIMPLE_TOOLBAR_PANE_STYLE |  CCS_ADJUSTABLE | TBSTYLE_ALTDRAG, 0, ATL_IDW_TOOLBAR);
	ATLASSERT( ::IsWindow(hWnd) );
	
	SetExtendedStyle(TBSTYLE_EX_MIXEDBUTTONS | TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_HIDECLIPPEDBUTTONS);
	SetButtonStructSize();

	//_InitButton();	// ボタン登録

	return hWnd;
}

//-------------------------------
/// DropDownメニューの設定
void	CDonutToolBar::Impl::SetDropDownMenu(HMENU hMenu, HMENU hMenuUser, HMENU hMenuCSS)
{
	m_menuFavorites 	= hMenu;
	m_menuFavoritesUser = hMenuUser;
	m_menuCSS			= hMenuCSS;
}

//------------------------------
/// スキンを読み込む(必ず一度は呼ばれる必要がある)
void	CDonutToolBar::Impl::ReloadSkin()
{
	_UpdateImageLIst();
	_InitButton();
	_AdjustStyle();
}

//-----------------------------
/// カスタマイズダイアログを表示する
void	CDonutToolBar::Impl::Customize()
{
	CStdToolBarAdditionalCustomizeDlg dlg;
	dlg.Install(s_dwToolbarStyle, m_hWnd);	// フック

	__super::Customize();	// 表示

	s_dwToolbarStyle = dlg.m_dwStdToolBarStyle;

	// 表示するボタンのインデックスを更新
	CToolBarOption::s_vecShowBtn.clear();
	int nCount = GetButtonCount();
	for (int i = 0; i < nCount; ++i) {
		TBBUTTON btn;
		GetButton(i, &btn);
		if (btn.fsStyle & TBSTYLE_SEP)
			btn.iBitmap = -1;
		CToolBarOption::s_vecShowBtn.push_back(btn.iBitmap);
	}
	CToolBarOption::WriteProfileToolbarShowButton();	// 保存

	ReloadSkin();	// 表示更新
}


// Overrides

//---------------------------
/// チェブロン用にサブメニューを用意する
HMENU	CDonutToolBar::Impl::ChevronHandler_OnGetChevronMenu(int nCmdID, HMENU &hMenuDestroy)
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



// Message map

//----------------------------
/// 右ボタンup : ツールバーの表示設定メニューを表示
void	CDonutToolBar::Impl::OnRButtonUp(UINT nFlags, CPoint point)
{
	GetTopLevelParent().SendMessage(WM_SHOW_TOOLBARMENU);
}

//-----------------------------
/// ツールバーの何もないところをダブルクリックしたときにカスタムダイアログを表示する
void	CDonutToolBar::Impl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nIndex = HitTest(&point);
	if (nIndex < 0)
		Customize();
	else
		SetMsgHandled(FALSE);
}

//-----------------------------
/// ツールチップテキスト表示前
LRESULT CDonutToolBar::Impl::OnToolTipText(LPNMHDR pnmh)
{
	CString strText = CExMenuManager::GetToolTip((int)pnmh->idFrom);
	if ( strText.IsEmpty() ) {
		SetMsgHandled(FALSE);
		return 0;
	}

	LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO) pnmh;
	lstrcpyn( pDispInfo->szText, strText, _countof(pDispInfo->szText) );

	return 0;
}

//------------------------------
LRESULT CDonutToolBar::Impl::OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
{
	ATLASSERT( ((LPNMREBARCHEVRON)pnmh)->wID == GetDlgCtrlID() );

	if ( !PushChevron( pnmh, GetTopLevelParent() ) ) {
		bHandled = FALSE;
		return 1;
	}

	return 0;
}

//------------------------------
LRESULT CDonutToolBar::Impl::OnDropDown(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR) pnmh;
	int 		nCmdID = lpnmtb->iItem;

	auto funcTrackDropDownMenu = [this](int nCmdID, HMENU hMenu, HWND hWndOwner) {
		CRect	  rc;
		GetItemRect(CommandToIndex(nCmdID), &rc);
		this->ClientToScreen(&rc);
		UINT	  uMenuFlags = TPM_LEFTBUTTON /*| TPM_RIGHTBUTTON*/ | TPM_VERTICAL | TPM_LEFTALIGN | TPM_TOPALIGN | TPM_VERPOSANIMATION;
		TPMPARAMS TPMParams = { sizeof (TPMPARAMS) };
		TPMParams.rcExclude = rc;

		::TrackPopupMenuEx(hMenu, uMenuFlags, rc.left, rc.bottom, hWndOwner, &TPMParams);
	};

	bool		bDestroy;
	bool		bSubMenu;
	CMenuHandle menu = _GetDropDownMenu(nCmdID, bDestroy, bSubMenu);
	if (menu.m_hMenu) {
		if (bSubMenu)
			funcTrackDropDownMenu( nCmdID, menu.GetSubMenu(0), GetTopLevelParent() );
		else
			funcTrackDropDownMenu( nCmdID, menu, GetTopLevelParent() );
	}

	if (bDestroy)
		menu.DestroyMenu();

	return TBDDRET_DEFAULT;
}

//---------------------------
/// ツールバーのカスタマイズダイログにボタンを登録する
LRESULT CDonutToolBar::Impl::OnTbnGetButtonInfo(LPNMHDR pnmh)
{
	LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR) pnmh;
	int nMaxButton = (int)s_vecTBbtns.size();
	if (lpnmtb->iItem < nMaxButton) {
		lpnmtb->pszText[0] = 0;

		CString strText;
		if ( CToolTipManager::LoadToolTipText(s_vecTBbtns[lpnmtb->iItem].idCommand, strText) ) {
			// I don't know the buffer size of lpnmtb->pszText...
			strText = strText.Left(30);
			::lstrcpy(lpnmtb->pszText, strText);	//\\ これいいのか？
		}

		TBBUTTON&	btn = lpnmtb->tbButton;
		btn.iBitmap		= lpnmtb->iItem;
		btn.fsState		= TBSTATE_ENABLED;
		btn.idCommand	= s_vecTBbtns[lpnmtb->iItem].idCommand;
		btn.fsStyle		= (BYTE) (s_vecTBbtns[lpnmtb->iItem].fsStyle & ~BTNS_STD_LIST);
		btn.iString		= lpnmtb->iItem;
		btn.dwData		= 0;

		return TRUE;
	} else {
		return FALSE;
	}
}

//------------------------------
/// ボタンを移動し終わったとき？
LRESULT CDonutToolBar::Impl::OnTbnToolBarChange(LPNMHDR pnmh)
{
	_UpdateBandInfo();
	return 0;
}

//------------------------------
/// ボタンをリセットする
LRESULT CDonutToolBar::Impl::OnTbnReset(LPNMHDR pnmh)
{
	return 0;
}




//---------------------------
/// ツールバーボタン初期化/登録
void	CDonutToolBar::Impl::_InitButton()
{
	auto funcGetSep = [this]() -> TBBUTTON {
		TBBUTTON tbBtn;

		tbBtn.iBitmap	= s_kcxSeparator;
		tbBtn.idCommand = 0;
		tbBtn.fsState	= 0;
		tbBtn.fsStyle	= TBSTYLE_SEP;
		tbBtn.dwData	= 0;
		tbBtn.iString	= 0;
		return tbBtn;
	};

	auto funcGetTBBtn = [this](int nIndex) -> TBBUTTON {
		TBBUTTON tbBtn;
		tbBtn.iBitmap	= nIndex;
		tbBtn.idCommand = s_vecTBbtns[nIndex].idCommand;
		tbBtn.fsState	= TBSTATE_ENABLED;
		tbBtn.dwData	= 0;
		tbBtn.fsStyle	= (BYTE) (s_vecTBbtns[nIndex].fsStyle & ~BTNS_STD_LIST) | BTNS_AUTOSIZE;
		tbBtn.iString	= nIndex;
		return tbBtn;
	};

	// clean up previous toolbar buttons
	while ( DeleteButton(0) ) ;

	for (auto it = s_vecShowBtn.cbegin(); it != s_vecShowBtn.cend(); ++it) {
		if (*it == -1) {			// separator
			TBBUTTON tbBtn = funcGetSep();
			InsertButton(-1, &tbBtn);
		} else if ( 0 <= *it && *it < (int)s_vecTBbtns.size() ) {
			TBBUTTON tbBtn = funcGetTBBtn(*it);
			InsertButton(-1, &tbBtn);
		}
	}
	CSize size;
	GetButtonSize(size);
	SetWindowPos(NULL, 0, 0, 10, size.cy, SWP_NOMOVE | SWP_NOZORDER);


}

//------------------------
/// 画像を読み込み
void	CDonutToolBar::Impl::_UpdateImageLIst()
{
	enum TBICONSTYLE { TB_SMALLICON = 0, TB_LARGEICON = 1 };
	static LPCTSTR tbl[2][3] = {
		{ _T("Small.bmp")	, _T("SmallHot.bmp"), _T("SmallDisable.bmp"),	},
		{ _T("Big.bmp") 	, _T("BigHot.bmp")	, _T("BigDisable.bmp"),		},
	};
	COLORREF	clrMask = RGB(255, 0, 255);
 
	TBICONSTYLE mode;
	if (_check_flag(STD_TBSTYLE_SMALLICON, s_dwToolbarStyle))
		mode = TB_SMALLICON;
	else if (_check_flag(STD_TBSTYLE_LARGEICON, s_dwToolbarStyle))
		mode = TB_LARGEICON;
	else {
		ATLASSERT(FALSE);
		return ;
	}

	CSize	iconSize;
	int		nCount = (int)s_vecTBbtns.size();
	CString strDir = _GetSkinDir();
	CString strBasePath   = strDir + tbl[mode][0];
	CString strHotPath	  = strDir + tbl[mode][1];
	CString strDisPath	  = strDir + tbl[mode][2];

	bool	bUseDefault = false;
	CBitmap    bmp;
	bmp.Attach( AtlLoadBitmapImage( strBasePath.GetBuffer(0), LR_LOADFROMFILE) );
	if (bmp.IsNull()) {			//+++ 一応デフォルトを用意.
		bmp.LoadBitmap(IDB_MAINFRAME_TOOLBAR);
		bUseDefault = true;						// 内臓のツールバーイメージを使う
	}

	if (bmp.m_hBitmap) {	// 読み込んだビットマップからサイズを得る
		bmp.GetSize(iconSize);
	} else {
		switch (mode) {
		case TB_SMALLICON: iconSize = CSize(16, 16); break;
		case TB_LARGEICON: iconSize = CSize(21, 21); break;
		}
	}
	iconSize.cx = iconSize.cy;

	// Normal
	CImageList imgs;
	MTLVERIFY( imgs.Create(iconSize.cx, iconSize.cy, ILC_COLOR24 | ILC_MASK, nCount, 1) );
	if (bmp.m_hBitmap) {
		MTLVERIFY( imgs.Add(bmp, clrMask) != -1 );
	}

	// Hot
	CBitmap    bmpHot;
	bmpHot.Attach( AtlLoadBitmapImage( strHotPath.GetBuffer(0), LR_LOADFROMFILE ) );
	if (bmpHot.IsNull() && bUseDefault) 		//+++ 一応デフォルトを用意.
		bmpHot.LoadBitmap(IDB_MAINFRAME_TOOLBAR_HOT);
	
	CImageList imgsHot;
	MTLVERIFY( imgsHot.Create(iconSize.cx, iconSize.cy, ILC_COLOR24 | ILC_MASK, nCount, 1) );
	if (bmpHot.m_hBitmap) {
		MTLVERIFY( imgsHot.Add(bmpHot, clrMask) != -1 );
	}

	// Disable
	CBitmap    bmpDis;
	bmpDis.Attach( AtlLoadBitmapImage( strDisPath.GetBuffer(0), LR_LOADFROMFILE) );
	if (bmpDis.IsNull() && bUseDefault)  		//+++ 一応デフォルトを用意.
		bmpDis.LoadBitmap(IDB_MAINFRAME_TOOLBAR_DIS);
	
	CImageList imgsDis;
	MTLVERIFY( imgsDis.Create(iconSize.cx, iconSize.cy, ILC_COLOR24 | ILC_MASK, nCount, 1) );
	if (bmpDis.m_hBitmap) {
		MTLVERIFY( imgsDis.Add(bmpDis, clrMask) != -1 );
	}

	// イメージリスト設定
	SetImageList(imgs).Destroy();
	SetHotImageList(imgsHot).Destroy();
	if (bmpDis.IsNull() == false)
		SetDisabledImageList(imgsDis).Destroy();
}

//----------------------
/// スタイルを適応する(ラベルなど)
void	CDonutToolBar::Impl::_AdjustStyle()
{
	if ( _check_flag(STD_TBSTYLE_HIDELABEL, s_dwToolbarStyle) )
		_HideLabel();
	else if ( _check_flag(STD_TBSTYLE_SHOWLABEL, s_dwToolbarStyle) )
		_ShowLabel();
	else if ( _check_flag(STD_TBSTYLE_SHOWLABELRIGHT, s_dwToolbarStyle) )
		_ShowLabelRight();
	else
		ATLASSERT(FALSE);

	_UpdateBandInfo();
}

//----------------------------
/// テキストを表示しない
void	CDonutToolBar::Impl::_HideLabel()
{
	CLockRedraw lock(m_hWnd);

	ModifyStyle(0, TBSTYLE_LIST);
	
	int nCount = GetButtonCount();
	for (int i = 0; i < nCount; ++i) {
		// get button ID
		TBBUTTON	tbb;
		GetButton(i, &tbb);
		int 	CmdID	= tbb.idCommand;
		BYTE	fsStyle = tbb.fsStyle;

		// separator or not
		if (fsStyle & BTNS_SEP)
			continue;

		fsStyle   |= BTNS_AUTOSIZE;
		fsStyle   &= ~BTNS_SHOWTEXT;

		CVersional<TBBUTTONINFO> bi;
		bi.dwMask  = TBIF_STYLE;
		bi.fsStyle = fsStyle;
		SetButtonInfo(CmdID, &bi);
	}

	AutoSize();
}

//---------------------------------
/// テキストを表示
void	CDonutToolBar::Impl::_ShowLabel()
{
	CLockRedraw lock(m_hWnd);

	ModifyStyle(TBSTYLE_LIST, 0);

	_AddStrings();	// add it now!

	int nCount = GetButtonCount();
	for (int i = 0; i < nCount; ++i) {
		// get button ID and Style
		TBBUTTON	tbb;
		GetButton(i, &tbb);
		int 	nCmdID  = tbb.idCommand;
		BYTE	fsStyle = tbb.fsStyle;

		// separator or not
		if (fsStyle & BTNS_SEP)
			continue;

		// update info
		CVersional<TBBUTTONINFO> bi;
		bi.dwMask  = TBIF_STYLE;

		fsStyle   |= BTNS_AUTOSIZE; 		// fixed button size (IE5 style) or not
		fsStyle   |= BTNS_SHOWTEXT;
		bi.fsStyle = fsStyle;
		SetButtonInfo(nCmdID, &bi);
	}

	AutoSize();
}

//-----------------------------
/// テキストを右側に表示
void	CDonutToolBar::Impl::_ShowLabelRight()
{
	CLockRedraw	  lock(m_hWnd);

	ModifyStyle(0, TBSTYLE_LIST);
	_AddStrings();

	CSimpleArray<int> arrShowRightID;
	for (auto it = s_vecTBbtns.cbegin(); it != s_vecTBbtns.cend(); ++it) {
		if (it->fsStyle & BTNS_STD_LIST) {
			int nCmdID = it->idCommand;
			arrShowRightID.Add(nCmdID);
		}
	}

	int nCount = GetButtonCount();
	for (int i = 0; i < nCount; ++i) {
		// get button ID and Style
		TBBUTTON	tbb;
		GetButton(i, &tbb);

		int 	nCmdID	= tbb.idCommand;
		BYTE	fsStyle = tbb.fsStyle;

		// separator or not
		if (fsStyle & BTNS_SEP)
			continue;

		// update info
		CVersional<TBBUTTONINFO>	bi;
		bi.dwMask  = TBIF_STYLE;

		fsStyle   |= BTNS_AUTOSIZE;

		if (arrShowRightID.Find(nCmdID) != -1)	// 見つかったら表示
			fsStyle |= BTNS_SHOWTEXT;
		else
			fsStyle &= ~BTNS_SHOWTEXT;

		bi.fsStyle = fsStyle;
		SetButtonInfo(nCmdID, &bi);
	}

	AutoSize();
	_UpdateBandInfo();
}

//------------------------------
void	CDonutToolBar::Impl::_AddStrings()
{
	int nLen = 0;
	CSimpleArray<CString>	arrText;
	for (auto it = s_vecTBbtns.cbegin(); it != s_vecTBbtns.cend(); ++it) {
		CString 	strText;

		if ( _check_flag(BTNS_SEP, it->fsStyle) )
			continue;

		if ( CToolTipManager::LoadToolTipText(it->idCommand, strText) ) {
			nLen += strText.GetLength() + 1;
			arrText.Add(strText);
		}
	}
	if (nLen == 0) {
		ATLASSERT(FALSE);
		return ;
	}
	unique_ptr<TCHAR[]>	lpsz(new TCHAR[nLen + 1]);
	LPTSTR	lpszRunner = lpsz.get();
	int nCount = arrText.GetSize();
	for (int i = 0; i < nCount; ++i) {
		::lstrcpy(lpszRunner, arrText[i]);
		lpszRunner += arrText[i].GetLength() + 1;
	}
	AddStrings(lpsz.get());	// 登録
}


//-------------------------------------
void	CDonutToolBar::Impl::_UpdateBandInfo()
{
	CReBarCtrl	rebar = GetParent();
	UINT nCount = rebar.GetBandCount();
	if (nCount <= 0)  // probably not a rebar
		return ;

	int 	nBtnCount = GetButtonCount();
	CRect	rcRight;
	if ( !GetItemRect(nBtnCount - 1, &rcRight) )
		return ;

	CRect	rcLeft;
	if ( !GetItemRect(0, &rcLeft) )
		return ;

	int nIndex = rebar.IdToIndex(GetDlgCtrlID());
	if (nIndex == -1)
		return ;

	REBARBANDINFO rbbi = { sizeof (REBARBANDINFO) };
	rbbi.fMask		= RBBIM_IDEALSIZE | RBBIM_CHILDSIZE | RBBIM_STYLE;
	rebar.GetBandInfo(nIndex, &rbbi);
	rbbi.fStyle		|= RBBS_USECHEVRON;
	rbbi.cxMinChild = rcLeft.right;
	rbbi.cyMinChild = rcLeft.Height();
	rbbi.cxIdeal	= rcRight.right;
	rebar.SetBandInfo(nIndex, &rbbi);
}

//-----------------------------------
HMENU CDonutToolBar::Impl::_GetDropDownMenu(int nCmdID, bool &bDestroy, bool &bSubMenu)
{
	CMenuHandle 	menu;					// handle

	bDestroy = bSubMenu = true;

	switch (nCmdID) {
	case ID_FILE_NEW:
		menu.LoadMenu(IDR_FILE_NEW);
		break;

	case ID_VIEW_FONT_SIZE:
		menu.LoadMenu(IDR_VIEW_FONT_SIZE);
		break;

	case ID_FILE_NEW_CLIPBOARD2:
		menu.LoadMenu(IDR_FILE_NEW_CLIPBOARD);
		break;

	case ID_VIEW_BACK:
		menu.LoadMenu(IDR_VIEW_BACK);
		{
			HWND hChildFrm = ::GetParent( GetParent() );
			::SendMessage(hChildFrm, WM_MENU_GOBACK, (WPARAM) (HMENU) menu.m_hMenu, (LPARAM) 0);
		}
		break;

	case ID_VIEW_FORWARD:
		menu.LoadMenu(IDR_VIEW_FORWARD);
		{
			HWND hChildFrm = ::GetParent( GetParent() );
			::SendMessage(hChildFrm, WM_MENU_GOFORWARD, (WPARAM) (HMENU) menu.m_hMenu, (LPARAM) 0);
		}
		break;

	case ID_FAVORITES_DROPDOWN:
		menu	 = m_menuFavorites;
		bDestroy = bSubMenu = false;	// it's not mine.
		break;

	//case ID_MAIN_EX_NEWWINDOW:
	//	menu.LoadMenu(IDR_MENU_FAVTREE_BAR);
	//	break;

	case ID_MULTIMEDIA:
	case ID_DLCTL_CHG_MULTI:
		menu.LoadMenu(IDR_MULTIMEDIA);
		break;

	case ID_SECURITY:
	case ID_DLCTL_CHG_SECU:
		menu.LoadMenu(IDR_SECURITY);
		break;

	case ID_COOKIE:
	case ID_URLACTION_COOKIES_CHG:
		menu.LoadMenu(IDR_COOKIE);
		break;

	case ID_TOOLBAR:
		menu.LoadMenu(IDR_TOOLBAR);
		break;

	case ID_EXPLORERBAR:
		menu.LoadMenu(IDR_EXPLORERBAR);
		//menu.LoadMenu(IDR_MENU_FAVTREE_BAR);
		break;

	case ID_MOVE:
		menu.LoadMenu(IDR_MOVE);
		break;

	case ID_OPTION:
		menu.LoadMenu(IDR_OPTION);
		break;

	case ID_AUTO_REFRESH:
		menu.LoadMenu(IDR_AUTO_REFRESH);
		break;

	case ID_DOUBLE_CLOSE:
		menu.LoadMenu(IDR_DOUBLE_CLOSE);
		break;

	case ID_COOKIE_IE6:
		menu.LoadMenu(IDR_COOKIE_IE6);
		break;

	case ID_FAVORITES_GROUP_DROPDOWN:
		menu	 = m_menuFavoritesUser;
		bDestroy = bSubMenu = false;	// it's not mine.
		break;

	case ID_CSS_DROPDOWN:
		menu	 = m_menuCSS;
		bDestroy = bSubMenu = false;	// it's not mine.
		break;

	case ID_RECENT_DOCUMENT:
		menu.LoadMenu(IDR_RECENT_DOC);
		::SendMessage(GetTopLevelParent(), WM_MENU_RECENTDOCUMENT, (WPARAM) (HMENU) menu.GetSubMenu(0), (LPARAM) 0);
		break;

	default:
		ATLASSERT(FALSE);
	}

	return menu.m_hMenu;
}





//////////////////////////////////////////////////
// CDonutToolBar

// Constructor/Destructor
CDonutToolBar::CDonutToolBar() : pImpl(new Impl)
{	}

CDonutToolBar::~CDonutToolBar()
{
	delete pImpl;
}

//-----------------------------
HWND	CDonutToolBar::Create(HWND hWndParent)
{
	return pImpl->Create(hWndParent);
}

//-----------------------------
void	CDonutToolBar::SetFont(HFONT hFont)
{
	pImpl->SetFont(hFont);
}

//-----------------------------
void	CDonutToolBar::SetDropDownMenu(HMENU hMenu, HMENU hMenuUser, HMENU hMenuCSS)
{
	pImpl->SetDropDownMenu(hMenu, hMenuUser, hMenuCSS);
}

//-------------------------------
void	CDonutToolBar::ReloadSkin()
{
	pImpl->ReloadSkin();
}

//-----------------------------------
void	CDonutToolBar::Customize()
{
	pImpl->Customize();
}








