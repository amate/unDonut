/**
 *	@file	DropDownButton.h
 *	@brief	オーナードローボタンでスピンボタンの下矢印ボタンを代用するためのコントロール.
 */

#pragma once

#include "stdafx.h"
#include "MtlWin.h"
//#include "atltheme_d.h"
#include "option/SkinOption.h"



/*
 *	オーナードローボタンでスピンボタンの下矢印ボタンを代用するためのコントロール
 *　スキン機能にも対応
 */
template <class T, class TBase = CButton, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE 	CDropDownButtonCtrlImpl
	: public CWindowImpl< T, TBase, TWinTraits >
	, public CThemeImpl<CDropDownButtonCtrlImpl< T, TBase, TWinTraits > >
	, public CTrackMouseLeave<CDropDownButtonCtrlImpl< T, TBase, TWinTraits > >
{
public:
	DECLARE_WND_SUPERCLASS( NULL, TBase::GetWndClassName() )

private:
	typedef CWindowImpl< T, TBase, TWinTraits >  baseClass;
	UINT	m_nID;
	int 	m_nDrawStyle;
	BOOL	m_bExistManifest;

public:
	CDropDownButtonCtrlImpl() : m_nDrawStyle(SKN_TAB_STYLE_DEFAULT)
	{
		SetThemeClassList(L"SPIN");
		m_bExistManifest = IsExistManifestFile();
	}


	HWND Create(HWND hWndParent, _U_RECT rect = NULL, UINT nID = 0)
	{
		m_nID = nID;
		return baseClass::Create(hWndParent, *rect.m_lpRect, NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0, nID);
	}


public:
	BEGIN_MSG_MAP(CDropDownButtonCtrlImpl)
		CHAIN_MSG_MAP(CThemeImpl<CDropDownButtonCtrlImpl>)
		CHAIN_MSG_MAP(CTrackMouseLeave<CDropDownButtonCtrlImpl>)
		MESSAGE_HANDLER(OCM_DRAWITEM   , OnDrawItem   )
		MESSAGE_HANDLER(OCM_MEASUREITEM, OnMeasureItem)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()


private:
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		UINT	nID  = UINT(wParam);
		if (nID != m_nID) {
			bHandled = FALSE;
			return 0;
		}

		LPDRAWITEMSTRUCT pDis = (LPDRAWITEMSTRUCT) lParam;

		if (m_nDrawStyle != SKN_TAB_STYLE_CLASSIC && m_hTheme && m_bExistManifest) {
			try {
				if (pDis->itemState & ODS_SELECTED) {
					DrawThemeBackground(pDis->hDC, SPNP_DOWN, DNS_PRESSED, &pDis->rcItem);
				} else {
					if (m_bTrackMouseLeave) {
						DrawThemeBackground(pDis->hDC, SPNP_DOWN, DNS_HOT, &pDis->rcItem);
					} else {
						DrawThemeBackground(pDis->hDC, SPNP_DOWN, DNS_NORMAL, &pDis->rcItem);
					}
				}
			} catch (...) {
				ATLASSERT(FALSE);
			}
		} else {
			if (pDis->itemState & ODS_SELECTED) {
				CRect rc(pDis->rcItem);
				rc.bottom += 2; //三角をちょっと下にずらすための補正
				::DrawFrameControl(pDis->hDC, &rc, DFC_SCROLL, DFCS_SCROLLDOWN);
				::DrawEdge(pDis->hDC, &pDis->rcItem, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
			} else {
				::DrawFrameControl(pDis->hDC, &pDis->rcItem, DFC_SCROLL, DFCS_SCROLLDOWN | DFCS_ADJUSTRECT);
			}
		}

		return 0;
	}


	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		UINT	nID  = UINT(wParam);
		if (nID != m_nID) {
			bHandled = FALSE;
			return 0;
		}

		LPMEASUREITEMSTRUCT pMis = (LPMEASUREITEMSTRUCT) lParam;
		CRect		rc;
		GetClientRect(&rc);

		CString 	ss;
		ss.Format( _T("pMis->itemWidrh = %d , pMis->itemHeight = %d\nrc.Width() = %d , rc.Height() = %d"),
				  pMis->itemWidth , 	 pMis->itemHeight , 	rc.Width(), 	  rc.Height() );
		MessageBox(ss);
		return 0;
	}


public:
	void OnTrackMouseMove(UINT nFlags, CPoint pt)
	{
		InvalidateRect(NULL, TRUE);
		//DrawThemeBackground(pDis->hDC, SPNP_DOWN, DNS_HOT, &pDis->rcItem);
	}


	void OnTrackMouseLeave()
	{
		InvalidateRect(NULL, TRUE);
	}


	void SetDrawStyle(int nStyle)
	{
		m_nDrawStyle = nStyle;
	}
};



class CDropDownButtonCtrl : public CDropDownButtonCtrlImpl<CDropDownButtonCtrl> {
public:
};

