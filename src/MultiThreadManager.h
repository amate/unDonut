/**
*	@file	MultiThreadManager.h
*	@brief	CChildFrame用のスレッド作成管理
*/

#pragma once

// 前方宣言
class CChildFrame;
struct NewChildFrameData;


namespace MultiThreadManager {

int Run(LPTSTR lpstrCmdLine, int nCmdShow, bool bTray);


#define NEWCHILDFRAMESHAREDMEMORYNAME	_T("DonutChildFrameSharedMemoryData")

struct NewChildFrameProcessData {
	HWND	hWndParent;
	WCHAR	url[INTERNET_MAX_URL_LENGTH];
	DWORD	dwDLCtrl;
	DWORD	dwExStyle;
	DWORD	dwAutoRefresh;
	bool	bActive;
	bool	bLink;

	// AutoHilight
	bool	bAutoHilight;
	WCHAR	searchWord[512];
};

/// マルチプロセスモードでの子プロセスメインループ
bool	RunChildProcessMessageLoop(HINSTANCE hInstance);

/// マルチプロセスモードでの子スレッド作成
void	AddChildThread(NewChildFrameData* pData);


/// マルチスレッドモードでの子スレッド作成
void	ExecuteChildFrameThread(CChildFrame* pChild, NewChildFrameData* pData);


};	// namespace MultiThreadManager





