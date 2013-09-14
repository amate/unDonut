/**
*	@file APIHook.cpp
*/

#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN
//#define STRICT

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <Dbghelp.h>

#include <WinInet.h>

#include <string>
#include <unordered_map>
#include <list>
#include <memory>
#include <set>
#include <atlsync.h>
#include "DonutPFunc.h"
#include "FileNotification.h"
#include "Misc.h"


// ImageDirectoryEntryToData
#pragma comment(lib, "Dbghelp.lib")

namespace {

// ひとつのモジュールに対してAPIフックを行う関数
void ReplaceIATEntryInOneMod(
                             PCSTR pszModuleName,
                             PROC pfnCurrent,
                             PROC pfnNew,
                             HMODULE hmodCaller) 
{
    ULONG ulSize;
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
    pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
        hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

    if (pImportDesc == NULL)
        return;

    while(pImportDesc->Name) {
        PSTR pszModName = (PSTR) ((PBYTE) hmodCaller + pImportDesc->Name);
        if (lstrcmpiA(pszModName, pszModuleName) == 0) 
            break;
        pImportDesc++;
    }

    if (pImportDesc->Name == 0)
        return;

    PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA) 
        ((PBYTE) hmodCaller + pImportDesc->FirstThunk);

    while(pThunk->u1.Function) {
        PROC *ppfn = (PROC*) &pThunk->u1.Function;
        BOOL fFound = (*ppfn == pfnCurrent);
        if (fFound) {
            DWORD dwDummy;
            VirtualProtect(ppfn, sizeof(ppfn), PAGE_EXECUTE_READWRITE, &dwDummy);
            WriteProcessMemory(
                GetCurrentProcess(), ppfn, &pfnNew, sizeof(pfnNew), NULL);
            return;
        }
        pThunk++;
    }
    return;
}

// すべてのモジュールに対してAPIフックを行う関数
void ReplaceIATEntryInAllMods(
                              PCSTR pszModuleName, 
                              PROC pfnCurrent, 
                              PROC pfnNew) 
{
    // モジュールリストを取得
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(
        TH32CS_SNAPMODULE, GetCurrentProcessId());
    if(hModuleSnap == INVALID_HANDLE_VALUE)
        return;

    MODULEENTRY32 me;
    me.dwSize = sizeof(me);
    BOOL bModuleResult = Module32First(hModuleSnap, &me);
    // それぞれのモジュールに対してReplaceIATEntryInOneModを実行
    while(bModuleResult) {        
        ReplaceIATEntryInOneMod(pszModuleName, pfnCurrent, pfnNew, me.hModule);
        bModuleResult = Module32Next(hModuleSnap, &me);
    }
    CloseHandle(hModuleSnap);
}

}	// namespace

////////////////////////////////////////////////////////////////////////////////
// リンクしているdll内にある関数を置換する

void	APIHook(PCSTR pszModuleName, PCSTR pszFuncName, PROC pfnReplace, PROC* ppfnOrig)
{
    PROC pfnOrig = ::GetProcAddress(GetModuleHandleA(pszModuleName), pszFuncName);
	if (pfnOrig == NULL) {
		ATLASSERT( FALSE );
		return;
	}

    ReplaceIATEntryInAllMods(
        pszModuleName, pfnOrig, pfnReplace);
	*ppfnOrig = pfnOrig;
}



/////////////////////////////////////////////////////////////////////////////////////
// InternetConnectW Hook

namespace {

FILETIME	GetFileLastWriteTime(LPCTSTR filepath)
{
	FILETIME lastWriteTime = {};
	HANDLE hFile = CreateFile(GetConfigFilePath(_T("kill.txt")), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return lastWriteTime;
	
	GetFileTime(hFile, NULL, NULL, &lastWriteTime);
	CloseHandle(hFile);
	return lastWriteTime;
}

class CURLHashMatch
{
public:
	CURLHashMatch();

	void	StartWatch();
	void	LoadURLList();

	bool	IsKillURL(LPCWSTR url);

private:
	std::vector<CString>	_SplitURL(LPCWSTR host, int count);

	struct WordUnit {
		std::unordered_map<std::wstring, std::unique_ptr<WordUnit> > wordmap;
		bool	bEnd;

		WordUnit() : bEnd(false)
		{	}
	};
	WordUnit	m_unitList;
	//CDirectoryWatcher	m_killtxtWatcher;
	CFileNotification	m_killtxtModifyNotificate;
	FILETIME	m_lastWriteTime;
} matchtest;

CURLHashMatch::CURLHashMatch() : m_lastWriteTime()
{
#if 0
	m_killtxtWatcher.SetCallbackFunction([this](const CString& filename) {
		if (filename.CompareNoCase(_T("kill.txt")) == 0)
			LoadURLList();
	});
	m_killtxtWatcher.WatchDirectory(GetConfigFilePath(_T("")));
#endif
	LoadURLList();
}

void	CURLHashMatch::StartWatch()
{
	m_lastWriteTime = GetFileLastWriteTime(GetConfigFilePath(_T("kill.txt")));
	m_killtxtModifyNotificate.SetFileNotifyFunc([this]() {
		FILETIME nowLastWriteTime = GetFileLastWriteTime(GetConfigFilePath(_T("kill.txt")));
		if (   nowLastWriteTime.dwHighDateTime != m_lastWriteTime.dwHighDateTime 
			|| nowLastWriteTime.dwLowDateTime != m_lastWriteTime.dwLowDateTime)
		{
			LoadURLList();
			m_lastWriteTime = nowLastWriteTime;
		}
	});
	m_killtxtModifyNotificate.SetUpFileNotification(GetConfigFilePath(_T("")));
}

void	CURLHashMatch::LoadURLList()
{
	//TIMERSTART();
	m_unitList.wordmap.clear();

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
			if (&vecWordList.back() == &(*rit))
				map->bEnd = true;
		}
	}

	//TIMERSTOP(_T("CURLHashMatch::LoadURLList()"));
}

bool	CURLHashMatch::IsKillURL(LPCWSTR host)
{
	std::vector<CString>	vecWordList = _SplitURL(host, ::wcslen(host));
	WordUnit* map = &m_unitList;
	for (auto it = vecWordList.cbegin(); it != vecWordList.cend(); ++it) {
		auto findit = map->wordmap.find(static_cast<LPCWSTR>(*it));
		if (findit == map->wordmap.cend()) {
			return false;		// 見つからなかった
		} else {
			map = findit->second.get();
			if (map->wordmap.size() == 0)	// もう終わりなので
				return true;
			if (map->bEnd)			// 上位のドメインで登録されていたら
				return true;
		}
	}

	return false;
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
	if (lastDotPos > 0) {
		WCHAR word[512] = L"";
		wcsncpy_s(word, host, lastDotPos);
		if (word[0] != L'*')
			vecWordList.push_back(word);
	}
	return vecWordList;
}


class CInternetRequestManager
{
public:
	void	AddKillConnectHandle(HINTERNET hInternet)
	{
		CCritSecLock	lock(m_cs);
		m_sethConnect.insert(hInternet);
	}

	void	RemoveKillConnectHandle(HINTERNET hInternet)
	{
		CCritSecLock	lock(m_cs);
		m_sethConnect.erase(hInternet);
	}

	bool	IsKillConnectHandle(HINTERNET hInternet)
	{
		CCritSecLock	lock(m_cs);
		return m_sethConnect.find(hInternet) != m_sethConnect.end();
	}


	void	AddRequestHandle(HINTERNET hRequest)
	{
		CCritSecLock	lock(m_csReq);
		m_sethRequest.insert(hRequest);
	}

	void	RemoveRequestHandle(HINTERNET hRequest)
	{
		CCritSecLock	lock(m_csReq);
		m_sethRequest.erase(hRequest);
	}

	bool	IsExistRequestHandle(HINTERNET hRequest)
	{
		CCritSecLock	lock(m_csReq);
		return m_sethRequest.find(hRequest) != m_sethRequest.end();
	}

private:
	// Data members
	std::set<HINTERNET>	m_sethConnect;
	CCriticalSection	m_cs;

	std::set<HINTERNET>	m_sethRequest;
	CCriticalSection	m_csReq;

} internetRequestManager;

////////////////////////////////////////////////////////////
// HookInternetConnectW

typedef HINTERNET (WINAPI* FuncInternetConnectW)(
    _In_ HINTERNET hInternet,
    _In_ LPCWSTR lpszServerName,
    _In_ INTERNET_PORT nServerPort,
    _In_opt_ LPCWSTR lpszUserName,
    _In_opt_ LPCWSTR lpszPassword,
    _In_ DWORD dwService,
    _In_ DWORD dwFlags,
    _In_opt_ DWORD_PTR dwContext
    );
FuncInternetConnectW	pfOrgInternetConnectW = nullptr;

HINTERNET WINAPI HookInternetConnectW(
    _In_ HINTERNET hInternet,
    _In_ LPCWSTR lpszServerName,
    _In_ INTERNET_PORT nServerPort,
    _In_opt_ LPCWSTR lpszUserName,
    _In_opt_ LPCWSTR lpszPassword,
    _In_ DWORD dwService,
    _In_ DWORD dwFlags,
    _In_opt_ DWORD_PTR dwContext
    )
{
	if (matchtest.IsKillURL(lpszServerName)) {
		if (!(::GetAsyncKeyState(VK_PAUSE) < 0)) {	//　pauseキーを押してればバイパス
			TRACEIN(_T("urlkill : %s"), lpszServerName);
			//return NULL;
			//lpszServerName	= L"localhost";
			HINTERNET hConnect = pfOrgInternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
			internetRequestManager.AddKillConnectHandle(hConnect);
			return hConnect;
		}
	}
	return pfOrgInternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

//////////////////////////////////////////////////////////////////////
// HookHttpOpenRequestW

typedef HINTERNET (WINAPI* FuncHttpOpenRequestW)(
    _In_ HINTERNET hConnect,
    _In_opt_ LPCWSTR lpszVerb,
    _In_opt_ LPCWSTR lpszObjectName,
    _In_opt_ LPCWSTR lpszVersion,
    _In_opt_ LPCWSTR lpszReferrer,
    _In_opt_z_ LPCWSTR FAR * lplpszAcceptTypes,
    _In_ DWORD dwFlags,
    _In_opt_ DWORD_PTR dwContext
    );
FuncHttpOpenRequestW	pfOrgHttpOpenRequestW = nullptr;

HINTERNET WINAPI HookHttpOpenRequestW(
    _In_ HINTERNET hConnect,
    _In_opt_ LPCWSTR lpszVerb,
    _In_opt_ LPCWSTR lpszObjectName,
    _In_opt_ LPCWSTR lpszVersion,
    _In_opt_ LPCWSTR lpszReferrer,
    _In_opt_z_ LPCWSTR FAR * lplpszAcceptTypes,
    _In_ DWORD dwFlags,
    _In_opt_ DWORD_PTR dwContext
    )
{
	if (internetRequestManager.IsKillConnectHandle(hConnect)) {
		internetRequestManager.RemoveKillConnectHandle(hConnect);
		lpszVerb = L"HEAD";
	}
	return pfOrgHttpOpenRequestW(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

#if 0
//////////////////////////////////////////////////////////////////////
// HookHttpSendRequestW

typedef BOOL (WINAPI* FuncHttpSendRequestW)(
    _In_ HINTERNET hRequest,
    _In_reads_opt_(dwHeadersLength) LPCWSTR lpszHeaders,
    _In_ DWORD dwHeadersLength,
    _In_reads_bytes_opt_(dwOptionalLength) LPVOID lpOptional,
    _In_ DWORD dwOptionalLength
    );
FuncHttpSendRequestW	pfOrgHttpSendRequestW = nullptr;

BOOL WINAPI HookHttpSendRequestW(
    _In_ HINTERNET hRequest,
    _In_reads_opt_(dwHeadersLength) LPCWSTR lpszHeaders,
    _In_ DWORD dwHeadersLength,
    _In_reads_bytes_opt_(dwOptionalLength) LPVOID lpOptional,
    _In_ DWORD dwOptionalLength
    )
{
	return pfOrgHttpSendRequestW(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}
#endif

#if 0
////////////////////////////////////////////////////////////////////
// HookInternetReadFile

typedef BOOL (WINAPI* FuncInternetReadFile)(
    _In_ HINTERNET hFile,
    _Out_writes_bytes_(dwNumberOfBytesToRead) __out_data_source(NETWORK) LPVOID lpBuffer,
    _In_ DWORD dwNumberOfBytesToRead,
    _Out_ LPDWORD lpdwNumberOfBytesRead
    );
FuncInternetReadFile	pfOrgInternetReadFile = nullptr;

BOOL WINAPI HookInternetReadFile(
    _In_ HINTERNET hFile,
    _Out_writes_bytes_(dwNumberOfBytesToRead) __out_data_source(NETWORK) LPVOID lpBuffer,
    _In_ DWORD dwNumberOfBytesToRead,
    _Out_ LPDWORD lpdwNumberOfBytesRead
    )
{
	if (internetRequestManager.IsExistRequestHandle(hFile) == false) {
		internetRequestManager.AddRequestHandle(hFile);
		WCHAR	contentType[512] = L"";
		DWORD	buffSize = 512;
		DWORD	index = 0;
		if (::HttpQueryInfo(hFile, HTTP_QUERY_CONTENT_TYPE, contentType, &buffSize, &index)) {
			if (::wcsncmp(contentType, L"text/html", 9) == 0) {
				LPCSTR kDocType = "<!DOCTYPE html>";
				enum { kDocTypeLength = 15, kMaxSearchCount = 128, };
				if (dwNumberOfBytesToRead > kDocTypeLength) {
					DWORD readableSize = dwNumberOfBytesToRead - kDocTypeLength;
					if (readableSize > kMaxSearchCount)
						readableSize = kMaxSearchCount;
					BOOL bRet = pfOrgInternetReadFile(hFile, lpBuffer, readableSize, lpdwNumberOfBytesRead);
					if (bRet && *lpdwNumberOfBytesRead > 0 && *static_cast<char*>(lpBuffer) != '{') {
						memmove_s(static_cast<char*>(lpBuffer) + kDocTypeLength, readableSize, lpBuffer, *lpdwNumberOfBytesRead);
						memcpy_s(lpBuffer, dwNumberOfBytesToRead, kDocType, kDocTypeLength);
						*lpdwNumberOfBytesRead += kDocTypeLength;
						return TRUE;
					}
					return bRet;
				}
			}
		}
	}
	BOOL bRet = pfOrgInternetReadFile(hFile, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead);
	return bRet;
}

///////////////////////////////////////////////////////////////
// HookInternetCloseHandle

typedef BOOL (WINAPI* FuncInternetCloseHandle)(
    _In_ HINTERNET hInternet
    );
FuncInternetCloseHandle	pfOrgInternetCloseHandle = nullptr;

BOOL WINAPI HookInternetCloseHandle(
    _In_ HINTERNET hInternet
    )
{
	internetRequestManager.RemoveRequestHandle(hInternet);
	return pfOrgInternetCloseHandle(hInternet);
}
#endif

typedef BOOL (WINAPI* FuncHttpQueryInfoA)(
    _In_ HINTERNET hRequest,
    _In_ DWORD dwInfoLevel,
    _Inout_updates_bytes_to_opt_(*lpdwBufferLength, *lpdwBufferLength) __out_data_source(NETWORK) LPVOID lpBuffer,
    _Inout_ LPDWORD lpdwBufferLength,
    _Inout_opt_ LPDWORD lpdwIndex
    );

FuncHttpQueryInfoA	pfOrgHttpQueryInfoA = nullptr;

BOOL WINAPI HookHttpQueryInfoA(
    _In_ HINTERNET hRequest,
    _In_ DWORD dwInfoLevel,
    _Inout_updates_bytes_to_opt_(*lpdwBufferLength, *lpdwBufferLength) __out_data_source(NETWORK) LPVOID lpBuffer,
    _Inout_ LPDWORD lpdwBufferLength,
    _Inout_opt_ LPDWORD lpdwIndex
    )
{
	if (dwInfoLevel == HTTP_QUERY_CUSTOM && strcmp((LPCSTR)lpBuffer, "X-UA-Compatible") == 0) {
		if (pfOrgHttpQueryInfoA(hRequest, dwInfoLevel, lpBuffer, lpdwBufferLength, lpdwIndex) && strncmp((LPCSTR)lpBuffer, "IE=", 3) == 0)
			return TRUE;
		::strcpy((LPSTR)lpBuffer, "IE=edge");
		*lpdwBufferLength = 7;
		return TRUE;
	}
	return pfOrgHttpQueryInfoA(hRequest, dwInfoLevel, lpBuffer, lpdwBufferLength, lpdwIndex);
}

}	// namespace


///////////////////////////////////////////////////////////////////////////////////////////
// InternetConnectに対してフックを仕掛ける
void	DoHookInternetConnect()
{
	APIHook("wininet.dll", "InternetConnectW", (PROC)&HookInternetConnectW, (PROC*)&pfOrgInternetConnectW);
	matchtest.StartWatch();

	APIHook("wininet.dll", "HttpOpenRequestW", (PROC)&HookHttpOpenRequestW, (PROC*)&pfOrgHttpOpenRequestW);

	APIHook("wininet.dll", "HttpQueryInfoA", (PROC)&HookHttpQueryInfoA, (PROC*)&pfOrgHttpQueryInfoA);

#if 0
	auto renderMode = Misc::GetRenderingModeAndForce();
	int majorIEversion = (int)Misc::getIEMejourVersion();
	if (renderMode.first == majorIEversion) {
		//APIHook("wininet.dll", "InternetReadFile", (PROC)&HookInternetReadFile, (PROC*)&pfOrgInternetReadFile);

		//APIHook("wininet.dll", "InternetCloseHandle", (PROC)&HookInternetCloseHandle, (PROC*)&pfOrgInternetCloseHandle);
	}
#endif
}

