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

	s_pThis = this;
}

//--------------------------------
/// DLManagerを使うかどうかを返す
bool CDownloadManager::UseDownloadManager()
{
	return CDLControlOption::s_bUseDLManager ? true : false;
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
		//dwDLOption	|= CDLOptions::bShowWindowOnDL ? DLO_SHOWWINDOW : 0;
	}
	if (dwDLOption & DLO_SHOWWINDOW || CDLOptions::bShowWindowOnDL) 
		OnShowDLManager(0, 0, NULL);

	CCustomBindStatusCallBack* pCBSCB = _CreateCustomBindStatusCallBack();
	pCBSCB->SetReferer(s_strReferer);
	s_strReferer.Empty();
	pCBSCB->SetOption(strDLFolder, hWnd, dwDLOption);
	CString* pstrURL = new CString(strURL);
	boost::thread trd(boost::bind(&CDownloadManager::_DLStart, this, pstrURL, (IBindStatusCallback*)pCBSCB));

}

//---------------------------------------------
/// 現在ダウンロード中のアイテムの数を返す
int		CDownloadManager::GetDownloadingCount() const
{
	return m_wndDownload.GetDownloadingCount();
}




// IUnknown

STDMETHODIMP CDownloadManager::QueryInterface(REFIID iid, void ** ppvObject)
{
    if (ppvObject == NULL) 
		return E_INVALIDARG;
    
	*ppvObject = NULL;

	if (iid == IID_IUnknown) {
		*ppvObject = (IUnknown*)this;
	} else if (iid == IID_IDownloadManager) {
		*ppvObject = (IDownloadManager*)this;
	}

	if (*ppvObject == NULL)
		return E_NOINTERFACE;

	return S_OK;
}


// IDownloadManager

//--------------------------------------------
/// DLが開始されるときに呼ばれる
STDMETHODIMP CDownloadManager::Download(
	IMoniker* pmk,  
	IBindCtx* pbc,  
	DWORD	  dwBindVerb,  
	LONG	  grfBINDF,  
	BINDINFO* pBindInfo,  
	LPCOLESTR pszHeaders,  
	LPCOLESTR pszRedir,  
	UINT	  uiCP )
{
	if (CDLControlOption::s_bUseDLManager == false)
		return E_FAIL;

	if (::GetKeyState(VK_MENU) < 0)
		return E_FAIL;	// Altを押しているとデフォルトに任せる

	if (CDLOptions::bShowWindowOnDL)
		OnShowDLManager(0, 0, NULL);

	CCustomBindStatusCallBack* pCBSCB = _CreateCustomBindStatusCallBack();
	IBindStatusCallback* pbscbPrev;
	HRESULT hr = ::RegisterBindStatusCallback(pbc, (IBindStatusCallback*)pCBSCB, &pbscbPrev, 0);
	if (FAILED(hr) && pbscbPrev) {
		hr = pbc->RevokeObjectParam(L"_BSCB_Holder_");
		if (SUCCEEDED(hr)) {
			// 今度は成功する
			hr = ::RegisterBindStatusCallback(pbc, (IBindStatusCallback*)pCBSCB, NULL, 0);
			if (SUCCEEDED(hr)) {
				pCBSCB->SetBSCB(pbscbPrev);
				pCBSCB->SetBindCtx(pbc);
			}
		}
	} else {
#if 0	//\\ 自前でやっちゃうとまずいっぽい
		// pbscbPrevがNULLだったときの場合
		LPOLESTR strUrl;
		hr = pmk->GetDisplayName(pbc, NULL, &strUrl);
		if (SUCCEEDED(hr)) {
			DownloadStart(strUrl);
			::CoTaskMemFree(strUrl);
			return S_OK;
		}
#endif
	}
	if (SUCCEEDED(hr)) {
		CComPtr<IStream>	spStream;
		hr = pmk->BindToStorage(pbc, NULL, IID_IStream, (void**)&spStream);
	} else {
		delete pCBSCB;
	}

	return hr;
}



//----------------------------------
/// 通知用ウィンドウの初期化 : DLManagerのフレームウィンドウを作成
int CDownloadManager::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);
	m_wndDownload.CreateEx();
	//m_wndDownload.ShowWindow(TRUE);
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

//-------------------------------------
/// ダウンロードマネージャーを表示する
void CDownloadManager::OnShowDLManager(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (CDLControlOption::s_bUseDLManager == false)	{
		::MessageBox(NULL, _T("[Donutのオプション]->[ブラウザ]->「ダウンロードマネージャーを使用する」にチェックを入れてください"), NULL, MB_OK | MB_ICONWARNING);
		return;
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
	DLStartItem* pItem  = (DLStartItem*)wParam;
	if (CDLControlOption::s_bUseDLManager == false)
		return E_FAIL;

	DownloadStart(pItem->strURL, pItem->strDLFolder, pItem->hWnd, pItem->dwOption);
	return S_OK;
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


//---------------------------------------
CCustomBindStatusCallBack*	CDownloadManager::_CreateCustomBindStatusCallBack()
{
	DLItem* pDLItem = new DLItem;
	CCustomBindStatusCallBack* pCustomBscb = new CCustomBindStatusCallBack(pDLItem, m_wndDownload.m_wndDownloadingListView.m_hWnd);
	return pCustomBscb;
}