/**
 *	@file	PluginDialog.cpp
 *	@brief	donutのオプション : プラグイン
 */

#include "stdafx.h"
#include "PluginDialog.h"
#include "../IniFile.h"
#include "../DonutPFunc.h"
#include "../PluginManager.h"
#include "../MtlMisc.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





// Constructor
CPluginPropertyPage::CPluginPropertyPage()
{
}



CPluginPropertyPage::~CPluginPropertyPage()
{
	m_listview.Detach();
	//memory leak bug fixed (release13)
	int nCount = m_mapPlaginInfo.GetSize();

	for (int i = 0; i < nCount; i++) {
		delete m_mapPlaginInfo.GetValueAt(i);
		m_mapPlaginInfo.GetValueAt(i) = NULL;	//+++ 念のためクリア.
	}
}



// Overrides
BOOL CPluginPropertyPage::OnSetActive()
{
	if (NULL == m_listview.m_hWnd) {
		m_listview.SubclassWindow( GetDlgItem(IDC_LIST_PLUGIN) );
		_SetData();
	}

	return DoDataExchange(FALSE);
}



BOOL CPluginPropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CPluginPropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



void CPluginPropertyPage::OnBtnSetting(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	int 	  nIndex = m_listview.GetSelectedIndex();

	if (nIndex < 0)
		return;

	CString   strFile;
	m_listview.GetItemText( nIndex, 0, strFile );

	//+++ HINSTANCE hLib	 = ::LoadLibrary( _T("Plugin\\") + strFile );
	HINSTANCE hLib	 = ::LoadLibrary( CPluginManager::PluginDir() + strFile );
	if (!hLib)
		return;

	void	  (WINAPI * __PluginSetting)(); //+++ = NULL;
	//+++ __PluginSetting 	= ( void (WINAPI *)() )GetProcAddress( hLib, _T("PluginSetting") );
	__PluginSetting 	= ( void (WINAPI *)() )GetProcAddress( hLib, "PluginSetting" );
	if (__PluginSetting)
		__PluginSetting();

	::FreeLibrary( hLib );
}



void CPluginPropertyPage::_GetData()
{
	CSimpleMap<int , CSimpleArray<CString>*>	map;

	for (int nType = PLT_TOOLBAR/*1*/; nType <= PLUGIN_TYPECNT; nType++) {
		CSimpleArray<CString>*pAry = new CSimpleArray<CString>;
		map.Add( nType, pAry );
	}

	int 	nIndex;

	for (nIndex = 0; nIndex < m_listview.GetItemCount(); nIndex++) {
		CString 	strFile;
		m_listview.GetItemText( nIndex, 0, strFile );

		if ( FALSE == m_listview.GetCheckState( nIndex ) )
			continue;

		int 	nType = int( m_listview.GetItemData( nIndex ) );

		CSimpleArray<CString>*		pAry = NULL;
		pAry = map.Lookup( nType );

		if (NULL == pAry)
			continue;

		pAry->Add( strFile );
	}

	for (nIndex = 0; nIndex < map.GetSize(); nIndex++) {
		CSimpleArray<CString>*pAry	= NULL;
		pAry = map.GetValueAt( nIndex );

		int 		nType = map.GetKeyAt( nIndex );

		CString 	strKey;
		strKey.Format( _T("Plugin%02d"), nType );

		CIniFileO	pr( g_szIniFileName, strKey );
		pr.SetValue( pAry->GetSize(), _T("Count") );

		for (int nNo = 0; nNo < pAry->GetSize(); nNo++) {
			strKey.Format(_T("%02d"), nNo);
			pr.SetString( (*pAry)[nNo], strKey );
		}

		delete pAry;
	}
}



void CPluginPropertyPage::_SetData()
{
	TCHAR		titles[] = _T("ファイル名");
	LVCOLUMN	col;

	col.mask	= LVCF_TEXT | LVCF_WIDTH;
	col.pszText = titles;
	col.cx		= 150;
	m_listview.InsertColumn(0, &col);

	// List Item
	InitListItem(Misc::GetExeDirectory() + CPluginManager::PluginDir());
	DoCheckListItem();

	::EnableWindow( GetDlgItem( IDC_BTN_SETTING ), FALSE );
}



LRESULT CPluginPropertyPage::OnListPluginSelectchange(LPNMHDR)
{
	int nIndex = m_listview.GetSelectedIndex();

	if (-1 == nIndex) {
		::EnableWindow( GetDlgItem(IDC_BTN_SETTING), FALSE );
		m_strPluginName 	= _T("");
		m_strPluginKind 	= _T("");
		m_strPluginVer		= _T("");
		m_strPluginDate 	= _T("");
		m_strPluginComment	= _T("");
		m_strAuthuorName	= _T("");
		m_strAuthuorURL 	= _T("");
		m_strAuthuorEMail	= _T("");
	} else {
		::EnableWindow( GetDlgItem(IDC_BTN_SETTING), TRUE );
		CString 	strItemSelect;
		m_listview.GetItemText( nIndex, 0, strItemSelect );

		PLUGININFO *pstPluginInfo = m_mapPlaginInfo.Lookup( strItemSelect );
		if (pstPluginInfo) {
			m_strPluginName 		= pstPluginInfo->name;
			static LPCTSTR PluginType[] = { _T("Toolbar"), _T("Explorerbar"), _T("Statusbar"), _T("Operation"), _T("Docking") };
			m_strPluginKind 		= PluginType[(pstPluginInfo->type & 0xF) - 1];
			m_strPluginVer			= pstPluginInfo->version;
			m_strPluginDate 		= pstPluginInfo->versionDate;
			m_strPluginComment		= pstPluginInfo->comment;
			m_strAuthuorName		= pstPluginInfo->authorName;
			m_strAuthuorURL 		= pstPluginInfo->authorUrl;
			m_strAuthuorEMail		= pstPluginInfo->authorEmail;
		}
	}

	DoDataExchange( FALSE );
	return S_OK;
}



void CPluginPropertyPage::InitListItem(const CString& strDirectory)
{
	MtlForEachObject_OldShell(strDirectory, [this](const CString& strPath, bool bDir) {
		if (bDir == false && Misc::GetFileExt(strPath).CompareNoCase(_T("dll")) == 0) {
			PLUGININFO *pstPluginInfo = new PLUGININFO;
			memset( pstPluginInfo, 0, sizeof (PLUGININFO) );

			HINSTANCE	hLib = ::LoadLibrary( strPath );
			if (!hLib) {
				DWORD errNum = ::GetLastError();
			}

			void		(WINAPI * __GetPluginInfo)(PLUGININFO * pstPlugin); //+++ = NULL;
			__GetPluginInfo = ( void (WINAPI *)(PLUGININFO *) )GetProcAddress(hLib, "GetPluginInfo");
			if (__GetPluginInfo) {
				__GetPluginInfo( pstPluginInfo );
				CString strPluginDir = Misc::GetExeDirectory() + CPluginManager::PluginDir();
				CString strName = strPath.Mid(strPluginDir.GetLength());
				m_mapPlaginInfo.Add( strName, pstPluginInfo );

				// List Add
				int nIndex = m_listview.InsertItem( m_listview.GetItemCount(), strName );
				m_listview.SetItemData( nIndex, pstPluginInfo->type );
			} else {
				delete pstPluginInfo;
			}

			::FreeLibrary( hLib );
		} else if (bDir && Misc::GetDirName(strPath).CompareNoCase(_T("_ignore")) != 0) {
			InitListItem(strPath);
		}

	});
}



void CPluginPropertyPage::DoCheckListItem()
{
	for (int nType = PLT_TOOLBAR/*1*/; nType <= PLUGIN_TYPECNT; nType++) {
		CString 	strKey;
		strKey.Format( _T("Plugin%02d"), /*PLT_TOOLBAR +*/ nType ); 	//+++ 余分に足しすぎ.

		CIniFileI	pr( g_szIniFileName, strKey );

		DWORD		dwCount = 0;
		pr.QueryValue( dwCount, _T("Count") );

		for (int nNo = 0; nNo < (int) dwCount; nNo++) {
			strKey.Format(_T("%02d"), nNo);
			CString szFile = pr.GetString( strKey );	//+++ 手抜きで元の名前のままCString化(ネーミングルール破壊になるが...)
			for (int nIndex = 0; nIndex < m_listview.GetItemCount(); nIndex++) {
				CString strItem;
				m_listview.GetItemText( nIndex, 0, strItem );
				if (strItem != szFile)
					continue;
				m_listview.SetCheckState( nIndex, TRUE );
				break;
			}
		}
	}
}



