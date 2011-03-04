/**
 *	@file	DonutToolBar.cpp
 *	@brief	ツールバー
 */

#include "stdafx.h"
#include "DonutToolBar.h"
#include "MtlProfile.h"
#include "DonutPFunc.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace 	MTL;



CDonutToolBar::CDonutToolBar()
{
	m_strToolbarIniPath = GetToolBarFilePath();
}



// Ctor
void CDonutToolBar::DonutToolBar_SetFavoritesMenu(HMENU hMenu, HMENU hMenuUser, HMENU hMenuCSS)
{
	m_menuFavorites 	= hMenu;
	m_menuFavoritesUser = hMenuUser;
	m_menuCSS			= hMenuCSS;
}



LRESULT CDonutToolBar::OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
{
	ATLASSERT( ( (LPNMREBARCHEVRON) pnmh )->wID == GetDlgCtrlID() );

	if ( !PushChevron( pnmh, GetTopLevelParent() ) ) {
		bHandled = FALSE;
		return 1;
	}

	return 0;
}



LRESULT CDonutToolBar::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	::SendMessage(GetTopLevelParent(), WM_SHOW_TOOLBARMENU, 0, 0);
	return 0;
}



LRESULT CDonutToolBar::OnToolTipText(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	CString 		strText   = CExMenuManager::GetToolTip(idCtrl);

	if ( strText.IsEmpty() ) {
		bHandled = FALSE;
		return 0;
	}
  #ifdef UNICODE	//+++ UNICODE 対応
	LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO) pnmh;
	lstrcpyn( pDispInfo->szText, strText, sizeof (pDispInfo->szText) / sizeof (pDispInfo->szText[0]) );
  #else
	LPNMTTDISPINFOA pDispInfo = (LPNMTTDISPINFOA) pnmh;
	lstrcpynA( pDispInfo->szText, strText, sizeof (pDispInfo->szText) / sizeof (pDispInfo->szText[0]) );
  #endif

	return 0;
}



// Implemantation
HMENU CDonutToolBar::_GetDropDownMenu(int nCmdID, bool &bDestroy, bool &bSubMenu)
{
	CMenuHandle 	menu;					// handle

	bDestroy = bSubMenu = true;

	switch (nCmdID) {
	case ID_FILE_NEW:
		menu.LoadMenu(IDR_FILE_NEW);
		break;

	case ID_VIEW_FONT_SIZE:
		menu.LoadMenu(IDR_VIEW_FONT_SIZE);
		break;

	case ID_FILE_NEW_CLIPBOARD2:
		menu.LoadMenu(IDR_FILE_NEW_CLIPBOARD);
		break;

	case ID_VIEW_BACK:
		menu.LoadMenu(IDR_VIEW_BACK);
		{
			HWND hChildFrm = ::GetParent( GetParent() );
			::SendMessage(hChildFrm, WM_MENU_GOBACK, (WPARAM) (HMENU) menu.m_hMenu, (LPARAM) 0);
		}
		break;

	case ID_VIEW_FORWARD:
		menu.LoadMenu(IDR_VIEW_FORWARD);
		{
			HWND hChildFrm = ::GetParent( GetParent() );
			::SendMessage(hChildFrm, WM_MENU_GOFORWARD, (WPARAM) (HMENU) menu.m_hMenu, (LPARAM) 0);
		}
		break;

	case ID_FAVORITES_DROPDOWN:
		menu	 = m_menuFavorites;
		bDestroy = bSubMenu = false;	// it's not mine.
		break;

	//case ID_MAIN_EX_NEWWINDOW:
	//	menu.LoadMenu(IDR_MENU_FAVTREE_BAR);
	//	break;

	case ID_MULTIMEDIA:
	case ID_DLCTL_CHG_MULTI:
		menu.LoadMenu(IDR_MULTIMEDIA);
		break;

	case ID_SECURITY:
	case ID_DLCTL_CHG_SECU:
		menu.LoadMenu(IDR_SECURITY);
		break;

	case ID_COOKIE:
	case ID_URLACTION_COOKIES_CHG:
		menu.LoadMenu(IDR_COOKIE);
		break;

	case ID_TOOLBAR:
		menu.LoadMenu(IDR_TOOLBAR);
		break;

	case ID_EXPLORERBAR:
		menu.LoadMenu(IDR_EXPLORERBAR);
		//menu.LoadMenu(IDR_MENU_FAVTREE_BAR);
		break;

	case ID_MOVE:
		menu.LoadMenu(IDR_MOVE);
		break;

	case ID_OPTION:
		menu.LoadMenu(IDR_OPTION);
		break;

	case ID_AUTO_REFRESH:
		menu.LoadMenu(IDR_AUTO_REFRESH);
		break;

	case ID_DOUBLE_CLOSE:
		menu.LoadMenu(IDR_DOUBLE_CLOSE);
		break;

	case ID_COOKIE_IE6:
		menu.LoadMenu(IDR_COOKIE_IE6);
		break;

	case ID_FAVORITES_GROUP_DROPDOWN:
		menu	 = m_menuFavoritesUser;
		bDestroy = bSubMenu = false;	// it's not mine.
		break;

	case ID_CSS_DROPDOWN:
		menu	 = m_menuCSS;
		bDestroy = bSubMenu = false;	// it's not mine.
		break;

	case ID_RECENT_DOCUMENT:
		menu.LoadMenu(IDR_RECENT_DOC);
		::SendMessage(GetTopLevelParent(), WM_MENU_RECENTDOCUMENT, (WPARAM) (HMENU) menu.GetSubMenu(0), (LPARAM) 0);
		break;

	default:
		ATLASSERT(FALSE);
	}

	return menu.m_hMenu;
}



// Overrides
HMENU CDonutToolBar::ChevronHandler_OnGetChevronMenu(int nCmdID, HMENU &hMenuDestroy)
{
	//		if (nCmdID == ID_VIEW_FAVEXPBAR)
	//			return NULL;// give up.

	bool		bDestroy = 0;
	bool		bSubMenu = 0;
	CMenuHandle menu = _GetDropDownMenu(nCmdID, bDestroy, bSubMenu);

	if (bDestroy)
		hMenuDestroy = menu.m_hMenu;

	if (bSubMenu)
		return menu.GetSubMenu(0);
	else
		return menu;
}



void CDonutToolBar::Chevronhandler_OnCleanupChevronMenu()
{
	// m_menuFavorites.RemoveMenu(ID_FAVORITES_DROPDOWN, MF_BYCOMMAND);
}



LRESULT CDonutToolBar::StdToolBar_OnDropDown(int nCmdID)
{
	bool		bDestroy;
	bool		bSubMenu;
	CMenuHandle menu = _GetDropDownMenu(nCmdID, bDestroy, bSubMenu);

	if (menu.m_hMenu) {
		if (bSubMenu)
			StdToolBar_TrackDropDownMenu( nCmdID, menu.GetSubMenu(0), GetTopLevelParent() );
		else
			StdToolBar_TrackDropDownMenu( nCmdID, menu, GetTopLevelParent() );
	}

	if (bDestroy)
		menu.DestroyMenu();

	return TBDDRET_DEFAULT;
}



void CDonutToolBar::StdToolBar_WriteProfile()
{
	CString 	strFile = m_strToolbarIniPath;
	CIniFileO	pr( strFile, _T("ToolBar") );

	MtlWriteProfileTBBtns(pr, m_hWnd);
	pr.SetValue( m_dwStdToolBarStyle, _T("Std_ToolBar_Style") );
}



void CDonutToolBar::StdToolBar_GetProfile()
{
	CString 	strFile = m_strToolbarIniPath;
	CIniFileI	pr( strFile, _T("ToolBar") );

	CSimpleArray<int>	arrBmpIndex;

	if ( MtlGetProfileTBBtns(pr, arrBmpIndex) )
		StdToolBar_InitButtons( _begin(arrBmpIndex), _end(arrBmpIndex) );
	else
		StdToolBar_InitButtons( _begin(m_arrBmpDefaultIndex), _end(m_arrBmpDefaultIndex) );

	DWORD		dwStyle = STD_TBSTYLE_DEFAULT;
	pr.QueryValue( dwStyle, _T("Std_ToolBar_Style") );

	StdToolBar_SetStyle(dwStyle, true);
}



// Methods
HWND CDonutToolBar::DonutToolBar_Create(HWND hWndParent)
{
	STD_TBBUTTON *	 pBtns		   = NULL;
	int 			 nBtnCnt	   = GetToolBarStatus(pBtns);
  #if 1	//+++ +mod版のtoolbar.ini互換にする...
	static const int defaultBtns[] = { 0, 1, 2, 3, 4, 5, 51, 60, -1, 9, 10, -1, 12 };
  #else
	static const int defaultBtns[] = { 0, 1, 2, 3, 4, 5, -1, 9, 10, 11, -1, 12 };
  #endif

	StdToolBar_Init( pBtns,
					pBtns + nBtnCnt,
					defaultBtns,
					defaultBtns + _countof(defaultBtns),
					std::make_pair(IDB_MAINFRAME_TOOLBAR, IDB_MAINFRAME_TOOLBAR_HOT),
					std::make_pair(IDB_MAINFRAME_LARGE_TOOLBAR, IDB_MAINFRAME_LARGE_TOOLBAR_HOT),
					CSize(16, 16),
					CSize(20, 20),
					RGB(255, 0, 255) );

	delete[] pBtns;
	return StdToolBar_Create(hWndParent,
							 ATL_SIMPLE_TOOLBAR_PANE_STYLE |  CCS_ADJUSTABLE | TBSTYLE_ALTDRAG,
							 TBSTYLE_EX_MIXEDBUTTONS | TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_HIDECLIPPEDBUTTONS);
}



CString CDonutToolBar::GetToolBarFilePath()
{
  #if 1	//+++
	CString 	strSkinDir = _GetSkinDir();
	CString 	strPath    = strSkinDir + _T("Toolbar.ini");
	if (::GetFileAttributes(strPath) == 0xFFFFFFFF) {
		if (::GetFileAttributes(strSkinDir) == 0xFFFFFFFF) {	//+++ SKINフォルダが無い場合は、別のところを対象にする.
			CIniFileI	pr( g_szIniFileName, _T("Path") );
			CString 	strFile = pr.GetStringUW( _T("ToolbarPath") );
			pr.Close();
			if ( strFile.IsEmpty() )
				strPath = Misc::GetExeDirectory() + _T("Toolbar\\Toolbar.ini");
			else
				strPath = strFile + _T("\\Toolbar.ini");
		  #if 1	//+++ フォルダがない場合、終了時に、エラー終了になりやがるので、とりあえず、吐き出せるようにする
			if (::GetFileAttributes(strPath) == 0xFFFFFFFF) {
				strPath = Misc::GetExeDirectory() + _T("Toolbar.ini");
			}
		  #endif
		}
	}
	return	strPath;
  #else
	CString 	strPath = _GetSkinDir() + _T("Toolbar.ini");
	if (::GetFileAttributes(strPath) == 0xFFFFFFFF) {
		CIniFileI	pr( g_szIniFileName, _T("Path") );
		CString 	strFile = pr.GetStringUW( _T("ToolbarPath") );
		pr.Close();
		if ( strFile.IsEmpty() )
			strPath = Misc::GetExeDirectory() + _T("Toolbar\\Toolbar.ini");
		else
			strPath = strFile + _T("\\Toolbar.ini");
	  #if 1	//+++ フォルダがない場合、終了時に、エラー終了になりやがるので、とりあえず、吐き出せるようにする
		if (::GetFileAttributes(strPath) == 0xFFFFFFFF) {
			strPath = Misc::GetExeDirectory() + _T("Toolbar.ini");
		}
	  #endif
	}
	return	strPath;
  #endif
}



int CDonutToolBar::GetToolBarStatus(STD_TBBUTTON * &pBtns)
{
	pBtns	= NULL;

	// ツールバーファイルパス
	CString 	strFile(m_strToolbarIniPath);

	// ツールバー番号
	DWORD		dwCount = 0;
	{
		CIniFileI	pr( strFile, _T("TOOLBAR") );
		pr.QueryValue( dwCount, _T("TOOLBAR_CNT") );
		pr.Close();
	}
	if (dwCount == 0)
		return GetToolBarStatusStd(pBtns);

	// ツールーバー作成
	pBtns	= new STD_TBBUTTON[dwCount];
	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// ツールバー
	CIniFileI		pr( strFile, _T("TOOLBAR") );

	for (int ii = 0; ii < (int) dwCount; ii++) {
		CString 	strKeyID;
		CString 	strKeyStyle;
		strKeyID.Format(_T("ID_%d"), ii);
		strKeyStyle.Format(_T("STYLE_%d"), ii);

		DWORD	dwID = 0, dwStyle = 0;
		pr.QueryValue(dwID	 , strKeyID);
		pr.QueryValue(dwStyle, strKeyStyle);

		pBtns[ii].idCommand = dwID;
		pBtns[ii].fsStyle	= dwStyle;
	}

	pr.Close();
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	return dwCount;
}



int CDonutToolBar::GetToolBarStatusStd(STD_TBBUTTON * &pBtns)
{
	static const STD_TBBUTTON btns[] = {
		{ ID_FILE_NEW,					BTNS_BUTTON | BTNS_DROPDOWN 											 },	//  1
		{ ID_VIEW_BACK, 				BTNS_BUTTON | BTNS_STD_LIST | BTNS_DROPDOWN 							 },	//  2
		{ ID_VIEW_FORWARD,				BTNS_BUTTON | BTNS_STD_LIST | BTNS_DROPDOWN 							 }, //	3
		{ ID_VIEW_STOP, 				BTNS_BUTTON 															 },	//	4
		{ ID_VIEW_REFRESH,				BTNS_BUTTON 															 },	//  5
		{ ID_VIEW_HOME, 				BTNS_BUTTON 															 },	//	6
		{ ID_FILE_NEW_CLIPBOARD2,		BTNS_BUTTON | BTNS_DROPDOWN 											 },	//	7

		{ ID_VIEW_STOP_ALL, 			BTNS_BUTTON 															 },	//	8
		{ ID_VIEW_REFRESH_ALL,			BTNS_BUTTON 															 },	//	9

		{ ID_VIEW_FAVEXPBAR,			BTNS_BUTTON | BTNS_STD_LIST 											 }, //BTNS_DROPDOWN},
		{ ID_VIEW_FAVEXPBAR_HIST,		BTNS_BUTTON | BTNS_STD_LIST 											 },	// 10
		{ ID_VIEW_CLIPBOARDBAR, 		BTNS_BUTTON | BTNS_STD_LIST 											 },	// 11

		{ ID_FILE_PRINT,				BTNS_BUTTON 															 },	// 12

		{ ID_VIEW_FULLSCREEN,			BTNS_BUTTON 															 },	// 13
		{ ID_WINDOW_CASCADE,			BTNS_BUTTON 															 },	// 14
		{ ID_WINDOW_TILE_HORZ,			BTNS_BUTTON 															 },	// 15
		{ ID_WINDOW_TILE_VERT,			BTNS_BUTTON 															 },	// 16

		{ ID_VIEW_TABBAR_MULTI, 		BTNS_BUTTON 															 },	// 17
		{ ID_TAB_LEFT,					BTNS_BUTTON 															 },	// 18
		{ ID_TAB_RIGHT, 				BTNS_BUTTON 															 },	// 19

		{ ID_EDIT_CUT,					BTNS_BUTTON 															 },	// 20
		{ ID_EDIT_COPY, 				BTNS_BUTTON 															 },	// 21
		{ ID_EDIT_PASTE,				BTNS_BUTTON 															 },	// 22

		{ ID_VIEW_FONT_SIZE,			BTNS_BUTTON | BTNS_DROPDOWN | BTNS_WHOLEDROPDOWN						 },	// 23
		{ ID_DOCHOSTUI_OPENNEWWIN,		BTNS_BUTTON 															 },	// 24

		{ ID_DLCTL_DLIMAGES,			BTNS_BUTTON 															 },	// 25
		{ ID_DLCTL_SCRIPTS, 			BTNS_BUTTON 															 },	// 26
		{ ID_DLCTL_JAVA,				BTNS_BUTTON 															 },	// 27
		{ ID_DLCTL_RUNACTIVEXCTLS,		BTNS_BUTTON 															 },	// 28
		{ ID_DLCTL_DLACTIVEXCTLS,		BTNS_BUTTON 															 },	// 29
		{ ID_DLCTL_BGSOUNDS,			BTNS_BUTTON 															 },	// 30
		{ ID_DLCTL_VIDEOS,				BTNS_BUTTON 															 },	// 31

		{ ID_FILE_CLOSE,				BTNS_BUTTON 															 },	// 32
		{ ID_WINDOW_CLOSE_ALL,			BTNS_BUTTON 															 },	// 33
		{ ID_WINDOW_CLOSE_EXCEPT,		BTNS_BUTTON 															 },	// 34

		{ ID_MAIN_EX_NEWWINDOW, 		BTNS_BUTTON 															 },	// 35
		{ ID_MAIN_EX_NOACTIVATE,		BTNS_BUTTON 															 },	// 36
		{ ID_MAIN_EX_NOACTIVATE_NEWWIN, BTNS_BUTTON 															 },	// 37
		{ ID_REGISTER_AS_BROWSER,		BTNS_BUTTON 															 },	// 38
		{ ID_FAVORITES_DROPDOWN,		BTNS_BUTTON | BTNS_STD_LIST | BTNS_DROPDOWN | BTNS_WHOLEDROPDOWN		 },	// 39
		{ ID_EDIT_OPEN_SELECTED_REF,	BTNS_BUTTON 															 },	// 40
		{ ID_EDIT_OPEN_SELECTED_TEXT,	BTNS_BUTTON 															 },	// 41

		// U.H
		{ ID_WINDOW_REFRESH_EXCEPT, 	BTNS_BUTTON 															 },	// 42

		//+++
		{ ID_VIEW_UP				,	BTNS_BUTTON 															 },	// 43
		{ ID_VIEW_FAVEXPBAR_GROUP	,	BTNS_BUTTON 															 },	// 44
		{ ID_VIEW_SOURCE			,	BTNS_BUTTON 															 },	// 45
		{ ID_VIEW_SOURCE_SELECTED	,	BTNS_BUTTON 															 },	// 46
		{ 57609						,	BTNS_BUTTON 															 },	// 47	//+++ "印刷プレビュー(&V)..."
		{ 57636						,	BTNS_BUTTON 															 },	// 48	//+++ "このページの検索(&F)..."
		{ ID_EDIT_FIND_MAX			,	BTNS_BUTTON 															 },	// 49
		{ ID_VIEW_SEARCHBAR			,	BTNS_BUTTON 															 },	// 50
		{ ID_RECENT_DOCUMENT		,	BTNS_BUTTON | BTNS_DROPDOWN												 },	// 51
		{ ID_DLCTL_CHG_SECU			,	BTNS_BUTTON | BTNS_DROPDOWN												 },	// 52
		{ ID_DLCTL_CHG_MULTI		,	BTNS_BUTTON | BTNS_DROPDOWN												 },	// 53
		{ ID_URLACTION_COOKIES_CHG	,	BTNS_BUTTON | BTNS_DROPDOWN												 },	// 54
		{ ID_VIEW_FAVEXPBAR_USER	,	BTNS_BUTTON 															 },	// 55
		{ ID_URLACTION_COOKIES		,	BTNS_BUTTON 															 },	// 56
		{ ID_URLACTION_COOKIES_SESSION,	BTNS_BUTTON 															 },	// 57
		{ ID_FAVORITES_GROUP_DROPDOWN,	BTNS_BUTTON | BTNS_DROPDOWN | BTNS_WHOLEDROPDOWN						 },	// 58
		{ ID_AUTO_REFRESH			,	BTNS_BUTTON | BTNS_DROPDOWN | BTNS_WHOLEDROPDOWN						 },	// 59
		{ ID_VIEW_OPTION_DONUT		,	BTNS_BUTTON 															 },	// 60
		{ ID_VIEW_OPTION			,	BTNS_BUTTON 															 },	// 61
		{ ID_APP_HELP				,	BTNS_BUTTON 															 },	// 62
		{ ID_LEFT_CLOSE				,	BTNS_BUTTON 															 },	// 63
		{ ID_RIGHT_CLOSE			,	BTNS_BUTTON 															 },	// 64
		//{ ID_SHOW_DLMANAGER			,	BTNS_BUTTON																 }, // 65?
		//{ 0						,	BTNS_BUTTON 															 },
		//{ 0						,	BTNS_BUTTON 															 },
	};

	pBtns = new STD_TBBUTTON[_countof(btns)];
	memcpy( pBtns, btns, _countof(btns) * sizeof (STD_TBBUTTON) );
	return _countof(btns);
}



void CDonutToolBar::ReloadSkin()
{
	StdToolBar_WriteProfile(); //状態を保存

	//イメージリスト及びボタンの再構築
	m_strToolbarIniPath = GetToolBarFilePath();
	StdToolBar_DestroyImageList();
	StdToolBar_UpdateImageList();
	InvalidateRect(NULL, TRUE);
	StdToolBar_GetProfile();
}
