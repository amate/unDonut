// Windows Template Library - WTL version 7.1
// Copyright (C) 1997-2003 Microsoft Corporation
// All rights reserved.
//
// This file is a part of the Windows Template Library.
// The code and information is provided "as-is" without
// warranty of any kind, either expressed or implied.

//+++ unDonutで使っている wtl80 側に、これと同様の修正をほどこしたので、このファイルは不要
//+++ (だけど、なんかこっちのほうが実行サイズ小さいかもなんで、こっちを使っている場合もあるかもないかも)

#ifndef __ATLTHEME_H__
#define __ATLTHEME_H__
#define __ATLTHEME_D_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifdef _WIN32_WCE
	#error atltheme.h is not supported on Windows CE
#endif

#ifndef __ATLAPP_H__
	#error atltheme.h requires atlapp.h to be included first
#endif

#ifndef __ATLWIN_H__
	#error atltheme.h requires atlwin.h to be included first
#endif

//#if (_WIN32_WINNT < 0x0501)
//	#error atltheme.h requires _WIN32_WINNT >= 0x0501
//#endif //(_WIN32_WINNT < 0x0501)

#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED 0x031A
#endif


#include <tmschema.h>
#include <uxtheme.h>
//#pragma comment(lib, "uxtheme.lib")

// Note: To create an application that also runs on older versions of Windows,
// use delay load of uxtheme.dll and ensure that no calls to the Theme API are
// made if theming is not supported. It is enough to check if m_hTheme is NULL.
// Example:
//	if(m_hTheme != NULL)
//	{
//		DrawThemeBackground(dc, BP_PUSHBUTTON, PBS_NORMAL, &rect, NULL);
//		DrawThemeText(dc, BP_PUSHBUTTON, PBS_NORMAL, L"Button", -1, DT_SINGLELINE | DT_CENTER | DT_VCENTER, 0, &rect);
//	}
//	else
//	{
//		dc.DrawFrameControl(&rect, DFC_BUTTON, DFCS_BUTTONPUSH);
//		dc.DrawText(_T("Button"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
//	}
//
// Delay load is NOT AUTOMATIC for VC++ 7, you have to link to delayimp.lib,
// and add uxtheme.dll in the Linker.Input.Delay Loaded DLLs section of the
// project properties.
//#if (_MSC_VER < 1300) && !defined(_WTL_NO_THEME_DELAYLOAD)
//	#pragma comment(lib, "delayimp.lib")
//	#pragma comment(linker, "/delayload:uxtheme.dll")
//#endif //(_MSC_VER < 1300) && !defined(_WTL_NO_THEME_DELAYLOAD)


//+++ 元は CThemeDllLoader だったのを、初期化/終了処理をコンストラクタ/デストラクタに任せて、
//+++ apiのチェック＆呼び出しを呼び元でなく、ここのメンバーにて用意。
//
class UxTheme_Wrap {
	typedef HTHEME	(WINAPI *F_OPENTHEMEDATA)(HWND, LPCWSTR);
	typedef HRESULT (WINAPI *F_CLOSETHEMEDATA)(HTHEME);
	typedef HRESULT (WINAPI *F_DRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, const RECT *, OPTIONAL const RECT *);
	typedef HRESULT (WINAPI *F_SETWINDOWTHEME)(HWND, LPCWSTR, LPCWSTR);
	typedef HRESULT (WINAPI *F_GETTHEMEINT)(HTHEME, int, int, int, OUT int *);

	static HMODULE					s_hThemeDLL;

	static F_OPENTHEMEDATA			s_pf_OpenThemeData;
	static F_CLOSETHEMEDATA 		s_pf_CloseThemeData;
	static F_DRAWTHEMEBACKGROUND	s_pf_DrawThemeBackground;
	static F_SETWINDOWTHEME 		s_pf_SetWindowTheme;
	static F_GETTHEMEINT			s_pf_GetThemeInt;

public:
	UxTheme_Wrap()
	{
		ATLASSERT(s_hThemeDLL == 0);
		s_hThemeDLL = ::LoadLibrary(_T("uxtheme.dll"));
		if (s_hThemeDLL) {
			s_pf_OpenThemeData			= (F_OPENTHEMEDATA) 	 ::GetProcAddress( s_hThemeDLL, "OpenThemeData" 		);
			s_pf_CloseThemeData 		= (F_CLOSETHEMEDATA)	 ::GetProcAddress( s_hThemeDLL, "CloseThemeData"		);
			s_pf_DrawThemeBackground	= (F_DRAWTHEMEBACKGROUND)::GetProcAddress( s_hThemeDLL, "DrawThemeBackground"	);
			s_pf_SetWindowTheme 		= (F_SETWINDOWTHEME)	 ::GetProcAddress( s_hThemeDLL, "SetWindowTheme"		);
			s_pf_GetThemeInt			= (F_GETTHEMEINT)		 ::GetProcAddress( s_hThemeDLL, "GetThemeInt"			);
		}
	}


	~UxTheme_Wrap()
	{
		if(s_hThemeDLL) {
			::FreeLibrary(s_hThemeDLL);
			s_hThemeDLL 			= 0;
			s_pf_OpenThemeData		= 0;
			s_pf_CloseThemeData 	= 0;
			s_pf_DrawThemeBackground= 0;
			s_pf_SetWindowTheme 	= 0;
			s_pf_GetThemeInt		= 0;
		}
	}


  #if 1 //*+++ 関数化.
	static HTHEME  OpenThemeData(HWND hWnd, LPCWSTR pszClassList) {
		if (hWnd && s_pf_OpenThemeData)
			return s_pf_OpenThemeData(hWnd, pszClassList);
		return NULL;
	}

	static HRESULT CloseThemeData(HTHEME hTheme) {
		if (hTheme && s_pf_CloseThemeData)
			return s_pf_CloseThemeData(hTheme);
		return S_FALSE;
	}

	//x static bool HaveDrawThemeBackground() { return s_pf_DrawThemeBackground != 0; }

	static HRESULT DrawThemeBackground(HTHEME hTheme, HDC hDc, int nPartID, int nStateID, const RECT *pRect, OPTIONAL const RECT *pRect2) {
		if (hTheme && s_pf_DrawThemeBackground)
			return s_pf_DrawThemeBackground(hTheme, hDc, nPartID, nStateID, pRect, pRect2);
		return S_FALSE;
	}

	static HRESULT SetWindowTheme(HWND hWnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIDList) {
		if (hWnd && s_pf_SetWindowTheme)
			return s_pf_SetWindowTheme(hWnd, pszSubAppName, pszSubIDList);
		return S_FALSE;
	}

	static HRESULT GetThemeInt(HTHEME hTheme, int nPartID, int nStateID, int nPropID, OUT int *pnVal) {
		if (hTheme && s_pf_GetThemeInt)
			return s_pf_GetThemeInt(hTheme, nPartID, nStateID, nPropID, pnVal);
		return S_FALSE;
	}
  #endif
};
__declspec(selectany) HMODULE								UxTheme_Wrap::s_hThemeDLL			  = NULL;
__declspec(selectany) UxTheme_Wrap::F_OPENTHEMEDATA			UxTheme_Wrap::s_pf_OpenThemeData 	  = NULL;
__declspec(selectany) UxTheme_Wrap::F_CLOSETHEMEDATA 		UxTheme_Wrap::s_pf_CloseThemeData	  = NULL;
__declspec(selectany) UxTheme_Wrap::F_DRAWTHEMEBACKGROUND	UxTheme_Wrap::s_pf_DrawThemeBackground = NULL;
__declspec(selectany) UxTheme_Wrap::F_SETWINDOWTHEME 		UxTheme_Wrap::s_pf_SetWindowTheme	  = NULL;
__declspec(selectany) UxTheme_Wrap::F_GETTHEMEINT			UxTheme_Wrap::s_pf_GetThemeInt		  = NULL;

__declspec(selectany) UxTheme_Wrap							g_themeDLLLoader;


///////////////////////////////////////////////////////////////////////////////
// Classes in this file:
//
// CTheme
// CThemeImpl<T, TBase>
//
// Global functions:
//	 AtlDrawThemeClientEdge()



namespace WTL
{

///////////////////////////////////////////////////////////////////////////////
// CTheme - wrapper for theme handle

class CTheme
{
public:
// Data members
	HTHEME m_hTheme;
	static int m_nIsThemingSupported;

// Constructor
	CTheme(HTHEME hTheme = NULL) : m_hTheme(hTheme)
	{
		IsThemingSupported();
	}

// Operators and helpers
	bool IsThemeNull() const
	{
		return (m_hTheme == NULL);
	}

	CTheme& operator =(HTHEME hTheme)
	{
		m_hTheme = hTheme;
		return *this;
	}

	operator HTHEME() const
	{
		return m_hTheme;
	}

	void Attach(HTHEME hTheme)
	{
		m_hTheme = hTheme;
	}

	HTHEME Detach()
	{
		HTHEME hTheme = m_hTheme;
		m_hTheme = NULL;
		return hTheme;
	}

// Theme support helper
	static bool IsThemingSupported()
	{
		if(m_nIsThemingSupported == -1)
		{
			::EnterCriticalSection(&_Module.m_csStaticDataInit);
			if(m_nIsThemingSupported == -1)
			{
				HMODULE hThemeDLL = ::LoadLibrary(_T("uxtheme.dll"));
				m_nIsThemingSupported = (hThemeDLL != NULL) ? 1 : 0;
				if(hThemeDLL != NULL)
					::FreeLibrary(hThemeDLL);
			}
			::LeaveCriticalSection(&_Module.m_csStaticDataInit);
		}

		ATLASSERT(m_nIsThemingSupported != -1);
		return (m_nIsThemingSupported == 1);
	}

// Operations and theme properties
	HTHEME OpenThemeData(HWND hWnd, LPCWSTR pszClassList)
	{
		if(!IsThemingSupported())
			return NULL;

		ATLASSERT(m_hTheme == NULL);
	  #if 1 //+++
		m_hTheme = UxTheme_Wrap::OpenThemeData(hWnd, pszClassList);
	  #else
		if(UxTheme_Wrap::s_pf_OpenThemeData){
			m_hTheme = (UxTheme_Wrap::s_pf_OpenThemeData)(hWnd, pszClassList);
		}
	  #endif
		return m_hTheme;
	}

	HRESULT CloseThemeData()
	{
	  #if 1 //+++
		HRESULT hRet = m_hTheme && UxTheme_Wrap::CloseThemeData(m_hTheme);
		if (SUCCEEDED(hRet))
			m_hTheme = NULL;
		return hRet;
	  #else
		HRESULT hRet = S_FALSE;
		if(m_hTheme != NULL)
		{
			if(!UxTheme_Wrap::s_pf_CloseThemeData)
				return hRet; //S_FALSE;
			hRet = (UxTheme_Wrap::s_pf_CloseThemeData)(m_hTheme);
			if(SUCCEEDED(hRet))
				m_hTheme = NULL;
		}
		return hRet;
	  #endif
	}


	HRESULT DrawThemeBackground(HDC hDC, int nPartID, int nStateID, LPCRECT pRect, LPCRECT pClipRect = NULL)
	{
	  #if 1 //+++
		return UxTheme_Wrap::DrawThemeBackground(m_hTheme, hDC, nPartID, nStateID, pRect, pClipRect);
	  #else
		if(UxTheme_Wrap::s_pf_DrawThemeBackground && m_hTheme){
			return (UxTheme_Wrap::s_pf_DrawThemeBackground)(m_hTheme, hDC, nPartID, nStateID, pRect, pClipRect);
		}else{
			return S_FALSE;
		}
	  #endif
	}


	HRESULT SetWindowTheme(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList)
	{
	  #if 1 //+++
		return UxTheme_Wrap::SetWindowTheme(hwnd, pszSubAppName, pszSubIdList);
	  #else
		HRESULT hRet = S_FALSE;
		if(UxTheme_Wrap::s_pf_SetWindowTheme)
			hRet = (UxTheme_Wrap::s_pf_SetWindowTheme)(hwnd, pszSubAppName, pszSubIdList);

		return hRet;
	  #endif
	}

};

__declspec(selectany) int CTheme::m_nIsThemingSupported = -1;


///////////////////////////////////////////////////////////////////////////////
// CThemeImpl - theme support implementation

// Derive from this class to implement window with theme support.
// Example:
//	class CMyThemeWindow : public CWindowImpl<CMyThemeWindow>, public CThemeImpl<CMyThemeWindow>
//	{
//	...
//		BEGIN_MSG_MAP(CMyThemeWindow)
//			CHAIN_MSG_MAP(CThemeImpl<CMyThemeWindow>)
//			...
//		END_MSG_MAP()
//	...
//	};
//
// If you set theme class list, the class will automaticaly open/close/reopen theme data.


// Helper for drawing theme client edge
inline bool AtlDrawThemeClientEdge(HTHEME hTheme, HWND hWnd, HRGN hRgnUpdate = NULL, HBRUSH hBrush = NULL, int nPartID = 0, int nStateID = 0)
{
	ATLASSERT(hTheme != NULL);
	ATLASSERT(::IsWindow(hWnd));

	CWindowDC dc(hWnd);
	if(dc.IsNull())
		return false;

	// Get border size
	int cxBorder = GetSystemMetrics(SM_CXBORDER);
	int cyBorder = GetSystemMetrics(SM_CYBORDER);

  #if 1 //+++
	if (SUCCEEDED(UxTheme_Wrap::GetThemeInt(hTheme, nPartID, nStateID, TMT_SIZINGBORDERWIDTH, &cxBorder)))
		cyBorder = cxBorder;
  #else
	if(UxTheme_Wrap::s_pf_GetThemeInt)
		if(SUCCEEDED((UxTheme_Wrap::s_pf_GetThemeInt)(hTheme, nPartID, nStateID, TMT_SIZINGBORDERWIDTH, &cxBorder)))
			cyBorder = cxBorder;
  #endif

	RECT rect;
	::GetWindowRect(hWnd, &rect);

	// Remove the client edge from the update region
	int cxEdge = GetSystemMetrics(SM_CXEDGE);
	int cyEdge = GetSystemMetrics(SM_CYEDGE);
	::InflateRect(&rect, -cxEdge, -cyEdge);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rect);
	if(rgn.IsNull())
		return false;

	if(hRgnUpdate != NULL)
		rgn.CombineRgn(hRgnUpdate, rgn, RGN_AND);

	::OffsetRect(&rect, -rect.left, -rect.top);

	::OffsetRect(&rect, cxEdge, cyEdge);
	dc.ExcludeClipRect(&rect);
	::InflateRect(&rect, cxEdge, cyEdge);

  #if 1 //+++
	UxTheme_Wrap::DrawThemeBackground(hTheme, dc, nPartID, nStateID, &rect, NULL);
  #else
	if(UxTheme_Wrap::s_pf_DrawThemeBackground)
		(UxTheme_Wrap::s_pf_DrawThemeBackground)(hTheme, dc, nPartID, nStateID, &rect, NULL);
  #endif

	// Use background brush too, since theme border might not cover everything
	if(cxBorder < cxEdge && cyBorder < cyEdge)
	{
		if(hBrush == NULL)
// need conditional code because types don't match in winuser.h
#ifdef _WIN64
			hBrush = (HBRUSH)::GetClassLongPtr(hWnd, GCLP_HBRBACKGROUND);
#else
			hBrush = (HBRUSH)UlongToPtr(::GetClassLongPtr(hWnd, GCLP_HBRBACKGROUND));
#endif

		::InflateRect(&rect, cxBorder - cxEdge, cyBorder - cyEdge);
		dc.FillRect(&rect, hBrush);
	}

	::DefWindowProc(hWnd, WM_NCPAINT, (WPARAM)rgn.m_hRgn, 0L);

	return true;
}


// Theme extended styles
#define THEME_EX_3DCLIENTEDGE		0x00000001
#define THEME_EX_THEMECLIENTEDGE	0x00000002

template <class T, class TBase = CTheme>
class CThemeImpl : public TBase
{
public:
// Data members
	LPWSTR m_lpstrThemeClassList;
	DWORD m_dwExtendedStyle;   // theme specific extended styles

// Constructor & destructor
	CThemeImpl() : m_lpstrThemeClassList(NULL), m_dwExtendedStyle(0)
	{ }

	~CThemeImpl()
	{
		delete [] m_lpstrThemeClassList;
		m_lpstrThemeClassList	= NULL; 	//+++	delete後は強制クリアしとく.
	}

// Attributes
	bool SetThemeClassList(LPCWSTR lpstrThemeClassList)
	{
		if(m_lpstrThemeClassList != NULL)
		{
			delete [] m_lpstrThemeClassList;
			m_lpstrThemeClassList = NULL;
		}

		if(lpstrThemeClassList == NULL)
			return true;

		int cchLen = lstrlenW(lpstrThemeClassList) + 1;
		ATLTRY(m_lpstrThemeClassList = new WCHAR[cchLen]);
		if(m_lpstrThemeClassList == NULL)
			return false;

		bool bRet = (lstrcpyW(m_lpstrThemeClassList, lpstrThemeClassList) != NULL);
		if(!bRet)
		{
			delete [] m_lpstrThemeClassList;
			m_lpstrThemeClassList = NULL;
		}
		return bRet;
	}

	bool GetThemeClassList(LPWSTR lpstrThemeClassList, int cchListBuffer) const
	{
		int cchLen = lstrlenW(m_lpstrThemeClassList) + 1;
		if(cchListBuffer < cchLen)
			return false;

		return (lstrcpyW(lpstrThemeClassList, m_lpstrThemeClassList) != NULL);
	}

	LPCWSTR GetThemeClassList() const
	{
		return m_lpstrThemeClassList;
	}

	DWORD SetThemeExtendedStyle(DWORD dwExtendedStyle, DWORD dwMask = 0)
	{
		DWORD dwPrevStyle = m_dwExtendedStyle;
		if(dwMask == 0)
			m_dwExtendedStyle = dwExtendedStyle;
		else
			m_dwExtendedStyle = (m_dwExtendedStyle & ~dwMask) | (dwExtendedStyle & dwMask);
		return dwPrevStyle;
	}

	DWORD GetThemeExtendedStyle() const
	{
		return m_dwExtendedStyle;
	}

// Operations
	HTHEME OpenThemeData()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		ATLASSERT(m_lpstrThemeClassList != NULL);
		if(m_lpstrThemeClassList == NULL)
			return NULL;
		CloseThemeData();
		return TBase::OpenThemeData(pT->m_hWnd, m_lpstrThemeClassList);
	}

	HTHEME OpenThemeData(LPCWSTR pszClassList)
	{
		if(!SetThemeClassList(pszClassList))
			return NULL;
		return OpenThemeData();
	}

	HRESULT SetWindowTheme(LPCWSTR pszSubAppName, LPCWSTR pszSubIDList)
	{
		if(!IsThemingSupported())
			return S_FALSE;

		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return TBase::SetWindowTheme(pT->m_hWnd, pszSubAppName, pszSubIDList);
	}

	/*HTHEME GetWindowTheme() const
	{
		if(!IsThemingSupported())
			return NULL;

		const T* pT = static_cast<const T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return ::GetWindowTheme(pT->m_hWnd);
	}

	HRESULT EnableThemeDialogTexture(DWORD dwFlags)
	{
		if(!IsThemingSupported())
			return S_FALSE;

		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return ::EnableThemeDialogTexture(pT->m_hWnd, dwFlags);
	}

	BOOL IsThemeDialogTextureEnabled() const
	{
		if(!IsThemingSupported())
			return FALSE;

		const T* pT = static_cast<const T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return ::IsThemeDialogTextureEnabled(pT->m_hWnd);
	}

	HRESULT DrawThemeParentBackground(HDC hDC, LPRECT pRect = NULL)
	{
		if(!IsThemingSupported())
			return S_FALSE;

		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return ::DrawThemeParentBackground(pT->m_hWnd, hDC, pRect);
	}*/

// Message map and handlers
	// Note: If you handle any of these messages in your derived class,
	// it is better to put CHAIN_MSG_MAP at the start of your message map.
	BEGIN_MSG_MAP(CThemeImpl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_THEMECHANGED, OnThemeChanged)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(m_lpstrThemeClassList != NULL)
			OpenThemeData();
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		CloseThemeData();
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnThemeChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		CloseThemeData();
		if(m_lpstrThemeClassList != NULL)
			OpenThemeData();
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		LRESULT lRet = 0;
		bHandled = FALSE;
		if(IsThemingSupported() && ((pT->GetExStyle() & WS_EX_CLIENTEDGE) != 0))
		{
			if((m_dwExtendedStyle & THEME_EX_3DCLIENTEDGE) != 0)
			{
				lRet = ::DefWindowProc(pT->m_hWnd, uMsg, wParam, lParam);
				bHandled = TRUE;
			}
			else if((m_hTheme != NULL) && ((m_dwExtendedStyle & THEME_EX_THEMECLIENTEDGE) != 0))
			{
				HRGN hRgn = (wParam != 1) ? (HRGN)wParam : NULL;
				if(pT->DrawThemeClientEdge(hRgn))
					bHandled = TRUE;
			}
		}
		return lRet;
	}

// Drawing helper
	bool DrawThemeClientEdge(HRGN hRgnUpdate)
	{
		T* pT = static_cast<T*>(this);
		return AtlDrawThemeClientEdge(m_hTheme, pT->m_hWnd, hRgnUpdate, NULL, 0, 0);
	}
};

}; //namespace WTL

#endif // __ATLTHEME_H__
