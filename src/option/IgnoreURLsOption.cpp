/**
 *	@file	IgnoreURLsOption.cpp
 *	@brief	donutのオプション : ポップアップ抑止.
 */

#include "stdafx.h"
#include "IgnoreURLsOption.h"
#include "../IniFile.h"
#include "../DonutPFunc.h"
#include "../MtlMisc.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace MTL;



////////////////////////////////////////////////////////////////////////////////
//CIgnoredURLsOptionの定義
////////////////////////////////////////////////////////////////////////////////

CIgnoredURLsOption::CStringList * CIgnoredURLsOption::s_pIgnoredURLs = NULL;
bool CIgnoredURLsOption::s_bValid									 = true;



void CIgnoredURLsOption::GetProfile()
{
	CIniFileI	pr( g_szIniFileName, _T("IgnoredURLs") );
	DWORD		dwValid = 0;	//+++ = 0.

	LONG	lRet = pr.QueryValue( dwValid, _T("IsValid") );
	if (lRet == ERROR_SUCCESS)
		s_bValid = (dwValid == 1);

	s_pIgnoredURLs = new CStringList;
	//		MtlGetProfileString(pr, std::back_inserter(*s_pIgnoredURLs), _T("url"));
	::FileReadString(GetConfigFilePath( _T("CloseURL.ini") ), *s_pIgnoredURLs);
}



void CIgnoredURLsOption::WriteProfile()
{
	//x MtlIniDeleteSection( g_szIniFileName, _T("IgnoredURLs") );	// clean up 	//メンバー関数に置換.
	CIniFileO	 pr( g_szIniFileName, _T("IgnoredURLs") );
	pr.DeleteSection();
	//x pr.SetValue( s_bValid == true ? 1 : 0, _T("IsValid") );
	pr.SetValue( s_bValid != 0, _T("IsValid") );

	//		MtlWriteProfileString(s_pIgnoredURLs->begin(), s_pIgnoredURLs->end(), pr, _T("url"));
	FileWriteString(GetConfigFilePath( _T("CloseURL.ini") ), *s_pIgnoredURLs);

	delete	s_pIgnoredURLs;
	s_pIgnoredURLs = NULL;	//+++ deleteしたらクリア.
}



bool CIgnoredURLsOption::SearchString(const CString &strURL)
{
	ATLASSERT(s_pIgnoredURLs != NULL);

	if (!s_bValid)												// allways can't found
		return false;

	return MtlSearchStringWildCard(s_pIgnoredURLs->begin(), s_pIgnoredURLs->end(), strURL);
}



void CIgnoredURLsOption::Add(const CString &strURL)
{
	if ( !SearchString(strURL) )
		s_pIgnoredURLs->push_back(strURL);
}



////////////////////////////////////////////////////////////////////////////////
//CIgnoredURLsPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////

// Constructor
CIgnoredURLsPropertyPage::CIgnoredURLsPropertyPage(const CString &strAddressBar)
	: m_urls(*CIgnoredURLsOption::s_pIgnoredURLs)
	, m_strAddressBar(strAddressBar)
	, m_wndList(this, 1)
{
	m_urls.sort();
	m_nValid = CIgnoredURLsOption::s_bValid != 0;		//+++ ? 1 : 0;
}



// Overrides
BOOL CIgnoredURLsPropertyPage::OnSetActive()
{
	SetModified(TRUE);
	return _DoDataExchange(FALSE);
}



BOOL CIgnoredURLsPropertyPage::OnKillActive()
{
	return _DoDataExchange(TRUE);
}



BOOL CIgnoredURLsPropertyPage::OnApply()
{
	if ( _DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



// overrides
BOOL CIgnoredURLsPropertyPage::_DoDataExchange(BOOL bSaveAndValidate)	// get data from controls?
{
	if (!bSaveAndValidate) {											// set data of control
		if (!m_listbox.m_hWnd)
			m_listbox.Attach( GetDlgItem(IDC_IGNORED_URL_LIST) );

		m_listbox.ResetContent();
		std::for_each( m_urls.begin(), m_urls.end(), _AddToListBox(m_listbox) );

		if (!m_edit.m_hWnd)
			m_edit.Attach( GetDlgItem(IDC_URL_EDIT) );

		m_edit.SetWindowText(m_strAddressBar);
	}

	return DoDataExchange(bSaveAndValidate);
}



LRESULT CIgnoredURLsPropertyPage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_wndList.SubclassWindow( GetDlgItem(IDC_IGNORED_URL_LIST) );
	return 0;
}



LRESULT CIgnoredURLsPropertyPage::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_wndList.UnsubclassWindow();
	return 0;
}



void CIgnoredURLsPropertyPage::OnDelCmd(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	//とりあえず選択項目の番号をリストにしてソート
	std::list<int> sellist;
	int 		   nSelCount = m_listbox.GetSelCount();
	int *		   pIdx 	 = (int *) new int[nSelCount];

	if (m_listbox.GetSelItems(nSelCount, pIdx) != LB_ERR) {
		for (int i = 0; i < nSelCount; i++)
			sellist.push_back(pIdx[i]);

		sellist.sort();
		sellist.reverse();

		for (std::list<int>::iterator it = sellist.begin(); it != sellist.end(); ++it)
			m_listbox.DeleteString(*it);
	}

	delete[] pIdx;
}



void CIgnoredURLsPropertyPage::OnDelAllCmd(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	m_listbox.ResetContent();
}



void CIgnoredURLsPropertyPage::OnAddCmd(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	TCHAR 	szEdit[INTERNET_MAX_PATH_LENGTH];
	szEdit[0]	= 0;	//+++

	if (m_edit.GetWindowText(szEdit, INTERNET_MAX_PATH_LENGTH) == 0)
		return;

	int   nIndex = m_listbox.FindStringExact(-1, szEdit);

	if (nIndex == LB_ERR) {
		m_listbox.AddString(szEdit);
	} else {
		//m_listbox.SetCurSel(nIndex);
		m_listbox.SetSel(nIndex, TRUE);
	}
}



void CIgnoredURLsPropertyPage::OnSelChange(UINT code, int id, HWND hWnd)
{
	//int nIndex = m_listbox.GetCurSel();
	int 	nIndex = 0;

	if (m_listbox.GetSelItems(1, &nIndex) == 0)
		return;

	CString 	strBox;
	m_listbox.GetText(nIndex, strBox);
	m_edit.SetWindowText(strBox);
}



void CIgnoredURLsPropertyPage::OnListKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DELETE)
		OnDelCmd(0, 0, NULL);
}



void CIgnoredURLsPropertyPage::_GetData()
{
	// update ignored urls list
	int 	nCount = m_listbox.GetCount();
	CString strbuf;

	m_urls.clear();

	for (int i = 0; i < nCount; i++) {
		m_listbox.GetText(i, strbuf);
		m_urls.push_back(strbuf);
	}

	*CIgnoredURLsOption::s_pIgnoredURLs = m_urls;
	CIgnoredURLsOption::s_bValid		= m_nValid != 0;		//+++ == 1 ? true : false;
}
