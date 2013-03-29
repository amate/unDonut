/**
*	@file	DonutScriptHost.h
*	@brief	スクリプトを提供する
*/

#pragma once

#include <ActivScp.h>

class IunDonutAPI
{
public:
	virtual HWND	GetChildFrame(int nIndex) = 0;
	virtual LONG	GetTabCount() = 0;
	virtual LONG	get_ActiveTabIndex() = 0;
	virtual void	put_ActiveTabIndex(int nIndex) = 0;
	virtual void	OpenTab(const CString& url, bool bActive) = 0;
};

///////////////////////////////////////////////////////
// CDonutScriptHost

class CDonutScriptHost : public IActiveScriptSite
{
public:
	CDonutScriptHost(IunDonutAPI* pAPI) : m_punDonutAPI(pAPI) { }

	void	ExecuteUserScript(const CString& scriptFilePath);

	//IUnknown
	STDMETHODIMP_(ULONG) AddRef() { return 1; }
	STDMETHODIMP_(ULONG) Release() { return 1; }
	STDMETHODIMP QueryInterface(REFIID riid,LPVOID *ppvOut);

	//IActiveScriptSite
	STDMETHODIMP GetLCID(LCID *plcid) { return E_NOTIMPL; }
	STDMETHODIMP GetItemInfo(LPCOLESTR pstrName,
							DWORD dwReturnMask,
							IUnknown **ppiunkItem,
							ITypeInfo **ppti);
	STDMETHODIMP GetDocVersionString(BSTR *pbstrVersion) { return E_NOTIMPL; }
	STDMETHODIMP OnScriptError(IActiveScriptError *pscripterror);
	STDMETHODIMP OnStateChange(SCRIPTSTATE ssScriptState) { return S_OK; }
	STDMETHODIMP OnScriptTerminate(const VARIANT *pvarResult,const EXCEPINFO *pexcepinfo) { return S_OK; }
	STDMETHODIMP OnEnterScript(void) { return S_OK; }
	STDMETHODIMP OnLeaveScript(void) { return S_OK; }


private:

	// Data members
	IunDonutAPI*	m_punDonutAPI;

};















