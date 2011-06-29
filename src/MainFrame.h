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


//////////////////////////////////////////////////////////////////////////////
// CMainFrame

class CMainFrame
	:	public CMDIFrameWindowImpl			< CMainFrame >
	,	public CMessageFilter
	,	public CIdleHandler
	,	public CAppCommandHandler			< CMainFrame >
	,	public CUpdateCmdUI 				< CMainFrame >
	,	public CWindowMenuCommandHandler	< CMainFrame >
	,	public CDDEMessageHandler			< CMainFrame >
	,	public CMDIFrameTitleUpsideDownMessageHandlerWeb < CMainFrame >
	,	public CHelpMessageLine 			< CMainFrame >
	,	public CMSMouseWheelMessageHandler	< CMainFrame >
	,	public CMainFrameFileDropTarget 	< CMainFrame >
{
public:
	// Declarelations
	DECLARE_FRAME_WND_CLASS(DONUT_WND_CLASS_NAME, IDR_MAINFRAME)
	//DECLARE_MTL_REBAR_UPDATELAYOUT() //これは廃止してUpdateLayout関数を書き足した

public:
	// Constructor/Destructor
	CMainFrame();
	~CMainFrame();

	void 			UpdateLayout(BOOL bResizeBars = TRUE);
	bool 			OnDDEOpenFile(const CString &strFileName);
	HWND 			OnUserOpenFile(const CString& strFile, DWORD openFlag);	// alternates OpenFile
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

	//+++HICON		LoadIcon4AboutDialog();

	void			SaveAllTab();
	void			RestoreAllTab(LPCTSTR strFilePath = NULL, bool bClose = false);
	void			OpenBingTranslator(const CString& strText);
	void			SetFocusToSearchBarWithSelectedText();

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

	// Message map
	BEGIN_MSG_MAP( CMainFrame )

		COMMAND_EXMENU_RANGE( EXMENU_FIRST, EXMENU_LAST )
		RELECT_STATUSBAR_DRAWITEM_MESSAGE( )

		CHAIN_MSG_MAP_MEMBER( m_FavGroupMenu	)
		CHAIN_MSG_MAP_MEMBER( m_FavoriteMenu	)
		CHAIN_MSG_MAP_MEMBER( m_styleSheetMenu	)
		CHAIN_MSG_MAP_MEMBER( m_ScriptMenu 		)
		CHAIN_MSG_MAP_MEMBER( m_DropScriptMenu  )
		CHAIN_MSG_MAP_MEMBER( m_MenuEncode		)
		CHAIN_MSG_MAP_MEMBER( m_TranslateMenu	)
		CHAIN_MSG_MAP		( CUpdateCmdUI<CMainFrame> )

#ifdef _DEBUG
		COMMAND_ID_HANDLER_EX( ID_DEBUGCOMMAND, OnDebugCommand )
#endif
		//x MESSAGE_HANDLER		( WM_SIZE  , OnSize   )			//+++ 失敗
		MESSAGE_HANDLER 		( WM_CREATE, OnCreate )
		MSG_WM_CLOSE			( OnClose			)
		MSG_WM_DESTROY			( OnDestroy 		)
		MSG_WM_ENDSESSION		( OnEndSession		)
		USER_MSG_WM_NEWINSTANCE ( OnNewInstance 	)
		MSG_WM_USER_MDICHILD	( OnMDIChild		)
		MSG_WM_PAINT			( OnPaint			)
		MSG_WM_USER_OPENFILE	( OnUserOpenFile	)
		MSG_WM_USER_GET_ACTIVE_IWEBBROWSER( )

		USER_MSG_WM_UPDATE_TITLEBAR( UpdateTitleBar )						// UDT DGSTR

		MESSAGE_HANDLER 		 ( MYWM_NOTIFYICON , OnMyNotifyIcon  )		// UDT DGSTR ( SystemTrayIcon Handler )
		COMMAND_ID_HANDLER_EX	 ( ID_GET_OUT	   , OnGetOut		 )		// UDT DGSTR
		USER_MSG_WM_UPDATE_EXPBAR( UpdateExpBar )							// UDT DGSTR

		MSG_WM_USER_GET_ACTIVE_WINDOW( )
		MSG_WM_USER_BROWSER_CAN_SETFOCUS( OnBrowserCanSetFocus )
		MSG_WM_ACTIVATE 	 ( OnActivate )

		MESSAGE_HANDLER 	 ( WM_MENUDRAG	   , OnMenuDrag 	 )
		MESSAGE_HANDLER 	 ( WM_MENUGETOBJECT, OnMenuGetObject )

		// map custom draw notifycation to addressbar
		MESSAGE_HANDLER 	 ( WM_NOTIFY	   , OnNotify		 )

		HANDLE_MESSAGE		 ( WM_MDISETMENU		)
		HANDLE_MESSAGE		 ( WM_SETFOCUS			)				// default handler will set focus to server window

		MSG_WM_TIMER		 ( OnTimer				)
		MSG_WM_MOUSEWHEEL	 ( OnMouseWheel 		)
		DEBUG_CHECK_FOCUS_COMMAND_ID_HANDLER_EX()

		COMMAND_ID_HANDLER	 ( ID_APP_ABOUT 			, OnAppAbout			)
		COMMAND_RANGE_HANDLER( ID_RECENTDOCUMENT_FIRST, ID_RECENTDOCUMENT_LAST, OnFileRecent )
		COMMAND_RANGE_HANDLER( ID_FILE_MRU_FIRST	  , ID_FILE_MRU_LAST	  , OnFileRecent )
		COMMAND_ID_HANDLER	 ( ID_FILE_NEW				, OnFileNew 			)
		COMMAND_ID_HANDLER	 ( ID_FILE_OPEN 			, OnFileOpen			)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_HOME 		, OnFileNewHome 		)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_CLIPBOARD	, OnFileNewClipBoard	)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_CLIPBOARD2	, OnFileNewClipBoard2	)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_CLIPBOARD_EX , OnFileNewClipBoardEx	)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_BLANK		, OnFileNewBlank		)
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_COPY 		, OnFileNewCopy 		)
		COMMAND_ID_HANDLER_EX( ID_FILE_WORKOFFLINE		, OnFileWorkOffline 	)

		COMMAND_ID_HANDLER_EX( ID_EDIT_CUT				, OnEditCut 			)
		COMMAND_ID_HANDLER_EX( ID_EDIT_COPY 			, OnEditCopy			)
		COMMAND_ID_HANDLER_EX( ID_EDIT_PASTE			, OnEditPaste			)
		COMMAND_ID_HANDLER_EX( ID_EDIT_SELECT_ALL		, OnEditSelectAll		)

		COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_SAVE	, OnFavoriteGroupSave	)
		COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_ADD	, OnFavoriteGroupAdd	)	//+++ 追加
		COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_ORGANIZE, OnFavoriteGroupOrganize )

		COMMAND_ID_HANDLER_EX( ID_VIEW_REFRESH_ALL		, OnViewRefreshAll		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_STOP_ALL 		, OnViewStopAll 		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_FULLSCREEN		, OnViewFullScreen		)

		COMMAND_ID_HANDLER_EX( ID_BACKUPOPTION_CHANGED	, OnBackUpOptionChanged )

		COMMAND_ID_HANDLER_EX( ID_TAB_LEFT				, OnTabLeft 			)
		COMMAND_ID_HANDLER_EX( ID_TAB_RIGHT 			, OnTabRight			)
		COMMAND_RANGE_HANDLER( ID_TAB_IDX_1				, ID_TAB_IDX_LAST, OnTabIdx )	//+++ 先頭タブから1..8選択. 9は最後のタブを選択

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

		COMMAND_ID_HANDLER_EX( ID_VIEW_OPTION_DONUT 	, OnViewOptionDonut 	)

		COMMAND_ID_HANDLER_EX( ID_FAVORITE_ADD			, OnFavoriteAdd 		)
		COMMAND_ID_HANDLER_EX( ID_FAVORITE_ORGANIZE 	, OnFavoriteOrganize	)

		COMMAND_ID_HANDLER_EX( ID_SETFOCUS_ADDRESSBAR	, OnSetFocusAddressBar	)
		COMMAND_ID_HANDLER_EX( ID_SETFOCUS_SEARCHBAR	, OnSetFocusSearchBar	)

		COMMAND_ID_HANDLER	 ( ID_WINDOW_CLOSE_ALL		, OnWindowCloseAll		)

		COMMAND_ID_HANDLER	 ( ID_VIEW_IDLE 			, OnViewIdle			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_HOME 			, OnViewHome			)

		// UH
		USER_MEG_WM_MENU_GET_FAV		( OnMenuGetFav		)
		USER_MEG_WM_MENU_GET_FAV_GROUP	( OnMenuGetFavGroup )
		USER_MEG_WM_MENU_GET_SCRIPT 	( OnMenuGetScript	)
		USER_MSG_WM_MENU_GOBACK 		( OnMenuGoBack		)
		USER_MSG_WM_MENU_GOFORWARD		( OnMenuGoForward	)
		USER_MEG_WM_MENU_GET_BINGTRANSLATE( OnMenuGetBingTranslate )

		COMMAND_ID_HANDLER_EX( ID_RESIZED				, OnResized 			);
		COMMAND_ID_HANDLER_EX( ID_FILE_NEW_SELECTED 	, OnFileNewSelectText	)

		MSG_WM_USER_SHOW_TEXT_CHG		( OnShowTextChg  )

		COMMAND_ID_HANDLER_EX( ID_GO_ADDRESSBAR 		, OnGoAddressBar )

		MSG_WM_SYSCOMMAND				( OnSysCommand	  )
		MSG_WM_INITMENUPOPUP			( OnInitMenuPopup )

		COMMAND_ID_HANDLER_EX( ID_POPUP_CLOSE			 , OnPopupClose  )
		COMMAND_ID_HANDLER_EX( ID_TITLE_CLOSE			 , OnTitleClose  )
		COMMAND_ID_HANDLER_EX( ID_DOUBLE_CLOSE			 , OnDoubleClose )
		COMMAND_ID_HANDLER_EX( ID_URLACTION_COOKIES_BLOCK, OnCookiesIE6  )
		COMMAND_ID_HANDLER_EX( ID_URLACTION_COOKIES_HI	 , OnCookiesIE6  )
		COMMAND_ID_HANDLER_EX( ID_URLACTION_COOKIES_MIDHI, OnCookiesIE6  )
		COMMAND_ID_HANDLER_EX( ID_URLACTION_COOKIES_MID  , OnCookiesIE6  )
		COMMAND_ID_HANDLER_EX( ID_URLACTION_COOKIES_LOW  , OnCookiesIE6  )
		COMMAND_ID_HANDLER_EX( ID_URLACTION_COOKIES_ALL  , OnCookiesIE6  )

		MSG_WM_USER_HILIGHT 			( OnHilight 	   );
		MSG_WM_USER_FIND_KEYWORD		( OnFindKeyWord    );
		MSG_WM_USER_WINDOWS_CLOSE_CMP	( OnWindowCloseCmp );

		COMMAND_ID_HANDLER( ID_SEARCHBAR_SEL_DOWN, OnSearchBarCmd )
		COMMAND_ID_HANDLER( ID_SEARCHBAR_SEL_UP  , OnSearchBarCmd )
		COMMAND_ID_HANDLER( ID_SEARCHBAR_HILIGHT , OnSearchBarCmd )

		MSG_WM_USER_CHANGE_CSS			( OnChangeCSS )
		COMMAND_ID_HANDLER( ID_PRIVACYREPORT, OnPrivacyReport )
		// ^^^

		//minit
		COMMAND_ID_HANDLER( ID_SPECIAL_HOME 	, OnSpecialKeys    )
		COMMAND_ID_HANDLER( ID_SPECIAL_END		, OnSpecialKeys    )
		COMMAND_ID_HANDLER( ID_SPECIAL_PAGEUP	, OnSpecialKeys    )
		COMMAND_ID_HANDLER( ID_SPECIAL_PAGEDOWN , OnSpecialKeys    )
		COMMAND_ID_HANDLER( ID_SPECIAL_UP		, OnSpecialKeys    )
		COMMAND_ID_HANDLER( ID_SPECIAL_DOWN 	, OnSpecialKeys    )
		COMMAND_ID_HANDLER( ID_SECURITYREPORT	, OnSecurityReport )
		COMMAND_ID_HANDLER_EX( ID_SETFOCUS_SEARCHBAR_ENGINE, OnSetFocusSearchBarEngine )

		USER_MEG_WM_MENU_REFRESH_SCRIPT 	( OnMenuRefreshScript	)
		USER_MSG_WM_MENU_RESTRICT_MESSAGE	( OnRestrictMessage 	)
		USER_MSG_WM_GET_FAVORITEFILEPATH	( OnGetFavoriteFilePath )

		COMMAND_ID_HANDLER( ID_WINDOW_THUMBNAIL 		  , OnWindowThumbnail			)

		COMMAND_ID_HANDLER( ID_SPECIAL_REFRESH_SEARCHENGIN, OnSpecialRefreshSearchEngine) 	//長いなぁ
		COMMAND_ID_HANDLER( ID_RECENT_DOCUMENT			  , OnMenuRecentLast			)

		COMMAND_RANGE_HANDLER_EX(ID_INSERTPOINT_SEARCHENGINE, ID_INSERTPOINT_SEARCHENGINE_END, OnSearchWeb_engineId)	//+++


		USER_MSG_WM_MENU_RECENTDOCUMENT( OnMenuRecentDocument )

		COMMAND_ID_HANDLER( ID_STYLESHEET_USE_USERS, OnUseUserStyleSheet )
		COMMAND_ID_HANDLER( ID_STYLESHEET_SET_USERS, OnSetUserStyleSheet )

		USER_MSG_WM_CHANGE_SKIN 		( OnSkinChange		 )
		USER_MSG_WM_COMMAND_DIRECT		( OnCommandDirect	 )
		USER_MSG_WM_REFRESH_EXPBAR		( OnRefreshExpBar	 )
		USER_MSG_WM_SEARCH_WEB_SELTEXT	( OnSearchWebSelText )
		USER_MSG_WM_SET_EXPROPERTY		( OnSetExProperty	 )

		COMMAND_ID_HANDLER( ID_TABLIST_DEFAULT, 	OnTabListDefault	)
		COMMAND_ID_HANDLER( ID_TABLIST_VISIBLE, 	OnTabListVisible	)
		COMMAND_ID_HANDLER( ID_TABLIST_ALPHABET,	OnTabListAlphabet	)
		COMMAND_ID_HANDLER( ID_MAINFRAME_MINIMIZE,	OnMainFrameMinimize )
		COMMAND_ID_HANDLER( ID_MAINFRAME_MAXIMIZE,	OnMainFrameMaximize )
		COMMAND_ID_HANDLER( ID_MAINFRAME_NORM_MAX_SIZE,OnMainFrameNormMaxSize )
		COMMAND_ID_HANDLER( ID_SHOW_EXMENU, 		OnShowExMenu		)
		COMMAND_ID_HANDLER( ID_SELECT_USERFOLDER,	OnSelectUserFolder	)
		COMMAND_ID_HANDLER( ID_SEARCH_HISTORY,		OnSearchHistory 	)
		COMMAND_ID_HANDLER( ID_JUMP_WEBSITE,		OnJumpToWebSite 	)

		#define WM_PLUGIN_COMMAND	ID_PLUGIN_COMMAND
		MESSAGE_HANDLER 			( WM_PLUGIN_COMMAND, OnPluginCommand )
		USER_MSG_WM_OPEN_WITHEXPROP ( OnOpenWithExProp	)
		USER_MSG_WM_GET_SEARCHBAR	( OnGetSearchBar	)
		USER_MSG_WM_SHOW_TOOLBARMENU( OnShowToolBarMenu )
		COMMAND_ID_HANDLER			( ID_SHOW_ACTIVEMENU, OnShowActiveMenu )

		// UDT DGSTR //処理が同一なので一つの関数にまとめました( minit )
		COMMAND_ID_HANDLER( ID_FAVTREE_BAR_STANDARD  , OnFavoriteExpBar )
		COMMAND_ID_HANDLER( ID_FAVTREE_BAR_GROUP	 , OnFavoriteExpBar )
		COMMAND_ID_HANDLER( ID_FAVTREE_BAR_SCRIPT	 , OnFavoriteExpBar )
		COMMAND_ID_HANDLER( ID_FAVTREE_BAR_USER 	 , OnFavoriteExpBar )
		COMMAND_ID_HANDLER( ID_FAVTREE_BAR_MYCOMPUTER, OnFavoriteExpBar )
		COMMAND_ID_HANDLER( ID_FAVTREE_BAR_HISTORY	 , OnFavoriteExpBar )
		COMMAND_ID_HANDLER( ID_FILE_OPEN_TABLIST	 , OnFileOpenTabList)
		COMMAND_ID_HANDLER( ID_OPEN_EXE_DIR			 , OnOpenExeDir 	)
		COMMAND_ID_HANDLER_EX( ID_REGISTER_AS_BROWSER, OnRegisterAsBrowser )
		COMMAND_ID_HANDLER_EX( ID_SAVEIMAGE			 , OnSaveImage )

		CHAIN_COMMANDS_MEMBER	( m_ReBar		)
		CHAIN_COMMANDS_MEMBER	( m_MainOption	)
		CHAIN_COMMANDS_MEMBER	( m_secZone 	)
		CHAIN_MSG_MAP_MEMBER	( m_DownloadManager )
		CHAIN_COMMANDS			( CWindowMenuCommandHandler<CMainFrame> )
		CHAIN_COMMANDS			( CAppCommandHandler<CMainFrame>		)
		CHAIN_COMMANDS_TO_EXPLORERBAR( m_ExplorerBar		)

		CHAIN_MDI_CHILD_COMMANDS( )
		COMMAND_ID_HANDLER_EX	( ID_VIEW_OPTION, OnViewOption	)

		CHAIN_MSG_MAP	( CDDEMessageHandler<CMainFrame>			)
		CHAIN_MSG_MAP	( CMDIFrameTitleUpsideDownMessageHandlerWeb<CMainFrame> )
		CHAIN_MSG_MAP	( CHelpMessageLine<CMainFrame>				)
		CHAIN_MSG_MAP	( CMSMouseWheelMessageHandler<CMainFrame>	)

		REFLECT_CHEVRONPUSHED_NOTIFICATION( )
		CHAIN_MSG_MAP	( baseClass )
		REFLECT_NOTIFICATIONS( )	// must be last

	ALT_MSG_MAP( 1 )
		MSG_WM_ERASEBKGND( OnMDIClientEraseBkgnd )
		MSG_WM_SIZE( OnMDIClientSize )
	END_MSG_MAP()


public:
	BEGIN_UPDATE_COMMAND_UI_MAP( CMainFrame )

		CHAIN_UPDATE_COMMAND_UI_MEMBER( m_MainOption  )
		CHAIN_UPDATE_COMMAND_UI_MEMBER( m_secZone	  )
		CHAIN_UPDATE_COMMAND_UI_MEMBER( m_ExplorerBar )
		CHAIN_MDI_CHILD_UPDATE_COMMAND_UI() 						// first of all

		bool bActiveChild = (MDIGetActive() != NULL);

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
		//UPDATE_COMMAND_UI_POPUP_ENABLE_IF 	( ID_RECENT_DOCUMENT,CMainOption::s_pMru->m_arrDocs.GetSize( ))
		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP	( ID_RECENT_DOCUMENT, _GetRecentCount(), _GetRecentCount() )

		// Edit menu
		//		UPDATE_COMMAND_UI_ENABLE_IF( ID_EDIT_CUT		, bActiveChild )
		//		UPDATE_COMMAND_UI_ENABLE_IF( ID_EDIT_COPY		, bActiveChild )
		//		UPDATE_COMMAND_UI_ENABLE_IF( ID_EDIT_PASTE		, bActiveChild )
		//		UPDATE_COMMAND_UI_ENABLE_IF( ID_EDIT_SELECT_ALL , bActiveChild )

		// アドレスバーかサーチバーにフォーカスがある or OLEコマンドが使用可能かどうか
		CEdit editAddress = m_AddressBar.GetEditCtrl();
		CEdit editSearch  = m_SearchBar.GetEditCtrl();
		bool  bFocus	  = (::GetFocus() == editAddress || ::GetFocus() == editSearch);
		bool  bCut		  = false, bCopy = false, bPaste = false, bSelectAll = false;
		if (bActiveChild) {
			CWebBrowser2 wb = DonutGetIWebBrowser2( MDIGetActive() );
		  #if 1	//+++ 実はQUeryStatusWBがバグって、失敗しててもゴミを返していた...で、たまたま表示されることが多かったのかも.
				//+++ が、そもそも必ず失敗するのが解せない... が、不便なので、暫定対処
			bCut = true, bCopy = true, bPaste = true, bSelectAll = true;
		  #else
			bCut			= (wb.QueryStatusWB(OLECMDID_CUT)		& OLECMDF_ENABLED) != 0;
			bCopy			= (wb.QueryStatusWB(OLECMDID_COPY)		& OLECMDF_ENABLED) != 0;
			bPaste			= (wb.QueryStatusWB(OLECMDID_PASTE) 	& OLECMDF_ENABLED) != 0;
			bSelectAll		= (wb.QueryStatusWB(OLECMDID_SELECTALL) & OLECMDF_ENABLED) != 0;
		  #endif
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

		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP	( ID_VIEW_BACK, bActiveChild, bActiveChild		)			// with popup
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

		//UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_VIEW_ENCODECONTAINER, bActiveChild, bActiveChild ) 	// minit

		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_VIEW_SOURCE		, bActiveChild			)
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_VIEW_FULLSCREEN	, IsFullScreen()		)

		// Favorite menu
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_FAVORITE_ADD		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_FAVORITE_GROUP_SAVE, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_FAVORITE_GROUP_ADD , bActiveChild			)
		// Tool menu
		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_DLCTL_DLIMAGES, bActiveChild, bActiveChild ) 			// with poup
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_CHG_MULTI	, bActiveChild			)					// UDT DGSTR
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_ON_OFF_MULTI , bActiveChild			)					// UDT UH
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_BGSOUNDS 	, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_VIDEOS		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_DLCTL_RUNACTIVEXCTLS, bActiveChild, bActiveChild )		// with popup
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_CHG_SECU 	, bActiveChild			)					// UDT DGSTR
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_ON_OFF_SECU	, bActiveChild			)					// UDT UH
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_DLACTIVEXCTLS, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_SCRIPTS		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DLCTL_JAVA 		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_DOCHOSTUI_OPENNEWWIN, bActiveChild 		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_MESSAGE_FILTER 	, bActiveChild			)					// UH
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_MOUSE_GESTURE		, bActiveChild			)					// UH
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_BLOCK_MAILTO		, bActiveChild			)					// UH
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_EDIT_IGNORE		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_TITLE_COPY 		, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_COPY_TITLEANDURL	, bActiveChild			)					//minit
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_URL_COPY			, bActiveChild			)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_EDIT_CLOSE_TITLE	, bActiveChild			)					// UDT DGSTR

		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_STYLESHEET_USE_USERS, CStyleSheetOption::GetUseUserSheet() )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_STYLESHEET_SET_USERS, CStyleSheetOption::s_bSetUserSheet	)

		// Help
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_APP_HELP			, _IsExistHTMLHelp()	)					// UDT DGSTR

		// Misc
		// Window menu
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_WINDOW_CLOSE_ALL		, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_WINDOW_CLOSE_EXCEPT	, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_WINDOW_REFRESH_EXCEPT	, bActiveChild		)					// U.H
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_TAB_LEFT				, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_TAB_RIGHT				, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_WINDOW_RESTORE, bActiveChild, bActiveChild ) 			// with popup
		UPDATE_COMMAND_UI_ENABLE_IF 	( ATL_IDS_SCPREVWINDOW		, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ATL_IDS_SCNEXTWINDOW		, bActiveChild		)
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_WINDOW_CASCADE  , bActiveChild && !CMainOption::s_bTabMode )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_WINDOW_TILE_HORZ, bActiveChild && !CMainOption::s_bTabMode )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_WINDOW_TILE_VERT, bActiveChild && !CMainOption::s_bTabMode )
		UPDATE_COMMAND_UI				( ID_WINDOW_ARRANGE  , OnUpdateWindowArrange	)					// to update Window Menu
		// UPDATE_COMMAND_UI_ENABLE_IF( ID_WINDOW_ARRANGE, bActiveChild && !CMainOption::s_bTabMode )
		// Misc
		// UPDATE_COMMAND_UI_SETTEXT( ID_DEFAULT_PANE, _T("レディ" ))
		UPDATE_COMMAND_UI				( IDC_PROGRESS		, OnUpdateProgressUI		)
		UPDATE_COMMAND_UI				( ID_SECURE_PANE	, OnUpdateStautsIcon		)
		UPDATE_COMMAND_UI				( ID_PRIVACY_PANE	, OnUpdateStautsIcon		)

		// UH
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_POPUP_CLOSE	, CIgnoredURLsOption::s_bValid )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_TITLE_CLOSE	, CCloseTitlesOption::s_bValid )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_DOUBLE_CLOSE	, CIgnoredURLsOption::s_bValid && CCloseTitlesOption::s_bValid )

		// UH
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_BLOCK, ChkCookies(ID_URLACTION_COOKIES_BLOCK) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_HI	, ChkCookies(ID_URLACTION_COOKIES_HI   ) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_MIDHI, ChkCookies(ID_URLACTION_COOKIES_MIDHI) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_MID	, ChkCookies(ID_URLACTION_COOKIES_MID  ) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_LOW	, ChkCookies(ID_URLACTION_COOKIES_LOW  ) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_ALL	, ChkCookies(ID_URLACTION_COOKIES_ALL  ) )
		UPDATE_COMMAND_UI_SETCHECK_IF	( ID_URLACTION_COOKIES_CSTM , ChkCookies(ID_URLACTION_COOKIES_CSTM ) )

		UPDATE_COMMAND_UI_POPUP_ENABLE_IF( ID_STYLESHEET_BASE	, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_CSS_DROPDOWN		, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_VIEW_UP			, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_EDIT_FIND_MAX		, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_VIEW_SOURCE_SELECTED,bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_FILE_PRINT_PREVIEW , bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_SEARCH_PAGE		, bActiveChild )							//検索バーのページ内検索ボタン
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_SEARCHBAR_SEL_UP	, bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_SEARCHBAR_SEL_DOWN , bActiveChild )
		UPDATE_COMMAND_UI_ENABLE_IF 	( ID_SEARCHBAR_HILIGHT	, bActiveChild )
		//minit
		//CHAIN_UPDATE_COMMAND_UI_MEMBER( m_DockingPluginManager )

	END_UPDATE_COMMAND_UI_MAP()

private:
	// Overrides
	//各ウィンドウへ(主にキー)メッセージを転送する
	virtual BOOL PreTranslateMessage(MSG *pMsg);

	HWND 		UserOpenFile(const CString& strFile, DWORD openFlag, int dlCtrlFlag = -1, int extendedStyleFlags = -1);

	BOOL 		 OnXButtonUp(WORD wKeys, WORD wButton, CPoint point);
	void 		 ExplorerBarAutoShow(MSG *pMsg);
	BOOL 		 OnMButtonHook(MSG* pMsg);
	// Mouse Gesture
	BOOL 		 OnRButtonHook(MSG *pMsg);
	DWORD 		 GetMouseWheelCmd(short nWheel);
	DWORD 		 GetMouseButtonUpCmd(UINT uMsg, UINT nXButton = 0);
	virtual BOOL OnIdle();
	void 		 _OnIdleCritical();

#ifdef _DEBUG
	void		OnDebugCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
#endif

  #if 0	//+++	失敗
	LRESULT 	OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  #endif
  #if 1	//+++
	void 		ShowWindow_Restore(bool flag);
  #endif
	LRESULT 	UpdateTitleBar(LPCTSTR lpszStatusBar, DWORD dwReserved);

	LRESULT 	OnMenuGetFav();
	LRESULT 	OnMenuGetFavGroup();
	LRESULT 	OnMenuGetScript();
	LRESULT 	OnMenuGoBack(HMENU hMenu);
	LRESULT 	OnMenuGoForward(HMENU hMenu);
	LRESULT		OnMenuGetBingTranslate();
	LRESULT 	OnMenuRefreshScript(BOOL bInit);
	LRESULT 	OnRestrictMessage(BOOL bOn);
	LRESULT		OnGetFavoriteFilePath(int nID);
	void		IfTrayRestoreWindow() {	if (m_bTray) OnGetOut(0, 0, 0); }	//+++ トレイ状態だったら窓を復活.
	void 		OnGetOut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT 	OnMyNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/);
	LRESULT 	UpdateExpBar(LPCTSTR lpszExpBar, DWORD dwReserved);
	LRESULT 	OnRefreshExpBar(int nType);

	LRESULT 	OnGetSearchBar() { return (LRESULT) &m_SearchBar; }
	LRESULT 	OnOpenWithExProp(_EXPROP_ARGS *pArgs);
	HWND 		OpenInternetShortcut(CString strUrlFile, DWORD dwOpenFlag);
	HWND 		OpenUrlWithExProp(CString strUrl, DWORD dwOpenFlag, DWORD dwExProp, DWORD dwExProp2);
	HWND 		OpenUrlWithExProp(CString strUrl, DWORD dwOpenFlag, CString strIniFile, CString strSection = DONUT_SECTION);

	HWND 		OpenExPropertyActive(CString &strUrl, DWORD dwExProp, DWORD dwExProp2, DWORD dwOpenFlag);
	HWND 		OpenExPropertyNew(CString &strUrl, DWORD dwExProp, DWORD dwExProp2, DWORD dwOpenFlag);
	HWND 		OpenExPropertyNot(CString &strUrl, DWORD dwOpenFlag);

	LRESULT 	OnMenuDrag(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT 	OnMenuGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	//////////////////////////////////////////////////////////////////
	// to avoid the flicker on resizing
	BOOL		OnMDIClientEraseBkgnd(CDCHandle dc);
	void	 	OnMDIClientSize(UINT nType, CSize size);

	//////////////////////////////////////////////////////////////////
	// custom draw of addressbar
	LRESULT 	OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	//////////////////////////////////////////////////////////////////
	// the custom message from MDI child
	void 		OnMDIChild(HWND hWnd, UINT nCode);
	BOOL 		OnBrowserCanSetFocus();
	void 		OnParentNotify(UINT fwEvent, UINT idChild, LPARAM lParam);
	void 		OnDestroy();
	void 		OnEndSession(BOOL wParam, UINT lParam);
	void 		OnActivate(UINT nState, BOOL bMinimized, HWND hWndOther);
	LRESULT 	OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/);
	void		OnPaint(CDCHandle /*dc*/);

	void 		init_menus_infomation();
	HWND		init_commandBarWindow();
	HWND		init_toolBar();
	HWND		init_addressBar();
	HWND		init_searchBar();
	HWND		init_tabCtrl();
	HWND		init_linkBar();
	void		init_rebar();
	void		init_statusBar();
	void 		init_pluginManager();
	void 		init_band_position( HWND cmdBar, HWND toolBar, HWND addressBar, HWND mdiTab, HWND linkBar, HWND searchBar );
	void 		init_loadStatusBarState();
	void 		init_mdiClientWindow();
	void 		init_splitterWindow();
	void 		init_explorerBar();
	void		init_mdiClient_misc(HWND hWndCmdBar, HWND hWndToolBar);
	void 		init_message_loop();
	void 		init_sysMenu();
	void 		init_loadPlugins();

	BOOL 		_IsRebarBandLocked();
	LRESULT 	OnViewToolBarLock	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void 		OnExplorerBarAutoShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT 	OnMouseWheel		(UINT fwKeys, short zDelta, CPoint point);
	LRESULT 	OnFileRecent		(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnViewIdle			(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/) { _OnIdleCritical(); return 0; }
	void 		OnNewInstance		(ATOM nAtom);			// WM_NEWINSTANCE
	void 		OnClose();
	void 		SetHideTrayIcon();
	void 		DeleteTrayIcon();		//+++ トレイ化の終了/トレイアイコンの削除.
	void 		DelTempFiles();
	void 		DelHistory();


	////////////////////////////////////////////////////////////////////////////////
	//ファイルメニュー
	//　コマンドハンドラ
	////////////////////////////////////////////////////////////////////////////////

	////新規作成
	void 		OnFileNewHome		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);		//ポームページ
	void 		OnFileNewCopy		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);		//現在のページ
	LRESULT 	OnFileNew			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void 		OnFileNewBlank		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);		//空白ページ
	void 		OnFileNewClipBoard	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);		//クリップボード
#if 0
	LRESULT 	OnFileOpenDef		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
#endif
	LRESULT		OnFileOpenTabList	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void 		OnViewHome			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnResized			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		ShowLinkText		(BOOL bShow);
	void 		OnFileNewSelectText	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnShowTextChg		(BOOL bShow);
	void 		OnGoAddressBar		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT 	OnSysCommand		(UINT nID, CPoint point);
	LRESULT 	OnPopupClose		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT 	OnTitleClose		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT 	OnDoubleClose		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT 	OnPrivacyReport		(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnCookiesIE6		(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	LRESULT 	OnHilight			(LPCTSTR lpszKeyWord);
	LRESULT 	OnFindKeyWord		(LPCTSTR lpszKeyWord, BOOL bBack, long Flags = 0);
	LRESULT 	OnWindowCloseCmp	(int nTarIndex, BOOL bLeft);
   #ifndef NO_STYLESHEET
	LRESULT 	OnChangeCSS			(LPCTSTR lpszStyleSheet);
   #endif
	LRESULT 	OnSearchBarCmd		(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnInitMenuPopup		(HMENU hMenuPopup, UINT uPos, BOOL bSystemMenu);
	LRESULT 	OnFavoriteExpBar	(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void 		OnRegisterAsBrowser	(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnFileNewClipBoard2	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnFileNewClipBoardEx(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnFileWorkOffline	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT 	OnFileOpen			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnFileExit			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void 		OnEditCut			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnEditCopy			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnEditPaste			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnEditSelectAll		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT 	OnViewSearchBar		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnViewCommandBar	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnViewToolBar		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnViewAddressBar	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnViewLinkBar		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnViewGoButton		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnViewTabBar		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnViewTabBarMulti	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnViewAddBarDropDown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnViewStatusBar		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnViewToolBarCust	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void 		OnFavoriteAdd		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnFavoriteOrganize	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	void 		OnBackUpOptionChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnTimer				(UINT_PTR nIDEvent, TIMERPROC dmy = 0);
	void 		OnViewOptionDonut	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		ShowNewStyleDonutOption();
	void 		OnViewOption		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT 	OnWindowCloseAll	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	void 		_ShowBandsFullScreen(BOOL bOn, bool bInit = false);
	void 		_FullScreen			(BOOL bOn);
	void 		OnViewFullScreen	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	void 		OnFavoriteGroupSave	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnFavoriteGroupAdd	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnFavoriteGroupOrganize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnViewRefreshAll	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnViewStopAll		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 		OnSetFocusAddressBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);
	void 		OnSetFocusSearchBar	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);
	void 		OnSetFocusSearchBarEngine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);
	void 		OnTabLeft			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);
	void 		OnTabRight			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);

  #if 1 //+++
	//+++ 先頭タブから1..8選択. 9は最後のタブを選択
	LRESULT 	OnTabIdx		(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL & /*bHandled*/);
  #endif


	// Implementation

	void 		_WriteProfile();

	///+++
	void _SaveGroupOption(const CString &strFileName, bool bDelay=false);
	void _LoadGroupOption(const CString &strFileName, bool bClose);

	bool 		_IsExistHTMLHelp();
	bool 		ChkCookies(UINT nID);
	void 		OnUpdateWindowArrange(CCmdUI *pCmdUI);
	void 		OnUpdateProgressUI(CCmdUI *pCmdUI);
	void 		OnUpdateStautsIcon(CCmdUI *pCmdUI);
	bool 		_IsClipboardAvailable();
	void 		_FocusChecker();
	LRESULT 	OnSpecialKeys		(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnSecurityReport	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	BOOL 		TranslateMessageToBHO(MSG *pMsg);

	LRESULT 	OnWindowThumbnail	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	LRESULT 	OnSpecialRefreshSearchEngine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	BOOL 		_Load_OptionalData(CChildFrame *pChild, const CString &strFileName, CString &strSection);
	BOOL 		_Load_OptionalData2(CChildFrame *pChild,
							 std::vector<std::pair<CString, CString> > &ArrayFore,
							 std::vector<std::pair<CString, CString> > &ArrayBack);
	LRESULT 	OnMenuRecentDocument(HMENU hMenu);
	LRESULT 	OnMenuRecentLast	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	int 		_GetRecentCount();
	LRESULT 	_OnMDIActivate(HWND hWndActive);
	LRESULT 	OnTabListDefault	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnTabListVisible	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnTabListAlphabet	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnShowActiveMenu	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnMainFrameMinimize	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnMainFrameMaximize	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnMainFrameNormMaxSize(WORD/*wNotifyCode*/,WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
   #ifndef NO_STYLESHEET
	LRESULT 	OnUseUserStyleSheet	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnSetUserStyleSheet	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
   #endif
	LRESULT 	OnShowExMenu		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnSelectUserFolder	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnSearchHistory		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnJumpToWebSite		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnOpenExeDir		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);	//+++
	LRESULT 	OnCommandDirect(int nCommand, LPCTSTR lpstr);
	void		OnSaveImage(UINT uNotifyCode, int nID, CWindow wndCtl);

	void 		InitSkin();
	HRESULT 	OnSkinChange();

	void 		setMainFrameCaptionSw(int sw);	//+++ メインフレームのCaption on/off
	void 		initCurrentIcon();				//+++ 現在のスキンのアイコンを(再)設定.
	void 		setMainFrameBg(int bgColor);	//+++ メインフレームのBg設定.
	bool 		drawMainFrameBg(HDC hDC);		//+++ bg描画
	void 		DrawBmpBg_Tile(HDC hDC);		//+++ bg描画(敷き詰めるタイプ)
	//x void 	DrawBmpBg_Stretch(HDC hDC);		//+++ 元画像を拡大して表示.

  #if 1	// saitama専用だったのを外部ファイル読込にして汎用化.
	HWND 			_OpenAboutFile(CString strFile);
  #endif
	HRESULT 		LoadWebBrowserFromStream(IWebBrowser *pWebBrowser, IStream *pStream);
	LRESULT 		OnShowToolBarMenu();
	HRESULT 		OnSearchWebSelText(LPCTSTR lpstrText, LPCTSTR lpstrEngine);
	//+++
	void 			OnSearchWeb_engineId(UINT code, int id, HWND hWnd);

	LRESULT 		OnSetExProperty(LPCTSTR lpstrUrlFile);


private:
	typedef CMDIFrameWindowImpl<CMainFrame>  baseClass;

	// Constants
	enum {
		SHDVID_SSLSTATUS	=	33, 					//SSL証明書表示用
		ENT_READ_ACCEL		=	 2,
		STDBAR_COUNT		=	 6,
	};

	enum {
		_nPosFavoriteMenu	   = 3, _nPosFavGroupMenu	=  2, _nPosViewMenu   = 2,
		_nPosFavoriteMenuOfTab = 2, _nPosMRU			= 12, _nPosWindowMenu = 5, _nWindowMenuItemCount = 14,
		_nPosCssMenu		   = 4, _nPosSubCssMenu 	= 14, _nPosEncodeMenu = 2, _nPosEncodeMenuSub	 = 18
	};

	struct 			_Function_EnumChildSaveOption;
	struct 			_ExtendProfileInfo;

	///////////////////////////////////////////////////////////
	// CSaveGroupOptionToFile
	class  	CSaveGroupOptionToFile {
	public:
		CSaveGroupOptionToFile();
		~CSaveGroupOptionToFile();
		void Run(CMainFrame* pMainWnd, const CString &strFileName, bool bDelay); // modified by minit

	private:
		static UINT __stdcall WriteIniFile_Thread(void* pParam);
		void WriteIniFile();

	private:
		HANDLE* 						m_phThread;
		BOOL							m_bOldMaximized;
		bool							m_bSaveTravelLog;
		int 							m_nIndex;
		int 							m_nActiveIndex;
		CString 						m_strFileName;
		std::list<SDfgSaveInfo> 		m_dfgSaveInfoList;
	};
	///////////////////////////////////////////////////////////


private:

	bool	_IsSelectedTextInner();
	void	_RefreshFavMenu();
	void 	_RemoveShortcutTmpDirectory();


	// Daba members
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

	CSplitterWindow 					m_wndSplit;
	CDonutExplorerBar					m_ExplorerBar;

	CDonutFavoritesMenu<CMainFrame> 	m_FavoriteMenu;
	CFavoriteGroupMenu<CMainFrame>		m_FavGroupMenu;
	CStyleSheetMenu						m_styleSheetMenu;
	CExplorerMenu						m_ScriptMenu;
	CMenu								m_ScriptMenuMst;

	//added by minit
	//for Custom Context Dropmenu
	CExplorerMenu						m_DropScriptMenu;
	
	CMenuEncode 						m_MenuEncode;			// for EncodeMenu
	CBingTranslatorMenu					m_TranslateMenu;

	CMenuControl						m_mcCmdBar;
	CMenuControl						m_mcToolBar;

	CContainedWindow					m_wndMDIClient;

	UINT_PTR							m_nBackUpTimerID;
	CMenuHandle 						m_menuWindow;

	HWND								m_hWndFocus;

	CSimpleMap<UINT, BOOL>				m_mapToolbarOldVisible;

	CMenuDropTargetWindow				m_wndMenuDropTarget;

	CDownloadManager					m_DownloadManager;

	HANDLE								m_hGroupThread;
#ifdef _DEBUG
	CDebugWindow						m_wndDebug;
#endif
	CSaveGroupOptionToFile				m_saveGroupOptionToFile;		//+++ 自動セーブ処理.
	CBitmap 							m_bmpBg;						//+++ BG画像
	//CBitmap							m_bmpBgStretch;					//+++ BG画像描画用
	CString 							m_strIcon;						//+++ アイコンの名前.
	CString 							m_strIconSm;					//+++ アイコン小の名前.

	int 								m_OnUserOpenFile_nCmdShow;		//+++ 直前に実行したOnUserOpenFile()で選択されたnCmdShowの控え.
	int 								m_nMenuBarStyle;				//+++
	int/*COLORREF*/ 					m_nBgColor; 					//+++ 背景色
	BYTE/*bool*/						m_bTray;						//+++ トレイ化してるかどうかのフラグ.
	//bool								m_bMinimized;					//+++ 最小化しているかどうか
	BYTE/*bool*/						m_bOldMaximized;				//+++ 最大化していたかどうか
	BYTE/*bool*/						m_bFullScreen;					//+++ トレイからの復帰用に窓がフルスクリーンかどうかを保持

	BYTE/*BOOL*/						m_bShow;
	bool								m_bCancelRButtonUp;
	bool								m_bWM_TIMER;

	static const UINT					STDBAR_ID[];
	static const UINT					STDBAR_STYLE[];
	//static const UINT					STDBAR_STYLE[];
	static const LPTSTR					STDBAR_TEXT[];

};

/////////////////////////////////////////////////////////////////////////////
