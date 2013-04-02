/**
 *	@file	CommandSelectDialog.cpp
 *	@brief	"コマンドを選ぶ"ダイアログ
 */
#include "stdafx.h"
#include "CommandSelectDialog.h"
#include "..\DonutPFunc.h"
#include "..\ToolTipManager.h"


////////////////////////////////////////////////////////////////////////////////////
// CCommandSelectDialog

CCommandSelectDialog::CCommandSelectDialog(HMENU hMenu)
	: m_dwCommandID(0)
{
	m_hMenu = hMenu;
}



DWORD_PTR CCommandSelectDialog::GetCommandID()
{
	return m_dwCommandID;
}



LRESULT CCommandSelectDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	CenterWindow( GetParent() );
	DoDataExchange(FALSE);

	m_cmbCategory.Attach( GetDlgItem(IDC_CMB_CATEGORY) );
	m_cmbCommand.Attach( GetDlgItem(IDC_CMB_COMMAND) );

	if (_SetCombboxCategory(m_cmbCategory, m_hMenu) == FALSE)
		return FALSE;

	// する事
	m_cmbCommand.SetDroppedWidth(250);
	OnSelChangeCate(0, 0, 0);
	return TRUE;
}



LRESULT CCommandSelectDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}



void CCommandSelectDialog::OnSelChangeCate(UINT code, int id, HWND hWnd)
{
	int nIndex = m_cmbCategory.GetCurSel();

	// コマンド選択
	PickUpCommand(m_hMenu, nIndex, m_cmbCommand);

	m_dwCommandID = 0;
	SetGuideMsg();
}



// コマンド変更時
void CCommandSelectDialog::OnSelChangeCmd(UINT code, int id, HWND hWnd)
{
	int nIndex = m_cmbCommand.GetCurSel();

	if (nIndex == -1) return;

	m_dwCommandID = m_cmbCommand.GetItemData(nIndex);
	SetGuideMsg();
}



void CCommandSelectDialog::SetGuideMsg()
{
	CString strGuide;

	if (m_dwCommandID != 0)
		strGuide.LoadString( UINT(m_dwCommandID) );
	else
		strGuide = _T("コマンドを選定しません。");

	int nFindIt = strGuide.Find( _T('\t') );
	if (nFindIt != -1)
		strGuide = strGuide.Mid(nFindIt + 1);

	nFindIt = strGuide.Find( _T('\n') );

	if (nFindIt != -1)
		strGuide = strGuide.Left(nFindIt);

	::SetWindowText(::GetDlgItem(m_hWnd, IDC_STC01), strGuide);
}
