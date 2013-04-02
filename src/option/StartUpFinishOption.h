/**
 *	@file	StartUpFinishOption.h
 *	@brief	unDonutのオプション : 起動/終了処理
 */
#pragma once

#include "../resource.h"

// 前方宣言
class CMainFrame;

//////////////////////////////////////////////////////////
// CStartUpOption

class CStartUpOption
{
	friend class CStartUpPropertyPage;
public:
	// Constants
	enum EStartup_Flag {
		STARTUP_NOINITWIN		=	0x00000000L,
		STARTUP_GOHOME			=	0x00000001L,
		STARTUP_LATEST			=	0x00000002L,
		STARTUP_DFG 			=	0x00000004L,
	};

	static DWORD	s_dwFlags;
	static bool		s_bActivateOnExternalOpen;
	static CString	s_szDfgPath;

	// Implementation
	static void 	GetProfile();
	static void 	WriteProfile();

	static void 	StartUp(CMainFrame& __frame);
	static void		EndFinish();

};


///////////////////////////////////////////////////////////////////
// CStartUpFinishPropertyPage

class CStartUpFinishPropertyPage : 
	public CPropertyPageImpl<CStartUpFinishPropertyPage>,
	public CWinDataExchange<CStartUpFinishPropertyPage>,
	protected CStartUpOption
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_STARTUP_FINISH };

	// Constructor
	CStartUpFinishPropertyPage();

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	// DDX map
	BEGIN_DDX_MAP( CStartUpFinishPropertyPage )
		// 起動処理
		DDX_RADIO( IDC_RADIO_NOINITWIN			, m_nRadio			)
		DDX_CHECK( IDC_CHECK_STARTUP_ACTIVATE	, s_bActivateOnExternalOpen	)
		DDX_TEXT( IDC_EDIT_DFG, s_szDfgPath )

		// 終了処理
		DDX_CHECK( IDC_CHECK_MAIN_DEL_CASH		, m_nDelCash		)
		DDX_CHECK( IDC_CHECK_MAIN_DEL_COOKIE	, m_nDelCookie		)
		DDX_CHECK( IDC_CHECK_MAIN_DEL_HISTORY	, m_nDelHistory 	)
		DDX_CHECK( IDC_CHECK_MAIN_MAKECASH		, m_nMakeCash		)
		DDX_CHECK( IDC_CHECK_MAIN_DEL_RECENTCLOSE, m_nDelRecentClose )
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CStartUpFinishPropertyPage )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_EXPDFG, OnButton )
		CHAIN_MSG_MAP( CPropertyPageImpl<CStartUpFinishPropertyPage> )
	END_MSG_MAP()


	void OnButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

private:

	// Data members
	bool	m_bInit;
	int 	m_nRadio;

	int 	m_nDelCash;
	int 	m_nDelCookie;
	int 	m_nDelHistory;
	int 	m_nMakeCash;
	int 	m_nDelRecentClose;

};



