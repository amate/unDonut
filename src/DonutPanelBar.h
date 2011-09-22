/**
 *	@file	DonutPanelBar.h
 *	@brief	パネル・バー
 */
#pragma once

#include "DonutView.h"

template <class _PanelBar>
class CPanelMenu : public CExplorerMenu {
private:
	_PanelBar * 	__m_pPanelBar;

public:
	CPanelMenu(_PanelBar *__pPanelBar, int nInsertPointMenuItemID)
		: CExplorerMenu( nInsertPointMenuItemID, _T("(なし)") )
		, __m_pPanelBar(__pPanelBar)
	{
		SetStyle(EMS_DRAW_ICON);
	}


	void OnExecute(const CString &strFilePath)
	{
		__m_pPanelBar->OpenFile(strFilePath);
	}


	BEGIN_MSG_MAP(CPanelMenu)
		CHAIN_MSG_MAP( CExplorerMenu )
	END_MSG_MAP()
};



template <class T, /* class TParent, */ class TBase = CWindow, class TWinTraits = CControlWinTraits>
class CDonutPanelBarImpl
	: public CWindowImpl< T, TBase, TWinTraits >
	, public IWebBrowserEvents2Impl<CDonutPanelBarImpl< T, TBase, TWinTraits >, ID_DONUTVIEW>
	, public CWebBrowserCommandHandler<CDonutPanelBarImpl< T, TBase, TWinTraits > >
	, public CChevronHandler<CDonutPanelBarImpl< T, TBase, TWinTraits > >
	, public CWebBrowser2
{
public:
	DECLARE_WND_CLASS_EX(NULL, 0, -1)

private:
	typedef CDonutPanelBarImpl<T, TBase, TWinTraits>	thisClass;
	//typedef CWindowImpl< T, TBase, TWinTraits >		baseClass;
	CToolBarCtrl					m_wndToolBar;
	CReBarCtrl						m_wndReBar;
	//CDonutView						m_view;
	CString 						m_strPanelFile;
	BOOL							m_bInitNavi;

	int 							m_cxyHeader;
	CPanelMenu<CDonutPanelBarImpl>	m_menuPanel;
	CMenu							m_menuPlacement;

	struct _ThreadParam {
		thisClass *pPanelBar;
		HANDLE	   _hExitEvent;

		_ThreadParam() : pPanelBar(0), _hExitEvent(0) {}	//+++
	};

	_ThreadParam					m_ThreadParams;
	HANDLE							m_hThread;

	CString 						m_strStatusBar; 		//+++

public:
	CDonutPanelBarImpl()
		: //m_view(/*CDLControlOption::DLCTL_DEFAULT, CDLControlOption::s_dwExtendedStyleFlags*/)
		 m_menuPanel(this, ID_INSERTPOINT_PANELTYPEMENU)
		, m_strPanelFile("about:blank")
		, m_bInitNavi(TRUE)
	  #if 1 //+++
		, m_cxyHeader( 0 )
		, m_hThread(0)
		, m_wndToolBar()
		, m_wndReBar()
		, m_menuPlacement()
	  #endif
	{
	}


	HWND CreatePanelBar(HWND hWndParent, BOOL bInitNavi = TRUE)
	{
		m_bInitNavi = bInitNavi;
		return Create( hWndParent, rcDefault, _T("パネルバー") );
	}


	// Message map and handlers
	BEGIN_MSG_MAP(CDonutPanelBarImpl)
		CHAIN_MSG_MAP_MEMBER( m_menuPanel )

		MESSAGE_HANDLER 	(WM_CREATE	, OnCreate		)
		MESSAGE_HANDLER 	(WM_DESTROY , OnDestroy 	)
		MESSAGE_HANDLER 	(WM_SIZE	, OnSize		)
		COMMAND_ID_HANDLER	(ID_PANEL_TYPE	, OnPanelType)
		NOTIFY_CODE_HANDLER (TBN_DROPDOWN	, OnTbnDropDown)

		CHAIN_MSG_MAP		(CChevronHandler<CDonutPanelBarImpl>)
		CHAIN_COMMANDS		(CWebBrowserCommandHandler<CDonutPanelBarImpl>)
		//::CHAIN_COMMANDS_MEMBER(m_view)		// CHAIN_CLIENT_COMMANDS() not send, why?
		CHAIN_MSG_MAP_MEMBER( m_menuPanel )
		//CHAIN_MSG_MAP(baseClass)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()


private:
	LRESULT OnPanelType(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		OpenFile(m_strPanelFile);
		return S_OK;
	}


	void SetTitle()
	{
		CString strTitle;

		strTitle.Format( _T("パネル:%s"), MtlGetDisplayTextFromPath(m_strPanelFile) );
		SetWindowText(strTitle);
	}


public:
	void OpenFile(const CString& strFile)
	{
		m_strPanelFile = strFile;
		SetTitle();
		CString strPath = strFile;
		MtlRemoveStringHeaderAndFooter(strPath);

		if (  !MtlIsProtocol( strPath, _T("http") )
		   && !MtlIsProtocol( strPath, _T("https") ) )
		{
			if ( MtlPreOpenFile(strPath) )
				return;
			// handled
		}

		MTL::ParseInternetShortcutFile(strPath);

		if (strPath.GetLength() > INTERNET_MAX_PATH_LENGTH)
			return;

		Navigate2(strPath);
	}


private:
	LRESULT OnTbnDropDown(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
	{
		LPNMTOOLBAR lpnmtb	   = (LPNMTOOLBAR) pnmh;

		int 		nID 	   = lpnmtb->iItem;
		int 		index	   = -1;
		int 		nCount	   = m_wndToolBar.GetButtonCount();

		for (int i = 0; i < nCount; i++) {
			TBBUTTON tb;
			m_wndToolBar.GetButton(i, &tb);

			if (nID == tb.idCommand) {
				index = i;
				break;
			}
		}

		if (index == -1)
			return 0;

		CRect		rc;
		m_wndToolBar.GetItemRect(index, &rc);
		ClientToScreen(&rc);

		UINT		uMenuFlags = TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_VERTICAL | TPM_LEFTALIGN | TPM_TOPALIGN |
								 (!AtlIsOldWindows() ? TPM_VERPOSANIMATION : 0);
		TPMPARAMS	TPMParams;
		TPMParams.cbSize	= sizeof (TPMPARAMS);
		TPMParams.rcExclude = rc;

		switch (lpnmtb->iItem) {
		case ID_PANEL_TYPE:
			::TrackPopupMenuEx(m_menuPlacement.GetSubMenu(0), uMenuFlags, rc.left + 1, rc.bottom + 2, m_hWnd, &TPMParams);
			break;

		case ID_PANEL_NAVI:
			break;

		case ID_FAVORITE_PLACEMENT:
			::SendMessage(GetParent(), WM_OPEN_EXPFAVMENU, rc.left + 1, rc.bottom + 2);
			//CMenu menu; menu.LoadMenu(IDR_MENU_FAVTREE_BAR);
			//::TrackPopupMenuEx(menu.GetSubMenu(0), uMenuFlags, rc.left + 1, rc.bottom + 2, m_hWnd, &TPMParams);
			break;
		}

		return TBDDRET_DEFAULT;
	}


	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &bHandled)
	{
		bHandled   = FALSE;

		//x 	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
#if 0	//:::
		m_view.Create(m_hWnd, rcDefault, _T("about:blank"), WS_CHILD | WS_VISIBLE, WS_EX_CLIENTEDGE, ID_DONUTVIEW);
		m_wndReBar = _CreateSimpleReBarCtrl(m_hWnd, ATL_SIMPLE_REBAR_NOBORDER_STYLE | CCS_NOPARENTALIGN);

		InitToolBar( RGB(255, 0, 255) );

		m_view.SetOperateDragDrop(FALSE, 0); //ビューのドラッグドロップ拡張を解除
		m_view.SetIeMenuNoCstm(TRUE);
		m_view.QueryControl(IID_IWebBrowser2, (void **) &m_spBrowser);
		WebBrowserEvents2Advise();

		m_menuPlacement.LoadMenu(IDR_PANEL_TYPE);

		CMenuHandle menuPanel = m_menuPlacement.GetSubMenu(0);
		m_menuPanel.SetRootDirectoryPath( Misc::GetExeDirectory() + _T("Panel") );
		m_menuPanel.SetTargetWindow(m_hWnd);
		m_menuPanel.InstallExplorerMenu(menuPanel);
		m_menuPanel.RefreshMenu();
		m_menuPanel.SetExcuteFunction(boost::bind(&CDonutPanelBarImpl::OpenFile, this, _1));

		if (m_bInitNavi) {
			// Thread Create
			m_ThreadParams._hExitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			m_ThreadParams.pPanelBar   = this;
			DWORD wThreadId = 0;
			//\\ m_hThread				   = ::CreateThread(NULL, 0, _PanelThread, (LPVOID) &m_ThreadParams, 0, &wThreadId);
			::_beginthread(_PanelThread, 0,(void *) &m_ThreadParams);
			//\\ ::SetThreadPriority(m_hThread , THREAD_PRIORITY_IDLE);
		}
#endif
		return 0;
	}


	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;

		if (m_bInitNavi) {
			// Thread Remove
			::SetEvent(m_ThreadParams._hExitEvent);
			DWORD dwResult = ::WaitForSingleObject(m_hThread, DONUT_THREADWAIT);
			::CloseHandle(m_ThreadParams._hExitEvent);
			::CloseHandle(m_hThread);
		}

		_WriteProfile();
		return 0;
	}

	static void __cdecl _PanelThread(void *lpParam){
		_ThreadParam *pParam = (_ThreadParam *) lpParam;

		pParam->pPanelBar->_GetProfile();
		pParam->pPanelBar->PostMessage(WM_COMMAND, ID_PANEL_TYPE);
	}
//\\ _beginthreadを使ったものに変更
#if 0
	static DWORD WINAPI _PanelThread(LPVOID lpParam)
	{
		_ThreadParam *pParam = (_ThreadParam *) lpParam;

		pParam->pPanelBar->_GetProfile();
		pParam->pPanelBar->PostMessage(WM_COMMAND, ID_PANEL_TYPE);
		return 0;
	}
#endif

	void _WriteProfile()
	{
		CIniFileO pr( g_szIniFileName, _T("Panel_Bar") );
		pr.SetStringUW( m_strPanelFile, _T("LastPanel") );
	}


	void _GetProfile()
	{
		CIniFileI	pr( g_szIniFileName, _T("Panel_Bar") );
	  #if 1	//+++
	  	CString	tmp = pr.GetStringUW( _T("LastPanel") );
	  	if (!tmp.IsEmpty())
			m_strPanelFile = tmp;
	  #else
		TCHAR		cBuff[MAX_PATH];
		DWORD		dwCount = MAX_PATH;
		if ( ERROR_SUCCESS == pr.QueryValue(cBuff, _T("LastPanel"), &dwCount) )
			m_strPanelFile = cBuff;
	  #endif
	}


	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL & /*bHandled*/)
	{
		CRect rc;

		GetClientRect(&rc);

		CRect rcToolBar;
		m_wndReBar.GetClientRect(&rcToolBar);
		m_wndReBar.MoveWindow(0, 0, rc.Width(), rcToolBar.Height(), TRUE);

		rc.top += rcToolBar.Height();
		//:::m_view.MoveWindow(&rc, TRUE);

		return 0;
	}


public:
	void OnBeforeNavigate2(IDispatch *			pDisp,
						   const CString &		strURL,
						   DWORD				nFlags,
						   const CString &		strTargetFrameName,
						   CSimpleArray<BYTE> & baPostedData,
						   const CString &		strHeaders,
						   bool &				bCancel)
	{
		ATLTRACE(_T("CDonutPanelBarImpl::BeforeNavigate2 url(%s) frame(%s)\n"), strURL.Left(100), strTargetFrameName);
	}


	void OnNewWindow2(IDispatch **ppDisp, bool &bCancel)
	{
		if (CMainOption::s_bAppClosing) {
			ATLTRACE( _T(" application closing, ignore this event\n") );
			bCancel = true;
			return;
		}
#if 0	//:::
		CChildFrame *pChild = CChildFrame::NewWindow(g_pMainWnd->m_hWndMDIClient,
													 g_pMainWnd->mdiTab(),
													 *CDonutAddressBar::GetInstance()/*g_pMainWnd->m_AddressBar*/,
													 true,
													 CDLControlOption::s_dwDLControlFlags); 	//minit 継承しないようにした_//_GetInheritedDLControlFlags());

		if (pChild == NULL) {
			bCancel = true;
			return;
		}

		pChild->m_spBrowser->QueryInterface(IID_IDispatch, (void **) ppDisp);
		ATLASSERT(ppDisp != NULL);
#endif
	}


private:
	DWORD _GetInheritedDLControlFlags()
	{
		DWORD dwDLFlags;
#if 0	//:::
		if ( _check_flag(MAIN_EX_INHERIT_OPTIONS, CMainOption::s_dwMainExtendedStyle) )
			dwDLFlags = m_view._GetDLControlFlags();
		else
			dwDLFlags = CDLControlOption::s_dwDLControlFlags;

		// default
#endif
		return dwDLFlags;
	}


public:
	//fixed by minit
	//BOOL->BYTE
	//return value : _MTL_TRANSLATE_PASS : _MTL_TRANSLATE_HANDLE : _MTL_TRANSLATE_WANT
	BYTE PreTranslateMessage(MSG *pMsg)
	{
		if ( pMsg->message == WM_MBUTTONDOWN && ::IsChild(m_hWnd, pMsg->hwnd) ) {
			CIniFileI	pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
			DWORD		dwLinkOpenBtnM = pr.GetValue(_T("LinkOpenBtnM"), 0);
			pr.Close();
			if (dwLinkOpenBtnM) {
				if ( ::IsWindow(m_hWnd) == 0 )
					return _MTL_TRANSLATE_PASS;
				bool bLink = m_strStatusBar.IsEmpty() == 0 && m_strStatusBar != "ページが表示されました";	//+++ カーソルがリンクをさしていたらstatusBar用のメッセージがあることを利用.
				::SendMessage(pMsg->hwnd, WM_LBUTTONDOWN, 0, pMsg->lParam); 				//+++ おまじない.
				int 	nTabCnt  = g_pMainWnd->mdiTab().GetItemCount(); 					//+++ リンクをクリックしたかどうかのチェック用.
				::SendMessage(pMsg->hwnd, WM_COMMAND, dwLinkOpenBtnM, 0);					//+++ リンクを新しい窓にひらく
				int 	nTabCnt2 = g_pMainWnd->mdiTab().GetItemCount();
				if (nTabCnt != nTabCnt2 || bLink)											//+++ リンクメッセージがあるか、タブが増えていたら、リンクをクリックしたとする.
					return _MTL_TRANSLATE_HANDLE;											//+++ trueを返して中ボタンクリックの処理をやったことにする.
				::SendMessage(pMsg->hwnd, WM_LBUTTONUP, 0, pMsg->lParam);					//+++ リンク以外をクリックした場合おまじないの左クリック押しを終了しておく.
																							//+++ (IEコンポーネントにウィールクリックの処理を任せる)
			}
		} else if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST) {
			if ( ::IsChild(m_hWnd, pMsg->hwnd) ) {
				//:::return m_view.PreTranslateMessage(pMsg);
				//m_view.SendMessage(pMsg->message,pMsg->wParam,pMsg->lParam);
				//m_view.SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
				return _MTL_TRANSLATE_WANT;
			}
		}

		return _MTL_TRANSLATE_PASS;
	}


	void OnStatusTextChange(const CString &strText)
	{
		m_strStatusBar = strText;
	}


private:
	void SetFocusControl()
	{
#if 0	//:::
		if ( !MtlIsApplicationActive(m_hWnd) )
			return;

		if (m_view.m_spBrowser == NULL)
			return;

		CComPtr<IDispatch>		  spDisp;
		HRESULT 				  hr	= m_view.m_spBrowser->get_Document(&spDisp);

		if ( FAILED(hr) )
			return;

		CComQIPtr<IHTMLDocument2> spDoc = spDisp;

		if (!spDoc) 				// document not initialized yet
			return;

		CComPtr<IHTMLWindow2>	  spWnd;
		hr = spDoc->get_parentWindow(&spWnd);

		if (!spWnd)
			return;

		hr = spWnd->focus();		// makes panelview active
#endif
	}


	CString GetSkinPanelBarPath(BOOL bHot)
	{
		CString strBmp;

		if (bHot)
			strBmp = _T("FavBarHot.bmp");
		else
			strBmp = _T("FavBar.bmp");

		return _GetSkinDir() + strBmp;
	}


	void InitToolBar(COLORREF clrMask, UINT nFlags = ILC_COLOR24)
	{
		CBitmap 				 bmp;
		bmp.Attach( AtlLoadBitmapImage(GetSkinPanelBarPath(FALSE).GetBuffer(0), LR_LOADFROMFILE) );

		CBitmap 				 bmpHot;
		bmpHot.Attach( AtlLoadBitmapImage(GetSkinPanelBarPath(TRUE).GetBuffer(0), LR_LOADFROMFILE) );

		CSize					 szBmp;
		if (bmp.m_hBitmap)
			bmp.GetSize(szBmp);
		else
			return; 	// ビットマップがなかったらデフォルトを読み込むようにする。

		CImageList				 imgs;
		imgs.Create(szBmp.cy, szBmp.cy, nFlags | ILC_MASK, szBmp.cx / szBmp.cy, 1);

		CImageList				 imgsHot;
		imgsHot.Create(szBmp.cy, szBmp.cy, nFlags | ILC_MASK, szBmp.cx / szBmp.cy, 1);

		imgs.Add(bmp, clrMask);
		imgsHot.Add(bmpHot, clrMask);

		m_wndToolBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST, 0);
		m_wndToolBar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
		m_wndToolBar.SetButtonStructSize( sizeof (TBBUTTON) );

		// add Add and Organize button
		TBBUTTON	btnType = { 3, ID_PANEL_TYPE, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_DROPDOWN, 0, 0 };
		m_wndToolBar.InsertButton(-1, &btnType);

		CVersional<TBBUTTONINFO> bi;
		bi.dwMask  = TBIF_TEXT;
		bi.pszText = _T("パネル");
		m_wndToolBar.SetButtonInfo(ID_PANEL_TYPE, &bi);

		m_wndToolBar.SetImageList(imgs);
		m_wndToolBar.SetHotImageList(imgsHot);

		_AddSimpleReBarBandCtrl(m_wndReBar, m_wndToolBar);
	}


public:
	void ReloadSkin()
	{
		if ( !::IsWindow(m_hWnd) || !m_wndToolBar.IsWindow() )
			return;

		CImageList imgs, imgsHot;
		imgs	= m_wndToolBar.GetImageList();
		imgsHot = m_wndToolBar.GetHotImageList();

		_ReplaceImageList(GetSkinPanelBarPath(FALSE), imgs);
		_ReplaceImageList(GetSkinPanelBarPath(TRUE), imgsHot);

		m_wndToolBar.InvalidateRect(NULL, TRUE);
	}


	IDispatch *GetPanelWebBrowserObject()
	{
		CWebBrowser2	browser  = m_spBrowser.p;
		IWebBrowser2 *	spBrowser;

		browser.m_spBrowser->QueryInterface(IID_IWebBrowser2, (void **) &spBrowser);
		return (IDispatch *) spBrowser;
	}


	IDispatch *GetPanelWindowObject()
	{
		CWebBrowser2			  browser = m_spBrowser.p;

		CComPtr<IDispatch>		  spDisp;
		HRESULT 				  hr	  = browser.m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2> spDoc   = spDisp;

		IHTMLWindow2 *			  spWnd;
		spDoc->get_parentWindow(&spWnd);
		return (IDispatch *) spWnd;
	}


	IDispatch *GetPanelDocumentObject()
	{
		CWebBrowser2 browser = m_spBrowser.p;

		IDispatch *  spDocument;

		browser.m_spBrowser->get_Document(&spDocument);
		return (IDispatch *) spDocument;
	}
};



//template <class T>
class CDonutPanelBar : public CDonutPanelBarImpl<CDonutPanelBar> //<T>, T >
{
public:
	DECLARE_WND_CLASS_EX(_T("Donut_PanelBar"), 0, -1)
};

