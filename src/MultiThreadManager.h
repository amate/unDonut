/**
*	@file	MultiThreadManager.h
*	@brief	CChildFrame用のスレッド作成管理
*/

#pragma once

#include <vector>

// 前方宣言
class CChildFrame;
struct NewChildFrameData;


namespace MultiThreadManager {

/// メインフレーム用のメッセージループ
int		RunMainFrameMessageLoop(LPTSTR lpstrCmdLine, int nCmdShow, bool bTray);

/// マルチプロセスモードでの子プロセスメインループ
/// コマンドライン引数を見て自分が子プロセスとして起動されていたならループに入る
bool	RunChildProcessMessageLoop(HINSTANCE hInstance);


// グローバル変数
struct ChildProcessProcessThreadId
{
	DWORD	dwProcessId;
	DWORD	dwThreadId;

	ChildProcessProcessThreadId(DWORD processId, DWORD threadId) : dwProcessId(processId), dwThreadId(threadId) { }
};
extern std::vector<ChildProcessProcessThreadId>	g_vecChildProcessProcessThreadId;

#define	WM_NOTIFYOBSERVERFROMMAINFRAME	(WM_APP + 400)

//===================================================================================

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

/// マルチスレッド/プロセスモードでの子スレッド作成
void	CreateChildFrameThread(NewChildFrameData& data, bool bMultiProcessMode);

/// マルチプロセスモードで子プロセスの作成
void	CreateChildProcess(NewChildFrameData& data);

};	// namespace MultiThreadManager





