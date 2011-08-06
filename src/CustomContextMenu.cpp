// CustomContextMenu.cpp

#include "stdafx.h"
#include "CustomContextMenu.h"
#include "Download/DownloadManager.h"
#include "MainFrame.h"


///////////////////////////////////////////////////////////////
// CCustomContextMenu

// Constructor
CCustomContextMenu::CCustomContextMenu() 
	: m_hWndTopLevel(NULL)
	, m_hWndTarget(NULL)
	, m_nContextMenuMode(0)
	, m_spDisp(NULL)
{
	s_pThis = this; 
}

// Destructor
CCustomContextMenu::~CCustomContextMenu()
{
	s_pThis  = NULL;
	m_spDisp = NULL;
}

HRESULT CCustomContextMenu::Show(DWORD dwId, DWORD x, DWORD y, IUnknown* pCommandTarget, IDispatch* pDisp)
{
	// ウィンドウハンドルが欲しい。pCommandTargetから取れないだろうか
	CComQIPtr<IOleWindow>	pWindow = pCommandTarget;
	ATLASSERT(pWindow);

	HRESULT 	hr	  = pWindow->GetWindow(&m_hWndTarget);
	ATLASSERT(m_hWndTarget);

	m_hWndTopLevel = CWindow(m_hWndTarget).GetTopLevelWindow();

	CPoint		pt(x, y);
	return ShowContextMenu(dwId, &pt, pCommandTarget, pDisp);
}

// private:

HRESULT CCustomContextMenu::ShowContextMenuEx(POINT *pptPosition)
{
	//まずメニュー用のリソースが使用されていればそれを開放する
	::SendMessage(m_hWndTopLevel, WM_RELEASE_CONTEXTMENU, 0, 0);

	CMenu			menu;
	menu.CreatePopupMenu();

	MENUITEMINFO	menuInfo;
	memset( &menuInfo, 0, sizeof (MENUITEMINFO) );
	menuInfo.cbSize = sizeof (MENUITEMINFO);
	menuInfo.fMask	= MIIM_ID | MIIM_TYPE;

	CSimpleArray<HMENU> aryDestroyMenu;

	CString 		strPath = _GetFilePath( _T("Menu.ini") );
	CIniFileI		pr(strPath, _T("MenuEx"));
	DWORD			dwCount = pr.GetValue(_T("Count"), 0);
	CString 		strKey;

	for (DWORD ii = 0; ii < dwCount; ++ii) {
		strKey.Format(_T("%02d"), ii);
		DWORD		dwCmd = pr.GetValue(strKey, 0);

		CString 	strCmd;
		CToolTipManager::LoadToolTipText(dwCmd, strCmd);

		if (strCmd.IsEmpty() == FALSE) {
			menuInfo.fType		= MFT_STRING;
			menuInfo.wID		= dwCmd;
			#if 1	//+++
			menuInfo.dwTypeData = LPTSTR(LPCTSTR(strCmd));
			#else
			menuInfo.dwTypeData = strCmd.GetBuffer(0);
			#endif
			menuInfo.cch		= strCmd.GetLength();
		} else {
			menuInfo.fType = MFT_SEPARATOR;
		}

		::InsertMenuItem(menu.m_hMenu, ii, MF_BYPOSITION, &menuInfo);
		CstmContextMenuDropdown(menu.m_hMenu, dwCmd, aryDestroyMenu);

		//特殊メニューの初期化 minit
		_BeforeInitSpecialMenu(dwCmd);
	}
	pr.Close();

	// 編集しないメニューとする
	menuInfo.fType = MFT_SEPARATOR;
	::InsertMenuItem(menu.m_hMenu, 0, MF_BYPOSITION, &menuInfo);

	//表示中はメインメニューの方に送るメッセージを制限する minit
	::SendMessage(m_hWndTopLevel, WM_MENU_RESTRICT_MESSAGE, (WPARAM) TRUE, 0);

	// Show shortcut menu
	CIniFileI		pr2(strPath, _T("Option"));
	DWORD	dwREqualL	= pr2.GetValue(_T("REqualL"), 0);
	pr2.Close();

	DWORD	dwMenuStyle = TPM_LEFTALIGN | TPM_RETURNCMD;
	if (dwREqualL)
		dwMenuStyle |= TPM_RIGHTBUTTON;

	int 	iSelection	= ::TrackPopupMenu(   menu.m_hMenu
											, dwMenuStyle
											, pptPosition->x
											, pptPosition->y
											, 0
											, GetTopLevelWindow()
											, (RECT *) NULL);

	for (int jj = 0; jj < aryDestroyMenu.GetSize(); jj++)
		::DestroyMenu(aryDestroyMenu[jj]);

	::SendMessage(m_hWndTopLevel, WM_COMMAND, iSelection, NULL);

	//メッセージの制限を解除する minit
	::PostMessage(m_hWndTopLevel, WM_MENU_RESTRICT_MESSAGE, (WPARAM) FALSE, 0);

	return S_OK;
}


// MSHTML requests to display its context menu
HRESULT CCustomContextMenu::ShowContextMenu(DWORD dwID, POINT* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit)
{
	m_spDisp = pDispatchObjectHit;

	m_nContextMenuMode			= 0;	//+++
	// まずメニュー用のリソースが使用されていればそれを開放する
	::SendMessage(m_hWndTopLevel, WM_RELEASE_CONTEXTMENU, 0, 0);

	if (::GetKeyState(VK_LBUTTON) < 0) {	// || ::GetKeyState(VK_LBUTTON)<0) //なんじゃこりゃ
		dwID = CONTEXT_MENU_HOLDLEFTBUTTON;
		//return ShowContextMenuEx(pptPosition);
	}

	if (   dwID != CONTEXT_MENU_DEFAULT
		&& dwID != CONTEXT_MENU_IMAGE
		&& dwID != CONTEXT_MENU_TEXTSELECT
		&& dwID != CONTEXT_MENU_ANCHOR
		&& dwID != CONTEXT_MENU_HOLDLEFTBUTTON)
	{
		return S_FALSE;
	}


	m_nContextMenuMode			= dwID;	//+++
	if (pptPosition) 					//+++
		m_pt	= *pptPosition;			//+++
	
	if (dwID == CONTEXT_MENU_ANCHOR) {
		if (pDispatchObjectHit) {
			CComQIPtr<IHTMLAnchorElement>	spAnchor = pDispatchObjectHit;
			ATLASSERT(spAnchor);
			CComBSTR strURL;
			HRESULT hr = spAnchor->get_href(&strURL);
			if (strURL) {
				m_strUrl = strURL;
				goto ANCHORFINISH;
			}
		}
		m_strUrl = Donut_GetActiveStatusStr();
	}
	ANCHORFINISH:


	enum {
		IDR_BROWSE_CONTEXT_MENU 	= 24641,
		IDR_FORM_CONTEXT_MENU		= 24640,
		SHDVID_GETMIMECSETMENU		=	 27,
		SHDVID_ADDMENUEXTENSIONS	=	 53,
	};

	HRESULT	hr = 0;
	CComQIPtr<IOleCommandTarget>	spOleCommandTarget = pCommandTarget;
	ATLASSERT(spOleCommandTarget);

	CMenuHandle	menu = CCustomContextMenuOption::GetContextMenuFromID(dwID);

	if (dwID == CONTEXT_MENU_DEFAULT) {	
		// Get the language submenu	// エンコードのサブメニューを追加 これをするとメニューの表示に時間がかかる...
		VARIANT var;
		hr = spOleCommandTarget->Exec(&CGID_ShellDocView, SHDVID_GETMIMECSETMENU, 0, NULL, &var);

		MENUITEMINFO	mii = { sizeof(MENUITEMINFO) };
		mii.fMask	   = MIIM_SUBMENU;
		mii.hSubMenu   = (HMENU) var.byref;

		#ifndef IDM_LANGUAGE
		enum { IDM_LANGUAGE 	= 2292 };
		#endif
		menu.SetMenuItemInfo(IDM_LANGUAGE, FALSE, &mii);
	}

	{	// Insert Shortcut Menu Extensions from registry
		VARIANT					var1;
		VARIANT					var2;

		V_VT(&var1)    = VT_INT_PTR;
		V_BYREF(&var1) = menu.m_hMenu;

		V_VT(&var2)    = VT_I4;
		V_I4(&var2)    = dwID;

		hr = spOleCommandTarget->Exec(&CGID_ShellDocView, SHDVID_ADDMENUEXTENSIONS, 0, &var1, &var2);
	}
	/* メニューの有効/無効を設定 */
	_SetMenuEnable(menu, spOleCommandTarget);


	CSimpleArray<HMENU> arrDestroyMenu;
	CCustomContextMenuOption::AddSubMenu(menu, m_hWndTopLevel, arrDestroyMenu);


	// 初期化前メッセージを送る
//	for (int ii = 0; ii < mapCmd.GetSize(); ii++) {
//		_BeforeInitSpecialMenu( mapCmd.GetValueAt( ii ) );
//	}

	// 表示中はメインメニューの方に送るメッセージを制限する minit
	::SendMessage(m_hWndTopLevel, WM_MENU_RESTRICT_MESSAGE, (WPARAM) TRUE, 0);

	// Show shortcut menu
	int 	nCmdAddFav	= MtlGetCmdIDFromAccessKey( menu, _T("&F") );

	DWORD	dwMenuStyle = TPM_LEFTALIGN | TPM_RETURNCMD;
	if (CMenuOption::s_bR_Equal_L) dwMenuStyle |= TPM_RIGHTBUTTON;
	
	{	// メニューの右にショートカットキーを表示しないようにする(メインフレームで設定されるのを防ぐ)
		MENUITEMINFO menuInfo = { sizeof (MENUITEMINFO) };
		menuInfo.fMask	= MIIM_TYPE;
		menuInfo.fType	= MF_SEPARATOR;
		menu.InsertMenuItem(0, MF_BYPOSITION, &menuInfo);
	}

	/* メニューを表示 */
	int 	iSelection	= menu.TrackPopupMenu( dwMenuStyle
											  ,pptPosition->x
											  ,pptPosition->y
											  ,m_hWndTopLevel );

	enum { ID_SAVEDIALOG = 2268 };	// 対象をファイルに保存
	if (dwID == CONTEXT_MENU_ANCHOR	
		&& iSelection == ID_SAVEDIALOG 
		&& CDownloadManager::UseDownloadManager()) 
	{	// DLManagerに送る
		CDownloadManager::GetInstance()->SetReferer(g_pMainWnd->GetActiveChildFrame()->GetLocationURL());
		CDownloadManager::GetInstance()->DownloadStart(m_strUrl, NULL, NULL, DLO_SHOWWINDOW);
	} else {
		// Send selected shortcut menu item command to shell
		LRESULT  lRes	= S_OK;
		if (iSelection != 0) {
			lRes = ::SendMessage(m_hWndTarget, WM_COMMAND, iSelection, NULL);
		}

		{	// MainFrameにメッセージを送信するかどうか
	#if 0
		BOOL	bSendFrm = FALSE;

		// コマンド範囲
		if (COMMAND_RANGE_START <= iSelection && iSelection <= COMMAND_RANGE_END)
			bSendFrm = TRUE;

		// お気に入り、グループ
		else if ( (FAVORITE_MENU_ID_MIN <= iSelection && iSelection <= FAVORITE_MENU_ID_MAX)
				||(FAVGROUP_MENU_ID_MIN <= iSelection && iSelection <= FAVGROUP_MENU_ID_MAX) )
			bSendFrm = TRUE;

		// スクリプト
		else if (ID_INSERTPOINT_SCRIPTMENU <= iSelection && iSelection <= ID_INSERTPOINT_SCRIPTMENU_END)
			bSendFrm = TRUE;

		#if 1	//+++ 検索エンジン
		else if (ID_INSERTPOINT_SEARCHENGINE <= iSelection && iSelection <= ID_INSERTPOINT_SEARCHENGINE_END)
			bSendFrm = TRUE;
		#endif

		if ( bSendFrm == FALSE && mapCmd.Lookup( (DWORD) iSelection ) )
			bSendFrm = TRUE;

		if (true/*bSendFrm*/) {		// メインフレームにコマンドを送信する
			lRes = ::SendMessage(m_hWndTopLevel, WM_COMMAND, iSelection, NULL);
		}
	#endif
			//\\ 絶対送信するようにしてみる
			lRes = ::SendMessage(m_hWndTopLevel, WM_COMMAND, iSelection, NULL);

			//この時点でウィンドウが消されてしまう可能性があるけれども・・
			if (iSelection == nCmdAddFav) {
				::SendMessage(m_hWndTopLevel, WM_REFRESH_EXPBAR, 0, 0);
			}
		}
	}

	// ここでお片づけ
	CCustomContextMenuOption::RemoveSubMenu(menu, arrDestroyMenu);


//	for (int ii = 0; ii < mapCmd.GetSize(); ii++) {
//		::RemoveMenu( hMenuSub, mapCmd.GetKeyAt( ii ), MF_BYCOMMAND );
//		_RestoreSpecialMenu( mapCmd.GetValueAt( ii ) );
//	}

	//メッセージの制限を解除する minit
	::PostMessage(m_hWndTopLevel, WM_MENU_RESTRICT_MESSAGE, (WPARAM) FALSE, 0);

	//m_nContextMenuMode = 0;	//+++

	return S_OK;
}



void CCustomContextMenu::_RestoreSpecialMenu(DWORD dwCmd)
{
	switch (dwCmd) {
	case ID_SCRIPT:
		::SendMessage(m_hWndTopLevel, WM_MENU_REFRESH_SCRIPT   , (WPARAM) FALSE, 0);
		break;
	}
}



void CCustomContextMenu::_BeforeInitSpecialMenu(DWORD dwCmd)
{
	switch (dwCmd) {
	case ID_SCRIPT:
		::SendMessage(m_hWndTopLevel, WM_MENU_REFRESH_SCRIPT   , (WPARAM) TRUE, 0);
		break;
	}
}



void CCustomContextMenu::CstmContextMenu(HMENU hMenu, DWORD dwID, CSimpleMap<DWORD, DWORD> &mapCmd, CSimpleArray<HMENU> &aryDestroyMenu)
{
	//コンテキストメニューの「前に戻る」から「デスクトップ項目として設定」の次のセパレータまでを削除
	//for (int ii=0; ii<8 && dwID==CONTEXT_MENU_DEFAULT; ii++)
	for (int ii = 0; ii < 3 && dwID == CONTEXT_MENU_DEFAULT; ii++)	//前に戻ると次に進むだけ消す
		::DeleteMenu(hMenu, 0 , MF_BYPOSITION);

	MENUITEMINFO menuInfo = { sizeof (MENUITEMINFO) };
	menuInfo.fMask	= MIIM_ID | MIIM_TYPE;

	CString 	strSection;
	strSection.Format(_T("Type%d"), dwID);
	CString 	strPath 	= _GetFilePath( _T("Menu.ini") );
	CIniFileI	pr(strPath, strSection);
	DWORD		dwCount 	= pr.GetValue(_T("FrontCount"), 0 );

	//メインメニューからメニューの文字列を取得するため
	//CMenuHandle mh;
	//mh.LoadMenu(IDR_MAINFRAME);

	DWORD		 jj;
	for (jj = 0; jj < dwCount; jj++) {
		CString strKey;
		strKey.Format(_T("Front%02d"), jj);
		DWORD	dwCmd = pr.GetValue(strKey, 0);

		CString strCmd;
		CToolTipManager::LoadToolTipText(dwCmd, strCmd);			//StringTableから文字列を取得
		//mh.GetMenuString(dwCmd, strCmd, MF_BYCOMMAND);			// メインメニューから文字列を取得

		if (strCmd.IsEmpty() == FALSE) {
			menuInfo.fType		= MFT_STRING;
			menuInfo.wID		= dwCmd;
			#if 1	//+++
			menuInfo.dwTypeData = LPTSTR(LPCTSTR(strCmd));
			#else
			menuInfo.dwTypeData = strCmd.GetBuffer(0);
			#endif
			menuInfo.cch		= strCmd.GetLength();

			// コマンドを覚える
			DWORD value = 1;

			if (dwCmd == ID_FAVORITES_DROPDOWN) {
				value = 2;
				//menuInfo.dwTypeData = _T("お気に入り(&A)"); 			//メインメニューから文字列を取得できないため
			} else if (dwCmd == ID_FAVORITES_GROUP_DROPDOWN) {
				value = 3;
				//menuInfo.dwTypeData = _T("お気に入りグループ(&G)"); 	//上に同じ
			} else if (dwCmd == ID_SCRIPT) {
				value = 4;
				//menuInfo.dwTypeData = _T("スクリプト(&S)"); 			//上に同じ
			}

			mapCmd.Add(dwCmd, value);
		} else {
			menuInfo.fType = MFT_SEPARATOR;
		}

		::InsertMenuItem(hMenu, jj, MF_BYPOSITION, &menuInfo);

		CstmContextMenuDropdown(hMenu, dwCmd, aryDestroyMenu);
	}

	dwCount = pr.GetValue(_T("BackCount"), 0);

	for (jj = 0; jj < dwCount; jj++) {
		CString 	strKey;
		strKey.Format(_T("Back%02d"), jj);
		DWORD	dwCmd = pr.GetValue(strKey, 0);

		CString 	strCmd;
		CToolTipManager::LoadToolTipText(dwCmd, strCmd);

		if (strCmd.IsEmpty() == FALSE) {
			menuInfo.fType		= MFT_STRING;
			menuInfo.wID		= dwCmd;
			#if 1	//+++
			menuInfo.dwTypeData = LPTSTR(LPCTSTR(strCmd));
			#else
			menuInfo.dwTypeData = strCmd.GetBuffer(0);
			#endif
			menuInfo.cch		= strCmd.GetLength();

			// コマンドを覚える
			mapCmd.Add(dwCmd, 1);
		} else {
			menuInfo.fType = MFT_SEPARATOR;
		}

		::InsertMenuItem(hMenu, ::GetMenuItemCount(hMenu), MF_BYPOSITION, &menuInfo);

		CstmContextMenuDropdown(hMenu, dwCmd, aryDestroyMenu);
	}

	pr.Close();

	// 編集しないメニューとする
	menuInfo.fType = MFT_SEPARATOR;
	::InsertMenuItem(hMenu, 0, MF_BYPOSITION, &menuInfo);
}


void CCustomContextMenu::CstmContextMenuDropdown(HMENU hMenu, DWORD dwCmd, CSimpleArray<HMENU> &aryDestroyMenu)
{
	MENUITEMINFO mii  = { sizeof (MENUITEMINFO) };
	mii.fMask  = MIIM_SUBMENU;

	CMenuHandle  menu;
	DWORD		 dwID = 0;

	switch (dwCmd) {
	case ID_FAVORITES_DROPDOWN:
		mii.hSubMenu = (HMENU) ::SendMessage(m_hWndTopLevel, WM_MENU_GET_FAV	  , 0, 0);
		break;

	case ID_FAVORITES_GROUP_DROPDOWN:
		mii.hSubMenu = (HMENU) ::SendMessage(m_hWndTopLevel, WM_MENU_GET_FAV_GROUP, 0, 0);
		break;

	case ID_SCRIPT:
		mii.hSubMenu = (HMENU) ::SendMessage(m_hWndTopLevel, WM_MENU_GET_SCRIPT   , 0, 0);
		break;

	case ID_SEARCHENGINE_MENU:
		{
			CMenuHandle menu = CDonutSearchBar::GetInstance()->GetSearchEngineMenuHandle();
			mii.hSubMenu	 = menu.m_hMenu;
		}
		break;

	case ID_DLCTL_CHG_MULTI:	dwID = IDR_MULTIMEDIA;		break;
	case ID_DLCTL_CHG_SECU: 	dwID = IDR_SECURITY;		break;
	case ID_VIEW_FONT_SIZE: 	dwID = IDR_VIEW_FONT_SIZE;	break;
	case ID_COOKIE_IE6: 		dwID = IDR_COOKIE_IE6;		break;
	case ID_HTMLZOOM_MENU: 		dwID = IDR_ZOOM_MENU;		break;		//+++
	default:
		return;
		break;
	}

	if (dwID != 0) {
		menu.LoadMenu(dwID);
		HMENU	hMenuSub = menu.GetSubMenu(0);
		mii.hSubMenu	 = hMenuSub;
		aryDestroyMenu.Add(hMenuSub);
	}

	::SetMenuItemInfo(hMenu, dwCmd, FALSE, &mii);
}



// thinks for http://www.usefullcode.net/2009/03/ie_contextmenu.html
//			  http://www.ailight.jp/blog/sha256/archive/2005/10/13/9905.aspx
//			  http://msdn.microsoft.com/en-us/library/ms688491
// メニューの有効・無効をspOleCommandTargetから読み込む
void	CCustomContextMenu::_SetMenuEnable(HMENU hMenu, IOleCommandTarget* spOleCommandTarget)
{
	for (int i = 0; i < GetMenuItemCount(hMenu); ++i) {
		OLECMD olecmd = { 0 };
		olecmd.cmdID = GetMenuItemID(hMenu, i);
		HRESULT hr = spOleCommandTarget->QueryStatus(&CGID_MSHTML, 1, &olecmd, NULL);
		if (SUCCEEDED(hr)) {
			if (olecmd.cmdf & OLECMDF_SUPPORTED) {
				UINT mf = MF_BYCOMMAND;
				mf |= (olecmd.cmdf & OLECMDF_ENABLED) ? MF_ENABLED : MF_DISABLED;
				mf |= (olecmd.cmdf & OLECMDF_LATCHED) ? MF_CHECKED : MF_UNCHECKED;
				::EnableMenuItem(hMenu, olecmd.cmdID, mf);
				::CheckMenuItem(hMenu, olecmd.cmdID, mf);
			}
		}
	}
}


















