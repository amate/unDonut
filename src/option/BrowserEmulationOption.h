/**
*	@file	BrowserEmulationOption.h
*	@brief	ブラウザ エミュレーションのオプション
*/

#pragma once

#include "resource.h"
#include <vector>
#include <string>

///////////////////////////////////////////////////////////
// CBrowserEmulationOption

class CBrowserEmulationOption
{
public:
	static void		GetProfile();
	static void		WriteProfile();

	static std::string FindEmulationMode(const CString& URL);

protected:
	// Data members
	static std::string	s_strDefaultBrowserEmulation;
	static std::vector<std::pair<CString, std::string>>	s_vecBrowserEmulation;	// URL, BrowserEmulationMode

};

////////////////////////////////////////////////////////////
// CBrowserEmulationPropertyPage

class CBrowserEmulationPropertyPage :
	public CPropertyPageImpl<CBrowserEmulationPropertyPage>,
	protected CBrowserEmulationOption
{
public:
	enum { 
		IDD = IDD_PROPPAGE_BROWSER_EMULATION,
		IDC_FLOATCOMBOBOX = 5000,
	};

	CBrowserEmulationPropertyPage(const CString& url);

	// Overrides
	//BOOL	OnSetActive();
	//BOOL	OnKillActive();
	BOOL	OnApply();

	// Message map
	BEGIN_MSG_MAP(CBrowserEmulationPropertyPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_ADDURL, OnAddURL )
		NOTIFY_CODE_HANDLER_EX(LVN_ENDLABELEDIT, OnListLabelEditEnd)
		NOTIFY_CODE_HANDLER_EX(LVN_KEYDOWN	, OnListKeyDown )
		NOTIFY_CODE_HANDLER_EX(NM_DBLCLK, OnListItemDblClk)
		CHAIN_MSG_MAP(CPropertyPageImpl<CBrowserEmulationPropertyPage>)
	ALT_MSG_MAP(1)
		COMMAND_HANDLER_EX(IDC_FLOATCOMBOBOX, CBN_SELCHANGE, OnFloatComboSelChange)
		COMMAND_HANDLER_EX(IDC_FLOATCOMBOBOX, CBN_CLOSEUP, OnFloatComboCloseUp )
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnAddURL(UINT uNotifyCode, int nID, CWindow wndCtl);

	LRESULT OnListLabelEditEnd(LPNMHDR pnmh);
	LRESULT OnListKeyDown(LPNMHDR pnmh);
	LRESULT OnListItemDblClk(LPNMHDR pnmh);
	LRESULT OnComboSelChange(LPNMHDR pnmh);
	void OnFloatComboSelChange(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnFloatComboCloseUp(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	// Data members
	CString		m_browserURL;
	CComboBox		m_cmbDefaultBrowserEmulation;
	CContainedWindowT<CListViewCtrl>	m_listBrowserEmulation;
	CComboBox		m_cmbFloatBrowseMode;

};