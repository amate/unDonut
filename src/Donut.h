/**
 *	@file	Donut.h
 *	@brief	Donut固有のWindowメッセージ定義＋それようのサブルーチン.
 */

#pragma once

#include "option/MainOption.h"


#if 0	//+++ 未使用.
#define WM_USER_OPENFILES	(WM_USER + 29)
#define MSG_WM_USER_OPENFILES(func) 							\
	if (uMsg == WM_USER_OPENFILES) {							\
		SetMsgHandled(TRUE);									\
		func( (CSimpleArray<CString>*)wParam, (DWORD) lParam ); \
		lResult = 0;											\
		if ( IsMsgHandled() )									\
			return TRUE;										\
	}
#endif


#define WM_USER_OPENFILE	(WM_USER + 32)
#define MSG_WM_USER_OPENFILE(func)									  \
	if (uMsg == WM_USER_OPENFILE) { 								  \
		SetMsgHandled(TRUE);										  \
		lResult = (LRESULT) func( (LPCTSTR) wParam, (DWORD) lParam ); \
		if ( IsMsgHandled() )										  \
			return TRUE;											  \
	}


// WM_USER_GO's lParam flags
#define WM_USER_GET_IWEBBROWSER 	(WM_USER + 30)
#define MSG_WM_USER_GET_IWEBBROWSER()	   \
	if (uMsg == WM_USER_GET_IWEBBROWSER) { \
		SetMsgHandled(TRUE);			   \
		lResult = (LRESULT) m_spBrowser.p; \
		if ( IsMsgHandled() )			   \
			return TRUE;				   \
	}


#define MSG_WM_USER_GET_ACTIVE_IWEBBROWSER()				\
	if (uMsg == WM_USER_GET_IWEBBROWSER) {					\
		SetMsgHandled(TRUE);								\
		HWND hWnd = MDIGetActive(); 						\
		if (hWnd != NULL)									\
			lResult = (LRESULT) DonutGetIWebBrowser2(hWnd); \
		else												\
			lResult = NULL; 								\
		if ( IsMsgHandled() )								\
			return TRUE;									\
	}


#if 0	//+++ 未使用.
#define WM_USER_INITIAL_UPDATE	(WM_USER + 31)
#define MSG_WM_USER_INITIAL_UPDATE(func)							  \
	if (uMsg == WM_USER_INITIAL_UPDATE) {							  \
		SetMsgHandled(TRUE);										  \
		func( (IWebBrowser2 *) wParam, (IAxWinHostWindow *) lParam ); \
		lResult = 0;												  \
		if ( IsMsgHandled() )										  \
			return TRUE;											  \
	}
#endif


#define WM_USER_GET_ACTIVE_WINDOW	(WM_USER + 35)
#define MSG_WM_USER_GET_ACTIVE_WINDOW() 	 \
	if (uMsg == WM_USER_GET_ACTIVE_WINDOW) { \
		SetMsgHandled(TRUE);				 \
		lResult = (LRESULT) MDIGetActive();  \
		if ( IsMsgHandled() )				 \
			return TRUE;					 \
	}


#define WM_USER_BROWSER_CAN_SETFOCUS	(WM_USER + 36)
#define MSG_WM_USER_BROWSER_CAN_SETFOCUS(func)	\
	if (uMsg == WM_USER_BROWSER_CAN_SETFOCUS) { \
		SetMsgHandled(TRUE);					\
		lResult = (LRESULT) func(); 			\
		if ( IsMsgHandled() )					\
			return TRUE;						\
	}


#if 0	//+++ 未使用.
#define WM_USER_GET_DLCONTROL_FLAGS 	(WM_USER + 40)
#define MSG_WM_USER_GET_DLCONTROL_FLAGS(func)	   \
	if (uMsg == WM_USER_GET_DLCONTROL_FLAGS) {	   \
		SetMsgHandled(TRUE);					   \
		lResult = (LRESULT) func( (BOOL) wParam ); \
		if ( IsMsgHandled() )					   \
			return TRUE;						   \
	}
#endif


// U.H
#define WM_USER_SHOW_TEXT_CHG	(WM_USER + 41)
#define MSG_WM_USER_SHOW_TEXT_CHG(func)  \
	if (uMsg == WM_USER_SHOW_TEXT_CHG) { \
		SetMsgHandled(TRUE);			 \
		lResult = 0;					 \
		func( (BOOL) wParam );			 \
		if ( IsMsgHandled() )			 \
			return TRUE;				 \
	}
//END


#define WM_USER_IS_SELECTED_TAB 	(WM_USER + 42)
#define MSG_WM_USER_IS_SELECTED(func)			   \
	if (uMsg == WM_USER_IS_SELECTED_TAB) {		   \
		SetMsgHandled(TRUE);					   \
		lResult = (LRESULT) func( (HWND) wParam ); \
		if ( IsMsgHandled() )					   \
			return TRUE;						   \
	}


#define WM_USER_WINDOWS_CLOSE_CMP	(WM_USER + 43)
#define MSG_WM_USER_WINDOWS_CLOSE_CMP(func) 					 \
	if (uMsg == WM_USER_WINDOWS_CLOSE_CMP) {					 \
		SetMsgHandled(TRUE);									 \
		lResult = (LRESULT) func( (int) wParam, (BOOL) lParam ); \
		if ( IsMsgHandled() )									 \
			return TRUE;										 \
	}

#define WM_USER_SIZE_CHG_EX 		(WM_USER + 44)

//-------------------------------------------------------------------------------------


inline IWebBrowser2 *DonutGetIWebBrowser2(HWND hWnd)
{
	return (IWebBrowser2 *) ::SendMessage(hWnd, WM_USER_GET_IWEBBROWSER, 0, 0);
}


/// DonutでファイルやURLを開く
void	DonutOpenFile(const CString &strFileOrURL);
void	DonutOpenFile(const CString &strFileOrURL, DWORD dwOpenFlag);


inline bool 	DonutBrowserCanSetFocus(HWND hWnd)
{
	CWindow wnd(hWnd);
  #if 1 //+++
	return ::SendMessage(wnd.GetTopLevelParent(), WM_USER_BROWSER_CAN_SETFOCUS, 0L, 0L) != 0;
  #else
	BOOL	b = (BOOL) ::SendMessage(wnd.GetTopLevelParent(), WM_USER_BROWSER_CAN_SETFOCUS, 0L, 0L);
	return b == TRUE ? true : false;
  #endif
}


inline void 	DonutToggleOpenFlag(DWORD &dwFlag)
{
	if (::GetAsyncKeyState(VK_CONTROL) < 0 || ::GetAsyncKeyState(VK_SHIFT) < 0) {
		if ( _check_flag(D_OPENFILE_NOCREATE, dwFlag) )
			dwFlag &= ~D_OPENFILE_NOCREATE;
		else
			dwFlag |= D_OPENFILE_NOCREATE;
	}
}

/// オプションで指定された標準のオープン方法を返す
inline DWORD	DonutGetStdOpenFlag()
{
	DWORD	dwFlag = 0;
	if ( !_check_flag(MAIN_EX_NEWWINDOW, CMainOption::s_dwMainExtendedStyle) )
		dwFlag |= D_OPENFILE_NOCREATE;
	if ( !_check_flag(MAIN_EX_NOACTIVATE, CMainOption::s_dwMainExtendedStyle) )
		dwFlag |= D_OPENFILE_ACTIVATE;

	DonutToggleOpenFlag(dwFlag);
	return dwFlag;
}


// UDT DGSTR
inline DWORD	DonutGetStdOpenCreateFlag()
{
	DWORD	dwFlag = 0;

	if ( !_check_flag(MAIN_EX_NOACTIVATE, CMainOption::s_dwMainExtendedStyle) )
		dwFlag |= D_OPENFILE_ACTIVATE;

	return dwFlag;
}
//ENDE


#if 0	//+++ 未使用.
inline DWORD DonutGetStdOpenFlagNoToggle()
{
	DWORD	dwFlag = 0;

	if ( !_check_flag(MAIN_EX_NEWWINDOW, CMainOption::s_dwMainExtendedStyle) )
		dwFlag |= D_OPENFILE_NOCREATE;

	if ( !_check_flag(MAIN_EX_NOACTIVATE, CMainOption::s_dwMainExtendedStyle) )
		dwFlag |= D_OPENFILE_ACTIVATE;

	return dwFlag;
}
#endif


inline DWORD DonutGetStdOpenActivateFlag()
{
	DWORD	dwFlag = 0;

	if ( !_check_flag(MAIN_EX_NOACTIVATE, CMainOption::s_dwMainExtendedStyle) )
		dwFlag |= D_OPENFILE_ACTIVATE;

	return dwFlag;
}


#if 0	//+++ 未使用
inline DWORD DonutGetDLContrlFlags(HWND hWnd, bool bFromView)
{
	return (DWORD) ::SendMessage(hWnd, WM_USER_GET_DLCONTROL_FLAGS, (bFromView == true) ? TRUE : FALSE, 0L);
}
#endif


//#ifdef _DEBUG
#define DEBUG_CHECK_FOCUS_COMMAND_ID_HANDLER_EX()																 \
	if ( uMsg == WM_COMMAND && ID_DEBUG_CHECK_FOCUS == LOWORD(wParam) ) {										 \
		SetMsgHandled(TRUE);																					 \
		MessageBox(MtlGetWindowClassName( ::GetFocus() ), _T("Focus Checker"), MB_OK);							 \
		ATLTRACE( _T("Donut::Current Focus = (%x,%s)\n"), ::GetFocus(), MtlGetWindowClassName( ::GetFocus() ) ); \
		lResult = 0;																							 \
		if ( IsMsgHandled() )																					 \
			return TRUE;																						 \
	}


//#else
//#define	DEBUG_CHECK_FOCUS_COMMAND_ID_HANDLER_EX()
//#endif


///+++ 現在のアクティブ頁で選択中のテキストを返す.
///+++ ※ CSearchBar向けに用意. 本来は g_pMainWnd-> の同名関数を呼べばいいだけだが、
///+++	  include の依存関係が面倒なので...
extern CString Donut_GetActiveSelectedText();

///+++ 手抜きで現在のアクティブ頁でのステータス文字列を返す.
///+++ ※ カスタムメニュー側で、メニューが開かれたときにある文字列を取得するため.
extern CString Donut_GetActiveStatusStr();

///+++ CDonutExplorerBar::GetInstance()->RefreshExpBar(0); をするだけ.
void  Donut_ExplorerBar_RefreshFavoriteBar();

void	CommandLineArg(CMainFrame& wndMain, LPTSTR lpstrCmdLine);

void 	_PrivateTerm();


