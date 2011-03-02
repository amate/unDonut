/**
 *	@file	MenuControl.h
 */
#pragma once


#include "MtlBase.h"



class CMenuControl : public CWindowImpl<CMenuControl> {
	// Data members
	CMDIWindow	m_wndMDIClient;
	CWindow 	m_wndCmdSent;

	HWND		m_hWndNoFrameMode;	//+++

	CRect		m_arrCaption[3];
	BOOL		m_bPushed;
	int 		m_nTracking;
public:
	bool		m_bVisible;
private:
	bool		m_bOnlyCloseButton;
	bool		m_bNoButton;

	bool		m_bSpecialRestore;
	int 		m_nCmdIDRestore;


	// Constants
	enum _MDIMenuControlConstants {
		s_kcxGap = 2
	};

public:
	// Constructor
	CMenuControl()
		: m_bPushed(false)
		, m_nTracking(-1)
		, m_bVisible(false)
		, m_bOnlyCloseButton(false)
		, m_bSpecialRestore(false)
		, m_nCmdIDRestore(0)
		, m_bNoButton(false)
		, m_hWndNoFrameMode(0)	//+++
	{
	}


	~CMenuControl()
	{
	}


	void InstallAsMDICmdBar(HWND hWndCmdBar, HWND hWndMDIClient, bool bOnlyCloseButton)
	{
		ATLASSERT( ::IsWindow(hWndMDIClient) );

		// calc init layout
		CRect		rc;
		::GetClientRect(hWndCmdBar, &rc);
		_UpdateLayout( rc.Size() );

		MTLVERIFY( SubclassWindow(hWndCmdBar) );

		m_wndMDIClient.m_hWnd		   = hWndMDIClient;
		m_wndMDIClient.m_hWndMDIClient = hWndMDIClient;

		m_bOnlyCloseButton			   = bOnlyCloseButton;
	}


	void InstallAsStandard(HWND hWndDrawn, HWND hWndCmdSent, bool bSpecialRestore = false, int nID = 0, bool bOnlyCloseButton = false)
	{
		// calc init layout
		CRect		rc;
		::GetClientRect(hWndDrawn, &rc);
		_UpdateLayout( rc.Size() );

		MTLVERIFY( SubclassWindow(hWndDrawn) );

		m_wndCmdSent	   = hWndCmdSent;
		m_bSpecialRestore  = bSpecialRestore;
		m_nCmdIDRestore    = nID;

		m_bOnlyCloseButton = bOnlyCloseButton;
	}


	void Uninstall()
	{
		if ( IsWindow() )
			UnsubclassWindow();
	}


	void UpdateMDIMenuControl()
	{
		bool bPrev = m_bVisible;

		m_bVisible = _IsChildWindowMaximized();

		if (bPrev != m_bVisible)
			_InvalidateControls();
	}


  #if 1 //+++
	void setOnlyCloseButton(bool bOnlyCloseButton) {
		if (m_bOnlyCloseButton != bOnlyCloseButton) {
			m_bOnlyCloseButton = bOnlyCloseButton;
			for (int i = 0; i < 3; ++i) {
				InvalidateRect(m_arrCaption[i]);
			}
		}
	}
  #endif



private:
	void ShowMDIMenuControl(bool bOn)
	{
		m_bVisible = bOn;
		_InvalidateControls();
	}


public:
	void ShowButton(bool bShow)
	{
		if (m_hWndNoFrameMode) return;	//+++

		m_bNoButton = !bShow;
		_InvalidateControls();
	}


  #if 1 //+++ ï£ñ≥ÇµópÇ…ÅA_ox ÇÃàµÇ¢ÇïœçXÇ∑ÇÈ.
	void SetExMode(HWND hWnd)
	{
		if (m_bNoButton && hWnd) {
			m_hWndNoFrameMode = 0;
			ShowButton(1);
		}
		m_hWndNoFrameMode = hWnd;
	}
  #endif

	// Message map and handlers
	BEGIN_MSG_MAP(CMenuControl)
		// for drawn window
		MESSAGE_HANDLER 	(WM_PAINT, OnPaint	  )
		MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
		MSG_WM_LBUTTONDOWN	(OnLButtonDown)
		MSG_WM_MOUSEMOVE	(OnMouseMove  )
		MSG_WM_LBUTTONUP	(OnLButtonUp  )
	END_MSG_MAP()


private:
	void OnLButtonDown(UINT nFlags, CPoint pt)
	{
		SetMsgHandled( _OnMouseMsg(WM_LBUTTONDOWN, nFlags, pt) );
	}


	void OnMouseMove(UINT nFlags, CPoint pt)
	{
		SetMsgHandled( _OnMouseMsg(WM_MOUSEMOVE, nFlags, pt) );
	}


	void OnLButtonUp(UINT nFlags, CPoint pt)
	{
		SetMsgHandled( _OnMouseMsg(WM_LBUTTONUP, nFlags, pt) );
	}


	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		// ATLTRACE2(atlTraceGeneral, 4, _T("CMDIMenuControl::OnPaint\n"));
		if ((!m_bVisible || m_bNoButton)
			&& m_hWndNoFrameMode == 0		//+++
		){
			bHandled = FALSE;
			return 0;
		}

		CRect	rc;
		GetUpdateRect(&rc, FALSE);

		LRESULT lRet = DefWindowProc();

		if (_HitTest(rc) != -1) {
			CClientDC dc(m_hWnd);
			_DoPaint(dc.m_hDC);
		}

		return lRet;
	}


	void OnWindowPosChanged(LPWINDOWPOS lpwndpos)
	{
		ATLTRACE2(atlTraceGeneral, 4, _T("CMDIMenuControl::OnWindowPosChanged (%d, %d)\n"), lpwndpos->cx, lpwndpos->cy);
		SetMsgHandled(FALSE);

		if (   m_bNoButton
			&& m_hWndNoFrameMode == 0		//+++
		){
			return;
		}

		if (!m_bVisible) {
			CSize size(lpwndpos->cx, lpwndpos->cy);
			_UpdateLayout(size);
			return;
		}

		// erase controls on previous position
		_InvalidateControls();

		CSize	  size(lpwndpos->cx, lpwndpos->cy);
		_UpdateLayout(size);

		CClientDC dc(m_hWnd);
		_DoPaint(dc.m_hDC);
	}


	// Implementation
	BOOL _OnMouseMsg(UINT msg, UINT nFlags, CPoint pt)
	{
		if ((!m_bVisible || m_bNoButton)
			&& m_hWndNoFrameMode == 0		//+++
		){
			return FALSE;
		}

		if (msg == WM_LBUTTONDOWN) {
			//LTRACE(_T("	 WM_LBUTTONDOWN\n"));
			m_nTracking = _HitTest(pt);
		  #if 0 //+++ é¿å±
			if (m_nTracking == 3) {
				::PostMessage(m_hWndNoFrameMode, WM_COMMAND, ID_WINDOW_MOVE, 0);
				m_nTracking = -1;
			}
		  #endif
			if (m_nTracking >= 0) {
				CClientDC dc(m_hWnd);
				_DrawFrameControl(dc.m_hDC, m_nTracking, TRUE);
				m_bPushed = TRUE;
				SetCapture();					// grab mouse input
				return TRUE;
			}

		} else if ( (msg == WM_MOUSEMOVE) && m_nTracking >= 0 ) {
			// mouse moved, and I am tracking: possibly draw button up/down
			BOOL bOldDown = m_bPushed;
			m_bPushed = m_arrCaption[m_nTracking].PtInRect(pt) != 0;

			if (bOldDown != m_bPushed) {
				// up/down state changed: need to redraw button
				CClientDC dc(m_hWnd);
				_DrawFrameControl(dc.m_hDC, m_nTracking, m_bPushed);
			}

			return TRUE;						// handled

		} else if (msg == WM_LBUTTONUP && m_nTracking >= 0) {
			// user released the mouse and I am tracking: do button command
			::ReleaseCapture(); 				// let go the mouse

			if (m_bPushed) {
				// if button was down when released: draw button up, and do system cmd
				CClientDC dc(m_hWnd);
				_DrawFrameControl(dc.m_hDC, m_nTracking, FALSE);
				SendCommand(m_nTracking);
			}

			m_nTracking = -1;					// stop tracking
			return TRUE;						// handled (eat)
		}

		return FALSE;
	}


	void SendCommand(int nTracking)
	{
	  #if 1 //+++ ògñ≥ÇµÇÃÇ∆Ç´.
		if (m_hWndNoFrameMode) {
			if (m_nTracking == 0) { 		// Å~
				::PostMessage(m_hWndNoFrameMode, WM_CLOSE, 0, 0);
			} else if (m_nTracking == 1) {	// Å†
				::PostMessage(m_hWndNoFrameMode, WM_COMMAND, ID_MAINFRAME_NORM_MAX_SIZE, 0);
			} else if (m_nTracking == 2) {	// ÅQ
				::PostMessage(m_hWndNoFrameMode, WM_COMMAND, ID_MAINFRAME_MINIMIZE, 0);
			}
		} else
	  #endif
		if (m_wndMDIClient.m_hWnd) {			// MDI mode
			CWindow wnd = m_wndMDIClient.MDIGetActive();

			if (wnd.m_hWnd) {
				static const int syscmd[3] = { /*SC_MOUSEMENU,*/ SC_CLOSE, SC_RESTORE, SC_MINIMIZE };
				// SendMessage occur access violation!
				wnd.PostMessage(WM_SYSCOMMAND, syscmd[m_nTracking]);
			}
		} else {
			if (m_wndCmdSent.m_hWnd) {
				static const int syscmd[3] = { /*SC_MOUSEMENU,*/ SC_CLOSE, SC_RESTORE, SC_MINIMIZE };

				// SendMessage occur access violation!
				if (m_bSpecialRestore && nTracking == 1)
					m_wndCmdSent.PostMessage(WM_COMMAND, m_nCmdIDRestore);
				else
					m_wndCmdSent.PostMessage(WM_SYSCOMMAND, syscmd[nTracking]);
			}
		}
	}


	void _DoPaint(CDCHandle hDC)
	{
		if (m_hWndNoFrameMode == 0) {		//+++
			if (!m_bVisible || m_bNoButton)
				return;
		}

		for (int i = 0; i < 3; ++i) {
			_DrawFrameControl(hDC, i, FALSE);
		}
	}


	void _UpdateLayout(const CSize &sizeWnd)
	{
		if (m_hWndNoFrameMode == 0) {		//+++
			// ATLTRACE2(atlTraceGeneral, 4, _T("CMDIMenuControl::_UpdateLayout : size(%d, %d)\n"), size.cx, size.cy);
			if (m_bNoButton)
				return;
		}
		CSize size		  = sizeWnd;
		CSize sizeCaption = _GetCaptionSize();

		int   cxCaption   = sizeCaption.cx;
		int   cyCaption   = sizeCaption.cy;

		if (cyCaption > size.cy) {				//over
			cyCaption = size.cy;
			size.cy  += s_kcxGap;				// fix
		}

		CRect rcCaption;
		rcCaption.right  = size.cx;
		rcCaption.bottom = size.cy - s_kcxGap;
		rcCaption.left	 = rcCaption.right - cxCaption;
		rcCaption.top	 = rcCaption.bottom - cyCaption;

		m_arrCaption[0]  = rcCaption;

		rcCaption		-= CPoint(cxCaption + s_kcxGap, 0);
		m_arrCaption[1]  = rcCaption;

		rcCaption		-= CPoint(cxCaption, 0);
		m_arrCaption[2]  = rcCaption;
	}


	void _InvalidateControls()
	{
		if (m_hWndNoFrameMode == 0) {		//+++
			if (m_bNoButton)
				return;

			if (m_bOnlyCloseButton) {		// invalidate only close button
				InvalidateRect(m_arrCaption[0]);
				return;
			}
		}

		for (int i = 0; i < 3; ++i) {
			InvalidateRect(m_arrCaption[i]);
		}
	}


	void _DrawFrameControl(CDCHandle hDC, int nIndex, BOOL bPushed)
	{
		if (m_hWndNoFrameMode == 0) {		//+++
			if (m_bNoButton)
				return;

			if (m_bOnlyCloseButton) {				// do only 0(close button)
				if (nIndex == 1 || nIndex == 2)
					return;
			}
		}

		// draw frame controls
		CRect & 		  rc	  = m_arrCaption[nIndex];
		static const UINT dfcs[3] = { DFCS_CAPTIONCLOSE, DFCS_CAPTIONRESTORE, DFCS_CAPTIONMIN };
		UINT			  uState  = dfcs[nIndex];

		if (bPushed)
			uState |= DFCS_PUSHED;

		hDC.DrawFrameControl(rc, DFC_CAPTION, uState);
	}


	int _HitTest(CPoint point)
	{
		if (m_hWndNoFrameMode == 0) {		//+++
			if (m_bNoButton)
				return -1;
			if (m_bOnlyCloseButton) {		// only check 0
				if ( m_arrCaption[0].PtInRect(point) )
					return	0;
				else
					return -1;
			}
		}

		for (int i = 0; i < 3; ++i) {
			if ( m_arrCaption[i].PtInRect(point) )
				return i;
		}

		return -1;
	}


	int _HitTest(const CRect &rect)
	{
		if (m_hWndNoFrameMode == 0) {		//+++
			if (m_bNoButton)
				return -1;
			if (m_bOnlyCloseButton) {		// only check 0
				CRect rcCross = m_arrCaption[0] & rect;
				if ( !rcCross.IsRectEmpty() )
					return 0;
				else
					return -1;
			}
		}

		for (int i = 0; i < 3; ++i) {
			CRect rcCross = m_arrCaption[i] & rect;

			if ( !rcCross.IsRectEmpty() )
				return i;
		}

		return -1;
	}


	CSize _GetCaptionSize()
	{
		// fixed by JOBBY
		int cxCaption = ::GetSystemMetrics(SM_CXMENUSIZE) - s_kcxGap;
		int cyCaption = ::GetSystemMetrics(SM_CYMENUSIZE) - s_kcxGap * 2;

		return CSize(cxCaption, cyCaption);
	}


	bool _IsChildWindowMaximized()
	{
		BOOL bMaximized = FALSE;

		m_wndMDIClient.MDIGetActive(&bMaximized);
		return (bMaximized == TRUE);
	}

};
