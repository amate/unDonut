/**
 *	@file	MenuOption.cpp
 *	@brief	donutのオプション : メニュー
 */

#include "stdafx.h"
#include "MenuDialog.h"
#include "MainOption.h"
#include "RightClickMenuDialog.h"
#include "..\IniFile.h"
#include "..\DonutPFunc.h"
#include "..\ToolTipManager.h"
#include "..\ItemIDList.h"
#include "..\PopupMenu.h"

using namespace MTL;

////////////////////////////////////////////////////////////
// CMenuOption

bool	CMenuOption::s_bNoCustomMenu	= false;
bool	CMenuOption::s_bNoCustomIEMenu	= true;
bool	CMenuOption::s_bR_Equal_L		= false;

// iniから設定を読み込む
void	CMenuOption::GetProfile()
{
	{
		CIniFileI	pr(g_szIniFileName, _T("Menu"));
		DWORD dwExStyle = pr.GetValue(_T("ExStyle"), 0);
		s_bNoCustomMenu		= (dwExStyle & MENU_EX_NOCUSTOMMENU		) != 0;
		s_bNoCustomIEMenu	= (dwExStyle & MENU_EX_NOCUSTOMIEMENU	) != 0;
		s_bR_Equal_L		= (dwExStyle & MENU_EX_R_EQUAL_L		) != 0;
	}
}

// iniへ設定を保存する
void	CMenuOption::WriteProfile()
{
	{
		DWORD	dwExStyle = 0;
		if (s_bNoCustomMenu)
			dwExStyle |= MENU_EX_NOCUSTOMMENU;
		if (s_bNoCustomIEMenu)
			dwExStyle |= MENU_EX_NOCUSTOMIEMENU;
		if (s_bR_Equal_L)
			dwExStyle |= MENU_EX_R_EQUAL_L;

		CIniFileO	pr(g_szIniFileName, _T("Menu"));
		pr.SetValue(dwExStyle		, _T("ExStyle"));
		
	}
}



////////////////////////////////////////////////////////////////////////
// CFavoritesMenuOption

bool	CFavoritesMenuOption::s_bPackItem	= false;


// iniから設定を読み込む
void CFavoritesMenuOption::GetProfile()
{
	CIniFileI	pr( g_szIniFileName, _T("FavoritesMenu") );
	s_bPackItem = pr.GetValue(_T("PackItem"), s_bPackItem) != 0;
}

// iniへ設定を保存する
void CFavoritesMenuOption::WriteProfile()
{
	CIniFileIO	pr( g_szIniFileName, _T("FavoritesMenu") );
	pr.SetValue(s_bPackItem, _T("PackItem"));
}


////////////////////////////////////////////////////////////
// CMenuPropertyPage

// Constructor
CMenuPropertyPage::CMenuPropertyPage()
	: m_bInit(false)
{
}


// Destructor
CMenuPropertyPage::~CMenuPropertyPage()
{
}



// Overrides
BOOL CMenuPropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_bInit == false) {
		m_bInit = true;
	}

	return DoDataExchange(DDX_LOAD);
}



BOOL CMenuPropertyPage::OnKillActive()
{
	return TRUE;
}



BOOL CMenuPropertyPage::OnApply()
{
	if ( DoDataExchange(DDX_SAVE) ) {
		// データを保存
		CMenuOption::WriteProfile();

		CFavoritesMenuOption::WriteProfile();

		return TRUE;
	} else {
		return FALSE;
	}
}


namespace {

CString BrowseForFolder(HWND hwnd)
{
	TCHAR	szDisplayName[MAX_PATH+1] = _T("\0");
	BROWSEINFO	bi = {
		hwnd, NULL, szDisplayName, _T("フォルダ選択"),
		BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE, 
		NULL, 0,				0
	};

	CItemIDList idl;
	idl.Attach( ::SHBrowseForFolder(&bi) );
	return idl.GetPath();
}

};	// namespace


/// リンクをフォルダからインポートする
void CMenuPropertyPage::OnLinkImportFromFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (MessageBox(_T("インポートすると現在のリンクはすべて削除されますが、よろしいですか？"), _T("確認"), MB_YESNO) == IDNO)
		return ;
	CWaitCursor	waitCursor;
	CString folder = BrowseForFolder(m_hWnd);
	if (folder.GetLength() > 0) {
		CRootFavoritePopupMenu::LinkImportFromFolder(folder);
	}
}

/// リンクをフォルダへエクスポートする
void CMenuPropertyPage::OnLinkExportToFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	bool bOverWrite = false;
	switch (MessageBox(_T("フォルダにリンクがすでにある時、リンクを上書きしますか？"), _T("確認"), MB_YESNOCANCEL)) {
	case IDYES:	bOverWrite = true;	break;
	case IDNO:	bOverWrite = false;	break;
	case IDCANCEL:
		return ;
	}
	CWaitCursor	waitCursor;
	CString folder = BrowseForFolder(m_hWnd);
	if (folder.GetLength() > 0) {
		CRootFavoritePopupMenu::LinkExportToFolder(folder, bOverWrite);
	}
}


