/**
 *	@file FileNotification.h
 *	@brief	ファイルの更新の監視. (リンクバー専用?)
 */

#pragma once

//#include <boost/thread.hpp>


class CFileNotification 
{
public:
	// Declarations
	DECLARE_REGISTERED_MESSAGE( Mtl_FileNotification )

private:
	struct _ThreadParam {
		HWND	_hWnd;
		HANDLE	_hExitEvent;
		HANDLE	_hNotification;
		CString	strPath;

		_ThreadParam() : _hWnd(0), _hExitEvent(0), _hNotification(0) {} 	//+++
	};

public:
	// Ctor/Dtor
	CFileNotification()
		: m_hNotificationThread( NULL )
		, m_uNotificationThreadID( 0 )
		, m_ThreadParams()					//+++ 抜けチェック対策.
	{
	}


	~CFileNotification()
	{
		if (m_hNotificationThread != NULL)
			_CleanUpNotification();
	}


private:
	// Data members
	HANDLE			m_hNotificationThread;
	UINT			m_uNotificationThreadID;
	_ThreadParam	m_ThreadParams;

public:
	bool SetUpFileNotificationThread(HWND hWnd, const CString &strDirPath, bool bWatchSubTree = false)
	{
		/* イベントを作成 */
		m_ThreadParams._hExitEvent	  = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		ATLASSERT(m_ThreadParams._hExitEvent != INVALID_HANDLE_VALUE);

		/* 通知先のウィンドウハンドルを設定 */
		ATLASSERT( ::IsWindow(hWnd) );
		m_ThreadParams._hWnd		  = hWnd;

		CString 		strPath = strDirPath;
		//MtlRemoveTrailingBackSlash(strPath);
		m_ThreadParams.strPath = strPath;

		HANDLE	hWait = ::FindFirstChangeNotification(  strPath,
														bWatchSubTree,													// flag for monitoring
														FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);	// fixed by INUYA, thank you.

		// お気に入りのリンクフォルダが見つからなかったらfalseを返す
		if (hWait == INVALID_HANDLE_VALUE || hWait == NULL) 				// can't find the Link directory
			return false;

		m_ThreadParams._hNotification = hWait;
		//boost::thread	thread_Notification( boost::bind(&CFileNotification::_FileNotificationThread, this, (LPVOID)&m_ThreadParams) );

		//m_hNotificationThread	= ::CreateThread(NULL, 0, _FileNotificationThread, (LPVOID) &m_ThreadParams, 0, &m_dwNotificationThreadID);
		//ATLASSERT(m_hNotificationThread != INVALID_HANDLE_VALUE);

		m_hNotificationThread	= (HANDLE)::_beginthreadex(NULL, 0, _FileNotificationThread, (LPVOID) &m_ThreadParams, 0, &m_uNotificationThreadID);
		ATLASSERT(m_hNotificationThread);

		return true;
	}

private:
	void _CleanUpNotification()
	{
		MTLVERIFY( ::SetEvent(m_ThreadParams._hExitEvent) );

		DWORD dwResult = ::WaitForSingleObject(m_hNotificationThread, DONUT_THREADWAIT);

		if (dwResult == WAIT_OBJECT_0) {
			// wait the thread over
		}

		::FindCloseChangeNotification(m_ThreadParams._hNotification);
		::CloseHandle(m_ThreadParams._hExitEvent);
		::CloseHandle(m_hNotificationThread);								// fixed by DGSTR, thanks!
	}


	static UINT WINAPI _FileNotificationThread(LPVOID lpParam)
	{
		_ThreadParam*	pParam	= (_ThreadParam *) lpParam;

		ATLASSERT( ::IsWindow(pParam->_hWnd) );

		HANDLE			handles[] = { pParam->_hExitEvent, pParam->_hNotification };

		for (;;) {
			DWORD dwResult = ::WaitForMultipleObjects(2, handles, FALSE, INFINITE);

			if (dwResult == WAIT_OBJECT_0) {								// killevent
				break;														// thread must be ended
			} else if (dwResult == WAIT_OBJECT_0 + 1) { 					//notification
				::SendMessage(pParam->_hWnd, GET_REGISTERED_MESSAGE(Mtl_FileNotification), (WPARAM)(LPCTSTR)pParam->strPath, 0);
				::FindNextChangeNotification(pParam->_hNotification);
			} else if (dwResult == WAIT_FAILED) {
				ATLASSERT(FALSE);
				break;
			} else {
				ATLASSERT(FALSE);
				break;
			}
		}
		_endthreadex(0);
		return 0;
	}
};
