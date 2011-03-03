/**
 *	@file	FavoriteMenuDialog.h
 *	@brief	donutのオプション : "お気に入りメニュー"
 */

#pragma once

#include "../resource.h"
#include "../MtlWin.h"

// Extended styles

enum EExplorerMenuStyle {
	EMS_ADDITIONAL_OPENALL		= 0x00000001L,	// 「これらをすべて開く」を追加する
	EMS_IE_ORDER				= 0x00000002L,	// IEの順番で並べる
	EMS_DRAW_ICON				= 0x00000004L,	// アイコンを表示する
	EMS_SPACE_MIN				= 0x00000008L,	// 間隔を詰める
	EMS_DRAW_FAVICON			= 0x00000010L,	// Faviconを表示する
	EMS_ADDITIONAL_SAVEFAV_TO	= 0x00000020L,	// 「この位置にお気に入り追加」という項目を追加する
	EMS_CUSTOMIZE_ORDER 		= 0x00000040L,	// お気に入りの順番をドラッグして並び替えできるようにする
	EMS_ADDITIONAL_RESETORDER	= 0x00000080L,	// 「並び順を初期化する」という項目を追加する
	EMS_USER_DEFINED_FOLDER		= 0x00000100L,	// ユーザー定義フォルダを使用する

	EMS_DEFAULTBITS	= EMS_ADDITIONAL_OPENALL
					| EMS_IE_ORDER
					| EMS_DRAW_ICON
					| EMS_CUSTOMIZE_ORDER,
};

 

////////////////////////////////////////////////////////////////////////
// CFavoritesMenuOption

class CFavoritesMenuOption 
{
public:
	static DWORD	s_dwStyle;

	static bool 	s_bAddOpenAll;				// 「これらをすべて開く」を追加する
	static bool		s_bAddSaveFav;				// 「この位置にお気に入り追加」を追加する
	static bool 	s_bIEOrder;					// IEの順番で並べる
	static bool 	s_bDrawIcon;				// アイコンを表示する
	static bool 	s_bSpaceMin;				// 間隔を詰める
	static bool		s_bDrawFavicon;				// Faviconを表示する

	static DWORD	s_dwMaxMenuItemTextLength;	// メニューの最大文字数
	static DWORD	s_dwMaxMenuBreakCount;		// 格段の最大項目数
					
	static bool		s_bUserFolder;				// ユーザー定義フォルダを使用する
	static CString 	s_strUserFolder;			// ユーザー定義フォルダのパス

	static bool		s_bCstmOrder;				// お気に入りの順番をドラッグして並べ替えできるようにする
	static bool		s_bAddResetOrder;			// 「並び順を初期化する」を追加する

//
	static void SetFuncRefreshFav(std::function<void ()> func) { s_funcRefreshFav = func; }
	static void CallBack()
	{
		if (s_funcRefreshFav)
			s_funcRefreshFav();
	}


	static CString	GetUserDirectory() { return s_strUserFolder; }
	static void		SetUserDirectory(const CString &strPath) { s_strUserFolder = strPath; }

	static DWORD	GetStyle() { return s_dwStyle; }
	static void		SetStyle(DWORD dwStyle) { s_dwStyle = dwStyle; }

	static void		SetMaxMenuItemTextLength(DWORD n) { s_dwMaxMenuItemTextLength = n; }
	static DWORD	GetMaxMenuItemTextLength() { return s_dwMaxMenuItemTextLength; }

	static void		SetMaxMenuBreakCount(DWORD n) { s_dwMaxMenuBreakCount = n; }
	static DWORD	GetMaxMenuBreakCount() { return s_dwMaxMenuBreakCount; }


	static void		GetProfile();
	static void		WriteProfile();

private:
	static std::function<void ()>	s_funcRefreshFav;
};








/////////////////////////////////////////////////////////////////
// CDonutFavoritesMenuPropertyPage : [Donutのオプション] - [お気に入りメニュー]

class CDonutFavoritesMenuPropertyPage
	: public CPropertyPageImpl<CDonutFavoritesMenuPropertyPage>
	, public CWinDataExchange<CDonutFavoritesMenuPropertyPage>
	, protected CFavoritesMenuOption
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_EXPMENU };

private:
	// Data members
	int		m_nMaxMenuItemTextLength;
	int		m_nMaxMenuBreakCount;

	CString m_strFolder;

	CEdit	m_editUserFolder;

	CButton	m_btnDrawIcon;
	CButton	m_btnSpaceMin;
	CButton	m_btnDrawFavicon;
	CButton	m_btnIEOrder;
	CButton	m_btnUserDefinedFolder;
	CButton	m_btnCstmOrder;

	bool	m_bInit;

public:
	// Constructor
	CDonutFavoritesMenuPropertyPage();

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

private:
	CString _BrowseForFolder();
	void	_SwitchEnable(int nID);

public:
	// DDX map
	BEGIN_DDX_MAP( CDonutFavoritesMenuPropertyPage )
		DDX_CONTROL_HANDLE( IDC_CHECK_DRAW_ICON		, m_btnDrawIcon		)
		DDX_CONTROL_HANDLE( IDC_CHECK_SPACE_MIN		, m_btnSpaceMin		)
		DDX_CONTROL_HANDLE( IDC_CHECK_DRAW_FAVICON	, m_btnDrawFavicon	)
		DDX_CONTROL_HANDLE( IDC_CHECK_FAVMENU_ORDER	, m_btnIEOrder		)
		DDX_CONTROL_HANDLE( IDC_CHECK_FAVMENU_USER	, m_btnUserDefinedFolder )
		DDX_CONTROL_HANDLE( IDC_EDIT_FAVMENU_USER	, m_editUserFolder	)
		DDX_CONTROL_HANDLE( IDC_CHECK_MENUORDER		, m_btnCstmOrder	)

		DDX_CHECK( IDC_CHECK_EXPMENU_ADDITIONAL 	 , s_bAddOpenAll	)
		DDX_CHECK( IDC_CHECK_FAVMENU_ORDER			 , s_bIEOrder		)

		DDX_INT_RANGE( IDC_EDIT_EXPMENU_ITEMTEXTMAX  , m_nMaxMenuItemTextLength	, 1,  255 )
		DDX_INT_RANGE( IDC_EDIT_EXPMENU_BREAKCOUNTMAX, m_nMaxMenuBreakCount		, 2, 5000 )

		DDX_TEXT( IDC_EDIT_FAVMENU_USER 			, m_strFolder		)

		DDX_CHECK( IDC_CHECK_FAVMENU_USER			, s_bUserFolder		)
		DDX_CHECK( IDC_CHECK_DRAW_ICON				, s_bDrawIcon		)
		DDX_CHECK( IDC_CHECK_SPACE_MIN				, s_bSpaceMin		)
		DDX_CHECK( IDC_CHECK_EXPMENU_ADDITIONAL2	, s_bAddSaveFav		)
		DDX_CHECK( IDC_CHECK_MENUORDER				, s_bCstmOrder		)
		DDX_CHECK( IDC_CHECK_RESETORDER				, s_bAddResetOrder	)
		DDX_CHECK( IDC_CHECK_DRAW_FAVICON			, s_bDrawFavicon	)
	END_DDX_MAP()


	// Message map and handlers
	BEGIN_MSG_MAP( COpenURLDlg )
		COMMAND_ID_HANDLER( IDC_BUTTON_FAVMENU_USER, OnBtnFavMenuUser )
		COMMAND_ID_HANDLER_EX( IDC_CHECK_DRAW_ICON		, OnCommandSwitch	)
		COMMAND_ID_HANDLER_EX( IDC_CHECK_DRAW_FAVICON	, OnCommandSwitch	)
		COMMAND_ID_HANDLER_EX( IDC_CHECK_FAVMENU_ORDER	, OnCommandSwitch	)
		COMMAND_ID_HANDLER_EX( IDC_CHECK_FAVMENU_USER	, OnCommandSwitch	)
		CHAIN_MSG_MAP( CPropertyPageImpl<CDonutFavoritesMenuPropertyPage> )
	END_MSG_MAP()


	LRESULT OnBtnFavMenuUser(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void	OnCommandSwitch(UINT uNotifyCode, int nID, CWindow wndCtl);
};



