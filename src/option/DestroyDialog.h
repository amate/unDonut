/**
 *	@file	DestroyDialog.h
 *	@brief	donutのオプション: 終了時の処理.
 */

#pragma once

#include "../resource.h"



class CDestroyPropertyPage
	: public CPropertyPageImpl<CDestroyPropertyPage>
	, public CWinDataExchange<CDestroyPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_DESTROY };

private:
	int 	m_nDelCash;
	int 	m_nDelCookie;
	int 	m_nDelHistory;
	int 	m_nMakeCash;
	int 	m_nDelRecentClose;

public:
	// ctor
	CDestroyPropertyPage();

	// DDX map
	//x BEGIN_DDX_MAP( CMainPropertyPage2 )
	BEGIN_DDX_MAP( CDestroyPropertyPage )
		DDX_CHECK( IDC_CHECK_MAIN_DEL_CASH		, m_nDelCash		)
		DDX_CHECK( IDC_CHECK_MAIN_DEL_COOKIE	, m_nDelCookie		)
		DDX_CHECK( IDC_CHECK_MAIN_DEL_HISTORY	, m_nDelHistory 	)
		DDX_CHECK( IDC_CHECK_MAIN_MAKECASH		, m_nMakeCash		)

		DDX_CHECK( IDC_CHECK_MAIN_DEL_RECENTCLOSE,m_nDelRecentClose )
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CDestroyPropertyPage )
		CHAIN_MSG_MAP( CPropertyPageImpl<CDestroyPropertyPage> )
	END_MSG_MAP()


	// Overrides
	BOOL OnSetActive();
	BOOL OnKillActive();
	BOOL OnApply();

private:
	void _GetData();
	void _SetData();
};
