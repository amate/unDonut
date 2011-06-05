/**
*	@file FaviconManager.cpp
*/

#include "stdafx.h"
#include "FaviconManager.h"
#include "Misc.h"
#include <boost/thread.hpp>
using boost::thread;
#include "GdiplusUtil.h"
#include <atlsync.h>


/////////////////////////////////////////
// CFaviconManager

// Data members
HWND	CFaviconManager::s_hWndTabBar = NULL;
unordered_map<std::wstring, CIcon>	CFaviconManager::s_mapIcon;	// key:favicon‚ÌURL ’l:icon


//--------------------------
/// ‰Šú‰»
void	CFaviconManager::Init(HWND hWndTabBar)
{
	s_hWndTabBar = hWndTabBar;
}

//-------------------------
void	CFaviconManager::SetFavicon(HWND hWndChild, LPCTSTR strFaviconURL)
{
	thread td(boost::bind(_DLIconAndRegister, CString(strFaviconURL), hWndChild));
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

//--------------------------
void CFaviconManager::_DLIconAndRegister(CString strFaviconURL, HWND hWnd)
{
	static CCriticalSection lock;
	try {
	lock.Enter();
	HICON	hFaviIcon = GetFavicon(strFaviconURL);
	if (hFaviIcon == NULL && strFaviconURL.IsEmpty() == FALSE) {
		CString strSaveIconPath;
		::URLDownloadToCacheFile(NULL, strFaviconURL, strSaveIconPath.GetBuffer(INTERNET_MAX_URL_LENGTH), INTERNET_MAX_URL_LENGTH, 0, NULL);
		strSaveIconPath.ReleaseBuffer();
		HICON hIcon = AtlLoadIconImage((LPCTSTR)strSaveIconPath, LR_LOADFROMFILE | LR_DEFAULTCOLOR, 16, 16);
		if (hIcon) {
			s_mapIcon[std::wstring(strFaviconURL)] = hIcon;
			hFaviIcon = hIcon;
		} else {
			CString strExt = Misc::GetFileExt(strSaveIconPath);
			strExt.MakeLower();
			if (strExt == _T("png") || strExt == _T("gif")) {
				Gdiplus::Bitmap*	pbmp = Gdiplus::Bitmap::FromFile(strSaveIconPath);
				if (pbmp) {
					pbmp->GetHICON(&hIcon);
					if (hIcon) {
						s_mapIcon[std::wstring(strFaviconURL)] = hIcon;
						hFaviIcon = hIcon;
					}
					delete pbmp;
				}
#if 0
				Gdiplus::Image	image(strSaveIconPath);
				CString strEncodedPath = strSaveIconPath + _T(".bmp");
				Gdiplus::ImageCodecInfo* pEncoder = GetEncoderByMimeType(L"image/bmp");
				ATLASSERT(pEncoder);
				if (image.Save(strEncodedPath, &pEncoder->Clsid) == Gdiplus::Status::Ok) {
						
					bmp.GetHICON(&hIcon);
					if (hIcon) {
						s_mapIcon[std::wstring(strFaviconURL)] = hIcon;
						hFaviIcon = hIcon;
					}
				}
#endif
			}
		}
	}
	PostMessage(s_hWndTabBar, WM_SETFAVICONIMAGE, (WPARAM)hWnd, (LPARAM)hFaviIcon);
	lock.Leave();
	} catch(...) {
		ATLASSERT(FALSE);
	}
}

























