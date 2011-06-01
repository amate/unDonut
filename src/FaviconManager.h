/**
*	@file FaviconManager.h
*/

#pragma once

#include <unordered_map>
using std::unordered_map;



#define WM_SETFAVICONIMAGE		(WM_APP + 1)
#define USER_MSG_WM_SETFAVICONIMAGE( func )	\
	if (uMsg == WM_SETFAVICONIMAGE) {		\
		SetMsgHandled(TRUE);				\
		OnSetFaviconImage((HWND)wParam, (HICON)lParam);  \
		lResult = 0;						\
		if ( IsMsgHandled() )				\
			return TRUE; 					\
	}

namespace MTL {
	class CMDITabCtrl;
};
using namespace MTL;

/////////////////////////////////////////////////
/// FaviconÇä«óùÇ∑ÇÈ 

class CFaviconManager
{
public:
	static void	Init(CMDITabCtrl* pTabCtrl);

	static void		SetFavicon(HWND hWndChild, LPCTSTR strFaviconURL);
	static HICON	GetFavicon(LPCTSTR strFaviconURL);

private:
	static void _DLIconAndRegister(CString strFaviconURL, HWND hWnd);

	// Data members
	static CMDITabCtrl*	s_pTabCtrl;
	static unordered_map<std::wstring, CIcon>	s_mapIcon;	// key:faviconÇÃURL íl:icon
};

























