/**
*	@file	FavoriteGroupEditDialog.cpp
*	@brief	お気に入りグループ編集ダイアログ
*/

#include "stdafx.h"
#include "FavoriteGroupEditDialog.h"
#include "IniFile.h"
#include "PopupMenu.h"

///////////////////////////////////////////////////////////////////////
// CFavoriteGroupEditDialog

BOOL CFavoriteGroupEditDialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CRect rcTree;
	GetDlgItem(IDC_TREE).GetClientRect(&rcTree);
	GetDlgItem(IDC_TREE).MapWindowPoints(m_hWnd, &rcTree);
	GetDlgItem(IDC_TREE).ShowWindow(FALSE);
	GetDlgItem(IDC_TREE).SetDlgCtrlID(0);

	m_tree.Create(m_hWnd);
	m_tree.MoveWindow(&rcTree);
	m_tree.SetDlgCtrlID(IDC_TREE);

	DlgResize_Init(true, true, WS_THICKFRAME | WS_CLIPCHILDREN);

	switch (m_favoriteGroupMode) {
	case kFavoriteGroupAdd:
		SetWindowText(_T("お気に入りグループに追加"));
		GetDlgItem(IDOK).SetWindowText(_T("追加"));
		break;

	case kFavoriteGroupOrganize:
		SetWindowText(_T("お気に入りグループの整理"));
		GetDlgItem(IDOK).ShowWindow(FALSE);
		break;

	default:
		ATLASSERT( FALSE );
	}

	m_tree.SetFocus();

	CIniFileI pr(g_szIniFileName, _T("FavoriteGroupEditDialog"));
	CRect rc;
	rc.right	= pr.GetValuei(_T("Width"));
	rc.bottom	= pr.GetValuei(_T("Height"));
	if (rc != CRect()) {
		SetWindowPos(NULL, 0, 0, rc.right, rc.bottom, SWP_NOMOVE | SWP_NOZORDER);
	}
	CenterWindow(GetParent());

	return FALSE;
}

void CFavoriteGroupEditDialog::OnOK(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	switch (m_favoriteGroupMode) {
	case kFavoriteGroupAdd:
		{
			HTREEITEM htSel = m_tree.GetSelectedItem();
			if (htSel == NULL)
				return ;
			if (m_tree.IsRootChildren(htSel) == false)
				htSel = m_tree.GetParentItem(htSel);
			m_tree.AddActiveTab(htSel);
			OnCancel(0, IDOK, NULL);
		}
		break;

	case kFavoriteGroupOrganize:
		{
			OnCancel(0, IDOK, NULL);
		}
		break;
	}
}

void CFavoriteGroupEditDialog::OnDelete(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	HTREEITEM htSel = m_tree.GetSelectedItem();
	if (htSel == NULL)
		return ;
	if (m_tree.IsRootChildren(htSel) == false) {
		HTREEITEM htParent = m_tree.GetParentItem(htSel);
		LPCTSTR filePath = (LPCTSTR)m_tree.GetItemData(htParent);
		CDonutTabList	tabList;
		if (tabList.Load(filePath)) {
			int nIndex = m_tree.GetIndexFromHTREEITEM(htParent, htSel);
			ATLASSERT(nIndex != -1);
			tabList.Delete(nIndex);
			m_tree.DeleteItem(htSel);
			tabList.Save(filePath, false);
		}
	} else {
		LPCTSTR filePath = (LPCTSTR)m_tree.GetItemData(htSel);
		auto pvecFileList = CRootFavoriteGroupPopupMenu::GetFavoriteGroupFilePathList();
		for (auto it = pvecFileList->begin(); it != pvecFileList->end(); ++it) {
			if ((*it) == filePath) {
				::DeleteFile(filePath);
				pvecFileList->erase(it);				

				m_tree._RefreshTree();
				break;
			}
		}
	}
}

void CFavoriteGroupEditDialog::OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CIniFileIO pr(g_szIniFileName, _T("FavoriteGroupEditDialog"));
	CRect rc;
	GetWindowRect(&rc);
	pr.SetValue(rc.Width()	, _T("Width"));
	pr.SetValue(rc.Height()	, _T("Height"));

	m_tree.DestroyWindow();

	CRootFavoriteGroupPopupMenu::LoadFavoriteGroup();

	EndDialog(nID);
}

