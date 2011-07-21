/**
 *	@file	ChildFrame.h
 *	@brief	タブページ１つの処理.
 */

#pragma once

#include "PluginManager.h"
#include "MtlBrowser.h"
#include "MDIChildUserMessenger.h"

#include "FavoritesMenu.h"
#include "DonutView.h"
#include "DonutAddressBar.h"
#include "DonutTabBar.h"

#include "option/DLControlOption.h"
#include "option/StartUpOption.h"
#include "option/DonutConfirmOption.h"
#include "option/IgnoreURLsOption.h"
#include "option/CloseTitleOption.h"

#include "DonutPFunc.h" 		//+++
//#include "IEToolBar.h"		//+++


/////////////////////////////////////////////////////////////
// CChildFrame

class CChildFrame
		: public CMDIChildWindowImplFixed<CChildFrame, 2>
		, public IWebBrowserEvents2Impl<CChildFrame, ID_DONUTVIEW>
		, public CWebBrowserCommandHandler<CChildFrame>
		, public CUpdateCmdUI<CChildFrame>
		, public CMDIChildUserMessenger<CChildFrame>
		, public CWebBrowser2
{
public:
	typedef CMDIChildWindowImplFixed<CChildFrame, 2> baseClass;
	// Declarations

	DECLARE_FRAME_WND_CLASS(NULL, IDR_MDICHILD)
	DECLARE_FRAME_WND_MENU(NULL)

	enum {	//+++ 実際に参照があるのは、最期のNOREFRESHのみだが念のため.
		FLAG_SE_DLIMAGES		=	0x00000001,
		FLAG_SE_VIDEOS			=	0x00000002,
		FLAG_SE_BGSOUNDS		=	0x00000004,
		FLAG_SE_RUNACTIVEXCTLS	=	0x00000008,
		FLAG_SE_DLACTIVEXCTLS	=	0x00000010,
		FLAG_SE_SCRIPTS 		=	0x00000020,
		FLAG_SE_JAVA			=	0x00000040,

		FLAG_SE_NAVIGATELOCK	=	0x00000080,
	  #ifdef USE_ORG_UNDONUT_INI				//+++ unDonut オリジナル版の値
		FLAG_SE_MSGFILTER		=	0x00000100,
		FLAG_SE_MOUSEGESTURE	=	0x00000200,
		FLAG_SE_BLOCKMAILTO 	=	0x00000400,
		FLAG_SE_FLATVIEW		=	0x00000800, //+++ オリジナルにはないが設定.
	  #else 									//+++ unDonut+ の値.
		FLAG_SE_FLATVIEW		=	0x00000100,
		FLAG_SE_MSGFILTER		=	0x00000200,
		FLAG_SE_MOUSEGESTURE	=	0x00000400,
		FLAG_SE_BLOCKMAILTO 	=	0x00000800,
	  #endif

		FLAG_SE_VIEWED			=	0x00001000,

		FLAG_SE_REFRESH_NONE	=	0x00010000,
		FLAG_SE_REFRESH_15		=	0x00020000,
		FLAG_SE_REFRESH_30		=	0x00040000,
		FLAG_SE_REFRESH_1M		=	0x00080000,
		FLAG_SE_REFRESH_2M		=	0x00100000,
		FLAG_SE_REFRESH_5M		=	0x00200000,
		FLAG_SE_REFRESH_USER	=	0x00400000,

		FLAG_SE_NOREFRESH		=	0x01000000,
	};

	bool	m_bAllowNewWindow;	// 新規タブで開くのを許可するかどうか

public:
	// Constructor/Destructor
	CChildFrame(  CDonutTabBar &MDITab
				, CDonutAddressBar &addressbar
				, bool bNewWindow2
				, DWORD dwDefaultDLControlFlags
				, DWORD dwDefaultExtendedStyleFlags
			   );
	~CChildFrame();

	static CChildFrame *NewWindow(
			HWND				hWndMDIClient,
			CDonutTabBar &		tabMDI,
			CDonutAddressBar &	adBar,
			bool				bNewWindow2 = false,
			DWORD				dwDLFlags	= CDLControlOption::s_dwDLControlFlags,
			DWORD				dwESFlags   = CDLControlOption::s_dwExtendedStyleFlags);

	virtual void OnFinalMessage(HWND /*hWnd*/) { delete this; }
	int		ActivateFrame(int nCmdShow = -1);
	int		ActivateFrameForNewWindowDelayed(int nCmdShow = -1);

	CDonutView&		  view() 		{ return m_view; }
	const CDonutView& view() const  { return m_view; }

	int  		 _AddGroupOption(const CString& strFileName);
	static void  SetMainframeCloseFlag() { s_bMainframeClose = true; } 			//+++ mainfrmがcloseするときonにされる.

	void	searchEngines(const CString &strKeyWord );
	void	StyleSheet(CString strSheet, BOOL bOff, CString strSheetPath);
	CString GetSelectedText();
	CString GetSelectedTextLine();		//+++

	// Event handlers
	void	OnSetSecureLockIcon(long nSecureLockIcon) { m_nSecureLockIcon = nSecureLockIcon; }
	void	OnPrivacyImpactedStateChange(bool bPrivacyImpacted) { m_bPrivacyImpacted = bPrivacyImpacted; ATLTRACE( _T("[OnPrivacyImpactedStateChange]\n") ); }
	void	OnStatusTextChange(const CString &strText);
	void	OnDocumentComplete(IDispatch *pDisp, const CString &strURL);
	void	OnBeforeNavigate2(IDispatch *			pDisp,
						   const CString &		strURL,
						   DWORD				nFlags,
						   const CString &		strTargetFrameName,
						   CSimpleArray<BYTE>&	baPostedData,
						   const CString &		strHeaders,
						   bool &				bCancel );
	void	OnProgressChange(long progress, long progressMax);
	void	OnCommandStateChange(long Command, bool bEnable);
	void	OnDownloadBegin() { }
	void	OnDownloadComplete();
	void	OnTitleChange(const CString &strTitle);
	void	OnNewWindow2(IDispatch **ppDisp, bool &bCancel);
	void	OnStateConnecting();
	void	OnStateDownloading();
	void	OnStateCompleted();
	void	OnFileDownload(bool bActiveDocument, bool& bCancel) { }
	void	OnNewWindow3(IDispatch **ppDisp, bool& Cancel, DWORD dwFlags, BSTR bstrUrlContext,  BSTR bstrUrl);
	void	OnWindowClosing(bool IsChildWindow, bool& bCancel);

	const CString& strStatusBar() const { return m_strStatusBar; }

	LRESULT OnGetExtendedTabStyle();
	void 	OnSetExtendedTabStyle(DWORD dwStyle);

	CChildFrame *OnGetChildFrame() { return this; }

	void 	SetViewExStyle(DWORD dwStyle, BOOL bExProp = FALSE);
	void 	SetSearchWordAutoHilight( const CString& str, bool autoHilightSw );
	void 	SetArrayHist(std::vector< std::pair<CString, CString> > &	ArrayFore, std::vector< std::pair<CString, CString> > &	ArrayBack );
	void	SetDfgFileNameSection(const CString &strFileName, const CString &strSection);

	void	SetUrlSecurityExStyle(LPCTSTR lpszFile);

	CString GetNowSearchWord(){ return m_strSearchWord; }; //\\+
	void SetNowSearchWord(const CString& strWord){ m_strSearchWord = strWord; }; //\\+
	void SaveSearchWordflg(const bool b){ m_bSaveSearchWordflg = b; }; //\\+


	// Message map and handlers
	BEGIN_MSG_MAP(CChildFrame)
		try {	//+++
		PASS_MSG_MAP_MDICHILD_TO_TAB  (m_MDITab)
//		PASS_MSG_MAP_MENUOWNER_TO_EXPMENU(m_FavMenu)

		MSG_WM_SIZE 		( OnSize			)
		MSG_WM_PAINT		( OnPaint			)
		MSG_WM_SYSCOMMAND	( OnSysCommand		)
		MSG_WM_CREATE		( OnCreate			)
		MSG_WM_CLOSE		( OnClose			)
		MSG_WM_DESTROY		( OnDestroy 		)
		MSG_WM_MDIACTIVATE	( OnMDIActivate 	)
		MSG_WM_FORWARDMSG	( OnForwardMsg		)
		HANDLE_MESSAGE		( WM_SETFOCUS		)			// baseClass handler is not effective
		MSG_WM_USER_GET_IWEBBROWSER( )
		USER_MSG_WM_SAVE_OPTION( OnSaveOption	)

		// UH
		USER_MSG_WM_MENU_GOBACK 	( OnMenuGoBack		)
		USER_MSG_WM_MENU_GOFORWARD	( OnMenuGoForward	)
		MSG_WM_USER_HILIGHT 		( OnHilight 		)
		MSG_WM_USER_FIND_KEYWORD	( OnFindKeyWord 	)
		COMMAND_ID_HANDLER			( ID_WINDOW_TILE_HORZ, OnWindowTile )
		COMMAND_ID_HANDLER			( ID_WINDOW_TILE_VERT, OnWindowTile )
		MESSAGE_HANDLER 			( WM_USER_SIZE_CHG_EX, OnSizeChgEx	)
		MSG_WM_USER_USED_MOUSE_GESTURE( OnUsedMouseGesture )
		COMMAND_ID_HANDLER			( ID_STYLESHEET_BASE , OnStyleSheet )
		COMMAND_ID_HANDLER			( ID_STYLESHEET_OFF  , OnStyleSheet )
		MSG_WM_USER_CHANGE_CSS( OnChangeCSS )
		//^^^

		//minit
		USER_MSG_WM_GET_EXTENDED_TABSTYLE( OnGetExtendedTabStyle )
		USER_MSG_WM_SET_EXTENDED_TABSTYLE( OnSetExtendedTabStyle )
		USER_MSG_WM_SET_CHILDFRAME( OnGetChildFrame )

		COMMAND_ID_HANDLER_EX( ID_FILE_CLOSE			, OnFileClose				)
		COMMAND_ID_HANDLER_EX( ID_WINDOW_CLOSE_ALL		, OnWindowCloseAll			)	// UH
		COMMAND_ID_HANDLER_EX( ID_EDIT_IGNORE			, OnEditIgnore				)
		COMMAND_ID_HANDLER_EX( ID_EDIT_OPEN_SELECTED_REF, OnEditOpenSelectedRef 	)
		COMMAND_ID_HANDLER_EX( ID_EDIT_OPEN_SELECTED_TEXT, OnEditOpenSelectedText	)

		COMMAND_ID_HANDLER_EX( ID_EDIT_CLOSE_TITLE		, OnEditCloseTitle			)	// UDT DGSTR
		COMMAND_ID_HANDLER_EX( ID_EDIT_FIND_MAX 		, OnEditFindMax 			)	//moved from CWebBrowserCommandHandler by minit

		COMMAND_ID_HANDLER_EX( ATL_IDS_SCPREVWINDOW 	, OnWindowPrev				)
		COMMAND_ID_HANDLER_EX( ATL_IDS_SCNEXTWINDOW 	, OnWindowNext				)

		COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_ADD	, OnFavoriteGroupAdd		)
		COMMAND_ID_HANDLER_EX( ID_FAVORITE_ADD			, OnFavoriteAdd 			)

		COMMAND_ID_HANDLER_EX( ID_VIEW_SETFOCUS 		, OnViewSetFocus			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_REFRESH			, OnViewRefresh 			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_STOP 			, OnViewStop				)
		COMMAND_ID_HANDLER_EX( ID_VIEW_OPTION			, OnViewOption				)
		COMMAND_ID_HANDLER_EX( ID_FILE_SAVE_AS			, OnFileSaveAs				)
		COMMAND_ID_HANDLER_EX( ID_FILE_PAGE_SETUP		, OnFilePageSetup			)
		COMMAND_ID_HANDLER_EX( ID_FILE_PRINT			, OnFilePrint				)
		COMMAND_ID_HANDLER_EX( ID_FILE_PROPERTIES		, OnFileProperties			)

		COMMAND_ID_HANDLER_EX( ID_EDIT_FIND 			, OnEditFind				)

		COMMAND_ID_HANDLER( ID_WINDOW_CLOSE_EXCEPT		, OnWindowCloseExcept		)
		COMMAND_ID_HANDLER( ID_WINDOW_REFRESH_EXCEPT	, OnWindowRefreshExcept 	)

		COMMAND_ID_HANDLER( ID_LEFT_CLOSE				, OnLeftRightClose			)
		COMMAND_ID_HANDLER( ID_RIGHT_CLOSE				, OnLeftRightClose			)

		COMMAND_ID_HANDLER_EX( ID_DOCHOSTUI_OPENNEWWIN	, OnDocHostUIOpenNewWin 	)
		COMMAND_ID_HANDLER_EX( ID_REGISTER_AS_BROWSER	, OnRegisterAsBrowser		)

		COMMAND_RANGE_HANDLER_EX( ID_VIEW_BACK1   , ID_VIEW_BACK9	, OnViewBackX	)
		COMMAND_RANGE_HANDLER_EX( ID_VIEW_FORWARD1, ID_VIEW_FORWARD9, OnViewForwardX)

		COMMAND_ID_HANDLER( ID_HTMLZOOM_MENU			, OnHtmlZoomMenu			)	//+++
		COMMAND_ID_HANDLER( ID_HTMLZOOM_ADD				, OnHtmlZoomAdd				)	//+++
		COMMAND_ID_HANDLER( ID_HTMLZOOM_SUB				, OnHtmlZoomSub				)	//+++
		COMMAND_ID_HANDLER( ID_HTMLZOOM_100TOGLE		, OnHtmlZoom100Togle		)	//+++
		COMMAND_RANGE_HANDLER_EX( ID_HTMLZOOM_400 , ID_HTMLZOOM_050 , OnHtmlZoom    )	//+++

		COMMAND_ID_HANDLER_EX( ID_TITLE_COPY			, OnTitleCopy				)
		COMMAND_ID_HANDLER_EX( ID_URL_COPY				, OnUrlCopy 				)
		COMMAND_ID_HANDLER_EX( ID_COPY_TITLEANDURL		, OnTitleAndUrlCopy 		)

		COMMAND_ID_HANDLER_EX( ID_SAVE_FAVORITE_TO		, OnSaveFavoriteTo			)

		COMMAND_ID_HANDLER_EX( ID_TAB_CLONE				, OnTabClone				)
		COMMAND_RANGE_HANDLER_EX( FAVORITE_MENU_ID_MIN, FAVORITE_MENU_ID_MAX, OnFavoritesOpen )

		CHAIN_COMMANDS( CWebBrowserCommandHandler<CChildFrame> )
		CHAIN_MSG_MAP( CUpdateCmdUI<CChildFrame>			)
		CHAIN_MSG_MAP( CMDIChildUserMessenger<CChildFrame>	)

		CHAIN_COMMANDS_MEMBER( m_view ) // CHAIN_CLIENT_COMMANDS() not send, why?
		CHAIN_MSG_MAP( baseClass )
		
		if (uMsg == WM_COMMAND || uMsg == WM_MEASUREITEM || uMsg == WM_DRAWITEM) {
			::SendMessage(GetTopLevelWindow(), uMsg, wParam, lParam);
		}
		} catch (...) {
			ATLASSERT(0);
		}
	END_MSG_MAP()


	// Update Command UI Map
	BEGIN_UPDATE_COMMAND_UI_MAP( CChildFrame )
		CHAIN_UPDATE_COMMAND_UI_MEMBER( m_view )

		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_VIEW_BACK   , m_bNavigateBack   , true ) 	// with popup
		UPDATE_COMMAND_UI_ENABLE_IF 		  ( ID_VIEW_FORWARD, m_bNavigateForward )

		// UPDATE_COMMAND_UI_SETDEFAULT_PASS( ID_VIEW_BACK1    )
		// UPDATE_COMMAND_UI_SETDEFAULT_PASS( ID_VIEW_FORWARD1 )

		UPDATE_COMMAND_UI_POPUP_ENABLE_IF( ID_VIEW_BACK1   , m_bNavigateBack	)
		UPDATE_COMMAND_UI_POPUP_ENABLE_IF( ID_VIEW_FORWARD1, m_bNavigateForward )

		UPDATE_COMMAND_UI(	ID_VIEW_FONT_SMALLEST, OnUpdateFontSmallestUI )
		UPDATE_COMMAND_UI(	ID_VIEW_FONT_SMALLER , OnUpdateFontSmallerUI  )
		UPDATE_COMMAND_UI(	ID_VIEW_FONT_MEDIUM  , OnUpdateFontMediumUI   )
		UPDATE_COMMAND_UI(	ID_VIEW_FONT_LARGER  , OnUpdateFontLargerUI   )
		UPDATE_COMMAND_UI(	ID_VIEW_FONT_LARGEST , OnUpdateFontLargestUI  ) 			// with popup

		UPDATE_COMMAND_UI(	ID_DEFAULT_PANE , OnUpdateStatusBarUI )
		UPDATE_COMMAND_UI(	IDC_PROGRESS	, OnUpdateProgressUI  )
		UPDATE_COMMAND_UI(	ID_SECURE_PANE	, OnUpdateSecureUI	  )
		UPDATE_COMMAND_UI(	ID_PRIVACY_PANE , OnUpdatePrivacyUI   )

		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_SEARCHBAR_HILIGHT, m_bNowHilight )

		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_WINDOW_RESTORE, _GetShowCmd() != SW_SHOWNORMAL && !CMainOption::s_bTabMode, true )	// with popup
		UPDATE_COMMAND_UI_ENABLE_IF( ID_WINDOW_MOVE    , _GetShowCmd() != SW_SHOWMAXIMIZED && !CMainOption::s_bTabMode )
		UPDATE_COMMAND_UI_ENABLE_IF( ID_WINDOW_SIZE    , _GetShowCmd() != SW_SHOWMAXIMIZED && _GetShowCmd() != SW_SHOWMINIMIZED && !CMainOption::s_bTabMode )
		UPDATE_COMMAND_UI_ENABLE_IF( ID_WINDOW_MINIMIZE, _GetShowCmd() != SW_SHOWMINIMIZED && !CMainOption::s_bTabMode )
		UPDATE_COMMAND_UI_ENABLE_IF( ID_WINDOW_MAXIMIZE, _GetShowCmd() != SW_SHOWMAXIMIZED && !CMainOption::s_bTabMode )

		UPDATE_COMMAND_UI( ID_STYLESHEET_BASE, OnStyleSheetBaseUI )
	END_UPDATE_COMMAND_UI_MAP()


	// Command overrides
	LRESULT OnChangeCSS(LPCTSTR lpszStyleSheet);
	LRESULT OnStyleSheet(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void 	SetDefaultStyleSheet(const CString& strStyleSheet);

	LRESULT OnUsedMouseGesture() { return (m_view.m_ViewOption.m_dwExStyle & DVS_EX_MOUSE_GESTURE) != 0; }
	LRESULT OnSizeChgEx(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) { m_nCmdType = SC_RESTORE; return 0; }
	LRESULT OnWindowTile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/) { m_nCmdType = SC_RESTORE; return 0; }
	LRESULT OnMenuGoBack(HMENU hMenu) { MenuChgGoBack(hMenu); return 0; }
	LRESULT OnMenuGoForward(HMENU hMenu) { MenuChgGoForward(hMenu); return 0; }
	void 	OnRegisterAsBrowser(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/) { SetRegisterAsBrowser(wNotifyCode == NM_ON); }

	void 	OnDocHostUIOpenNewWin(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnViewSetFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	void 	OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnViewStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnFavoriteAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnFavoriteGroupAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	void 	OnClose();
	void 	OnPaint(HDC hDc);	//+++ 追加

	//+++ 失敗 void OnLButtonDown(WORD wparam, const WTL::CPoint& pt);
	void	OnSize(UINT nType, CSize size);	//+++ 追加
	void 	OnSysCommand(UINT uCmdType, CPoint pt);

	void 	OnWindowPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)	{ MDINext(m_hWnd, TRUE); }
	void 	OnWindowNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/) { MDINext(m_hWnd, FALSE); }

	void	OnEditFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	LRESULT OnWindowCloseExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnLeftRightClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnWindowRefreshExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	LRESULT OnForwardMsg(LPMSG pMsg, DWORD);
	static void CALLBACK OncePaintReq(HWND hWnd, UINT wparam, UINT_PTR lparam, DWORD );	///< +++ タイマーで無理やりまって、画面をリサイズで描画

	LRESULT OnCreate(LPCREATESTRUCT lpcreatestruct);
	void 	_InitDragDropSetting();

	void 	OnDestroy();


	void 	OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnEditIgnore(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnEditCloseTitle(WORD , WORD , HWND );
	void 	OnEditOpenSelectedRef(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnEditOpenSelectedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void  	OnEditFindMax(WORD wNotifyCode, WORD wID, HWND hWndCtl) { _OnEditFindMax(wNotifyCode, wID, hWndCtl); }

	LRESULT OnHilight(CString strKeyWord);	//+++ , BOOL bToggle = TRUE, BOOL bFlag = FALSE);

  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
	LRESULT OnHilightOnce(IDispatch *pDisp, LPCTSTR lpszKeyWord);
  #endif

	void 	StyleSheetRecursive(CComPtr<IHTMLDocument2> spDocument, CString strSheet, BOOL bOff, CString strSheetPath);
	BOOL 	CheckFrameDefinePage(CComPtr<IHTMLDocument2> spDocument);
	void 	StyleSheetSub(IHTMLDocument2 *pDocument, CString strSheet, BOOL bOff, CString strSheetPath);

	LRESULT OnFindKeyWord(LPCTSTR lpszKeyWord, BOOL bFindDown, long Flags = 0);
	BOOL 	_FindKeyWordOne(IHTMLDocument2 *pDocument, const CString& rStrKeyWord, BOOL bFindDown, long Flags = 0);	//ページ内検索


	void 	_OpenSelectedText(IHTMLDocument2 *pDocument);
	CString _GetSelectedText(IHTMLDocument2* pDocument);

	struct _Function_SelectEmpt;
	struct _Function_Hilight;

  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
	// gae _Function_Hilightと引数の意味が違うので注意
	struct _Function_Hilight2;
  #endif	// KIKE_UNDONUT_G
	

  #if 1
	void OnSaveOption(LPCTSTR lpszFileName, int nIndex);
  #endif

	void OnViewBackX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnViewForwardX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnTitleCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnTitleAndUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnSaveFavoriteTo(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/) { }

	void OnTabClone(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnFavoritesOpen(UINT uNotifyCode, int nID, CWindow wndCtl);

	void OnMDIActivate(HWND hwndChildDeact, HWND hwndChildAct);

	void ApplyDefaultStyleSheet();

	void SetTravelLogData();

	BOOL _Load_TravelData(std::vector<std::pair<CString, CString> >&	arrFore,
						  std::vector<std::pair<CString, CString> >&	arrBack,
						  CString & 									strFileName,
						  CString & 									strSection);

	BOOL _Load_TravelLog(	std::vector<std::pair<CString, CString> >&	arrLog,
							CComPtr<IWebBrowser2> pWB2,
							BOOL bFore);

	LPOLESTR _ConvertString(LPCTSTR lpstrBuffer, int nBufferSize);

	/// 画像ファイルならIEの設定にしたがってサイズ調整する
	void CheckImageAutoSize(const CString* pStrURL, BOOL bFirst);
	LRESULT OnHtmlZoomMenu(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnHtmlZoomAdd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnHtmlZoomSub(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	///+++ 100% とその他の比率をトグル切替.
	LRESULT OnHtmlZoom100Togle(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL &);
	void 	OnHtmlZoom(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void 	SetBodyStyleZoom(int addSub, int scl, bool bWheel = false);

	//プログレスペインの幅を設定
	BOOL 	SetProgressPaneWidth(int cxWidth);

	DWORD 	_GetInheritedDLControlFlags();
	DWORD 	_GetInheritedExtendedStyleFlags();

	void 	_SaveFocus() { m_hWndFocus = ::GetFocus(); }
	void 	_RestoreFocus();
	void 	_SetPageFocus();

	// Update Command UI Handlers
	void 	OnStyleSheetBaseUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontSmallestUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontSmallerUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontMediumUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontLargerUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontLargestUI(CCmdUI *pCmdUI);
	void 	OnUpdateStatusBarUI(CCmdUI *pCmdUI);
	void 	OnUpdateSecureUI(CCmdUI *pCmdUI);
	void 	OnUpdatePrivacyUI(CCmdUI *pCmdUI);
	void 	OnUpdateProgressUI(CCmdUI *pCmdUI);

	int 	_GetShowCmd();

	DWORD 	GetTabItemState(HWND hTarWnd);

	virtual void PreDocumentComplete( /*[in]*/ IDispatch *pDisp, /*[in]*/ VARIANT *URL);

private:

	void	_SetFavicon(const CString& strURL);
	void	_SetFocusToHTML();
	void	_KillFocusToHTML();

	// Constants
	enum { _nPosFavoriteMenuOfTab = 10 };

	// Data members
	CDonutView									m_view;

	CDonutTabBar&								m_MDITab;
	CDonutAddressBar&							m_AddressBar;
//	CChildFavoriteMenu<CChildFrame> 			m_FavMenu;		// タブ右クリックのお気に入り用？
	CString 									m_strStatusBar;
	HWND										m_hWndFocus;
	HWND										m_hWndF;
	HWND										m_hWndA;
	
	CComBSTR/*BSTR*/							m_strBookmark;
	CString										m_strOldKeyword;

	CImageList									m_imgList;

	CString 									m_strDfgFileName;
	CString 									m_strSection;

	std::vector<std::pair<CString, CString> >	m_ArrayHistFore;
	std::vector<std::pair<CString, CString> >	m_ArrayHistBack;

	CString 									m_strSearchWord;
	bool										m_bSaveSearchWordflg;	//\\ trueならタブ切り替え時に保存する

	//std::auto_ptr<CIEToolBar>					m_ieToolBar;				//+++ 実験...失敗.

	long										m_nProgress;
	long										m_nProgressMax;
	int 										m_nPainBookmark;
	int 										m_nCmdType;
	int 										m_nSecureLockIcon;
	int 										m_nImgWidth;
	int 										m_nImgHeight;

	int											m_nImgScl;					//+++ zoom,imgサイズ自動設定での設定値.
	int											m_nImgSclSav;				//+++ zoom,imgサイズの100%とのトグル切り替え用
	int											m_nImgSclSw;				//+++ zoom,imgサイズの100%とのトグル切り替え用

	BYTE/*bool BOOL*/							m_bNavigateBack;
	BYTE/*bool BOOL*/							m_bNavigateForward;
	BYTE/*bool*/								m_bNewWindow2;
	BYTE/*bool*/								m_bClosing;
	BYTE/*bool*/								m_bPageFocusInitialized;
	BYTE/*bool*/								m_bWaitNavigateLock;
	BYTE/*bool*/								m_bPrivacyImpacted;
	BYTE/*bool BOOL*/							m_bInitTravelLog;			//minit
	BYTE/*bool*/								m_bOperateDrag;
	BYTE/*bool*/								m_bExPropLock;
	BYTE/*bool*/								m_bAutoHilight;
	BYTE/*bool*/								m_bNowHilight;
	BYTE/*bool*/								m_bImg;						//+++ urlが画像かどうか
	BYTE/*bool*/								m_bImageAutoSizeReq;		//+++ OnSizeされたか?(自動画像調整のため)
	BYTE/*bool*/								m_bImgAuto_NouseLClk;		//+++ 画像自動リサイズで左クリックを使わない.

	bool										m_bNowNavigate;				// for Javascirpt:void(0)
	bool										m_bReload;
	CString										m_strFaviconURL;			// Adressbar用
	static bool 								s_bMainframeClose;			//+++ mainfrmがcloseするときonにされる.
};



#if 1	//+++ 手抜き. CMainFrameが終了するときにtrueにする.
__declspec(selectany) bool	   CChildFrame::s_bMainframeClose	= 0;
#endif



/////////////////////////////////////////////////////////////////////////////
