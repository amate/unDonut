/**
 *	@file	ChildFrame.cpp
 *	@brief	タブページ１つの処理.
 */

#include "stdafx.h"

//#include <vector>
#include "PluginManager.h"
#include "MtlBrowser.h"

#include "FavoritesMenu.h"
#include "DonutFavoritesMenu.h"
#include "FavoriteOrder.h"
#include "DonutView.h"
#include "MDITabCtrl.h"
#include "MDIChildUserMessenger.h"

#include "option/AddressBarPropertyPage.h"
#include "option/SearchPropertyPage.h"
#include "option/DLControlOption.h"
#include "option/StartUpOption.h"
#include "option/DonutConfirmOption.h"
#include "option/IgnoreURLsOption.h"
#include "option/CloseTitleOption.h"
#include "option/UrlSecurityOption.h"	//+++
#include "ExStyle.h"					//+++

#include "ParseInternetShortcutFile.h"	//+++

#include "FileNotification.h"
#include "DonutPFunc.h" 		//+++
#include "ChildFrame.h"

//#include "MainFrame.h"			//+++ debug
#include "Download/DownloadManager.h"
#include "AtlHostEx.h"

//#include "MDIChildUserMessenger.h"
#ifdef _DEBUG
	const bool _Donut_ChildFrame_traceOn = false;	//false;
	#define dcfTRACE	if (_Donut_ChildFrame_traceOn) ATLTRACE
#else
	#define dcfTRACE
#endif

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



static const int	g_LIGHTMAX		= 5;

#if 1 //+++ 実験でoff ... やっぱり、このまま.
#if 0 //def USE_ORG_UNDONUT_INI
static const LPCTSTR		g_lpszLight[] = {
	"<span id='DonutP' style='color:black;background:FFFF00'>",
	"<span id='DonutP' style='color:black;background:00FFFF'>",
	"<span id='DonutP' style='color:black;background:FF00FF'>",
	"<span id='DonutP' style='color:black;background:7FFF00'>",
	"<span id='DonutP' style='color:black;background:1F8FFF'>"
};
#else
static const LPCTSTR		g_lpszLight[] = {
  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
	_T("<span id='unDonutHilight' style='color:black;background:#FFFF00'>"),
	_T("<span id='unDonutHilight' style='color:black;background:#00FFFF'>"),
	_T("<span id='unDonutHilight' style='color:black;background:#FF00FF'>"),
	_T("<span id='unDonutHilight' style='color:black;background:#7FFF00'>"),
	_T("<span id='unDonutHilight' style='color:black;background:#1F8FFF'>"),
  #else
	"<span id='Donut' style='color:black;background:FFFF00'>",
	"<span id='Donut' style='color:black;background:00FFFF'>",
	"<span id='Donut' style='color:black;background:FF00FF'>",
	"<span id='Donut' style='color:black;background:7FFF00'>",
	"<span id='Donut' style='color:black;background:1F8FFF'>"
  #endif
};
#endif
#endif



// ===========================================================================
// 初期化・ページ読み込み

CChildFrame::CChildFrame(  CMDITabCtrl &MDITab
						 , CDonutAddressBar &addressbar
						 , bool bNewWindow2
						 , DWORD dwDefaultDLControlFlags
						 , DWORD dwDefaultExtendedStyleFlags
						)
	: m_bNavigateBack(FALSE)
	, m_bNavigateForward(FALSE)
	, m_MDITab(MDITab)
	, m_AddressBar(addressbar)
	, m_bNewWindow2(bNewWindow2)
//	, m_FavMenu(this, ID_INSERTPOINT_FAVORITEMENU/*ID_INSERTPOINT_CHILDFAVORITEMENU*/)
	, m_nProgress(-1)
	, m_nProgressMax(0)
	, m_bPageFocusInitialized(false)
	, m_bClosing(false)
	, m_bWaitNavigateLock(true)
	, m_hWndFocus(NULL)
	, m_view(dwDefaultDLControlFlags, dwDefaultExtendedStyleFlags)	//+++引数追加.
	, m_bOperateDrag(false)
	, m_bExPropLock(false)
	, m_hWndF(NULL)
	, m_hWndA(NULL)
	, m_strBookmark(NULL)
	, m_nPainBookmark(0)
	, m_nSecureLockIcon( secureLockIconUnsecure )
	, m_bPrivacyImpacted( TRUE )
	, m_bInitTravelLog( TRUE )
	, m_nImgWidth( -1 )
	, m_nImgHeight( -1 )
	, m_bNowHilight(FALSE)
	, m_bAutoHilight( false )
	, m_bSaveSearchWordflg( true ) //\\+
  #if 1	//+++
	, m_nCmdType(0)
	, m_bImageAutoSizeReq(0)
	, m_nImgScl(100)
	, m_nImgSclSav(100)
	, m_bImg(0)
  #endif
	, m_bNowNavigate(false)
	, m_bReload(false)
	, m_bAllowNewWindow(false)
{
  #if 1	//+++
	{
		static bool bInit				= false;
		static bool bImgAuto_NouseLClk	= false;
		static int  nImgSclSw			= 0;
		if (bInit == 0) {
			CIniFileI pr( g_szIniFileName, _T("ETC") );
			bImgAuto_NouseLClk	= pr.GetValue(_T("ImageAutoSize_NouseLClick")) != 0;
			nImgSclSw			= pr.GetValue(_T("ImageAutoSize_FirstOn")) != 0;
		}
		m_bImgAuto_NouseLClk	= bImgAuto_NouseLClk;
		m_nImgSclSw				= nImgSclSw;
	}
  #endif
}


/** リンクを新しいタブで開く、等
 */
void CChildFrame::OnDocHostUIOpenNewWin(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (wNotifyCode == NM_ON) {
		m_view.m_ViewOption.m_dwExStyle |= DVS_EX_OPENNEWWIN;
		m_MDITab.NavigateLockTab(m_hWnd, true);
	} else if (wNotifyCode == NM_OFF) {
		m_view.m_ViewOption.m_dwExStyle &= ~DVS_EX_OPENNEWWIN;
		m_MDITab.NavigateLockTab(m_hWnd, false);
	} else if (m_view.m_ViewOption.m_dwExStyle & DVS_EX_OPENNEWWIN) {
		m_view.m_ViewOption.m_dwExStyle &= ~DVS_EX_OPENNEWWIN;
		m_MDITab.NavigateLockTab(m_hWnd, false);
	} else {
		m_view.m_ViewOption.m_dwExStyle |= DVS_EX_OPENNEWWIN;
		m_MDITab.NavigateLockTab(m_hWnd, true);
	}
}


void CChildFrame::OnNewWindow2(IDispatch **ppDisp, bool &bCancel)
{
	dcfTRACE( _T("CChildFrame::OnNewWindow2\n") );

	if (CMainOption::s_bAppClosing) {
		dcfTRACE( _T(" application closing, ignore this event\n") );
		bCancel = true;
		return;
	}

	DWORD		 exStyle= _GetInheritedExtendedStyleFlags();	//+++ 新規に子供を作るときの、その子供に渡す ExtendedStyle を求める...

	m_MDITab.SetLinkState(LINKSTATE_A_ON);
	CChildFrame *pChild = CChildFrame::NewWindow(	m_hWndMDIClient,
													m_MDITab,
													m_AddressBar,
													true,
													_GetInheritedDLControlFlags(),
													exStyle );	//+++ 引数追加.
	
	//m_MDITab.SetLinkState(LINKSTATE_OFF);
	// Note: In this case, must be activated in BeforeNavigate2 strangely
	// pChild->ActivateFrameForNewWindowDelayed();
	if (pChild == NULL) {
		bCancel = true;
		return;
	}
	pChild->m_spBrowser->get_Application(ppDisp);
	//Raise Plugin Event
	//CComBSTR bstr;
	//pChild->m_spBrowser->get_LocationURL(&bstr);
	//CPluginManager::ChainCast_PluginEvent(DEVT_TAB_OPENED,(DWORD)(LPCTSTR)CString(bstr),0);

	pChild->SetWaitBeforeNavigate2Flag();			//+++ 無理やり、BeforeNavigate2()が実行されるまでの間アドレスバーを更新しないようにするフラグをon
	// 検索ワードとハイライトの設定を継承する
	pChild->SetSearchWordAutoHilight(m_strSearchWord, m_bNowHilight != 0);

	//pChild->m_spBrowser->QueryInterface(IID_IDispatch, (void **) ppDisp);
	ATLASSERT(ppDisp != NULL);
}


void CChildFrame::OnNewWindow3(IDispatch **ppDisp, bool& Cancel, DWORD dwFlags, BSTR bstrUrlContext,  BSTR bstrUrl)
{
	if (::GetKeyState(VK_SHIFT) < 0)
		return;

	if (CMainOption::s_bIgnore_blank && m_bAllowNewWindow == false) {
		CComPtr<IDispatch>	spDisp;
		m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2>	spDocument = spDisp;
		ATLASSERT(spDocument);
		CPoint	pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);
		CComPtr<IHTMLElement>	spHitElement;
		spDocument->elementFromPoint(pt.x, pt.y, &spHitElement);
		// Hit先がリンクの場合
		CComQIPtr<IHTMLAnchorElement> spAnchor = spHitElement;
		if (spAnchor == NULL) {
			// 親を確かめる
			CComPtr<IHTMLElement>	spParentElement;
			spHitElement->get_parentElement(&spParentElement);
			spAnchor = spParentElement;
			if (spAnchor == NULL) {
				// フレームの可能性
				CComQIPtr<IHTMLFrameElement3>	spFrame = spHitElement;
				if (spFrame) {
					LONG x, y;
					spHitElement->get_offsetTop(&y);
					spHitElement->get_offsetLeft(&x);
					pt.Offset(-x, -y);
					CComPtr<IDispatch>	spFrameDisp;
					spFrame->get_contentDocument(&spFrameDisp);
					CComQIPtr<IHTMLDocument2>	spFrameDocument = spFrameDisp;
					if (spFrameDocument == NULL) 
						return;	// 別ドメイン
					ATLASSERT(spFrameDocument);
					CComPtr<IHTMLElement>	spHitFrameElement;
					spFrameDocument->elementFromPoint(pt.x, pt.y, &spHitFrameElement);
					spAnchor = spHitFrameElement;
					CComBSTR strTag;
					spHitFrameElement->get_tagName(&strTag);
					DEBUGPUT(strTag);
					if (spAnchor == NULL) {
						// 親を確かめる
						CComPtr<IHTMLElement>	spParentElement2;
						spHitFrameElement->get_parentElement(&spParentElement2);
						spAnchor = spParentElement2;
						// とりあえず調べるのはここまで
					}
				} else {
					//ATLASSERT(FALSE);
				}
			}
		}
		if (spAnchor) {
			CComBSTR strTarget;
			spAnchor->get_target(&strTarget);
			if (strTarget == _T("_blank") || strTarget == _T("_new")) {
				Cancel = true;
				Navigate2(bstrUrl);
			}
		}
	}
	// スクリプトなどによる新規タブは許可
}

/// window.close()が呼ばれたのでウィンドウを閉じる
void CChildFrame::OnWindowClosing(bool IsChildWindow, bool& bCancel)
{
	m_bClosing = true;
	PostMessage(WM_CLOSE);
}

#if 0
void	_threadChildFrame(CChildFrame* pChild, HWND hWndMDIClient, bool* pbCreated)
{
	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	ATLASSERT( SUCCEEDED(hRes) );
	// If you are running on NT 4.0 or higher you can use the following call instead to
	// make the EXE free threaded. This means that calls come in on a random RPC thread
	//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	hRes		 = ::OleInitialize(NULL);
	ATLASSERT( SUCCEEDED(hRes) );

	CMessageLoop theLoop;

	if (pChild) 
		pChild->CreateEx(hWndMDIClient);
	*pbCreated = true;
	theLoop.Run();	//


	::OleUninitialize();
	::CoUninitialize();
}
#endif

CChildFrame *CChildFrame::NewWindow(
		HWND				hWndMDIClient,
		CMDITabCtrl &		tabMDI,
		CDonutAddressBar &	adBar,
		bool				bNewWindow2 /*= false*/,
		DWORD				dwDLFlags	/*= CDLControlOption::s_dwDLControlFlags*/,
		DWORD				dwESFlags	/*= CDLControlOption::s_dwExtendedStyleFlags*/	//+++ 追加
){
	int nCount = tabMDI.GetItemCount();
	if ( !CMainOption::IsQualify( nCount ) )
		return NULL;

	if (dwDLFlags == (DWORD)-1)
		dwDLFlags = CDLControlOption::s_dwDLControlFlags;

	if (dwESFlags == (DWORD)-1)									//+++
		dwESFlags = CDLControlOption::s_dwExtendedStyleFlags;	//+++

	CChildFrame *pChild = new CChildFrame(tabMDI, adBar, bNewWindow2, dwDLFlags, dwESFlags);
	//bool bCreated = false;
	//boost::thread th(boost::bind(&_threadChildFrame, pChild, hWndMDIClient, &bCreated));
	//んー?
	if (pChild) 
		pChild->CreateEx(hWndMDIClient);
#if 0
	int 	ret;
	MSG 	msg 	= { 0 };
	int 	n		= 0;
	HWND hWndTop = CWindow(hWndMDIClient).GetTopLevelWindow();
	while ((ret = ::PeekMessage(&msg, hWndTop, 0, 0, PM_NOREMOVE)) != 0) {
		if (!GetMessage (&msg, hWndTop, 0, 0))	/* メッセージ処理. QUITだったら、速攻かえる*/
			return pChild;	//x return msg.wParam ;
		//if (OnForwardMsg(&msg, 0) == 0) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		//}
		++n;
	}
#endif
	return pChild;
}


/// ウィンドウの初期化
LRESULT CChildFrame::OnCreate(LPCREATESTRUCT/* lpcreatestruct*/)
{
	dcfTRACE( _T("CChildFrame::OnCreate\n") );

	HRESULT hr = S_OK;

	// Let me initialize itself
	LRESULT lRet = DefWindowProc();

	// ビューウィンドウを作成(IE窓)
	m_hWndClient = m_view.Create(m_hWnd, rcDefault, _T("about:blank"), WS_CHILD | WS_VISIBLE /* | WS_CLIPSIBLINGS | WS_CLIPCHILDREN*/, 0/*WS_EX_CLIENTEDGE*/, ID_DONUTVIEW);

	//コンテキストメニューをカスタマイズするかどうか
	m_view.SetIeMenuNoCstm(CMenuOption::s_bNoCustomIEMenu);

	//ドラッグドロップの処理を設定
	_InitDragDropSetting();

	// IWebBrowser2を取得
	m_view.QueryControl(IID_IWebBrowser2, (void **) &m_spBrowser);

	// イベントシンクに接続
	WebBrowserEvents2Advise();

	if ( _check_flag(MAIN_EX_REGISTER_AS_BROWSER, CMainOption::s_dwMainExtendedStyle) )
		SetRegisterAsBrowser(true); 	// for target name resolution

	if (!m_bNewWindow2)
		SetVisible(true);

	//m_ieToolBar    = std::auto_ptr<CIEToolBar>(new CIEToolBar(((CWindow*)g_pMainWnd)->m_hWnd/*GetTopLevelParent()*/, m_spBrowser.p, _T("{625AA53D-1F10-44FE-B907-91FE25220D3F}")));	//+++ 実験.
	//m_ieToolBar    = std::auto_ptr<CIEToolBar>(new CIEToolBar(((CWindow*)g_pMainWnd)->m_hWnd/*GetTopLevelParent()*/, m_spBrowser.p, _T("32099AAC-C132-4136-9E9A-4E364A424E17")));	//+++ 実験.

	return lRet;
}


/// Drag&Drop設定の初期化
void CChildFrame::_InitDragDropSetting()
{
	CIniFileI	pr( _GetFilePath("MouseEdit.ini"), _T("MouseCtrl") );
	DWORD			dwCommand = pr.GetValue(_T("DragDrop"), 0);
	m_bOperateDrag = (dwCommand != 0);
	pr.Close();
	if (m_bOperateDrag) {
		m_view.SetOperateDragDrop(TRUE, dwCommand);
	}
}



/// ナビゲートの準備
void CChildFrame::OnBeforeNavigate2(
					IDispatch *			pDisp,
					const CString &		strURL,
					DWORD				nFlags,
					const CString &		strTargetFrameName,
					CSimpleArray<BYTE>&	baPostedData,
					const CString &		strHeaders,
					bool &				bCancel )
{
	dcfTRACE(_T("CChildFrame::OnBeforeNavigate2 URL(%s) frame(%s)\n"), strURL.Left(100), strTargetFrameName);

  #if 1	//+++ 自動リサイズ可能なページに新たに絵でないページを表示すると、そのページもリサイズ可能になってしまっていたのを対処.
	if (m_nImgWidth >= 0) {
		m_nImgWidth			= -1;		//+++ 拡張子(再)チェックのために、初期化
		m_bImg				= 0;
		m_bImageAutoSizeReq = 1;
	}
  #endif

	//m_view.InitDLControlFlags();	//\\+

	{
		//プラグインイベント - ナビゲート前
		DEVTS_TAB_NAVIGATE stn;
		stn.nIndex			 = m_MDITab.GetTabIndex(m_hWnd);
		stn.lpstrURL		 = (LPCTSTR) strURL;
		stn.lpstrTargetFrame = (LPCTSTR) strTargetFrameName;
		int 	nRet		 = CPluginManager::ChainCast_PluginEvent(DEVT_TAB_BEFORENAVIGATE, stn.nIndex, (SPARAM) &stn);

		if (nRet == -1) {
			//ナビゲートキャンセル
			bCancel = true;
			return;
		} else if (nRet == -2) {
			//クローズ
			bCancel 	  = true;
			m_bNewWindow2 = false;
			m_bClosing	  = true;
			PostMessage(WM_CLOSE);		// It's possible to post twice, but don't care.
			return;
		}
	}

	//\\ mailto:無効 が有効なとき開かない
	if (m_view.m_ViewOption.m_dwExStyle & DVS_EX_BLOCK_MAILTO) {
		CString strMailto = strURL.Left(6);
		strMailto.MakeUpper();

		if (strMailto.Compare( _T("MAILTO") ) == 0) {
			bCancel = true;
			return;
		}
	}

	if (m_bClosing) {
		bCancel = true;
		return;
	}

	// deter popups
	if ( m_bNewWindow2 && IsPageIWebBrowser(pDisp) && CIgnoredURLsOption::SearchString(strURL) ) {	// close ignored url
		bCancel 	  = true;
		m_bNewWindow2 = false;
		m_bClosing	  = true;
		PostMessage(WM_CLOSE);			// It's possible to post twice, but don't care.

		{
			//プラグインイベント - ポップアップ抑止
			CPluginManager::BroadCast_PluginEvent(DEVT_BLOCKPOPUP, (FPARAM) (LPCTSTR) strURL, 0);
		}
		return;
	}
	//\\ Navigate中かつjavescriptから始まるウィンドウはナビゲートしない
	if (m_bNowNavigate && strURL.Left(15).CompareNoCase(_T("javascript:void")) == 0) {
		bCancel = true;
		return;
	}
	// javascriptから始まるNewWindowはclose
	if (m_bNewWindow2) {
		CString strJava = strURL.Left(10);
		strJava.MakeUpper();

		if (strJava.Find(_T("JAVASCRIPT")) != -1) {
			bCancel 	  = true;
			m_bNewWindow2 = false;
			m_bClosing	  = true;
			PostMessage(WM_CLOSE);		// It's possible to post twice, but don't care.

			DonutOpenFile(m_hWnd, strURL);
			return;
		}
	}

	// 自分でDLコントロールを切り替えたときは無視するように
	if (m_view.m_bLightRefresh == false) {
	  #if 1	//+++ Url別拡張プロパティ設定の処理. 主にリンク用.(通常のopenの場合は、CMainFrame::OnUserOpenFileな処理を通るので、そちらで設定ずみ)
		DWORD exopts = 0xFFFFFFFF, dlCtlFlg = 0xFFFFFFFF, exstyle = 0xFFFFFFFF, autoRefresh = 0xFFFFFFFF, dwExPropOpt = 8;
		if (CUrlSecurityOption::IsUndoSecurity(GetLocationURL())) {
			m_view.PutDLControlFlags(CDLControlOption::s_dwDLControlFlags);
			SetViewExStyle(CDLControlOption::s_dwExtendedStyleFlags);
		}
		if (CUrlSecurityOption::FindUrl( strURL, &exopts, &dwExPropOpt, 0 )) {
			CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, exopts, dwExPropOpt);
			dlCtlFlg	= ExProp.GetDLControlFlags();
			exstyle		= ExProp.GetExtendedStyleFlags();
			autoRefresh = ExProp.GetAutoRefreshFlag();
		}
	  #endif

		// Navigate lock
		if ( !m_bExPropLock
		   && _check_flag(DVS_EX_OPENNEWWIN, m_view.m_ViewOption.m_dwExStyle)
		   && m_bWaitNavigateLock == false
		   && !IsRefreshBeforeNavigate2(pDisp) )
		{
			m_bExPropLock = FALSE;
			dcfTRACE( _T(" ナビゲートロックされていますので、新しいタブで開きます\n") );
			if (dlCtlFlg == 0xFFFFFFFF)
				dlCtlFlg =  _GetInheritedDLControlFlags();
			if (exstyle  == 0xFFFFFFFF)			//+++
				exstyle  = _GetInheritedExtendedStyleFlags();	//+++ 新規に子供を作るときの、その子供に渡す ExtendedStyle を求める...
			CChildFrame *pChild = CChildFrame::NewWindow( m_hWndMDIClient, m_MDITab, m_AddressBar, false, dlCtlFlg, exstyle );
			if (pChild) {
				pChild->SetWaitBeforeNavigate2Flag();	//*+++ 無理やり、BeforeNavigate2()が実行されるまでの間アドレスバーを更新しないようにするフラグをon
				pChild->ActivateFrameForNewWindowDelayed();
				pChild->Navigate2(strURL);
				pChild->m_strSearchWord = m_strSearchWord;
			  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
				pChild->m_bNowHilight	= m_bNowHilight;
			  #endif

			  #if 1	//+++ url別拡張プロパティの処理.
				if (exopts != 0xFFFFFFFF) {
					pChild->view().PutDLControlFlags( dlCtlFlg );
					pChild->SetViewExStyle( exstyle );
					pChild->view().m_ViewOption.SetAutoRefreshStyle( autoRefresh );
				}
			  #endif
			}
			bCancel 	  = true;
			return;
		}

	  #if 1	//+++ url別拡張プロパティの処理....
			//+++	戻る・進むでの拡張プロパティ情報の頁ごとの保存ができていないので、破綻する...
			//+++	が、バグっても反映されることのほうが意味ありそうなので利用
		if (CUrlSecurityOption::s_bValid) {
			if (exopts != 0xFFFFFFFF && CUrlSecurityOption::activePageToo()) {
				m_view.PutDLControlFlags( dlCtlFlg );
				m_view.m_ViewOption.SetAutoRefreshStyle( autoRefresh );
				SetViewExStyle( exstyle );	//+++メモ: マウス中ボタンクリックでの、リンク別タブ表示の場合、まだタブ位置未決定のため設定できない...
			//} else {
			}
		}
	  #endif
	} else {	// URL別セキュリティをスキップしたのでもとに戻しておく
		m_view.m_bLightRefresh = false;
	}
	m_bExPropLock	= FALSE;

	// Note: Some ActivateFrame would go mad, because MDI client window not setup yet.
	if (m_bNewWindow2) {				// delayed activation if OnNewWindow2 called
		ActivateFrameForNewWindowDelayed();
		m_bNewWindow2 = false;
	}

	// UH
	// ハイライト失踪･･･
  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
	//m_bNowHilight=FALSE;	// gae: 一度適用したハイライトは明示的に解除されるまで維持される
  #else
	m_bNowHilight	= FALSE;
  #endif
	m_nPainBookmark = 0;
	m_strBookmark	= LPCOLESTR(NULL);

	m_bNowNavigate = true;	// Navigate中である

	{
		//イベント発生 - ナビゲート
		DEVTS_TAB_NAVIGATE stn;
		stn.nIndex			 = m_MDITab.GetTabIndex(m_hWnd);
		stn.lpstrURL		 = (LPCTSTR) strURL;
		stn.lpstrTargetFrame = (LPCTSTR) strTargetFrameName;
		CPluginManager::BroadCast_PluginEvent(DEVT_TAB_NAVIGATE, stn.nIndex, (SPARAM) &stn);
	}
}



DWORD CChildFrame::_GetInheritedDLControlFlags()
{
	DWORD dwDLFlags;

	if (this &&  _check_flag(MAIN_EX_INHERIT_OPTIONS, CMainOption::s_dwMainExtendedStyle) )
		dwDLFlags = m_view._GetDLControlFlags();
	else
		dwDLFlags = CDLControlOption::s_dwDLControlFlags;

	// default
	return dwDLFlags;
}


//+++ 新規に子供を作るときの、その子供に渡す ExtendedStyle を求める...
DWORD CChildFrame::_GetInheritedExtendedStyleFlags()
{
	DWORD dwExFlags;

	if (this &&  _check_flag(MAIN_EX_INHERIT_OPTIONS, CMainOption::s_dwMainExtendedStyle) ) {
		dwExFlags = m_view._GetExtendedStypeFlags();
	  #if 1	//+++ ナビゲートロックに関しては継承しない....
		dwExFlags &= ~DVS_EX_OPENNEWWIN;											//+++ off
		dwExFlags |= CDLControlOption::s_dwExtendedStyleFlags & DVS_EX_OPENNEWWIN;	//+++ けど、デフォルト設定があれば、それを反映.
	  #endif
	} else {
		dwExFlags = CDLControlOption::s_dwExtendedStyleFlags;
	}

	// default
	return dwExFlags;
}



int CChildFrame::ActivateFrameForNewWindowDelayed(int nCmdShow /*= -1*/)
{
	dcfTRACE( _T("CChildFrame::ActivateFrameForNewWindowDelayed - now activating!\n") );

	HWND	hWndActive = MDIGetActive();
	if (hWndActive != NULL && CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE_NEWWIN)
		nCmdShow = SW_SHOWNOACTIVATE;

	return ActivateFrame(nCmdShow);
}



int CChildFrame::ActivateFrame(int nCmdShow /*= -1*/)
{
	ATLASSERT( ::IsWindow(m_hWnd) );
	if (m_MDITab.GetTabIndex(m_hWnd) == -1) 	//なんだかMDITabのMDIActivateの方が先に呼び出されてしまうので
		m_MDITab.OnMDIChildCreate(m_hWnd);		//いろいろと順番が狂っている

	if ( _check_flag(m_view.m_ViewOption.m_dwExStyle, DVS_EX_OPENNEWWIN)) {
		m_MDITab.NavigateLockTab(m_hWnd, true);
	}

	// Raise Plugin Event
	int nNewIndex = m_MDITab.GetTabIndex(m_hWnd);
	CPluginManager::ChainCast_PluginEvent(DEVT_TAB_OPENED, nNewIndex, 0);

	return baseClass::ActivateFrame(nCmdShow);
}

/// documentが操作できるようになった
void CChildFrame::OnDocumentComplete(IDispatch *pDisp, const CString &strURL)
{
	if ( IsPageIWebBrowser(pDisp) ) {	// it means a page complete downloading
		dcfTRACE( _T("CChildFrame::OnDocumentComplete\n") );
		m_bWaitNavigateLock = false;	// you are allowed!
		m_bExPropLock		= FALSE;

		if ( MDIGetActive() == m_hWnd && DonutBrowserCanSetFocus(m_hWnd) ) {
			_SetPageFocus();
		}
	}

	//常用スタイルシートを設定する
	ApplyDefaultStyleSheet();

	//戻る・進むの履歴をロードする
	SetTravelLogData();

	//画像ファイルのとき、オプションで、サイズ調整する
	CheckImageAutoSize(&strURL, TRUE);
	//SetBodyStyleZoom(0, TRUE);	//+++

	CDonutSearchBar* pSearch = CDonutSearchBar::GetInstance();
	//オートハイライト
	if (m_bAutoHilight) {
		pSearch->ForceSetHilightBtnOn(true);
	}

	bool bHilight = m_bAutoHilight || pSearch->GetHilightSw();
	if (bHilight) {
	  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
		if (m_strSearchWord.IsEmpty() == FALSE) {
			if (m_bNowHilight == FALSE && IsWindowVisible()) {
				pSearch->SetSearchStr(m_strSearchWord);	// 選択検索用
			}
			CString strWords = m_strSearchWord;
			DeleteMinimumLengthWord(strWords);
			m_bNowHilight = TRUE;
			OnHilightOnce(pDisp, strWords);
		}
	  #else
		if (!m_strSearchWord.IsEmpty())
			OnHilight(m_strSearchWord);
	  #endif
	}

	{
		//プラグインイベント - ロード完了
		//if(IsPageIWebBrowser(pDisp)){
		CComQIPtr<IWebBrowser2> 	pWB2 = pDisp;

		if (pWB2) {
			DEVTS_TAB_DOCUMENTCOMPLETE	dc;
			dc.lpstrURL   = (LPCTSTR) strURL;

			CComBSTR	bstrTitle;
			pWB2->get_LocationName(&bstrTitle);

			CString 	strTitle = bstrTitle;
			dc.lpstrTitle = (LPCTSTR) strTitle;
			dc.nIndex	  = m_MDITab.GetTabIndex(m_hWnd);
			dc.bMainDoc   = IsPageIWebBrowser(pDisp);
			CPluginManager::BroadCast_PluginEvent(DEVT_TAB_DOCUMENTCOMPLETE, dc.nIndex, (DWORD_PTR) &dc);
		}
		//}
	}
}


void CChildFrame::OnDownloadComplete()
{
	dcfTRACE( _T("CChildFrame::OnDownloadComplete\n") );
	m_bNowNavigate = false;		// Navigate終了
}



void CChildFrame::OnStatusTextChange(const CString &strText)
{
	{
		//プラグインイベント - ステータステキスト変更
		int nRet = CPluginManager::ChainCast_PluginEvent(DEVT_CHANGESTATUSTEXT, (FPARAM) (LPCTSTR) strText, 0);
		if (nRet < 0)
			return;
	}

	BOOL bFilter = FALSE;
	if (m_view.m_ViewOption.m_dwExStyle & DVS_EX_MESSAGE_FILTER) {
		if (strText.Find(_T("http"  )) != -1)						bFilter = TRUE;
		if (strText.Find(_T("ftp"	)) != -1)						bFilter = TRUE;
		if (strText.Find(_T("接続"  )) != -1)						bFilter = TRUE;
		if (strText.Find(_T("mailto")) != -1)						bFilter = TRUE;
		if (strText.Find(_T("file"  )) != -1)						bFilter = TRUE;
		if (strText.Find(_T("java"  )) != -1)						bFilter = TRUE;
		if ( strText.IsEmpty()	 )									bFilter = TRUE;
		if (strText.Compare(_T("ページが表示されました")) == 0) 	bFilter = TRUE; 	//minit
		if (strText == _T("完了"))									bFilter = TRUE;
		if (bFilter == FALSE)										return;
	}

	if (m_strStatusBar != strText)
		m_strStatusBar = strText;

	if (MDIGetActive() == m_hWnd) {
		CWindow wndFrame   = GetTopLevelParent();
		HWND	hWndStatus = wndFrame.GetDlgItem(ATL_IDW_STATUS_BAR);
		int 	nMode	   = (int) ::SendMessage(hWndStatus, WM_GET_OWNERDRAWMODE, 0, 0);
		if (nMode)
			MtlSetStatusText(hWndStatus, strText);
		else
			MtlSetWindowText(hWndStatus, strText);

		// UDT DGSTR ( update
		LPCTSTR lpszText   = strText;
		SendMessage(GetTopLevelParent(), WM_UPDATE_TITLEBAR, (WPARAM) lpszText, (LPARAM) 0);
		// ENDE
	}
}



void CChildFrame::OnTitleChange(const CString &strTitle)
{
	CString strURL = GetLocationURL();

	if ( CCloseTitlesOption::SearchString(strTitle) ) {
		m_bClosing = true;
		PostMessage(WM_CLOSE);
		{
			//プラグインイベント - タイトル抑止
			CPluginManager::BroadCast_PluginEvent(DEVT_BLOCKTITLE, (FPARAM) (LPCTSTR) strTitle, 0);
		}
		return;
	}

	dcfTRACE( _T("OnTitleChange : %s\n"), strTitle.Left(100) );
	// change title
	SetWindowText( MtlCompactString(strTitle, 255) );

  #if 0 //+++ 強引に、ページ窓開け時のabout:blank設定をキャンセルする... +mod.1.17以前の不具合対策で、どうも以降では帰って不具合に化ける場合がありそなんで、止め.
	// change address bar
	if (MDIGetActive() == m_hWnd) {
		if ((strURL == "about:blank" || strURL.IsEmpty()) && IsWaitBeforeNavigate2Flag()) { //+++
			;
		} else {	//+++ 通常.
			m_AddressBar.SetWindowText(strURL);
		}
	}
  #else
	// change address bar
	if (MDIGetActive() == m_hWnd)
		m_AddressBar.SetWindowText(strURL);
  #endif
}


void CChildFrame::OnCommandStateChange(long Command, bool bEnable)
{
	if (Command == CSC_NAVIGATEBACK) {
		m_bNavigateBack 	= bEnable /*? TRUE : FALSE*/;
	} else if (Command == CSC_NAVIGATEFORWARD) {
		m_bNavigateForward	= bEnable /*? TRUE : FALSE*/;
	} else if (Command == CSC_UPDATECOMMANDS) {
	}
}


void CChildFrame::OnStateConnecting()
{
	m_MDITab.SetConnecting(m_hWnd);
	m_bPrivacyImpacted = TRUE;	// ページ読み込み後もOnStateConnectingが呼ばれるページがあるので
								// クッキー制限アイコンが表示されないページがある
	//m_nSecureLockIcon = secureLockIconUnsecure;
}


//void CChildFrame::OnStateDownloading()
//{
//	m_MDITab.SetDownloading(m_hWnd);
//}


void CChildFrame::OnStateCompleted()
{
	m_MDITab.SetComplete(m_hWnd);

	HWND hWndA2 = ::GetActiveWindow();
	HWND hWndF2 = ::GetFocus();

	if (m_hWndA)
		::SetActiveWindow(m_hWndA);

	if (m_hWndF)
		::SetFocus(m_hWndF);

	if (m_bReload) {
		CString strUrl = GetLocationURL();
		CComQIPtr<IDispatch>	spDisp = m_spBrowser;
		OnDocumentComplete(spDisp, strUrl);
		m_bReload = false;
	}
}




// ===========================================================================
// 終了処理

/** +++メモ:自分以外のタブを閉じる
 */
LRESULT CChildFrame::OnWindowCloseExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	if ( !CDonutConfirmOption::OnCloseAllExcept( GetTopLevelParent() ) )
		return 0;

	CWaitCursor 		 cur;
	CLockRedrawMDIClient lock(m_hWndMDIClient);
	CLockRedraw 		 lock2(m_MDITab);
	MtlCloseAllMDIChildrenExcept(m_hWndMDIClient, m_hWnd);
	return 0;
}



/** +++メモ:タブの右側/左側をすべて閉じる
 */
LRESULT CChildFrame::OnLeftRightClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	bool	bLeft	= (wID == ID_LEFT_CLOSE);

	if ( !CDonutConfirmOption::OnCloseLeftRight( GetTopLevelParent(), bLeft ) )
		return 0;

	CWaitCursor cur;

	HWND		hWndActive = (HWND) ::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM) NULL);
	int 		nCurSel    = m_MDITab.GetTabIndex(m_hWnd);
	int 		nCurSel2   = m_MDITab.GetTabIndex(hWndActive);
	int 		nCurSel3   = m_MDITab.GetTabIndex(m_hWndMDIClient);

	if (nCurSel == -1)
		return 0;

	::SendMessage(GetTopLevelParent(), WM_USER_WINDOWS_CLOSE_CMP, (WPARAM) nCurSel, (LPARAM) bLeft);
	return 0;
}


/** 全てのタブを閉じる
 */
void CChildFrame::OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	::SendMessage(::GetParent( ::GetParent(m_hWnd) ), WM_COMMAND, ID_WINDOW_CLOSE_ALL, 0);
}



/** +++メモ:ファイルの閉じる.
 */
void CChildFrame::OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
  #if 1 //+++ タブロック対策... OnClose側のだけで大丈夫のようだけれど、とりあえず.
	if ( s_bMainframeClose == 0 												//+++ unDonut終了時以外で
		&& _check_flag(m_view.m_ViewOption.m_dwExStyle, DVS_EX_OPENNEWWIN)		//+++ ナビゲートロックのページで
		&& (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_NOCLOSE_NAVILOCK)	//+++ ナビゲートロックのページを閉じない、の指定があれば
	){
		m_bClosing = false;
		return; 																//+++ 閉じずに帰る
	}
  #endif
	m_bClosing = true;
	PostMessage(WM_CLOSE);
}



//======================

/** +++メモ:閉じる
 */
void CChildFrame::OnClose()
{
  #if 1 //+++ タブロック対策...
	if ( s_bMainframeClose == 0 												//+++ unDonut終了時以外で
		&& _check_flag(m_view.m_ViewOption.m_dwExStyle, DVS_EX_OPENNEWWIN)		//+++ ナビゲートロックのページで
		&& (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_NOCLOSE_NAVILOCK)	//+++ ナビゲートロックのページを閉じない、の指定があれば
	){
		m_bClosing = false;
		return; 																//+++ 閉じずに帰る
	}
  #endif

	SetMsgHandled(FALSE);
	m_bClosing = true;

	int  nIndex 		= m_MDITab.GetTabIndex(m_hWnd);

	// for mdi tab ctrl
	HWND hWndActiveNext = NULL;

	if (MDIGetActive() == m_hWnd) {
		// このウィンドウがアクティブなら
		int nIndex = m_MDITab.ManageClose(m_hWnd);

		if (nIndex != -1) {
			hWndActiveNext = m_MDITab.GetTabHwnd(nIndex);
			ATLASSERT( ::IsWindow(hWndActiveNext) );
		}
	}

	m_MDITab.OnMDIChildDestroy(m_hWnd);		// このウィンドウに関連付けられたタブを削除

	if (hWndActiveNext) {
		// このウィンドウは破棄されるのでManageCloseで返されたタブをアクティブにする
		MDIActivate(hWndActiveNext);
	}

	if ( !CMainOption::s_bAppClosing ||  !(CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_LATEST) ) { 	//minit
		CString strURL	 = GetLocationURL();
		if ( strURL.IsEmpty() == FALSE ) {	//+++ 先にチェック.
			CString strTitle = GetLocationName();
			SDfgSaveInfo::List	arrFore;
			SDfgSaveInfo::List	arrBack;

			if (CMainOption::s_bTravelLogClose) {
				m_view.m_ViewOption._OutPut_TravelLogs(arrFore, arrBack);
			}
			if (CMainOption::s_pMru)	//+++
				CMainOption::s_pMru->AddToList(strURL, &arrFore, &arrBack, strTitle);
		}
	}

	{
		//プラグインイベント - クローズ
		CPluginManager::BroadCast_PluginEvent(DEVT_TAB_CLOSE, nIndex, 0);
	}

	//+++ destroyへ //+++ DonutRAPTよりパク( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
	//+++           RtlSetMinProcWorkingSetSize();
}


/** 終了開放
 */
void CChildFrame::OnDestroy()
{
	dcfTRACE( _T("CChildFrame::OnDestroy\n") );
	SetMsgHandled(FALSE);
	m_bClosing							  = true;

	if (m_bOperateDrag)
		m_view.SetOperateDragDrop(FALSE, 0);

	SetRegisterAsBrowser(false);

	// m_view.DestroyWindow makes offline state reset, so
	// save global offline state
	bool bGlobalOffline = MtlIsGlobalOffline();

	// Before AtlAdviseSinkMap(this, false) destroy view,
	// cuz I wanna handle OnNewWindow2 event
	// And while destroying view, a meaningless WM_DRAWCLIPBOARD may be sent, so ignore it.

	CMainOption::s_bIgnoreUpdateClipboard = true;
	dcfTRACE( _T(" m_view.Destroy\n") );

	if ( m_view.IsWindow() ) {
		m_view.DestroyWindow();
	}

	CMainOption::s_bIgnoreUpdateClipboard = false;

	dcfTRACE( _T(" DispEventUnadvise\n") );

	WebBrowserEvents2Unadvise();
	m_spBrowser.Release();	//\\+

	// restore global offline state
	//MtlSetGlobalOffline(bGlobalOffline);

	//+++ DonutRAPTよりパク( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
	RtlSetMinProcWorkingSetSize();
}



CChildFrame::~CChildFrame()
{
	dcfTRACE( _T("CChildFrame::~CChildFrame\n") );

	ATLASSERT(m_hMenu == NULL); 				// no menu
}



// ===========================================================================
// 基本的な処理


int CChildFrame::_GetShowCmd()
{
	CWindowPlacement	wndpl;
	if ( m_hWnd && ::GetWindowPlacement(m_hWnd, &wndpl) ) {
		return wndpl.showCmd;
	}
	return -1;
}


///+++  実際のOnPaintはどこかほかで行われてるだろうで、ここでは
///+++  OnPaintのタイミングで画像自動サイズ調整の処理を行うのみ. (他のOnPaintがなされるようにMsgHandled=0にして返す)
void CChildFrame::OnPaint(HDC hDc)
{
	if (m_bImageAutoSizeReq) {
		CheckImageAutoSize(NULL, FALSE);
		m_bImageAutoSizeReq = false;
	}
	SetMsgHandled(FALSE);
}


#if 0	//+++	失敗
void CChildFrame::OnLButtonDown(WORD wparam, const WTL::CPoint& pt)
{
	CRect	rect;
	GetClientRect(&rect);
	if (rect.PtInRect(pt)) {
		MtlSendCommand(m_hWnd, ID_HTMLZOOM_100TOGLE);
	}
}
#endif
BOOL CALLBACK EnumMoveChildProc(HWND hwnd, LPARAM lParam)
{
	CRect rc;
	rc.right = GET_X_LPARAM(lParam);
	rc.bottom= GET_Y_LPARAM(lParam);
	CWindow(hwnd).MoveWindow(&rc);
	return TRUE;
}
//+++
void CChildFrame::OnSize(UINT nType, CSize size)
{
	BOOL	bMaxActive = FALSE;
	HWND	hWndActive = MDIGetActive(&bMaxActive);

	if (bMaxActive == FALSE)
		bMaxActive = ::IsZoomed(hWndActive) != 0;

	int 	nCmdType   = m_nCmdType;
	m_nCmdType = 0;

	//CheckImageAutoSize(NULL, FALSE);		//+++ このタイミングで更新するのは不味いみたい.
	m_bImageAutoSizeReq = true;				//+++ 別のタイミングでサイズ調整するリクエスト

	dcfTRACE(_T("CChildFrame::OnSize type(%d)\n"), nType);

	if (GetTopLevelWindow().IsZoomed() && !(GetTopLevelWindow().GetStyle() & WS_CAPTION)) {
		CRect rcParent;
		GetParent().GetClientRect(&rcParent);
		ModifyStyle(WS_CAPTION | WS_THICKFRAME, 0);
		MoveWindow(&rcParent);
		m_view.MoveWindow(&rcParent);
		::EnumChildWindows(m_view.m_hWnd, EnumMoveChildProc, MAKELPARAM(rcParent.right, rcParent.bottom));
		//m_view.m_spBrowser->put_Width(rcParent.right);
		//m_view.m_spBrowser->put_Height(rcParent.bottom);
		//for (HWND hWndChild = m_view.GetWindow(GW_CHILD); hWndChild != NULL; hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT)) {
		//	CWindow(hWndChild).MoveWindow(&rcParent);
		//}
		
		return;
	}

	if ( CMainOption::s_bTabMode && (nType == SIZE_RESTORED || nType == SIZE_MINIMIZED) ) {
		dcfTRACE( _T(" restored\n") );		
		return ;	// eat it!!
	} else if (    bMaxActive
				&& ((nCmdType != SC_RESTORE && nCmdType != SC_MINIMIZE)
				&& (nType == SIZE_RESTORED || nType == SIZE_MINIMIZED) ) )
	{
		dcfTRACE( _T(" restored\n") );	
		return ;	// eat it!!
	} else {
		SetMsgHandled(FALSE);
		return ;
	}
}


void CChildFrame::OnSysCommand(UINT uCmdType, CPoint pt)
{
  #if 1	//+++ CTRL(+SHIFT)+TABでのタブ移動の順番を見た目通りにする処理
	if ((m_MDITab.GetMDITabExtendedStyle() & MTB_EX_CTRLTAB_MDI) == 0) {	//+++ 元のMDI式にするのでなければ
		if (uCmdType == SC_PREVWINDOW) {
			m_MDITab.LeftTab();
			SetMsgHandled(TRUE);
			return;
		} else if (uCmdType == SC_NEXTWINDOW) {
			m_MDITab.RightTab();
			SetMsgHandled(TRUE);
			return;
		}
	}
  #endif

	m_nCmdType = uCmdType;

	if ( CMainOption::s_bTabMode && (uCmdType == SC_MINIMIZE || uCmdType == SC_RESTORE) )
		SetMsgHandled(TRUE);
	else
		SetMsgHandled(FALSE);
}


/// PretranslateMessageの代わり
LRESULT CChildFrame::OnForwardMsg(LPMSG pMsg, DWORD)
{
  #if 1	//+++	画像表示のとき、窓内でクリックしたら、拡大率を切り替える
	if (m_bImg && m_bImgAuto_NouseLClk == 0 && pMsg->message == WM_LBUTTONDOWN /*&& pMsg->hwnd == m_hWnd*/) {
		CPoint 	pt;
		::GetCursorPos(&pt);
		CRect	rect;
		//CRect	rectB;
		::GetClientRect(pMsg->hwnd, &rect);
		//CClientDC dc(pMsg->hwnd);
		//::GetBoundsRect(dc, &rectB, 0/*DCB_RESET*/);
		ClientToScreen(&rect);
		rect.right -= GetSystemMetrics(SM_CXVSCROLL);
		rect.bottom-= GetSystemMetrics(SM_CYHSCROLL);
		if (rect.PtInRect(pt)) {
			MtlSendCommand(m_hWnd, ID_HTMLZOOM_100TOGLE);
		}
	}
  #endif
	if (pMsg->message == WM_MOUSEWHEEL) {
		UINT nFlags = (UINT)LOWORD(pMsg->wParam);
		short zDelta = (short)HIWORD(pMsg->wParam);
		// 文字を拡大する
		if ( nFlags == MK_CONTROL ) {
			CComVariant vZoomSize;
			//\\ 現在の文字サイズを取得
			m_spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, NULL, &vZoomSize); 
			if ( zDelta > 0 ){	
				vZoomSize.lVal += 1;
			} else {
				vZoomSize.lVal -= 1;
			}
			//\\ 文字サイズを変更
			m_spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vZoomSize, NULL); 
			return TRUE;
		}

		// ページを拡大する
		if ( ::GetKeyState(VK_MENU) & 0x80 ){
			if ( zDelta > 0 ){
				SetBodyStyleZoom(10, 0);
			} else {
				SetBodyStyleZoom(-10, 0);
			}
			return TRUE;
		}
	}

  #if 1	//+++	F5でリロードしたときに、ZOOMが戻されちゃうのを強制するために、無理やり調整
	LRESULT lr = m_view.PreTranslateMessage(pMsg);

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F5 && m_nImgScl != 100) {
		m_bImageAutoSizeReq = true;
		//SendMessage(WM_PAINT);
	  #if _ATL_VER >= 0x700		//+++ atl3ではバグることになるけど、しかたなし
		SetTimer(1, 20, OncePaintReq);
	  #endif
	}

	//if (m_ieToolBar.get())						//+++ 実験.
	//	m_ieToolBar->PreTranslateMessage(pMsg); 	//+++ 実験.

	return lr;
  #else
	if ( baseClass::PreTranslateMessage(pMsg) )
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
  #endif
}


//+++ タイマーで無理やりまって、画面をリサイズで描画
void CALLBACK CChildFrame::OncePaintReq(HWND hWnd, UINT wparam, UINT_PTR lparam, DWORD )
{
	//m_bImageAutoSizeReq = true;
	::SendMessage(hWnd, WM_PAINT, wparam, lparam);
	::KillTimer(hWnd, 1);
}



// It will be a template method.
void CChildFrame::OnMDIActivate(HWND hwndChildDeact, HWND hwndChildAct)
{
	dcfTRACE( _T("CChildFrame::OnMDIActivate\n") );
	SetMsgHandled(FALSE);

	CDonutSearchBar* pSearchBar = CDonutSearchBar::GetInstance();

	if (hwndChildAct == m_hWnd) {		// I'm activated このタブがアクティブになった
	  #if 0 //+++ 強引に、ページ窓開け時のabout:blank設定をキャンセルする... +mod.1.17以前の不具合対策で、どうも以降では帰って不具合に化ける場合がありそなんで、止め.
		CString strURL = GetLocationURL();
		if ((strURL == "about:blank" || strURL.IsEmpty()) && IsWaitBeforeNavigate2Flag()) { //+++
			;
		} else {	//+++ 通常.
			m_AddressBar.SetWindowText(strURL);
		}
	  #else
		m_AddressBar.SetWindowText( GetLocationURL() );
	  #endif
		_SetPageFocus();
		_RestoreFocus();
		if (CSearchBarOption::s_bSaveSearchWord) {
			pSearchBar->SetSearchStr(m_strSearchWord); //\\ 保存しておいた文字列を検索バーに戻す
			pSearchBar->ForceSetHilightBtnOn(m_bNowHilight != 0);
		}

		return;
	}

	if (hwndChildDeact == m_hWnd) { 	// I'm deactivated タブが切り替わった
		_SaveFocus();
		if( m_bSaveSearchWordflg == true ){	//\\ 現在、検索バーにある文字列を取っておく
			pSearchBar->GetEditCtrl().GetWindowText(m_strSearchWord.GetBuffer(1024), 1024);
			m_strSearchWord.ReleaseBuffer();
		} else {
			m_bSaveSearchWordflg = true;
		}
	}

	{
		//プラグインイベント - アクティブタブ変更
		int nIndexDeact = m_MDITab.GetTabIndex(hwndChildDeact);
		int nIndexAct	= m_MDITab.GetTabIndex(hwndChildAct);
		CPluginManager::BroadCast_PluginEvent(DEVT_TAB_CHANGEACTIVE, nIndexDeact, nIndexAct);
	}
}



///+++ メモ:ID_VIEW_SETFOCUS:"現在のページのフォーカスを初期化します。フォーカスのリセット"
void CChildFrame::OnViewSetFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	// NOTE. _SetPageFoucs would fail if document not completed.
	m_bPageFocusInitialized = false;			// reset focus
	_SetPageFocus();
}



void CChildFrame::_SetPageFocus()
{
	dcfTRACE( _T("CChildFrame::_SetPageFocus(src:%d)\n"), ::GetFocus() );

	if (m_bPageFocusInitialized)
		return;

	if ( !MtlIsApplicationActive(m_hWnd) )
		return;

	if ( IsBrowserNull() )
		return;

	CComPtr<IDispatch>		  spDisp;
	HRESULT 				  hr	= m_spBrowser->get_Document(&spDisp);
	if ( FAILED(hr) )
		return;

	CComQIPtr<IHTMLDocument2> spDoc = spDisp;
	if (!spDoc) 								// document not initialized yet
		return;

	CComPtr<IHTMLWindow2>	  spWnd;
	hr						= spDoc->get_parentWindow(&spWnd);
	if (!spWnd)
		return;

	m_bPageFocusInitialized = true; 			// avoid the endless loop
	hr						= spWnd->focus();	// makes mainframe active
}



void CChildFrame::_RestoreFocus()
{
	if (m_hWndFocus && IsChild(m_hWndFocus) && ::GetFocus() != m_hWndFocus)
	{
		::SetFocus(m_hWndFocus);
	}
}





// ===========================================================================
// 個別関係


#if 1	//+++ url別拡張プロパティの処理.
void CChildFrame::SetUrlSecurityExStyle(LPCTSTR lpszFile)
{
	DWORD opts  = 0;
	DWORD opts2 = 0;
	if (CUrlSecurityOption::FindUrl( lpszFile, &opts, &opts2, 0 )) {
		CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, opts, opts2);
		this->view().PutDLControlFlags( ExProp.GetDLControlFlags() );
		this->SetViewExStyle(ExProp.GetExtendedStyleFlags(), TRUE);
		this->view().m_ViewOption.SetAutoRefreshStyle( ExProp.GetAutoRefreshFlag() );
	}
}
#endif



/**+++メモ:ID_VIEW_REFRESH:"現在のページのコンテンツを最新の情報に更新します。"
 */
void CChildFrame::OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (MDIGetActive() == m_hWnd) {
		m_hWndA = NULL;
		m_hWndF = NULL;
	} else {
		m_hWndA = ::GetActiveWindow();
		m_hWndF = ::GetFocus();
	}
	m_bReload = true;
	if (::GetAsyncKeyState(VK_CONTROL) < 0) 	// Inspired by DOGSTORE, Thanks
		Refresh2(REFRESH_COMPLETELY);
	else
		Refresh();
}



/**+++メモ:ID_VIEW_STOP:"現在のページの読み込みを中止します。"
 */
void CChildFrame::OnViewStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	Stop();

	// make sure
	m_nDownloadCounter = 0;
	OnStateCompleted();
}



/**+++メモ:ID_FAVORITE_ADD:"現在のページをお気に入りの一覧に追加します。"
 */
void CChildFrame::OnFavoriteAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	bool bOldShell = _check_flag(MAIN_EX_ADDFAVORITEOLDSHELL, CMainOption::s_dwMainExtendedStyle);

	MtlAddFavorite(GetLocationURL(), MtlGetWindowText(m_hWnd), bOldShell, DonutGetFavoritesFolder(), m_hWnd);

	::SendMessage(GetTopLevelParent(), WM_REFRESH_EXPBAR, 0, 0);
}


/**+++メモ:ID_FAVORITE_GROUP_ADD:"このページをお気に入りグループに追加します。"
 */
void CChildFrame::OnFavoriteGroupAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	TCHAR		szOldPath[MAX_PATH];			// save current directory
	szOldPath[0]	= 0;	//+++
	::GetCurrentDirectory(MAX_PATH, szOldPath);

	CString		dir	= DonutGetFavoriteGroupFolder();
	::SetCurrentDirectory( dir );

	const TCHAR szFilter[] = _T("Donut Favorite Groupファイル(*.dfg)\0*.dfg\0\0");
	CFileDialog fileDlg(FALSE, _T("dfg"), NULL, OFN_HIDEREADONLY, szFilter);
	fileDlg.m_ofn.lpstrInitialDir = dir;
	fileDlg.m_ofn.lpstrTitle	  = _T("お気に入りグループに追加");

	if (fileDlg.DoModal() == IDOK) {
		_AddGroupOption(fileDlg.m_szFileName);
		::SendMessage(GetTopLevelParent(), WM_REFRESH_EXPBAR, 1, 0);
	}

	// restore current directory
	::SetCurrentDirectory(szOldPath);
}



/**+++メモ：お気に入りグループに追加ページのオプション情報を追加
 */
int  CChildFrame::_AddGroupOption(const CString& strFileName)
{
	CIniFileIO	pr( strFileName, _T("Header") );
	DWORD		dwCount = pr.GetValue( _T("count"), 0 );
	OnSaveOption(strFileName, dwCount);
	++dwCount;
	pr.SetValue( dwCount, _T("count") );
	return dwCount;
}



/** 子窓のオプション情報をiniにセーブ
 */
void CChildFrame::OnSaveOption(LPCTSTR lpszFileName, int nIndex)
{
	m_view.m_ViewOption.WriteProfile(lpszFileName, nIndex);
}



//vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// U.H
/**+++メモ:ID_WINDOW_REFRESH_EXCEPT:"このウィンドウ以外を更新"
 */
LRESULT CChildFrame::OnWindowRefreshExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CWaitCursor 		 cur;
	CLockRedrawMDIClient lock(m_hWndMDIClient);
	CLockRedraw 		 lock2(m_MDITab);

	MtlRefreshAllMDIChildrenExcept(m_hWndMDIClient, m_hWnd);
	return 0;
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^




/**+++メモ:ID_EDIT_IGNORE:"ポップアップ抑止に追加して閉じます。"
 */
void CChildFrame::OnEditIgnore(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	dcfTRACE( _T("CChildFrame::OnEditIgnore\n") );
	CIgnoredURLsOption::Add( GetLocationURL() );
	m_bClosing = true;
	PostMessage(WM_CLOSE);
}


// UDT DGSTR ( close Title , but don't post WM_CLOSE so far. )
/**+++ メモ:タイトル抑止に追加
 */
void CChildFrame::OnEditCloseTitle(WORD , WORD , HWND )
{
	ATLTRACE2( atlTraceGeneral, 4, _T("CChildFrame::OnEditCloseTitle\n") );
	CCloseTitlesOption::Add( MtlGetWindowText(m_hWnd) );
	m_bClosing = true;
	PostMessage(WM_CLOSE);
}
// ENDE



/** +++メモ:nページ戻る
 */
void CChildFrame::OnViewBackX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	int 		nStep = (wID - ID_VIEW_BACK1) + 1;
	CLockRedraw lock(m_hWnd);

	for (int i = 0; i < nStep; ++i)
		GoBack();
}



/** +++メモ:nページ進む
 */
void CChildFrame::OnViewForwardX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	int 		nStep = (wID - ID_VIEW_FORWARD1) + 1;
	CLockRedraw lock(m_hWnd);

	for (int i = 0; i < nStep; ++i)
		GoForward();
}



/**+++メモ:ID_TITLE_COPY:"タイトルをクリップボードにコピーします。"
 */
void CChildFrame::OnTitleCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	MtlSetClipboardText(MtlGetWindowText(m_hWnd), m_hWnd);
}



/**+++メモ:ID_URL_COPY:"アドレスをクリップボードにコピーします。"
 */
void CChildFrame::OnUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	MtlSetClipboardText(GetLocationURL(), m_hWnd);
}



/**+++メモ:ID_COPY_TITLEANDURL:"タイトルとアドレスをクリップボードにコピーします。"
 */
void CChildFrame::OnTitleAndUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	CString strText = MtlGetWindowText(m_hWnd) + _T("\r\n");

	strText += GetLocationURL();
	MtlSetClipboardText(strText, m_hWnd);
}

// タブを複製します
void CChildFrame::OnTabClone(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	// OnBeforeNavigate2のナビゲートロックの部分からぱくり

  #if 1	//+++ Url別拡張プロパティ設定の処理. 主にリンク用.(通常のopenの場合は、CMainFrame::OnUserOpenFileな処理を通るので、そちらで設定ずみ)
	DWORD exopts = 0xFFFFFFFF, dlCtlFlg = 0xFFFFFFFF, exstyle = 0xFFFFFFFF, autoRefresh = 0xFFFFFFFF, dwExPropOpt = 8;
	if (CUrlSecurityOption::FindUrl( GetLocationURL(), &exopts, &dwExPropOpt, 0 )) {
		CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, exopts, dwExPropOpt);
		dlCtlFlg	= ExProp.GetDLControlFlags();
		exstyle		= ExProp.GetExtendedStyleFlags();
		autoRefresh = ExProp.GetAutoRefreshFlag();
	}
  #endif

	dlCtlFlg = _GetInheritedDLControlFlags();
	exstyle  = _GetInheritedExtendedStyleFlags();	//+++ 新規に子供を作るときの、その子供に渡す ExtendedStyle を求める...
	CChildFrame *pChild = CChildFrame::NewWindow( m_hWndMDIClient, m_MDITab, m_AddressBar, false, dlCtlFlg, exstyle );
	if (pChild) {
		pChild->SetWaitBeforeNavigate2Flag();	//*+++ 無理やり、BeforeNavigate2()が実行されるまでの間アドレスバーを更新しないようにするフラグをon
		pChild->ActivateFrameForNewWindowDelayed();
		pChild->Navigate2(GetLocationURL());
		pChild->m_strSearchWord = m_strSearchWord;
		#if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
		pChild->m_bNowHilight	= m_bNowHilight;
		#endif

		#if 1	//+++ url別拡張プロパティの処理.
		if (exopts != 0xFFFFFFFF) {
			pChild->view().PutDLControlFlags( dlCtlFlg );
			pChild->SetViewExStyle( exstyle );
			pChild->view().m_ViewOption.SetAutoRefreshStyle( autoRefresh );
		}
		#endif
	}
	return;
}


void CChildFrame::OnFavoritesOpen(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strFilePath = (LPCTSTR)GetTopLevelWindow().SendMessage(WM_GET_FAVORITEFILEPATH, nID);
	ATLASSERT(strFilePath.IsEmpty() == FALSE);

	CString strCheck = strFilePath.Left(2);
	CString strFile;

	if (strFilePath[1] == ' ') {
		strFile = strFilePath.Mid(2);
	} else {
		strFile = strFilePath;
	}
	if ( strCheck == _T("s ") ) {
		CString strFileTitle = MtlGetWindowText(m_hWnd);
		SaveInternetShortcutFile( strFileTitle, strFile, GetLocationURL() );
		return;
	} else if ( strCheck == _T("a ") ) {
		CLockRedrawMDIClient	 lock(m_hWndMDIClient);
		CMDITabCtrl::CLockRedraw lock2(m_MDITab);
		MtlForEachFile( strFile, [this](const CString &strFileName) {
			DonutOpenFile(GetTopLevelWindow(), strFileName);
		});
		return;
	} else if ( strCheck == _T("r ") ) {
		CFavoriteOrderHandler	order;
		CString strRegKey = order.GetOrderKeyName(strFile);
		if ( strRegKey.IsEmpty() )
			return;

		Misc::CRegKey 	rkOrder;
		if (rkOrder.Open(HKEY_CURRENT_USER, strRegKey) != ERROR_SUCCESS) {
			CString strMsg;
			strMsg.Format(_T("レジストリキー\n%s\nのオープンに失敗しました。キーが存在しない可能性があります。"), strRegKey);
			::MessageBox(NULL, strMsg, _T("エラー"), MB_OK);
			return;
		}

		rkOrder.DeleteValue( _T("Order") );
		rkOrder.Close();
		return;
	}

	if (  !MtlIsProtocol( strFile, _T("http") )
		&& !MtlIsProtocol( strFile, _T("https") ) )
	{
		if ( MtlPreOpenFile(strFile) )
			return;
		// handled
	}

	MTL::ParseInternetShortcutFile(strFile);

	if ( strFile == _T("javascript:location.reload()") ) 
		return;

	Navigate2(strFile);
}



/** プログレスペインの幅を設定
 */
BOOL CChildFrame::SetProgressPaneWidth(int cxWidth)
{
  #if _ATL_VER >= 0x700
	CStatusBarCtrl statusbar   = GetTopLevelParent().GetDlgItem(ATL_IDW_STATUS_BAR);
  #else //+++
	CStatusBarCtrl statusbar   = CWindow( GetTopLevelParent() ).GetDlgItem(ATL_IDW_STATUS_BAR);
  #endif

	const int		nPanes	   = 5;
	//+++ プログレスペインとプロキシペインの交換時の不具合修正.
	int 			nIndex	   = (g_bSwapProxy == 0) ? 1 : 4;

	// get pane positions
	int *			pPanesPos  = (int *) _alloca( nPanes * sizeof (int) );

	statusbar.GetParts(nPanes, pPanesPos);

	// calculate offset
	int 		   cxPaneWidth = pPanesPos[nIndex] - ( (nIndex == 0) ? 0 : pPanesPos[nIndex - 1] );
	int 		   cxOff	   = cxWidth - cxPaneWidth;

	// find variable width pane
	int 		   nDef 	   = 0;

	// resize
	if (nIndex < nDef) {						// before default pane
		for (int i = nIndex; i < nDef; i++)
			pPanesPos[i] += cxOff;
	} else {									// after default one
		for (int i = nDef; i < nIndex; i++)
			pPanesPos[i] -= cxOff;
	}

	// set pane postions
	return	statusbar.SetParts(nPanes, pPanesPos) != 0;
}



// ==========================================================================
// UI

// Update Command UI Handlers


void CChildFrame::OnStyleSheetBaseUI(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	}
}


void CChildFrame::OnUpdateFontSmallestUI(CCmdUI *pCmdUI)
{
	CComVariant 	var;
	ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, NULL, &var);
	pCmdUI->SetRadio( var == CComVariant(0L) /*? true : false*/);
}


void CChildFrame::OnUpdateFontSmallerUI(CCmdUI *pCmdUI)
{
	CComVariant 	var;
	ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, NULL, &var);
	pCmdUI->SetRadio( var == CComVariant(1L) /*? true : false*/);
}


void CChildFrame::OnUpdateFontMediumUI(CCmdUI *pCmdUI)
{
	CComVariant 	var;
	ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, NULL, &var);
	pCmdUI->SetRadio( var == CComVariant(2L) /*? true : false*/);
}


void CChildFrame::OnUpdateFontLargerUI(CCmdUI *pCmdUI)
{
	CComVariant 	var;
	ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, NULL, &var);
	pCmdUI->SetRadio(var == CComVariant(3L) /*? true : false*/);
}


void CChildFrame::OnUpdateFontLargestUI(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) { // popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		CComVariant var;
		ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, NULL, &var);
		pCmdUI->SetRadio(var == CComVariant(4L) /*? true : false*/);
	}
}

void CChildFrame::OnUpdateStatusBarUI(CCmdUI *pCmdUI)
{
	pCmdUI->SetText(m_strStatusBar);
}


void CChildFrame::OnUpdateSecureUI(CCmdUI *pCmdUI)
{
	CStatusBarCtrl wndStatus = pCmdUI->m_wndOther;

	if (m_nSecureLockIcon != secureLockIconUnsecure) {
		wndStatus.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(ID_SECURE_PANE, 0), 0);

		// ツールチップのセット
		CString tip = _T("SSL 保護付き ");

		switch (m_nSecureLockIcon) {
		case secureLockIconUnsecure:			tip  = "";					break;
		case secureLockIconMixed:				tip += _T("(複数）");		break;
		case secureLockIconSecureUnknownBits:	tip += _T("(不明）");		break;
		case secureLockIconSecure40Bit: 		tip += _T("(40ビット)");	break;
		case secureLockIconSecure56Bit: 		tip += _T("(56ビット)");	break;
		case secureLockIconSecureFortezza:		tip += _T("(Fortezza)");	break;
		case secureLockIconSecure128Bit:		tip += _T("(128ビット)");	break;
		default:								tip  = "";
		}
		wndStatus.SendMessage( WM_STATUS_SETTIPTEXT, (WPARAM) ID_SECURE_PANE, (LPARAM) tip.GetBuffer(0) );

	} else {
		wndStatus.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(ID_SECURE_PANE, -1), 0);
	}
}


void CChildFrame::OnUpdatePrivacyUI(CCmdUI *pCmdUI)
{
	CStatusBarCtrl wndStatus = pCmdUI->m_wndOther;

	if (m_bPrivacyImpacted == FALSE)
		wndStatus.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(ID_PRIVACY_PANE, 1), 0);
	else
		wndStatus.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(ID_PRIVACY_PANE, -1), 0);

	wndStatus.SendMessage( WM_STATUS_SETTIPTEXT, (WPARAM) ID_PRIVACY_PANE, (LPARAM) _T("プライバシー レポート") );
}


///+++ メモ:ページ読み込みプログレスバーの状態更新.
void CChildFrame::OnProgressChange(long progress, long progressMax)
{
	m_nProgress    = progress;
	m_nProgressMax = progressMax;
}



///+++ メモ:ページ読み込みプログレスバーの表示
void CChildFrame::OnUpdateProgressUI(CCmdUI *pCmdUI)
{
	CProgressBarCtrl progressbar = pCmdUI->m_wndOther;
	CStatusBarCtrl	 statusbar	 = pCmdUI->m_wndOther.GetParent();


	if ( m_nProgress == -1 || (m_nProgress == 0 && m_nProgressMax == 0) ) {
		if (g_bSwapProxy == 0) {		//+++ 通常(左側にある)時のみ幅を0にする. プロキシと位置を交換してるときはやめ.
			SetProgressPaneWidth(0);
			progressbar.ShowWindow(SW_HIDE);
		} else {						//+++ 交換してるときは、とりあえず、長さ0で出しっぱなしで対処.
			progressbar.SetPos(0);		//+++ ただしゲージバー自体は消しておく
		}
		return;
	}

	CRect			 rcProgressPart;
	if (g_bSwapProxy == false)
		statusbar.GetRect(1, rcProgressPart);
	else
		statusbar.GetRect(4, rcProgressPart);

	rcProgressPart.DeflateRect(2, 2);
	progressbar.MoveWindow(rcProgressPart, TRUE);
	progressbar.SetRange32(0, m_nProgressMax);
	//x 0になっているときに毎フレームSetPos(0)するようにしたので、ここでは不要
	//x (というか、すこしでも最大値描画が行われるようにするため、無いほうがよい)
	//x if (m_nProgress >= m_nProgressMax && m_nProgressMax && g_bSwapProxy)
	//x progressbar.SetPos(0);
	//x else
	progressbar.SetPos(m_nProgress);
	progressbar.ShowWindow(SW_SHOWNORMAL);

	//+++ サイズ固定でオプション設定でのサイズが反映されていなかったのでそれを反映. グローバル変数なのはとりあえず...
	SetProgressPaneWidth( g_dwProgressPainWidth );
}



// ==========================================================================
// 拡張プロパティ

BYTE CChildFrame::GetTabItemState(HWND hTarWnd)
{
	int  nTabIndex = m_MDITab.GetTabIndex(hTarWnd);
	BYTE bytData   = 0;

	m_MDITab.GetItemState(nTabIndex, bytData);
	return bytData;
}



LRESULT CChildFrame::OnGetExtendedTabStyle()
{
	DWORD dwFlags		 = 0;
	DWORD dwDLFlags 	 = m_view.GetDLControlFlags();
	DWORD dwViewExStyle  = m_view.m_ViewOption.m_dwExStyle;
	DWORD dwRefreshStyle = m_view.m_ViewOption.m_dwAutoRefreshStyle;

	if (dwDLFlags & DLCTL_DLIMAGES) 				dwFlags |= FLAG_SE_DLIMAGES;
	if (dwDLFlags & DLCTL_VIDEOS)					dwFlags |= FLAG_SE_VIDEOS;
	if (dwDLFlags & DLCTL_BGSOUNDS) 				dwFlags |= FLAG_SE_BGSOUNDS;
	if ( !(dwDLFlags & DLCTL_NO_RUNACTIVEXCTLS) )	dwFlags |= FLAG_SE_RUNACTIVEXCTLS;
	if ( !(dwDLFlags & DLCTL_NO_DLACTIVEXCTLS) )	dwFlags |= FLAG_SE_DLACTIVEXCTLS;
	if ( !(dwDLFlags & DLCTL_NO_SCRIPTS) )			dwFlags |= FLAG_SE_SCRIPTS;
	if ( !(dwDLFlags & DLCTL_NO_JAVA) ) 			dwFlags |= FLAG_SE_JAVA;
	if (dwViewExStyle & DVS_EX_OPENNEWWIN)			dwFlags |= FLAG_SE_NAVIGATELOCK;
	if (dwViewExStyle & DVS_EX_FLATVIEW)			dwFlags |= FLAG_SE_FLATVIEW;
	if (dwViewExStyle & DVS_EX_MESSAGE_FILTER)		dwFlags |= FLAG_SE_MSGFILTER;
	if (dwViewExStyle & DVS_EX_MOUSE_GESTURE)		dwFlags |= FLAG_SE_MOUSEGESTURE;
	if (dwViewExStyle & DVS_EX_BLOCK_MAILTO)		dwFlags |= FLAG_SE_BLOCKMAILTO;

	BYTE bytState = GetTabItemState(m_hWnd);
	if ( !(bytState & TCISTATE_INACTIVE) )			dwFlags |= FLAG_SE_VIEWED;

	if (dwRefreshStyle == 0)
		dwFlags |= FLAG_SE_REFRESH_NONE;
	else
		dwFlags |= ( dwRefreshStyle * ( FLAG_SE_REFRESH_15 / DVS_AUTOREFRESH_15SEC) );

	return dwFlags;
}



void CChildFrame::OnSetExtendedTabStyle(DWORD dwStyle)
{
	DWORD dwDLFlags 	 = m_view.GetDLControlFlags();
	DWORD dwViewExStyle  = m_view.m_ViewOption.m_dwExStyle;
	DWORD dwRefreshStyle = ( dwStyle /	( FLAG_SE_REFRESH_15 / DVS_AUTOREFRESH_15SEC) ) & DVS_AUTOREFRESH_OR;

	if (dwStyle & FLAG_SE_DLIMAGES) 		dwDLFlags |=  DLCTL_DLIMAGES;
	else									dwDLFlags &= ~DLCTL_DLIMAGES;
	if (dwStyle & FLAG_SE_VIDEOS)			dwDLFlags |=  DLCTL_VIDEOS;
	else									dwDLFlags &= ~DLCTL_VIDEOS;
	if (dwStyle & FLAG_SE_BGSOUNDS) 		dwDLFlags |=  DLCTL_BGSOUNDS;
	else									dwDLFlags &= ~DLCTL_BGSOUNDS;
	if (dwStyle & FLAG_SE_RUNACTIVEXCTLS)	dwDLFlags &= ~DLCTL_NO_RUNACTIVEXCTLS;
	else									dwDLFlags |=  DLCTL_NO_RUNACTIVEXCTLS;
	if (dwStyle & FLAG_SE_DLACTIVEXCTLS)	dwDLFlags &= ~DLCTL_NO_DLACTIVEXCTLS;
	else									dwDLFlags |=  DLCTL_NO_DLACTIVEXCTLS;
	if (dwStyle & FLAG_SE_SCRIPTS)			dwDLFlags &= ~DLCTL_NO_SCRIPTS;
	else									dwDLFlags |=  DLCTL_NO_SCRIPTS;
	if (dwStyle & FLAG_SE_JAVA) 			dwDLFlags &= ~DLCTL_NO_JAVA;
	else									dwDLFlags |=  DLCTL_NO_JAVA;

	if (dwStyle & FLAG_SE_NAVIGATELOCK) 	dwViewExStyle |=  DVS_EX_OPENNEWWIN;
	else									dwViewExStyle &= ~DVS_EX_OPENNEWWIN;
	if (dwStyle & FLAG_SE_FLATVIEW) 		dwViewExStyle |=  DVS_EX_FLATVIEW;
	else									dwViewExStyle &= ~DVS_EX_FLATVIEW;			//* +++ ここに ~ がないのはワザとなのかバグなのか? おそらくバグだろうで~しとく.
	if (dwStyle & FLAG_SE_MSGFILTER)		dwViewExStyle |=  DVS_EX_MESSAGE_FILTER;
	else									dwViewExStyle &= ~DVS_EX_MESSAGE_FILTER;
	if (dwStyle & FLAG_SE_MOUSEGESTURE) 	dwViewExStyle |=  DVS_EX_MOUSE_GESTURE;
	else									dwViewExStyle &= ~DVS_EX_MOUSE_GESTURE;
	if (dwStyle & FLAG_SE_BLOCKMAILTO)		dwViewExStyle |=  DVS_EX_BLOCK_MAILTO;
	else									dwViewExStyle &= ~DVS_EX_BLOCK_MAILTO;

	int nIndex = m_MDITab.GetTabIndex(m_hWndA);

	if (dwStyle & FLAG_SE_VIEWED)
		m_MDITab.SetItemActive(nIndex);
	else
		m_MDITab.SetItemInactive(nIndex);

	//ナビゲートロックの変更を反映させる
	// if ( (m_view.m_ViewOption.m_dwExStyle & DVS_EX_OPENNEWWIN) != (dwStyle & FLAG_SE_NAVIGATELOCK) )
	//+++ なにかよくわかりにくい条件式だ...DVS_EX_OPENNEWWIN=0x01, FLAG_SE_NAVIGATELOCK=0x80なので、
	//+++ !=が偽になるのは両辺が共に0のときのみ...なので、どちらかがonになっていればif文の中にはいる.
	//+++ とりあえず、そのように記述しなおす
	if ( (m_view.m_ViewOption.m_dwExStyle & DVS_EX_OPENNEWWIN) || (dwStyle & FLAG_SE_NAVIGATELOCK) )
	{
		m_MDITab.NavigateLockTab(m_hWnd, (dwStyle & FLAG_SE_NAVIGATELOCK) != 0/*? true : false*/);
	}

	m_MDITab.InvalidateRect(NULL);

	//フラグを変更する
	m_view.PutDLControlFlags(dwDLFlags);
	m_view.m_ViewOption.m_dwExStyle = dwViewExStyle;
	m_view.m_ViewOption.SetAutoRefreshStyle(dwRefreshStyle);

	//変更を適用するためにリフレッシュする
	//m_view.Navigate2(m_view.GetLocationURL());
	if ( !(dwStyle & FLAG_SE_NOREFRESH) )
		::SendMessage(m_hWnd, WM_COMMAND, (WPARAM) (ID_VIEW_REFRESH & 0xFFFF), 0);
}



void CChildFrame::SetViewExStyle(DWORD dwStyle, BOOL bExProp /*= FALSE*/)
{
	bool bNavigateLock = (dwStyle & DVS_EX_OPENNEWWIN) != 0/*? true : false*/;

	m_MDITab.NavigateLockTab(m_hWnd, bNavigateLock);
	//m_view.m_dwDefaultExtendedStyleFlags	= dwStyle;			//+++
	m_view.m_ViewOption.m_dwExStyle = dwStyle;
	m_bExPropLock					= (bNavigateLock && bExProp) /*? true : false*/;
}



// ==========================================================================
// 進む戻る


void	CChildFrame::SetDfgFileNameSection(const CString &strFileName, const CString &strSection)
{
	m_strDfgFileName = strFileName;
	m_strSection	 = strSection;
}



void CChildFrame::SetArrayHist(
			std::vector< std::pair<CString, CString> > &	ArrayFore,
			std::vector< std::pair<CString, CString> > &	ArrayBack
){
	m_ArrayHistFore = ArrayFore;
	m_ArrayHistBack = ArrayBack;
}



void CChildFrame::SetTravelLogData()
{
	if (m_bInitTravelLog) {
		if (m_ArrayHistBack.size() == 0 && m_ArrayHistFore.size() == 0) {
			//iniから読み込む
			std::vector<std::pair<CString, CString> > ArrFore;
			std::vector<std::pair<CString, CString> > ArrBack;
			_Load_TravelData(ArrFore, ArrBack, m_strDfgFileName, m_strSection);
			_Load_TravelLog(ArrFore, m_spBrowser, TRUE);
			_Load_TravelLog(ArrBack, m_spBrowser, FALSE);
		} else {
			//配列のデータを使う
			_Load_TravelLog(m_ArrayHistFore, m_spBrowser, TRUE);
			_Load_TravelLog(m_ArrayHistBack, m_spBrowser, FALSE);
			m_ArrayHistBack.clear();
			m_ArrayHistFore.clear();
		}
	}

	m_bInitTravelLog = FALSE;
}



BOOL CChildFrame::_Load_TravelData(
	std::vector<std::pair<CString, CString> >&	arrFore,
	std::vector<std::pair<CString, CString> >&	arrBack,
	CString & 									strFileName,
	CString & 									strSection)
{
	if ( strFileName.IsEmpty() )
		return FALSE;

	if ( strSection.IsEmpty() )
		return FALSE;

	CIniFileI	pr(strFileName, strSection);
	CString 		strKey;
	for (unsigned i = 0; i < 0x7fffffff; ++i) {
		strKey.Format(_T("Fore_URL%d"), i);
		CString strURL = pr.GetString(strKey/*, NULL, MAX_PATH*/);
		if (strURL.IsEmpty())
			break;
		strKey.Format(_T("Fore_Title%d"), i);
		CString strTitle = pr.GetStringUW(strKey/*, NULL, 1024*/);
		if (strTitle.IsEmpty())
			break;
		arrFore.push_back( std::make_pair(strTitle, strURL) );
	}

	for (unsigned i = 0; i < 0x7fffffff; ++i) {
		strKey.Format(_T("Back_URL%d"), i);
		CString strURL = pr.GetString(strKey/*, NULL, MAX_PATH*/);
		if (strURL.IsEmpty())
			break;
		strKey.Format(_T("Back_Title%d"), i);
		CString strTitle = pr.GetStringUW(strKey/*, NULL, 1024*/);
		if (strTitle.IsEmpty())
			break;
		arrBack.push_back( std::make_pair(strTitle, strURL) );
	}

	return TRUE;
}



BOOL CChildFrame::_Load_TravelLog(
	std::vector<std::pair<CString, CString> >&	arrLog,
	CComPtr<IWebBrowser2> 						pWB2,
	BOOL 										bFore)
{
	if (arrLog.size() == 0)
		return FALSE;

	HRESULT 					 hr;
	CComPtr<IServiceProvider>	 pISP;
	CComPtr<ITravelLogStg>		 pTLStg;
	CComPtr<IEnumTravelLogEntry> pTLEnum;
	CComPtr<ITravelLogEntry>	 pTLEntryBase = NULL;
	int 						 nDir;

	nDir   = bFore ? TLEF_RELATIVE_FORE : TLEF_RELATIVE_BACK;

	if (pWB2 == NULL)
		return FALSE;

	hr	   = pWB2->QueryInterface(IID_IServiceProvider, (void **) &pISP);
	if (FAILED(hr) || pISP == NULL)
		return FALSE;

	hr	   = pISP->QueryService(SID_STravelLogCursor, IID_ITravelLogStg, (void **) &pTLStg);
	if (FAILED(hr) || pTLStg == NULL)
		return FALSE;

	hr	   = pTLStg->EnumEntries(nDir, &pTLEnum);
	if (FAILED(hr) || pTLEnum == NULL)
		return FALSE;

	int		nLast	= ( 10 > arrLog.size() ) ? (int) arrLog.size() : 10;
	for (int i = 0; i < nLast; i++) {
		CComPtr<ITravelLogEntry> pTLEntry;

	  #if 1	//+++ UNICODE 対応.
		std::vector<wchar_t>	title = Misc::tcs_to_wcs( LPCTSTR( arrLog[i].first ) );
		std::vector<wchar_t>	url   = Misc::tcs_to_wcs( LPCTSTR( arrLog[i].second) );
		// CreateEntry の第四引数がTRUEだと前に追加される
		hr			 = pTLStg->CreateEntry(&url[0], &title[0], pTLEntryBase, !bFore, &pTLEntry);
	  #else
		CString 	strTitle	= arrLog[i].first;
		CString 	strURL		= arrLog[i].second;

		LPOLESTR				 pszwURL   = _ConvertString(strURL, strURL.GetLength() + 1);		//new
		LPOLESTR				 pszwTitle = _ConvertString(strTitle, strTitle.GetLength() + 1);	//new

		hr			 = pTLStg->CreateEntry(pszwURL, pszwTitle, pTLEntryBase, !bFore, &pTLEntry);
		delete[] pszwURL;																			//Don't forget!
		delete[] pszwTitle;
	  #endif

		if (FAILED(hr) || pTLEntry == NULL)
			return FALSE;

		if (pTLEntryBase)
			pTLEntryBase.Release();

		pTLEntryBase = pTLEntry;
	}

	return TRUE;
}



#if 0	//+++ UNICODE対応の折、不要化
LPOLESTR CChildFrame::_ConvertString(LPCTSTR lpstrBuffer, int nBufferSize)
{
	LPOLESTR pszwDest = new OLECHAR[ nBufferSize ];

	::MultiByteToWideChar( CP_ACP, 0, lpstrBuffer, nBufferSize, pszwDest, nBufferSize * sizeof (OLECHAR) );
	return pszwDest;
}
#endif




// ===========================================================================
// 拡大関係

#ifndef USE_DIET
/// 画像ファイルならIEの設定にしたがってサイズ調整する
void CChildFrame::CheckImageAutoSize(const CString* pStrURL, BOOL bFirst)
{
	//+++ CString strExt = pStrURL ? MtlGetExt(*pStrURL, false) : CString("");
	CString 	strExt = pStrURL ? Misc::GetFileExt(*pStrURL) : CString("");
	strExt.MakeLower();

	m_bImg = false;	//+++
	if ( m_nImgWidth != -1
	   || strExt == _T("bmp") || strExt == _T("jpg") || strExt == _T("jpeg")
	   || strExt == _T("gif") || strExt == _T("png") )
	{
	  #if 0	//+++ レサイズの有無は、IEのレジストリ設定を参照.
		//if (CMainOption::s_bStretchImage)
		{
			Misc::CRegKey 	rk;
			LONG		lRet  = rk.Open( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main") );
			if (lRet != ERROR_SUCCESS)
				return;
			TCHAR	buf[32];
			buf[0]		= 0;
			ULONG	num = 30;
			lRet = rk.QueryStringValue(_T("Enable AutoImageResize"), buf, &num);
			//if (memcmp(buf, "yes", 4) != 0)		//+++ yes 以外が設定されていたらno扱い
			if (lstrcmp(buf, _T("no")) == 0)		//+++ noが設定されていたら帰る。それ以外はyes扱い.(レジストリがないときはyesの扱いにするため)
				return;
		}
	  #else	//+++ クリックによるサイズ切り替えが無いというのを、自動リサイズなし、という意味に変えた.
		if (m_bImgAuto_NouseLClk)
			return;
	  #endif
		m_nImgScl	= 100;		//+++

	  #if 1	//+++ ちょっとやり方をかえてみる...
		CComPtr<IHTMLDocument2> 			pDoc;
		if ( FAILED( m_spBrowser->get_Document( (IDispatch **) &pDoc ) ) )
			return;

		CComPtr<IHTMLElementCollection> 	pElemClct;
		if ( FAILED( pDoc->get_images(&pElemClct) ) )
			return;

		long	length = 0;
		pElemClct->get_length(&length);

		CComPtr<IDispatch>			pDisp;
		CComPtr<IHTMLImgElement>	pImg;
		CComVariant 				varName ( (long) 0 );
		CComVariant 				varIndex( (long) 0 );
		if ( FAILED( pElemClct->item(varName, varIndex, &pDisp) ) )
			return;
		if (!pDisp)		//+++ 失敗してNULLのことがあるようなんでチェック
			return;
		if ( FAILED( pDisp->QueryInterface(&pImg) ) )
			return;

		long	Width  = m_nImgWidth;
		long	Height = m_nImgHeight;
		if (bFirst) {
			if ( FAILED( pImg->get_width(&Width) ) )
				return;
			if ( FAILED( pImg->get_height(&Height) ) )
				return;
			m_nImgWidth  = Width;
			m_nImgHeight = Height;
		}

		CComPtr<IHTMLElement>	spHTMLElement;
		if ( FAILED( pDoc->get_body(&spHTMLElement) ) )
			return;
		if (!spHTMLElement)
			return;
		CComPtr<IHTMLElement2>  spHTMLElement2;
		if ( FAILED( spHTMLElement->QueryInterface(&spHTMLElement2)) )
			return;
		if (!spHTMLElement2)
			return;

		long 	scWidth  = 0;
		long 	scHeight = 0;
	   #if 1	//+++
		if ( FAILED( m_spBrowser->get_Width(&scWidth) ) )
			return;
		if ( FAILED( m_spBrowser->get_Height(&scHeight) ) )
			return;
	   #else	// clientWidth,Heightの値が、たまにへんな値が帰ってくる...
		if ( FAILED(spHTMLElement2->get_clientWidth (&scWidth )) )
			return;
		if ( FAILED(spHTMLElement2->get_clientHeight(&scHeight)) )
			return;
	   #endif

		m_bImg		= true;		//+++

		enum { MGN = 20/*32*/ };
		scWidth  = (scWidth  - MGN < 0) ? 1 : scWidth  - MGN;
		scHeight = (scHeight - MGN < 0) ? 1 : scHeight - MGN;

		//if (scWidth < Width || scHeight < Height)
		{
			double  sclW = scWidth  * 100.0 / Width;
			double  sclH = scHeight * 100.0 / Height;
			double  scl  = sclW < sclH ? sclW : sclH;
			if (scl < 1) {
				scl = 1;
			} else if (scl >= 100) {
				scl			= 100;
				m_nImgSclSav = 100;
			}
			if (m_nImgSclSw == 0) {
				m_nImgSclSav = int(scl);
				m_nImgSclSw  = 0;
				scl			 = 100;
			}
			m_nImgScl = int( scl );
			wchar_t		wbuf[128];
			//swprintf(wbuf, 128, L"%d%%", int(scl));
			wsprintfW(wbuf, L"%d%%", int(scl));
			CComVariant variantVal(wbuf);
			CComBSTR	bstrZoom( L"zoom" );

			CComPtr<IHTMLStyle>		pHtmlStyle;
			if ( FAILED(spHTMLElement2->get_runtimeStyle(&pHtmlStyle)) )
				return;
			if (!pHtmlStyle)
				return;
			pHtmlStyle->setAttribute(bstrZoom, variantVal, 1);
		}
	  #else	//+++ 元...消し
	  #endif
	}
}
#endif


///+++
LRESULT CChildFrame::OnHtmlZoomMenu(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	::SetForegroundWindow(m_hWnd);
	CMenu/*Handle*/ 	menu0;
	menu0.LoadMenu(IDR_ZOOM_MENU);
	if (menu0.m_hMenu == NULL)
		return 0;
	CMenuHandle menu = menu0.GetSubMenu(0);
	if (menu.m_hMenu == NULL)
		return 0;

	// ポップアップメニューを開く.
	POINT 	pt;
	::GetCursorPos(&pt);
	HRESULT hr = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON| TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
	if (hr)
		MtlSendCommand(m_hWnd, WORD(hr));
	return 0;
}



///+++
LRESULT CChildFrame::OnHtmlZoomAdd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	SetBodyStyleZoom(10, 0);
	return 0;
}



///+++
LRESULT CChildFrame::OnHtmlZoomSub(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	SetBodyStyleZoom(-10, 0);
	return 0;
}



///+++ 100% とその他の比率をトグル切替.
LRESULT CChildFrame::OnHtmlZoom100Togle(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL &)
{
	if (m_nImgScl == 100) {
		m_nImgScl	 = m_nImgSclSav;
		m_nImgSclSw	 = 1;
	} else {
		m_nImgSclSav = m_nImgScl;
		m_nImgScl    = 100;
		m_nImgSclSw	 = 0;
	}
	SetBodyStyleZoom(0, m_nImgScl);
	return 0;
}



///+++
void CChildFrame::OnHtmlZoom(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	ATLASSERT(wID >= ID_HTMLZOOM_400 && wID <= ID_HTMLZOOM_050);
	static const int scls[] = { 400, 200, 150, 125, 100, 75, 50 };
	int  n = wID - ID_HTMLZOOM_400;
	if (n < 0 || n > ID_HTMLZOOM_050-ID_HTMLZOOM_400)
		return;
	SetBodyStyleZoom(0, scls[n]);
	return;
}



///+++ htmlを拡大縮小
void CChildFrame::SetBodyStyleZoom(int addSub, int scl)
{
	if (Misc::getIEMejourVersion() >= 7) {
		CComVariant vZoom;
		if (addSub) {
			m_spBrowser->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, NULL, &vZoom);
			scl =  static_cast<int>(vZoom.lVal);
			scl += addSub;
		}
		if (scl < 5)
			scl = 5;
		else if (scl > 500)
			scl = 500;

		m_nImgScl = scl;
		
		vZoom.lVal = scl;
		m_spBrowser->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vZoom, NULL); 


	} else {
		CComPtr<IHTMLDocument2> 			pDoc;
		if ( FAILED( m_spBrowser->get_Document( (IDispatch **) &pDoc ) ) )
			return;

		CComPtr<IHTMLElement>	spHTMLElement;
		if ( FAILED( pDoc->get_body(&spHTMLElement) ) )
			return;
		if (!spHTMLElement)
			return;
		CComPtr<IHTMLElement2>  spHTMLElement2;
		if ( FAILED( spHTMLElement->QueryInterface(&spHTMLElement2)) )
			return;
		if (!spHTMLElement2)
			return;

		CComPtr<IHTMLStyle>		pHtmlStyle;
		if ( FAILED(spHTMLElement2->get_runtimeStyle(&pHtmlStyle)) )
			return;
		if (!pHtmlStyle)
			return;

		wchar_t		wbuf[128];
		wbuf[0] = 0;
		CComVariant variantVal; //(wbuf);
		CComBSTR	bstrZoom( L"zoom" );
		if (addSub) {
			if ( FAILED(pHtmlStyle->getAttribute(bstrZoom, 1, &variantVal)) )
				return;
			CComBSTR	bstrTmp(variantVal.bstrVal);
			scl =  !bstrTmp.m_str ? 100 : wcstol((wchar_t*)LPCWSTR(bstrTmp.m_str), 0, 10);
			scl += addSub;
		}
		if (scl < 5)
			scl = 5;
		else if (scl > 500)
			scl = 500;

		m_nImgScl = scl;

		//swprintf(wbuf, 128, L"%d%%", int(scl));
		wsprintfW(wbuf, L"%d%%", int(scl));
		variantVal = CComVariant(wbuf);

		pHtmlStyle->setAttribute(bstrZoom, variantVal, 1);
	}
}



// ===========================================================================
// スタイルシート

#ifndef NO_STYLESHEET

LRESULT CChildFrame::OnChangeCSS(LPCTSTR lpszStyleSheet)
{
	CString strSheetPath(lpszStyleSheet);
	CString strSheetName = strSheetPath.Mid(strSheetPath.ReverseFind('\\') + 1);

	StyleSheet(strSheetName, FALSE, strSheetPath);

	SetDefaultStyleSheet(strSheetPath);

	return 1;
}


LRESULT CChildFrame::OnStyleSheet(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CString strSheet;
	BOOL	bOff = FALSE;

	switch (wID) {
	case ID_STYLESHEET_BASE:
		break;
	case ID_STYLESHEET_OFF:
		bOff = TRUE;
		break;
	}

	StyleSheet( strSheet, bOff, _T("") );
	SetDefaultStyleSheet( _T("") );

	return 1;
}



void CChildFrame::SetDefaultStyleSheet(const CString& strStyleSheet)
{
	CIniFileIO	pr( g_szIniFileName, _T("StyleSheet") );
	DWORD		dwSetDefault = pr.GetValue( _T("SetDefault") );
	if (dwSetDefault)
		pr.SetString( strStyleSheet, _T("Default") );
}



//public:
void CChildFrame::StyleSheet(CString strSheet, BOOL bOff, CString strSheetPath)
{
	CComPtr<IDispatch>		spDisp;
	HRESULT 				hr = m_spBrowser->get_Document(&spDisp);
	if ( FAILED(hr) )
		return;

	CComPtr<IHTMLDocument2> spDocument;
	hr = spDisp->QueryInterface(&spDocument);
	if ( FAILED(hr) )
		return;

	StyleSheetRecursive(spDocument, strSheet, bOff, strSheetPath);
}


//private:
void CChildFrame::StyleSheetRecursive(CComPtr<IHTMLDocument2> spDocument, CString strSheet, BOOL bOff, CString strSheetPath)
{
	if ( !CheckFrameDefinePage(spDocument) )
		StyleSheetSub(spDocument, strSheet, bOff, strSheetPath);

	HRESULT 	hr;
	CComPtr<IHTMLFramesCollection2> spFrames;
	hr = spDocument->get_frames(&spFrames);

	// cf. Even if no frame, get_frames would succeed.
	if ( FAILED(hr) )
		return;

	long		nCount = 0;
	hr = spFrames->get_length(&nCount);
	if ( FAILED(hr) )
		return;

	for (LONG ii = 0; ii < nCount; ii++) {
		CComVariant 	varItem(ii);
		CComVariant 	varResult;

		hr = spFrames->item(&varItem, &varResult);
		if ( FAILED(hr) )
			continue;

		CComQIPtr<IHTMLWindow2> 	spWindow = varResult.pdispVal;
		if (!spWindow)
			continue;

		CComPtr<IHTMLDocument2> 	spDocumentFr;
		hr = spWindow->get_document(&spDocumentFr);
		if ( FAILED(hr) )
			continue;

		StyleSheetRecursive(spDocumentFr, strSheet, bOff, strSheetPath);
	}
}


BOOL CChildFrame::CheckFrameDefinePage(CComPtr<IHTMLDocument2> spDocument)
{
	CComPtr<IHTMLElementCollection> spClct;
	HRESULT 						hr		= spDocument->get_all(&spClct);

	if ( SUCCEEDED(hr) ) {
		CComVariant 	   varStr( CComBSTR( _T("frameSet") ) );
		CComPtr<IDispatch> spDisp;
		hr = spClct->tags(varStr, &spDisp);

		if (SUCCEEDED(hr) && spDisp) {
			CComQIPtr<IHTMLElementCollection> spFrameSetClct = spDisp;
			LONG							  lCount		 = 0;
			hr = spFrameSetClct->get_length(&lCount);

			if (SUCCEEDED(hr) && lCount)
				return TRUE;
		}
	}

	return FALSE;
}


void CChildFrame::StyleSheetSub(IHTMLDocument2 *pDocument, CString strSheet, BOOL bOff, CString strSheetPath)
{
	// スタイルシート一覧を取得
	CComPtr<IHTMLStyleSheetsCollection> spSheetsColl;
	pDocument->get_styleSheets(&spSheetsColl);

	if (!spSheetsColl)
		return;

	bool	bFindTarSheet = false;
	long	lLength;
	spSheetsColl->get_length(&lLength);

	for (long i = 0; i < lLength; i++) {
		VARIANT varIdx, varSheet;
		varIdx.vt		  = VT_I4;
		varIdx.lVal 	  = i;
		varSheet.vt 	  = VT_DISPATCH;
		varSheet.pdispVal = NULL;
		spSheetsColl->item(&varIdx, &varSheet);

		if (varSheet.pdispVal) {
			CComPtr<IHTMLStyleSheet> spSheet2 = NULL;
			varSheet.pdispVal->QueryInterface(IID_IHTMLStyleSheet, (void **) &spSheet2);

			CComBSTR	strTitle;
			spSheet2->get_title(&strTitle);

			CString 	strTarSheet(strTitle);

			if (strTarSheet == strSheet && !bOff) {
				spSheet2->put_disabled(VARIANT_FALSE);
				bFindTarSheet = true;
			} else {
				spSheet2->put_disabled(VARIANT_TRUE);
			}
		}

		varSheet.pdispVal->Release();
	}

	if ( !bFindTarSheet && !bOff && !strSheetPath.IsEmpty() ) {
		CComPtr<IHTMLStyleSheet> spSheet;
		pDocument->createStyleSheet(NULL, -1, &spSheet);

		if (spSheet) {
			// 属性を設定
			long lPos;
			spSheet->addImport( (BSTR) CComBSTR(strSheetPath), -1, &lPos );
			spSheet->put_title( (BSTR) CComBSTR(strSheet) );
			spSheet->put_disabled(VARIANT_FALSE);
		}
	}
}


//private:
void CChildFrame::ApplyDefaultStyleSheet()
{
	CIniFileI pr( g_szIniFileName, _T("StyleSheet") );
	CString 	strSheet = pr.GetString(_T("Default"));
	pr.Close();

	if ( !strSheet.IsEmpty() ) {
		StyleSheet(MtlGetFileName(strSheet), FALSE, strSheet);
	}
}

#endif	// NO_STYLESHEET



// ===========================================================================
// 範囲選択関係


///+++ 選択範囲の文字列を取得(フレーム対応版)
CString CChildFrame::GetSelectedText()
{
	CString 	strSelText;
	//m_MDITab.SetLinkState(LINKSTATE_B_ON);
	MtlForEachHTMLDocument2( m_spBrowser, [&strSelText](IHTMLDocument2 *pDocument) {
		CComPtr<IHTMLSelectionObject>		spSelection;
		HRESULT 	hr	= pDocument->get_selection(&spSelection);
		if ( SUCCEEDED( hr ) ) {
			CComPtr<IDispatch>				spDisp;
			hr	   = spSelection->createRange(&spDisp);
			if ( SUCCEEDED( hr ) ) {
				CComQIPtr<IHTMLTxtRange>	spTxtRange = spDisp;
				if (spTxtRange != NULL) {
					CComBSTR				bstrText;
					hr	   = spTxtRange->get_text(&bstrText);
					if (SUCCEEDED(hr) && !!bstrText)
						strSelText = bstrText;
				}
			}
		}
	});
	//m_MDITab.SetLinkState(LINKSTATE_OFF);

	if (strSelText.IsEmpty()) {
		//+++ 選択範囲がない場合、右クリックメニュー経由のことがあるので、それ対策を強引に...
		CCustomContextMenu* pMenu = CCustomContextMenu::GetInstance();
		if (pMenu && pMenu->GetContextMenuMode() == CONTEXT_MENU_ANCHOR && pMenu->GetAnchorUrl().IsEmpty() == 0) {
			//bstrText		= pMenu->GetAnchorUrl();
			//bstrLocationUrl = pMenu->GetAnchorUrl();
			strSelText = CString(pMenu->GetAnchorUrl());
		}
	}
	return strSelText;
}



//+++ _OpenSelectedText()より分離.
CString CChildFrame::GetSelectedTextLine()
{
	CString str = GetSelectedText();
	if (str.IsEmpty() == 0) {
		int 	n	= str.Find(_T("\r"));
		int 	n2	= str.Find(_T("\n"));
		if ((n < 0 || n > n2) && n2 >= 0)
			n = n2;
		if (n >= 0) {
			str = str.Left(n);
		}
	}
	return str;
}



/// 選択範囲のリンクを開く
void CChildFrame::OnEditOpenSelectedRef(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	dcfTRACE( _T("CChildFrame::OnEditOpenSelectedRef\n") );
	m_MDITab.SetLinkState(LINKSTATE_B_ON);
	CSimpleArray<CString> arrUrls;
	MtlForEachHTMLDocument2( m_spBrowser, [&arrUrls, this] (IHTMLDocument2 *pDocument) {
		CComPtr<IHTMLSelectionObject>	spSelection;
		HRESULT 	hr	= pDocument->get_selection(&spSelection);
		if ( FAILED(hr) )
			return;

		CComPtr<IDispatch>				spDisp;
		hr = spSelection->createRange(&spDisp);
		if ( FAILED(hr) )
			return;

		CComQIPtr<IHTMLTxtRange>		spTxtRange = spDisp;
		if (!spTxtRange)
			return;

		CComBSTR						bstrLocationUrl;
		CComBSTR						bstrText;
		hr = spTxtRange->get_htmlText(&bstrText);
		if (FAILED(hr) || !bstrText) {	//+++
		  #if 1 //+++ 選択範囲がない場合、右クリックメニュー経由のことがあるので、それ対策を強引に...
			CCustomContextMenu* pMenu = CCustomContextMenu::GetInstance();
			if (pMenu && pMenu->GetContextMenuMode() == CONTEXT_MENU_ANCHOR && pMenu->GetAnchorUrl().IsEmpty() == 0) {
				//bstrText		= pMenu->GetAnchorUrl();
				//bstrLocationUrl = pMenu->GetAnchorUrl();
			   #if _ATL_VER >= 0x700	//+++
				arrUrls.Add(pMenu->GetAnchorUrl());
			   #else
				arrUrls.Add( *const_cast<CString*>(&pMenu->GetAnchorUrl()) );
			   #endif
			} else {
				return;
			}
		  #else
			return;
		  #endif
		} else {
			hr = pDocument->get_URL(&bstrLocationUrl);
			if ( FAILED(hr) )
				return;

			//BASEタグに対処する minit
			CComPtr<IHTMLElementCollection> spAllClct;
			hr = pDocument->get_all(&spAllClct);
			if ( SUCCEEDED(hr) ) {
				CComQIPtr<IHTMLElementCollection> spBaseClct;
				CComVariant 		val = _T("BASE");
				CComPtr<IDispatch>	spDisp;
				hr = spAllClct->tags(val, &spDisp);
				spBaseClct	= spDisp;
				if ( SUCCEEDED(hr) && spBaseClct ) {
					long	length;
					hr = spBaseClct->get_length(&length);
					if (length > 0) {
						CComPtr<IHTMLElement> spElem;
						CComVariant 		  val1( (int) 0 ), val2( (int) 0 );
						hr = spBaseClct->item(val1, val2, (IDispatch **) &spElem);
						if ( SUCCEEDED(hr) ) {
							CComPtr<IHTMLBaseElement> spBase;
							hr = spElem->QueryInterface(&spBase);
							if ( SUCCEEDED(hr) ) {
								CComBSTR bstrBaseUrl;
								hr = spBase->get_href(&bstrBaseUrl);
								if ( SUCCEEDED(hr) && !(!bstrBaseUrl) )
									bstrLocationUrl = bstrBaseUrl;
							}
						}
					}
				}
			}

			MtlCreateHrefUrlArray( arrUrls, WTL::CString(bstrText), WTL::CString(bstrLocationUrl) );
		}
	});
	for (int i = 0; i < arrUrls.GetSize(); ++i) {
		DonutOpenFile(m_hWnd, arrUrls[i], 0);
	}
	m_MDITab.SetLinkState(LINKSTATE_OFF);
}


/// URLテキストを開く
void CChildFrame::OnEditOpenSelectedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	dcfTRACE( _T("CChildFrame::OnEditOpenSelectedTitle\n") );
	CString strText = GetSelectedText();
	if (strText.IsEmpty())
		return;

	std::vector<CString> lines;
	lines.reserve(20);
	Misc::SeptTextToLines(lines, strText);
	size_t	size = lines.size();
  #if 1	//+++ 選択文字列中にurlぽいものがなかったら、文字列検索にしてみる.
	bool	f	= 0;
	for (unsigned i = 0; i < size; ++i) {
		CString& strUrl = lines[i];
		f |= (strUrl.Find(_T(':')) >= 0) || (strUrl.Find(_T('/')) >= 0) || (strUrl.Find(_T('.')) >= 0) ||  (strUrl.Find(_T('\\')) >= 0);
		if (f)
			break;
	}
	if (f == 0) {	// urlぽい文字列がなかった...
		CDonutSearchBar*	pSearchBar = CDonutSearchBar::GetInstance();
		if (pSearchBar) {
			CEdit edit = pSearchBar->GetEditCtrl();
			edit.SendMessage(WM_CHAR, 'P');
			CString str		= lines[0];
			LPCTSTR		strExcept  = _T(" \t\"\r\n　");
			str.TrimLeft(strExcept);
			str.TrimRight(strExcept);
			edit.SetWindowText(str);
			pSearchBar->SearchWeb();
			return;
		}
	}
  #endif
	for (unsigned i = 0; i < size; ++i) {
		CString& strUrl = lines[i];
		Misc::StrToNormalUrl(strUrl);		//+++ 関数化
		DonutOpenFile(m_hWnd, strUrl);
	}
}






// ===========================================================================
// 検索

/// 選択中の文字列を検索(アドレスバーでCtrl+Enterしたときの検索エンジンが使われる)
void CChildFrame::searchEngines(const CString &strKeyWord )
{
	CString 	strSearchWord = strKeyWord;

	if (CAddressBarOption::s_bReplaceSpace)
		strSearchWord.Replace( _T("　"), _T(" ") );

	//_ReplaceCRLF(strSearchWord,CString(_T(" ")));
	//strSearchWord.Replace('\n',' ');
	//strSearchWord.Remove('\r');
	strSearchWord.Replace( _T("\r\n"), _T("") );

	CIniFileI	pr( g_szIniFileName, STR_ADDRESS_BAR );
	CString 		strEngin = pr.GetStringUW( _T("EnterCtrlEngin"), NULL, 256 );
	pr.Close();
	::SendMessage(GetTopLevelParent(), WM_SEARCH_WEB_SELTEXT, (WPARAM) (LPCTSTR) strSearchWord, (LPARAM) (LPCTSTR) strEngin);

	//CWebBrowserCommandHandler<CChildFrame>::searchEngines(strSearchWord);
}




//
// ページ内検索
//
LRESULT CChildFrame::OnFindKeyWord(LPCTSTR lpszKeyWord, BOOL bFindDown)
{
	// アクティブウィンドウの取得
	HWND	hWndActive = MDIGetActive();
	if (hWndActive == NULL)
		return 0;

	if (!m_spBrowser)
		return 0;

	// ドキュメントの取得
	CComPtr<IDispatch>	spDisp;
	HRESULT 			hr	= m_spBrowser->get_Document(&spDisp);
	if ( FAILED(hr) )
		return 0;

	// htmlの取得
	CComQIPtr<IHTMLDocument2>	spDocument = spDisp;
	if (!spDocument)
		return 0;

	// 検索
	BOOL	bSts = _FindKeyWordOne(spDocument, lpszKeyWord, bFindDown);
	if (bSts)
		return 1;

	// フレームウィンドウの取得
	CComPtr<IHTMLFramesCollection2> 	spFrames;
	hr = spDocument->get_frames(&spFrames);
	// cf. Even if no frame, get_frames would succeed.
	if ( FAILED(hr) )
		return 0;

	// フレーム内ウィンドウの数を取得
	LONG	nCount	   = 0;
	hr = spFrames->get_length(&nCount);
	if ( FAILED(hr) )
		return 0;

	BOOL	bFindIt    = FALSE;
	if (bFindDown) {	// ページ内検索 - 下
		for (LONG ii = m_nPainBookmark; ii < nCount; ii++) {
			CComVariant 			varItem(ii);
			CComVariant 			varResult;

			// フレーム内のウィンドウを取得
			hr		= spFrames->item(&varItem, &varResult);
			if ( FAILED(hr) )
				continue;

			CComQIPtr<IHTMLWindow2> spWindow = varResult.pdispVal;
			if (!spWindow)
				continue;

			CComPtr<IHTMLDocument2> spDocumentFr;
			hr		= spWindow->get_document(&spDocumentFr);
			if ( FAILED(hr) ) {
				CComQIPtr<IServiceProvider>  spServiceProvider = spWindow;
				ATLASSERT(spServiceProvider);
				CComPtr<IWebBrowser2>	spBrowser;
				hr = spServiceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&spBrowser);
				if (!spBrowser)
					continue;
				CComPtr<IDispatch>	spDisp;
				hr = spBrowser->get_Document(&spDisp);
				if (!spDisp)
					continue;
				spDocumentFr = spDisp;
				if (!spDocument)
					continue;
			}

			// 検索
			bFindIt = _FindKeyWordOne(spDocumentFr, lpszKeyWord, bFindDown);
			if (bFindIt) {
				m_nPainBookmark = ii;
				break;
			}
		}

		if (!bFindIt) {
			m_nPainBookmark = 0;
			m_strBookmark	= LPCOLESTR(NULL);
		}

	} else {			// ページ内検索 - 上
		if (m_nPainBookmark == 0 && !m_strBookmark)
			m_nPainBookmark = nCount - 1;

		for (LONG ii = m_nPainBookmark; ii >= 0; ii--) {
			CComVariant 			varItem(ii);
			CComVariant 			varResult;

			// ウィンドウの取得
			hr		= spFrames->item(&varItem, &varResult);
			if ( FAILED(hr) )
				continue;

			CComQIPtr<IHTMLWindow2> spWindow = varResult.pdispVal;
			if (!spWindow)
				continue;

			CComPtr<IHTMLDocument2> spDocumentFr;
			hr		= spWindow->get_document(&spDocumentFr);
			if ( FAILED(hr) )
				continue;

			// 検索
			bFindIt = _FindKeyWordOne(spDocumentFr, lpszKeyWord, bFindDown);
			if (bFindIt) {
				m_nPainBookmark = ii;
				break;
			}
		}

		if (!bFindIt) {
			m_nPainBookmark = 0;
			m_strBookmark	= LPCOLESTR(NULL);
		}
	}

	return 1;
}


//
//ページ内検索
//
BOOL CChildFrame::_FindKeyWordOne(IHTMLDocument2 *pDocument, const CString& rStrKeyWord, BOOL bFindDown)
{
	// ドキュメントがNULLなら終了
	if (!pDocument)
		return FALSE;

	// 待機中はカーソルを砂時計に変更する
	HCURSOR 	hCursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );
	HRESULT	hr = S_OK;
	// キーワードを一語取得
	//x strKeyWord = strtok( (LPSTR) strKeyWord.GetBuffer(0), " " );
	CString 	strKeyWord = Misc::GetStrWord( rStrKeyWord );
	LPCTSTR		strExcept  = _T(" \t\"\r\n　");
	strKeyWord.TrimLeft(strExcept);
	strKeyWord.TrimRight(strExcept);

	// <body>を取得
	CComPtr<IHTMLElement>		spHTMLElement;
	pDocument->get_body(&spHTMLElement);
	if (!spHTMLElement)
		return FALSE;

  #if (defined _WIN64 == 0) 	//+++ 暫定対処.
	if (_CheckOsVersion_XPLater() == 0 || strKeyWord.Find(_T('ﾞ')) >= 0 || strKeyWord.Find(_T('ﾟ')) >= 0) {
		// IE6(win2000?)使用時、findTextでフリーズする現象を回避するコード
		CComBSTR					bstr;
		spHTMLElement->get_innerHTML(&bstr);
		//+++ どうもこの、自分自身の入れ直しは、フレームなページだと、あまりよろしくない模様なのでframe以外のときのみに.
		if (!!bstr && CString(bstr, 7).CompareNoCase(_T("<FRAME ")) != 0)
			spHTMLElement->put_innerHTML(bstr);
	}
  #endif

	CComQIPtr<IHTMLBodyElement> spHTMLBody = spHTMLElement;
	if (!spHTMLBody)
		return FALSE;

	// テキストレンジを取得
	CComPtr<IHTMLTxtRange>		spTxtRange;
	spHTMLBody->createTextRange(&spTxtRange);

	if (!spTxtRange)
		return FALSE;

	if (m_strBookmark != NULL) {
		VARIANT_BOOL vMoveBookmark = VARIANT_FALSE;
		long		 nMove;
		spTxtRange->moveToBookmark(m_strBookmark, &vMoveBookmark);

		if (vMoveBookmark == VARIANT_FALSE) {
			;
		} else {
			if (bFindDown) {
				CComBSTR	bstrNow;
				spTxtRange->get_text(&bstrNow);

				CString  strNow(bstrNow);

				if (strNow != strKeyWord)
					spTxtRange->collapse(false);

				spTxtRange->moveStart( (BSTR) CComBSTR("Character"), 1, &nMove );
				spTxtRange->moveEnd  ( (BSTR) CComBSTR("Textedit" ), 1, &nMove );
			} else {
				spTxtRange->moveStart( (BSTR) CComBSTR("Textedit" ), -1, &nMove );
				spTxtRange->moveEnd  ( (BSTR) CComBSTR("Character"), -10, &nMove );
			}
		}
	}

	CComBSTR		bstrText(strKeyWord);
	BOOL			bSts  = FALSE;
	VARIANT_BOOL	vBool = VARIANT_FALSE;
	int	nSearchCount = 0;
	while (1) {
		hr	  = spTxtRange->findText(bstrText, (bFindDown) ? 1 : -1, 0, &vBool);
		if (vBool == VARIANT_FALSE)
			break;
	
		auto funcMove = [&spTxtRange, bFindDown] () {	// 検索範囲を変更する関数
			CComBSTR bstrUnitChar = L"character";
			long lActual = 0;
			if (bFindDown)
				spTxtRange->moveStart(bstrUnitChar, 1, &lActual);
			else
				spTxtRange->moveEnd  (bstrUnitChar, -10, &lActual );
		};

		// 親タグ名が"TEXTAREA"なら選択領域の最後へ移動(?)
		CComPtr<IHTMLElement> spParentElement;
		hr = spTxtRange->parentElement(&spParentElement);
		if (FAILED(hr))
			break;

		// 親
		CComQIPtr<IHTMLElement2>	spElement2 = spParentElement;
		ATLASSERT(spElement2);
		CComPtr<IHTMLCurrentStyle>	spStyle;
		spElement2->get_currentStyle(&spStyle);
		CComBSTR	strdisplay;
		if (spStyle) {
			hr = spStyle->get_display(&strdisplay);
			if (strdisplay && strdisplay == _T("none")) {
				funcMove();
				continue;
			}
		}
		// 親の親
		CComPtr<IHTMLElement>	spParentElement2;
		spParentElement->get_parentElement(&spParentElement2);
		if (spParentElement2) {
			spElement2 = spParentElement2;
			ATLASSERT(spElement2);
			CComPtr<IHTMLCurrentStyle>	spStyle;
			spElement2->get_currentStyle(&spStyle);
			if (spStyle) {
				hr = spStyle->get_display(&strdisplay);
				if (strdisplay && strdisplay == _T("none")) {
					funcMove();
					continue;
				}
			}
		}

		CComBSTR	bstrParentTag;
		hr = spParentElement->get_tagName(&bstrParentTag);
		if (FAILED(hr))
			break;
		if (   bstrParentTag != _T("SCRIPT")
			&& bstrParentTag != _T("TEXTAREA")) 
			break;	// 終わり

		++nSearchCount;
		if (nSearchCount > 5)	// 5以上で打ち止め
			break;

		funcMove();
	}

	auto funcScrollBy = [](IHTMLDocument2 *pDoc2) {
		CComPtr<IHTMLDocument3> 	   pDoc3;
		HRESULT 	hr = pDoc2->QueryInterface(&pDoc3);
		if ( FAILED(hr) )
			return;

		CComPtr<IHTMLElement>		   pElem;
		hr	= pDoc3->get_documentElement(&pElem);
		if ( FAILED(hr) )
			return;

		long		height = 0;
		hr	= pElem->get_offsetHeight(&height); 	// HTML表示領域の高さ
		if ( FAILED(hr) )
			return;

		CComPtr<IHTMLSelectionObject>  pSel;
		hr	= pDoc2->get_selection(&pSel);
		if ( FAILED(hr) )
			return;

		CComPtr<IDispatch>			   pDisp;
		hr	= pSel->createRange(&pDisp);

		if ( FAILED(hr) )
			return;

		CComPtr<IHTMLTextRangeMetrics> pTxtRM;
		hr	= pDisp->QueryInterface(&pTxtRM);
		if ( FAILED(hr) )
			return;

		long		y = 0;
		hr	= pTxtRM->get_offsetTop(&y);		// 選択部分の画面上からのy座標
		if ( FAILED(hr) )
			return;

		long scy = y - height / 2;				// 画面中央までの距離

		// 距離が表示部分の1/4より大きければスクロールさせる
		if ( (scy > height / 4) || (scy < -height / 4) ) {
			CComPtr<IHTMLWindow2> pWnd;
			hr = pDoc2->get_parentWindow(&pWnd);

			if ( FAILED(hr) )
				return;

			pWnd->scrollBy(0, scy);
		}
	};

	if (FAILED(hr) || vBool == VARIANT_FALSE) {
		CComPtr<IHTMLSelectionObject> spSelection;
		HRESULT 	hr = pDocument->get_selection(&spSelection);

		if (spSelection)
			spSelection->empty();
	} else {
		if (spTxtRange->getBookmark(&m_strBookmark) != S_OK)
			m_strBookmark = LPCOLESTR(NULL);

		spTxtRange->select();
		spTxtRange->scrollIntoView(VARIANT_TRUE);

		bSts = TRUE;

		DWORD		dwStatus = 0;
		CIniFileI pr( g_szIniFileName, _T("SEARCH") );
		pr.QueryValue( dwStatus, _T("Status") );
		pr.Close();

		if (dwStatus & STS_SCROLLCENTER)
			funcScrollBy(pDocument);
	}

	// カーソルを元に戻す
	SetCursor(hCursor);

	return bSts;
}
//^^^



void CChildFrame::ScrollBy(IHTMLDocument2 *pDoc2)
{
	CComPtr<IHTMLDocument3> 	   pDoc3;
	HRESULT 	hr = pDoc2->QueryInterface(&pDoc3);
	if ( FAILED(hr) )
		return;

	CComPtr<IHTMLElement>		   pElem;
	hr	= pDoc3->get_documentElement(&pElem);
	if ( FAILED(hr) )
		return;

	long		height = 0;
	hr	= pElem->get_offsetHeight(&height); 	// HTML表示領域の高さ
	if ( FAILED(hr) )
		return;

	CComPtr<IHTMLSelectionObject>  pSel;
	hr	= pDoc2->get_selection(&pSel);
	if ( FAILED(hr) )
		return;

	CComPtr<IDispatch>			   pDisp;
	hr	= pSel->createRange(&pDisp);

	if ( FAILED(hr) )
		return;

	CComPtr<IHTMLTextRangeMetrics> pTxtRM;
	hr	= pDisp->QueryInterface(&pTxtRM);
	if ( FAILED(hr) )
		return;

	long		y = 0;
	hr	= pTxtRM->get_offsetTop(&y);		// 選択部分の画面上からのy座標
	if ( FAILED(hr) )
		return;

	long scy = y - height / 2;				// 画面中央までの距離

	// 距離が表示部分の1/4より大きければスクロールさせる
	if ( (scy > height / 4) || (scy < -height / 4) ) {
		CComPtr<IHTMLWindow2> pWnd;
		hr = pDoc2->get_parentWindow(&pWnd);

		if ( FAILED(hr) )
			return;

		pWnd->scrollBy(0, scy);
	}
}



#if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
// gae _Function_Hilightと引数の意味が違うので注意
struct CChildFrame::_Function_Hilight2 {
	LPCTSTR 	m_lpszKeyWord;
	BOOL		m_bHilight;

	_Function_Hilight2(LPCTSTR lpszKeyWord, BOOL bHilight)
		: m_lpszKeyWord(lpszKeyWord), m_bHilight(bHilight)
	{
	}

	void operator ()(IHTMLDocument2* pDocument)
	{
		if (m_bHilight) {
			if ( !FindHilight(pDocument) ) {
				MakeHilight(pDocument);
			}
		} else {
			RemoveHilight(pDocument);
		}
	}

	// ハイライト作成
	void MakeHilight(IHTMLDocument2* pDocument)
	{
	try {
		// キーワードの最初の一語を取得
		CString		strKeyWord = m_lpszKeyWord;

		//+++ 単語区切りを調整
		LPCTSTR		strExcept	= _T(" \t\"\r\n　");
		strKeyWord = _tcstok( strKeyWord.GetBuffer(0), strExcept );
		strKeyWord.TrimLeft(strExcept);
		strKeyWord.TrimRight(strExcept);

		int 	nLightIndex = 0;
		HRESULT hr;

		// キーワードが空になるまで繰り返し
		while ( !strKeyWord.IsEmpty() ) {
			CComPtr<IHTMLElement>		spHTMLElement;
			// <body>を取得
			hr = pDocument->get_body(&spHTMLElement);
			if (spHTMLElement == NULL) 
				break;
#if 0
			static BOOL bXPLater = _CheckOsVersion_XPLater();
			//+++ メモ：IE6(win2000?)使用時に、findTextでフリーズすることがある現象を回避するためのコード.
			//    たぶん、 このへん http://d.hatena.ne.jp/fublog/20070221 なんかも参照.
			//    この方法は、リンク先のようにペナルティもあるようなんで、なるべく通さないほうがよい...
			//+++ "ｶﾞｸｶﾞｸﾌﾞﾙﾌﾞﾙ"のような?、濁音、半濁音文字等が複数混ざった半角文字列の場合でも、稀に、
			//	  findtextで無限trueを返される状態が発生するよう(ただｶﾞｸｶﾞｸｶﾞｸｶﾞｸでは大丈夫だったりよくわからない)
			if (bXPLater == FALSE || strKeyWord.Find(_T('ﾞ')) >= 0 || strKeyWord.Find(_T('ﾟ')) >= 0) {
				CComBSTR					bstr;
				spHTMLElement->get_innerHTML(&bstr);
				//+++ どうもこの、自分自身の入れ直しは、フレームなページだと、あまりよろしくない模様なのでframe以外のときのみに.
				if (!!bstr && CString(bstr, 7).CompareNoCase(_T("<FRAME ")) != 0)
					spHTMLElement->put_innerHTML(bstr);
			}
#endif
			CComQIPtr<IHTMLBodyElement>	spHTMLBody = spHTMLElement;
			if (spHTMLBody == NULL) 
				break;

			// テキストレンジを取得
			CComPtr<IHTMLTxtRange>	  spHTMLTxtRange;
			hr = spHTMLBody->createTextRange(&spHTMLTxtRange);
			if (!spHTMLTxtRange)
				AtlThrow(hr);

			// キーワードを検索
			CComBSTR		bstrText= strKeyWord;
			VARIANT_BOOL	vBool	= VARIANT_FALSE;
			hr	= spHTMLTxtRange->findText(bstrText, 1, 0, &vBool);

			//+++ 最大キーワード数(無限ループ対策)
			static unsigned maxKeyword	= Misc::getIEMejourVersion() <= 6 ? 1000 : 10000;
			//+++ 無限ループ状態を強制終了させるため、ループをカウントする
			unsigned num = 0;

			while (vBool == VARIANT_TRUE) {
				// 現在選択しているHTMLテキストを取得
				CComBSTR	bstrTextNow;
				hr = spHTMLTxtRange->get_text(&bstrTextNow);
				if (FAILED(hr))
					AtlThrow(hr);

				// <span>を付加
				CComBSTR	bstrTextNew;
				bstrTextNew.Append(g_lpszLight[nLightIndex]);	// <span ～
				bstrTextNew.Append(bstrTextNow);
				bstrTextNew.Append(_T("</span>"));


				// 親タグ名が"TEXTAREA"なら選択領域の最後へ移動(?)
				CComPtr<IHTMLElement> spParentElement;
				hr = spHTMLTxtRange->parentElement(&spParentElement);
				if (FAILED(hr))
					AtlThrow(hr);

				CComBSTR	bstrParentTag;
				hr = spParentElement->get_tagName(&bstrParentTag);
				if (FAILED(hr))
					AtlThrow(hr);

				if (   bstrParentTag != _T("SCRIPT")
					&& bstrParentTag != _T("TEXTAREA")) 
				{
					//CComBSTR	strInner;
					//spParentElement->get_outerText(&strInner);
					//strInner += _T("\n");
					//DEBUGPUT(strInner);
					//if (bstrParentTag == _T("TEXTAREA"))
					//	spHTMLTxtRange->collapse(VARIANT_FALSE);

					// ハイライトする
					hr = spHTMLTxtRange->pasteHTML(bstrTextNew);
					if (FAILED(hr))
						AtlThrow(hr);

					//+++ 通常のページでハイライト置換がこんなにもあることはないだろうで、無限ループ扱いでうちどめしとく
					if (++num > maxKeyword)		
						break;
				}

				CComBSTR bstrUnitChar = L"character";
				long lActual = 0;
				spHTMLTxtRange->moveStart(bstrUnitChar, 1, &lActual);

				vBool = VARIANT_FALSE;
				hr	  = spHTMLTxtRange->findText(bstrText, 1, 0, &vBool);
			}

			++nLightIndex;
			if (nLightIndex >= g_LIGHTMAX)
				nLightIndex = 0;

			// 次のキーワードに
			strKeyWord = _tcstok(NULL, strExcept);
			strKeyWord.TrimLeft(strExcept);
			strKeyWord.TrimRight(strExcept);
		}

	} catch (const CAtlException& e) {
			e;	// 例外を握りつぶす
	}	// try
	}

	// ハイライトを解除する
	void RemoveHilight(IHTMLDocument2* pDocument)
	{
		CComBSTR	hilightID(L"unDonutHilight");
		CComBSTR	hilightTag(L"SPAN");

		CComPtr<IHTMLElementCollection> pAll;

		if (SUCCEEDED( pDocument->get_all(&pAll) ) && pAll != NULL) {
			CComVariant 		id(L"unDonutHilight");
			CComPtr<IDispatch>	pDisp;
			CComVariant 		vIndex(0);
			pAll->item(id, vIndex, &pDisp);

			if (pDisp == NULL) {
				return;
			}

			CComPtr<IUnknown>	pUnk;

			if (SUCCEEDED( pAll->get__newEnum(&pUnk) ) && pUnk != NULL) {
				CComQIPtr<IEnumVARIANT> pEnumVariant = pUnk;

				if (pEnumVariant != NULL) {
					VARIANT  v;
					ULONG	 ul;
					CComBSTR bstrTagName;
					CComBSTR bstrID;
					CComBSTR bstrTmp;

					while (pEnumVariant->Next(1, &v, &ul) == S_OK) {
						CComQIPtr<IHTMLElement> pElement = v.pdispVal;
						VariantClear(&v);

						if (pElement != NULL) {
							bstrTagName.Empty();
							bstrID.Empty();
							pElement->get_tagName(&bstrTagName);
							pElement->get_id(&bstrID);

							if (bstrTagName == hilightTag && bstrID == hilightID) {
								bstrTmp.Empty();
								pElement->get_innerHTML(&bstrTmp);
								pElement->put_outerHTML(bstrTmp);
							}
						}
					}
				}
			}
		}
	}

	// ハイライトがすでにされているか確認する
	BOOL FindHilight(IHTMLDocument2* pDocument)
	{
		CComPtr<IHTMLElementCollection> 	pAll;

		if (SUCCEEDED(pDocument->get_all(&pAll)) && pAll != NULL) {
			CComVariant 		id(L"unDonutHilight");
			CComPtr<IDispatch>	pDisp;
			CComVariant 		vIndex(0);
			pAll->item(id, vIndex, &pDisp);
			if (pDisp != NULL) {
				return TRUE;
			}
		}
		return FALSE;
	}
};
#endif	// KIKE_UNDONUT_G





struct CChildFrame::_Function_SelectEmpt {
	_Function_SelectEmpt(){ }

	void operator ()(IHTMLDocument2 *pDocument)
	{
		CComPtr<IHTMLSelectionObject> spSelection;
		HRESULT 	hr = pDocument->get_selection(&spSelection);
		if (spSelection)
			spSelection->empty();
	}
};



//+++
void CChildFrame::SetSearchWordAutoHilight( const CString& str, bool autoHilightSw )
{
	m_strSearchWord = str;
	m_bAutoHilight	= autoHilightSw;
}



//vvv
// UH
LRESULT CChildFrame::OnHilight(CString strKeyWord)	//+++ 未使用な引数を削除. , BOOL bToggle /*= TRUE*/, BOOL bFlag /*= FALSE*/)
{
	HWND hWndActive = MDIGetActive();
	if (hWndActive == NULL) return 0;

  #if 0	//+++ 実質未使用の仕組みを削除
	if (!bToggle)
		m_bNowHilight = bFlag;
  #endif
  #if 1	//+++
	bool bHilightSw = CDonutSearchBar::GetInstance()->GetHilightSw();
	//if (m_bNowHilight != bHilightSw)
	{
		//MtlForEachHTMLDocument2( m_spBrowser, _Function_SelectEmpt() );
		//MtlForEachHTMLDocument2( m_spBrowser, _Function_Hilight(lpszKeyWord, m_bNowHilight) );
		m_bNowHilight	= bHilightSw;
		if (bHilightSw) {
			m_strSearchWord = strKeyWord;
			DeleteMinimumLengthWord(strKeyWord);
		} else {
			m_strSearchWord.Empty();
		}
		MtlForEachHTMLDocument2g(m_spBrowser, _Function_SelectEmpt());
		MtlForEachHTMLDocument2g(m_spBrowser, _Function_Hilight2(strKeyWord, bHilightSw));
	}
  #elif 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
   #if 1	//+++
	m_bNowHilight = CDonutSearchBar::GetInstance()->GetHilightSw();
   #else
	m_bNowHilight = !m_bNowHilight;
   #endif
	if (m_bNowHilight)
		m_strSearchWord = lpszKeyWord;
	else
		m_strSearchWord.Empty();
	MtlForEachHTMLDocument2g(m_spBrowser, _Function_SelectEmpt());
	MtlForEachHTMLDocument2g(m_spBrowser, _Function_Hilight2(lpszKeyWord, m_bNowHilight));
  #else
	MtlForEachHTMLDocument2( m_spBrowser, _Function_SelectEmpt() );
	MtlForEachHTMLDocument2( m_spBrowser, _Function_Hilight(lpszKeyWord, m_bNowHilight) );
	+++ m_bNowHilight = !m_bNowHilight;
  #endif
	return 1;
}


#if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
LRESULT CChildFrame::OnHilightOnce(IDispatch *pDisp, LPCTSTR lpszKeyWord)
{
	CComQIPtr<IWebBrowser2> 	pWebBrowser = pDisp;
	if (pWebBrowser) {
		MtlForEachHTMLDocument2g(pWebBrowser, _Function_SelectEmpt());
		MtlForEachHTMLDocument2g(pWebBrowser, _Function_Hilight2(lpszKeyWord, TRUE));
#if 0
		if (SUCCEEDED(pWebBrowser->get_Document(&pDisp))) {
			CComQIPtr<IHTMLDocument2> pDoc = pDisp;
			if(pDoc) {
				_Function_SelectEmpt	se;
				se(pDoc);

				_Function_Hilight2		hl(lpszKeyWord, TRUE);
				hl(pDoc);
			}
		}
#endif
	}
	return 1;
}
#endif








