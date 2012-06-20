/**
 *	@file	Donut.h
 *	@brief	Donut固有のWindowメッセージ定義＋それようのサブルーチン.
 */

#pragma once

#include "option/MainOption.h"
#include  "DonutDefine.h"

//-------------------------------------------------------------------------------------


inline IWebBrowser2 *DonutGetIWebBrowser2(HWND hWnd)
{
	return (IWebBrowser2 *) ::SendMessage(hWnd, WM_USER_GET_IWEBBROWSER, 0, 0);
}


/// DonutでファイルやURLを開く
void	DonutOpenFile(const CString &strFileOrURL);
void	DonutOpenFile(const CString &strFileOrURL, DWORD dwOpenFlag);

/// ctrl か shift が押されていると タブの作成フラグをトグルする
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

/// 返されるフラグは タブ作成 & オプションで設定されていれば アクティブにするフラグを返す
inline DWORD DonutGetStdOpenActivateFlag()
{
	DWORD	dwFlag = 0;

	if ( !_check_flag(MAIN_EX_NOACTIVATE, CMainOption::s_dwMainExtendedStyle) )
		dwFlag |= D_OPENFILE_ACTIVATE;

	return dwFlag;
}


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
void	Donut_ExplorerBar_RefreshFavoriteBar();

void	PerseUrls(LPCTSTR lpszCommandline, std::vector<CString>& vecUrls);

void	CommandLineArg(CMainFrame& wndMain, LPTSTR lpstrCmdLine);

void 	_PrivateTerm();


