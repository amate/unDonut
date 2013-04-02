/**
 *	@file	StartUpFinishOption.cpp
 *	@brief	unDonutのオプション : 起動/終了処理
 */
#include "stdafx.h"
#include "StartUpFinishOption.h"
#include <fstream>
#include "MainOption.h"
#include "..\IniFile.h"
#include "..\MainFrame.h"
#include "..\DonutTabList.h"


using namespace MTL;

////////////////////////////////////////////////////////////////////////////////
//CStartUpOptionの定義
////////////////////////////////////////////////////////////////////////////////

DWORD CStartUpOption::s_dwFlags    = CStartUpOption::STARTUP_NOINITWIN;
bool	CStartUpOption::s_bActivateOnExternalOpen	= true;
CString CStartUpOption::s_szDfgPath;




void CStartUpOption::GetProfile()
{
	CIniFileI	pr( g_szIniFileName, _T("StartUp") );
	pr.QueryValue( s_dwFlags, _T("StartUp_Flags") );
	s_bActivateOnExternalOpen = pr.GetValue( _T("StartUp_Activate"), s_bActivateOnExternalOpen ) != 0;

	s_szDfgPath = pr.GetStringUW(_T("Favorite_Group"));
}



void CStartUpOption::WriteProfile()
{
	CIniFileO	pr( g_szIniFileName, _T("StartUp") );
	pr.SetValue   ( s_dwFlags	   , _T("StartUp_Flags"     ) );
	pr.SetStringUW( s_szDfgPath    , _T("Favorite_Group"    ) );
	pr.SetValue   ( s_bActivateOnExternalOpen, _T("StartUp_Activate"  ) );							// UDT DGSTR ( dai
}




void CStartUpOption::StartUp(CMainFrame& __frame)
{
	class CRestoreTabListSelector : public CDialogImpl<CRestoreTabListSelector>
	{
	public:
		enum { IDD = IDD_RESTORE_TABLIST_SELECTOR };

		BEGIN_MSG_MAP_EX( CRestoreTabListSelector )
			MSG_WM_INITDIALOG( OnInitDialog )
			NOTIFY_CODE_HANDLER_EX( LVN_KEYDOWN, OnListViewKeyDown )
			COMMAND_ID_HANDLER_EX(IDCANCEL, OnCommand )
			COMMAND_ID_HANDLER_EX(IDC_BUTTON_TABLISTXML, OnCommand )
			COMMAND_ID_HANDLER_EX(IDC_BUTTON_TABLISTBAKXML, OnCommand )
		END_MSG_MAP()

		BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
		{
			CenterWindow(GetParent());

			m_bTabListChanged = false;

			m_listTabList		= GetDlgItem(IDC_LIST_TABLISTXML);
			m_listTabListBak	= GetDlgItem(IDC_LIST_TABLISTBAKXML);

			m_listTabList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
			m_listTabListBak.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

			m_listTabList.InsertColumn(0, _T("タイトル"), LVCFMT_LEFT, 170);
			m_listTabList.InsertColumn(1, _T("URL"), LVCFMT_LEFT, 400);
			m_listTabListBak.InsertColumn(0, _T("タイトル"), LVCFMT_LEFT, 170);
			m_listTabListBak.InsertColumn(1, _T("URL"), LVCFMT_LEFT, 400);

			auto funcInsertItemToListView = [](CListViewCtrl listview, const CString& TabListPath, CDonutTabList& tabList) {
				if (tabList.Load(TabListPath)) {
					int nCount = tabList.GetCount();
					for (int i = 0; i < nCount; ++i) {
						CString title = tabList.At(i)->strTitle;
						CString url = tabList.At(i)->strURL;
						int nInsertPos = listview.AddItem(listview.GetItemCount(), 0, title);
						listview.AddItem(nInsertPos, 1, url);
					}
				}
			};
			funcInsertItemToListView(m_listTabList, GetConfigFilePath(_T("TabList.donutTabList")), m_tabList);
			funcInsertItemToListView(m_listTabListBak, GetConfigFilePath(_T("TabList.bak.donutTabList")), m_tabListBak);

			return TRUE;
		}

		LRESULT OnListViewKeyDown(LPNMHDR pnmh)
		{
			auto pnkd = (LPNMLVKEYDOWN)pnmh;
			if (pnkd->wVKey == VK_DELETE) {
				CDonutTabList*	pTabList = nullptr;
				CListViewCtrl	listView;
				if (pnkd->hdr.idFrom == IDC_LIST_TABLISTXML) {
					pTabList = &m_tabList;
					listView = m_listTabList;
				} else {
					pTabList = &m_tabListBak;
					listView = m_listTabListBak;
				}
				int nIndex = listView.GetSelectedIndex();
				if (nIndex == -1)
					return 0;
				pTabList->Delete(nIndex);
				listView.DeleteItem(nIndex);
				if (listView.GetItemCount() <= nIndex)
					--nIndex;
				listView.SelectItem(nIndex);

				m_bTabListChanged = true;
			}

			return 0;
		}

		void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			if (m_bTabListChanged) {
				if (nID == IDC_BUTTON_TABLISTXML)
					m_tabList.Save(GetConfigFilePath(_T("TabList.donutTabList")), false);
				else if (nID == IDC_BUTTON_TABLISTBAKXML)
					m_tabListBak.Save(GetConfigFilePath(_T("TabList.bak.donutTabList")), false);
			}
			EndDialog(nID);
		}

	private:
		// Data members
		CListViewCtrl	m_listTabList;
		CListViewCtrl	m_listTabListBak;
		CDonutTabList	m_tabList;
		CDonutTabList	m_tabListBak;
		bool			m_bTabListChanged;
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
				break;
			}
			if (tabListXmlPath.GetLength() > 0)
				__frame.RestoreAllTab(tabListXmlPath);
		} else {
			__frame.RestoreAllTab();
		}
		break;

	case STARTUP_DFG:
		{
			if ( s_szDfgPath.GetLength() > 0 )
				__frame.UserOpenFile(s_szDfgPath, D_OPENFILE_CREATETAB);
		}
		::PostMessage(__frame.GetHWND(), WM_INITPROCESSFINISHED, 0, 0);
		break;

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


////////////////////////////////////////////////////////////////////////////////
// CStartUpFinishPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////

// Constructor
CStartUpFinishPropertyPage::CStartUpFinishPropertyPage() : 
	m_bInit(false),
	m_nRadio(0)
{
}


// Overrides

BOOL CStartUpFinishPropertyPage::OnSetActive()
{
	if (m_bInit == false) {
		// 起動処理
		if		(CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_NOINITWIN ) m_nRadio = 0;
		else if (CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_GOHOME	) m_nRadio = 1;
		else if (CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_LATEST	) m_nRadio = 2;
		else if (CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_DFG		) m_nRadio = 3;
		else	ATLASSERT(FALSE);

		// 終了処理
		m_nDelCash		  = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_CASH		) != 0;
		m_nDelCookie	  = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_COOKIE		) != 0;
		m_nDelHistory	  = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_HISTORY 	) != 0;
		m_nMakeCash 	  = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MAKECASH		) != 0;
		m_nDelRecentClose = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_RECENTCLOSE ) != 0;

		m_bInit = true;
	}

	return DoDataExchange(DDX_LOAD);
}



BOOL CStartUpFinishPropertyPage::OnKillActive()
{
	return DoDataExchange(DDX_SAVE);
}



BOOL CStartUpFinishPropertyPage::OnApply()
{
	if ( DoDataExchange(DDX_SAVE) ) {
		// 起動処理
		switch (m_nRadio) {
		case 0: 	CStartUpOption::s_dwFlags = CStartUpOption::STARTUP_NOINITWIN;	break;
		case 1: 	CStartUpOption::s_dwFlags = CStartUpOption::STARTUP_GOHOME; 	break;
		case 2: 	CStartUpOption::s_dwFlags = CStartUpOption::STARTUP_LATEST; 	break;
		case 3: 	CStartUpOption::s_dwFlags = CStartUpOption::STARTUP_DFG;		break;
		default:	ATLASSERT(FALSE);	break;
		}

		WriteProfile();

		// 終了処理
		{
			//+++	ここで使う以外のフラグはガードしとく.
			CMainOption::s_dwMainExtendedStyle2   &= ~(MAIN_EX2_DEL_CASH | MAIN_EX2_DEL_COOKIE | MAIN_EX2_DEL_HISTORY | MAIN_EX2_MAKECASH | MAIN_EX2_DEL_RECENTCLOSE);

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

			CIniFileIO	pr( g_szIniFileName, _T("Main") );
			pr.SetValue( CMainOption::s_dwMainExtendedStyle2, _T("Extended_Style2") );
		}

		return TRUE;
	} else {
		return FALSE;
	}
}



void CStartUpFinishPropertyPage::OnButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	TCHAR	szOldPath[MAX_PATH] = _T(""); 										// save current directory
	::GetCurrentDirectory(MAX_PATH, szOldPath);
	::SetCurrentDirectory( Misc::GetExeDirectory() + _T("FavoriteGroup\\") );

	static const TCHAR szFilter[] = _T("Donut Favorite Groupファイル(*.donutTabList)\0*.donutTabList\0\0");

	CFileDialog 	   fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	fileDlg.m_ofn.lpstrTitle = _T("スタートアップお気に入りグループ");

	if (fileDlg.DoModal() == IDOK) {
		CEdit edit = GetDlgItem(IDC_EDIT_DFG);
		edit.SetWindowText(fileDlg.m_szFileName);
	}

	// restore current directory
	::SetCurrentDirectory(szOldPath);
}


