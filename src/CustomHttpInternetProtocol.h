/**
*	@file	CustomHttpInternetProtocol.h
*	@brief	カスタム接続クラス
*/

#pragma once

class CCustomHttpInternetProtocol :	public IInternetProtocol
{
public:
	CCustomHttpInternetProtocol();
	~CCustomHttpInternetProtocol();

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	
	// IInternetProtocol
	STDMETHODIMP Start(LPCWSTR szUrl, IInternetProtocolSink *pOIProtSink, IInternetBindInfo *pOIBindInfo, DWORD grfPI, HANDLE_PTR dwReserved);
	STDMETHODIMP Continue(PROTOCOLDATA *pProtocolData);
	STDMETHODIMP Abort(HRESULT hrReason, DWORD dwOptions);
	STDMETHODIMP Terminate(DWORD dwOptions);
	STDMETHODIMP Suspend();
	STDMETHODIMP Resume();

	STDMETHODIMP Read(void *pv, ULONG cb, ULONG *pcbRead);
	STDMETHODIMP Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
	STDMETHODIMP LockRequest(DWORD dwOptions);
	STDMETHODIMP UnlockRequest();

private:
	LONG                m_cRef;
	BOOL                m_bDefaultHandler;
	BOOL                m_bComplete;
	IInternetProtocolEx*	m_spProtocol;
	DWORD               m_dwDataSize;
	char                m_szData[256];
};


class CCustomHttpInternetProtocolClassFactory : public IClassFactory
{
public:
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef() { return 1; } 
	STDMETHODIMP_(ULONG) Release() { return 1; }
	
	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
	STDMETHODIMP LockServer(BOOL fLock) { return S_OK; }
};
