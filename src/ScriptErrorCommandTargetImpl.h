/**
 *	@file ScriptErrorCommandTargetImpl.h
 *	@brief	スクリプトエラー
 */

#pragma once


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

		//T * 	pT		  = static_cast<T *>(this);
		//CWindow wndFrame  = pT->GetTopLevelParent();
		//CWindow wndStatus = wndFrame.GetDlgItem(ATL_IDW_STATUS_BAR);
		//wndStatus.SetWindowText( _T("スクリプトエラーが発生しました") );
		pvaOut->vt	   = VT_BOOL;
		V_BOOL(pvaOut) = VARIANT_FALSE;//VARIANT_TRUE;

		return S_OK;
	}

};

