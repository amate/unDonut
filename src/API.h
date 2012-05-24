/**
 *	@file	API.h
 *	@brief	CAPI の宣言.
 */

#ifndef __API_H_
#define __API_H_

#include "DonutPCP.h"

#include "MDIChildUserMessenger.h"
#include "option\CloseTitleOption.h"
#include "option\DonutConfirmOption.h"
#include "dialog\aboutdlg.h"
#include "ChildFrame.h"			//#include ""ChildFrm.h"	//+++
#include "MainFrame.h"			//#include "mainfrm.h"		//+++

extern CMainFrame *g_pMainWnd;

class	CAPI;
extern	CAPI *	   g_pAPI;


#if 0

/////////////////////////////////////////////////////////////////////////////
// CAPI

class ATL_NO_VTABLE CAPI
		: public CComObjectRootEx<CComSingleThreadModel>
		, public CComCoClass<CAPI, &CLSID_API>
		, public IConnectionPointContainerImpl<CAPI>
		, public IDispatchImpl<IAPI4, &IID_IAPI4, &LIBID_DONUTPLib>
		, public CProxyIDonutPEvents< CAPI >
{
private:
	BOOL							m_bFirst;
	CSimpleArray<DWORD *>			m_aryCookie;
	//vector<CComPtr<IUnknown> >	m_vecUnk;

public:
	CAPI() : m_bFirst(TRUE) {}

	virtual ~CAPI() {
		DWORD *pdw;

		for (int i = 0; i < m_aryCookie.GetSize(); i++) {
			pdw = m_aryCookie[i];
			Unadvise(*pdw);
		}
	}


public:
	DECLARE_REGISTRY_RESOURCEID(IDR_API)

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAPI)
		COM_INTERFACE_ENTRY(IAPI)
		COM_INTERFACE_ENTRY(IAPI2)
		COM_INTERFACE_ENTRY(IAPI3)
		COM_INTERFACE_ENTRY(IAPI4)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CAPI)
		CONNECTION_POINT_ENTRY(DIID_IDonutPEvents)
	END_CONNECTION_POINT_MAP()

	// IAPI


public:

	BOOL IsConfirmScripting()
	{
	  #if 1 //+++
		return CDonutConfirmOption::WhetherConfirmScript() != 0;
	  #else
		BOOL bFlag =  CDonutConfirmOption::WhetherConfirmScript();
		return bFlag ? TRUE : FALSE;
	  #endif
	}


	static HRESULT InternalQueryInterface( void *pThis, const _ATL_INTMAP_ENTRY *pEntries, REFIID iid, void **ppvObject )
	{
		CAPI *_pThis = (CAPI *) pThis;

		if (_pThis->m_bFirst) {
			const _ATL_INTMAP_ENTRY *pEnt = pEntries;

			while (pEnt->piid) {
				if ( InlineIsEqualGUID(iid, *pEnt->piid) ) {
					_pThis->m_bFirst = FALSE;
					return CComObjectRootEx<CComSingleThreadModel>::InternalQueryInterface(pThis, pEntries, iid, ppvObject);
				}
				pEnt++;
			}

			_pThis->m_bFirst = FALSE;

			if ( _pThis->IsConfirmScripting() ) {
				//HWND hWnd = (g_pMainWnd) ? g_pMainWnd->m_hWnd : NULL;
				//int  nRet = ::MessageBox(hWnd,  _T("スクリプトまたはプラグインがDonutの機能を使用しようとしています。\n")
				//								_T("悪意のあるコードが実行される可能性があります。\n")
				//								_T("処理の続行を許可しますか？"), _T("確認"), MB_YESNO);

				if (nRet == IDYES) {
					return CComObjectRootEx<CComSingleThreadModel>::InternalQueryInterface(pThis, pEntries, iid, ppvObject);
				} else {
					return E_NOTIMPL;
				}
			}
		}

		return CComObjectRootEx<CComSingleThreadModel>::InternalQueryInterface(pThis, pEntries, iid, ppvObject);
	}


	STDMETHOD	(Advise) (IUnknown * pUnk, DWORD * pdwCookie);
	/*{
		HRESULT hr = CProxy_IDonutPEvents<CAPI>::Advise(pUnk, pdwCookie);
		if (SUCCEEDED(hr))
			g_pAPI = this;
		return hr;
	   }*/


	STDMETHOD	(Unadvise) (DWORD dwCookie);
	/*{
		HRESULT hr = CProxy_IDonutPEvents<CAPI>::Unadvise(dwCookie);
		if (SUCCEEDED(hr))
			g_pAPI = NULL;
		return hr;
	   }*/


	STDMETHOD	(GetPanelWebBrowserObject) ( /*[out, retval]*/ IDispatch * *pVal)
	{
		*pVal = NULL;
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiGetPanelWebBrowserObject();
		return S_OK;
	}


	STDMETHOD	(GetPanelWindowObject) ( /*[out, retval]*/ IDispatch * *pVal)
	{
		*pVal = NULL;
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiGetPanelWindowObject();
		return S_OK;
	}


	STDMETHOD	(GetPanelDocumentObject) ( /*[out, retval]*/ IDispatch * *pVal)
	{
		*pVal = NULL;
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiGetPanelDocumentObject();
		return S_OK;
	}


	STDMETHOD	(GetTabState) (int nIndex, /*[out, retval]*/ long *pVal)
	{
		*pVal = 0;
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiGetTabState(nIndex);
		return S_OK;
	}


	STDMETHOD	(ShowPanelBar) ()
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiShowPanelBar();
		return S_OK;
	}


	STDMETHOD	(MessageBox) (BSTR bstrText, BSTR bstrCaption, UINT uType, /*[out, retval]*/ long *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;

		CString strText(bstrText);
		CString strCaption(bstrCaption);
		*pVal = ::MessageBox(g_pMainWnd->m_hWnd, strText, strCaption, uType);
		return S_OK;
	}


	STDMETHOD	(NewWindow) (BSTR bstrURL, BOOL bActive, /*[out, retval]*/ long *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiNewWindow(bstrURL, bActive);
		return S_OK;
	}


	STDMETHOD	(MoveToTab) (WORD wBefor, WORD wAfter)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiMoveToTab(wBefor, wAfter);
		return S_OK;
	}


	STDMETHOD	(GetTabCount) ( /*[out, retval]*/ long *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiGetTabCount();
		return S_OK;
	}


	STDMETHOD	(get_TabIndex) ( /*[out, retval]*/ long *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiGetTabIndex();
		return S_OK;
	}


	STDMETHOD	(put_TabIndex) ( /*[in]*/ long newVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiSetTabIndex(newVal);
		return S_OK;
	}


	STDMETHOD	(GetWindowObject) (int nIndex, /*[out, retval]*/ IDispatch * *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiGetWindowObject(nIndex);
		return S_OK;
	}


	STDMETHOD	(GetDocumentObject) (int nIndex, /*[out, retval]*/ IDispatch * *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiGetDocumentObject(nIndex);
		return S_OK;
	}


	STDMETHOD	(GetWebBrowserObject) (int nIndex, /*[out, retval]*/ IDispatch * *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiGetWebBrowserObject(nIndex);
		return S_OK;
	}


	STDMETHOD	(Close) (int nIndex)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiClose(nIndex);
		return S_OK;
	}


	//IAPI2 by minit
	STDMETHOD	(ExecuteCommand) (int nCommand)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiExecuteCommand(nCommand);
		return S_OK;
	}


	STDMETHOD	(GetSearchText) ( /*[out, retval]*/ BSTR * pbstrText)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiGetSearchText(pbstrText);
		return S_OK;
	}


	STDMETHOD	(SetSearchText) (BSTR bstrText)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiSetSearchText(bstrText);
		return S_OK;
	}


	STDMETHOD	(GetAddressText) ( /*[out, retval]*/ BSTR * pbstrText)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiGetAddressText(pbstrText);
		return S_OK;
	}


	STDMETHOD	(SetAddressText) (BSTR bstrText)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiSetAddressText(bstrText);
		return S_OK;
	}


	STDMETHOD	(GetExtendedTabState) (int nIndex, /*[out, retval]*/ long *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = (long)g_pMainWnd->ApiGetExtendedTabState(nIndex);
		return S_OK;
	}


	STDMETHOD	(SetExtendedTabState) (int nIndex, long nState)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiSetExtendedTabState(nIndex, nState);
		return S_OK;
	}


	STDMETHOD	(GetKeyState) (int nKey, /*[out, retval]*/ long *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = (long)g_pMainWnd->ApiGetKeyState(nKey);
		return S_OK;
	}


	STDMETHOD	(GetProfileInt) (BSTR bstrFile, BSTR bstrSection, BSTR bstrKey, int nDefault, /*[out, retval]*/ long *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiGetProfileInt(bstrFile, bstrSection, bstrKey, nDefault);
		return S_OK;
	}


	STDMETHOD	(WriteProfileInt) (BSTR bstrFile, BSTR bstrSection, BSTR bstrKey, int nValue)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiWriteProfileInt(bstrFile, bstrSection, bstrKey, nValue);
		return S_OK;
	}


	STDMETHOD	(GetProfileString) (BSTR bstrFile, BSTR bstrSection, BSTR bstrKey, BSTR bstrDefault, /*[out, retval]*/ BSTR * pbstrText)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiGetProfileString(bstrFile, bstrSection, bstrKey, bstrDefault, pbstrText);
		return S_OK;
	}


	STDMETHOD	(WriteProfileString) (BSTR bstrFile, BSTR bstrSection, BSTR bstrKey, BSTR bstrText)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiWriteProfileString(bstrFile, bstrSection, bstrKey, bstrText);
		return S_OK;
	}


	STDMETHOD	(GetScriptFolder) ( /*[out, retval]*/ BSTR * pbstrFolder)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiGetScriptFolder(pbstrFolder);
		return S_OK;
	}


	STDMETHOD	(GetCSSFolder) ( /*[out, retval]*/ BSTR * pbstrFolder)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiGetCSSFolder(pbstrFolder);
		return S_OK;
	}


	STDMETHOD	(GetBaseFolder) ( /*[out, retval]*/ BSTR * pbstrFolder)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiGetBaseFolder(pbstrFolder);
		return S_OK;
	}


	STDMETHOD	(GetExePath) ( /*[out, retval]*/ BSTR * pbstrPath)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiGetExePath(pbstrPath);
		return S_OK;
	}


	STDMETHOD	(SetStyleSheet) (int nIndex, BSTR bstrStyleSheet, BOOL bOff)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiSetStyleSheet(nIndex, bstrStyleSheet, bOff);
		return S_OK;
	}


	//IAPI3
	STDMETHOD	(SaveGroup) (BSTR bstrGroupFile)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiSaveGroup(bstrGroupFile);
		return S_OK;
	}


	STDMETHOD	(LoadGroup) (BSTR bstrGroupFile, BOOL bClose)
	{
		if (!g_pMainWnd)
			return S_OK;
		g_pMainWnd->ApiLoadGroup(bstrGroupFile, bClose);
		return S_OK;
	}


	STDMETHOD	(EncryptString) (BSTR bstrString, BSTR bstrPass, BSTR * bstrRet)
	{
		return E_NOTIMPL;	//未実装
	}


	STDMETHOD	(DecryptString) (BSTR bstrString, BSTR * bstrRet)
	{
		return E_NOTIMPL;	//未実装
	}


	STDMETHOD	(InputBox) (BSTR bstrTitle, BSTR bstrDescript, BSTR bstrDefault, int nFlag, long *pVal)
	{
		return E_NOTIMPL;	//未実装
	}


	STDMETHOD	(NewWindow3) (BSTR bstrURL, BOOL bActive, long ExStyle, const int *pHistInfo, long *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiNewWindow3(bstrURL, bActive, ExStyle, (void *) pHistInfo);
		return S_OK;
	}


	STDMETHOD	(AddGroupItem) (BSTR bstrGroupFile, int nIndex, long *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiAddGroupItem(bstrGroupFile, nIndex);
		return S_OK;
	}


	STDMETHOD	(DeleteGroupItem) (BSTR bstrGroupFile, int nIndex, long *pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = g_pMainWnd->ApiDeleteGroupItem(bstrGroupFile, nIndex);
		return S_OK;
	}


	//IAPI4
	STDMETHOD	(GetHWND) (long nType, LONG_PTR * pVal)
	{
		if (!g_pMainWnd)
			return S_OK;
		*pVal = (LONG_PTR) g_pMainWnd->ApiGetHWND(nType);
		return S_OK;
	}

};

#endif

#endif	//__API_H_
