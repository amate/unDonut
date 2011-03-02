/**
 *	@file	MenuEncode.h
 *	@brief	エンコード選択メニュー.
 */
#ifndef __MENUENCODE_H_
#define __MENUENCODE_H_

#include <mshtmcid.h>


///////////////////////////////////////////////////
// CMenuEncode

class ATL_NO_VTABLE CMenuEncode 
{
	CMenuHandle 				m_Menu;
	CMenuHandle 				m_MenuSub;

	CMenuHandle 				m_CtxMenu;
	CMenuHandle 				m_CtxMenuSub;
	CMenuHandle 				m_EncodeMenu;

	enum { ID_START = 0x6000, ID_END   = 0x6d00 };

	int 						m_posStart;
	int 						m_insertPoint;
	HWND						m_hWnd;
	HWND						m_hWndMDIClient;
	HWND						m_hWndTarget;

	HINSTANCE					m_hInstDLL;

	CComPtr<IOleCommandTarget>	spCT;
	CComPtr<IOleWindow> 		spWnd;

	BOOL						m_bFlag;


public:
	CMenuEncode()
	{
		m_posStart		= 0;
		m_insertPoint	= 0;
		m_hWnd			= NULL;
		m_hWndMDIClient = NULL;
		m_hInstDLL		= NULL;
		m_hWndTarget	= NULL;
		m_bFlag 		= FALSE;
	}

	~CMenuEncode()
	{
		spCT.Detach();
		spWnd.Detach();
	}


	// Message map and handlers
	BEGIN_MSG_MAP( CMenuEncode )
		MSG_WM_INITMENUPOPUP( OnInitMenuPopup )
		USER_MSG_WM_RELEASE_CONTEXTMENU( OnReleaseContextMenu )
		MESSAGE_HANDLER( WM_EXITMENULOOP, OnExitMenuLoop )
		COMMAND_RANGE_HANDLER_EX( ID_START, ID_END, OnRangeCommand )
	END_MSG_MAP()


	void Init(HMENU hMenu, HWND hWnd, int insertPoint)
	{
		m_Menu		  = hMenu;
		m_hWnd		  = hWnd;
		m_insertPoint = insertPoint;
	}


private:

	void OnInitMenuPopup(HMENU hmenuPopup, UINT uPos, BOOL fSystemMenu)
	{
		SetMsgHandled(FALSE);

		HWND hWndActive = DonutGetActiveWindow(m_hWnd);

		if (hmenuPopup == m_Menu.m_hMenu) {

			ReleaseMenu();
			if (hWndActive) CreateMenu();

			MENUITEMINFO mii = { sizeof (MENUITEMINFO) };
			mii.fMask = MIIM_TYPE;
			m_Menu.GetMenuItemInfo(m_insertPoint, TRUE, &mii);
			if (mii.fType & MFT_SEPARATOR) {
				mii.fMask	   = MIIM_SUBMENU | MIIM_TYPE;
				mii.dwTypeData = _T("エンコード(&M)");
				mii.cch 	   = 15;
				mii.fType	   = MFT_STRING;
				mii.hSubMenu   = m_EncodeMenu.m_hMenu;
				m_Menu.InsertMenuItem(m_insertPoint, TRUE, &mii);
			}

		}

		if (!hWndActive) {
			EnableMenuItem(m_Menu.m_hMenu, m_insertPoint, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		} else
			EnableMenuItem(m_Menu.m_hMenu, m_insertPoint, MF_BYPOSITION | MF_ENABLED);
	}


	LRESULT OnReleaseContextMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		ReleaseMenu();
		return 0;
	}


	void ReleaseMenu()
	{
		if ( m_EncodeMenu.IsMenu() ) { m_EncodeMenu.DestroyMenu(); }
		m_posStart	 = 0;
		m_hWndTarget = NULL;
		if (spCT)		{ spCT.Release() ;			 spCT		= NULL; }
		if (spWnd)		{ spWnd.Release();			 spWnd		= NULL; }
		if (m_hInstDLL) { ::FreeLibrary(m_hInstDLL); m_hInstDLL = NULL; }
	}


	void ClearMenu()
	{
		MENUITEMINFO mii = { sizeof (MENUITEMINFO) };
		mii.fMask = MIIM_TYPE;
		m_Menu.GetMenuItemInfo(m_insertPoint, TRUE, &mii);
		if ( !(mii.fType & MFT_SEPARATOR) )
			RemoveMenu(m_Menu.m_hMenu, m_insertPoint, MF_BYPOSITION);

		if ( m_CtxMenu.IsMenu() ) {
			m_CtxMenu.DestroyMenu();
		}
	}


	LRESULT OnExitMenuLoop(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		ClearMenu();
		return 0;
	}


	void OnRangeCommand(UINT, int nID, HWND hWndCtrl)
	{
		MENUITEMINFO mii   = { sizeof (MENUITEMINFO) };
		mii.fMask = MIIM_DATA;

		HMENU		 hMenu = _SearchSubMenu(nID, m_EncodeMenu.m_hMenu);
		if (!hMenu)  return;

		::GetMenuItemInfo(hMenu, nID, FALSE, &mii);

		if (m_hWndTarget) {
			::SendMessage(m_hWndTarget, WM_COMMAND, mii.dwItemData, 0);
		}
	}


	HMENU _SearchSubMenu(int nID, HMENU hParentMenu)
	{
		HMENU			hRet;
		MENUITEMINFO	mii   = { sizeof (MENUITEMINFO) };
		mii.fMask = MIIM_SUBMENU | MIIM_ID;
		int 	count = ::GetMenuItemCount(hParentMenu);
		for (int i = 0; i < count; i++) {
			::GetMenuItemInfo(hParentMenu, i, TRUE, &mii);
			if (mii.wID == nID)
				return hParentMenu;
			if (mii.hSubMenu) {
				hRet = _SearchSubMenu(nID, mii.hSubMenu);
				if (hRet)		return hRet;
			}
		}
		return NULL;
	}


	BOOL CreateMenu()
	{
		CComPtr<IUnknown>		pUnk;
		CComPtr<IWebBrowser2>	pWB2;
		CComPtr<IHTMLDocument2> pDoc;
		HRESULT 				hr;

		//IHTMLDocument2を取得する
		CString 	str;
		::GetClassName(m_hWndMDIClient, str.GetBuffer(1024), 1024);
		str.ReleaseBuffer();
		ATLTRACE("classname is %s", str);

		HWND	hWndActive = DonutGetActiveWindow(m_hWnd);
		if (!hWndActive) return FALSE;

		pWB2 = DonutGetIWebBrowser2(hWndActive);
		if (!pWB2)		return FALSE;

		pWB2->get_Document( (IDispatch **) &pDoc );
		if (!pDoc)		return FALSE;

		//defined on AtlhostEx.h
		enum {
			IDR_BROWSE_CONTEXT_MENU 	= 24641,
			SHDVID_GETMIMECSETMENU		=	 27,
		};

		//ICommandTargetとIOleWindowを取得。ウィンドウハンドルは保管
		hr			 = pDoc->QueryInterface(IID_IOleCommandTarget, (void **) &spCT);

		hr			 = pDoc->QueryInterface(IID_IOleWindow, (void **) &spWnd);
		if ( (!spCT) || (!spWnd) )	return FALSE;

		hr			 = spWnd->GetWindow(&m_hWndTarget);
		if (!m_hWndTarget)	return FALSE;

		//DLLからメニューをロードする
		m_hInstDLL	 = LoadLibrary( TEXT("SHDOCLC.DLL") );
		if (!m_hInstDLL) {
			m_hInstDLL = LoadLibrary( TEXT("ieframe.dll") );	// for vista
			if (!m_hInstDLL)
				return FALSE;
		}

		m_CtxMenu	 = LoadMenu( m_hInstDLL, MAKEINTRESOURCE(IDR_BROWSE_CONTEXT_MENU) );
		m_CtxMenuSub = GetSubMenu(m_CtxMenu.m_hMenu, CONTEXT_MENU_DEFAULT);

		//エンコードメニューを取得する
		//CComVariant 	var;
		VARIANT	var = { 0 };
		hr			 = spCT->Exec(&CGID_ShellDocView, SHDVID_GETMIMECSETMENU, 0, NULL, &var);

		MENUITEMINFO	mii   = { 0 };
		mii.cbSize	 = sizeof (mii);
		mii.fMask	 = MIIM_SUBMENU;
		mii.hSubMenu = (HMENU) var.byref;
		SetMenuItemInfo(m_CtxMenuSub.m_hMenu, IDM_LANGUAGE, FALSE, &mii);

		int 	count = ::GetMenuItemCount(mii.hSubMenu);
		ATLTRACE(_T("mii.hSubMenu Count = %d\n"), count);
		for (int s = 0; s < count; s++) {
			TCHAR	buf[300];
			buf[0]	= 0;		//+++
			::GetMenuString(mii.hSubMenu, s, buf, 300, MF_BYPOSITION);
			ATLTRACE(_T("hMenuSub %d text = %s\n"), s, buf);
		}

		//m_CtxMenuSubにおけるエンコードメニューの先頭項目のIDを保管
		MENUITEMINFO	mii2  = { sizeof (MENUITEMINFO) };
		count		 = ::GetMenuItemCount(m_CtxMenuSub.m_hMenu);
		m_posStart	 = 0;
		mii2.fMask	 = MIIM_SUBMENU | MIIM_ID;

		for (int i = 0; i < count; i++) {
			::GetMenuItemInfo(m_CtxMenuSub.m_hMenu, i, TRUE, &mii2);
			if (mii2.hSubMenu == mii.hSubMenu)
				m_posStart = mii2.wID;
		}

		//エンコードメニューをコピー
		int 					id	  = ID_START;
		m_EncodeMenu.CreatePopupMenu();
		CopySubMenu(m_EncodeMenu.m_hMenu, mii.hSubMenu, id);

		return TRUE;
	}


	HMENU CopySubMenu(HMENU hCopy , HMENU hOriginal, int &id)
	{
		CMenuHandle  Original = hOriginal;
		CMenuHandle  Copy	  = hCopy;
		int 		 count	  = Original.GetMenuItemCount();
		MENUITEMINFO mii;
		CString 	 strBuf;

		mii.cbSize = sizeof (MENUITEMINFO);
		mii.fMask  = MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE ;

		for (int i = 0; i < count; i++) {
			mii.dwTypeData = strBuf.GetBuffer(1024);
			mii.cch 	   = 1024;
			Original.GetMenuItemInfo(i, TRUE, &mii);
			mii.dwItemData = mii.wID;	//このメニューをクリックすると
			mii.wID 	   = id++;		///コマンドハンドラが呼び出されるようにする
			strBuf.ReleaseBuffer();

			if (mii.hSubMenu == NULL) {
				Copy.InsertMenuItem(i, TRUE, &mii);
			} else {
				CMenuHandle Parent;
				Parent.CreatePopupMenu();
				Parent		 = CopySubMenu(Parent.m_hMenu, mii.hSubMenu, id);
				mii.fMask	 = MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE ;
				mii.hSubMenu = Parent.m_hMenu;
				Copy.InsertMenuItem(i, TRUE, &mii);
			}
		}

		return Copy.m_hMenu;
	}
};


#endif		//__MENUENCODE_H_
