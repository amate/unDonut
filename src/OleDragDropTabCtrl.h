/**
 *	@file	OleDragDropTabCtrl.h
 *	@brief	MTL : タブでのドラッグ＆ドロップ処理.
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
// TabCtrl.h: Last updated: February 12, 2001
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include "TabCtrl.h"
#include "MtlDragDrop.h"


namespace MTL {


// for debug
#ifdef _DEBUG
	const bool _Mtl_DragDrop_TabCtrl2_traceOn = false;
	#define DDTCTRACE	if (_Mtl_DragDrop_TabCtrl2_traceOn) ATLTRACE
#else
	#define DDTCTRACE
#endif



template <class T>
class COleDragDropTabCtrl
	: public CTabCtrl2Impl<T>
	, public IDropTargetImpl<T>
	, public IDropSourceImpl<T>
{
public:
	DECLARE_WND_CLASS_EX(_T("Mtl_OleDragDrop_TabCtrl"), CS_DBLCLKS, COLOR_BTNFACE)	// don't forget CS_DBLCLKS

private:
	// Data members
	CRect				m_rcInvalidateOnDrawingInsertionEdge;
	bool				m_bDragFromItself;
	CSimpleArray<int>	m_arrCurDragItems;

	int					m_nTabSeparatorWidth;
	bool				m_bFirefoxLike;

public:
	// Ctor
	COleDragDropTabCtrl() 
		: m_bDragFromItself(false) 
		, m_nTabSeparatorWidth(10)
		, m_bFirefoxLike(false)
	{ }

	void	SetTabSeparatorWidth(int nWidth) { m_nTabSeparatorWidth = nWidth; }
	int		GetTabSeparatorWidth() const { return m_nTabSeparatorWidth; }
	void	SetFirefoxLike(bool b) { m_bFirefoxLike = b; }
	bool	GetFirefoxLike() const { return m_bFirefoxLike; }

	// Overrides
	bool	OnScroll(UINT nScrollCode, UINT nPos, bool bDoScroll = true);

	DROPEFFECT OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect);
	DROPEFFECT OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point);
	void	OnDragLeave() { _ClearInsertionEdge(); }

private:
	// Overridables
	bool OnNewTabCtrlItems(
			int 						nInsertIndex,
			CSimpleArray<CTabCtrlItem>& items,
			IDataObject*				pDataObject,
			DROPEFFECT& 				dropEffect)
	{
		return false;
	}

	bool OnDropTabCtrlItem(int nIndex, IDataObject *pDataObject, DROPEFFECT &dropEffect)
	{	// if return true, delete src items considering ctrl key
		return true;
	}

	void OnDeleteItemDrag(int nIndex) { }


	HRESULT OnGetTabCtrlDataObject(CSimpleArray<int> arrIndex, IDataObject **ppDataObject)
	{
		ATLASSERT(ppDataObject != NULL);
		*ppDataObject = NULL;
		return E_NOTIMPL;
	}


	// Mothods
	enum _hitTestFlag
	{
		htInsetLeft,	// タブバーの左
//		htInsetRight,	// タブバーの右
		htItem,			// タブ
		htSeparator,	// タブの境界
		htOutside,		// タブバーの領域外
		htWhole,		// タブがない?
		htOneself,		// 自分のタブ
		htOneselfLeft,	// 自分のタブの左
		htOneselfRight,	// 自分のタブの右
	};
	// Implementation
	int _HitTestSideInset(CPoint point, int nInset = 0, bool *pbLeft = NULL);

	// Dragしているタブとその左右にカーソルがあるか
	int _HitTestCurTabSide(CPoint point, _hitTestFlag &flag);

	// どのタブの間にカーソルがあるかどうか
	int _HitTestSeparator(CPoint point, std::vector<std::pair<int, CRect> >& vecRect/*, int nInset*/);

	// タブバーの左側上にカーソルがあるかどうか
	int _HitTestLeftSide(CPoint point, std::vector<std::pair<int, CRect> >& vecRect);

	int HitTestOnDragging(_hitTestFlag &flag, CPoint point);


	enum insertFlags { insertLeft, insertMiddle, insertRight };

	void _ClearInsertionEdge();
	bool _DrawInsertionEdge(_hitTestFlag flag, int nIndex);
	void _DrawInsertionEdgeAux(CPoint pt, insertFlags flag);
	void _DrawInsertionEdgeAux(const CRect &rc);

	bool _IsSameIndexDropped(int nDestIndex);

	void _DoDragDrop(CPoint pt, UINT nFlags, int nIndex, bool bLeftButton);

public:
	// Message map and handlers
	BEGIN_MSG_MAP(COleDragDropTabCtrl<T>)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		CHAIN_MSG_MAP(CTabCtrl2Impl<T>)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

};	// COleDragDropTabCtrl



// Implementation
template <class T>
int COleDragDropTabCtrl<T>::_HitTestSideInset(CPoint point, int nInset/* = 0*/, bool *pbLeft/* = NULL*/)
{
	int  cxGap			= (nInset == 0) ? (s_kcxGap * 2 + s_kcxSeparator) : nInset;
	int  cyGap			= GetItemHeight();

	bool bCheckLeftSide = (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE)
							|| ( m_nFirstIndexOnSingleLine == 0 && !(m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) );

	if (bCheckLeftSide && m_items.size() > 0) {
		int 		 nIndex = HitTest(point);

		if (nIndex == -1)
			return -1;

		ATLASSERT( _IsValidIndex(nIndex) );
		CRect		 rc;
		GetClientRect(&rc);
		const CRect &rcItem = m_items[nIndex].m_rcItem;

		if (rcItem.left == 0) { 													// left side
			if ( CRect(rcItem.left, rcItem.top, rcItem.left + cxGap, rcItem.top + cyGap).PtInRect(point) ) {
				if (pbLeft)
					*pbLeft = true;

				return nIndex;
			} else
				return -1;
		} else if (rcItem.right > rc.right - s_kcxGap - s_kcxSeparator) {			// right side
			if ( CRect(rcItem.right - cxGap, rcItem.top, rcItem.right, rcItem.top + cyGap).PtInRect(point) ) {
				if (pbLeft)
					*pbLeft = false;

				return nIndex;
			} else
				return -1;
		}
	}

	return -1;
}

// Dragしているタブとその左右にカーソルがあるか
template <class T>
int COleDragDropTabCtrl<T>::_HitTestCurTabSide(CPoint point, _hitTestFlag &flag)
{
	// Dragされているタブが一つなら
	if (m_arrCurDragItems.GetSize() == 1) {
		int nCurDragItemIndex = m_arrCurDragItems[0];

		/* Dragされているタブの上にカーソルがあるかどうか */
		if ( GetItem(nCurDragItemIndex).m_rcItem.PtInRect(point) ) {
			flag = htOneself;	// 自分のタブの上にポインタがあるので無効にする
			return 0;
		}
		int	nTop = 0;
		/* Dragされているタブの左のタブの上にカーソルがあるかどうか */
		if (_IsValidIndex(nCurDragItemIndex -1)) {
			const CRect &rcLeft = GetItem(nCurDragItemIndex -1).m_rcItem;
			nTop = rcLeft.top;
			if (rcLeft.PtInRect(point)) {
				// 左のタブ上にある
				flag = htOneselfLeft;
				return nCurDragItemIndex -1;
			}
		}

		/* Dragされているタブの右のタブの上にカーソルがあるかどうか */
		if (_IsValidIndex(nCurDragItemIndex +1)) {
			const CRect &rcRight = GetItem(nCurDragItemIndex +1).m_rcItem;
			if (rcRight.top != nTop) {
				return -1;	// タブが同じ列じゃないので
			}
			if (rcRight.PtInRect(point)) {
				// 右のタブ上にある
				flag = htOneselfRight;
				return nCurDragItemIndex +1;
			}
		}
	}
	return -1;
}

// どのタブの間にカーソルがあるかどうか
template <class T>
int COleDragDropTabCtrl<T>::_HitTestSeparator(CPoint point, std::vector<std::pair<int, CRect> >& vecRect/*, int nInset*/)
{
	DDTCTRACE( _T("CTablCtrl2Impl::_HitTestSeparator\n") );

	int   cyGap = GetItemHeight();

	int   i 	= (GetTabCtrl2ExtendedStyle() & TAB2_EX_MULTILINE) ? 0 : m_nFirstIndexOnSingleLine;

	int	cyOffset = -1;
	for (; i < GetItemCount(); ++i) {
		const CRect &rcItem = GetItemRect(i);

		/* 左端のタブのインデックスと大きさを記憶する */
		if (cyOffset != rcItem.top) {
			vecRect.push_back(std::make_pair(i, rcItem));
			cyOffset = rcItem.top;
		}

		CRect		 rcSep	= CRect(
								rcItem.right,
								rcItem.top,
								rcItem.right,
								rcItem.top + cyGap);

		//rcSep.InflateRect(nInset, 0);
		rcSep.InflateRect(m_nTabSeparatorWidth, 0);

		if ( rcSep.PtInRect(point) ) {
			DDTCTRACE( _T(" between items hit!\n") );
			return i;
		}
	}

	DDTCTRACE( _T(" not inside of the gaps \n") );
	return -1;
}

// タブバーの左側上にカーソルがあるかどうか
template <class T>
int COleDragDropTabCtrl<T>::_HitTestLeftSide(CPoint point, std::vector<std::pair<int, CRect> >& vecRect)
{
	for (int i = 0; i < (int)vecRect.size(); ++i) {
		vecRect[i].second.left = 0;
		vecRect[i].second.right = m_nTabSeparatorWidth;
		if (vecRect[i].second.PtInRect(point)) {
			return vecRect[i].first;
		}
	}
	return -1;
}


template <class T>
int COleDragDropTabCtrl<T>::HitTestOnDragging(_hitTestFlag &flag, CPoint point)
{
	int nIndex;

	if (m_bFirefoxLike) {
		// Dragされているタブ自身とその左右にカーソルがあるかどうか
		if (( nIndex = _HitTestCurTabSide(point, flag) ) != -1) {
			if (flag == htOneself) {
				return -1;	// Dragされているのタブ上にカーソルがある
			} else {
				return nIndex;
			}
		}
	}

	std::vector<std::pair<int, CRect> > vecRect;

	// タブの境界上にカーソルがあるかどうか
	if (( nIndex = _HitTestSeparator(point, vecRect/*, s_nScrollInset*/) ) != -1) {
		// タブの境界にHitした
		flag = htSeparator;
		return nIndex;
	}

	// 左側にカーソルがあるかどうか
	if (( nIndex = _HitTestLeftSide(point, vecRect) ) != -1) {
		flag = htInsetLeft;
		return nIndex;
	}

	// タブの上にあるかどうか
	if (( nIndex = HitTest(point) ) != -1) {
		flag = htItem;
		return nIndex;
	}

	// out side of items
	flag = htOutside;
	return -1;
#if 0
	// inside of insets
	bool bLeft	= false;
	int  nIndex = _HitTestSideInset(point, s_nScrollInset, &bLeft);

	if (nIndex != -1) {
		flag = bLeft ? htInsetLeft : htInsetRight;
		return nIndex;
	}

	if ( ( nIndex = _HitTestCurTabSide(point, flag) ) != -1 ) {
		// Dragされているタブの左右にHitした
		return nIndex;
	}


	// inside of separator
	if ( ( nIndex = _HitTestSeparator(point, s_nScrollInset) ) != -1 ) {
		flag = htSeparator;
		return nIndex;
	}

	// inside of the item rect
	if ( ( nIndex = HitTest(point) ) != -1 ) {		// hit test for item rect
		flag = htItem;
		return nIndex;
	}

	// out side of items
	flag = htOutside;
	return -1;
#endif
}


template <class T>
void COleDragDropTabCtrl<T>::_ClearInsertionEdge()
{
	if ( !m_rcInvalidateOnDrawingInsertionEdge.IsRectEmpty() ) {
		InvalidateRect(m_rcInvalidateOnDrawingInsertionEdge);
		UpdateWindow();
		m_rcInvalidateOnDrawingInsertionEdge.SetRectEmpty();
	}
}


template <class T>
bool COleDragDropTabCtrl<T>::_DrawInsertionEdge(_hitTestFlag flag, int nIndex)
{
	CRect rcInvalidateOnDrawingInsertionEdge;

	if (flag == htOutside) {
		if (GetItemCount() > 0) {
			flag   = htSeparator;
			nIndex = GetItemCount() - 1;
		} else {
			flag = htWhole;
		}
	}

	CRect rcItem;

	switch (flag) {
		case htInsetLeft:
			{
				ATLASSERT( _IsValidIndex(nIndex) );
				rcItem = m_items[nIndex].m_rcItem;
				_DrawInsertionEdgeAux(rcItem.TopLeft(), insertLeft);
				rcInvalidateOnDrawingInsertionEdge.SetRect(
					rcItem.left, 
					rcItem.top, 
					rcItem.left + s_kcxSeparator * 2,
					rcItem.bottom);
			}
			break;
				
		case htOneselfLeft:
			{
				ATLASSERT( _IsValidIndex(nIndex) );
				rcItem = m_items[nIndex].m_rcItem;
				CPoint ptSep(rcItem.left/* + s_kcxGap*/, rcItem.top);
				_DrawInsertionEdgeAux(ptSep, insertMiddle);
				rcInvalidateOnDrawingInsertionEdge.SetRect(
					rcItem.left - (s_kcxSeparator + 1),
					rcItem.top,
					rcItem.left + s_kcxGap * 2 - 1,
					rcItem.bottom);
			}
			break;

		case htOneselfRight:
			{
				ATLASSERT( _IsValidIndex(nIndex) );
				rcItem = m_items[nIndex].m_rcItem;
				CPoint ptSep(rcItem.right/* + s_kcxGap*/, rcItem.top);
				_DrawInsertionEdgeAux(ptSep, insertMiddle);
				rcInvalidateOnDrawingInsertionEdge.SetRect(
					rcItem.right - (s_kcxSeparator + 1),
					rcItem.top,
					rcItem.right + s_kcxGap * 2 - 1,
					rcItem.bottom);
			}
			break;

		case htSeparator:
			{
				ATLASSERT( _IsValidIndex(nIndex) );
				rcItem = m_items[nIndex].m_rcItem;
				CPoint ptSep(rcItem.right/* + s_kcxGap*/, rcItem.top);
				_DrawInsertionEdgeAux(ptSep, insertMiddle);
				rcInvalidateOnDrawingInsertionEdge.SetRect(
					rcItem.right - (s_kcxSeparator + 1),
					rcItem.top,
					rcItem.right + s_kcxGap * 2 - 1,
					rcItem.bottom);
			}
			break;

		case htItem:
			{
				ATLASSERT( _IsValidIndex(nIndex) );
				rcItem	= m_items[nIndex].m_rcItem;
				_DrawInsertionEdgeAux(rcItem);
				rcInvalidateOnDrawingInsertionEdge = rcItem;
			}
			break;

		case htWhole:
			{
				GetClientRect(rcItem);
				_DrawInsertionEdgeAux(rcItem.TopLeft(), insertLeft);
				rcInvalidateOnDrawingInsertionEdge.SetRect(rcItem.left, rcItem.top, rcItem.left + s_kcxSeparator * 2, rcItem.bottom);
			}
			break;

		default:
			_ClearInsertionEdge();
			return false;
	}

	if (rcInvalidateOnDrawingInsertionEdge != m_rcInvalidateOnDrawingInsertionEdge) {
		_ClearInsertionEdge();
		m_rcInvalidateOnDrawingInsertionEdge = rcInvalidateOnDrawingInsertionEdge;
	}

	return true;
#if 0
	CRect rcInvalidateOnDrawingInsertionEdge;

	if (flag == htOutside) {
		if (m_items.size() > 0) {
			flag   = htSeparator;
			nIndex = GetItemCount() - 1;
		} else {
			flag = htWhole;
		}
	}

	CRect rcItem;

	if (flag == htInsetLeft) {
		ATLASSERT( _IsValidIndex(nIndex) );
		rcItem = m_items[nIndex].m_rcItem;
		rcInvalidateOnDrawingInsertionEdge.SetRect(rcItem.left, rcItem.top, rcItem.left + s_kcxSeparator * 2, rcItem.bottom);
		_DrawInsertionEdgeAux(rcItem.TopLeft(), insertLeft);

	} else if (flag == htInsetRight) {
		ATLASSERT( _IsValidIndex(nIndex) );
		rcItem = m_items[nIndex].m_rcItem;
		rcInvalidateOnDrawingInsertionEdge.SetRect(rcItem.right - s_kcxSeparator * 2,
													rcItem.top,
													rcItem.right,
													rcItem.bottom);
		_DrawInsertionEdgeAux(CPoint(rcItem.right - s_kcxSeparator, rcItem.top), insertRight);
	} else if (flag == htSeparator) {
		ATLASSERT( _IsValidIndex(nIndex) );
		rcItem = m_items[nIndex].m_rcItem;
		CPoint ptSep(rcItem.right/* + s_kcxGap*/, rcItem.top);
		_DrawInsertionEdgeAux(ptSep, insertMiddle);
		rcInvalidateOnDrawingInsertionEdge.SetRect(
			rcItem.right - (s_kcxSeparator + 1),
			rcItem.top,
			rcItem.right + s_kcxSeparator * 2 - 1,
			rcItem.bottom);
	} else if (flag == htItem) {
		ATLASSERT( _IsValidIndex(nIndex) );
		rcItem							   = m_items[nIndex].m_rcItem;
		_DrawInsertionEdgeAux(rcItem);
		rcInvalidateOnDrawingInsertionEdge = rcItem;
	} else if (flag == htWhole) {
		GetClientRect(rcItem);
		rcInvalidateOnDrawingInsertionEdge.SetRect(rcItem.left, rcItem.top, rcItem.left + s_kcxSeparator * 2, rcItem.bottom);
		_DrawInsertionEdgeAux(rcItem.TopLeft(), insertLeft);
	} else {
		_ClearInsertionEdge();
		return false;
	}

	if (rcInvalidateOnDrawingInsertionEdge != m_rcInvalidateOnDrawingInsertionEdge) {
		_ClearInsertionEdge();
		m_rcInvalidateOnDrawingInsertionEdge = rcInvalidateOnDrawingInsertionEdge;
	}

	return true;
#endif
}

template <class T>
void COleDragDropTabCtrl<T>::_DrawInsertionEdgeAux(CPoint pt, insertFlags flag)
{
	int 		 cy 	= GetItemHeight();
	int 		 sep	= s_kcxSeparator;
	CClientDC	 dc(m_hWnd);
	CBrush		 hbr;

	hbr.CreateSolidBrush( ::GetSysColor(COLOR_3DDKSHADOW) );
	dc.SetBrushOrg(pt.x, pt.y);
	CBrushHandle hbrOld = dc.SelectBrush(hbr);

	if (flag == insertLeft) {
		POINT pts[] = { { pt.x			, pt.y				  }, { pt.x 		 , pt.y + cy - 1 }, { pt.x + sep * 2 - 1, pt.y + cy - 1 },
						{ pt.x + sep - 1, pt.y + cy - sep - 1 }, { pt.x + sep - 1, pt.y + sep	 }, { pt.x + sep * 2 - 1, pt.y			}, };
		dc.Polygon( pts, _countof(pts) );

	} else if (flag == insertMiddle) {
		pt.x -= sep + 1;	// ↓左上の座標
		POINT pts[] = { { pt.x			, pt.y				  }, { pt.x + sep		 , pt.y + sep	 }, { pt.x + sep		, pt.y + cy - sep - 1 },
						{ pt.x			, pt.y + cy - 1 	  }, { pt.x + sep * 3 - 1, pt.y + cy - 1 }, { pt.x + sep * 2 - 1, pt.y + cy - sep - 1 },
						{ pt.x + sep * 2 - 1,  pt.y + sep	  }, { pt.x + sep * 3 - 1, pt.y 		 }	};
		dc.Polygon( pts, _countof(pts) );

	} else if (flag == insertRight) {
		POINT pts[] = { { pt.x - sep	,pt.y			}, { pt.x			, pt.y + sep	}, { pt.x			, pt.y + cy - sep - 1 },
						{ pt.x - sep	, pt.y + cy - 1 }, { pt.x + sep - 1 , pt.y + cy - 1 }, { pt.x + sep - 1 , pt.y				  }, };
		dc.Polygon( pts, _countof(pts) );
	}

	dc.SelectBrush(hbrOld);
}


template <class T>
void COleDragDropTabCtrl<T>::_DrawInsertionEdgeAux(const CRect &rc)
{
	CClientDC	 dc(m_hWnd);
	CBrush		 hbr;

	hbr.CreateSolidBrush( ::GetSysColor(COLOR_3DDKSHADOW) );
	dc.SetBrushOrg(rc.left, rc.top);
	CBrushHandle hbrOld = dc.SelectBrush(hbr);

	POINT pts[]	= { 
		{ rc.left, rc.top }, 
		{ rc.left, rc.bottom - 1 },
		{ rc.right - 1, rc.bottom - 1 }, 
		{ rc.right - 1, rc.top },
		{ rc.left, rc.top }
	};

	dc.Polyline( pts, _countof(pts) );

	dc.SelectBrush(hbrOld);
}


template <class T>
bool COleDragDropTabCtrl<T>::_IsSameIndexDropped(int nDestIndex)
{
	if (m_bDragFromItself == false)
		return false;

	for (int i = 0; i < m_arrCurDragItems.GetSize(); ++i) {
		int nSrcIndex = m_arrCurDragItems[i];

		if (nSrcIndex == nDestIndex)
			return true;
	}

	return false;
}


template <class T>
void COleDragDropTabCtrl<T>::_DoDragDrop(CPoint pt, UINT nFlags, int nIndex, bool bLeftButton)
{
	if ( PreDoDragDrop(m_hWnd, NULL, false) ) { 	// now dragging
		_HotItem(); 								// clean up hot item

		CComPtr<IDataObject> spDataObject;
		T * 	pT = static_cast<T *>(this);

		// set up current drag item index list
		GetCurMultiSel(m_arrCurDragItems);
		if (m_arrCurDragItems.Find(nIndex) == -1) { // not multi-select draging
			m_arrCurDragItems.RemoveAll();
			m_arrCurDragItems.Add(nIndex);
		}

		HRESULT 	hr = pT->OnGetTabCtrlDataObject(m_arrCurDragItems, &spDataObject);
		if ( SUCCEEDED(hr) ) {
			m_bDragFromItself = true;
			DROPEFFECT dropEffect = DoDragDrop(spDataObject, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);
			m_bDragFromItself = false;
		}

		m_arrCurDragItems.RemoveAll();

	} else {
		if (bLeftButton) {
			SetCurSel(nIndex, true);
			NMHDR nmhdr = { m_hWnd, GetDlgCtrlID(), TCN_SELCHANGE };
			::SendMessage(GetParent(), WM_NOTIFY, (WPARAM) GetDlgCtrlID(), (LPARAM) &nmhdr);
		} else {
			SendMessage( WM_RBUTTONUP, (WPARAM) nFlags, MAKELPARAM(pt.x, pt.y) );
		}
	}
}


template <class T>
LRESULT COleDragDropTabCtrl<T>::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	DDTCTRACE("COleDragDropTabCtrl::OnLButtonDown\n");
	POINT pt	 = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if (wParam & MK_CONTROL) {
		bHandled = FALSE;
		return 0;
	}

	int   nIndex = HitTest(pt);

	if (nIndex != -1) {
		//デフォルトではDown時に切り替え release10β4
		//以前はUp時に切り替えでDownはD&Dの前段階という仕様
		if (GetTabCtrl2ExtendedStyle() & TAB2_EX_MOUSEDOWNSELECT) {
			SetCurSel(nIndex, true);
			NMHDR nmhdr = { m_hWnd, GetDlgCtrlID(), TCN_SELCHANGE };
			::SendMessage(GetParent(), WM_NOTIFY, (WPARAM) GetDlgCtrlID(), (LPARAM) &nmhdr);
		} else {
			_DoDragDrop(pt, (UINT) wParam, nIndex, true);
		}
	}

	return 0;
}


template <class T>
LRESULT COleDragDropTabCtrl<T>::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	DDTCTRACE("COleDragDropTabCtrl::OnRButtonDown\n");
	POINT pt	 = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if (wParam & MK_CONTROL) {
		bHandled = FALSE;
		return 0;
	}

	int   nIndex = HitTest(pt);

	if (nIndex != -1) {
		_DoDragDrop(pt, (UINT) wParam, nIndex, false);
	}

	return 0;
}


template <class T>
LRESULT COleDragDropTabCtrl<T>::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;

	ATLASSERT( ::IsWindow(m_hWnd) );
	bool bReg = RegisterDragDrop();
	ATLASSERT(bReg == true);
	return 0;
}


template <class T>
LRESULT COleDragDropTabCtrl<T>::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &bHandled)
{
	bHandled = FALSE;
	RevokeDragDrop();
	return 0;
}

// Overrides
template <class T>
bool COleDragDropTabCtrl<T>::OnScroll(UINT nScrollCode, UINT nPos, bool bDoScroll/* = true*/)
{
	DDTCTRACE("COleDragDropTabCtrl::OnScroll\n");

	bool bResult = false;

	if ( m_wndUpDown.IsWindowVisible() == false )
		return bResult;

	if (LOBYTE(nScrollCode) == SB_LINEUP) {
		if (bDoScroll)
			bResult = ScrollItem(false);
		else
			bResult = CanScrollItem(false);
	} else if (LOBYTE(nScrollCode) == SB_LINEDOWN) {
		if (bDoScroll)
			bResult = ScrollItem(true);
		else
			bResult = CanScrollItem(true);
	}

	return bResult;
}


template <class T>
DROPEFFECT COleDragDropTabCtrl<T>::OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
{
	_hitTestFlag flag;
	int 		 nIndex = HitTestOnDragging(flag, point);

	_DrawInsertionEdge(flag, nIndex);

	if ( flag == htItem && _IsSameIndexDropped(nIndex) )
		return DROPEFFECT_NONE;
#if 0
	if (flag == htItem && m_bDragFromItself && m_arrCurDragItems.GetSize() > 1)
		return DROPEFFECT_NONE;
#endif
	if (!m_bDragFromItself)
		return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect) | DROPEFFECT_COPY;

	return _MtlStandardDropEffect(dwKeyState);
}


template <class T>
DROPEFFECT COleDragDropTabCtrl<T>::OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
{
	_ClearInsertionEdge();

	T * 					   pT	  = static_cast<T *>(this);
	CSimpleArray<CTabCtrlItem> arrItems;

	_hitTestFlag			   flag;
	int 					   nIndex = HitTestOnDragging(flag, point);

	switch (flag) {
		case htInsetLeft:
			if ( m_bDragFromItself && (dropEffect & DROPEFFECT_MOVE) ) {							// just move items
				MoveItems(nIndex, m_arrCurDragItems);
			} else {
				pT->OnNewTabCtrlItems(nIndex, arrItems, pDataObject, dropEffect);

				for (int i = 0; i < arrItems.GetSize(); ++i)
					InsertItem(nIndex, arrItems[i]);
			}
			break;

		case htOneselfLeft:
			// 自分のタブの左にドロップされた
			if ( m_bDragFromItself && (dropEffect & DROPEFFECT_MOVE) ) {
				// just move items
				MoveItems(nIndex, m_arrCurDragItems);
			} else {
				pT->OnNewTabCtrlItems(nIndex, arrItems, pDataObject, dropEffect);

				for (int i = 0; i < arrItems.GetSize(); ++i)
					InsertItem(nIndex + 1, arrItems[i]);
			}
			break;

		case htOneselfRight:
			// 自分のタブの右にドロップされた
			if ( m_bDragFromItself && (dropEffect & DROPEFFECT_MOVE) ) {
				// just move items
				MoveItems(nIndex +1, m_arrCurDragItems);
			} else {
				pT->OnNewTabCtrlItems(nIndex + 1, arrItems, pDataObject, dropEffect);

				for (int i = 0; i < arrItems.GetSize(); ++i)
					InsertItem(nIndex + 1, arrItems[i]);
			}
			break;

		case htSeparator:
			if ( m_bDragFromItself && (dropEffect & DROPEFFECT_MOVE) ) {							// just move items
				MoveItems(nIndex + 1, m_arrCurDragItems);
			} else {
				pT->OnNewTabCtrlItems(nIndex + 1, arrItems, pDataObject, dropEffect);

				for (int i = 0; i < arrItems.GetSize(); ++i)
					InsertItem(nIndex + 1, arrItems[i]);
			}
			break;

		case htItem:
			if ( _IsSameIndexDropped(nIndex) == false ) 
			{
				DROPEFFECT dropEffectPrev = dropEffect;
				bool	   bDelSrc		  = pT->OnDropTabCtrlItem(nIndex, pDataObject, dropEffect);

				if ( bDelSrc && m_bDragFromItself && (dropEffectPrev & DROPEFFECT_MOVE) ) { // just move items
					// do nothing
					for (int i = 0; i < m_arrCurDragItems.GetSize(); ++i) {
						pT->OnDeleteItemDrag(m_arrCurDragItems[i]);
					}

					DeleteItems(m_arrCurDragItems);
				}
			}
			break;

		case htOutside:
			if ( m_bDragFromItself && (dropEffect & DROPEFFECT_MOVE) ) {							// just move items to tail
				MoveItems(GetItemCount(), m_arrCurDragItems);
			} else {
				pT->OnNewTabCtrlItems(GetItemCount(), arrItems, pDataObject, dropEffect);

				for (int i = 0; i < arrItems.GetSize(); ++i)
					AddItem(arrItems[i]);
			}
			break;

		default:
			break;
	}

	return dropEffect;
}



////////////////////////////////////////////////////////////////////////////



}	//namespace MTL



#ifndef _MTL_NO_AUTOMATIC_NAMESPACE
using namespace MTL;
#endif	//!_MTL_NO_AUTOMATIC_NAMESPACE
