// AddressBarPropertyPage.cpp

#include "stdafx.h"
#include "AddressBarPropertyPage.h"
#include "../DonutAddressBar.h"
#include "../DonutSearchBar.h"
#include "../IniFile.h"
#include "../MtlBase.h"
#include "../DonutDefine.h"

///////////////////////////////////////////////////////////////
// CAddressBarOption

bool CAddressBarOption::s_bNewWindow	= false;
bool CAddressBarOption::s_bAutoComplete	= false;
bool CAddressBarOption::s_bGoBtnVisible	= true;
bool CAddressBarOption::s_bNoActivate	= false;
bool CAddressBarOption::s_bLoadTypedUrls= false;
bool CAddressBarOption::s_bTextVisible	= true;

bool CAddressBarOption::s_bUseEnterCtrl	= false;
CString CAddressBarOption::s_strEnterCtrlEngine;
bool CAddressBarOption::s_bUseEnterShift= false;
CString CAddressBarOption::s_strEnterShiftEngine;
bool CAddressBarOption::s_bReplaceSpace	= false;

CString CAddressBarOption::s_strIeExePath;

// 設定を読み込む
void CAddressBarOption::GetProfile()
{
	CString 	strSection = _T("AddressBar#");
	strSection.Append(IDC_ADDRESSBAR);

	CIniFileI	pr(g_szIniFileName, strSection);
	DWORD dwFlags = pr.GetValue(_T("ExtendedStyle"), ABR_EX_DEFAULT_BITS);

	s_bAutoComplete  = _check_flag(ABR_EX_AUTOCOMPLETE,   dwFlags); 	//+++ ? 1 : 0;
	s_bNewWindow	 = _check_flag(ABR_EX_OPENNEWWIN,	  dwFlags); 	//+++ ? 1 : 0;
	s_bNoActivate	 = _check_flag(ABR_EX_NOACTIVATE,	  dwFlags); 	//+++ ? 1 : 0;
	s_bLoadTypedUrls = _check_flag(ABR_EX_LOADTYPEDURLS,  dwFlags); 	//+++ ? 1 : 0;
	s_bGoBtnVisible  = _check_flag(ABR_EX_GOBTNVISIBLE,   dwFlags); 	//+++ ? 1 : 0;

	// vvv UH vvv
	s_bTextVisible	 = _check_flag(ABR_EX_TEXTVISIBLE,	  dwFlags); 	//+++ ? 1 : 0;
	s_bUseEnterCtrl  = _check_flag(ABR_EX_ENTER_CTRL,	  dwFlags); 	//+++ ? 1 : 0;
	s_bUseEnterShift = _check_flag(ABR_EX_ENTER_SHIFT,	  dwFlags); 	//+++ ? 1 : 0;
	//minit
	s_bReplaceSpace  = _check_flag(ABR_EX_SEARCH_REPLACE, dwFlags); 	//+++ ? 1 : 0;

	pr.ChangeSectionName(_T("AddressBar"));
	s_strEnterCtrlEngine	= pr.GetString(_T("EnterCtrlEngin"));
	s_strEnterShiftEngine	= pr.GetString(_T("EnterShiftEngin"));

	s_strIeExePath	= pr.GetString(_T("IeExePath"));
}

// 設定を保存する
void CAddressBarOption::SaveProfile()
{
	// update  flags
	DWORD	dwFlags = 0;
	if (s_bAutoComplete ) { dwFlags |= ABR_EX_AUTOCOMPLETE;		}
	if (s_bNewWindow	) { dwFlags |= ABR_EX_OPENNEWWIN;		}
	if (s_bNoActivate	) { dwFlags |= ABR_EX_NOACTIVATE;		}
	if (s_bNewWindow	) { dwFlags |= ABR_EX_OPENNEWWIN;		}
	if (s_bLoadTypedUrls) { dwFlags |= ABR_EX_LOADTYPEDURLS;	}
	if (s_bGoBtnVisible ) { dwFlags |= ABR_EX_GOBTNVISIBLE;		}
	// U.H
	if (s_bTextVisible	) { dwFlags |= ABR_EX_TEXTVISIBLE; 		}
	if (s_bUseEnterCtrl ) { dwFlags |= ABR_EX_ENTER_CTRL;		}
	if (s_bUseEnterShift) { dwFlags |= ABR_EX_ENTER_SHIFT; 		}
	if (s_bReplaceSpace ) { dwFlags |= ABR_EX_SEARCH_REPLACE;	}

	CString 	strSection = _T("AddressBar#");
	strSection.Append(IDC_ADDRESSBAR);

	CIniFileO	pr(g_szIniFileName, strSection);
	pr.SetValue(dwFlags, _T("ExtendedStyle"));


	pr.ChangeSectionName(_T("AddressBar"));

	pr.SetStringUW( s_strEnterCtrlEngine , _T("EnterCtrlEngin") );
	pr.SetStringUW( s_strEnterShiftEngine, _T("EnterShiftEngin") );

	pr.SetStringUW( s_strIeExePath, _T("IeExePath")	);				//+++
}


////////////////////////////////////////////////////////////////
// CDonutAddressBarPropertyPage

// Constructor
CDonutAddressBarPropertyPage::CDonutAddressBarPropertyPage(CDonutAddressBar &adBar, CDonutSearchBar &searchBar)
	: m_AddressBar(adBar)
	, m_SearchBar(searchBar)
	, m_bInit(FALSE)
{ }


// Overrides
BOOL CDonutAddressBarPropertyPage::OnSetActive()
{
	if (m_bInit == FALSE) {
		m_bInit = TRUE;
		DoDataExchange(DDX_LOAD);
		_InitComboBox();
		m_strIeExePath = s_strIeExePath;
		m_edit.SetWindowText(s_strIeExePath);
	}

	SetModified(TRUE);
	return TRUE;
}


BOOL CDonutAddressBarPropertyPage::OnKillActive()
{
	return TRUE;
}


BOOL CDonutAddressBarPropertyPage::OnApply()
{
	if ( DoDataExchange(DDX_SAVE) ) {
		s_strIeExePath = m_strIeExePath;

		CComboBox	cmbCtrl  = GetDlgItem(IDC_COMBO_CTRL_ENTER);
		CComboBox	cmbShift = GetDlgItem(IDC_COMBO_SHIFT_ENTER);
	
		int idx;
		idx = cmbCtrl.GetCurSel();
		if (idx != CB_ERR) {
			CString 	strBufCtrl;
			cmbCtrl.GetLBText(idx, strBufCtrl);
			s_strEnterCtrlEngine = strBufCtrl;
		}

		idx = cmbShift.GetCurSel();
		if (idx != CB_ERR) {
			CString		strBufShift;
			cmbShift.GetLBText(idx, strBufShift);
			s_strEnterShiftEngine = strBufShift;
		}
		m_AddressBar.ShowGoButton(s_bGoBtnVisible);

		SaveProfile();
		return TRUE;
	} else
		return FALSE;
}


// [...]
void 	CDonutAddressBarPropertyPage::OnButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	static const TCHAR szFilter[] = _T("全ファイル(*.*)\0*.*\0\0");

	CFileDialog 	   fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	fileDlg.m_ofn.lpstrTitle = _T("アドレスバー・アイコンで起動するアプリ");
	if (fileDlg.DoModal(m_hWnd) == IDOK) {
		m_edit.SetWindowText(fileDlg.m_szFileName);
		m_strIeExePath	= fileDlg.m_szFileName;
	}
}



void	CDonutAddressBarPropertyPage::_InitComboBox()
{
	CComboBox cmbCtrl  = GetDlgItem(IDC_COMBO_CTRL_ENTER);
	CComboBox cmbShift = GetDlgItem(IDC_COMBO_SHIFT_ENTER);

	#if 0//\\+	//+++
	m_SearchBar.InitComboBox_for_AddressBarPropertyPage(cmbCtrl, cmbShift);
	#else
	CString strBuf;
	CMenu menu = m_SearchBar.GetSearchEngineMenuHandle();
	int nCount = menu.GetMenuItemCount();
	for (int i = 0; i < nCount; i++) {
		menu.GetMenuString(i, strBuf, MF_BYPOSITION);
		cmbCtrl.AddString(strBuf);
		cmbShift.AddString(strBuf);
	}
	#endif

	cmbCtrl.SelectString(0, s_strEnterCtrlEngine);
	cmbShift.SelectString(0, s_strEnterShiftEngine);
}
