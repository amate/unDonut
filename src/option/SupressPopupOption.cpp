/**
 *	@file	SupressPopupOption.cpp
 *	@brief	donutのオプション : ポップアップ/タイトル抑止.
 */

#include "stdafx.h"
#include "SupressPopupOption.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/list.hpp>
#include "MainOption.h"
#include "../CustomSerializeClass.h"
#include "../IniFile.h"
#include "../DonutPFunc.h"
#include "../MtlMisc.h"
#include "../DonutDefine.h"

using namespace MTL;


////////////////////////////////////////////////////////////////////////////////
//CIgnoredURLsOptionの定義
////////////////////////////////////////////////////////////////////////////////

#define IGNOREURLLISTSHAREDMEMNAME	_T("DonutSupressPopupDataSharedMemName")

PopupBlockData	CSupressPopupOption::s_PopupBlockData;
HWND			CSupressPopupOption::s_hWndMainFrame = NULL;
CSharedMemory	CSupressPopupOption::s_sharedMem;


void CSupressPopupOption::GetProfile()
{
	{
		CIniFileI	pr( g_szIniFileName, _T("IgnoredURLs") );
		s_PopupBlockData.bValidIgnoreURL = pr.GetValue(_T("IsValid")) != 0;

		FileReadString(GetConfigFilePath(_T("CloseURL.ini")), s_PopupBlockData.IgnoreURLList);
	}
	{
		CIniFileI	pr( g_szIniFileName, _T("CloseTitles") );
		s_PopupBlockData.bValidCloseTitle	= pr.GetValue(_T("IsValid")) != 0;

		FileReadString(GetConfigFilePath( _T("CloseTitle.ini") ), s_PopupBlockData.CloseTitleList);
	}
}



void CSupressPopupOption::WriteProfile(bool bIgnoreURLPropertyPage)
{
	if (bIgnoreURLPropertyPage) {
		CIniFileO	 pr( g_szIniFileName, _T("IgnoredURLs") );
		pr.DeleteSection();

		pr.SetValue( s_PopupBlockData.bValidIgnoreURL != 0, _T("IsValid") );

		FileWriteString(GetConfigFilePath(_T("CloseURL.ini")), s_PopupBlockData.IgnoreURLList);

	} else {
		CIniFileO	 pr( g_szIniFileName, _T("CloseTitles") );
		pr.DeleteSection();

		pr.SetValue( s_PopupBlockData.bValidCloseTitle != 0, _T("IsValid") );

		FileWriteString(GetConfigFilePath(_T("CloseTitle.ini")), s_PopupBlockData.CloseTitleList);
	}
}

void	CSupressPopupOption::NotifyUpdateToChildFrame() 
{
	if (CMainOption::s_BrowserOperatingMode == BROWSEROPERATINGMODE::kMultiProcessMode)
		PostMessage(s_hWndMainFrame, WM_UPDATESUPRESSPOPUPDATA, 0, 0);

}

// for MainFrame
void CSupressPopupOption::CreateSupressPopupData(HWND hWndMainFrame)
{
	s_hWndMainFrame = hWndMainFrame;

	s_sharedMem.CloseHandle();
	s_PopupBlockData.IgnoreURLList.clear();
	s_PopupBlockData.CloseTitleList.clear();

	GetProfile();

	CString sharedMemName;
	sharedMemName.Format(_T("%s%#x"), IGNOREURLLISTSHAREDMEMNAME, hWndMainFrame);
	s_sharedMem.Serialize(s_PopupBlockData, sharedMemName);
}


// for ChildFrame
void CSupressPopupOption::UpdateSupressPopupData(HWND hWndMainFrame)
{
	CString sharedMemName;
	sharedMemName.Format(_T("%s%#x"), IGNOREURLLISTSHAREDMEMNAME, hWndMainFrame);
	CSharedMemory sharedMem;
	sharedMem.Deserialize(s_PopupBlockData, sharedMemName);
}


bool CSupressPopupOption::SearchURLString(const CString &strURL)
{
	if (s_PopupBlockData.bValidIgnoreURL == false)
		return false;
	return MtlSearchStringWildCard(s_PopupBlockData.IgnoreURLList.begin(), s_PopupBlockData.IgnoreURLList.end(), strURL);
}

bool CSupressPopupOption::SearchTitleString(const CString &strTitle)
{
	if (s_PopupBlockData.bValidCloseTitle == false)
		return false;
	return MtlSearchStringWildCard(s_PopupBlockData.CloseTitleList.begin(), s_PopupBlockData.CloseTitleList.end(), strTitle);
}

// for MainFrame
void CSupressPopupOption::AddIgnoreURL(const CString &strURL)
{
	if (SearchURLString(strURL) == false) {
		s_PopupBlockData.IgnoreURLList.push_back(strURL);
		WriteProfile(true);

		CreateSupressPopupData(s_hWndMainFrame);
		NotifyUpdateToChildFrame();
	}
}

void CSupressPopupOption::AddCloseTitle(const CString &strTitle)
{
	if (SearchTitleString(strTitle) == false) {
		s_PopupBlockData.CloseTitleList.push_back(strTitle);
		WriteProfile(false);

		CreateSupressPopupData(s_hWndMainFrame);
		NotifyUpdateToChildFrame();
	}
}



////////////////////////////////////////////////////////////////////////////////
//CIgnoredURLsPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////

// Constructor
CIgnoredURLsPropertyPage::CIgnoredURLsPropertyPage(const CString &strAddressBar)
	: m_bInit(false)
	, m_strAddressBar(strAddressBar)
	, m_wndList(this, 1)
{
	s_PopupBlockData.IgnoreURLList.sort();
	m_nValid = s_PopupBlockData.bValidIgnoreURL != 0;		//+++ ? 1 : 0;
}



// Overrides
BOOL CIgnoredURLsPropertyPage::OnSetActive()
{
	SetModified(TRUE);
	if (m_bInit == false){
		_DoDataExchange(FALSE);
		m_bInit = true;
	}
	return TRUE;
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
		std::for_each( s_PopupBlockData.IgnoreURLList.begin(), s_PopupBlockData.IgnoreURLList.end(), _AddToListBox(m_listbox) );

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
	int *		   pIdx 	 = new int[nSelCount];

	if (m_listbox.GetSelItems(nSelCount, pIdx) != LB_ERR) {
		for (int i = 0; i < nSelCount; i++)
			sellist.push_back(pIdx[i]);

		sellist.sort();
		sellist.reverse();

		for (std::list<int>::iterator it = sellist.begin(); it != sellist.end(); ++it)
			m_listbox.DeleteString(*it);

		if (m_listbox.SetSel(pIdx[0]) == -1)
			m_listbox.SetSel(pIdx[0] - 1);
	}

	delete[] pIdx;
}

void	CIgnoredURLsPropertyPage::OnApply(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	int nSelCount = m_listbox.GetSelCount();
	if (nSelCount == 0)
		return ;
	unique_ptr<int[]> pIdx(new int[nSelCount]);
	m_listbox.GetSelItems(nSelCount, pIdx.get());

	m_listbox.DeleteString(pIdx[0]);
	CString str = MtlGetWindowText(m_edit);
	m_listbox.InsertString(pIdx[0], str);
	for (int i = 0; i < nSelCount; ++i) {
		m_listbox.SetSel(pIdx[i], FALSE);
	}
	m_listbox.SetSel(pIdx[0], TRUE);
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

	s_PopupBlockData.IgnoreURLList.clear();

	for (int i = 0; i < nCount; i++) {
		m_listbox.GetText(i, strbuf);
		s_PopupBlockData.IgnoreURLList.push_back(strbuf);
	}

	s_PopupBlockData.bValidIgnoreURL = m_nValid != 0;

	WriteProfile(true);
	CreateSupressPopupData(s_hWndMainFrame);
	NotifyUpdateToChildFrame();
}





////////////////////////////////////////////////////////////////////////////////
//CCloseTitlesPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////

//コンストラクタ
CCloseTitlesPropertyPage::CCloseTitlesPropertyPage(const CString &strAddressBar)
	: m_bInit(false)
	, m_strAddressBar(strAddressBar)
	, m_wndList(this, 1)
{
	s_PopupBlockData.CloseTitleList.sort();
	m_nValid = s_PopupBlockData.bValidCloseTitle != 0;
}



//オーバーライド関数
BOOL CCloseTitlesPropertyPage::OnSetActive()
{
	SetModified(TRUE);
	if (m_bInit == false) {
		DataExchange(FALSE);
		m_bInit = true;
	}
	return TRUE;
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


void	CCloseTitlesPropertyPage::OnApply(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	int nSelCount = m_listbox.GetSelCount();
	if (nSelCount == 0)
		return ;
	unique_ptr<int[]> pIdx(new int[nSelCount]);
	m_listbox.GetSelItems(nSelCount, pIdx.get());

	m_listbox.DeleteString(pIdx[0]);
	CString str = MtlGetWindowText(m_edit);
	m_listbox.InsertString(pIdx[0], str);
	for (int i = 0; i < nSelCount; ++i) {
		m_listbox.SetSel(pIdx[i], FALSE);
	}
	m_listbox.SetSel(pIdx[0], TRUE);
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

	s_PopupBlockData.CloseTitleList.clear();

	for (int i = 0; i < nCount; i++) {
		m_listbox.GetText(i, strbuf);
		s_PopupBlockData.CloseTitleList.push_back(strbuf);
	}

	s_PopupBlockData.bValidCloseTitle	= m_nValid != 0;

	WriteProfile(false);
	CreateSupressPopupData(s_hWndMainFrame);
	NotifyUpdateToChildFrame();
}



BOOL CCloseTitlesPropertyPage::DataExchange(BOOL bSaveAndValidate)	// get data from controls?
{
	if (!bSaveAndValidate) {										// set data of control
		if (!m_listbox.m_hWnd)
			m_listbox.Attach( GetDlgItem(IDC_IGNORED_URL_LIST) );

		m_listbox.ResetContent();
		std::for_each( s_PopupBlockData.CloseTitleList.begin(), s_PopupBlockData.CloseTitleList.end(), AddToListBox(m_listbox) );

		if (!m_edit.m_hWnd)
			m_edit.Attach( GetDlgItem(IDC_URL_EDIT) );

		m_edit.SetWindowText(m_strAddressBar);
	}

	return DoDataExchange(bSaveAndValidate);
}

