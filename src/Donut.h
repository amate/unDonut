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


//==============================================================================
/// Donut.cppに実装してある関数

void	PerseUrls(LPCTSTR lpszCommandline, std::vector<CString>& vecUrls);

void	CommandLineArg(CMainFrame& wndMain, LPTSTR lpstrCmdLine);

void 	_PrivateTerm();


