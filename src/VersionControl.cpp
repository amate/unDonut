// VersionControl.cpp

#include "stdafx.h"
#include "VersionControl.h"
#include "IniFile.h"
#include "DonutPFunc.h"
#include "DonutOptions.h"


///////////////////////////////////////////////////////////////////
// CVersionControl

void	CVersionControl::Run()
{
	CIniFileIO	pr(g_szIniFileName, _T("Version"));

	// ファイルが存在しなければ各GetProfileに任せる
	if (::PathFileExists(g_szIniFileName)) {
		Misc::CopyToBackupFile(g_szIniFileName);

		DWORD dwVersion = pr.GetValue(_T("IniVersion"), 0);
		switch (dwVersion) {
		case 0:	_0to1();
		case 1: _1to2();
		case 2: _2to3();
		case 3: _3to4();
		case 4: _4to5();
			break;
		}
	}
	// 最新バージョンを書き込む
	pr.SetValue(LATESTVERSION, _T("IniVersion"));
	pr.Close();
}


void	CVersionControl::_0to1()
{
	{	// for CMenuOption
		enum { MAIN_EX2_NOCSTMMENU 			= 0x00000001L };

		/* iniから設定を読み込む */
		CIniFileIO	donutProfile(g_szIniFileName, _T("Main"));
		DWORD dwMainExStyle2	= donutProfile.GetValue(_T("Extended_Style2"), TRUE);
		DWORD dwNewMainExStyle2 = (dwMainExStyle2 & ~MAIN_EX2_NOCSTMMENU);
		donutProfile.SetValue(dwNewMainExStyle2, _T("Extended_Style2"));	// 設定から取り除く
		donutProfile.ChangeSectionName(_T("ETC"));
		DWORD dwIeMenuNoCstm	= donutProfile.GetValue(_T("IeMenuNoCstm"), TRUE);
		DWORD dwMenuBarStyle	= donutProfile.GetValue(_T("MenuBarStyle"), 0 );

		CString strMenuPath = _GetFilePath(_T("Menu.ini"));
		CIniFileIO	menuProfile(strMenuPath, _T("Option"));
		DWORD dwR_Equal_L		= menuProfile.GetValue(_T("REqualL"));
		DWORD dwDontShowButton	= menuProfile.GetValue(_T("NoButton"));
		menuProfile.RemoveFileToBak();	// Menu.iniはもう使わないので

		/* iniから設定を削除する */
		donutProfile.DeleteValue(_T("IeMenuNoCstm"));
		donutProfile.DeleteValue(_T("MenuBarStyle"));

		/* 新しい設定を書き込む */
		DWORD dwExStyle = 0;
		dwExStyle |= ((dwMainExStyle2 & MAIN_EX2_NOCSTMMENU) != 0) ? MENU_EX_NOCUSTOMMENU : 0;
		dwExStyle |= dwIeMenuNoCstm		? MENU_EX_NOCUSTOMIEMENU	: 0;
		dwExStyle |= dwR_Equal_L		? MENU_EX_R_EQUAL_L			: 0;
		dwExStyle |= dwDontShowButton	? MENU_EX_DONTSHOWBUTTON	: 0;

		donutProfile.ChangeSectionName(_T("Menu"));
		donutProfile.SetValue(dwExStyle			, _T("ExStyle"));
		donutProfile.SetValue(dwMenuBarStyle	, _T("MenuBarStyle"));
	}
}

void	CVersionControl::_1to2()
{
	enum {
		OLD_EMS_ADDITIONALMENUITEMNOSEP	= 0x00000002L,
		OLD_EMS_DEL_NOASK				= 0x00000010L,
		OLD_EMS_ORDER_FAVORITES 		= 0x00010000L,
		OLD_EMS_USER_DEFINED_FOLDER		= 0x00040000L,
	};
	CIniFileIO	pr(g_szIniFileName, _T("FavoritesMenu"));
	DWORD	dwStyle = pr.GetValue(_T("Style"));
	dwStyle &= ~OLD_EMS_ADDITIONALMENUITEMNOSEP;
	dwStyle &= ~OLD_EMS_DEL_NOASK;
	DWORD	dwNewStyle = dwStyle;
	if (dwStyle & OLD_EMS_ORDER_FAVORITES) {
		dwNewStyle &= ~OLD_EMS_ORDER_FAVORITES;
		dwNewStyle |= EMS_IE_ORDER;
	}
	if (dwStyle & OLD_EMS_USER_DEFINED_FOLDER) {
		dwNewStyle &= ~OLD_EMS_USER_DEFINED_FOLDER;
		dwNewStyle |= EMS_USER_DEFINED_FOLDER;
	}

	pr.SetValue(dwNewStyle, _T("Style"));
	
}

void	CVersionControl::_2to3()
{
	CIniFileIO	pr(g_szIniFileName, _T("SEARCH"));
	pr.DeleteValue(_T("Show_ToolBarIcon"));
}

void	CVersionControl::_3to4()
{
	CIniFileIO pr(g_szIniFileName, _T("Main"));
	pr.QueryValue(CMainOption::s_dwMainExtendedStyle, _T("Extended_Style"));
	CMainOption::s_dwMainExtendedStyle |= MAIN_EX_EXTERNALNEWTAB | MAIN_EX_EXTERNALNEWTABACTIVE;
	pr.SetValue(CMainOption::s_dwMainExtendedStyle, _T("Extended_Style"));
}

void	CVersionControl::_4to5()
{
	CIniFileIO	pr(g_szIniFileName, _T("LinkBar"));
	pr.DeleteValue(_T("ExtendedStyle"));
}



















