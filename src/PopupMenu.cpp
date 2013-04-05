/**
*	@file	PopupMenu.h
*	@brief	カスタムポップアップメニュー
*/

#include "stdafx.h"
#include "PopupMenu.h"
#include <fstream>
#include <codecvt>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\xml_parser.hpp>
#include <boost\thread.hpp>
#include <boost\format.hpp>
#include <atlenc.h>
#include "option\MenuDialog.h"
#include "MtlWeb.h"
#include "ExStyle.h"
#include "RecentClosedTabList.h"
#include "ChildFrame.h"
#include "MainFrame.h"
#include "DonutTabList.h"

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
			
				DrawThemeText(memDC, MENU_POPUPITEM, item.state, pClosedTabData->strTitle, pClosedTabData->strTitle.GetLength(), DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX, 0, &rcText);
				rcText.MoveToY(rcText.top + kItemHeight);

				DrawThemeText(memDC, MENU_POPUPITEM, item.state, item.name, item.name.GetLength(), DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX, 0, &rcText);
			} else {
				DrawThemeText(memDC, MENU_POPUPITEM, item.state, item.name, item.name.GetLength(), DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX, 0, &rcText);
			}
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

				memDC.DrawText(pClosedTabData->strTitle, pClosedTabData->strTitle.GetLength(), rcText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
				rcText.MoveToY(rcText.top + kItemHeight);
				memDC.DrawText(item.name, item.name.GetLength(), rcText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
			} else {
				memDC.DrawText(item.name, item.name.GetLength(), rcText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
			}
		}
	}


	memDC.SelectFont(hFontPrev);
}

bool CRecentClosedTabPopupMenu::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_DELETE) {
		if (m_nHotIndex != -1) {
			/* Deleteキーでカーソル下のアイテムを削除する */
			s_pRecentClosedTabList->RemoveFromList(m_vecMenuItem[m_nHotIndex].nID);
			m_vecMenuItem.clear();
			InitMenuItem();

			CPoint pt;
			::GetCursorPos(&pt);
			ScreenToClient(&pt);
			int nIndex = _HitTest(pt);
			m_nHotIndex = -1;
			_HotItem(nIndex);

			Invalidate(FALSE);
			return true;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}

int CRecentClosedTabPopupMenu::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//SetMsgHandled(FALSE);

	_InitTooltip();

	//CMessageLoop *pLoop = _Module.GetMessageLoop();
	//pLoop->AddMessageFilter(this);

	__super::OnCreate(lpCreateStruct);

	m_nWheelLines = 1;
	SetScrollLine(1, kItemHeight * 3);

	return 0;
}

void CRecentClosedTabPopupMenu::OnDestroy()
{
	SetMsgHandled(FALSE);

	//CMessageLoop *pLoop = _Module.GetMessageLoop();
	//pLoop->RemoveMessageFilter(this);
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
	InitMenuItem();
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


void CRecentClosedTabPopupMenu::InitMenuItem()
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
	if (m_vecMenuItem.empty()) {
		CRect rc;
		rc.top	= nTop;
		rc.left	= kBoundMargin;
		rc.bottom= nTop + kItemHeight;
		m_vecMenuItem.emplace_back(_T("なし"), 0, rc);
		m_vecMenuItem.back().state = MPI_DISABLED;
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
			if (ChildFrameDataOnClose* pClosedTabData = static_cast<ChildFrameDataOnClose*>(it->pUserData)) {
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
	}
	return CalcWindowWidth(nMaxTextWidth);
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

	CRect	rcWindow = CalcPopupWindowPos(ptLeftBottom, CSize(ComputeWindowWidth(), ComputeWindowHeight()), hWndParent);
	Create(GetDesktopWindow(), rcWindow);
	ShowWindow(SW_SHOWNOACTIVATE);
}


////////////////////////////////////////////////////////////////////////
// CToolBarChevronPopupMenu

IBasePopupMenu* CToolBarChevronPopupMenu::CreateSubMenu(int nIndex)
{
	HMENU hMenu = m_vecMenuItem[nIndex].submenu.m_hMenu;
	for (auto& creator : m_vecCreateSubMenuFactory) {
		if (creator.first == hMenu) {
			return creator.second();
		}
	}
	return new CBasePopupMenu;
}

//////////////////////////////////////////////////////////////////////////////////
// CRootFavoriteGroupPopupMenu

std::vector<CString>	CRootFavoriteGroupPopupMenu::s_vecFavoriteGroupFilePath;
std::vector<std::pair<HWND, std::function<void ()>>>		CRootFavoriteGroupPopupMenu::s_vecfuncRefreshNotify;


void	CRootFavoriteGroupPopupMenu::LoadFavoriteGroup()
{
	s_vecFavoriteGroupFilePath.clear();

	CString strFavoriteGroupPath = Misc::GetExeDirectory() + _T("FavoriteGroup\\");
	::SHCreateDirectory(NULL, strFavoriteGroupPath);
	MtlForEachFileSort(strFavoriteGroupPath, [strFavoriteGroupPath](const CString& filePath) {
		CString fileExt = Misc::GetFileExt(filePath);
		if (fileExt.CompareNoCase(_T("donutTabList")) == 0) {
			s_vecFavoriteGroupFilePath.push_back(filePath);

		} else if (fileExt.CompareNoCase(_T("dfg")) == 0) {
			// 旧ファイルから読み込む
			using boost::wformat;
			CIniFileI	pr(filePath, _T("Header"));
			int	nCount = pr.GetValuei(_T("count"));
			int nActive = pr.GetValuei(_T("active"), -1);
			std::vector<std::unique_ptr<ChildFrameDataOnClose>>	vecpTabList;
			for (int i = 0; i < nCount; ++i) {
				pr.ChangeSectionName((wformat(L"Window%1%") % i).str().c_str());
				std::unique_ptr<ChildFrameDataOnClose>	pChildFrameData(new ChildFrameDataOnClose);
				ChildFrameDataOnClose& 	data = *pChildFrameData;
				data.strTitle	= pr.GetString(_T("Title"));
				data.strTitle.Replace(_T("\""), _T("_"));
				data.strURL		= pr.GetString(_T("Location_URL"));
				data.dwDLCtrl	= pr.GetValue(_T("DL_Control_Flags"));
				data.dwExStyle	= pr.GetValue(_T("Extended_Style"));
				data.dwAutoRefreshStyle	= pr.GetValue(_T("Auto_Refresh_Style"));
				auto funcAddTravelLog = [&pr](LPCTSTR prefix, std::vector<std::pair<CString, CString> >& TravelLog) {
					enum { kMaxTravelLog = 20 };
					for (int nTravelCount = 0; nTravelCount < kMaxTravelLog; ++nTravelCount) {
						CString title = pr.GetString((wformat(L"%1%_Title%2%") % prefix % nTravelCount).str().c_str());
						title.Replace(_T("\""), _T("_"));
						CString url = pr.GetString((wformat(L"%1%_URL%2%") % prefix % nTravelCount).str().c_str());
						if (title.IsEmpty() && url.IsEmpty())
							break;
						TravelLog.emplace_back(title, url);
					}
				};
				funcAddTravelLog(_T("Back"), data.TravelLogBack);
				funcAddTravelLog(_T("Fore"), data.TravelLogFore);
				vecpTabList.push_back(std::move(pChildFrameData));
			}
			pr.Close();

			if (vecpTabList.size() > 0) {
				/// 新形式に書き込み
				CDonutTabList tabList;
				tabList.Swap(vecpTabList);
				tabList.SetActiveIndex(nActive);

				CString fileName = Misc::GetFileBaseNoExt(filePath);
				CString strTabList = strFavoriteGroupPath + fileName + _T(".donutTabList");
				tabList.Save(strTabList);

				CString strOldFolderPath = strFavoriteGroupPath + _T("#old\\");
				::SHCreateDirectory(NULL, strOldFolderPath);
				::MoveFile(filePath, strOldFolderPath + Misc::GetFileBaseName(filePath));

				s_vecFavoriteGroupFilePath.push_back(strTabList);
			}
		}
	});

	/// 更新通知
	for (auto& pair : s_vecfuncRefreshNotify)
		pair.second();
}

void CRootFavoriteGroupPopupMenu::SetRefreshNotify(HWND hWnd, std::function<void ()> callback, bool bRegister)
{
	if (bRegister) {
		s_vecfuncRefreshNotify.push_back(std::make_pair(hWnd, callback));
	} else {
		for (auto it = s_vecfuncRefreshNotify.begin(); it != s_vecfuncRefreshNotify.end(); ++it) {
			if (it->first == hWnd) {
				s_vecfuncRefreshNotify.erase(it);
				break;
			}
		}
	}
}

void CRootFavoriteGroupPopupMenu::InitMenuItem()
{
	int nTop = kBoundMargin;
	struct FavoriteGroupMenuItem {
		LPCTSTR name;
		int		ID;
	};
	FavoriteGroupMenuItem	headItems[] = {
		{ _T("お気に入りグループに追加")	, ID_FAVORITE_GROUP_ADD	},
		{ _T("お気に入りグループを保存")	, ID_FAVORITE_GROUP_SAVE },
		{ _T("お気に入りグループを整理")	, ID_FAVORITE_GROUP_ORGANIZE },
		{ _T(""), -1 }
	};
	for (auto& item : headItems) {
		CRect rc;
		rc.top	= nTop;
		rc.left	= kBoundMargin;
		//rc.right= nWidht - kBoundMargin;
		rc.bottom= nTop +  (item.ID == -1 ? kSeparatorHeight : kItemHeight);
		nTop = rc.bottom;
		m_vecMenuItem.emplace_back(item.name, item.ID, rc, item.ID == -1);
	}

	int nCount = (int)s_vecFavoriteGroupFilePath.size();
	CClientDC dc(NULL);
	HFONT prevFont = dc.SelectFont(m_font);
	for (int i = 0; i < nCount; ++i) {
		CRect rc;
		rc.top	= nTop;
		rc.left	= kBoundMargin;
		rc.bottom= nTop + kItemHeight;

		nTop = rc.bottom;
#if _MSC_VER >= 1700
		m_vecMenuItem.emplace_back(Misc::GetFileBaseNoExt(s_vecFavoriteGroupFilePath[i]), kFavoriteGroupFirstID + i, rc);
#else
		m_vecMenuItem.push_back(MenuItem(name, kFavoriteGroupFirstID + i, rc));
#endif
		m_vecMenuItem.back().icon.LoadIcon(IDI_FAVORITEGROUP);
	}
	dc.SelectFont(prevFont);
}


void CRootFavoriteGroupPopupMenu::OnClick(const CPoint& pt, bool bLButtonUp /*= false*/)
{
	int nIndex = _HitTest(pt);
	if (nIndex == -1)
		return ;

	MenuItem& item = m_vecMenuItem[nIndex];
	if (item.bSeparator || item.state == MPI_DISABLEDHOT)
		return ;

	if (bLButtonUp) {
		// メニューを閉じる
		_CloseBaseSubMenu();

		if (0 <= nIndex && nIndex <= 2) {
			// アイテムを実行...
			CWindow(s_hWndCommandBar).GetTopLevelWindow().PostMessage(WM_COMMAND, item.nID);
		} else {
			g_pMainWnd->RestoreAllTab(s_vecFavoriteGroupFilePath[item.nID - kFavoriteGroupFirstID], (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOCLOSEDFG) == 0);
		}
	}
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

std::vector<std::pair<HWND, std::function<void ()>>>		CRootFavoritePopupMenu::s_vecfuncRefreshNotify;


CRootFavoritePopupMenu::CRootFavoritePopupMenu() : m_pLinkSubMenu(nullptr), m_hWndLinkBar(NULL)
{
}

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

void CRootFavoritePopupMenu::SetRefreshNotify(HWND hWnd, std::function<void ()> callback, bool bRegister)
{
	if (bRegister) {
		s_vecfuncRefreshNotify.push_back(std::make_pair(hWnd, callback));
	} else {
		for (auto it = s_vecfuncRefreshNotify.begin(); it != s_vecfuncRefreshNotify.end(); ++it) {
			if (it->first == hWnd) {
				s_vecfuncRefreshNotify.erase(it);
				break;
			}
		}
	}
}

/// お気に入りリストの変化を通知する
void CRootFavoritePopupMenu::NotifyRefresh()
{
	for (auto& callback : s_vecfuncRefreshNotify)
		callback.second();
}

// Overrides
IBasePopupMenu* CRootFavoritePopupMenu::CreateSubMenu(int nIndex)
{
	/// お気に入りグループ
	return new CRootFavoriteGroupPopupMenu;
}

void	CRootFavoritePopupMenu::DoTrackPopupMenu(CMenuHandle menu, CPoint ptLeftBottom, HWND hWndParent)
{
	m_menu = menu;
	if (m_menu.IsNull())
		m_menu.LoadMenu(IDR_DROPDOWN_FAV);	// ツールバーから
	s_hWndCommandBar = hWndParent;
	InitMenuItem();

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

		Create(GetDesktopWindow(), rcWindow);
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

		Create(GetDesktopWindow(), rcWindow);
		ShowWindow(SW_SHOWNOACTIVATE);
	}
}

void	CRootFavoritePopupMenu::DoTrackSubPopupMenu(CMenuHandle menu, CRect rcClientItem, HWND hWndParent, int nInheritIndex)
{
	m_menu = menu;
	if (m_menu.IsNull())
		m_menu.LoadMenu(IDR_DROPDOWN_FAV);	// ツールバーから
	m_nInheritMenuIndex = nInheritIndex;
	InitMenuItem();

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

		Create(hWndParent, rcWindow);
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

		Create(GetDesktopWindow(), rcWindow);
		ShowWindow(SW_SHOWNOACTIVATE);
	}
}

bool	CRootFavoritePopupMenu::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (m_pLinkSubMenu == nullptr) {
			return __super::PreTranslateMessage(pMsg);
		}
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
		if (nChar == VK_DOWN) {
			// どちらもHot状態のアイテムがないので一番上のアイテムをHot状態にする
			if (m_nHotIndex == -1 && m_pLinkSubMenu->m_nHotIndex == -1) {
				_HotItem(0);
				return true;
			} else if (m_nHotIndex == -1) {		// リンクアイテムの選択状態を変える				
				if (m_pLinkSubMenu->m_nHotIndex == (m_pLinkSubMenu->m_pFolder->size() - 1) && m_pLinkSubMenu->m_pSubMenu == nullptr) {
					m_pLinkSubMenu->_HotItem(-1);
					_HotItem(0);
				} else {
					m_pLinkSubMenu->PreTranslateMessage(pMsg);
				}
				return true;
			} else {
				int nNewHotIndex = m_nHotIndex + 1;
				if ((int)m_vecMenuItem.size() - 1 <= nNewHotIndex) {
					m_pLinkSubMenu->_CloseSubMenu();
					_HotItem(-1);
					m_pLinkSubMenu->PreTranslateMessage(pMsg);
				} else {
					_HotItem(nNewHotIndex);
				}
				return true;
			}
		} else if (nChar == VK_UP) {
			// どちらもHot状態のアイテムがないので一番下のアイテムをHot状態にする
			if (m_nHotIndex == -1 && m_pLinkSubMenu->m_nHotIndex == -1) {
				m_pLinkSubMenu->PreTranslateMessage(pMsg);
			} else if (m_nHotIndex == -1) {		// リンクアイテムの選択状態を変える
				if (m_pLinkSubMenu->m_nHotIndex == 0 && m_pLinkSubMenu->m_pSubMenu == nullptr) {
					m_pLinkSubMenu->_HotItem(-1);
					_HotItem(m_vecMenuItem.size() - 2);
				} else {
					m_pLinkSubMenu->PreTranslateMessage(pMsg);
				}
				return true;
			} else {
				int nNewHotIndex = m_nHotIndex - 1;
				if (nNewHotIndex == -1) {
					_HotItem(-1);
					m_pLinkSubMenu->PreTranslateMessage(pMsg);
				} else {
					_HotItem(nNewHotIndex);
				}
				return true;
			}
		} else if (nChar == VK_RIGHT || nChar == VK_RETURN) {			// 選択アイテムのサブメニューを表示する
			if (_DoTrackSubPopupMenu(m_nHotIndex)) {
				if (m_pSubMenu) {
					// 一番上のアイテムをHot状態にする
					MSG msg = *pMsg;
					msg.wParam	= VK_DOWN;
					m_pSubMenu->PreTranslateMessage(&msg);
					return true;
				} else {
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
			} else if (m_nHotIndex == -1) {				
				return m_pLinkSubMenu->PreTranslateMessage(pMsg) != 0;

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
		} else if ((nChar == VK_LEFT || nChar == VK_ESCAPE) && (m_pSubMenu == nullptr && m_pLinkSubMenu->m_pSubMenu == nullptr)) {		// 自分が最上階のポップアップメニューなら自分自身を親に閉じてもらう
			return true;

		} else {	// プレフィックス処理
			if (m_pLinkSubMenu->m_pSubMenu) {
				m_pLinkSubMenu->PreTranslateMessage(pMsg);
			} else {
				int nCount = (int)m_vecMenuItem.size();
				std::vector<int>	vecSamePrefixIndex;
				for (int i = 0; i < nCount; ++i) {
					if (m_vecMenuItem[i].prefix == (TCHAR)nChar) 
						vecSamePrefixIndex.push_back(i);
				}
				enum { kLinkItemBound = 100 };
				int nLinkCount = (int)m_pLinkSubMenu->m_pFolder->size();
				for (int i = 0; i < nLinkCount; ++i) {
					if (m_pLinkSubMenu->m_pFolder->at(i)->strName.Left(1) == (TCHAR)nChar)
						vecSamePrefixIndex.push_back(kLinkItemBound + i);
				}
				if (vecSamePrefixIndex.size() > 0) {
					if (vecSamePrefixIndex.size() == 1) {
						int nFirstIndex = vecSamePrefixIndex.front();
						if (nFirstIndex < kLinkItemBound) {
							if (_DoTrackSubPopupMenu(nFirstIndex)) {
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
							m_pLinkSubMenu->_DoExec(m_pLinkSubMenu->m_pFolder->at(nFirstIndex - kLinkItemBound)->rcItem.TopLeft(), true);
						}
					} else {
						int nHotIndex = m_nHotIndex;
						if (nHotIndex == -1 && m_pLinkSubMenu->m_nHotIndex != -1)
							nHotIndex = m_pLinkSubMenu->m_nHotIndex + kLinkItemBound;
						for (int nIndex : vecSamePrefixIndex) {
							if (nHotIndex < nIndex) {
								if (nIndex < kLinkItemBound) {
									_HotItem(nIndex);
								} else {
									m_pLinkSubMenu->_HotItem(nIndex - kLinkItemBound);
								}
								return true;
							}
						}
						if (vecSamePrefixIndex.front() < kLinkItemBound) {
							_HotItem(vecSamePrefixIndex.front());
						} else {
							m_pLinkSubMenu->_HotItem(vecSamePrefixIndex.front() - kLinkItemBound);
						}
					}
					return true;
				}
			}
		}
	}
	return false;
}


void CRootFavoritePopupMenu::OnTrackMouseLeave()
{
	KillTimer(kSubMenuPopupTimerID);
	if (m_pSubMenu) {	// カーソルが出て行ったときにサブメニューを閉じる
		CPoint	pt;
		GetCursorPos(&pt);
		HWND hWnd = WindowFromPoint(pt);
		if (m_pLinkSubMenu && hWnd == m_pLinkSubMenu->m_hWnd) {
			// お気に入り方向にカーソルが移動したら直ちに閉じる
			m_pSubMenu->DestroyWindow();
			m_pSubMenu = nullptr;
		} else if (hWnd != m_pSubMenu->GetHWND()) {
			SetTimer(kSubMenuPopupCloseTimerID, kSubMenuPopupCloseTime);
		} else {
			_HotItem(m_pSubMenu->GetInheritMenuIndex());
			return ;
		}
	}
	_HotItem(-1);
}


void CRootFavoritePopupMenu::OnDestroy()
{
	SetMsgHandled(FALSE);

	if (m_pLinkSubMenu)
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
		if (m_pLinkSubMenu && m_pLinkSubMenu->m_hWnd == hWnd) {
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

	NotifyRefresh();
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

