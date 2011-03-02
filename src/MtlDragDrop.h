/**
 *	@file	MtlDragDrop.h
 *	@brief	MTL : ドラッグ＆ドロップ
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
// MtlDragDrop.h: Last updated: March 19, 2001
/////////////////////////////////////////////////////////////////////////////

#ifndef __MTLDRAGDROP_H__
#define __MTLDRAGDROP_H__

#pragma once

#include "MtlFile.h"
#include "MtlWeb.h"



namespace MTL {

typedef DWORD  DROPEFFECT;


/////////////////////////////////////////////////////////////////////////////
// IDropSourceImpl
//

// for debug
#ifdef _DEBUG
	const bool _Mtl_DropSourceImpl_traceOn = false;
	#define DSTRACE 	if (_Mtl_DropSourceImpl_traceOn)  ATLTRACE
#else
	#define DSTRACE
#endif



class ATL_NO_VTABLE _IDropSource {
public:
	//this method needs a different name than QueryInterface
	STDMETHOD							(_LocDSQueryInterface) (REFIID riid, void **ppvObject) = 0;
	virtual ULONG STDMETHODCALLTYPE 	AddRef(void)  = 0;
	virtual ULONG STDMETHODCALLTYPE 	Release(void) = 0;
};



class ATL_NO_VTABLE _IDropSourceLocator : public _IDropSource {
public:
};



class IDropSourceImplBase {
	static bool 	s_bStaticInit;

public:
	// metrics for drag start determination
	static UINT 	s_nDragMinDist; 	// min. amount mouse must move for drag
	static UINT 	s_nDragDelay;		// delay before drag starts

public:
	IDropSourceImplBase()
	{
		// init static variables
		if (!s_bStaticInit) {
			::EnterCriticalSection(&_Module.m_csStaticDataInit);

			if (!s_bStaticInit) {
				// get drag metrics from win.ini
				static const TCHAR szWindows[]	   = _T("windows");
				static const TCHAR szDragMinDist[] = _T("DragMinDist");
				static const TCHAR szDragDelay[]   = _T("DragDelay");

				s_nDragMinDist = ::GetProfileInt(szWindows, szDragMinDist, DD_DEFDRAGMINDIST);
				s_nDragDelay   = ::GetProfileInt(szWindows, szDragDelay  , DD_DEFDRAGDELAY);

				s_bStaticInit  = true;
			}

			::LeaveCriticalSection(&_Module.m_csStaticDataInit);
		}
	}

};


__declspec(selectany) bool IDropSourceImplBase::s_bStaticInit  = false;
__declspec(selectany) UINT IDropSourceImplBase::s_nDragMinDist = DD_DEFDRAGMINDIST;
__declspec(selectany) UINT IDropSourceImplBase::s_nDragDelay   = DD_DEFDRAGDELAY;



template <class T>
class ATL_NO_VTABLE IDropSourceImpl
	: public _IDropSourceLocator
	, public IDropSourceImplBase
{
private:
	// COM Identity
	STDMETHOD	(_LocDSQueryInterface) (REFIID riid, void **ppvObject)
	{
		if ( InlineIsEqualUnknown(riid) || InlineIsEqualGUID(riid, IID_IDropSource) ) {
			if (ppvObject == NULL)
				return E_POINTER;

			*ppvObject = this;
			AddRef();
		  #ifdef _ATL_DEBUG_INTERFACES
			_Module.AddThunk( (IUnknown **) ppvObject, _T("IDropSourceImpl"), riid );
		  #endif	// _ATL_DEBUG_INTERFACES
			return S_OK;

		} else
			return E_NOINTERFACE;
	}


	virtual ULONG STDMETHODCALLTYPE AddRef()
	{
		return 1;
	}


	virtual ULONG STDMETHODCALLTYPE Release()
	{
		return 1;
	}


private:
	// Data members
	CRect	m_rectStartDrag;	// when mouse leaves this rect, drag drop starts
	bool	m_bDragStarted; 	// has drag really started yet?
	DWORD	m_dwButtonCancel;	// which button will cancel (going down)
	DWORD	m_dwButtonDrop; 	// which button will confirm (going up)

public:
	// Ctor/dtor
	IDropSourceImpl()
		: m_bDragStarted(false)
		, m_dwButtonCancel(0)
		, m_dwButtonDrop(0)
	{
	}


private:
	// Attributes
	static bool IsSamePoint(CPoint ptNew, CPoint ptPrev)
	{
		CRect rc(ptPrev.x, ptPrev.y, ptPrev.x, ptPrev.y);

		rc.InflateRect(s_nDragMinDist, s_nDragMinDist);

		return rc.PtInRect(ptNew) == TRUE /*? true : false*/;
	}


	// IDropSource
	STDMETHOD	(QueryContinueDrag) (BOOL bEscapePressed, DWORD dwKeyState)
	{
		// check escape key or right button -- and cancel
		if (bEscapePressed || (dwKeyState & m_dwButtonCancel) != 0) {
			m_bDragStarted = false; // avoid unecessary cursor setting
			return DRAGDROP_S_CANCEL;
		}

		// check left-button up to end drag/drop and do the drop
		if ( (dwKeyState & m_dwButtonDrop) == 0 )
			return m_bDragStarted ? DRAGDROP_S_DROP : DRAGDROP_S_CANCEL;

		// otherwise, keep polling...
		return S_OK;
	}


	STDMETHOD	(GiveFeedback) (DWORD)
	{
		// don't change the cursor until drag is officially started
		return m_bDragStarted ? DRAGDROP_S_USEDEFAULTCURSORS : S_OK;
	}


public:
	// Mothods
	DROPEFFECT DoDragDrop(IDataObject *pDataObject, DWORD dwOKEffects = DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK)
	{
		// call global OLE api to do the drag drop
		DWORD dwResultEffect = DROPEFFECT_NONE;

		::DoDragDrop(pDataObject, (IDropSource *) this, dwOKEffects, &dwResultEffect);
		return dwResultEffect;
	}


	bool PreDoDragDrop(HWND hWnd, LPRECT lpRectStartDrag = NULL, bool bUseDragDelay = false)
	{								// cf. MFC6::COleDataSource::DoDragDrop
		m_bDragStarted = false;

		if (lpRectStartDrag != NULL) {
			// set drop source drag start rect to parameter provided
			m_rectStartDrag.CopyRect(lpRectStartDrag);
		} else {
			// otherwise start with default empty rectangle around current point
			CPoint		ptCursor;
			::GetCursorPos(&ptCursor);
			m_rectStartDrag.SetRect(ptCursor.x, ptCursor.y, ptCursor.x, ptCursor.y);
		}

		if ( m_rectStartDrag.IsRectNull() ) {
			// null rect specifies no OnBeginDrag wait loop
			m_bDragStarted = true;
		} else if ( m_rectStartDrag.IsRectEmpty() ) {
			// empty rect specifies drag drop around starting point
			m_rectStartDrag.InflateRect( s_nDragMinDist * 2, s_nDragMinDist * 2);
		}

		// before calling OLE drag/drop code, wait for mouse to move outside
		//	the rectangle
		return OnBeginDrag(hWnd, bUseDragDelay);
	}


private:

	// Implementation
	bool OnBeginDrag(HWND hWnd, bool bUseDragDelay)
	{
		DSTRACE( _T("IDropSrouceImpl::OnBeginDrag\n") );

		m_bDragStarted	 = false;

		// opposite button cancels drag operation
		m_dwButtonCancel = 0;
		m_dwButtonDrop	 = 0;

		if (::GetKeyState(VK_LBUTTON) < 0) {
			m_dwButtonDrop	 |= MK_LBUTTON;
			m_dwButtonCancel |= MK_RBUTTON;
		} else if (::GetKeyState(VK_RBUTTON) < 0) {
			m_dwButtonDrop	 |= MK_RBUTTON;
			m_dwButtonCancel |= MK_LBUTTON;
		}

		DWORD dwLastTick = GetTickCount();
		::SetCapture(hWnd);

		while (!m_bDragStarted) {
			// some applications steal capture away at random times
			if (::GetCapture() != hWnd) {
				DSTRACE( _T(" some applications steal capture away at random times\n") );
				break;
			}

			// peek for next input message
			MSG msg;

			// Your system has to know about Filter message,
			// For example, you can never give WM_MOUSEWHEEL to PeekMessage on win95.
			if (  PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE)
			   || PeekMessage(&msg, NULL, WM_KEYFIRST  , WM_KEYLAST  , PM_REMOVE) )
			{
				// check for button cancellation (any button down will cancel)
				if (  msg.message == WM_LBUTTONUP	|| msg.message == WM_RBUTTONUP
				   || msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN)
				{
					DSTRACE( _T(" button cancellation\n") );
					// _LButtonDblClickMessenger(msg, hWnd);
					break;
				}

				// check for keyboard cancellation
				if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
					DSTRACE( _T(" keyboard cancellation\n") );
					break;
				}

				// check for drag start transition
				DSTRACE(_T(" check for drag start transition : pt(%d, %d) rect(%d, %d, %d, %d)\n"),
						msg.pt.x, msg.pt.y, m_rectStartDrag.left, m_rectStartDrag.top, m_rectStartDrag.right,
						m_rectStartDrag.bottom);
				m_bDragStarted = !m_rectStartDrag.PtInRect(msg.pt);
			}

			// if the user sits here long enough, we eventually start the drag
			if (bUseDragDelay && ::GetTickCount() - dwLastTick > s_nDragDelay) {
				DSTRACE( _T(" we eventually start the drag!\n") );
				m_bDragStarted = true;
			}
		}

		::ReleaseCapture();

		return m_bDragStarted;
	}


	void _LButtonDblClickMessenger(MSG msgSrc, HWND hWnd)
	{
		if (msgSrc.message != WM_LBUTTONUP)
			return;

		// eat next message if click is on the same button
		MSG msg;

		if ( ::PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE) ) {
			if ( msg.message == WM_LBUTTONDOWN && CPoint(msgSrc.pt) == CPoint(msg.pt) ) { // same point
				CPoint pt = msg.pt;
				::ScreenToClient(hWnd, &pt);
				::PostMessage( hWnd, WM_LBUTTONDBLCLK, msg.wParam, MAKELPARAM(pt.x, pt.y) );
			}
		}
	}
};


/////////////////////////////////////////////////////////////////////////////
// IDropTargetImpl
//
#ifdef _DEBUG
const bool _bDropTargetImplTraceOn = false;
	#define DTTRACE 	if (_bDropTargetImplTraceOn)	ATLTRACE
#else
	#define DTTRACE
#endif



// helper to filter out invalid DROPEFFECTs
inline DROPEFFECT _MtlFilterDropEffect(DROPEFFECT dropEffect, DROPEFFECT dwEffects)
{
	// return allowed dropEffect and DROPEFFECT_NONE
	if ( (dropEffect & dwEffects) != 0 )
		return dropEffect;

	// map common operations (copy/move) to alternates, but give negative
	//	feedback for DROPEFFECT_LINK.

	switch (dropEffect) {
	case DROPEFFECT_COPY:
		if (dwEffects & DROPEFFECT_MOVE)
			return DROPEFFECT_MOVE;
		else if (dwEffects & DROPEFFECT_LINK)
			return DROPEFFECT_LINK;
		break;

	case DROPEFFECT_MOVE:
		if (dwEffects & DROPEFFECT_COPY)
			return DROPEFFECT_COPY;
		else if (dwEffects & DROPEFFECT_LINK)
			return DROPEFFECT_LINK;
		break;

	case DROPEFFECT_LINK:
		break;
	}

	return DROPEFFECT_NONE;
}



// helper to get a standard effect
inline DROPEFFECT _MtlStandardDropEffect(DWORD dwKeyState)
{
	DROPEFFECT dropEffect;

	// check for force link
	if ( ( dwKeyState & (MK_CONTROL | MK_SHIFT) ) == (MK_CONTROL | MK_SHIFT) )
		dropEffect = DROPEFFECT_SCROLL | DROPEFFECT_LINK;

	// check for force copy
	else if ( (dwKeyState & MK_CONTROL) == MK_CONTROL )
		dropEffect = DROPEFFECT_SCROLL | DROPEFFECT_COPY;

	// check for force move
	else if ( (dwKeyState & MK_ALT) == MK_ALT
			|| (dwKeyState & MK_SHIFT) == MK_SHIFT )
		dropEffect = DROPEFFECT_SCROLL | DROPEFFECT_MOVE;

	// default -- recommended action is move
	else
		dropEffect = DROPEFFECT_SCROLL | DROPEFFECT_MOVE;

	return dropEffect;
}



inline DROPEFFECT _MtlFollowDropEffect(DROPEFFECT dwEffects)
{
	dwEffects &= ~DROPEFFECT_SCROLL;

	if (dwEffects == DROPEFFECT_COPY)		// only copy
		return DROPEFFECT_COPY;
	else if (dwEffects == DROPEFFECT_LINK)	// only link
		return DROPEFFECT_LINK;
	else
		return 0;							// no need to add
}



class ATL_NO_VTABLE _IDropTarget {			// = emulates IDropTarget's vtable
public:
	//this method needs a different name than QueryInterface
	STDMETHOD							(_LocDTQueryInterface) (REFIID riid, void **ppvObject) = 0;
	virtual ULONG STDMETHODCALLTYPE 	AddRef(void)  = 0;
	virtual ULONG STDMETHODCALLTYPE 	Release(void) = 0;
};



class ATL_NO_VTABLE _IDropTargetLocator : public _IDropTarget {
public:
};



class IDropTargetImplBase {
	static bool 	s_bStaticInit;

	// metrics for drag-scrolling
public:
	static int		s_nScrollInset;
	static UINT 	s_nScrollDelay;
	static UINT 	s_nScrollInterval;

public:
	IDropTargetImplBase()
	{
		// init static variables
		if (!s_bStaticInit) {
			::EnterCriticalSection(&_Module.m_csStaticDataInit);

			if (!s_bStaticInit) {
				// get scroll metrics from win.ini
				static const TCHAR szWindows[]		  = _T("windows");
				static const TCHAR szScrollDelay[]	  = _T("DragScrollDelay");
				static const TCHAR szScrollInset[]	  = _T("DragScrollInset");
				static const TCHAR szScrollInterval[] = _T("DragScrollInterval");

				s_nScrollInset	  = ::GetProfileInt(szWindows, szScrollInset, DD_DEFSCROLLINSET);
				s_nScrollDelay	  = ::GetProfileInt(szWindows, szScrollDelay, DD_DEFSCROLLDELAY);
				s_nScrollInterval = ::GetProfileInt(szWindows, szScrollInterval, DD_DEFSCROLLINTERVAL);
				s_bStaticInit	  = true;
			}

			::LeaveCriticalSection(&_Module.m_csStaticDataInit);
		}
	}
};



__declspec(selectany) bool IDropTargetImplBase::s_bStaticInit	  = false;
__declspec(selectany) int  IDropTargetImplBase::s_nScrollInset	  = DD_DEFSCROLLINSET;
__declspec(selectany) UINT IDropTargetImplBase::s_nScrollDelay	  = DD_DEFSCROLLDELAY;
__declspec(selectany) UINT IDropTargetImplBase::s_nScrollInterval = DD_DEFSCROLLINTERVAL;



template <class T>
class ATL_NO_VTABLE IDropTargetImpl : public _IDropTargetLocator, public IDropTargetImplBase {
public:
	// COM Identity
	STDMETHOD	(_LocDTQueryInterface) (REFIID riid, void **ppvObject)
	{
		if ( InlineIsEqualUnknown(riid) || InlineIsEqualGUID(riid, IID_IDropTarget) ) {
			if (ppvObject == NULL)
				return E_POINTER;

			*ppvObject = this;
			AddRef();
		  #ifdef _ATL_DEBUG_INTERFACES
			_Module.AddThunk( (IUnknown **) ppvObject, _T("IDropTargetImpl"), riid );
		  #endif // _ATL_DEBUG_INTERFACES
			return S_OK;

		} else {
			return E_NOINTERFACE;
		}
	}


private:
	virtual ULONG STDMETHODCALLTYPE 	AddRef()
	{
		return 1;
	}


	virtual ULONG STDMETHODCALLTYPE 	Release()
	{
		return 1;
	}


	// Methods
	DROPEFFECT	GetOkDropEffect()
	{
	}


	// IDropTarget
	STDMETHOD	(DragEnter) (IDataObject * pDataObject, DWORD dwKeyState, POINTL pt, DWORD * pdwEffect)
	{
		DTTRACE( _T("IDropTargetImpl::DragEnter\n") );
		ATLASSERT(pdwEffect   != NULL);
		ATLASSERT(pDataObject != NULL);

		SCODE	   sc		  = E_UNEXPECTED;

		// cache lpDataObject
		DTTRACE( _T(" cache lpDataObject step1\n") );
		m_spDataObject.Release();
		ATLASSERT(m_spDataObject.p == NULL);
		DTTRACE( _T(" cache lpDataObject step2\n") );
		m_spDataObject = pDataObject;

		T * 	   pT		  = static_cast<T *>(this);
		CPoint	   point( (int) pt.x, (int) pt.y );
		pT->ScreenToClient(&point);

		// check first for entering scroll area
		DROPEFFECT dropEffect = pT->OnDragScroll(dwKeyState, point);

		if ( (dropEffect & DROPEFFECT_SCROLL) == 0 ) {
			// funnel through OnDragEnter since not in scroll region
			dropEffect = pT->OnDragEnter(pDataObject, dwKeyState, point);
		}

		*pdwEffect	   = _MtlFilterDropEffect(dropEffect, *pdwEffect);
		sc			   = S_OK;

		return sc;
	}


	// If pdwEffect is only DROPEFFECT_COPY, you have to add DROPEFFECT_COPY to pdwEffect.
	// That is, MFC always sucks.
	STDMETHOD	(DragOver) (DWORD dwKeyState, POINTL pt, DWORD * pdwEffect)
	{
		DTTRACE( _T("IDropTargetImpl::DragOver\n") );
		ATLASSERT(pdwEffect != NULL);
		ATLASSERT(m_spDataObject != NULL);

		SCODE	   sc		  = E_UNEXPECTED;

		T * 	   pT		  = static_cast<T *>(this);
		CPoint	   point( (int) pt.x, (int) pt.y );
		pT->ScreenToClient(&point);

		// check first for entering scroll area
		DROPEFFECT dropEffect = pT->OnDragScroll(dwKeyState, point);

		if ( (dropEffect & DROPEFFECT_SCROLL) == 0 ) {
			// funnel through OnDragOver
			dropEffect = pT->OnDragOver(m_spDataObject, dwKeyState, point, *pdwEffect);
		}

		*pdwEffect = _MtlFilterDropEffect(dropEffect, *pdwEffect);
		sc		   = S_OK;

		return sc;
	}


public:
	STDMETHOD	(DragLeave) ()
	{
		DTTRACE( _T("IDropTargetImpl::DragLeave\n") );
		// cancel drag scrolling
		m_nTimerID = MAKEWORD(-1, -1);

		T *pT = static_cast<T *>(this);
		pT->OnDragLeave();

		// release cached data object
		m_spDataObject.Release();
		ATLASSERT(m_spDataObject.p == NULL);

		return S_OK;
	}


private:
	STDMETHOD	(Drop) (IDataObject * pDataObject, DWORD dwKeyState, POINTL pt, DWORD * pdwEffect)
	{
		DTTRACE( _T("IDropTargetImpl::Drop\n") );
		ATLASSERT(pdwEffect != NULL);
		ATLASSERT(pDataObject != NULL);

		SCODE	   sc		  = E_UNEXPECTED;

		// cancel drag scrolling
		m_nTimerID = MAKEWORD(-1, -1);

		// prepare for call to OnDragOver
		T * 	   pT		  = static_cast<T *>(this);
		CPoint	   point( (int) pt.x, (int) pt.y );
		pT->ScreenToClient(&point);

		// verify that drop is legal
		DROPEFFECT dropEffect = _MtlFilterDropEffect(pT->OnDragOver(pDataObject, dwKeyState, point, *pdwEffect), *pdwEffect);

		// execute the drop (try OnDropEx then OnDrop for backward compatibility)
		dropEffect = pT->OnDrop(pDataObject, dropEffect, *pdwEffect, point);

		// release potentially cached data object
		m_spDataObject.Release();

		*pdwEffect = dropEffect;
		sc		   = S_OK;

		return sc;
	}


private:
	// Data members
	CComPtr<IDataObject> m_spDataObject;	// != NULL between OnDragEnter, OnDragLeave
	UINT				 m_nTimerID;		// != MAKEWORD(-1, -1) when in scroll area
	DWORD				 m_dwLastTick;		// only valid when m_nTimerID valid
	UINT				 m_nScrollDelay;	// time to next scroll


public:
	// Ctor/dtor
	IDropTargetImpl()
		: m_nTimerID( MAKEWORD(-1, -1) )
	  #if 1 //*+++
		, m_dwLastTick(0)
		, m_nScrollDelay(0)
	  #endif
	{
	}


	~IDropTargetImpl()
	{
		ATLASSERT(m_spDataObject.p == NULL);
	}


	// Methods
	bool RegisterDragDrop()
	{
		DTTRACE( _T("IDropTargetImpl::RegisterDragDrop\n") );
		T *pT = static_cast<T *>(this);
		ATLASSERT( ::IsWindow(pT->m_hWnd) );

		// the object must be locked externally to keep LRPC connections alive
		if (::CoLockObjectExternal( (IUnknown *) this, true, false ) != S_OK)
			return false;

		// connect the HWND to the IDropTarget implementation
		if (::RegisterDragDrop(pT->m_hWnd, (IDropTarget *) this) != S_OK) {
			::CoLockObjectExternal( (IUnknown *) this, false, false );
			return false;
		}

		return true;
	}


	void RevokeDragDrop()
	{
		DTTRACE( _T("IDropTargetImpl::RevokeDragDrop\n") );
		T *pT = static_cast<T *>(this);
		// disconnect from OLE
		::RevokeDragDrop(pT->m_hWnd);
		::CoLockObjectExternal( (IUnknown *) this, false, true );
	}


private:
	// Overridables
	bool OnScroll(UINT nScrollCode, UINT nPos, bool bDoScroll = true)
	{
		return false;
	}


	DROPEFFECT OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
	{
		return _MtlStandardDropEffect(dwKeyState);
	}


	DROPEFFECT OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
	{
		return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect);
	}


	DROPEFFECT OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
	{
		return DROPEFFECT_NONE;
	}


	void OnDragLeave()
	{
	}


	// default implementation of drag/drop scrolling
	DROPEFFECT OnDragScroll(DWORD dwKeyState, CPoint point)
	{
		T * 	   pT		= static_cast<T *>(this);
		// get client rectangle of destination window
		CRect	   rectClient;

		pT->GetClientRect(&rectClient);
		CRect	   rect 	= rectClient;

		// hit-test against inset region
		UINT	   nTimerID = MAKEWORD(-1, -1);
		rect.InflateRect(-s_nScrollInset, -s_nScrollInset);

		//		CSplitterWnd* pSplitter = NULL;
		if ( rectClient.PtInRect(point) && !rect.PtInRect(point) ) {
			// determine which way to scroll along both X & Y axis
			if (point.x < rect.left)
				nTimerID = MAKEWORD( SB_LINEUP, HIBYTE(nTimerID) );
			else if (point.x >= rect.right)
				nTimerID = MAKEWORD( SB_LINEDOWN, HIBYTE(nTimerID) );

			if (point.y < rect.top)
				nTimerID = MAKEWORD(LOBYTE(nTimerID), SB_LINEUP);
			else if (point.y >= rect.bottom)
				nTimerID = MAKEWORD(LOBYTE(nTimerID), SB_LINEDOWN);

			ATLASSERT( nTimerID != MAKEWORD(-1, -1) );

			// check for valid scroll first
			BOOL bEnableScroll = false;
			bEnableScroll = pT->OnScroll(nTimerID, 0, false);

			if (!bEnableScroll)
				nTimerID = MAKEWORD(-1, -1);
		}

		if ( nTimerID == MAKEWORD(-1, -1) ) {
			if ( m_nTimerID != MAKEWORD(-1, -1) ) {
				// send fake OnDragEnter when transition from scroll->normal
				pT->OnDragEnter(m_spDataObject, dwKeyState, point);
				m_nTimerID = MAKEWORD(-1, -1);
			}

			return DROPEFFECT_NONE;
		}

		// save tick count when timer ID changes
		DWORD	   dwTick = GetTickCount();

		if (nTimerID != m_nTimerID) {
			m_dwLastTick   = dwTick;
			m_nScrollDelay = s_nScrollDelay;
		}

		// scroll if necessary
		if (dwTick - m_dwLastTick > m_nScrollDelay) {
			pT->OnScroll(nTimerID, 0, true);
			m_dwLastTick   = dwTick;
			m_nScrollDelay = s_nScrollInterval;
		}

		if ( m_nTimerID == MAKEWORD(-1, -1) ) {
			// send fake OnDragLeave when transitioning from normal->scroll
			pT->OnDragLeave();
		}

		m_nTimerID = nTimerID;
		DROPEFFECT dropEffect;

		// check for force link
		if ( ( dwKeyState & (MK_CONTROL | MK_SHIFT) ) == (MK_CONTROL | MK_SHIFT) )
			dropEffect = DROPEFFECT_SCROLL | DROPEFFECT_LINK;

		// check for force copy
		else if ( (dwKeyState & MK_CONTROL) == MK_CONTROL )
			dropEffect = DROPEFFECT_SCROLL | DROPEFFECT_COPY;

		// check for force move
		else if ( (dwKeyState & MK_ALT) == MK_ALT
				|| (dwKeyState & MK_SHIFT) == MK_SHIFT )
			dropEffect = DROPEFFECT_SCROLL | DROPEFFECT_MOVE;

		// default -- recommended action is move
		else
			dropEffect = DROPEFFECT_SCROLL | DROPEFFECT_MOVE;

		return dropEffect;
	}

};



// RButtonDrop service
inline bool MtlOnRButtonDrop(HWND hWnd, DROPEFFECT &dropEffect, CPoint pt, bool bAddShortcut = true)
{
	enum { s_nIDMove = 1, s_nIDCopy = 2, s_nIDShortCut = 3, s_nIDCancel = 4, };
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu( MF_ENABLED | MF_STRING, s_nIDMove, _T("ここに移動(&M)") );
	menu.AppendMenu( MF_ENABLED | MF_STRING, s_nIDCopy, _T("ここにコピー(&C)") );

	if (bAddShortcut)
		menu.AppendMenu( MF_ENABLED | MF_STRING, s_nIDShortCut, _T("ショートカットをここに作成(&S)") );

	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu( MF_ENABLED | MF_STRING, s_nIDCancel, _T("キャンセル") );

	if (dropEffect & DROPEFFECT_COPY)
		menu.SetMenuDefaultItem(s_nIDCopy, FALSE);
	else if (dropEffect & DROPEFFECT_MOVE)
		menu.SetMenuDefaultItem(s_nIDMove, FALSE);
	else if (dropEffect & DROPEFFECT_LINK)
		menu.SetMenuDefaultItem(s_nIDShortCut, FALSE);

	::ClientToScreen(hWnd, &pt);
	UINT  uMenuFlags = TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_TOPALIGN;
	int   nCmdID	 = menu.TrackPopupMenu(uMenuFlags | TPM_RETURNCMD , pt.x, pt.y, hWnd, NULL);

	if (nCmdID == s_nIDMove)
		dropEffect = DROPEFFECT_MOVE;
	else if (nCmdID == s_nIDCopy)
		dropEffect = DROPEFFECT_COPY;
	else if (nCmdID == s_nIDShortCut)
		dropEffect = DROPEFFECT_LINK;
	else
		return true;

	// canceled, eat it!
	return false;
}



// RButtonDrop service
inline bool MtlOnRButtonDropOpenFromApp(HWND hWnd, CPoint pt, const CString &strExePath, const CString &strArg)
{
	if ( !MtlIsFileExtExe(strExePath) )
		return false;

	enum { s_nIDOpen = 1, s_nIDCancel = 2 };
	CMenu	menu;
	menu.CreatePopupMenu();
	menu.AppendMenu( MF_ENABLED | MF_STRING, s_nIDOpen, _T("アプリケーションから開く(&O)") );
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu( MF_ENABLED | MF_STRING, s_nIDCancel, _T("キャンセル") );

	menu.SetMenuDefaultItem(s_nIDOpen, FALSE);

	::ClientToScreen(hWnd, &pt);
	UINT  uMenuFlags = TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_TOPALIGN;
	int   nCmdID	 = menu.TrackPopupMenu(uMenuFlags | TPM_RETURNCMD , pt.x, pt.y, hWnd, NULL);

	if (nCmdID == s_nIDOpen) {
		CString strExePath_(strExePath);
		MtlPreOpenFile(strExePath_, strArg);
		return true;
	} else {
		return true;
	}

	// canceled, eat it!
	return false;
}



////////////////////////////////////////////////////////////////////////////


}		//namespace MTL



#endif	// __MTLDRAGDROP_H__
