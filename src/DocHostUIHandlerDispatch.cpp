// DocHostUIHandlerDispatch.cpp

#include "stdafx.h"
#include "DocHostUIHandlerDispatch.h"
#include "DonutView.h"
#include "MainFrame.h"
#include "option/MainOption.h"


// Constructor
CDocHostUIHandlerDispatch::CDocHostUIHandlerDispatch(CDonutView* pView)
	: m_bNoIECustom(false)
	, m_pView(pView)
{ }


STDMETHODIMP	CDocHostUIHandlerDispatch::QueryInterface (REFIID riid, void **ppvObject)
{
	if (riid == IID_IOleCommandTarget) {
		*ppvObject = (IOleCommandTarget*)this;
		return S_OK;
	}
	return E_NOINTERFACE;
}

#if 0
// IDocHostUIHandler
STDMETHODIMP CDocHostUIHandlerDispatch::ShowContextMenu( 
    /* [in] */ DWORD dwID,
    /* [in] */ POINT *ppt,
    /* [in] */ IUnknown *pcmdtReserved,
    /* [in] */ IDispatch *pdispReserved)
{
	if ( m_bNoIECustom && (GetKeyState(VK_LBUTTON) >= 0) )								//カスタム&左クリックされているかどうか
		return S_FALSE;
	HRESULT hr = CCustomContextMenu().Show(dwID, ppt->x, ppt->y, pcmdtReserved, pdispReserved);					//丸投げ
	return hr;
}
        
STDMETHODIMP CDocHostUIHandlerDispatch::GetHostInfo( 
    /* [out][in] */ DOCHOSTUIINFO *pInfo)
{
	HRESULT hr	= m_pDefaultHandler->GetHostInfo(pInfo);	//デフォルト動作
	return hr;
}


STDMETHODIMP CDocHostUIHandlerDispatch::GetDropTarget( 
    /* [in] */ IDropTarget *pDropTarget,
    /* [out] */ IDropTarget **ppDropTarget) 
{
	m_pView->SetDefaultDropTarget(pDropTarget);
	*ppDropTarget = (IDropTarget*)m_pView;
	return S_OK;
	//if (m_pExternalDropTarget) {
	//if (SUCCEEDED(hr))
	//	return S_OK;
	//}
	return E_NOTIMPL; 
}

#endif

STDMETHODIMP CDocHostUIHandlerDispatch::ShowContextMenu(
	/* [in] */ DWORD				dwID,
	/* [in] */ DWORD				x,
	/* [in] */ DWORD				y,
	/* [in] */ IUnknown*			pcmdtReserved,
	/* [in] */ IDispatch*			pdispReserved,
	/* [retval][out] */ HRESULT*	dwRetVal)
{
	if ( m_bNoIECustom && (GetKeyState(VK_LBUTTON) >= 0) )								//カスタム&左クリックされているかどうか
		return S_FALSE;
	*dwRetVal = CCustomContextMenu().Show(dwID, x, y, pcmdtReserved, pdispReserved);					//丸投げ
	return S_OK;
}

STDMETHODIMP CDocHostUIHandlerDispatch::GetHostInfo(
	/* [out][in] */ DWORD* pdwFlags,
	/* [out][in] */ DWORD* pdwDoubleClick)
{
	//一時的に外部IDispatchを無効にしデフォルト動作させる
	CComQIPtr<IAxWinHostWindow> pHostWindow = m_pDefaultHandler;
	pHostWindow->SetExternalUIHandler(NULL);

	DOCHOSTUIINFO	info= { sizeof (DOCHOSTUIINFO) };
	info.dwFlags		= *pdwFlags;
	info.dwDoubleClick	= *pdwDoubleClick;
	HRESULT 		hr	= m_pDefaultHandler->GetHostInfo(&info);	//デフォルト動作

	//外部IDispatchを有効にする
	pHostWindow->SetExternalUIHandler(this);

	*pdwFlags		   = info.dwFlags;
	*pdwDoubleClick    = info.dwDoubleClick;
	return hr;
}

STDMETHODIMP CDocHostUIHandlerDispatch::GetDropTarget(/* [in] */ IUnknown* pDropTarget, /* [out] */ IUnknown** ppDropTarget)
{
	CComQIPtr<IDropTarget>	spDefaultDropTarget = pDropTarget;
	m_pView->SetDefaultDropTarget(spDefaultDropTarget);
	m_pView->QueryInterface(IID_IUnknown, (void**)ppDropTarget);
	return S_OK;
}

STDMETHODIMP CDocHostUIHandlerDispatch::TranslateAccelerator(
	/* [in] */ DWORD_PTR		hWnd,
	/* [in] */ DWORD			nMessage,
	/* [in] */ DWORD_PTR		wParam,
	/* [in] */ DWORD_PTR		lParam,
	/* [in] */ BSTR 			bstrGuidCmdGroup,
	/* [in] */ DWORD			nCmdID,
	/* [retval][out] */HRESULT* dwRetVal)
{
	if (CMainOption::s_bFocusToSearchBar && ::GetKeyState(VK_CONTROL) < 0 && nMessage != WM_CHAR) {
		if (wParam == 0x46) {	//F
			g_pMainWnd->SetFocusToSearchBarWithSelectedText();
			*dwRetVal = S_OK;
			return S_OK;
		}
	}

	return S_FALSE; //IE default action
}