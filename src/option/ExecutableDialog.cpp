/**
 *	@file	ExecuteableDialog.cpp
 *	@brief	donutのオプション : "関連づけ"
 */
#include "stdafx.h"
#include "ExecutableDialog.h"
#include "../DonutPFunc.h"
#include "../MtlMisc.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




// Ctor
CDonutExecutablePropertyPage::CDonutExecutablePropertyPage()
{
	static const TCHAR *types[] = {
		_T("file"),
		_T("ftp"),
		// _T("gopher"),
		_T("htmlfile"),
		_T("mhtmlfile"),
		_T("shtmlfile"),
		_T("http"),
		_T("https"),
		_T("InternetShortcut"),
		// _T("jsfile"),
		// _T("xslfile"),
	};

	for (int i = 0; i < _countof(types); ++i) {
		CString 	str(types[i]);
		m_arrType.Add(str);
	}
}



// Overrides
BOOL CDonutExecutablePropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_listview.m_hWnd == NULL) {
		m_listview.SubclassWindow( GetDlgItem(IDC_LIST_EXECUTABLE) );
		_SetData();
	}

	return _DoTypeDataExchange(FALSE);
}



BOOL CDonutExecutablePropertyPage::OnKillActive()
{
	return _DoTypeDataExchange(TRUE);
}



BOOL CDonutExecutablePropertyPage::OnApply()
{
	if( _CheckOsVersion_VistaLater() == TRUE )
		return TRUE; //\\ vista以降なら帰る

	if ( _DoTypeDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



// Implementation
BOOL CDonutExecutablePropertyPage::_DoTypeDataExchange(BOOL bSaveAndValidate)
{
	return TRUE;
}



void CDonutExecutablePropertyPage::_GetData()
{
	for (int i = 0; i < m_arrType.GetSize(); ++i) {
		bool	bCheck = (m_listview.GetCheckState(i) == TRUE);
		MtlSetExcutable(m_arrType[i], Misc::GetExeFileName(), _T("Donut"), bCheck);
	}
}



void CDonutExecutablePropertyPage::_SetData()
{
	if( _CheckOsVersion_VistaLater() == TRUE )
		return; //\\ vista以降なら帰る

	{	// set up column
		static const TCHAR* 	titles[] = { _T("種類"), _T("説明") };
		static const int		widths[] = { 100, 500, };

		LVCOLUMN			col;
		col.mask = LVCF_TEXT | LVCF_WIDTH;

		for (int i = 0; i < _countof(titles); ++i) {
			col.pszText = (LPTSTR) titles[i];
			col.cx		= widths[i];
			m_listview.InsertColumn(i, &col);
		}
	}

	{	// insert items
		LVITEM item;
		item.mask	  = LVIF_TEXT;
		item.iSubItem = 0;

		for (int i = 0; i < m_arrType.GetSize(); ++i) {
			item.iItem	  = i;
			item.iSubItem = 0;
			CString   str = m_arrType[i];
			item.pszText  = (LPTSTR) (LPCTSTR) str;
			m_listview.InsertItem(&item);
		}
	}

	{	// set sub text
		static const TCHAR *typeargs[] = {
			_T("URL:File プロトコル"),
			_T("URL:ファイル転送プロトコル (FTP)"),
			// _T("URL:Gopher プロトコル"),
			_T("Microsoft HTML Document 5.0"),
			_T("Microsoft MHTML Document 5.0"),
			_T("Microsoft SHTML Document 5.0"),
			_T("URL:HyperText 転送プロトコル (HTTP)"),
			_T("URL:HyperText 保護機能付き転送プロトコル (HTTP)"),
			_T("インターネットショートカット"),
			// _T("JScript ファイル"),
			// _T("XSL Stylesheet"),
		};

		LVITEM				item;

		item.mask	  = LVIF_TEXT;
		item.iSubItem = 0;

		for (int i = 0; i < _countof(typeargs); ++i) {
			item.iItem	  = i;
			item.iSubItem = 1;
			item.pszText  = (LPTSTR) typeargs[i];
			m_listview.SetItem(&item);
		}
	}

	// check it
	for (int i = 0; i < m_arrType.GetSize(); ++i) {
		BOOL	bCheck =  MtlIsExecutable( m_arrType[i], _T("Donut") ) != 0;
		m_listview.SetCheckState(i, bCheck);
	}
}
