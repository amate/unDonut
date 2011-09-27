/**
*	@file	MultiThreadManager.cpp
*	@brief	CChildFrame用のスレッド作成管理
*/

#include "stdafx.h"
#include "MultiThreadManager.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "Donut.h"

//スレッド初期化パラメーター
struct _RunMainData
{
	LPTSTR lpstrCmdLine;
	int nCmdShow;
	bool bTray;
};

struct _RunChildFrameData
{
	CChildFrame* pChild;
	NewChildFrameData	ConstructData;

	_RunChildFrameData(HWND hWndParent) : ConstructData(hWndParent)
	{	}
};


//////////////////////////////////////////
// CMultiThreadManager

class CMultiThreadManager
{
public:
	// Constructor
	CMultiThreadManager();

	int Run(LPTSTR lpstrCmdLine, int nCmdShow, bool bTray);
	DWORD	AddChildFrameThread(CChildFrame* pChild, NewChildFrameData* pData);

private:
	// スレッドプロシージャー
	static DWORD WINAPI RunMainThread(LPVOID lpData);
	static DWORD WINAPI RunChildFrameThread(LPVOID lpData);

	// Operations
	DWORD	AddMainThread(LPTSTR lpstrCmdLine, int nCmdShow, bool bTray);
	
	void	RemoveThread(DWORD dwIndex);

	// Data members
	DWORD m_dwCount;
	HANDLE m_arrThreadHandles[MAXIMUM_WAIT_OBJECTS - 1];

} g_MultiThreadManager;

//static CMultiThreadManager	g_MultiThreadManager;

// Constructor
CMultiThreadManager::CMultiThreadManager() : m_dwCount(0)
{ }


/// メインフレーム用のスレッド起動とスレッドの削除を管理する
int CMultiThreadManager::Run(LPTSTR lpstrCmdLine, int nCmdShow, bool bTray)
{
	MSG msg;
	// 強制的にメッセージキューを作らせる
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	AddMainThread(lpstrCmdLine, nCmdShow, bTray);

	int nRet = m_dwCount;
	DWORD dwRet;
	while(m_dwCount > 0)
	{
		dwRet = ::MsgWaitForMultipleObjects(m_dwCount, m_arrThreadHandles, FALSE, INFINITE, QS_ALLINPUT);

		if(dwRet == 0xFFFFFFFF)
		{
			::MessageBox(NULL, _T("エラー: オブジェクトのイベント待ち受けに失敗しました！"), _T("Multi"), MB_OK);
		}
		else if(dwRet >= WAIT_OBJECT_0 && dwRet <= (WAIT_OBJECT_0 + m_dwCount - 1))
		{
			RemoveThread(dwRet - WAIT_OBJECT_0);
		}
		else if(dwRet == (WAIT_OBJECT_0 + m_dwCount))
		{
			if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				//if(msg.message == WM_USER)
					//AddThread(_T(""), SW_SHOWNORMAL);
			}
		}
		else
		{
			::MessageBeep((UINT)-1);
		}
	}

	return nRet;
}

//スレッドプロシージャー

DWORD WINAPI CMultiThreadManager::RunMainThread(LPVOID lpData)
{
	_RunMainData* pData = (_RunMainData*)lpData;
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	::OleInitialize(NULL);

	_Module.StartMonitor();
	HRESULT hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED);
	ATLASSERT( SUCCEEDED(hRes) );
	hRes = ::CoResumeClassObjects();
	ATLASSERT( SUCCEEDED(hRes) );
	int nRet = 0;
	{
		CMessageLoop theLoop;
		_Module.AddMessageLoop(&theLoop);

		CMainFrame	 wndMain;
		if (wndMain.CreateEx() == NULL) {
			ATLTRACE( _T("Main window creation failed!\n") );
			return 0;
		}

		// load windowplacement
		wndMain.startupMainFrameStayle(pData->nCmdShow, pData->bTray);

		_Module.Lock();

		if (pData->lpstrCmdLine == NULL || pData->lpstrCmdLine[0] == 0) {	// no command line param
			CStartUpOption::StartUp(wndMain);
		} else {
			if (CStartUpOption::s_dwParam) {
				CStartUpOption::StartUp(wndMain);
			}
			CommandLineArg(wndMain, pData->lpstrCmdLine);
		}
		g_pMainWnd->SetAutoBackUp();		//自動更新するなら、開始.

		delete pData;
		// 実際のメインループ.
		nRet = theLoop.Run();

		_Module.RemoveMessageLoop();
	}
	_Module.RevokeClassObjects();
	::Sleep(_Module.m_dwPause);

	::CoUninitialize();
	::OleUninitialize();

	return nRet;	// ※ WTLのメイン窓クローズ時に1を正常値として返しているので注意.
}

DWORD WINAPI CMultiThreadManager::RunChildFrameThread(LPVOID lpData)
{
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	::OleInitialize(NULL);

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	_RunChildFrameData* pData = (_RunChildFrameData*)lpData;

	int	nThreadRefCount = 0;
	pData->pChild->SetThreadRefCount(&nThreadRefCount);

	CWindow wnd = pData->pChild->CreateEx(pData->ConstructData.hWndParent);
	pData->pChild->Navigate2(pData->ConstructData.strURL);

	DWORD dwOption = 0;
	if (pData->ConstructData.bActive)
		dwOption |= TAB_ACTIVE;
	if (pData->ConstructData.bLink)
		dwOption |= TAB_LINK;
	wnd.GetTopLevelWindow().SendMessage(WM_TABCREATE, (WPARAM)wnd.m_hWnd, (LPARAM)dwOption);
	
	if (pData->ConstructData.funcCallAfterCreated)
		pData->ConstructData.funcCallAfterCreated(pData->pChild);
	delete pData;

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();

	::CoUninitialize();
	::OleUninitialize();

	return nRet;
}


// Operations


/// メインフレームのスレッドを作成
DWORD CMultiThreadManager::AddMainThread(LPTSTR lpstrCmdLine, int nCmdShow, bool bTray)
{
	if (m_dwCount == (MAXIMUM_WAIT_OBJECTS - 1)) {
		::MessageBox(NULL, _T("エラー: これ以上スレッドを作成できません!!!"), _T("Multi"), MB_OK);
		return 0;
	}

	_RunMainData* pData = new _RunMainData;
	pData->lpstrCmdLine = lpstrCmdLine;
	pData->nCmdShow = nCmdShow;
	pData->bTray = bTray;

	DWORD dwThreadID;
	HANDLE hThread = ::CreateThread(NULL, 0, RunMainThread, pData, 0, &dwThreadID);
	if (hThread == NULL) {
		::MessageBox(NULL, _T("エラー: スレッドを作成できません!!!"), _T("Multi"), MB_OK);
		return 0;
	}

	m_arrThreadHandles[m_dwCount] = hThread;
	m_dwCount++;
	return dwThreadID;
}

DWORD CMultiThreadManager::AddChildFrameThread(CChildFrame* pChild, NewChildFrameData* pChildFrameData)
{
	if (m_dwCount == (MAXIMUM_WAIT_OBJECTS - 1)) {
		::MessageBox(NULL, _T("エラー: これ以上スレッドを作成できません!!!"), _T("Multi"), MB_OK);
		return 0;
	}

	_RunChildFrameData* pData = new _RunChildFrameData(pChildFrameData->hWndParent);
	pData->pChild = pChild;
	pData->ConstructData	= *pChildFrameData;

	DWORD dwThreadID;
	HANDLE hThread = ::CreateThread(NULL, 0, RunChildFrameThread, pData, 0, &dwThreadID);
	if(hThread == NULL) {
		::MessageBox(NULL, _T("エラー: スレッドを作成できません!!!"), _T("Multi"), MB_OK);
		return 0;
	}

	m_arrThreadHandles[m_dwCount] = hThread;
	m_dwCount++;
	return dwThreadID;
}

void CMultiThreadManager::RemoveThread(DWORD dwIndex)
{
	::CloseHandle(m_arrThreadHandles[dwIndex]);
	if(dwIndex != (m_dwCount - 1))
		m_arrThreadHandles[dwIndex] = m_arrThreadHandles[m_dwCount - 1];
	m_dwCount--;
}




////////////////////////////////////////////////////////////////////
// namespace MultiThreadManager

namespace MultiThreadManager {

int Run(LPTSTR lpstrCmdLine, int nCmdShow, bool bTray)
{
	return g_MultiThreadManager.Run(lpstrCmdLine, nCmdShow, bTray);
}

void	ExecuteChildFrameThread(CChildFrame* pChild, NewChildFrameData* pData)
{
	g_MultiThreadManager.AddChildFrameThread(pChild, pData);
}


};	// namespace MultiThreadManager


