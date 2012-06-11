/**
 *	@file	FileNewOption.cpp
 *	@brief	donut のオプション : "新規作成"
 */

#include "stdafx.h"
#include "FileNewOption.h"
#include "../IniFile.h"
#include "../Misc.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





////////////////////////////////////////////////////////////////////////////////
//CFileNewOptionの定義
////////////////////////////////////////////////////////////////////////////////

DWORD 	CFileNewOption::s_dwFlags = FILENEW_BLANK;
CString CFileNewOption::s_strUsr;


void CFileNewOption::GetProfile()
{
	CIniFileI	pr( g_szIniFileName, _T("Main") );
	s_dwFlags = pr.GetValue( _T("File_New_Option"), FILENEW_BLANK );
	s_strUsr  = pr.GetStringUW( _T("File_New_UsrPage") );
	pr.Close();
}



void CFileNewOption::WriteProfile()
{
	CIniFileO	pr( g_szIniFileName, _T("Main") );
	pr.SetValue( s_dwFlags, _T("File_New_Option") );
	pr.SetStringUW( s_strUsr, _T("File_New_UsrPage") );
	pr.Close();
}



////////////////////////////////////////////////////////////////////////////////
//CFileNewPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////

// Constructor
CFileNewPropertyPage::CFileNewPropertyPage()
	: m_nRadio(0)
{
	_SetData();
}



// Overrides

BOOL CFileNewPropertyPage::OnSetActive()
{
	if (m_editUsr.m_hWnd == NULL) {
		m_editUsr.Attach( GetDlgItem(IDC_EDIT_NEWPAGE_USER) );
		m_editUsr.SetWindowText(CFileNewOption::s_strUsr);
	}
	SetModified(TRUE);
	return DoDataExchange(FALSE);
}



BOOL CFileNewPropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CFileNewPropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



// Implementation
void CFileNewPropertyPage::_GetData()
{
	// update dl control flags
	CFileNewOption::s_dwFlags = 0;
	switch (m_nRadio) {
	case  0: CFileNewOption::s_dwFlags = FILENEW_BLANK; break;
	case  1: CFileNewOption::s_dwFlags = FILENEW_COPY ; break;
	case  2: CFileNewOption::s_dwFlags = FILENEW_HOME ; break;
	case  3: CFileNewOption::s_dwFlags = FILENEW_USER ; break;
	default: ATLASSERT(FALSE);							break;
	}
	TCHAR usr[ MAX_PATH + 4];
	usr[0]	= 0;							//+++
	m_editUsr.GetWindowText(usr, MAX_PATH);	//+++
	CFileNewOption::s_strUsr	= usr;
}



void CFileNewPropertyPage::_SetData()
{
	if		(CFileNewOption::s_dwFlags == FILENEW_BLANK) m_nRadio = 0;
	else if (CFileNewOption::s_dwFlags == FILENEW_COPY ) m_nRadio = 1;
	else if (CFileNewOption::s_dwFlags == FILENEW_HOME ) m_nRadio = 2;
	else if (CFileNewOption::s_dwFlags == FILENEW_USER ) m_nRadio = 3;
	else	ATLASSERT(FALSE);

	//if (m_editUsr.m_hWnd == NULL)
	//	m_editUsr.Attach( GetDlgItem(IDC_EDIT_NEWPAGE_USER) );
	//m_editUsr.SetWindowText(CFileNewOption::s_strUsr);
}



//+++ 追加.
void CFileNewPropertyPage::OnButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	TCHAR			   szOldPath[MAX_PATH];
	szOldPath[0]	= 0;
	::GetCurrentDirectory(MAX_PATH, szOldPath);
	::SetCurrentDirectory( Misc::GetExeDirectory() );

	static const TCHAR szFilter[] = _T("全ファイル(*.*)\0*.*\0\0");

	CFileDialog 	   fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	fileDlg.m_ofn.lpstrTitle = _T("ユーザー指定の新規作成ページ");

	if (fileDlg.DoModal() == IDOK) {
		m_editUsr.SetWindowText(fileDlg.m_szFileName);
	}

	// restore current directory
	::SetCurrentDirectory(szOldPath);
}
