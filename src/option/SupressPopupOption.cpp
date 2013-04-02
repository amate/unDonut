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


void CSupressPopupOption::ReCreateSupressPopupDataAndNotify()
{
	s_sharedMem.CloseHandle();

	CString sharedMemName;
	sharedMemName.Format(_T("%s%#x"), IGNOREURLLISTSHAREDMEMNAME, s_hWndMainFrame);
	s_sharedMem.Serialize(s_PopupBlockData, sharedMemName);

	NotifyUpdateToChildFrame();
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
//CSurpressPopupPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////

// Constructor
CSurpressPopupPropertyPage::CSurpressPopupPropertyPage(const CString& url, const CString& title)
	: m_bInit(false)
	, m_strURL(url)
	, m_strTitle(title)
	, m_wndURLList(this, 1)
	, m_wndTitleList(this, 2)
{
}



// Overrides
BOOL CSurpressPopupPropertyPage::OnSetActive()
{
	SetModified(TRUE);
	if (m_bInit == false){
		s_PopupBlockData.IgnoreURLList.sort();
		m_bIgnoreURLValid = s_PopupBlockData.bValidIgnoreURL != 0;

		s_PopupBlockData.CloseTitleList.sort();
		m_bTitleCloseValid = s_PopupBlockData.bValidCloseTitle != 0;

		DoDataExchange(DDX_LOAD);

		auto funcAddListBox = [](CListBox box, const std::list<CString>& list) {
			box.ResetContent();
			for (auto& str : list) {
				box.AddString(str);
			}
		};
		funcAddListBox(m_listboxIgnoreURLs, s_PopupBlockData.IgnoreURLList);
		funcAddListBox(m_listboxCloseTitle, s_PopupBlockData.CloseTitleList);

		m_bInit = true;
	}
	return TRUE;
}



BOOL CSurpressPopupPropertyPage::OnKillActive()
{
	return TRUE;
}



BOOL CSurpressPopupPropertyPage::OnApply()
{
	if ( DoDataExchange(DDX_SAVE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}




LRESULT CSurpressPopupPropertyPage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_wndURLList.SubclassWindow( GetDlgItem(IDC_IGNORED_URL_LIST) );
	m_wndTitleList.SubclassWindow( GetDlgItem(IDC_CLOSE_TITLE_LIST) );
	return 0;
}



LRESULT CSurpressPopupPropertyPage::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_wndURLList.UnsubclassWindow();
	m_wndTitleList.UnsubclassWindow();
	return 0;
}

std::tuple<CListBox, CEdit, std::list<CString>&> CSurpressPopupPropertyPage::_GetListFromID(int ID)
{
	switch (ID) {
	case IDC_IGNORED_URL_LIST:
	case IDC_ADD_BUTTON:
	case IDC_APPLY:
	case IDC_DEL_BUTTON:
	case IDC_DELALL_BUTTON:
		return std::make_tuple(m_listboxIgnoreURLs, CEdit(GetDlgItem(IDC_URL_EDIT)), s_PopupBlockData.IgnoreURLList);

	case IDC_CLOSE_TITLE_LIST:
	case IDC_ADD_BUTTON2:
	case IDC_APPLY2:
	case IDC_DEL_BUTTON2:
	case IDC_DELALL_BUTTON2:
		return std::make_tuple(m_listboxCloseTitle, CEdit(GetDlgItem(IDC_TITLE_EDIT)), s_PopupBlockData.CloseTitleList);

	default:
		ATLASSERT( FALSE );
	}
	static std::list<CString> list;
	return std::make_tuple(CListBox(), CEdit(), list);
}

void CSurpressPopupPropertyPage::OnDelCmd(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	//とりあえず選択項目の番号をリストにしてソート
	auto tupleList = _GetListFromID(wID);
	std::list<int> sellist;
	int nSelCount = std::get<0>(tupleList).GetSelCount();
	std::unique_ptr<int[]>	pIdx(new int[nSelCount]);
	if (std::get<0>(tupleList).GetSelItems(nSelCount, pIdx.get()) != LB_ERR) {
		for (int i = 0; i < nSelCount; i++)
			sellist.push_back(pIdx[i]);

		sellist.sort();
		sellist.reverse();

		for (int index : sellist)
			std::get<0>(tupleList).DeleteString(index);

		if (std::get<0>(tupleList).SetSel(pIdx[0]) == -1)
			std::get<0>(tupleList).SetSel(pIdx[0] - 1);
	}
}

void	CSurpressPopupPropertyPage::OnApply(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	auto tupleList = _GetListFromID(wID);
	int nSelCount = std::get<0>(tupleList).GetSelCount();
	if (nSelCount == 0)
		return ;
	unique_ptr<int[]> pIdx(new int[nSelCount]);
	std::get<0>(tupleList).GetSelItems(nSelCount, pIdx.get());

	std::get<0>(tupleList).DeleteString(pIdx[0]);
	CString str = MtlGetWindowText(std::get<1>(tupleList));
	std::get<0>(tupleList).InsertString(pIdx[0], str);
	for (int i = 0; i < nSelCount; ++i) {
		std::get<0>(tupleList).SetSel(pIdx[i], FALSE);
	}
	std::get<0>(tupleList).SetSel(pIdx[0], TRUE);
}


void CSurpressPopupPropertyPage::OnDelAllCmd(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	auto tupleList = _GetListFromID(wID);
	std::get<0>(tupleList).ResetContent();
}



void CSurpressPopupPropertyPage::OnAddCmd(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	auto tupleList = _GetListFromID(wID);
	CString str = MtlGetWindowText(std::get<1>(tupleList));
	if (str.IsEmpty())
		return;

	int   nIndex = std::get<0>(tupleList).FindStringExact(-1, str);
	if (nIndex == LB_ERR) {
		std::get<0>(tupleList).AddString(str);
	} else {
		//m_listbox.SetCurSel(nIndex);
		std::get<0>(tupleList).SetSel(nIndex, TRUE);
	}
}



void CSurpressPopupPropertyPage::OnSelChange(UINT code, int id, HWND hWnd)
{
	auto tupleList = _GetListFromID(id);
	int nIndex = 0;
	if (std::get<0>(tupleList).GetSelItems(1, &nIndex) == 0)
		return;

	CString 	strBox;
	std::get<0>(tupleList).GetText(nIndex, strBox);
	std::get<1>(tupleList).SetWindowText(strBox);
}



void CSurpressPopupPropertyPage::OnURLListKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DELETE)
		OnDelCmd(0, IDC_DEL_BUTTON, NULL);
}

void CSurpressPopupPropertyPage::OnTitleListKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DELETE)
		OnDelCmd(0, IDC_DEL_BUTTON2, NULL);
}

// 設定を保存する
void CSurpressPopupPropertyPage::_GetData()
{
	auto funcAddListFromListBox = [](CListBox box, std::list<CString>& list) {
		list.clear();
		int nCount = box.GetCount();
		for (int i = 0; i < nCount; ++i) {
			CString text;
			box.GetText(i, text);
			list.push_back(text);
		}
	};
	funcAddListFromListBox(m_listboxIgnoreURLs, s_PopupBlockData.IgnoreURLList);
	funcAddListFromListBox(m_listboxCloseTitle, s_PopupBlockData.CloseTitleList);

	s_PopupBlockData.bValidIgnoreURL	= m_bIgnoreURLValid;
	s_PopupBlockData.bValidCloseTitle	= m_bTitleCloseValid;

	WriteProfile(true);
	WriteProfile(false);
	CreateSupressPopupData(s_hWndMainFrame);
	NotifyUpdateToChildFrame();
}



