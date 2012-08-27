/**
*	@file	MultiThreadManager.cpp
*	@brief	CChildFrame用のスレッド作成管理
*/

#include "stdafx.h"
#include "MultiThreadManager.h"
#include <boost\thread.hpp>
#include <boost\serialization\string.hpp>
#include <boost\serialization\vector.hpp>
#include <boost\serialization\utility.hpp>
#include "SharedMemoryUtil.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "Donut.h"
#include "option\StartUpOption.h"

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

#define MAINFRAMEJOBOBJECTNAME	_T("DonutMainFrameJobObject")


//////////////////////////////////////////
// CMultiThreadManager

class CMultiThreadManager
{
public:
	DWORD	AddChildFrameThread(CChildFrame* pChild, NewChildFrameData* pData);

private:
	// スレッドプロシージャー
	static DWORD WINAPI RunChildFrameThread(LPVOID lpData);

} g_MultiThreadManager;


/// ChildFrame のスレッド作成
DWORD CMultiThreadManager::AddChildFrameThread(CChildFrame* pChild, NewChildFrameData* pChildFrameData)
{
	_RunChildFrameData* pData = new _RunChildFrameData(pChildFrameData->hWndParent);
	pData->pChild = pChild;
	pData->ConstructData	= *pChildFrameData;

	DWORD dwThreadID;
	HANDLE hThread = ::CreateThread(NULL, 0, RunChildFrameThread, pData, 0, &dwThreadID);
	if(hThread == NULL) {
		::MessageBox(NULL, _T("エラー: スレッドを作成できません!!!"), _T("Multi"), MB_OK);
		return 0;
	}
	::CloseHandle(hThread);

	return dwThreadID;
}


//スレッドプロシージャー

/// ChildFrame のメッセージループの本体
DWORD WINAPI CMultiThreadManager::RunChildFrameThread(LPVOID lpData)
{
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	::OleInitialize(NULL);

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	_RunChildFrameData* pData = (_RunChildFrameData*)lpData;

	// ChildFrameウィンドウ作成
	CChildFrame* pChild = new CChildFrame;
	int	nThreadRefCount = 0;
	CWindow wnd = pData->pChild->CreateChildFrame(pData->ConstructData, &nThreadRefCount);

	const NewChildFrameData& NewChildData = pData->ConstructData;
	DWORD dwOption = 0;
	if (NewChildData.bActive)
		dwOption |= TAB_ACTIVE;
	if (NewChildData.bLink)
		dwOption |= TAB_LINK;
	CWindow wndMainFrame = wnd.GetTopLevelWindow();
	wndMainFrame.SendMessage(WM_TABCREATE, (WPARAM)wnd.m_hWnd, (LPARAM)dwOption);

	//if (NewChildData.strURL.GetLength() > 0)
	//	pData->pChild->Navigate2(NewChildData.strURL);

	class CThreadRefManager : public CMessageFilter
	{
	public:
		CThreadRefManager(int* pCount) : m_pThreadRefCount(pCount)
		{	}

		virtual BOOL PreTranslateMessage(MSG* pMsg)
		{
			switch (pMsg->message) {
			case WM_DECREMENTTHREADREFCOUNT:
				--(*m_pThreadRefCount);
				TRACEIN(_T("WM_DECREMENTTHREADREFCOUNT : %d"), *m_pThreadRefCount);
				if (*m_pThreadRefCount == 0) {
					TRACEIN(_T("ChildFreameスレッドの破棄"));
					PostQuitMessage(0);
				}
				return TRUE;
			}
			return FALSE;
		}
	private:
		int*	m_pThreadRefCount;
	};
	CThreadRefManager threadRefManager(&nThreadRefCount);
	theLoop.AddMessageFilter(&threadRefManager);

	int nRet = theLoop.Run();

	theLoop.RemoveMessageFilter(&threadRefManager);

	_Module.RemoveMessageLoop();

	::CoUninitialize();
	::OleUninitialize();

	return nRet;
}


////////////////////////////////////////////////////////////////////
/// 子プロセスのスレッドを管理する

class CChildProcessThreadManager
{
public:
	enum { WM_NEWCHILDFRAMETHREAD = WM_APP + 300 };

	CChildProcessThreadManager() : m_dwChildProcessThreadId(0)
	{	}

	void	init() { m_dwChildProcessThreadId = ::GetCurrentThreadId(); }

	/// スレッドの追加の全スレッドの終了待機をする
	void	ObserveChildProcessThread()
	{
		MSG msg;
		DWORD dwRet;
		while (m_dwCount > 0) {
			dwRet = ::MsgWaitForMultipleObjects(m_dwCount, m_arrThreadHandles, FALSE, INFINITE, QS_ALLINPUT);

			if (dwRet == 0xFFFFFFFF) {
				::MessageBox(NULL, _T("エラー: オブジェクトのイベント待ち受けに失敗しました！"), _T("Multi"), MB_OK);

			} else if(dwRet >= WAIT_OBJECT_0 && dwRet <= (WAIT_OBJECT_0 + m_dwCount - 1)) {
				// スレッドが終了したので待ち配列から削除する
				_RemoveThread(dwRet - WAIT_OBJECT_0);

			} else if(dwRet == (WAIT_OBJECT_0 + m_dwCount)) {
				if(::PeekMessage(&msg, (HWND)-1, 0, 0, PM_REMOVE)) {
					if(msg.message == WM_NEWCHILDFRAMETHREAD)
						_AddThread((NewChildFrameData*)msg.wParam);
				}
			} else {
				::MessageBeep((UINT)-1);
			}
		}
	}

	void	AddFirstChildThread(const NewChildFrameData& NewChildData)
	{
		_AddThread(new NewChildFrameData(NewChildData));
	}

	void	AddChildThread(const NewChildFrameData& NewChildData)
	{
		ATLASSERT( m_dwChildProcessThreadId );
		::PostThreadMessage(m_dwChildProcessThreadId, WM_NEWCHILDFRAMETHREAD, (WPARAM)new NewChildFrameData(NewChildData), 0);
	}

private:

	void _AddThread(NewChildFrameData* pData)
	{
		if (m_dwCount == (MAXIMUM_WAIT_OBJECTS - 1)) {
			MessageBox(NULL, _T("これ以上スレッドを作成できません"), _T("エラー"), MB_ICONERROR);
			return ;
		}
		TRACEIN(_T("_AddThread() m_dwCount(%d)"), m_dwCount);
		DWORD dwThreadID = 0;
		HANDLE hThread = ::CreateThread(NULL, 0, _ChildThreadMessageLoop, (LPVOID)pData, 0, &dwThreadID);
		m_arrThreadHandles[m_dwCount] = hThread;
		++m_dwCount;	
	}

	void _RemoveThread(DWORD dwIndex)
	{
		TRACEIN(_T("_RemoveThread(%d)"), dwIndex);
		::CloseHandle(m_arrThreadHandles[dwIndex]);
		if(dwIndex != (m_dwCount - 1))
			m_arrThreadHandles[dwIndex] = m_arrThreadHandles[m_dwCount - 1];
		m_dwCount--;
	}


	static DWORD WINAPI _ChildThreadMessageLoop(LPVOID pData)
	{
		NewChildFrameData* pNewChildData = (NewChildFrameData*)pData;
		NewChildFrameData& NewChildData = *pNewChildData;

		::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		::OleInitialize(NULL);

		CMessageLoop theLoop;
		_Module.AddMessageLoop(&theLoop);

		// ChildFrameウィンドウ作成
		CChildFrame* pChild = new CChildFrame;
		int	nThreadRefCount = 0;
		CWindow wnd = pChild->CreateChildFrame(NewChildData, &nThreadRefCount);

		// タブに追加
		DWORD dwOption = 0;
		if (NewChildData.bActive)
			dwOption |= TAB_ACTIVE;
		if (NewChildData.bLink)
			dwOption |= TAB_LINK;
		CWindow wndMainFrame = wnd.GetTopLevelWindow();
		wndMainFrame.SendMessage(WM_TABCREATE, (WPARAM)wnd.m_hWnd, (LPARAM)dwOption);
	
		delete pNewChildData;

		class CThreadRefManager : public CMessageFilter
		{
		public:
			CThreadRefManager(int* pCount) : m_pThreadRefCount(pCount)
			{	}

			virtual BOOL PreTranslateMessage(MSG* pMsg)
			{
				switch (pMsg->message) {
				case WM_DECREMENTTHREADREFCOUNT:
					--(*m_pThreadRefCount);
					TRACEIN(_T("WM_DECREMENTTHREADREFCOUNT : 残り %d"), *m_pThreadRefCount);
					if (*m_pThreadRefCount == 0) {
						TRACEIN(_T("ChildFreameスレッドの破棄"));
						PostQuitMessage(0);
					}
					return TRUE;
				}
				return FALSE;
			}
		private:
			int*	m_pThreadRefCount;
		};
		CThreadRefManager threadRefManager(&nThreadRefCount);
		theLoop.AddMessageFilter(&threadRefManager);

		int nRet = theLoop.Run();

		theLoop.RemoveMessageFilter(&threadRefManager);

		_Module.RemoveMessageLoop();

		::CoUninitialize();
		::OleUninitialize();
		return 0;
	}

	// Data members
	DWORD m_dwChildProcessThreadId;
	DWORD m_dwCount;
	HANDLE m_arrThreadHandles[MAXIMUM_WAIT_OBJECTS - 1];

} g_child_process_thread_manager;


////////////////////////////////////////////////////////////////////
// namespace MultiThreadManager

namespace MultiThreadManager {


bool	RunChildProcessMessageLoop(HINSTANCE hInstance)
{
	const LPCTSTR strNewProcessSharedMemoryData = _T("-NewProcessSharedMemoryData=");
	CString strCmd = ::GetCommandLine();
	int nIndex = strCmd.Find(strNewProcessSharedMemoryData);
	if (nIndex == -1)
		return false;

	CString strMapHandle = strCmd.Mid(nIndex + (int)::wcslen(strNewProcessSharedMemoryData));
	ATLVERIFY(!strMapHandle.IsEmpty());
#ifdef WIN64
	HANDLE hMap = (HANDLE)_wtoi64(strMapHandle);
#else
	HANDLE hMap = (HANDLE)_wtoi(strMapHandle);
#endif
	if (hMap == NULL) {
		CString strError = _T("RunChildProcessMessageLoop : 共有メモリのオープンに失敗\n");
		MessageBox(NULL, strError, NULL, MB_OK);
		return true;
	}

	NewChildFrameData	NewChildData(NULL);
	CSharedMemory sharedMem;
	sharedMem.Deserialize(NewChildData, hMap);
	sharedMem.CloseHandle();

	_Module.Init(NULL, hInstance);

	//::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	//::OleInitialize(NULL);

	// ActiveXコントロールをホストするための準備
	AtlAxWinInit();

	HANDLE hJob = ::OpenJobObject(JOB_OBJECT_ASSIGN_PROCESS, FALSE, MAINFRAMEJOBOBJECTNAME);
	AssignProcessToJobObject(hJob, GetCurrentProcess());
	::CloseHandle(hJob);

	// 強制的にメッセージキューを作らせる
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	g_child_process_thread_manager.init();
	g_child_process_thread_manager.AddFirstChildThread(NewChildData);

	g_child_process_thread_manager.ObserveChildProcessThread();

	//::CoUninitialize();
	//::OleUninitialize();

	_Module.Term();

	TRACEIN(_T("RunChildProcessMessageLoop() 終了..."));

	return true;
}

/// マルチプロセスモードでの子スレッド作成
void	AddChildThread(NewChildFrameData* pData)
{
	g_child_process_thread_manager.AddChildThread(*pData);
}


/// マルチスレッドモードでの子スレッド作成
void	ExecuteChildFrameThread(CChildFrame* pChild, NewChildFrameData* pData)
{
	g_MultiThreadManager.AddChildFrameThread(pChild, pData);
}


};	// namespace MultiThreadManager


