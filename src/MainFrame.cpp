/**
 *	@file	MainFrame.cpp
 *	@brief	メインフレームの実装
 *	@note
 *		+++ mainfrm.h を ヘッダとcppに分割。また、クラス名に併せてファイル名もMainFrameに変更.
 */
#pragma once
#include "stdafx.h"
#include "MainFrame.h"
#include <deque>
#include <sstream>
#include <boost\range\algorithm.hpp>
#include <boost\serialization\string.hpp>
#include <boost\serialization\vector.hpp>
#include <boost\serialization\utility.hpp>
#include <boost\archive\text_wiarchive.hpp>
#include <boost\archive\text_woarchive.hpp>
#include "SharedMemoryUtil.h"
//--------- MainFrame.hから ------------

#include "MtlProfile.h"		// MTL::CLogFont
#include "MtlUser.h"
#include "MtlUpdateCmdUI.h"
#include "MtlMisc.h"
#include "MainFrameFileDropTarget.h"

// 各種バンド
#include "DonutRebarCtrl.h"
#include "DonutCommandBar.h"
#include "DonutToolBar.h"
#include "DonutAddressBar.h"
#include "DonutSearchBar.h"
#include "DonutTabBar.h"
#include "DonutLinkBarCtrl.h"
#include "DonutStatusBarCtrl.h"
#include "FindBar.h"
//#include "PluginBar.h"
#include "DonutExplorerBar.h"

#include "ChildFrame.h"

#include "RecentClosedTabList.h"
#include "Download/DownloadManager.h"
#include "GdiplusUtil.h"
#include "AccelManager.h"
#include "ToolTipManager.h"
#include "ExMenu.h"
#include "ChildFrameCommandUIUpdater.h"
#include "GlobalConfig.h"
#include "FaviconManager.h"
#include "dialog\OpenURLDialog.h"
#include "ExStyle.h"
#include "FavoriteEditDialog.h"
#include "ProcessMonitorDialog.h"
#include "AutoLogin.h"

//#include "ExMenu.h"
#include "DonutOption.h"
#include "DonutOptions.h"
#include "ParseInternetShortcutFile.h"

#include "DonutTabList.h"
#include "DonutFavoriteGroupTreeView.h"
#include "FavoriteGroupEditDialog.h"
#include "PopupMenu.h"
#include "dialog\RenameFileDialog.h"
#include "DonutScriptHost.h"
#include "MultiThreadManager.h"
#include "PageMoveConfirmDialogAutoClose.h"


namespace {

struct ReBarIDAndStyle {
	UINT nID;
	UINT fStyle;
};

const ReBarIDAndStyle	 DefaultReBarStyle[] = {
	{ ATL_IDW_COMMAND_BAR	, RBBS_BREAK },
	{ ATL_IDW_TOOLBAR		, RBBS_USECHEVRON | RBBS_BREAK },
	{ IDC_ADDRESSBAR		, RBBS_BREAK	},
	{ IDC_MDITAB			, RBBS_BREAK	},
	{ IDC_LINKBAR			, RBBS_BREAK	},
	{ IDC_SEARCHBAR			, RBBS_BREAK	}
};


const LPTSTR	STDBAR_TEXT[]	= { _T("")/*NULL*/	   , _T("")/*NULL*/ 				,_T("アドレス") , NULL		 , _T("リンク") 			   , _T("検索") 					};	// memo. NULL だと一番左のボタンをシェブロンに含めることができない

};	// namespace 

///////////////////////////////////////////////////////////////
// CChildFrameClient

#pragma region CChildFrameClient
CChildFrameClient::CChildFrameClient() : m_hWndChildFrame(NULL)
{	}

HWND	CChildFrameClient::Create(HWND hWndMainFrame)
{
	CString strPath = _GetSkinDir() + _T("bg.bmp");
	m_bmpBackground = AtlLoadBitmapImage(strPath.GetBuffer(0), LR_LOADFROMFILE);

	return __super::Create(hWndMainFrame, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
}

void	CChildFrameClient::SetChildFrameWindow(HWND hWndChildFrame)
{
	enum { kTimeOut = 5000 };
	DWORD_PTR result = 0;
	CChildFrameCommandUIUpdater::ChangeCommandUIMap(hWndChildFrame);
	if (m_hWndChildFrame) {
		// タブに検索テキストを設定する
		CString text;
		if (m_funcGetSearchString(text)) {
			COPYDATASTRUCT cds = {};
			cds.dwData	= kSetSearchText;
			cds.lpData	= static_cast<LPVOID>(text.GetBuffer(0));
			cds.cbData	= (text.GetLength() + 1) * sizeof(TCHAR);
			::SendMessageTimeout(m_hWndChildFrame, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds, SMTO_ABORTIFHUNG, kTimeOut, &result);
		}
	}
	HWND hWndPrevChildFrame = m_hWndChildFrame;
	m_hWndChildFrame = hWndChildFrame;
#if 0
	if (m_hWndChildFrame && ::IsWindow(m_hWndChildFrame)) {
		::PostMessage(m_hWndChildFrame, WM_CHILDFRAMEACTIVATE, (WPARAM)hWndChildFrame, (LPARAM)m_hWndChildFrame);
	} else if (hWndChildFrame) {
		::PostMessage(hWndChildFrame, WM_CHILDFRAMEACTIVATE, (WPARAM)hWndChildFrame, (LPARAM)m_hWndChildFrame);	
	}
#endif
	if (hWndChildFrame) {
		::SendMessageTimeout(hWndChildFrame, WM_CHILDFRAMEACTIVATE, (WPARAM)hWndChildFrame, (LPARAM)hWndPrevChildFrame, SMTO_ABORTIFHUNG, kTimeOut, &result);		
	} else {
		InvalidateRect(NULL);
	}
}


void CChildFrameClient::OnSize(UINT nType, CSize size)
{
	DefWindowProc();
	if (m_hWndChildFrame) 
		::SetWindowPos(m_hWndChildFrame, NULL, 0, 0, size.cx, size.cy, SWP_ASYNCWINDOWPOS | SWP_NOZORDER | SWP_SHOWWINDOW);
}

BOOL CChildFrameClient::OnEraseBkgnd(CDCHandle dc)
{
	if (m_hWndChildFrame)
		return 1;

	//*+++ BG描画指定.
	if (m_bmpBackground) {		// bg画像を敷き詰めて表示
		CRect	rc;
		GetClientRect(&rc);

		CDC 	dcSrc;
		dcSrc.CreateCompatibleDC(dc);
		HBITMAP hOldbmpSrc	= dcSrc.SelectBitmap(m_bmpBackground);
		SIZE	size;
		m_bmpBackground.GetSize(size);
		DWORD	srcW = size.cx;
		DWORD	srcH = size.cy;
		DWORD	dstW = rc.Width();
		DWORD	dstH = rc.Height();
		for (unsigned y = 0; y < dstH; y += srcH) {
			for (unsigned x = 0; x < dstW; x += srcW) {
				::BitBlt(dc, x, y, srcW, srcH, dcSrc, 0, 0, SRCCOPY);
			}
		}
		dcSrc.SelectBitmap(hOldbmpSrc);
		return 1;
	} else if (CSkinOption::s_nMainFrameBgColor >= 0) {	// 色指定があれば、その色でべた塗り
		HBRUSH	hBrushBg = CreateSolidBrush(COLORREF(CSkinOption::s_nMainFrameBgColor));
		RECT	rect;
		GetClientRect(&rect);
		::FillRect( dc, &rect, hBrushBg );
		DeleteObject(hBrushBg);
		return 1;
	}

	// no need to erase it
	SetMsgHandled(FALSE);
	return 0;
}

#pragma endregion


//////////////////////////////////////////////////////////////////////////////
// CMainFrame::Impl

class CMainFrame::Impl :
	public CFrameWindowImpl<CMainFrame::Impl>,
	public CMessageFilter,
	public CIdleHandler,
	public CAppCommandHandler<CMainFrame::Impl>,
	public CUpdateCmdUI<CMainFrame::Impl>,
	public CDDEMessageHandler<CMainFrame::Impl>,
	public CMSMouseWheelMessageHandler<CMainFrame::Impl>,	// 必要？
	public CMainFrameFileDropTarget<CMainFrame::Impl>,
	public IunDonutAPI
{
public:
	DECLARE_FRAME_WND_CLASS(DONUT_WND_CLASS_NAME, IDR_MAINFRAME)

	Impl();
	~Impl();

	void	RestoreAllTab(LPCTSTR strFilePath = nullptr, bool bCloseAllTab = false);
	void	SaveAllTab();

	void	UserOpenFile(LPCTSTR url, DWORD openFlags = DonutGetStdOpenFlag(), 
						 DWORD DLCtrl = -1, 
						 DWORD ExStyle = -1, 
						 DWORD AutoRefresh = 0);
	void	UserOpenMultiFile(const std::vector<OpenMultiFileData>& vecOpenData, bool bLink = false, bool bUseDelayLoad = true);

	HWND	GetActiveChildFrameHWND() { return m_ChildFrameClient.GetActiveChildFrameWindow(); }
	CString GetActiveLocationURL();

	void	ExecuteUserScript(const CString& scriptFilePath) { m_donutScriptHost.ExecuteUserScript(scriptFilePath); }

	// Overrides
	BOOL AddSimpleReBarBandCtrl(HWND hWndReBar, HWND hWndBand, int nID, LPTSTR lpstrTitle, UINT fStyle, int cxWidth);
	void UpdateLayout(BOOL bResizeBars = TRUE);
	BOOL PreTranslateMessage(MSG* pMsg) override;
	BOOL OnIdle() override;
	bool OnDDEOpenFile(const CString& strFileName);

	// IunDonutAPI
	virtual HWND	GetChildFrame(int nIndex) override { return m_TabBar.GetTabHwnd(nIndex); }
	virtual LONG	GetTabCount() override { return m_TabBar.GetItemCount(); }
	virtual LONG	get_ActiveTabIndex() override { return m_TabBar.GetCurSel(); }
	virtual void	put_ActiveTabIndex(int nIndex) override { m_TabBar.SetCurSel(nIndex); }
	virtual void	OpenTab(const CString& url, bool bActive) override {  UserOpenFile(url, bActive ? D_OPENFILE_ACTIVATE : 0); }
	
	// Message map
	BEGIN_MSG_MAP_EX_decl( CMainFrame::Impl )

	// UpdateUI map
	BEGIN_UPDATE_COMMAND_UI_MAP_decl( CMainFrame::Impl )


//	void	OnCommandIDHandlerEX(UINT uNotifyCode, int nID, CWindow wndCtl)

	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	void	OnClose();
	void	OnDestroy();
	BOOL	OnQueryEndSession(UINT nSource, UINT uLogOff);
	void	OnEndSession(BOOL bEnding, UINT uLogOff);
	void	OnTimer(UINT_PTR nIDEvent);
	void	OnPaint(CDCHandle /*dc*/);
	void	OnActivate(UINT nState, BOOL bMinimized, CWindow wndOther);
	void	OnSysCommand(UINT nID, CPoint pt);
	void	OnInitMenuPopup(CMenuHandle menuPopup, UINT nIndex, BOOL bSysMenu);
	BOOL	OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct);
	LRESULT OnMyNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL	OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void	OnInitProcessFinished(bool bHome);
	void	OnBrowserTitleChange(HWND hWndChildFrame, LPCTSTR strTitle);
	void	OnBrowserLocationChange(LPCTSTR strURL, HICON hFavicon);

	LRESULT OnOpenWithExProp(_EXPROP_ARGS *pArgs);
	void	OnHilightSwitchChange(bool bOn) {
		m_GlobalConfigManageData.pGlobalConfig->bHilightSwitch = bOn;
	}

	void	OnSetProxyToChildFrame();

	void	OnCleanUpNewProcessSharedMemHandle(HANDLE hDel) { ::CloseHandle(hDel); }
	void	OnSetDLConfigToGlobalConfig();

	void	OnReleaseProcessMonitorPtr() { m_pProcessMonitor.reset(); }

	void	OnRemoveChildProcessId(DWORD dwProcessId);

	// for DownloadManager
	LRESULT OnGetUniqueNumberForDLItem();

	// Commands
	void	OnFileOpen(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnFileRecent(UINT uNotifyCode, int nID, CWindow wndCtl);

	void	OnEditOperation(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSearchBarCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT	OnFindKeyWord(LPCTSTR lpszKeyWord, BOOL bBack, long Flags = 0);
	LRESULT OnHilight(CString strKeyword);

	void	OnViewBar(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnAutoLoginEdit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSetFocusToBar(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnViewFullScreen(UINT uNotifyCode, int nID, CWindow wndCtl) { _FullScreen(!m_bFullScreen); }

	void	OnFavoriteAdd(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnFavoriteOrganize(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnFavoriteGroupCommand(UINT uNotifyCode, int nID, CWindow wndCtl);

	void	OnDoubleClose(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnPopupClose(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnTitleClose(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnGetOut(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnViewOption(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnViewOptionDonut(UINT uNotifyCode, int nID, CWindow wndCtl);

	void	OnTabClose(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnTabSwitch(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnOperateCommandToAllTab(UINT uNotifyCode, int nID, CWindow wndCtl);

	void	OnOpenDonutExeFolder(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnAuthorWebSite(UINT uNotifyCode, int nID, CWindow wndCtl);

	void	OnMenuRecentLast(UINT uNotifyCode, int nID, CWindow wndCtl) { 
		OnFileRecent(0, ID_RECENTDOCUMENT_FIRST, NULL);
	}
	void	OnSpecialRefreshSearchEngine(UINT uNotifyCode, int nID, CWindow wndCtl) {
		m_SearchBar.RefreshEngine();
	}

	LRESULT OnShowToolBarMenu();
	void	OnShowBandTextChange(bool bShow);

	void	OnTabCreate(HWND hWndChildFrame, DWORD dwOption);
	void	OnTabDestory(HWND hWndChildFrame);	
	void	OnAddRecentClosedTab(HWND hWndChildFrame);
	//void	OnOpenFindBarWithText(LPCTSTR strText);
	//int		OnGetTabIndex(HWND hWndChildFrame) { return m_MDITab.GetTabIndex(hWndChildFrame); }
	void	OnChildFrameConnecting(HWND hWndChildFrame)	{ m_TabBar.SetConnecting(hWndChildFrame); }
	void	OnChildFrameDownloading(HWND hWndChildFrame){ m_TabBar.SetDownloading(hWndChildFrame); }
	void	OnChildFrameComplete(HWND hWndChildFrame)	{ m_TabBar.SetComplete(hWndChildFrame); }
	void	OnChildFrameExStyleChange(HWND hWndChildFrame, DWORD ExStyle);
	void	OnMouseGesture(HWND hWndChildFrame, HANDLE hMapForClose);

private:
	void	_initRebar();
	HWND	_initCommandBar();
	HWND	_initToolBar();
	HWND	_initAddressBar();
	HWND	_initTabBar();
	HWND	_initLinkBar();
	void	_initBandPosition(HWND hWndCmdBar, HWND hWndToolBar, HWND hWndAddressBar, HWND hWndSearchBar, HWND hWndLinkBar, HWND hWndTabBar);
	void	_initStatusBar();
	void	_initSplitterWindow();
	void	_initChildFrameClient();
	void	_initExplorerBar();
	void	_initSkin();
	void	_initSysMenu();
	bool	_ConfirmCloseForFileDownloading();

	void	_SaveBandPosition();

	void	_FullScreen(bool bOn);
	void	_ShowBandsFullScreen(bool bOn);

	void	_SetHideTrayIcon();
	void	_DeleteTrayIcon();

	void	_NewDonutInstance(const CString& strURL);
	void	_NavigateChildFrame(HWND hWnd, LPCTSTR strURL, DWORD DLCtrl = -1, DWORD ExStyle = -1, DWORD AutoRefresh = 0);

	bool	_RButtonHook(MouseGestureData data);

	CDonutTabList	_CollectAllChildFrameData();
	unique_ptr<ChildFrameDataOnClose>	_GetChildFrameData(HWND hWndChildFrame);

	// for updateUI
	bool	_IsClipboardAvailable() { return ::IsClipboardFormatAvailable(MTL_CF_TEXT) == TRUE; }
	int		_GetRecentCount() { return m_RecentClosedTabList.GetRecentCount(); }
	bool	_IsRebarBandLocked();
	void	_UpdateProgressUI(CCmdUI *pCmdUI) {
		CProgressBarCtrl progressbar = pCmdUI->m_wndOther;
		progressbar.ShowWindow(SW_HIDE);
	}
	void	_UpdateStautsIcon(CCmdUI *pCmdUI) {
		pCmdUI->m_wndOther.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(pCmdUI->m_nID, -1), 0);
	}

	// Constants
	enum { kPosMRU	= 11, kAutoBackupTimerId = 1 };

	// Data members
	CDonutReBarCtrl 	m_ReBar;
	CDonutCommandBar	m_CommandBar;
	CDonutToolBar		m_ToolBar;
	CDonutAddressBar	m_AddressBar;
	CDonutSearchBar 	m_SearchBar;
	CDonutTabBar 		m_TabBar;
	CDonutLinkBarCtrl	m_LinkBar;
	CDonutStatusBarCtrl m_StatusBar;
	CFindBar			m_FindBar;

	CSplitterWindow		m_SplitterWindow;
	CDonutExplorerBar	m_ExplorerBar;			// エクスプローラー バー
	CChildFrameClient	m_ChildFrameClient;		// ChildFrameの親ウィンドウ

	CRecentClosedTabList	m_RecentClosedTabList;
	CDownloadManager		m_DownloadManager;

	CMainOption			m_MainOption;
	CChildFrameCommandUIUpdater	m_ChildFrameUIState;
	GlobalConfigManageData		m_GlobalConfigManageData;
	CFaviconManager		m_FaviconManager;

	std::deque<unique_ptr<NewChildFrameData> >	m_deqNewChildFrameData;

	HWND	m_hWndRestoreFocus;
	bool	m_bCommandFromChildFrame;
	bool	m_bFullScreen;

	unique_ptr<CProcessMonitorDialog>	m_pProcessMonitor;

	CDonutScriptHost	m_donutScriptHost;

	CSharedMemory	m_sharedMemKeyMessage;
	CPageMoveConfirmDialogAutoClose	m_pageMoveConfirmAutoClose;
	bool			m_bInitProcessFinished;
};

#include "MainFrame.inl"


//////////////////////////////////////////////////////////////////////////////
// CMainFrame

CMainFrame::CMainFrame() : pImpl(new Impl)
{
	g_pMainWnd = this;
}

CMainFrame::~CMainFrame()
{
	delete pImpl;
	g_pMainWnd = nullptr;
}

HWND	CMainFrame::CreateEx()
{
	return pImpl->CreateEx();
}

void	CMainFrame::StartupMainFrameStyle(int nCmdShow, bool bTray)
{
	if ((nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_SHOWMINNOACTIVE)
		&& (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MINBTN2TRAY))		//+++ .lnkとかで最小化で、起動されたとき、トレイに入る設定ならトレイへ.
	{
		bTray    = true;
		nCmdShow = SW_SHOWNORMAL;		// うまくいかない...が、残す...
	}
	CIniFileI  pr( g_szIniFileName, _T("Main") );
	int wndSML	= MtlGetProfileMainFrameState(pr, *pImpl, nCmdShow);				//+++ s,m,l,fullを判別しておく(暫定版)
	pr.Close();

	//m_bOldMaximized 	= 0;
	//if (wndSML == 2)
	//	m_bOldMaximized = 1;

	if (bTray) {
		//OnGetOut(0,0,0);
	} else if (CMainOption::s_dwMainExtendedStyle & MAIN_EX_FULLSCREEN) {
		//_FullScreen(TRUE);
	}

	// キーボードショートカットの初期化
	CAcceleratorOption::CreateOriginAccelerator(pImpl->m_hWnd, pImpl->m_hAccel);	
}

void	CMainFrame::RestoreAllTab(LPCTSTR strFilePath /*= nullptr*/, bool bCloseAllTab /*= false*/)
{
	pImpl->RestoreAllTab(strFilePath, bCloseAllTab);
}

void	CMainFrame::UserOpenFile(LPCTSTR url, DWORD openFlags, 
								 DWORD DLCtrl /*= -1*/, 
								 DWORD ExStyle /*= -1*/, 
								 DWORD AutoRefresh /*= 0*/)
{
	pImpl->UserOpenFile(url, openFlags, DLCtrl, ExStyle, AutoRefresh);
}

void	CMainFrame::UserOpenMultiFile(const std::vector<OpenMultiFileData>& vecOpenData)
{
	pImpl->UserOpenMultiFile(vecOpenData, false, true);
}


HWND	CMainFrame::GetHWND()
{
	return pImpl->m_hWnd;
}

HWND	CMainFrame::GetActiveChildFrameHWND()
{
	return pImpl->GetActiveChildFrameHWND();
}


CString CMainFrame::GetActiveLocationURL()
{
	return pImpl->GetActiveLocationURL();
}

void	CMainFrame::ExecuteUserScript(const CString& scriptFilePath)
{
	pImpl->ExecuteUserScript(scriptFilePath);
}


