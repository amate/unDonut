/**
 *	@file	ExplorerBarDialog.cpp
 *	@brief	donutのオプション : エクスプローラバー
 */

#pragma once

#include "../resource.h"


enum ExplorerBarOption {
	EXPLORERBAROPTION_AUTOSHOW		= 0x00000001L,
};

class CExplorerBarOption
{
public:
	static DWORD	s_dwExplorerBarStyle;

	static void GetProfile();
	static void WriteProfile();
};

//////////////////////////////////////////////////
// CExplorerBarPropertyPage : エクスプローラーバーのオプション

class CExplorerBarPropertyPage
	: public CPropertyPageImpl<CExplorerBarPropertyPage>
	, public CWinDataExchange<CExplorerBarPropertyPage>
	, protected CExplorerBarOption
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_EXPLORERBAR };

	// DDX map
	BEGIN_DDX_MAP( CExplorerBarPropertyPage )
		DDX_CHECK( IDC_CHECK_EXP_AUTOSHOW, m_bExplorerBarAutoShow )

	END_DDX_MAP()

	// Constructor
	CExplorerBarPropertyPage(function<void (bool)>	func);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	// Message map and handlers
	BEGIN_MSG_MAP( CExplorerPropertyPage )
		CHAIN_MSG_MAP( CPropertyPageImpl<CExplorerBarPropertyPage> )
	END_MSG_MAP()

private:

	// Data members
	int		m_bExplorerBarAutoShow;
	function<void (bool)>	m_funcHookForAutoShow;
};
