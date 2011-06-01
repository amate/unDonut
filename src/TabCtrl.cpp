// TabCtrl.cpp

#include "stdafx.h"
#include "TabCtrl.h"
#include "MtlMisc.h"

/////////////////////////////////////////////////////////////////////
// CTabSkin

void	CTabSkin::Update(CDCHandle dc, HIMAGELIST hImgList, HIMAGELIST hImgFavicon, const CTabCtrlItem& item, bool bAnchorColor)
{
	if (item.m_fsState & TCISTATE_HIDDEN)
		return;

	int	cxIcon			= 0;
	int cyIcon			= 0;
	int	cxIconOffset	= 0;

	enum { 
		cxTabIcon = 4, cyTabIcon = 1, cxIconText = 2, faviconSize = 16, 
		LoadIconSize = 6, cxTabLoadIcon = 4, cyTabText = 3,
	};
	//if (item.m_nImgIndex != -1) {
	//	::ImageList_GetIconSize(hImgList, &cxIcon, &cyIcon);
	//	cxIconOffset = 20;//cxIcon + s_kcxIconGap + 3;
	//} else {
		cxIconOffset = cxTabIcon + 16 + cxIconText;
	//}

	bool	bHot		= (item.m_fsState & TCISTATE_HOT) != 0;
	bool	bPressed	= (item.m_fsState & TCISTATE_PRESSED) != 0;

	CPoint	ptOffset(0, 0);

	if (item.m_fsState & TCISTATE_SELECTED) {
		// 選択された
		_DrawSkinCur(dc, item.m_rcItem);
//		Update_Selected(dc, pTabSkin, bHot, bPressed);
		//ptOffset += CPoint(2, 2);	// タブに書かれる文字を若干下げる
	} else if (item.m_fsState & TCISTATE_MSELECTED) {
		// 複数選択された
		_DrawSkinSel(dc, item.m_rcItem);
//		Update_MultiSel(dc, pTabSkin, bHot, bPressed);
		//ptOffset += CPoint(4, 6);	// タブに書かれる文字を若干下げる
	} else {
		// 選択されていない
		if (bHot == true && bPressed == false) {
			_DrawSkinSel(dc, item.m_rcItem);
		} else {
			_DrawSkinNone(dc, item.m_rcItem);
		}
//		Update_NotSel(dc, pTabSkin, bHot, bPressed, ptOffset);
		//ptOffset += CPoint(2, 4);
	}

	_DrawText(dc, /*ptOffset + */CPoint(cxIconOffset, cyTabText), item, bAnchorColor);

	if (item.m_nImgIndex != -1) {
		::ImageList_Draw(hImgList, item.m_nImgIndex, dc, 
			item.m_rcItem.left + ptOffset.x + s_kcxIconGap + cxTabLoadIcon,
			item.m_rcItem.top + ((item.m_rcItem.Height() - LoadIconSize) / 2) /*+ (ptOffset.y / 2)*/ + cyTabIcon, ILD_TRANSPARENT);
	} else {
		int nFaviconIndex = (item.m_nFaviconIndex != -1) ? item.m_nFaviconIndex : 0;
		::ImageList_Draw(hImgFavicon, nFaviconIndex, dc,
			item.m_rcItem.left + ptOffset.x + cxTabIcon,
			item.m_rcItem.top + ((item.m_rcItem.Height() - faviconSize) / 2) /*+ (ptOffset.y / 2)*/ + cyTabIcon, ILD_TRANSPARENT);
	}

#if 0
	if (item.m_fsState & TCISTATE_NAVIGATELOCK) {
		CPoint	pt(item.m_rcItem.left + ptOffset.x + s_kcxIconGap + 6
				 , item.m_rcItem.top + ((item.m_rcItem.Height() - cyIcon) / 2) + (ptOffset.y / 2) + 4);
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
	m_imgLock.Create(CXICON, CYICON, ILC_COLOR32 | ILC_MASK, 1, 1);

	CString	strTabSkinDir = _GetSkinDir();

//	CString	strNavigateLock = strTabSkinDir;
//	strNavigateLock += _T("NavigateLock.ico");
//	ATLASSERT(FALSE);
//	HICON hIcon = AtlLoadIconImage(strNavigateLock.GetBuffer(0), LR_LOADFROMFILE/* | LR_LOADTRANSPARENT*/, CXICON, CYICON);
	HICON hIcon = AtlLoadIcon(IDI_NAVIGATELOCK);
	m_imgLock.AddIcon(hIcon);
	::DestroyIcon(hIcon);
}


void	CTabSkin::_DrawText(CDCHandle dc, CPoint ptOffset, const CTabCtrlItem& item, bool bAnchorColor)
{
	COLORREF	clr;

	if ( !(item.m_fsState & TCISTATE_ENABLED) )
		clr = m_colDisable;
	else if (item.m_fsState & TCISTATE_INACTIVE)
		clr = bAnchorColor ? m_colInActive	: m_colText;
	else
		clr = bAnchorColor ? m_colActive	: m_colText;

	COLORREF	clrOld = dc.SetTextColor(clr);

	CRect		rcBtn(item.m_rcItem.left + ptOffset.x, item.m_rcItem.top + ptOffset.y, 
					  item.m_rcItem.right, item.m_rcItem.bottom);
	rcBtn.DeflateRect(2, 0);

	UINT	uFormat;
	int		nWidth	= MtlComputeWidthOfText(item.m_strItem, dc.GetCurrentFont());

	if ( nWidth > rcBtn.Width() ) {
		uFormat = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT | DT_END_ELLIPSIS;
	} else {
		uFormat = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | /*DT_CENTER | */DT_NOCLIP;
	}

	if ( !(item.m_fsState & TCISTATE_ENABLED) ) {
		CRect		rcDisabled	= rcBtn + CPoint(1, 1);
		COLORREF	clrOld2		= dc.SetTextColor(m_colDisableHi);
		dc.DrawText(item.m_strItem, -1, rcDisabled, uFormat);
		dc.SetTextColor(clrOld2);
	}

	// タブ上に文字列を書く
	dc.DrawText(item.m_strItem, -1, rcBtn, uFormat);
	dc.SetTextColor(clrOld);
}


////////////////////////////////////////////////////////////////////////
// CTabSkinDefault

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
		ATLASSERT(FALSE);
		MessageBox(NULL, file + _T("が存在しません"), NULL, NULL);
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

// Overrides
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
