/**
*	@file	BasePopupMenu.h
*	@brief	ポップアップメニューの基本クラス
*/

#pragma once

#include <atlscrl.h>
#include <atltheme.h>
#include "MtlWin.h"		// for CTrackMouseLeave
#include "MtlMisc.h"	// for MtlComputeWidthOfText
#include "Misc.h"		// for GetMonitorWorkArea


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
	virtual bool	PreTranslateMessage(MSG* pMsg) { return true; }

	static HWND	s_hWndCommandBar;
};


class CBasePopupMenuBase
{
public:
	// Constants
	enum { 		
		kBoundMargin = 2,
		kBoundBorder = 1,
		kBiteWidth	= 3,	// どれ程サブメニューを食い込ませるか

		kIconWidth	= 26,
		kVerticalLineWidth = 2,
		kTextMargin	= 4,
		kLeftTextPos = kBoundMargin + kIconWidth + kVerticalLineWidth + kTextMargin,
		kArrowWidth	= 25,	// => の幅

		kNoneTextWidth = 50,
		kMaxMenuTextWidth	= 400,

	};

	/// ポップアップウィンドウの表示位置を計算して返す
	static	CRect	CalcPopupWindowPos(const CPoint& ptLeftBottom, const CSize& size, HWND hWndParent)
	{
		CRect	rcWindow;
		rcWindow.right	= size.cx;
		rcWindow.bottom	= size.cy;
		rcWindow.MoveToXY(ptLeftBottom.x, ptLeftBottom.y);	
		CRect rcWork = Misc::GetMonitorWorkArea(hWndParent);
		if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
			rcWindow.bottom	= rcWork.bottom;	// モニターより下に表示されないようにする
			rcWindow.right += ::GetSystemMetrics(SM_CXVSCROLL);
		}
		if (rcWork.right < rcWindow.right) {	// 右にはみ出る
			int nWidth = rcWindow.Width();
			rcWindow.MoveToX(rcWork.right - nWidth);	// はみ出た分左にずらす
		}
		return rcWindow;
	}

	/// サブポップアップウィンドウの表示位置を計算して返す
	static CRect	CalcSubPopupWindowPos(CRect rcClientItem, const CSize& size, HWND hWndParent)
	{
		CRect rcWork = Misc::GetMonitorWorkArea(hWndParent);
		CRect rcWindow;
		int nTop	= rcClientItem.top - (kBoundBorder + kBoundMargin);
		int nLeft	= rcClientItem.right	- kBiteWidth;
		rcWindow.right	= size.cx;
		rcWindow.bottom	= size.cy;
		rcWindow.MoveToXY(nLeft, nTop);
		CWindow(hWndParent).ClientToScreen(&rcWindow);
		if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
			int nOverHeight = rcWindow.bottom - rcWork.bottom;
			rcWindow.MoveToY( rcWindow.top - nOverHeight );	// はみ出た分上にずらす
			if (rcWindow.top < rcWork.top)		// ずらした分モニターからはみ出た
				rcWindow.top = rcWork.top;		// ので、はみ出ないようにする
			rcWindow.bottom	= rcWork.bottom;
			static const int s_nVScrollWidth = ::GetSystemMetrics(SM_CXVSCROLL);
			rcWindow.right += s_nVScrollWidth;
		}
		if (rcWork.right < rcWindow.right) {	// 右にはみ出る
			int nWidth = rcWindow.Width();
			CWindow(hWndParent).ClientToScreen(&rcClientItem);
			//rcClientItem.left += kBiteWidth;
			rcWindow.MoveToX(rcClientItem.left - nWidth);	// 反対側に出す
		}
		return rcWindow;
	}

	/// テキストの幅からウィンドウの幅を計算して返す
	static int	CalcWindowWidth(int nMaxTextWidth)
	{
		if (nMaxTextWidth == 0)
			nMaxTextWidth = kNoneTextWidth;
		else if (kMaxMenuTextWidth < nMaxTextWidth)
			nMaxTextWidth = kMaxMenuTextWidth;
		return kLeftTextPos + nMaxTextWidth + kTextMargin + kArrowWidth + (kBoundMargin * 2);
	}
};


typedef CWinTraits<WS_POPUP | WS_BORDER | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST>	CBasePopupWinTraits;

template<typename T>
class CBasePopupMenuImpl : 
	public CBasePopupMenuBase,
	public CScrollWindowImpl<T, CWindow, CBasePopupWinTraits>,
	public CThemeImpl<T>,
	public CTrackMouseLeave<T>,
	public IBasePopupMenu
{
public:
	DECLARE_WND_CLASS_EX(_T("DonutBasePopupMenu"), CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW, COLOR_MENU)

	typedef CScrollWindowImpl<T, CWindow, CBasePopupWinTraits>	baseClass;

	// Constants
	enum {
		kItemHeight = 22, 
		kSeparatorHeight = 6,
		kCheckWidth = 22,

		kNoThemeLeftTextPos = kBoundMargin + kCheckWidth,
		
		kcxIcon = 16,
		kcyIcon = 16,
		kTopIconMargin = 3,
		kLeftIconMargin = kBoundMargin + 4,

		kSubMenuPopupCloseTimerID = 1,
		kSubMenuPopupCloseTime	= 500,
		kSubMenuPopupTimerID = 2,
		kSubMenuPopupTime	= 250,

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
		InitMenuItem();

		/* 表示位置を計算する */
		CRect	rcWindow = CalcPopupWindowPos(ptLeftBottom, CSize(ComputeWindowWidth(), ComputeWindowHeight()), hWndParent);

		Create(GetDesktopWindow(), rcWindow);
		ShowWindow(SW_SHOWNOACTIVATE);
	}

	virtual void	DoTrackSubPopupMenu(CMenuHandle menu, CRect rcClientItem, HWND hWndParent, int nInheritIndex) override
	{
		m_menu = menu;
		m_nInheritMenuIndex = nInheritIndex;
		InitMenuItem();

		/* 表示位置を計算する */
		CRect rcWindow = CalcSubPopupWindowPos(rcClientItem, CSize(ComputeWindowWidth(), ComputeWindowHeight()), hWndParent);

		Create(hWndParent, rcWindow, nullptr);
		ShowWindow(SW_SHOWNOACTIVATE);
	}

	virtual int	ComputeWindowWidth()
	{
		int nMaxTextWidth = 0;
		for (auto it = m_vecMenuItem.begin(); it != m_vecMenuItem.end(); ++it) {
			nMaxTextWidth = std::max(nMaxTextWidth, MTL::MtlComputeWidthOfText(it->name, m_font));
		}		
		return CalcWindowWidth(nMaxTextWidth);
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

	void DoPaint(CDCHandle dc);

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

	virtual bool	PreTranslateMessage(MSG* pMsg) override
	{
		if (pMsg->message == WM_KEYDOWN) {
			UINT nChar = (UINT)pMsg->wParam;
			if (m_pSubMenu) {
				if (m_pSubMenu->PreTranslateMessage(pMsg)) {
					if (pMsg->wParam == VK_RIGHT && pMsg->lParam == -1)
						return true;
					if ((pMsg->wParam == VK_LEFT || nChar == VK_ESCAPE) && m_pSubMenu) {
						m_pSubMenu->DestroyWindow();
						m_pSubMenu = nullptr;
						pMsg->wParam = 0;	// 打ち止めにしとく
					}
					return true;
				}
			}
			
			if (nChar == VK_DOWN || nChar == VK_UP) {	// 選択アイテムのを変える
				int nNewHotIndex = m_nHotIndex;
				if (nNewHotIndex == -1) {
					nNewHotIndex = 0;
				} else {
					if (nChar == VK_DOWN) {
						++nNewHotIndex;
						int nCount = (int)m_vecMenuItem.size();
						if (nCount <= nNewHotIndex)		// 一番下まで行ったら一番上に戻す
							nNewHotIndex = 0;						
						if (m_vecMenuItem[nNewHotIndex].bSeparator)	// セパレーターは飛ばす
							++nNewHotIndex;

					} else if (nChar == VK_UP) {
						--nNewHotIndex;
						if (nNewHotIndex < 0)			// 一番上まで行ったら一番下に戻す
							nNewHotIndex = m_vecMenuItem.size() - 1;
						if (m_vecMenuItem[nNewHotIndex].bSeparator)	// セパレーターは飛ばす
							--nNewHotIndex;
					}
				}
				_HotItem(nNewHotIndex);
				return true;
			} else if (nChar == VK_RIGHT || nChar == VK_RETURN) {			// 選択アイテムのサブメニューを表示する
				// サブメニューを表示する
				if (_DoTrackSubPopupMenu(m_nHotIndex)) {
					if (m_pSubMenu) {
						// 一番上のアイテムをHot状態にする
						MSG msg = *pMsg;
						msg.wParam	= VK_DOWN;
						m_pSubMenu->PreTranslateMessage(&msg);
						return true;
					} else {	// 無効アイテム
						pMsg->lParam = -1;
						return true;
					}
				} else if (nChar == VK_RETURN && m_nHotIndex != -1) {
					// アイテムを実行...
					MenuItem& item = m_vecMenuItem[m_nHotIndex];
					if (item.state & (POPUPITEMSTATES::MPI_HOT | POPUPITEMSTATES::MPI_NORMAL)) {
						// メニューを閉じる
						_CloseBaseSubMenu();
						CWindow(s_hWndCommandBar).GetTopLevelWindow().PostMessage(WM_COMMAND, item.nID);
						return true;
					}
				} else if (nChar == VK_RIGHT) {
					if (m_pSubMenu) {
						// 一番上のアイテムをHot状態にする
						MSG msg = *pMsg;
						msg.wParam	= VK_DOWN;
						m_pSubMenu->PreTranslateMessage(&msg);
					} else {	// 無効アイテム
						pMsg->lParam = -1;
					}
					return true;
				}
			} else if (nChar == VK_LEFT && nChar == VK_ESCAPE && m_pSubMenu == nullptr) {		// 自分が最上階のポップアップメニューなら自分自身を親に閉じてもらう
				return true;

			} else {	// プレフィックス処理
				int nCount = (int)m_vecMenuItem.size();
				std::vector<int>	vecSamePrefixIndex;
				for (int i = 0; i < nCount; ++i) {
					if (m_vecMenuItem[i].prefix == (TCHAR)nChar) 
						vecSamePrefixIndex.push_back(i);
				}
				if (vecSamePrefixIndex.size() > 0) {
					if (vecSamePrefixIndex.size() == 1) {
						// サブメニューを表示する
						if (_DoTrackSubPopupMenu(vecSamePrefixIndex.front())) {
							if (m_pSubMenu) {
								// 一番上のアイテムをHot状態にする
								MSG msg = *pMsg;
								msg.wParam	= VK_DOWN;
								m_pSubMenu->PreTranslateMessage(&msg);
								return true;
							}
						}
						// アイテムを実行...
						MenuItem& item = m_vecMenuItem[vecSamePrefixIndex.front()];
						// メニューを閉じる
						_CloseBaseSubMenu();
						if (item.state & (POPUPITEMSTATES::MPI_HOT | POPUPITEMSTATES::MPI_NORMAL)) {
							CWindow(s_hWndCommandBar).GetTopLevelWindow().PostMessage(WM_COMMAND, item.nID);
						}
					} else {
						for (int nIndex : vecSamePrefixIndex) {
							if (m_nHotIndex < nIndex) {
								_HotItem(nIndex);
								return true;
							}
						}
						_HotItem(vecSamePrefixIndex.front());
					}
				}
				return true;
			}
		}
		return false;
	}

	BEGIN_MSG_MAP( CBasePopupMenuImpl<T> )
		MSG_WM_CREATE		( OnCreate )
		MSG_WM_DESTROY		( OnDestroy )
		MSG_WM_TIMER		( OnTimer )
		MSG_WM_ERASEBKGND	( OnEraseBkgnd )
		MSG_WM_MOUSEACTIVATE( OnMouseActivate )
		MSG_WM_KEYDOWN		( OnKeyDown )
		MSG_WM_LBUTTONDOWN	( OnLButtonDown )
		MSG_WM_LBUTTONUP	( OnLButtonUp	)
		CHAIN_MSG_MAP( baseClass )
		CHAIN_MSG_MAP( CThemeImpl<T> )
		CHAIN_MSG_MAP( CTrackMouseLeave<T> )
		CHAIN_MSG_MAP_ALT(baseClass, 1)
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
		SetMsgHandled(FALSE);
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
			OnClick(pt);	// popup
		}
	}


	BOOL OnEraseBkgnd(CDCHandle dc) { return TRUE; }

	/// クリックでアクティブにならないようにする
	int  OnMouseActivate(CWindow wndTopLevel, UINT nHitTest, UINT message) { return MA_NOACTIVATE; }

	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
	}

	void OnLButtonDown(UINT nFlags, CPoint point) {
		OnClick(point, false);
	}
	void OnLButtonUp(UINT nFlags, CPoint point) {
		OnClick(point, true);
	}

protected:
	/// m_vecMenuItem を初期化する
	virtual void	InitMenuItem()
	{
		// メニューを初期化する(チェックや有効/無効などを設定してもらう)
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

	virtual void	OnClick(const CPoint& pt, bool bLButtonUp = false)
	{
		int nIndex = _HitTest(pt);
		if (nIndex == -1)
			return ;

		if (_DoTrackSubPopupMenu(nIndex)) {	// サブメニューをポップアップさせる
			return ;
		} else if (bLButtonUp) {
			// メニューを閉じる
			_CloseBaseSubMenu();

			// アイテムを実行...
			MenuItem& item = m_vecMenuItem[nIndex];
			CWindow(s_hWndCommandBar).GetTopLevelWindow().PostMessage(WM_COMMAND, item.nID);
		}
	}

	bool	_DoTrackSubPopupMenu(int nIndex)
	{
		if (_IsValidIndex(nIndex) == false)
			return false;

		MenuItem& item = m_vecMenuItem[nIndex];
		if (m_pSubMenu) {
			if (m_pSubMenu->GetInheritMenuIndex() == nIndex)
				return true;	// 派生元フォルダなら何もしない
			m_pSubMenu->DestroyWindow();
			m_pSubMenu = nullptr;
		}
		// 無効アイテムなので実行しない
		if (item.bSeparator || item.state == MPI_DISABLEDHOT)
			return true;

		// サブメニューをポップアップさせる
		if (item.submenu) {
			_HotItem(nIndex);
			m_pSubMenu = CreateSubMenu(nIndex);
			CRect rcClientItem = _GetClientItemRect(item.rect);
			m_pSubMenu->DoTrackSubPopupMenu(item.submenu, rcClientItem, m_hWnd, nIndex);
			return true;
		} else {
			return false;
		}
	}


	bool	_IsValidIndex(int nIndex) {
		return 0 <= nIndex && nIndex < static_cast<int>(m_vecMenuItem.size());
	}
	int		_HitTest(const CPoint& pt)
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
			ScrollToView(item.rect);
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

		{
			const DWORD ROP_DSno = 0x00BB0226L;
			const DWORD ROP_DSa = 0x008800C6L;
			const DWORD ROP_DSo = 0x00EE0086L;
			const DWORD ROP_DSna = 0x00220326L;

			// draw mask
			RECT rcSource = { 0, 0, min(size.cx, rc.right - rc.left), min(size.cy, rc.bottom - rc.top) };
			dcMask.DrawFrameControl(&rcSource, DFC_MENU, bRadio ? DFCS_MENUBULLET : DFCS_MENUCHECK);

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

		return TRUE;
	}

	/// メニューアイテム一つを表す
	struct MenuItem {
		CString name;
		UINT	nID;
		CRect	rect;
		POPUPITEMSTATES	state;
		bool	bSeparator;
		CMenuHandle submenu;
		bool	bChecked;
		void*	pUserData;
		CIcon	icon;
		TCHAR	prefix;

		MenuItem(CString str, UINT id, CRect rc, bool bSep = false, CMenuHandle menu = NULL) : 
			name(str), nID(id), rect(rc), state(POPUPITEMSTATES::MPI_NORMAL), bSeparator(bSep), submenu(menu), bChecked(false), pUserData(nullptr)
		{
			int nPos = name.Find(_T('&'));
			if (nPos != -1) 
				prefix = name.Mid(nPos + 1, 1)[0];
		}
	};

	// Data members
	CMenuHandle m_menu;
	CFont	m_font;
	vector<MenuItem>	m_vecMenuItem;
	int		m_nHotIndex;
	IBasePopupMenu*	m_pSubMenu;
	int		m_nInheritMenuIndex;
};


template<typename T>
void CBasePopupMenuImpl<T>::DoPaint(CDCHandle dc)
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
			if (item.icon) {
				POINT ptIcon;
				ptIcon.x	= kLeftIconMargin;
				ptIcon.y	= item.rect.top + kTopIconMargin;
				item.icon.DrawIconEx(memDC, ptIcon, CSize(kcxIcon, kcyIcon));
			}
			if (item.bChecked) {
				CRect rcCheck = item.rect;
				rcCheck.right	= rcCheck.left + kCheckWidth;
				DrawThemeBackground(memDC, MENU_POPUPCHECKBACKGROUND, MCB_NORMAL, &rcCheck);
				DrawThemeBackground(memDC, MENU_POPUPCHECK, MC_CHECKMARKNORMAL, &rcCheck);
			}
		}
	} else {
		memDC.FillRect(rcClient, COLOR_3DFACE);

		for (auto it = m_vecMenuItem.begin(); it != m_vecMenuItem.end(); ++it) {
			MenuItem& item = *it;
			item.rect.right = rcClient.right - kBoundMargin;

			if (item.icon) {
				POINT ptIcon;
				ptIcon.x	= kLeftIconMargin;
				ptIcon.y	= item.rect.top + kTopIconMargin;
				item.icon.DrawIconEx(memDC, ptIcon, CSize(kcxIcon, kcyIcon));
			}
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
	}


	memDC.SelectFont(hFontPrev);
}

////////////////////////////////////////////////////////////////////////
// カスタムポップアップメニューの基本クラス

class CBasePopupMenu : public CBasePopupMenuImpl<CBasePopupMenu>
{
public:

};

