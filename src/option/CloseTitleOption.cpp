/**
 *	@file CloseTitleOption.cpp
 *	@brief	donutのオプション: タイトルによる表示抑止
 */

#include "stdafx.h"
#include "CloseTitleOption.h"

#include "../MtlMisc.h"
#include "../DonutPFunc.h"
#include "../IniFile.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace 	MTL;



////////////////////////////////////////////////////////////////////////////////
//CCloseTitlesOptionの定義
////////////////////////////////////////////////////////////////////////////////


//スタティック変数の定義
CCloseTitlesOption::CStringList *	CCloseTitlesOption::s_pCloseTitles	= NULL;
bool								CCloseTitlesOption::s_bValid		= true;



void CCloseTitlesOption::GetProfile()
{
	CIniFileI	pr( g_szIniFileName, _T("CloseTitles") );

	s_pCloseTitles = new CStringList;
	FileReadString(_GetFilePath( _T("CloseTitle.ini") ), s_pCloseTitles);

	DWORD		dwValid = 0;	//+++ = 0.
	LONG		lRet = pr.QueryValue( dwValid, _T("IsValid") );
	if (lRet == ERROR_SUCCESS)
		s_bValid = (dwValid == 1);
}



void CCloseTitlesOption::WriteProfile()
{

	//x MtlIniDeleteSection( g_szIniFileName, _T("CloseTitles") );	// clean up 	//+++ メンバー関数に置換
	CIniFileO	 pr( g_szIniFileName, _T("CloseTitles") );
	pr.DeleteSection();

	//x pr.SetValue( s_bValid == true ? 1 : 0, _T("IsValid") );
	pr.SetValue( s_bValid != 0, _T("IsValid") );
	pr.Close();

	FileWriteString(_GetFilePath( _T("CloseTitle.ini") ), s_pCloseTitles);

	delete s_pCloseTitles;
	s_pCloseTitles = NULL;	//+++ deleteしたらクリア.
}



bool CCloseTitlesOption::SearchString(const CString &strTitle)
{
	ATLASSERT(s_pCloseTitles != NULL);

	if (!s_bValid)												// allways can't found
		return false;

	return MtlSearchStringWildCard(s_pCloseTitles->begin(), s_pCloseTitles->end(), strTitle);
}



void CCloseTitlesOption::Add(const CString &strTitle)
{
	if ( !SearchString(strTitle) )
		s_pCloseTitles->push_back(strTitle);
}



////////////////////////////////////////////////////////////////////////////////
//CCloseTitlesPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////

//コンストラクタ
CCloseTitlesPropertyPage::CCloseTitlesPropertyPage(const CString &strAddressBar)
	: m_urls(*CCloseTitlesOption::s_pCloseTitles)
	, m_strAddressBar(strAddressBar)
	, m_wndList(this, 1)
{
	m_urls.sort();
	m_nValid = CCloseTitlesOption::s_bValid != 0; /* ? 1 : 0;*/
}



//メッセージハンドラ
LRESULT CCloseTitlesPropertyPage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_wndList.SubclassWindow( GetDlgItem(IDC_IGNORED_URL_LIST) );
	return 0;
}



LRESULT CCloseTitlesPropertyPage::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_wndList.UnsubclassWindow();
	return 0;
}



//コマンドハンドラ
void CCloseTitlesPropertyPage::OnDelCmd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
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



void CCloseTitlesPropertyPage::OnDelAllCmd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	m_listbox.ResetContent();
}



void CCloseTitlesPropertyPage::OnAddCmd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	TCHAR szEdit[MAX_PATH];
	szEdit[0]	= 0;	//+++

	if (m_edit.GetWindowText(szEdit, MAX_PATH) == 0)
		return;

	int   nIndex = m_listbox.FindStringExact(-1, szEdit);

	if (nIndex == LB_ERR) {
		m_listbox.AddString(szEdit);
	} else {
		//m_listbox.SetCurSel(nIndex);
		m_listbox.SetSel(nIndex, TRUE);
	}
}



void CCloseTitlesPropertyPage::OnSelChange(UINT code, int id, HWND hWnd)
{
	//int nIndex = m_listbox.GetCurSel();
	int 	nIndex = 0;

	if (m_listbox.GetSelItems(1, &nIndex) == 0)
		return;

	CString strBox;
	m_listbox.GetText(nIndex, strBox);
	m_edit.SetWindowText(strBox);
}



void CCloseTitlesPropertyPage::OnListKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DELETE)
		OnDelCmd(0, 0, NULL);
}



//内部関数
void CCloseTitlesPropertyPage::GetData()
{
	// update close title list
	int 	nCount = m_listbox.GetCount();
	CString strbuf;

	m_urls.clear();

	for (int i = 0; i < nCount; i++) {
		m_listbox.GetText(i, strbuf);
		m_urls.push_back(strbuf);
	}

	*CCloseTitlesOption::s_pCloseTitles = m_urls;
	CCloseTitlesOption::s_bValid		= (m_nValid != 0);	//+++ m_nValid == 1 ? true : false;
}



BOOL CCloseTitlesPropertyPage::DataExchange(BOOL bSaveAndValidate)	// get data from controls?
{
	if (!bSaveAndValidate) {										// set data of control
		if (!m_listbox.m_hWnd)
			m_listbox.Attach( GetDlgItem(IDC_IGNORED_URL_LIST) );

		m_listbox.ResetContent();
		std::for_each( m_urls.begin(), m_urls.end(), AddToListBox(m_listbox) );

		if (!m_edit.m_hWnd)
			m_edit.Attach( GetDlgItem(IDC_URL_EDIT) );

		m_edit.SetWindowText(m_strAddressBar);
	}

	return DoDataExchange(bSaveAndValidate);
}



//オーバーライド関数
BOOL CCloseTitlesPropertyPage::OnSetActive()
{
	SetModified(TRUE);
	return DataExchange(FALSE);
}



BOOL CCloseTitlesPropertyPage::OnKillActive()
{
	return DataExchange(TRUE);
}



BOOL CCloseTitlesPropertyPage::OnApply()
{
	if ( DataExchange(TRUE) ) {
		GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}
