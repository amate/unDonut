/**
 *	@file	LinkBarPropertyPage.h
 *	@brief	リンクバーのオプション設定.
 */

#include "stdafx.h"
#include "LinkBarPropertyPage.h"
#include "../IniFile.h"
#include "../ItemIDList.h"
#include "../DonutLinkBarCtrl.h"
#include "../MtlWeb.h"

//////////////////////////////////////////
// CLinkBarOption

// 定義
bool	CLinkBarOption::s_bShowIconOnly = false;
int		CLinkBarOption::s_nMaxTextWidth	= 100;
bool	CLinkBarOption::s_bNoShowDragImage = false;

enum ELinkBarOptionFlags {
	LBOF_SHOWICONONLY = 0x01,
	LBOF_NOSHOWDRAGIMAGE = 0x2,

	LBOF_DEFAULTVALUE = 0,
};

void CLinkBarOption::GetProfile()
{
	DWORD dwOptionFlags = LBOF_DEFAULTVALUE;
	CIniFileI	pr(g_szIniFileName, _T("LinkBar"));
	pr.QueryValue(dwOptionFlags, _T("OptionFlags"));
	s_bShowIconOnly		= (dwOptionFlags & LBOF_SHOWICONONLY) != 0;
	s_bNoShowDragImage	= (dwOptionFlags & LBOF_NOSHOWDRAGIMAGE) != 0;

	pr.QueryValue(s_nMaxTextWidth, _T("MaxTextWidth"));
}

void CLinkBarOption::WriteProfile()
{
	CIniFileO	pr(g_szIniFileName, _T("LinkBar"));
	DWORD dwOptionFlags = 0;
	if (s_bShowIconOnly)	dwOptionFlags |= LBOF_SHOWICONONLY;
	if (s_bNoShowDragImage)	dwOptionFlags |= LBOF_NOSHOWDRAGIMAGE;
	pr.SetValue(dwOptionFlags, _T("OptionFlags"));

	pr.SetValue(s_nMaxTextWidth, _T("MaxTextWidth"));
}


//////////////////////////////////////////////////////////
// CLinkBarPropertyPage

CLinkBarPropertyPage::CLinkBarPropertyPage(CDonutLinkBarCtrl& LinkBar) : 
	m_rLinkBarCtrl(LinkBar),
	m_bInit(false)
{
}


BOOL CLinkBarPropertyPage::OnSetActive()
{
	if (!m_bInit) {
		m_bInit = true;
		DoDataExchange(DDX_LOAD);
		CUpDownCtrl(GetDlgItem(IDC_SPIN_MAXTEXTWIDTH)).SetRange(10, 500);
	}
	return TRUE;
}


BOOL	CLinkBarPropertyPage::OnKillActive()
{
	return TRUE;
}


BOOL	CLinkBarPropertyPage::OnApply()
{
	if ( DoDataExchange(DDX_SAVE) ) {
		WriteProfile();
		m_rLinkBarCtrl.Refresh();
		return TRUE;
	} else
		return FALSE;
}

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

void CLinkBarPropertyPage::OnLinkImportFromFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (MessageBox(_T("インポートすると現在のリンクはすべて削除されますが、よろしいですか？"), _T("確認"), MB_YESNO) == IDNO)
		return ;
	CWaitCursor	waitCursor;
	CString folder = BrowseForFolder(m_hWnd);
	if (folder.GetLength() > 0) {
		m_rLinkBarCtrl.LinkImportFromFolder(folder);
	}
}

void CLinkBarPropertyPage::OnLinkExportToFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CWaitCursor	waitCursor;
	CString folder = BrowseForFolder(m_hWnd);
	if (folder.GetLength() > 0) {
		m_rLinkBarCtrl.LinkExportToFolder(folder);
	}
}







