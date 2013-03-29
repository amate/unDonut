/**
*	@file	DonutScriptHost.cpp
*	@brief	スクリプトを提供する
*/

#include "stdafx.h"
#include "DonutScriptHost.h"
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <atlcom.h>
#include "DonutDefine.h"
#include "MtlMisc.h"


struct InvokeMethodData {
	LPCTSTR	name;
	DISPID	id;
};
#define BEGIN_INVOKE_METHOD_MAP()	\
	DISPID	GetInvokeMethodDispId(const std::wstring& name) override {	\
		for (auto pList = GetInvokeMethodList(); pList->name ; ++pList) {	\
			if (lstrcmpW(name.c_str(), pList->name) == 0)		\
				return pList->id;								\
		}														\
		return DISPID_UNKNOWN;												\
	}															\
	const InvokeMethodData*	GetInvokeMethodList() {				\
	static const InvokeMethodData s_invokeMethodList[] = {	

#define INVOKE_METHOAD(functionName)	\
		{	_T( #functionName )	, kDispId_##functionName	},	

#define	END_INVOKE_METHOAD_MAP()	\
		{ nullptr, DISPID_UNKNOWN }				\
	};								\
		return s_invokeMethodList;	\
	}								


////////////////////////////////////////////////////////////
// CDispatchImpl

class CDispatchImpl : 	
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatch
{
public:

	BEGIN_COM_MAP(CDispatchImpl)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	// Overridable
	virtual DISPID	GetInvokeMethodDispId(const std::wstring& name) = 0;

	// IDispatch
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
		/* [out] */ __RPC__out UINT *pctinfo) { return E_NOTIMPL; }        
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( 
        /* [in] */ UINT iTInfo,
        /* [in] */ LCID lcid,
		/* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo) { return E_NOTIMPL; }

    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
        /* [in] */ __RPC__in REFIID riid,
        /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
        /* [range][in] */ __RPC__in_range(0,16384) UINT cNames,
        /* [in] */ LCID lcid,
        /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)
	{
		HRESULT hRes = S_OK;
		for (UINT i = 0; i < cNames; ++i) {
			rgDispId[i] = DISPID_UNKNOWN;
			hRes = DISP_E_MEMBERNOTFOUND;
			LPOLESTR szName = rgszNames[i];
			DISPID dispId = GetInvokeMethodDispId(szName);
			if (dispId != DISPID_UNKNOWN) {
				rgDispId[i] = dispId;
				hRes = S_OK;
			}
		}
		return hRes;

	}
        
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
        /* [annotation][in] */ 
        _In_  DISPID dispIdMember,
        /* [annotation][in] */ 
        _In_  REFIID riid,
        /* [annotation][in] */ 
        _In_  LCID lcid,
        /* [annotation][in] */ 
        _In_  WORD wFlags,
        /* [annotation][out][in] */ 
        _In_  DISPPARAMS *pDispParams,
        /* [annotation][out] */ 
        _Out_opt_  VARIANT *pVarResult,
        /* [annotation][out] */ 
        _Out_opt_  EXCEPINFO *pExcepInfo,
        /* [annotation][out] */ 
        _Out_opt_  UINT *puArgErr)
	{
		return DISP_E_MEMBERNOTFOUND;
	}
};

//////////////////////////////////////////////////////////////////////
// CChildFrameObject
class CChildFrameObject : public CDispatchImpl
{
public:
	CChildFrameObject() : m_hWndChildFrame(NULL) { }

	void	SetChildFrameHWND(HWND hWnd) {
		m_hWndChildFrame = hWnd;
	}

	BEGIN_INVOKE_METHOD_MAP()
		INVOKE_METHOAD( GetWebBrowserObject )
		INVOKE_METHOAD( DLCtrl )
		INVOKE_METHOAD( ExStyle )
		INVOKE_METHOAD( AutoRefresh )
	END_INVOKE_METHOAD_MAP()

	// IDispatch
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
        /* [annotation][in] */ 
        _In_  DISPID dispIdMember,
        /* [annotation][in] */ 
        _In_  REFIID riid,
        /* [annotation][in] */ 
        _In_  LCID lcid,
        /* [annotation][in] */ 
        _In_  WORD wFlags,
        /* [annotation][out][in] */ 
        _In_  DISPPARAMS *pDispParams,
        /* [annotation][out] */ 
        _Out_opt_  VARIANT *pVarResult,
        /* [annotation][out] */ 
        _Out_opt_  EXCEPINFO *pExcepInfo,
        /* [annotation][out] */ 
        _Out_opt_  UINT *puArgErr)
	{
		switch (dispIdMember) {
		case kDispId_GetWebBrowserObject:
			if (pDispParams->cArgs == 0) {
				IStream* pStream = (IStream*)::SendMessage(m_hWndChildFrame, WM_GETMARSHALIWEBBROWSERPTR, 0, 0);
				pVarResult->vt = VT_DISPATCH;
				if (pStream) {
					ATLVERIFY(SUCCEEDED(::CoGetInterfaceAndReleaseStream(pStream, IID_IDispatch, (void**)&pVarResult->pdispVal)));
				} else {
					pVarResult->pdispVal = nullptr;
				}
				return S_OK;
			}
			break;

		case kDispId_DLCtrl:
		case kDispId_ExStyle:
		case kDispId_AutoRefresh:
			ChildFrameChangeFlag change;
			if (dispIdMember == kDispId_DLCtrl)
				change = kChangeDLCtrl;
			else if (dispIdMember == kDispId_ExStyle)
				change = kChangeExStyle;
			else if (dispIdMember == kDispId_AutoRefresh)
				change = kChangeAutoRefresh;
			else
				ATLASSERT( FALSE );
			if ((wFlags & DISPATCH_PROPERTYGET) && pDispParams->cArgs == 0) {
				DWORD flags = (DWORD)::SendMessage(m_hWndChildFrame, WM_CHANGECHILDFRAMEFLAGS, change, -1);
				pVarResult->vt		= VT_I4;
				pVarResult->lVal	= flags;
				return S_OK;
			} else if ((wFlags & DISPATCH_PROPERTYPUT) && pDispParams->cArgs == 1 && pDispParams->rgvarg[0].vt == VT_I4) {
				::SendMessage(m_hWndChildFrame, WM_CHANGECHILDFRAMEFLAGS, change, pDispParams->rgvarg[0].lVal);
				return S_OK;
			}
			break;
		}
		return DISP_E_MEMBERNOTFOUND;
	}

private:

	enum DISPIDs {
		kDispId_GetWebBrowserObject = 1,
		kDispId_DLCtrl,
		kDispId_ExStyle,
		kDispId_AutoRefresh,
	};

	// Data members
	HWND	m_hWndChildFrame;

};

//////////////////////////////////////////////////////////////////////
// CunDonutObject

class CunDonutObject : public CDispatchImpl
{
public:
	CunDonutObject() : m_punDonutAPI(nullptr) { }
	~CunDonutObject() { }

	void	SetunDonutAPI(IunDonutAPI* pAPI) { m_punDonutAPI = pAPI; }

	BEGIN_INVOKE_METHOD_MAP()
		INVOKE_METHOAD( Alert )
		INVOKE_METHOAD( TabCount )
		INVOKE_METHOAD( ActiveTabIndex )
		INVOKE_METHOAD( GetChildFrame )
		INVOKE_METHOAD( GetActiveChildFrame )
		INVOKE_METHOAD( GetClipboardText )
		INVOKE_METHOAD( SetClipboardText )
		INVOKE_METHOAD( OpenTab )
	END_INVOKE_METHOAD_MAP()


	// IDispatch

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
        /* [annotation][in] */ 
        _In_  DISPID dispIdMember,
        /* [annotation][in] */ 
        _In_  REFIID riid,
        /* [annotation][in] */ 
        _In_  LCID lcid,
        /* [annotation][in] */ 
        _In_  WORD wFlags,
        /* [annotation][out][in] */ 
        _In_  DISPPARAMS *pDispParams,
        /* [annotation][out] */ 
        _Out_opt_  VARIANT *pVarResult,
        /* [annotation][out] */ 
        _Out_opt_  EXCEPINFO *pExcepInfo,
        /* [annotation][out] */ 
        _Out_opt_  UINT *puArgErr)
	{
		switch (dispIdMember) {
		case kDispId_Alert:
			if (pDispParams->cArgs == 1 /*&& pDispParams->rgvarg[0].vt == VT_BSTR*/) {
				CComVariant vArg(pDispParams->rgvarg[0]);
				vArg.ChangeType(VT_BSTR);
				MessageBox(NULL, vArg.bstrVal, _T("スクリプト - unDonut"), MB_OK);
				return S_OK;
			}
			break;
			//DISPATCH_PROPERTYGET

		case kDispId_TabCount:
			if ((wFlags & DISPATCH_PROPERTYGET) && pDispParams->cArgs == 0) {
				CComVariant vRet(m_punDonutAPI->GetTabCount());
				*pVarResult = vRet;
				return S_OK;
			}
			break;

		case kDispId_ActiveTabIndex:
			if ((wFlags & DISPATCH_PROPERTYGET) && pDispParams->cArgs == 0) {
				CComVariant vRet(m_punDonutAPI->get_ActiveTabIndex());
				*pVarResult = vRet;
				return S_OK;
			} else if ((wFlags & DISPATCH_PROPERTYPUT) && pDispParams->cArgs == 1 && pDispParams->rgvarg[0].vt == VT_I4) {
				m_punDonutAPI->put_ActiveTabIndex(pDispParams->rgvarg[0].lVal);
				return S_OK;
			}
			break;

		case kDispId_GetChildFrame:
		case kDispId_GetActiveChildFrame:
			if ((dispIdMember == kDispId_GetChildFrame && pDispParams->cArgs == 1 && pDispParams->rgvarg[0].vt == VT_I4) || 
				(dispIdMember == kDispId_GetActiveChildFrame && pDispParams->cArgs == 0) )
			{
				int nIndex = -1;
				if (pDispParams->cArgs == 1)
					nIndex = pDispParams->rgvarg[0].lVal;
				else
					nIndex = m_punDonutAPI->get_ActiveTabIndex();
				HWND hWndChildFrame = m_punDonutAPI->GetChildFrame(nIndex);
				pVarResult->vt = VT_DISPATCH;
				if (hWndChildFrame == NULL) {
					pVarResult->pdispVal	= nullptr;
				} else {
					CComObject<CChildFrameObject>* pChildFrameObject;
					CComObject<CChildFrameObject>::CreateInstance(&pChildFrameObject);
					pChildFrameObject->SetChildFrameHWND(hWndChildFrame);
					pChildFrameObject->QueryInterface(&pVarResult->pdispVal);
				}
				return S_OK;
			}
			break;

		case kDispId_GetClipboardText:
			if (pDispParams->cArgs == 0) {
				CComBSTR text(MtlGetClipboardText());
				CComVariant vRet(text);
				vRet.Detach(pVarResult);
				return S_OK;
			}
			break;

		case kDispId_SetClipboardText:
			if (pDispParams->cArgs == 1) {
				CComVariant vArg(pDispParams->rgvarg[0]);
				vArg.ChangeType(VT_BSTR);
				MtlSetClipboardText(vArg.bstrVal, NULL);
				return S_OK;
			}

		case kDispId_OpenTab:
			if (pDispParams->cArgs == 2 && pDispParams->rgvarg[1].vt == VT_BSTR) {
				CComVariant vActive(pDispParams->rgvarg[0]);
				vActive.ChangeType(VT_BOOL);
				m_punDonutAPI->OpenTab(pDispParams->rgvarg[1].bstrVal, vActive.boolVal == VARIANT_TRUE);
				return S_OK;
			}
			break;

		default:
			break;
		}
		return DISP_E_MEMBERNOTFOUND;
	}

private:
	enum DISPIDs {
		kDispId_Alert = 1,
		kDispId_TabCount,
		kDispId_ActiveTabIndex,
		kDispId_GetChildFrame,
		kDispId_GetActiveChildFrame,
		kDispId_GetClipboardText,
		kDispId_SetClipboardText,
		kDispId_OpenTab,
	};

	// Data members
	IunDonutAPI*	m_punDonutAPI;
};


/////////////////////////////////////////////////////////////////
// CDonutScriptHost

/// スクリプトを実行する
void	CDonutScriptHost::ExecuteUserScript(const CString& scriptFilePath)
{
	std::wifstream	filestream(scriptFilePath);
	if (!filestream) {
		return ;
	}
	filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));
	std::wstring scriptText;
	while (filestream.good()) {
		wchar_t buff[512 + 1] = L"";
		filestream.read(buff, 512);
		scriptText.append(buff, (size_t)filestream.gcount());
	}
	filestream.close();

	if (scriptText.length() > 0) {
		CLSID CLSID_Script;
		ATLVERIFY(SUCCEEDED(CLSIDFromProgID(L"JScript", &CLSID_Script)));
		CComPtr<IActiveScript>	spActiveScript;
		ATLVERIFY(SUCCEEDED(spActiveScript.CoCreateInstance(CLSID_Script)));
		ATLVERIFY(SUCCEEDED(spActiveScript->SetScriptSite(this)));

		ATLVERIFY(SUCCEEDED(spActiveScript->AddNamedItem(L"unDonut", SCRIPTITEM_GLOBALMEMBERS | SCRIPTITEM_ISVISIBLE)));

		CComQIPtr<IActiveScriptParse>	spActiveScriptParse = spActiveScript;
		ATLASSERT( spActiveScriptParse );

		HRESULT hr = spActiveScriptParse->InitNew();
		ATLVERIFY(SUCCEEDED(hr));

		EXCEPINFO	excepInfo = {};
		hr = spActiveScriptParse->ParseScriptText(scriptText.c_str(), 
												 NULL,
												 NULL,
												 NULL,
												 0,
												 0,
												 SCRIPTTEXT_ISPERSISTENT,
												 NULL,
												 &excepInfo);
		ATLVERIFY(SUCCEEDED(hr));

		hr = spActiveScript->SetScriptState(SCRIPTSTATE_CONNECTED);
		ATLVERIFY(SUCCEEDED(hr));

		hr = spActiveScript->SetScriptState(SCRIPTSTATE_CLOSED);
		ATLVERIFY(SUCCEEDED(hr));
		hr = spActiveScript->Close();
		ATLVERIFY(SUCCEEDED(hr));
	}
}


HRESULT CDonutScriptHost::QueryInterface(REFIID riid,LPVOID *ppvOut)
{
	*ppvOut = NULL;

	if (IsEqualIID(riid,IID_IActiveScriptSite)) {
		*ppvOut = (IActiveScriptSite*)this;
	} else if(IsEqualIID(riid,IID_IUnknown)) {
		*ppvOut = this;
	} else {
		return E_NOINTERFACE;
	}

	//AddRef();
	return S_OK;
}



HRESULT CDonutScriptHost::GetItemInfo(LPCOLESTR pstrName,
						DWORD dwReturnMask,
						IUnknown **ppiunkItem,
						ITypeInfo **ppti)
{
	if ((dwReturnMask & SCRIPTINFO_IUNKNOWN)) {
		if (lstrcmpW(pstrName, L"unDonut") == 0) {
			CComObject<CunDonutObject>* punDonutObject;
			CComObject<CunDonutObject>::CreateInstance(&punDonutObject);
			punDonutObject->SetunDonutAPI(m_punDonutAPI);
			punDonutObject->QueryInterface(ppiunkItem);
			return S_OK;
		}
	}
	return TYPE_E_ELEMENTNOTFOUND;
}

/// スクリプトエラー発生時に呼ばれる
HRESULT CDonutScriptHost::OnScriptError(IActiveScriptError *pscripterror)
{
	if (pscripterror == nullptr)
		return E_POINTER;

	EXCEPINFO ei = {};
	HRESULT hRes = pscripterror->GetExceptionInfo(&ei);
	DWORD	dwSourceContext = 0;
	ULONG	ulLineNumber = 0;
	LONG	nCharactorPosition = 0;
	hRes = pscripterror->GetSourcePosition(&dwSourceContext, &ulLineNumber, &nCharactorPosition);
	
	std::wstringstream	ss;
	ss << ei.bstrDescription << std::endl;
	ss << L"Line : " << ulLineNumber << std::endl;
	ss << L"CharPos : " << nCharactorPosition << std::endl;
	MessageBox(NULL, ss.str().c_str(), ei.bstrSource, MB_ICONERROR);

	return hRes;
}












