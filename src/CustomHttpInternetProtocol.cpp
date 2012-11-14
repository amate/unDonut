#include "stdafx.h"
#include "CustomHttpInternetProtocol.h"
#include <string>
#include <unordered_map>
#include <list>
#include <memory>
#include "DonutPFunc.h"

namespace {


	class CURLHashMatch
	{
	public:
		CURLHashMatch() { LoadURLList(); }

		void	LoadURLList();

		bool	IsKillURL(LPCWSTR url);

	private:
		std::vector<CString>	_SplitURL(LPCWSTR host, int count);

		struct WordUnit {
			std::unordered_map<std::wstring, std::unique_ptr<WordUnit> > wordmap;
		};
		WordUnit	m_unitList;
	};

	void	CURLHashMatch::LoadURLList()
	{
		TIMERSTART();

		CString killfilepath = GetConfigFilePath(_T("Kill.txt"));
		std::list<CString>	urlList;
		FileReadString(killfilepath, urlList);

		for (auto it = urlList.begin(); it != urlList.end(); ++it) {
			if ((*it).Left(1) == _T("#") || it->IsEmpty())
				continue;
			
			it->MakeLower();

			int count = it->GetLength();
			int slashPos = it->Find(_T('/'));
			if (slashPos != -1)
				count = slashPos;
			std::vector<CString>	vecWordList = _SplitURL(*it, count);
			
			WordUnit* map = &m_unitList;
			for (auto rit = vecWordList.begin(); rit != vecWordList.end(); ++rit) {
				auto findit = map->wordmap.find(static_cast<LPCTSTR>(*rit));
				if (findit != map->wordmap.end()) {  // もうすでにある
					map = findit->second.get();
				} else {
					WordUnit* newmap = new WordUnit;
					map->wordmap[std::wstring(*rit)] = std::unique_ptr<WordUnit>(std::move(newmap));
					map = newmap;
				}
			}
		}

		TIMERSTOP(_T("CURLHashMatch::LoadURLList()"));
	}

	bool	CURLHashMatch::IsKillURL(LPCWSTR url)
	{
		// URLを分解する
		WCHAR	host[512] = L"";
		DWORD	dwSize = 512;
		::UrlGetPartW(url, host, &dwSize, URL_PART_HOSTNAME, 0);

		std::vector<CString>	vecWordList = _SplitURL(host, dwSize);
		WordUnit* map = &m_unitList;
		for (auto it = vecWordList.cbegin(); it != vecWordList.cend(); ++it) {
			auto findit = map->wordmap.find(static_cast<LPCWSTR>(*it));
			if (findit == map->wordmap.cend()) {
				return false;		// 見つからなかった
			} else {
				map = findit->second.get();
			}
		}

		return true;
	}


	std::vector<CString>	CURLHashMatch::_SplitURL(LPCWSTR host, int count)
	{
		std::vector<CString>	vecWordList;
		int lastDotPos = count;
		for (int i = count - 1; i >= 0; --i) {
			if (host[i] == L'.') {
				WCHAR	word[512] = L"";
				wcsncpy_s(word, &host[i + 1], lastDotPos - i - 1);
				lastDotPos = i;
				vecWordList.push_back(word);
			}
		}
		WCHAR word[512] = L"";
		wcsncpy_s(word, host, lastDotPos);
		if (word[0] != L'*')
			vecWordList.push_back(word);

		return vecWordList;
	}



}	// namespace


/////////////////////////////////////////////////////////////////////
// CCustomHttpInternetProtocol


CCustomHttpInternetProtocol::CCustomHttpInternetProtocol() : m_cRef(1), m_spProtocol(nullptr)
{

	m_bDefaultHandler = FALSE;
	m_bComplete = FALSE;
	lstrcpyA(m_szData, "<html><body>アクセスがブロックされているページです。</body></html>");
	m_dwDataSize = lstrlenA(m_szData);

	CoCreateInstance(CLSID_HttpProtocol, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_spProtocol));
	ATLASSERT( m_spProtocol );
}

CCustomHttpInternetProtocol::~CCustomHttpInternetProtocol()
{
}

STDMETHODIMP CCustomHttpInternetProtocol::QueryInterface(REFIID riid, void **ppvObject)
{
	*ppvObject = NULL;
	
	if (IsEqualIID(riid, IID_IInternetProtocol)) {
		*ppvObject = static_cast<IInternetProtocol*>(this);
	} else if (IsEqualIID(riid, IID_IInternetProtocolEx)) {
		return E_NOINTERFACE;
	} else {
		return m_spProtocol->QueryInterface(riid, ppvObject);
	}

	AddRef();

	return S_OK;
}

STDMETHODIMP_(ULONG) CCustomHttpInternetProtocol::AddRef()
{
	m_spProtocol->AddRef();
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CCustomHttpInternetProtocol::Release()
{
	m_spProtocol->Release();
	if (InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}

	return m_cRef;
}

STDMETHODIMP CCustomHttpInternetProtocol::Start(LPCWSTR szUrl, IInternetProtocolSink *pOIProtSink, IInternetBindInfo *pOIBindInfo, DWORD grfPI, HANDLE_PTR dwReserved)
{
	int     i;
	HRESULT hr;
	BOOL    bBlock = FALSE;

	//TRACEIN(szUrl);

	static CURLHashMatch matchtest;
	if (matchtest.IsKillURL(szUrl)) {
		CString matchurl = szUrl;
		matchurl.Replace(_T("%"), _T("％"));
		TRACEIN(matchurl);
		return E_FAIL;
	}
#if 0
	for (i = 0; i < g_nBlockUrlCount; i++) {
		if (lstrcmpW(szUrl, g_lpszBlockUrls[i]) == 0) {
			bBlock = TRUE;
			break;
		}
	}
#endif

	if (bBlock) {
		m_bDefaultHandler = FALSE;
		m_bComplete = FALSE;
		pOIProtSink->ReportProgress(BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE, L"text/html");
		pOIProtSink->ReportData(BSCF_FIRSTDATANOTIFICATION | BSCF_LASTDATANOTIFICATION | BSCF_DATAFULLYAVAILABLE, m_dwDataSize, m_dwDataSize);
		pOIProtSink->ReportResult(S_OK, 0, NULL);
		hr = S_OK;
	}
	else {
		m_bDefaultHandler = TRUE;
		hr = m_spProtocol->Start(szUrl, pOIProtSink, pOIBindInfo, grfPI, dwReserved);
	}

	return hr;
}

STDMETHODIMP CCustomHttpInternetProtocol::Continue(PROTOCOLDATA *pProtocolData)
{
	HRESULT hr;

	if (m_bDefaultHandler)
		hr = m_spProtocol->Continue(pProtocolData);
	else
		hr = S_OK;

	return hr;
}

STDMETHODIMP CCustomHttpInternetProtocol::Abort(HRESULT hrReason, DWORD dwOptions)
{
	HRESULT hr;

	if (m_bDefaultHandler)
		hr = m_spProtocol->Abort(hrReason, dwOptions);
	else
		hr = S_OK;

	return hr;
}

STDMETHODIMP CCustomHttpInternetProtocol::Terminate(DWORD dwOptions)
{
	HRESULT hr;

	if (m_bDefaultHandler)
		hr = m_spProtocol->Terminate(dwOptions);
	else
		hr = S_OK;

	return hr;
}

STDMETHODIMP CCustomHttpInternetProtocol::Suspend()
{
	HRESULT hr;

	if (m_bDefaultHandler)
		hr = m_spProtocol->Suspend();
	else
		hr = E_NOTIMPL;

	return hr;
}

STDMETHODIMP CCustomHttpInternetProtocol::Resume()
{
	HRESULT hr;

	if (m_bDefaultHandler)
		hr = m_spProtocol->Resume();
	else
		hr = E_NOTIMPL;

	return hr;
}

STDMETHODIMP CCustomHttpInternetProtocol::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	HRESULT hr;

	if (m_bDefaultHandler)
		hr = m_spProtocol->Read(pv, cb, pcbRead);
	else {
		if (m_bComplete)
			hr = S_FALSE;
		else {
			CopyMemory(pv, m_szData, m_dwDataSize);
			*pcbRead = m_dwDataSize;
			m_bComplete = TRUE;
			hr = S_OK;
		}
	}

	return hr;
}

STDMETHODIMP CCustomHttpInternetProtocol::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	HRESULT hr;

	if (m_bDefaultHandler)
		hr = m_spProtocol->Seek(dlibMove, dwOrigin, plibNewPosition);
	else
		hr = E_NOTIMPL;

	return hr;
}

STDMETHODIMP CCustomHttpInternetProtocol::LockRequest(DWORD dwOptions)
{
	HRESULT hr;

	if (m_bDefaultHandler)
		hr = m_spProtocol->LockRequest(dwOptions);
	else
		hr = E_NOTIMPL;

	return hr;
}

STDMETHODIMP CCustomHttpInternetProtocol::UnlockRequest()
{
	HRESULT hr;

	if (m_bDefaultHandler)
		hr = m_spProtocol->UnlockRequest();
	else
		hr = E_NOTIMPL;

	return hr;
}





/////////////////////////////////////////////////////////////////////////////
// CCustomHttpInternetProtocolClassFactory

HRESULT CCustomHttpInternetProtocolClassFactory::QueryInterface(REFIID riid, void **ppvObject)
{
	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
		*ppvObject = static_cast<IClassFactory *>(this);
	else
		return E_NOINTERFACE;

	AddRef();
	
	return S_OK;
}

#if 0
ULONG	CCustomHttpInternetProtocolClassFactory::AddRef() 
{
	return InterlockedIncrement(&m_cRef);
}


ULONG	CCustomHttpInternetProtocolClassFactory::Release()
{
	if (InterlockedDecrement(&m_cRef) == 0) {
		delete this;
		return 0;
	}
	return m_cRef;
}
#endif

HRESULT CCustomHttpInternetProtocolClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject)
{
	if (pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;
	
	*ppvObject = NULL;

	CCustomHttpInternetProtocol* p = new CCustomHttpInternetProtocol;
	HRESULT hr = p->QueryInterface(riid, ppvObject);
	p->Release();

	return hr;
}







