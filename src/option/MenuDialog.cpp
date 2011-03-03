/**
 *	@file	MenuOption.cpp
 *	@brief	donutのオプション : メニュー
 */

#include "stdafx.h"
#include "../MtlCtrlw.h"		//+++
#include "MenuDialog.h"
#include "MainOption.h"
#include "RightClickMenuDialog.h"
#include "../IniFile.h"
#include "../DonutPFunc.h"
#include "../ToolTipManager.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace MTL;

////////////////////////////////////////////////////////////
// CMenuOption

bool	CMenuOption::s_bNoCustomMenu	= false;
bool	CMenuOption::s_bNoCustomIEMenu	= true;
bool	CMenuOption::s_bR_Equal_L		= false;
bool	CMenuOption::s_bDontShowButton	= false;
int		CMenuOption::s_nMenuBarStyle	= 0;

// iniから設定を読み込む
void	CMenuOption::GetProfile()
{
	{
		CIniFileI	pr(g_szIniFileName, _T("Menu"));
		DWORD dwExStyle = pr.GetValue(_T("ExStyle"), 0);
		s_bNoCustomMenu		= (dwExStyle & MENU_EX_NOCUSTOMMENU		) != 0;
		s_bNoCustomIEMenu	= (dwExStyle & MENU_EX_NOCUSTOMIEMENU	) != 0;
		s_bR_Equal_L		= (dwExStyle & MENU_EX_R_EQUAL_L		) != 0;
		s_bDontShowButton	= (dwExStyle & MENU_EX_DONTSHOWBUTTON	) != 0;

		s_nMenuBarStyle	= pr.GetValue(_T("MenuBarStyle"));
	}
}

// iniへ設定を保存する
void	CMenuOption::WriteProfile()
{
	{
		DWORD	dwExStyle = 0;
		dwExStyle |= s_bNoCustomMenu	? MENU_EX_NOCUSTOMMENU	 : 0;
		dwExStyle |= s_bNoCustomIEMenu	? MENU_EX_NOCUSTOMIEMENU : 0;
		dwExStyle |= s_bR_Equal_L		? MENU_EX_R_EQUAL_L		 : 0;
		dwExStyle |= s_bDontShowButton	? MENU_EX_DONTSHOWBUTTON : 0;

		CIniFileO	pr(g_szIniFileName, _T("Menu"));
		pr.SetValue(dwExStyle		, _T("ExStyle"));
		pr.SetValue(s_nMenuBarStyle	, _T("MenuBarStyle"));
		
	}
}




////////////////////////////////////////////////////////////
// CMenuPropertyPage

// Constructor
CMenuPropertyPage::CMenuPropertyPage(HMENU hMenu, CCommandBarCtrl2& rCmdBar)
	: m_rCmdBar(rCmdBar)
	, m_hMenu(hMenu)
	, m_nNoCstmMenu(0)
	, m_nNoCstmIeMenu(0)
	, m_nREqualL(0)
	, m_nNoButton(0)
	, m_nMenuBarStyle(0)
	, m_bInit(false)
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
		m_nNoCstmMenu	= CMenuOption::s_bNoCustomMenu;
		m_nNoCstmIeMenu = CMenuOption::s_bNoCustomIEMenu;
		m_nREqualL		= CMenuOption::s_bR_Equal_L;
		m_nNoButton		= CMenuOption::s_bDontShowButton;

		m_nMenuBarStyle	= CMenuOption::s_nMenuBarStyle;
	}

	return DoDataExchange(FALSE);
}



BOOL CMenuPropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CMenuPropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_SaveData();
		return TRUE;
	} else {
		return FALSE;
	}
}



// データを保存
void CMenuPropertyPage::_SaveData()
{
	{
		CMenuOption::s_bNoCustomMenu	= m_nNoCstmMenu		!= 0;
		CMenuOption::s_bNoCustomIEMenu	= m_nNoCstmIeMenu	!= 0;
		CMenuOption::s_bR_Equal_L		= m_nREqualL		!= 0;
		CMenuOption::s_bDontShowButton	= m_nNoButton		!= 0;
		CMenuOption::s_nMenuBarStyle	= m_nMenuBarStyle;

		CMenuOption::WriteProfile();
	}

	m_rCmdBar.setMenuBarStyle();		//+++ MenuBarStyleの反映
}


