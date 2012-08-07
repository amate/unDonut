/**
*	@file	PopupMenu.h
*	@brief	カスタムポップアップメニュー
*/

#include "stdafx.h"
#include "PopupMenu.h"
#include <fstream>
#include <codecvt>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/thread.hpp>
#include <atlenc.h>
#include "option/FavoriteMenuDialog.h"
#include "MtlWeb.h"
#include "ExStyle.h"
#include "RecentClosedTabList.h"
#include "ChildFrame.h"

using namespace boost::property_tree;

HWND	IBasePopupMenu::s_hWndCommandBar = NULL;

//////////////////////////////////////////////////////////////////////////////////
// CFilePopupMenu
IBasePopupMenu* CFilePopupMenu::CreateSubMenu(int nIndex)
{
	enum { kRecentTabClosedPos = 11 };
	if (nIndex == kRecentTabClosedPos) {
		return new CRecentClosedTabPopupMenu;
	} else {
		return new CBasePopupMenu;
	}
}

////////////////////////////////////////////////////////////////////////
// CRecentClosedTabPopupMenu

CRecentClosedTabList*	CRecentClosedTabPopupMenu::s_pRecentClosedTabList = nullptr;

// Overrides
//void	CRecentClosedTabPopupMenu::DoTrackPopupMenu(CMenuHandle menu, CPoint ptLeftBottom, HWND hWndParent)
//{
//	s_hWndCommandBar = hWndParent;
//	_initMenuItem();
//
//	CRect	rcWindow;
//	rcWindow.right	= ComputeWindowWidth();
//	rcWindow.bottom	= ComputeWindowHeight();
//	rcWindow.MoveToXY(ptLeftBottom.x, ptLeftBottom.y);	
//	CRect rcWork = Misc::GetMonitorWorkArea(hWndParent);
//	if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
//		rcWindow.bottom	= rcWork.bottom;
//		rcWindow.right += ::GetSystemMetrics(SM_CXVSCROLL);
//	}
//	if (rcWork.right < rcWindow.right) {	// 右にはみ出る
//		int nWidth = rcWindow.Width();
//		rcWindow.MoveToX(rcWork.right - nWidth);
//	}
//
//	Create(GetDesktopWindow(), rcWindow, NULL, WS_POPUP | WS_BORDER , WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
//	ShowWindow(SW_SHOWNOACTIVATE);
//}
//
//void	CRecentClosedTabPopupMenu::DoTrackSubPopupMenu(CMenuHandle menu, CRect rcClientItem, HWND hWndParent, int nInheritIndex)
//{
//	m_nInheritMenuIndex = nInheritIndex;
//	_initMenuItem();
//
//	CRect rcWork = Misc::GetMonitorWorkArea(hWndParent);
//	CRect rcWindow;
//	int nTop	= rcClientItem.top - (kBoundBorder + kBoundMargin);
//	int nLeft	= rcClientItem.right	- kBiteWidth;
//	rcWindow.right	= ComputeWindowWidth();
//	rcWindow.bottom	= ComputeWindowHeight();
//	rcWindow.MoveToXY(nLeft, nTop);
//	CWindow(hWndParent).ClientToScreen(&rcWindow);
//	if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
//		int nOverHeight = rcWindow.bottom - rcWork.bottom;
//		rcWindow.MoveToY( rcWindow.top - nOverHeight );
//		if (rcWindow.top < rcWork.top)
//			rcWindow.top = rcWork.top;
//		rcWindow.bottom	= rcWork.bottom;
//		static const int s_nVScrollWidth = ::GetSystemMetrics(SM_CXVSCROLL);
//		rcWindow.right += s_nVScrollWidth;
//	}
//	if (rcWork.right < rcWindow.right) {	// 右にはみ出る
//		int nWidth = rcWindow.Width();
//		ClientToScreen(&rcClientItem);
//		//rcClientItem.left += kBiteWidth;
//		rcWindow.MoveToX(rcClientItem.left - nWidth);	// 反対側に出す
//	}
//
//	Create(hWndParent, rcWindow, nullptr, WS_POPUP | WS_BORDER, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST);
//	ShowWindow(SW_SHOWNOACTIVATE);
//}

void CRecentClosedTabPopupMenu::DoPaint(CDCHandle dc)
{
	CPoint ptOffset;
	GetScrollOffset(ptOffset);

	CRect rcClient;
	GetClientRect(rcClient);
	rcClient.MoveToY(ptOffset.y);
	//rcClient.bottom	+= ptOffset.y;

	CMemoryDC	memDC(dc, rcClient);

	HFONT hFontPrev = memDC.SelectFont(m_font);
	memDC.SetBkMode(TRANSPARENT);

	int nType = s_pRecentClosedTabList->GetMenuType();

	if (IsThemeNull() == false) {
		DrawThemeBackground(memDC, MENU_POPUPBACKGROUND, 0, &rcClient);
		CRect rcGutter;
		rcGutter.left = kBoundMargin + kIconWidth;
		rcGutter.right= rcGutter.left + kVerticalLineWidth;
		rcGutter.bottom= rcClient.bottom;
		DrawThemeBackground(memDC, MENU_POPUPGUTTER, 0, &rcGutter);

		for (auto it = m_vecMenuItem.begin(); it != m_vecMenuItem.end(); ++it) {
			MenuItem& item = *it;
			if (!(rcClient.PtInRect(item.rect.TopLeft()) || rcClient.PtInRect(item.rect.BottomRight())))
				continue;
			ChildFrameDataOnClose* pClosedTabData = static_cast<ChildFrameDataOnClose*>(it->pUserData);

			item.rect.right = rcClient.right - kBoundMargin;

			CRect rcMenuBackground = item.rect;
			if (nType == 2)
				rcMenuBackground.bottom = rcMenuBackground.top + (kItemHeight * 2);
			DrawThemeBackground(memDC, MENU_POPUPITEM, item.state, &rcMenuBackground);

			CRect	rcText = item.rect;
			rcText.left	= kLeftTextPos;
			rcText.right	-= kArrowWidth + kBoundMargin;
			rcText.bottom	= rcText.top + kItemHeight;

			if (nType == 2) {
				RECT rc = item.rect;
				rc.top = rc.bottom - 6;
				rc.left	= kBoundMargin + kIconWidth;
				DrawThemeBackground(memDC, MENU_POPUPSEPARATOR, 0, &rc);   // draw separator line
			
				DrawThemeText(memDC, MENU_POPUPITEM, item.state, pClosedTabData->strTitle, pClosedTabData->strTitle.GetLength(), DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_HIDEPREFIX, 0, &rcText);
				rcText.MoveToY(rcText.top + kItemHeight);

				DrawThemeText(memDC, MENU_POPUPITEM, item.state, item.name, item.name.GetLength(), DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_HIDEPREFIX, 0, &rcText);
			} else {
				DrawThemeText(memDC, MENU_POPUPITEM, item.state, item.name, item.name.GetLength(), DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_HIDEPREFIX, 0, &rcText);
			}

			//if (item.submenu) {
			//	CRect rcArrow = rcText;
			//	rcArrow.left	= rcText.right;
			//	rcArrow.right	= rcClient.right;
			//	int nState = MSM_NORMAL;
			//	if (item.state == MPI_DISABLED || item.state == MPI_DISABLEDHOT)
			//		nState = MSM_DISABLED;
			//	DrawThemeBackground(memDC, MENU_POPUPSUBMENU, nState, &rcArrow);
			//}
		}
	} else {
		memDC.FillRect(rcClient, COLOR_3DFACE);

		for (auto it = m_vecMenuItem.begin(); it != m_vecMenuItem.end(); ++it) {
			MenuItem& item = *it;
			if (!(rcClient.PtInRect(item.rect.TopLeft()) || rcClient.PtInRect(item.rect.BottomRight())))
				continue;
			ChildFrameDataOnClose* pClosedTabData = static_cast<ChildFrameDataOnClose*>(it->pUserData);
			
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

			if (item.state == MPI_HOT || item.state == MPI_DISABLEDHOT) {
				CRect rcMenuBackground = item.rect;
				if (nType == 2)
					rcMenuBackground.bottom = rcMenuBackground.top + (kItemHeight * 2);
				memDC.FillRect(rcMenuBackground, COLOR_HIGHLIGHT);
			}

			memDC.SetTextColor(item.state == MPI_HOT ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_MENUTEXT));

			CRect	rcText = item.rect;
			rcText.left	= kNoThemeLeftTextPos;
			rcText.right	-= kArrowWidth + kBoundMargin;
			rcText.bottom	= rcText.top + kItemHeight;
			if (nType == 2) {
				RECT rc = item.rect;
				rc.top = rc.bottom - 2;
				memDC.DrawEdge(&rc, EDGE_ETCHED, BF_TOP);   // draw separator line

				memDC.DrawText(pClosedTabData->strTitle, pClosedTabData->strTitle.GetLength(), rcText, DT_SINGLELINE | DT_VCENTER | DT_HIDEPREFIX);
				rcText.MoveToY(rcText.top + kItemHeight);
				memDC.DrawText(item.name, item.name.GetLength(), rcText, DT_SINGLELINE | DT_VCENTER | DT_HIDEPREFIX);
			} else {
				memDC.DrawText(item.name, item.name.GetLength(), rcText, DT_SINGLELINE | DT_VCENTER | DT_HIDEPREFIX);
			}
			//if (item.submenu) {
			//	CRect rcArrow = rcText;
			//	rcArrow.left	= rcText.right;
			//	rcArrow.right	= rcClient.right;
			//	enum { kcxArrow = 12, kcyArrow = 12 };
			//	CDC	memDC2 = ::CreateCompatibleDC(memDC);
			//	
			//	CBitmap bmpArrow = ::CreateCompatibleBitmap(memDC, kcxArrow, kcyArrow);
			//	HBITMAP hbmpPrev = memDC2.SelectBitmap(bmpArrow);
			//	UINT nState = DFCS_MENUARROW;
			//	memDC2.DrawFrameControl(CRect(0, 0, kcxArrow, kcyArrow), DFC_MENU, DFCS_MENUARROW);
			//	if (item.state == MPI_HOT) {
			//		CBitmap bmpWiteArrow = ::CreateCompatibleBitmap(memDC, kcxArrow, kcyArrow);
			//		CDC	memDC3 = ::CreateCompatibleDC(memDC);
			//		HBITMAP hbmpPrev2 = memDC3.SelectBitmap(bmpWiteArrow);
			//		memDC3.BitBlt(0, 0, kcxArrow, kcyArrow, memDC2, 0, 0, NOTSRCCOPY);

			//		memDC.TransparentBlt(rcClient.right - kcxArrow - 4, rcText.top + ((rcText.Height() - kcyArrow) / 2), kcxArrow, kcyArrow, memDC3, 0, 0, kcxArrow, kcyArrow, RGB(0, 0, 0));
			//		memDC3.SelectBitmap(hbmpPrev2);
			//	} else {
			//		memDC.TransparentBlt(rcClient.right - kcxArrow - 4, rcText.top + ((rcText.Height() - kcyArrow) / 2), kcxArrow, kcyArrow, memDC2, 0, 0, kcxArrow, kcyArrow, RGB(0xFF, 0xFF, 0xFF));
			//	}
			//	memDC2.SelectBitmap(hbmpPrev);
			//}
		}
	}


	memDC.SelectFont(hFontPrev);
}

BOOL CRecentClosedTabPopupMenu::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_DELETE) {
		if (m_nHotIndex != -1) {
			/* Deleteキーでカーソル下のアイテムを削除する */
			s_pRecentClosedTabList->RemoveFromList(m_vecMenuItem[m_nHotIndex].nID);
			m_vecMenuItem.clear();
			_initMenuItem();

			CPoint pt;
			::GetCursorPos(&pt);
			ScreenToClient(&pt);
			int nIndex = _HitTest(pt);
			m_nHotIndex = -1;
			_HotItem(nIndex);

			Invalidate(FALSE);
		}
	}
	return FALSE;
}

int CRecentClosedTabPopupMenu::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//SetMsgHandled(FALSE);

	_InitTooltip();

	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	__super::OnCreate(lpCreateStruct);

	m_nWheelLines = 1;
	SetScrollLine(1, kItemHeight * 3);

	return 0;
}

void CRecentClosedTabPopupMenu::OnDestroy()
{
	SetMsgHandled(FALSE);

	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->RemoveMessageFilter(this);
}

void CRecentClosedTabPopupMenu::OnSize(UINT nType, CSize size)
{
	DefWindowProc();

	if (size != CSize(0, 0)) {

	}
}

void CRecentClosedTabPopupMenu::OnMButtonDown(UINT nFlags, CPoint point)
{
	int nIndex = _HitTest(point);
	if (nIndex == -1)
		return ;
	CWindow(s_hWndCommandBar).GetTopLevelWindow().SendMessage(WM_COMMAND, m_vecMenuItem[nIndex].nID);
	m_vecMenuItem.clear();
	_initMenuItem();
	m_nHotIndex = -1;
	Invalidate(FALSE);
}

void CRecentClosedTabPopupMenu::OnMouseMove(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);

	int nIndex = _HitTest(point);
	if ((m_nHotIndex == -1 || m_nHotIndex != nIndex)) {
		m_tip.Activate(FALSE);
		m_tip.Activate(TRUE);
	}
}


LRESULT CRecentClosedTabPopupMenu::OnTooltipGetDispInfo(LPNMHDR pnmh)
{
	LPNMTTDISPINFO pntdi = (LPNMTTDISPINFO)pnmh;
	m_strTipText.Empty();
	if (CLinkPopupMenu::s_bNowShowRClickMenu)
		return 0;
	CPoint pt;
	::GetCursorPos(&pt);
	ScreenToClient(&pt);
	int nIndex = _HitTest(pt);
	if (nIndex != -1) {
		ChildFrameDataOnClose* pItem = static_cast<ChildFrameDataOnClose*>(m_vecMenuItem[nIndex].pUserData);
		m_strTipText = pItem->strTitle + _T("\r\n") + pItem->strURL;		
		pntdi->lpszText = m_strTipText.GetBuffer(0);
	}
	return 0;
}


void CRecentClosedTabPopupMenu::_initMenuItem()
{
	int nType = s_pRecentClosedTabList->GetMenuType();
	int nTop = kBoundMargin;
	int nCount = s_pRecentClosedTabList->GetRecentCount();
	CClientDC dc(NULL);
	HFONT prevFont = dc.SelectFont(m_font);
	for (int i = 0; i < nCount; ++i) {
		ChildFrameDataOnClose* pItem;
		s_pRecentClosedTabList->GetFromList(ID_RECENTDOCUMENT_FIRST + i, &pItem);

		CString url = pItem->strURL;
		::PathCompactPath(dc, url.GetBuffer(INTERNET_MAX_URL_LENGTH), kMaxMenuTextWidth);
		url.ReleaseBuffer();
		int nSrash = url.ReverseFind(_T('\\'));
		if (nSrash != -1)
			url.SetAt(nSrash, _T('/'));

		CRect rc;
		rc.top	= nTop;
		rc.left	= kBoundMargin;
		//rc.right= nWidht - kBoundMargin;
		CString name;
		switch (nType) {
		case 0:		// URL
			rc.bottom= nTop + kItemHeight;
			name = url;
			break;

		case 1:		// タイトル
			rc.bottom= nTop + kItemHeight;
			name = pItem->strTitle;
			break;

		case 2:		// タイトル + URL
			rc.bottom= nTop +  (kItemHeight * 2) + 3/*border*/;
			name = url;
			break;
		}
		nTop = rc.bottom;
#if _MSC_VER >= 1700
		m_vecMenuItem.emplace_back(name, ID_RECENTDOCUMENT_FIRST + i, rc);
#else
		m_vecMenuItem.push_back(MenuItem(name, ID_RECENTDOCUMENT_FIRST + i, rc));
#endif
		m_vecMenuItem.back().pUserData = static_cast<void*>(pItem);
	}
	dc.SelectFont(prevFont);
}

void	CRecentClosedTabPopupMenu::_InitTooltip()
{
	m_tip.Create(m_hWnd, 0, NULL, TTS_ALWAYSTIP, WS_EX_TRANSPARENT);
	m_tip.Activate(TRUE);
	CToolInfo ti(TTF_SUBCLASS | TTF_TRANSPARENT, m_hWnd);
	ti.hwnd	 = m_hWnd;
	if (RunTimeHelper::IsCommCtrl6() == false)
		ti.cbSize = sizeof(TOOLINFO) - sizeof(void*);
	ATLVERIFY(m_tip.AddTool(&ti));

	m_tip.SetMaxTipWidth(SHRT_MAX);
	m_tip.SetDelayTime(TTDT_AUTOPOP, 30 * 1000);
}

int	CRecentClosedTabPopupMenu::ComputeWindowWidth()
{
	int nMaxTextWidth = 0;
	int nType = s_pRecentClosedTabList->GetMenuType();
	for (auto it = m_vecMenuItem.begin(); it != m_vecMenuItem.end(); ++it) {
		ChildFrameDataOnClose* pClosedTabData = static_cast<ChildFrameDataOnClose*>(it->pUserData);
		int nNameWidth = MTL::MtlComputeWidthOfText(pClosedTabData->strTitle, m_font);
		int nUrlWidth = MTL::MtlComputeWidthOfText(pClosedTabData->strURL, m_font);
		switch (nType) {
		case 0:
			if (nMaxTextWidth < nUrlWidth)
				nMaxTextWidth = nUrlWidth;
			break;

		case 1:
			if (nMaxTextWidth < nNameWidth)
				nMaxTextWidth = nNameWidth;
			break;

		case 2:
			if (nMaxTextWidth < std::max(nNameWidth, nUrlWidth))
				nMaxTextWidth = std::max(nNameWidth, nUrlWidth);
			break;
		}
	}
	if (nMaxTextWidth == 0)
		nMaxTextWidth = kNoneTextWidth;
	if (kMaxMenuTextWidth < nMaxTextWidth)
		nMaxTextWidth = kMaxMenuTextWidth;
	nMaxTextWidth += kLeftTextPos + kTextMargin + kArrowWidth + (kBoundMargin * 2);
	return nMaxTextWidth;
}

//int CRecentClosedTabPopupMenu::ComputeWindowHeight()
//{
//	int nHeight = (kBoundMargin * 2) + (kBoundBorder * 2);
//	if (m_vecClosedTabItem.size() > 0) {			
//		nHeight += m_vecClosedTabItem[m_vecClosedTabItem.size() - 1].rect.bottom - kBoundMargin;
//	} else {
//		nHeight += kItemHeight;
//	}
//	return nHeight;
//}


//////////////////////////////////////////////////////////////////////////////////
// CChevronPopupMenu

IBasePopupMenu* CChevronPopupMenu::CreateSubMenu(int nIndex)
{
	if (m_vecMenuItem.size() == 7 && nIndex == 0)
		return new CFilePopupMenu;
	if (m_vecMenuItem.size() >= 4) {
		if (nIndex == (m_vecMenuItem.size() - 4)) {
			return new CRootFavoritePopupMenu;
		}
	}
	return new CBasePopupMenu;
}

void	CChevronPopupMenu::DoTrackPopupMenu(CMenuHandle menu, CPoint ptLeftBottom, HWND hWndParent)
{
	m_menu = menu;
	s_hWndCommandBar = hWndParent;

	/* _initMenuItem() */
	int nWidht = ComputeWindowWidth();
	int nTop = kBoundMargin;
	int nCount = m_menu.GetMenuItemCount();
	for (int i = m_nIndex; i < nCount; ++i) {
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


//////////////////////////////////////////////////////////////////////////////////
// CRootFavoritePopupMenu

vector<unique_ptr<LinkItem> >	CRootFavoritePopupMenu::s_BookmarkList;
CCriticalSection				CRootFavoritePopupMenu::s_csBookmarkLock;
bool							CRootFavoritePopupMenu::s_bSaveBookmark = false;
boost::thread					CRootFavoritePopupMenu::s_SaveBookmarkListThread;
bool							CRootFavoritePopupMenu::s_bBookmarkLoading = false;
#if _MSC_VER >= 1700
std::atomic<bool>				CRootFavoritePopupMenu::s_bCancel(false);
#else
bool							CRootFavoritePopupMenu::s_bCancel = false;
#endif

void	CRootFavoritePopupMenu::LoadFavoriteBookmark()
{
	CString FavoriteBookmarkFilePath = GetConfigFilePath(_T("FavoriteBookmark.xml"));
	if (::PathFileExists(FavoriteBookmarkFilePath) == FALSE) {
		/* 設定やお気に入りフォルダから s_BookmarkList を構築する */
		bool bUserFolder = false;
		enum { EMS_USER_DEFINED_FOLDER		= 0x00000100L };
		CIniFileIO	pr( g_szIniFileName, _T("FavoritesMenu") );
		if (pr.GetValue(_T("Style")) & EMS_USER_DEFINED_FOLDER) {
			CString userFolder = pr.GetStringUW(_T("UserFolder"));
			if (userFolder.GetLength() > 0) {
				CString strDir = Misc::GetFullPath_ForExe(userFolder);
				LinkImportFromFolder(strDir);
				pr.DeleteValue(_T("Style"));
				pr.DeleteValue(_T("UserFolder"));
				pr.DeleteValue(_T("Max_Text_Length"));
				pr.DeleteValue(_T("Max_Break_Count"));
				bUserFolder = true;
			}
		}
		if (bUserFolder == false) {
			CString favoritesFolder;
			if (MtlGetFavoritesFolder(favoritesFolder)) {
				boost::thread td([favoritesFolder]() {
					s_bBookmarkLoading = true;
					TIMERSTART();
					LinkImportFromFolder(favoritesFolder);
					TIMERSTOP(L"お気に入りフォルダからリンクの読み込み完了");
					s_bBookmarkLoading = false;
				});
			}
		}

		return ;
	}

	/* FavoriteBookmark.xml から s_BookmarkList を構築する */
	boost::thread td([]() {
		//DWORD dw = ::timeGetTime();
		s_bBookmarkLoading = true;
		CString FavoriteBookmarkFilePath = GetConfigFilePath(_T("FavoriteBookmark.xml"));
		try {
			std::wifstream	filestream(FavoriteBookmarkFilePath);
			if (!filestream) {
				s_bBookmarkLoading = false;
				return ;
			}
			TIMERSTART();
			
			filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));
			wptree	pt;
			read_xml(filestream, pt);

			if (auto optChild = pt.get_child_optional(L"FavoriteBookmark")) {
				wptree ptChild = optChild.get();
				_AddLinkItem(&s_BookmarkList, ptChild);
			}

		} catch (const boost::property_tree::ptree_error& error) {
			CString strError = _T("FavoriteBookmark.xmlの読み込みに失敗\n");
			strError += error.what();
			::MessageBox(NULL, strError, NULL, MB_ICONERROR);
			::MoveFileEx(FavoriteBookmarkFilePath, FavoriteBookmarkFilePath + _T(".error"), MOVEFILE_REPLACE_EXISTING);
		}
		TIMERSTOP(L"FavoriteBookmark.xml読み込み完了");
		//CString str;
		//str.Format(L"FavoriteBookmark : %d", ::timeGetTime() - dw);
		//::MessageBox(NULL, str, 0, 0);
		s_bBookmarkLoading = false;
	});
}

void	CRootFavoritePopupMenu::LinkImportFromFolder(LPCTSTR folder)
{
	ATLASSERT( ::PathIsDirectory(folder) );

	s_BookmarkList.clear();

	std::function<void (LPCTSTR, bool, LinkFolderPtr)> funcAddLink;
	funcAddLink = [&](LPCTSTR strPath, bool bDirectory, LinkFolderPtr pFolder) {
		unique_ptr<LinkItem> pItem(new LinkItem);
		if (bDirectory) {
			CString strDirPath = strPath;
			::PathRemoveBackslash(strDirPath.GetBuffer(MAX_PATH));
			strDirPath.ReleaseBuffer();
			pItem->strName = Misc::GetFileBaseName(strDirPath);
			pItem->pFolder	= new LinkFolder;
			MtlForEachObject_OldShell(strPath, std::bind(funcAddLink, std::placeholders::_1, std::placeholders::_2, pItem->pFolder));
		} else {
			if (Misc::GetFileExt(strPath).CompareNoCase(_T("url")) != 0)
				return ;

			pItem->strName	= Misc::GetFileBaseNoExt(strPath);
			pItem->strUrl	= MtlGetInternetShortcutUrl(strPath);
			pItem->icon		= CreateIconFromIDList(CItemIDList(strPath));
			DWORD dwExProp = 0;
			DWORD dwExPropOpt = 0;
			if (CExProperty::CheckExPropertyFlag(dwExProp, dwExPropOpt, strPath)) {
				pItem->bExPropEnable = true;
				pItem->dwExProp	= dwExProp;
				pItem->dwExPropOpt	= dwExPropOpt;
			}			
		}
		pFolder->push_back(std::move(pItem));
	};
	MtlForEachObject_OldShell(folder, std::bind(funcAddLink, std::placeholders::_1, std::placeholders::_2, &s_BookmarkList));

	// 名前順で並び替え
	CLinkPopupMenu::SortByName(&s_BookmarkList);

	_SaveFavoriteBookmark();
}

void	CRootFavoritePopupMenu::LinkExportToFolder(LPCTSTR folder, bool bOverWrite)
{
	CString strBaseFolder = folder;
	MtlMakeSureTrailingBackSlash(strBaseFolder);

	std::function<void (LPCTSTR, LinkFolderPtr)>	funcAddLinkFile;
	funcAddLinkFile = [&](LPCTSTR folder, LinkFolderPtr pFolder) {
		for (auto it = pFolder->begin(); it != pFolder->end(); ++it) {
			LinkItem& item = *it->get();
			if (item.pFolder) {
				CString strNewFolder = folder;
				strNewFolder += item.strName;
				MtlMakeSureTrailingBackSlash(strNewFolder);
				CreateDirectory(strNewFolder, NULL);
				funcAddLinkFile(strNewFolder, item.pFolder);

			} else {
				CString LinkFilePath = folder;
				LinkFilePath += item.strName + _T(".url");
				if (bOverWrite == false) {
					int i = 0;
					while (::PathFileExists(LinkFilePath)) {
						LinkFilePath.Format(_T("%s%s(%d).url"), folder, item.strName, i);
						++i;
					}
				}
				if (MtlCreateInternetShortcutFile(LinkFilePath, item.strUrl)) {
					if (item.bExPropEnable) {
						CIniFileO	pr(LinkFilePath, DONUT_SECTION);
						pr.SetValue(true, EXPROP_KEY_ENABLED);
						pr.SetValue(item.dwExProp.get(), EXPROP_KEY);
						pr.SetValue(item.dwExPropOpt.get(), EXPROP_OPTION);
					}
				}

			}
		}
	};
	funcAddLinkFile(strBaseFolder, &s_BookmarkList);
}

void CRootFavoritePopupMenu::JoinSaveBookmarkThread()
{
	if (s_SaveBookmarkListThread.joinable())
		s_SaveBookmarkListThread.join();
}

void	CRootFavoritePopupMenu::DoTrackPopupMenu(CMenuHandle menu, CPoint ptLeftBottom, HWND hWndParent)
{
	m_menu = menu;
	s_hWndCommandBar = hWndParent;
	_initMenuItem();

	if (s_bBookmarkLoading == false) {
		CLinkPopupMenu* pSubMenu = new CLinkPopupMenu(&s_BookmarkList);

		int nParentWidth = ComputeWindowWidth();
		int nChildWidth = pSubMenu->ComputeWindowWidth(); 
		CRect	rcWindow;
		rcWindow.right	= (nChildWidth > nParentWidth) ? nChildWidth : nParentWidth;
		rcWindow.right	+= kBoundBorder * 2;
		rcWindow.bottom	= ComputeWindowHeight() - kItemHeight + pSubMenu->ComputeWindowHeight();
		rcWindow.MoveToXY(ptLeftBottom.x, ptLeftBottom.y);	

		CRect rcSubMenu;
		rcSubMenu.right	= pSubMenu->ComputeWindowWidth();
		rcSubMenu.bottom= pSubMenu->ComputeWindowHeight();
		rcSubMenu.MoveToY(ComputeWindowHeight() - kItemHeight - (kBoundBorder * 2));

		CRect rcWork = Misc::GetMonitorWorkArea(hWndParent);
		if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
			rcWindow.bottom	= rcWork.bottom;
			rcWindow.right += ::GetSystemMetrics(SM_CXVSCROLL);
			rcSubMenu.right+= ::GetSystemMetrics(SM_CXVSCROLL);
		}
		if (rcWork.right < rcWindow.right) {	// 右にはみ出る
			int nWidth = rcWindow.Width();
			rcWindow.MoveToX(rcWork.right - nWidth);
		}
	
		m_vecMenuItem.pop_back();	// dummyMenuItem erase

		Create(GetDesktopWindow(), rcWindow, NULL, WS_POPUP | WS_BORDER , WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
		ShowWindow(SW_SHOWNOACTIVATE);

		CRect rcTemp = rcWindow;
		ScreenToClient(&rcTemp);
		rcSubMenu.bottom = rcTemp.bottom;
		if (rcSubMenu.right < rcWindow.Width())
			rcSubMenu.right = rcWindow.Width() - kBoundMargin;

		pSubMenu->Create(m_hWnd, rcSubMenu, NULL, WS_CHILD);
		pSubMenu->ShowWindow(SW_SHOWNOACTIVATE);
		m_pLinkSubMenu = pSubMenu;

		m_hWndLinkBar = pSubMenu->SetLinkBarHWND(m_hWnd/*hWndParent*/);
	} else {
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
		m_vecMenuItem.back().name	= _T("お気に入りを読み込み中です...");

		Create(GetDesktopWindow(), rcWindow, NULL, WS_POPUP | WS_BORDER , WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
		ShowWindow(SW_SHOWNOACTIVATE);
	}
}

void	CRootFavoritePopupMenu::DoTrackSubPopupMenu(CMenuHandle menu, CRect rcClientItem, HWND hWndParent, int nInheritIndex)
{
	m_menu = menu;
	s_hWndCommandBar = hWndParent;
	_initMenuItem();

	int nTop	= rcClientItem.top - (kBoundBorder + kBoundMargin);
	int nLeft	= rcClientItem.right	- kBiteWidth;
	if (s_bBookmarkLoading == false) {
		CLinkPopupMenu* pSubMenu = new CLinkPopupMenu(&s_BookmarkList);

		int nParentWidth = ComputeWindowWidth();
		int nChildWidth = pSubMenu->ComputeWindowWidth(); 
		CRect	rcWindow;
		rcWindow.right	= (nChildWidth > nParentWidth) ? nChildWidth : nParentWidth;
		rcWindow.right	+= kBoundBorder * 2;
		rcWindow.bottom	= ComputeWindowHeight() - kItemHeight + pSubMenu->ComputeWindowHeight();
		rcWindow.MoveToXY(nLeft, nTop);	
		CWindow(hWndParent).ClientToScreen(&rcWindow);

		CRect rcSubMenu;
		rcSubMenu.right	= pSubMenu->ComputeWindowWidth();
		rcSubMenu.bottom= pSubMenu->ComputeWindowHeight();
		rcSubMenu.MoveToY(ComputeWindowHeight() - kItemHeight - (kBoundBorder * 2));

		CRect rcWork = Misc::GetMonitorWorkArea(hWndParent);
		if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
			rcWindow.bottom	= rcWork.bottom;
			rcWindow.right += ::GetSystemMetrics(SM_CXVSCROLL);
			rcSubMenu.right+= ::GetSystemMetrics(SM_CXVSCROLL);
		}
		if (rcWork.right < rcWindow.right) {	// 右にはみ出る
			int nWidth = rcWindow.Width();
			CWindow(hWndParent).ClientToScreen(&rcClientItem);
			rcWindow.MoveToX(rcClientItem.left - nWidth);	// 反対側に出す
		}
	
		m_vecMenuItem.pop_back();	// dummyMenuItem erase

		Create(hWndParent, rcWindow, NULL, WS_POPUP | WS_BORDER , WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
		ShowWindow(SW_SHOWNOACTIVATE);

		CRect rcTemp = rcWindow;
		ScreenToClient(&rcTemp);
		rcSubMenu.bottom = rcTemp.bottom;
		if (rcSubMenu.right < rcWindow.Width())
			rcSubMenu.right = rcWindow.Width() - kBoundMargin;

		pSubMenu->Create(m_hWnd, rcSubMenu, NULL, WS_CHILD);
		pSubMenu->ShowWindow(SW_SHOWNOACTIVATE);
		m_pLinkSubMenu = pSubMenu;

		m_hWndLinkBar = pSubMenu->SetLinkBarHWND(m_hWnd/*hWndParent*/);
	} else {
		CRect	rcWindow;
		rcWindow.right	= ComputeWindowWidth();
		rcWindow.bottom	= ComputeWindowHeight();
		rcWindow.MoveToXY(nLeft, nTop);	
		CRect rcWork = Misc::GetMonitorWorkArea(hWndParent);
		if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
			rcWindow.bottom	= rcWork.bottom;
			rcWindow.right += ::GetSystemMetrics(SM_CXVSCROLL);
		}
		if (rcWork.right < rcWindow.right) {	// 右にはみ出る
			int nWidth = rcWindow.Width();
			ClientToScreen(&rcClientItem);
			rcWindow.MoveToX(rcClientItem.left - nWidth);	// 反対側に出す
		}
		m_vecMenuItem.back().name	= _T("お気に入りを読み込み中です...");

		Create(GetDesktopWindow(), rcWindow, NULL, WS_POPUP | WS_BORDER , WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
		ShowWindow(SW_SHOWNOACTIVATE);
	}
}

void CRootFavoritePopupMenu::OnDestroy()
{
	SetMsgHandled(FALSE);

	m_pLinkSubMenu->SetLinkBarHWND(m_hWndLinkBar);

	if (s_bSaveBookmark) {
		_SaveFavoriteBookmark();
		s_bSaveBookmark = false;
	}
}

LRESULT CRootFavoritePopupMenu::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	HWND hWnd = ::WindowFromPoint(pt);
	if (hWnd) {
		bool bChildWnd = false;
		if (m_pLinkSubMenu->m_hWnd == hWnd) {
			bChildWnd = true;
		} else {
			CString className;
			GetClassName(hWnd, className.GetBuffer(128), 128);
			className.ReleaseBuffer();
			if (className == _T("DonutBasePopupMenu") || className == _T("DonutLinkPopupMenu"))
				bChildWnd = true;
		}
		if (bChildWnd) {
			::SendMessage(hWnd, WM_MOUSEWHEEL, wParam, lParam);
		}
	}
	return 0;
}

LRESULT CRootFavoritePopupMenu::OnCloseBaseSubMenu(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	::SendMessage(s_hWndCommandBar, WM_CLOSEBASESUBMENU, 0, 0);
	return 0;
}

LRESULT CRootFavoritePopupMenu::OnUpdateSubMenuItemPosition(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_pLinkSubMenu->SendMessage(WM_UPDATESUBMENUITEMPOS);
	return 0;
}

LRESULT CRootFavoritePopupMenu::OnSaveLinkBookmark(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	s_bSaveBookmark = true;	
	return 0;
}


void	CRootFavoritePopupMenu::_SaveFavoriteBookmark()
{
	DWORD dwTime = ::timeGetTime();
	LinkFolderPtr pvecBookmark = new vector<unique_ptr<LinkItem> >;
	//s_BookmarkList
	std::function<void (LinkFolderPtr, LinkFolderPtr)> funcCopy;
	funcCopy = [&](LinkFolderPtr pFolder, LinkFolderPtr pFolderTo) {
		for (auto it = pFolder->begin(); it != pFolder->end(); ++it) {
			LinkItem& item = *(*it);
			unique_ptr<LinkItem> pItem(new LinkItem(item));
			if (item.pFolder) {
				pItem->pFolder = new LinkFolder;
				funcCopy(item.pFolder, pItem->pFolder);
			} 
			pFolderTo->push_back(std::move(pItem));
		}
	};
	funcCopy(&s_BookmarkList, pvecBookmark); 
	ATLTRACE(_T("SaveFavoriteBookmark preparation : %d\n"), ::timeGetTime() - dwTime);

	s_SaveBookmarkListThread.swap(boost::thread([pvecBookmark]() {
		for (;;) {
			if (s_csBookmarkLock.TryEnter()) {
				DWORD dwTime = ::timeGetTime();
				CString LinkBookmarkFilePath = GetConfigFilePath(_T("FavoriteBookmark.xml"));
				CString tempPath = GetConfigFilePath(_T("FavoriteBookmark.temp.xml"));
				try {
					std::wofstream	filestream(tempPath);
					if (!filestream) {
						s_csBookmarkLock.Leave();
#if _MSC_VER >= 1700
						s_bCancel.store(false);
#else
						s_bCancel = false;
#endif
						delete pvecBookmark;
						return ;
					}
		
					wptree	pt;
					_AddPtree(pt.add(L"FavoriteBookmark", L""), pvecBookmark, &CRootFavoritePopupMenu::s_bCancel);
					
					filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));
					using namespace boost::property_tree::xml_parser;
					write_xml(filestream, pt, xml_writer_make_settings(L' ', 2, widen<wchar_t>("UTF-8")));
					filestream.close();
					::MoveFileEx(tempPath, LinkBookmarkFilePath, MOVEFILE_REPLACE_EXISTING);

				} catch (const boost::property_tree::ptree_error& error) {
					CString strError = _T("FavoriteBookmark.xmlへの書き込みに失敗\n");
					strError += error.what();
					::MessageBox(NULL, strError, NULL, MB_ICONERROR);
				}
				s_csBookmarkLock.Leave();
#if _MSC_VER >= 1700
				ATLTRACE(_T("SaveFavoriteBookmark : %d  (Cancel:%d)\n"), ::timeGetTime() - dwTime, s_bCancel.load());
				s_bCancel.store(false);
#else
				s_bCancel = false;
#endif
				delete pvecBookmark;
				break;

			} else {
				// 他のスレッドが保存処理を実行中...
				TRACEIN(_T("_SaveLinkBookmark : TryEnter failed"));
#if _MSC_VER >= 1700
				s_bCancel.store(true);
#else
				s_bCancel = true;
#endif
				::Sleep(100);
				continue;
			}
		}
	}));
}

