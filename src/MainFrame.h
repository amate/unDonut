/**
 *	@file	MainFrame.h
 *	@brief	interface of the CMainFrame class
 *	@note
 *		+++ mainfrm.h を ヘッダとcppに分割。また、クラス名に併せてファイル名もMainFrameに変更.
 */
// 本気で書き直したくなってきた(minit)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DonutDefine.h"
#include "DonutFavoritesMenu.h"
#include "DonutOptions.h"
#include "DonutSecurityZone.h"
#include "DonutSimpleEventManager.h"
#include "MainFrameFileDropTarget.h"
#include "DonutRebarCtrl.h"
#include "PluginBar.h"
#include "DonutExplorerBar.h"
#include "DonutLinksBarCtrl.h"
#include "DonutToolBar.h"
#include "DonutStatusBarCtrl.h"
#include "DonutAddressBar.h"
#include "DonutSearchBar.h"
#include "DonutTabBar.h"
#include "DonutP.h"
//#include "DonutP_i.c"
//+++ #include "FileCriticalSection.h"				//+++ 別の方法をとるので、不要になった.
#include "MDIChildUserMessenger.h"
#include "ChildFrame.h"								//+++ "ChildFrm.h"
#include "MenuControl.h"
#include "AccelManager.h"
#include "dialog/DebugWindow.h"
#include "MenuEncode.h"
#include "BingTranslatorMenu.h"
#include "StyleSheetOption.h"
#include "ExStyle.h"
#include "MenuDropTargetWindow.h"
#include "Download/DownloadManager.h"
#include "option/AddressBarPropertyPage.h"
#include "option/SearchPropertyPage.h"
#include "FindBar.h"
#include "ChildFrameCommandUIUpdater.h"
#include "DonutViewOption.h"
#include "RecentClosedTabList.h"
#include "ExMenu.h"


///////////////////////////////////////////////////////////////
// CChildFrameClient

class CChildFrameClient : public CWindowImpl<CChildFrameClient>
{
public:
	DECLARE_WND_CLASS_EX(_T("DonutChildFrameClient"), CS_BYTEALIGNCLIENT, COLOR_APPWORKSPACE)

	CChildFrameClient();

	HWND	Create(HWND hWndMainFrame);

	void	SetChildFrameWindow(HWND hWndChildFrame);
	HWND	GetActiveChildFrameWindow() const { return m_hWndChildFrame; }

	// Message map
	BEGIN_MSG_MAP( CChildFrameClient )
		MSG_WM_SIZE( OnSize )
		MSG_WM_ERASEBKGND( OnEraseBkgnd )
	END_MSG_MAP()

	void OnSize(UINT nType, CSize size);
	BOOL OnEraseBkgnd(CDCHandle dc);

private:
	// Data members
	HWND	m_hWndChildFrame;
	CBitmap	m_bmpBackground;
};


//////////////////////////////////////////////////////////////////////////////
// CMainFrame

class CMainFrame
	:	public CFrameWindowImpl				< CMainFrame >
	,	public CMessageFilter
	,	public CIdleHandler
	,	public CAppCommandHandler			< CMainFrame >
	,	public CUpdateCmdUI 				< CMainFrame >
	,	public CDDEMessageHandler			< CMainFrame >
	//:::,	public CMDIFrameTitleUpsideDownMessageHandlerWeb < CMainFrame >
	,	public CHelpMessageLine 			< CMainFrame >
	,	public CMSMouseWheelMessageHandler	< CMainFrame >
	,	public CMainFrameFileDropTarget 	< CMainFrame >
{
	typedef CFrameWindowImpl<CMainFrame>  baseClass;

public:
	// Declarelations
	DECLARE_FRAME_WND_CLASS(DONUT_WND_CLASS_NAME, IDR_MAINFRAME)	

	// Constructor/Destructor
	CMainFrame();
	~CMainFrame();

	void	SetCommandLine(LPCTSTR strCommandLine) { m_strCommandLine = strCommandLine; }
	
	bool 			OnDDEOpenFile(const CString &strFileName);
	void 			SetAutoBackUp() { OnBackUpOptionChanged(0,0,0); }	///+++	オートバックアップの開始設定.
	void 			startupMainFrameStayle(int nCmdShow, bool bTray);	//+++ 起動初回の窓サイズ設定.

	bool			IsFullScreen() { return m_bFullScreen != 0;	/*return (GetStyle() & WS_CAPTION) == 0;*/	} //+++
	CDonutTabBar& 	mdiTab() { return m_MDITab; }						//+++

	CChildFrame*	GetChildFrame(HWND hWndChild);
	CChildFrame*	GetActiveChildFrame();

	CString 		GetActiveSelectedText();

	LRESULT 		OnPluginCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled); //PluginEventImpl.hで実装

	static BOOL AddSimpleReBarBandCtrl(
					HWND	hWndReBar,
					HWND	hWndBand,
					int 	nID,
					LPTSTR	lpstrTitle,
					UINT	fStyle,
					int 	cxWidth,
					BOOL	bFullWidthAlways,
					HBITMAP hBitmap 		= NULL);

	void			SaveAllTab();
	void			RestoreAllTab(LPCTSTR strFilePath = NULL, bool bClose = false);
	void			OpenBingTranslator(const CString& strText);

public:
	/////////////////////////////////////////////
	// DonutP API
	/////////////////////////////////////////////

	IDispatch*	ApiGetDocumentObject(int nTabIndex);
	IDispatch*	ApiGetWindowObject(int nTabIndex);
	IDispatch*	ApiGetWebBrowserObject(int nTabIndex);
	long 		ApiGetTabIndex();
	void 		ApiSetTabIndex(int nTabIndex);
	void 		ApiClose(int nTabIndex);
	long 		ApiGetTabCount();
	void 		ApiMoveToTab(int nBeforIndex, int nAfterIndex);
	int 		ApiNewWindow(BSTR bstrURL, BOOL bActive);
	void 		ApiShowPanelBar();
	long 		ApiGetTabState(int nIndex);
	IDispatch*	ApiGetPanelWebBrowserObject();
	IDispatch*	ApiGetPanelWindowObject();
	IDispatch*	ApiGetPanelDocumentObject();
	void 		ApiExecuteCommand(int nCommand);	//IAPI2 by minit
	void 		ApiGetSearchText( /*[out, retval]*/ BSTR *bstrText);
	void 		ApiSetSearchText(BSTR bstrText);
	void 		ApiGetAddressText( /*[out, retval]*/ BSTR *bstrText);
	void 		ApiSetAddressText(BSTR bstrText);
	LRESULT 	ApiGetExtendedTabState(int nIndex);
	void 		ApiSetExtendedTabState(int nIndex, long nState);
	LRESULT 	ApiGetKeyState(int nKey);
	long 		ApiGetProfileInt(BSTR bstrFile, BSTR bstrSection, BSTR bstrKey, int nDefault);
	void 		ApiWriteProfileInt(BSTR bstrFile, BSTR bstrSection, BSTR bstrKey, int nValue);
	void 		ApiGetProfileString(BSTR bstrFile, BSTR bstrSection, BSTR bstrKey, BSTR bstrDefault, /*[out, retval]*/ BSTR *bstrText);
	void 		ApiWriteProfileString(BSTR bstrFile, BSTR bstrSection, BSTR bstrKey, BSTR bstrText);
	void 		ApiGetScriptFolder( /*[out, retval]*/ BSTR *bstrFolder);
	void 		ApiGetCSSFolder( /*[out, retval]*/ BSTR *bstrFolder);
	void 		ApiGetBaseFolder( /*[out, retval]*/ BSTR *bstrFolder);
	void 		ApiGetExePath( /*[out, retval]*/ BSTR *bstrPath);
	void 		ApiSetStyleSheet(int nIndex, BSTR bstrStyleSheet, BOOL bOff);
	void 		ApiSaveGroup(BSTR bstrGroupFile);
	void 		ApiLoadGroup(BSTR bstrGroupFile, BOOL bClose);
	int 		ApiNewWindow3(BSTR bstrURL, BOOL bActive, long ExStyle, void *pExInfo);
	long 		ApiAddGroupItem(BSTR bstrGroupFile, int nIndex);
	long 		ApiDeleteGroupItem(BSTR bstrGroupFile, int nIndex);
	HWND 		ApiGetHWND(long nType);	//IAPI4

public:
	#define 	RELECT_STATUSBAR_DRAWITEM_MESSAGE() 						   \
	{																		   \
		LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam; 				   \
		if (uMsg == WM_DRAWITEM && lpdis->hwndItem == m_wndStatusBar.m_hWnd) { \
			bHandled = TRUE;												   \
			lResult  = ReflectNotifications(uMsg, wParam, lParam, bHandled);   \
			if (bHandled)													   \
				return TRUE;												   \
		}																	   \
	}

	// Overrides
	virtual BOOL PreTranslateMessage(MSG *pMsg);	// 各ウィンドウへ(主にキー)メッセージを転送する
	virtual BOOL OnIdle();
	void 	UpdateLayout(BOOL bResizeBars = TRUE);

	// Message map
	BEGIN_MSG_MAP( CMainFrame )
		m_bCommandFromChildFrame = false;
		if (uMsg == WM_COMMAND_FROM_CHILDFRAME) {
			uMsg = WM_COMMAND;
			m_bCommandFromChildFrame	= true;
		}
		COMMAND_EXMENU_RANGE( EXMENU_FIRST, EXMENU_LAST )
		RELECT_STATUSBAR_DRAWITEM_MESSAGE( )

		CHAIN_MSG_MAP_MEMBER( m_FavGroupMenu	)
		CHAIN_MSG_MAP_MEMBER( m_FavoriteMenu	)
		CHAIN_MSG_MAP_MEMBER( m_styleSheetMenu	)
		CHAIN_MSG_MAP_MEMBER( m_ScriptMenu 		)
		CHAIN_MSG_MAP_MEMBER( m_DropScriptMenu  )
		CHAIN_MSG_MAP_MEMBER( m_MenuEncode		)
		CHAIN_MSG_MAP_MEMBER( m_TranslateMenu	)
		CHAIN_MSG_MAP_MEMBER( m_ChildFrameUIState )

		USER_MSG_WM_BROWSERTITLECHANGE( OnBrowserTitleChange )
		USER_MSG_WM_BROWSERLOCATIONCHANGE( OnBrowserLocationChange )
		USER_MSG_WM_SETSEARCHTEXT( OnSetSearchText )
		CHAIN_MSG_MAP		( CUpdateCmdUI<CMainFrame> )
		USER_MSG_WM_UIUPDATE()
#ifdef _DEBUG
		COMMAND_ID_HANDLER_EX( ID_DEBUGCOMMAND, OnDebugCommand )
#endif
		MSG_WM_CREATE			( OnCreate			)
		MSG_WM_CLOSE			( OnClose			)
		MSG_WM_DESTROY			( OnDestroy 		)
		MSG_WM_ENDSESSION		( OnEndSession		)
		USER_MSG_WM_NEWINSTANCE ( OnNewInstance 	)
		MSG_WM_USER_MDICHILD	( OnMDIChild		)
		MSG_WM_PAINT			( OnPaint			)
		MSG_WM_COPYDATA			( OnCopyData		)
		USER_MSG_WM_INITPROCESSFINISHED( OnInitProcessFinished )

		// CChildFrameから
		USER_MSG_WM_TABCREATE				( OnTabCreate	)	
		USER_MSG_WM_TABDESTROY				( OnTabDestory	)
		USER_MSG_WM_ADDRECENTCLOSEDTAB		( OnAddRecentClosedTab )
		USER_MSG_WM_OPENFINDBARWITHTEXT		( OnOpenFindBarWithText )
		USER_MSG_WM_GETTABINDEX				( OnGetTabIndex )
		USER_MSG_WM_CHILDFRAMECONNECTING	( OnChildFrameConnecting	)
		USER_MSG_WM_CHILDFRAMEDOWNLOADING	( OnChildFrameDownloading	)
		USER_MSG_WM_CHILDFRAMECOMPLETE		( OnChildFrameComplete		)

		USER_MSG_WM_UPDATE_TITLEBAR( UpdateTitleBar )						// UDT DGSTR

		MESSAGE_HANDLER 		 ( MYWM_NOTIFYICON , OnMyNotifyIcon  )		// UDT DGSTR ( SystemTrayIcon Handler )
		COMMAND_ID_HANDLER_EX	 ( ID_GET_OUT	   , OnGetOut		 )		// UDT DGSTR
		USER_MSG_WM_UPDATE_EXPBAR( UpdateExpBar )							// UDT DGSTR

		MSG_WM_USER_BROWSER_CAN_SETFOCUS( OnBrowserCanSetFocus )
		MSG_WM_ACTIVATE 	 ( OnActivate )

		MESSAGE_HANDLER 	 ( WM_MENUDRAG	   , OnMenuDrag 	 )
		MESSAGE_HANDLER 	 ( WM_MENUGETOBJECT, OnMenuGetObject )

		// map custom draw notifycation to addressbar
		MESSAGE_HANDLER 	 ( WM_NOTIFY	   , OnNotify		 )

		HANDLE_MESSAGE		 ( WM_MDISETMENU		)
		HANDLE_MESSAGE		 ( WM_SETFOCUS			)			// default handler will set focus to server window

		MSG_WM_TIMER		 ( OnTimer				)
		MSG_WM_MOUSEWHEEL	 ( OnMouseWheel 		)
		DEBUG_CHECK_FOCUS_COMMAND_ID_HANDLER_EX()

		// ファイル
		COMMAND_ID_HANDLER	 ( ID_FILE_NEW				, OnFileNew 			)
		COMMAND_ID_HANDLER	 ( ID_FILE_OPEN 			, OnFileOpen			)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_HOME 		, OnFileNewHome 		)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_CLIPBOARD	, OnFileNewClipBoard	)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_CLIPBOARD2	, OnFileNewClipBoard2	)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_CLIPBOARD_EX , OnFileNewClipBoardEx	)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_BLANK		, OnFileNewBlank		)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_COPY 		, OnFileNewCopy 		)
		COMMAND_ID_HANDLER( ID_FILE_OPEN_TABLIST		, OnFileOpenTabList		)
		COMMAND_RANGE_HANDLER( ID_RECENTDOCUMENT_FIRST, ID_RECENTDOCUMENT_LAST, OnFileRecent )
		COMMAND_RANGE_HANDLER( ID_FILE_MRU_FIRST	  , ID_FILE_MRU_LAST	  , OnFileRecent )

		// 編集
		COMMAND_ID_HANDLER_EX( ID_EDIT_CUT				, OnEditCut 			)
		COMMAND_ID_HANDLER_EX( ID_EDIT_COPY 			, OnEditCopy			)
		COMMAND_ID_HANDLER_EX( ID_EDIT_PASTE			, OnEditPaste			)
		COMMAND_ID_HANDLER_EX( ID_EDIT_SELECT_ALL		, OnEditSelectAll		)
		COMMAND_ID_HANDLER( ID_SEARCHBAR_SEL_DOWN, OnSearchBarCmd )
		COMMAND_ID_HANDLER( ID_SEARCHBAR_SEL_UP  , OnSearchBarCmd )
		COMMAND_ID_HANDLER( ID_SEARCHBAR_HILIGHT , OnSearchBarCmd )
		MSG_WM_USER_HILIGHT 			( OnHilight 	   );
		MSG_WM_USER_FIND_KEYWORD		( OnFindKeyWord    );

		// お気に入り
		COMMAND_ID_HANDLER_EX( ID_FAVORITE_ADD			, OnFavoriteAdd 		)
		COMMAND_ID_HANDLER_EX( ID_FAVORITE_ORGANIZE 	, OnFavoriteOrganize	)
		COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_SAVE	, OnFavoriteGroupSave	)
		COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_ADD	, OnFavoriteGroupAdd	)	//+++ 追加
		COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_ORGANIZE, OnFavoriteGroupOrganize )

		COMMAND_ID_HANDLER_EX( ID_BACKUPOPTION_CHANGED	, OnBackUpOptionChanged )

		// 表示
		COMMAND_ID_HANDLER	 ( ID_VIEW_SEARCHBAR		, OnViewSearchBar		)	// U.H
		COMMAND_ID_HANDLER	 ( ID_VIEW_COMMANDBAR		, OnViewCommandBar		)	// U.H
		COMMAND_ID_HANDLER	 ( ID_VIEW_TOOLBAR			, OnViewToolBar 		)
		COMMAND_ID_HANDLER	 ( ID_VIEW_ADDRESSBAR		, OnViewAddressBar		)
		COMMAND_ID_HANDLER	 ( ID_VIEW_LINKBAR			, OnViewLinkBar 		)
		COMMAND_ID_HANDLER	 ( ID_VIEW_TABBAR			, OnViewTabBar			)
		COMMAND_ID_HANDLER	 ( ID_VIEW_STATUS_BAR		, OnViewStatusBar		)
		COMMAND_ID_HANDLER	 ( ID_VIEW_GOBUTTON 		, OnViewGoButton		)
		COMMAND_ID_HANDLER	 ( ID_VIEW_TOOLBAR_CUST 	, OnViewToolBarCust 	)
		COMMAND_ID_HANDLER	 ( ID_VIEW_TABBAR_MULTI 	, OnViewTabBarMulti 	)
		COMMAND_ID_HANDLER	 ( ID_VIEW_ADDBAR_DROPDOWN	, OnViewAddBarDropDown	)
		COMMAND_ID_HANDLER	 ( ID_VIEW_TOOLBAR_LOCK 	, OnViewToolBarLock 	)	// minit
		COMMAND_ID_HANDLER_EX( ID_EXPLORERBAR_AUTOSHOW	, OnExplorerBarAutoShow )
		COMMAND_ID_HANDLER( ID_FAVTREE_BAR_STANDARD  , OnFavoriteExpBar )
		COMMAND_ID_HANDLER( ID_FAVTREE_BAR_GROUP	 , OnFavoriteExpBar )
		COMMAND_ID_HANDLER( ID_FAVTREE_BAR_SCRIPT	 , OnFavoriteExpBar )
		COMMAND_ID_HANDLER( ID_FAVTREE_BAR_USER 	 , OnFavoriteExpBar )
		COMMAND_ID_HANDLER( ID_FAVTREE_BAR_MYCOMPUTER, OnFavoriteExpBar )
		COMMAND_ID_HANDLER( ID_FAVTREE_BAR_HISTORY	 , OnFavoriteExpBar )
		COMMAND_ID_HANDLER_EX( ID_SETFOCUS_ADDRESSBAR	, OnSetFocusAddressBar	)
		COMMAND_ID_HANDLER_EX( ID_SETFOCUS_SEARCHBAR	, OnSetFocusSearchBar	)
		COMMAND_ID_HANDLER_EX( ID_SETFOCUS_SEARCHBAR_ENGINE, OnSetFocusSearchBarEngine )
		COMMAND_ID_HANDLER_EX( ID_VIEW_HOME 			, OnViewHome			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_STOP_ALL 		, OnViewStopAll 		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_REFRESH_ALL		, OnViewRefreshAll		)
		COMMAND_ID_HANDLER_EX( ID_WINDOW_REFRESH_EXCEPT	, OnWindowRefreshExcept )
		COMMAND_ID_HANDLER_EX( ID_VIEW_FULLSCREEN		, OnViewFullScreen		)

		// ツール
		COMMAND_ID_HANDLER_EX( ID_REGISTER_AS_BROWSER	, OnRegisterAsBrowser )
		COMMAND_ID_HANDLER_EX( ID_POPUP_CLOSE			 , OnPopupClose  )
		COMMAND_ID_HANDLER_EX( ID_TITLE_CLOSE			 , OnTitleClose  )
		COMMAND_ID_HANDLER_EX( ID_DOUBLE_CLOSE			 , OnDoubleClose )
		COMMAND_ID_HANDLER_EX( ID_URLACTION_COOKIES_BLOCK, OnCookiesIE6  )
		COMMAND_ID_HANDLER_EX( ID_URLACTION_COOKIES_HI	 , OnCookiesIE6  )
		COMMAND_ID_HANDLER_EX( ID_URLACTION_COOKIES_MIDHI, OnCookiesIE6  )
		COMMAND_ID_HANDLER_EX( ID_URLACTION_COOKIES_MID  , OnCookiesIE6  )
		COMMAND_ID_HANDLER_EX( ID_URLACTION_COOKIES_LOW  , OnCookiesIE6  )
		COMMAND_ID_HANDLER_EX( ID_URLACTION_COOKIES_ALL  , OnCookiesIE6  )
		MSG_WM_USER_CHANGE_CSS							( OnChangeCSS )
		COMMAND_ID_HANDLER_EX( ID_VIEW_OPTION			, OnViewOption	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_OPTION_DONUT 	, OnViewOptionDonut )

		// ウィンドウ
		COMMAND_ID_HANDLER_EX( ID_WINDOW_CLOSE_ALL		, OnWindowCloseAll		)
		COMMAND_ID_HANDLER_EX( ID_LEFT_CLOSE			, OnLeftRightClose		)
		COMMAND_ID_HANDLER_EX( ID_RIGHT_CLOSE			, OnLeftRightClose		)
		COMMAND_ID_HANDLER_EX( ID_WINDOW_CLOSE_EXCEPT	, OnWindowCloseExcept	)
		COMMAND_ID_HANDLER_EX( ID_TAB_LEFT				, OnTabLeft 			)
		COMMAND_ID_HANDLER_EX( ID_TAB_RIGHT 			, OnTabRight			)
		//COMMAND_RANGE_HANDLER( ID_TAB_IDX_1, ID_TAB_IDX_LAST, OnTabIdx )	
		//+++ 先頭タブから1..8選択. 9は最後のタブを選択

		// ヘルプ
		COMMAND_ID_HANDLER	 ( ID_OPEN_EXE_DIR			, OnOpenExeDir		)
		COMMAND_ID_HANDLER	 ( ID_JUMP_WEBSITE			, OnJumpToWebSite 	)
		COMMAND_ID_HANDLER	 ( ID_APP_ABOUT 			, OnAppAbout		)

		USER_MEG_WM_MENU_GET_FAV		( OnMenuGetFav		)
		USER_MEG_WM_MENU_GET_FAV_GROUP	( OnMenuGetFavGroup )
		USER_MEG_WM_MENU_GET_SCRIPT 	( OnMenuGetScript	)
		USER_MSG_WM_MENU_GOBACK 		( OnMenuGoBack		)
		USER_MSG_WM_MENU_GOFORWARD		( OnMenuGoForward	)
		USER_MEG_WM_MENU_GET_BINGTRANSLATE( OnMenuGetBingTranslate )

		MSG_WM_USER_SHOW_TEXT_CHG		( OnShowTextChg  )

		MSG_WM_SYSCOMMAND				( OnSysCommand	  )
		MSG_WM_INITMENUPOPUP			( OnInitMenuPopup )


		USER_MEG_WM_MENU_REFRESH_SCRIPT 	( OnMenuRefreshScript	)
		USER_MSG_WM_GET_FAVORITEFILEPATH	( OnGetFavoriteFilePath )

		COMMAND_RANGE_HANDLER_EX(ID_INSERTPOINT_SEARCHENGINE, ID_INSERTPOINT_SEARCHENGINE_END, OnSearchWeb_engineId)

		USER_MSG_WM_MENU_RECENTDOCUMENT( OnMenuRecentDocument )

		COMMAND_ID_HANDLER( ID_STYLESHEET_USE_USERS, OnUseUserStyleSheet )
		COMMAND_ID_HANDLER( ID_STYLESHEET_SET_USERS, OnSetUserStyleSheet )

		USER_MSG_WM_CHANGE_SKIN 		( OnSkinChange		 )
		USER_MSG_WM_COMMAND_DIRECT		( OnCommandDirect	 )
		USER_MSG_WM_REFRESH_EXPBAR		( OnRefreshExpBar	 )
		USER_MSG_WM_SEARCH_WEB_SELTEXT	( OnSearchWebSelText )
		USER_MSG_WM_SET_EXPROPERTY		( OnSetExProperty	 )

		COMMAND_ID_HANDLER( ID_MAINFRAME_NORM_MAX_SIZE,OnMainFrameNormMaxSize )

		#define WM_PLUGIN_COMMAND	ID_PLUGIN_COMMAND
		MESSAGE_HANDLER 			( WM_PLUGIN_COMMAND, OnPluginCommand )
		USER_MSG_WM_OPEN_WITHEXPROP ( OnOpenWithExProp	)	// SearchBarから
		USER_MSG_WM_GET_SEARCHBAR	( OnGetSearchBar	)
		USER_MSG_WM_SHOW_TOOLBARMENU( OnShowToolBarMenu )
		
		// Special Command
		COMMAND_ID_HANDLER( ID_RECENT_DOCUMENT	, OnMenuRecentLast		)
		COMMAND_ID_HANDLER( ID_TABLIST_DEFAULT	, OnTabListDefault		)
		COMMAND_ID_HANDLER( ID_TABLIST_VISIBLE	, OnTabListVisible		)
		COMMAND_ID_HANDLER( ID_TABLIST_ALPHABET	, OnTabListAlphabet		)
		COMMAND_ID_HANDLER( ID_WINDOW_THUMBNAIL , OnWindowThumbnail		)
		COMMAND_ID_HANDLER( ID_SPECIAL_REFRESH_SEARCHENGIN, OnSpecialRefreshSearchEngine)
		COMMAND_ID_HANDLER( ID_SHOW_EXMENU, 		OnShowExMenu		)
		COMMAND_ID_HANDLER( ID_SELECT_USERFOLDER,	OnSelectUserFolder	)
		COMMAND_ID_HANDLER( ID_SEARCH_HISTORY,		OnSearchHistory 	)
		COMMAND_ID_HANDLER( ID_MAINFRAME_MINIMIZE,	OnMainFrameMinimize )
		COMMAND_ID_HANDLER( ID_MAINFRAME_MAXIMIZE,	OnMainFrameMaximize )
		COMMAND_ID_HANDLER( ID_SHOW_ACTIVEMENU	, OnShowActiveMenu )
		COMMAND_ID_HANDLER( ID_PRIVACYREPORT	, OnPrivacyReport )
		COMMAND_ID_HANDLER( ID_SECURITYREPORT	, OnSecurityReport )

		CHAIN_COMMANDS_MEMBER	( m_ReBar		)
		CHAIN_COMMANDS_MEMBER	( m_MainOption	)
		CHAIN_COMMANDS_MEMBER	( m_secZone 	)
		CHAIN_MSG_MAP_MEMBER	( m_DownloadManager )
		CHAIN_COMMANDS			( CAppCommandHandler<CMainFrame>		)
		CHAIN_COMMANDS_TO_EXPLORERBAR( m_ExplorerBar		)

		if (uMsg == WM_COMMAND && m_bCommandFromChildFrame == false) {			
			HWND  hWndChild = m_ChildFrameUIState.GetActiveChildFrameWindowHandle();			
			if (hWndChild)
				::PostMessage(hWndChild, uMsg, wParam, lParam);
		}

		CHAIN_MSG_MAP	( CDDEMessageHandler<CMainFrame>			)
	//:::	CHAIN_MSG_MAP	( CMDIFrameTitleUpsideDownMessageHandlerWeb<CMainFrame> )
		CHAIN_MSG_MAP	( CHelpMessageLine<CMainFrame>				)
		CHAIN_MSG_MAP	( CMSMouseWheelMessageHandler<CMainFrame>	)

		REFLECT_CHEVRONPUSHED_NOTIFICATION( )
		CHAIN_MSG_MAP	( baseClass )
		REFLECT_NOTIFICATIONS( )	// must be last
	END_MSG_MAP()


public:
	BEGIN_UPDATE_COMMAND_UI_MAP( CMainFrame )
		CHAIN_UPDATE_COMMAND_UI_MEMBER( m_MainOption  )
		CHAIN_UPDATE_COMMAND_UI_MEMBER( m_secZone	  )
		CHAIN_UPDATE_COMMAND_UI_MEMBER( m_ExplorerBar )
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
		UPDATE_COMMAND_UI_SETCHECK_IF			( ID_FILE_WORKOFFLINE	, MtlIsGlobalOffline() )
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
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_CLIPBOARDBAR,  m_ExplorerBar.IsClipboardBarVisible()	)
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_PANELBAR, 	 m_ExplorerBar.IsPanelBarVisible()		)
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_PLUGINBAR,	 m_ExplorerBar.IsPluginBarVisible() 	)
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
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_FULLSCREEN	, IsFullScreen()		)

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
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_APP_HELP			, _IsExistHTMLHelp()	)

		// Misc
		// Window menu
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_WINDOW_CLOSE_ALL		, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_WINDOW_CLOSE_EXCEPT	, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_WINDOW_REFRESH_EXCEPT	, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_TAB_LEFT				, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_TAB_RIGHT				, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_WINDOW_RESTORE, bActiveChild, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ATL_IDS_SCPREVWINDOW		, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ATL_IDS_SCNEXTWINDOW		, bActiveChild		)

		// Misc
		// UPDATE_COMMAND_UI_SETTEXT( ID_DEFAULT_PANE, _T("レディ" ))
		UPDATE_COMMAND_UI				( IDC_PROGRESS		, OnUpdateProgressUI		)
		UPDATE_COMMAND_UI				( ID_SECURE_PANE	, OnUpdateStautsIcon		)
		UPDATE_COMMAND_UI				( ID_PRIVACY_PANE	, OnUpdateStautsIcon		)

		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_POPUP_CLOSE	, CIgnoredURLsOption::s_bValid )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_TITLE_CLOSE	, CCloseTitlesOption::s_bValid )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_DOUBLE_CLOSE	, CIgnoredURLsOption::s_bValid && CCloseTitlesOption::s_bValid )

		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_BLOCK, _CheckCookies(ID_URLACTION_COOKIES_BLOCK) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_HI	, _CheckCookies(ID_URLACTION_COOKIES_HI   ) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_MIDHI, _CheckCookies(ID_URLACTION_COOKIES_MIDHI) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_MID	, _CheckCookies(ID_URLACTION_COOKIES_MID  ) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_LOW	, _CheckCookies(ID_URLACTION_COOKIES_LOW  ) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_ALL	, _CheckCookies(ID_URLACTION_COOKIES_ALL  ) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_CSTM , _CheckCookies(ID_URLACTION_COOKIES_CSTM ) )

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


	void	OnTabCreate(HWND hWndChildFrame, DWORD dwOption);
	void	OnTabDestory(HWND hWndChildFrame);	
	void	OnAddRecentClosedTab(ChildFrameDataOnClose* pClosedTabData);
	void	OnOpenFindBarWithText(LPCTSTR strText);
	int		OnGetTabIndex(HWND hWndChildFrame) { return m_MDITab.GetTabIndex(hWndChildFrame); }
	void	OnChildFrameConnecting(HWND hWndChildFrame)	{ m_MDITab.SetConnecting(hWndChildFrame); }
	void	OnChildFrameDownloading(HWND hWndChildFrame){ m_MDITab.SetDownloading(hWndChildFrame); }
	void	OnChildFrameComplete(HWND hWndChildFrame)	{ m_MDITab.SetComplete(hWndChildFrame); }

#ifdef _DEBUG
	void	OnDebugCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
#endif

  #if 1	//+++
	void 	ShowWindow_Restore(bool flag);
  #endif
	void	OnBrowserTitleChange(HWND hWndChildFrame, LPCTSTR strTitle);
	void	OnBrowserLocationChange(LPCTSTR strURL, HICON hFavicon);
	void	OnSetSearchText(LPCTSTR strText, bool bHilightOn);

	LRESULT UpdateTitleBar(LPCTSTR lpszStatusBar, DWORD dwReserved);

	LRESULT OnMenuGetFav();
	LRESULT OnMenuGetFavGroup();
	LRESULT OnMenuGetScript();
	LRESULT OnMenuGoBack(HMENU hMenu);
	LRESULT OnMenuGoForward(HMENU hMenu);
	LRESULT	OnMenuGetBingTranslate();
	LRESULT OnMenuRefreshScript(BOOL bInit);
	LRESULT	OnGetFavoriteFilePath(int nID);
	void	IfTrayRestoreWindow() {	if (m_bTray) OnGetOut(0, 0, 0); }	//+++ トレイ状態だったら窓を復活.
	void 	OnGetOut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnMyNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/);
	LRESULT UpdateExpBar(LPCTSTR lpszExpBar, DWORD dwReserved);
	LRESULT OnRefreshExpBar(int nType);

	LRESULT OnGetSearchBar() { return (LRESULT) &m_SearchBar; }
	//------------------
	// open
	HWND 	UserOpenFile(CString strFileOrURL, DWORD openFlag = DonutGetStdOpenFlag(), int dlCtrlFlag = -1, int extendedStyleFlags = -1);
	LRESULT OnOpenWithExProp(_EXPROP_ARGS *pArgs);
#if 0
	HWND 	OpenUrlWithExProp(CString strUrl, DWORD dwOpenFlag, DWORD dwExProp, DWORD dwExProp2);
	HWND 	OpenUrlWithExProp(CString strUrl, DWORD dwOpenFlag, CString strIniFile, CString strSection = DONUT_SECTION);

	HWND 	OpenExPropertyActive(CString &strUrl, DWORD dwExProp, DWORD dwExProp2, DWORD dwOpenFlag);
	HWND 	OpenExPropertyNew(CString &strUrl, DWORD dwExProp, DWORD dwExProp2, DWORD dwOpenFlag);
	HWND 	OpenExPropertyNot(CString &strUrl, DWORD dwOpenFlag);
#endif
	//^^^^^^^^^^^^^^^^^^

	LRESULT OnMenuDrag(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnMenuGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	//////////////////////////////////////////////////////////////////
	// custom draw of addressbar
	LRESULT OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	//////////////////////////////////////////////////////////////////
	// the custom message from MDI child
	void 	OnMDIChild(HWND hWnd, UINT nCode);
	BOOL 	OnBrowserCanSetFocus();
	void 	OnParentNotify(UINT fwEvent, UINT idChild, LPARAM lParam);

	int		OnCreate(LPCREATESTRUCT /*lpCreateStruct*/);
	void 	OnDestroy();
	void 	OnClose();
	void 	OnEndSession(BOOL wParam, UINT lParam);
	void 	OnActivate(UINT nState, BOOL bMinimized, HWND hWndOther);
	void	OnPaint(CDCHandle /*dc*/);
	BOOL	OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct);
	void	OnInitProcessFinished();

	void 	OnExplorerBarAutoShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	LRESULT OnMouseWheel		(UINT fwKeys, short zDelta, CPoint point);

	void 	OnNewInstance		(ATOM nAtom);			// WM_NEWINSTANCE
	void 	SetHideTrayIcon();
	void 	DeleteTrayIcon();		//+++ トレイ化の終了/トレイアイコンの削除.
	void 	DelTempFiles();
	void 	DelHistory();

	LRESULT OnSysCommand		(UINT nID, CPoint point);
	LRESULT OnInitMenuPopup		(HMENU hMenuPopup, UINT uPos, BOOL bSystemMenu);


	////////////////////////////////////////////////////////////////////////////////

	// ファイルメニュー
	void 	OnFileNewHome		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);		//ポームページ
	void 	OnFileNewCopy		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);		//現在のページ
	LRESULT OnFileNew			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void 	OnFileNewBlank		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);		//空白ページ
	void 	OnFileNewClipBoard	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);		//クリップボード
	LRESULT	OnFileOpenTabList	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnFileOpen			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void 	OnFileNewClipBoard2	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnFileNewClipBoardEx(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnFileRecent		(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	// 編集
	void 	OnEditCut			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnEditCopy			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnEditPaste			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnEditSelectAll		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnSearchBarCmd		(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnHilight			(LPCTSTR lpszKeyWord);
	LRESULT OnFindKeyWord		(LPCTSTR lpszKeyWord, BOOL bBack, long Flags = 0);

	// 表示
	LRESULT OnViewSearchBar		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnViewCommandBar	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnViewToolBar		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnViewAddressBar	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnViewLinkBar		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnViewGoButton		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnViewTabBar		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnViewTabBarMulti	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnViewAddBarDropDown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnViewStatusBar		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnViewToolBarCust	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnViewToolBarLock	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnFavoriteExpBar	(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void 	OnSetFocusAddressBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);
	void 	OnSetFocusSearchBar	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);
	void 	OnSetFocusSearchBarEngine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);
	void 	OnViewHome			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnViewStopAll		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnViewRefreshAll	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnWindowRefreshExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnViewFullScreen	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// お気に入り
	void 	OnFavoriteAdd		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnFavoriteOrganize	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnFavoriteGroupSave	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnFavoriteGroupAdd	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnFavoriteGroupOrganize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// ツール
	void 	OnRegisterAsBrowser	(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnPopupClose		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnTitleClose		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnDoubleClose		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnCookiesIE6		(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	LRESULT OnChangeCSS			(LPCTSTR lpszStyleSheet);
	void 	OnViewOption		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnViewOptionDonut	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// ウィンドウ
	void	OnWindowCloseAll(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnLeftRightClose(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnWindowCloseExcept(UINT uNotifyCode, int nID, CWindow wndCtl);
	void 	OnTabLeft(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);
	void 	OnTabRight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);

	// ヘルプ
	LRESULT OnJumpToWebSite		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnOpenExeDir		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	// Special Commands
	LRESULT OnMenuRecentLast	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnTabListDefault	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnTabListVisible	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnTabListAlphabet	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnWindowThumbnail	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnSpecialRefreshSearchEngine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnShowExMenu		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnSelectUserFolder	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnSearchHistory		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnMainFrameMinimize	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnMainFrameMaximize	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnShowActiveMenu	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnPrivacyReport		(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnSecurityReport	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	// etc

	void 	OnBackUpOptionChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnTimer				(UINT_PTR nIDEvent, TIMERPROC dmy = 0);
	void 	ShowLinkText		(BOOL bShow);
	void 	OnShowTextChg		(BOOL bShow);

  #if 0 //+++
	//+++ 先頭タブから1..8選択. 9は最後のタブを選択
	LRESULT 	OnTabIdx		(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL & /*bHandled*/);
  #endif


	// Implementation

	void 	_WriteProfile();

	///+++
	void _SaveGroupOption(const CString &strFileName, bool bDelay=false);
	void _LoadGroupOption(const CString &strFileName, bool bClose);

	// UI map Handler
	BOOL 	_IsRebarBandLocked();
	bool 	_IsClipboardAvailable();
	bool 	_IsExistHTMLHelp();
	bool 	_CheckCookies(UINT nID);
	int 	_GetRecentCount();
	void 	OnUpdateProgressUI(CCmdUI *pCmdUI);
	void 	OnUpdateStautsIcon(CCmdUI *pCmdUI);

	BOOL 	_Load_OptionalData(CChildFrame *pChild, const CString &strFileName, CString &strSection);
	BOOL 	_Load_OptionalData2(CChildFrame *pChild,
						 std::vector<std::pair<CString, CString> > &ArrayFore,
						 std::vector<std::pair<CString, CString> > &ArrayBack);
	LRESULT OnMenuRecentDocument(HMENU hMenu);
	LRESULT _OnMDIActivate(HWND hWndActive);
	LRESULT OnMainFrameNormMaxSize(WORD/*wNotifyCode*/,WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	LRESULT OnUseUserStyleSheet	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnSetUserStyleSheet	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	LRESULT OnCommandDirect(int nCommand, LPCTSTR lpstr);

	HRESULT OnSkinChange();

	LRESULT OnShowToolBarMenu();
	HRESULT OnSearchWebSelText(LPCTSTR lpstrText, LPCTSTR lpstrEngine);
	//+++
	void 	OnSearchWeb_engineId(UINT code, int id, HWND hWnd);

	LRESULT OnSetExProperty(LPCTSTR lpstrUrlFile);


private:

	// Constants
	enum {
		SHDVID_SSLSTATUS	=	33, 					//SSL証明書表示用
		ENT_READ_ACCEL		=	 2,
		STDBAR_COUNT		=	 6,
	};

	enum {
		_nPosFavoriteMenu	   = 3, _nPosFavGroupMenu	=  2, _nPosViewMenu   = 2,
		_nPosFavoriteMenuOfTab = 2, _nPosMRU			= 11,
		_nPosCssMenu		   = 4, _nPosSubCssMenu 	= 14, _nPosEncodeMenu = 2, _nPosEncodeMenuSub	 = 18
	};

	struct 			_ExtendProfileInfo;

	// Implementation
	void 	init_menus_infomation();
	HWND	init_commandBarWindow();
	HWND	init_toolBar();
	HWND	init_addressBar();
	HWND	init_searchBar();
	HWND	init_tabCtrl();
	HWND	init_linkBar();
	void	init_rebar();
	void	init_statusBar();
	void 	init_pluginManager();
	void 	init_band_position( HWND cmdBar, HWND toolBar, HWND addressBar, HWND mdiTab, HWND linkBar, HWND searchBar );
	void 	init_loadStatusBarState();
	void 	init_ChildFrameClientWindow();
	void 	init_splitterWindow();
	void 	init_explorerBar();
	void	init_mdiClient_misc(HWND hWndCmdBar, HWND hWndToolBar);
	void 	init_message_loop();
	void 	init_sysMenu();
	void 	init_loadPlugins();
	void 	InitSkin();
	void 	setMainFrameCaptionSw(int sw);	//+++ メインフレームのCaption on/off
	void 	initCurrentIcon();				//+++ 現在のスキンのアイコンを(再)設定.

	void	_AnalyzeCommandLine(const CString& strCommandLine);

	// PretranslateMessage用
	BOOL 	OnXButtonUp(WORD wKeys, WORD wButton, CPoint point);
	void 	ExplorerBarAutoShow(MSG *pMsg);
	BOOL 	TranslateMessageToBHO(MSG *pMsg);

	bool 	_OpenAboutFile(CString strFile);
	void	_RefreshFavMenu();

	void 	_ShowBandsFullScreen(BOOL bOn, bool bInit = false);
	void 	_FullScreen			(BOOL bOn);


	// Daba members
	CMenu		m_MainFrameMenu;

	CDonutTabBar 						m_MDITab;
	CCommandBarCtrl2					m_CmdBar;
	CDonutToolBar						m_ToolBar;
	CDonutLinksBarCtrl<CMainFrame>		m_LinkBar;
	CDonutAddressBar					m_AddressBar;
	CDonutSearchBar 					m_SearchBar;
	CDonutReBarCtrl 					m_ReBar;
	CDonutStatusBarCtrl 				m_wndStatusBar;
	CFindBar							m_FindBar;

	CMainOption 						m_MainOption;
	CDonutSecurityZone					m_secZone;
	CChildFrameCommandUIUpdater			m_ChildFrameUIState;

	CSplitterWindow 					m_wndSplit;
	CDonutExplorerBar					m_ExplorerBar;
	CChildFrameClient					m_ChildFrameClient;

	CDonutFavoritesMenu<CMainFrame> 	m_FavoriteMenu;
	CFavoriteGroupMenu<CMainFrame>		m_FavGroupMenu;
	CStyleSheetMenu						m_styleSheetMenu;
	CExplorerMenu						m_ScriptMenu;
	CMenu								m_ScriptMenuMst;

	CRecentClosedTabList	m_RecentClosedTabList;
	//added by minit
	//for Custom Context Dropmenu
	CExplorerMenu			m_DropScriptMenu;
	
	CMenuEncode 			m_MenuEncode;			// for EncodeMenu
	CBingTranslatorMenu		m_TranslateMenu;

	CMenuControl			m_mcCmdBar;
	CMenuControl			m_mcToolBar;

	UINT_PTR				m_nBackUpTimerID;
	CMenuHandle 			m_menuWindow;

	HWND					m_hWndFocus;

	CSimpleMap<UINT, BOOL>	m_mapToolbarOldVisible;

	CMenuDropTargetWindow	m_wndMenuDropTarget;

	CDownloadManager		m_DownloadManager;

#ifdef _DEBUG
//	CDebugWindow			m_wndDebug;
#endif
	CString 				m_strIcon;						//+++ アイコンの名前.
	CString 				m_strIconSm;					//+++ アイコン小の名前.

	int 					m_nMenuBarStyle;				//+++
	BYTE/*bool*/			m_bTray;						//+++ トレイ化してるかどうかのフラグ.
	//bool					m_bMinimized;					//+++ 最小化しているかどうか
	BYTE/*bool*/			m_bOldMaximized;				//+++ 最大化していたかどうか
	BYTE/*bool*/			m_bFullScreen;					//+++ トレイからの復帰用に窓がフルスクリーンかどうかを保持

	bool					m_bCancelRButtonUp;
	bool					m_bWM_TIMER;
	bool					m_bCommandFromChildFrame;

	CString		m_strCommandLine;

	static const UINT		STDBAR_ID[];
	static const UINT		STDBAR_STYLE[];
	//static const UINT		STDBAR_STYLE[];
	static const LPTSTR		STDBAR_TEXT[];

};

/////////////////////////////////////////////////////////////////////////////
