/**
 *	@file	ProxyDialog.h
 *	@brief	donutのオプション : プロキシ
 */
#pragma once

#include "../resource.h"

class CProxyPropertyPage
	: public CPropertyPageImpl<CProxyPropertyPage>
	, public CWinDataExchange<CProxyPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_PROXY };

private:
	// Data members
	CEdit	m_editPrx;
	CEdit	m_editNoPrx;
	int 	m_nRandChk; 			// U.H
	int 	m_nRandTimeMin; 		// U.H
	int 	m_nRandTimeSec; 		// U.H
	int 	m_nLocalChk;
	int 	m_nUseIE;

public:
	// DDX map
	BEGIN_DDX_MAP(CProxyPropertyPage)
		// UDT DGSTR ( U.H
		DDX_CHECK( IDC_RAND_CHK , m_nRandChk	)
		DDX_INT_RANGE( IDC_EDIT_RAND_MIN, m_nRandTimeMin, 0, 59)
		DDX_INT_RANGE( IDC_EDIT_RAND_SEC, m_nRandTimeSec, 0, 59)
		DDX_CHECK( IDC_LOCAL_CHK, m_nLocalChk	)
		DDX_CHECK( IDC_IE_CHK	, m_nUseIE		)
		// ENDE
	END_DDX_MAP()

	// Constructor
	CProxyPropertyPage();

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	// Message map and handlers
	BEGIN_MSG_MAP( CProxyPropertyPage )
		CHAIN_MSG_MAP( CPropertyPageImpl<CProxyPropertyPage> )
	END_MSG_MAP()

	// Implementation
private:
	void	_GetData();
	void	_SetData();
};
