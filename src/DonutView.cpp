/**
 *	@file	DonutView.cpp
 *	@brief	View
 */

#include "stdafx.h"
#include "DonutView.h"
#include "mshtmdid.h"
#include "option/DLControlOption.h"
#include "Donut.h"

#include "ScriptErrorCommandTargetImpl.h"
#include "Download/DownloadManager.h"
#include "AtlHostEx.h"
#include "ParseInternetShortcutFile.h"

// Constants
enum {
	#if 1	//+++ xp未満ではフラットスクロールバーを設定、xp以後ではthemeを適用.
	DOCHOSTUIFLAG_FLATVIEW		= (DOCHOSTUIFLAG_FLAT_SCROLLBAR | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME),
	DOCHOSTUIFLAG_THEME_VIEW    = (DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME),
	DOCHOSTUIFLAG_NOT_FLATVIEW	= ( /*DOCHOSTUIFLAG_NO3DBORDER*/ DOCHOSTUIFLAG_NO3DOUTERBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE),
	#elif 1	// unDonut+
	DOCHOSTUIFLAG_FLATVIEW		= (DOCHOSTUIFLAG_FLAT_SCROLLBAR | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME),
	DOCHOSTUIFLAG_NOT_FLATVIEW	= ( /*DOCHOSTUIFLAG_NO3DBORDER*/ DOCHOSTUIFLAG_NO3DOUTERBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE),
	#else
	//docHostUIFlagDEFAULT		= (docHostUIFlagFLAT_SCROLLBAR | docHostUIFlagNO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE |DOCHOSTUIFLAG_THEME),
	//docHostUIFlagNotFlatView	= (docHostUIFlagNO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE),		// UDT DGSTR ( added by dai
	// DonutRAPT(1.26)の値.
	//docHostUIFlagDEFAULT 		= (docHostUIFlagFLAT_SCROLLBAR | docHostUIFlagNO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME),
	//docHostUIFlagNotFlatScrBar= (docHostUIFlagNO3DBORDER     | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME),
	#endif
};

///////////////////////////////////////////////////////////////////////////////
// CDonutView

/// Constructor
CDonutView::CDonutView(DWORD dwDefaultDLControlFlags, DWORD dwExStyleFlags)
	: m_ViewOption(this, dwExStyleFlags)						//+++ dwExStyleFlags追加.
	, m_dwDefaultDLControlFlags(dwDefaultDLControlFlags)
	, m_dwDLControlFlags(dwDefaultDLControlFlags)
	//, m_dwDefaultExtendedStyleFlags(dwExStyleFlags)			//+++
	, m_nDDCommand(0)
   #if _ATL_VER >= 0x700
	, m_ExternalUIDispatch(this)
   #endif
   #if defined USE_ATL3_BASE_HOSTEX == 0 /*_ATL_VER >= 0x700*/	//+++
	//, m_ExternalAmbientDispatch()
   #endif
	, m_bUseCustomDropTarget(false)
	, m_bExternalDrag(false)
	, m_bLightRefresh(false)
{ }


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



void CDonutView::SetIeMenuNoCstm(int nStatus)
{
  #if _ATL_VER >= 0x700
	m_ExternalUIDispatch.SetIEContextMenuCustom(!nStatus);
  #else
	m_spAxAmbient->SetIeMenuNoCstm(nStatus);
  #endif
}



//ドラッグドロップ時の操作を制御するかIEコンポに任せるか
void CDonutView::SetOperateDragDrop(BOOL bOn, int nCommand)
{
	CComPtr<IAxWinHostWindow> spAxWindow;
	HRESULT hr = QueryHost(&spAxWindow);
	if ( FAILED(hr) )
		return;

	SetRegisterAsDropTarget(bOn != 0/*? true : false*/);	// falseだとDDを受け付けない
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
	m_nDDCommand = nCommand;
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
		*ppvObject = (IUnknown*)(IDropTarget*)this;
	}

	if (*ppvObject == NULL)
		return E_NOINTERFACE;
	return S_OK;
}


// QueryService
STDMETHODIMP CDonutView::QueryService(REFGUID guidService, REFIID riid, void** ppv)
{
	if (guidService == SID_SDownloadManager && CDownloadManager::UseDownloadManager()) {
		CString strReferer = GetLocationURL();
		CDownloadManager::SetReferer(strReferer);
		*ppv = (IDownloadManager*)CDownloadManager::GetInstance();
		return S_OK;
	}

	*ppv = NULL;
    return E_NOINTERFACE;
}



// IDropTarget
STDMETHODIMP CDonutView::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	m_spDropTargetHelper->DragEnter(m_hWnd, pDataObj, (LPPOINT)&pt, *pdwEffect);
	HRESULT hr = m_spDefaultDropTarget->DragEnter(pDataObj, grfKeyState, pt, pdwEffect);
	m_bExternalDrag = MtlIsDataAvailable(pDataObj, ::RegisterClipboardFormat(CFSTR_FILENAME));
	if (m_bExternalDrag)
		*pdwEffect |= DROPEFFECT_LINK;
	//*pdwEffect |= DROPEFFECT_LINK;// | DROPEFFECT_COPY | DROPEFFECT_MOVE;
	return hr;
}

STDMETHODIMP CDonutView::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	m_spDropTargetHelper->DragOver((LPPOINT)&pt, *pdwEffect);
	HRESULT hr = m_spDefaultDropTarget->DragOver(grfKeyState, pt, pdwEffect);
	
	if (m_bUseCustomDropTarget) {
		if (*pdwEffect == DROPEFFECT_COPY) {
			m_bTempUseDefaultDropTarget = true;
		} else {
			m_bTempUseDefaultDropTarget = false;
		}
		hr = S_OK;
		*pdwEffect = DROPEFFECT_COPY | DROPEFFECT_LINK;
	}
	if (m_bExternalDrag)
		*pdwEffect |= DROPEFFECT_LINK;
	return hr;
}

STDMETHODIMP CDonutView::DragLeave()
{
	m_spDropTargetHelper->DragLeave();
	return m_spDefaultDropTarget->DragLeave();
}

STDMETHODIMP CDonutView::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	m_spDropTargetHelper->Drop(pDataObj, (LPPOINT)&pt, *pdwEffect);
	HRESULT hr = m_spDefaultDropTarget->Drop(pDataObj, grfKeyState, pt, pdwEffect);

	if (m_bUseCustomDropTarget && m_bTempUseDefaultDropTarget == false) {
		hr = S_OK;
		CSimpleArray<CString>	arrFiles;
		if ( MtlGetDropFileName(pDataObj, arrFiles) ) {	// ファイルがDropされた
			unsigned  df   = DonutGetStdOpenFlag();
			unsigned  size = arrFiles.GetSize();
			//if (size == 1)
			//	df |= D_OPENFILE_NOCREATE;
			for (unsigned i = 0; i < size; ++i)
				DonutOpenFile(m_hWnd, arrFiles[i], df);
			*pdwEffect = DROPEFFECT_COPY;
		} else {
			CString strText;
			if (   MtlGetHGlobalText(pDataObj, strText)
				|| MtlGetHGlobalText(pDataObj, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL)) )
			{
				::SendMessage(GetTopLevelParent(), WM_COMMAND_DIRECT, m_nDDCommand, (LPARAM) (LPCTSTR) strText);
				*pdwEffect = DROPEFFECT_NONE;
			}
		}
	} else {	// 外部から
		CString strURL;
		MtlGetHGlobalText(pDataObj, strURL, ::RegisterClipboardFormat(CFSTR_FILENAME));
		if (strURL.IsEmpty() == FALSE) {
			MTL::ParseInternetShortcutFile(strURL);	// ファイルパス->URL
			Navigate2(strURL);
		}
	}

	return hr;
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

		//フラットビューの可否を適用
		//hr = m_spAxAmbient->put_DocHostFlags(_bFlatView ? docHostUIFlagNotFlatView : docHostUIFlagDEFAULT); // UDT DGSTR ( added by dai
	  #if 1	//+++ xp以降は themeによって平面的であることを期待して、フラットスクロールバーにしないtheme onの設定にする.
		//BOOL	bFlatView	= (m_dwDefaultExtendedStyleFlags & DVS_EX_FLATVIEW) != 0;	//+++ ? 1 : 0;
		BOOL	bFlatView	= (m_ViewOption.m_dwExStyle      & DVS_EX_FLATVIEW) != 0;	//+++ ? 1 : 0;

		unsigned flags      = DOCHOSTUIFLAG_NOT_FLATVIEW;
		if (bFlatView) {
			flags = _CheckOsVersion_XPLater() ? DOCHOSTUIFLAG_THEME_VIEW : DOCHOSTUIFLAG_NOT_FLATVIEW;
		}
	  #else
		BOOL	bFlatView	= (CDLControlOption::s_dwExtendedStyleFlags & DVS_EX_FLATVIEW) != 0;	//+++ ? 1 : 0;
		unsigned flags      = bFlatView ? DOCHOSTUIFLAG_FLATVIEW : DOCHOSTUIFLAG_NOT_FLATVIEW;
	  #endif
		hr	= m_spAxAmbient->put_DocHostFlags(flags);		//\\ flagsを設定する
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

	  #if 1	//+++
		if (m_ViewOption.m_dwExStyle == (DWORD)-1)
			m_ViewOption.m_dwExStyle = CDLControlOption::s_dwExtendedStyleFlags; //_dwViewStyle;
	  #else
		ATLASSERT(m_ViewOption.m_dwExStyle == 0);
		m_ViewOption.m_dwExStyle = CDLControlOption::s_dwExtendedStyleFlags; //_dwViewStyle;
	  #endif
	}
	catch (const CAtlException& e) {
		e;
		MessageBox(_T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
	}
	return lRet;
}



void CDonutView::OnDestroy()
{
	SetMsgHandled(FALSE);

	HRESULT hr = m_spBrowser->Stop();
	CComQIPtr<IOleInPlaceObject>	spInPlaceObject = m_spBrowser;
	ATLASSERT(spInPlaceObject);
	hr = spInPlaceObject->InPlaceDeactivate();
	ATLASSERT(SUCCEEDED(hr));

	// Set Client Site
	CComPtr<IOleObject>	spOleObject;
	hr = m_spBrowser->QueryInterface(IID_IOleObject, (void**)&spOleObject);
	ATLASSERT(SUCCEEDED(hr));
	hr = spOleObject->Close(OLECLOSE_NOSAVE);
	hr = spOleObject->SetClientSite(NULL);

	m_ViewOption.Uninit();
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



DWORD CDonutView::_GetDLControlFlags()
{
	DWORD dwDLControlFlags = m_dwDLControlFlags;
	return dwDLControlFlags;
}


//+++
DWORD	CDonutView::_GetExtendedStypeFlags()
{
	//return m_dwDefaultExtendedStyleFlags;
	return m_ViewOption.m_dwExStyle;
}





void CDonutView::OnUpdateDLCTL_ChgMulti(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		enum { FLAGS = (DLCTL_DLIMAGES | DLCTL_BGSOUNDS | DLCTL_VIDEOS) };
		pCmdUI->SetCheck ( (GetDLControlFlags() & FLAGS) == FLAGS /*? 1 : 0*/ );
	}
}



void CDonutView::OnUpdateDLCTL_ChgSecu(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		BOOL bSts = TRUE;

		if (GetDLControlFlags() & DLCTL_NO_RUNACTIVEXCTLS)	bSts = FALSE;
		if (GetDLControlFlags() & DLCTL_NO_DLACTIVEXCTLS)	bSts = FALSE;
		if (GetDLControlFlags() & DLCTL_NO_SCRIPTS) 		bSts = FALSE;
		if (GetDLControlFlags() & DLCTL_NO_JAVA)			bSts = FALSE;

		pCmdUI->SetCheck(bSts);
	}
}



void CDonutView::OnUpdateDLCTL_DLIMAGES(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		pCmdUI->SetCheck((GetDLControlFlags() & DLCTL_DLIMAGES) != 0 /*? 1 : 0*/);
	}
}



void CDonutView::OnUpdateDLCTL_RUNACTIVEXCTLS(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		pCmdUI->SetCheck((GetDLControlFlags() & DLCTL_NO_RUNACTIVEXCTLS) ? 0 : 1);
	}
}



void CDonutView::OnUpdateDocHostUIOpenNewWinUI(CCmdUI *pCmdUI)
{
	DWORD dwDocHostFlags = 0;		//+++ 念のため初期値設定.

	m_spAxAmbient->get_DocHostFlags(&dwDocHostFlags);
	pCmdUI->Enable();
	pCmdUI->SetCheck((dwDocHostFlags & docHostUIFlagOPENNEWWIN) != 0 /*? 1 : 0*/);
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
