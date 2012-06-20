/**
 *	@file	DonutView.cpp
 *	@brief	View
 */

#include "stdafx.h"
#include "DonutView.h"
#include "mshtmdid.h"
#include <sstream>
#include <boost\archive\text_woarchive.hpp>
//#include "option\DLControlOption.h"
//#include "option\MouseDialog.h"
//#include "option\MenuDialog.h"
#include "GlobalConfig.h"
#include "ExStyle.h"
#include "Donut.h"

#include "ScriptErrorCommandTargetImpl.h"
#include "Download/DownloadManager.h"
#include "AtlHostEx.h"
#include "ParseInternetShortcutFile.h"
#include "ChildFrameCommandUIUpdater.h"

// Constants
enum {
	DOCHOSTUIFLAG_THEME_VIEW =	DOCHOSTUIFLAG_NO3DBORDER | 
								DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | 
								DOCHOSTUIFLAG_THEME
};

///////////////////////////////////////////////////////////////////////////////
// CDonutView

/// Constructor

CDonutView::CDonutView(CChildFrameUIStateChange& UI)
	: m_UIChange(UI)
   #if _ATL_VER >= 0x700
	, m_ExternalUIDispatch(this)
   #endif
   #if defined USE_ATL3_BASE_HOSTEX == 0 /*_ATL_VER >= 0x700*/	//+++
	//, m_ExternalAmbientDispatch()
   #endif
	, m_bUseCustomDropTarget(false)
	, m_bDragAccept(false)
	, m_bExternalDrag(false)
	, m_bLightRefresh(false)
	, m_dwAutoRefreshStyle(0)
	, m_dwExStyle(0)
	, m_dwCurrentThreadId(0)
	, m_pGlobalConfig(nullptr)
{ }


void	CDonutView::SetDefaultFlags(DWORD dwDefaultDLCtrl, DWORD dwDefaultExStyle, DWORD dwAutoRefresh)
{
	m_dwDefaultDLControlFlags	= dwDefaultDLCtrl;
	m_dwDLControlFlags			= dwDefaultDLCtrl;
	m_dwExStyle					= dwDefaultExStyle;
	m_dwAutoRefreshStyle		= dwAutoRefresh;
}

/// 自動更新変更
void	CDonutView::SetAutoRefreshStyle(DWORD dwStyle)
{
	m_dwAutoRefreshStyle = 0;
	DWORD dwCurFlag = DVS_AUTOREFRESH_USER;
	while (dwCurFlag > 0) {
		if (dwCurFlag & dwStyle) {
			m_dwAutoRefreshStyle = dwCurFlag;
			break;
		}
		dwCurFlag >>= 1;
	}

	KillTimer(AutoRefreshTimerID);
	_SetTimer();
}


/// DLコントロールを設定する
void CDonutView::PutDLControlFlags(DWORD dwDLControlFlags)
{
	m_dwDLControlFlags	= dwDLControlFlags;
	m_dwDefaultDLControlFlags = dwDLControlFlags;

	CComQIPtr<IDispatch>	spDisp = m_spHost;
	if (spDisp) {
		VARIANT 		   varResult;
		DISPPARAMS		   params = { 0 };
		HRESULT hr = spDisp->Invoke(DISPID_AMBIENT_DLCONTROL, IID_NULL, 1041 /*JP*/, DISPATCH_PROPERTYPUT, &params, &varResult, NULL, NULL);
	}
	m_UIChange.SetDLCtrl(m_dwDLControlFlags);
#if 0
	HRESULT	hr;
	CComQIPtr<IOleObject>	spOleObject = m_spBrowser;
	ATLASSERT(spOleObject);
	CComPtr<IOleClientSite>	spOleOrgSite;
	hr = spOleObject->GetClientSite(&spOleOrgSite);	// 現在のサイトを保存

	hr = spOleObject->SetClientSite((IOleClientSite*)this);
	CComQIPtr<IOleControl>	spOleControl = m_spBrowser;
	ATLASSERT(spOleControl);
	hr = spOleControl->OnAmbientPropertyChange(DISPID_AMBIENT_DLCONTROL);

	spOleObject->SetClientSite(spOleOrgSite);
#endif
}

void	CDonutView::SetExStyle(DWORD dwExStyle)
{
	m_dwExStyle	= dwExStyle;
	m_UIChange.SetExStyle(m_dwExStyle);
}




void CDonutView::SetIeMenuNoCstm(int nStatus)
{
  #if _ATL_VER >= 0x700
	m_ExternalUIDispatch.SetIEContextMenuCustom(!nStatus);
  #else
	m_spAxAmbient->SetIeMenuNoCstm(nStatus);
  #endif
}



//ドラッグドロップ時の操作を制御するかIEコンポに任せるか
void CDonutView::SetOperateDragDrop(BOOL bOn)
{
	CComPtr<IAxWinHostWindow> spAxWindow;
	HRESULT hr = QueryHost(&spAxWindow);
	if ( FAILED(hr) )
		return;

	//SetRegisterAsDropTarget(bOn != 0/*? true : false*/);	// falseだとDDを受け付けない
  #if 0 //_ATL_VER >= 0x700
	if (bOn)
		m_ExternalUIDispatch.SetExternalDropTarget(this);	// DDを受け付ける
	else
		m_ExternalUIDispatch.SetExternalDropTarget(NULL);	// DDを受け付けない
  #endif
	if (bOn) {
		m_bUseCustomDropTarget = true;
	} else {
		m_bUseCustomDropTarget = false;
	}
}

bool CDonutView::UseDownloadManager() const
{
	return m_pGlobalConfig->bUseDownloadManager;
}

/// ダウンロードマネージャーで strURL をダウンロードする
void CDonutView::StartTheDownload(LPCTSTR strURL, bool bSaveImage /*= false*/)
{
	CWindow wndMainFrame = GetTopLevelWindow();

	DownloadData	DLdata;
	DLdata.strURL	= strURL;
	DLdata.strReferer	= GetLocationURL();
	DLdata.unique		= ::timeGetTime();
	DLdata.dwImageExStyle	= m_pGlobalConfig->dwDLImageExStyle;
	if (bSaveImage) {
		DLdata.strFolder	= m_pGlobalConfig->strImageDLFolder;
		DLdata.dwImageExStyle	|= DLO_SAVEIMAGE;
	}
	
	std::wstringstream ss;
	boost::archive::text_woarchive	ar(ss);
	ar << DLdata;
	std::wstring serializedData = ss.str();

	COPYDATASTRUCT cds;
	cds.dwData	= kFileDownload;
	cds.lpData	= (LPVOID)serializedData.data();
	cds.cbData	= (serializedData.length() + 1) * sizeof(WCHAR);
	GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
}

// Overrides
BOOL CDonutView::PreTranslateMessage(MSG *pMsg)
{
	if (   (pMsg->message < WM_KEYFIRST   || pMsg->message > WM_KEYLAST )
		&& (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST) )
	{
		return FALSE;
	}

	// give HTML page a chance to translate this message
	return SendMessage(WM_FORWARDMSG, 0, (LPARAM) pMsg) != 0;
}



// IUnknown
STDMETHODIMP CDonutView::QueryInterface(REFIID iid, void ** ppvObject)
{
    if (ppvObject == NULL) 
		return E_INVALIDARG;
	*ppvObject = NULL;
	HRESULT hr = S_OK;
// IID_IServiceProvider
	if (iid == IID_IServiceProvider) {
		*ppvObject = (IServiceProvider*)this;
// IID_IDropTarget
	} else if (iid == IID_IDropTarget) {
		*ppvObject = (IDropTarget*)this;
// IID_IUnknown	IDropTargetのため？
	} else if (iid == IID_IUnknown) {
		if (m_bUseCustomDropTarget)
			*ppvObject = (IUnknown*)(IDropTarget*)this;
// IID_IDownloadManager
	} else if (iid == IID_IDownloadManager) {
		*ppvObject = (IDownloadManager*)this;
	}

	if (*ppvObject == NULL)
		return E_NOINTERFACE;
	return S_OK;
}


// QueryService
STDMETHODIMP CDonutView::QueryService(REFGUID guidService, REFIID riid, void** ppv)
{
	if (guidService == SID_SDownloadManager && m_pGlobalConfig->bUseDownloadManager) {
		*ppv = (IDownloadManager*)this;
		return S_OK;
	}

	*ppv = NULL;
    return E_NOINTERFACE;
}



// IDropTarget
STDMETHODIMP CDonutView::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	m_spDropTargetHelper->DragEnter(m_hWnd, pDataObj, (LPPOINT)&pt, *pdwEffect);
	m_bExternalDrag = MtlIsDataAvailable(pDataObj, ::RegisterClipboardFormat(CFSTR_FILENAME));
	if (m_bExternalDrag)
		*pdwEffect |= DROPEFFECT_LINK;
	m_bDragAccept = _MtlIsHlinkDataObject(pDataObj);
	if (m_bDragAccept && m_bUseCustomDropTarget)
		*pdwEffect |= DROPEFFECT_LINK | DROPEFFECT_COPY;
	//*pdwEffect |= DROPEFFECT_LINK;// | DROPEFFECT_COPY | DROPEFFECT_MOVE;
	return S_OK;
}

STDMETHODIMP CDonutView::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	m_spDropTargetHelper->DragOver((LPPOINT)&pt, *pdwEffect);

	if (m_bExternalDrag)
		*pdwEffect |= DROPEFFECT_LINK;

	if (m_bDragAccept && m_bUseCustomDropTarget)
		*pdwEffect |= DROPEFFECT_LINK | DROPEFFECT_COPY;

	return S_OK;
}

STDMETHODIMP CDonutView::DragLeave()
{
	m_spDropTargetHelper->DragLeave();
	return S_OK;
}

STDMETHODIMP CDonutView::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	m_spDropTargetHelper->Drop(pDataObj, (LPPOINT)&pt, *pdwEffect);

	if (m_bDragAccept && m_bUseCustomDropTarget) {
		CSimpleArray<CString>	arrFiles;
		if ( MtlGetDropFileName(pDataObj, arrFiles) ) {	// ファイルがDropされた
			DWORD  df   = DonutGetStdOpenFlag();
			int  size = arrFiles.GetSize();
			//if (size == 1)
			//	df |= D_OPENFILE_NOCREATE;
			for (int i = 0; i < size; ++i) {
				COPYDATASTRUCT cds = { 0 };
				cds.dwData	= kNewDonutInstance;
				cds.lpData	= static_cast<LPVOID>(arrFiles[i].GetBuffer(0));
				cds.cbData	= (arrFiles[i].GetLength() + 1) * sizeof(TCHAR);
				GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
				//DonutOpenFile(arrFiles[i], df);
			}
			*pdwEffect = DROPEFFECT_COPY;
		} else {
			CString strText;
			if (   MtlGetHGlobalText(pDataObj, strText)
				|| MtlGetHGlobalText(pDataObj, strText, CF_SHELLURLW) )
			{
				COPYDATASTRUCT cds;
				cds.dwData	= kCommandDirect;
				cds.lpData	= static_cast<LPVOID>(strText.GetBuffer(0));
				cds.cbData	= (strText.GetLength() + 1) * sizeof(WCHAR);
				GetTopLevelParent().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
				*pdwEffect = DROPEFFECT_NONE;
			}
		}
	} else if (MTL::MtlIsDataAvailable(pDataObj, CF_SHELLURLW)) {	// タブなどから
		CString strURL;
		MtlGetHGlobalText(pDataObj, strURL, CF_SHELLURLW);
		if (strURL.IsEmpty() == FALSE) {
			MTL::ParseInternetShortcutFile(strURL);	// ファイルパス->URL
			Navigate2(strURL);
		}

	} else if (m_bExternalDrag) {	// 外部から
		CString strURL;
		MtlGetHGlobalText(pDataObj, strURL, ::RegisterClipboardFormat(CFSTR_FILENAME));
		if (strURL.IsEmpty() == FALSE) {
			MTL::ParseInternetShortcutFile(strURL);	// ファイルパス->URL
			Navigate2(strURL);
		}
	}
	return S_OK;
}


// IDispatch
STDMETHODIMP	CDonutView::Invoke(
		DISPID			dispidMember,
		REFIID			riid,
		LCID			lcid,
		WORD			wFlags,
		DISPPARAMS *	pdispparams,
		VARIANT *		pvarResult,
		EXCEPINFO * 	pexcepinfo,
		UINT *			puArgErr)
{
	if (!pvarResult) {
		return E_INVALIDARG;
	}

	if (dispidMember == DISPID_AMBIENT_DLCONTROL) {
		pvarResult->vt	 = VT_I4;
		pvarResult->lVal = m_dwDLControlFlags;
		return S_OK;
	}

	return DISP_E_MEMBERNOTFOUND;
}

// IDownloadManager

//--------------------------------------------
/// DLが開始されるときに呼ばれる
STDMETHODIMP CDonutView::Download(
	IMoniker* pmk,  
	IBindCtx* pbc,  
	DWORD	  dwBindVerb,  
	LONG	  grfBINDF,  
	BINDINFO* pBindInfo,  
	LPCOLESTR pszHeaders,  
	LPCOLESTR pszRedir,  
	UINT	  uiCP )
{
	if (::GetKeyState(VK_SHIFT) < 0)
		return E_FAIL;	// shiftを押しているとデフォルトに任せる

	CWindow wndMainFrame = GetTopLevelWindow();
	if (m_pGlobalConfig->bShowDLManagerOnDL)
		wndMainFrame.SendMessage(WM_COMMAND, ID_SHOW_DLMANAGER);

	CString strReferer = GetLocationURL();
	CCustomBindStatusCallBack* pCBSCB = CDownloadManager::CreateCustomBindStatusCallBack(wndMainFrame, reinterpret_cast<uintptr_t>(pmk), m_pGlobalConfig->strDefaultDLFolder);
	IBindStatusCallback* pbscbPrev;
	HRESULT hr = ::RegisterBindStatusCallback(pbc, (IBindStatusCallback*)pCBSCB, &pbscbPrev, 0);
	if (FAILED(hr) && pbscbPrev) {
		hr = pbc->RevokeObjectParam(L"_BSCB_Holder_");
		if (SUCCEEDED(hr)) {
			TRACEIN(_T("Download() : _BSCB_Holder_"));
			// 今度は成功する
			hr = ::RegisterBindStatusCallback(pbc, (IBindStatusCallback*)pCBSCB, NULL, 0);
			if (SUCCEEDED(hr)) {				
				pCBSCB->SetReferer(strReferer);
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
		if (pszHeaders == nullptr) {
			TRACEIN(_T("Download() : Referer : %s"), (LPCTSTR)strReferer);
			pCBSCB->SetReferer(strReferer);
			IBindCtx* pBC;
			hr = ::CreateAsyncBindCtx(0, pCBSCB, NULL, &pBC);
			pbc = pBC;
		}
	}
	if (SUCCEEDED(hr)) {
		CComPtr<IDispatch> spDisp;
		m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2> spDoc = spDisp;
		if (spDoc) {
			CComPtr<IHTMLElement>	spElm;
			spDoc->get_body(&spElm);
			if (spElm) {
				CComBSTR strBody;
				spElm->get_outerHTML(&strBody);

				auto funcTravelLogCount = [this]() -> DWORD {
					CComQIPtr<IServiceProvider>	 pISP = m_spBrowser;
					if (pISP == nullptr)
						return 0;

					CComPtr<ITravelLogStg>		 pTLStg;
					HRESULT hr	= pISP->QueryService(SID_STravelLogCursor, IID_ITravelLogStg, (void **) &pTLStg);
					if (FAILED(hr) || pTLStg == nullptr)
						return 0;
					DWORD	dwTotalCount = 0;
					DWORD	dwCount = 0;
					hr = pTLStg->GetCount(TLEF_RELATIVE_FORE, &dwCount);
					dwTotalCount += dwCount;
					hr = pTLStg->GetCount(TLEF_RELATIVE_BACK, &dwCount);
					dwTotalCount += dwCount;
					return dwTotalCount;
				};
				DWORD dwHistoryCount = funcTravelLogCount();
				if (CString(strBody) == _T("\r\n<BODY></BODY>") && dwHistoryCount == 0)
					GetParent().PostMessage(WM_CLOSE);	// 空のページなので閉じる
			}
		}

		pCBSCB->SetOption(_T(""), NULL, m_pGlobalConfig->dwDLImageExStyle);
		pCBSCB->SetThreadId(m_dwCurrentThreadId);
		GetParent().SendMessage(WM_INCREMENTTHREADREFCOUNT);
		CComPtr<IStream>	spStream;
		hr = pmk->BindToStorage(pbc, NULL, IID_IStream, (void**)&spStream);
	} else {
		delete pCBSCB;
	}

	return hr;
}



// UDT DGSTR
void CDonutView::OnMultiChg(WORD, WORD, HWND)
{
	_ToggleFlag(ID_DLCTL_BGSOUNDS, DLCTL_BGSOUNDS);
	_ToggleFlag(ID_DLCTL_VIDEOS  , DLCTL_VIDEOS  );
	_ToggleFlag(ID_DLCTL_DLIMAGES, DLCTL_DLIMAGES);
	_LightRefresh();
}


/// 現在のDLコントロールを反転させる
void CDonutView::OnSecuChg(WORD, WORD, HWND)
{
	_ToggleFlag(ID_DLCTL_SCRIPTS		, DLCTL_NO_SCRIPTS			, TRUE);
	_ToggleFlag(ID_DLCTL_JAVA			, DLCTL_NO_JAVA 			, TRUE);
	_ToggleFlag(ID_DLCTL_DLACTIVEXCTLS	, DLCTL_NO_DLACTIVEXCTLS	, TRUE);
	_ToggleFlag(ID_DLCTL_RUNACTIVEXCTLS , DLCTL_NO_RUNACTIVEXCTLS	, TRUE);
	_LightRefresh();
}


/// 全部のDLコントロールのオン/オフ切り替え
void CDonutView::OnAllOnOff(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	switch (wID) {
	case ID_DLCTL_ON_OFF_MULTI:
		if ( ( GetDLControlFlags() & (DLCTL_DLIMAGES | DLCTL_BGSOUNDS | DLCTL_VIDEOS) ) == (DLCTL_DLIMAGES | DLCTL_BGSOUNDS | DLCTL_VIDEOS) )
			_RemoveFlag(DLCTL_BGSOUNDS | DLCTL_VIDEOS | DLCTL_DLIMAGES);
		else
			_AddFlag(DLCTL_BGSOUNDS | DLCTL_VIDEOS | DLCTL_DLIMAGES);
		break;

	case ID_DLCTL_ON_OFF_SECU:

		if ( ( ( GetDLControlFlags() & (DLCTL_NO_RUNACTIVEXCTLS | DLCTL_NO_DLACTIVEXCTLS | DLCTL_NO_SCRIPTS | DLCTL_NO_JAVA) ) == 0 ) ) {
			//チェックは全部ついている
			_AddFlag(DLCTL_NO_SCRIPTS | DLCTL_NO_JAVA | DLCTL_NO_DLACTIVEXCTLS | DLCTL_NO_RUNACTIVEXCTLS);
		} else {
			//チェックされていない項目がある
			_RemoveFlag(DLCTL_NO_SCRIPTS | DLCTL_NO_JAVA | DLCTL_NO_DLACTIVEXCTLS | DLCTL_NO_RUNACTIVEXCTLS);
		}

		break;
	}

	_LightRefresh();
}


/// ウィンドウの初期化
int CDonutView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// Let me initialize itself
	LRESULT lRet = DefWindowProc();

	m_dwCurrentThreadId = ::GetCurrentThreadId();
	try {
		HRESULT hr = QueryControl(IID_IWebBrowser2, (void**)&m_spBrowser);
		ATLASSERT(m_spBrowser);

		BOOL	bCheck	= GetRegisterAsDropTarget();
		hr = CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER,
                     IID_IDropTargetHelper, (LPVOID*)&m_spDropTargetHelper);
		ATLASSERT(hr == S_OK);

		// Set flat scrollbar style
		CComPtr<IAxWinHostWindow>	spAxHostWindow;
		hr	= QueryHost(&spAxHostWindow);	// ホストを取得
		if (FAILED(hr))
			AtlThrow(hr);

		m_spAxAmbient	= spAxHostWindow;
		ATLASSERT(m_spAxAmbient);
		hr	= m_spAxAmbient->put_DocHostFlags(DOCHOSTUIFLAG_THEME_VIEW);
		if (FAILED(hr))
			AtlThrow(hr);

		// 外部UIのIDispatchインターフェイスを設定する
		CComQIPtr<IDocHostUIHandler>	   pDefaultHandler = spAxHostWindow;
		m_ExternalUIDispatch.SetDefaultUIHandler(pDefaultHandler);
		spAxHostWindow->SetExternalUIHandler(&m_ExternalUIDispatch);

		// 外部AmbientIDispatchインターフェイスを設定する
		CComQIPtr<IAxWinAmbientDispatchEx> pAmbient 	   = spAxHostWindow;
		m_spHost = spAxHostWindow;
		//m_ExternalAmbientDispatch.SetHostWindow(spAxWindow);
		pAmbient->SetAmbientDispatch((IDispatch*)this);//(&m_ExternalAmbientDispatch);

		// DLMnager用にIServiceProviderを登録する
		CComQIPtr<IObjectWithSite>	spObjectWithSite = spAxHostWindow;
		ATLASSERT(spObjectWithSite);
		hr = spObjectWithSite->SetSite((IUnknown*)(IServiceProvider*)this);

		_InitDLControlFlags();

		SetIeMenuNoCstm(m_pGlobalConfig->bNoCustomIEMenu);
		if (m_pGlobalConfig->nDragDropCommandID)
			SetOperateDragDrop(TRUE);

	  #if 1	//+++
		if (m_dwExStyle == -1)
			m_dwExStyle = m_pGlobalConfig->dwExtendedStyleFlags;
		//m_UIChange.SetExStyle(m_dwExStyle);
	  #else
		ATLASSERT(m_dwExStyle == 0);
		m_dwExStyle = CDLControlOption::s_dwExtendedStyleFlags; //_dwViewStyle;
	  #endif
		_SetTimer();	// 設定されているなら自動更新を開始

		RegisterDragDrop(m_hWnd, this);
	}
	catch (const CAtlException& e) {
		MessageBox(GetLastErrorString(e));
	}
	return 0;
}



void CDonutView::OnDestroy()
{
	RevokeDragDrop(m_hWnd);

	if (m_spBrowser) {
#if 0
		HRESULT hr = m_spBrowser->Stop();
		CComQIPtr<IOleInPlaceObject>	spInPlaceObject = m_spBrowser;
		ATLASSERT(spInPlaceObject);
		hr = spInPlaceObject->InPlaceDeactivate();
		ATLASSERT(SUCCEEDED(hr));
		spInPlaceObject.Release();

		// Set Client Site
		CComPtr<IOleObject>	spOleObject;
		hr = m_spBrowser->QueryInterface(IID_IOleObject, (void**)&spOleObject);
		ATLASSERT(SUCCEEDED(hr));
		hr = spOleObject->Close(OLECLOSE_NOSAVE);
#endif
		//spOleObject.Release();
		m_spBrowser.Release();
		m_spAxAmbient.Release();
		m_spHost.Release();
	}

	if (m_bUseCustomDropTarget)
		SetOperateDragDrop(FALSE);

	KillTimer(AutoRefreshTimerID);

	DefWindowProc();
}



void CDonutView::OnMultiBgsounds(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	_ToggleFlag(ID_DLCTL_BGSOUNDS, DLCTL_BGSOUNDS);
	_LightRefresh();
}



void CDonutView::OnMultiVideos(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	_ToggleFlag(ID_DLCTL_VIDEOS, DLCTL_VIDEOS);
	_LightRefresh();
}



void CDonutView::OnMultiDlImages(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	ATLTRACE2( atlTraceGeneral, 4, _T("CDonutView::OnMultiDlImages\n") );

	if ( _ToggleFlag(ID_DLCTL_DLIMAGES, DLCTL_DLIMAGES) )
		_LightRefresh();
}



void CDonutView::OnSecurRunactivexctls(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	_ToggleFlag(ID_DLCTL_RUNACTIVEXCTLS, DLCTL_NO_RUNACTIVEXCTLS, TRUE);
	_LightRefresh();
}



void CDonutView::OnSecurDlactivexctls(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if ( !_ToggleFlag(ID_DLCTL_DLACTIVEXCTLS, DLCTL_NO_DLACTIVEXCTLS, TRUE) )
		_LightRefresh();
}



void CDonutView::OnSecurScritps(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	_ToggleFlag(ID_DLCTL_SCRIPTS, DLCTL_NO_SCRIPTS, TRUE);
	_LightRefresh();
}



void CDonutView::OnSecurJava(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	_ToggleFlag(ID_DLCTL_JAVA, DLCTL_NO_JAVA, TRUE);
	_LightRefresh();
}


void	CDonutView::OnBlockMailto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (m_dwExStyle & DVS_EX_BLOCK_MAILTO)
		m_dwExStyle &= ~DVS_EX_BLOCK_MAILTO;
	else
		m_dwExStyle |= DVS_EX_BLOCK_MAILTO;
	m_UIChange.SetExStyle(m_dwExStyle);
}

void	CDonutView::OnMouseGesture(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (m_dwExStyle & DVS_EX_MOUSE_GESTURE)
		m_dwExStyle &= ~DVS_EX_MOUSE_GESTURE;
	else
		m_dwExStyle |= DVS_EX_MOUSE_GESTURE;
	m_UIChange.SetExStyle(m_dwExStyle);
}

void	CDonutView::OnMessageFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (m_dwExStyle & DVS_EX_MESSAGE_FILTER)
		m_dwExStyle &= ~DVS_EX_MESSAGE_FILTER;
	else
		m_dwExStyle |= DVS_EX_MESSAGE_FILTER;
	m_UIChange.SetExStyle(m_dwExStyle);
}

void	CDonutView::OnAutoRefreshNone(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle = 0;
	KillTimer(AutoRefreshTimerID);
}

void	CDonutView::OnAutoRefresh15sec(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle  = 0;
	m_dwAutoRefreshStyle |= DVS_AUTOREFRESH_15SEC;
	KillTimer(AutoRefreshTimerID);
	_SetTimer();
}

void	CDonutView::OnAutoRefresh30sec(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle  = 0;
	m_dwAutoRefreshStyle |= DVS_AUTOREFRESH_30SEC;
	KillTimer(AutoRefreshTimerID);
	_SetTimer();
}

void	CDonutView::OnAutoRefresh1min(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle  = 0;
	m_dwAutoRefreshStyle |= DVS_AUTOREFRESH_1MIN;
	KillTimer(AutoRefreshTimerID);
	_SetTimer();
}

void	CDonutView::OnAutoRefresh2min(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle  = 0;
	m_dwAutoRefreshStyle |= DVS_AUTOREFRESH_2MIN;
	KillTimer(AutoRefreshTimerID);
	_SetTimer();
}

void	CDonutView::OnAutoRefresh5min(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle  = 0;
	m_dwAutoRefreshStyle |= DVS_AUTOREFRESH_5MIN;
	KillTimer(AutoRefreshTimerID);
	_SetTimer();
}

void	CDonutView::OnAutoRefreshUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle  = 0;
	m_dwAutoRefreshStyle |= DVS_AUTOREFRESH_USER;
	KillTimer(AutoRefreshTimerID);
	_SetTimer();
}

/// ページを更新する
void	CDonutView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == AutoRefreshTimerID) {
		::PostMessage(GetParent(), WM_COMMAND, ID_VIEW_REFRESH, 0);
	} else {
		SetMsgHandled(FALSE);
	}
}


// Implementation


/// DLフラグをトグルする
bool CDonutView::_ToggleFlag(WORD wID, DWORD dwFlag, BOOL bReverse)
{
	bool  bRet			   = false;
	DWORD dwDLControlFlags = m_dwDLControlFlags;
	if (dwDLControlFlags & dwFlag) {
		dwDLControlFlags &= ~dwFlag;
	} else {
		dwDLControlFlags |= dwFlag;
		bRet			  = true;
	}

	PutDLControlFlags(dwDLControlFlags);
	return bRet;
}


/// DLコントロールの変更を有効にするために更新する
void CDonutView::_LightRefresh()
{
	if (::GetKeyState(VK_CONTROL) < 0)
		return;

	m_bLightRefresh = true;
	CString strURL = GetLocationURL();
	Navigate2(strURL);
}


/// DLフラグを追加する
void CDonutView::_AddFlag(DWORD dwFlag) 	//minit
{
	DWORD	dwDLControlFlags = m_dwDLControlFlags;
	dwDLControlFlags |= dwFlag;

	PutDLControlFlags(dwDLControlFlags);
}

/// DLフラグを取り除く
void CDonutView::_RemoveFlag(DWORD dwFlag)	//minit
{
	DWORD dwDLControlFlags = m_dwDLControlFlags;
	dwDLControlFlags &= ~dwFlag;

	PutDLControlFlags(dwDLControlFlags);
}

/// 自動更新用のタイマーを開始する
void	CDonutView::_SetTimer()
{
	auto GetElapse = [this]() -> int {
		if (m_dwAutoRefreshStyle == 0)
			return -1;
		else if (m_dwAutoRefreshStyle & DVS_AUTOREFRESH_15SEC)
			return 15 * 1000;
		else if (m_dwAutoRefreshStyle & DVS_AUTOREFRESH_30SEC)
			return 30 * 1000;
		else if (m_dwAutoRefreshStyle & DVS_AUTOREFRESH_1MIN)
			return 1 * 60 * 1000;
		else if (m_dwAutoRefreshStyle & DVS_AUTOREFRESH_2MIN)
			return 2 * 60 * 1000;
		else if (m_dwAutoRefreshStyle & DVS_AUTOREFRESH_5MIN)
			return 5 * 60 * 1000;
		else if (m_dwAutoRefreshStyle & DVS_AUTOREFRESH_USER)
			return CMainOption::s_dwAutoRefreshTime * 1000;

		return -1;
	};

	int nElapse = GetElapse();
	if (nElapse != -1)
		SetTimer(AutoRefreshTimerID, nElapse);

	m_UIChange.SetAutoRefreshStyle(m_dwAutoRefreshStyle);
}
