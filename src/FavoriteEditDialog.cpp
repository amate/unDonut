/**
*	@file	FavoriteEditDialog.h
*	@brief	お気に入り編集ダイアログ
*/

#include "stdafx.h"
#include "FavoriteEditDialog.h"
#include "PopupMenu.h"

/////////////////////////////////////////////////////////
// CFavoriteEditDialog


CFavoriteEditDialog::CFavoriteEditDialog(bool bFavoriteAdd) : m_bFavoriteAdd(bFavoriteAdd), m_bSave(false)
{	}


BOOL CFavoriteEditDialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	if (m_bFavoriteAdd) {
		SetWindowText(_T("お気に入りに追加"));
		GetDlgItem(ID_APPLY).SetWindowText(_T("追加"));
	}

	DoDataExchange(DDX_LOAD);

	DlgResize_Init(true, true, WS_THICKFRAME | WS_CLIPCHILDREN);

	m_pBookmarkList = CRootFavoritePopupMenu::GetBookmarkListPtr();
	
	m_image_list.Create(16, 16, ILC_COLOR32 | ILC_MASK, 100, 100);
	
	m_image_list.AddIcon(CLinkPopupMenu::s_iconFolder);
	m_image_list.AddIcon(CLinkPopupMenu::s_iconLink);
	enum { kFolderIcon = 0, kLinkIcon = 1 };
	
	m_tree.SetImageList(m_image_list, TVSIL_NORMAL);
	
	m_tree.SetFocus();

	function<void (HTREEITEM, LinkFolderPtr)> funcInsertItemFromFolder;
	CTreeViewCtrl	tree = m_tree;
	bool			bFavoriteAdd = m_bFavoriteAdd;
	funcInsertItemFromFolder = [&](HTREEITEM hParent, LinkFolderPtr pFolder) {
		enum { kFolderIcon = 0, kLinkIcon = 1 };
		for (auto it = pFolder->cbegin(); it != pFolder->cend(); ++it) {
			LinkItem& item = *it->get();
			int nImageIndex;
			if (item.icon.m_hIcon) {
				if (bFavoriteAdd)
					continue;
				nImageIndex = m_image_list.AddIcon(item.icon);
			} else if (item.pFolder) {
				nImageIndex = kFolderIcon;
			} else {
				if (bFavoriteAdd)
					continue;
				nImageIndex = kLinkIcon;
			}
			HTREEITEM hItem = tree.InsertItem(item.strName, nImageIndex, nImageIndex, hParent, TVI_LAST);
			tree.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(it->get()));
			if (item.pFolder)
				funcInsertItemFromFolder(hItem, item.pFolder);
		}
	};
	HTREEITEM hRoot = TVI_ROOT;
	if (m_bFavoriteAdd)
		hRoot = m_tree.InsertItem(_T("お気に入り"), kFolderIcon, kFolderIcon, TVI_ROOT, TVI_LAST);
	funcInsertItemFromFolder(hRoot, static_cast<LinkFolderPtr>(m_pBookmarkList));	
	if (m_bFavoriteAdd)
		m_tree.Expand(hRoot);

	CIniFileI pr(g_szIniFileName, _T("FavoriteEditDialog"));
	CRect rc;
	rc.right	= pr.GetValuei(_T("Width"));
	rc.bottom	= pr.GetValuei(_T("Height"));
	if (rc != CRect()) {
		SetWindowPos(NULL, 0, 0, rc.right, rc.bottom, SWP_NOMOVE | SWP_NOZORDER);
	}
	CenterWindow(GetParent());

	return FALSE;
}

/// 適用/追加
void CFavoriteEditDialog::OnApply(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	HTREEITEM htItem = m_tree.GetSelectedItem();
	if (m_bFavoriteAdd) {
		DoDataExchange(DDX_SAVE);
		if (m_strName.IsEmpty() || m_strURL.IsEmpty()) {
			MessageBox(_T("名前かURLが空です"));
			return ;
		}
		auto funcAddLink = [this](LinkFolderPtr pFolder) {
			unique_ptr<LinkItem> pItem(new LinkItem);
			pItem->strName	= m_strName;
			pItem->strUrl	= m_strURL;
			if (m_favicon.m_hIcon)
				pItem->icon		= m_favicon.DuplicateIcon();
			pFolder->push_back(std::move(pItem));
		};
		LinkItem* pItem = reinterpret_cast<LinkItem*>(m_tree.GetItemData(htItem));
		if (htItem == NULL || pItem == nullptr) {
			funcAddLink(m_pBookmarkList);
		} else {			
			funcAddLink(pItem->pFolder);
		}
		m_bSave = true;
		OnCancel(0, IDCANCEL, NULL);

	} else {
		if (htItem == NULL)
			return ;
		DoDataExchange(DDX_SAVE);
		LinkItem* pItem = reinterpret_cast<LinkItem*>(m_tree.GetItemData(htItem));
		if (m_strName.IsEmpty()) {
			MessageBox(_T("名前が空です"));
			return ;
		}
		if (m_strURL.IsEmpty() && pItem->pFolder == nullptr) {
			MessageBox(_T("URLが空です"));
			return ;
		}
	
		if (pItem) {
			pItem->strName	= m_strName;
			if (pItem->pFolder == nullptr)
				pItem->strUrl	= m_strURL;
			m_tree.SetItemText(htItem, m_strName);
			m_bSave = true;
		}
	}
}

void CFavoriteEditDialog::OnCreateFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	auto funcAddFolder = [this](HTREEITEM hFolder, LinkFolderPtr pFolder) {
		unique_ptr<LinkItem> pItem(new LinkItem);
		pItem->pFolder	= new LinkFolder;
		pItem->strName	= _T("新しいフォルダ");
		HTREEITEM hNewItem = m_tree.InsertItem(pItem->strName, 0, 0, hFolder, TVI_LAST);
		m_tree.SetItemData(hNewItem, reinterpret_cast<DWORD_PTR>(pItem.get()));
		m_tree.SelectItem(hNewItem);
		pFolder->push_back(std::move(pItem));
	};
	HTREEITEM htItem = m_tree.GetSelectedItem();
	if (htItem == NULL) {
		funcAddFolder(TVI_ROOT, m_pBookmarkList);
	} else {
		LinkItem* pItem = reinterpret_cast<LinkItem*>(m_tree.GetItemData(htItem));
		if (pItem->pFolder) {
			funcAddFolder(htItem, pItem->pFolder);
		} else {
			function<void (LinkFolderPtr)> funcFindInsertFolder;
			funcFindInsertFolder = [&, this](LinkFolderPtr pFolder) {
				for (auto it = pFolder->cbegin(); it != pFolder->cend(); ++it) {
					if (it->get() == pItem) {
						funcAddFolder(m_tree.GetParentItem(htItem) ,pFolder);
						return ;
					} else if (it->get()->pFolder) {
						funcFindInsertFolder(it->get()->pFolder);
					}
				}
			};
			funcFindInsertFolder(m_pBookmarkList);
		}
	}
	m_bSave = true;
}

void CFavoriteEditDialog::OnDelete(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	HTREEITEM htItem = m_tree.GetSelectedItem();
	if (htItem == NULL) 
		return ;

	LinkItem* pItem = reinterpret_cast<LinkItem*>(m_tree.GetItemData(htItem));
	function<void (LinkFolderPtr)> funcDeleteItem;
	funcDeleteItem = [&, this](LinkFolderPtr pFolder) {
		for (auto it = pFolder->cbegin(); it != pFolder->cend(); ++it) {
			if (it->get() == pItem) {
				pFolder->erase(it);
				m_tree.DeleteItem(htItem);
				return ;
			} else if (it->get()->pFolder) {
				funcDeleteItem(it->get()->pFolder);
			}
		}
	};
	funcDeleteItem(m_pBookmarkList);
	m_bSave = true;
}

void CFavoriteEditDialog::OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl) 
{
	CIniFileIO pr(g_szIniFileName, _T("FavoriteEditDialog"));
	CRect rc;
	GetWindowRect(&rc);
	pr.SetValue(rc.Width()	, _T("Width"));
	pr.SetValue(rc.Height()	, _T("Height"));

	m_image_list.Destroy();
	if (m_bSave)
		CRootFavoritePopupMenu::SaveFavoriteBookmark();
	EndDialog(nID);
}


LRESULT CFavoriteEditDialog::OnTreeSelChanged(LPNMHDR pnmh)
{
	LPNMTREEVIEW pnmtv = reinterpret_cast<LPNMTREEVIEW>(pnmh);
	LinkItem* pItem = reinterpret_cast<LinkItem*>(pnmtv->itemNew.lParam);
	if (pItem) {
		m_strName	= pItem->strName;
		m_strURL	= pItem->strUrl;
		if (pItem->pFolder)
			m_tree.Expand(pnmtv->itemNew.hItem);
	} else {
		m_strName	= _T("");
		m_strURL	= _T("");
	}
	if (m_bFavoriteAdd)
		return 0;
	DoDataExchange(DDX_LOAD);
	return 0;
}

#if 0
LRESULT CFavoriteEditDialog::OnTreeBeginDrag(LPNMHDR pnmh)
{
	LPNMTREEVIEW pnmtv = reinterpret_cast<LPNMTREEVIEW>(pnmh);
	m_dragimage_list = m_tree.CreateDragImage(pnmtv->itemNew.hItem);
	m_dragimage_list.BeginDrag(0, 0, 0);
	ShowCursor(FALSE);
	SetCapture();
	m_tree.ClientToScreen(&pnmtv->ptDrag);
	m_dragimage_list.DragEnter(m_hWnd, pnmtv->ptDrag);
	return 0;
}

void CFavoriteEditDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetCapture() == m_hWnd) {
		//m_dragimage_list.DragLeave(NULL);
		UINT flags = TVHT_ONITEM;
		HTREEITEM htItem = m_tree.HitTest(point, &flags);
		if (htItem)
			m_tree.SelectDropTarget(htItem);
		m_dragimage_list.DragMove(point);
		//m_dragimage_list.DragEnter(NULL, point);
	}
}

void CFavoriteEditDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture() == m_hWnd) {
		m_dragimage_list.DragLeave(m_hWnd);
		m_dragimage_list.EndDrag();
		ReleaseCapture();
		ShowCursor(TRUE);
		UINT flags = TVHT_ONITEM;
		HTREEITEM htItem = m_tree.HitTest(point, &flags);

	}
}

#endif

