/**
 *	@file	FavoriteMenuDialog.cpp
 *	@brief	donutのオプション : "お気に入りメニュー"
 */

#include "stdafx.h"
#include "FavoriteMenuDialog.h"
#include "..\PopupMenu.h"

////////////////////////////////////////////////////////////////////////
// CFavoritesMenuOption

// iniから設定を読み込む
void CFavoritesMenuOption::GetProfile()
{


}

// iniへ設定を保存する
void CFavoritesMenuOption::WriteProfile()
{
}




////////////////////////////////////////////////////////////////////////////////
// CDonutFavoritesMenuPropertyPage


// Constructor
CDonutFavoritesMenuPropertyPage::CDonutFavoritesMenuPropertyPage()
	: m_bInit(false)
{
}


// Overrides
BOOL CDonutFavoritesMenuPropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_bInit == false) {
		m_bInit = true;

		DoDataExchange(DDX_LOAD);
	}

	return TRUE;
}


BOOL CDonutFavoritesMenuPropertyPage::OnKillActive()
{
	return TRUE;
}


BOOL CDonutFavoritesMenuPropertyPage::OnApply()
{
	if ( DoDataExchange(DDX_SAVE) ) {

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
void CDonutFavoritesMenuPropertyPage::OnLinkImportFromFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
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
void CDonutFavoritesMenuPropertyPage::OnLinkExportToFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
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

