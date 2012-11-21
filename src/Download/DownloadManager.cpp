// DownloadManager.cpp

#include "stdafx.h"
#include "DownloadManager.h"
#include <boost/thread.hpp>
#include "CustomBindStatusCallBack.h"
#include "../option/DLControlOption.h"
#include "DownloadOptionDialog.h"

//////////////////////////////////////////////////////
// CDownloadManager

CDownloadManager*	CDownloadManager::s_pThis = NULL;
CString	CDownloadManager::s_strReferer;

// Constructor
CDownloadManager::CDownloadManager()
{
	WM_GETDEFAULTDLFOLDER	= ::RegisterWindowMessage(REGISTERMESSAGE_GETDEFAULTDLFOLDER);
	WM_STARTDOWNLOAD		= ::RegisterWindowMessage(REGISTERMESSAGE_STARTDOWNLOAD);
	WM_SETREFERER			= ::RegisterWindowMessage(REGISTERMESSAGE_SETREFERER);

	s_pThis = this;
}


void	CDownloadManager::SetParent(HWND hWnd)
{
	m_hWndParent = hWnd;
	m_wndDownload.SetParentWindow(hWnd);
}

//--------------------------------
/// DLManagerを使うかどうかを返す
bool CDownloadManager::UseDownloadManager()
{
	return CDLControlOption::s_bUseDLManager ? true : false;
}


//---------------------------------------
CCustomBindStatusCallBack*	CDownloadManager::CreateCustomBindStatusCallBack(HWND hWndMainFrame, LPCTSTR defaultDLFolder)
{
	uintptr_t unique = ::SendMessage(hWndMainFrame, WM_GETUNIQUENUMBERFORDLITEM, 0, 0);
	/* 共有メモリを作成 */
	CString sharedMemName;
	sharedMemName.Format(_T("%s%#x"), DLITEMSHAREDMEMNAME, unique);
	HANDLE hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(DLItem), sharedMemName);
	ATLASSERT( hMap );
	/* 共有メモリからビューを得る */
	DLItem* pDLItem = static_cast<DLItem*>(::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0));
	pDLItem->unique	= unique;
	pDLItem->hMapForClose = hMap;
	/* カスタムバインドコールバックを作成 */
	CCustomBindStatusCallBack* pCustomBscb = 
		new CCustomBindStatusCallBack(pDLItem, (HWND)::SendMessage(hWndMainFrame, WM_GETDOWNLOADINGVIEWHWND, 0, 0), defaultDLFolder);
	return pCustomBscb;
}

/// CDonutViewからダウンロードを開始する
void	CDownloadManager::StartTheDownload(LPCTSTR strURL, IBindStatusCallback* pcbsc)
{
	CString* pstrURL = new CString(strURL);
	boost::thread trd(&_DLStart, pstrURL, pcbsc);
}

//-------------------------------
/// strURLをダウンロードする
void	CDownloadManager::DownloadStart(LPCTSTR strURL, LPCTSTR strDLFolder, HWND hWnd, DWORD dwDLOption)
{
	if (CDLControlOption::s_bUseDLManager == false)
		return ;
	if (dwDLOption & DLO_SAVEIMAGE) {
		strDLFolder = static_cast<LPCTSTR>(CDLOptions::strImgDLFolderPath);
		dwDLOption	|= CDLOptions::dwImgExStyle;
	}
	if (dwDLOption & DLO_SHOWWINDOW || CDLOptions::bShowWindowOnDL) 
		OnShowDLManager(0, 0, NULL);

	auto pCBSCB = CreateCustomBindStatusCallBack(m_hWndParent, CDLOptions::strDLFolderPath);
	pCBSCB->SetReferer(s_strReferer);
	s_strReferer.Empty();
	pCBSCB->SetOption(strDLFolder, hWnd, dwDLOption);
	StartTheDownload(strURL, pCBSCB);

}

//---------------------------------------------
/// 現在ダウンロード中のアイテムの数を返す
int		CDownloadManager::GetDownloadingCount() const
{
	return m_wndDownload.GetDownloadingCount();
}



//----------------------------------
/// 通知用ウィンドウの初期化 : DLManagerのフレームウィンドウを作成
int CDownloadManager::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);
	m_wndDownload.CreateEx();
	//m_wndDownload.ShowWindow(FALSE);
	m_wndDownload.SetParentWindow(m_hWndParent);

	return 0;
}

//---------------------------------
/// DLManagerのフレームウィンドウの破棄
void CDownloadManager::OnDestroy()
{
	SetMsgHandled(FALSE);
	if (m_wndDownload.IsWindow()) {
		m_wndDownload.DestroyWindow();
	}
}

HWND CDownloadManager::OnGetDownloadingViewHWND()
{
	return m_wndDownload.m_wndDownloadingListView.m_hWnd;
}

//-------------------------------------
/// ダウンロードマネージャーを表示する
void CDownloadManager::OnShowDLManager(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (CDLControlOption::s_bUseDLManager == false)	{
		// 直接呼び出された場合は無視する
		if (!(uNotifyCode == 0 && nID == 0 && wndCtl == NULL)) {
			::MessageBox(NULL, _T("[Donutのオプション]->[ブラウザ]->「ダウンロードマネージャーを使用する」にチェックを入れてください"), NULL, MB_OK | MB_ICONWARNING);
			return;
		}
	}
	if (m_wndDownload.IsWindow() == FALSE) {
		m_wndDownload.CreateEx();
		m_wndDownload.ShowWindow(TRUE);
	} else {
		if (m_wndDownload.IsWindowVisible() == FALSE) 
			m_wndDownload.SetParent(NULL);
		if (m_wndDownload.IsZoomed() == FALSE)
			m_wndDownload.ShowWindow(SW_RESTORE);
		m_wndDownload.SetActiveWindow();
	}
	m_wndDownload.EnableVisible();

	if (m_wndDownload.IsZoomed() == FALSE) {	// ウィンドウから出ていたら元に戻す
		CRect rcWnd;
		m_wndDownload.GetWindowRect(rcWnd);
		HMONITOR	hMonitor = ::MonitorFromWindow(m_wndDownload, MONITOR_DEFAULTTONEAREST);
		MONITORINFO moniInfo = { sizeof (MONITORINFO) };
		::GetMonitorInfo(hMonitor, &moniInfo);
		if (   ::PtInRect(&moniInfo.rcWork, rcWnd.TopLeft()) == FALSE
			|| ::PtInRect(&moniInfo.rcWork, rcWnd.BottomRight()) == FALSE)
		{
			if (moniInfo.rcWork.top > rcWnd.top) 
				rcWnd.MoveToY(moniInfo.rcWork.top);
			if (moniInfo.rcWork.left > rcWnd.left)
				rcWnd.MoveToX(moniInfo.rcWork.left);
			if (moniInfo.rcWork.right < rcWnd.right)
				rcWnd.MoveToX(moniInfo.rcWork.right - rcWnd.Width());
			if (moniInfo.rcWork.bottom < rcWnd.bottom)
				rcWnd.MoveToY(moniInfo.rcWork.bottom - rcWnd.Height());
			m_wndDownload.MoveWindow(rcWnd);
		}
	}
}

//---------------------------------------
/// 既定のダウンロードフォルダを返す
LRESULT CDownloadManager::OnDefaultDLFolder(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)(LPCTSTR)CDLOptions::strDLFolderPath;
}

//---------------------------------------
/// 外部からダウンロードマネージャーでファイルをＤＬする
///
/// @param [in]	wParam	DLStartItemのポインタ
LRESULT CDownloadManager::OnStartDownload(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (CDLControlOption::s_bUseDLManager == false)
		return E_FAIL;

	DLStartItem* pItem  = (DLStartItem*)wParam;
	DownloadStart(pItem->strURL, pItem->strDLFolder, pItem->hWnd, pItem->dwOption);
	return S_OK;
}

//---------------------------------------
/// 外部からリファラを設定する
LRESULT CDownloadManager::OnSetReferer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetReferer((LPCTSTR)wParam);
	return 0;
}


//--------------------------------------
/// 別スレッドで開始されるDLの本体
void	CDownloadManager::_DLStart(CString* pstrURL, IBindStatusCallback* bscb)
{
	::CoInitialize(NULL);
	HRESULT hr = ::URLOpenStream(NULL, *pstrURL, 0, bscb);
	delete pstrURL;
	ATLVERIFY(bscb->Release() == 0);
	::CoUninitialize();
}

