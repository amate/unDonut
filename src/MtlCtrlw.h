/**
 *	@file	MtlCtrlw.h
 *	@brief	MTL : コマンド・バー(メニュー)
 */
////////////////////////////////////////////////////////////////////////////
// MTL Version 0.10
// Copyright (C) 2001 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// MtlCtrlw.h: Last updated: March 17, 2001
/////////////////////////////////////////////////////////////////////////////

#ifndef __MTLCTRLW_H__
#define __MTLCTRLW_H__

#pragma once

#ifndef __MTLMISC_H__
#include "MtlMisc.h"
//#error mtlctrlw.h requires mtlmisc.h to be included first
#endif

#include "MtlWin.h"
#include "MtlUser.h"
#include "ChevronHandler.h"

#include "option/SkinOption.h"	//+++
#include "option/MenuDialog.h"


namespace MTL {


/////////////////////////////////////////////////////////////////////////////
// Command Bars

// Window Styles:
enum ECbr2Ws {
	CBR2WS_TOP				= CCS_TOP,
	CBR2WS_BOTTOM			= CCS_BOTTOM,
	CBR2WS_NORESIZE 		= CCS_NORESIZE,
	CBR2WS_NOPARENTALIGN	= CCS_NOPARENTALIGN,
	CBR2WS_NODIVIDER		= CCS_NODIVIDER,
};

// standard command bar styles
enum {
	MTL_SIMPLE_CMDBAR2_PANE_STYLE	= (WS_CHILD | WS_VISIBLE | CBR2WS_NODIVIDER | CBR2WS_NORESIZE | CBR2WS_NOPARENTALIGN),
};



// Extended styles
enum ECbr2 {
	CBR2_EX_TRANSPARENT 	= 0x00000001L,		// not supported yet
	CBR2_EX_SHAREMENU		= 0x00000002L,
};



//+++ enum { LBUTTON_DOWN			= 100, };		//+++ 未使用のよう




/////////////////////////////////////////////////////////////////////////////
// Forward declarations

template <class T, class TBase = CCommandBarCtrl2Base, class TWinTraits = CControlWinTraits>
class	CCommandBarCtrlImpl;

class	CCommandBarCtrl2;



/////////////////////////////////////////////////////////////////////////////
// CCmdBarButton

// CCmdBarButton state flags
enum ECbstate {
	CBSTATE_ENABLED 	= 0x01,
	CBSTATE_PRESSED 	= 0x02,
	CBSTATE_HOT 		= 0x04,
	CBSTATE_INACTIVE	= 0x08,
	CBSTATE_HIDDEN		= 0x10,
};



class CCmdBarButton {
	// Data members
public:
	BYTE			m_fsState;

private:
	CString 		m_strBtn;			// string on button
	CMenuHandle 	m_menuSub;			// handle to sub menu

public:
	TCHAR			m_cAccessKey;		// access key (Alt key + X)
	CRect			m_rcBtn;			//+++ atlのCRect::CRect()は何も初期値を設定しない! (wtlのはする.)
//	HFONT			m_hFont;			//+++

public:
	// Constructor/destructor
	CCmdBarButton(BYTE fsState, const CString &strBtn, HMENU hSubMenu)
		: m_fsState(fsState)
		, m_strBtn(strBtn)
		, m_menuSub(hSubMenu)
		, m_cAccessKey(0)
		, m_rcBtn(0,0,0,0)				//+++ atlのCRect::CRect()は何も初期値を設定しない...wtlのはする. どちらでもよいように初期値設定.
	{
		// init access key
		int nIndex = m_strBtn.Find( _T('&') );

		if ( nIndex + 1 >= m_strBtn.GetLength() ) {
			ATLTRACE( _T("warning : & is bad position, access key is invalid.\n") );
			m_cAccessKey = 0;
		} else {
			m_cAccessKey = m_strBtn[nIndex + 1];	// -1 + 1 = 0; it's ok
		}
	}


	// Attributes
	bool ModifyState(BYTE fsRemove, BYTE fsAdd)
	{
		bool bOldEnable = m_fsState & CBSTATE_ENABLED;

		m_fsState = (m_fsState & ~fsRemove) | fsAdd;
		bool bNewEnable = m_fsState & CBSTATE_ENABLED;
		return bOldEnable != bNewEnable;
	}


	// Methods
	void Update(CDCHandle dc)
	{
		if (m_fsState & CBSTATE_HIDDEN)
			return;

		if (m_fsState & CBSTATE_HOT) {
			DrawHot(dc);
		} else if (m_fsState & CBSTATE_PRESSED) {
			DrawPressed(dc);
		} else {
			_DrawText(dc);
		}
	}


	void TrackPopup(HWND hWnd, HWND hWndMenuOwner, bool bW2K)
	{
		// Menu animation flags
	  #ifndef TPM_VERPOSANIMATION
		const UINT TPM_VERPOSANIMATION = 0x1000L;
	  #endif
	  #ifndef TPM_NOANIMATION
		const UINT TPM_NOANIMATION	   = 0x4000L;
	  #endif

		// "! menu"
		ATLASSERT( m_menuSub.IsMenu() );

		CWindow    wnd(hWnd);
		// get popup menu and it's position
		CRect	   rect 			   = m_rcBtn;
		wnd.ClientToScreen(&rect);
		TPMPARAMS  TPMParams;
		TPMParams.cbSize	= sizeof (TPMPARAMS);
		TPMParams.rcExclude = rect;

		m_menuSub.TrackPopupMenuEx(
					TPM_LEFTBUTTON|TPM_VERTICAL|TPM_LEFTALIGN|TPM_TOPALIGN|(bW2K ? (true ? TPM_VERPOSANIMATION : TPM_NOANIMATION) : 0),
					rect.left,
					rect.bottom,
					hWndMenuOwner,
					&TPMParams );
	}


private:
	// Implementation
	void _DrawText( CDCHandle dc, CPoint ptOffset = CPoint(0, 0) )
	{
		COLORREF	clr;

		if (m_fsState & CBSTATE_INACTIVE)
			clr = ::GetSysColor(COLOR_GRAYTEXT);
		else if (m_fsState & CBSTATE_ENABLED)
			clr = ::GetSysColor(COLOR_MENUTEXT);
		else
			clr = ::GetSysColor(COLOR_3DSHADOW);

		COLORREF clrOld = dc.SetTextColor(clr);
		CRect	 rcBtn	= m_rcBtn + ptOffset;

		if ( !(m_fsState & CBSTATE_ENABLED) ) {
			// disabled - draw shadow text shifted down and right 1 pixel (unles selected)
			CRect	 rcDisabled = rcBtn + CPoint(1, 1);
			COLORREF clrOld2	= dc.SetTextColor( ::GetSysColor(COLOR_3DHILIGHT) );
			dc.DrawText(m_strBtn, -1, rcDisabled, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			dc.SetTextColor(clrOld2);
		}

		dc.DrawText(m_strBtn, -1, rcBtn, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		dc.SetTextColor(clrOld);
	}


	void DrawHot(CDCHandle dc)
	{
		dc.DrawEdge(m_rcBtn, BDR_RAISEDINNER, BF_RECT);
		//		dc.FillRect(&m_rcBtn, (HBRUSH)LongToPtr(COLOR_HIGHLIGHT + 1));
		//		dc.DrawFrameControl(m_rcBtn, DFC_BUTTON, DFCS_FLAT|DFCS_BUTTONPUSH);
		_DrawText(dc);
	}


	void DrawPressed(CDCHandle dc)
	{
		dc.DrawEdge(m_rcBtn, BDR_SUNKENOUTER, BF_RECT);
		//		dc.FillRect(&m_rcBtn, (HBRUSH)LongToPtr(COLOR_HIGHLIGHT + 1));
		//		dc.DrawFrameControl(m_rcBtn, DFC_BUTTON, DFCS_FLAT|DFCS_BUTTONPUSH|DFCS_PUSHED);
		_DrawText( dc, CPoint(1, 1) );
	}
};



/////////////////////////////////////////////////////////////////////////////
// CCommandBarCtrl2Base - base class for the Command Bar2 implementation

class CCommandBarCtrl2Base : public CWindow {
public:
	static bool 				 s_bW2K;		// For animation flag
	static CCommandBarCtrl2Base* s_pCurrentBar;
private:
	static bool 				 s_bStaticInit;
public:
	static HHOOK				 s_hMsgHook;

public:
	CCommandBarCtrl2Base()
	{
		// init static variables
		if (!s_bStaticInit) {
			CLockStaticDataInit lock;

			if (!s_bStaticInit) {
				// Animation on Win2000 only
				s_bW2K		  = !AtlIsOldWindows();
				// done
				s_bStaticInit = true;
			}
		}
	}

	virtual bool OnMenuInput(MSG *pMsg) = 0;
};



__declspec(selectany) CCommandBarCtrl2Base *	CCommandBarCtrl2Base::s_pCurrentBar = NULL;
__declspec(selectany) bool						CCommandBarCtrl2Base::s_bW2K		= false;
__declspec(selectany) bool						CCommandBarCtrl2Base::s_bStaticInit = false;
__declspec(selectany) HHOOK 					CCommandBarCtrl2Base::s_hMsgHook	= NULL;



/////////////////////////////////////////////////////////////////////////////
// CCommandBarCtrl2 - MTL implementation of Command Bars

template <class T, class TBase = CCommandBarCtrl2Base, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CCommandBarCtrl2Impl
	: public CWindowImpl< T, TBase, TWinTraits >
	, public CTrackMouseLeave<CCommandBarCtrl2Impl< T, TBase, TWinTraits> >
{
public:
	DECLARE_WND_CLASS_EX(NULL, 0, -1)

	// Constants
	enum _CmdBarDrawConstants {
		s_kcxTextMargin = 7,
		s_kcyTextMargin = 2
	};

	enum {
		_nMaxMenuItemTextLength = 100,
		_chChevronShortcut		= _T('/')
	};

	enum _TrackingState {
		none = 0,
		hotByAlt,
		popup,
		hotByMouse
	};

	// Data members
	HMENU						m_hMenu;
	CSimpleArray<CCmdBarButton> m_arrBtn;
	_TrackingState				m_nTrackingState;

	CPoint						m_ptMouse;
	int 						m_nPopBtn;
	bool						m_bLoop;
	bool						m_bProcessRightArrow;
	bool						m_bProcessLeftArrow;
	bool						m_bIgnoreAlt;
	bool						m_bIgnoreInputMouseMove;

	CContainedWindow			m_wndParent;

	CFont						m_fontMenu;

	DWORD						m_dwExtendedStyle; // Command Bar2 specific extended styles
	int 						m_nIDEvent;


public:
	// Constructor/destructor
	CCommandBarCtrl2Impl()
		: m_hMenu(NULL)
		, m_wndParent(this, 1)
		, m_nPopBtn(-1)
		, m_dwExtendedStyle(CBR2_EX_TRANSPARENT | CBR2_EX_SHAREMENU)
		, m_bLoop(false)
		, m_bProcessRightArrow(false)
		, m_bProcessLeftArrow(false)
		, m_bIgnoreAlt(false)
		, m_bIgnoreInputMouseMove(false)
		, m_nTrackingState(none)
		, m_ptMouse(-1, -1)
		, m_nIDEvent(0)
	{
	}


	~CCommandBarCtrl2Impl()
	{
		if ( m_wndParent.IsWindow() )
			/*scary!*/ m_wndParent.UnsubclassWindow();

		if (m_hMenu != NULL && (m_dwExtendedStyle & CBR2_EX_SHAREMENU) == 0)
			::DestroyMenu(m_hMenu);
	}


private:
	// Attributes
	DWORD GetCommandBar2ExtendedStyle() const
	{
		return m_dwExtendedStyle;
	}


	DWORD SetCommandBar2ExtendedStyle(DWORD dwExtendedStyle, DWORD dwMask = 0)
	{
		DWORD	dwPrevStyle = m_dwExtendedStyle;

		if (dwMask == 0)
			m_dwExtendedStyle = dwExtendedStyle;
		else
			m_dwExtendedStyle = (m_dwExtendedStyle & ~dwMask) | (dwExtendedStyle & dwMask);

		return dwPrevStyle;
	}


public:
	CMenuHandle GetMenu() const
	{
		ATLASSERT( ::IsWindow(m_hWnd) );
		return m_hMenu;
	}


private:
	int GetButtonCount() const
	{
		return m_arrBtn.GetSize();
	}


public:
	// Methods
	HWND Create(
			HWND		hWndParent,
			RECT &		rcPos,
			LPCTSTR 	szWindowName = NULL,
			DWORD		dwStyle 	 = 0,
			DWORD		dwExStyle	 = 0,
			UINT		nID 		 = 0,
			LPVOID		lpCreateParam = NULL)
	{
		return	CWindowImpl< T, TBase, TWinTraits >
						::Create( hWndParent, rcPos, szWindowName, dwStyle, dwExStyle,nID, lpCreateParam );
	}


private:
	BOOL LoadMenu(_U_STRINGorID menu)
	{
		ATLASSERT( ::IsWindow(m_hWnd) );

		if (menu.m_lpstr == NULL)
			return false;

		HMENU hMenu = ::LoadMenu(_Module.GetResourceInstance(), menu.m_lpstr);
		if (hMenu == NULL)
			return false;

		return AttachMenu(hMenu);
	}


public:
	BOOL AttachMenu(HMENU hMenu)
	{
		ATLASSERT( ::IsWindow(m_hWnd) );
		ATLASSERT( ::IsMenu(hMenu) );

		if ( hMenu != NULL && !::IsMenu(hMenu) )
			return false;

		// destroy old menu, if needed, and set new one
		if (m_hMenu != NULL && (m_dwExtendedStyle & CBR2_EX_SHAREMENU) == 0
			&& m_hMenu != hMenu 		//+++ 追加
		){
			::DestroyMenu(m_hMenu);
		}
		m_hMenu = hMenu;

		// Clear all
		_ClearAll();
		// Add buttons for each menu item
		_AddButtons(m_hMenu);

		Invalidate();
		UpdateWindow();

		return true;
	}


public:
	void EnableButton(int nIndex, bool bEnable = true)
	{
		if ( !_IsValidIndex(nIndex) )
			return;

		BYTE fsRemove, fsAdd;

		if (bEnable) {
			fsRemove = 0;
			fsAdd	 = CBSTATE_ENABLED;
		} else {
			fsRemove = CBSTATE_ENABLED;
			fsAdd	 = 0;
		}

		if ( m_arrBtn[nIndex].ModifyState(fsRemove, fsAdd) ) {
			InvalidateRect(m_arrBtn[nIndex].m_rcBtn);
			UpdateWindow();
		}
	}


public:
	void RefreshBandIdealSize(CReBarCtrl rebar)
	{
		REBARBANDINFO rbBand = { sizeof (REBARBANDINFO) };
		rbBand.fMask  = RBBIM_IDEALSIZE | RBBIM_CHILDSIZE;
						// | RBBIM_SIZE //+++

		int nIndex = rebar.IdToIndex( GetDlgCtrlID() );
		rebar.GetBandInfo(nIndex, &rbBand);

		int nCount = GetButtonCount();
		if (nCount != 0) {
			rbBand.cxIdeal		= m_arrBtn[nCount - 1].m_rcBtn.right;
			// rbBand.cx		= rbBand.cxIdeal;				//+++
		  #if 0 //+++ undonut r13test での処理
			rbBand.cxMinChild	= m_arrBtn[0].m_rcBtn.Width();
		  #else //+++ undonut r13test +4 での処理
			rbBand.cxMinChild	= 0;
		  #endif
		}

		rebar.SetBandInfo(nIndex, &rbBand);
	}


	// U.H(コピー)
	void SetMenuLogFont(const LOGFONT &lf)
	{
		CFontHandle font;

		MTLVERIFY( font.CreateFontIndirect(&lf) );

		if (font.m_hFont) {
			if (m_fontMenu.m_hFont != NULL)
				m_fontMenu.DeleteObject();

			m_fontMenu.Attach(font.m_hFont);
			SetFont(m_fontMenu);
			m_wndParent.SetFont(font.m_hFont);		//+++

			Invalidate();
		}
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CCommandBarCtrl2Impl)
		MESSAGE_HANDLER(WM_CREATE			, OnCreate		)
		MESSAGE_HANDLER(WM_PAINT			, OnPaint		)
		MESSAGE_HANDLER(WM_SIZE 			, OnSize		)		//+++
		MESSAGE_HANDLER(WM_LBUTTONDOWN		, OnLButtonDown )
		MESSAGE_HANDLER(WM_ERASEBKGND		, OnEraseBackground)
		MESSAGE_HANDLER(WM_RBUTTONUP		, OnRButtonUp	)
		CHAIN_MSG_MAP(CTrackMouseLeave<CCommandBarCtrl2Impl>)
	ALT_MSG_MAP(1)		// Parent window messages
		MESSAGE_HANDLER(WM_ACTIVATE 		, OnParentActivate)
		MESSAGE_HANDLER(WM_MENUSELECT		, OnMenuSelect	)
	   #if (_WIN32_IE >= 0x0500)
		NOTIFY_CODE_HANDLER(RBN_CHEVRONPUSHED, OnChevronPushed)
	  #endif
	END_MSG_MAP()


private:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		// Let the toolbar initialize itself
		LRESULT lRet			  = DefWindowProc(uMsg, wParam, lParam);

		// get and use system settings
		GetSystemSettings();

		// Parent init
		CWindow wndParent		  = GetParent();
		CWindow wndTopLevelParent = wndParent.GetTopLevelParent();
		m_wndParent.SubclassWindow(wndTopLevelParent);

		return lRet;
	}


	void _DoPaint(CDCHandle dc, LPCRECT lpRect = NULL)
	{
		CFontHandle fontOld = dc.SelectFont(m_fontMenu.m_hFont);
		int 		modeOld = dc.SetBkMode(TRANSPARENT);

		for (int i = 0; i < m_arrBtn.GetSize(); ++i) {
			if ( lpRect == NULL || MtlIsCrossRect(m_arrBtn[i].m_rcBtn, lpRect) ) {
				m_arrBtn[i].Update(dc.m_hDC);
			}
		}

		dc.SelectFont(fontOld);
		dc.SetBkMode(modeOld);
	}


  #if 1 //+++
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &bHandled)
	{
		CRect rc;
		GetClientRect(rc);
		InvalidateRect( rc );
		return 0;
	}
  #endif


	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &bHandled)
	{
		// UDT JOBBY
		// メニューバーを透明にする
		// 参照: http://www.microsoft.com/japan/developer/library/vccore/vcconusingdialogbarwithrebarcontrol.htm
		HWND	hWnd	= GetParent();
		CPoint	pt(0, 0);

		MapWindowPoints(hWnd, &pt, 1);
		pt = ::OffsetWindowOrgEx( (HDC) wParam, pt.x, pt.y, NULL );
		LRESULT lResult = ::SendMessage(hWnd, WM_ERASEBKGND, wParam, 0L);
		//		::SetWindowOrgEx((HDC)wParam, pt.x, pt.y, NULL);
		::SetWindowOrgEx( (HDC) wParam, 0, 0, NULL );

		return lResult;
		// ENDE
	}


	LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;

		UINT  uItem = (UINT) LOWORD(wParam);					// menu item or submenu index
		//x 	UINT fuFlags = (UINT) HIWORD(wParam); // menu flags
		HMENU hmenu = (HMENU) lParam;

		if (m_nTrackingState == popup && hmenu != m_hMenu) {	// it's not mine
			// if no submenu, ->
			m_bProcessRightArrow = ( ::GetSubMenu(hmenu, uItem) == NULL
								   || ::GetMenuState(hmenu, uItem, MF_BYPOSITION) & (MF_GRAYED | MF_DISABLED) );
			// if top leveled menu popping up, <-
			HMENU hSubMenu = ::GetSubMenu(m_hMenu, m_nPopBtn);
			m_bProcessLeftArrow  = (hmenu == hSubMenu);
		}

		return 1;
	}


	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if (wParam != NULL) {
			_DoPaint( (HDC) wParam );
		} else {
			CPaintDC dc(m_hWnd);
			_DoPaint(dc.m_hDC, &dc.m_ps.rcPaint);
		}

		return 0;
	}


	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		POINT pt	 = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		m_bIgnoreInputMouseMove = false;
		int   nIndex = _HitTest(pt);
		if (nIndex != -1)
			_DoTrackPopupMenu(nIndex);

	  #if 1 //+++ メインフレームのキャプション無しにした場合に、メニューバーの隙間をつかんで、ウィンドウの移動をできるようにする.
	   #if 0
		Donut_FakeCaptionLButtonDown(m_hWnd, GetTopLevelWindow(), lParam);
	   #else
		if (CSkinOption::s_nMainFrameCaption == 0) {
		  #if 1 //+++ メニューのお気に入りを出して、その項目を移動しようとしたら窓が移動してしまうので、その対策
			CRect	rc;
			GetWindowRect(rc);
			GetCursorPos(&pt);
			if (rc.PtInRect(pt))
		  #endif
			{
				HWND	hWndTopLevelParent = GetTopLevelWindow();
				::PostMessage(hWndTopLevelParent, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
			}
		}
	   #endif
	  #endif

		return 0;
	}


	LRESULT OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		::SendMessage(GetTopLevelParent(), WM_SHOW_TOOLBARMENU, 0, 0);
		return 0;
	}


public:
	// Overrides
	void OnTrackMouseMove(UINT nFlags, CPoint pt)
	{
		int nIndex = _HitTest(pt);

		if ( _IsValidIndex(nIndex) ) {
			if (m_nPopBtn == -1 || m_nPopBtn != nIndex) {	// if other button
				_UpdateBar(hotByMouse, nIndex); 			// button made hot with mouse
			}
		} else {
			_UpdateBar();
		}
	}


	void OnTrackMouseLeave()
	{
		if (m_nTrackingState != popup) {
			_UpdateBar();
		}
	}


private:
	LRESULT OnParentActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bool bParentActive = (wParam != WA_INACTIVE);

		if (!bParentActive)
			_UpdateBar();

		for (int i = 0; i < m_arrBtn.GetSize(); ++i) {
			if (bParentActive)
				m_arrBtn[i].ModifyState(CBSTATE_INACTIVE, 0);
			else
				m_arrBtn[i].ModifyState(0, CBSTATE_INACTIVE);
		}

		Invalidate();
		UpdateWindow();
		bHandled = FALSE;
		return 1;
	}


  #if (_WIN32_IE >= 0x0500)
	LRESULT OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
	{
		LPNMREBARCHEVRON lpnm = (LPNMREBARCHEVRON) pnmh;

		if ( lpnm->wID != GetDlgCtrlID() ) {
			bHandled = FALSE;
			return 1;
		}

		CMenuHandle 	 menu = _PrepareChevronMenu();
		_DisplayChevronMenu(menu, lpnm);
		_CleanupChevronMenu(menu, lpnm);

		return 0;
	}
  #endif


	// Implementation - Hook proc
	static LRESULT CALLBACK MessageHookProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		LPMSG pMsg = (LPMSG) lParam;

		return ( nCode == MSGF_MENU && s_pCurrentBar && s_pCurrentBar->OnMenuInput(pMsg) )
			   ? true : ::CallNextHookEx(s_hMsgHook, nCode, wParam, lParam);
	}


	void _CloseMenu()
	{
		//		m_wndParent.PostMessage(WM_KEYDOWN, VK_ESCAPE);
		m_wndParent.PostMessage(WM_CANCELMODE);
	}


	virtual bool OnMenuInput(MSG *pMsg)
	{
		int 	nMsg = pMsg->message;
		CPoint	pt	 = DWORD( pMsg->lParam );

		ScreenToClient(&pt);

		switch (nMsg) {
		case WM_MOUSEMOVE:
			if (pt != m_ptMouse && !m_bIgnoreInputMouseMove) {
				m_bIgnoreInputMouseMove = false;
				int nIndex = _HitTest(pt);

				if (_IsValidIndex(nIndex) && nIndex != m_nPopBtn) {
					// defferent button clicked
					//					ATLTRACE(_T("defferent button clicked\n"));
					_CloseMenu();
					_UpdateBar();							// clean up
					m_nPopBtn = nIndex;
					m_bLoop   = true;						// continue loop
				}
				m_ptMouse	  = pt;
			}
			break;

		case WM_LBUTTONDOWN:	{
				int nIndex = _HitTest(pt);

				if (nIndex != -1 && nIndex == m_nPopBtn) {
					// same button clicked
					_CloseMenu();
					_UpdateBar(hotByMouse, m_nPopBtn);
					m_bLoop = false;						// out of loop
					return true;							// eat it!
				}
			}
			break;

		case WM_DRAWITEM:
			break;

		case WM_RBUTTONDOWN:
			break;

		case WM_KEYDOWN:	{
				TCHAR vKey = (TCHAR) pMsg->wParam;

				if ( (vKey == VK_LEFT  && m_bProcessLeftArrow)
				   || (vKey == VK_RIGHT && m_bProcessRightArrow) )
				{
					// no submenu
					int nNewIndex = _GetNextOrPrevButton(m_nPopBtn, vKey == VK_LEFT);
					_CloseMenu();
					_UpdateBar();
					m_nPopBtn = nNewIndex;
					m_bLoop   = true;						// continue loop
					return true;							// eat it!
				}
			}
			break;

		case WM_SYSKEYDOWN:
			//		LTRACE(_T("    m_bIgnore = true\n"));
			m_bIgnoreAlt = true;							// next SysKeyUp will be ignored
			break;
		}

		return false;										// pass along...
	}


	bool _IsAltKeyDown(MSG *pMsg)
	{
		bool bAlt = HIWORD(pMsg->lParam) & KF_ALTDOWN;
		bool bExt = HIWORD(pMsg->lParam) & 0x0100;

		return bAlt && !bExt;
	}


	BOOL PreTranslateMessagePrevVer(MSG *pMsg)
	{
		if (!m_hWnd)
			return FALSE;

		UINT nMsg = pMsg->message;

		if (WM_LBUTTONDOWN <= nMsg && nMsg <= WM_MOUSELAST) {
			if (pMsg->hwnd != m_hWnd && m_nTrackingState > 0) {
				// clicked outside
				_UpdateBar();
			}
		} else if (nMsg == WM_SYSKEYDOWN || nMsg == WM_SYSKEYUP || nMsg == WM_KEYDOWN) {
			bool  bAlt = _IsAltKeyDown(pMsg);			// Alt key pressed?
			TCHAR vkey = pMsg->wParam;					// + X key
			if ( vkey == VK_MENU
			   || ( vkey == VK_F10 && ! ( (GetKeyState(VK_SHIFT)   & 0x80000000)
									   || (GetKeyState(VK_CONTROL) & 0x80000000) || bAlt ) ) )
			{
				// only alt key pressed
				if (nMsg == WM_SYSKEYUP) {
					switch (m_nTrackingState) {
					case hotByMouse:					// NOTE. the mouse hook send WM_MOUSEMOVE even if mouse not moving...
						m_bIgnoreInputMouseMove = true;
						// break;			// do nothing

					case none:
						if (m_bIgnoreAlt) {
							m_bIgnoreAlt = false;
							break;
						}
						_UpdateBar(hotByAlt, 0);
						break;

					case hotByAlt:
						_UpdateBar();
						break;
					}
				}
				return true;

			} else if ( (nMsg == WM_SYSKEYDOWN || nMsg == WM_KEYDOWN) ) {
				if (m_nTrackingState == hotByAlt) {
					switch (vkey) {
					case VK_LEFT:
					case VK_RIGHT: {
							int nNewIndex  = _GetNextOrPrevButton(m_nPopBtn, vkey == VK_LEFT);
							_UpdateBar(hotByAlt, nNewIndex);
							return true;
						}

					case VK_SPACE:
					case VK_UP:
					case VK_DOWN:
						_DoTrackPopupMenu(m_nPopBtn);
						return true;

					case VK_ESCAPE:
						_UpdateBar();
						return true;
					}
				}

				// Alt + X pressed
				if ( (bAlt || m_nTrackingState == hotByAlt) && Mtl_istalnum(vkey) ) {
					int nIndex;

					if ( _MapAccessKey(vkey, nIndex) ) {
						_UpdateBar();
						_DoTrackPopupMenu(nIndex);
						return true;	// eat it!
					} else if (m_nTrackingState == hotByAlt && !bAlt) {
						//					MessageBeep(0); 	// if you want
						return true;
					}
				}

				if (m_nTrackingState > 0) { 					// unknown key
					if (m_nTrackingState != hotByMouse) {		// if tracked with mouse, don't update bar
						_UpdateBar();
					}
				}
			}
		}

		return FALSE;		// pass along...
	}


public:
	BOOL PreTranslateMessage(MSG *pMsg)
	{
		if (m_hWnd == NULL)
			return FALSE;

		int  nVirtKey = (int) pMsg->wParam;
		UINT message  = pMsg->message;

		if (message == WM_SYSKEYUP && nVirtKey == VK_MENU) {	// just alt key up
			switch (m_nTrackingState) {
			case hotByMouse:									// NOTE. the mouse hook send WM_MOUSEMOVE even if mouse not moving...
				m_bIgnoreInputMouseMove = true;
				// break;			// do nothing

			case none:
				if (m_bIgnoreAlt) {
					m_bIgnoreAlt = false;
					break;
				}
				_UpdateBar(hotByAlt, 0);
				break;

			case hotByAlt:
				_UpdateBar();
				break;
			}
			return TRUE;		// eat it

		} else if (message == WM_KEYDOWN && m_nTrackingState == hotByAlt) { // alt key tracking
			switch (nVirtKey) {
			case VK_LEFT:
			case VK_RIGHT: {
					int nNewIndex  = _GetNextOrPrevButton(m_nPopBtn, nVirtKey == VK_LEFT);
					_UpdateBar(hotByAlt, nNewIndex);
					return TRUE;
				}

			case VK_RETURN:
			case VK_UP:
			case VK_DOWN:
				_DoTrackPopupMenu(m_nPopBtn);
				return TRUE;

			case VK_ESCAPE:
				_UpdateBar();
				return TRUE;
			}

		} else if (pMsg->message == WM_SYSKEYDOWN) {	// access key
			if (nVirtKey == VK_MENU || nVirtKey == VK_SHIFT)
				return FALSE;

			// Alt + X pressed
			if ( Mtl_istalnum(nVirtKey) ) {
				int nIndex;

				if ( _MapAccessKey(nVirtKey, nIndex) ) {
					_UpdateBar();
					_DoTrackPopupMenu(nIndex);
					return TRUE;	// eat it!
				}
			}

			// unknown access key
			if (m_nTrackingState != hotByMouse) {
				_UpdateBar();
			}
		}

		return FALSE;				// pass along...
	}


private:
	// Implementation
	void _DoTrackPopupMenu(int nIndex)
	{
		ATLASSERT( _IsValidIndex(nIndex) );

		m_nPopBtn = nIndex;
		m_bLoop   = true;

		while (m_bLoop) {
//m_arrBtn[nIndex].m_hFont = m_fontMenu.m_hFont;			//+++
			//			UpdateWindow();// force to repaint when button hidden by other window
			_UpdateBar(popup, m_nPopBtn);

			// install hook
			ATLASSERT(s_pCurrentBar == NULL);
			s_pCurrentBar	 = this;
			ATLASSERT(s_hMsgHook == NULL);

			m_bLoop 		 = false;
			s_hMsgHook		 = ::SetWindowsHookEx( WH_MSGFILTER, MessageHookProc, NULL, ::GetCurrentThreadId() );	// m_bLoop may become true

			// popup!!
			m_nTrackingState = popup;
			m_wndParent.SendMessage(WM_MENUSELECT, MAKEWPARAM(m_nPopBtn, MF_POPUP), (LPARAM) m_hMenu);

			// for help message line
			//メニューバーの項目を開いた直後にカーソルを他の項目に移動させると出るエラー対策
			if ( _IsValidIndex(m_nPopBtn) ) {
				ATLASSERT( _IsValidIndex(m_nPopBtn) );
				m_arrBtn[m_nPopBtn].TrackPopup(m_hWnd, m_wndParent, s_bW2K);
			}

			// uninstall hook
			::UnhookWindowsHookEx(s_hMsgHook);
			s_hMsgHook		 = NULL;
			s_pCurrentBar	 = NULL;
		}

		_UpdateBar();
	}


	void _UpdateBar(_TrackingState nState = none, int nNewPopBtn = -1)
	{
		if (m_nTrackingState == hotByMouse)
			m_bIgnoreAlt = false;

		// if prev state is hotByMouse, always should be false!
		m_nTrackingState	 = nState;

		// clean up
		if ( _IsValidIndex(m_nPopBtn) ) {
			m_arrBtn[m_nPopBtn].ModifyState(CBSTATE_HOT | CBSTATE_PRESSED, 0);
			InvalidateRect(m_arrBtn[m_nPopBtn].m_rcBtn);
			UpdateWindow();
		}

		m_nPopBtn			 = -1;

		//メニューバーの項目を開いた直後にカーソルを他の項目に移動させると出るエラー対策
		if ( nState != none && _IsValidIndex(nNewPopBtn) ) {
			ATLASSERT( _IsValidIndex(nNewPopBtn) );
			m_nPopBtn = nNewPopBtn;
			CCmdBarButton &cbbtn = m_arrBtn[nNewPopBtn];

			if (nState == hotByAlt || nState == hotByMouse) {
				cbbtn.ModifyState(CBSTATE_PRESSED, CBSTATE_HOT);
				InvalidateRect(cbbtn.m_rcBtn);
				UpdateWindow();
			} else if (nState == popup) {
				cbbtn.ModifyState(CBSTATE_HOT, CBSTATE_PRESSED);
				InvalidateRect(cbbtn.m_rcBtn);
				UpdateWindow();
			}
		} else {
			// must be default parameter
			ATLASSERT(nNewPopBtn == -1);
		}

		m_bProcessRightArrow = m_bProcessLeftArrow = true;
	}


	bool _MapAccessKey(TCHAR cAccessKey, int &nIndex)
	{
		for (int i = 0; i < m_arrBtn.GetSize(); ++i) {
			// fixing point
			TCHAR	cKey = m_arrBtn[i].m_cAccessKey;

			if (   cKey
				&& Mtl_tolower(cKey) /*_totupper(cKey)*/ == Mtl_tolower(cAccessKey)
				&& (m_arrBtn[i].m_fsState & CBSTATE_ENABLED)
			){
				nIndex = i;
				return true;
			}
		}

		return false;
	}


	int _HitTest(CPoint point)
	{
		CRect	rc;
		GetClientRect(&rc);
		if ( !rc.PtInRect(point) )
			return -1;
		if (m_nTrackingState == popup) {
			CPoint ptScreen = point;
			ClientToScreen(&ptScreen);

			if (::WindowFromPoint(ptScreen) != m_hWnd)	// the puzzle figured out!
				return -1;
		}

		for (int i = 0; i < m_arrBtn.GetSize(); ++i) {
			if (m_arrBtn[i].m_rcBtn.PtInRect(point)
			   && m_arrBtn[i].m_fsState & CBSTATE_ENABLED)
			{
				return i;
			}
		}

		return -1;
	}


	int _GetNextOrPrevButton(int nIndex, BOOL bPrev)
	{
		int nCount = m_arrBtn.GetSize();

		if (bPrev) {	// <-
			--nIndex;
			if (nIndex < 0)
				nIndex = nCount - 1;
		} else {		// ->
			++nIndex;
			if (nIndex >= nCount)
				nIndex = 0;
		}

		if (  (m_arrBtn[nIndex].m_fsState & CBSTATE_HIDDEN )
		  || !(m_arrBtn[nIndex].m_fsState & CBSTATE_ENABLED) )
		{
			return _GetNextOrPrevButton(nIndex, bPrev);
		}

		return nIndex;
	}


	bool _IsValidIndex(int nIndex)
	{
		if ( 0 <= nIndex && nIndex < m_arrBtn.GetSize() )
			return true;
		else
			return false;
	}


	void _ClearAll()
	{
		m_arrBtn.RemoveAll();
	}


	void _AddButtons(HMENU hMenu)
	{
		ATLASSERT( ::IsMenu(hMenu) );
		int   nItems   = ::GetMenuItemCount(hMenu);

		T *   pT	   = static_cast<T *>(this);
		TCHAR szString[pT->_nMaxMenuItemTextLength + 4];
		int   cxOffset = 0;

		for (int i = 0; i < nItems; ++i) {
			CMenuItemInfo	mii;
			mii.fMask		= MIIM_TYPE | MIIM_STATE | MIIM_SUBMENU;
			mii.fType		= MFT_STRING;
			szString[0]		= 0;	//+++
			mii.dwTypeData	= szString;
			mii.cch 		= pT->_nMaxMenuItemTextLength;
			BOOL	bRet	= ::GetMenuItemInfo(m_hMenu, i, true, &mii);
			ATLASSERT(bRet);

			// If we have more than the buffer, we assume we have bitmaps bits
			//+++ if (::lstrlen(szString) > pT->_nMaxMenuItemTextLength - 1)
			if (bRet == 0 || ::lstrlen(szString) > pT->_nMaxMenuItemTextLength - 1)
			{
				mii.fType	= MFT_BITMAP;
				::SetMenuItemInfo(m_hMenu, i, true, &mii);
				szString[0] = 0;
			}

			// NOTE: Command Bar2 currently supports only drop-down menu items
			ATLASSERT(mii.hSubMenu != NULL);
			BYTE		  fsState = (BYTE) ( ( (mii.fState & MFS_DISABLED) == 0 ) ? CBSTATE_ENABLED : 0 );
			CCmdBarButton cbbtn(fsState, szString, mii.hSubMenu);
			cbbtn.m_rcBtn  = _MeasureButton(szString) + CPoint(cxOffset, 0);
			cxOffset	   = cxOffset + cbbtn.m_rcBtn.Width();
			m_arrBtn.Add(cbbtn);
			// ATLTRACE(_T("cxOffset : %d\n"), cxOffset);
		}

		MoveWindow( 0, 0, cxOffset, m_arrBtn[0].m_rcBtn.Height() );
	}


	CRect _MeasureButton(const CString &strText)
	{
		// compute size of text - use DrawText with DT_CALCRECT
		int 	cx		= MtlComputeWidthOfText(strText, m_fontMenu);
		LOGFONT 	lf;
		m_fontMenu.GetLogFont(lf);
		int 	cy		= lf.lfHeight;
		if (cy < 0)
			cy = -cy;
		cy += 2 * s_kcyTextMargin;

		// height of item is the bigger of these two
		//		cy = MtlMax(cy, ::GetSystemMetrics(SM_CYMENU) - 1);
		int 	nMenuCY = ::GetSystemMetrics(SM_CYMENU) - 1;
		cy += 2;

		// width is width of text plus a bunch of stuff
		//+++ tinyモードならマージン減らす
		if (CSkinOption::s_nMenuStyle != 1) 	//+++ 通常 か Englishのとき
			cx += 2 * s_kcxTextMargin;			// L/R margin for readability
		else									//+++	tinyモード
			cx += 2;							// L/R margin for readability

		return CRect(0, 0, cx, cy);
	}


	CMenuHandle _PrepareChevronMenu()
	{
		CMenuHandle menuCmdBar(m_hMenu);

		// build a menu from hidden items
		CMenuHandle menu;
		BOOL		bRet = menu.CreatePopupMenu();

		ATLASSERT(bRet);
		RECT		rcClient = {0};
		bRet = GetClientRect(&rcClient);
		ATLASSERT(bRet);

		int client_right = rcClient.right;
		unsigned	num = m_arrBtn.GetSize();
		for (unsigned i = 0; i < num; ++i) {
			CCmdBarButton cbb	   = m_arrBtn[i];
			bool		  bEnabled = ( (cbb.m_fsState & CBSTATE_ENABLED) != 0 );

			int cbb_btn_right = cbb.m_rcBtn.right;
			if (cbb_btn_right > client_right) {
				enum { BUFF_SZ = 100 };
				TCHAR			szBuff[BUFF_SZ+1];
				szBuff[0]	   = 0;	//+++
				CMenuItemInfo	mii;
				mii.fMask	   = MIIM_TYPE | MIIM_SUBMENU;
				mii.dwTypeData = szBuff;
				mii.cch 	   = BUFF_SZ;	// sizeof (szBuff) / sizeof (TCHAR);
				bRet		   = menuCmdBar.GetMenuItemInfo(i, TRUE, &mii);
				ATLASSERT(bRet);
				// Note: CmdBar currently supports only drop-down items
				ATLASSERT( ::IsMenu(mii.hSubMenu) );
				bRet		   = menu.AppendMenu( MF_STRING|MF_POPUP|(bEnabled ? MF_ENABLED : MF_GRAYED),
												 (UINT_PTR) mii.hSubMenu,
												 mii.dwTypeData );
				ATLASSERT(bRet);
			}
		}

		if (menu.m_hMenu && menu.GetMenuItemCount() == 0) { // no hidden buttons after all
			menu.DestroyMenu();
			return NULL;
		}

		return menu;
	}


  #if (_WIN32_IE >= 0x0500)
	void _DisplayChevronMenu(CMenuHandle menu, LPNMREBARCHEVRON lpnm)
	{
	  #if 1 //+++
		if (menu.m_hMenu == 0)
			return;
	  #endif
		// convert chevron rect to screen coordinates
		CWindow   wndFrom	 = lpnm->hdr.hwndFrom;
		RECT	  rc		 = lpnm->rc;

		wndFrom.ClientToScreen(&rc);
		// set up flags and rect
		UINT		uMenuFlags = TPM_LEFTBUTTON | TPM_VERTICAL | TPM_LEFTALIGN | TPM_TOPALIGN
							   | (!AtlIsOldWindows() ? TPM_VERPOSANIMATION : 0);
		TPMPARAMS	TPMParams;
		TPMParams.cbSize	= sizeof (TPMPARAMS);
		TPMParams.rcExclude = rc;
		::TrackPopupMenuEx(menu.m_hMenu, uMenuFlags, rc.left, rc.bottom, m_wndParent, &TPMParams);
	}


	void _CleanupChevronMenu(CMenuHandle menu, LPNMREBARCHEVRON lpnm)
	{
		if (menu.m_hMenu) { //+++ NULLチェック追加
			// if menu is from a command bar, detach submenus so they are not destroyed
			for (int i = menu.GetMenuItemCount() - 1; i >= 0; i--)
				menu.RemoveMenu(i, MF_BYPOSITION);
		}

		// destroy menu
		menu.DestroyMenu();
		// convert chevron rect to screen coordinates
		CWindow wndFrom = lpnm->hdr.hwndFrom;
		RECT	rc		= lpnm->rc;
		wndFrom.ClientToScreen(&rc);
		// eat next message if click is on the same button
		MtlEatNextLButtonDownOnChevron(m_wndParent, rc);
	}
  #endif


	void GetSystemSettings()
	{
		// refresh our font
		NONCLIENTMETRICS info;

		info.cbSize = sizeof (info);
		::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof (info), &info, 0);
		LOGFONT 		 logfont;
		memset( &logfont, 0, sizeof (LOGFONT) );

		if (m_fontMenu.m_hFont != NULL)
			m_fontMenu.GetLogFont(logfont);

		if (  logfont.lfHeight			!= info.lfMenuFont.lfHeight
		   || logfont.lfWidth			!= info.lfMenuFont.lfWidth
		   || logfont.lfEscapement		!= info.lfMenuFont.lfEscapement
		   || logfont.lfOrientation 	!= info.lfMenuFont.lfOrientation
		   || logfont.lfWeight			!= info.lfMenuFont.lfWeight
		   || logfont.lfItalic			!= info.lfMenuFont.lfItalic
		   || logfont.lfUnderline		!= info.lfMenuFont.lfUnderline
		   || logfont.lfStrikeOut		!= info.lfMenuFont.lfStrikeOut
		   || logfont.lfCharSet 		!= info.lfMenuFont.lfCharSet
		   || logfont.lfOutPrecision	!= info.lfMenuFont.lfOutPrecision
		   || logfont.lfClipPrecision	!= info.lfMenuFont.lfClipPrecision
		   || logfont.lfQuality 		!= info.lfMenuFont.lfQuality
		   || logfont.lfPitchAndFamily	!= info.lfMenuFont.lfPitchAndFamily
		   || lstrcmp(logfont.lfFaceName, info.lfMenuFont.lfFaceName) != 0)
		{
			HFONT hFontMenu = ::CreateFontIndirect(&info.lfMenuFont);
			ATLASSERT(hFontMenu != NULL);

			if (hFontMenu != NULL) {
				if (m_fontMenu.m_hFont != NULL)
					m_fontMenu.DeleteObject();

				m_fontMenu.Attach(hFontMenu);
				SetFont(m_fontMenu);
			}
		}
	}
};



class CCommandBarCtrl2 : public CCommandBarCtrl2Impl<CCommandBarCtrl2> {
public:
	CCommandBarCtrl2() : m_hWndToolBar(0), m_mode(-1) {}

	DECLARE_WND_CLASS_EX(_T("MTL_CommandBar2"), NULL, COLOR_BTNFACE)

	void setTooBar(HWND hWndToolBar) { m_hWndToolBar = hWndToolBar; }

	///+++ メニューの文字列を CSkinOption::s_nMenuStyle(0:日本語 1:1文字名 2:英語)にしたがって設定.
	int setMenuBarStyle(HWND hWndToolBar=0, bool sameChk = true)
	{
		int mode = CSkinOption::s_nMenuStyle;
		if (mode == 0) {
			//mode = CMenuOption::s_nMenuBarStyle;
		}
		ATLASSERT(mode >= 0 && mode <= 2);
		if (mode < 0 || mode > 2)
			mode = 0;

		if (sameChk && mode == m_mode)
			return mode;

		static const TCHAR* tbl[7][3] = {
			//	DEFAULT 			 MINI
			{ _T("ファイル(&F)"),	_T("&F"), _T("&File")	, },
			{ _T("編集(&E)")	,	_T("&E"), _T("&Edit")	, },
			{ _T("表示(&V)")	,	_T("&V"), _T("&View")	, },
			{ _T("お気に入り(&A)"), _T("&A"), _T("F&avorites"), },
			{ _T("ツール(&T)"), 	_T("&T"), _T("&Tools")	, },
			{ _T("ウィンドウ(&W)"), _T("&W"), _T("&Window") , },
			{ _T("ヘルプ(&H)"), 	_T("&H"), _T("&Help")	, },
		};
		CMenuHandle 	   menu = this->m_hMenu;
		for (unsigned i = 0; i < 7; ++i) {
			const TCHAR*	s = tbl[i][mode];
			menu.ModifyMenu(i, MF_BYPOSITION|MF_STRING, (UINT_PTR)0/*submenu*/, s);
		}

		this->AttachMenu(this->m_hMenu);
		if (hWndToolBar == 0)
			hWndToolBar   = m_hWndToolBar;
		else
			m_hWndToolBar = hWndToolBar;
		if (hWndToolBar) {
			this->RefreshBandIdealSize(hWndToolBar);		// Chevronの範囲を再設定
		}
		m_mode = mode;
		return mode;
	}

private:
	HWND	m_hWndToolBar;
	int 	m_mode;
};



#ifdef __ATLCTRLW_H__
class CCoolMenuManager {
	WTL::CCommandBarCtrl	m_CmdBar;

public:
	BOOL Install(HWND hWnd)
	{
		return m_CmdBar.AttachToWindow(hWnd) != 0;
	}


	BOOL LoadImages(
			int *		__firstID,
			int *		__lastID,
			UINT		nImageBmpID,
			int 		cx,
			int 		cy,
			COLORREF	clrMask,
			UINT		nFlags = ILC_COLOR24)
	{
		return MtlCmdBarLoadImages(m_CmdBar, __firstID, __lastID, nImageBmpID, cx, cy, clrMask, nFlags);
	}


	void Validate(bool bOn)
	{
		m_CmdBar.SetImagesVisible(bOn);
	}

};
#endif



////////////////////////////////////////////////////////////////////////////

}		//namespace MTL


#endif	// __MTLCTRLW_H__
