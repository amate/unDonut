/**
*	@brief	CMainFrame::Impl の実装
*/

///////////////////////////////////////////////////////////////////////////
// メッセージマップ
BEGIN_MSG_MAP_EX_impl( CMainFrame::Impl )

	CHAIN_MSG_MAP_MEMBER( m_MainOption			)
	CHAIN_MSG_MAP_MEMBER( m_ChildFrameUIState	)
	CHAIN_MSG_MAP_MEMBER( m_DownloadManager		)
	CHAIN_MSG_MAP_MEMBER( m_FaviconManager		)

	MSG_WM_CREATE	( OnCreate	)
	MSG_WM_CLOSE	( OnClose	)
	MSG_WM_DESTROY	( OnDestroy )
	MSG_WM_QUERYENDSESSION( OnQueryEndSession	)
	MSG_WM_ENDSESSION( OnEndSession	)
	MSG_WM_TIMER	( OnTimer	)
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
	USER_MSG_WM_UPDATESUPRESSPOPUPDATA	( OnUpdateSupressPopupData	)

	USER_MSG_WM_CLEANUPNEWPROCESSSHAREDMEMHANDLE( OnCleanUpNewProcessSharedMemHandle )
	USER_MSG_WM_SETDLCONFIGTOGLOBALCONFIG( OnSetDLConfigToGlobalConfig	)

	USER_MSG_WM_RELEASE_PROCESSMONITOR_PTR( OnReleaseProcessMonitorPtr	)

	USER_MSG_WM_ADDREMOVECHILDPROCESSID( OnAddRemoveChildProcessId	)

	// for DownloadManager
	USER_MSG_WM_GETUNIQUENUMBERFORDLITEM( OnGetUniqueNumberForDLItem	)

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
	COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_ADD 	, OnFavoriteGroupCommand	)
	COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_SAVE 	, OnFavoriteGroupCommand	)
	COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_ORGANIZE , OnFavoriteGroupCommand	)

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
	COMMAND_ID_HANDLER_EX( ID_AUTOLOGINEDIT			, OnAutoLoginEdit	)
	CHAIN_COMMANDS_TO_EXPLORERBAR( m_ExplorerBar )	// Explorer バー へ
	COMMAND_ID_HANDLER_EX( ID_SETFOCUS_ADDRESSBAR		, OnSetFocusToBar	)
	COMMAND_ID_HANDLER_EX( ID_SETFOCUS_SEARCHBAR		, OnSetFocusToBar	)
	COMMAND_ID_HANDLER_EX( ID_SETFOCUS_SEARCHBAR_ENGINE	, OnSetFocusToBar	)
	COMMAND_ID_HANDLER_EX( ID_VIEW_ADDBAR_DROPDOWN		, OnSetFocusToBar	)
	COMMAND_ID_HANDLER_EX( ID_VIEW_FULLSCREEN		, OnViewFullScreen		)

	// ツール
	COMMAND_ID_HANDLER_EX( ID_DOUBLE_CLOSE			, OnDoubleClose		)
	COMMAND_ID_HANDLER_EX( ID_POPUP_CLOSE			, OnPopupClose		)
	COMMAND_ID_HANDLER_EX( ID_TITLE_CLOSE			, OnTitleClose		)
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
	COMMAND_ID_HANDLER_EX( ID_SPECIAL_REFRESH_SEARCHENGIN, OnSpecialRefreshSearchEngine	)

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
	USER_MSG_WM_CHILDFRAMEEXSTYLECHANGE ( OnChildFrameExStyleChange )
	USER_MSG_WM_MOUSEGESTURE			( OnMouseGesture			)

	REFLECT_CHEVRONPUSHED_NOTIFICATION()	// CFrameWindowImpl より前に置く必要がある
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
	REFLECT_NOTIFICATIONS()
END_MSG_MAP()
	

////////////////////////////////////////////////////////////////////////////////////////
// UI マップ

BEGIN_UPDATE_COMMAND_UI_MAP_impl( CMainFrame::Impl )
	CHAIN_UPDATE_COMMAND_UI_MEMBER( m_MainOption		)
	CHAIN_UPDATE_COMMAND_UI_MEMBER( m_ExplorerBar		)
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

	UPDATE_COMMAND_UI_SETCHECK_IF	( ID_POPUP_CLOSE	, CSupressPopupOption::s_PopupBlockData.bValidIgnoreURL )
	UPDATE_COMMAND_UI_SETCHECK_IF	( ID_TITLE_CLOSE	, CSupressPopupOption::s_PopupBlockData.bValidCloseTitle )
	UPDATE_COMMAND_UI_SETCHECK_IF	( ID_DOUBLE_CLOSE	, CSupressPopupOption::s_PopupBlockData.bValidIgnoreURL && 
															CSupressPopupOption::s_PopupBlockData.bValidCloseTitle )

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



CMainFrame::Impl::Impl() : 
	CDDEMessageHandler<CMainFrame::Impl>(_T("Donut")),
	m_ExplorerBar(m_SplitterWindow),
	m_hWndRestoreFocus(NULL),
	m_bFullScreen(false),
	m_donutScriptHost(this)
{
	GdiplusInit();
}

CMainFrame::Impl::~Impl()
{
	GdiplusTerm();

	g_pMainWnd = nullptr;
}

void	CMainFrame::Impl::RestoreAllTab(LPCTSTR strFilePath, bool bCloseAllTab)
{
	/* default.dfgがあればそちらを使う */
	CString	strFile = CStartUpOption::GetDefaultDFGFilePath();
	if (::PathFileExists(strFile)) {
		if ( MtlIsExt( strFile, _T(".dfg") ) ) {
			if ( !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOCLOSEDFG) ) {
				//_LoadGroupOption(strFile, true);
			} else {
				//_LoadGroupOption(strFile, false);
			}
			CString strBakFile = Misc::GetFileNameNoExt(strFile) + _T(".bak.dfg");
			::MoveFileEx(strFile, strBakFile, MOVEFILE_REPLACE_EXISTING);
			PostMessage(WM_INITPROCESSFINISHED);
			return;
		}
	}

	CString	TabListPath;
	if (strFilePath == NULL) {
		TabListPath = GetConfigFilePath(_T("TabList.donutTabList"));
	} else {
		TabListPath = strFilePath;
	}

	CDonutTabList	tabList;
	if (tabList.Load(TabListPath) == false) {
		MessageBox(_T("RestoreAllTabでエラーが発生しました!"));
		PostMessage(WM_INITPROCESSFINISHED);
		return ;
	}
	int nCount = tabList.GetCount();
	for (int i = 0; i < nCount; ++i) {
		std::unique_ptr<NewChildFrameData>	pdata(new NewChildFrameData(m_ChildFrameClient));
		auto& pTab = tabList.At(i);
		pdata->strTitle	= pTab->strTitle;
		pdata->strURL	= pTab->strURL;
		pdata->dwDLCtrl	= pTab->dwDLCtrl;
		pdata->dwExStyle= pTab->dwExStyle;
		pdata->dwAutoRefresh	= pTab->dwAutoRefreshStyle;
		pdata->TravelLogBack	= pTab->TravelLogBack;
		pdata->TravelLogFore	= pTab->TravelLogFore;
		pdata->bActive	= (i == tabList.GetActiveIndex());
		pdata->bDelayLoad	= CMainOption::s_bDelayLoad;

		m_deqNewChildFrameData.push_back(std::move(pdata));
	}

	if (bCloseAllTab) {
		m_ChildFrameClient.SetChildFrameWindow(NULL);
		m_TabBar.ForEachWindow([](HWND hWnd) {
			::PostMessage(hWnd, WM_CLOSE, 0, 0);
		});
	}

	if (m_deqNewChildFrameData.size() > 0) {
		m_TabBar.InsertHere(true);
		m_TabBar.SetInsertIndex(m_TabBar.GetItemCount());
		CChildFrame::AsyncCreate(*m_deqNewChildFrameData[0]);
	} else {
		PostMessage(WM_INITPROCESSFINISHED);
	}
}

CDonutTabList	CMainFrame::Impl::_CollectAllChildFrameData()
{
	/* 現在開いているタブの情報を集める */
	HWND hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
	int	nCount = 0;
	int nActiveIndex = -1;
	vector<unique_ptr<ChildFrameDataOnClose> >	vecpSaveData;
	auto CollectChildFrameData = [&](HWND hWnd) {
		auto pChildFrameData = _GetChildFrameData(hWnd);
		vecpSaveData.push_back(unique_ptr<ChildFrameDataOnClose>(std::move(pChildFrameData)));
		if (hWnd == hWndActive)
			nActiveIndex = nCount;
		++nCount;
	};
	m_TabBar.ForEachWindow(CollectChildFrameData);

	CDonutTabList tabList;
	tabList.Swap(vecpSaveData);
	tabList.SetActiveIndex(nActiveIndex);
	return tabList;
}

unique_ptr<ChildFrameDataOnClose>	CMainFrame::Impl::_GetChildFrameData(HWND hWndChildFrame)
{
	::SendMessage(hWndChildFrame, WM_GETCHILDFRAMEDATA, true, 0);

	unique_ptr<ChildFrameDataOnClose> pChildFrameData(new ChildFrameDataOnClose);
	CString strSharedMemName;
	strSharedMemName.Format(_T("%s%#x"), NOWCHILDFRAMEDATAONCLOSESHAREDMEMNAME, hWndChildFrame);
	CSharedMemory sharedMem;
	sharedMem.Deserialize(*pChildFrameData, strSharedMemName);

	::SendMessage(hWndChildFrame, WM_GETCHILDFRAMEDATA, false, 0);
	return pChildFrameData;
}

void	CMainFrame::Impl::SaveAllTab()
{
	auto tabList = _CollectAllChildFrameData();

	/* TabList.donutTabList に保存する */
	if (tabList.Save(GetConfigFilePath(_T("TabList.donutTabList"))) == false) {
		MessageBox(_T("SaveAllTabでエラー発生!"));
	}
}

void	CMainFrame::Impl::UserOpenFile(LPCTSTR url, DWORD openFlags /*= DonutGetStdOpenFlag()*/, 
									   DWORD DLCtrl /*= -1*/, 
									   DWORD ExStyle /*= -1*/, 
									   DWORD AutoRefresh /*= 0*/)
{
	HWND	hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();

	CString strFileOrURL = url;
	if ( MtlIsExt(strFileOrURL, _T(".url")) && ::PathFileExists(strFileOrURL) ) {
		DWORD dwExProp = 0xAAAAAA;		//+++ 初期値変更
		DWORD dwExProp2= 0x8;			//+++ 拡張プロパティを増設.
		if (CExProperty::CheckExPropertyFlag(dwExProp, dwExProp2, strFileOrURL)) {
			CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, dwExProp, dwExProp2);
			DLCtrl	= ExProp.GetDLControlFlags();
			ExStyle	= ExProp.GetExtendedStyleFlags();
			AutoRefresh = ExProp.GetAutoRefreshFlag();
		}
		if (MTL::ParseInternetShortcutFile(strFileOrURL) == false) {
			TRACEIN(_T("UseOpenFile(), .urlファイルからURLが取得できませんでした : %s"), strFileOrURL);
			return ;
		}
	}

	if ( MtlIsExt(strFileOrURL, _T(".donutTabList")) && ::PathFileExists(strFileOrURL) ) {
		RestoreAllTab(strFileOrURL, (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOCLOSEDFG) == 0);
		return ;
	}

	bool bJavascript = false;
	if (strFileOrURL.Left(11).CompareNoCase(_T("javascript:")) == 0) {
		bJavascript = true;
		openFlags |= D_OPENFILE_NOCREATE;
	}

	/* openFlag が D_OPENFILE_NOCREATE ならアクティブなページを移動する */
	if ( hWndActive != NULL && _check_flag(D_OPENFILE_NOCREATE, openFlags) ) {
		if (bJavascript) {
			// Javascriptを実行する
			COPYDATASTRUCT cds;
			cds.dwData	= kExecuteUserJavascript;
			cds.lpData	= static_cast<LPVOID>(strFileOrURL.GetBuffer(0));
			cds.cbData	= (strFileOrURL.GetLength() + 1) * sizeof(WCHAR);
			::SendMessage(hWndActive, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);

		} else {
			_NavigateChildFrame(hWndActive, strFileOrURL, DLCtrl, ExStyle, AutoRefresh);
		}

		if ( !_check_flag(D_OPENFILE_NOSETFOCUS, openFlags) ) {
			// reset focus
			::SetFocus(NULL);
			MtlSendCommand(hWndActive, ID_VIEW_SETFOCUS);
		}
	} else {

		/* 新規ChildFrame作成 */
		NewChildFrameData	data(m_ChildFrameClient);
		data.strURL		= strFileOrURL;
		data.dwDLCtrl	= DLCtrl;
		data.dwExStyle	= ExStyle;
		data.dwAutoRefresh	= AutoRefresh;
		data.bActive	= _check_flag(openFlags, D_OPENFILE_ACTIVATE) 
			|| m_ChildFrameClient.GetActiveChildFrameWindow() == NULL;
		CChildFrame::AsyncCreate(data);
	}
}

/// 複数URLを開く
void	CMainFrame::Impl::UserOpenMultiFile(const std::vector<OpenMultiFileData>& vecOpenData, bool bLink /*= false*/, bool bUseDelayLoad /*= true*/)
{
	for (auto it = vecOpenData.cbegin(); it != vecOpenData.cend(); ++it) {
		NewChildFrameData*	pdata = new NewChildFrameData(m_ChildFrameClient);
		pdata->strTitle	= it->strTitle;
		pdata->strURL	= it->strURL;
		pdata->dwDLCtrl	= it->DLCtrl;
		pdata->dwExStyle= it->ExStyle;
		pdata->dwAutoRefresh = it->AutoRefresh;
		pdata->bLink	= bLink;
		if (bUseDelayLoad && CMainOption::s_bDelayLoad)
			pdata->bDelayLoad = true;

		m_deqNewChildFrameData.push_back(std::unique_ptr<NewChildFrameData>(std::move(pdata)));
	}
	if (m_deqNewChildFrameData.size() > 0) {
		if (bLink)	// kNowDonutInstanceからなら逆転
			std::reverse(m_deqNewChildFrameData.begin(), m_deqNewChildFrameData.end());
		if (m_ChildFrameClient.GetActiveChildFrameWindow() == NULL)
			m_deqNewChildFrameData.back()->bActive = true;
		//m_TabBar.InsertHere(true);
		//m_TabBar.SetInsertIndex(m_TabBar.GetItemCount());
		CChildFrame::AsyncCreate(*m_deqNewChildFrameData[0]);
	} 
}


CString CMainFrame::Impl::GetActiveLocationURL() 
{
	HWND activeChildFrame = m_ChildFrameClient.GetActiveChildFrameWindow();
	if (activeChildFrame == NULL)
		return CString();

	auto pChildFrameUIData = CChildFrameCommandUIUpdater::GetChildFrameUIData(activeChildFrame);
	ATLASSERT( pChildFrameUIData );
	return pChildFrameUIData->strLocationURL;
}



// Overrides

BOOL CMainFrame::Impl::AddSimpleReBarBandCtrl(HWND hWndReBar, HWND hWndBand, int nID, LPTSTR lpstrTitle, UINT fStyle, int cxWidth)
{
	ATLASSERT( ::IsWindow(hWndReBar) ); 	// must be already created
	ATLASSERT( ::IsWindow(hWndBand) );		// must be already created
	MTLASSERT_KINDOF(REBARCLASSNAME, hWndReBar);

	// Set band info structure
	REBARBANDINFO rbBand = { sizeof (REBARBANDINFO) };
	rbBand.fMask	 = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE | RBBIM_IDEALSIZE;

	rbBand.fMask	|= RBBIM_BACKGROUND | RBBIM_TEXT;
	rbBand.fStyle	 = fStyle;
	rbBand.lpText	 = lpstrTitle;
	rbBand.hwndChild = hWndBand;
	rbBand.wID		 = nID;

	// Calculate the size of the band
	BOOL	bRet;
	RECT	rcTmp;
	int 	nBtnCount = (int) ::SendMessage(hWndBand, TB_BUTTONCOUNT, 0, 0L);
	if (nBtnCount > 0) {
		// ツールバーの場合
		rbBand.fStyle	|= RBBS_USECHEVRON;
		bRet				= ::SendMessage(hWndBand, TB_GETITEMRECT, nBtnCount - 1, (LPARAM) &rcTmp) != 0;
		ATLASSERT(bRet);
		rbBand.cx			= (cxWidth != 0) ? cxWidth : rcTmp.right;
		rbBand.cyMinChild	= rcTmp.bottom - rcTmp.top;

		if (lpstrTitle == 0) {
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
		rbBand.cxMinChild	= 0;
		rbBand.cyMinChild	= rcTmp.bottom - rcTmp.top;
	}

	// NOTE: cxIdeal used for CHEVRON, if MDI cxIdeal changed dynamically.
	rbBand.cxIdeal = rcTmp.right;			//rbBand.cx is not good.

	// Add the band
	LRESULT 	  lRes = ::SendMessage(hWndReBar, RB_INSERTBAND, (WPARAM) -1, (LPARAM) &rbBand);

	if (lRes == 0) {
		ATLTRACE2( atlTraceUI, 0, _T("Failed to add a band to the rebar.\n") );
		return FALSE;
	}

	//if (nID == IDC_LINKBAR)
	//	::SendMessage(hWndBand, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_HIDECLIPPEDBUTTONS);

	return TRUE;
}

void CMainFrame::Impl::UpdateLayout(BOOL bResizeBars /*= TRUE*/)
{
	CRect	  rc;
	GetClientRect(&rc);

	if (bResizeBars) {
		//CReBarCtrl rebar(m_hWndToolBar);
		//if (rebar.m_hWnd == NULL) 
		//	goto END;

		CRect	rcSrc;
		m_ReBar.GetClientRect(&rcSrc);

		CRect	rcNew(0, 0, rc.right, rcSrc.Height());
		m_ReBar.MoveWindow(rcNew);
		m_ReBar.RedrawWindow();
	}
//END:
	UpdateBarsPosition(rc, bResizeBars);

	if (m_bFullScreen == false)
		rc.top++;

	// ページ内検索バー
	HWND hWndFind = m_FindBar.GetHWND();
	if (::IsWindowVisible(hWndFind)) {			
		CRect rcFind;
		::GetClientRect(hWndFind, &rcFind);
		if (m_FindBar.FindBarBottom()) {
			rc.bottom -= rcFind.bottom;
			::SetWindowPos( hWndFind, NULL, rc.left, rc.bottom, rc.right, rcFind.bottom, SWP_NOZORDER | SWP_NOACTIVATE );

		} else {
			::SetWindowPos( hWndFind, NULL, rc.left, rc.top, rc.right, rcFind.bottom, SWP_NOZORDER | SWP_NOACTIVATE );
			rc.top += rcFind.bottom;
		}
	}

	if (m_hWndClient)
		::SetWindowPos( m_hWndClient, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER | SWP_NOACTIVATE );
}

BOOL CMainFrame::Impl::PreTranslateMessage(MSG* pMsg)
{
	// 検索バー
	BOOL ptFlag = m_SearchBar.PreTranslateMessage(pMsg);
	if (ptFlag == _MTL_TRANSLATE_HANDLE) {
		return TRUE;
	} else if (ptFlag == _MTL_TRANSLATE_WANT) {
		return FALSE;
	}

	// マウスジェスチャー
	if (pMsg->message == WM_RBUTTONDOWN) {
		if (CDLControlOption::s_dwExtendedStyleFlags & DVS_EX_MOUSE_GESTURE) {
			MouseGestureData	data;
			data.hwnd = pMsg->hwnd;
			data.wParam	= pMsg->wParam;
			data.lParam = pMsg->lParam;
			data.bCursorOnSelectedText = false;
			if (_RButtonHook(data))
				return TRUE;
		}
	}

	return CFrameWindowImpl<CMainFrame::Impl>::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::Impl::OnIdle()
{
	CmdUIUpdateToolBars();
	CmdUIUpdateStatusBar	(m_hWndStatusBar, ID_DEFAULT_PANE	);
	CmdUIUpdateStatusBar	(m_hWndStatusBar, ID_SECURE_PANE	);
	CmdUIUpdateStatusBar	(m_hWndStatusBar, ID_PRIVACY_PANE	);
	CmdUIUpdateChildWindow	(m_hWndStatusBar, IDC_PROGRESS		);

	return FALSE;
}

bool CMainFrame::Impl::OnDDEOpenFile(const CString& strFileName)
{
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
		_DeleteTrayIcon();							//+++ トレイ状態だったら復活.
		if (IsZoomed() == FALSE)
			ShowWindow(SW_RESTORE);
			//ShowWindow_Restore(true);
		MtlSetForegroundWindow(m_hWnd);
	}
	return true;
}


int		CMainFrame::Impl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);

	// message loop
	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	/* GlobalConfig作成 */
	CreateGlobalConfig(&m_GlobalConfigManageData);
	SetGlobalConfig(m_GlobalConfigManageData.pGlobalConfig);
	OnSetDLConfigToGlobalConfig();
	m_StatusBar.GetProxyComboBox().SetGlobalConfig(m_GlobalConfigManageData.pGlobalConfig);

	//TIMERSTART();
	/* 各種バンドウィンドウ作成 */
	_initRebar();
	HWND hWndCmdBar		= _initCommandBar();
	HWND hWndToolBar	= _initToolBar();
	TIMERSTART();
	HWND hWndAddressBar = _initAddressBar();
	TIMERSTOP(L"_initAddressBar");
	TIMERSTART();
	HWND hWndSearchBar	= m_SearchBar.Create(m_ReBar);
	TIMERSTOP(L"m_SearchBar");
	HWND hWndTabBar		= _initTabBar();
	HWND hWndLinkBar	= _initLinkBar();
	TIMERSTART();
	_initBandPosition(hWndCmdBar, hWndToolBar, hWndAddressBar, hWndSearchBar, hWndLinkBar, hWndTabBar);
	TIMERSTOP(L"_initBandPosition");
	//TIMERSTOP(_T("各種バンド作成"));
	
	m_GlobalConfigManageData.pGlobalConfig->SearchEditHWND	= m_SearchBar.GetEditCtrl();

	TIMERSTART();

	_initStatusBar();
	_initSplitterWindow();
	_initChildFrameClient();
	_initExplorerBar();

	TIMERSTOP(_T("misc"));

	m_FindBar.Create(m_hWnd);
	m_FindBar.SetUpdateLayoutFunc(boost::bind(&CMainFrame::Impl::UpdateLayout, this, _1));
	
	TIMERSTART();
	_initSkin();
	TIMERSTOP(_T("initSkin"));
	TIMERSTART();
	_initSysMenu();

	/* UI 更新を設定 */
	m_ChildFrameUIState.SetMainFrameHWND(m_hWnd);
	CmdUIAddToolBar(hWndToolBar);					// set up UI
	CmdUIAddToolBar(m_SearchBar.GetHWndToolBar());	// set up UI

	CFaviconManager::Init(hWndTabBar);

	m_DownloadManager.SetParent(m_hWnd);

	// 最近閉じたタブのメニューの設定やオプションの設定を行う
	m_RecentClosedTabList.SetMaxEntries(CMainOption::s_nMaxRecentClosedTabCount);
	m_RecentClosedTabList.SetMenuType(CMainOption::s_RecentClosedTabMenuType);
	m_RecentClosedTabList.ReadFromXmlFile();

	CDonutFavoriteGroupTreeView::SetFunctions(
		[this]() { return _CollectAllChildFrameData(); }, 
		[this]() -> unique_ptr<ChildFrameDataOnClose> {
			if (m_ChildFrameClient.GetActiveChildFrameWindow() == NULL)
				return nullptr;
			return _GetChildFrameData(m_ChildFrameClient.GetActiveChildFrameWindow());
		});

	CUrlSecurityOption::UpdateOriginalUrlSecurityList(m_hWnd);
	CCustomContextMenuOption::UpdateCustomContextMenuList(m_hWnd);
	CLoginDataManager::CreateOriginalLoginDataList(m_hWnd);
	CSupressPopupOption::CreateSupressPopupData(m_hWnd);

	CDLControlOption::SetUserAgent();

	RegisterDragDrop();

	TIMERSTOP(L"メインフレームのOnCreateにかかった時間");
	return 0;
}

/// ファイルをダウンロード中だが終了していいのか返す
bool	CMainFrame::Impl::_ConfirmCloseForFileDownloading()
{
	int nDLCount = m_DownloadManager.GetDownloadingCount();
	if (nDLCount > 0) {
		CString msg;
		msg.Format(_T("ダウンロード中のアイテムが %d個あります。\n終了しますか？"), nDLCount);
		if (MessageBox(msg, _T("確認"), MB_ICONQUESTION | MB_OKCANCEL | MB_DEFBUTTON2) == IDCANCEL)
			return false;
	}
	return true;
}

/// メインフレームのＸボタンを押したとき
void	CMainFrame::Impl::OnClose()
{
	if (_ConfirmCloseForFileDownloading() == false)
		return ;

	if ( CDonutConfirmOption::OnDonutExit(m_hWnd, m_setChildProcessId) == false ) {
		if (IsWindowVisible() == FALSE) {
			_SetHideTrayIcon();
		}
		return;
	}

	SetMsgHandled(FALSE);
}


/// 終了時に設定されていればキャッシュとクッキーを削除する
static void _DeleteCacheAndCookie()
{
	bool	bDelCache	 = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_CASH  ) != 0;
	bool	bDelCookie	 = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_COOKIE) != 0;
	if (bDelCache == false && bDelCookie == false)
		return;

	bool	bDone		 = FALSE;
	LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;

	DWORD	dwTrySize;
	DWORD	dwEntrySize  = 4096;				// start buffer size
	HANDLE	hCacheDir	 = NULL;
	DWORD	dwError 	 = ERROR_INSUFFICIENT_BUFFER;

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
			if ( bDelCache && !( lpCacheEntry->CacheEntryType & (COOKIE_CACHE_ENTRY | URLHISTORY_CACHE_ENTRY) ) )
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

/// 履歴を削除する
static void _DeleteHistory()
{
	bool	bDelHistory  = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_HISTORY) != 0;
	if (bDelHistory == false)
		return;

	CComPtr<IUrlHistoryStg2>	spUrlHistoryStg2;
	HRESULT hr = spUrlHistoryStg2.CoCreateInstance(CLSID_CUrlHistory);
	if ( SUCCEEDED(hr) )
		hr = spUrlHistoryStg2->ClearHistory();
}


void	CMainFrame::Impl::OnDestroy()
{
	TRACEIN(_T("メインフレームの終了中..."));
	SetMsgHandled(FALSE);

	_PrivateTerm();		// 設定の保存

	KillTimer(kAutoBackupTimerId);

	RevokeDragDrop();

	_SaveBandPosition();

	CIniFileO	pr(g_szIniFileName, _T("Main"));
	MtlWriteProfileMainFrameState(pr, m_hWnd);

	m_ReBar.UnsubclassWindow();

	m_GlobalConfigManageData.pGlobalConfig->bMainFrameClosing = true;

	if (CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_LATEST) {
		SaveAllTab();	// 現在表示中のタブを保存する
	} else {
#if 0
		vector<HWND> vechWnd;
		m_TabBar.ForEachWindow([&](HWND hWndChildFrame) {	// 最近閉じたタブに追加
			vechWnd.push_back(hWndChildFrame);			
		});
		for (auto it = vechWnd.cbegin(); it != vechWnd.cend(); ++it)
			::PostMessage(*it, WM_CLOSE, 0, 0);

		// 履歴がすべて保存されるまで待つ
		enum { kMaxWaitTime = 30 * 1000 };
		DWORD startTime = ::timeGetTime();
		while ((::timeGetTime() - startTime) < kMaxWaitTime) {
			MSG msg = {};
			if (PeekMessage(&msg , NULL , 0 , 0 , PM_REMOVE)) {
				DispatchMessage(&msg);
			} else {
				if (m_TabBar.GetItemCount() == 0)
					break;
				::Sleep(10);
			}
		}
#endif
		// 現在表示中のタブを最近閉じたタブに追加する
		auto tabList = _CollectAllChildFrameData();
		for (auto& tab : tabList)
			m_RecentClosedTabList.AddToList(std::move(tab));
	}

	if (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_RECENTCLOSE) {
		m_RecentClosedTabList.DeleteRecentClosedTabFile();
	} else {
		m_RecentClosedTabList.WriteToXmlFile();
	}

	CAcceleratorOption::DestroyOriginAccelerator(m_hWnd, m_hAccel);

	DestroyGlobalConfig(&m_GlobalConfigManageData);

	// メッセージループからメッセージフィルタとアイドルハンドラを削除
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->RemoveMessageFilter(this);
    pLoop->RemoveIdleHandler(this);

	_DeleteCacheAndCookie();
	_DeleteHistory();

	CStartUpOption::EndFinish();
}

/// Windowsが終了前に呼ばれる　ファイルがダウンロード中なら中止するかどうか確認する
BOOL	CMainFrame::Impl::OnQueryEndSession(UINT nSource, UINT uLogOff)
{
	if (_ConfirmCloseForFileDownloading() == false)
		return FALSE;

	return TRUE;
}

void	CMainFrame::Impl::OnEndSession(BOOL bEnding, UINT uLogOff)
{
	if (bEnding) {
		OnDestroy();	// 設定の保存
	}
}

void	CMainFrame::Impl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == kAutoBackupTimerId) {
		TIMERSTART();
		SaveAllTab();
		TIMERSTOP(L"定期TabList.donutTabListセーブ");
	}
}

void	CMainFrame::Impl::_initRebar()
{
	DWORD	dwRebarStyle = ATL_SIMPLE_REBAR_STYLE | CCS_NOPARENTALIGN | CCS_NODIVIDER | RBS_DBLCLKTOGGLE;
	{	// TODO: 後で設定クラスに移す
		CIniFileI	pr( g_szIniFileName, _T("ReBar") );
		DWORD		dwNoBoader 		= pr.GetValue( _T("NoBoader") );
		if (dwNoBoader)
			dwRebarStyle &= ~RBS_BANDBORDERS;	// ボーダーを消す
	}

	CreateSimpleReBar(dwRebarStyle);
	m_ReBar.SubclassWindow(m_hWndToolBar);
}

HWND	CMainFrame::Impl::_initCommandBar()
{
	SetMenu(NULL);		// remove menu

	HWND	hWndCmdBar = m_CommandBar.Create(m_ReBar);
	ATLASSERT( ::IsWindow(hWndCmdBar) );

	m_CommandBar.SetRecentClosedTabList(&m_RecentClosedTabList);	// 最近閉じたタブ ポップアップメニュー用

	return hWndCmdBar;
}

HWND	CMainFrame::Impl::_initToolBar()
{
	HWND hWndToolBar = m_ToolBar.Create(m_hWnd);
	ATLASSERT( ::IsWindow(hWndToolBar) );

	return hWndToolBar;
}

HWND	CMainFrame::Impl::_initAddressBar()
{
	HWND hWndAddressBar = m_AddressBar.Create(m_ReBar, IDC_ADDRESSBAR, ID_VIEW_GO,
												 16, 16, RGB(255, 0, 255) );
	ATLASSERT( ::IsWindow(hWndAddressBar) );
	m_AddressBar.SetSearchWebWidthEngineFunc(
		std::bind(&CDonutSearchBar::SearchWebWithEngine, &m_SearchBar, std::placeholders::_1, std::placeholders::_2));

	return hWndAddressBar;
}

HWND	CMainFrame::Impl::_initTabBar()
{
	HWND	hWndMDITab = m_TabBar.Create(m_ReBar);
	ATLASSERT( ::IsWindow(hWndMDITab) );

	return hWndMDITab;
}

HWND	CMainFrame::Impl::_initLinkBar()
{
	HWND	hWndLinkBar = m_LinkBar.Create(m_ReBar);
	ATLASSERT( ::IsWindow(hWndLinkBar) );

	return 	hWndLinkBar;
}

void	CMainFrame::Impl::_initBandPosition(HWND hWndCmdBar, 
											HWND hWndToolBar, 
											HWND hWndAddressBar, 
											HWND hWndSearchBar, 
											HWND hWndLinkBar, 
											HWND hWndTabBar	)
{
	std::vector<HWND>	vecBand;
	vecBand.push_back(hWndCmdBar);
	vecBand.push_back(hWndToolBar);
	vecBand.push_back(hWndAddressBar);
	vecBand.push_back(hWndSearchBar);
	vecBand.push_back(hWndLinkBar);
	vecBand.push_back(hWndTabBar);

	struct ReBarBandInfo {
		UINT		nIndex; 	// must be filled, cause stable_sort requires CRT
		HWND		hWnd;
		UINT		nID;
		UINT		fStyle;
		LPTSTR		lpText;
		UINT		cx; 		// can be 0
	};

	auto funcGetDefaultStyle = [](UINT nID) -> UINT {
		auto it = std::find_if(std::begin(DefaultReBarStyle), std::end(DefaultReBarStyle), 
			[nID](const ReBarIDAndStyle& idAndstyle) { return idAndstyle.nID == nID; }
		);
		if (it != std::end(DefaultReBarStyle)) {
			return it->fStyle;
		} else {
			ATLASSERT( FALSE );
			return 0;
		}
	};
	std::vector<ReBarBandInfo>	vecReBarBandInfo;
	CIniFileI	pr(g_szIniFileName, _T("ReBar"));
	for (auto it = vecBand.cbegin(); it != vecBand.cend(); ++it) {
		ReBarBandInfo	rbi = { 0 };
		rbi.hWnd	= *it;
		rbi.nID		= ::GetDlgCtrlID((*it));
		CString prefix;
		prefix.Format(_T("band#%d"), rbi.nID);
		rbi.nIndex	= pr.GetValuei(prefix + _T(".index"));
		rbi.cx		= pr.GetValuei(prefix + _T(".cx"));
		rbi.fStyle	= pr.GetValuei(prefix + _T(".fStyle"), funcGetDefaultStyle(rbi.nID));
		vecReBarBandInfo.push_back(rbi);
	}
	boost::sort(vecReBarBandInfo, [](const ReBarBandInfo& rbi1, const ReBarBandInfo& rbi2) {
		return rbi1.nIndex < rbi2.nIndex;
	});
	boost::for_each(vecReBarBandInfo, [this](const ReBarBandInfo& rbi) {
		AddSimpleReBarBandCtrl(m_ReBar, rbi.hWnd, rbi.nID, nullptr, rbi.fStyle, rbi.cx);
	});
	m_ReBar.LockBands( (vecReBarBandInfo.front().fStyle & RBBS_NOGRIPPER) != 0 );
	m_AddressBar.InitReBarBandInfo(m_ReBar);

	OnShowBandTextChange(CAddressBarOption::s_bTextVisible);
}

void	CMainFrame::Impl::_initStatusBar()
{
	m_hWndStatusBar	= m_StatusBar.Create(m_hWnd, ATL_IDS_IDLEMESSAGE,
							 WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | SBARS_SIZEGRIP | SBT_TOOLTIPS);
	ATLASSERT( ::IsWindow(m_hWndStatusBar) );

	//		int nPanes[] = { ID_DEFAULT_PANE, ID_PROGRESS_PANE, ID_COMBOBOX_PANE};
	static int   nPanes[] = { ID_DEFAULT_PANE, ID_PROGRESS_PANE, ID_PRIVACY_PANE, ID_SECURE_PANE, ID_COMBOBOX_PANE };
	m_StatusBar.SetPanes( nPanes, _countof(nPanes), false );
	m_StatusBar.SetCommand( ID_DEFAULT_PANE , ID_STATUSBAR_DEFAULTPANE);
	m_StatusBar.SetCommand( ID_PRIVACY_PANE, ID_PRIVACYREPORT );
	m_StatusBar.SetCommand( ID_SECURE_PANE, ID_SECURITYREPORT);
	m_StatusBar.SetIcon( ID_PRIVACY_PANE, 1 );				//minit
	m_StatusBar.SetIcon( ID_SECURE_PANE , 0 );				//minit
	m_StatusBar.SetOwnerDraw( m_StatusBar.IsValidBmp() );

	// TODO: ここらへんもなんとかする
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
		m_StatusBar.SetPaneWidth(ID_PAIN_1, 0);			//dwSzPain1); //起動時はペインサイズが0
		if (m_StatusBar.GetProxyComboBox().IsUseIE())
			dwSzPain2 = 0;									// IEのProxyを使う場合はProxyペインサイズを0に
		m_StatusBar.SetPaneWidth(ID_PAIN_2, dwSzPain2);
	} else {	// 交換しているとき.
		g_dwProgressPainWidth = dwSzPain2;					//+++ 手抜きでプログレスペインの横幅をグローバル変数に控える.
		m_StatusBar.SetPaneWidth(ID_PAIN_2, dwSzPain2);	//+++ 交換してるときは、最初からサイズ確保.
		if (m_StatusBar.GetProxyComboBox().IsUseIE())
			dwSzPain1 = 0;									// IEのProxyを使う場合はProxyペインサイズを0に
		m_StatusBar.SetPaneWidth(ID_PAIN_1, dwSzPain1);
	}

	m_StatusBar.SetPaneWidth(ID_SECURE_PANE	, 25);
	m_StatusBar.SetPaneWidth(ID_PRIVACY_PANE , 25);
	
	// init_loadStatusBarState
	CIniFileI pr( g_szIniFileName, _T("Main") );
	BOOL	bStatusBarVisible = TRUE;
	MtlGetProfileStatusBarState(pr, m_hWndStatusBar, bStatusBarVisible);
}

void	CMainFrame::Impl::_initSplitterWindow()
{
	m_hWndClient = m_SplitterWindow.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	ATLASSERT( ::IsWindow(m_hWndClient) );
	m_SplitterWindow.SetSplitterExtendedStyle(SPLIT_GRADIENTBAR/*0*/);
}

void	CMainFrame::Impl::_initChildFrameClient()
{
	m_ChildFrameClient.Create(m_hWndClient);
	ATLASSERT( m_ChildFrameClient.IsWindow() );
	m_ChildFrameClient.SetGetSearchStringFunc([this](CString& text)-> bool {
		if (m_GlobalConfigManageData.pGlobalConfig->bSaveSearchWord == false)
			return false;

		m_SearchBar.GetEditCtrl().GetWindowText(text.GetBuffer(1024 + 1), 1024);
		text.ReleaseBuffer();
		return true;
	});

	m_TabBar.SetChildFrameClient(&m_ChildFrameClient);
}

void	CMainFrame::Impl::_initExplorerBar()
{
	m_ExplorerBar.Create(m_hWndClient);
	ATLASSERT( m_ExplorerBar.IsWindow() );

	m_ExplorerBar.SetFuncSinglePaneMode(std::bind(&CSplitterWindow::SetSinglePaneMode, &m_SplitterWindow, SPLIT_PANE_RIGHT));

	m_SplitterWindow.SetSplitterPanes(m_ExplorerBar, m_ChildFrameClient);

	UpdateLayout();

	m_ExplorerBar.InitPane();
}

void	CMainFrame::Impl::_initSkin()
{
	/* フォント */
	m_TabBar.SetFont		(CSkinOption::s_lfTabBar.CreateFontIndirect());
	m_AddressBar.SetFont	(CSkinOption::s_lfAddressBar.CreateFontIndirect());
	m_SearchBar.SetFont		(CSkinOption::s_lfSearchBar.CreateFontIndirect());
	m_LinkBar.SetFont		(CSkinOption::s_lfLinkBar.CreateFontIndirect());
	m_StatusBar.SetProxyComboBoxFont(CSkinOption::s_lfProxyComboBox.CreateFontIndirect());

	CIniFileI	prFont( g_szIniFileName, _T("Main") );
	MTL::CLogFont	lf;
	lf.InitDefault();
	if ( lf.GetProfile(prFont) ) {
		m_CommandBar.SetFont(::CreateFontIndirect(&lf));

		CFontHandle 	font;	//\\ SetFontもここでするように
		MTLVERIFY( font.CreateFontIndirect(&lf) );
		if (font.m_hFont) 
			SetFont(font);
	} else {
		WTL::CLogFont	lf;
		lf.SetMenuFont();
		m_CommandBar.SetFont(lf.CreateFontIndirect());
	}

	/* スキン */
	//initCurrentIcon();											//+++ アイコン
	m_ReBar.RefreshSkinState(); 								//ReBar
	m_TabBar.ReloadSkin();										//タブ
	m_ToolBar.ReloadSkin(); 									//ツールバー
	m_AddressBar.ReloadSkin(CSkinOption::s_nComboStyle);		//アドレスバー
	m_SearchBar.ReloadSkin(CSkinOption::s_nComboStyle); 		//検索バー
	//m_LinkBar.InvalidateRect(NULL, TRUE);						//リンクバー
//	m_ExplorerBar.ReloadSkin(); 								//エクスプローラバー
//	m_ExplorerBar.m_PanelBar.ReloadSkin();						//パネルバー
//	m_ExplorerBar.m_PluginBar.ReloadSkin(); 					//プラグインバー
	
	m_StatusBar.ReloadSkin( CSkinOption::s_nStatusStyle		//ステータスバー
							 , CSkinOption::s_nStatusTextColor
							 , CSkinOption::s_nStatusBackColor);
}

void	CMainFrame::Impl::_initSysMenu()
{
	CMenuHandle SysMenu = GetSystemMenu(FALSE);

	SysMenu.InsertMenu(0, MF_BYPOSITION | MF_STRING, ID_VIEW_COMMANDBAR, _T("メニューを表示(&V)"));
	SysMenu.InsertMenu(1, MF_BYPOSITION | MF_SEPARATOR);
	SysMenu.InsertMenu(2, MF_BYPOSITION | MF_STRING, ID_VIEW_PROCESSMONITOR, _T("プロセス モニター(&P)"));
	SysMenu.InsertMenu(3, MF_BYPOSITION | MF_SEPARATOR);
}


void	CMainFrame::Impl::_SaveBandPosition()
{
	CIniFileO pr(g_szIniFileName, _T("ReBar"));
	UINT nCount = m_ReBar.GetBandCount();
	for (UINT i = 0; i < nCount; ++i) {
		REBARBANDINFO	rbi = { sizeof(REBARBANDINFO) };
		rbi.fMask	= RBBIM_SIZE | RBBIM_STYLE | RBBIM_CHILD;
		m_ReBar.GetBandInfo(i, &rbi);
		int nCtrlID = ::GetDlgCtrlID(rbi.hwndChild);
		CString prefix;
		prefix.Format(_T("band#%d"), nCtrlID);
		pr.SetValue(i			, prefix + _T(".index"));
		pr.SetValue(rbi.cx		, prefix + _T(".cx"));
		pr.SetValue(rbi.fStyle	, prefix + _T(".fStyle"));
	}
}


// Message handler


// リバーとビューの間の横線を書く
void	CMainFrame::Impl::OnPaint(CDCHandle /*dc*/)
{
	//SetMsgHandled(FALSE);

	// Constants
	enum _ReBarBorderConstants {
		s_kcxBorder = 2
	};

	CPaintDC	dc(m_hWnd);

	CRect rc;
	m_ReBar.GetClientRect(&rc);

	rc.InflateRect(s_kcxBorder, s_kcxBorder);
	dc.DrawEdge(rc, EDGE_ETCHED, BF_RECT);
}

/// フォーカスを復元する
void	CMainFrame::Impl::OnActivate(UINT nState, BOOL bMinimized, CWindow wndOther)
{
	if (nState == WA_INACTIVE) {
		m_hWndRestoreFocus = ::GetFocus();
	} else if (m_hWndRestoreFocus) {
		::SetFocus(m_hWndRestoreFocus);
	}
}

/// タイトルバーの右クリックメニューのコマンド
void	CMainFrame::Impl::OnSysCommand(UINT nID, CPoint pt)
{
	switch (nID) {
	case ID_VIEW_COMMANDBAR:
		OnViewBar(0, ID_VIEW_COMMANDBAR, NULL);
		break;

	case ID_VIEW_PROCESSMONITOR:
		{
			if (m_pProcessMonitor) {
				m_pProcessMonitor->ShowWindow(TRUE);
				m_pProcessMonitor->SetActiveWindow();
			} else {
				m_pProcessMonitor.reset(new CProcessMonitorDialog(std::bind(&CDonutTabBar::ForEachWindow, &m_TabBar, std::placeholders::_1), CMainOption::s_BrowserOperatingMode == BROWSEROPERATINGMODE::kMultiProcessMode));
				m_pProcessMonitor->Create(m_hWnd);
				m_pProcessMonitor->ShowWindow(TRUE);
			}
		}
		break;

	case SC_RESTORE:
		if (m_bFullScreen) {
			_FullScreen(false);
		} else {
			SetMsgHandled(FALSE);
		}
		break;

	//+++ 最小化ボタンを押した時に、タスクトレイに入るようにしてみる.
	case SC_MINIMIZE:
		if ((CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MINBTN2TRAY)	//+++ 最小化ボタンでタスクトレイに入れる設定のとき、
			&& (pt.x || pt.y)											//+++ x,yが0,0ならタスクバーでクリックした場合だろうで、トレイにいれず、最小化だけしてみる.
		) {
			OnGetOut(0,0,0);
			SetMsgHandled(TRUE);
			break;
		}
		SetMsgHandled(FALSE);
		break;

	case SC_CLOSE:
		if ((CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_CLOSEBTN2TRAY)	//+++ 最小化ボタンでタスクトレイに入れる設定のとき、
			&& (pt.x || pt.y)											//+++ x,yが0,0ならタスクバーでクリックした場合だろうで、トレイにいれず、最小化だけしてみる.
		) {
			OnGetOut(0,0,0);
			SetMsgHandled(TRUE);
			break;
		}
		SetMsgHandled(FALSE);
		break;

	default:
		SetMsgHandled(FALSE);
		break;
	}

}

/// メニューの右端にショートカットキーを表示するように設定する
void CMainFrame::Impl::OnInitMenuPopup(CMenuHandle menuPopup, UINT nIndex, BOOL bSysMenu)
{
	SetMsgHandled(FALSE);

	if (bSysMenu)	// システムメニューは、処理しない
		return ;
	
	//if (   hMenuPopup == m_FavoriteMenu.GetMenu().m_hMenu 
	//	|| hMenuPopup == m_FavGroupMenu.GetMenu().m_hMenu 
	//	|| hMenuPopup == m_styleSheetMenu.GetMenu().m_hMenu
	//	|| m_FavoriteMenu.IsSubMenu(hMenuPopup) )
	//	return 0;

	//CMenuHandle 	menu = hMenuPopup;
	//CAccelerManager accel(m_hAccel);

	// 一番目のメニューが得られない時はそれを消して編集しない
	CString		strCmd;
	if (menuPopup.GetMenuString(0, strCmd, MF_BYPOSITION) == 0) {
		menuPopup.RemoveMenu(0, MF_BYPOSITION);
		return ;
	}
}


BOOL	CMainFrame::Impl::OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
	switch (pCopyDataStruct->dwData) {
	case kNewDonutInstance:
		{
			CString strURL = static_cast<LPCTSTR>(pCopyDataStruct->lpData);
			_NewDonutInstance(strURL);
		}
		break;

	case kNewDonutLink:
		{
			CString strURL = static_cast<LPCTSTR>(pCopyDataStruct->lpData);
			NewChildFrameData	data(m_ChildFrameClient);
			data.strURL	= strURL;
			data.bActive= true;
			data.bLink	= true;
			CChildFrame::AsyncCreate(data);
		}
		break;

	case kSearchTextWithEngine:
		{
			CString str = (LPCWSTR)pCopyDataStruct->lpData;
			int nIndex = str.Find(_T('\n'));
			CString strEngine = str.Left(nIndex);
			CString strText	= str.Mid(nIndex + 1);
			m_SearchBar.SearchWebWithEngine(strText, strEngine);
		}
		break;

	case kSetSearchText:
		{
			CString str = (LPCWSTR)pCopyDataStruct->lpData;
			bool bHilight = false;
			if (str.Left(1) == _T("1"))
				bHilight = true;
			m_SearchBar.SetSearchStr(str.Mid(1));
			m_SearchBar.ForceSetHilightBtnOn(bHilight);
		}
		break;

	case kOpenMultiUrl:
		{
			std::vector<CString> vecUrl = GetMultiText((LPCWSTR)pCopyDataStruct->lpData);
			std::vector<OpenMultiFileData> vecData;
			std::transform(vecUrl.begin(), vecUrl.end(), std::back_inserter(vecData), [](const CString& url) {
				return CMainFrame::OpenMultiFileData(url);
			});
			UserOpenMultiFile(vecData, true, false);
		}
		break;

	case kFileDownload:
		{
			std::wstringstream ss;			
			ss << static_cast<LPCTSTR>(pCopyDataStruct->lpData);
			boost::archive::text_wiarchive	ar(ss);
			DownloadData DLdata;
			ar >> DLdata;

			if (CDLOptions::bShowWindowOnDL)
				m_DownloadManager.OnShowDLManager(0, 0, NULL);

			auto pCustomBindStatusCallback = CDownloadManager::CreateCustomBindStatusCallBack(m_hWnd, CDLOptions::strDLFolderPath);
			pCustomBindStatusCallback->SetReferer(DLdata.strReferer.c_str());			
			pCustomBindStatusCallback->SetOption(DLdata.strFolder.c_str(), NULL, DLdata.dwImageExStyle);

			CDownloadManager::StartTheDownload(DLdata.strURL.c_str(), pCustomBindStatusCallback);

		}
		break;

	case kOpenFindBarWithText:
		if (CMainOption::s_bUseCustomFindBar) {
			m_FindBar.ShowFindBar(static_cast<LPCTSTR>(pCopyDataStruct->lpData));
			return 1;
		}
		break;

	case kCommandDirect:
		{
			CString str = (LPCWSTR)pCopyDataStruct->lpData;
			if (str.Find( _T("tp://") ) != -1
				|| str.Find( _T("https://") ) != -1
				|| str.Find( _T("file://") ) != -1) {
				Misc::StrToNormalUrl(str);		//+++
				UserOpenFile(str, D_OPENFILE_CREATETAB | DonutGetStdOpenActivateFlag());
			} else {
				if (CMouseOption::s_nDragDropCommandID == ID_SEARCH_DIRECT) {				// 即検索
					m_SearchBar.SearchWeb(str);

				} else if (CMouseOption::s_nDragDropCommandID == ID_OPENLINK_DIRECT) {	// リンクを開く
					SendMessage(m_ChildFrameClient.GetActiveChildFrameWindow(), WM_COMMAND, ID_EDIT_OPEN_SELECTED_REF, 0);
				}
			}
		}
		break;

	case kAddIgnoreURL:
	case kAddCloseTitle:
		{
			CString str = (LPCWSTR)pCopyDataStruct->lpData;
			if (pCopyDataStruct->dwData == kAddIgnoreURL)
				CSupressPopupOption::AddIgnoreURL(str);
			else
				CSupressPopupOption::AddCloseTitle(str);
			
			break;
		}

	case kDebugTrace:
		TRACE(static_cast<LPCTSTR>(pCopyDataStruct->lpData));
		break;

	default:
		SetMsgHandled(FALSE);
		break;
	}
	return 0;
}

/// タスクトレイからの通知メッセージ
LRESULT CMainFrame::Impl::OnMyNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ( IsWindowVisible() ) {
		return -1;
	} else {
		switch (lParam) {
		case WM_LBUTTONUP:
			//_DeleteTrayIcon();
			OnGetOut(0, 0, NULL);
			return 0;
			break;

		case WM_RBUTTONUP:
			{
				::SetForegroundWindow(m_hWnd);
				CMenu	menu0;
				menu0.LoadMenu(IDR_TRAYICON_MENU);
				CMenuHandle menu = menu0.GetSubMenu(0);
				ATLASSERT(menu.IsNull() == false);

				// ポップアップメニューを開く.
				POINT 	pt;
				::GetCursorPos(&pt);
				HRESULT hr = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON| TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
				if (hr == 57666/*復帰*/) {
					//_DeleteTrayIcon();
					OnGetOut(0, 0, NULL);
					return 0;
				}
				if (hr == 57665/*終了*/) {
					//_DeleteTrayIcon();
					OnGetOut(0, 0, NULL);
					PostMessage(WM_CLOSE);
					break;
				}
			}
		}

		return -1;
	}
}

BOOL	CMainFrame::Impl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (  CTabBarOption::s_bWheel
	   && MtlIsBandVisible(m_hWndToolBar, IDC_MDITAB) )
	{
		CRect rcTab;
		m_TabBar.GetWindowRect(&rcTab);

		if ( rcTab.PtInRect(pt) ) {
			if (zDelta > 0) {
				m_TabBar.LeftTab();
			} else {
				m_TabBar.RightTab();
			}
			return 0;
		}
	}

	// 右クリックされていたら - スクロールのあるビュー上だとビューがスクロールされるバグ
	//if (nFlags == VK_RBUTTON) {
	//	if (zDelta > 0) {
	//		m_TabBar.LeftTab();
	//	} else {
	//		m_TabBar.RightTab();
	//	}
	//	return 0;
	//}

	SetMsgHandled(FALSE);
	return 1;
}

/// コマンドラインを実行する
void	CMainFrame::Impl::OnInitProcessFinished(bool bHome)
{
	static bool s_bInit = false;
	if (s_bInit == false) {
		extern CString g_commandline;	// Donut.cppにある
		if (bHome) {
			m_deqNewChildFrameData.push_back(std::unique_ptr<NewChildFrameData>());
			std::vector<CString> vecUrl;
			PerseUrls(g_commandline, vecUrl);
			for (auto it = vecUrl.cbegin(); it != vecUrl.cend(); ++it) {
				NewChildFrameData*	pdata = new NewChildFrameData(m_ChildFrameClient);
				pdata->strURL	= *it;
				pdata->bActive	= it == vecUrl.cbegin();
				m_deqNewChildFrameData.push_back(std::unique_ptr<NewChildFrameData>(std::move(pdata)));
			}
			// あとはOnTabCreateがタブを作ってくれる
		} else {
			bool bSaveFlag = CMainOption::s_bExternalNewTabActive;
			CMainOption::s_bExternalNewTabActive = true;
			_NewDonutInstance(g_commandline);
			CMainOption::s_bExternalNewTabActive = bSaveFlag;
		}
		// タブリストの定期保存を開始
		if (CMainOption::s_dwMainExtendedStyle & MAIN_EX_BACKUP)
			SetTimer(kAutoBackupTimerId, CMainOption::s_dwBackUpTime * 60000);

		s_bInit = true;
	}
}


void	CMainFrame::Impl::OnBrowserTitleChange(HWND hWndChildFrame, LPCTSTR strTitle)
{
	if (::wcsncmp(strTitle, L"ポップアップをブロックしました。: ", 18) != 0)
		m_TabBar.SetTitle(hWndChildFrame, strTitle);

	// キャプションを変更
	if (m_ChildFrameClient.GetActiveChildFrameWindow() == hWndChildFrame) {
		CString strapp;
		strapp.LoadString(IDR_MAINFRAME);
		CString strMainTitle;
		strMainTitle.Format(_T("%s - %s"), strTitle, strapp);
		SetWindowText(strMainTitle);
	}
}

void	CMainFrame::Impl::OnBrowserLocationChange(LPCTSTR strURL, HICON hFavicon)
{
	m_AddressBar.SetWindowText(strURL);	
	m_AddressBar.ReplaceIcon(hFavicon);
}

/// 検索バーから呼ばれる 検索したときに飛んでくる関数
LRESULT CMainFrame::Impl::OnOpenWithExProp(_EXPROP_ARGS *pArgs)
{
	ATLASSERT(pArgs);

	HWND hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
	//CChildFrame *pActiveChild = GetActiveChildFrame();
	//if( pActiveChild)
	//	pActiveChild->SaveSearchWordflg(false); //\\ 検索バーで検索したときアクティブなタブの検索文字列を保存しないようにする

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
	if (CSearchBarOption::s_bAutoHilight) {
		data.bAutoHilight = true;
		data.searchWord	= pArgs->strSearchWord;
	}

	if (hWndActive && _check_flag(D_OPENFILE_NOCREATE, pArgs->dwOpenFlag)) {
		// 既存のタブをナビゲート
		COPYDATASTRUCT cds;
		cds.dwData	= kSetSearchText;
		cds.lpData	= static_cast<LPVOID>(data.searchWord.GetBuffer(0));
		cds.cbData	= (data.searchWord.GetLength() + 1) * sizeof(WCHAR);
		::SendMessage(hWndActive, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
		_NavigateChildFrame(hWndActive, data.strURL, data.dwDLCtrl, data.dwExStyle, data.dwAutoRefresh);

	} else {
		// 検索バーの文字列が消える件に一応の対処
		m_GlobalConfigManageData.pGlobalConfig->bSaveSearchWord = false;

		// 新規タブを作成する
		CString str = pArgs->strSearchWord;
		//data.funcCallAfterCreated = [str, bOldSaveFlag](CChildFrame* pChild) {
		//	CSearchBarOption::s_bSaveSearchWord = bOldSaveFlag;
	 // 		//+++ 子窓に検索設定を反映 (関数化)
		//	if (CSearchBarOption::s_bAutoHilight)
		//		pChild->SetSearchWordAutoHilight(str, true);
		//};
		data.bActive = true;
		CChildFrame::AsyncCreate(data);
	}

	return 0;
}

/// ChildFrameのURL別セキュリティリストを更新する
void	CMainFrame::Impl::OnUpdateUrlSecurityList()
{
	m_TabBar.ForEachWindow([](HWND hWnd) {
		::SendMessage(hWnd, WM_UPDATEURLSECURITYLIST, 0, 0);
	});
}

/// ChildFrameのカスタムコンテキストメニューを更新する
void	CMainFrame::Impl::OnUpdateCustomContextMenu()
{
	std::set<DWORD> setProcessId;
	m_TabBar.ForEachWindow([&setProcessId](HWND hWnd) {
		DWORD processId = 0;
		::GetWindowThreadProcessId(hWnd, &processId);
		auto it = setProcessId.insert(processId);
		if (it.second)	// 挿入が成功した場合、同じプロセスには送っていないので送る
			::SendMessage(hWnd, WM_UPDATECUSTOMCONTEXTMENU, 0, 0);
	});
}

/// ChildFrameにプロクシ切替を通知する
void	CMainFrame::Impl::OnSetProxyToChildFrame()
{
	if (CMainOption::s_BrowserOperatingMode == BROWSEROPERATINGMODE::kMultiProcessMode) {
		m_TabBar.ForEachWindow([](HWND hWnd) {
			::PostMessage(hWnd, WM_SETPROXYTOCHLDFRAME, 0, 0);
		});
	}
}

/// ChildFrameのポップアップ抑止の情報を更新する
void	CMainFrame::Impl::OnUpdateSupressPopupData()
{
	std::set<DWORD> setProcessId;
	m_TabBar.ForEachWindow([&setProcessId](HWND hWnd) {
		DWORD processId = 0;
		::GetWindowThreadProcessId(hWnd, &processId);
		auto it = setProcessId.insert(processId);
		if (it.second)	// 挿入が成功した場合、同じプロセスには送っていないので送る
			::SendMessage(hWnd, WM_UPDATESUPRESSPOPUPDATA, 0, 0);
	});
}

/// 現在のダウンロードマネージャーの設定をGlobalConfigに設定する
void	CMainFrame::Impl::OnSetDLConfigToGlobalConfig()
{
	m_GlobalConfigManageData.pGlobalConfig->bShowDLManagerOnDL	= CDLOptions::bShowWindowOnDL;
	::wcscpy_s(m_GlobalConfigManageData.pGlobalConfig->strDefaultDLFolder, CDLOptions::strDLFolderPath);
	::wcscpy_s(m_GlobalConfigManageData.pGlobalConfig->strImageDLFolder, CDLOptions::strImgDLFolderPath);
	m_GlobalConfigManageData.pGlobalConfig->dwDLImageExStyle	= CDLOptions::dwImgExStyle;
}

/// ChildProcessIdを追加/削除する
void	CMainFrame::Impl::OnAddRemoveChildProcessId(DWORD dwProcessId, bool bAdd)
{
	if (bAdd) {
		m_setChildProcessId.insert(dwProcessId);
	} else {
		m_setChildProcessId.erase(dwProcessId);
	}
}

// for DownloadManager
LRESULT CMainFrame::Impl::OnGetUniqueNumberForDLItem()
{
	static LRESULT unique = 0;
	++unique;
	return unique;
}



void	CMainFrame::Impl::OnFileOpen(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (nID == ID_VIEW_HOME) {
		if (m_ChildFrameUIState.GetActiveChildFrameWindowHandle() == NULL
			|| ::GetKeyState(VK_CONTROL) < 0) {
			nID = ID_FILE_NEW_HOME;
		} else {
			SetMsgHandled(FALSE);
			return ;
		}
	}

	if (nID == ID_FILE_NEW) {
		switch (CFileNewOption::s_dwFlags) {
		case FILENEW_BLANK:	nID = ID_FILE_NEW_BLANK;	break;
		case FILENEW_COPY:	nID = ID_FILE_NEW_COPY;		break;
		case FILENEW_HOME:	nID = ID_FILE_NEW_HOME;		break;
		case FILENEW_USER:
			{
				UserOpenFile(CFileNewOption::s_strUsr, DonutGetStdOpenActivateFlag());
				return ;
			}
			break;

		default:	nID = ID_FILE_NEW_BLANK;	break;
		}
	}

	switch (nID) {
	case ID_FILE_NEW_BLANK:
		UserOpenFile(_T("about:blank"), DonutGetStdOpenActivateFlag());
		PostMessage(WM_COMMAND, ID_SETFOCUS_ADDRESSBAR, 0);
		break;

	case ID_FILE_NEW_HOME:	// TODO: 後で変える
		UserOpenFile(_T("about:home"), DonutGetStdOpenActivateFlag());
		break;

	case ID_FILE_NEW_COPY:
		{
			auto pChildUIData = m_ChildFrameUIState.GetActiveChildFrameUIData();
			if (pChildUIData) 
				UserOpenFile(pChildUIData->strLocationURL, DonutGetStdOpenActivateFlag());
		}
		break;

	case ID_FILE_NEW_CLIPBOARD:
		{
			CString strText = MtlGetClipboardText();
			if ( strText.IsEmpty() )
				return;
			UserOpenFile( strText, DonutGetStdOpenActivateFlag() );
		}
		break;

	case ID_FILE_OPEN_TABLIST:
		{
			CString strPath = GetConfigFilePath(_T("TabList.donutTabList"));
			UserOpenFile( strPath, DonutGetStdOpenActivateFlag() );
		}
		break;

	case ID_FILE_OPEN:
		{
			COpenURLDlg dlg;
			if ( dlg.DoModal() == IDOK && !dlg.m_strEdit.IsEmpty() ) {
				UserOpenFile( dlg.m_strEdit, DonutGetStdOpenFlag() );
			}
		}
		break;
	}
}

void	CMainFrame::Impl::OnFileRecent(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	// get file name from the MRU list
	if (ID_FILE_MRU_FIRST <= nID && nID <= ID_FILE_MRU_LAST)			// 旧範囲IDから新範囲IDへ変換
		nID = ID_RECENTDOCUMENT_FIRST + (nID - ID_FILE_MRU_FIRST);

	ChildFrameDataOnClose*	pdata = nullptr;
	ATLVERIFY(m_RecentClosedTabList.GetFromList(nID, &pdata));
	NewChildFrameData	data(m_ChildFrameClient);
	data.strURL		= pdata->strURL;
	data.dwDLCtrl	= pdata->dwDLCtrl;
	data.bActive	= _check_flag(DonutGetStdOpenActivateFlag(), D_OPENFILE_ACTIVATE);
	if (m_ChildFrameClient.GetActiveChildFrameWindow() == NULL)
		data.bActive = true;
	data.TravelLogBack = pdata->TravelLogBack;
	data.TravelLogFore = pdata->TravelLogFore;
	m_RecentClosedTabList.RemoveFromList(nID);
	CChildFrame::AsyncCreate(data);
}


void	CMainFrame::Impl::OnEditOperation(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CEdit editAddress = m_AddressBar.GetEditCtrl();
	CEdit editSearch  = m_SearchBar.GetEditCtrl();

	if (::GetFocus() != editAddress && ::GetFocus() != editSearch) {
		SetMsgHandled(FALSE);
		return ;
	}
	bool bFocusAddressBar = (::GetFocus() == editAddress);
	switch (nID) {
	case ID_EDIT_CUT:	bFocusAddressBar ? editAddress.Cut() : editSearch.Cut();	break;
	case ID_EDIT_COPY:	bFocusAddressBar ? editAddress.Copy() : editSearch.Copy();	break;
	case ID_EDIT_PASTE:	bFocusAddressBar ? editAddress.Paste() : editSearch.Paste();	break;
	case ID_EDIT_SELECT_ALL:	bFocusAddressBar ? editAddress.SetSelAll() : editSearch.SetSelAll();	break;
	}
}

void	CMainFrame::Impl::OnSearchBarCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	HWND hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
	if (hWndActive == NULL)
		return ;

	CString str = _GetSelectText(m_SearchBar.GetEditCtrl());
	if ( str.IsEmpty() )
		return ;

	switch (nID) {
	case ID_SEARCHBAR_SEL_UP:	m_SearchBar.SearchPage(false);	break;
	case ID_SEARCHBAR_SEL_DOWN:	m_SearchBar.SearchPage(true);	break;
	case ID_SEARCHBAR_HILIGHT:	m_SearchBar.SearchHilight();	break;
	}
}

/// 検索バーからのページ内検索
LRESULT CMainFrame::Impl::OnFindKeyWord(LPCTSTR lpszKeyWord, BOOL bBack, long Flags /*= 0*/)
{
	HWND hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
	if (hWndActive == NULL)
		return 0;

	FindKeywordData	findKeywordData = { lpszKeyWord, bBack != 0, Flags };

	CString strSharedMemName;
	strSharedMemName.Format(_T("%s%#x"), FINDKEYWORDATASHAREDMEMNAME, lpszKeyWord);
	CSharedMemory sharedMem;
	sharedMem.Serialize(findKeywordData, strSharedMemName);

	LRESULT lRet = ::SendMessage(hWndActive, WM_CHILDFRAMEFINDKEYWORD, (WPARAM)lpszKeyWord, 0);
	return lRet;
}

LRESULT CMainFrame::Impl::OnHilight(CString strKeyword)
{
	HWND hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
	if (hWndActive == NULL)
		return 0;

	COPYDATASTRUCT cds = { sizeof(cds) };
	cds.dwData	= kHilightText;
	cds.lpData	= static_cast<LPVOID>(strKeyword.GetBuffer(0));
	cds.cbData	= (strKeyword.GetLength() + 1) * sizeof(TCHAR);
	return SendMessage(hWndActive, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
}


void	CMainFrame::Impl::OnViewBar(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	auto funcToggleBandVisible = [this](int nCtrlID) {
		MtlToggleBandVisible(m_ReBar, nCtrlID);
		UpdateLayout();
	};
	// Togle
	switch (nID) {
	case ID_VIEW_SEARCHBAR:		funcToggleBandVisible(IDC_SEARCHBAR);	break;
	case ID_VIEW_COMMANDBAR:	funcToggleBandVisible(ATL_IDW_COMMAND_BAR);	break;
	case ID_VIEW_TOOLBAR:		funcToggleBandVisible(ATL_IDW_TOOLBAR);	break;
	case ID_VIEW_ADDRESSBAR:	funcToggleBandVisible(IDC_ADDRESSBAR);	break;
	case ID_VIEW_LINKBAR:		funcToggleBandVisible(IDC_LINKBAR);	break;
	case ID_VIEW_TABBAR:		funcToggleBandVisible(IDC_MDITAB);	break;

	default:
		switch (nID) {
		case ID_VIEW_GOBUTTON:		m_AddressBar.ShowGoButton(!CAddressBarOption::s_bGoBtnVisible);	break;

		case ID_VIEW_TABBAR_MULTI:
			{
				CTabBarOption::s_bMultiLine = !CTabBarOption::s_bMultiLine;
				CTabBarOption::WriteProfile();	// 設定の保存
				m_TabBar.ReloadSkin();
			}
			break;

		case ID_VIEW_TOOLBAR_CUST:	m_ToolBar.Customize();	break;

		case ID_VIEW_TOOLBAR_LOCK:
			{
				REBARBANDINFO rbbi = { sizeof (REBARBANDINFO) };
				rbbi.fMask	= RBBIM_STYLE;
				if ( !m_ReBar.GetBandInfo(0, &rbbi) )
					return ;	// Bandが一つもない

				m_ReBar.LockBands( !( (rbbi.fStyle & RBBS_NOGRIPPER) != 0) );
			}
			break;

		case ID_VIEW_STATUS_BAR:
			{
				BOOL bNew = !::IsWindowVisible(m_hWndStatusBar);
				//UpdateTitleBar(_T(""), 0);		//+++		statusオフからオンにしたときに、タイトルバーに出してしたステータス文字列を消すため.
				CIniFileO	pr(g_szIniFileName, _T("Main"));
				pr.SetValue( bNew, _T("statusbar.Visible") );

				::ShowWindow(m_hWndStatusBar, bNew ? SW_SHOWNOACTIVATE : SW_HIDE);
				UpdateLayout();
			}
			break;
		}
		break;
	}	

}

/// 自動ログイン編集ダイアログを表示する
void	CMainFrame::Impl::OnAutoLoginEdit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	LoginInfomation info;
	HWND activeChildFrame = m_ChildFrameClient.GetActiveChildFrameWindow();
	if (activeChildFrame) {
		HANDLE hMapForClose = (HANDLE)::SendMessage(activeChildFrame, WM_GETLOGININFOMATION, NULL, 0);
		ATLASSERT( hMapForClose );
		CString sharedMemName;
		sharedMemName.Format(_T("%s%#x"), GETLOGININFOMATIONSHAREDMEMNAME, activeChildFrame);
		CSharedMemory sharedMem;
		sharedMem.Deserialize(info, sharedMemName);
		::SendMessage(activeChildFrame, WM_GETLOGININFOMATION, (WPARAM)hMapForClose, 0);
	}
	auto funcRefresh = [activeChildFrame]() {
		if (CMainOption::s_BrowserOperatingMode == BROWSEROPERATINGMODE::kMultiProcessMode)
			::SendMessage(activeChildFrame, WM_UPDATEAUTOLOGINDATALIST, 0, 0);
		::SendMessage(activeChildFrame, WM_COMMAND, ID_VIEW_REFRESH, 0);
	};
	CLoginInfoEditDialog dlg(info);
	dlg.SetAutoLoginfunc(funcRefresh);
	dlg.SetTabBarForEach(std::bind(&CDonutTabBar::ForEachWindow, &m_TabBar, std::placeholders::_1));
	dlg.DoModal(m_hWnd);

	::SetFocus(m_ChildFrameClient.GetActiveChildFrameWindow());
}

void	CMainFrame::Impl::OnSetFocusToBar(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	switch (nID) {
	case ID_SETFOCUS_ADDRESSBAR:	::SetFocus( m_AddressBar.GetEditCtrl() );	break;
	case ID_SETFOCUS_SEARCHBAR:		::SetFocus( m_SearchBar.GetEditCtrl() );	break;
	case ID_SETFOCUS_SEARCHBAR_ENGINE:	m_SearchBar.SetFocusToEngine();		break;
	case ID_VIEW_ADDBAR_DROPDOWN:	m_AddressBar.ShowDropDown(!m_AddressBar.GetDroppedStateEx());	break;
	}
}


void	CMainFrame::Impl::OnFavoriteAdd(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ChildFrameUIData* pUIData = m_ChildFrameUIState.GetActiveChildFrameUIData();
	if (pUIData == nullptr)
		return ;

	CFavoriteEditDialog dlg(true);
	dlg.SetNameURLFavicon(pUIData->strTitle, pUIData->strLocationURL, CFaviconManager::GetFavicon(pUIData->strFaviconURL));
	dlg.DoModal(m_hWnd);

	SendMessage(WM_REFRESH_EXPBAR);
}

void	CMainFrame::Impl::OnFavoriteOrganize(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CFavoriteEditDialog dlg(false);
	dlg.DoModal(m_hWnd);
	//bool bOldShell = _check_flag(MAIN_EX_ORGFAVORITEOLDSHELL, CMainOption::s_dwMainExtendedStyle);
	//MtlOrganizeFavorite( m_hWnd, bOldShell, DonutGetFavoritesFolder() );
}

/// お気に入りグループコマンド
void	CMainFrame::Impl::OnFavoriteGroupCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	switch (nID) {
	case ID_FAVORITE_GROUP_ADD:
		{
			CFavoriteGroupEditDialog	dlg(CFavoriteGroupEditDialog::kFavoriteGroupAdd);
			dlg.DoModal(m_hWnd);
		}
		break;


	case ID_FAVORITE_GROUP_SAVE:
		{
			CDonutTabList tabList = _CollectAllChildFrameData();
			CString favoriteGroupPath = Misc::GetExeDirectory() + _T("FavoriteGroup\\");
			std::time_t nowTime = time(NULL);
			tm*	timePtr = localtime(&nowTime);
			wchar_t buff[32] = L"";
			wcsftime(buff, sizeof(buff), L"%Y%m%d_%H%M%S", timePtr);
			CRenameDialog	renameDialog(buff, _T(""), false);
			if (renameDialog.DoModal(m_hWnd) == IDOK) {
				tabList.Save(favoriteGroupPath + renameDialog.GetNewFileName() + _T(".donutTabList"), false);

				CRootFavoriteGroupPopupMenu::LoadFavoriteGroup();
			}
		}
		break;

	case ID_FAVORITE_GROUP_ORGANIZE:
		{
			CFavoriteGroupEditDialog	dlg(CFavoriteGroupEditDialog::kFavoriteGroupOrganize);
			dlg.DoModal(m_hWnd);
		}
		break;
	}

}


void	CMainFrame::Impl::OnDoubleClose(UINT uNotifyCode, int nID, CWindow wndCtl) 
{
	CSupressPopupOption::s_PopupBlockData.bValidIgnoreURL = !CSupressPopupOption::s_PopupBlockData.bValidIgnoreURL;
	CSupressPopupOption::s_PopupBlockData.bValidCloseTitle = !CSupressPopupOption::s_PopupBlockData.bValidCloseTitle;
	CSupressPopupOption::ReCreateSupressPopupDataAndNotify();
}

void	CMainFrame::Impl::OnPopupClose(UINT uNotifyCode, int nID, CWindow wndCtl) 
{
	CSupressPopupOption::s_PopupBlockData.bValidIgnoreURL = !CSupressPopupOption::s_PopupBlockData.bValidIgnoreURL;
	CSupressPopupOption::ReCreateSupressPopupDataAndNotify();
}

void	CMainFrame::Impl::OnTitleClose(UINT uNotifyCode, int nID, CWindow wndCtl) 
{
	CSupressPopupOption::s_PopupBlockData.bValidCloseTitle = !CSupressPopupOption::s_PopupBlockData.bValidCloseTitle;
	CSupressPopupOption::ReCreateSupressPopupDataAndNotify();
}


/// タスクトレイに退避
void	CMainFrame::Impl::OnGetOut(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	static int nShow = SW_SHOWNORMAL;

	if ( IsWindowVisible() ) {
		//+++ メモ:窓状態のときはトレイ化
		WINDOWPLACEMENT wp = { sizeof (WINDOWPLACEMENT) };
		GetWindowPlacement(&wp);
		nShow = wp.showCmd;
#if 0
		m_bTray = true; 		//+++
		WINDOWPLACEMENT wp = { sizeof (WINDOWPLACEMENT) };
		::GetWindowPlacement(m_hWnd, &wp);
		nShow		  = wp.showCmd;
	  #if 1 //+++ フルスクリーン以外なら今が最大かどうかを控える
		if (m_bFullScreen == 0)
			m_bOldMaximized = (nShow == SW_MAXIMIZE);
	  #endif
#endif
		_SetHideTrayIcon();	//+++ トレイアイコン化
		//Sleep(100); 		// UDT TODO
	} else {				// just db F9 press , come here :p
		//m_bTray      = false;	//+++
		//m_bMinimized = 0;
		//x ShowWindow( SW_SHOW /*nShow*/ );

		_DeleteTrayIcon();	//+++ トレイアイコン削除

		ShowWindow(nShow);
	}
}


/// IEのオプションを表示する
void	CMainFrame::Impl::OnViewOption(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_ChildFrameClient.GetActiveChildFrameWindow() == NULL)
		MtlShowInternetOptions();
	else
		SetMsgHandled(FALSE);	// ChildFrameへ
}

/// Donutのオプションを表示
void	CMainFrame::Impl::OnViewOptionDonut(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	BOOL	bSkinChange = FALSE;

	CMenu	menu		= CExMenuManager::LoadFullMenu();

	CMainPropertyPage				pageMain(m_hWnd);
	CMainPropertyPage2				pageMain2(m_hWnd, m_RecentClosedTabList);
	CDLControlPropertyPage			pageDLC(m_hWnd);
	CMDITabPropertyPage 			pageTab(&m_TabBar, menu.m_hMenu);
	CDonutAddressBarPropertyPage	pageAddress(m_AddressBar, m_SearchBar);
	CDonutFavoritesMenuPropertyPage pageFav;
	CFileNewPropertyPage			pageFileNew;
	CStartUpPropertyPage			pageStartUp;
	CProxyPropertyPage				pageProxy;
	CKeyBoardPropertyPage			pageKeyBoard(m_hAccel, menu.m_hMenu, m_hWnd, std::bind(&CDonutTabBar::ForEachWindow, &m_TabBar, std::placeholders::_1));
	CToolBarPropertyPage			pageToolBar(menu.m_hMenu, &bSkinChange, std::bind(&CDonutToolBar::ReloadSkin, &m_ToolBar));
	CMousePropertyPage				pageMouse(menu.m_hMenu, m_SearchBar.GetSearchEngineMenuHandle());
	CMouseGesturePropertyPage		pageGesture(menu.m_hMenu);
	CSearchPropertyPage 			pageSearch;
	CMenuPropertyPage				pageMenu(menu.m_hMenu);
	CRightClickPropertyPage			pageRightMenu(menu, m_hWnd);
	CExplorerBarPropertyPage		pageExplorerBar(std::bind(&CDonutExplorerBar::HookMouseMoveForAutoShow, &m_ExplorerBar, std::placeholders::_1));
	CDestroyPropertyPage			pageDestroy;
	CSkinPropertyPage				pageSkin(m_hWnd, &bSkinChange);
	CLinkBarPropertyPage			pageLinks(m_LinkBar);

	CString strURL, strTitle;
	if (auto pChildUIData = CChildFrameCommandUIUpdater::GetActiveChildFrameUIData()) {
		strURL	 = pChildUIData->strLocationURL;
		strTitle = pChildUIData->strTitle;
	}

	CIgnoredURLsPropertyPage		pageURLs(strURL);
	CCloseTitlesPropertyPage		pageTitles(strTitle);
	CUrlSecurityPropertyPage		pageUrlSecu(strURL, m_hWnd);		//+++
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
	sheet.AddPage( pageExplorerBar		 );
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

	SetGlobalConfig(m_GlobalConfigManageData.pGlobalConfig);
	//m_cmbBox.ResetProxyList();

	if (CMainOption::s_dwMainExtendedStyle & MAIN_EX_BACKUP)
		SetTimer(kAutoBackupTimerId, CMainOption::s_dwBackUpTime * 60000);
	else
		KillTimer(kAutoBackupTimerId);

	//RtlSetMinProcWorkingSetSize();		//+++ (メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等)
	SendMessage(WM_COMMAND, ID_VIEW_SETFOCUS);
}


void	CMainFrame::Impl::OnTabClose(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	switch (nID) {
	case ID_WINDOW_CLOSE_EXCEPT:	/// このウィンドウ以外を閉じる
		{
			if ( !CDonutConfirmOption::OnCloseAllExcept(m_hWnd) )
				return ;
			CWaitCursor  cur;
			CLockRedrawMDIClient		lock(m_ChildFrameClient);
			CDonutTabBar::CLockRedraw	lock2(m_TabBar);
			vector<HWND>	vechwnd;
			HWND	hWndActive = m_ChildFrameUIState.GetActiveChildFrameWindowHandle();
			m_TabBar.ForEachWindow([&](HWND hWnd) { 
				if (hWnd == hWndActive)
					return ;
				vechwnd.push_back(hWnd);
			});
			boost::reverse(vechwnd);
			boost::for_each(vechwnd, [](HWND hWnd) { ::PostMessage(hWnd, WM_CLOSE, 0, 0); });
			//RtlSetMinProcWorkingSetSize();
		}
		break;

	case ID_WINDOW_CLOSE_ALL:		/// 全てのウィンドウを閉じる(すべて閉じる)
		{
			if ( !CDonutConfirmOption::OnCloseAll(m_hWnd) )
				return ;
			CWaitCursor  cur;
			CLockRedrawMDIClient		lock(m_ChildFrameClient);
			CDonutTabBar::CLockRedraw	lock2(m_TabBar);
			vector<HWND>	vechwnd;
			m_TabBar.ForEachWindow([&](HWND hWnd) {
				vechwnd.push_back(hWnd);
			});
			boost::reverse(vechwnd);
			m_ChildFrameClient.SetChildFrameWindow(NULL);
			boost::for_each(vechwnd, [](HWND hWnd) { ::PostMessage(hWnd, WM_CLOSE, 0, 0); });
			//RtlSetMinProcWorkingSetSize();
		}
		break;

	case ID_LEFT_CLOSE:		/// タブの右側/左側をすべて閉じる
	case ID_RIGHT_CLOSE:
		{
			bool bLeft = (nID == ID_LEFT_CLOSE);
			if ( !CDonutConfirmOption::OnCloseLeftRight(m_hWnd, bLeft) )
				return ;

			CWaitCursor cur;
			HWND	hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
			int 	nCurSel    = m_TabBar.GetTabIndex(hWndActive);
			if (nCurSel == -1 || hWndActive == NULL)
				return ;

			CSimpleArray<HWND> arrWnd;
			int nCount = m_TabBar.GetItemCount();
			for (int ii = 0; ii < nCount; ++ii) {
				HWND hWnd = m_TabBar.GetTabHwnd(ii);
				if (bLeft && ii < nCurSel)
					arrWnd.Add(hWnd);
				else if (bLeft == false && ii > nCurSel)
					arrWnd.Add(hWnd);
			}
			for (int ii = 0; ii < arrWnd.GetSize(); ++ii) {
				::PostMessage(arrWnd[ii], WM_CLOSE, 0, 0);
			}
		}
		break;
	}
}

void	CMainFrame::Impl::OnTabSwitch(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (nID == ID_TAB_LEFT) {
		m_TabBar.LeftTab();
	} else {
		m_TabBar.RightTab();
	}
}

void	CMainFrame::Impl::OnOperateCommandToAllTab(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nApplyCommand = 0;
	HWND hWndExceptChildFrame = NULL;
	switch (nID) {
	case ID_VIEW_STOP_ALL:
		nApplyCommand = ID_VIEW_STOP;
		break;

	case ID_VIEW_REFRESH_ALL:
		nApplyCommand = ID_VIEW_REFRESH;
		break;

	case ID_WINDOW_REFRESH_EXCEPT:
		nApplyCommand = ID_VIEW_REFRESH;
		hWndExceptChildFrame = m_ChildFrameClient.GetActiveChildFrameWindow();
		break;
	}
	m_TabBar.ForEachWindow([=](HWND hWnd) {
		if (hWnd != hWndExceptChildFrame)
			::SendMessage(hWnd, WM_COMMAND, nApplyCommand, 0);
	});
}

/// Donutのあるフォルダを開く
void	CMainFrame::Impl::OnOpenDonutExeFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString	progDir = Misc::GetExeDirectory();
	ShellExecute(NULL, NULL, progDir, progDir, NULL, SW_SHOW);
}

/// 作者のページを開く
void	CMainFrame::Impl::OnAuthorWebSite(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	//CString strSite = _T("http://www5.ocn.ne.jp/~minute/tab/"); //unDonut古い方
	//CString strSite = _T("http://undonut.sakura.ne.jp/"); // unDonut新しい方
	//CString strSite = _T("http://tekito.genin.jp/undonut+.html"); //unDonut+
	//CString strSite = _T("http://ichounonakano.sakura.ne.jp/64/undonut/"); //unDonut+mod.	旧
	//CString strSite = _T("http://undonut.undo.jp/"); //unDonut+mod.
	//CString strSite = _T("http://cid-8830de058eedff85.skydrive.live.com/browse.aspx/%e5%85%ac%e9%96%8b/unDonut"); // 安堂夏.
	CString strSite = _T("http://www31.atwiki.jp/lafe/pages/32.html");

	UserOpenFile( strSite, DonutGetStdOpenFlag() );
}


/// メインメニューの [表示]-[ツールバー]を表示する
LRESULT CMainFrame::Impl::OnShowToolBarMenu()
{
	CPoint pt;
	::GetCursorPos(&pt);	
	
	CMenu main_menu;
	main_menu.LoadMenu(IDR_MAINFRAME);
	enum { kShowPos = 2, kToolbarPos = 0 };
	CMenuHandle submenu = main_menu.GetSubMenu(kShowPos).GetSubMenu(kToolbarPos);
	if ( submenu.IsMenu() )
		submenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);
	return 0;
}


/// リバーのバンドの左端にテキストを設定する
void	CMainFrame::Impl::OnShowBandTextChange(bool bShow)
{
	ATLASSERT(m_ReBar.IsWindow());

	REBARBANDINFO rbBand = { sizeof (REBARBANDINFO) };
	int nCount = m_ReBar.GetBandCount();
	for (int ii = 0; ii < nCount; ii++) {
		rbBand.fMask = RBBIM_ID;
		m_ReBar.GetBandInfo(ii, &rbBand);
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
		m_ReBar.SetBandInfo(ii, &rbBand);
	}
}



void	CMainFrame::Impl::OnTabCreate(HWND hWndChildFrame, DWORD dwOption)
{
	if (dwOption & TAB_LINK) {
		m_TabBar.SetLinkState(LINKSTATE_A_ON);
		// ページ内のリンクを開くとき、作成された新規タブをアクティブにしない
		if (!_check_flag(MAIN_EX_NOACTIVATE_NEWWIN, CMainOption::s_dwMainExtendedStyle))
			dwOption |= TAB_ACTIVE;
	}
	m_TabBar.OnMDIChildCreate(hWndChildFrame, (dwOption & TAB_ACTIVE) != 0);
	
	//if ( _check_flag(m_view.m_ViewOption.m_dwExStyle, DVS_EX_OPENNEWWIN)) {
	//	m_MDITab.NavigateLockTab(m_hWnd, true);
	//}
	if (m_deqNewChildFrameData.size() > 0) {
		m_deqNewChildFrameData.pop_front();
		if (m_deqNewChildFrameData.size() > 0) {
			if (m_deqNewChildFrameData[0]->bLink == false)
				m_TabBar.SetInsertIndex(m_TabBar.GetItemCount());
			CChildFrame::AsyncCreate(*m_deqNewChildFrameData[0]);
		} else {
			m_TabBar.InsertHere(false);
			PostMessage(WM_INITPROCESSFINISHED);
		}
	} else {
		m_TabBar.InsertHere(false);	// タブバーからの操作を終わらせる
	}
}

void	CMainFrame::Impl::OnTabDestory(HWND hWndChildFrame)
{
	m_TabBar.OnMDIChildDestroy(hWndChildFrame);
}

void	CMainFrame::Impl::OnAddRecentClosedTab(HWND hWndChildFrame)
{
	std::unique_ptr<ChildFrameDataOnClose>	pClosedTabData(new ChildFrameDataOnClose);
	CString strSharedMemName;
	strSharedMemName.Format(_T("%s%#x"), CHILDFRAMEDATAONCLOSESHAREDMEMNAME, hWndChildFrame);
	CSharedMemory sharedMem;
	sharedMem.Deserialize(*pClosedTabData, strSharedMemName);

	m_RecentClosedTabList.AddToList(std::move(pClosedTabData));
}

void	CMainFrame::Impl::OnChildFrameExStyleChange(HWND hWndChildFrame, DWORD ExStyle)
{
	m_TabBar.NavigateLockTab(hWndChildFrame, (ExStyle & DVS_EX_OPENNEWWIN) != 0);
}


static DWORD GetMouseButtonCommand(const MSG& msg)
{
	CString 	strKey;
	switch (msg.message) {
	case WM_LBUTTONUP:	strKey = _T("LButtonUp");					break;
	case WM_MBUTTONUP:	strKey = _T("MButtonUp");					break;
	case WM_XBUTTONUP:	strKey.Format(_T("XButtonUp%d"), GET_XBUTTON_WPARAM(msg.wParam)); break;
	case WM_MOUSEWHEEL:
		short zDelta = (short)HIWORD(msg.wParam);
		if (zDelta > 0)
			strKey = _T("WHEEL_UP");
		else
			strKey = _T("WHEEL_DOWN");
		break;
	}

	CIniFileI	pr( GetConfigFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
	return pr.GetValue(strKey, 0);;
}

static int PointDistance(const CPoint& pt1, const CPoint& pt2)
{
	return (int)sqrt( pow(float (pt1.x - pt2.x), 2.0f) + pow(float (pt1.y - pt2.y), 2.0f) );
}

bool	CMainFrame::Impl::_RButtonHook(MouseGestureData data)
{
	CString StatusText;
	auto funcSetStatusText = [&StatusText, this](LPCTSTR text) {
		if (StatusText != text) {
			StatusText = text;
			m_StatusBar.SetWindowText(text);
		}
	};

	::SetCapture(m_hWnd);

	CPoint	ptDown(GET_X_LPARAM(data.lParam), GET_Y_LPARAM(data.lParam));
	::ClientToScreen(data.hwnd, &ptDown);
	CPoint	ptLast = ptDown;

	HMODULE	hModule	= ::LoadLibrary(_T("ole32.dll"));
	CCursor cursor	= ::LoadCursor(hModule, MAKEINTRESOURCE(3));
	CString	strSearchEngine;
	CString strLastMark;
	CString strMove;
	DWORD	dwTime = 0;
	int		nDistance = 0;
	bool	bNoting = true;	// 何もしなかった
	bool	bCancelRButtonUp = false;
	MSG msg = { 0 };
	do {
		BOOL nRet = GetMessage(&msg, NULL, 0, 0);
		if (nRet == 0 || nRet == -1 || GetCapture() != m_hWnd)
			break;
		if (GetAsyncKeyState(VK_RBUTTON) > 0) {		// 右ボタンが離された
			funcSetStatusText(_T(""));
			break;
		}

		DWORD dwCommand = 0;
		switch (msg.message) {
		case WM_MOUSEWHEEL:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
			dwCommand = GetMouseButtonCommand(msg);
			break;

		case WM_MOUSEMOVE: {
			SetCursor(::LoadCursor(NULL, IDC_ARROW));
			if (bNoting == false)	// 他のコマンドを実行済み
				break;
			CPoint	ptNow(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			::ClientToScreen(msg.hwnd, &ptNow);
			if (data.bCursorOnSelectedText) {
				if (nDistance < 10) {
					nDistance = PointDistance(ptDown, ptNow);	// 距離を求める
					if (nDistance < 10)
						break;
				}
				SetCursor(cursor);	// カーソルを変更する

				CString strMark;
				if (m_GlobalConfigManageData.pGlobalConfig->bUseRect) {
					int nAng  = (int) _GetAngle(ptDown, ptNow);	// 角度を求める
					if		  (nAng <  45 || nAng >  315) {
						strSearchEngine = m_GlobalConfigManageData.pGlobalConfig->strREngine;	
						strMark = _T("[→] ");
					} else if (nAng >= 45 && nAng <= 135) {
						strSearchEngine = m_GlobalConfigManageData.pGlobalConfig->strTEngine;	
						strMark = _T("[↑] ");
					} else if (nAng > 135 && nAng <  225) {
						strSearchEngine = m_GlobalConfigManageData.pGlobalConfig->strLEngine;	
						strMark = _T("[←] ");
					} else if (nAng >= 225 && nAng <= 315) {
						strSearchEngine = m_GlobalConfigManageData.pGlobalConfig->strBEngine;
						strMark = _T("[↓] ");
					}
				} else {
					strSearchEngine = m_GlobalConfigManageData.pGlobalConfig->strCEngine;
				}
				if (strSearchEngine.IsEmpty() == FALSE) {
					CString strMsg;
					strMsg.Format(_T("検索 %s: %s"), strMark, strSearchEngine);
					funcSetStatusText(strMsg);
				} else {
					funcSetStatusText(_T(""));
				}
			} else {
				nDistance = PointDistance(ptLast, ptNow);	// 距離を求める
				if (nDistance < 10)
					break;
				
				CString strMark1;
				int nAng1  = (int) _GetAngle(ptLast, ptNow);	// 角度を求める
				if		(nAng1 <  45 || nAng1 >  315)
					strMark1 = _T("→");
				else if (nAng1 >= 45 && nAng1 <= 135)
					strMark1 = _T("↑");
				else if (nAng1 > 135 && nAng1 <  225)
					strMark1 = _T("←");
				else if (nAng1 >= 225 && nAng1 <= 315)
					strMark1 = _T("↓");

				if (strMark1 == strLastMark) {					// 同じ方向に動かして、かつ300ms以上経ったなら有効		
					DWORD dwTimeNow = ::GetTickCount();
					if ( (dwTimeNow - dwTime) > 300 ) {
						strLastMark = _T("");
						dwTime	 = dwTimeNow;
					}
				}
				if (strMark1 != strLastMark) {
					strMove	+= strMark1;	// 方向を追加
					strLastMark = strMark1;	

					CString strCmdName;
					CIniFileI	pr( GetConfigFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
					DWORD	dwCommand = pr.GetValue(strMove);
					if (dwCommand) {
						// 合致するコマンドがあれば表示
						CString strTemp;
						CToolTipManager::LoadToolTipText(dwCommand, strTemp);
						strCmdName.Format(_T("[ %s ]"), strTemp);
					}

					// ステータスバーに表示
					CString 	strMsg;
					strMsg.Format(_T("ジェスチャー : %s %s"), strMove, strCmdName);
					funcSetStatusText(strMsg);
				}
				dwTime = ::GetTickCount();
			}
			ptLast = ptNow;
			break;
							}

		case WM_LBUTTONDOWN:
			if (data.bCursorOnSelectedText && nDistance >= 10) {	// 右ボタンドラッグをキャンセルする
				bCancelRButtonUp = true;
				msg.message = WM_RBUTTONUP;
				funcSetStatusText(_T(""));
			}
			break;

		default:
			::DispatchMessage(&msg);
			break;
		}	// switch

		switch (dwCommand) {
		case 0:	
			break;
			
		case ID_FILE_CLOSE:
			::PostMessage(m_ChildFrameClient.GetActiveChildFrameWindow(), WM_CLOSE, 0, 0);	// これだけじゃたぶんダメ
			//msg.message = WM_RBUTTONUP;
			//::PostMessage(m_hWnd, WM_COMMAND, ID_FILE_CLOSE, 0);
			//::PostMessage(hWnd, WM_CLOSE, 0, 0);
			bNoting    = false;
			funcSetStatusText(_T(""));
			break;

		default:
			::PostMessage(m_ChildFrameClient.GetActiveChildFrameWindow(), WM_COMMAND, dwCommand, 0);
			bNoting    = false;
			funcSetStatusText(_T(""));
			break;
		}

	} while (msg.message != WM_RBUTTONUP);

	ReleaseCapture();
	::FreeLibrary(hModule);
	if (data.bCursorOnSelectedText) {
		SetCursor(::LoadCursor(NULL, IDC_ARROW));	// カーソルを元に戻す
		if (bCancelRButtonUp)
			return TRUE;
		if (strSearchEngine.GetLength() > 0) {	// 右ボタンドラッグ実行
			funcSetStatusText(_T(""));
			m_SearchBar.SearchWebWithEngine(data.strSelectedTextLine, strSearchEngine);
			bNoting = false;
		}
	}

	/* マウスジェスチャーコマンド実行 */
	if (bNoting) {
		ptLast.SetPoint(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
		::ClientToScreen(msg.hwnd, &ptLast);

		funcSetStatusText(_T(""));

		CIniFileI	pr( GetConfigFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
		DWORD dwCommand = pr.GetValue(strMove);
		if (dwCommand) {
			::SendMessage(m_hWnd, WM_COMMAND, dwCommand, 0);
			bNoting = false;
		} else if (dwCommand == -1)
			return TRUE;
	}

	if ( bNoting && strMove.IsEmpty() ) {	// 右クリックメニューを出す
		::GetCursorPos(&ptLast);
		::ScreenToClient(data.hwnd, &ptLast);
		data.lParam = MAKELONG(ptLast.x, ptLast.y);
		if (m_ChildFrameClient.GetActiveChildFrameWindow() == data.hwnd || ::IsChild(m_ChildFrameClient.GetActiveChildFrameWindow(), data.hwnd)) {
			::PostMessage(m_ChildFrameClient.GetActiveChildFrameWindow(), WM_DEFAULTRBUTTONDOWN, data.wParam, data.lParam);
			::PostMessage(m_ChildFrameClient.GetActiveChildFrameWindow(), WM_DEFAULTRBUTTONUP, data.wParam, data.lParam);
		} else {
			//::PostMessage(data.hwnd, WM_RBUTTONDOWN, data.wParam, data.lParam);
			::PostMessage(data.hwnd, WM_RBUTTONUP, data.wParam, data.lParam);
		}
	}
	return !bNoting;
}

void CMainFrame::Impl::OnMouseGesture(HWND hWndChildFrame, HANDLE hMapForClose)
{
	MouseGestureData	data;
	CString sharedMemName;
	sharedMemName.Format(_T("%s0x%x"), MOUSEGESTUREDATASHAREDMEMNAME, hWndChildFrame);
	CSharedMemory sharedMem;
	sharedMem.Deserialize(data, sharedMemName);

	::PostMessage(hWndChildFrame, WM_CLOSEHANDLEFORSHAREDMEM, (WPARAM)hMapForClose, 0);

	_RButtonHook(data);
}



bool	CMainFrame::Impl::_IsRebarBandLocked()
{
	CReBarCtrl	  rebar(m_hWndToolBar);
	REBARBANDINFO rbbi = { sizeof (REBARBANDINFO) };
	rbbi.fMask	= RBBIM_STYLE;
	if ( !rebar.GetBandInfo(0, &rbbi) )
		return false;

	return (rbbi.fStyle & RBBS_NOGRIPPER) != 0;
}


void	CMainFrame::Impl::_FullScreen(bool bOn)
{
	_ShowBandsFullScreen(bOn);
	static bool m_bOldMaximized = false;	//\\+
	SetRedraw(FALSE);

	if (bOn) {							// remove caption
		// save prev visible
		CWindowPlacement	wndpl;
		GetWindowPlacement(&wndpl);

		CIniFileO	pr( g_szIniFileName, _T("Main") );
		wndpl.WriteProfile(pr, _T("frame."));

		m_bFullScreen = true;

		m_bOldMaximized 	= (wndpl.showCmd == SW_SHOWMAXIMIZED);
		ModifyStyle(WS_CAPTION, 0);

		SetMenu(NULL);
		ShowWindow(SW_HIDE);

//		m_mcToolBar.m_bVisible = true;

		ShowWindow(SW_MAXIMIZE);
		if (m_bOldMaximized == false) {
			SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
		
	} else {
		m_bFullScreen = false;

//		m_mcToolBar.m_bVisible = false;
		// restore prev visible
		if (CSkinOption::s_nMainFrameCaption)	//+++	キャプションの着けはづしがありの場合
			ModifyStyle(0, WS_CAPTION);

		if (m_bOldMaximized == false) {
			ShowWindow(SW_RESTORE);		//+++ サイズを戻す
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

void	CMainFrame::Impl::_ShowBandsFullScreen(bool bOn)
{
	static BOOL s_bOldVisibleStatusBar;
	static CSimpleMap<UINT, BOOL>	s_mapToolbarOldVisible;
	int nIndex = 0;
	if (bOn) { // remove caption
		// save prev visible
		int nToolbarPluginCount = CPluginManager::GetCount(PLT_TOOLBAR);
		s_mapToolbarOldVisible.RemoveAll();

		for (nIndex = 0; nIndex < _countof(DefaultReBarStyle); nIndex++) {
			s_mapToolbarOldVisible.Add( DefaultReBarStyle[nIndex].nID, MtlIsBandVisible( m_hWndToolBar, DefaultReBarStyle[nIndex].nID ) );
		}
		//for (nIndex = 0; nIndex < nToolbarPluginCount; nIndex++) {
		//	s_mapToolbarOldVisible.Add( IDC_PLUGIN_TOOLBAR + nIndex,
		//								MtlIsBandVisible( m_hWndToolBar, IDC_PLUGIN_TOOLBAR + nIndex ) );
		//}
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
		//for (nIndex = 0; nIndex < nToolbarPluginCount; nIndex++)
		//	MtlShowBand(m_hWndToolBar, IDC_PLUGIN_TOOLBAR + nIndex, FALSE);
	} else {
		SetRedraw(FALSE);
		for (nIndex = 0; nIndex < s_mapToolbarOldVisible.GetSize(); nIndex++)
			MtlShowBand( m_hWndToolBar, s_mapToolbarOldVisible.GetKeyAt(nIndex), s_mapToolbarOldVisible.GetValueAt(nIndex) );

		::ShowWindow(m_hWndStatusBar, s_bOldVisibleStatusBar ? SW_SHOWNOACTIVATE : SW_HIDE);
	}
	SetRedraw(TRUE);
}


/// トレイアイコンの設定.
void CMainFrame::Impl::_SetHideTrayIcon()
{
	//RtlSetMinProcWorkingSetSize();		//+++ (メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等)
	CString iconPath = _GetSkinDir() + _T("icon.ico");
	CIconHandle icon;
	if (::PathFileExists(iconPath))
		icon.LoadIcon((LPCTSTR)iconPath, 16, 16, LR_LOADFROMFILE);
	if (icon.IsNull())
		icon.LoadIcon(IDR_MAINFRAME);
	TrayMessage(m_hWnd, NIM_ADD, TM_TRAY, icon, DONUT_NAME);		//+++

	ShowWindow(SW_HIDE);
}

/// トレイ化の終了/トレイアイコンの削除.
void CMainFrame::Impl::_DeleteTrayIcon()
{
	TrayMessage(m_hWnd, NIM_DELETE, TM_TRAY, 0, NULL);
	//x m_bTrayFlag = false;
	if (IsWindowVisible() == false)
		ShowWindow(SW_RESTORE);

	//RtlSetMinProcWorkingSetSize();		//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
}


void	CMainFrame::Impl::_NewDonutInstance(const CString& strURL)
{
	std::vector<CString> vecUrl;
	PerseUrls(strURL, vecUrl);
	if (vecUrl.size() == 1) {
		OnDDEOpenFile(vecUrl.front());

	} else if (vecUrl.size() > 1) {
		for (auto it = vecUrl.cbegin(); it !=  vecUrl.cend(); ++it) {
			std::unique_ptr<NewChildFrameData>	item(new NewChildFrameData(m_ChildFrameClient));
			item->strURL	= *it;
			m_deqNewChildFrameData.push_back(std::move(item));
		}
		OnDDEOpenFile(m_deqNewChildFrameData[0]->strURL);
	}
}


void	CMainFrame::Impl::_NavigateChildFrame(HWND hWnd, LPCTSTR strURL, DWORD DLCtrl /*= -1*/, DWORD ExStyle /*= -1*/, DWORD AutoRefresh /*= 0*/)
{
	NavigateChildFrame	data;
	data.strURL	= strURL;
	data.dwDLCtrl	= DLCtrl;
	data.dwExStyle	= ExStyle;
	data.dwAutoRefresh	= AutoRefresh;
			
	std::wstringstream ss;
	boost::archive::text_woarchive ar(ss);
	ar << data;
	std::wstring serializedData = ss.str();

	COPYDATASTRUCT cds = { sizeof(cds) };
	cds.dwData	= kNavigateChildFrame;
	cds.lpData	= (LPVOID)serializedData.data();
	cds.cbData	= (serializedData.length() + 1) * sizeof(TCHAR);
	::SendMessage(hWnd, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
}













