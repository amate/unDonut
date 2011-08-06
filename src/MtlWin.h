/**
 *	@file	MtlWin.h
 *	@brief	MTL : ウィンドウ関係
 */
////////////////////////////////////////////////////////////////////////////
// MTL Version 0.10
// Copyright (C) 2001 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// Mtlwin.h: Last updated: February 12, 2001
////////////////////////////////////////////////////////////////////////////

#ifndef __MTLWIN_H__
#define __MTLWIN_H__

#pragma once

//何故かここでアプリ定義の定数が登場。もはやライブラリとはいえない
#include "DonutDefine.h"
#include "MtlBase.h"


namespace MTL {


enum EMtl_Translate {
	_MTL_TRANSLATE_PASS 	= 0x00,
	_MTL_TRANSLATE_HANDLE	= 0x01,
	_MTL_TRANSLATE_WANT 	= 0x02,
};



////////////////////////////////////////////////////////////////////////////
// Easy macros

#define HANDLE_MESSAGE(msg) \
	if (uMsg == msg) {		\
		bHandled = TRUE;	\
		return TRUE;		\
	}


#define DEFWINDOWPROC_MESSAGE(msg)					\
	if (uMsg == msg) {								\
		return DefWindowProc(uMsg, wParam, lParam); \
	}


// System global window message
#define DECLARE_REGISTERED_MESSAGE(message) 								\
	static UINT GetRegistered##message()									\
	{																		\
		static UINT uRegisterd##message = 0;								\
		if (uRegisterd##message == 0) { 									\
			uRegisterd##message = ::RegisterWindowMessage( _T(#message) );	\
		}																	\
		ATLASSERT(uRegisterd##message != 0);								\
		return uRegisterd##message; 										\
	}


#define GET_REGISTERED_MESSAGE(message) 	GetRegistered##message()


// WTL sucks
#define DECLARE_FRAME_WND_MENU(menu)														\
	HWND CreateEx(HWND hWndParent,															\
				  _U_RECT rect				= NULL, 										\
				  LPCTSTR lpcstrWindowName	= NULL, 										\
				  DWORD   dwStyle			= 0,											\
				  DWORD   dwExStyle 		= 0,											\
				  LPVOID  lpCreateParam 	= NULL) 										\
	{																						\
		TCHAR	szWindowName[256];															\
		szWindowName[0] = 0;																\
		if (lpcstrWindowName == NULL) { 													\
			::LoadString(_Module.GetResourceInstance(), GetWndClassInfo().m_uCommonResourceID, szWindowName, 256); \
			lpcstrWindowName	= szWindowName; 											\
		}																					\
		HWND	hWnd = Create(																\
						hWndParent, 														\
						rect,																\
						lpcstrWindowName,													\
						dwStyle,															\
						dwExStyle,															\
						menu,																\
						lpCreateParam); 													\
		if (hWnd != NULL)																	\
			m_hAccel	= ::LoadAccelerators( _Module.GetResourceInstance(), MAKEINTRESOURCE(GetWndClassInfo().m_uCommonResourceID) );	\
		return	hWnd;																		\
	}




inline CString MtlGetWindowClassName(HWND hWnd)
{
	enum { NAME_SIZE = 0x1000/*255*/ };
	TCHAR	name[ NAME_SIZE + 1 + 1 ] = _T("\0");
	int nRetLen = ::GetClassName(hWnd, name, NAME_SIZE+1 );
	if (nRetLen < 0)
		nRetLen = 0;
	else if (nRetLen > NAME_SIZE)
		nRetLen = NAME_SIZE;
	name[nRetLen] = _T('\0');
	return CString(name);
}



// Antithesis to MFC
#if 1	//+++ アロケートの頻度を減らすように修正
inline bool MtlIsKindOf(HWND hWnd, const TCHAR* pClassName)
{
	enum { NAME_SIZE = 0x1000/*255*/ };
	TCHAR	name[ NAME_SIZE + 1 ] = _T("\0");
	int nRetLen = ::GetClassName(hWnd, name, NAME_SIZE+1 );
	if (nRetLen < 0)
		nRetLen = 0;
	else if (nRetLen > NAME_SIZE)
		nRetLen = NAME_SIZE;
	name[nRetLen] = _T('\0');
	return _tcscmp(name, pClassName) == 0;
}
#else
inline bool MtlIsKindOf(HWND hWnd, const CString &strClassName)
{
	return MtlGetWindowClassName(hWnd) == strClassName;
}
#endif


#define MTLASSERT_KINDOF(window_class_name, object) 	ATLASSERT( MtlIsKindOf(object, window_class_name) )



////////////////////////////////////////////////////////////////////////////
// Window message functions

inline CString MtlGetWindowText(HWND hWnd)
{
	int 	nLen	= ::GetWindowTextLength(hWnd);
  #if 1	//+++ 必要以上に アロケートしないように再修正
	enum { NAME_LEN = 0x1000 };
	TCHAR	name[ NAME_LEN + 1 + 1] = _T("\0");
	if (nLen <= NAME_LEN) {
		int 	nRetLen = ::GetWindowText(hWnd, name, nLen + 1);
		name[nLen]		= _T('\0');	//+++1.48c: いつぞやのバグ報告の反映. +1は不味い...
		return CString(name);
	} else {
		TCHAR*	pName	= new TCHAR[ nLen + 1 + 1] = _T("\0");
		name[nLen]		= _T('\0');	//+++1.48c: いつぞやのバグ報告の反映. +1は不味い...
		int 	nRetLen = ::GetWindowText(hWnd, pName, nLen + 1);
		CString str( pName );
		delete pName;
		return str;
	}
  #else
	CString 	strText;
	int nRetLen = ::GetWindowText(hWnd, strText.GetBufferSetLength(nLen), nLen + 1);
	strText.ReleaseBuffer();
	if(nRetLen < nLen)
		return CString();
	return strText;
  #endif
}



inline void MtlSetWindowText(HWND hWndCtrl, LPCTSTR lpszNew)
{ // MFC6::AfxSetWindowText
	int   nNewLen = lstrlen(lpszNew);
	TCHAR szOld[0x1000/*256*/];
	szOld[0] = 0;	//+++

	// fast check to see if text really changes (reduces flash in controls)
	if (nNewLen > _countof(szOld)
	   || ::GetWindowText( hWndCtrl, szOld, _countof(szOld) ) != nNewLen
	   || lstrcmp(szOld, lpszNew) != 0)
	{
		// change it
		::SetWindowText(hWndCtrl, lpszNew);
	}
}


//minit
inline void MtlSetStatusText(HWND hWndStatusBar, LPCTSTR lpszNew, int nPane = 0)
{
	::SetWindowText(hWndStatusBar, lpszNew);
	//::SendMessage(hWndStatusBar, SB_SETTEXT, (WPARAM)(nPane|SBT_OWNERDRAW), (LPARAM)lpszNew);
}


// Fatal bug fixed by JOBBY, thanks!
int 	MtlGetLBTextFixed(HWND hWndCombo, int nIndex, CString &strText);
int 	MtlListBoxGetText(HWND hWndBox	, int nIndex, CString &strText);


// UDT DGSTR ( add , delete TrayIcon
BOOL	TrayMessage(HWND hwnd, DWORD dwMessage, UINT uID, HICON hIcon, LPCTSTR pszTip);


inline BOOL MtlSendCommand(HWND hWnd, WORD wID)
{
	return ::SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(wID, 0), 0) != 0;
}


inline void MtlPostCommand(HWND hWnd, WORD wID)
{
	::PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(wID, 0), 0);
}



// Locking redraw
class CLockRedraw {
	CWindow 	m_wnd;

public:
	CLockRedraw(HWND hWnd);
	~CLockRedraw();
};



class CLockRedrawMDIClient {
	CWindow 	m_wndMDIClient;

public:
	CLockRedrawMDIClient(HWND hWnd);
	~CLockRedrawMDIClient();
};



struct CPostCommand {
private:
	WORD	m_wID;
	WORD	m_wNotifyCode;

public:
	CPostCommand(WORD wID, WORD wNotifyCode = 0) : m_wID(wID), m_wNotifyCode(wNotifyCode) { }

	void operator ()(HWND hWnd)
	{
		::PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(m_wID, m_wNotifyCode), 0);
	}
};



struct CSendCommand {
private:
	WORD	m_wID;
	WORD	m_wNotifyCode;
	BOOL	m_bResult;

public:
	CSendCommand(WORD wID, WORD wNotifyCode = 0)
		: m_wID(wID), m_wNotifyCode(wNotifyCode), m_bResult(FALSE)
	{
	}


	void operator ()(HWND hWnd)
	{
		m_bResult = ::SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(m_wID, m_wNotifyCode), 0) != 0;
	}

};



bool	MtlIsFamily(HWND hWnd1, HWND hWnd2);
void	MtlSetForegroundWindow(HWND hWnd);
bool	MtlIsApplicationActive(HWND hWnd);


//////////////////////////////////////////////////////////////////
// CTrackMouseLeave : マウスがウィンドウから離れたときOnTrackMouseLeaveを呼び出します

template <class T>
class CTrackMouseLeave {
public:
	bool	m_bTrackMouseLeave;

public:
	CTrackMouseLeave() : m_bTrackMouseLeave(false) {}

private:
	// Overridables
	void OnTrackMouseMove(UINT nFlags, CPoint pt) { }

	void OnTrackMouseLeave() { }


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CTrackMouseLeave<T>)
		MESSAGE_HANDLER(WM_MOUSEMOVE , OnMouseMove )
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
	END_MSG_MAP()


private:
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;

		_StartTrackMouseLeave();

		T *   pT = static_cast<T *>(this);
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		pT->OnTrackMouseMove( (UINT) wParam, pt );
		return 1;
	}


public:
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;

		_LastTrackMouseLeave();
		T *pT = static_cast<T *>(this);
		pT->OnTrackMouseLeave();
		return 0;
	}


private:
	BOOL _StartTrackMouseLeave()
	{
		T * 			pT = static_cast<T *>(this);

		if (m_bTrackMouseLeave)
			return FALSE;

		TRACKMOUSEEVENT tme;
		tme.cbSize		   = sizeof (tme);
		tme.dwFlags 	   = TME_LEAVE;
		tme.hwndTrack	   = pT->m_hWnd;
		m_bTrackMouseLeave = true;
		return ::_TrackMouseEvent(&tme);
	}


	void _LastTrackMouseLeave()
	{
		m_bTrackMouseLeave = false;
	}
};



template <class T>
class CFrameWndFixWrongActiveMessageHandler {
public:
	BEGIN_MSG_MAP(CFrameWndFixWrongActiveMessageHandler<T>)
		MSG_WM_ACTIVATE(OnActivate)
		// MSG_WM_NCACTIVATE(OnNcActivate)
		// MSG_WM_ENABLE(OnEnable)
	END_MSG_MAP()

private:
	// Overridables
	void OnFrameActivate(bool bActive) { }

	void OnFrameWrongInactivate() { }


	void OnActivate(UINT nState, BOOL bMinimized, HWND hWndOther)
	{
		//SetMsgHandled(FALSE);
		T *pT = static_cast<T *>(this);
		pT->DefWindowProc();
		pT->OnFrameActivate(nState != WA_INACTIVE);
	}


	BOOL OnNcActivate(BOOL bActive)
	{
		ATLTRACE(_T("CFrameWndFixWrongActiveMessageHandler:OnNcActivate(%d)\n"), m_bStayActive);
		T *pT = static_cast<T *>(this);

		// stay active if WF_STAYACTIVE bit is on
		//	if (m_bStayActive)
		//		bActive = TRUE;

		// but do not stay active if the window is disabled
		if ( !pT->IsWindowEnabled() )
			bActive = FALSE;

		// do not call the base class because it will call Default()
		//	and we may have changed bActive.
		return	pT->DefWindowProc(WM_NCACTIVATE, bActive, 0L) != 0;
	}


	void OnEnable(BOOL bEnable)
	{
		T *pT = static_cast<T *>(this);

		if (bEnable && m_bStayActive) {
			// Work around for MAPI support. This makes sure the main window
			// remains disabled even when the mail system is booting.
			pT->EnableWindow(FALSE);
			::SetFocus(NULL);
			return;
		}

		// only for top-level (and non-owned) windows
		if (pT->GetParent() != NULL)
			return;
	}
};



// OnIdle not called while Flush prugin running
// and WM_TIMER associated with HWND will not be sent.
class CCriticalIdleTimer {
	static UINT_PTR s_nTimerID;
	static HWND 	s_hWndMainFrame;
	static WORD 	s_wCmdID;

public:
	static void 			InstallCriticalIdleTimer(HWND hWndMainFrame, WORD wCmdID);
	static void 			UninstallCriticalIdleTimer();
	static VOID CALLBACK	FocusCheckerTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

	static void 			FocusChecker();
};




enum {
	NM_ON	= 0xffa6,		// (NM_FIRST-90)
	NM_OFF	= 0xffa5,		// (NM_FIRST-91)
};


inline void MtlSendOnCommand(HWND hWndTarget, WORD nCmd, HWND hWndSrc = NULL)
{
	::SendMessage(hWndTarget, WM_COMMAND, MAKEWPARAM(nCmd, NM_ON), (LPARAM) hWndSrc);
}


inline void MtlSendOffCommand(HWND hWndTarget, WORD nCmd, HWND hWndSrc = NULL)
{
	::SendMessage(hWndTarget, WM_COMMAND, MAKEWPARAM(nCmd, NM_OFF), (LPARAM) hWndSrc);
}


inline bool MtlIsWindowCurrentProcess(HWND hWnd)
{
	DWORD dwProcessId = 0;

	::GetWindowThreadProcessId(hWnd, &dwProcessId);
	return (::GetCurrentProcessId() == dwProcessId);
}



template <class _Function>
inline _Function MtlForEachTopLevelWindow(LPCTSTR lpszClass, LPCTSTR lpszWindow, _Function __f)
{
	for ( HWND	hWnd = ::FindWindowEx(NULL, NULL, lpszClass, lpszWindow);
		  hWnd != NULL;
		  hWnd = ::FindWindowEx(NULL, hWnd, lpszClass, lpszWindow) )
	{
		if ( !__f(hWnd) )
			break;
	}

	return __f;
}



////////////////////////////////////////////////////////////////////////////


}	// namespace MTL;



#endif	// __MTLWIN_H__
