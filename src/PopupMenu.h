/**
*	@file	PopupMenu.h
*	@brief	カスタムポップアップメニュー
*/

#pragma once

#include <boost\thread.hpp>
#include <atlscrl.h>
#include <atltheme.h>
#include "MtlWin.h"
#include "MtlMisc.h"
#include "Misc.h"
#include "LinkPopupMenu.h"

#define WM_CLOSEBASESUBMENU		(WM_APP + 1)

class IBasePopupMenu
{
public:
	virtual IBasePopupMenu* CreateSubMenu(int nIndex) = 0;
	virtual void	DoTrackPopupMenu(CMenuHandle menu, CPoint ptLeftBottom, HWND hWndParent) = 0;
	virtual void	DoTrackSubPopupMenu(CMenuHandle menu, CRect rcClientItem, HWND hWndParent, int nInheritIndex) = 0;
	virtual HWND	GetHWND() = 0;
	virtual IBasePopupMenu*	GetSubMenu() = 0;
	virtual int		GetInheritMenuIndex() = 0;
	virtual void	DestroyWindow() = 0;

	static HWND	s_hWndCommandBar;
};

template<typename T>
class CBasePopupMenuImpl : 
	public CScrollWindowImpl<T>,
	public CThemeImpl<T>,
	public CTrackMouseLeave<T>,
	public IBasePopupMenu
{
public:
	DECLARE_WND_CLASS_EX(_T("DonutBasePopupMenu"), CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW, COLOR_MENU)

	// Constants
	enum {
		kBoundMargin = 2,
		kBoundBorder = 1,
		kItemHeight = 22, 
		kSeparatorHeight = 6,
		kIconWidth	= 26,
		kCheckWidth = 22,
		kVerticalLineWidth = 2,
		kTextMargin	= 4,
		kLeftTextPos = kBoundMargin + kIconWidth + kVerticalLineWidth + kTextMargin,
		kNoThemeLeftTextPos = kBoundMargin + kCheckWidth,
		kArrowWidth	= 25,
		kNoneTextWidth = 50,
		kMaxMenuTextWidth	= 400,
		kBiteWidth	= 3,

		kcxIcon = 16,
		kcyIcon = 16,
		kTopIconMargin = 3,
		kLeftIconMargin = kBoundMargin + 4,

		kSubMenuPopupCloseTimerID = 1,
		kSubMenuPopupCloseTime	= 500,
		kSubMenuPopupTimerID = 2,
		kSubMenuPopupTime	= 300,

		kDragInsertHitWidthOnFolder = 3,

		kcySeparatorPadding = 3,
	};


	CBasePopupMenuImpl() : m_nHotIndex(-1), m_pSubMenu(nullptr), m_nInheritMenuIndex(-1)
	{
		WTL::CLogFont	lf;
		lf.SetMenuFont();
		m_font = lf.CreateFontIndirect();
	}

	virtual ~CBasePopupMenuImpl() { }

	virtual IBasePopupMenu* CreateSubMenu(int nIndex) override { return new CBasePopupMenu; }

	virtual void	DoTrackPopupMenu(CMenuHandle menu, CPoint ptLeftBottom, HWND hWndParent) override
	{
		m_menu = menu;
		s_hWndCommandBar = hWndParent;
		_initMenuItem();

		CRect	rcWindow;
		rcWindow.right	= ComputeWindowWidth();
		rcWindow.bottom	= ComputeWindowHeight();
		rcWindow.MoveToXY(ptLeftBottom.x, ptLeftBottom.y);	
		CRect rcWork = Misc::GetMonitorWorkArea(hWndParent);
		if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
			rcWindow.bottom	= rcWork.bottom;
			rcWindow.right += ::GetSystemMetrics(SM_CXVSCROLL);
		}
		if (rcWork.right < rcWindow.right) {	// 右にはみ出る
			int nWidth = rcWindow.Width();
			rcWindow.MoveToX(rcWork.right - nWidth);
		}

		Create(GetDesktopWindow(), rcWindow, NULL, WS_POPUP | WS_BORDER , WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
		ShowWindow(SW_SHOWNOACTIVATE);
	}

	virtual void	DoTrackSubPopupMenu(CMenuHandle menu, CRect rcClientItem, HWND hWndParent, int nInheritIndex) override
	{
		m_menu = menu;
		m_nInheritMenuIndex = nInheritIndex;
		_initMenuItem();

		CRect rcWork = Misc::GetMonitorWorkArea(hWndParent);
		CRect rcWindow;
		int nTop	= rcClientItem.top - (kBoundBorder + kBoundMargin);
		int nLeft	= rcClientItem.right	- kBiteWidth;
		rcWindow.right	= ComputeWindowWidth();
		rcWindow.bottom	= ComputeWindowHeight();
		rcWindow.MoveToXY(nLeft, nTop);
		CWindow(hWndParent).ClientToScreen(&rcWindow);
		if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
			int nOverHeight = rcWindow.bottom - rcWork.bottom;
			rcWindow.MoveToY( rcWindow.top - nOverHeight );
			if (rcWindow.top < rcWork.top)
				rcWindow.top = rcWork.top;
			rcWindow.bottom	= rcWork.bottom;
			static const int s_nVScrollWidth = ::GetSystemMetrics(SM_CXVSCROLL);
			rcWindow.right += s_nVScrollWidth;
		}
		if (rcWork.right < rcWindow.right) {	// 右にはみ出る
			int nWidth = rcWindow.Width();
			ClientToScreen(&rcClientItem);
			//rcClientItem.left += kBiteWidth;
			rcWindow.MoveToX(rcClientItem.left - nWidth);	// 反対側に出す
		}

		Create(hWndParent, rcWindow, nullptr, WS_POPUP | WS_BORDER, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST);
		ShowWindow(SW_SHOWNOACTIVATE);
	}

	virtual int	ComputeWindowWidth()
	{
		int nMaxTextWidth = 0;
		for (auto it = m_vecMenuItem.begin(); it != m_vecMenuItem.end(); ++it) {
			int nNameWidth = MTL::MtlComputeWidthOfText(it->name, m_font);
			if (nMaxTextWidth < nNameWidth)
				nMaxTextWidth = nNameWidth;
		}
		if (nMaxTextWidth == 0)
			nMaxTextWidth = kNoneTextWidth;
		if (kMaxMenuTextWidth < nMaxTextWidth)
			nMaxTextWidth = kMaxMenuTextWidth;
		nMaxTextWidth += kLeftTextPos + kTextMargin + kArrowWidth + (kBoundMargin * 2);
		return nMaxTextWidth;
	}

	virtual int ComputeWindowHeight()
	{
		int nHeight = (kBoundMargin * 2) + (kBoundBorder * 2);
		if (m_vecMenuItem.size() > 0) {			
			nHeight += m_vecMenuItem[m_vecMenuItem.size() - 1].rect.bottom - kBoundMargin;
		} else {
			nHeight += kItemHeight;
		}
		return nHeight;
	}

	// Overrides
	virtual void OnFinalMessage(HWND /*hWnd*/) { delete this; }
	void DoPaint(CDCHandle dc)
	{
		CPoint ptOffset;
		GetScrollOffset(ptOffset);

		CRect rcClient;
		GetClientRect(rcClient);
		rcClient.bottom	+= ptOffset.y;

		CMemoryDC	memDC(dc, rcClient);

		HFONT hFontPrev = memDC.SelectFont(m_font);
		memDC.SetBkMode(TRANSPARENT);

		if (IsThemeNull() == false) {
			DrawThemeBackground(memDC, MENU_POPUPBACKGROUND, 0, &rcClient);
			CRect rcGutter;
			rcGutter.left = kBoundMargin + kIconWidth;
			rcGutter.right= rcGutter.left + kVerticalLineWidth;
			rcGutter.bottom= rcClient.bottom;
			DrawThemeBackground(memDC, MENU_POPUPGUTTER, 0, &rcGutter);

			for (auto it = m_vecMenuItem.begin(); it != m_vecMenuItem.end(); ++it) {
				MenuItem& item = *it;
				item.rect.right = rcClient.right - kBoundMargin;
				if (item.bSeparator) {
					item.rect.left = kBoundMargin + kIconWidth;
					DrawThemeBackground(memDC, MENU_POPUPSEPARATOR, 0, &item.rect);
				} else {					
					DrawThemeBackground(memDC, MENU_POPUPITEM, item.state, &item.rect);

					CRect	rcText = item.rect;
					rcText.left	= kLeftTextPos;
					rcText.right	-= kArrowWidth + kBoundMargin;
					DrawThemeText(memDC, MENU_POPUPITEM, item.state, item.name, item.name.GetLength(), DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_HIDEPREFIX, 0, &rcText);

					if (item.submenu) {
						CRect rcArrow = rcText;
						rcArrow.left	= rcText.right;
						rcArrow.right	= rcClient.right;
						int nState = MSM_NORMAL;
						if (item.state == MPI_DISABLED || item.state == MPI_DISABLEDHOT)
							nState = MSM_DISABLED;
						DrawThemeBackground(memDC, MENU_POPUPSUBMENU, nState, &rcArrow);
					}
				}
				if (item.bChecked) {
					CRect rcCheck = item.rect;
					rcCheck.right	= rcCheck.left + kCheckWidth;
					DrawThemeBackground(memDC, MENU_POPUPCHECKBACKGROUND, MCB_NORMAL, &rcCheck);
					DrawThemeBackground(memDC, MENU_POPUPCHECK, MC_CHECKMARKNORMAL, &rcCheck);
				}
			}
			//if (m_bDrawDisableHot) {
			//	CRect rcNone = rcClient;
			//	rcNone.DeflateRect(kBoundMargin, kBoundMargin);
			//	DrawThemeBackground(memDC, MENU_POPUPITEM, MPI_DISABLEDHOT, &rcNone);
			//}

			//if (m_pFolder->size() == 0) {
			//	CRect	rcText;
			//	rcText.top	= kBoundMargin;
			//	rcText.left	= kLeftTextPos;
			//	rcText.right	= rcClient.right;
			//	rcText.bottom	= rcText.top + kItemHeight;

			//	LPCTSTR strNone = _T("(なし)");
			//	DrawThemeText(memDC, MENU_POPUPITEM, MPI_DISABLED, strNone, ::lstrlen(strNone), DT_SINGLELINE | DT_VCENTER, 0, &rcText);
			//}
		} else {
			memDC.FillRect(rcClient, COLOR_3DFACE);

			for (auto it = m_vecMenuItem.begin(); it != m_vecMenuItem.end(); ++it) {
				MenuItem& item = *it;
				item.rect.right = rcClient.right - kBoundMargin;

				//POINT ptIcon;
				//ptIcon.x	= kLeftIconMargin;
				//ptIcon.y	= item.rcItem.top + kTopIconMargin;
				//CIconHandle icon;
				//if (item.pFolder) {
				//	icon = CLinkPopupMenu::s_iconFolder;
				//} else {
				//	icon = item.icon;
				//	if (icon == NULL)
				//		icon = CLinkPopupMenu::s_iconLink;
				//}
				//icon.DrawIconEx(memDC, ptIcon, CSize(kcxIcon, kcyIcon));
				if (item.bSeparator) {
					RECT rc = item.rect;
					rc.top += (rc.bottom - rc.top) / 2;      // vertical center
					memDC.DrawEdge(&rc, EDGE_ETCHED, BF_TOP);   // draw separator line

				} else {
					if (item.state == MPI_HOT || item.state == MPI_DISABLEDHOT)
						memDC.FillRect(item.rect, COLOR_HIGHLIGHT);

					CRect	rcText = item.rect;
					rcText.left	= kNoThemeLeftTextPos;
					rcText.right	-= kArrowWidth + kBoundMargin;
					if (item.state == MPI_DISABLED || item.state == MPI_DISABLEDHOT) {
						bool bSelected = (item.state == MPI_DISABLEDHOT);
						COLORREF colorText = ::GetSysColor(bSelected ? COLOR_GRAYTEXT : COLOR_3DSHADOW);
						memDC.SetTextColor(colorText);
						memDC.DrawText(item.name, item.name.GetLength(), &rcText, DT_SINGLELINE | DT_VCENTER | DT_HIDEPREFIX);

					} else {
						memDC.SetTextColor(item.state == MPI_HOT ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_MENUTEXT));
						memDC.DrawText(item.name, item.name.GetLength(), rcText, DT_SINGLELINE | DT_VCENTER | DT_HIDEPREFIX);
					}
					if (item.submenu) {
						CRect rcArrow = rcText;
						rcArrow.left	= rcText.right;
						rcArrow.right	= rcClient.right;
						enum { kcxArrow = 12, kcyArrow = 12 };
						CDC	memDC2 = ::CreateCompatibleDC(memDC);
				
						CBitmap bmpArrow = ::CreateCompatibleBitmap(memDC, kcxArrow, kcyArrow);
						HBITMAP hbmpPrev = memDC2.SelectBitmap(bmpArrow);
						UINT nState = DFCS_MENUARROW;
						memDC2.DrawFrameControl(CRect(0, 0, kcxArrow, kcyArrow), DFC_MENU, DFCS_MENUARROW);
						if (item.state == MPI_HOT) {
							CBitmap bmpWiteArrow = ::CreateCompatibleBitmap(memDC, kcxArrow, kcyArrow);
							CDC	memDC3 = ::CreateCompatibleDC(memDC);
							HBITMAP hbmpPrev2 = memDC3.SelectBitmap(bmpWiteArrow);
							memDC3.BitBlt(0, 0, kcxArrow, kcyArrow, memDC2, 0, 0, NOTSRCCOPY);

							memDC.TransparentBlt(rcClient.right - kcxArrow - 4, rcText.top + ((rcText.Height() - kcyArrow) / 2), kcxArrow, kcyArrow, memDC3, 0, 0, kcxArrow, kcyArrow, RGB(0, 0, 0));
							memDC3.SelectBitmap(hbmpPrev2);
						} else {
							memDC.TransparentBlt(rcClient.right - kcxArrow - 4, rcText.top + ((rcText.Height() - kcyArrow) / 2), kcxArrow, kcyArrow, memDC2, 0, 0, kcxArrow, kcyArrow, RGB(0xFF, 0xFF, 0xFF));
						}
						memDC2.SelectBitmap(hbmpPrev);
					}
					if (item.bChecked) {
						CRect rcCheck = item.rect;
						rcCheck.right	= rcCheck.left + kCheckWidth;
						DrawCheckmark(memDC.m_hDC, rcCheck, item.state == MPI_HOT || item.state == MPI_DISABLEDHOT, item.state == MPI_DISABLED || item.state == MPI_DISABLEDHOT, false, NULL);
					}
				}
			}
			//if (m_pFolder->size() == 0) {
			//	CRect	rcText;
			//	rcText.top	= kBoundMargin;
			//	rcText.left	= kLeftTextPos;
			//	rcText.right	= rcClient.right;
			//	rcText.bottom	= rcText.top + kItemHeight;

			//	LPCTSTR strNone = _T("(なし)");
			//	memDC.DrawText(strNone, ::lstrlen(strNone), rcText, DT_SINGLELINE | DT_VCENTER);
			//}
		}


		memDC.SelectFont(hFontPrev);
	}


	/// カーソル上のアイテムをHOT状態にする
	void OnTrackMouseMove(UINT nFlags, CPoint pt)
	{
		int nIndex = _HitTest(pt);
		if (nIndex != -1) {
			if (nIndex != m_nHotIndex) {
				_HotItem(nIndex);
				SetTimer(kSubMenuPopupTimerID, kSubMenuPopupTime);
			}
		}
	}

	/// アイテムのHOT状態を解除する
	void OnTrackMouseLeave()
	{
		KillTimer(kSubMenuPopupTimerID);
		if (m_pSubMenu) {	// カーソルが出て行ったときにサブメニューを閉じる
			CPoint	pt;
			GetCursorPos(&pt);
			HWND hWnd = WindowFromPoint(pt);
			if (hWnd != m_pSubMenu->GetHWND()) {
				SetTimer(kSubMenuPopupCloseTimerID, kSubMenuPopupCloseTime);
			} else {
				_HotItem(m_pSubMenu->GetInheritMenuIndex());
				return ;
			}
		}
		_HotItem(-1);
	}

	// IBasePopupMenu
	virtual HWND	GetHWND() override { return m_hWnd; }
	virtual IBasePopupMenu*	GetSubMenu() override { return m_pSubMenu; }
	virtual int		GetInheritMenuIndex() override { return m_nInheritMenuIndex; }
	virtual void	DestroyWindow() override { ::DestroyWindow(m_hWnd); }

	BEGIN_MSG_MAP( CBasePopupMenuImpl<T> )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		MSG_WM_TIMER( OnTimer )
		MSG_WM_ERASEBKGND	( OnEraseBkgnd )
		MSG_WM_MOUSEACTIVATE( OnMouseActivate )
		MSG_WM_KEYDOWN( OnKeyDown )
		MSG_WM_LBUTTONDOWN( OnLButtonDown )
		MSG_WM_LBUTTONUP	( OnLButtonUp	)
		CHAIN_MSG_MAP( CScrollWindowImpl<T> )
		CHAIN_MSG_MAP( CThemeImpl<T> )
		CHAIN_MSG_MAP( CTrackMouseLeave<T> )
		CHAIN_MSG_MAP_ALT(CScrollWindowImpl<T>, 1)
	END_MSG_MAP()

	int  OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		SetMsgHandled(FALSE);

		OpenThemeData(VSCLASS_MENU);

		enum { kBorderMargin = 2 };
		SetScrollSize(ComputeWindowWidth() - kBorderMargin, ComputeWindowHeight() - kBorderMargin);
		SetScrollLine(1, kItemHeight);

		return 0;
	}

	void OnDestroy()
	{
	}

	void OnTimer(UINT_PTR nIDEvent)
	{
		if (nIDEvent == kSubMenuPopupCloseTimerID) {
			KillTimer(nIDEvent);
			if (m_pSubMenu) {
				CPoint pt;
				::GetCursorPos(&pt);
				HWND hWnd = ::WindowFromPoint(pt);
				if (hWnd == m_pSubMenu->GetHWND()) {
					_HotItem(m_pSubMenu->GetInheritMenuIndex());
				} else {
					m_pSubMenu->DestroyWindow();
					m_pSubMenu = nullptr;
				}
			}
		} else if (nIDEvent == kSubMenuPopupTimerID) {
			KillTimer(nIDEvent);
			CPoint pt;
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			_DoExec(pt);	// popup
		}
	}


	BOOL OnEraseBkgnd(CDCHandle dc) { return TRUE; }

	/// クリックでアクティブにならないようにする
	int  OnMouseActivate(CWindow wndTopLevel, UINT nHitTest, UINT message) { return MA_NOACTIVATE; }

	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
	}

	void OnLButtonDown(UINT nFlags, CPoint point) {
		_DoExec(point, false);
	}
	void OnLButtonUp(UINT nFlags, CPoint point) {
		_DoExec(point, true);
	}

protected:
	virtual void	_initMenuItem()
	{
		CWindow(s_hWndCommandBar).GetTopLevelWindow().SendMessage(WM_INITMENUPOPUP, (WPARAM)m_menu.m_hMenu);

		int nWidht = ComputeWindowWidth();
		int nTop = kBoundMargin;
		int nCount = m_menu.GetMenuItemCount();
		for (int i = 0; i < nCount; ++i) {
			CString name;
			m_menu.GetMenuString(i, name, MF_BYPOSITION);
			UINT nID = m_menu.GetMenuItemID(i);
			MENUITEMINFO mii = { sizeof(mii) };
			mii.fMask	= MIIM_FTYPE | MIIM_STATE;
			m_menu.GetMenuItemInfo(i, TRUE, &mii);
			bool bSeparator = (mii.fType & MFT_SEPARATOR) != 0;
			POPUPITEMSTATES state = MPI_NORMAL;
			if (mii.fState & MFS_DISABLED)
				state = MPI_DISABLED;
			CRect rc;
			rc.top	= nTop;
			rc.left	= kBoundMargin;
			rc.right= nWidht - kBoundMargin;
			rc.bottom= nTop +  (bSeparator ? kSeparatorHeight : kItemHeight);
			nTop = rc.bottom;
#if _MSC_VER >= 1700
			m_vecMenuItem.emplace_back(name, nID, rc, bSeparator, m_menu.GetSubMenu(i));
#else
			m_vecMenuItem.push_back(MenuItem(name, nID, rc, bSeparator, m_menu.GetSubMenu(i)));
#endif
			m_vecMenuItem.back().state = state;
			m_vecMenuItem.back().bChecked = (mii.fState & MFS_CHECKED) != 0;
		}
	}
	bool	_IsValidIndex(int nIndex) {
		return 0 <= nIndex && nIndex < static_cast<int>(m_vecMenuItem.size());
	}
	int		_HitTest(const CPoint pt)
	{
		POINT ptOffset;
		GetScrollOffset(ptOffset);
		CRect rcClient;
		GetClientRect(&rcClient);
		int nCount = static_cast<int>(m_vecMenuItem.size());
		for (int i = 0; i < nCount; ++i) {
			CRect rcItem = m_vecMenuItem[i].rect;
			rcItem.top	-= ptOffset.y;
			rcItem.bottom	-= ptOffset.y;
			rcItem.right	= rcClient.right;
			if (rcItem.PtInRect(pt))
				return i;
		}
		return -1;
	}

	void	_HotItem(int nIndex)
	{
		// clean up
		if ( _IsValidIndex(m_nHotIndex) ) {
			MenuItem& item = m_vecMenuItem.at(m_nHotIndex);
			if (item.state == POPUPITEMSTATES::MPI_HOT)
				item.state	= POPUPITEMSTATES::MPI_NORMAL;
			else
				item.state = POPUPITEMSTATES::MPI_DISABLED;
			//if ( item.ModifyState(item.kItemHot, 0) )
				InvalidateRect(_GetClientItemRect(item.rect));
		}

		m_nHotIndex = nIndex;

		if ( _IsValidIndex(m_nHotIndex) ) {
			MenuItem& item = m_vecMenuItem.at(m_nHotIndex);
			if (item.state == POPUPITEMSTATES::MPI_NORMAL)
				item.state	= POPUPITEMSTATES::MPI_HOT;
			else
				item.state = POPUPITEMSTATES::MPI_DISABLEDHOT;
			//if ( item.ModifyState(0, item.kItemHot) )
				InvalidateRect(_GetClientItemRect(item.rect));
		}
	}
	
	CRect	_GetClientItemRect(const CRect& rcItem)
	{
		POINT ptOffset;
		GetScrollOffset(ptOffset);

		CRect rcClientRect = rcItem;
		rcClientRect.top	-= ptOffset.y;
		rcClientRect.bottom	-= ptOffset.y;
		return rcClientRect;
	}

	void	_DoExec(const CPoint& pt, bool bLButtonUp = false)
	{
		int nIndex = _HitTest(pt);
		if (nIndex == -1)
			return ;

		MenuItem& item = m_vecMenuItem[nIndex];
		if (m_pSubMenu) {
			if (m_pSubMenu->GetInheritMenuIndex() == nIndex)
				return ;	// 派生元フォルダなら何もしない
			m_pSubMenu->DestroyWindow();
			m_pSubMenu = nullptr;
		}
		if (item.bSeparator || item.state == MPI_DISABLEDHOT)
			return ;

		// サブメニューをポップアップさせる
		if (item.submenu) {
			m_pSubMenu = CreateSubMenu(nIndex);
			CRect rcClientItem = _GetClientItemRect(item.rect);
			m_pSubMenu->DoTrackSubPopupMenu(item.submenu, rcClientItem, m_hWnd, nIndex);

		} else if (bLButtonUp) {
			// メニューを閉じる
			_CloseBaseSubMenu();

			// アイテムを実行...
			CWindow(s_hWndCommandBar).GetTopLevelWindow().SendMessage(WM_COMMAND, item.nID);
		}
	}

	void	_CloseBaseSubMenu() {
		::PostMessage(s_hWndCommandBar, WM_CLOSEBASESUBMENU, 0, 0);
	}

	BOOL DrawCheckmark(CDCHandle dc, const RECT& rc, BOOL bSelected, BOOL bDisabled, BOOL bRadio, HBITMAP hBmpCheck)
	{
		// get checkmark bitmap, if none, use Windows standard
		SIZE size = { 0, 0 };
		CBitmapHandle bmp = hBmpCheck;
		if(hBmpCheck != NULL)
		{
			bmp.GetSize(size);
		}
		else
		{
			size.cx = ::GetSystemMetrics(SM_CXMENUCHECK); 
			size.cy = ::GetSystemMetrics(SM_CYMENUCHECK); 
			bmp.CreateCompatibleBitmap(dc, size.cx, size.cy);
			ATLASSERT(bmp.m_hBitmap != NULL);
		}
		// center bitmap in caller's rectangle
		RECT rcDest = rc;
		if((rc.right - rc.left) > size.cx)
		{
			rcDest.left = rc.left + (rc.right - rc.left - size.cx) / 2;
			rcDest.right = rcDest.left + size.cx;
		}
		if((rc.bottom - rc.top) > size.cy)
		{
			rcDest.top = rc.top + (rc.bottom - rc.top - size.cy) / 2;
			rcDest.bottom = rcDest.top + size.cy;
		}
		// paint background
		//if(!m_bFlatMenus)
		//{
		//	if(bSelected && !bDisabled)
		//	{
		//		dc.FillRect(&rcDest, COLOR_MENU);
		//	}
		//	else
		//	{
		//		COLORREF clrTextOld = dc.SetTextColor(::GetSysColor(COLOR_BTNFACE));
		//		COLORREF clrBkOld = dc.SetBkColor(::GetSysColor(COLOR_BTNHILIGHT));
		//		CBrush hbr(CDCHandle::GetHalftoneBrush());
		//		dc.SetBrushOrg(rcDest.left, rcDest.top);
		//		dc.FillRect(&rcDest, hbr);
		//		dc.SetTextColor(clrTextOld);
		//		dc.SetBkColor(clrBkOld);
		//	}
		//}

		// create source image
		CDC dcSource;
		dcSource.CreateCompatibleDC(dc);
		HBITMAP hBmpOld = dcSource.SelectBitmap(bmp);
		// set colors
		const COLORREF clrBlack = RGB(0, 0, 0);
		const COLORREF clrWhite = RGB(255, 255, 255);
		COLORREF clrTextOld = dc.SetTextColor(clrBlack);
		COLORREF clrBkOld = dc.SetBkColor(clrWhite);
		// create mask
		CDC dcMask;
		dcMask.CreateCompatibleDC(dc);
		CBitmap bmpMask;
		bmpMask.CreateBitmap(size.cx, size.cy, 1, 1, NULL);
		HBITMAP hBmpOld1 = dcMask.SelectBitmap(bmpMask);

		// draw the checkmark transparently
		int cx = rcDest.right - rcDest.left;
		int cy = rcDest.bottom - rcDest.top;
		//if(hBmpCheck != NULL)
		//{
		//	// build mask based on transparent color	
		//	dcSource.SetBkColor(m_clrMask);
		//	dcMask.SetBkColor(clrBlack);
		//	dcMask.SetTextColor(clrWhite);
		//	dcMask.BitBlt(0, 0, size.cx, size.cy, dcSource, 0, 0, SRCCOPY);
		//	// draw bitmap using the mask
		//	dc.BitBlt(rcDest.left, rcDest.top, cx, cy, dcSource, 0, 0, SRCINVERT);
		//	dc.BitBlt(rcDest.left, rcDest.top, cx, cy, dcMask, 0, 0, SRCAND);
		//	dc.BitBlt(rcDest.left, rcDest.top, cx, cy, dcSource, 0, 0, SRCINVERT);
		//}
		//else
		{
			const DWORD ROP_DSno = 0x00BB0226L;
			const DWORD ROP_DSa = 0x008800C6L;
			const DWORD ROP_DSo = 0x00EE0086L;
			const DWORD ROP_DSna = 0x00220326L;

			// draw mask
			RECT rcSource = { 0, 0, min(size.cx, rc.right - rc.left), min(size.cy, rc.bottom - rc.top) };
			dcMask.DrawFrameControl(&rcSource, DFC_MENU, bRadio ? DFCS_MENUBULLET : DFCS_MENUCHECK);

			// draw shadow if disabled
			//if(!m_bFlatMenus && bDisabled)
			//{
			//	// offset by one pixel
			//	int x = rcDest.left + 1;
			//	int y = rcDest.top + 1;
			//	// paint source bitmap
			//	const int nColor = COLOR_3DHILIGHT;
			//	dcSource.FillRect(&rcSource, nColor);
			//	// draw checkmark - special case black and white colors
			//	COLORREF clrCheck = ::GetSysColor(nColor);
			//	if(clrCheck == clrWhite)
			//	{
			//		dc.BitBlt(x, y, cx, cy, dcMask,  0, 0,   ROP_DSno);
			//		dc.BitBlt(x, y, cx, cy, dcSource, 0, 0, ROP_DSa);
			//	}
			//	else
			//	{
			//		if(clrCheck != clrBlack)
			//		{
			//			ATLASSERT(dcSource.GetTextColor() == clrBlack);
			//			ATLASSERT(dcSource.GetBkColor() == clrWhite);
			//			dcSource.BitBlt(0, 0, size.cx, size.cy, dcMask, 0, 0, ROP_DSna);
			//		}
			//		dc.BitBlt(x, y, cx, cy, dcMask,  0,  0,  ROP_DSa);
			//		dc.BitBlt(x, y, cx, cy, dcSource, 0, 0, ROP_DSo);
			//	}
			//}

			// paint source bitmap
			const int nColor = bDisabled ? COLOR_BTNSHADOW : COLOR_MENUTEXT;
			dcSource.FillRect(&rcSource, nColor);
			// draw checkmark - special case black and white colors
			COLORREF clrCheck = ::GetSysColor(nColor);
			if(clrCheck == clrWhite)
			{
				dc.BitBlt(rcDest.left, rcDest.top, cx, cy, dcMask,  0, 0,   ROP_DSno);
				dc.BitBlt(rcDest.left, rcDest.top, cx, cy, dcSource, 0, 0, ROP_DSa);
			}
			else
			{
				if(clrCheck != clrBlack)
				{
					ATLASSERT(dcSource.GetTextColor() == clrBlack);
					ATLASSERT(dcSource.GetBkColor() == clrWhite);
					dcSource.BitBlt(0, 0, size.cx, size.cy, dcMask, 0, 0, ROP_DSna);
				}
				dc.BitBlt(rcDest.left, rcDest.top, cx, cy, dcMask,  0,  0,  ROP_DSa);
				dc.BitBlt(rcDest.left, rcDest.top, cx, cy, dcSource, 0, 0, ROP_DSo);
			}
		}
		// restore all
		dc.SetTextColor(clrTextOld);
		dc.SetBkColor(clrBkOld);
		dcSource.SelectBitmap(hBmpOld);
		dcMask.SelectBitmap(hBmpOld1);
		if(hBmpCheck == NULL)
			bmp.DeleteObject();
		// draw pushed-in hilight
		//if(!m_bFlatMenus && !bDisabled)
		//{
		//	if(rc.right - rc.left > size.cx)
		//		::InflateRect(&rcDest, 1,1);   // inflate checkmark by one pixel all around
		//	dc.DrawEdge(&rcDest, BDR_SUNKENOUTER, BF_RECT);
		//}

		return TRUE;
	}

	struct MenuItem {
		CString name;
		UINT	nID;
		CRect	rect;
		POPUPITEMSTATES	state;
		bool	bSeparator;
		CMenuHandle submenu;
		bool	bChecked;
		void*	pUserData;

		MenuItem(CString str, UINT id, CRect rc, bool bSep = false, CMenuHandle menu = NULL) : name(str), nID(id), rect(rc), state(POPUPITEMSTATES::MPI_NORMAL), bSeparator(bSep), submenu(menu), bChecked(false), pUserData(nullptr)
		{	}
	};

	// Data members
	CMenuHandle m_menu;
	CFont	m_font;
	vector<MenuItem>	m_vecMenuItem;
	int		m_nHotIndex;
	IBasePopupMenu*	m_pSubMenu;
	int		m_nInheritMenuIndex;
};


////////////////////////////////////////////////////////////////////////
// カスタムポップアップメニューの基本クラス

class CBasePopupMenu : public CBasePopupMenuImpl<CBasePopupMenu>
{
public:

};

////////////////////////////////////////////////////////////////////////
// サブメニューに CRecentClosedTabPopupMenu を表示するためのクラス

class CFilePopupMenu : public CBasePopupMenuImpl<CFilePopupMenu>
{
public:
	// Overrides
	virtual IBasePopupMenu* CreateSubMenu(int nIndex) override;
};


// 前方宣言
class CRecentClosedTabList;
struct ChildFrameDataOnClose;

////////////////////////////////////////////////////////////////////////
// 最近閉じたタブ サブメニューの実装クラス
class CRecentClosedTabPopupMenu : 
	public CBasePopupMenuImpl<CRecentClosedTabPopupMenu>,
	public CMessageFilter
{
public:
	static void	SetRecentClosedTabList(CRecentClosedTabList* pList) { s_pRecentClosedTabList = pList; }

	// Overrides
	//virtual void	DoTrackPopupMenu(CMenuHandle menu, CPoint ptLeftBottom, HWND hWndParent) override;
	//virtual void	DoTrackSubPopupMenu(CMenuHandle menu, CRect rcClientItem, HWND hWndParent, int nInheritIndex) override;
	void DoPaint(CDCHandle dc);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	BEGIN_MSG_MAP( CRecentClosedTabPopupMenu )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		MSG_WM_MBUTTONDOWN( OnMButtonDown )
		MSG_WM_MOUSEMOVE( OnMouseMove )
		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnTooltipGetDispInfo)
		CHAIN_MSG_MAP( CBasePopupMenuImpl<CRecentClosedTabPopupMenu> )
	END_MSG_MAP()

	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnMButtonDown(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	LRESULT OnTooltipGetDispInfo(LPNMHDR pnmh);

private:
	void _initMenuItem();
	void _InitTooltip();
	int	ComputeWindowWidth();
	//int ComputeWindowHeight();

	CToolTipCtrl	m_tip;
	CString			m_strTipText;

	static CRecentClosedTabList*	s_pRecentClosedTabList;
};

////////////////////////////////////////////////////////////////////////
// コマンドバーのチェブロンメニュー表示用

class CChevronPopupMenu : public CBasePopupMenuImpl<CChevronPopupMenu>
{
public:
	CChevronPopupMenu(int nIndex) : m_nIndex(nIndex) {	}

	// Overrides
	virtual IBasePopupMenu* CreateSubMenu(int nIndex) override;
	virtual void	DoTrackPopupMenu(CMenuHandle menu, CPoint ptLeftBottom, HWND hWndParent) override;

private:
	int	m_nIndex;

};

////////////////////////////////////////////////////////////////////////
// お気に入りのルートメニュー実装

class CRootFavoritePopupMenu : public CBasePopupMenuImpl<CRootFavoritePopupMenu>
{
public:

	static void	LoadFavoriteBookmark();
	static void JoinSaveBookmarkThread();

	// Overrides
	virtual void	DoTrackPopupMenu(CMenuHandle menu, CPoint ptLeftBottom, HWND hWndParent) override;
	virtual void	DoTrackSubPopupMenu(CMenuHandle menu, CRect rcClientItem, HWND hWndParent, int nInheritIndex) override;

	BEGIN_MSG_MAP( CRootFavoritePopupMenu )
		MSG_WM_DESTROY	( OnDestroy	)
		MESSAGE_HANDLER_EX( WM_CLOSEBASESUBMENU, OnCloseBaseSubMenu )
		MESSAGE_HANDLER_EX( WM_UPDATESUBMENUITEMPOS, OnUpdateSubMenuItemPosition	)
		MESSAGE_HANDLER_EX( WM_SAVELINKBOOKMARK, OnSaveLinkBookmark	)
		CHAIN_MSG_MAP( CBasePopupMenuImpl<CRootFavoritePopupMenu> )
	END_MSG_MAP()

	void OnDestroy();
	LRESULT OnCloseBaseSubMenu(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateSubMenuItemPosition(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSaveLinkBookmark(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
private:
	static void	_SaveFavoriteBookmark();

	// Data members
	static vector<unique_ptr<LinkItem> >	s_BookmarkList;
	static CCriticalSection					s_csBookmarkLock;
	static bool								s_bSaveBookmark;
	static boost::thread					s_SaveBookmarkListThread;
	static bool								s_bBookmarkLoading;
#if _MSC_VER >= 1700
	static std::atomic<bool>				s_bCancel;
#else
	static bool	s_bCancel;
#endif
	CLinkPopupMenu*	m_pLinkSubMenu;
	HWND	m_hWndLinkBar;

};


