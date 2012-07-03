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
#include <codecvt>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\xml_parser.hpp>
#include <boost\serialization\string.hpp>
#include <boost\serialization\vector.hpp>
#include <boost\serialization\utility.hpp>
#include "SharedMemoryUtil.h"
//--------- MainFrame.hから ------------

#include "MtlProfile.h"		// MTL::CLogFont
#include "MtlUser.h"
#include "MtlUpdateCmdUI.h"
#include "MtlMisc.h"
#include "MainFrameFileDropTarget.h"
//#include "DonutDefine.h"
//#include "DonutFavoritesMenu.h"
//#include "DonutOptions.h"
//#include "DonutSecurityZone.h"
//#include "DonutSimpleEventManager.h"

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

//#include "DonutP.h"
////#include "DonutP_i.c"
////+++ #include "FileCriticalSection.h"				//+++ 別の方法をとるので、不要になった.
//#include "MDIChildUserMessenger.h"
#include "ChildFrame.h"								//+++ "ChildFrm.h"
//#include "MenuControl.h"
//#include "AccelManager.h"
//#include "dialog/DebugWindow.h"
//#include "MenuEncode.h"
//#include "BingTranslatorMenu.h"
//#include "StyleSheetOption.h"
//#include "MenuDropTargetWindow.h"

//#include "option/AddressBarPropertyPage.h"
//#include "option/SearchPropertyPage.h"
//#include "ChildFrameCommandUIUpdater.h"
//#include "DonutViewOption.h"

//#include "ExMenu.h"
#include "DonutOption.h"
#include "DonutOptions.h"
//#include "option\DLControlOption.h"
//#include "option\ToolBarDialog.h"
//#include "option\AddressBarPropertyPage.h"
//#include "option\MDITabDialog.h"
//#include "option\FileNewOption.h"
//--------------------------------------

//#include <boost\scope_exit.hpp>
//#include "DialogKiller.h"
//#include "dialog/DebugWindow.h"
//#include "PropertySheet.h"
//#include "DonutOption.h"
//#include "MenuEncode.h"
//#include "StyleSheetOption.h"
//#include "ExStyle.h"
//#include "MenuDropTargetWindow.h"
//#include "ParseInternetShortcutFile.h"
//
//#include "option/AddressBarPropertyPage.h"	//+++ AddressBar.hより分離
//#include "option/SearchPropertyPage.h"		//+++ SearchBar.hより分離
//#include "option/LinkBarPropertyPage.h" 	//+++
//#include "option/UrlSecurityOption.h"		//+++
//#include "option/RightClickMenuDialog.h"
//
//#include "DialogHook.h"
//#include "dialog/OpenURLDialog.h"
//#include "api.h"
//#include "PluginEventImpl.h"
//#include "Thumbnail.h"
//#include "FaviconManager.h"
//#include "GdiplusUtil.h"


#ifdef _DEBUG
	const bool _Donut_MainFrame_traceOn = false;
	#define dmfTRACE	if (_Donut_MainFrame_traceOn)  ATLTRACE
#else
	#define dmfTRACE
#endif


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
#if 1	/* 前のウィンドウの画面を更新しておく */
	if (   m_hWndChildFrame && hWndChildFrame
		&& Misc::IsGpuRendering() 
		&& CDLControlOption::s_nGPURenderStyle != CDLControlOption::GPURENDER_NONE)
	{
		static CBitmap	bmpPage;
		if (bmpPage.IsNull() == false)
			bmpPage.DeleteObject();
//		CClientDC	dc(m_hWndChildFrame);
		//CClientDC	dc2(hWndChildFrame);
		RECT rect;
		::GetClientRect(m_hWndChildFrame, &rect);
		CDC Desktopdc = ::GetDC(NULL);
		bmpPage.CreateCompatibleBitmap(Desktopdc, rect.right, rect.bottom);
		CDC	memDC = CreateCompatibleDC(Desktopdc);
		HBITMAP bmpPrev = memDC.SelectBitmap(bmpPage);

		/* ページのビットマップを取得 */
		memDC.FillSolidRect(&rect, RGB(255, 255, 255));
		::SendMessage(hWndChildFrame, WM_DRAWCHILDFRAMEPAGE, (WPARAM)memDC.m_hDC, 0);

//		dc.BitBlt(0, 0, rect.right, rect.bottom, memDC, rect.right, rect.bottom, SRCCOPY);
		//dc2.BitBlt(0, 0, rect.right, rect.bottom, memDC, rect.right, rect.bottom, SRCCOPY);	//
		memDC.SelectBitmap(bmpPrev);

		::SendMessage(hWndChildFrame, WM_SETPAGEBITMAP, (WPARAM)&bmpPage.m_hBitmap, 0);
		::SendMessage(m_hWndChildFrame, WM_SETPAGEBITMAP, (WPARAM)&bmpPage.m_hBitmap, 0);

		::InvalidateRect(m_hWndChildFrame, NULL, FALSE);
		::UpdateWindow(m_hWndChildFrame);
	}
#endif
	//SetRedraw(FALSE);
	CChildFrameCommandUIUpdater::ChangeCommandUIMap(hWndChildFrame);
	if (m_hWndChildFrame) {
		::PostMessage(m_hWndChildFrame, WM_CHILDFRAMEACTIVATE, (WPARAM)hWndChildFrame, (LPARAM)m_hWndChildFrame);
		//::ShowWindowAsync(m_hWndChildFrame, FALSE);
	}
	
	if (hWndChildFrame) {
		::PostMessage(hWndChildFrame, WM_CHILDFRAMEACTIVATE, (WPARAM)hWndChildFrame, (LPARAM)m_hWndChildFrame);		
		RECT rcClient;
		GetClientRect(&rcClient);
		//::SetWindowPos(hWndChildFrame, NULL, 0, 0, rcClient.right, rcClient.bottom, /*SWP_ASYNCWINDOWPOS | */SWP_NOZORDER | /*SWP_SHOWWINDOW | */SWP_NOREDRAW);
		//::ShowWindowAsync(hWndChildFrame, TRUE);
		//::RedrawWindow(hWndChildFrame, NULL, NULL, RDW_FRAME | RDW_INVALIDATE/* | RDW_UPDATENOW*/ | RDW_ALLCHILDREN);
		//::BringWindowToTop(hWndChildFrame);
	} else {
		InvalidateRect(NULL);
	}

	m_hWndChildFrame = hWndChildFrame;
		
	//SetRedraw(TRUE);
	//RedrawWindow(NULL, NULL, RDW_FRAME | RDW_ERASE | RDW_ERASENOW | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
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
	public CMainFrameFileDropTarget<CMainFrame::Impl>
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
	void	UserOpenMultiFile(const std::vector<OpenMultiFileData>& vecOpenData, bool bLink = false);

	HWND	GetActiveChildFrameHWND() { return m_ChildFrameClient.GetActiveChildFrameWindow(); }

	// Overrides
	BOOL AddSimpleReBarBandCtrl(HWND hWndReBar, HWND hWndBand, int nID, LPTSTR lpstrTitle, UINT fStyle, int cxWidth);
	void UpdateLayout(BOOL bResizeBars = TRUE);
	BOOL PreTranslateMessage(MSG* pMsg) override;
	BOOL OnIdle() override;
	bool OnDDEOpenFile(const CString& strFileName);

	// Message map
	BEGIN_MSG_MAP_EX( Impl )

		CHAIN_MSG_MAP_MEMBER( m_MainOption			)
		CHAIN_MSG_MAP_MEMBER( m_ChildFrameUIState	)
		CHAIN_MSG_MAP_MEMBER( m_DownloadManager		)
		CHAIN_MSG_MAP_MEMBER( m_FaviconManager		)

		MSG_WM_CREATE	( OnCreate	)
		MSG_WM_CLOSE	( OnClose	)
		MSG_WM_DESTROY	( OnDestroy )
		MSG_WM_QUERYENDSESSION( OnQueryEndSession	)
		MSG_WM_ENDSESSION( OnEndSession	)
		MSG_WM_PAINT	( OnPaint	)
		MSG_WM_ACTIVATE ( OnActivate )
		MSG_WM_SYSCOMMAND( OnSysCommand	)
		MSG_WM_INITMENUPOPUP( OnInitMenuPopup	)
		MSG_WM_COPYDATA	( OnCopyData	)
		MESSAGE_HANDLER_EX( MYWM_NOTIFYICON , OnMyNotifyIcon  )
		MSG_WM_MOUSEWHEEL( OnMouseWheel	)

		USER_MSG_WM_INITPROCESSFINISHED		( OnInitProcessFinished )
		USER_MSG_WM_UIUPDATE()
		USER_MSG_WM_BROWSERTITLECHANGE		( OnBrowserTitleChange		)
		USER_MSG_WM_BROWSERLOCATIONCHANGE	( OnBrowserLocationChange	)
		USER_MSG_WM_OPEN_WITHEXPROP			( OnOpenWithExProp	)	// SearchBarから
		USER_MSG_WM_HILIGHTSWITCHCHANGE		( OnHilightSwitchChange	)
		// ChildFrame へ
		USER_MSG_WM_UPDATEURLSECURITYLIST	( OnUpdateUrlSecurityList	)
		USER_MSG_WM_UPDATECUSTOMCONTEXTMENU	( OnUpdateCustomContextMenu	)
		USER_MSG_WM_SETPROXYTOCHLDFRAME		( OnSetProxyToChildFrame	)

		USER_MSG_WM_CLEANUPNEWPROCESSSHAREDMEMHANDLE( OnCleanUpNewProcessSharedMemHandle )
		USER_MSG_WM_SETDLCONFIGTOGLOBALCONFIG( OnSetDLConfigToGlobalConfig	)

		m_bCommandFromChildFrame = false;
		if (uMsg == WM_COMMAND_FROM_CHILDFRAME) {		// Loop防止
			uMsg = WM_COMMAND;
			m_bCommandFromChildFrame	= true;
		}
		
		// ファイル
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW				, OnFileOpen 		)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_BLANK		, OnFileOpen		)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_HOME 		, OnFileOpen 		)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_COPY 		, OnFileOpen 		)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_CLIPBOARD	, OnFileOpen		)
		COMMAND_ID_HANDLER_EX( ID_FILE_OPEN_TABLIST		, OnFileOpen		)
		COMMAND_ID_HANDLER_EX( ID_FILE_OPEN 			, OnFileOpen		)
		COMMAND_RANGE_HANDLER_EX( ID_RECENTDOCUMENT_FIRST, ID_RECENTDOCUMENT_LAST, OnFileRecent )
		COMMAND_RANGE_HANDLER_EX( ID_FILE_MRU_FIRST		 , ID_FILE_MRU_LAST		 , OnFileRecent )
		COMMAND_ID_HANDLER_EX( ID_VIEW_HOME				, OnFileOpen		)

		// 編集
		COMMAND_ID_HANDLER_EX( ID_EDIT_CUT 				, OnEditOperation	)
		COMMAND_ID_HANDLER_EX( ID_EDIT_COPY 			, OnEditOperation	)
		COMMAND_ID_HANDLER_EX( ID_EDIT_PASTE 			, OnEditOperation	)
		COMMAND_ID_HANDLER_EX( ID_EDIT_SELECT_ALL		, OnEditOperation	)
		COMMAND_ID_HANDLER_EX( ID_SEARCHBAR_SEL_DOWN	, OnSearchBarCommand )
		COMMAND_ID_HANDLER_EX( ID_SEARCHBAR_SEL_UP		, OnSearchBarCommand )
		COMMAND_ID_HANDLER_EX( ID_SEARCHBAR_HILIGHT		, OnSearchBarCommand )
		MSG_WM_USER_FIND_KEYWORD( OnFindKeyWord    )	// from SearchBar
		MSG_WM_USER_HILIGHT 	( OnHilight 	   )

		// お気に入り
		COMMAND_ID_HANDLER_EX( ID_FAVORITE_ADD			, OnFavoriteAdd 		)
		COMMAND_ID_HANDLER_EX( ID_FAVORITE_ORGANIZE 	, OnFavoriteOrganize	)

		// 表示
		COMMAND_ID_HANDLER_EX( ID_VIEW_SEARCHBAR		, OnViewBar			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_COMMANDBAR		, OnViewBar			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_TOOLBAR			, OnViewBar			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_ADDRESSBAR		, OnViewBar			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_LINKBAR			, OnViewBar			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_TABBAR			, OnViewBar			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_GOBUTTON			, OnViewBar			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_TABBAR_MULTI		, OnViewBar			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_TOOLBAR_CUST		, OnViewBar			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_TOOLBAR_LOCK		, OnViewBar			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_STATUS_BAR		, OnViewBar			)
		COMMAND_ID_HANDLER_EX( ID_SETFOCUS_ADDRESSBAR		, OnSetFocusToBar	)
		COMMAND_ID_HANDLER_EX( ID_SETFOCUS_SEARCHBAR		, OnSetFocusToBar	)
		COMMAND_ID_HANDLER_EX( ID_SETFOCUS_SEARCHBAR_ENGINE	, OnSetFocusToBar	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_ADDBAR_DROPDOWN		, OnSetFocusToBar	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_FULLSCREEN		, OnViewFullScreen		)

		// ツール
		COMMAND_ID_HANDLER_EX( ID_GET_OUT				, OnGetOut			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_OPTION			, OnViewOption		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_OPTION_DONUT 	, OnViewOptionDonut	)

		// ウィンドウ
		COMMAND_ID_HANDLER_EX( ID_WINDOW_CLOSE_ALL		, OnTabClose		)
		COMMAND_ID_HANDLER_EX( ID_WINDOW_CLOSE_EXCEPT	, OnTabClose		)
		COMMAND_ID_HANDLER_EX( ID_LEFT_CLOSE			, OnTabClose		)
		COMMAND_ID_HANDLER_EX( ID_RIGHT_CLOSE			, OnTabClose		)
		COMMAND_ID_HANDLER_EX( ID_TAB_LEFT				, OnTabSwitch 		)
		COMMAND_ID_HANDLER_EX( ID_TAB_RIGHT 			, OnTabSwitch		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_STOP_ALL			, OnOperateCommandToAllTab )
		COMMAND_ID_HANDLER_EX( ID_VIEW_REFRESH_ALL		, OnOperateCommandToAllTab )
		COMMAND_ID_HANDLER_EX( ID_WINDOW_REFRESH_EXCEPT	, OnOperateCommandToAllTab )

		// ヘルプ
		COMMAND_ID_HANDLER_EX( ID_OPEN_EXE_DIR	, OnOpenDonutExeFolder	)
		COMMAND_ID_HANDLER_EX( ID_JUMP_WEBSITE	, OnAuthorWebSite		)

		// Special Command
		COMMAND_ID_HANDLER_EX( ID_RECENT_DOCUMENT	, OnMenuRecentLast		)

		USER_MSG_WM_SHOW_TOOLBARMENU( OnShowToolBarMenu )
		USER_MSG_WM_SHOW_BAND_TEXT_CHANGE( OnShowBandTextChange	)

		// CChildFrameから
		USER_MSG_WM_TABCREATE				( OnTabCreate	)	
		USER_MSG_WM_TABDESTROY				( OnTabDestory	)
		USER_MSG_WM_ADDRECENTCLOSEDTAB		( OnAddRecentClosedTab )
		//USER_MSG_WM_OPENFINDBARWITHTEXT		( OnOpenFindBarWithText )
		//USER_MSG_WM_GETTABINDEX				( OnGetTabIndex	)
		USER_MSG_WM_CHILDFRAMECONNECTING	( OnChildFrameConnecting	)
		USER_MSG_WM_CHILDFRAMEDOWNLOADING	( OnChildFrameDownloading	)
		USER_MSG_WM_CHILDFRAMECOMPLETE		( OnChildFrameComplete		)
		USER_MSG_WM_MOUSEGESTURE			( OnMouseGesture			)

		CHAIN_MSG_MAP( CFrameWindowImpl<CMainFrame::Impl> )
		CHAIN_MSG_MAP( CAppCommandHandler<CMainFrame::Impl> )
		CHAIN_MSG_MAP( CUpdateCmdUI<CMainFrame::Impl> )
		CHAIN_MSG_MAP( CDDEMessageHandler<CMainFrame::Impl> )
		CHAIN_MSG_MAP( CMSMouseWheelMessageHandler<CMainFrame::Impl> )
		if (uMsg == WM_COMMAND && m_bCommandFromChildFrame == false) {
			HWND hWndChild = m_ChildFrameUIState.GetActiveChildFrameWindowHandle();
			if (hWndChild)
				::PostMessage(hWndChild, uMsg, wParam, lParam);
		}
		REFLECT_CHEVRONPUSHED_NOTIFICATION()
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	
	// UpdateUI map
#pragma region UI map
	BEGIN_UPDATE_COMMAND_UI_MAP( Impl )
		CHAIN_UPDATE_COMMAND_UI_MEMBER( m_MainOption		)
		CHAIN_UPDATE_COMMAND_UI_MEMBER( m_ChildFrameUIState )

		bool bActiveChild = (m_ChildFrameClient.GetActiveChildFrameWindow() != NULL);

		// File menu
		UPDATE_COMMAND_UI_SETDEFAULT_FLAG		( ID_FILE_NEW_HOME , FILENEW_HOME , CFileNewOption::s_dwFlags )
		UPDATE_COMMAND_UI_SETDEFAULT_FLAG_PASS	( ID_FILE_NEW_COPY , FILENEW_COPY , CFileNewOption::s_dwFlags )
		UPDATE_COMMAND_UI_SETDEFAULT_FLAG		( ID_FILE_NEW_BLANK, FILENEW_BLANK, CFileNewOption::s_dwFlags )
		UPDATE_COMMAND_UI_ENABLE_IF 			( ID_FILE_NEW_COPY		, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 			( ID_FILE_NEW_CLIPBOARD , _IsClipboardAvailable() )
		UPDATE_COMMAND_UI_SETDEFAULT			( ID_FILE_NEW_CLIPBOARD2 )
		UPDATE_COMMAND_UI_ENABLE_IF 			( ID_FILE_NEW_CLIPBOARD2, _IsClipboardAvailable() )
		UPDATE_COMMAND_UI_ENABLE_IF 			( ID_FILE_SAVE_AS		, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 			( ID_FILE_PAGE_SETUP	, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 			( ID_FILE_PRINT 		, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 			( ID_FILE_PROPERTIES	, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 			( ID_FILE_CLOSE 		, bActiveChild )
		UPDATE_COMMAND_UI_POPUP_ENABLE_IF 		( ID_RECENT_DOCUMENT, _GetRecentCount() )
		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP	( ID_RECENT_DOCUMENT, _GetRecentCount(), _GetRecentCount() )

		// Edit menu
		// アドレスバーかサーチバーにフォーカスがある or OLEコマンドが使用可能かどうか
		CEdit editAddress = m_AddressBar.GetEditCtrl();
		CEdit editSearch  = m_SearchBar.GetEditCtrl();
		bool  bFocus	  = (::GetFocus() == editAddress || ::GetFocus() == editSearch);
		bool  bCut		  = false, bCopy = false, bPaste = false, bSelectAll = false;
		if (bActiveChild) {
		//+++ 実はQUeryStatusWBがバグって、失敗しててもゴミを返していた...で、たまたま表示されることが多かったのかも.
		//+++ が、そもそも必ず失敗するのが解せない... が、不便なので、暫定対処
			bCut = true, bCopy = true, bPaste = true, bSelectAll = true;
		}
		if (bFocus)
			bCut = true, bCopy = true, bPaste = true, bSelectAll = true;
		UPDATE_COMMAND_UI_ENABLE_IF ( ID_EDIT_CUT		, bCut			)
		UPDATE_COMMAND_UI_ENABLE_IF ( ID_EDIT_COPY		, bCopy			)
		UPDATE_COMMAND_UI_ENABLE_IF ( ID_EDIT_PASTE 	, bPaste		)
		UPDATE_COMMAND_UI_ENABLE_IF ( ID_EDIT_SELECT_ALL, bSelectAll	)
		// ここまで

		UPDATE_COMMAND_UI_ENABLE_IF ( ID_EDIT_FIND				, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF ( ID_EDIT_OPEN_SELECTED_REF , bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF ( ID_EDIT_OPEN_SELECTED_TEXT, bActiveChild )
		//UPDATE_COMMAND_UI_ENABLE_IF( ID_VIEW_SETFOCUS, bActiveChild )

		// View menu
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_SEARCHBAR,	MtlIsBandVisible( m_hWndToolBar, IDC_SEARCHBAR		) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_COMMANDBAR,	MtlIsBandVisible( m_hWndToolBar, ATL_IDW_COMMAND_BAR) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_TOOLBAR,		MtlIsBandVisible( m_hWndToolBar, ATL_IDW_TOOLBAR	) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_ADDRESSBAR,	MtlIsBandVisible( m_hWndToolBar, IDC_ADDRESSBAR 	) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_LINKBAR,		MtlIsBandVisible( m_hWndToolBar, IDC_LINKBAR		) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_GOBUTTON, 	CAddressBarOption::s_bGoBtnVisible )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_TABBAR,		MtlIsBandVisible( m_hWndToolBar, IDC_MDITAB 		) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_TABBAR_MULTI, CTabBarOption::s_bMultiLine )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_TOOLBAR_LOCK,  _IsRebarBandLocked()					)
//		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_CLIPBOARDBAR,  m_ExplorerBar.IsClipboardBarVisible()	)
//		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_PANELBAR, 	 m_ExplorerBar.IsPanelBarVisible()		)
//		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_PLUGINBAR,	 m_ExplorerBar.IsPluginBarVisible() 	)
		UPDATE_COMMAND_UI_SETCHECK_FLAG ( ID_EXPLORERBAR_AUTOSHOW, MAIN_EXPLORER_AUTOSHOW, CMainOption::s_dwExplorerBarStyle )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_PRIVACYREPORT, 	bActiveChild							)
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_STATUS_BAR,	::IsWindowVisible(m_hWndStatusBar)		)

		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP	( ID_VIEW_BACK, bActiveChild, bActiveChild		)		// with popup
		UPDATE_COMMAND_UI_ENABLE_IF 		( ID_VIEW_FORWARD	, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 		( ID_VIEW_REFRESH	, bActiveChild			)

		UPDATE_COMMAND_UI_POPUP_ENABLE_IF	( ID_VIEW_BACK1 	, bActiveChild			)
		UPDATE_COMMAND_UI_POPUP_ENABLE_IF	( ID_VIEW_FORWARD1	, bActiveChild			)

		UPDATE_COMMAND_UI_ENABLE_IF 	(  ID_VIEW_STOP 		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	(  ID_VIEW_STOP_ALL 	, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	(  ID_VIEW_REFRESH		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	(  ID_VIEW_REFRESH_ALL	, bActiveChild			)
		UPDATE_COMMAND_UI_POPUP_ENABLE_IF( ID_AUTOREFRESH_NONE	, bActiveChild			)
		UPDATE_COMMAND_UI_POPUP_ENABLE_IF( ID_VIEW_FONT_LARGEST , bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	(  ID_VIEW_FONT_SIZE	, bActiveChild			)

		//UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_VIEW_ENCODECONTAINER, bActiveChild, bActiveChild ) // minit

		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_VIEW_SOURCE		, bActiveChild			)
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_FULLSCREEN	, m_bFullScreen			)

		// Favorite menu
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_FAVORITE_ADD		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_FAVORITE_GROUP_SAVE, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_FAVORITE_GROUP_ADD , bActiveChild			)

		// Tool menu
		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_DLCTL_DLIMAGES, bActiveChild, bActiveChild ) 	// with poup
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_CHG_MULTI	, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_ON_OFF_MULTI , bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_BGSOUNDS 	, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_VIDEOS		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_DLCTL_RUNACTIVEXCTLS, bActiveChild, bActiveChild )// with popup
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_CHG_SECU 	, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_ON_OFF_SECU	, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_DLACTIVEXCTLS, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_SCRIPTS		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_JAVA 		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DOCHOSTUI_OPENNEWWIN, bActiveChild 		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_MESSAGE_FILTER 	, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_MOUSE_GESTURE		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_BLOCK_MAILTO		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_EDIT_IGNORE		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_TITLE_COPY 		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_COPY_TITLEANDURL	, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_URL_COPY			, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_EDIT_CLOSE_TITLE	, bActiveChild			)

		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_STYLESHEET_USE_USERS, CStyleSheetOption::GetUseUserSheet() )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_STYLESHEET_SET_USERS, CStyleSheetOption::s_bSetUserSheet	)

		// Help
		//UPDATE_COMMAND_UI_ENABLE_IF 	( ID_APP_HELP			, _IsExistHTMLHelp()	)

		// Misc
		// Window menu
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_WINDOW_CLOSE_ALL		, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_WINDOW_CLOSE_EXCEPT	, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF		( ID_LEFT_CLOSE				, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF		( ID_RIGHT_CLOSE			, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_WINDOW_REFRESH_EXCEPT	, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_TAB_LEFT				, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_TAB_RIGHT				, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_WINDOW_RESTORE, bActiveChild, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ATL_IDS_SCPREVWINDOW		, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ATL_IDS_SCNEXTWINDOW		, bActiveChild		)

		// Misc
		// UPDATE_COMMAND_UI_SETTEXT( ID_DEFAULT_PANE, _T("レディ" ))
		UPDATE_COMMAND_UI				( IDC_PROGRESS		, _UpdateProgressUI		)
		UPDATE_COMMAND_UI				( ID_SECURE_PANE	, _UpdateStautsIcon		)
		UPDATE_COMMAND_UI				( ID_PRIVACY_PANE	, _UpdateStautsIcon		)

		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_POPUP_CLOSE	, CIgnoredURLsOption::s_bValid )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_TITLE_CLOSE	, CCloseTitlesOption::s_bValid )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_DOUBLE_CLOSE	, CIgnoredURLsOption::s_bValid && CCloseTitlesOption::s_bValid )

		UPDATE_COMMAND_UI_POPUP_ENABLE_IF( ID_STYLESHEET_BASE	, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_CSS_DROPDOWN		, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_VIEW_UP			, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_EDIT_FIND_MAX		, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_VIEW_SOURCE_SELECTED,bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_FILE_PRINT_PREVIEW , bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_SEARCH_PAGE		, bActiveChild )	//検索バーのページ内検索ボタン
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_SEARCHBAR_SEL_UP	, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_SEARCHBAR_SEL_DOWN , bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_SEARCHBAR_HILIGHT	, bActiveChild )
	END_UPDATE_COMMAND_UI_MAP()
#pragma endregion

//	void	OnCommandIDHandlerEX(UINT uNotifyCode, int nID, CWindow wndCtl)

	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	void	OnClose();
	void	OnDestroy();
	BOOL	OnQueryEndSession(UINT nSource, UINT uLogOff);
	void	OnEndSession(BOOL bEnding, UINT uLogOff);
	void	OnPaint(CDCHandle /*dc*/);
	void	OnActivate(UINT nState, BOOL bMinimized, CWindow wndOther);
	void	OnSysCommand(UINT nID, CPoint pt);
	void	OnInitMenuPopup(CMenuHandle menuPopup, UINT nIndex, BOOL bSysMenu);
	BOOL	OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct);
	LRESULT OnMyNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL	OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void	OnInitProcessFinished();
	void	OnBrowserTitleChange(HWND hWndChildFrame, LPCTSTR strTitle);
	void	OnBrowserLocationChange(LPCTSTR strURL, HICON hFavicon);

	LRESULT OnOpenWithExProp(_EXPROP_ARGS *pArgs);
	void	OnHilightSwitchChange(bool bOn) {
		m_GlobalConfigManageData.pGlobalConfig->bHilightSwitch = bOn;
	}
	void	OnUpdateUrlSecurityList();
	void	OnUpdateCustomContextMenu();
	void	OnSetProxyToChildFrame();

	void	OnCleanUpNewProcessSharedMemHandle(HANDLE hDel) { ::CloseHandle(hDel); }
	void	OnSetDLConfigToGlobalConfig();

	void	OnFileOpen(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnFileRecent(UINT uNotifyCode, int nID, CWindow wndCtl);

	void	OnEditOperation(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSearchBarCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT	OnFindKeyWord(LPCTSTR lpszKeyWord, BOOL bBack, long Flags = 0);
	LRESULT OnHilight(CString strKeyword);

	void	OnViewBar(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSetFocusToBar(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnViewFullScreen(UINT uNotifyCode, int nID, CWindow wndCtl) { _FullScreen(!m_bFullScreen); }

	void	OnFavoriteAdd(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnFavoriteOrganize(UINT uNotifyCode, int nID, CWindow wndCtl);

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
	enum { kPosMRU	= 11 };

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
	CChildFrameClient	m_ChildFrameClient;		// ChildFrameの親ウィンドウ
//	CDonutExplorerBar	m_ExplorerBar;

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
	pImpl->UserOpenMultiFile(vecOpenData);
}


HWND	CMainFrame::GetHWND()
{
	return pImpl->m_hWnd;
}

HWND	CMainFrame::GetActiveChildFrameHWND()
{
	return pImpl->GetActiveChildFrameHWND();
}

/// 現在表示中のページで選択されたテキストを返す
CString	CMainFrame::GetActiveSelectedText()
{
	return CString();
}


