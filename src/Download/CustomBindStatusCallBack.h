// CustomBindStatusCallBack.h

#pragma once

#include <wininet.h>
#include <UrlMon.h>


// ëOï˚êÈåæ
struct DLItem;

#define WM_USER_REMOVEFROMDOWNLIST	(WM_APP + 1)
#define WM_USER_ADDTODOWNLOADLIST	(WM_APP	+ 2)
#define WM_USER_USESAVEFILEDIALOG	(WM_APP + 3)

enum { kDownloadingFileExists = 1, kDownloadingURLExists = 2, };


//////////////////////////////////////////////////
// CCustomBindStatusCallBack

class CCustomBindStatusCallBack
	: public IBindStatusCallback 
	, public IHttpNegotiate
{
public:
	// Constructor\Destructor
	CCustomBindStatusCallBack(DLItem* pItem, HWND hWndDLing, LPCTSTR defaultDLFolder);
	~CCustomBindStatusCallBack();

	void	SetThreadId(DWORD dwID);
	void	SetReferer(LPCTSTR strReferer);
	void	SetOption(LPCTSTR strDLFolder, HWND hWnd, DWORD dwOption);
	void	SetBSCB(IBindStatusCallback* pPrev) { m_spBSCBPrev = pPrev; }
	void	SetBindCtx(IBindCtx* pBind) { m_spBindCtx = pBind; }

	void	Cancel();

	////////////////////////////
	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	////////////////////////////
	// IBindStatusCallback
    STDMETHOD(OnStartBinding)( 
        /* [in] */ DWORD dwReserved,
		/* [in] */ IBinding *pib);
    
    STDMETHOD(GetPriority)(/* [out] */ LONG *pnPriority) { return S_OK; }
    
    STDMETHOD(OnLowResource)(/* [in] */ DWORD reserved) { return S_OK; }
    
    STDMETHOD(OnProgress)( 
        /* [in] */ ULONG ulProgress,
        /* [in] */ ULONG ulProgressMax,
        /* [in] */ ULONG ulStatusCode,
        /* [in] */ LPCWSTR szStatusText);
    
    STDMETHOD(OnStopBinding)( 
        /* [in] */ HRESULT hresult,
        /* [unique][in] */ LPCWSTR szError);
    
    STDMETHOD(GetBindInfo)( 
        /* [out] */ DWORD *grfBINDF,
        /* [unique][out][in] */ BINDINFO *pbindinfo);

    STDMETHOD(OnDataAvailable)( 
        /* [in] */ DWORD grfBSCF,
        /* [in] */ DWORD dwSize,
        /* [in] */ FORMATETC *pformatetc,
        /* [in] */ STGMEDIUM *pstgmed);
    
    STDMETHOD(OnObjectAvailable)( 
        /* [in] */ REFIID riid,
        /* [iid_is][in] */ IUnknown *punk) { return S_OK; }

	//////////////////////////
	// IHttpNegotiate
	STDMETHOD(BeginningTransaction)(      
		LPCWSTR szURL,
		LPCWSTR szHeaders,
		DWORD	dwReserved,
		LPWSTR *pszAdditionalHeaders );

	STDMETHOD(OnResponse)(      
		DWORD dwResponseCode,
		LPCWSTR szResponseHeaders,
		LPCWSTR szRequestHeaders,
		LPWSTR *pszAdditionalRequestHeaders );

private:
	int		_ActiveMessageBox(const CString& strText, UINT uType = 0);
	bool	_GetFileName();

	// Data members
	CComPtr<IBinding>		m_spBinding;
	CComPtr<IStream>		m_spStream;
	DWORD					m_dwTotalRead;
	DLItem*					m_pDLItem;
	HWND					m_hWndDLing;
	CComPtr<IBindStatusCallback>	m_spBSCBPrev;
	CComPtr<IBindCtx>				m_spBindCtx;
	HANDLE							m_hFile;
	ULONG						m_cRef;

	CString			m_strDefaultDLFolder;
	CString			m_strDLFolder;
	HWND			m_hWndNotify;
	DWORD			m_dwDLOption;
};