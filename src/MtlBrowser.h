/**
 *	@file	MtlBrowser.h
 *	@brief	MTL : ブラウザ関係
 */
////////////////////////////////////////////////////////////////////////////
// MTL Version 0.10
// Copyright (C) 2001 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// MtlBrowser.h: Last updated: March 17, 2001
/////////////////////////////////////////////////////////////////////////////

#ifndef __MTLBROWSER_H__
#define __MTLBROWSER_H__

#pragma once
#include <tlogstg.h>
#include "MtlCom.h"
#include "resource.h"	//+++
#include <ExDispid.h>

namespace MTL {


/////////////////////////////////////////////////////////////////////////////

// IWebBrowser2 wrapper
class CWebBrowser2 {
public:
	CComPtr<IWebBrowser2>	m_spBrowser;

public:
	// Ctor
	CWebBrowser2(IWebBrowser2 * pBrowser = NULL) : m_spBrowser(pBrowser)
	{
	}

	bool IsBrowserNull() { return (m_spBrowser.p == NULL); }


	// properties
	void SetRegisterAsBrowser(bool bNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_RegisterAsBrowser(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
	}


	void SetRegisterAsDropTarget(bool bNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_RegisterAsDropTarget(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
	}


private:
	void SetTheaterMode(bool bNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_TheaterMode(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
	}


public:
	void SetVisible(bool bNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_Visible(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
	}


//private:
	void SetMenuBar(bool bNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_MenuBar(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
	}


	void SetToolBar(int nNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_ToolBar(nNewValue);
	}


	void SetOffline(bool bNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_Offline(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
	}


	void SetSilent(bool bNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_Silent(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
	}


public:
	void GoBack()
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->GoBack();
	}

	void GoForward()
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->GoForward();
	}

	void GoHome()
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->GoHome();
	}


private:
	void GoSearch()
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->GoSearch();
	}


public:
	void Refresh()
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->Refresh();
	}

	void Refresh2(int nLevel)
	{
		ATLASSERT(m_spBrowser != NULL);
		CComVariant v( (long) nLevel );
		m_spBrowser->Refresh2(&v);
	}


	void Stop()
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->Stop();
	}


private:
	void SetFullScreen(bool bNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_FullScreen(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
	}


	void SetAddressBar(bool bNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_AddressBar(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
	}


	void SetHeight(long nNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_Height(nNewValue);
	}


	void PutProperty(LPCTSTR lpszPropertyName, long lValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		PutProperty( lpszPropertyName, CComVariant(lValue) );
	}

	void PutProperty(LPCTSTR lpszPropertyName, short nValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		PutProperty( lpszPropertyName, CComVariant(nValue) );
	}

	void PutProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		PutProperty( lpszPropertyName, CComVariant(lpszValue) );
	}

	void PutProperty(LPCTSTR lpszPropertyName, double dValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		PutProperty( lpszPropertyName, CComVariant(dValue) );
	}


	void SetTop(long nNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_Top(nNewValue);
	}


	void SetLeft(long nNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_Left(nNewValue);
	}


	void SetStatusBar(bool bNewValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		m_spBrowser->put_StatusBar(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
	}


	const CString GetType() const
	{
		ATLASSERT(m_spBrowser != NULL);

		CComBSTR bstr;
		m_spBrowser->get_Type(&bstr);
	  #ifdef NDEBUG 	//+++
		return CString(bstr);
	  #else
		CString retVal(bstr);
		return retVal;
	  #endif
	}


	long GetLeft() const
	{
		ATLASSERT(m_spBrowser != NULL);

		long result = 0;
		m_spBrowser->get_Left(&result);
		return result;
	}


	long GetTop() const
	{
		ATLASSERT(m_spBrowser != NULL);
		long result = 0;
		m_spBrowser->get_Top(&result);
		return result;
	}


	int GetToolBar() const
	{
		ATLASSERT(m_spBrowser != NULL);
		int 	result = 0;
		m_spBrowser->get_ToolBar(&result);
		return result;
	}


	long GetHeight() const
	{
		ATLASSERT(m_spBrowser != NULL);

		long	result = 0;
		m_spBrowser->get_Height(&result);
		return result;
	}


	bool GetVisible() const
	{
		ATLASSERT(m_spBrowser != NULL);

		VARIANT_BOOL result = VARIANT_FALSE;
		m_spBrowser->get_Visible(&result);
		return (result == VARIANT_TRUE);
	}


public:
	const CString GetLocationName() const
	{
		ATLASSERT(m_spBrowser != NULL);

		CComBSTR	bstr;
		m_spBrowser->get_LocationName(&bstr);
	  #ifdef NDEBUG 	//+++
		return CString(bstr);
	  #else
		CString 	retVal(bstr);
		return retVal;
	  #endif
	}


	const CString GetLocationURL() const
	{
		ATLASSERT(m_spBrowser != NULL);

		CComBSTR	bstr;
		m_spBrowser->get_LocationURL(&bstr);
	  #ifdef NDEBUG 	//+++
		return CString(bstr);
	  #else
		CString 	retVal(bstr);
		return retVal;
	  #endif
	}


private:
	bool GetBusy() const
	{
		ATLASSERT(m_spBrowser != NULL);

		VARIANT_BOOL result = VARIANT_FALSE;
		m_spBrowser->get_Busy(&result);
		return (result == VARIANT_TRUE);
	}


	READYSTATE GetReadyState() const
	{
		ATLASSERT(m_spBrowser != NULL);

		READYSTATE result = READYSTATE_UNINITIALIZED;
		m_spBrowser->get_ReadyState(&result);
		return result;
	}


	bool GetOffline() const
	{
		ATLASSERT(m_spBrowser != NULL);

		VARIANT_BOOL result = VARIANT_FALSE;
		m_spBrowser->get_Offline(&result);
		return (result == VARIANT_TRUE);
	}


	bool GetSilent() const
	{
		ATLASSERT(m_spBrowser != NULL);

		VARIANT_BOOL result = VARIANT_FALSE;
		m_spBrowser->get_Silent(&result);
		return (result == VARIANT_TRUE);
	}


	IDispatch *GetApplication() const
	{
		ATLASSERT(m_spBrowser != NULL);

		IDispatch *result = NULL;
		m_spBrowser->get_Application(&result);
		return result;
	}


	IDispatch *GetParentBrowser() const
	{
		ATLASSERT(m_spBrowser != NULL);

		IDispatch *result = NULL;
		m_spBrowser->get_Parent(&result);
		return result;
	}


	IDispatch *GetContainer() const
	{
		ATLASSERT(m_spBrowser != NULL);

		IDispatch *result = NULL;
		m_spBrowser->get_Container(&result);
		return result;
	}


	IDispatch *GetHtmlDocument() const
	{
		ATLASSERT(m_spBrowser != NULL);

		IDispatch *result = NULL;
		m_spBrowser->get_Document(&result);
		return result;
	}


	bool GetTopLevelContainer() const
	{
		ATLASSERT(m_spBrowser != NULL);

		VARIANT_BOOL result = VARIANT_FALSE;
		m_spBrowser->get_TopLevelContainer(&result);
		return (result == VARIANT_TRUE);
	}


	bool GetMenuBar() const
	{
		ATLASSERT(m_spBrowser != NULL);

		VARIANT_BOOL result = VARIANT_FALSE;
		m_spBrowser->get_MenuBar(&result);
		return (result == VARIANT_TRUE);
	}


	bool GetFullScreen() const
	{
		ATLASSERT(m_spBrowser != NULL);

		VARIANT_BOOL result = VARIANT_FALSE;
		m_spBrowser->get_FullScreen(&result);
		return (result == VARIANT_TRUE);
	}


	bool GetStatusBar() const
	{
		ATLASSERT(m_spBrowser != NULL);

		VARIANT_BOOL result = VARIANT_FALSE;
		m_spBrowser->get_StatusBar(&result);
		return (result == VARIANT_TRUE);
	}


public:
	OLECMDF QueryStatusWB(OLECMDID cmdID) const
	{
		ATLASSERT(m_spBrowser != NULL);

		OLECMDF result = OLECMDF(0);		//+++ 初期化抜け...
		m_spBrowser->QueryStatusWB(cmdID, &result);
		return result;
	}


	void ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt,
				VARIANT *pvaIn, VARIANT *pvaOut)
	{
		ATLASSERT(m_spBrowser != NULL);

		m_spBrowser->ExecWB(cmdID, cmdexecopt, pvaIn, pvaOut);
	}


private:
	bool GetRegisterAsBrowser() const
	{
		ATLASSERT(m_spBrowser != NULL);

		VARIANT_BOOL result = VARIANT_FALSE;
		m_spBrowser->get_RegisterAsBrowser(&result);
		return (result == VARIANT_TRUE);
	}


public:
	bool GetRegisterAsDropTarget() const
	{
		ATLASSERT(m_spBrowser != NULL);

		VARIANT_BOOL result = VARIANT_FALSE;
		m_spBrowser->get_RegisterAsDropTarget(&result);
		return (result == VARIANT_TRUE);
	}


private:
	bool GetTheaterMode() const
	{
		ATLASSERT(m_spBrowser != NULL);

		VARIANT_BOOL result = VARIANT_FALSE;
		m_spBrowser->get_TheaterMode(&result);
		return (result == VARIANT_TRUE);
	}


	bool GetAddressBar() const
	{
		ATLASSERT(m_spBrowser != NULL);

		VARIANT_BOOL result = VARIANT_FALSE;
		m_spBrowser->get_AddressBar(&result);
		return (result == VARIANT_TRUE);
	}


  #if 0 //+++ 未使用のよう?
	// operations
	bool LoadFromResource(LPCTSTR lpszResource)
	{
	  #if 1 //+++
		CString str = GetExeFileName();
		if (str.IsEmpty())
			return false;
		CString   strResourceURL;
		strResourceURL.Format(_T("res://%s/%s"), LPCTSTR(str), lpszResource);
		Navigate(strResourceURL, 0, 0, 0);
		return true;
	  #else
		HINSTANCE hInstance  = _Module.GetModuleInstance();

		ATLASSERT(hInstance != NULL);

		CString   strResourceURL;
		bool	  bRetVal	 = true;
		LPTSTR	  lpszModule = new TCHAR[_MAX_PATH];

		if ( ::GetModuleFileName(hInstance, lpszModule, _MAX_PATH) ) {
			strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
			Navigate(strResourceURL, 0, 0, 0);
		} else
			bRetVal = false;

		delete[] lpszModule;
		return bRetVal;
	  #endif
	}


	bool LoadFromResource(UINT nRes)
	{
	  #if 1 //+++
		CString str = GetExeFileName();
		if (str.IsEmpty())
			return false;
		CString   strResourceURL;
		strResourceURL.Format(_T("res://%s/%d"), LPCTSTR(str), nRes);
		Navigate(strResourceURL, 0, 0, 0);
		return true;
	  #else
		HINSTANCE hInstance  = _Module.GetModuleInstance();

		ATLASSERT(hInstance != NULL);

		CString   strResourceURL;
		bool	  bRetVal	 = true;
		LPTSTR	  lpszModule = new TCHAR[_MAX_PATH];

		if ( ::GetModuleFileName(hInstance, lpszModule, _MAX_PATH) ) {
			strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
			Navigate(strResourceURL, 0, 0, 0);
		} else
			bRetVal = false;

		delete[] lpszModule;
		return bRetVal;
	  #endif
	}
  #endif


	void Navigate(LPCTSTR	lpszURL,
				  DWORD 	dwFlags 			= 0 ,
				  LPCTSTR	lpszTargetFrameName = NULL,
				  LPCTSTR	lpszHeaders 		= NULL,
				  LPVOID	lpvPostData 		= NULL,
				  DWORD 	dwPostDataLen		= 0)
	{
		CComBSTR bstrURL(lpszURL);

		CComSafeArray	vPostData;

		if (lpvPostData != NULL) {
			if (dwPostDataLen == 0)
				dwPostDataLen = ::lstrlen( (LPCTSTR) lpvPostData );

			vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
		}

		CComVariant 	vHeaders;
		CComVariant 	vTargetFrameName;
		CComVariant 	vFlags( (long) dwFlags );

		// 
		if (lpszHeaders)
			vHeaders = lpszHeaders;

		if (lpszTargetFrameName)
			vTargetFrameName = lpszTargetFrameName;

		m_spBrowser->Navigate(bstrURL, &vFlags, &vTargetFrameName, vPostData, &vHeaders);
	}


#if 0	//+++ 未使用ぽい
	void Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags = 0, LPCTSTR lpszTargetFrameName = NULL)
	{
		ATLASSERT(m_spBrowser != NULL);

		CComVariant 	vPIDL;
		MtlInitVariantFromItemIDList(vPIDL, pIDL);
		CComVariant 	empty;
		CComVariant 	vTargetFrameName(lpszTargetFrameName);
		CComVariant 	vFlags( (long) dwFlags );

		m_spBrowser->Navigate2(&vPIDL, &vFlags, &vTargetFrameName, &empty, &empty);
	}
#endif


public:
	void Navigate2(
			LPCTSTR 	lpszURL,
			DWORD		dwFlags 			= 0,
			LPCTSTR 	lpszTargetFrameName = NULL,
			LPCTSTR 	lpszHeaders 		= NULL,
			LPVOID		lpvPostData 		= NULL,
			DWORD		dwPostDataLen		= 0)
	{
		ATLASSERT(m_spBrowser != NULL);

		CComSafeArray vPostData;

		if (lpvPostData != NULL) {
			if (dwPostDataLen == 0)
				dwPostDataLen = lstrlen( (LPCTSTR) lpvPostData );

			vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
		}

		CComVariant   vURL;
		CComVariant   vHeaders;
		CComVariant   vTargetFrameName;
		CComVariant   vFlags( (long) dwFlags );

		// for stupid Mizilla
		if (lpszURL)
			vURL = lpszURL;

		if (lpszHeaders) 
			vHeaders = lpszHeaders;

		if (lpszTargetFrameName)
			vTargetFrameName = lpszTargetFrameName;

		try {
			HRESULT hr = m_spBrowser->Navigate2(&vURL, &vFlags, &vTargetFrameName, &vPostData, &vHeaders);
			ATLASSERT(hr == S_OK);
		} catch (...) {
			ATLASSERT(0);
		}
	}


private:
#if 0	//+++ 未使用ぽい
	void Navigate2(
			LPCTSTR 			lpszURL,
			DWORD				dwFlags,
			CSimpleArray<BYTE>& baPostData,
			LPCTSTR 			lpszTargetFrameName = NULL,
			LPCTSTR 			lpszHeaders 		= NULL)
	{
		ATLASSERT(m_spBrowser != NULL);

		CComVariant 	vPostData;
		MtlInitVariantFromArray(vPostData, baPostData);
		CComVariant 	vURL;
		CComVariant 	vHeaders;
		CComVariant 	vTargetFrameName;
		CComVariant 	vFlags( (long) dwFlags );

		// for stupid Mozilla
		if (lpszURL)
			vURL = lpszURL;

		if (lpszHeaders)
			vHeaders = lpszHeaders;

		if (lpszTargetFrameName)
			vTargetFrameName = lpszTargetFrameName;

		HRESULT hr = m_spBrowser->Navigate2(&vURL, &vFlags, &vTargetFrameName, &vPostData, &vHeaders);
		ATLASSERT(hr == S_OK);
	}
#endif


	void PutProperty(LPCTSTR lpszProperty, const VARIANT &vtValue)
	{
		ATLASSERT(m_spBrowser != NULL);
		CComBSTR	bstrProp(lpszProperty);
		m_spBrowser->PutProperty(bstrProp, vtValue);
	}


	bool GetProperty(LPCTSTR lpszProperty, CString &strValue)
	{
		ATLASSERT(m_spBrowser != NULL);

		CComBSTR	bstrProperty(lpszProperty);
		bool		bResult = false;
		CComVariant vReturn;
		vReturn.vt		= VT_BSTR;
		vReturn.bstrVal = NULL;

		HRESULT hr		= m_spBrowser->GetProperty(bstrProperty, &vReturn);
		if ( SUCCEEDED(hr) ) {
			strValue = CString(vReturn.bstrVal);
			bResult  = true;
		}

		return bResult;
	}


  #if 0 //+++ 未使用ぽい
	CComVariant GetProperty(LPCTSTR lpszProperty)
	{
		ATLASSERT(m_spBrowser != NULL);

		CComVariant 		 result;
		CComQIPtr<IDispatch> spDisp = m_spBrowser;

		CComDispatchDriver::GetProperty(spDisp, 0x12f, &result);
		return result;
		//		static BYTE parms[] = VTS_BSTR;
		//		m_wndBrowser.InvokeHelper(0x12f, DISPATCH_METHOD,
		//			VT_VARIANT, (void*)&result, parms, lpszProperty);
	}
  #endif


  #if 0 //+++ 未使用ぽい
	CString GetFullName() const
	{
		ATLASSERT(m_spBrowser != NULL);

		CComBSTR bstr;
		m_spBrowser->get_FullName(&bstr);
	  #ifdef NDEBUG 	//+++
		return CString(bstr);
	  #else
		CString retVal(bstr);
		return retVal;
	  #endif
	}
  #endif


public:
	void MenuChgGoBack(HMENU hMenu)
	{
		CMenuHandle 		 menu	  = hMenu; // handle

		HRESULT 			 hr 	  = S_OK;
		IServiceProvider *	 pISP	  = NULL;
		ITravelLogStg * 	 pTLStg   = NULL;
		ITravelLogEntry *	 pTLEntry = NULL;
		IEnumTravelLogEntry *pTLEnum  = NULL;

		if ( SUCCEEDED( m_spBrowser->QueryInterface(IID_IServiceProvider, (void **) &pISP) ) ) {
			if ( SUCCEEDED( pISP->QueryService(SID_STravelLogCursor, IID_ITravelLogStg, (void **) &pTLStg) ) ) {
				if (SUCCEEDED( pTLStg->EnumEntries(TLEF_RELATIVE_BACK, &pTLEnum) ) && pTLEnum) {
					hr = pTLEnum->Next(1, &pTLEntry, NULL);

					int nIndex = 0;
					while (hr != S_FALSE) {
						LPOLESTR szTitle = NULL;

						if (SUCCEEDED( pTLEntry->GetTitle(&szTitle) ) && szTitle) {
							CString strTitle(szTitle);
							menu.ModifyMenu(ID_VIEW_BACK1 + nIndex, MF_BYCOMMAND, ID_VIEW_BACK1 + nIndex, strTitle);
							menu.EnableMenuItem(ID_VIEW_BACK1 + nIndex, MF_BYCOMMAND | MF_ENABLED);
							nIndex++;
						}

						pTLEntry->Release();
						pTLEntry = NULL;

						if (nIndex >= 9)
							break;

						hr		 = pTLEnum->Next(1, &pTLEntry, NULL);
					}

					if (nIndex < 9) {
						for (int ii = nIndex; ii < 9; ii++)
							menu.RemoveMenu(ID_VIEW_BACK1 + ii, MF_BYCOMMAND);
					}
				}
			}
		}

		if (pTLStg)
			pTLStg->Release();

		if (pTLEnum)
			pTLEnum->Release();

		menu.Detach();
	}


	void MenuChgGoForward(HMENU hMenu)
	{
		CMenuHandle 		 menu	  = hMenu; // handle

		HRESULT 			 hr 	  = S_OK;
		IServiceProvider *	 pISP	  = NULL;
		ITravelLogStg * 	 pTLStg   = NULL;
		ITravelLogEntry *	 pTLEntry = NULL;
		IEnumTravelLogEntry *pTLEnum  = NULL;

		if ( SUCCEEDED( m_spBrowser->QueryInterface(IID_IServiceProvider, (void **) &pISP) ) ) {
			if ( SUCCEEDED( pISP->QueryService(SID_STravelLogCursor, IID_ITravelLogStg, (void **) &pTLStg) ) ) {
				if (SUCCEEDED( pTLStg->EnumEntries(TLEF_RELATIVE_FORE, &pTLEnum) ) && pTLEnum) {
					hr = pTLEnum->Next(1, &pTLEntry, NULL);
					int nIndex = 0;
					while (hr != S_FALSE) {
						LPOLESTR szTitle = NULL;

						if (SUCCEEDED( pTLEntry->GetTitle(&szTitle) ) && szTitle) {
							CString strTitle(szTitle);
							menu.ModifyMenu(ID_VIEW_FORWARD1 + nIndex, MF_BYCOMMAND, ID_VIEW_FORWARD1 + nIndex, strTitle);
							menu.EnableMenuItem(ID_VIEW_FORWARD1 + nIndex, MF_BYCOMMAND | MF_ENABLED);
							nIndex++;
						}

						pTLEntry->Release();
						pTLEntry = NULL;

						if (nIndex >= 9)
							break;

						hr		 = pTLEnum->Next(1, &pTLEntry, NULL);
					}

					if (nIndex < 9) {
						for (int ii = nIndex; ii < 9; ii++)
							menu.RemoveMenu(ID_VIEW_FORWARD1 + ii, MF_BYCOMMAND);
					}
				}
			}
		}

		if (pTLStg)
			pTLStg->Release();

		if (pTLEnum)
			pTLEnum->Release();

		menu.Detach();
	}
};



/////////////////////////////////////////////////////////////////////////////
// IWebBrowserEvents2
class IWebBrowserEvents2Base {
public:
	static _ATL_FUNC_INFO	StatusTextChangeInfo;
	static _ATL_FUNC_INFO	DownloadBeginInfo ;
	static _ATL_FUNC_INFO	NewWindow2Info;
	static _ATL_FUNC_INFO	CommandStateChangeInfo;
	static _ATL_FUNC_INFO	BeforeNavigate2Info;
	static _ATL_FUNC_INFO	ProgressChangeInfo;
	static _ATL_FUNC_INFO	NavigateComlete2Info;
	static _ATL_FUNC_INFO	OnVisibleInfo;
	static _ATL_FUNC_INFO	OnSecureLockIcon;
	static _ATL_FUNC_INFO	PrivacyImpactedStateChange;
	static _ATL_FUNC_INFO	FileDownloadInfo;
	static _ATL_FUNC_INFO	NewWindow3Info;
	static _ATL_FUNC_INFO	WindowClosingInfo;
	static _ATL_FUNC_INFO	RedirectXDomainBlocked;
};



__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::StatusTextChangeInfo		= { CC_STDCALL, VT_EMPTY, 1, { VT_BSTR } };
__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::DownloadBeginInfo			= { CC_STDCALL, VT_EMPTY, 0, { NULL } };
__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::NewWindow2Info 			= { CC_STDCALL, VT_EMPTY, 2, { VT_BYREF | VT_BOOL, VT_BYREF | VT_DISPATCH } };
__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::CommandStateChangeInfo 	= { CC_STDCALL, VT_EMPTY, 2, { VT_I4, VT_BOOL } };
__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::BeforeNavigate2Info		= { CC_STDCALL, VT_EMPTY, 7, { VT_DISPATCH, VT_BYREF|VT_VARIANT, VT_BYREF|VT_VARIANT, VT_BYREF|VT_VARIANT, VT_BYREF|VT_VARIANT, VT_BYREF|VT_VARIANT, VT_BYREF|VT_BOOL } };
__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::ProgressChangeInfo 		= { CC_STDCALL, VT_EMPTY, 2, { VT_I4, VT_I4 } };
__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::NavigateComlete2Info		= { CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH, VT_BYREF | VT_VARIANT } };
__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::OnVisibleInfo				= { CC_STDCALL, VT_EMPTY, 1, { VT_BOOL } };
__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::OnSecureLockIcon			= { CC_STDCALL, VT_EMPTY, 1, { VT_I4 } };
__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::PrivacyImpactedStateChange = { CC_STDCALL, VT_EMPTY, 1, { VT_BOOL } };
__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::FileDownloadInfo			= { CC_STDCALL, VT_EMPTY, 2, { VT_BOOL, VT_BYREF | VT_BOOL} };
__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::NewWindow3Info 			= { CC_STDCALL, VT_EMPTY, 5, { VT_BYREF | VT_DISPATCH, VT_BYREF | VT_BOOL, VT_VARIANT, VT_BSTR, VT_BSTR } };
__declspec(selectany) _ATL_FUNC_INFO IWebBrowserEvents2Base::WindowClosingInfo			= { CC_STDCALL, VT_EMPTY, 2, { VT_BOOL, VT_BYREF | VT_BOOL} };


template <class T, UINT nID>
class IWebBrowserEvents2Impl
	: public IDispEventSimpleImpl<nID, IWebBrowserEvents2Impl< T, nID >, &DIID_DWebBrowserEvents2>
	, public IWebBrowserEvents2Base
{
	typedef IWebBrowserEvents2Impl<T, nID>	thisClass;

public:
	// Constructor
	IWebBrowserEvents2Impl()
		: m_nDownloadCounter(0)
	{ }


	// Methods
	void WebBrowserEvents2Advise()
	{
		T *pT = static_cast<T *>(this);
		DispEventAdvise(pT->m_spBrowser, &DIID_DWebBrowserEvents2);
	}


	void WebBrowserEvents2Unadvise()
	{
		T *pT = static_cast<T *>(this);
		DispEventUnadvise(pT->m_spBrowser, &DIID_DWebBrowserEvents2);
	}


	bool IsPageIWebBrowser(IDispatch *pDisp)
	{								//Knowledge Base Q180366
		T * 			   pT = static_cast<T *>(this);
		CComPtr<IDispatch> spDispatch;

		HRESULT 		   hr = pT->m_spBrowser->QueryInterface(IID_IDispatch, (void **) &spDispatch);
		if ( FAILED(hr) )
			return false;

		return (pDisp == spDispatch.p); 	// Top-level Window object
		//		return pT->m_spBrowser.IsEqualObject(pDisp); I guess this is right...
	}


	static bool IsRefreshBeforeNavigate2(IDispatch *pDisp)
	{								// Implemented by fub, thanks.
		CComQIPtr<IWebBrowser2> spBrowser = pDisp;
		if (!spBrowser)
			return false;

		CComBSTR				bstr;
		spBrowser->get_LocationURL(&bstr);

		CString str(bstr);
		return	str.IsEmpty() || str == _T("about:blank");
	}


private:
	// Overridables
	void OnStatusTextChange(const CString &strText) 		{}
	void OnProgressChange(long progress, long progressMax)	{}
	void OnCommandStateChange(long Command, bool bEnable)	{}
	void OnDownloadBegin()		{}
	void OnDownloadComplete()	{}
	void OnTitleChange(const CString &strTitle) 	{}
	void OnNavigateComplete2(IDispatch *pDisp, const CString &strURL) {}

	void OnBeforeNavigate2(
			IDispatch * 		pDisp,
			const CString & 	strURL,
			DWORD				nFlags,
			const CString & 	strTargetFrameName,
			CSimpleArray<BYTE>& baPostedData,
			const CString & 	strHeaders,
			bool &				bCancel)
	{
	}

	void OnPropertyChange(const CString &strProperty) { }
	void OnNewWindow2(IDispatch **ppDisp, bool &bCancel) { }
	void OnDocumentComplete(IDispatch *pDisp, const CString &strURL) { }
	void OnQuit() { }
	void OnVisible(bool bVisible) { }
	void OnToolBar(bool bToolBar) { }
	void OnMenuBar(bool bMenuBar) { }
	void OnStatusBar(bool bStatusBar) { }
	void OnFullScreen(bool bFullScreen) { }
	void OnTheaterMode(bool bTheaterMode) { }
	void OnSetSecureLockIcon(long SecureLockIcon) { }
	void OnPrivacyImpactedStateChange(bool bPrivacyImpacted) { }
	void OnFileDownload(bool bActiveDocument, bool& bCancel) { }
	void OnNewWindow3(IDispatch **ppDisp, bool& Cancel, DWORD dwFlags, BSTR bstrUrlContext,  BSTR bstrUrl) { }
	void OnWindowClosing(bool IsChildWindow, bool& bCancel) { }

public:
	// For downloading state
	int 			m_nDownloadCounter;

private:
	void OnStateConnecting()  { }
	void OnStateDownloading() { }
	void OnStateCompleted()   { }

	virtual void PreDocumentComplete( /*[in]*/ IDispatch *pDisp, /*[in]*/ VARIANT *URL) { };


public:
	// Sink map and event handlers
	BEGIN_SINK_MAP(thisClass)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_STATUSTEXTCHANGE	, &__StatusTextChange	, &StatusTextChangeInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_PROGRESSCHANGE		, &__ProgressChange		, &ProgressChangeInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_COMMANDSTATECHANGE	, &__CommandStateChange , &CommandStateChangeInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_DOWNLOADBEGIN		, &__DownloadBegin 		, &DownloadBeginInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_DOWNLOADCOMPLETE	, &__DownloadComplete	, &DownloadBeginInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_TITLECHANGE		, &__TitleChange		, &StatusTextChangeInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2	, &__NavigateComplete2	, &NavigateComlete2Info)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2	, &__BeforeNavigate2	, &BeforeNavigate2Info)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_PROPERTYCHANGE		, &__PropertyChange		, &StatusTextChangeInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW2			, &__NewWindow2			, &NewWindow2Info)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE	, &__DocumentComplete	, &NavigateComlete2Info)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_ONQUIT				, &__OnQuit				, &DownloadBeginInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_ONVISIBLE			, &__OnVisible 			, &OnVisibleInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_ONTOOLBAR			, &__OnToolBar 			, &OnVisibleInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_ONMENUBAR			, &__OnMenuBar 			, &OnVisibleInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_ONSTATUSBAR		, &__OnStatusBar		, &OnVisibleInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_ONFULLSCREEN		, &__OnFullScreen		, &OnVisibleInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_ONTHEATERMODE		, &__OnTheaterMode 		, &OnVisibleInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_SETSECURELOCKICON	, &__OnSetSecureLockIcon , &OnSecureLockIcon)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_PRIVACYIMPACTEDSTATECHANGE, &__OnPrivacyImpactedStateChange, &PrivacyImpactedStateChange)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_FILEDOWNLOAD		, &__OnFileDownload		, &FileDownloadInfo)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW3			, &__OnNewWindow3		, &NewWindow3Info)
		SINK_ENTRY_INFO(nID, DIID_DWebBrowserEvents2, DISPID_WINDOWCLOSING		, &__OnWindowClosing	, &WindowClosingInfo)	
	END_SINK_MAP()


private:
	void __stdcall __StatusTextChange( /*[in]*/ BSTR bstrText)
	{
		T *pT = static_cast<T *>(this);
		CString strStatusBar(bstrText);
		pT->OnStatusTextChange(strStatusBar);
	}


	void __stdcall __ProgressChange( /*[in]*/ long progress, /*[in]*/ long progressMax)
	{
		T *pT = static_cast<T *>(this);
		pT->OnProgressChange(progress, progressMax);
	}


	void __stdcall __CommandStateChange( /*[in]*/ long Command, /*[in]*/ VARIANT_BOOL Enable)
	{
		T *  pT 	 = static_cast<T *>(this);
		bool bEnable = (Enable == VARIANT_TRUE) /*? true : false*/;
		pT->OnCommandStateChange(Command, bEnable);
	}


	void __stdcall __DownloadBegin()
	{
		T *pT = static_cast<T *>(this);
		pT->OnDownloadBegin();
		if (m_nDownloadCounter++ == 0)
			pT->OnStateConnecting();		// yes, connecting
	}


	void __stdcall __DownloadComplete()
	{
		T *pT = static_cast<T *>(this);
		pT->OnDownloadComplete();
		if (--m_nDownloadCounter == 0)
			pT->OnStateCompleted(); 	// complete!
	}


	void __stdcall __TitleChange( /*[in]*/ BSTR bstrText)
	{
		CString 	strTitle(bstrText);
		T *pT = static_cast<T *>(this);
		pT->OnTitleChange(strTitle);
	}


	void __stdcall __NavigateComplete2( /*[in]*/ IDispatch *pDisp, /*[in]*/ VARIANT *URL)
	{
		ATLASSERT(V_VT(URL) == VT_BSTR);

		CString strURL = V_BSTR(URL);
		T * 	pT	   = static_cast<T *>(this);
		pT->OnNavigateComplete2(pDisp, strURL);

		if (m_nDownloadCounter > 0)
			pT->OnStateDownloading();
	}

#if 1	//+++ 実験 Navigate2を開始してから、BeforeNavigate2が呼ばれるまでの間を検出するためのフラグを用意.
private:
volatile bool m_bBeforeNavigate2;
public:
	bool IsWaitBeforeNavigate2Flag() { return m_bBeforeNavigate2; }
	void SetWaitBeforeNavigate2Flag() { m_bBeforeNavigate2 = 1; }
	bool WaitBeforeNavigate2() {
		if (m_bBeforeNavigate2) {
			for (int i = 0; m_bBeforeNavigate2 && i < 100; ++i) {
				Sleep(10);
			}
		}
		bool rc = m_bBeforeNavigate2;
		m_bBeforeNavigate2 = 0;
		return rc;
	}
#endif

	void __stdcall __BeforeNavigate2(
			/*[in]*/ IDispatch *	pDisp,
			/*[in]*/ VARIANT *		URL,
			/*[in]*/ VARIANT *		Flags,
			/*[in]*/ VARIANT *		TargetFrameName,
			/*[in]*/ VARIANT *		PostData,
			/*[in]*/ VARIANT *		Headers,
			/*[out]*/VARIANT_BOOL*	Cancel)
	{
		ATLASSERT(V_VT(URL) == VT_BSTR);
		ATLASSERT(V_VT(TargetFrameName) == VT_BSTR);
		ATLASSERT( V_VT(PostData) == (VT_VARIANT | VT_BYREF) );
		ATLASSERT(V_VT(Headers) == VT_BSTR);
		ATLASSERT(Cancel != NULL);

		VARIANT *		   vtPostedData = V_VARIANTREF(PostData);
		CSimpleArray<BYTE> array;

		if (V_VT(vtPostedData) & VT_ARRAY) {
			// must be a vector of bytes
			ATLASSERT(vtPostedData->parray->cDims == 1 && vtPostedData->parray->cbElements == 1);

			vtPostedData->vt |= VT_UI1;
			CComSafeArray safe(vtPostedData);

			DWORD  dwSize = safe.GetOneDimSize();
			LPVOID pVoid;
			safe.AccessData(&pVoid);

			MtlSetSimpleArraySize(array, dwSize);
			LPBYTE lpByte = array.GetData();

			::memcpy(lpByte, pVoid, dwSize);
			safe.UnaccessData();
		}

		CString strTargetFrameName( V_BSTR (TargetFrameName) );

		CString 		   strURL		= V_BSTR(URL);
		CString 		   strHeaders	= V_BSTR(Headers);
		DWORD			   nFlags		= V_I4(Flags);
		bool			   bCancel		= false;
		T * 			   pT			= static_cast<T *>(this);
		pT->OnBeforeNavigate2(pDisp, strURL, nFlags, strTargetFrameName, array, strHeaders, bCancel);

		*Cancel = bCancel ? VARIANT_TRUE : VARIANT_FALSE;
		if (!bCancel) {
			if ( IsPageIWebBrowser(pDisp) )
				if (m_nDownloadCounter++ == 0)
					pT->OnStateConnecting();
		}
m_bBeforeNavigate2 = 0; //+++ 実験.
	}


	void __stdcall __PropertyChange( /*[in]*/ BSTR bstrProperty)
	{
		CString 	strProperty(bstrProperty);
		T *pT = static_cast<T *>(this);
		pT->OnPropertyChange(strProperty);
	}


	void __stdcall __NewWindow2( /*[out]*/ IDispatch **ppDisp, /*[out]*/ VARIANT_BOOL *Cancel)
	{
		bool bCancel = false;
		T *  pT 	 = static_cast<T *>(this);

		pT->OnNewWindow2(ppDisp, bCancel);
		*Cancel = bCancel ? VARIANT_TRUE : VARIANT_FALSE;
	}


	void __stdcall __DocumentComplete( /*[in]*/ IDispatch *pDisp, /*[in]*/ VARIANT *URL)
	{
		ATLASSERT(V_VT(URL) == VT_BSTR);

		PreDocumentComplete(pDisp, URL);
		CString strURL = V_BSTR(URL);
		T * 	pT	   = static_cast<T *>(this);
		pT->OnDocumentComplete(pDisp, strURL);

		if ( IsPageIWebBrowser(pDisp) ) { // downloading over
			m_nDownloadCounter = 0;
			pT->OnStateCompleted();
		}
	}


	void __stdcall __OnQuit()
	{
		T *pT = static_cast<T *>(this);
		pT->OnQuit();
	}


	void __stdcall __OnVisible( /*[in]*/ VARIANT_BOOL Visible)
	{
		T *pT = static_cast<T *>(this);
		pT->OnVisible(Visible == VARIANT_TRUE /*? true : false*/);
	}


	void __stdcall __OnToolBar( /*[in]*/ VARIANT_BOOL ToolBar)
	{
		T *pT = static_cast<T *>(this);
		pT->OnToolBar(ToolBar == VARIANT_TRUE /*? true : false*/);
	}


	void __stdcall __OnMenuBar( /*[in]*/ VARIANT_BOOL MenuBar)
	{
		T *pT = static_cast<T *>(this);
		pT->OnMenuBar(MenuBar == VARIANT_TRUE /*? true : false*/);
	}


	void __stdcall __OnStatusBar( /*[in]*/ VARIANT_BOOL StatusBar)
	{
		T *pT = static_cast<T *>(this);
		pT->OnStatusBar(StatusBar == VARIANT_TRUE /*? true : false*/);
	}


	void __stdcall __OnFullScreen( /*[in]*/ VARIANT_BOOL FullScreen)
	{
		T *pT = static_cast<T *>(this);
		pT->OnFullScreen(FullScreen == VARIANT_TRUE /*? true : false*/);
	}


	void __stdcall __OnTheaterMode( /*[in]*/ VARIANT_BOOL TheaterMode)
	{
		T *pT = static_cast<T *>(this);
		pT->OnTheaterMode(TheaterMode == VARIANT_TRUE /*? true : false*/);
	}


	void __stdcall __OnSetSecureLockIcon( /*[in]*/ long SecureLockIcon)
	{
		T *pT = static_cast<T *>(this);
		pT->OnSetSecureLockIcon(SecureLockIcon);
	}


	void __stdcall __OnPrivacyImpactedStateChange( /*[in]*/ VARIANT_BOOL PrivacyImpacted)
	{
		T *pT = static_cast<T *>(this);
		pT->OnPrivacyImpactedStateChange(PrivacyImpacted == VARIANT_TRUE /*? true : false*/);
	}


	void __stdcall __OnFileDownload( /*[in]*/ VARIANT_BOOL ActiveDocument, /*[out]*/ VARIANT_BOOL* Cancel)
	{
		ATLASSERT( Cancel != NULL );
		bool bCancel = false;
		T *pT = static_cast<T*>(this);
		pT->OnFileDownload(ActiveDocument == VARIANT_TRUE ? true : false, bCancel);
		*Cancel = bCancel ? VARIANT_TRUE : VARIANT_FALSE;
	}

	void __stdcall __OnNewWindow3(IDispatch **ppDisp, VARIANT_BOOL *Cancel, VARIANT Flags, BSTR bstrUrlContext,  BSTR bstrUrl)
	{
		T *pT = static_cast<T*>(this);
		bool  bCancel	= false;
		DWORD dwFlags	= Flags.lVal;
		pT->OnNewWindow3(ppDisp, bCancel, dwFlags, bstrUrlContext, bstrUrl);
		*Cancel = bCancel ? VARIANT_TRUE : VARIANT_FALSE;
	}

	void __stdcall __OnWindowClosing( /*[in]*/ VARIANT_BOOL IsChildWindow, /*[out]*/ VARIANT_BOOL* Cancel)
	{
		T *pT = static_cast<T *>(this);
		bool bCancel = false;
		pT->OnWindowClosing(IsChildWindow == VARIANT_TRUE ? true : false, bCancel);
		*Cancel = bCancel ? VARIANT_TRUE : VARIANT_FALSE;
	}

};



////////////////////////////////////////////////////////////////////////////

}		//namespace MTL

#endif	// __MTLBROWSER_H__
