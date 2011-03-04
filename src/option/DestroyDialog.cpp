/**
 *	@file	DestroyDialog.cpp
 *	@brief	donutのオプション: 終了時の処理.
 */

#include "stdafx.h"
#include "DestroyDialog.h"
#include "MainOption.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDestroyPropertyPage::CDestroyPropertyPage()
  #if 0 //+++ _SetDataで5変数を初期化するので不要.
	: m_nDelCash(0)
	, m_nDelCookie(0)
	, m_nDelHistory(0)
	, m_nMakeCash(0)
	, m_nDelRecentClose(0)
  #endif
{
	_SetData();
}



// Overrides
BOOL CDestroyPropertyPage::OnSetActive()
{
	SetModified(TRUE);
	return DoDataExchange(FALSE);
}



BOOL CDestroyPropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CDestroyPropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



void CDestroyPropertyPage::_GetData()
{
	// Update main style
	// UH
	 //+++	ここで使う以外のフラグはガードしとく.
	CMainOption::s_dwMainExtendedStyle2   &= ~(MAIN_EX2_DEL_CASH|MAIN_EX2_DEL_COOKIE|MAIN_EX2_DEL_HISTORY|MAIN_EX2_MAKECASH|MAIN_EX2_DEL_RECENTCLOSE);

	if (m_nDelCash /*== 1*/)
		CMainOption::s_dwMainExtendedStyle2 |= MAIN_EX2_DEL_CASH;

	if (m_nDelCookie /*== 1*/)
		CMainOption::s_dwMainExtendedStyle2 |= MAIN_EX2_DEL_COOKIE;

	if (m_nDelHistory /*== 1*/)
		CMainOption::s_dwMainExtendedStyle2 |= MAIN_EX2_DEL_HISTORY;

	if (m_nMakeCash /*== 1*/)
		CMainOption::s_dwMainExtendedStyle2 |= MAIN_EX2_MAKECASH;

	if (m_nDelRecentClose /*== 1*/)
		CMainOption::s_dwMainExtendedStyle2 |= MAIN_EX2_DEL_RECENTCLOSE;
}



void CDestroyPropertyPage::_SetData()
{
	// UH
	m_nDelCash		  = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_CASH		) != 0;
	m_nDelCookie	  = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_COOKIE		) != 0;
	m_nDelHistory	  = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_HISTORY 	) != 0;
	m_nMakeCash 	  = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MAKECASH		) != 0;
	m_nDelRecentClose = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_RECENTCLOSE ) != 0;
}
