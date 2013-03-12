/**
*	@file FaviconManager.h
*/

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <atlsync.h>



#define WM_SETFAVICONIMAGE		(WM_APP + 1)
#define USER_MSG_WM_SETFAVICONIMAGE( func )	\
	if (uMsg == WM_SETFAVICONIMAGE) {		\
		SetMsgHandled(TRUE);				\
		OnSetFaviconImage((HWND)wParam, (HICON)lParam);  \
		lResult = 0;						\
		if ( IsMsgHandled() )				\
			return TRUE; 					\
	}


/////////////////////////////////////////////////
/// FaviconÇä«óùÇ∑ÇÈ 

class CFaviconManager : public CWindowImpl<CFaviconManager>
{
public:
	static void	Init(HWND hWndTabBar);

	BEGIN_MSG_MAP_EX( CFaviconManager )
		MSG_WM_COPYDATA( OnCopyData )
	END_MSG_MAP()

	BOOL OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct);

	static void		SetFavicon(HWND hWndChild, LPCTSTR strFaviconURL);
	static HICON	GetFavicon(LPCTSTR strFaviconURL);

	static HICON	GetFaviconFromURL(LPCTSTR url);

private:
	static void		_DLIconAndRegister(CString strFaviconURL, HWND hWnd);
	static HICON	_DownloadFavicon(LPCTSTR FaviconURL);

	// Data members
	static HWND	s_hWndTabBar;
	static std::unordered_map<std::wstring, CIcon>	s_mapIcon;	// key:faviconÇÃURL íl:icon
	static std::unordered_set<std::wstring>	s_setNotFoundFaviconURL;
	static CCriticalSection	s_cs;
};

























