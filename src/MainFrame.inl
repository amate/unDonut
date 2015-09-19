/**
*	@brief	CMainFrame::Impl �̎���
*/

///////////////////////////////////////////////////////////////////////////
// ���b�Z�[�W�}�b�v
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
	USER_MSG_WM_OPEN_WITHEXPROP			( OnOpenWithExProp	)	// SearchBar����
	USER_MSG_WM_HILIGHTSWITCHCHANGE		( OnHilightSwitchChange	)
	// ChildFrame ��
	USER_MSG_WM_SETPROXYTOCHLDFRAME		( OnSetProxyToChildFrame	)

	USER_MSG_WM_CLEANUPNEWPROCESSSHAREDMEMHANDLE( OnCleanUpNewProcessSharedMemHandle )
	USER_MSG_WM_SETDLCONFIGTOGLOBALCONFIG( OnSetDLConfigToGlobalConfig	)

	USER_MSG_WM_RELEASE_PROCESSMONITOR_PTR( OnReleaseProcessMonitorPtr	)

	USER_MSG_WM_REMOVECHILDPROCESSID( OnRemoveChildProcessId	)

	// for DownloadManager
	USER_MSG_WM_GETUNIQUENUMBERFORDLITEM( OnGetUniqueNumberForDLItem	)

	m_bCommandFromChildFrame = false;
	if (uMsg == WM_COMMAND_FROM_CHILDFRAME) {		// Loop�h�~
		uMsg = WM_COMMAND;
		m_bCommandFromChildFrame	= true;
	}
		
	// �t�@�C��
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

	// �ҏW
	COMMAND_ID_HANDLER_EX( ID_EDIT_CUT 				, OnEditOperation	)
	COMMAND_ID_HANDLER_EX( ID_EDIT_COPY 			, OnEditOperation	)
	COMMAND_ID_HANDLER_EX( ID_EDIT_PASTE 			, OnEditOperation	)
	COMMAND_ID_HANDLER_EX( ID_EDIT_SELECT_ALL		, OnEditOperation	)
	COMMAND_ID_HANDLER_EX( ID_SEARCHBAR_SEL_DOWN	, OnSearchBarCommand )
	COMMAND_ID_HANDLER_EX( ID_SEARCHBAR_SEL_UP		, OnSearchBarCommand )
	COMMAND_ID_HANDLER_EX( ID_SEARCHBAR_HILIGHT		, OnSearchBarCommand )
	MSG_WM_USER_FIND_KEYWORD( OnFindKeyWord    )	// from SearchBar
	MSG_WM_USER_HILIGHT 	( OnHilight 	   )

	// ���C�ɓ���
	COMMAND_ID_HANDLER_EX( ID_FAVORITE_ADD			, OnFavoriteAdd 		)
	COMMAND_ID_HANDLER_EX( ID_FAVORITE_ORGANIZE 	, OnFavoriteOrganize	)
	COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_ADD 	, OnFavoriteGroupCommand	)
	COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_SAVE 	, OnFavoriteGroupCommand	)
	COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_ORGANIZE , OnFavoriteGroupCommand	)

	// �\��
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
	CHAIN_COMMANDS_TO_EXPLORERBAR( m_ExplorerBar )	// Explorer �o�[ ��
	COMMAND_ID_HANDLER_EX( ID_SETFOCUS_ADDRESSBAR		, OnSetFocusToBar	)
	COMMAND_ID_HANDLER_EX( ID_SETFOCUS_SEARCHBAR		, OnSetFocusToBar	)
	COMMAND_ID_HANDLER_EX( ID_SETFOCUS_SEARCHBAR_ENGINE	, OnSetFocusToBar	)
	COMMAND_ID_HANDLER_EX( ID_VIEW_ADDBAR_DROPDOWN		, OnSetFocusToBar	)
	COMMAND_ID_HANDLER_EX( ID_VIEW_FULLSCREEN		, OnViewFullScreen		)

	// �c�[��
	COMMAND_ID_HANDLER_EX( ID_DOUBLE_CLOSE			, OnDoubleClose		)
	COMMAND_ID_HANDLER_EX( ID_POPUP_CLOSE			, OnPopupClose		)
	COMMAND_ID_HANDLER_EX( ID_TITLE_CLOSE			, OnTitleClose		)
	COMMAND_ID_HANDLER_EX( ID_GET_OUT				, OnGetOut			)
	COMMAND_ID_HANDLER_EX( ID_VIEW_OPTION			, OnViewOption		)
	COMMAND_ID_HANDLER_EX( ID_VIEW_OPTION_DONUT 	, OnViewOptionDonut	)

	// �E�B���h�E
	COMMAND_ID_HANDLER_EX( ID_WINDOW_CLOSE_ALL		, OnTabClose		)
	COMMAND_ID_HANDLER_EX( ID_WINDOW_CLOSE_EXCEPT	, OnTabClose		)
	COMMAND_ID_HANDLER_EX( ID_LEFT_CLOSE			, OnTabClose		)
	COMMAND_ID_HANDLER_EX( ID_RIGHT_CLOSE			, OnTabClose		)
	COMMAND_ID_HANDLER_EX( ID_TAB_LEFT				, OnTabSwitch 		)
	COMMAND_ID_HANDLER_EX( ID_TAB_RIGHT 			, OnTabSwitch		)
	COMMAND_ID_HANDLER_EX( ID_VIEW_STOP_ALL			, OnOperateCommandToAllTab )
	COMMAND_ID_HANDLER_EX( ID_VIEW_REFRESH_ALL		, OnOperateCommandToAllTab )
	COMMAND_ID_HANDLER_EX( ID_WINDOW_REFRESH_EXCEPT	, OnOperateCommandToAllTab )

	// �w���v
	COMMAND_ID_HANDLER_EX( ID_OPEN_EXE_DIR	, OnOpenDonutExeFolder	)
	COMMAND_ID_HANDLER_EX( ID_JUMP_WEBSITE	, OnAuthorWebSite		)

	// Special Command
	COMMAND_ID_HANDLER_EX( ID_RECENT_DOCUMENT	, OnMenuRecentLast		)
	COMMAND_ID_HANDLER_EX( ID_SPECIAL_REFRESH_SEARCHENGIN, OnSpecialRefreshSearchEngine	)
	COMMAND_ID_HANDLER_EX(ID_MAINFRAME_MINIMIZE, OnMainFrameMiniMaximize)
	COMMAND_ID_HANDLER_EX(ID_MAINFRAME_MAXIMIZE, OnMainFrameMiniMaximize)
	

	USER_MSG_WM_SHOW_TOOLBARMENU( OnShowToolBarMenu )
	USER_MSG_WM_SHOW_BAND_TEXT_CHANGE( OnShowBandTextChange	)

	// CChildFrame����
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

	REFLECT_CHEVRONPUSHED_NOTIFICATION()	// CFrameWindowImpl ���O�ɒu���K�v������
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
// UI �}�b�v

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
	// �A�h���X�o�[���T�[�`�o�[�Ƀt�H�[�J�X������ or OLE�R�}���h���g�p�\���ǂ���
	CEdit editAddress = m_AddressBar.GetEditCtrl();
	CEdit editSearch  = m_SearchBar.GetEditCtrl();
	bool  bFocus	  = (::GetFocus() == editAddress || ::GetFocus() == editSearch);
	bool  bCut		  = false, bCopy = false, bPaste = false, bSelectAll = false;
	if (bActiveChild) {
	//+++ ����QUeryStatusWB���o�O���āA���s���ĂĂ��S�~��Ԃ��Ă���...�ŁA���܂��ܕ\������邱�Ƃ����������̂���.
	//+++ ���A���������K�����s����̂������Ȃ�... ���A�s�ւȂ̂ŁA�b��Ώ�
		bCut = true, bCopy = true, bPaste = true, bSelectAll = true;
	}
	if (bFocus)
		bCut = true, bCopy = true, bPaste = true, bSelectAll = true;
	UPDATE_COMMAND_UI_ENABLE_IF ( ID_EDIT_CUT		, bCut			)
	UPDATE_COMMAND_UI_ENABLE_IF ( ID_EDIT_COPY		, bCopy			)
	UPDATE_COMMAND_UI_ENABLE_IF ( ID_EDIT_PASTE 	, bPaste		)
	UPDATE_COMMAND_UI_ENABLE_IF ( ID_EDIT_SELECT_ALL, bSelectAll	)
	// �����܂�

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
	// UPDATE_COMMAND_UI_SETTEXT( ID_DEFAULT_PANE, _T("���f�B" ))
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
	UPDATE_COMMAND_UI_ENABLE_IF 	( ID_SEARCH_PAGE		, bActiveChild )	//�����o�[�̃y�[�W�������{�^��
	UPDATE_COMMAND_UI_ENABLE_IF 	( ID_SEARCHBAR_SEL_UP	, bActiveChild )
	UPDATE_COMMAND_UI_ENABLE_IF 	( ID_SEARCHBAR_SEL_DOWN , bActiveChild )
	UPDATE_COMMAND_UI_ENABLE_IF 	( ID_SEARCHBAR_HILIGHT	, bActiveChild )
END_UPDATE_COMMAND_UI_MAP()



CMainFrame::Impl::Impl() : 
	CDDEMessageHandler<CMainFrame::Impl>(_T("Donut")),
	m_ExplorerBar(m_SplitterWindow),
	m_hWndRestoreFocus(NULL),
	m_bFullScreen(false),
	m_donutScriptHost(this),
	m_bInitProcessFinished(false)
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
	/* default.dfg������΂�������g�� */
	CString	strFile = Misc::GetExeDirectory() + _T("Default.dfg");
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
		MessageBox(_T("RestoreAllTab�ŃG���[���������܂���!"));
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
	/* ���݊J���Ă���^�u�̏����W�߂� */
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

	/* TabList.donutTabList �ɕۑ����� */
	if (tabList.Save(GetConfigFilePath(_T("TabList.donutTabList"))) == false) {
		MessageBox(_T("SaveAllTab�ŃG���[����!"));
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
		DWORD dwExProp = 0xAAAAAA;		//+++ �����l�ύX
		DWORD dwExProp2= 0x8;			//+++ �g���v���p�e�B�𑝐�.
		if (CExProperty::CheckExPropertyFlag(dwExProp, dwExProp2, strFileOrURL)) {
			CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, dwExProp, dwExProp2);
			DLCtrl	= ExProp.GetDLControlFlags();
			ExStyle	= ExProp.GetExtendedStyleFlags();
			AutoRefresh = ExProp.GetAutoRefreshFlag();
		}
		if (MTL::ParseInternetShortcutFile(strFileOrURL) == false) {
			TRACEIN(_T("UseOpenFile(), .url�t�@�C������URL���擾�ł��܂���ł��� : %s"), strFileOrURL);
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

	/* openFlag �� D_OPENFILE_NOCREATE �Ȃ�A�N�e�B�u�ȃy�[�W���ړ����� */
	if ( hWndActive != NULL && _check_flag(D_OPENFILE_NOCREATE, openFlags) ) {
		if (bJavascript) {
			// Javascript�����s����
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

		/* �V�KChildFrame�쐬 */
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

/// ����URL���J��
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
		if (bLink)	// kNowDonutInstance����Ȃ�t�]
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
		// �c�[���o�[�̏ꍇ
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

	// �y�[�W�������o�[
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
	// �����o�[
	BOOL ptFlag = m_SearchBar.PreTranslateMessage(pMsg);
	if (ptFlag == _MTL_TRANSLATE_HANDLE) {
		return TRUE;
	} else if (ptFlag == _MTL_TRANSLATE_WANT) {
		return FALSE;
	}

	// �}�E�X�W�F�X�`���[
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

	if ((WM_KEYFIRST <= pMsg->message || pMsg->message <= WM_KEYLAST) && pMsg->hwnd == m_CommandBar.GetHWND()) {
		HWND hWndActiveChildFrame = m_ChildFrameClient.GetActiveChildFrameWindow();
		if (hWndActiveChildFrame) {
			*static_cast<MSG*>(m_sharedMemKeyMessage.GetPointer()) = *pMsg;
			bool bProcess = ::SendMessage(hWndActiveChildFrame, WM_FORWARDMSG, 0, 0) != 0;
			if (bProcess)
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
	} else {	// �����̃^�u���i�r�Q�[�g����
		dwOpen |= D_OPENFILE_NOCREATE;
		if (CStartUpOption::s_bActivateOnExternalOpen == false)
			dwOpen |= D_OPENFILE_NOSETFOCUS;
	}

	if (m_bInitProcessFinished == false && m_deqNewChildFrameData.size() > 0) {
		auto pNewData = std::make_unique<NewChildFrameData>(m_ChildFrameClient.m_hWnd);

		CString strFileOrURL = strFileName;
		if (MtlIsExt(strFileOrURL, _T(".url")) && ::PathFileExists(strFileOrURL)) {
			DWORD dwExProp = 0xAAAAAA;		//+++ �����l�ύX
			DWORD dwExProp2 = 0x8;			//+++ �g���v���p�e�B�𑝐�.
			if (CExProperty::CheckExPropertyFlag(dwExProp, dwExProp2, strFileOrURL)) {
				CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, dwExProp, dwExProp2);
				pNewData->dwDLCtrl = ExProp.GetDLControlFlags();
				pNewData->dwExStyle = ExProp.GetExtendedStyleFlags();
				pNewData->dwAutoRefresh = ExProp.GetAutoRefreshFlag();
			}
			ATLVERIFY(MTL::ParseInternetShortcutFile(strFileOrURL));
		}
		pNewData->strURL = strFileOrURL;
		pNewData->bActive = (dwOpen & D_OPENFILE_ACTIVATE) != 0;
		m_deqNewChildFrameData.push_back(std::move(pNewData));

	} else {
		UserOpenFile(strFileName, dwOpen);
	}
	if (CStartUpOption::s_bActivateOnExternalOpen) {
		_DeleteTrayIcon();							//+++ �g���C��Ԃ������畜��.
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

	/* GlobalConfig�쐬 */
	CreateGlobalConfig(&m_GlobalConfigManageData);
	SetGlobalConfig(m_GlobalConfigManageData.pGlobalConfig);
	OnSetDLConfigToGlobalConfig();
	m_StatusBar.GetProxyComboBox().SetGlobalConfig(m_GlobalConfigManageData.pGlobalConfig);

	//TIMERSTART();
	/* �e��o���h�E�B���h�E�쐬 */
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
	//TIMERSTOP(_T("�e��o���h�쐬"));
	
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

	/* UI �X�V��ݒ� */
	m_ChildFrameUIState.SetMainFrameHWND(m_hWnd);
	CmdUIAddToolBar(hWndToolBar);					// set up UI
	CmdUIAddToolBar(m_SearchBar.GetHWndToolBar());	// set up UI

	CFaviconManager::Init(hWndTabBar);

	m_DownloadManager.SetParent(m_hWnd);

	// �ŋߕ����^�u�̃��j���[�̐ݒ��I�v�V�����̐ݒ���s��
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

	CString strSharedMemNameKeyMessage;
	strSharedMemNameKeyMessage.Format(_T("DonutKeyMessageSharedMemName%#x"), m_hWnd);
	m_sharedMemKeyMessage.CreateSharedMemory(strSharedMemNameKeyMessage, sizeof(MSG));

	if (CDLControlOption::s_bAutoClosePageMoveConfirmDialog)
		ATLVERIFY(m_pageMoveConfirmAutoClose.WatchStart());

	TIMERSTOP(L"���C���t���[����OnCreate�ɂ�����������");
	return 0;
}

/// �t�@�C�����_�E�����[�h�������I�����Ă����̂��Ԃ�
bool	CMainFrame::Impl::_ConfirmCloseForFileDownloading()
{
	int nDLCount = m_DownloadManager.GetDownloadingCount();
	if (nDLCount > 0) {
		CString msg;
		msg.Format(_T("�_�E�����[�h���̃A�C�e���� %d����܂��B\n�I�����܂����H"), nDLCount);
		if (MessageBox(msg, _T("�m�F"), MB_ICONQUESTION | MB_OKCANCEL | MB_DEFBUTTON2) == IDCANCEL)
			return false;
	}
	return true;
}

/// ���C���t���[���̂w�{�^�����������Ƃ�
void	CMainFrame::Impl::OnClose()
{
	if (_ConfirmCloseForFileDownloading() == false)
		return ;

	std::set<DWORD>	setChildProcessId;
	setChildProcessId.insert(GetCurrentProcessId());
	for (auto id : MultiThreadManager::g_vecChildProcessProcessThreadId)
		setChildProcessId.insert(id.dwProcessId);
	if ( CDonutConfirmOption::OnDonutExit(m_hWnd, setChildProcessId) == false ) {
		if (IsWindowVisible() == FALSE) {
			_SetHideTrayIcon();
		}
		return;
	}

	SetMsgHandled(FALSE);
}


/// �I�����ɐݒ肳��Ă���΃L���b�V���ƃN�b�L�[���폜����
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
			//x bResult 			   = TRUE;						//+++ �������ǖ��g�p�̂悤.
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

/// �������폜����
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
	TRACEIN(_T("���C���t���[���̏I����..."));
	SetMsgHandled(FALSE);

	_PrivateTerm();		// �ݒ�̕ۑ�

	KillTimer(kAutoBackupTimerId);

	m_sharedMemKeyMessage.CloseHandle();

	m_pageMoveConfirmAutoClose.WatchStop();

	RevokeDragDrop();

	_SaveBandPosition();

	CIniFileO	pr(g_szIniFileName, _T("Main"));
	MtlWriteProfileMainFrameState(pr, m_hWnd);

	m_ReBar.UnsubclassWindow();

	m_GlobalConfigManageData.pGlobalConfig->bMainFrameClosing = true;

	if (CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_LATEST) {
		SaveAllTab();	// ���ݕ\�����̃^�u��ۑ�����
	} else {
#if 0
		vector<HWND> vechWnd;
		m_TabBar.ForEachWindow([&](HWND hWndChildFrame) {	// �ŋߕ����^�u�ɒǉ�
			vechWnd.push_back(hWndChildFrame);			
		});
		for (auto it = vechWnd.cbegin(); it != vechWnd.cend(); ++it)
			::PostMessage(*it, WM_CLOSE, 0, 0);

		// ���������ׂĕۑ������܂ő҂�
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
		// ���ݕ\�����̃^�u���ŋߕ����^�u�ɒǉ�����
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

	// ���b�Z�[�W���[�v���烁�b�Z�[�W�t�B���^�ƃA�C�h���n���h�����폜
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->RemoveMessageFilter(this);
    pLoop->RemoveIdleHandler(this);

	_DeleteCacheAndCookie();
	_DeleteHistory();

	CStartUpOption::EndFinish();
}

/// Windows���I���O�ɌĂ΂��@�t�@�C�����_�E�����[�h���Ȃ璆�~���邩�ǂ����m�F����
BOOL	CMainFrame::Impl::OnQueryEndSession(UINT nSource, UINT uLogOff)
{
	if (_ConfirmCloseForFileDownloading() == false)
		return FALSE;

	return TRUE;
}

void	CMainFrame::Impl::OnEndSession(BOOL bEnding, UINT uLogOff)
{
	if (bEnding) {
		OnDestroy();	// �ݒ�̕ۑ�
	}
}

void	CMainFrame::Impl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == kAutoBackupTimerId) {
		TIMERSTART();
		SaveAllTab();
		TIMERSTOP(L"���TabList.donutTabList�Z�[�u");
	}
}

void	CMainFrame::Impl::_initRebar()
{
	DWORD	dwRebarStyle = ATL_SIMPLE_REBAR_STYLE | CCS_NOPARENTALIGN | CCS_NODIVIDER | RBS_DBLCLKTOGGLE;
	{	// TODO: ��Őݒ�N���X�Ɉڂ�
		CIniFileI	pr( g_szIniFileName, _T("ReBar") );
		DWORD		dwNoBoader 		= pr.GetValue( _T("NoBoader") );
		if (dwNoBoader)
			dwRebarStyle &= ~RBS_BANDBORDERS;	// �{�[�_�[������
	}

	CreateSimpleReBar(dwRebarStyle);
	m_ReBar.SubclassWindow(m_hWndToolBar);
}

HWND	CMainFrame::Impl::_initCommandBar()
{
	SetMenu(NULL);		// remove menu

	HWND	hWndCmdBar = m_CommandBar.Create(m_ReBar);
	ATLASSERT( ::IsWindow(hWndCmdBar) );

	m_CommandBar.SetRecentClosedTabList(&m_RecentClosedTabList);	// �ŋߕ����^�u �|�b�v�A�b�v���j���[�p

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
		if (rbi.fStyle & RBBS_HIDDEN)
			::ShowWindow(rbi.hWnd, SW_HIDE);
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

	// TODO: ������ւ���Ȃ�Ƃ�����
	enum {	//+++ ID�̓f�t�H���g�̖��O�ɂȂ��Ă��邪�A���������ꍇ�ɂ�₱�����̂ŁA���̈�A���̈�Ƃ��Ĉ���.
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

	//+++ �ʒu�����̏C��.
	if (g_bSwapProxy == FALSE) {
		g_dwProgressPainWidth = dwSzPain1;					//+++ �蔲���Ńv���O���X�y�C���̉������O���[�o���ϐ��ɍT����.
		m_StatusBar.SetPaneWidth(ID_PAIN_1, 0);			//dwSzPain1); //�N�����̓y�C���T�C�Y��0
		if (m_StatusBar.GetProxyComboBox().IsUseIE())
			dwSzPain2 = 0;									// IE��Proxy���g���ꍇ��Proxy�y�C���T�C�Y��0��
		m_StatusBar.SetPaneWidth(ID_PAIN_2, dwSzPain2);
	} else {	// �������Ă���Ƃ�.
		g_dwProgressPainWidth = dwSzPain2;					//+++ �蔲���Ńv���O���X�y�C���̉������O���[�o���ϐ��ɍT����.
		m_StatusBar.SetPaneWidth(ID_PAIN_2, dwSzPain2);	//+++ �������Ă�Ƃ��́A�ŏ�����T�C�Y�m��.
		if (m_StatusBar.GetProxyComboBox().IsUseIE())
			dwSzPain1 = 0;									// IE��Proxy���g���ꍇ��Proxy�y�C���T�C�Y��0��
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
	/* �t�H���g */
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

		CFontHandle 	font;	//\\ SetFont�������ł���悤��
		MTLVERIFY( font.CreateFontIndirect(&lf) );
		if (font.m_hFont) 
			SetFont(font);
	} else {
		WTL::CLogFont	lf;
		lf.SetMenuFont();
		m_CommandBar.SetFont(lf.CreateFontIndirect());
	}

	/* �X�L�� */
	//initCurrentIcon();											//+++ �A�C�R��
	m_ReBar.RefreshSkinState(); 								//ReBar
	m_TabBar.ReloadSkin();										//�^�u
	m_ToolBar.ReloadSkin(); 									//�c�[���o�[
	m_AddressBar.ReloadSkin(CSkinOption::s_nComboStyle);		//�A�h���X�o�[
	m_SearchBar.ReloadSkin(CSkinOption::s_nComboStyle); 		//�����o�[
	//m_LinkBar.InvalidateRect(NULL, TRUE);						//�����N�o�[
//	m_ExplorerBar.ReloadSkin(); 								//�G�N�X�v���[���o�[
//	m_ExplorerBar.m_PanelBar.ReloadSkin();						//�p�l���o�[
//	m_ExplorerBar.m_PluginBar.ReloadSkin(); 					//�v���O�C���o�[
	
	m_StatusBar.ReloadSkin( CSkinOption::s_nStatusStyle		//�X�e�[�^�X�o�[
							 , CSkinOption::s_nStatusTextColor
							 , CSkinOption::s_nStatusBackColor);
}

void	CMainFrame::Impl::_initSysMenu()
{
	CMenuHandle SysMenu = GetSystemMenu(FALSE);

	SysMenu.InsertMenu(0, MF_BYPOSITION | MF_STRING, ID_VIEW_COMMANDBAR, _T("���j���[��\��(&V)"));
	SysMenu.InsertMenu(1, MF_BYPOSITION | MF_SEPARATOR);
	SysMenu.InsertMenu(2, MF_BYPOSITION | MF_STRING, ID_VIEW_PROCESSMONITOR, _T("�v���Z�X ���j�^�[(&P)"));
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


// ���o�[�ƃr���[�̊Ԃ̉���������
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

/// �t�H�[�J�X�𕜌�����
void	CMainFrame::Impl::OnActivate(UINT nState, BOOL bMinimized, CWindow wndOther)
{
	if (nState == WA_INACTIVE) {
		m_hWndRestoreFocus = ::GetFocus();
	} else if (m_hWndRestoreFocus) {
		::SetFocus(m_hWndRestoreFocus);
	}
}

/// �^�C�g���o�[�̉E�N���b�N���j���[�̃R�}���h
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

	//+++ �ŏ����{�^�������������ɁA�^�X�N�g���C�ɓ���悤�ɂ��Ă݂�.
	case SC_MINIMIZE:
		if ((CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MINBTN2TRAY)	//+++ �ŏ����{�^���Ń^�X�N�g���C�ɓ����ݒ�̂Ƃ��A
			&& (pt.x || pt.y)											//+++ x,y��0,0�Ȃ�^�X�N�o�[�ŃN���b�N�����ꍇ���낤�ŁA�g���C�ɂ��ꂸ�A�ŏ����������Ă݂�.
		) {
			OnGetOut(0,0,0);
			SetMsgHandled(TRUE);
			break;
		}
		SetMsgHandled(FALSE);
		break;

	case SC_CLOSE:
		if ((CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_CLOSEBTN2TRAY)	//+++ �ŏ����{�^���Ń^�X�N�g���C�ɓ����ݒ�̂Ƃ��A
			&& (pt.x || pt.y)											//+++ x,y��0,0�Ȃ�^�X�N�o�[�ŃN���b�N�����ꍇ���낤�ŁA�g���C�ɂ��ꂸ�A�ŏ����������Ă݂�.
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

/// ���j���[�̉E�[�ɃV���[�g�J�b�g�L�[��\������悤�ɐݒ肷��
void CMainFrame::Impl::OnInitMenuPopup(CMenuHandle menuPopup, UINT nIndex, BOOL bSysMenu)
{
	SetMsgHandled(FALSE);

	if (bSysMenu)	// �V�X�e�����j���[�́A�������Ȃ�
		return ;
	
	//if (   hMenuPopup == m_FavoriteMenu.GetMenu().m_hMenu 
	//	|| hMenuPopup == m_FavGroupMenu.GetMenu().m_hMenu 
	//	|| hMenuPopup == m_styleSheetMenu.GetMenu().m_hMenu
	//	|| m_FavoriteMenu.IsSubMenu(hMenuPopup) )
	//	return 0;

	//CMenuHandle 	menu = hMenuPopup;
	//CAccelerManager accel(m_hAccel);

	// ��Ԗڂ̃��j���[�������Ȃ����͂���������ĕҏW���Ȃ�
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

	case kAddToSearchBoxUnique:
		{
			CString str = (LPCWSTR)pCopyDataStruct->lpData;
			m_SearchBar.AddToSearchBoxUnique(str);
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
				if (CMouseOption::s_nDragDropCommandID == ID_SEARCH_DIRECT) {				// ������
					m_SearchBar.SearchWeb(str);

				} else if (CMouseOption::s_nDragDropCommandID == ID_OPENLINK_DIRECT) {	// �����N���J��
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

/// �^�X�N�g���C����̒ʒm���b�Z�[�W
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

				// �|�b�v�A�b�v���j���[���J��.
				POINT 	pt;
				::GetCursorPos(&pt);
				HRESULT hr = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON| TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
				if (hr == 57666/*���A*/) {
					//_DeleteTrayIcon();
					OnGetOut(0, 0, NULL);
					return 0;
				}
				if (hr == 57665/*�I��*/) {
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

	// �E�N���b�N����Ă����� - �X�N���[���̂���r���[�ゾ�ƃr���[���X�N���[�������o�O
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

/// �R�}���h���C�������s����
void	CMainFrame::Impl::OnInitProcessFinished(bool bHome)
{
	if (m_bInitProcessFinished == false) {
		extern CString g_commandline;	// Donut.cpp�ɂ���
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
			// ���Ƃ�OnTabCreate���^�u������Ă����
		} else {
			bool bSaveFlag = CMainOption::s_bExternalNewTabActive;
			CMainOption::s_bExternalNewTabActive = true;
			_NewDonutInstance(g_commandline);
			CMainOption::s_bExternalNewTabActive = bSaveFlag;
		}
		// �^�u���X�g�̒���ۑ����J�n
		if (CMainOption::s_dwMainExtendedStyle & MAIN_EX_BACKUP)
			SetTimer(kAutoBackupTimerId, CMainOption::s_dwBackUpTime * 60000);

		m_bInitProcessFinished = true;
	}
}


void	CMainFrame::Impl::OnBrowserTitleChange(HWND hWndChildFrame, LPCTSTR strTitle)
{
	if (::wcsncmp(strTitle, L"�|�b�v�A�b�v���u���b�N���܂����B: ", 18) != 0)
		m_TabBar.SetTitle(hWndChildFrame, strTitle);

	// �L���v�V������ύX
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

/// �����o�[����Ă΂�� ���������Ƃ��ɔ��ł���֐�
LRESULT CMainFrame::Impl::OnOpenWithExProp(_EXPROP_ARGS *pArgs)
{
	ATLASSERT(pArgs);

	HWND hWndActive = m_ChildFrameClient.GetActiveChildFrameWindow();
	//CChildFrame *pActiveChild = GetActiveChildFrame();
	//if( pActiveChild)
	//	pActiveChild->SaveSearchWordflg(false); //\\ �����o�[�Ō��������Ƃ��A�N�e�B�u�ȃ^�u�̌����������ۑ����Ȃ��悤�ɂ���

	NewChildFrameData	data(m_ChildFrameClient);
	data.strURL		= pArgs->strUrl;
	DWORD dwExProp = 0xAAAAAA;		//+++ �����l�ύX
	DWORD dwExProp2= 0x8;			//+++ �g���v���p�e�B�𑝐�.
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
		// �����̃^�u���i�r�Q�[�g
		COPYDATASTRUCT cds;
		cds.dwData	= kSetSearchText;
		cds.lpData	= static_cast<LPVOID>(data.searchWord.GetBuffer(0));
		cds.cbData	= (data.searchWord.GetLength() + 1) * sizeof(WCHAR);
		::SendMessage(hWndActive, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
		_NavigateChildFrame(hWndActive, data.strURL, data.dwDLCtrl, data.dwExStyle, data.dwAutoRefresh);

	} else {
		// �����o�[�̕����񂪏����錏�Ɉꉞ�̑Ώ�
		m_GlobalConfigManageData.pGlobalConfig->bSaveSearchWord = false;

		// �V�K�^�u���쐬����
		CString str = pArgs->strSearchWord;
		//data.funcCallAfterCreated = [str, bOldSaveFlag](CChildFrame* pChild) {
		//	CSearchBarOption::s_bSaveSearchWord = bOldSaveFlag;
	 // 		//+++ �q���Ɍ����ݒ�𔽉f (�֐���)
		//	if (CSearchBarOption::s_bAutoHilight)
		//		pChild->SetSearchWordAutoHilight(str, true);
		//};
		data.bActive = true;
		CChildFrame::AsyncCreate(data);
	}

	return 0;
}


/// ChildFrame�Ƀv���N�V�ؑւ�ʒm����
void	CMainFrame::Impl::OnSetProxyToChildFrame()
{
	if (CMainOption::s_BrowserOperatingMode == BROWSEROPERATINGMODE::kMultiProcessMode) {
		m_TabBar.ForEachWindow([](HWND hWnd) {
			::PostMessage(hWnd, WM_SETPROXYTOCHLDFRAME, 0, 0);
		});
	}
}

/// ���݂̃_�E�����[�h�}�l�[�W���[�̐ݒ��GlobalConfig�ɐݒ肷��
void	CMainFrame::Impl::OnSetDLConfigToGlobalConfig()
{
	m_GlobalConfigManageData.pGlobalConfig->bShowDLManagerOnDL	= CDLOptions::bShowWindowOnDL;
	::wcscpy_s(m_GlobalConfigManageData.pGlobalConfig->strDefaultDLFolder, CDLOptions::strDLFolderPath);
	::wcscpy_s(m_GlobalConfigManageData.pGlobalConfig->strImageDLFolder, CDLOptions::strImgDLFolderPath);
	m_GlobalConfigManageData.pGlobalConfig->dwDLImageExStyle	= CDLOptions::dwImgExStyle;
}

/// ChildProcessThreadId���폜����
void	CMainFrame::Impl::OnRemoveChildProcessId(DWORD dwProcessId)
{
	for (auto it = MultiThreadManager::g_vecChildProcessProcessThreadId.begin(); it != MultiThreadManager::g_vecChildProcessProcessThreadId.end(); ++it) {
		if (it->dwProcessId == dwProcessId) {
			MultiThreadManager::g_vecChildProcessProcessThreadId.erase(it);
			break;
		}
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

	case ID_FILE_NEW_HOME:	// TODO: ��ŕς���
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
	if (ID_FILE_MRU_FIRST <= nID && nID <= ID_FILE_MRU_LAST)			// ���͈�ID����V�͈�ID�֕ϊ�
		nID = ID_RECENTDOCUMENT_FIRST + (nID - ID_FILE_MRU_FIRST);

	ChildFrameDataOnClose*	pdata = nullptr;
	if (m_RecentClosedTabList.GetFromList(nID, &pdata) == FALSE)
		return ;
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

/// �����o�[����̃y�[�W������
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
				CTabBarOption::WriteProfile();	// �ݒ�̕ۑ�
				m_TabBar.ReloadSkin();
			}
			break;

		case ID_VIEW_TOOLBAR_CUST:	m_ToolBar.Customize();	break;

		case ID_VIEW_TOOLBAR_LOCK:
			{
				REBARBANDINFO rbbi = { sizeof (REBARBANDINFO) };
				rbbi.fMask	= RBBIM_STYLE;
				if ( !m_ReBar.GetBandInfo(0, &rbbi) )
					return ;	// Band������Ȃ�

				m_ReBar.LockBands( !( (rbbi.fStyle & RBBS_NOGRIPPER) != 0) );
			}
			break;

		case ID_VIEW_STATUS_BAR:
			{
				BOOL bNew = !::IsWindowVisible(m_hWndStatusBar);
				//UpdateTitleBar(_T(""), 0);		//+++		status�I�t����I���ɂ����Ƃ��ɁA�^�C�g���o�[�ɏo���Ă����X�e�[�^�X���������������.
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

/// �������O�C���ҏW�_�C�A���O��\������
void	CMainFrame::Impl::OnAutoLoginEdit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	LoginInfomation info;
	HWND activeChildFrame = m_ChildFrameClient.GetActiveChildFrameWindow();
	if (activeChildFrame) {
		if (GetKeyState(VK_CONTROL) < 0) {
			::PostMessage(activeChildFrame, WM_COMMAND, ID_DOAUTOLOGIN, 0);
			return;
		}

		HANDLE hMapForClose = (HANDLE)::SendMessage(activeChildFrame, WM_GETLOGININFOMATION, NULL, 0);
		ATLASSERT( hMapForClose );
		CString sharedMemName;
		sharedMemName.Format(_T("%s%#x"), GETLOGININFOMATIONSHAREDMEMNAME, activeChildFrame);
		CSharedMemory sharedMem;
		sharedMem.Deserialize(info, sharedMemName);
		::SendMessage(activeChildFrame, WM_GETLOGININFOMATION, (WPARAM)hMapForClose, 0);
	}
	auto funcRefresh = [activeChildFrame]() {
		::PostMessage(activeChildFrame, WM_COMMAND, ID_VIEW_REFRESH, 0);
	};
	CLoginInfoEditDialog dlg(info);
	dlg.SetAutoLoginfunc(funcRefresh);
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

/// ���C�ɓ���O���[�v�R�}���h
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


/// �^�X�N�g���C�ɑޔ�
void	CMainFrame::Impl::OnGetOut(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	static int nShow = SW_SHOWNORMAL;

	if ( IsWindowVisible() ) {
		//+++ ����:����Ԃ̂Ƃ��̓g���C��
		WINDOWPLACEMENT wp = { sizeof (WINDOWPLACEMENT) };
		GetWindowPlacement(&wp);
		nShow = wp.showCmd;
#if 0
		m_bTray = true; 		//+++
		WINDOWPLACEMENT wp = { sizeof (WINDOWPLACEMENT) };
		::GetWindowPlacement(m_hWnd, &wp);
		nShow		  = wp.showCmd;
	  #if 1 //+++ �t���X�N���[���ȊO�Ȃ獡���ő傩�ǂ������T����
		if (m_bFullScreen == 0)
			m_bOldMaximized = (nShow == SW_MAXIMIZE);
	  #endif
#endif
		_SetHideTrayIcon();	//+++ �g���C�A�C�R����
		//Sleep(100); 		// UDT TODO
	} else {				// just db F9 press , come here :p
		//m_bTray      = false;	//+++
		//m_bMinimized = 0;
		//x ShowWindow( SW_SHOW /*nShow*/ );

		_DeleteTrayIcon();	//+++ �g���C�A�C�R���폜

		ShowWindow(nShow);
	}
}


/// IE�̃I�v�V������\������
void	CMainFrame::Impl::OnViewOption(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_ChildFrameClient.GetActiveChildFrameWindow() == NULL)
		MtlShowInternetOptions();
	else
		SetMsgHandled(FALSE);	// ChildFrame��
}

/// Donut�̃I�v�V������\��
void	CMainFrame::Impl::OnViewOptionDonut(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	BOOL	bSkinChange = FALSE;

	CMenu	menu		= CExMenuManager::LoadFullMenu();

	CMainPropertyPage				pageMain(m_hWnd);
	CMainPropertyPage2				pageMain2(m_hWnd, m_RecentClosedTabList);
	CDLControlPropertyPage			pageDLC(m_hWnd);
	CMDITabPropertyPage 			pageTab(&m_TabBar, menu.m_hMenu);
	CDonutAddressBarPropertyPage	pageAddress(m_AddressBar, m_SearchBar);
	CStartUpFinishPropertyPage		pageStartUpFinish;
	CProxyPropertyPage				pageProxy;
	CKeyBoardPropertyPage			pageKeyBoard(m_hAccel, menu.m_hMenu, m_hWnd);
	CToolBarPropertyPage			pageToolBar(menu.m_hMenu, &bSkinChange, std::bind(&CDonutToolBar::ReloadSkin, &m_ToolBar));
	CMousePropertyPage				pageMouse(menu.m_hMenu, m_SearchBar.GetSearchEngineMenuHandle());
	CMouseGesturePropertyPage		pageGesture(menu.m_hMenu);
	CSearchPropertyPage 			pageSearch;
	CMenuPropertyPage				pageMenu;
	CRightClickPropertyPage			pageRightMenu(menu, m_hWnd);
	CExplorerBarPropertyPage		pageExplorerBar(std::bind(&CDonutExplorerBar::HookMouseMoveForAutoShow, &m_ExplorerBar, std::placeholders::_1));
	CSkinPropertyPage				pageSkin(m_hWnd, &bSkinChange);
	CLinkBarPropertyPage			pageLinks(m_LinkBar);

	CString strURL, strTitle;
	if (auto pChildUIData = CChildFrameCommandUIUpdater::GetActiveChildFrameUIData()) {
		strURL	 = pChildUIData->strLocationURL;
		strTitle = pChildUIData->strTitle;
	}

	CSurpressPopupPropertyPage		pageSurpreePopup(strURL, strTitle);
	CUrlSecurityPropertyPage		pageUrlSecu(strURL, m_hWnd);		//+++
	CBrowserEmulationPropertyPage	pageBrowserEmulation(strURL);
	CUserDefinedCSSPropertyPage		pageUserCSS(strURL);
	CUserDefinedJsPropertyPage		pageUserJs(strURL);
	CDonutExecutablePropertyPage	pageExe;
	CPluginPropertyPage 			pagePlugin;

	CTreeViewPropertySheet			sheet( _T("Donut�̃I�v�V����") );

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
	sheet.AddPage( pageKeyBoard			 );
	sheet.AddPage( pageMouse			 );
	sheet.AddPage( pageGesture	 , TRUE  );
	sheet.AddPage( pageDLC				 );
	sheet.AddPage( pageSurpreePopup, TRUE  );
	sheet.AddPage( pageUrlSecu 	 , FALSE );				//+++
	sheet.AddPage( pageBrowserEmulation, FALSE );
	//sheet.AddPage( pageUserCSS	 , FALSE );
	//sheet.AddPage( pageUserJs	 , FALSE );
	sheet.AddPage( pageStartUpFinish );
	sheet.AddPage( pageExe			 );
	sheet.AddPage( pageProxy		 );
	sheet.AddPage( pageSkin 		 );
	//sheet.AddPage( pagePlugin		 );

	/* [Donut�̃I�v�V����]��\�� */
	sheet.DoModal();

	SetGlobalConfig(m_GlobalConfigManageData.pGlobalConfig);
	//m_cmbBox.ResetProxyList();

	if (CMainOption::s_dwMainExtendedStyle & MAIN_EX_BACKUP)
		SetTimer(kAutoBackupTimerId, CMainOption::s_dwBackUpTime * 60000);
	else
		KillTimer(kAutoBackupTimerId);

	//RtlSetMinProcWorkingSetSize();		//+++ (�������̗\��̈���ꎞ�I�ɍŏ����B�E�B���h�E���ŏ��������ꍇ�Ɠ���)
	SendMessage(WM_COMMAND, ID_VIEW_SETFOCUS);
}


void	CMainFrame::Impl::OnTabClose(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	switch (nID) {
	case ID_WINDOW_CLOSE_EXCEPT:	/// ���̃E�B���h�E�ȊO�����
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

	case ID_WINDOW_CLOSE_ALL:		/// �S�ẴE�B���h�E�����(���ׂĕ���)
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

	case ID_LEFT_CLOSE:		/// �^�u�̉E��/���������ׂĕ���
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

/// Donut�̂���t�H���_���J��
void	CMainFrame::Impl::OnOpenDonutExeFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString	progDir = Misc::GetExeDirectory();
	ShellExecute(NULL, NULL, progDir, progDir, NULL, SW_SHOW);
}

/// ��҂̃y�[�W���J��
void	CMainFrame::Impl::OnAuthorWebSite(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	//CString strSite = _T("http://www5.ocn.ne.jp/~minute/tab/"); //unDonut�Â���
	//CString strSite = _T("http://undonut.sakura.ne.jp/"); // unDonut�V������
	//CString strSite = _T("http://tekito.genin.jp/undonut+.html"); //unDonut+
	//CString strSite = _T("http://ichounonakano.sakura.ne.jp/64/undonut/"); //unDonut+mod.	��
	//CString strSite = _T("http://undonut.undo.jp/"); //unDonut+mod.
	//CString strSite = _T("http://cid-8830de058eedff85.skydrive.live.com/browse.aspx/%e5%85%ac%e9%96%8b/unDonut"); // ������.
	CString strSite = _T("http://www31.atwiki.jp/lafe/pages/32.html");

	UserOpenFile( strSite, DonutGetStdOpenFlag() );
}

void	CMainFrame::Impl::OnMainFrameMiniMaximize(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	switch (nID) {
	case ID_MAINFRAME_MINIMIZE:
		ShowWindow(SW_MINIMIZE);
		break;

	case ID_MAINFRAME_MAXIMIZE:
		ShowWindow(SW_MAXIMIZE);
		break;

	default:
		ATLASSERT(FALSE);
	}
}

/// ���C�����j���[�� [�\��]-[�c�[���o�[]��\������
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


/// ���o�[�̃o���h�̍��[�Ƀe�L�X�g��ݒ肷��
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
				rbBand.lpText = _T("�A�h���X");
				break;

			case IDC_LINKBAR:
				rbBand.lpText = _T("�����N");
				break;

			case IDC_SEARCHBAR:
				rbBand.lpText = _T("����");
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
		// �y�[�W���̃����N���J���Ƃ��A�쐬���ꂽ�V�K�^�u���A�N�e�B�u�ɂ��Ȃ�
		if (!_check_flag(MAIN_EX_NOACTIVATE_NEWWIN, CMainOption::s_dwMainExtendedStyle))
			dwOption |= TAB_ACTIVE;
	}
	m_TabBar.OnMDIChildCreate(hWndChildFrame, (dwOption & TAB_ACTIVE) != 0);
	
	if (dwOption & TAB_LOCK) {
		m_TabBar.NavigateLockTab(hWndChildFrame, true);
	}
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
		m_TabBar.InsertHere(false);	// �^�u�o�[����̑�����I��点��
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
	bool	bNoting = true;	// �������Ȃ�����
	bool	bCancelRButtonUp = false;
	MSG msg = { 0 };
	do {
		BOOL nRet = GetMessage(&msg, NULL, 0, 0);
		if (nRet == 0 || nRet == -1 || GetCapture() != m_hWnd)
			break;
		if (GetAsyncKeyState(VK_RBUTTON) > 0) {		// �E�{�^���������ꂽ
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
			if (bNoting == false)	// ���̃R�}���h�����s�ς�
				break;
			CPoint	ptNow(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			::ClientToScreen(msg.hwnd, &ptNow);
			if (data.bCursorOnSelectedText) {
				if (nDistance < 10) {
					nDistance = PointDistance(ptDown, ptNow);	// ���������߂�
					if (nDistance < 10)
						break;
				}
				SetCursor(cursor);	// �J�[�\����ύX����

				CString strMark;
				if (m_GlobalConfigManageData.pGlobalConfig->bUseRect) {
					int nAng  = (int) _GetAngle(ptDown, ptNow);	// �p�x�����߂�
					if		  (nAng <  45 || nAng >  315) {
						strSearchEngine = m_GlobalConfigManageData.pGlobalConfig->strREngine;	
						strMark = _T("[��] ");
					} else if (nAng >= 45 && nAng <= 135) {
						strSearchEngine = m_GlobalConfigManageData.pGlobalConfig->strTEngine;	
						strMark = _T("[��] ");
					} else if (nAng > 135 && nAng <  225) {
						strSearchEngine = m_GlobalConfigManageData.pGlobalConfig->strLEngine;	
						strMark = _T("[��] ");
					} else if (nAng >= 225 && nAng <= 315) {
						strSearchEngine = m_GlobalConfigManageData.pGlobalConfig->strBEngine;
						strMark = _T("[��] ");
					}
				} else {
					strSearchEngine = m_GlobalConfigManageData.pGlobalConfig->strCEngine;
				}
				if (strSearchEngine.IsEmpty() == FALSE) {
					CString strMsg;
					strMsg.Format(_T("���� %s: %s"), strMark, strSearchEngine);
					funcSetStatusText(strMsg);
				} else {
					funcSetStatusText(_T(""));
				}
			} else {
				nDistance = PointDistance(ptLast, ptNow);	// ���������߂�
				if (nDistance < 10)
					break;
				
				CString strMark1;
				int nAng1  = (int) _GetAngle(ptLast, ptNow);	// �p�x�����߂�
				if		(nAng1 <  45 || nAng1 >  315)
					strMark1 = _T("��");
				else if (nAng1 >= 45 && nAng1 <= 135)
					strMark1 = _T("��");
				else if (nAng1 > 135 && nAng1 <  225)
					strMark1 = _T("��");
				else if (nAng1 >= 225 && nAng1 <= 315)
					strMark1 = _T("��");

				if (strMark1 == strLastMark) {					// ���������ɓ������āA����300ms�ȏ�o�����Ȃ�L��		
					DWORD dwTimeNow = ::GetTickCount();
					if ( (dwTimeNow - dwTime) > 300 ) {
						strLastMark = _T("");
						dwTime	 = dwTimeNow;
					}
				}
				if (strMark1 != strLastMark) {
					strMove	+= strMark1;	// ������ǉ�
					strLastMark = strMark1;	

					CString strCmdName;
					CIniFileI	pr( GetConfigFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
					DWORD	dwCommand = pr.GetValue(strMove);
					if (dwCommand) {
						// ���v����R�}���h������Ε\��
						CString strTemp;
						CToolTipManager::LoadToolTipText(dwCommand, strTemp);
						strCmdName.Format(_T("[ %s ]"), strTemp);
					}

					// �X�e�[�^�X�o�[�ɕ\��
					CString 	strMsg;
					strMsg.Format(_T("�W�F�X�`���[ : %s %s"), strMove, strCmdName);
					funcSetStatusText(strMsg);
				}
				dwTime = ::GetTickCount();
			}
			ptLast = ptNow;
			break;
							}

		case WM_LBUTTONDOWN:
			if (data.bCursorOnSelectedText && nDistance >= 10) {	// �E�{�^���h���b�O���L�����Z������
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
			::PostMessage(m_ChildFrameClient.GetActiveChildFrameWindow(), WM_CLOSE, 0, 0);	// ���ꂾ�����Ⴝ�Ԃ�_��
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
		SetCursor(::LoadCursor(NULL, IDC_ARROW));	// �J�[�\�������ɖ߂�
		if (bCancelRButtonUp)
			return TRUE;
		if (strSearchEngine.GetLength() > 0) {	// �E�{�^���h���b�O���s
			funcSetStatusText(_T(""));
			m_SearchBar.SearchWebWithEngine(data.strSelectedTextLine, strSearchEngine);
			bNoting = false;
		}
	}

	/* �}�E�X�W�F�X�`���[�R�}���h���s */
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

	if ( bNoting && strMove.IsEmpty() ) {	// �E�N���b�N���j���[���o��
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
		if (CSkinOption::s_nMainFrameCaption)	//+++	�L���v�V�����̒����͂Â�������̏ꍇ
			ModifyStyle(0, WS_CAPTION);

		if (m_bOldMaximized == false) {
			ShowWindow(SW_RESTORE);		//+++ �T�C�Y��߂�
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


/// �g���C�A�C�R���̐ݒ�.
void CMainFrame::Impl::_SetHideTrayIcon()
{
	//RtlSetMinProcWorkingSetSize();		//+++ (�������̗\��̈���ꎞ�I�ɍŏ����B�E�B���h�E���ŏ��������ꍇ�Ɠ���)
	CString iconPath = _GetSkinDir() + _T("icon.ico");
	CIconHandle icon;
	if (::PathFileExists(iconPath))
		icon.LoadIcon((LPCTSTR)iconPath, 16, 16, LR_LOADFROMFILE);
	if (icon.IsNull())
		icon.LoadIcon(IDR_MAINFRAME);
	TrayMessage(m_hWnd, NIM_ADD, TM_TRAY, icon, DONUT_NAME);		//+++

	ShowWindow(SW_HIDE);
}

/// �g���C���̏I��/�g���C�A�C�R���̍폜.
void CMainFrame::Impl::_DeleteTrayIcon()
{
	TrayMessage(m_hWnd, NIM_DELETE, TM_TRAY, 0, NULL);
	//x m_bTrayFlag = false;
	if (IsWindowVisible() == false)
		ShowWindow(SW_RESTORE);

	//RtlSetMinProcWorkingSetSize();		//+++ ( �������̗\��̈���ꎞ�I�ɍŏ����B�E�B���h�E���ŏ��������ꍇ�Ɠ��� )
}


void	CMainFrame::Impl::_NewDonutInstance(const CString& strURL)
{
	std::vector<CString> vecUrl;
	PerseUrls(strURL, vecUrl);
	if (vecUrl.size() == 1) {
		OnDDEOpenFile(vecUrl.front());

	} else if (vecUrl.size() > 1) {
		for (auto it = vecUrl.cbegin(); it != vecUrl.cend(); ++it) {
			std::unique_ptr<NewChildFrameData>    item(new NewChildFrameData(m_ChildFrameClient));
			item->strURL = *it;
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













