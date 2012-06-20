/**
 *	@file	MtlUser.h
 */
// MTL Version 0.03
// Copyright (C) 2000 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// Last updated: August 11, 2000

#ifndef __MTLUSER_H__
#define __MTLUSER_H__

#pragma once

#include "MtlMisc.h"
#include "MtlWin.h"

#include <functional>
#include <zmouse.h>

#include "dialog/aboutdlg.h"


namespace MTL {

using WTL::CReBarCtrl;


#if 1
// In Win32 the WM_ messages have changed.	They have
// to be translated from the 32-bit values to 16-bit
// values here.
//#define WIN16_LB_ADDSTRING		0x0401
//#define WIN16_CB_ADDSTRING		0x0403
//#define AFX_CB_ADDSTRING			0x1234
enum {
	WIN16_LB_ADDSTRING		= 0x0401,
	WIN16_CB_ADDSTRING		= 0x0403,
	AFX_CB_ADDSTRING		= 0x1234,
};
#endif




////////////////////////////////////////////////////////////////////////////
// for ReBar Bands

inline BOOL MtlToggleBandVisible(CReBarCtrl rebar, UINT nID)
{
	ATLASSERT( rebar.IsWindow() );

	int 	nIndex = rebar.IdToIndex(nID);
	ATLASSERT(nIndex != -1);

	CVersional<REBARBANDINFO>	rbBand;
	rbBand.fMask = RBBIM_STYLE;
	MTLVERIFY( rebar.GetBandInfo(nIndex, &rbBand) );

	bool	bNew	 = (rbBand.fStyle & RBBS_HIDDEN) != 0;	//+++ ? TRUE : FALSE;
	MTLVERIFY( rebar.ShowBand(nIndex, bNew) );
	return bNew;
}


inline BOOL MtlShowBand(CReBarCtrl rebar, UINT nID, BOOL bVisible = TRUE)
{
	ATLASSERT( rebar.IsWindow() );

	int 	nIndex	  = rebar.IdToIndex(nID);
	ATLASSERT(nIndex != -1);

	CVersional<REBARBANDINFO>	rbBand;
	rbBand.fMask = RBBIM_STYLE;
	MTLVERIFY( rebar.GetBandInfo(nIndex, &rbBand) );

	BOOL	bOldVisible = (rbBand.fStyle & RBBS_HIDDEN) == 0;	//+++ ? FALSE : TRUE;
	if (bOldVisible != bVisible)
		MTLVERIFY( rebar.ShowBand(nIndex, bVisible) );

	return TRUE;
}



inline BOOL MtlIsBandVisible(CReBarCtrl rebar, UINT nID)
{
	ATLASSERT( rebar.IsWindow() );

	int 	nIndex = rebar.IdToIndex(nID);
	ATLASSERT(nIndex != -1);

	CVersional<REBARBANDINFO>	rbBand;
	rbBand.fMask = RBBIM_STYLE;
	MTLVERIFY( rebar.GetBandInfo(nIndex, &rbBand) );

	//x return (rbBand.fStyle & RBBS_HIDDEN) ? FALSE : TRUE;
	return (rbBand.fStyle & RBBS_HIDDEN) == 0;
}



#define REFLECT_CHEVRONPUSHED_NOTIFICATION()										 \
	{																				 \
		if (uMsg == WM_NOTIFY && RBN_CHEVRONPUSHED == ( (LPNMHDR) lParam )->code) {  \
			bHandled = TRUE;														 \
			LPNMREBARCHEVRON lpnm	   = (LPNMREBARCHEVRON) lParam; 				 \
			HWND			 hWndReBar = lpnm->hdr.hwndFrom;						 \
			HWND			 hWndBand  = MtlGetReBarBandHwnd(hWndReBar, lpnm->wID);  \
			if (hWndBand != NULL)													 \
				lResult = ::SendMessage(hWndBand, OCM__BASE + uMsg, wParam, lParam); \
			else																	 \
				bHandled = FALSE;													 \
			if (bHandled)															 \
				return TRUE;														 \
		}																			 \
	}



inline HWND MtlGetReBarBandHwnd(CReBarCtrl rebar, UINT nID)
{
	ATLASSERT( rebar.IsWindow() );

	int 					  nIndex = rebar.IdToIndex(nID);
	ATLASSERT(nIndex != -1);

	CVersional<REBARBANDINFO> rbBand;
	rbBand.fMask = RBBIM_CHILD;
	MTLVERIFY( rebar.GetBandInfo(nIndex, &rbBand) );
	ATLASSERT( ::IsWindow(rbBand.hwndChild) );
	return rbBand.hwndChild;
}



////////////////////////////////////////////////////////////////////////////
// UIEnable and UISetCheck, UISetRadio

class UIEnableFunction : public std::unary_function<int, BOOL> {
public:
	explicit UIEnableFunction(CUpdateUIBase *pUpdateUI, BOOL bEnable)
		: _pUpdateUI(pUpdateUI)
		, _bEnable(bEnable)
	{
	}

	result_type operator ()(argument_type arg)
	{
		return _pUpdateUI->UIEnable(arg, _bEnable);
	}

private:
	CUpdateUIBase *_pUpdateUI;
	BOOL		   _bEnable;
};



class UISetCheckFunction : public std::unary_function<int, BOOL> {
public:
	explicit UISetCheckFunction(CUpdateUIBase *pUpdateUI, int nCheck)
		: _pUpdateUI(pUpdateUI)
		, _nCheck(nCheck)
	{
	}

	result_type operator ()(argument_type arg)
	{
		return _pUpdateUI->UISetCheck(arg, _nCheck);
	}

private:
	CUpdateUIBase *_pUpdateUI;
	int 		   _nCheck;
};



class UISetRadioFunction : public std::unary_function<int, BOOL> {
public:
	explicit UISetRadioFunction(CUpdateUIBase *pUpdateUI, BOOL bRadio)
		: _pUpdateUI(pUpdateUI)
		, _bRadio(bRadio)
	{
	}

	result_type operator ()(argument_type arg)
	{
		return _pUpdateUI->UISetRadio(arg, _bRadio);
	}


private:
	CUpdateUIBase *_pUpdateUI;
	BOOL		   _bRadio;
};



////////////////////////////////////////////////////////////////////////////
// for standard application commands

template <class T>
class CAppCommandHandler {
public:
	BEGIN_MSG_MAP(CAppCommandHandler)
		COMMAND_ID_HANDLER(ID_APP_EXIT , OnAppExit )
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_APP_HELP , OnAppHelp )	   // UDT DGSTR
	END_MSG_MAP()


	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		//x 	T* pT = static_cast<T*>(this);
		CAboutDlg dlg;

		dlg.DoModal();
		return 0;
	}


private:
	LRESULT OnAppExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		T *pT = static_cast<T *>(this);

		ATLASSERT( ::IsWindow(pT->m_hWnd) );
		pT->PostMessage(WM_CLOSE);
		return 0;
	}


	// UDT DGSTR ( ShowHelp )
	LRESULT OnAppHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		CString strPath = MtlGetHTMLHelpPath();

		// For compacting size , dont use htmlHelp api.
		ShellExecute(NULL , NULL , strPath , NULL , NULL , SW_SHOW );
		return 0;
	}
	// ENDE
};



////////////////////////////////////////////////////////////////////////////
// for Window Menu command

struct _Mtl_Function_CloseAll {
	void operator ()(HWND hWnd)
	{
		::SendMessage(hWnd, WM_CLOSE, 0, 0);
	}
};



struct _Mtl_Function_CloseAllExcept {
	HWND _hWndExcept;
	_Mtl_Function_CloseAllExcept(HWND hWndExcept) : _hWndExcept(hWndExcept) { }
	void operator ()(HWND hWnd)
	{
		if (hWnd != _hWndExcept) {
			::SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
};



struct _Mtl_Function_CloseAllExcept2 {
private:
	HWND	_hWndExcept;
	HWND	_hWndExcept2;

public:
	_Mtl_Function_CloseAllExcept2(HWND hWndExcept, HWND hWndExcept2)
		:	_hWndExcept(hWndExcept) , _hWndExcept2(hWndExcept2)
	{
	}

	void operator ()(HWND hWnd)
	{
		if (hWnd != _hWndExcept && hWnd != _hWndExcept2) {
			// BOOL bSelected = (BOOL)::SendMessage(hWnd, WM_USER_IS_SELECTED_TAB, (WPARAM)hWnd, 0);
			::SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
};



//vvvvvvvvvvvvvvvvvvvvvvv
// U.H
struct _Mtl_Function_RefreshAllExcept {
private:
	HWND	_hWndExcept;

public:
	_Mtl_Function_RefreshAllExcept(HWND hWndExcept) : _hWndExcept(hWndExcept) { }

	void operator ()(HWND hWnd)
	{
		if (hWnd != _hWndExcept)
			::SendMessage(hWnd, WM_COMMAND, ID_VIEW_REFRESH, 0);
	}
};



struct _Mtl_Function_RefreshAllExcept2 {
private:
	HWND	_hWndExcept;
	HWND	_hWndExcept2;

public:
	_Mtl_Function_RefreshAllExcept2(HWND hWndExcept, HWND hWndExcept2)
		: _hWndExcept(hWndExcept)
		, _hWndExcept2(hWndExcept2)
	{
	}

	void operator ()(HWND hWnd)
	{
		if (hWnd != _hWndExcept && hWnd != _hWndExcept2)
			::SendMessage(hWnd, WM_COMMAND, ID_VIEW_REFRESH, 0);
	}
};
//^^^^^^^^^^^^^^^^^^^^^^^



inline void MtlCloseAllMDIChildren(HWND hWndMDIClient)
{
	HWND hWndActive = (HWND) ::SendMessage(hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM) NULL);

	MtlForEachMDIChild( hWndMDIClient, _Mtl_Function_CloseAllExcept(hWndActive) );
	::SendMessage(hWndActive, WM_CLOSE, 0, 0);
}



inline void MtlCloseAllMDIChildrenExcept(HWND hWndMDIClient, HWND hWndExcept)
{
	HWND hWndActive = (HWND) ::SendMessage(hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM) NULL);
	if (hWndActive == hWndExcept) {
		MtlForEachMDIChild( hWndMDIClient, _Mtl_Function_CloseAllExcept(hWndExcept) );
	} else {
		MtlForEachMDIChild( hWndMDIClient, _Mtl_Function_CloseAllExcept2(hWndActive, hWndExcept) );
		::SendMessage(hWndActive, WM_CLOSE, 0, 0);
	}
}



//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// U.H
inline void MtlRefreshAllMDIChildrenExcept(HWND hWndMDIClient, HWND hWndExcept)
{
	HWND hWndActive = (HWND) ::SendMessage(hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM) NULL);

	if (hWndActive == hWndExcept) {
		MtlForEachMDIChild( hWndMDIClient, _Mtl_Function_RefreshAllExcept(hWndExcept) );
	} else {
		MtlForEachMDIChild( hWndMDIClient, _Mtl_Function_RefreshAllExcept2(hWndActive, hWndExcept) );
		::SendMessage(hWndActive, WM_COMMAND, ID_VIEW_REFRESH, 0);
	}
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^



inline void MtlCompactMDIWindowMenuDocumentList(CMenuHandle menuWindow, int nWindowMenuItemCount, int nMaxTextLength)
{
	int    nLen 	= nMaxTextLength + 5;		// add a little
	LPTSTR lpszText = (LPTSTR) _alloca( (nLen + 1) * sizeof (TCHAR) );
	int    i		= nWindowMenuItemCount + 1; // add a separator

	for (; i < menuWindow.GetMenuItemCount(); ++i) {
		CMenuItemInfo mii;
		mii.fMask	   = MIIM_TYPE;
		mii.cch 	   = nLen;
		mii.dwTypeData = lpszText;
		::GetMenuItemInfo(menuWindow, i, TRUE, &mii);

		CString 	  strMenuItem(mii.dwTypeData);
		int 		  nHeaderLen = strMenuItem.Left(1) == _T("&") ? 3 : 0;
		CString 	  strHeader  = strMenuItem.Left(nHeaderLen);
		strMenuItem    = MtlCompactString(strMenuItem.Right(strMenuItem.GetLength() - nHeaderLen), nMaxTextLength);
		strMenuItem    = strHeader + strMenuItem;
		mii.dwTypeData = (LPTSTR) (LPCTSTR) strMenuItem;
		::SetMenuItemInfo(menuWindow, i, TRUE, &mii);
	}
}



#define WM_USER_SIZE_CHG_EX 	(WM_USER + 44)


template <class T>
class CWindowMenuCommandHandler {
public:
	// Message map and handlers
	BEGIN_MSG_MAP(CWindowMenuCommandHandler)
		COMMAND_ID_HANDLER( ID_WINDOW_CASCADE  , OnWindowCascade	)
		COMMAND_ID_HANDLER( ID_WINDOW_TILE_HORZ, OnWindowTileHorz	)
		COMMAND_ID_HANDLER( ID_WINDOW_TILE_VERT, OnWindowTileVert	)
		COMMAND_ID_HANDLER( ID_WINDOW_ARRANGE  , OnWindowArrangeIcons)
		COMMAND_ID_HANDLER( ID_WINDOW_CLOSE_ALL, OnWindowCloseAll	)
		//COMMAND_ID_HANDLER(ID_WINDOW_CLOSE_EXCEPT, OnWindowCloseExcept)

		COMMAND_ID_HANDLER( ID_WINDOW_RESTORE  , OnWindowRestore	)
		COMMAND_ID_HANDLER( ID_WINDOW_MOVE	   , OnWindowMove		)
		COMMAND_ID_HANDLER( ID_WINDOW_SIZE	   , OnWindowSize		)
		COMMAND_ID_HANDLER( ID_WINDOW_MINIMIZE , OnWindowMinimize	)
		COMMAND_ID_HANDLER( ID_WINDOW_MAXIMIZE , OnWindowMaximize	)
	END_MSG_MAP()


private:
	LRESULT OnWindowCascade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		T * 	pT = static_cast<T *>(this);
		ATLASSERT( ::IsWindow(pT->m_hWnd) );
		::SendMessage(pT->MDIGetActive(), WM_USER_SIZE_CHG_EX, 0, 0);
		pT->MDICascade();
		return 0;
	}


	LRESULT OnWindowTileHorz(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		T * 	pT = static_cast<T *>(this);
		ATLASSERT( ::IsWindow(pT->m_hWnd) );
		::SendMessage(pT->MDIGetActive(), WM_USER_SIZE_CHG_EX, 0, 0);
		pT->MDITile();
		return 0;
	}


	LRESULT OnWindowTileVert(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		T * 	pT = static_cast<T *>(this);
		ATLASSERT( ::IsWindow(pT->m_hWnd) );
		::SendMessage(pT->MDIGetActive(), WM_USER_SIZE_CHG_EX, 0, 0);
		pT->MDITile(MDITILE_VERTICAL);
		return 0;
	}


	LRESULT OnWindowArrangeIcons(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		T * 	pT = static_cast<T *>(this);
		ATLASSERT( ::IsWindow(pT->m_hWnd) );
		pT->MDIIconArrange();
		return 0;
	}


	LRESULT OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		T * 	pT = static_cast<T *>(this);
		ATLASSERT( ::IsWindow(pT->m_hWnd) );
		ATLASSERT( ::IsWindow(pT->m_hWndMDIClient) );

		CWaitCursor 			cur;
		CLockRedrawMDIClient	lock(pT->m_hWndMDIClient);
		MtlCloseAllMDIChildren(pT->m_hWndMDIClient);
		return 0;
	}


	LRESULT OnWindowCloseExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		T * 	pT = static_cast<T *>(this);
		ATLASSERT( ::IsWindow(pT->m_hWnd) );
		ATLASSERT( ::IsWindow(pT->m_hWndMDIClient) );

		CWaitCursor 			cur;
		CLockRedrawMDIClient	lock(pT->m_hWndMDIClient);
		MtlCloseAllMDIChildrenExcept( pT->m_hWndMDIClient, pT->MDIGetActive() );
		return 0;
	}


	LRESULT OnWindowRestore(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		_SendSysCommand(SC_RESTORE);
		return 0;
	}


	LRESULT OnWindowMove(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		_SendSysCommand(SC_MOVE);
		return 0;
	}


	LRESULT OnWindowSize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		_SendSysCommand(SC_SIZE);
		return 0;
	}


	LRESULT OnWindowMinimize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		_SendSysCommand(SC_MINIMIZE);
		return 0;
	}


	LRESULT OnWindowMaximize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		_SendSysCommand(SC_MAXIMIZE);
		return 0;
	}


private:
	void _SendSysCommand(int wID)
	{
		T * 		pT		   = static_cast<T *>(this);

		ATLASSERT( ::IsWindow(pT->m_hWnd) );
		ATLASSERT( ::IsWindow(pT->m_hWndMDIClient) );
		BOOL		bMaximized = FALSE;
		CWindow 	wnd 	   = pT->MDIGetActive(&bMaximized);

		if (wnd.m_hWnd == NULL)
			return;

		if (wID == SC_MAXIMIZE && bMaximized)
			return;

		wnd.SendMessage(WM_SYSCOMMAND, (WPARAM) wID);
	}
};




////////////////////////////////////////////////////////////////////////////
// Fixing CMDIChildWindowImpl
// WTL sucks, it support only english menu.

template < class T, int __nWindowMenuPos = 2, class TBase = CMDIWindow, class TWinTraits = CMDIChildWinTraits >
class ATL_NO_VTABLE CMDIChildWindowImplFixed : public CMDIChildWindowImpl<T, TBase, TWinTraits> {
//public:
	// Declarations
	typedef CMDIChildWindowImpl<T, TBase, TWinTraits>  baseClass;

	// Overrides
	static DWORD GetWndStyle(DWORD dwStyle)
	{	// created invisibly
		return TWinTraits::GetWndStyle(dwStyle) & ~(WS_VISIBLE | WS_CLIPCHILDREN /* makes drawing slower*/ | WS_CLIPSIBLINGS);
	}


public:
	//無理矢理上書き(minit) : WTL7.1以降親切にも最大化のルーチンが入ったが、
	//それが原因でタブのアクティブ化が阻害されるので、それを除いたバージョンをここに記述する
	HWND Create(
			HWND			hWndParent,
			ATL::_U_RECT	rect			= NULL,
			LPCTSTR 		szWindowName	= NULL,
			DWORD			dwStyle 		= 0,
			DWORD			dwExStyle		= 0,
			UINT			nMenuID 		= 0,
			LPVOID			lpCreateParam	= NULL)
	{
		ATOM	 atom = T::GetWndClassInfo().Register(&m_pfnSuperWindowProc);
		if (nMenuID != 0)
		  #if (_ATL_VER >= 0x0700)
			m_hMenu = ::LoadMenu( ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(nMenuID) );
		  #else 	//!(_ATL_VER >= 0x0700)
			m_hMenu = ::LoadMenu( _Module.GetResourceInstance(), MAKEINTRESOURCE(nMenuID) );
		  #endif	//!(_ATL_VER >= 0x0700)

		dwStyle 			 = T::GetWndStyle(dwStyle);
		dwExStyle			 = T::GetWndExStyle(dwExStyle);

		dwExStyle			|= WS_EX_MDICHILD;	// force this one
		m_pfnSuperWindowProc = ::DefMDIChildProc;
		m_hWndMDIClient 	 = hWndParent;
		ATLASSERT( ::IsWindow(m_hWndMDIClient) );

		if (rect.m_lpRect == NULL)
			rect.m_lpRect = &TBase::rcDefault;

		// If the currently active MDI child is maximized, we want to create this one maximized too
		ATL::CWindow wndParent = hWndParent;

		HWND		 hWnd	   = CFrameWindowImplBase<TBase, TWinTraits >::Create(
									hWndParent,
									rect.m_lpRect,
									szWindowName,
									dwStyle,
									dwExStyle,
									(UINT) 0U,
									atom,
									lpCreateParam );

		if ( hWnd != NULL && ::IsWindowVisible(m_hWnd) && !::IsChild( hWnd, ::GetFocus() ) ) {
			::SetFocus(hWnd);
		}

		return hWnd;
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CMDIChildWindowImplFixed)
		MSG_WM_MDIACTIVATE(OnMDIActivate)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()


private:
	void OnMDIActivate(HWND hwndChildDeact, HWND hwndChildAct)
	{
		if (hwndChildAct == m_hWnd && m_hMenu != NULL)
			_SetMDIFrameMenuFixed();
		else if (hwndChildAct == NULL)
			::SendMessage(GetMDIFrame(), WM_MDISETMENU, 0, 0);
		// don't allow CMDIChildWindowImpl to handle this one
		DefWindowProc();
	}


public:
	// Methods
	int ActivateFrame(int nCmdShow = -1)
	{
		// cf.MFC6::CMDIChildWnd::ActivateFrame
		// determine default show command
		if (nCmdShow == -1) {
			// get maximized state of previously active child
			BOOL  bMaximized = FALSE;
			HWND  hWndActive = MDIGetActive(&bMaximized);
			// convert show command based on current style
			DWORD dwStyle	 = GetStyle();

			if ( bMaximized || !hWndActive || (dwStyle & WS_MAXIMIZE) )
				nCmdShow = SW_SHOWMAXIMIZED;
			else if (dwStyle & WS_MINIMIZE)
				nCmdShow = SW_SHOWMINIMIZED;
		}

		// translate default nCmdShow (-1)
		if (nCmdShow == -1) {
			if ( !IsWindowVisible() )
				nCmdShow = SW_SHOWNORMAL;
			else if ( IsIconic() )
				nCmdShow = SW_RESTORE;
		}

		// bring to top before showing <-important
		BringToTop(nCmdShow);

		if (nCmdShow != -1) {
			// show the window as specified
			ShowWindow(nCmdShow);
			// and finally, bring to top after showing
			BringToTop(nCmdShow);
		}

		// strangely, MDIRefreshMenu in CMDIWindowFixed::SetMDIFrameMenuFixed ignored
		MDIRefreshMenu();	// refresh the Window menu

		ATLTRACE2( atlTraceGeneral, 4, _T("CChildFrame::ActivateFrame - activated!\n") );
		return nCmdShow;
	}


private:
	void BringToTop(int nCmdShow)
	{	// cf.MFC6::CFrameWnd::BringToTop
		// place the window on top except for certain nCmdShow
		if (  nCmdShow != SW_HIDE
		   && nCmdShow != SW_MINIMIZE
		   && nCmdShow != SW_SHOWMINNOACTIVE
		   && nCmdShow != SW_SHOWNA
		   && nCmdShow != SW_SHOWNOACTIVATE)
		{
			// if no last active popup, it will return m_hWnd
			HWND hWndLastPop = ::GetLastActivePopup(m_hWnd);
			::BringWindowToTop(hWndLastPop);
		}
	}


	// Implementation
	HMENU _GetStandardWindowMenuFixed(HMENU hMenu)
	{
		int    nCount	= ::GetMenuItemCount(hMenu);
		if (nCount == -1)
			return NULL;

		int    nLen 	= ::GetMenuString(hMenu, nCount - __nWindowMenuPos, NULL, 0, MF_BYPOSITION);
		if (nLen == 0)
			return NULL;

		LPTSTR lpszText = (LPTSTR) _alloca( (nLen + 1) * sizeof (TCHAR) );

		if (::GetMenuString(hMenu, nCount - __nWindowMenuPos, lpszText, nLen + 1, MF_BYPOSITION) != nLen)
			return NULL;

		//		if(lstrcmp(lpszText, m_lpszWindowMenu))
		//			return NULL;
		return ::GetSubMenu(hMenu, nCount - __nWindowMenuPos);
	}


	void _SetMDIFrameMenuFixed()
	{
		ATLTRACE2( atlTraceGeneral, 4, _T("CMDIWindowFixed::SetMDIFrameMenuFixed\n") );
		HMENU	hWindowMenu = _GetStandardWindowMenuFixed(m_hMenu);
		MDISetMenu(m_hMenu, hWindowMenu);
		MDIRefreshMenu();
		::DrawMenuBar( GetMDIFrame() );
	}
};



template <class T>
class CClipboardViewer {
private:	//public:
	// Data members
	HWND	m_hWndNext;
	bool	m_bFirst;
	bool	m_bInitialUpdate;
	bool	m_bSetClipboardViewer;


public:
	// Constructor
	CClipboardViewer()
		: m_hWndNext(NULL)
		, m_bFirst(true)
		, m_bInitialUpdate(false)
		, m_bSetClipboardViewer(false)
	{
	}


	~CClipboardViewer()
	{
		ATLASSERT(m_bSetClipboardViewer == false);
	}


	void InstallClipboardViewer(bool bInitialUpdate = false)
	{
		if (m_bSetClipboardViewer)
			return;

		T *pT = static_cast<T *>(this);
		m_bInitialUpdate	  = bInitialUpdate;
		m_hWndNext			  = ::SetClipboardViewer(pT->m_hWnd);
		m_bInitialUpdate	  = bInitialUpdate;

		m_bSetClipboardViewer = true;
	}


	void UninstallClipboardViewer()
	{
		if (!m_bSetClipboardViewer)
			return;

		T *pT = static_cast<T *>(this);
		::ChangeClipboardChain(pT->m_hWnd, m_hWndNext);
		m_hWndNext			  = NULL;
		m_bFirst			  = true;
		m_bInitialUpdate	  = false;

		m_bSetClipboardViewer = false;
	}


private:
	// Overridables
	void OnUpdateClipboard()
	{
		ATLASSERT(FALSE);
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CClipboardViewer)
		MSG_WM_DRAWCLIPBOARD(OnDrawClipboard)
		MSG_WM_CHANGECBCHAIN(OnChangeCBChain)
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()


private:
	void OnDrawClipboard()
	{
		ATLTRACE2( atlTraceGeneral, 4, _T("CClipboardViewer::OnDrawClipboard\n") );
		T *pT = static_cast<T *>(this);

		if (m_hWndNext != NULL)
			::PostMessage(m_hWndNext, WM_DRAWCLIPBOARD, 0, 0);		//!!Post!?

		if (m_bFirst) { 				// first time
			if (m_bInitialUpdate) { 	// callback
				pT->OnUpdateClipboard();
				m_bInitialUpdate = false;
			}

			m_bFirst = false;
		} else {
			pT->OnUpdateClipboard();
		}
	}


	void OnChangeCBChain(HWND hWndRemove, HWND hWndAfter)
	{
		ATLTRACE2( atlTraceGeneral, 4, _T("CClipboardViewer::OnChangeCBChain\n") );

		if (m_hWndNext == hWndRemove)
			m_hWndNext = hWndAfter;
		else if (m_hWndNext != NULL)	// SendMessage if I'm not last
			::SendMessage(m_hWndNext, WM_CHANGECBCHAIN, (WPARAM) hWndRemove, (LPARAM) hWndAfter);
	}


	void OnDestroy()
	{
		SetMsgHandled(FALSE);
		UninstallClipboardViewer();
	}

};



inline void MtlDestroyImageLists(HWND hWndToolBar)
{
	ATLTRACE2( atlTraceGeneral, 4, _T("MtlDestroyImageList\n") );
	CToolBarCtrl toolbar(hWndToolBar);
	HIMAGELIST	 hImg = toolbar.GetImageList();

	if (hImg) {
		ATLTRACE2( atlTraceGeneral, 4, _T(" delete imagelist\n") );
		MTLVERIFY( ImageList_Destroy(hImg) );
	}

	hImg = toolbar.GetHotImageList();

	if (hImg) {
		ATLTRACE2( atlTraceGeneral, 4, _T(" delete hot imagelist\n") );
		MTLVERIFY( ImageList_Destroy(hImg) );
	}
}



// __nID == 0 then separator
HWND	MtlCreateSimpleToolBarCtrl(
			HWND		hWndParent,
			int *		__firstID,
			int *		__lastID,
			UINT		nImageBmpID,
			UINT		nHotImageBmpID,
			int 		cx,
			int 		cy,
			COLORREF	clrMask,
			UINT		nFlags	= ILC_COLOR24,
			DWORD		dwStyle = ATL_SIMPLE_TOOLBAR_PANE_STYLE,
			UINT		nID 	= ATL_IDW_TOOLBAR);


#ifdef __ATLCTRLW_H__
BOOL	MtlCmdBarLoadImages(
			CCommandBarCtrl& cmdbar,
			int *			__firstID,
			int *			__lastID,
			UINT			nImageBmpID,
			int 			cx,
			int 			cy,
			COLORREF		clrMask,
			UINT			nFlags = ILC_COLOR24);
#endif



////////////////////////////////////////////////////////////////////////////
// Mouse Wheel Support for Windows 95 or Windows NT 3.51

class CMSMouseWheelMessageHandlerBase {
public:
	DECLARE_REGISTERED_MESSAGE(MSWHEEL_ROLLMSG)
};



template <class T>
class CMSMouseWheelMessageHandler : public CMSMouseWheelMessageHandlerBase {
public:
	BEGIN_MSG_MAP(CMSMouseWheelMessageHandler<T>)
		MESSAGE_HANDLER(GET_REGISTERED_MESSAGE(MSWHEEL_ROLLMSG), OnRegisteredMouseWheel)
	END_MSG_MAP()

private:
	LRESULT OnRegisteredMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{	// pasted from MFC6
		// convert from MSH_MOUSEWHEEL to WM_MOUSEWHEEL

		WORD	   keyState  = 0;
		keyState |= (::GetKeyState(VK_CONTROL) < 0) ? MK_CONTROL : 0;
		keyState |= (::GetKeyState(VK_SHIFT  ) < 0) ? MK_SHIFT	 : 0;

		LRESULT    lResult;
		HWND	   hwFocus	 = ::GetFocus();
		const HWND hwDesktop = ::GetDesktopWindow();

		if (hwFocus == NULL) {
			T *pT = static_cast<T *>(this);
			lResult = pT->SendMessage(WM_MOUSEWHEEL, (wParam << 16) | keyState, lParam);
		} else {
			do {
				lResult = ::SendMessage(hwFocus, WM_MOUSEWHEEL, (wParam << 16) | keyState, lParam);
				hwFocus = ::GetParent(hwFocus);
			} while (lResult == 0 && hwFocus != NULL && hwFocus != hwDesktop);
		}

		return lResult;
	}
};




template <class T>
class CHelpMessageLine {
public:
	BEGIN_MSG_MAP(CHelpMessageLine)
		MESSAGE_HANDLER(WM_MENUSELECT, OnMenuSelect)
	END_MSG_MAP()

private:
	LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		T * 	pT	   = static_cast<T *>(this);

		if (pT->m_hWndStatusBar == NULL)
			return pT->DefWindowProc(uMsg, wParam, lParam);

		HMENU		hMenu  = (HMENU) lParam;
		WORD		wFlags = HIWORD(wParam);
		UINT		uItem  = (UINT) LOWORD(wParam); 	// menu item or submenu index

		CString 	strHelp;

		if (wFlags == 0xFFFF && hMenu == NULL) {												// menu closing
			::SendMessage(pT->m_hWndStatusBar, SB_SIMPLE, FALSE, 0L);
		} else {
			if ( !(wFlags & MF_POPUP) ) {
				WORD wID	= LOWORD(wParam);

				// check for special cases
				if (wID >= 0xF000 && wID < 0xF1F0)												// system menu IDs
					wID = (WORD) ( ( (wID - 0xF000) >> 4 ) + ATL_IDS_SCFIRST );
				else if (wID >= ID_RECENTDOCUMENT_FIRST && wID <= ID_RECENTDOCUMENT_LAST)		// MRU items
					wID = ATL_IDS_MRU_FILE;
				else if (wID >= ATL_IDM_FIRST_MDICHILD) 										// MDI child windows
					wID = ATL_IDS_MDICHILD;

				strHelp.LoadString(wID);
				int  nStart = strHelp.Find( _T('\t') );
				int  nLast	= strHelp.Find( _T('\n') );

				if (nStart == -1 && nLast != -1) {
					strHelp = strHelp.Mid(0, nLast);
				} else if (nStart != -1 && nLast == -1) {
					strHelp = strHelp.Right(strHelp.GetLength() - nStart - 1);
				} else if (nStart != -1 && nLast != -1) {
					ATLASSERT(nStart < nLast);
					strHelp = strHelp.Mid(nStart + 1, nLast - nStart - 1);
				}

			} else {																			// popup support
				HMENU hMenuPopup = ::GetSubMenu(hMenu, uItem);

				UINT  	nID		 = -1;
				enum { 	LEN		 = 100 };
				TCHAR 	szString[LEN+1];

				// search the first non-popup menu's ID
				for (int i = 0; i < ::GetMenuItemCount(hMenuPopup); ++i) {
					// Fixed by Nisizawa@mcomp.co.jp, thanks.
					// Note. GetMenuItemInfo often change mii.dwTypeData.
					//		 even if the API reference says not.
					CMenuItemInfo mii;
					mii.fMask	   = MIIM_ID | MIIM_TYPE | MIIM_SUBMENU;
					szString[0]	   = 0;
					mii.dwTypeData = szString;
					mii.cch 	   = LEN/*100*/;

					if ( !::GetMenuItemInfo(hMenuPopup, i, TRUE, &mii) )
						continue;

					bool		  bSkip = _check_flag(MFT_SEPARATOR, mii.fType) || mii.hSubMenu != NULL;
					nID 		   =  bSkip ? -1 : mii.wID;

					if (nID != -1) {
						if (nID >= ID_RECENTDOCUMENT_FIRST && nID <= ID_RECENTDOCUMENT_LAST)	// MRU items
							nID = ATL_IDS_MRU_FILE;
						else if (nID >= ATL_IDM_FIRST_MDICHILD) 								// MDI child windows
							nID = ATL_IDS_MDICHILD;

						break;
					}
				}

				if (nID != -1) {
					strHelp.LoadString(nID);
					int nIndex = strHelp.Find( _T('\t') );

					if (nIndex != -1) {
						strHelp = strHelp.Left(nIndex);
					} else {
						strHelp.Empty();
					}
				}
			}

			::SendMessage(pT->m_hWndStatusBar, SB_SIMPLE, TRUE, 0L);
			::SendMessage(pT->m_hWndStatusBar, SB_SETTEXT, (255 | SBT_NOBORDERS), (LPARAM) (LPCTSTR) strHelp);
		}

		return pT->DefWindowProc(uMsg, wParam, lParam);
	}
};




/////////////////////////////////////////////////////////////////////////////
// CInitDialogImpl - property page that supports DropDownList initialization

#ifndef _ATL_RT_DLGINIT
	#define _ATL_RT_DLGINIT  MAKEINTRESOURCE(240)
#endif



template <class T, class TBase = CWindow>
class ATL_NO_VTABLE CInitDialogImpl : public CDialogImpl< T, TBase > {
	typedef CInitDialogImpl< T, TBase >  thisClass;
	typedef CDialogImpl< T, TBase > 	 baseClass;

	// Data members
	HGLOBAL 	m_hInitData;

public:
	// Constructor/destructor
	CInitDialogImpl() : m_hInitData(NULL)
	{
		T * 	  pT			 = static_cast<T *>(this);
		pT; // avoid level 4 warning

		HINSTANCE hInstance 	 = _Module.GetResourceInstance();
		LPCTSTR   lpTemplateName = MAKEINTRESOURCE(pT->IDD);
		HRSRC	  hDlg			 = ::FindResource(hInstance, lpTemplateName, (LPTSTR) RT_DIALOG);

		if (hDlg != NULL) {
			HRSRC hDlgInit = ::FindResource(hInstance, lpTemplateName, (LPTSTR) _ATL_RT_DLGINIT);

			if (hDlgInit != NULL) {
				m_hInitData = ::LoadResource(hInstance, hDlgInit);
			}
		} else {
			ATLASSERT( FALSE && _T("CInitDialogImpl - Cannot find dialog template!") );
		}
	}

	~CInitDialogImpl()
	{
		if (m_hInitData != NULL) {
			UnlockResource(m_hInitData);
			::FreeResource(m_hInitData);
		}
	}

	// Methods
	bool ExecuteDlgInit()
	{
		BYTE *pInitData = (BYTE *) ::LockResource(m_hInitData);

		return _ExecuteDlgInit(pInitData);
	}


private:
	bool _ExecuteDlgInit(LPVOID lpResource)
	{		// cf.MFC
		bool bSuccess = true;

		if (lpResource != NULL) {
			UNALIGNED WORD *lpnRes = (WORD *) lpResource;

			while (bSuccess && *lpnRes != 0) {
				WORD  nIDC	= *lpnRes++;
				WORD  nMsg	= *lpnRes++;
				DWORD dwLen = *( (UNALIGNED DWORD * &)lpnRes )++;

			  #if 0
				// In Win32 the WM_ messages have changed.	They have
				// to be translated from the 32-bit values to 16-bit
				// values here.
				#define WIN16_LB_ADDSTRING		0x0401
				#define WIN16_CB_ADDSTRING		0x0403
				#define AFX_CB_ADDSTRING		0x1234
			  #endif

				// unfortunately, WIN16_CB_ADDSTRING == CBEM_INSERTITEM
				if (nMsg == AFX_CB_ADDSTRING)
					nMsg = CBEM_INSERTITEM;
				else if (nMsg == WIN16_LB_ADDSTRING)
					nMsg = LB_ADDSTRING;
				else if (nMsg == WIN16_CB_ADDSTRING)
					nMsg = CB_ADDSTRING;

				// check for invalid/unknown message types
				ATLASSERT(nMsg == LB_ADDSTRING || nMsg == CB_ADDSTRING || nMsg == CBEM_INSERTITEM);

			  #ifdef _DEBUG
				// For AddStrings, the count must exactly delimit the
				// string, including the NULL termination.	This check
				// will not catch all mal-formed ADDSTRINGs, but will
				// catch some.
				if (nMsg == LB_ADDSTRING || nMsg == CB_ADDSTRING || nMsg == CBEM_INSERTITEM)
					ATLASSERT(*( (LPBYTE) lpnRes + (UINT) dwLen - 1 ) == 0);
			  #endif

				if (nMsg == CBEM_INSERTITEM) {
					USES_CONVERSION;
					COMBOBOXEXITEM	item;
					item.mask	 = CBEIF_TEXT;
					item.iItem	 = -1;
					item.pszText = A2T( LPSTR(lpnRes) );

					if (::SendDlgItemMessage(m_hWnd, nIDC, nMsg, 0, (LPARAM) &item) == -1)
						bSuccess = false;

				} else if (nMsg == LB_ADDSTRING || nMsg == CB_ADDSTRING) {
					// List/Combobox returns -1 for error
					if (::SendDlgItemMessageA(m_hWnd, nIDC, nMsg, 0, (LPARAM) lpnRes) == -1)
						bSuccess = false;
				}

				// skip past data
				lpnRes = (WORD *) ( (LPBYTE) lpnRes + (UINT) dwLen );
			}
		}

		// send update message to all controls after all other siblings loaded
		//		if (bSuccess)
		//			SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, FALSE, FALSE);

		return bSuccess;
	}


public:
	// DDX methods
	void DDX_CBIndex(UINT nID, int &nValue, BOOL bSave)
	{
		HWND hWndCtrl = GetDlgItem(nID);

		ATLASSERT( ::IsWindow(hWndCtrl) );

		if (bSave) {
			nValue = (int) ::SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0L);
			ATLASSERT( nValue >= 0 && nValue < (int) ::SendMessage(hWndCtrl, CB_GETCOUNT, 0, 0L) );

		} else {
			if ( nValue < 0 || nValue >= (int) ::SendMessage(hWndCtrl, CB_GETCOUNT, 0, 0L) ) {
				ATLTRACE2(atlTraceUI, 0, "ATL: Warning - dialog data combobox value (%d) out of range.\n", nValue);
				nValue = 0; 	// default to off
			}

			LRESULT nRet = ::SendMessage(hWndCtrl, CB_SETCURSEL, nValue, 0L);
			ATLASSERT(nRet != CB_ERR);
		}
	}
};



/////////////////////////////////////////////////////////////////////////////
// CInitPropertyPageImpl - property page that supports DropDownList initialization
//	 That is, the subset of CAxProtertyPageImpl
//	 If you use combobox ex, don't miss ICC_USEREX_CLASSES.

#define DDX_CBINDEX(nID, var)				   \
	if (nCtlID == (UINT) -1 || nCtlID == nID)  \
		DDX_CBIndex(nID, var, bSaveAndValidate);


template <class T, class TBase = CPropertyPageWindow>
class ATL_NO_VTABLE CInitPropertyPageImpl : public CPropertyPageImpl< T, TBase > {
	typedef CInitPropertyPageImpl< T, TBase >  thisClass;
	typedef CPropertyPageImpl< T, TBase >	   baseClass;

	// Data members
	HGLOBAL 	m_hInitData;

public:
	// Constructor/destructor
	CInitPropertyPageImpl(_U_STRINGorID title = (LPCTSTR) NULL)
		: CPropertyPageImpl< T, TBase >(title)
		, m_hInitData(NULL)
	{
		T * 	pT	= static_cast<T *>(this);
		pT; 					// avoid level 4 warning

		HINSTANCE hInstance    = _Module.GetResourceInstance();
		LPCTSTR lpTemplateName = MAKEINTRESOURCE(pT->IDD);
		HRSRC hDlg			   = ::FindResource(hInstance, lpTemplateName, (LPTSTR) RT_DIALOG);

		if (hDlg != NULL) {
			HRSRC hDlgInit = ::FindResource(hInstance, lpTemplateName, (LPTSTR) _ATL_RT_DLGINIT);

			if (hDlgInit != NULL) {
				m_hInitData = ::LoadResource(hInstance, hDlgInit);
			}
		} else {
			ATLASSERT( FALSE && _T("CInitPropertyPageImpl - Cannot find dialog template!") );
		}
	}


	~CInitPropertyPageImpl()
	{
		if (m_hInitData != NULL) {
			UnlockResource(m_hInitData);
			::FreeResource(m_hInitData);
		}
	}


public:
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()


private:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &bHandled)
	{
		bHandled = FALSE;

		BYTE *pInitData = (BYTE *) ::LockResource(m_hInitData);
		ExecuteDlgInit(pInitData);

		return TRUE;
	}


	// Methods
	bool ExecuteDlgInit(LPVOID lpResource)
	{							// cf.MFC
		bool bSuccess = true;

		if (lpResource != NULL) {
			UNALIGNED WORD *lpnRes = (WORD *) lpResource;

			while (bSuccess && *lpnRes != 0) {
				WORD nIDC	= *lpnRes++;
				WORD nMsg	= *lpnRes++;
				DWORD dwLen = *( (UNALIGNED DWORD * &)lpnRes )++;

			  #if 0
				// In Win32 the WM_ messages have changed.	They have
				// to be translated from the 32-bit values to 16-bit
				// values here.
				#define WIN16_LB_ADDSTRING		0x0401
				#define WIN16_CB_ADDSTRING		0x0403
				#define AFX_CB_ADDSTRING		0x1234
			  #endif

				// unfortunately, WIN16_CB_ADDSTRING == CBEM_INSERTITEM
				if (nMsg == AFX_CB_ADDSTRING)
					nMsg = CBEM_INSERTITEM;
				else if (nMsg == WIN16_LB_ADDSTRING)
					nMsg = LB_ADDSTRING;
				else if (nMsg == WIN16_CB_ADDSTRING)
					nMsg = CB_ADDSTRING;

				// check for invalid/unknown message types
				ATLASSERT(nMsg == LB_ADDSTRING || nMsg == CB_ADDSTRING || nMsg == CBEM_INSERTITEM);

			  #ifdef _DEBUG
				// For AddStrings, the count must exactly delimit the
				// string, including the NULL termination.	This check
				// will not catch all mal-formed ADDSTRINGs, but will
				// catch some.
				if (nMsg == LB_ADDSTRING || nMsg == CB_ADDSTRING || nMsg == CBEM_INSERTITEM)
					ATLASSERT(*( (LPBYTE) lpnRes + (UINT) dwLen - 1 ) == 0);
			  #endif

				if (nMsg == CBEM_INSERTITEM) {
					USES_CONVERSION;
					COMBOBOXEXITEM item;
					item.mask	 = CBEIF_TEXT;
					item.iItem	 = -1;
					item.pszText = A2T( LPSTR(lpnRes) );

					if (::SendDlgItemMessage(m_hWnd, nIDC, nMsg, 0, (LPARAM) &item) == -1)
						bSuccess = false;
				} else if (nMsg == LB_ADDSTRING || nMsg == CB_ADDSTRING) {
					// List/Combobox returns -1 for error
					if (::SendDlgItemMessageA(m_hWnd, nIDC, nMsg, 0, (LPARAM) lpnRes) == -1)
						bSuccess = false;
				}


				// skip past data
				lpnRes = (WORD *) ( (LPBYTE) lpnRes + (UINT) dwLen );
			}
		}

		// send update message to all controls after all other siblings loaded
		//		if (bSuccess)
		//			SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, FALSE, FALSE);

		return bSuccess;
	}


public:
	// DDX methods
	void DDX_CBIndex(UINT nID, int &nValue, BOOL bSave)
	{
		HWND hWndCtrl = GetDlgItem(nID);

		if (bSave) {
			nValue = (int) ::SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0L);
			ATLASSERT( nValue >= 0 && nValue < (int) ::SendMessage(hWndCtrl, CB_GETCOUNT, 0, 0L) );
		} else {
			if ( nValue < 0 || nValue >= (int) ::SendMessage(hWndCtrl, CB_GETCOUNT, 0, 0L) ) {
				ATLTRACE2(atlTraceUI, 0, "ATL: Warning - dialog data combobox value (%d) out of range.\n", nValue);
				nValue = 0;  // default to off
			}

			::SendMessage(hWndCtrl, CB_SETCURSEL, nValue, 0L);
		}
	}
};



/////////////////////////////////////////////////////////////////////////////
// WTL forgot to center a sheet...
class CCenterPropertySheet : public CPropertySheetImpl<CCenterPropertySheet> {
	bool	m_bCentered;

public:
	CCenterPropertySheet(_U_STRINGorID title = (LPCTSTR) NULL, UINT uStartPage = 0, HWND hWndParent = NULL)
		: CPropertySheetImpl<CCenterPropertySheet>(title, uStartPage, hWndParent)
		, m_bCentered(false)
	{
	}

	BEGIN_MSG_MAP(CCenterPropertySheet)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_COMMAND, CPropertySheetImpl<CCenterPropertySheet>::OnCommand)
	END_MSG_MAP()


private:
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		if (!m_bCentered) {
			// now center the property sheet
			CenterWindow( GetParent() );
			m_bCentered = true;
		}

		return 0;
	}

};



/////////////////////////////////////////////////////////////////////////////
// menu helper

inline int MtlGetCmdIDFromAccessKey(CMenuHandle menu, const CString &strAccess, bool bFromBottom = true)
{
	int 	nID 	= -1;
	CString strItem;

	if (bFromBottom) {
		for (int i = menu.GetMenuItemCount() - 1; i >= 0; --i) {
			if (menu.GetMenuString(i, strItem, MF_BYPOSITION) == 0)
				continue;

			if (strItem.Find(strAccess) != -1) {
				nID = menu.GetMenuItemID(i);
				break;
			}
		}
	} else {
		for (int i = 0; i < menu.GetMenuItemCount(); ++i) {
			if (menu.GetMenuString(i, strItem, MF_BYPOSITION) == 0)
				continue;

			if (strItem.Find(strAccess) != -1) {
				nID = menu.GetMenuItemID(i);
				break;
			}
		}
	}

	return nID;
}



/////////////////////////////////////////////////////////////////////////////


}		//namespace MTL



#endif	// __MTLUSER_H__
