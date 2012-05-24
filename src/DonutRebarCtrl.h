/**
 *	@file	DonutRebarCtrl.h
 *	@brief	ReBar(固定していない複数のツールバーを再配置したりする)処理
 */
#pragma once

#include <atlctrls.h>
#include "option\SkinOption.h"

//////////////////////////////////////////////////////////////////
// CReBarCtrlImpl

template <class T, class TBase = CReBarCtrl, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CReBarCtrlImpl : public CWindowImpl< T, TBase, TWinTraits >
{
public:
	// Message map
	BEGIN_MSG_MAP(CReBarCtrlImpl)
		//MESSAGE_HANDLER(WM_PAINT, OnPaint)
		//MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackGround)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()


private:
	// Overrides
	void OnRefreshBackGround() { }


	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		T *pT = static_cast<T *>(this);
		pT->OnRefreshBackGround();
		return 0;
	}

};



///////////////////////////////////////////////////////////////////////
// CDonutReBarCtrl

class CDonutReBarCtrl : public CReBarCtrlImpl<CDonutReBarCtrl>
{
	CBitmap 	m_bmpBack;
	CBitmap 	m_bmpStretch;
	int 		m_nBGStyle;

public:
	CDonutReBarCtrl() : m_nBGStyle(0)
	{
	}


	// Overrides
	void OnRefreshBackGround()
	{
		if (m_nBGStyle == SKN_REBAR_STYLE_STRETCH) {
			CRect	rc;
			GetClientRect(&rc);

			CClientDC	hDesktopDC(NULL);	//HDC 	hDesktopDC	= ::GetDC(NULL);	//+++ WTLなクラスに後片付け任せる.
			CDC 	dcSrc, dcDest;
			dcDest.CreateCompatibleDC(hDesktopDC);
			dcSrc.CreateCompatibleDC(hDesktopDC);

			if (m_bmpStretch.m_hBitmap)
				m_bmpStretch.DeleteObject();

			m_bmpStretch.CreateCompatibleBitmap( hDesktopDC, rc.Width(), rc.Height() );

			HBITMAP hOldbmpSrc	= dcSrc.SelectBitmap(m_bmpBack.m_hBitmap);
			HBITMAP hOldbmpDest = dcDest.SelectBitmap(m_bmpStretch.m_hBitmap);

			SIZE	size;
			m_bmpBack.GetSize(size);
			::StretchBlt(dcDest, 0, 0, rc.Width(), rc.Height(), dcSrc , 0, 0, size.cx, size.cy, SRCCOPY);

			dcSrc.SelectBitmap(hOldbmpSrc);
			dcDest.SelectBitmap(hOldbmpDest);

			_SetBackGround(m_bmpStretch);

			//::ReleaseDC(NULL, hDesktopDC);		//+++ きっと、必要.	//+++ WTLなクラスに後片付け任せる.

			InvalidateRect(NULL, TRUE);
		}
	}


	void RefreshSkinState()
	{
		//背景描画方法の設定
		_SetDrawStyle(CSkinOption::s_nRebarBGStyle);
		CString strBmpPath = _GetSkinDir() + _T("rebar.bmp");

		//背景のビットマップ読み込み
		m_bmpBack.Attach( AtlLoadBitmapImage(strBmpPath.GetBuffer(0), LR_LOADFROMFILE) );

		if (m_nBGStyle == SKN_REBAR_STYLE_TILE)
			_SetBackGround(m_bmpBack);

		OnRefreshBackGround();

		//境界線の設定
		_SetBoader( !CSkinOption::s_nRebarNoBoader );

		//再描画
		InvalidateRect(NULL, TRUE);
	}


private:
	//Function
	void _SetDrawStyle(int nStyle)
	{
		m_nBGStyle = nStyle;

	  #if 1	//+++ uxtheme.dll の関数の呼び出し方を変更.
		if (nStyle == SKN_REBAR_STYLE_THEME)
			UxTheme_Wrap::SetWindowTheme(m_hWnd, NULL, L"REBAR");
		else
			UxTheme_Wrap::SetWindowTheme(m_hWnd, L" ", L" ");
	  #else
		CTheme	theme;
		if (nStyle == SKN_REBAR_STYLE_THEME)
			theme.SetWindowTheme(m_hWnd, NULL, L"REBAR");
		else
			theme.SetWindowTheme(m_hWnd, L" ", L" ");
	  #endif
	}


	void _SetBackGround(HBITMAP hBitmap)
	{
		REBARBANDINFO rbbi	 = { sizeof (REBARBANDINFO) };

		rbbi.fMask	 = RBBIM_BACKGROUND;
		rbbi.hbmBack = hBitmap;

		int 		  nCount = GetBandCount();

		for (int i = 0; i < nCount; i++)
			SetBandInfo(i, &rbbi);
	}


	void _SetBoader(BOOL bShow)
	{
		int nStyle = GetWindowLong(GWL_STYLE);

		if (bShow)
			nStyle |= RBS_BANDBORDERS;
		else
			nStyle &= ~RBS_BANDBORDERS;

		SetWindowLong(GWL_STYLE, nStyle);
	}

};



/////////////////////////////////////////////////////////////////////////////////////
// CTransparentToolBarCtrl

class CTransparentToolBarCtrl : public CWindowImpl<CTransparentToolBarCtrl, CToolBarCtrl> 
{
public:
	// Message map
	BEGIN_MSG_MAP(CTransparentToolBarCtrl)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackGround)
	END_MSG_MAP()

private:
	LRESULT OnEraseBackGround(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		HWND	hWnd	= GetParent();
		CPoint	pt;

		MapWindowPoints(hWnd, &pt, 1);
		::OffsetWindowOrgEx( (HDC) wParam, pt.x, pt.y, NULL );
		LRESULT lResult = ::SendMessage(hWnd, WM_ERASEBKGND, wParam, 0L);
		::SetWindowOrgEx( (HDC) wParam, 0, 0, NULL );
		return lResult;
	}
};



//////////////////////////////////////////////////////////////////////
// CDonutExplorerReBarCtrl

class CDonutExplorerReBarCtrl : public CDonutReBarCtrl 
{
public:
	CDonutExplorerReBarCtrl& operator =(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	// Message map
	BEGIN_MSG_MAP(CDonutExplorerReBarCtrl)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackGround)
	END_MSG_MAP()


private:
	LRESULT OnEraseBackGround(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		HWND	hWnd	= GetParent();
		CPoint	pt;

		MapWindowPoints(hWnd, &pt, 1);
		::OffsetWindowOrgEx( (HDC) wParam, pt.x, pt.y, NULL );
		LRESULT lResult = ::SendMessage(hWnd, WM_ERASEBKGND, wParam, 0L);
		::SetWindowOrgEx( (HDC) wParam, 0, 0, NULL );
		return lResult;
	}
};



