/**
	@file	AmbientDispatch.h
	@brief	DLControl を制御するための外部 AmbientDispatch。
	@note

	IAmbientDispatch & CAmbientDispatch

	DLControl を制御するための外部 AmbientDispatch。
	以前は atlhost.h を直接書き換えて実装していましたが、ATLのバージョンアップにより
	IAxWinAmbientDispatchEx が用意されたのでそのような方法をとらなくても良くなりました。
	これは標準では制御されない DLControl 等のアンビエントプロパティとメソッドを
	実装するクラスです。

	インスタンスを作成したら SetHostWindow() でホストウィンドウのポインタを
	指定してください。その後ホストウィンドウから IAxWinAmbientDispatchEx をクエリーし
	SetAmbientDispatch() を呼んでインスタンスのポインタを設定します。
	DLControl の設定・取得は put/get_DLControlFlags を呼び出してください。
 */

#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlcom.h>
#include <mshtmdid.h>



class IAmbientDispatch : public IDispatch {
public:
	virtual HRESULT STDMETHODCALLTYPE	put_DLControlFlags(DWORD dwControlFlags)   = 0;
	virtual HRESULT STDMETHODCALLTYPE	get_DLControlFlags(DWORD *pdwControlFlags) = 0;
};



class CAmbientDispatch : public IAmbientDispatch 
{
private:	//protected:
	//メンバ変数
	DWORD					  m_dwDLControlFlags;
	CComPtr<IAxWinHostWindow> m_pHost;


public: //CAmbientDispatch
	CAmbientDispatch()
		: m_dwDLControlFlags(DLCTL_DLIMAGES | DLCTL_VIDEOS | DLCTL_BGSOUNDS)	//+++
	{ }

	void SetHostWindow(const CComPtr<IAxWinHostWindow> &pHost) { m_pHost = pHost; }


	void RefreshAmbient(DISPID dispid)
	{
		//CAxHostWindowのFireAmbientPropertyChangeを呼び出したいが、
		//ポインタが得られそうにないので、Invokeを介して呼び出す。
		//CAxHostWindow::Invokeを見れば目的が達成されているのが分かるはず。
		CComPtr<IDispatch> pDisp;
		HRESULT 		   hr = m_pHost->QueryInterface(&pDisp);
		if ( FAILED(hr) )
			return;
		VARIANT 		   varResult;
		DISPPARAMS		   params = { 0 };
		hr = pDisp->Invoke(dispid, IID_NULL, 1041 /*JP*/, DISPATCH_PROPERTYPUT, &params, &varResult, NULL, NULL);
	}


public: 
	//IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) { return E_NOINTERFACE; }
	STDMETHODIMP_(ULONG) 	AddRef()  { return 1; }
	STDMETHODIMP_(ULONG) 	Release() { return 1; }


public: //IDispatch
	STDMETHODIMP	GetTypeInfoCount(UINT * pctinfo) { return E_NOTIMPL; }
	STDMETHODIMP	GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo * *pptinfo) { return E_NOTIMPL; }
	STDMETHODIMP	GetIDsOfNames(REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgdispid) { return E_NOTIMPL; }

	STDMETHODIMP	Invoke(
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

		HRESULT hr = E_NOTIMPL;

		if (dispidMember == DISPID_AMBIENT_DLCONTROL) {
			pvarResult->vt	 = VT_I4;
			pvarResult->lVal = m_dwDLControlFlags;
			hr				 = S_OK;
		}

		return hr;
	}

	// IAmbientDispatch
	STDMETHODIMP	put_DLControlFlags(DWORD dwControlFlags)
	{
		m_dwDLControlFlags = dwControlFlags;
		RefreshAmbient(DISPID_AMBIENT_DLCONTROL);
		return S_OK;
	}

	STDMETHODIMP	get_DLControlFlags(DWORD * pdwControlFlags)
	{
		*pdwControlFlags = m_dwDLControlFlags;
		return S_OK;
	}

};
