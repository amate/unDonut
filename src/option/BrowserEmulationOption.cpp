/**
*	@file	BrowserEmulationOption.cpp
*	@brief	ブラウザ エミュレーションのオプション
*/

#include "stdafx.h"
#include "BrowserEmulationOption.h"
#include <fstream>
#include "..\MtlWin.h"
#include "..\MtlMisc.h"
#include "..\DonutPFunc.h"


///////////////////////////////////////////////////////////
// CBrowserEmulationOption


std::string	CBrowserEmulationOption::s_strDefaultBrowserEmulation = "edge";
std::vector<std::pair<CString, std::string>>	CBrowserEmulationOption::s_vecBrowserEmulation;	// URL, BrowserEmulationMode


void		CBrowserEmulationOption::GetProfile()
{
	try {
		CString configPath = GetConfigFilePath(_T("BrowserEmulationList.txt"));
		std::ifstream fs(configPath);
		if (!fs)
			throw std::exception();

		std::string line;
		std::getline(fs, line);
		if (line.find("DefaultBrowserEmulation\t") == std::string::npos)
			throw std::exception();
		s_strDefaultBrowserEmulation = line.substr(24);

		while (std::getline(fs, line)) {
			std::size_t tabPos = line.find('\t');
			if (tabPos == std::string::npos)
				continue;
			CString url = line.substr(0, tabPos).c_str();
			std::string mode = line.substr(tabPos + 1);
			s_vecBrowserEmulation.emplace_back(url, mode);
			if (fs.eof())
				break;
		}
	}
	catch (const std::exception& e) {
		e;
	}

}

void		CBrowserEmulationOption::WriteProfile()
{
	try {
		CString configPath = GetConfigFilePath(_T("BrowserEmulationList.txt"));
		std::ofstream fs(configPath);
		if (!fs)
			throw std::exception();

		fs << "DefaultBrowserEmulation\t" << s_strDefaultBrowserEmulation;
		for (auto& pair : s_vecBrowserEmulation) {
			fs << '\n' << (LPSTR)CT2A(pair.first) << '\t' << pair.second;
		}

	}
	catch (const std::exception& e) {
		e;
		MessageBox(NULL, _T("BrowserEmulationList.txtの書き込みに失敗しました"), NULL, MB_ICONERROR);
	}

}

std::string CBrowserEmulationOption::FindEmulationMode(const CString& URL)
{
	for (auto& pair : s_vecBrowserEmulation) {
		if (MtlStrMatchWildCard(pair.first, URL))
			return pair.second;
	}
	return s_strDefaultBrowserEmulation;
}


/////////////////////////////////////////////////////////////////////////////////
// CBrowserEmulationPropertyPage

static LPCTSTR BrowseModeList[] = {
	_T("edge"),
	_T("5"),
	_T("7"),
	_T("8"),
	_T("9"),
	_T("10"),
	_T("11"),
	_T("EmulateIE7"),
	_T("EmulateIE8"),
	_T("EmulateIE9"),
	_T("EmulateIE10"),
	_T("EmulateIE11"),
};

CBrowserEmulationPropertyPage::CBrowserEmulationPropertyPage(const CString& url) :
	m_browserURL(url),
	m_listBrowserEmulation(this, 1)
{ }

BOOL CBrowserEmulationPropertyPage::OnApply()
{
	int nSel = m_cmbDefaultBrowserEmulation.GetCurSel();
	if (nSel == 0) {
		s_strDefaultBrowserEmulation.clear();
	} else {
		CString defaultEmu;
		m_cmbDefaultBrowserEmulation.GetLBText(nSel, defaultEmu);
		s_strDefaultBrowserEmulation = CT2A(defaultEmu);
	}
	
	s_vecBrowserEmulation.clear();
	int nCount = m_listBrowserEmulation.GetItemCount();
	for (int i = 0; i < nCount; ++i) {
		CString url;
		CString mode;
		m_listBrowserEmulation.GetItemText(i, 0, url);
		m_listBrowserEmulation.GetItemText(i, 1, mode);
		s_vecBrowserEmulation.emplace_back(url, (LPCSTR)CT2A(mode));
	}

	WriteProfile();
	return TRUE;
}

BOOL CBrowserEmulationPropertyPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CEdit(GetDlgItem(IDC_EDIT_URL)).SetWindowText(m_browserURL);

	m_cmbDefaultBrowserEmulation = GetDlgItem(IDC_COMBO_DEFAULT_BROWSER_EMULATION);
	m_cmbDefaultBrowserEmulation.AddString(_T("指定しない"));
	for (LPCTSTR mode : BrowseModeList)
		m_cmbDefaultBrowserEmulation.AddString(mode);
	if (s_strDefaultBrowserEmulation.length() > 0) {
		m_cmbDefaultBrowserEmulation.SelectString(0, CString(s_strDefaultBrowserEmulation.c_str()));
	} else {
		m_cmbDefaultBrowserEmulation.SetCurSel(0);
	}

	m_listBrowserEmulation.SubclassWindow(GetDlgItem(IDC_LISTIVEW_BROWSER_EMULATION));

	m_listBrowserEmulation.InsertColumn(0, _T("URL"), LVCFMT_LEFT, 360);
	m_listBrowserEmulation.InsertColumn(1, _T("動作モード"), LVCFMT_LEFT, 90);
	m_listBrowserEmulation.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	int nCount = 0;
	for (auto& pair : s_vecBrowserEmulation) {
		m_listBrowserEmulation.InsertItem(nCount, pair.first);
		m_listBrowserEmulation.SetItemText(nCount, 1, CA2T(pair.second.c_str()));
		++nCount;
	}

	m_cmbFloatBrowseMode.Create(m_listBrowserEmulation, CRect(0, 0, 80, 400), nullptr, WS_CHILD | CBS_DROPDOWNLIST);
	m_cmbFloatBrowseMode.SetDlgCtrlID(IDC_FLOATCOMBOBOX);
	m_cmbFloatBrowseMode.SetFont(m_cmbDefaultBrowserEmulation.GetFont());
	for (LPCTSTR mode : BrowseModeList)
		m_cmbFloatBrowseMode.AddString(mode);

	return 0;
}



void CBrowserEmulationPropertyPage::OnAddURL(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString url = MtlGetWindowText(GetDlgItem(IDC_EDIT_URL));
	if (url.IsEmpty())
		return ;

	int nInsertedIndex = m_listBrowserEmulation.InsertItem(m_listBrowserEmulation.GetItemCount(), url);
	m_listBrowserEmulation.SetItemText(nInsertedIndex, 1, _T("edge"));
	m_listBrowserEmulation.EnsureVisible(nInsertedIndex, FALSE);
	m_listBrowserEmulation.SelectItem(nInsertedIndex);	

}


LRESULT CBrowserEmulationPropertyPage::OnListLabelEditEnd(LPNMHDR pnmh)
{
	auto pdi = (LPNMLVDISPINFOW)pnmh;
	if (pdi->item.pszText == nullptr || pdi->item.pszText[0] == L'\0')
		return 0;
	m_listBrowserEmulation.SetItemText(pdi->item.iItem, 0, pdi->item.pszText);
	return 0;
}


LRESULT CBrowserEmulationPropertyPage::OnListItemDblClk(LPNMHDR pnmh)
{
	auto lpnmitem = (LPNMITEMACTIVATE)pnmh;
	if (lpnmitem->iItem == -1 || lpnmitem->iSubItem != 1)
		return 0;

	CString strEmu;
	m_listBrowserEmulation.GetItemText(lpnmitem->iItem, 1, strEmu);
	if (strEmu.IsEmpty()) {
		m_cmbFloatBrowseMode.SetCurSel(0);
	} else {
		m_cmbFloatBrowseMode.SelectString(0, strEmu);
	}
	CRect rcItem;
	m_listBrowserEmulation.GetSubItemRect(lpnmitem->iItem, 1, LVIR_BOUNDS, &rcItem);
	rcItem.top -= 2;
	rcItem.left += 2;
	m_cmbFloatBrowseMode.MoveWindow(rcItem);
	m_cmbFloatBrowseMode.ShowWindow(TRUE);
	m_cmbFloatBrowseMode.ShowDropDown();

	return 0;
}

LRESULT CBrowserEmulationPropertyPage::OnListKeyDown(LPNMHDR pnmh)
{
	auto pnkd = (LPNMLVKEYDOWN)pnmh;
	if (pnkd->wVKey == VK_DELETE) {
		int nSel = m_listBrowserEmulation.GetSelectedIndex();
		if (nSel == -1)
			return 0;
		m_listBrowserEmulation.DeleteItem(nSel);
		if (m_listBrowserEmulation.SelectItem(nSel) == FALSE)
			m_listBrowserEmulation.SelectItem(nSel - 1);
	}
	return 0;
}


void CBrowserEmulationPropertyPage::OnFloatComboSelChange(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nSel = m_cmbFloatBrowseMode.GetCurSel();
	CString strEmu;
	m_cmbFloatBrowseMode.GetLBText(nSel, strEmu);
	m_listBrowserEmulation.SetItemText(m_listBrowserEmulation.GetSelectedIndex(), 1, strEmu);
}


void CBrowserEmulationPropertyPage::OnFloatComboCloseUp(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_cmbFloatBrowseMode.ShowWindow(FALSE);
}

