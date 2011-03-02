/**
 *	@file	ExplorerBarDialog.cpp
 *	@brief	donutのオプション : エクスプローラバー
 */

#include "stdafx.h"
#include "ExplorerBarDialog.h"
#include "MainOption.h"
#include "../ItemIDList.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




// Constructor
CExplorerPropertyPage::CExplorerPropertyPage()
{
	_SetData();
}



// Overrides

BOOL CExplorerPropertyPage::OnSetActive()
{
	SetModified(TRUE);
	return DoDataExchange(FALSE);
}



BOOL CExplorerPropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CExplorerPropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



void CExplorerPropertyPage::OnUserFolder(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	CString strPath = _BrowseForFolder();

	if ( !strPath.IsEmpty() ) {
		m_strUserFolder = strPath;
		DoDataExchange(FALSE);
	}
}



CString CExplorerPropertyPage::_BrowseForFolder()
{
	TCHAR		szDisplayName[MAX_PATH];
	::ZeroMemory(szDisplayName, sizeof szDisplayName);

	BROWSEINFO	bi = {
		m_hWnd, 			  NULL, szDisplayName, _T("ユーザー定義のお気に入りフォルダ"),
		BIF_RETURNONLYFSDIRS, NULL, 0,				0,
	};

	CItemIDList idl;

	idl.Attach( ::SHBrowseForFolder(&bi) );
	return idl.GetPath();
}



void CExplorerPropertyPage::_GetData()
{
	CMainOption::s_dwExplorerBarStyle	   = 0;

	if (m_nExplorerHScroll	/*== 1*/) CMainOption::s_dwExplorerBarStyle |= MAIN_EXPLORER_HSCROLL	;
	if (m_nExplorerNoSpace	/*== 1*/) CMainOption::s_dwExplorerBarStyle |= MAIN_EXPLORER_NOSPACE	;
	if (m_nExplorerOrgImage /*== 1*/) CMainOption::s_dwExplorerBarStyle |= MAIN_EXPLORER_FAV_ORGIMG ;
	if (m_nExplorerDragDrop /*== 1*/) CMainOption::s_dwExplorerBarStyle |= MAIN_EXPLORER_NODRAGDROP ;
	if (m_nExplorerAutoShow /*== 1*/) CMainOption::s_dwExplorerBarStyle |= MAIN_EXPLORER_AUTOSHOW	;

	CMainOption::SetExplorerUserDirectory( m_strUserFolder );
}



void CExplorerPropertyPage::_SetData()
{
	m_nExplorerHScroll	= (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_HSCROLL	) != 0;
	m_nExplorerNoSpace	= (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_NOSPACE	) != 0;
	m_nExplorerOrgImage = (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_FAV_ORGIMG ) != 0;
	m_nExplorerDragDrop = (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_NODRAGDROP ) != 0;
	m_nExplorerAutoShow = (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_AUTOSHOW	) != 0;

	m_strUserFolder 	= CMainOption::GetExplorerUserDirectory();
}
