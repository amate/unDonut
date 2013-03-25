/**
 *	@file	ExplorerBarDialog.cpp
 *	@brief	donutのオプション : エクスプローラバー
 */

#include "stdafx.h"
#include "ExplorerBarDialog.h"
#include "..\IniFile.h"

///////////////////////////////////////////////////////
// CExplorerBarOption

DWORD CExplorerBarOption::s_dwExplorerBarStyle = 0;

void CExplorerBarOption::GetProfile()
{
	CIniFileI	pr(g_szIniFileName, _T("ExplorerBar"));
	pr.QueryValue(s_dwExplorerBarStyle, _T("ExplorerBar_Style"));
}

void CExplorerBarOption::WriteProfile()
{
	CIniFileIO pr(g_szIniFileName, _T("ExplorerBar"));
	pr.SetValue(s_dwExplorerBarStyle, _T("ExplorerBar_Style"));
}

//////////////////////////////////////////////////////
// CExplorerBarPropertyPage

// Constructor
CExplorerBarPropertyPage::CExplorerBarPropertyPage(function<void (bool)>	func) : m_funcHookForAutoShow(func)
{
	m_bExplorerBarAutoShow	= (s_dwExplorerBarStyle & EXPLORERBAROPTION_AUTOSHOW) != 0;
}



// Overrides

BOOL CExplorerBarPropertyPage::OnSetActive()
{
	SetModified(TRUE);
	return DoDataExchange(DDX_LOAD);
}



BOOL CExplorerBarPropertyPage::OnKillActive()
{
	return DoDataExchange(DDX_SAVE);
}



BOOL CExplorerBarPropertyPage::OnApply()
{
	if ( DoDataExchange(DDX_SAVE) ) {
		s_dwExplorerBarStyle = 0;
		if (m_bExplorerBarAutoShow)	s_dwExplorerBarStyle |= EXPLORERBAROPTION_AUTOSHOW;

		m_funcHookForAutoShow(m_bExplorerBarAutoShow != 0);
		
		WriteProfile();

		return TRUE;
	} else {
		return FALSE;
	}
}

