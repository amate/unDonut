/**
 *	@file	MouseDialog.h
 *	@brief	donutのオプション : マウス
 */

#pragma once

#include "../resource.h"


///////////////////////////////////////////////
// CMouseOption

class CMouseOption 
{
public:
	static bool	s_bUseRightDragSearch;
	static bool	s_bUseRect;
	static CString s_strTEngine;
	static CString s_strLEngine;
	static CString s_strREngine;
	static CString s_strBEngine;
	static CString s_strCEngine;
	static int	s_nDragDropCommandID;

	static void	GetProfile();
	static void	WriteProfile();
};


///////////////////////////////////////////////
// CMousePropertyPage

class CMousePropertyPage
	: public CPropertyPageImpl<CMousePropertyPage>
	, public CWinDataExchange<CMousePropertyPage>
	, protected CMouseOption
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MOUSE };

	// Constructor
	CMousePropertyPage(HMENU hMenu, HMENU hSearchEngineMenu);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();


	// DDX map
	BEGIN_DDX_MAP( CMousePropertyPage )
		DDX_CONTROL_HANDLE(IDC_COMBO_TSEARCH, m_cmbT)
		DDX_CONTROL_HANDLE(IDC_COMBO_LSEARCH, m_cmbL)
		DDX_CONTROL_HANDLE(IDC_COMBO_RSEARCH, m_cmbR)
		DDX_CONTROL_HANDLE(IDC_COMBO_BSEARCH, m_cmbB)
		DDX_CONTROL_HANDLE(IDC_COMBO_CSEARCH, m_cmbC)
		DDX_CONTROL_HANDLE(IDC_CHECK_RIGHT_DRAGDROP, m_btnUseRightDrag)
		DDX_CONTROL_HANDLE(IDC_CHECK_USERECT	   , m_btnUseRect)
		DDX_CHECK(IDC_CHECK_RIGHT_DRAGDROP, s_bUseRightDragSearch)
		DDX_CHECK(IDC_CHECK_USERECT		  , s_bUseRect)
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CMousePropertyPage )
		CHAIN_MSG_MAP( CPropertyPageImpl<CMousePropertyPage> )

		COMMAND_HANDLER_EX( IDC_CMB_LINK_M, CBN_SELCHANGE, OnSelChange )

		COMMAND_ID_HANDLER_EX( IDC_SETBTN_RL		, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_RM		, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_RX1		, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_RX2		, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_RW_UP 	, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_RW_DOWN	, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_SIDE1 	, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_SIDE2 	, OnSetBtn )

		COMMAND_ID_HANDLER( IDC_STC_RL			, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_RM			, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_RX1 		, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_RX2 		, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_RW_UP		, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_RW_DOWN 	, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_SIDE1		, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_SIDE2		, OnSetStatic )

		COMMAND_ID_HANDLER_EX(IDC_CHECK_RIGHT_DRAGDROP, OnCommandSwitch )
		COMMAND_ID_HANDLER_EX(IDC_CHECK_USERECT		  , OnCommandSwitch )
	END_MSG_MAP()

private:
	LRESULT OnSetStatic(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled); // UDT DGSTR
	void	OnCommandSwitch(UINT uNotifyCode, int nID, CWindow wndCtl);

	void	OnSetBtn(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/);
	void	OnSelChange(UINT code, int id, HWND hWnd);

	CString GetTarString(int id);
	DWORD	GetTarStaticID(int id);


private:
	void	OnInitSetting();
	void	OnInitCmb();

	// データを得る
	void	_SetData();

	// データを保存
	void	_GetData();

	void	SetCmdString();
	void	AddMenuItem(CMenu &menu, CComboBox &cmb);
	void	AddDragDropItem(CComboBox &cmb);

	void	_InitRightDragComboBox();
	void	_SetRightDragProfile();
	void	_InitRightDragOption();
	void	_SwitchCommand(int nID);

	// Data members
	CComboBox					m_cmbLinkM;
	CComboBox					m_cmbDragDrop;
	CSimpleMap<CString, DWORD>	m_mapMouseCmd;
	HMENU						m_hMenu;
	CMenu						m_menuSearchEngine;

	CString 					m_strPath;

	CComboBox					m_cmbT;
	CComboBox					m_cmbL;
	CComboBox					m_cmbR;
	CComboBox					m_cmbB;
	CComboBox					m_cmbC;
	CButton						m_btnUseRightDrag;
	CButton						m_btnUseRect;
};

