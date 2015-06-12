/**
*	@file	MultiThreadManager.cpp
*	@brief	CChildFrame�p�̃X���b�h�쐬�Ǘ�
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
#include "option\StartUpFinishOption.h"
#include "GlobalConfig.h"

#include "option\SupressPopupOption.h"
#include "option\RightClickMenuDialog.h"
#include "option\KeyBoardDialog.h"
#include "option\UrlSecurityOption.h"
#include "AutoLogin.h"

namespace {

//�X���b�h�������p�����[�^�[
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
				TRACEIN(_T("ChildFreame�X���b�h�̔j��"));
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

	// ChildFrame�E�B���h�E�쐬
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
	pChild->InitChildFrame(NewChildData);

	CThreadRefManager threadRefManager(&nThreadRefCount);
	theLoop.AddMessageFilter(&threadRefManager);

	int nRet = theLoop.Run();

	theLoop.RemoveMessageFilter(&threadRefManager);

	_Module.RemoveMessageLoop();

	::OleUninitialize();
	::CoUninitialize();

	return nRet;
}

}	// namespace


//////////////////////////////////////////
// CMainProcessChildFrameThreadManager : �}���`�X���b�h�p�̎q�X���b�h�Ǘ��N���X

class CMainProcessChildFrameThreadManager
{
public:
	DWORD	CreateChildFrameThread(const NewChildFrameData& data)
	{
		DWORD dwThreadID;
		HANDLE hThread = ::CreateThread(NULL, 0, RunChildFrameThread, (LPVOID)new NewChildFrameData(data), 0, &dwThreadID);
		if(hThread == NULL) {
			::MessageBox(NULL, _T("�G���[: �X���b�h���쐬�ł��܂���!!!"), _T("Multi"), MB_OK);
			return 0;
		}
		::CloseHandle(hThread);

		return dwThreadID;
	}

private:
	// �X���b�h�v���V�[�W���[
	static DWORD WINAPI RunChildFrameThread(LPVOID lpData)
	{
		NewChildFrameData* pData = (NewChildFrameData*)lpData;
		int nRet = RunChildFrameMessageLoop(*pData);
		delete pData;

		return nRet;
	}

} g_main_process_childframe_thread_manager;


////////////////////////////////////////////////////////////////////
/// CChildProcessThreadManager : �}���`�v���Z�X�p�̎q�v���Z�X�̃X���b�h���Ǘ�����

class CChildProcessThreadManager
{
public:
	enum { WM_NEWCHILDFRAMETHREAD = WM_APP + 300 };

	CChildProcessThreadManager() : m_dwChildProcessThreadId(0)
	{	}

	void	init() { m_dwChildProcessThreadId = ::GetCurrentThreadId(); }

	/// �X���b�h�̒ǉ��̑S�X���b�h�̏I���ҋ@������
	void	ObserveChildProcessThread()
	{
		MSG msg;
		DWORD dwRet;
		while (m_dwCount > 0) {
			dwRet = ::MsgWaitForMultipleObjects(m_dwCount, m_arrThreadHandles, FALSE, INFINITE, QS_ALLINPUT);

			if (dwRet == 0xFFFFFFFF) {
				::MessageBox(NULL, _T("�G���[: �I�u�W�F�N�g�̃C�x���g�҂��󂯂Ɏ��s���܂����I"), _T("Multi"), MB_OK);

			} else if(dwRet >= WAIT_OBJECT_0 && dwRet <= (WAIT_OBJECT_0 + m_dwCount - 1)) {
				// �X���b�h���I�������̂ő҂��z�񂩂�폜����
				_RemoveThread(dwRet - WAIT_OBJECT_0);

			} else if(dwRet == (WAIT_OBJECT_0 + m_dwCount)) {
				if(::PeekMessage(&msg, (HWND)-1, 0, 0, PM_REMOVE)) {
					if (msg.message == WM_NEWCHILDFRAMETHREAD) {
						_AddThread((NewChildFrameData*)msg.wParam);
					} else if (msg.message == WM_NOTIFYOBSERVERFROMMAINFRAME) {
						CSharedDataChangeSubject::NotifyFromMainFrame((ObserverClass)msg.wParam, (HWND)msg.lParam);
					}
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
			MessageBox(NULL, _T("����ȏ�X���b�h���쐬�ł��܂���"), _T("�G���["), MB_ICONERROR);
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

static HANDLE g_hJob = NULL;

/// ���C���t���[���p�̃��b�Z�[�W���[�v
int		RunMainFrameMessageLoop(LPTSTR lpstrCmdLine, int nCmdShow, bool bTray)
{
	ATLVERIFY(g_hJob = ::CreateJobObject(NULL, NULL));
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedLimit = { 0 };
	extendedLimit.BasicLimitInformation.LimitFlags	= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	::SetInformationJobObject(g_hJob, JobObjectExtendedLimitInformation, &extendedLimit, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));

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

		//wndMain.SetAutoBackUp();		//�����X�V����Ȃ�A�J�n.

		// ���ۂ̃��C�����[�v.
		nRet = theLoop.Run();

		_Module.RemoveMessageLoop();
	}

	_Module.RevokeClassObjects();
	::Sleep(_Module.m_dwPause);

	::CloseHandle(g_hJob);
	g_hJob = NULL;

	return nRet;
}


void	AddObserverAndLoadConfig(HWND hWndMainFrame)
{
	CSharedDataChangeSubject::AddObserver(ObserverClass::kSupressPopupOption, std::bind(&CSupressPopupOption::UpdateSupressPopupData, std::placeholders::_1));
	CSupressPopupOption::UpdateSupressPopupData(hWndMainFrame);

	CSharedDataChangeSubject::AddObserver(ObserverClass::kLoginDataManager, std::bind(&CLoginDataManager::UpdateLoginDataList, std::placeholders::_1));
	CLoginDataManager::UpdateLoginDataList(hWndMainFrame);

	CSharedDataChangeSubject::AddObserver(ObserverClass::kCustomContextMenuOption, std::bind(&CCustomContextMenuOption::ReloadCustomContextMenuList, std::placeholders::_1));
	CCustomContextMenuOption::ReloadCustomContextMenuList(hWndMainFrame);

	CSharedDataChangeSubject::AddObserver(ObserverClass::kUrlSecurityOption, std::bind(&CUrlSecurityOption::UpdateUrlSecurityList, std::placeholders::_1));
	CUrlSecurityOption::UpdateUrlSecurityList(hWndMainFrame);

	CSharedDataChangeSubject::AddObserver(ObserverClass::kAcceleratorOption, std::bind(&CAcceleratorOption::ReloadAccelerator, std::placeholders::_1));
	CAcceleratorOption::ReloadAccelerator(hWndMainFrame);
}


/// �}���`�v���Z�X���[�h�ł̎q�v���Z�X���C�����[�v
/// �R�}���h���C�����������Ď������q�v���Z�X�Ƃ��ċN������Ă����Ȃ烋�[�v�ɓ���
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
		CString strError = _T("RunChildProcessMessageLoop : ���L�������̃I�[�v���Ɏ��s\n");
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

	// ActiveX�R���g���[�����z�X�g���邽�߂̏���
	AtlAxWinInit();

	CWindow wndMainFrame = CWindow(NewChildData.hWndParent).GetTopLevelWindow();

	AddObserverAndLoadConfig(wndMainFrame);

	// �����I�Ƀ��b�Z�[�W�L���[����点��
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	g_child_process_thread_manager.init();
	g_child_process_thread_manager.AddFirstChildThread(NewChildData);

	g_child_process_thread_manager.ObserveChildProcessThread();

	/// �t�@�C����DL���Ȃ�v���Z�X���I��点�Ȃ�
	for (;;) {
		bool bFound = false;
		MtlForEachTopLevelWindow(_T("#32770"), NULL, [&bFound](HWND hWnd) -> bool {
			if ( MtlIsWindowCurrentProcess(hWnd) ) {
				CString strCaption = MtlGetWindowText(hWnd);
				if ( (strCaption.Find( _T('%') ) != -1 && strCaption.Find( _T("�������܂���") ) != -1)
					|| strCaption.Find( _T("�t�@�C���̃_�E�����[�h") ) != -1 )
				{
					bFound = true;
					return false;
				}
			}

			return true; // continue finding
		});
		if (bFound == false)
			break;	// �I�����Ă�����
		
		::Sleep(60 * 1000);	// 1�����Ƃɒ��ׂ�
	}
	//::CoUninitialize();
	//::OleUninitialize();

	_Module.Term();

	::PostMessage(wndMainFrame, WM_REMOVECHILDPROCESSID, ::GetCurrentProcessId(), 0);

	TRACEIN(_T("RunChildProcessMessageLoop() �I��..."));

	return true;
}

// �O���[�o���ϐ�
std::vector<ChildProcessProcessThreadId>	g_vecChildProcessProcessThreadId;

//===================================================================================

/// �}���`�X���b�h/�v���Z�X���[�h�ł̎q�X���b�h�쐬
void	CreateChildFrameThread(NewChildFrameData& data, bool bMultiProcessMode)
{
	if (bMultiProcessMode) {
		g_child_process_thread_manager.AddChildThread(data);
	} else {
		g_main_process_childframe_thread_manager.CreateChildFrameThread(data);
	}
}

/// �}���`�v���Z�X���[�h�Ŏq�v���Z�X�̍쐬
void	CreateChildProcess(NewChildFrameData& data)
{
	CSharedMemoryHandle sharedMem;
	sharedMem.Serialize(data, nullptr, true);
	CString commandline;
	commandline.Format(_T("-NewProcessSharedMemoryData=%d"), sharedMem.Handle());

	/* �q�v���Z�X�쐬 */
	STARTUPINFO	startupInfo = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION	processInfo = { 0 };
	ATLVERIFY(::CreateProcess(Misc::GetExeFileName(), commandline.GetBuffer(0), NULL, NULL, TRUE, CREATE_BREAKAWAY_FROM_JOB, NULL, NULL, &startupInfo, &processInfo));

	g_vecChildProcessProcessThreadId.emplace_back(processInfo.dwProcessId, processInfo.dwThreadId);

	ATLVERIFY(::AssignProcessToJobObject(g_hJob, processInfo.hProcess));

	::CloseHandle(processInfo.hProcess);
	::CloseHandle(processInfo.hThread);
}


};	// namespace MultiThreadManager


