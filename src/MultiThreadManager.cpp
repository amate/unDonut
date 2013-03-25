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

namespace {

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

int	RunChildFrameMessageLoop(const NewChildFrameData& NewChildData)
{
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	::OleInitialize(NULL);

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	// ChildFrameウィンドウ作成
	CChildFrame* pChild = new CChildFrame;
	int	nThreadRefCount = 0;
	CWindow wnd = pChild->CreateChildFrame(NewChildData, &nThreadRefCount);

	DWORD dwOption = 0;
	if (NewChildData.bActive)
		dwOption |= TAB_ACTIVE;
	if (NewChildData.bLink)
		dwOption |= TAB_LINK;
	CWindow wndMainFrame = wnd.GetTopLevelWindow();
	wndMainFrame.SendMessage(WM_TABCREATE, (WPARAM)wnd.m_hWnd, (LPARAM)dwOption);

	//if (NewChildData.strURL.GetLength() > 0)
	//	pData->pChild->Navigate2(NewChildData.strURL);

	CThreadRefManager threadRefManager(&nThreadRefCount);
	theLoop.AddMessageFilter(&threadRefManager);

	int nRet = theLoop.Run();

	theLoop.RemoveMessageFilter(&threadRefManager);

	_Module.RemoveMessageLoop();

	::CoUninitialize();
	::OleUninitialize();

	return nRet;
}

}	// namespace


//////////////////////////////////////////
// CMainProcessChildFrameThreadManager : マルチスレッド用の子スレッド管理クラス

class CMainProcessChildFrameThreadManager
{
public:
	DWORD	CreateChildFrameThread(const NewChildFrameData& data)
	{
		DWORD dwThreadID;
		HANDLE hThread = ::CreateThread(NULL, 0, RunChildFrameThread, (LPVOID)new NewChildFrameData(data), 0, &dwThreadID);
		if(hThread == NULL) {
			::MessageBox(NULL, _T("エラー: スレッドを作成できません!!!"), _T("Multi"), MB_OK);
			return 0;
		}
		::CloseHandle(hThread);

		return dwThreadID;
	}

private:
	// スレッドプロシージャー
	static DWORD WINAPI RunChildFrameThread(LPVOID lpData)
	{
		NewChildFrameData* pData = (NewChildFrameData*)lpData;
		int nRet = RunChildFrameMessageLoop(*pData);
		delete pData;

		return nRet;
	}

} g_main_process_childframe_thread_manager;


////////////////////////////////////////////////////////////////////
/// CChildProcessThreadManager : マルチプロセス用の子プロセスのスレッドを管理する

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
		int nRet = RunChildFrameMessageLoop(*pNewChildData);
		delete pNewChildData;

		return nRet;
	}

	// Data members
	DWORD m_dwChildProcessThreadId;
	DWORD m_dwCount;
	HANDLE m_arrThreadHandles[MAXIMUM_WAIT_OBJECTS - 1];

} g_child_process_thread_manager;


////////////////////////////////////////////////////////////////////
// namespace MultiThreadManager

namespace MultiThreadManager {

/// メインフレーム用のメッセージループ
int		RunMainFrameMessageLoop(LPTSTR lpstrCmdLine, int nCmdShow, bool bTray)
{
	CHandle hJob;
	LPCTSTR kMainFrameJobName = _T("DonutMainFrameJobObject");
	hJob.Attach(::CreateJobObject(NULL, kMainFrameJobName));
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedLimit = { 0 };
	extendedLimit.BasicLimitInformation.LimitFlags	= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	::SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &extendedLimit, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));

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
		wndMain.StartupMainFrameStyle(nCmdShow, bTray);

		_Module.Lock();

		CStartUpOption::StartUp(wndMain);

		//wndMain.SetAutoBackUp();		//自動更新するなら、開始.

		// 実際のメインループ.
		nRet = theLoop.Run();

		_Module.RemoveMessageLoop();
	}

	_Module.RevokeClassObjects();
	::Sleep(_Module.m_dwPause);

	return nRet;
}

	
/// マルチプロセスモードでの子プロセスメインループ
/// コマンドライン引数を見て自分が子プロセスとして起動されていたならループに入る
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

	CWindow wndMainFrame = CWindow(NewChildData.hWndParent).GetTopLevelWindow();

	// 強制的にメッセージキューを作らせる
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	g_child_process_thread_manager.init();
	g_child_process_thread_manager.AddFirstChildThread(NewChildData);

	g_child_process_thread_manager.ObserveChildProcessThread();

	/// ファイルのDL中ならプロセスを終わらせない
	for (;;) {
		bool bFound = false;
		MtlForEachTopLevelWindow(_T("#32770"), NULL, [&bFound](HWND hWnd) -> bool {
			if ( MtlIsWindowCurrentProcess(hWnd) ) {
				CString strCaption = MtlGetWindowText(hWnd);
				if ( (strCaption.Find( _T('%') ) != -1 && strCaption.Find( _T("完了しました") ) != -1)
					|| strCaption.Find( _T("ファイルのダウンロード") ) != -1 )
				{
					bFound = true;
					return false;
				}
			}

			return true; // continue finding
		});
		if (bFound == false)
			break;	// 終了してもおｋ
		
		::Sleep(60 * 1000);	// 1分ごとに調べる
	}
	//::CoUninitialize();
	//::OleUninitialize();

	_Module.Term();

	::PostMessage(wndMainFrame, WM_ADDREMOVECHILDPROCESSID, ::GetCurrentProcessId(), false);

	TRACEIN(_T("RunChildProcessMessageLoop() 終了..."));

	return true;
}

//===================================================================================

/// マルチスレッド/プロセスモードでの子スレッド作成
void	CreateChildFrameThread(NewChildFrameData& data, bool bMultiProcessMode)
{
	if (bMultiProcessMode) {
		g_child_process_thread_manager.AddChildThread(data);
	} else {
		g_main_process_childframe_thread_manager.CreateChildFrameThread(data);
	}
}

/// マルチプロセスモードで子プロセスの作成
void	CreateChildProcess(NewChildFrameData& data)
{
	CSharedMemoryHandle sharedMem;
	sharedMem.Serialize(data, nullptr, true);
	CString commandline;
	commandline.Format(_T("-NewProcessSharedMemoryData=%d"), sharedMem.Handle());

	/* 子プロセス作成 */
	STARTUPINFO	startupInfo = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION	processInfo = { 0 };
	ATLVERIFY(::CreateProcess(Misc::GetExeFileName(), commandline.GetBuffer(0), NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo));

	CWindow(data.hWndParent).GetTopLevelWindow().PostMessage(WM_ADDREMOVECHILDPROCESSID, processInfo.dwProcessId, true);

	::CloseHandle(processInfo.hProcess);
	::CloseHandle(processInfo.hThread);
}


};	// namespace MultiThreadManager


