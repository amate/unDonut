/**
 *	@file ScriptErrorCommandTargetImpl.h
 *	@brief	スクリプトエラー
 */

#pragma once

#include "DonutDefine.h"

//+++ #include "DonutDefine.h" したら、その中のincludeで依存関係の辻褄が
//+++ 合わなくなったので必要な1行をコピペ(ここでしか使わないので移動)
static const CLSID	DONUT_CGID_DocHostCommandHandler	= { 0xf38bc242, 0xb950, 0x11d1, { 0x89, 0x18, 0x00, 0xc0, 0x4f, 0xc2, 0xc8, 0x36 } };


// for debug
#ifdef _DEBUG
	const bool _Mtl_ScriptErrorCommandTargetImpl_traceOn = false;
	#define secTRACE	if (_Mtl_ScriptErrorCommandTargetImpl_traceOn)	ATLTRACE
#else
	#define secTRACE
#endif


template <class T>
class ATL_NO_VTABLE IScriptErrorCommandTargetImpl : public IOleCommandTarget {
public:
	// IUnknown
	STDMETHOD					(QueryInterface) (REFIID riid, void **ppvObject) { return E_NOINTERFACE; }

	ULONG STDMETHODCALLTYPE 	AddRef()  { return 1; }
	ULONG STDMETHODCALLTYPE 	Release() { return 1; }


	// IOleCommandTarget
	STDMETHOD	(QueryStatus) (
		/* [unique][in] */		const GUID __RPC_FAR *	pguidCmdGroup,
		/* [in] */				ULONG					cCmds,
		/* [out][in][size_is] */OLECMD __RPC_FAR		prgCmds[],
		/* [unique][out][in] */ OLECMDTEXT __RPC_FAR *	pCmdText)
	{
		return E_FAIL;
	}


	STDMETHOD	(Exec) (
		/* [unique][in] */		const GUID __RPC_FAR *	pguidCmdGroup,
		/* [in] */				DWORD					nCmdID,
		/* [in] */				DWORD					nCmdexecopt,
		/* [unique][in] */		VARIANT __RPC_FAR * 	pvaIn,
		/* [unique][out][in] */ VARIANT __RPC_FAR * 	pvaOut)
	{
		if ( pguidCmdGroup == NULL || !::IsEqualGUID(*pguidCmdGroup, DONUT_CGID_DocHostCommandHandler) )
			return OLECMDERR_E_UNKNOWNGROUP;

		if (nCmdID != OLECMDID_SHOWSCRIPTERROR)
			return OLECMDERR_E_NOTSUPPORTED;

		secTRACE( _T("IScriptErrorCommandTargetImpl::Exec\n") );

		pvaOut->vt	   = VT_BOOL;
		V_BOOL(pvaOut) = VARIANT_TRUE;

		CString strErrorMessage;

		CComPtr<IHTMLDocument2>	spIHTMLDocument2;
		CComPtr<IHTMLWindow2>	spIHTMLWindow2;
		CComPtr<IHTMLEventObj>	spIHTMLEventObj;

		if (pvaIn && pvaIn->punkVal)
			pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2,(void**)&spIHTMLDocument2);
		if (spIHTMLDocument2)
			spIHTMLDocument2->get_parentWindow(&spIHTMLWindow2);
		if (spIHTMLWindow2)
			spIHTMLWindow2->get_event(&spIHTMLEventObj);
		if (spIHTMLEventObj) {
			OLECHAR*	pwszName[] = {L"errorLine",L"errorCharacter",L"errorCode",L"errorMessage",L"errorUrl"};
			TCHAR*		pszHeader[] = {_T("ライン："),_T("文字："),_T("エラーコード："),_T(""),_T("URI：")};

			for (int i = 0; i < 5; ++i) {
				DISPID		nDispID;
				HRESULT		hr;
				DISPPARAMS	sDispParams;
				CComVariant	vResult;

				//エラー情報のDISPID取得
				hr = spIHTMLEventObj->GetIDsOfNames(IID_NULL, &pwszName[i], 1, LOCALE_SYSTEM_DEFAULT, &nDispID);
				if(FAILED(hr))
					continue;

				//エラー情報取得
				::ZeroMemory(&sDispParams,sizeof(DISPPARAMS));
				hr = spIHTMLEventObj->Invoke(nDispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &sDispParams, &vResult, NULL, NULL);
				if(FAILED(hr))
					continue;

				CString	strBuff;
				CComVariant varResult;
				hr = ::VariantChangeType( &varResult, &vResult, 0, VT_BSTR );
				if (SUCCEEDED(hr))
					strBuff = varResult.bstrVal;

				strErrorMessage += pszHeader[i];
				strErrorMessage += strBuff;
				strErrorMessage += _T("\r\n");
			}
		}
		T* pT = static_cast<T*>(this);
		pT->GetParent().SendMessage(WM_SETLASTSCRIPTERRORMESSAGE, (WPARAM)strErrorMessage.GetBuffer(0));

		return S_OK;
	}

};

