/**
 *	@file	MenuOption.h
 *	@brief	donutのオプション : メニュー
 */

#pragma once

#include "../resource.h"

namespace MTL { class CCommandBarCtrl2; }

enum EMenuEx {
	MENU_EX_NOCUSTOMMENU	= 0x00000001L,	// メニューの文字を編集しない
	MENU_EX_NOCUSTOMIEMENU	= 0x00000002L,	// 右クリックメニューをカスタムしない
	MENU_EX_R_EQUAL_L		= 0x00000004L,	// 右クリックメニュー上の右クリックは...
	MENU_EX_DONTSHOWBUTTON	= 0x00000008L,	// メニューに最小化etc..ボタンを表示しない
};

////////////////////////////////////////////////////////////
// CMenuOption

class CMenuOption
{
public:
	static bool		s_bNoCustomMenu;	// メニューの文字を編集しない
	static bool		s_bNoCustomIEMenu;	// 右クリックメニューをカスタムしない
	static bool		s_bR_Equal_L;		// 右クリックメニュー上の右クリックは...
	static bool		s_bDontShowButton;	// メニューに最小化etc..ボタンを表示しない
	static int		s_nMenuBarStyle;	// メニューバーの項目の表記(0:日本語 1:1文字 2:英語)

public:
	static void		GetProfile();
	static void		WriteProfile();
};




////////////////////////////////////////////////////////////
// CMenuPropertyPage : [Donutのオプション] - [メニュー]

class CMenuPropertyPage
	: public CPropertyPageImpl<CMenuPropertyPage>
	, public CWinDataExchange<CMenuPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MENU };

private:
	// Data members
	HMENU					m_hMenu;

	int 					m_nNoCstmMenu;
	int 					m_nNoCstmIeMenu;
	int 					m_nREqualL;
	int 					m_nNoButton;

	int 					m_nMenuBarStyle;		//+++

	bool					m_bInit;

public:
	// Constructor/Destructor
	CMenuPropertyPage(HMENU hMenu);
	~CMenuPropertyPage();

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

private:
	// データを保存
	void	_SaveData();

public:

	// DDX map
	BEGIN_DDX_MAP( CMenuPropertyPage )
		DDX_CHECK( IDC_NO_CSTM_TXT_MENU 	, m_nNoCstmMenu )
		DDX_CHECK( IDC_NO_CSTM_IE_MENU		, m_nNoCstmIeMenu)
		DDX_CHECK( IDC_CHECK_R_EQUAL_L		, m_nREqualL	)
		DDX_CHECK( IDC_CHECK_NOBUTTON		, m_nNoButton	)
		DDX_RADIO( IDC_MENU_BAR_STYLE_JAPAN , m_nMenuBarStyle)
	END_DDX_MAP()


	// Message map and handlers
	BEGIN_MSG_MAP( CMenuPropertyPage )
		CHAIN_MSG_MAP( CPropertyPageImpl<CMenuPropertyPage> )
	END_MSG_MAP()
};

