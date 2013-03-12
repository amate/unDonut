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

	CRect rcTree;
	GetDlgItem(IDC_TREE).GetClientRect(&rcTree);
	GetDlgItem(IDC_TREE).MapWindowPoints(m_hWnd, &rcTree);
	GetDlgItem(IDC_TREE).ShowWindow(FALSE);
	GetDlgItem(IDC_TREE).SetDlgCtrlID(0);

	m_tree.Create(m_hWnd);
	m_tree.MoveWindow(&rcTree);
	m_tree.SetDlgCtrlID(IDC_TREE);
	m_tree.SetItemClickFunc([this](LinkItem* pLinkItem) {
		m_strName	= pLinkItem->strName;
		m_strURL	= pLinkItem->strUrl;
		if (m_bFavoriteAdd)
			return ;
		DoDataExchange(DDX_LOAD);
	});

	DlgResize_Init(true, true, WS_THICKFRAME | WS_CLIPCHILDREN);

	m_tree.SetFocus();

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
			funcAddLink(CRootFavoritePopupMenu::GetBookmarkListPtr());
		} else {
			if (pItem->pFolder) {
				funcAddLink(pItem->pFolder);
			} else {
				funcAddLink(m_tree.GetParentLinkFolderPtr(htItem));
			}
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
		funcAddFolder(TVI_ROOT, CRootFavoritePopupMenu::GetBookmarkListPtr());
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
			funcFindInsertFolder(CRootFavoritePopupMenu::GetBookmarkListPtr());
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
	funcDeleteItem(CRootFavoritePopupMenu::GetBookmarkListPtr());
	m_bSave = true;
}

void CFavoriteEditDialog::OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl) 
{
	CIniFileIO pr(g_szIniFileName, _T("FavoriteEditDialog"));
	CRect rc;
	GetWindowRect(&rc);
	pr.SetValue(rc.Width()	, _T("Width"));
	pr.SetValue(rc.Height()	, _T("Height"));

	if (m_bSave)
		CRootFavoritePopupMenu::SaveFavoriteBookmark();

	m_tree.DestroyWindow();
	CRootFavoritePopupMenu::NotifyRefresh();

	EndDialog(nID);
}
