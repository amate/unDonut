// CustomBindStatusCallBack.cpp

#include "stdafx.h"
#include "CustomBindStatusCallBack.h"
#include <regex>
#include <MLang.h>
#include <atldlgs.h>
#include "../Misc.h"
#include "../MtlMisc.h"
#include "DownloadOptionDialog.h"
#include "DownloadingListView.h"

#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"Urlmon.lib")
#pragma comment(lib,"winmm.lib")


////////////////////////////////////////////////////////////////////////////////////////
// CCustomBindStatusCallBack

// どうもm_spBSCBPrevがNULLだとＤＬが正常終了しないようで（Ｘ押しても止まらないし）

// Constructor;
CCustomBindStatusCallBack::CCustomBindStatusCallBack(DLItem* pItem, HWND hWndDLing)
	: m_dwTotalRead(0)
	, m_hFile(INVALID_HANDLE_VALUE)
	, m_pDLItem(pItem)
	, m_hWndDLing(hWndDLing) 
	, m_cRef(1)
	, m_hWndNotify(NULL)
{	}

// Destructor
CCustomBindStatusCallBack::~CCustomBindStatusCallBack()
{
}


//------------------------------
/// リファラを設定
void	CCustomBindStatusCallBack::SetReferer(LPCTSTR strReferer)
{
	m_pDLItem->strReferer = strReferer;
}



void	CCustomBindStatusCallBack::SetOption(LPCTSTR strDLFolder, HWND hWnd, DWORD dwOption)
{
	m_strDLFolder = strDLFolder;
	m_hWndNotify = hWnd;
	m_dwDLOption = dwOption;
}


// DLをキャンセルする
void	CCustomBindStatusCallBack::Cancel()
{
	if (m_pDLItem) 
		m_pDLItem->bAbort = true;
	if (m_spBinding)
		m_spBinding->Abort();
}



// 最前面に来るメッセージボックスを表示する
int CCustomBindStatusCallBack::_ActiveMessageBox(const CString& strText, UINT uType)
{
	// 最前面プロセスのスレッドIDを取得する 
	int foregroundID = ::GetWindowThreadProcessId( ::GetForegroundWindow(), NULL); 
	// 最前面アプリケーションの入力処理機構に接続する 
	AttachThreadInput( ::GetCurrentThreadId(), foregroundID, TRUE); 
	// 最前面ウィンドウを変更する 
	::SetForegroundWindow(m_hWndDLing);

	int nReturn = MessageBox(m_hWndDLing, strText, NULL, uType);
	// 接続を解除する
	AttachThreadInput( ::GetCurrentThreadId(), foregroundID, FALSE);

	return nReturn;
}

//////////////////////////////////////////////////////////
// IUnknown
HRESULT CCustomBindStatusCallBack::QueryInterface(REFIID riid, void **ppvObject) 
{
	if (IsEqualIID(riid, IID_IUnknown)) {
		*ppvObject = (IUnknown*)(IBindStatusCallback*)this;
		AddRef();
		return S_OK;
	}
	if (IsEqualIID(riid, IID_IHttpNegotiate)){
		*ppvObject = (IHttpNegotiate*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CCustomBindStatusCallBack::AddRef()
{
	return ::InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CCustomBindStatusCallBack::Release()
{
	::InterlockedDecrement(&m_cRef);
	if (m_cRef == 0) {
        delete this;
		return 0;
	}
	return m_cRef;
}

//////////////////////////////////////////////////////////
// IBindStatusCallback

// バインド開始
HRESULT CCustomBindStatusCallBack::OnStartBinding( 
    /* [in] */ DWORD dwReserved,
	/* [in] */ IBinding *pib)
{
	m_spBinding = pib;

	if (m_spBSCBPrev) 
		m_spBSCBPrev->OnStopBinding(HTTP_STATUS_OK, NULL);
	return S_OK;
}

HRESULT CCustomBindStatusCallBack::OnProgress( 
    /* [in] */ ULONG ulProgress,
    /* [in] */ ULONG ulProgressMax,
    /* [in] */ ULONG ulStatusCode,
    /* [in] */ LPCWSTR szStatusText) 
{
	if (ulStatusCode == BINDSTATUS_BEGINDOWNLOADDATA) {
		m_pDLItem->strURL = szStatusText;
	}

	if (ulStatusCode & BINDSTATUS_DOWNLOADINGDATA) {
		m_pDLItem->nProgress = ulProgress;
		m_pDLItem->nProgressMax = ulProgressMax;
	}

	if (m_pDLItem->bAbort)
		Cancel();

	if (m_spBSCBPrev) {
		if (ulStatusCode == BINDSTATUS_CONTENTDISPOSITIONATTACH)
			return S_OK;
		m_spBSCBPrev->OnProgress(ulProgress, ulProgressMax, ulStatusCode, szStatusText);
	}
	return S_OK;
}
   
// バインド終了
HRESULT CCustomBindStatusCallBack::OnStopBinding( 
    /* [in] */ HRESULT hresult,
    /* [unique][in] */ LPCWSTR szError) 
{
	if (m_hFile != INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hFile);
	}
	if (m_spBinding)
		m_spBinding.Release();

	if (m_spBSCBPrev && m_spBindCtx) {
		HRESULT hr = m_spBindCtx->RegisterObjectParam(L"_BSCB_Holder_", m_spBSCBPrev);
		m_spBSCBPrev.Release();
		m_spBindCtx.Release();
	}

	/* 外部にDL終了通知 */
	if (m_hWndNotify) {
		UINT uMsg = ::RegisterWindowMessage(REGISTERMESSAGE_DLCOMPLETE);
		::SendMessage(m_hWndNotify, uMsg, (WPARAM)(LPCTSTR)m_pDLItem->strText, 0);
	}

	/* DLリストから削除 */
	::PostMessage(m_hWndDLing, WM_USER_REMOVEFROMDOWNLIST, (WPARAM)m_pDLItem, (LPARAM)this);

	// お片付け
	if (::PathFileExists(m_pDLItem->strIncompleteFilePath)) {
		::DeleteFile(m_pDLItem->strIncompleteFilePath);
		::SHChangeNotify(SHCNE_DELETE, SHCNF_PATH, static_cast<LPCTSTR>(m_pDLItem->strIncompleteFilePath), nullptr);
		TRACEIN(_T("不完全ファイルを削除しました。: %s"), (LPCTSTR)m_pDLItem->strIncompleteFilePath);
	}

	return S_OK;
}

HRESULT CCustomBindStatusCallBack::GetBindInfo( 
    /* [out] */ DWORD *grfBINDF,
    /* [unique][out][in] */ BINDINFO *pbindinfo) 
{
	if (m_strDLFolder.IsEmpty()) {
		*grfBINDF = BINDF_ASYNCHRONOUS | /*BINDF_ASYNCSTORAGE | */BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE | /*BINDF_PULLDATA | */BINDF_PRAGMA_NO_CACHE | BINDF_FROMURLMON;
	} else {
		*grfBINDF = BINDF_ASYNCHRONOUS | BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE;
	}
	return S_OK;
}

HRESULT CCustomBindStatusCallBack::OnDataAvailable( 
    /* [in] */ DWORD grfBSCF,
    /* [in] */ DWORD dwSize,
    /* [in] */ FORMATETC *pformatetc,
    /* [in] */ STGMEDIUM *pstgmed )
{ 
	HRESULT hr = S_OK;

	// Get the Stream passed
	if (BSCF_FIRSTDATANOTIFICATION & grfBSCF)  {
		if (m_spStream == NULL && pstgmed->tymed == TYMED_ISTREAM) {
			m_spStream = pstgmed->pstm;	

			// ファイル名を取得する
			if (_GetFileName() == false) {
				// キャンセルされたので帰る
				Cancel();
				return E_ABORT;
			}

			// フォルダが存在しなければ作成
			CString strDir = MtlGetDirectoryPath(m_pDLItem->strFilePath);
			if (::PathIsDirectory(strDir) == FALSE)
				::SHCreateDirectory(NULL, strDir);

			m_hFile = ::CreateFile(m_pDLItem->strIncompleteFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (m_hFile == INVALID_HANDLE_VALUE) {
				//ATLASSERT(FALSE);
				CString strError = _T("ファイルの作成に失敗しました\n");
				strError += GetLastErrorString();
				MessageBox(NULL, strError, NULL, MB_OK | MB_ICONWARNING);
				Cancel();
				return E_ABORT;
			}

			/* DLリストに追加 */
			::PostMessage(m_hWndDLing, WM_USER_ADDTODOWNLOADLIST, (WPARAM)m_pDLItem, 0);
		}
	}

	if (m_spStream && dwSize > m_dwTotalRead) {
		DWORD dwRead = dwSize - m_dwTotalRead; // まだ読まれていないデータ量
		DWORD dwActuallyRead = 0;
		if (dwRead > 0) {
			do {
				BYTE* pBytes = NULL;
				ATLTRY(pBytes = new BYTE[dwRead + 1]);
				if (pBytes == NULL) {
					hr = E_OUTOFMEMORY;
				} else {
					hr = m_spStream->Read(pBytes, dwRead, &dwActuallyRead);
					pBytes[dwActuallyRead] = 0;
					if (dwActuallyRead > 0) {
						DWORD dwWritten = 0;
						m_dwTotalRead += dwActuallyRead;
						::WriteFile(m_hFile, (LPCVOID)pBytes, dwActuallyRead, &dwWritten, NULL);
						ATLASSERT(dwWritten == dwActuallyRead);
					}
					delete[] pBytes;
				}
			//} while (!(hr == E_PENDING || hr == S_FALSE)/* && SUCCEEDED(hr)*/);
			} while (hr == S_OK);
		}
	}

	// Clean up
	if (BSCF_LASTDATANOTIFICATION & grfBSCF) {
		m_spStream.Release();
		if (m_hFile != INVALID_HANDLE_VALUE) {
			::CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;

			if (m_pDLItem->nProgress != m_pDLItem->nProgressMax) {
				m_pDLItem->bAbort = true;
				::DeleteFile(m_pDLItem->strIncompleteFilePath);
				::SHChangeNotify(SHCNE_DELETE, SHCNF_PATH, static_cast<LPCTSTR>(m_pDLItem->strIncompleteFilePath), nullptr);
				TRACEIN(_T("BSCF_LASTDATANOTIFICATION (%s): サイズが一致しません！"), (LPCTSTR)m_pDLItem->strFileName);
			} else {

				::MoveFileEx(m_pDLItem->strIncompleteFilePath, m_pDLItem->strFilePath, MOVEFILE_REPLACE_EXISTING);
				/* エクスプローラーにファイルの変更通知 */
				::SHChangeNotify(SHCNE_RENAMEITEM, SHCNF_PATH, static_cast<LPCTSTR>(m_pDLItem->strIncompleteFilePath), static_cast<LPCTSTR>(m_pDLItem->strFilePath));
			}
		}
	}
	return S_OK;
}


//////////////////////////////////////////////////
// IHttpNegotiate

// 接続
HRESULT CCustomBindStatusCallBack::BeginningTransaction(      
	LPCWSTR szURL,
	LPCWSTR szHeaders,
	DWORD	dwReserved,
	LPWSTR *pszAdditionalHeaders ) 
{
	TRACEIN(_T("CCustomBindStatusCallBack::BeginningTransaction()"));
	m_pDLItem->strURL	 = szURL;
	if (pszAdditionalHeaders == NULL) 
		return E_POINTER;
	*pszAdditionalHeaders = NULL;

	if (m_pDLItem->strReferer.IsEmpty() == FALSE) {	// リファラーを追加する
		CString strBuffer;
		strBuffer.Format(_T("Referer: %s\r\n"), m_pDLItem->strReferer);
		TRACEIN(_T("　Referer : %s"), m_pDLItem->strReferer);

		int nstrSize = (strBuffer.GetLength() + 1) * sizeof(WCHAR);
		LPWSTR wszAdditionalHeaders = (LPWSTR)CoTaskMemAlloc(nstrSize);
		if (wszAdditionalHeaders == NULL) 
			return E_OUTOFMEMORY;

		wcscpy(wszAdditionalHeaders, (LPCTSTR)strBuffer);
		*pszAdditionalHeaders = wszAdditionalHeaders;
	}

	return S_OK;
}
	
// 返答
HRESULT CCustomBindStatusCallBack::OnResponse(      
	DWORD dwResponseCode,
	LPCWSTR szResponseHeaders,
	LPCWSTR szRequestHeaders,
	LPWSTR *pszAdditionalRequestHeaders ) 
{
	TRACEIN(_T("CCustomBindStatusCallBack::OnResponse()"));
	std::wstring strRespons = szResponseHeaders;
	{	// ファイル名を取得
		std::wregex regex(L"Content-Disposition:.*?; filename=\"(.*?)\"");
		std::wsmatch	smatch;
		if (std::regex_search(strRespons, smatch, regex)) {
			CString strBuffer = smatch[1].str().c_str();
			vector<char> filename = Misc::urlstr_decode(strBuffer);
			m_pDLItem->strFileName = Misc::UnknownToCString(filename);
			TRACEIN(_T("　filename : %s"), (LPCTSTR)m_pDLItem->strFileName);
		} else {
			m_pDLItem->strFileName = m_pDLItem->strURL;
			m_pDLItem->strFileName = Misc::GetFileBaseName(m_pDLItem->strFileName);
			int nQuestion = m_pDLItem->strFileName.ReverseFind(_T('?'));
			if (nQuestion != -1) {
				m_pDLItem->strFileName = m_pDLItem->strFileName.Left(nQuestion);
			}
		}
	}
	
	return S_OK;
}


bool	CCustomBindStatusCallBack::_GetFileName()
{
	HRESULT hr = S_OK;
	CComQIPtr<IWinInetHttpInfo> spInfo = m_spBinding;
	if (spInfo) {
		char buff[1024];
		DWORD dwBuffSize = 1024;
		hr = spInfo->QueryInfo(HTTP_QUERY_CONTENT_DISPOSITION, (LPVOID)buff, &dwBuffSize, 0, NULL);
		if (hr == S_OK) {	// CONTENT_DISPOSITIONからファイル名を取得する
			std::string	strbuff = buff;
			std::regex rx("filename\\*=(?: |)UTF-8''(.+)");
			std::regex rx1("filename=(?:\"|)([^\";]+)");
			std::smatch result;
			if (std::regex_search(strbuff, result, rx)) {
				CString strtemp = result.str(1).c_str();
				vector<char> strurldecoded = Misc::urlstr_decode(strtemp);
				m_pDLItem->strFileName = Misc::utf8_to_CString(strurldecoded);	//
			} else if (std::regex_search(strbuff, result, rx1))  {
				std::string strtemp1 = result.str(1);

				int nPerCount = 0;
				std::for_each(strtemp1.begin(), strtemp1.end(), [&nPerCount](char c) {
					if (c == '%')
						++nPerCount;
				});
				vector<char> strUrlDecoded;
				bool	bShiftJis = false;
				if (nPerCount != 0) {
					double dRate = double(strtemp1.length()) / double(nPerCount);
					if (dRate >= 2.4) {	// '%'が多いならURLDecode
						char tempdecoded[INTERNET_MAX_PATH_LENGTH] = "\0";
						DWORD dwBufferLength = INTERNET_MAX_PATH_LENGTH;
						hr = ::UrlUnescapeA(const_cast<LPSTR>(strtemp1.c_str()), tempdecoded, &dwBufferLength, 0);
						if (FAILED(hr)) {
							TRACEIN(_T("UrlUnescapeA 失敗 : Error「%s」\n　(%s)"), (LPCTSTR)GetLastErrorString(hr), (LPCTSTR)CString(strtemp1.c_str()));
							::strcpy_s(tempdecoded, strtemp1.c_str());	// 失敗したので元に戻しておく
						} else {
							strtemp1 = tempdecoded;
							for (auto it = strtemp1.begin(); it != strtemp1.end(); ++it) {	// '+'を空白に置換する
								if (*it == '+')
									*it = ' ';
							}
						}
					} 
				} else {
					if (Misc::IsShiftJIS(strtemp1.c_str(), strtemp1.length())) {
						bShiftJis = true;
						m_pDLItem->strFileName = strtemp1.c_str();
						TRACEIN(_T("Shift-JIS文字列でした : %s"), (LPCTSTR)m_pDLItem->strFileName);
					}
				}
				if (bShiftJis == false) {
					strUrlDecoded.resize(strtemp1.length() + 1, '\0');
					::strcpy_s(strUrlDecoded.data(), strtemp1.length() + 1, strtemp1.c_str());
					m_pDLItem->strFileName = Misc::utf8_to_CString(strUrlDecoded);
				}
			} else {
				TRACEIN(_T("CONTENT_DISPOSITIONが見つからない？ 内容: %s"), (LPCTSTR)CString(buff));
			}
		}
		if (m_pDLItem->strFileName.IsEmpty()) {
			dwBuffSize = 1024;
			hr = spInfo->QueryOption(INTERNET_OPTION_DATAFILE_NAME, (LPVOID)buff, &dwBuffSize);
			if (hr == S_OK) {
				// ファイル名の部分を取得する([?]部分を除く)
				CString strBaseName = Misc::GetFileBaseName(buff);
				int nIndex = strBaseName.ReverseFind(_T('['));
				if (nIndex != -1) {
					m_pDLItem->strFileName = strBaseName.Left(nIndex);
					CString strExt = Misc::GetFileExt(strBaseName);
					if (strExt.IsEmpty() == FALSE) {
						m_pDLItem->strFileName += _T('.') + strExt;
					}
				} else {
					m_pDLItem->strFileName = strBaseName;
				}
			}
		}
	} else {
		ATLASSERT(m_strDLFolder.IsEmpty() == FALSE);	// これ以外で失敗すると困る
		m_pDLItem->strFileName = Misc::GetFileBaseName(m_pDLItem->strURL);	// [?]がつくかも
		int nQIndex = m_pDLItem->strFileName.ReverseFind(_T('?'));
		if (nQIndex != -1) {
			m_pDLItem->strFileName = m_pDLItem->strFileName.Left(nQIndex);
		}
		if (m_pDLItem->strFileName.IsEmpty())
			m_pDLItem->strFileName = _T("index");	// めったにないと思うけど一応

		if (m_pDLItem->strFileName.Find(_T('%')) != -1) {	// URLデコードする
			//vector<char> filename = Misc::urlstr_decode(m_pDLItem->strFileName);
			m_pDLItem->strFileName = Misc::urlstr_decodeJpn(m_pDLItem->strFileName, 3);//Misc::UnknownToCString(filename);
		}
	}

	// リンク抽出ダイアログより(画像を保存も)
	if (m_strDLFolder.IsEmpty() == FALSE) {
		m_pDLItem->strFilePath = m_strDLFolder + m_pDLItem->strFileName;
		if (::PathFileExists(m_pDLItem->strFilePath)) {
			if (m_dwDLOption & DLO_OVERWRITEPROMPT) {
				CString strMessage;
				strMessage.Format(_T("%s は既に存在します。\n上書きしますか？\n"), (LPCTSTR)m_pDLItem->strFileName);;
				if (MessageBox(NULL, strMessage, _T("確認"), MB_OKCANCEL | MB_ICONWARNING) == IDCANCEL) {
					return false;
				}
			} else if (m_dwDLOption & DLO_USEUNIQUENUMBER) {	// 連番を付ける
				int nCount = 0;
				CString strOriginalFileName = m_pDLItem->strFileName;
				while (TRUE) {
					CString strAppend;
					strAppend.Format(_T("_[%d]"), nCount);
					int nExt = m_pDLItem->strFileName.Find(_T('.'));
					if (nExt != -1) {
						m_pDLItem->strFileName.Insert(nExt, strAppend);
					} else {
						m_pDLItem->strFileName += strAppend;
					}
					m_pDLItem->strFilePath = m_strDLFolder + m_pDLItem->strFileName;
					if (::PathFileExists(m_pDLItem->strFilePath) == FALSE)
						break;

					m_pDLItem->strFileName = strOriginalFileName;
					++nCount;
				}
			}
		}
		m_pDLItem->strIncompleteFilePath = m_pDLItem->strFilePath + _T(".incomplete");
		return true;
	}
	// 名前を付けて保存ダイアログを出す
	if (CDLOptions::bUseSaveFileDialog || ::GetKeyState(VK_CONTROL) < 0) {
		COMDLG_FILTERSPEC filter[] = {
			{ L"テキスト文書 (*.txt)", L"*.txt" },// ダミー
			{ L"すべてのファイル", L"*.*" }
		};
		CString strExt = Misc::GetFileExt(m_pDLItem->strFileName);
		filter[0].pszName	= strExt.GetBuffer(0);
		CString strSpec = _T("*.") + strExt;
		filter[0].pszSpec	= strSpec.GetBuffer(0);

		CShellFileSaveDialog	ShellSaveFileDialog(m_pDLItem->strFileName, FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT, strExt, filter, 2);
		if (ShellSaveFileDialog.IsNull()  == false) {
			if (ShellSaveFileDialog.DoModal(NULL) == IDOK) {		
				ShellSaveFileDialog.GetFileTitle(m_pDLItem->strFileName);
				ShellSaveFileDialog.GetFilePath(m_pDLItem->strFilePath);
			} else {
				return false;
			}
		} else {
			CFileDialog dlg(FALSE, NULL, m_pDLItem->strFileName, OFN_OVERWRITEPROMPT, _T("すべてのファイル (*.*)\0*.*\0\0"));
			if (dlg.DoModal(NULL) == IDOK) {
				m_pDLItem->strFileName = dlg.m_szFileTitle;
				if (Misc::GetFileExt(m_pDLItem->strFileName).IsEmpty())
					m_pDLItem->strFileName += _T(".") + strExt;
				m_pDLItem->strFilePath = dlg.m_szFileName;
				if (Misc::GetFileExt(m_pDLItem->strFilePath).IsEmpty())
					m_pDLItem->strFilePath += _T(".") + strExt;
			} else {
				return false;
			}
		}
	} else {
		m_pDLItem->strFilePath = CDLOptions::strDLFolderPath + m_pDLItem->strFileName;
		if (::PathFileExists(m_pDLItem->strFilePath)) {
			CString strMessage;
			strMessage.Format(_T("%s は既に存在します。\n上書きしますか？\n"), (LPCTSTR)m_pDLItem->strFileName);;
			if (MessageBox(NULL, strMessage, _T("確認"), MB_OKCANCEL | MB_ICONWARNING) == IDCANCEL) {
				return false;
			}
		}
	}
	m_pDLItem->strIncompleteFilePath = m_pDLItem->strFilePath + _T(".incomplete");
	return true;
}















