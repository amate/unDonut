/**
 *	@file	MainOption.h
 *	@brief	donutのオプション : 全般, 全般2
 */

#pragma once

#include <set>
#include "../MtlUpdateCmdUI.h"
#include "../MtlProfile.h"
#include "../MtlUser.h"
#include "../resource.h"

enum EMain_Ex {
	MAIN_EX_NEWWINDOW				= 0x00000001L,
	MAIN_EX_NOACTIVATE				= 0x00000002L,
	MAIN_EX_DELAYLOAD				= 0x00000004L,	// 一度に複数タブが開かれるとき、タブがアクティブになるまで読み込みを延期する
	MAIN_EX_ONEINSTANCE 			= 0x00000008L,
	// MAIN_EX_WINDOWLIMIT 			= 0x00000010L,
	MAIN_EX_NOCLOSEDFG				= 0x00000020L,	// お気に入りグループを開くとき、他のウィンドウを閉じない
	// MAIN_EX_NOMDI					= 0x00000040L,
	// MAIN_EX_VIEWCLIPBOARD		= 0x00000080L,
	MAIN_EX_FULLSCREEN				= 0x00000100L,
	MAIN_EX_BACKUP					= 0x00000200L,
	MAIN_EX_NOACTIVATE_NEWWIN		= 0x00000400L,	// ページ内のリンクを開くとき、作成された新規タブをアクティブにしない
	// MAIN_EX_ADDFAVORITEOLDSHELL 	= 0x00000800L,
	// MAIN_EX_ORGFAVORITEOLDSHELL 	= 0x00001000L,
	MAIN_EX_GLOBALOFFLINE			= 0x00002000L,
	// MAIN_EX_LOADGLOBALOFFLINE		= 0x00004000L,
	// MAIN_EX_KILLDIALOG				= 0x00008000L,
	MAIN_EX_REGISTER_AS_BROWSER 	= 0x00010000L,
	MAIN_EX_INHERIT_OPTIONS 		= 0x00020000L,
	MAIN_EX_IGNORE_BLANK			= 0x00040000L,
	//MAIN_EX_FOCUSTOSEARCHBAR		= 0x00080000L,
	MAIN_EX_USECUSTOMFINDBER		= 0x00100000L,
	MAIN_EX_EXTERNALNEWTAB			= 0x00080000L,
	MAIN_EX_EXTERNALNEWTABACTIVE	= 0x00200000L,
};


enum EMain_Ex2 {
	//\\メニューの方に移動MAIN_EX2_NOCSTMMENU 			= 0x00000001L,		//+++ メモ:MainOptionで設定.
	MAIN_EX2_DEL_CASH				= 0x00000002L,		//+++ メモ:実処理はDestroyDialog("終了処理"のオプション)
	MAIN_EX2_DEL_COOKIE 			= 0x00000004L,		//+++ メモ:実処理はDestroyDialog("終了処理"のオプション)
	MAIN_EX2_DEL_HISTORY			= 0x00000008L,		//+++ メモ:実処理はDestroyDialog("終了処理"のオプション)
	MAIN_EX2_MAKECASH				= 0x00000010L,		//+++ メモ:実処理はDestroyDialog("終了処理"のオプション)
	MAIN_EX2_DEL_RECENTCLOSE		= 0x00000020L,		//+++ メモ:実処理はDestroyDialog("終了処理"のオプション)

	//+++ +mod版追加
	MAIN_EX2_NOCLOSE_NAVILOCK		= 0x00000040L,		//+++ ナビゲートLock時、Closeできなくする. 実処理はDLControlOption
	MAIN_EX2_MINBTN2TRAY			= 0x00000080L,		//+++ 最小化ボタンでタスクトレイに入るようにする.
	MAIN_EX2_CLOSEBTN2TRAY			= 0x00000100L,		//+++ 閉じるボタンでタスクトレイに入るようにする.
	MAIN_EX2_USER_AGENT_FLAG		= 0x00000400L,		//+++ このフラグがonのときのみ UserAgent文字列を設定する(offならieのまま)
  #if 0	//+++ 失敗
	MAIN_EX2_TITLEBAR_STR_SWAP		= 0x00000200L,		//+++ タイトルバーの表示を,"ページ名 - unDonut"にする(通常は "unDonut - ページ名")
  #endif
};

// 前方宣言
class CRecentClosedTabList;

enum ERecentDoc {
	RECENTDOC_MENUTYPE_URL		= 0,
	RECENTDOC_MENUTYPE_TITLE	= 1,
	RECENTDOC_MENUTYPE_DOUBLE	= 2,
};

enum EAutoImageResize {
	AUTO_IMAGE_RESIZE_NONE		= 0,
	AUTO_IMAGE_RESIZE_FIRSTON	= 1,
	AUTO_IMAGE_RESIZE_LCLICK	= 2,
};

enum BROWSEROPERATINGMODE {
	kMultiThreadMode  = 0,
	kMultiProcessMode = 1,
};

/////////////////////////////////////////////////////////////////////////////
// CMainOption

class CMainOption {
	friend class CMainPropertyPage;
	friend class CMainPropertyPage2;

public:
	static DWORD	s_dwMainExtendedStyle;
	static DWORD	s_dwMainExtendedStyle2;

	static DWORD	s_dwBackUpTime;
	static DWORD	s_dwAutoRefreshTime;
	static volatile bool	s_bAppClosing;

	static volatile bool 	s_bIgnoreUpdateClipboard;	// for insane WM_DRAWCLIBOARD

	static bool 	s_bStretchImage;

	static bool		s_bIgnore_blank;
	static bool		s_bUseCustomFindBar;
	static bool		s_bExternalNewTab;
	static bool		s_bExternalNewTabActive;
	static bool		s_bDelayLoad;

	static int		s_nMaxRecentClosedTabCount;
	static int		s_RecentClosedTabMenuType;

	static int		s_nAutoImageResizeType;

	/// ブラウザビューの動作モード　起動時にのみ値を設定する
	static BROWSEROPERATINGMODE s_BrowserOperatingMode;

private:
	static DWORD	s_dwErrorBlock;

	static CString	s_strExplorerUserDirectory;

public:
	// Constructor
	CMainOption();

	static void 	GetProfile();
	static void 	WriteProfile();
	static void 	SetExplorerUserDirectory(const CString &strPath);
	static const CString&	GetExplorerUserDirectory();


	// Message map and handlers
	BEGIN_MSG_MAP(CMainFrame)
		COMMAND_ID_HANDLER_EX( ID_MAIN_EX_NEWWINDOW 		, OnMainExNewWindow 		)
		COMMAND_ID_HANDLER_EX( ID_MAIN_EX_NOACTIVATE		, OnMainExNoActivate		)
		COMMAND_ID_HANDLER_EX( ID_MAIN_EX_NOACTIVATE_NEWWIN , OnMainExNoActivateNewWin	)
	END_MSG_MAP()


	void	OnMainExNewWindow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnMainExNoActivate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnMainExNoActivateNewWin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);


	// Update command UI and handlers
	BEGIN_UPDATE_COMMAND_UI_MAP(CMainOption)
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_MAIN_EX_NEWWINDOW		 , MAIN_EX_NEWWINDOW		  , s_dwMainExtendedStyle)
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_MAIN_EX_NOACTIVATE		 , MAIN_EX_NOACTIVATE		  , s_dwMainExtendedStyle)
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_MAIN_EX_NOACTIVATE_NEWWIN, MAIN_EX_NOACTIVATE_NEWWIN  , s_dwMainExtendedStyle)
		//UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_REGISTER_AS_BROWSER 	 , MAIN_EX_REGISTER_AS_BROWSER, s_dwMainExtendedStyle)
	END_UPDATE_COMMAND_UI_MAP()
};



///////////////////////////////////////////////////////////////////////////////
// CMainPropertyPage : [Donutのオプション] - [全般]

class CMainPropertyPage
	: public CPropertyPageImpl<CMainPropertyPage>
	, public CWinDataExchange<CMainPropertyPage>
	, protected CMainOption
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MAIN };

	// Constructor
	CMainPropertyPage(HWND hWnd);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();


	// DDX map
	BEGIN_DDX_MAP( CMainPropertyPage )
		DDX_CHECK( IDC_CHECK_MAIN_NEWWINDOW 			, m_nNewWindow			)
		DDX_CHECK( IDC_CHECK_MAIN_NOACTIVATE			, m_nNoActivate 		)
		DDX_CHECK( IDC_CHECK_MAIN_NOACTIVATE_NEWWIN 	, m_nNoActivateNewWin	)
		DDX_CHECK( IDC_CHECK_EXTERNALNEWTAB				, s_bExternalNewTab		)
		DDX_CHECK( IDC_CHECK_EXTERNALNEWTABACTIVE		, s_bExternalNewTabActive)
		DDX_CHECK( IDC_CHECK_MAIN_INHERIT_OPTIONS		, m_nInheritOptions 	)
		DDX_CHECK( IDC_CHECK_ONEINSTANCE				, m_nOneInstance		)
		DDX_CHECK( IDC_CHECK_MAIN_BACKUP				, m_nBackUp 			)
		DDX_CHECK( IDC_CHECK_MAIN_NOCLOSEDFG			, m_nNoCloseDFG 		)
		DDX_CHECK( IDC_CHECK_DELAYLOAD					, s_bDelayLoad			)
		DDX_CHECK( IDC_CHECK_MAIN_REGISTER_AS_BROWSER	, m_nRegisterAsBrowser	)
		DDX_CHECK( IDC_CHK_IGNORE_BLANK					, s_bIgnore_blank		)
		DDX_CHECK( IDC_CHK_USECUSTOMFINDBAR				, s_bUseCustomFindBar	)
		//+++ DDX_CHECK( IDC_CHECK_NOCLOSE_NAVILOCK 	, m_nNoCloseNL			)
		DDX_COMBO_INDEX( IDC_COMBO_BROWSREOPERATINGMODE	, m_nBrowserOperatingMode)

		DDX_INT_RANGE( IDC_EDIT_MAIN_BACKUPTIME 		, m_nBackUpTime    , 1, 120 )

		// UDT DGSTR ( dai
		DDX_INT_RANGE( IDC_EDIT_MAIN_AUTOREFRESHTIME_MIN, m_nAutoRefTimeMin, 0,  59 )
		DDX_INT_RANGE( IDC_EDIT_MAIN_AUTOREFRESHTIME_SEC, m_nAutoRefTimeSec, 0,  59 )
		// ENDE
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CMainPropertyPage )
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDC_CHECK_EXTERNALNEWTAB, OnCheckExternalNewTab )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_BAR_FONT, OnFont )
		CHAIN_MSG_MAP( CPropertyPageImpl<CMainPropertyPage> )
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnCheckExternalNewTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);
	void OnFont(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);
	
private:
	// Implementation
	void	_GetData();
	void	_SetData();


	// Data members
	bool	m_bInit;

	int 	m_nNewWindow;
	int 	m_nNoActivate;
	int 	m_nNoActivateNewWin;
	int 	m_nOneInstance;
	int 	m_nNoCloseDFG;
	int 	m_nBackUp;
	int 	m_nRegisterAsBrowser;
	int 	m_nBackUpTime;
	int 	m_nAutoRefreshTime;
	int 	m_nAutoRefTimeMin;
	int 	m_nAutoRefTimeSec; // UDT DGSTR ( dai
	int 	m_nInheritOptions;
	//+++ int		m_nNoCloseNL;			//+++ ナビロック時閉じれなくするフラグ
	int		m_nBrowserOperatingMode;

	CWindow m_wnd;
	MTL::CLogFont	m_lf;
};


//===========================================================================

enum EFileNew {
	FILENEW_BLANK	= 0x00000001L,
	FILENEW_COPY	= 0x00000002L,
	FILENEW_HOME	= 0x00000004L,
	FILENEW_USER	= 0x00000008L,		//+++
};


class CFileNewOption 
{
	friend class CMainPropertyPage2;
	friend class CMainFrame;

	static DWORD	s_dwFlags;
	static CString	s_strUsr;

public:
	static void 	GetProfile();
	static void 	WriteProfile();
};



/**
	CDonutConfirmOption
	各種確認ダイアログを出すためのクラス
 */
class CDonutConfirmOption 
{
	friend class CMainPropertyPage2;
public:
	//メンバ関数
	static void 	GetProfile();
	static void 	WriteProfile();

	static bool 	OnDonutExit(HWND hWnd, const std::set<DWORD>& setProcessId);
	static bool 	OnCloseAll(HWND hWnd = NULL);
	static bool 	OnCloseAllExcept(HWND hWnd = NULL);
	static bool		OnCloseLeftRight(HWND hWnd = NULL, bool bLeft = false);

private:
	static bool 	_SearchDownloadingDialog(const std::set<DWORD>& setProcessId);

	//確認ダイアログの設定フラグの定数
	enum EDonut_Confirm {
		DONUT_CONFIRM_EXIT			 = 0x00000001L,
		DONUT_CONFIRM_CLOSEALL		 = 0x00000002L,
		DONUT_CONFIRM_CLOSEALLEXCEPT = 0x00000004L,
		DONUT_CONFIRM_STOPSCRIPT	 = 0x00000008L,
		DONUT_CONFIRM_CLOSELEFTRIGHT = 0x00000010L,
	};

	//メンバ変数
	static DWORD	s_dwFlags;
};


/////////////////////////////////////////////////////////////////////////////////////
// CMainPropertyPage2 : [Donutのオプション] - [全般2]

class CMainPropertyPage2
	: public CInitPropertyPageImpl<CMainPropertyPage2>
	//, public CPropertyPageImpl<CMainPropertyPage2>
	, public CWinDataExchange<CMainPropertyPage2>
	, protected CMainOption
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MAIN2 };

	// Constructor
	CMainPropertyPage2(HWND hWnd, CRecentClosedTabList& rRecent);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	// DDX map
	BEGIN_DDX_MAP( CMainPropertyPage2 )
		DDX_UINT ( IDC_EDIT_SZ_PAIN1		, m_nSzPain1	)
		DDX_UINT ( IDC_EDIT_SZ_PAIN2		, m_nSzPain2	)
		DDX_CHECK( IDC_CHK_SWAP_PAIN		, m_nChkSwapPain)

		DDX_CHECK( IDC_CHK_MENU 			, m_nShowMenu	)
		DDX_CHECK( IDC_CHK_TOOLBAR			, m_nShowToolBar)
		DDX_CHECK( IDC_CHK_ADRESS			, m_nShowAdress )
		DDX_CHECK( IDC_CHK_TAB				, m_nShowTab	)
		DDX_CHECK( IDC_CHK_LINK 			, m_nShowLink	)
		DDX_CHECK( IDC_CHK_SEARCH			, m_nShowSearch )
		DDX_CHECK( IDC_CHK_STATUS			, m_nShowStatus )
		DDX_RADIO( IDC_RADIO_NO2TRAY		, m_nMinBtn2Tray )			//+++

		// 最近閉じたタブ
		DDX_INT_RANGE( IDC_EDIT_MRUCOUNT	, s_nMaxRecentClosedTabCount, m_nMRUCountMin, m_nMRUCountMax )
		DDX_CBINDEX( IDC_COMBO_MRU_MENUTYPE	, s_RecentClosedTabMenuType )

		// 新規ページの標準の動作
		DDX_RADIO( IDC_RADIO_BLANK	, m_nDefaultNewFileButtonRadio )
		DDX_TEXT( IDC_EDIT_NEWPAGE_USER, CFileNewOption::s_strUsr )

		// 動作前に確認ダイアログを出す
		DDX_CHECK( IDC_CHECK_CFD_EXIT,			m_nExit 		 )
		DDX_CHECK( IDC_CHECK_CFD_CLOSEALL,		m_nCloseAll 	 )
		DDX_CHECK( IDC_CHECK_CFD_CLOSEEXCEPT,	m_nCloseAllExcept)
		DDX_CHECK( IDC_CHECK_CFD_CLOSELEFTRIGHT,m_nCloseLeftRight)

		// その他
		DDX_RADIO( IDC_RADIO_IMG_AUTO_RESIZE_NONE, s_nAutoImageResizeType )	//+++

	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CMainPropertyPage2 )
		COMMAND_ID_HANDLER_EX( IDC_BTN_NEWPAGE_USER, OnButton )
		CHAIN_MSG_MAP( CPropertyPageImpl<CMainPropertyPage2> )
	END_MSG_MAP()

	void 	OnButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);


private:
	CString 	BrowseForFolder(const CString& strTitle, const CString& strNowPath);

	void		_GetData();
	void		_SetData();
	void		InitCtrls();

	// Data members
	CRecentClosedTabList&	m_rRecentClosedTabList;
	CWindow 	m_wnd;

	// UH
	WORD		m_nSzPain1;
	WORD		m_nSzPain2;
	int 		m_nChkSwapPain;

	int 		m_nShowMenu;
	int 		m_nShowToolBar;
	int 		m_nShowAdress;
	int 		m_nShowTab;
	int 		m_nShowLink;
	int 		m_nShowSearch;
	int 		m_nShowStatus;

	const int	m_nMRUCountMin;
	const int	m_nMRUCountMax;

	int			m_nDefaultNewFileButtonRadio;

	int 		m_nExit;
	int 		m_nCloseAll;
	int 		m_nCloseAllExcept;
	int 		m_nCloseLeftRight;

	int			m_nMinBtn2Tray;			//+++

	bool		m_bInit;
};
