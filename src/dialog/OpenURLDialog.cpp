/**
 *	@file OpenURLDialog.cpp
 */

#include "stdafx.h"
#include "OpenURLDialog.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



// Constructor
COpenURLDlg::COpenURLDlg()
{
}



// Handler
LRESULT COpenURLDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	CenterWindow( GetParent() );
	DoDataExchange(FALSE);

	m_edit.Attach( GetDlgItem(IDC_EDIT_URL) );
	return 0;
}



LRESULT COpenURLDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	DoDataExchange(TRUE);
	EndDialog(wID);
	return 0;
}



void COpenURLDlg::OnRefCmd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	const TCHAR szFilter[] = _T("HTMLファイル(*.htm;*.html)\0*.htm;*.html\0")
							 _T("タブリストファイル(*.xml)\0*.xml\0")
							 _T("Donut Favorite Groupファイル(*.dfg)\0*.dfg\0")
							 _T("すべてのファイル(*.*)\0*.*\0\0");

	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);

	fileDlg.m_ofn.lpstrTitle = _T("開く");

	if (fileDlg.DoModal() == IDOK) {
		m_edit.SetWindowText(fileDlg.m_szFileName);
	}
}
