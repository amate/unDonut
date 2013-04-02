/**
 *	@file	MouseGestureDialog.cpp
 *	@brief	donutのオプション : マウスジェスチャ
 */

#include "stdafx.h"
#include "MouseGestureDialog.h"
#include "..\MtlBase.h"
#include "..\MtlWin.h"
#include "..\IniFile.h"
#include "..\DonutPFunc.h"
#include "..\ToolTipManager.h"


using namespace MTL;

////////////////////////////////////////////////////////////////////
// CMouseGesturePropertyPage

CMouseGesturePropertyPage::CMouseGesturePropertyPage(HMENU hMenu) : m_bInit(false)
{
	m_hMenu   = hMenu;
	m_strPath = GetConfigFilePath( _T("MouseEdit.ini") );
}


// Overrides
BOOL CMouseGesturePropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_bInit == false) {
		m_bInit = true;

		DoDataExchange(DDX_LOAD);

		OnInitCmb();
		OnInitList();

	}
	
	return TRUE;
}


BOOL CMouseGesturePropertyPage::OnKillActive()
{
	return TRUE;
}


BOOL CMouseGesturePropertyPage::OnApply()
{
	_GetData();
	return TRUE;
}


// データを保存
void CMouseGesturePropertyPage::_GetData()
{
	{
		CIniFileO	pr( m_strPath, _T("MouseCtrl") );

		int 		ii;
		for (ii = 0; ii < m_mapMouseCmd.GetSize(); ii++) {
			CString strKey	  = m_mapMouseCmd.GetKeyAt(ii);
			DWORD	dwCommand = m_mapMouseCmd.GetValueAt(ii);

			pr.SetValue(dwCommand, strKey);
		}

		for (ii = 0; ii < m_aryDelJst.GetSize(); ii++)
			pr.DeleteValue(m_aryDelJst[ii]);

		for (ii = 0; ii < m_ltMoveCmd.GetItemCount(); ii++) {
			CString strMoveCmd;
			m_ltMoveCmd.GetItemText(ii, 0, strMoveCmd);
			DWORD	dwCommand = 0;
			dwCommand = DWORD( m_ltMoveCmd.GetItemData(ii) );

			pr.SetValue(dwCommand, strMoveCmd);
		}

		//x pr.Close(); //+++
	}

	{
		CIniFileO	pr( m_strPath, _T("MouseJst") );
		pr.SetValue( m_ltMoveCmd.GetItemCount(), _T("Count") );

		for (int ii = 0; ii < m_ltMoveCmd.GetItemCount(); ii++) {
			CString strMoveCmd;
			m_ltMoveCmd.GetItemText(ii, 0, strMoveCmd);

			CString strKey;
			strKey.Format(_T("%d"), ii);
			pr.SetString(strMoveCmd, strKey);
		}
	}
}


void CMouseGesturePropertyPage::OnInitCmb()
{
	// カテゴリセット
	_SetCombboxCategory(m_cmbCategory, m_hMenu);
	OnSelChangeCate(0, 0, 0);
}


void CMouseGesturePropertyPage::OnInitList()
{
	static const TCHAR* titles[] = { _T("ジェスチャー"), _T("コマンド") };
	static const int	widths[] = { 90, 140 };

	LVCOLUMN		col;
	col.mask = LVCF_TEXT | LVCF_WIDTH;

	for (int i = 0; i < _countof(titles); ++i) {
		col.pszText = (LPTSTR) titles[i];
		col.cx		= widths[i];
		m_ltMoveCmd.InsertColumn(i, &col);
	}

	m_ltMoveCmd.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES);

	CIniFileI		pr( m_strPath, _T("MouseJst") );
	CIniFileI		pr2( m_strPath, _T("MouseCtrl") );

	DWORD			dwCount  = 0;
	pr.QueryValue( dwCount, _T("Count") );

	for (int ii = 0; ii < (int) dwCount; ii++) {
		CString 	strMoveCmd;
		m_ltMoveCmd.GetItemText(ii, 0, strMoveCmd);

		CString 	strKey;
		strKey.Format(_T("%d"), ii);

		CString strJst	= pr.GetString( strKey );
		DWORD	dwCmdID = pr2.GetValue( strJst, 0 );

		CString 	strCmdName;
		CToolTipManager::LoadToolTipText(dwCmdID, strCmdName);

		m_ltMoveCmd.InsertItem(ii, strJst);
		m_ltMoveCmd.SetItemText(ii, 1, strCmdName);
		m_ltMoveCmd.SetItemData(ii, dwCmdID);
	}

	::EnableWindow(GetDlgItem(IDC_BTN_ADD), FALSE);
	::EnableWindow(GetDlgItem(IDC_BTN_DEL), FALSE);
}

/// 登録
void CMouseGesturePropertyPage::OnBtnAdd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	int 	nIndexCmd = m_listCommand.GetCurSel();
	UINT	nCmdID	  = (UINT) m_listCommand.GetItemData(nIndexCmd);
	if (nCmdID == 0)
		return;

	CString strText = MtlGetWindowText(m_stcMoveCmd);
	if ( strText.IsEmpty() )
		return;

	CString strCmd;
	CToolTipManager::LoadToolTipText(nCmdID, strCmd);
	if ( strCmd.IsEmpty() )
		return;

	// コマンドを上書きする場合、前のジェスチャーを削除する
	int nCount = m_ltMoveCmd.GetItemCount();
	for (int i = 0; i < nCount; ++i) {
		CString strGesture;
		m_ltMoveCmd.GetItemText(i, 0, strGesture);
		if (strGesture == strText) {
			m_ltMoveCmd.DeleteItem(i);
			break;
		}
	}

	int nCnt = m_ltMoveCmd.GetItemCount();
	nCnt	= m_ltMoveCmd.InsertItem(nCnt, strText);
	m_ltMoveCmd.SetItemText(nCnt, 1, strCmd);
	m_ltMoveCmd.SetItemData(nCnt, nCmdID);
	m_ltMoveCmd.SelectItem(nCnt);

	m_stcMoveCmd.SetWindowText(_T(""));
	GetDlgItem(IDC_BTN_ADD).SetWindowText(_T("登録"));
	GetDlgItem(IDC_STATIC_EXSISTCOMMAND).SetWindowText(_T(""));
	::EnableWindow(GetDlgItem(IDC_BTN_ADD), FALSE);
	::EnableWindow(GetDlgItem(IDC_BTN_DEL), TRUE);
}

/// 変更
void	CMouseGesturePropertyPage::OnBtnChange(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	CString strText = MtlGetWindowText(m_stcMoveCmd);
	if ( strText.IsEmpty() )
		return;

	int nSelIndex = m_ltMoveCmd.GetSelectedIndex();
	if (nSelIndex == -1)
		return ;

	CString strExsistCommand = MtlGetWindowText(GetDlgItem(IDC_STATIC_EXSISTCOMMAND));
	if (strExsistCommand.GetLength() > 0) {
		if (MessageBox(_T("同じジェスチャーがすでに登録されています。\n上書きしますか？"), _T("確認"), MB_OKCANCEL) == IDCANCEL)
			return ;

		// 同じジェスチャーを削除する
		int nCount = m_ltMoveCmd.GetItemCount();
		for (int i = 0; i < nCount; ++i) {
			if (i == nSelIndex)
				continue ;
			CString strGesture;
			m_ltMoveCmd.GetItemText(i, 0, strGesture);
			if (strGesture == strText) {
				m_ltMoveCmd.DeleteItem(i);
				break;
			}
		}
	}

	// ジェスチャーを変更
	m_ltMoveCmd.SetItemText(nSelIndex, 0, strText);

	m_stcMoveCmd.SetWindowText(_T(""));
	GetDlgItem(IDC_STATIC_EXSISTCOMMAND).SetWindowText(_T(""));
	::EnableWindow(GetDlgItem(IDC_BTN_ADD), FALSE);
}

/// 削除
void CMouseGesturePropertyPage::OnBtnDel(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	int nIndex = m_ltMoveCmd.GetSelectedIndex();
	if (nIndex == -1)
		return;

	CString strJst;
	m_ltMoveCmd.GetItemText(nIndex, 0, strJst);
	m_aryDelJst.Add(strJst);

	m_ltMoveCmd.DeleteItem(nIndex);
	if (m_ltMoveCmd.GetItemCount() == nIndex)
		--nIndex;
	m_ltMoveCmd.SelectItem(nIndex);
	::EnableWindow(GetDlgItem(IDC_BTN_DEL), TRUE);
}

// 矢印
void CMouseGesturePropertyPage::OnBtn(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	CString strText = MtlGetWindowText(m_stcMoveCmd);

	switch (wID) {
	case IDC_BTN_UP:	strText += _T("↑");	break;
	case IDC_BTN_DOWN:	strText += _T("↓");	break;
	case IDC_BTN_LEFT:	strText += _T("←");	break;
	case IDC_BTN_RIGHT: strText += _T("→");	break;
	case IDC_BTN_CLEAR: strText  = _T("");		break;
	}

	m_stcMoveCmd.SetWindowText(strText);
	EnableAddBtn();
}

/// コマンドリストの選択アイテムが変わった
void CMouseGesturePropertyPage::OnSelChangeCommandList(UINT code, int id, HWND hWnd)
{
	EnableAddBtn();
}


/// ジェスチャーリストのアイテムをコマンドリストで選択状態にする
LRESULT CMouseGesturePropertyPage::OnGestureListDblClk(LPNMHDR pnmh)
{
	CPoint pt;
	::GetCursorPos(&pt);
	m_ltMoveCmd.ScreenToClient(&pt);
	UINT flags = 0;
	int nIndex = m_ltMoveCmd.HitTest(pt, &flags);
	if (nIndex != -1) {
		UINT cmd = m_ltMoveCmd.GetItemData(nIndex);
		if (cmd) {
			int nCategoryCount = m_cmbCategory.GetCount();
			for (int i = 0; i < nCategoryCount; ++i) {
				PickUpCommand(m_hMenu, i, m_listCommand);
				int nListCount = m_listCommand.GetCount();
				for (int k = 0; k < nListCount; ++k) {
					if (m_listCommand.GetItemData(k) == cmd) {
						m_cmbCategory.SetCurSel(i);
						m_listCommand.SetCurSel(k);
						OnSelChangeCommandList(0, 0, NULL);
						return 0;
					}
				}
			}
		}
	}
	return 0;
}

/// Deleteキーで選択ジェスチャーを消す
LRESULT CMouseGesturePropertyPage::OnGestureListKeyDown(LPNMHDR pnmh)
{
	auto pnkd = (LPNMLVKEYDOWN)pnmh;
	if (pnkd->wVKey == VK_DELETE) {
		int nSelIndex = m_ltMoveCmd.GetSelectedIndex();
		if (nSelIndex != -1) {
			m_ltMoveCmd.SelectItem(nSelIndex);
			OnBtnDel(0, 0, NULL);
		}
	}
	return 0;
}


void CMouseGesturePropertyPage::EnableAddBtn()
{
	CString strNowJst = MtlGetWindowText(m_stcMoveCmd);
	GetDlgItem(IDC_BTN_ADD).SetWindowText(_T("登録"));
	GetDlgItem(IDC_STATIC_EXSISTCOMMAND).SetWindowText(_T(""));

	BOOL	bEnable   = TRUE;

	int 	nIndexCmd = m_listCommand.GetCurSel();
	UINT	nCmdID	  = (UINT) m_listCommand.GetItemData(nIndexCmd);
	if (nCmdID == 0 || nIndexCmd == -1 || strNowJst.IsEmpty())
		bEnable = FALSE;

	for (int ii = 0; ii < m_ltMoveCmd.GetItemCount(); ++ii) {
		CString strMoveJst;
		m_ltMoveCmd.GetItemText(ii, 0, strMoveJst);
		if (strNowJst == strMoveJst) {
			CString strCommand;
			m_ltMoveCmd.GetItemText(ii, 1, strCommand);
			GetDlgItem(IDC_STATIC_EXSISTCOMMAND).SetWindowText(strCommand);
			GetDlgItem(IDC_BTN_ADD).SetWindowText(_T("上書き"));
			break;
		}
	}

	::EnableWindow(GetDlgItem(IDC_BTN_ADD), bEnable);
}


LRESULT CMouseGesturePropertyPage::OnItemChg(LPNMHDR pnmh)
{
	LPNMLISTVIEW pnmv = (LPNMLISTVIEW) pnmh;

	if (pnmv->uNewState != LBN_SELCANCEL)
		return 0;

	::EnableWindow(GetDlgItem(IDC_BTN_DEL), TRUE);
	return 0;
}


// カテゴリ変更時
void CMouseGesturePropertyPage::OnSelChangeCate(UINT code, int id, HWND hWnd)
{
	int nIndex = m_cmbCategory.GetCurSel();

	// コマンド選択
	PickUpCommand(m_hMenu, nIndex, m_listCommand);
}

