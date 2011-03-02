/**
 *	@file	FlatComboBox.h
 *	@brief	フラット・コンボ・ボックス
 */
#pragma once


// The Web Browser style flat combo box control (light-flat combo box)
//
// Note. I can't find the way to avoid the thumb flicker while resizing.
//
#include "MtlMisc.h"
#include "MtlWin.h"
#include "option/SkinOption.h"
#include "DonutPFunc.h"


typedef CWinTraits<WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 0>
CComboBoxTraits;

///////////////////////////////////////////////////////////
// CFlatComboBox

class CFlatComboBox	
	: public CWindowImpl<CFlatComboBox, CComboBox, CComboBoxTraits>
	, public CTrackMouseLeave<CFlatComboBox>
{
public:
	DECLARE_WND_SUPERCLASS(_T("FlatComboBox"), _T("COMBOBOX"))

private:
	// Constants
	enum			 { s_kcxyBorder = 2 };
	enum _ComboState { _comboNormal, _comboHot, _comboPressed };

	// Data members
	_ComboState 	m_state;
	int 			m_nDrawStyle;
	BOOL			m_bExistManifest;

public:
	// Ctor
	CFlatComboBox()
		: m_state(_comboNormal)
		, m_nDrawStyle(SKN_COMBO_STYLE_DEFAULT)
	{
		m_bExistManifest = IsExistManifestFile();
	}


	bool FlatComboBox_Install(HWND hWndCombo)
	{
		MTLVERIFY( SubclassWindow(hWndCombo) );
		return true;
	}


	void FlatComboBox_Uninstall()
	{
		if ( IsWindow() )
			UnsubclassWindow();
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CFlatComboBox)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		CHAIN_MSG_MAP(CTrackMouseLeave<CFlatComboBox>)
	END_MSG_MAP()


private:

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if (   m_nDrawStyle == SKN_COMBO_STYLE_DEFAULT
		   || (m_nDrawStyle == SKN_COMBO_STYLE_THEME && !m_bExistManifest) )
		{
			CRect	  rc;
			GetUpdateRect(&rc, FALSE);

			ValidateRect(NULL); // validate the whole

			// invaldate only the internal rect
			CRect	  rcCombo  = _GetFlatComboInvalidateRect();
			CRect	  rcThumb  = _GetFlatThumbInvalidateRect();

			CRect	  rcUpdate = rc & rcCombo;
			InvalidateRect(&rcUpdate, FALSE);
			rcUpdate = rc & rcThumb;
			InvalidateRect(&rcUpdate, FALSE);

			// update!
			LRESULT   lRet	   = DefWindowProc();
			CClientDC dc(m_hWnd);
			_DoComboPaint(dc.m_hDC);
			return lRet;

		} else {
			bHandled = FALSE;
			return 0;
		}
	}


	void _DoComboPaint(CDCHandle dc)
	{
		_DrawFlatCombo(dc, m_state);
		_DrawFlatThumb(dc);
	}


	void _DrawFlatThumb(CDCHandle &dc)
	{
		CRect	 rc 		= _GetOriginalThumbRect();

		dc.DrawEdge(rc, BDR_RAISEDINNER, BF_RECT);

		rc.DeflateRect(1, 1);
		COLORREF clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
		dc.Draw3dRect(rc, clrBtnFace, clrBtnFace);
	}


	CRect _GetOriginalThumbRect()
	{
		CRect rc;

		GetClientRect(&rc);
		rc.right  -= s_kcxyBorder;
		rc.left    = rc.right - ::GetSystemMetrics(SM_CXHTHUMB);
		rc.top	  += s_kcxyBorder;
		rc.bottom -= s_kcxyBorder;
		return rc;
	}


	CRect _GetFlatThumbInvalidateRect()
	{
		CRect rc = _GetOriginalThumbRect();

		rc.DeflateRect(2, 2);
		return rc;
	}


	CRect _GetFlatComboInvalidateRect()
	{
		CRect rc;

		GetClientRect(&rc);
		rc.DeflateRect(2, 2);
		rc.right -= ::GetSystemMetrics(SM_CXHTHUMB);
		return rc;
	}


	void _DrawFlatCombo(CDCHandle &dc, _ComboState state)
	{
		COLORREF clrBtnFace = ::GetSysColor(COLOR_BTNFACE);

		CRect	 rc;
		GetClientRect(&rc);

		dc.Draw3dRect(rc, clrBtnFace, clrBtnFace);
		rc.DeflateRect(1, 1);

		switch (state) {
		case _comboNormal:
			dc.DrawEdge(rc, BDR_SUNKENOUTER, BF_RECT);
			break;

		case _comboHot:
			dc.DrawEdge(rc, BDR_RAISEDINNER, BF_RECT);
			break;

		case _comboPressed:
			break;
		}
	}

	void _SetUseXPTheme(HWND hWnd, BOOL bUse)
	{
		HRESULT   (WINAPI * __SetWindowTheme)(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList) = NULL;

		HINSTANCE hInst = ::LoadLibrary( _T("UxTheme.dll") );

		if (hInst != NULL) {
			__SetWindowTheme = ( HRESULT (WINAPI *)(HWND, LPCWSTR, LPCWSTR) ) ::GetProcAddress( hInst, "SetWindowTheme" );

			if (__SetWindowTheme != NULL) {
				if (bUse)
					(__SetWindowTheme) ( hWnd, L" ", L"COMBOBOX");
				else
					(__SetWindowTheme) ( hWnd, (L" "), (L" ") );
			}

			::FreeLibrary( hInst );
		}
	}


public:
	void SetDrawStyle(int nStyle)
	{
		m_nDrawStyle = nStyle;
		m_bExistManifest = IsExistManifestFile();		//+++
		if (nStyle == SKN_COMBO_STYLE_THEME && m_bExistManifest) {
			_SetUseXPTheme(m_hWnd, TRUE);
		} else {
			_SetUseXPTheme(m_hWnd, FALSE);
		}
	}

	// manifestの存在チェック
	static bool IsExistManifestFile() { return (::GetFileAttributes(Misc::GetExeFileName() + _T(".manifest")) != 0xFFFFFFFF); }
};
