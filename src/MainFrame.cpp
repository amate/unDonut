/**
 *	@file	MainFrame.cpp
 *	@brief	メインフレームの実装
 *	@note
 *		+++ mainfrm.h を ヘッダとcppに分割。また、クラス名に併せてファイル名もMainFrameに変更.
 */

#include "stdafx.h"
#include "MainFrame.h"
#include <codecvt>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\xml_parser.hpp>
#include <boost\scope_exit.hpp>
#include "XmlFile.h"
#include "DialogKiller.h"
#include "dialog/DebugWindow.h"
//#include "PropertySheet.h"
#include "DonutOption.h"
#include "MenuEncode.h"
#include "StyleSheetOption.h"
#include "ExStyle.h"
#include "MenuDropTargetWindow.h"
#include "ParseInternetShortcutFile.h"

#include "option/AddressBarPropertyPage.h"	//+++ AddressBar.hより分離
#include "option/SearchPropertyPage.h"		//+++ SearchBar.hより分離
#include "option/LinkBarPropertyPage.h" 	//+++
#include "option/UrlSecurityOption.h"		//+++
#include "option/RightClickMenuDialog.h"

#include "DialogHook.h"
#include "dialog/OpenURLDialog.h"
#include "api.h"
#include "PluginEventImpl.h"
#include "Thumbnail.h"
#include "FaviconManager.h"
#include "GdiplusUtil.h"


#ifdef _DEBUG
	const bool _Donut_MainFrame_traceOn = false;
	#define dmfTRACE	if (_Donut_MainFrame_traceOn)  ATLTRACE
#else
	#define dmfTRACE
#endif


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




static const CLSID CLSID_IEnumPrivacyRecords = { 0x3050f844, 0x98b5, 0x11cf, { 0xbb, 0x82, 0x00, 0xaa, 0x00, 0xbd, 0xce, 0x0b } };

const UINT		CMainFrame::STDBAR_ID[] 	= { ATL_IDW_COMMAND_BAR, ATL_IDW_TOOLBAR				, IDC_ADDRESSBAR, IDC_MDITAB , IDC_LINKBAR				   , IDC_SEARCHBAR					};
const UINT		CMainFrame::STDBAR_STYLE[]	= { RBBS_USECHEVRON    , RBBS_USECHEVRON | RBBS_BREAK	, RBBS_BREAK	, RBBS_BREAK , RBBS_USECHEVRON | RBBS_BREAK, RBBS_BREAK 					};
//const UINT	CMainFrame::STDBAR_STYLE[]	= { RBBS_USECHEVRON    , RBBS_USECHEVRON | RBBS_BREAK	, RBBS_BREAK	, RBBS_BREAK , RBBS_USECHEVRON | RBBS_BREAK, RBBS_USECHEVRON | RBBS_BREAK	};
const LPTSTR	CMainFrame::STDBAR_TEXT[]	= { _T("")/*NULL*/	   , _T("")/*NULL*/ 				,_T("アドレス") , NULL		 , _T("リンク") 			   , _T("検索") 					};	// memo. NULL だと一番左のボタンをシェブロンに含めることができない



///////////////////////////////////////////////////////////////
// CChildFrameClient


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
	SetRedraw(FALSE);
	CChildFrameCommandUIUpdater::ChangeCommandUIMap(hWndChildFrame);
	if (m_hWndChildFrame) {
		::SendMessage(m_hWndChildFrame, WM_CHILDFRAMEACTIVATE, (WPARAM)hWndChildFrame, (LPARAM)m_hWndChildFrame);
		::ShowWindow/*Async*/(m_hWndChildFrame, FALSE);
	}
	
	if (hWndChildFrame) {
		::SendMessage(hWndChildFrame, WM_CHILDFRAMEACTIVATE, (WPARAM)hWndChildFrame, (LPARAM)m_hWndChildFrame);
		//::ShowWindow(hWndChildFrame, TRUE);
		RECT rcClient;
		GetClientRect(&rcClient);
		::SetWindowPos(hWndChildFrame, NULL, 0, 0, rcClient.right, rcClient.bottom, /*SWP_ASYNCWINDOWPOS | */SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOREDRAW);
		//::RedrawWindow(hWndChildFrame, NULL, NULL, RDW_FRAME | RDW_INVALIDATE/* | RDW_UPDATENOW*/ | RDW_ALLCHILDREN);
		//::BringWindowToTop(hWndChildFrame);
	} else {
		InvalidateRect(NULL);
	}

	m_hWndChildFrame = hWndChildFrame;
		
	SetRedraw(TRUE);
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_ERASE | RDW_ERASENOW | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
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




//////////////////////////////////////////////////////////////////////////////
// CMainFrame

#ifdef _DEBUG
void	CMainFrame::OnDebugCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{

	RestoreAllTab();
}
#endif

// ===========================================================================
// 初期化

CMainFrame::CMainFrame()
	: m_FavoriteMenu(this, ID_INSERTPOINT_FAVORITEMENU)
	, m_FavGroupMenu(this, ID_INSERTPOINT_GROUPMENU)
	, m_nBackUpTimerID(0)
	, CDDEMessageHandler<CMainFrame>( _T("Donut") )
	, m_hWndFocus(NULL)
	, m_ExplorerBar(m_wndSplit)
	, m_ScriptMenu(ID_INSERTPOINT_SCRIPTMENU, _T("(empty)"), ID_INSERTPOINT_SCRIPTMENU, ID_INSERTPOINT_SCRIPTMENU_END)
	, m_DropScriptMenu(ID_INSERTPOINT_SCRIPTMENU, _T("(empty)"), ID_INSERTPOINT_SCRIPTMENU, ID_INSERTPOINT_SCRIPTMENU_END)
	, m_MenuEncode(this)
	, m_nMenuBarStyle(-1)		//+++
	, m_bTray(0)				//+++
	, m_bFullScreen(0)			//+++
	, m_bOldMaximized(0)		//+++
	//, m_bMinimized(0)			//+++
//	, m_DownloadManager(this)
	, m_TranslateMenu(this)
	, m_styleSheetMenu(m_hWnd)
	, m_bWM_TIMER(false)
	, m_bNowSaveAllTab(false)
{
	g_pMainWnd = this;			//+++ CreateEx()中にプラグイン初期化とかで参照されるので、呼び元でなく CMainFreameで設定するように変更.
}



//+++ 各初期化処理ごとに関数分離
/** メインフレーム作成. 各種初期化.
 */
int		CMainFrame::OnCreate(LPCREATESTRUCT /*lpCreateStruct*/)
{
	SetMsgHandled(FALSE);

	init_message_loop();							// メッセージループの準備

	// オプションでお気に入りメニューの表示変更時、お気に入りメニュー更新
	CFavoritesMenuOption::SetFuncRefreshFav(std::bind(&CMainFrame::_RefreshFavMenu, this));

	init_menus_infomation();						// メニュー情報の初期化

	HWND hWndCmdBar 	= init_commandBarWindow();	// メニューバーの初期化
	HWND hWndToolBar	= init_toolBar();			// ツールバーの初期化
	HWND hWndAddressBar	= init_addressBar();		// アドレスバーの初期化
	HWND hWndSearchBar	= init_searchBar();			// 検索バーの初期化
	HWND hWndMDITab		= init_tabCtrl();			// タブバーの初期化
	HWND hWndLinkBar	= init_linkBar();			// リンクバーの初期化

	m_FindBar.Create(m_hWnd);
	m_FindBar.SetUpdateLayoutFunc(boost::bind(&CMainFrame::UpdateLayout, this, _1));

	init_rebar();									// リバーの初期化
	init_statusBar();								// ステータスバーの初期化
	init_pluginManager();							// プラグインマネージャの初期化

	// 各種バーの配置
	init_band_position( hWndCmdBar,hWndToolBar,hWndAddressBar,hWndMDITab,hWndLinkBar,hWndSearchBar );

	init_loadStatusBarState();						// ステータスバーの状態設定
	init_splitterWindow();							// 窓分割(エクスプローラバーと通常ページ)の初期化
	init_ChildFrameClientWindow();					//
	init_explorerBar();								// エクスプローラバーの初期化
	init_mdiClient_misc(hWndCmdBar, hWndToolBar);	// mdi-client関係の雑多な設定

	m_ChildFrameUIState.SetMainFrameHWND(m_hWnd);
	CmdUIAddToolBar(hWndToolBar);					// set up UI
	CmdUIAddToolBar(m_SearchBar.GetHWndToolBar());	// set up UI

	CDLControlOption::SetUserAgent();				// ユーザーエージェントの設定

	//SetAutoBackUp();//OnBackUpOptionChanged(0,0,0);// OnCreate後の処理で別途呼び出すようにした.
	RegisterDragDrop();	//DragAcceptFiles();		// ドラッグ＆ドロップ準備

	init_sysMenu();									// システムメニューに追加

	SetTimer(ENT_READ_ACCEL, 200);					// アクセラキー読み込み. 遅延させるためにWM_TIMERで処理.

	InitSkin();										//スキンを初期化

	CDonutSimpleEventManager::RaiseEvent(EVENT_INITIALIZE_COMPLETE);	// イベント関係の準備
	CDialogHook::InstallHook(m_hWnd);				// ダイアログ関係の準備

	m_DownloadManager.SetParent(m_hWnd);

	init_loadPlugins();								// プラグインを読み込む

	UpdateLayout();									// 画面更新

	GdiplusInit();

	return 0;	//return lRet;
}

//-------------------------------------
/// initialize menus' infomation
void CMainFrame::init_menus_infomation()
{
	m_MainFrameMenu.LoadMenu(IDR_MAINFRAME);

	/* お気に入りメニュー設定 */
	CMenuHandle 	   menu 		  = m_MainFrameMenu;
	CMenuHandle 	   menuFav		  = menu.GetSubMenu(_nPosFavoriteMenu);
	m_FavoriteMenu.InstallExplorerMenu(menuFav);
	m_FavoriteMenu.SetTargetWindow(m_hWnd);
	m_FavoriteMenu.RefreshMenu();

	/* お気に入りグループ設定 */
	CMenuHandle 	   menuGroup	  = menuFav.GetSubMenu(_nPosFavGroupMenu);
	m_FavGroupMenu.InstallExplorerMenu(menuGroup);
	m_FavGroupMenu.SetTargetWindow(m_hWnd);
	m_FavGroupMenu.RefreshMenu();

	/* スタイルシートメニュー設定 */
	CMenuHandle 	   menuCss		  = menu.GetSubMenu(_nPosCssMenu);
	CMenuHandle 	   menuCssSub	  = menuCss.GetSubMenu(_nPosSubCssMenu);
	m_styleSheetMenu.SetRootDirectoryPath( Misc::GetExeDirectory() + _T("CSS") );
	m_styleSheetMenu.SetTargetWindow(m_hWnd);
	m_styleSheetMenu.InstallExplorerMenu(menuCssSub);
	m_styleSheetMenu.RefreshMenu();

	/* スクリプトメニュー設定 */
	m_ScriptMenuMst.LoadMenu(IDR_SCRIPT);
	m_ScriptMenu.SetRootDirectoryPath( Misc::GetExeDirectory() + _T("Script") );
	m_ScriptMenu.SetTargetWindow(m_hWnd);
	m_ScriptMenu.InstallExplorerMenu(m_ScriptMenuMst);
	m_ScriptMenu.RefreshMenu();

	/* DropDownメニューの方のユーザースクリプトメニューを設定 */
	OnMenuGetScript();

	/* エンコードメニュー設定 */
	CMenuHandle 	   menuEncode	  = menu.GetSubMenu(_nPosEncodeMenu);
	m_MenuEncode.Init(menuEncode, _nPosEncodeMenuSub);

	// 最近閉じたタブのメニューの設定やオプションの設定を行う
	m_RecentClosedTabList.SetMenuHandle(menu.GetSubMenu(0).GetSubMenu(_nPosMRU));
	m_RecentClosedTabList.SetMaxEntries(CMainOption::s_nMaxRecentClosedTabCount);
	m_RecentClosedTabList.SetMenuType(CMainOption::s_RecentClosedTabMenuType);
	m_RecentClosedTabList.ReadFromXmlFile();
	m_RecentClosedTabList.UpdateMenu();

	//MenuDropTaget
	m_wndMenuDropTarget.Create(m_hWnd, rcDefault, _T("MenuDropTargetWindow"), WS_POPUP, 0);
	m_wndMenuDropTarget.SetTargetMenu(menu);
}

//-------------------------------------
/// create command bar window
HWND	CMainFrame::init_commandBarWindow()
{
	SetMenu(NULL);		// remove menu
	HWND	hWndCmdBar = m_CmdBar.Create(m_hWnd,rcDefault,NULL,MTL_SIMPLE_CMDBAR2_PANE_STYLE,0,ATL_IDW_COMMAND_BAR);
	ATLASSERT( ::IsWindow(hWndCmdBar) );

	CIniFileI	prFont( g_szIniFileName, _T("Main") );
	MTL::CLogFont	lf;
	lf.InitDefault();
	if ( lf.GetProfile(prFont) ) {
		m_CmdBar.SetMenuLogFont(lf);

		CFontHandle 	font;	//\\ SetFontもここでするように
		MTLVERIFY( font.CreateFontIndirect(&lf) );
		if (font.m_hFont) 
			SetFont(font);
	}

	m_CmdBar.AttachMenu(m_MainFrameMenu);

	return hWndCmdBar;
}

//-------------------------------------
/// create toolbar
HWND	CMainFrame::init_toolBar()
{
  #if 1 //+++ 関数分離の都合取得しなおし
	CMenuHandle		menu		= m_MainFrameMenu;
	CMenuHandle 	menuFav		= menu.GetSubMenu(_nPosFavoriteMenu);
	CMenuHandle 	menuGroup	= menuFav.GetSubMenu(_nPosFavGroupMenu);
	CMenuHandle 	menuCss		= menu.GetSubMenu(_nPosCssMenu);
	CMenuHandle 	menuCssSub	= menuCss.GetSubMenu(_nPosSubCssMenu);
  #endif

	HWND	hWndToolBar   = m_ToolBar.Create(m_hWnd);
	ATLASSERT( ::IsWindow(hWndToolBar) );

	m_ToolBar.SetDropDownMenu(menuFav, menuGroup, menuCssSub);

	if (m_CmdBar.m_fontMenu.m_hFont) {	// コマンドバーのフォント設定と同じに
		LOGFONT lf;
		m_CmdBar.m_fontMenu.GetLogFont(&lf);
		CFontHandle font;
		m_ToolBar.SetFont(font.CreateFontIndirect(&lf));
	}

	return hWndToolBar;
}

//-------------------------------------
/// create addressbar
HWND	CMainFrame::init_addressBar()
{
	HWND	hWndAddressBar = m_AddressBar.Create(m_hWnd, IDC_ADDRESSBAR, ID_VIEW_GO,
												 16, 16, RGB(255, 0, 255) );
	ATLASSERT( ::IsWindow(hWndAddressBar) );

	//m_AddressBar.m_comboFlat.SetDrawStyle(CSkinOption::s_nComboStyle);
	return hWndAddressBar;
}

//-------------------------------------
/// create searchbar
HWND	CMainFrame::init_searchBar()
{
	HWND	hWndSearchBar  = m_SearchBar.Create(m_hWnd);
	ATLASSERT( ::IsWindow(hWndSearchBar) );

	return	hWndSearchBar;
}

//-------------------------------------
/// create tabctrl
HWND	CMainFrame::init_tabCtrl()
{
	HWND	hWndMDITab = m_MDITab.Create(m_hWnd);
	ATLASSERT( ::IsWindow(hWndMDITab) );

	CFaviconManager::Init(hWndMDITab);

	return hWndMDITab;
}

//------------------------------------
/// create link bar
HWND	CMainFrame::init_linkBar()
{
	//CIniFileI prLnk( g_szIniFileName, _T("LinkBar") );
	//DWORD		dwStyle   = prLnk.GetValue( _T("ExtendedStyle") );
	//prLnk.Close();

	//m_LinkBar.SetOptionalStyle(dwStyle);
	HWND	hWndLinkBar   = m_LinkBar.Create(m_hWnd/*, rcDefault, NULL, ATL_SIMPLE_TOOLBAR_PANE_STYLE, 0, IDC_LINKBAR*/);
	ATLASSERT( ::IsWindow(hWndLinkBar) );

	return 	hWndLinkBar;
}

//------------------------------------
/// create rebar
void	CMainFrame::init_rebar()
{
	DWORD	dwRebarStyle = ATL_SIMPLE_REBAR_STYLE | CCS_NOPARENTALIGN | CCS_NODIVIDER | RBS_DBLCLKTOGGLE;
	{
		CIniFileI	pr( g_szIniFileName, _T("ReBar") );
		DWORD		dwNoBoader 		= pr.GetValue( _T("NoBoader") );
		if (dwNoBoader)
			dwRebarStyle &= ~RBS_BANDBORDERS;	// ボーダーを消す
	}

	CreateSimpleReBar(dwRebarStyle);
	m_ReBar.SubclassWindow(m_hWndToolBar);

	//CreateSimpleReBar(MTL_SIMPLE_REBAR_STYLE | RBS_DBLCLKTOGGLE);
}

//------------------------------------
/// create statusbar
void	CMainFrame::init_statusBar()
{
	//CreateSimpleStatusBar();
	m_wndStatusBar.Create(m_hWnd, ATL_IDS_IDLEMESSAGE,
							 WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | SBARS_SIZEGRIP | SBT_TOOLTIPS);
	ATLASSERT( ::IsWindow(m_wndStatusBar) );
	m_hWndStatusBar = m_wndStatusBar.m_hWnd;
	//		int nPanes[] = { ID_DEFAULT_PANE, ID_PROGRESS_PANE, ID_COMBOBOX_PANE};
	static int   nPanes[] = { ID_DEFAULT_PANE, ID_PROGRESS_PANE, ID_PRIVACY_PANE, ID_SECURE_PANE, ID_COMBOBOX_PANE };
	m_wndStatusBar.SetPanes( nPanes, _countof(nPanes), false );
	m_wndStatusBar.SetCommand( ID_PRIVACY_PANE, ID_PRIVACYREPORT );
	m_wndStatusBar.SetCommand( ID_SECURE_PANE, ID_SECURITYREPORT);
	m_wndStatusBar.SetIcon( ID_PRIVACY_PANE, 1 );				//minit
	m_wndStatusBar.SetIcon( ID_SECURE_PANE , 0 );				//minit
	m_wndStatusBar.SetOwnerDraw( m_wndStatusBar.IsValidBmp() );


	enum {	//+++ IDはデフォルトの名前になっているが、交換した場合にややこしいので、第一領域、第二領域として扱う.
		ID_PAIN_1	= ID_PROGRESS_PANE,
		ID_PAIN_2	= ID_COMBOBOX_PANE,
	};
	DWORD		dwSzPain1 = 125;
	DWORD		dwSzPain2 = 125;
	{
		DWORD		dwVal	  = 0;
		CIniFileI	pr( g_szIniFileName, _T("StatusBar") );

		if (pr.QueryValue( dwVal, _T("SizePain") ) == ERROR_SUCCESS) {
			dwSzPain1 = LOWORD(dwVal);
			dwSzPain2 = HIWORD(dwVal);
		}

		if (pr.QueryValue( dwVal, _T("SwapPain") ) == ERROR_SUCCESS)
			g_bSwapProxy = dwVal != 0;
	}

	//+++ 位置交換の修正.
	if (g_bSwapProxy == FALSE) {
		g_dwProgressPainWidth = dwSzPain1;					//+++ 手抜きでプログレスペインの横幅をグローバル変数に控える.
		m_wndStatusBar.SetPaneWidth(ID_PAIN_1, 0);			//dwSzPain1); //起動時はペインサイズが0
		if (m_wndStatusBar.GetProxyComboBox().IsUseIE())
			dwSzPain2 = 0;									// IEのProxyを使う場合はProxyペインサイズを0に
		m_wndStatusBar.SetPaneWidth(ID_PAIN_2, dwSzPain2);
	} else {	// 交換しているとき.
		g_dwProgressPainWidth = dwSzPain2;					//+++ 手抜きでプログレスペインの横幅をグローバル変数に控える.
		m_wndStatusBar.SetPaneWidth(ID_PAIN_2, dwSzPain2);	//+++ 交換してるときは、最初からサイズ確保.
		if (m_wndStatusBar.GetProxyComboBox().IsUseIE())
			dwSzPain1 = 0;									// IEのProxyを使う場合はProxyペインサイズを0に
		m_wndStatusBar.SetPaneWidth(ID_PAIN_1, dwSzPain1);
	}

	m_wndStatusBar.SetPaneWidth(ID_SECURE_PANE	, 25);
	m_wndStatusBar.SetPaneWidth(ID_PRIVACY_PANE , 25);
}

//---------------------------------------------
/// Plugin Toolbar Load
void CMainFrame::init_pluginManager()
{
	CPluginManager::Init();	//\\いらないけどとりあえず読んどく
	CPluginManager::ReadPluginData(PLT_TOOLBAR, m_hWnd);
	CPluginManager::LoadAllPlugin(PLT_TOOLBAR, m_hWnd, true);	//ツールバープラグインを全部ロード

	{
		int nCount = CPluginManager::GetCount(PLT_TOOLBAR);
		for (int i = 0; i < nCount; i++) {
			HWND hWnd = CPluginManager::GetHWND(PLT_TOOLBAR, i);

			if ( ::IsWindow(hWnd) )
				::SetProp( hWnd, _T("Donut_Plugin_ID"), HANDLE( IDC_PLUGIN_TOOLBAR + i) );
		}
	}
	//LoadPluginToolbars();
}

//-------------------------------------
/// load band position
void CMainFrame::init_band_position(
	HWND 	hWndCmdBar,
	HWND	hWndToolBar,
	HWND	hWndAddressBar,
	HWND	hWndMDITab,
	HWND	hWndLinkBar,
	HWND	hWndSearchBar  )
{
	CSimpleArray<HWND> aryHWnd;
	aryHWnd.Add( hWndCmdBar 	);		// メニューバー
	aryHWnd.Add( hWndToolBar	);		// ツールバー
	aryHWnd.Add( hWndAddressBar );		// アドレスバー
	aryHWnd.Add( hWndMDITab 	);		// タブバー
	aryHWnd.Add( hWndLinkBar	);		// リンクバー
	aryHWnd.Add( hWndSearchBar	);		// 検索バー

	int nToolbarPluginCount = CPluginManager::GetCount(PLT_TOOLBAR);
	CReBarBandInfo*   pRbis	= new CReBarBandInfo[STDBAR_COUNT + nToolbarPluginCount];

	for (int nIndex = 0; nIndex < aryHWnd.GetSize(); nIndex++) {
		pRbis[nIndex].nIndex	= nIndex;
		pRbis[nIndex].hWnd		= aryHWnd	  [ nIndex ];
		pRbis[nIndex].nID		= STDBAR_ID   [ nIndex ];
		pRbis[nIndex].fStyle	= STDBAR_STYLE[ nIndex ];
		pRbis[nIndex].lpText	= STDBAR_TEXT [ nIndex ];
		pRbis[nIndex].cx		= 0;
	}

	for (int nIndex = 0; nIndex < nToolbarPluginCount; nIndex++) {
		pRbis[STDBAR_COUNT + nIndex].nIndex   = STDBAR_COUNT + nIndex;
		pRbis[STDBAR_COUNT + nIndex].hWnd	  = CPluginManager::GetHWND(PLT_TOOLBAR, nIndex);
		pRbis[STDBAR_COUNT + nIndex].nID	  = IDC_PLUGIN_TOOLBAR + nIndex;
		pRbis[STDBAR_COUNT + nIndex].fStyle   = RBBS_BREAK;
		pRbis[STDBAR_COUNT + nIndex].lpText   = NULL;
		pRbis[STDBAR_COUNT + nIndex].cx 	  = 0;
	}

	{
		CIniFileI pr( g_szIniFileName, _T("ReBar") );
		MtlGetProfileReBarBandsState( pRbis, pRbis + STDBAR_COUNT + nToolbarPluginCount, pr, *this);
	}

	delete[] pRbis;

	m_CmdBar.RefreshBandIdealSize(m_hWndToolBar);
	m_AddressBar.InitReBarBandInfo(m_hWndToolBar);	// if you dislike a header, remove this.
	ShowLinkText(CAddressBarOption::s_bTextVisible);
}

//-------------------------------------
/// load status bar state
void CMainFrame::init_loadStatusBarState()
{
	CIniFileI pr( g_szIniFileName, _T("Main") );
	BOOL	bStatusBarVisible = TRUE;
	MtlGetProfileStatusBarState(pr, m_hWndStatusBar, bStatusBarVisible);
}

//-------------------------------------
/// splitter window
void CMainFrame::init_splitterWindow()
{
	m_hWndClient	= m_wndSplit.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	ATLASSERT( ::IsWindow(m_hWndClient) );
	m_wndSplit.SetSplitterExtendedStyle(SPLIT_GRADIENTBAR/*0*/);
}

//-------------------------------------
/// create mdi client window
void CMainFrame::init_ChildFrameClientWindow()
{
	m_ChildFrameClient.Create(m_hWndClient);
	ATLASSERT( m_ChildFrameClient.IsWindow() );
}

//------------------------------------
/// pane container
void CMainFrame::init_explorerBar()
{
	m_ExplorerBar.Create(m_hWndClient);
	m_ExplorerBar.Init(m_ChildFrameClient);
}

//------------------------------------
/// MDIClient misc
void CMainFrame::init_mdiClient_misc(HWND hWndCmdBar, HWND hWndToolBar)
{
	m_mcCmdBar.InstallAsMDICmdBar(hWndCmdBar, m_ChildFrameClient, CMainOption::s_bTabMode);
	m_mcToolBar.InstallAsStandard(hWndToolBar, m_hWnd, true, ID_VIEW_FULLSCREEN);

	m_mcCmdBar.ShowButton(!CMenuOption::s_bDontShowButton);	// メニューの閉じるボタンを表示しない
	m_MDITab.SetChildFrameClient(&m_ChildFrameClient);
}

//-----------------------------------
/// メッセージフィルタとアイドルハンドラを登録
void CMainFrame::init_message_loop()
{
	// message loop
	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
}


//--------------------------------------
/// システムメニューに「メニューを表示]を追加
void CMainFrame::init_sysMenu()
{
	HMENU		hSysMenu = ::GetSystemMenu(m_hWnd, FALSE);
	//		::AppendMenu(hSysMenu, MF_ENABLED|MF_STRING, ID_VIEW_COMMANDBAR, _T("メニューを表示"));

	TCHAR		cText[]	 = _T("メニューを表示");
	MENUITEMINFO  menuInfo = { sizeof(MENUITEMINFO) };
	menuInfo.fMask		= MIIM_ID | MIIM_TYPE;
	menuInfo.fType		= MFT_STRING;
	menuInfo.wID		= ID_VIEW_COMMANDBAR;
	menuInfo.dwTypeData = cText;
	menuInfo.cch		= sizeof (cText);
	::InsertMenuItem(hSysMenu, 0, MF_BYPOSITION, &menuInfo);
}

//--------------------------------------
/// プラグインを読み込む
void CMainFrame::init_loadPlugins()
{
	// エクスプローラーバープラグイン読み込み
	CPluginManager::LoadAllPlugin(PLT_EXPLORERBAR, m_ExplorerBar.m_PluginBar);

	//オペレーションプラグインのロード
	CPluginManager::ReadPluginData(PLT_OPERATION);

  #if 1	//+++ 無理やり DockingBarプラグインを、ExplorerBarとして扱ってみる...
	CPluginManager::LoadAllPlugin(PLT_DOCKINGBAR, m_ExplorerBar.m_PluginBar);
  #else
	CPluginManager::LoadAllPlugin(PLT_DOCKINGBAR, m_hWnd);
  #endif
}

// ===========================================================================
// 終了処理
#if 1
CMainFrame::~CMainFrame()
{
	GdiplusTerm();
}

//---------------------------------------
/// システムが終了する時
void CMainFrame::OnEndSession(BOOL wParam, UINT lParam) 						//　ShutDown minit
{
	if (wParam == TRUE)
		OnDestroy();
}

//------------------------------------
/// 一時フォルダのファイルを削除してフォルダを削除する
static void _RemoveDonutTempDirectory()
{
	CString strTempPath;
	if (GetDonutTempPath(strTempPath)) {
		MtlForEachFile( strTempPath, [](const CString& strFile) {
				::DeleteFile(strFile);
		});
		::RemoveDirectory(strTempPath);
	}
}

//---------------------------------------
/// ウィンドウ終了時
void CMainFrame::OnDestroy()
{
	SetMsgHandled(FALSE);

	_PrivateTerm();		// 設定の保存

	if (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_RECENTCLOSE)
		m_RecentClosedTabList.DeleteRecentClosedTabFile();
	else 
		m_RecentClosedTabList.WriteToXmlFile();

	//\\MtlSendCommand(m_hWnd, ID_VIEW_STOP_ALL);									// added by DOGSTORE

	//CSearchBoxHook::UninstallSearchHook();
	CDialogHook::UninstallHook();
#ifdef _DEBUG
	//デバッグウィンドウ削除
	//m_wndDebug.Destroy();
#endif
	//全プラグイン解放
	CPluginManager::Term();

	//:::m_wndMDIClient.UnsubclassWindow();
	m_ReBar.UnsubclassWindow();

	//\\?CCriticalIdleTimer::UninstallCriticalIdleTimer();

	// what can I trust?
	//:::ATLASSERT( ::IsMenu(m_hMenu) );
	//::: ::DestroyMenu(m_hMenu);

	_RemoveDonutTempDirectory();

	OnMenuRefreshScript(FALSE);

	RevokeDragDrop();

	// メッセージループからメッセージフィルタとアイドルハンドラを削除
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->RemoveMessageFilter(this);
    pLoop->RemoveIdleHandler(this);

	ATLTRACE(_T("メインフレームの終了中...\n"));
#if 0
	CString strPath = Misc::GetExeDirectory() + _T("lock");
	HANDLE hHandle = ::CreateFile(strPath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hHandle != INVALID_HANDLE_VALUE) {
		::CloseHandle(hHandle);
	} else {
		ATLASSERT(FALSE);
	}
#endif
}


//----------------------------------
/// メインフレームのＸボタンを押したとき
void CMainFrame::OnClose()
{
	int nDLCount = m_DownloadManager.GetDownloadingCount();
	if (nDLCount > 0) {
		CString msg;
		msg.Format(_T("ダウンロード中のアイテムが %d個あります。\n終了しますか？"), nDLCount);
		if (MessageBox(msg, _T("確認"), MB_ICONQUESTION | MB_OKCANCEL | MB_DEFBUTTON2) == IDCANCEL)
			return;
	}
	SetMsgHandled(FALSE);

	if ( !CDonutConfirmOption::OnDonutExit(m_hWnd) ) {
		SetMsgHandled(TRUE);
		if (IsWindowVisible() == FALSE) {
			SetHideTrayIcon();
			Sleep(100);
		}
		return;
	}

	// タイマーをとめる.
	if (m_nBackUpTimerID != 0) {
		KillTimer(m_nBackUpTimerID);
		m_nBackUpTimerID	= 0;			//+++ 一応 0クリアしとく.
	}

	// 現在表示中のタブを保存する
	if (CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_LATEST) {
		SaveAllTab();
	} else {
		// 最近閉じたタブに追加
		m_MDITab.ForEachWindow([](HWND hWndChildFrame) {
			::SendMessage(hWndChildFrame, WM_CLOSE, 0, 0);
		});
	}
	//:::CChildFrame::SetMainframeCloseFlag();	//+++ mainfrmがcloseすることをChildFrameに教える(ナビロックのclose不可をやめるため)


	if ( IsFullScreen() ) {
		CMainOption::s_dwMainExtendedStyle |= MAIN_EX_FULLSCREEN;	// save style
		_ShowBandsFullScreen(FALSE);								// restore bands position
	} else {
		CMainOption::s_dwMainExtendedStyle &= ~MAIN_EX_FULLSCREEN;
	}

	m_mcCmdBar.Uninstall();
	m_mcToolBar.Uninstall();

	_WriteProfile();

	CMainOption::s_bAppClosing = true;

	// delete cash
	DelTempFiles();
	// delete history
	DelHistory();
}


//------------------------------------
/// キャッシュ(htmlなど)を削除する
void CMainFrame::DelTempFiles()
{
	bool						bDelCash	 = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_CASH  ) != 0;	//+++ ? TRUE : FALSE;
	bool						bDelCookie	 = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_COOKIE) != 0;	//+++ ? TRUE : FALSE;

	if (bDelCash == FALSE && bDelCookie == FALSE)
		return;

	bool						bDone		 = FALSE;
	LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;

	DWORD						dwTrySize;
	DWORD						dwEntrySize  = 4096;				// start buffer size
	HANDLE						hCacheDir	 = NULL;
	DWORD						dwError 	 = ERROR_INSUFFICIENT_BUFFER;

	do {
		switch (dwError) {
			// need a bigger buffer
		case ERROR_INSUFFICIENT_BUFFER:
			delete[] lpCacheEntry;
			lpCacheEntry			   = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
			lpCacheEntry->dwStructSize = dwEntrySize;
			dwTrySize				   = dwEntrySize;
			BOOL bSuccess;
			if (hCacheDir == NULL)
				bSuccess = ( hCacheDir = FindFirstUrlCacheEntry(NULL, lpCacheEntry, &dwTrySize) ) != NULL;
			else
				bSuccess = FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize) != 0;

			if (bSuccess)
				dwError = ERROR_SUCCESS;
			else {
				dwError 	= GetLastError();
				dwEntrySize = dwTrySize;							// use new size returned
			}
			break;

			// we are done
		case ERROR_NO_MORE_ITEMS:
			bDone					   = TRUE;
			//x bResult 			   = TRUE;						//+++ 代入あれど未使用のよう.
			break;

			// we have got an entry
		case ERROR_SUCCESS:
			if (bDelCookie && lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY)
				DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);

			// Fixed by zzZ(thx
			if ( bDelCash && !( lpCacheEntry->CacheEntryType & (COOKIE_CACHE_ENTRY | URLHISTORY_CACHE_ENTRY) ) )
				DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);

			// get ready for next entry
			dwTrySize				   = dwEntrySize;

			if ( FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize) )
				dwError = ERROR_SUCCESS;
			else {
				dwError 	= GetLastError();
				dwEntrySize = dwTrySize;							// use new size returned
			}
			break;

			// unknown error
		default:
			bDone	= TRUE;
			break;
		}

		if (bDone) {
			delete[] lpCacheEntry;

			if (hCacheDir)
				FindCloseUrlCache(hCacheDir);
		}
	} while (!bDone);
}

//-------------------------------
/// 履歴を削除する
void CMainFrame::DelHistory()
{
	bool	bDelHistory  = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_HISTORY) != 0;	//+++ ? TRUE : FALSE;
	if (bDelHistory == false)
		return;

	CComPtr<IUrlHistoryStg2>	spUrlHistoryStg2;
	HRESULT hr = spUrlHistoryStg2.CoCreateInstance(CLSID_CUrlHistory);
	if ( SUCCEEDED(hr) )
		hr = spUrlHistoryStg2->ClearHistory();
}
#endif	// term

// ===========================================================================
// MainFrame_PreTrnMsgから


#ifndef WM_XBUTTONDOWN
 #define WM_XBUTTONDOWN 				0x020B
 #define WM_XBUTTONUP					0x020C
 #define GET_KEYSTATE_WPARAM(wParam)	( LOWORD(wParam) )
 #define GET_XBUTTON_WPARAM(wParam) 	( HIWORD(wParam) )
 #define MK_XBUTTON1					0x0020
 #define MK_XBUTTON2					0x0040
 #define XBUTTON1						0x0001
 #define XBUTTON2						0x0002
#endif

/// カーソルの下のウィンドウにホイールメッセージを通知する
static BOOL OnMouseWheelHook(MSG *pMsg, HWND hWndChild)
{
	CPoint pt(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));

	HWND hWndTarget = ::WindowFromPoint(pt);
	if (hWndTarget) {
		if (::IsChild(hWndChild, hWndTarget)) {
			CChildFrame* pChild = (CChildFrame *) ::SendMessage(hWndChild, WM_GET_CHILDFRAME, 0, 0);
			UINT nFlags = (UINT)LOWORD(pMsg->wParam);
			int	 zDelta = (short)HIWORD(pMsg->wParam);
			// 文字を拡大する
			if ( nFlags == MK_CONTROL ) {
				CComVariant	vEmpty;
				CComVariant vZoomSize;
				//\\ 現在の文字サイズを取得
				pChild->GetMarshalIWebBrowser()->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vEmpty, &vZoomSize); 
				if ( zDelta > 0 ){	
					vZoomSize.lVal += 1;
				} else {
					vZoomSize.lVal -= 1;
				}
				//\\ 文字サイズを変更
				pChild->GetMarshalIWebBrowser()->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vZoomSize, &vEmpty); 
				return TRUE;
			}

			// ページを拡大する
			if ( ::GetKeyState(VK_MENU) & 0x80 ){
				if ( zDelta > 0 ){
					//pChild->SetBodyStyleZoom(10, 0, true);
					::PostMessage(pChild->GetHwnd(), WM_COMMAND, ID_HTMLZOOM_ADD, 0);
				} else {
					::PostMessage(pChild->GetHwnd(), WM_COMMAND, ID_HTMLZOOM_SUB, 0);
					//pChild->SetBodyStyleZoom(-10, 0, true);
				}
				return TRUE;
			}
		}
		::SendMessage(hWndTarget, pMsg->message, pMsg->wParam, MAKELPARAM(pt.x, pt.y));
		return TRUE;
	}
	return FALSE;
}

//各ウィンドウへ(主にキー)メッセージを転送する
BOOL CMainFrame::PreTranslateMessage(MSG *pMsg)
{
	m_bWM_TIMER = pMsg->message == WM_TIMER;
	if (m_bWM_TIMER) {
		if (pMsg->hwnd == NULL && pMsg->lParam == 0)
			return FALSE;
		return FALSE;
	}

	//コマンドバー(メニュー)
	if ( m_CmdBar.PreTranslateMessage(pMsg) )
		return TRUE;

	//アドレスバー
	BOOL ptFlag = m_AddressBar.PreTranslateMessage(pMsg);
	if (ptFlag == _MTL_TRANSLATE_HANDLE)
		return TRUE;
	else if (ptFlag == _MTL_TRANSLATE_WANT)
		return FALSE;

	//検索バー
	ptFlag = m_SearchBar.PreTranslateMessage(pMsg);
	if (ptFlag == _MTL_TRANSLATE_HANDLE)
		return TRUE;
	else if (ptFlag == _MTL_TRANSLATE_WANT)
		return FALSE;

	//エクスプローラバー
	//if (m_ExplorerBar.PreTranslateMessage(pMsg)) return TRUE;
	ptFlag = m_ExplorerBar.PreTranslateMessage(pMsg);
	if (ptFlag == _MTL_TRANSLATE_HANDLE)
		return TRUE;
	else if (ptFlag == _MTL_TRANSLATE_WANT)
		return FALSE;

  #if 1
	//+++ アドレスバーorサーチバーにフォーカスが当たっている時、他の処理で余計なこと(キー入力)させないようにガードしてみる.
	int	bFocus = false;
	if (m_AddressBar.IsWindow())
		bFocus	|= (::GetFocus() == m_AddressBar.GetEditCtrl().m_hWnd);
	if (::IsWindow(m_SearchBar.GetEditCtrl()/*m_SearchBar.m_hWnd*/))
		bFocus	|= (::GetFocus() == m_SearchBar.GetEditCtrl().m_hWnd);
	HWND hWndFind = m_FindBar.GetHWND();
	if (::IsWindow(hWndFind))
		bFocus	|= ::IsChild(hWndFind, pMsg->hwnd);
  #endif

	// 基底クラス
	if (/*bFocus == 0 &&*/ baseClass::PreTranslateMessage(pMsg) )
		return TRUE;

	// アクティブ・チャイルド・ウィンドウ
	HWND hWnd = m_ChildFrameClient.GetActiveChildFrameWindow();


	// ホイール
	if ( pMsg->message == WM_MOUSEWHEEL && OnMouseWheelHook(pMsg, hWnd) )
		return TRUE;

	// サイドボタン
	if (pMsg->message == WM_XBUTTONUP) {
		if ( OnXButtonUp( GET_KEYSTATE_WPARAM(pMsg->wParam), GET_XBUTTON_WPARAM(pMsg->wParam),
						 CPoint( GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) ) ) )
			return TRUE;
	}

	// エクスプローラバーを自動で表示
	if (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_AUTOSHOW)
		ExplorerBarAutoShow(pMsg);

	// BHO プラグインへ
	if (bFocus == false && TranslateMessageToBHO(pMsg) )											//+++ bFocusチェック
		return TRUE;

#if 0	//::: これが固まる原因？
	if (bFocus == false && hWnd && ::SendMessage(hWnd, WM_FORWARDMSG, 0, (LPARAM) pMsg) )	//+++ bFocusチェック
		return TRUE;
#endif
	return FALSE; // IsDialogMessage(pMsg);
}

//---------------------------------------------
/// コントロールの配置更新
void CMainFrame::UpdateLayout(BOOL bResizeBars /*= TRUE*/)
{
	CRect	  rc;
	GetClientRect(&rc);

	if (bResizeBars) {
		CReBarCtrl rebar(m_hWndToolBar);
		if (rebar.m_hWnd == NULL) 
			goto END;

		CRect	   rcSrc;
		rebar.GetClientRect(&rcSrc);

		CRect	   rcNew(0, 0, rc.right, rcSrc.Height());
		rebar.MoveWindow(rcNew);
		rebar.RedrawWindow();
	}
END:
	UpdateBarsPosition(rc, bResizeBars);
	if (m_bFullScreen == false)
		rc.top++;

	HWND hWndFind = m_FindBar.GetHWND();
	if (::IsWindowVisible(hWndFind)) {
		CRect rcFind;
		::GetClientRect(hWndFind, &rcFind);
		::SetWindowPos( hWndFind, NULL, rc.left, rc.top, rc.right, rcFind.bottom, SWP_NOZORDER | SWP_NOACTIVATE );
		rc.top += rcFind.bottom;
	}

	if (m_hWndClient)
		::SetWindowPos( m_hWndClient, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER | SWP_NOACTIVATE );

	{
		//プラグインイベント - リサイズ
		CPluginManager::BroadCast_PluginEvent(DEVT_CHANGESIZE, 0, 0);
	}
}



BOOL CMainFrame::OnXButtonUp(WORD wKeys, WORD wButton, CPoint point)
{
	CString 	strKey;

	switch (wButton) {
	case XBUTTON1: strKey = _T("Side1"); break;
	case XBUTTON2: strKey = _T("Side2"); break;
	}

	DWORD		dwCommand = 0;
	{
		CIniFileI pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
		pr.QueryValue(dwCommand, strKey);
	}

	if (dwCommand == 0)
		return FALSE;

	::SendMessage(m_hWnd, WM_COMMAND, dwCommand, 0);
	return TRUE;
}

//----------------------------------------
/// カーソルがウィンドウの左端にあるとエクスプローラーバーを表示する
void CMainFrame::ExplorerBarAutoShow(MSG *pMsg)
{
	if (pMsg->message != WM_MOUSEMOVE)
		return;

	if (pMsg->wParam != 0)
		return;

	if (  m_ExplorerBar.IsFavBarVisible()		== false
	   && m_ExplorerBar.IsClipboardBarVisible() == false
	   && m_ExplorerBar.IsPanelBarVisible() 	== false
	   && m_ExplorerBar.IsPluginBarVisible()	== false)
		return;

	CPoint point;
	::GetCursorPos(&point);

	CRect  rcWndCmd;
	::GetWindowRect(m_hWndToolBar, &rcWndCmd);

	if ( rcWndCmd.PtInRect(point) )
		return;

	ScreenToClient(&point);

	CRect  rcWnd;
	GetClientRect(&rcWnd);

	if (m_ExplorerBar.IsExplorerBarHidden() == false) {
		CRect rcWndExp;
		m_ExplorerBar.GetClientRect(&rcWndExp);
		rcWnd.left += rcWndExp.Width() + 5;

		if ( rcWnd.PtInRect(point) )
			m_wndSplit.SetSinglePaneMode(SPLIT_PANE_RIGHT);
	} else {
		if (point.x < 10 && point.x >= 0) {
			m_ExplorerBar.ShowBar(m_ExplorerBar.m_dwExStyle, true);
		}
	}
}

//------------------------------------
/// プラグインにメッセージをフィルタする機会を与える
BOOL CMainFrame::TranslateMessageToBHO(MSG *pMsg)
{
  #if 1	//+++ お試しで、そもそもif文をやめる...やめれない?
	if (  (WM_KEYFIRST		 <= pMsg->message) && (pMsg->message <= WM_KEYLAST	)
	   || (WM_IME_SETCONTEXT <= pMsg->message) && (pMsg->message <= WM_IME_KEYUP)
	  #if 0	//+++ お試し...
	   || (WM_MOUSEFIRST     <= pMsg->message) && (pMsg->message <= WM_MOUSELAST)
	   || (WM_NCMOUSEMOVE    <= pMsg->message) && (pMsg->message <= 0xAD/*WM_NCXBUTTONDBLCLK*/)
	  #endif
	)
  #endif 
	{
		int nCount = CPluginManager::GetCount(PLT_TOOLBAR);

		for (int i = 0; i < nCount; i++) {
			if ( CPluginManager::Call_PreTranslateMessage(PLT_TOOLBAR, i, pMsg) )
				return TRUE;
		}
	}

	return FALSE;
}



// ===========================================================================
// スキン関係

void CMainFrame::InitSkin()
{
	/* フォント */
	m_MDITab.SetFont(CSkinOption::s_lfTabBar.CreateFontIndirect());
	m_AddressBar.SetFont(CSkinOption::s_lfAddressBar.CreateFontIndirect());
	m_SearchBar.SetFont(CSkinOption::s_lfSearchBar.CreateFontIndirect());
	m_LinkBar.SetFont(CSkinOption::s_lfLinkBar.CreateFontIndirect());
	m_wndStatusBar.SetProxyComboBoxFont(CSkinOption::s_lfProxyComboBox.CreateFontIndirect());

	/* スキン */
	initCurrentIcon();											//+++ アイコン
	m_CmdBar.setMenuBarStyle(m_hWndToolBar, false); 			//+++ メニュー (FEVATWHの短名にするか否か)
	m_CmdBar.InvalidateRect(NULL, TRUE);						//メニューバー
	m_ReBar.RefreshSkinState(); 								//ReBar
	m_MDITab.ReloadSkin();										//タブ
	CToolBarOption::GetProfile();
	m_ToolBar.ReloadSkin(); 									//ツールバー
	m_AddressBar.ReloadSkin(CSkinOption::s_nComboStyle);		//アドレスバー
	m_SearchBar.ReloadSkin(CSkinOption::s_nComboStyle); 		//検索バー
	//m_LinkBar.InvalidateRect(NULL, TRUE);						//リンクバー
	m_ExplorerBar.ReloadSkin(); 								//エクスプローラバー
	m_ExplorerBar.m_PanelBar.ReloadSkin();						//パネルバー
	m_ExplorerBar.m_PluginBar.ReloadSkin(); 					//プラグインバー
	
	m_wndStatusBar.ReloadSkin( CSkinOption::s_nStatusStyle		//ステータスバー
							 , CSkinOption::s_nStatusTextColor
							 , CSkinOption::s_nStatusBackColor);

	setMainFrameCaptionSw(CSkinOption::s_nMainFrameCaption);	//+++ メインフレームのキャプションの有無.


}

// [Donutのオプション]-[スキン]の[適用]が押されたとき
HRESULT CMainFrame::OnSkinChange()
{
	//メッセージをブロードキャストするべきか
	CSkinOption::GetProfile();

	InitSkin();

	//リフレッシュ
	RedrawWindow(NULL, NULL, RDW_INTERNALPAINT | RDW_UPDATENOW | RDW_ALLCHILDREN);;
	return 0;
}


#if 1 //+++	メインフレームのCaption on/off
void CMainFrame::setMainFrameCaptionSw(int sw)
{
	if (sw == 0) {	//+++ キャプションを外す場合
		ModifyStyle(WS_CAPTION, 0);
	} else {
		ModifyStyle(0, WS_CAPTION); 			//+++ キャプションをつける場合.
	}
	m_mcCmdBar.SetExMode(sw ? 0/*有*/ : m_hWnd/*無*/);

	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
}
#endif


///+++ 現在のスキンのアイコンを(再)設定.
///    ※ HICON の開放ってしなくてよい? 駄目でリークしてる?...winのリソース管理ってようわかんね....
void CMainFrame::initCurrentIcon()
{
	//+++ xp ビジュアルスタイルを一時的にoff
  #if 1	//+++ uxtheme.dll の関数の呼び出し方を変更.
	UxTheme_Wrap::SetWindowTheme(m_hWnd, L" ", L" ");
  #else
	CTheme		theme;
	theme.SetWindowTheme(m_hWnd, L" ", L" ");
  #endif

	/*static*/ HICON hIcon	= 0;
	//if (hIcon)
	//	::CloseHandle(hIcon);
	//hIcon 		= 0;
	CString strDir	= _GetSkinDir();
	m_strIcon		= strDir + _T("MainFrameBig.ico");
	if (Misc::IsExistFile(m_strIcon) == 0)
		m_strIcon	= strDir + _T("icon.ico");
	if (Misc::IsExistFile(m_strIcon))
		hIcon = (HICON)::LoadImage(ModuleHelper::GetResourceInstance(), m_strIcon, IMAGE_ICON, 32, 32, LR_SHARED|LR_LOADFROMFILE );
	if (hIcon == 0) {
		m_strIcon.Empty();
		hIcon = (HICON)::LoadImage(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	}
	if (hIcon)
		::SetClassLongPtr(m_hWnd, GCLP_HICON  , (LONG_PTR)hIcon );
	//::CloseHandle(hIcon);

	/*static*/ HICON hIconSm	= 0;
	//if (hIconSm)
	//	::CloseHandle(hIconSm);
	//hIconSm			= 0;
	m_strIconSm 		= strDir + _T("MainFrameSmall.ico");
	if (Misc::IsExistFile(m_strIconSm) == 0)
		m_strIconSm 	= strDir + _T("icon.ico");
	if (Misc::IsExistFile(m_strIconSm))
		hIconSm 	= (HICON)::LoadImage(ModuleHelper::GetResourceInstance(), m_strIconSm, IMAGE_ICON, 16, 16, LR_SHARED|LR_LOADFROMFILE);
	if (hIconSm == 0) {
		m_strIconSm.Empty();
		hIconSm   = (HICON)::LoadImage(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	}
	if (hIconSm)
		::SetClassLongPtr(m_hWnd, GCLP_HICONSM, (LONG_PTR)hIconSm );
	//::CloseHandle(hIconSm);

	//+++ XPのビジュアルスタイルに戻す...これはいらないかも
  #if 1	//+++ uxtheme.dll の関数の呼び出し方を変更.
	UxTheme_Wrap::SetWindowTheme(m_hWnd, 0, 0);
  #else
	theme.SetWindowTheme(m_hWnd, 0, 0);
  #endif
}


void	CMainFrame::_AnalyzeCommandLine(const CString& strCommandLine)
{
	// 検索バーを使って検索する
	if (strCommandLine.Left(13) == _T("SearchEngine:")) {
		std::wstring	strUrl2 = strCommandLine;
		std::wregex	rx(L"SearchEngine:\"(.+?)\" Keyword:\"(.+?)\"");
		std::wsmatch	rt;
		if (std::regex_search(strUrl2, rt, rx)) {
			CString strKeyword = rt.str(2).c_str();
			if (strKeyword.IsEmpty())
				return ;
			strKeyword.TrimLeft(_T(" \t\r\n　"));
			strKeyword.TrimRight(_T(" \t\r\n　"));
			TRACEIN(_T("コマンドラインからの検索:%s"), (LPCTSTR)strKeyword);
			CString strEngine  = rt.str(1).c_str();
			m_SearchBar.SearchWebWithEngine(strKeyword, strEngine);
		}
		return ;
	}

	vector<CString>	vecUrls;
	PerseUrls(strCommandLine, vecUrls);

	bool bActive = CMainOption::s_bExternalNewTabActive 
		|| m_ChildFrameClient.GetActiveChildFrameWindow() == NULL;

	vector<NewChildFrameData*>	vecpNewChildData;
	vecpNewChildData.reserve(vecUrls.size());
	int nCount = (int)vecUrls.size();
	for (int i = 0; i < nCount; ++i) {
		NewChildFrameData*	pNewChildData = new NewChildFrameData(m_ChildFrameClient);
		pNewChildData->strURL	= vecUrls[i];
		if (bActive && i == nCount - 1)
			pNewChildData->bActive	= true;	// 一番最後をアクティブに
		vecpNewChildData.push_back(pNewChildData);
	}
	// 次を指し示すようにする
	for (int i = 0; i < nCount - 1; ++i) {	
		vecpNewChildData[i]->pNext	= vecpNewChildData[i + 1];
	}
	// funcCallAfterCreated設定
	for (int i = 0; i < nCount; ++i) {
		NewChildFrameData*	pThis = vecpNewChildData[i];
		pThis->funcCallAfterCreated	= [pThis, this](CChildFrame* pChild) {
			if (pThis->pNext) {
				m_MDITab.SetInsertIndex(m_MDITab.GetItemCount());
				CChildFrame::AsyncCreate(*pThis->pNext);	// 次のChildFrameを作成
			} else {
				m_MDITab.InsertHere(false);
			}
			delete pThis;

		};
	}
	// 連鎖作成開始
	if (vecpNewChildData.size() > 0) {
		m_MDITab.InsertHere(true);
		m_MDITab.SetInsertIndex(m_MDITab.GetItemCount());
		CChildFrame::AsyncCreate(*vecpNewChildData[0]);
	}
}

void	CMainFrame::OpenBingTranslator(const CString& strText)
{
	CString strUrl;
	strUrl.Format(_T("http://www.microsofttranslator.com/?ref=JAIME&from=en&to=ja&text=%s"), strText);

	DWORD dwDLFlags = CDLControlOption::s_dwDLControlFlags & ~(DLCTL_NO_SCRIPTS | DLCTL_NO_RUNACTIVEXCTLS);
	UserOpenFile(strUrl, D_OPENFILE_CREATETAB | D_OPENFILE_ACTIVATE, dwDLFlags);
}

// ===========================================================================
// MainFrame_OpenFileから

bool CMainFrame::OnDDEOpenFile(const CString &strFileName)
{
	//dmfTRACE(_T("CMainFrame::OnDDEOpenFile(%s)\n"), strFileName);
  #if 1 //+++ トレイ状態からの復帰でのバグ対策.
	DWORD dwOpen = 0;
	if (CMainOption::s_bExternalNewTab) {
		dwOpen |= D_OPENFILE_CREATETAB;
		if (CMainOption::s_bExternalNewTabActive)
			dwOpen |= D_OPENFILE_ACTIVATE;
	} else {	// 既存のタブをナビゲートする
		dwOpen |= D_OPENFILE_NOCREATE;
		if (!CStartUpOption::s_dwActivate)
			dwOpen |= D_OPENFILE_NOSETFOCUS;
	}

	UserOpenFile( strFileName, dwOpen );
	if (CStartUpOption::s_dwActivate) {
		IfTrayRestoreWindow();							//+++ トレイ状態だったら復活.
		if (IsZoomed() == FALSE)
			ShowWindow_Restore(true);
		MtlSetForegroundWindow(m_hWnd);
	}
  #else
	OnUserOpenFile( strFileName, DonutGetStdOpenFlag() );
	if (CStartUpOption::s_dwActivate) {
		//+++ OnUserOpenFile()の時点で最大化/フルスクリーンだった場合の対策でnCmdShowを渡して考慮するように変更.
		MtlSetForegroundWindow(m_hWnd, m_OnUserOpenFile_nCmdShow);	//+++ 実はこのnCmdShowは意味が違うものを持ってきてるような気がする...があとまわし.
	}
  #endif
	// UDT DGSTR ( added by dai
	return true;
}




//public:
// Message handlers
// alternates OpenFile

#if 0	//+++ url別拡張プロパティの処理を追加
HWND CMainFrame::OnUserOpenFile(const CString& strUrl, DWORD dwOpenFlag)
{
#if 0
  #if 0	//+++ CChildFrame::OnBeforeNavigate2側で処理するように変更したので、これでok ...としたい...
	return UserOpenFile(strUrl, dwOpenFlag);
  #else	//+++ CChildFrame::OnBeforeNavigate2側で処理するようにしたので、2重に検索がかかってしまうが、
		//+++ オプション反映のタイミングの都合なるべく早くフラグを設定したほうがよさそうなので、速度犠牲にするなら、こちら

	CString		str(strUrl);
	MtlRemoveStringHeaderAndFooter(str);

	// 検索バーを使って検索する
	if (str.Left(13) == _T("SearchEngine:")) {
		std::wstring	strUrl2 = str;
		std::wregex	rx(L"SearchEngine:\"(.+?)\" Keyword:\"(.*?)\"");
		std::wsmatch	rt;
		if (std::regex_search(strUrl2, rt, rx)) {
			CString strKeyword = rt.str(2).c_str();
			if (strKeyword.IsEmpty())
				return NULL;
			strKeyword.TrimLeft(_T(" \t\r\n　"));
			strKeyword.TrimRight(_T(" \t\r\n　"));
			TRACEIN(_T("コマンドラインからの検索:%s"), (LPCTSTR)strKeyword);
			CString strEngine  = rt.str(1).c_str();
			m_SearchBar.SearchWebWithEngine(strKeyword, strEngine);
			return NULL;
		}
		return NULL;
	}

   #if 0	//+++ 仕様と思ったが、.url側の拡張プロパティの件があるので、ここではできない
	if ( MtlIsExt( str, _T(".url") ) ) {
		if (MTL::ParseInternetShortcutFile(str) == 0)
			return NULL;
	}
   #endif

	DWORD dwExProp	 = 0xFFFFFFFF;
	DWORD dwExProp2	 = 8;
	if (   (_check_flag(D_OPENFILE_NOCREATE, dwOpenFlag) == 0 || CUrlSecurityOption::activePageToo())
		&& CUrlSecurityOption::FindUrl(str, &dwExProp, &dwExProp2, 0)
	) {
		return OpenUrlWithExProp(str, dwOpenFlag, dwExProp, dwExProp2);
	} else {
		return UserOpenFile(str, dwOpenFlag);
	}
  #endif
#endif
	return nullptr;
}
#endif



//+++ url別拡張プロパティ対応で、本来のOnUserOpenFileをUserOpenFileに変名. 引数を末に追加.
/// @param openFlag		DonutOpenFileFlags
HWND CMainFrame::UserOpenFile(CString strFileOrURL, 
							  DWORD openFlag /*= DonutGetStdOpenFlag()*/, 
							  int dlCtrlFlag /*= -1*/, 
							  int extededStyleFlags /*= -1*/,
							  int AutoRefresh /*= 0*/)
{
	MtlRemoveStringHeaderAndFooter(strFileOrURL);
#pragma region ~.url open
	//+++ メモ:urlのときの処理. 拡張プロパティの取得とかあるので、専用のopen処理へ...
	if ( MtlIsExt( strFileOrURL, _T(".url") ) ) {	
		// OpenInternetShortcut
		CString strUrl = strFileOrURL;
		if ( !MTL::ParseInternetShortcutFile(strUrl) )	// ~.url -> http://~
			return NULL;

		CChildFrame *pActiveChild = GetActiveChildFrame();

		bool	bJavascript = false;
		if (strUrl.Left(11).CompareNoCase(_T("javascript:")) == 0) {
			bJavascript = true;
			openFlag |= D_OPENFILE_NOCREATE;
		}

		NewChildFrameData	data(m_ChildFrameClient);
		data.strURL		= strUrl;
		DWORD dwExProp = 0xAAAAAA;		//+++ 初期値変更
		DWORD dwExProp2= 0x8;			//+++ 拡張プロパティを増設.
		if (CExProperty::CheckExPropertyFlag(dwExProp, dwExProp2, strFileOrURL)) {
			CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, dwExProp, dwExProp2);
			data.dwDLCtrl	= ExProp.GetDLControlFlags();
			data.dwExStyle	= ExProp.GetExtendedStyleFlags();
			data.dwAutoRefresh = ExProp.GetAutoRefreshFlag();
		}

		if (pActiveChild && _check_flag(D_OPENFILE_NOCREATE, openFlag)) {
			// 既存のタブをナビゲート
			if (data.dwDLCtrl != -1)
				pActiveChild->SetMarshalDLCtrl(data.dwDLCtrl);
			if (data.dwExStyle != -1)
				pActiveChild->SetExStyle(data.dwExStyle);

			if (bJavascript) {
				::PostMessage(pActiveChild->GetHwnd(), WM_EXECUTEUSERJAVASCRIPT, (WPARAM)(LPCTSTR)new CString(strUrl), 0);
			} else {
				pActiveChild->Navigate2(data.strURL);
			}
			if (data.dwAutoRefresh)
				pActiveChild->SetAutoRefreshStyle(data.dwAutoRefresh);
		} else if (bJavascript) {	// javascript: かつ pActiveChildがないのでなにもしない
			return NULL;
		} else {
			// 新規タブを作成する
			data.bActive	= _check_flag(openFlag, D_OPENFILE_ACTIVATE) 
				|| m_ChildFrameClient.GetActiveChildFrameWindow() == NULL;
			CChildFrame::AsyncCreate(data);
		}
		return NULL;
	}
#pragma endregion

	if (  !MtlIsProtocol( strFileOrURL, _T("http") )
	   && !MtlIsProtocol( strFileOrURL, _T("https") ) )
	{
		if ( MtlPreOpenFile(strFileOrURL) )
			return NULL;	// handled
	  #if 1	//+++	タブがあるとき
		if (m_MDITab.GetItemCount() > 0) {
			if (strFileOrURL.Find(':') < 0 && strFileOrURL.Left(1) != _T("/") && strFileOrURL.Left(1) != _T("\\")) {	// すでに具体的なパスでないとき、
				HWND			hWndChild 	= m_MDITab.GetTabHwnd(m_MDITab.GetCurSel());
				CChildFrame*	pChild 		= GetChildFrame(hWndChild);
				if (pChild) {		//+++ アクティブページをみて
					CString strBase  = pChild->GetLocationURL();
					CString str7     = strBase.Left(7);
					if (str7 == "file://") {	//+++ file://だったらば、ローカルでのディレクトリ移動だろうとみなして
						strFileOrURL = Misc::MakeFullPath( strBase, strFileOrURL );	//+++ フルパス化
					}
				}
			}
		}
	  #endif
	}

	if (strFileOrURL.GetLength() > INTERNET_MAX_PATH_LENGTH)
		return NULL;

	// "javascript:"ならウィンドウを作成しないフラグを追加する
	if ( strFileOrURL.Left(11).CompareNoCase(_T("javascript:")) == 0 ) {
		if ( strFileOrURL == _T("javascript:location.reload()") )
			return NULL;
		openFlag |= D_OPENFILE_NOCREATE;
	}

	// dfg files
	if ( MtlIsExt( strFileOrURL, _T(".dfg") ) && ::PathFileExists(strFileOrURL)) {
		_LoadGroupOption(strFileOrURL, !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOCLOSEDFG));
		return NULL;
	}
	if ( MtlIsExt( strFileOrURL, _T(".xml") ) && ::PathFileExists(strFileOrURL) ) {
		RestoreAllTab(strFileOrURL, !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOCLOSEDFG));
		return NULL;
	}

	//+++	サイタマ封印(about:blankの不具合修正で誤ってコメントアウト...
	//+++	だったがunDonut+より、ちゃんと機能していないようだし?、わざわざ復活する必要もないかな、と)
	//+++   ちょっと気が変わったので条件付の暫定復活
	// minit(about:* pages)
	if ( strFileOrURL.Left(6) == _T("about:") && strFileOrURL != _T("about:blank") ) {
		if (_OpenAboutFile(strFileOrURL))
			return NULL;
	}

	HWND	hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();

	/* openFlag が D_OPENFILE_NOCREATE ならアクティブなページを移動する */
	if ( hWndActive != NULL && _check_flag(D_OPENFILE_NOCREATE, openFlag) ) {
		CChildFrame* pChild  = GetActiveChildFrame();
		if (strFileOrURL.Left(11).CompareNoCase(_T("javascript:")) == 0) {
			::PostMessage(pChild->GetHwnd(), WM_EXECUTEUSERJAVASCRIPT, (WPARAM)(LPCTSTR)new CString(strFileOrURL), 0);
		} else {
			if (dlCtrlFlag != -1)
				pChild->SetDLCtrl(dlCtrlFlag);
			if (extededStyleFlags != -1)
				pChild->SetExStyle(extededStyleFlags);
			if (AutoRefresh != 0)
				pChild->SetAutoRefreshStyle(AutoRefresh);
			pChild->Navigate2(strFileOrURL);
		}

		if ( !_check_flag(D_OPENFILE_NOSETFOCUS, openFlag) ) {
			// reset focus
			::SetFocus(NULL);
			MtlSendCommand(hWndActive, ID_VIEW_SETFOCUS);
		}
		return NULL;
	}

	/* 新規ChildFrame作成 */
	NewChildFrameData	data(m_ChildFrameClient);
	data.strURL		= strFileOrURL;
	data.dwDLCtrl	= dlCtrlFlag;
	data.dwExStyle	= extededStyleFlags;
	data.dwAutoRefresh	= AutoRefresh;
	data.bActive	= _check_flag(openFlag, D_OPENFILE_ACTIVATE) 
		|| m_ChildFrameClient.GetActiveChildFrameWindow() == NULL;
	CChildFrame::AsyncCreate(data);
	return NULL;
#if 0	//:::
	/* 新規ウィンドウ作成 */
	{
		CChildFrame* pChild = CChildFrame::NewWindow(m_hWndMDIClient, m_MDITab, m_AddressBar, false/*true*/, dlCtrlFlag, extededStyleFlags);
		if (pChild == NULL)
			return NULL;

		if ( strFile.IsEmpty() == FALSE ) {
			pChild->SetWaitBeforeNavigate2Flag();			//+++ 無理やり、BeforeNavigate2()が実行されるまでの間アドレスバーを更新しないようにするフラグをon... これもう不要だろうが...
			m_OnUserOpenFile_nCmdShow = pChild->ActivateFrame(nCmdShow);	//\\ タブに追加

			const SearchPostData& PostData = m_SearchBar.GetSearchPostData();
			if (PostData.pPostData) {	// POSTする
				pChild->Navigate2(strFile, 0, NULL, 
					_T("Content-Type: application/x-www-form-urlencoded"), PostData.pPostData, PostData.nPostBytes);
			} else {
				pChild->Navigate2(strFile);
			}
		} else {
			m_OnUserOpenFile_nCmdShow = pChild->ActivateFrame(nCmdShow);	//\\ タブに追加
			TRACEIN(_T("UserOpenFile() : strFile.IsEmpty()"));
		}

		if ( !_check_flag(D_OPENFILE_NOSETFOCUS, openFlag) ) {
			if (m_ChildFrameClient.GetActiveChildFrameWindow() == pChild->m_hWnd) { // a new window activated, so automatically set focus
				// reset focus
				::SetFocus(NULL);
				MtlSendCommand(pChild->m_hWnd, ID_VIEW_SETFOCUS);
			} else {
				// It's reasonable not to touch a current focus.
			}
		}

		return pChild->m_hWnd;
	}
#endif
}


/// 検索バーから呼ばれる 検索したときに飛んでくる関数
LRESULT CMainFrame::OnOpenWithExProp(_EXPROP_ARGS *pArgs)
{
	if (pArgs == NULL) 
		return 0;

	CChildFrame *pActiveChild = GetActiveChildFrame();
	if( pActiveChild)
		pActiveChild->SaveSearchWordflg(false); //\\ 検索バーで検索したときアクティブなタブの検索文字列を保存しないようにする
	
	bool bOldSaveFlag = CSearchBarOption::s_bSaveSearchWord;	// 検索バーの文字列が消える件に一応の対処
	CSearchBarOption::s_bSaveSearchWord = false;				// 本当はこんなことしちゃダメ

	NewChildFrameData	data(m_ChildFrameClient);
	data.strURL		= pArgs->strUrl;
	DWORD dwExProp = 0xAAAAAA;		//+++ 初期値変更
	DWORD dwExProp2= 0x8;			//+++ 拡張プロパティを増設.
	if (CExProperty::CheckExPropertyFlag(dwExProp, dwExProp2, pArgs->strIniFile, pArgs->strSection)) {
		CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, dwExProp, dwExProp2);
		data.dwDLCtrl	= ExProp.GetDLControlFlags();
		data.dwExStyle	= ExProp.GetExtendedStyleFlags();
		data.dwAutoRefresh = ExProp.GetAutoRefreshFlag();
	}

	if (pActiveChild && _check_flag(D_OPENFILE_NOCREATE, pArgs->dwOpenFlag)) {
		// 既存のタブをナビゲート
		if (data.dwDLCtrl != -1)
			pActiveChild->SetMarshalDLCtrl(data.dwDLCtrl);
		if (data.dwExStyle != -1)
			pActiveChild->SetExStyle(data.dwExStyle);
		pActiveChild->Navigate2(data.strURL);
		if (data.dwAutoRefresh)
			pActiveChild->SetAutoRefreshStyle(data.dwAutoRefresh);
	} else {
		// 新規タブを作成する
		CString str = pArgs->strSearchWord;
		data.funcCallAfterCreated = [str, bOldSaveFlag](CChildFrame* pChild) {
			CSearchBarOption::s_bSaveSearchWord = bOldSaveFlag;
	  		//+++ 子窓に検索設定を反映 (関数化)
			if (CSearchBarOption::s_bAutoHilight)
				pChild->SetSearchWordAutoHilight(str, true);
		};
		data.bActive	= !_check_flag(CMainOption::s_dwMainExtendedStyle, MAIN_EX_NOACTIVATE);
		CChildFrame::AsyncCreate(data);
	}

	return 0;
}


#if 0	//:::
HWND CMainFrame::OpenUrlWithExProp(CString strUrl, DWORD dwOpenFlag, DWORD dwExProp, DWORD dwExProp2)
{
	if ( _check_flag(D_OPENFILE_NOCREATE, dwOpenFlag) ) {
		return OpenExPropertyActive(strUrl, dwExProp, dwExProp2, dwOpenFlag);		//既存のタブで開く
	} else {
		return OpenExPropertyNew(strUrl, dwExProp, dwExProp2, dwOpenFlag); 		//新規に開く
	}
}


HWND CMainFrame::OpenUrlWithExProp(CString strUrl, DWORD dwOpenFlag, CString strIniFile, CString strSection /*= DONUT_SECTION*/)
{
	DWORD dwExProp = 0xAAAAAA;		//+++ 初期値変更
	DWORD dwExProp2= 0x8;			//+++ 拡張プロパティを増設.

	if ( CExProperty::CheckExPropertyFlag(dwExProp, dwExProp2, strIniFile, strSection) ) {
		if ( _check_flag(D_OPENFILE_NOCREATE, dwOpenFlag) ) {
			return OpenExPropertyActive(strUrl, dwExProp, dwExProp2, dwOpenFlag);	//既存のタブで開く
		} else {
			return OpenExPropertyNew(strUrl, dwExProp, dwExProp2, dwOpenFlag); 	//新規に開く
		}
	} else {
	  #if 1	//+++	URL別拡張プロパティのチェック＆処理.
			//		...だったが、CChildFrame::OnBeforeNavigate2側で処理するように変更したので、ここはなし
			//		...にしたかったっが、どうも、dlオプション反映のタイミングにラグがあるようなんで、
			//		なるべく早く反映されるように、速度犠牲で反映.
		if (   (_check_flag(D_OPENFILE_NOCREATE, dwOpenFlag) == 0 || CUrlSecurityOption::activePageToo())
			&& CUrlSecurityOption::FindUrl(strUrl, &dwExProp, &dwExProp2, 0)
		) {
			return OpenUrlWithExProp(strUrl, dwOpenFlag, dwExProp, dwExProp2);
		}
	  #endif
		return OpenExPropertyNot(strUrl, dwOpenFlag);					//標準オプションで開く
	}
}


//既存タブにナビゲートしたのちに拡張設定を適用する.  //+++ dwExProp2増設.
HWND CMainFrame::OpenExPropertyActive(CString &strUrl, DWORD dwExProp, DWORD dwExProp2, DWORD dwOpenFlag)
{
	dwOpenFlag |= D_OPENFILE_NOCREATE;

	//アクティブなタブがナビゲートロックされているかを確認
	HWND	 hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
	if ( hWndActive && ::IsWindow(hWndActive) ) {
		CChildFrame *pChild = GetChildFrame(hWndActive);
		if (pChild) {
			DWORD dwExFlag = pChild->GetExStyle();
			if (dwExFlag & DVS_EX_OPENNEWWIN)
				return OpenExPropertyNew(strUrl, dwExProp, dwExProp2, dwOpenFlag);
		}
	}

	//+++ (dwExProp2増設でUserOpenFileの後にあったのを前に移動.)
	CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, dwExProp, dwExProp2);

	//取得したURLをアクティブなタブで開かせる（標準処理に任せる）
	BOOL		 	bOpened		= FALSE;
	HWND 			hWndNew		= UserOpenFile(strUrl, dwOpenFlag, ExProp.GetDLControlFlags(), ExProp.GetExtendedStyleFlags() );

	if (hWndNew && !hWndActive) {
		hWndActive = hWndNew;											//ウィンドウが無かったので新規に開いた
		bOpened    = TRUE;
	}
#if 0	//:::
	//拡張プロパティを適用する
	if (hWndActive == NULL)
		return NULL;
	CChildFrame*	pChild 	= GetChildFrame(hWndActive);
	if (!pChild)
		return NULL;

	pChild->view().PutDLControlFlags( ExProp.GetDLControlFlags() );
	pChild->SetViewExStyle(ExProp.GetExtendedStyleFlags(), TRUE);
	pChild->view().m_ViewOption.SetAutoRefreshStyle( ExProp.GetAutoRefreshFlag() );

	if (bOpened)
		return hWndActive;
#endif
	return NULL;
}


//新規タブを開いたのち拡張設定を適用する
HWND CMainFrame::OpenExPropertyNew(CString &strUrl, DWORD dwExProp, DWORD dwExProp2, DWORD dwOpenFlag)
{
	dwOpenFlag &= ~D_OPENFILE_NOCREATE;

	CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, dwExProp, dwExProp2);
	int			 dlCtrlFlag        = ExProp.GetDLControlFlags();
	int			 extendedStyleFlag = ExProp.GetExtendedStyleFlags();
	//URLで新規タブを開く
	HWND 			hWndNew		= UserOpenFile(strUrl, dwOpenFlag, dlCtrlFlag, extendedStyleFlag);
	if ( hWndNew == 0 || !::IsWindow(hWndNew) )
		return NULL;
#if	0 //:::
	//拡張プロパティを適用する
	CChildFrame *pChild  = GetChildFrame(hWndNew);
	if (!pChild)
		return NULL;

	pChild->view().PutDLControlFlags( dlCtrlFlag );
	pChild->SetViewExStyle(ExProp.GetExtendedStyleFlags(), TRUE);
	pChild->view().m_ViewOption.SetAutoRefreshStyle( ExProp.GetAutoRefreshFlag() );

	return hWndNew;
#endif
	return NULL;
}


//タブを開いたのち、標準の設定を適用する
HWND CMainFrame::OpenExPropertyNot(CString &strUrl, DWORD dwOpenFlag)
{
#if 0	//:::
	//アクティブなタブがナビゲートロックされているかを確認
	HWND hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();

	if ( ::IsWindow(hWndActive) ) {
		CChildFrame *pChild = GetChildFrame(hWndActive);

		if (pChild) {
			DWORD dwExFlag = pChild->view().m_ViewOption.m_dwExStyle;

			if (dwExFlag & DVS_EX_OPENNEWWIN)
				dwOpenFlag &= ~D_OPENFILE_NOCREATE; 			//新規タブで開くように
		}
	}

	//取得したURLを開かせる
	HWND 			hWndNew		= UserOpenFile(strUrl, dwOpenFlag);
	if (hWndNew) {
		//新規に開いたタブなので何もしなくてよい
		return hWndNew;
	} else {
		//オプションを標準のもので上書き
		CChildFrame *pChild = GetChildFrame( m_ChildFrameClient.GetActiveChildFrameWindow() );

		if (!pChild)
			return NULL;

		pChild->view().PutDLControlFlags(CDLControlOption::s_dwDLControlFlags);
		pChild->SetViewExStyle(CDLControlOption::s_dwExtendedStyleFlags, TRUE);
		pChild->view().m_ViewOption.SetAutoRefreshStyle(0);
	}
#endif
	return NULL;
}
#endif

/// 最近閉じたタブを開く
LRESULT CMainFrame::OnFileRecent(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	// get file name from the MRU list
	if (ID_FILE_MRU_FIRST <= wID && wID <= ID_FILE_MRU_LAST)						//旧範囲IDから新範囲IDへ変換
		wID = wID - ID_FILE_MRU_FIRST + ID_RECENTDOCUMENT_FIRST;

	ChildFrameDataOnClose*	pdata = nullptr;
	if ( m_RecentClosedTabList.GetFromList(wID, &pdata) ) {
		NewChildFrameData	data(m_ChildFrameClient);
		data.strURL		= pdata->strURL;
		data.dwDLCtrl	= pdata->dwDLCtrl;
		data.bActive	= _check_flag(DonutGetStdOpenCreateFlag(), D_OPENFILE_ACTIVATE);	// Force New Window
		typedef vector<std::pair<CString, CString> > tlog;
		tlog* pvecBack = new tlog(pdata->TravelLogBack);
		tlog* pvecFore = new tlog(pdata->TravelLogFore);
		data.funcCallAfterCreated	= [pvecBack, pvecFore, this](CChildFrame* pChild) {
			if (CMainOption::s_bTravelLogClose) 
				pChild->SetTravelLog(*pvecFore, *pvecBack);
			delete pvecBack;
			delete pvecFore;
		};
		m_RecentClosedTabList.RemoveFromList(wID);
		CChildFrame::AsyncCreate(data);

	} else {
		::MessageBeep(MB_ICONERROR);
	}

	return 0;
}



//+++ メモ：複数起動無しで、別のプログラムから引数付きで起動された場合に、ここにくる。
//+++       他の外部からの追加表示はDDE経由で行われるようで、こちらにくることはなさそう...?
void CMainFrame::OnNewInstance(ATOM nAtom)			// WM_NEWINSTANCE
{
	enum { NAME_LEN = 0x4000 };
	TCHAR szBuff[NAME_LEN+2] = _T("\0");
	bool	bActive = !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE);

	if (::GlobalGetAtomName(nAtom, szBuff, NAME_LEN) != 0) {
		//\\ 1行ずつ渡す方式に変えたので
		CString strPath = szBuff;
		::GlobalDeleteAtom(nAtom);
		if (strPath.CompareNoCase(_T("-tray")) == 0 || strPath.CompareNoCase(_T("/tray")) == 0)
			bActive = false;

		// 検索バーを使って検索する
		if (strPath.Left(13) == _T("SearchEngine:")) {
			std::wstring	strUrl2 = strPath;
			std::wregex	rx(L"SearchEngine:\"(.+?)\" Keyword:\"(.*?)\"");
			std::wsmatch	rt;
			if (std::regex_search(strUrl2, rt, rx)) {
				CString strKeyword = rt.str(2).c_str();
				if (strKeyword.IsEmpty())
					return ;
				strKeyword.TrimLeft(_T(" \t\r\n　"));
				strKeyword.TrimRight(_T(" \t\r\n　"));
				TRACEIN(_T("コマンドラインからの検索:%s"), (LPCTSTR)strKeyword);
				CString strEngine  = rt.str(1).c_str();
				m_SearchBar.SearchWebWithEngine(strKeyword, strEngine);
			} else
				return ;
		} else {
			UserOpenFile( strPath, DonutGetStdOpenFlag() );
		}
	}

	if ( bActive ) {
		IfTrayRestoreWindow();									//+++ トレイ状態だったら復活.
		if (IsZoomed() == FALSE)
			ShowWindow(SW_RESTORE);
		MtlSetForegroundWindow(m_hWnd); 						//ウインドウをアクティブにする
		//if (m_bOldMaximized == 0 && m_bFullScreen == 0) 		//+++
		//	ShowWindow_Restore(1);	//ShowWindow(SW_RESTORE);	//+++ サイズを戻す.
	}
}



////////////////////////////////////////////////////////////////////////////////
//ファイルメニュー
//　コマンドハンドラ
////////////////////////////////////////////////////////////////////////////////
////新規作成

//ポームページ
void CMainFrame::OnFileNewHome(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	NewChildFrameData	data(m_ChildFrameClient);
	data.strURL		= _T("about:blank");
	data.bActive	= _check_flag(DonutGetStdOpenActivateFlag(), D_OPENFILE_ACTIVATE);
	data.funcCallAfterCreated	= [this](CChildFrame* pChild) {
		pChild->GetIWebBrowser()->GoHome();
		if (m_strCommandLine.IsEmpty() == FALSE)
			this->PostMessage(WM_INITPROCESSFINISHED);
	};
	CChildFrame::AsyncCreate(data);
}


/// 現在のページ
void CMainFrame::OnFileNewCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CChildFrame* pChild = GetActiveChildFrame();
	if (pChild) 
		UserOpenFile(pChild->GetLocationURL(), DonutGetStdOpenActivateFlag());
}

// オプションで指定した既定の新規タブ作成
LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	if (CFileNewOption::s_dwFlags == FILENEW_BLANK) {
		OnFileNewBlank(0, 0, 0);
	} else if (CFileNewOption::s_dwFlags == FILENEW_COPY) {
		if (m_ChildFrameClient.GetActiveChildFrameWindow() != NULL)
			OnFileNewCopy(0, 0, 0);
		else
			OnFileNewBlank(0, 0, 0);
	} else if (CFileNewOption::s_dwFlags == FILENEW_HOME) {
		OnFileNewHome(0, 0, 0);
	} else if (CFileNewOption::s_dwFlags == FILENEW_USER) {		//+++ ユーザー指定のページを開く
		//CIniFileI 	pr( g_szIniFileName, _T("Main") );
		//CString		str = pr.GetStringUW( _T("File_New_UsrPage") );
		//pr.Close();
		CString&	str	= CFileNewOption::s_strUsr;
		if ( !str.IsEmpty() )
			UserOpenFile(str, DonutGetStdOpenActivateFlag());
		else
			OnFileNewBlank(0, 0, 0);
	} else {
		ATLASSERT(FALSE);
		OnFileNewBlank(0, 0, 0);
	}

	return 0;
}


/// 空白ページ
void CMainFrame::OnFileNewBlank(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	UserOpenFile( _T("about:blank"), DonutGetStdOpenActivateFlag() );
	PostMessage(WM_COMMAND, ID_SETFOCUS_ADDRESSBAR, 0);
}


/// クリップボード
void CMainFrame::OnFileNewClipBoard(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CString strText = MtlGetClipboardText();
	if ( strText.IsEmpty() )
		return;

	UserOpenFile( strText, DonutGetStdOpenActivateFlag() );
}

/// TabList.xmlを開く
LRESULT	CMainFrame::OnFileOpenTabList	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CString strPath = Misc::GetExeDirectory() + _T("TabList.xml");
	UserOpenFile( strPath, DonutGetStdOpenCreateFlag() );
	return 0;
}

/// クリップボードから開く(標準のオープン方法で)
void CMainFrame::OnFileNewClipBoard2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CString strText = MtlGetClipboardText();
	if ( strText.IsEmpty() )
		return;

	UserOpenFile( strText, DonutGetStdOpenFlag() );	// allow the option
}


/// 開く
LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	COpenURLDlg dlg;

	if ( dlg.DoModal() == IDOK && !dlg.m_strEdit.IsEmpty() ) {
		UserOpenFile( dlg.m_strEdit, DonutGetStdOpenFlag() );
	}

	return 0;
}



/// undonut の配布サイトを開く
LRESULT CMainFrame::OnJumpToWebSite(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	//CString strSite = _T("http://www5.ocn.ne.jp/~minute/tab/"); //unDonut古い方
	//CString strSite = _T("http://undonut.sakura.ne.jp/"); // unDonut新しい方
	//CString strSite = _T("http://tekito.genin.jp/undonut+.html"); //unDonut+
	//CString strSite = _T("http://ichounonakano.sakura.ne.jp/64/undonut/"); //unDonut+mod.	旧
	//CString strSite = _T("http://undonut.undo.jp/"); //unDonut+mod.
	CString strSite = _T("http://cid-8830de058eedff85.skydrive.live.com/browse.aspx/%e5%85%ac%e9%96%8b/unDonut"); // amate.

	UserOpenFile( strSite, DonutGetStdOpenFlag() );
	return 0;
}


///+++ .exeのあるフォルダを開く
LRESULT CMainFrame::OnOpenExeDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CString		progDir = Misc::GetExeDirectory();
	ShellExecute(NULL, _T("open"), progDir, progDir, NULL, SW_SHOW);
	return 0;
}


/// unDonutのあるフォルダ\\help\\about\\ 以下にあるabout:「～」ファイルを開く
///+++ saitama専用だったのを外部ファイル読込にして汎用化.
bool CMainFrame::_OpenAboutFile(CString strFile)
{
	if (strFile.Left(6) != _T("about:"))
		return false;
	strFile     = strFile.Mid(6);		// about:"～"
	bool  bWarn = (strFile == "warning");
	if (bWarn == false) {
		strFile = Misc::GetFullPath_ForExe( "help\\about\\" + strFile );	//
		if (::PathFileExists(strFile) == FALSE)	//ファイルがなかったらかえる
			return false;
	}

	NewChildFrameData	data(m_ChildFrameClient);
	data.strURL		= _T("about:blank");
	data.funcCallAfterCreated	= [=](CChildFrame* pChild) {
		CComPtr<IHTMLDocument2> pDoc;
		HRESULT hr;
		int ct = 0;
		while (true) {
			hr = pChild->GetIWebBrowser()->get_Document( (IDispatch **) &pDoc );
			if (SUCCEEDED(hr) && pDoc) {
				break;
			}
			Sleep(100);

			ct++;
			if (ct > 50)
				return ;
		}

		TRACEIN(strFile);
		const char*			pText = NULL;
		unsigned			nSize = 0;
		std::vector<char>	vecText;
		if (bWarn) {	//+++ 警告のときは、りそーすから読む
			HRSRC	hRes = ::FindResource(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_TEXT_WARNING/*IDR_TEXT_SAITAMA*/), _T("TEXT") );
			HGLOBAL hMem = ::LoadResource(_Module.GetModuleInstance(), hRes);
			pText 		 = (LPSTR) ::LockResource(hMem);
			nSize		 = (UINT)strlen(pText);
		} else {		//+++ 通常は help/about/下のファイルを読み込む
			Misc::FileLoad( strFile, vecText );
			pText  = vecText.data();
			nSize  = (UINT)vecText.size();
		}

		HGLOBAL hHTMLText = ::GlobalAlloc(GPTR, (nSize + 16/*気持ち*/) );
		if (hHTMLText) {
			::ZeroMemory(hHTMLText, nSize + 16 );
			::CopyMemory(hHTMLText, pText, nSize);
			{
				CComPtr<IStream> pStream;
				hr = ::CreateStreamOnHGlobal(hHTMLText, TRUE, &pStream);
				if ( SUCCEEDED(hr) ) {
					// Call the helper function to load the browser from the stream.
					CComPtr<IDispatch> 		pHtmlDoc;
					// Retrieve the document object.
					HRESULT hr = pChild->GetIWebBrowser()->get_Document( &pHtmlDoc );
					if ( SUCCEEDED(hr) ) {
						// Query for IPersistStreamInit.
						CComQIPtr<IPersistStreamInit> pPersistStreamInit = pHtmlDoc;
						if ( pPersistStreamInit.p ) {
							// Initialize the document.
							hr = pPersistStreamInit->InitNew();
							if ( SUCCEEDED(hr) ) {
								// Load the contents of the stream.
								hr = pPersistStreamInit->Load( pStream );
							}
						}
					}
				}
			}
		}
	};	// lamda
	CChildFrame::AsyncCreate(data);
	return true;
}



// =============================================================================================




struct _Function_Enum_ChildInfomation {
	CMainFrame*					m_pMainFrame;
	HWND						m_hWndActive;
	std::list<SDfgSaveInfo>&	m_rSaveInfoList;
	int 						m_nIndex;
	bool						m_bSaveFB;
	int 						m_nActiveIndex;

	// コンストラクタ
	_Function_Enum_ChildInfomation(
			CMainFrame*					pMainFrame,
			HWND						hWndActive,
			std::list<SDfgSaveInfo>&	rSaveInfoList,
			int 						nIndex		= 0,
			bool						bSaveFB 	= FALSE)
		:	m_pMainFrame(pMainFrame)
		,	m_hWndActive(hWndActive)
		,	m_rSaveInfoList(rSaveInfoList)
		,	m_nIndex(nIndex)
		,	m_bSaveFB(bSaveFB)
		,	m_nActiveIndex(-1)
	{
	}

	void operator ()(HWND hWnd)
	{
		if (hWnd == m_hWndActive)
			m_nActiveIndex = m_nIndex;

		//3つ以上の引数を送るのが面倒なので直にCChildFrameを操作
		CChildFrame *pChild = m_pMainFrame->GetChildFrame(hWnd);
		if (!pChild) {
			ErrorLogPrintf(_T("dfgのセーブでの情報取得に失敗\n"));
			return;
		}

		try {	//+++ 念のため例外チェック.
			m_rSaveInfoList.push_back( SDfgSaveInfo() );
			//:::pChild->view().m_ViewOption.GetDfgSaveInfo( m_rSaveInfoList.back(), m_bSaveFB );
		} catch(...) {
			ErrorLogPrintf(_T("dfgのセーブ中に例外発生\n"));
		}
		++m_nIndex;
	}
};


void	CMainFrame::SaveAllTab()
{
	m_bNowSaveAllTab = true;

	HWND hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
	int	nCount = 0;
	int nActiveIndex = -1;
	vector<unique_ptr<ChildFrameDataOnClose> >	vecpSaveData;
	auto CollectChildFrameData = [&](HWND hWnd) {
		ChildFrameDataOnClose* pData = new ChildFrameDataOnClose;
		::SendMessage(hWnd, WM_GETCHILDFRAMEDATA, (WPARAM)pData, 0);
		vecpSaveData.push_back(unique_ptr<ChildFrameDataOnClose>(std::move(pData)));
		if (hWnd == hWndActive)
			nActiveIndex = nCount;
		++nCount;
	};
	m_MDITab.ForEachWindow(CollectChildFrameData);

	try {
		using boost::property_tree::wptree;

		auto AddTravelLog = [](wptree& ptLog, const vector<std::pair<CString, CString> >& vecTravelLog) {
			for (auto it = vecTravelLog.cbegin(); it != vecTravelLog.cend(); ++it) {
				wptree& ptItem = ptLog.add(L"item", L"");
				ptItem.put(L"<xmlattr>.title", (LPCTSTR)it->first);
				ptItem.put(L"<xmlattr>.url"	 , (LPCTSTR)it->second);
			}
		};
		wptree	pt;
		wptree&	ptTabList = pt.add(L"TabList", L"");
		ptTabList.add(L"<xmlattr>.ActiveIndex", nActiveIndex);
		for (auto it = vecpSaveData.cbegin(); it != vecpSaveData.cend(); ++it) {
			ChildFrameDataOnClose& data = *(*it);
			wptree& ptItem = ptTabList.add(L"Tab", L"");
			ptItem.put(L"<xmlattr>.title", (LPCTSTR)data.strTitle);
			ptItem.put(L"<xmlattr>.url"	 , (LPCTSTR)data.strURL);
			ptItem.put(L"<xmlattr>.DLCtrlFlags", data.dwDLCtrl);
			ptItem.put(L"<xmlattr>.ExStyle",	data.dwExStyle);
			ptItem.put(L"<xmlattr>.AutoRefreshStyle", data.dwAutoRefreshStyle);
			AddTravelLog(ptItem.add(L"TravelLog.Back", L""), data.TravelLogBack);
			AddTravelLog(ptItem.add(L"TravelLog.Fore", L""), data.TravelLogFore);
		}
		using namespace boost::property_tree::xml_parser;

		CString strTempTabList = Misc::GetExeDirectory() + _T("TabList.temp.xml");
		std::wofstream filestream(strTempTabList);
		if (!filestream)
			throw "error";
		filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));
		write_xml(filestream, pt, xml_writer_make_settings(L' ', 2, widen<wchar_t>("UTF-8")));	
		CString	TabList = Misc::GetExeDirectory() + _T("TabList.xml");
		if (::PathFileExists(TabList)) {
			CString strBakFile = Misc::GetFileNameNoExt(TabList) + _T(".bak.xml");
			::MoveFileEx(TabList, strBakFile, MOVEFILE_REPLACE_EXISTING);
		}
		filestream.close();
		::MoveFileEx(strTempTabList, TabList, MOVEFILE_REPLACE_EXISTING);

	} catch (...) {
		MessageBox(_T("SaveAllTabでエラー発生!"));
	}
	m_bNowSaveAllTab = false;
}



void	CMainFrame::RestoreAllTab(LPCTSTR strFilePath, bool bClose)
{
	/* default.dfgがあればそちらを使う */
	CString	strFile = CStartUpOption::GetDefaultDFGFilePath();
	if (::PathFileExists(strFile)) {
		if ( MtlIsExt( strFile, _T(".dfg") ) ) {
			if ( !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOCLOSEDFG) ) {
				_LoadGroupOption(strFile, true);
			} else {
				_LoadGroupOption(strFile, false);
			}
			CString strBakFile = Misc::GetFileNameNoExt(strFile) + _T(".bak.dfg");
			::MoveFileEx(strFile, strBakFile, MOVEFILE_REPLACE_EXISTING);
			PostMessage(WM_INITPROCESSFINISHED);
			return;
		}
	}

	int	nActiveIndex = 0;
	std::vector<unique_ptr<ChildFrameDataOnClose> >	vecpSaveData;

	CString	TabList;
	if (strFilePath == NULL) {
		TabList = Misc::GetExeDirectory() + _T("TabList.xml");
	} else {
		TabList = strFilePath;
	}

	try {
		using boost::property_tree::wptree;

		std::wifstream	filestream(TabList);
		if (!filestream) {
			PostMessage(WM_INITPROCESSFINISHED);
			return ;
		}
		filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));

		wptree	pt;
		boost::property_tree::read_xml(filestream, pt);

		auto SetTravelLog	= [](wptree& ptLog, vector<std::pair<CString, CString> >& vecTravelLog) {
			for (auto it = ptLog.begin(); it != ptLog.end(); ++it) {
				wptree& item = it->second;
				vecTravelLog.push_back(std::pair<CString, CString>(
					item.get(L"<xmlattr>.title", L"").c_str(), 
					item.get(L"<xmlattr>.url", L"").c_str()));
			}
		};

		wptree&	ptChild = pt.get_child(L"TabList");
		auto it = ptChild.begin();
		nActiveIndex = it->second.get(L"ActiveIndex", 0);
		++it;
		for (; it != ptChild.end(); ++it) {
			wptree& ptItem = it->second;
			unique_ptr<ChildFrameDataOnClose>	pdata(new ChildFrameDataOnClose);

			pdata->strTitle	= ptItem.get(L"<xmlattr>.title", L"").c_str();
			pdata->strURL	= ptItem.get(L"<xmlattr>.url", L"").c_str();
			pdata->dwDLCtrl	= ptItem.get<DWORD>(L"<xmlattr>.DLCtrlFlags", CDLControlOption::s_dwDLControlFlags);
			pdata->dwExStyle= ptItem.get<DWORD>(L"<xmlattr>.ExStyle",	CDLControlOption::s_dwExtendedStyleFlags);
			pdata->dwAutoRefreshStyle	= ptItem.get<DWORD>(L"<xmlattr>.AutoRefreshStyle", 0);
			SetTravelLog(ptItem.get_child(L"TravelLog.Back"), pdata->TravelLogBack);
			SetTravelLog(ptItem.get_child(L"TravelLog.Fore"), pdata->TravelLogFore);

			vecpSaveData.push_back(std::move(pdata));
		}
	} catch (...) {
		MessageBox(_T("RestoreAllTabでエラーが発生しました!"));
		PostMessage(WM_INITPROCESSFINISHED);
		return ;
	}

	CLockRedrawMDIClient	 lock(m_ChildFrameClient);
	CDonutTabBar::CLockRedraw lock2(m_MDITab);

	if (bClose) 
		MtlCloseAllMDIChildren(m_ChildFrameClient);

	int nCount = (int)vecpSaveData.size();
	vector<NewChildFrameData*>	vecpNewChildData;
	vecpNewChildData.reserve(nCount);
	for (int i = 0; i < nCount; ++i) {						// OnCloseからNewChildに転写
		ChildFrameDataOnClose&	data = *vecpSaveData[i];

		NewChildFrameData*	pNewChildData = new NewChildFrameData(m_ChildFrameClient);
		pNewChildData->strURL		= data.strURL;
		pNewChildData->dwDLCtrl		= data.dwDLCtrl;
		pNewChildData->dwExStyle	= data.dwExStyle;
		pNewChildData->dwAutoRefresh= data.dwAutoRefreshStyle;
		pNewChildData->bActive	= (i == nActiveIndex);
		vecpNewChildData.push_back(pNewChildData);
	}
	for (int i = 0; i < nCount - 1; ++i) {	
		vecpNewChildData[i]->pNext	= vecpNewChildData[i + 1];
	}
	for (int i = 0; i < nCount; ++i) {
		ChildFrameDataOnClose* pData = vecpSaveData[i].release();
		NewChildFrameData*	pThis = vecpNewChildData[i];
		pThis->funcCallAfterCreated	= [pData, pThis, this](CChildFrame* pChild) {
			pChild->SetTravelLog(pData->TravelLogFore, pData->TravelLogBack);
			delete pData;
			if (pThis->pNext) {
				m_MDITab.SetInsertIndex(m_MDITab.GetItemCount());
				CChildFrame::AsyncCreate(*pThis->pNext);	// 次のChildFrameを作成
			} else {
				m_MDITab.InsertHere(false);
				this->PostMessage(WM_INITPROCESSFINISHED);
			}
			delete pThis;

		};
	}
	if (vecpNewChildData.size() > 0) {
		m_MDITab.InsertHere(true);
		m_MDITab.SetInsertIndex(m_MDITab.GetItemCount());
		CChildFrame::AsyncCreate(*vecpNewChildData[0]);
	} else {
		PostMessage(WM_INITPROCESSFINISHED);
	}
}


// ===========================================================================

///+++
void CMainFrame::_SaveGroupOption(const CString &strFileName, bool bDelay /*=false*/)
{
	MessageBox(_T("_SaveGroupOption 未実装です"));
  #if 0	//+++ v1.48c で削除. 高頻度の自動セーブが設定されていると、環境によってはパフォーマンス悪化が大きいみたいなのでoff.
	//x RtlSetMinProcWorkingSetSize();				//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 ) ... ひょっとするとあまりよろしくないかもだが...
  #endif
}



void CMainFrame::_LoadGroupOption(const CString &strFileName, bool bClose)
{
#if 0	//:::

	//dmfTRACE( _T("CMainFrame::_LoadGroupOption\n") );

	CLockRedrawMDIClient	 lock(m_hWndMDIClient);
	CDonutTabBar::CLockRedraw lock2(m_MDITab);
	CWaitCursor 			 cur;

	if (bClose)
		MtlCloseAllMDIChildren(m_hWndMDIClient);

	DWORD			dwCount 	= 0;
  #if INISECTION_USE_MUTEX != 1
	CIniFileI		pr( strFileName, _T("Header") );
	if (pr.QueryValue( dwCount, _T("count") ) != ERROR_SUCCESS)
		return;
	DWORD			dwActive	= pr.GetValue( _T("active")   , 0 );
	DWORD			dwMaximized = pr.GetValue( _T("maximized"), 0 );
	pr.Close();
  #else
	CIniFileI		pr( strFileName, _T("Header") );
	if (pr.QueryValue( dwCount, _T("count") ) != ERROR_SUCCESS)
		return;
	DWORD			dwActive	= pr.GetValue( _T("active")   , 0 );
	DWORD			dwMaximized = pr.GetValue( _T("maximized"), 0 );
  #endif

	bool			bActiveChildExistAlready = (m_ChildFrameClient.GetActiveChildFrameWindow() != NULL);

	m_MDITab.InsertHere(true);
	int nInsertIndex = m_MDITab.GetItemCount();
	m_MDITab.SetInsertIndex(nInsertIndex);

	CChildFrame *	pChildActive = NULL;
	for (DWORD dw = 0; dw < dwCount; ++dw) {
		CChildFrame *pChild = CChildFrame::NewWindow(m_hWndMDIClient, m_MDITab, m_AddressBar);
		// if no active child, as there is no client edge in MDI client window,
		// so GetClientRect is different a little and a resizing will occur when switching.
		// That is, only the first child window is activated.
		if (pChild == NULL)
			continue;

		++nInsertIndex;
		m_MDITab.SetInsertIndex(nInsertIndex);

		// activate now!
		pChild->ActivateFrame(m_ChildFrameClient.GetActiveChildFrameWindow() != NULL ? SW_SHOWNOACTIVATE : -1);

		// if tab mode, no need to load window placement.
		pChild->view().m_ViewOption.GetProfile(strFileName, dw, !CMainOption::s_bTabMode);

		// 戻る・進むの項目を設定する
		if (CMainOption::s_bTravelLogGroup) {
			CString 	strSection;
			strSection.Format(_T("Window%d"), dw);
			_Load_OptionalData(pChild, strFileName, strSection);
		}

		if (dw == dwActive)
			pChildActive = pChild;
	}
	m_MDITab.InsertHere(false);
	m_MDITab.SetInsertIndex(-1);

	if (pChildActive == NULL)
		return;

	if (!bActiveChildExistAlready) {						// there was no active window
		MDIActivate(pChildActive->m_hWnd);
		if (CMainOption::s_bTabMode || dwMaximized == 1) {	// in tab mode
			MDIMaximize(pChildActive->m_hWnd);
		}
	} else {												// already an active window exists
		if ( !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE) )
			MDIActivate(pChildActive->m_hWnd);
	}
#endif
}



// ===========================================================================

void CMainFrame::OnBackUpOptionChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (m_nBackUpTimerID != 0) {
		KillTimer(m_nBackUpTimerID);
		m_nBackUpTimerID = 0;
	}

	if (CMainOption::s_dwMainExtendedStyle & MAIN_EX_BACKUP)
		m_nBackUpTimerID = SetTimer(1, 1000 * 60 * CMainOption::s_dwBackUpTime);
}


//private:
void CMainFrame::OnFavoriteGroupSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (m_ChildFrameClient.GetActiveChildFrameWindow() == NULL)
		return;

	TCHAR		szOldPath[MAX_PATH];	// save current directory
	szOldPath[0]	= 0;	//+++
	::GetCurrentDirectory(MAX_PATH, szOldPath);

	CString dir = DonutGetFavoriteGroupFolder();
	::SetCurrentDirectory( LPCTSTR(dir) );

	const TCHAR szFilter[] = _T("Donut Favorite Groupファイル(*.dfg)\0*.dfg\0\0");
	CFileDialog fileDlg(FALSE, _T("dfg"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	fileDlg.m_ofn.lpstrInitialDir = dir;
	fileDlg.m_ofn.lpstrTitle	  = _T("お気に入りグループの保存");

	if (fileDlg.DoModal() == IDOK) {
		_SaveGroupOption(fileDlg.m_szFileName);
		::SendMessage(m_hWnd, WM_REFRESH_EXPBAR, 1, 0);
	}

	// restore current directory
	::SetCurrentDirectory(szOldPath);
}



#if 1 //+++ WS_CAPTION無しがフルスクリーン、という前提を捨てたので、Mtlの外部関数だったのをこちらへ移動
template <class _Profile>
static void /*Mtl*/WriteProfileStatusBarState( _Profile &__profile, HWND hWndStatusBar, const CString &strPrefix = _T("statusbar.") )
{
	ATLASSERT( ::IsWindow(hWndStatusBar) );

	// 親が、退避時は保存しない
	HWND hWndParent = ::GetParent(hWndStatusBar);
	if (hWndParent) {
		if (::IsWindowVisible(hWndParent) == FALSE)
			return;
		//if (IsFullScreen())
		if (g_pMainWnd->IsFullScreen())		//+++
			return;
	}
	LONG lRet		= __profile.SetValue( ::IsWindowVisible(hWndStatusBar) != 0, strPrefix + _T("Visible") );
	ATLASSERT(lRet == ERROR_SUCCESS);
}
#endif


void CMainFrame::_WriteProfile()
{
	// save frame and status bar
	CIniFileO	pr( g_szIniFileName, _T("Main") );
	MtlWriteProfileMainFrameState(pr, m_hWnd);
	/*Mtl*/WriteProfileStatusBarState(pr, m_hWndStatusBar);

	// save rebar
	pr.ChangeSectionName(_T("ReBar"));
	MtlWriteProfileReBarBandsState(pr, m_hWndToolBar);
	pr.Close();

  #if 0 //+++ OnCloseのしょっぱなに行うように変更... //やっぱりここで
	// save group options
	_SaveGroupOption( CStartUpOption::GetDefaultDFGFilePath() );
  #endif
}



// ====================================================================================

void CMainFrame::OnTimer(UINT_PTR nIDEvent, TIMERPROC dmy /*= 0*/)
{
	switch (nIDEvent) {
	case ENT_READ_ACCEL:		//+++ メモ:起動時のみの、アクセラキー読み込み(遅延のためタイマーを使う)
		if (m_hAccel) {
			CAccelerManager 	accelManager(m_hAccel);
			m_hAccel = accelManager.LoadAccelaratorState(m_hAccel);
			::KillTimer(m_hWnd, nIDEvent);
		}
		break;

	default:
		//+++ 自動セーブが設定されていた場合、default.dfgをオートバックアップ.
		if (m_nBackUpTimerID == nIDEvent && m_nBackUpTimerID) {
			//_SaveGroupOption( CStartUpOption::GetDefaultDFGFilePath(), true );
			if (m_bNowSaveAllTab) {
				MessageBox(_T("TabList.xmlの自動セーブの時間がかかりすぎています。"));
			} else {
				boost::thread thrd(boost::bind(&CMainFrame::SaveAllTab, this));
			}
			//SaveAllTab();
		} else {
			SetMsgHandled(FALSE);
			return;
		}
		break;
	}
}



BOOL CMainFrame::OnIdle()
{
	if (m_bWM_TIMER/*IsIconic()*/)
		return TRUE;
	//return FALSE;
	// Note. under 0.01 sec (in dbg-mode on 330mhz cpu)
	CmdUIUpdateToolBars();
	CmdUIUpdateStatusBar	(m_hWndStatusBar, ID_DEFAULT_PANE	);
	CmdUIUpdateStatusBar	(m_hWndStatusBar, ID_SECURE_PANE	);
	CmdUIUpdateStatusBar	(m_hWndStatusBar, ID_PRIVACY_PANE	);
	CmdUIUpdateChildWindow	(m_hWndStatusBar, IDC_PROGRESS		);
	m_mcCmdBar.UpdateMDIMenuControl();

#if 0
	if ( _check_flag(MAIN_EX_KILLDIALOG, CMainOption::s_dwMainExtendedStyle) )
		CDialogKiller2::KillDialog();
#endif
	return FALSE;
}

// リバーとビューの間の横線を書く
void	CMainFrame::OnPaint(CDCHandle /*dc*/)
{
	SetMsgHandled(FALSE);

	// Constants
	enum _ReBarBorderConstants {
		s_kcxBorder = 2
	};

	CClientDC	dc(m_hWnd);

	CRect rc;
	::GetWindowRect(m_hWndToolBar, &rc);
	ScreenToClient(&rc);

	rc.InflateRect(s_kcxBorder, s_kcxBorder);
	dc.DrawEdge(rc, EDGE_ETCHED, BF_RECT);
}


// コマンドラインからURLを取り出す
static void PerseUrls(LPCTSTR lpszCommandline, std::vector<CString>& vecUrls)
{
	std::wstring	strCommandline = lpszCommandline;
	auto			itbegin = strCommandline.cbegin();
	auto			itend	= strCommandline.cend();
	std::wregex		rx(L"(?:\")([^\"]+)(?:\")");	// "～"
	std::wsmatch	result;
	while (std::regex_search(itbegin, itend, result, rx)) {
		vecUrls.push_back(result[1].str().c_str());
		itbegin = result[0].second;
	}
	if (vecUrls.size() == 0) {
		strCommandline = lpszCommandline;
		itbegin = strCommandline.cbegin();
		itend	= strCommandline.cend();
		std::wregex		rx(L"([^ ]+)");
		std::wsmatch	result;
		while (std::regex_search(itbegin, itend, result, rx)) {
			vecUrls.push_back(result[1].str().c_str());
			itbegin = result[0].second;
		}
	}
}

/// 複数起動されたunDonutの引数が渡される
BOOL	CMainFrame::OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
	if (pCopyDataStruct->dwData != 1)
		return FALSE;
	
	bool	bActive = !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE);

	CString	strCommandLine = (LPCTSTR)pCopyDataStruct->lpData;
	_AnalyzeCommandLine(strCommandLine);

	if ( bActive ) {
		IfTrayRestoreWindow();									//+++ トレイ状態だったら復活.
		if (IsZoomed() == FALSE)
			ShowWindow(SW_RESTORE);
		MtlSetForegroundWindow(m_hWnd); 						//ウインドウをアクティブにする
		//if (m_bOldMaximized == 0 && m_bFullScreen == 0) 		//+++
		//	ShowWindow_Restore(1);	//ShowWindow(SW_RESTORE);	//+++ サイズを戻す.
	}

	return TRUE;
}

/// 初期化処理が全て終了したあとにコマンドライン文字列を処理するための関数
void	CMainFrame::OnInitProcessFinished()
{
	_AnalyzeCommandLine(m_strCommandLine);
	m_strCommandLine.Empty();
}


// ===========================================================================

// UDT DGSTR
LRESULT CMainFrame::UpdateTitleBar(LPCTSTR lpszStatusBar, DWORD /*dwReserved*/)
{
#if 0	//:::
	if ((GetStyle() & WS_CAPTION) != 0 && m_bTray == 0) {	//+++ チェック追加:タイトルバーが表示されているとき
		if ( !::IsWindowVisible(m_hWndStatusBar) )			//+++ メモ:ステータスバーが表示されていなかったら
			UpdateTitleBarUpsideDown(lpszStatusBar);		//+++ メモ:タイトルバーに、ステータスバー文字列を出す...
	}
#endif
	return 0;
}
// ENDE

/// ChildFrameが作成されたのでタブを作成
void	CMainFrame::OnTabCreate(HWND hWndChildFrame, DWORD dwOption)
{
	if (dwOption & TAB_LINK)
		m_MDITab.SetLinkState(LINKSTATE_A_ON);
	if (!_check_flag(MAIN_EX_NOACTIVATE_NEWWIN, CMainOption::s_dwMainExtendedStyle))
		dwOption |= TAB_ACTIVE;
	m_MDITab.OnMDIChildCreate(hWndChildFrame, (dwOption & TAB_ACTIVE) != 0);
	
	//if ( _check_flag(m_view.m_ViewOption.m_dwExStyle, DVS_EX_OPENNEWWIN)) {
	//	m_MDITab.NavigateLockTab(m_hWnd, true);
	//}

	// Raise Plugin Event
	int nNewIndex = m_MDITab.GetTabIndex(hWndChildFrame);
	CPluginManager::ChainCast_PluginEvent(DEVT_TAB_OPENED, nNewIndex, 0);
}

/// ChildFrameが破棄されたのでタブを破棄
void	CMainFrame::OnTabDestory(HWND hWndChildFrame)
{
	m_MDITab.OnMDIChildDestroy(hWndChildFrame);
}

/// 最近閉じたタブに追加
void	CMainFrame::OnAddRecentClosedTab(ChildFrameDataOnClose* pClosedTabData)
{
	m_RecentClosedTabList.AddToList(pClosedTabData);
}

/// 独自ページ内検索バーを表示する
void	CMainFrame::OnOpenFindBarWithText(LPCTSTR strText)
{
	if (CMainOption::s_bUseCustomFindBar) 
		m_FindBar.ShowFindBar(strText);
}

void	CMainFrame::OnBrowserTitleChange(HWND hWndChildFrame, LPCTSTR strTitle)
{
	m_MDITab.SetTitle(hWndChildFrame, strTitle);

	// キャプションを変更
	if (m_ChildFrameClient.GetActiveChildFrameWindow() == hWndChildFrame) {
		CString strapp;
		strapp.LoadString(IDR_MAINFRAME);
		CString strMainTitle;
		strMainTitle.Format(_T("%s - %s"), strTitle, strapp);
		SetWindowText(strMainTitle);
	}
}

void	CMainFrame::OnBrowserLocationChange(LPCTSTR strURL, HICON hFavicon)
{
	m_AddressBar.SetWindowText(strURL);	
	m_AddressBar.ReplaceIcon(hFavicon);
}

void	CMainFrame::OnSetSearchText(LPCTSTR strText, bool bHilightOn)
{
	m_SearchBar.SetSearchStr(strText);
	m_SearchBar.ForceSetHilightBtnOn(bHilightOn);
}


// UH -  minit
//------------------------
/// お気に入りのメニューハンドルを返す
LRESULT CMainFrame::OnMenuGetFav()
{
	return (LRESULT)m_FavoriteMenu.GetMenu().m_hMenu;
}

//-------------------------
/// お気に入りグループのメニューハンドルを返す
LRESULT CMainFrame::OnMenuGetFavGroup()
{
	return (LRESULT)m_FavGroupMenu.GetMenu().m_hMenu;
}

//-------------------------
/// ユーザースクリプトのメニューハンドルを返す
LRESULT CMainFrame::OnMenuGetScript()
{
	if ( !m_DropScriptMenu.GetMenu().IsMenu() ) {
		CMenuHandle menu;
		menu.LoadMenu(IDR_DROPDOWN_SCRIPT);
		m_DropScriptMenu.SetRootDirectoryPath( Misc::GetExeDirectory() + _T("Script") );
		m_DropScriptMenu.SetTargetWindow(m_hWnd);
		m_DropScriptMenu.InstallExplorerMenu(menu);
		m_DropScriptMenu.RefreshMenu();
	}

	return (LRESULT) m_DropScriptMenu.GetMenu().m_hMenu;
}


LRESULT CMainFrame::OnMenuGoBack(HMENU hMenu)
{
	HWND hMDIActive = m_ChildFrameClient.GetActiveChildFrameWindow();

	return ::SendMessage(hMDIActive, WM_MENU_GOBACK, (WPARAM) (HMENU) hMenu, (LPARAM) 0);
}


LRESULT CMainFrame::OnMenuGoForward(HMENU hMenu)
{
	HWND hMDIActive = m_ChildFrameClient.GetActiveChildFrameWindow();

	return ::SendMessage(hMDIActive, WM_MENU_GOFORWARD, (WPARAM) (HMENU) hMenu, (LPARAM) 0);
}

LRESULT	CMainFrame::OnMenuGetBingTranslate()
{
	return (LRESULT)m_TranslateMenu.GetMenu().m_hMenu;
}


LRESULT CMainFrame::OnMenuRefreshScript(BOOL bInit)
{
	if ( m_DropScriptMenu.GetMenu().IsMenu() ) {
		if (bInit == FALSE) {
			m_DropScriptMenu.GetMenu().DestroyMenu();
		}
	}

	return S_OK;
}


/// お気に入りメニューからリンクのパスを返す
LRESULT	CMainFrame::OnGetFavoriteFilePath(int nID)
{
	return (LRESULT)(LPCTSTR)m_FavoriteMenu.GetFilePath(nID);
}


LRESULT CMainFrame::OnMenuDrag(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	return m_wndMenuDropTarget.OnMenuDrag(uMsg, wParam, lParam, bHandled);
}



LRESULT CMainFrame::OnMenuGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	return m_wndMenuDropTarget.OnMenuGetObject(uMsg, wParam, lParam, bHandled);
}


//////////////////////////////////////////////////////////////////
// custom draw of addressbar
LRESULT CMainFrame::OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;

	LPNMHDR lpnmh = (LPNMHDR) lParam;
	LRESULT lRet  = 0;

	//if (lpnmh->code == NM_CUSTOMDRAW && lpnmh->hwndFrom == m_hWndToolBar) { 	// from rebar
	//	lRet = m_AddressBar.OnCustomDraw(0, lpnmh, bHandled);
	//}

	if (lpnmh->code == TBN_DROPDOWN) {
		int nCount = CPluginManager::GetCount(PLT_TOOLBAR);

		for (int nIndex = 0; nIndex < nCount; nIndex++) {
			HWND	hWndBar = CPluginManager::GetHWND(PLT_TOOLBAR, nIndex);

			if (lpnmh->hwndFrom != hWndBar)
				continue;

			CPluginManager::Call_ShowToolBarMenu(PLT_TOOLBAR, nIndex, ( (LPNMTOOLBAR) lpnmh )->iItem);

			bHandled = TRUE;
			break;
		}
	}

	return lRet;
}


//////////////////////////////////////////////////////////////////
// the custom message from MDI child
void CMainFrame::OnMDIChild(HWND hWnd, UINT nCode)
{
	SetMsgHandled(FALSE);

	if (nCode == MDICHILD_USER_ALLCLOSED) {
		m_AddressBar.SetWindowText( _T("") );
		::SetWindowText( m_hWndStatusBar, _T("レディ") );
		//m_CmdBar.EnableButton(_nPosWindowMenu, false);
	} else if (nCode == MDICHILD_USER_FIRSTCREATED) {
		//m_CmdBar.EnableButton(_nPosWindowMenu, true);
	} else if (nCode == MDICHILD_USER_ACTIVATED) {
		_OnMDIActivate(hWnd);
		OnIdle();						// make sure when cpu is busy
	} else if (nCode == MDICHILD_USER_TITLECHANGED) {
		//OnTitleChanged();
	}
}


BOOL CMainFrame::OnBrowserCanSetFocus()
{										// asked by browser
	HWND hWndFocus = ::GetFocus();
	if (hWndFocus == NULL)
		return TRUE;

	if (m_SearchBar.GetEditCtrl().m_hWnd == hWndFocus)
		return FALSE;

	if (m_AddressBar.GetEditCtrl().m_hWnd == hWndFocus)
		return FALSE;

	if ( ::IsChild(m_hWndToolBar, hWndFocus) )
		return FALSE;

	if ( m_ExplorerBar.IsChild(hWndFocus) )
		return FALSE;

	return TRUE;
}


void CMainFrame::OnParentNotify(UINT fwEvent, UINT idChild, LPARAM lParam)
{
	if (fwEvent != WM_RBUTTONDOWN)
		return;

	CPoint		pt( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
	ClientToScreen(&pt);

	CRect		rc;
	::GetClientRect(m_hWndToolBar, &rc);
	CPoint		ptRebar  = pt;
	::ScreenToClient(m_hWndToolBar, &ptRebar);

	if ( !rc.PtInRect(ptRebar) )		// not on rebar
		return;
#if 0
	HWND		hWnd	 = ::WindowFromPoint(pt);

	if (hWnd == m_MDITab.m_hWnd) {		// on tab bar
		CPoint ptTab = pt;
		m_MDITab.ScreenToClient(&ptTab);

		if (m_MDITab.HitTest(ptTab) != -1)
			return;
	}
#endif
	CMenuHandle menuView = ::GetSubMenu(m_CmdBar.GetMenu(), _nPosViewMenu);
	CMenuHandle menu	 = menuView.GetSubMenu(0);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd, NULL);
}


void CMainFrame::OnActivate(UINT nState, BOOL bMinimized, HWND hWndOther)
{
	if (nState == WA_INACTIVE) {
		m_hWndFocus = ::GetFocus();
	} else {
		if (m_hWndFocus)
			::SetFocus(m_hWndFocus);
	}
}


//minit
BOOL CMainFrame::_IsRebarBandLocked()
{
	CReBarCtrl	  rebar(m_hWndToolBar);
	REBARBANDINFO rbbi = { sizeof (REBARBANDINFO) };
	rbbi.fMask	= RBBIM_STYLE;
	if ( !rebar.GetBandInfo(0, &rbbi) )
		return FALSE;

	return (rbbi.fStyle & RBBS_NOGRIPPER) != 0;
}

// ツールバーなどをロック(固定)する
LRESULT CMainFrame::OnViewToolBarLock(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	m_ReBar.LockBands(!_IsRebarBandLocked());
	return 0;
}


void CMainFrame::OnExplorerBarAutoShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_AUTOSHOW)
		CMainOption::s_dwExplorerBarStyle &= ~MAIN_EXPLORER_AUTOSHOW;
	else
		CMainOption::s_dwExplorerBarStyle |= MAIN_EXPLORER_AUTOSHOW;
}


void CMainFrame::_RefreshFavMenu()
{
	m_FavoriteMenu.RefreshMenu();
	m_FavGroupMenu.RefreshMenu();
	m_styleSheetMenu.RefreshMenu();

	m_ExplorerBar.RefreshFavBar();
}


////////////////////////////////////////////////////////////////////////////////
//ファイルメニュー
//　コマンドハンドラ
////////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnViewHome(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	bool bNew = ::GetAsyncKeyState(VK_CONTROL) < 0 || ::GetAsyncKeyState(VK_SHIFT) < 0;

	if (bNew || m_ChildFrameClient.GetActiveChildFrameWindow() == NULL) {
		OnFileNewHome(0, 0, 0);
	} else {
		SetMsgHandled(FALSE);
	}
}


/// bShow == TRUE で各リバーバンドの左端にテキストを表示する
void CMainFrame::ShowLinkText(BOOL bShow)
{
	CReBarCtrl	rebar(m_hWndToolBar);
	if (rebar.IsWindow() == FALSE)
		return ;

	REBARBANDINFO rbBand = { sizeof (REBARBANDINFO) };
	int nCount = rebar.GetBandCount();
	for (int ii = 0; ii < nCount; ii++) {
		rbBand.fMask = RBBIM_ID;
		rebar.GetBandInfo(ii, &rbBand);
		if (rbBand.wID != IDC_LINKBAR && rbBand.wID != IDC_SEARCHBAR && rbBand.wID != IDC_ADDRESSBAR)
			continue;

		if (bShow) {
			switch (rbBand.wID) {
			case IDC_ADDRESSBAR:
				rbBand.lpText = _T("アドレス");
				break;

			case IDC_LINKBAR:
				rbBand.lpText = _T("リンク");
				break;

			case IDC_SEARCHBAR:
				rbBand.lpText = _T("検索");
				break;
			}
		}
		rbBand.fMask = RBBIM_TEXT;
		rebar.SetBandInfo(ii, &rbBand);
	}
}

void CMainFrame::OnShowTextChg(BOOL bShow)
{
	ShowLinkText(bShow);
	m_AddressBar.ShowAddresText(m_hWndToolBar, bShow);
}



LRESULT CMainFrame::OnSysCommand(UINT nID, CPoint point)
{
	switch (nID) {
	case ID_VIEW_COMMANDBAR:
		SendMessage(m_hWnd, WM_COMMAND, ID_VIEW_COMMANDBAR, 0);
		SetMsgHandled(TRUE);
		break;

  #if 1	//+++ 最小化ボタンを押した時に、タスクトレイに入るようにしてみる.
	case SC_MINIMIZE:
		if ((CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MINBTN2TRAY)	//+++ 最小化ボタンでタスクトレイに入れる設定のとき、
			&& (point.x || point.y)											//+++ x,yが0,0ならタスクバーでクリックした場合だろうで、トレイにいれず、最小化だけしてみる.
		) {
			OnGetOut(0,0,0);
			SetMsgHandled(TRUE);
			break;
		}
		SetMsgHandled(FALSE);
		break;

	case SC_CLOSE:
		if ((CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_CLOSEBTN2TRAY)	//+++ 最小化ボタンでタスクトレイに入れる設定のとき、
			&& (point.x || point.y)											//+++ x,yが0,0ならタスクバーでクリックした場合だろうで、トレイにいれず、最小化だけしてみる.
		) {
			OnGetOut(0,0,0);
			SetMsgHandled(TRUE);
			break;
		}
		SetMsgHandled(FALSE);
		break;
  #endif

	default:
		SetMsgHandled(FALSE);
		break;
	}

	return 0;
}

/// ポップアップ抑止をトグル
LRESULT CMainFrame::OnPopupClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CIgnoredURLsOption::s_bValid = !CIgnoredURLsOption::s_bValid;
	return 0;
}

/// タイトル抑止をトグル
LRESULT CMainFrame::OnTitleClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CCloseTitlesOption::s_bValid = !CCloseTitlesOption::s_bValid;
	return 0;
}

/// ポップアップとタイトル抑止両方をトグル
LRESULT CMainFrame::OnDoubleClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CIgnoredURLsOption::s_bValid = !CIgnoredURLsOption::s_bValid;
	CCloseTitlesOption::s_bValid = !CCloseTitlesOption::s_bValid;
	return 0;
}

/// プライバシーレポートを表示
LRESULT CMainFrame::OnPrivacyReport(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CChildFrame* pChild = GetActiveChildFrame();
	if (pChild == NULL)
		return 0;

	CString 		strURL	   = pChild->GetLocationURL();
	CComBSTR		bstrURL(strURL);

	CComQIPtr<IServiceProvider>		spSP = pChild->GetMarshalIWebBrowser();
	if (spSP == NULL)
		return 0;

	CComPtr<IEnumPrivacyRecords>	spEnumRecords;
	spSP->QueryService(CLSID_IEnumPrivacyRecords, &spEnumRecords);
	if (spEnumRecords == NULL)
		return 0;

	HINSTANCE		hInstDLL;
	typedef DWORD	(WINAPI* FuncDoPrivacyDlg)(HWND, LPOLESTR, IEnumPrivacyRecords*, BOOL);
	FuncDoPrivacyDlg pfnDoPrivacyDlg = NULL;

	if (Misc::getIEMejourVersion() >= 8 && _CheckOsVersion_VistaLater() == 0){//\\ XP+IE8の場合
		hInstDLL = ::LoadLibrary( _T("ieframe.dll") );
	} else {//vista+IE8の場合
		hInstDLL = ::LoadLibrary( _T("shdocvw.dll") );
	}
	if ( hInstDLL == NULL )
		return 0;

	pfnDoPrivacyDlg	= (FuncDoPrivacyDlg)GetProcAddress( hInstDLL, "DoPrivacyDlg" );
	if (pfnDoPrivacyDlg == NULL) {
		::FreeLibrary(hInstDLL);
		return 0;
	}

	BOOL	bPrivacyImpacted = FALSE;
	spEnumRecords->GetPrivacyImpacted(&bPrivacyImpacted);
	pfnDoPrivacyDlg(m_hWnd, bstrURL, spEnumRecords, !bPrivacyImpacted);

	::FreeLibrary( hInstDLL );

	return 1;
}


/// クッキーの制限を設定
LRESULT CMainFrame::OnCookiesIE6(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	DWORD	  dwLv = 0;

	switch (wID) {
	case ID_URLACTION_COOKIES_BLOCK:	dwLv = PRIVACY_TEMPLATE_NO_COOKIES; 	break;
	case ID_URLACTION_COOKIES_HI:		dwLv = PRIVACY_TEMPLATE_HIGH;			break;
	case ID_URLACTION_COOKIES_MIDHI:	dwLv = PRIVACY_TEMPLATE_MEDIUM_HIGH;	break;
	case ID_URLACTION_COOKIES_MID:		dwLv = PRIVACY_TEMPLATE_MEDIUM; 		break;
	case ID_URLACTION_COOKIES_LOW:		dwLv = PRIVACY_TEMPLATE_MEDIUM_LOW; 	break;
	case ID_URLACTION_COOKIES_ALL:		dwLv = PRIVACY_TEMPLATE_LOW;			break;
	}

	HINSTANCE	hInstDLL;
	typedef DWORD	(WINAPI* FuncPrivacySetZonePreferenceW)(DWORD, DWORD, DWORD, LPCWSTR);
	FuncPrivacySetZonePreferenceW	pfnPrivacySetZonePreferenceW = nullptr;

	if ( ( hInstDLL = ::LoadLibrary(_T("wininet.dll")) ) == NULL )
		return 0;

	pfnPrivacySetZonePreferenceW = (FuncPrivacySetZonePreferenceW)GetProcAddress( hInstDLL, "PrivacySetZonePreferenceW" );
	if (pfnPrivacySetZonePreferenceW == NULL) {
		::FreeLibrary( hInstDLL );
		return 0;
	}

	pfnPrivacySetZonePreferenceW(URLZONE_INTERNET, PRIVACY_TYPE_FIRST_PARTY, dwLv, NULL);
	pfnPrivacySetZonePreferenceW(URLZONE_INTERNET, PRIVACY_TYPE_THIRD_PARTY, dwLv, NULL);
	::FreeLibrary( hInstDLL );
	return 0;
}

// ウィンドウ

/// 全てのウィンドウを閉じる(すべて閉じる)
void CMainFrame::OnWindowCloseAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if ( !CDonutConfirmOption::OnCloseAll(m_hWnd) )
		return ;

	CWaitCursor 		 cur;
	//CLockRedrawMDIClient lock(m_hWndMDIClient);
	CDonutTabBar::CLockRedraw 		 lock2(m_MDITab);
	m_MDITab.ForEachWindow([](HWND hWnd) {
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
	});
	//MtlCloseAllMDIChildren(m_hWndMDIClient);

	RtlSetMinProcWorkingSetSize();		//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
}

/// タブの右側/左側をすべて閉じる
void	CMainFrame::OnLeftRightClose(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	bool	bLeft	= (nID == ID_LEFT_CLOSE);
	if ( !CDonutConfirmOption::OnCloseLeftRight( GetTopLevelParent(), bLeft ) )
		return ;

	CWaitCursor cur;

	HWND		hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
	int 		nCurSel    = m_MDITab.GetTabIndex(hWndActive);
	if (nCurSel == -1 || hWndActive == NULL)
		return ;

	CSimpleArray<HWND> arrWnd;
	int nCount = m_MDITab.GetItemCount();
	for (int ii = 0; ii < nCount; ++ii) {
		HWND hWnd = m_MDITab.GetTabHwnd(ii);

		if (bLeft && ii < nCurSel)
			arrWnd.Add(hWnd);
		else if (bLeft == false && ii > nCurSel)
			arrWnd.Add(hWnd);
	}

	for (int ii = 0; ii < arrWnd.GetSize(); ++ii) {
		::PostMessage(arrWnd[ii], WM_CLOSE, 0, 0);
	}
}

/// このウィンドウ以外を閉じる
void	CMainFrame::OnWindowCloseExcept(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if ( !CDonutConfirmOption::OnCloseAllExcept( m_hWnd ) )
		return ;

	HWND	hWndActive = m_ChildFrameUIState.GetActiveChildFrameWindowHandle();
	CWaitCursor 		 cur;
	//CLockRedrawMDIClient lock(m_hWndMDIClient);
	CDonutTabBar::CLockRedraw 		 lock2(m_MDITab);
	m_MDITab.ForEachWindow([hWndActive](HWND hWnd) {
		if (hWndActive != hWnd)
			::PostMessage(hWnd, WM_CLOSE, 0, 0);
	});
}

/// 左のタブをアクティブに
void CMainFrame::OnTabLeft(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl)
{
	m_MDITab.LeftTab();
}

/// 右のタブをアクティブに
void CMainFrame::OnTabRight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl)
{
	m_MDITab.RightTab();
}


#if 0 //+++
//+++ 先頭タブから1..8選択. 9は最後のタブを選択
LRESULT CMainFrame::OnTabIdx(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL & /*bHandled*/)
{
	ATLASSERT(wID >= ID_TAB_IDX_1 && wID <= ID_TAB_IDX_LAST);
	int nCount = m_MDITab.GetItemCount();

	if (wID == ID_TAB_IDX_LAST) {
		wID =  nCount - 1;
	} else {
		wID -= ID_TAB_IDX_1;
		if (wID < 0 || wID >=8 || wID >= nCount)
			return 0;
	}
	m_MDITab.SetCurSel(wID);
	return 0;
}
#endif


LRESULT CMainFrame::OnChangeCSS(LPCTSTR lpszStyleSheet)
{
	if (CStyleSheetOption::s_bSetUserSheet)
		CStyleSheetOption::SetUserSheetName(lpszStyleSheet);

	//minit

	HWND hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
	if (hWndActive == NULL)
		return 0;

	return SendMessage(hWndActive, WM_USER_CHANGE_CSS, (WPARAM) lpszStyleSheet, 0);
}



/// メニューの右端にショートカットキーを表示するように設定する
LRESULT CMainFrame::OnInitMenuPopup(HMENU hMenuPopup, UINT uPos, BOOL bSystemMenu)
{
	// システムメニューは、処理しない
	if (bSystemMenu)
		return 0;
	
	if (   hMenuPopup == m_FavoriteMenu.GetMenu().m_hMenu 
		|| hMenuPopup == m_FavGroupMenu.GetMenu().m_hMenu 
		|| hMenuPopup == m_styleSheetMenu.GetMenu().m_hMenu
		|| m_FavoriteMenu.IsSubMenu(hMenuPopup) )
		return 0;

	CMenuHandle 	menu = hMenuPopup;
	CAccelerManager accel(m_hAccel);

	// 一番目のメニューが得られない時はそれを消して編集しない
	CString 		strCmd;

	if (menu.GetMenuString(0, strCmd, MF_BYPOSITION) == 0) {
		menu.RemoveMenu(0, MF_BYPOSITION);
		return 0;
	}

	if (CMenuOption::s_bNoCustomMenu) return 0;

	for (int ii = 0; ii < menu.GetMenuItemCount(); ii++) {
		strCmd = _T("");
		UINT	nID    = menu.GetMenuItemID(ii);
		menu.GetMenuString(nID, strCmd, MF_BYCOMMAND);

		if ( strCmd.IsEmpty() ) {
			if (strCmd.LoadString(nID) == FALSE)
				continue;
		}

		CString strShorCut;

		if ( !accel.FindAccelerator(nID, strShorCut) )	//逆になってたので修正 minit
			continue;

		if (strCmd.Find(_T("\t")) != -1) {
			strCmd = strCmd.Left( strCmd.Find(_T("\t")) );
		}

		if (strShorCut.IsEmpty() == FALSE)
			strCmd = strCmd + _T("\t") + strShorCut;

		UINT	uState = menu.GetMenuState(nID, MF_BYCOMMAND);
		menu.ModifyMenu(nID, MF_BYCOMMAND, nID, strCmd);

		if (uState & MF_CHECKED)
			menu.CheckMenuItem(nID, MF_CHECKED);

		if (uState & MF_GRAYED)
			menu.EnableMenuItem(nID, MF_GRAYED);
	}

	return 0;
}


void CMainFrame::OnRegisterAsBrowser(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/)
{
#if 0	//:::
	if (wNotifyCode == NM_ON) {
		MtlForEachMDIChild( m_hWndMDIClient, CSendCommand(ID_REGISTER_AS_BROWSER, NM_ON) );
	} else if (wNotifyCode == NM_OFF) {
		MtlForEachMDIChild( m_hWndMDIClient, CSendCommand(ID_REGISTER_AS_BROWSER, NM_OFF) );
	} else if ( !_check_flag(MAIN_EX_REGISTER_AS_BROWSER, CMainOption::s_dwMainExtendedStyle) ) {
		CMainOption::s_dwMainExtendedStyle |= MAIN_EX_REGISTER_AS_BROWSER;
		MtlForEachMDIChild( m_hWndMDIClient, CSendCommand(ID_REGISTER_AS_BROWSER, NM_ON) );
	} else {
		CMainOption::s_dwMainExtendedStyle &= ~MAIN_EX_REGISTER_AS_BROWSER;
		MtlForEachMDIChild( m_hWndMDIClient, CSendCommand(ID_REGISTER_AS_BROWSER, NM_OFF) );
	}
#endif
}


// 編集

void CMainFrame::OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CEdit editAddress = m_AddressBar.GetEditCtrl();
	CEdit editSearch  = m_SearchBar.GetEditCtrl();

	if (::GetFocus() == editAddress) {
		editAddress.Cut();
	} else if (::GetFocus() == editSearch) {
		editSearch.Cut();
	} else {
		SetMsgHandled(FALSE);							// MtlWeb.hのCWebBrowserCommandHandlerが処理
		return;
	}
}


void CMainFrame::OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CEdit editAddress = m_AddressBar.GetEditCtrl();
	CEdit editSearch  = m_SearchBar.GetEditCtrl();

	if (::GetFocus() == editAddress) {
		editAddress.Copy();
	} else if (::GetFocus() == editSearch) {
		editSearch.Copy();
	} else {
		SetMsgHandled(FALSE);							// MtlWeb.hのCWebBrowserCommandHandlerが処理
		return;
	}
}


void CMainFrame::OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CEdit editAddress = m_AddressBar.GetEditCtrl();
	CEdit editSearch  = m_SearchBar.GetEditCtrl();

	if (::GetFocus() == editAddress) {
		editAddress.Paste();
	} else if (::GetFocus() == editSearch) {
		editSearch.Paste();
	} else {
		SetMsgHandled(FALSE);
		return;
	}
}


void CMainFrame::OnEditSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CEdit editAddress = m_AddressBar.GetEditCtrl();
	CEdit editSearch  = m_SearchBar.GetEditCtrl();

	if (::GetFocus() == editAddress) {
		editAddress.SetSelAll();
	} else if (::GetFocus() == editSearch) {
		editSearch.SetSelAll();
	} else {
		SetMsgHandled(FALSE);							// MtlWeb.hのCWebBrowserCommandHandlerが処理
		return;
	}
}


// 表示

LRESULT CMainFrame::OnViewCommandBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlToggleBandVisible(m_hWndToolBar, ATL_IDW_COMMAND_BAR);
	UpdateLayout();
	return 0;
}


LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	// It sucks, band index is dynamic.
	// ::SendMessage(m_hWndToolBar, RB_SHOWBAND, 0, bNew);	// toolbar is band #0
	MtlToggleBandVisible(m_hWndToolBar, ATL_IDW_TOOLBAR);
	UpdateLayout();
	return 0;
}


LRESULT CMainFrame::OnViewAddressBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlToggleBandVisible(m_hWndToolBar, IDC_ADDRESSBAR);
	UpdateLayout();
	return 0;
}


LRESULT CMainFrame::OnViewLinkBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlToggleBandVisible(m_hWndToolBar, IDC_LINKBAR);
	UpdateLayout();
	return 0;
}


LRESULT CMainFrame::OnViewTabBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlToggleBandVisible(m_hWndToolBar, IDC_MDITAB);
	UpdateLayout();
	return 0;
}



LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	BOOL bNew = !::IsWindowVisible(m_hWndStatusBar);

	UpdateTitleBar(_T(""), 0);		//+++		statusオフからオンにしたときに、タイトルバーに出してしたステータス文字列を消すため.

	::ShowWindow(m_hWndStatusBar, bNew ? SW_SHOWNOACTIVATE : SW_HIDE);

	UpdateLayout();
	return 0;
}

//---------------------------------------
/// [ユーザー定義] : ツールバーのカスタマイズダイアログを表示する
LRESULT CMainFrame::OnViewToolBarCust(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	m_ToolBar.Customize();
	return 0;
}



/// すべてのウィンドウを中止
void CMainFrame::OnViewStopAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_MDITab.ForEachWindow([](HWND hWnd) {
		::PostMessage(hWnd, WM_COMMAND, ID_VIEW_STOP, 0);
	});
}

/// すべてのウィンドウを更新
void CMainFrame::OnViewRefreshAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_MDITab.ForEachWindow([](HWND hWnd) {
		::PostMessage(hWnd, WM_COMMAND, ID_VIEW_REFRESH, 0);
	});
}

/// このウィンドウ以外を更新
void	CMainFrame::OnWindowRefreshExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	HWND hWndActive = m_ChildFrameUIState.GetActiveChildFrameWindowHandle();
	m_MDITab.ForEachWindow([hWndActive](HWND hWnd) {
		if (hWndActive != hWnd)
			::PostMessage(hWnd, WM_COMMAND, ID_VIEW_REFRESH, 0);
	});
}


// お気に入り

/// お気に入りに追加する
void CMainFrame::OnFavoriteAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CChildFrame* pChild = GetChildFrame(m_ChildFrameUIState.GetActiveChildFrameWindowHandle());
	if (pChild == nullptr)
		return ;

	bool bOldShell = _check_flag(MAIN_EX_ADDFAVORITEOLDSHELL, CMainOption::s_dwMainExtendedStyle);

	MtlAddFavorite(pChild->GetLocationURL(), MtlGetWindowText(pChild->GetHwnd()), bOldShell, DonutGetFavoritesFolder(), m_hWnd);

	::SendMessage(m_hWnd, WM_REFRESH_EXPBAR, 0, 0);
}

/// お気に入りの整理
void CMainFrame::OnFavoriteOrganize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	bool bOldShell = _check_flag(MAIN_EX_ORGFAVORITEOLDSHELL, CMainOption::s_dwMainExtendedStyle);

	//		CString strPath = DonutGetFavoritesFolder();
	//		MtlOrganizeFavorite(m_hWnd, bOldShell, strPath);
	MtlOrganizeFavorite( m_hWnd, bOldShell, DonutGetFavoritesFolder() );
}


void CMainFrame::OnFavoriteGroupAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	::SendMessage(m_ChildFrameClient.GetActiveChildFrameWindow(), WM_COMMAND, (WPARAM) ID_FAVORITE_GROUP_ADD, 0);
}


void CMainFrame::OnFavoriteGroupOrganize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	::ShellExecute(m_hWnd, NULL, DonutGetFavoriteGroupFolder(), NULL, NULL, SW_SHOWNORMAL);
}


/// IEのオプションを表示する?
void CMainFrame::OnViewOption(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (m_ChildFrameClient.GetActiveChildFrameWindow() == NULL)
		MtlShowInternetOptions();
	else
		SetMsgHandled(FALSE);	// ChildFrameへ
}

/// Donutのオプションを表示
void CMainFrame::OnViewOptionDonut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	BOOL							bSkinChange = FALSE;

	CMenu							menu		= CExMenuManager::LoadFullMenu();

	CMainPropertyPage				pageMain(m_hWnd);
	CMainPropertyPage2				pageMain2(m_hWnd, m_RecentClosedTabList);
	CDLControlPropertyPage			pageDLC(m_hWnd);
	CMDITabPropertyPage 			pageTab(&m_MDITab, menu.m_hMenu);
	CDonutAddressBarPropertyPage	pageAddress(m_AddressBar, m_SearchBar);
	CDonutFavoritesMenuPropertyPage pageFav;
	CFileNewPropertyPage			pageFileNew;
	CStartUpPropertyPage			pageStartUp;
	CProxyPropertyPage				pageProxy;
	CKeyBoardPropertyPage			pageKeyBoard(m_hAccel, menu.m_hMenu);
	CToolBarPropertyPage			pageToolBar(menu.m_hMenu, &bSkinChange, std::bind(&CDonutToolBar::ReloadSkin, &m_ToolBar));
	CMousePropertyPage				pageMouse(menu.m_hMenu, m_SearchBar.GetSearchEngineMenuHandle());
	CMouseGesturePropertyPage		pageGesture(menu.m_hMenu);
	CSearchPropertyPage 			pageSearch;
	CMenuPropertyPage				pageMenu(menu.m_hMenu, m_CmdBar);
	CRightClickPropertyPage			pageRightMenu(menu);
	CExplorerPropertyPage			pageExplorer;
	CDestroyPropertyPage			pageDestroy;
	CSkinPropertyPage				pageSkin(m_hWnd, &bSkinChange);
	CLinkBarPropertyPage			pageLinks(m_LinkBar);

	CString strURL, strTitle;
	CChildFrame* pChild = GetActiveChildFrame();
	if (pChild) {
		strURL	 = pChild->GetLocationURL();
		strTitle = MtlGetWindowText(pChild->GetHwnd());
	}

	CIgnoredURLsPropertyPage		pageURLs(strURL);
	CCloseTitlesPropertyPage		pageTitles( strTitle );
	CUrlSecurityPropertyPage		pageUrlSecu(strURL);		//+++
	CUserDefinedCSSPropertyPage		pageUserCSS(strURL);
	CUserDefinedJsPropertyPage		pageUserJs(strURL);
	CDonutExecutablePropertyPage	pageExe;
	CDonutConfirmPropertyPage		pageCnf;
	CPluginPropertyPage 			pagePlugin;

	CTreeViewPropertySheet			sheet( _T("Donutのオプション") );

	sheet.AddPage( pageMain				 );
	sheet.AddPage( pageMain2	 , TRUE  );
	sheet.AddPage( pageToolBar			 );
	sheet.AddPage( pageTab		 , TRUE  );
	sheet.AddPage( pageAddress	 , FALSE );
	sheet.AddPage( pageSearch	 , FALSE );
	sheet.AddPage( pageLinks	 , FALSE );
	sheet.AddPage( pageExplorer			 );
	sheet.AddPage( pageMenu				 );
	sheet.AddPage( pageRightMenu , TRUE  );
	sheet.AddPage( pageFav		 , FALSE );
	sheet.AddPage( pageKeyBoard			 );
	sheet.AddPage( pageMouse			 );
	sheet.AddPage( pageGesture	 , TRUE  );
	sheet.AddPage( pageFileNew			 );
	sheet.AddPage( pageStartUp			 );
	sheet.AddPage( pageDestroy			 );
	sheet.AddPage( pageDLC				 );
	sheet.AddPage( pageURLs 	 , TRUE  );
	sheet.AddPage( pageTitles	 , FALSE );
	sheet.AddPage( pageUrlSecu 	 , FALSE );				//+++
	sheet.AddPage( pageUserCSS	 , FALSE );
	sheet.AddPage( pageUserJs	 , FALSE );
	//sheet.AddPage( pageDeterrent	 );

	sheet.AddPage( pageExe			 );
	sheet.AddPage( pageCnf			 );
	sheet.AddPage( pageProxy		 );
	sheet.AddPage( pageSkin 		 );
	sheet.AddPage( pagePlugin		 );

	/* [Donutのオプション]を表示 */
	sheet.DoModal();

	//m_cmbBox.ResetProxyList();

	// キーの呼出
	CAccelerManager accelManager(m_hAccel);
	m_hAccel = accelManager.LoadAccelaratorState(m_hAccel);

	RtlSetMinProcWorkingSetSize();		//+++ (メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等)
}


// Implementation

BOOL CMainFrame::AddSimpleReBarBandCtrl(
	HWND	hWndReBar,
	HWND	hWndBand,
	int 	nID,
	LPTSTR	lpstrTitle,
	UINT	fStyle,
	int 	cxWidth,
	BOOL	bFullWidthAlways,
	HBITMAP hBitmap /* = NULL*/)
{
	dmfTRACE( _T("CMainFrame::AddSimpleReBarBandCtrl\n") );
	ATLASSERT( ::IsWindow(hWndReBar) ); 	// must be already created
	ATLASSERT( ::IsWindow(hWndBand) );		// must be already created
	MTLASSERT_KINDOF(REBARCLASSNAME, hWndReBar);

	// Set band info structure
	REBARBANDINFO rbBand;
	rbBand.cbSize	 = sizeof (REBARBANDINFO);
  #if (_WIN32_IE >= 0x0400)
	rbBand.fMask	 = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE | RBBIM_IDEALSIZE;
  #else
	rbBand.fMask	 = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE;
  #endif	//!(_WIN32_IE >= 0x0400)

	rbBand.fMask	|= RBBIM_BACKGROUND | RBBIM_TEXT;
	rbBand.fStyle	 = fStyle;
	rbBand.lpText	 = lpstrTitle;
	rbBand.hwndChild = hWndBand;
	rbBand.wID		 = nID;

	rbBand.hbmBack	 = hBitmap;
	rbBand.fStyle	|= RBBS_FIXEDBMP;

	// Calculate the size of the band
	BOOL		  bRet;
	RECT		  rcTmp;
	int 		  nBtnCount = (int) ::SendMessage(hWndBand, TB_BUTTONCOUNT, 0, 0L);

	if (nBtnCount > 0) {
		// ツールバーの場合
		bRet				= ::SendMessage(hWndBand, TB_GETITEMRECT, nBtnCount - 1, (LPARAM) &rcTmp) != 0;
		ATLASSERT(bRet);
		rbBand.cx			= (cxWidth != 0) ? cxWidth : rcTmp.right;
		rbBand.cyMinChild	= rcTmp.bottom - rcTmp.top;

		if (bFullWidthAlways) {
			rbBand.cxMinChild = rbBand.cx;
		} else if (lpstrTitle == 0) {
			CRect rcTmp;					// check!!
			bRet			  = ::SendMessage(hWndBand, TB_GETITEMRECT, 0, (LPARAM) &rcTmp) != 0;
			ATLASSERT(bRet);
			rbBand.cxMinChild = rcTmp.right;
		} else {
			rbBand.cxMinChild = 0;
		}
	} else {								// no buttons, either not a toolbar or really has no buttons
		bRet				= ::GetWindowRect(hWndBand, &rcTmp) != 0;
		ATLASSERT(bRet);
		rbBand.cx			= (cxWidth != 0) ? cxWidth : (rcTmp.right - rcTmp.left);
		rbBand.cxMinChild	= (bFullWidthAlways) ? rbBand.cx : 0;
		rbBand.cyMinChild	= rcTmp.bottom - rcTmp.top;
	}

  #if (_WIN32_IE >= 0x0400)
	// NOTE: cxIdeal used for CHEVRON, if MDI cxIdeal changed dynamically.
	rbBand.cxIdeal = rcTmp.right;			//rbBand.cx is not good.
  #endif		//(_WIN32_IE >= 0x0400)

	// Add the band
	LRESULT 	  lRes = ::SendMessage(hWndReBar, RB_INSERTBAND, (WPARAM) -1, (LPARAM) &rbBand);

	if (lRes == 0) {
		ATLTRACE2( atlTraceUI, 0, _T("Failed to add a band to the rebar.\n") );
		return FALSE;
	}

  #if (_WIN32_IE >= 0x0501)
	if (nID == IDC_LINKBAR)
		::SendMessage(hWndBand, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_HIDECLIPPEDBUTTONS);
  #endif		//(_WIN32_IE >= 0x0501)

	return TRUE;
}


// UI map Handler

bool CMainFrame::_IsExistHTMLHelp(void)
{
	return ::PathFileExists( MtlGetHTMLHelpPath() ) != 0;
}

bool CMainFrame::_CheckCookies(UINT nID)
{
	HINSTANCE	hInstDLL = LoadLibrary(_T("wininet.dll"));
	if (hInstDLL == NULL)
		return false;

	DWORD (WINAPI * __PrivacyGetZonePreferenceW)(DWORD, DWORD, LPDWORD, LPWSTR, LPDWORD) = NULL;
	__PrivacyGetZonePreferenceW = ( DWORD (WINAPI *)(DWORD,DWORD,LPDWORD,LPWSTR, LPDWORD) )
									GetProcAddress(hInstDLL,"PrivacyGetZonePreferenceW");
	if (__PrivacyGetZonePreferenceW == NULL) {
		FreeLibrary(hInstDLL);
		return false;
	}

	DWORD	dwCookie1 = 0;
	DWORD	dwCookie2 = 0;
	DWORD	BufLen	  = 0;
	DWORD	ret;
	ret = __PrivacyGetZonePreferenceW(URLZONE_INTERNET, PRIVACY_TYPE_FIRST_PARTY, &dwCookie1, NULL, &BufLen);
	ret = __PrivacyGetZonePreferenceW(URLZONE_INTERNET, PRIVACY_TYPE_THIRD_PARTY, &dwCookie2, NULL, &BufLen);

	bool		bSts		= false;
	switch (nID) {
	case ID_URLACTION_COOKIES_BLOCK:
		if (dwCookie1 == dwCookie2 && dwCookie1 == PRIVACY_TEMPLATE_NO_COOKIES)
			bSts = true;
		break;

	case ID_URLACTION_COOKIES_HI:
		if (dwCookie1 == dwCookie2 && dwCookie1 == PRIVACY_TEMPLATE_HIGH)
			bSts = true;
		break;

	case ID_URLACTION_COOKIES_MIDHI:
		if (dwCookie1 == dwCookie2 && dwCookie1 == PRIVACY_TEMPLATE_MEDIUM_HIGH)
			bSts = true;
		break;

	case ID_URLACTION_COOKIES_MID:
		if (dwCookie1 == dwCookie2 && dwCookie1 == PRIVACY_TEMPLATE_MEDIUM)
			bSts = true;
		break;

	case ID_URLACTION_COOKIES_LOW:
		if (dwCookie1 == dwCookie2 && dwCookie1 == PRIVACY_TEMPLATE_MEDIUM_LOW)
			bSts = true;
		break;

	case ID_URLACTION_COOKIES_ALL:
		if (dwCookie1 == dwCookie2 && dwCookie1 == PRIVACY_TEMPLATE_LOW)
			bSts = true;
		break;

	case ID_URLACTION_COOKIES_CSTM:
		if (dwCookie1 == PRIVACY_TEMPLATE_ADVANCED)
			bSts = true;
		if (dwCookie2 == PRIVACY_TEMPLATE_ADVANCED)
			bSts = true;
		break;
	}
	FreeLibrary(hInstDLL);
	return bSts;
}


void CMainFrame::OnUpdateProgressUI(CCmdUI *pCmdUI)
{
	CProgressBarCtrl progressbar = pCmdUI->m_wndOther;

	progressbar.ShowWindow(SW_HIDE);
}


void CMainFrame::OnUpdateStautsIcon(CCmdUI *pCmdUI)
{
	pCmdUI->m_wndOther.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(pCmdUI->m_nID, -1), 0);
}


bool CMainFrame::_IsClipboardAvailable()
{
	return ::IsClipboardFormatAvailable(MTL_CF_TEXT) == TRUE;		//+++ UNICODE対応(MTL_CF_TEXT)
}


/// 証明書情報表示
LRESULT CMainFrame::OnSecurityReport(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CChildFrame* pChild = GetActiveChildFrame();
	if (pChild == nullptr)
		return 0;

	CComQIPtr<IOleCommandTarget> pct = pChild->GetMarshalIWebBrowser();
	if (pct) {
		pct->Exec(&CGID_ShellDocView, SHDVID_SSLSTATUS, 0, NULL, NULL);
	}
	return 0;
}


//+++ ウィンドウ・サムネール表示(ページ選択)
LRESULT CMainFrame::OnWindowThumbnail(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
#if 0	//:::
  #if 1	//+++ ページがなかったらかえる
	HWND	hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
	if (!hWndActive)
		return 0;
  #endif
  #if 1 //+++	元々コメントだったのを、ためし復活用に#if化.
	CThumbnailDlg	dlg;
	dlg.DoModal(m_hWnd, (LPARAM)m_hWndMDIClient);
  #endif
#endif
	return S_OK;
}


LRESULT CMainFrame::_OnMDIActivate(HWND hWndActive)
{
	//プラグインにタブが変更されたことを伝える
	int 		  nTabIndex;
	IWebBrowser2 *pWB2;

	if (hWndActive == NULL) {
		nTabIndex = -1;
		pWB2	  = NULL;
	} else {
		nTabIndex = m_MDITab.GetTabIndex(hWndActive);
		pWB2	  = DonutGetIWebBrowser2(hWndActive);
	}

	int 		  nCount = CPluginManager::GetCount(PLT_TOOLBAR);

	for (int i = 0; i < nCount; i++) {
		if ( CPluginManager::Call_Event_TabChanged(PLT_TOOLBAR, i, nTabIndex, pWB2) )
			return TRUE;
	}

	return 0;
}

/// アクティブなタブのメニューを表示する
LRESULT CMainFrame::OnShowActiveMenu(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	HWND	hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
	if (!hWndActive)
		return 0;

	int  nIndex 	= m_MDITab.GetTabIndex(hWndActive);
	m_MDITab.ShowTabMenu(nIndex);

	return 0;
}


#ifndef NO_STYLESHEET
//public:
LRESULT CMainFrame::OnUseUserStyleSheet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	bool	bNowFlag = !CStyleSheetOption::GetUseUserSheet();

	CStyleSheetOption::SetUseUserSheet(bNowFlag);
	return 0;
}


//private:
LRESULT CMainFrame::OnSetUserStyleSheet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	bool	bNowFlag = !CStyleSheetOption::s_bSetUserSheet;

	CStyleSheetOption::s_bSetUserSheet = bNowFlag;
	return 0;
}
#endif


LRESULT CMainFrame::OnShowExMenu(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CMenu menu = CExMenuManager::LoadExMenu(CExMenuManager::EXMENU_ID_FIRST);
	POINT pos;

	GetCursorPos(&pos);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pos.x, pos.y, m_hWnd, NULL);
	return 0;
}


CChildFrame *CMainFrame::GetChildFrame(HWND hWndChild)
{
	if ( !::IsWindow(hWndChild) )
		return NULL;

	//成功　ChildFrame ポインタ
	//失敗　NULL((BOOL)FALSE)
	return (CChildFrame *) ::SendMessage(hWndChild, WM_GET_CHILDFRAME, 0, 0);
}


CChildFrame *CMainFrame::GetActiveChildFrame()
{
	HWND	hWnd = m_ChildFrameClient.GetActiveChildFrameWindow();
	if (hWnd)
		return GetChildFrame(hWnd);
	return NULL;
}


//private:

// メインメニューの [表示]-[ツールバー]を表示する
LRESULT CMainFrame::OnShowToolBarMenu()
{
	CPoint pt;
	::GetCursorPos(&pt);
	
	CMenuHandle submenu = m_MainFrameMenu.GetSubMenu(2).GetSubMenu(0);
	if ( submenu.IsMenu() )
		submenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);

	return 0;
}



///+++ メモ:.url に対する拡張プロパティ
LRESULT CMainFrame::OnSetExProperty(LPCTSTR lpstrUrlFile)
{
	CString 		  strUrlFile = lpstrUrlFile;

	if ( !MtlIsExt( strUrlFile, _T(".url") ) )
		return 0;

	CExPropertyDialog dlg(strUrlFile);
	dlg.DoModal();

	return 0;
}


// ==========================================================================
// 戻る・進む

BOOL CMainFrame::_Load_OptionalData(CChildFrame *pChild, const CString &strFileName, CString &strSection)
{
	//:::pChild->SetDfgFileNameSection(strFileName, strSection);
	return TRUE;
}


BOOL CMainFrame::_Load_OptionalData2(CChildFrame *pChild,
						 std::vector<std::pair<CString, CString> > &ArrayFore,
						 std::vector<std::pair<CString, CString> > &ArrayBack)
{
	//:::pChild->SetArrayHist( ArrayFore, ArrayBack );
	return TRUE;
}



LRESULT CMainFrame::OnMenuRecentDocument(HMENU hMenu)
{
	CMenuHandle menu = hMenu;
	if (menu.m_hMenu == NULL)
		return 0;

	if (menu.GetMenuItemCount() == 0)
		menu.AppendMenu(MF_ENABLED | MF_STRING, ID_RECENTDOCUMENT_FIRST, (LPCTSTR) NULL);
	else if (menu.GetMenuItemID(0) != ID_RECENTDOCUMENT_FIRST)
		return 0;

	HMENU	hMenuT = m_RecentClosedTabList.GetMenuHandle();
	m_RecentClosedTabList.SetMenuHandle(menu);
	m_RecentClosedTabList.UpdateMenu();
	m_RecentClosedTabList.SetMenuHandle(hMenuT);

	return TRUE;
}


LRESULT CMainFrame::OnMenuRecentLast(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	return ::SendMessage(m_hWnd, WM_COMMAND, ID_RECENTDOCUMENT_FIRST, 0);
}


int CMainFrame::_GetRecentCount()
{
	return m_RecentClosedTabList.GetRecentCount();
}



// ==========================================================================
// タブ関係


LRESULT CMainFrame::OnMouseWheel(UINT fwKeys, short zDelta, CPoint point)
{
	// I don't have a wheel mouse...
	if (  CTabBarOption::s_bWheel
	   && MtlIsBandVisible(m_hWndToolBar, IDC_MDITAB) )
	{
		CRect rcTab;
		m_MDITab.GetWindowRect(&rcTab);

		if ( rcTab.PtInRect(point) ) {
			if (zDelta > 0) {
				m_MDITab.LeftTab();
			} else {
				m_MDITab.RightTab();
			}

			SetMsgHandled(TRUE);
			return 0;
		}
	}

	// 右クリックされていたら - スクロールのあるビュー上だとビューがスクロールされるバグ
	if (fwKeys == VK_RBUTTON) {
		if (zDelta > 0) {
			m_MDITab.LeftTab();
		} else {
			m_MDITab.RightTab();
		}

		SetMsgHandled(TRUE);
		return 0;
	}

	SetMsgHandled(FALSE);
	return 1;
}



LRESULT CMainFrame::OnViewTabBarMulti(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CTabBarOption::s_bMultiLine = !CTabBarOption::s_bMultiLine;
	CTabBarOption::WriteProfile();

	m_MDITab.ReloadSkin();
	return 0;
}




LRESULT CMainFrame::OnTabListDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
#if 0	//:::
	CPoint pos;
	::GetCursorPos(&pos);

	int    nRet = m_MDITab.ShowTabListMenuDefault(pos.x, pos.y);

	if (nRet == -1)
		return 0;

	HWND   hWnd = m_MDITab.GetTabHwnd(nRet);

	if ( ::IsWindow(hWnd) )
		::SendMessage(m_wndMDIClient, WM_MDIACTIVATE, (WPARAM) hWnd, 0);
#endif
	return 0;
}


LRESULT CMainFrame::OnTabListVisible(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
#if 0	//:::
	CPoint pos;
	::GetCursorPos(&pos);

	int    nRet = m_MDITab.ShowTabListMenuVisible(pos.x, pos.y);

	if (nRet == -1)
		return 0;

	HWND   hWnd = m_MDITab.GetTabHwnd(nRet);

	if ( ::IsWindow(hWnd) )
		::SendMessage(m_wndMDIClient, WM_MDIACTIVATE, (WPARAM) hWnd, 0);
#endif
	return 0;
}


LRESULT CMainFrame::OnTabListAlphabet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
#if 0	//:::
	CPoint pos;
	::GetCursorPos(&pos);

	int    nRet = m_MDITab.ShowTabListMenuAlphabet(pos.x, pos.y);

	if (nRet == -1)
		return 0;

	HWND   hWnd = m_MDITab.GetTabHwnd(nRet);

	if ( ::IsWindow(hWnd) )
		::SendMessage(m_wndMDIClient, WM_MDIACTIVATE, (WPARAM) hWnd, 0);
#endif
	return 0;
}



// ==========================================================================
// 窓サイズ関係


#if 1	//+++
#if 1	//+++ 失敗のごまかし
void CMainFrame::ShowWindow_Restore(bool flag)
{
	int nShow = flag ? SW_RESTORE : SW_SHOW;
	ShowWindow( nShow );
}
#else	//失敗
void CMainFrame::ShowWindow_Restore(bool)
{
	int nShow = SW_SHOWNORMAL;	//SW_RESTORE;
	if (m_bMinimized)
		nShow = SW_SHOWMINIMIZED;
	if (m_bOldMaximized)		//+++ m_bFullScreen,m_bOldMaximizedが正しく設定されているとして処理.
		nShow = SW_SHOWMAXIMIZED;
	ShowWindow( nShow );
}
#endif
#endif


LRESULT CMainFrame::OnMainFrameMinimize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	ShowWindow(SW_MINIMIZE);
	//m_bOldMaximized = 0;
	// //m_bMinimized    = 1;
	RtlSetMinProcWorkingSetSize();		//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
	return 0;
}


LRESULT CMainFrame::OnMainFrameMaximize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	ShowWindow(SW_MAXIMIZE);
	m_bOldMaximized = 1;
	// //m_bMinimized    = 0;
	RtlSetMinProcWorkingSetSize();		//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
	return 0;
}


LRESULT CMainFrame::OnMainFrameNormMaxSize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	if (m_bOldMaximized) {
		//ShowWindow(SW_RESTORE);
		ShowWindow_Restore(1);	//ShowWindow(SW_RESTORE);		//サイズを戻す
		m_bOldMaximized = 0;
	} else {
		ShowWindow(SW_MAXIMIZE);
		m_bOldMaximized = 1;
	}
	RtlSetMinProcWorkingSetSize();		//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
	return 0;
}


// ==================================================
// フルスクリーン関係

//+++ 起動初回の窓サイズ設定.
void CMainFrame::startupMainFrameStayle(int nCmdShow, bool bTray)
{
	if ((nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_SHOWMINNOACTIVE)
		&& (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MINBTN2TRAY))		//+++ .lnkとかで最小化で、起動されたとき、トレイに入る設定ならトレイへ.
	{
		bTray    = true;
		nCmdShow = SW_SHOWNORMAL;		// うまくいかない...が、残す...
	}
	CIniFileI  pr( g_szIniFileName, _T("Main") );
	int wndSML	= MtlGetProfileMainFrameState(pr, *this, nCmdShow);				//+++ s,m,l,fullを判別しておく(暫定版)
	pr.Close();
	m_bOldMaximized 	= 0;
	if (wndSML == 2)
		m_bOldMaximized = 1;

	if (bTray) {
		OnGetOut(0,0,0);
	} else if (CMainOption::s_dwMainExtendedStyle & MAIN_EX_FULLSCREEN) {
		_FullScreen(TRUE);
	}
}


#if 0
bool CMainFrame::IsFullScreen()
{
  #if 1 //+++
	return m_bFullScreen;
  #else
	return (GetStyle() & WS_CAPTION) == 0;
  #endif
}
#endif



void CMainFrame::OnViewFullScreen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (IsFullScreen() == 0)			//+++
	{
		_FullScreen(TRUE);
	} else {
		_FullScreen(FALSE);
	}
}



void CMainFrame::_FullScreen(BOOL bOn)
{
	//static bool m_bOldMaximized = 0;
	_ShowBandsFullScreen(bOn);

	SetRedraw(FALSE);

	if (bOn) {							// remove caption
		// save prev visible
		CWindowPlacement	wndpl;
		GetWindowPlacement(&wndpl);

		{
			CIniFileO	pr( g_szIniFileName, _T("Main") );
			wndpl.WriteProfile(pr, _T("frame."));
			//x pr.Close();
		}
		
		m_bFullScreen = true;

		//m_bMinimized		= 0;
		m_bOldMaximized 	= (wndpl.showCmd == SW_SHOWMAXIMIZED);
		ModifyStyle(WS_CAPTION, 0);

		SetMenu(NULL);
		ShowWindow(SW_HIDE);

		m_mcToolBar.m_bVisible = true;

		ShowWindow(SW_MAXIMIZE);
		if (m_bOldMaximized == 0) {
			SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
		
	} else {
		m_bFullScreen = false;

		m_mcToolBar.m_bVisible = false;
		// restore prev visible
	  #if 1 //+++	キャプションの着けはづしがありの場合
		if (CSkinOption::s_nMainFrameCaption)
			ModifyStyle(0, WS_CAPTION);
	  #else
		ModifyStyle(0, WS_CAPTION);
	  #endif
		if (m_bOldMaximized == 0) {
			ShowWindow_Restore(1);		//ShowWindow(SW_RESTORE);		//+++ サイズを戻す
		} else {
			SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}
	SetRedraw(TRUE);
	RedrawWindow(NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
	if (UxTheme_Wrap::IsCompositionActive() == FALSE) {
		::RedrawWindow(GetDesktopWindow(), NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
		HWND hWndTray = ::FindWindow(_T("Shell_TrayWnd"), NULL);
		if (hWndTray)
			::RedrawWindow(hWndTray, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}
	UpdateLayout(TRUE);
}



void CMainFrame::_ShowBandsFullScreen(BOOL bOn, bool bInit /*= false*/)
{
	static BOOL 	s_bOldVisibleStatusBar;
	int 			nIndex = 0;
	if (bOn) { // remove caption
		// save prev visible
		int 		nToolbarPluginCount = CPluginManager::GetCount(PLT_TOOLBAR);
		m_mapToolbarOldVisible.RemoveAll();

		for (nIndex = 0; nIndex < _countof(STDBAR_ID); nIndex++) {
			m_mapToolbarOldVisible.Add( STDBAR_ID[nIndex], MtlIsBandVisible( m_hWndToolBar, STDBAR_ID[nIndex] ) );
		}
		for (nIndex = 0; nIndex < nToolbarPluginCount; nIndex++) {
			m_mapToolbarOldVisible.Add( IDC_PLUGIN_TOOLBAR + nIndex,
										MtlIsBandVisible( m_hWndToolBar, IDC_PLUGIN_TOOLBAR + nIndex ) );
		}
		s_bOldVisibleStatusBar = ::IsWindowVisible(m_hWndStatusBar) != 0;
		SetRedraw(FALSE);
		{
			CIniFileI pr( g_szIniFileName, _T("FullScreen") );
			MtlShowBand(m_hWndToolBar, ATL_IDW_COMMAND_BAR	, pr.GetValue(_T("ShowMenu")	, FALSE) != 0);
			MtlShowBand(m_hWndToolBar, ATL_IDW_TOOLBAR		, pr.GetValue(_T("ShowToolBar") , FALSE) != 0);
			MtlShowBand(m_hWndToolBar, IDC_ADDRESSBAR		, pr.GetValue(_T("ShowAdress")	, FALSE) != 0);
			MtlShowBand(m_hWndToolBar, IDC_MDITAB			, pr.GetValue(_T("ShowTab") 	, FALSE) != 0);
			MtlShowBand(m_hWndToolBar, IDC_LINKBAR			, pr.GetValue(_T("ShowLink")	, FALSE) != 0);
			MtlShowBand(m_hWndToolBar, IDC_SEARCHBAR		, pr.GetValue(_T("ShowSearch")	, FALSE) != 0);
			if (pr.GetValue(_T("ShowStatus"), FALSE ) == FALSE)
				::ShowWindow(m_hWndStatusBar, SW_HIDE);
			else
				::ShowWindow(m_hWndStatusBar, SW_SHOWNOACTIVATE);
		}
		for (nIndex = 0; nIndex < nToolbarPluginCount; nIndex++)
			MtlShowBand(m_hWndToolBar, IDC_PLUGIN_TOOLBAR + nIndex, FALSE);
	} else {
		SetRedraw(FALSE);
		for (nIndex = 0; nIndex < m_mapToolbarOldVisible.GetSize(); nIndex++)
			MtlShowBand( m_hWndToolBar, m_mapToolbarOldVisible.GetKeyAt(nIndex), m_mapToolbarOldVisible.GetValueAt(nIndex) );

		::ShowWindow(m_hWndStatusBar, s_bOldVisibleStatusBar ? SW_SHOWNOACTIVATE : SW_HIDE);
	}
	SetRedraw(TRUE);
}



// ==================================================
// トレイアイコン関係


// UDT DGSTR
void CMainFrame::OnGetOut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	static int nShow = SW_SHOWNORMAL;

	if ( IsWindowVisible() ) {
		//+++ メモ:窓状態のときはトレイ化
		m_bTray = true; 		//+++
		WINDOWPLACEMENT wp;
		wp.length = sizeof (WINDOWPLACEMENT);
		::GetWindowPlacement(m_hWnd, &wp);
		nShow		  = wp.showCmd;
	  #if 1 //+++ フルスクリーン以外なら今が最大かどうかを控える
		if (m_bFullScreen == 0)
			m_bOldMaximized = (nShow == SW_MAXIMIZE);
	  #endif
		SetHideTrayIcon();	//+++ トレイアイコン化
		Sleep(100); 		// UDT TODO
	} else {				// just db F9 press , come here :p
		m_bTray      = false;	//+++
		//m_bMinimized = 0;
		ShowWindow_Restore(0);	//x ShowWindow( SW_SHOW /*nShow*/ );

		//+++ TrayMessage(m_hWnd, NIM_DELETE, TM_TRAY, 0, NULL);
		DeleteTrayIcon();	//+++ トレイアイコン削除
	}
}
// ENDE



// UDT DGSTR ( hide window & display icon )
LRESULT CMainFrame::OnMyNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
{
	if ( IsWindowVisible() ) {
		return -1;
	} else {
		switch (lParam) {
		case WM_LBUTTONUP:
			ShowWindow_Restore(0);	//ShowWindow(SW_SHOW);
			DeleteTrayIcon();	//+++
			return 0;
			break;

		case WM_RBUTTONUP:
		  #if 1	//+++
			{
				::SetForegroundWindow(m_hWnd);
				CMenu/*Handle*/ 	menu0;
				menu0.LoadMenu(IDR_TRAYICON_MENU);
				if (menu0.m_hMenu == NULL)
					return 0;
				CMenuHandle menu = menu0.GetSubMenu(0);
				if (menu.m_hMenu == NULL)
					return 0;

				// ポップアップメニューを開く.
				POINT 	pt;
				::GetCursorPos(&pt);
				HRESULT hr = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON| TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
				if (hr == 57666/*復帰*/) {
					ShowWindow_Restore(0);	//ShowWindow(SW_SHOW);
					DeleteTrayIcon();	//+++
					return 0;
				}
				if (hr == 57665/*終了*/) {
					DeleteTrayIcon();	//+++
					PostMessage(WM_CLOSE, 0, 0);
					break;
				}
			}
		  #else
			DeleteTrayIcon();	//+++
			PostMessage(WM_CLOSE, 0, 0);
			break;
		  #endif
		}

		return -1;
	}
}
// ENDE



#if 1 //+++ トレイアイコンの設定.
void CMainFrame::SetHideTrayIcon()
{
  #if 1 //+++
	RtlSetMinProcWorkingSetSize();		//+++ (メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等)
	HICON hIcon = 0;
	if (Misc::IsExistFile(m_strIconSm))
		hIcon	= (HICON)::LoadImage(ModuleHelper::GetResourceInstance(), m_strIconSm, IMAGE_ICON, 16, 16, LR_SHARED|LR_LOADFROMFILE);
	if (hIcon == 0)
		hIcon = LoadIcon( _Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME) );
	TrayMessage( m_hWnd, NIM_ADD, TM_TRAY, hIcon, DONUT_NAME/*_T("unDonut")*/ );		//+++
  #else //元
	//x TrayMessage( m_hWnd, NIM_ADD, TM_TRAY, IDR_MAINFRAME, DONUT_NAME/*_T("unDonut")*/ );
  #endif
	ShowWindow(SW_HIDE);
}
#endif


//+++ トレイ化の終了/トレイアイコンの削除.
void CMainFrame::DeleteTrayIcon()
{
	TrayMessage(m_hWnd, NIM_DELETE, TM_TRAY, 0, NULL);
	//x m_bTrayFlag = false;

	RtlSetMinProcWorkingSetSize();		//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
}




// ===========================================================================
// エクスプローラバー

// UDT DGSTR
LRESULT CMainFrame::UpdateExpBar(LPCTSTR lpszExpBar, DWORD dwReserved)
{
	//if(lpszExpBar == NULL) return 0;
	m_ExplorerBar.SetTitle(lpszExpBar);
	return 0;
}
// ENDE


//minit
LRESULT CMainFrame::OnRefreshExpBar(int nType)
{
	m_ExplorerBar.RefreshExpBar(nType);
	return 0;
}


// UDT DGSTR //Update minit
LRESULT CMainFrame::OnFavoriteExpBar(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	m_ExplorerBar.ShowBar(true);
	//m_ExplorerBar.Show();
	MtlSendCommand(m_ExplorerBar.m_FavBar.m_hWnd, wID);
	m_wndSplit.SetSinglePaneMode();
	return 0;
}


void CMainFrame::OnFileNewClipBoardEx(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_ExplorerBar.m_ClipBar.OpenClipboardUrl();
}



LRESULT CMainFrame::OnSelectUserFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlSendCommand(m_ExplorerBar.m_FavBar.m_hWnd, ID_SELECT_USERFOLDER);
	return 0;
}



// ===========================================================================
// アドレスバー


LRESULT CMainFrame::OnViewGoButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	// トグル
	m_AddressBar.ShowGoButton(!CAddressBarOption::s_bGoBtnVisible);

	return 0;
}


LRESULT CMainFrame::OnViewAddBarDropDown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	if ( m_AddressBar.GetDroppedStateEx() )
		m_AddressBar.ShowDropDown(FALSE);
	else
		m_AddressBar.ShowDropDown(TRUE);

	return 0;
}


void CMainFrame::OnSetFocusAddressBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl)
{
	::SetFocus( m_AddressBar.GetEditCtrl() );
}





// ===========================================================================
// 検索

CString CMainFrame::GetActiveSelectedText()
{
	HWND hWnd = m_ChildFrameClient.GetActiveChildFrameWindow();
	if (hWnd == nullptr)
		return CString();

	LPCTSTR str = nullptr;
	::SendMessage(hWnd, WM_GETSELECTEDTEXT, (WPARAM)&str, 0);
	ATLASSERT(str);
	CString strSelectedText = str;
	delete str;
	return strSelectedText;
}


LRESULT CMainFrame::OnCommandDirect(int nCommand, LPCTSTR lpstr)
{
	CString str(lpstr);

	if (str.Find( _T("tp://") ) != -1
	  #if 1	//+++
		|| str.Find( _T("https://") ) != -1
		|| str.Find( _T("file://") ) != -1
//|| str.Left(11) == _T("javascript:")		//*+++ 実験
	  #endif
	){
		Misc::StrToNormalUrl(str);		//+++
		DonutOpenFile( str );
	} else {
		if (nCommand == ID_SEARCH_DIRECT) {				// 即検索
			CEdit edit = m_SearchBar.GetEditCtrl();
			edit.SendMessage(WM_CHAR, 'P');
			edit.SetWindowText(lpstr);
			m_SearchBar.SearchWeb();
		} else if (nCommand == ID_OPENLINK_DIRECT) {	// リンクを開く
			SendMessage(WM_COMMAND, ID_EDIT_OPEN_SELECTED_REF, 0);
		}
	}

	return 0;
}



LRESULT CMainFrame::OnSearchBarCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	HWND	hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();

	if (hWndActive == NULL)
		return 0;

	CEdit	edit	   = m_SearchBar.GetEditCtrl();
	CString str;	//+++   = MtlGetWindowText(edit);
	str = _GetSelectText(edit);

	if ( str.IsEmpty() )
		return 0;

	switch (wID) {
	case ID_SEARCHBAR_SEL_UP:
		m_SearchBar.SearchPage(FALSE);
		//SendMessage(hWndActive, WM_USER_FIND_KEYWORD, (WPARAM)str.GetBuffer(0), (LPARAM)FALSE);
		break;

	case ID_SEARCHBAR_SEL_DOWN:
		m_SearchBar.SearchPage(TRUE);
		//SendMessage(hWndActive, WM_USER_FIND_KEYWORD, (WPARAM)str.GetBuffer(0), (LPARAM)TRUE);
		break;

	case ID_SEARCHBAR_HILIGHT:
		m_SearchBar.SearchHilight();
		//SendMessage(hWndActive, WM_USER_HILIGHT, (WPARAM)str.GetBuffer(0), (LPARAM)0);
		break;
	}

	return 0;
}


LRESULT CMainFrame::OnHilight(LPCTSTR lpszKeyWord)
{
	HWND hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();

	if (hWndActive == NULL)
		return 0;

	return SendMessage(hWndActive, WM_USER_HILIGHT, (WPARAM) lpszKeyWord, 0);
}

/// ※今のところFlags指定は意味ない
LRESULT CMainFrame::OnFindKeyWord(LPCTSTR lpszKeyWord, BOOL bBack, long Flags /*= 0*/)
{
	HWND hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();

	if (hWndActive == NULL)
		return 0;

	return SendMessage(hWndActive, WM_USER_FIND_KEYWORD, (WPARAM) lpszKeyWord, (LPARAM) bBack);
}


// U.H
LRESULT CMainFrame::OnViewSearchBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlToggleBandVisible(m_hWndToolBar, IDC_SEARCHBAR);
	UpdateLayout();
	return 0;
}


LRESULT CMainFrame::OnSearchHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlSendCommand(m_ExplorerBar.m_FavBar.m_hWnd, ID_SEARCH_HISTORY);
	return 0;
}


void CMainFrame::OnSetFocusSearchBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl)
{
	::SetFocus( m_SearchBar.GetEditCtrl() );
}


void CMainFrame::OnSetFocusSearchBarEngine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl)
{ //minit
	m_SearchBar.SetFocusToEngine();
}


LRESULT CMainFrame::OnSpecialRefreshSearchEngine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	m_SearchBar.RefreshEngine();
	return 0;
}

/// 範囲選択されたテキストで検索する？
HRESULT CMainFrame::OnSearchWebSelText(LPCTSTR lpstrText, LPCTSTR lpstrEngine)
{
	//BOOL	bFirst	 = (DonutGetStdOpenCreateFlag() & D_OPENFILE_ACTIVATE) != 0;	//+++ ? TRUE : FALSE;

	m_SearchBar.SearchWebWithEngine(lpstrText, lpstrEngine);

	return S_OK;
}


//+++
/// グループ・メニューの検索エンジン指定での検索
void CMainFrame::OnSearchWeb_engineId(UINT code, int id, HWND hWnd)
{
	ATLASSERT(ID_INSERTPOINT_SEARCHENGINE <= id && id <= ID_INSERTPOINT_SEARCHENGINE_END);

	m_SearchBar.SearchWebWithIndex(GetActiveSelectedText(), id - ID_INSERTPOINT_SEARCHENGINE);
}



// ==================================================================================

/////////////////////////////////////////////
// DonutP API
/////////////////////////////////////////////


IDispatch *CMainFrame::ApiGetDocumentObject(int nTabIndex)
{
	HWND		 hTabWnd = m_MDITab.GetTabHwnd(nTabIndex);
	if (hTabWnd == NULL)
		return NULL;

	CChildFrame* pChild = GetChildFrame(hTabWnd);
	if (pChild == nullptr)
		return nullptr;

	CComPtr<IWebBrowser2>	spBrwoser = pChild->GetMarshalIWebBrowser();
	IDispatch*  spDocument = nullptr;
	spBrwoser->get_Document(&spDocument);
	return spDocument;
}


IDispatch *CMainFrame::ApiGetWindowObject(int nTabIndex)
{
	HWND					  hTabWnd = m_MDITab.GetTabHwnd(nTabIndex);
	if (hTabWnd == NULL)
		return NULL;

	CChildFrame* pChild = GetChildFrame(hTabWnd);
	if (pChild == nullptr)
		return nullptr;

	CComPtr<IWebBrowser2>	spBrwoser = pChild->GetMarshalIWebBrowser();

	CComPtr<IDispatch>		  spDisp  = 0;
	HRESULT 				  hr	  = spBrwoser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2> spDoc   = spDisp;

	IHTMLWindow2*			  spWnd   = 0;
	spDoc->get_parentWindow(&spWnd);
	return (IDispatch*)spWnd;
}


IDispatch *CMainFrame::ApiGetWebBrowserObject(int nTabIndex)
{
	HWND		  hTabWnd	= m_MDITab.GetTabHwnd(nTabIndex);
	if (hTabWnd == NULL)
		return NULL;

	CChildFrame* pChild = GetChildFrame(hTabWnd);
	if (pChild == nullptr)
		return nullptr;

	CComPtr<IWebBrowser2>	spBrwoser = pChild->GetMarshalIWebBrowser();
	IDispatch*	pDisp = nullptr;
	spBrwoser->QueryInterface(&pDisp);
	return (IDispatch *) pDisp;
}


long CMainFrame::ApiGetTabIndex()
{
	return m_MDITab.GetCurSel();
}


void CMainFrame::ApiSetTabIndex(int nTabIndex)
{
	m_MDITab.SetCurSel(nTabIndex);
}


void CMainFrame::ApiClose(int nTabIndex)
{
	HWND hTabWnd = m_MDITab.GetTabHwnd(nTabIndex);
	if (hTabWnd)
		::SendMessage(hTabWnd, WM_COMMAND, ID_FILE_CLOSE, 0);
}


long CMainFrame::ApiGetTabCount()
{
	return m_MDITab.GetItemCount();
}


void CMainFrame::ApiMoveToTab(int nBeforIndex, int nAfterIndex)
{
	CSimpleArray<int> aryBefor;
	aryBefor.Add(nBeforIndex);
	m_MDITab.MoveItems(nAfterIndex + 1, aryBefor);
}


int CMainFrame::ApiNewWindow(BSTR bstrURL, BOOL bActive)
{
	CString 	 strURL(bstrURL);
	
	if (m_MDITab.GetItemCount() == 0)
		bActive = TRUE;
	UserOpenFile(bstrURL, bActive ? D_OPENFILE_ACTIVATE : 0);

	return -1;	//\\ インデックスは分からない
}


void CMainFrame::ApiShowPanelBar()
{
	if ( m_ExplorerBar.IsPanelBarVisible() )
		return;

	SendMessage(WM_COMMAND, ID_VIEW_PANELBAR, 0);
}


long CMainFrame::ApiGetTabState(int nIndex)
{
	DWORD state = 0;
	m_MDITab.GetItemState(nIndex, state);
	if (state == 0)
		return -1;

	long nRet	 = 0;

	if (state & TISS_SELECTED) {
		nRet = 1;
	} else if (state & TISS_MSELECTED) {
		nRet = 2;
	}

	return nRet;
}


IDispatch *CMainFrame::ApiGetPanelWebBrowserObject()
{
	if ( !m_ExplorerBar.m_PanelBar.IsWindow() )
		m_ExplorerBar.m_PanelBar.CreatePanelBar(m_hWnd, FALSE);

	return m_ExplorerBar.m_PanelBar.GetPanelWebBrowserObject();
}


IDispatch *CMainFrame::ApiGetPanelWindowObject()
{
	if ( !m_ExplorerBar.m_PanelBar.IsWindow() )
		m_ExplorerBar.m_PanelBar.CreatePanelBar(m_hWnd, FALSE);

	return m_ExplorerBar.m_PanelBar.GetPanelWindowObject();
}


IDispatch *CMainFrame::ApiGetPanelDocumentObject()
{
	if ( !m_ExplorerBar.m_PanelBar.IsWindow() )
		m_ExplorerBar.m_PanelBar.CreatePanelBar(m_hWnd, FALSE);

	return m_ExplorerBar.m_PanelBar.GetPanelDocumentObject();
}


//IAPI2 by minit
void CMainFrame::ApiExecuteCommand(int nCommand)
{
	::SendMessage(m_hWnd, WM_COMMAND, (WPARAM) (nCommand & 0xFFFF), 0);
}


void CMainFrame::ApiGetSearchText( /*[out, retval]*/ BSTR *bstrText)
{
	CString strBuf;
	CEdit	edit = m_SearchBar.GetEditCtrl();
	int 	len  = edit.GetWindowTextLength() + 1;

	edit.GetWindowText(strBuf.GetBuffer(len), len);
	strBuf.ReleaseBuffer();
	*bstrText = CComBSTR(strBuf).Copy();
}


void CMainFrame::ApiSetSearchText(BSTR bstrText)
{
	CString strText = bstrText;
	m_SearchBar.SetSearchStr(strText);
}


void CMainFrame::ApiGetAddressText( /*[out, retval]*/ BSTR *bstrText)
{
	CString strBuf;
	CEdit	edit = m_AddressBar.GetEditCtrl();
	int 	len  = edit.GetWindowTextLength() + 1;

	edit.GetWindowText(strBuf.GetBuffer(len), len);
	strBuf.ReleaseBuffer();
	*bstrText = CComBSTR(strBuf).Copy();
}


void CMainFrame::ApiSetAddressText(BSTR bstrText)
{
	CString strText = bstrText;
	m_AddressBar.SetWindowText(strText);
}


LRESULT CMainFrame::ApiGetExtendedTabState(int nIndex)
{
	HWND hWndChild = m_MDITab.GetTabHwnd(nIndex);

	if ( !::IsWindow(hWndChild) )
		return 0x80000000;

	return ::SendMessage(hWndChild, WM_GET_EXTENDED_TABSTYLE, 0, 0);
}


void CMainFrame::ApiSetExtendedTabState(int nIndex, long nState)
{
	HWND hWndChild = m_MDITab.GetTabHwnd(nIndex);

	if ( !::IsWindow(hWndChild) )
		return;

	::SendMessage(hWndChild, WM_SET_EXTENDED_TABSTYLE, (WPARAM) nState, 0);
}


LRESULT CMainFrame::ApiGetKeyState(int nKey)
{
	//return (::GetAsyncKeyState(nKey) & 0x80000000) != 0;		//+++ これだと、既存スクリプトで不具合でることあるかもなんで、返型のほうを現状に合わせた.
	return (::GetAsyncKeyState(nKey) & 0x80000000);
}


long CMainFrame::ApiGetProfileInt(BSTR bstrFile, BSTR bstrSection, BSTR bstrKey, int nDefault)
{
	CString 	strFile    = bstrFile;
	CString 	strSection = bstrSection;
	CString 	strKey	   = bstrKey;
	CIniFileI	pr(strFile, strSection);
	return (long)pr.GetValue( strKey, nDefault );
}


void CMainFrame::ApiWriteProfileInt(BSTR bstrFile, BSTR bstrSection, BSTR bstrKey, int nValue)
{
	CString 	strFile    = bstrFile;
	CString 	strSection = bstrSection;
	CString 	strKey	   = bstrKey;
	CIniFileO	pr(strFile, strSection);
	pr.SetValue(nValue, strKey);
	//x pr.Close(); 	//+++
}


void CMainFrame::ApiGetProfileString(BSTR bstrFile, BSTR bstrSection, BSTR bstrKey, BSTR bstrDefault, /*[out, retval]*/ BSTR *bstrText)
{
	CString 	strFile 	= bstrFile;
	CString 	strSection	= bstrSection;
	CString 	strKey		= bstrKey;
	CString 	strDefault	= bstrDefault;
	CIniFileI	pr(strFile, strSection);
	CString 	strBuf		= pr.GetStringUW(strKey, strDefault, 0xFFFFFFFF); //+++ size=0xFFFFFFFFの場合はバッファサイズ拡張ありで取得.
	*bstrText = CComBSTR(strBuf).Copy();
}


void CMainFrame::ApiWriteProfileString(BSTR bstrFile, BSTR bstrSection, BSTR bstrKey, BSTR bstrText)
{
	CString 		strFile    = bstrFile;
	CString 		strSection = bstrSection;
	CString 		strKey	   = bstrKey;
	CString 		strText    = bstrText;
	CIniFileO	pr(strFile, strSection);
	pr.SetStringUW( strText, strKey );
}


void CMainFrame::ApiGetScriptFolder( /*[out, retval]*/ BSTR *bstrFolder)
{
	ATLASSERT(bstrFolder != 0);
	CString 	strBuf = Misc::GetExeDirectory() + _T("Script\\");
	if (::GetFileAttributes(strBuf) == 0xFFFFFFFF)
		strBuf.Empty();

	*bstrFolder = CComBSTR(strBuf).Copy();
}


void CMainFrame::ApiGetCSSFolder( /*[out, retval]*/ BSTR *bstrFolder)
{
	ATLASSERT(bstrFolder != 0);
	CString 	strBuf = Misc::GetExeDirectory() + _T("CSS\\");
	if (::GetFileAttributes(strBuf) == 0xFFFFFFFF)
		strBuf.Empty();

	*bstrFolder = CComBSTR(strBuf).Copy();
}


void CMainFrame::ApiGetBaseFolder( /*[out, retval]*/ BSTR *bstrFolder)
{
	ATLASSERT(bstrFolder != 0);
	CString 	strBuf = Misc::GetExeDirectory();
	if (::GetFileAttributes(strBuf) == 0xFFFFFFFF)
		strBuf.Empty();

	*bstrFolder = CComBSTR(strBuf).Copy();
}


void CMainFrame::ApiGetExePath( /*[out, retval]*/ BSTR *bstrPath)
{
	ATLASSERT(bstrPath != 0);
	TCHAR Buf[MAX_PATH];
	Buf[0]		= 0;	//+++
	::GetModuleFileName(_Module.GetModuleInstance(), Buf, MAX_PATH);
	*bstrPath 	= CComBSTR(Buf).Copy();
}


void CMainFrame::ApiSetStyleSheet(int nIndex, BSTR bstrStyleSheet, BOOL bOff)
{
#if 0	//:::
	HWND		 hWndChild = m_MDITab.GetTabHwnd(nIndex);

	if ( !::IsWindow(hWndChild) )
		return;

	CChildFrame *pChild    = GetChildFrame(hWndChild);
	if (pChild == NULL)
		return;

	CString 	 strStylePath(bstrStyleSheet);
	CString 	 strStyleTitle;

	if ( bOff || strStylePath.IsEmpty() ) {
		// Off or Default
	} else {
		// Set New StyleSheet
		strStyleTitle = strStylePath.Mid(strStylePath.ReverseFind('\\') + 1);
	}

	pChild->StyleSheet(strStyleTitle, bOff, strStylePath);
#endif
}


//IAPI3
void CMainFrame::ApiSaveGroup(BSTR bstrGroupFile)
{
	CString strFile = bstrGroupFile;

	_SaveGroupOption(strFile);
}


void CMainFrame::ApiLoadGroup(BSTR bstrGroupFile, BOOL bClose)
{
	CString strFile = bstrGroupFile;

	_LoadGroupOption(strFile, bClose != 0/*? true : false*/);
}


//private:
///+++ プラグイン側から渡されるiniファイル情報...
struct CMainFrame::_ExtendProfileInfo {
	/*const*/LPCTSTR	lpstrIniPath;
	/*const*/LPCTSTR	lpstrSection;
	BOOL			  	bGroup;
};


//public:
int CMainFrame::ApiNewWindow3(BSTR bstrURL, BOOL bActive, long ExStyle, void *pExInfo)
{
	CString 	 strURL   = bstrURL;

	if (m_MDITab.GetItemCount() == 0)
		bActive = TRUE;

	UserOpenFile(strURL, bActive ? D_OPENFILE_ACTIVATE : 0, -1, (int)ExStyle);
#if 0
	CChildFrame *pChild   = CChildFrame::NewWindow(m_hWndMDIClient, m_MDITab, m_AddressBar);

	if (pChild == NULL)
		return -1;

	pChild->ActivateFrame(nCmdShow);

	if ( strURL.IsEmpty() )
		strURL = _T("about:blank");

	if (pExInfo && *(INT_PTR *) pExInfo) {
		_ExtendProfileInfo *_pExInfo    = (_ExtendProfileInfo *) pExInfo;
	  #ifdef UNICODE
		const char*			s			= (const char*)_pExInfo->lpstrIniPath;
		const char*			t			= (const char*)_pExInfo->lpstrSection;
		CString 			strPath;
		CString 			strSection;
		if (*s && s[1] == 0) {	//+++ パス名の最初はほぼ半角だろうで、かつ、1バイトのpathはないだろうとして、utf16と判断.
			strPath     = (TCHAR*)s;
			strSection  = (TCHAR*)t;
		} else {				//+++ プラグイン側がMB版でコンパイルされている場合を考慮.
			strPath     = (char*)s;
			strSection  = (char*)t;
		  #if 0	//+++ ファイルだなくて URL なんだから、まずい
			if (Misc::IsExistFile(strPath) == 0) {	// ファイルが存在しない場合は、やっぱりutf16として扱ってみる.
				strPath     = (TCHAR*)s;
				strSection  = (TCHAR*)t;
			}
		  #endif
		}
	  #else
		CString 			strPath     = _pExInfo->lpstrIniPath;
		CString 			strSection  = _pExInfo->lpstrSection;
	  #endif
		try {
			_Load_OptionalData(pChild, strPath, strSection);
		} catch (...) {
			ErrorLogPrintf(_T("ApiNewWindow3"));
			MessageBox( _T("例外エラーが発生しました。(In NewWindow3 Function)\n")
						_T("安全のためプログラムを再起動させてください。")			);
		}

		if (_pExInfo->bGroup) {
			pChild->view().m_ViewOption._GetProfile(strPath, strSection, !CMainOption::s_bTabMode);
			//+++ _GetProfile中で Navigate2 しているので、直後でまたNavigate2する必要ないから、ここで帰っておく
			return m_MDITab.GetTabIndex(pChild->m_hWnd);
		} else {
			pChild->OnSetExtendedTabStyle(ExStyle | CChildFrame::FLAG_SE_NOREFRESH);
		}
	} else {
		pChild->OnSetExtendedTabStyle(ExStyle | CChildFrame::FLAG_SE_NOREFRESH);
	}

	pChild->SetWaitBeforeNavigate2Flag();			//+++ 無理やり、BeforeNavigate2()が実行されるまでの間アドレスバーを更新しないようにするフラグをon
	pChild->Navigate2(strURL);

	return m_MDITab.GetTabIndex(pChild->m_hWnd);
#endif
	return -1;
}


long CMainFrame::ApiAddGroupItem(BSTR bstrGroupFile, int nIndex)
{
#if 0	//:::
	HWND		 hWndChild = m_MDITab.GetTabHwnd(nIndex);

	if ( !::IsWindow(hWndChild) )
		return -1;

	CChildFrame *pChild    = GetChildFrame(hWndChild);

	if (pChild == NULL)
		return -1;

	return pChild->_AddGroupOption(bstrGroupFile);
#endif
	return -1;
}


long CMainFrame::ApiDeleteGroupItem(BSTR bstrGroupFile, int nIndex)
{
	CString 		strGroupFile = bstrGroupFile;
	{
		CString 	strSection;
		strSection.Format(_T("Window%d"), nIndex);
		CIniFileO	pr(strGroupFile, strSection);
		bool		bRet = pr.DeleteSection();
		pr.Close();
		if (!bRet) {
			MessageBox(_T("Error1: セクションの削除に失敗しました。"));
			return 0;
		}
	}

	std::list<CString>	buf;
	bool bRet = FileReadString(strGroupFile, buf);
	if (!bRet) {
		MessageBox(_T("Error2: ファイルの読み込みに失敗しました。"));
		return 0;
	}

	std::list<CString>::iterator str;

	for (str = buf.begin(); str != buf.end(); ++str) {
		CString strCheck = str->Left(7);
		strCheck.MakeUpper();

		if ( strCheck == _T("[WINDOW") ) {
			int nSecIndex = _ttoi( str->Mid(7, str->GetLength() - 7 - 1) );
			if (nSecIndex > nIndex) {
				str->Format(_T("[Window%d]"), nSecIndex - 1);
			}
		}
	}

	bRet = FileWriteString(strGroupFile, buf);
	if (!bRet) {
		MessageBox(_T("Error3: ファイルの書き込みに失敗しました。"));
		return 0;
	}

	DWORD		dwCount = 0, dwActive = 0, dwMaximize = 0;
	CIniFileIO	pr( strGroupFile, _T("Header") );
	pr.QueryValue( dwCount, _T("Count") );
	if (dwCount) {
		dwCount--;
		pr.SetValue( dwCount, _T("Count") );
	}

	pr.QueryValue( dwActive, _T("active") );
	if (dwCount == 0)
		pr.DeleteValue( _T("active") );
	else if ( (int) dwActive > nIndex )
		pr.SetValue( dwActive - 1, _T("active") );

	pr.QueryValue( dwMaximize, _T("maximized") );

	if (dwCount == 0)
		pr.DeleteValue( _T("maximized") );
	else if ( (int) dwMaximize > nIndex )
		pr.SetValue( dwMaximize - 1, _T("maximized") );

	return 1;
}


//IAPI4
HWND CMainFrame::ApiGetHWND(long nType)
{
	switch (nType) {
	case 0:
		return m_hWnd;
	case 5:
		return m_SearchBar.GetHWND();
	case 6:
		return m_SearchBar.GetKeywordComboBox();
	case 8:
		return m_SearchBar.GetHWndToolBar();

	}
	return NULL;
}

