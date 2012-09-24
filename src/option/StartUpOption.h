/**
 *	@file	StartUpOption.h
 *	@brief	donutのオプション : スタートアップ
 */
#pragma once

#include "../resource.h"


class CStartUpOption {
	friend class CStartUpPropertyPage;

	enum {
		STARTUP_ACTIVATE		=	0x00000001L,				// UDT DGSTR ( dai
		STARTUP_WITH_PARAM		=	0x00000001L,				// UDT DGSTR ( dai
	};


	static TCHAR	s_szDfgPath[MAX_PATH];						//+++ メモ:あとで CString化したほうが無難かも?

public:
	//+++ STARTUP_LATEST が CChildFrame で参照されるのでpublic
	enum EStartup_Flag {
		STARTUP_NOINITWIN		=	0x00000000L,
		STARTUP_GOHOME			=	0x00000001L,
		STARTUP_LATEST			=	0x00000002L,
		STARTUP_DFG 			=	0x00000004L,
	};

	static DWORD	s_dwFlags;
	static DWORD	s_dwActivate;								// UDT DGSTR ( dai
	static DWORD	s_dwParam;									// UDT DGSTR ( dai

public:
	static void 	GetProfile();
	static void 	WriteProfile();

	template <class _MainFrame>
	static void 	StartUp(_MainFrame &__frame);

	static CString	GetDefaultDFGFilePath();

	static BOOL 	Isflag_Latest();							//minit 変な名前、後で直そう
};




class CStartUpPropertyPage
	: public CPropertyPageImpl<CStartUpPropertyPage>
	, public CWinDataExchange<CStartUpPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_STARTUP };

private:
	// Data members
	int 		m_nRadio;
	CString 	m_strDfg;
	CEdit		m_edit;
	int 		m_nCheckActivate;									// UDT DGSTR ( dai
	int 		m_nCheckParam;										// UDT DGSTR ( dai
	int 		m_nCheckSaveFB; 									//minit

public:
	// DDX map
	BEGIN_DDX_MAP( CStartUpPropertyPage )
		DDX_RADIO( IDC_RADIO_NOINITWIN			, m_nRadio			)
		DDX_CHECK( IDC_CHECK_STARTUP_WITH_PARAM , m_nCheckParam 	)		// UDT DGSTR ( dai
		DDX_CHECK( IDC_CHECK_STARTUP_ACTIVATE	, m_nCheckActivate	)		// UDT DGSTR ( dai
		DDX_CHECK( IDC_CHECK_STARTUP_SAVE_FB	, m_nCheckSaveFB	)		//minit
		DDX_TEXT_LEN( IDC_EDIT_DFG, m_strDfg, MAX_PATH )
	END_DDX_MAP()

	// Constructor
	CStartUpPropertyPage();

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	// Message map and handlers
	BEGIN_MSG_MAP(CStartUpPropertyPage)
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_EXPDFG, OnButton )
		CHAIN_MSG_MAP( CPropertyPageImpl<CStartUpPropertyPage> )
	END_MSG_MAP()

private:
	void OnButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// Implementation
	void	_GetData();
	void	_SetData();
};




//テンプレートメンバ関数の定義
template <class _MainFrame>
void CStartUpOption::StartUp(_MainFrame &__frame)
{
	HWND	hWndChild = NULL;

	switch (s_dwFlags) {
	case STARTUP_NOINITWIN:
		::PostMessage(__frame.GetHWND(), WM_INITPROCESSFINISHED, 0, 0);
		break;

	case STARTUP_GOHOME:
		::PostMessage(__frame.GetHWND(), WM_COMMAND, ID_FILE_NEW_HOME, 0);
		::PostMessage(__frame.GetHWND(), WM_INITPROCESSFINISHED, 0, 0);
		break;

	case STARTUP_LATEST:
		__frame.RestoreAllTab();
		break;

	//case STARTUP_DFG:
	//	{
	//		CString 	strPath(s_szDfgPath);
	//		if ( !strPath.IsEmpty() )
	//			__frame.UserOpenFile(s_szDfgPath, 0);
	//	}
	//	__frame.PostMessage(WM_INITPROCESSFINISHED);
	//	break;

	default:
		ATLASSERT(FALSE);
		break;
	}
}
