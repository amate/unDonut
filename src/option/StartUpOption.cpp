/**
 *	@file	StartUpOption.cpp
 *	@brief	donutのオプション : スタートアップ
 */
#include "stdafx.h"
#include "StartUpOption.h"
#include <fstream>
#include <clocale>
#include <codecvt>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\xml_parser.hpp>
#include "../IniFile.h"
#include "../DonutFavoritesMenu.h"
#include "../MainFrame.h"

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




void CStartUpOption::StartUp(CMainFrame& __frame)
{
	class CRestoreTabListSelector : public CDialogImpl<CRestoreTabListSelector>
	{
	public:
		enum { IDD = IDD_RESTORE_TABLIST_SELECTOR };

		BEGIN_MSG_MAP_EX( CRestoreTabListSelector )
			MSG_WM_INITDIALOG( OnInitDialog )
			COMMAND_ID_HANDLER_EX(IDC_BUTTON_TABLISTXML, OnCommand )
			COMMAND_ID_HANDLER_EX(IDC_BUTTON_TABLISTBAKXML, OnCommand )
		END_MSG_MAP()

		BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
		{
			CenterWindow(GetParent());

			m_listTabList		= GetDlgItem(IDC_LIST_TABLISTXML);
			m_listTabListBak	= GetDlgItem(IDC_LIST_TABLISTBAKXML);

			m_listTabList.InsertColumn(0, _T("タイトル"), LVCFMT_LEFT, 100);
			m_listTabList.InsertColumn(1, _T("URL"), LVCFMT_LEFT, 200);
			m_listTabListBak.InsertColumn(0, _T("タイトル"), LVCFMT_LEFT, 100);
			m_listTabListBak.InsertColumn(1, _T("URL"), LVCFMT_LEFT, 200);

			auto funcInsertItemToListView = [](CListViewCtrl listview, const CString& TabList) {
				try {
					using boost::property_tree::wptree;

					std::wifstream	filestream(TabList);
					if (!filestream) {
						return ;
					}
					filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));

					wptree	pt;
					boost::property_tree::read_xml(filestream, pt);

					wptree&	ptChild = pt.get_child(L"TabList");
					auto it = ptChild.begin();
					++it;
					for (; it != ptChild.end(); ++it) {
						wptree& ptItem = it->second;
						CString title = ptItem.get(L"<xmlattr>.title", L"").c_str();
						CString url = ptItem.get(L"<xmlattr>.url", L"").c_str();
						listview.InsertItem(0, title);

						LVITEM	Item = { 0 };
						Item.mask		= LVIF_TEXT;
						Item.iSubItem	= 1;
						Item.pszText	= (LPWSTR)(LPCTSTR)url;
						listview.SetItem(&Item);
					}
				} catch (...) {
					return ;
				}
			};
			funcInsertItemToListView(m_listTabList, GetConfigFilePath(_T("TabList.donutTabList")));
			funcInsertItemToListView(m_listTabListBak, GetConfigFilePath(_T("TabList.bak.donutTabList")));

			return TRUE;
		}

		void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			EndDialog(nID);
		}

	private:
		// Data members
		CListViewCtrl	m_listTabList;
		CListViewCtrl	m_listTabListBak;
	};

	switch (s_dwFlags) {
	case STARTUP_NOINITWIN:
		::PostMessage(__frame.GetHWND(), WM_INITPROCESSFINISHED, 0, 0);
		break;

	case STARTUP_GOHOME:
		::PostMessage(__frame.GetHWND(), WM_COMMAND, ID_FILE_NEW_HOME, 0);
		::PostMessage(__frame.GetHWND(), WM_INITPROCESSFINISHED, true, 0);
		break;

	case STARTUP_LATEST:
		if (::PathFileExists(GetConfigFilePath(_T("#lock")))) {
			CRestoreTabListSelector	dlg;
			CString tabListXmlPath;
			switch (dlg.DoModal(__frame.GetHWND())) {
			case IDC_BUTTON_TABLISTXML:		tabListXmlPath = GetConfigFilePath(_T("TabList.donutTabList"));	break;
			case IDC_BUTTON_TABLISTBAKXML:	tabListXmlPath = GetConfigFilePath(_T("TabList.bak.donutTabList"));	break;
			default:
				ATLASSERT( FALSE );
			}
			__frame.RestoreAllTab(tabListXmlPath);
		} else {
			__frame.RestoreAllTab();
		}
		break;

	//case STARTUP_DFG:
	//	{
	//		CString 	strPath(s_szDfgPath);
	//		if ( !strPath.IsEmpty() )
	//			__frame.UserOpenFile(s_szDfgPath, 0);
	//	}
	//	__frame.PostMessage(WM_INITPROCESSFINISHED);
	//	break;

	default:
		ATLASSERT(FALSE);
		break;
	}
	std::ofstream fs(GetConfigFilePath(_T("#lock")));
}



void	CStartUpOption::EndFinish()
{
	::DeleteFile(GetConfigFilePath(_T("#lock")));
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
