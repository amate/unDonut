// Aero.h
//
//  Aero sample support classes

#ifndef __AERO_H__
#define __AERO_H__

#pragma once

#if !defined _WTL_VER || _WTL_VER < 0x800
	#error Aero.h requires the Windows Template Library 8.0
#endif

#if _WIN32_WINNT < 0x0600
	//#error Aero.h requires _WIN32_WINNT >= 0x0600
	#include "dwmapi_wrap.h"
#ifndef COLOR_MENUHILIGHT
 #define COLOR_MENUHILIGHT       29
#endif
#ifndef COLOR_MENUBAR
 #define COLOR_MENUBAR           30
#endif

#elif !defined(NTDDI_VERSION) || (NTDDI_VERSION < NTDDI_LONGHORN)
	#error Aero.h requires the Windows Vista SDK
#endif

#ifndef __ATLTHEME_H__
	#include "atltheme.h"
#endif

#include <dwmapi.h>
#pragma comment (lib, "dwmapi.lib")

#if (_MSC_VER < 1300) && !defined(_WTL_NO_THEME_DELAYLOAD)
  #pragma comment(lib, "delayimp.lib")
  #pragma comment(linker, "/delayload:dwmapi.dll")
#endif // (_MSC_VER < 1300) && !defined(_WTL_NO_THEME_DELAYLOAD)


///////////////////////////////////////////////////////////////////////////////
// Classes in this file:
//
// CAeroImpl<T> - enables Aero translucency (when available) for any window
// CAeroDialogImpl<T, TBase> - dialog implementation of Aero translucency (when available)
// CAeroFrameImpl<T, TBase, TWinTraits> - frame implementation of Aero translucency (when available) 
// CAeroCtrlImpl - base implementation of Aero opacity for controls
// CAeroStatic - Aero opaque Static control
// CAeroButton - Aero opaque Button control
// CAeroEdit - Aero opaque Edit control

namespace WTL
{

template <class T>
class CAeroImpl :
	public CBufferedPaintImpl<T>,
	public CThemeImpl<T>
{
public:
	CAeroImpl(LPCWSTR lpstrThemeClassList = L"globals") 
	{
		m_PaintParams.dwFlags = BPPF_ERASE;

		SetThemeClassList(lpstrThemeClassList);
		MARGINS m = {0};
		m_Margins = m;
	}

	static bool IsAeroSupported() 
	{
		return IsBufferedPaintSupported();
	}

	bool IsCompositionEnabled() const
	{
		BOOL bEnabled = FALSE;
		return IsAeroSupported() ? SUCCEEDED(DwmApi_Wrap::DwmIsCompositionEnabled(&bEnabled)) && bEnabled : false;
	}    
	
	bool IsTheming() const
	{
		return m_hTheme != 0;
	}    
	
	MARGINS m_Margins;

	bool SetMargins(MARGINS& m)
	{
		m_Margins = m;
		T* pT = static_cast<T*>(this);
		return pT->IsWindow() && IsAeroSupported() ? SUCCEEDED(DwmApi_Wrap::DwmExtendFrameIntoClientArea(pT->m_hWnd, &m_Margins)) : true;
	}

// implementation
	void DoPaint(CDCHandle dc, RECT& rDest)
	{
		T* pT = static_cast<T*>(this);

		RECT rClient;
		pT->GetClientRect(&rClient);

		RECT rView = {rClient.left + m_Margins.cxLeftWidth, rClient.top + m_Margins.cyTopHeight, 
			rClient.right - m_Margins.cxRightWidth, rClient.bottom - m_Margins.cyBottomHeight};

		if (IsCompositionEnabled())
				dc.FillSolidRect(&rClient, RGB(0, 0, 0));
		else
			if (IsTheming())
				pT->DrawThemeBackground(dc, WP_FRAMEBOTTOM, pT->m_hWnd == GetFocus() ? 1 : 2, &rClient, &rDest);
			else
				dc.FillSolidRect(&rClient, GetSysColor(COLOR_MENUBAR));

		if (m_Margins.cxLeftWidth != -1)
			dc.FillSolidRect(&rView, GetSysColor(COLOR_WINDOW));
		else 
			::SetRectEmpty(&rView);

		pT->AeroDoPaint(dc, rClient, rView, rDest);
	}

	void AeroDrawText(HDC dc, LPCTSTR pStr, LPRECT prText, UINT uFormat, DTTOPTS &dto)
	{
        if(IsTheming())
			if (IsAeroSupported())
				DrawThemeTextEx (dc, TEXT_BODYTITLE, 0, pStr, -1, uFormat, prText, &dto );
			else
				DrawThemeText(dc, TEXT_BODYTITLE, 0, pStr, -1, uFormat, 0, prText);
		else
			CDCHandle(dc).DrawText(pStr, -1, prText, uFormat);
	}

	void AeroDrawText(HDC dc, LPCTSTR pStr, LPRECT prText, UINT uFormat, DWORD dwFlags, int iGlowSize)
	{
		DTTOPTS dto = { sizeof(DTTOPTS) };
		dto.dwFlags = dwFlags;
		dto.iGlowSize = iGlowSize;
		AeroDrawText(dc, pStr, prText, uFormat, dto);
	}

// overridable
	void AeroDoPaint(CDCHandle dc, RECT& rClient, RECT& rView, RECT& rDest)
	{}

	BEGIN_MSG_MAP(CAeroImpl)
		CHAIN_MSG_MAP(CThemeImpl<T>)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
        MESSAGE_HANDLER(WM_DWMCOMPOSITIONCHANGED, OnCompositionChanged)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPrintClient)
		CHAIN_MSG_MAP(CBufferedPaintImpl<T>)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (IsThemingSupported())
			OpenThemeData();

		if (IsCompositionEnabled())
			DwmApi_Wrap::DwmExtendFrameIntoClientArea(static_cast<T*>(this)->m_hWnd, &m_Margins);
		return bHandled = FALSE;
	}

	LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (!IsCompositionEnabled() && IsTheming())
			static_cast<T*>(this)->Invalidate(FALSE);
		return bHandled = FALSE;
	}

	LRESULT OnCompositionChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (IsCompositionEnabled())
			SetMargins(m_Margins);
		return 0;
	}

	LRESULT OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		return ::DefWindowProc(pT->m_hWnd, uMsg, wParam, lParam);
	}
};


///////////////////////////////////////////////////////////////////////////////
// CAeroDialogImpl - dialog implementation of Aero translucency (when available)

template <class T, class TBase  = CWindow>
class ATL_NO_VTABLE CAeroDialogImpl : public CDialogImpl<T, TBase>, public CAeroImpl<T>
{
public:
	CAeroDialogImpl(LPCWSTR lpstrThemeClassList = L"dialog") : CAeroImpl(lpstrThemeClassList)
	{}

	void AeroDoPaint(CDCHandle dc, RECT& rClient, RECT& rView, RECT& rDest)
	{
		if (!::IsRectEmpty(&rView))
		{
			if (IsTheming())
				DrawThemeBackground(dc, WP_DIALOG, 1, &rView, &rDest);
			else
				dc.FillSolidRect(&rView, GetSysColor(COLOR_BTNFACE));
		}
	}

	BEGIN_MSG_MAP(CAeroDialogImpl)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CAeroImpl<T>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (IsThemingSupported())
		{
			OpenThemeData();
			EnableThemeDialogTexture(ETDT_ENABLE);
		}

		if (IsCompositionEnabled())
			DwmApi_Wrap::DwmExtendFrameIntoClientArea(static_cast<T*>(this)->m_hWnd, &m_Margins);

		return bHandled = FALSE;
	}

};

///////////////////////////////////////////////////////////////////////////////
// CAeroFrameImpl - frame implementation of Aero translucency (when available)

template <class T, class TBase = ATL::CWindow, class TWinTraits = ATL::CFrameWinTraits>
class ATL_NO_VTABLE CAeroFrameImpl : 
	public CFrameWindowImpl<T, TBase, TWinTraits>,
	public CAeroImpl<T>
{
	typedef CFrameWindowImpl<T, TBase, TWinTraits> _baseClass;
public:
	CAeroFrameImpl(LPCWSTR lpstrThemeClassList = L"window") : CAeroImpl(lpstrThemeClassList)
	{}

	void UpdateLayout(BOOL bResizeBars = TRUE)
	{
		RECT rect = { 0 };
		GetClientRect(&rect);

		// position margins
		if (m_Margins.cxLeftWidth != -1)
		{
			rect.left += m_Margins.cxLeftWidth;
			rect.top += m_Margins.cyTopHeight;
			rect.right -= m_Margins.cxRightWidth;
			rect.bottom -= m_Margins.cyBottomHeight;
		}

		// position bars and offset their dimensions
		UpdateBarsPosition(rect, bResizeBars);

		// resize client window
		if(m_hWndClient != NULL)
			::SetWindowPos(m_hWndClient, NULL, rect.left, rect.top,
				rect.right - rect.left, rect.bottom - rect.top,
				SWP_NOZORDER | SWP_NOACTIVATE);

		Invalidate(FALSE);
	}

	void UpdateBarsPosition(RECT& rect, BOOL bResizeBars = TRUE)
	{
		// resize toolbar
		if(m_hWndToolBar != NULL && ((DWORD)::GetWindowLong(m_hWndToolBar, GWL_STYLE) & WS_VISIBLE))
		{
			RECT rectTB = { 0 };
			::GetWindowRect(m_hWndToolBar, &rectTB);
			if(bResizeBars)
			{
				::SetWindowPos(m_hWndToolBar, NULL, rect.left, rect.top,
					rect.right - rect.left, rectTB.bottom - rectTB.top,
					SWP_NOZORDER | SWP_NOACTIVATE);
				::InvalidateRect(m_hWndToolBar, NULL, FALSE);
			}
			rect.top += rectTB.bottom - rectTB.top;
		}

		// resize status bar
		if(m_hWndStatusBar != NULL && ((DWORD)::GetWindowLong(m_hWndStatusBar, GWL_STYLE) & WS_VISIBLE))
		{
			RECT rectSB = { 0 };
			::GetWindowRect(m_hWndStatusBar, &rectSB);
			rect.bottom -= rectSB.bottom - rectSB.top;
			if(bResizeBars)
				::SetWindowPos(m_hWndStatusBar, NULL, rect.left, rect.bottom,
					rect.right - rect.left, rectSB.bottom - rectSB.top,
					SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	BOOL CreateSimpleStatusBar(LPCTSTR lpstrText, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS , UINT nID = ATL_IDW_STATUS_BAR)
	{
		ATLASSERT(!::IsWindow(m_hWndStatusBar));
		m_hWndStatusBar = ::CreateStatusWindow(dwStyle | CCS_NOPARENTALIGN , lpstrText, m_hWnd, nID);
		return (m_hWndStatusBar != NULL);
	}

	BOOL CreateSimpleStatusBar(UINT nTextID = ATL_IDS_IDLEMESSAGE, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS , UINT nID = ATL_IDW_STATUS_BAR)
	{
		const int cchMax = 128;   // max text length is 127 for status bars (+1 for null)
		TCHAR szText[cchMax];
		szText[0] = 0;
		::LoadString(ModuleHelper::GetResourceInstance(), nTextID, szText, cchMax);
		return CreateSimpleStatusBar(szText, dwStyle, nID);
	}

	static HWND CreateSimpleReBarCtrl(HWND hWndParent, DWORD dwStyle = ATL_SIMPLE_REBAR_STYLE, UINT nID = ATL_IDW_TOOLBAR)
	{
		return _baseClass::CreateSimpleReBarCtrl(hWndParent, dwStyle | CCS_NOPARENTALIGN, nID);
	}

	BOOL CreateSimpleReBar(DWORD dwStyle = ATL_SIMPLE_REBAR_STYLE, UINT nID = ATL_IDW_TOOLBAR)
	{
		ATLASSERT(!::IsWindow(m_hWndToolBar));
		m_hWndToolBar = _baseClass::CreateSimpleReBarCtrl(m_hWnd, dwStyle | CCS_NOPARENTALIGN, nID);
		return (m_hWndToolBar != NULL);
	}

	BEGIN_MSG_MAP(CAeroFrameImpl)
		CHAIN_MSG_MAP(CAeroImpl<T>)
		CHAIN_MSG_MAP(_baseClass)
	END_MSG_MAP()
};

///////////////////////////////////////////////////////////////////////////////
// CAeroCtrlImpl - implementation of Aero opacity for controls

template <class TBase>
class CAeroCtrlImpl : public CBufferedPaintWindowImpl<CAeroCtrlImpl<TBase>, TBase>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

	void DoBufferedPaint(CDCHandle dc, RECT& rect)
	{
		HDC hDCPaint = NULL;
		if(IsBufferedPaintSupported())
			m_BufferedPaint.Begin(dc, &rect, m_dwFormat, &m_PaintParams, &hDCPaint);

		if(hDCPaint != NULL)
			DoPaint(hDCPaint, rect);
		else
			DoPaint(dc.m_hDC, rect);

		if(IsBufferedPaintSupported() && !m_BufferedPaint.IsNull())
		{
			m_BufferedPaint.MakeOpaque(&rect);
			m_BufferedPaint.End();
		}
	}

	virtual void DoPaint(CDCHandle dc, RECT& /*rect*/)
	{
		DefWindowProc(WM_PRINTCLIENT, (WPARAM)dc.m_hDC, PRF_CLIENT);
	}
};

///////////////////////////////////////////////////////////////////////////////
// CAeroStatic - Aero opaque Static control
// CAeroButton - Aero opaque Button control

//typedef CAeroCtrlImpl<CStatic> CAeroStatic;
typedef CAeroCtrlImpl<CButton> CAeroButton;

///////////////////////////////////////////////////////////////////////////////
// CAeroEdit - Aero opaque Edit control
class CAeroEdit : public CAeroCtrlImpl<CEdit>
{
	BEGIN_MSG_MAP(CAeroEdit)
		REFLECTED_COMMAND_CODE_HANDLER(EN_CHANGE , OnChange)
		CHAIN_MSG_MAP(CAeroCtrlImpl<CEdit>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
        Invalidate(FALSE); 
		return bHandled = FALSE;
    } 
};

/////////////////////////////////////////////////////////////////////
// CAeroStatic

class CAeroStatic : 
	public CBufferedPaintWindowImpl<CAeroStatic, CStatic>, 
	public CThemeImpl<CAeroStatic>
{
public:

	CAeroStatic()
	{
		SetThemeClassList(VSCLASS_TEXTSTYLE);
	}

	BOOL	SubclassWindow(HWND hWnd)
	{	
		BOOL b = __super::SubclassWindow(hWnd);
		OpenThemeData();
		return b;
	}

	// Overrides
	void DoPaint(CDCHandle dc, RECT& rect)
	{
		if (IsCompositionEnabled()) {
			RECT rcClient;
			GetClientRect(&rcClient);

			DWORD dwStyle = DT_SINGLELINE;
			if (GetStyle() & SS_CENTER)
				dwStyle |= DT_CENTER;
			if (GetStyle() & SS_CENTERIMAGE)
				dwStyle |= DT_VCENTER;
			if (GetStyle() & SS_SIMPLE)
				dwStyle |= DT_SINGLELINE;

			CString strText;
			int nLength = GetWindowTextLength() + 1;
			GetWindowText(strText.GetBuffer(nLength), nLength);
			strText.ReleaseBuffer();

			dc.FillSolidRect(&rect, RGB(0, 0, 0));
			enum { cxMargin = 3 };
			rcClient.left += cxMargin;
			//HFONT prev = dc.SelectFont(GetFont());
			AeroDrawText(dc, strText, &rcClient, dwStyle, DTT_COMPOSITED | DTT_GLOWSIZE, 9);
			//dc.SelectFont(prev);
		} else {
			DefWindowProc(WM_PRINTCLIENT, (WPARAM)dc.m_hDC, 0);
		}
	}


	// Message map
	BEGIN_MSG_MAP_EX( CAeroStatic )
		CHAIN_MSG_MAP( CThemeImpl<CAeroStatic> )
		if (uMsg == WM_PAINT || uMsg == WM_ERASEBKGND && IsCompositionEnabled())
			CHAIN_MSG_MAP( CBufferedPaintWindowImpl )
	END_MSG_MAP()


private:
	void AeroDrawText(HDC dc, LPCTSTR pStr, LPRECT prText, UINT uFormat, DTTOPTS &dto)
	{
        if(IsTheming())
			if (IsAeroSupported())
				DrawThemeTextEx (dc, TEXT_LABEL, 0, pStr, -1, uFormat, prText, &dto );
			else
				DrawThemeText(dc, TEXT_LABEL, 0, pStr, -1, uFormat, 0, prText);
		else
			CDCHandle(dc).DrawText(pStr, -1, prText, uFormat);
	}

	void AeroDrawText(HDC dc, LPCTSTR pStr, LPRECT prText, UINT uFormat, DWORD dwFlags, int iGlowSize)
	{
		DTTOPTS dto = { sizeof(DTTOPTS) };
		dto.dwFlags = dwFlags;
		dto.iGlowSize = iGlowSize;
		AeroDrawText(dc, pStr, prText, uFormat, dto);
	}

	bool IsAeroSupported() 
	{
		return IsBufferedPaintSupported();
	}

	bool IsCompositionEnabled()
	{
		BOOL bEnabled = FALSE;
		return IsAeroSupported() ? SUCCEEDED(DwmApi_Wrap::DwmIsCompositionEnabled(&bEnabled)) && bEnabled : false;
	}    
	
	bool IsTheming() const
	{
		return m_hTheme != 0;
	}
};


//////////////////////////////////////////////////////////////////////
/// CAeroLineEditCtrl

class CAeroLineEditCtrl : 
	public CWindowImpl<CAeroLineEditCtrl>,
	public CThemeImpl<CAeroLineEditCtrl>,
	public CDwm
{
public:
	CAeroLineEditCtrl() : m_bTracked(false), m_edit(this, 1) {
		SetThemeClassList(L"SearchBoxComposited::SearchBox");
	}


	// Message map and Handler
	BEGIN_MSG_MAP_EX( CAeroLineEditCtrl )
		CHAIN_MSG_MAP( CThemeImpl<CAeroLineEditCtrl> )
		MESSAGE_HANDLER_EX( WM_DWMCOMPOSITIONCHANGED, OnDwmCompositionChanged )
		MSG_WM_CREATE		( OnCreate		)
		MSG_WM_SIZE			( OnSize		)
		MSG_WM_ERASEBKGND	( OnEraseBkgnd	)
	ALT_MSG_MAP(1)
		MESSAGE_HANDLER_EX(WM_MOUSEMOVE		, OnMouseMove)
		MESSAGE_HANDLER_EX(WM_MOUSELEAVE	, OnMouseLeave)
	END_MSG_MAP()


	LRESULT OnDwmCompositionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (IsDwmSupported()) {
			BOOL bCompositionEnable = DwmIsCompositionEnabled();
			UxTheme_Wrap::SetWindowTheme(m_edit, (bCompositionEnable) ? L"EditComposited"/*L"SearchBoxEditComposited"*/ : L"Edit", NULL);
		}
		return 0;
	}

	int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		m_edit.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_TABSTOP | WS_VISIBLE | ES_AUTOHSCROLL, WS_EX_CONTROLPARENT, IDC_AERO_EDIT);
		ATLASSERT(m_edit.IsWindow());
		m_edit.SetFont(AtlGetDefaultGuiFont());
		OnDwmCompositionChanged(0, 0, 0);
		return 0;
	}

	void OnSize(UINT nType, CSize size)
	{
		int cxTextBorder = ::GetSystemMetrics(SM_CXEDGE);
		int cxBorder	 = ::GetSystemMetrics(SM_CXBORDER);
		if (IsThemingSupported()) {
			HTHEME hTextTheme = UxTheme_Wrap::GetWindowTheme(m_edit);
			if (hTextTheme) {
				UxTheme_Wrap::GetThemeInt(hTextTheme, SBBACKGROUND, SB_NORMAL, TMT_TEXTBORDERSIZE, &cxTextBorder);
				UxTheme_Wrap::GetThemeInt(hTextTheme, SBBACKGROUND, SB_NORMAL, TMT_BORDERSIZE, &cxBorder);
			}
		}
		m_edit.MoveWindow(cxTextBorder, cxTextBorder, size.cx - (cxTextBorder*2), size.cy - (cxTextBorder*2));
	}

	BOOL OnEraseBkgnd(CDCHandle dc)
	{
		RECT	rcClient;
		GetClientRect(&rcClient);
		if (IsThemingSupported()) {
			int nStateId = SB_NORMAL;
			if( m_bTracked || ::GetFocus() == m_edit.m_hWnd ) 
				nStateId = SB_FOCUSED;
			if( !IsWindowEnabled() ) 
				nStateId = SB_DISABLED;
			if (IsThemeBackgroundPartiallyTransparent(SBBACKGROUND, nStateId))
				DrawThemeParentBackground(dc, &rcClient);
			DrawThemeBackground(dc, SBBACKGROUND, nStateId, &rcClient, &rcClient);
		}
		return 1;
	}

	// Edit
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		SetMsgHandled(FALSE);

		if (m_bTracked == false) {
			TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, m_edit.m_hWnd };
			_TrackMouseEvent(&tme);
			m_edit.Invalidate();
			m_bTracked = true;
		}
		return 0;
	}
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		SetMsgHandled(FALSE);
		m_bTracked = false;
		m_edit.Invalidate();
		return 0;
	}

private:
	// Constants
	enum { IDC_AERO_EDIT = 1001 };

	enum SEARCHBOXPARTS {
		SBBACKGROUND = 1,
	};

	enum SEARCHBOXSTATES {
		SB_NORMAL	= 1,
		SB_HOT		= 2,
		SB_DISABLED = 3,
		SB_FOCUSED	= 4,
	};

	// Data members
	bool	m_bTracked;
	CContainedWindowT<CEdit>	m_edit;
};


//////////////////////////////////////////////////////////////////////
/// CAeroEditCtrl

class CAeroEditCtrl : 
	public CWindowImpl<CAeroEditCtrl>,
	public CThemeImpl<CAeroEditCtrl>,
	public CDwm
{
public:
	CAeroEditCtrl() : m_edit(this, 1), m_dwFormat(BPBF_TOPDOWNDIB)
	{
		SetThemeClassList(L"SearchBoxComposited::SearchBox");

		memset(&m_PaintParams, 0, sizeof(BP_PAINTPARAMS));
		m_PaintParams.cbSize = sizeof(BP_PAINTPARAMS);
	}

	void	SetFont(HFONT hFont) { m_edit.SetFont(hFont); }

	// Message map and Handler
	BEGIN_MSG_MAP_EX( CAeroEditCtrl )
		CHAIN_MSG_MAP( CThemeImpl<CAeroEditCtrl> )
		MSG_WM_CREATE		( OnCreate		)
		MSG_WM_SIZE			( OnSize		) 
		MSG_WM_ERASEBKGND	( OnEraseBkgnd	)
		DEFAULT_REFLECTION_HANDLER()
	ALT_MSG_MAP(1)
		MSG_WM_PAINT	( OnEditPaint )
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()


	int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		m_edit.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_TABSTOP | WS_VISIBLE | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN, WS_EX_CONTROLPARENT, IDC_AERO_EDIT);
		ATLASSERT(m_edit.IsWindow());
		m_edit.SetFont(AtlGetDefaultGuiFont());
		return 0;
	}

	void OnSize(UINT nType, CSize size)
	{
		int cxTextBorder = ::GetSystemMetrics(SM_CXEDGE);
		int cxBorder	 = ::GetSystemMetrics(SM_CXBORDER);
		m_edit.MoveWindow(cxTextBorder, cxTextBorder, size.cx - (cxTextBorder*2), size.cy - (cxTextBorder*2));
	}

	BOOL OnEraseBkgnd(CDCHandle dc)
	{
		RECT	rcClient;
		GetClientRect(&rcClient);
		if (DwmIsCompositionEnabled()) {
			int nStateId = SB_NORMAL;
			if (IsThemeBackgroundPartiallyTransparent(SBBACKGROUND, nStateId))
				DrawThemeParentBackground(dc, &rcClient);
			DrawThemeBackground(dc, SBBACKGROUND, nStateId, &rcClient, &rcClient);
		} else {
			SetMsgHandled(FALSE);
		}
		return 1;
	}
	
	// Edit
	void OnEditPaint(CDCHandle /*dc*/)
	{
		if (IsThemingSupported()) {
			RECT rcClient;
			m_edit.GetClientRect(&rcClient);

			CPaintDC dc(m_hWnd);
			HDC hDCPaint = NULL;
			if(CBufferedPaintBase::IsBufferedPaintSupported())
				m_BufferedPaint.Begin(dc, &rcClient, m_dwFormat, &m_PaintParams, &hDCPaint);

			DoEditPaint(hDCPaint ? hDCPaint : dc.m_hDC, rcClient);

			if(CBufferedPaintBase::IsBufferedPaintSupported() && !m_BufferedPaint.IsNull())
			{
				m_BufferedPaint.MakeOpaque(&rcClient);
				m_BufferedPaint.End();
			}
		} else
			SetMsgHandled(FALSE);
	}

	void	DoEditPaint(CDCHandle dc, RECT& rect)
	{
		//dc.FillSolidRect(&rect, RGB(255, 255, 255));
		m_edit.SendMessage(WM_PRINTCLIENT, (WPARAM)dc.m_hDC, PRF_CLIENT);
	}

private:
	// Constants
	enum { IDC_AERO_EDIT = 1001 };

	enum SEARCHBOXPARTS {
		SBBACKGROUND = 1,
	};

	enum SEARCHBOXSTATES {
		SB_NORMAL	= 1,
		SB_HOT		= 2,
		SB_DISABLED = 3,
		SB_FOCUSED	= 4,
	};

	// Data members
	CContainedWindowT<CEdit>	m_edit;

	CBufferedPaint 		m_BufferedPaint;
	BP_BUFFERFORMAT 	m_dwFormat;
	BP_PAINTPARAMS 		m_PaintParams;
};

}; // namespace WTL

#endif // __AERO_H__
