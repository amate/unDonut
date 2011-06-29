/**
 *	@file	AddressBarPropertyPage.h
 *	@brief	アドレスバーのオプション設定.
 *	@note
 *		 +++ AddressBar.hより分離
 */
#pragma once

#include "../resource.h"

enum EAbr_Ex {
	ABR_EX_AUTOCOMPLETE 	= 0x00000001L,
	ABR_EX_LOADTYPEDURLS	= 0x00000002L,
	ABR_EX_GOBTNVISIBLE 	= 0x00000004L,
	//UH
	ABR_EX_TEXTVISIBLE		= 0x00000008L,
	ABR_EX_ENTER_CTRL		= 0x00000010L,
	ABR_EX_ENTER_SHIFT		= 0x00000020L,
	//minit
	ABR_EX_SEARCH_REPLACE	= 0x00000040L,
	ABR_EX_USER 			= 0x00010000L,
	ABR_PANE_STYLE			=
		 WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,

	// for Donut
	ABR_EX_OPENNEWWIN		= 0x00010000L,
	ABR_EX_NOACTIVATE		= 0x00020000L,

	ABR_EX_DEFAULT_BITS	= 
		ABR_EX_LOADTYPEDURLS 
	  | ABR_EX_TEXTVISIBLE 
	  | ABR_EX_ENTER_CTRL 
	  | ABR_EX_ENTER_SHIFT 
	  | ABR_EX_SEARCH_REPLACE,
};

///////////////////////////////////////////////////////////////
// CAddressBarOption

struct CAddressBarOption
{
	static bool	s_bNewWindow;
	static bool	s_bAutoComplete;
	static bool s_bGoBtnVisible;
	static bool s_bNoActivate;
	static bool s_bLoadTypedUrls;
	static bool	s_bTextVisible;

	static bool s_bUseEnterCtrl;
	static CString s_strEnterCtrlEngine;
	static bool s_bUseEnterShift;
	static CString s_strEnterShiftEngine;
	static bool s_bReplaceSpace;

	static CString s_strIeExePath;

	static void GetProfile();
	static void SaveProfile();
};


class CDonutAddressBar;
class CDonutSearchBar;

class CDonutAddressBarPropertyPage
		: public CPropertyPageImpl < CDonutAddressBarPropertyPage >
		, public CWinDataExchange < CDonutAddressBarPropertyPage >
		, protected CAddressBarOption
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_ADDRESSBAR };

	// Constructor
	CDonutAddressBarPropertyPage(CDonutAddressBar &adBar, CDonutSearchBar &searchBar);

	// Overrides
	BOOL OnSetActive();
	BOOL OnKillActive();
	BOOL OnApply();


	// DDX map
	BEGIN_DDX_MAP( CDonutAddressBarPropertyPage )
		DDX_CHECK( IDC_CHECK_ABR_NEWWIN,		s_bNewWindow	)
		DDX_CHECK( IDC_CHECK_ABR_AUTOCOMPLETE,	s_bAutoComplete )
		DDX_CHECK( IDC_CHECK_ABR_GOBTNVISIBLE,	s_bGoBtnVisible )
		DDX_CHECK( IDC_CHECK_ADB_NOACTIVATE,	s_bNoActivate	)
		DDX_CHECK( IDC_CHECK_ABR_LOADTYPEDURLS, s_bLoadTypedUrls)
		DDX_CHECK( IDC_CHECK_ABR_TEXTVISIBLE,	s_bTextVisible	)
		// UH
		DDX_CHECK( IDC_CHECK_CTRL_ENTER,		s_bUseEnterCtrl )
		DDX_CHECK( IDC_CHECK_SHIFT_ENTER,		s_bUseEnterShift)
		DDX_CHECK( IDC_CHECK_REPLACE,			s_bReplaceSpace )

		DDX_CONTROL_HANDLE( IDC_ADDRESS_BAR_ICON_EXE, m_edit )
	END_DDX_MAP()

	// Message map
	BEGIN_MSG_MAP( CDonutAddressBarPropertyPage )
		COMMAND_ID_HANDLER_EX( IDC_BTN_ADDRESS_BAR_EXE, OnButton )
		CHAIN_MSG_MAP(CPropertyPageImpl<CDonutAddressBarPropertyPage>)
	END_MSG_MAP()


	// [...]
	void 	OnButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	
private:
	void	_InitComboBox();

	// Data members
	CDonutAddressBar&	m_AddressBar;
	CDonutSearchBar &	m_SearchBar;	//minit

	//minit
	BOOL				m_bInit;

	//+++
	CString 			m_strIeExePath;
	CEdit				m_edit;				//+++
};

