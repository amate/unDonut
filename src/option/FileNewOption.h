/**
 *	@file	FileNewOption.h
 *	@brief	donut のオプション : "新規作成"
 */

#pragma once

#include "../resource.h"


enum EFileNew {
	FILENEW_BLANK	= 0x00000001L,
	FILENEW_COPY	= 0x00000002L,
	FILENEW_HOME	= 0x00000004L,
	FILENEW_USER	= 0x00000008L,		//+++
};


class CFileNewOption {
	//+++ 解析向けにs_dwFlagsのスコープ範囲を狭める
	friend class CFileNewPropertyPage;
	friend class CMainFrame;

	static DWORD	s_dwFlags;
	static CString	s_strUsr;

public:
	static void 	GetProfile();
	static void 	WriteProfile();
};



class CFileNewPropertyPage
	: public CPropertyPageImpl<CFileNewPropertyPage>
	, public CWinDataExchange<CFileNewPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_FILENEW };

	// DDX map
	BEGIN_DDX_MAP(CFileNewPropertyPage)
		DDX_RADIO( IDC_RADIO_BLANK	, m_nRadio )
		DDX_TEXT_LEN( IDC_EDIT_NEWPAGE_USER, CFileNewOption::s_strUsr, MAX_PATH )
	END_DDX_MAP()

	// Constructor
	CFileNewPropertyPage();

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();


	// Message map and handlers
	BEGIN_MSG_MAP( CFileNewPropertyPage )
		COMMAND_ID_HANDLER_EX( IDC_BTN_NEWPAGE_USER, OnButton )	//+++
		CHAIN_MSG_MAP(CPropertyPageImpl<CFileNewPropertyPage>)
	END_MSG_MAP()

	// Implementation
private:
	void	_GetData();
	void	_SetData();
	void 	OnButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);	//+++

private:
	// Data members
	int 	m_nRadio;

	//CString m_strUsr;		//+++
	CEdit	m_editUsr;		//+++
};

