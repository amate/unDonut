/**
 *	@file	DonutTabBar.cpp
 */

#include "stdafx.h"
#include "DonutTabBar.h"
#include "MtlProfile.h"
#include "MtlBrowser.h"
#include "ParseInternetShortcutFile.h"	//+++ for ParseInternetShortcutFile()
#include "Donut.h"
#include "option/MenuDialog.h"
#include "option/RightClickMenuDialog.h"
#include "DonutAddressBar.h"

#include "HlinkDataObject.h"
#include "dialog/CommandSelectDialog.h"
#include "FaviconManager.h"

#include "option/MDITabDialog.h"
#include "DropDownButton.h"

//////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////
// CTabCtrlItem : タブ１個の情報

struct TabItem 
{
	// Data members
	DWORD		state;
	CString 	strItem;		// string on button
	CRect		rcItem;
	int 		nImgIndex;	// image list index
	int			nFaviconIndex;
	HWND		hWnd;
	
	// Constructor
	TabItem() : state(TISS_NORMAL), nImgIndex(-1), nFaviconIndex(-1), hWnd(NULL)
	{	}


	// Attributes
	bool ModifyState(DWORD Remove, DWORD Add)
	{
		DWORD stateOld = state;

		state = (state & ~Remove) | Add;
		if ( (stateOld & TISS_SELECTED || stateOld & TISS_MSELECTED)
		   && (Remove == TISS_HOT || Add == TISS_HOT) ) 	// selected, no need to update
			return false;
		else
			return state != stateOld;
	}

};


/////////////////////////////////////////////////////////////////////////////
// CTabSkin

class CTabSkin
{
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

	void	Update(CDCHandle dc, HIMAGELIST hImgList, HIMAGELIST hImgFavicon, const TabItem& item);

private:
	void	_LoadTabTextSetting();
	void	_LoadImage();

	void	_DrawText(CDCHandle dc, CPoint ptOffset, const TabItem& item, bool bAnchorColor);

	// Overridable
	virtual void  _DrawSkinCur(CDCHandle dc, CRect rcItem) = 0;		// Active
	virtual void  _DrawSkinNone(CDCHandle dc, CRect rcItem) = 0;	// Normal
	virtual void  _DrawSkinSel(CDCHandle dc, CRect rcItem) = 0;		// Hot
};

void	CTabSkin::Update(CDCHandle dc, HIMAGELIST hImgList, HIMAGELIST hImgFavicon, const TabItem& item)
{
	int	cxIcon			= 0;
	int cyIcon			= 0;
	int	cxIconOffset	= 2;

	enum { 
		cxTabIcon = 4, cyTabIcon = 1, cxIconText = 2, faviconSize = 16, 
		LoadIconSize = 6, cxTabLoadIcon = 4, cyTabText = 3,
	};
	//if (item.m_nImgIndex != -1) {
	//	::ImageList_GetIconSize(hImgList, &cxIcon, &cyIcon);
	//	cxIconOffset = 20;//cxIcon + s_kcxIconGap + 3;
	//} else {
	if (item.nImgIndex != -1 || CTabBarOption::s_bShowFavicon)
		cxIconOffset = cxTabIcon + 16 + cxIconText;
	//}

	bool	bHot		= (item.state & TISS_HOT) != 0;
	bool	bPressed	= (item.state & TISS_PRESSED) != 0;

	CPoint	ptOffset(0, 0);

	if (item.state & TISS_SELECTED) {
		// 選択された
		_DrawSkinCur(dc, item.rcItem);
//		Update_Selected(dc, pTabSkin, bHot, bPressed);
		ptOffset += CTabBarOption::s_ptOffsetActive;
	} else if (item.state & TISS_MSELECTED) {
		// 複数選択された
		_DrawSkinSel(dc, item.rcItem);
//		Update_MultiSel(dc, pTabSkin, bHot, bPressed);
		ptOffset += CTabBarOption::s_ptOffsetMSelect;
	} else {
		// 選択されていない
		if (bHot == true && bPressed == false) {
			_DrawSkinSel(dc, item.rcItem);
		} else {
			_DrawSkinNone(dc, item.rcItem);
		}
//		Update_NotSel(dc, pTabSkin, bHot, bPressed, ptOffset);
		ptOffset += CTabBarOption::s_ptOffsetNormal;
	}

	_DrawText(dc, ptOffset + CPoint(cxIconOffset, cyTabText), item, CTabBarOption::s_bAnchorColor);

	if (item.nImgIndex != -1) {
		::ImageList_Draw(hImgList, item.nImgIndex, dc, 
			item.rcItem.left + ptOffset.x + s_kcxIconGap + cxTabLoadIcon,
			item.rcItem.top + ((item.rcItem.Height() - LoadIconSize) / 2) + (ptOffset.y / 2) + cyTabIcon, ILD_TRANSPARENT);
	} else if (CTabBarOption::s_bShowFavicon) {
		int nFaviconIndex = (item.nFaviconIndex != -1) ? item.nFaviconIndex : 0;
		::ImageList_Draw(hImgFavicon, nFaviconIndex, dc,
			item.rcItem.left + ptOffset.x + cxTabIcon,
			item.rcItem.top + ((item.rcItem.Height() - faviconSize) / 2) + (ptOffset.y / 2) + cyTabIcon, ILD_TRANSPARENT);
	}

#if 0
	if (item.m_fsState & TCISTATE_NAVIGATELOCK) {
		CPoint	pt(item.rcItem.left + ptOffset.x + s_kcxIconGap + 6
				 , item.rcItem.top + ((item.rcItem.Height() - cyIcon) / 2) + (ptOffset.y / 2) + 4);
		m_imgLock.Draw(dc, 0, pt, ILD_TRANSPARENT);
	}
#endif
}


static int _Pack(int hi, int low)
{
	if ( !( ( ('0' <= low && low <= '9') || ('A' <= low && low <= 'F') || ('a' <= low && low <= 'f') )
		  && ( ('0' <= hi && hi  <= '9') || ('A' <= hi	&& hi  <= 'F') || ('a' <= hi  && hi  <= 'f') ) ) )
		return 0;	//数値ではない

	int nlow = ('0' <= low && low <= '9') ? low - '0'
			 : ('A' <= low && low <= 'F') ? low - 'A' + 0xA
			 :								low - 'a' + 0xA ;
	int nhi  = ('0' <= hi  && hi  <= '9') ? hi	- '0'
			 : ('A' <= hi  && hi  <= 'F') ? hi	- 'A' + 0xA
			 :								hi	- 'a' + 0xA ;

	return (nhi << 4) + nlow;
}

static BOOL _QueryColorString(CIniFileI &pr, COLORREF &col, LPCTSTR lpstrKey)
{
	CString strCol = pr.GetString(lpstrKey, NULL, 20);
	//+++ 元々 pr.QueryValue(文字列)でのエラーは、長さが0の時のことなので、文字列が空かどうかのチェックだけで十分.
	if ( strCol.IsEmpty() )
		return FALSE;

	strCol.TrimLeft('#');

	col = RGB( _Pack(strCol[0], strCol[1]) ,
			   _Pack(strCol[2], strCol[3]) ,
			   _Pack(strCol[4], strCol[5])
			 );

	return TRUE;
}


void	CTabSkin::_LoadTabTextSetting()
{
	CString	strTabSkinIni = Misc::GetExeDirectory() + _T("TabSkin.ini");
	// ファイルの存在チェック
	if (::PathFileExists(strTabSkinIni) == FALSE) {
		strTabSkinIni = _GetSkinDir() + _T("TabSkin.ini");
	}

	m_colText		= ::GetSysColor(COLOR_BTNTEXT);
	m_colActive		= RGB( 128, 0, 128 );
	m_colInActive	= RGB(   0, 0, 255 );
	m_colDisable	= ::GetSysColor(COLOR_3DSHADOW);
	m_colDisableHi	= ::GetSysColor(COLOR_3DHILIGHT);
	m_colBtnFace	= ::GetSysColor(COLOR_BTNFACE);
	m_colBtnHi		= ::GetSysColor(COLOR_BTNHILIGHT);

	// .iniから読み込み
	CIniFileI pr( strTabSkinIni, _T("Color") );
	_QueryColorString( pr, m_colText		, _T("Text") );
	_QueryColorString( pr, m_colActive		, _T("Active") );
	_QueryColorString( pr, m_colInActive	, _T("InActive") );
	_QueryColorString( pr, m_colDisable		, _T("Disable") );
	_QueryColorString( pr, m_colDisableHi	, _T("DisableHilight") );
	_QueryColorString( pr, m_colBtnFace 	, _T("ButtonFace") );
	_QueryColorString( pr, m_colBtnHi		, _T("ButtonHilight") );
}

void	CTabSkin::_LoadImage()
{
	m_imgLock.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);

	CString	strTabSkinDir = _GetSkinDir();

//	CString	strNavigateLock = strTabSkinDir;
//	strNavigateLock += _T("NavigateLock.ico");
//	ATLASSERT(FALSE);
//	HICON hIcon = AtlLoadIconImage(strNavigateLock.GetBuffer(0), LR_LOADFROMFILE/* | LR_LOADTRANSPARENT*/, CXICON, CYICON);
	HICON hIcon = AtlLoadIcon(IDI_NAVIGATELOCK);
	m_imgLock.AddIcon(hIcon);
	::DestroyIcon(hIcon);
}


void	CTabSkin::_DrawText(CDCHandle dc, CPoint ptOffset, const TabItem& item, bool bAnchorColor)
{
	COLORREF	clr;

	if ( item.state & TISS_NAVIGATELOCK )
		clr = m_colDisable;
	else if (item.state & TISS_INACTIVE)
		clr = bAnchorColor ? m_colInActive	: m_colText;
	else
		clr = bAnchorColor ? m_colActive	: m_colText;

	COLORREF	clrOld = dc.SetTextColor(clr);

	CRect		rcBtn(item.rcItem.left + ptOffset.x, item.rcItem.top + ptOffset.y, 
					  item.rcItem.right, item.rcItem.bottom);
	rcBtn.DeflateRect(2, 0);

	UINT	uFormat;
	int		nWidth	= MtlComputeWidthOfText(item.strItem, dc.GetCurrentFont());

	if ( nWidth > rcBtn.Width() ) {
		uFormat = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT | DT_END_ELLIPSIS;
	} else {
		uFormat = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | /*DT_CENTER | */DT_NOCLIP;
		if (CTabBarOption::s_bCenterAlign)
			uFormat |= DT_CENTER;
	}

	if ( item.state & TISS_NAVIGATELOCK ) {
		CRect		rcDisabled	= rcBtn + CPoint(1, 1);
		COLORREF	clrOld2		= dc.SetTextColor(m_colDisableHi);
		dc.DrawText(item.strItem, -1, rcDisabled, uFormat);
		dc.SetTextColor(clrOld2);
	}

	// タブ上に文字列を書く
	dc.DrawText(item.strItem, -1, rcBtn, uFormat);
	dc.SetTextColor(clrOld);
}



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


CTabSkinDefault::CTabSkinDefault()
{
	_LoadBitmap();
}


void	CTabSkinDefault::_LoadBitmap()
{
	CString	strTabSkinDir = _GetSkinDir();	// スキンを置いているフォルダを取得
	CString strTabSkinCur;
	CString strTabSkinNone;
	CString strTabSkinSel;

	strTabSkinCur.Format (_T("%sTabSkinCur.bmp")	, strTabSkinDir);
	strTabSkinNone.Format(_T("%sTabSkinNone.bmp")	, strTabSkinDir);
	strTabSkinSel.Format (_T("%sTabSkinSel.bmp")	, strTabSkinDir);

	if (::PathFileExists(strTabSkinCur) == FALSE || ::PathFileExists(strTabSkinNone) == FALSE || ::PathFileExists(strTabSkinSel) == FALSE) {
		strTabSkinDir = Misc::GetExeDirectory() + _T("skin\\default\\");	// スキンフォルダにファイルがなかった
		strTabSkinCur.Format (_T("%sTabSkinCur.bmp")	, strTabSkinDir);
		strTabSkinNone.Format(_T("%sTabSkinNone.bmp")	, strTabSkinDir);
		strTabSkinSel.Format (_T("%sTabSkinSel.bmp")	, strTabSkinDir);
	}
	// タブ・スキン
	m_TabSkinCur.Attach	( AtlLoadBitmapImage(strTabSkinCur.GetBuffer(0)	, LR_LOADFROMFILE) );
	m_TabSkinNone.Attach( AtlLoadBitmapImage(strTabSkinNone.GetBuffer(0), LR_LOADFROMFILE) );
	m_TabSkinSel.Attach	( AtlLoadBitmapImage(strTabSkinSel.GetBuffer(0)	, LR_LOADFROMFILE) );

	try {
	if (m_TabSkinCur.m_hBitmap	== NULL) throw strTabSkinCur;
	if (m_TabSkinNone.m_hBitmap == NULL) throw strTabSkinNone;
	if (m_TabSkinSel.m_hBitmap	== NULL) throw strTabSkinSel;
	} catch (const CString& file) {
		//MessageBox(NULL, file + _T("が存在しません\nタブをクラシックスタイルに変更します"), NULL, NULL);
		throw (LPCTSTR)file;
	}
}


void	CTabSkinDefault::_DrawSkin(HDC hDC, CRect rcItem, CBitmap& pBmp)
{
	CSize	szBmp;
	pBmp.GetSize(szBmp);
	HDC		hdcCompatible	= ::CreateCompatibleDC(hDC);
	HBITMAP	hBitmapOld		= (HBITMAP)SelectObject(hdcCompatible, pBmp.m_hBitmap);

	if ( rcItem.Width() <= szBmp.cx ) {
		// All
		::StretchBlt(hDC, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), hdcCompatible, 0, 0, szBmp.cx, szBmp.cy, SRCCOPY);

		int	nWidthLR = rcItem.Width() / 3;
		if ( nWidthLR < szBmp.cx ) {
			// Left
			::StretchBlt(hDC, rcItem.left, rcItem.top, nWidthLR, rcItem.Height(), hdcCompatible, 0, 0, nWidthLR, szBmp.cy, SRCCOPY);

			// Right
			::StretchBlt(hDC, rcItem.right - nWidthLR, rcItem.top, nWidthLR, rcItem.Height(), hdcCompatible, szBmp.cx - nWidthLR, 0, nWidthLR, szBmp.cy, SRCCOPY);
		}
	} else {
		int	nWidthLR = szBmp.cy / 3;
		// Left
		::StretchBlt(hDC, rcItem.left, rcItem.top, nWidthLR, rcItem.Height(), hdcCompatible, 0, 0, nWidthLR, szBmp.cy, SRCCOPY);

		// Right
		::StretchBlt(hDC, rcItem.right - nWidthLR, rcItem.top, nWidthLR, rcItem.Height(), hdcCompatible, szBmp.cx - nWidthLR, 0, nWidthLR, szBmp.cy, SRCCOPY);

		// Mid
		::StretchBlt(hDC, rcItem.left + nWidthLR, rcItem.top, rcItem.Width() - (nWidthLR * 2), rcItem.Height(), hdcCompatible, nWidthLR, 0, nWidthLR, szBmp.cy, SRCCOPY);
	}

	::SelectObject(hdcCompatible, hBitmapOld);
	::DeleteObject(hdcCompatible);
}


void	CTabSkinDefault::_DrawSkinCur(CDCHandle dc, CRect rcItem)
{
	_DrawSkin(dc, rcItem, m_TabSkinCur);
}

void	CTabSkinDefault::_DrawSkinNone(CDCHandle dc, CRect rcItem)
{
	_DrawSkin(dc, rcItem, m_TabSkinNone);
}

void	CTabSkinDefault::_DrawSkinSel(CDCHandle dc, CRect rcItem)
{
	_DrawSkin(dc, rcItem, m_TabSkinSel);
}




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

CTabSkinTheme::CTabSkinTheme(CTheme& theme)
	: m_Theme(theme)
{
}


void	CTabSkinTheme::_DrawSkinCur(CDCHandle dc, CRect rcItem)
{
#if 0
	rcItem.left -= 2;
	rcItem.right+= 2;
#endif
	m_Theme.DrawThemeBackground(dc, TABP_TABITEM, TIS_SELECTED, rcItem);
}

void	CTabSkinTheme::_DrawSkinNone(CDCHandle dc, CRect rcItem)
{
	rcItem.top += 2;
	m_Theme.DrawThemeBackground(dc, TABP_TABITEM, TIS_NORMAL, rcItem);
}

void	CTabSkinTheme::_DrawSkinSel(CDCHandle dc, CRect rcItem)
{
	rcItem.top += 2;
	m_Theme.DrawThemeBackground(dc, TABP_TABITEM, TIS_HOT, rcItem);
}



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

void	CTabSkinClassic::_DrawSkinCur(CDCHandle dc, CRect rcItem)
{
	rcItem.right -= 6;
	//dc.DrawEdge(rcItem, EDGE_SUNKEN, BF_RECT);
	COLORREF crTxt = dc.SetTextColor(m_colBtnFace);
	COLORREF crBk  = dc.SetBkColor(m_colBtnHi);
	CBrush	 hbr( CDCHandle::GetHalftoneBrush() );
	dc.SetBrushOrg(rcItem.left, rcItem.top);
	dc.FillRect(rcItem, hbr);
	dc.SetTextColor(crTxt);
	dc.SetBkColor(crBk);
	dc.DrawEdge(rcItem, EDGE_SUNKEN, BF_RECT);

	_DrawSeparators(dc, rcItem);
}

void	CTabSkinClassic::_DrawSkinNone(CDCHandle dc, CRect rcItem)
{
	rcItem.right -= 6;

	_DrawSeparators(dc, rcItem);
}

void	CTabSkinClassic::_DrawSkinSel(CDCHandle dc, CRect rcItem)
{
	rcItem.right -= 6;
	dc.DrawEdge(rcItem, BDR_RAISEDINNER, BF_RECT);

	_DrawSeparators(dc, rcItem);
}


void	CTabSkinClassic::_DrawSeparators(CDCHandle dc, CRect rcItem)
{
	int   cy = rcItem.Height();
	for (int i = 0; i < m_rarrSeparators.GetSize(); ++i) {
		CPoint &pt = m_rarrSeparators[i];
		//CRect	rc(pt.x, pt.y + 2, pt.x + 2, pt.y + cy - 2);
		CRect	rc(pt.x - 4, pt.y + 2, pt.x - 2, pt.y + cy - 2);
		dc.DrawEdge(rc, EDGE_ETCHED, BF_LEFT);	// draw separator line
	}
}


#define LOADINGSTRING	_T("Loading...")

////////////////////////////////////////////////////////////////////////
// CDonutTabBar::Impl

class CDonutTabBar::Impl : 
	public CDoubleBufferWindowImpl<Impl>,
	public CThemeImpl<Impl>,
	public CTrackMouseLeave<Impl>,
	public IDropTargetImpl<Impl>,
	public IDropSourceImpl<Impl>,
	protected CTabBarOption
{
	friend struct CDonutTabBar::CLockRedraw;
public:
	DECLARE_WND_CLASS_EX(_T("DonutTabBar"), CS_DBLCLKS, COLOR_BTNFACE)

	// Constant
	enum _ETabDrawConstants {
		s_kcxTextMargin = 7,
		s_kcyTextMargin = 3,	// タブとテキスト間の上下の余白
		s_kcxGap		= 2,
		s_kcyGap		= 2,	// タブの上下間の幅
		s_kSideMargin	= 2,	// 左右の余白
		s_kcxSeparator	= 2,

		s_kcxUpDown 	= 28,
		s_kcyUpDown 	= 14,
	};

	enum _hitTestFlag { 
		htInsetLeft, 	// タブバーの左
//		htInsetRight, 	// タブバーの右
		htItem, 		// タブ
		htSeparator, 	// タブの境界
		htOutside, 		// タブバーの領域外
		htWhole, 		// タブがない?
		htOneself,		// 自分のタブ
		htOneselfLeft,	// 自分のタブの左
		htOneselfRight,	// 自分のタブの右
	};

	enum insertFlags { 
		insertLeft, 	// タブバーの左
		insertMiddle, 	// タブの間
		insertRight,	// タブバーの右
	};
	
	// Command ID
	enum { COMMAND_ID_DROPBUTTON= 0x00000001L, };


	// Constructor
	Impl();

	HWND	Create(HWND hWndParent);
	void	SetMDIClient(HWND hWndMDIClient);

	// Attributes
	HWND	GetTabHwnd(int nIndex);
	int 	GetTabIndex(HWND hWndMDIChild);
	void	SetFont(HFONT hFont) { m_font = hFont; }
	int		GetItemCount() const { return (int)m_vecpItem.size(); }
	void	GetItemState(int nIndex, DWORD& state);
	void	SetItemActive(int nIndex);
	void	SetItemInactive(int nIndex);
	int		GetCurSel() const;
	bool	SetCurSel(int nIndex, bool bClicked = false, bool bActiveOnly = false);
	void	GetCurMultiSel(CSimpleArray<int> &arrDest, bool bIncludeCurSel = true);
	void	GetCurMultiSelEx(CSimpleArray<int>& arrDest, int nIndex);
	void	SetLinkState(ELinkState State) { m_LinkState = State; }
	void	SetConnecting(HWND hWnd);
	void	SetDownloading(HWND hWnd);
	void	SetComplete(HWND hWnd);
	void	NavigateLockTab(HWND hWnd, bool bOn);
	void	ReloadSkin();

	// Oparation
	void	OnMDIChildCreate(HWND hWnd);
	void	OnMDIChildDestroy(HWND hWnd);
	void	InsertHere(bool bUse) { m_bInsertHere = bUse; }
	void	SetInsertIndex(int nIndex) { m_nInsertIndex = nIndex; }
	bool	MoveItems(int nDestIndex, CSimpleArray<int>& arrSrcs);
	void	LeftTab();
	void	RightTab();
	void	ShowTabMenu(int nIndex);
	int 	ShowTabListMenuDefault (int nX, int nY);
	int 	ShowTabListMenuVisible (int nX, int nY);
	int 	ShowTabListMenuAlphabet(int nX, int nY);

	// Overrides
	void	DoPaint(CDCHandle dc);
	void	OnTrackMouseMove(UINT nFlags, CPoint pt);
	void	OnTrackMouseLeave();

	bool	OnScroll(UINT nScrollCode, UINT nPos, bool bDoScroll = true);
	DROPEFFECT OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect);
	DROPEFFECT OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point);
	void	OnDragLeave() { _ClearInsertionEdge(); }

	bool	OnNewTabCtrlItems(int	nInsertIndex,
							  CSimpleArray<unique_ptr<TabItem> >&	/*items*/,
							  IDataObject*	pDataObject,
							  DROPEFFECT&	dropEffect);
	bool	OnDropTabCtrlItem(int nIndex, IDataObject *pDataObject, DROPEFFECT &dropEffect);
	void	OnDeleteItemDrag(int nIndex);

	// Message map and handlers
	BEGIN_MSG_MAP( Impl )
		MSG_WM_CREATE		( OnCreate )
		MSG_WM_DESTROY		( OnDestroy )
		MSG_WM_SIZE			( OnSize )
		MESSAGE_HANDLER_EX	( WM_MENUSELECT, OnMenuSelect )

		MSG_WM_LBUTTONDOWN	( OnLButtonDown )	// Drag

		MSG_WM_RBUTTONUP	( OnRButtonUp )
		MSG_WM_LBUTTONDBLCLK( OnLButtonDblClk )
		MSG_WM_MBUTTONUP	( OnMButtonUp )
		MSG_WM_COMMAND		( OnCommand 	  )
		COMMAND_ID_HANDLER_EX( COMMAND_ID_DROPBUTTON, OnPushDropButton	)
		USER_MSG_WM_SETFAVICONIMAGE( OnSetFaviconImage )
		NOTIFY_CODE_HANDLER_EX( UDN_DELTAPOS   , OnDeltaPos	)
		NOTIFY_CODE_HANDLER_EX( TTN_GETDISPINFO, OnGetDispInfo 	)

		CHAIN_MSG_MAP( CDoubleBufferWindowImpl<Impl> )
		CHAIN_MSG_MAP( CThemeImpl<Impl> )
		CHAIN_MSG_MAP( CTrackMouseLeave<Impl> )
		REFLECT_NOTIFICATIONS()
	ALT_MSG_MAP(1)							// MDI child windows messages
		m_wndMDIChildProcessing = hWnd;
		MSG_WM_MDIACTIVATE	( OnMDIActivate )
		MSG_WM_SETTEXT		( OnMDISetText 	)
		m_wndMDIChildProcessing = NULL;
	END_MSG_MAP()

	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	void	OnDestroy();
	void	OnSize(UINT nType, CSize size);
	LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void	OnLButtonDown(UINT nFlags, CPoint point);

	void	OnRButtonUp(UINT nFlags, CPoint point);
	void	OnLButtonDblClk(UINT nFlags, CPoint point);
	void	OnMButtonUp(UINT nFlags, CPoint point);
	void	OnCommand(UINT wNotifyCode, int wID, HWND hwndCtl);
	void	OnPushDropButton(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSetFaviconImage(HWND hWnd, HICON hIcon);
	LRESULT OnDeltaPos(LPNMHDR pnmh);
	LRESULT OnGetDispInfo(LPNMHDR pnmh);

	// CChildFrameから
	void	OnMDIActivate(HWND hWndChildDeact, HWND hWndChildAct);
	int		OnMDISetText(LPCTSTR lpstrText);

	void	OnSetCurSel(int nIndex, int nOldIndex);
	HRESULT OnGetTabCtrlDataObject(CSimpleArray<int>& arrIndex, IDataObject** ppDataObject);

private:

	void	_InitTooltip();
	int		_GetRequiredHeight();
	int		_GetItemHeight();
	bool	_IsValidIndex(int nIndex) const;
	int		_HitTest(const CPoint& pt);
	void	_SetTabText(int nIndex, LPCTSTR lpszTab);
	void	_RefreshBandInfo();
	void	_UpdateSingleLineLayout();
	void	_UpdateMultiLineLayout(int nWidth);
	void	_UpdateLayout();
	CRect	_InflateGapWidth(const CRect& rc) const;
	void	_ShowOrHideUpDownCtrl(const CRect& rcClient);
	void	_ScrollOpposite(int nNewSel, bool bClicked);
	void	_HotItem(int nNewHotIndex = -1);
	void	_PressItem(int nPressedIndex = -1);
	void	_LockRedraw(bool bLock);
	CRect	_MeasureItem(const CString& strText);
	void	_SetItemImageIndex(int nIndex, int nImgIndex);
	bool	_IsLinkOpen();
	int		_GetFirstVisibleIndex();
	int		_GetLastVisibleIndex();

	bool	_InsertItem(int nIndex, unique_ptr<TabItem>&& item);
	bool	_DeleteItem(int nIndex, bool bMoveNow = false);
	void	_DeleteItems(CSimpleArray<int>& arrSrcs);
	void	_ReplaceFavicon(int nIndex, HICON hIcon);

	// Drag&Drop
	int		_HitTestCurTabSide(CPoint point, _hitTestFlag &flag);
	int		_HitTestSeparator(CPoint point, vector<std::pair<int, CRect> >& vecRect/*, int nInset*/);
	int		_HitTestLeftSide(CPoint point, vector<std::pair<int, CRect> >& vecRect);
	int		_HitTestOnDragging(_hitTestFlag &flag, CPoint point);
	void	_ClearInsertionEdge();
	bool	_DrawInsertionEdge(_hitTestFlag flag, int nIndex);
	void	_DrawInsertionEdgeAux(CPoint pt, insertFlags flag);
	void	_DrawInsertionEdgeAux(const CRect &rc);
	bool	_IsSameIndexDropped(int nDestIndex);
	void	_DoDragDrop(const CPoint& pt, UINT nFlags, int nIndex);
	bool	_CanScrollItem(bool bRight = true) const;
	bool	_ScrollItem(bool bRight = true);

	// Data members
	vector<unique_ptr<TabItem> >	m_vecpItem;
	vector<int>			m_vecSeparators;
	CFont				m_font;
	int 				m_nFirstIndexOnSingleLine;	// for single line
	CUpDownCtrl			m_wndUpDown;				// for single line
	CDropDownButtonCtrl	m_wndDropBtn;				// for single line
	CToolTipCtrl		m_ToolTip;
	CString				m_strTooltipText;
	CImageList			m_imgs;
	CImageList			m_FaviconImageList;
	unique_ptr<CTabSkin>	m_pTabSkin;
	int			m_nActiveIndex;
	int			m_nHotIndex;
	int 		m_nPressedIndex;					// Current capturing index
	CSimpleArray<CPoint> m_arrSeparators;			// Point of separator

	CRect				m_rcInvalidateOnDrawingInsertionEdge;
	bool				m_bDragFromItself;
	CSimpleArray<int>	m_arrCurDragItems;

	CWindow 	m_wndMDIChildProcessing;		// while message processing
	CMDIWindow	m_wndMDIChildPopuping;			// while menu popuping (m_bPopup is not enough)
	bool		m_bRedrawLocked;
	bool		m_bLockRefreshBandInfo;

	CMenu		m_menuPopup;
	ELinkState	m_LinkState;
	bool		m_bInsertHere;
	int			m_nInsertIndex;


};

/////////////////////////////////////////


// Constructor
CDonutTabBar::Impl::Impl()
	: m_nFirstIndexOnSingleLine(0)
	, m_nActiveIndex(-1)
	, m_nHotIndex(-1)
	, m_nPressedIndex(-1)
	, m_bDragFromItself(false)
	, m_bRedrawLocked(false)
	, m_bLockRefreshBandInfo(false)
	, m_LinkState(LINKSTATE_OFF)
	, m_bInsertHere(false)
	, m_nInsertIndex(-1)
{
	ATLVERIFY(SetThemeClassList(L"TAB"));
}


//--------------------------------
/// DonutTabBar作成
HWND	CDonutTabBar::Impl::Create(HWND hWndParent)
{
	HWND hWnd = __super::Create(hWndParent, CRect(0, 0, 200, 20), NULL, WS_CHILD | WS_VISIBLE, NULL, IDC_MDITAB);
	ATLASSERT(::IsWindow(hWnd));
	return hWnd;
}

//--------------------------------
void CDonutTabBar::Impl::SetMDIClient(HWND hWndMDIClient)
{
	ATLASSERT( ::IsWindow(hWndMDIClient) );
	ATLASSERT(m_wndMDIChildPopuping.m_hWndMDIClient == NULL);
	m_wndMDIChildPopuping.m_hWndMDIClient = hWndMDIClient;
}

// Attributes

//----------------------------
/// インデックス -> ウィンドウハンドル
HWND	CDonutTabBar::Impl::GetTabHwnd(int nIndex)
{
	if (_IsValidIndex(nIndex))
		return m_vecpItem[nIndex]->hWnd;
	return NULL;
}

//----------------------------
/// ウィンドウハンドル -> インデックス
int 	CDonutTabBar::Impl::GetTabIndex(HWND hWndMDIChild)
{
	if (hWndMDIChild == NULL)
		return -1;

	int nCount = GetItemCount();
	for (int i = 0; i < nCount; ++i) {
		if (m_vecpItem[i]->hWnd == hWndMDIChild)
			return i;
	}

	return -1;
}

//-----------------------------
void	CDonutTabBar::Impl::GetItemState(int nIndex, DWORD& state)
{
	ATLASSERT(_IsValidIndex(nIndex));
	state = m_vecpItem[nIndex]->state;
}

//-----------------------------
void	CDonutTabBar::Impl::SetItemActive(int nIndex)
{
	ATLASSERT(_IsValidIndex(nIndex));

	if ( m_vecpItem[nIndex]->ModifyState(TISS_INACTIVE, 0) ) {
		InvalidateRect(m_vecpItem[nIndex]->rcItem, FALSE);
	}
}


//-----------------------------
void	CDonutTabBar::Impl::SetItemInactive(int nIndex)
{
	ATLASSERT(_IsValidIndex(nIndex));

	if ( m_vecpItem[nIndex]->ModifyState(0, TISS_INACTIVE) ) {
		InvalidateRect(m_vecpItem[nIndex]->rcItem, FALSE);
	}
}




//----------------------------
/// 現在アクティブなタブを返す
int		CDonutTabBar::Impl::GetCurSel() const
{
	if (m_nActiveIndex == -1) {
		int nCount = GetItemCount();
		for (int i = 0; i < nCount; ++i) {
			if (m_vecpItem[i]->state & TISS_SELECTED)
				return i;
		}
	}
	return m_nActiveIndex;
}

bool	CDonutTabBar::Impl::SetCurSel(int nIndex, bool bClicked/* = false*/, bool bActiveOnly /*= false*/)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	//_ResetMultiSelectedItems();
	for (auto it = m_vecpItem.begin(); it != m_vecpItem.end(); ++it) {
		if ( (*it)->ModifyState(TISS_MSELECTED, 0) )
			InvalidateRect((*it)->rcItem);
	}

	int nCurSel = GetCurSel();
	if (nCurSel != -1 && nCurSel == nIndex)
		return true;

	// clean up
	if ( _IsValidIndex(nCurSel) ) {
		if ( m_vecpItem[nCurSel]->ModifyState(TISS_SELECTED | TISS_HOT | TISS_PRESSED | TISS_MSELECTED, 0) )
			InvalidateRect(m_vecpItem[nCurSel]->rcItem);
	}

	// new selected item
	if ( m_vecpItem[nIndex]->ModifyState(TISS_HOT | TISS_PRESSED | TISS_MSELECTED, TISS_SELECTED) )
		InvalidateRect(m_vecpItem[nIndex]->rcItem);

	_ScrollOpposite(nIndex, bClicked);

	if (bActiveOnly == false)
		OnSetCurSel(nIndex, nCurSel);

	UpdateWindow();
	return true;
}

//-------------------------------------------------------
/// 現在(複数)選択されているタブのインデックスを取得
/// bIncludeCurSel == trueで現在アクティブなタブを含める
void	CDonutTabBar::Impl::GetCurMultiSel(CSimpleArray<int> &arrDest, bool bIncludeCurSel/* = true*/)
{						// have to be sorted!
	arrDest.RemoveAll();

	int nCount = GetItemCount();
	for (int i = 0; i < nCount; ++i) {
		if (bIncludeCurSel) {
			if (m_vecpItem[i]->state & TISS_SELECTED) {
				arrDest.Add(i);
				continue;
			}
		}

		if (m_vecpItem[i]->state & TISS_MSELECTED)
			arrDest.Add(i);
	}
}

//--------------------------------------------------------
void	CDonutTabBar::Impl::GetCurMultiSelEx(CSimpleArray<int>& arrDest, int nIndex)
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

//--------------------------------------------------------
void	CDonutTabBar::Impl::SetConnecting(HWND hWnd)
{
	int nItem = GetTabIndex(hWnd);
	if (nItem == -1)
		return;

	_SetItemImageIndex(nItem, 0);
}

//--------------------------------------------------------
void	CDonutTabBar::Impl::SetDownloading(HWND hWnd)
{
	int nItem = GetTabIndex(hWnd);
	if (nItem == -1)
		return;

	_SetItemImageIndex(nItem, 1);
}

//--------------------------------------------------------
void	CDonutTabBar::Impl::SetComplete(HWND hWnd)
{
	int nItem = GetTabIndex(hWnd);
	if (nItem == -1)
		return;

	_SetItemImageIndex(nItem, -1);
}

//-------------------------------------------------------
void	CDonutTabBar::Impl::NavigateLockTab(HWND hWnd, bool bOn)
{
	int nIndex = GetTabIndex(hWnd);
	if (nIndex >= 0) {
		if (bOn)
			m_vecpItem[nIndex]->ModifyState(0, TISS_NAVIGATELOCK);
		else
			m_vecpItem[nIndex]->ModifyState(TISS_NAVIGATELOCK, 0);
		InvalidateRect(_InflateGapWidth(m_vecpItem[nIndex]->rcItem), FALSE);
	}
}

//--------------------------------------------------------
/// スキンを読み込む
void	CDonutTabBar::Impl::ReloadSkin()
{
	CString strTabBmp = _GetSkinDir() + _T("Tab.bmp");
	if (m_imgs.IsNull() == false)
		m_imgs.Destroy();
	
	m_imgs.Create(6, 6, ILC_COLOR24 | ILC_MASK, 2, 1);
	ATLASSERT(m_imgs.m_hImageList != NULL);

	CBitmap bmp;
	bmp.Attach( AtlLoadBitmapImage(strTabBmp.GetBuffer(0), LR_LOADFROMFILE) );
	if (bmp.m_hBitmap == NULL)
		bmp.LoadBitmap(IDB_MDITAB);
	ATLASSERT(bmp.m_hBitmap != NULL);
	m_imgs.Add(bmp, RGB(255, 0, 255));

	m_wndDropBtn.SetDrawStyle(CSkinOption::s_nTabStyle);
	if (CSkinOption::s_nTabStyle == SKN_TAB_STYLE_CLASSIC) {
		UxTheme_Wrap::SetWindowTheme(m_wndUpDown.m_hWnd, L" ", L" ");
	} else {
		UxTheme_Wrap::SetWindowTheme(m_wndUpDown.m_hWnd, NULL, L"SPIN");
	}


	if (::PathFileExists(_GetSkinDir()) == FALSE) {
		CSkinOption::s_nTabStyle = SKN_TAB_STYLE_CLASSIC;
	}
	try {
		switch (CSkinOption::s_nTabStyle)
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
	} catch (LPCTSTR /*strFile*/) {
		CSkinOption::s_nTabStyle = SKN_TAB_STYLE_CLASSIC;
		ReloadSkin();
	}

	InvalidateRect(NULL, FALSE);
	_UpdateLayout();
}



// Oparation

//----------------------------------------
void	CDonutTabBar::Impl::OnMDIChildCreate(HWND hWnd)
{
	unique_ptr<TabItem>	pItem(new TabItem);
	pItem->hWnd		= hWnd;
	pItem->strItem	= LOADINGSTRING;
	if ( m_bRedrawLocked || hWnd != m_wndMDIChildPopuping.MDIGetActive() )
		pItem->state |= TISS_INACTIVE;

	if (m_bInsertHere) {
		_InsertItem(m_nInsertIndex, std::move(pItem));

	} else {
		ATLASSERT(GetTabIndex(hWnd) == -1);

		// add tab item
		int  nPos;
		int  nCurSel = GetCurSel();
		if (nCurSel == -1)
			nPos = 0;
		else if ( (s_dwExStyle & MTB_EX_ADDLINKACTIVERIGHT) && _IsLinkOpen() )
			nPos = nCurSel + 1;
		else if (s_dwExStyle & MTB_EX_ADDLEFT)
			nPos = 0;
		else if (s_dwExStyle & MTB_EX_ADDLEFTACTIVE)
			nPos = nCurSel;
		else if (s_dwExStyle & MTB_EX_ADDRIGHTACTIVE)
			nPos = nCurSel + 1;
		else
			nPos = GetItemCount();

		_InsertItem(nPos, std::move(pItem));
	}
}

//----------------------------------------
void	CDonutTabBar::Impl::OnMDIChildDestroy(HWND hWnd)
{
	int nIndex = GetTabIndex(hWnd);
	int nCurIndex = GetCurSel();
	if (nCurIndex == nIndex) {
		// アクティブなビューが破棄された
		// 次のタブをアクティブにする
		auto funcManageClose = [this](int nActiveIndex) ->int {
			int	nCount	= GetItemCount();
			if (s_dwExStyle & MTB_EX_LEFTACTIVEONCLOSE) {
				// 閉じるときアクティブなタブの左をアクティブにする
				int nNext = nActiveIndex - 1;
				if (nNext >= 0) {
					return nNext;
				} else {
					nNext = nActiveIndex + 1;

					if (nNext < nCount) {
						return nNext;
					}
				}
			} else if (s_dwExStyle & MTB_EX_RIGHTACTIVEONCLOSE) {
				// 閉じるときアクティブなタブの右をアクティブにする
				int nNext = nActiveIndex + 1;
				if (nNext < nCount) {
					return nNext;
				} else {
					nNext = nActiveIndex - 1;

					if (nNext >= 0) {
						return nNext;
					}
				}
			}
			// 全タブが削除された
			return -1;
		};
		int nNextIndex = funcManageClose(nCurIndex);
		if (nNextIndex == -1) {
			// 最後のタブが閉じられた
			//m_funcTabChangeNotify(NULL);
			//return ;	// 今のところ閉じない
		} else
			SetCurSel(nNextIndex);

		_DeleteItem(nCurIndex);			// タブを削除する
	} else {
		// アクティブでないビューが破棄された
		_DeleteItem(nIndex);
	}
}


//----------------------------------------
bool	CDonutTabBar::Impl::MoveItems(int nDestIndex, CSimpleArray<int>& arrSrcs)
{
	// arrSrcs has to be sorted! if nDestIndex is invalid, added to tail
	if (arrSrcs.GetSize() <= 0)
		return false;

	int i = 0;

	if (arrSrcs.GetSize() == 1) {
		if (nDestIndex == arrSrcs[0] || nDestIndex == arrSrcs[0] + 1)
			return true;		// no need

		if (!_IsValidIndex(nDestIndex) && arrSrcs[0] == m_vecpItem.size() - 1)
			return true;		// no need
	}

	_LockRedraw(true);
	SetRedraw(FALSE);
	m_bLockRefreshBandInfo = true;

	vector<TabItem> temp;

	//( _T(" save src indexs - ") );

	for (i = 0; i < arrSrcs.GetSize(); ++i) {
		int nIndex = arrSrcs[i];

		if ( !_IsValidIndex(nIndex) )
			continue;

		temp.push_back(*m_vecpItem[nIndex]);
	}


	//( _T(" delete item indexs - ") );

	for (i = arrSrcs.GetSize() - 1; i >= 0; --i) {
		int nIndex = arrSrcs[i];

		if ( !_IsValidIndex(nIndex) )
			continue;

		if (nDestIndex > nIndex)
			--nDestIndex;

		_DeleteItem(nIndex, true);
	}

	// add all
	if ( !_IsValidIndex(nDestIndex) ) {
		nDestIndex = GetItemCount();
	}

	for (i = 0; i < (int) temp.size(); ++i) {
		TabItem* pItem = new TabItem;
		*pItem = temp[i];
		_InsertItem(nDestIndex, unique_ptr<TabItem>(pItem));
		++nDestIndex;
	}

	m_bLockRefreshBandInfo = false;
	_RefreshBandInfo();

	_LockRedraw(false);
	SetRedraw(TRUE);
	Invalidate();
	UpdateWindow();

	return true;
}


//----------------------------------
void CDonutTabBar::Impl::LeftTab()
{
	int nIndex = GetCurSel();
	int nCount = GetItemCount();

	if (nCount < 2)
		return;

	int nNext  = nIndex - 1 < 0 ? nCount - 1 : nIndex - 1;
	SetCurSel(nNext);
}

//-------------------------
void CDonutTabBar::Impl::RightTab()
{
	int nIndex = GetCurSel();
	int nCount = GetItemCount();

	if (nCount < 2)
		return;

	int nNext  = (nIndex + 1 < nCount) ? nIndex + 1 : 0;
	SetCurSel(nNext);
}

//------------------------
void CDonutTabBar::Impl::ShowTabMenu(int nIndex)
{
	HWND	hWndChild = GetTabHwnd(nIndex);

	if (hWndChild == NULL)
		return;

	CPoint			point;
	::GetCursorPos(&point);
	CMenuHandle 	menu	  = m_menuPopup.GetSubMenu(0);

	DWORD dwFlags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON;
	if (CMenuOption::s_bR_Equal_L) 
		dwFlags |= TPM_RIGHTBUTTON;

	menu.TrackPopupMenu( dwFlags,
						 point.x,
						 point.y,
						 hWndChild );
}


//-------------------------------
int CDonutTabBar::Impl::ShowTabListMenuDefault(int nX, int nY)
{
	CMenu	menu;
	int 	nCount = GetItemCount();
	menu.CreatePopupMenu();

	for (int i = 0; i < nCount; ++i) {
		menu.AppendMenu( 0, i + 1, m_vecpItem[i]->strItem );
	}

	int 	nRet	 = menu.TrackPopupMenu(
						TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
						nX,
						nY + 1,
						m_hWnd);
	return	nRet - 1;
}


//---------------------------------
int CDonutTabBar::Impl::ShowTabListMenuVisible(int nX, int nY)
{
	int idxFirst = _GetFirstVisibleIndex();
	int idxLast  = _GetLastVisibleIndex();
	if (idxFirst == -1 || idxLast == -1)
		return -1;

	// no item exist

	//表示されていない左側のタブ
	int 		i		 = 0;
	int 		nCount	 = GetItemCount();
	CMenu		menu;
	menu.CreatePopupMenu();

	for (i = 0; i < idxFirst; i++) {
		menu.AppendMenu( 0, i + 1, m_vecpItem[i]->strItem );
	}

	//表示中のタブ
	if (i >= nCount)
		return -1;

	if (idxFirst != 0) {
		menu.AppendMenu(MF_SEPARATOR, 0);
	}

	CMenuHandle menuSub;
	menuSub.CreatePopupMenu();

	for (i = idxFirst; i <= idxLast; i++) {
		menuSub.AppendMenu( 0, i + 1, m_vecpItem[i]->strItem );
	}

	menu.AppendMenu( MF_POPUP, (UINT_PTR) menuSub.m_hMenu, _T("表示中のタブ") );

	//表示されていない右側のタブ
	if (i > nCount)
		return -1;

	if (idxLast != nCount - 1) {
		menu.AppendMenu(MF_SEPARATOR, 0);
	}

	for (i = idxLast + 1; i < nCount; i++) {
		menu.AppendMenu( 0, i + 1, m_vecpItem[i]->strItem );
	}

	int 	nRet = menu.TrackPopupMenu(
						TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
						nX,
						nY + 1,
						m_hWnd);
	return nRet - 1;
}




//-------------------------------------
int CDonutTabBar::Impl::ShowTabListMenuAlphabet(int nX, int nY)
{
	struct _Object_TabSorting {
		CString strTitle;
		int 	nIndex;
	};

	vector<_Object_TabSorting> aryTab;

	int 	nCount = GetItemCount();
	for (int i = 0; i < nCount; ++i) {
		_Object_TabSorting tab;
		tab.strTitle = m_vecpItem[i]->strItem;
		tab.nIndex	 = i;
		aryTab.push_back(tab);
	}

	std::sort( aryTab.begin(), aryTab.end(), [] (const _Object_TabSorting &tab1, const _Object_TabSorting &tab2) {
		return ::lstrcmp(tab1.strTitle, tab2.strTitle) < 0;
	});

	CMenu	menu;
	menu.CreatePopupMenu();
	for (int i = 0; i < nCount; i++) {
		menu.AppendMenu(0, i + 1, aryTab[i].strTitle);
	}

	int 	nRet   = menu.TrackPopupMenu(
							TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
							nX,
							nY + 1,
							m_hWnd);

	if (nRet == 0)
		return -1;

	return aryTab[nRet - 1].nIndex;
}







// Overrides

//--------------------------
/// OnMouseMoveの代わり
void	CDonutTabBar::Impl::OnTrackMouseMove(UINT nFlags, CPoint pt)
{
	// 真下にあるタブをHotにする
	int nIndex = _HitTest(pt);
	if ( _IsValidIndex(nIndex) ) {
		// if other button made hot or first hot
		if (m_nHotIndex == -1 || m_nHotIndex != nIndex) {
			m_ToolTip.Activate(FALSE);
			m_ToolTip.Activate(TRUE);

			_HotItem(nIndex);
		}
	} else {
		m_ToolTip.Activate(FALSE);
		_HotItem();	// clean up
	}
}


//---------------------------
/// ツールチップを消す
void	CDonutTabBar::Impl::OnTrackMouseLeave()
{
	m_ToolTip.SetDelayTime(TTDT_INITIAL, 500);
	_HotItem();
}

//---------------------------
void	CDonutTabBar::Impl::DoPaint(CDCHandle dc)
{
	HWND	hWnd	= GetParent();
	CPoint	pt;
	MapWindowPoints(hWnd, &pt, 1);
	::OffsetWindowOrgEx( (HDC)dc.m_hDC, pt.x, pt.y, NULL );
	LRESULT lResult = ::SendMessage(hWnd, WM_ERASEBKGND, (WPARAM)dc.m_hDC, 0L);
	::SetWindowOrgEx((HDC)dc.m_hDC, 0, 0, NULL);

	CFontHandle fontOld = dc.SelectFont(m_font);
	int 		modeOld = dc.SetBkMode(TRANSPARENT);

	int	i = s_bMultiLine ? 0 : m_nFirstIndexOnSingleLine;
	int nCount = GetItemCount();
	for (auto it = m_vecpItem.begin(); it != m_vecpItem.end(); ++it) {
		// 各タブを描写する
		m_pTabSkin->Update(dc, m_imgs, m_FaviconImageList, *(*it));
	}

	dc.SelectFont(fontOld);
	dc.SetBkMode(modeOld);
}

//------------------------------
bool CDonutTabBar::Impl::OnScroll(UINT nScrollCode, UINT nPos, bool bDoScroll/* = true*/)
{
	bool bResult = false;

	if ( m_wndUpDown.IsWindowVisible() == false )
		return bResult;

	if (LOBYTE(nScrollCode) == SB_LINEUP) {
		if (bDoScroll)
			bResult = _ScrollItem(false);
		else
			bResult = _CanScrollItem(false);
	} else if (LOBYTE(nScrollCode) == SB_LINEDOWN) {
		if (bDoScroll)
			bResult = _ScrollItem(true);
		else
			bResult = _CanScrollItem(true);
	}

	return bResult;
}


//---------------------------------------
DROPEFFECT CDonutTabBar::Impl::OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
{
	_hitTestFlag flag;
	int nIndex = _HitTestOnDragging(flag, point);

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

//---------------------------------------
DROPEFFECT CDonutTabBar::Impl::OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
{
	_ClearInsertionEdge();

	CSimpleArray<unique_ptr<TabItem> > arrItems;

	_hitTestFlag	flag;
	int nIndex = _HitTestOnDragging(flag, point);

	switch (flag) {
		case htInsetLeft:
			if ( m_bDragFromItself && (dropEffect & DROPEFFECT_MOVE) ) {							// just move items
				MoveItems(nIndex, m_arrCurDragItems);
			} else {
				OnNewTabCtrlItems(nIndex, arrItems, pDataObject, dropEffect);

				for (int i = 0; i < arrItems.GetSize(); ++i)
					_InsertItem(nIndex, std::move(arrItems[i]));
			}
			break;

		case htOneselfLeft:
			// 自分のタブの左にドロップされた
			if ( m_bDragFromItself && (dropEffect & DROPEFFECT_MOVE) ) {
				// just move items
				MoveItems(nIndex, m_arrCurDragItems);
			} else {
				OnNewTabCtrlItems(nIndex, arrItems, pDataObject, dropEffect);

				for (int i = 0; i < arrItems.GetSize(); ++i)
					_InsertItem(nIndex + 1, std::move(arrItems[i]));
			}
			break;

		case htOneselfRight:
			// 自分のタブの右にドロップされた
			if ( m_bDragFromItself && (dropEffect & DROPEFFECT_MOVE) ) {
				// just move items
				MoveItems(nIndex +1, m_arrCurDragItems);
			} else {
				OnNewTabCtrlItems(nIndex + 1, arrItems, pDataObject, dropEffect);

				for (int i = 0; i < arrItems.GetSize(); ++i)
					_InsertItem(nIndex + 1, std::move(arrItems[i]));
			}
			break;

		case htSeparator:
			if ( m_bDragFromItself && (dropEffect & DROPEFFECT_MOVE) ) {							// just move items
				MoveItems(nIndex + 1, m_arrCurDragItems);
			} else {
				OnNewTabCtrlItems(nIndex + 1, arrItems, pDataObject, dropEffect);

				for (int i = 0; i < arrItems.GetSize(); ++i)
					_InsertItem(nIndex + 1, std::move(arrItems[i]));
			}
			break;

		case htItem:
			if ( _IsSameIndexDropped(nIndex) == false ) 
			{
				DROPEFFECT dropEffectPrev = dropEffect;
				bool	   bDelSrc		  = OnDropTabCtrlItem(nIndex, pDataObject, dropEffect);

				if ( bDelSrc && m_bDragFromItself && (dropEffectPrev & DROPEFFECT_MOVE) ) { // just move items
					// do nothing
					for (int i = 0; i < m_arrCurDragItems.GetSize(); ++i) {
						OnDeleteItemDrag(m_arrCurDragItems[i]);
					}

					_DeleteItems(m_arrCurDragItems);
				}
			}
			break;

		case htOutside:
			if ( m_bDragFromItself && (dropEffect & DROPEFFECT_MOVE) ) {							// just move items to tail
				MoveItems(GetItemCount(), m_arrCurDragItems);
			} else {
				OnNewTabCtrlItems(GetItemCount(), arrItems, pDataObject, dropEffect);

				for (int i = 0; i < arrItems.GetSize(); ++i)
					_InsertItem(GetItemCount(), std::move(arrItems[i]));
			}
			break;

		default:
			break;
	}

	return dropEffect;
}

//------------------------------------------
bool CDonutTabBar::Impl::OnNewTabCtrlItems(
		int 							nInsertIndex,
		CSimpleArray<unique_ptr<TabItem> >&	/*items*/,
		IDataObject *					pDataObject,
		DROPEFFECT &					dropEffect)
{
	CSimpleArray<CString> arrUrls;

	if ( MtlGetDropFileName(pDataObject, arrUrls) ) {
		ATLTRACE(" HDROP found\n");
		m_bInsertHere  = true;
		m_nInsertIndex = nInsertIndex;
		UINT	size   = arrUrls.GetSize();
		for (UINT i = 0; i < size; ++i) {
			HWND hWnd  = DonutOpenFile(m_hWnd, arrUrls[i]);
			if (hWnd)
				++m_nInsertIndex;
		}

		m_nInsertIndex = -1;
		m_bInsertHere  = false;
		dropEffect	   = DROPEFFECT_COPY;
		return true;
	}

	CString 	strText;

	if ( MtlGetHGlobalText(pDataObject, strText)
	   || MtlGetHGlobalText( pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) )
	{
		m_bInsertHere  = true;
		m_nInsertIndex = nInsertIndex;
		DonutOpenFile(m_hWnd, strText);
		m_nInsertIndex = -1;
		m_bInsertHere  = false;
		dropEffect	   = DROPEFFECT_COPY;
		return true;
	}

	dropEffect = DROPEFFECT_NONE;
	return false;
}


//-------------------------------------------------
bool CDonutTabBar::Impl::OnDropTabCtrlItem(int nIndex, IDataObject *pDataObject, DROPEFFECT &dropEffect)
{
	HWND	hWnd = GetTabHwnd(nIndex);
	ATLASSERT( ::IsWindow(hWnd) );

	// first, get drop filename
	CSimpleArray<CString> arrFiles;

	if ( MtlGetDropFileName(pDataObject, arrFiles) ) {
		CWebBrowser2 browser = DonutGetIWebBrowser2(hWnd);

		if (browser.m_spBrowser != NULL) {
			CString strFile = arrFiles[0];

			if ( !MtlIsProtocol( strFile, _T("http") )
			   && !MtlIsProtocol( strFile, _T("https") ) )
			{
				if ( MtlPreOpenFile(strFile) )
					return false;

				// handled
			}

			MTL::ParseInternetShortcutFile(strFile);

			browser.Navigate2(strFile);
			dropEffect = DROPEFFECT_COPY;
			return false;
		}
	}

	// last, get text
	CString 	strText;

	if ( MtlGetHGlobalText(pDataObject, strText)
	   || MtlGetHGlobalText( pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) )
	{
		CWebBrowser2 browser = DonutGetIWebBrowser2(hWnd);

		if (browser.m_spBrowser != NULL) {
			browser.Navigate2(strText);
			dropEffect = DROPEFFECT_COPY;
			return false;
		}
	}

	dropEffect = (DROPEFFECT) -1;
	return false;
}


//-----------------------------------
void CDonutTabBar::Impl::OnDeleteItemDrag(int nIndex)
{
	HWND hWnd = GetTabHwnd(nIndex);
	if ( hWnd )
		::PostMessage( hWnd, WM_CLOSE, 0, 0 );
}




// Message map

//--------------------------------
/// ウィンドウの初期化
int		CDonutTabBar::Impl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);

	ATLVERIFY(RegisterDragDrop());

	WTL::CLogFont	lf;
	lf.SetMenuFont();
	m_font.CreateFontIndirect(&lf);

	m_menuPopup.LoadMenu(IDR_MENU_TAB);

	m_wndUpDown.Create(m_hWnd, CRect(0, 0, s_kcxUpDown, s_kcyUpDown), NULL, WS_CHILD | WS_OVERLAPPED | UDS_HORZ);
	m_wndDropBtn.Create(m_hWnd, CRect(0, 0, s_kcxUpDown, s_kcxUpDown), COMMAND_ID_DROPBUTTON);
	m_wndDropBtn.SetWindowPos(m_wndUpDown, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

	m_FaviconImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 200, 50);
	CIcon	icon = AtlLoadIcon(IDI_GENERIC_DOCUMENT);
	m_FaviconImageList.AddIcon(icon);

	_InitTooltip();

	// default size
	MoveWindow(0, 0, 200, _GetRequiredHeight(), FALSE);

	return 0;
}

//-------------------------------
void	CDonutTabBar::Impl::OnDestroy()
{
	RevokeDragDrop();

	m_imgs.Destroy();
	m_FaviconImageList.Destroy();
}

//-------------------------------
void	CDonutTabBar::Impl::OnSize(UINT nType, CSize size)
{
	DefWindowProc();

	m_wndUpDown.MoveWindow(size.cx - s_kcyUpDown * 3, size.cy - s_kcyUpDown, s_kcxUpDown, s_kcyUpDown);
	m_wndDropBtn.MoveWindow(size.cx - s_kcyUpDown, size.cy - s_kcyUpDown, s_kcyUpDown, s_kcyUpDown);
	//m_wndUpDown.MoveWindow(size.cx - s_kcxUpDown, size.cy - s_kcyUpDown, s_kcxUpDown, s_kcyUpDown);
	CRect rc(0, 0, s_kcyUpDown, s_kcyUpDown);
	m_wndDropBtn.InvalidateRect(&rc, TRUE);
	_ShowOrHideUpDownCtrl( CRect(0, 0, size.cx, size.cy) );

	if (s_bMultiLine) {
		_UpdateMultiLineLayout(size.cx);
		_RefreshBandInfo();
	}
}

//--------------------------------
/// for help message line
LRESULT CDonutTabBar::Impl::OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ATLASSERT( ::IsWindow(m_wndMDIChildPopuping.m_hWndMDIClient) );
	static HWND hWndToplevelWindow = CWindow(m_wndMDIChildPopuping.m_hWndMDIClient).GetTopLevelWindow();
	::SendMessage(hWndToplevelWindow, WM_MENUSELECT, wParam, lParam);
	return 0;
}

//---------------------------
// 左クリックDOWN : DragDrop開始
void	CDonutTabBar::Impl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nIndex = _HitTest(point);
	if (nIndex == -1)
		return;

	if (nFlags & MK_CONTROL) {	// 複数選択
		if ( GetCurSel() != nIndex ) {
			CSimpleArray<int>	arrIndex;
			GetCurMultiSel(arrIndex, false);
			if ( arrIndex.Find(nIndex) == -1 ) {	// 複数選択
				if ( m_vecpItem[nIndex]->ModifyState(TISS_SELECTED, TISS_MSELECTED) )
					InvalidateRect(m_vecpItem[nIndex]->rcItem);
			} else {								// 複数選択解除
				if ( m_vecpItem[nIndex]->ModifyState(TISS_MSELECTED, 0) )
					InvalidateRect(m_vecpItem[nIndex]->rcItem);
			}
		} else {
			_PressItem(nIndex);
		}

	} else if (nFlags & MK_SHIFT) {
		CSimpleArray<int>	arrIndex;
		GetCurMultiSel(arrIndex, false);
		if (arrIndex.GetSize() == 0) {	// アクティブなタブからnIndexまでを複数選択する
			if (nIndex < GetCurSel()) {
				for (int i = nIndex; i < GetCurSel(); ++i) {
					if ( m_vecpItem[i]->ModifyState(TISS_SELECTED, TISS_MSELECTED) )
						InvalidateRect(m_vecpItem[i]->rcItem);
				}
			} else {
				for (int i = GetCurSel() + 1; i <= nIndex; ++i) {
					if ( m_vecpItem[i]->ModifyState(TISS_SELECTED, TISS_MSELECTED) )
						InvalidateRect(m_vecpItem[i]->rcItem);
				}
			}
		} else {
			int nLastMultiSel = arrIndex[arrIndex.GetSize() - 1];
			if (nIndex < nLastMultiSel) {							// nIndexが複数選択タブの左側
				for (int i = nIndex; i < nLastMultiSel; ++i) {
					if (i != GetCurSel()) {
						if ( m_vecpItem[i]->ModifyState(TISS_SELECTED, TISS_MSELECTED) )
							InvalidateRect(m_vecpItem[i]->rcItem);
					}
				}
			} else {
				for (int i = arrIndex[0] + 1; i <= nIndex; ++i) {
					if (i != GetCurSel()) {
						if ( m_vecpItem[i]->ModifyState(TISS_SELECTED, TISS_MSELECTED) )
							InvalidateRect(m_vecpItem[i]->rcItem);
					}
				}
			}
		}
		
	} else {
		//デフォルトではDown時に切り替え release10β4
		//以前はUp時に切り替えでDownはD&Dの前段階という仕様
		if (s_bMouseDownSelect) {
			SetCurSel(nIndex, true);
		} else {
			_DoDragDrop(point, nFlags, nIndex);
		}
	}
}


//---------------------------
/// 右クリック : コマンド実行
void	CDonutTabBar::Impl::OnRButtonUp(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);

	auto funcUpdateMenu = [this](CMenuHandle &menuSys) {	// Emulation is needed cause MDI child window won't update its menu information until clicked.
		ATLASSERT( ::IsMenu(menuSys.m_hMenu) );

		// no effect
		// m_wndMDIChildPopuping.SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOACTIVATE |
		//	SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

		menuSys.EnableMenuItem(SC_RESTORE , MF_BYCOMMAND | MF_ENABLED);
		menuSys.EnableMenuItem(SC_MOVE	  , MF_BYCOMMAND | MF_ENABLED);
		menuSys.EnableMenuItem(SC_SIZE	  , MF_BYCOMMAND | MF_ENABLED);
		menuSys.EnableMenuItem(SC_MINIMIZE, MF_BYCOMMAND | MF_ENABLED);
		menuSys.EnableMenuItem(SC_MAXIMIZE, MF_BYCOMMAND | MF_ENABLED);

		CWindowPlacement wndpl;
		m_wndMDIChildPopuping.GetWindowPlacement(&wndpl);

		switch (wndpl.showCmd) {
		case SW_SHOWNORMAL:
			ATLTRACE2( atlTraceGeneral, 4, _T(" SW_SHOWNORMAL\n") );
			menuSys.EnableMenuItem(SC_RESTORE, MF_BYCOMMAND | MF_GRAYED);
			::SetMenuDefaultItem(menuSys, SC_CLOSE, FALSE);
			break;

		case SW_SHOWMAXIMIZED:
			ATLTRACE2( atlTraceGeneral, 4, _T(" SW_SHOWMAXIMIZED\n") );
			menuSys.EnableMenuItem(SC_MOVE	  , MF_BYCOMMAND | MF_GRAYED);
			menuSys.EnableMenuItem(SC_SIZE	  , MF_BYCOMMAND | MF_GRAYED);
			menuSys.EnableMenuItem(SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);
			::SetMenuDefaultItem(menuSys, SC_CLOSE, FALSE);
			break;

		case SW_SHOWMINIMIZED:
			ATLTRACE2( atlTraceGeneral, 4, _T(" SW_SHOWMINIMIZED\n") );
			menuSys.EnableMenuItem(SC_SIZE	  , MF_BYCOMMAND | MF_GRAYED);
			menuSys.EnableMenuItem(SC_MINIMIZE, MF_BYCOMMAND | MF_GRAYED);
			::SetMenuDefaultItem(menuSys, SC_RESTORE, FALSE);
			break;

		default:
			ATLASSERT(FALSE);
			break;
		}

		if ( m_wndMDIChildPopuping.m_hWnd != m_wndMDIChildPopuping.MDIGetActive() ) {	// it's not active
			menuSys.EnableMenuItem(SC_MOVE, MF_BYCOMMAND | MF_GRAYED);
			menuSys.EnableMenuItem(SC_SIZE, MF_BYCOMMAND | MF_GRAYED);
		}
	};

	int nIndex = _HitTest(point);
	if (nIndex != -1) {
		HWND hWndChild = GetTabHwnd(nIndex);
		ATLASSERT(hWndChild != NULL);

		if (s_dwExStyle & MTB_EX_RIGHTCLICKCLOSE) {
			::PostMessage(hWndChild, WM_CLOSE, 0, 0);
		} else if (s_dwExStyle & MTB_EX_RIGHTCLICKREFRESH) {
			::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_VIEW_REFRESH, 0);
		} else if ( (s_dwExStyle & MTB_EX_RIGHTCLICKCOMMAND) && s_RClickCommand ) {		//minit
			::PostMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM) s_RClickCommand, 0);
		} else if (m_menuPopup.m_hMenu) {
			ClientToScreen(&point);
			//CMenuHandle menu	 = m_menuPopup.GetSubMenu(0);

			DWORD	dwFlag = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD;
			if (CMenuOption::s_bR_Equal_L) 
				dwFlag |= TPM_RIGHTBUTTON;

			CMenuHandle menu = CCustomContextMenuOption::s_menuTabItem;
			CSimpleArray<HMENU>	arrDestroyMenu;
			CCustomContextMenuOption::AddSubMenu(menu, GetTopLevelWindow(), arrDestroyMenu);
			// ポップアップメニューを表示する
			int nCmd = menu.TrackPopupMenu(dwFlag, point.x, point.y, hWndChild);
			if (nCmd != 0) {
				if (nCmd == ID_FILE_CLOSE) {	// 場合によっては複数選択されたタブも閉じる
					CSimpleArray<int>	arr;
					GetCurMultiSelEx(arr, nIndex);
					int nCount = arr.GetSize();
					for (int i = 0; i < nCount; ++i) {
						::PostMessage(GetTabHwnd(arr[i]), WM_CLOSE, 0, 0);
					}
				} else 
					MtlSendCommand(hWndChild, nCmd);
			}

			CCustomContextMenuOption::RemoveSubMenu(menu, arrDestroyMenu);

		} else {																				// system menu (default)
			CMenuHandle menuSys = ::GetSystemMenu(hWndChild, FALSE);
			ClientToScreen(&point);
			m_wndMDIChildPopuping = hWndChild;
			funcUpdateMenu(menuSys);
			menuSys.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
								   point.x, point.y, m_hWnd);									// owner is me!!
		}
	} else {
		::SendMessage(GetTopLevelParent(), WM_SHOW_TOOLBARMENU, 0, 0);
	}
}

//---------------------------
/// ダブルクリック : コマンド実行
void	CDonutTabBar::Impl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);

	CSimpleArray<int> arrCurMultiSel;
	HWND			  hWndChild;
	int	nIndex = _HitTest(point);
	GetCurMultiSelEx(arrCurMultiSel, nIndex);

	if (arrCurMultiSel.GetSize() > 1) {
		for (int i = 0; i < arrCurMultiSel.GetSize(); ++i) {
			hWndChild = GetTabHwnd(arrCurMultiSel[i]);

			if (s_dwExStyle & MTB_EX_DOUBLECLICKCLOSE)
				::PostMessage(hWndChild, WM_CLOSE, 0, 0);
			else if (s_dwExStyle & MTB_EX_DOUBLECLICKREFRESH)
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_VIEW_REFRESH, 0);
			else if (s_dwExStyle & MTB_EX_DOUBLECLICKNLOCK)								// UDT DGSTR
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_DOCHOSTUI_OPENNEWWIN, 0);
			else if ( (s_dwExStyle & MTB_EX_DOUBLECLICKCOMMAND) && s_DClickCommand ) 	//minit
				::PostMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM) s_DClickCommand, 0);
		}
	} else {
		int nIndex = _HitTest(point);
		if (nIndex != -1) {
			hWndChild = GetTabHwnd(nIndex);

			if (s_dwExStyle & MTB_EX_DOUBLECLICKCLOSE)
				::PostMessage(hWndChild, WM_CLOSE, 0, 0);
			else if (s_dwExStyle & MTB_EX_DOUBLECLICKREFRESH)
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_VIEW_REFRESH, 0);
			else if (s_dwExStyle & MTB_EX_DOUBLECLICKNLOCK)								// UDT DGSTR
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_DOCHOSTUI_OPENNEWWIN, 0);
			else if ( (s_dwExStyle & MTB_EX_DOUBLECLICKCOMMAND) && s_DClickCommand ) 	//minit
				::PostMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM) s_DClickCommand, 0);
		}
	}
}

//---------------------------
/// ミドルクリック : コマンド実行
void	CDonutTabBar::Impl::OnMButtonUp(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);

	CSimpleArray<int> arrCurMultiSel;
	HWND			  hWndChild;
	int	nIndex = _HitTest(point);
	GetCurMultiSelEx(arrCurMultiSel, nIndex);

	if (arrCurMultiSel.GetSize() > 1) {
		for (int i = 0; i < arrCurMultiSel.GetSize(); ++i) {
			hWndChild = GetTabHwnd(arrCurMultiSel[i]);

			if (s_dwExStyle & MTB_EX_XCLICKCLOSE)
				::PostMessage(hWndChild, WM_CLOSE, 0, 0);
			else if (s_dwExStyle & MTB_EX_XCLICKREFRESH)
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_VIEW_REFRESH, 0);
			else if (s_dwExStyle & MTB_EX_XCLICKNLOCK)									// UDT DGSTR
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_DOCHOSTUI_OPENNEWWIN, 0);
			else if ( (s_dwExStyle & MTB_EX_XCLICKCOMMAND) && s_MClickCommand )			//minit
				::PostMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM) s_MClickCommand, 0);
		}
	} else {
		int nIndex = _HitTest(point);

		if (nIndex != -1) {
			hWndChild = GetTabHwnd(nIndex);

			if (s_dwExStyle & MTB_EX_XCLICKCLOSE)
				::PostMessage(hWndChild, WM_CLOSE, 0, 0);
			else if (s_dwExStyle & MTB_EX_XCLICKREFRESH)
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_VIEW_REFRESH, 0);
			else if (s_dwExStyle & MTB_EX_XCLICKNLOCK)									// UDT DGSTR
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_DOCHOSTUI_OPENNEWWIN, 0);
			else if ( (s_dwExStyle & MTB_EX_XCLICKCOMMAND) && s_MClickCommand )			//minit
				::PostMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM) s_MClickCommand, 0);
		}
	}
}

//--------------------------
void	CDonutTabBar::Impl::OnCommand(UINT wNotifyCode, int wID, HWND hwndCtl)
{
	if (m_wndMDIChildPopuping.m_hWnd == NULL) { 		// this is not my command message
		SetMsgHandled(FALSE);
		return;
	}

	ATLASSERT( m_wndMDIChildPopuping.IsWindow() );
	CMDIWindow		wnd   = m_wndMDIChildPopuping;
	m_wndMDIChildPopuping = NULL;		// It must be reset cause SendMessage(WM_SYSCOMMAND) make one more WM_COMMAND.

	if (wID != SC_CLOSE && wID != SC_NEXTWINDOW)
		wnd.BringWindowToTop();

	// Note: If you send SC_NEXTWINDOW to inactive child window,
	//		 order is broken. Tell me why.
	if (wID == SC_NEXTWINDOW)
		wnd.MDINext(NULL);					// I think NULL is natural.
	else if (wID == SC_MAXIMIZE)
		wnd.ShowWindow(SW_MAXIMIZE);		// without this, frame window maximized. why?
	//else if (wID == SC_CLOSE)
	//	wnd.PostMessage(WM_CLOSE);			// without this, debug assertion occurs?
	else
		wnd.SendMessage(WM_SYSCOMMAND, (WPARAM) wID);
}

//----------------------------
void	CDonutTabBar::Impl::OnPushDropButton(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CRect rc;

	m_wndDropBtn.GetClientRect(&rc);
	m_wndDropBtn.ClientToScreen(&rc);
	int nIndex = ShowTabListMenuVisible(rc.left, rc.top);
	if (nIndex == -1)
		return;

	HWND  hChild = GetTabHwnd(nIndex);
	ATLASSERT(::IsWindow(hChild));

	::SendMessage(m_wndMDIChildPopuping.m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM) hChild, 0);
}

//-----------------------------
/// タブにFaviconを設定する
void	CDonutTabBar::Impl::OnSetFaviconImage(HWND hWnd, HICON hIcon)
{
	int nIndex = GetTabIndex(hWnd);
	_ReplaceFavicon(nIndex, hIcon);

	if (nIndex == GetCurSel())
		CDonutAddressBar::GetInstance()->ReplaceIcon(hIcon);
}

//-------------------------
LRESULT CDonutTabBar::Impl::OnDeltaPos(LPNMHDR pnmh)
{
	LPNMUPDOWN lpnmud = (LPNMUPDOWN) pnmh;

	_ScrollItem(lpnmud->iDelta > 0);

	return 0;
}

//--------------------------------
/// ツールチップテキストを設定
LRESULT CDonutTabBar::Impl::OnGetDispInfo(LPNMHDR pnmh)
{
	LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO) pnmh;
	CPoint		   ptCursor;
	::GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);
	int nIndex = _HitTest(ptCursor);
	if ( _IsValidIndex(nIndex) ) {
		m_strTooltipText.Empty();
		HWND	hWnd = GetTabHwnd(nIndex);
		if (hWnd) {
			CWebBrowser2 browser = DonutGetIWebBrowser2(hWnd);
			if ( browser.IsBrowserNull() == false ) {
				CString   strName = MtlGetWindowText(hWnd);
				CString   strUrl  = browser.GetLocationURL();
				m_strTooltipText = strName + _T("\n") + strUrl;
			}
		}
		pDispInfo->lpszText = m_strTooltipText.GetBuffer(0);
	} else {
		pDispInfo->lpszText = NULL;
	}

	m_ToolTip.SetDelayTime( TTDT_INITIAL, m_ToolTip.GetDelayTime(TTDT_RESHOW) );

	return 0;
}


// CChildFrameから

//------------------------------
/// タブのテキストを更新
int		CDonutTabBar::Impl::OnMDISetText(LPCTSTR lpstrText)
{
	SetMsgHandled(FALSE);
	// for new text
	int nIndex = GetTabIndex(m_wndMDIChildProcessing);
	if (nIndex != -1)
		_SetTabText(nIndex, lpstrText);

	return 0;
}

//------------------------------
void	CDonutTabBar::Impl::OnMDIActivate(HWND hWndChildDeact, HWND hWndChildAct)
{
	SetMsgHandled(FALSE);

	if (hWndChildAct == NULL)
		return;

	int nIndex = GetTabIndex(hWndChildAct);
	ATLASSERT(nIndex != -1);

	if (!m_bRedrawLocked) {
		SetItemActive(nIndex);
		UpdateWindow();
	}

	SetCurSel(nIndex, false, true);
}

//------------------------------
/// 
void	CDonutTabBar::Impl::OnSetCurSel(int nIndex, int nOldIndex)
{
	HWND	hWnd = GetTabHwnd(nIndex);
	ATLASSERT( ::IsWindow(hWnd) );

	if (Misc::IsGpuRendering()) {
		/* 前のウィンドウの画面を更新しておく */
		HWND	hWndOld = GetTabHwnd(nOldIndex);
		if (hWndOld) {
			CRect rcOldWnd;
			::GetClientRect(hWndOld, &rcOldWnd);
			::InvalidateRect(hWndOld, &rcOldWnd, FALSE);
			::UpdateWindow(hWndOld);
		}

		CWindow wndMDI(m_wndMDIChildPopuping.m_hWndMDIClient);
		wndMDI.SetRedraw(FALSE);

		m_wndMDIChildPopuping.MDIActivate(hWnd);

		wndMDI.SetRedraw(TRUE);
		::RedrawWindow(m_wndMDIChildPopuping.m_hWndMDIClient, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
	} else {
	#if 1 //+++ メモ:unDonut+
		CWindow wndMDI(m_wndMDIChildPopuping.m_hWndMDIClient);
		wndMDI.SetRedraw(FALSE);

		m_wndMDIChildPopuping.MDIActivate(hWnd);

		wndMDI.SetRedraw(TRUE);
		wndMDI.RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

	#else	//+++ メモ:r13test	//*お試し
		m_wndMDIChildPopuping.MDIActivate(hWnd);
	#endif
	}
}


//-------------------------------------
HRESULT CDonutTabBar::Impl::OnGetTabCtrlDataObject(CSimpleArray<int>& arrIndex, IDataObject** ppDataObject)
{
	ATLASSERT(ppDataObject != NULL);
	ATLASSERT(arrIndex.GetSize() > 0);

	HRESULT hr	= CHlinkDataObject::_CreatorClass::CreateInstance(NULL,IID_IDataObject, (void **) ppDataObject);
	if ( FAILED(hr) ) {
		*ppDataObject = NULL;
		return E_NOTIMPL;
	}

  #ifdef _ATL_DEBUG_INTERFACES
	ATLASSERT( FALSE && _T("_ATL_DEBUG_INTERFACES crashes the following\n") );
  #endif

	CHlinkDataObject *pHlinkDataObject = NULL;		// this is hack, no need to release

	hr = (*ppDataObject)->QueryInterface(IID_NULL, (void **) &pHlinkDataObject);
	if ( SUCCEEDED(hr) ) {
		for (int i = 0; i < arrIndex.GetSize(); ++i) {
			int 		 nIndex  = arrIndex[i];
			HWND		 hWnd	 = GetTabHwnd(nIndex);
			ATLASSERT( ::IsWindow(hWnd) );
			CWebBrowser2 browser = (IWebBrowser2 *) ::SendMessage(hWnd, WM_USER_GET_IWEBBROWSER, 0, 0);
			CString 	 strName = MtlGetWindowText(hWnd);
			CString 	 strUrl  = browser.GetLocationURL();

			if ( strUrl.Left(5) == _T("file:") ) {	// Donut, to be explorer or not
				strName.Empty();
				strUrl = strUrl.Right(strUrl.GetLength() - 8);
				strUrl.Replace( _T('/'), _T('\\') );
			}

			pHlinkDataObject->m_arrNameAndUrl.Add( std::make_pair(strName, strUrl) );
		}
	}

	return S_OK;
}



//------------------------------
/// ツールチップの作成＆初期化
void	CDonutTabBar::Impl::_InitTooltip()
{
	bool bUseTheme = false;
	// comctl32のバージョンを取得
    HINSTANCE         hComCtl;
    HRESULT           hr;
    DLLGETVERSIONPROC pDllGetVersion;
	DLLVERSIONINFO    dvi = { sizeof(DLLVERSIONINFO) };
    hComCtl = LoadLibrary(_T("comctl32.dll"));
    if (hComCtl) {
        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hComCtl, "DllGetVersion");
        if (pDllGetVersion) {
            hr = (*pDllGetVersion)(&dvi);
            if (SUCCEEDED(hr) && dvi.dwMajorVersion >= 6) {
				bUseTheme = true;
			}
        }
    }
    FreeLibrary(hComCtl);


	// create a tool tip
	m_ToolTip.Create(m_hWnd);
	ATLASSERT( m_ToolTip.IsWindow() );
	CToolInfo tinfo(TTF_SUBCLASS, m_hWnd);			
	if (bUseTheme == false)
		tinfo.cbSize = sizeof(TOOLINFO) - sizeof(void*);
	tinfo.hwnd = m_hWnd;		// WTL always sucks...
	m_ToolTip.AddTool(tinfo);
	m_ToolTip.SetMaxTipWidth(SHRT_MAX);
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 30 * 1000);
}

//------------------------------
/// タブバーが必要とする高さを返す
int		CDonutTabBar::Impl::_GetRequiredHeight()
{
	size_t size = m_vecpItem.size();	// = GetItemCount();
	if (size == 0) {
		return _GetItemHeight();
	} else {
		return m_vecpItem[size - 1]->rcItem.bottom;
	}
}

//------------------------------
/// タブ1個の高さを返す
int		CDonutTabBar::Impl::_GetItemHeight()
{
	if (s_bUseFixedSize) {
		return s_FixedSize.cy;
	} else {
		int nFontHeight = GetFontHeight(m_font);
		nFontHeight += s_kcyTextMargin * 2;
		return nFontHeight;
	}
}

//------------------------------
/// nIndexが有効かどうかを返す
bool	CDonutTabBar::Impl::_IsValidIndex(int nIndex) const
{
	return 0 <= nIndex && nIndex < (int)m_vecpItem.size();
}

//------------------------------
/// ptの位置にあるタブのインデックスを返す
int		CDonutTabBar::Impl::_HitTest(const CPoint& pt)
{
	CRect	rc;
	GetClientRect(&rc);
	if ( !rc.PtInRect(pt) )
		return -1;

	int	i = s_bMultiLine ? 0 : m_nFirstIndexOnSingleLine;
	for (; i < GetItemCount(); ++i) {
		if (m_vecpItem[i]->rcItem.PtInRect(pt))
			return i;
	}

	return -1;
}


//------------------------------
/// タブのテキストを更新する
void	CDonutTabBar::Impl::_SetTabText(int nIndex, LPCTSTR lpszTab)
{
	ATLASSERT(_IsValidIndex(nIndex));

	CString strTab =lpszTab;
	strTab = MtlCompactString(strTab, s_nMaxTextLength);

	if (m_vecpItem[nIndex]->strItem == strTab)
		return ;

	m_vecpItem[nIndex]->strItem = strTab;

	if (s_bUseFixedSize) {
		InvalidateRect(m_vecpItem[nIndex]->rcItem);
		UpdateWindow();
	} else {
		InvalidateRect(m_vecpItem[nIndex]->rcItem);	// even if layout will not be changed
		_UpdateLayout();							//_UpdateItems(nIndex);
	}
}

//-----------------------------------
/// リバーを更新する
void	CDonutTabBar::Impl::_RefreshBandInfo()
{
	if (m_bLockRefreshBandInfo)
		return;
	HWND		  hWndReBar = GetParent();
	CReBarCtrl	  rebar(hWndReBar);

	static UINT wID = 0;
	if (wID == 0) {
		REBARBANDINFO rb = { sizeof (REBARBANDINFO) };
		rb.fMask	= RBBIM_CHILD | RBBIM_ID;
		int nCount	= rebar.GetBandCount();
		for (int i = 0; i < nCount; ++i) {
			rebar.GetBandInfo(i, &rb);
			if (rb.hwndChild == m_hWnd) {
				wID = rb.wID;
				break;
			}
		}
		return;
	}

	int	nIndex = rebar.IdToIndex( wID );
	if ( nIndex == -1 ) 
		return;				// 設定できないので帰る

	REBARBANDINFO rbBand = { sizeof (REBARBANDINFO) };
	rbBand.fMask  = RBBIM_CHILDSIZE;
	rebar.GetBandInfo(nIndex, &rbBand);
	if ( rbBand.cyMinChild != _GetRequiredHeight() ) {
		// Calculate the size of the band
		rbBand.cxMinChild = 0;
		rbBand.cyMinChild = _GetRequiredHeight();

		rebar.SetBandInfo(nIndex, &rbBand);
	}
}

//-----------------------------------
/// シングルラインレイアウト
void	CDonutTabBar::Impl::_UpdateSingleLineLayout()
{
	ATLASSERT( _IsValidIndex(m_nFirstIndexOnSingleLine) );
	m_arrSeparators.RemoveAll();

	// clean invisible items
	int nCount = GetItemCount();
	for (int i = 0; i < m_nFirstIndexOnSingleLine && i < nCount; ++i) {
		m_vecpItem[i]->rcItem.SetRectEmpty();
	}

	int cxOffset = 0;	//s_kcxGap;

	for (int i = m_nFirstIndexOnSingleLine; i < nCount; ++i) {
		TabItem& item	= *m_vecpItem[i];
		CRect	rcSrc	= item.rcItem;

		// update the rect
		if (s_bUseFixedSize) {
			item.rcItem = CRect(cxOffset, 0, cxOffset + s_FixedSize.cx, s_FixedSize.cy);
		} else {
			item.rcItem = _MeasureItem(m_vecpItem[i]->strItem) + CPoint(cxOffset, 0);
		}

		cxOffset = cxOffset + item.rcItem.Width();

		m_arrSeparators.Add( CPoint(cxOffset, 0) );

		if (rcSrc != item.rcItem) {
			InvalidateRect( _InflateGapWidth(rcSrc) );
			InvalidateRect( _InflateGapWidth(item.rcItem) );
		}
	}
}

//------------------------------------
/// 複数ラインレイアウト
void	CDonutTabBar::Impl::_UpdateMultiLineLayout(int nWidth)
{
	m_arrSeparators.RemoveAll();

	int cxOffset = 0;
	int cyOffset = 0;

	int nCount = GetItemCount();
	for (int i = 0; i < nCount; ++i) {
		TabItem& item	= *m_vecpItem[i];
		CRect  rcSrc	= item.rcItem;

		// update the rect
		if (s_bUseFixedSize)
			item.rcItem = CRect(cxOffset, cyOffset, cxOffset + s_FixedSize.cx, cyOffset + s_FixedSize.cy);
		else
			item.rcItem = _MeasureItem(item.strItem) + CPoint(cxOffset, cyOffset);

		if (i != 0 && item.rcItem.right > nWidth) { // do wrap
			cxOffset  = 0;
			cyOffset += _GetItemHeight() + s_kcyGap;

			if (s_bUseFixedSize)
				item.rcItem = CRect(cxOffset, cyOffset, cxOffset + s_FixedSize.cx, cyOffset + s_FixedSize.cy);
			else
				item.rcItem = _MeasureItem(item.strItem) + CPoint(cxOffset, cyOffset);
		}

		cxOffset = cxOffset + item.rcItem.Width();

		m_arrSeparators.Add( CPoint(cxOffset, cyOffset) );

		if (rcSrc != item.rcItem) {
			InvalidateRect( _InflateGapWidth(rcSrc) );
			InvalidateRect( _InflateGapWidth(item.rcItem) );
		}
	}
}

//-------------------------------
/// タブの配置を更新する
void	CDonutTabBar::Impl::_UpdateLayout()
{
	if (m_vecpItem.size() == 0) {
		m_arrSeparators.RemoveAll();
		Invalidate();

		_RefreshBandInfo();

	} else {

		CRect		rc;
		GetClientRect(&rc);

		if (s_bMultiLine) 
			_UpdateMultiLineLayout( rc.Width() );
		else
			_UpdateSingleLineLayout();
		
		_ShowOrHideUpDownCtrl(rc);

		_RefreshBandInfo();
		UpdateWindow();
	}
}

//-------------------------------------
///       ↓ここの分プラスする
/// | tab |
CRect	CDonutTabBar::Impl::_InflateGapWidth(const CRect& rc) const
{
	int cxSeparatorOffset = s_kcxGap * 2 + s_kcxSeparator;

	return CRect(rc.left, rc.top, rc.right + cxSeparatorOffset, rc.bottom);
}

//-------------------------------------
void	CDonutTabBar::Impl::_ShowOrHideUpDownCtrl(const CRect& rcClient)
{
	int nCount = GetItemCount();
	if ( s_bMultiLine || nCount == 0 ) {
		m_wndDropBtn.ShowWindow(SW_HIDE);
		m_wndUpDown.ShowWindow(SW_HIDE);
		return;
	}

	m_wndUpDown.SetRange( 0, nCount );

	if (   m_nFirstIndexOnSingleLine != 0
		|| rcClient.right < m_vecpItem[nCount - 1]->rcItem.right)
	{
		m_wndDropBtn.ShowWindow(SW_SHOWNORMAL);
		m_wndUpDown.ShowWindow(SW_SHOWNORMAL);
	} else {
		m_wndDropBtn.ShowWindow(SW_HIDE);
		m_wndUpDown.ShowWindow(SW_HIDE);
	}
}

//-------------------------------------
void	CDonutTabBar::Impl::_ScrollOpposite(int nNewSel, bool bClicked)
{
	ATLASSERT( _IsValidIndex(nNewSel) );

	if (   GetItemCount() < 2
		|| s_bMultiLine
		|| m_wndUpDown.IsWindowVisible() == false)
		return;

	const CRect& rcItem = m_vecpItem[nNewSel]->rcItem;

	CRect	rcClient;
	GetClientRect(rcClient);
	if (bClicked) {
		if (m_nFirstIndexOnSingleLine != nNewSel) {
			CRect rcUpDown;
			m_wndUpDown.GetWindowRect(rcUpDown);
			ScreenToClient(rcUpDown);
			if ( MtlIsCrossRect(rcItem, rcUpDown) )
				_ScrollItem();
		}
	} else if (rcItem.IsRectEmpty() || (rcClient & rcItem) != rcItem) { // outside or crossed
		//+++ m_nFirstIndexOnSingleLine = MtlMax(0, nNewSel - 1);			//nNewSel;
		m_nFirstIndexOnSingleLine = std::max(0, nNewSel - 1);			//nNewSel;
		_UpdateSingleLineLayout();
		Invalidate();
		_ShowOrHideUpDownCtrl(rcClient);
	}
}

//------------------------------------
void	CDonutTabBar::Impl::_HotItem(int nNewHotIndex)
{
	// clean up
	if ( _IsValidIndex(m_nHotIndex) ) {
		TabItem& item = *m_vecpItem[m_nHotIndex];

		if ( item.ModifyState(TISS_HOT, 0) )
			InvalidateRect(item.rcItem);
	}

	m_nHotIndex = nNewHotIndex;

	if ( _IsValidIndex(m_nHotIndex) ) {
		TabItem& item = *m_vecpItem[m_nHotIndex];

		if ( item.ModifyState(0, TISS_HOT) )
			InvalidateRect(item.rcItem);
	}
}

//------------------------------------
void	CDonutTabBar::Impl::_PressItem(int nPressedIndex)
{
	// clean up prev
	if ( _IsValidIndex(m_nPressedIndex) ) {
		TabItem& item = *m_vecpItem[m_nPressedIndex];

		if ( item.ModifyState(TISS_PRESSED, 0) )
			InvalidateRect(item.rcItem);
	}

	m_nPressedIndex = nPressedIndex;

	if ( _IsValidIndex(m_nPressedIndex) ) {
		TabItem& item = *m_vecpItem[m_nPressedIndex];

		if ( item.ModifyState(0, TISS_PRESSED) )
			InvalidateRect(item.rcItem);
	}
}

//--------------------------------------
/// 再描写を抑制する
void	CDonutTabBar::Impl::_LockRedraw(bool bLock)
{
	if (bLock) {
		m_bRedrawLocked = true;
	} else {
		m_bRedrawLocked = false;
		int nIndex = GetCurSel();
		//if ( m_wndMDIChildPopuping.MDIGetActive() == GetTabHwnd(nIndex) )
			//SetItemActive(nIndex);
	}
}

//--------------------------------------
/// タブの大きさを返す
CRect	CDonutTabBar::Impl::_MeasureItem(const CString& strText)
{
	if (s_bUseFixedSize)
		return CRect(0, 0, s_FixedSize.cx, s_FixedSize.cy);

	// compute size of text - use DrawText with DT_CALCRECT
	int cx = MtlComputeWidthOfText(strText, m_font);
	// width is width of text plus a bunch of stuff
	cx += s_kcxTextMargin * 2;	// L/R margin for readability

	int	cy = GetFontHeight(m_font);
	cy += s_kcyTextMargin * 2;
	// height of item is the bigger of these two
	cy	= std::max(cy, 16);

	return CRect(0, 0, cx, cy);
}


//--------------------------------------
void	CDonutTabBar::Impl::_SetItemImageIndex(int nIndex, int nImgIndex)
{
	ATLASSERT(_IsValidIndex(nIndex));

	if (m_vecpItem[nIndex]->nImgIndex == nImgIndex)
		return ;

	m_vecpItem[nIndex]->nImgIndex = nImgIndex;

	// imagelist does'nt change item rect
	InvalidateRect(m_vecpItem[nIndex]->rcItem);
	UpdateWindow();
}

//--------------------------------------
/// リンクを開くときtrueを返す
bool CDonutTabBar::Impl::_IsLinkOpen()
{
	bool ret = false;

	if (m_LinkState == LINKSTATE_A_ON || m_LinkState == LINKSTATE_B_ON)
		ret = true;

	if (m_LinkState == LINKSTATE_A_ON)
		m_LinkState = LINKSTATE_OFF;

	return ret;
}

//-------------------------------
int		CDonutTabBar::Impl::_GetFirstVisibleIndex()
{
	if (s_bMultiLine) {
		return 0;
	} else {
		CRect rc;
		GetClientRect(&rc);

		int nCount = GetItemCount();
		for (int i = 0; i < nCount; ++i) {
			if ( MtlIsCrossRect(m_vecpItem[i]->rcItem, &rc) ) {
				return i;
			}
		}
	}

	return -1;
}

//--------------------------
int		CDonutTabBar::Impl::_GetLastVisibleIndex()
{
	if (s_bMultiLine) {
		return GetItemCount() - 1;
	} else {
		CRect rc;
		GetClientRect(&rc);

		for (int i = GetItemCount() - 1; i >= 0; i--) {
			if ( MtlIsCrossRect(m_vecpItem[i]->rcItem, &rc) ) {
				return i;
			}
		}
	}

	return -1;
}

//--------------------------------------
bool	CDonutTabBar::Impl::_InsertItem(int nIndex, unique_ptr<TabItem>&& item)
{
	// if nDestIndex is invalid, added to tail
	_HotItem();

	#if 1 //+++
	int size = GetItemCount();
	if (size == 0) {
		m_vecpItem.push_back(std::move(item));
	} else {
		if ( nIndex < 0 || nIndex > size ) {
			nIndex = size;
		}
		m_vecpItem.insert(m_vecpItem.begin() + nIndex, std::move(item));
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

//----------------------------------------
bool	CDonutTabBar::Impl::_DeleteItem(int nIndex, bool bMoveNow/* = false*/)
{
	if ( !_IsValidIndex(nIndex) )
		return false;

	_HotItem();

	// first, post it
	if (bMoveNow == false)
		InvalidateRect( _InflateGapWidth(m_vecpItem[nIndex]->rcItem) );
	//		_InvalidateItemsRect(nIndex, m_items.size());

	if ( nIndex < m_nFirstIndexOnSingleLine || nIndex + 1 == GetItemCount() ) {
		--m_nFirstIndexOnSingleLine;

		if (m_nFirstIndexOnSingleLine < 0)
			m_nFirstIndexOnSingleLine = 0;
	}
	if (bMoveNow == false) {
		int nRemovedIndex = m_vecpItem[nIndex]->nFaviconIndex;
		if (nRemovedIndex != -1) {
			m_FaviconImageList.Remove(nRemovedIndex);
			for (auto it = m_vecpItem.begin(); it != m_vecpItem.end(); ++it) {
				if (nRemovedIndex < (*it)->nFaviconIndex)
					(*it)->nFaviconIndex--;
			}
		}
	}

	m_vecpItem.erase(m_vecpItem.begin() + nIndex);

	_UpdateLayout();
	return true;
}

//----------------------------------------
void	CDonutTabBar::Impl::_DeleteItems(CSimpleArray<int>& arrSrcs)
{
	for (int i = arrSrcs.GetSize() - 1; i >= 0; --i) {
		int nIndex = arrSrcs[i];
		if ( !_IsValidIndex(nIndex) )
			continue;

		_DeleteItem(nIndex);
	}
}

//-----------------------------------------
void	CDonutTabBar::Impl::_ReplaceFavicon(int nIndex, HICON hIcon)
{
	if ( _IsValidIndex(nIndex) == false)
		return;

	int& nFaviconIndex = m_vecpItem[nIndex]->nFaviconIndex;
	if (hIcon == NULL && nFaviconIndex != -1) {
		m_FaviconImageList.Remove(nFaviconIndex);
		for (auto it = m_vecpItem.begin(); it != m_vecpItem.end(); ++it) {
			if (nFaviconIndex < (*it)->nFaviconIndex)
				(*it)->nFaviconIndex--;
		}
		nFaviconIndex = -1;
	} else if (nFaviconIndex != -1) {
		m_FaviconImageList.ReplaceIcon(nFaviconIndex, hIcon);
	} else {
		nFaviconIndex = m_FaviconImageList.AddIcon(hIcon);
	}
	InvalidateRect(m_vecpItem[nIndex]->rcItem, FALSE);
}


// Drag Drop

//------------------------------------------------
/// Dragしているタブとその左右にカーソルがあるか
int CDonutTabBar::Impl::_HitTestCurTabSide(CPoint point, _hitTestFlag &flag)
{
	// Dragされているタブが一つなら
	if (m_arrCurDragItems.GetSize() == 1) {
		int nCurDragItemIndex = m_arrCurDragItems[0];

		/* Dragされているタブの上にカーソルがあるかどうか */
		if ( m_vecpItem[nCurDragItemIndex]->rcItem.PtInRect(point) ) {
			flag = htOneself;	// 自分のタブの上にポインタがあるので無効にする
			return 0;
		}
		int	nTop = 0;
		/* Dragされているタブの左のタブの上にカーソルがあるかどうか */
		if (_IsValidIndex(nCurDragItemIndex -1)) {
			const CRect &rcLeft = m_vecpItem[nCurDragItemIndex -1]->rcItem;
			nTop = rcLeft.top;
			if (rcLeft.PtInRect(point)) {
				// 左のタブ上にある
				flag = htOneselfLeft;
				return nCurDragItemIndex -1;
			}
		}

		/* Dragされているタブの右のタブの上にカーソルがあるかどうか */
		if (_IsValidIndex(nCurDragItemIndex +1)) {
			const CRect &rcRight = m_vecpItem[nCurDragItemIndex +1]->rcItem;
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

//-----------------------------------------
/// どのタブの間にカーソルがあるかどうか
int CDonutTabBar::Impl::_HitTestSeparator(CPoint point, vector<std::pair<int, CRect> >& vecRect/*, int nInset*/)
{
	int   cyGap = _GetItemHeight();
	int	cyOffset = -1;
	int   i 	= s_bMultiLine ? 0 : m_nFirstIndexOnSingleLine;
	int nCount = GetItemCount();
	for (; i < nCount; ++i) {
		const CRect &rcItem = m_vecpItem[i]->rcItem;

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
		rcSep.InflateRect(s_nTabSeparatorWidth, 0);

		if ( rcSep.PtInRect(point) ) {
			return i;	// between items hit!
		}
	}

	return -1;	// not inside of the gaps
}

//----------------------------------------------
/// タブバーの左側上にカーソルがあるかどうか
int CDonutTabBar::Impl::_HitTestLeftSide(CPoint point, vector<std::pair<int, CRect> >& vecRect)
{
	int nCount = (int)vecRect.size();
	for (int i = 0; i < nCount; ++i) {
		vecRect[i].second.left = 0;
		vecRect[i].second.right = s_nTabSeparatorWidth;
		if (vecRect[i].second.PtInRect(point)) {
			return vecRect[i].first;
		}
	}
	return -1;
}

//----------------------------------------------
/// OnMouseMove(OnTrackMouseMove)用のHitTest
int CDonutTabBar::Impl::_HitTestOnDragging(_hitTestFlag &flag, CPoint point)
{
	int nIndex;

	if (s_bFirefoxLike) {
		// Dragされているタブ自身とその左右にカーソルがあるかどうか
		if (( nIndex = _HitTestCurTabSide(point, flag) ) != -1) {
			if (flag == htOneself) {
				return -1;	// Dragされているのタブ上にカーソルがある
			} else {
				return nIndex;
			}
		}
	}

	vector<std::pair<int, CRect> > vecRect;

	// タブの境界上にカーソルがあるかどうか
	if (( nIndex = _HitTestSeparator(point, vecRect/*, s_nScrollInset*/) ) != -1) {
		flag = htSeparator;		// タブの境界にHitした
		return nIndex;
	}

	// 左側にカーソルがあるかどうか
	if (( nIndex = _HitTestLeftSide(point, vecRect) ) != -1) {
		flag = htInsetLeft;
		return nIndex;
	}

	// タブの上にあるかどうか
	if (( nIndex = _HitTest(point) ) != -1) {
		flag = htItem;
		return nIndex;
	}

	// out side of items
	flag = htOutside;
	return -1;
}


//-----------------------------------------
/// 挿入マークを消す
void CDonutTabBar::Impl::_ClearInsertionEdge()
{
	if ( !m_rcInvalidateOnDrawingInsertionEdge.IsRectEmpty() ) {
		InvalidateRect(m_rcInvalidateOnDrawingInsertionEdge);
		UpdateWindow();
		m_rcInvalidateOnDrawingInsertionEdge.SetRectEmpty();
	}
}

//-------------------------------------------
/// 挿入マークを描写
bool CDonutTabBar::Impl::_DrawInsertionEdge(_hitTestFlag flag, int nIndex)
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
				rcItem = m_vecpItem[nIndex]->rcItem;
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
				rcItem = m_vecpItem[nIndex]->rcItem;
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
				rcItem = m_vecpItem[nIndex]->rcItem;
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
				rcItem = m_vecpItem[nIndex]->rcItem;
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
				rcItem	= m_vecpItem[nIndex]->rcItem;
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
}

//---------------------------------------
/// 挿入マークを描写 : 実際に描写する
void CDonutTabBar::Impl::_DrawInsertionEdgeAux(CPoint pt, insertFlags flag)
{
	int 		 cy 	= _GetItemHeight();
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

//----------------------------------------------
/// rcを囲むように描く
void CDonutTabBar::Impl::_DrawInsertionEdgeAux(const CRect &rc)
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

//----------------------------------------------
bool CDonutTabBar::Impl::_IsSameIndexDropped(int nDestIndex)
{
	if (m_bDragFromItself == false)
		return false;

	for (int i = 0; i < m_arrCurDragItems.GetSize(); ++i) {
		if (m_arrCurDragItems[i] == nDestIndex)
			return true;
	}

	return false;
}

//-----------------------------------------------
void CDonutTabBar::Impl::_DoDragDrop(const CPoint& pt, UINT nFlags, int nIndex)
{
	if ( PreDoDragDrop(m_hWnd, NULL, false) ) { 	// now dragging
		_HotItem(); 								// clean up hot item

		// set up current drag item index list
		GetCurMultiSelEx(m_arrCurDragItems, nIndex);

		CComPtr<IDataObject> spDataObject;
		HRESULT hr = OnGetTabCtrlDataObject(m_arrCurDragItems, &spDataObject);
		if ( SUCCEEDED(hr) ) {
			m_bDragFromItself = true;
			DROPEFFECT dropEffect = DoDragDrop(spDataObject, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);
			m_bDragFromItself = false;
		}

		m_arrCurDragItems.RemoveAll();

	} else {	// Drag操作をしていなかったのでタブをアクティブに
		SetCurSel(nIndex, true);
	}
}

//-------------------------------------------
bool	CDonutTabBar::Impl::_CanScrollItem(bool bRight/* = true*/) const
{
	if (s_bMultiLine)
		return false;			// can't

	if (bRight) {
		int 	nCount = GetItemCount();
		return	m_nFirstIndexOnSingleLine + 1 < nCount;
	} else {
		return	m_nFirstIndexOnSingleLine - 1 >= 0;
	}
}

//--------------------------------------------
bool	CDonutTabBar::Impl::_ScrollItem(bool bRight/* = true*/)
{
	if (s_bMultiLine)
		return false;		// do nothing

	if (bRight) {
		int 	nCount = GetItemCount();
		if ( m_nFirstIndexOnSingleLine + 1 < nCount ) {
			++m_nFirstIndexOnSingleLine;
			_UpdateSingleLineLayout();
			Invalidate();
		} else {
			return false;
		}
	} else {
		if (m_nFirstIndexOnSingleLine - 1 >= 0) {
			--m_nFirstIndexOnSingleLine;
			_UpdateSingleLineLayout();
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


#if 0

void CMDITabCtrl::SetCurSelEx(int nIndex, bool bActivate)
{
	SetCurSel(nIndex);

	if (bActivate) {
		HWND	hWnd = GetTabHwnd(nIndex);
		ATLASSERT( ::IsWindow(hWnd) );
#if 1 //+++ メモ:unDonut+
		CWindow wndMDI(m_wndMDIChildPopuping.m_hWndMDIClient);
		wndMDI.SetRedraw(FALSE);
		m_wndMDIChildPopuping.MDIActivate(hWnd);
		wndMDI.SetRedraw(TRUE);
		wndMDI.RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
#else	//+++ メモ:r13test	//*お試し
		m_wndMDIChildPopuping.MDIActivate(hWnd);
#endif
	}
}




void CMDITabCtrl::_SendSelChange(int nIndex)
{
	int   idCtrl = ::GetDlgCtrlID(m_hWnd);
	NMHDR nmh	 = { m_hWnd, idCtrl, TCN_SELCHANGE };

	SendMessage(WM_NOTIFY, (WPARAM) idCtrl, (LPARAM) &nmh);
}



DWORD CMDITabCtrl::GetMDITabExtendedStyle() const
{
	return m_dwExtendedStyle;
}



DWORD CMDITabCtrl::SetMDITabExtendedStyle(DWORD dwExtendedStyle)
{
	DWORD dwPrevStyle = m_dwExtendedStyle;

	m_dwExtendedStyle = dwExtendedStyle;

	if (dwExtendedStyle & MTB_EX_MULTILINE)
		ModifyTabCtrl2ExtendedStyle(0, TAB2_EX_MULTILINE);
	else
		ModifyTabCtrl2ExtendedStyle(TAB2_EX_MULTILINE, 0);

	if (dwExtendedStyle & MTB_EX_FIXEDSIZE)
		ModifyTabCtrl2ExtendedStyle(0, TAB2_EX_FIXEDSIZE);
	else
		ModifyTabCtrl2ExtendedStyle(TAB2_EX_FIXEDSIZE, 0);

	if (dwExtendedStyle & MTB_EX_ANCHORCOLOR)
		ModifyTabCtrl2ExtendedStyle(0, TAB2_EX_ANCHORCOLOR);
	else
		ModifyTabCtrl2ExtendedStyle(TAB2_EX_ANCHORCOLOR, 0);

	if (dwExtendedStyle & MTB_EX_MOUSEDOWNSELECT)
		ModifyTabCtrl2ExtendedStyle(0, TAB2_EX_MOUSEDOWNSELECT);
	else
		ModifyTabCtrl2ExtendedStyle(TAB2_EX_MOUSEDOWNSELECT, 0);

	return dwPrevStyle;
}




void CMDITabCtrl::SetMaxTabItemTextLength(int nLength)
{
	if (m_nMaxTabItemTextLength == nLength)
		return;

	m_nMaxTabItemTextLength = nLength;

	UINT nCount = GetItemCount();
	for (UINT i = 0; i < nCount; ++i) {
		HWND  hWnd = GetTabHwnd(i);
		ATLASSERT( ::IsWindow(hWnd) );
		TCHAR szText[MAX_PATH];
		szText[0] = 0;
		::GetWindowText(hWnd, szText, MAX_PATH);
		_SetTabText(i, szText);
	}
}



int CMDITabCtrl::GetMaxTabItemTextLength()
{
	return m_nMaxTabItemTextLength;
}





// タブが切り替わるときに呼ばれる
LRESULT CMDITabCtrl::OnTcnSelChange(LPNMHDR lpnhmdr)
{
	// Watch OnLButtonDown, this handler will not be called by windows.
	// The flat style tab has a wastefull animation.

	ATLTRACE2( atlTraceGeneral, 4, _T("CMDITabCtrlImpl::OnTcnSelChange : %d\n"), GetCurSel() );
	int 	nIndex	   = GetCurSel();
	ATLASSERT(nIndex != -1);
#if 1	//+++ メモ:unDonut+
	CWindow wndMDI(m_wndMDIChildPopuping.m_hWndMDIClient);
	wndMDI.SetRedraw(FALSE);

	HWND	hWndActive = GetTabHwnd(nIndex);
	m_wndMDIChildPopuping.MDIActivate(hWndActive);

	wndMDI.SetRedraw(TRUE);
	wndMDI.RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
#else	//+++ メモ:r13test	//*お試し
	HWND	hWndActive = GetTabHwnd(nIndex);
	m_wndMDIChildPopuping.MDIActivate(hWndActive);
#endif
	return 0;
}


void CMDITabCtrl::OnMDIActivate(HWND hWndChildDeact, HWND hWndChildAct)
{
	ATLTRACE2( atlTraceGeneral, 4, _T("CMDITabCtrlImpl::OnMDIActivate\n") );
	SetMsgHandled(FALSE);

	if (hWndChildAct == NULL)
		return;

	int nIndex = GetTabIndex(hWndChildAct);
	ATLASSERT(nIndex != -1);

	if (!m_bRedrawLocked) {
		SetItemActive(nIndex);
		UpdateWindow();
	}

	SetCurSelEx(nIndex, false);
}



CString CMDITabCtrl::_GetTabText(int nIndex)
{
	CString str;

	GetItemText(nIndex, str);
	return str;
}






void CMDITabCtrl::_SetImageListIndex(int nItem, int nIndex)
{
	SetItemImageIndex(nItem, nIndex);
}



bool CMDITabCtrl::IsConnecting(HWND hWnd)
{
	int nItem = GetTabIndex(hWnd);

	if (nItem == -1)
		return false;

	return _GetImageListIndex(nItem) == 0;
}



bool CMDITabCtrl::IsDownloading(HWND hWnd)
{
	int nItem = GetTabIndex(hWnd);

	if (nItem == -1)
		return false;

	return _GetImageListIndex(nItem) == 1;
}



bool CMDITabCtrl::IsCompleted(HWND hWnd)
{
	int nItem = GetTabIndex(hWnd);

	if (nItem == -1)
		return false;

	return _GetImageListIndex(nItem) == -1;
}



int CMDITabCtrl::_GetImageListIndex(int nItem)
{
	int nImgIndex = -1;

	GetItemImageIndex(nItem, nImgIndex);
	return nImgIndex;
}


int CMDITabCtrl::ShowTabListMenuDefault(int nX, int nY)
{
	int 	nCount = GetItemCount();
	CMenu	menu;

	menu.CreatePopupMenu();

	for (int i = 0; i < nCount; i++) {
		menu.AppendMenu( 0, i + 1, _GetTabText(i) );
	}

	int 	nRet	 = menu.TrackPopupMenu(
						TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
						nX,
						nY + 1,
						m_hWnd);
	return	nRet - 1;
}





// Overrides
void CMDITabCtrl::OnDropDownButton()
{
	CRect rc;

	m_wndDropBtn.GetClientRect(&rc);
	m_wndDropBtn.ClientToScreen(&rc);
	int   nIndex = ShowTabListMenuVisible(rc.left, rc.top);

	if (nIndex == -1)
		return;

	HWND  hChild = GetTabHwnd(nIndex);

	if ( !::IsWindow(hChild) )
		return;

	::SendMessage(m_wndMDIChildPopuping.m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM) hChild, 0);
}


void CMDITabCtrl::OnDeleteItemDrag(int nIndex)
{
	HWND hWnd;

	if ( ( hWnd = GetTabHwnd(nIndex) ) != 0 )
		::PostMessage( (HWND) hWnd, WM_CLOSE, 0, 0 );
}



#endif

//////////////////////////////////////////
// CDonutTabBar

CDonutTabBar::CDonutTabBar()
	: pImpl(new Impl)
{	}

CDonutTabBar::~CDonutTabBar()
{
	delete pImpl;
}

//--------------------
HWND	CDonutTabBar::Create(HWND hWndParent)
{
	return pImpl->Create(hWndParent);
}

//--------------------
void	CDonutTabBar::SetMDIClient(HWND hWndMDIClient)
{
	pImpl->SetMDIClient(hWndMDIClient);
}



// Attributes

//-----------------------------------
HWND	CDonutTabBar::GetTabHwnd(int nIndex)
{
	return pImpl->GetTabHwnd(nIndex);
}

//-----------------------------------
int 	CDonutTabBar::GetTabIndex(HWND hWndMDIChild)
{
	return pImpl->GetTabIndex(hWndMDIChild);
}

//----------------------------------
void	CDonutTabBar::SetFont(HFONT hFont)
{
	pImpl->SetFont(hFont);
}

//----------------------------------
int		CDonutTabBar::GetItemCount() const
{
	return pImpl->GetItemCount();
}

//--------------------------------
void	CDonutTabBar::GetItemState(int nIndex, DWORD& state)
{
	pImpl->GetItemState(nIndex, state);
}

//--------------------------------
void	CDonutTabBar::SetItemActive(int nIndex)
{
	pImpl->SetItemActive(nIndex);
}

//--------------------------------
void	CDonutTabBar::SetItemInactive(int nIndex)
{
	pImpl->SetItemInactive(nIndex);
}

//----------------------------------
int		CDonutTabBar::GetCurSel() const
{
	return pImpl->GetCurSel();
}

//------------------------
bool	CDonutTabBar::SetCurSel(int nIndex)
{
	return pImpl->SetCurSel(nIndex);
}

//----------------------------------
void	CDonutTabBar::SetLinkState(ELinkState State)
{
	pImpl->SetLinkState(State);
}

//----------------------------------
void	CDonutTabBar::SetConnecting(HWND hWnd)
{
	pImpl->SetConnecting(hWnd);
}

//----------------------------------
void	CDonutTabBar::SetDownloading(HWND hWnd)
{
	pImpl->SetDownloading(hWnd);
}

//----------------------------------
void	CDonutTabBar::SetComplete(HWND hWnd)
{
	pImpl->SetComplete(hWnd);
}
//-------------------------------------------------------
void	CDonutTabBar::NavigateLockTab(HWND hWnd, bool bOn)
{
	pImpl->NavigateLockTab(hWnd, bOn);
}

//----------------------------------
void	CDonutTabBar::ReloadSkin()
{
	pImpl->ReloadSkin();
}

//-----------------
void	CDonutTabBar::GetWindowRect(LPRECT rect)
{
	pImpl->GetWindowRect(rect);
}

// Operation

//---------------------------------
void	CDonutTabBar::OnMDIChildCreate(HWND hWnd)
{
	pImpl->OnMDIChildCreate(hWnd);
}

//----------------------------------
void	CDonutTabBar::OnMDIChildDestroy(HWND hWnd)
{
	pImpl->OnMDIChildDestroy(hWnd);
}

//----------------------------------
void	CDonutTabBar::InsertHere(bool bUse)
{
	pImpl->InsertHere(bUse);
}

//----------------------------------
void	CDonutTabBar::SetInsertIndex(int nIndex)
{
	pImpl->SetInsertIndex(nIndex);
}

//-----------------------------------
bool	CDonutTabBar::MoveItems(int nDestIndex, CSimpleArray<int>& arrSrcs)
{
	return pImpl->MoveItems(nDestIndex, arrSrcs);
}

//--------------------
void	CDonutTabBar::LeftTab()
{
	pImpl->LeftTab();
}

//--------------------
void	CDonutTabBar::RightTab()
{
	pImpl->RightTab();
}

//------------------------
void	CDonutTabBar::ShowTabMenu(int nIndex)
{
	pImpl->ShowTabMenu(nIndex);
}

//-----------------------
int 	CDonutTabBar::ShowTabListMenuDefault (int nX, int nY)
{
	return pImpl->ShowTabListMenuDefault(nX, nY);
}

//-----------------------
int 	CDonutTabBar::ShowTabListMenuVisible (int nX, int nY)
{
	return pImpl->ShowTabListMenuVisible(nX, nY);
}

//------------------------
int 	CDonutTabBar::ShowTabListMenuAlphabet(int nX, int nY)
{
	return pImpl->ShowTabListMenuAlphabet(nX, nY);
}



//-----------------------
void	CDonutTabBar::ForEachWindow(function<void (HWND)> func)
{
	int	nCount = pImpl->GetItemCount();
	for (int i = 0; i < nCount; ++i) {
		HWND	hWnd = pImpl->GetTabHwnd(i);
		ATLASSERT( ::IsWindow(hWnd) );
		func(hWnd);
	}
}



BEGIN_MSG_MAP_EX_impl(CDonutTabBar)
ALT_MSG_MAP(1)
	CHAIN_MSG_MAP_ALT_MEMBER( (*pImpl), 1)
END_MSG_MAP()


void	CDonutTabBar::OnSetFaviconImage(HWND hWnd, HICON hIcon)
{
	pImpl->OnSetFaviconImage(hWnd, hIcon);
}








/////////////////////////////
// CLockRedraw

CDonutTabBar::CLockRedraw::CLockRedraw(CDonutTabBar& tabbar)
	: m_TabBar(tabbar)
{
	m_TabBar.pImpl->_LockRedraw(true);
	m_TabBar.pImpl->SetRedraw(FALSE);
}

CDonutTabBar::CLockRedraw::~CLockRedraw()
{
	m_TabBar.pImpl->_LockRedraw(false);
	m_TabBar.pImpl->SetRedraw(TRUE);
	m_TabBar.pImpl->Invalidate();
	m_TabBar.pImpl->UpdateWindow();
}


