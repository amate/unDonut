/**
*	@file	DonutFavoriteGroupTreeView.cpp
*	@brief	お気に入りグループ
*/
#include "stdafx.h"
#include "DonutFavoriteGroupTreeView.h"
#include <boost\format.hpp>
#include "PopupMenu.h"
#include "Misc.h"
#include "MainFrame.h"
#include "DonutTabList.h"
#include "LinkPopupMenu.h"

using boost::wformat;

/////////////////////////////////////////////////////////////////////
// CDonutFavoriteGroupTreeView

std::function<CDonutTabList ()>	CDonutFavoriteGroupTreeView::s_funcGetAllDonutTabList;
std::function<std::unique_ptr<ChildFrameDataOnClose> ()>	CDonutFavoriteGroupTreeView::s_funcGetActiveDonutTabData;


CDonutFavoriteGroupTreeView::CDonutFavoriteGroupTreeView(void)
{
}


CDonutFavoriteGroupTreeView::~CDonutFavoriteGroupTreeView(void)
{
}


HWND	CDonutFavoriteGroupTreeView::Create(HWND hWndParent)
{
	DWORD dwStyle =	 WS_CHILD  | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER
					| TVS_INFOTIP | TVS_SHOWSELALWAYS  | TVS_FULLROWSELECT | TVS_TRACKSELECT | TVS_SINGLEEXPAND | TVS_EDITLABELS;

	HWND hWnd = __super::Create(hWndParent, 0, nullptr, dwStyle);  
	ATLASSERT( ::IsWindow(hWnd) );
	return hWnd;
}



int CDonutFavoriteGroupTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	DefWindowProc();

	SetWindowText(_T("お気に入りグループ"));

#ifndef TVS_EX_RICHTOOLTIP
#define TVS_EX_RICHTOOLTIP          0x0010
#endif
#ifndef TVS_EX_DOUBLEBUFFER
#define TVS_EX_DOUBLEBUFFER         0x0004
#endif

	SetExtendedStyle(TVS_EX_RICHTOOLTIP | TVS_EX_DOUBLEBUFFER, TVS_EX_RICHTOOLTIP | TVS_EX_DOUBLEBUFFER);

	m_imageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 1);
	CIcon	icoFavoriteGroup;
	icoFavoriteGroup.LoadIcon(IDI_FAVORITEGROUP);
	m_imageList.AddIcon(icoFavoriteGroup);
	m_imageList.AddIcon(CLinkPopupMenu::s_iconLink);
	SetImageList(m_imageList);

	CRootFavoriteGroupPopupMenu::SetRefreshNotify(m_hWnd, [this]() {
		_RefreshTree();
	}, true);

	_RefreshTree();

	return 0;
}

void CDonutFavoriteGroupTreeView::OnDestroy()
{
	DefWindowProc();

	m_imageList.Destroy();

	CRootFavoriteGroupPopupMenu::SetRefreshNotify(m_hWnd, [](){}, false);
}

/// Root直下のアイテム以外は編集禁止にする
LRESULT CDonutFavoriteGroupTreeView::OnTreeBeginLabelEdit(LPNMHDR pnmh)
{
	auto ptvdi = (LPNMTVDISPINFO)pnmh;
	if (IsRootChildren(ptvdi->item.hItem))
		return 0;

	return 1;	// Cancel
}

LRESULT CDonutFavoriteGroupTreeView::OnTreeEndLabelEdit(LPNMHDR pnmh)
{
	auto ptvdi = (LPNMTVDISPINFO)pnmh;
	SetItem(&ptvdi->item);
	if (ptvdi->item.pszText == nullptr)
		return 0;

	std::vector<HTREEITEM>	vechtRootItem;
	HTREEITEM htItem = GetChildItem(TVI_ROOT);
	if (htItem) {
		do {
			vechtRootItem.push_back(htItem);
		} while (htItem = GetNextSiblingItem(htItem));
	}
	int nCount = (int)vechtRootItem.size();
	for (int i = 0; i < nCount; ++i) {
		if (vechtRootItem[i] == ptvdi->item.hItem) {
			auto pvecFileList = CRootFavoriteGroupPopupMenu::GetFavoriteGroupFilePathList();
			CString strPrevFilePath = pvecFileList->at(i);
			pvecFileList->at(i) = Misc::GetExeDirectory() + _T("FavoriteGroup\\") + ptvdi->item.pszText + _T(".donutTabList");

			if (::MoveFile(strPrevFilePath, pvecFileList->at(i)) == 0) {
				MessageBox(_T("名前の変更に失敗しました"), _T("エラー"), MB_ICONWARNING);
				pvecFileList->at(i) = strPrevFilePath;
				SetItemText(ptvdi->item.hItem, Misc::GetFileBaseNoExt(strPrevFilePath));
			}
			SetItemData(ptvdi->item.hItem, (DWORD_PTR)pvecFileList->at(i).GetBuffer(0));
			break;
		}
	}

	return 0;
}

void	CDonutFavoriteGroupTreeView::_AddTreeItem(HTREEITEM htParent, const CString& title, const CString& url)
{
	HTREEITEM htInsertItem = InsertItem(title, htParent, TVI_LAST);
	m_vecURLs.push_back(url);
	SetItemImage(htInsertItem, kIconIndexLink, kIconIndexLink);
	SetItemData(htInsertItem, (DWORD_PTR)m_vecURLs.size() - 1);
}

LRESULT CDonutFavoriteGroupTreeView::OnTreeItemExpanding(LPNMHDR pnmh)
{
	LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)pnmh;
	if (GetChildItem(pnmtv->itemNew.hItem))
		return 0;

	LPCTSTR filePath = (LPCTSTR)pnmtv->itemNew.lParam;
	CDonutTabList	tabList;
	if (tabList.Load(filePath) == false) {
		MessageBox((wformat(L"\"%1%\"の読み込みに失敗") % filePath).str().c_str());
		return 0;
	}
	int nCount = tabList.GetCount();
	for (int i = 0; i < nCount; ++i) {
		_AddTreeItem(pnmtv->itemNew.hItem, tabList.At(i)->strTitle, tabList.At(i)->strURL);
	}
	return 0;
}

bool	CDonutFavoriteGroupTreeView::IsRootChildren(HTREEITEM ht)
{
	std::vector<HTREEITEM>	vechtRootItem;
	HTREEITEM htItem = GetChildItem(TVI_ROOT);
	if (htItem) {
		do {
			vechtRootItem.push_back(htItem);
		} while (htItem = GetNextSiblingItem(htItem));
	}
	return std::find_if(vechtRootItem.begin(), vechtRootItem.end(), [ht](HTREEITEM htItem) { return ht == htItem; }) != vechtRootItem.end();
}

void	CDonutFavoriteGroupTreeView::AddActiveTab(HTREEITEM htFavoriteGroup)
{
	LPCTSTR filePath = (LPCTSTR)GetItemData(htFavoriteGroup);
	if (auto pActiveTabData = s_funcGetActiveDonutTabData()) {
		CDonutTabList	tabList;
		if (tabList.Load(filePath)) {
			if (GetItemState(htFavoriteGroup, TVIS_EXPANDED) & TVIS_EXPANDED)
				_AddTreeItem(htFavoriteGroup, pActiveTabData->strTitle, pActiveTabData->strURL);

			tabList.PushBack(std::move(pActiveTabData));
			tabList.Save(filePath, false);
		}
	}
}


LRESULT CDonutFavoriteGroupTreeView::OnTreeItemRClick(LPNMHDR pnmh)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	UINT flags = 0;
	HTREEITEM htHit = HitTest(pt, &flags);
	if (htHit == NULL)
		return 0;

	CMenu menu;
	menu.CreatePopupMenu();
	enum { 
		kRenameFavoriteGroup = 1,
		kFavoriteGroupOpen,
		kAddActiveTab,
		kAddAllTab,
		kFavoriteGroupDelete,
		kItemDelete,
	};

	LPCTSTR filePath = nullptr;
	if (IsRootChildren(htHit)) {
		menu.AppendMenu(0, kFavoriteGroupOpen, _T("グループを開く"));
		menu.AppendMenu(MF_SEPARATOR, 0U, _T(""));
		menu.AppendMenu(0, kAddActiveTab, _T("アクティブなタブをグループに追加"));
		menu.AppendMenu(0, kAddAllTab, _T("全タブをグループに追加"));
		menu.AppendMenu(MF_SEPARATOR, 0U, _T(""));
		menu.AppendMenu(0, kRenameFavoriteGroup, _T("名前の変更"));
		menu.AppendMenu(MF_SEPARATOR, 0U, _T(""));
		menu.AppendMenu(0, kFavoriteGroupDelete, _T("削除"));

		filePath = (LPCTSTR)GetItemData(htHit);
	} else {
		menu.AppendMenu(0, kItemDelete, _T("削除"));
	}

	ClientToScreen(&pt);
	int nCmd = menu.TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, m_hWnd, nullptr);
	switch (nCmd) {
	case kFavoriteGroupOpen:
		{
			CString favoriteGroupPath = (LPCTSTR)GetItemData(htHit);
			g_pMainWnd->RestoreAllTab(favoriteGroupPath, (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOCLOSEDFG) == 0);
		}
		break;

	case kRenameFavoriteGroup:
		EditLabel(htHit);
		break;

	case kAddActiveTab:
		{
			if (auto pActiveTabData = s_funcGetActiveDonutTabData()) {
				CDonutTabList	tabList;
				if (tabList.Load(filePath)) {
					if (GetItemState(htHit, TVIS_EXPANDED) & TVIS_EXPANDED)
						_AddTreeItem(htHit, pActiveTabData->strTitle, pActiveTabData->strURL);

					tabList.PushBack(std::move(pActiveTabData));
					tabList.Save(filePath, false);
				}
			}
		}
		break;

	case kAddAllTab:
		{
			auto allTabList = s_funcGetAllDonutTabList();
			CDonutTabList	tabList;
			if (tabList.Load(filePath)) {
				int nCount = allTabList.GetCount();
				for (int i = 0; i < nCount; ++i) {
					if (GetItemState(htHit, TVIS_EXPANDED) & TVIS_EXPANDED)
						_AddTreeItem(htHit, allTabList.At(i)->strTitle, allTabList.At(i)->strURL);
					tabList.PushBack(unique_ptr<ChildFrameDataOnClose>(std::move(allTabList.At(i))));
				}
				tabList.Save(filePath, false);
			}
		}
		break;

	case kFavoriteGroupDelete:
		{
			auto pvecFileList = CRootFavoriteGroupPopupMenu::GetFavoriteGroupFilePathList();
			for (auto it = pvecFileList->begin(); it != pvecFileList->end(); ++it) {
				if ((*it) == filePath) {
					::DeleteFile(filePath);
					pvecFileList->erase(it);

					_RefreshTree();
					break;
				}
			}

		}
		break;

	case kItemDelete:
		{
			HTREEITEM htParent = GetParentItem(htHit);
			ATLASSERT( htParent );
			filePath = (LPCTSTR)GetItemData(htParent);
			int nIndex = GetIndexFromHTREEITEM(htParent, htHit);
			ATLASSERT(nIndex != -1);
			CDonutTabList tabList;
			if (tabList.Load(filePath)) {
				tabList.Delete(nIndex);
				DeleteItem(htHit);
				tabList.Save(filePath, false);
			}
		}
		break;
	}

	return 0;
}

int		CDonutFavoriteGroupTreeView::GetIndexFromHTREEITEM(HTREEITEM htParent, HTREEITEM htItem)
{
	if (HTREEITEM htNow = GetChildItem(htParent)) {
		int nIndex = 0;
		do {
			if (htNow == htItem)
				return nIndex;
			++nIndex;
		} while (htNow = GetNextSiblingItem(htNow));
	}
	return -1;
}


/// URLを開く
LRESULT CDonutFavoriteGroupTreeView::OnTreeSelChanged(LPNMHDR pnmh)
{
	auto pnmtv = (LPNMTREEVIEW)pnmh;
	if (IsRootChildren(pnmtv->itemNew.hItem))
		return 0;

	CString url = m_vecURLs[pnmtv->itemNew.lParam];
	DonutOpenFile(url);
	return 0;
}

/// お気に入りグループを開く
LRESULT CDonutFavoriteGroupTreeView::OnTreeItemDoubleClick(LPNMHDR pnmh)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	UINT flags = 0;
	HTREEITEM htHit = HitTest(pt, &flags);
	if (htHit == NULL || IsRootChildren(htHit) == false)
		return 0;

	CString favoriteGroupPath = (LPCTSTR)GetItemData(htHit);
	g_pMainWnd->RestoreAllTab(favoriteGroupPath, (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOCLOSEDFG) == 0);
	return 0;
}


void CDonutFavoriteGroupTreeView::_RefreshTree()
{
	DeleteAllItems();

	auto pvecFileList = CRootFavoriteGroupPopupMenu::GetFavoriteGroupFilePathList();
	for (auto& filePath : *pvecFileList) {
		HTREEITEM htItem = InsertItem(Misc::GetFileBaseNoExt(filePath), TVI_ROOT, TVI_LAST);
		TVITEM tvi = {};
		tvi.hItem	= htItem;
		tvi.mask	= TVIF_CHILDREN;
		tvi.cChildren = TRUE;
		SetItem(&tvi);
		SetItemImage(htItem, kIconIndexFavoriteGroup, kIconIndexFavoriteGroup);
		SetItemData(htItem, (DWORD_PTR)filePath.GetBuffer(0));
	}
}






