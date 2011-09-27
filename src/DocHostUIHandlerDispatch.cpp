// DocHostUIHandlerDispatch.cpp

#include "stdafx.h"
#include "DocHostUIHandlerDispatch.h"
#include "DonutView.h"
#include "MainFrame.h"
#include "option/MainOption.h"


// Constructor
CDocHostUIHandlerDispatch::CDocHostUIHandlerDispatch(CDonutView* pView)
	: m_bNoIECustom(false)
	, m_pView(pView)
	, m_nContextMenuMode(0)
{ }


STDMETHODIMP CDocHostUIHandlerDispatch::ShowContextMenu(
	/* [in] */ DWORD				dwID,
	/* [in] */ DWORD				x,
	/* [in] */ DWORD				y,
	/* [in] */ IUnknown*			pcmdtReserved,
	/* [in] */ IDispatch*			pdispReserved,
	/* [retval][out] */ HRESULT*	dwRetVal)
{
	if ( m_bNoIECustom && (GetKeyState(VK_LBUTTON) >= 0) )	//カスタム&左クリックされているかどうか
		return S_FALSE;

	POINT	pt = { (int)x, (int)y };
	*dwRetVal = _ShowCustomContextMenu(dwID, &pt, pcmdtReserved, pdispReserved);
	return S_OK;
}

STDMETHODIMP CDocHostUIHandlerDispatch::GetHostInfo(
	/* [out][in] */ DWORD* pdwFlags,
	/* [out][in] */ DWORD* pdwDoubleClick)
{
	//一時的に外部IDispatchを無効にしデフォルト動作させる
	CComQIPtr<IAxWinHostWindow> pHostWindow = m_pDefaultHandler;
	pHostWindow->SetExternalUIHandler(NULL);

	DOCHOSTUIINFO	info= { sizeof (DOCHOSTUIINFO) };
	info.dwFlags		= *pdwFlags;
	info.dwDoubleClick	= *pdwDoubleClick;
	HRESULT 		hr	= m_pDefaultHandler->GetHostInfo(&info);	//デフォルト動作

	//外部IDispatchを有効にする
	pHostWindow->SetExternalUIHandler(this);

	*pdwFlags		   = info.dwFlags;
	*pdwDoubleClick    = info.dwDoubleClick;
	return hr;
}

STDMETHODIMP CDocHostUIHandlerDispatch::GetDropTarget(/* [in] */ IUnknown* pDropTarget, /* [out] */ IUnknown** ppDropTarget)
{
	return m_pView->QueryInterface(IID_IUnknown, (void**)ppDropTarget);;
}

STDMETHODIMP CDocHostUIHandlerDispatch::TranslateAccelerator(
	/* [in] */ DWORD_PTR		hWnd,
	/* [in] */ DWORD			nMessage,
	/* [in] */ DWORD_PTR		wParam,
	/* [in] */ DWORD_PTR		lParam,
	/* [in] */ BSTR 			bstrGuidCmdGroup,
	/* [in] */ DWORD			nCmdID,
	/* [retval][out] */HRESULT* dwRetVal)
{
	if (CMainOption::s_bUseCustomFindBar && ::GetKeyState(VK_CONTROL) < 0 && nMessage != WM_CHAR) {
		if (wParam == 0x46) {	//F
			m_pView->GetParent().SendMessage(WM_COMMAND, ID_EDIT_FIND);
			*dwRetVal = S_OK;
			return S_OK;
		}
	}

	return S_FALSE; //IE default action
}

/// カスタムコンテキストメニューを表示する
HRESULT	CDocHostUIHandlerDispatch::_ShowCustomContextMenu(DWORD dwID, POINT* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit)
{
	CComQIPtr<IOleWindow>	pWindow = pCommandTarget;
	ATLASSERT(pWindow);
	HWND hWndTarget = NULL;
	HRESULT hr = pWindow->GetWindow(&hWndTarget);
	ATLASSERT(hWndTarget);

	HWND hWndTopLevel = CWindow(hWndTarget).GetTopLevelWindow();

	m_nContextMenuMode	= 0;	//+++
	// まずメニュー用のリソースが使用されていればそれを開放する
	::SendMessage(hWndTopLevel, WM_RELEASE_CONTEXTMENU, 0, 0);

	if (::GetKeyState(VK_LBUTTON) < 0) 
		dwID = CONTEXT_MENU_HOLDLEFTBUTTON;

	if (   dwID != CONTEXT_MENU_DEFAULT
		&& dwID != CONTEXT_MENU_IMAGE
		&& dwID != CONTEXT_MENU_TEXTSELECT
		&& dwID != CONTEXT_MENU_ANCHOR
		&& dwID != CONTEXT_MENU_HOLDLEFTBUTTON)
	{
		return S_FALSE;
	}


	m_nContextMenuMode	= dwID;	//+++
	//if (pptPosition) 					//+++
	//	m_pt	= *pptPosition;			//+++
	
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
		//m_strUrl = Donut_GetActiveStatusStr();
	}
	ANCHORFINISH:


	enum {
		IDR_BROWSE_CONTEXT_MENU 	= 24641,
		IDR_FORM_CONTEXT_MENU		= 24640,
		SHDVID_GETMIMECSETMENU		=	 27,
		SHDVID_ADDMENUEXTENSIONS	=	 53,
	};

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
	CCustomContextMenuOption::AddSubMenu(menu, hWndTopLevel, arrDestroyMenu);
	{
		// 初期化前メッセージを送る
	//	for (int ii = 0; ii < mapCmd.GetSize(); ii++) {
	//		_BeforeInitSpecialMenu( mapCmd.GetValueAt( ii ) );
	//	}

		// Show shortcut menu
		int 	nCmdAddFav	= MtlGetCmdIDFromAccessKey( menu, _T("&F") );

		DWORD	dwMenuStyle = TPM_LEFTALIGN | TPM_RETURNCMD;
		if (CMenuOption::s_bR_Equal_L) 
			dwMenuStyle |= TPM_RIGHTBUTTON;

		{	// メニューの右にショートカットキーを表示しないようにする(メインフレームで設定されるのを防ぐ)
			MENUITEMINFO menuInfo = { sizeof (MENUITEMINFO) };
			menuInfo.fMask	= MIIM_TYPE;
			menuInfo.fType	= MF_SEPARATOR;
			menu.InsertMenuItem(0, MF_BYPOSITION, &menuInfo);
		}

		/* メニューを表示 */
		int iSelection = menu.TrackPopupMenu(dwMenuStyle, pptPosition->x, pptPosition->y, m_pView->m_hWnd);//hWndTarget);//hWndTopLevel);

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
				lRes = ::SendMessage(hWndTarget, WM_COMMAND, iSelection, NULL);
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
				lRes = ::SendMessage(hWndTopLevel, WM_COMMAND, iSelection, NULL);

				//この時点でウィンドウが消されてしまう可能性があるけれども・・
				if (iSelection == nCmdAddFav) {
					::SendMessage(hWndTopLevel, WM_REFRESH_EXPBAR, 0, 0);
				}
			}
		}
	}
	// ここでお片づけ
	CCustomContextMenuOption::RemoveSubMenu(menu, arrDestroyMenu);


//	for (int ii = 0; ii < mapCmd.GetSize(); ii++) {
//		::RemoveMenu( hMenuSub, mapCmd.GetKeyAt( ii ), MF_BYCOMMAND );
//		_RestoreSpecialMenu( mapCmd.GetValueAt( ii ) );
//	}

	//m_nContextMenuMode = 0;	//+++

	return S_OK;
}

/// メニューの有効・無効をspOleCommandTargetから読み込む
void	CDocHostUIHandlerDispatch::_SetMenuEnable(HMENU hMenu, IOleCommandTarget* spOleCommandTarget)
{
	int nCount = GetMenuItemCount(hMenu);
	for (int i = 0; i < nCount; ++i) {
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

