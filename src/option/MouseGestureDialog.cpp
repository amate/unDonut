/**
 *	@file	MouseGestureDialog.cpp
 *	@brief	donutのオプション : マウスジェスチャ
 */

#include "stdafx.h"
#include "MouseGestureDialog.h"
#include "../MtlBase.h"
#include "../IniFile.h"
#include "../DonutPFunc.h"
#include "../ToolTipManager.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace MTL;

CMouseGesturePropertyPage::CMouseGesturePropertyPage(HMENU hMenu)
{
	m_hMenu   = hMenu;
	m_strPath = _GetFilePath( _T("MouseEdit.ini") );
}


// Overrides
BOOL CMouseGesturePropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_cmbCategory.m_hWnd == NULL) {
		if (m_cmbCategory.m_hWnd == NULL)
			m_cmbCategory.Attach( GetDlgItem(IDC_CMB_CATEGORY) );

		if (m_cmbCommand.m_hWnd == NULL)
			m_cmbCommand.Attach( GetDlgItem(IDC_CMB_COMMAND) );

		OnInitCmb();
		OnInitList();

		m_stcMoveCmd.Attach( GetDlgItem(IDC_STC01) );
	}

	_SetData();
	return DoDataExchange(FALSE);
}


BOOL CMouseGesturePropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}


BOOL CMouseGesturePropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}


// Constructor
// データを得る
void CMouseGesturePropertyPage::_SetData()
{
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
	m_ltMoveCmd.Attach( GetDlgItem(IDC_LISTBOX) );

	static const TCHAR* titles[] = { _T("ジェスチャー"), _T("コマンド") };
	static const int	widths[] = { 350, 150 };

	LVCOLUMN		col;
	col.mask = LVCF_TEXT | LVCF_WIDTH;

	for (int i = 0; i < _countof(titles); ++i) {
		col.pszText = (LPTSTR) titles[i];
		col.cx		= widths[i];
		m_ltMoveCmd.InsertColumn(i, &col);
	}

	m_ltMoveCmd.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

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


void CMouseGesturePropertyPage::OnBtnAdd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	int 	nIndexCmd = m_cmbCommand.GetCurSel();
	UINT	nCmdID	  = (UINT) m_cmbCommand.GetItemData(nIndexCmd);

	if (nCmdID == 0)
		return;

	TCHAR	szText[255];
	szText[0]	= 0;	//+++
	CString strText;
	m_stcMoveCmd.GetWindowText(szText, 255);
	strText = szText;

	if ( strText.IsEmpty() )
		return;

	CString strCmd;
	CToolTipManager::LoadToolTipText(nCmdID, strCmd);

	if ( strCmd.IsEmpty() )
		return;

	int 	nCnt	  = m_ltMoveCmd.GetItemCount();
	nCnt	= m_ltMoveCmd.InsertItem(nCnt, strText);
	m_ltMoveCmd.SetItemText(nCnt, 1, strCmd);
	m_ltMoveCmd.SetItemData(nCnt, nCmdID);

	m_stcMoveCmd.SetWindowText(_T(""));
	::EnableWindow(GetDlgItem(IDC_BTN_ADD), FALSE);
	::EnableWindow(GetDlgItem(IDC_BTN_DEL), FALSE);
}


void CMouseGesturePropertyPage::OnBtnDel(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	int 	nIndex = m_ltMoveCmd.GetSelectedIndex();

	if (nIndex == -1)
		return;

	CString strJst;
	m_ltMoveCmd.GetItemText(nIndex, 0, strJst);
	m_aryDelJst.Add(strJst);

	m_ltMoveCmd.DeleteItem(nIndex);
	::EnableWindow(GetDlgItem(IDC_BTN_DEL), TRUE);
}


void CMouseGesturePropertyPage::OnBtn(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	TCHAR	szText[255];
	szText[0]	= 0;	//+++
	CString strText;

	m_stcMoveCmd.GetWindowText(szText, 255);
	strText = szText;

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


void CMouseGesturePropertyPage::OnSelChangeCmd(UINT code, int id, HWND hWnd)
{
	EnableAddBtn();
}


void CMouseGesturePropertyPage::EnableAddBtn()
{
	TCHAR	szText[255];
	szText[0]	= 0;	//+++
	CString strNowJst;

	m_stcMoveCmd.GetWindowText(szText, 255);
	strNowJst = szText;

	BOOL	bEnable   = TRUE;

	int 	nIndexCmd = m_cmbCommand.GetCurSel();
	UINT	nCmdID	  = (UINT) m_cmbCommand.GetItemData(nIndexCmd);

	if (nCmdID == 0 || nIndexCmd == -1)
		bEnable = FALSE;

	for (int ii = 0; ii < m_ltMoveCmd.GetItemCount() && bEnable; ii++) {
		CString strMoveJst;
		m_ltMoveCmd.GetItemText(ii, 0, strMoveJst);

		if (strNowJst != strMoveJst)
			continue;

		bEnable = FALSE;
		break;
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
	_PickUpCommand(m_hMenu, nIndex, m_cmbCommand);
}
