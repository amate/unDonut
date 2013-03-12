/**
*	@file FaviconManager.cpp
*/

#include "stdafx.h"
#include "FaviconManager.h"
#include "Misc.h"
#include <fstream>
#include <regex>
#include <boost/thread.hpp>
using boost::thread;
#include "GdiplusUtil.h"
#include <atlsync.h>
#include <atlfile.h>
#include "DonutDefine.h"

using std::unordered_map;

/////////////////////////////////////////
// CFaviconManager

// Data members
HWND	CFaviconManager::s_hWndTabBar = NULL;
unordered_map<std::wstring, CIcon>	CFaviconManager::s_mapIcon;	// key:faviconのURL 値:icon
std::unordered_set<std::wstring>	CFaviconManager::s_setNotFoundFaviconURL;
CCriticalSection	CFaviconManager::s_cs;


//--------------------------
/// 初期化
void	CFaviconManager::Init(HWND hWndTabBar)
{
	s_hWndTabBar = hWndTabBar;
}


BOOL CFaviconManager::OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
	if (pCopyDataStruct->dwData != kSetFaviconURL) {
		SetMsgHandled(FALSE);
		return 0;
	}

	thread td(boost::bind(_DLIconAndRegister, CString((LPCTSTR)pCopyDataStruct->lpData), wnd.m_hWnd));
	return 0;
}

//-------------------------
void	CFaviconManager::SetFavicon(HWND hWndChild, LPCTSTR strFaviconURL)
{
	COPYDATASTRUCT	cds = { 0 };
	cds.dwData	= kSetFaviconURL;
	cds.lpData	= (LPVOID)strFaviconURL;
	cds.cbData	= (::lstrlen(strFaviconURL) + 1) * sizeof(WCHAR);
	CWindow(hWndChild).GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)hWndChild, (LPARAM)&cds);
}

//--------------------------
HICON CFaviconManager::GetFavicon(LPCTSTR strFaviconURL)
{
	HICON	hFavicon = NULL;
	auto it = s_mapIcon.find(std::wstring(strFaviconURL));
	if (it != s_mapIcon.end()) 
		hFavicon = it->second;	
	return hFavicon;
}


HICON	CFaviconManager::GetFaviconFromURL(LPCTSTR url)
{
	CString strFaviconURL;
	CString strHtmlPath;
	if (SUCCEEDED(::URLDownloadToCacheFile(NULL, url, strHtmlPath.GetBuffer(MAX_PATH), MAX_PATH, 0, NULL))) {
		strHtmlPath.ReleaseBuffer();
		std::ifstream filestream(strHtmlPath);
		if (filestream) {
			enum { kMaxReadSize = 3000 };
			unique_ptr<char[]>	htmlContent(new char[kMaxReadSize + 1]);
			filestream.read(htmlContent.get(), kMaxReadSize);
			htmlContent[static_cast<size_t>(filestream.gcount())] = '\0';

			std::regex	rx1("<link[ ]+rel=[\"']?(?:shortcut icon|icon)[\"']?[ ]+href=[\"']?([^ \"]+)[\"']?[^>]*>", std::regex::icase);
			std::regex	rx2("<link[ ]+href=[\"']?([^ \"]+)[\"']?[ ]+rel=[\"']?(?:shortcut icon|icon)[\"']?[^>]*>", std::regex::icase);
			std::cmatch	result;
			if (std::regex_search(htmlContent.get(), result, rx1) || std::regex_search(htmlContent.get(), result, rx2)) {
				CString strhref = result[1].str().c_str();
				DWORD	dwSize = INTERNET_MAX_URL_LENGTH;
				::UrlCombine(url, strhref, strFaviconURL.GetBuffer(INTERNET_MAX_URL_LENGTH), &dwSize, 0);
				strFaviconURL.ReleaseBuffer();
			}
		}
	}
	if (strFaviconURL.IsEmpty()) {	// ルートにあるFaviconのアドレスを得る
		DWORD cchResult = INTERNET_MAX_URL_LENGTH;
		if (::CoInternetParseUrl(url, PARSE_ROOTDOCUMENT, 0, strFaviconURL.GetBuffer(INTERNET_MAX_URL_LENGTH), INTERNET_MAX_URL_LENGTH, &cchResult, 0) == S_OK) {
			strFaviconURL.ReleaseBuffer();
			strFaviconURL += _T("/favicon.ico");
		}
	}

	if (strFaviconURL.GetLength() > 0) {
		CCritSecLock	lock(s_cs);
		CIconHandle hIcon = GetFavicon(strFaviconURL);
		if (hIcon == NULL) {
			hIcon = _DownloadFavicon(strFaviconURL);
			if (hIcon) {
				s_mapIcon[std::wstring(strFaviconURL)] = hIcon;
				hIcon	= hIcon.DuplicateIcon();
			}
		} else {
			hIcon = hIcon.DuplicateIcon();
		}
		return hIcon;
	}
	return NULL;
}



//--------------------------
void CFaviconManager::_DLIconAndRegister(CString strFaviconURL, HWND hWnd)
{
	try {
		CCritSecLock	lock(s_cs);
		HICON	hFaviIcon = GetFavicon(strFaviconURL);
		if (hFaviIcon == NULL && strFaviconURL.IsEmpty() == FALSE) {
			HICON hIcon = _DownloadFavicon(strFaviconURL);
			if (hIcon) {
				s_mapIcon[std::wstring(strFaviconURL)] = hIcon;
				hFaviIcon = hIcon;
			}
		}
		::PostMessage(s_hWndTabBar, WM_SETFAVICONIMAGE, (WPARAM)hWnd, (LPARAM)hFaviIcon);
	} catch(...) {
		ATLASSERT(FALSE);
	}
}

HICON	CFaviconManager::_DownloadFavicon(LPCTSTR FaviconURL)
{
	if (s_setNotFoundFaviconURL.find(FaviconURL) != s_setNotFoundFaviconURL.end()) 
		return NULL;

	CoInitialize(NULL);

	CString strSaveIconPath;
	::URLDownloadToCacheFile(NULL, FaviconURL, strSaveIconPath.GetBuffer(INTERNET_MAX_URL_LENGTH), INTERNET_MAX_URL_LENGTH, 0, NULL);
	strSaveIconPath.ReleaseBuffer();

	CoUninitialize();

	CString strExt = Misc::GetFileExt(strSaveIconPath);
	strExt.MakeLower();
	HICON hIcon = NULL;
	if (strExt == _T("png") || strExt == _T("gif") || strExt == _T("ico")) {
		unique_ptr<Gdiplus::Bitmap>	pbmp(Gdiplus::Bitmap::FromFile(strSaveIconPath));
		if (pbmp) {
			pbmp->GetHICON(&hIcon);
			return hIcon;
		}
	}
	s_setNotFoundFaviconURL.insert(FaviconURL);
	return NULL;
}





















