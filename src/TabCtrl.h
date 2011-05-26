/**
 *	@file	TabCtrl.h
 *	@brief	MTL : タブ制御
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

// April 10, 2004 minit added some

#pragma once

#include <vector>
#include "DropDownButton.h"
//#include "atltheme_d.h"
#include "option/SkinOption.h"
#include "DonutPFunc.h"
//スキン切り替えとかの機能を入れたので移植性とかかなり損なわれてます。
//ごめんなさい(minit)

namespace MTL {


// for debug
#ifdef _DEBUG
const bool _Mtl_TabCtrl2_traceOn = false;
	# define TCTRACE	if (_Mtl_TabCtrl2_traceOn)	ATLTRACE
#else
	# define TCTRACE
#endif


/////////////////////////////////////////////////////////////////////////////
// Tab Bars

#define	CXICON	16
#define CYICON	16

// Window Styles:
enum ETab2Ws {
	TAB2WS_TOP				= CCS_TOP,
	TAB2WS_BOTTOM			= CCS_BOTTOM,
	TAB2WS_NORESIZE 		= CCS_NORESIZE,
	TAB2WS_NOPARENTALIGN	= CCS_NOPARENTALIGN,
	TAB2WS_NODIVIDER		= CCS_NODIVIDER,
};


// Extended styles
enum ETab2_Ex {
	TAB2_EX_TRANSPARENT 	= 0x00000001L,	// not supported yet
	TAB2_EX_SHAREIMGLIST	= 0x00000002L,
	TAB2_EX_MULTILINE		= 0x00000004L,	// 複数列表示にする
	TAB2_EX_FIXEDSIZE		= 0x00000008L,	// タブの大きさを固定する
	TAB2_EX_SUPPORTREBAR	= 0x00000010L,
//	TAB2_EX_SCROLLOPPOSITE	= 0x00000020L,
	TAB2_EX_ANCHORCOLOR 	= 0x00000040L,	// タブを表示、未表示で色を変える
	TAB2_EX_MOUSEDOWNSELECT = 0x00000080L,	// クリックしたときにタブを切り替える
};


// Command ID
enum { COMMAND_ID_DROPBUTTON= 0x00000001L, };




/////////////////////////////////////////////////////////////////////////////
// Forward declarations

template <class T, class TBase, class TWinTraits>
class	CCommandBarCtrlImpl;

class	CTabCtrlItem;

class	CTabCtrl2;



/////////////////////////////////////////////////////////////////////////////
// CTabButton

// CTabButton state flags
enum ETcistate {
	TCISTATE_HIDDEN 			= 0x01,
	// standard text state
	TCISTATE_ENABLED			= 0x02,
	TCISTATE_INACTIVE			= 0x04,
	// select or not
	TCISTATE_SELECTED			= 0x08, 	// ordianry selected
	TCISTATE_MSELECTED			= 0x10, 	// multi-selected
	// event state
	TCISTATE_PRESSED			= 0x20, 	// mouse pressed
	TCISTATE_HOT				= 0x40, 	// mouse above item
	TCISTATE_MOUSEAWAYCAPTURED	= 0x80, 	// mouse away but captured
};


/////////////////////////////////////////////////////////////////////////////
// CTabSkin

class CTabSkin
{
	friend class CTabItem;

	enum { s_kcxIconGap = 5 };

protected:
	COLORREF	m_colText;
	COLORREF	m_colActive;
	COLORREF	m_colInActive;
	COLORREF	m_colDisable;
	COLORREF	m_colDisableHi;
	COLORREF	m_colBtnFace;
	COLORREF	m_colBtnHi;

	CImageList	m_imgLock;

protected:
	// コンストラクタ
	CTabSkin() { _LoadTabTextSetting(); _LoadImage(); }
public:
	virtual ~CTabSkin() { m_imgLock.Destroy(); }

	void	Update(CDCHandle dc, HIMAGELIST hImgList, const CTabCtrlItem& item, bool bAnchorColor);

private:
	void	_LoadTabTextSetting();
	void	_LoadImage();

	void	_DrawText(CDCHandle dc, CPoint ptOffset, const CTabCtrlItem& item, bool bAnchorColor);

	// Overridable
	virtual void  _DrawSkinCur(CDCHandle dc, CRect rcItem) = 0;		// Active
	virtual void  _DrawSkinNone(CDCHandle dc, CRect rcItem) = 0;	// Normal
	virtual void  _DrawSkinSel(CDCHandle dc, CRect rcItem) = 0;		// Hot
};



////////////////////////////////////////////////////////////////////////////
// CTabSkinDefault

class CTabSkinDefault : public CTabSkin
{
private:
	CBitmap		m_TabSkinCur;
	CBitmap		m_TabSkinNone;
	CBitmap		m_TabSkinSel;

public:
	// コンストラクタ
	CTabSkinDefault();

private:
	void	_LoadBitmap();

	void	_DrawSkin(HDC hDC, CRect rcItem, CBitmap& pBmp);

	// Overrides
	void	_DrawSkinCur(CDCHandle dc, CRect rcItem);
	void	_DrawSkinNone(CDCHandle dc, CRect rcItem);
	void	_DrawSkinSel(CDCHandle dc, CRect rcItem);
};


////////////////////////////////////////////////////////////////////////////
// CTabSkinTheme

class CTabSkinTheme : public CTabSkin
{
private:
	CTheme&	m_Theme;

public:
	// コンストラクタ
	CTabSkinTheme(CTheme& theme);

private:
	// Overrides
	void	_DrawSkinCur(CDCHandle dc, CRect rcItem);
	void	_DrawSkinNone(CDCHandle dc, CRect rcItem);
	void	_DrawSkinSel(CDCHandle dc, CRect rcItem);
};

////////////////////////////////////////////////////////////////////////////
// CTabSkinClassic

class CTabSkinClassic : public CTabSkin
{
private:
	CSimpleArray<CPoint>&	  m_rarrSeparators;				// Point of separator

public:
	// コンストラクタ
	CTabSkinClassic(CSimpleArray<CPoint>& arrSep) 
		: m_rarrSeparators(arrSep)
	{ }

private:
	// Overrides
	void	_DrawSkinCur(CDCHandle dc, CRect rcItem);
	void	_DrawSkinNone(CDCHandle dc, CRect rcItem);
	void	_DrawSkinSel(CDCHandle dc, CRect rcItem);

	void	_DrawSeparators(CDCHandle dc, CRect rcItem);
};




////////////////////////////////////////////////////////////////////////////
// CTabCtrlItem : タブ１個の情報

class CTabCtrlItem 
{
public:
	// Data members
	BYTE		m_fsState;
	CString 	m_strItem;		// string on button
	CRect		m_rcItem;
	int 		m_nImgIndex;	// image list index
	DWORD_PTR	m_dwUser;		// user's data 

private:
	// Constants
	enum { s_kcxIconGap = 2 };

public:
	// Constructor/destructor
	CTabCtrlItem(const CString &strBtn = CString(), int nImgIndex = -1, DWORD_PTR dwUser = 0, BYTE fsState = TCISTATE_ENABLED)
		: m_fsState(fsState)
		, m_strItem(strBtn)
		, m_nImgIndex(nImgIndex)
		, m_dwUser(dwUser)
	{
	}


	// Attributes
	bool ModifyState(BYTE fsRemove, BYTE fsAdd)
	{
		BYTE fsStateOld = m_fsState;

		m_fsState = (m_fsState & ~fsRemove) | fsAdd;

		if ( (fsStateOld & TCISTATE_SELECTED || fsStateOld & TCISTATE_MSELECTED)
		   && (fsRemove == TCISTATE_HOT || fsAdd == TCISTATE_HOT) ) 	// selected, no need to update
			return false;
		else
			return m_fsState != fsStateOld;
	}

};



/////////////////////////////////////////////////////////////////////////////
// CTabCtrl2 - MTL implementation of Tab Ctrl2

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CTabCtrl2Impl
	: public CDoubleBufferWindowImpl< T, TBase, TWinTraits >
	, public CTrackMouseLeave<CTabCtrl2Impl< T, TBase, TWinTraits > >
	, public CThemeImpl<CTabCtrl2Impl< T, TBase, TWinTraits > >
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, -1)

private:
	typedef CTabCtrl2Impl< T, TBase, TWinTraits >	thisClass;

public:
	// Constants
	enum _TabCtrl2DrawConstants {
		s_kcxTextMargin = 7,
		s_kcyTextMargin = 3,
		s_kcxGap		= 2,
		s_kcyGap		= 2,
		s_kcxSeparator	= 2,

		s_kcxUpDown 	= 28,
		s_kcyUpDown 	= 14
	};

private:
	enum {
		_nMaxMenuItemTextLength = 100,
	};

	// Data members
public:
	typedef std::vector<CTabCtrlItem>	TabItems;
	TabItems			m_items;
	CImageList			m_imgs;

	CFont				m_font;
private:
	DWORD				m_dwTabCtrl2ExtendedStyle;	// TabCtrl2 specific extended styles

	int 				m_nHotIndex;				// Current hot index
	int 				m_nPressedIndex;			// Current capturing index
	CSimpleArray<CPoint> m_arrSeparators;			// Point of separator
	

	CSize				m_sizeItem;					// for fixed size
	bool				m_bLockRefreshBandInfo;		// trueで描写を抑制する
	int 				m_nDrawStyle;
	unique_ptr<CTabSkin> m_pTabSkin;
	CToolTipCtrl		m_tip;

	bool				m_bUseTheme;
	CString				m_strToolTip;

protected:
	int 				m_nFirstIndexOnSingleLine;	// for single line
	CUpDownCtrl			m_wndUpDown;				// for single line
	CDropDownButtonCtrl	m_wndDropBtn;

protected:
	// Constructor/destructor
	CTabCtrl2Impl();
	~CTabCtrl2Impl();

	void	_InitToolTip();

	bool	_IsValidIndex(int nIndex) const;

	int		_GetRequiredHeight();
	CRect	_MeasureItem(const CString &strText);

	void	_HotItem(int nNewHotIndex = -1);
	void	_PressItem(int nPressedIndex = -1);

	bool	_FindIndexFromCurMultiSelected(int nIndex);
	void	_ResetMultiSelectedItems();

	void	_ShowOrHideUpDownCtrl(const CRect &rcClient);
	void	_ScrollOpposite(int nNewSel, bool bClicked);

	void	_RefreshBandInfo();
	void	_UpdateSingleLineLayout(int nFirstIndex);
	void	_UpdateMultiLineLayout(int nWidth);
	void	_UpdateLayout();
	
	void	_DoPaint(CDCHandle dc, LPCRECT lpRect = NULL);

	const CRect _InflateGapWidth(const CRect &rc) const;

	void	_SetDrawStyle(int nStyle);
	void	_ReloadSkinData();

private:
	// Overridables
	void	OnSetCurSel(int nIndex) { }
	CString OnGetToolTipText(int nIndex);
	void	OnDropDownButton() { }


public:
	// Attributes
	DWORD	GetTabCtrl2ExtendedStyle() const { return m_dwTabCtrl2ExtendedStyle; }

	void	SetItemSize(const CSize &size);
	CSize	GetItemSize() { return m_sizeItem; }

	void	ModifyTabCtrl2ExtendedStyle(DWORD dwRemove, DWORD dwAdd);

	void	ReloadSkin(int nStyle);
	void	SetFont(HFONT hFont) { m_font = hFont; }

	int		GetItemCount() const { return (int) m_items.size(); }

	int		GetCurSel();

	int		HitTest(const CPoint& point);

	bool	InsertItem(int nIndex, const CTabCtrlItem &item);

	void	DeleteItems(CSimpleArray<int> &arrSrcs);

	bool	MoveItems(int nDestIndex, CSimpleArray<int> &arrSrcs);


	bool	SetItemText(int nIndex, const CString &str);
	bool	SetItemImageIndex(int nIndex, int nImgIndex);
	bool	SetItemInactive(int nIndex);
	bool	SetItemActive(int nIndex);
	bool	SetItemDisabled(int nIndex);
	bool	SetItemEnabled(int nIndex);

	// CTabCtrlItem
	const CRect& GetItemRect(int nIndex) const { return m_items.at(nIndex).m_rcItem; }


protected:
	bool	SetItemUserData(int nIndex, DWORD_PTR dwUser);

	bool	GetItem(int nIndex, CTabCtrlItem &item);

	CTabCtrlItem& GetItem(int nIndex) { return m_items.at(nIndex); }

public:
	bool	GetItemText(int nIndex, CString &str);
	bool	GetItemImageIndex(int nIndex, int &nImgIndex);
	bool	GetItemUserData(int nIndex, DWORD_PTR &dwData);
	bool	GetItemState(int nIndex, BYTE &bytData);
	bool	GetItemRect(int nIndex, CRect &rc);

	int		GetFirstVisibleIndex();
	int		GetLastVisibleIndex();

	bool	AddItem(const CTabCtrlItem &item);

	bool	DeleteItem(int nIndex);

	void	GetCurMultiSel(CSimpleArray<int> &arrDest, bool bIncludeCurSel = true);
	void	GetCurMultiSelEx(CSimpleArray<int>& arrDest, int nIndex);

	bool	SetCurSel(int nIndex, bool bClicked = false);

public:
	int		GetItemHeight() const;

	bool	CanScrollItem(bool bRight = true) const;
	bool	ScrollItem(bool bRight = true);


	// Overrides
	void	OnTrackMouseMove(UINT nFlags, CPoint pt);
	void	OnTrackMouseLeave();
	void	DoPaint(CDCHandle dc);

public:
	// Message map and handlers
	BEGIN_MSG_MAP( CTabCtrl2Impl )
		CHAIN_MSG_MAP( CThemeImpl<CTabCtrl2Impl> )
		COMMAND_ID_HANDLER( COMMAND_ID_DROPBUTTON,OnPushDropButton	)
		MESSAGE_HANDLER( WM_CREATE			, OnCreate			)
		MESSAGE_HANDLER( WM_WINDOWPOSCHANGING,OnWindowPosChanging )
		MESSAGE_HANDLER( WM_LBUTTONDOWN 	, OnLButtonDown 	)
		MESSAGE_HANDLER( WM_LBUTTONUP		, OnLButtonUp		)
		MESSAGE_HANDLER( WM_ERASEBKGND		, OnEraseBackground )
		NOTIFY_CODE_HANDLER( UDN_DELTAPOS	, OnDeltaPos		)
		NOTIFY_CODE_HANDLER( TTN_GETDISPINFO, OnGetDispInfo 	)
		CHAIN_MSG_MAP( CTrackMouseLeave<CTabCtrl2Impl> )
		CHAIN_MSG_MAP( CDoubleBufferWindowImpl )
		REFLECT_NOTIFICATIONS( )
	END_MSG_MAP( )


	LRESULT OnPushDropButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
	LRESULT OnDeltaPos(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnWindowPosChanging(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	bool _MustBeInvalidateOnMultiLine(CSize size);

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

private:

	void	_DrawSeparators(CDCHandle dc, LPCRECT lpRect = NULL);

};




//////////////////////////////////////////////////////////////////////
// CTabCtrl2Impl

template <class T, class TBase, class TWinTraits>
CTabCtrl2Impl<T, TBase, TWinTraits>::CTabCtrl2Impl()
	: m_dwTabCtrl2ExtendedStyle(TAB2_EX_TRANSPARENT | TAB2_EX_SUPPORTREBAR)
	, m_nFirstIndexOnSingleLine(0)
	, m_sizeItem(200, 50)
	, m_nHotIndex(-1)
	, m_nPressedIndex(-1)
	, m_bLockRefreshBandInfo(false)
	, m_nDrawStyle(SKN_TAB_STYLE_DEFAULT)
	, m_bUseTheme(false)
{
	SetThemeClassList(L"TAB");
}

template <class T, class TBase, class TWinTraits>
CTabCtrl2Impl<T, TBase, TWinTraits>::~CTabCtrl2Impl()
{
	if (m_imgs.m_hImageList != NULL && (m_dwTabCtrl2ExtendedStyle & TAB2_EX_SHAREIMGLIST) == 0) {
		m_imgs.Destroy();
	}
}


template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_InitToolTip()
{
	m_bUseTheme = false;
	// comctl32のバージョンを取得
    HINSTANCE         hComCtl;
    HRESULT           hr;
    DLLGETVERSIONPROC pDllGetVersion;
	DLLVERSIONINFO    dvi = { sizeof(DLLVERSIONINFO) };

    hComCtl = LoadLibrary(TEXT("comctl32.dll"));
    if (hComCtl) {
        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hComCtl, "DllGetVersion");
        if (pDllGetVersion) {
            hr = (*pDllGetVersion)(&dvi);
            if (SUCCEEDED(hr) && dvi.dwMajorVersion >= 6) {
				m_bUseTheme = true;
			}
        }
    }
    FreeLibrary(hComCtl);


	// create a tool tip
	m_tip.Create(m_hWnd);
	ATLASSERT( m_tip.IsWindow() );
	CToolInfo tinfo(TTF_SUBCLASS, m_hWnd);			
	if (m_bUseTheme == false)
		tinfo.cbSize = sizeof(TOOLINFO) - sizeof(void*);
	tinfo.hwnd = m_hWnd;		// WTL always sucks...
	m_tip.AddTool(tinfo);
	m_tip.SetMaxTipWidth(SHRT_MAX);
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::_IsValidIndex(int nIndex) const
{
	size_t	size	= m_items.size();	// = GetItemCount();
	bool	rc		= ((size_t)nIndex < size);
	return rc;
}


template <class T, class TBase, class TWinTraits>
int		CTabCtrl2Impl<T, TBase, TWinTraits>::_GetRequiredHeight()
{
	size_t size = m_items.size();	// = GetItemCount();
	if (size == 0) {
		return GetItemHeight();
	} else {
		return m_items[size - 1].m_rcItem.bottom;
	}

	return 0;
}

template <class T, class TBase, class TWinTraits>
CRect	CTabCtrl2Impl<T, TBase, TWinTraits>::_MeasureItem(const CString &strText)
{
	if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE)
		return CRect(0, 0, m_sizeItem.cx, m_sizeItem.cy);

	// compute size of text - use DrawText with DT_CALCRECT
	int 	cx	   = MtlComputeWidthOfText(strText, m_font);

	int 	cxIcon = 0, cyIcon = 0;

	if (m_imgs.m_hImageList != NULL)
		m_imgs.GetIconSize(cxIcon, cyIcon);

	LOGFONT lf;
	m_font.GetLogFont(lf);
	int 	cy	   = lf.lfHeight;

	if (cy < 0)
		cy = -cy;

	cy += 2 * s_kcyTextMargin;

	// height of item is the bigger of these two
	cy	= std::max(cy, cyIcon);

	// width is width of text plus a bunch of stuff
	cx += 2 * s_kcxTextMargin;	// L/R margin for readability

	//		ATLTRACE(_T("(cx, cy) = (%d, %d)\n"), cx, cy);
	return CRect(0, 0, cx, cy);
}


template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_HotItem(int nNewHotIndex)
{
	TCTRACE( _T("_HotItem\n") );

	// clean up
	if ( _IsValidIndex(m_nHotIndex) ) {
		TCTRACE(_T(" clean up - %d\n"), m_nHotIndex);
		CTabCtrlItem &item = m_items[m_nHotIndex];

		if ( item.ModifyState(TCISTATE_HOT, 0) )
			InvalidateRect(item.m_rcItem);
	}

	m_nHotIndex = nNewHotIndex;

	if ( _IsValidIndex(m_nHotIndex) ) {
		TCTRACE(_T(" hot - %d\n"), m_nHotIndex);
		CTabCtrlItem &item = m_items[m_nHotIndex];

		if ( item.ModifyState(0, TCISTATE_HOT) )
			InvalidateRect(item.m_rcItem);
	}
}

template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_PressItem(int nPressedIndex)
{
	TCTRACE( _T("_PressItem\n") );

	// clean up prev
	if ( _IsValidIndex(m_nPressedIndex) ) {
		TCTRACE(_T(" clean up - %d\n"), m_nPressedIndex);
		CTabCtrlItem &item = m_items[m_nPressedIndex];

		if ( item.ModifyState(TCISTATE_PRESSED, 0) )
			InvalidateRect(item.m_rcItem);
	}

	m_nPressedIndex = nPressedIndex;

	if ( _IsValidIndex(m_nPressedIndex) ) {
		TCTRACE(_T(" press - %d\n"), m_nPressedIndex);
		CTabCtrlItem &item = m_items[m_nPressedIndex];

		if ( item.ModifyState(0, TCISTATE_PRESSED) )
			InvalidateRect(item.m_rcItem);
	}
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::_FindIndexFromCurMultiSelected(int nIndex)
{
	CSimpleArray<int> arrCurMultiSel;
	GetCurMultiSel(arrCurMultiSel, false);

	if (arrCurMultiSel.Find(nIndex) != -1)
		return true;

	return false;
}

template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_ResetMultiSelectedItems()
{
	for (int i = 0; i < (int) m_items.size(); ++i) {
		if ( m_items[i].ModifyState(TCISTATE_MSELECTED, 0) )
			InvalidateRect(m_items[i].m_rcItem);
	}
}


template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_ShowOrHideUpDownCtrl(const CRect &rcClient)
{
	TCTRACE( _T("_ShowOrHideUpDownCtrl\n") );

	int 	nCount = GetItemCount();	// = m_items.size();	//+++
	if ( (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) || nCount/*m_items.size()*/ < 1 ) {
		m_wndDropBtn.ShowWindow(SW_HIDE);
		m_wndUpDown.ShowWindow(SW_HIDE);
		return;
	}

	m_wndUpDown.SetRange( 0, nCount );
	ATLASSERT(nCount == GetItemCount());	//+++
	nCount = GetItemCount();				//+++

	if (m_nFirstIndexOnSingleLine != 0) {
		m_wndDropBtn.ShowWindow(SW_SHOWNORMAL);
		m_wndUpDown.ShowWindow(SW_SHOWNORMAL);
		return;
	} else if (m_items[nCount/*m_items.size()*/ - 1].m_rcItem.right > rcClient.right) {
		TCTRACE( _T(" show!\n") );
		m_wndDropBtn.ShowWindow(SW_SHOWNORMAL);
		m_wndUpDown.ShowWindow(SW_SHOWNORMAL);
		return;
	}

	m_wndDropBtn.ShowWindow(SW_HIDE);
	m_wndUpDown.ShowWindow(SW_HIDE);
}

template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_ScrollOpposite(int nNewSel, bool bClicked)
{
	ATLASSERT( _IsValidIndex(nNewSel) );

	if (GetItemCount() < 2
	 || m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE
	 || m_wndUpDown.IsWindowVisible() == false)
		return;

	const CRect &rcItem = m_items[nNewSel].m_rcItem;

	CRect		 rcClient;
	GetClientRect(rcClient);
	if (bClicked) {
		if (m_nFirstIndexOnSingleLine != nNewSel) {
			CRect rcUpDown;
			m_wndUpDown.GetWindowRect(rcUpDown);
			ScreenToClient(rcUpDown);

			if ( MtlIsCrossRect(rcItem, rcUpDown) )
				ScrollItem();
		}
	} else if (rcItem.IsRectEmpty() || (rcClient & rcItem) != rcItem) { // outside or crossed
		//+++ m_nFirstIndexOnSingleLine = MtlMax(0, nNewSel - 1);			//nNewSel;
		m_nFirstIndexOnSingleLine = std::max(0, nNewSel - 1);			//nNewSel;
		_UpdateSingleLineLayout(m_nFirstIndexOnSingleLine);
		Invalidate();
		_ShowOrHideUpDownCtrl(rcClient);
	}
}


template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_RefreshBandInfo()
{
	ATLASSERT(m_dwTabCtrl2ExtendedStyle & TAB2_EX_SUPPORTREBAR);

	if (m_bLockRefreshBandInfo)
		return;

	HWND		  hWndReBar = GetParent();
	CReBarCtrl	  rebar(hWndReBar);
	REBARBANDINFO rbBand;
	rbBand.cbSize = sizeof (REBARBANDINFO);
	rbBand.fMask  = RBBIM_CHILDSIZE;

	int 		  nIndex	= rebar.IdToIndex( GetDlgCtrlID() );
	rebar.GetBandInfo(nIndex, &rbBand);

	if ( rbBand.cyMinChild != _GetRequiredHeight() ) {
		// Calculate the size of the band
		rbBand.cxMinChild = 0;
		rbBand.cyMinChild = _GetRequiredHeight();

		rebar.SetBandInfo(nIndex, &rbBand);
	}
}

template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_UpdateSingleLineLayout(int nFirstIndex)
{
	TCTRACE(_T("_UpdateSingleLineLayout index(%d)\n"), nFirstIndex);
	ATLASSERT( _IsValidIndex(nFirstIndex) );
	m_arrSeparators.RemoveAll();

	// clean invisible items
	int i;

	for (i = 0; i < nFirstIndex && i < (int) m_items.size(); ++i) {
		m_items[i].m_rcItem.SetRectEmpty();
	}

	int cxOffset = 0;	//s_kcxGap;

	for (i = nFirstIndex; i < (int) m_items.size(); ++i) {
		CTabCtrlItem &item	= m_items[i];
		CRect		  rcSrc = item.m_rcItem;

		// update the rect
		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE) {
			item.m_rcItem = CRect(cxOffset, 0, cxOffset + m_sizeItem.cx, m_sizeItem.cy);
		} else {
			item.m_rcItem = _MeasureItem(m_items[i].m_strItem) + CPoint(cxOffset, 0);
		}

		cxOffset = cxOffset + item.m_rcItem.Width();

		m_arrSeparators.Add( CPoint(cxOffset, 0) );

		if (rcSrc != item.m_rcItem) {
			InvalidateRect( _InflateGapWidth(rcSrc) );
			InvalidateRect( _InflateGapWidth(item.m_rcItem) );
		}
	}
}

template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_UpdateMultiLineLayout(int nWidth)
{
	TCTRACE( _T("_UpdateMultiLineLayout\n") );

	m_arrSeparators.RemoveAll();

	int cxOffset = 0;
	int cyOffset = 0;

	for (int i = 0; i < (int) m_items.size(); ++i) {
		CTabCtrlItem &item	= m_items[i];
		CRect		  rcSrc = item.m_rcItem;

		// update the rect
		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE)
			item.m_rcItem = CRect(cxOffset, cyOffset, cxOffset + m_sizeItem.cx, cyOffset + m_sizeItem.cy);
		else
			item.m_rcItem = _MeasureItem(m_items[i].m_strItem) + CPoint(cxOffset, cyOffset);

		if (i != 0 && item.m_rcItem.right > nWidth) { // do wrap
			cxOffset  = 0;
			cyOffset += GetItemHeight() + s_kcyGap;

			if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE)
				item.m_rcItem = CRect(cxOffset, cyOffset, cxOffset + m_sizeItem.cx, cyOffset + m_sizeItem.cy);
			else
				item.m_rcItem = _MeasureItem(m_items[i].m_strItem) + CPoint(cxOffset, cyOffset);
		}

		cxOffset = cxOffset + m_items[i].m_rcItem.Width();

		m_arrSeparators.Add( CPoint(cxOffset, cyOffset) );

		if (rcSrc != item.m_rcItem) {
			InvalidateRect( _InflateGapWidth(rcSrc) );
			InvalidateRect( _InflateGapWidth(item.m_rcItem) );
		}
	}
}

template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_UpdateLayout()
{
	if (m_items.size() == 0) {
		m_arrSeparators.RemoveAll();
		Invalidate();

		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_SUPPORTREBAR)
			_RefreshBandInfo();

		return;
	}

	CRect		rc;
	GetClientRect(&rc);

	if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) {
		_UpdateMultiLineLayout( rc.Width() );
	} else {
		_UpdateSingleLineLayout(m_nFirstIndexOnSingleLine);
	}

	_ShowOrHideUpDownCtrl(rc);

	if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_SUPPORTREBAR)
		_RefreshBandInfo();

	UpdateWindow();
}


template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_DoPaint(CDCHandle dc, LPCRECT lpRect)
{
	HWND	hWnd	= GetParent();
	CPoint	pt(0, 0);
	MapWindowPoints(hWnd, &pt, 1);
	pt = ::OffsetWindowOrgEx( (HDC)dc.m_hDC, pt.x, pt.y, NULL );
	LRESULT lResult = ::SendMessage(hWnd, WM_ERASEBKGND, (WPARAM)dc.m_hDC, 0L);
	::SetWindowOrgEx((HDC)dc.m_hDC, 0, 0, NULL);


	CFontHandle fontOld = dc.SelectFont(m_font);
	int 		modeOld = dc.SetBkMode(TRANSPARENT);

	int	i;
	if (GetTabCtrl2ExtendedStyle() & TAB2_EX_MULTILINE)
		i = 0;
	else {
		i = m_nFirstIndexOnSingleLine;
	}
	bool	bAnchorColor = (GetTabCtrl2ExtendedStyle() & TAB2_EX_ANCHORCOLOR) != 0;
	for (; i < GetItemCount(); ++i) {
		if ( lpRect == NULL || MtlIsCrossRect(m_items[i].m_rcItem, lpRect) ) {
			// 各タブを描写する
			m_pTabSkin->Update(dc, m_imgs, GetItem(i), bAnchorColor);
		}
	}

	dc.SelectFont(fontOld);
	dc.SetBkMode(modeOld);
}

//       ↓ここの分プラスする
// | tab |
template <class T, class TBase, class TWinTraits>
const CRect	CTabCtrl2Impl<T, TBase, TWinTraits>::_InflateGapWidth(const CRect &rc) const
{
	int cxSeparatorOffset = s_kcxGap * 2 + s_kcxSeparator;

	return CRect(rc.left, rc.top, rc.right + cxSeparatorOffset, rc.bottom);
}


// Attributes

template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::SetItemSize(const CSize &size)
{
	CSize sizePrev = m_sizeItem;

	m_sizeItem = size;

	if (sizePrev != m_sizeItem
		&& m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE)
	{
		_UpdateLayout();
	}
}

template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::ModifyTabCtrl2ExtendedStyle(DWORD dwRemove, DWORD dwAdd)
{
	DWORD dwOldStyle = m_dwTabCtrl2ExtendedStyle;

	m_dwTabCtrl2ExtendedStyle = (m_dwTabCtrl2ExtendedStyle & ~dwRemove) | dwAdd;

	if (dwOldStyle != m_dwTabCtrl2ExtendedStyle) {
		m_nFirstIndexOnSingleLine = 0;
		_UpdateLayout();
	}

	if ( (dwOldStyle & TAB2_EX_ANCHORCOLOR) != (m_dwTabCtrl2ExtendedStyle & TAB2_EX_ANCHORCOLOR) )
		Invalidate();
}

template <class T, class TBase, class TWinTraits>
int		CTabCtrl2Impl<T, TBase, TWinTraits>::GetCurSel()
{
	for (int i = 0; i < (int) m_items.size(); ++i) {
		if (m_items[i].m_fsState & TCISTATE_SELECTED)
			return i;
	}

	return -1;
}

template <class T, class TBase, class TWinTraits>
int		CTabCtrl2Impl<T, TBase, TWinTraits>::HitTest(const CPoint& point)
{
	CRect	rc;
	GetClientRect(&rc);
	if ( !rc.PtInRect(point) )
		return -1;

	int   i = (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) ? 0 : m_nFirstIndexOnSingleLine;

	for (; i < GetItemCount(); ++i) {
		if (m_items[i].m_rcItem.PtInRect(point)/* && m_items[i].m_fsState & TCISTATE_ENABLED*/) {
			return i;
		}
	}

	return -1;
}

template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::InsertItem(int nIndex, const CTabCtrlItem &item)
{		// if nDestIndex is invalid, added to tail
	_HotItem();

	#if 1 //+++
	int size = int (m_items.size());	// = GetItemCount();
	if (size == 0) {
		m_items.push_back(item);
	} else {
		if ( nIndex < 0 || nIndex > size ) {
			nIndex = size;
		}
		m_items.insert(m_items.begin() + nIndex, item);
	}
	//int size2 = int(m_items.size());	//*+++ debug
	#else
	if (m_items.size() == 0) {
		m_items.push_back(item);
	} else {
		if ( nIndex < 0 || nIndex > (int) m_items.size() ) {
			nIndex = (int) m_items.size();
		}

		m_items.insert(m_items.begin() + nIndex, item);
	}
	#endif

	_UpdateLayout();

	return true;
}


template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::DeleteItems(CSimpleArray<int> &arrSrcs)
{
	for (int i = arrSrcs.GetSize() - 1; i >= 0; --i) {
		int nIndex = arrSrcs[i];

		if ( !_IsValidIndex(nIndex) )
			continue;

		DeleteItem(nIndex);
	}
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::MoveItems(int nDestIndex, CSimpleArray<int> &arrSrcs)
{
	// arrSrcs has to be sorted! if nDestIndex is invalid, added to tail
	TCTRACE( _T("CTabCtrlImpl::MoveItems\n") );

	if (arrSrcs.GetSize() <= 0)
		return false;

	int 					  i = 0;

	if (arrSrcs.GetSize() == 1) {
		if (nDestIndex == arrSrcs[0] || nDestIndex == arrSrcs[0] + 1)
			return true;		// no need

		if (!_IsValidIndex(nDestIndex) && arrSrcs[0] == m_items.size() - 1)
			return true;		// no need
	}

	CLockRedraw 			  lock(m_hWnd);
	m_bLockRefreshBandInfo = true;

	std::vector<CTabCtrlItem> temp;

	TCTRACE( _T(" save src indexs - ") );

	for (i = 0; i < arrSrcs.GetSize(); ++i) {
		int nIndex = arrSrcs[i];

		if ( !_IsValidIndex(nIndex) )
			continue;

		TCTRACE(_T(" %d"), nIndex);
		temp.push_back(m_items[nIndex]);
	}

	TCTRACE( _T("\n") );

	TCTRACE( _T(" delete item indexs - ") );

	for (i = arrSrcs.GetSize() - 1; i >= 0; --i) {
		int nIndex = arrSrcs[i];

		if ( !_IsValidIndex(nIndex) )
			continue;

		if (nDestIndex > nIndex)
			--nDestIndex;

		TCTRACE(_T(" %d"), nIndex);
		DeleteItem(nIndex);
	}

	TCTRACE( _T("\n") );

	// add all
	if ( !_IsValidIndex(nDestIndex) ) {
		nDestIndex = GetItemCount();
	}

	for (i = 0; i < (int) temp.size(); ++i) {
		InsertItem(nDestIndex, temp[i]);
		++nDestIndex;
	}

	m_bLockRefreshBandInfo = false;
	_RefreshBandInfo();

	return true;
}

template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::SetItemText(int nIndex, const CString &str)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	if (m_items[nIndex].m_strItem == str)
		return true;

	m_items[nIndex].m_strItem = str;

	if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE) {
		InvalidateRect(m_items[nIndex].m_rcItem);
		UpdateWindow();
	} else {
		InvalidateRect(m_items[nIndex].m_rcItem);	// even if layout will not be changed
		_UpdateLayout();							//_UpdateItems(nIndex);
	}

	return true;
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::SetItemImageIndex(int nIndex, int nImgIndex)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	if (m_items[nIndex].m_nImgIndex == nImgIndex)
		return true;

	m_items[nIndex].m_nImgIndex = nImgIndex;

	// imagelist does'nt change item rect
	InvalidateRect(m_items[nIndex].m_rcItem);
	UpdateWindow();
	return true;
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::SetItemInactive(int nIndex)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	if ( m_items[nIndex].ModifyState(0, TCISTATE_INACTIVE) ) {
		InvalidateRect(m_items[nIndex].m_rcItem, FALSE);
		return true;
	}

	return false;
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::SetItemActive(int nIndex)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	if ( m_items[nIndex].ModifyState(TCISTATE_INACTIVE, 0) ) {
		InvalidateRect(m_items[nIndex].m_rcItem, FALSE);
		return true;
	}

	return false;
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::SetItemDisabled(int nIndex)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	if ( m_items[nIndex].ModifyState(TCISTATE_ENABLED, 0) ) {
		InvalidateRect(m_items[nIndex].m_rcItem);
		return true;
	}

	return false;
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::SetItemEnabled(int nIndex)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	if ( m_items[nIndex].ModifyState(0, TCISTATE_ENABLED) ) {
		InvalidateRect(m_items[nIndex].m_rcItem);
		return true;
	}

	return false;
}

template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::SetItemUserData(int nIndex, DWORD_PTR dwUser)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	m_items[nIndex].m_dwUser = m_dwUser;
	return true;
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::GetItem(int nIndex, CTabCtrlItem &item)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	item = m_items[nIndex];
	return true;
}

template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::GetItemText(int nIndex, CString &str)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	str = m_items[nIndex].m_strItem;
	return true;
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::GetItemImageIndex(int nIndex, int &nImgIndex)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	nImgIndex = m_items[nIndex].m_nImgIndex;
	return true;
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::GetItemUserData(int nIndex, DWORD_PTR &dwData)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	dwData = m_items[nIndex].m_dwUser;
	return true;
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::GetItemState(int nIndex, BYTE &bytData)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	bytData = m_items[nIndex].m_fsState;
	return true;
}

template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::GetItemRect(int nIndex, CRect &rc)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	if (!(m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) && nIndex < m_nFirstIndexOnSingleLine)
		return false;

	rc = m_items[nIndex].m_rcItem;
	return true;
}

template <class T, class TBase, class TWinTraits>
int		CTabCtrl2Impl<T, TBase, TWinTraits>::GetFirstVisibleIndex()
{
	if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) {
		return 0;
	} else {
		CRect rc;
		GetClientRect(&rc);

		for (int i = 0; i < (int) m_items.size(); ++i) {
			if ( MtlIsCrossRect(m_items[i].m_rcItem, &rc) ) {
				return i;
			}
		}
	}

	return -1;
}


template <class T, class TBase, class TWinTraits>
int		CTabCtrl2Impl<T, TBase, TWinTraits>::GetLastVisibleIndex()
{
	if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) {
		return m_items.size() - 1;
	} else {
		CRect rc;
		GetClientRect(&rc);

		for (int i = int(m_items.size()) - 1; i >= 0; i--) {
			if ( MtlIsCrossRect(m_items[i].m_rcItem, &rc) ) {
				return i;
			}
		}
	}

	return -1;
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::AddItem(const CTabCtrlItem &item)
{
	_HotItem();

	m_items.push_back(item);
	_UpdateLayout();	// _UpdateItems(m_items.size() - 1);
	return true;
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::DeleteItem(int nIndex)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	_HotItem();

	// first, post it
	InvalidateRect( _InflateGapWidth(m_items[nIndex].m_rcItem) );
	//		_InvalidateItemsRect(nIndex, m_items.size());

	if ( nIndex < m_nFirstIndexOnSingleLine || nIndex + 1 == GetItemCount() ) {
		--m_nFirstIndexOnSingleLine;

		if (m_nFirstIndexOnSingleLine < 0)
			m_nFirstIndexOnSingleLine = 0;
	}

	m_items.erase(m_items.begin() + nIndex);
	_UpdateLayout();
	return true;
}

// 現在(複数)選択されているタブのインデックスを取得
// bIncludeCurSel == trueで現在アクティブなタブを含める
template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::GetCurMultiSel(CSimpleArray<int> &arrDest, bool bIncludeCurSel = true)
{						// have to be sorted!
	arrDest.RemoveAll();

	for (int i = 0; i < GetItemCount(); ++i) {
		if (bIncludeCurSel) {
			if (m_items[i].m_fsState & TCISTATE_SELECTED) {
				arrDest.Add(i);
				continue;
			}
		}

		if (m_items[i].m_fsState & TCISTATE_MSELECTED)
			arrDest.Add(i);
	}
}

template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::GetCurMultiSelEx(CSimpleArray<int>& arrDest, int nIndex)
{
	GetCurMultiSel(arrDest);

	if (arrDest.Find(nIndex) == -1) {
		// 複数選択されているタブ以外がクリックされた
		arrDest.RemoveAll();
		arrDest.Add(nIndex);
	} else if (nIndex != GetCurSel()) {
		// 複数選択されているタブだけにする
		arrDest.Remove(GetCurSel());
	}
}

template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::SetCurSel(int nIndex, bool bClicked = false)
{
	TCTRACE( _T("SetCurSel\n") );

	if ( !_IsValidIndex(nIndex) ) {
		TCTRACE( _T(" invalid index\n") );
		return false;
	}

	_ResetMultiSelectedItems();
	int nCurSel = GetCurSel();

	if (nCurSel != -1 && nCurSel == nIndex) {
		TCTRACE( _T(" no need\n") );
		return true;
	}

	// clean up
	if ( _IsValidIndex(nCurSel) ) {
		if ( m_items[nCurSel].ModifyState(TCISTATE_SELECTED | TCISTATE_HOT | TCISTATE_PRESSED | TCISTATE_MSELECTED, 0) )
			InvalidateRect(m_items[nCurSel].m_rcItem);
	}

	// new selected item
	if ( m_items[nIndex].ModifyState(TCISTATE_HOT | TCISTATE_PRESSED | TCISTATE_MSELECTED, TCISTATE_SELECTED) )
		InvalidateRect(m_items[nIndex].m_rcItem);

	_ScrollOpposite(nIndex, bClicked);

	T * pT		= static_cast<T *>(this);
	pT->OnSetCurSel(nIndex);

	UpdateWindow();
	return true;
}


template <class T, class TBase, class TWinTraits>
int		CTabCtrl2Impl<T, TBase, TWinTraits>::GetItemHeight() const
{
	if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE) {
		return m_sizeItem.cy;
	} else {
		int 	cxIcon = 0, cyIcon = 0;

		if (m_imgs.m_hImageList != NULL)
			m_imgs.GetIconSize(cxIcon, cyIcon);

		LOGFONT lf;
		m_font.GetLogFont(lf);
		int 	cy	   = lf.lfHeight;

		if (cy < 0)
			cy = -cy;

		cy += 2 * s_kcyTextMargin;
		return std::max(cy, cxIcon);
	}
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::CanScrollItem(bool bRight = true) const
{
	if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE)
		return false;			// can't
	if (bRight) {
		int 	nCount = GetItemCount();
		return	m_nFirstIndexOnSingleLine + 1 < nCount;
	} else {
		return	m_nFirstIndexOnSingleLine - 1 >= 0;
	}
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::ScrollItem(bool bRight = true)
{
	if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE)
		return false;		// do nothing

	if (bRight) {
		int 	nCount = GetItemCount();
		if ( m_nFirstIndexOnSingleLine + 1 < nCount ) {
			++m_nFirstIndexOnSingleLine;
			_UpdateSingleLineLayout(m_nFirstIndexOnSingleLine);
			Invalidate();
		} else {
			return false;
		}
	} else {
		if (m_nFirstIndexOnSingleLine - 1 >= 0) {
			--m_nFirstIndexOnSingleLine;
			_UpdateSingleLineLayout(m_nFirstIndexOnSingleLine);
			Invalidate();
		} else {
			return false;
		}
	}

	CRect	rcClient;
	GetClientRect(&rcClient);

	_ShowOrHideUpDownCtrl(rcClient);
	return true;
}


// Overrides
template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::OnTrackMouseMove(UINT nFlags, CPoint pt)
{
	int nIndex = HitTest(pt);

	if ( _IsValidIndex(nIndex) ) {
		if (m_nHotIndex == -1 || m_nHotIndex != nIndex) {							// if other button made hot or first hot
			m_tip.Activate(FALSE);
			m_tip.Activate(TRUE);
			PostMessage( WM_MOUSEMOVE, (WPARAM) nFlags, MAKELPARAM(pt.x, pt.y) );	// make sure

			_HotItem(nIndex);														// button made hot with mouse
		}
	} else {
		m_tip.Activate(FALSE);
		_HotItem();
	}
}


template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::OnTrackMouseLeave()
{
	m_tip.SetDelayTime(TTDT_INITIAL, 500);
	_HotItem();
}

template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::DoPaint(CDCHandle dc)
{
	_DoPaint(dc);
}


template <class T, class TBase, class TWinTraits>
LRESULT	CTabCtrl2Impl<T, TBase, TWinTraits>::OnPushDropButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{
	T *pT = static_cast<T *>(this);

	pT->OnDropDownButton();
	return 0;
}


template <class T, class TBase, class TWinTraits>
LRESULT	CTabCtrl2Impl<T, TBase, TWinTraits>::OnDeltaPos(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMUPDOWN lpnmud = (LPNMUPDOWN) pnmh;

	TCTRACE(_T("OnDeltaPos iPos(%d), iDelta(%d)\n"), lpnmud->iPos, lpnmud->iDelta);
	ScrollItem(lpnmud->iDelta > 0);

	return 0;
}


template <class T, class TBase, class TWinTraits>
LRESULT	CTabCtrl2Impl<T, TBase, TWinTraits>::OnGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO) pnmh;

	TCTRACE( _T("OnGetDispInfo\n") );
	CPoint		   ptCursor;
	::GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);
	int 		   nIndex	 = HitTest(ptCursor);
	if ( _IsValidIndex(nIndex) ) {
		T * 	pT		   = static_cast<T *>(this);
		m_strToolTip = pT->OnGetToolTipText(nIndex);
		//strToolTip = MtlCompactString( strToolTip, _countof(pDispInfo->szText) );
		//::lstrcpyn( pDispInfo->szText, strToolTip, _countof(pDispInfo->szText) );
		pDispInfo->lpszText = m_strToolTip.GetBuffer(0);
	} else {
		pDispInfo->lpszText = NULL;
		//pDispInfo->szText[0] = _T('\0');
	}

	m_tip.SetDelayTime( TTDT_INITIAL, m_tip.GetDelayTime(TTDT_RESHOW) );

	return 0;
}


template <class T, class TBase, class TWinTraits>
LRESULT	CTabCtrl2Impl<T, TBase, TWinTraits>::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	// maybe nothing done
	LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);

	// get and use system settings
	WTL::CLogFont	lf;
	lf.SetMenuFont();
	m_font.CreateFontIndirect(&lf);

	m_wndUpDown.Create(m_hWnd, CRect(0, 0, s_kcxUpDown, s_kcyUpDown), NULL, WS_CHILD | WS_OVERLAPPED | UDS_HORZ);
	m_wndDropBtn.Create(m_hWnd, CRect(0, 0, s_kcxUpDown, s_kcxUpDown), COMMAND_ID_DROPBUTTON);
	m_wndDropBtn.SetWindowPos(m_wndUpDown.m_hWnd, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	_InitToolTip();

	// default size
	MoveWindow(0, 0, 200, _GetRequiredHeight(), FALSE);
	return lRet;
}


template <class T, class TBase, class TWinTraits>
LRESULT	CTabCtrl2Impl<T, TBase, TWinTraits>::OnWindowPosChanging(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	LPWINDOWPOS lpwndpos = (LPWINDOWPOS) lParam;

	{
		CSize size(lpwndpos->cx, lpwndpos->cy);
		m_wndUpDown.MoveWindow(size.cx - s_kcyUpDown * 3, size.cy - s_kcyUpDown, s_kcxUpDown, s_kcyUpDown);
		m_wndDropBtn.MoveWindow(size.cx - s_kcyUpDown, size.cy - s_kcyUpDown, s_kcyUpDown, s_kcyUpDown);
		//m_wndUpDown.MoveWindow(size.cx - s_kcxUpDown, size.cy - s_kcyUpDown, s_kcxUpDown, s_kcyUpDown);
		CRect rc(0, 0, s_kcyUpDown, s_kcyUpDown);
		m_wndDropBtn.InvalidateRect(&rc, TRUE);
		_ShowOrHideUpDownCtrl( CRect(0, 0, size.cx, size.cy) );

		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) {
			_UpdateMultiLineLayout(size.cx);

			if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_SUPPORTREBAR) {
				_RefreshBandInfo();
			}
		}
	}
	return 0;
}


template <class T, class TBase, class TWinTraits>
bool	CTabCtrl2Impl<T, TBase, TWinTraits>::_MustBeInvalidateOnMultiLine(CSize size)
{
	CRect rc;  GetClientRect(rc);		//*+++

	if (rc.Width() < size.cx)	// expanding
		return true;

	int   cxLeft = 0;

	for (int i = 0; i < m_items.size(); ++i) {
		cxLeft = max(m_items[i].m_rcItem.right, cxLeft);
	}

	if (cxLeft != 0 && cxLeft < size.cx) {
		return false;
	} else {
		return true;
	}
}


template <class T, class TBase, class TWinTraits>
LRESULT	CTabCtrl2Impl<T, TBase, TWinTraits>::OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &bHandled)
{
	if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_TRANSPARENT) {
		bHandled = FALSE;
		return 0;
	}

	RECT rect;
	GetClientRect(&rect);
	::FillRect( (HDC) wParam, &rect, (HBRUSH) LongToPtr(COLOR_BTNFACE + 1) );

	return 1;					// don't do the default erase
}

template <class T, class TBase, class TWinTraits>
LRESULT	CTabCtrl2Impl<T, TBase, TWinTraits>::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	TCTRACE( _T("OnLButtonDown\n") );
	POINT pt	 = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	int   nIndex = HitTest(pt);

	if (nIndex != -1) {
		ATLASSERT( _IsValidIndex(nIndex) );

		if ( (wParam & MK_CONTROL) && GetCurSel() != nIndex ) {
			if ( !_FindIndexFromCurMultiSelected(nIndex) ) {
				if ( m_items[nIndex].ModifyState(TCISTATE_SELECTED, TCISTATE_MSELECTED) )
					InvalidateRect(m_items[nIndex].m_rcItem);
			} else {
				if ( m_items[nIndex].ModifyState(TCISTATE_MSELECTED, 0) )
					InvalidateRect(m_items[nIndex].m_rcItem);
			}
		} else {
			_PressItem(nIndex);
			SetCapture();
		}
	}

	return 0;
}

template <class T, class TBase, class TWinTraits>
LRESULT	CTabCtrl2Impl<T, TBase, TWinTraits>::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	TCTRACE( _T("OnLButtonUp\n") );
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if (GetCapture() == m_hWnd) {
		ReleaseCapture();

		int nIndex = HitTest(pt);

		if (nIndex != -1 && nIndex == m_nPressedIndex) {
			TCTRACE( _T(" change current selelected item\n") );
			ATLASSERT( _IsValidIndex(nIndex) );
			_PressItem();	// always clean up pressed flag
			SetCurSel(nIndex, true);
			NMHDR nmhdr = { m_hWnd, GetDlgCtrlID(), TCN_SELCHANGE };
			::SendMessage(GetParent(), WM_NOTIFY, (WPARAM) GetDlgCtrlID(), (LPARAM) &nmhdr);
		} else {
			_PressItem();	// always clean up pressed flag
		}
	}

	return 0;
}


// Implementation


template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_DrawSeparators(CDCHandle dc, LPCRECT lpRect)
{
	int   cy = GetItemHeight();
	CRect rect(lpRect);

	for (int i = 0; i < m_arrSeparators.GetSize(); ++i) {
		CPoint &pt = m_arrSeparators[i];
		CRect	rc(pt.x, pt.y + 2, pt.x + 2, pt.y + cy - 2);

		if ( lpRect == NULL || MtlIsCrossRect(rc, lpRect) ) {
			dc.DrawEdge(rc, EDGE_ETCHED, BF_LEFT);	// draw separator line
		}
	}
}




template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_SetDrawStyle(int nStyle)
{
	m_nDrawStyle = nStyle;

	m_wndDropBtn.SetDrawStyle(nStyle);
	#if 1	//+++ uxtheme.dll の関数の呼び出し方を変更.
	if (nStyle == SKN_TAB_STYLE_CLASSIC) {
		UxTheme_Wrap::SetWindowTheme(m_wndUpDown.m_hWnd, L" ", L" ");
	} else {
		UxTheme_Wrap::SetWindowTheme(m_wndUpDown.m_hWnd, NULL, L"SPIN");
	}

	#else
	CTheme		theme;
	theme.Attach(m_hTheme);
	if (nStyle == SKN_TAB_STYLE_CLASSIC) {
		theme.SetWindowTheme(m_wndUpDown.m_hWnd, L" ", L" ");
	} else {
		theme.SetWindowTheme(m_wndUpDown.m_hWnd, NULL, L"SPIN");
	}
	#endif
}


template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::_ReloadSkinData()
{
	if (::PathFileExists(_GetSkinDir()) == FALSE) {
		m_nDrawStyle = SKN_TAB_STYLE_CLASSIC;
	}

	switch (m_nDrawStyle)
	{
	case SKN_TAB_STYLE_DEFAULT:
		m_pTabSkin.reset(new CTabSkinDefault);
		break;

	case SKN_TAB_STYLE_THEME:
		m_pTabSkin.reset(new CTabSkinTheme(static_cast<CTheme&>(*this)));
		break;

	case SKN_TAB_STYLE_CLASSIC:
		m_pTabSkin.reset(new CTabSkinClassic(m_arrSeparators));
		break;

	default:
		m_pTabSkin.reset(new CTabSkinClassic(m_arrSeparators));
		ATLASSERT(FALSE);
	}

	InvalidateRect(NULL, TRUE);
}

template <class T, class TBase, class TWinTraits>
void	CTabCtrl2Impl<T, TBase, TWinTraits>::ReloadSkin(int nStyle)
{
	CString strTabBmp = _GetSkinDir() + _T("Tab.bmp");

	_ReplaceImageList(strTabBmp, m_imgs, IDB_MDITAB);
	_SetDrawStyle(nStyle);
	_ReloadSkinData();
}


//////////////////////////////////////////////////////////////////


class CTabCtrl2 : public CTabCtrl2Impl<CTabCtrl2> {
public:
	DECLARE_WND_CLASS_EX(_T("MTL_TabCtrl2"), CS_DBLCLKS, COLOR_BTNFACE)
};






////////////////////////////////////////////////////////////////////////////



}	//namespace MTL



#ifndef _MTL_NO_AUTOMATIC_NAMESPACE
using namespace MTL;
#endif	//!_MTL_NO_AUTOMATIC_NAMESPACE
