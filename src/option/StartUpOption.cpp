/**
 *	@file	StartUpOption.cpp
 *	@brief	donutのオプション : スタートアップ
 */
#include "stdafx.h"
#include "StartUpOption.h"
#include "../IniFile.h"
#include "../DonutFavoritesMenu.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace MTL;

////////////////////////////////////////////////////////////////////////////////
//CStartUpOptionの定義
////////////////////////////////////////////////////////////////////////////////

DWORD CStartUpOption::s_dwFlags    = CStartUpOption::STARTUP_NOINITWIN;
DWORD CStartUpOption::s_dwParam    = CStartUpOption::STARTUP_WITH_PARAM;			// UDT DGSTR ( dai
DWORD CStartUpOption::s_dwActivate = 1; //CStartUpOption::STARTUP_ACTIVATE;				// UDT DGSTR ( dai

#if 1	//+++	Donut.cpp に置かれていたのをこちらへ移動.
TCHAR CStartUpOption::s_szDfgPath[MAX_PATH];
#endif




void CStartUpOption::GetProfile()
{
	CIniFileI	pr( g_szIniFileName, _T("StartUp") );
	pr.QueryValue( s_dwFlags, _T("StartUp_Flags") );
	pr.QueryValue( s_dwParam, _T("StartUp_With_Param") );							// UDT DGSTR ( dai
	pr.QueryValue( s_dwActivate, _T("StartUp_Activate") );							// UDT DGSTR ( dai

  #if 1	//+++
	CString dfgPath = pr.GetStringUW(_T("Favorite_Group"));
	lstrcpyn(s_szDfgPath, dfgPath, MAX_PATH/*dfgPath.GetLength()*/);
  #else
	DWORD dwCount = MAX_PATH;
	if (pr.QueryValue(s_szDfgPath, _T("Favorite_Group"), &dwCount) != ERROR_SUCCESS)
		::lstrcpy( s_szDfgPath, _T("") );
	//+++ pr.Close();
  #endif
}



void CStartUpOption::WriteProfile()
{
	CIniFileO	pr( g_szIniFileName, _T("StartUp") );
	pr.SetValue   ( s_dwFlags	   , _T("StartUp_Flags"     ) );
	pr.SetStringUW( s_szDfgPath    , _T("Favorite_Group"    ) );
	pr.SetValue   ( s_dwParam	   , _T("StartUp_With_Param") );							// UDT DGSTR ( dai
	pr.SetValue   ( s_dwActivate   , _T("StartUp_Activate"  ) );							// UDT DGSTR ( dai
}



CString CStartUpOption::GetDefaultDFGFilePath()
{
	return Misc::GetExeDirectory() + _T("Default.dfg");
}



BOOL CStartUpOption::Isflag_Latest()												//minit
{
	return s_dwFlags == STARTUP_LATEST;
}



////////////////////////////////////////////////////////////////////////////////
//CStartUpPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////

// Constructor
CStartUpPropertyPage::CStartUpPropertyPage()
	: m_nRadio(0)
	, m_strDfg()
	, m_edit()
	, m_nCheckActivate(0)
	, m_nCheckParam(0)
	, m_nCheckSaveFB(0)
{
	_SetData();
}



// Overrides
BOOL CStartUpPropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_edit.m_hWnd == NULL)
		m_edit.Attach( GetDlgItem(IDC_EDIT_DFG) );

	return DoDataExchange(FALSE);
}



BOOL CStartUpPropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CStartUpPropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



void CStartUpPropertyPage::OnButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	TCHAR			   szOldPath[MAX_PATH]; 										// save current directory
	szOldPath[0]	= 0;	//+++
	::GetCurrentDirectory(MAX_PATH, szOldPath);
	::SetCurrentDirectory( DonutGetFavoriteGroupFolder() );

	static const TCHAR szFilter[] = _T("Donut Favorite Groupファイル(*.dfg)\0*.dfg\0\0");

	CFileDialog 	   fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	fileDlg.m_ofn.lpstrTitle = _T("スタートアップお気に入りグループ");

	if (fileDlg.DoModal() == IDOK) {
		m_edit.SetWindowText(fileDlg.m_szFileName);
	}

	// restore current directory
	::SetCurrentDirectory(szOldPath);
}



// Implementation
void CStartUpPropertyPage::_GetData()
{
	CStartUpOption::s_dwParam	 = (m_nCheckParam	) == 0; 	//+++ ?  0 : 1; 	// UDT DGSTR ( dai
	CStartUpOption::s_dwActivate = (m_nCheckActivate) != 0; 	//+++ ?  1 : 0; 	// UDT DGSTR ( dai

	// update dl control flags
	CStartUpOption::s_dwFlags	 = 0;

	switch (m_nRadio) {
	case 0: 	CStartUpOption::s_dwFlags = CStartUpOption::STARTUP_NOINITWIN;	break;
	case 1: 	CStartUpOption::s_dwFlags = CStartUpOption::STARTUP_GOHOME; 	break;
	case 2: 	CStartUpOption::s_dwFlags = CStartUpOption::STARTUP_LATEST; 	break;
	case 3: 	CStartUpOption::s_dwFlags = CStartUpOption::STARTUP_DFG;		break;
	default:	ATLASSERT(FALSE);	break;
	}

	m_edit.GetWindowText(CStartUpOption::s_szDfgPath, MAX_PATH);
}



void CStartUpPropertyPage::_SetData()
{
	//+++ m_nCheckParam    = (CStartUpOption::s_dwParam    == STARTUP_WITH_PARAM) ? 0 : 1;	// UDT DGSTR ( dai
	//+++ m_nCheckActivate = (CStartUpOption::s_dwActivate == STARTUP_ACTIVATE	) ? 1 : 0;	// UDT DGSTR ( dai
	m_nCheckParam	 = (CStartUpOption::s_dwParam	 != CStartUpOption::STARTUP_WITH_PARAM);		// UDT DGSTR ( dai
	m_nCheckActivate = (CStartUpOption::s_dwActivate != 0  );		// UDT DGSTR ( dai

	if		(CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_NOINITWIN ) m_nRadio = 0;
	else if (CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_GOHOME	) m_nRadio = 1;
	else if (CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_LATEST	) m_nRadio = 2;
	else if (CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_DFG		) m_nRadio = 3;
	else	ATLASSERT(FALSE);

	m_strDfg		 = CStartUpOption::s_szDfgPath;
}
