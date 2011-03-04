/**
 *	@file	ChevronHandler.h
 *	@brief	シャブロン・ハンドラ
 *	@note
 *		+++メモ： シャブロン=メニューバーで隠れたモノがあることを示す＆メニュー化してる">>"の部分.
 */
#pragma once

#include "MtlBase.h"


inline void 	MtlEatNextLButtonDownOnChevron(HWND hWndTopLevel, const CRect &rcChevron)
{
	MSG 	msg;

	if ( ::PeekMessage(&msg, hWndTopLevel, NULL, NULL, PM_NOREMOVE) ) {
		if ( msg.message == WM_LBUTTONDOWN && rcChevron.PtInRect(msg.pt) )
			::PeekMessage(&msg, hWndTopLevel, WM_LBUTTONDOWN, WM_LBUTTONDOWN, PM_REMOVE);
		else if ( msg.message == WM_LBUTTONDBLCLK && rcChevron.PtInRect(msg.pt) )
			::PeekMessage(&msg, hWndTopLevel, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK, PM_REMOVE);
	}
}



template <class T>
class CChevronHandler {
private:
	struct _ChevronMenuInfo {
		HMENU				hMenu;
		LPNMREBARCHEVRON	lpnm;
		bool				bCmdBar;
	};

	// Data members
	CSimpleArray<HMENU> 	m_arrPopupMenu;

public:
	// Methods
	bool PushChevron(LPNMHDR pnmh, HWND hWndMenuOwner)
	{
		_ChevronMenuInfo	cmi = { NULL, (LPNMREBARCHEVRON) pnmh, false };

		if ( !_PrepareChevronMenu(cmi) )
			return false;

		// display a popup menu with hidden items
		_DisplayChevronMenu(cmi, hWndMenuOwner);
		// cleanup
		_CleanupChevronMenu(cmi);

		return true;
	}

	// Overridables
	HMENU ChevronHandler_OnGetChevronMenu(int nCmdID, HMENU &hMenuDestroy) { return NULL; }


public:
	BEGIN_MSG_MAP(CChevronHandler<T>)
		NOTIFY_CODE_HANDLER(RBN_CHEVRONPUSHED, OnChevronPushed)
	END_MSG_MAP()


private:
	void Chevronhandler_OnCleanupChevronMenu() { }

	LRESULT OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
	{
		T *pT = static_cast<T *>(this);
		if ( !PushChevron(pnmh, pT->m_hWnd) ) {
			bHandled = FALSE;
			return 1;
		}
		return 0;
	}


	bool _PrepareChevronMenu(_ChevronMenuInfo &cmi)
	{
		// get rebar and toolbar
		REBARBANDINFO	rbbi;
		rbbi.cbSize = sizeof (REBARBANDINFO);
		rbbi.fMask	= RBBIM_CHILD;
		BOOL		  bRet		 = ::SendMessage(cmi.lpnm->hdr.hwndFrom, RB_GETBANDINFO, cmi.lpnm->uBand, (LPARAM) &rbbi) != 0;
		ATLASSERT(bRet);

		// assume the band is a toolbar
		CWindow 	  wnd		 = rbbi.hwndChild;
		int 		  nCount	 = (int) wnd.SendMessage(TB_BUTTONCOUNT);

		if (nCount <= 0)  // probably not a toolbar
			return false;

		// check if it's a command bar
		CMenuHandle   menuCmdBar = (HMENU) wnd.SendMessage(CBRM_GETMENU);
		cmi.bCmdBar = (menuCmdBar.m_hMenu != NULL);

		// build a menu from hidden items
		CMenuHandle   menu;
		bRet		= menu.CreatePopupMenu();
		ATLASSERT(bRet);
		RECT		  rcClient;
		bRet		= wnd.GetClientRect(&rcClient);
		ATLASSERT(bRet);

		for (int i = 0; i < nCount; i++) {
			TBBUTTON tbb;
			bRet = wnd.SendMessage(TB_GETBUTTON, i, (LPARAM) &tbb) != 0;
			ATLASSERT(bRet);

			RECT	 rcButton;
			bRet = wnd.SendMessage(TB_GETITEMRECT, i, (LPARAM) &rcButton) != 0;
			ATLASSERT(bRet);
			bool	 bEnabled = ( (tbb.fsState & TBSTATE_ENABLED) != 0 );

			if (rcButton.right > rcClient.right) {
				if (tbb.fsStyle & BTNS_SEP) {
					if (menu.GetMenuItemCount() > 0)
						menu.AppendMenu(MF_SEPARATOR);
				} else if (cmi.bCmdBar) {
					TCHAR		  szBuff[100];
					szBuff[0]	   = 0;
					CMenuItemInfo mii;
					mii.fMask	   = MIIM_TYPE | MIIM_SUBMENU;
					mii.dwTypeData = szBuff;
					mii.cch 	   = sizeof (szBuff) / sizeof (TCHAR);
					bRet		   = menuCmdBar.GetMenuItemInfo(i, TRUE, &mii);
					ATLASSERT(bRet);
					// Note: CmdBar currently supports only drop-down items
					ATLASSERT( ::IsMenu(mii.hSubMenu) );
					bRet		   = menu.AppendMenu(
											MF_STRING | MF_POPUP | (bEnabled ? MF_ENABLED : MF_GRAYED),
											(UINT_PTR) mii.hSubMenu,
											mii.dwTypeData );
					ATLASSERT(bRet);
				} else {
					// get button's text
					TCHAR  szBuff[100];
					szBuff[0]	   = 0;
					LPTSTR lpstrText = szBuff;

					if (wnd.SendMessage(TB_GETBUTTONTEXT, tbb.idCommand, (LPARAM) szBuff) == -1) {
						// no text for this button, try a resource string
						lpstrText = _T("?");
						::LoadString( _Module.GetResourceInstance(), tbb.idCommand, szBuff, sizeof (szBuff) / sizeof (TCHAR) );

						int  len   = lstrlen(szBuff);
						for (int n = 0; n < len; n++) {
							if ( szBuff[n] == _T('\n') ) {
								lpstrText = &szBuff[n + 1];
								break;
							}
						}
					}

					_InsertMenuItem(wnd.m_hWnd, menu, tbb.idCommand, lpstrText, bEnabled);
					//	bRet = menu.AppendMenu(MF_STRING | (bEnabled ? MF_ENABLED : MF_GRAYED), tbb.idCommand, lpstrText);
					ATLASSERT(bRet);
				}
			}
		}

		if (menu.GetMenuItemCount() == 0) {  // no hidden buttons after all
			menu.DestroyMenu();
			::MessageBeep( (UINT) -1 );
			return false;
		}

		cmi.hMenu = menu;
		return true;
	}


	void _DisplayChevronMenu(_ChevronMenuInfo &cmi, HWND hWndMenuOwner)
	{
		T * 	  pT		 = static_cast<T *>(this);

		// convert chevron rect to screen coordinates
		CWindow   wndFrom	 = cmi.lpnm->hdr.hwndFrom;
		RECT	  rc		 = cmi.lpnm->rc;

		wndFrom.ClientToScreen(&rc);
		// set up flags and rect
		UINT	  uMenuFlags = TPM_LEFTBUTTON | TPM_VERTICAL | TPM_LEFTALIGN | TPM_TOPALIGN
									| (!AtlIsOldWindows() ? TPM_VERPOSANIMATION : 0);
		TPMPARAMS TPMParams;
		TPMParams.cbSize	 = sizeof (TPMPARAMS);
		TPMParams.rcExclude  = rc;
		// check if this window has a command bar
		HWND	  hWndCmdBar = (HWND) ::SendMessage(pT->m_hWnd, CBRM_GETCMDBAR, 0, 0L);

		if ( ::IsWindow(hWndCmdBar) ) {
			CBRPOPUPMENU CBRPopupMenu = { sizeof (CBRPOPUPMENU), cmi.hMenu, uMenuFlags, rc.left, rc.bottom, &TPMParams };
			::SendMessage(hWndCmdBar, CBRM_TRACKPOPUPMENU, 0, (LPARAM) &CBRPopupMenu);
		} else {
			::TrackPopupMenuEx(cmi.hMenu, uMenuFlags, rc.left, rc.bottom, hWndMenuOwner, &TPMParams);
		}
	}


	void _CleanupChevronMenu(_ChevronMenuInfo &cmi)
	{
		T * 		pT		= static_cast<T *>(this);

		CMenuHandle menu	= cmi.hMenu;

		// if menu is from a command bar, detach submenus so they are not destroyed
		for (int i = menu.GetMenuItemCount() - 1; i >= 0; i--)
			menu.RemoveMenu(i, MF_BYPOSITION);

		pT->Chevronhandler_OnCleanupChevronMenu();

		// clean popups if need
		for (int j = 0; j < m_arrPopupMenu.GetSize(); ++j)
			::DestroyMenu(m_arrPopupMenu[j]);

		m_arrPopupMenu.RemoveAll();

		// destroy menu
		menu.DestroyMenu();
		// convert chevron rect to screen coordinates
		CWindow 	wndFrom = cmi.lpnm->hdr.hwndFrom;
		RECT		rc		= cmi.lpnm->rc;
		wndFrom.ClientToScreen(&rc);
		// eat next message if click is on the same button
		MtlEatNextLButtonDownOnChevron(pT->GetTopLevelParent(), rc);
	}


	void _InsertMenuItem(CToolBarCtrl toolbar, CMenuHandle menu, int nIDCmd, const CString &strText, bool bEnabled)
	{
		CVersional<MENUITEMINFO>	mii;
		mii.fMask	   = MIIM_ID | MIIM_TYPE | MIIM_STATE;
		mii.wID 	   = nIDCmd;
		mii.fType	   = MFT_STRING;
		mii.dwTypeData = (LPTSTR) (LPCTSTR) strText;
		mii.fState	   = bEnabled ? MFS_ENABLED : MFS_GRAYED;

		CVersional<TBBUTTONINFO> tbi;
		tbi.dwMask	   = TBIF_STYLE;

		if (toolbar.GetButtonInfo(nIDCmd, &tbi) == -1)
			return;

		if (tbi.fsStyle & BTNS_DROPDOWN || tbi.fsStyle & BTNS_WHOLEDROPDOWN) {
			T * 		pT			 = static_cast<T *>(this);
			HMENU		hMenuDestroy = NULL;
			CMenuHandle menu		 = pT->ChevronHandler_OnGetChevronMenu(nIDCmd, hMenuDestroy);
			if (menu.m_hMenu) {
				mii.fMask	|= MIIM_SUBMENU;
				mii.hSubMenu = menu.m_hMenu;
				if (hMenuDestroy)
					m_arrPopupMenu.Add(hMenuDestroy);
			}
		}

		menu.InsertMenuItem(-1, TRUE, &mii);
	}

};
